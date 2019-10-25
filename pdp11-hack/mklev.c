/*
 * Mklev.c
 */

#define NORMAL_IO
#include "hack.h"

char   *tfile, *tspe, **args, nul[20];

#include "mklev.svlev.c"

char    mmon[8][8] = {
	"BGHJKLr",
	"aEhiOyZ",
	"AfNpQqv",
	"bCcgjkS",
	"FoRstWw",
	"dlMmTuY",
	"IUVXxz:",
	"De'n,P&"
};

#define MAZX ((rnd(37) << 1) + 1)
#define MAZY ((rnd(8) << 1) + 1)

#define somex() rn1( croom->hx - croom->lx + 1, croom->lx )
#define somey() rn1( croom->hy - croom->ly + 1, croom->ly )

#define NOT_HERE	0

PART levl[80][22];

MONSTER fmon;
OBJECT fobj;
GOLD_TRAP fgold, ftrap;

MKROOM rooms[15], *croom, *troom;

COORDINATES doors[DOORMAX];

int     doorindex = 0, nroom, comp ();

char    dlevel, *geno, goldseen,
        xdnstair, xupstair, ydnstair, yupstair,
        wizard, nxcor, x, y,
        dx, dy, tx, ty;
 /* For corridors and other things... */

#define RUIN	5
#define MAZE	6

main (argc, argv)
char   *argv[];
{
	char    lowy, lowx;
	register unsigned       tryct;

	if (argc < 6)
		panic ("Too few arguments!!");
	args = argv;
	tfile = argv[1];
	tspe = argv[2];
	dlevel = atoi (argv[3]);
	if (dlevel >= 30)
		dlevel = 30;
	geno = argv[4];
	wizard = (*argv[5] == 'w');
	srand (getpid ());
/* for( lowy=2;lowy<19;lowy++) for( lowx=2;lowx<77;lowx++ )
	 				levl[lowx][lowy].typ = 0; * /

/*
 * a: normal;
 * b: maze;
 * n: next level will be maze
 */
	if (*tspe == 'b') {
		makemaz ();
		savelev ();
		exit (1);
	}

/* Construct the rooms */
	croom = rooms;
	tryct = 0;
	while (nroom < 7) {
		for (lowy = rn1 (3, 3); lowy < 15; lowy += rn1 (2, 4))
			for (lowx = rn1 (3, 4); lowx < 70; lowx +=
					rn1 (2, 7)) {
				if (++tryct > 10000) {
					printf ("tryct overflow\n");
					goto jumpout;
				}
				if ((lowy += rn1 (5, -2)) < 3)
					lowy = 3;
				else if (lowy > 16)
					lowy = 16;
				if (levl[lowx][lowy].typ)
					continue;
				if (maker (lowx, rn1 (9, lowx + 2), lowy,
							rn1 (4, lowy + 2)) && nroom > 13)
					goto jumpout;
			}
	}
jumpout: 
	croom -> hx = -1;

/* For each room: put things inside */
	for (croom = rooms; croom -> hx > 0; croom++) {

/* Put a sleeping monster inside */
		if (!rn2 (3))
			makemon (1, somex (), somey ());

/* Put traps and mimics inside */
		goldseen = 0;
		while (!rn2 (8 - (dlevel / 6)))
			mktrap (0, 0);
		if (!goldseen && !rn2 (3))
			mkgold (0, somex (), somey ());
		tryct = 0;
		if (!rn2 (3))
			do {
				mkobj (0);
				levl[fobj -> ox = somex ()][fobj -> oy = somey ()].scrsym = fobj -> olet;
				if (tryct++ > 100) {
					printf ("tryct overflow4\n");
					break;
				}
			} while (!rn2 (5));
	}
	tryct = 0;
	do {
		if (++tryct > 1000)
			panic ("Cannot make dnstairs\n");
		croom = &rooms[rn2 (nroom)];
		xdnstair = somex ();
		ydnstair = somey ();
	} while ((*tspe == 'n' &&
				(xdnstair % 2 == 0 || ydnstair % 2 == 0)) ||
			g_at (xdnstair, ydnstair, ftrap));
	levl[xdnstair][ydnstair].scrsym = '>';
	troom = croom;
	do {
		if (++tryct > 2000)
			panic ("Cannot make upstairs\n");
		croom = &rooms[rn2 (nroom)];
		xupstair = somex ();
		yupstair = somey ();
	} while (croom == troom || m_at (xupstair, yupstair) ||
			o_at (xupstair, yupstair) ||
			g_at (xupstair, yupstair, ftrap));
	levl[xupstair][yupstair].scrsym = '<';
	qsort (rooms, nroom, sizeof (MKROOM), comp);
	croom = rooms;
	troom = croom + 1;
	nxcor = 0;
	mkpos ();
	do
		makecor (x + dx, y + dy);
	while (croom -> hx > 0 && troom -> hx > 0);
	if ((dlevel < 10 && rn2 (2)) || (dlevel > 9 && !rn2 (3)) ||
			wizard)
		mkshop ();
	if (dlevel > 7 && !rn2 (6))
		mkzoo ();
	else if (dlevel > 11 && !rn2 (6) || wizard)
		mkyard ();
	else if ((dlevel > 18 && !rn2 (6)))
		mkswamp ();
	else if ((dlevel > 8 && !rn2 (8)) || wizard)
		mk_knox ();
	savelev ();
}

