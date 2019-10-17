/*	SCCS Id: @(#)extern.h	2.3	87/12/12 */
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */

#ifndef EXTERN_H
#define EXTERN_H

#include "config.h"
#include "global.h"
#include "coord.h"
#include "alloc.h"

struct obj;
struct trap;
struct gold;
struct mkroom;
struct shclass;

/* Ignore GCC attributes if we don't have them */
#ifndef __GNUC__
#define __attribute__(x)
#endif

/*** apply.c ***/
extern void dighole(void);
extern int doapply(void);
extern int dorub(void);
extern int holetime(void);

/*** bones.c ***/
extern int getbones(void);
extern void savebones(void);
extern char bones[];

/*** cmd.c ***/
extern void confdir(void);
extern int getdir(boolean s);
extern int isok(int x, int y);
extern int movecmd(char sym);
extern char popch(void);
extern void pushch(char ch);
extern void rhack(char *cmd);
extern void savech(char ch);
extern void set_occupation(int (*fn)(void), const char *txt, int time);
extern char sdir[];
extern schar xdir[];
extern schar ydir[];

/*** decl.c ***/
extern char fut_geno[60];
extern char genocided[60];
extern boolean in_mklev;
extern const char *nomovemsg;
extern char nul[40];
extern char plname[PL_NSIZ];
extern char quitchars[];
extern boolean restoring;
extern boolean stoned;
extern boolean unweapon;
extern long wailmsg;
extern struct monst youmonst;

/*** do.c ***/
extern int doddrop(void);
extern int dodown(void);
extern int dodrop(void);
extern int donull(void);
extern int doup(void);
extern int dowipe(void);
extern void dropx(struct obj *obj);
extern void dropy(struct obj *obj);
extern void goto_level(int newlevel, boolean at_stairs);
extern void heal_legs(void);
extern void more_experienced(int exp, int rexp);
extern void set_wounded_legs(long side, int timex);
extern struct obj *splitobj(struct obj *obj, int num);

/*** do_name.c ***/
extern char *amonnam(struct monst *mtmp, const char *adj);
extern char * Amonnam(struct monst *mtmp, const char *adj);
extern int ddocall(void);
extern char *defmonnam(struct monst *mtmp);
extern void docall(struct obj *obj);
extern int do_mname(void);
extern int getpos(coord *cc, int force, const char *goal);
extern char *lmonnam(struct monst *mtmp);
extern char *monnam(struct monst *mtmp);
extern char *Monnam(struct monst *mtmp);
extern void oname(struct obj *obj, const char *buf);
extern char *xmonnam(struct monst *mtmp, int vb);
extern char *Xmonnam(struct monst *mtmp);

/*** do_wear.c ***/
extern int armoroff(struct obj *otmp);
extern void corrode_armor(void);
extern int doddoremarm(void);
extern int doremarm(void);
extern int doremring(void);
extern int doweararm(void);
extern int dowearring(void);
extern void find_ac(void);
extern void glibr(void);
extern void off_msg(struct obj *otmp);
extern void ringoff(struct obj *obj);
extern struct obj *some_armor(void);

/*** dog.c ***/
extern int dogfood(struct obj *obj);
extern void fall_down(struct monst *mtmp);
extern int inroom(xchar x, xchar y);
extern void keepdogs(void);
extern void losedogs(void);
extern struct monst *makedog(void);
extern int tamedog(struct monst *mtmp, struct obj *obj);
extern struct monst *mydogs;
extern struct permonst li_dog;
extern struct permonst dog;
extern struct permonst la_dog;
extern char dogname[63];

/*** dogmove.c ***/
extern int dog_move(struct monst *mtmp, int after);

/*** dothrow.c ***/
extern int dothrow(void);

/*** eat.c ***/
extern int doeat(void);
extern void gethungry(void);
extern void init_uhunger(void);
extern void lesshungry(int num);
extern void morehungry(int num);
extern void newuhs(boolean incr);
extern int poisonous(struct obj *otmp);
extern char POISONOUS[];
extern const char *hu_stat[];

