/*
 * Hack.pri.c
 */

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <term.h>
#include "hack.h"

static char scrlx, scrhx, scrly, scrhy;

static char xcurses[200];		/* Contain's curser stuff */
static char *HO, *CL, *CE, *CM, *UP, *BC;
static char PC;
static int putch (int c);

static COORDINATES ou = {
	-1, 0
};				/* Coordinates of @ on screen (if ou.x>=0) */

#ifdef NORMAL_IO
static char obuf[BUFSIZ];
#endif /* NORMAL_IO */
static void donscrt (int mode, int umv);

void
startup (void)
{
	char   *bp = malloc (1024);
	char   *atcurs = xcurses;

	if (tgetent (bp, getenv ("TERM")) <= 0)
		panic (NOCORE, "I know about many terminals but alas, not this one\n");
	PC = tgetflag ("pc");
	HO = tgetstr ("ho", &atcurs);
	CL = tgetstr ("cl", &atcurs);
	CE = tgetstr ("ce", &atcurs);
	UP = tgetstr ("up", &atcurs);
	if (!(BC = tgetstr ("bc", &atcurs))) {
		if (!tgetflag ("bs"))
			panic (NOCORE,
					"You don't have a backspaced terminal\n");
		BC = (char *) NULL;
	}
	if (!(CM = tgetstr ("cm", &atcurs)))
		panic (NOCORE, "Hack needs cursor addressible terminals\n");
	else if (!UP || !CL || tgetflag ("os"))
		panic (NOCORE, "Hack needs `up' and `cl' and no `os'\n");
	free (bp);
#ifdef NORMAL_IO
	setbuf (stdout, obuf);
#endif /* NORMAL_IO */
}

int
panic (int coredump, const char *str, ...)
{
	va_list args;

	home ();
#ifdef NORMAL_IO
	printf ("ERROR:  ");
#else
	WRITE ("ERROR:  ", 10);
#endif /* NORMAL_IO */
	va_start (args, str);
	vprintf (str, args);
	va_end (args);
	hackmode (OFF);
	if (!unlink (SAVEFILE))
		printf ("Savefile removed.\n");
	flush ();
	if (coredump)
		abort ();
	exit (2);
}

void
seeatl (int x, int y, int c)
{
	if (cansee (x, y))
		atl (x, y, c);
}

void
cls (void)
{
	tputs (CL, 0, putch);
	curx = 1;
	cury = 1;
	ou.x = -1;
	flags.topl = 0;
}

void
home (void)
{
	if (HO)
		tputs (HO, 0, putch);
	else
		tgoto (CM, 0, 0);
	curx = 1;
	cury = 1;
}

void
atl (int x, int y, int ch)
{
	register        PART * crm = &levl[x][y];

	if (crm -> scrsym == ch)
		return;
	if (x < 0 || x > 79 || y < 0 || y > 21)
		panic (CORE, "atl(%d,%d,%c_%d_)", x, y, ch, ch);
	crm -> scrsym = ch;
	crm -> new = 1;
	on (x, y);
}

void
on (int x, int y)
{
	if (flags.dscr) {
		if (x < scrlx)
			scrlx = x;
		else if (x > scrhx)
			scrhx = x;
		if (y < scrly)
			scrly = y;
		else if (y > scrhy)
			scrhy = y;
	}
	else {
		flags.dscr = 1;
		scrlx = scrhx = x;
		scrly = scrhy = y;
	}
}

void
at (int x, int y, char ch)
{
	if (!ch || x < 0 || x > 79 || y < 0 || y > 21)
		panic (CORE, "at(%d %d,%d) at %d %d", x, y, ch,
				u.ux, u.uy);
	y += 2;
	curs (x, y);
	putchar (ch == '\t' ? ' ' : ch);
	curx++;
}

void
prme (void)
{
	if (!u.uinvis)
		at (u.ux, u.uy, '@');
}

void
pru (void)
{
	prl (u.ux, u.uy);
}

void
prl (int x, int y)
{
	register        PART * room;
	register        MONSTER mtmp;

	room = &levl[x][y];
	if (!room -> typ || (room -> typ < DOOR &&
				levl[u.ux][u.uy].typ == CORR))
		return;
	if ((mtmp = m_at (x, y)) && (!mtmp -> invis || u.ucinvis))
		atl (x, y,
				(mtmp -> wormno && (mtmp -> mx != x || mtmp -> my != y)) ?
				'~' :
				mtmp -> data -> mlet);
	else
		newunseen (x, y);
}

