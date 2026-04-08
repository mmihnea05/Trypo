#include <iostream>
#include <nanodbc/nanodbc.h>
#include <exception>
using namespace std;
#include "Client.h"

int main() {
    try {
        // String de conexiune pentru LocalDB sau SQL Express
        // Driver-ul trebuie să coincidă cu cel instalat pe PC (13, 17 sau 18)
        std::string connection_string =
            NANODBC_TEXT("Driver={ODBC Driver 17 for SQL Server};Server=.\\SQLEXPRESS;Database=Trypo;Trusted_Connection=yes;");

        nanodbc::connection conn(connection_string);

        if (conn.connected()) {
            std::cout << "Succes! Nanodbc este instalat si conectat la SSMS." << std::endl;
            std::cout << "Nume DBMS: " << conn.dbms_name() << std::endl;
        }

        IUsers* u=new Client("Alin","parola","c@gmail.com","0788888888","2000-11-21","Romania","M","Strada Nu stiu"); // Cream un obiect de tip User

        // --- PASUL 2: INSERAREA ---
        cout << "Se insereaza utilizatorul: " << u->getName() << "..." << endl;

        std::string name = u->getName();
        std::string pass = u->getPassword();
        std::string mail = u->getMail();
        std::string phone = u->getPhone();
        std::string bday = u->getBirthDate();
        std::string country = u->getCountry();
        std::string gender = u->getGender();
        std::string address = u->getAddress();

        nanodbc::statement stmt_insert(conn);
        nanodbc::prepare(stmt_insert, "INSERT INTO Users (Nume, Parola, Email, Telefon, DataNastere, Tara, Gen, Adresa) VALUES (?, ?, ?, ?, ?, ?, ?, ?)");

        // 2. Mapăm variabilele locale (NU getterii direct)
        stmt_insert.bind(0, name.c_str());
        stmt_insert.bind(1, pass.c_str());
        stmt_insert.bind(2, mail.c_str());
        stmt_insert.bind(3, phone.c_str());
        stmt_insert.bind(4, bday.c_str());
        stmt_insert.bind(5, country.c_str());
        stmt_insert.bind(6, gender.c_str());
        stmt_insert.bind(7, address.c_str());

        nanodbc::execute(stmt_insert);
        cout << "Inserare reusita!" << endl << "----------------------------" << endl;


        // --- PASUL 3: AFISAREA (SELECT) ---
        cout << "Lista utilizatorilor din baza de date:" << endl;

        auto results = nanodbc::execute(conn, "SELECT Id, Nume, Email, Tara FROM Users");

        while (results.next()) {
            // Preluam datele dupa index (0, 1, 2...) sau dupa numele coloanei
            int id = results.get<int>("Id");
            string nume = results.get<string>("Nume");
            string email = results.get<string>("Email");
            string tara = results.get<string>("Tara");

            cout << "ID: " << id << " | Nume: " << nume << " | Email: " << email << " | Tara: " << tara << endl;
        }

    }
    
    catch (const std::exception& e) {
        std::cerr << "Eroare: " << e.what() << std::endl;
    }

    return 0;
}