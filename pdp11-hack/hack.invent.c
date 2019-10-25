/*
 * Hack.invent.c
 */

#include	"hack.h"

#define NOT_AT	0
#define NO_OBJ	0

extern  WORMSEGMENT wsegs[32];

extern  OBJECT yourinvent0;

OBJECT addinv (obj)
register        OBJECT obj;
{
	register        OBJECT otmp;

	for (otmp = invent; otmp; otmp = otmp -> nobj) {
		if (otmp -> otyp == obj -> otyp && otmp -> olet == obj -> olet
				&& !obj -> unpaid && !otmp -> unpaid &&
				((obj -> otyp < F_COOKIE &&
						obj -> olet == ')' &&
						obj -> quan + otmp -> quan < 32 &&
						obj -> spe == otmp -> spe) ||
					index ("%?!*", otmp -> olet))) {
			otmp -> quan += obj -> quan;
			ofree (obj);
			return otmp;
		}
		if (!otmp -> nobj) {
			otmp -> nobj = obj;
			obj -> nobj = 0;
			return obj;
		}
	}
	invent = obj;
	obj -> nobj = 0;
	return obj;
}

useup (obj)
register        OBJECT obj;
{
	register        OBJECT otmp;

	if (obj -> quan > 1) {
		obj -> quan--;
		return;
	}
	if (obj == invent)
		invent = invent -> nobj;
	else {
		for (otmp = invent; otmp -> nobj != obj;
				otmp = otmp -> nobj);
		otmp -> nobj = obj -> nobj;
	}
	if (!onbill (obj))
		ofree (obj);
}

delobj (obj)
register        OBJECT obj;
{
	freeobj (obj);
	ofree (obj);
}

ofree (obj)
register        OBJECT obj;
{
	if (obj > yourinvent0)
		free (obj);
}

/*  Unlink obj from chain starting with fobj  */

freeobj (obj)
register        OBJECT obj;
{
	register        OBJECT otmp;

	if (obj == fobj)
		fobj = fobj -> nobj;
	else {
		for (otmp = fobj; otmp -> nobj != obj; otmp = otmp -> nobj)
			if (!otmp)
				panic (CORE, "Try to free non-existing object");
		otmp -> nobj = obj -> nobj;
	}
}

deltrap (trap)
register        GOLD_TRAP trap;
{
	register        GOLD_TRAP gtmp;

	if (trap == ftrap)
		ftrap = ftrap -> ngen;
	else {
		for (gtmp = ftrap; gtmp -> ngen != trap;
				gtmp = gtmp -> ngen);
		gtmp -> ngen = trap -> ngen;
	}
	free (trap);
}

MONSTER m_at (x, y)
register        x, y;
{
	register        MONSTER mtmp;
	register        WORMSEGMENT wtmp;

	for (mtmp = fmon; mtmp; mtmp = mtmp -> nmon) {
		if (mtmp -> mx == x && mtmp -> my == y)
			return (mtmp);
		if (mtmp -> wormno)
			for (wtmp = wsegs[mtmp -> wormno]; wtmp;
					wtmp = wtmp -> nseg)
				if (wtmp -> wx == x && wtmp -> wy == y)
					return (mtmp);
	}
	return (NOT_AT);
}

OBJECT o_at (x, y)
register        x, y;
{
	register        OBJECT otmp;

	for (otmp = fobj; otmp; otmp = otmp -> nobj)
		if (otmp -> ox == x && otmp -> oy == y)
			return (otmp);
	return (NOT_AT);
}

GOLD_TRAP g_at (x, y, ptr)
register        x, y;
register        GOLD_TRAP ptr;
{
	while (ptr) {
		if (ptr -> gx == x && ptr -> gy == y)
			return (ptr);
		ptr = ptr -> ngen;
	}
	return (NOT_AT);
}

