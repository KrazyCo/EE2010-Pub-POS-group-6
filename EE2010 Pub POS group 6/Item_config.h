#pragma once

#include "Liquor.h"
#include "Draft.h"
#include "Food.h"
#include <vector>

// Configure catalog with starting quantities

// Liquor
Liquor doubleVodka(
    ShotMeasure::doubleShot,
    "Double Vodka",
    5.50f,
    1,
    5 // quantity
);
Liquor singleVodka(
    ShotMeasure::singleShot,
    "Single Vodka",
    4.00f,
    1,
    8
);

Liquor doubleWhiskey(
    ShotMeasure::doubleShot,
    "Double Whiskey",
    6.00f,
    1,
    4
);
Liquor singleWhiskey(
    ShotMeasure::singleShot,
    "Single Whiskey",
    4.50f,
    1,
    10
);

// Draft
Draft Asahi(
    Measure::pint,
    true,
    "Asahi",
    7.00f,
    2,
    12
);
Draft HalfAsahi(
    Measure::halfPint,
    true,
    "Half Asahi",
    3.50f,
    2,
    15
);
Draft Heineken(
    Measure::pint,
    true,
    "Heineken",
    6.50f,
    2,
    10
);
Draft HalfHeineken(
    Measure::halfPint,
    true,
    "Half Heineken",
    3.25f,
    2,
    14
);
Draft Coke(
    Measure::pint,
    false,
    "Coke",
    3.00f,
    2,
    8
);
Draft HalfCoke(
    Measure::halfPint,
    false,
    "Half Coke",
    1.50f,
    2,
    12
);

// Food
Food smallFries(
    FoodSize::small,
    "Small Fries",
    2.50f,
    3,
    20
);
Food regularFries(
    FoodSize::regular,
    "Regular Fries",
    3.50f,
    3,
    18
);

Food largeFries(
    FoodSize::large,
    "Large Fries",
    4.50f,
    3,
    16
);

Food smallWings(
    FoodSize::small,
    "Small Wings",
    5.00f,
    3,
    14
);
Food regularWings(
    FoodSize::regular,
    "Regular Wings",
    7.00f,
    3,
    12
);

Food largeWings(
    FoodSize::large,
    "Large Wings",
    9.00f,
    3,
    10
);

// Single unified non-owning catalog
static std::vector<Item*> catalogItems{
    &doubleVodka, &singleVodka, &doubleWhiskey, &singleWhiskey,
    &Asahi, &HalfAsahi, &Heineken, &HalfHeineken, &Coke, &HalfCoke,
    &smallFries, &regularFries, &largeFries, &smallWings, &regularWings, &largeWings
};