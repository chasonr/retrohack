/*
 * Hack.bones.c
 *
 * Added increased chances on bones files.
 * Fixed bug with mad shopkeeper.
 *
 */

#include "hack.h"

#define NOBONES	0
#define BONES	1

extern char     plname[10];
extern  MONSTER shopkeeper;

struct permonst pm_ghost = {
	"ghost", '\t', 10, 3, -5, 1, 1, sizeof (plname)
};

/* (a3)#define	GHMAX	10 */

char    bones[] = "bones_xx";

/* Save bones and possessions of a deceased adventurer */
savebones () {
	register        fd;
	register        OBJECT otmp;
	register        GOLD_TRAP gtmp;
	register        MONSTER mtmp;

	if (!rn2 (1 + dlevel/2))
		return;	/* not so many ghosts on low levels */
	sprintf (&bones[6], "%d", dlevel);
	if ((fd = open (bones, 0)) >= 0 ) {
		close (fd);
		return;
	}

 /* Drop everything; the corpse's possessions are usually cursed */
	otmp = invent;
	while (otmp) {
		otmp -> ox = u.ux;
		otmp -> oy = u.uy;
		otmp -> unpaid = 0;
		otmp -> known = 0;
		if (rn2 (5))
			otmp -> cursed = 1;
		if (!otmp -> nobj) {
			otmp -> nobj = fobj;
			fobj = invent;
			invent = 0;/* Superfluous */
			break;
		}
		otmp = otmp -> nobj;
	}
	if (makemon (&pm_ghost))
		return;
	fmon -> mx = u.ux;
	fmon -> my = u.uy;
	fmon -> msleep = 1;
/* Added: sizeof fmon->extra */
/* This is actually nonsens. Ghosts do not have names in this version */
	strncpy (fmon -> mextra, plname, sizeof fmon -> mextra);
	if (shopkeeper &&
			inshproom (shopkeeper -> mx, shopkeeper -> my))
		shopkeeper -> angry = 0;
	gtmp = newgen ();
	gtmp -> ngen = fgold;
	fgold = gtmp;
	gtmp -> gx = u.ux;
	gtmp -> gy = u.uy;
	gtmp -> gflag = somegold () + d (dlevel, 30);
	u.ux = u.uy = 100;	/* Random outside map */
	keepdogs (1);		/* All tame animals become wild again */
	for (mtmp = fmon; mtmp; mtmp = mtmp -> nmon)
		levlsym (mtmp -> mx, mtmp -> my, mtmp -> data -> mlet);
	if ((fd = creat (bones, 0644)) < 0)
		return;
	savelev (fd);
	close (fd);
}

getbones () {
	register        fd, x, y;

	sprintf (&bones[6], "%d", dlevel);
	if ((fd = open (bones, 0)) < 0 || rn2 (3)) {
		close (fd);	/* Sometimes we find bones */
		return NOBONES;
	}
	getlev (fd);
	close (fd);
	if (unlink (bones) < 0) {
		pline ("Cannot unlink %s", bones);
		return NOBONES;
	}
	for (x = 0; x < 80; x++)
		for (y = 0; y < 22; y++) {
			levl[x][y].seen = 0;
			levl[x][y].new = 0;
		}
	return BONES;
}
