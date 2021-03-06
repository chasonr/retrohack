/*	SCCS Id: @(#)shk.c	2.3	88/01/24 */
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */

#include <stdio.h>
#include <stdlib.h>
#include "hack.h"
#ifdef QUEST
int shlevel = 0;
struct monst *shopkeeper = 0;
struct obj *billobjs = 0;
void
obfree(struct obj *obj, struct obj *merge)
{
    free((char *) obj);
}
int
inshop(void)
{
    return (0);
}
void
addtobill(void)
{
}
void
subfrombill(void)
{
}
void
splitbill(struct obj *obj, struct obj *otmp)
{
}
int
dopay(void)
{
    return (0);
}
void
paybill(void)
{
}
int
doinvbill(int mode)
{
    return (0);
}
void
shkdead(struct monst *mtmp)
{
}
int
shkcatch(register struct obj *obj)
{
    return (0);
}
int
shk_move(register struct monst *shkp)
{
    return (0);
}
void
replshk(struct monst *mtmp, struct monst *mtmp2)
{
}
char *
shkname(struct monst *mtmp)
{
    return ("");
}

#else
#include "eshk.h"
#include "mfndpos.h"
#include "mkroom.h"

#define ESHK(mon) ((struct eshk *) (&(mon->mextra[0])))
#define NOTANGRY(mon) mon->mpeaceful
#define ANGRY(mon) !NOTANGRY(mon)

static struct obj *bp_to_obj(struct bill_x *bp);

/* Descriptor of current shopkeeper. Note that the bill need not be
   per-shopkeeper, since it is valid only when in a shop. */
static struct monst *shopkeeper = 0;
static struct bill_x *bill;
static int shlevel = 0; /* level of this shopkeeper */
struct obj *billobjs;   /* objects on bill with bp->useup */
/* only accessed here and by save & restore */
static long int total;     /* filled by addupbill() */
static long int followmsg; /* last time of follow message */
static void setpaid(void);
static void findshk(int roomno);
static int dopayobj(struct bill_x *bp);
static int getprice(struct obj *obj);
static int realhunger(void);

/*
        invariants: obj->unpaid iff onbill(obj) [unless bp->useup]
                obj->quan <= bp->bquan
 */

char *
shkname(struct monst *mtmp) /* called in do_name.c */
{
    return (ESHK(mtmp)->shknam);
}

void
shkdead(struct monst *mtmp) /* called in mon.c */
{
    register struct eshk *eshk = ESHK(mtmp);

    if (eshk->shoplevel == dlevel)
        rooms[eshk->shoproom].rtype = OROOM;
    if (mtmp == shopkeeper) {
        setpaid();
        shopkeeper = 0;
        bill = (struct bill_x *) -1000; /* dump core when referenced */
    }
}

void
replshk(struct monst *mtmp, struct monst *mtmp2)
{
    if (mtmp == shopkeeper) {
        shopkeeper = mtmp2;
        bill = &(ESHK(shopkeeper)->bill[0]);
    }
}

static void
setpaid(void)
{   /* caller has checked that shopkeeper exists */
    /* either we paid or left the shop or he just died */
    register struct obj *obj;
    register struct monst *mtmp;
    for (obj = invent; obj; obj = obj->nobj)
        obj->unpaid = 0;
    for (obj = fobj; obj; obj = obj->nobj)
        obj->unpaid = 0;
    for (obj = fcobj; obj; obj = obj->nobj)
        obj->unpaid = 0;
    for (mtmp = fmon; mtmp; mtmp = mtmp->nmon)
        for (obj = mtmp->minvent; obj; obj = obj->nobj)
            obj->unpaid = 0;
    for (mtmp = fallen_down; mtmp; mtmp = mtmp->nmon)
        for (obj = mtmp->minvent; obj; obj = obj->nobj)
            obj->unpaid = 0;
    while ((obj = billobjs) != NULL) {
        billobjs = obj->nobj;
        free((char *) obj);
    }
    ESHK(shopkeeper)->billct = 0;
}

static void
addupbill(void)
{   /* delivers result in total */
    /* caller has checked that shopkeeper exists */
    register int ct = ESHK(shopkeeper)->billct;
    register struct bill_x *bp = bill;
    total = 0;
    while (ct--) {
        total += bp->price * bp->bquan;
        bp++;
    }
}

