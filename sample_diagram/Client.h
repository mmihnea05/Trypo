#pragma once
#include "CUsers.h"
class Client : public CUsers {
private:
	string birthDate;
	string country;
	string gender;
	string address;
	float balance;

public:
	Client(string name, string password, string mail, string phoneNumber, string birthDate, string country, string gender, string address) : CUsers(name, password, mail, phoneNumber), birthDate(birthDate), country(country), gender(gender), address(address) {
		this->balance = 3000;
	}
	~Client(){}

	std::string getBirthDate() const { return birthDate; }
	std::string getCountry() const { return country; }
	std::string getGender() const { return gender; }
	std::string getAddress() const { return address; }
	float addBalance(float sum) {
		this->balance += sum;
	}
	float deductBalance(float sum) {
		this->balance -= sum;
	}
	bool enoughBalance(float sum) {
		if (this->balance > sum)
			return true;
		else
			return false;
	}
};

