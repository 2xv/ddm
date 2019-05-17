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

#include <cerrno>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "guty_abort.h"

int guty_abort (const char *caller_name, int flags, ...)
{
    char    *msg, *val;
    int      err,  lim;
    long     min,  max;
    va_list  vlst;

    va_start (vlst, flags);

    if ((flags & GA_ERROR)) err = va_arg (vlst, int);       // Error Number
    else                    err = errno;

    if ((flags & GA_MSG))   msg = va_arg (vlst, char *);    // Error Message
    else                    msg = NULL;

    if ((flags & GA_VALUE)) val = va_arg (vlst, char *);    // Value
    else                    val = NULL;

    if (!msg) msg = strerror (err);
    fprintf (stderr, "%s: %s.", caller_name, msg);

    if (val) fprintf (stderr, " %s", val);

    if ((flags & GA_KEY))
    {
        fprintf (stderr, " [%s", va_arg (vlst, char *));         // Key

        if (!(lim = flags & GA_LIMITS)) fputs ("].", stderr);    // Limits
        else
        {
            min = va_arg (vlst, long);    // Min
            max = va_arg (vlst, long);    // Max

                 if (lim == GA_LIMITS) fprintf (stderr, " (%ld..%ld)].", min, max);
            else if (lim == GA_MIN)    fprintf (stderr, " (>=%ld)].",    min);
            else                       fprintf (stderr, " (<=%ld)].",    max);
        }
    }
    else if (val) fputc ('.', stderr);

    if ((flags & GA_LINE)) fprintf (stderr, " Break at line #%u.\n", va_arg (vlst, int));    // Line
    else fputc ('\n', stderr);

    va_end (vlst);
    if ((flags & GA_NO_EXIT)) return err;
    exit (err);
}
