/*
 * Hack.mon.c
 */

/* Contains various monster routines */

#include "hack.h"

extern char     WCLEV[], STOPGLOW[];

dist (x, y)
register        x, y;
{
	x -= u.ux;
	y -= u.uy;
	return (x * x + y * y);
}

r_free (x, y, mtmp)
register        x, y;
register        MONSTER mtmp;
{
	if (mtmp -> ale)
		return (levl[x][y].typ == POOL);
	else
		return (levl[x][y].typ > SDOOR &&
				(x != u.ux || y != u.uy) && levl[x][y].typ < POOL);
 /* DOOR,CORR,ROOM */
}


/* Puts m next to u, or anywhere if there isn't room there */
mnexto (mtmp)
MONSTER mtmp;
{
	register        x, y, z;
	struct {
		char    zx, zy;
	}       foo[15], *tfoo;
	int     range;

	tfoo = foo;
	range = 1;
	do {			/* Full kludge action */
		for (y = 0; y < 2; y++)
			for (x = u.ux - range; x <= u.ux + range; x++) {
				z = range;
				if (!y)
					z = -z;
				if (test (x, z += u.uy)) {
					tfoo -> zx = x;
					tfoo++ -> zy = z;
					if (tfoo == &foo[15])
						goto foofull;
				}
			}
		for (x = 0; x < 2; x++)
			for (y = u.uy + 1 - range; y < u.uy + range;
					y++) {
				z = range;
				if (!x)
					z = -z;
				if (test (z += u.ux, y)) {
					tfoo -> zx = z;
					tfoo++ -> zy = y;
					if (tfoo == &foo[15])
						goto foofull;
				}
			}
		range++;
	} while (tfoo == foo);
foofull: 
	tfoo = &foo[rn2 (tfoo - foo)];
	mtmp -> mx = tfoo -> zx;
	mtmp -> my = tfoo -> zy;
	pmon (mtmp);
	if (mtmp -> data -> mlet == 'w')
		initworm (mtmp);
}

rloc (mtmp)
MONSTER mtmp;
{
	register        tx, ty;
	register char   ch = mtmp -> data -> mlet;

	if (ch == 'w' && mtmp -> mx)
		return;		/* Do not relocate worms */
	levlsym (mtmp -> mx, mtmp -> my, ch);
	if (mtmp -> ale) {
		do {
			tx = rn1 (77, 2);
			ty = rn2 (22);
		/* until CORR,DORR,or ROOM; or... */
		} while (levl[tx][ty].typ != POOL || m_at (tx, ty) ||
				(tx == u.ux && ty == u.uy));
	}
	else {
		do {
			tx = rn1 (77, 2);
			ty = rn2 (22);
		/* until CORR,DORR,or ROOM; or... */
		} while (levl[tx][ty].typ < DOOR || m_at (tx, ty) ||
				(tx == u.ux && ty == u.uy)
				|| levl[tx][ty].typ >= 7);
	}
	mtmp -> mx = tx;
	mtmp -> my = ty;
	pmon (mtmp);
	if (ch == 'w')
		initworm (mtmp);
}

test (x, y) {
	if (x <= 0 || x > 78 || y <= 0 || y > 20)
		return 0;
	if (m_at (x, y) || levl[x][y].typ < DOOR || levl[x][y].typ >= 7)
		return 0;
	return 1;
}

poisoned (string, pname)
register char  *string, *pname;
{
	pseebl ("%s was poisoned!", string);
	if (u.upres) {
		pline ("The poison doesn't seem to affect you.");
		return;
	}

	switch (rn2 (6)) {
		case 0: 
			u.uhp = 0;
			break;
		case 1: 
		case 2: 
		case 3: 
			losestr (rn1 (3, 3));
			break;
		case 4: 
		case 5: 
			losehp (rn1 (10, 6), pname);
			return;
	}

	if (u.uhp <= 0)
		killer = pname;
}

