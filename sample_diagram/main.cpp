#include <iostream>
#include <nanodbc/nanodbc.h>
#include <exception>
using namespace std;
#include "Client.h"
#include "Service.h"

int main() {
    try {
        Service* service = new Service(NANODBC_TEXT("Driver={ODBC Driver 17 for SQL Server};Server=.\\SQLEXPRESS;Database=Trypo;Trusted_Connection=yes;"));
            service->startServer();
        
        
    }
    catch (const std::exception& e) {
        std::cerr << "Eroare: " << e.what() << std::endl;
    }
    return 0;
}