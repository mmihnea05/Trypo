#include <iostream>
#include <nanodbc/nanodbc.h>
#include <exception>
using namespace std;
#include "Client.h"
#include "Service.h"

int main() {
    try {
        Service* service = new Service(NANODBC_TEXT("Driver={ODBC Driver 17 for SQL Server};Server=.\\SQLEXPRESS;Database=Trypo;Trusted_Connection=yes;"));
        IUsers* u=new Client("Alin","parola","c@gmail.com","0788888888","2000-11-21","Romania","M","Strada Nu stiu");
        //service->registerUser(u);
        service->printUsers();

        service->loginUser("c@gmail.com", "parola");
    }
    catch (const std::exception& e) {
        std::cerr << "Eroare: " << e.what() << std::endl;
    }
    Logger::getInstanceLogger().printMessage();
    return 0;
}