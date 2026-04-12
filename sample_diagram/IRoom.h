#pragma once
#include<iostream>
using namespace std;
class Facilities;
class ReservationTime;
class Extras;

class IRoom{
public:
	virtual void setPricePerNight(float pricePerNight) = 0;
};

