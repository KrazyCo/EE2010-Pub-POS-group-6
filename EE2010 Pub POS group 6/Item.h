#pragma once

#include <ctime> // time_t
#include <cstdint>
#include "2dUtils.h"

class Item
{
private:
	float price{}; // price of the item
	time_t timeOrdered{}; // time when the item was ordered
	uint8_t screen{}; // screen on the GUI
	coord2d position{}; // position on the screen
	span2d span{}; // size on the screen
};

