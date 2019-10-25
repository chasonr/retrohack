/*
 * Hack.lev.c
 */

#include "hack.h"
#include <signal.h>

#define MAXLEVEL	40
#define ERROR		 1
#define OK		 0

extern char    *itoa (), nul[], upxstairs[MAXLEVEL], upystairs[MAXLEVEL];

extern  WORMSEGMENT wsegs[32], wheads[32];
extern unsigned wgrowtime[32];
extern struct permonst  pm_ghost;

#include "hack.savelev.c"

struct permonst pm_ale = {
	"giant eel", ';', 15, 6, -3, 3, 6, 0
};

getlev (fd) {
	register        MONSTER mtmp;
	register        GOLD_TRAP gtmp;
	register        OBJECT otmp;
	register        WORMSEGMENT wtmp;
	int     tmp, xl;
	unsigned        tmoves, omoves;
	STOLE stmp;

	if (fd < 0 || read (fd, levl, sizeof (levl)) != sizeof (levl))
		return ERROR;
	fmon = 0;
	fobj = 0;
	fgold = 0;
	ftrap = 0;
	shopkeeper = 0;
	vaultkeeper = 0;
	mread (fd, &omoves, sizeof (unsigned));
	mread (fd, &xupstair, 1);
	mread (fd, &yupstair, 1);
	mread (fd, &xdnstair, 1);
	mread (fd, &ydnstair, 1);
	if (omoves)
		tmoves = (moves > omoves) ? moves - omoves : 0;
	for (;;) {
		mread (fd, &xl, sizeof (int));
		if (xl == -1)
			break;
		mtmp = newmonst (xl);
		mread (fd, mtmp, xl + sizeof (struct monst));

/* Michiel restore stolen objects */
		stmp = newstole ();
		mread (fd, stmp, sizeof (struct stole));
		if (stmp -> sgold || stmp -> sobj) {
			mtmp -> mstole = stmp;
			mtmp -> mstole -> sobj = 0;
			for (;;) {
				otmp = newobj ();
				mread (fd, otmp, sizeof (struct obj));
				if (!otmp -> olet)
					break;
				otmp -> nobj = mtmp -> mstole -> sobj;
				mtmp -> mstole -> sobj = otmp;
			}
			ofree (otmp);
		}
		else
			free (stmp);
/* Regenerate animals if you've been on another level */
		if (omoves) {
			if (!index (genocided, mtmp -> data -> mlet)) {
				if (index ("ViT", mtmp -> data -> mlet))
					mtmp -> mhp += mtmp -> mhp + tmoves;
				else
					mtmp -> mhp += tmoves / 20;
				if (mtmp -> mhp > mtmp -> orig_hp)
					mtmp -> mhp = mtmp -> orig_hp;
				if (mtmp -> data -> mlet == '@') {
					if (*mtmp -> data -> mname == 's')
						shopkeeper = mtmp;
					else if (*mtmp -> data -> mname == 'v')
						vaultkeeper = mtmp;
				}
				mtmp -> nmon = fmon;
				fmon = mtmp;
			}
		}
		else {		/* Convert code from MKLEV */
			if (mtmp -> mhp == 10)
				mtmp -> data = &pm_ghost;
			else if (mtmp -> ale)
				mtmp -> data = &pm_ale;
			else
				mtmp -> data = &mon[mtmp -> mhp][mtmp -> orig_hp];
			if (mtmp -> data -> mlet == 'D')
				mtmp -> mhp = 80;
			else
				mtmp -> mhp = mtmp -> data -> mhd ?
					d (mtmp -> data -> mhd, 8) : rnd (4);
			mtmp -> orig_hp = mtmp -> mhp;
			mtmp -> cham = (mtmp -> data -> mlet == ':');
			mtmp -> invis = (mtmp -> data -> mlet == 'I');
			if (mtmp -> data -> mlet == 'w' &&
					getwn (mtmp))
				initworm (mtmp);
			mtmp -> nmon = fmon;
			fmon = mtmp;
		}
	}
	for (;;) {
		gtmp = newgen ();
		mread (fd, gtmp, sizeof (struct gen));
		if (!gtmp -> gx)
			break;
		gtmp -> ngen = fgold;
		fgold = gtmp;
	}
	for (;;) {
		mread (fd, gtmp, sizeof (struct gen));
		if (!gtmp -> gx)
			break;
		gtmp -> ngen = ftrap;
		ftrap = gtmp;
		gtmp = newgen ();
	}
	free (gtmp);
	for (;;) {
		otmp = newobj ();
		mread (fd, otmp, sizeof (struct obj));
		if (!otmp -> olet)
			break;
		otmp -> nobj = fobj;
		fobj = otmp;
	}
	ofree (otmp);
	mread (fd, rooms, sizeof (rooms));
	mread (fd, doors, sizeof (doors));
	if (!omoves)
		return OK;	/* From MKLEV */
	mread (fd, wsegs, sizeof (wsegs));
	for (tmp = 1; tmp < 32; tmp++)
		if (wsegs[tmp]) {
			wheads[tmp] = wsegs[tmp] = wtmp = newseg ();
			for (;;) {
				mread (fd, wtmp, sizeof (struct wseg));
				if (!wtmp -> nseg)
					break;
				wheads[tmp] -> nseg = wtmp = newseg ();
				wheads[tmp] = wtmp;
			}
		}
	mread (fd, wgrowtime, sizeof (wgrowtime));
	return OK;
}

mread (fd, buffer, len)
register        fd, len;
register char  *buffer;
{
	register        rlen;

	if ((rlen = read (fd, buffer, len)) != len)
		panic (CORE, "Read %d instead of %d bytes from file #%d\n",
				rlen, len, fd);
}

mklev () {
	register        fd;
	char    type[2];

#ifndef DEBUG
	if (getbones ()) {
		sleep (2);
		goto Z;
	}
#endif DEBUG
	if (flags.next) {
		flags.next = 0;
		type[0] = 'b';
	}
	else if (dlevel < rn1 (3, MAXLEVEL - 3))
		type[0] = 'a';
	else {
		type[0] = 'n';
		flags.next = 1;
	}
	type[1] = '\0';
	hackexec (0, "./mklev", lock, type, itoa (dlevel), genocided,
			wizard ? "w" : "", NULL);
	if ((fd = open (lock, 0)) < 0) {
		pline ("Can't open %s! Second try.", lock);
		flush ();
		hackexec (0, "./mklev", lock, type, itoa (dlevel), genocided,
				wizard ? "w" : "", NULL);
		if ((fd = open (lock, 0)) < 0)
			panic (NOCORE, "Mklev error no level");
	}
	getlev (fd);
	close (fd);
Z: 
	if (!upxstairs[dlevel] && !upystairs[dlevel]) {
		upxstairs[dlevel] = xupstair;
		upystairs[dlevel] = yupstair;
	}
}
