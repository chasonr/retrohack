/*
 * Hack.h
 */

#define NORMAL_IO
#ifdef NORMAL_IO

#include <stdio.h>
#define flush()		fflush( stdout )

#else

#undef putchar
#undef getchar
#define NULL	0
#define STDOUT  0		/* Defines print in I/O packet */

#endif NORMAL_IO

#include "envir.h"
#include "hack.name.h"

char   *index ();

long    pow ();

#define BUFSZ   256


/* Arguments for panic to give just an error message or a core dump */
#define NOCORE	0
#define CORE	1

#define ON      1
#define OFF     0

#define WALL    1
#define SDOOR   2
#define DOOR    3
#define CORR    4
#define ROOM    5
#define POOL    7
#define VAULT   9

#define TRAPNUM 9

#define MSLOW   1
#define MFAST   2

#define BEAR    0
#define ARROW   1
#define DART    2
#define TDOOR   3
#define TELE    4
#define PIT     5
#define SLPTRP  6
#define PIERC   7
#define MIMIC   8
#define SEEN    32

#define UWEP	0
#define UARM    1
#define UARM2   2
#define ULEFT   3
#define URIGHT  4

#define POTN    1
#define SCRN    2
#define WANN    4
#define RINN    8

#define UNC     1		/* unCoff(): */
#define COFF    0

#define SETC    1		/* setCon(): */
#define CON     0

#define IT1     1
#define THEIT2  2		/* kluns */

#define CHOKED  0
#define DIED    1
#define STARVED 2
#define DROWNED 3
#define QUIT    4		/* Michiel args how you died */
#define ESCAPED 5

#define NOMOVE  0
#define MOVE    1
#define DEAD    2

#define MISS    0
#define HIT     1
#define KILL    2



typedef struct func_tab {
	char    f_char;
	int     (*f_funct) ();
} FUNCTIONS;
extern  FUNCTIONS list[];



struct rm {
	unsigned        scrsym:	7;
	unsigned        typ:	6;
	unsigned        new:	1;
	unsigned        seen:	1;
	unsigned        lit:	1;
};
typedef struct rm       PART;
extern  PART levl[80][22];




struct mkroom {
	char    lx, hx, ly, hy, rtype, rlit, doorct, fdoor;
};
typedef struct mkroom   MKROOM;
extern  MKROOM rooms[15];



#define DOORMAX 100
struct coord {
	char    x, y;
};
typedef struct coord    COORDINATES;
extern  COORDINATES doors[DOORMAX];



struct food {
	char   *foodnam, prob, delay;
	int     nutrition;
};
typedef struct food    *FOOD;
extern struct food      foods[];



struct armor {
	char   *armnam, prob, delay, a_ac, a_can;
};
typedef struct armor   *ARMOR;
extern struct armor     armors[];





struct weapon {
	char   *wepnam, prob, wsdam, wldam;
};
typedef struct weapon  *WEAPON;
extern struct weapon    weapons[];




struct permonst {
	char   *mname, mlet, mhd, mmove, ac, damn, damd;
	unsigned        pxlth;
};
typedef struct permonst *MONSTDATA;
extern struct permonst  mon[8][7];
#define PM_MIMIC        &mon[5][2]
#define PM_PIERC        &mon[2][3]
#define PM_DEMON        &mon[7][6]
#define PM_CHAM         &mon[6][6]




struct obj {
	struct obj     *nobj;
	char    otyp;
	int     spe;
	unsigned        ox:	7;
	unsigned        oy:	5;
	unsigned        olet:	7;
	unsigned        quan:	5;
	unsigned        known:	1;
	unsigned        cursed: 1;
	unsigned        unpaid:	1;
};
typedef struct obj     *OBJECT;
extern  OBJECT fobj, invent, uwep, uarm, uarm2, uleft, uright;



struct stole {
	OBJECT sobj;
	unsigned        sgold;
};
typedef struct stole   *STOLE;


struct monst {
	struct monst   *nmon;
	MONSTDATA	data;
	STOLE		mstole;
	char    mx, my;
	int     mhp, orig_hp;
	unsigned        invis:	1;
	unsigned        cham:	1;
	unsigned        angry:	1;	/* Michiel: only for shopkeeper */
	unsigned        ale:	1;	/* Michiel: is it an ale?? */
	unsigned        mspeed:	2;
	unsigned        msleep: 1;
	unsigned        mfroz:	1;
	unsigned        mconf:	1;
	unsigned        mflee:	1;
	unsigned        mcan:	1;
	unsigned        mtame:	1;
	unsigned        wormno: 5;
	unsigned        mxlth;
	char    mextra[1];
};
typedef struct monst   *MONSTER;
extern  MONSTER fmon, shopkeeper, vaultkeeper;
extern struct permonst  treasurer;