comp (xcoord, ycoord)
register        MKROOM * xcoord, *ycoord;
{
	if (xcoord -> lx < ycoord -> lx)
		return - 1;
	return (xcoord -> lx > ycoord -> lx);
}

mkpos () {
	if (troom -> hx < 0 || croom -> hx < 0)
		return;
	if (troom -> lx > croom -> hx) {
		x = croom -> hx + 1;
		dx = 1;
		tx = troom -> lx - 1;
	}
	else if (troom -> hy < croom -> ly) {
		y = croom -> ly - 1;
		dy = -1;
		dx = 0;
		ty = troom -> hy + 1;
	}
	else if (troom -> hx < croom -> lx) {
		x = croom -> lx - 1;
		dx = -1;
		tx = troom -> hx + 1;
	}
	else {
		y = croom -> hy + 1;
		dy = 1;
		dx = 0;
		ty = troom -> ly - 1;
	}
	if (dx) {
		dy = 0;
		y = croom -> ly + rn2 (croom -> hy - croom -> ly + 1);
		ty = troom -> ly + rn2 (troom -> hy - troom -> ly + 1);
	}
	else {
		x = croom -> lx + rn2 (croom -> hx - croom -> lx + 1);
		tx = troom -> lx + rn2 (troom -> hx - troom -> lx + 1);
	}
	if (levl[x + dx][y + dy].typ) {
		if (nxcor)
			newloc ();
		else {
			dodoor (x, y, croom);
			x += dx;
			y += dy;
		}
		return;
	}
	dodoor (x, y, croom);
}

dodoor (doorx, doory, aroom)
register int    doorx, doory;
register        MKROOM * aroom;
{
	register        MKROOM * broom;
	register int    tmp;

	if ((tmp = levl[doorx - 1][doory].typ) == DOOR || tmp == SDOOR)
		return;
	if ((tmp = levl[doorx + 1][doory].typ) == DOOR || tmp == SDOOR)
		return;
	if ((tmp = levl[doorx][doory - 1].typ) == DOOR || tmp == SDOOR)
		return;
	if ((tmp = levl[doorx][doory + 1].typ) == DOOR || tmp == SDOOR)
		return;
	if (levl[doorx][doory].typ != WALL || doorindex >= DOORMAX)
		return;
	if (!rn2 (8))
		levl[doorx][doory].typ = SDOOR;
	else {
		levl[doorx][doory].scrsym = '+';
		levl[doorx][doory].typ = DOOR;
	}
	aroom -> doorct++;
	broom = aroom + 1;
	if (broom -> hx < 0)
		tmp = doorindex;
	else
		for (tmp = doorindex; tmp > broom -> fdoor; tmp--) {
			doors[tmp].x = doors[tmp - 1].x;
			doors[tmp].y = doors[tmp - 1].y;
		}
	doorindex++;
	doors[tmp].x = doorx;
	doors[tmp].y = doory;
	for (; broom -> hx >= 0; broom++)
		broom -> fdoor++;
}

newloc () {
	register int    tryct = 0;

	++croom;
	++troom;
	if (nxcor || croom -> hx < 0 || troom -> hx < 0) {
		if (nxcor++ > rn1 (nroom, 4)) {
			croom = &rooms[nroom];
			return;
		}
		do {
			if (++tryct > 100) {
				printf ("tryct overflow5\n");
				croom = &rooms[nroom];
				return;
			}
			croom = &rooms[rn2 (nroom)];
			troom = &rooms[rn2 (nroom)];
		}
		while (croom == troom || (troom == croom + 1 &&
					!rn2 (3)));
	}
	mkpos ();
}

move (xdir, ydir, dir)
register int   *xdir, *ydir, dir;
{
	switch (dir) {
		case 0: 
			--(*xdir);
			break;
		case 1: 
			(*ydir)++;
			break;
		case 2: 
			(*xdir)++;
			break;
		case 3: 
			--(*ydir);
			break;
	}
}