int
inshop(void)
{
    register int roomno = inroom(u.ux, u.uy);

    /* Did we just leave a shop? */
    if (u.uinshop
        && (u.uinshop != roomno + 1 || shlevel != dlevel || !shopkeeper)) {
        /* This is part of the bugfix for shopkeepers not having their
         * bill paid.  As reported by ab@unido -dgk
         * I made this standard due to the KOPS code below. -mrs
         */
        if (shopkeeper) {
            if (ESHK(shopkeeper)->billct) {
                if (inroom(shopkeeper->mx, shopkeeper->my)
                    == u.uinshop - 1) /* ab@unido */
                    pline("Somehow you escaped the shop without paying!");
                addupbill();
                pline("You stole for a total worth of %ld zorkmids.", total);
                ESHK(shopkeeper)->robbed += total;
                setpaid();
                if ((rooms[ESHK(shopkeeper)->shoproom].rtype == SHOPBASE)
                    == (rn2(3) == 0))
                    ESHK(shopkeeper)->following = 1;
#ifdef KOPS
                { /* Keystone Kops srt@ucla */
                    coord mm;
                    register int cnt = dlevel + rnd(3);
                    /* Create a swarm near the staircase */
                    pline("An alarm sounds throughout the dungeon!");
                    pline("The Keystone Kops are after you!");
                    mm.x = xdnstair;
                    mm.y = ydnstair;
                    while (cnt--) {
                        (void) enexto(&mm, mm.x, mm.y);
                        (void) mkmon_at('K', mm.x, mm.y);
                    }
                    /* Create a swarm near the shopkeeper */
                    cnt = dlevel + rnd(3);
                    mm.x = shopkeeper->mx;
                    mm.y = shopkeeper->my;
                    while (cnt--) {
                        (void) enexto(&mm, mm.x, mm.y);
                        (void) mkmon_at('K', mm.x, mm.y);
                    }
                }
#endif
            }
            shopkeeper = 0;
            shlevel = 0;
        }
        u.uinshop = 0;
    }

    /* Did we just enter a zoo of some kind? */
    if (roomno >= 0) {
        register int rt = rooms[roomno].rtype;
        register struct monst *mtmp;
        if (rt == ZOO) {
            pline("Welcome to David's treasure zoo!");
        } else if (rt == SWAMP) {
            pline("It looks rather muddy down here.");
        } else if (rt == COURT) {
            pline("You are in an opulent throne room!");
        } else if (rt == MORGUE) {
            if (midnight())
                pline("Go away! Go away!");
            else
                pline("You get an uncanny feeling ...");
        } else
            rt = 0;
        if (rt != 0) {
            rooms[roomno].rtype = OROOM;
            for (mtmp = fmon; mtmp; mtmp = mtmp->nmon)
                if (rt != ZOO || !rn2(3))
                    mtmp->msleep = 0;
        }
    }

    /* Did we just enter a shop? */
    if (roomno >= 0 && rooms[roomno].rtype >= SHOPBASE) {
        register int rt = rooms[roomno].rtype;

        if (shlevel != dlevel || !shopkeeper
            || ESHK(shopkeeper)->shoproom != roomno)
            findshk(roomno);
        if (!shopkeeper) {
            rooms[roomno].rtype = OROOM;
            u.uinshop = 0;
        } else if (!u.uinshop) {
            if (!ESHK(shopkeeper)->visitct
                || strncmp(ESHK(shopkeeper)->customer, plname, PL_NSIZ)) {
                /* He seems to be new here */
                ESHK(shopkeeper)->visitct = 0;
                ESHK(shopkeeper)->following = 0;
                (void) strncpy(ESHK(shopkeeper)->customer, plname, PL_NSIZ);
                NOTANGRY(shopkeeper) = 1;
            }
            if (!ESHK(shopkeeper)->following) {
                boolean box, pick;

                pline("Hello %s%s! Welcome%s to %s's %s!",
                      (Badged) ? "Officer " : "", plname,
                      ESHK(shopkeeper)->visitct++ ? " again" : "",
                      shkname(shopkeeper), shtypes[rt - SHOPBASE].name);
                /*			shtypes[rooms[ESHK(shopkeeper)->shoproom].rtype
                 * - SHOPBASE].name);
                 */
                box = carrying(ICE_BOX) != (struct obj *) 0;
                pick = carrying(PICK_AXE) != (struct obj *) 0;
                if (box || pick) {
                    if (dochug(shopkeeper)) {
                        u.uinshop = 0; /* he died moving */
                        return (0);
                    }
                    pline("Will you please leave your %s outside?",
                          (box && pick) ? "box and pick-axe"
                                        : box ? "box" : "pick-axe");
                }
            }
            u.uinshop = roomno + 1;
        }
    }
    return (u.uinshop);
}

static void
findshk(register int roomno)
{
    register struct monst *mtmp;
    for (mtmp = fmon; mtmp; mtmp = mtmp->nmon)
        if (mtmp->isshk && ESHK(mtmp)->shoproom == roomno
            && ESHK(mtmp)->shoplevel == dlevel) {
            shopkeeper = mtmp;
            bill = &(ESHK(shopkeeper)->bill[0]);
            shlevel = dlevel;
            if (ANGRY(shopkeeper)
                && strncmp(ESHK(shopkeeper)->customer, plname, PL_NSIZ))
                NOTANGRY(shopkeeper) = 1;
            /* billobjs = 0; -- this is wrong if we save in a shop */
            /* (and it is harmless to have too many things in billobjs) */
            return;
        }
    shopkeeper = 0;
    shlevel = 0;
    bill = (struct bill_x *) -1000; /* dump core when referenced */
}

