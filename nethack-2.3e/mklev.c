/*	SCCS Id: @(#)mklev.c	2.3	87/12/12
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */

#include "hack.h"

extern char *getlogin(), *getenv();
extern struct monst *makemon(), *mkmon_at();
extern struct obj *mkobj_at(), *mksobj_at();
extern struct trap *maketrap();

#ifdef RPH
extern struct permonst pm_medusa;
#endif

#ifdef STOOGES
extern struct permonst pm_larry, pm_curly, pm_moe;
#endif

#define somex() ((int) (rand() % (croom->hx - croom->lx + 1)) + croom->lx)
#define somey() ((int) (rand() % (croom->hy - croom->ly + 1)) + croom->ly)

#include "mkroom.h"
#define XLIM 4 /* define minimum required space around a room */
#define YLIM 3
boolean secret; /* TRUE while making a vault: increase [XY]LIM */
extern struct mkroom rooms[MAXNROFROOMS + 1];
int smeq[MAXNROFROOMS + 1];
extern coord doors[DOORMAX];
int doorindex;
struct rm zerorm;
int comp();
schar nxcor;
boolean goldseen;
int nroom;
extern xchar xdnstair, xupstair, ydnstair, yupstair;

/* Definitions used by makerooms() and addrs() */
#define MAXRS 50 /* max lth of temp rectangle table - arbitrary */
struct rectangle {
    xchar rlx, rly, rhx, rhy;
} rs[MAXRS + 1];
int rscnt, rsmax; /* 0..rscnt-1: currently under consideration */
                  /* rscnt..rsmax: discarded */

