/*
 * Showlevel.c
 */

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#define SHOW
#include "hack.h"

PART levl[80][22];

static char mmon[8][8] = {
	"BGHJKLr",
	"aEhiOyZ",
	"AfNpQqv",
	"bCcgjkS",
	"FoRstWw",
	"dlMmTuY",
	"IUVXxz:",
	"De'n,P&"
};

static char stairs[4], mbuf[1000], obuf[BUFSIZ];

static unsigned omoves;
static void mread (int fd, void *buf, int n);
static void show (void);
static void pch (char ch);
static void showrecord (void);
static void delrecord (void);
static void doname (char let, char *buf);
static void getret (void);

int
main (int argc, char **argv)
{
	register int    fd;
	register        MONSTER mtmp = (MONSTER) mbuf;
	char    buffer[100];
	struct stole    stmp;
	struct obj      otmp;
	char    buf[BUFSZ];
	int     xl;

	setbuf (stdout, obuf);
	if (!strcmp (argv[1], "-r")) {
		showrecord ();
		exit (0);
	}
	if (!strcmp (argv[1], "-d")) {
		delrecord ();
		exit (0);
	}
	while (--argc) {
		printf ("Next file is %s\n", argv[argc]);
		if ((fd = open (argv[argc], 0)) < 0) {
			printf ("Cannot open %s\n", argv[argc]);
			getret ();
			continue;
		}
		if (read (fd, levl, sizeof levl) != sizeof levl) {
			printf ("Error reading level\n");
			getret ();
			continue;
		}
		show ();
		mread (fd, &omoves, sizeof (unsigned));
		mread (fd, stairs, 4);
		mread (fd, &xl, sizeof (int));
		printf ("Show Monsters? (%sSpecified) ", omoves ? "Not " : "");
		fflush (stdout);
		fgets (buffer, sizeof(buffer), stdin);
		if (strchr (buffer, 'y') == NULL)
			continue;
		while (xl != -1) {
			mread (fd, mtmp, xl + sizeof (struct monst));
			mread (fd, &stmp, sizeof (struct stole));
			if (!omoves) {
				if (mtmp -> mhp == 10 && mtmp -> orig_hp == 10)
					printf ("\' \'");
				else if (mtmp -> mhp == 9 && mtmp -> orig_hp == 9)
					printf ("Ale");
				else
					printf ("%c",
							mmon[mtmp -> mhp][mtmp -> orig_hp]);
				printf (" at [%d,%d]", mtmp -> mx, mtmp -> my);
			}
			else {
				printf ("monster at [%d,%d]", mtmp -> mx, mtmp -> my);
				if (stmp.sgold || stmp.sobj) {
					printf (" stole ");
					if (stmp.sgold)
						printf ("%u goldpieces", stmp.sgold);
					for (;;) {
						mread (fd, &otmp,
								sizeof (struct obj));
						if (!otmp.olet)
							break;
						doname (otmp.olet, buf);
						printf (" %s", buf);
					}
				}
			}
			if (mtmp -> msleep)
				printf (" SLEEP");
			if (mtmp -> invis)
				printf (" INVIS");
			if (mtmp -> cham)
				printf (" CHAM");
			if (mtmp -> mspeed)
				printf (" MSPEED");
			if (mtmp -> mconf)
				printf (" MCONF");
			if (mtmp -> mflee)
				printf (" MFLEE");
			if (mtmp -> mcan)
				printf (" MCAN");
			if (mtmp -> mtame)
				printf (" TAME");
			if (mtmp -> angry)
				printf (" MANGRY");
			if (mtmp -> wormno)
				printf (" WORM[%d]", mtmp -> wormno);
			if (mtmp -> mxlth)
				printf (" +%d extra", mtmp -> mxlth);
			if (mtmp -> mfroz)
				printf (" FROZ");
			putchar ('\n');
			mread (fd, &xl, sizeof (int));
		}
		getret ();
	}
}

static void
mread (int fd, void *buf, int n)
{
	register int    nn;

	if ((nn = read (fd, buf, n)) != n) {
		printf ("error: read %d instead of %d bytes\n", nn, n);
		exit (2);
	}
}