MONSTER m_at ();



struct wseg {
	struct wseg    *nseg;
	char    wx, wy;
};

typedef struct wseg    *WORMSEGMENT;

#define newseg()        (alloc( sizeof(struct wseg) )->Wseg)


struct gen {
	struct gen     *ngen;
	char    	gx, gy;
	unsigned        gflag;
};
typedef struct gen     *GOLD_TRAP;
extern  GOLD_TRAP fgold, ftrap;

GOLD_TRAP g_at ();
OBJECT o_at (), getobj ();


struct flag {
	unsigned        topl:		1;
	unsigned        botl:		1;
 /* faint:1, screen:1, */
	unsigned        oneline:	1;
	unsigned        next:		1;
	unsigned        move:		1;
	unsigned        mv:		1;
	unsigned        run:		2;
	unsigned        dgold:		1;
	unsigned        dhp:		1;
	unsigned        dhpmax:		1;
	unsigned        dstr:		1;
	unsigned        dac:		1;
	unsigned        dulev:		1;
	unsigned        dexp:		1;
	unsigned        dhs:		1;
	unsigned        dscr:		1;
};
typedef struct flag     FLAG;
extern  FLAG flags;

struct you {
	char    ux, uy, ustr, ustrmax, udaminc, uac;
	int     uhunger;
	unsigned        ufast:		7;
	unsigned        uconfused:	6;
	unsigned        uinvis:		6;
	unsigned        ulevel:		5;
	unsigned        utrap:		3;
	unsigned        upit:		1;
	unsigned        uinshop:	1;
	unsigned        uinzoo:		1;
	unsigned        uinyard:	1;
	unsigned        uinswamp:	1;
	unsigned        uinknox:	1;
	unsigned        umconf:		1;
	unsigned        uhcursed:	1;
	unsigned        ufireres:	1;
	unsigned        ucoldres:	1;
	unsigned        utel:		1;
	unsigned        upres:		1;
	unsigned        ustelth:	1;
	unsigned        uagmon:		1;
	unsigned        ufeed:		1;
	unsigned        usearch:	1;
	unsigned        ucinvis:	1;
	unsigned        uregen:		1;
	unsigned        ufloat:		1;
	unsigned        uswallow:	1;
	unsigned        uswldtim:	4;
	unsigned        ucham:		1;
	unsigned        uhs:		2;
	unsigned        ublind;
	short   uhp, uhpmax;
	long    ugold, uexp, urexp;
	MONSTER ustuck;
};
typedef struct you      YOU;
extern  YOU u;



extern char    *wepnam[], *pottyp[], *scrtyp[], *traps[],
               *wantyp[], *ringtyp[], *potcol[], *scrnam[],
               *wannam[], *rinnam[], wdam[], oiden[],
               *potcall[], *scrcall[], *wandcall[], *ringcall[],
                curx, cury, savx,
                xdnstair, ydnstair, xupstair, yupstair,
                seehx, seelx, seehy, seely,
               *save_cm, *killer, dlevel, maxdlevel,
                dx, dy, buf[], lock[],
                genocided[60], oldux, olduy, wizard;

extern unsigned moves;

extern  multi;

#define newmonst(xl)    (alloc( xl + sizeof(struct monst) )->Mtmp )
#define newobj()        (alloc( sizeof(struct obj) )->Otmp )
#define newgen()        (alloc( sizeof(struct gen) )->Gtmp )
#define newstole()      (alloc( sizeof(struct stole) )->Stmp )


#define CHAR_NULL	(char *)NULL
#define OBJ_NULL	(struct obj *)NULL
#define TRAP_NULL	(struct gen *)NULL
#define MON_NULL	(struct monst *)NULL
#define STOLE_NULL	(struct stole *)NULL

#ifndef SHOW
union PTRS {
	GOLD_TRAP Gtmp;
	MONSTER Mtmp;
	OBJECT Otmp;
	STOLE Stmp;
	WORMSEGMENT Wseg;
	char   *Val;
};

extern  union PTRS * alloc ();
#endif SHOW