makelevel()
{
    register struct mkroom *croom, *troom;
    register unsigned tryct;
#ifndef REGBUG
    register
#endif
        int x,
        y;
#ifdef SPIDERS /* always put a web with a spider */
    struct monst *tmonst;
#endif

    nroom = 0;
    doorindex = 0;
    rooms[0].hx = -1; /* in case we are in a maze */

    for (x = 0; x < COLNO; x++)
        for (y = 0; y < ROWNO; y++)
            levl[x][y] = zerorm;

    oinit(); /* assign level dependent obj probabilities */
#ifdef RPH
    if (u.wiz_level == 0) {
        u.medusa_level = rn1(3, (MAXLEVEL > 30) ? 25 : (MAXLEVEL - 4));
        u.wiz_level = rn1(MAXLEVEL - u.medusa_level, u.medusa_level) + 1;
#ifdef STOOGES
        u.stooge_level = rn1(6, 4);
#endif
    }
    if (dlevel > u.medusa_level) {
        makemaz();
        return;
    }
#else
    if (dlevel >= rn1(3, 26)) { /* there might be several mazes */
        makemaz();
        return;
    }
#endif
    /* construct the rooms */
    nroom = 0;
    secret = FALSE;
    (void) makerooms();

    /* construct stairs (up and down in different rooms if possible) */
    croom = &rooms[rn2(nroom)];
    xdnstair = somex();
    ydnstair = somey();
    levl[xdnstair][ydnstair].scrsym = DN_SYM;
    levl[xdnstair][ydnstair].typ = STAIRS;
#ifdef RPH
    {
        struct monst *mtmp;
        if (dlevel == u.medusa_level)
            if (mtmp = makemon(PM_MEDUSA, xdnstair, ydnstair))
                mtmp->msleep = 1;
    }
#endif
#ifdef STOOGES
    {
        struct monst *mtmp;
        if (dlevel == u.stooge_level) { /* probably should use enexto */
            mtmp = makemon(PM_MOE, xdnstair, ydnstair);
            if (mtmp)
                mtmp->isstooge = 1;
            if (mtmp)
                mtmp->mpeaceful = 1;
            if (goodpos(xdnstair + 1, ydnstair))
                mtmp = makemon(PM_LARRY, xdnstair + 1, ydnstair);
            else if (goodpos(xdnstair - 1, ydnstair))
                mtmp = makemon(PM_LARRY, xdnstair - 1, ydnstair);
            if (mtmp)
                mtmp->isstooge = 1;
            if (mtmp)
                mtmp->mpeaceful = 1;
            if (goodpos(xdnstair, ydnstair + 1))
                mtmp = makemon(PM_CURLY, xdnstair, ydnstair + 1);
            else if (goodpos(xdnstair, ydnstair - 1))
                mtmp = makemon(PM_CURLY, xdnstair, ydnstair - 1);
            if (mtmp)
                mtmp->isstooge = 1;
            if (mtmp)
                mtmp->mpeaceful = 1;
        }
    }
#endif
    if (nroom > 1) {
        troom = croom;
        croom = &rooms[rn2(nroom - 1)];
        if (croom >= troom)
            croom++;
    }
    xupstair = somex(); /* %% < and > might be in the same place */
    yupstair = somey();
    levl[xupstair][yupstair].scrsym = UP_SYM;
    levl[xupstair][yupstair].typ = STAIRS;

    /* for each room: put things inside */
    for (croom = rooms; croom->hx > 0; croom++) {
        /* put a sleeping monster inside */
        /* Note: monster may be on the stairs. This cannot be
           avoided: maybe the player fell through a trapdoor
           while a monster was on the stairs. Conclusion:
           we have to check for monsters on the stairs anyway. */
#ifdef BVH
        if (has_amulet() || !rn2(3))
#else
        if (!rn2(3))
#endif
#ifndef SPIDERS
            (void) makemon((struct permonst *) 0, somex(), somey());
#else
        {
            x = somex();
            y = somey();
            tmonst = makemon((struct permonst *) 0, x, y);
            if (tmonst && tmonst->data->mlet == 's')
                (void) maketrap(x, y, WEB);
        }
#endif
        /* put traps and mimics inside */
        goldseen = FALSE;
        while (!rn2(8 - (dlevel / 6)))
            mktrap(0, 0, croom);
        if (!goldseen && !rn2(3))
            mkgold(0L, somex(), somey());
#ifdef FOUNTAINS
        if (!rn2(10))
            mkfount(0, croom);
#endif
#ifdef SINKS
        if (!rn2(80))
            mksink(croom);
#endif
        if (!rn2(3)) {
            (void) mkobj_at(0, somex(), somey());
            tryct = 0;
            while (!rn2(5)) {
                if (++tryct > 100) {
                    printf("tryct overflow4\n");
                    break;
                }
                (void) mkobj_at(0, somex(), somey());
            }
        }
    }

    qsort((char *) rooms, nroom, sizeof(struct mkroom), comp);
    makecorridors();
    make_niches();

    /* make a secret treasure vault, not connected to the rest */
    if (nroom <= (2 * MAXNROFROOMS / 3))
        if (rn2(3)) {
            troom = &rooms[nroom];
            secret = TRUE;
            if (makerooms()) {
                troom->rtype = VAULT; /* treasure vault */
                for (x = troom->lx; x <= troom->hx; x++)
                    for (y = troom->ly; y <= troom->hy; y++)
                        mkgold((long) (rnd(dlevel * 100) + 50), x, y);
                if (!rn2(3))
                    makevtele();
            }
        }

#ifdef WIZARD
    if (wizard && getenv("SHOPTYPE"))
        mkroom(SHOPBASE);
    else
#endif
        if (dlevel > 1 && dlevel < 20 && rn2(dlevel) < 3)
        mkroom(SHOPBASE);
    else
#ifdef NEWCLASS
        if (dlevel > 4 && !rn2(6))
        mkroom(COURT);
    else
#endif
        if (dlevel > 6 && !rn2(7))
        mkroom(ZOO);
    else if (dlevel > 9 && !rn2(5))
        mkroom(BEEHIVE);
    else if (dlevel > 11 && !rn2(6))
        mkroom(MORGUE);
    else
#ifdef SAC
        if (dlevel > 14 && !rn2(4))
        mkroom(BARRACKS);
    else
#endif
        if (dlevel > 18 && !rn2(6))
        mkroom(SWAMP);
}

