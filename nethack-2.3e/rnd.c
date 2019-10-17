/*	SCCS Id: @(#)rnd.c	2.3	87/12/12
 */
#include <stdlib.h>
#include "config.h"
/* rand() is either random() or lrand48() - see config.h. */
#ifdef UNIX
#define RND(x) (rand() % (x))
#else
/* Good luck: the bottom order bits are cyclic. */
#define RND(x) ((rand() >> 3) % (x))
#endif

int
rn1(int x, int y) /* y <= rn1(x,y) < (y+x) */
{
    return (RND(x) + y);
}

int
rn2(int x) /* 0 <= rn2(x) < x */
{
    return (RND(x));
}

int
rnd(int x) /* 1 <= rnd(x) <= x */
{
    return (RND(x) + 1);
}

int
d(int n, int x) /* n <= d(n,x) <= (n*x) */
{
    register int tmp = n;

    while (n--)
        tmp += RND(x);
    return (tmp);
}

int
rne(int x) /* by stewr 870807 */
{
    register int tmp = 1;
    while (!rn2(x))
        tmp++;
    return (tmp);
}

int
rnz(int i)
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
