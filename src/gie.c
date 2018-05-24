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

operation,     which defines the PROJ.4 operation to test,
accept,        which defines the input coordinate to read, and
expect,        which defines the result to expect.

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
'tolerance' command verb (and yes, I know, linguistically speaking, both
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

And hence, according to Gerald, *they should not be particularly much
harder to use*, for a programmer, than the sin()s, tan()s and hypot()s
so readily available.

Gerald's ingenuity also showed in the implementation of the vision,
where he devised a comprehensive, yet simple, system of key-value
pairs for parameterising a map projection, and the highly flexible
PJ struct, storing run-time compiled versions of those key-value pairs,
hence making a map projection function call, pj_fwd(PJ, point), as easy
as a traditional function call like hypot(x,y).

While today, we may have more formally well defined metadata systems
(most prominent the OGC WKT2 representation), nothing comes close being
as easily readable ("human compatible") as Gerald's key-value system.
This system in particular, and the PROJ.4 system in general, was
Gerald's great gift to anyone using and/or communicating about geodata.

It is only reasonable to name a program, keeping an eye on the integrity
of the PROJ.4 system, in honour of Gerald.

So in honour, and hopefully also in the spirit, of Gerald Ian Evenden
(1935--2016), this is the Geospatial Integrity Investigation Environment.

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

#include <ctype.h>
#include <errno.h>
#include <math.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "proj.h"
#include "proj_internal.h"
#include "proj_math.h"
#include "proj_strtod.h"
#include "projects.h"

#include "optargpm.h"

/* Package for flexible format I/O - ffio */
typedef struct ffio {
    FILE *f;
    const char **tags;
    const char *tag;
    char *args;
    char *next_args;
    size_t n_tags;
    size_t args_size;
    size_t next_args_size;
    size_t argc;
    size_t lineno, next_lineno;
    size_t level;
}  ffio;

static int get_inp (ffio *G);
static int skip_to_next_tag (ffio *G);
static int step_into_gie_block (ffio *G);
static int locate_tag (ffio *G, const char *tag);
static int nextline (ffio *G);
static int at_end_delimiter (ffio *G);
static const char *at_tag (ffio *G);
static int at_decorative_element (ffio *G);
static ffio *ffio_destroy (ffio *G);
static ffio *ffio_create (const char **tags, size_t n_tags, size_t max_record_size);

static const char *gie_tags[] = {
    "<gie>", "operation", "accept", "expect", "roundtrip", "banner", "verbose",
    "direction", "tolerance", "ignore", "builtins", "echo", "skip", "</gie>"
};

static const size_t n_gie_tags = sizeof gie_tags / sizeof gie_tags[0];


int   main(int argc, char **argv);

static int   dispatch (const char *cmnd, const char *args);
static int   errmsg (int errlev, const char *msg, ...);
static int   errno_from_err_const (const char *err_const);
static int   list_err_codes (void);
static int   process_file (const char *fname);

static const char *column (const char *buf, int n);
static const char *err_const_from_errno (int err);



#define SKIP -1

typedef struct {
    char operation[10000];
    PJ *P;
    PJ_COORD a, b, c, e;
    PJ_DIRECTION dir;
    int verbosity;
    int skip;
    int op_id;
    int op_ok,    op_ko,    op_skip;
    int total_ok, total_ko, total_skip;
    int grand_ok, grand_ko, grand_skip;
    size_t operation_lineno;
    size_t dimensions_given, dimensions_given_at_last_accept;
    double tolerance;
    int ignore;
    const char *curr_file;
    FILE *fout;
} gie_ctx;

ffio *F = 0;

static gie_ctx T;
int tests=0, succs=0, succ_fails=0, fail_fails=0, succ_rtps=0, fail_rtps=0;
int succ_builtins=0, fail_builtins=0;


static const char delim[] = {"-------------------------------------------------------------------------------\n"};


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
    "    -l                List the PROJ internal system error codes\n"
    "--------------------------------------------------------------------------------\n"
    "Long Options:\n"
    "--------------------------------------------------------------------------------\n"
    "    --output          Alias for -o\n"
    "    --verbose         Alias for -v\n"
    "    --help            Alias for -h\n"
    "    --list            Alias for -l\n"
    "    --version         Print version number\n"
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
    const char *longflags[]  = {"v=verbose", "q=quiet", "h=help", "l=list", "version", 0};
    const char *longkeys[]   = {"o=output", 0};
    OPTARGS *o;

    memset (&T, 0, sizeof (T));
    T.dir = PJ_FWD;
    T.verbosity = 1;
    T.tolerance = 5e-4;
    T.ignore = 5555; /* Error code that will not be issued by proj_create() */

    o = opt_parse (argc, argv, "hlvq", "o", longflags, longkeys);
    if (0==o)
        return 0;

    if (opt_given (o, "h") || argc==1) {
        printf (usage, o->progname);
        free (o);
        return 0;
    }


    if (opt_given (o, "version")) {
        fprintf (stdout, "%s: %s\n", o->progname, pj_get_release ());
        free (o);
        return 0;
    }


    if (opt_given (o, "l")) {
        free (o);
        return list_err_codes ();
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
        free (o);
        return 0;
    }

    F = ffio_create (gie_tags, n_gie_tags, 1000);
    if (0==F) {
        fprintf (stderr, "%s: No memory\n", o->progname);
        free (o);
        return 1;
    }

    for (i = 0;  i < o->fargc;  i++)
        process_file (o->fargv[i]);

    if (T.verbosity > 0) {
        if (o->fargc > 1) {
            fprintf (T.fout, "%sGrand total: %d. Success: %d, Skipped: %d, Failure: %d\n",
                     delim, T.grand_ok+T.grand_ko+T.grand_skip, T.grand_ok, T.grand_skip,
                     T.grand_ko);
        }
        fprintf (T.fout, "%s", delim);
        if (T.verbosity > 1) {
            fprintf (T.fout, "Failing roundtrips: %4d,    Succeeding roundtrips: %4d\n", fail_rtps, succ_rtps);
            fprintf (T.fout, "Failing failures:   %4d,    Succeeding failures:   %4d\n", fail_fails, succ_fails);
            fprintf (T.fout, "Failing builtins:   %4d,    Succeeding builtins:   %4d\n", fail_builtins, succ_builtins);
            fprintf (T.fout, "Internal counters:                            %4.4d(%4.4d)\n", tests, succs);
            fprintf (T.fout, "%s", delim);
        }
    }
    else
        if (T.grand_ko)
            fprintf (T.fout, "Failures: %d", T.grand_ko);

    if (stdout != T.fout)
        fclose (T.fout);

    free (o);
    ffio_destroy (F);
    return T.grand_ko;
}

static int another_failure (void) {
    T.op_ko++;
    T.total_ko++;
    proj_errno_reset (T.P);
    return 0;
}

static int another_skip (void) {
    T.op_skip++;
    T.total_skip++;
    return 0;
}

static int another_success (void) {
    T.op_ok++;
    T.total_ok++;
    proj_errno_reset (T.P);
    return 0;
}

static int another_succeeding_failure (void) {
    succ_fails++;
    return another_success ();
}

static int another_failing_failure (void) {
    fail_fails++;
    return another_failure ();
}

static int another_succeeding_roundtrip (void) {
    succ_rtps++;
    return another_success ();
}

static int another_failing_roundtrip (void) {
    fail_rtps++;
    return another_failure ();
}

static int another_succeeding_builtin (void) {
    succ_builtins++;
    return another_success ();
}

static int another_failing_builtin (void) {
    fail_builtins++;
    return another_failure ();
}


