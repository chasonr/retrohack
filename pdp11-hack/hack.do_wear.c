/*
 * Hack.do_wear.c
 */

#include "hack.h"

#define CURSED		1
#define NOTCURSED	0

off_msg (otmp)
register        OBJECT otmp;
{
	doname (otmp, buf);
	pline ("You were wearing %s.", buf);
}

doremarm () {
	register        OBJECT oldarm = uarm;

	nomove ();
	if (!uarm) {
		pline ("Not wearing any armor.");
		return;
	}
	flags.move = 1;
	if (cursed (uarm))
		return;
	nomul (-armors[uarm -> otyp].delay);
	u.uac += uarm -> spe;
	uarm = uarm2;
	uarm2 = 0;
	off_msg (oldarm);
	flags.dac = 1;
}

doremring () {

	nomove ();
	if (!uleft && !uright) {
		pline ("Not wearing any ring.");
		return;
	}
	if (!uleft)
		dorr (&uright);
	else if (!uright)
		dorr (&uleft);
	else
		for (;;) {
			pline ("What ring, Right or Left? ");
			flush ();
			*buf = getchar ();
			flags.topl = 0;
			if (*buf == '\n' || *buf == '\033')
				return;
			if (*buf == 'l' || *buf == 'L') {
				dorr (&uleft);
				return;
			}
			if (*buf == 'r' || *buf == 'R') {
				dorr (&uright);
				return;
			}
			if (*buf == '?')
				doinv ("=", 0);
		}
}

dorr (ring)
register        OBJECT * ring;
{
	register        OBJECT otmp = *ring;

	if (cursed (otmp))
		return;
	*ring = 0;
	doring (otmp, OFF);
	off_msg (otmp);
}

cursed (otmp)
register        OBJECT otmp;
{
	if (otmp -> cursed) {
		pline ("You can't. It appears to be cursed.");
		return (CURSED);
	}
	return (NOTCURSED);
}

armwear () {
	register        OBJECT otmp;

	otmp = getobj ("[", "wear");
	if (!otmp) {
		nomove ();
		return;
	}
	if (uarm) {
		if (otmp -> otyp == A_ELVEN_CLOAK && !uarm2)
			uarm2 = uarm;
		else {
			pline ("You are already wearing some armor.");
			flags.move = 0;
			return;
		}
	}
	uarm = otmp;
	nomul (-armors[otmp -> otyp].delay);
	uarm -> known = 1;
	u.uac -= uarm -> spe;
	flags.dac = 1;
}

dowearring () {
	register        OBJECT otmp;

	otmp = getobj ("=", "wear");
	if (!otmp) {
R: 
		nomove ();
		return;
	}
	if (uleft && uright) {
		pline ("There are no more fingers to fill.");
		goto R;
	}
	if (otmp == uleft || otmp == uright) {
		pline ("You are already wearing that.");
		goto R;
	}
	if (uleft)
		uright = otmp;
	else if (uright)
		uleft = otmp;
	else
		while (!uright && !uleft) {
			pline ("What finger, Right or Left? ");
			flush ();
			*buf = getchar ();
			flags.topl = 0;
			if (*buf == 'l' || *buf == 'L')
				uleft = otmp;
			else if (*buf == 'r' || *buf == 'R')
				uright = otmp;
		}
	doring (otmp, ON);
	prinv (otmp);
}

doring (obj, eff)
register        OBJECT obj;
register        eff;
{
	register        tmp;

	if (!obj)
		return;
	tmp = obj -> spe;
	if (eff == OFF)
		tmp = -tmp;
	if (uleft && uright && uleft == uright &&
			obj -> otyp <= R_GAIN_STR)
		return;
 /* If wearing the same ring sometimes the flag must remain actif. */
	switch (obj -> otyp) {
		case R_TELE: 
			u.utel = eff;
		case R_ADORNMENT: 
			break;
		case R_REGEN: 
			u.uregen = eff;
			break;
		case R_SEARCH: 
			u.usearch = eff;
			break;
		case R_SEE_INV: 
			u.ucinvis = eff;
			break;
		case R_STEALTH: 
			u.ustelth = eff;
			break;
		case R_FLOAT: 
			u.ufloat = eff;
			break;
		case R_POISON_RES: 
			u.upres = eff;
			break;
		case R_AGGRAV_MON: 
			u.uagmon = eff;
			break;
		case R_HUNGER: 
			u.ufeed = eff;
			break;
		case R_FIRE_RES: 
			u.ufireres = eff;
			break;
		case R_COLD_RES: 
			u.ucoldres = eff;
			break;
		case R_SHAPE: 
			u.ucham = eff;
			if (eff != OFF)
				rescham ();
			break;
		case R_GAIN_STR: 
			u.ustr += tmp;
			u.ustrmax += tmp;
			flags.dstr = 1;
			break;
		case R_DAM_INC: 
			u.udaminc += tmp;
			break;
		case R_PROTECTION: 
			u.uac -= tmp;
			flags.dac = 1;
			break;
		default: 
			pline ("Bad(%d)ring", obj -> otyp);
			impossible ();
	}
}
