#include "Liquor.h"

Liquor::Liquor(ShotMeasure measure, std::string name, float price, uint8_t screen, int quantity)
    : Drink(true, std::move(name), price, screen, quantity), measure(measure) {
}