makerooms()
{
    register struct rectangle *rsp;
    register int lx, ly, hx, hy, lowx, lowy, hix, hiy, dx, dy;
    int tryct = 0, xlim, ylim;

    /* init */
    xlim = XLIM + secret;
    ylim = YLIM + secret;
    if (nroom == 0) {
        rsp = rs;
        rsp->rlx = rsp->rly = 0;
        rsp->rhx = COLNO - 1;
        rsp->rhy = ROWNO - 1;
        rsmax = 1;
    }
    rscnt = rsmax;

    /* make rooms until satisfied */
    while (rscnt > 0 && nroom < MAXNROFROOMS - 1) {
        if (!secret && nroom > (MAXNROFROOMS / 3)
            && !rn2((MAXNROFROOMS - nroom) * (MAXNROFROOMS - nroom)))
            return (0);

        /* pick a rectangle */
        rsp = &rs[rn2(rscnt)];
        hx = rsp->rhx;
        hy = rsp->rhy;
        lx = rsp->rlx;
        ly = rsp->rly;

        /* find size of room */
        if (secret)
            dx = dy = 1;
        else {
            dx = 2 + rn2((hx - lx - 8 > 20) ? 12 : 8);
            dy = 2 + rn2(4);
            if (dx * dy > 50)
                dy = 50 / dx;
        }

        /* look whether our room will fit */
        if (hx - lx < dx + dx / 2 + 2 * xlim
            || hy - ly < dy + dy / 3 + 2 * ylim) {
            /* no, too small */
            /* maybe we throw this area out */
            if (secret || !rn2(MAXNROFROOMS + 1 - nroom - tryct)) {
                rscnt--;
                rs[rsmax] = *rsp;
                *rsp = rs[rscnt];
                rs[rscnt] = rs[rsmax];
                tryct = 0;
            } else
                tryct++;
            continue;
        }

        lowx = lx + xlim + rn2(hx - lx - dx - 2 * xlim + 1);
        lowy = ly + ylim + rn2(hy - ly - dy - 2 * ylim + 1);
        hix = lowx + dx;
        hiy = lowy + dy;

        if (maker(lowx, dx, lowy, dy)) {
            if (secret)
                return (1);
            addrs(lowx - 1, lowy - 1, hix + 1, hiy + 1);
            tryct = 0;
        } else if (tryct++ > 100)
            break;
    }
    return (0); /* failed to make vault - very strange */
}

addrs(lowx, lowy, hix, hiy)
register int lowx, lowy, hix, hiy;
{
    register struct rectangle *rsp;
    register int lx, ly, hx, hy, xlim, ylim;
    boolean discarded;

    xlim = XLIM + secret;
    ylim = YLIM + secret;

    /* walk down since rscnt and rsmax change */
    for (rsp = &rs[rsmax - 1]; rsp >= rs; rsp--) {
        if ((lx = rsp->rlx) > hix || (ly = rsp->rly) > hiy
            || (hx = rsp->rhx) < lowx || (hy = rsp->rhy) < lowy)
            continue;
        if ((discarded = (rsp >= &rs[rscnt]))) {
            *rsp = rs[--rsmax];
        } else {
            rsmax--;
            rscnt--;
            *rsp = rs[rscnt];
            if (rscnt != rsmax)
                rs[rscnt] = rs[rsmax];
        }
        if (lowy - ly > 2 * ylim + 4)
            addrsx(lx, ly, hx, lowy - 2, discarded);
        if (lowx - lx > 2 * xlim + 4)
            addrsx(lx, ly, lowx - 2, hy, discarded);
        if (hy - hiy > 2 * ylim + 4)
            addrsx(lx, hiy + 2, hx, hy, discarded);
        if (hx - hix > 2 * xlim + 4)
            addrsx(hix + 2, ly, hx, hy, discarded);
    }
}

