#include "Drink.h"

Drink::Drink(bool alcohol, std::string name, float price, uint8_t screen, coord2d pos, span2d span, int quantity)
	: Item(std::move(name), price, screen, pos, span, quantity), isAlcohol(alcohol) {
}
