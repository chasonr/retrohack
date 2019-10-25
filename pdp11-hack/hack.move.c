/*
 * Hack.move.c
 */

#include "hack.h"

extern char     STOPGLOW[], UMISS[], *setan ();


extern  OBJECT addinv ();

char    seelx, seehx, seely, seehy;/* Corners of lit room */
 /* l for Low, h for High */

#define ZOO		1
#define GRAVEYARD	2
#define SWAMP		3
#define FORT_KNOX	4


/* Contains move commands */

char    sdir[] = "hjklyubn";
short   xdir[8] = {
	-1, 0, 0, 1, -1, 1, -1, 1
}      ,
        ydir[8] = {
	0, 1, -1, 0, -1, -1, 1, 1
};

movecm (cmd)
register char  *cmd;
{
	register char  *dp;

	if (!(dp = index (sdir, *cmd)))
		return 0;
	dx = xdir[dp - sdir];
	dy = ydir[dp - sdir];
	return 1;
}


domove () {
	char    nx, ny, tmpx, tmpy, let;
	register        MONSTER mtmp;
	register        PART * tmpr, *ust;
	GOLD_TRAP gold, trap, gtm1;
	OBJECT otmp, obj;

	if (u.uswallow) {
		nx = u.ux;
		ny = u.uy;
		goto M;
	}
 /*  n<letter> is used for u.u<letter> + d<letter>  */
	if (u.uconfused) {
		do {
			dx = rn2 (3);
			dy = rn2 (3);
			dx--;
			dy--;
			tmpr = &levl[u.ux + dx][u.uy + dy];
		}
		while ((!dx && !dy) || tmpr -> typ < DOOR);
	}
	else
		tmpr = &levl[u.ux + dx][u.uy + dy];
	ust = &levl[u.ux][u.uy];
	tmpx = u.ux;
	tmpy = u.uy;
	nx = u.ux + dx;
	ny = u.uy + dy;
	if (trap = g_at (nx, ny, ftrap)) {
		if (trap -> gflag == MIMIC) {
			nomul (0);
			pline ("The door is actually a mimic.");
			deltrap (trap);
			if (makemon (PM_MIMIC)) {
				newsym (nx, ny);
				return;
			}
			if (m_at (nx, ny))
				mnexto (fmon);
			else {
				fmon -> mx = nx;
				fmon -> my = ny;
			}
			if (!u.ustuck)
				u.ustuck = fmon;
			pmon (fmon);
			return;
		}
		if (trap -> gflag & SEEN)
			nomul (0);
	}
	if (u.ustuck && (nx != u.ustuck -> mx || ny != u.ustuck -> my)) {
		pseebl ("You cannot escape from %s!",
				u.ustuck -> data -> mname);
		nomul (0);
		return;
	}
M: 
	if (mtmp = m_at (nx, ny)) {
/* Attack monster */
		char    tmp;
		register        MONSTDATA mdat = mtmp -> data;

		tmp = u.ulevel - 1 + mdat -> ac + abon ();
		if (uwep) {
			tmp += uwep -> spe;
			if (uwep -> otyp == W_TWOH_SWORD)
				tmp--;
			else if (uwep -> otyp == W_DAGGER)
				tmp += 2;
			else if (uwep -> otyp == W_SPEAR && index ("XDne",
						mdat -> mlet))
				tmp += 2;
		}
		if (mtmp -> msleep) {
			mtmp -> msleep = 0;
			tmp += 2;
		}
		if (mtmp -> mfroz) {
			tmp += 4;
			if (!rn2 (10))
				mtmp -> mfroz = 0;
		}
		if (mtmp -> mflee)
			tmp += 2;
		if (u.utrap)
			tmp -= 3;
		if (mtmp == shopkeeper)
			setangry ();
		if (tmp < rnd (20) && !u.uswallow)
			pseebl (UMISS, mdat -> mname);
		else {
/* We hit the monster; but: it might die! */

			if (hmon (mtmp, uwep)) {
			/* 0-destroy,1-hit */
				if (!u.uswallow && !rn2 (25) &&
						mtmp -> mhp < mtmp -> orig_hp >> 1) {
/* You might be stucked at this point ! { FRED } */
					if (mtmp == u.ustuck)
						unstuck (mtmp);
					mtmp -> mflee = 1;
				}
				pseebl ("You hit %s!", mdat -> mname);
				if (u.umconf && !u.uswallow) {/* a3 */
					if (!u.ublind) {
						pline (STOPGLOW);
						pseebl ("The %s appears confused.", mdat -> mname);
					}
					mtmp -> mconf = 1;
					u.umconf = 0;
				}
				if (mtmp -> wormno)
					cutworm (mtmp, nx, ny,
							uwep -> otyp);
				switch (mdat -> mlet) {

					case 'a': 
						if (rn2 (2)) {
							pline ("You are splashed by the blob's acid!");
							losehp (rnd (6), mdat -> mname);
						}
						if (!rn2 (6) && uwep) {
							pline ("Your %s corrodes!",
									weapons[uwep -> otyp].wepnam);
							--uwep -> spe;
						}
						break;

					case 'E': 
						if (!u.ublind && rn2 (2)) {
							pline ("You are frozen by the floating eye's gaze!");
							nomul (rn1 (20, -20));
							return;
						}
						break;
				}
			}
		}
		nomul (0);
		return;
	}

/* Not attacking an animal, so we try to move  */
	if (u.utrap) {
		pline ((u.upit) ? "You are still in a pit." :
				"You are caught in a beartrap.");
		if (u.upit || (dx && dy) || !rn2 (5))
			u.utrap--;
		return;
	}
	if ((dx && dy && (tmpr -> typ == DOOR || ust -> typ == DOOR)) ||
			tmpr -> typ < DOOR || tmpr -> typ == VAULT) {
	/* 0, WALL, or SDOOR */
		flags.move = 0;
		nomul (0);
		return;
	}
	u.ux = nx;		/* u.ux+=dx; u.uy+=dy; */
	u.uy = ny;
	nx += dx;
	ny += dy;
	if (flags.run)
		if (tmpr -> typ == DOOR ||
				(xupstair == u.ux && yupstair == u.uy) ||
				(xdnstair == u.ux && ydnstair == u.uy))
			nomul (0);
	if (tmpr -> typ >= 30 && tmpr -> typ <= 41) {
		for (otmp = invent; otmp; otmp = otmp -> nobj) {
			if (otmp -> otyp == tmpr -> typ && otmp -> olet == '_') {
				pline ("The door opens.");
				doname (otmp, buf);
				pline ("The %s vanishes.", buf);
				useup (otmp);
				tmpr -> typ = DOOR;
				break;
			}
		}
		if (!otmp) {
			if (rn2 (2))
				pline ("The door is locked!");
			else
				pline ("You cannot unlock the door!");
			u.ux -= dx;
			u.uy -= dy;
			return;
		}
	}
	if (ust -> scrsym == '@') {
		newsym (tmpx, tmpy);
		oldux = tmpx;
		olduy = tmpy;
	}
	if (!u.ublind) {
		if (ust -> lit) {
			if (tmpr -> lit) {
				if (tmpr -> typ == DOOR)
					prl1 (nx, ny);
				if (ust -> typ == DOOR)
					nose1 (tmpx - dx, tmpy - dy);
			}
			else {
				unCoff (UNC, 1);
				prl1 (nx, ny);
			}
		}
		else {
			if (tmpr -> lit)
				setCon (SETC);
			else {
				prl1 (nx, ny);
				if (tmpr -> typ == DOOR) {
					if (dy) {
						prl (u.ux - 1, u.uy);
						prl (u.ux + 1, u.uy);
					}
					else {
						prl (u.ux, u.uy - 1);
						prl (u.ux, u.uy + 1);
					}
				}
			}
			nose1 (tmpx - dx, tmpy - dy);
		}
	}
	else
		newunseen (tmpx, tmpy);
	if (!multi)
		pru ();
	while (gold = g_at (u.ux, u.uy, fgold)) {
		if (!gold -> gflag) {
			pline ("The chest was a mimic!");
			if (!makemon (PM_MIMIC)) {
				mnexto (fmon);
				u.ustuck = fmon;
			}
			nomul (0);
		}
		else {
			if (u.uhcursed) {
				pline ("You cannot pick up the gold!");
				break;
			}
			if (gold -> gflag == 1)
				gold -> gflag++;/* a3 */
			pline ("%u gold pieces", gold -> gflag);
			u.ugold += gold -> gflag;
			flags.dgold = 1;
		}
		if (gold == fgold)
			fgold = fgold -> ngen;
		else {
			for (gtm1 = fgold; gtm1 -> ngen != gold;
					gtm1 = gtm1 -> ngen);
			gtm1 -> ngen = gold -> ngen;
		}
		free (gold);
		if (flags.run)
			nomul (0);
		if (u.uinvis)
			newsym (u.ux, u.uy);
	}
	while (obj = o_at (u.ux, u.uy)) {
		for (otmp = invent, let = 0; otmp; otmp = otmp -> nobj)
			let += weight (otmp);
		let += weight (obj);
		if (let > 85) {
			pline ("You can't carry anything more.");
			if (flags.run)
				nomul (0);
			break;
		}
		if (let > 80)
			pline ("You have a little trouble lifting");
		if (!(obj -> cursed))
			obj -> cursed = u.uhcursed;
		freeobj (obj);
		addtobill (obj);/* Sets obj->unpaid if necessary */
		prinv (addinv (obj));
	/* Might merge it with other objects */
		if (u.uinvis)
			newsym (u.ux, u.uy);
		if (flags.run)
			nomul (0);
	}
	if (trap) {
		nomul (0);
		if (trap -> gflag & SEEN && !rn2 (5))
			pline ("You escape a%s.",
					traps[trap -> gflag & 037]);
		else {
			trap -> gflag |= SEEN;
			switch (((trap -> gflag) & 037)) {

				case SLPTRP: 
					pline ("A cloud of gas puts you to sleep!");
					nomul (-rnd (25));
					break;

				case BEAR: 
					u.utrap = rn1 (4, 4);
					u.upit = 0;
					pline ("A bear trap closes on your foot!");
					break;

				case PIERC: 
					pline ("A piercer suddenly drops from the ceiling!");
					deltrap (trap);
					if (!makemon (PM_PIERC)) {
						mnexto (fmon);
						hitu (3, d (4, 6),
								"falling piercer");
					}
					break;

				case ARROW: 
					pline ("An arrow shoots out at you!");
					hitu (8, rnd (6), "arrow");
					break;

				case TDOOR: 
					if (!xdnstair) {
						pline ("A trap door in the ceiling opens and a rock falls on your head!");
						losehp (d (2, 10),
								"falling rock");
						break;
					}
					pline ("A trap door opens up under you!");
					if (u.ufloat || u.ustuck) {
						pline ("For some reason you don't fall in.");
						break;
					}
					more ();
					keepdogs (1);
					unCoff (COFF, 1);
					do {
						dosavelev ();
						dodown ();
						levl[u.ux][u.uy].scrsym = '<';
					} while (!rn2 (4) && xdnstair);
					land ();
					losedogs ();
					setCon (CON);
					inshop ();/* a3:zie tele */
					break;

				case DART: 
					pline ("A little dart shoots out at you!");
					if (hitu (7, rnd (3), "little dart") &&
							!rn2 (6))
						poisoned ("dart", "poison dart");
					break;

				case TELE: 
					newsym (u.ux, u.uy);
					tele ();
					break;

				case PIT: 
					if (u.ufloat) {
						pline ("A pit opens up under you!");
						pline ("You don't fall in!");
						break;
					}
					pline ("You fall into a pit!");
					u.utrap = rn1 (6, 2);
					u.upit = 1;
					losehp (rnd (6), "pit");
					break;

				default: 
					pline ("Bad(%d)trap", trap -> gflag);
					impossible ();
			}
		}
	}
	if (tmpr -> typ == DOOR && dlevel) {
		inshop ();
		switch (rooms[inroom (u.ux, u.uy)].rtype) {
			case ZOO: 
				if (!u.uinzoo) {
					pline ("Welcome to the Zoo!");
					u.uinzoo++;
				}
				break;
			case GRAVEYARD: 
				if (!u.uinyard) {
					pline ("Welcome to the Graveyard!");
					u.uinyard++;
				}
				break;
			case SWAMP: 
				if (!u.uinswamp) {
					pline ("Welcome to the Swamp!");
					u.uinswamp++;
				}
				break;
			case FORT_KNOX: 
				if (!u.uinknox) {
					pline ("Welcome to Fort Knox!");
					u.uinknox++;
				}
				break;
			default: 
				break;/* Who knows what more may come */
		}
	}
	if (tmpr -> typ == CORR) {
		if (u.uinshop)
			inshop ();/* Outside shop now */
		u.uinzoo = 0;	/* You left the Zoo ?? */
		u.uinyard = 0;	/* You left the Graveyard ?? */
		u.uinswamp = 0;	/* You left the Swamp ?? */
		u.uinknox = 0;	/* You left Fort Knox ?? */
	}
	if (tmpr -> typ == POOL)
		if (!u.ufloat) {
			pline ("You fall into a pool!");
			pline ("You can't swim!");
			pline ("You drown...");
			more ();
			killer = "Pool of water";
			done (DROWNED);
		}
}

