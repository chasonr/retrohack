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

#endif /* NORMAL_IO */

#include "envir.h"
#include "hack.name.h"

/* use GCC attributes if we have them */
#ifndef __GNUC__
# define __attribute__(x)
#endif

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
	void    (*f_funct) (void);
} FUNCTIONS;



struct rm {
	unsigned        scrsym:	7;
	unsigned        typ:	6;
	unsigned        new:	1;
	unsigned        seen:	1;
	unsigned        lit:	1;
};
typedef struct rm       PART;




struct mkroom {
	signed char lx, hx, ly, hy, rtype, rlit, doorct, fdoor;
};
typedef struct mkroom   MKROOM;



#define DOORMAX 100
struct coord {
	signed char x, y;
};
typedef struct coord    COORDINATES;



struct food {
	char   *foodnam;
	signed char prob, delay;
	int     nutrition;
};
typedef struct food    *FOOD;



struct armor {
	char   *armnam;
	signed char prob, delay, a_ac, a_can;
};
typedef struct armor   *ARMOR;





struct weapon {
	char   *wepnam;
	signed char prob, wsdam, wldam;
};
typedef struct weapon  *WEAPON;




struct permonst {
	char   *mname;
	signed char mlet, mhd, mmove, ac, damn, damd;
	unsigned        pxlth;
};
typedef struct permonst *MONSTDATA;


