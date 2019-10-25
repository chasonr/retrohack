/*
 * Hack.main.c
 */

#include <signal.h>
#include "hack.h"

#define exception( id )	( id == uid )
#define HUISJES	2201
#define WILDE	2216

extern char    *hu_stat[4], *getenv (), *malloc (), *parse ();

int     rfile;

COORDINATES doors[DOORMAX];

PART levl[80][22];
MKROOM rooms[15];
MONSTER fmon = 0;
GOLD_TRAP fgold = 0, ftrap = 0;
FLAG flags;
YOU u;
OBJECT fobj = 0, invent, uwep, uarm,
uarm2 = 0, uright = 0, uleft = 0;

extern  OBJECT yourinvent0;
extern struct obj       mace0, uarm0;

char    nul[20];		/* Contains zeros */
/* Lock contains 'pid'.dlevel */
char    plname[10], lock[16], wizard,
        curx, cury, savx,
        xupstair, yupstair, xdnstair, ydnstair,
       *save_cm = 0, *killer, *nomvmsg, dlevel = 0,
        dx, dy, buf[BUFSZ], genocided[60],
        SAVEFILE[37] = SAVEDIR;

unsigned        moves = 1;

int     multi = 0, done1 (), hangup (), hackpid;
int     uid;

main (argc, argv)
char   *argv[];
{
	int     fd;
	char *getlogin();
	register char  *yourname = getlogin ();

	uid = getuid ();
	/*
	 * Check on UID's
	 */

	if (kantoor () && !exception (HUISJES) && !exception (WILDE)) {
		printf ("Sorry, You can't play hack now\n" );
		flush ();
		exit (0);
	}
	if (!access (LOCK, 0) && strcmp (WIZARD, yourname)) {
		printf ("Sorry, I'm busy with debugging.\nTry again later.\n");
		flush ();
		exit (0);
	}

	strncpy (plname, yourname, sizeof (plname) - 1);
#ifdef WIZARD
	while (argc > 1 && **++argv == '-') {
		switch (argv[0][1]) {
			case 'w': 
				if (!exception (HUISJES) && !exception (WILDE) && strcmp (yourname, WIZARD))
					printf ("Sorry\n");
				else {
					strcpy (plname, "wizard");
					wizard++;
				}
				break;
			default: 
				printf ("Unknown option: %s\n", *argv);
		}
		flush ();
	}
#endif WIZARD

	if (chdir (PLAYGROUND) < 0)
		panic (NOCORE, "Cannot chdir to %s!", PLAYGROUND);
	if ((rfile = open (RECORD, 2)) < 0)
		panic (NOCORE, "Can't open %s!", RECORD);
	setuid (getuid ());
	umask (0);
	srand (hackpid = getpid ());
	startup ();
	signal (SIGHUP, hangup);
	signal (SIGINT, done1);
	hackmode (ON);
	if (!wizard) {
		cls ();
		flush ();
	}
	strcat (SAVEFILE, plname);
	fd = open (SAVEFILE, 0);
	if (fd >= 0) {
		if (dorecover (fd) < 0)
			goto normalplay;
		flags.move = 0;
	}
	else {
normalplay: 
		shuffle ();
		invent = yourinvent0;
		uwep = &mace0;
		uarm = &uarm0;
		u.uac = 6;	/* 10 - uarm->spe */
		u.ulevel = 1;
		u.uhunger = 900;
		u.uhpmax = u.uhp = 12;
		u.ustrmax = u.ustr = rn2 (20) ? 16 : rn1 (7, 14);
		flags.move = 1;
		dodown ();	/* a3 */
		cls ();
		setCon (SETC);
		flags.botl = 1;
		makedog ();
	}
	for (;;) {
		if (flags.move) {
			if (!u.ufast || moves % 2 == 0) {
				if (fmon)
					movemon ();
				if (!rn2 (70)) {
					makemon (0);
					fmon -> mx = 0;
					fmon -> my = 0;
					rloc (fmon);
					seeatl (fmon -> mx, fmon -> my,
							fmon -> data -> mlet);
				}
			}
			if (u.ufast && !--u.ufast)
				pline ("You feel yourself slowing down");
			if (u.uconfused && !--u.uconfused)
				pline ("You feel less confused now");
			if (u.ublind && !--u.ublind) {
				pline ("You can see again");
				if (!u.uswallow)
					setCon (SETC);
			}
			if (u.uinvis && !--u.uinvis) {
				if (!u.uswallow)
					on (u.ux, u.uy);
				pline ("You are no longer invisible");
			}
			++moves;
			if (u.uhp <= 0) {
				pline ("You die...");
				more ();
				done (DIED);
			}
			if (u.uhp < u.uhpmax) {
				if (u.ulevel > 9) {
					if (u.uregen || moves % 3 == 0) {
						flags.dhp = 1;
						u.uhp +=
							rnd (u.ulevel - 9);
						if (u.uhp > u.uhpmax)
							u.uhp = u.uhpmax;
					}
				}
				else if (u.uregen || moves %
						(22 - (u.ulevel << 1)) == 0) {
					flags.dhp = 1;
					u.uhp++;
				}
			}
			if (u.utel && !rn2 (85))
				tele ();
			if (u.usearch)
				dosearch ();
			gethungry ();
		}
		flags.move = 1;
		if (flags.dscr && !flags.mv)
			nscr ();
		if (flags.botl)
			bot ();
		else if (flags.dgold) {
			flags.dgold = 0;
			curs (16, 24);
			curx = 21;
			printf ("%-5U", u.ugold);
		}
		if (flags.dhp) {
			flags.dhp = 0;
			curs (26, 24);
			curx = 29;
			printf ("%3d", u.uhp);
		}
		if (flags.dhpmax) {
			flags.dhpmax = 0;
			curs (30, 24);
			printf ("%d)", u.uhpmax);
			if (u.uhpmax < 100)
				putchar (' ');
			curx = (u.uhpmax < 10) ? 33 : 34;
		}
		if (flags.dac) {
			flags.dac = 0;
			curs (37, 24);
			printf ("%-3d", u.uac);
			curx = 40;
		}
		if (flags.dstr) {
			flags.dstr = 0;
			curs (46, 24);
			prustr ();
			curx = 51;
		}
		if (flags.dulev) {
			flags.dulev = 0;
			curs (57, 24);
			printf ("%2d", u.ulevel);
			curx = 59;
		}
		if (flags.dexp) {
			flags.dexp = 0;
			curs (60, 24);
			if (u.ulevel < 14)
				printf ("%-5U", u.uexp);
			else
				printf ("MAX++");
			curx = 65;
		}
		if (flags.dhs) {
			flags.dhs = 0;
			curs (71, 24);
			printf (hu_stat[u.uhs]);
			curx = 79;
		}
		if (multi < 0) {
			if (!++multi) {
				pline (nomvmsg ? nomvmsg :
						"You can move again.");
				nomvmsg = 0;
			}
		}
		else {
			if (multi) {
				lookaround ();
				if (!multi) {
					flags.move = 0;
					continue;
				}
				if (flags.mv) {
					if (multi < 80 && !--multi) {
						flags.mv = 0;
						flags.run = 0;
					}
					domove ();
				}
				else {
					--multi;
					rhack (save_cm);
				}
			}
			else
				rhack (parse ());
		}
	}
}

glo (n)
register        n;		/* Construct the string `hackpid.n' */
{
/*
	register char *tf = lock;

	while( *tf && *tf != '.' )
		tf++;
	*tf++ = '.';
	sprintf( tf, "%d", n );
*/
	sprintf (lock, "%d.%d", hackpid, n);
}

impossible () {
	pline ("Program in disorder - perhaps you'd better Quit");
}
