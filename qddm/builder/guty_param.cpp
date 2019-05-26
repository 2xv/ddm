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

#include <cerrno>
#include <climits>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <new>
#include "guty_abort.h"
#include "guty_param.h"

#define ID_IDX    0
#define NAME_IDX  1
#define INFO_IDX  2
#define VALUE_IDX 3

const char *guty_param::caller_name;

int guty_param::get (int argc, char **argv, guty_param *prm_tab, int flags, const char **caller_name_out)
{
    const char *emsg = NULL, *eval = NULL, *prm_name, *prm_val;
    int         i, j, k, prm_idx, prm_ini = 0;

    flags &= GA_NO_EXIT;

    for (i = strlen (argv [0]) - 1; i > 0 && argv [0][i - 1] != '/'; i--);
    caller_name = *argv + i;

    if (caller_name_out) *caller_name_out = caller_name;
    for (i = 0; prm_tab [i].name; i++) prm_tab [i].optype &= GO_NOT_GIVEN;    // Reset GIVEN flag

    for (i = 1; i < argc; i++)
    {
        if (argv [i][0] != '-')    // Parametro Posizionale
        {
            for (prm_idx = prm_ini; prm_tab [prm_idx].name && *prm_tab [prm_idx].name == '-'; prm_idx++);    // Ricerca Argomento

            if (!prm_tab [prm_idx].name)
            {
                eval = argv [i];
                goto GUTY_PARAM_CLEANUP;
            }

            if (prm_tab [prm_idx].fnct) prm_ini = prm_idx;
            else prm_ini = prm_idx + 1;

            prm_name = prm_tab [prm_idx].name;
            prm_val  = argv    [i];
        }
        else    // Parametro Opzionale
        {
            if (argv [i][1] == '-') j = 2;
            else                    j = 1;

            if (!strcmp (argv [i] + j, "help") || (!argv [i][2] && argv [i][1] == '?'))    // -help --help -?
            {
                argc = i;
                help_maker (prm_tab);
                guty_value::cleanup ();

                if (flags) return  EINVAL;
                else       exit   (EINVAL);
            }

            for (prm_idx = 0; prm_tab [prm_idx].name && strcmp (prm_tab [prm_idx].name, argv [i]) &&
                            (!prm_tab [prm_idx].id   || strcmp (prm_tab [prm_idx].id,   argv [i])); prm_idx++);    // Ricerca Opzione

            if (!prm_tab [prm_idx].name)
            {
                flags |=  GA_MSG;
                eval   =  argv [i];
                emsg   = "Invalid option";
                goto GUTY_PARAM_CLEANUP;
            }

            if ((prm_tab [prm_idx].optype & GT_FLAG))                      // Gestione Flag con opzione senza valore seguente
                if ((prm_tab [prm_idx].optype & GT_NEG)) prm_val = "0";
                else                                     prm_val = "1";
            else
            {
                prm_name = argv [i++];
                prm_val  = argv [i];

                if (i >= argc)
                {
                    flags |=  GA_MSG;
                    eval   =  prm_name;
                    emsg   = "Missing option";
                    goto GUTY_PARAM_CLEANUP;
                }
            }
        }

        k = prm_tab [prm_idx].optype & GA_LIMITS;
        j = guty_value::get (prm_val, prm_tab [prm_idx].var1, prm_tab [prm_idx].var2, prm_tab [prm_idx].var3, 
                             prm_tab [prm_idx].min, prm_tab [prm_idx].max, prm_tab [prm_idx].optype, 0, &emsg);

        if (!j && prm_tab [prm_idx].fnct)
        {
            k = 0;
            j = prm_tab [prm_idx].fnct (caller_name, prm_tab + prm_idx, &emsg, &eval);
            if (!j && *prm_name != '-') prm_ini++;
        }

        if (j > 0)
        {
            guty_value::cleanup ();

            if (!eval) eval = argv [i];
            return guty_abort (caller_name, GA_ERROR | GA_MSG | GA_VALUE | GA_KEY | k | flags, j,
                               emsg, eval, prm_name, prm_tab [prm_idx].min, prm_tab [prm_idx].max);
        }

        prm_tab [prm_idx].optype |= GO_GIVEN;
    }

    for (i = 0; prm_tab [i].name; i++)
        if ((prm_tab [i].optype & (GO_REQ | GO_GIVEN)) == GO_REQ)
        {
            flags |=  GA_MSG;
            eval   =  prm_tab [i].name;
            emsg   = "Missing argument";
            goto GUTY_PARAM_CLEANUP;
        }

    return 0;

GUTY_PARAM_CLEANUP:

    guty_value::cleanup ();
    return guty_abort (caller_name, GA_ERROR | GA_VALUE | flags, EINVAL, emsg, eval);
}

