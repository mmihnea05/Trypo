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

};

