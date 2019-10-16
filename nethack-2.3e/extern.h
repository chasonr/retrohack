/*	SCCS Id: @(#)extern.h	2.3	87/12/12 */
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */

#include "config.h"
#include "global.h"

/*
 *	I have been told, that in Sys V R3.1, this has to be commented out.
 */
#ifndef MSDOS
/*extern char *sprintf();*/
#endif

#include "coord.h"
#include "alloc.h"

/*** apply.c ***/
extern void dighole(/*unknown*/);
extern int doapply(/*unknown*/);
extern int dorub(/*unknown*/);
extern int holetime(/*unknown*/);

/*** bones.c ***/
extern int getbones(/*unknown*/);
extern void savebones(/*unknown*/);

/*** cmd.c ***/
extern void confdir(/*unknown*/);
extern int getdir(/*unknown*/);
extern int isok(/*unknown*/);
extern int movecmd(/*unknown*/);
extern char popch();
extern void pushch(/*unknown*/);
extern void rhack(/*unknown*/);
extern void savech(/*unknown*/);
extern void set_occupation(/*unknown*/);
extern char sdir[];
extern schar xdir[];
extern schar ydir[];

/*** decl.c ***/
extern char fut_geno[60];
extern char genocided[60];
extern boolean in_mklev;
extern char *nomovemsg;
extern char nul[40];
extern char plname[PL_NSIZ];
extern char quitchars[];
extern boolean restoring;
extern boolean stoned;
extern boolean unweapon;
extern long wailmsg;
extern struct monst youmonst;

/*** do.c ***/
extern int doddrop(/*unknown*/);
extern int dodown(/*unknown*/);
extern int dodrop(/*unknown*/);
extern int donull(/*unknown*/);
extern int doup(/*unknown*/);
extern int dowipe(/*unknown*/);
extern void dropx(/*unknown*/);
extern void dropy(/*unknown*/);
extern void goto_level(/*unknown*/);
extern void heal_legs(/*unknown*/);
extern void more_experienced(/*unknown*/);
extern void set_wounded_legs(/*unknown*/);
extern struct obj *splitobj(/*unknown*/);

/*** do_name.c ***/
extern char *amonnam();
extern char *Amonnam();
extern int ddocall(/*unknown*/);
extern char *defmonnam(/*unknown*/);
extern void docall(/*unknown*/);
extern int do_mname(/*unknown*/);
extern int getpos(/*unknown*/);
extern char *lmonnam(/*unknown*/);
extern char *monnam();
extern char *Monnam();
extern void oname(/*unknown*/);
extern char *xmonnam(/*unknown*/);
extern char *Xmonnam(/*unknown*/);

/*** do_wear.c ***/
extern int armoroff(/*unknown*/);
extern void corrode_armor(/*unknown*/);
extern int doddoremarm(/*unknown*/);
extern int doremarm(/*unknown*/);
extern int doremring(/*unknown*/);
extern int doweararm(/*unknown*/);
extern int dowearring(/*unknown*/);
extern void find_ac(/*unknown*/);
extern void glibr(/*unknown*/);
extern void off_msg(/*unknown*/);
extern void ringoff(/*unknown*/);
extern struct obj *some_armor(/*unknown*/);

/*** dog.c ***/
extern int dogfood(/*unknown*/);
extern void fall_down(/*unknown*/);
extern int inroom(/*unknown*/);
extern void keepdogs(/*unknown*/);
extern void losedogs(/*unknown*/);
extern struct monst *makedog(/*unknown*/);
extern int tamedog(/*unknown*/);
extern struct monst *mydogs;
extern struct permonst li_dog;
extern struct permonst dog;
extern struct permonst la_dog;
extern char dogname[63];

/*** dogmove.c ***/
extern int dog_move(/*unknown*/);

/*** dothrow.c ***/
extern int dothrow(/*unknown*/);

