#pragma once

#include <string>
#include <vector>
#include <ctime> // time_t

#include "Item.h"

class Bill
{
private:
	std::string serverName{}; // name of the server
	std::vector<Item> itemsOrdered{}; // vector of items ordered
	float totalPrice{}; // total price of the bill
	bool paid{}; // whether the bill has been paid
	bool fulfilled{}; // whether the order has been fulfilled
	float discount{}; // discount applied to the bill if any
	time_t timeOpened{}; // time when the bill was opened
};