/*** end.c ***/
extern void charcat(char *s, char c);
extern void clearlocks(void);
extern void done(const char *st1);
extern void done1(int sig);
extern int doquit(void);
extern void done_in_by(struct monst *mtmp);
extern int done_hup;
extern int done_stopprint;

/*** engrave.c ***/
extern int doengrave(void);
extern int freehand(void);
extern void make_engr_at(int x, int y, const char *s);
extern void read_engr_at(int x, int y);
extern void rest_engravings(int fd);
extern void save_engravings(int fd);
extern int sengr_at(const char *s, xchar x, xchar y);
extern void u_wipe_engr(int cnt);
extern void wipe_engr_at(xchar x, xchar y, xchar cnt);

/*** fight.c ***/
extern int attack(struct monst *mtmp);
extern int fightm(struct monst *mtmp);
extern int hitmm(struct monst *magr, struct monst *mdef);
extern boolean hmon(struct monst *mon, struct obj *obj, int thrown);
extern void mondied(struct monst *mdef);
extern int thitu(int tlev, int dam, const char *name);
extern char mlarge[];

/*** fountain.c ***/
extern int dipfountain(struct obj *obj);
extern void drinkfountain(void);
extern void dryup(void);

/*** hack.c ***/
extern int abon(void);
extern int cansee(xchar x, xchar y);
extern void change_luck(schar n);
extern int dbon(void);
extern void domove(void);
extern int dopickup(void);
extern int inv_weight(void);
extern void lookaround(void);
extern void losehp(int n, const char *knam);
extern void losehp_m(int n, struct monst *mtmp);
extern void losestr(int num);
extern void losexp(void);
extern int monster_nearby(void);
extern long newuexp(void);
extern void nomul(int nval);
extern void pickup(int all);
extern void seeoff(int mode);
extern void setsee(void);
extern int sgn(int a);
extern void unsee(void);

/*** invent.c ***/
extern struct obj *addinv(struct obj *obj);
extern int askchain(struct obj *objchn, char *olets, int allflag,
                    int (*fn)(struct obj *), int (*ckfn)(struct obj *),
                    int max);
extern int carried(struct obj *obj);
extern struct obj *carrying(int type);
extern int ddoinv(void);
extern void delobj(struct obj *obj);
extern void deltrap(struct trap *trap);
extern int digit(char c);
extern void doinv(char *lets);
extern int dolook(void);
extern int doprarm(void);
extern int doprgold(void);
extern int doprring(void);
extern int doprwep(void);
extern int dotypeinv(void);
extern void freegold(struct gold *gold);
extern void freeinv(struct obj *obj);
extern void freeobj(struct obj *obj);
extern struct gold *g_at(int x, int y);
extern struct obj *getobj(const char *let, const char *word);
extern int ggetobj(const char *word, int (*fn)(struct obj *), int max);
extern char *let_to_name(char let);
extern struct monst *m_at(int x, int y);
extern struct obj *o_at(int x, int y);
extern struct obj *o_on(int id, struct obj *objchn);
extern void prinv(struct obj *obj);
extern void reassign(void);
extern struct obj *sobj_at(int n, int x, int y);
extern void stackobj(struct obj *obj);
extern struct trap *t_at(int x, int y);
extern void useup(struct obj *obj);
extern void useupf(struct obj *obj);
extern struct wseg *m_atseg;
extern char inv_order[];

/*** ioctl.c ***/
extern int getioctls(void);
extern int setioctls(void);

/*** lev.c ***/
extern void bwrite(int fd, char *loc, unsigned num);
extern void getlev(int fd, int pid, xchar lev);
extern void mklev(void);
extern void mread(int fd, char *buf, unsigned len);
#ifdef DGK
extern int savelev(int fd, xchar lev, int mode);
#else
extern void savelev(int fd, xchar lev);
#endif
extern void savemonchn(int fd, struct monst *mtmp);
extern void saveobjchn(int fd, struct obj *otmp);
extern boolean level_exists[];

/*** unixmain.c ***/
extern void askname(void);
extern void glo(int foo);
__attribute__((format(printf, 1, 2)))
extern void impossible(const char *s, ...);
extern void stop_occupation(void);
extern int locknum;

