#pragma once

#include "Liquor.h"
#include "Draft.h"
#include "Food.h"
#include "2dUtils.h"
#include <vector>

// Configure catalog with starting quantities

// Liquor
Liquor doubleVodka(
    ShotMeasure::doubleShot,
    "Double Vodka",
    5.50f,
    1,
    coord2d{0, 0},
    span2d{1, 1},
    50 // quantity
);
Liquor singleVodka(
    ShotMeasure::singleShot,
    "Single Vodka",
    4.00f,
    1,
    coord2d{0, 1},
    span2d{1, 1},
    80
);

Liquor doubleWhiskey(
    ShotMeasure::doubleShot,
    "Double Whiskey",
    6.00f,
    1,
    coord2d{1, 0},
    span2d{1, 1},
    40
);
Liquor singleWhiskey(
    ShotMeasure::singleShot,
    "Single Whiskey",
    4.50f,
    1,
    coord2d{1, 1},
    span2d{1, 1},
    70
);

// Draft
Draft Asahi(
    Measure::pint,
    true,
    "Asahi",
    7.00f,
    2,
    coord2d{0, 0},
    span2d{1, 1},
    100
);
Draft HalfAsahi(
    Measure::halfPint,
    true,
    "Half Asahi",
    3.50f,
    2,
    coord2d{0, 1},
    span2d{1, 1},
    120
);
Draft Heineken(
    Measure::pint,
    true,
    "Heineken",
    6.50f,
    2,
    coord2d{1, 0},
    span2d{1, 1},
    90
);
Draft HalfHeineken(
    Measure::halfPint,
    true,
    "Half Heineken",
    3.25f,
    2,
    coord2d{1, 1},
    span2d{1, 1},
    110
);
Draft Coke(
    Measure::pint,
    false,
    "Coke",
    3.00f,
    2,
    coord2d{2, 0},
    span2d{1, 1},
    60
);
Draft HalfCoke(
    Measure::halfPint,
    false,
    "Half Coke",
    1.50f,
    2,
    coord2d{2, 1},
    span2d{1, 1},
    80
);

// Food
Food smallFries(
    FoodSize::small,
    "Small Fries",
    2.50f,
    3,
    coord2d{0, 0},
    span2d{1, 1},
    200
);
Food regularFries(
    FoodSize::regular,
    "Regular Fries",
    3.50f,
    3,
    coord2d{0, 1},
    span2d{1, 1},
    180
);

Food largeFries(
    FoodSize::large,
    "Large Fries",
    4.50f,
    3,
    coord2d{0, 2},
    span2d{1, 1},
    160
);

Food smallWings(
    FoodSize::small,
    "Small Wings",
    5.00f,
    3,
    coord2d{1, 0},
    span2d{1, 1},
    140
);
Food regularWings(
    FoodSize::regular,
    "Regular Wings",
    7.00f,
    3,
    coord2d{1, 1},
    span2d{1, 1},
    120
);

Food largeWings(
    FoodSize::large,
    "Large Wings",
    9.00f,
    3,
    coord2d{1, 2},
    span2d{1, 1},
    100
);

// Single unified non-owning catalog
static std::vector<Item*> catalogItems{
    &doubleVodka, &singleVodka, &doubleWhiskey, &singleWhiskey,
    &Asahi, &HalfAsahi, &Heineken, &HalfHeineken, &Coke, &HalfCoke,
    &smallFries, &regularFries, &largeFries, &smallWings, &regularWings, &largeWings
};