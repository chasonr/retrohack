/*	SCCS Id: @(#)prisym.c	2.3	88/03/29 */
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */

#include <stdio.h>
#include "hack.h"
#include "panic.h"

void
atl(int x, int y, int ch)
{
    register struct rm *crm = &levl[x][y];

    if (x < 0 || x > COLNO - 1 || y < 0 || y > ROWNO - 1) {
        impossible("atl(%d,%d,%c)", x, y, ch);
        return;
    }
    if (crm->seen && crm->scrsym == ch)
        return;
    crm->scrsym = ch;
    crm->new = 1;
    on_scr(x, y);
}

void
on_scr(int x, int y)
{
    if (x < scrlx)
        scrlx = x;
    if (x > scrhx)
        scrhx = x;
    if (y < scrly)
        scrly = y;
    if (y > scrhy)
        scrhy = y;
}

/* call: (x,y) - display
        (-1,0) - close (leave last symbol)
        (-1,-1)- close (undo last symbol)
        (-1,let)-open: initialize symbol
        (-2,let)-change let
*/

void
tmp_at(int x, int y)
{
    static schar prevx, prevy;
    static char let;
    if ((int) x == -2) { /* change let call */
        let = y;
        return;
    }
    if ((int) x == -1 && (int) y >= 0) { /* open or close call */
        let = y;
        prevx = -1;
        return;
    }
    if (prevx >= 0 && cansee(prevx, prevy)) {
        delay_output();
        prl(prevx, prevy); /* in case there was a monster */
        at(prevx, prevy, levl[prevx][prevy].scrsym);
    }
    if (x >= 0) { /* normal call */
        if (cansee(x, y))
            at(x, y, let);
        prevx = x;
        prevy = y;
    } else { /* close call */
        let = 0;
        prevx = -1;
    }
}

/* like the previous, but the symbols are first erased on completion */
void
Tmp_at(int x, int y)
{
    static char let;
    static xchar cnt;
    static coord tc[COLNO]; /* but watch reflecting beams! */
    register int xx, yy;
    if ((int) x == -1) {
        if (y > 0) { /* open call */
            let = y;
            cnt = 0;
            return;
        }
        /* close call (do not distinguish y==0 and y==-1) */
        while (cnt--) {
            xx = tc[cnt].x;
            yy = tc[cnt].y;
            prl(xx, yy);
            at(xx, yy, levl[xx][yy].scrsym);
        }
        cnt = let = 0; /* superfluous */
        return;
    }
    if ((int) x == -2) { /* change let call */
        let = y;
        return;
    }
    /* normal call */
    if (cansee(x, y)) {
        if (cnt)
            delay_output();
        at(x, y, let);
        tc[cnt].x = x;
        tc[cnt].y = y;
        if (++cnt >= COLNO)
            panic("Tmp_at overflow?");
        levl[x][y].new = 0; /* prevent pline-nscr erasing --- */
    }
}

void
curs_on_u(void)
{
    curs(u.ux, u.uy + 2);
}

void
pru(void)
{
    if (u.udispl && (Invisible || u.udisx != u.ux || u.udisy != u.uy))
        /* if(! levl[u.udisx][u.udisy].new) */
        if (!vism_at(u.udisx, u.udisy))
            newsym(u.udisx, u.udisy);
    if (Invisible) {
        u.udispl = 0;
        prl(u.ux, u.uy);
    } else if (!u.udispl || u.udisx != u.ux || u.udisy != u.uy) {
        atl(u.ux, u.uy, u.usym);
        u.udispl = 1;
        u.udisx = u.ux;
        u.udisy = u.uy;
    }
    levl[u.ux][u.uy].seen = 1;
}

#ifndef NOWORM
#include "wseg.h"
#endif

/* print a position that is visible for @ */
void
prl(int x, int y)
{
    register struct rm *room;
    register struct monst *mtmp;
    register struct obj *otmp;
    register struct trap *ttmp;

    if (x == u.ux && y == u.uy && (!Invisible)) {
        pru();
        return;
    }
    if (!isok(x, y))
        return;
    room = &levl[x][y];
    if ((!room->typ) || (IS_ROCK(room->typ) && levl[u.ux][u.uy].typ == CORR))
        return;
    if ((mtmp = m_at(x, y)) && !mtmp->mhide
        && (!mtmp->minvis || See_invisible)) {
#ifndef NOWORM
        if (m_atseg)
            pwseg(m_atseg);
        else
#endif
            pmon(mtmp);
    } else if ((otmp = o_at(x, y)) && room->typ != POOL)
        atl(x, y, Hallucination ? rndobjsym() : otmp->olet);
#ifdef SPIDERS
    else if ((!mtmp || mtmp->data == PM_SPIDER) && (ttmp = t_at(x, y))
             && ttmp->ttyp == WEB)
        atl(x, y, WEB_SYM);
#endif
    else if (mtmp && (!mtmp->minvis || See_invisible)) {
        /* must be a hiding monster, but not hiding right now */
        /* assume for the moment that long worms do not hide */
        pmon(mtmp);
    } else if (g_at(x, y) && room->typ != POOL)
        atl(x, y, Hallucination ? rndobjsym() : GOLD_SYM);
    else if (!room->seen || room->scrsym == STONE_SYM) {
        room->new = room->seen = 1;
        newsym(x, y);
        on_scr(x, y);
    }
    room->seen = 1;
}