/*** unixmain.c and pcmain.c ***/
extern char SAVEF[];
extern int hackpid;
extern char *hname;
extern int (*occupation)(void);
extern void (*afternmv)(void);

/*** makemon.c ***/
extern int enexto(coord *cc, xchar xx, xchar yy);
extern int goodpos(int x, int y);
extern struct monst *makemon(struct permonst *ptr, int x, int y);
extern struct monst *mkmon_at(char let, int x, int y);
extern void rloc(struct monst *mtmp);

/*** mhitu.c ***/
extern int demon_hit(struct monst *mtmp);
extern int demon_talk(struct monst *mtmp);
extern int hitu(struct monst *mtmp, int dam);
extern int mhitu(struct monst *mtmp);

/*** mklev.c ***/
extern void makelevel(void);
extern void mktrap(int num, int mazeflag, struct mkroom *croom);
extern int okdoor(int x, int y);
extern int doorindex;
extern int nroom;

/*** mkmaze.c ***/
extern void makemaz(void);
extern int mazexy(coord *cc);

/*** mkobj.c ***/
extern int letter(int c);
extern void mkgold(long num, int x, int y);
extern struct obj *mk_named_obj_at(int let, int x, int y, char *nm, int lth);
extern struct obj *mkobj(int let);
extern struct obj *mkobj_at(int let, int x, int y);
extern struct obj *mksobj(int otyp);
extern struct obj *mksobj_at(int otyp, int x, int y);
extern int weight(struct obj *obj);
extern struct obj zeroobj;

/*** mkshop.c ***/
extern void mkroom(int roomtype);                  /* make and stock a room of a given type */
extern boolean nexttodoor(int sx, int sy);         /* TRUE if adjacent to a door */
extern boolean has_dnstairs(struct mkroom *sroom); /* TRUE if given room has a down staircase */
extern boolean has_upstairs(struct mkroom *sroom); /* TRUE if given room has an up staircase */
extern int dist2(int x0, int y0, int x1, int y1);  /* Euclidean square-of-distance function */
extern struct permonst *courtmon(void);            /* generate a court monster */

/*** mon.c ***/
extern int canseemon(struct monst *mtmp);
extern int dist(int x, int y);
extern int disturb(struct monst *mtmp);
extern void justswld(struct monst *mtmp, char *name);
extern void killed(struct monst *mtmp);
extern void kludge(const char *str, const char *arg1, const char *arg2);
extern void meatgold(struct monst *mtmp);
extern int mfndpos(struct monst *mon, coord poss[9], long info[9], long flag);
extern void mnexto(struct monst *mtmp);
extern void mondead(struct monst *mtmp);
extern void monfree(struct monst *mtmp);
extern void movemon(void);
extern void mpickgems(struct monst *mtmp);
extern void mpickgold(struct monst *mtmp);
extern int newcham(struct monst *mtmp, struct permonst *mdat);
extern void poisoned(const char *string, const char *pname);
extern void relmon(struct monst *mon);
extern void replmon(struct monst *mtmp, struct monst *mtmp2);
extern void rescham(void);
extern void restartcham(void);
extern void setmangry(struct monst *mtmp);
extern void unstuck(struct monst *mtmp);
extern void xkilled(struct monst *mtmp, int dest);
extern void youswld(struct monst *mtmp, int dam, int die, char *name);
extern int warnlevel;

/*** monmove.c ***/
extern int dochug(struct monst *mtmp);
extern int dochugw(struct monst *mtmp);
extern int m_move(struct monst *mtmp, int after);

/*** monst.c ***/
extern struct permonst pm_djinni;
extern struct permonst pm_eel;
extern struct permonst pm_ghost;
extern struct permonst pm_gremlin;
extern struct permonst pm_mail_daemon;
extern struct permonst pm_medusa;
extern struct permonst pm_moe, pm_larry, pm_curly;
extern struct permonst pm_soldier;
extern struct permonst pm_wizard;

