/*
 * Hack.do.misc.c
 */

/* Routines to do various user commands */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include "hack.h"

static void doredraw();
static void dohelp();
static void dosh();
static void dowield();
static void ddodown();
static void ddoup();
static void m_call();
static void donull();
static void dothrow();

#include "hack.do.vars.h"

 /* (MT) has 'do' structures and lists */

static OBJECT loseone ();

void    done1 ();

char    upxstairs[MAXLEVEL], upystairs[MAXLEVEL];

static void dostairs();

void
rhack (cmd)
register char  *cmd;
{
	register        FUNCTIONS * tlist = list;

	if (!cmd) {
		pline ("Rhack: 0");
		impossible ();
		return;
	}
	if (!*cmd || *cmd == -1)
		return;		/* Probably interrupt? */
	if (movecm (cmd)) {
		if (multi)
			flags.mv = 1;
		domove ();
		return;
	}
	if (movecm (lowc (cmd))) {
		flags.run = 1;
		multi = 80;
		flags.mv = 1;
		domove ();
		return;
	}
	if (*cmd == 'f' && movecm (cmd + 1)) {
		flags.run = 2;
		multi = 80;
		flags.mv = 1;
		domove ();
		return;
	}
	if (*cmd == 'F' && movecm (lowc (cmd + 1))) {
		flags.run = 3;
		multi = 80;
		flags.mv = 1;
		domove ();
		return;
	}
	while (tlist -> f_char) {
		if (*cmd == tlist -> f_char) {
			(*(tlist -> f_funct)) ();
			return;
		}
		tlist++;
	}
	pline ("Unknown command '%s'", cmd);
	nomove ();
}

static void
doredraw () {
	docrt ();
	nomove ();
}

/*
 * VARIABELE ARGUMENTS, but if not given the last argument will always
 * be a NULL.		(Michiel)
 */
/*VARARGS*/
void
hackexec (num, file, arg1, arg2, arg3, arg4, arg5, arg6)
register int    num;
register char  *file, *arg1;
char   *arg2, *arg3, *arg4, *arg5, *arg6;
{
	nomove ();
	switch (fork ()) {
		case -1: 
			pline ("Fork failed. Will try again.");
			hackexec (num, file, arg1, arg2, arg3, arg4,
					arg5, arg6);
			break;
		case 0: 
			if (num) {
				signal (SIGINT, SIG_DFL);
				setuid (getuid ());
				hackmode (OFF);
				cls ();
				flush ();
				if (num == 2)
					chdir (getenv ("HOME"));
			}
			else
				signal (SIGINT, SIG_IGN);
			execl (file, file, arg1, arg2, arg3, arg4, arg5, arg6);
			panic (NOCORE, "%s: cannot execute.", file);
			break;
		default: 
			signal (SIGINT, SIG_IGN);
			signal (SIGQUIT, SIG_IGN);
			wait (0);
			if (num) {
				hackmode (ON);
				docrt ();
			}
			signal (SIGINT, done1);
			signal (SIGQUIT, SIG_DFL);
			break;
	}
}

static void
dohelp () {
	hackexec (1, MORE, HELP, NULL);
}

static void
dosh () {
	register char  *str;

	if ((str = getenv ("SHELL")) != NULL)
		hackexec (2, str, NULL);
	else
		hackexec (2, "/bin/sh", "-i", NULL);
}

static void
dowield () {
	register        OBJECT wep;

	multi = 0;
	if (!(wep = getobj (")", "wield")))
		flags.move = 0;
	else if (uwep && uwep -> cursed)
		pline (WELDED, weapons[uwep -> otyp].wepnam);
	else {
		uwep = wep;
		if (uwep -> cursed)
			pline ("The %s welds itself to your hand!",
					weapons[uwep -> otyp].wepnam);
		else
			prinv (uwep);
	}
}

static void
ddodown () {
	dostairs ("down");
}

static void
ddoup () {
	dostairs ("up");
}

static void
dostairs (dir)
register char  *dir;
{
	if (u.ustuck ||
			(*dir == 'd' && (u.ux != xdnstair || u.uy != ydnstair)) ||
			(*dir == 'u' && (u.ux != xupstair || u.uy != yupstair))) {
		pline ("You can't go %s here", dir);
		nomove ();
		return;
	}
	keepdogs (1);
	unCoff (COFF, 1);
	dosavelev ();
	if (*dir == 'd')
		dodown ();
	else
		doup ();
	losedogs ();
	if (u.ucham)
		rescham ();
	setCon (CON);
	if (u.uhcursed)
		docurse ();
}

void
dosavelev () {
	register int    fd;

	glo (dlevel);
	fd = creat (lock, 0644);
	savelev (fd);
	close (fd);
}

static void
checklev (dir)			/* Michiel: Geen geknoei */
register char  *dir;
{
	if ((upxstairs[dlevel] != xupstair ||
				upystairs[dlevel] != yupstair) && !wizard) {
		clearlocks ();
		panic (NOCORE, "Way %s has been changed...", dir);
	}
}

void
dodown () {
	register int    fd;

	glo (++dlevel);
	if ((fd = open (lock, 0)) < 0)
		mklev ();
	else {
		if (maxdlevel < dlevel)
			mklev ();/* Bad file  */
		else
			getlev (fd);
		close (fd);
		checklev ("down");
	}
	if (maxdlevel < dlevel)
		maxdlevel = dlevel;/* Trapdoor/stairs */
	u.ux = xupstair;
	u.uy = yupstair;
	inshop ();
}

