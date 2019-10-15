/*	SCCS Id: @(#)worn.c	2.3	88/01/21 */
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */

#include "hack.h"

struct worn {
    long w_mask;
    struct obj **w_obj;
} worn[] = { { W_ARM, &uarm },    { W_ARM2, &uarm2 },   { W_ARMH, &uarmh },
             { W_ARMS, &uarms },  { W_ARMG, &uarmg },   { W_ARMU, &uarmu },
             { W_RINGL, &uleft }, { W_RINGR, &uright }, { W_WEP, &uwep },
             { W_BALL, &uball },  { W_CHAIN, &uchain }, { 0, 0 } };

void
setworn(obj, mask)
register struct obj *obj;
long mask;
{
    register struct worn *wp;
    register struct obj *oobj;

    for (wp = worn; wp->w_mask; wp++)
        if (wp->w_mask & mask) {
            oobj = *(wp->w_obj);
            if (oobj && !(oobj->owornmask & wp->w_mask))
                impossible("Setworn: mask = %ld.", wp->w_mask);
            if (oobj)
                oobj->owornmask &= ~wp->w_mask;
            if (obj && oobj && wp->w_mask == W_ARM) {
                if (uarm2) {
                    impossible("Setworn: uarm2 set?");
                } else
                    setworn(uarm, W_ARM2);
            }
            *(wp->w_obj) = obj;
            if (obj)
                obj->owornmask |= wp->w_mask;
        }
    if (uarm2 && !uarm) {
        uarm = uarm2;
        uarm2 = 0;
        uarm->owornmask ^= (W_ARM | W_ARM2);
    }
}

/* called e.g. when obj is destroyed */
void
setnotworn(obj)
register struct obj *obj;
{
    register struct worn *wp;

    for (wp = worn; wp->w_mask; wp++)
        if (obj == *(wp->w_obj)) {
            *(wp->w_obj) = 0;
            obj->owornmask &= ~wp->w_mask;
        }
    if (uarm2 && !uarm) {
        uarm = uarm2;
        uarm2 = 0;
        uarm->owornmask ^= (W_ARM | W_ARM2);
    }
}