/*** o_init.c ***/
extern int dodiscovered(void);
extern int init_corpses(void);
extern void init_objects(void);
extern int letindex(char let);
extern void oinit(void);
extern int probtype(char let);
extern void restnames(int fd);
extern void savenames(int fd);
extern int bases[];
extern char obj_symbols[];

/*** objnam.c ***/
extern char *aobjnam(struct obj *otmp, const char *verb);
extern char *doname(struct obj *obj);
extern char *Doname(struct obj *obj);
extern struct obj *readobjnam(char *bp);
extern void setan(const char *str, char *buf);
extern char *typename(int otyp);
extern char *xname(struct obj *obj);

/*** options.c ***/
extern int doset(void);
extern int dotogglepickup(void);
extern void initoptions(void);
extern void parseoptions(char *opts, boolean from_env);

/*** pager.c ***/
extern int child(int wt);
extern void cornline(int mode, const char *text);
extern int dohelp(void);
extern int dosh(void);
extern int dowhatis(void);
extern int page_file(const char *fnam, boolean silent);
extern int page_line(const char *s);
extern int readnews(void);
extern void set_pager(int mode);
extern void set_whole_screen(void);

/*** polyself.c ***/
extern int cantweararm(char c);
extern int cantwield(char c);
extern int dobreathe(void);
extern int doremove(void);
extern int humanoid(char c);
extern void polyself(void);
extern void rehumanize(void);

/*** potion.c ***/
extern void djinni_from_bottle(void);
extern int dodip(void);
extern int dodrink(void);
extern void gainstr(int inc);
extern void healup(int nhp, int nxtra, boolean curesick, boolean cureblind);
extern int peffects(struct obj *otmp);
extern void pluslvl(void);
extern void potionbreathe(struct obj *obj);
extern void potionhit(struct monst *mon, struct obj *obj);
extern void strange_feeling(struct obj *obj, const char *txt);

/*** pray.c ***/
extern int dopray(void);
extern int dosacrifice(void);
extern int doturn(void);

/*** pri.c ***/
extern void at(xchar x, xchar y, char ch);
extern void bot(void);
extern void cls(void);
extern void docorner(int xmin, int ymax);
extern void docrt(void);
extern int doredraw(void);
extern const char *hcolor(void);
extern void mstatusline(struct monst *mtmp);
extern void nscr(void);
extern void pmon(struct monst *mon);
extern void prme(void);
extern int rndmonsym(void);
extern int rndobjsym(void);
extern void seemons(void);
extern void seeobjs(void);
extern void setclipped(void);
extern void swallowed(void);
extern void unpmon(struct monst *mon);
extern void ustatusline(void);
extern xchar scrlx, scrhx, scrly, scrhy;

/*** prisym.c ***/
extern void atl(int x, int y, int ch);
extern void curs_on_u(void);
extern void mnewsym(int x, int y);
extern char news0(xchar x, xchar y);
extern void newsym(int x, int y);
extern void nose1(int x, int y);
extern void nosee(int x, int y);
extern void on_scr(int x, int y);
extern void prl(int x, int y);
extern void prl1(int x, int y);
extern void pru(void);
extern void tmp_at(int x, int y);
extern void Tmp_at(int x, int y);
extern void unpobj(struct obj *obj);
extern int vism_at(int x, int y);

/*** read.c ***/
extern int destroy_arm(void);
extern void do_genocide(void);
extern void do_mapping(void);
extern int doread(void);
extern int identify(struct obj *otmp);
extern void litroom(boolean on);
extern int seffects(struct obj *sobj);

/*** rip.c ***/
extern void outrip(void);

/*** rnd.c ***/
extern int d(int n, int x);
extern int rn1(int x, int y);
extern int rn2(int x);
extern int rnd(int x);
extern int rne(int x);
extern int rnz(int x);

/*** rumors.c ***/
extern void outrumor(void);

/*** save.c ***/
extern int dorecover(int fd);
extern int dosave(void);
extern int dosave0(int hu);
extern void nh_hangup(int sig);
extern struct monst *restmonchn(int fd);
extern struct obj *restobjchn(int fd);