static int process_file (const char *fname) {
    FILE *f;

    F->lineno = F->next_lineno = F->level = 0;
    T.op_ok = T.total_ok = 0;
    T.op_ko = T.total_ko = 0;
    T.op_skip = T.total_skip = 0;

    if (T.skip)
        return proj_destroy (T.P), T.P = 0, 0;

    f = fopen (fname, "rt");
    if (0==f) {
        if (T.verbosity > 0) {
            fprintf (T.fout, "%sCannot open spec'd input file '%s' - bye!\n", delim, fname);
            return 2;
        }
        errmsg (2, "Cannot open spec'd input file '%s' - bye!\n", fname);
    }
    F->f = f;

    if (T.verbosity > 0)
        fprintf (T.fout, "%sReading file '%s'\n", delim, fname);
    T.curr_file = fname;

    while (get_inp(F)) {
        if (SKIP==dispatch (F->tag, F->args))
            return proj_destroy (T.P), T.P = 0, 0;
    }

    fclose (f);
    F->lineno = F->next_lineno = 0;

    T.grand_ok   += T.total_ok;
    T.grand_ko   += T.total_ko;
    T.grand_skip += T.grand_skip;
    if (T.verbosity > 0) {
        fprintf (T.fout, "%stotal: %2d tests succeeded, %2d tests skipped, %2d tests %s\n",
                 delim, T.total_ok, T.total_skip, T.total_ko,
                 T.total_ko? "FAILED!": "failed.");
    }
    if (F->level==0)
        return errmsg (-3, "File '%s':Missing '<gie>' cmnd - bye!\n", fname);
    if (F->level && F->level%2)
        return errmsg (-4, "File '%s':Missing '</gie>' cmnd - bye!\n", fname);
    return 0;
}


/*****************************************************************************/
const char *column (const char *buf, int n) {
/*****************************************************************************
Return a pointer to the n'th column of buf. Column numbers start at 0.
******************************************************************************/
    int i;
    if (n <= 0)
        return buf;
    for (i = 0;  i < n;  i++) {
        while (isspace(*buf))
            buf++;
        if (i == n - 1)
            break;
        while ((0 != *buf) && !isspace(*buf))
            buf++;
    }
    return buf;
}


/*****************************************************************************/
static double strtod_scaled (const char *args, double default_scale) {
/*****************************************************************************
Interpret <args> as a numeric followed by a linear decadal prefix.
Return the properly scaled numeric
******************************************************************************/
    double s;
    const double GRS80_DEG = 111319.4908; /* deg-to-m at equator of GRS80 */
    const char *endp = args;
    s = proj_strtod (args, (char **) &endp);
    if (args==endp)
        return HUGE_VAL;

    endp = column (args, 2);

    if (0==strcmp(endp, "km"))
        s *= 1000;
    else if (0==strcmp(endp, "m"))
        s *= 1;
    else if (0==strcmp(endp, "dm"))
        s /= 10;
    else if (0==strcmp(endp, "cm"))
        s /= 100;
    else if (0==strcmp(endp, "mm"))
        s /= 1000;
    else if (0==strcmp(endp, "um"))
        s /= 1e6;
    else if (0==strcmp(endp, "nm"))
        s /= 1e9;
    else if (0==strcmp(endp, "rad"))
        s = GRS80_DEG * proj_todeg (s);
    else if (0==strcmp(endp, "deg"))
        s = GRS80_DEG * s;
    else
        s *= default_scale;
    return s;
}


static int banner (const char *args) {
    char dots[] = {"..."}, nodots[] = {""}, *thedots = nodots;
    if (strlen(args) > 70)
        thedots = dots;
    fprintf (T.fout, "%s%-70.70s%s\n", delim, args, thedots);
    return 0;
}


static int tolerance (const char *args) {
    T.tolerance = strtod_scaled (args, 1);
    if (HUGE_VAL==T.tolerance) {
        T.tolerance = 0.0005;
        return 1;
    }
    return 0;
}

static int ignore (const char *args) {
    T.ignore = errno_from_err_const (column (args, 1));
    return 0;
}

