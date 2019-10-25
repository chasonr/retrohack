/*
 * Hack.do1.c
 */

#include "hack.h"

extern char     NOTHIN[], WAND[];

extern  MONSTER shopkeeper;
MONSTER vaultkeeper;

char   *wandeffect[] = {
	"magic missile",
	"bolt of fire",
	"sleep ray",
	"bolt of cold",
	"death ray",
	"bolt of confusion"
};

#define MAXLEVEL	40

char    vaultflag[MAXLEVEL];

MONSTER bhit ();



/* More various user do commands */




dozap () {
	register        OBJECT obj;
	register        MONSTER mtmp;
	char    zx, zy;
	register        num;

	if (!(obj = getobj ("/", "zap"))) {
		nomove ();
		return;
	}
	if (!obj -> spe) {
		pline (NOTHIN);
		return;
	}
	obj -> spe--;
	if (obj -> otyp <= Z_CREATE_MON) {
		switch (obj -> otyp) {

			case Z_LIGHT: 
				if (dlevel)
					litroom ();
				else
					pline (NOTHIN);
				break;

			case Z_DETEC: 
				if (!findit ())
					return;
				break;

			case Z_CREATE_MON: 
				makemon (0);
				mnexto (fmon);
				break;
		}

		if (oiden[obj -> otyp] & WANN)
			return;
		u.urexp += 10;
		oiden[obj -> otyp] |= WANN;
		return;
	}
	if (!getdir ()) {
		obj -> spe++;
		nomove ();
		return;
	}
	if (obj -> otyp <= Z_TELEPORT) {
		if (mtmp = bhit (dx, dy, rn1 (8, 6))) {
			switch (obj -> otyp) {

				case Z_EXHAUST: 
					pline ("You fight and fight and fight......");
					home ();
					flush ();
					sleep (2);
					flags.topl = 0;
					u.uhp -= mtmp -> mhp;
					if (u.uhp <= 0) {
						pseebl ("You cannot beat %s",
								mtmp -> data -> mname);
						pline ("You die....");
						done (DIED);
					}
					else {
						mtmp -> mhp = 0;
						flags.dhp = 1;
						killed (mtmp);
					}
					oiden[obj -> otyp] |= WANN;
					break;

				case Z_SLOW_MON: 
					mtmp -> mspeed = MSLOW;
					break;

				case Z_SPEED_MON: 
					mtmp -> mspeed = MFAST;
					break;

				case Z_UND_TUR: 
					if (index (" WVZ&", mtmp -> data -> mlet)) {
						mtmp -> mhp -= rnd (8);
						if (alive (mtmp))
							mtmp -> mflee = 1;
					}
					break;

				case Z_POLYMORF: 
					if (mtmp -> ale && !cansee (mtmp -> mx, mtmp -> my))
						break;
					if (mtmp == shopkeeper)
						shkdead ();/* Michiel */
					unstuck (mtmp);
					newcham (mtmp, &mon[rn2 (8)][rn2 (7)]);
					oiden[obj -> otyp] |= WANN;
					return;

				case Z_CAN: 
					mtmp -> mcan = 1;
					break;

				case Z_TELEPORT: 
					unstuck (u.ustuck);
					oiden[obj -> otyp] |= WANN;
					if (mtmp == shopkeeper)
						setangry ();/* FRED */
					else if (mtmp == vaultkeeper)
						mtmp -> angry = 1;
					rloc (mtmp);
					break;

			}
		}
		return;
	}
	if (obj -> otyp == Z_CLOSING) {
		PART * room;

		zx = u.ux + dx;
		zy = u.uy + dy;
		room = &levl[zx][zy];
		while (room -> typ >= CORR) {
			zx += dx;
			zy += dy;
			room = &levl[zx][zy];
		}
		if (room -> typ == DOOR || room -> typ == SDOOR) {
			pline ("The %sdoor closes forever.",
					room -> typ == SDOOR ? "secret " : "");
			oiden[obj -> otyp] |= WANN;
			room -> typ = WALL;
			newsym (zx, zy);
		}
		return;
	}
	if (obj -> otyp == Z_DIGGING) {
	/* This is further improved by Michiel and Fred */
		PART * room;
		int     range = 1;

		zx = u.ux + dx;
		zy = u.uy + dy;
		num = ROOM;
		for (;;) {
			if (zx < 1 || zx > 78 || zy < 1 || zy > 21) {
				zx -= dx;
				zy -= dy;
				break;
			}
			atl (zx, zy, '*');
			at (zx, zy, '*');
			++range;
			room = &levl[zx][zy];
			if (!xdnstair) {
				if (zx < 3 || zx > 76 || zy < 3 ||
						zy > 18)
					break;
				if (room -> typ == WALL) {
					room -> typ = ROOM;
					break;
				}
			}
			else if (room -> typ == POOL)
				goto check;
			else if (num == ROOM || num == 10) {
				if (room -> typ) {
					if (room -> typ == VAULT)
						vaultinit ();
					if (room -> typ != ROOM) {
						if (room -> typ == VAULT)
							room -> typ = ROOM;
						else
						if (room -> typ != CORR)
							room -> typ = DOOR;
						if (num == 10)
							break;
						num = 10;
					}
				}
				else
					room -> typ = CORR;
			}
			else {
				if (room -> typ == VAULT)
					vaultinit ();
				if (room -> typ % 4) {
				/* WALL,(S)DOOR,ROOM */
					room -> typ = DOOR;
					break;
				}
				else
					room -> typ = CORR;
			}
	check: 
			newsym (zx, zy);
			zx += dx;
			zy += dy;
		}
		while (--range) {
			newsym (zx, zy);
			if (mtmp = m_at (zx, zy))
				pmon (mtmp);
			zx -= dx;
			zy -= dy;
		}
	}
	else
		buzz (obj -> otyp, u.ux, u.uy, dx, dy);
	oiden[obj -> otyp] |= WANN;
}

