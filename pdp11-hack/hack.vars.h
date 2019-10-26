/*
 * Hack.vars.h
 */

#define	SIZE(x)	(sizeof x)/(sizeof x[0])

const char NOTHIN[]   = "Nothing Happens",
           WCLEV[]    = "Welcome to level %d.",
           STOPGLOW[] = "Your hands stop glowing blue.",
           UMISS[]    = "You miss %s.";


const char * const traps[] = {
	" bear trap",
	"n arrow trap",
	" dart trap",
	" trapdoor",
	" teleportation trap",
	" pit",
	" sleeping gas trap",
	" piercer",
	" mimic"
};

#include "hack.foods.h"
#include "hack.armors.h"
#include "hack.weapons.h"

const char * const pottyp[] = {
	"restore strength",
	"booze",
	"invisibility",
	"fruit juice",
	"healing",
	"paralysis",
	"monster detection",
	"object detection",
	"sickness",
	"confusion",
	"gain strength",
	"speed",
	"blindness",
	"gain level",
	"extra healing"
};



const char *potcol[] = {
	"ebony",
	"magenta",
	"clear",
	"brilliant blue",
	"sky blue",
	"emerald",
	"dark green",
	"ruby",
	"purple-red",
	"swirly",
	"white",
	"yellow",
	"purple",
	"puce",
	"pink",
	"smokey",
	"glowing",
	"bubbly",
	"orange"
};

char   *potcall[SIZE (pottyp)];

 /* SCROLLS */

const char * const scrtyp[] = {
	"enchant armor",
	"losing",
	"curse levels",
	"remove curse",
	"enchant weapon",
	"create monster",
	"damage weapon",
	"genocide",
	"destroy armor",
	"light",
	"transportation",
	"gold detection",
	"identify",
	"magic mapping",
	"fire"
};


const char *scrnam[] = {
	"VELOX NEB",
	"FOOBIE BLETCH",
	"TEMOV",
	"GARVEN DEH",
	"ZELGO MER",
	"ANDOVA BEGARIN",
	"ELAM EBOW",
	"KERNOD WEL",
	"THARR",
	"VENZAR BORGAVVE",
 /* "ELBIB YLOH", */
	"VERR YED HORRE",
	"JUYED AWK YACC",
	"HACKEM MUCHE",
	"LEP GEX VEN ZEA",
	"DAIYEN FOOELS"
};
char   *scrcall[SIZE (scrtyp)];


 /* WANDS */

const char * const wantyp[] = {
	"light",
	"secret door + trap detection",
	"create monster",
	"exhaustion",
	"slow monster",
	"speed monster",
	"undead turning",
	"polymorph",
	"cancellation",
	"teleport monster",
	"digging",
	"magic missile",
	"fire",
	"sleep",
	"cold",
	"death",
	"confusion",
	"door closing"
};


const char *wannam[] = {
	"oak",
	"ebony",
	"runed",
	"long",
	"short",
	"curved",
	"steel",
	"aluminium",
	"iron",
	"marble",
	"pine",
	"maple",
	"brass",
	"silver",
	"copper",
	"balsa",
	"zinc",
	"platinum"
};
char   *wandcall[SIZE (wantyp)];


 /* RINGS */

const char * const ringtyp[] = {
	"adornment",
	"teleportation",
	"regeneration",
	"searching",
	"see invisible",
	"stealth",
	"floating",
	"poison resistance",
	"aggravate monster",
	"hunger",
	"fire resistance",
	"cold resistance",
	"protection from shape-changers",
	"gain strength",
	"increase damage",
	"protection"
};


const char *rinnam[] = {
	"blackened",
 /* "ivory", */
	"granite",
	"silver",
	"ruby",
	"jade",
	"diamond",
	"copper",
	"gold",
	"shining",
	"tiger eye",
	"agate",
	"moonstone",
	"sapphire",
	"pearl",
	"topaz",
 /* "black onix", */
	"wired"
};

char   *ringcall[SIZE (ringtyp)], oiden[40];
signed char oldux, olduy;