addrsx(lx, ly, hx, hy, discarded)
register int lx, ly, hx, hy;
boolean discarded; /* piece of a discarded area */
{
    register struct rectangle *rsp;

    /* check inclusions */
    for (rsp = rs; rsp < &rs[rsmax]; rsp++) {
        if (lx >= rsp->rlx && hx <= rsp->rhx && ly >= rsp->rly
            && hy <= rsp->rhy)
            return;
    }

    /* make a new entry */
    if (rsmax >= MAXRS) {
#ifdef WIZARD
        if (wizard)
            pline("MAXRS may be too small.");
#endif
        return;
    }
    rsmax++;
    if (!discarded) {
        *rsp = rs[rscnt];
        rsp = &rs[rscnt];
        rscnt++;
    }
    rsp->rlx = lx;
    rsp->rly = ly;
    rsp->rhx = hx;
    rsp->rhy = hy;
}

comp(x, y)
register struct mkroom *x, *y;
{
    if (x->lx < y->lx)
        return (-1);
    return (x->lx > y->lx);
}

finddpos(cc, xl, yl, xh, yh)
coord *cc;
int xl, yl, xh, yh;
{
    register x, y;

    x = (xl == xh) ? xl : (xl + rn2(xh - xl + 1));
    y = (yl == yh) ? yl : (yl + rn2(yh - yl + 1));
    if (okdoor(x, y))
        goto gotit;

    for (x = xl; x <= xh; x++)
        for (y = yl; y <= yh; y++)
            if (okdoor(x, y))
                goto gotit;

    for (x = xl; x <= xh; x++)
        for (y = yl; y <= yh; y++)
            if (levl[x][y].typ == DOOR || levl[x][y].typ == SDOOR)
                goto gotit;
    /* cannot find something reasonable -- strange */
    x = xl;
    y = yh;
gotit:
    cc->x = x;
    cc->y = y;
    return (0);
}

/* see whether it is allowable to create a door at [x,y] */
okdoor(x, y)
register x, y;
{
    if (levl[x - 1][y].typ == DOOR || levl[x + 1][y].typ == DOOR
        || levl[x][y + 1].typ == DOOR || levl[x][y - 1].typ == DOOR
        || levl[x - 1][y].typ == SDOOR || levl[x + 1][y].typ == SDOOR
        || levl[x][y - 1].typ == SDOOR || levl[x][y + 1].typ == SDOOR
        || (levl[x][y].typ != HWALL && levl[x][y].typ != VWALL)
        || doorindex >= DOORMAX)
        return (0);
    return (1);
}

dodoor(x, y, aroom)
register x, y;
register struct mkroom *aroom;
{
    if (doorindex >= DOORMAX) {
        impossible("DOORMAX exceeded?");
        return;
    }
    if (!okdoor(x, y) && nxcor)
        return;
    dosdoor(x, y, aroom, rn2(8) ? DOOR : SDOOR);
}

dosdoor(x, y, aroom, type)
register x, y;
register struct mkroom *aroom;
register type;
{
    register struct mkroom *broom;
    register tmp;

    if (!IS_WALL(levl[x][y].typ)) /* avoid SDOORs with DOOR_SYM as scrsym */
        type = DOOR;
    levl[x][y].typ = type;
    if (type == DOOR)
        levl[x][y].scrsym = DOOR_SYM;
    aroom->doorct++;
    broom = aroom + 1;
    if (broom->hx < 0)
        tmp = doorindex;
    else
        for (tmp = doorindex; tmp > broom->fdoor; tmp--)
            doors[tmp] = doors[tmp - 1];
    doorindex++;
    doors[tmp].x = x;
    doors[tmp].y = y;
    for (; broom->hx >= 0; broom++)
        broom->fdoor++;
}

