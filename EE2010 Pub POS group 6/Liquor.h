#pragma once

#include "Drink.h"
#include <cstdint> // uint8_t
#include <corecrt.h>

// Liquor shots. Tracks single/double measure.
enum class ShotMeasure
{
    singleShot,
    doubleShot
};

class Liquor : public Drink
{
private:
    ShotMeasure measure{}; // single or double

public:
    Liquor(ShotMeasure measure, std::string name, float price, uint8_t screen, int quantity);

    ShotMeasure getMeasure() const {
        return measure;
    }
};