void
newunseen (int x, int y)
{
	if (!levl[x][y].seen) {
		levl[x][y].new = 1;
		on (x, y);
	}
}

char
news0 (int x, int y)
{
	register        OBJECT otmp;
	register        GOLD_TRAP gtmp;
	PART * room;
	register char   tmp;

	room = &levl[x][y];
	if (!u.ublind && (otmp = o_at (x, y)))
		tmp = otmp -> olet;
	else if (!u.ublind && g_at (x, y, fgold))
		tmp = '$';
	else if ((gtmp = g_at (x, y, ftrap)) != NULL) {
		if (gtmp -> gflag == MIMIC)
			tmp = '+';
		else if (gtmp -> gflag & SEEN)
			tmp = '^';
		else
			tmp = '.';
	}
	else if (room -> typ >= 30 && room -> typ <= 41)
		tmp = '0';
	else
		switch (room -> typ) {

			case SDOOR: 
			case WALL: 
				if ((room - 1) -> typ == WALL && (room + 1) -> typ
						== WALL)
					tmp = '|';
				else
					tmp = '-';
				break;

			case DOOR: 
				tmp = '+';
				break;

			case ROOM: 
				if (x == xupstair && y == yupstair)
					tmp = '<';
				else if (x == xdnstair && y == ydnstair)
					tmp = '>';
				else if (room -> lit || cansee (x, y) || u.ublind)
					tmp = '.';
				else
					tmp = ' ';
				break;

			case CORR: 
				tmp = '#';
				break;

			case POOL: 
				tmp = '}';
				break;
			case VAULT: 
				tmp = '-';
				break;

			default: 
				tmp = '`';
				impossible ();
		}
	return tmp;
}

void
newsym (int x, int y)
{
	atl (x, y, news0 (x, y));
}

void
levlsym (int x, int y, int c)
{
	if (levl[x][y].scrsym == c)
		newsym (x, y);
}

static void
nosee (int x, int y)
{
	register        PART * room;

	room = &levl[x][y];
	if (room -> scrsym == '.' && !room -> lit && !u.ublind) {
		if (room -> new && (x != oldux || y != olduy))
			room -> new = 0;
		else {
			room -> scrsym = ' ';
			room -> new = 1;
			on (x, y);
		}
	}
}

void
prl1 (int x, int y)
{
	register int    count;

	if (dx) {
		if (dy) {
			prl (x - (dx << 1), y);
			prl (x - dx, y);
			prl (x, y);
			prl (x, y - dy);
			prl (x, y - (dy << 1));
		}
		else
			for (count = -1; count <= 1; ++count)
				prl (x, y + count);
	}
	else
		for (count = -1; count <= 1; ++count)
			prl (x + count, y);
}

void
nose1 (int x, int y)
{
	register int    count;

	if (dx)
		if (dy) {
			nosee (x, u.uy);
			nosee (x, u.uy - dy);
			nosee (x, y);
			nosee (u.ux - dx, y);
			nosee (u.ux, y);
		}
		else
			for (count = -1; count <= 1; ++count)
				nosee (x, y + count);
	else
		for (count = -1; count <= 1; ++count)
			nosee (x + count, y);
}

void
doreprint (void)
{
	nomove ();
	pline ("\200");		/* Michiel: Code for repeating last message */
}

void
pline (const char *line, ...)
{
	char    pbuf[BUFSZ];
	static char     prevbuf[BUFSZ];

	if (strchr (line, '\200'))
		strcpy (pbuf, prevbuf);
	else {
		if (!strchr (line, '%'))
			strcpy (pbuf, line);
		else {
			va_list args;
			va_start (args, line);
			vsprintf (pbuf, line, args);
			va_end (args);
		}
		if (multi && !strcmp (pbuf, prevbuf))
			return;
		strcpy (prevbuf, pbuf);
	}
	if (flags.dscr)
		nscr ();
	if (flags.topl) {
		curs (savx, 1);
		more ();
	}
	flags.topl = 1;
	home ();
	cl_end ();
	printf ("%s", pbuf);
	savx = strlen (pbuf);
	curx = ++savx;
}

