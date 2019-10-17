/*	SCCS Id: @(#)sit.c	2.3	88/02/02 */
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */

#include "hack.h"

#ifdef NEWCLASS
int
dosit(void)
{
    register int cnt;

    if (Levitation) {
        pline("You are floating in the air, you can't sit!");
    } else if (IS_THRONE(levl[u.ux][u.uy].typ)) {
        pline("As you sit in the opulent throne");
        if (rnd(6) > 4) {
            switch (rnd(13)) {
            case 1:
                pline("you feel suddenly weaker.");
                if (Poison_resistance) {
                    losestr(rn1(1, 2));
                    losehp(rnd(6), "cursed throne");
                } else {
                    losestr(rn1(4, 3));
                    losehp(rnd(10), "cursed throne");
                }
                break;
            case 2:
                pline("you feel suddenly stronger.");
                gainstr(0);
                break;
            case 3:
                pline("A%s charge of electricity shoots through your body!",
                      (Shock_resistance) ? "" : " massive");
                if (Shock_resistance)
                    losehp(rnd(6), "electric chair");
                else
                    losehp(rnd(30), "electric chair");
                break;
            case 4:
                pline("you feel much, much better!");
                if (u.uhp >= (u.uhpmax - 5))
                    u.uhpmax += 4;
                u.uhp = u.uhpmax;
                if (Blinded)
                    Blinded = 1;
                if (Sick)
                    Sick = 0;
                heal_legs();
                flags.botl = 1;
                break;
            case 5:
                if (u.ugold <= 0) {
                    pline("you feel a strange sensation.");
                } else {
                    pline("you notice you have no gold!");
                    u.ugold = 0;
                    flags.botl = 1;
                }
                break;
            case 6:
                if (u.uluck + rn2(5) < 0) {
                    pline("you feel your luck is changing.");
                    change_luck(1);
                } else
                    makewish();
                break;
            case 7:
                cnt = rnd(10);
                pline("you hear a voice echo:");
                pline("Your audience has been summoned, Sire!");
                while (cnt--)
                    (void) makemon(courtmon(), u.ux, u.uy);
                break;
            case 8:
                if (Confusion != 0) {
                    pline("you hear a voice echo:");
                    pline("By your Imperious order Sire...");
                }
                do_genocide();
                break;
            case 9:
                pline("you hear a voice echo:");
                pline("A curse upon you for sitting upon this most holy "
                      "throne!");
                if (u.uluck > 0) {
                    if (!Blind)
                        pline("a cloud of darkness falls upon you.");
                    Blinded += rn1(100, 250);
                    seeoff(0);
                } else
                    rndcurse();
                break;
            case 10:
                if (u.uluck < 0) {
                    pline("an image forms in your mind.");
                    do_mapping();
                } else {
                    pline("your vision clarifies.");
                    HSee_invisible |= INTRINSIC;
                }
                break;
            case 11:
                if (u.uluck < 0) {
                    pline("you feel threatened.");
                    aggravate();
                } else {
                    pline("you feel a wrenching sensation.");
                    tele(); /* teleport him */
                }
                break;
            case 12:
                pline("you are granted a gift of insight!");
                while (!ggetobj("identify", identify, rn2(5)) && invent)
                    ;
                break;
            case 13:
                pline("your mind turns into a pretzel!");
                HConfusion += rn1(7, 16);
                break;
            default:
                impossible("throne effect");
                break;
            }
        } else
            pline("you feel somehow out of place...");

        if (!rn2(3) && IS_THRONE(levl[u.ux][u.uy].typ)) {
            pline("The throne vanishes in a puff of logic.");
            /*			levl[u.ux][u.uy].scrsym = ROOM_SYM; */
            levl[u.ux][u.uy].typ = ROOM;
        }

    } else
        pline("Having fun sitting on the floor???");
    return (1);
}
#endif /* NEWCLASS */

#if defined(NEWCLASS) || defined(PRAYERS) || defined(HARD)
void
rndcurse(void)
{ /* curse a few inventory items at random! */

    int nobj = 0;
    int cnt, onum;
    struct obj *otmp;

    for (otmp = invent; otmp; otmp = otmp->nobj)
        nobj++;
    for (cnt = rnd(6); cnt > 0; cnt--) {
        onum = rn2(nobj);
        for (otmp = invent; onum != 0; onum--)
            otmp = otmp->nobj;

        otmp->cursed++;
    }
}
#endif

void
attrcurse(void)
{ /* remove a random INTRINSIC ability */
    switch (rnd(10)) {
    case 1:
        if (HFire_resistance & INTRINSIC) {
            HFire_resistance &= ~INTRINSIC;
            if (Inhell && !Fire_resistance) {
                pline("You burn to a crisp.");
                killer = "gremlin curse";
                done("died");
            } else
                pline("You feel warmer!");
            break;
        }
    case 2:
        if (HTeleportation & INTRINSIC) {
            HTeleportation &= ~INTRINSIC;
            pline("You don't feel jumpy!");
            break;
        }
    case 3:
        if (HPoison_resistance & INTRINSIC) {
            HPoison_resistance &= ~INTRINSIC;
            pline("You feel a little sick!");
            break;
        }
    case 4:
        if (HTelepat & INTRINSIC) {
            HTelepat &= ~INTRINSIC;
            pline("Your senses fail!");
            break;
        }
    case 5:
        if (HCold_resistance & INTRINSIC) {
            HCold_resistance &= ~INTRINSIC;
            pline("You feel colder!");
            break;
        }
    case 6:
        if (HInvis & INTRINSIC) {
            HInvis &= ~INTRINSIC;
            pline("You feel paranoid!");
            break;
        }
    case 7:
        if (HSee_invisible & INTRINSIC) {
            HSee_invisible &= ~INTRINSIC;
            pline("You think you see something!");
            break;
        }
    case 8:
        if (Fast & INTRINSIC) {
            Fast &= ~INTRINSIC;
            pline("You feel slower!");
            break;
        }
    case 9:
        if (Stealth & INTRINSIC) {
            Stealth &= ~INTRINSIC;
            pline("You feel clumsy!");
            break;
        }
    case 10:
        if (Protection & INTRINSIC) {
            Protection &= ~INTRINSIC;
            pline("You feel vulnerable!");
            break;
        }
    default:
        break;
    }
}
