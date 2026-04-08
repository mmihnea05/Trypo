#pragma once
#include "IUsers.h"
class CUsers : public IUsers{
protected:
	int id; // static
	string name;
	string password;
	string mail;
	string phoneNumber;

public:
	CUsers(string name,string password, string mail,string phoneNumber): name(name),password(password), mail(mail), phoneNumber(phoneNumber){}
	~CUsers() {}
	std::string getName() const { return name; }
	std::string getPassword() const { return password; }
	std::string getMail() const { return mail; }
	std::string getPhone() const { return phoneNumber; }
};

