#pragma once
#include "IUsers.h"
#include "Accomodation.h"
#include <iostream>
#include <vector>
#include <nanodbc/nanodbc.h>
#include "Logger.h"
#include "Admin.h"
#include "Client.h"

class Service{
private:
	nanodbc::connection conn;
	IUsers* user;
	std::vector<Accomodation*> accom;
public:

    nanodbc::connection& getConn() {
        return conn;
    }

	Service(string connection_string) {
        try {
            conn = nanodbc::connection(connection_string);
            if (conn.connected()) {
                cout << "Conexiune reusita la BD.";
                Logger::getInstanceLogger().setMessage("Conexiune reusita la BD.");
            }
        }
        catch (const std::exception& e) {
            Logger::getInstanceLogger().setMessage("Eroare la conectare: " + std::string(e.what()));
        }
    }
    ~Service() {
        for (auto a : accom) {
            delete a;
        }
        accom.clear();
        if (user) delete user;
    }
    void registerUser(IUsers* newUser);
    void loginUser(string mail, string password);
    void printUsers();
};

