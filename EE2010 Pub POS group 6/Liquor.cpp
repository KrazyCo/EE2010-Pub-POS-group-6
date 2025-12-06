#include "Liquor.h"

Liquor::Liquor(ShotMeasure measure, std::string name, float price, uint8_t screen, coord2d pos, span2d span, int quantity)
	: Drink(true, std::move(name), price, screen, pos, span, quantity), measure(measure) {
}
