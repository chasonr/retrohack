/*
 * Hack.savelev.c
 */

savelev (fd) {
	register        MONSTER mtmp, mtmp2;
	register        GOLD_TRAP gtmp, gtmp2;
	register        OBJECT otmp, otmp2;
	register        WORMSEGMENT wtmp, wtmp2;
	register        tmp;
	int     minusone = -1;

	if (fd < 0)
		panic (CORE, "Save on bad file!");
	bwrite (fd, levl, sizeof (levl));
	bwrite (fd, &moves, sizeof (unsigned));
	bwrite (fd, &xupstair, 1);
	bwrite (fd, &yupstair, 1);
	bwrite (fd, &xdnstair, 1);
	bwrite (fd, &ydnstair, 1);
	for (mtmp = fmon; mtmp; mtmp = mtmp2) {
		mtmp2 = mtmp -> nmon;
		bwrite (fd, &mtmp -> mxlth, sizeof (int));
		bwrite (fd, mtmp, mtmp -> mxlth + sizeof (struct monst));

/* Michiel save stolen objects */
		if (mtmp -> mstole) {
			bwrite (fd, mtmp -> mstole, sizeof (struct stole));
			for (otmp = mtmp -> mstole -> sobj; otmp;
					otmp = otmp -> nobj)
				bwrite (fd, otmp, sizeof (struct obj));
			bwrite (fd, nul, sizeof (struct obj));
			free (mtmp -> mstole);
		}
		else
			bwrite (fd, nul, sizeof (struct stole));
		free (mtmp);
	}
	bwrite (fd, &minusone, sizeof (int));
	for (gtmp = fgold; gtmp; gtmp = gtmp2) {
		gtmp2 = gtmp -> ngen;
		bwrite (fd, gtmp, sizeof (struct gen));
		free (gtmp);
	}
	bwrite (fd, nul, sizeof (struct gen));
	for (gtmp = ftrap; gtmp; gtmp = gtmp2) {
		gtmp2 = gtmp -> ngen;
		bwrite (fd, gtmp, sizeof (struct gen));
		free (gtmp);
	}
	bwrite (fd, nul, sizeof (struct gen));
	for (otmp = fobj; otmp; otmp = otmp2) {
		otmp2 = otmp -> nobj;
		bwrite (fd, otmp, sizeof (struct obj));
		ofree (otmp);
	}
	bwrite (fd, nul, sizeof (struct obj));
	bwrite (fd, rooms, sizeof (rooms));
	bwrite (fd, doors, sizeof (doors));
	fgold = 0;
	ftrap = 0;
	fmon = 0;
	fobj = 0;
	bwrite (fd, wsegs, sizeof (wsegs));
	for (tmp = 1; tmp < 32; tmp++) {
		for (wtmp = wsegs[tmp]; wtmp; wtmp = wtmp2) {
			wtmp2 = wtmp -> nseg;
			bwrite (fd, wtmp, sizeof (struct wseg));
		}
		wsegs[tmp] = 0;
	}
	bwrite (fd, wgrowtime, sizeof (wgrowtime));
	shopkeeper = 0;
}

bwrite (fd, loc, num)
register        fd, num;
register char  *loc;
{
	if (write (fd, loc, num) != num)
		panic (CORE, "Cannot write %d bytes to file #%d", num,
				fd);
}
