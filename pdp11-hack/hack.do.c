/*
 * Hack.do.c
 */

#include "hack.h"

extern char     NOTHIN[], WCLEV[], *nomvmsg;

char    WELDED[] = "The %s is welded into your hand!";

#define LETTER(ch)      ((ch >= '@' && ch <= 'Z' ) || \
						       (ch >= 'a' && ch <= 'z'))
#define CURSED		1
#define NOTCURSED       0
#define MAXLEVEL       40

#define SIZE( x ) 	sizeof( x )/sizeof( x[0] )

/* Routines to do various user commands */

doglow (num)
register        num;
{
	pline ("Your %s glows %s for a %s.", (flags.dac) ?
		armors[uarm -> otyp].armnam : weapons[uwep -> otyp].wepnam,
		(num < 0) ?
		"black" : "green",
		(num * num == 1) ?
		"moment" : "while");
}

doread () {
	register        OBJECT otmp;
	register        MONSTER mtmp, mtmp2;
	register        GOLD_TRAP gtmp;
	OBJECT otmp2, otmp3;
	int     num, zx, zy, xtmp, ytmp;

	if (!(otmp = getobj ("?", "read"))) {
		nomove ();
		return;
	}
	pline ("As you read the scroll, it disappears.");

 /* 
  Sometimes a scroll doesn't do anything
  */
	if (!rn2 (20)) {
		pline ("You can't read the dialect!");
		useup (otmp);
		return;
	}

	switch (otmp -> otyp) {

		case S_ENCH_ARMOR: 
			if (!uarm) {
				nothin (otmp);
				return;
			}
			uarm -> cursed = 0;
			uarm -> spe++;
			u.uac--;
			flags.dac = 1;
			doglow (1);
			break;

		case S_DROP: 
/*
 * New scroll instead of scroll of monster confusion. This scroll
 * drops everything you have on you. And put it somewhere on the level
 * Michiel and Fred
 */
			pline ("Bloody hell, what's going on?");
			do {
				xtmp = rn2 (80);
				ytmp = rn2 (22);
			} while (levl[xtmp][ytmp].typ != ROOM);
			useup (otmp);
			oiden[S_DROP] |= SCRN;
			doring (uleft, OFF);
		/* Checked on existence in routine */
			doring (uright, OFF);
			for (otmp2 = invent; otmp2; otmp2 = otmp3) {
				otmp3 = otmp2 -> nobj;
				if (otmp2 == uarm || otmp2 == uarm2) {
					u.uac += otmp2 -> spe;
					flags.dac = 1;
				}
				otmp2 -> ox = xtmp;
				otmp2 -> oy = ytmp;
				subfrombill (otmp2);
				otmp -> unpaid = 0;
				otmp2 -> nobj = fobj;
				fobj = otmp2;
			}
			if (u.ugold) {
				gtmp = newgen ();
				gtmp -> gx = xtmp;
				gtmp -> gy = ytmp;
				gtmp -> gflag = (unsigned) u.ugold;
				gtmp -> ngen = fgold;
				fgold = gtmp;
				u.ugold = 0L;
				flags.dgold = 1;
			}
			uright = NULL;
			uleft = NULL;
			uwep = NULL;
			invent = NULL;
			uarm = NULL;
			uarm2 = NULL;
			levl[xtmp][ytmp].scrsym = fobj -> olet;
			levl[xtmp][ytmp].new = 0;
			return;

		case S_CURSE_LEV: /* Michiel */
			pline ("Your body begins to glow black.");
			docurse ();
			u.uhcursed = 1;
			break;

		case S_REM_CURSE: 
			pline ("You feel like someone is helping you.");
			if (uleft)
				uleft -> cursed = 0;
			if (uright)
				uright -> cursed = 0;
			if (uarm)
				uarm -> cursed = 0;
			if (uarm2)
				uarm2 -> cursed = 0;
			if (uwep)
				uwep -> cursed = 0;
			break;

		case S_ENCH_WEP: 
		case S_DAM_WEP: 
			if (!uwep) {
				nothin (otmp);
				return;
			}
			num = 5 - otmp -> otyp;/* 4 or 6 */
			if (!rn2 (6))
				num <<= 1;
			uwep -> spe += num;
			if (num > 0)
				uwep -> cursed = 0;
			doglow (num);
			break;

		case S_CREAT_MON: 
			makemon (0);
			mnexto (fmon);
			break;

		case S_GENOCIDE: 
			pline ("You have found a scroll of genocide!");
			do {
				pline ("What monster do you want to genocide (Type the letter)? ");
				flags.topl = 0;
				getlin (buf);
			} while (strlen (buf) != 1 || !LETTER (*buf) ||
					 /* a3 */ index (genocided, *buf));
			strcat (genocided, buf);
			for (mtmp = fmon; mtmp; mtmp = mtmp2) {
				mtmp2 = mtmp -> nmon;
				if (mtmp -> data -> mlet == *buf)
					cmdel (mtmp);
			}
			if (*buf == '@') {
				killer = "scroll of genocide";
				u.uhp = 0;
			}
			break;

		case S_DESTR_ARMOR: 
			if (!uarm) {
				nothin (otmp);
				return;
			}
			pline ("Your armor turns to dust and falls to the floor!");
			u.uac += uarm -> spe;
			flags.dac = 1;
			useup (uarm);
			uarm = uarm2;
			uarm2 = 0;
			break;

		case S_LIGHT: 
			litroom ();
			break;

		case S_TELEPORT: 
/* 
 * Extended by Michiel and Fred:
 * One can jump between levels
 */
			pline ("The scroll turns into an elevator.");
			do {
				pline ("Which stock please? ");
				getlin (buf);
				num = atoi (buf);
			} while (num > MAXLEVEL - 3 && num != dlevel);
			if (!*buf)
				break;
			if (num <= 0)
				pline ("Don't fool around");
			else if (num == dlevel)
				tele ();
			else if (u.ufloat || u.ustuck || getinventory ("\"")) {
				nothin (otmp);
				pline ("The elevator vanishes");
				return;
			}
			else {
				home ();
				flush ();
				keepdogs (1);
				unCoff (COFF, 1);
				dosavelev ();
				if (num > dlevel) {
					dlevel = (num > maxdlevel) ?
						maxdlevel : num - 1;
					while (dlevel < num) {
						dodown ();
						levl[u.ux][u.uy].scrsym = '<';
					}
				}
				else {
					dlevel = num + 1;
					doup ();
					levl[u.ux][u.uy].scrsym = '>';
				}
				land ();
				losedogs ();
				setCon (CON);
				inshop ();/* a3: zie tele */
			}
			pline ("The elevator vanishes");
			break;

		case S_GOLD_DETEC: 
			if (!fgold) {
				nothin (otmp);
				return;
			}
			cls ();
			for (gtmp = fgold; gtmp; gtmp = gtmp -> ngen)
				at (gtmp -> gx, gtmp -> gy, '$');
			prme ();
			pline ("You feel very greedy, and sense gold!");
			more ();
			docrt ();
			break;

		case S_IDENTIFY: 
			pline ("This is an identify scroll.");
			useup (otmp);
			oiden[S_IDENTIFY] |= SCRN;
			otmp = getobj (0, "identify");
			if (otmp) {
				switch (otmp -> olet) {
					case '!': 
						oiden[otmp -> otyp] |= POTN;
						break;
					case '?': 
						oiden[otmp -> otyp] |= SCRN;
						break;
					case '/': 
						oiden[otmp -> otyp] |= WANN;
					case '[': 
					case ')': 
						otmp -> known = 1;
						break;
					case '=': 
						oiden[otmp -> otyp] |= RINN;
						if (otmp -> otyp >= R_GAIN_STR)
							otmp -> known = 1;
						break;
				}
				prinv (otmp);
			}
			return;

		case S_MAG_MAP: 
			pline ("On this scroll is a map!");
			for (zy = 0; zy < 22; zy++)
				for (zx = 0; zx < 80; zx++) {
					if ((num = levl[zx][zy].typ) == SDOOR) {
						levl[zx][zy].typ = DOOR;
						atl (zx, zy, '+');
					}
					else if ((num >= WALL && num <= CORR) &&
					/* or DOOR; no SDOOR */
							!levl[zx][zy].seen)
						newunseen (zx, zy);
					else if (num >= 30 && num <= 41)
						newunseen (zx, zy);
				}
			newunseen (xupstair, yupstair);
			if (xdnstair)/* maze */
				newunseen (xdnstair, ydnstair);
			break;

		case S_FIRE: 
			pline ("The scroll erupts in a tower of flame!");
			if (u.ufireres)
				pline ("You are uninjured.");
			else {
				num = rnd (6);
				losehp (num, "scroll of fire");
				u.uhpmax -= num;
				flags.dhpmax = 1;
			}
			break;

		default: 
			pline ("Bad(%d)scroll", otmp -> otyp);
			impossible ();

	}
	if (!(oiden[otmp -> otyp] & SCRN)) {
		if (otmp -> otyp > S_CREAT_MON && (otmp -> otyp != S_LIGHT
					|| !u.ublind)) {
			oiden[otmp -> otyp] |= SCRN;
			u.urexp += 10;
		}
		else if (!scrcall[otmp -> otyp])
			docall (otmp);
	}
	if (u.uhcursed && otmp -> otyp == S_REM_CURSE) {
		u.uhcursed = 0;
		pline ("Your body stops glowing black.");
	}
	useup (otmp);
}

