/*	SCCS Id: @(#)apply.c	2.3	88/01/21 */
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */

#include <stdlib.h>
#include "edog.h"
#include "hack.h"
#include "mkroom.h"
#include "panic.h"
static struct monst *bchit(int ddx, int ddy, int range, char sym);
static int next_to(struct monst *mtmp);

#ifdef KAA
static int use_stethoscope(void);
#endif
#ifdef WALKIES
static int use_leash(struct obj *obj);
#endif
static void use_camera(struct obj *obj);
static void use_ice_box(struct obj *obj);
static void use_whistle(struct obj *obj);
static void use_magic_whistle(struct obj *obj);
static int use_pick_axe(struct obj *obj);
#ifdef RPH
static void use_mirror(struct obj *obj);
#endif
static void use_lamp(struct obj *obj);

int
doapply(void)
{
    register struct obj *obj;
    register int res = 1;

    obj = getobj("(", "use or apply");
    if (!obj)
        return (0);

    switch (obj->otyp) {
    case EXPENSIVE_CAMERA:
        use_camera(obj);
        break;
    case ICE_BOX:
        use_ice_box(obj);
        break;
    case PICK_AXE:
        res = use_pick_axe(obj);
        break;

    case MAGIC_WHISTLE:
        if (pl_character[0] == 'W' || u.ulevel > 9) {
            use_magic_whistle(obj);
            break;
        }
        /* fall into next case */
    case WHISTLE:
        use_whistle(obj);
        break;
#ifdef RPH
    case MIRROR:
        use_mirror(obj);
        break;
#endif
    case LAMP:
    case MAGIC_LAMP:
        use_lamp(obj);
        break;
#ifdef WALKIES
    case LEASH:
        use_leash(obj);
        break;
#endif
#ifdef MARKER
    case MAGIC_MARKER:
        dowrite(obj);
        break;
#endif
    case CAN_OPENER:
        if (!carrying(TIN)) {
            pline("You have no can to open.");
            goto xit;
        }
        pline("You cannot open a tin without eating its contents.");
        pline("In order to eat, use the 'e' command.");
        if (obj != uwep)
            pline("Opening the tin will be much easier if you wield the "
                  "can-opener.");
        goto xit;

#ifdef KAA
    case STETHOSCOPE:
        res = use_stethoscope();
        break;
#endif
    case BLINDFOLD:
        if (Blindfolded && obj->owornmask) {
            pline("You remove the blindfold.");
            Blindfolded = 0;
            obj->owornmask = 0;
            if (!Blinded)
                Blinded = 1; /* see on nexte */
            else
                pline("You still cannot see.");
        } else if (!Blindfolded) {
            pline("You put the blindfold on.");
            Blindfolded = INTRINSIC;
            obj->owornmask = W_TOOL;
            seeoff(0);
        } else
            pline("You are already wearing a blindfold!");
        break;
    case BADGE:
        if (Badged && obj->owornmask) {
            pline("You remove the badge.");
            Badged = 0;
            obj->owornmask = 0;
        } else if (!Badged) {
            pline("You pin the badge on your chest.");
            Badged = INTRINSIC;
            obj->owornmask = W_TOOL;
        } else
            pline("You are already wearing a badge!");
        break;
    default:
        pline("Sorry, I don't know how to use that.");
    xit:
        nomul(0);
        return (0);
    }
    nomul(0);
    return (res);
}

