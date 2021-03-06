/*	SCCS Id: @(#)mkmaze.c	2.3	88/03/31 */
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */

#include "hack.h"
#include "mkroom.h" /* not really used */
#include "panic.h"

static void walkfrom(int x, int y);
static void move(int *x, int *y, int dir);
static int okay(int x, int y, int dir);

static struct permonst hell_hound = { "hell hound", 'd', 12, 14, 2, 20, 3, 6, 0 };

void
makemaz(void)
{
    int x, y;
    register int zx, zy;
    coord mm;
#ifndef RPH
    boolean al = (dlevel >= 30 && !flags.made_amulet);
#endif

    for (x = 2; x < COLNO - 1; x++)
        for (y = 2; y < ROWNO - 1; y++)
            levl[x][y].typ = (x % 2 && y % 2) ? 0 : HWALL;
#ifndef RPH
    if (al) {
#else /* make decoy wizard levels */
    if ((dlevel == u.wiz_level)
        || (!rn2(3) && (dlevel > u.medusa_level + 1))) {
#endif
        register struct monst *mtmp;

        zx = 2 * (COLNO / 4) - 1;
        zy = 2 * (ROWNO / 4) - 1;
        for (x = zx - 2; x < zx + 4; x++)
            for (y = zy - 2; y <= zy + 2; y++) {
                levl[x][y].typ =
                    (y == zy - 2 || y == zy + 2 || x == zx - 2 || x == zx + 3)
                        ? POOL
                        : (y == zy - 1 || y == zy + 1 || x == zx - 1
                           || x == zx + 2)
                              ? HWALL
                              : ROOM;
            }
#ifdef RPH
        if (dlevel == u.wiz_level) {
#endif
            (void) mkobj_at(AMULET_SYM, zx, zy);
            flags.made_amulet = 1;
            walkfrom(zx + 4, zy);
            if ((mtmp = makemon(&hell_hound, zx, zy)) != NULL)
                mtmp->msleep = 1;
            if ((mtmp = makemon(PM_WIZARD, zx + 1, zy)) != NULL) {
                mtmp->msleep = 1;
                flags.no_of_wizards = 1;
            }
#ifdef RPH
        } else {
            struct obj *ot;
            /* make a cheap plastic imitation */
            if ((ot = mkobj_at(AMULET_SYM, zx, zy)) != NULL)
                ot->spe = -1;
            walkfrom(zx + 4, zy);
            if ((mtmp = makemon(&hell_hound, zx, zy)) != NULL)
                mtmp->msleep = 1;
            mkmon_at('&', zx + 1, zy);
        }
#endif
    } else {
        mazexy(&mm);
        zx = mm.x;
        zy = mm.y;
        walkfrom(zx, zy);
#ifdef RPH
        if (!rn2(10) || (dlevel == u.medusa_level + 1))
#endif
            (void) mksobj_at(WAN_WISHING, zx, zy);
        (void) mkobj_at(ROCK_SYM, zx, zy); /* put a rock on top of it */
    }

    for (x = 2; x < COLNO - 1; x++)
        for (y = 2; y < ROWNO - 1; y++) {
            switch (levl[x][y].typ) {
            case HWALL:
                levl[x][y].scrsym = HWALL_SYM;
                break;
            case ROOM:
                levl[x][y].scrsym = ROOM_SYM;
                break;
            }
        }
    for (x = rn1(8, 11); x; x--) {
        mazexy(&mm);
        (void) mkobj_at(rn2(2) ? GEM_SYM : 0, mm.x, mm.y);
    }
    for (x = rn1(10, 2); x; x--) {
        mazexy(&mm);
        (void) mkobj_at(ROCK_SYM, mm.x, mm.y);
    }
    mazexy(&mm);
    (void) makemon(PM_MINOTAUR, mm.x, mm.y);
    for (x = rn1(5, 7); x; x--) {
        mazexy(&mm);
        (void) makemon((struct permonst *) 0, mm.x, mm.y);
    }
    for (x = rn1(6, 7); x; x--) {
        mazexy(&mm);
        mkgold(0L, mm.x, mm.y);
    }
    for (x = rn1(6, 7); x; x--)
        mktrap(0, 1, (struct mkroom *) 0);
    mazexy(&mm);
    levl[(xupstair = mm.x)][(yupstair = mm.y)].scrsym = UP_SYM;
    levl[xupstair][yupstair].typ = STAIRS;
    xdnstair = ydnstair = 0;
}

#ifdef DGK
/* Make the mazewalk iterative by faking a stack.  This is needed to
 * ensure the mazewalk is successful in the limited stack space of
 * the program.  This iterative version uses the mimumum amount of stack
 * that is totally safe.
 */
walkfrom(int x, int y)
{
#define CELLS (ROWNO * COLNO) / 4            /* a maze cell is 4 squares */
    char mazex[CELLS + 1], mazey[CELLS + 1]; /* char's are OK */
    int q, a, dir, pos;
    int dirs[4];

    pos = 1;
    mazex[pos] = (char) x;
    mazey[pos] = (char) y;
    while (pos) {
        x = (int) mazex[pos];
        y = (int) mazey[pos];
        levl[x][y].typ = ROOM;
        q = 0;
        for (a = 0; a < 4; a++)
            if (okay(x, y, a))
                dirs[q++] = a;
        if (!q)
            pos--;
        else {
            dir = dirs[rn2(q)];
            move(&x, &y, dir);
            levl[x][y].typ = ROOM;
            move(&x, &y, dir);
            pos++;
            if (pos > CELLS)
                panic("Overflow in walkfrom");
            mazex[pos] = (char) x;
            mazey[pos] = (char) y;
        }
    }
}
#else

void
walkfrom(int x, int y)
{
    register int q, a, dir;
    int dirs[4];
    levl[x][y].typ = ROOM;
    while (1) {
        q = 0;
        for (a = 0; a < 4; a++)
            if (okay(x, y, a))
                dirs[q++] = a;
        if (!q)
            return;
        dir = dirs[rn2(q)];
        move(&x, &y, dir);
        levl[x][y].typ = ROOM;
        move(&x, &y, dir);
        walkfrom(x, y);
    }
}
#endif /* DGK */

static void
move(int *x, int *y, int dir)
{
    switch (dir) {
    case 0:
        --(*y);
        break;
    case 1:
        (*x)++;
        break;
    case 2:
        (*y)++;
        break;
    case 3:
        --(*x);
        break;
    }
}

int
okay(int x, int y, int dir)
{
    move(&x, &y, dir);
    move(&x, &y, dir);
    if (x < 3 || y < 3 || x > COLNO - 3 || y > ROWNO - 3
        || levl[x][y].typ != 0)
        return (0);
    else
        return (1);
}

int
mazexy(coord *cc)
{
    cc->x = 3 + 2 * rn2(COLNO / 2 - 2);
    cc->y = 3 + 2 * rn2(ROWNO / 2 - 2);
    return (0);
}
