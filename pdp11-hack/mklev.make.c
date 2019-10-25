/*
 * Mklev.make.c
 */

#define ZOO		1
#define GRAVEYARD	2
#define SWAMP		3
#define FORT_KNOX	4
#define MAZE		6

#define NORMAL_IO
#include "hack.h"

extern char     mmon[8][8];

#define MAZX ((rnd(37) << 1) + 1)
#define MAZY ((rnd(8) << 1) + 1)

#define somex() rn1( croom->hx - croom->lx + 1, croom->lx )
#define somey() rn1( croom->hy - croom->ly + 1, croom->ly )

extern  PART levl[80][22];

extern  MONSTER fmon;
extern  OBJECT fobj;
extern  GOLD_TRAP fgold, ftrap;

extern  MKROOM rooms[15], *croom;

extern  COORDINATES doors[DOORMAX];

extern int      doorindex, nroom, comp ();

extern char     dlevel, *geno, goldseen,
                xdnstair, xupstair, ydnstair, yupstair,
                wizard, nxcor, x, y,
                dx, dy, tx, ty;
 /* For corridors and other things... */

makemaz () {

/* This is all Kenny's fault.  He seems to have his x and y reversed */

	int     xx, yy, a, q, sp, dir, dirs[5], stack[200];
	register int    zx, zy;
	register        OBJECT otmp;

	for (xx = 2; xx < 19; xx++)
		for (yy = 2; yy < 77; yy++) {
			if (xx % 2 == 0 || yy % 2 == 0)
				levl[yy][xx].typ++;/* WALL==1 */
		}
	zx = MAZY;
	zy = MAZX;
	sp = 1;
	stack[1] = 100 * zx + zy;
	while (sp) {
		xx = stack[sp] / 100;
		yy = stack[sp] % 100;
		levl[yy][xx].typ = 2;
		q = 0;
		for (a = 0; a < 4; a++)
			if (okay (xx, yy, a))
				dirs[q++] = a;
		if (q) {
			dir = dirs[rn2 (q)];
			move (&xx, &yy, dir);
			levl[yy][xx].typ = 0;
			move (&xx, &yy, dir);
			stack[++sp] = 100 * xx + yy;
		}
		else
			sp--;
	}
	for (xx = 2; xx < 77; xx++)
		for (yy = 2; yy < 19; yy++) {/* This was mine */
			if (levl[xx][yy].typ == WALL)
				levl[xx][yy].scrsym = '-';
			else {
				levl[xx][yy].typ = ROOM;
				levl[xx][yy].scrsym = '.';
			}
		}
	for (xx = rn1 (8, 11); xx; xx--) {
		mkobj (0);
		levl[(fobj -> ox = MAZX)][(fobj -> oy = MAZY)].scrsym = fobj -> olet;
	}
	for (xx = rn1 (5, 7); xx; xx--)
		makemon (1, MAZX, MAZY);
	for (xx = rn1 (6, 7); xx; xx--)
		mkgold (0, MAZX, MAZY);
	for (xx = rn1 (6, 7); xx; xx--)
		mktrap (0, 1);
	levl[(xupstair = MAZX)][(yupstair = MAZY)].scrsym = '<';
	levl[zy][zx].scrsym = '"';
	otmp = newobj ();
	otmp -> nobj = fobj;
	fobj = otmp;
	otmp -> ox = zy;
	otmp -> oy = zx;
	otmp -> olet = '"';
	xdnstair = 0;
	ydnstair = 0;
}

