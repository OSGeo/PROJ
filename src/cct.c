/***********************************************************************

                 The cct 4D Transformation program

************************************************************************

cct is a 4D equivalent to the "proj" projection program.

cct is an acronym meaning "Coordinate Conversion and Transformation".

The acronym refers to definitions given in the OGC 08-015r2/ISO-19111
standard "Geographical Information -- Spatial Referencing by Coordinates",
which defines two different classes of coordinate operations:

*Coordinate Conversions*, which are coordinate operations where input
and output datum are identical (e.g. conversion from geographical to
cartesian coordinates) and

*Coordinate Transformations*, which are coordinate operations where
input and output datums differ (e.g. change of reference frame).

cct, however, also refers to Carl Christian Tscherning (1942--2014),
professor of Geodesy at the University of Copenhagen, mentor and advisor
for a generation of Danish geodesists, colleague and collaborator for
two generations of global geodesists, Secretary General for the
International Association of Geodesy, IAG (1995--2007), fellow of the
American Geophysical Union (1991), recipient of the IAG Levallois Medal
(2007), the European Geosciences Union Vening Meinesz Medal (2008), and
of numerous other honours.

cct, or Christian, as he was known to most of us, was recognized for his
good mood, his sharp wit, his tireless work, and his great commitment to
the development of geodesy - both through his scientific contributions,
comprising more than 250 publications, and by his mentoring and teaching
of the next generations of geodesists.

As Christian was an avid Fortran programmer, and a keen Unix connoisseur,
he would have enjoyed to know that his initials would be used to name a
modest Unix style transformation filter, hinting at the tireless aspect
of his personality, which was certainly one of the reasons he accomplished
so much, and meant so much to so many people.

Hence, in honour of cct (the geodesist) this is cct (the program).

************************************************************************

Thomas Knudsen, thokn@sdfe.dk, 2016-05-25/2017-10-26

************************************************************************

* Copyright (c) 2016, 2017 Thomas Knudsen
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
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "proj.h"
#include "proj_internal.h"
#include "proj_strtod.h"
#include "projects.h"
#include "optargpm.h"


static void logger(void *data, int level, const char *msg);
static void print(PJ_LOG_LEVEL log_level, const char *fmt, ...);

/* Prototypes from functions in this file */
char *column (char *buf, int n);
PJ_COORD parse_input_line (char *buf, int *columns, double fixed_height, double fixed_time);


static const char usage[] = {
    "--------------------------------------------------------------------------------\n"
    "Usage: %s [-options]... [+operator_specs]... infile...\n"
    "--------------------------------------------------------------------------------\n"
    "Options:\n"
    "--------------------------------------------------------------------------------\n"
    "    -c x,y,z,t        Specify input columns for (up to) 4 input parameters.\n"
    "                      Defaults to 1,2,3,4\n"
    "    -I                Do the inverse transformation\n"
    "    -o /path/to/file  Specify output file name\n"
    "    -t value          Provide a fixed t value for all input data (e.g. -t 0)\n"
    "    -z value          Provide a fixed z value for all input data (e.g. -z 0)\n"
    "    -s n              Skip n first lines of a infile\n"
    "    -v                Verbose: Provide non-essential informational output.\n"
    "                      Repeat -v for more verbosity (e.g. -vv)\n"
    "--------------------------------------------------------------------------------\n"
    "Long Options:\n"
    "--------------------------------------------------------------------------------\n"
    "    --output          Alias for -o\n"
    "    --columns         Alias for -c\n"
    "    --height          Alias for -z\n"
    "    --time            Alias for -t\n"
    "    --verbose         Alias for -v\n"
    "    --inverse         Alias for -I\n"
    "    --skip-lines      Alias for -s\n"
    "    --help            Alias for -h\n"
    "    --version         Print version number\n"
    "--------------------------------------------------------------------------------\n"
    "Operator Specs:\n"
    "--------------------------------------------------------------------------------\n"
    "The operator specs describe the action to be performed by cct, e.g:\n"
    "\n"
    "        +proj=utm  +ellps=GRS80  +zone=32\n"
    "\n"
    "instructs cct to convert input data to Universal Transverse Mercator, zone 32\n"
    "coordinates, based on the GRS80 ellipsoid.\n"
    "\n"
    "Hence, the command\n"
    "\n"
    "        echo 12 55 | cct -z0 -t0 +proj=utm +zone=32 +ellps=GRS80\n"
    "\n"
    "Should give results comparable to the classic proj command\n"
    "\n"
    "        echo 12 55 | proj +proj=utm +zone=32 +ellps=GRS80\n"
    "--------------------------------------------------------------------------------\n"
    "Examples:\n"
    "--------------------------------------------------------------------------------\n"
    "1. convert geographical input to UTM zone 32 on the GRS80 ellipsoid:\n"
    "    cct +proj=utm +ellps=GRS80 +zone=32\n"
    "2. roundtrip accuracy check for the case above:\n"
    "    cct +proj=pipeline +proj=utm +ellps=GRS80 +zone=32 +step +step +inv\n"
    "3. as (1) but specify input columns for longitude, latitude, height and time:\n"
    "    cct -c 5,2,1,4  +proj=utm +ellps=GRS80 +zone=32\n"
    "4. as (1) but specify fixed height and time, hence needing only 2 cols in input:\n"
    "    cct -t 0 -z 0  +proj=utm  +ellps=GRS80  +zone=32\n"
    "--------------------------------------------------------------------------------\n"
};


