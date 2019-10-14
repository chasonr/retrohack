/*	SCCS Id: @(#)you.h	2.3	87/12/12
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */

#ifndef YOU_H
#define YOU_H

#include "config.h"
#include "onames.h"
#include "permonst.h"

struct prop {
#define TIMEOUT 007777     /* mask */
#define LEFT_RING W_RINGL  /* 010000L */
#define RIGHT_RING W_RINGR /* 020000L */
#define INTRINSIC 040000L
#define LEFT_SIDE LEFT_RING
#define RIGHT_SIDE RIGHT_RING
#define BOTH_SIDES (LEFT_SIDE | RIGHT_SIDE)

    long p_flgs;
    int (*p_tofn)(); /* called after timeout */
};

struct you {
    xchar ux, uy;
    schar dx, dy, dz;   /* direction of move (or zap or ... ) */
    schar di;           /* direction of FF */
    xchar ux0, uy0;     /* initial position FF */
    xchar udisx, udisy; /* last display pos */
    char usym;          /* usually '@' */
    schar uluck;
#define LUCKMAX 10 /* on moonlit nights 11 */
#define LUCKMIN (-10)
    int last_str_turn; /* 0: none, 1: half turn, 2: full turn */
                       /* +: turn right, -: turn left */
    unsigned udispl;   /* @ on display */
    unsigned ulevel;   /* 1 - 14 */
#ifdef QUEST
    unsigned uhorizon;
#endif
    unsigned utrap;     /* trap timeout */
    unsigned utraptype; /* defined if utrap nonzero */
#define TT_BEARTRAP 0
#define TT_PIT 1
#ifdef SPIDERS
#define TT_WEB 2
#endif
    unsigned uinshop; /* used only in shk.c - (roomno+1) of shop */

/* perhaps these #define's should also be generated by makedefs */
#define TELEPAT LAST_RING /* not a ring */
#define HTelepat u.uprops[TELEPAT].p_flgs
#define Telepat ((HTelepat) || (u.usym == 'E'))
#define FAST (LAST_RING + 1) /* not a ring */
#define Fast u.uprops[FAST].p_flgs
#define CONFUSION (LAST_RING + 2) /* not a ring */
#define HConfusion u.uprops[CONFUSION].p_flgs
#define Confusion ((HConfusion) || index("BIy", u.usym))
#define INVIS (LAST_RING + 3) /* not a ring */
#define HInvis u.uprops[INVIS].p_flgs
#define Invis ((HInvis) || u.usym == 'I')
#define Invisible (Invis && !See_invisible)
#define GLIB (LAST_RING + 4) /* not a ring */
#define Glib u.uprops[GLIB].p_flgs
#define PUNISHED (LAST_RING + 5) /* not a ring */
#define Punished u.uprops[PUNISHED].p_flgs
#define SICK (LAST_RING + 6) /* not a ring */
#define Sick u.uprops[SICK].p_flgs
#define BLINDED (LAST_RING + 7) /* not a ring */
#define Blinded u.uprops[BLINDED].p_flgs
#define WOUNDED_LEGS (LAST_RING + 8) /* not a ring */
#define Wounded_legs u.uprops[WOUNDED_LEGS].p_flgs
#define STONED (LAST_RING + 9) /* not a ring */
#define Stoned u.uprops[STONED].p_flgs
#define HALLUCINATION (LAST_RING + 10) /* not a ring */
#define Hallucination u.uprops[HALLUCINATION].p_flgs
#define BLINDFOLDED (LAST_RING + 11) /* not a ring */
#define Blindfolded u.uprops[BLINDFOLDED].p_flgs
#define Blind (Blinded || Blindfolded)
#define BADGED (LAST_RING + 12) /* not a ring */
#define Badged u.uprops[BADGED].p_flgs
#define LAST_PROP (BADGED)          /* the last property */
#define PROP(x) (x - RIN_ADORNMENT) /* convert ring to index in uprops */
    struct prop uprops[LAST_PROP + 1];

    unsigned umconf;
    char *usick_cause;
    int mh, mhmax, mtimedone, umonnum; /* for polymorph-self */
    schar mstr, mstrmax;               /* for saving ustr/ustrmax */
#if defined(KOPS) && defined(KAA)
    unsigned ucreamed;
#endif
    unsigned uswallow; /* set if swallowed by a monster */
    unsigned uswldtim; /* time you have been swallowed */
    unsigned uhs;      /* hunger state - see hack.eat.c */
#ifdef HARD
    unsigned udemigod; /* once you kill the wiz */
    unsigned udg_cnt;  /* how long you have been demigod */
#endif
#ifdef RPH
    int medusa_level; /* level of wiz and medusa */
    int wiz_level;
#endif
#ifdef STOOGES
    int stooge_level;
#endif
    schar ustr, ustrmax;
    schar udaminc;
    schar uac;
    int uhp, uhpmax;
#ifdef SPELLS
    int uen, uenmax; /* magical energy - M. Stephenson */
#endif
#ifdef PRAYERS
    int ugangr;              /* if the gods are angry at you */
    int ublessed, ublesscnt; /* blessing/duration from #pray */
#endif
    long int ugold, ugold0, uexp, urexp;
    int uhunger; /* refd only in eat.c and shk.c */
    int uinvault;
    struct monst *ustuck;
    int nr_killed[CMNUM + 2]; /* used for experience bookkeeping */
};

#endif /* YOU_H /**/
