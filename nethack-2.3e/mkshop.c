/*	SCCS Id: @(#)mkshop.c	2.3	87/12/12 */
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */

/*
 * Entry points:
 *	mkroom() -- make and stock a room of a given type
 *	nexttodoor() -- return TRUE if adjacent to a door
 *	has_dnstairs() -- return TRUE if given room has a down staircase
 *	has_upstairs() -- return TRUE if given room has an up staircase
 *	dist2() -- Euclidean square-of-distance function
 *	courtmon() -- generate a court monster
 *
 * (note: this module should become mkroom.c in the next major release)
 */
#ifndef QUEST
#include <stdlib.h>
#include "hack.h"
#include "mkroom.h"

static void mkshop(void);
static void mkzoo(int type);
static struct permonst *morguemon(void);
static void mkswamp(void);
static int sq(int a);

static boolean
isbig(register struct mkroom *sroom)
{
    register int area = (sroom->hx - sroom->lx) * (sroom->hy - sroom->ly);
    return (area > 20);
}

/* make and stock a room of a given type */
void
mkroom(int roomtype)
{
    if (roomtype >= SHOPBASE)
        mkshop(); /* someday, we should be able to specify shop type */
    else
        switch (roomtype) {
        case COURT:
            mkzoo(COURT);
            break;
        case ZOO:
            mkzoo(ZOO);
            break;
        case BEEHIVE:
            mkzoo(BEEHIVE);
            break;
        case MORGUE:
            mkzoo(MORGUE);
            break;
        case BARRACKS:
            mkzoo(BARRACKS);
            break;
        case SWAMP:
            mkswamp();
            break;
        default:
            impossible("Tried to make a room of type %d.", roomtype);
        }
}

static void
mkshop(void)
{
    register struct mkroom *sroom;
    int roomno, i = -1;
#ifdef WIZARD
    /* first determine shoptype */
    if (wizard) {
        register char *ep = getenv("SHOPTYPE");
        if (ep) {
            if (*ep == 'z' || *ep == 'Z') {
                mkzoo(ZOO);
                return;
            }
            if (*ep == 'm' || *ep == 'M') {
                mkzoo(MORGUE);
                return;
            }
            if (*ep == 'b' || *ep == 'B') {
                mkzoo(BEEHIVE);
                return;
            }
#ifdef NEWCLASS
            if (*ep == 't' || *ep == 'T') {
                mkzoo(COURT);
                return;
            }
#endif
#ifdef SAC
            if (*ep == '3') {
                mkzoo(BARRACKS);
                return;
            }
#endif /* SAC */
            if (*ep == 's' || *ep == 'S') {
                mkswamp();
                return;
            }
            for (i = 0; shtypes[i].name; i++)
                if (*ep == shtypes[i].symb)
                    goto gottype;
            i = -1;
        }
    }
gottype:
#endif
    for (sroom = &rooms[0], roomno = 0;; sroom++, roomno++) {
        if (sroom->hx < 0)
            return;
        if (sroom - rooms >= nroom) {
            pline("rooms not closed by -1?");
            return;
        }
        if (sroom->rtype != OROOM)
            continue;
        if (!sroom->rlit || has_dnstairs(sroom) || has_upstairs(sroom))
            continue;
        if (
#ifdef WIZARD
            (wizard && getenv("SHOPTYPE") && sroom->doorct != 0) ||
#endif
            sroom->doorct == 1)
            break;
    }

    if (i < 0) { /* shoptype not yet determined */
        register int j;

        /* pick a shop type at random */
        for (j = rn2(100), i = 0; j -= shtypes[i].prob; i++)
            if (j < 0)
                break;

        /* big rooms cannot be wand or book shops,
         * - so make them general stores
         */
        if (isbig(sroom)
            && (shtypes[i].symb == WAND_SYM
#ifdef SPELLS
                || shtypes[i].symb == SPBOOK_SYM
#endif
                ))
            i = 0;
    }
    sroom->rtype = SHOPBASE + i;

    /* stock the room with a shopkeeper and artifacts */
    stock_room(&(shtypes[i]), sroom);
}

