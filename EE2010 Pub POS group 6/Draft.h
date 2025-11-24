#pragma once

#include "Drink.h"

enum class Measure
{
	pint,
	halfPint
};

class Draft : public Drink
{
private:
	Measure measure{}; // measure of the draft
};