litroom () {
	register        zx, zy;

	if (!xdnstair || !dlevel) {
		pline (NOTHIN);
		return;
	}
	if (levl[u.ux][u.uy].typ == CORR) {
		if (!u.ublind)
			pline ("The corridor lights up around you, then fades.");
		return;
	}
	else if (!u.ublind)
		if (levl[u.ux][u.uy].lit)
			pline ("The room lights up around you.");
		else
			pline ("The room is lit.");
	if (levl[u.ux][u.uy].lit)
		return;
	if (levl[u.ux][u.uy].typ == DOOR) {
		if (levl[u.ux][u.uy + 1].typ == ROOM)
			zy = u.uy + 1;
		else if (levl[u.ux][u.uy - 1].typ == ROOM)
			zy = u.uy - 1;
		else
			zy = u.uy;
		if (levl[u.ux + 1][u.uy].typ == ROOM)
			zx = u.ux + 1;
		else if (levl[u.ux - 1][u.uy].typ == ROOM)
			zx = u.ux - 1;
		else
			zx = u.ux;
	}
	else {
		zx = u.ux;
		zy = u.uy;
	}
	for (seelx = u.ux; levl[seelx - 1][zy].typ % 2; seelx--);
 /* ROOM or WALL or DOOR */
	for (seehx = u.ux; levl[seehx + 1][zy].typ % 2; seehx++);
	for (seely = u.uy; levl[zx][seely - 1].typ % 2; seely--);
	for (seehy = u.uy; levl[zx][seehy + 1].typ % 2; seehy++);
	for (zy = seely; zy <= seehy; zy++)
		for (zx = seelx; zx <= seehx; zx++) {
			levl[zx][zy].lit = 1;
			if (!u.ublind && dist (zx, zy) > 2)
				prl (zx, zy);
		}
}

