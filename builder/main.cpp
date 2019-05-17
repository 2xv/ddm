#include <cerrno>
#include <cstdio>
#include <cstring>
#include <ctime>

#include "guty_abort.h"
#include "guty_param.h"

struct DDProps
{
    DDProps    *next;
    char       *name [2];
    long long   min;
    long long   max;
    char       *def  [2];
    char       *inOrProps [2];
    int         id;
    int         number;
    int         operTypeId;
    bool        f_min;
    bool        f_max;
    bool        f_req;
    bool        f_sub_props;
};

const char *types [] = {  "BYTE",   "SHORT",   "INT",        "LONG",    "FLOAT",   "DOUBLE",   "NUMBER",   "S_NUMBER",
                          "TIME",   "DATE",    "DATETIME",   "BYTES",   "STRING",  "LIST",
                        "A_BYTE", "A_SHORT", "A_INT",      "A_LONG",  "A_FLOAT", "A_DOUBLE", "A_NUMBER", "A_S_NUMBER",
                        "A_TIME", "A_DATE",  "A_DATETIME", "A_BYTES", "A_STRING",   nullptr};

FILE       *fj = nullptr, *fq = nullptr;
const char *caller_name;
char        b [4096], *name, sep2 = ';';
DDProps    *p_work;
int         cnt, id;
int         col_name = 1,col_type = 2, col_id = 3, col_req = 4, col_min = 5, col_max = 6, col_num = 7, col_def = 8, col_in = 9;

void generic_invalid (const char *lab)
{
    char msg [256];

    snprintf   (msg, sizeof (msg), "Invalid %s [id=%d, name=%s]", lab, id, name);
    guty_abort (caller_name, GA_ERROR | GA_VALUE, EINVAL, msg);
}

void check_bytes (const char *lab, long long num, int size)
{
    bool valid = true;
    long long  v;

    switch (size)
    {
        case  1: if ((v = num >>  8) != -1 && !v) valid = false; break;
        case  2: if ((v = num >> 16) != -1 && !v) valid = false; break;
        default: if ((v = num >> 32) != -1 && !v) valid = false;
    }

    if (!valid) generic_invalid (lab);
}

int item_count (char *s)
{
    int i, j;

    for (i = 0, j = 0; s [i]; i++)
         if (s [i] == ',') j++;

    return j + 1;
}

void add_default (const char *fmtj, const char *fmtq, char *data, bool f_bytes = false)
{
    char buf [4096];
    int  len = snprintf (buf, sizeof (buf), fmtj, data);

    if (f_bytes)
    {
        buf [len++] = '{';
        bool ignore =  false;
        bool npv    =  false;

        for (int i = 0, j = strlen (data); i < j; i++)
        {
            int v = data [i];

            if (v != '"') ignore = false;
            else
            {
                if (ignore) {ignore = false; continue;}
                if (!i || i == (j - 1)) continue;
                ignore = true;
            }

            if (v == '\\' && data [i + 1] >= '0' && data [i + 1] <= '3' &&
                             data [i + 2] >= '0' && data [i + 2] <= '9' &&
                             data [i + 3] >= '0' && data [i + 3] <= '9')
            {
                v = ((data [i + 1] - '0') * 8 + data [i + 2] - '0') * 8 + data [i + 3] - '0';
                i += 3;
            }

            if (npv) buf [len++] = ',';
            len += snprintf (buf + len, sizeof (buf) - len, "%d", (char) v);
            npv  = true;
        }

        buf [len++] = '}';
        buf [len++] =  0;
    }

    p_work->def [0] = new char [len + 1];
    memcpy (p_work->def [0], buf, len + 1);

    len = snprintf (buf, sizeof (buf), fmtq, data);

    p_work->def [1] = new char [len + 1];
    memcpy (p_work->def [1], buf, len + 1);
}