/*** eat.c ***/
extern int doeat(/*unknown*/);
extern void gethungry(/*unknown*/);
extern void init_uhunger(/*unknown*/);
extern void lesshungry(/*unknown*/);
extern void morehungry(/*unknown*/);
extern void newuhs(/*unknown*/);
extern int poisonous(/*unknown*/);
extern char POISONOUS[];
extern char *hu_stat[];

/*** end.c ***/
extern void charcat(/*unknown*/);
extern void clearlocks(/*unknown*/);
extern void done(/*unknown*/);
extern void done1(int sig);
extern int doquit(void);
extern void done_in_by(/*unknown*/);
extern int done_hup;
extern int done_stopprint;

/*** engrave.c ***/
extern int doengrave(/*unknown*/);
extern int freehand(/*unknown*/);
extern void make_engr_at(/*unknown*/);
extern void read_engr_at(/*unknown*/);
extern void rest_engravings(/*unknown*/);
extern void save_engravings(/*unknown*/);
extern int sengr_at(/*unknown*/);
extern void u_wipe_engr(/*unknown*/);
extern void wipe_engr_at(/*unknown*/);

/*** fight.c ***/
extern int attack(/*unknown*/);
extern int fightm(/*unknown*/);
extern int hitmm(/*unknown*/);
extern boolean hmon(/*unknown*/);
extern void mondied(/*unknown*/);
extern int thitu(/*unknown*/);
extern char mlarge[];

/*** fountain.c ***/
extern int dipfountain(/*unknown*/);
extern void drinkfountain(/*unknown*/);
extern void dryup(/*unknown*/);

/*** hack.c ***/
extern int abon(/*unknown*/);
extern int cansee(/*unknown*/);
extern void change_luck(/*unknown*/);
extern int dbon(/*unknown*/);
extern void domove(/*unknown*/);
extern int dopickup(/*unknown*/);
extern int inv_weight(/*unknown*/);
extern void lookaround(/*unknown*/);
extern void losehp(/*unknown*/);
extern void losehp_m(/*unknown*/);
extern void losestr(/*unknown*/);
extern void losexp(/*unknown*/);
extern int monster_nearby(/*unknown*/);
extern long newuexp(/*unknown*/);
extern void nomul(/*unknown*/);
extern void pickup(/*unknown*/);
extern void seeoff(/*unknown*/);
extern void setsee(/*unknown*/);
extern int sgn(/*unknown*/);
extern void unsee(/*unknown*/);

/*** invent.c ***/
extern struct obj *addinv(/*unknown*/);
extern int askchain(/*unknown*/);
extern int carried(/*unknown*/);
extern struct obj *carrying(/*unknown*/);
extern int ddoinv(/*unknown*/);
extern void delobj(/*unknown*/);
extern void deltrap(/*unknown*/);
extern int digit(/*unknown*/);
extern void doinv(/*unknown*/);
extern int dolook(/*unknown*/);
extern int doprarm(/*unknown*/);
extern int doprgold(/*unknown*/);
extern int doprring(/*unknown*/);
extern int doprwep(/*unknown*/);
extern int dotypeinv(/*unknown*/);
extern void freegold(/*unknown*/);
extern void freeinv(/*unknown*/);
extern void freeobj(/*unknown*/);
extern struct gold *g_at(/*unknown*/);
struct obj *getobj();
extern int ggetobj(/*unknown*/);
extern char *let_to_name(/*unknown*/);
extern struct monst *m_at(/*unknown*/);
struct obj *o_at();
extern struct obj *o_on(/*unknown*/);
extern void prinv(/*unknown*/);
extern void reassign(/*unknown*/);
struct obj *sobj_at();
extern void stackobj(/*unknown*/);
extern struct trap *t_at(/*unknown*/);
extern void useup(/*unknown*/);
extern void useupf(/*unknown*/);
extern struct wseg *m_atseg;
extern char inv_order[];

/*** ioctl.c ***/
extern int getioctls(/*unknown*/);
extern int setioctls(/*unknown*/);

/*** lev.c ***/
extern void bwrite(/*unknown*/);
extern void getlev(/*unknown*/);
extern void mklev(/*unknown*/);
extern void mread(/*unknown*/);
extern void savelev(/*unknown*/);
extern void savemonchn(/*unknown*/);
extern void saveobjchn(/*unknown*/);
extern boolean level_exists[];