static struct bill_x *
onbill(register struct obj *obj)
{
    register struct bill_x *bp;
    if (!shopkeeper)
        return (0);
    for (bp = bill; bp < &bill[ESHK(shopkeeper)->billct]; bp++)
        if (bp->bo_id == obj->o_id) {
            if (!obj->unpaid)
                pline("onbill: paid obj on bill?");
            return (bp);
        }
    if (obj->unpaid)
        pline("onbill: unpaid obj not on bill?");
    return (0);
}

/* called with two args on merge */
void
obfree(struct obj *obj, struct obj *merge)
{
    register struct bill_x *bp = onbill(obj);
    register struct bill_x *bpm;
    if (bp) {
        if (!merge) {
            bp->useup = 1;
            obj->unpaid = 0; /* only for doinvbill */
            obj->nobj = billobjs;
            billobjs = obj;
            return;
        }
        bpm = onbill(merge);
        if (!bpm) {
            /* this used to be a rename */
            impossible("obfree: not on bill??");
            return;
        } else {
            /* this was a merger */
            bpm->bquan += bp->bquan;
            ESHK(shopkeeper)->billct--;
            *bp = bill[ESHK(shopkeeper)->billct];
        }
    }
    free((char *) obj);
}

static void
pay(long tmp, struct monst *shkp)
{
    long robbed = ESHK(shkp)->robbed;

    u.ugold -= tmp;
    shkp->mgold += tmp;
    flags.botl = 1;
    if (robbed) {
        robbed -= tmp;
        if (robbed < 0)
            robbed = 0;
        ESHK(shkp)->robbed = robbed;
    }
}

int
dopay(void)
{
    long ltmp;
    register struct bill_x *bp;
    register struct monst *shkp;
    int pass, tmp;

    multi = 0;
    (void) inshop();
    for (shkp = fmon; shkp; shkp = shkp->nmon)
        if (shkp->isshk && dist(shkp->mx, shkp->my) < 3)
            break;
    if (!shkp && u.uinshop
        && inroom(shopkeeper->mx, shopkeeper->my)
               == ESHK(shopkeeper)->shoproom)
        shkp = shopkeeper;

    if (!shkp) {
        pline("There is nobody here to receive your payment.");
        return (0);
    }
    ltmp = ESHK(shkp)->robbed;
    if (shkp != shopkeeper && NOTANGRY(shkp)) {
        if (!ltmp) {
            pline("You do not owe %s anything.", monnam(shkp));
        } else if (!u.ugold) {
            pline("You have no money.");
        } else {
            long ugold = u.ugold;

            if (u.ugold > ltmp) {
                pline("You give %s the %ld gold pieces %s asked for.",
                      monnam(shkp), ltmp,
                      index("@CGHKLOQTVWZ&ehimt", shkp->data->mlet)
                          ? "he"
                          : (index("nN", shkp->data->mlet) ? "she" : "it"));
                pay(ltmp, shkp);
            } else {
                pline("You give %s all your gold.", monnam(shkp));
                pay(u.ugold, shkp);
            }
            if (ugold < ltmp / 2) {
                pline("Unfortunately, %s doesn't look satisfied.",
                      index("@CGHKLOQTVWZ&ehimt", shkp->data->mlet)
                          ? "he"
                          : (index("nN", shkp->data->mlet) ? "she" : "it"));
            } else {
                ESHK(shkp)->robbed = 0;
                ESHK(shkp)->following = 0;
                if (ESHK(shkp)->shoplevel != dlevel) {
                    /* For convenience's sake, let him disappear */
                    shkp->minvent = 0; /* %% */
                    shkp->mgold = 0;
                    mondead(shkp);
                }
            }
        }
        return (1);
    }

    if (!ESHK(shkp)->billct) {
        pline("You do not owe %s anything.", monnam(shkp));
        if (!u.ugold) {
            pline("Moreover, you have no money.");
            return (1);
        }
        if (ESHK(shkp)->robbed) {
            pline("But since %s shop has been robbed recently,",
                  index("@CGHKLOQTVWZ&ehimt", shkp->data->mlet)
                      ? "his"
                      : (index("nN", shkp->data->mlet) ? "her" : "its"));
            pline("you %srepay %s's expenses.",
                  (u.ugold < ESHK(shkp)->robbed) ? "partially " : "",
                  monnam(shkp));
            pay(min(u.ugold, ESHK(shkp)->robbed), shkp);
            ESHK(shkp)->robbed = 0;
            return (1);
        }
        if (ANGRY(shkp)) {
            pline("But in order to appease %s,", amonnam(shkp, "angry"));
            if (u.ugold >= 1000) {
                ltmp = 1000;
                pline(" you give %s 1000 gold pieces.",
                      index("@CGHKLOQTVWZ&ehimt", shkp->data->mlet)
                          ? "him"
                          : (index("nN", shkp->data->mlet) ? "her" : "it"));
            } else {
                ltmp = u.ugold;
                pline(" you give %s all your money.",
                      index("@CGHKLOQTVWZ&ehimt", shkp->data->mlet)
                          ? "him"
                          : (index("nN", shkp->data->mlet) ? "her" : "it"));
            }
            pay(ltmp, shkp);
            if (strncmp(ESHK(shkp)->customer, plname, PL_NSIZ) || rn2(3)) {
                pline("%s calms down.", Monnam(shkp));
                NOTANGRY(shkp) = 1;
            } else
                pline("%s is as angry as ever.", Monnam(shkp));
        }
        return (1);
    }
    if (shkp != shopkeeper) {
        impossible("dopay: not to shopkeeper?");
        if (shopkeeper)
            setpaid();
        return (0);
    }
    for (pass = 0; pass <= 1; pass++) {
        tmp = 0;
        while (tmp < ESHK(shopkeeper)->billct) {
            bp = &bill[tmp];
            if (!pass && !bp->useup) {
                tmp++;
                continue;
            }
            if (!dopayobj(bp))
                return (1);
#ifdef MSDOS
            *bp = bill[--ESHK(shopkeeper)->billct];
#else
            bill[tmp] = bill[--ESHK(shopkeeper)->billct];
#endif /* MSDOS */
        }
    }
    pline("Thank you for shopping in %s's %s!", shkname(shopkeeper),
          shtypes[rooms[ESHK(shopkeeper)->shoproom].rtype - SHOPBASE].name);
    NOTANGRY(shopkeeper) = 1;
    return (1);
}