dodrink () {
	register        OBJECT otmp, objs;
	register        MONSTER mtmp;
	register        num;

	if (!(otmp = getobj ("!", "drink"))) {
		nomove ();
		return;
	}
	switch (otmp -> otyp) {

		case P_REST_STR: 
			pline ("Wow!  This makes you feel great!");
			if (u.ustr < u.ustrmax) {
				u.ustr = u.ustrmax;
				flags.dstr = 1;
			}
			break;

		case P_BOOZE: 
			pline ("Ooph!  This tastes like liquid fire!");
			u.uconfused += d (3, 8);
			if (u.uhp < u.uhpmax)
				losehp (-1, "");
			if (!rn2 (4)) {
				pline ("You pass out.");
				multi = -rnd (15);
				nomvmsg = "You awake with a headache.";
			}
			break;

		case P_INVIS: 
			pline ("Gee!  All of a sudden, you can't see yourself.");
			newsym (u.ux, u.uy);
			u.uinvis += rn1 (15, 31);
			break;

		case P_JUICE: 
			pline ("This tastes like fruit juice.");
			lesshungry (20);
			break;

		case P_HEALING: 
			pline ("You begin to feel better.");
			num = rnd (10);
	H: 
			if (u.uhp + num > u.uhpmax) {
				u.uhp = ++u.uhpmax;
				if (otmp -> otyp == P_EXTRA_HEALING)
					u.uhp = ++u.uhpmax;
				flags.dhpmax = 1;
			}
			else
				u.uhp += num;
			flags.dhp = 1;
			if (u.ublind)
				u.ublind = 1;
			break;

		case P_FROZEN: 
			pline ("Your feet are frozen to the floor!");
			nomul (-rn1 (10, 25));
			break;

		case P_MONDETEC: 
			if (!fmon) {
				nothin (otmp);
				return;
			}
			cls ();
			for (mtmp = fmon; mtmp; mtmp = mtmp -> nmon)
				at (mtmp -> mx, mtmp -> my, mtmp -> data -> mlet);
			killer = "monster";
	P: 
			prme ();
			pline ("You sense the presence of %ss.", killer);
			more ();
			docrt ();
			break;

		case P_OBJDETEC: 
			if (!fobj) {
				nothin (otmp);
				return;
			}
			cls ();
			for (objs = fobj; objs; objs = objs -> nobj)
				at (objs -> ox, objs -> oy, objs -> olet);
			killer = "object";
			goto P;

		case P_POISON: 
			pline ("Yech! This stuff tastes like poison.");
			losestr (rn1 (4, 3));
			losehp (rnd (10), "poison potion");
			break;

		case P_CONF: 
			pline ("Huh, What?  Where am I?");
			u.uconfused += rn1 (7, 16);
			break;

		case P_GAIN_STR: 
			if (u.ustr == 118) {
				pline ("You've had enough of it!");
				break;/* Michiel */
			}
			pline ("Wow do you feel strong!");
			if (u.ustr > 17)
				u.ustr += rnd (118 - u.ustr);
			else
				u.ustr++;
			if (u.ustr > u.ustrmax)
				u.ustrmax = u.ustr;
			flags.dstr = 1;
			break;

		case P_SPEED: 
			pline ("You are suddenly moving much faster.");
			u.ufast += rn1 (10, 100);
			break;

		case P_BLIND: 
			pline ("A cloud of darkness falls upon you.");
			u.ublind += rn1 (100, 250);
			unCoff (COFF, 0);
			break;

		case P_GAIN_LEV: 
			num = rnd (10);
			u.uhpmax += num;
			u.uhp += num;
			if (u.ulevel < 14) {/* a3 */
				pline ("You feel more experienced.");
				u.uexp = (10 * pow (u.ulevel - 1)) + 1;
				pline (WCLEV, ++u.ulevel);
				flags.dulev = 1;
				flags.dexp = 1;
			}
			else
				pline ("You feel more capable.");
			flags.dhpmax = 1;
			flags.dhp = 1;
			break;

		case P_EXTRA_HEALING: 
			pline ("You feel much better.");
			num = d (2, 20) + 1;
			goto H; /* a3 */

		default: 
			pline ("Bad(%d)potion", otmp -> otyp);
			impossible ();
	}

	if (!(oiden[otmp -> otyp] & POTN)) {
		if (otmp -> otyp > P_BOOZE) {
			oiden[otmp -> otyp] |= POTN;
			u.urexp += 10;
		}
		else if (!potcall[otmp -> otyp])
			docall (otmp);
	}
	useup (otmp);
}