/*** unixmain.c ***/
extern void askname(/*unknown*/);
extern void glo(/*unknown*/);
extern void impossible(/*unknown*/);
extern void stop_occupation(/*unknown*/);
extern int locknum;

/*** unixmain.c and pcmain.c ***/
extern char SAVEF[];
extern int hackpid;
extern char *hname;
extern int (*occupation)(void);
extern void (*afternmv)(void);

/*** makemon.c ***/
extern int enexto(/*unknown*/);
extern int goodpos(/*unknown*/);
extern struct monst *makemon(/*unknown*/);
extern struct monst *mkmon_at(/*unknown*/);
extern void rloc(/*unknown*/);

/*** mhitu.c ***/
extern int demon_hit(/*unknown*/);
extern int demon_talk(/*unknown*/);
extern int hitu(/*unknown*/);
extern int mhitu(/*unknown*/);

/*** mklev.c ***/
extern void makelevel(/*unknown*/);
extern void mktrap(/*unknown*/);
extern int okdoor(/*unknown*/);
extern int doorindex;
extern int nroom;

/*** mkmaze.c ***/
extern void makemaz(/*unknown*/);
extern int mazexy(/*unknown*/);

/*** mkobj.c ***/
extern int letter(/*unknown*/);
extern void mkgold(/*unknown*/);
extern struct obj *mk_named_obj_at(/*unknown*/);
extern struct obj *mkobj(/*unknown*/);
extern struct obj *mkobj_at(/*unknown*/);
extern struct obj *mksobj(/*unknown*/);
extern struct obj *mksobj_at(/*unknown*/);
extern int weight(/*unknown*/);
extern struct obj zeroobj;

/*** mkshop.c ***/
extern struct permonst *courtmon(/*unknown*/);
extern int dist2(/*unknown*/);
extern boolean has_dnstairs(/*unknown*/);
extern boolean has_upstairs(/*unknown*/);
extern void mkroom(/*unknown*/);
extern boolean nexttodoor(/*unknown*/);

/*** mon.c ***/
extern int canseemon(/*unknown*/);
extern int dist(/*unknown*/);
extern int disturb(/*unknown*/);
extern void justswld(/*unknown*/);
extern void killed(/*unknown*/);
extern void kludge(/*unknown*/);
extern void meatgold(/*unknown*/);
extern int mfndpos(/*unknown*/);
extern void mnexto(/*unknown*/);
extern void mondead(/*unknown*/);
extern void monfree(/*unknown*/);
extern void movemon(/*unknown*/);
extern void mpickgems(/*unknown*/);
extern void mpickgold(/*unknown*/);
extern int newcham(/*unknown*/);
extern void poisoned(/*unknown*/);
extern void relmon(/*unknown*/);
extern void replmon(/*unknown*/);
extern void rescham(/*unknown*/);
extern void restartcham(/*unknown*/);
extern void setmangry(/*unknown*/);
extern void unstuck(/*unknown*/);
extern void xkilled(/*unknown*/);
extern void youswld(/*unknown*/);
extern int warnlevel;

/*** monmove.c ***/
extern int dochug(/*unknown*/);
extern int dochugw(/*unknown*/);
extern int m_move(/*unknown*/);

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
extern int dodiscovered(/*unknown*/);
extern int init_corpses(/*unknown*/);
extern void init_objects(/*unknown*/);
extern int letindex(/*unknown*/);
extern void oinit(/*unknown*/);
extern int probtype(/*unknown*/);
extern void restnames(/*unknown*/);
extern void savenames(/*unknown*/);
extern int bases[];
extern char obj_symbols[];

/*** objnam.c ***/
extern char *aobjnam();
extern char *doname();
extern char *Doname(/*unknown*/);
extern struct obj *readobjnam(/*unknown*/);
extern void setan(/*unknown*/);
extern char *typename(/*unknown*/);
extern char *xname(/*unknown*/);

