#pragma once
#include "CRoom.h"
class Single : public CRoom {
public:
	Single(int id, float price, Facilities* facilities, Extras* extras) : CRoom(id, price, facilities, extras) {
		pricePerNight = price * 1.2;
		capacity = 1;
	}
	float getPrice() { return pricePerNight; }
	~Single() = default;
};
