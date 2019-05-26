//  #========================================#
//  | Global Utility - Abort - Version 1.2.0 |
//  |                                        |
//  | Author: Marco Vagnoni                  |
//  | Date:   March     2015 (v1.0.0)        |
//  |         March     2016 (v1.1.0)        |
//  |         March     2017 (v1.1.1)        |
//  |         October   2017 (v1.1.2)        |
//  |         July      2018 (v1.2.0)        |
//  #========================================#

#ifndef GUTY_ABORT_H
#define GUTY_ABORT_H

#define GA_ERROR   0x01
#define GA_MSG     0x02
#define GA_VALUE   0x04
#define GA_KEY     0x08
#define GA_LINE    0x10
#define GA_NO_EXIT 0x20

#define GA_MIN     0x010000    // = GO_CHKMIN    (guty_value.h)
#define GA_MAX     0x020000    // = GO_CHKMAX
#define GA_LIMITS  0x030000    // = GO_CHKLIM

int guty_abort (const char *caller_name, int flags, ...);

#endif    // GUTY_ABORT_H
