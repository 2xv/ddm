//  #========================================#
//  | Global Utility - Param - Version 1.2.0 |
//  |                                        |
//  | Author: Marco Vagnoni                  |
//  | Date:   March     2015 (v1.0.0)        |
//  |         March     2016 (v1.1.0)        |
//  |         March     2017 (v1.1.1)        |
//  |         October   2017 (v1.1.2)        |
//  |         July      2018 (v1.2.0)        |
//  #========================================#

#ifndef GUTY_PARAM_H
#define GUTY_PARAM_H

#include "guty_value.h"

class guty_param
{
    static const char *caller_name;

    static void help_maker     (guty_param *prm_tab);
    static void help_formatter (guty_param *prm_tab, char **val_tab, int len_tab [4][4], int phase);

public:

    const char    *id,    *name;
    void          *var1,  *var2, *var3;
    long           min,    max;
    unsigned int   optype;
    const char    *info,  *help;
    int          (*fnct)(const char *caller_name, guty_param *prm_tab, const char **emsg, const char **eval);           // ret: <0 = same arg,
                                                                                                                        //       0 = next arg,
    static int get (int argc, char **argv, guty_param *prm_tab, int flags = 0, const char **caller_name_out = NULL);    //      >0 = aborted.
};

#endif    // GUTY_PARAM_H
