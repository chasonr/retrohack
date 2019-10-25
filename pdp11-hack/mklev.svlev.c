/*
 * Mklev.savelev.c
 */

savelev () {
	register int    fd;
	register        MONSTER mtmp, mtmp2;
	register        GOLD_TRAP gtmp, gtmp2;
	register        OBJECT otmp, otmp2;
	int     minusone = -1;

	if ((fd = creat (tfile, 0644)) < 0)
		panic ("Cannot create %s\n", tfile);
	bwrite (fd, levl, sizeof (levl));
	bwrite (fd, nul, sizeof (unsigned));
	bwrite (fd, (char *) & xupstair, 1);
	bwrite (fd, (char *) & yupstair, 1);
	bwrite (fd, (char *) & xdnstair, 1);
	bwrite (fd, (char *) & ydnstair, 1);
	for (mtmp = fmon; mtmp; mtmp = mtmp2) {
		mtmp2 = mtmp -> nmon;
		bwrite (fd, &mtmp -> mxlth, sizeof (int));
		bwrite (fd, mtmp, mtmp -> mxlth + sizeof (struct monst));

/* Michiel save stolen objects */
		bwrite (fd, nul, sizeof (struct stole));
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
		free (otmp);
	}
	bwrite (fd, nul, sizeof (struct obj));
	bwrite (fd, rooms, sizeof (rooms));
	bwrite (fd, doors, sizeof (doors));
	fgold = TRAP_NULL;
	ftrap = TRAP_NULL;
	fmon = MON_NULL;
	fobj = OBJ_NULL;
}

/*NOSTRICT*/
bwrite (fd, loc, num)
register int    fd, num;
register char  *loc;
{
	if (write (fd, loc, num) != num)
		panic ("Cannot write %d bytes to file #%d", num, fd);
}