/* return 1 if paid successfully */
/*        0 if not enough money */
/*       -1 if object could not be found (but was paid) */
static int
dopayobj(register struct bill_x *bp)
{
    register struct obj *obj;
    long ltmp;

    /* find the object on one of the lists */
    obj = bp_to_obj(bp);

    if (!obj) {
        impossible("Shopkeeper administration out of order.");
        setpaid(); /* be nice to the player */
        return (0);
    }

    if (!obj->unpaid && !bp->useup) {
        impossible("Paid object on bill??");
        return (1);
    }
    obj->unpaid = 0;
    ltmp = bp->price * bp->bquan;
    if (ANGRY(shopkeeper))
        ltmp += ltmp / 3;
    if (u.ugold < ltmp) {
        pline("You don't have gold enough to pay %s.", doname(obj));
        obj->unpaid = 1;
        return (0);
    }
    pay(ltmp, shopkeeper);
    pline("You bought %s for %ld gold piece%s.", doname(obj), ltmp,
          plur(ltmp));
    if (bp->useup) {
        register struct obj *otmp = billobjs;
        if (obj == billobjs)
            billobjs = obj->nobj;
        else {
            while (otmp && otmp->nobj != obj)
                otmp = otmp->nobj;
            if (otmp)
                otmp->nobj = obj->nobj;
            else
                pline("Error in shopkeeper administration.");
        }
        free((char *) obj);
    }
    return (1);
}

/* routine called after dying (or quitting) with nonempty bill */
void
paybill(void)
{
    if (shlevel == dlevel && shopkeeper && ESHK(shopkeeper)->billct) {
        addupbill();
        if (total > u.ugold) {
            shopkeeper->mgold += u.ugold;
            u.ugold = 0;
            pline("%s comes and takes all your possessions.",
                  Monnam(shopkeeper));
        } else {
            u.ugold -= total;
            shopkeeper->mgold += total;
            pline("%s comes and takes the %ld zorkmids you owed him.",
                  Monnam(shopkeeper), total);
        }
        setpaid(); /* in case we create bones */
    }
}

/* find obj on one of the lists */
struct obj *
bp_to_obj(register struct bill_x *bp)
{
    register struct obj *obj;
    register struct monst *mtmp;
    register unsigned id = bp->bo_id;

    if (bp->useup)
        obj = o_on(id, billobjs);
    else if (!(obj = o_on(id, invent)) && !(obj = o_on(id, fobj))
             && !(obj = o_on(id, fcobj))) {
        for (mtmp = fmon; mtmp; mtmp = mtmp->nmon)
            if ((obj = o_on(id, mtmp->minvent)) != NULL)
                break;
        for (mtmp = fallen_down; mtmp; mtmp = mtmp->nmon)
            if ((obj = o_on(id, mtmp->minvent)) != NULL)
                break;
    }
    return (obj);
}

