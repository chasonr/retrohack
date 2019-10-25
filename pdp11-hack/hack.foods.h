/*
 * Hack.foods.h
 */

/* Dog eats foods 0-4 but prefers 1 above 0, 2, 3, 4 */

struct food foods[] = {
	{ "food ration",  50, 5, 800 },
	{ "tripe ration", 20, 1, 200 },
	{ "pancake",  	   3, 1, 200 },
	{ "dead lizard",   3, 0,  40 },
	{ "cookie",	   7, 0,  40 },
	{ "orange",	   3, 0,  80 },
	{ "apple",	   3, 0,  50 },
	{ "pear",	   3, 0,  50 },
	{ "melon",	   1, 0, 100 },
	{ "banana",	   3, 0,  80 },
	{ "candy bar",	   3, 0, 100 },
	{ "egg",	   1, 0,  80 },
	{ "PROG ERROR",  100, 0,   0 }
};