/* ARGSUSED */
static void
use_camera(struct obj *obj)
{
    register struct monst *mtmp;
    if (!getdir(1)) { /* ask: in what direction? */
        flags.move = multi = 0;
        return;
    }
    if (u.uswallow) {
        pline("You take a picture of %s's stomach.", monnam(u.ustuck));
        return;
    }
    if (u.dz) {
        pline("You take a picture of the %s.",
              (u.dz > 0) ? "floor" : "ceiling");
        return;
    }
#ifdef KAA
    if (!u.dx && !u.dy && !u.dz) {
        if (!Blind) {
            pline("You are blinded by the flash!");
            Blinded += rnd(25);
            seeoff(0);
        }
        return;
    }
#endif
    if ((mtmp = bchit(u.dx, u.dy, COLNO, '!')) != NULL) {
        if (mtmp->msleep) {
            mtmp->msleep = 0;
            pline("The flash awakens %s.", monnam(mtmp)); /* a3 */
        } else if (mtmp->data->mlet != 'y')
            if (mtmp->mcansee || mtmp->mblinded) {
                register int tmp = dist(mtmp->mx, mtmp->my);
                register int tmp2;
                if (cansee(mtmp->mx, mtmp->my))
                    pline("%s is blinded by the flash!", Monnam(mtmp));
                setmangry(mtmp);
                if (tmp < 9 && !mtmp->isshk && rn2(4)) {
                    mtmp->mflee = 1;
                    if (rn2(4))
                        mtmp->mfleetim = rnd(100);
                }
                if (tmp < 3)
                    mtmp->mcansee = mtmp->mblinded = 0;
                else {
                    tmp2 = mtmp->mblinded;
                    tmp2 += rnd(1 + 50 / tmp);
                    if (tmp2 > 127)
                        tmp2 = 127;
                    mtmp->mblinded = tmp2;
                    mtmp->mcansee = 0;
                }
            }
    }
}

#ifdef KAA
/* Strictly speaking it makes no sense for usage of a stethoscope to
   not take any time; however, unless it did, the stethoscope would be
   almost useless. */
static int
use_stethoscope(void)
{
    register struct monst *mtmp;
    register struct rm *lev;
    register int rx, ry;
    if (!freehand()) {
        pline("You have no free hand!");
        return (1);
    }
    if (!getdir(1)) {
        flags.move = multi = 0;
        return (0);
    }
    if (u.dz < 0 || (u.dz && Levitation)) {
        pline("You can't reach the %s!", u.dz < 0 ? "ceiling" : "floor");
        return (1);
    }
    if (u.dz) {
        pline("The floor seems healthy enough.");
        return (0);
    }
    if (Confusion)
        confdir();
    rx = u.ux + u.dx;
    ry = u.uy + u.dy;
    if (u.uswallow) {
        mstatusline(u.ustuck);
        return (0);
    }
    if ((mtmp = m_at(rx, ry)) != NULL) {
        mstatusline(mtmp);
        return (0);
    }
    if (!isok(rx, ry)) {
        pline("You hear the sounds at the end of the universe.");
        return (0);
    }
    lev = &levl[rx][ry];
    if (lev->typ == SDOOR) {
        pline("You hear a hollow sound!  This must be a secret door!");
        lev->typ = DOOR;
        atl(rx, ry, DOOR_SYM);
        return (0);
    }
    if (lev->typ == SCORR) {
        pline("You hear a hollow sound!  This must be a secret passage!");
        lev->typ = CORR;
        atl(rx, ry, CORR_SYM);
        return (0);
    }
    pline("You hear nothing special.");
    return (0);
}
#endif

static struct obj
    *current_ice_box; /* a local variable of use_ice_box, to be
                      used by its local procedures in/ck_ice_box */
static int
in_ice_box(register struct obj *obj)
{
    if (obj == current_ice_box
        || (Punished && (obj == uball || obj == uchain))) {
        pline("You must be kidding.");
        return (0);
    }
    if (obj->owornmask & (W_ARMOR | W_RING)) {
        pline("You cannot refrigerate something you are wearing.");
        return (0);
    }
    if (obj->owt + current_ice_box->owt > 70) {
        pline("It won't fit.");
        return (1); /* be careful! */
    }
    if (obj == uwep) {
        if (uwep->cursed) {
            pline("Your weapon is welded to your hand!");
            return (0);
        }
        setuwep((struct obj *) 0);
    }
    current_ice_box->owt += obj->owt;
    freeinv(obj);
    obj->o_cnt_id = current_ice_box->o_id;
    obj->nobj = fcobj;
    fcobj = obj;
    obj->age = moves - obj->age; /* actual age */
    return (1);
}