static void
mkzoo(int type)
{
    register struct mkroom *sroom;
    register struct monst *mon;
    register int sh, sx, sy, i;
    int goldlim = 500 * dlevel;
    int moct = 0;

    i = nroom;
    for (sroom = &rooms[rn2(nroom)];; sroom++) {
        if (sroom == &rooms[nroom])
            sroom = &rooms[0];
        if (!i-- || sroom->hx < 0)
            return;
        if (sroom->rtype != OROOM)
            continue;
        if (has_upstairs(sroom) || (has_dnstairs(sroom) && rn2(3)))
            continue;
        if (sroom->doorct == 1 || !rn2(5))
            break;
    }
    sroom->rtype = type;
    sh = sroom->fdoor;
    for (sx = sroom->lx; sx <= sroom->hx; sx++)
        for (sy = sroom->ly; sy <= sroom->hy; sy++) {
            if ((sx == sroom->lx && doors[sh].x == sx - 1)
                || (sx == sroom->hx && doors[sh].x == sx + 1)
                || (sy == sroom->ly && doors[sh].y == sy - 1)
                || (sy == sroom->hy && doors[sh].y == sy + 1))
                continue;
            mon = makemon(
#ifdef NEWCLASS
                (type == COURT)
                    ? courtmon()
                    :
#endif
#ifdef SAC
                    (type == BARRACKS) ? PM_SOLDIER :
#endif
                                       (type == MORGUE)
                                           ? morguemon()
                                           : (type == BEEHIVE)
                                                 ? PM_KILLER_BEE
                                                 : (struct permonst *) 0,
                sx, sy);
            if (mon)
                mon->msleep = 1;
            switch (type) {
            case ZOO:
                i = sq(dist2(sx, sy, doors[sh].x, doors[sh].y));
                if (i >= goldlim)
                    i = 5 * dlevel;
                goldlim -= i;
                mkgold((long) (10 + rn2(i)), sx, sy);
                break;
            case MORGUE:
                /* Usually there is one dead body in the morgue */
                if (!moct && rn2(3)) {
                    mksobj_at(CORPSE, sx, sy);
                    moct++;
                }
                break;
            case BEEHIVE:
                if (!rn2(3))
                    mksobj_at(LUMP_OF_ROYAL_JELLY, sx, sy);
                break;
            }
        }
#ifdef NEWCLASS
    if (type == COURT) {
        sx = sroom->lx + (rn2(sroom->hx - sroom->lx));
        sy = sroom->ly + (rn2(sroom->hy - sroom->ly));
        levl[sx][sy].typ = THRONE;
        levl[sx][sy].scrsym = THRONE_SYM;
        mkgold((long) rn1(50 * dlevel, 10), sx, sy);
    }
#endif
}

static struct permonst *
morguemon(void)
{
    register int i = rn2(100), hd = rn2(dlevel);

    if (hd > 10 && i < 10)
        return (PM_DEMON);
    if (hd > 8 && i > 85)
        return (PM_VAMPIRE);
    return ((i < 40) ? PM_GHOST : (i < 60) ? PM_WRAITH : PM_ZOMBIE);
}

static void
mkswamp(void) /* Michiel Huisjes & Fred de Wilde */
{
    register struct mkroom *sroom;
    register int sx, sy, i, eelct = 0;

    for (i = 0; i < 5; i++) { /* 5 tries */
        sroom = &rooms[rn2(nroom)];
        if (sroom->hx < 0 || sroom->rtype != OROOM || has_upstairs(sroom)
            || has_dnstairs(sroom))
            continue;

        /* satisfied; make a swamp */
        sroom->rtype = SWAMP;
        for (sx = sroom->lx; sx <= sroom->hx; sx++)
            for (sy = sroom->ly; sy <= sroom->hy; sy++)
                if ((sx + sy) % 2 && !o_at(sx, sy) && !t_at(sx, sy)
                    && !m_at(sx, sy) && !nexttodoor(sx, sy)) {
                    levl[sx][sy].typ = POOL;
                    levl[sx][sy].scrsym = POOL_SYM;
                    if (!eelct || !rn2(4)) {
                        (void) makemon(PM_EEL, sx, sy);
                        eelct++;
                    }
                }
    }
}

boolean
nexttodoor(int sx, int sy)
{
    register int dx, dy;
    register struct rm *lev;
    for (dx = -1; dx <= 1; dx++)
        for (dy = -1; dy <= 1; dy++)
            if ((lev = &levl[sx + dx][sy + dy])->typ == DOOR
                || lev->typ == SDOOR || lev->typ == LDOOR)
                return (TRUE);
    return (FALSE);
}

boolean
has_dnstairs(register struct mkroom *sroom)
{
    return (sroom->lx <= xdnstair && xdnstair <= sroom->hx
            && sroom->ly <= ydnstair && ydnstair <= sroom->hy);
}

boolean
has_upstairs(register struct mkroom *sroom)
{
    return (sroom->lx <= xupstair && xupstair <= sroom->hx
            && sroom->ly <= yupstair && yupstair <= sroom->hy);
}

int
dist2(int x0, int y0, int x1, int y1)
{
    return ((x0 - x1) * (x0 - x1) + (y0 - y1) * (y0 - y1));
}

static int
sq(int a)
{
    return (a * a);
}
#endif /* QUEST */

#ifdef NEWCLASS
struct permonst *
courtmon(void)
{
    int i = rn2(60) + rn2(3 * dlevel);

    if (i > 100)
        return (PM_DRAGON);
    else if (i > 95)
        return (PM_XORN);
    else if (i > 85)
        return (PM_TROLL);
    else if (i > 75)
        return (PM_ETTIN);
    else if (i > 60)
        return (PM_CENTAUR);
    else if (i > 45)
        return (PM_ORC);
    else if (i > 30)
        return (PM_HOBGOBLIN);
#ifdef KOPS
    else
        return (PM_GNOME);
#else
    else if (i > 15)
        return (PM_GNOME);
    else
        return (PM_KOBOLD);
#endif
}
#endif /* NEWCLASS */