/* Only called from makerooms() */
maker(lowx, ddx, lowy, ddy)
schar lowx, ddx, lowy, ddy;
{
    register struct mkroom *croom;
    register x, y, hix = lowx + ddx, hiy = lowy + ddy;
    register xlim = XLIM + secret, ylim = YLIM + secret;

    if (nroom >= MAXNROFROOMS)
        return (0);
    if (lowx < XLIM)
        lowx = XLIM;
    if (lowy < YLIM)
        lowy = YLIM;
    if (hix > COLNO - XLIM - 1)
        hix = COLNO - XLIM - 1;
    if (hiy > ROWNO - YLIM - 1)
        hiy = ROWNO - YLIM - 1;
chk:
    if (hix <= lowx || hiy <= lowy)
        return (0);

    /* check area around room (and make room smaller if necessary) */
    for (x = lowx - xlim; x <= hix + xlim; x++) {
        for (y = lowy - ylim; y <= hiy + ylim; y++) {
            if (levl[x][y].typ) {
#ifdef WIZARD
                if (wizard && !secret)
                    pline("Strange area [%d,%d] in maker().", x, y);
#endif
                if (!rn2(3))
                    return (0);
                if (x < lowx)
                    lowx = x + xlim + 1;
                else
                    hix = x - xlim - 1;
                if (y < lowy)
                    lowy = y + ylim + 1;
                else
                    hiy = y - ylim - 1;
                goto chk;
            }
        }
    }

    croom = &rooms[nroom];

    /* on low levels the room is lit (usually) */
    /* secret vaults are always lit */
    if ((rnd(dlevel) < 10 && rn2(77)) || (ddx == 1 && ddy == 1)) {
        for (x = lowx - 1; x <= hix + 1; x++)
            for (y = lowy - 1; y <= hiy + 1; y++)
                levl[x][y].lit = 1;
        croom->rlit = 1;
    } else
        croom->rlit = 0;
    croom->lx = lowx;
    croom->hx = hix;
    croom->ly = lowy;
    croom->hy = hiy;
    croom->rtype = OROOM;
    croom->doorct = croom->fdoor = 0;

    for (x = lowx - 1; x <= hix + 1; x++)
        for (y = lowy - 1; y <= hiy + 1; y += (hiy - lowy + 2)) {
            levl[x][y].scrsym = HWALL_SYM;
            levl[x][y].typ = HWALL;
        }
    for (x = lowx - 1; x <= hix + 1; x += (hix - lowx + 2))
        for (y = lowy; y <= hiy; y++) {
            levl[x][y].scrsym = VWALL_SYM;
            levl[x][y].typ = VWALL;
        }
    for (x = lowx; x <= hix; x++)
        for (y = lowy; y <= hiy; y++) {
            levl[x][y].scrsym = ROOM_SYM;
            levl[x][y].typ = ROOM;
        }
    levl[lowx - 1][lowy - 1].scrsym = TLCORN_SYM;
    levl[hix + 1][lowy - 1].scrsym = TRCORN_SYM;
    levl[lowx - 1][hiy + 1].scrsym = BLCORN_SYM;
    levl[hix + 1][hiy + 1].scrsym = BRCORN_SYM;

    smeq[nroom] = nroom;
    croom++;
    croom->hx = -1;
    nroom++;
    return (1);
}

makecorridors()
{
    register a, b;

    nxcor = 0;
    for (a = 0; a < nroom - 1; a++)
        join(a, a + 1);
    for (a = 0; a < nroom - 2; a++)
        if (smeq[a] != smeq[a + 2])
            join(a, a + 2);
    for (a = 0; a < nroom; a++)
        for (b = 0; b < nroom; b++)
            if (smeq[a] != smeq[b])
                join(a, b);
    if (nroom > 2)
        for (nxcor = rn2(nroom) + 4; nxcor; nxcor--) {
            a = rn2(nroom);
            b = rn2(nroom - 2);
            if (b >= a)
                b += 2;
            join(a, b);
        }
}

