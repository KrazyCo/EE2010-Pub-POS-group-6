#pragma once

#include "Drink.h"

enum class Measure
{
	singleShot,
	doubleShot
};

class Liquor : public Drink
{
private:
	Measure measure{}; // measure of the liquor
};