/*** options.c ***/
extern int doset(/*unknown*/);
extern int dotogglepickup(/*unknown*/);
extern void initoptions(/*unknown*/);
extern void parseoptions();

/*** pager.c ***/
extern int child(/*unknown*/);
extern void cornline(/*unknown*/);
extern int dohelp(/*unknown*/);
extern int dosh(/*unknown*/);
extern int dowhatis(/*unknown*/);
extern int page_file();
extern int page_line(/*unknown*/);
extern int readnews(/*unknown*/);
extern void set_pager(/*unknown*/);
extern void set_whole_screen(/*unknown*/);

/*** polyself.c ***/
extern int cantweararm(/*unknown*/);
extern int cantwield(/*unknown*/);
extern int dobreathe(/*unknown*/);
extern int doremove(/*unknown*/);
extern int humanoid(/*unknown*/);
extern void polyself(/*unknown*/);
extern void rehumanize(/*unknown*/);

/*** potion.c ***/
extern void djinni_from_bottle(/*unknown*/);
extern int dodip(/*unknown*/);
extern int dodrink(/*unknown*/);
extern void gainstr(/*unknown*/);
extern void healup(/*unknown*/);
extern int peffects(/*unknown*/);
extern void pluslvl(/*unknown*/);
extern void potionbreathe(/*unknown*/);
extern void potionhit(/*unknown*/);
extern void strange_feeling(/*unknown*/);

/*** pray.c ***/
extern int dopray(/*unknown*/);
extern int dosacrifice(/*unknown*/);
extern int doturn(/*unknown*/);

/*** pri.c ***/
extern void at(/*unknown*/);
extern void bot(/*unknown*/);
extern void cls(/*unknown*/);
extern void docorner(/*unknown*/);
extern void docrt(/*unknown*/);
extern int doredraw(/*unknown*/);
extern char *hcolor(/*unknown*/);
extern void mstatusline(/*unknown*/);
extern void nscr(/*unknown*/);
extern void pmon(/*unknown*/);
extern void prme(/*unknown*/);
extern int rndmonsym(/*unknown*/);
extern int rndobjsym(/*unknown*/);
extern void seemons(/*unknown*/);
extern void seeobjs(/*unknown*/);
extern void setclipped(/*unknown*/);
extern void swallowed(/*unknown*/);
extern void unpmon(/*unknown*/);
extern void ustatusline(/*unknown*/);
extern xchar scrlx, scrhx, scrly, scrhy;

/*** prisym.c ***/
extern void atl(/*unknown*/);
extern void curs_on_u(/*unknown*/);
extern void mnewsym(/*unknown*/);
extern char news0(/*unknown*/);
extern void newsym(/*unknown*/);
extern void nose1(/*unknown*/);
extern void nosee(/*unknown*/);
extern void on_scr(/*unknown*/);
extern void prl(/*unknown*/);
extern void prl1(/*unknown*/);
extern void pru(/*unknown*/);
extern void tmp_at(/*unknown*/);
extern void Tmp_at(/*unknown*/);
extern void unpobj(/*unknown*/);
extern int vism_at(/*unknown*/);

/*** read.c ***/
extern int destroy_arm(/*unknown*/);
extern void do_genocide(/*unknown*/);
extern void do_mapping(/*unknown*/);
extern int doread(/*unknown*/);
extern int identify(/*unknown*/);
extern void litroom(/*unknown*/);
extern int seffects(/*unknown*/);

/*** rip.c ***/
extern void outrip(/*unknown*/);

/*** rnd.c ***/
extern int d(/*unknown*/);
extern int rn1(/*unknown*/);
extern int rn2(/*unknown*/);
extern int rnd(/*unknown*/);
extern int rne(/*unknown*/);
extern int rnz(/*unknown*/);

/*** rumors.c ***/
extern void outrumor(/*unknown*/);

/*** save.c ***/
extern int dorecover(/*unknown*/);
extern int dosave(/*unknown*/);
extern int dosave0(/*unknown*/);
extern void nh_hangup(int sig);
extern struct monst *restmonchn(/*unknown*/);
extern struct obj *restobjchn(/*unknown*/);

