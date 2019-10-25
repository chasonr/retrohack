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
	void    (*f_funct) ();
} FUNCTIONS;



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

#ifdef MKLEV

/*### mklev.make.c ###*/
extern void delmon();
extern void makemaz();
extern void makemon();
extern void mkgold();
extern void mk_knox();
extern void mkshop();
extern void mkswamp();
extern void mktrap();
extern void mkyard();
extern void mkzoo();
extern int panic();

/*### mklev.mkobj.c ###*/
extern void mkobj();
//0000000000000560 D armors
//0000000000000600 D foods
//0000000000000050 D NOTHIN
//0000000000000028 C oiden
//0000000000000001 C oldux
//0000000000000001 C olduy
//0000000000000078 C potcall
//0000000000000340 D potcol
//00000000000003e0 D pottyp
//0000000000000080 C ringcall
//0000000000000080 D ringtyp
//0000000000000000 D rinnam
//0000000000000078 C scrcall
//0000000000000240 D scrnam
//00000000000002c0 D scrtyp
//0000000000000010 D STOPGLOW
//00000000000006e0 D traps
//0000000000000000 D UMISS
//0000000000000090 C wandcall
//0000000000000100 D wannam
//00000000000001a0 D wantyp
//0000000000000030 D WCLEV
//0000000000000460 D weapons

/*### mklev.c ###*/
extern void bwrite();
extern GOLD_TRAP g_at ();
extern int main();
extern MONSTER m_at();
extern void move();
extern OBJECT o_at();
extern int okay();
extern void savelev();
//0000000000000008 C croom
//0000000000000001 C dlevel
//00000000000000c8 C doors
//0000000000000001 C dx
//0000000000000001 C dy
//0000000000000008 C fgold
//0000000000000008 C fmon
//0000000000000008 C fobj
//0000000000000008 C ftrap
//0000000000000008 C geno
//0000000000000001 C goldseen
//0000000000001b80 C levl
//0000000000000000 D mmon
//0000000000000004 C nroom
//0000000000000014 C nul
//0000000000000078 C rooms
//0000000000000001 C tx
//0000000000000001 C ty
//0000000000000001 C wizard
//0000000000000001 C xdnstair
//0000000000000001 C xupstair
//0000000000000001 C ydnstair
//0000000000000001 C yupstair

#else /* MKLEV */
/*### hack.bones.c ###*/
extern int getbones();
extern void savebones();
//0000000000000000 D pm_ghost

/*### hack.debug.c ###*/

/*### hack.do1.c ###*/
extern MONSTER bhit();
extern void buzz();
extern void dosearch();
extern void doset();
extern void doshow();
extern void dowhatis();
extern void dozap();
extern void hit();
extern void miss();
//0000000000000028 C vaultflag
//0000000000000008 C vaultkeeper

/*### hack.dog.c ###*/
extern int dog_move();
extern int hitmm();
extern int inroom();
extern void keepdogs();
extern void losedogs();
extern void makedog();
extern int tamedog();
//0000000000000000 B mydogs

/*### hack.do.misc.c ###*/
extern void docall();
extern void docurse();
extern void dodown();
extern void dosavelev();
extern void doup();
extern int getdir();
extern void hackexec();
extern void rhack();
//0000000000000028 C upxstairs
//0000000000000028 C upystairs

/*### hack.do.c ###*/
extern void dodrink();
extern void dodrop();
extern void doread();
extern void gemsdrop();
extern void litroom();
extern int uwepcursed();
//0000000000000000 D WELDED

/*### hack.do_wear.c ###*/
extern void armwear();
extern void doremarm();
extern void doremring();
extern void doring();
extern void dowearring();

/*### hack.eat.c ###*/
extern void doeat();
extern void gethungry();
extern void lesshungry();
//0000000000000000 D hu_stat
//0000000000000000 B starved

/*### hack.end.c ###*/
extern void clearlocks();
extern void done();
extern void done1();
extern int hangup();
extern char *itoa();
//0000000000000000 B maxdlevel

/*### hack.invent.c ###*/
extern OBJECT addinv();
extern void ddoinv();
extern void delobj();
extern void deltrap();
extern void doinv();
extern void freeobj();
extern GOLD_TRAP g_at ();
extern OBJECT getobj();
extern MONSTER m_at();
extern OBJECT o_at();
extern void ofree();
extern void prinv();
extern void useup();

/*### hack.invinit.c ###*/
//0000000000000050 D mace0
//0000000000000030 D uarm0
//0000000000000000 D yourinvent0

/*### hack.io.c ###*/
extern void getlin();
extern void getret();
extern void hackmode();
extern void more();

/*### hack.lev.c ###*/
extern void bwrite();
extern int getlev();
extern void mklev();
extern void mread();
extern void savelev();

