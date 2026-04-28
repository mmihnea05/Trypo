#pragma once
#include<iostream>
using namespace std;
class Facilities;
class ReservationTime;
class Extras;

class IRoom {
public:
	virtual int getId() = 0;
	virtual float getPrice() = 0;
	virtual Facilities* getFacilities() = 0;
	virtual int getId()=0;
	virtual float getPrice()=0;
	virtual Facilities* getFacilities() = 0;
};