/* Stop running if we see something interesting */
lookaround () {
	register        x, y, corrct = 0;
	register        MONSTER mtmp;

	if (u.ublind || flags.run < 2)
		return;
	for (x = u.ux - 1; x <= u.ux + 1; x++)
		for (y = u.uy - 1; y <= u.uy + 1; y++) {
			if (x == u.ux && y == u.uy)
				continue;
/* Note: we cannot call r_free: perhaps a M is hidden in the wall */
			if (!levl[x][y].typ)
				continue;
			if (mtmp = m_at (x, y))
				if (!mtmp -> mtame || (x == u.ux + dx &&
							y == u.uy + dy)) {
					nomul (0);
					return;
				}
			if (x == u.ux - dx && y == u.uy - dy)
				continue;
			if (mtmp)
				corrct++;
			else
				switch (levl[x][y].scrsym) {
					case '+': 
						if (x == u.ux || y == u.uy) {
							nomul (0);
							return;
						}
					case '0': 
						multi = 0;
						flags.run = 0;
						return;
					case '.': 
					case '|': 
					case '-': 
						break;
					case '#': 
						corrct++;
						break;
					default: 
						nomul (0);
						return;
				}
		}
	if (corrct > 1 && flags.run == 2)
		nomul (0);
}

nomul (nval)
register        nval;
{
	if (multi < 0)
		return;
	if (flags.mv)
		pru ();
	multi = nval;
	flags.mv = 0;
	flags.run = 0;
}

char   *
        parse () {
	static char     inline[80];
	register        foo;

	oldux = 0;
	olduy = 0;
	flags.move = 1;
	if (!u.uinvis)
		curs (u.ux, u.uy + 2);
	else
		home ();
	flush ();
	while ((foo = getchar ()) >= '0' && foo <= '9')
		multi += 10 * multi + foo - '0';
	if (multi) {
		multi--;
		save_cm = inline;
	}
	inline[0] = foo;
	inline[1] = 0;
	if (foo == 'f' || foo == 'F') {
		inline[1] = getchar ();
		inline[2] = 0;
	}
	if (flags.topl) {
		home ();
		cl_end ();
		flags.topl = 0;
	}
	return (inline);
}

nomove () {
	multi = 0;
	flags.move = 0;
}