/* called in hack.c when we pickup an object */
void
addtobill(register struct obj *obj)
{
    register struct bill_x *bp;
    char buf[40];

    if (!inshop()
        || (u.ux == ESHK(shopkeeper)->shk.x
            && u.uy == ESHK(shopkeeper)->shk.y)
        || (u.ux == ESHK(shopkeeper)->shd.x
            && u.uy == ESHK(shopkeeper)->shd.y)
        || onbill(obj) /* perhaps we threw it away earlier */
    )
        return;
    if (ESHK(shopkeeper)->billct == BILLSZ) {
        pline("You got that for free!");
        return;
    }
#ifdef DGKMOD
    /* To recognize objects the showkeeper is not interested in. -dgk
     */
    if (obj->no_charge) {
        obj->no_charge = 0;
        return;
    }
#endif
    bp = &bill[ESHK(shopkeeper)->billct];
    bp->bo_id = obj->o_id;
    bp->bquan = obj->quan;
    bp->useup = 0;
    bp->price = getprice(obj);
    strcpy(buf, "For you, ");
    if (ANGRY(shopkeeper))
        strcat(buf, "scum ");
    else {
        switch (rnd(4)
#ifdef HARD
                + u.udemigod
#endif
        ) {
        case 1:
            strcat(buf, "good");
            break;
        case 2:
            strcat(buf, "honored");
            break;
        case 3:
            strcat(buf, "most gracious");
            break;
        case 4:
            strcat(buf, "esteemed");
            break;
        case 5:
            strcat(buf, "holy");
            break;
        }
        if (u.usym != '@')
            strcat(buf, " creature");
        else
            strcat(buf, (flags.female) ? " lady" : " sir");
    }
    pline("%s; only %d %s %s.", buf, bp->price,
          (bp->bquan > 1) ? "per" : "for this", typename((int) obj->otyp));

    ESHK(shopkeeper)->billct++;
    obj->unpaid = 1;
}

void
splitbill(struct obj *obj, struct obj *otmp)
{
    /* otmp has been split off from obj */
    register struct bill_x *bp;
    register int tmp;
    bp = onbill(obj);
    if (!bp) {
        impossible("splitbill: not on bill?");
        return;
    }
    if (bp->bquan < otmp->quan) {
        impossible("Negative quantity on bill??");
    }
    if (bp->bquan == otmp->quan) {
        impossible("Zero quantity on bill??");
    }
    bp->bquan -= otmp->quan;

    /* addtobill(otmp); */
    if (ESHK(shopkeeper)->billct == BILLSZ)
        otmp->unpaid = 0;
    else {
        tmp = bp->price;
        bp = &bill[ESHK(shopkeeper)->billct];
        bp->bo_id = otmp->o_id;
        bp->bquan = otmp->quan;
        bp->useup = 0;
        bp->price = tmp;
        ESHK(shopkeeper)->billct++;
    }
}

void
subfrombill(register struct obj *obj)
{
    long ltmp;
    register struct obj *otmp;
    register struct bill_x *bp;
    if (!inshop()
        || (u.ux == ESHK(shopkeeper)->shk.x
            && u.uy == ESHK(shopkeeper)->shk.y)
        || (u.ux == ESHK(shopkeeper)->shd.x
            && u.uy == ESHK(shopkeeper)->shd.y))
        return;
    if ((bp = onbill(obj)) != 0) {
        obj->unpaid = 0;
        if (bp->bquan > obj->quan) {
            otmp = newobj(0);
            *otmp = *obj;
            bp->bo_id = otmp->o_id = flags.ident++;
            otmp->quan = (bp->bquan -= obj->quan);
            otmp->owt = 0; /* superfluous */
            otmp->onamelth = 0;
            bp->useup = 1;
            otmp->nobj = billobjs;
            billobjs = otmp;
            return;
        }
        ESHK(shopkeeper)->billct--;
        *bp = bill[ESHK(shopkeeper)->billct];
        return;
    }
    if (obj->unpaid) {
        pline("%s didn't notice.", Monnam(shopkeeper));
        obj->unpaid = 0;
        return; /* %% */
    }
    /* he dropped something of his own - probably wants to sell it */
    if (shopkeeper->msleep || shopkeeper->mfroz
        || inroom(shopkeeper->mx, shopkeeper->my)
               != ESHK(shopkeeper)->shoproom)
        return;
    if (ESHK(shopkeeper)->billct == BILLSZ
        || !saleable(rooms[ESHK(shopkeeper)->shoproom].rtype - SHOPBASE, obj)
        || index("_0", obj->olet)) {
        pline("%s seems not interested.", Monnam(shopkeeper));
#ifdef DGKMOD
        obj->no_charge = 1;
#endif
        return;
    }
    ltmp = getprice(obj) * obj->quan;
    if (ANGRY(shopkeeper)) {
        ltmp /= 3;
        NOTANGRY(shopkeeper) = 1;
    } else
        ltmp /= 2;
    if (ESHK(shopkeeper)->robbed) {
        if ((ESHK(shopkeeper)->robbed -= ltmp) < 0)
            ESHK(shopkeeper)->robbed = 0;
        pline("Thank you for your contribution to restock this recently "
              "plundered shop.");
        return;
    }
    if (ltmp > shopkeeper->mgold)
        ltmp = shopkeeper->mgold;
    pay(-ltmp, shopkeeper);
    if (!ltmp) {
        pline("%s gladly accepts %s but cannot pay you at present.",
              Monnam(shopkeeper), doname(obj));
#ifdef DGKMOD
        obj->no_charge = 1;
#endif
    } else
        pline("You sold %s and got %ld gold piece%s.", doname(obj), ltmp,
              plur(ltmp));
}

