#pragma once
class IUsers;
class IRoom;

typedef enum status {
	cancelled, confirmed, finished
};

class Accomodation{
private:
	int accomodationId; // static
	int clientId;
	IUsers* user;
	IRoom* rentedRoom;
	status accomodationStatus;

public:
};