hit (str, mtmp)
register char  *str;
register        MONSTER mtmp;
{
	psee (THEIT2, mtmp -> mx, mtmp -> my, "%s hits %s", str,
			mtmp -> data -> mname);
}

miss (str, mtmp)
register char  *str;
register        MONSTER mtmp;
{
	psee (THEIT2, mtmp -> mx, mtmp -> my, "%s misses %s", str,
			mtmp -> data -> mname);
}

findit () {
	char    num, lx, hx, ly, hy;
	register char   zx, zy;
	register        GOLD_TRAP gtmp, gt1;

	for (lx = u.ux; levl[lx - 1][u.uy].typ % CORR; lx--);/* typ!=0 */
 /* WALL, SDOOR, DOOR, or ROOM (see hack.h) */

	for (hx = u.ux; levl[hx + 1][u.uy].typ % 4; hx++);
	for (ly = u.uy; levl[u.ux][ly - 1].typ % 4; ly--);
	for (hy = u.uy; levl[u.ux][hy + 1].typ % 4; hy++);
	num = 0;
	for (zy = ly; zy <= hy; zy++)
		for (zx = lx; zx <= hx; zx++) {
			if (levl[zx][zy].typ == SDOOR) {
				levl[zx][zy].typ = DOOR;
				atl (zx, zy, '+');
				num++;
			}
			else if (gtmp = g_at (zx, zy, ftrap)) {
				if (gtmp -> gflag == PIERC) {
					mkmonat (PM_PIERC, zx, zy);
					num++;
					deltrap (gtmp);
				}
				else if (gtmp -> gflag == MIMIC) {
					deltrap (gtmp);
			M: 
					mkmonat (PM_MIMIC, zx, zy);
					num++;
				}
				else if (!gtmp -> gflag & SEEN) {
					gtmp -> gflag |= SEEN;
					atl (zx, zy, '^');
					num++;
				}
			}
			else if ((gtmp = g_at (zx, zy, fgold)) &&
					!gtmp -> gflag) {
				if (gtmp == fgold)
					fgold = gtmp -> ngen;
				else {
					for (gt1 = fgold; gt1 -> ngen !=
							gtmp; gt1 = gt1 -> ngen);
					gt1 -> ngen = gtmp -> ngen;
				}
				free (gtmp);
				goto M;
			}
		}
	return (num);
}

