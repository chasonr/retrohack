/*
 * Hack.shk.c
 */

#include "hack.h"

#define BILLSZ  200
#define ONBILL    1
#define NOTONBILL 0
#define GDIST(x, y)     ((x - gx)*(x - gx) + (y - gy)*(y - gy) )

struct {
	OBJECT op;
unsigned        useup:                 1;
unsigned        bquan:                 5;
	unsigned        price;
}       bill[BILLSZ];

MONSTER shopkeeper;

struct permonst shk_pm = {
	"shopkeeper", '@', 10, 12, 0, 4, 8, 0
};

long    robbed = 0, total;

/*
 * shoproom = index in rooms; set by inshop()
 * shlevel  = last level we initialized shopkeeper
 */

char    billct = 0, shoproom, shlevel;
char   *shopnam[] = {
	"engagement ring", "walking cane", "antique weapon",
	"delicatessen", "second hand book", "liquor",
	"used armor", "assorted antiques"
};

COORDINATES shk, shd;
 /* Usual position shopkeeper;position shop door */

#define SHOP_NAME               shopnam[rooms[shoproom].rtype - 8]

shkdead () {
	shopkeeper = 0;
	rooms[shoproom].rtype = 0;
	setpaid ();
}

setpaid () {
	register        tmp;
	register        OBJECT obj;

	for (obj = invent; obj; obj = obj -> nobj)
		obj -> unpaid = 0;
	for (tmp = 0; tmp < billct; tmp++)
		if (bill[tmp].useup)
			ofree (bill[tmp].op);
	billct = 0;
}

addupbill () {			/* Delivers result in total */
	register        ct = billct;

	total = 0;
	while (ct--)
		total += bill[ct].price;
}

inshproom (x, y)
register        x, y;		/* a3 */
{
	return (inroom (x, y) == shoproom);
}

inshop () {
	register        tmp = inroom (u.ux, u.uy);

	if (tmp < 0 || rooms[tmp].rtype < 8) {
		u.uinshop = 0;
		if (billct) {
			pline ("Somehow you escaped the shop without paying!");
			addupbill ();
			pline ("You stole for a total worth of %U zorkmids.", total);
			robbed += total;
			setpaid ();
		}
	}
	else {
		shoproom = tmp;
		if (shlevel != dlevel)
			shopinit ();
		if (!u.uinshop) {
			pline ("%s to Dirk's %s shop!",
					(shopkeeper -> angry) ?
					"You're not that welcome" : "Welcome",
					SHOP_NAME);
			++u.uinshop;
		}
	}
	return (u.uinshop);
}

/* Called by useup and dothrow only */
onbill (obj)
register        OBJECT obj;
{
	register        tmp;

	for (tmp = 0; tmp < billct; tmp++)
		if (bill[tmp].op == obj) {
			bill[tmp].useup = 1;
			obj -> unpaid = 0;/* only for doinvbill */
			return (ONBILL);
		}
	return (NOTONBILL);
}

dopay () {
	register unsigned       tmp;
	char    buffer[BUFSZ];

	multi = 0;
	if (!inshop ()) {
		flags.move = 0;
		pline ("You are not in a shop.");
		return;
	}

	if (!shopkeeper || !inshproom (shopkeeper -> mx,
				shopkeeper -> my)) {
		pline ("There is nobody here to receive your payment.");
		return;
	}
	if (!billct) {
		pline ("You do not owe the shopkeeper anything.");
		if (!u.ugold) {
			pline ("Moreover, you have no money.");
			return;
		}
		if (robbed) {
			pline ("But since the shop has been robbed recently");
			pline ("You %srepay the shopkeeper's expenses.", (u.ugold < robbed) ?
					"partially " : "");
			u.ugold -= robbed;
			if (u.ugold < 0)
				u.ugold = 0;
			flags.dgold = 1;
			robbed = 0;
			return;
		}
		if (shopkeeper -> angry) {
			pline ("But in order to appease the angry shopkeeper,");
			if (u.ugold >= 1000L) {
				tmp = 1000;
				pline ("You give him 1000 gold pieces.");
			}
			else {
				tmp = (int) u.ugold;
				pline ("You give him all your money.");
			}
			u.ugold -= (long) tmp;
			flags.dgold = 1;
			if (rn2 (3)) {
				pline ("The shopkeeper calms down.");
				shopkeeper -> angry = 0;
			}
			else
				pline ("The shopkeeper is as angry as ever.");
		}
		return;
	}

	while (billct) {
		billct--;
		bill[billct].op -> unpaid = 0;
		doname (bill[billct].op, buffer);
		tmp = bill[billct].price;
		if (shopkeeper -> angry)
			tmp += tmp / 3;
		if (u.ugold < tmp) {
			++bill[billct].op -> unpaid;
			billct++;
			pline ("You don't have gold enough to pay %s.",
					buffer);
			return;
		}
		u.ugold -= tmp;
		flags.dgold = 1;
		pline ("You bought %s for %d gold pieces.", buffer, tmp);
		if (bill[billct].useup)
			ofree (bill[billct].op);
	}
	pline ("Thank you for shopping in Dirk's %s store!",
			SHOP_NAME);
	shopkeeper -> angry = 0;
}

