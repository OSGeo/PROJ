/***********************************************************************

       gie - The Geospatial Integrity Investigation Environment

************************************************************************

The Geospatial Integrity Investigation Environment "gie" is a modest
regression testing environment for the PROJ.4 transformation library.

Its primary design goal was to be able to replace those thousands of
lines of regression testing code that are (at time of writing) part
of PROJ.4, while not requiring any other kind of tooling than the same
C compiler already employed for compiling the library.

The basic functionality of the gie command language is implemented
through just 3 command verbs:

OPERATION,     which defines the PROJ.4 operation to test,
ACCEPT,        which defines the input coordinate to read, and
EXPECT,        which defines the result to expect.

E.g:

operation  +proj=utm  +zone=32  +ellps=GRS80
accept     12  55
expect     691_875.632_14   6_098_907.825_05

Note that gie accepts the underscore ("_") as a thousands separator.
It is not required (in fact, it is entirely ignored by the input
routine), but it significantly improves the readability of the very
long strings of numbers typically required in projected coordinates.

By default, gie considers the EXPECTation met, if the result comes to
within 0.5 mm of the expected. This default can be changed using the
TOLERANCE command verb (and yes, I know, linguistically speaking, both
"operation" and "tolerance" are nouns, not verbs). See the first
few hundred lines of the "builtins.gie" test file for more details of
the command verbs available (verbs of both the VERBal and NOUNistic
persuation).

--

But more importantly than being an acronym for "Geospatial Integrity
Investigation Environment", gie were also the initials, user id, and
USGS email address of Gerald Ian Evenden (1935--2016), the geospatial
visionary, who, already in the 1980s, started what was to become the
PROJ.4 of today.

Gerald's clear vision was that map projections are *just special
functions*. Some of them rather complex, most of them of two variables,
but all of them *just special functions*, and not particularly more
special than the sin(), cos(), tan(), and hypot() already available in
the C standard library.

And hence, *they should not be particularly much harder to use*, for a
programmer, than the sin()s, tan()s and hypot()s so readily available.

Gerald's ingenuity also showed in the implementation of the vision,
where he devised a highly comprehensible, yet simple, system of key-value
pairs for parameterising a map projection, and the highly flexible
PJ struct, storing run-time compiled versions of those key-value pairs,
hence making a map projection function call, pj_fwd(PJ, point), as easy
as a traditional function call like hypot(x,y).

While today, we may have more formally well defined metadata systems
(most prominent the OGC WKT representation), nothing comes close being
as easily readable ("human compatible") as Gerald's key-value system.
This system in particular, and the PROJ.4 system in general, was
Gerald's great gift to anyone using and/or communicating about geodata.

It is only reasonable to name a program keeping an eye on the integrity
of the PROJ.4 system in honour of Gerald. So in honour, and hopefully
also in the spirit, of Gerald Ian Evenden (1935--2016), this is the
Geospatial Integrity Investigation Environment.

************************************************************************

Thomas Knudsen, thokn@sdfe.dk, 2017-10-01/2017-10-08

************************************************************************

* Copyright (c) 2017 Thomas Knudsen
* Copyright (c) 2017, SDFE
*
* Permission is hereby granted, free of charge, to any person obtaining a
* copy of this software and associated documentation files (the "Software"),
* to deal in the Software without restriction, including without limitation
* the rights to use, copy, modify, merge, publish, distribute, sublicense,
* and/or sell copies of the Software, and to permit persons to whom the
* Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included
* in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
* OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
* DEALINGS IN THE SOFTWARE.

***********************************************************************/

#include "optargpm.h"

#include <proj.h>
#include "proj_internal.h"
#include "projects.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include <string.h>
#include <ctype.h>

#include <math.h>
#include <errno.h>



/* from proj_strtod.c */
double proj_strtod(const char *str, char **endptr);
double proj_atof(const char *str);

int   main(int argc, char **argv);

static int   process_file (char *fname);
static int   errmsg (int errlev, char *msg, ...);
static int   get_inp (FILE *f, char *inp, int size);
static int   get_cmnd (char *inp, char *cmnd, int len);
static char *get_args (char *inp);
static int   dispatch (char *cmnd, char *args);



#define SKIP -1

typedef struct {
    char operation[10000];
    PJ *P;
    PJ_COORD a, b, c, e;
    PJ_DIRECTION dir;
    int verbosity;
    int nargs;
    int op_id;
    int op_ok,    op_ko;
    int total_ok, total_ko;
    int grand_ok, grand_ko;
    double tolerance;
    char *curr_file;
    FILE *fout;
} gie_ctx;