/*** search.c ***/
extern int doidtrap(void);
extern int dosearch(void);
extern int findit(void);
extern void seemimic(struct monst *mtmp);
extern void wakeup(struct monst *mtmp);

/*** shk.c ***/
extern void addtobill(struct obj *obj);
extern int doinvbill(int mode);
extern int dopay(void);
extern int follower(struct monst *mtmp);
extern int inshop(void);
extern void obfree(struct obj *obj, struct obj *merge);
extern int online(xchar x, xchar y);
extern void paybill(void);
extern void replshk(struct monst *mtmp, struct monst *mtmp2);
extern int shkcatch(struct obj *obj);
extern void shkdead(struct monst *mtmp);
extern int shk_move(struct monst *shkp);
extern char *shkname(struct monst *mtmp);
extern void shopdig(int fall);
extern void splitbill(struct obj *obj, struct obj *otmp);
extern void subfrombill(struct obj *obj);
extern struct obj *billobjs;

/*** shknam.c ***/
extern int saleable(int nshop, struct obj *obj);
extern void stock_room(struct shclass *shp, struct mkroom *sroom);

/*** sit.c ***/
extern void attrcurse(void);
extern int dosit(void);
extern void rndcurse(void);

/*** spell.c ***/
extern int docast(void);
extern int dovspell(void);
extern int doxcribe(void);
extern void losespells(void);

/*** steal.c ***/
extern void mpickobj(struct monst *mtmp, struct obj *otmp);
extern void relobj(struct monst *mtmp, int show);
extern long somegold(void);
extern int steal(struct monst *mtmp);
extern int stealamulet(struct monst *mtmp);
extern void stealgold(struct monst *mtmp);

/*** termcap.c ***/
extern void backsp(void);
extern void cl_end(void);
extern void cl_eos(void);
extern void cmov(int x, int y);
extern void curs(int x, int y);
extern void delay_output(void);
extern void end_screen(void);
extern void home(void);
extern void nh_bell(void);
extern void nh_clear_screen(void);
extern void standoutbeg(void);
extern void standoutend(void);
extern void start_screen(void);
extern void startup(void);
extern int xputc(int c);
extern void xputs(const char *s);
extern const char *CD;
extern int CO, LI;
extern short ospeed;

/*** timeout.c ***/
extern void timeout(void);

/*** topl.c ***/
extern void addtopl(const char *s);
extern void clrlin(void);
extern void cmore(const char *s);
extern int doredotopl(void);
extern void more(void);
__attribute__((format(printf, 1, 2)))
extern void pline(const char *line, ...);
extern void putstr(const char *s);
extern void putsym(char c);
extern void remember_topl(void);

/*** topten.c ***/
extern char *eos(char *s);
extern void prscore(int argc, char **argv);
extern void topten(void);

/*** track.c ***/
extern coord *gettrack(int x, int y);
extern void initrack(void);
extern void settrack(void);

/*** trap.c ***/
extern int dotele(void);
extern void dotrap(struct trap *trap);
extern void drown(void);
extern void float_down(void);
extern void float_up(void);
extern int has_amulet(void);
extern void level_tele(void);
extern struct trap *maketrap(int x, int y, int typ);
extern int mintrap(struct monst *mtmp);
extern void placebc(int attach);
extern void selftouch(const char *arg);
extern void tele(void);
extern void unplacebc(void);

/*** unixtty.c ***/
extern void cgetret(const char *s);
__attribute__((format(printf, 1, 2)))
extern int error(const char *s, ...);
extern void get_ext_cmd(char *bufp);
extern void getlin(char *bufp);
extern void getret(void);
extern void gettty(void);
extern char *parse(void);
extern char readchar(void);
extern void setftty(void);
extern void settty(const char *s);
extern void xwaitforspace(const char *s);

/*** unixtty.c and pctty.c ***/
extern char morc;

/*** u_init.c ***/
extern void plnamesuffix(void);
extern void u_init(void);
extern char pl_character[PL_CSIZ];

