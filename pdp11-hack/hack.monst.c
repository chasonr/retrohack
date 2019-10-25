/*
 * Hack.monst.c
 */

#include "hack.h"
#include "hack.dog.h"

/*
 * Unfortunately the syntax for structure initialization is
 * different on VAX and 11/45. Define VAX to get the VAX syntax
 */

#define VAX

#ifdef VAX

#define	SOSEP	{
#define	SCSEP	}
#define GOSEP
#define	GMSEP
#define	GCSEP

#else

#define	SOSEP
#define	SCSEP
#define	GOSEP	{
#define	GMSEP	},{
#define	GCSEP	}

#endif VAX

struct permonst mon[8][7]={
	GOSEP
	SOSEP "bat",		'B',1,22,8,1,4,0 SCSEP,
	SOSEP "gnome",		'G',1,6,5,1,6,0 SCSEP,
	SOSEP "hobgoblin",	'H',1,9,5,1,8,0 SCSEP,
	SOSEP "jackal",		'J',0,12,7,1,2,0 SCSEP,
	SOSEP "kobold",		'K',1,6,7,1,4,0 SCSEP,
	SOSEP "leprechaun",	'L',1,15,8,1,5,0 SCSEP,
	SOSEP "giant rat",	'r',0,12,7,1,3,0 SCSEP,
	GMSEP
	SOSEP "acid blob",	'a',2,3,8,0,0,0 SCSEP,
	SOSEP "floating eye",	'E',2,1,9,0,0,0 SCSEP,
	SOSEP "homunculus",	'h',2,6,6,1,3,0 SCSEP,
	SOSEP "imp",		'i',2,6,2,1,4,0 SCSEP,
	SOSEP "orc",		'O',2,9,6,1,8,0 SCSEP,
	SOSEP "yellow light",	'y',3,15,0,0,0,0 SCSEP,
	SOSEP "zombie",		'Z',2,6,8,1,8,0 SCSEP,
	GMSEP
	SOSEP "giant ant",	'A',3,18,3,1,6,0 SCSEP,
	SOSEP "fog cloud",	'f',3,1,0,1,6,0 SCSEP,
	SOSEP "nymph",		'N',3,12,9,1,4,0 SCSEP,
	SOSEP "piercer",	'p',3,1,3,2,6,0 SCSEP,
	SOSEP "quasit",		'Q',3,15,3,1,4,0 SCSEP,
	SOSEP "quivering blob",	'q',3,1,8,1,8,0 SCSEP,
	SOSEP "violet fungi",	'v',3,1,7,1,4,0 SCSEP,
	GMSEP
	SOSEP "giant beetle",	'b',4,6,4,3,4,0 SCSEP,
	SOSEP "centaur",	'C',4,18,4,1,6,0 SCSEP,
	SOSEP "cockatrice",	'c',4,6,6,1,3,0 SCSEP,
	SOSEP "gelatinous cube",'g',4,6,8,2,4,0 SCSEP,
	SOSEP "jaguar",		'j',4,15,6,1,8,0 SCSEP,
	SOSEP "killer bee",	'k',4,6,4,2,4,0 SCSEP,
	SOSEP "snake",		'S',4,15,3,1,6,0 SCSEP,
	GMSEP
	SOSEP "freezing sphere",'F',2,13,4,0,0,0 SCSEP,
	SOSEP "owlbear",	'o',5,12,5,2,6,0 SCSEP,
	SOSEP "rust monster",	'R',5,18,3,0,0,0 SCSEP,
	SOSEP "giant scorpion",	's',5,15,3,1,4,0 SCSEP,
	SOSEP "teleporter",	't',5,3,5,1,7,0 SCSEP,
	SOSEP "wraith",		'W',5,12,5,1,6,0 SCSEP,
	SOSEP "long worm",	'w',6,3,5,2,5,0 SCSEP,
	GMSEP
	SOSEP "large dog",	'd',6,15,4,2,4,0 SCSEP,
	SOSEP "leocrotta",	'l',6,18,4,3,6,0 SCSEP,
	SOSEP "mimic",		'M',7,3,7,3,4,0 SCSEP,
	SOSEP "minotaur",	'm',6,12,6,2,8,0 SCSEP,
	SOSEP "troll",		'T',7,12,4,2,6,0 SCSEP,
	SOSEP "ugod",		'u',6,11,5,1,10,0 SCSEP,
	SOSEP "yeti",		'Y',5,15,6,1,6,0 SCSEP,
	GMSEP
	SOSEP "invisible stalker",'I',8,12,3,4,4,0 SCSEP,
	SOSEP "umber hulk",	'U',9,6,2,2,14,0 SCSEP,
	SOSEP "vampire",	'V',8,12,1,1,6,0 SCSEP,
	SOSEP "xorn",		'X',8,9,-2,4,6,0 SCSEP,
	SOSEP "xerp",		'x',7,6,3,2,4,0 SCSEP,
	SOSEP "zelomp",		'z',9,8,3,3,6,0 SCSEP,
	SOSEP "chameleon",	':',6,5,6,4,2,0 SCSEP,
	GMSEP
	SOSEP "dragon",		'D',20,9,-1,3,8,0 SCSEP,
	SOSEP "ettin",		'e',10,12,3,2,8,0 SCSEP,
	SOSEP "lurker above",	'\'',10,3,3,0,0,0 SCSEP,
	SOSEP "neo-otyugh",	'n',11,6,0,1,3,0 SCSEP,
	SOSEP "trapper",	',',12,3,3,0,0,0 SCSEP,
	SOSEP "purple worm",	'P',20,9,-1,2,8,0 SCSEP,
	SOSEP "demon",		'&',14,9,-4,1,6,0 SCSEP
	GCSEP
};
struct permonst /* _mon={0,0,0,0,0,0,0,0},*/
	li_dog={"little dog",	'd',2,18,6,1,6,sizeof(struct edog)},
		dog={"dog",	'd',4,16,5,1,6,sizeof(struct edog)},
	la_dog={"large dog",	'd',6,15,4,2,4,sizeof(struct edog)};