static int
ck_ice_box(register struct obj *obj)
{
    return (obj->o_cnt_id == current_ice_box->o_id);
}

static int
out_ice_box(register struct obj *obj)
{
    register struct obj *otmp;
    if (obj == fcobj)
        fcobj = fcobj->nobj;
    else {
        for (otmp = fcobj; otmp->nobj != obj; otmp = otmp->nobj)
            if (!otmp->nobj)
                panic("out_ice_box");
        otmp->nobj = obj->nobj;
    }
    current_ice_box->owt -= obj->owt;
    obj->age = moves - obj->age; /* simulated point of time */
    (void) addinv(obj);
    return (1);
}

static void
use_ice_box(register struct obj *obj)
{
    register int cnt = 0;
    register struct obj *otmp;
    current_ice_box = obj; /* for use by in/out_ice_box */
    for (otmp = fcobj; otmp; otmp = otmp->nobj)
        if (otmp->o_cnt_id == obj->o_id)
            cnt++;
    if (!cnt)
        pline("Your ice-box is empty.");
    else {
        pline("Do you want to take something out of the ice-box? [yn] ");
        if (readchar() == 'y')
            if (askchain(fcobj, (char *) 0, 0, out_ice_box, ck_ice_box, 0))
                return;
        pline("That was all. Do you wish to put something in? [yn] ");
        if (readchar() != 'y')
            return;
    }
    /* call getobj: 0: allow cnt; #: allow all types; %: expect food */
    otmp = getobj("0#%", "put in");
    if (!otmp || !in_ice_box(otmp))
        flags.move = multi = 0;
}

static struct monst *
bchit(int ddx, int ddy, int range, char sym)
{
    register struct monst *mtmp = (struct monst *) 0;
    register int bchx = u.ux, bchy = u.uy;

    if (sym)
        Tmp_at(-1, sym); /* open call */
    while (range--) {
        bchx += ddx;
        bchy += ddy;
        if ((mtmp = m_at(bchx, bchy)) != NULL)
            break;
        if (!ZAP_POS(levl[bchx][bchy].typ)) {
            bchx -= ddx;
            bchy -= ddy;
            break;
        }
        if (sym)
            Tmp_at(bchx, bchy);
    }
    if (sym)
        Tmp_at(-1, -1);
    return (mtmp);
}

/* ARGSUSED */
static void
use_whistle(struct obj *obj)
{
    register struct monst *mtmp = fmon;
    pline("You produce a high whistling sound.");
    while (mtmp) {
        if (dist(mtmp->mx, mtmp->my) < u.ulevel * 20) {
            if (mtmp->msleep)
                mtmp->msleep = 0;
            if (mtmp->mtame)
                EDOG(mtmp)->whistletime = moves;
        }
        mtmp = mtmp->nmon;
    }
}

/* ARGSUSED */
static void
use_magic_whistle(struct obj *obj)
{
    register struct monst *mtmp = fmon;
    pline("You produce a strange whistling sound.");
    while (mtmp) {
        if (mtmp->mtame)
            mnexto(mtmp);
        mtmp = mtmp->nmon;
    }
}

#ifdef WALKIES
/* ARGSUSED */
static int
use_leash(struct obj *obj)
{
    register struct monst *mtmp = fmon;

    while (mtmp && !mtmp->mleashed)
        mtmp = mtmp->nmon;

    if (mtmp) {
        if (next_to(mtmp)) {
            mtmp->mleashed = 0;
            pline("You remove the leash from your %s.",
#ifdef RPH
                  /* a hack to include the dogs full name.  +4 elminates */
                  /* the 'the' at the start of the name */
                  lmonnam(mtmp) + 4);
#else
                  mtmp->data->mname);
#endif
        } else
            pline("You must be next to your %s to unleash him.",
#ifdef RPH
                  lmonnam(mtmp) + 4);
#else
                  mtmp->data->mname);
#endif
    } else {
        for (mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
            if (mtmp->mtame && next_to(mtmp)) {
                pline("You slip the leash around your %s.",
                      mtmp->data->mname);
                mtmp->mleashed = 1;
                if (mtmp->msleep)
                    mtmp->msleep = 0;
                return (0);
            }
        }
        pline("There's nothing here to put a leash on.");
    }
    return (0);
}

