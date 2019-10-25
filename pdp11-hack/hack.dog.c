/*
 * Hack.dog.c
 */

#include        "hack.h"
#include        "hack.dog.h"

#define UNDEF   127		/* Some large number */
#define EDOG(mp)        ( (struct edog *)(&(mp->mextra[0])) )

extern struct permonst  li_dog, dog, la_dog;

char    SADFEEL[] = "You have a sad feeling for a moment, then it passes";

makedog () {
	if (makemon (&li_dog))
		return;		/* Dogs were genocided */
	mnexto (fmon);
	initedog (fmon);
}

initedog (mtmp)
register        MONSTER mtmp;
{
	mtmp -> mtame = 1;
	EDOG (mtmp) -> hungrytime = 1000 + moves;
	EDOG (mtmp) -> eattime = 0;
	EDOG (mtmp) -> droptime = 0;
	EDOG (mtmp) -> dropdist = 10000;
	EDOG (mtmp) -> apport = 10;
	EDOG (mtmp) -> carry = 0;
}

/* Attach the monsters that went down (or up) together with @ */

MONSTER mydogs = 0;

losedogs () {
	register        MONSTER mtmp;

	while (mtmp = mydogs) {
		mydogs = mtmp -> nmon;
		mtmp -> nmon = fmon;
		fmon = mtmp;
		mnexto (mtmp);
	}
}

keepdogs (checkdist)
int     checkdist;
{
	register        MONSTER mtmp;
	register        PART * dr;

	for (mtmp = fmon; mtmp; mtmp = mtmp -> nmon)
		if (mtmp -> mtame) {
			if (checkdist && dist (mtmp -> mx, mtmp -> my) > 2) {
				mtmp -> mtame = 0;/* Dog becomes wild */
				mtmp -> mxlth = 0;
				continue;
			}
			relmon (mtmp);
			mtmp -> nmon = mydogs;
			mydogs = mtmp;
			dr = &levl[mtmp -> mx][mtmp -> my];
			if (dr -> scrsym == mtmp -> data -> mlet)
				dr -> scrsym = news0 (mtmp -> mx, mtmp -> my);
		/* We destroyed the link, so use recursion */
			keepdogs (checkdist);
			return;	/* (admittedly somewhat primitive) */
		}
}

#define GDIST(x, y) ((x - gx)*(x - gx) + (y - gy)*(y - gy) )
#define DDIST(x, y) ((x - omx)*(x - omx) + (y - omy)*(y - omy) )