paybill () {			/* Called after dying (or quitting) with
				   nonempty bill */
	if (shopkeeper) {
		addupbill ();
		if (total > u.ugold) {
			u.ugold = 0;
			if (invent)
				pline ("The shopkeeper comes and takes all your possessions.");
		}
		else {
			u.ugold -= total;
			pline ("The shopkeeper comes and takes the %D zorkmids you owed him.",
					total);
		}
	}
	more ();
}

addtobill (obj)
register        OBJECT obj;
{
	if (!inshop ())
		return;
	if (billct == BILLSZ) {
		pline ("You got that for free!");
		return;
	}
	bill[billct].op = obj;
	bill[billct].bquan = obj -> quan;
	bill[billct].useup = 0;
	bill[billct++].price = getprice (obj);
	obj -> unpaid = 1;
}

subfrombill (obj)
register        OBJECT obj;
{
	register        tmp;
	register        OBJECT otmp;

	if (!inshop ())
		return;
	for (tmp = 0; tmp < billct; tmp++)
		if (bill[tmp].op == obj) {
			obj -> unpaid = 0;
			if (bill[tmp].bquan != obj -> quan) {
				bill[tmp].op = otmp = newobj ();
				*otmp = *obj;
				otmp -> quan = (bill[tmp].bquan -=
						obj -> quan);
				bill[tmp].price *= otmp -> quan;
				bill[tmp].price /= (otmp -> quan +
						obj -> quan);
				bill[tmp].useup = 1;
				return;
			}
			billct--;
			bill[tmp] = bill[billct];
			return;
		}
/* I dropped something of my own, wanting to sell it */
	if (shopkeeper -> msleep || shopkeeper -> mfroz ||
			!inshproom (shopkeeper -> mx, shopkeeper -> my) ||
			robbed || u.ux == shk.x && u.uy == shk.y ||
			u.ux == shd.x && u.uy == shd.y)
		return;
	tmp = getprice (obj);
	if (shopkeeper -> angry) {
		tmp /= 3;
		shopkeeper -> angry = 0;
	}
	else
		tmp >>= 1;
	if (tmp < 2)
		tmp = 2;
	u.ugold += tmp;
	flags.dgold = 1;
	doname (obj, buf);
	pline ("You sold %s and got %d gold pieces.", buf, tmp);
}

doinvbill () {
	register unsigned       tmp, cnt = 0;

	for (tmp = 0; tmp < billct; tmp++)
		if (bill[tmp].useup) {
			if (!cnt && !flags.oneline) {
				getret ();
				cls ();
				printf ("\n\nUnpaid articles already used up:\n\n");
			}
			strcpy (buf, "* -  ");
			doname (bill[tmp].op, &buf[5]);
			for (cnt = 0; buf[cnt]; cnt++);
			while (cnt < 50)
				buf[cnt++] = ' ';
			sprintf (&buf[cnt], " %5d zorkmids",
					bill[tmp].price);
			if (flags.oneline)
				pline (buf);
			else
				printf ("%s\n", buf);
			if (!cnt % 20)
				getret ();
		}
}

getprice (obj)
register        OBJECT obj;
{
	register        tmp, ac;

	switch (obj -> olet) {
		case '"': 
			tmp = rnd (500);
			break;
		case '=': 
		case '/': 
			tmp = rnd (100);
			break;
		case '?': 
		case '!': 
			tmp = rnd (50);
			break;
		case '*': 
			tmp = rnd (6);
			break;
		case '%': 
			tmp = rnd (5 + 2000 / realhunger ());
			break;
		case '[': 
			ac = obj -> spe;
			tmp = (100 + ac * ac * rnd (10 + ac)) / 10;
			break;
		case ')': 
			if (obj -> otyp <= W_AMMUNITION)
				tmp = rnd (10);
			else if (obj -> otyp == W_LONG_SWORD || obj -> otyp ==
					W_TWOH_SWORD)
				tmp = rnd (150);
			else
				tmp = rnd (75);
			break;
		case '_': 
		default: 
			tmp = 1000;
	}
	return (10 * obj -> quan * tmp);
}

