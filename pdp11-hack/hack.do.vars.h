/*
 * Hack.do.vars.h
 */

void	dodrop(),    dodrink(),    doread(),
	dosearch(),  armwear(),   dowearring(), doremarm(),
	doremring(), dopay(),     save(),
	ddoinv(),    dozap(),     doset(),
	dowhatis(),  doeat(),
	done1(),
	doshow(),    doreprint(),  gemsdrop();

#ifdef DEBUG
int debug();
#endif /* DEBUG */

static FUNCTIONS list[] = {
	'\020', doredraw, 	 /* was  '\014' */
	'A', doreprint,	 /* Michiel: Repeat last message printed */
#ifdef DEBUG
	'D', debug,
#endif /* DEBUG */
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
