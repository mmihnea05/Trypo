#pragma once
#define WIN32_LEAN_AND_MEAN
#define _HAS_STD_BYTE 0
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
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
                Logger::getInstanceLogger().setMessage(std::string("[CONEXIUNE] Client nou: ") + std::string(clientIP));
                Logger::getInstanceLogger().printMessageOnFile();

                const char* msg = "Salut din Serverul C++!";
                send(clientSocket, msg, (int)strlen(msg), 0);

                closesocket(clientSocket);
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
    void loginUser(string mail, string password);
    void printUsers();
    void printRentals();
    void loadAllRentals();
    RentalUnit* findRentalById(int id);
    void populateRoomsOfRental(int id);
    IUsers* findConnectedUsers(int id);
};