gie_ctx T = {{""}, 0, {{0,0,0,0}}, {{0,0,0,0}}, {{0,0,0,0}}, {{0,0,0,0}}, PJ_FWD, 1,   0,   0,0,0,0,0,0,0,  0.0005, 0, 0};

OPTARGS *o;



size_t tol_lineno = 0;
size_t lineno = 0;
size_t level  = 0;
char delim[] = {"-------------------------------------------------------------------------------\n"};
char DELIM[] = {"===============================================================================\n"};


#define CMDLEN 25000

int nfiles = 0;


static const char usage[] = {
    "--------------------------------------------------------------------------------\n"
    "Usage: %s [-options]... infile...\n"
    "--------------------------------------------------------------------------------\n"
    "Options:\n"
    "--------------------------------------------------------------------------------\n"
    "    -h                Help: print this usage information\n"
    "    -o /path/to/file  Specify output file name\n"
    "    -v                Verbose: Provide non-essential informational output.\n"
    "                      Repeat -v for more verbosity (e.g. -vv)\n"
    "    -q                Quiet: Opposite of verbose. In quiet mode not even errors\n"
    "                      are reported. Only interaction is through the return code\n"
    "                      (0 on success, non-zero indicates number of FAILED tests)\n"
    "--------------------------------------------------------------------------------\n"
    "Long Options:\n"
    "--------------------------------------------------------------------------------\n"
    "    --output          Alias for -o\n"
    "    --verbose         Alias for -v\n"
    "    --help            Alias for -h\n"
    "--------------------------------------------------------------------------------\n"
    "Examples:\n"
    "--------------------------------------------------------------------------------\n"
    "1. Run all tests in file \"corner-cases.gie\", providing much extra information\n"
    "       gie -vvvv corner-cases.gie\n"
    "2. Run all tests in files \"foo\" and \"bar\", providing info on failures only\n"
    "       gie foo bar\n"
    "--------------------------------------------------------------------------------\n"
};

int main (int argc, char **argv) {
    int  i;
    const char *longflags[]  = {"v=verbose", "q=quiet", "h=help", 0};
    const char *longkeys[]   = {"o=output", 0};

    o = opt_parse (argc, argv, "hvq", "o", longflags, longkeys);
    if (0==o)
        return 0;

    if (opt_given (o, "h")) {
        printf (usage, o->progname);
        return 0;
    }



    T.verbosity = opt_given (o, "q");
    if (T.verbosity)
        T.verbosity = -1;
    if (T.verbosity != -1)
        T.verbosity = opt_given (o, "v") + 1;

    T.fout = stdout;
    if (opt_given (o, "o"))
        T.fout = fopen (opt_arg (o, "output"), "rt");
    if (0==T.fout) {
        fprintf (stderr, "%s: Cannot open '%s' for output\n", o->progname, opt_arg (o, "output"));
        free (o);
        return 1;
    }

    if (0==o->fargc) {
        if (T.verbosity==-1)
            return -1;
        fprintf (T.fout, "Nothing to do\n");
        return 0;
    }

    for (i = 0;  i < o->fargc; i++)
        process_file (o->fargv[i]);

    if (T.verbosity > 0) {
        if (o->fargc > 1)
        fprintf (T.fout, "%sGrand total: %d. Success: %d, Failure: %d\n", delim, T.grand_ok+T.grand_ko, T.grand_ok, T.grand_ko);
        fprintf (T.fout, "%s", delim);
    }
    else
        if (T.grand_ko)
            fprintf (T.fout, "Failures: %d", T.grand_ko);

    if (stdout != T.fout)
        fclose (T.fout);

    free (o);
    return T.grand_ko;
}





