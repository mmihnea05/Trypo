#pragma once
#include "CRoom.h"
class Double : public CRoom{
public:
	Double(int id, float price, Facilities* facilities, Extras* extras) : CRoom(id, price, facilities, extras) {
		pricePerNight = price * 1.3;
		capacity = 2;
	}
};

