/*
 * Mklev.mkobj.c
 */

#define NORMAL_IO
#include "hack.h"
#include "hack.vars.h"

mkfood () {
	register        FOOD fp;
	register        i = rn2 (100);

	fp = &foods[0];
	while ((i -= fp -> prob) >= 0)
		fp++;
	return (fp - foods);
}

mkarm () {
	register        ARMOR ap;
	register        i = rn2 (100);

	ap = &armors[0];
	while ((i -= ap -> prob) >= 0)
		ap++;
	return (ap - armors);
}

mkwep () {
	register        WEAPON wp;
	register        i = rn2 (100);
	wp = &weapons[0];
	while ((i -= wp -> prob) >= 0)
		wp++;
	return (wp - weapons);
}

char    mkobjstr[] = "))[[!!!!????%%%%//=**";

mkobj (let)
register        let;
{
	register        OBJECT otmp;

	otmp = newobj ();
	otmp -> nobj = fobj;
	fobj = otmp;
	otmp -> known = 0;
	otmp -> cursed = 0;
	otmp -> spe = 0;
	otmp -> unpaid = 0;
	otmp -> quan = 1;
	if (!let)
		let = mkobjstr[rn2 (sizeof (mkobjstr) - 1)];
	otmp -> olet = let;
	switch (let) {

		case ')': 
			otmp -> otyp = mkwep ();
			if (otmp -> otyp <= W_AMMUNITION)
				otmp -> quan = rn1 (6, 6);
			if (!rn2 (11))
				otmp -> spe = rnd (3);
			else if (!rn2 (10)) {
				otmp -> cursed = 1;
				otmp -> spe = -rnd (3);
			}
			break;

		case '*': 
			otmp -> otyp = rn2 (SIZE (potcol));
			otmp -> quan = rn2 (6) ? 1 : 2;
			break;

		case '[': 
			otmp -> otyp = mkarm ();
			if (!rn2 (8))
				otmp -> cursed = 1;
			if (!rn2 (10))
				otmp -> spe = rnd (3);
			else if (!rn2 (9)) {
				otmp -> spe = -rnd (3);
				otmp -> cursed = 1;
			}
			otmp -> spe += 10 - armors[otmp -> otyp].a_ac;
			break;

		case '!': 
			otmp -> otyp = rn2 (SIZE (pottyp));
			break;

		case '?': 
			otmp -> otyp = rn2 (SIZE (scrtyp));
			break;

		case '%': 
			otmp -> otyp = mkfood ();
			otmp -> quan = rn2 (6) ? 1 : 2;
			break;

		case '/': 
			otmp -> otyp = rn2 (SIZE (wantyp));
			if (otmp -> otyp == Z_DEATH)
				otmp -> otyp = rn2 (SIZE (wantyp));
			otmp -> spe = rn1 (5, (otmp -> otyp <= Z_CREATE_MON) ?
					11 : 4);
			break;

		case '=': 
			otmp -> otyp = rn2 (SIZE (ringtyp));
			if (otmp -> otyp >= R_GAIN_STR) {
				if (!rn2 (3)) {
					otmp -> spe = -rnd (2);
					otmp -> cursed = 1;
				}
				else
					otmp -> spe = rnd (2);
			}
			else if (otmp -> otyp == R_TELE ||
						otmp -> otyp == R_AGGRAV_MON ||
					otmp -> otyp == R_HUNGER)
				otmp -> cursed = 1;
			break;

		default: 
			panic ("impossible mkobj");
	}
}
