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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <proj.h>
#include "proj_internal.h"
#include "projects.h"

#include "optargpm.h"

/* From readblock.c (now included here) */
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

FILE *test = 0;

static int get_inp (ffio *F);
static int skip_to_next_tag (ffio *F);
static int step_into_gie_block (ffio *F);
static int locate_tag (ffio *F, const char *tag);
static int nextline (ffio *F);
static int at_end_delimiter (ffio *F);
static const char *at_tag (ffio *F);
static int at_decorative_element (ffio *F);
static ffio *ffio_destroy (ffio *F);
static ffio *ffio_create (const char **tags, size_t n_tags, size_t max_record_size);

static const char *gie_tags[] = {
    "<gie>", "operation", "accept", "expect", "roundtrip", "banner", "verbose",
    "direction", "tolerance", "builtins", "echo", "</gie>"
};

static const size_t n_gie_tags = sizeof gie_tags / sizeof gie_tags[0];

int test_main (void);


/* from proj_strtod.c */
double proj_strtod(const char *str, char **endptr);
double proj_atof(const char *str);

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
    int op_id;
    int op_ok,    op_ko;
    int total_ok, total_ko;
    int grand_ok, grand_ko;
    size_t operation_lineno;
    double tolerance;
    const char *curr_file;
    FILE *fout;
} gie_ctx;

ffio *F = 0;

static gie_ctx T;


static const char delim[] = {"-------------------------------------------------------------------------------\n"};


#define CMDLEN 250000

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
    "    -l                List the PROJ internal system error codes\n"
    "--------------------------------------------------------------------------------\n"
    "Long Options:\n"
    "--------------------------------------------------------------------------------\n"
    "    --output          Alias for -o\n"
    "    --verbose         Alias for -v\n"
    "    --help            Alias for -h\n"
    "    --list            Alias for -l\n"
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
    const char *longflags[]  = {"v=verbose", "q=quiet", "h=help", "l=list", 0};
    const char *longkeys[]   = {"o=output", 0};
    OPTARGS *o;
test_main ();
    memset (&T, 0, sizeof (T));
    T.dir = PJ_FWD;
    T.verbosity = 1;
    T.tolerance = 5e-4;

    test = fopen ("test.c", "wt");
    o = opt_parse (argc, argv, "hlvq", "o", longflags, longkeys);
    if (0==o)
        return 0;

    if (opt_given (o, "h")) {
        printf (usage, o->progname);
        return 0;
    }

    if (opt_given (o, "l"))
        return list_err_codes ();


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
    ffio_destroy (F);
    return T.grand_ko;
}

static int another_failure (void) {
    T.op_ko++;
    T.total_ko++;
    return 0;
}

static int another_success (void) {
    T.op_ok++;
    T.total_ok++;
    return 0;
}


static int process_file (const char *fname) {
    FILE *f;

    F->lineno = F->next_lineno = F->level = 0;
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

    T.grand_ok += T.total_ok;
    T.grand_ko += T.total_ko;
    if (T.verbosity > 0)
    fprintf (T.fout, "%stotal: %2d tests succeeded,  %2d tests %s\n", delim, T.total_ok, T.total_ko, T.total_ko? "FAILED!": "failed.");

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
    else
        s *= default_scale;
    return s;
}


static int banner (const char *args) {
    char dots[] = {"..."}, nodots[] = {""}, *thedots = nodots;
    if (strlen(args) > 70)
        thedots = dots;
    fprintf (T.fout, "%s%-70.70s%s\n", delim, args, thedots);
    puts ("banner completed!");
    return 0;
}


static int tolerance (const char *args) {
    T.tolerance = strtod_scaled (args, 1);
    if (HUGE_VAL==T.tolerance) {
        T.tolerance = 0.0005;
fprintf (test, "    tolerance = %.12f;\n", T.tolerance);
        return 1;
    }
fprintf (test, "    tolerance = %.12f;\n", T.tolerance);
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
fprintf (test, "    direction = %d;\n", T.dir);

    return 0;
}


static void finish_previous_operation (const char *args) {
    if (T.verbosity > 1 && T.op_id > 1 && T.op_ok+T.op_ko)
        fprintf (T.fout, "%s     %d tests succeeded,  %d tests %s\n", delim, T.op_ok, T.op_ko, T.op_ko? "FAILED!": "failed.");
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
        banner ("Did banner finish?");
    }


    T.op_ok = 0;
    T.op_ko = 0;

    direction ("forward");
    tolerance ("0.5 mm");

    proj_errno_reset (T.P);

    if (T.P)
        proj_destroy (T.P);
    proj_errno_reset (0);

    printf ("Creating [%s]\n", F->args);
    T.P = proj_create (0, F->args);
    puts ("created");
fprintf (test, "    (void) P;\n    pj_free (P);\n\n\n    P = proj_create(\"%s\");\n", F->args);
    /* Checking that proj_create succeeds is first done at "expect" time, */
    /* we want to support "expect"ing specific error codes */

    return 0;
}




static int pj_unitconvert_selftest (void);
static int pj_cart_selftest (void);
static int pj_horner_selftest (void);

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
puts ("builtins - calling unitconvert");
    i = pj_unitconvert_selftest ();
    if (i!=0) {
        printf ("pj_unitconvert_selftest fails with %d\n", i);
        another_failure();
    }
    else
        another_success ();


puts ("builtins - calling cart");
    i = pj_cart_selftest ();
    if (i!=0) {
        printf ("pj_cart_selftest fails with %d\n", i);
        another_failure();
    }
    else
        another_success ();

puts ("builtins - calling horner");
    i = pj_horner_selftest ();
    if (i!=0) {
        printf ("pj_horner_selftest fails with %d\n", i);
        another_failure();
    }
    else
        another_success ();
puts ("builtins - completed");

    return 0;
}


static PJ_COORD torad_coord (PJ_COORD a) {
    PJ_COORD c = a;
    c.lpz.lam = proj_torad (a.lpz.lam);
    c.lpz.phi = proj_torad (a.lpz.phi);
    return c;
}


static PJ_COORD todeg_coord (PJ_COORD a) {
    PJ_COORD c = a;
    c.lpz.lam = proj_todeg (a.lpz.lam);
    c.lpz.phi = proj_todeg (a.lpz.phi);
    return c;
}



