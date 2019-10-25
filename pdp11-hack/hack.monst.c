/*
 * Hack.monst.c
 */

#include "hack.h"
#include "hack.dog.h"

struct permonst mon[8][7]={
	{
		{ "bat",               'B',1,22,8,1,4,0 },
		{ "gnome",             'G',1,6,5,1,6,0 },
		{ "hobgoblin",         'H',1,9,5,1,8,0 },
		{ "jackal",            'J',0,12,7,1,2,0 },
		{ "kobold",            'K',1,6,7,1,4,0 },
		{ "leprechaun",        'L',1,15,8,1,5,0 },
		{ "giant rat",         'r',0,12,7,1,3,0 },
	},
	{
		{ "acid blob",         'a',2,3,8,0,0,0 },
		{ "floating eye",      'E',2,1,9,0,0,0 },
		{ "homunculus",        'h',2,6,6,1,3,0 },
		{ "imp",               'i',2,6,2,1,4,0 },
		{ "orc",               'O',2,9,6,1,8,0 },
		{ "yellow light",      'y',3,15,0,0,0,0 },
		{ "zombie",            'Z',2,6,8,1,8,0 },
	},
	{
		{ "giant ant",         'A',3,18,3,1,6,0 },
		{ "fog cloud",         'f',3,1,0,1,6,0 },
		{ "nymph",             'N',3,12,9,1,4,0 },
		{ "piercer",           'p',3,1,3,2,6,0 },
		{ "quasit",            'Q',3,15,3,1,4,0 },
		{ "quivering blob",    'q',3,1,8,1,8,0 },
		{ "violet fungi",      'v',3,1,7,1,4,0 },
	},
	{
		{ "giant beetle",      'b',4,6,4,3,4,0 },
		{ "centaur",           'C',4,18,4,1,6,0 },
		{ "cockatrice",        'c',4,6,6,1,3,0 },
		{ "gelatinous cube",   'g',4,6,8,2,4,0 },
		{ "jaguar",            'j',4,15,6,1,8,0 },
		{ "killer bee",        'k',4,6,4,2,4,0 },
		{ "snake",             'S',4,15,3,1,6,0 },
	},
	{
		{ "freezing sphere",   'F',2,13,4,0,0,0 },
		{ "owlbear",           'o',5,12,5,2,6,0 },
		{ "rust monster",      'R',5,18,3,0,0,0 },
		{ "giant scorpion",    's',5,15,3,1,4,0 },
		{ "teleporter",        't',5,3,5,1,7,0 },
		{ "wraith",            'W',5,12,5,1,6,0 },
		{ "long worm",         'w',6,3,5,2,5,0 },
	},
	{
		{ "large dog",         'd',6,15,4,2,4,0 },
		{ "leocrotta",         'l',6,18,4,3,6,0 },
		{ "mimic",             'M',7,3,7,3,4,0 },
		{ "minotaur",          'm',6,12,6,2,8,0 },
		{ "troll",             'T',7,12,4,2,6,0 },
		{ "ugod",              'u',6,11,5,1,10,0 },
		{ "yeti",              'Y',5,15,6,1,6,0 },
	},
	{
		{ "invisible stalker", 'I',8,12,3,4,4,0 },
		{ "umber hulk",        'U',9,6,2,2,14,0 },
		{ "vampire",           'V',8,12,1,1,6,0 },
		{ "xorn",              'X',8,9,-2,4,6,0 },
		{ "xerp",              'x',7,6,3,2,4,0 },
		{ "zelomp",            'z',9,8,3,3,6,0 },
		{ "chameleon",         ':',6,5,6,4,2,0 },
	},
	{
		{ "dragon",            'D',20,9,-1,3,8,0 },
		{ "ettin",             'e',10,12,3,2,8,0 },
		{ "lurker above",      '\'',10,3,3,0,0,0 },
		{ "neo-otyugh",        'n',11,6,0,1,3,0 },
		{ "trapper",           ',',12,3,3,0,0,0 },
		{ "purple worm",       'P',20,9,-1,2,8,0 },
		{ "demon",             '&',14,9,-4,1,6,0 }
	}
};
struct permonst /* _mon={0,0,0,0,0,0,0,0},*/
	li_dog={"little dog",	'd',2,18,6,1,6,sizeof(struct edog)},
		dog={"dog",	'd',4,16,5,1,6,sizeof(struct edog)},
	la_dog={"large dog",	'd',6,15,4,2,4,sizeof(struct edog)};
