/*
 * Hack.invinit.c
 */

#include "hack.h"

/*
 * struct obj {
 *     struct obj *nobj;
 *     char otyp, spe;
 *     ox, oy, olet, quan, known, cursed, unpaid
 * };
 */

struct obj arrows0 = {
	(struct obj *)0,
	W_ARROW, 0, 0, 0, ')', 25, 1, 0, 0	/* 25 +0 arrows */
};

struct obj bow0 = {
	&arrows0,
 	W_BOW, 1, 0, 0, ')', 1, 1, 0, 0		/* +1 bow */
};

struct obj mace0 = {
	&bow0,
 	W_MACE, 1, 0, 0, ')', 1, 1, 0, 0	/* +1 mace */
};

struct obj uarm0 = {
	&mace0,
 	A_RING, 4, 0, 0, '[', 1, 1, 0, 0	/* +1 ring mail */
};

struct obj food0 = {
	&uarm0,
 	F_FOOD, 0, 0, 0, '%', 2, 1, 0, 0	/* 2 food rations */
};

struct obj *yourinvent0 = &food0;
