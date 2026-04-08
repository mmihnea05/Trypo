#pragma once
#include "CUsers.h"
class Client : public CUsers {
private:
	string birthDate;
	string country;
	string gender;
	string address;

public:
	Client(string name, string password, string mail, string phoneNumber,string birthDate,string country, string gender, string address): CUsers(name,password,mail,phoneNumber), birthDate(birthDate), country(country), gender(gender), address(address){}
	~Client(){}

	std::string getBirthDate() const { return birthDate; }
	std::string getCountry() const { return country; }
	std::string getGender() const { return gender; }
	std::string getAddress() const { return address; }
};

