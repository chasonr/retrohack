/*
 * Hack.worm.c
 */

#include "hack.h"

/* Linked list, tail first */
WORMSEGMENT wsegs[32], wheads[32];

unsigned        wgrowtime[32];

getwn (mtmp)
MONSTER mtmp;
{
	register        tmp;

	for (tmp = 1; tmp < 32; tmp++)
		if (!wsegs[tmp]) {
			mtmp -> wormno = tmp;
			return 1;
		}
	return 0;		/* Level infested with worms */
}

initworm (mtmp)
MONSTER mtmp;
{
	register        WORMSEGMENT wtmp;
	register        tmp = mtmp -> wormno;

	if (!tmp)
		return;
	wheads[tmp] = wsegs[tmp] = wtmp = newseg ();
	wgrowtime[tmp] = 0;
	wtmp -> wx = mtmp -> mx;
	wtmp -> wy = mtmp -> my;
	wtmp -> nseg = 0;
}

worm_move (mtmp)
MONSTER mtmp;
{
	register        WORMSEGMENT wtmp, whd;
	register        tmp = mtmp -> wormno;

	wtmp = newseg ();
	wtmp -> wx = mtmp -> mx;
	wtmp -> wy = mtmp -> my;
	wtmp -> nseg = 0;
	whd = wheads[tmp];
	whd -> nseg = wtmp;
	wheads[tmp] = wtmp;
	seeatl (whd -> wx, whd -> wy, '~');
	if (wgrowtime[tmp] <= moves) {
		if (!wgrowtime[tmp])
			wgrowtime[tmp] = moves + rnd (5);
		else
			wgrowtime[tmp] += 2 + rnd (15);
		mtmp -> orig_hp++;
		mtmp -> mhp++;
		return;
	}
	whd = wsegs[tmp];
	wsegs[tmp] = whd -> nseg;
	remseg (whd);
}

worm_nomove (mtmp)
register        MONSTER mtmp;
{
	register        tmp;
	register        WORMSEGMENT wtmp;

	tmp = mtmp -> wormno;
	wtmp = wsegs[tmp];
	if (wtmp == wheads[tmp])
		return;
	wsegs[tmp] = wtmp -> nseg;
	remseg (wtmp);
	mtmp -> mhp--;		/* Orig_hp not changed! */
}

wormdead (mtmp)
register        MONSTER mtmp;
{
	register        tmp = mtmp -> wormno;
	register        WORMSEGMENT wtmp, wtmp2;

	if (!tmp)
		return;
	for (wtmp = wsegs[tmp]; wtmp; wtmp = wtmp2) {
		wtmp2 = wtmp -> nseg;
		remseg (wtmp);
	}
	wsegs[tmp] = 0;
}

wormhit (mtmp)
register        MONSTER mtmp;
{
	register        tmp = mtmp -> wormno;
	register        MONSTDATA mdat = mtmp -> data;
	register        WORMSEGMENT wtmp;

	if (!tmp)
		return;		/* Worm without tail */
	for (wtmp = wsegs[tmp]; wtmp; wtmp = wtmp -> nseg)
		hitu (mdat -> mhd, 1, mdat -> mname);
}

wormsee (tmp)
register        tmp;
{
	register        WORMSEGMENT wtmp;

	for (wtmp = wsegs[tmp]; wtmp -> nseg; wtmp = wtmp -> nseg)
		if (!cansee (wtmp -> wx, wtmp -> wy))
			levlsym (wtmp -> wx, wtmp -> wy, '~');
}

cutworm (mtmp, x, y, weptyp)
register        MONSTER mtmp;
register char   x, y;
register unsigned       weptyp;
{
	register        WORMSEGMENT wtmp, wtmp2;
	register        MONSTER mtmp2;
	register        tmp, tmp2;

	if (mtmp -> mx == x && mtmp -> my == y)
		return;		/* Hit headon */

/* Cutting goes best with axe or sword */
	tmp = rnd (20);
	if (weptyp == 7 || weptyp == 8 || weptyp == 5)
		tmp += 5;
	if (tmp < 16)
		return;

/* If tail then worm just loses a tail segment */
	tmp = mtmp -> wormno;
	wtmp = wsegs[tmp];
	if (wtmp -> wx == x && wtmp -> wy == y) {
		wsegs[tmp] = wtmp -> nseg;
		remseg (wtmp);
		return;
	}

/* Cut the worm in two halves */
	mtmp2 = newmonst (0);
	*mtmp2 = *mtmp;

/* Sometimes the tail end dies */
	if (rn2 (8) || !getwn (mtmp2)) {/* Michiel was rn2(3) */
		free (mtmp2);
		tmp2 = 0;
	}
	else {
		tmp2 = mtmp2 -> wormno;
		wsegs[tmp2] = wsegs[tmp];
		wgrowtime[tmp2] = 0;
	}
	do {
		if (wtmp -> nseg -> wx == x && wtmp -> nseg -> wy == y) {
			if (tmp2)
				wheads[tmp2] = wtmp;
			wsegs[tmp] = wtmp -> nseg -> nseg;
			remseg (wtmp -> nseg);
			wtmp -> nseg = 0;
			if (tmp2) {
				pline ("You cut the worm in half.");
				mtmp2 -> orig_hp = mtmp2 -> mhp =
					d (mtmp2 -> data -> mhd, 8);
				mtmp2 -> mx = wtmp -> wx;
				mtmp2 -> my = wtmp -> wy;
				mtmp2 -> nmon = fmon;
				fmon = mtmp2;
				pmon (mtmp2);
			}
			else {
				pline ("You cut off part of the worm's tail.");
				remseg (wtmp);
			}
			mtmp -> mhp >>= 1;
			return;
		}
		wtmp2 = wtmp -> nseg;
		if (!tmp2)
			remseg (wtmp);
		wtmp = wtmp2;
	} while (wtmp -> nseg);
	panic (CORE, "Cannot find worm segment");
}

remseg (wtmp)
register        WORMSEGMENT wtmp;
{
	levlsym (wtmp -> wx, wtmp -> wy, '~');
	free (wtmp);
}