struct obj {
	struct obj     *nobj;
	signed char otyp;
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



struct stole {
	OBJECT sobj;
	unsigned        sgold;
};
typedef struct stole   *STOLE;


struct monst {
	struct monst   *nmon;
	MONSTDATA	data;
	STOLE		mstole;
	signed char mx, my;
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


struct wseg {
	struct wseg    *nseg;
	signed char wx, wy;
};

typedef struct wseg    *WORMSEGMENT;

#define newseg()        (alloc( sizeof(struct wseg) )->Wseg)


struct gen {
	struct gen     *ngen;
	signed char    	gx, gy;
	unsigned        gflag;
};
typedef struct gen     *GOLD_TRAP;


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

struct you {
	signed char ux, uy, ustr, ustrmax, udaminc, uac;
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


#define newmonst(xl)    (alloc( xl + sizeof(struct monst) )->Mtmp )
#define newobj()        (alloc( sizeof(struct obj) )->Otmp )
#define newgen()        (alloc( sizeof(struct gen) )->Gtmp )
#define newstole()      (alloc( sizeof(struct stole) )->Stmp )


#define CHAR_NULL	(char *)NULL
#define OBJ_NULL	(struct obj *)NULL
#define TRAP_NULL	(struct gen *)NULL
#define MON_NULL	(struct monst *)NULL
#define STOLE_NULL	(struct stole *)NULL

#define MAXLEVEL	40

#ifndef SHOW
union PTRS {
	GOLD_TRAP Gtmp;
	MONSTER Mtmp;
	OBJECT Otmp;
	STOLE Stmp;
	WORMSEGMENT Wseg;
	char   *Val;
};

extern  union PTRS * alloc (int num);

#ifdef MKLEV

/*### mklev.make.c ###*/
extern void makemaz (void);
extern void makemon (int sl, int monx, int mony);
extern void mkgold (int num, int goldx, int goldy);
extern void mk_knox (void);
extern void mkshop (void);
extern void mkswamp (void);
extern void mktrap (int num, int mazeflag);
extern void mkyard (void);
extern void mkzoo (void);
__attribute__((format(printf, 1, 2)))
extern int panic (const char *str, ...);

/*### mklev.mkobj.c ###*/
extern void mkobj (int let);

/*### mklev.c ###*/
extern GOLD_TRAP g_at (int gtx, int gty, GOLD_TRAP ptr);
extern MONSTER m_at (int monx, int mony);
extern void move (int *xdir, int *ydir, int dir);
extern OBJECT o_at (int objx, int objy);
extern int okay (int xx, int yy, int dir);
extern char mmon[8][8];
extern PART levl[80][22];
extern MONSTER fmon;
extern OBJECT fobj;
extern GOLD_TRAP fgold, ftrap;
extern MKROOM rooms[15], *croom;
extern COORDINATES doors[DOORMAX];
extern int nroom;
extern char *geno;
extern signed char dlevel, goldseen,
                   xdnstair, ydnstair, xupstair, yupstair,
                   tx, ty;

#else /* MKLEV */
/*### hack.bones.c ###*/
extern int getbones (void);
extern void savebones (void);
extern struct permonst pm_ghost;

/*### hack.debug.c ###*/
#ifdef DEBUG
void debug (void);
#endif

/*### hack.do1.c ###*/
extern MONSTER bhit (int ddx, int ddy, int range);
extern void buzz (int type, int sx, int sy, int ddx, int ddy);
extern void dosearch (void);
extern void doset (void);
extern void doshow (void);
extern void dowhatis (void);
extern void dozap (void);
extern void hit (char *str, MONSTER mtmp);
extern void miss (char *str, MONSTER mtmp);
extern MONSTER vaultkeeper;
extern char vaultflag[MAXLEVEL];

/*### hack.dog.c ###*/
extern int dog_move (MONSTER mtmp, int after);
extern int hitmm (MONSTER magr, MONSTER mdef);
extern int inroom (signed char x, signed char y);
extern void keepdogs (int checkdist);
extern void losedogs (void);
extern void makedog (void);
extern int tamedog (MONSTER mtmp, OBJECT obj);
extern MONSTER mydogs;

/*### hack.do.misc.c ###*/
extern void docall (OBJECT obj);
extern void docurse (void);
extern void dodown (void);
extern void dosavelev (void);
extern void doup (void);
extern int getdir (void);
extern void hackexec (int num, const char *file, const char *arg1,
                      const char *arg2, const char *arg3, const char *arg4,
                      const char *arg5, const char *arg6);
extern void rhack (char *cmd);
extern char upxstairs[MAXLEVEL], upystairs[MAXLEVEL];

/*### hack.do.c ###*/
extern void dodrink (void);
extern void dodrop (void);
extern void doread (void);
extern void gemsdrop (void);
extern void litroom (void);
extern int uwepcursed (void);
extern char WELDED[];

/*### hack.do_wear.c ###*/
extern void armwear (void);
extern void doremarm (void);
extern void doremring (void);
extern void doring (OBJECT obj, int eff);
extern void dowearring (void);

/*### hack.eat.c ###*/
extern void doeat (void);
extern void gethungry (void);
extern void lesshungry (int num);
extern unsigned starved;
extern char *hu_stat[4];

/*### hack.end.c ###*/
extern void clearlocks (void);
extern void done (int status);
extern void done1 (int sig);
extern void do_quit (void);
#ifndef hangup
extern void hangup (int sig);
#endif
extern char *itoa (int a);
extern signed char maxdlevel;

/*### hack.invent.c ###*/
extern OBJECT addinv (OBJECT obj);
extern void ddoinv (void);
extern void delobj (OBJECT obj);
extern void deltrap (GOLD_TRAP trap);
extern void doinv (char *str, int opt);
extern void freeobj (OBJECT obj);
extern GOLD_TRAP g_at (int x, int y, GOLD_TRAP ptr);
extern OBJECT getobj (char *let, char *word);
extern MONSTER m_at (int x, int y);
extern OBJECT o_at (int x, int y);
extern void ofree (OBJECT obj);
extern void prinv (OBJECT obj);
extern void useup (OBJECT obj);

/*### hack.invinit.c ###*/
extern struct obj mace0;
extern struct obj uarm0;
extern struct obj *yourinvent0;

/*### hack.io.c ###*/
extern void getlin (char *str);
extern void getret (void);
extern void hackmode (int x);
extern void more (void);

/*### hack.lev.c ###*/
extern int getlev (int fd);
extern void mklev (void);
extern void mread (int fd, void *buffer, int len);

/*### hack.main.c ###*/
extern void glo (int n);
extern void impossible (void);
extern int rfile;
extern COORDINATES doors[DOORMAX];
extern PART levl[80][22];
extern MKROOM rooms[15];
extern MONSTER fmon;
extern GOLD_TRAP fgold, ftrap;
extern FLAG flags;
extern YOU u;
extern OBJECT fobj, invent, uwep, uarm, uarm2, uleft, uright;
extern char nul[20];		/* Contains zeros */
extern char plname[10], lock[],
           *save_cm,
           *killer,
           *nomvmsg,
            buf[],
            genocided[60],
            SAVEFILE[37];
extern signed char wizard, curx, cury, savx,
                   xdnstair, ydnstair, xupstair, yupstair,
                   dlevel,
                   dx, dy;
extern unsigned moves;
extern int multi;

/*### hack.mkobj.c ###*/
extern void callsrestore (int fd);
extern void callssave (int fd);
extern void mkobj (int let);
extern void restnames (int fd);
extern void savenames (int fd);
extern void shuffle (void);
extern struct armor     armors[];
extern struct food      foods[];
extern char oiden[];
extern signed char oldux, olduy;
extern char *potcall[];
extern char *potcol[];
extern char *pottyp[];
extern char *ringcall[];
extern char *ringtyp[];
extern char *rinnam[];
extern char *scrcall[];
extern char *scrnam[];
extern char *scrtyp[];
extern char *traps[];
extern char *wandcall[];
extern char *wannam[];
extern char *wantyp[];
extern struct weapon    weapons[];

/*### hack.mon.do.c ###*/
extern void cmdel (MONSTER mtmp);
extern void movemon (void);

/*### hack.mon.c ###*/
extern void delmon (MONSTER mtmp);
extern int dist (int x, int y);
extern void killed (MONSTER mtmp);
extern int makemon (MONSTDATA ptr);
extern void mkmonat (MONSTDATA ptr, int x, int y);
extern void mnexto (MONSTER mtmp);
extern void newcham (MONSTER mtmp, MONSTDATA mdat);
extern void p2xthe (char *str, char *name);
extern void poisoned (char *string, char *pname);
extern int psee (int mode, int x, int y, char *str, char *name, char *arg);
extern void pseebl (char *str, char *name);
extern void relmon (MONSTER mtmp);
extern void relobj (MONSTER mtmp);
extern void rescham (void);
extern int r_free (int x, int y, MONSTER mtmp);
extern void rloc (MONSTER mtmp);
extern int somegold (void);
extern void steal (MONSTER mtmp);
extern void stlobj (MONSTER mtmp, OBJECT otmp);
extern void unstuck (MONSTER mtmp);

/*### hack.monst.c ###*/
extern struct permonst li_dog;
extern struct permonst dog;
extern struct permonst la_dog;
extern struct permonst mon[8][7];
#define PM_MIMIC       &mon[5][2]
#define PM_PIERC       &mon[2][3]
#define PM_DEMON       &mon[7][6]
#define PM_CHAM        &mon[6][6]

/*### hack.move.c ###*/
extern void domove (void);
extern void lookaround (void);
extern int movecm (char *cmd);
extern void nomove (void);
extern void nomul (int);
extern char *parse (void);
/* Corners of lit room; l for Low, h for High */
extern signed char seehx, seelx, seehy, seely;

/*### hack.c ###*/
extern int abon (void);
extern int alive (MONSTER monst);
extern int cansee (signed char x, signed char y);
extern void doname (OBJECT obj, char *buffer);
extern int hitu (int mlev, int dam, char *name);
extern int hmon (MONSTER monst, OBJECT obj);
extern void land (void);
extern void losehp (int n, char *knam);
extern void losestr (int num);
extern char *lowc (char *str);
extern long pow2 (int num);
extern char *setan (char *str);
extern void setCon (int setc);
extern void tele (void);
extern void unCoff (int unc, int mode);
extern int weight (OBJECT obj);

/*### hack.office.c ###*/
extern int kantoor (void);

/*### hack.pri.c ###*/
extern void at (int x, int y, char ch);
extern void atl (int x, int y, int ch);
extern void bot (void);
extern void cl_end (void);
extern void cls (void);
extern void curs (int x, int y);
extern void docrt (void);
extern void doreprint (void);
extern void home (void);
extern void levlsym (int x, int y, int c);
extern char news0 (int x, int y);
extern void newsym (int x, int y);
extern void newunseen (int x, int y);
extern void nose1 (int x, int y);
extern void nscr (void);
extern void on (int x, int y);
__attribute__((format(printf, 2, 3)))
extern int panic (int coredump, const char *str, ...);
__attribute__((format(printf, 1, 2)))
extern void pline (const char *line, ...);
extern void pmon (MONSTER mtmp);
extern void prl (int x, int y);
extern void prl1 (int x, int y);
extern void prme (void);
extern void pru (void);
extern void prustr (void);
extern void seeatl (int x, int y, int c);
extern void startup (void);

/*### hack.rip.c ###*/
extern void outrip (void);

/*### hack.save.c ###*/
extern int dorecover (int fd);
extern void save (void);

/*### hack.savelev.c ###*/
extern void bwrite (int fd, void *loc, int num);
extern void savelev (int fd);

/*### hack.shk.c ###*/
extern void addtobill (OBJECT obj);
extern void doinvbill (void);
extern void dopay (void);
extern int inshop (void);
extern int inshproom (int x, int y);
extern int onbill (OBJECT obj);
extern void paybill (void);
extern void setangry (void);
extern void shkdead (void);
extern int shk_move (void);
extern void subfrombill (OBJECT obj);
extern MONSTER shopkeeper;
extern long robbed;
extern signed char billct, shlevel;
extern COORDINATES shd, shk;

/*### hack.str.c ###*/

/*### hack.worm.c ###*/
extern void cutworm (MONSTER mtmp, char x, char y, unsigned weptyp);
extern int getwn (MONSTER mtmp);
extern void initworm (MONSTER mtmp);
extern void wormdead (MONSTER mtmp);
extern void wormhit (MONSTER mtmp);
extern void worm_move (MONSTER mtmp);
extern void worm_nomove (MONSTER mtmp);
extern void wormsee (int tmp);
extern WORMSEGMENT wsegs[32], wheads[32];
extern unsigned wgrowtime[32];

/*### hack.vars.h ###*/
extern char NOTHIN[];
extern char STOPGLOW[];
extern char UMISS[];
extern char WCLEV[];
#endif /* MKLEV */

/*### rnd.c ###*/
extern int d (int n, int x);
extern int rn1 (int x, int y);
extern int rn2 (int x);
extern int rnd (int x);

#endif /* SHOW */