nothin (obj)
register        OBJECT obj;
{
	pline ("You have a strange feeling for a moment, then it passes.");
	if (obj -> olet == '?') {
		if ((!(oiden[obj -> otyp] & SCRN)) &&
				(!scrcall[obj -> otyp]))
			docall (obj);
	}
	else if ((!(oiden[obj -> otyp] & POTN)) &&
			(!potcall[obj -> otyp]))
		docall (obj);
	useup (obj);
}

dodrop () {
	register        OBJECT obj, otmp;
	register int    num;

	if (!(obj = getobj (0, "drop"))) {
		nomove ();
		return;
	}
	if (obj -> quan > 1 && !obj -> unpaid) {
		pline ("How many do you want to drop (%d max) ?",
				obj -> quan);
		getlin (buf);
		num = atoi (buf);

		if (num > obj -> quan || (num <= 0 && *buf) ||
				*buf == 0) {
			if (num != 0)
				pline ("You can't drop that %s!",
						(num > 0) ? "many" : "few");
			nomove ();
			return;
		}
		if (num != obj -> quan) {
			otmp = newobj ();
			*otmp = *obj;/* Copies whole structure */
			obj -> quan = num;
			otmp -> quan -= num;
			obj -> nobj = otmp;
		}
	}
	if (obj == uarm || obj == uarm2 ||
			obj == uright || obj == uleft) {
		pline ("You cannot drop something you are wearing.");
		nomove ();
		return;
	}
 /* (a3) i.p.v. `if( obj==uwep) uwep=0;':  */
	if (obj == uwep && uwepcursed ())
		return;
	dropit (obj);
	doname (fobj, buf);
	pline ("You dropped %s.", buf);
	subfrombill (obj);
}

gemsdrop () {
	register        OBJECT obj;
	register        counting = 0;

	for (obj = invent; obj;) {
		if (obj -> olet == '*') {
			counting += obj -> quan;
			dropit (obj);
			subfrombill (obj);
			obj = invent;
		}
		else
			obj = obj -> nobj;
	}
	if (!counting) {
		nomove ();
		pline ("You ain't got no gems, Hacker!");
	}
	else
		pline ("You dropped %d gem%s.", counting,
				counting == 1 ? "" : "s");
}

uwepcursed () {			/* a3,drop or throw uwep */
	if (uwep -> cursed) {
		multi = 0;	/* Dowield() */
		pline (WELDED, weapons[uwep -> otyp].wepnam);
		return CURSED;
	}
	uwep = 0;
	return NOTCURSED;
}

getinventory (string)
char   *string;
{
	register        OBJECT otmp;

	for (otmp = invent; otmp && !index (string, otmp -> olet); otmp =
			otmp -> nobj);
	return (otmp ? 1 : 0);
}

dropit (obj)
register        OBJECT obj;
{
	register        OBJECT otmp;

	if (obj == invent)
		invent = invent -> nobj;
	else {
		for (otmp = invent; otmp -> nobj != obj;
				otmp = otmp -> nobj);
		otmp -> nobj = obj -> nobj;
	}
	obj -> ox = u.ux;
	obj -> oy = u.uy;
	obj -> nobj = fobj;
	fobj = obj;
	if (u.uinvis)
		newsym (u.ux, u.uy);
}