static int process_file (char *fname) {
    FILE *f;
    char inp[CMDLEN];
    char cmnd[1000];
    char *args;

    lineno = level = 0;
    T.op_ok = T.total_ok = 0;
    T.op_ko = T.total_ko = 0;

    f = fopen (fname, "rt");
    if (0==f) {
        if (T.verbosity > 0) {
            fprintf (T.fout, "%sCannot open spec'd input file '%s' - bye!\n", delim, fname);
            return 2;
        }
        errmsg (2, "Cannot open spec'd input file '%s' - bye!\n", fname);
    }
    if (T.verbosity > 0)
        fprintf (T.fout, "%sReading file '%s'\n", delim, fname);
    T.curr_file = fname;
    while (get_inp(f, inp, CMDLEN)) {
        int len;

        if (feof(f))
            break;
        len = get_cmnd (inp, cmnd, 1000);
        if (len>=999) {
            errmsg (2, "Command verb too long: '%s' - bye!\n", cmnd);
            proj_destroy (T.P);
            T.P = 0;
            return 0;

        }
        args = get_args (inp);
        if (SKIP==dispatch (cmnd, args))
            return proj_destroy (T.P), T.P = 0, 0;
    }
    fclose (f);

    T.grand_ok += T.total_ok;
    T.grand_ko += T.total_ko;
    if (T.verbosity > 0)
    fprintf (T.fout, "%stotal: %2d tests succeeded,  %2d tests %s\n", delim, T.total_ok, T.total_ko, T.total_ko? "FAILED!": "failed.");

    if (level==0)
        return errmsg (-3, "File '%s':Missing 'BEGIN' cmnd - bye!\n", fname);
    if (level && level%2)
        return errmsg (-4, "File '%s':Missing 'END' cmnd - bye!\n", fname);
    return 0;
}






static int banner (char *args) {
    char dots[] = {"..."}, nodots[] = {""}, *thedots = nodots;
    if (T.total_ko > 0 && T.op_ko==0)
        printf ("\n\n");
    if (strlen(args) > 70)
        thedots = dots;
        fprintf (T.fout, "%s%-70.70s%s\n", delim, args, thedots);
    return 0;
}






static int tolerance (char *args) {
    char *endp = args;
    T.tolerance = proj_strtod (endp, &endp);
    if (HUGE_VAL==T.tolerance) {
        T.tolerance = 0.0005;
        return 1;
    }
    while (isspace (*endp))
        endp++;

    if (0==strcmp(endp, "km"))
        T.tolerance *= 1000;
    else if (0==strcmp(endp, "m"))
        T.tolerance *= 1;
    else if (0==strcmp(endp, "dm"))
        T.tolerance /= 10;
    else if (0==strcmp(endp, "cm"))
        T.tolerance /= 100;
    else if (0==strcmp(endp, "mm"))
        T.tolerance /= 1000;
    else if (0==strcmp(endp, "um"))
        T.tolerance /= 1e6;
    else if (0==strcmp(endp, "nm"))
        T.tolerance /= 1e9;
    else
        T.tolerance /= 1000;  /* If no unit, assume mm */
    return 0;
}


static int direction (char *args) {
    char *endp = args;
    while (isspace (*endp))
        endp++;
    switch (*endp) {
        case 'F':
        case 'f':
        T.dir = PJ_FWD;
            break;
        case 'I':
        case 'i':
        case 'R':
        case 'r':
            T.dir = PJ_INV;
            break;
        default:
            return 1;
    }
    return 0;
}




static void finish_previous_operation (char *args) {
    if (T.verbosity > 1 && T.op_id > 1 && T.op_ok+T.op_ko)
        fprintf (T.fout, "%s     %d tests succeeded,  %d tests %s\n", delim, T.op_ok, T.op_ko, T.op_ko? "FAILED!": "failed.");
    (void) args;
}

static int operation (char *args) {
    T.op_id++;
    strcpy (&(T.operation[0]), args);
    if (T.verbosity > 1) {
        finish_previous_operation (args);
        banner (args);
    }
    /* if (0==T.op_ko)
        printf ("%d\n", (int) tol_lineno); */
    T.op_ok = 0;
    T.op_ko = 0;

    direction ("forward");
    tolerance ("0.5");

    if (T.P)
        proj_destroy (T.P);
    T.P = proj_create (0, args);
    if (0==T.P)
        return errmsg(3, "Invalid operation definition!\n");
    return 0;
}


static PJ_COORD torad_if_needed (PJ *P, PJ_DIRECTION dir, PJ_COORD a) {
    enum pj_io_units u = P->left;
    PJ_COORD c;
    if (dir==PJ_INV)
        u = P->right;
    if (u==PJ_IO_UNITS_CLASSIC || u==PJ_IO_UNITS_METERS)
        return a;

    c.lpz.lam = proj_torad (T.a.lpz.lam);
    c.lpz.phi = proj_torad (T.a.lpz.phi);
    c.v[2] = T.a.v[2];
    c.v[3] = T.a.v[3];

    return c;
}