void
doup () {
	register int    fd;

	if (dlevel == 1)
		done (ESCAPED);
	glo (--dlevel);
	if ((fd = open (lock, 0)) < 0)
		panic (CORE, "Cannot open %s\n", lock);
	getlev (fd);
	close (fd);
	checklev ("up");
	u.ux = xdnstair;
	u.uy = ydnstair;
}

static void
m_call () {
	register        OBJECT obj;

	obj = getobj ("?!=/", "call");
	if (obj)
		docall (obj);
	flags.move = 0;
}

void
docall (obj)
register        OBJECT obj;
{
	register char  *str, **str1;

	pline ("Call it:");
	getlin (buf);
	flags.topl = 0;
	if (!*buf)
		return;
	str = alloc (strlen (buf) + 1) -> Val;
	strcpy (str, buf);
	switch (obj -> olet) {
		case '_': 
			free (str);
			return;
		case '?': 
			str1 = &scrcall[obj -> otyp];
			break;
		case '!': 
			str1 = &potcall[obj -> otyp];
			break;
		case '/': 
			str1 = &wandcall[obj -> otyp];
			break;
		case '=': 
			str1 = &ringcall[obj -> otyp];
			break;
		default: 
			pline ("What a weird(%c %d)thing to call", obj -> olet, obj -> otyp);

	}
	if (*str1)
		free (*str1);
	*str1 = str;
}

static void
donull () {
}

MONSTER bhit ();

static void
dothrow () {
	register        OBJECT obj;
	register        MONSTER monst;
	register int    tmp;
	char    x, y;

	obj = getobj ("#%)", "throw");
 /* One can also throw food */
	if (!obj || !getdir ()) {
		nomove ();
		return;		/* Sets dx and dy to direction */
	}
	if (obj == uarm || obj == uarm2 || obj == uleft ||
			obj == uright) {
		pline ("You can't throw something you are wearing");
		return;
	}
	if (obj == uwep && uwepcursed ())
		return;
	monst = bhit (dx, dy, 8);
	x = dx;
	y = dy;
	obj = loseone (obj);	/* Separate one out from list */
	if (monst) {
		if (obj -> olet == ')') {
			tmp = u.ulevel - 1 + monst -> data -> ac + abon ();
			if (obj -> otyp <= W_AMMUNITION) {
				if (!uwep || uwep -> otyp != obj -> otyp +
						11)
					tmp -= 4;
				else {
					if (uwep -> cursed)
						tmp -= uwep -> spe;
					else
						tmp += uwep -> spe;
				}
			}
			else {
				if (obj -> cursed)
					tmp -= obj -> spe;
				else
					tmp += obj -> spe;
			}
			if (tmp >= rnd (20)) {
				if (hmon (monst, obj)) {
					hit (weapons[obj -> otyp].wepnam,
							monst);
					cutworm (monst, x, y, obj -> otyp);
				}
				else
					monst = 0;
				if (obj -> otyp <= W_AMMUNITION &&
						rn2 (2)) {
					freeobj (obj);
					if (!onbill (obj))
						ofree (obj);
					return;
				}
			}
			else
				miss (weapons[obj -> otyp].wepnam, monst);
		}
		else {
			psee (IT1, x, y, UMISS, monst -> data -> mname, NULL);
			if (obj -> olet == '%' && monst -> data -> mlet == 'd')
				if (tamedog (monst, obj))
					return;
		}
/* Awake monster if sleeping */
		if (monst) {
			monst -> msleep = 0;
			if (monst == shopkeeper)
				setangry ();
		}
	}
	obj -> ox = x;
	obj -> oy = y;
	if (obj -> unpaid && inshproom (x, y))
		subfrombill (obj);
	if (!m_at (x, y))
		newsym (x, y);
}

/* Create a new object (at fobj) of multiplicity 1
				  remove obj from invent if necessary */
static OBJECT
loseone (obj)
register        OBJECT obj;
{
	register        OBJECT otmp;

	if (!strchr ("/=", obj -> olet) && obj -> quan > 1) {
		obj -> quan--;
		otmp = newobj ();
		*otmp = *obj;
		otmp -> quan = 1;
		otmp -> unpaid = 0;/* Obj is still on the bill */
		otmp -> nobj = fobj;
		fobj = otmp;
	}
	else {
		if (obj == invent)
			invent = invent -> nobj;
		else {
			for (otmp = invent; otmp -> nobj != obj;
					otmp = otmp -> nobj);
			otmp -> nobj = obj -> nobj;
		}
		obj -> nobj = fobj;
		fobj = obj;
	}
	return (fobj);
}

int
getdir () {
	pline ("What direction?");
	flush ();
	*buf = getchar ();
	flags.topl = 0;
	return (movecm (buf));
}

void
docurse () {
	register        MONSTER mtmp;

	for (mtmp = fmon; mtmp; mtmp = mtmp -> nmon) {
		mtmp -> msleep = 0;
		mtmp -> mtame = 0;
	}
	if (shopkeeper)
		shopkeeper -> angry = 1;
	if (vaultkeeper)
		vaultkeeper -> angry = 1;
}