OBJECT getobj (let, word)
register char  *let, *word;
{
	register        OBJECT otmp;
	register char   ilet, ilet1, ilet2;
	char    buffer[BUFSZ], allowall = 0;
	register        foo = 0, foo2;

	if (*let == '#') {
		let++;
		allowall++;
	}
	ilet = 'a';
	for (otmp = invent; otmp; otmp = otmp -> nobj) {
		if (!let || index (let, otmp -> olet))
			buffer[foo++] = ilet;
		if (ilet == 'z')
			ilet = 'A';
		else
			ilet++;
	}
	buffer[foo] = 0;
	if (foo > 5) {		/* Compactify string */
		foo = foo2 = 1;
		ilet2 = buffer[0];
		ilet1 = buffer[1];
		while (ilet = buffer[++foo2] = buffer[++foo]) {
			if (ilet == ilet1 + 1) {
				if (ilet1 == ilet2 + 1)
					buffer[foo2 - 1] = ilet1 = '-';
				else if (ilet2 == '-') {
					buffer[--foo2] = ++ilet1;
					continue;
				}
			}
			ilet2 = ilet1;
			ilet1 = ilet;
		}
	}
	if (!foo && !allowall) {
		pline ("You don't have anything to %s.", word);
		return (NO_OBJ);
	}
	for (;;) {
		pline ((foo) ? "What do you want to %s [%s or ?]? " :
				"What do you want to %s? ", word, buffer);
		flags.topl = 0;
		flush ();
		ilet = getchar ();
		if (ilet == '\33' || ilet == ' ' || ilet == '\n')
			if (strcmp (word, "identify"))
				return (NO_OBJ);
			else
				continue;/* sukkel */
		if (ilet == '?')
			doinv (foo ? let : 0, 0);
		else {
			if (ilet >= 'A' && ilet <= 'Z')
				ilet += 26 - 'A';
			else
				ilet -= 'a';
			for (otmp = invent; otmp && ilet; ilet--,
					otmp = otmp -> nobj);
			if (!otmp) {
				pline ("You don't have that object.");
				continue;
			}
			break;
		}
	}
	if (!allowall && let && !index (let, otmp -> olet)) {
		pline ("That is a silly thing to %s.", word);
		return (NO_OBJ);
	}
	return (otmp);
}

prinv (obj)
register        OBJECT obj;
{
	register        OBJECT otmp;
	register char   ilet = 'a';

	for (otmp = invent; otmp != obj; otmp = otmp -> nobj)
		if (++ilet > 'z')
			ilet = 'A';
	prname (obj, ilet, 1);
}

prname (obj, let, onelin)
register        OBJECT obj;
register char   let;
{
	char    li[BUFSZ];

	doname (obj, buf);
	sprintf (li, "%c - %s.", let, buf);
	if (onelin)
		pline (li);
	else
		printf ("%s\n", li);
}

ddoinv () {
	nomove ();
	if (!invent)
		pline ("You are empty handed.");
	else
		doinv (0, 1);
}

/*
 * Page inventory done by Fred
 *
 */

doinv (str, opt)
register char  *str;
int     opt;
{
	register        OBJECT otmp;
	register char   ilet = 'a';
	register int    count = 1;
	int     ct = 0;

	if (!flags.oneline)
		for (otmp = invent; otmp; otmp = otmp -> nobj)
			if (!str || index (str, otmp -> olet))
				ct++;
	if (ct > 1)
		cls ();
	for (otmp = invent; otmp; otmp = otmp -> nobj) {
		if (!str || index (str, otmp -> olet)) {
			prname (otmp, ilet, ct <= 1);
			count++;
		}
		if (++ilet > 'z')
			ilet = 'A';
		if (!(count % 22) && otmp -> nobj) {
			getret ();
			cls ();	/* M. F. Page listing */
		}
	}
	if (!str && opt)
		doinvbill ();
	if (ct > 1) {
		getret ();
		docrt ();
	}
}
