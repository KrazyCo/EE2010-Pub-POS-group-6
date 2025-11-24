#pragma once

#include "Item.h"

class Drink : public Item
{
private:
	bool isAlcohol{}; // whether the drink is alcoholic so needs to be ID'ed
};