okay (xx, yy, dir)
int     xx, yy;
register int    dir;
{
	move (&xx, &yy, dir);
	move (&xx, &yy, dir);
	if (xx < 3 || yy < 3 || xx > 17 || yy > 75 || levl[yy][xx].typ != 0)
		return 0;
	return 1;
}

maker (lowx, hix, lowy, hiy)
char    lowx, hix, lowy, hiy;
{
	register        PART * ltmp, *lmax;
	register int    tmpx;

	if (hix > 75)
		hix = 75;
	if (hiy > 18)
		hiy = 18;
	for (tmpx = lowx - 4; tmpx < hix + 5; tmpx++)
		for (ltmp = &levl[tmpx][lowy - 3],
				lmax = &levl[tmpx][hiy + 4];
				ltmp != lmax; ltmp++)
			if (ltmp -> typ)
				return 0;
	if (10 > rnd (dlevel)) {
		for (tmpx = lowx - 1; tmpx < hix + 2; tmpx++)
			for (ltmp = &levl[tmpx][lowy - 1],
					lmax = &levl[tmpx][hiy + 2];
					ltmp != lmax; ltmp++)
				ltmp -> lit = 1;
	}
	croom -> lx = lowx;
	croom -> hx = hix;
	croom -> ly = lowy;
	croom -> hy = hiy;
	croom -> rtype = 0;
	croom -> doorct = 0;
	croom -> fdoor = 0;
	croom++;
	for (tmpx = lowx - 1; tmpx <= hix + 1; tmpx++) {
		ltmp = &levl[tmpx][lowy - 1];
		lmax = &levl[tmpx][hiy + 1];
		ltmp -> scrsym = '-';
		ltmp -> typ = WALL;
		while (++ltmp != lmax) {
			if (tmpx >= lowx && tmpx <= hix) {
				ltmp -> scrsym = '.';
				ltmp -> typ = ROOM;
			}
			else {
				ltmp -> scrsym = '|';
				ltmp -> typ = WALL;
			}
		}
		ltmp -> scrsym = '-';
		ltmp -> typ = WALL;
	}
 /* 
  -------
  |.....|
  |.....|		(This sort of room is made) (a3)
  |.....|
  -------
  */
	++nroom;
	return 1;
}

makecor (nx, ny)
register int    nx, ny;
{
	register        PART * crm;
	register int    dix, diy;

	if (nxcor && !rn2 (35)) {
		newloc ();
		return;
	}
	dix = abs (nx - tx);
	diy = abs (ny - ty);
	if (nx == 79 || !nx || !ny || ny == 21) {
		if (nxcor) {
			newloc ();
			return;
		}
		printf ("something went wrong. we try again...\n");
		execl ("./mklev", args[0], tfile, tspe, args[3],
				args[4], 0);
		panic ("cannot execute ./mklev\n");
	}
	if (dy && dix > diy) {
		dy = 0;
		dx = (nx > tx) ? -1 : 1;
	}
	else if (dx && diy > dix) {
		dx = 0;
		dy = (ny > ty) ? -1 : 1;
	}
	crm = &levl[nx][ny];
	if (!crm -> typ) {
		crm -> typ = CORR;
		crm -> scrsym = '#';
		x = nx;
		y = ny;
		return;
	}
	if (crm -> typ == CORR) {
		x = nx;
		y = ny;
		return;
	}
	if (nx == tx && ny == ty) {
		dodoor (nx, ny, troom);
		newloc ();
		return;
	}
	if (x + dx != nx || y + dy != ny)
		return;
	if (dx) {
		dy = 1;
		if (ty < ny || levl[nx + dx][ny - 1].typ != ROOM)
			dy = -1;
		dx = 0;
	}
	else {
		dx = 1;
		if (tx < nx || levl[nx - 1][ny + dy].typ != ROOM)
			dx = -1;
		dy = 0;
	}
}

MONSTER m_at (monx, mony)
register int    monx, mony;
{
	register        MONSTER mtmp;

	for (mtmp = fmon; mtmp; mtmp = mtmp -> nmon)
		if (mtmp -> mx == monx && mtmp -> my == mony)
			return (mtmp);
	return (NOT_HERE);
}

OBJECT o_at (objx, objy)
register int    objx, objy;
{
	register        OBJECT obj;

	for (obj = fobj; obj; obj = obj -> nobj)
		if (obj -> ox == objx && obj -> oy == objy)
			return (obj);
	return (NOT_HERE);
}

GOLD_TRAP g_at (gtx, gty, ptr)
register int    gtx, gty;
register        GOLD_TRAP ptr;
{
	while (ptr) {
		if (ptr -> gx == gtx && ptr -> gy == gty)
			return (ptr);
		ptr = ptr -> ngen;
	}
	return NOT_HERE;
}