/* Make a trap somewhere (in croom if mazeflag=0) */
mktrap (num, mazeflag)
register int    num, mazeflag;
{
	register        GOLD_TRAP gtmp;
	register int    nopierc;
	int     nomimic, fakedoor, fakegold, tryct = 0;

	gtmp = newgen ();
	if (!num || num >= TRAPNUM) {
		nopierc = (dlevel < 4);
		nomimic = (dlevel < 9 || goldseen);
		gtmp -> gflag = rn2 (TRAPNUM - nopierc - nomimic);
/* Note: PIERC = 7, MIMIC = 8, TRAPNUM = 9 */
	}
	else
		gtmp -> gflag = num;
	fakedoor = (gtmp -> gflag == MIMIC && rn2 (2) && !mazeflag);
	fakegold = (gtmp -> gflag == MIMIC && !fakedoor);
	do {
		if (++tryct > 200) {
			printf ("tryct overflow7\n");
			free (gtmp);
			return;
		}
		if (mazeflag) {
			gtmp -> gx = MAZX;
			gtmp -> gy = MAZY;
		}
		else if (!fakedoor) {
			gtmp -> gx = somex ();
			gtmp -> gy = somey ();
		}
		else {
			if (rn2 (2)) {
				if (rn2 (2))
					gtmp -> gx = croom -> hx + 1;
				else
					gtmp -> gx = croom -> lx - 1;
				gtmp -> gy = somey ();
			}
			else {
				if (rn2 (2))
					gtmp -> gy = croom -> hy + 1;
				else
					gtmp -> gy = croom -> ly - 1;
				gtmp -> gx = somex ();
			}
		}
	} while (g_at (gtmp -> gx, gtmp -> gy, (fakegold) ? fgold : ftrap));
	if (!fakegold) {
		gtmp -> ngen = ftrap;
		ftrap = gtmp;
	}
	else {
		gtmp -> gflag = 0;
		gtmp -> ngen = fgold;
		fgold = gtmp;
		goldseen++;
	}
	if (mazeflag && !rn2 (10) && gtmp -> gflag < PIERC)
		gtmp -> gflag |= SEEN;
	if (fakedoor)
		num = '+';
	else if (fakegold)
		num = '$';
	else
		return;
	levl[gtmp -> gx][gtmp -> gy].scrsym = num;
}

mkgold (num, goldx, goldy)
register int    num;
{
	register        GOLD_TRAP gtmp;

	gtmp = newgen ();
	gtmp -> ngen = fgold;
	levl[gtmp -> gx = goldx][gtmp -> gy = goldy].scrsym = '$';
	gtmp -> gflag = (num) ? num : 1 + rnd (dlevel + 2) * rnd (30);
	fgold = gtmp;
}

/*VARARGS*/
panic (str, arg1, arg2)
register char  *str;
{
	fprintf (stderr, "\nMKLEV ERROR: ");
	fprintf (stderr, str, arg1, arg2);
	abort ();
}

makemon (sl, monx, mony)
register int    sl, monx, mony;
{
	register        MONSTER mtmp;
	register int    tryct = 0;

	mtmp = newmonst (0);
	mtmp -> mstole = STOLE_NULL;
	mtmp -> msleep = sl;
	mtmp -> mx = monx;
	mtmp -> my = mony;
	mtmp -> mfroz = 0;
	mtmp -> mconf = 0;
	mtmp -> mflee = 0;
	mtmp -> mspeed = 0;
	mtmp -> mtame = 0;
	mtmp -> angry = 0;
	mtmp -> mxlth = 0;
	mtmp -> wormno = 0;
	mtmp -> ale = 0;
	if (levl[monx][mony].typ == POOL) {
		mtmp -> ale = 1;
		mtmp -> invis = 1;
	}
	else {
		do {
			if (++tryct > 100) {
				printf ("tryct overflow8\n");
				free (mtmp);
				return;
			}
			mtmp -> mhp = rn2 (dlevel / 3 + 1) % 8;
			mtmp -> orig_hp = rn2 (7);
		} while (index (geno, mmon[mtmp -> mhp][mtmp -> orig_hp]));
	}
	mtmp -> nmon = fmon;
	fmon = mtmp;
}


char    shtypes[] = "=/)%?![";	/* 8 shoptypes: 7 specialised, 1 mixed */
char    shprobs[] = {
	3, 3, 5, 5, 10, 10, 14, 50
};				/* Their probabilities */

mkshop () {
	register        MKROOM * sroom;
	register int    sh, sx, sy, i;
	register char   let;

	for (sroom = &rooms[0];; sroom++) {
		if (sroom -> hx < 0)
			return;
		if (ch_upstairs (sroom) || ch_dnstairs (sroom))
			continue;
		if (sroom -> doorct == 1)
			break;
	}
	for (i = rn2 (100), let = 0; (i -= shprobs[let]) >= 0; let++)
		if (!shtypes[let])
			break;	/* Superfluous */
	sroom -> rtype = 8 + let;
	let = shtypes[let];
	sh = sroom -> fdoor;
	for (sx = sroom -> lx; sx <= sroom -> hx; sx++)
		for (sy = sroom -> ly; sy <= sroom -> hy; sy++) {
			if (sx == sroom -> lx && doors[sh].x == sx - 1 ||
					sx == sroom -> hx && doors[sh].x == sx + 1 ||
					sy == sroom -> ly && doors[sh].y == sy - 1 ||
					sy == sroom -> hy && doors[sh].y == sy + 1)
				continue;
			mkobj (let);
			levl[fobj -> ox = sx][fobj -> oy = sy].scrsym =
				fobj -> olet;
		}
}