static int
next_to(register struct monst *mtmp)
{
    return ((abs(u.ux - mtmp->mx) <= 1) && (abs(u.uy - mtmp->my) <= 1));
}
#endif

static int dig_effort; /* effort expended on current pos */
static uchar dig_level;
static coord dig_pos;
static boolean dig_down;

static int
dig(void)
{
    register struct rm *lev;
    register int dpx = dig_pos.x, dpy = dig_pos.y;

    /* perhaps a nymph stole his pick-axe while he was busy digging */
    /* or perhaps he teleported away */
    if (u.uswallow || !uwep || uwep->otyp != PICK_AXE || dig_level != dlevel
        || ((dig_down && (dpx != u.ux || dpy != u.uy))
            || (!dig_down && dist(dpx, dpy) > 2)))
        return (0);

    dig_effort += 10 + abon() + uwep->spe + rn2(5);
    if (dig_down) {
        if (!xdnstair) {
            pline("The floor here seems too hard to dig in.");
            return (0);
        }
        if (dig_effort > 250) {
            dighole();
            return (0); /* done with digging */
        }
        if (dig_effort > 50) {
            register struct trap *ttmp = t_at(dpx, dpy);

            if (!ttmp) {
                ttmp = maketrap(dpx, dpy, PIT);
                ttmp->tseen = 1;
                pline("You have dug a pit.");
                u.utrap = rn1(4, 2);
                u.utraptype = TT_PIT;
                return (0);
            }
        }
    } else if (dig_effort > 100) {
        register const char *digtxt;
        register struct obj *obj;

        lev = &levl[dpx][dpy];
        if ((obj = sobj_at(ENORMOUS_ROCK, dpx, dpy)) != NULL) {
            fracture_rock(obj);
            digtxt = "The rock falls apart.";
        } else if (!lev->typ || lev->typ == SCORR) {
            lev->typ = CORR;
            digtxt = "You succeeded in cutting away some rock.";
        } else if (lev->typ == HWALL || lev->typ == VWALL
                   || lev->typ == SDOOR) {
            lev->typ = xdnstair ? DOOR : ROOM;
            digtxt = "You just made an opening in the wall.";
        } else
            digtxt = "Now what exactly was it that you were digging in?";
        mnewsym(dpx, dpy);
        prl(dpx, dpy);
        pline("%s", digtxt); /* after mnewsym & prl */
        return (0);
    } else {
        if (IS_WALL(levl[dpx][dpy].typ)) {
            register int rno = inroom(dpx, dpy);

            if (rno >= 0 && rooms[rno].rtype >= SHOPBASE) {
                pline("This wall seems too hard to dig into.");
                return (0);
            }
        }
        pline("You hit the rock with all your might.");
    }
    return (1);
}

/* When will hole be finished? Very rough indication used by shopkeeper. */
int
holetime(void)
{
    return ((occupation == dig) ? (250 - dig_effort) / 20 : -1);
}

void
dighole(void)
{
    register struct trap *ttmp = t_at(u.ux, u.uy);

    if (!xdnstair) {
        pline("The floor here seems too hard to dig in.");
    } else {
        if (ttmp)
            ttmp->ttyp = TRAPDOOR;
        else
            ttmp = maketrap(u.ux, u.uy, TRAPDOOR);
        ttmp->tseen = 1;
        pline("You've made a hole in the floor.");
        if (!u.ustuck && !Levitation) { /* KAA */
            if (inshop())
                shopdig(1);
            pline("You fall through ...");
            if (u.utraptype == TT_PIT) {
                u.utrap = 0;
                u.utraptype = 0;
            }
            goto_level(dlevel + 1, FALSE);
        }
    }
}

