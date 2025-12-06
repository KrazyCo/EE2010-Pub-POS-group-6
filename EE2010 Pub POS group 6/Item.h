#pragma once

#include <ctime> // time_t
#include <cstdint>
#include "2dUtils.h"
#include <string>

class Item
{
private:
	std::string name{}; // name of the item
	float price{}; // price of the item
	uint8_t screen{}; // screen on the GUI
	coord2d position{}; // position on the screen
	span2d span{}; // size on the screen
public:
	Item(std::string name, float price, uint8_t screen, coord2d pos, span2d span)
		: name(name), price(price), screen(screen), position(pos), span(span) {
	}
};

