/* panic.h */

#ifndef PANIC_H
#define PANIC_H

__attribute__((format(printf, 1, 2)))
extern void panic(const char *str, ...);

#endif