/*** search.c ***/
extern int doidtrap(/*unknown*/);
extern int dosearch(/*unknown*/);
extern int findit(/*unknown*/);
extern void seemimic(/*unknown*/);
extern void wakeup(/*unknown*/);

/*** shk.c ***/
extern void addtobill(/*unknown*/);
extern int doinvbill(/*unknown*/);
extern int dopay(/*unknown*/);
extern int follower(/*unknown*/);
extern int inshop(/*unknown*/);
extern void obfree(/*unknown*/);
extern int online(/*unknown*/);
extern void paybill(/*unknown*/);
extern void replshk(/*unknown*/);
extern int shkcatch(/*unknown*/);
extern void shkdead(/*unknown*/);
extern int shk_move(/*unknown*/);
extern char *shkname(/*unknown*/);
extern void shopdig(/*unknown*/);
extern void splitbill(/*unknown*/);
extern void subfrombill(/*unknown*/);
extern struct obj *billobjs;

/*** shknam.c ***/
extern int saleable(/*unknown*/);
extern void stock_room(/*unknown*/);

/*** sit.c ***/
extern void attrcurse(/*unknown*/);
extern int dosit(/*unknown*/);
extern void rndcurse(/*unknown*/);

/*** spell.c ***/
extern int docast(/*unknown*/);
extern int dovspell(/*unknown*/);
extern int doxcribe(/*unknown*/);
extern void losespells(/*unknown*/);

/*** steal.c ***/
extern void mpickobj(/*unknown*/);
extern void relobj(/*unknown*/);
extern long somegold(/*unknown*/);
extern int steal(/*unknown*/);
extern int stealamulet(/*unknown*/);
extern void stealgold(/*unknown*/);

/*** termcap.c ***/
extern void backsp(/*unknown*/);
extern void cl_end(/*unknown*/);
extern void cl_eos(/*unknown*/);
extern void cmov();
extern void curs(/*unknown*/);
extern void delay_output(/*unknown*/);
extern void end_screen(/*unknown*/);
extern void home(/*unknown*/);
extern void nh_bell(/*unknown*/);
extern void nh_clear_screen(/*unknown*/);
extern void standoutbeg(/*unknown*/);
extern void standoutend(/*unknown*/);
extern void start_screen(/*unknown*/);
extern void startup(/*unknown*/);
extern int xputc(int c);
extern void xputs(/*unknown*/);
extern char *CD;
extern int CO, LI;
extern short ospeed;

/*** timeout.c ***/
extern void timeout(/*unknown*/);

/*** topl.c ***/
extern void addtopl(/*unknown*/);
extern void clrlin(/*unknown*/);
extern void cmore(/*unknown*/);
extern int doredotopl(/*unknown*/);
extern void more(/*unknown*/);
extern void pline(/*unknown*/);
extern void putstr(/*unknown*/);
extern void putsym(/*unknown*/);
extern void remember_topl(/*unknown*/);

/*** topten.c ***/
extern char *eos(/*unknown*/);
extern void prscore(/*unknown*/);
extern void topten(/*unknown*/);

/*** track.c ***/
extern coord *gettrack(/*unknown*/);
extern void initrack(/*unknown*/);
extern void settrack(/*unknown*/);

/*** trap.c ***/
extern int dotele(/*unknown*/);
extern void dotrap(/*unknown*/);
extern void drown(/*unknown*/);
extern void float_down(/*unknown*/);
extern void float_up(/*unknown*/);
extern int has_amulet(/*unknown*/);
extern void level_tele(/*unknown*/);
extern struct trap *maketrap(/*unknown*/);
extern int mintrap(/*unknown*/);
extern void placebc(/*unknown*/);
extern void selftouch(/*unknown*/);
extern void tele(/*unknown*/);
extern void unplacebc(/*unknown*/);

