/*
 * Hack.save.c
 */

/*
 * The old version of save () didn't work at all. Many things are changed,
 * but some things are not implemented yet, like saving in a shop, or saving
 * while swallowed or stuck
 */

#include "hack.h"
#include "hack.dog.h"
#include <signal.h>

#define MAXLEVEL	40

extern char     SAVEFILE[], nul[], upxstairs[MAXLEVEL],
                upystairs[MAXLEVEL], shlevel, vaultflag[MAXLEVEL];
extern long     robbed;
extern unsigned starved;
extern  COORDINATES shk, shd;
extern  MONSTER shopkeeper;
extern  MONSTER mydogs;

save () {
	register        fd, ofd, tmp;
	register        OBJECT otmp, otmp2;
	MONSTER mtmp;
	int     version = VERSION;

	nomove ();
	if (shopkeeper && inshproom (u.ux, u.uy)) {
		pline ("You are not allowed to save in a shop. (Continue or Quit)");
		return;
	}
	else if (u.ustuck || u.uswallow) {
		pline ("Not implemented when you're stuck or swallowed. (Continue or Quit)");
		return;
	}
	if ((fd = creat (SAVEFILE, 0644)) < 0) {
		pline ("Cannot creat save file. (Continue or Quit)");
		return;
	}
	signal (SIGINT, SIG_IGN);
	signal (SIGQUIT, SIG_IGN);

	bwrite (fd, &version, sizeof (version));
	keepdogs (0);
	savelev (fd);
	for (otmp = invent; otmp; otmp = otmp2) {
		bwrite (fd, otmp, sizeof (struct obj));
		if (otmp == uarm)
			bwrite (fd, "a", 1);
		else if (otmp == uarm2)
			bwrite (fd, "b", 1);
		else if (otmp == uwep)
			bwrite (fd, "w", 1);
		else if (otmp == uleft)
			bwrite (fd, "l", 1);
		else if (otmp == uright)
			bwrite (fd, "r", 1);
		else
			bwrite (fd, "n", 1);
		otmp2 = otmp -> nobj;
		ofree (otmp);
	}
	bwrite (fd, nul, sizeof (struct obj));
	bwrite (fd, &flags, sizeof (struct flag));
	bwrite (fd, &dlevel, sizeof dlevel);
	bwrite (fd, &moves, sizeof moves);
	bwrite (fd, &u, sizeof (struct you));
	bwrite (fd, genocided, sizeof genocided);
	bwrite (fd, upxstairs, sizeof upxstairs);
	bwrite (fd, upystairs, sizeof upystairs);
	bwrite (fd, vaultflag, sizeof vaultflag);

	savenames (fd);

/* SHOP part */
	bwrite (fd, &shd, sizeof (struct coord));
	bwrite (fd, &shk, sizeof (struct coord));
	bwrite (fd, &shlevel, sizeof shlevel);
	bwrite (fd, &robbed, sizeof robbed);

/* Various globals */
	bwrite (fd, &starved, sizeof starved);
	bwrite (fd, &seehx, sizeof seehx);
	bwrite (fd, &seelx, sizeof seelx);
	bwrite (fd, &seehy, sizeof seehy);
	bwrite (fd, &seely, sizeof seely);
	bwrite (fd, &dx, sizeof dx);
	bwrite (fd, &dy, sizeof dy);
	bwrite (fd, &maxdlevel, sizeof maxdlevel);

/* And the dog(s) if any */
	for (mtmp = mydogs; mtmp; mtmp = mtmp -> nmon)
		bwrite (fd, mtmp, sizeof (struct monst) +
			                                        sizeof (struct edog));
	bwrite (fd, nul, sizeof (struct monst) + sizeof (struct edog));

	callssave (fd);

	cls ();
	printf ("Saving level ");
	flush ();
	for (tmp = 1;; tmp++) {
		glo (tmp);
		if ((ofd = open (lock, 0)) < 0)
			break;
		getlev (ofd);
		close (ofd);
		savelev (fd);
		printf ("%2d - %s", tmp,
				(tmp % 10) ? "" : "\n             ");
		flush ();
		unlink (lock);
	}

	close (fd);
	(*index (lock, '.')) = '\0';/* Remove main lock */
	unlink (lock);
	printf ("\n\nSee you around...\n");
	flush ();
	hackmode (OFF);
	exit (0);
}

