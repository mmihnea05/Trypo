#pragma once
#include<iostream>

class Facilities;
class ReservationTime;
class Extras;

class IRoom {
public:
	virtual int getId() = 0;
	virtual float getPrice() = 0;
	virtual Facilities* getFacilities() = 0;
	virtual ~IRoom() =default;
};

