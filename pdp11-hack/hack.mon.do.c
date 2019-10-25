/*
 * Hack.mon.do.c
 */

/* Contains monster control routines */

#include "hack.h"

extern  MONSTER bhit ();

movemon () {
	register        MONSTER mtmp, mtmp2;

	for (mtmp = fmon; mtmp; mtmp = mtmp2) {
		mtmp2 = mtmp -> nmon;
		if (mtmp -> mspeed != MSLOW || moves % 2 == 0)
			if (dochug (mtmp))
				continue;/* Monster died */
		if (mtmp -> mspeed == MFAST)
			if (dochug (mtmp))
				continue;

/* If we left the room: make monsters invis, even if they didn't move */

		if (!cansee (mtmp -> mx, mtmp -> my))
			levlsym (mtmp -> mx, mtmp -> my, mtmp -> data -> mlet);
		if (mtmp -> wormno && mtmp -> data -> mlet == 'w')
			wormsee (mtmp -> wormno);
	}
}

justswld (mtmp)
register        MONSTER mtmp;
{
	newsym (mtmp -> mx, mtmp -> my);
	mtmp -> mx = u.ux;
	mtmp -> my = u.uy;
	u.ustuck = mtmp;
	at (u.ux, u.uy, mtmp -> data -> mlet);
	pseebl ("%s swallows you!", mtmp -> data -> mname);
	more ();
	u.uswallow = 1;
	docrt ();
}

youswld (mtmp, dam, die)
register        MONSTER mtmp;
register        dam, die;
{
	pseebl ("%s digests you!", killer = mtmp -> data -> mname);
	if (dam > 0) {
		u.uhp -= dam;
		flags.dhp = 1;
	}
	if (u.uswldtim++ == die) {
		pline ("It totally digests you!");
		u.uhp = 0;
	}
}

x2hitu (mlev, x, name)
register        mlev, x;
register char  *name;		/* a3 */
{
	register        i;

	for (i = 0; i < 2; i++)
		hitu (mlev, (x < 0) ? d (2, x) : (x == 0) ? d (3, 4) :
				rnd (x), name);
}