steal (mtmp)
MONSTER mtmp;
{
	register        OBJECT otmp, ot1;
	register        tmp;

	for (otmp = invent, tmp = 0; otmp -> nobj; otmp = otmp -> nobj, tmp++);

	tmp = rn2 (tmp);
	otmp = invent;
	if (!tmp)
		invent = invent -> nobj;
	else {
		for (; otmp && tmp; tmp--, otmp = otmp -> nobj);
		ot1 = otmp -> nobj;
		otmp -> nobj = ot1 -> nobj;/* rm obj from invent */
		otmp = ot1;
	}
	if (otmp == uarm || otmp == uarm2) {
		u.uac += otmp -> spe;
		if (otmp == uarm)
			uarm = uarm2;
		uarm2 = 0;
		flags.dac = 1;
	}
	else if (otmp == uwep)
		uwep = 0;
	else if (otmp == uleft) {
		uleft = 0;
		doring (otmp, OFF);
	}
	else if (otmp == uright) {
		uright = 0;
		doring (otmp, OFF);
	}
	doname (otmp, buf);
	pline ("She stole %s.", buf);
	stlobj (mtmp, otmp);
}

stlobj (mtmp, otmp)
register        MONSTER mtmp;
register        OBJECT otmp;
{
	otmp -> nobj = 0;	/* Michiel: dog and two objects? */
	if (mtmp -> mstole) {
		otmp -> nobj = mtmp -> mstole -> sobj;
		mtmp -> mstole -> sobj = otmp;
		return;
	}			/* Michiel save stolen object */
	else {
		mtmp -> mstole = newstole ();
		mtmp -> mstole -> sobj = otmp;
		mtmp -> mstole -> sgold = 0;
	}
}

delmon (mtmp)
register        MONSTER mtmp;
{
	unstuck (mtmp);		/* a3 */
	relmon (mtmp);
	if (mtmp == shopkeeper)
		shkdead ();
	if (mtmp == vaultkeeper) {
		mtmp -> data -> mmove = -1;
		vaultkeeper = 0;
	}
	if (mtmp -> wormno)
		wormdead (mtmp);
	free (mtmp);
}

relmon (mtmp)
register        MONSTER mtmp;
{
	register        MONSTER mtmp2;

	if (mtmp == fmon)
		fmon = fmon -> nmon;
	else {
		for (mtmp2 = fmon; mtmp2 -> nmon != mtmp; mtmp2 = mtmp2 -> nmon);
		mtmp2 -> nmon = mtmp -> nmon;
	}
}

/* Release the objects the killed animal has stolen */
relobj (mtmp)
register        MONSTER mtmp;
{
	register        GOLD_TRAP gtmp;
	register        tmp = 0;
	OBJECT otmp, otmp2;

	if (mtmp -> mstole) {	/* Michiel drop stolen obj or gold */
		if (mtmp -> mstole -> sgold)
			tmp = mtmp -> mstole -> sgold;
		else {
			otmp = mtmp -> mstole -> sobj;
			do {
				otmp -> ox = mtmp -> mx;
				otmp -> oy = mtmp -> my;
				otmp2 = otmp;
				otmp = otmp -> nobj;
			} while (otmp);
			otmp2 -> nobj = fobj;
			fobj = mtmp -> mstole -> sobj;
			if (mtmp -> data -> mlet != 'd')
				seeatl (otmp -> ox, otmp -> oy, otmp -> olet);
		}
		free (mtmp -> mstole);
		mtmp -> mstole = NULL;
	}
	if (mtmp -> data -> mlet == 'L') {
		gtmp = newgen ();
		gtmp -> ngen = fgold;
		gtmp -> gx = mtmp -> mx;
		gtmp -> gy = mtmp -> my;
		if (dlevel)
			gtmp -> gflag = tmp + d (dlevel, 30);
		else
			gtmp -> gflag = tmp + d (maxdlevel, 30);
		fgold = gtmp;
		seeatl (mtmp -> mx, mtmp -> my, '$');
	}
}

