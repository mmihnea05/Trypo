#pragma once
#include "IRoom.h"
class CRoom : public IRoom {
protected:
	float pricePerNight;
	int capacity;
	int id; // static
	Facilities* facilities;
	ReservationTime* reservationTime;
	Extras* extras;

public:
	CRoom(int id, float pricePerNight, Facilities* facilities, Extras* extras) : id(id), pricePerNight(pricePerNight), facilities(facilities), extras(extras) {}
	virtual ~CRoom() = default;
	int getId() { return id; }
	virtual float getPrice() = 0;
	Facilities* getFacilities() { return facilities; }
};