static int accept (char *args) {
    int n, i;
    char *endp = args;
    T.a = proj_coord (0,0,0,0);
    n = 4;
    for (i = 0; i < 4; i++) {
        T.a.v[i] = proj_strtod (endp, &endp);
        if (HUGE_VAL==T.a.v[i]) {
            n--;
            T.a.v[i] = 0;
        }
    }
    T.a = torad_if_needed (T.P, T.dir, T.a);
    if (T.verbosity > 3)
        printf ("#  %s", args);
    return 0;
}



static int roundtrip (char *args) {
    int ntrips;
    double d, r, ans;
    char *endp;
    ans = proj_strtod (args, &endp);
    ntrips = (int) (ans==HUGE_VAL? 100: fabs(ans));
    d = proj_strtod (endp, &endp);
    d = d==HUGE_VAL?  T.tolerance:  d / 1000;
    r = proj_roundtrip (T.P, PJ_FWD, ntrips, T.a);
    if (r > d) {
        if (T.verbosity > -1) {
            if (0==T.op_ko && T.verbosity < 2)
                banner (T.operation);
            fprintf (T.fout, "%s", T.op_ko? "     -----\n": delim);
            fprintf (T.fout, "     FAILURE in %s(%d):\n", opt_strip_path (T.curr_file), (int) lineno);
            fprintf (T.fout, "     roundtrip deivation: %.3f mm, expected: %.3f mm\n", 1000*r, 1000*d);
        }
        T.op_ko++;
        T.total_ko++;
    }
    return 0;
}

static int expect (char *args) {
    double d;
    enum pj_io_units unit;
    char *endp = args;
    int i;

    T.e     =  proj_coord (0,0,0,0);
    T.b     =  proj_coord (0,0,0,0);
    T.nargs = 4;
    for (i = 0; i < 4; i++) {
        T.e.v[i] = proj_strtod (endp, &endp);
        if (HUGE_VAL==T.e.v[i]) {
            T.nargs--;
            T.e.v[i] = 0;
        }
    }
    T.e = torad_if_needed (T.P, T.dir==PJ_FWD? PJ_INV:PJ_FWD, T.e);

    T.b = proj_trans_coord (T.P, T.dir, T.a);
    T.b = torad_if_needed (T.P, T.dir==PJ_FWD? PJ_INV:PJ_FWD, T.b);

    if (T.nargs < 2) {
        T.op_ko++;
        T.total_ko++;
        if (T.verbosity > -1) {
            if (0==T.op_ko && T.verbosity < 2)
                banner (T.operation);
            fprintf (T.fout, "%s", T.op_ko? "     -----\n": delim);
            fprintf (T.fout, "     FAILURE in %s(%d):\n     Too few args: %s\n", opt_strip_path (T.curr_file), (int) lineno, args);
        }
     return 1;
    }

    unit = T.dir==PJ_FWD? T.P->right: T.P->left;
    if (PJ_IO_UNITS_CLASSIC==unit)
        unit = PJ_IO_UNITS_METERS;

    if (unit==PJ_IO_UNITS_RADIANS)
        d = proj_lp_dist (T.P, T.b.lp, T.e.lp);
    else
        d = proj_xyz_dist (T.b.xyz, T.e.xyz);

    if (d > T.tolerance) {
        if (d > 10)
            d = 9.999999;
        if (T.verbosity > -1) {
            if (0==T.op_ko && T.verbosity < 2)
                banner (T.operation);
            fprintf (T.fout, "%s", T.op_ko? "     -----\n": delim);

            fprintf (T.fout, "     FAILURE in %s(%d):\n", opt_strip_path (T.curr_file), (int) lineno);
            fprintf (T.fout, "     expected: %s\n", args);
            fprintf (T.fout, "     got:      %.9f   %.9f", T.b.xy.x,  T.b.xy.y);
            if (T.nargs > 2)
                fprintf (T.fout, "   %.9f", T.b.xyz.z);
            if (T.nargs > 3)
                fprintf (T.fout, "   %.9f", T.b.xyzt.t);
            fprintf (T.fout, "\n");
            fprintf (T.fout, "     deviation:  %.3f mm,  expected:  %.3f mm\n", 1000*d, 1000*T.tolerance);
        }
        T.op_ko++;
        T.total_ko++;
    }
    else {
        T.op_ok++;
        T.total_ok++;
    }
    return 0;
}






static int verbose (char *args) {
    int i = (int) proj_atof (args);

    /* if -q/--quiet flag has been given, we do nothing */
    if (T.verbosity < 0)
        return 0;

    if (strlen (args))
        T.verbosity = i;
    else
        T.verbosity++;
    return 0;
}