static int direction (const char *args) {
    const char *endp = args;
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


static void finish_previous_operation (const char *args) {
    if (T.verbosity > 1 && T.op_id > 1 && T.op_ok+T.op_ko)
        fprintf (T.fout, "%s     %d tests succeeded,  %d tests skipped, %d tests %s\n",
                 delim, T.op_ok, T.op_skip, T.op_ko, T.op_ko? "FAILED!": "failed.");
    (void) args;
}



/*****************************************************************************/
static int operation (char *args) {
/*****************************************************************************
Define the operation to apply to the input data (in ISO 19100 lingo,
an operation is the general term describing something that can be
either a conversion or a transformation)
******************************************************************************/
    T.op_id++;

    T.operation_lineno = F->lineno;

    strcpy (&(T.operation[0]), F->args);

    if (T.verbosity > 1) {
        finish_previous_operation (F->args);
        banner (args);
    }


    T.op_ok = 0;
    T.op_ko = 0;
    T.op_skip = 0;

    direction ("forward");
    tolerance ("0.5 mm");
    ignore ("pjd_err_dont_skip");

    proj_errno_reset (T.P);

    if (T.P)
        proj_destroy (T.P);
    proj_errno_reset (0);

    T.P = proj_create (0, F->args);

    /* Checking that proj_create succeeds is first done at "expect" time, */
    /* since we want to support "expect"ing specific error codes */

    return 0;
}




static int unitconvert_selftest (void);
static int cart_selftest (void);
static int horner_selftest (void);

/*****************************************************************************/
static int builtins (const char *args) {
/*****************************************************************************
There are still a few tests that cannot be described using gie
primitives. Instead, they are implemented as builtins, and invoked
using the "builtins" command verb.
******************************************************************************/
    int i;
    if (T.verbosity > 1) {
        finish_previous_operation (args);
        banner ("builtins: unitconvert, horner, cart");
    }
    T.op_ok = 0;
    T.op_ko = 0;
    T.op_skip = 0;
    i = unitconvert_selftest ();
    if (i!=0) {
        fprintf (T.fout, "unitconvert_selftest fails with %d\n", i);
        another_failing_builtin();
    }
    else
        another_succeeding_builtin ();


    i = cart_selftest ();
    if (i!=0) {
        fprintf (T.fout, "cart_selftest fails with %d\n", i);
        another_failing_builtin();
    }
    else
        another_succeeding_builtin ();

    i = horner_selftest ();
    if (i!=0) {
        fprintf (T.fout, "horner_selftest fails with %d\n", i);
        another_failing_builtin();
    }
    else
        another_succeeding_builtin ();

    return 0;
}


static PJ_COORD torad_coord (PJ *P, PJ_DIRECTION dir, PJ_COORD a) {
    size_t i, n;
    char *axis = "enut";
    paralist *l = pj_param_exists (P->params, "axis");
    if (l && dir==PJ_INV)
        axis = l->param + strlen ("axis=");
    for (i = 0,  n = strlen (axis);  i < n;  i++)
        if (strchr ("news", axis[i]))
            a.v[i] = proj_torad (a.v[i]);
    return a;
}


static PJ_COORD todeg_coord (PJ *P, PJ_DIRECTION dir, PJ_COORD a) {
    size_t i, n;
    char *axis = "enut";
    paralist *l = pj_param_exists (P->params, "axis");
    if (l && dir==PJ_FWD)
        axis = l->param + strlen ("axis=");
    for (i = 0,  n = strlen (axis);  i < n;  i++)
        if (strchr ("news", axis[i]))
            a.v[i] = proj_todeg (a.v[i]);
    return a;
}



/*****************************************************************************/
static PJ_COORD parse_coord (const char *args) {
/*****************************************************************************
Attempt to interpret args as a PJ_COORD.
******************************************************************************/
    int i;
    const char *endp, *prev = args;
    PJ_COORD a = proj_coord (0,0,0,0);

    for (i = 0, T.dimensions_given = 0;   i < 4;   i++, T.dimensions_given++) {
        double d = proj_strtod (prev,  (char **) &endp);

        /* Break out if there were no more numerals */
        if (prev==endp)
            return i > 1? a: proj_coord_error ();

        a.v[i] = d;
        prev = endp;
    }

    return a;
}


/*****************************************************************************/
static int accept (const char *args) {
/*****************************************************************************
Read ("ACCEPT") a 2, 3, or 4 dimensional input coordinate.
******************************************************************************/
    T.a = parse_coord (args);
    if (T.verbosity > 3)
        printf ("#  %s\n", args);
    T.dimensions_given_at_last_accept = T.dimensions_given;
    return 0;
}


/*****************************************************************************/
static int roundtrip (const char *args) {
/*****************************************************************************
Check how far we go from the ACCEPTed point when doing successive
back/forward transformation pairs.

Without args, roundtrip defaults to 100 iterations:

  roundtrip

With one arg, roundtrip will default to a tolerance of T.tolerance:

  roundtrip ntrips

With two args:

  roundtrip ntrips tolerance

Always returns 0.
******************************************************************************/
    int ntrips;
    double d, r, ans;
    char *endp;
    PJ_COORD coo;

    if (0==T.P) {
        if (T.ignore == proj_errno(T.P))
            return another_skip();

        return another_failure ();
    }

    ans = proj_strtod (args, &endp);
    if (endp==args) {
        /* Default to 100 iterations if not args. */
        ntrips = 100;
    } else {
        if (ans < 1.0 || ans > 1000000.0) {
            errmsg (2, "Invalid number of roundtrips: %lf\n", ans);
            return another_failing_roundtrip ();
        }
        ntrips = (int)ans;
    }

    d = strtod_scaled (endp, 1);
    d = d==HUGE_VAL?  T.tolerance:  d;

    /* input ("accepted") values - probably in degrees */
    coo = proj_angular_input  (T.P, T.dir)? torad_coord (T.P, T.dir, T.a):  T.a;

    r = proj_roundtrip (T.P, T.dir, ntrips, &coo);
    if (r <= d)
        return another_succeeding_roundtrip ();

    if (T.verbosity > -1) {
        if (0==T.op_ko && T.verbosity < 2)
            banner (T.operation);
        fprintf (T.fout, "%s", T.op_ko? "     -----\n": delim);
        fprintf (T.fout, "     FAILURE in %s(%d):\n", opt_strip_path (T.curr_file), (int) F->lineno);
        fprintf (T.fout, "     roundtrip deviation: %.6f mm, expected: %.6f mm\n", 1000*r, 1000*d);
    }
    return another_failing_roundtrip ();
}


static int expect_message (double d, const char *args) {
    another_failure ();

    if (T.verbosity < 0)
        return 1;
    if (d > 1e6)
        d = 999999.999999;
    if (0==T.op_ko && T.verbosity < 2)
        banner (T.operation);
    fprintf (T.fout, "%s", T.op_ko? "     -----\n": delim);

    fprintf (T.fout, "     FAILURE in %s(%d):\n", opt_strip_path (T.curr_file), (int) F->lineno);
    fprintf (T.fout, "     expected: %s\n", args);
    fprintf (T.fout, "     got:      %.12f   %.12f", T.b.xy.x,  T.b.xy.y);
    if (T.b.xyzt.t!=0 || T.b.xyzt.z!=0)
        fprintf (T.fout, "   %.9f", T.b.xyz.z);
    if (T.b.xyzt.t!=0)
        fprintf (T.fout, "   %.9f", T.b.xyzt.t);
    fprintf (T.fout, "\n");
    fprintf (T.fout, "     deviation:  %.6f mm,  expected:  %.6f mm\n", 1000*d, 1000*T.tolerance);
    return 1;
}


static int expect_message_cannot_parse (const char *args) {
    another_failure ();
    if (T.verbosity > -1) {
        if (0==T.op_ko && T.verbosity < 2)
            banner (T.operation);
        fprintf (T.fout, "%s", T.op_ko? "     -----\n": delim);
        fprintf (T.fout, "     FAILURE in %s(%d):\n     Too few args: %s\n", opt_strip_path (T.curr_file), (int) F->lineno, args);
    }
    return 1;
}

static int expect_failure_with_errno_message (int expected, int got) {
    another_failing_failure ();

    if (T.verbosity < 0)
        return 1;
    if (0==T.op_ko && T.verbosity < 2)
        banner (T.operation);
    fprintf (T.fout, "%s", T.op_ko? "     -----\n": delim);
    fprintf (T.fout, "     FAILURE in %s(%d):\n",    opt_strip_path (T.curr_file), (int) F->lineno);
    fprintf (T.fout, "     got errno %s (%d): %s\n", err_const_from_errno(got), got,  pj_strerrno (got));
    fprintf (T.fout, "     expected %s (%d):  %s",   err_const_from_errno(expected), expected, pj_strerrno (expected));
    fprintf (T.fout, "\n");
    return 1;
}


/* For test purposes, we want to call a transformation of the same */
/* dimensionality as the number of dimensions given in accept */
static PJ_COORD expect_trans_n_dim (PJ_COORD ci) {
    if (4==T.dimensions_given_at_last_accept)
        return proj_trans (T.P, T.dir, ci);

    if (3==T.dimensions_given_at_last_accept)
        return pj_approx_3D_trans (T.P, T.dir, ci);

    return pj_approx_2D_trans (T.P, T.dir, ci);
}


/*****************************************************************************/
static int expect (const char *args) {
/*****************************************************************************
Tell GIE what to expect, when transforming the ACCEPTed input
******************************************************************************/
    PJ_COORD ci, co, ce;
    double d;
    int expect_failure = 0;
    int expect_failure_with_errno = 0;

    if (0==strncmp (args, "failure", 7)) {
        expect_failure = 1;

        /* Option: Fail with an expected errno (syntax: expect failure errno -33) */
        if (0==strncmp (column (args, 2), "errno", 5))
            expect_failure_with_errno = errno_from_err_const (column (args, 3));
    }

    if (T.ignore==proj_errno(T.P))
        return another_skip ();

    if (0==T.P) {
        /* If we expect failure, and fail, then it's a success... */
        if (expect_failure) {
            /* Failed to fail correctly? */
            if (expect_failure_with_errno && proj_errno (T.P)!=expect_failure_with_errno)
                return expect_failure_with_errno_message (expect_failure_with_errno, proj_errno(T.P));

            return another_succeeding_failure ();
        }

        /* Otherwise, it's a true failure */
        banner (T.operation);
        errmsg (3, "%sInvalid operation definition in line no. %d:\n       %s (errno=%s/%d)\n",
            delim, (int) T.operation_lineno, pj_strerrno(proj_errno(T.P)),
            err_const_from_errno (proj_errno(T.P)), proj_errno(T.P)
        );
        return another_failing_failure ();
    }

    /* We may still successfully fail even if the proj_create succeeded */
    if (expect_failure) {
        proj_errno_reset (T.P);

        /* Try to carry out the operation - and expect failure */
        ci = proj_angular_input (T.P, T.dir)? torad_coord (T.P, T.dir, T.a): T.a;
        co = expect_trans_n_dim (ci);

        if (expect_failure_with_errno) {
            if (proj_errno (T.P)==expect_failure_with_errno)
                return another_succeeding_failure ();
            printf ("errno=%d, expected=%d\n", proj_errno (T.P), expect_failure_with_errno);
            return another_failing_failure ();
        }


        /* Succeeded in failing? - that's a success */
        if (co.xyz.x==HUGE_VAL)
            return another_succeeding_failure ();

        /* Failed to fail? - that's a failure */
        banner (T.operation);
        errmsg (3, "%sFailed to fail. Operation definition in line no. %d\n",
            delim, (int) T.operation_lineno
        );
        return another_failing_failure ();
    }


    if (T.verbosity > 3) {
        puts (T.P->inverted? "INVERTED": "NOT INVERTED");
        puts (T.dir== 1? "forward": "reverse");
        puts (proj_angular_input (T.P, T.dir)?  "angular in":  "linear in");
        puts (proj_angular_output (T.P, T.dir)? "angular out": "linear out");
        printf ("left: %d   right:  %d\n", T.P->left, T.P->right);
    }

    tests++;
    T.e  =  parse_coord (args);
    if (HUGE_VAL==T.e.v[0])
        return expect_message_cannot_parse (args);


    /* expected angular values, probably in degrees */
    ce = proj_angular_output (T.P, T.dir)? torad_coord (T.P, T.dir, T.e): T.e;
    if (T.verbosity > 3)
        printf ("EXPECTS  %.12f  %.12f  %.12f  %.12f\n", ce.v[0],ce.v[1],ce.v[2],ce.v[3]);

    /* input ("accepted") values, also probably in degrees */
    ci = proj_angular_input (T.P, T.dir)? torad_coord (T.P, T.dir, T.a): T.a;
    if (T.verbosity > 3)
        printf ("ACCEPTS  %.12f  %.12f  %.12f  %.12f\n", ci.v[0],ci.v[1],ci.v[2],ci.v[3]);

    /* do the transformation, but mask off dimensions not given in expect-ation */
    co = expect_trans_n_dim (ci);
    if (T.dimensions_given < 4)
        co.v[3] = 0;
    if (T.dimensions_given < 3)
        co.v[2] = 0;

    /* angular output from proj_trans comes in radians */
    T.b = proj_angular_output (T.P, T.dir)? todeg_coord (T.P, T.dir, co): co;
    if (T.verbosity > 3)
        printf ("GOT      %.12f  %.12f  %.12f  %.12f\n", co.v[0],co.v[1],co.v[2],co.v[3]);

#if 0
    /* We need to handle unusual axis orders - that'll be an item for version 5.1 */
    if (T.P->axisswap) {
        ce = proj_trans (T.P->axisswap, T.dir, ce);
        co = proj_trans (T.P->axisswap, T.dir, co);
    }
#endif
    if (proj_angular_output (T.P, T.dir))
        d = proj_lpz_dist (T.P, ce, co);
    else
        d = proj_xyz_dist (co, ce);

    if (d > T.tolerance)
        return expect_message (d, args);
    succs++;

    another_success ();
    return 0;
}



/*****************************************************************************/
static int verbose (const char *args) {
/*****************************************************************************
Tell the system how noisy it should be
******************************************************************************/
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


/*****************************************************************************/
static int echo (const char *args) {
/*****************************************************************************
Add user defined noise to the output stream
******************************************************************************/
fprintf (T.fout, "%s\n", args);
    return 0;
}



/*****************************************************************************/
static int skip (const char *args) {
/*****************************************************************************
Indicate that the remaining material should be skipped. Mostly for debugging.
******************************************************************************/
    T.skip = 1;
    (void) args;
    F->level = 2; /* Silence complaints about missing </gie> element */
    return 0;
}


static int dispatch (const char *cmnd, const char *args) {
    if (T.skip)
        return SKIP;
    if  (0==strcmp (cmnd, "operation")) return  operation ((char *) args);
    if  (0==strcmp (cmnd, "accept"))    return  accept    (args);
    if  (0==strcmp (cmnd, "expect"))    return  expect    (args);
    if  (0==strcmp (cmnd, "roundtrip")) return  roundtrip (args);
    if  (0==strcmp (cmnd, "banner"))    return  banner    (args);
    if  (0==strcmp (cmnd, "verbose"))   return  verbose   (args);
    if  (0==strcmp (cmnd, "direction")) return  direction (args);
    if  (0==strcmp (cmnd, "tolerance")) return  tolerance (args);
    if  (0==strcmp (cmnd, "ignore"))    return  ignore    (args);
    if  (0==strcmp (cmnd, "builtins"))  return  builtins  (args);
    if  (0==strcmp (cmnd, "echo"))      return  echo      (args);
    if  (0==strcmp (cmnd, "skip"))      return  skip      (args);

    return 0;
}




struct errno_vs_err_const {const char *the_err_const; int the_errno;};
static const struct errno_vs_err_const lookup[] = {
    {"pjd_err_no_args"                  ,  -1},
    {"pjd_err_no_option_in_init_file"   ,  -2},
    {"pjd_err_no_colon_in_init_string"  ,  -3},
    {"pjd_err_proj_not_named"           ,  -4},
    {"pjd_err_unknown_projection_id"    ,  -5},
    {"pjd_err_eccentricity_is_one"      ,  -6},
    {"pjd_err_unknown_unit_id"          ,  -7},
    {"pjd_err_invalid_boolean_param"    ,  -8},
    {"pjd_err_unknown_ellp_param"       ,  -9},
    {"pjd_err_rev_flattening_is_zero"   ,  -10},
    {"pjd_err_ref_rad_larger_than_90"   ,  -11},
    {"pjd_err_es_less_than_zero"        ,  -12},
    {"pjd_err_major_axis_not_given"     ,  -13},
    {"pjd_err_lat_or_lon_exceed_limit"  ,  -14},
    {"pjd_err_invalid_x_or_y"           ,  -15},
    {"pjd_err_wrong_format_dms_value"   ,  -16},
    {"pjd_err_non_conv_inv_meri_dist"   ,  -17},
    {"pjd_err_non_con_inv_phi2"         ,  -18},
    {"pjd_err_acos_asin_arg_too_large"  ,  -19},
    {"pjd_err_tolerance_condition"      ,  -20},
    {"pjd_err_conic_lat_equal"          ,  -21},
    {"pjd_err_lat_larger_than_90"       ,  -22},
    {"pjd_err_lat1_is_zero"             ,  -23},
    {"pjd_err_lat_ts_larger_than_90"    ,  -24},
    {"pjd_err_control_point_no_dist"    ,  -25},
    {"pjd_err_no_rotation_proj"         ,  -26},
    {"pjd_err_w_or_m_zero_or_less"      ,  -27},
    {"pjd_err_lsat_not_in_range"        ,  -28},
    {"pjd_err_path_not_in_range"        ,  -29},
    {"pjd_err_h_less_than_zero"         ,  -30},
    {"pjd_err_k_less_than_zero"         ,  -31},
    {"pjd_err_lat_1_or_2_zero_or_90"    ,  -32},
    {"pjd_err_lat_0_or_alpha_eq_90"     ,  -33},
    {"pjd_err_ellipsoid_use_required"   ,  -34},
    {"pjd_err_invalid_utm_zone"         ,  -35},
    {"pjd_err_tcheby_val_out_of_range"  ,  -36},
    {"pjd_err_failed_to_find_proj"      ,  -37},
    {"pjd_err_failed_to_load_grid"      ,  -38},
    {"pjd_err_invalid_m_or_n"           ,  -39},
    {"pjd_err_n_out_of_range"           ,  -40},
    {"pjd_err_lat_1_2_unspecified"      ,  -41},
    {"pjd_err_abs_lat1_eq_abs_lat2"     ,  -42},
    {"pjd_err_lat_0_half_pi_from_mean"  ,  -43},
    {"pjd_err_unparseable_cs_def"       ,  -44},
    {"pjd_err_geocentric"               ,  -45},
    {"pjd_err_unknown_prime_meridian"   ,  -46},
    {"pjd_err_axis"                     ,  -47},
    {"pjd_err_grid_area"                ,  -48},
    {"pjd_err_invalid_sweep_axis"       ,  -49},
    {"pjd_err_malformed_pipeline"       ,  -50},
    {"pjd_err_unit_factor_less_than_0"  ,  -51},
    {"pjd_err_invalid_scale"            ,  -52},
    {"pjd_err_non_convergent"           ,  -53},
    {"pjd_err_missing_args"             ,  -54},
    {"pjd_err_lat_0_is_zero"            ,  -55},
    {"pjd_err_ellipsoidal_unsupported"  ,  -56},
    {"pjd_err_too_many_inits"           ,  -57},
    {"pjd_err_invalid_arg"              ,  -58},
    {"pjd_err_dont_skip"                ,  5555},
    {"pjd_err_unknown"                  ,  9999},
    {"pjd_err_enomem"                   ,  ENOMEM},
};

static const struct errno_vs_err_const unknown = {"PJD_ERR_UNKNOWN", 9999};


static int list_err_codes (void) {
    int i;
    const int n = sizeof lookup / sizeof lookup[0];

    for (i = 0;  i < n;  i++) {
        if (9999==lookup[i].the_errno)
            break;
        printf ("%25s  (%2.2d):  %s\n", lookup[i].the_err_const + 8, lookup[i].the_errno, pj_strerrno(lookup[i].the_errno));
    }
    return 0;
}


static const char *err_const_from_errno (int err) {
    size_t i;
    const size_t n = sizeof lookup / sizeof lookup[0];

    for (i = 0;  i < n;  i++) {
        if (err==lookup[i].the_errno)
            return lookup[i].the_err_const + 8;
    }
    return unknown.the_err_const;
}


static int errno_from_err_const (const char *err_const) {
    const size_t n = sizeof lookup / sizeof lookup[0];
    size_t i, len;
    int ret;
    char tolower_err_const[100];

    /* Make a lower case copy for matching */
    for (i = 0;  i < 99; i++) {
        if (0==err_const[i] || isspace (err_const[i]))
             break;
        tolower_err_const[i] = (char) tolower (err_const[i]);
    }
    tolower_err_const[i] = 0;

    /* If it looks numeric, return that numeric */
    ret = (int) pj_atof (err_const);
    if (0!=ret)
        return ret;

    /* Else try to find a matching identifier */
    len = strlen (tolower_err_const);

    /* First try to find a match excluding the PJD_ERR_ prefix */
    for (i = 0;  i < n;  i++) {
        if (0==strncmp (lookup[i].the_err_const + 8, err_const, len))
            return lookup[i].the_errno;
    }

    /* If that did not work, try with the full name */
    for (i = 0;  i < n;  i++) {
        if (0==strncmp (lookup[i].the_err_const, err_const, len))
            return lookup[i].the_errno;
    }

    /* On failure, return something unlikely */
    return 9999;
}


static int errmsg (int errlev, const char *msg, ...) {
    va_list args;
    va_start(args, msg);
    vfprintf(stdout, msg, args);
    va_end(args);
    if (errlev)
        errno = errlev;
    return errlev;
}








/****************************************************************************************

FFIO - Flexible format I/O

FFIO provides functionality for reading proj style instruction strings written
in a less strict format than usual:

*  Whitespace is generally allowed everywhere
*  Comments can be written inline, '#' style
*  ... or as free format blocks

The overall mission of FFIO is to facilitate communications of geodetic
parameters and test material in a format that is highly human readable,
and provides ample room for comment, documentation, and test material.

See the PROJ ".gie" test suites for examples of supported formatting.

****************************************************************************************/


/***************************************************************************************/
static ffio *ffio_create (const char **tags, size_t n_tags, size_t max_record_size) {
/****************************************************************************************
Constructor for the ffio object.
****************************************************************************************/
    ffio *G = calloc (1, sizeof (ffio));
    if (0==G)
        return 0;

    if (0==max_record_size)
        max_record_size = 1000;

    G->args = calloc (1, 5*max_record_size);
    if (0==G->args) {
        free (G);
        return 0;
    }

    G->next_args = calloc (1, max_record_size);
    if (0==G->args) {
        free (G->args);
        free (G);
        return 0;
    }

    G->args_size = 5*max_record_size;
    G->next_args_size = max_record_size;

    G->tags = tags;
    G->n_tags = n_tags;
    return G;
}



/***************************************************************************************/
static ffio *ffio_destroy (ffio *G) {
/****************************************************************************************
Free all allocated associated memory, then free G itself. For extra RAII compliancy,
the file object should also be closed if still open, but this will require additional
control logic, and ffio is a gie tool specific package, so we fall back to asserting that
fclose has been called prior to ffio_destroy.
****************************************************************************************/
    free (G->args);
    free (G->next_args);
    free (G);
    return 0;
}



/***************************************************************************************/
static int at_decorative_element (ffio *G) {
/****************************************************************************************
A decorative element consists of a line of at least 5 consecutive identical chars,
starting at buffer position 0:
"-----", "=====", "*****", etc.

A decorative element serves as a end delimiter for the current element, and
continues until a gie command verb is found at the start of a line
****************************************************************************************/
    int i;
    char *c;
    if (0==G)
        return 0;
    c = G->next_args;
    if (0==c)
        return 0;
    if (0==c[0])
        return 0;
    for (i = 1; i < 5; i++)
        if (c[i]!=c[0])
            return 0;
    return 1;
}



/***************************************************************************************/
static const char *at_tag (ffio *G) {
/****************************************************************************************
A start of a new command serves as an end delimiter for the current command
****************************************************************************************/
    size_t j;
    for (j = 0;  j < G->n_tags;  j++)
        if (strncmp (G->next_args, G->tags[j], strlen(G->tags[j]))==0)
            return G->tags[j];
    return 0;
}



/***************************************************************************************/
static int at_end_delimiter (ffio *G) {
/****************************************************************************************
An instruction consists of everything from its introductory tag to its end
delimiter.  An end delimiter can be either the introductory tag of the next
instruction, or a "decorative element", i.e. one of the "ascii art" style
block delimiters typically used to mark up block comments in a free format
file.
****************************************************************************************/
    if (G==0)
        return 0;
    if (at_decorative_element (G))
        return 1;
    if (at_tag (G))
        return 1;
    return 0;
}



/***************************************************************************************/
static int nextline (ffio *G) {
/****************************************************************************************
Read next line of input file. Returns 1 on success, 0 on failure.
****************************************************************************************/
    G->next_args[0] = 0;
    if (T.skip)
        return 0;
    if (0==fgets (G->next_args, (int) G->next_args_size - 1, G->f))
        return 0;
    if (feof (G->f))
        return 0;
    pj_chomp (G->next_args);
    G->next_lineno++;
    return 1;
}



/***************************************************************************************/
static int locate_tag (ffio *G, const char *tag) {
/****************************************************************************************
Find start-of-line tag (currently only used to search for for <gie>, but any tag
valid).

Returns 1 on success, 0 on failure.
****************************************************************************************/
    size_t n = strlen (tag);
    while (0!=strncmp (tag, G->next_args, n))
        if (0==nextline (G))
            return 0;
    return 1;
}



/***************************************************************************************/
static int step_into_gie_block (ffio *G) {
/****************************************************************************************
Make sure we're inside a <gie>-block. Return 1 on success, 0 otherwise.
****************************************************************************************/
    /* Already inside */
    if (G->level % 2)
        return 1;

    if (0==locate_tag (G, "<gie>"))
        return 0;

    while (0!=strncmp ("<gie>", G->next_args, 5)) {
        G->next_args[0] = 0;
        if (feof (G->f))
            return 0;
        if (0==fgets (G->next_args, (int) G->next_args_size - 1, G->f))
            return 0;
        pj_chomp (G->next_args);
        G->next_lineno++;
    }
    G->level++;

    /* We're ready at the start - now step into the block */
    return nextline (G);
}



/***************************************************************************************/
static int skip_to_next_tag (ffio *G) {
/****************************************************************************************
Skip forward to the next command tag. Return 1 on success, 0 otherwise.
****************************************************************************************/
    const char *c;
    if (0==step_into_gie_block (G))
        return 0;

    c = at_tag (G);

    /* If not already there - get there */
    while (!c) {
        if (0==nextline (G))
            return 0;
        c = at_tag (G);
    }

    /* If we reached the end of a <gie> block, locate the next and retry */
    if (0==strcmp (c, "</gie>")) {
        G->level++;
        if (feof (G->f))
            return 0;
        if (0==step_into_gie_block (G))
            return 0;
        G->args[0] = 0;
        return skip_to_next_tag (G);
    }
    G->lineno = G->next_lineno;

    return 1;
}

/* Add the most recently read line of input to the block already stored. */
static int append_args (ffio *G) {
    size_t skip_chars = 0;
    size_t next_len = strlen (G->next_args);
    size_t args_len = strlen (G->args);
    const char *tag = at_tag (G);

    if (tag)
        skip_chars = strlen (tag);

    if (G->args_size < args_len + next_len - skip_chars + 1) {
        void *p = realloc (G->args, 2 * G->args_size);
        if (0==p)
            return 0;
        G->args = p;
        G->args_size = 2 * G->args_size;
    }

    G->args[args_len] = ' ';
    strcpy (G->args + args_len + 1,  G->next_args + skip_chars);

    G->next_args[0] = 0;
    return 1;
}





/***************************************************************************************/
static int get_inp (ffio *G) {
/****************************************************************************************
The primary command reader for gie. Reads a block of gie input, cleans up repeated
whitespace etc. The block is stored in G->args. Returns 1 on success, 0 otherwise.
****************************************************************************************/
    G->args[0] = 0;

    if (0==skip_to_next_tag (G))
        return 0;
    G->tag = at_tag (G);

    if (0==G->tag)
        return 0;

    do {
        append_args (G);
        if (0==nextline (G))
            return 0;
    } while (!at_end_delimiter (G));

    pj_shrink (G->args);
    return 1;
}





static const char tc32_utm32[] = {
    " +proj=horner"
    " +ellps=intl"
    " +range=500000"
    " +fwd_origin=877605.269066,6125810.306769"
    " +inv_origin=877605.760036,6125811.281773"
    " +deg=4"
    " +fwd_v=6.1258112678e+06,9.9999971567e-01,1.5372750011e-10,5.9300860915e-15,2.2609497633e-19,4.3188227445e-05,2.8225130416e-10,7.8740007114e-16,-1.7453997279e-19,1.6877465415e-10,-1.1234649773e-14,-1.7042333358e-18,-7.9303467953e-15,-5.2906832535e-19,3.9984284847e-19"
    " +fwd_u=8.7760574982e+05,9.9999752475e-01,2.8817299305e-10,5.5641310680e-15,-1.5544700949e-18,-4.1357045890e-05,4.2106213519e-11,2.8525551629e-14,-1.9107771273e-18,3.3615590093e-10,2.4380247154e-14,-2.0241230315e-18,1.2429019719e-15,5.3886155968e-19,-1.0167505000e-18"
    " +inv_v=6.1258103208e+06,1.0000002826e+00,-1.5372762184e-10,-5.9304261011e-15,-2.2612705361e-19,-4.3188331419e-05,-2.8225549995e-10,-7.8529116371e-16,1.7476576773e-19,-1.6875687989e-10,1.1236475299e-14,1.7042518057e-18,7.9300735257e-15,5.2881862699e-19,-3.9990736798e-19"
    " +inv_u=8.7760527928e+05,1.0000024735e+00,-2.8817540032e-10,-5.5627059451e-15,1.5543637570e-18,4.1357152105e-05,-4.2114813612e-11,-2.8523713454e-14,1.9109017837e-18,-3.3616407783e-10,-2.4382678126e-14,2.0245020199e-18,-1.2441377565e-15,-5.3885232238e-19,1.0167203661e-18"
};


static const char sb_utm32[] = {
    " +proj=horner"
    " +ellps=intl"
    " +range=500000"
    " +tolerance=0.0005"
    " +fwd_origin=4.94690026817276e+05,6.13342113183056e+06"
    " +inv_origin=6.19480258923588e+05,6.13258568148837e+06"
    " +deg=3"
    " +fwd_c=6.13258562111350e+06,6.19480105709997e+05,9.99378966275206e-01,-2.82153291753490e-02,-2.27089979140026e-10,-1.77019590701470e-09,1.08522286274070e-14,2.11430298751604e-15"
    " +inv_c=6.13342118787027e+06,4.94690181709311e+05,9.99824464710368e-01,2.82279070814774e-02,7.66123542220864e-11,1.78425334628927e-09,-1.05584823306400e-14,-3.32554258683744e-15"
};

static int horner_selftest (void) {
    PJ *P;
    PJ_COORD a, b, c;
    double dist;

    /* Real polynonia relating the technical coordinate system TC32 to "System 45 Bornholm" */
    P = proj_create (PJ_DEFAULT_CTX, tc32_utm32);
    if (0==P)
        return 10;

    a = b = proj_coord (0,0,0,0);
    a.uv.v = 6125305.4245;
    a.uv.u =  878354.8539;
    c = a;

    /* Check roundtrip precision for 1 iteration each way, starting in forward direction */
    dist = proj_roundtrip (P, PJ_FWD, 1, &c);
    if (dist > 0.01)
        return 1;
    proj_destroy(P);

    /* The complex polynomial transformation between the "System Storebaelt" and utm32/ed50 */
    P = proj_create (PJ_DEFAULT_CTX, sb_utm32);
    if (0==P)
        return 11;

    /* Test value: utm32_ed50(620000, 6130000) = sb_ed50(495136.8544, 6130821.2945) */
    a = b = c = proj_coord (0,0,0,0);
    a.uv.v = 6130821.2945;
    a.uv.u =  495136.8544;
    c.uv.v = 6130000.0000;
    c.uv.u =  620000.0000;

    /* Forward projection */
    b = proj_trans (P, PJ_FWD, a);
    dist = proj_xy_dist (b, c);
    if (dist > 0.001)
        return 2;

    /* Inverse projection */
    b = proj_trans (P, PJ_INV, c);
    dist = proj_xy_dist (b, a);
    if (dist > 0.001)
        return 3;

    /* Check roundtrip precision for 1 iteration each way */
    dist = proj_roundtrip (P, PJ_FWD, 1, &a);
    if (dist > 0.01)
        return 4;

    proj_destroy(P);
    return 0;
}












/* Testing quite a bit of the pj_obs_api as a side effect (inspired by pj_obs_api_test.c) */
static int cart_selftest (void) {
    PJ_CONTEXT *ctx;
    PJ *P;
    PJ_COORD a, b, obs[2];
    PJ_COORD coord[2];

    PJ_INFO info;
    PJ_PROJ_INFO pj_info;
    PJ_GRID_INFO grid_info;
    PJ_INIT_INFO init_info;

    PJ_FACTORS factors;

    const PJ_OPERATIONS *oper_list;
    const PJ_ELLPS *ellps_list;
    const PJ_UNITS *unit_list;
    const PJ_PRIME_MERIDIANS *pm_list;

    int err;
    size_t n, sz;
    double dist, h, t;
    char *args[3] = {"proj=utm", "zone=32", "ellps=GRS80"};
    char arg[50] = {"+proj=utm; +zone=32; +ellps=GRS80"};
    char buf[40];

    /* An utm projection on the GRS80 ellipsoid */
    P = proj_create (PJ_DEFAULT_CTX, arg);
    if (0==P)
        return 1;


    /* Clean up */
    proj_destroy (P);

    /* Same projection, now using argc/argv style initialization */
    P = proj_create_argv (PJ_DEFAULT_CTX, 3, args);
    if (0==P)
        return 2;

    /* zero initialize everything, then set (longitude, latitude) to (12, 55) */
    a = proj_coord (0,0,0,0);
    /* a.lp: The coordinate part of a, interpreted as a classic LP pair */
    a.lp.lam = PJ_TORAD(12);
    a.lp.phi = PJ_TORAD(55);

    /* Forward projection */
    b = proj_trans (P, PJ_FWD, a);

    /* Inverse projection */
    a = proj_trans (P, PJ_INV, b);

    /* Null projection */
    a = proj_trans (P, PJ_IDENT, a);

    /* Forward again, to get two linear items for comparison */
    a = proj_trans (P, PJ_FWD, a);

    dist = proj_xy_dist (a, b);
    if (dist > 2e-9)
        return 3;

    /* Clear any previous error */
    proj_errno_reset (P);

    /* Invalid projection */
    a = proj_trans (P, 42, a);
    if (a.lpz.lam!=HUGE_VAL)
        return 4;
    err = proj_errno (P);
    if (0==err)
        return 5;

    /* Clear error again */
    proj_errno_reset (P);

    /* Clean up */
    proj_destroy (P);

    /* Now do some 3D transformations */
    P = proj_create (PJ_DEFAULT_CTX, "+proj=cart +ellps=GRS80");
    if (0==P)
        return 6;

    /* zero initialize everything, then set (longitude, latitude, height) to (12, 55, 100) */
    a = b = proj_coord (0,0,0,0);
    a.lpz.lam = PJ_TORAD(12);
    a.lpz.phi = PJ_TORAD(55);
    a.lpz.z   = 100;

    /* Forward projection: 3D-Cartesian-to-Ellipsoidal */
    b = proj_trans (P, PJ_FWD, a);

    /* Check roundtrip precision for 10000 iterations each way */
    dist  = proj_roundtrip (P, PJ_FWD, 10000, &a);
    dist += proj_roundtrip (P, PJ_INV, 10000, &b);
    if (dist > 4e-9)
        return 7;


    /* Test at the North Pole */
    a = b = proj_coord (0,0,0,0);
    a.lpz.lam = PJ_TORAD(0);
    a.lpz.phi = PJ_TORAD(90);
    a.lpz.z   = 100;

    /* Forward projection: Ellipsoidal-to-3D-Cartesian */
    dist = proj_roundtrip (P, PJ_FWD, 1, &a);
    if (dist > 1e-9)
        return 8;

    /* Test at the South Pole */
    a = b = proj_coord (0,0,0,0);
    a.lpz.lam = PJ_TORAD(0);
    a.lpz.phi = PJ_TORAD(-90);
    a.lpz.z   = 100;
    b = a;

    /* Forward projection: Ellipsoidal-to-3D-Cartesian */
    dist = proj_roundtrip (P, PJ_FWD, 1, &a);
    if (dist > 1e-9)
        return 9;


    /* Inverse projection: 3D-Cartesian-to-Ellipsoidal */
    b = proj_trans (P, PJ_INV, b);

    /* Move p to another context */
    ctx = proj_context_create ();
    if (ctx==pj_get_default_ctx())
        return 10;
    proj_context_set (P, ctx);
    if (ctx != P->ctx)
        return 11;
    b = proj_trans (P, PJ_FWD, b);

    /* Move it back to the default context */
    proj_context_set (P, 0);
    if (pj_get_default_ctx() != P->ctx)
        return 12;
    proj_context_destroy (ctx);

    /* We go on with the work - now back on the default context */
    b = proj_trans (P, PJ_INV, b);
    proj_destroy (P);


    /* Testing proj_trans_generic () */

    /* An utm projection on the GRS80 ellipsoid */
    P = proj_create (PJ_DEFAULT_CTX, "+proj=utm +zone=32 +ellps=GRS80");
    if (0==P)
        return 13;

    obs[0] = proj_coord (PJ_TORAD(12), PJ_TORAD(55), 45, 0);
    obs[1] = proj_coord (PJ_TORAD(12), PJ_TORAD(56), 50, 0);
    sz = sizeof (PJ_COORD);

    /* Forward projection */
    a = proj_trans (P, PJ_FWD, obs[0]);
    b = proj_trans (P, PJ_FWD, obs[1]);

    n = proj_trans_generic (
        P, PJ_FWD,
        &(obs[0].lpz.lam), sz, 2,
        &(obs[0].lpz.phi), sz, 2,
        &(obs[0].lpz.z),   sz, 2,
        0,                     sz, 0
    );
    if (2!=n)
        return 14;
    if (a.lpz.lam != obs[0].lpz.lam)  return 15;
    if (a.lpz.phi != obs[0].lpz.phi)  return 16;
    if (a.lpz.z   != obs[0].lpz.z)    return 17;
    if (b.lpz.lam != obs[1].lpz.lam)  return 18;
    if (b.lpz.phi != obs[1].lpz.phi)  return 19;
    if (b.lpz.z   != obs[1].lpz.z)    return 20;

    /* now test the case of constant z */
    obs[0] = proj_coord (PJ_TORAD(12), PJ_TORAD(55), 45, 0);
    obs[1] = proj_coord (PJ_TORAD(12), PJ_TORAD(56), 50, 0);
    h = 27;
    t = 33;
    n = proj_trans_generic (
        P, PJ_FWD,
        &(obs[0].lpz.lam), sz, 2,
        &(obs[0].lpz.phi), sz, 2,
        &h,                     0, 1,
        &t,                     0, 1
    );
    if (2!=n)
        return 21;
    if (a.lpz.lam != obs[0].lpz.lam)  return 22;
    if (a.lpz.phi != obs[0].lpz.phi)  return 23;
    if (45            != obs[0].lpz.z)    return 24;
    if (b.lpz.lam != obs[1].lpz.lam)  return 25;
    if (b.lpz.phi != obs[1].lpz.phi)  return 26;
    if (50            != obs[1].lpz.z)    return 27; /* NOTE: unchanged */
    if (50==h) return 28;

    /* test proj_trans_array () */

    coord[0] = proj_coord (PJ_TORAD(12), PJ_TORAD(55), 45, 0);
    coord[1] = proj_coord (PJ_TORAD(12), PJ_TORAD(56), 50, 0);
    if (proj_trans_array (P, PJ_FWD, 2, coord))
        return 40;

    if (a.lpz.lam != coord[0].lpz.lam)  return 41;
    if (a.lpz.phi != coord[0].lpz.phi)  return 42;
    if (a.lpz.z   != coord[0].lpz.z)    return 43;
    if (b.lpz.lam != coord[1].lpz.lam)  return 44;
    if (b.lpz.phi != coord[1].lpz.phi)  return 45;
    if (b.lpz.z   != coord[1].lpz.z)    return 46;

    /* Clean up  after proj_trans_* tests */
    proj_destroy (P);

    /* test proj_create_crs_to_crs() */
    P = proj_create_crs_to_crs(PJ_DEFAULT_CTX, "epsg:25832", "epsg:25833", NULL);
    if (P==0)
        return 50;

    a.xy.x =  700000.0;
    a.xy.y = 6000000.0;
    b.xy.x =  307788.8761171057;
    b.xy.y = 5999669.3036037628;

    a = proj_trans(P, PJ_FWD, a);
    if (dist > 1e-7)
        return 51;
    proj_destroy(P);

    /* let's make sure that only entries in init-files results in a usable PJ */
    P = proj_create_crs_to_crs(PJ_DEFAULT_CTX, "proj=utm +zone=32 +datum=WGS84", "proj=utm +zone=33 +datum=WGS84", NULL);
    if (P != 0) {
        proj_destroy(P);
        return 52;
    }
    proj_destroy(P);

    /* ********************************************************************** */
    /*                          Test info functions                           */
    /* ********************************************************************** */

    /* proj_info()                                                            */
    /* this one is difficult to test, since the output changes with the setup */
    info = proj_info();

    if (info.version[0] != '\0' ) {
        char tmpstr[64];
        sprintf(tmpstr, "%d.%d.%d", info.major, info.minor, info.patch);
        if (strcmp(info.version, tmpstr)) return 55;
    }
    if (info.release[0] == '\0')    return 56;
    if (getenv ("HOME") || getenv ("PROJ_LIB"))
        if (info.searchpath[0] == '\0') return 57;

    /* proj_pj_info() */
    P = proj_create(PJ_DEFAULT_CTX, "+proj=august"); /* august has no inverse */
    if (proj_pj_info(P).has_inverse) { proj_destroy(P); return 60; }
    proj_destroy(P);

    P = proj_create(PJ_DEFAULT_CTX, arg);
    pj_info = proj_pj_info(P);
    if ( !pj_info.has_inverse )            {  proj_destroy(P); return 61; }
    pj_shrink (arg);
    if ( strcmp(pj_info.definition, arg) ) {  proj_destroy(P); return 62; }
    if ( strcmp(pj_info.id, "utm") )       {  proj_destroy(P); return 63; }

    proj_destroy(P);

    /* proj_grid_info() */
    grid_info = proj_grid_info("null");
    if ( strlen(grid_info.filename) == 0 )            return 64;
    if ( strcmp(grid_info.gridname, "null") ) return 65;
    grid_info = proj_grid_info("nonexistinggrid");
    if ( strlen(grid_info.filename) > 0 )             return 66;

    /* proj_init_info() */
    init_info = proj_init_info("unknowninit");
    if ( strlen(init_info.filename) != 0 )  return 67;

    init_info = proj_init_info("epsg");
    /* Need to allow for "Unknown" until all commonly distributed EPSG-files comes with a metadata section */
    if ( strcmp(init_info.origin, "EPSG") && strcmp(init_info.origin, "Unknown") )    return 69;
    if ( strcmp(init_info.name, "epsg") )      return 68;


    /* test proj_rtodms() and proj_dmstor() */
    if (strcmp("180dN", proj_rtodms(buf, M_PI, 'N', 'S')))
        return 70;

    if (proj_dmstor(&buf[0], NULL) != M_PI)
        return 71;

    if (strcmp("114d35'29.612\"S", proj_rtodms(buf, -2.0, 'N', 'S')))
        return 72;

    /* we can't expect perfect numerical accuracy so testing with a tolerance */
    if (fabs(-2.0 - proj_dmstor(&buf[0], NULL)) > 1e-7)
        return 73;


    /* test proj_derivatives_retrieve() and proj_factors_retrieve() */
    P = proj_create(PJ_DEFAULT_CTX, "+proj=merc");
    a = proj_coord (0,0,0,0);
    a.lp.lam = PJ_TORAD(12);
    a.lp.phi = PJ_TORAD(55);

    factors = proj_factors(P, a);
    if (proj_errno(P))
        return 85; /* factors not created correctly */

    /* check a few key characteristics of the Mercator projection */
    if (factors.angular_distortion != 0.0)  return 86; /* angular distortion should be 0 */
    if (factors.meridian_parallel_angle != M_PI_2)  return 87; /* Meridian/parallel angle should be 90 deg */
    if (factors.meridian_convergence != 0.0)  return 88; /* meridian convergence should be 0 */

    proj_destroy(P);

    /* Check that proj_list_* functions work by looping through them */
    n = 0;
    for (oper_list = proj_list_operations(); oper_list->id; ++oper_list) n++;
    if (n == 0) return 90;

    n = 0;
    for (ellps_list = proj_list_ellps(); ellps_list->id; ++ellps_list) n++;
    if (n == 0) return 91;

    n = 0;
    for (unit_list = proj_list_units(); unit_list->id; ++unit_list) n++;
    if (n == 0) return 92;

    n = 0;
    for (pm_list = proj_list_prime_meridians(); pm_list->id; ++pm_list) n++;
    if (n == 0) return 93;


    /* check io-predicates */

    /* angular in on fwd, linear out */
    P = proj_create (PJ_DEFAULT_CTX, "+proj=cart +ellps=GRS80");
    if (0==P) return 0;
    if (!proj_angular_input (P, PJ_FWD))  return 100;
    if ( proj_angular_input (P, PJ_INV))  return 101;
    if ( proj_angular_output (P, PJ_FWD)) return 102;
    if (!proj_angular_output (P, PJ_INV)) return 103;
    P->inverted = 1;
    if ( proj_angular_input (P, PJ_FWD))  return 104;
    if (!proj_angular_input (P, PJ_INV))  return 105;
    if (!proj_angular_output (P, PJ_FWD)) return 106;
    if ( proj_angular_output (P, PJ_INV)) return 107;
    proj_destroy(P);

    /* angular in and out */
    P = proj_create(PJ_DEFAULT_CTX,
        "+proj=molodensky +a=6378160 +rf=298.25 "
        "+da=-23 +df=-8.120449e-8 +dx=-134 +dy=-48 +dz=149 "
        "+abridged "
    );
    if (0==P) return 0;
    if (!proj_angular_input (P, PJ_FWD))  return 108;
    if (!proj_angular_input (P, PJ_INV))  return 109;
    if (!proj_angular_output (P, PJ_FWD)) return 110;
    if (!proj_angular_output (P, PJ_INV)) return 111;
    P->inverted = 1;
    if (!proj_angular_input (P, PJ_FWD))  return 112;
    if (!proj_angular_input (P, PJ_INV))  return 113;
    if (!proj_angular_output (P, PJ_FWD)) return 114;
    if (!proj_angular_output (P, PJ_INV)) return 115;
    proj_destroy(P);

    /* linear in and out */
    P = proj_create(PJ_DEFAULT_CTX,
        " +proj=helmert"
        " +x=0.0127 +y=0.0065 +z=-0.0209 +s=0.00195"
        " +rx=-0.00039 +ry=0.00080 +rz=-0.00114"
        " +dx=-0.0029 +dy=-0.0002 +dz=-0.0006 +ds=0.00001"
        " +drx=-0.00011 +dry=-0.00019 +drz=0.00007"
        " +t_epoch=1988.0 +transpose +no_defs"
    );
    if (0==P) return 0;
    if (proj_angular_input (P, PJ_FWD))  return 116;
    if (proj_angular_input (P, PJ_INV))  return 117;
    if (proj_angular_output (P, PJ_FWD)) return 118;
    if (proj_angular_output (P, PJ_INV)) return 119;
    P->inverted = 1;
    if (proj_angular_input (P, PJ_FWD))  return 120;
    if (proj_angular_input (P, PJ_INV))  return 121;
    if (proj_angular_output (P, PJ_FWD)) return 122;
    if (proj_angular_output (P, PJ_INV)) return 123;

    /* We specified "no_defs" but didn't give any ellipsoid info */
    /* pj_init_ctx should default to WGS84 */
    if (P->a != 6378137.0) return 124;
    if (P->f != 1.0/298.257223563) return 125;
    proj_destroy(P);

    /* Test that pj_fwd* and pj_inv* returns NaNs when receiving NaN input */
    P = proj_create(PJ_DEFAULT_CTX, "+proj=merc");
    if (0==P) return 0;
    a = proj_coord(NAN, NAN, NAN, NAN);
    a = proj_trans(P, PJ_FWD, a);
    if ( !( isnan(a.v[0]) && isnan(a.v[1]) && isnan(a.v[2]) && isnan(a.v[3]) ) )
        return 126;
    a = proj_coord(NAN, NAN, NAN, NAN);
    a = proj_trans(P, PJ_INV, a);
    if ( !( isnan(a.v[0]) && isnan(a.v[1]) && isnan(a.v[2]) && isnan(a.v[3]) ) )
        return 127;
    proj_destroy(P);

    return 0;
}




static int test_time(const char* args, double tol, double t_in, double t_exp) {
    PJ_COORD in, out;
    PJ *P = proj_create(PJ_DEFAULT_CTX, args);
    int ret = 0;

    if (P == 0)
        return 5;

    in = proj_coord(0.0, 0.0, 0.0, t_in);

    out = proj_trans(P, PJ_FWD, in);
    if (fabs(out.xyzt.t - t_exp) > tol) {
        proj_log_error(P, "out: %10.10g, expect: %10.10g", out.xyzt.t, t_exp);
        ret = 1;
    }
    out = proj_trans(P, PJ_INV, out);
    if (fabs(out.xyzt.t - t_in) > tol) {
        proj_log_error(P, "out: %10.10g, expect: %10.10g", out.xyzt.t, t_in);
        ret = 2;
    }
    pj_free(P);

    proj_log_level(NULL, 0);
    return ret;
}

static int unitconvert_selftest (void) {
    int ret = 0;
    char args1[] = "+proj=unitconvert +t_in=decimalyear +t_out=decimalyear";
    double in1 = 2004.25;

    char args2[] = "+proj=unitconvert +t_in=gps_week +t_out=gps_week";
    double in2 = 1782.0;

    char args3[] = "+proj=unitconvert +t_in=mjd +t_out=mjd";
    double in3 = 57390.0;

    char args4[] = "+proj=unitconvert +t_in=gps_week +t_out=decimalyear";
    double in4 = 1877.71428, exp4 = 2016.0;

    char args5[] = "+proj=unitconvert +t_in=yyyymmdd +t_out=yyyymmdd";
    double in5 = 20170131;

    ret = test_time(args1, 1e-6, in1, in1);   if (ret) return ret + 10;
    ret = test_time(args2, 1e-6, in2, in2);   if (ret) return ret + 20;
    ret = test_time(args3, 1e-6, in3, in3);   if (ret) return ret + 30;
    ret = test_time(args4, 1e-6, in4, exp4);  if (ret) return ret + 40;
    ret = test_time(args5, 1e-6, in5, in5);   if (ret) return ret + 50;

    return 0;

}
