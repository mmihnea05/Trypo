#pragma once
#include "CUsers.h"
class Admin : public CUsers{
public:
	Admin(string name, string password, string mail, string phoneNumber): CUsers(name,password,mail,phoneNumber){}
	~Admin() {}
};