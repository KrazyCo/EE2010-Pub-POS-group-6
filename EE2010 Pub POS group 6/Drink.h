#pragma once

#include "Item.h"
#include <cstdint>
#include "2dUtils.h"
#include <string>

class Drink : public Item
{
private:
	bool isAlcohol{}; // whether the drink is alcoholic so needs to be ID'ed
public:
	Drink(bool alcohol, std::string name, float price, uint8_t screen, coord2d pos, span2d span)
		: Item(name, price, screen, pos, span), isAlcohol(alcohol) {
	}
};

