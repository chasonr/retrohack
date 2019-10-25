/*
 * Showlevel.c
 */

#define NORMAL_IO
#define SHOW
#include "hack.h"

PART levl[80][22];

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

char    stairs[4], mbuf[1000], obuf[BUFSIZ];

unsigned        omoves;
extern char    *setan ();

main (argc, argv)
int     argc;
char  **argv;
{
	register        fd;
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
		gets (buffer);
		if (strcmp (buffer, "y"))
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

mread (fd, buf, n) {
	register        nn;

	if ((nn = read (fd, buf, n)) != n) {
		printf ("error: read %d instead of %d bytes\n", nn, n);
		exit (2);
	}
}

show () {
	register        i, j;

	for (j = 0; j < 22; j++)
		for (i = 0; i < 80; i++)
			pch (levl[i][j].scrsym);
	printf ("***     ");
}

pch (ch)
char    ch;
{
	putchar (ch ? ch : '_');
}

extern char    *malloc ();

char   *
        alloc (num)
register        num;
{
	register char  *val;

	if (!(val = malloc (num)))
		error ("Cannot get %d bytes", num);
	return val;
}

error (s) {
	printf (s);
	putchar ('\n');
	fflush (stdout);
	exit (1);
}

extern char    *itoa ();

#define	NAMESIZE	 8
#define	DEATHSIZE	40

struct recitem {
	long    points;
	int     level, maxlvl, hp, maxhp;
	char    str[NAMESIZE + 1], death[DEATHSIZE + 1];
}               record;

showrecord () {
	register        killed;
	register        place = 0;
	register        rfile;

	if ((rfile = open (RECORD, 0)) < 0)
		error ("Cannot open %s", RECORD);
	printf ("Number Points Name\n");
	while (read (rfile, &record, sizeof (struct recitem)) > 0) {
		printf ("%2d  %6D %8s ", ++place, record.points,
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

int     deleted[45];

delrecord () {
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

char   *
        itoa (a)
register int    a;
{
	static char     buf[8];

	sprintf (buf, "%d", a);
	return (buf);
}

doname (let, buf)
register char   let;
register char  *buf;
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

getret () {
	printf ("AHit j<return>k to continue");
	fflush (stdout);
	while (getchar () != '\n');
}