join(a, b)
register a, b;
{
    coord cc, tt;
    register tx, ty, xx, yy;
    register struct rm *crm;
    register struct mkroom *croom, *troom;
    register dx, dy, dix, diy, cct;

    croom = &rooms[a];
    troom = &rooms[b];

    /* find positions cc and tt for doors in croom and troom
       and direction for a corridor between them */

    if (troom->hx < 0 || croom->hx < 0 || doorindex >= DOORMAX)
        return;
    if (troom->lx > croom->hx) {
        dx = 1;
        dy = 0;
        xx = croom->hx + 1;
        tx = troom->lx - 1;
        finddpos(&cc, xx, croom->ly, xx, croom->hy);
        finddpos(&tt, tx, troom->ly, tx, troom->hy);
    } else if (troom->hy < croom->ly) {
        dy = -1;
        dx = 0;
        yy = croom->ly - 1;
        finddpos(&cc, croom->lx, yy, croom->hx, yy);
        ty = troom->hy + 1;
        finddpos(&tt, troom->lx, ty, troom->hx, ty);
    } else if (troom->hx < croom->lx) {
        dx = -1;
        dy = 0;
        xx = croom->lx - 1;
        tx = troom->hx + 1;
        finddpos(&cc, xx, croom->ly, xx, croom->hy);
        finddpos(&tt, tx, troom->ly, tx, troom->hy);
    } else {
        dy = 1;
        dx = 0;
        yy = croom->hy + 1;
        ty = troom->ly - 1;
        finddpos(&cc, croom->lx, yy, croom->hx, yy);
        finddpos(&tt, troom->lx, ty, troom->hx, ty);
    }
    xx = cc.x;
    yy = cc.y;
    tx = tt.x - dx;
    ty = tt.y - dy;
    if (nxcor && levl[xx + dx][yy + dy].typ)
        return;
    dodoor(xx, yy, croom);

    cct = 0;
    while (xx != tx || yy != ty) {
        xx += dx;
        yy += dy;

        /* loop: dig corridor at [xx,yy] and find new [xx,yy] */
        if (cct++ > 500 || (nxcor && !rn2(35)))
            return;

        if (xx == COLNO - 1 || xx == 0 || yy == 0 || yy == ROWNO - 1)
            return; /* impossible */

        crm = &levl[xx][yy];
        if (!(crm->typ)) {
            if (rn2(100)) {
                crm->typ = CORR;
                crm->scrsym = CORR_SYM;
                if (nxcor && !rn2(50))
                    (void) mkobj_at(ROCK_SYM, xx, yy);
            } else {
                crm->typ = SCORR;
                crm->scrsym = STONE_SYM;
            }
        } else if (crm->typ != CORR && crm->typ != SCORR) {
            /* strange ... */
            return;
        }

        /* find next corridor position */
        dix = abs(xx - tx);
        diy = abs(yy - ty);

        /* do we have to change direction ? */
        if (dy && dix > diy) {
            register ddx = (xx > tx) ? -1 : 1;

            crm = &levl[xx + ddx][yy];
            if (!crm->typ || crm->typ == CORR || crm->typ == SCORR) {
                dx = ddx;
                dy = 0;
                continue;
            }
        } else if (dx && diy > dix) {
            register ddy = (yy > ty) ? -1 : 1;

            crm = &levl[xx][yy + ddy];
            if (!crm->typ || crm->typ == CORR || crm->typ == SCORR) {
                dy = ddy;
                dx = 0;
                continue;
            }
        }

        /* continue straight on? */
        crm = &levl[xx + dx][yy + dy];
        if (!crm->typ || crm->typ == CORR || crm->typ == SCORR)
            continue;

        /* no, what must we do now?? */
        if (dx) {
            dx = 0;
            dy = (ty < yy) ? -1 : 1;
            crm = &levl[xx + dx][yy + dy];
            if (!crm->typ || crm->typ == CORR || crm->typ == SCORR)
                continue;
            dy = -dy;
            continue;
        } else {
            dy = 0;
            dx = (tx < xx) ? -1 : 1;
            crm = &levl[xx + dx][yy + dy];
            if (!crm->typ || crm->typ == CORR || crm->typ == SCORR)
                continue;
            dx = -dx;
            continue;
        }
    }

    /* we succeeded in digging the corridor */
    dodoor(tt.x, tt.y, troom);

    if (smeq[a] < smeq[b])
        smeq[b] = smeq[a];
    else
        smeq[a] = smeq[b];
}

make_niches()
{
    register int ct = rnd(nroom / 2 + 1);
#ifdef NEWCLASS
    boolean ltptr = TRUE, vamp = TRUE;

    while (ct--) {
        if (dlevel > 15 && !rn2(6) && ltptr) {
            ltptr = FALSE;
            makeniche(LEVEL_TELEP);
        } else if (dlevel > 5 && dlevel < 25 && !rn2(6) && vamp) {
            vamp = FALSE;
            makeniche(TRAPDOOR);
        } else
            makeniche(NO_TRAP);
    }
#else
    while (ct--)
        makeniche(NO_TRAP);
#endif
}

