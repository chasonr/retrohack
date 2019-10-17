/* vpline.h */
/* This is separate so everything doesn't need stdarg.h */

#ifndef VPLINE_H
#define VPLINE_H

#include <stdarg.h>

__attribute__((format(printf, 1, 0)))
extern void vpline(const char *line, va_list args);

#endif
