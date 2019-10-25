/*
 * Hack.do.vars.h
 */

int 	doredraw(),  dodrop(),    dodrink(),    doread(),
	dosearch(),  armwear(),   dowearring(), doremarm(),
	doremring(), dopay(),     save(),       dowield(),
	ddoinv(),    dozap(),     m_call(),     doset(),
	dowhatis(),  dohelp(),    doeat(),      ddoup(),
	ddodown(),   done1(),     donull(),     dothrow(),
	doshow(),    dosh(),	  doreprint(),  gemsdrop();

#ifdef DEBUG
int debug();
#endif DEBUG

FUNCTIONS list[] = {
	'\020', doredraw, 	 /* was  '\014' */
	'A', doreprint,	 /* Michiel: Repeat last message printed */
#ifdef DEBUG
	'D', debug,
#endif DEBUG
	'P', dowearring,
	'Q', done1,
	'R', doremring,
	'G', gemsdrop,
	'S', save,
	'T', doremarm,
	'W', armwear,
	'a', doshow,	/* Michiel: show everything you're wearing */
	'c', m_call,
	'd', dodrop,
	'e', doeat,
	'i', ddoinv,
	'o', doset,
	'p', dopay,
	'q', dodrink,
	'r', doread,
	's', dosearch,
	't', dothrow,
	'w', dowield,
	'z', dozap,
	'<', ddoup,
	'>', ddodown,
	'/', dowhatis,
	'?', dohelp,
	'!', dosh,
	'\n', donull,
	' ', donull,
	0, 0
};