void guty_param::help_maker (guty_param *prm_tab)
{
    bool          array;
    char          b [4096], *str, **val_tab;
    int           cnt_arg = 0, cnt_opt = 0, i, j, k, l, x, y,
                  len_tab [4][4] = {{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0}};
    long          num;
    tm            tm;
    unsigned int  optype;
    void         *var;

    for (i = 0, j = 0; prm_tab [i].name; i++)    // Calcolo larghezza massima colonne id
    {
        if ( *prm_tab [i].name == '-' && prm_tab [i].id && (l = strlen (prm_tab [i].id) + 1) > len_tab [ID_IDX][j]) len_tab [ID_IDX][j] = l;
        if (!(prm_tab [i].optype & GO_NEXTCOL) || prm_tab [i].help) j = 0;
        else j = (j + 1) & 3;
    }

    if (!i) return;

    for (i = 0, j = 0; prm_tab [i].name; i++)    // Calcolo larghezze massime colonne name ed info - Conteggio args e opts
    {
        optype = prm_tab [i].optype;

        if (*prm_tab [i].name == '-')
        {
            cnt_opt++;
            l = len_tab [ID_IDX][j];
        }
        else
        {
            cnt_arg++;
            if ((optype & GO_REQ)) l = 1;
            else l = 2;                      // +2 per i caratteri '[]' (= arg opzionale)
        }

        if ((l += strlen (prm_tab [i].name)) > len_tab [NAME_IDX][j]) len_tab [NAME_IDX][j] = l;
        if (prm_tab [i].info && (l = strlen (prm_tab [i].info)) > len_tab [INFO_IDX][j]) len_tab [INFO_IDX][j] = l;

        if (!(optype & GO_NEXTCOL) || prm_tab [i].help) j = 0;
        else j = (j + 1) & 3;
    }

    if (!(val_tab = new (std::nothrow) char * [i * sizeof (char *)] ())) guty_abort (caller_name, GA_ERROR | GA_VALUE, ENOMEM, "val_tab");

    for (i = 0, j = 0; prm_tab [i].name; i++)
    {
        optype = prm_tab [i].optype;
        k      = optype & GT_MAX;
        array  = optype & GT_ARRAY;

        if (array && prm_tab [i].var2 && prm_tab [i].var3)
        {
            b [0] = '[';
            l     =  1;
            var   =  prm_tab [i].var2;
            y     = *((int *) prm_tab [i].var3);
        }
        else
        {
            l   = 0;
            var = prm_tab [i].var1;
            y   = 1;
        }

        for (x = 0; x < y; x++)
        {
            if (x) b [l++] = ',';

            switch (k)
            {
                case GT_BYTE:  num = *((char  *)          var + x); break;
                case GT_PORT:  num = *((unsigned short *) var + x); break;
                case GT_SHORT: num = *((short *)          var + x); break;
                case GT_INT:   num = *((int   *)          var + x); break;
                case GT_LONG:  num = *((long  *)          var + x); break;

                case GT_CHAR:

                    b [l++] = *((char *) var + x);
                    continue;

                case GT_HOST:

                    num = ntohl (*((int *) var + x));
                    l  += snprintf (b + l, sizeof (b) - l, "%d.%d.%d.%d", (unsigned) num >> 24,       ((unsigned) num >> 16) & 0xFF,
                                                                         ((unsigned) num >> 8) & 0xFF, (unsigned) num        & 0xFF);
                    continue;

                case GT_TIME:

                    localtime_r ((time_t *) var + x, &tm);
                    l += strftime (b + l, sizeof (b) - l, "%Y-%m-%d %H:%M:%S", &tm);
                    continue;

                case GT_FILE:

                    if (!array && prm_tab [i].var2) var = prm_tab [i].var2;
                    else
                    {
                        l += snprintf (b + l, sizeof (b) - l, "0x%lx", *((long *) var + x));
                        continue;
                    }

                case GT_STRING:

                    b [l++] = '"';

                    if ((optype & GO_DIRECT)) str =   (char  *) var + x;
                    else                      str = *((char **) var + x);

                    if (str) l += snprintf (b + l, sizeof (b) - l, "%s", str);
                    b [l++] = '"';
                    continue;

                default: continue;
            }

            l += snprintf (b + l, sizeof (b) - l, "%ld", num);
        }

        if (array) b [l++] = ']';
        b [l] = 0;

        if ((l += 2) > len_tab [VALUE_IDX][j]) len_tab [VALUE_IDX][j] = l;

        if ((optype & GO_NEXTCOL)) j = (j + 1) & 1;
        else j = 0;

        if (!(val_tab [i] = new (std::nothrow) char [l + 1])) guty_abort (caller_name, GA_ERROR | GA_VALUE, ENOMEM, "val_tab []");

        val_tab [i][0] = '=';
        val_tab [i][1] = ' ';

        memcpy (val_tab [i] + 2, b, l - 1);
    }

    fprintf (stderr, "Usage: %s", caller_name);

    if (cnt_arg)
    {
        strncpy (b, " Arguments", sizeof (b));
        if (cnt_arg < 2) b [9] = 0;

        fputs (b, stderr);
    }

    if (cnt_opt)
    {
        i = 25;
        strncpy (b + 16, " [Options]", sizeof (b) - 16);
        if (cnt_opt < 2) b [24] = ']', b [i--] = 0;

        fputs (b + 16, stderr);
        b [i] = 0;
    }

    fputc ('\n', stderr);

    if (cnt_arg)
    {
        fprintf (stderr, "%s:\n", b + 1);
        help_formatter (prm_tab, val_tab, len_tab, 0);
    }

    if (cnt_opt)
    {
        fprintf (stderr, "%s:\n", b + 18);
        help_formatter (prm_tab, val_tab, len_tab, 1);
    }

    for (i = 0; prm_tab [i].name; i++) if (val_tab [i]) delete val_tab [i];    // Cleanup
    delete val_tab;
}