dochug (mtmp)
register        MONSTER mtmp;
{
	register        MONSTDATA mdat;
	register        tmp = 0, ctmp;

	if (mtmp -> mhp <= 0 && !mtmp -> mtame)
		return 1;	/* Killed by dog or ? */
	if (mtmp -> cham && !rn2 (6))
		newcham (mtmp, &mon[rn1 (6, 2)][rn2 (7)]);
	mdat = mtmp -> data;
 /* 
  if( mdat->mhd < 0 )
  panic( CORE, "Bad(%d)monster %c", mdat->mhd,
  mdat->mlet );
  */
	if ((moves % 20 == 0 || index ("ViT", mdat -> mlet)) &&
			mtmp -> mhp < mtmp -> orig_hp)
		mtmp -> mhp++;	/* Regenerate monsters */
	if (mtmp -> mfroz)
		return 0;	/* Frozen monsters don't do anything */
	if (mtmp -> msleep) {
	/* Wake up a monster, or get out of here */
		if (cansee (mtmp -> mx, mtmp -> my) && !u.ustelth &&
				(!rn2 (7) || u.uagmon))
			mtmp -> msleep = 0;
		else
			return 0;
	}

/* Confused monsters get unconfused with small probability */

	if (mtmp -> mconf && !rn2 (50))
		mtmp -> mconf = 0;
	if (mdat -> mmove >= rnd (12) && (mtmp -> mflee || mtmp -> mconf ||
				dist (mtmp -> mx, mtmp -> my) > 2 || mtmp -> mtame ||
				mtmp == shopkeeper) && (tmp = m_move (mtmp, 0)) &&
			mdat -> mmove <= 12)
		return (tmp == 2);
 /* Move monsters and exit if rate<=12 */
	if (tmp == 2)
		return 1;	/* Monster died moving */

	if (!index ("Ea", mdat -> mlet) && dist (mtmp -> mx, mtmp -> my) < 3
			&& !mtmp -> mtame && mtmp != shopkeeper && u.uhp > 0) {
		nomul (tmp = 0);
		if (u.uswallow) {
			if (mtmp != u.ustuck)
				if (mdat -> mmove - 12 > rnd (12))
					tmp = m_move (mtmp, 1);
		}
		else if (!index ("&DyF", mdat -> mlet)) {
			if (mtmp -> ale && cansee (mtmp -> mx, mtmp -> my)) {
				mtmp -> invis = 0;
				pmon (mtmp);
			}
			tmp = hitu (mdat -> mhd, d (mdat -> damn, mdat -> damd),
					mdat -> mname);
		}

/* Increase chance of hitting (no damage) for L and R */
		if (index ("LR", mdat -> mlet) && hitu (5, 0, mdat -> mname))
			tmp++;

		ctmp = (tmp && !mtmp -> mcan && (!uarm ||
					armors[uarm -> otyp].a_can < rnd (3)));

		switch (mdat -> mlet) {

			case ';': 
				if (mtmp -> mcan)
					break;
				if (!u.ustuck && !rn2 (20)) {
					p2xthe ("%s swings itself around you!",
							mdat -> mname);
					u.ustuck = mtmp;
				}
				else if (u.ustuck == mtmp && ctmp) {
					p2xthe ("%s drowns you...", mdat -> mname);
					more ();
					done (DROWNED);
				}
				break;

			case '&': 
				if (!mtmp -> mcan && !rn2 (15)) {
					makemon (PM_DEMON);
					mnexto (fmon);
				}
				else {
					x2hitu (10, -6, mdat -> mname);
					x2hitu (10, 3, mdat -> mname);
					hitu (10, rn1 (4, 2), mdat -> mname);
				}
				break;

			case ',': 
			case '\'': 
				if (u.uswallow)
					if (mdat -> mlet == ',')
						youswld (mtmp, 4 + u.uac, 5);
					else
						youswld (mtmp, rnd (6), 7);
				else if (tmp)
					justswld (mtmp);
				break;

			case 'A': 
				if (ctmp && rn2 (2)) {
					pline ("You feel weaker!");
					losestr (1);
				}
				break;

			case 'C': 
			case 'Y': 
				hitu (4, rnd (6), mdat -> mname);
				break;

			case 'c': 
				if (ctmp && !rn2 (5)) {
					pline ("You get turned to stone!");
					u.uhp = 0;
				}
				break;

			case 'D': 
				if (rn2 (6) || mtmp -> mcan) {
					hitu (10, d (3, 10), mdat -> mname);
					x2hitu (10, 8, mdat -> mname);
					break;
				}
				pseebl ("%s breathes fire!", mdat -> mname);
				buzz (Z_FIRE, mtmp -> mx, mtmp -> my,
						u.ux - mtmp -> mx,
						u.uy - mtmp -> my);
				break;

			case 'd': 
				hitu (6, d (2, 4), mdat -> mname);
				break;

			case 'e': 
				hitu (10, d (3, 6), mdat -> mname);
				break;

			case 'F': 
				if (mtmp -> mcan)
					break;
				pseebl ("%s explodes!", mdat -> mname);
				if (u.ucoldres)
					pline ("You don't seem affected by it.");
				else {
					if (17 - (u.ulevel >> 1) > rnd (20)) {
						pline ("You get blasted!");
						tmp = 6;
					}
					else {
						pline ("You duck the blast...");
						tmp = 3;
					}
					losehp (d (tmp, 6), mdat -> mname);
				}
				cmdel (mtmp);
				return 1;

			case 'g': 
				if (!ctmp || multi < 0 || rn2 (6))
					break;
				pseebl ("You are frozen by %ss juices", "cube'");
				nomul (-rnd (10));
				break;

			case 'h': 
				if (!ctmp || multi < 0 || rn2 (5))
					break;
				pseebl ("You are put to sleep by %ss bite!",
						"homunculus'");
				nomul (-rnd (10));
				break;

			case 'j': 
				tmp = hitu (4, rnd (3), mdat -> mname);
				tmp &= hitu (4, rnd (3), mdat -> mname);
				if (tmp)
					x2hitu (4, 4, mdat -> mname);
				break;

			case 'k': 
				if ((hitu (4, rnd (4), mdat -> mname) || !rn2 (3))
						&& ctmp)
					poisoned ("bee's sting", mdat -> mname);
				break;

			case 'L': 
				if (ctmp && u.ugold && rn2 (2)) {
					u.ugold -= (ctmp = somegold ());
					pline ("Your purse feels lighter.");
					flags.dgold = 1;

				/* Michiel save stolen gold */
					if (mtmp -> mstole)
						mtmp -> mstole -> sgold += ctmp;
					else {
						mtmp -> mstole = newstole ();
						mtmp -> mstole -> sobj = 0;
						mtmp -> mstole -> sgold = ctmp;
					}
					mtmp -> mflee = 1;
					rloc (mtmp);
				}
				break;

			case 'N': 
				if (ctmp && invent && rn2 (2)) {
					steal (mtmp);
					rloc (mtmp);
					mtmp -> mflee = 1;
				}
				break;

			case 'n': 
				x2hitu (11, -6, mdat -> mname);
				break;

			case 'o': 
				hitu (5, rnd (6), mdat -> mname);
				/*  tmp= ??  */
				if (hitu (5, rnd (6), mdat -> mname) && ctmp &&
						!u.ustuck && rn2 (2)) {
					u.ustuck = mtmp;
					pseebl ("%s has grabbed you!",
							mdat -> mname);
					u.uhp -= d (2, 8);
					break;
				}
				if (u.ustuck == mtmp) {
					pline ("You are being crushed.");
					u.uhp -= d (2, 8);
				}
				break;

			case 'P': 
				if (u.uswallow)
					youswld (mtmp, d (2, 4), 12);
				else if (ctmp && !rn2 (4))
					justswld (mtmp);
				else
					hitu (15, d (2, 4), mdat -> mname);
				break;

			case 'Q': 
				x2hitu (3, 2, mdat -> mname);
				break;

			case 'R': 
				if (ctmp && uarm && uarm -> otyp < A_STD_LEATHER
						&& uarm -> spe > -2) {
					pline ("Your armor rusts!");
					--uarm -> spe;
					u.uac++;
					flags.dac = 1;
				}
				break;

			case 'S': 
				if (ctmp && !rn2 (8))
					poisoned ("snake's bite", mdat -> mname);
				break;

			case 's': 
				if (tmp && !rn2 (8))
					poisoned ("scorpion's sting",
							mdat -> mname);
				x2hitu (5, 8, mdat -> mname);
				break;

			case 'T': 
				x2hitu (6, 6, mdat -> mname);
				break;

			case 'U': 
				x2hitu (9, 0, mdat -> mname);/* 0: d(3,4) */
				break;

			case 'v': 
				if (ctmp && !u.ustuck)
					u.ustuck = mtmp;
				break;

			case 'V': 
				if (tmp)
					u.uhp -= 4;
				if (rn2 (3))
					break;
		V: 
				if (ctmp) {/* hit by V or W */
					if (u.ulevel > 1)
						pline ("Goodbye level %d.",
								u.ulevel--);
					else
						u.uhp = 0;
					ctmp = rnd (10);
					u.uhp -= ctmp;
					u.uhpmax -= ctmp;
					u.uexp = 10L * pow (u.ulevel - 1) - 1L;
					flags.dhp = 1;
					flags.dhpmax = 1;
					flags.dulev = 1;
					flags.dexp = 1;
				}
				break;

			case 'W': 
				if (rn2 (5))
					break;
				goto V;
			case 'w': 
				if (tmp)
					wormhit (mtmp);
				break;
			case 'X': 
				for (tmp = 0; tmp < 3; tmp++)
					hitu (8, rnd (3), mdat -> mname);
				break;

			case 'y': 
				if (mtmp -> mcan)
					break;
				cmdel (mtmp);
				if (!u.ublind) {
					pline ("You are blinded by a blast of light!");
					u.ublind = d (4, 12);
					unCoff (COFF, 0);
				}
				return 1;
		}		/* switch */

		if (u.uhp <= 0)
			killer = mdat -> mname;
	}
	else if (mtmp -> ale && cansee (mtmp -> mx, mtmp -> my)) {
		mtmp -> invis = 1;
		newsym (mtmp -> mx, mtmp -> my);
	}
/* Extra movement for fast monsters */
	if (mdat -> mmove - 12 > rnd (12))
		tmp = m_move (mtmp, 1);
	return (tmp == 2);
}