static int
use_pick_axe(struct obj *obj)
{
    char dirsyms[12];
    register char *dsp = dirsyms, *sdp = sdir;
    register struct monst *mtmp;
    register struct rm *lev;
    register int rx, ry, res = 0;

#ifndef FREEHAND
    /* edited by GAN 10/20/86 so that you can't apply the
     * pick-axe while wielding a cursed weapon
     */
    if (!freehand()) {
        pline("You have no free hand to dig with!");
        return (0);
    }
#ifdef KAA
    if (cantwield(u.usym)) {
        pline("You can't hold it strongly enough.");
        return (0);
    }
#endif
#else
    if (obj != uwep) {
        if (uwep && uwep->cursed) {
            /* Andreas Bormann - ihnp4!decvax!mcvax!unido!ab */
            pline("Since your weapon is welded to your hand,");
            pline("you cannot use that pick-axe.");
            return (0);
        }
#ifdef KAA
        if (cantwield(u.usym)) {
            pline("You can't hold it strongly enough.");
            return (0);
        }
        unweapon = TRUE;
#endif
        pline("You now wield %s.", doname(obj));
        setuwep(obj);
        res = 1;
    }
#endif
    while (*sdp) {
        (void) movecmd(*sdp); /* sets u.dx and u.dy and u.dz */
        rx = u.ux + u.dx;
        ry = u.uy + u.dy;
        if (u.dz > 0
            || (u.dz == 0 && isok(rx, ry)
                && (IS_ROCK(levl[rx][ry].typ)
                    || sobj_at(ENORMOUS_ROCK, rx, ry))))
            *dsp++ = *sdp;
        sdp++;
    }
    *dsp = 0;
    pline("In what direction do you want to dig? [%s] ", dirsyms);
    if (!getdir(0)) /* no txt */
        return (res);
    if (u.uswallow && attack(u.ustuck)) /* return(1) */
        ;
    else if (u.dz < 0)
        pline("You cannot reach the ceiling.");
    else
#ifdef KAA
        if (!u.dx && !u.dy && !u.dz) {
        pline("You hit yourself with your own pick-axe.");
        losehp(rnd(2) + dbon(), "self-inflicted wound");
        flags.botl = 1;
        return (1);
    }
#endif
    if (u.dz == 0) {
        if (Confusion)
            confdir();
        rx = u.ux + u.dx;
        ry = u.uy + u.dy;
        if ((mtmp = m_at(rx, ry)) && attack(mtmp))
            return (1);
        if (!isok(rx, ry)) {
            pline("Clash!");
            return (1);
        }
        lev = &levl[rx][ry];
        if (lev->typ == DOOR)
            pline("Your %s against the door.", aobjnam(obj, "clang"));
        else if (!IS_ROCK(lev->typ) && !sobj_at(ENORMOUS_ROCK, rx, ry)) {
            /* ACCESSIBLE or POOL */
            pline("You swing your %s through thin air.",
                  aobjnam(obj, (char *) 0));
        } else {
            if (dig_pos.x != rx || dig_pos.y != ry || dig_level != dlevel
                || dig_down) {
                dig_down = FALSE;
                dig_pos.x = rx;
                dig_pos.y = ry;
                dig_level = dlevel;
                dig_effort = 0;
                pline("You start digging.");
            } else
                pline("You continue digging.");
#ifdef DGKMOD
            set_occupation(dig, "digging", 0);
#else
            occupation = dig;
            occtxt = "digging";
#endif
        }
    } else if (Levitation) {
        pline("You cannot reach the floor.");
    } else {
        if (dig_pos.x != u.ux || dig_pos.y != u.uy || dig_level != dlevel
            || !dig_down) {
            dig_down = TRUE;
            dig_pos.x = u.ux;
            dig_pos.y = u.uy;
            dig_level = dlevel;
            dig_effort = 0;
            pline("You start digging in the floor.");
            if (inshop())
                shopdig(0);
        } else
            pline("You continue digging in the floor.");
#ifdef DGKMOD
        set_occupation(dig, "digging", 0);
#else
        occupation = dig;
        occtxt = "digging";
#endif
    }
    return (1);
}

