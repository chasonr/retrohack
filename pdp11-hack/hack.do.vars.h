/*
 * Hack.do.vars.h
 */

static FUNCTIONS list[] = {
	{ '\020', doredraw }, 	 /* was  '\014' */
	{ 'A', doreprint },	 /* Michiel: Repeat last message printed */
#ifdef DEBUG
	{ 'D', debug },
#endif /* DEBUG */
	{ 'P', dowearring },
	{ 'Q', do_quit },
	{ 'R', doremring },
	{ 'G', gemsdrop },
	{ 'S', save },
	{ 'T', doremarm },
	{ 'W', armwear },
	{ 'a', doshow },	/* Michiel: show everything you're wearing */
	{ 'c', m_call },
	{ 'd', dodrop },
	{ 'e', doeat },
	{ 'i', ddoinv },
	{ 'o', doset },
	{ 'p', dopay },
	{ 'q', dodrink },
	{ 'r', doread },
	{ 's', dosearch },
	{ 't', dothrow },
	{ 'w', dowield },
	{ 'z', dozap },
	{ '<', ddoup },
	{ '>', ddodown },
	{ '/', dowhatis },
	{ '?', dohelp },
	{ '!', dosh },
	{ '\n', donull },
	{ ' ', donull },
	{ 0, 0 }
};
