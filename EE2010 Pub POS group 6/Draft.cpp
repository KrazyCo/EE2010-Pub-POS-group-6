#include "Draft.h"

Draft::Draft(Measure measure, bool isAlcohol, std::string name, float price, uint8_t screen, int quantity)
    : Drink(isAlcohol, std::move(name), price, screen, quantity), measure(measure) {
}