static void
show (void)
{
	register int    i, j;

	for (j = 0; j < 22; j++)
		for (i = 0; i < 80; i++)
			pch (levl[i][j].scrsym);
	printf ("***     ");
}

static void
pch (char ch)
{
	putchar (ch ? ch : '_');
}

__attribute__((format(printf, 1, 2)))
static void
error (const char *s, ...)
{
	va_list args;
	va_start (args, s);
	vprintf (s, args);
	va_end (args);
	putchar ('\n');
	fflush (stdout);
	exit (1);
}

static char *itoa (int a);

static struct recitem {
	long    points;
	int     level, maxlvl, hp, maxhp;
	char    str[PLNAMESIZE + 1], death[DEATHSIZE + 1];
}               record;

static void
showrecord (void)
{
	register int    killed;
	register int    place = 0;
	register int    rfile;

	if ((rfile = open (RECORD, 0)) < 0)
		error ("Cannot open %s", RECORD);
	printf ("Number Points Name\n");
	while (read (rfile, &record, sizeof (struct recitem)) > 0) {
		printf ("%2d  %6ld %8s ", ++place, record.points,
			record.str);
		killed = 0;
		if (*record.death == 'e')
			printf ("escaped the dungeon [max level %d]",
				record.maxlvl);
		else {
			switch (record.death[1]) {
				case 'u': 
					printf ("quit");
					break;
				case 'h': 
					printf ("choked in his/her food");
					break;
				case 't': 
					printf ("starved");
					break;
				case 'r': 
					printf ("drowned");
					break;
				default: 
					printf ("was killed");
					killed++;
			}
			                                                        printf (" on%s level %d", killed ? "" :
				                                                        " dungeon", record.level);
			if (record.maxlvl != record.level)
				printf (" [%d]", record.maxlvl);
		}
		if (killed)
			printf (" by %s", record.death);
		putchar ('.');
		if (record.maxhp)
			printf (" Hp: %s [%d]", (record.hp > 0) ?
					itoa (record.hp) : "-", record.maxhp);
		putchar ('\n');
	}
	close (rfile);
}

static int deleted[45];

static void
delrecord (void)
{
	register int    fd, fd2;
	int     count = 0;

	printf ("Delete (Terminate with a zero): ");
	fflush (stdout);
	fd = open (RECORD, 0);
	fd2 = creat (".Temp", 0777);
	if (fd < 0 || fd2 < 0) {
		printf ("Cannot open files!\n");
		exit (2);
	}
	do {
		scanf ("%d", &count);
		++deleted[count];
	} while (count);
	fprintf (stderr, "Deleted nr");
	count = 1;
	while (read (fd, &record, sizeof (struct recitem)) >    0) {
		if (!deleted[count])
			write (fd2, &record, sizeof (struct recitem));
		else
			                                                fprintf (stderr, " %d", count);
		                                              ++count;
	}
	                                                        putc ('\n', stderr);
	close (fd);
	close (fd2);
	execl ("/bin/mv", "mv", ".Temp", RECORD, NULL);
}

static char   *
itoa (int a)
{
	static char     buf[8];

	sprintf (buf, "%d", a);
	return (buf);
}

static void
doname (char let, char *buf)
{
	switch (let) {

		case '"': 
			strcpy (buf, "The amulet of Frobozz");
			break;

		case '%': 
			strcpy (buf, "some food");
			break;

		case ')': 
			strcpy (buf, "a weapon");
			break;

		case '[': 
			strcpy (buf, "armor");
			break;

		case '!': 
			strcpy (buf, "a potion");
			break;

		case '?': 
			strcpy (buf, "a scroll");
			break;

		case '/': 
			strcpy (buf, "a wand");
			break;

		case '=': 
			strcpy (buf, "a ring");
			break;

		case '*': 
			strcpy (buf, "a gem");
			break;

		default: 
			sprintf (buf, "a glorkum %c(0%o)", let, let);
	}
}

static void
getret (void)
{
	printf ("AHit j<return>k to continue");
	fflush (stdout);
	while (getchar () != '\n');
}
