//  #========================================#
//  | Global Utility - Value - Version 1.2.0 |
//  |                                        |
//  | Author: Marco Vagnoni                  |
//  | Date:   March     2015 (v1.0.0)        |
//  |         March     2016 (v1.1.0)        |
//  |         March     2017 (v1.1.1)        |
//  |         October   2017 (v1.1.2)        |
//  |         July      2018 (v1.2.0)        |
//  #========================================#

#ifndef GUTY_VALUE_H
#define GUTY_VALUE_H

#include <netdb.h>    // gethostbyname (), getservbyname ()

#define GT_BYTE      0x00
#define GT_SHORT     0x01
#define GT_INT       0x02
#define GT_LONG      0x03
#define GT_CHAR      0x04
#define GT_STRING    0x05
#define GT_HOST      0x06
#define GT_PORT      0x07
#define GT_TIME      0x08
#define GT_FILE      0x09
#define GT_MAX       0x0F

#define GT_ARRAY     0x10
#define GT_FLAG      0x20
#define GT_NEG       0x40

#define GO_APPEND    0x0100
#define GO_REQ       0x0200
#define GO_LIST      0x0400
#define GO_UDP       0x0800
#define GO_WRITE     0x1000
#define GO_DIRECT    0x2000

#define GO_CHKMIN    0x010000    // = GA_MIN    (guty_abort.h)
#define GO_CHKMAX    0x020000    // = GA_MAX
#define GO_CHKLIM    0x030000    // = GA_LIMITS
#define GO_NEXTCOL   0x040000
#define GO_HELPCOL   0x080000
#define GO_NO_FORMAT 0x100000
#define GO_NO_VALCOL 0x200000

#define GO_GIVEN     0x80000000
#define GO_NOT_GIVEN 0x7FFFFFFF

class guty_value
{
    static hostent *hep;
    static servent *sep;

public:

    static void cleanup (void);
    static int  get     (const char *str, void *var1, void *var2, void *var3, long  min, long max,
                         int optype, bool copy_value, const char **emsg);
};

#endif    // GUTY_VALUE_H
