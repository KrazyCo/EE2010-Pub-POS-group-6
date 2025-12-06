#pragma once

#include "Item.h"
#include <cstdint>
#include <string>

class Drink : public Item
{
private:
    bool isAlcohol{}; // whether the drink is alcoholic so needs to be ID'ed
public:
    Drink(bool alcohol, std::string name, float price, uint8_t screen, int quantity);

    bool getIsAlcohol() const {
        return isAlcohol;
    }
};