/*****************************************************************************/
static PJ_COORD parse_coord (const char *args) {
/*****************************************************************************
    Attempt to interpret args as a PJ_COORD.
******************************************************************************/
    int i;
    const char *endp, *prev = args;
    PJ_COORD a = proj_coord (0,0,0,0);

    for (i = 0; i < 4; i++) {
        double d = proj_strtod (prev,  (char **) &endp);
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
    return 0;
}


/*****************************************************************************/
static int roundtrip (const char *args) {
/*****************************************************************************
    Check how far we go from the ACCEPTed point when doing successive
    back/forward transformation pairs.
******************************************************************************/
    int ntrips;
    double d, r, ans;
    char *endp;
    PJ_COORD coo;

    if (0==T.P)
        return another_failure ();

    ans = proj_strtod (args, &endp);
    ntrips = (int) (endp==args? 100: fabs(ans));
    d = strtod_scaled (endp, 1);
    d = d==HUGE_VAL?  T.tolerance:  d;

    /* input ("accepted") values - probably in degrees */
    coo = proj_angular_input  (T.P, T.dir)? torad_coord (T.a):  T.a;

    r = proj_roundtrip (T.P, T.dir, ntrips, &coo);
    if (r <= d)
        return another_success ();

    if (T.verbosity > -1) {
        if (0==T.op_ko && T.verbosity < 2)
            banner (T.operation);
        fprintf (T.fout, "%s", T.op_ko? "     -----\n": delim);
        fprintf (T.fout, "     FAILURE in %s(%d):\n", opt_strip_path (T.curr_file), (int) F->lineno);
        fprintf (T.fout, "     roundtrip deviation: %.3f mm, expected: %.3f mm\n", 1000*r, 1000*d);
    }
    return another_failure ();
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
    fprintf (T.fout, "     got:      %.9f   %.9f", T.b.xy.x,  T.b.xy.y);
    if (T.b.xyzt.t!=0 || T.b.xyzt.z!=0)
        fprintf (T.fout, "   %.9f", T.b.xyz.z);
    if (T.b.xyzt.t!=0)
        fprintf (T.fout, "   %.9f", T.b.xyzt.t);
    fprintf (T.fout, "\n");
    fprintf (T.fout, "     deviation:  %.3f mm,  expected:  %.3f mm\n", 1000*d, 1000*T.tolerance);
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
    another_failure ();

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

    if (0==T.P) {
        /* If we expect failure, and fail, then it's a success... */
        if (expect_failure) {
            /* Failed to fail correctly? */
            if (expect_failure_with_errno && proj_errno (T.P)!=expect_failure_with_errno)
                return expect_failure_with_errno_message (expect_failure_with_errno, proj_errno(T.P));
            return another_success ();
        }

        /* Otherwise, it's a true failure */
        banner (T.operation);
        errmsg(3, "%sInvalid operation definition in line no. %d: %s\n",
            delim, (int) T.operation_lineno, pj_strerrno(proj_errno(T.P)));
        return another_failure ();
    }

    /* We may still successfully fail even if the proj_create succeeded */
    if (expect_failure) {
        proj_errno_reset (T.P);

        /* Try to carry out the operation - and expect failure */
        ci = proj_angular_input (T.P, T.dir)? torad_coord (T.a): T.a;
        co = proj_trans (T.P, T.dir, ci);

        /* Failed to fail? - that's a failure */
        if (co.xyz.x!=HUGE_VAL)
            return another_failure ();

        if (expect_failure_with_errno) {
            printf ("errno=%d, expected=%d\n", proj_errno (T.P), expect_failure_with_errno);
            if (proj_errno (T.P)==expect_failure_with_errno)
                return another_success ();

            return another_failure ();
        }

        /* Yes, we failed successfully */
        return another_success ();
    }


    if (T.verbosity > 3) {
        puts (T.P->inverted? "INVERTED": "NOT INVERTED");
        puts (T.dir== 1? "forward": "reverse");
        puts (proj_angular_input (T.P, T.dir)?  "angular in":  "linear in");
        puts (proj_angular_output (T.P, T.dir)? "angular out": "linear out");
        printf ("left: %d   right:  %d\n", T.P->left, T.P->right);
    }

    T.e  =  parse_coord (args);
    if (HUGE_VAL==T.e.v[0])
        return expect_message_cannot_parse (args);

    /* expected angular values, probably in degrees */
    ce = proj_angular_output (T.P, T.dir)? torad_coord (T.e): T.e;
    if (T.verbosity > 3)
        printf ("EXPECTS  %.4f  %.4f  %.4f  %.4f\n", ce.v[0],ce.v[1],ce.v[2],ce.v[3]);

    /* input ("accepted") values, also probably in degrees */
    ci = proj_angular_input (T.P, T.dir)? torad_coord (T.a): T.a;
    if (T.verbosity > 3)
        printf ("ACCEPTS  %.4f  %.4f  %.4f  %.4f\n", ci.v[0],ci.v[1],ci.v[2],ci.v[3]);
fprintf (test, "    ci.xyzt.x = %.15f;\n", ci.xyzt.x);
fprintf (test, "    ci.xyzt.y = %.15f;\n", ci.xyzt.y);
fprintf (test, "    ci.xyzt.z = %.15f;\n", ci.xyzt.z);
fprintf (test, "    ci.xyzt.t = %.15f;\n", ci.xyzt.t);

fprintf (test, "    ce.xyzt.x = %.15f;\n", ce.xyzt.x);
fprintf (test, "    ce.xyzt.y = %.15f;\n", ce.xyzt.y);
fprintf (test, "    ce.xyzt.z = %.15f;\n", ce.xyzt.z);
fprintf (test, "    ce.xyzt.t = %.15f;\n", ce.xyzt.t);

fprintf (test, "    co = proj_trans (P, direction, ci);\n");

    /* angular output from proj_trans comes in radians */
    co = proj_trans (T.P, T.dir, ci);
    T.b = proj_angular_output (T.P, T.dir)? todeg_coord (co): co;
    if (T.verbosity > 3)
        printf ("GOT      %.4f  %.4f  %.4f  %.4f\n", ci.v[0],ci.v[1],ci.v[2],ci.v[3]);

    /* but there are a few more possible input conventions... */
    if (proj_angular_output (T.P, T.dir)) {
        double e = HUGE_VAL;
        d = proj_lpz_dist (T.P, ce.lpz, co.lpz);
fprintf (test, "    d = proj_lpz_dist (co, ce);\n");
        /* check whether input was already in radians */
        if (d > T.tolerance)
            e = proj_lpz_dist (T.P, T.e.lpz, co.lpz);
        if (e < d)
            d = e;

        /* or the tolerance may be based on euclidean distance */
        if (d > T.tolerance)
            e = proj_xyz_dist (T.b.xyz, T.e.xyz);
        if (e < d)
            d = e;
    }
    else {
        d = proj_xyz_dist (T.b.xyz, T.e.xyz);
        fprintf (test, "    d = proj_xyz_dist (cb, ce);\n");
    }
fprintf (test, "    update (d, tolerance, \"%s\", ci, ce, cb);\n\n", T.operation);
    if (d > T.tolerance)
        return expect_message (d, args);

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



static int dispatch (const char *cmnd, const char *args) {
    if  (0==strcmp (cmnd, "OPERATION")) return  operation ((char *) args);
    if  (0==strcmp (cmnd, "operation")) return  operation ((char *) args);
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
    if  (0==strcmp (cmnd, "BUILTINS"))  return  builtins  (args);
    if  (0==strcmp (cmnd, "builtins"))  return  builtins  (args);
    if  (0==strcmp (cmnd, "ECHO"))      return  echo      (args);
    if  (0==strcmp (cmnd, "echo"))      return  echo      (args);

#if 0
    if (proj_errno(T.P))
        printf ("#####***** ERRNO=%d\n", proj_errno(T.P));
    proj_errno_restore (T.P, last_errno);
#endif
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
    {"pjd_err_unknow_unit_id"           ,  -7},
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

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include <string.h>
#include <ctype.h>

#include <math.h>
#include <errno.h>



static int get_inp (ffio *F);
static int skip_to_next_tag (ffio *F);
static int step_into_gie_block (ffio *F);
static int locate_tag (ffio *F, const char *tag);
static int nextline (ffio *F);
static int at_end_delimiter (ffio *F);
static const char *at_tag (ffio *F);
static int at_decorative_element (ffio *F);
static ffio *ffio_destroy (ffio *F);
static ffio *ffio_create (const char **tags, size_t n_tags, size_t max_record_size);



/***************************************************************************************/
static ffio *ffio_create (const char **tags, size_t n_tags, size_t max_record_size) {
/****************************************************************************************

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
    Read next line of input file
****************************************************************************************/
    G->next_args[0] = 0;
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
    find start-of-line tag
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
    Make sure we're inside a <gie>-block
****************************************************************************************/
    /* Already inside */
    if (G->level % 2)
        return 1;

    if (0==locate_tag (G, "<gie>"))
        return 0;

    while (0!=strncmp ("<gie>", G->next_args, 5)) {
        printf ("skipping [%s]\n", G->next_args);
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
    Skip forward to the next command tag
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
    The primary command reader for gie. May be useful in the init-file reader as well.
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





char tc32_utm32[] = {
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


char sb_utm32[] = {
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

static int pj_horner_selftest (void) {
    PJ *P;
    PJ_COORD a, b, c;
    double dist;

puts ("pj_horner_selftest");
    /* Real polynonia relating the technical coordinate system TC32 to "System 45 Bornholm" */
    P = proj_create (PJ_DEFAULT_CTX, tc32_utm32);
    if (0==P)
        return 10;
puts ("pj_horner_selftest");

    a = b = proj_coord (0,0,0,0);
    a.uv.v = 6125305.4245;
    a.uv.u =  878354.8539;
    c = a;

    /* Check roundtrip precision for 1 iteration each way, starting in forward direction */
    dist = proj_roundtrip (P, PJ_FWD, 1, &c);
    if (dist > 0.01)
        return 1;
puts ("pj_horner_selftest");
    proj_destroy(P);

    /* The complex polynomial transformation between the "System Storebaelt" and utm32/ed50 */
    P = proj_create (PJ_DEFAULT_CTX, sb_utm32);
    if (0==P)
        return 11;
puts ("pj_horner_selftest");

    /* Test value: utm32_ed50(620000, 6130000) = sb_ed50(495136.8544, 6130821.2945) */
    a = b = c = proj_coord (0,0,0,0);
    a.uv.v = 6130821.2945;
    a.uv.u =  495136.8544;
    c.uv.v = 6130000.0000;
    c.uv.u =  620000.0000;

    /* Forward projection */
    b = proj_trans (P, PJ_FWD, a);
    dist = proj_xy_dist (b.xy, c.xy);
    if (dist > 0.001)
        return 2;
puts ("pj_horner_selftest");

    /* Inverse projection */
    b = proj_trans (P, PJ_INV, c);
    dist = proj_xy_dist (b.xy, a.xy);
    if (dist > 0.001)
        return 3;
puts ("pj_horner_selftest");

    /* Check roundtrip precision for 1 iteration each way */
    dist = proj_roundtrip (P, PJ_FWD, 1, &a);
    if (dist > 0.01)
        return 4;

    proj_destroy(P);
puts ("pj_horner_selftest - done");
    return 0;
}












/* Testing quite a bit of the pj_obs_api as a side effect (inspired by pj_obs_api_test.c) */
static int pj_cart_selftest (void) {
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
    const char *arg = {"+proj=utm +zone=32 +ellps=GRS80"};
    char buf[40];

puts ("pj_cart_selftest - start");
    /* An utm projection on the GRS80 ellipsoid */
    P = proj_create (PJ_DEFAULT_CTX, arg);
    if (0==P)
        return 1;


    /* Clean up */
    proj_destroy (P);
puts ("pj_cart_selftest");

    /* Same projection, now using argc/argv style initialization */
    P = proj_create_argv (PJ_DEFAULT_CTX, 3, args);
    if (0==P)
        return 2;
puts ("pj_cart_selftest");

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

    dist = proj_xy_dist (a.xy, b.xy);
    if (dist > 2e-9)
        return 3;
puts ("pj_cart_selftest");

    /* Clear any previous error */
    proj_errno_reset (P);

    /* Invalid projection */
    a = proj_trans (P, 42, a);
    if (a.lpz.lam!=HUGE_VAL)
        return 4;
    err = proj_errno (P);
    if (0==err)
        return 5;
puts ("pj_cart_selftest");

    /* Clear error again */
    proj_errno_reset (P);

    /* Clean up */
    proj_destroy (P);

    /* Now do some 3D transformations */
    P = proj_create (PJ_DEFAULT_CTX, "+proj=cart +ellps=GRS80");
    if (0==P)
        return 6;
puts ("pj_cart_selftest");

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
puts ("pj_cart_selftest");


    /* Test at the North Pole */
    a = b = proj_coord (0,0,0,0);
    a.lpz.lam = PJ_TORAD(0);
    a.lpz.phi = PJ_TORAD(90);
    a.lpz.z   = 100;

    /* Forward projection: Ellipsoidal-to-3D-Cartesian */
    dist = proj_roundtrip (P, PJ_FWD, 1, &a);
    if (dist > 1e-12)
        return 8;
puts ("pj_cart_selftest");

    /* Test at the South Pole */
    a = b = proj_coord (0,0,0,0);
    a.lpz.lam = PJ_TORAD(0);
    a.lpz.phi = PJ_TORAD(-90);
    a.lpz.z   = 100;
    b = a;

    /* Forward projection: Ellipsoidal-to-3D-Cartesian */
    dist = proj_roundtrip (P, PJ_FWD, 1, &a);
    if (dist > 1e-12)
        return 9;
puts ("pj_cart_selftest");


    /* Inverse projection: 3D-Cartesian-to-Ellipsoidal */
    b = proj_trans (P, PJ_INV, b);

    /* Move p to another context */
    ctx = proj_context_create ();
    if (ctx==pj_get_default_ctx())
        return 10;
puts ("pj_cart_selftest - 10");
    proj_context_set (P, ctx);
    if (ctx != P->ctx)
        return 11;
puts ("pj_cart_selftest");
    b = proj_trans (P, PJ_FWD, b);

    /* Move it back to the default context */
    proj_context_set (P, 0);
    if (pj_get_default_ctx() != P->ctx)
        return 12;
puts ("pj_cart_selftest");
    proj_context_destroy (ctx);

    /* We go on with the work - now back on the default context */
    b = proj_trans (P, PJ_INV, b);
    proj_destroy (P);


    /* Testing proj_trans_generic () */

    /* An utm projection on the GRS80 ellipsoid */
    P = proj_create (PJ_DEFAULT_CTX, "+proj=utm +zone=32 +ellps=GRS80");
    if (0==P)
        return 13;
puts ("pj_cart_selftest");

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
puts ("pj_cart_selftest");

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
puts ("pj_cart_selftest - 28");

    /* test proj_trans_array () */

    coord[0] = proj_coord (PJ_TORAD(12), PJ_TORAD(55), 45, 0);
    coord[1] = proj_coord (PJ_TORAD(12), PJ_TORAD(56), 50, 0);
    if (proj_trans_array (P, PJ_FWD, 2, coord))
        return 40;
puts ("pj_cart_selftest");

    if (a.lpz.lam != coord[0].lpz.lam)  return 41;
    if (a.lpz.phi != coord[0].lpz.phi)  return 42;
    if (a.lpz.z   != coord[0].lpz.z)    return 43;
    if (b.lpz.lam != coord[1].lpz.lam)  return 44;
    if (b.lpz.phi != coord[1].lpz.phi)  return 45;
    if (b.lpz.z   != coord[1].lpz.z)    return 46;

    /* Clean up  after proj_trans_* tests */
    proj_destroy (P);
puts ("pj_cart_selftest");

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
puts ("pj_cart_selftest");

    /* let's make sure that only entries in init-files results in a usable PJ */
    P = proj_create_crs_to_crs(PJ_DEFAULT_CTX, "proj=utm +zone=32 +datum=WGS84", "proj=utm +zone=33 +datum=WGS84", NULL);
    if (P != 0) {
        proj_destroy(P);
        return 52;
    }
puts ("pj_cart_selftest - a");
    proj_destroy(P);
puts ("pj_cart_selftest - b");

    /* ********************************************************************** */
    /*                          Test info functions                           */
    /* ********************************************************************** */

    /* proj_info()                                                            */
    /* this one is difficult to test, since the output changes with the setup */
    info = proj_info();
puts ("pj_cart_selftest - c");
    if (info.version[0] != '\0' ) {
        char tmpstr[64];
        sprintf(tmpstr, "%d.%d.%d", info.major, info.minor, info.patch);
        if (strcmp(info.version, tmpstr)) return 55;
    }
    if (info.release[0] == '\0')    return 56;
    if (info.searchpath[0] == '\0') return 57;

    /* proj_pj_info() */
    P = proj_create(PJ_DEFAULT_CTX, "+proj=august"); /* august has no inverse */
    if (proj_pj_info(P).has_inverse) { proj_destroy(P); return 60; }
    proj_destroy(P);
puts ("pj_cart_selftest - d");

    P = proj_create(PJ_DEFAULT_CTX, arg);
    pj_info = proj_pj_info(P);
    if ( !pj_info.has_inverse )            {  proj_destroy(P); return 61; }
    if ( strcmp(pj_info.definition, arg) ) {  proj_destroy(P); return 62; }
    if ( strcmp(pj_info.id, "utm") )       {  proj_destroy(P); return 63; }
puts ("pj_cart_selftest - e");

    proj_destroy(P);
puts ("pj_cart_selftest");

    /* proj_grid_info() */
    grid_info = proj_grid_info("egm96_15.gtx");
puts ("pj_cart_selftest - 1");
    if ( strlen(grid_info.filename) == 0 )            return 64;
puts ("pj_cart_selftest - 64");
    if ( strcmp(grid_info.gridname, "egm96_15.gtx") ) return 65;
puts ("pj_cart_selftest - 65");
    grid_info = proj_grid_info("nonexistinggrid");
puts ("pj_cart_selftest - 65a");
    if ( strlen(grid_info.filename) > 0 )             return 66;
puts ("pj_cart_selftest - 66");

    /* proj_init_info() */
    init_info = proj_init_info("unknowninit");
puts ("pj_cart_selftest - 66a");
    if ( strlen(init_info.filename) != 0 )  return 67;
puts ("pj_cart_selftest - 67");

    init_info = proj_init_info("epsg");
puts ("pj_cart_selftest - 67a");
    /* Need to allow for "Unknown" until all commonly distributed EPSG-files comes with a metadata section */
    if ( strcmp(init_info.origin, "EPSG") && strcmp(init_info.origin, "Unknown") )    return 69;
puts ("pj_cart_selftest - 69");
    if ( strcmp(init_info.name, "epsg") )      return 68;
puts ("pj_cart_selftest - 68");



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

    factors = proj_factors(P, a.lp);
    if (proj_errno(P))
        return 85; /* factors not created correctly */
puts ("pj_cart_selftest");

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

puts ("pj_cart_selftest");

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
puts ("pj_cart_selftest");

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
puts ("pj_cart_selftest");

    /* linear in and out */
    P = proj_create(PJ_DEFAULT_CTX,
        " +proj=helmert +ellps=GRS80"
        " +x=0.0127 +y=0.0065 +z=-0.0209 +s=0.00195"
        " +rx=-0.00039 +ry=0.00080 +rz=-0.00114"
        " +dx=-0.0029 +dy=-0.0002 +dz=-0.0006 +ds=0.00001"
        " +drx=-0.00011 +dry=-0.00019 +drz=0.00007"
        " +t_epoch=1988.0 +transpose"
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
    proj_destroy(P);
puts ("pj_cart_selftest - done");


    return 0;
}















static int test_time(const char* args, double tol, double t_in, double t_exp) {
    PJ_COORD in, out;
    PJ *P = proj_create(PJ_DEFAULT_CTX, args);
    int ret = 0;

    if (P == 0)
        return 5;

    in.xyzt.t = t_in;

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

static int test_xyz(const char* args, double tol, PJ_COORD in, PJ_COORD exp) {
    PJ_COORD out = {{0,0,0,0}}, obs_in = {{0,0,0,0}};
    PJ *P = proj_create(PJ_DEFAULT_CTX, args);
    int ret = 0;

    if (P == 0)
        return 5;

    obs_in.xyz = in.xyz;
    out = proj_trans(P, PJ_FWD, obs_in);
    if (proj_xyz_dist(out.xyz, exp.xyz) > tol) {
        printf("exp: %10.10g, %10.10g, %10.10g\n", exp.xyz.x, exp.xyz.y, exp.xyz.z);
        printf("out: %10.10g, %10.10g, %10.10g\n", out.xyz.x, out.xyz.y, out.xyz.z);
        ret = 1;
    }

    out = proj_trans(P, PJ_INV, out);
    if (proj_xyz_dist(out.xyz, in.xyz) > tol) {
        printf("exp: %g, %g, %g\n", in.xyz.x, in.xyz.y, in.xyz.z);
        printf("out: %g, %g, %g\n", out.xyz.x, out.xyz.y, out.xyz.z);
        ret += 2;
    }
    proj_destroy(P);
    proj_log_level(NULL, 0);
    return ret;
}


static int pj_unitconvert_selftest (void) {
    int ret = 0;
    char args1[] = "+proj=unitconvert +t_in=decimalyear +t_out=decimalyear";
    double in1 = 2004.25;

    char args2[] = "+proj=unitconvert +t_in=gps_week +t_out=gps_week";
    double in2 = 1782.0;

    char args3[] = "+proj=unitconvert +t_in=mjd +t_out=mjd";
    double in3 = 57390.0;

    char args4[] = "+proj=unitconvert +t_in=gps_week +t_out=decimalyear";
    double in4 = 1877.71428, exp4 = 2016.0;

    char args5[] = "+proj=unitconvert +xy_in=m +xy_out=dm +z_in=cm +z_out=mm";
    PJ_COORD in5 = {{55.25, 23.23, 45.5, 0}}, exp5 = {{552.5, 232.3, 455.0, 0}};

    char args6[] = "+proj=unitconvert +xy_in=m +xy_out=m +z_in=m +z_out=m";
    PJ_COORD in6 = {{12.3, 45.6, 7.89, 0}};
puts ("pj_unitconvert_selftest - start");
    ret = test_time(args1, 1e-6, in1, in1);   if (ret) return ret + 10;
puts ("pj_unitconvert_selftest");
    ret = test_time(args2, 1e-6, in2, in2);   if (ret) return ret + 20;
puts ("pj_unitconvert_selftest");
    ret = test_time(args3, 1e-6, in3, in3);   if (ret) return ret + 30;
puts ("pj_unitconvert_selftest");
    ret = test_time(args4, 1e-6, in4, exp4);  if (ret) return ret + 40;
puts ("pj_unitconvert_selftest");
    ret = test_xyz (args5, 1e-10, in5, exp5); if (ret) return ret + 50;
puts ("pj_unitconvert_selftest");
    ret = test_xyz (args6, 1e-10, in6, in6);  if (ret) return ret + 50;
puts ("pj_unitconvert_selftest - done");

    return 0;

}
















#include <stdio.h>
/* #include <proj.h>*/

int OK = 0, KO = 0;

static void test_update (double distance, double tolerance, char *definition, PJ_COORD ci, PJ_COORD ce, PJ_COORD co) {
    (void) ci;
    (void) ce;
    (void) co;
    (void) definition;

    if (distance < 1000*tolerance)
        OK++;
    else {
        KO++;
        printf ("%80.80s: %.3f\n", definition, distance);
    }
}

int test_main (void);
int test_main (void) {
    double d, tolerance;
    enum PJ_DIRECTION direction = 1;
    PJ_COORD ci, ce, co;
    PJ *P;


    P = proj_create (0, "proj=axisswap order=1,2,3,4");
    tolerance = 0.000001000000;
    ci.xyzt.x = 1.000000000000000;
    ci.xyzt.y = 2.000000000000000;
    ci.xyzt.z = 3.000000000000000;
    ci.xyzt.t = 4.000000000000000;
    ce.xyzt.x = 1.000000000000000;
    ce.xyzt.y = 2.000000000000000;
    ce.xyzt.z = 3.000000000000000;
    ce.xyzt.t = 4.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "proj=axisswap order=1,2,3,4", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "proj=axisswap order=4,3,2,1");
    tolerance = 0.000001000000;
    ci.xyzt.x = 1.000000000000000;
    ci.xyzt.y = 2.000000000000000;
    ci.xyzt.z = 3.000000000000000;
    ci.xyzt.t = 4.000000000000000;
    ce.xyzt.x = 4.000000000000000;
    ce.xyzt.y = 3.000000000000000;
    ce.xyzt.z = 2.000000000000000;
    ce.xyzt.t = 1.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "proj=axisswap order=4,3,2,1", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "proj=axisswap order=-1,-2,-3,-4");
    tolerance = 0.000001000000;
    ci.xyzt.x = 1.000000000000000;
    ci.xyzt.y = 2.000000000000000;
    ci.xyzt.z = 3.000000000000000;
    ci.xyzt.t = 4.000000000000000;
    ce.xyzt.x = -1.000000000000000;
    ce.xyzt.y = -2.000000000000000;
    ce.xyzt.z = -3.000000000000000;
    ce.xyzt.t = -4.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "proj=axisswap order=-1,-2,-3,-4", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "proj=axisswap order=1,2,-3,4");
    tolerance = 0.000001000000;
    ci.xyzt.x = 1.000000000000000;
    ci.xyzt.y = 2.000000000000000;
    ci.xyzt.z = 3.000000000000000;
    ci.xyzt.t = 4.000000000000000;
    ce.xyzt.x = 1.000000000000000;
    ce.xyzt.y = 2.000000000000000;
    ce.xyzt.z = -3.000000000000000;
    ce.xyzt.t = 4.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "proj=axisswap order=1,2,-3,4", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "proj=axisswap order=-1,2,3,4");
    tolerance = 0.000001000000;
    ci.xyzt.x = 1.000000000000000;
    ci.xyzt.y = 2.000000000000000;
    ci.xyzt.z = 3.000000000000000;
    ci.xyzt.t = 4.000000000000000;
    ce.xyzt.x = -1.000000000000000;
    ce.xyzt.y = 2.000000000000000;
    ce.xyzt.z = 3.000000000000000;
    ce.xyzt.t = 4.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "proj=axisswap order=-1,2,3,4", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "proj=axisswap order=1,2,3,-4");
    tolerance = 0.000001000000;
    ci.xyzt.x = 1.000000000000000;
    ci.xyzt.y = 2.000000000000000;
    ci.xyzt.z = 3.000000000000000;
    ci.xyzt.t = 4.000000000000000;
    ce.xyzt.x = 1.000000000000000;
    ce.xyzt.y = 2.000000000000000;
    ce.xyzt.z = 3.000000000000000;
    ce.xyzt.t = -4.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "proj=axisswap order=1,2,3,-4", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "proj=axisswap order=-2,1");
    tolerance = 0.000001000000;
    ci.xyzt.x = 1.000000000000000;
    ci.xyzt.y = 2.000000000000000;
    ci.xyzt.z = 3.000000000000000;
    ci.xyzt.t = 4.000000000000000;
    ce.xyzt.x = -2.000000000000000;
    ce.xyzt.y = 1.000000000000000;
    ce.xyzt.z = 3.000000000000000;
    ce.xyzt.t = 4.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "proj=axisswap order=-2,1", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "proj=axisswap order=3,-2,1");
    tolerance = 0.000001000000;
    ci.xyzt.x = 1.000000000000000;
    ci.xyzt.y = 2.000000000000000;
    ci.xyzt.z = 3.000000000000000;
    ci.xyzt.t = 4.000000000000000;
    ce.xyzt.x = 3.000000000000000;
    ce.xyzt.y = -2.000000000000000;
    ce.xyzt.z = 1.000000000000000;
    ce.xyzt.t = 4.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "proj=axisswap order=3,-2,1", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "proj=pipeline step proj=latlong step proj=axisswap order=1,2,3,4 angularunits");
    tolerance = 0.000010000000;
    ci.xyzt.x = 0.209439510239320;
    ci.xyzt.y = 0.959931088596881;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.209439510239320;
    ce.xyzt.y = 0.959931088596881;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "proj=pipeline step proj=latlong step proj=axisswap order=1,2,3,4 angularunits", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "proj=pipeline step proj=latlong step proj=axisswap order=-2,-1,3,4 angularunits");
    tolerance = 0.000010000000;
    ci.xyzt.x = 0.209439510239320;
    ci.xyzt.y = 0.959931088596881;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.959931088596881;
    ce.xyzt.y = -0.209439510239320;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "proj=pipeline step proj=latlong step proj=axisswap order=-2,-1,3,4 angularunits", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=aea ellps=GRS80 +lat_1=0 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222571.608757106010000;
    ce.xyzt.y = 110653.326743030000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=aea ellps=GRS80 +lat_1=0 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222706.306508390990000;
    ce.xyzt.y = -110484.267144400000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=aea ellps=GRS80 +lat_1=0 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222571.608757106010000;
    ce.xyzt.y = 110653.326743030000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=aea ellps=GRS80 +lat_1=0 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222706.306508390990000;
    ce.xyzt.y = -110484.267144400000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=aea ellps=GRS80 +lat_1=0 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031357126393;
    ce.xyzt.y = 0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=aea ellps=GRS80 +lat_1=0 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031357108940;
    ce.xyzt.y = -0.000015784234156;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=aea ellps=GRS80 +lat_1=0 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031357126393;
    ce.xyzt.y = 0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=aea ellps=GRS80 +lat_1=0 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031357108940;
    ce.xyzt.y = -0.000015784234156;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=aea ellps=GRS80 +lat_1=0 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=aea R=6400000 +lat_1=0 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223334.085170885000000;
    ce.xyzt.y = 111780.431884472000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=aea R=6400000 +lat_1=0 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223470.154991687010000;
    ce.xyzt.y = -111610.339430990000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=aea R=6400000 +lat_1=0 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223334.085170885000000;
    ce.xyzt.y = 111780.431884472000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=aea R=6400000 +lat_1=0 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223470.154991687010000;
    ce.xyzt.y = -111610.339430990000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=aea R=6400000 +lat_1=0 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031250015537;
    ce.xyzt.y = 0.000015624990315;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=aea R=6400000 +lat_1=0 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=aea R=6400000 +lat_1=0 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031250015537;
    ce.xyzt.y = 0.000015624990315;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=aea R=6400000 +lat_1=0 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=aea R=6400000 +lat_1=0 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=aeqd ellps=GRS80 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222616.522190052000000;
    ce.xyzt.y = 110596.996549549990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=aeqd ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222616.522190052000000;
    ce.xyzt.y = -110596.996549549990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=aeqd ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222616.522190052000000;
    ce.xyzt.y = 110596.996549549990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=aeqd ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222616.522190052000000;
    ce.xyzt.y = -110596.996549549990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=aeqd ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031357126393;
    ce.xyzt.y = 0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=aeqd ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031357126393;
    ce.xyzt.y = -0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=aeqd ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031357126393;
    ce.xyzt.y = 0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=aeqd ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031357126393;
    ce.xyzt.y = -0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=aeqd ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=aeqd R=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223379.456047271000000;
    ce.xyzt.y = 111723.757570854000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=aeqd R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223379.456047271000000;
    ce.xyzt.y = -111723.757570854000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=aeqd R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223379.456047271000000;
    ce.xyzt.y = 111723.757570854000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=aeqd R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223379.456047271000000;
    ce.xyzt.y = -111723.757570854000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=aeqd R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=aeqd R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=aeqd R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=aeqd R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=aeqd R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=airy a=6400000 +lat_1=0 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 189109.886908621000000;
    ce.xyzt.y = 94583.752387504006000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=airy a=6400000 +lat_1=0 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 189109.886908621000000;
    ce.xyzt.y = -94583.752387504006000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=airy a=6400000 +lat_1=0 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -189109.886908621000000;
    ce.xyzt.y = 94583.752387504006000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=airy a=6400000 +lat_1=0 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -189109.886908621000000;
    ce.xyzt.y = -94583.752387504006000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=airy a=6400000 +lat_1=0 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=aitoff R=6400000 +lat_1=0 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223379.458811696010000;
    ce.xyzt.y = 111706.742883853000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=aitoff R=6400000 +lat_1=0 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223379.458811696010000;
    ce.xyzt.y = -111706.742883853000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=aitoff R=6400000 +lat_1=0 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223379.458811696010000;
    ce.xyzt.y = 111706.742883853000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=aitoff R=6400000 +lat_1=0 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223379.458811696010000;
    ce.xyzt.y = -111706.742883853000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=aitoff R=6400000 +lat_1=0 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=aitoff R=6400000 +lat_1=0 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=aitoff R=6400000 +lat_1=0 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=aitoff R=6400000 +lat_1=0 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=aitoff R=6400000 +lat_1=0 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=alsk +ellps=clrk66");
    tolerance = 0.000000010000;
    ci.xyzt.x = -2.792526803190927;
    ci.xyzt.y = 0.959931088596881;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -513253.146950842000000;
    ce.xyzt.y = -968928.031867943010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=alsk +ellps=clrk66", ci, ce, co);

    ci.xyzt.x = -2.792526803190927;
    ci.xyzt.y = 1.221730476396031;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -305001.133897637020000;
    ce.xyzt.y = 687494.464958650990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=alsk +ellps=clrk66", ci, ce, co);

    ci.xyzt.x = -2.530727415391778;
    ci.xyzt.y = 1.221730476396031;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 266454.305088600030000;
    ce.xyzt.y = 683423.477493030950000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=alsk +ellps=clrk66", ci, ce, co);

    ci.xyzt.x = -2.530727415391778;
    ci.xyzt.y = 1.047197551196598;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 389141.322439244020000;
    ce.xyzt.y = -423913.251230397030000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=alsk +ellps=clrk66", ci, ce, co);

    direction = -1;
    ci.xyzt.x = -500000.000000000000000;
    ci.xyzt.y = -950000.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -2.789573781198071;
    ce.xyzt.y = 0.963128449092835;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=alsk +ellps=clrk66", ci, ce, co);

    ci.xyzt.x = -305000.000000000000000;
    ci.xyzt.y = 700000.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -2.793263387217860;
    ce.xyzt.y = 1.223669307928546;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=alsk +ellps=clrk66", ci, ce, co);

    ci.xyzt.x = 250000.000000000000000;
    ci.xyzt.y = 700000.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -2.537377879950219;
    ce.xyzt.y = 1.224591086887639;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=alsk +ellps=clrk66", ci, ce, co);

    ci.xyzt.x = 400000.000000000000000;
    ci.xyzt.y = -400000.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -2.526520918141052;
    ce.xyzt.y = 1.050739333902489;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=alsk +ellps=clrk66", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=alsk +R=6370997");
    tolerance = 0.000000100000;
    ci.xyzt.x = -2.792526803190927;
    ci.xyzt.y = 0.959931088596881;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -511510.319410844010000;
    ce.xyzt.y = -967150.991676077950000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=alsk +R=6370997", ci, ce, co);

    ci.xyzt.x = -2.792526803190927;
    ci.xyzt.y = 1.221730476396031;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -303744.771290368980000;
    ce.xyzt.y = 685439.745941122990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=alsk +R=6370997", ci, ce, co);

    ci.xyzt.x = -2.530727415391778;
    ci.xyzt.y = 1.221730476396031;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 265354.974019663000000;
    ce.xyzt.y = 681386.892874573010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=alsk +R=6370997", ci, ce, co);

    ci.xyzt.x = -2.530727415391778;
    ci.xyzt.y = 1.047197551196598;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 387711.995394026980000;
    ce.xyzt.y = -422980.685505462990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=alsk +R=6370997", ci, ce, co);

    direction = -1;
    ci.xyzt.x = -500000.000000000000000;
    ci.xyzt.y = -950000.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -2.789978874822718;
    ce.xyzt.y = 0.962822293680533;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=alsk +R=6370997", ci, ce, co);

    ci.xyzt.x = -305000.000000000000000;
    ci.xyzt.y = 700000.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -2.793963774163304;
    ce.xyzt.y = 1.223969866785522;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=alsk +R=6370997", ci, ce, co);

    ci.xyzt.x = 250000.000000000000000;
    ci.xyzt.y = 700000.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -2.536798148872602;
    ce.xyzt.y = 1.224899416945283;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=alsk +R=6370997", ci, ce, co);

    ci.xyzt.x = 400000.000000000000000;
    ci.xyzt.y = -400000.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -2.526089025352258;
    ce.xyzt.y = 1.050575893103191;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=alsk +R=6370997", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=apian a=6400000 +lat_1=0 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223374.577355252990000;
    ce.xyzt.y = 111701.072127637000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=apian a=6400000 +lat_1=0 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223374.577355252990000;
    ce.xyzt.y = -111701.072127637000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=apian a=6400000 +lat_1=0 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223374.577355252990000;
    ce.xyzt.y = 111701.072127637000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=apian a=6400000 +lat_1=0 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223374.577355252990000;
    ce.xyzt.y = -111701.072127637000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=apian a=6400000 +lat_1=0 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=august a=6400000 +lat_1=0 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223404.978180971990000;
    ce.xyzt.y = 111722.340289763000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=august a=6400000 +lat_1=0 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223404.978180971990000;
    ce.xyzt.y = -111722.340289763000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=august a=6400000 +lat_1=0 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223404.978180971990000;
    ce.xyzt.y = 111722.340289763000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=august a=6400000 +lat_1=0 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223404.978180971990000;
    ce.xyzt.y = -111722.340289763000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=august a=6400000 +lat_1=0 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=bacon a=6400000 +lat_1=0 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223334.132555965010000;
    ce.xyzt.y = 175450.725922666000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=bacon a=6400000 +lat_1=0 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223334.132555965010000;
    ce.xyzt.y = -175450.725922666000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=bacon a=6400000 +lat_1=0 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223334.132555965010000;
    ce.xyzt.y = 175450.725922666000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=bacon a=6400000 +lat_1=0 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223334.132555965010000;
    ce.xyzt.y = -175450.725922666000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=bacon a=6400000 +lat_1=0 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=bipc ellps=GRS80 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 2452160.217725756100000;
    ce.xyzt.y = -14548450.759654747000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=bipc ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 2447915.213725341000000;
    ce.xyzt.y = -14763427.212798730000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=bipc ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 2021695.522934909000000;
    ce.xyzt.y = -14540413.695283702000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=bipc ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 2018090.503004699000000;
    ce.xyzt.y = -14755620.651414108000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=bipc ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -1.274765801350571;
    ce.xyzt.y = 0.301036457005734;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=bipc ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -1.274741427024715;
    ce.xyzt.y = 0.301059085408925;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=bipc ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -1.274716808003699;
    ce.xyzt.y = 0.300991391504890;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=bipc ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -1.274692433677843;
    ce.xyzt.y = 0.301014019576469;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=bipc ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=bipc a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 2460565.740974965000000;
    ce.xyzt.y = -14598319.989330800000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=bipc a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 2456306.185935200200000;
    ce.xyzt.y = -14814033.339502094000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=bipc a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 2028625.497819099100000;
    ce.xyzt.y = -14590255.375482792000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=bipc a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 2025008.120589142900000;
    ce.xyzt.y = -14806200.018759441000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=bipc a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -1.274765676035930;
    ce.xyzt.y = 0.301036418678303;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=bipc a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -1.274741384979734;
    ce.xyzt.y = 0.301058969780862;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=bipc a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -1.274716850048680;
    ce.xyzt.y = 0.300991507132953;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=bipc a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -1.274692558992483;
    ce.xyzt.y = 0.301014057903899;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=bipc a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=boggs a=6400000 +lat_1=0 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 211949.700808182010000;
    ce.xyzt.y = 117720.998305411010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=boggs a=6400000 +lat_1=0 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 211949.700808182010000;
    ce.xyzt.y = -117720.998305411010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=boggs a=6400000 +lat_1=0 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -211949.700808182010000;
    ce.xyzt.y = 117720.998305411010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=boggs a=6400000 +lat_1=0 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -211949.700808182010000;
    ce.xyzt.y = -117720.998305411010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=boggs a=6400000 +lat_1=0 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=bonne ellps=GRS80 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222605.296097157000000;
    ce.xyzt.y = 55321.139565495003000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=bonne ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222605.296099239000000;
    ce.xyzt.y = -165827.647799052010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=bonne ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222605.296097157000000;
    ce.xyzt.y = 55321.139565495003000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=bonne ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222605.296099239000000;
    ce.xyzt.y = -165827.647799052010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=bonne ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031358313217;
    ce.xyzt.y = 0.008742430476675;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=bonne ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031358295764;
    ce.xyzt.y = 0.008710862043269;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=bonne ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031358313217;
    ce.xyzt.y = 0.008742430476675;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=bonne ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031358295764;
    ce.xyzt.y = 0.008710862043269;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=bonne ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=bonne R=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223368.115572528010000;
    ce.xyzt.y = 55884.555246393997000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=bonne R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223368.115574631990000;
    ce.xyzt.y = -167517.599369694010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=bonne R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223368.115572528010000;
    ce.xyzt.y = 55884.555246393997000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=bonne R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223368.115574631990000;
    ce.xyzt.y = -167517.599369694010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=bonne R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031251202361;
    ce.xyzt.y = 0.008742271250287;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=bonne R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031251184908;
    ce.xyzt.y = 0.008711021252203;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=bonne R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031251202361;
    ce.xyzt.y = 0.008742271250287;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=bonne R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031251184908;
    ce.xyzt.y = 0.008711021252203;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=bonne R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=calcofi ellps=GRS80 +lat_1=0.5 +lat_2=2 +no_defs");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 508.444872149999980;
    ce.xyzt.y = -1171.764860417999900;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=calcofi ellps=GRS80 +lat_1=0.5 +lat_2=2 +no_defs", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 514.999168152000040;
    ce.xyzt.y = -1145.821981468000100;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=calcofi ellps=GRS80 +lat_1=0.5 +lat_2=2 +no_defs", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 500.685384124999980;
    ce.xyzt.y = -1131.445377920000100;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=calcofi ellps=GRS80 +lat_1=0.5 +lat_2=2 +no_defs", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 507.369719137000000;
    ce.xyzt.y = -1106.178201483000100;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=calcofi ellps=GRS80 +lat_1=0.5 +lat_2=2 +no_defs", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -1.926203096683601;
    ce.xyzt.y = 0.209999010018752;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=calcofi ellps=GRS80 +lat_1=0.5 +lat_2=2 +no_defs", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -1.718364069739549;
    ce.xyzt.y = 0.326354293490859;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=calcofi ellps=GRS80 +lat_1=0.5 +lat_2=2 +no_defs", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -3.620633601060157;
    ce.xyzt.y = 1.419198586179172;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=calcofi ellps=GRS80 +lat_1=0.5 +lat_2=2 +no_defs", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -1.090592071266480;
    ce.xyzt.y = 1.535553869633825;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=calcofi ellps=GRS80 +lat_1=0.5 +lat_2=2 +no_defs", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=calcofi R=6400000 +lat_1=0.5 +lat_2=2 +no_defs");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 507.090507488000010;
    ce.xyzt.y = -1164.727375197999900;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=calcofi R=6400000 +lat_1=0.5 +lat_2=2 +no_defs", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 513.686136375000050;
    ce.xyzt.y = -1138.999268217000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=calcofi R=6400000 +lat_1=0.5 +lat_2=2 +no_defs", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 499.336261476000000;
    ce.xyzt.y = -1124.435130997000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=calcofi R=6400000 +lat_1=0.5 +lat_2=2 +no_defs", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 506.060570392999980;
    ce.xyzt.y = -1099.375665067000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=calcofi R=6400000 +lat_1=0.5 +lat_2=2 +no_defs", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -1.925188754693774;
    ce.xyzt.y = 0.209999010018752;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=calcofi R=6400000 +lat_1=0.5 +lat_2=2 +no_defs", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -1.716048926911800;
    ce.xyzt.y = 0.326354293490859;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=calcofi R=6400000 +lat_1=0.5 +lat_2=2 +no_defs", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -3.622341969649115;
    ce.xyzt.y = 1.419198586179172;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=calcofi R=6400000 +lat_1=0.5 +lat_2=2 +no_defs", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -1.092173819848491;
    ce.xyzt.y = 1.535553869633825;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=calcofi R=6400000 +lat_1=0.5 +lat_2=2 +no_defs", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=cass ellps=GRS80 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222605.285776990990000;
    ce.xyzt.y = 110642.229253999000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=cass ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222605.285776990990000;
    ce.xyzt.y = -110642.229253999000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=cass ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222605.285776990990000;
    ce.xyzt.y = 110642.229253999000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=cass ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222605.285776990990000;
    ce.xyzt.y = -110642.229253999000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=cass ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031357126393;
    ce.xyzt.y = 0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=cass ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031357126393;
    ce.xyzt.y = -0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=cass ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031357126393;
    ce.xyzt.y = 0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=cass ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031357126393;
    ce.xyzt.y = -0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=cass ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=cass R=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223368.105203484010000;
    ce.xyzt.y = 111769.145040585990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=cass R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223368.105203484010000;
    ce.xyzt.y = -111769.145040585990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=cass R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223368.105203484010000;
    ce.xyzt.y = 111769.145040585990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=cass R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223368.105203484010000;
    ce.xyzt.y = -111769.145040585990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=cass R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=cass R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=cass R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=cass R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=cass R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=pipeline +step +proj=ccon +lat_1=52 +lat_0=52 +lon_0=19 +a=6390000 +x_0=330000 +y_0=-350000 +step +proj=axisswap +order=1,-2,3,4");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.418879020478639;
    ci.xyzt.y = 0.959931088596881;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 650031.541094132230000;
    ce.xyzt.y = 4106.161777064360600;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=pipeline +step +proj=ccon +lat_1=52 +lat_0=52 +lon_0=19 +a=6390000 +x_0=330000 +y_0=-350000 +step +proj=axisswap +order=1,-2,3,4", ci, ce, co);

    ci.xyzt.x = 0.261799387799149;
    ci.xyzt.y = 0.855211333477221;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 37074.189007307468000;
    ce.xyzt.y = 676826.235592700420000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=pipeline +step +proj=ccon +lat_1=52 +lat_0=52 +lon_0=19 +a=6390000 +x_0=330000 +y_0=-350000 +step +proj=axisswap +order=1,-2,3,4", ci, ce, co);

    ci.xyzt.x = 0.418879020478639;
    ci.xyzt.y = 0.855211333477221;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 696053.360616178480000;
    ce.xyzt.y = 672294.567958271950000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=pipeline +step +proj=ccon +lat_1=52 +lat_0=52 +lon_0=19 +a=6390000 +x_0=330000 +y_0=-350000 +step +proj=axisswap +order=1,-2,3,4", ci, ce, co);

    ci.xyzt.x = 0.331612557878923;
    ci.xyzt.y = 0.907571211037051;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 330000.000000000000000;
    ce.xyzt.y = 350000.000000000000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=pipeline +step +proj=ccon +lat_1=52 +lat_0=52 +lon_0=19 +a=6390000 +x_0=330000 +y_0=-350000 +step +proj=axisswap +order=1,-2,3,4", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 0.000000000000000;
    ci.xyzt.y = 0.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.241557535932658;
    ce.xyzt.y = 0.960461738391808;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=pipeline +step +proj=ccon +lat_1=52 +lat_0=52 +lon_0=19 +a=6390000 +x_0=330000 +y_0=-350000 +step +proj=axisswap +order=1,-2,3,4", ci, ce, co);

    ci.xyzt.x = 0.000000000000000;
    ci.xyzt.y = 700000.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.253325004353958;
    ce.xyzt.y = 0.851264233583056;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=pipeline +step +proj=ccon +lat_1=52 +lat_0=52 +lon_0=19 +a=6390000 +x_0=330000 +y_0=-350000 +step +proj=axisswap +order=1,-2,3,4", ci, ce, co);

    ci.xyzt.x = 700000.000000000000000;
    ci.xyzt.y = 0.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.432539837923184;
    ce.xyzt.y = 0.959992445737815;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=pipeline +step +proj=ccon +lat_1=52 +lat_0=52 +lon_0=19 +a=6390000 +x_0=330000 +y_0=-350000 +step +proj=axisswap +order=1,-2,3,4", ci, ce, co);

    ci.xyzt.x = 700000.000000000000000;
    ci.xyzt.y = 700000.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.419360919211760;
    ce.xyzt.y = 0.850856319344845;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=pipeline +step +proj=ccon +lat_1=52 +lat_0=52 +lon_0=19 +a=6390000 +x_0=330000 +y_0=-350000 +step +proj=axisswap +order=1,-2,3,4", ci, ce, co);

    ci.xyzt.x = 330000.000000000000000;
    ci.xyzt.y = 350000.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.331612557878923;
    ce.xyzt.y = 0.907571211037051;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=pipeline +step +proj=ccon +lat_1=52 +lat_0=52 +lon_0=19 +a=6390000 +x_0=330000 +y_0=-350000 +step +proj=axisswap +order=1,-2,3,4", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=cc a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223402.144255274000000;
    ce.xyzt.y = 111712.415540593010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=cc a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223402.144255274000000;
    ce.xyzt.y = -111712.415540593010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=cc a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223402.144255274000000;
    ce.xyzt.y = 111712.415540593010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=cc a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223402.144255274000000;
    ce.xyzt.y = -111712.415540593010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=cc a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=cc a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=cc a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=cc a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=cc a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=cea ellps=GRS80 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222638.981586546990000;
    ce.xyzt.y = 110568.812396267010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=cea ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222638.981586546990000;
    ce.xyzt.y = -110568.812396266000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=cea ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222638.981586546990000;
    ce.xyzt.y = 110568.812396267010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=cea ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222638.981586546990000;
    ce.xyzt.y = -110568.812396266000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=cea ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031357126393;
    ce.xyzt.y = 0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=cea ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031357126393;
    ce.xyzt.y = -0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=cea ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031357126393;
    ce.xyzt.y = 0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=cea ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031357126393;
    ce.xyzt.y = -0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=cea ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=cea R=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223402.144255274000000;
    ce.xyzt.y = 111695.401198614000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=cea R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223402.144255274000000;
    ce.xyzt.y = -111695.401198614000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=cea R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223402.144255274000000;
    ce.xyzt.y = 111695.401198614000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=cea R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223402.144255274000000;
    ce.xyzt.y = -111695.401198614000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=cea R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=cea R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=cea R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=cea R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=cea R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=chamb R=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -27864.779586801000000;
    ce.xyzt.y = -223364.324593274010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=chamb R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -251312.283053493010000;
    ce.xyzt.y = -223402.145526208010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=chamb R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -27864.785649105001000;
    ce.xyzt.y = 223364.327328827000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=chamb R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -251312.289116442990000;
    ce.xyzt.y = 223402.142197287000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=chamb R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=collg a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 249872.921577930010000;
    ce.xyzt.y = 99423.174788460004000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=collg a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 254272.532301245000000;
    ce.xyzt.y = -98559.307760743002000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=collg a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -249872.921577930010000;
    ce.xyzt.y = 99423.174788460004000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=collg a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -254272.532301245000000;
    ce.xyzt.y = -98559.307760743002000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=collg a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000027694832211;
    ce.xyzt.y = 0.000017630844865;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=collg a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000027694343519;
    ce.xyzt.y = -0.000017631001944;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=collg a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000027694832211;
    ce.xyzt.y = 0.000017630844865;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=collg a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000027694343519;
    ce.xyzt.y = -0.000017631001944;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=collg a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=comill a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223402.144255274000000;
    ce.xyzt.y = 110611.859089459000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=comill a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223402.144255274000000;
    ce.xyzt.y = -110611.859089459000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=comill a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223402.144255274000000;
    ce.xyzt.y = 110611.859089459000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=comill a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223402.144255274000000;
    ce.xyzt.y = -110611.859089459000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=comill a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = 0.000015779643940;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=comill a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = -0.000015779643940;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=comill a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = 0.000015779643940;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=comill a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = -0.000015779643940;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=comill a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=crast a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 218280.142056781010000;
    ce.xyzt.y = 114306.045604280010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=crast a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 218280.142056781010000;
    ce.xyzt.y = -114306.045604280010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=crast a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -218280.142056781010000;
    ce.xyzt.y = 114306.045604280010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=crast a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -218280.142056781010000;
    ce.xyzt.y = -114306.045604280010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=crast a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031978952299;
    ce.xyzt.y = 0.000015268820975;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=crast a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031978952299;
    ce.xyzt.y = -0.000015268820975;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=crast a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031978952299;
    ce.xyzt.y = 0.000015268820975;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=crast a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031978952299;
    ce.xyzt.y = -0.000015268820975;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=crast a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=denoy a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223377.422876953990000;
    ce.xyzt.y = 111701.072127637000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=denoy a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223377.422876953990000;
    ce.xyzt.y = -111701.072127637000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=denoy a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223377.422876953990000;
    ce.xyzt.y = 111701.072127637000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=denoy a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223377.422876953990000;
    ce.xyzt.y = -111701.072127637000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=denoy a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=eck1 a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 204680.888202951000000;
    ce.xyzt.y = 102912.178426065000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=eck1 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 204680.888202951000000;
    ce.xyzt.y = -102912.178426065000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=eck1 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -204680.888202951000000;
    ce.xyzt.y = 102912.178426065000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=eck1 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -204680.888202951000000;
    ce.xyzt.y = -102912.178426065000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=eck1 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000033918990483;
    ce.xyzt.y = 0.000016959399248;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=eck1 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000033918990483;
    ce.xyzt.y = -0.000016959399248;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=eck1 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000033918990483;
    ce.xyzt.y = 0.000016959399248;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=eck1 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000033918990483;
    ce.xyzt.y = -0.000016959399248;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=eck1 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=eck2 a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 204472.870907959990000;
    ce.xyzt.y = 121633.734975242000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=eck2 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 204472.870907959990000;
    ce.xyzt.y = -121633.734975242000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=eck2 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -204472.870907959990000;
    ce.xyzt.y = 121633.734975242000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=eck2 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -204472.870907959990000;
    ce.xyzt.y = -121633.734975242000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=eck2 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000033918990483;
    ce.xyzt.y = 0.000014395545484;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=eck2 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000033918990483;
    ce.xyzt.y = -0.000014395545484;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=eck2 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000033918990483;
    ce.xyzt.y = 0.000014395545484;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=eck2 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000033918990483;
    ce.xyzt.y = -0.000014395545484;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=eck2 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=eck3 a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 188652.015721537990000;
    ce.xyzt.y = 94328.919337030995000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=eck3 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 188652.015721537990000;
    ce.xyzt.y = -94328.919337030995000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=eck3 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -188652.015721537990000;
    ce.xyzt.y = 94328.919337030995000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=eck3 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -188652.015721537990000;
    ce.xyzt.y = -94328.919337030995000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=eck3 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000037005186386;
    ce.xyzt.y = 0.000018502584466;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=eck3 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000037005186386;
    ce.xyzt.y = -0.000018502584466;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=eck3 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000037005186386;
    ce.xyzt.y = 0.000018502584466;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=eck3 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000037005186386;
    ce.xyzt.y = -0.000018502584466;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=eck3 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=eck4 a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 188646.389356416010000;
    ce.xyzt.y = 132268.540174065010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=eck4 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 188646.389356416010000;
    ce.xyzt.y = -132268.540174065010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=eck4 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -188646.389356416010000;
    ce.xyzt.y = 132268.540174065010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=eck4 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -188646.389356416010000;
    ce.xyzt.y = -132268.540174065010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=eck4 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000037005186386;
    ce.xyzt.y = 0.000013194950944;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=eck4 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000037005186386;
    ce.xyzt.y = -0.000013194950944;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=eck4 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000037005186386;
    ce.xyzt.y = 0.000013194950944;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=eck4 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000037005186386;
    ce.xyzt.y = -0.000013194950944;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=eck4 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=eck5 a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 197031.392134061000000;
    ce.xyzt.y = 98523.198847227002000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=eck5 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 197031.392134061000000;
    ce.xyzt.y = -98523.198847227002000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=eck5 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -197031.392134061000000;
    ce.xyzt.y = 98523.198847227002000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=eck5 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -197031.392134061000000;
    ce.xyzt.y = -98523.198847227002000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=eck5 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000035429817296;
    ce.xyzt.y = 0.000017714899922;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=eck5 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000035429817296;
    ce.xyzt.y = -0.000017714899922;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=eck5 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000035429817296;
    ce.xyzt.y = 0.000017714899922;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=eck5 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000035429817296;
    ce.xyzt.y = -0.000017714899922;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=eck5 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=eck6 a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 197021.605628991990000;
    ce.xyzt.y = 126640.420733174000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=eck6 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 197021.605628991990000;
    ce.xyzt.y = -126640.420733174000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=eck6 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -197021.605628991990000;
    ce.xyzt.y = 126640.420733174000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=eck6 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -197021.605628991990000;
    ce.xyzt.y = -126640.420733174000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=eck6 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000035429817296;
    ce.xyzt.y = 0.000013781643373;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=eck6 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000035429817296;
    ce.xyzt.y = -0.000013781643373;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=eck6 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000035429817296;
    ce.xyzt.y = 0.000013781643373;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=eck6 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000035429817296;
    ce.xyzt.y = -0.000013781643373;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=eck6 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=eqc a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223402.144255274000000;
    ce.xyzt.y = 111701.072127637000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=eqc a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223402.144255274000000;
    ce.xyzt.y = -111701.072127637000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=eqc a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223402.144255274000000;
    ce.xyzt.y = 111701.072127637000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=eqc a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223402.144255274000000;
    ce.xyzt.y = -111701.072127637000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=eqc a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=eqc a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=eqc a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=eqc a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=eqc a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=eqdc ellps=GRS80 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222588.440269285990000;
    ce.xyzt.y = 110659.134907347000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=eqdc ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222756.836702042000000;
    ce.xyzt.y = -110489.578087221000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=eqdc ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222588.440269285990000;
    ce.xyzt.y = 110659.134907347000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=eqdc ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222756.836702042000000;
    ce.xyzt.y = -110489.578087221000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=eqdc ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031352379098;
    ce.xyzt.y = 0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=eqdc ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031352361645;
    ce.xyzt.y = -0.000015784234156;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=eqdc ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031352379098;
    ce.xyzt.y = 0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=eqdc ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031352361645;
    ce.xyzt.y = -0.000015784234156;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=eqdc ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=eqdc R=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223351.088175114010000;
    ce.xyzt.y = 111786.108747174000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=eqdc R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223521.200266734990000;
    ce.xyzt.y = -111615.970741241010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=eqdc R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223351.088175114010000;
    ce.xyzt.y = 111786.108747174000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=eqdc R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223521.200266734990000;
    ce.xyzt.y = -111615.970741241010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=eqdc R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031245250788;
    ce.xyzt.y = 0.000015624990315;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=eqdc R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031245233335;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=eqdc R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031245250788;
    ce.xyzt.y = 0.000015624990315;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=eqdc R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031245233335;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=eqdc R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=euler ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222597.634659108010000;
    ce.xyzt.y = 111404.240549919010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=euler ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222767.165631876010000;
    ce.xyzt.y = -111234.676491018010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=euler ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222597.634659108010000;
    ce.xyzt.y = 111404.240549919010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=euler ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222767.165631876010000;
    ce.xyzt.y = -111234.676491018010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=euler ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031351017741;
    ce.xyzt.y = 0.000015678554470;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=euler ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031350982834;
    ce.xyzt.y = -0.000015678571923;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=euler ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031351017741;
    ce.xyzt.y = 0.000015678554470;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=euler ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031350982834;
    ce.xyzt.y = -0.000015678571923;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=euler ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=euler a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223360.655598693990000;
    ce.xyzt.y = 111786.112389791000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=euler a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223530.767690315990000;
    ce.xyzt.y = -111615.967098623990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=euler a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223360.655598693990000;
    ce.xyzt.y = 111786.112389791000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=euler a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223530.767690315990000;
    ce.xyzt.y = -111615.967098623990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=euler a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031243906885;
    ce.xyzt.y = 0.000015624990315;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=euler a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031243889432;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=euler a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031243906885;
    ce.xyzt.y = 0.000015624990315;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=euler a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031243889432;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=euler a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=etmerc ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5 +zone=30");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222650.796797585990000;
    ce.xyzt.y = 110642.229411933000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=etmerc ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5 +zone=30", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222650.796797585990000;
    ce.xyzt.y = -110642.229411933000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=etmerc ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5 +zone=30", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222650.796797585990000;
    ce.xyzt.y = 110642.229411933000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=etmerc ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5 +zone=30", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222650.796797585990000;
    ce.xyzt.y = -110642.229411933000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=etmerc ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5 +zone=30", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031357126393;
    ce.xyzt.y = 0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=etmerc ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5 +zone=30", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031357126393;
    ce.xyzt.y = -0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=etmerc ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5 +zone=30", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031357126393;
    ce.xyzt.y = 0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=etmerc ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5 +zone=30", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031357126393;
    ce.xyzt.y = -0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=etmerc ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5 +zone=30", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=fahey a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 182993.344649123990000;
    ce.xyzt.y = 101603.193569884000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=fahey a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 182993.344649123990000;
    ce.xyzt.y = -101603.193569884000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=fahey a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -182993.344649123990000;
    ce.xyzt.y = 101603.193569884000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=fahey a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -182993.344649123990000;
    ce.xyzt.y = -101603.193569884000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=fahey a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000038149214804;
    ce.xyzt.y = 0.000017178333350;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=fahey a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000038149214804;
    ce.xyzt.y = -0.000017178333350;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=fahey a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000038149214804;
    ce.xyzt.y = 0.000017178333350;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=fahey a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000038149214804;
    ce.xyzt.y = -0.000017178333350;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=fahey a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=fouc ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222588.120675891990000;
    ce.xyzt.y = 111322.316700694010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=fouc ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222588.120675891990000;
    ce.xyzt.y = -111322.316700694010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=fouc ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222588.120675891990000;
    ce.xyzt.y = 111322.316700694010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=fouc ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222588.120675891990000;
    ce.xyzt.y = -111322.316700694010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=fouc ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031357126393;
    ce.xyzt.y = 0.000015678554470;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=fouc ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031357126393;
    ce.xyzt.y = -0.000015678554470;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=fouc ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031357126393;
    ce.xyzt.y = 0.000015678554470;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=fouc ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031357126393;
    ce.xyzt.y = -0.000015678554470;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=fouc ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=fouc a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223351.109003413990000;
    ce.xyzt.y = 111703.907721713000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=fouc a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223351.109003413990000;
    ce.xyzt.y = -111703.907721713000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=fouc a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223351.109003413990000;
    ce.xyzt.y = 111703.907721713000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=fouc a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223351.109003413990000;
    ce.xyzt.y = -111703.907721713000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=fouc a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=fouc a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=fouc a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=fouc a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=fouc a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=fouc_s a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223402.144255274000000;
    ce.xyzt.y = 111695.401198614000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=fouc_s a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223402.144255274000000;
    ce.xyzt.y = -111695.401198614000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=fouc_s a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223402.144255274000000;
    ce.xyzt.y = 111695.401198614000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=fouc_s a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223402.144255274000000;
    ce.xyzt.y = -111695.401198614000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=fouc_s a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=fouc_s a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=fouc_s a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=fouc_s a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=fouc_s a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=gall a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 157969.171134520000000;
    ce.xyzt.y = 95345.249178386002000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=gall a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 157969.171134520000000;
    ce.xyzt.y = -95345.249178386002000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=gall a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -157969.171134520000000;
    ce.xyzt.y = 95345.249178386002000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=gall a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -157969.171134520000000;
    ce.xyzt.y = -95345.249178386002000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=gall a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000044194180121;
    ce.xyzt.y = 0.000018305833500;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=gall a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000044194180121;
    ce.xyzt.y = -0.000018305833500;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=gall a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000044194180121;
    ce.xyzt.y = 0.000018305833500;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=gall a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000044194180121;
    ce.xyzt.y = -0.000018305833500;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=gall a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=geocent ellps=GRS80 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222638.981586546990000;
    ce.xyzt.y = 111319.490793274000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=geocent ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222638.981586546990000;
    ce.xyzt.y = -111319.490793274000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=geocent ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222638.981586546990000;
    ce.xyzt.y = 111319.490793274000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=geocent ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222638.981586546990000;
    ce.xyzt.y = -111319.490793274000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=geocent ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031357126393;
    ce.xyzt.y = 0.000015678554470;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=geocent ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031357126393;
    ce.xyzt.y = -0.000015678554470;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=geocent ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031357126393;
    ce.xyzt.y = 0.000015678554470;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=geocent ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031357126393;
    ce.xyzt.y = -0.000015678554470;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=geocent ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=geocent a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=geos ellps=GRS80 +lat_1=0.5 +lat_2=2 +h=35785831");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222527.070365800000000;
    ce.xyzt.y = 110551.303413329010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=geos ellps=GRS80 +lat_1=0.5 +lat_2=2 +h=35785831", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222527.070365800000000;
    ce.xyzt.y = -110551.303413329010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=geos ellps=GRS80 +lat_1=0.5 +lat_2=2 +h=35785831", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222527.070365800000000;
    ce.xyzt.y = 110551.303413329010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=geos ellps=GRS80 +lat_1=0.5 +lat_2=2 +h=35785831", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222527.070365800000000;
    ce.xyzt.y = -110551.303413329010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=geos ellps=GRS80 +lat_1=0.5 +lat_2=2 +h=35785831", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031357126393;
    ce.xyzt.y = 0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=geos ellps=GRS80 +lat_1=0.5 +lat_2=2 +h=35785831", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031357126393;
    ce.xyzt.y = -0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=geos ellps=GRS80 +lat_1=0.5 +lat_2=2 +h=35785831", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031357126393;
    ce.xyzt.y = 0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=geos ellps=GRS80 +lat_1=0.5 +lat_2=2 +h=35785831", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031357126393;
    ce.xyzt.y = -0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=geos ellps=GRS80 +lat_1=0.5 +lat_2=2 +h=35785831", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=geos R=6400000 +lat_1=0.5 +lat_2=2 +h=35785831");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223289.457635795000000;
    ce.xyzt.y = 111677.657456537000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=geos R=6400000 +lat_1=0.5 +lat_2=2 +h=35785831", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223289.457635795000000;
    ce.xyzt.y = -111677.657456537000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=geos R=6400000 +lat_1=0.5 +lat_2=2 +h=35785831", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223289.457635795000000;
    ce.xyzt.y = 111677.657456537000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=geos R=6400000 +lat_1=0.5 +lat_2=2 +h=35785831", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223289.457635795000000;
    ce.xyzt.y = -111677.657456537000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=geos R=6400000 +lat_1=0.5 +lat_2=2 +h=35785831", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=geos R=6400000 +lat_1=0.5 +lat_2=2 +h=35785831", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=geos R=6400000 +lat_1=0.5 +lat_2=2 +h=35785831", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=geos R=6400000 +lat_1=0.5 +lat_2=2 +h=35785831", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=geos R=6400000 +lat_1=0.5 +lat_2=2 +h=35785831", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=gins8 a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 194350.250939590010000;
    ce.xyzt.y = 111703.907635335000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=gins8 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 194350.250939590010000;
    ce.xyzt.y = -111703.907635335000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=gins8 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -194350.250939590010000;
    ce.xyzt.y = 111703.907635335000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=gins8 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -194350.250939590010000;
    ce.xyzt.y = -111703.907635335000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=gins8 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=gn_sinu a=6400000 +lat_1=0.5 +lat_2=2 +m=1 n=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223385.132504696000000;
    ce.xyzt.y = 111698.236447187000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=gn_sinu a=6400000 +lat_1=0.5 +lat_2=2 +m=1 n=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223385.132504696000000;
    ce.xyzt.y = -111698.236447187000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=gn_sinu a=6400000 +lat_1=0.5 +lat_2=2 +m=1 n=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223385.132504696000000;
    ce.xyzt.y = 111698.236447187000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=gn_sinu a=6400000 +lat_1=0.5 +lat_2=2 +m=1 n=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223385.132504696000000;
    ce.xyzt.y = -111698.236447187000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=gn_sinu a=6400000 +lat_1=0.5 +lat_2=2 +m=1 n=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=gn_sinu a=6400000 +lat_1=0.5 +lat_2=2 +m=1 n=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=gn_sinu a=6400000 +lat_1=0.5 +lat_2=2 +m=1 n=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=gn_sinu a=6400000 +lat_1=0.5 +lat_2=2 +m=1 n=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=gn_sinu a=6400000 +lat_1=0.5 +lat_2=2 +m=1 n=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=gnom a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223492.924747185000000;
    ce.xyzt.y = 111780.509206592990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=gnom a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223492.924747185000000;
    ce.xyzt.y = -111780.509206592990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=gnom a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223492.924747185000000;
    ce.xyzt.y = 111780.509206592990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=gnom a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223492.924747185000000;
    ce.xyzt.y = -111780.509206592990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=gnom a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=gnom a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=gnom a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=gnom a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=gnom a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=goode a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223368.119026631990000;
    ce.xyzt.y = 111701.072127637000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=goode a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223368.119026631990000;
    ce.xyzt.y = -111701.072127637000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=goode a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223368.119026631990000;
    ce.xyzt.y = 111701.072127637000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=goode a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223368.119026631990000;
    ce.xyzt.y = -111701.072127637000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=goode a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=goode a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=goode a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=goode a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=goode a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=gs48 +R=6370997");
    tolerance = 0.000000100000;
    ci.xyzt.x = -2.076941809873252;
    ci.xyzt.y = 0.698131700797732;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -1923908.446529346100000;
    ce.xyzt.y = 355874.658944479020000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=gs48 +R=6370997", ci, ce, co);

    ci.xyzt.x = -1.221730476396031;
    ci.xyzt.y = 1.117010721276371;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 1354020.375109297900000;
    ce.xyzt.y = 3040846.007866525100000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=gs48 +R=6370997", ci, ce, co);

    ci.xyzt.x = -1.396263401595464;
    ci.xyzt.y = 0.436332312998582;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 1625139.160484320000000;
    ce.xyzt.y = -1413614.894029108100000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=gs48 +R=6370997", ci, ce, co);

    ci.xyzt.x = -1.658062789394613;
    ci.xyzt.y = 0.610865238198015;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 90241.658071458005000;
    ce.xyzt.y = -439595.048485902020000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=gs48 +R=6370997", ci, ce, co);

    direction = -1;
    ci.xyzt.x = -1923000.000000000000000;
    ci.xyzt.y = 355000.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -2.076716882538123;
    ce.xyzt.y = 0.698034831341601;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=gs48 +R=6370997", ci, ce, co);

    ci.xyzt.x = 1354000.000000000000000;
    ci.xyzt.y = 3040000.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -1.221821390579314;
    ce.xyzt.y = 1.116895317243889;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=gs48 +R=6370997", ci, ce, co);

    ci.xyzt.x = 1625000.000000000000000;
    ci.xyzt.y = -1413000.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -1.396269451081184;
    ce.xyzt.y = 0.436430095890230;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=gs48 +R=6370997", ci, ce, co);

    ci.xyzt.x = 90000.000000000000000;
    ci.xyzt.y = -439000.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -1.658108280930327;
    ce.xyzt.y = 0.610959917161215;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=gs48 +R=6370997", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=gs50 +ellps=clrk66");
    tolerance = 0.000000100000;
    ci.xyzt.x = -2.792526803190927;
    ci.xyzt.y = 1.134464013796314;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -1874628.537740232900000;
    ce.xyzt.y = 2660907.942291014800000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=gs50 +ellps=clrk66", ci, ce, co);

    ci.xyzt.x = -2.268928027592629;
    ci.xyzt.y = 0.785398163397448;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -771831.518853335990000;
    ce.xyzt.y = 48465.166491304997000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=gs50 +ellps=clrk66", ci, ce, co);

    ci.xyzt.x = -1.134464013796314;
    ci.xyzt.y = 0.785398163397448;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 4030931.833981508900000;
    ce.xyzt.y = 1323687.864777399000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=gs50 +ellps=clrk66", ci, ce, co);

    ci.xyzt.x = -1.396263401595464;
    ci.xyzt.y = 0.628318530717959;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 3450764.261536100900000;
    ce.xyzt.y = -175619.041820732000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=gs50 +ellps=clrk66", ci, ce, co);

    direction = -1;
    ci.xyzt.x = -1800000.000000000000000;
    ci.xyzt.y = 2600000.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -2.757433206121689;
    ce.xyzt.y = 1.131873240247870;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=gs50 +ellps=clrk66", ci, ce, co);

    ci.xyzt.x = -800000.000000000000000;
    ci.xyzt.y = 500000.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -2.289372648068253;
    ce.xyzt.y = 0.856694335309505;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=gs50 +ellps=clrk66", ci, ce, co);

    ci.xyzt.x = 4000000.000000000000000;
    ci.xyzt.y = 1300000.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -1.143043505849263;
    ce.xyzt.y = 0.785273161589970;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=gs50 +ellps=clrk66", ci, ce, co);

    ci.xyzt.x = 3900000.000000000000000;
    ci.xyzt.y = -170000.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -1.318607770643029;
    ce.xyzt.y = 0.596747515551179;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=gs50 +ellps=clrk66", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=gs50 +R=6370997");
    tolerance = 0.000000100000;
    ci.xyzt.x = -2.792526803190927;
    ci.xyzt.y = 1.134464013796314;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -1867268.253460008900000;
    ce.xyzt.y = 2656506.230401822800000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=gs50 +R=6370997", ci, ce, co);

    ci.xyzt.x = -2.268928027592629;
    ci.xyzt.y = 0.785398163397448;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -769572.189672993960000;
    ce.xyzt.y = 48324.312440864000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=gs50 +R=6370997", ci, ce, co);

    ci.xyzt.x = -1.134464013796314;
    ci.xyzt.y = 0.785398163397448;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 4019393.068680791200000;
    ce.xyzt.y = 1320191.309350288900000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=gs50 +R=6370997", ci, ce, co);

    ci.xyzt.x = -1.396263401595464;
    ci.xyzt.y = 0.628318530717959;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 3442685.615172346100000;
    ce.xyzt.y = -178760.423489429000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=gs50 +R=6370997", ci, ce, co);

    direction = -1;
    ci.xyzt.x = -1800000.000000000000000;
    ci.xyzt.y = 2600000.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -2.760470254338483;
    ce.xyzt.y = 1.131920866007100;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=gs50 +R=6370997", ci, ce, co);

    ci.xyzt.x = -800000.000000000000000;
    ci.xyzt.y = 500000.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -2.289990957013537;
    ce.xyzt.y = 0.856658479352618;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=gs50 +R=6370997", ci, ce, co);

    ci.xyzt.x = 4000000.000000000000000;
    ci.xyzt.y = 1300000.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -1.140554256811863;
    ce.xyzt.y = 0.784652780083652;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=gs50 +R=6370997", ci, ce, co);

    ci.xyzt.x = 3900000.000000000000000;
    ci.xyzt.y = -170000.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -1.316795423383205;
    ce.xyzt.y = 0.596647894775469;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=gs50 +R=6370997", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=hammer a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223373.788703241010000;
    ce.xyzt.y = 111703.907397767010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=hammer a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223373.788703241010000;
    ce.xyzt.y = -111703.907397767010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=hammer a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223373.788703241010000;
    ce.xyzt.y = 111703.907397767010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=hammer a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223373.788703241010000;
    ce.xyzt.y = -111703.907397767010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=hammer a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=hammer a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=hammer a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=hammer a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=hammer a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=hatano a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 189878.878946528010000;
    ce.xyzt.y = 131409.802440625990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=hatano a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 189881.081952445010000;
    ce.xyzt.y = -131409.142276074010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=hatano a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -189878.878946528010000;
    ce.xyzt.y = 131409.802440625990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=hatano a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -189881.081952445010000;
    ce.xyzt.y = -131409.142276074010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=hatano a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000036764697468;
    ce.xyzt.y = 0.000013281205116;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=hatano a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000036764697468;
    ce.xyzt.y = -0.000013281222569;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=hatano a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000036764697468;
    ce.xyzt.y = 0.000013281205116;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=hatano a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000036764697468;
    ce.xyzt.y = -0.000013281222569;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=hatano a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=healpix ellps=GRS80 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222390.103949239010000;
    ce.xyzt.y = 130406.588664482000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=healpix ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222390.103949239010000;
    ce.xyzt.y = -130406.588664480990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=healpix ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222390.103949239010000;
    ce.xyzt.y = 130406.588664482000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=healpix ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222390.103949239010000;
    ce.xyzt.y = -130406.588664480990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=healpix ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031392207511;
    ce.xyzt.y = 0.000013383097438;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=healpix ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031392207511;
    ce.xyzt.y = -0.000013383097438;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=healpix ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031392207511;
    ce.xyzt.y = 0.000013383097438;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=healpix ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031392207511;
    ce.xyzt.y = -0.000013383097438;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=healpix ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=healpix R=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223402.144255274000000;
    ce.xyzt.y = 131588.044441998990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=healpix R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223402.144255274000000;
    ce.xyzt.y = -131588.044441998990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=healpix R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223402.144255274000000;
    ce.xyzt.y = 131588.044441998990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=healpix R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223402.144255274000000;
    ce.xyzt.y = -131588.044441998990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=healpix R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = 0.000013262914066;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=healpix R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = -0.000013262914066;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=healpix R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = 0.000013262914066;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=healpix R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = -0.000013262914066;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=healpix R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=rhealpix ellps=GRS80 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222390.103949239010000;
    ce.xyzt.y = 130406.588664482000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=rhealpix ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222390.103949239010000;
    ce.xyzt.y = -130406.588664480990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=rhealpix ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222390.103949239010000;
    ce.xyzt.y = 130406.588664482000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=rhealpix ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222390.103949239010000;
    ce.xyzt.y = -130406.588664480990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=rhealpix ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031392207511;
    ce.xyzt.y = 0.000013383097438;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=rhealpix ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031392207511;
    ce.xyzt.y = -0.000013383097438;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=rhealpix ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031392207511;
    ce.xyzt.y = 0.000013383097438;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=rhealpix ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031392207511;
    ce.xyzt.y = -0.000013383097438;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=rhealpix ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=rhealpix R=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223402.144255274000000;
    ce.xyzt.y = 131588.044441998990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=rhealpix R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223402.144255274000000;
    ce.xyzt.y = -131588.044441998990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=rhealpix R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223402.144255274000000;
    ce.xyzt.y = 131588.044441998990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=rhealpix R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223402.144255274000000;
    ce.xyzt.y = -131588.044441998990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=rhealpix R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = 0.000013262914066;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=rhealpix R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = -0.000013262914066;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=rhealpix R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = 0.000013262914066;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=rhealpix R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = -0.000013262914066;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=rhealpix R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=igh a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223878.497456271000000;
    ce.xyzt.y = 111701.072127637000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=igh a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223708.371313058010000;
    ce.xyzt.y = -111701.072127637000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=igh a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222857.740596991990000;
    ce.xyzt.y = 111701.072127637000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=igh a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223027.866740205010000;
    ce.xyzt.y = -111701.072127637000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=igh a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249928271;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=igh a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249963177;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=igh a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031250067897;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=igh a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031250050444;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=igh a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=imw_p ellps=GRS80 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222588.441139375990000;
    ce.xyzt.y = 55321.128653810003000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=imw_p ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222756.906377687000000;
    ce.xyzt.y = -165827.584288324000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=imw_p ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222588.441139375990000;
    ce.xyzt.y = 55321.128653810003000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=imw_p ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222756.906377687000000;
    ce.xyzt.y = -165827.584288324000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=imw_p ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031358313217;
    ce.xyzt.y = 0.008742440163252;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=imw_p ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031358295764;
    ce.xyzt.y = 0.008710852356691;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=imw_p ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031358313217;
    ce.xyzt.y = 0.008742440163252;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=imw_p ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031358295764;
    ce.xyzt.y = 0.008710852356691;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=imw_p ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=isea a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -1097074.948022474100000;
    ce.xyzt.y = 3442909.309037182900000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=isea a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -1097074.948264794900000;
    ce.xyzt.y = 3233611.728585708000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=isea a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -1575486.353641554000000;
    ce.xyzt.y = 3442168.342028188100000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=isea a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -1575486.353880283000000;
    ce.xyzt.y = 3234352.695594706100000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=isea a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=kav5 ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 200360.905308829010000;
    ce.xyzt.y = 123685.082476998000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=kav5 ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 200360.905308829010000;
    ce.xyzt.y = -123685.082476998000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=kav5 ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -200360.905308829010000;
    ce.xyzt.y = 123685.082476998000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=kav5 ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -200360.905308829010000;
    ce.xyzt.y = -123685.082476998000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=kav5 ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000034841292273;
    ce.xyzt.y = 0.000014110690296;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=kav5 ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000034841292273;
    ce.xyzt.y = -0.000014110690296;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=kav5 ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000034841292273;
    ce.xyzt.y = 0.000014110690296;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=kav5 ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000034841292273;
    ce.xyzt.y = -0.000014110690296;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=kav5 ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=kav5 a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 201047.703110878010000;
    ce.xyzt.y = 124109.050629171000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=kav5 a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 201047.703110878010000;
    ce.xyzt.y = -124109.050629171000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=kav5 a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -201047.703110878010000;
    ce.xyzt.y = 124109.050629171000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=kav5 a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -201047.703110878010000;
    ce.xyzt.y = -124109.050629171000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=kav5 a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000034722278271;
    ce.xyzt.y = 0.000014062484302;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=kav5 a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000034722278271;
    ce.xyzt.y = -0.000014062484302;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=kav5 a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000034722278271;
    ce.xyzt.y = 0.000014062484302;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=kav5 a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000034722278271;
    ce.xyzt.y = -0.000014062484302;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=kav5 a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=kav7 a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 193462.974943729000000;
    ce.xyzt.y = 111701.072127637000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=kav7 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 193462.974943729000000;
    ce.xyzt.y = -111701.072127637000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=kav7 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -193462.974943729000000;
    ce.xyzt.y = 111701.072127637000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=kav7 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -193462.974943729000000;
    ce.xyzt.y = -111701.072127637000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=kav7 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000036084385579;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=kav7 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000036084385579;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=kav7 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000036084385579;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=kav7 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000036084385579;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=kav7 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=krovak +ellps=GRS80 no_defs");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -3196535.232563640900000;
    ce.xyzt.y = -6617878.867551444100000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=krovak +ellps=GRS80 no_defs", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -3260035.440552108900000;
    ce.xyzt.y = -6898873.614878031400000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=krovak +ellps=GRS80 no_defs", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -3756305.328869175200000;
    ce.xyzt.y = -6478142.561571511400000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=krovak +ellps=GRS80 no_defs", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -3831703.658501982200000;
    ce.xyzt.y = -6759107.170155394800000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=krovak +ellps=GRS80 no_defs", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.433473793882657;
    ce.xyzt.y = 1.042980904367579;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=krovak +ellps=GRS80 no_defs", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.433475479277302;
    ce.xyzt.y = 1.042954453780592;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=krovak +ellps=GRS80 no_defs", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.433373067947011;
    ce.xyzt.y = 1.042980904367579;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=krovak +ellps=GRS80 no_defs", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.433371382552366;
    ce.xyzt.y = 1.042954453780592;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=krovak +ellps=GRS80 no_defs", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=labrd ellps=GRS80 +lon_0=0.5 +lat_0=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 166973.166090228010000;
    ce.xyzt.y = -110536.912730266010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=labrd ellps=GRS80 +lon_0=0.5 +lat_0=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 166973.168287156990000;
    ce.xyzt.y = -331761.993650884020000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=labrd ellps=GRS80 +lon_0=0.5 +lat_0=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -278345.500519976020000;
    ce.xyzt.y = -110469.032642032010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=labrd ellps=GRS80 +lon_0=0.5 +lat_0=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -278345.504185269990000;
    ce.xyzt.y = -331829.870790274990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=labrd ellps=GRS80 +lon_0=0.5 +lat_0=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.008758022375547;
    ce.xyzt.y = 0.034922369047150;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=labrd ellps=GRS80 +lon_0=0.5 +lat_0=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.008758022340641;
    ce.xyzt.y = 0.034890800997717;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=labrd ellps=GRS80 +lon_0=0.5 +lat_0=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.008695270144396;
    ce.xyzt.y = 0.034922369047150;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=labrd ellps=GRS80 +lon_0=0.5 +lat_0=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.008695270179303;
    ce.xyzt.y = 0.034890800997717;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=labrd ellps=GRS80 +lon_0=0.5 +lat_0=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=laea ellps=GRS80 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222602.471450095010000;
    ce.xyzt.y = 110589.827224409990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=laea ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222602.471450095010000;
    ce.xyzt.y = -110589.827224409000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=laea ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222602.471450095010000;
    ce.xyzt.y = 110589.827224409990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=laea ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222602.471450095010000;
    ce.xyzt.y = -110589.827224409000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=laea ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031357126393;
    ce.xyzt.y = 0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=laea ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031357126393;
    ce.xyzt.y = -0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=laea ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031357126393;
    ce.xyzt.y = 0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=laea ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031357126393;
    ce.xyzt.y = -0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=laea ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=laea R=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223365.281370125010000;
    ce.xyzt.y = 111716.668072916000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=laea R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223365.281370125010000;
    ce.xyzt.y = -111716.668072916000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=laea R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223365.281370125010000;
    ce.xyzt.y = 111716.668072916000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=laea R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223365.281370125010000;
    ce.xyzt.y = -111716.668072916000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=laea R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=laea R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=laea R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=laea R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=laea R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=lagrng a=6400000 +W=2 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 111703.375917226000000;
    ce.xyzt.y = 27929.831908033000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=lagrng a=6400000 +W=2 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 111699.122088816000000;
    ce.xyzt.y = -83784.178013358003000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=lagrng a=6400000 +W=2 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -111703.375917226000000;
    ce.xyzt.y = 27929.831908033000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=lagrng a=6400000 +W=2 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -111699.122088816000000;
    ce.xyzt.y = -83784.178013358003000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=lagrng a=6400000 +W=2 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=larr a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223393.637624200990000;
    ce.xyzt.y = 111707.215961256010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=larr a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223393.637624200990000;
    ce.xyzt.y = -111707.215961256010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=larr a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223393.637624200990000;
    ce.xyzt.y = 111707.215961256010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=larr a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223393.637624200990000;
    ce.xyzt.y = -111707.215961256010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=larr a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=lask a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 217928.275907355010000;
    ce.xyzt.y = 112144.329220142000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=lask a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 217928.275907355010000;
    ce.xyzt.y = -112144.329220142000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=lask a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -217928.275907355010000;
    ce.xyzt.y = 112144.329220142000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=lask a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -217928.275907355010000;
    ce.xyzt.y = -112144.329220142000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=lask a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=lcc ellps=GRS80 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222588.439735967990000;
    ce.xyzt.y = 110660.533870800010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=lcc ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222756.879700279010000;
    ce.xyzt.y = -110532.797660827000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=lcc ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222588.439735967990000;
    ce.xyzt.y = 110660.533870800010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=lcc ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222756.879700279010000;
    ce.xyzt.y = -110532.797660827000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=lcc ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031352379098;
    ce.xyzt.y = 0.000015781825602;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=lcc ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031352361645;
    ce.xyzt.y = -0.000015781843055;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=lcc ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031352379098;
    ce.xyzt.y = 0.000015781825602;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=lcc ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031352361645;
    ce.xyzt.y = -0.000015781843055;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=lcc ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=lcca ellps=GRS80 +lat_0=1 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222605.285770237010000;
    ce.xyzt.y = 67.806007272000002;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=lcca ellps=GRS80 +lat_0=1 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222740.037637937000000;
    ce.xyzt.y = -221125.539829602000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=lcca ellps=GRS80 +lat_0=1 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222605.285770237010000;
    ce.xyzt.y = 67.806007272000002;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=lcca ellps=GRS80 +lat_0=1 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222740.037637937000000;
    ce.xyzt.y = -221125.539829602000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=lcca ellps=GRS80 +lat_0=1 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031361873689;
    ce.xyzt.y = 0.017469076684286;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=lcca ellps=GRS80 +lat_0=1 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031361856236;
    ce.xyzt.y = 0.017437508338147;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=lcca ellps=GRS80 +lat_0=1 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031361873689;
    ce.xyzt.y = 0.017469076684286;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=lcca ellps=GRS80 +lat_0=1 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031361856236;
    ce.xyzt.y = 0.017437508338147;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=lcca ellps=GRS80 +lat_0=1 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=leac ellps=GRS80 +lat_1=0 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 220685.140542979000000;
    ce.xyzt.y = 112983.500889396000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=leac ellps=GRS80 +lat_1=0 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 224553.312279826000000;
    ce.xyzt.y = -108128.636744873000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=leac ellps=GRS80 +lat_1=0 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -220685.140542979000000;
    ce.xyzt.y = 112983.500889396000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=leac ellps=GRS80 +lat_1=0 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -224553.312279826000000;
    ce.xyzt.y = -108128.636744873000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=leac ellps=GRS80 +lat_1=0 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031357370739;
    ce.xyzt.y = 0.000015783919997;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=leac ellps=GRS80 +lat_1=0 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031356864594;
    ce.xyzt.y = -0.000015784530862;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=leac ellps=GRS80 +lat_1=0 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031357370739;
    ce.xyzt.y = 0.000015783919997;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=leac ellps=GRS80 +lat_1=0 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031356864594;
    ce.xyzt.y = -0.000015784530862;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=leac ellps=GRS80 +lat_1=0 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=leac R=6400000 +lat_1=0 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 221432.868592852000000;
    ce.xyzt.y = 114119.454526532000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=leac R=6400000 +lat_1=0 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 225331.724127111000000;
    ce.xyzt.y = -109245.829435056000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=leac R=6400000 +lat_1=0 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -221432.868592852000000;
    ce.xyzt.y = 114119.454526532000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=leac R=6400000 +lat_1=0 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -225331.724127111000000;
    ce.xyzt.y = -109245.829435056000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=leac R=6400000 +lat_1=0 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031250242430;
    ce.xyzt.y = 0.000015624693609;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=leac R=6400000 +lat_1=0 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249753738;
    ce.xyzt.y = -0.000015625304475;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=leac R=6400000 +lat_1=0 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031250242430;
    ce.xyzt.y = 0.000015624693609;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=leac R=6400000 +lat_1=0 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249753738;
    ce.xyzt.y = -0.000015625304475;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=leac R=6400000 +lat_1=0 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=lee_os R=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000001000000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -25564478.952605054000000;
    ce.xyzt.y = 154490848.828625500000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=lee_os R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 30115393.938574638000000;
    ce.xyzt.y = 125193997.439701940000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=lee_os R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -31039340.592166007000000;
    ce.xyzt.y = 57678685.044891536000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=lee_os R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -3088419.939423570900000;
    ce.xyzt.y = 58150091.099111006000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=lee_os R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -2.879749274033809;
    ce.xyzt.y = -0.174511263237408;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=lee_os R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -2.879749273702196;
    ce.xyzt.y = -0.174554586829845;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=lee_os R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -2.879837257547479;
    ce.xyzt.y = -0.174511263237408;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=lee_os R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -2.879837257879091;
    ce.xyzt.y = -0.174554586829845;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=lee_os R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=loxim a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223382.295791339010000;
    ce.xyzt.y = 55850.536063819003000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=loxim a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223393.637462243000000;
    ce.xyzt.y = -167551.608191456010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=loxim a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223382.295791339010000;
    ce.xyzt.y = 55850.536063819003000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=loxim a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223393.637462243000000;
    ce.xyzt.y = -167551.608191456010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=loxim a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031251184908;
    ce.xyzt.y = 0.008742271267740;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=loxim a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031251184908;
    ce.xyzt.y = 0.008711021252203;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=loxim a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031251184908;
    ce.xyzt.y = 0.008742271267740;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=loxim a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031251184908;
    ce.xyzt.y = 0.008711021252203;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=loxim a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=lsat +ellps=GRS80 +lat_1=0.5 +lat_2=2 +lsat=1 +path=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 18241950.014558554000000;
    ce.xyzt.y = 9998256.839822936800000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=lsat +ellps=GRS80 +lat_1=0.5 +lat_2=2 +lsat=1 +path=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 18746856.253319457000000;
    ce.xyzt.y = 10215761.669925213000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=lsat +ellps=GRS80 +lat_1=0.5 +lat_2=2 +lsat=1 +path=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 18565503.683633164000000;
    ce.xyzt.y = 9085039.146727051600000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=lsat +ellps=GRS80 +lat_1=0.5 +lat_2=2 +lsat=1 +path=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 19019696.902028911000000;
    ce.xyzt.y = 9247763.039432821800000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=lsat +ellps=GRS80 +lat_1=0.5 +lat_2=2 +lsat=1 +path=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 2.199122254829090;
    ce.xyzt.y = 0.000030085671487;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=lsat +ellps=GRS80 +lat_1=0.5 +lat_2=2 +lsat=1 +path=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 2.199153494529732;
    ce.xyzt.y = 0.000032814895198;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=lsat +ellps=GRS80 +lat_1=0.5 +lat_2=2 +lsat=1 +path=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 2.199127676415159;
    ce.xyzt.y = -0.000032814895198;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=lsat +ellps=GRS80 +lat_1=0.5 +lat_2=2 +lsat=1 +path=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 2.199158916133253;
    ce.xyzt.y = -0.000030085671487;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=lsat +ellps=GRS80 +lat_1=0.5 +lat_2=2 +lsat=1 +path=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=mbt_s ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 204131.517850273000000;
    ce.xyzt.y = 121400.330225508010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=mbt_s ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 204131.517850273000000;
    ce.xyzt.y = -121400.330225508010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=mbt_s ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -204131.517850273000000;
    ce.xyzt.y = 121400.330225508010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=mbt_s ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -204131.517850273000000;
    ce.xyzt.y = -121400.330225508010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=mbt_s ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000034197684658;
    ce.xyzt.y = 0.000014376259595;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=mbt_s ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000034197684658;
    ce.xyzt.y = -0.000014376259595;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=mbt_s ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000034197684658;
    ce.xyzt.y = 0.000014376259595;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=mbt_s ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000034197684658;
    ce.xyzt.y = -0.000014376259595;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=mbt_s ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=mbt_s a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 204831.240570992000000;
    ce.xyzt.y = 121816.466696035000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=mbt_s a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 204831.240570992000000;
    ce.xyzt.y = -121816.466696035000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=mbt_s a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -204831.240570992000000;
    ce.xyzt.y = 121816.466696035000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=mbt_s a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -204831.240570992000000;
    ce.xyzt.y = -121816.466696035000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=mbt_s a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000034080852317;
    ce.xyzt.y = 0.000014327146030;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=mbt_s a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000034080852317;
    ce.xyzt.y = -0.000014327146030;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=mbt_s a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000034080852317;
    ce.xyzt.y = 0.000014327146030;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=mbt_s a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000034080852317;
    ce.xyzt.y = -0.000014327146030;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=mbt_s a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=mbt_fps a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 198798.176129850010000;
    ce.xyzt.y = 125512.017254531010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=mbt_fps a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 198798.176129850010000;
    ce.xyzt.y = -125512.017254531010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=mbt_fps a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -198798.176129850010000;
    ce.xyzt.y = 125512.017254531010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=mbt_fps a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -198798.176129850010000;
    ce.xyzt.y = -125512.017254531010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=mbt_fps a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000035115518405;
    ce.xyzt.y = 0.000013905247590;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=mbt_fps a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000035115518405;
    ce.xyzt.y = -0.000013905247590;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=mbt_fps a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000035115518405;
    ce.xyzt.y = 0.000013905247590;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=mbt_fps a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000035115518405;
    ce.xyzt.y = -0.000013905247590;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=mbt_fps a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=mbtfpp a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 206804.786929819990000;
    ce.xyzt.y = 120649.762565793000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=mbtfpp a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 206804.786929819990000;
    ce.xyzt.y = -120649.762565793000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=mbtfpp a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -206804.786929819990000;
    ce.xyzt.y = 120649.762565793000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=mbtfpp a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -206804.786929819990000;
    ce.xyzt.y = -120649.762565793000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=mbtfpp a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000033753864882;
    ce.xyzt.y = 0.000014465934612;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=mbtfpp a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000033753864882;
    ce.xyzt.y = -0.000014465934612;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=mbtfpp a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000033753864882;
    ce.xyzt.y = 0.000014465934612;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=mbtfpp a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000033753864882;
    ce.xyzt.y = -0.000014465934612;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=mbtfpp a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=mbtfpq a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 209391.854738393010000;
    ce.xyzt.y = 119161.040199055000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=mbtfpq a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 209391.854738393010000;
    ce.xyzt.y = -119161.040199055000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=mbtfpq a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -209391.854738393010000;
    ce.xyzt.y = 119161.040199055000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=mbtfpq a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -209391.854738393010000;
    ce.xyzt.y = -119161.040199055000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=mbtfpq a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000033337638762;
    ce.xyzt.y = 0.000014646541283;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=mbtfpq a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000033337638762;
    ce.xyzt.y = -0.000014646541283;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=mbtfpq a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000033337638762;
    ce.xyzt.y = 0.000014646541283;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=mbtfpq a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000033337638762;
    ce.xyzt.y = -0.000014646541283;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=mbtfpq a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=mbtfps a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 204740.117478572010000;
    ce.xyzt.y = 121864.729719340000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=mbtfps a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 204740.117478572010000;
    ce.xyzt.y = -121864.729719340000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=mbtfps a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -204740.117478572010000;
    ce.xyzt.y = 121864.729719340000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=mbtfps a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -204740.117478572010000;
    ce.xyzt.y = -121864.729719340000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=mbtfps a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000034093523408;
    ce.xyzt.y = 0.000014321822776;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=mbtfps a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000034093523408;
    ce.xyzt.y = -0.000014321822776;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=mbtfps a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000034093523408;
    ce.xyzt.y = 0.000014321822776;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=mbtfps a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000034093523408;
    ce.xyzt.y = -0.000014321822776;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=mbtfps a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=merc ellps=GRS80 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222638.981586546990000;
    ce.xyzt.y = 110579.965218250000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=merc ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222638.981586546990000;
    ce.xyzt.y = -110579.965218249000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=merc ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222638.981586546990000;
    ce.xyzt.y = 110579.965218250000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=merc ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222638.981586546990000;
    ce.xyzt.y = -110579.965218249000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=merc ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031357126393;
    ce.xyzt.y = 0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=merc ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031357126393;
    ce.xyzt.y = -0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=merc ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031357126393;
    ce.xyzt.y = 0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=merc ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031357126393;
    ce.xyzt.y = -0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=merc ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=merc R=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223402.144255274000000;
    ce.xyzt.y = 111706.743574944000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=merc R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223402.144255274000000;
    ce.xyzt.y = -111706.743574944000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=merc R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223402.144255274000000;
    ce.xyzt.y = 111706.743574944000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=merc R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223402.144255274000000;
    ce.xyzt.y = -111706.743574944000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=merc R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=merc R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=merc R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=merc R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=merc R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=mil_os R=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -1908527.949594205000000;
    ce.xyzt.y = -1726237.473061448000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=mil_os R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -1916673.022918485100000;
    ce.xyzt.y = -1943133.888125523000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=mil_os R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -2344429.412089623000000;
    ce.xyzt.y = -1706258.051218912000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=mil_os R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -2354637.835532999100000;
    ce.xyzt.y = -1926468.605135417100000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=mil_os R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.349101392179034;
    ce.xyzt.y = 0.314176166202431;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=mil_os R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.349101391795061;
    ce.xyzt.y = 0.314142364149008;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=mil_os R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.349030308618698;
    ce.xyzt.y = 0.314176166202431;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=mil_os R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.349030309002671;
    ce.xyzt.y = 0.314142364149008;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=mil_os R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=mill a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223402.144255274000000;
    ce.xyzt.y = 111704.701754394000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=mill a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223402.144255274000000;
    ce.xyzt.y = -111704.701754396000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=mill a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223402.144255274000000;
    ce.xyzt.y = 111704.701754394000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=mill a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223402.144255274000000;
    ce.xyzt.y = -111704.701754396000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=mill a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=mill a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=mill a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=mill a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=mill a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=misrsom ellps=GRS80 +lat_1=0.5 +lat_2=2 +path=1");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 18556630.368369825000000;
    ce.xyzt.y = 9533394.675311271100000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=misrsom ellps=GRS80 +lat_1=0.5 +lat_2=2 +path=1", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 19041866.006729737000000;
    ce.xyzt.y = 9707182.175323523600000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=misrsom ellps=GRS80 +lat_1=0.5 +lat_2=2 +path=1", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 18816810.130184799000000;
    ce.xyzt.y = 8647669.649802954900000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=misrsom ellps=GRS80 +lat_1=0.5 +lat_2=2 +path=1", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 19252610.784536727000000;
    ce.xyzt.y = 8778164.085801404000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=misrsom ellps=GRS80 +lat_1=0.5 +lat_2=2 +path=1", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 2.229823995305426;
    ce.xyzt.y = 0.000030284080516;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=misrsom ellps=GRS80 +lat_1=0.5 +lat_2=2 +path=1", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 2.229855262582269;
    ce.xyzt.y = 0.000032671970185;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=misrsom ellps=GRS80 +lat_1=0.5 +lat_2=2 +path=1", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 2.229828738865987;
    ce.xyzt.y = -0.000032671970185;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=misrsom ellps=GRS80 +lat_1=0.5 +lat_2=2 +path=1", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 2.229860006142830;
    ce.xyzt.y = -0.000030284080516;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=misrsom ellps=GRS80 +lat_1=0.5 +lat_2=2 +path=1", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=misrsom R=6400000 +lat_1=0.5 +lat_2=2 +path=1");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 18641249.279170383000000;
    ce.xyzt.y = 9563342.532334165600000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=misrsom R=6400000 +lat_1=0.5 +lat_2=2 +path=1", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 19130982.461581279000000;
    ce.xyzt.y = 9739539.593504635600000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=misrsom R=6400000 +lat_1=0.5 +lat_2=2 +path=1", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 18903483.515011538000000;
    ce.xyzt.y = 8675064.500617975400000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=misrsom R=6400000 +lat_1=0.5 +lat_2=2 +path=1", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 19343388.399800610000000;
    ce.xyzt.y = 8807471.904068488600000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=misrsom R=6400000 +lat_1=0.5 +lat_2=2 +path=1", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 2.229824015551245;
    ce.xyzt.y = 0.000029953881675;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=misrsom R=6400000 +lat_1=0.5 +lat_2=2 +path=1", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 2.229855172750173;
    ce.xyzt.y = 0.000032360516180;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=misrsom R=6400000 +lat_1=0.5 +lat_2=2 +path=1", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 2.229828828698083;
    ce.xyzt.y = -0.000032360516180;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=misrsom R=6400000 +lat_1=0.5 +lat_2=2 +path=1", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 2.229859985879557;
    ce.xyzt.y = -0.000029953881675;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=misrsom R=6400000 +lat_1=0.5 +lat_2=2 +path=1", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=moll a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 201113.698641813010000;
    ce.xyzt.y = 124066.283433859990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=moll a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 201113.698641813010000;
    ce.xyzt.y = -124066.283433859990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=moll a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -201113.698641813010000;
    ce.xyzt.y = 124066.283433859990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=moll a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -201113.698641813010000;
    ce.xyzt.y = -124066.283433859990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=moll a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000034710026060;
    ce.xyzt.y = 0.000014067441038;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=moll a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000034710026060;
    ce.xyzt.y = -0.000014067441038;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=moll a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000034710026060;
    ce.xyzt.y = 0.000014067441038;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=moll a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000034710026060;
    ce.xyzt.y = -0.000014067441038;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=moll a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=murd1 ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222600.813473554010000;
    ce.xyzt.y = 111404.244180546000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=murd1 ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222770.349287864000000;
    ce.xyzt.y = -111234.672856675000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=murd1 ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222600.813473554010000;
    ce.xyzt.y = 111404.244180546000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=murd1 ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222770.349287864000000;
    ce.xyzt.y = -111234.672856675000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=murd1 ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031350563955;
    ce.xyzt.y = 0.000015678554470;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=murd1 ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031350546502;
    ce.xyzt.y = -0.000015678571923;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=murd1 ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031350563955;
    ce.xyzt.y = 0.000015678554470;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=murd1 ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031350546502;
    ce.xyzt.y = -0.000015678571923;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=murd1 ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=murd1 a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223363.845309492000000;
    ce.xyzt.y = 111786.116032863010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=murd1 a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223533.962259251010000;
    ce.xyzt.y = -111615.963451823000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=murd1 a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223363.845309492000000;
    ce.xyzt.y = 111786.116032863010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=murd1 a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223533.962259251010000;
    ce.xyzt.y = -111615.963451823000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=murd1 a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031243470553;
    ce.xyzt.y = 0.000015624990315;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=murd1 a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031243453099;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=murd1 a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031243470553;
    ce.xyzt.y = 0.000015624990315;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=murd1 a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031243453099;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=murd1 a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=murd2 ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222588.099751230010000;
    ce.xyzt.y = 111426.140027412000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=murd2 ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222757.726267018010000;
    ce.xyzt.y = -111341.431317505000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=murd2 ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222588.099751230010000;
    ce.xyzt.y = 111426.140027412000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=murd2 ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222757.726267018010000;
    ce.xyzt.y = -111341.431317505000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=murd2 ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031352344191;
    ce.xyzt.y = 0.000015671084461;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=murd2 ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031352326738;
    ce.xyzt.y = -0.000015671101914;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=murd2 ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031352344191;
    ce.xyzt.y = 0.000015671084461;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=murd2 ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031352326738;
    ce.xyzt.y = -0.000015671101914;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=murd2 ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=murd2 a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223351.088007026990000;
    ce.xyzt.y = 111808.086934388000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=murd2 a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223521.295969169990000;
    ce.xyzt.y = -111723.087859673000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=murd2 a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223351.088007026990000;
    ce.xyzt.y = 111808.086934388000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=murd2 a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223521.295969169990000;
    ce.xyzt.y = -111723.087859673000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=murd2 a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031245250788;
    ce.xyzt.y = 0.000015617555213;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=murd2 a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031245233335;
    ce.xyzt.y = -0.000015617572666;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=murd2 a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031245250788;
    ce.xyzt.y = 0.000015617555213;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=murd2 a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031245233335;
    ce.xyzt.y = -0.000015617572666;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=murd2 a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=murd3 ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222600.814077577000000;
    ce.xyzt.y = 111404.246601372000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=murd3 ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222770.354733899000000;
    ce.xyzt.y = -111234.670432178000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=murd3 ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222600.814077577000000;
    ce.xyzt.y = 111404.246601372000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=murd3 ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222770.354733899000000;
    ce.xyzt.y = -111234.670432178000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=murd3 ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031350563955;
    ce.xyzt.y = 0.000015678554470;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=murd3 ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031350546502;
    ce.xyzt.y = -0.000015678571923;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=murd3 ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031350563955;
    ce.xyzt.y = 0.000015678554470;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=murd3 ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031350546502;
    ce.xyzt.y = -0.000015678571923;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=murd3 ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=murd3 a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223363.845915585000000;
    ce.xyzt.y = 111786.118461987000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=murd3 a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223533.967723953010000;
    ce.xyzt.y = -111615.961019015000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=murd3 a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223363.845915585000000;
    ce.xyzt.y = 111786.118461987000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=murd3 a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223533.967723953010000;
    ce.xyzt.y = -111615.961019015000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=murd3 a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031243470553;
    ce.xyzt.y = 0.000015624990315;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=murd3 a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031243453099;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=murd3 a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031243470553;
    ce.xyzt.y = 0.000015624990315;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=murd3 a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031243453099;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=murd3 a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=natearth a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 194507.265257889000000;
    ce.xyzt.y = 112508.737358294990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=natearth a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 194507.265257889000000;
    ce.xyzt.y = -112508.737358294990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=natearth a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -194507.265257889000000;
    ce.xyzt.y = 112508.737358294990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=natearth a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -194507.265257889000000;
    ce.xyzt.y = -112508.737358294990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=natearth a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000035890654032;
    ce.xyzt.y = 0.000015512905271;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=natearth a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000035890654032;
    ce.xyzt.y = -0.000015512905271;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=natearth a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000035890654032;
    ce.xyzt.y = 0.000015512905271;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=natearth a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000035890654032;
    ce.xyzt.y = -0.000015512905271;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=natearth a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=natearth2 a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 189255.172934731000000;
    ce.xyzt.y = 113022.495810907000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=natearth2 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 189255.172934731000000;
    ce.xyzt.y = -113022.495810907000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=natearth2 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -189255.172934731000000;
    ce.xyzt.y = 113022.495810907000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=natearth2 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -189255.172934731000000;
    ce.xyzt.y = -113022.495810907000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=natearth2 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000036886643623;
    ce.xyzt.y = 0.000015442324156;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=natearth2 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000036886643623;
    ce.xyzt.y = -0.000015442324156;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=natearth2 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000036886643623;
    ce.xyzt.y = 0.000015442324156;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=natearth2 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000036886643623;
    ce.xyzt.y = -0.000015442324156;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=natearth2 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=nell a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223385.132504696000000;
    ce.xyzt.y = 111698.236447187000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=nell a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223385.132504696000000;
    ce.xyzt.y = -111698.236447187000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=nell a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223385.132504696000000;
    ce.xyzt.y = 111698.236447187000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=nell a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223385.132504696000000;
    ce.xyzt.y = -111698.236447187000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=nell a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=nell a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=nell a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=nell a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=nell a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=nell_h a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223385.131640953010000;
    ce.xyzt.y = 111698.236533562000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=nell_h a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223385.131640953010000;
    ce.xyzt.y = -111698.236533562000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=nell_h a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223385.131640953010000;
    ce.xyzt.y = 111698.236533562000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=nell_h a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223385.131640953010000;
    ce.xyzt.y = -111698.236533562000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=nell_h a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=nell_h a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=nell_h a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=nell_h a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=nell_h a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=nicol a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223374.561814140000000;
    ce.xyzt.y = 111732.553988545000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=nicol a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223374.561814140000000;
    ce.xyzt.y = -111732.553988545000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=nicol a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223374.561814140000000;
    ce.xyzt.y = 111732.553988545000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=nicol a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223374.561814140000000;
    ce.xyzt.y = -111732.553988545000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=nicol a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=nsper a=6400000 +h=1000000");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222239.816114099990000;
    ce.xyzt.y = 111153.763991925010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=nsper a=6400000 +h=1000000", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222239.816114099990000;
    ce.xyzt.y = -111153.763991925010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=nsper a=6400000 +h=1000000", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222239.816114099990000;
    ce.xyzt.y = 111153.763991925010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=nsper a=6400000 +h=1000000", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222239.816114099990000;
    ce.xyzt.y = -111153.763991925010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=nsper a=6400000 +h=1000000", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=nsper a=6400000 +h=1000000", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=nsper a=6400000 +h=1000000", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=nsper a=6400000 +h=1000000", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=nsper a=6400000 +h=1000000", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=nzmg ellps=GRS80 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000010000000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 3352675144.747424600000000;
    ce.xyzt.y = -7043205391.100244500000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=nzmg ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 3691989502.779305500000000;
    ce.xyzt.y = -6729069415.332104700000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=nzmg ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 4099000768.453239000000000;
    ce.xyzt.y = -7863208779.667248700000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=nzmg ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 4466166927.369976000000000;
    ce.xyzt.y = -7502531736.628606800000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=nzmg ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200000.000000000000000;
    ci.xyzt.y = 100000.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 3.062740193401719;
    ce.xyzt.y = -1.211654554191880;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=nzmg ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200000.000000000000000;
    ci.xyzt.y = -100000.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 3.067535182637135;
    ce.xyzt.y = -1.213589756155136;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=nzmg ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200000.000000000000000;
    ci.xyzt.y = 100000.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 2.349302520655395;
    ce.xyzt.y = -1.072679283418543;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=nzmg ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200000.000000000000000;
    ci.xyzt.y = -100000.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 2.344565087648867;
    ce.xyzt.y = -1.075499001840615;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=nzmg ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=ob_tran +R=6400000 +o_proj=latlon +o_lon_p=20 +o_lat_p=20 +lon_0=180");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -2.685687214000000;
    ce.xyzt.y = 1.237430235000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=ob_tran +R=6400000 +o_proj=latlon +o_lon_p=20 +o_lat_p=20 +lon_0=180", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -2.695406975000000;
    ce.xyzt.y = 1.202683395000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=ob_tran +R=6400000 +o_proj=latlon +o_lon_p=20 +o_lat_p=20 +lon_0=180", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -2.899366393000000;
    ce.xyzt.y = 1.237430235000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=ob_tran +R=6400000 +o_proj=latlon +o_lon_p=20 +o_lat_p=20 +lon_0=180", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -2.889646631000000;
    ce.xyzt.y = 1.202683395000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=ob_tran +R=6400000 +o_proj=latlon +o_lon_p=20 +o_lat_p=20 +lon_0=180", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 2.121480427947509;
    ce.xyzt.y = -0.044263297899995;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=ob_tran +R=6400000 +o_proj=latlon +o_lon_p=20 +o_lat_p=20 +lon_0=180", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 1.104115955444076;
    ce.xyzt.y = 0.306921726668973;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=ob_tran +R=6400000 +o_proj=latlon +o_lon_p=20 +o_lat_p=20 +lon_0=180", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -2.462672371736954;
    ce.xyzt.y = 0.455386287205369;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=ob_tran +R=6400000 +o_proj=latlon +o_lon_p=20 +o_lat_p=20 +lon_0=180", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -1.149515481920648;
    ce.xyzt.y = 0.904609301391311;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=ob_tran +R=6400000 +o_proj=latlon +o_lon_p=20 +o_lat_p=20 +lon_0=180", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=ocea a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 19994423.837934088000000;
    ce.xyzt.y = 223322.760576728000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=ocea a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 20217962.128015257000000;
    ce.xyzt.y = 223322.760576728990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=ocea a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 19994423.837934088000000;
    ce.xyzt.y = -223322.760576727010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=ocea a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 20217962.128015265000000;
    ce.xyzt.y = -223322.760576725010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=ocea a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 3.141577028582025;
    ce.xyzt.y = 0.000031249998084;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=ocea a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -3.141577028582025;
    ce.xyzt.y = 0.000031249998084;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=ocea a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 3.141577028582025;
    ce.xyzt.y = -0.000031249998084;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=ocea a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -3.141577028582025;
    ce.xyzt.y = -0.000031249998084;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=ocea a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=oea a=6400000 +lat_1=0.5 +lat_2=2 +n=1 m=2 +theta=3");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 228926.872097863990000;
    ce.xyzt.y = 99870.488430076002000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=oea a=6400000 +lat_1=0.5 +lat_2=2 +n=1 m=2 +theta=3", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 217242.584036940010000;
    ce.xyzt.y = -123247.885607474990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=oea a=6400000 +lat_1=0.5 +lat_2=2 +n=1 m=2 +theta=3", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -217242.584036940010000;
    ce.xyzt.y = 123247.885607474990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=oea a=6400000 +lat_1=0.5 +lat_2=2 +n=1 m=2 +theta=3", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -228926.872097863990000;
    ce.xyzt.y = -99870.488430076002000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=oea a=6400000 +lat_1=0.5 +lat_2=2 +n=1 m=2 +theta=3", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000030389428590;
    ce.xyzt.y = 0.000017239088261;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=oea a=6400000 +lat_1=0.5 +lat_2=2 +n=1 m=2 +theta=3", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000032024924272;
    ce.xyzt.y = -0.000013968079443;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=oea a=6400000 +lat_1=0.5 +lat_2=2 +n=1 m=2 +theta=3", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000032024924272;
    ce.xyzt.y = 0.000013968079443;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=oea a=6400000 +lat_1=0.5 +lat_2=2 +n=1 m=2 +theta=3", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000030389428590;
    ce.xyzt.y = -0.000017239088261;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=oea a=6400000 +lat_1=0.5 +lat_2=2 +n=1 m=2 +theta=3", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=omerc ellps=GRS80 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222650.796885260990000;
    ce.xyzt.y = 110642.229314984000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=omerc ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222650.796885260990000;
    ce.xyzt.y = -110642.229314984000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=omerc ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222650.796885262010000;
    ce.xyzt.y = 110642.229314984000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=omerc ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222650.796885262010000;
    ce.xyzt.y = -110642.229314984000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=omerc ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031357126393;
    ce.xyzt.y = 0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=omerc ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031357126393;
    ce.xyzt.y = -0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=omerc ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031357126393;
    ce.xyzt.y = 0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=omerc ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031357126393;
    ce.xyzt.y = -0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=omerc ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=ortel a=6400000 +lat_1=0 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223374.577355252990000;
    ce.xyzt.y = 111701.072127637000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=ortel a=6400000 +lat_1=0 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223374.577355252990000;
    ce.xyzt.y = -111701.072127637000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=ortel a=6400000 +lat_1=0 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223374.577355252990000;
    ce.xyzt.y = 111701.072127637000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=ortel a=6400000 +lat_1=0 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223374.577355252990000;
    ce.xyzt.y = -111701.072127637000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=ortel a=6400000 +lat_1=0 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=ortho a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223322.760576727010000;
    ce.xyzt.y = 111695.401198614000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=ortho a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223322.760576727010000;
    ce.xyzt.y = -111695.401198614000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=ortho a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223322.760576727010000;
    ce.xyzt.y = 111695.401198614000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=ortho a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223322.760576727010000;
    ce.xyzt.y = -111695.401198614000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=ortho a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=ortho a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=ortho a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=ortho a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=ortho a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=pconic ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222588.098841617000000;
    ce.xyzt.y = 111416.604770067000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=pconic ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222757.718091090010000;
    ce.xyzt.y = -111331.881531080000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=pconic ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222588.098841617000000;
    ce.xyzt.y = 111416.604770067000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=pconic ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222757.718091090010000;
    ce.xyzt.y = -111331.881531080000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=pconic ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031352361645;
    ce.xyzt.y = 0.000015672428364;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=pconic ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031352326738;
    ce.xyzt.y = -0.000015672445818;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=pconic ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031352361645;
    ce.xyzt.y = 0.000015672428364;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=pconic ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031352326738;
    ce.xyzt.y = -0.000015672445818;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=pconic ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=pconic a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223351.087094295010000;
    ce.xyzt.y = 111798.518992055000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=pconic a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223521.287765217010000;
    ce.xyzt.y = -111713.505338457010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=pconic a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223351.087094295010000;
    ce.xyzt.y = 111798.518992055000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=pconic a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223521.287765217010000;
    ce.xyzt.y = -111713.505338457010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=pconic a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031245250788;
    ce.xyzt.y = 0.000015618899116;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=pconic a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031245233335;
    ce.xyzt.y = -0.000015618899116;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=pconic a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031245250788;
    ce.xyzt.y = 0.000015618899116;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=pconic a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031245233335;
    ce.xyzt.y = -0.000015618899116;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=pconic a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=patterson a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223402.144255274000000;
    ce.xyzt.y = 113354.250397779990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=patterson a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223402.144255274000000;
    ce.xyzt.y = -113354.250397779990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=patterson a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223402.144255274000000;
    ce.xyzt.y = 113354.250397779990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=patterson a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223402.144255274000000;
    ce.xyzt.y = -113354.250397779990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=patterson a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = 0.000015397120128;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=patterson a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = -0.000015397120128;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=patterson a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = 0.000015397120128;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=patterson a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = -0.000015397120128;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=patterson a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=poly ellps=GRS80 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222605.285770237010000;
    ce.xyzt.y = 110642.194561440000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=poly ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222605.285770237010000;
    ce.xyzt.y = -110642.194561440000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=poly ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222605.285770237010000;
    ce.xyzt.y = 110642.194561440000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=poly ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222605.285770237010000;
    ce.xyzt.y = -110642.194561440000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=poly ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031357126393;
    ce.xyzt.y = 0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=poly ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031357126393;
    ce.xyzt.y = -0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=poly ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031357126393;
    ce.xyzt.y = 0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=poly ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031357126393;
    ce.xyzt.y = -0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=poly ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=poly R=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223368.105210218990000;
    ce.xyzt.y = 111769.110491225000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=poly R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223368.105210218990000;
    ce.xyzt.y = -111769.110491225000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=poly R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223368.105210218990000;
    ce.xyzt.y = 111769.110491225000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=poly R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223368.105210218990000;
    ce.xyzt.y = -111769.110491225000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=poly R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=poly R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=poly R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=poly R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=poly R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=putp1 a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 211642.762754160010000;
    ce.xyzt.y = 105831.180787330000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=putp1 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 211642.762754160010000;
    ce.xyzt.y = -105831.180787330000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=putp1 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -211642.762754160010000;
    ce.xyzt.y = 105831.180787330000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=putp1 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -211642.762754160010000;
    ce.xyzt.y = -105831.180787330000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=putp1 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000032983267111;
    ce.xyzt.y = 0.000016491633555;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=putp1 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000032983267111;
    ce.xyzt.y = -0.000016491633555;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=putp1 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000032983267111;
    ce.xyzt.y = 0.000016491633555;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=putp1 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000032983267111;
    ce.xyzt.y = -0.000016491633555;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=putp1 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=putp2 a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 211638.039634338990000;
    ce.xyzt.y = 117895.033043380000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=putp2 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 211638.039634338990000;
    ce.xyzt.y = -117895.033043380000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=putp2 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -211638.039634338990000;
    ce.xyzt.y = 117895.033043380000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=putp2 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -211638.039634338990000;
    ce.xyzt.y = -117895.033043380000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=putp2 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000032983267111;
    ce.xyzt.y = 0.000014803917622;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=putp2 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000032983267111;
    ce.xyzt.y = -0.000014803917622;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=putp2 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000032983267111;
    ce.xyzt.y = 0.000014803917622;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=putp2 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000032983267111;
    ce.xyzt.y = -0.000014803917622;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=putp2 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=putp3 a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 178227.115507793990000;
    ce.xyzt.y = 89124.560786087997000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=putp3 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 178227.115507793990000;
    ce.xyzt.y = -89124.560786087997000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=putp3 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -178227.115507793990000;
    ce.xyzt.y = 89124.560786087997000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=putp3 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -178227.115507793990000;
    ce.xyzt.y = -89124.560786087997000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=putp3 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000039166061079;
    ce.xyzt.y = 0.000019583030540;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=putp3 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000039166061079;
    ce.xyzt.y = -0.000019583030540;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=putp3 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000039166061079;
    ce.xyzt.y = 0.000019583030540;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=putp3 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000039166061079;
    ce.xyzt.y = -0.000019583030540;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=putp3 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=putp3p a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 178238.118539985010000;
    ce.xyzt.y = 89124.560786087997000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=putp3p a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 178238.118539985010000;
    ce.xyzt.y = -89124.560786087997000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=putp3p a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -178238.118539985010000;
    ce.xyzt.y = 89124.560786087997000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=putp3p a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -178238.118539985010000;
    ce.xyzt.y = -89124.560786087997000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=putp3p a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000039166061079;
    ce.xyzt.y = 0.000019583030540;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=putp3p a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000039166061079;
    ce.xyzt.y = -0.000019583030540;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=putp3p a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000039166061079;
    ce.xyzt.y = 0.000019583030540;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=putp3p a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000039166061079;
    ce.xyzt.y = -0.000019583030540;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=putp3p a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=putp4p a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 195241.477349386000000;
    ce.xyzt.y = 127796.782307926000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=putp4p a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 195241.477349386000000;
    ce.xyzt.y = -127796.782307926000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=putp4p a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -195241.477349386000000;
    ce.xyzt.y = 127796.782307926000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=putp4p a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -195241.477349386000000;
    ce.xyzt.y = -127796.782307926000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=putp4p a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000035753558419;
    ce.xyzt.y = 0.000013656852331;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=putp4p a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000035753558419;
    ce.xyzt.y = -0.000013656852331;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=putp4p a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000035753558419;
    ce.xyzt.y = 0.000013656852331;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=putp4p a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000035753558419;
    ce.xyzt.y = -0.000013656852331;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=putp4p a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=putp5 a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 226367.213380562000000;
    ce.xyzt.y = 113204.568558475000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=putp5 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 226367.213380562000000;
    ce.xyzt.y = -113204.568558475000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=putp5 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -226367.213380562000000;
    ce.xyzt.y = 113204.568558475000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=putp5 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -226367.213380562000000;
    ce.xyzt.y = -113204.568558475000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=putp5 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000030834958788;
    ce.xyzt.y = 0.000015417488121;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=putp5 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000030834958788;
    ce.xyzt.y = -0.000015417488121;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=putp5 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000030834958788;
    ce.xyzt.y = 0.000015417488121;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=putp5 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000030834958788;
    ce.xyzt.y = -0.000015417488121;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=putp5 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=putp5p a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 226388.175248755990000;
    ce.xyzt.y = 113204.568558475000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=putp5p a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 226388.175248755990000;
    ce.xyzt.y = -113204.568558475000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=putp5p a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -226388.175248755990000;
    ce.xyzt.y = 113204.568558475000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=putp5p a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -226388.175248755990000;
    ce.xyzt.y = -113204.568558475000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=putp5p a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000030834958788;
    ce.xyzt.y = 0.000015417488121;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=putp5p a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000030834958788;
    ce.xyzt.y = -0.000015417488121;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=putp5p a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000030834958788;
    ce.xyzt.y = 0.000015417488121;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=putp5p a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000030834958788;
    ce.xyzt.y = -0.000015417488121;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=putp5p a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=putp6 a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 226369.395133403010000;
    ce.xyzt.y = 110218.523796521000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=putp6 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 226369.395133403010000;
    ce.xyzt.y = -110218.523796521000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=putp6 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -226369.395133403010000;
    ce.xyzt.y = 110218.523796521000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=putp6 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -226369.395133403010000;
    ce.xyzt.y = -110218.523796521000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=putp6 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000030834958788;
    ce.xyzt.y = 0.000015835302490;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=putp6 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000030834958788;
    ce.xyzt.y = -0.000015835302490;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=putp6 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000030834958788;
    ce.xyzt.y = 0.000015835302490;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=putp6 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000030834958788;
    ce.xyzt.y = -0.000015835302490;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=putp6 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=putp6p a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 198034.195132194990000;
    ce.xyzt.y = 125989.475461323000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=putp6p a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 198034.195132194990000;
    ce.xyzt.y = -125989.475461323000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=putp6p a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -198034.195132194990000;
    ce.xyzt.y = 125989.475461323000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=putp6p a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -198034.195132194990000;
    ce.xyzt.y = -125989.475461323000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=putp6p a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000035247814362;
    ce.xyzt.y = 0.000013852957526;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=putp6p a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000035247814362;
    ce.xyzt.y = -0.000013852957526;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=putp6p a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000035247814362;
    ce.xyzt.y = 0.000013852957526;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=putp6p a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000035247814362;
    ce.xyzt.y = -0.000013852957526;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=putp6p a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=qua_aut ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222613.549033096990000;
    ce.xyzt.y = 111318.077887984010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=qua_aut ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222613.549033096990000;
    ce.xyzt.y = -111318.077887984010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=qua_aut ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222613.549033096990000;
    ce.xyzt.y = 111318.077887984010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=qua_aut ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222613.549033096990000;
    ce.xyzt.y = -111318.077887984010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=qua_aut ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031357126393;
    ce.xyzt.y = 0.000015678554470;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=qua_aut ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031357126393;
    ce.xyzt.y = -0.000015678554470;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=qua_aut ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031357126393;
    ce.xyzt.y = 0.000015678554470;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=qua_aut ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031357126393;
    ce.xyzt.y = -0.000015678554470;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=qua_aut ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=qua_aut a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223376.624524029000000;
    ce.xyzt.y = 111699.654379186000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=qua_aut a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223376.624524029000000;
    ce.xyzt.y = -111699.654379186000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=qua_aut a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223376.624524029000000;
    ce.xyzt.y = 111699.654379186000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=qua_aut a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223376.624524029000000;
    ce.xyzt.y = -111699.654379186000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=qua_aut a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=qua_aut a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=qua_aut a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=qua_aut a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=qua_aut a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=qsc ellps=GRS80 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 304638.450843852010000;
    ce.xyzt.y = 164123.870923793990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=qsc ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 304638.450843852010000;
    ce.xyzt.y = -164123.870923793990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=qsc ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -304638.450843852010000;
    ce.xyzt.y = 164123.870923793990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=qsc ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -304638.450843852010000;
    ce.xyzt.y = -164123.870923793990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=qsc ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000023061750992;
    ce.xyzt.y = 0.000010657905437;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=qsc ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000023061750992;
    ce.xyzt.y = -0.000010657905437;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=qsc ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000023061750992;
    ce.xyzt.y = 0.000010657905437;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=qsc ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000023061750992;
    ce.xyzt.y = -0.000010657905437;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=qsc ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=qsc R=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 305863.792402890980000;
    ce.xyzt.y = 165827.722754715010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=qsc R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 305863.792402890980000;
    ce.xyzt.y = -165827.722754715010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=qsc R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -305863.792402890980000;
    ce.xyzt.y = 165827.722754715010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=qsc R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -305863.792402890980000;
    ce.xyzt.y = -165827.722754715010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=qsc R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000022982966829;
    ce.xyzt.y = 0.000010550393155;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=qsc R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000022982966829;
    ce.xyzt.y = -0.000010550393155;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=qsc R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000022982966829;
    ce.xyzt.y = 0.000010550393155;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=qsc R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000022982966829;
    ce.xyzt.y = -0.000010550393155;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=qsc R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=robin a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 189588.423282508010000;
    ce.xyzt.y = 107318.530350703000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=robin a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 189588.423282508010000;
    ce.xyzt.y = -107318.530350703000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=robin a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -189588.423282508010000;
    ce.xyzt.y = 107318.530350703000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=robin a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -189588.423282508010000;
    ce.xyzt.y = -107318.530350703000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=robin a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000036821019243;
    ce.xyzt.y = 0.000016263082690;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=robin a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000036821019243;
    ce.xyzt.y = -0.000016263082690;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=robin a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000036821019243;
    ce.xyzt.y = 0.000016263082690;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=robin a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000036821019243;
    ce.xyzt.y = -0.000016263082690;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=robin a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=rouss ellps=GRS80 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222644.894131617010000;
    ce.xyzt.y = 110611.091868370000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=rouss ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222644.894131617010000;
    ce.xyzt.y = -110611.091868370000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=rouss ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222644.894131617010000;
    ce.xyzt.y = 110611.091868370000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=rouss ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222644.894131617010000;
    ce.xyzt.y = -110611.091868370000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=rouss ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031357126393;
    ce.xyzt.y = 0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=rouss ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031357126393;
    ce.xyzt.y = -0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=rouss ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031357126393;
    ce.xyzt.y = 0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=rouss ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031357126393;
    ce.xyzt.y = -0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=rouss ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=rpoly a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223368.098302014000000;
    ce.xyzt.y = 111769.110486991010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=rpoly a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223368.098302014000000;
    ce.xyzt.y = -111769.110486991010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=rpoly a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223368.098302014000000;
    ce.xyzt.y = 111769.110486991010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=rpoly a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223368.098302014000000;
    ce.xyzt.y = -111769.110486991010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=rpoly a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=sinu ellps=GRS80 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222605.299539466010000;
    ce.xyzt.y = 110574.388554153000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=sinu ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222605.299539466010000;
    ce.xyzt.y = -110574.388554153000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=sinu ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222605.299539466010000;
    ce.xyzt.y = 110574.388554153000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=sinu ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222605.299539466010000;
    ce.xyzt.y = -110574.388554153000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=sinu ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031357126393;
    ce.xyzt.y = 0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=sinu ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031357126393;
    ce.xyzt.y = -0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=sinu ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031357126393;
    ce.xyzt.y = 0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=sinu ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031357126393;
    ce.xyzt.y = -0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=sinu ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=sinu R=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223368.119026631990000;
    ce.xyzt.y = 111701.072127637000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=sinu R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223368.119026631990000;
    ce.xyzt.y = -111701.072127637000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=sinu R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223368.119026631990000;
    ce.xyzt.y = 111701.072127637000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=sinu R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223368.119026631990000;
    ce.xyzt.y = -111701.072127637000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=sinu R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=sinu R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=sinu R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=sinu R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=sinu R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=somerc ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222638.981586546990000;
    ce.xyzt.y = 110579.965218249000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=somerc ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222638.981586546990000;
    ce.xyzt.y = -110579.965218251000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=somerc ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222638.981586546990000;
    ce.xyzt.y = 110579.965218249000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=somerc ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222638.981586546990000;
    ce.xyzt.y = -110579.965218251000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=somerc ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031357126393;
    ce.xyzt.y = 0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=somerc ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031357126393;
    ce.xyzt.y = -0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=somerc ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031357126393;
    ce.xyzt.y = 0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=somerc ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031357126393;
    ce.xyzt.y = -0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=somerc ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=somerc R=6400000 +lat_1=0.5 +lat_2=2 +n=0.5");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223402.144255274000000;
    ce.xyzt.y = 111706.743574944000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=somerc R=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223402.144255274000000;
    ce.xyzt.y = -111706.743574944990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=somerc R=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223402.144255274000000;
    ce.xyzt.y = 111706.743574944000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=somerc R=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223402.144255274000000;
    ce.xyzt.y = -111706.743574944990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=somerc R=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=somerc R=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=somerc R=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=somerc R=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=somerc R=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=stere ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222644.854550116990000;
    ce.xyzt.y = 110610.883474174000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=stere ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222644.854550116990000;
    ce.xyzt.y = -110610.883474174000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=stere ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222644.854550116990000;
    ce.xyzt.y = 110610.883474174000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=stere ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222644.854550116990000;
    ce.xyzt.y = -110610.883474174000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=stere ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031357126393;
    ce.xyzt.y = 0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=stere ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031357126393;
    ce.xyzt.y = -0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=stere ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031357126393;
    ce.xyzt.y = 0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=stere ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031357126393;
    ce.xyzt.y = -0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=stere ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=stere R=6400000 +lat_1=0.5 +lat_2=2 +n=0.5");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223407.810259507010000;
    ce.xyzt.y = 111737.938996443000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=stere R=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223407.810259507010000;
    ce.xyzt.y = -111737.938996443000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=stere R=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223407.810259507010000;
    ce.xyzt.y = 111737.938996443000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=stere R=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223407.810259507010000;
    ce.xyzt.y = -111737.938996443000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=stere R=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=stere R=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=stere R=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=stere R=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=stere R=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=sterea ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222644.894109198010000;
    ce.xyzt.y = 110611.091871736990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=sterea ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222644.894109198010000;
    ce.xyzt.y = -110611.091871738000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=sterea ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222644.894109198010000;
    ce.xyzt.y = 110611.091871736990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=sterea ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222644.894109198010000;
    ce.xyzt.y = -110611.091871738000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=sterea ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031357126393;
    ce.xyzt.y = 0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=sterea ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031357126393;
    ce.xyzt.y = -0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=sterea ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031357126393;
    ce.xyzt.y = 0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=sterea ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031357126393;
    ce.xyzt.y = -0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=sterea ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=sterea R=6400000 +lat_1=0.5 +lat_2=2 +n=0.5");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223407.810259507010000;
    ce.xyzt.y = 111737.938996443000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=sterea R=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223407.810259507010000;
    ce.xyzt.y = -111737.938996443000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=sterea R=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223407.810259507010000;
    ce.xyzt.y = 111737.938996443000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=sterea R=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223407.810259507010000;
    ce.xyzt.y = -111737.938996443000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=sterea R=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=sterea R=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=sterea R=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=sterea R=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=sterea R=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=gstmerc R=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223413.466406322000000;
    ce.xyzt.y = 111769.145040585990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=gstmerc R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223413.466406322000000;
    ce.xyzt.y = -111769.145040587000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=gstmerc R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223413.466406322990000;
    ce.xyzt.y = 111769.145040585990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=gstmerc R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223413.466406322990000;
    ce.xyzt.y = -111769.145040587000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=gstmerc R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=gstmerc R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=gstmerc R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=gstmerc R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=gstmerc R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=tcc a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223458.844192458000000;
    ce.xyzt.y = 111769.145040585990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=tcc a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223458.844192458000000;
    ce.xyzt.y = -111769.145040585990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=tcc a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223458.844192458000000;
    ce.xyzt.y = 111769.145040585990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=tcc a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223458.844192458000000;
    ce.xyzt.y = -111769.145040585990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=tcc a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=tcea a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223322.760576727010000;
    ce.xyzt.y = 111769.145040585990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=tcea a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223322.760576727010000;
    ce.xyzt.y = -111769.145040585990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=tcea a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223322.760576727010000;
    ce.xyzt.y = 111769.145040585990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=tcea a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223322.760576727010000;
    ce.xyzt.y = -111769.145040585990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=tcea a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=tcea a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=tcea a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=tcea a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=tcea a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=times +ellps=sphere");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.000000000000000;
    ci.xyzt.y = 0.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000000000000000;
    ce.xyzt.y = 0.000000000000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=times +ellps=sphere", ci, ce, co);

    ci.xyzt.x = 1.396263401595464;
    ci.xyzt.y = 1.221730476396031;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 5785183.576067095600000;
    ce.xyzt.y = 7615452.066120422400000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=times +ellps=sphere", ci, ce, co);

    ci.xyzt.x = 0.436332312998582;
    ci.xyzt.y = -0.174532925199433;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 2065971.530107880900000;
    ce.xyzt.y = -951526.064849458980000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=times +ellps=sphere", ci, ce, co);

    ci.xyzt.x = -0.610865238198015;
    ci.xyzt.y = 0.349065850398866;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -2873054.045485095100000;
    ce.xyzt.y = 1917730.953000521100000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=times +ellps=sphere", ci, ce, co);

    ci.xyzt.x = -0.785398163397448;
    ci.xyzt.y = -0.523598775598299;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -3651383.203521486800000;
    ce.xyzt.y = -2914213.457815920900000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=times +ellps=sphere", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 0.000000000000000;
    ci.xyzt.y = 0.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000000000000000;
    ce.xyzt.y = 0.000000000000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=times +ellps=sphere", ci, ce, co);

    ci.xyzt.x = 5785183.576067095600000;
    ci.xyzt.y = 7615452.066120422400000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 1.396263401595464;
    ce.xyzt.y = 1.221730476396031;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=times +ellps=sphere", ci, ce, co);

    ci.xyzt.x = 2065971.530107880900000;
    ci.xyzt.y = -951526.064849458980000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.436332312998582;
    ce.xyzt.y = -0.174532925199433;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=times +ellps=sphere", ci, ce, co);

    ci.xyzt.x = -2873054.045485095100000;
    ci.xyzt.y = 1917730.953000521100000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.610865238198015;
    ce.xyzt.y = 0.349065850398866;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=times +ellps=sphere", ci, ce, co);

    ci.xyzt.x = -3651383.203521486800000;
    ci.xyzt.y = -2914213.457815920900000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.785398163397448;
    ce.xyzt.y = -0.523598775598299;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=times +ellps=sphere", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=tissot ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222641.078699631010000;
    ce.xyzt.y = 54347.828487281004000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=tissot ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222810.614513940990000;
    ce.xyzt.y = -168291.088549939010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=tissot ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222641.078699631010000;
    ce.xyzt.y = 54347.828487281004000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=tissot ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222810.614513940990000;
    ce.xyzt.y = -168291.088549939010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=tissot ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031351017741;
    ce.xyzt.y = 0.008961304992504;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=tissot ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031350982834;
    ce.xyzt.y = 0.008929947883564;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=tissot ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031351017741;
    ce.xyzt.y = 0.008961304992504;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=tissot ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031350982834;
    ce.xyzt.y = 0.008929947883564;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=tissot ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=tissot a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223404.248556848990000;
    ce.xyzt.y = 54534.122161157997000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=tissot a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223574.365506608010000;
    ce.xyzt.y = -168867.957323528010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=tissot a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223404.248556848990000;
    ce.xyzt.y = 54534.122161157997000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=tissot a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223574.365506608010000;
    ce.xyzt.y = -168867.957323528010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=tissot a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031243906885;
    ce.xyzt.y = 0.008961251428349;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=tissot a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031243889432;
    ce.xyzt.y = 0.008930001430265;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=tissot a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031243906885;
    ce.xyzt.y = 0.008961251428349;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=tissot a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031243889432;
    ce.xyzt.y = 0.008930001430265;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=tissot a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=tmerc ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222650.796795778010000;
    ce.xyzt.y = 110642.229411926990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=tmerc ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222650.796795778010000;
    ce.xyzt.y = -110642.229411926990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=tmerc ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222650.796795778010000;
    ce.xyzt.y = 110642.229411926990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=tmerc ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222650.796795778010000;
    ce.xyzt.y = -110642.229411926990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=tmerc ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031357126393;
    ce.xyzt.y = 0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=tmerc ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031357126393;
    ce.xyzt.y = -0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=tmerc ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031357126393;
    ce.xyzt.y = 0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=tmerc ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031357126393;
    ce.xyzt.y = -0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=tmerc ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=tmerc R=6400000 +lat_1=0.5 +lat_2=2 +n=0.5");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223413.466406322000000;
    ce.xyzt.y = 111769.145040597000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=tmerc R=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223413.466406322000000;
    ce.xyzt.y = -111769.145040597000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=tmerc R=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223413.466406322000000;
    ce.xyzt.y = 111769.145040597000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=tmerc R=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223413.466406322000000;
    ce.xyzt.y = -111769.145040597000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=tmerc R=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=tmerc R=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=tmerc R=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=tmerc R=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=tmerc R=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=tpeqd ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -27750.758831678999000;
    ce.xyzt.y = -222599.403691777000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=tpeqd ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -250434.937024035990000;
    ce.xyzt.y = -222655.938193265990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=tpeqd ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -27750.758831678999000;
    ce.xyzt.y = 222599.403691777000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=tpeqd ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -250434.937024035990000;
    ce.xyzt.y = 222655.938193265990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=tpeqd ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000015682760714;
    ce.xyzt.y = 0.021847972758869;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=tpeqd ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000015682760714;
    ce.xyzt.y = 0.021847972758869;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=tpeqd ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000015682725807;
    ce.xyzt.y = 0.021785258523536;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=tpeqd ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000015682725807;
    ce.xyzt.y = 0.021785258523536;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=tpeqd ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=tpeqd a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -27845.882978484999000;
    ce.xyzt.y = -223362.430695260000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=tpeqd a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -251293.378764650990000;
    ce.xyzt.y = -223419.158985908000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=tpeqd a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -27845.882978484999000;
    ce.xyzt.y = 223362.430695260000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=tpeqd a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -251293.378764650990000;
    ce.xyzt.y = 223419.158985908000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=tpeqd a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000015629179106;
    ce.xyzt.y = 0.021847865648013;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=tpeqd a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000015629179106;
    ce.xyzt.y = 0.021847865648013;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=tpeqd a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000015629161652;
    ce.xyzt.y = 0.021785365651845;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=tpeqd a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000015629161652;
    ce.xyzt.y = 0.021785365651845;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=tpeqd a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=tpers a=6400000 +h=1000000 +azi=20");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 170820.288955531000000;
    ce.xyzt.y = 180460.865555805010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=tpers a=6400000 +h=1000000 +azi=20", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 246853.941538941990000;
    ce.xyzt.y = -28439.878035775000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=tpers a=6400000 +h=1000000 +azi=20", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -246853.941538941990000;
    ce.xyzt.y = 28439.878035775000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=tpers a=6400000 +h=1000000 +azi=20", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -170820.288955531000000;
    ce.xyzt.y = -180460.865555805010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=tpers a=6400000 +h=1000000 +azi=20", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000034709467554;
    ce.xyzt.y = 0.000003994569966;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=tpers a=6400000 +h=1000000 +azi=20", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000024021333014;
    ce.xyzt.y = -0.000025370821592;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=tpers a=6400000 +h=1000000 +azi=20", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000024021333014;
    ce.xyzt.y = 0.000025370821592;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=tpers a=6400000 +h=1000000 +azi=20", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000034709467554;
    ce.xyzt.y = -0.000003994569966;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=tpers a=6400000 +h=1000000 +azi=20", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=ups ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 2433455.563438467200000;
    ce.xyzt.y = -10412543.301512826000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=ups ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 2448749.118568199200000;
    ce.xyzt.y = -10850493.419804076000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=ups ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 1566544.436561533000000;
    ce.xyzt.y = -10412543.301512826000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=ups ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 1551250.881431801000000;
    ce.xyzt.y = -10850493.419804076000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=ups ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.785373161521007;
    ce.xyzt.y = 1.133036967795808;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=ups ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.785323161520582;
    ce.xyzt.y = 1.133015743963786;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=ups ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.785473161521857;
    ce.xyzt.y = 1.132994521283680;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=ups ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.785423161521432;
    ce.xyzt.y = 1.132973299772945;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=ups ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=urm5 a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223393.638433963990000;
    ce.xyzt.y = 111696.818785116990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=urm5 a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223393.638433963990000;
    ce.xyzt.y = -111696.818785116990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=urm5 a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223393.638433963990000;
    ce.xyzt.y = 111696.818785116990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=urm5 a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223393.638433963990000;
    ce.xyzt.y = -111696.818785116990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=urm5 a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=urmfps a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 196001.708134192010000;
    ce.xyzt.y = 127306.843329993000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=urmfps a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 196001.708134192010000;
    ce.xyzt.y = -127306.843329993000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=urmfps a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -196001.708134192010000;
    ce.xyzt.y = 127306.843329993000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=urmfps a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -196001.708134192010000;
    ce.xyzt.y = -127306.843329993000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=urmfps a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000035617300565;
    ce.xyzt.y = 0.000013709107489;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=urmfps a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000035617300565;
    ce.xyzt.y = -0.000013709107489;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=urmfps a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000035617300565;
    ce.xyzt.y = 0.000013709107489;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=urmfps a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000035617300565;
    ce.xyzt.y = -0.000013709107489;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=urmfps a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=utm ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5 +zone=30");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 1057002.405491298100000;
    ce.xyzt.y = 110955.141175949000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=utm ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5 +zone=30", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 1057002.405491298100000;
    ce.xyzt.y = -110955.141175949000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=utm ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5 +zone=30", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 611263.812278904950000;
    ce.xyzt.y = 110547.105696804010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=utm ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5 +zone=30", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 611263.812278904950000;
    ce.xyzt.y = -110547.105696804010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=utm ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5 +zone=30", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.130671964787398;
    ce.xyzt.y = 0.000015741822655;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=utm ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5 +zone=30", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.130671964787398;
    ce.xyzt.y = -0.000015741822655;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=utm ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5 +zone=30", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.130734510389019;
    ce.xyzt.y = 0.000015741735389;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=utm ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5 +zone=30", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.130734510389019;
    ce.xyzt.y = -0.000015741735389;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=utm ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5 +zone=30", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=vandg a=6400000 +lat_1=0.5 +lat_2=2 +no_defs");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223395.249543407000000;
    ce.xyzt.y = 111704.596633675000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=vandg a=6400000 +lat_1=0.5 +lat_2=2 +no_defs", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223395.249543407000000;
    ce.xyzt.y = -111704.596633675000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=vandg a=6400000 +lat_1=0.5 +lat_2=2 +no_defs", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223395.249543407000000;
    ce.xyzt.y = 111704.596633675000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=vandg a=6400000 +lat_1=0.5 +lat_2=2 +no_defs", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223395.249543407000000;
    ce.xyzt.y = -111704.596633675000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=vandg a=6400000 +lat_1=0.5 +lat_2=2 +no_defs", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031250015537;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=vandg a=6400000 +lat_1=0.5 +lat_2=2 +no_defs", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031250015537;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=vandg a=6400000 +lat_1=0.5 +lat_2=2 +no_defs", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031250015537;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=vandg a=6400000 +lat_1=0.5 +lat_2=2 +no_defs", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031250015537;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=vandg a=6400000 +lat_1=0.5 +lat_2=2 +no_defs", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=vandg2 a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223395.247850437010000;
    ce.xyzt.y = 111718.491037226010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=vandg2 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223395.247850437010000;
    ce.xyzt.y = -111718.491037226010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=vandg2 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223395.247850437010000;
    ce.xyzt.y = 111718.491037226010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=vandg2 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223395.247850437010000;
    ce.xyzt.y = -111718.491037226010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=vandg2 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=vandg3 a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223395.249552831000000;
    ce.xyzt.y = 111704.519904421000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=vandg3 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223395.249552831000000;
    ce.xyzt.y = -111704.519904421000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=vandg3 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223395.249552831000000;
    ce.xyzt.y = 111704.519904421000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=vandg3 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223395.249552831000000;
    ce.xyzt.y = -111704.519904421000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=vandg3 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=vandg4 R=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000010000000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223374.577294355000000;
    ce.xyzt.y = 111701.195484154000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=vandg4 R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223374.577294355000000;
    ce.xyzt.y = -111701.195484154000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=vandg4 R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223374.577294355000000;
    ce.xyzt.y = 111701.195484154000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=vandg4 R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223374.577294355000000;
    ce.xyzt.y = -111701.195484154000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=vandg4 R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=vitk1 ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222607.171211457990000;
    ce.xyzt.y = 111404.251442434990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=vitk1 ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222776.716709598000000;
    ce.xyzt.y = -111234.665587444990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=vitk1 ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222607.171211457990000;
    ce.xyzt.y = 111404.251442434990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=vitk1 ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222776.716709598000000;
    ce.xyzt.y = -111234.665587444990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=vitk1 ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031349673837;
    ce.xyzt.y = 0.000015678554470;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=vitk1 ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031349638931;
    ce.xyzt.y = -0.000015678571923;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=vitk1 ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031349673837;
    ce.xyzt.y = 0.000015678554470;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=vitk1 ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031349638931;
    ce.xyzt.y = -0.000015678571923;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=vitk1 ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=vitk1 a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223370.224840471000000;
    ce.xyzt.y = 111786.123319644000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=vitk1 a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223540.351507254990000;
    ce.xyzt.y = -111615.956157675000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=vitk1 a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223370.224840471000000;
    ce.xyzt.y = 111786.123319644000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=vitk1 a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223540.351507254990000;
    ce.xyzt.y = -111615.956157675000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=vitk1 a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031242580435;
    ce.xyzt.y = 0.000015624990315;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=vitk1 a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031242545528;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=vitk1 a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031242580435;
    ce.xyzt.y = 0.000015624990315;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=vitk1 a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031242545528;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=vitk1 a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=wag1 a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 195986.781561157990000;
    ce.xyzt.y = 127310.075060660000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=wag1 a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 195986.781561157990000;
    ce.xyzt.y = -127310.075060660000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=wag1 a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -195986.781561157990000;
    ce.xyzt.y = 127310.075060660000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=wag1 a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -195986.781561157990000;
    ce.xyzt.y = -127310.075060660000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=wag1 a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000035617300565;
    ce.xyzt.y = 0.000013709107489;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=wag1 a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000035617300565;
    ce.xyzt.y = -0.000013709107489;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=wag1 a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000035617300565;
    ce.xyzt.y = 0.000013709107489;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=wag1 a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000035617300565;
    ce.xyzt.y = -0.000013709107489;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=wag1 a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=wag2 a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 206589.888099962000000;
    ce.xyzt.y = 120778.040357547000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=wag2 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 206589.888099962000000;
    ce.xyzt.y = -120778.040357547000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=wag2 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -206589.888099962000000;
    ce.xyzt.y = 120778.040357547000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=wag2 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -206589.888099962000000;
    ce.xyzt.y = -120778.040357547000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=wag2 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000033789993198;
    ce.xyzt.y = 0.000014450593168;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=wag2 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000033789993198;
    ce.xyzt.y = -0.000014450593168;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=wag2 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000033789993198;
    ce.xyzt.y = 0.000014450593168;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=wag2 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000033789993198;
    ce.xyzt.y = -0.000014450593168;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=wag2 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=wag3 a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223387.021718166010000;
    ce.xyzt.y = 111701.072127637000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=wag3 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223387.021718166010000;
    ce.xyzt.y = -111701.072127637000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=wag3 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223387.021718166010000;
    ce.xyzt.y = 111701.072127637000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=wag3 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223387.021718166010000;
    ce.xyzt.y = -111701.072127637000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=wag3 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=wag3 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=wag3 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=wag3 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=wag3 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=wag4 a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 192801.218662384000000;
    ce.xyzt.y = 129416.216394803010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=wag4 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 192801.218662384000000;
    ce.xyzt.y = -129416.216394803010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=wag4 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -192801.218662384000000;
    ce.xyzt.y = 129416.216394803010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=wag4 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -192801.218662384000000;
    ce.xyzt.y = -129416.216394803010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=wag4 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000036206907692;
    ce.xyzt.y = 0.000013485862424;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=wag4 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000036206907692;
    ce.xyzt.y = -0.000013485862424;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=wag4 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000036206907692;
    ce.xyzt.y = 0.000013485862424;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=wag4 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000036206907692;
    ce.xyzt.y = -0.000013485862424;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=wag4 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=wag5 a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 203227.051925325010000;
    ce.xyzt.y = 138651.631442713000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=wag5 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 203227.051925325010000;
    ce.xyzt.y = -138651.631442713000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=wag5 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -203227.051925325010000;
    ce.xyzt.y = 138651.631442713000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=wag5 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -203227.051925325010000;
    ce.xyzt.y = -138651.631442713000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=wag5 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000034349336316;
    ce.xyzt.y = 0.000012587593818;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=wag5 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000034349336316;
    ce.xyzt.y = -0.000012587593818;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=wag5 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000034349336316;
    ce.xyzt.y = 0.000012587593818;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=wag5 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000034349336316;
    ce.xyzt.y = -0.000012587593818;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=wag5 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=wag6 a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 211652.562164409990000;
    ce.xyzt.y = 105831.180787330000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=wag6 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 211652.562164409990000;
    ce.xyzt.y = -105831.180787330000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=wag6 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -211652.562164409990000;
    ce.xyzt.y = 105831.180787330000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=wag6 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -211652.562164409990000;
    ce.xyzt.y = -105831.180787330000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=wag6 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000032983267111;
    ce.xyzt.y = 0.000016491633555;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=wag6 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000032983267111;
    ce.xyzt.y = -0.000016491633555;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=wag6 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000032983267111;
    ce.xyzt.y = 0.000016491633555;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=wag6 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000032983267111;
    ce.xyzt.y = -0.000016491633555;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=wag6 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=wag7 a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 198601.876957311990000;
    ce.xyzt.y = 125637.045714171010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=wag7 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 198601.876957311990000;
    ce.xyzt.y = -125637.045714171010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=wag7 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -198601.876957311990000;
    ce.xyzt.y = 125637.045714171010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=wag7 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -198601.876957311990000;
    ce.xyzt.y = -125637.045714171010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=wag7 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=weren a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223378.515757634010000;
    ce.xyzt.y = 146214.093042288000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=weren a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223378.515757634010000;
    ce.xyzt.y = -146214.093042288000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=weren a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223378.515757634010000;
    ce.xyzt.y = 146214.093042288000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=weren a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223378.515757634010000;
    ce.xyzt.y = -146214.093042288000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=weren a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = 0.000011936620914;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=weren a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = -0.000011936620914;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=weren a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = 0.000011936620914;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=weren a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = -0.000011936620914;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=weren a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=wink1 a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223385.131640953010000;
    ce.xyzt.y = 111701.072127637000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=wink1 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223385.131640953010000;
    ce.xyzt.y = -111701.072127637000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=wink1 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223385.131640953010000;
    ce.xyzt.y = 111701.072127637000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=wink1 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223385.131640953010000;
    ce.xyzt.y = -111701.072127637000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=wink1 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=wink1 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=wink1 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=wink1 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=wink1 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=wink2 a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223387.396433785990000;
    ce.xyzt.y = 124752.032797445000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=wink2 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223387.396433785990000;
    ce.xyzt.y = -124752.032797445000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=wink2 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223387.396433785990000;
    ce.xyzt.y = 124752.032797445000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=wink2 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223387.396433785990000;
    ce.xyzt.y = -124752.032797445000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=wink2 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=wintri a=6400000 +lat_1=0 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223390.801533485010000;
    ce.xyzt.y = 111703.907505745000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=wintri a=6400000 +lat_1=0 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223390.801533485010000;
    ce.xyzt.y = -111703.907505745000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=wintri a=6400000 +lat_1=0 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223390.801533485010000;
    ce.xyzt.y = 111703.907505745000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=wintri a=6400000 +lat_1=0 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223390.801533485010000;
    ce.xyzt.y = -111703.907505745000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=wintri a=6400000 +lat_1=0 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=wintri a=6400000 +lat_1=0 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=wintri a=6400000 +lat_1=0 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=wintri a=6400000 +lat_1=0 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=wintri a=6400000 +lat_1=0 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=deformation +xy_grids=alaska +t_epoch=2016.0 +t_obs=2000.0");
    tolerance = 0.000001000000;
    ci.xyzt.x = -3004295.588250307800000;
    ci.xyzt.y = -1093474.169060383200000;
    ci.xyzt.z = 5500477.133825145700000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -3004295.588876612000000;
    ce.xyzt.y = -1093474.168851342300000;
    ce.xyzt.z = 5500477.133825145700000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=deformation +xy_grids=alaska +t_epoch=2016.0 +t_obs=2000.0", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=deformation +z_grids=egm96_15.gtx +t_epoch=2016.0 +t_obs=2000.0");
    tolerance = 0.000001000000;
    ci.xyzt.x = -3004295.588250307800000;
    ci.xyzt.y = -1093474.169060383200000;
    ci.xyzt.z = 5500477.133825145700000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -3004295.588250307800000;
    ce.xyzt.y = -1093474.169060383200000;
    ce.xyzt.z = 5500234.008855661400000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=deformation +z_grids=egm96_15.gtx +t_epoch=2016.0 +t_obs=2000.0", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=deformation +xy_grids=alaska +z_grids=egm96_15.gtx +t_epoch=2016.0 +t_obs=2000.0");
    tolerance = 0.000001000000;
    ci.xyzt.x = -3004295.588250307800000;
    ci.xyzt.y = -1093474.169060383200000;
    ci.xyzt.z = 5500477.133825145700000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -3004295.588876612000000;
    ce.xyzt.y = -1093474.168851342300000;
    ce.xyzt.z = 5500234.008855661400000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=deformation +xy_grids=alaska +z_grids=egm96_15.gtx +t_epoch=2016.0 +t_obs=2000.0", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=deformation +xy_grids=alaska +t_epoch=2016.0");
    tolerance = 0.000001000000;
    ci.xyzt.x = -3004295.588250307800000;
    ci.xyzt.y = -1093474.169060383200000;
    ci.xyzt.z = 5500477.133825145700000;
    ci.xyzt.t = 2000.000000000000000;
    ce.xyzt.x = -3004295.588876612000000;
    ce.xyzt.y = -1093474.168851342300000;
    ce.xyzt.z = 5500477.133825145700000;
    ce.xyzt.t = 2000.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=deformation +xy_grids=alaska +t_epoch=2016.0", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=deformation +z_grids=egm96_15.gtx +t_epoch=2016.0");
    tolerance = 0.000001000000;
    ci.xyzt.x = -3004295.588250307800000;
    ci.xyzt.y = -1093474.169060383200000;
    ci.xyzt.z = 5500477.133825145700000;
    ci.xyzt.t = 2000.000000000000000;
    ce.xyzt.x = -3004295.588250307800000;
    ce.xyzt.y = -1093474.169060383200000;
    ce.xyzt.z = 5500234.008855661400000;
    ce.xyzt.t = 2000.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=deformation +z_grids=egm96_15.gtx +t_epoch=2016.0", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=deformation +xy_grids=alaska +z_grids=egm96_15.gtx +t_epoch=2016.0 +ellps=GRS80");
    tolerance = 0.000001000000;
    ci.xyzt.x = -3004295.588250307800000;
    ci.xyzt.y = -1093474.169060383200000;
    ci.xyzt.z = 5500477.133825145700000;
    ci.xyzt.t = 2000.000000000000000;
    ce.xyzt.x = -3004295.588876612000000;
    ce.xyzt.y = -1093474.168851342300000;
    ce.xyzt.z = 5500234.008855661400000;
    ce.xyzt.t = 2000.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=deformation +xy_grids=alaska +z_grids=egm96_15.gtx +t_epoch=2016.0 +ellps=GRS80", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "proj=merc R=6400000");
    tolerance = 0.000000010000;
    ci.xyzt.x = 0.017453292519943;
    ci.xyzt.y = 0.034906585039887;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 111701.072127637100000;
    ce.xyzt.y = 223447.526203260500000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "proj=merc R=6400000", ci, ce, co);

    ci.xyzt.x = 0.209439510239320;
    ci.xyzt.y = 0.959931088596881;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 1340412.865531645200000;
    ce.xyzt.y = 7387101.143096734800000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "proj=merc R=6400000", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "proj=merc a=6400000 rf=297");
    tolerance = 0.000000010000;
    ci.xyzt.x = 0.017453292519943;
    ci.xyzt.y = 0.034906585039887;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 111701.072127637100000;
    ce.xyzt.y = 221945.968183208810000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "proj=merc a=6400000 rf=297", ci, ce, co);

    ci.xyzt.x = 0.209439510239320;
    ci.xyzt.y = 0.959931088596881;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 1340412.865531645200000;
    ce.xyzt.y = 7351803.915170588500000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "proj=merc a=6400000 rf=297", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "proj=merc ellps=GRS80");
    tolerance = 0.000000010000;
    ci.xyzt.x = 0.017453292519943;
    ci.xyzt.y = 0.034906585039887;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 111319.490793273600000;
    ce.xyzt.y = 221194.077160423710000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "proj=merc ellps=GRS80", ci, ce, co);

    ci.xyzt.x = 0.209439510239320;
    ci.xyzt.y = 0.959931088596881;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 1335833.889519282800000;
    ce.xyzt.y = 7326837.714873876400000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "proj=merc ellps=GRS80", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "proj=merc ellps=GRS80000000000");
    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "proj=merc +a=-1");
    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "proj=merc no_defs");
    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "proj=merc");
    ci.xyzt.x = 0.000000000000000;
    ci.xyzt.y = 0.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000000000000000;
    ce.xyzt.y = 0.000000000000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "proj=merc", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "proj=merc +es=-1");
    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "");
    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "cobra");
    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "proj=merc ellps=GRS80 R_A");
    tolerance = 0.000000010000;
    ci.xyzt.x = 0.209439510239320;
    ci.xyzt.y = 0.959931088596881;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 1334340.623729770300000;
    ce.xyzt.y = 7353636.629655202900000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "proj=merc ellps=GRS80 R_A", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "proj=merc ellps=GRS80 R_V");
    tolerance = 0.000000010000;
    ci.xyzt.x = 0.209439510239320;
    ci.xyzt.y = 0.959931088596881;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 1334339.285267565200000;
    ce.xyzt.y = 7353629.253304272000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "proj=merc ellps=GRS80 R_V", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "proj=merc ellps=GRS80 R_a");
    tolerance = 0.000000010000;
    ci.xyzt.x = 0.209439510239320;
    ci.xyzt.y = 0.959931088596881;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 1333594.490452750400000;
    ce.xyzt.y = 7349524.641382550800000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "proj=merc ellps=GRS80 R_a", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "proj=merc ellps=GRS80 R_g");
    tolerance = 0.000000010000;
    ci.xyzt.x = 0.209439510239320;
    ci.xyzt.y = 0.959931088596881;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 1333592.610229132700000;
    ce.xyzt.y = 7349514.279349780600000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "proj=merc ellps=GRS80 R_g", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "proj=merc ellps=GRS80 R_h");
    tolerance = 0.000000010000;
    ci.xyzt.x = 0.209439510239320;
    ci.xyzt.y = 0.959931088596881;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 1333590.730008165800000;
    ce.xyzt.y = 7349503.917331623800000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "proj=merc ellps=GRS80 R_h", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "proj=merc ellps=GRS80 R_lat_a=60");
    tolerance = 0.000000010000;
    ci.xyzt.x = 0.209439510239320;
    ci.xyzt.y = 0.959931088596881;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 1338073.743626892100000;
    ce.xyzt.y = 7374210.092480332600000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "proj=merc ellps=GRS80 R_lat_a=60", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "proj=merc ellps=GRS80 R_lat_g=60");
    tolerance = 0.000000010000;
    ci.xyzt.x = 0.209439510239320;
    ci.xyzt.y = 0.959931088596881;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 1338073.269610159100000;
    ce.xyzt.y = 7374207.480143763100000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "proj=merc ellps=GRS80 R_lat_g=60", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "proj=healpix a=1 lon_0=0 ellps=WGS84");
    ci.xyzt.x = 0.000000000000000;
    ci.xyzt.y = 0.731953631859906;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000000000000000;
    ce.xyzt.y = 0.784520000000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "proj=healpix a=1 lon_0=0 ellps=WGS84", ci, ce, co);

    ci.xyzt.x = -1.570796326794897;
    ci.xyzt.y = 0.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -1.569040000000000;
    ce.xyzt.y = 0.000000000000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "proj=healpix a=1 lon_0=0 ellps=WGS84", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "proj=utm zone=32 ellps=GRS80 rf=0");
    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "proj=utm zone=32 ellps=GRS80 es=1");
    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "proj=utm zone=32 ellps=GRS80 b=0");
    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "proj=utm zone=32 ellps=GRS80 b=6000000");
    ci.xyzt.x = 0.209439510239320;
    ci.xyzt.y = 0.959931088596881;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 699293.087999999990000;
    ce.xyzt.y = 5674591.529500000200000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "proj=utm zone=32 ellps=GRS80 b=6000000", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "proj=utm zone=32 ellps=GRS80 rf=300");
    ci.xyzt.x = 0.209439510239320;
    ci.xyzt.y = 0.959931088596881;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 691873.121200000050000;
    ce.xyzt.y = 6099054.966099999800000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "proj=utm zone=32 ellps=GRS80 rf=300", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "proj=utm zone=32 ellps=GRS80 f=0.00333333333333");
    ci.xyzt.x = 0.209439510239320;
    ci.xyzt.y = 0.959931088596881;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 691873.121200000050000;
    ce.xyzt.y = 6099054.966099999800000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "proj=utm zone=32 ellps=GRS80 f=0.00333333333333", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "proj=utm zone=32 ellps=GRS80 b=6000000");
    ci.xyzt.x = 0.209439510239320;
    ci.xyzt.y = 0.959931088596881;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 699293.087999999990000;
    ce.xyzt.y = 5674591.529500000200000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "proj=utm zone=32 ellps=GRS80 b=6000000", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "proj=utm zone=32 a=6400000 b=6000000");
    ci.xyzt.x = 0.209439510239320;
    ci.xyzt.y = 0.959931088596881;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 700416.589999999970000;
    ce.xyzt.y = 5669475.888399999600000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "proj=utm zone=32 a=6400000 b=6000000", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "proj=helmert x=0.06155 rx=-0.0394924 y=-0.01087 ry=-0.0327221 z=-0.04019 rz=-0.0328979 s=-0.009994");
    tolerance = 0.000075000000;
    ci.xyzt.x = -4052051.764299999900000;
    ci.xyzt.y = 4212836.201700000100000;
    ci.xyzt.z = -2545106.024499999800000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -4052052.737900000100000;
    ce.xyzt.y = 4212835.989699999800000;
    ce.xyzt.z = -2545104.589800000200000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "proj=helmert x=0.06155 rx=-0.0394924 y=-0.01087 ry=-0.0327221 z=-0.04019 rz=-0.0328979 s=-0.009994", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "proj=pipeline ellps=GRS80 step proj=cart step proj=helmert x=0.06155 rx=-0.0394924 y=-0.01087 ry=-0.0327221 z=-0.04019 rz=-0.0328979 s=-0.009994 step proj=cart inv");
    ci.xyzt.x = 2.336743027633126;
    ci.xyzt.y = -0.413121596235468;
    ci.xyzt.z = 603.346599999999970;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 2.336743172669987;
    ce.xyzt.y = -0.413121356252696;
    ce.xyzt.z = 603.248900000000050;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "proj=pipeline ellps=GRS80 step proj=cart step proj=helmert x=0.06155 rx=-0.0394924 y=-0.01087 ry=-0.0327221 z=-0.04019 rz=-0.0328979 s=-0.009994 step proj=cart inv", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "proj=helmert ellps=GRS80 exact x=0 rx=0 dx=0 drx=0.00150379 y=0 ry=0 dy=0 dry=0.00118346 z=0 rz=0 dz=0 drz=0.00120716 s=0 ds=0 t_epoch=2020.0");
    tolerance = 0.000040000000;
    ci.xyzt.x = -4052052.658799999900000;
    ci.xyzt.y = 4212835.993800000300000;
    ci.xyzt.z = -2545104.694600000000000;
    ci.xyzt.t = 2018.000000000000000;
    ce.xyzt.x = -4052052.737300000200000;
    ce.xyzt.y = 4212835.983500000100000;
    ce.xyzt.z = -2545104.586699999900000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "proj=helmert ellps=GRS80 exact x=0 rx=0 dx=0 drx=0.00150379 y=0 ry=0 dy=0 dry=0.00118346 z=0 rz=0 dz=0 drz=0.00120716 s=0 ds=0 t_epoch=2020.0", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=ob_tran o_proj=moll +R=6378137.0 +o_lon_p=0 +o_lat_p=0 +lon_0=180");
    tolerance = 0.001000000000;
    direction = -1;
    ci.xyzt.x = 300000.000000000000000;
    ci.xyzt.y = 400000.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.746236741984416;
    ce.xyzt.y = 1.493847101725128;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=ob_tran o_proj=moll +R=6378137.0 +o_lon_p=0 +o_lat_p=0 +lon_0=180", ci, ce, co);

    direction = 1;
    ci.xyzt.x = 0.174532925199433;
    ci.xyzt.y = 0.349065850398866;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -1384841.187870000000000;
    ce.xyzt.y = 7581707.882399999500000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=ob_tran o_proj=moll +R=6378137.0 +o_lon_p=0 +o_lat_p=0 +lon_0=180", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "proj=molodensky a=6378160 rf=298.25 da=-23 df=-8.120449e-8 dx=-134 dy=-48 dz=149 abridged");
    tolerance = 2.000000000000;
    ci.xyzt.x = 2.530146220750864;
    ci.xyzt.y = -0.659734457253856;
    ci.xyzt.z = 50.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 2.530168910031139;
    ce.xyzt.y = -0.659707928249226;
    ce.xyzt.z = 46.378000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "proj=molodensky a=6378160 rf=298.25 da=-23 df=-8.120449e-8 dx=-134 dy=-48 dz=149 abridged", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "proj=molodensky a=6378160 rf=298.25 da=-23 df=-8.120449e-8 dx=-134 dy=-48 dz=149");
    tolerance = 2.000000000000;
    ci.xyzt.x = 2.530146220750864;
    ci.xyzt.y = -0.659734457253856;
    ci.xyzt.z = 50.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 2.530168910031139;
    ce.xyzt.y = -0.659707928249226;
    ce.xyzt.z = 46.378000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "proj=molodensky a=6378160 rf=298.25 da=-23 df=-8.120449e-8 dx=-134 dy=-48 dz=149", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "proj=pipeline zone=32 step proj=utm ellps=GRS80 step proj=utm ellps=GRS80 inv");
    tolerance = 0.000100000000;
    ci.xyzt.x = 0.209439510239320;
    ci.xyzt.y = 0.959931088596881;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.209439510239320;
    ce.xyzt.y = 0.959931088596881;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "proj=pipeline zone=32 step proj=utm ellps=GRS80 step proj=utm ellps=GRS80 inv", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 0.209439510239320;
    ci.xyzt.y = 0.959931088596881;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.209439510239320;
    ce.xyzt.y = 0.959931088596881;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "proj=pipeline zone=32 step proj=utm ellps=GRS80 step proj=utm ellps=GRS80 inv", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "proj=pipeline zone=32 ellps=GRS80 step proj=utm inv step proj=utm");
    ci.xyzt.x = 691875.632140000000000;
    ci.xyzt.y = 6098907.825009999800000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 691875.632140000000000;
    ce.xyzt.y = 6098907.825009999800000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "proj=pipeline zone=32 ellps=GRS80 step proj=utm inv step proj=utm", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 691875.632140000000000;
    ci.xyzt.y = 6098907.825009999800000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 691875.632140000000000;
    ce.xyzt.y = 6098907.825009999800000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "proj=pipeline zone=32 ellps=GRS80 step proj=utm inv step proj=utm", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "proj=pipeline step proj=utm zone=32 ellps=GRS80");
    ci.xyzt.x = 0.209439510239320;
    ci.xyzt.y = 0.959931088596881;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 691875.632140000000000;
    ce.xyzt.y = 6098907.825009999800000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "proj=pipeline step proj=utm zone=32 ellps=GRS80", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 691875.632140000000000;
    ci.xyzt.y = 6098907.825009999800000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.209439510239320;
    ce.xyzt.y = 0.959931088596881;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "proj=pipeline step proj=utm zone=32 ellps=GRS80", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "proj=pipeline step init=epsg:25832 inv step init=epsg:25833 step init=epsg:25833 inv step init=epsg:25832");
    ci.xyzt.x = 691875.632140000000000;
    ci.xyzt.y = 6098907.825009999800000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 691875.632140000000000;
    ce.xyzt.y = 6098907.825009999800000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "proj=pipeline step init=epsg:25832 inv step init=epsg:25833 step init=epsg:25833 inv step init=epsg:25832", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 12.000000000000000;
    ci.xyzt.y = 55.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 12.000000000000000;
    ce.xyzt.y = 55.000000000000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "proj=pipeline step init=epsg:25832 inv step init=epsg:25833 step init=epsg:25833 inv step init=epsg:25832", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "proj=vgridshift grids=egm96_15.gtx ellps=GRS80");
    tolerance = 0.150000000000;
    ci.xyzt.x = 0.218166156499291;
    ci.xyzt.y = 0.968657734856853;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.218166156499291;
    ce.xyzt.y = 0.968657734856853;
    ce.xyzt.z = -36.021305084228523;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "proj=vgridshift grids=egm96_15.gtx ellps=GRS80", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "proj=vgridshift");
    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "proj=vgridshift grids=nonexistinggrid.gtx");
    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "proj=hgridshift +grids=nzgd2kgrid0005.gsb ellps=GRS80");
    tolerance = 0.000000001000;
    ci.xyzt.x = 3.019419605950190;
    ci.xyzt.y = -0.785398163397448;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 3.019417724154021;
    ce.xyzt.y = -0.785426445270361;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "proj=hgridshift +grids=nzgd2kgrid0005.gsb ellps=GRS80", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 3.019417724154021;
    ci.xyzt.y = -0.785426445270361;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 3.019419605950190;
    ce.xyzt.y = -0.785398163397448;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "proj=hgridshift +grids=nzgd2kgrid0005.gsb ellps=GRS80", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "proj=hgridshift grids=@nonexistinggrid.gsb,anothernonexistinggrid.gsb");
    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "proj=hgridshift");
    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "proj=helmert ellps=GRS80 x=0.67678 y=0.65495 z=-0.52827 rx=-0.022742 ry=0.012667 rz=0.022704 s=-0.01070");
    tolerance = 0.000001000000;
    ci.xyzt.x = 3565285.000000000000000;
    ci.xyzt.y = 855949.000000000000000;
    ci.xyzt.z = 5201383.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 3565285.413423509800000;
    ce.xyzt.y = 855948.679867589960000;
    ce.xyzt.z = 5201382.729397909700000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "proj=helmert ellps=GRS80 x=0.67678 y=0.65495 z=-0.52827 rx=-0.022742 ry=0.012667 rz=0.022704 s=-0.01070", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "proj=helmert ellps=GRS80 exact x=-081.0703 rx=-0.48488 y=-089.3603 ry=-0.02436 z=-115.7526 rz=-0.41321 s=-0.540645");
    tolerance = 0.000001000000;
    ci.xyzt.x = 3494994.301200000100000;
    ci.xyzt.y = 1056601.972499999900000;
    ci.xyzt.z = 5212382.166600000100000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 3494909.840263680100000;
    ce.xyzt.y = 1056506.789386329900000;
    ce.xyzt.z = 5212265.666997609700000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "proj=helmert ellps=GRS80 exact x=-081.0703 rx=-0.48488 y=-089.3603 ry=-0.02436 z=-115.7526 rz=-0.41321 s=-0.540645", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "proj=helmert ellps=GRS80 transpose x=0.0127 dx=-0.0029 rx=-0.00039 drx=-0.00011 y=0.0065 dy=-0.0002 ry=0.00080 dry=-0.00019 z=-0.0209 dz=-0.0006 rz=-0.00114 drz=0.00007 s=0.00195 ds=0.00001 t_epoch=1988.0");
    tolerance = 0.000030000000;
    ci.xyzt.x = 3370658.378000000000000;
    ci.xyzt.y = 711877.314000000010000;
    ci.xyzt.z = 5349787.086000000100000;
    ci.xyzt.t = 2017.000000000000000;
    ce.xyzt.x = 3370658.188900000000000;
    ce.xyzt.y = 711877.423700000040000;
    ce.xyzt.z = 5349787.124300000300000;
    ce.xyzt.t = 2017.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "proj=helmert ellps=GRS80 transpose x=0.0127 dx=-0.0029 rx=-0.00039 drx=-0.00011 y=0.0065 dy=-0.0002 ry=0.00080 dry=-0.00019 z=-0.0209 dz=-0.0006 rz=-0.00114 drz=0.00007 s=0.00195 ds=0.00001 t_epoch=1988.0", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "proj=helmert ellps=GRS80 x=-9597.3572 y=.6112 s=0.304794780637 theta=-1.244048");
    tolerance = 0.001000000000;
    ci.xyzt.x = 2546506.956999999900000;
    ci.xyzt.y = 542256.609000000050000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 766563.675000000050000;
    ce.xyzt.y = 165282.277000000000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "proj=helmert ellps=GRS80 x=-9597.3572 y=.6112 s=0.304794780637 theta=-1.244048", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "proj=helmert ellps=GRS80 transpose x=0.01270 dx=-0.0029 rx=-0.00039 drx=-0.00011 y=0.00650 dy=-0.0002 ry=0.00080 dry=-0.00019 z=-0.0209 dz=-0.0006 rz=-0.00114 drz=0.00007 s=0.00195 ds=0.00001 t_epoch=1988.0");
    tolerance = 0.000100000000;
    ci.xyzt.x = 3370658.378000000000000;
    ci.xyzt.y = 711877.314000000010000;
    ci.xyzt.z = 5349787.086000000100000;
    ci.xyzt.t = 2017.000000000000000;
    ce.xyzt.x = 3370658.188900000000000;
    ce.xyzt.y = 711877.423700000040000;
    ce.xyzt.z = 5349787.124300000300000;
    ce.xyzt.t = 2017.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "proj=helmert ellps=GRS80 transpose x=0.01270 dx=-0.0029 rx=-0.00039 drx=-0.00011 y=0.00650 dy=-0.0002 ry=0.00080 dry=-0.00019 z=-0.0209 dz=-0.0006 rz=-0.00114 drz=0.00007 s=0.00195 ds=0.00001 t_epoch=1988.0", ci, ce, co);

    ci.xyzt.x = 3370658.378000000000000;
    ci.xyzt.y = 711877.314000000010000;
    ci.xyzt.z = 5349787.086000000100000;
    ci.xyzt.t = 2018.000000000000000;
    ce.xyzt.x = 3370658.180869999800000;
    ce.xyzt.y = 711877.427499999990000;
    ce.xyzt.z = 5349787.126480000100000;
    ce.xyzt.t = 2018.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "proj=helmert ellps=GRS80 transpose x=0.01270 dx=-0.0029 rx=-0.00039 drx=-0.00011 y=0.00650 dy=-0.0002 ry=0.00080 dry=-0.00019 z=-0.0209 dz=-0.0006 rz=-0.00114 drz=0.00007 s=0.00195 ds=0.00001 t_epoch=1988.0", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "proj=geoc ellps=GRS80");
    tolerance = 1e-9;
    ci.xyzt.x = 0.209439510239320;
    ci.xyzt.y = 0.959931088596881;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.209439510239320;
    ce.xyzt.y = 0.956771576793152;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "proj=geoc ellps=GRS80", ci, ce, co);

    ci.xyzt.x = 0.209439510239320;
    ci.xyzt.y = 1.570796326794897;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.209439510239320;
    ce.xyzt.y = 1.570796326794897;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "proj=geoc ellps=GRS80", ci, ce, co);

    ci.xyzt.x = 0.209439510239320;
    ci.xyzt.y = -1.570796326794897;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.209439510239320;
    ce.xyzt.y = -1.570796326794897;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "proj=geoc ellps=GRS80", ci, ce, co);

    ci.xyzt.x = 0.209439510239320;
    ci.xyzt.y = 1.570796326794722;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.209439510239320;
    ce.xyzt.y = 1.570796326794722;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "proj=geoc ellps=GRS80", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "proj=utm ellps=GRS80 zone=32 to_meter=0");
    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "proj=utm ellps=GRS80 zone=32 to_meter=10");
    ci.xyzt.x = 0.209439510239320;
    ci.xyzt.y = 0.959931088596881;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 69187.563200000004000;
    ce.xyzt.y = 609890.782499999970000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "proj=utm ellps=GRS80 zone=32 to_meter=10", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "proj=pipeline step init=epsg:25832 inv step init=epsg:25833 step init=epsg:25833 inv step init=epsg:25832");
    ci.xyzt.x = 691875.632140000000000;
    ci.xyzt.y = 6098907.825009999800000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 691875.632140000000000;
    ce.xyzt.y = 6098907.825009999800000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "proj=pipeline step init=epsg:25832 inv step init=epsg:25833 step init=epsg:25833 inv step init=epsg:25832", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 12.000000000000000;
    ci.xyzt.y = 55.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 12.000000000000000;
    ce.xyzt.y = 55.000000000000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "proj=pipeline step init=epsg:25832 inv step init=epsg:25833 step init=epsg:25833 inv step init=epsg:25832", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "proj=helmert ellps=GRS80 transpose x=0.0127 dx=-0.0029 rx=-0.00039 drx=-0.00011 y=0.0065 dy=-0.0002 ry=0.00080 dry=-0.00019 z=-0.0209 dz=-0.0006 rz=-0.00114 drz=0.00007 s=0.00195 ds=0.00001 t_epoch=1988.0");
    tolerance = 0.000030000000;
    ci.xyzt.x = 3370658.378000000000000;
    ci.xyzt.y = 711877.314000000010000;
    ci.xyzt.z = 5349787.086000000100000;
    ci.xyzt.t = 2017.000000000000000;
    ce.xyzt.x = 3370658.188900000000000;
    ce.xyzt.y = 711877.423700000040000;
    ce.xyzt.z = 5349787.124300000300000;
    ce.xyzt.t = 2017.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "proj=helmert ellps=GRS80 transpose x=0.0127 dx=-0.0029 rx=-0.00039 drx=-0.00011 y=0.0065 dy=-0.0002 ry=0.00080 dry=-0.00019 z=-0.0209 dz=-0.0006 rz=-0.00114 drz=0.00007 s=0.00195 ds=0.00001 t_epoch=1988.0", ci, ce, co);

    printf ("OK: %d,  KO: %d\nOL: %d\n", OK, KO, OK+KO);
    return 0;
}