realhunger () {			/* not completely foolproof (??) */
	register        tmp = u.uhunger;
	register        OBJECT otmp = invent;

	while (otmp) {
		if (otmp -> olet == '%' && !otmp -> unpaid)
			tmp += foods[otmp -> otyp].nutrition;
		otmp = otmp -> nobj;
	}
	return tmp;
}

shopinit () {
	register        MKROOM * sroom = &rooms[shoproom];
	register        i, j, x, y;

	shlevel = dlevel;
	shd = doors[sroom -> fdoor];
	shk.x = shd.x;
	shk.y = shd.y;
	if (shk.x == sroom -> lx - 1)
		shk.x++;
	else if (shk.x == sroom -> hx + 1)
		shk.x--;
	else if (shk.y == sroom -> ly - 1)
		shk.y++;
	else if (shk.y == sroom -> hy + 1)
		shk.y--;
	else {
		sroom -> rtype = 0;
		pline ("Where is shopdoor?");
		impossible ();
		return;
	}
	if (shopkeeper)
		return;		/* We have been on this level before */
	if (makemon (&shk_pm))
		panic (CORE, "Cannot create shopkeeper?");
	shopkeeper = fmon;
	shopkeeper -> angry = u.uhcursed;
	if (m_at (shk.x, shk.y) || (shk.x == u.ux && shk.y == u.uy)) {
	/* (a3)`mnexto(shopkeeper)' is fout gaan */
		prl (shk.x, shk.y);
		for (i = -1; i < 2; i++)
			for (j = -1; j < 2; j++)
				if (levl[x = shk.x + i][y = shk.y + j].typ ==
						ROOM)
					if (!m_at (x, y)) {
						fmon -> mx = x;
						fmon -> my = y;
						pmon (shopkeeper);
						return;
					}
		fmon -> mx = shk.x;
		fmon -> my = shk.y;
		return;		/* bovenop een ander monster */
	}
	else {
		fmon -> mx = shk.x;
		fmon -> my = shk.y;
	}
	pmon (shopkeeper);
}

setangry () {
	if (shopkeeper -> data -> mlet == '@' && !shopkeeper -> angry) {
		pline ("The shopkeeper gets angry.");
		++shopkeeper -> angry;
	}
}

shk_move () {
	register        MONSTER mtmp;
	char    gx, gy, omx, omy, cnt, appr,
	        nix, niy, ddx, ddy, zx, zy, num;

	omx = shopkeeper -> mx;
	omy = shopkeeper -> my;
	if (!u.uinshop && inshproom (omx, omy) &&
			levl[omx][omy].typ == ROOM)
		return NOMOVE;
	if (shopkeeper -> angry && dist (omx, omy) < 3) {
		hitu (shk_pm.mhd, d (shk_pm.damn, shk_pm.damd),
				shk_pm.mname);
		return NOMOVE;
	}
	appr = 1;
	if ((shopkeeper -> angry) && !u.uinvis) {
		gx = u.ux;	/* Fred */
		gy = u.uy;
	}
	else if (shk.x == omx && shk.y == omy && !billct &&
				!shopkeeper -> angry &&
				(!robbed || (u.ux == shd.x && u.uy == shd.y)) &&
			dist (omx, omy) < 3) {
		appr = 0;
		gx = 0;
		gy = 0;
	}
	else {
		gx = shk.x;
		gy = shk.y;
	}
	cnt = 0;
	if (omx == gx && omy == gy)
		return NOMOVE;
	if (shopkeeper -> mconf)
		appr = 0;
	nix = omx;
	niy = omy;
	for (ddx = -1; ddx <= 1; ddx++)
		for (ddy = -1; ddy <= 1; ddy++) {
			zx = omx + ddx;
			zy = omy + ddy;
			num = levl[zx][zy].typ;/* a3 */
			if ((ddx || ddy) && (num == ROOM || !inshproom (omx, omy)
						&& (num == CORR || num == DOOR)) &&
					(shopkeeper -> mconf ||
						((zx != u.ux || zy != u.uy) && !m_at (zx, zy))) &&
					(!appr && !rn2 (++cnt) || appr && GDIST (zx, zy) <
						GDIST (nix, niy))) {
				nix = zx;
				niy = zy;
			}
		}
	if (nix != omx || niy != omy) {
		if (shopkeeper -> mconf && (mtmp = m_at (nix, niy))) {
			if (hitmm (shopkeeper, mtmp) == 1 && rn2 (3)
					&& hitmm (mtmp, shopkeeper) == DEAD)
				return DEAD;
			return NOMOVE;
		}
		shopkeeper -> mx = nix;
		shopkeeper -> my = niy;
/* The shopkeeper might have been turned into an X */
		levlsym (omx, omy, shopkeeper -> data -> mlet);
		pmon (shopkeeper);
		return MOVE;
	}
	return NOMOVE;
}