void add_in (const char *fmtj, const char *fmtq, char **tab, int max,
             const char *ifmtj = "%s", const char *ifmtq = "%s", bool f_bytes = false)
{
    char buf [4096];
    int  len = snprintf (buf, sizeof (buf), fmtj);

    for (int i = 0; i < max; i++)
    {
        if (i) buf [len++] = ',';

        if (!f_bytes) len += snprintf (buf + len, sizeof (buf) - len, ifmtj, tab [i]);
        else
        {
            len += snprintf (buf + len, sizeof (buf) - len, ifmtj);
            buf [len++] = '{';
            bool ignore =  false;
            bool npv    =  false;

            for (int j = 0, k = strlen (tab [i]); j < k; j++)
            {
                int v = tab [i][j];

                if (v != '"') ignore = false;
                else
                {
                    if (ignore) {ignore = false; continue;}
                    if (!j || j == (k - 1)) continue;
                    ignore = true;
                }

                if (v == '\\' && tab [i][j + 1] >= '0' && tab [i][j + 1] <= '3' &&
                                 tab [i][j + 2] >= '0' && tab [i][j + 2] <= '9' &&
                                 tab [i][j + 3] >= '0' && tab [i][j + 3] <= '9')
                {
                    v = ((tab [i][j + 1] - '0') * 8 + tab [i][j + 2] - '0') * 8 + tab [i][j + 3] - '0';
                    j += 3;
                }

                if (npv) buf [len++] = ',';
                len += snprintf (buf + len, sizeof (buf) - len, "%d", (char) v);
                npv  = true;
            }

            buf [len++] = '}';
        }
    }

    buf [len++] = '}';
    buf [len++] =  0;

    p_work->inOrProps [0] = new char [len + 1];
    memcpy (p_work->inOrProps [0], buf, len + 1);

    len = snprintf (buf, sizeof (buf), fmtq);

    for (int i = 0; i < max; i++)
    {
        if (i) buf [len++] = ',';
        len += snprintf (buf + len, sizeof (buf) - len, ifmtq, tab [i]);
    }

    buf [len++] = '}';
    buf [len++] =  0;

    p_work->inOrProps [1] = new char [len + 1];
    memcpy (p_work->inOrProps [1], buf, len + 1);
}

long long get_number (char *str, bool neg)
{
    if (!*str) return 1;

    long long lh = 0, ll =  0;                           // Long Limits = -9223372036854775808..9223372036854775807
    long long lmax = (neg ? 6854775808 : 6854775807);
    int       i;

    for (i = 0; str [i]; i++)
    {
        if (str [i] < '0' || str [i] > '9') return 1;

             if (i < 10) lh = lh * 10 + (str [i] - '0');
        else if (i < 19) ll = ll * 10 + (str [i] - '0');
        else return  1;
    }

    if (i <= 10) return -lh;
    if (lh > 922337203 || (lh == 922337203 && ll > lmax)) return 1;
    return (-lh) * 10000000000 + (-ll);
}

int split (char *b, char **bp, char sep, bool unquote = false)
{
    char *s1,  *s2;
    int   bpm = 0, i, j;
    bool  sv  = false;

    for (i = 0; b [i]; i++)
    {
        if (!sv)    // Assegnazione Valore
        {
            if (bpm >= 2048) guty_abort   (caller_name, GA_ERROR | GA_VALUE | GA_LINE, E2BIG, "Too many elements [max=2048]", cnt);
            for (; b [i] && b [i] <= ' ' && b [i] != sep; i++);

            bp [bpm++] = b + i;
            sv         = true;
        }

        if (b [i] == sep)    // Chiusura Valore
        {
            b [i] = 0;
            sv    = false;

            for (j = i - 1; j >= 0 && b [j] && b [j] <= ' '; j--) b [j] = 0;

            if (unquote && *bp [bpm - 1] == '"')
            {
                for (j = 0, s1 = bp [bpm - 1], s2 = s1 + 1; *s2; j++, s1++, s2++)
                {
                    *s1 = *s2;
                     if (*s2 == '"' && *(s2 + 1) == '"') s2++;
                }

                if (j > 1 && *(s1 - 1) == '"') *(s1 - 1) = 0;
            }

//          printf ("%4d [%s]\n", bpm - 1, bp [bpm - 1]);
        }
    }

    return bpm;
}

