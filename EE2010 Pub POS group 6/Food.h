#pragma once

#include "Item.h"

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
};

