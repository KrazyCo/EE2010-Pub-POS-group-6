#include "Draft.h"

Draft::Draft(Measure measure, bool isAlcohol, std::string name, float price, uint8_t screen, coord2d pos, span2d span, int quantity)
	: Drink(isAlcohol, std::move(name), price, screen, pos, span, quantity), measure(measure) {
}