void write_java_props ()
{
    int i = snprintf (b, sizeof (b), "DDConst.%s", types [p_work->operTypeId]);
    if (p_work->f_req) i += snprintf (b + i, sizeof (b) - i, " | DDConst.REQ");

         if (p_work->f_min && p_work->f_max) i += snprintf (b + i, sizeof (b) - i, " | DDConst.CHKLIM");
    else if (p_work->f_min)                  i += snprintf (b + i, sizeof (b) - i, " | DDConst.CHKMIN");
    else if (p_work->f_max)                  i += snprintf (b + i, sizeof (b) - i, " | DDConst.CHKMAX");

    fprintf (fj, "        new DDProps (\"%s\", %s, %lld, %lld, %d, %s, %s, %s),\n",
             p_work->name [0], p_work->name [1],
            (p_work->f_min  ?  p_work->min : 0),
            (p_work->f_max  ?  p_work->max : 0), p_work->number, b,
            (p_work->def       [0] ? p_work->def       [0] : "null"),
            (p_work->inOrProps [0] ? p_work->inOrProps [0] : "null"));
}

void write_qt_props ()
{
    int i = snprintf (b, sizeof (b), "DDC_%s", types [p_work->operTypeId]);
    if (p_work->f_req) i += snprintf (b + i, sizeof (b) - i, " | DDC_REQ");

         if (p_work->f_min && p_work->f_max) i += snprintf (b + i, sizeof (b) - i, " | DDC_CHKLIM");
    else if (p_work->f_min)                  i += snprintf (b + i, sizeof (b) - i, " | DDC_CHKMIN");
    else if (p_work->f_max)                  i += snprintf (b + i, sizeof (b) - i, " | DDC_CHKMAX");

    fprintf (fj, "    {\"%s\", %s, %lld, %lld, %d, %s, %s, %s}",
             p_work->name [0], p_work->name [1],
            (p_work->f_min  ?  p_work->min : 0),
            (p_work->f_max  ?  p_work->max : 0), p_work->number, b,
            (p_work->def       [1] ? p_work->def       [1] : "nullptr"),
            (p_work->inOrProps [1] ? p_work->inOrProps [1] : "nullptr"));
}

