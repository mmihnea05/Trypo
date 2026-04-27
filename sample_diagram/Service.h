#pragma once
#define WIN32_LEAN_AND_MEAN
#define _HAS_STD_BYTE 0
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <sstream>
#include <iostream>
#include <vector>
#include <thread>
#include <nanodbc/nanodbc.h>
#include "IUsers.h"
#include "Accomodation.h"
#include "Logger.h"
#include "Admin.h"
#include "Client.h"
#include "RentalUnit.h"
#include "Promotions.h"
#include "Single.h"
#include "Double.h"
#include "Triple.h"
#include "Extras.h"
#include "Facilities.h"
#include "MyException.h"
#include "Accomodation.h"
#include "json.hpp" // nlohmann/json
using json = nlohmann::json;
#pragma comment(lib, "ws2_32.lib")


class Service{
private:
	nanodbc::connection conn;
    SOCKET serverSocket;
    bool serverRunning;
    std::thread workerThread;
	std::vector<IUsers*> users;
	std::vector<Accomodation*> accom;
    std::vector<RentalUnit*> rentals;
public:

    nanodbc::connection& getConn() {
        return conn;
    }

	Service(string connection_string): serverRunning(false), serverSocket(INVALID_SOCKET) {
        try {
            conn = nanodbc::connection(connection_string);
            if (conn.connected()) {
                cout << "Conexiune reusita la BD.";
                Logger::getInstanceLogger().setMessage("Conexiune reusita la BD.");
                Logger::getInstanceLogger().printMessageOnFile();
            }
            WSADATA wsaData;
            if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
                throw MyException("Eroare la WSAStartup");
            }

            workerThread = std::thread(&Service::startServer, this);
            workerThread.detach();
        }
        catch (const MyException& e) {
            Logger::getInstanceLogger().setMessage("Eroare la conectare: " + std::string(e.what()));
            Logger::getInstanceLogger().printMessageOnFile();
        }
    }

    void handleClient(SOCKET clientSocket, std::string clientIP) {
        char buffer[4096];
        std::string emailLogat = "";

        while (serverRunning) {
            int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
            if (bytesReceived <= 0) break;

            buffer[bytesReceived] = '\0';
            std::string request(buffer);

            try {
                auto j = json::parse(request);


                if (j.contains("type") && j["type"] == "REGISTER_USER") {
                    IUsers* newUser = new Client(
                        j["name"].get<std::string>(), j["password"].get<std::string>(),
                        j["email"].get<std::string>(), j["phone"].get<std::string>(),
                        j["dob"].get<std::string>(), j["country"].get<std::string>(),
                        j["gender"].get<std::string>(), j["address"].get<std::string>()
                    );

                    json res;
                    res["type"] = "REGISTER_RESPONSE";
                    if (this->searchUser(newUser)) {
                        res["status"] = "error";
                        res["message"] = "Acest email este deja utilizat!";
                    }
                    else {
                        this->registerUser(newUser);
                        res["status"] = "success";
                        res["message"] = "Cont creat cu succes!";
                    }
                    send(clientSocket, res.dump().c_str(), (int)res.dump().length(), 0);
                    delete newUser;
                }

                else if (j.contains("type") && j["type"] == "LOGIN_USER") {
                    std::string email = j["email"].get<std::string>();
                    std::string password = j["password"].get<std::string>();

                    json res;
                    res["type"] = "LOGIN_RESPONSE";

                    bool isAlreadyConnected = false;
                    for (auto u : this->users) {
                        if (u->getMail() == email) {
                            isAlreadyConnected = true;
                            break;
                        }
                    }

                    if (isAlreadyConnected) {
                        res["status"] = "error";
                        res["message"] = "Utilizatorul este deja conectat";
                    }
                    else {
                        IUsers* user = this->loginUser(email, password);
                        if (user != nullptr) {
                            emailLogat = email;

                            res["status"] = "success";
                            json userData;
                            userData["name"] = user->getName();
                            userData["email"] = user->getMail();
                            userData["phone"] = user->getPhone();

                            Client* c = dynamic_cast<Client*>(user);
                            if (c) {
                                userData["role"] = 0;
                                userData["dob"] = c->getBirthDate();
                                userData["country"] = c->getCountry();
                                userData["gender"] = c->getGender();
                                userData["address"] = c->getAddress();
                            }
                            else {
                                userData["role"] = 1;
                            }
                            res["data"] = userData;
                            cout << res;
                        }
                        else {
                            res["status"] = "error";
                            res["message"] = "Date incorecte.";
                        }
                    }
                    send(clientSocket, res.dump().c_str(), (int)res.dump().length(), 0);
                }
            }
            catch (const std::exception& e) {
                std::cout << "Eroare: " << e.what() << std::endl;
            }
        }

        if (!emailLogat.empty()) {


            for (auto it = users.begin(); it != users.end(); ++it) {
                if ((*it)->getMail() == emailLogat) {
                    Logger::getInstanceLogger().setMessage("[CLEANUP] Client deconectat." + emailLogat);
                    Logger::getInstanceLogger().printMessageOnFile();
                    delete* it;
                    users.erase(it); 
                    break;
                }
            }
        }

        closesocket(clientSocket);
    }

    void startServer() {
        serverSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (serverSocket == INVALID_SOCKET) {
            Logger::getInstanceLogger().setMessage("Eroare la crearea socket-ului.");
            Logger::getInstanceLogger().printMessageOnFile();
            return;
        }

        int opt = 1;
        setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));

        sockaddr_in serverAddr;
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_addr.s_addr = INADDR_ANY;
        serverAddr.sin_port = htons(12345);

        if (::bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
            Logger::getInstanceLogger().setMessage("Eroare la Bind. Cod: " +WSAGetLastError());
            Logger::getInstanceLogger().printMessageOnFile();
            closesocket(serverSocket);
            return;
        }

        if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
            Logger::getInstanceLogger().setMessage("Eroare la Listen.");
            Logger::getInstanceLogger().printMessageOnFile();
            closesocket(serverSocket);
            return;
        }

        serverRunning = true;
        Logger::getInstanceLogger().setMessage("Serverul C++ asculta pe portul 12345...");
        Logger::getInstanceLogger().printMessageOnFile();

        while (serverRunning) {
            sockaddr_in clientAddr;
            int clientSize = sizeof(clientAddr);

            SOCKET clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientSize);

            if (clientSocket != INVALID_SOCKET && serverRunning) {
                char clientIP[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, &clientAddr.sin_addr, clientIP, INET_ADDRSTRLEN);
                cout << "Client connectat la server";
                Logger::getInstanceLogger().setMessage(std::string("[CONEXIUNE] Client nou: ") + std::string(clientIP));
                Logger::getInstanceLogger().printMessageOnFile();
                std::thread t(&Service::handleClient, this, clientSocket, std::string(clientIP));
                t.detach();
            }
        }
    }

    void stopServer() {
        serverRunning = false;
        closesocket(serverSocket);
        WSACleanup();
    }
    ~Service() {
        for (auto a : accom) {
            delete a;
        }
        accom.clear();
        for (auto u : users) {
            delete u;
        }
        stopServer();
    }
    void registerUser(IUsers* newUser);
    bool searchUser(IUsers* newUser);
    IUsers* loginUser(string mail, string password);
    void printUsers();
    void printRentals();
    void loadAllRentals();
    RentalUnit* findRentalById(int id);
    void populateRoomsOfRental(int id);

};