static int comment (char *args) {
    (void) args;
    return 0;
}


static int echo (char *args) {
    fprintf (T.fout, "%s\n", args);
    return 0;
}



static int dispatch (char *cmnd, char *args) {
    if  (0==level%2) {
        if (0==strcmp (cmnd, "BEGIN"))
           level++;
        return 0;
    }
    if  (0==strcmp (cmnd, "OPERATION")) return  operation (args);
    if  (0==strcmp (cmnd, "operation")) return  operation (args);
    if  (0==strcmp (cmnd, "ACCEPT"))    return  accept    (args);
    if  (0==strcmp (cmnd, "accept"))    return  accept    (args);
    if  (0==strcmp (cmnd, "EXPECT"))    return  expect    (args);
    if  (0==strcmp (cmnd, "expect"))    return  expect    (args);
    if  (0==strcmp (cmnd, "ROUNDTRIP")) return  roundtrip (args);
    if  (0==strcmp (cmnd, "roundtrip")) return  roundtrip (args);
    if  (0==strcmp (cmnd, "BANNER"))    return  banner    (args);
    if  (0==strcmp (cmnd, "banner"))    return  banner    (args);
    if  (0==strcmp (cmnd, "VERBOSE"))   return  verbose   (args);
    if  (0==strcmp (cmnd, "verbose"))   return  verbose   (args);
    if  (0==strcmp (cmnd, "DIRECTION")) return  direction (args);
    if  (0==strcmp (cmnd, "direction")) return  direction (args);
    if  (0==strcmp (cmnd, "TOLERANCE")) return  tolerance (args);
    if  (0==strcmp (cmnd, "tolerance")) return  tolerance (args);
    if  (0==strcmp (cmnd, "ECHO"))      return  echo      (args);
    if  (0==strcmp (cmnd, "echo"))      return  echo      (args);
    if  (0==strcmp  (cmnd, "END"))      return          finish_previous_operation (args), level++, 0;
    if  ('#'==cmnd[0])                  return  comment   (args);
    return 0;
}








static int errmsg (int errlev, char *msg, ...) {
    va_list args;
    va_start(args, msg);
    vfprintf(stdout, msg, args);
    va_end(args);
    if (errlev)
        errno = errlev;
    return errlev;
}

#define skipspace(f, c)                           \
    do {                                          \
        while (isspace (c=fgetc(f)) && !feof(f)){ \
            if ('\n'==c) lineno++;                \
        }                                         \
        if (feof(f))                              \
            break;                                \
    } while (ungetc(c, f), 0)

#define skipline(f, c)                            \
    do {                                          \
        while ((c=fgetc(f)) && !feof(f)) {        \
            if ((c=='\r') || (c=='\n'))           \
                break;                            \
        }                                         \
        skipspace (f, c);                         \
    } while (0)


/* skip whitespace at continuation line */
#define continuation(f, buf, c)                   \
    if ((c=='\r')||(c=='\n')) {                   \
        if (c=='\n') lineno++;                    \
        next--;                                   \
        while (isspace (c=fgetc(f)) && !feof(f)); \
    }

static int get_inp (FILE *f, char *inp, int size) {
    char *next;
    int c = 0, esc;
    char *endp = inp + size - 2;

    skipspace (f, c);

    for (c = esc = 0, next = inp; !feof(f); ) {
        c = fgetc(f);
        if (esc) {
            continuation (f, next, c);
            esc = 0;
            /* handle escape sequences here */
            switch (c) {
                case '\\': c = '\\'; break;
                default: (void) c;
            }
        }
        if (c=='\r')
            break;
        if (c=='\n') {
            lineno++;
            break;
        }

        *next++ = (char) c;
        if ('\\'==c)
            esc = 1;
        if (feof(f) || (next==endp))
            break;
    }
    *(next) = 0;
    return (int) strlen(inp);
}

static int get_cmnd (char *inp, char *cmnd, int len) {
    cmnd[0] = 0;
    while (isspace(*inp++));
    inp--;
    while (len-- && !isspace(*inp) && *inp)
        *cmnd++ = *inp++;
    *cmnd = 0;
    return len;
}

static char *get_args (char *inp) {
    char *args = inp;
    while (isspace(*args++))
        if (0==*args)
            return args;
    while (!isspace(*++args))
        if (0==*args)
            return args;
    while (isspace(*args++))
        if (0==*args)
            return args;
    return --args;
}