/* Sets dx,dy to the final position of the weapon thrown */
MONSTER bhit (ddx, ddy, range) {
	register        MONSTER mtmp;

	dx = u.ux;
	dy = u.uy;
	if (u.uswallow)
		return u.ustuck;/* a3 */
	while (range-- > 0) {
		dx += ddx;
		dy += ddy;
		if (mtmp = m_at (dx, dy))
			return (mtmp);
		if (levl[dx][dy].typ < CORR) {
			dx -= ddx;
			dy -= ddy;
			return (0);
		}
	}
	return (0);
}

buzz (type, sx, sy, ddx, ddy)
register        sx, sy;
{
	PART * lev;
	register char   range, let;
	register        MONSTER mtmp;
	register        wandeftype = type - 11;

	if (u.uswallow) {
		pline ("The %s rips into the %s.",
				wandeffect[wandeftype],
				u.ustuck -> data -> mname);
		zhit (u.ustuck, type);
		alive (u.ustuck);/* a3 */
		return;
	}
	range = rn1 (7, 7);
	if (ddx == ddy)
		let = '\\';
	else if (ddx && ddy)
		let = '/';
	else if (ddx)
		let = '-';
	else
		let = '|';
	while (range-- > 0) {
		sx += ddx;
		sy += ddy;
		if ((lev = &levl[sx][sy]) -> typ) {
			at (sx, sy, let);
			on (sx, sy);
			lev -> new = 1;
		}
		if (mtmp = m_at (sx, sy)) {
			if (mtmp == vaultkeeper)
				mtmp -> angry = 1;
			if (rnd (20) < 18 + mtmp -> data -> ac) {
				zhit (mtmp, type);
				if (alive (mtmp))
					hit (wandeffect[wandeftype],
							mtmp);
				range -= 2;
			}
			else
				miss (wandeffect[wandeftype], mtmp);
		}
		else if (sx == u.ux && sy == u.uy) {
			if (rnd (20) < 18 + u.uac) {
				range -= 2;
				flags.dhp = 1;/* Michiel */
				pline ("The %s hits you!",
						wandeffect[wandeftype]);
				switch (type) {
					case Z_MAG_MISSILE: 
						u.uhp -= d (2, 6);
						break;
					case Z_FIRE: 
						if (u.ufireres) {
							pline ("You don't feel hot!");
							break;
						}
						u.uhp -= d (6, 6);
						break;
					case Z_SLEEP: 
						nomul (-rnd (25));
						break;
					case Z_COLD: 
						if (u.ucoldres) {
							pline ("You don't feel cold!");
							break;
						}
						u.uhp -= d (6, 6);
						break;
					case Z_DEATH: 
						u.uhp = 0;
						break;
					case Z_CONF_MON: 
						u.uconfused = d (4, 6);
				}
				if (u.uhp <= 0)
					killer = wandeffect[wandeftype];
			}
			else
				pline ("The %s wizzes by you!",
						wandeffect[wandeftype]);
		}
		if (lev -> typ <= DOOR || lev -> typ == VAULT) {
			psee (0, sx, sy, "%s bounces!",
					wandeffect[wandeftype], NULL);
			ddx = -ddx;
			ddy = -ddy;
			range--;
		}
	}
}

