/*	SCCS Id: @(#)rnd.c	2.3	87/12/12
 */
#include "config.h"
/* rand() is either random() or lrand48() - see config.h. */
#ifdef UNIX
#define RND(x) (rand() % (x))
#else
/* Good luck: the bottom order bits are cyclic. */
#define RND(x) ((rand() >> 3) % (x))
#endif

int
rn1(x, y) /* y <= rn1(x,y) < (y+x) */
register int x, y;
{
    return (RND(x) + y);
}

int
rn2(x) /* 0 <= rn2(x) < x */
register int x;
{
    return (RND(x));
}

int
rnd(x) /* 1 <= rnd(x) <= x */
register int x;
{
    return (RND(x) + 1);
}

int
d(n, x) /* n <= d(n,x) <= (n*x) */
register int n, x;
{
    register int tmp = n;

    while (n--)
        tmp += RND(x);
    return (tmp);
}

int
rne(x) /* by stewr 870807 */
register int x;
{
    register int tmp = 1;
    while (!rn2(x))
        tmp++;
    return (tmp);
}

int
rnz(i)
int i;
{
    register long x = i;
    register long tmp = 1000;
    tmp += rn2(1000);
    tmp *= rne(4);
    if (rn2(2)) {
        x *= tmp;
        x /= 1000;
    } else {
        x *= 1000;
        x /= tmp;
    }
    return ((int) x);
}
