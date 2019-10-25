/*
 * Hack.end.c
 */

#include "hack.h"
#include <signal.h>

#define MAXLEVEL	40

extern char     plname[], *itoa (), *setan ();

extern int      billct, rfile;

char    maxdlevel = 0, *strcat ();

char   *statxx[] = {
	"choked",
	"died",
	"starved",
	"drowned",
	"quit",
	"escaped"
};

done1 () {
	register char   c;

	nomove ();
	signal (SIGINT, done1);
	pline ("Really quit?");
	flush ();
	if ((c = getchar ()) == 'y')
		done (QUIT);
	else if (c == 'S')
		hangup ();
}

done (status)
register int    status;
{
	if (wizard && status != QUIT && status != ESCAPED) {
		u.ustr = (u.ustrmax += 2);
		u.uhp = (u.uhpmax += 10);
		u.uac--;
		if (uwep)
			uwep -> spe++;
		pline ("For some reason you are still alive.");
		nomove ();
		flags.botl = 1;
		return;
	}
	if (status == QUIT && u.uhp <= 0)
		status = DIED;
	if (billct)
		paybill ();
	clearlocks ();
	if (status < QUIT) {	/* Not when quit or escaped */
#ifndef DEBUG
		savebones ();
#endif DEBUG
		outrip ();
	}
	hackmode (OFF);
	cls ();
	printf ("Goodbye %s...\n\n", plname);
	u.urexp += u.ugold;
	if (status == DIED) {
		strcpy (killer, setan (killer));
		u.urexp -= u.ugold / 10;
	}
	else
		killer = statxx[status];
	if (status == ESCAPED) {
		OBJECT otmp;

		u.urexp += 150;
		for (otmp = invent; otmp; otmp = otmp -> nobj) {
			if (otmp -> olet == '*')
				u.urexp += otmp -> quan * 10 * rnd (250);
			else if (otmp -> olet == '"')
				u.urexp += 25000;
		}
		printf ("You escaped from the dungeon");
	}
	else
		printf ("You %s on dungeon level %d,", statxx[status],
				dlevel);
	printf (" with %U points\n", u.urexp);
	printf ("and %U pieces of gold, after %u moves.\n",
			u.ugold, moves);
	printf ("You were level %d with a maximum of %d hit points when you %s.\n\n", u.ulevel, u.uhpmax, statxx[status]);
	topten ();
	flush ();
	exit (0);
}

#define	NAMESIZE	 8
#define	DEATHSIZE	40
#define TOPPRINT	15	/* Aantal scores dat wordt afgedrukt */
#define	TOPLIST		25	/* Length of 'top ten' list */

topten () {
	int     tmp;
	struct recitem {
		long    points;
		int     level, maxlvl, hp, maxhp;
		char    str[NAMESIZE + 1], death[DEATHSIZE + 1];
	}               rec[TOPLIST + 1], *t1;
	register        flg;

	for (t1 = rec; t1 < &rec[TOPLIST]; t1++)
		if (read (rfile, t1, sizeof (struct recitem)) <= 0)
			                                                t1 -> points = 0;
	flg = 0;
	if (u.urexp > rec[TOPLIST - 1].points && !wizard) {
		signal (SIGINT, SIG_IGN);
		if (u.urexp > rec[TOPPRINT - 1].points)
			printf ("You made the top %d list!\n", TOPPRINT);
		if (lseek (rfile, 0L, 0) < 0)
			panic (CORE, "Cannot lseek on record file");

/* Stick in new entry. NB: we save the last few
			entries that disappeared from the list */

		for (tmp = TOPLIST - 2; tmp >= 0 && rec[tmp].points <
				u.urexp; tmp--)
			rec[tmp + 1] = rec[tmp];
		tmp++;		/* Point to right place */
		rec[tmp].points = u.urexp;
		rec[tmp].level = dlevel;
		rec[tmp].maxlvl = maxdlevel;
		rec[tmp].hp = u.uhp;
		rec[tmp].maxhp = u.uhpmax;
		strncpy (rec[tmp].str, plname, NAMESIZE);
		rec[tmp].str[NAMESIZE] = 0;
		strncpy (rec[tmp].death, killer, DEATHSIZE);
		rec[tmp].death[DEATHSIZE] = 0;
		flg++;
	}
	printf ("Number Points Name\n");
	for (t1 = rec, tmp = 1; t1 < &rec[TOPLIST]; tmp++, t1++) {
		char    killed = 0;

		if (flg && t1 -> points)
			write (rfile, t1, sizeof (struct recitem));
		if (t1 >= &rec[TOPPRINT] || t1 -> points == 0)
			continue;
		printf ("%2d  %6D %8s ", tmp, t1 -> points,
				t1 -> str);
		if (*t1 -> death == 'e')
			printf ("escaped the dungeon [max level %d]", t1 -> maxlvl);
		else {
			switch (t1 -> death[1]) {
				case 'u': 
					printf ("quit");
					break;
				case 'h': 
					printf ("choked on his food");
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
			printf (" on%s level %d",
					killed ? "" : " dungeon", t1 -> level);
			if (t1 -> maxlvl != t1 -> level)
				printf (" [%d]", t1 -> maxlvl);
		}
		if (killed)
			printf (" by %s", t1 -> death);
		putchar ('.');
		if (t1 -> maxhp)
			printf (" Hp: %s [%d]", (t1 -> hp > 0) ?
					itoa (t1 -> hp) : "-", t1 -> maxhp);
		putchar ('\n');
	}
	close (rfile);
}

char   *
        itoa (a)
register int    a;
{
	static char     buffer[8];

	sprintf (buffer, "%d", a);
	return (buffer);
}

clearlocks () {
	register        x;

	signal (SIGINT, SIG_IGN);
	for (x = 1; x <= MAXLEVEL; x++) {
		glo (x);
		if (unlink (lock))
			break;
	}
#ifdef DEBUG
	glo (0);
	unlink (lock);
#endif DEBUG
}

hangup () {
	save ();
	clearlocks ();
	exit (1);
}
