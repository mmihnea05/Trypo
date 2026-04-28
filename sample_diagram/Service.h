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
#include "json.hpp"
#include <mutex>
using json = nlohmann::json;
#pragma comment(lib, "ws2_32.lib")


class Service {
private:
	nanodbc::connection conn;
	SOCKET serverSocket;
	bool serverRunning;
	std::thread workerThread;
	std::vector<IUsers*> users;
	std::vector<Accomodation*> accom;
	std::vector<RentalUnit*> rentals;
	std::mutex mtx;
public:

	nanodbc::connection& getConn() {
		return conn;
	}

	Service(string connection_string) : serverRunning(false), serverSocket(INVALID_SOCKET) {
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
			if (bytesReceived <= 0) {
				if (bytesReceived == 0) {
					Logger::getInstanceLogger().setMessage("[INFO] Clientul a inchis conexiunea ordonat.");
					Logger::getInstanceLogger().printMessageOnFile();
				}
				else {
					Logger::getInstanceLogger().setMessage("[ERROR] Eroare de retea/Socket Error: ");
					Logger::getInstanceLogger().printMessageOnFile();
				}
				break;
			}

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
							std::lock_guard<std::mutex> lock(mtx);
							this->users.push_back(user);
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
						}
						else {
							res["status"] = "error";
							res["message"] = "Date incorecte.";
						}
					}
					send(clientSocket, res.dump().c_str(), (int)res.dump().length(), 0);
				}

				else if (j["type"] == "GET_ACCOMMODATIONS") {
					json response;
					response["type"] = "GET_ACCOMMODATIONS";
					json dataArray = json::array();

					this->loadAllRentals();
					for (auto* acc : this->rentals) {
						json accJson;
						accJson["id"] = acc->getId();
						accJson["name"] = acc->getName();
						accJson["location"] = acc->getLocation();
						accJson["address"] = acc->getAddress();
						this->populateRoomsOfRental(acc->getId());

						json roomsArray = json::array();
						for (auto* room : acc->getRooms()) {
							json rJson;
							rJson["id"] = room->getId();
							rJson["price"] = room->getPrice();
							rJson["facilities"] = json::array();
							Facilities* f = room->getFacilities();
							rJson["facilities"] = json::array();
							if(f->getAC())
								rJson["facilities"].push_back("AC");
							if(f->getBalcony())
								rJson["facilities"].push_back("Balcony");
							if(f->getCouch())
								rJson["facilities"].push_back("Couch");
							if(f->getFridge())
								rJson["facilities"].push_back("Fridge");
							if(f->getTV())
								rJson["facilities"].push_back("TV");
							if (dynamic_cast<Single*>(room)) {
								rJson["type"] = "Single";
								rJson["beds"] = 1;
							}
							else if (dynamic_cast<Double*>(room)) {
								rJson["type"] = "Double";
								rJson["beds"] = 2;
							}
							else if (dynamic_cast<Triple*>(room)) {
								rJson["type"] = "Triple";
								rJson["beds"] = 3;
							}

							roomsArray.push_back(rJson);
						}

						accJson["rooms"] = roomsArray;
						dataArray.push_back(accJson);
					}

					response["data"] = dataArray;
					std::string s = response.dump();
					send(clientSocket, s.c_str(), (int)s.length(), 0);
				}
				else if (j["type"] == "FORCE_LOGOUT") {
					std::string email = j["email"].get<std::string>();

					json res;
					res["type"] = "FORCE_LOGOUT";

					for (auto u : this->users) {
						if (u->getMail() == email) {
							auto it = std::find(users.begin(), users.end(), u);
							if (it != users.end()) {
								delete* it;
								users.erase(it);
							}
						}
					}
					res["status"] = "success";
					res["message"] = "Utilizatorul a fost deconectat";
					send(clientSocket, res.dump().c_str(), (int)res.dump().length(), 0);
				}

			}
			catch (const std::exception& e) {
				std::cout << "Eroare: " << e.what() << std::endl;
			}
		}

		if (!emailLogat.empty()) {
			std::lock_guard<std::mutex> lock(mtx); 
			for (auto it = users.begin(); it != users.end(); ++it) {
				if ((*it)->getMail() == emailLogat) {
					delete* it;      
					users.erase(it); 
					Logger::getInstanceLogger().setMessage("[CLEANUP] Sesiune inchisa pentru: " + emailLogat);
					Logger::getInstanceLogger().printMessageOnFile();
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
			Logger::getInstanceLogger().setMessage("Eroare la Bind. Cod: " + WSAGetLastError());
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
	bool searchUser(IUsers* newUser);
	IUsers* loginUser(string mail, string password);
	void printUsers();
	void printRentals();
	void loadAllRentals();
	RentalUnit* findRentalById(int id);
	void populateRoomsOfRental(int id);

};