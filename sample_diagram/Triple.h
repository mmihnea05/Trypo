#pragma once
#include "CRoom.h"
class Triple : public CRoom {
public:
	Triple(int id, float price, Facilities* facilities, Extras* extras) : CRoom(id, price, facilities, extras) {
		pricePerNight = price * 1.5;
		capacity = 3;
	}
	float getPrice() { return pricePerNight; }
};