zhit (mtmp, type)
register        MONSTER mtmp;
register        type;
{
	if (mtmp == shopkeeper)
		setangry ();
	switch (type) {
		case Z_MAG_MISSILE: 
			mtmp -> mhp -= d (2, 6);
			break;
		case Z_FIRE: 
			if (index ("Dg", mtmp -> data -> mlet))
				return;
			mtmp -> mhp -= d (6, 6);
			if (mtmp -> data -> mlet == 'Y')
				mtmp -> mhp -= 7;
			break;
		case Z_SLEEP: 
			mtmp -> mfroz = 1;
			break;
		case Z_COLD: 
			if (index ("Ygf", mtmp -> data -> mlet))
				return;
			if (mtmp -> data -> mlet == 'D')
				mtmp -> mhp -= 7;
			mtmp -> mhp -= d (6, 6);
			break;
		case Z_DEATH: 
			if (index ("WVZ ", mtmp -> data -> mlet))
				return;
			mtmp -> mhp = 0;
			break;
		case Z_CONF_MON: 
			if (mtmp == u.ustuck)
				return;
			mtmp -> mconf = 1;
			break;

	}
}

dowhatis () {
	register        fd;
	register char  *str;


	pline ("Specify what? ");
	flags.topl = 0;
	getlin (buf);
	str = buf;
	while (*str == ' ')
		str++;
	nomove ();
	buf[52] = '\0';
	if (*(str + 1))
		pline ("One character please.");
	else if ((fd = open (DATA, 0)) < 0)
		pline ("Cannot open data file!");
	else {
		lseek (fd, (long) (*str * 51), 0);
		if (read (fd, buf, 51) > 0 && *buf != '\\')
			pline (buf);
		else
			pline ("Unknown symbol.");
		close (fd);
	}
}

doshow () {			/* Michiel: Show everything you're wearing */
	nomove ();
	show (uarm2);
	show (uarm);
	show (uwep);
	show (uleft);
	show (uright);
}

show (otmp)
register        OBJECT otmp;
{
	if (otmp)
		prinv (otmp);
}

dosearch () {
	register char   x, y;
	register        GOLD_TRAP tgen;

	for (x = u.ux - 1; x < u.ux + 2; x++)
		for (y = u.uy - 1; y < u.uy + 2; y++)
			if (levl[x][y].typ == SDOOR && !rn2 (7)) {
				levl[x][y].typ = DOOR;
				atl (x, y, '+');
				nomul (0);
			}
			else {
				for (tgen = ftrap; tgen; tgen = tgen -> ngen)
					if (tgen -> gx == x && tgen -> gy == y &&
							(!rn2 (8) || ((!u.usearch) &&
									tgen -> gflag & SEEN))) {
						nomul (0);
						pline ("You find a%s", traps[tgen -> gflag & 037]);
						if ((tgen -> gflag & 037) ==
								PIERC) {
							deltrap (tgen);
							mkmonat (PM_PIERC, x, y);
							return;
						}
						if ((tgen -> gflag & 037) ==
								MIMIC) {
							deltrap (tgen);
							mkmonat (PM_MIMIC, x, y);
							return;
						}
						if (!(tgen -> gflag & SEEN)) {
							tgen -> gflag |= SEEN;
							atl (x, y, '^');
						}
					}
			}
}

doset () {
	pline ("Give one inventory per line? ");
	flush ();
	flags.oneline = (getchar () == 'y');
	nomove ();
}



/*
 * The whole vault was implemented by Fred and Michiel 
 *
 */


struct permonst treasurer = {
	"treasurer", '@', 15, 12, -1, 4, 8, 0
};

vaultinit () {
	GOLD_TRAP gtmp;

	if (vaultflag[dlevel])
		return;
	vaultflag[dlevel] = 1;;
	makemon (&treasurer);
	vaultkeeper = fmon;
	for (gtmp = fgold; gtmp -> gflag < 10000; gtmp = gtmp -> ngen);
	fmon -> mx = gtmp -> gx;
	fmon -> my = gtmp -> gy;
	if (!u.ublind)
		pmon (fmon);
}
