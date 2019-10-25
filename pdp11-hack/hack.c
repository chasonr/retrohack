/*
 * Hack.c
 */

#include "hack.h"

extern char     news0 (), *setan ();

extern char     seelx, seehx, seely, seehy;/* Corners of lit room */
 /* l for Low, h for High */

#define CANSEE 		1
#define CANNOTSEE 	0
#define HITYOU		1
#define MISSYOU		0
#define MONALIVE	1
#define MONDEAD		0


char   *
        lowc (str)
register char  *str;
{
	if (*str >= 'A' && *str <= 'Z')
		*buf = *str + 'a' - 'A';
	else
		*buf = *str++;
	buf[1] = 0;
	return (buf);
}

/* (a3) mix van setsee() en seeon() */
setCon (setc) {			/* setc: 1-setsee, 0-seeon (we just went to a
				   new level) */
	register        x, y;
	register        MONSTER mtmp;
	int     lx, hx, ly, hy;

	if (u.ublind) {
		if (setc)
			pru ();
		else
			docrt ();
		return;
	}
	if (levl[u.ux][u.uy].lit) {
		for (seelx = u.ux; levl[seelx - 1][u.uy].lit; seelx--);
		for (seehx = u.ux; levl[seehx + 1][u.uy].lit; seehx++);
		for (seely = u.uy; levl[u.ux][seely - 1].lit; seely--);
		for (seehy = u.uy; levl[u.ux][seehy + 1].lit; seehy++);
		lx = seelx;
		hx = seehx;
		ly = seely;
		hy = seehy;
	}
	else {
		seehx = 0;
		lx = u.ux - 1;
		hx = u.ux + 1;
		ly = u.uy - 1;
		hy = u.uy + 1;
		if (setc) {
			seelx = lx;
			seehx = hx;
			seely = ly;
			seehy = hy;
		}
	}
	for (x = lx; x <= hx; x++)
		for (y = ly; y <= hy; y++) {
			if (setc)
				prl (x, y);
			else {
				if (!levl[u.ux][u.uy].lit &&
						!levl[x][y].typ)
					continue;
				levl[x][y].seen = 1;
				if (mtmp = m_at (x, y))
					pmon (mtmp);
			}
		}
	if (!setc) {
		docrt ();
		return;
	}
	if (!levl[u.ux][u.uy].lit)
		seehx = 0;	/* Seems necessary elsewhere */
	else {
		if (seely == u.uy)
			for (x = u.ux - 1; x <= u.ux + 1; x++)
				prl (x, seely - 1);
		if (seehy == u.uy)
			for (x = u.ux - 1; x <= u.ux + 1; x++)
				prl (x, seehy + 1);
		if (seelx == u.ux)
			for (y = u.uy - 1; y <= u.uy + 1; y++)
				prl (seelx - 1, y);
		if (seehx == u.ux)
			for (y = u.uy - 1; y <= u.uy + 1; y++)
				prl (seehx - 1, y);
	}
}

unCoff (unc, mode) {
 /* 
  * (a3) mix van unsee() en seeoff()
  * unc: 1-unsee, 0-seeoff
  * mode: 1-redo @ (misc movement),
  * 0-leave them (blindness (Usually))
  */

	register        x, y;
	register        PART * lev;
	int     lx, hx, ly, hy;

	if (seehx) {
		lx = seelx;
		hx = seehx;
		ly = seely;
		hy = seehy;
	}
	else {
		lx = u.ux - 1;
		hx = u.ux + 1;
		ly = u.uy - 1;
		hy = u.uy + 1;
	}
	for (x = lx; x <= hx; x++)
		for (y = ly; y <= hy; y++) {
			lev = &levl[x][y];
			if (lev -> scrsym == '@' && mode) {
				if (unc)
					newsym (x, y);
				else
					lev -> scrsym = news0 (x, y);
			}
			else if (!seehx && lev -> scrsym == '.') {
				if (mode) {
					if (unc) {
						lev -> scrsym = ' ';
						lev -> new = 1;
						on (x, y);
					}
				}
				else
					lev -> seen = 0;
			}
		}
	seehx = 0;
}

hitu (mlev, dam, name)
register        mlev, dam;
register char  *name;
{
	mlev += (u.uac - 1);
	if (multi < 0)
		mlev += 4;
	if (u.uinvis)
		mlev -= 2;
	if (mlev < rnd (20)) {
		pseebl ("%s misses", name);
		return (MISSYOU);
	}
	pseebl ("%s hits!", name);
	if (name == NULL)
		impossible ();
	losehp (dam, name);
	return (HITYOU);
}

