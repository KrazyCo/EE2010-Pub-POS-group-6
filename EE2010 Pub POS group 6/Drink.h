#pragma once

#include "Item.h"
#include <cstdint>
#include <string>

// Drinks extend Item with an alcohol flag so we can enforce ID checks.
class Drink : public Item
{
private:
    bool isAlcohol{}; // true if age-restricted

public:
    Drink(bool alcohol, std::string name, float price, uint8_t screen, int quantity);

    bool getIsAlcohol() const {
        return isAlcohol;
    }
};