int main (int argc, char **argv)
{
    struct com_list
    {
        com_list *next;
        char     *row;
    };

    DDProps   *p_first  = nullptr, *p_last = nullptr, *p_work1;
    FILE      *fin;
    bool       java = true, qt = true;
    char      *entity = nullptr, *input_path = nullptr, *package = nullptr;
    char       sep1   = 9;
    char      *bp [2048], *defbp [2048],  *inbp [2048];
    int        bpm, defbm, i, j,  k, inbm, type;
    int        num;
    long long  min, max;
    bool       f_req,  f_min, f_max, f_neg, f_sub_props;
    com_list  *c_first = nullptr, *c_last = nullptr, *c_work;

    guty_param prm_tab [] =
        {{ nullptr, "input_path",   &fin,    &input_path, nullptr, 0, 0, GT_FILE,          "(file)",   nullptr, nullptr},
         {"-n",     "--name",       &entity,  nullptr,    nullptr, 0, 0, GT_STRING,        "(string)", nullptr, nullptr},
         {"-p",     "--package",    &package, nullptr,    nullptr, 0, 0, GT_STRING,        "(string)", nullptr, nullptr},
         {"-j-",    "--no_java",    &java,    nullptr,    nullptr, 0, 0, GT_FLAG | GT_NEG, "(=0)",     nullptr, nullptr},
         {"-q-",    "--no_qt",      &qt,      nullptr,    nullptr, 0, 0, GT_FLAG | GT_NEG, "(=0)",     nullptr, nullptr},
         {"-s1",    "--separator1", &sep1,    nullptr,    nullptr, 0, 0, GT_CHAR,          "(char)",   nullptr, nullptr},
         {"-s2",    "--separator2", &sep2,    nullptr,    nullptr, 0, 0, GT_CHAR,          "(char)",   nullptr, nullptr},
         { nullptr,  nullptr,       nullptr,  nullptr,    nullptr, 0, 0, 0,                 nullptr,   nullptr, nullptr}};

    guty_param::get (argc, argv, prm_tab, 0, &caller_name);
    if (!fin) fin = stdin;

//  +-----------+
//  | Main Loop |
//  +-----------+

    for (cnt = 1; ; cnt++)
    {
        if  (!fgets (b, sizeof (b), fin)) break;
        bpm = split (b, bp, sep1, true);

        for (; bpm && !*bp [bpm - 1]; bpm--);
        if (!bpm || **bp == '#') continue;

        if (**bp == '*' && !bp [0][1])
        {
            c_work = new com_list;
            c_work->next = nullptr;

            if (c_last) c_last->next = c_work;
            else        c_first      = c_work;

            c_last = c_work;
            for (i = 1, j = 0; i < bpm; i++) j += strlen (bp [i]) + 1;

            c_work->row = new char [j];
            c_work->row [j - 1] = 0;

            for (i = 1, k = 0; i < bpm; i++) k += snprintf (c_work->row + k, j - k, "%s ", bp [i]);
            continue;
        }

        if ((**bp == 'C' || **bp == 'c') && !bp [0][1])
        {
            for (i = 1, k = 0; i < bpm; i++)
            {
                for (j = 0; j < strlen (bp [i]); j++)
                     if (bp [i][j] >= 'A' && bp [i][j] <= 'Z') bp [i][j] += ' ';

                     if (!strcmp (bp [i], "nome")    || !strcmp (bp [i], "nome campo")   ||
                         !strcmp (bp [i], "name")    || !strcmp (bp [i], "field name")) {col_name = i; k |=   1;}
                else if (!strcmp (bp [i], "tipo")    || !strcmp (bp [i], "type"))       {col_type = i; k |=   2;}
                else if (!strcmp (bp [i], "req"))                                       {col_req  = i; k |=   4;}
                else if (!strcmp (bp [i], "min"))                                       {col_min  = i; k |=   8;}
                else if (!strcmp (bp [i], "max"))                                       {col_max  = i; k |=  16;}
                else if (!strcmp (bp [i], "number")  || !strcmp (bp [i], "num"))        {col_num  = i; k |=  32;}
                else if (!strcmp (bp [i], "default") || !strcmp (bp [i], "def"))        {col_def  = i; k |=  64;}
                else if (!strcmp (bp [i], "in"))                                        {col_in   = i; k |= 128;}
            }

            if (k != 255)
            {
                snprintf   (b, sizeof (b), "Some columns are missing [bitmap=0x%02X]", k);
                guty_abort (caller_name, GA_ERROR | GA_VALUE | GA_LINE, EINVAL, b, cnt);
            }

            continue;
        }

        if ((**bp != '+' && **bp != '-') || bp [0][1]) continue;
        f_sub_props = **bp == '+';
// name
        if (bpm < col_name || !*bp [col_name] || *bp [col_name] == '#')
            guty_abort (caller_name, GA_ERROR | GA_VALUE | GA_LINE, EINVAL, "Missing name", cnt);

        for (i = 0; bp [col_name][i]; i++)
        {
            if (bp [col_name][i] >= 'A' && bp [col_name][i] <= 'Z') continue;
            if (bp [col_name][i] >= 'a' && bp [col_name][i] <= 'z') continue;

            if (!i || bp [col_name][i] < '0' || (bp [col_name][i] > '9' && bp [col_name][i] != '_'))
                guty_abort (caller_name, GA_ERROR | GA_VALUE | GA_LINE, EINVAL, "Invalid name", cnt);
        }

        if (*bp [col_name] >= 'A' && *bp [col_name] <= 'Z') *bp [col_name] += ' ';
        name = bp [col_name];
// type
        if (bpm < col_type || !*bp [col_type] || *bp [col_type] == '#')
            guty_abort (caller_name, GA_ERROR | GA_VALUE | GA_LINE, EINVAL, "Missing type", cnt);

        for (i = 0; bp [col_type][i]; i++)
        {
            if (bp [col_type][i] >= 'A' && bp [col_type][i] <= 'Z') continue;
            if (bp [col_type][i] == '_')                     continue;

            if (bp [col_type][i] <  'a' || bp [col_type][i] >  'z')
                guty_abort (caller_name, GA_ERROR | GA_VALUE | GA_LINE, EINVAL, "Invalid type", cnt);

            bp [col_type][i] -= ' ';
        }

        for (type = 0; types [type] && strcmp (bp [col_type], types [type]); type++);
        if (!types [type]) guty_abort (caller_name, GA_ERROR | GA_VALUE | GA_LINE, EINVAL, "Unknown type", cnt);

// ID
        if (bpm < col_id || !*bp [col_id] || *bp [col_id] == '#')
            guty_abort (caller_name, GA_ERROR | GA_VALUE | GA_LINE, EINVAL, "Missing id", cnt);

        for (i = 0, id = 0, j = 0; bp [col_id][i]; i++, j = id)
        {
            if (bp [col_id][i] < '0' || bp [col_id][i] > '9') break;
            id = id * 10 + (bp [col_id][i] - '0');
            if (id < j) break;
        }

        if (bp [col_id][i]) guty_abort (caller_name, GA_ERROR | GA_VALUE | GA_LINE, EINVAL, "Invalid id", cnt);
// opts
        num   =  0;
        f_req =  false;
        f_min =  false;
        f_max =  false;
        defbm =  0;
        inbm  =  0;
// req
        if (bpm > col_req && *bp [col_req])
        {
            if (*bp [col_req] == '#') goto add_props;

            for (i = 0; bp [col_req][i]; i++)
            {
                if (bp [col_req][i] >= 'a' && bp [col_req][i] <= 'z') continue;
                if (bp [col_req][i] <  'A' || bp [col_req][i] >  'Z') break;
                bp [col_req][i] += ' ';
            }

            if (bp [col_req][i]) i = 0;
            else
            {
                if (!strcmp (bp [col_req], "y") || !strcmp (bp [col_req], "yes") ||
                    !strcmp (bp [col_req], "s") || !strcmp (bp [col_req], "si")  ||
                    !strcmp (bp [col_req], "1"))   f_req =  true;
                else
                    if (strcmp (bp [col_req], "n") && strcmp (bp [col_req], "no") ||
                        strcmp (bp [col_req], "0"))   i = 0;
            }

            if (!i) guty_abort (caller_name, GA_ERROR | GA_VALUE | GA_LINE, EINVAL, "Invalid req", cnt);
        }

// min
        if (bpm > col_min && *bp [col_min])
        {
            if (*bp [col_min] == '#') goto add_props;

            f_neg = *bp [col_min] == '-';
            min   =  get_number ((f_neg ? bp [col_min] + 1 : bp [col_min]), f_neg);

            if (min > 0) guty_abort (caller_name, GA_ERROR | GA_VALUE | GA_LINE, EINVAL, "Invalid min", cnt);
            if (!f_neg) min = -min;

            f_min = true;
        }

// max
        if (bpm > col_max && *bp [col_max])
        {
            if (*bp [col_max] == '#') goto add_props;

            f_neg = *bp [col_max] == '-';
            max   =  get_number ((f_neg ? bp [col_max] + 1 : bp [col_max]), f_neg);

            if (max > 0) guty_abort (caller_name, GA_ERROR | GA_VALUE | GA_LINE, EINVAL, "Invalid max", cnt);
            if (!f_neg) max = -max;

            if (max < min) guty_abort (caller_name, GA_ERROR | GA_VALUE | GA_LINE, EINVAL, "Max < min", cnt);
            f_max = true;
        }

// num
        if (bpm > col_num && *bp [col_num])
        {
            if (*bp [col_num] == '#') goto add_props;

            if (type >= 14)    // Arrays
            {
                f_neg = *bp [col_num] == '-';

                for (i = (f_neg ? 1 : 0); bp [col_num][i]; i++)
                {
                    if (num > 65530 || bp [col_num][i] < '0' || bp [col_num][i] > '9')
                        guty_abort (caller_name, GA_ERROR | GA_VALUE | GA_LINE, EINVAL, "Invalid number", cnt);

                    num = num * 10 + (bp [col_num][i] - '0');
                }

                if (f_neg) num = -num;
            }
        }

// def
        if (bpm > col_def && *bp [col_def])
        {
            if (*bp [col_def] == '#') goto add_props;
            defbm = split (bp [col_def], defbp, sep2);
        }

// in
        if (bpm > col_in && *bp [col_in])
        {
            if (*bp [col_in] == '#') goto add_props;
            inbm = split (bp [col_in], inbp, sep2);
        }

    add_props:

        p_work                = new DDProps ();
        p_work->next          = nullptr;
        p_work->def       [0] = nullptr;
        p_work->def       [1] = nullptr;
        p_work->inOrProps [0] = nullptr;
        p_work->inOrProps [1] = nullptr;

        if (p_last) p_last->next = p_work;
        else        p_first      = p_work;

        p_last = p_work;

        switch (type)
        {
/*byte*/    case  0:    if (f_min) check_bytes ("min", min, 1);
                        if (f_max) check_bytes ("max", max, 1);
                        if (defbm) add_default ("(byte)%s",    "new char(%s)",      *defbp);
                        if (inbm)  add_in      ("new byte[]{", "new QVector<char>{", inbp, inbm);
                        break;

/*short*/   case  1:    if (f_min) check_bytes ("min", min, 2);
                        if (f_max) check_bytes ("max", max, 2);
                        if (defbm) add_default ("(short)%s",    "new short(%s)",      *defbp);
                        if (inbm)  add_in      ("new short[]{", "new QVector<short>{", inbp, inbm);
                        break;
/*time*/    case  8:
/*date*/    case  9:
/*int*/     case  2:    if (f_min) check_bytes ("min", min, 4);
                        if (f_max) check_bytes ("max", max, 4);
                        if (defbm) add_default ("(int)%s",    "new int(%s)",      *defbp);
                        if (inbm)  add_in      ("new int[]{", "new QVector<int>{", inbp, inbm);
                        break;

/*float*/   case  4:    if (f_min) check_bytes ("min", min, 4);
                        if (f_max) check_bytes ("max", max, 4);
                        if (defbm) add_default ("(float)%s",    "new float(%s)",      *defbp);
                        if (inbm)  add_in      ("new float[]{", "new QVector<float>{", inbp, inbm);
                        break;

/*double*/  case  5:    if (defbm) add_default ("(double)%s",    "new double(%s)",      *defbp);
                        if (inbm)  add_in      ("new double[]{", "new QVector<double>{", inbp, inbm);
                        break;

/*bytes*/   case 11:    if (f_min && min < 0)     generic_invalid ("min");
                        if (f_max && max > 65530) generic_invalid ("max");
                        if (defbm) add_default ("new byte[]",    "new QByteArray(%s)",      *defbp, true);
                        if (inbm)  add_in      ("new byte[][]{", "new QVector<QByteArray>{", inbp,  inbm,
                                                "new byte[]",    "QByteArray(%s)",           true);
                        break;

/*string*/  case 12:    if (f_min && min < 0)     generic_invalid ("min");
                        if (f_max && max > 65530) generic_invalid ("max");
                        if (defbm) add_default ("(String)%s",    "new QString(%s)",      *defbp);
                        if (inbm)  add_in      ("new String[]{", "new QVector<QString>{", inbp, inbm);
                        break;

/*list*/    case 13:    if (f_min && min < 0)     generic_invalid ("min");
                        if (f_max && max > 65530) generic_invalid ("max");
                        break;

/*a_byte*/  case 14:    if (f_min) check_bytes ("min", min, 1);
                        if (f_max) check_bytes ("max", max, 1);
                        if (defbm) add_default ("new byte[]{%s}", "new QVector<char>{%s}",      *defbp);
                        if (inbm)  add_in      ("new byte[][]{",  "new QVector<QVector<char>>{", inbp, inbm,
                                                "new byte[]{%s}", "QVector<char>{%s}");
                        break;

/*a_short*/ case 15:    if (f_min) check_bytes ("min", min, 2);
                        if (f_max) check_bytes ("max", max, 2);
                        if (defbm) add_default ("new short[]{%s}", "new QVector<short>{%s}",      *defbp);
                        if (inbm)  add_in      ("new short[][]{",  "new QVector<QVector<short>>{", inbp, inbm,
                                                "new short[]{%s}", "QVector<short>{%s}");
                        break;
/*a_time*/  case 22:
/*a_date*/  case 23:
/*a_int*/   case 16:    if (f_min) check_bytes ("min", min, 4);
                        if (f_max) check_bytes ("max", max, 4);
                        if (defbm) add_default ("new int[]{%s}", "new QVector<int>{%s}",      *defbp);
                        if (inbm)  add_in      ("new int[][]{",  "new QVector<QVector<int>>{", inbp, inbm,
                                                "new int[]{%s}", "QVector<int>{%s}");
                        break;

/*a_float*/ case 18:    if (f_min) check_bytes ("min", min, 4);
                        if (f_max) check_bytes ("max", max, 4);
                        if (defbm) add_default ("new float[]{%s}", "new QVector<float>{%s}",      *defbp);
                        if (inbm)  add_in      ("new float[][]{",  "new QVector<QVector<float>>{", inbp, inbm,
                                                "new float[]{%s}", "QVector<float>{%s}");
                        break;

/*a_double*/case 19:    if (defbm) add_default ("new double[]{%s}", "new QVector<double>{%s}",      *defbp);
                        if (inbm)  add_in      ("new double[][]{",  "new QVector<QVector<double>>{", inbp, inbm,
                                                "new double[]{%s}", "QVector<double>{%s}");
                        break;

/*a_number*/case 20:    if (f_min && min < 0) generic_invalid ("min");
/*a_s_num*/ case 21:
/*a_datetm*/case 24:
/*a_long*/  case 17:    if (defbm) add_default ("new long[]{%s}", "new QVector<qint64>{%s}",      *defbp);
                        if (inbm)  add_in      ("new long[][]{",  "new QVector<QVector<qint64>>{", inbp, inbm,
                                                "new long[]{%s}", "QVector<qint64>{%s}");
                        break;

/*a_bytes*/ case 25:    if (f_min && min < 0)     generic_invalid ("min");
                        if (f_max && max > 65530) generic_invalid ("max");
                        break;

/*a_string*/case 26:    if (f_min && min < 0)     generic_invalid ("min");
                        if (f_max && max > 65530) generic_invalid ("max");
                        if (defbm) add_default ("new String[]{%s}", "new QVector<QString>{%s}",      *defbp);
                        if (inbm)  add_in      ("new String[][]{",  "new QVector<QVector<QString>>{", inbp, inbm,
                                                "new String[]{%s}", "QVector<QString>{%s}");
                        break;

/*number*/  case  6:    if (f_min && min < 0) generic_invalid ("min");
/*s_number  case  7: */
/*datetime  case 10: */
/*long*/    default:    if (defbm) add_default ("(long)%s",    "new qint64(%s)",      *defbp);
                        if (inbm)  add_in      ("new long[]{", "new QVector<qint64>{", inbp, inbm);
        }

        i = strlen (name);
/*name*/p_work->name [0] = new char [i + 1];
        memcpy (p_work->name [0], name, i + 1);

        for (j = 0, k = 0; j < i; j++, k++)
        {
            if (j && name [j]     >= 'A' && name [j]     <= 'Z'  &&
                     name [j - 1] >= 'a' && name [j - 1] <= 'z') b [k++] = '_';

            b [k] = name [j] & 0xDF;
        }

        b [k] = 0;
        p_work->name [1] = new char [k + 1];
        memcpy (p_work->name [1], b, k + 1);

/*id*/  p_work->id          = id;
/*min*/ p_work->f_min       = f_min;
        p_work->min         = min;
/*max*/ p_work->f_max       = f_max;
        p_work->max         = max;
/*req*/ p_work->f_req       = f_req;
/*num*/ p_work->number      = num;
/*opty*/p_work->operTypeId  = type;
        p_work->f_sub_props = f_sub_props;
    }

    if (!java && !qt) return 0;
    if (!p_first)     guty_abort (caller_name, GA_ERROR | GA_VALUE, ECANCELED, "No properties found");

    for (p_work = p_first; p_work; p_work = p_work->next)
         for (p_work1 = p_work->next; p_work1; p_work1 = p_work1->next)
              if (p_work->id == p_work1->id)
              {
                  snprintf   (b, sizeof (b), "Duplicate id [id=%d]", p_work->id);
                  guty_abort (caller_name, GA_ERROR | GA_VALUE, ECANCELED, b);
              }

//  +------------------+
//  | Build Properties |
//  +------------------+

    if (!entity)
    {
        if (!input_path) entity = (char *) "noname";
        else
        {
            for (i = strlen (input_path) - 1; i && input_path [i - 1] != '/' && input_path [i - 1] != '\\'; i--);
            for (j = i; input_path [j] && input_path [j] != '.'; j++);

            input_path [j] = 0;
            entity = input_path + i;
        }
    }

    if (java)
    {
        snprintf (b, sizeof (b), "%s.java", entity);
        if (!(fj = fopen (b, "w"))) guty_abort (caller_name, GA_ERROR | GA_VALUE, errno, b);

        tm tm;
        time_t t = time (nullptr);
        localtime_r (&t, &tm);

        fprintf (fj, "// Generated by Dynamic Data Builder [dateTime=%04d-%02d-%02d_%02d:%02d:%02d]\n",
                         tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

        for (c_work = c_first; c_work; c_work = c_work->next) fprintf (fj, "// %s\n", c_work->row);

        fprintf (fj, "\npackage %s\n\n"
                     "import it.xv2.ddm.DDConst;\n"
                     "import it.xv2.ddm.DDProps;\n\n"
                     "public final class %s\n{\n", (package ? package : "..."), entity);

        for (p_work = p_first; p_work; p_work = p_work->next)
             fprintf (fj, "    public static final int %s = %d;\n", p_work->name [1], p_work->id);

        fprintf (fj, "\n    public static final DDProps [] Props = // name, id, min, max, number, optype, default, in\n    {\n");

        for (p_work = p_first; p_work; p_work = p_work->next)
        {
            if (p_work->f_sub_props) continue;
            write_java_props ();
        }

        fprintf (fj, "    };\n");

        for (p_work = p_first; p_work; )
            if (p_work->operTypeId == 13)
            {
                if (!p_work->next || !p_work->next->f_sub_props)
                {
                    snprintf   (b, sizeof (b), "No list properties found [id=%d]", p_work->id);
                    guty_abort (caller_name, GA_ERROR | GA_VALUE, ECANCELED, b);
                }

                fprintf (fj, "\n    public static final DDProps [] %sProps = // name, id, min, max, number, optype, default, in\n    {\n",
                                   *p_work->name);

                for (p_work = p_work->next; p_work && p_work->f_sub_props; p_work = p_work->next) write_java_props ();
                fprintf (fj, "    };\n");
            }
            else
                p_work = p_work->next;

        fprintf (fj, "}\n");
        fclose (fj);
    }

    if (qt)
    {
        snprintf (b, sizeof (b), "%s.qt", entity);
        if (!(fq = fopen (b, "w"))) guty_abort (caller_name, GA_ERROR | GA_VALUE, errno, b);

        tm tm;
        time_t t = time (nullptr);
        localtime_r (&t, &tm);

        fprintf (fq, "// Generated by Dynamic Data Builder [dateTime=%04d-%02d-%02d_%02d:%02d:%02d]\n",
                         tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

        for (c_work = c_first; c_work; c_work = c_work->next) fprintf (fq, "// %s\n", c_work->row);

        fprintf (fq, "\n#include <QtGlobal>\n"
                     "#include <QByteArray>\n"
                     "#include <QString>\n"
                     "#include <QVector>\n"
                     "#include \"DDConst.h\"\n"
                     "#include \"DDProps.h\"\n\n");

        for (p_work = p_first; p_work; p_work =  p_work->next)
             fprintf (fq, "#define %s %d\n", p_work->name [1], p_work->id);

        fprintf (fq, "\nstatic QVector <DDProps> %s_props = { // name, id, min, max, number, optype, default, in\n", entity);

        for (p_work = p_first; p_work; p_work = p_work->next)
        {
            if (p_work->f_sub_props) continue;
            write_qt_props ();

            if (p_work->next) fputc (',', fq);
            fputc ('\n', fq);
        }

        fprintf (fq, "};\n");

        for (p_work = p_first; p_work; )
            if (p_work->operTypeId == 13)
            {
                if (!p_work->next || !p_work->next->f_sub_props)
                {
                    snprintf   (b, sizeof (b), "No list properties found [id=%d]", p_work->id);
                    guty_abort (caller_name, GA_ERROR | GA_VALUE, ECANCELED, b);
                }

                fprintf (fq, "\nstatic QVector <DDProps> %s_%s_props = { // name, id, min, max, number, optype, default, in\n", entity,
                              *p_work->name);

                for (p_work = p_work->next; p_work && p_work->f_sub_props; p_work = p_work->next)
                {
                    write_qt_props ();

                    if (p_work->next && p_work->next->f_sub_props) fputc (',', fq);
                    fputc ('\n', fq);
                }

                fprintf (fq, "};\n");
            }
            else
                p_work = p_work->next;

        fclose  (fq);
    }

    return 0;
}