void
prustr (void)
{
	if (u.ustr > 18) {
		if (u.ustr > 117)
			printf ("18/00");
		else
			printf ("18/%02d", u.ustr - 18);
	}
	else
		printf ("%-5d", u.ustr);
	curx += 5;
}

void
pmon (MONSTER mtmp)
{
	if (!mtmp -> invis || u.ucinvis)
		seeatl (mtmp -> mx, mtmp -> my, mtmp -> data -> mlet);
}

void
docrt (void)
{
	cls ();
	if (u.uswallow) {
		curs (u.ux - 1, u.uy - 1);
		printf ("/-\\");
		curs (u.ux - 1, u.uy);
		printf ("|@|");
		curs (u.ux - 1, u.uy + 1);
		printf ("\\-/");
		curx = u.ux + 2;
	}
	else
		donscrt (0, 0);	/* a3 */
	bot ();
}

void
nscr (void)
{
	register int    umv;

	umv = ((ou.x < 0 && !u.uinvis) || (ou.x >= 0 &&
				(u.uinvis || ou.x != u.ux || ou.y != u.uy)));
	if (ou.x >= 0 && umv && !levl[ou.x][ou.y].new)
		newsym (ou.x, ou.y);
	donscrt (1, umv);
}

static void
donscrt (int mode, int umv)		/* mode: 0- docrt(), 1- nscr()  */
{
	register        PART * room;
	register int    x, y, ly, hy, lx, hx;

	if (u.uinvis) {
		if (mode)
			ou.x = -1;
	}
	else {
		ou.x = u.ux;
		ou.y = u.uy;
		if (mode && umv)
			atl (ou.x, ou.y, '@');
		else {
			(room = &levl[ou.x][ou.y]) -> scrsym = '@';
			if (!mode)
				room -> seen = 1;
			else
				room -> new = 0;
		}
	}
	if (mode) {
		ly = scrly;
		hy = scrhy;
		lx = scrlx;
		hx = scrhx;
	}
	else {
		ly = 0;
		hy = 21;
		lx = 0;
		hx = 79;
	}
	for (y = ly; y <= hy; y++)
		for (x = lx; x <= hx; x++)
			if ((room = &levl[x][y]) -> new) {
				room -> new = 0;
				at (x, y, room -> scrsym);
				if (room -> scrsym == ' ') {
					room -> seen = 0;
					room -> scrsym = '.';
				}
				else
					room -> seen = 1;
			}
			else if (!mode)
				if (room -> seen)
					at (x, y, room -> scrsym);
	flags.dscr = 0;
	scrhx = 0;
	scrhy = 0;
	scrlx = 80;
	scrly = 22;
}

void
bot (void)
{
	flags.botl = 0;
	flags.dhp = 0;
	flags.dhpmax = 0;
	flags.dac = 0;
	flags.dstr = 0;
	flags.dgold = 0;
	flags.dhs = 0;
	curs (1, 24);
	printf ("Level %-4dGold %-7ldHp%4d(%d)",
			dlevel, u.ugold, u.uhp, u.uhpmax);
	if (u.uhpmax < 10)
		printf ("  ");
	else if (u.uhpmax < 100)
		putchar (' ');
	printf ("Ac %-5dStr ", u.uac);
	prustr ();
	printf ("  Exp%3d/", u.ulevel);
	if (u.ulevel < 14)
		printf ("%-11ld", u.uexp);
	else
		printf ("%-11s", "MAX++");
	printf ("%s", hu_stat[u.uhs]);
	curx = 78;
}

void
curs (int x, int y)
{
	if (y == cury && x == curx)
		return;		/* Do nothing, gracefully */
	tputs (tgoto (CM, x - 1, y - 1), 0, putch);
	flush ();
	cury = y;
	curx = x;
}

void
cl_end (void)
{
	if (CE)
		tputs (CE, 0, putch);
	else {
		printf ("%80s", "");
		home ();
	}
}

static int
putch (int c)
{
#ifdef NORMAL_IO
	putchar (c);
#else
    char ch = (char) c;
	WRITE (&ch, 1);
#endif /* NORMAL_IO */
	return 0;
}
