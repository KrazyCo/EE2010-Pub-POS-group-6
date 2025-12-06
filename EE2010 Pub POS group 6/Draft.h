#pragma once

#include "Drink.h"
#include <corecrt.h>
#include <cstdint>
#include <string>

enum class Measure
{
    pint,
    halfPint
};

class Draft : public Drink
{
private:
    Measure measure{}; // measure of the draft
public:
    Draft(Measure measure, bool isAlcohol, std::string name, float price, uint8_t screen, int quantity);

    Measure getMeasure() const {
        return measure;
    }
};