mkzoo () {
	register        MKROOM * zroom;
	register int    zx, zy;

	for (;;) {
		zroom = &rooms[rn2 (nroom)];
		if (zroom -> hx < 0 || zroom -> rtype >= 8 ||
				ch_upstairs (zroom))
			continue;/* Niet in kamer met terugweg */
		break;		/* Kamer gevonden */
	}
	zroom -> rtype = ZOO;
	for (zx = zroom -> lx; zx <= zroom -> hx; zx++)
		for (zy = zroom -> ly; zy <= zroom -> hy; zy++) {
			if (!m_at (zx, zy)) {
				mkgold (0, zx, zy);
				makemon (1, zx, zy);
			}
		}
}


mkyard () {
	register        MKROOM * yroom;
	register int    yx, yy;

	for (;;) {
		yroom = &rooms[rn2 (nroom)];
		if (yroom -> hx < 0 || yroom -> rtype >= 8 ||
				ch_upstairs (yroom))
			continue;/* Niet in kamer met terugweg */
		break;		/* Kamer gevonden */
	}
	yroom -> rtype = GRAVEYARD;
	for (yx = yroom -> lx; yx <= yroom -> hx; yx++)
		for (yy = yroom -> ly; yy <= yroom -> hy; yy++) {
			if (!mymakemon (yx, yy)) {
				if (fmon -> mhp == 7)
					mymakemon (yx, yy);
			/* Not so many demons */
				mkgold (0, yx, yy);
				if (fmon -> mhp != 1) {
					mkobj (0);
					if (rn2 (8))
						fobj -> cursed = 1;
					levl[fobj -> ox = yx][fobj -> oy = yy].scrsym = fobj -> olet;
				}
			}
		}
}

#define NOMON	1
#define MON	0

mymakemon (monx, mony) {
	register        MONSTER mtmp;
	register int    tryct = 0;
	register int    tmp = 0;

	if (m_at (monx, mony))
		return NOMON;
	mtmp = newmonst (0);
	mtmp -> mstole = STOLE_NULL;
	mtmp -> msleep = 1;
	mtmp -> mx = monx;
	mtmp -> my = mony;
	mtmp -> mfroz = 0;
	mtmp -> mconf = 0;
	mtmp -> mflee = 0;
	mtmp -> mspeed = 0;
	mtmp -> mtame = 0;
	mtmp -> angry = 0;
	mtmp -> mxlth = 0;
	mtmp -> wormno = 0;
	do {
		if (++tryct > 1000) {
			printf ("tryct overflow yard\n");
			free (mtmp);
			return NOMON;
		}
		tmp = putyard (mtmp);
	} while (index (geno, mmon[mtmp -> mhp][mtmp -> orig_hp]) || tmp);
	mtmp -> nmon = fmon;
	fmon = mtmp;
	return 0;
}

putyard (mtmp)
MONSTER mtmp;
{
	switch (rn2 (5)) {
		case 0: 
			mtmp -> mhp = 1;/* level 3 */
			mtmp -> orig_hp = 6;/* ZOMBIE */
			break;
		case 1: 
			if (dlevel < 18)
				return NOMON;
			mtmp -> mhp = 6;/* level 18 */
			mtmp -> orig_hp = 2;/* VAMPIRE */
			break;
		case 2: 
			mtmp -> mhp = 4;/* level 12 */
			mtmp -> orig_hp = 5;/* Wraith */
			break;
		case 3: 
			if (dlevel < 21)
				return NOMON;
			mtmp -> mhp = 7;/* level 21 */
			mtmp -> orig_hp = 6;/* DEMON */
			break;
		case 4: 
			mtmp -> mhp = 10;/* altijd */
			mtmp -> orig_hp = 10;/* GHOST */
			break;
		default: 
			mtmp -> mhp = rn2 (dlevel / 3 + 1) % 8;
			mtmp -> orig_hp = rn2 (7);
	}
	return MON;
}