makevtele()
{
    makeniche(TELEP_TRAP);
}

/* there should be one of these per trap */
char *engravings[] = { "",
                       "",
                       "",
                       "",
                       "",
                       "ad ae?ar um",
                       "?la? ?as ?er?",
                       "",
                       "",
                       ""
#ifdef NEWTRAPS
                       ,
                       "",
                       ""
#endif
#ifdef SPIDERS
                       ,
                       ""
#endif
#ifdef NEWCLASS
                       ,
                       "",
                       "ad ae?ar um"
#endif
#ifdef SPELLS
                       ,
                       ""
#endif
#ifdef KAA
                       ,
                       ""
#ifdef RPH
                       ,
                       ""
#endif
#endif
#ifdef SAC
                       ,
                       ""
#endif
};

makeniche(trap_type)
int trap_type;
{
    register struct mkroom *aroom;
    register struct rm *rm;
    register int vct = 8;
    coord dd;
    register dy, xx, yy;
    register struct trap *ttmp;

    if (doorindex < DOORMAX)
        while (vct--) {
            aroom = &rooms[rn2(nroom - 1)];
            if (aroom->rtype != OROOM)
                continue; /* not an ordinary room */
            if (aroom->doorct == 1 && rn2(5))
                continue;
            if (rn2(2)) {
                dy = 1;
                finddpos(&dd, aroom->lx, aroom->hy + 1, aroom->hx,
                         aroom->hy + 1);
            } else {
                dy = -1;
                finddpos(&dd, aroom->lx, aroom->ly - 1, aroom->hx,
                         aroom->ly - 1);
            }
            xx = dd.x;
            yy = dd.y;
            if ((rm = &levl[xx][yy + dy])->typ)
                continue;
            if (trap_type || !rn2(4)) {
                rm->typ = SCORR;
                rm->scrsym = STONE_SYM;
                if (trap_type) {
                    ttmp = maketrap(xx, yy + dy, trap_type);
                    ttmp->once = 1;
                    if (strlen(engravings[trap_type]) > 0)
                        make_engr_at(xx, yy - dy, engravings[trap_type]);
                }
                dosdoor(xx, yy, aroom, SDOOR);
            } else {
                rm->typ = CORR;
                rm->scrsym = CORR_SYM;
                if (rn2(7))
                    dosdoor(xx, yy, aroom, rn2(5) ? SDOOR : DOOR);
                else {
                    mksobj_at(SCR_TELEPORTATION, xx, yy + dy);
                    if (!rn2(3))
                        (void) mkobj_at(0, xx, yy + dy);
                }
            }
            return;
        }
}

/* make a trap somewhere (in croom if mazeflag = 0) */
mktrap(num, mazeflag, croom)
#ifndef REGBUG
    register
#endif
    int num,
    mazeflag;
#ifndef REGBUG
register
#endif
    struct mkroom *croom;
{
#ifndef REGBUG
    register
#endif
        struct trap *ttmp;
#ifndef REGBUG
    register
#endif
        int kind,
        nopierc, nomimic, fakedoor, fakegold,
#ifdef SPIDERS
        nospider,
#endif
#ifdef NEWCLASS
        nospikes, nolevltp,
#endif
#ifdef SAC
        nolandmine,
#endif
        tryct = 0;

    xchar mx, my;
    extern char fut_geno[];

    if (!num || num >= TRAPNUM) {
        nopierc = (dlevel < 4) ? 1 : 0;
#ifdef NEWCLASS
        nolevltp = (dlevel < 5) ? 1 : 0;
        nospikes = (dlevel < 6) ? 1 : 0;
#endif
#ifdef SPIDERS
        nospider = (dlevel < 7) ? 1 : 0;
#endif
#ifdef SAC
        nolandmine = (dlevel < 5) ? 1 : 0;
#endif
        nomimic = (dlevel < 9 || goldseen) ? 1 : 0;
        if (index(fut_geno, 'M'))
            nomimic = 1;

        do {
            kind = rnd(TRAPNUM - 1);
            if ((kind == PIERC && nopierc) || (kind == MIMIC && nomimic)
#ifdef SPIDERS
                || ((kind == WEB) && nospider)
#endif
#ifdef NEWCLASS
                || (kind == SPIKED_PIT && nospikes)
                || (kind == LEVEL_TELEP && nolevltp)
#endif
#ifdef SAC
                || (kind == LANDMINE && nolandmine)
#endif
            )
                kind = NO_TRAP;
        } while (kind == NO_TRAP);
    } else
        kind = num;