dorecover (fd)
register        fd;
{
	register        nfd, tmp;
	register        OBJECT otmp, olast;
	MONSTER mtmp;
	int     version;

	cls ();
	printf ("Starting up a suspended game....\n");
	flush ();
	mread (fd, &version, sizeof (version));
	if (version != VERSION) {
		printf ("Sorry, you're savefile is out of date.\n");
		printf ("I will have to remove it.\n");
		printf ("Type <space> to continue.");
		close (fd);
		unlink (SAVEFILE);
		flush ();
		while (getchar () != ' ');
		return - 1;
	}

	getlev (fd);

	invent = otmp = newobj ();
	while (1) {
		mread (fd, otmp, sizeof (struct obj));
		if (!otmp -> olet) {
			if (otmp == invent)
				invent = 0;
			else
				olast -> nobj = 0;
			ofree (otmp);
			break;
		}
		olast = otmp;
		olast -> nobj = otmp = newobj ();
		mread (fd, buf, 1);
		switch (*buf) {
			case 'w': 
				uwep = olast;
				break;
			case 'r': 
				uright = olast;
				break;
			case 'l': 
				uleft = olast;
				break;
			case 'a': 
				uarm = olast;
				break;
			case 'b': 
				uarm2 = olast;
			case 'n': 
				break;
			default: 
				panic (CORE, "Error reading save file");
		}
	}
	mread (fd, &flags, sizeof (struct flag));
	mread (fd, &dlevel, sizeof dlevel);
	mread (fd, &moves, sizeof moves);
	mread (fd, &u, sizeof (struct you));
	mread (fd, genocided, sizeof genocided);
	mread (fd, upxstairs, sizeof upxstairs);
	mread (fd, upystairs, sizeof upystairs);
	mread (fd, vaultflag, sizeof vaultflag);

	restnames (fd);

/* Restore shop part */
	mread (fd, &shd, sizeof (struct coord));
	mread (fd, &shk, sizeof (shk));
	mread (fd, &shlevel, sizeof shlevel);
	mread (fd, &robbed, sizeof robbed);

/* Restore various globals */
	mread (fd, &starved, sizeof starved);
	mread (fd, &seehx, sizeof seehx);
	mread (fd, &seelx, sizeof seelx);
	mread (fd, &seehy, sizeof seehy);
	mread (fd, &seely, sizeof seely);
	mread (fd, &dx, sizeof dx);
	mread (fd, &dy, sizeof dy);
	mread (fd, &maxdlevel, sizeof maxdlevel);

/* Let's try the dog again */
	while (1) {
		mtmp = newmonst (sizeof (struct edog));
		mread (fd, mtmp, sizeof (struct monst) +
			                                        sizeof (struct edog));
		if (mtmp -> data == 0)
			break;
		else {
			mtmp -> nmon = mydogs;
			mydogs = mtmp;
		}
	}
	free (mtmp);

	callsrestore (fd);

	printf ("Restoring level ");
	flush ();
	for (tmp = 1;; tmp++) {
		if (getlev (fd))
			break;
		glo (tmp);
		if ((nfd = creat (lock, 0644)) < 0)
			panic (CORE, "Cannot open temp file %s!\n",
					lock);
		savelev (nfd);
		printf ("%2d - %s", tmp,
				(tmp % 10) ? "" : "\n                ");
		flush ();
		close (nfd);
	}

	lseek (fd, (long) (sizeof (version)), 0);
	getlev (fd);
	close (fd);
	losedogs ();
	unlink (SAVEFILE);
	docrt ();
	return 1;
}