/* a3 */
unstuck (mtmp)
register        MONSTER mtmp;
{
	if (mtmp == u.ustuck) {
		if (u.uswallow) {
			u.uswallow = 0;
			u.uswldtim = 0;
			docrt ();
			setCon (SETC);/* Try a3 */
		}
		u.ustuck = 0;
	}
}

killed (mtmp)
register        MONSTER mtmp;
{
	register        tmp;

	unstuck (mtmp);
	levlsym (mtmp -> mx, mtmp -> my, mtmp -> data -> mlet);
	if (mtmp -> cham)
		mtmp -> data = (PM_CHAM);
	pseebl ("You destroy %s!", mtmp -> data -> mname);
	if (!u.ublind && u.umconf) {
		pline (STOPGLOW);
		u.umconf = 0;
	}
	tmp = mtmp -> data -> mhd;
	tmp *= tmp;
	++tmp;
	if (mtmp -> data -> ac < 3)
		tmp += (7 - mtmp -> data -> ac) << 1;
	if (index ("AcsSDXaeRTVWU&In:P", mtmp -> data -> mlet))
		tmp += mtmp -> data -> mhd << 1;
	if (index ("DeV&P", mtmp -> data -> mlet))
		tmp += 7 * mtmp -> data -> mhd;
	if (mtmp -> data -> mhd > 6)
		tmp += 50;
	if (mtmp -> ale)
		tmp += 1000;
	relobj (mtmp);
	if ((index ("NTV&", mtmp -> data -> mlet) || !rn2 (5)) && !mtmp -> ale
			&& levl[mtmp -> mx][mtmp -> my].typ > SDOOR) {
	/* Mimic in wall? */
		mkobj (0);
		fobj -> ox = mtmp -> mx;
		fobj -> oy = mtmp -> my;
		if (!u.ublind)
			atl (mtmp -> mx, mtmp -> my, fobj -> olet);
	}
	delmon (mtmp);
	u.urexp += tmp << 2;
	u.uexp += tmp;
	flags.dexp = 1;
	while (u.uexp >= 10L * pow (u.ulevel - 1)) {
		pline (WCLEV, ++u.ulevel);
		tmp = rnd (10);
		if (tmp < 3)
			tmp = rnd (10);
		u.uhpmax += tmp;
		u.uhp += tmp;
		flags.dhp = 1;
		flags.dhpmax = 1;
		flags.dulev = 1;
	}
}

#define TBLIND	5
#define NOTEST	6

/*VARARGS*/
psee (mode, x, y, str, name, arg)/* Str bevat %s */
register char  *str, *name, *arg;
{
	char   *a1, *a2;

	a1 = "the %s";
	a2 = "the %s";
	if (mode == TBLIND) {
		if (u.ublind)
			a1 = "it";
	}
	else if (mode != NOTEST && !cansee (x, y))
		switch (mode) {
			case IT1: 
				a1 = "it";
				break;
			case THEIT2: 
				a2 = "it";
				break;
			case 0: 
				return 0;
			default: 
				pline ("Bad(%d) mode in psee", mode);
		}
	sprintf (buf, str, a1, a2);
	if (*buf >= 'a' && *buf <= 'z')
		*buf += 'A' - 'a';
	pline (buf, name, arg);
	return 1;
}

/*VARARGS*/
p2xthe (str, name, arg)
register char  *str, *name, *arg;
{
	psee (NOTEST, 0, 0, str, name, arg);
}

pseebl (str, name)
register char  *str, *name;
{
	psee (TBLIND, 0, 0, str, name, NULL);
}

rescham () {			/* Force all chameleons to become normal */
	register        MONSTER mtmp;

	for (mtmp = fmon; mtmp; mtmp = mtmp -> nmon)
		if (mtmp -> cham) {
			mtmp -> cham = 0;
			if (u.uswallow && u.ustuck == mtmp) {
				unstuck (mtmp);
				mnexto (mtmp);
			}
			newcham (mtmp, PM_CHAM);
		}
}