void guty_param::help_formatter (guty_param *prm_tab, char **val_tab, int len_tab [4][4], int phase)    // phase: 0 = arg, 1 = opt
{
    char          c;
    const char   *hlp, *name, *str;
    int           i, j, k, l, x;
    unsigned int  optype;

    for (i = 0, j = 0, name = prm_tab [i].name; name; i++, name = prm_tab [i].name)
    {
        if (phase) {if (*name != '-') continue;}    // Filtro opzioni
        else       {if (*name == '-') continue;}    // Filtro argomenti

        if (i > 0 && ((k = prm_tab [i - 1].optype) & GO_NO_VALCOL) && j > 0 && (k & GO_NEXTCOL)) fputc (' ',stderr);
        else fputs ("   ", stderr);

        optype = prm_tab [i].optype;

        x = 1;
        if ((optype & GO_NO_FORMAT)) x = 0;

        l = 0;

        if (phase && (str = prm_tab [i].id))    // Stampa ID
        {
            l = len_tab [ID_IDX][j];
            k = l - strlen (str) - 1;

            fputs (str, stderr);

            if (x) for (; k > 0; k--) fputc (' ', stderr);
            fputc ('|', stderr);
        }

        k = optype & GO_REQ;
        l = len_tab [NAME_IDX][j] - strlen (name) - l;

        if (!phase)    // in fase arg aggiunge [] ai campi non req
        {
            if (!k) c = '[';
            else    c = ' ';

            fputc (c, stderr);
            l--;
        }

        fputs (name, stderr);

        if (!phase && !k)         {fputc (']', stderr); l--;}
        if (x) for (; l > 0; l--)  fputc (' ', stderr);

        if ((l = len_tab [INFO_IDX][j]))    // Stampa Info
        {
            fputc (' ', stderr);

            if ((str = prm_tab [i].info))
            {
                l -= strlen (str);
                fputs (str, stderr);
            }

            if (x) for (; l > 0; l--) fputc (' ', stderr);
        }

        hlp = prm_tab [i].help;

        if (!(optype & GO_NO_VALCOL))    // Stampa Value
        {
            str = val_tab [i];
            fprintf (stderr, " %s", str);

            if (x && j < 4 && ((optype & GO_NEXTCOL) || hlp))
                for (l = len_tab [VALUE_IDX][j] - strlen (str); l > 0; l--) fputc (' ', stderr);
        }

        if (hlp)    // Stampa Help
        {
            c = '\n';
            if ((optype & GO_HELPCOL)) c = ' ';

            fprintf (stderr, "%c%s", c, hlp);
            optype = 0;                          // Column Reset
        }

        if ((optype & GO_NEXTCOL)) j = (j + 1) & 3;
        else j = 0;

        if (!j) fputc ('\n', stderr);
    }

    if (j) fputc ('\n', stderr);
}