int
doinvbill(int mode) /* 0: deliver count 1: paged */
{
    register struct bill_x *bp;
    register struct obj *obj;
    long totused, thisused;
    char buf[BUFSZ];

    if (mode == 0) {
        register int cnt = 0;

        if (shopkeeper)
            for (bp = bill; bp - bill < ESHK(shopkeeper)->billct; bp++)
                if (bp->useup
                    || ((obj = bp_to_obj(bp)) && obj->quan < bp->bquan))
                    cnt++;
        return (cnt);
    }

    if (!shopkeeper) {
        impossible("doinvbill: no shopkeeper?");
        return (0);
    }

    set_pager(0);
    if (page_line("Unpaid articles already used up:") || page_line(""))
        goto quit;

    totused = 0;
    for (bp = bill; bp - bill < ESHK(shopkeeper)->billct; bp++) {
        obj = bp_to_obj(bp);
        if (!obj) {
            impossible("Bad shopkeeper administration.");
            goto quit;
        }
        if (bp->useup || bp->bquan > obj->quan) {
            register int cnt, oquan, uquan;

            oquan = obj->quan;
            uquan = (bp->useup ? bp->bquan : bp->bquan - oquan);
            thisused = bp->price * uquan;
            totused += thisused;
            obj->quan = uquan; /* cheat doname */
            (void) sprintf(buf, "x -  %s", doname(obj));
            obj->quan = oquan; /* restore value */
            for (cnt = 0; buf[cnt]; cnt++)
                ;
            while (cnt < 50)
                buf[cnt++] = ' ';
            (void) sprintf(&buf[cnt], " %5ld zorkmids", thisused);
            if (page_line(buf))
                goto quit;
        }
    }
    (void) sprintf(buf, "Total:%50ld zorkmids", totused);
    if (page_line("") || page_line(buf))
        goto quit;
    set_pager(1);
    return (0);
quit:
    set_pager(2);
    return (0);
}

static int
getprice(register struct obj *obj)
{
    register int tmp, ac;
    switch (obj->olet) {
    case AMULET_SYM:
        tmp = rn1(1500, 3500);
        break;
    case TOOL_SYM:
        switch (obj->otyp) {
        case EXPENSIVE_CAMERA:
            tmp = rn1(400, 200);
            break;
#ifdef MARKER
        case MAGIC_MARKER:
            tmp = rn1(100, 50);
            break;
#endif
#ifdef WALKIES
        case LEASH:
            tmp = rn1(40, 20);
            break;
#endif
#ifdef RPH
        case BLINDFOLD:
            tmp = rn1(40, 20);
            break;
        case MIRROR:
            tmp = rn1(80, 40);
            break;
#endif
        case STETHOSCOPE:
            tmp = rn1(100, 80);
            break;
        case CAN_OPENER:
            tmp = rn1(50, 30);
            break;
        default:
            tmp = rn1(20, 10);
            break;
        }
        break;
    case RING_SYM:
        tmp = rn1(200, 100);
        break;
    case WAND_SYM:
        tmp = rn1(300, 150);
        break;
    case SCROLL_SYM:
#ifdef MAIL
        if (obj->otyp == SCR_MAIL)
            tmp = rn1(5, 5);
        else
#endif
            tmp = rn1(200, 100);
        break;
    case POTION_SYM:
        tmp = rn1(200, 100);
        break;
#ifdef SPELLS
    case SPBOOK_SYM:
        tmp = rn1(500, 500);
        break;
#endif
    case FOOD_SYM:

        tmp = (2000 + objects[obj->otyp].nutrition) / realhunger();
        tmp =
            rn1((tmp < 10) ? 10 : tmp, objects[obj->otyp].nutrition / 20 + 5);
        break;
    case GEM_SYM:
        tmp = rn1(120, 60);
        break;
    case ARMOR_SYM:
        ac = ARM_BONUS(obj);
        if (ac <= -10) /* probably impossible */
            ac = -9;
        tmp = ac * ac + 10 * rn1(10 + ac, 10);
        break;
    case WEAPON_SYM:
        if (obj->otyp < BOOMERANG)
            tmp = rn1(4, 2);
        else if (obj->otyp == BOOMERANG || obj->otyp == DAGGER
                 || obj->otyp == CLUB || obj->otyp == SLING)
            tmp = rn1(50, 50);
        else if (obj->otyp == KATANA)
            tmp = rn1(700, 800);
        else if (obj->otyp == LONG_SWORD || obj->otyp == TWO_HANDED_SWORD
                 || obj->otyp == BROAD_SWORD)
            tmp = rn1(500, 500);
        else
            tmp = rn1(150, 100);
        break;
    case CHAIN_SYM:
        pline("Strange ..., carrying a chain?");
    case BALL_SYM:
        tmp = 10;
        break;
    default:
        tmp = 10000;
    }
    return (tmp);
}

