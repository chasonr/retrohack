/*	SCCS Id: @(#)objclass.h	2.3	88/01/24 */
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */

#ifndef OBJCLASS_H
#define OBJCLASS_H

/* definition of a class of objects */

struct objclass {
    const char *oc_name;  /* actual name */
    const char *oc_descr; /* description when name unknown */
    char *oc_uname; /* called by user */
    Bitfield(oc_name_known, 1);
    Bitfield(oc_merge, 1); /* merge otherwise equal objects */
    char oc_olet;
    schar oc_prob;  /* probability for mkobj() */
    schar oc_delay; /* delay when using such an object */
    uchar oc_weight;
    schar oc_oc1, oc_oc2;
    int oc_oi;
#define nutrition oc_oi /* for foods */
#define a_ac oc_oc1     /* for armors - only used in ARM_BONUS */
#define ARM_BONUS(obj) ((10 - objects[obj->otyp].a_ac) + obj->spe)
#define a_can oc_oc2 /* for armors */
#define bits oc_oc1  /* for wands and rings */
    /* wands */
#define NODIR 1
#define IMMEDIATE 2
#define RAY 4
    /* rings */
#define SPEC 1 /* +n is meaningful */
               /* Check the AD&D rules!  The FIRST is small monster damage. */
#define wsdam oc_oc1 /* for weapons and PICK_AXE */
#define wldam oc_oc2 /* for weapons and PICK_AXE */

#define g_val oc_oi /* for gems: value on exit */

#ifdef SPELLS
#define spl_lev oc_oi /* for books: spell level */
#endif
};

extern struct objclass objects[];

/* definitions of all object-symbols */

#define RANDOM_SYM '\0' /* used for generating random objects */
#define ILLOBJ_SYM '\\'
#define AMULET_SYM '"'
#define FOOD_SYM '%'
#define WEAPON_SYM ')'
#define TOOL_SYM '('
#define BALL_SYM '0'
#define CHAIN_SYM '_'
#define ROCK_SYM '`'
#define ARMOR_SYM '['
#define POTION_SYM '!'
#define SCROLL_SYM '?'
#define WAND_SYM '/'
#define RING_SYM '='
#define GEM_SYM '*'
#define GOLD_SYM '$'
#ifdef SPELLS
#define SPBOOK_SYM '+' /* actually SPELL-book */
#endif
/* Other places with explicit knowledge of object symbols:
 * mklev.c:	"=/)%?![<>+"	(used for calculating Amulet apperances)
 * mkobj.c:	char mkobjstr[] = "))[[!!!!????%%%%/=**+";
 * apply.c:   otmp = getobj("0#%", "put in");
 * eat.c:     otmp = getobj("%", "eat");
 * invent.c:          if(index("!%?[)=*(0/+\"", sym)){
 * invent.c:    || index("%?!*+",otmp->olet))){
 */

#endif
