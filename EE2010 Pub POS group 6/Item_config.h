#pragma once

#include "Liquor.h"
#include "Draft.h"
#include "2dUtils.h"


Liquor doubleVodka(
	ShotMeasure::doubleShot,
	"Double Vodka",
	5.50f,
	1,
	coord2d{0, 0},
	span2d{1, 1}
);
Liquor singleVodka(
	ShotMeasure::singleShot,
	"Single Vodka",
	4.00f,
	1,
	coord2d{0, 1},
	span2d{1, 1}
);
Liquor doubleWhiskey(
	ShotMeasure::doubleShot,
	"Double Whiskey",
	6.00f,
	1,
	coord2d{1, 0},
	span2d{1, 1}
);
Liquor singleWhiskey(
	ShotMeasure::singleShot,
	"Single Whiskey",
	4.50f,
	1,
	coord2d{1, 1},
	span2d{1, 1}
);

Draft Asahi(
	Measure::pint,
	true,
	"Asahi",
	7.00f,
	2,
	coord2d{0, 0},
	span2d{1, 1}
);
Draft HalfAsahi(
	Measure::halfPint,
	true,
	"Half Asahi",
	3.50f,
	2,
	coord2d{0, 1},
	span2d{1, 1}
);
Draft Heineken(
	Measure::pint,
	true,
	"Heineken",
	6.50f,
	2,
	coord2d{1, 0},
	span2d{1, 1}
);
Draft HalfHeineken(
	Measure::halfPint,
	true,
	"Half Heineken",
	3.25f,
	2,
	coord2d{1, 1},
	span2d{1, 1}
);
Draft Coke(
	Measure::pint,
	false,
	"Coke",
	3.00f,
	2,
	coord2d{2, 0},
	span2d{1, 1}
);
Draft HalfCoke(
	Measure::halfPint,
	false,
	"Half Coke",
	1.50f,
	2,
	coord2d{2, 1},
	span2d{1, 1}
);