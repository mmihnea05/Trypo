#include "Service.h"

void Service::registerUser(IUsers* newUser) {
    if (!newUser) return;

    if (this->searchUser(newUser)) {
        throw std::runtime_error("Email-ul este deja inregistrat.");
    }
    try {

        nanodbc::statement stmt(this->conn);
        nanodbc::prepare(stmt, "INSERT INTO Users (Name, Password, Mail, PhoneNumber, BirthDate, Country, Gender, Address, Role) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)");

        std::string n = newUser->getName();
        std::string p = newUser->getPassword();
        std::string m = newUser->getMail();
        std::string ph = newUser->getPhone();
        std::string bd = newUser->getBirthDate();
        std::string c = newUser->getCountry();
        std::string g = newUser->getGender();
        std::string a = newUser->getAddress();
        int r = 0;

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
        Logger::getInstanceLogger().printMessageOnFile();
    }
    catch (const std::exception& e) {
        Logger::getInstanceLogger().setMessage("Eroare la inregistrare: " + std::string(e.what()));
        Logger::getInstanceLogger().printMessageOnFile();
        throw;
    }

}
bool Service::searchUser(IUsers* newUser) {
    try {
        nanodbc::statement stmt(this->conn);
        nanodbc::prepare(stmt, "SELECT * FROM Users WHERE Mail = ?");

        stmt.bind(0, newUser->getMail().c_str());

        auto result = nanodbc::execute(stmt);

        if (result.next())
            return true;
        else
            return false;
    }
    catch (const std::exception& e) {
        Logger::getInstanceLogger().setMessage("Eroare SQL Login: " + std::string(e.what()));
        Logger::getInstanceLogger().printMessageOnFile();
        return true;
    }
}

IUsers* Service::loginUser(string mail, string password) {
    for (auto u : this->users) {
        if (u->getMail() == mail) {
            Logger::getInstanceLogger().setMessage("Tentativa de login dublu pentru: " + mail);
            Logger::getInstanceLogger().printMessageOnFile();
            return nullptr;
        }
    }
    try {
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
            string address = result.get<string>("Address");
            int rol = result.get<int>("Role");
            string role;
            IUsers* u = nullptr;
            if (rol == 1) {
                u = new Admin(nume, password, mail, phone);
                role = "Admin";

                this->users.push_back(u);

            }
            else {
                u = new Client(nume, password, mail, phone, birthDate, country, gender, address);
                role = "Client";

            }
            Logger::getInstanceLogger().setMessage("Login reusit pentru: " + nume + " [" + role + "]");
            Logger::getInstanceLogger().printMessageOnFile();
            return u;
        }
        else {
            return nullptr;
            Logger::getInstanceLogger().setMessage("Tentativa de login esuata pentru: " + mail);
            Logger::getInstanceLogger().printMessageOnFile();
        }
    }
    catch (const std::exception& e) {
        Logger::getInstanceLogger().setMessage("Eroare SQL Login: " + std::string(e.what()));
        Logger::getInstanceLogger().printMessageOnFile();
    }
}

void Service::printUsers() {
    cout << "Lista utilizatorilor: " << endl;
    auto results = nanodbc::execute(getConn(), "SELECT Id, Name, Mail, Country, Role FROM Users");

    while (results.next()) {
        int id = results.get<int>("Id");
        string nume = results.get<string>("Name");
        string email = results.get<string>("Mail");
        string tara = results.get<string>("Country");
        string adr = results.get<string>("Address");
        int rol = results.get<int>("Role");
        string role;
        if (rol == 0)
            role = "Client";
        else
            role = "Admin";

        cout << "ID: " << id << " | Nume: " << nume << " | Email: " << email << " | Tara: " << tara << " | Rol: " << role << " | Adresa: " << adr << endl;
    }
}

void Service::printRentals() {
    cout << "Lista unitatilor de cazare: " << endl;
    auto results = nanodbc::execute(getConn(), "SELECT id, name, address,capacity FROM RentalUnit");

    while (results.next()) {
        int id = results.get<int>("id");
        string nume = results.get<string>("name");
        string address = results.get<string>("address");
        int capacity = results.get<int>("capacity");
        cout << "ID: " << id << " | Nume: " << nume << " | Adresa: " << address << " | Capacity: " << capacity << endl;
    }
}

void Service::loadAllRentals() {
    auto results = nanodbc::execute(getConn(), "SELECT * FROM RentalUnit");
    while (results.next()) {
        int id = results.get<int>("id");
        string nume = results.get<string>("name");
        string address = results.get<string>("address");
        string location = results.get<string>("location");
        int capacity = results.get<int>("capacity");
        float discount = results.get<float>("discount");

        RentalUnit* r = new RentalUnit (id, nume,location, address, capacity, discount);
        if (findRentalById(id) == nullptr)
            this->rentals.push_back(r);
    }

}
RentalUnit* Service::findRentalById(int id) {
    for (auto r : this->rentals)
        if (r->getId() == id)
            return r;
    return nullptr;
}
void Service::populateRoomsOfRental(int id) {
    nanodbc::statement stmt(this->conn);
    nanodbc::prepare(stmt, ("SELECT * FROM Rooms WHERE rentalUnitID = ?"));
    stmt.bind(0, &id);
    auto results = nanodbc::execute(stmt);
    while (results.next()) {
        int capacity = results.get<int>("capacity");
        int ID = results.get<int>("id");
        float price = results.get<float>("pricePerNight");
        bool bf = results.get<int>("breakfast");
        bool eC = results.get<int>("extraCleaning");
        bool park = results.get<int>("parking");
        bool pool = results.get<int>("pool");
        bool sauna = results.get<int>("sauna");
        bool AC = results.get<int>("AC");
        bool balcony = results.get<int>("balcony");
        bool couch = results.get<int>("couch");
        bool fridge = results.get<int>("fridge");
        int numBeds = results.get<int>("numBeds");
        bool TV = results.get<int>("TV");
        Extras* e = new Extras(bf, park, pool, sauna, eC);
        Facilities* f = new Facilities(balcony, fridge, AC, numBeds, TV, couch);
        RentalUnit* r = findRentalById(id);
        IRoom* c;
        if (capacity == 1)
            c = new Single(ID, price, f, e);
        else
            if (capacity == 2)
                c = new Double(ID, price, f, e);
            else
                c = new Triple(ID, price, f, e);
        r->addRoom(c);
    }
}