/* Make a chameleon look like a new monster */
newcham (mtmp, mdat)
register        MONSTER mtmp;
register        MONSTDATA mdat;
{
	register        mhp, hpn, hpd;

	if (mdat == mtmp -> data)
		return;		/* Still the same monster */
	if (u.uswallow && mdat -> mlet == 'w')
		return;
	if (mtmp -> wormno)
		wormdead (mtmp);/* Throw tail away */
	hpn = mtmp -> mhp;
	hpd = mtmp -> data -> mhd << 3;
	mtmp -> data = mdat;
	mtmp -> invis = 0;
	mtmp -> mtame = 0;
	mhp = mdat -> mhd << 3;
/* New hp: same fraction of max as before */
	mtmp -> mhp = 2 + (hpn * mhp) / hpd;
	hpn = mtmp -> orig_hp;
	mtmp -> orig_hp = 2 + (hpn * mhp) / hpd;
	if (mdat -> mlet == 'I') {
		++mtmp -> invis;
		if (cansee (mtmp -> mx, mtmp -> my))
			prl (mtmp -> mx, mtmp -> my);
	}
	if (mdat -> mlet == 'w' && getwn (mtmp))
		initworm (mtmp);
	if (u.uswallow && mtmp == u.ustuck &&
			!index (",'P", mdat -> mlet)) {
		unstuck (mtmp);
		mnexto (mtmp);
	}
	pmon (mtmp);
}

makemon (ptr)
register        MONSTDATA ptr;
{
	register        MONSTER mtmp;

	if (!ptr) {
		do
			ptr = &mon[rn2 (dlevel / 3 + 1) % 8][rn2 (7)];
		while (index (genocided, ptr -> mlet));
	}
	else {
		if (index (genocided, ptr -> mlet)) {
			if (!u.ublind)
				p2xthe ("%s vanishes!", ptr -> mname);
			return 1;
		}
	}
	mtmp = newmonst (ptr -> pxlth);
	mtmp -> nmon = fmon;
	fmon = mtmp;
	mtmp -> mstole = 0;
	mtmp -> invis = 0;
	mtmp -> cham = 0;
	mtmp -> msleep = 0;
	mtmp -> mfroz = 0;
	mtmp -> mconf = 0;
	mtmp -> mflee = 0;
	mtmp -> mtame = 0;
	mtmp -> mspeed = 0;
	mtmp -> mcan = 0;
	mtmp -> angry = 0;
	mtmp -> mxlth = 0;
	mtmp -> ale = 0;
	mtmp -> data = ptr;
	switch (ptr -> mlet) {
		case 'I': 
			++mtmp -> invis;
			break;
		case 'L': 
			mtmp -> msleep = u.uhcursed;
			break;
		case ':': 
			++mtmp -> cham;
			if (!u.ucham)
				newcham (mtmp, &mon[rn1 (6, 2)][rn2 (7)]);
			break;
		case ';': 
			mtmp -> ale = 1;
			break;
	}
	if (ptr -> mlet != 'w' || !getwn (mtmp))
		mtmp -> wormno = 0;
	mtmp -> mhp = rnd (4);
	if (ptr -> mhd)
		mtmp -> mhp = d (ptr -> mhd, 8);
	mtmp -> orig_hp = mtmp -> mhp;
	return 0;
}

somegold () {
	return ((u.ugold < 100L) ? u.ugold :
			(u.ugold > 10000L) ? rnd (10000) : rnd ((int) u.ugold));
}

mkmonat (ptr, x, y)
register        MONSTDATA ptr;
register        x, y;
{
	if (makemon (ptr))
		return;
	if (x == u.ux && y == u.uy)
		mnexto (fmon);
	else {
		atl (x, y, ptr -> mlet);
		fmon -> mx = x;
		fmon -> my = y;
	}
}
