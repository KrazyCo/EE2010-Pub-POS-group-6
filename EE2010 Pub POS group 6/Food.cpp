#include "Food.h"

Food::Food(FoodSize size, std::string name, float price, uint8_t screen, int quantity)
    : Item(std::move(name), price, screen, quantity), size(size) {
}