static int
realhunger(void)
{ /* not completely foolproof */
    register int tmp = u.uhunger;
    register struct obj *otmp = invent;
    while (otmp) {
        if (otmp->olet == FOOD_SYM && !otmp->unpaid)
            tmp += objects[otmp->otyp].nutrition;
        otmp = otmp->nobj;
    }
    return ((tmp <= 0) ? 1 : tmp);
}

int
shkcatch(register struct obj *obj)
{
    register struct monst *shkp = shopkeeper;

    if (u.uinshop && shkp && !shkp->mfroz && !shkp->msleep && u.dx && u.dy
        && inroom(u.ux + u.dx, u.uy + u.dy) + 1 == u.uinshop
        && shkp->mx == ESHK(shkp)->shk.x && shkp->my == ESHK(shkp)->shk.y
        && u.ux == ESHK(shkp)->shd.x && u.uy == ESHK(shkp)->shd.y) {
        pline("%s nimbly catches the %s.", Monnam(shkp), xname(obj));
        obj->nobj = shkp->minvent;
        shkp->minvent = obj;
        return (1);
    }
    return (0);
}

/*
 * shk_move: return 1: he moved  0: he didnt  -1: let m_move do it
 */
int
shk_move(register struct monst *shkp)
{
    register struct monst *mtmp;
    register struct permonst *mdat = shkp->data;
    register xchar gx, gy, omx, omy, nx, ny, nix, niy;
    register schar appr, i;
    register int udist;
    int z;
    schar shkroom, chi, chcnt, cnt;
    boolean uondoor = FALSE, satdoor, avoid = FALSE, badinv;
    coord poss[9];
    long info[9];
    struct obj *ib = 0;

    omx = shkp->mx;
    omy = shkp->my;

    if ((udist = dist(omx, omy)) < 3) {
        if (ANGRY(shkp)) {
            (void) hitu(shkp, d(mdat->damn, mdat->damd) + 1);
            return (0);
        }
        if (ESHK(shkp)->following) {
            if (strncmp(ESHK(shkp)->customer, plname, PL_NSIZ)) {
                pline("Hello %s%s! I was looking for %s.",
                      (Badged) ? "Officer " : "", plname,
                      ESHK(shkp)->customer);
                ESHK(shkp)->following = 0;
                return (0);
            }
            if (!ESHK(shkp)->robbed) { /* impossible? */
                ESHK(shkp)->following = 0;
                return (0);
            }
            if (moves > followmsg + 4) {
                pline("Hello %s%s! Didn't you forget to pay?",
                      (Badged) ? "Officer " : "", plname);
                followmsg = moves;
                if (Badged)
                    pline("You should be upholding the law!");
#ifdef HARD
                if (!rn2((Badged) ? 3 : 5)) {
                    pline("%s doesn't like customers who don't pay.",
                          Monnam(shkp));
                    NOTANGRY(shkp) = 0;
                }
#endif
            }
            if (udist < 2)
                return (0);
        }
    }

    shkroom = inroom(omx, omy);
    appr = 1;
    gx = ESHK(shkp)->shk.x;
    gy = ESHK(shkp)->shk.y;
    satdoor = (gx == omx && gy == omy);
    if (ESHK(shkp)->following || ((z = holetime()) >= 0 && z * z <= udist)) {
        gx = u.ux;
        gy = u.uy;
        if (shkroom < 0 || shkroom != inroom(u.ux, u.uy))
            if (udist > 4)
                return (-1); /* leave it to m_move */
    } else if (ANGRY(shkp)) {
        long saveBlind = Blinded;
        long saveBlindf = Blindfolded;
        Blinded = Blindfolded = 0;
        if (shkp->mcansee && !Invis && cansee(omx, omy)) {
            gx = u.ux;
            gy = u.uy;
        }
        Blinded = saveBlind;
        Blindfolded = saveBlindf;
        avoid = FALSE;
    } else {
#define GDIST(x, y) ((x - gx) * (x - gx) + (y - gy) * (y - gy))
        if (Invis)
            avoid = FALSE;
        else {
            uondoor =
                (u.ux == ESHK(shkp)->shd.x && u.uy == ESHK(shkp)->shd.y);
            if (uondoor) {
                if (ESHK(shkp)->billct)
                    pline("Hello %s%s! Will you please pay before leaving?",
                          (Badged) ? "Officer " : "", plname);
                badinv = (carrying(PICK_AXE) || carrying(ICE_BOX));
                if (satdoor && badinv)
                    return (0);
                avoid = !badinv;
            } else {
                avoid = (u.uinshop && dist(gx, gy) > 8);
                badinv = FALSE;
            }

            if (((!ESHK(shkp)->robbed && !ESHK(shkp)->billct) || avoid)
                && GDIST(omx, omy) < 3) {
                if (!badinv && !online(omx, omy))
                    return (0);
                if (satdoor)
                    appr = gx = gy = 0;
            }
        }
    }
    if (omx == gx && omy == gy)
        return (0);
    if (shkp->mconf) {
        avoid = FALSE;
        appr = 0;
    }
    nix = omx;
    niy = omy;
    cnt = mfndpos(shkp, poss, info, ALLOW_SSM);
    if (avoid && uondoor) { /* perhaps we cannot avoid him */
        for (i = 0; i < cnt; i++)
            if (!(info[i] & NOTONL))
                goto notonl_ok;
        avoid = FALSE;
    notonl_ok:;
    }
    chi = -1;
    chcnt = 0;
    for (i = 0; i < cnt; i++) {
        nx = poss[i].x;
        ny = poss[i].y;
        if (levl[nx][ny].typ == ROOM || shkroom != ESHK(shkp)->shoproom
            || ESHK(shkp)->following) {
#ifdef STUPID
            /* cater for stupid compilers */
            register int zz;
#endif
            if (uondoor && (ib = sobj_at(ICE_BOX, nx, ny))) {
                nix = nx;
                niy = ny;
                chi = i;
                break;
            }
            if (avoid && (info[i] & NOTONL))
                continue;
            if ((!appr && !rn2(++chcnt)) ||
#ifdef STUPID
                (appr && (zz = GDIST(nix, niy)) && zz > GDIST(nx, ny))
#else
                (appr && GDIST(nx, ny) < GDIST(nix, niy))
#endif
            ) {
                nix = nx;
                niy = ny;
                chi = i;
            }
        }
    }
    if (nix != omx || niy != omy) {
        if (info[chi] & ALLOW_M) {
            mtmp = m_at(nix, niy);
            if (hitmm(shkp, mtmp) == 1 && rn2(3) && hitmm(mtmp, shkp) == 2)
                return (2);
            return (0);
        } else if (info[chi] & ALLOW_U) {
            (void) hitu(shkp, d(mdat->damn, mdat->damd) + 1);
            return (0);
        }
        shkp->mx = nix;
        shkp->my = niy;
        pmon(shkp);
        if (ib) {
            freeobj(ib);
            mpickobj(shkp, ib);
        }
        return (1);
    }
    return (0);
}
#endif /* QUEST */

