/*
 * hack.eat.c
 */

#include "hack.h"

extern char    *nomvmsg;

unsigned        starved = 0;	/* Michiel: after 30 times fainting you starve
				   to death */

char   *hu_stat[4] = {
	"        ",
	"Hungry  ",		/* Hunger texts used in main (each 8 chars
				   long) */
	"Weak    ",
	"Fainting"
};

doeat () {
	register        OBJECT otmp;
	register        FOOD ftmp;

	if (!(otmp = getobj ("%", "eat"))) {
		nomove ();
		return;
	}
	starved = 0;
	ftmp = &foods[otmp -> otyp];
	if (!rn2 (7)) {
		pline ("Blecch!  Rotten food!");
		if (!rn2 (4)) {
			pline ("You feel rather light headed.");
			u.uconfused += d (2, 4);
		}
		else if (!rn2 (4) && !u.ublind) {
			pline ("Everything suddenly goes dark.");
			u.ublind = d (2, 10);
			unCoff (COFF, 0);
		}
		else if (!rn2 (3)) {
			pline ("The world spins and goes dark.");
			nomul (-rnd (10));
			nomvmsg = "You are conscious again";
		}
		lesshungry (ftmp -> nutrition >> 2);
	}
	else {
		multi = -ftmp -> delay;
		switch (otmp -> otyp) {

			case F_FOOD: 
				if (u.uhunger < 500)
					pline ("That food really hit the spot!");
				else if (u.uhunger < 1000)
					pline ("That satiated your stomach!");
				else if (u.uhunger < 1500) {
					pline ("You're having a hard time getting all that food down.");
					multi -= 2;
				}
				else {
					pline ("You choke over your food.");
					more ();
					killer = ftmp -> foodnam;
					done (CHOKED);
				}
				lesshungry (ftmp -> nutrition);
				if (multi < 0)
					nomvmsg = "You finished your meal.";
				break;

			case F_TRIPE: 
				pline ("Yak - dog food!");
				if (rn2 (2)) {
					pline ("You vomit.");
					lesshungry (-20);
				}
				else
					lesshungry (ftmp -> nutrition);
				break;

			default: 
				pline ("That %s was delicious!", ftmp -> foodnam);
				lesshungry (ftmp -> nutrition);
				break;
		}
	}
	if (multi < 0 && !nomvmsg) {
		static char     msgbuf[40];

		sprintf (msgbuf, "You finished eating your %s.",
				ftmp -> foodnam);
		nomvmsg = msgbuf;
	}
	useup (otmp);
}

lesshungry (num)		/* Eat stg or drink fruit juice */
register        num;
{
	register        newhunger;

	newhunger = u.uhunger + num;
	if (u.uhunger < 151 && newhunger > 150) {
		if (u.uhunger < 51 && u.ustr < u.ustrmax)
			losestr (-1);
		u.uhs = 0;
		goto Ldohs;
	}
	if (u.uhunger < 51 && newhunger > 50) {
		pline ("You only feel hungry now.");
		if (u.ustr < u.ustrmax)
			losestr (-1);
		u.uhs = 1;
		goto Ldohs;
	}
	if (u.uhunger <= 0 && newhunger < 50) {
		pline ("You feel weak now.");
		u.uhs = 2;
Ldohs: 
		flags.dhs = 1;
	}
	u.uhunger = newhunger;
}

gethungry () {			/* Called in main */
	--u.uhunger;
	if ((u.uregen || u.ufeed) && moves % 2)
		u.uhunger--;
	if (u.uhunger <= 150 && !u.uhs) {
		pline ("You are beginning to feel hungry.");
		u.uhs = 1;
		goto Ldohs;
	}
	if (u.uhunger <= 50 && u.uhs == 1) {
		pline ("You are beginning to feel weak.");
		losestr (1);
		u.uhs = 2;
		goto Ldohs;
	}
	if (u.uhunger <= 0) {
		pline ("You faint from lack of food.");
		if (starved++ == 100) {
			pline ("You starve!");
			more ();
			done (STARVED);
		}
		nomul (-20);
		u.uhunger = rn1 (4, 22);
		if (u.uhs != 3) {
			u.uhs = 3;
	Ldohs: 
			flags.dhs = 1;
		}
	}
}