cansee (x, y)
char    x, y;
{
	if (u.ublind || u.uswallow)
		return (CANNOTSEE);
	if (dist (x, y) < 3)
		return (CANSEE);
	if (levl[x][y].lit && seelx <= x && x <= seehx && seely <= y &&
			y <= seehy)
		return (CANSEE);
	return (CANNOTSEE);
}

long
        pow (num)
register        num;		/* Returns 2^num */
{
	return (1 << num);
}

land () {			/* a3 */
	do {
		u.ux = rn2 (80);
		u.uy = rn2 (22);
	} while (levl[u.ux][u.uy].typ != ROOM || m_at (u.ux, u.uy));
}

tele () {
	unCoff (UNC, 0);	/* Dat was een 1 (a3) */
	unstuck (u.ustuck);	/* a3 */
	u.utrap = 0;
	do
		land ();
	while (o_at (u.ux, u.uy) || g_at (u.ux, u.uy, ftrap) ||
			g_at (u.ux, u.uy, fgold));
	setCon (SETC);
	inshop ();
}

char   *
        sitoa (a)
register int    a;
{
	static char     buffer[8];

	sprintf (buffer, "+%d", a);
	return ((a < 0) ? buffer + 1 : buffer);
}

doname (obj, buffer)
register        OBJECT obj;
register char  *buffer;
{
	switch (obj -> olet) {

		case '"': 
			strcpy (buffer, "The amulet of Frobozz");
			break;

		case '%': 
			if (obj -> quan > 1)
				sprintf (buffer, "%d %ss", obj -> quan,
						foods[obj -> otyp].foodnam);
			else
				strcpy (buffer, setan (foods[obj -> otyp].foodnam));
			break;

		case ')': 
			killer = weapons[obj -> otyp].wepnam;/* a3 */
			if (obj -> known) {
				if (obj -> quan > 1)
					sprintf (buffer, "%d %s %ss", obj -> quan,
							sitoa (obj -> spe), killer);
				else
					sprintf (buffer, "a %s %s", sitoa (obj -> spe),
							killer);
			}
			else {
				if (obj -> quan > 1)
					sprintf (buffer, "%d %ss", obj -> quan,
							killer);
				else
					strcpy (buffer, setan (killer));
			}
			if (obj == uwep)
				strcat (buffer, " (weapon in hand)");
			break;

		case '[': 
			if (obj -> known)
				sprintf (buffer, "%s %s",
						sitoa (obj -> spe - 10 + armors[obj -> otyp].a_ac),
						armors[obj -> otyp].armnam);
			else
				strcpy (buffer, armors[obj -> otyp].armnam);
			if (obj == uarm || obj == uarm2)
				strcat (buffer, " (being worn)");
			break;

		case '!': 
			if (oiden[obj -> otyp] & POTN || potcall[obj -> otyp]) {
				if (obj -> quan > 1)
					sprintf (buffer, "%d potions ", obj -> quan);
				else
					strcpy (buffer, "a potion ");
				while (*buffer)
					buffer++;
				if (potcall[obj -> otyp])
					sprintf (buffer, "called %s",
							potcall[obj -> otyp]);
				else
					sprintf (buffer, "of %s",
							pottyp[obj -> otyp]);
			}
			else {
				killer = " potion";
		P: 
				if (obj -> quan > 1)
					sprintf (buffer, "%d %s%ss", obj -> quan,
							potcol[obj -> otyp], killer);
				else
					sprintf (buffer, "%s%s",
							setan (potcol[obj -> otyp]),
							killer);
			}
			break;

		case '?': 
			if (obj -> quan > 1)
				sprintf (buffer, "%d scrolls ", obj -> quan);
			else
				strcpy (buffer, "a scroll ");
			while (*buffer)
				buffer++;
			if (oiden[obj -> otyp] & SCRN)
				sprintf (buffer, "of %s", scrtyp[obj -> otyp]);
			else if (scrcall[obj -> otyp])
				sprintf (buffer, "called %s", scrcall[obj -> otyp]);
			else
				sprintf (buffer, "labeled %s", scrnam[obj -> otyp]);
			break;

		case '/': 
			if (oiden[obj -> otyp] & WANN)
				sprintf (buffer, "a wand of %s", wantyp[obj -> otyp]);
			else if (wandcall[obj -> otyp])
				sprintf (buffer, "a wand called %s",
						wandcall[obj -> otyp]);
			else
				sprintf (buffer, "%s wand",
						setan (wannam[obj -> otyp]));
			if (obj -> known) {
				while (*buffer)
					buffer++;
				sprintf (buffer, " (%d)", obj -> spe);
			}
			break;

		case '=': 
			if (oiden[obj -> otyp] & RINN) {
				if (obj -> known)
					sprintf (buffer, "a %s ring of %s",
							sitoa (obj -> spe),
							ringtyp[obj -> otyp]);
				else
					sprintf (buffer, "a ring of %s",
							ringtyp[obj -> otyp]);
			}
			else if (ringcall[obj -> otyp])
				sprintf (buffer, "a ring called %s",
						ringcall[obj -> otyp]);
			else
				sprintf (buffer, "%s ring",
						setan (rinnam[obj -> otyp]));
			if (obj == uright)
				strcat (buffer, " (on right hand)");
			if (obj == uleft)
				strcat (buffer, " (on left hand)");
			break;

		case '*': 
			killer = " gem";
			goto P;

		case '_': 
			sprintf (buffer, "%s key",
					setan (potcol[obj -> otyp - 30]));
			break;

		default: 
			sprintf (buffer, "a%dglorkum %c(0%o)%d", obj -> otyp,
					obj -> olet, obj -> olet, obj -> spe);
	}
	if (obj -> unpaid)
		strcat (buffer, " (unpaid)");
}