#ifdef RPH
static void
use_mirror(struct obj *obj)
{
    register struct monst *mtmp;
    register char mlet;

    if (!getdir(1)) { /* ask: in what direction? */
        flags.move = multi = 0;
        return;
    }
    if (u.uswallow) {
        pline("You reflect %s's stomach.", monnam(u.ustuck));
        return;
    }
    if (u.dz) {
        pline("You reflect the %s.", (u.dz > 0) ? "floor" : "ceiling");
        return;
    }
    if (!u.dx && !u.dy && !u.dz) {
        if (!Blind)
            pline("You look as ugly as ever.");
        else {
            if (rn2(4 - u.uluck / 3) || !HTelepat)
                pline("You can't see your ugly face.");
            else {
                const char *tm, *tl;
                int ll;
                if (rn2(4)) {
                    tm = "ugly monster";
                    ll = dlevel - u.medusa_level;
                } else {
                    tm = "intelligent being";
                    ll = dlevel - u.wiz_level;
                }
                if (ll < -10)
                    tl = "far below you";
                else if (ll < -1)
                    tl = "below you";
                else if (ll == -1)
                    tl = "under your feet";
                else if (ll == 0)
                    tl = "very close to you";
                else if (ll == 1)
                    tl = "above your head";
                else if (ll > 10)
                    tl = "far above you";
                else
                    tl = "above you";
                pline("You get an impression that an %s lives %s.", tm, tl);
            }
        }
        return;
    }
    if ((mtmp = bchit(u.dx, u.dy, COLNO, 0)) != NULL) {
        mlet = mtmp->data->mlet;
        if (mtmp->msleep) {
            pline("%s is tired and doesn't look at your mirror.",
                  Monnam(mtmp));
            mtmp->msleep = 0;
        } else if (!mtmp->mcansee) {
            pline("%s can't see anything at the moment.", Monnam(mtmp));
        }
        /* some monsters do special things */
        else if (!mtmp->mcan && index("EUN8", mlet))
            switch (mlet) {
            case '8':
                pline("%s is turned to stone!", Monnam(mtmp));
                killed(mtmp);
                break;
            case 'E':
                pline("%s is frozen by its reflection.", Monnam(mtmp));
                mtmp->mfroz = 1;
                break;
            case 'U':
                pline("%s has confused itself!", Monnam(mtmp));
                mtmp->mconf = 1;
                break;
            case 'N':
                pline("%s looks beautiful in your mirror.", Monnam(mtmp));
                pline("She decides to take it!");
                freeinv(obj);
                mpickobj(mtmp, obj);
                rloc(mtmp);
                break;
            default:
                break;
            }
        else if (mlet == 'V' || mlet == '&')
            pline("%s doesn't seem to reflect anything.", Monnam(mtmp));
        else if (!index("agquv1N", mlet) && rn2(5)) {
            pline("%s is frightened by its reflection.", Monnam(mtmp));
            mtmp->mflee = 1;
            mtmp->mfleetim += d(2, 4);
        } else
            pline("%s doesn't seem to mind %s refection.", Monnam(mtmp),
                  (mlet == '1' ? "his" : (mlet == 'N' ? "her" : "its")));
    } /* if monster hit with mirror */
} /* use_mirror */

#endif

static void
use_lamp(struct obj *obj)
{
    if (obj->spe <= 0 || obj->otyp == MAGIC_LAMP) {
        pline("This lamp has no oil.");
        return;
    }
    litroom(TRUE);
    obj->spe -= 1;
}

int
dorub(void)
{
    if (!(carrying(LAMP) || carrying(MAGIC_LAMP))) {
        pline("You do not have a lamp!");
        return(0);
    }
    if (!(uwep->otyp == LAMP || uwep->otyp == MAGIC_LAMP)) {
        pline("You must wield the lamp to rub it!");
        return(0);
    }
    if (uwep->otyp == MAGIC_LAMP && uwep->spe > 0 && !rn2(3)) {
        uwep->spe = 0;
        djinni_from_bottle();
    } else if (uwep->otyp == MAGIC_LAMP && rn2(2) && !Blind)
        pline("You see a puff of smoke.");
    else
        pline("Nothing happens.");
    return(1);
}
