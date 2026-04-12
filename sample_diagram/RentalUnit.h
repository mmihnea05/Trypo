#pragma once
#include<string>
#include<vector>
#include "Promotions.h"
using namespace std;
class IRoom;
class Promotions;

class RentalUnit{
private:
	int id; // static
	string name;
	string address;
	int capacity;
	vector<IRoom*> rooms;
	Promotions* promotion;

public:
	RentalUnit(string name, string address, int capacity, float promotion) : name(name), address(address), capacity(capacity) {
		this->promotion = new Promotions(promotion);
	}
	RentalUnit(int i,string name, string address, int capacity, float promotion) :id(i), name(name), address(address), capacity(capacity) {
		this->promotion = new Promotions(promotion);
	}
	~RentalUnit() {
		for (auto a : rooms) {
			delete a;
		}
	}
	void addRoom(IRoom* r) {
		this->rooms.push_back(r);
	}

	int getId() { return id; }
};