mkswamp () {
	register        MKROOM * zroom;
	register int    zx, zy;
	register int    first = 0;

	for (;;) {
		zroom = &rooms[rn2 (nroom)];
		if (zroom -> hx < 0)
			return;
		if (ch_upstairs (zroom) || ch_dnstairs (zroom))
			continue;
		if (zroom -> hx < 0 || zroom -> rtype >= 8)
			continue;
	/* Niet in kamer met terugweg of weg naar beneden */
		break;		/* Kamer gevonden */
	}
	zroom -> rtype = SWAMP;
	for (zx = zroom -> lx; zx <= zroom -> hx; zx++)
		for (zy = zroom -> ly; zy <= zroom -> hy; zy++) {
			if ((zx + zy) % 2 && !o_at (zx, zy) &&
					!g_at (zx, zy, fgold) && !m_at (zx, zy) &&
					ch_doors (zx, zy)) {
				levl[zx][zy].typ = POOL;
				levl[zx][zy].scrsym = '}';
				if (!first) {
					makemon (0, zx, zy);
					++first;
				}
				else if (!rn2 (4))
					makemon (0, zx, zy);
			}
		}
}

ch_doors (zx, zy)
register int    zx, zy;
{
	register int    xx, yy;
	register int    status = 1;

	for (xx = zx - 1; xx <= zx + 1; xx++)
		for (yy = zy - 1; yy <= zy + 1; yy++) {
			if (levl[xx][yy].typ == DOOR)
				status = 0;
			else if (levl[xx][yy].typ == SDOOR) {
				levl[xx][yy].typ = DOOR;
				levl[xx][yy].scrsym = '+';
				status = 0;
			}
		}
	return status;
}

mk_knox () {
	register        MKROOM * kroom;
	register int    kx, ky;
	register int    tmp = 10000;
	OBJECT otmp;
	MONSTER mtmp;
	GOLD_TRAP gtmp;

	for (kroom = &rooms[0];; ++kroom) {
		if (kroom -> hx < 0)
			return;
		if (ch_upstairs (kroom) || ch_dnstairs (kroom))
			continue;
		if ((kroom -> rtype >= 8 || kroom -> rtype <= SWAMP)
				&& kroom -> rtype)
				/* Shop or Zoo or Graveyard or Swamp */
			continue;
		if (kroom -> hx - kroom -> lx <= 3 || kroom -> hy - kroom -> ly <= 3)
			continue;
		break;
	}
	kroom -> rtype = FORT_KNOX;
	kx = ((kroom -> hx - kroom -> lx) / 2) + kroom -> lx;
	ky = ((kroom -> hy - kroom -> ly) / 2) + kroom -> ly;
	for (tx = kx - 1; tx <= kx + 1; ++tx)
		for (ty = ky - 1; ty <= ky + 1; ++ty) {
			if (tx == kx && ty == ky)
				continue;
			if ((otmp = o_at (tx, ty))) {
				otmp -> ox = kx;
				otmp -> oy = ky;
			}
			if ((mtmp = m_at (tx, ty)))
				delmon (mtmp);
			if ((gtmp = g_at (tx, ty, fgold))) {
				tmp += gtmp -> gflag;
				delgen (gtmp, fgold);
			}
			if ((gtmp = g_at (tx, ty, ftrap)))
				delgen (gtmp, ftrap);
			levl[tx][ty].typ = VAULT;
			levl[tx][ty].scrsym = (ty == ky) ? '|' : '-';
		}
	mkgold (tmp, kx, ky);
}

ch_upstairs (mroom)
register        MKROOM * mroom;
{
	return (mroom -> lx <= xupstair && xupstair <= mroom -> hx &&
			mroom -> ly <= yupstair && yupstair <= mroom -> hy);
}

ch_dnstairs (mroom)
register        MKROOM * mroom;
{
	return (mroom -> lx <= xdnstair && xdnstair <= mroom -> hx &&
			mroom -> ly <= ydnstair && ydnstair <= mroom -> hy);
}

delmon (mtmp)
register        MONSTER mtmp;
{
	register        MONSTER mtmp2;

	if (mtmp == fmon)
		fmon = fmon -> nmon;
	else {
		for (mtmp2 = fmon; mtmp2 -> nmon != mtmp;
				mtmp2 = mtmp2 -> nmon);
		mtmp2 -> nmon = mtmp -> nmon;
	}
}

delgen (gtmp, key)
register        GOLD_TRAP gtmp, key;
{
	register        GOLD_TRAP gtmp2;

	if (gtmp == key)
		key = key -> ngen;
	else {
		for (gtmp2 = key; gtmp2 -> ngen != gtmp;
				gtmp2 = gtmp2 -> ngen);
		gtmp2 -> ngen = gtmp -> ngen;
	}
}
