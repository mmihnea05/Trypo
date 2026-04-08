#include "Service.h"

void Service::registerUser(IUsers* newUser) {
    if (!newUser) return;

    try {
        // Pregătim comanda SQL
        // Notă: Coloana 'Role' va determina dacă obiectul creat la Login va fi Admin sau Client
        nanodbc::statement stmt(this->conn);
        nanodbc::prepare(stmt, "INSERT INTO Users (Name, Password, Mail, PhoneNumber, BirthDate, Country, Gender, Address, Role) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)");

        // Extragem datele folosind getterele din interfață
        // Salvăm în variabile locale pentru a păstra memoria validă pentru .c_str()
        std::string n = newUser->getName();
        std::string p = newUser->getPassword();
        std::string m = newUser->getMail();
        std::string ph = newUser->getPhone();
        std::string bd = newUser->getBirthDate();
        std::string c = newUser->getCountry();
        std::string g = newUser->getGender();
        std::string a = newUser->getAddress();
        Admin* adminPtr = dynamic_cast<Admin*>(this->user);
        int r;
        if (adminPtr) {
            r = 1;
        }
        else {
            r = 0;
        }

        stmt.bind(0, n.c_str());
        stmt.bind(1, p.c_str());
        stmt.bind(2, m.c_str());
        stmt.bind(3, ph.c_str());
        stmt.bind(4, bd.c_str());
        stmt.bind(5, c.c_str());
        stmt.bind(6, g.c_str());
        stmt.bind(7, a.c_str());
        stmt.bind(8, &r);

        nanodbc::execute(stmt);

        Logger::getInstanceLogger().setMessage("Utilizator inregistrat cu succes: " + n);
    }
    catch (const std::exception& e) {
        Logger::getInstanceLogger().setMessage("Eroare la inregistrare: " + std::string(e.what()));
        throw;
    }
}
void Service::loginUser(string mail,string password) {
    try {
        cout << "Test login" << endl;
        nanodbc::statement stmt(this->conn);
        nanodbc::prepare(stmt, "SELECT * FROM Users WHERE Mail = ? AND Password = ?");

        stmt.bind(0, mail.c_str());
        stmt.bind(1, password.c_str());

        auto result = nanodbc::execute(stmt);

        if (result.next()) {
            int id = result.get<int>("Id");
            string nume = result.get<string>("Name");
            string phone = result.get<string>("PhoneNumber");
            string birthDate = result.get<string>("BirthDate");
            string country = result.get<string>("Country");
            string gender = result.get<string>("Gender");
            string address = result.get<string>(7);
            int rol = result.get<int>("Role");
            string role;
            if (this->user != nullptr) delete this->user;

            // FACTORY: Creăm obiectul în funcție de rolul din SQL
            if (rol == 1) {
                this->user = new Admin(nume,password,mail,phone);
                role = "Admin";
            }
            else {
                this->user = new Client(nume,password,mail,phone,birthDate,country,gender,address);
                role = "Client";
            }
            cout << "Login reusit pentru: " << nume << " [" << rol << "]";
            Logger::getInstanceLogger().setMessage("Login reusit pentru: " + nume + " [" + role + "]");
        }
        else {
            cout << "Tentativa de login esuata pentru: ";
            Logger::getInstanceLogger().setMessage("Tentativa de login esuata pentru: " + mail);
            delete this->user;
            this->user = nullptr;
        }
    }
    catch (const std::exception& e) {
        Logger::getInstanceLogger().setMessage("Eroare SQL Login: " + std::string(e.what()));
        delete this->user;
        this->user = nullptr;
    }
}

void Service::printUsers() {
    cout << "Lista utilizatorilor: " << endl;
    auto results = nanodbc::execute(getConn(), "SELECT Id, Name, Mail, Country, Role FROM Users");

    while (results.next()) {
        // Preluam datele dupa index (0, 1, 2...) sau dupa numele coloanei
        int id = results.get<int>("Id");
        string nume = results.get<string>("Name");
        string email = results.get<string>("Mail");
        string tara = results.get<string>("Country");
        int rol = results.get<int>("Role");
        string role;
        if (rol == 0)
            role = "Client";
        else
            role = "Admin";

        cout << "ID: " << id << " | Nume: " << nume << " | Email: " << email << " | Tara: " << tara  <<" | Rol: "<< role << endl;
    }
}