/*### hack.main.c ###*/
extern void glo();
extern void impossible();
//0000000000000100 C buf
//0000000000000001 C curx
//0000000000000001 C cury
//0000000000000004 B dlevel
//00000000000000c8 C doors
//0000000000000001 C dx
//0000000000000001 C dy
//0000000000000038 B fgold
//0000000000000004 C flags
//0000000000000040 B fmon
//0000000000000028 B fobj
//0000000000000030 B ftrap
//000000000000003c C genocided
//0000000000000008 C invent
//0000000000000008 C killer
//0000000000001b80 C levl
//0000000000000010 C lock
//0000000000000000 D moves
//0000000000000000 B multi
//0000000000000008 C nomvmsg
//0000000000000014 C nul
//000000000000000a C plname
//0000000000000004 C rfile
//0000000000000078 C rooms
//0000000000000008 B save_cm
//0000000000000020 D SAVEFILE
//0000000000000001 C savx
//0000000000000040 C u
//0000000000000008 C uarm
//0000000000000020 B uarm2
//0000000000000010 B uleft
//0000000000000018 B uright
//0000000000000008 C uwep
//0000000000000001 C wizard
//0000000000000001 C xdnstair
//0000000000000001 C xupstair
//0000000000000001 C ydnstair
//0000000000000001 C yupstair

/*### hack.mkobj.c ###*/
extern void callsrestore();
extern void callssave();
extern void mkobj();
extern void restnames();
extern void savenames();
extern void shuffle();
//0000000000000560 D armors
//0000000000000600 D foods
//0000000000000050 D NOTHIN
//0000000000000028 C oiden
//0000000000000001 C oldux
//0000000000000001 C olduy
//0000000000000078 C potcall
//0000000000000340 D potcol
//00000000000003e0 D pottyp
//0000000000000080 C ringcall
//0000000000000080 D ringtyp
//0000000000000000 D rinnam
//0000000000000078 C scrcall
//0000000000000240 D scrnam
//00000000000002c0 D scrtyp
//0000000000000010 D STOPGLOW
//00000000000006e0 D traps
//0000000000000000 D UMISS
//0000000000000090 C wandcall
//0000000000000100 D wannam
//00000000000001a0 D wantyp
//0000000000000030 D WCLEV
//0000000000000460 D weapons

/*### hack.mon.do.c ###*/
extern void cmdel();
extern void movemon();

/*### hack.mon.c ###*/
extern void delmon();
extern int dist();
extern void killed();
extern int makemon();
extern void mkmonat();
extern void mnexto();
extern void newcham();
extern void p2xthe();
extern void poisoned();
extern int psee();
extern void pseebl();
extern void relmon();
extern void relobj();
extern void rescham();
extern int r_free();
extern void rloc();
extern int somegold();
extern void steal();
extern void stlobj();
extern void unstuck();

/*### hack.monst.c ###*/
//0000000000000020 D dog
//0000000000000000 D la_dog
//0000000000000040 D li_dog
//0000000000000060 D mon

/*### hack.move.c ###*/
extern void domove();
extern void lookaround();
extern int movecm();
extern void nomove();
extern void nomul();
extern char *parse();
//0000000000000001 C seehx
//0000000000000001 C seehy
//0000000000000001 C seelx
//0000000000000001 C seely

/*### hack.c ###*/
extern int abon();
extern int alive();
extern int cansee();
extern void doname();
extern int hitu();
extern int hmon();
extern void land();
extern void losehp();
extern void losestr();
extern char *lowc();
extern long pow2();
extern char *setan();
extern void setCon();
extern void tele();
extern void unCoff();
extern int weight();

/*### hack.office.c ###*/
extern int kantoor();

/*### hack.pri.c ###*/
extern void at();
extern void atl();
extern void bot();
extern void cl_end();
extern void cls();
extern void curs();
extern void docrt();
extern void doreprint();
extern void home();
extern void levlsym();
extern char news0();
extern void newsym();
extern void newunseen();
extern void nose1();
extern void nscr();
extern void on();
extern int panic();
extern void pline();
extern void pmon();
extern void prl();
extern void prl1();
extern void prme();
extern void pru();
extern void prustr();
extern void seeatl();
extern void startup();

/*### hack.rip.c ###*/
extern void outrip();

/*### hack.save.c ###*/
extern int dorecover();
extern void save();

/*### hack.shk.c ###*/
extern void addtobill();
extern void doinvbill();
extern void dopay();
extern int inshop();
extern int inshproom();
extern int onbill();
extern void paybill();
extern void setangry();
extern void shkdead();
extern int shk_move();
extern void subfrombill();
//0000000000000000 B billct
//0000000000000008 B robbed
//0000000000000002 C shd
//0000000000000002 C shk
//0000000000000001 C shlevel
//0000000000000008 C shopkeeper

/*### hack.str.c ###*/

/*### hack.worm.c ###*/
extern void cutworm();
extern int getwn();
extern void initworm();
extern void wormdead();
extern void wormhit();
extern void worm_move();
extern void worm_nomove();
extern void wormsee();
//0000000000000080 C wgrowtime
//0000000000000100 C wheads
//0000000000000100 C wsegs
#endif /* MKLEV */

/*### rnd.c ###*/
extern int d();
extern int rn1();
extern int rn2();
extern int rnd();

#endif SHOW
