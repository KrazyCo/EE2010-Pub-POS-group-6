#include "Drink.h"

Drink::Drink(bool alcohol, std::string name, float price, uint8_t screen, int quantity)
    : Item(std::move(name), price, screen, quantity), isAlcohol(alcohol) {
}