dog_move (mtmp, after)
register        MONSTER mtmp;
{
	register        MONSTER mtmp2;
	register struct edog   *edog = EDOG (mtmp);
	int     nix, niy, omx, omy, appr, nearer, cnt, udist, zx, zy;
	register        OBJECT obj;
	register        GOLD_TRAP trap;
	char    ddx, ddy, dogroom, uroom,
	        gx = 0, gy = 0, gtyp;/* Current goal */

	if (moves <= edog -> eattime)
		return NOMOVE;	/* Dog is still eating */
	omx = mtmp -> mx;
	omy = mtmp -> my;
	if (moves > edog -> hungrytime + 500 && !mtmp -> mconf) {
		mtmp -> mconf = 1;
		mtmp -> orig_hp /= 3;
		if (mtmp -> mhp > mtmp -> orig_hp)
			mtmp -> mhp = mtmp -> orig_hp;
		psee (0, omx, omy, "%s is confused from hunger",
				mtmp -> data -> mname);
	}
	else if (moves > edog -> hungrytime + 750 || mtmp -> mhp <= 0) {
		if (!psee (0, omx, omy, "%s dies from hunger",
					mtmp -> data -> mname))
			pline (SADFEEL);
		levlsym (omx, omy, mtmp -> data -> mlet);
		delmon (mtmp);
		return DEAD;
	}
	dogroom = inroom (omx, omy);
	uroom = inroom (u.ux, u.uy);
	udist = dist (omx, omy);

/*
 * if we are carrying stg then we drop it (perhaps near @ )
 * Note: if apport == 1 then our behaviour is independent of udist
 */
	if (edog -> carry) {
		if (!rn2 (udist) || !rn2 (edog -> apport))
			if (rn2 (10) < edog -> apport) {
				relobj (mtmp);
				if (edog -> apport > 1)
					edog -> apport--;
				edog -> carry = 0;
			}
	}
	else {
		if (obj = o_at (omx, omy))
			if (rn2 (20) < edog -> apport + 3)
				if (rn2 (udist) || !rn2 (edog -> apport)) {
					edog -> carry = 1;
					freeobj (obj);
					levlsym (omx, omy, obj -> olet);
					stlobj (mtmp, obj);
				}
	}

/* First we look for food */
	gtyp = UNDEF;		/* No goal as yet */
	obj = fobj;
	while (obj) {
		if (obj -> olet == '%'
				&& inroom (obj -> ox, obj -> oy) == dogroom
				&& (gtyp == UNDEF || (gtyp != 1 && obj -> otyp == 1)
					|| (((gtyp != 1 && obj -> otyp < 5)
							|| (gtyp == 1 && obj -> otyp == 1))
						&& DDIST (obj -> ox, obj -> oy) < DDIST (gx, gy)))) {
			gx = obj -> ox;
			gy = obj -> oy;
			gtyp = obj -> otyp;
		}
		else if ((gtyp == UNDEF || gtyp == 67) && dogroom >= 0
					&& inroom (obj -> ox, obj -> oy) == dogroom
					&& uroom == dogroom
				&& !edog -> carry && edog -> apport > rn2 (8)) {
			gx = obj -> ox;
			gy = obj -> oy;
			gtyp = 66 + obj -> cursed;/* Random */
		}
		obj = obj -> nobj;
	}
	if (gtyp == UNDEF
			|| (gtyp != 1 && gtyp != 66 && moves < edog -> hungrytime)) {
		if (dogroom < 0 || dogroom == uroom) {
			gx = u.ux;
			gy = u.uy;
		}
		else {
			int     tmp = rooms[dogroom].fdoor;

			cnt = rooms[dogroom].doorct;
			gx = gy = 100;/* Random, far away */
			while (cnt--) {
				if (dist (gx, gy) > dist (doors[tmp].x,
							doors[tmp].y)) {
					gx = doors[tmp].x;
					gy = doors[tmp].y;
				}
				tmp++;

			}
			if (gy == 100)
				panic (CORE, "No doors nearby?");
			if (gx == omx && gy == omy) {
				gx = u.ux;
				gy = u.uy;
			}
		}
		appr = 0;
		if (udist >= 9)
			appr++;
		else if (mtmp -> mflee)
			appr--;
		if (after && udist <= 4 && gx == u.ux && gy == u.uy)
			return NOMOVE;
		if (udist > 1) {
			if (levl[u.ux][u.uy].typ < ROOM || !rn2 (4) ||
					(edog -> carry && rn2 (edog -> apport)))
				appr = 1;
		}
/* If you have dog food he'll follow you more closely */
		if (appr == 0) {
			obj = invent;
			while (obj) {
				if (obj -> olet == '%' && obj -> otyp == 1) {
					appr = 1;
					break;
				}
				obj = obj -> nobj;
			}
		}
	}
	else
		appr = 1;
	if (mtmp -> mconf)
		appr = 0;
	nix = omx;
	niy = omy;
	cnt = 0;
	for (ddx = -1; ddx <= 1; ddx++)
		for (ddy = -1; ddy <= 1; ddy++) {
			if (!ddx && !ddy)
				continue;
			zx = omx + ddx;
			zy = omy + ddy;
			if (mtmp2 = m_at (zx, zy)) {
				if (mtmp2 -> data -> mhd >= mtmp -> data -> mhd + 2)
					continue;
				if (mtmp2 -> mtame)
					continue;
				if (after)
					return NOMOVE;
			/* Hit only once each move */
				if (hitmm (mtmp, mtmp2) == HIT && rn2 (4) &&
						hitmm (mtmp2, mtmp) == DEAD)
					return DEAD;
				return NOMOVE;
			}
			if (r_free (zx, zy, mtmp) && !(ddx && ddy
						&& (levl[omx][omy].typ == DOOR
							|| levl[zx][zy].typ == DOOR))) {
/* M_at(zx,zy) is impossible here */

/* Dog avoids unseen traps */
				if ((trap = g_at (zx, zy, ftrap))
						&& !(trap -> gflag & SEEN) && rn2 (10))
					continue;

/* Dog eschewes cursed objects but likes dog food */
				obj = fobj;
				while (obj) {
					if (obj -> ox != zx || obj -> oy != zy)
						goto nextobj;
					if (obj -> cursed)
						goto newdxy;
					if (obj -> olet == '%' &&
							(obj -> otyp == 1 || (obj -> otyp < 5 &&
									edog -> hungrytime <= moves))) {
						nix = zx;
						niy = zy;
						edog -> eattime = moves +
							foods[obj -> otyp].delay;
						edog -> hungrytime = moves + 5 *
							foods[obj -> otyp].nutrition;
						if (cansee (nix, niy)) {

							char    buffer[BUFSZ];

							doname (obj, buffer);
							pline ("The %s ate %s.", mtmp -> data -> mname, buffer);
						}
						delobj (obj);
/* perhaps this was a reward */
						edog -> apport += 200 / (edog -> dropdist + moves - edog -> droptime);
						goto newdogpos;
					}
			nextobj: 
					obj = obj -> nobj;
				}

				nearer = GDIST (zx, zy) - GDIST (nix, niy);
				nearer *= appr;
				if (!nearer && !rn2 (++cnt) || nearer < 0
						|| nearer > 0 && (omx == nix && omy ==
							niy && !rn2 (3) || !rn2 (12))) {
					nix = zx;
					niy = zy;
					if (nearer < 0)
						cnt = 0;
				}
			}
	newdxy: 	;
		}
newdogpos: 
	if (nix != omx || niy != omy) {
		mtmp -> mx = nix;
		mtmp -> my = niy;
	}
	levlsym (omx, omy, mtmp -> data -> mlet);
	pmon (mtmp);
	return MOVE;
}

