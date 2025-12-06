#pragma once

#include "Item.h"
#include <cstdint>
#include <corecrt.h>
#include <string>

// Food options with a size enum (small/regular/large).
enum class FoodSize
{
    small,
    regular,
    large
};

class Food : public Item
{
private:
    FoodSize size{}; // portion size

public:
    Food(FoodSize size, std::string name, float price, uint8_t screen, int quantity);

    FoodSize getSize() const {
        return size;
    }
};

