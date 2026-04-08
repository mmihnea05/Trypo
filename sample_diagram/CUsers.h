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
	virtual ~CUsers() {}
	std::string getName() const override{ return name; }
	std::string getPassword() const override { return password; }
	std::string getMail() const override { return mail; }
	std::string getPhone() const override { return phoneNumber; }
};