cmdel (mtmp)
register        MONSTER mtmp;
{
	register char   mx = mtmp -> mx, my = mtmp -> my;

	delmon (mtmp);
	if (cansee (mx, my))
		newsym (mx, my);
}

inrange (mtmp)
register        MONSTER mtmp;
{
	int     zx, zy;
	register char   tx = u.ux - mtmp -> mx, ty = u.uy - mtmp -> my;

/* This mess figures out if the person is within 8 */
	if ((!tx && abs (ty) < 8) || (!ty && abs (tx) < 8) ||
			(abs (tx) == abs (ty) && abs (tx) < 8)) {
		if (tx == 0)
			zx = 0;
		else
			zx = tx / abs (tx);
		if (ty == 0)
			zy = 0;
		else
			zy = ty / abs (ty);
/* If we don't save dx and dy a capital move may screw up: */
		tx = dx;
		ty = dy;
		if (bhit (zx, zy, 8) == mtmp)
			buzz (Z_FIRE, mtmp -> mx, mtmp -> my, dx, dy);
		dx = zx;
		dy = zy;
	}
}

m_move (mtmp, after)
register        MONSTER mtmp;
{
	register        MONSTER mtmp2;
	register        nix, niy, omx, omy, appr, nearer, cnt, zx, zy;
	char    ddx, ddy, mmoved = 0;

/* My dog gets a special treatment */
	if (mtmp -> mtame)
		return dog_move (mtmp, after);

	if (u.uswallow)
		return (1);	/* a3 */
/* Likewise for shopkeeper */
	if (mtmp == shopkeeper)
		return shk_move ();
	if (mtmp == vaultkeeper && !mtmp -> angry)
		return (0);

	if (mtmp -> data -> mlet == 't' && !rn2 (19)) {
		if (rn2 (2)) {
			ddx = mtmp -> mx;
			ddy = mtmp -> my;
			mnexto (mtmp);/* Doesn't change old position */
			levlsym (ddx, ddy, 't');
		}
		else
			rloc (mtmp);/* Rloc does */
		return 1;
	}
	if (!mtmp -> mcan)
		switch (mtmp -> data -> mlet) {
			case 'D': 
				inrange (mtmp);
				break;
			case 'U': 
				if (!rn2 (10) && !u.uconfused &&
						cansee (mtmp -> mx, mtmp -> my)) {
					pline ("You are confused!");
					u.uconfused = d (3, 4);
				}
				if (!mtmp -> mflee && u.uswallow &&
						u.ustuck != mtmp)
					return 1;
		}
	appr = 1;
	if (mtmp -> mflee)
		appr = -1;
	if (mtmp -> mconf || u.uinvis || (index ("BI", mtmp -> data -> mlet) &&
				!rn2 (3)))
		appr = 0;
	omx = mtmp -> mx;
	omy = mtmp -> my;
	nix = omx;
	niy = omy;
	cnt = 0;
	for (ddx = -1; ddx <= 1; ddx++)
		for (ddy = -1; ddy <= 1; ddy++)
			if (r_free (zx = omx + ddx, zy = omy + ddy, mtmp)
					&& (ddx || ddy)
					&& !(ddx && ddy && (levl[omx][omy].typ == DOOR ||
							levl[zx][zy].typ == DOOR))) {
				if (!mtmp -> mconf && m_at (zx, zy))
					continue;
				nearer = (dist (zx, zy) < dist (nix, niy));
				if ((appr > 0 && nearer) || (appr < 0 &&
							!nearer) ||
						(!mmoved && mtmp -> wormno) ||
						(!appr && !rn2 (++cnt))) {
					nix = zx;
					niy = zy;
					mmoved++;
				}
			}
	if (mmoved) {
		if (mtmp -> mconf && (mtmp2 = m_at (nix, niy))) {
			if (hitmm (mtmp, mtmp2) == 1 && rn2 (4) &&
					hitmm (mtmp2, mtmp) == 2)
				return 2;
			return 0;
		}
		if (!mtmp -> ale) {
			mtmp -> mx = nix;
			mtmp -> my = niy;
		}
		else if (levl[nix][niy].typ == POOL) {
			mtmp -> mx = nix;
			mtmp -> my = niy;
		}

		if (mtmp -> wormno && mtmp -> data -> mlet == 'w')
			worm_move (mtmp);
	}
	else {
		if (!rn2 (10) && index ("tNL", mtmp -> data -> mlet)) {
			rloc (mtmp);
			return 0;
		}
		if (mtmp -> wormno)
			worm_nomove (mtmp);
	}
	if (mmoved || !cansee (omx, omy))
		levlsym (omx, omy, mtmp -> data -> mlet);
	pmon (mtmp);
	return (mmoved);
}