    if (kind == MIMIC) {
        register struct monst *mtmp;

        fakedoor = (!rn2(3) && !mazeflag);
        fakegold = (!fakedoor && !rn2(2));
        if (fakegold)
            goldseen = TRUE;
        do {
            if (++tryct > 200)
                return;
            if (fakedoor) {
                /* note: fakedoor maybe on actual door */
                if (rn2(2)) {
                    if (rn2(2))
                        mx = croom->hx + 1;
                    else
                        mx = croom->lx - 1;
                    my = somey();
                } else {
                    if (rn2(2))
                        my = croom->hy + 1;
                    else
                        my = croom->ly - 1;
                    mx = somex();
                }
            } else if (mazeflag) {
                coord mm;
                mazexy(&mm);
                mx = mm.x;
                my = mm.y;
            } else {
                mx = somex();
                my = somey();
            }
        } while (m_at(mx, my) || levl[mx][my].typ == STAIRS);
        if (mtmp = makemon(PM_MIMIC, mx, my)) {
            mtmp->mimic = 1;
            mtmp->mappearance =
                fakegold
                    ? '$'
                    : fakedoor ? DOOR_SYM
                               : (mazeflag && rn2(2)) ? AMULET_SYM :
#ifdef SPELLS
                                                      "=/)%?![<>+"[rn2(10)];
#else
                                                      "=/)%?![<>"[rn2(9)];
#endif
        }
        return;
    }

    do {
        if (++tryct > 200)
            return;
        if (mazeflag) {
            coord mm;
            mazexy(&mm);
            mx = mm.x;
            my = mm.y;
        } else {
            mx = somex();
            my = somey();
        }
    } while (t_at(mx, my) || levl[mx][my].typ == STAIRS);
    ttmp = maketrap(mx, my, kind);
#ifdef SPIDERS
    if (kind == WEB)
        mkmon_at('s', mx, my);
#endif
    if (mazeflag && !rn2(10) && ttmp->ttyp < PIERC)
        ttmp->tseen = 1;
}

#ifdef FOUNTAINS
mkfount(mazeflag, croom)
register struct mkroom *croom;
register mazeflag;
{
    register xchar mx, my;
    register int tryct = 0;

    do {
        if (++tryct > 200)
            return;
        if (mazeflag) {
            coord mm;
            mazexy(&mm);
            mx = mm.x;
            my = mm.y;
        } else {
            mx = somex();
            my = somey();
        }
    } while (t_at(mx, my) || levl[mx][my].typ == STAIRS
#ifdef NEWCLASS
             || IS_THRONE(levl[mx][my].typ)
#endif
    );

    /* Put a fountain at mx, my */

    levl[mx][my].typ = FOUNTAIN;
    levl[mx][my].scrsym = FOUNTAIN_SYM;
}
#endif /* FOUNTAINS /**/

#ifdef SINKS
mksink(croom)
register struct mkroom *croom;
{
    register xchar mx, my;
    register int tryct = 0;

    do {
        if (++tryct > 200)
            return;
        mx = somex();
        my = somey();
    } while (t_at(mx, my) || levl[mx][my].typ == STAIRS
#ifdef FOUNTAINS
             || IS_FOUNTAIN(levl[mx][my].typ)
#endif
#ifdef NEWCLASS
             || IS_THRONE(levl[mx][my].typ)
#endif
    );

    /* Put a sink at mx, my */

    levl[mx][my].typ = SINK;
    levl[mx][my].scrsym = SINK_SYM;
}
#endif /* SINKS /**/