/*	New version to speed things up.
 *	Compiler dependant, may not always work.
 */
int
online(xchar x, xchar y)
{
    return ((x -= u.ux) == 0 || (y -= u.uy) == 0 || x == y || (x += y) == 0);
}

/*			Original version, just in case...
 *online(x,y) {
 *	return(x==u.ux || y==u.uy || (x-u.ux)*(x-u.ux) == (y-u.uy)*(y-u.uy));
 *}
 */

/* Does this monster follow me downstairs? */
int
follower(register struct monst *mtmp)
{
    return (mtmp->mtame || index("1TVWZi&, ", mtmp->data->mlet)
            || (mtmp->isshk && ESHK(mtmp)->following));
}

/* He is digging in the shop. */
void
shopdig(register int fall)
{
    if (!fall) {
        if (u.utraptype == TT_PIT)
            pline(
                "\"Be careful, sir, or you might fall through the floor.\"");
        else
            pline("\"Please, do not damage the floor here.\"");
    } else if (dist(shopkeeper->mx, shopkeeper->my) < 3) {
        register struct obj *obj, *obj2;

        pline("%s grabs your backpack!", shkname(shopkeeper));
        for (obj = invent; obj; obj = obj2) {
            obj2 = obj->nobj;
            if (obj->owornmask)
                continue;
            freeinv(obj);
            obj->nobj = shopkeeper->minvent;
            shopkeeper->minvent = obj;
            if (obj->unpaid)
                subfrombill(obj);
        }
    }
}
