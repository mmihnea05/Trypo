#pragma once
#include<string>
using namespace std;

class Users{
private:
	int id; // static
	string name;
	string password;
	string mail;
	string phoneNumber;
	string birthDate;
	string country;
	string gender;
	string address;

public:
	Users(string name,string password, string mail,string phonenumber,string birthDate, string country, string gender, string address): name(name), password(password), mail(mail), phoneNumber(phonenumber), birthDate(birthDate),country(country), gender(gender),address(address) {}

	std::string getName()  { return name; }
	std::string getPassword()  { return password; }
	std::string getMail()  { return mail; }
	std::string getPhone()  { return phoneNumber; }
	std::string getBirthDate() { return birthDate; }
	std::string getCountry()  { return country; }
	std::string getGender()  { return gender; }
	std::string getAddress() { return address; }
};