/*** unix.c ***/
extern void ckmailstatus(void);
extern char *getdate(void);
extern void gethdate(char *name);
extern void getlock(void);
extern void getmailstatus(void);
extern int getyear(void);
extern int midnight(void);
extern int night(void);
extern int phase_of_the_moon(void);
extern void readmail(void);
extern void regularize(char *s);
extern void setrandom(void);
extern int uptodate(int fd);

/*** vault.c ***/
extern void gddead(void);
extern int gd_move(void);
extern void invault(void);
extern void replgd(struct monst *mtmp, struct monst *mtmp2);
extern void setgd(void);

/*** version.c ***/
extern int doMSCversion(void);
extern int doversion(void);

/*** wield.c ***/
extern int chwepon(struct obj *otmp, int amount);
extern void corrode_weapon(void);
extern int dowield(void);
extern void setuwep(struct obj *obj);
extern int welded(struct obj *obj);

/*** wizard.c ***/
extern void aggravate(void);
extern void amulet(void);
extern int inrange(struct monst *mtmp);
extern void intervene(void);
extern void wizdead(struct monst *mtmp);
extern int wiz_hit(struct monst *mtmp);

/*** worm.c ***/
extern void cutworm(struct monst *mtmp, xchar x, xchar y, uchar weptyp);
extern int getwn(struct monst *mtmp);
extern void initworm(struct monst *mtmp);
extern void pwseg(struct wseg *wtmp);
extern void wormdead(struct monst *mtmp);
extern void wormhit(struct monst *mtmp);
extern void worm_move(struct monst *mtmp);
extern void worm_nomove(struct monst *mtmp);
extern void wormsee(unsigned tmp);
extern long wgrowtime[32];
extern struct wseg *wheads[32];
extern struct wseg *wsegs[32];

/*** worn.c ***/
extern void setnotworn(struct obj *obj);
extern void setworn(struct obj *obj, long mask);

/*** write.c ***/
extern int dowrite(struct obj *pen);

/*** zap.c ***/
extern struct monst *bhit(int ddx, int ddy, int range, char sym,
                          void (*fhitm)(struct monst *, struct obj *),
                          int (*fhito)(struct obj *, struct obj *),
                          struct obj *obj);
extern struct monst *boomhit(int dx, int dy);
extern void buzz(int type, xchar sx, xchar sy, int dx, int dy);
extern int dozap(void);
extern const char *exclam(int force);
extern void fracture_rock(struct obj *obj);
extern void hit(const char *str, struct monst *mtmp, const char *force);
extern void makewish(void);
extern void miss(const char *str, struct monst *mtmp);
extern int resist(struct monst *mtmp, char olet, int damage, int tell);
extern void weffects(struct obj *obj);
extern void zapnodir(struct obj *wand);
extern int zappable(struct obj *wand);
extern int zapyourself(struct obj *obj);

extern xchar xdnstair, ydnstair, xupstair, yupstair; /* stairs up and down. */

extern xchar dlevel;

#ifdef SPELLS
#include "spell.h"
extern struct spell spl_book[]; /* sized in decl.c */
#endif

extern int occtime;
extern const char *occtxt; /* defined when occupation != NULL */

#ifdef REDO
extern int in_doagain;
#endif

extern char *HI, *HE; /* set up in termcap.c */
#ifdef MSDOSCOLOR
extern char *HI_MON, *HI_OBJ; /* set up in termcap.c */
#endif

#include "obj.h"
extern struct obj *invent, *uwep, *uarm, *uarm2, *uarmh, *uarms, *uarmg,
#ifdef SHIRT
    *uarmu, /* under-wear, so to speak */
#endif
    *uleft, *uright, *fcobj;
extern struct obj *uchain; /* defined iff PUNISHED */
extern struct obj *uball;  /* defined if PUNISHED */

extern const char *traps[];
extern char vowels[];

#include "you.h"

extern struct you u;

extern xchar curx, cury; /* cursor location on screen */

extern xchar seehx, seelx, seehy, seely; /* where to see*/
extern char *save_cm;
extern const char *killer;

extern xchar dlevel, maxdlevel; /* dungeon level */

extern long moves;

extern int multi;

extern char lock[];

#endif