hitmm (magr, mdef)
register        MONSTER magr, mdef;
{
	register        MONSTDATA pa = magr -> data;
	register        MONSTDATA pd = mdef -> data;
	int     hit;
	char    tmp, vis;

	if (index ("Eay", magr -> data -> mlet))
		return NOMOVE;
	tmp = pd -> ac + pa -> mhd - 1;
	if (mdef -> mconf || mdef -> mfroz || mdef -> msleep) {
		tmp += 4;
		mdef -> msleep = 0;
	}
	hit = (tmp >= rnd (20));
	vis = (cansee (magr -> mx, magr -> my) &&
			cansee (mdef -> mx, mdef -> my));
	if (vis)
		pline ("The %s %s the %s.", pa -> mname,
				(hit) ? "hits" : "misses", pd -> mname);
	else
		pline ("You hear some noises %s.",
				(dist (magr -> mx, magr -> my) > 15) ? "in the distance"
				: "");
	if (hit) {
		if ((mdef -> mhp -= d (pa -> damn, pa -> damd)) <= 0) {
			if (vis)
				p2xthe ("%s is killed!", pd -> mname);
			else
			if (mdef -> mtame)
				pline (SADFEEL);
			unstuck (mdef);/* a3 */
			relobj (mdef);
			levlsym (mdef -> mx, mdef -> my, pd -> mlet);
			magr -> orig_hp += rnd (pd -> mhd + 1);
			if (magr -> mtame && magr -> orig_hp >
					pa -> mhd << 3) {
				if (pa == &li_dog)
					magr -> data = pa = &dog;
				else if (pa == &dog)
					magr -> data = pa = &la_dog;
			}
			delmon (mdef);
			hit = DEAD;
		}
	}
	return hit;
}

/* Return roomnumber or -1 */
inroom (x, y)
char    x, y;
{
	register        MKROOM * croom = &rooms[0];

	if (xdnstair && dlevel)	/* a3 */
		while (croom -> hx >= 0) {
			if (croom -> hx >= x - 1 && croom -> lx <= x + 1
					&& croom -> hy >= y - 1 && croom -> ly <= y + 1)
				return (croom - rooms);
			croom++;
		}
	return - 1;		/* In corridor or in maze */
}

#define NOTTAME 0
#define TAME    1

tamedog (mtmp, obj)
register        MONSTER mtmp;
register        OBJECT obj;
{
	register        MONSTER mtmp2;

	if (obj -> otyp >= 5 || mtmp -> mtame)
		return NOTTAME;
	psee (0, mtmp -> mx, mtmp -> my, "%s devours %s.",
			mtmp -> data -> mname, foods[obj -> otyp].foodnam);
	delobj (obj);
	mtmp2 = newmonst (sizeof (struct edog));
	*mtmp2 = *mtmp;
	mtmp2 -> mxlth = sizeof (struct edog);
	initedog (mtmp2);
	mtmp2 -> nmon = fmon;
	fmon = mtmp2;
	delmon (mtmp);		/* %% */
	return TAME;
}