/*** unixtty.c ***/
extern void cgetret(/*unknown*/);
extern int error(/*unknown*/);
extern void get_ext_cmd(/*unknown*/);
extern void getlin(/*unknown*/);
extern void getret(/*unknown*/);
extern void gettty(/*unknown*/);
extern char *parse(/*unknown*/);
extern char readchar();
extern void setftty(/*unknown*/);
extern void settty(/*unknown*/);
extern void xwaitforspace(/*unknown*/);

/*** unixtty.c and pctty.c ***/
extern char morc;

/*** u_init.c ***/
extern void plnamesuffix(/*unknown*/);
extern void u_init(/*unknown*/);
extern char pl_character[PL_CSIZ];

/*** unix.c ***/
extern void ckmailstatus(/*unknown*/);
extern char *getdate(/*unknown*/);
extern void gethdate(/*unknown*/);
extern void getlock(/*unknown*/);
extern void getmailstatus(/*unknown*/);
extern int getyear(/*unknown*/);
extern int midnight(/*unknown*/);
extern int night(/*unknown*/);
extern int phase_of_the_moon(/*unknown*/);
extern void readmail(/*unknown*/);
extern void regularize(/*unknown*/);
extern void setrandom(/*unknown*/);
extern int uptodate(/*unknown*/);

/*** vault.c ***/
extern void gddead(/*unknown*/);
extern int gd_move(/*unknown*/);
extern void invault(/*unknown*/);
extern void replgd(/*unknown*/);
extern void setgd(/*unknown*/);

/*** version.c ***/
extern int doMSCversion(/*unknown*/);
extern int doversion(/*unknown*/);

/*** wield.c ***/
extern int chwepon(/*unknown*/);
extern void corrode_weapon(/*unknown*/);
extern int dowield(/*unknown*/);
extern void setuwep(/*unknown*/);
extern int welded(/*unknown*/);

/*** wizard.c ***/
extern void aggravate(/*unknown*/);
extern void amulet(/*unknown*/);
extern int inrange(/*unknown*/);
extern void intervene(/*unknown*/);
extern void wizdead(/*unknown*/);
extern int wiz_hit(/*unknown*/);

/*** worm.c ***/
extern void cutworm(/*unknown*/);
extern int getwn(/*unknown*/);
extern void initworm(/*unknown*/);
extern void pwseg(/*unknown*/);
extern void wormdead(/*unknown*/);
extern void wormhit(/*unknown*/);
extern void worm_move(/*unknown*/);
extern void worm_nomove(/*unknown*/);
extern void wormsee(/*unknown*/);
extern long wgrowtime[32];
extern struct wseg *wheads[32];
extern struct wseg *wsegs[32];

/*** worn.c ***/
extern void setnotworn(/*unknown*/);
extern void setworn(/*unknown*/);

/*** write.c ***/
extern int dowrite(/*unknown*/);

/*** zap.c ***/
extern struct monst *bhit(/*unknown*/);
extern struct monst *boomhit(/*unknown*/);
extern void buzz(/*unknown*/);
extern int dozap(/*unknown*/);
extern char *exclam(/*unknown*/);
extern void fracture_rock(/*unknown*/);
extern void hit(/*unknown*/);
extern void makewish(/*unknown*/);
extern void miss(/*unknown*/);
extern int resist(/*unknown*/);
extern void weffects(/*unknown*/);
extern void zapnodir(/*unknown*/);
extern int zappable(/*unknown*/);
extern int zapyourself(/*unknown*/);

extern xchar xdnstair, ydnstair, xupstair, yupstair; /* stairs up and down. */

extern xchar dlevel;

#ifdef SPELLS
#include "spell.h"
extern struct spell spl_book[]; /* sized in decl.c */
#endif

extern int occtime;
extern char *occtxt; /* defined when occupation != NULL */

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

extern char *traps[];
extern char vowels[];

#include "you.h"

extern struct you u;

extern xchar curx, cury; /* cursor location on screen */

extern xchar seehx, seelx, seehy, seely; /* where to see*/
extern char *save_cm, *killer;

extern xchar dlevel, maxdlevel; /* dungeon level */

extern long moves;

extern int multi;

extern char lock[];
