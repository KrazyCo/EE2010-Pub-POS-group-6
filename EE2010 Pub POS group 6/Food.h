#pragma once

#include "Item.h"
#include <cstdint>
#include <corecrt.h>
#include "2dUtils.h"
#include <string>

enum class FoodSize
{
	small,
	regular,
	large
};

class Food : public Item
{
private:
	FoodSize size{}; // size of the food item
public:
	Food(FoodSize size, std::string name, float price, uint8_t screen, coord2d pos, span2d span, int quantity);

	FoodSize getSize() const {
		return size;
	}
};