char
news0(xchar x, xchar y)
{
    register struct obj *otmp;
    register struct trap *ttmp;
    struct rm *room;
    register char tmp;

    room = &levl[x][y];
    if (!room->seen)
        tmp = STONE_SYM;
    else if (room->typ == POOL)
        tmp = POOL_SYM;
    else if (!Blind && (otmp = o_at(x, y)))
        tmp = Hallucination ? rndobjsym() : otmp->olet;
    else if (!Blind && g_at(x, y))
        tmp = Hallucination ? rndobjsym() : GOLD_SYM;
    else if (x == xupstair && y == yupstair)
        tmp = UP_SYM;
    else if (x == xdnstair && y == ydnstair)
        tmp = DN_SYM;
#ifdef SPIDERS
    else if ((ttmp = t_at(x, y)) && ttmp->ttyp == WEB)
        tmp = WEB_SYM;
    else if (ttmp && ttmp->tseen)
        tmp = TRAP_SYM;
#else
    else if ((ttmp = t_at(x, y)) && ttmp->tseen)
        tmp = TRAP_SYM;
#endif
    else
        switch (room->typ) {
        case SCORR:
        case SDOOR:
            tmp = room->scrsym; /* %% wrong after killing mimic ! */
            break;
        case HWALL:
            tmp = room->scrsym; /* OK for corners only */
            if (!IS_CORNER(tmp))
                tmp = HWALL_SYM;
            break;
        case VWALL:
            tmp = VWALL_SYM;
            break;
        case LDOOR:
        case DOOR:
            tmp = DOOR_SYM;
            break;
        case CORR:
            tmp = CORR_SYM;
            break;
        case ROOM:
            if (room->lit || cansee(x, y) || Blind)
                tmp = ROOM_SYM;
            else
                tmp = STONE_SYM;
            break;
#ifdef FOUNTAINS
        case FOUNTAIN:
            tmp = FOUNTAIN_SYM;
            break;
#endif
#ifdef NEWCLASS
        case THRONE:
            tmp = THRONE_SYM;
            break;
#endif
#ifdef SINKS
        case SINK:
            tmp = SINK_SYM;
            break;
#endif
            /*
                    case POOL:
                            tmp = POOL_SYM;
                            break;
            */
        default:
            tmp = ERRCHAR;
        }
    return (tmp);
}

void
newsym(int x, int y)
{
    atl(x, y, news0(x, y));
}

/* used with wand of digging (or pick-axe): fill scrsym and force display */
/* also when a POOL evaporates */
void
mnewsym(int x, int y)
{
    register struct rm *room;
    char newscrsym;

    if (!vism_at(x, y)) {
        room = &levl[x][y];
        newscrsym = news0(x, y);
        if (room->scrsym != newscrsym) {
            room->scrsym = newscrsym;
            room->seen = 0;
        }
    }
}

void
nosee(int x, int y)
{
    register struct rm *room;

    if (!isok(x, y))
        return;
    room = &levl[x][y];
    if (room->scrsym == ROOM_SYM && !room->lit && !Blind) {
        room->scrsym = ' ';
        room->new = 1;
        on_scr(x, y);
    }
}

#ifndef QUEST
void
prl1(int x, int y)
{
    if (u.dx) {
        if (u.dy) {
            prl(x - (2 * u.dx), y);
            prl(x - u.dx, y);
            prl(x, y);
            prl(x, y - u.dy);
            prl(x, y - (2 * u.dy));
        } else {
            prl(x, y - 1);
            prl(x, y);
            prl(x, y + 1);
        }
    } else {
        prl(x - 1, y);
        prl(x, y);
        prl(x + 1, y);
    }
}

void
nose1(int x, int y)
{
    if (u.dx) {
        if (u.dy) {
            nosee(x, u.uy);
            nosee(x, u.uy - u.dy);
            nosee(x, y);
            nosee(u.ux - u.dx, y);
            nosee(u.ux, y);
        } else {
            nosee(x, y - 1);
            nosee(x, y);
            nosee(x, y + 1);
        }
    } else {
        nosee(x - 1, y);
        nosee(x, y);
        nosee(x + 1, y);
    }
}
#endif /* QUEST */

int
vism_at(int x, int y)
{
    register struct monst *mtmp;

    if (x == u.ux && y == u.uy && !Invisible)
        return (1);

    if ((mtmp = m_at(x, y)) != NULL)
        return ((Blind && Telepat) || canseemon(mtmp));

    return (0);
}

#ifdef NEWSCR
void
pobj(register struct obj *obj)
{
    register int show =
        (!obj->oinvis || See_invisible) && cansee(obj->ox, obj->oy);
    if (obj->odispl) {
        if (obj->odx != obj->ox || obj->ody != obj->oy || !show)
            if (!vism_at(obj->odx, obj->ody)) {
                newsym(obj->odx, obj->ody);
                obj->odispl = 0;
            }
    }
    if (show && !vism_at(obj->ox, obj->oy)) {
        atl(obj->ox, obj->oy, obj->olet);
        obj->odispl = 1;
        obj->odx = obj->ox;
        obj->ody = obj->oy;
    }
}
#endif /* NEWSCR */

void
unpobj(register struct obj *obj)
{
    /* 	if(obj->odispl){
                    if(!vism_at(obj->odx, obj->ody))
                            newsym(obj->odx, obj->ody);
                    obj->odispl = 0;
            }
    */
    if (!vism_at(obj->ox, obj->oy))
        newsym(obj->ox, obj->oy);
}
