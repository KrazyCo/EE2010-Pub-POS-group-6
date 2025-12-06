#include "Food.h"

Food::Food(FoodSize size, std::string name, float price, uint8_t screen, coord2d pos, span2d span, int quantity)
	: Item(std::move(name), price, screen, pos, span, quantity), size(size) {
}
