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

#define _XOPEN_SOURCE

#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <new>
#include <arpa/inet.h>    // htons (), inet_addr (), inet_ntoa (), ntohs ()
#include <netinet/in.h>   // in_addr
#include "guty_value.h"

hostent   *guty_value::hep;
servent   *guty_value::sep;

int guty_value::get (const char *str, void *var1, void *var2, void *var3, long min, long max, int optype, bool copy_value, const char **emsg)
{
    in_addr     addr;
    char       *endp;
    bool        f;
    int         len, type;
    long        num;
    const char *opt;
    tm          tm1;
    time_t      t;
    void       *vl = NULL, *vs = NULL;

    if ((type = optype & GT_MAX) <= GT_LONG)    // Gestione Numeri
    {
        errno = 0;
        num   = strtol (str, &endp, 10);
        if (!errno && (*endp || ((optype & GO_CHKMIN) && num < min) || ((optype & GO_CHKMAX) && num > max))) errno = EINVAL;

        if (errno)
        {
           *emsg = "Invalid value";
            return errno;
        }
    }
    else if (type > GT_CHAR) len = strlen (str);    // Gestione Stringhe, Hosts, Porte, Time e Files

    switch (type)
    {
        case GT_BYTE:  *((char  *) var1) =  num;  break;
        case GT_SHORT: *((short *) var1) =  num;  break;
        case GT_INT:   *((int   *) var1) =  num;  break;
        case GT_LONG:  *((long  *) var1) =  num;  break;
        case GT_CHAR:  *((char  *) var1) = *str;  break;

        case GT_STRING:

            if (((optype & GO_CHKMIN) && len < min) || ((optype & GO_CHKMAX) && len > max))
            {
               *emsg = "Invalid string-size";
                return  EINVAL;
            }

            vs = var1;    // String
            vl = var2;    // Length
            break;

        case GT_HOST:

            if ((addr.s_addr = inet_addr (str)) == -1 && (hep = gethostbyname (str)))
                 addr.s_addr = *((in_addr_t *) hep->h_addr_list [0]);

            if (addr.s_addr == -1)
            {
               *emsg = "Invalid host-name or ip-address";
                return  EFAULT;
            }

           *((in_addr_t *) var1) = addr.s_addr;    // IPAddr
            vs = var2;                             // String
            vl = var3;                             // Length
            break;

        case GT_PORT:

            errno = 0;
            num   = strtol (str, &endp, 10);

            if ((*endp || errno))
            {
                if ((optype & GO_UDP)) opt = "udp";
                else                   opt = "tcp";

                if (!(sep = getservbyname (str, opt))) num = -1;
                else num = ntohs (sep->s_port);
            }

            if (num < 0 || num > 0xFFFF || ((optype & GO_CHKMIN) && num < min) || ((optype & GO_CHKMAX) && num > max))
            {
               *emsg = "Invalid service-name or port-number";
                return  EINVAL;
            }

           *((in_port_t *) var1) = num;    // IPPort
            vs = var2;                     // String
            vl = var3;                     // Length
            break;

        case GT_TIME:

            f = false;
            t = time (NULL);
            localtime_r (&t, &tm1);

            tm1.tm_isdst = -1;
            for (; *str == ' '; str++);

            if ((endp = strptime (str, "%H:%M:%S", &tm1)))
            {
                for (; *endp == ' '; endp++);
                if (*endp && !strptime (endp, "%Y-%m-%d", &tm1)) f = true;
            }
            else
            {
                if ((endp = strptime (str, "%Y-%m-%d", &tm1)))
                {
                    for (; *endp == ' '; endp++);

                    if (*endp == 0)
                    {
                        tm1.tm_hour = 0;
                        tm1.tm_min  = 0;
                        tm1.tm_sec  = 0;
                    }
                    else if (!strptime (endp, "%H:%M:%S", &tm1)) f = true;
                }
                else f = true;
            }

            if (f)
            {
               *emsg = "Invalid date-time";
                return  EINVAL;
            }

           *((time_t *) var1) = mktime (&tm1);    // Time
            vs = var2;                            // String
            vl = var3;                            // Length
            break;

        case GT_FILE:

            if ((optype & GO_APPEND))    opt = "a";
            else
                if ((optype & GO_WRITE)) opt = "w";
                else                     opt = "r";

            if (!(*((FILE **) var1) = fopen (str, opt))) return errno;    // File
            vs = var2;                                                    // String
            vl = var3;                                                    // Length
            break;

        default:;
    }

    f = optype & GT_ARRAY;

    if (vs && (vs == var1 || !f))                               // String
        if ((optype & GO_DIRECT)) memcpy (vs, str, len + 1);
        else
            if (!copy_value) *((const char **) vs) = str;
            else
            {
                if (!(*((char **) vs) = new (std::nothrow) char [len + 1]))
                {
                   *emsg = str;
                    return ENOMEM;
                }

                memcpy (*((char **) vs), str, len + 1);
            }

    if (vl && !f) *((int *) vl) = len;    // Length
    return 0;
}

    void guty_value::cleanup (void)
    {
        if (hep) {endhostent (); hep = NULL;}
        if (sep) {endservent (); sep = NULL;}
    }
