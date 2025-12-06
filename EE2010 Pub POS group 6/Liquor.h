#pragma once

#include "Drink.h"
#include "2dUtils.h"
#include <cstdint> // uint8_t
#include <corecrt.h>

enum class ShotMeasure
{
	singleShot,
	doubleShot
};

class Liquor : public Drink
{
private:
	ShotMeasure measure{}; // measure of the liquor
public:
	Liquor(ShotMeasure measure, std::string name, float price, uint8_t screen, coord2d pos, span2d span, int quantity);

	ShotMeasure getMeasure() const {
		return measure;
	}
};

