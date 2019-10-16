/*	SCCS Id: @(#)func_tab.h	1.4	87/08/08 */
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* func_tab.h - version 1.0.2 */

struct func_tab {
    const char f_char;
    int (*f_funct)();
    const char *f_text;
};

struct ext_func_tab {
    const char *ef_txt, *ef_desc;
    int (*ef_funct)();
};

extern struct ext_func_tab extcmdlist[];