abon () {
	if (u.ustr == 3)
		return - 3;
	if (u.ustr < 6)
		return - 2;
	if (u.ustr < 8)
		return - 1;
	if (u.ustr < 17)
		return 0;
	if (u.ustr < 69)
		return 1;	/* up to 18/50 */
	if (u.ustr < 118)
		return 2;
	return 3;
}

dbon () {
	if (u.ustr < 6)
		return - 1;
	if (u.ustr < 16)
		return 0;
	if (u.ustr < 18)
		return 1;
	if (u.ustr == 18)
		return 2;	/* up to 18 */
	if (u.ustr < 94)
		return 3;	/* up to 18/75 */
	if (u.ustr < 109)
		return 4;	/* up to 18/90 */
	if (u.ustr < 118)
		return 5;	/* up to 18/99 */
	return 6;		/* 18/00 */
}

losestr (num)
register        num;
{
	u.ustr -= num;
	while (u.ustr < 3) {
		u.ustr++;
		u.uhp -= 6;
		u.uhpmax -= 6;
		flags.dhp = flags.dhpmax = 1;
	}
	flags.dstr = 1;
}

losehp (n, knam)
register        n;
char   *knam;
{
	u.uhp -= n;
	flags.dhp = 1;
	if (u.uhp <= 0)
		killer = knam;
}

char   *
        setan (str)
register char  *str;		/* a3 */
{
	static char     buffer[BUFSZ];

	sprintf (buffer, "a%s %s", index ("aeiou", *str) ? "n" : "", str);
	return buffer;
}

weight (obj)
register        OBJECT obj;
{
	switch (obj -> olet) {
		case '"': 
			return 2;
		case '[': 
			return 8;
		case '%': 
			if (obj -> otyp)/* Not a food ration */
		case '*': 
				return obj -> quan;
		case '?': 
			return (obj -> quan * 3);
		case '!': 
			return (obj -> quan << 1);
		case ')': 
			if (obj -> otyp == W_TWOH_SWORD)
				return 4;
			if (obj -> otyp <= W_AMMUNITION)/* darst arrows etc */
				return (obj -> quan >> 1);
		case '/': 
			return 3;
		case '_': 
		case '=': 
			return 1;
		default: 
			pline ("Weight: bad(%d) object 0%o.", obj -> otyp,
					obj -> olet);
			return 0;
	}
}

char    mlarge[] = "bCDdegIlmnoPSsTUwY',&";

hmon (monst, obj)
register        MONSTER monst;
register        OBJECT obj;
{
	register        tmp;

	if (!obj || obj == uwep && (obj -> otyp >= W_USE_AMMO ||
				obj -> otyp <= W_AMMUNITION))
		tmp = rnd (2);
	else {
		if (index (mlarge, monst -> data -> mlet)) {
			tmp = rnd (weapons[obj -> otyp].wldam);
			if (obj -> otyp == W_TWOH_SWORD)
				tmp += d (2, 6);
			else if (obj -> otyp == W_LONG_SWORD)
				tmp += rnd (4);
		}
		else {
			tmp = rnd (weapons[obj -> otyp].wsdam);
			if (obj -> otyp == W_FLAIL || obj -> otyp == W_MACE)
				tmp++;
		}
		tmp += obj -> spe;
	}
	tmp += u.udaminc + dbon ();
	if (u.uswallow) {
		if (monst -> data -> mlet == 'P')
			tmp++;
	}
	else if (tmp <= 0)
		tmp = 1;
	monst -> mhp -= tmp;
	return alive (monst);
}

alive (monst)
register        MONSTER monst;
{
	if (monst -> mhp > 0)
		return (MONALIVE);
	killed (monst);
	return (MONDEAD);
}