static void logger(void *data, int level, const char *msg) {
    FILE *stream;
    int log_tell = proj_log_level(PJ_DEFAULT_CTX, PJ_LOG_TELL);

    stream  = (FILE *) data;

    /* if we use PJ_LOG_NONE we always want to print stuff to stream */
    if (level == PJ_LOG_NONE) {
        fprintf(stream, "%s", msg);
        return;
    }

    /* should always print to stderr if level == PJ_LOG_ERROR */
    if (level == PJ_LOG_ERROR) {
        fprintf(stderr, "%s", msg);
        return;
    }

    /* otherwise only print if log level set by user is high enough */
    if (level <= log_tell)
        fprintf(stream, "%s", msg);
}

FILE *fout;

static void print(PJ_LOG_LEVEL log_level, const char *fmt, ...) {

    va_list args;
    char *msg_buf;

    va_start( args, fmt );

    msg_buf = (char *) malloc(100000);
    if( msg_buf == NULL )
        return;

    vsprintf( msg_buf, fmt, args );

    logger((void *) fout, log_level, msg_buf);

    va_end( args );
    free( msg_buf );
}


int main(int argc, char **argv) {
    PJ *P;
    PJ_COORD point;
    PJ_PROJ_INFO info;
    OPTARGS *o;
    char *buf;
    int nfields = 4, direction = 1, skip_lines = 0, verbose;
    double fixed_z = HUGE_VAL, fixed_time = HUGE_VAL;
    int columns_xyzt[] = {1, 2, 3, 4};
    const char *longflags[]  = {"v=verbose", "h=help", "I=inverse", "version", 0};
    const char *longkeys[]   = {
        "o=output",
        "c=columns",
        "z=height",
        "t=time",
        "s=skip-lines",
        0};

    fout = stdout;

    o = opt_parse (argc, argv, "hvI", "cozts", longflags, longkeys);
    if (0==o)
        return 0;

    if (opt_given (o, "h") || argc==1) {
        printf (usage, o->progname);
        return 0;
    }

    direction = opt_given (o, "I")? -1: 1;

    verbose   = MIN(opt_given (o, "v"), 3); /* log level can't be larger than 3 */
    proj_log_level (PJ_DEFAULT_CTX, verbose);
    proj_log_func  (PJ_DEFAULT_CTX, (void *) fout, logger);

    if (opt_given (o, "version")) {
        print (PJ_LOG_NONE, "%s: %s\n", o->progname, pj_get_release ());
        return 0;
    }

    if (opt_given (o, "o"))
        fout = fopen (opt_arg (o, "output"), "wt");
    if (0==fout) {
        print (PJ_LOG_ERROR, "%s: Cannot open '%s' for output\n", o->progname, opt_arg (o, "output"));
        free (o);
        return 1;
    }

    print (PJ_LOG_TRACE, "%s: Running in very verbose mode\n", o->progname);

    if (opt_given (o, "z")) {
        fixed_z = proj_atof (opt_arg (o, "z"));
        nfields--;
    }

    if (opt_given (o, "t")) {
        fixed_time = proj_atof (opt_arg (o, "t"));
        nfields--;
    }

    if (opt_given (o, "s")) {
        skip_lines = atoi (opt_arg(o, "s"));
    }

    if (opt_given (o, "c")) {
        /* cppcheck-suppress invalidscanf */
        int ncols = sscanf (opt_arg (o, "c"), "%d,%d,%d,%d", columns_xyzt, columns_xyzt+1, columns_xyzt+2, columns_xyzt+3);
        if (ncols != nfields) {
            print (PJ_LOG_ERROR, "%s: Too few input columns given: '%s'\n", o->progname, opt_arg (o, "c"));
            free (o);
            if (stdout != fout)
                fclose (fout);
            return 1;
        }
    }

    /* Setup transformation */
    P = proj_create_argv (0, o->pargc, o->pargv);
    if ((0==P) || (0==o->pargc)) {
        print (PJ_LOG_ERROR, "%s: Bad transformation arguments - (%s)\n    '%s -h' for help\n",
                 o->progname, pj_strerrno (proj_errno(P)), o->progname);
        free (o);
        if (stdout != fout)
            fclose (fout);
        return 1;
    }

    info = proj_pj_info (P);
    print (PJ_LOG_TRACE, "Final: %s argc=%d pargc=%d\n", info.definition, argc, o->pargc);

    if (direction==-1) {
        /* fail if an inverse operation is not available */
        if (!info.has_inverse) {
            print (PJ_LOG_ERROR, "Inverse operation not available\n");
            if (stdout != fout)
                fclose (fout);
            return 1;
        }
        /* We have no API call for inverting an operation, so we brute force it. */
        P->inverted = !(P->inverted);
    }
    direction = 1;

    /* Allocate input buffer */
    buf = calloc (1, 10000);
    if (0==buf) {
        print (PJ_LOG_ERROR, "%s: Out of memory\n", o->progname);
        pj_free (P);
        free (o);
        if (stdout != fout)
            fclose (fout);
        return 1;
    }


    /* Loop over all records of all input files */
    while (opt_input_loop (o, optargs_file_format_text)) {
        int err;
        void *ret = fgets (buf, 10000, o->input);
        char *c = column (buf, 1);
        opt_eof_handler (o);
        if (0==ret) {
            print (PJ_LOG_ERROR, "Read error in record %d\n", (int) o->record_index);
            continue;
        }
        point = parse_input_line (buf, columns_xyzt, fixed_z, fixed_time);
        if (skip_lines > 0) {
            skip_lines--;
            continue;
        }

        /* if it's a comment or blank line, we reflect it */
        if (c && ((*c=='\0') || (*c=='#'))) {
            fprintf (fout, "%s", buf);
            continue;
        }

        if (HUGE_VAL==point.xyzt.x) {
            /* otherwise, it must be a syntax error */
            print (PJ_LOG_NONE, "# Record %d UNREADABLE: %s", (int) o->record_index, buf);
            print (PJ_LOG_ERROR, "%s: Could not parse file '%s' line %d\n", o->progname, opt_filename (o), opt_record (o));
            continue;
        }

        if (proj_angular_input (P, direction)) {
            point.lpzt.lam = proj_torad (point.lpzt.lam);
            point.lpzt.phi = proj_torad (point.lpzt.phi);
        }
        err = proj_errno_reset (P);
        point = proj_trans (P, direction, point);

        if (HUGE_VAL==point.xyzt.x) {
            /* transformation error */
            print (PJ_LOG_NONE, "# Record %d TRANSFORMATION ERROR: %s (%s)",
                   (int) o->record_index, buf, pj_strerrno (proj_errno(P)));
            proj_errno_restore (P, err);
            continue;
        }
        proj_errno_restore (P, err);

        /* Time to print the result */
        if (proj_angular_output (P, direction)) {
            point.lpzt.lam = proj_todeg (point.lpzt.lam);
            point.lpzt.phi = proj_todeg (point.lpzt.phi);
            print (PJ_LOG_NONE, "%14.10f  %14.10f  %12.4f  %12.4f\n", point.xyzt.x, point.xyzt.y, point.xyzt.z, point.xyzt.t);
        }
        else
            print (PJ_LOG_NONE, "%13.4f  %13.4f  %12.4f  %12.4f\n", point.xyzt.x, point.xyzt.y, point.xyzt.z, point.xyzt.t);
    }

    if (stdout != fout)
        fclose (fout);
    free (o);
    free (buf);
    return 0;
}





/* return a pointer to the n'th column of buf */
char *column (char *buf, int n) {
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

/* column to double */
static double cold (char *args, int col) {
    char *endp;
    char *target;
    double d;
    target = column (args, col);
    d = proj_strtod (target, &endp);
    if (endp==target)
        return HUGE_VAL;
    return d;
}

PJ_COORD parse_input_line (char *buf, int *columns, double fixed_height, double fixed_time) {
    PJ_COORD err = proj_coord (HUGE_VAL, HUGE_VAL, HUGE_VAL, HUGE_VAL);
    PJ_COORD result = err;
    int prev_errno = errno;
    errno = 0;

    result.xyzt.z = fixed_height;
    result.xyzt.t = fixed_time;
    result.xyzt.x = cold (buf, columns[0]);
    result.xyzt.y = cold (buf, columns[1]);
    if (result.xyzt.z==HUGE_VAL)
        result.xyzt.z = cold (buf, columns[2]);
    if (result.xyzt.t==HUGE_VAL)
        result.xyzt.t = cold (buf, columns[3]);

    if (0!=errno)
        return err;

    errno = prev_errno;
    return result;
}
