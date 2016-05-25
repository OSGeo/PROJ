/***********************************************************************

              The TRAN 3D Transformation program

                 Thomas Knudsen, 2016-05-20

************************************************************************

This is a first attempt at a 3D equivalent to the proj projection
program.

************************************************************************

Thomas Knudsen, thokn@sdfe.dk, 2016-05-25

************************************************************************

* Copyright (c) 2016, Thomas Knudsen
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

#define PJ_LIB__
#include <projects.h>
#include <PJ_pipeline.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>


struct buf { char *buf;  size_t size, n, comment_or_blank, format; } buf;


COORDINATE point_adjust (COORDINATE point, int E, int N) {
    COORDINATE error = {{HUGE_VAL, HUGE_VAL, HUGE_VAL}};
    double swap;

    if (HUGE_VAL==point.xyz.x)
        return point;

    if (0==E)
        return point;

    /* Traditional format: latitude/longitude */
    if ((E=='S') || (E=='N')) {
        if ((N!='E') && (N!='W'))
            return error;
        if (E=='S')
            point.xyz.x = -point.xyz.x;
        if (N=='W')
            point.xyz.y = -point.xyz.y;
        swap = point.xyz.x;
        point.xyz.x = point.xyz.y;
        point.xyz.y = swap;
        return point;
    }

    /* proj format: longitude/latitude */
    if ((E=='E') || (E=='W')) {
        if ((N!='N') && (N!='S'))
            return error;
        if (E=='W')
            point.xyz.x = -point.xyz.x;
        if (N=='S')
            point.xyz.y = -point.xyz.y;
        return point;
    }

    return error;
}


COORDINATE parse_input_line (struct buf *buf) {
    COORDINATE point = {{HUGE_VAL, HUGE_VAL, HUGE_VAL}};
    char *p = buf->buf;
    int d, m, dd, mm;
    double s, ss, ms, mmss, z;
    char E, N;

    for (buf->comment_or_blank = 0; isspace(*p); p++)
        ;
    if (('#' == *p) || (0==*p)) {
        buf->comment_or_blank = 1;
        return point;
    }

    /* First try DdM'S" */
    if (9==sscanf (buf->buf, "%d%*[dD]%d'%lf\"%c%d%*[dD]%d'%lf\"%c%lf", &d, &m, &s, &E, &dd, &mm, &ss, &N, &z)) {
        point.lpz.lam = (d  + (m  + s/60 ) / 60) * DEG_TO_RAD;
        point.lpz.phi = (dd + (mm + ss/60) / 60) * DEG_TO_RAD;
        point.lpz.z   = z;
        buf->format = 1;
        return point_adjust(point, E, N);
    }

    /* Then try D:M:S with GeographicLib-style colon separators */
    if (9==sscanf (buf->buf, "%d:%d:%lf%c%d:%d:%lf%c%lf", &d, &m, &s, &E, &dd, &mm, &ss, &N, &z)) {
        point.lpz.lam = (d  + (m  + s/60 ) / 60) * DEG_TO_RAD;
        point.lpz.phi = (dd + (mm + ss/60) / 60) * DEG_TO_RAD;
        point.lpz.z   = z;
        buf->format = 2;
        return point_adjust(point, E, N);
    }

    /* Then try nautical DdM.mm' */
    if (7==sscanf (buf->buf, "%d%*[dD]%lf'%c%d%*[dD]%lf'%c%lf", &d, &ms, &E, &dd, &mmss, &N, &z)) {
        point.lpz.lam = (d  + ms / 60) * DEG_TO_RAD;
        point.lpz.phi = (dd + mmss/60) * DEG_TO_RAD;
        point.lpz.z   = z;
        buf->format = 3;
        return point_adjust(point, E, N);
    }

    /* Then try nautical D:M.mm with GeographicLib-style colon separators */
    if (7==sscanf (buf->buf, "%d:%lf%c%d:%lf%c%lf", &d, &ms, &E, &dd, &mmss, &N, &z)) {
        point.lpz.lam = (d  + ms / 60) * DEG_TO_RAD;
        point.lpz.phi = (dd + mmss/60) * DEG_TO_RAD;
        point.lpz.z   = z;
        buf->format = 4;
        return point_adjust(point, E, N);
    }

    /* Then try decimal degrees without northing/easting indicators */
    if (3==sscanf (buf->buf, "%lf%lf%lf", &ms, &mmss, &z)) {
        point.lpz.lam = ms   * DEG_TO_RAD;
        point.lpz.phi = mmss * DEG_TO_RAD;
        point.lpz.z   = z;
        buf->format = 5;
        return point_adjust(point, 0, 0);
    }

    /* Finally try decimal degrees with northing/easting indicators */
    if (5==sscanf (buf->buf, "%lf%c%lf%c%lf", &ms, &E, &mmss, &N, &z)) {
        point.lpz.lam = ms * DEG_TO_RAD;
        point.lpz.phi = mmss * DEG_TO_RAD;
        point.lpz.z   = z;
        buf->format = 6;
        return point_adjust(point, E, N);
    }

    return point;
}



char *skip_3_columns (char *buf) {
    int i;
    for (i = 0;  i < 3;  i++) {
        while (isspace(*buf))
            buf++;
        while (!isspace(*buf))
            buf++;
    }

    return buf;
}



int main(int argc, char **argv) {
    PJ *P;

    int i, j, k, first_file_arg;

    FILE *in = 0;
    struct buf buf = {0, 0, 0, 0, 0};
    int angular_pipe = 0;


    /* Used in handling classic '-'-style options */
    char *opt_arg[256];
    char takes_arg[256];

    /* Used in handling '+'-style proj options */
    #define MAX_PLUS_ARG 200
    char *plus_argv[MAX_PLUS_ARG];
    int   plus_argc = 0;

    for (i = 0; i < 256; i++) {
        opt_arg[i]   = 0;
        takes_arg[i] = 0;
    }

    takes_arg[(int) 't'] = 1;
    takes_arg[(int) 'o'] = 1;

    /* Process all '-'-style options */
    for (i = 1;  i < argc;  i++) {
        int arg_group_size = strlen(argv[i]);

        if ('-' != argv[i][0])
            break;

        for (j = 1; j < arg_group_size; j++) {
            char c =  argv[i][j];

            /* Flag style option, i.e. taking no arguments */
            if (0==takes_arg[(int) c]) {
                opt_arg[(int) c] = argv[0];
                continue;
            }

            /* Option arg separate */
            if (0==argv[i][j+1]) {
                if ((argc==i + 1) || '+'==argv[i+1][0])
                    return 0*fprintf (stderr, "Bad or missing arg for option \"%c\"\n", c);
                opt_arg[(int) c] = argv[i + 1];
                i++;
                break;
            }

            /* Option arg inline */
            opt_arg[(int) c] = argv[i] + j + 1;
            break;
        }
    }


    /* Process all '+'-style options */
    for (i = i; i < argc; i++) {
        if ('+' != argv[i][0])
            break;
        plus_argv[plus_argc++] = argv[i];
    }

    first_file_arg = i;


    if (opt_arg[(int) 'v']) {
        char none[] = {"(set)"};
        for (i = 0;  i < 256; i++)
            if (0 != opt_arg[i])
                printf ("%c:  %s\n", i, argv[0]!=opt_arg[i]? opt_arg[i]: none);
    }


    P = pj_init (plus_argc, plus_argv);
    if (0==P)
        return 0*fputs ("Bad projection arguments", stderr);

    if (0!=strstr (P->descr, "angular output"))
        angular_pipe = 1;



    /* Allocate input buffer */
    buf.buf = calloc (1, 10000);
    if (0==buf.buf)
        return 0*fputs ("Out of memory", stderr);
    buf.size = 10000;

    if (first_file_arg==argc)
        in = stdin;

    /* Loop over all input files (or stdin) */
    for (k = first_file_arg;  (k < argc) || (in==stdin);  k++) {
        char *ret;
        int  lineno = 0;

        if (in != stdin) {
            if (0==strcmp (argv[k], "-"))
                in = stdin;
            else
                in = fopen (argv[k], "rt");

            /* Ignore non-existing files */
            if (0==in)
                continue;
        }

        /* Loop over all lines of the current file */
        while ((ret = fgets (buf.buf, buf.size, in))) {
            COORDINATE point;
            char *p;
            lineno++;
            if ((0==ret) || (0==strchr(buf.buf, '\n') ) ){
                fprintf (stderr, "Read error or overlong line on file \"%s\", line %d\n", in==stdin? "<stdin>": argv[k], lineno);
                break;    /* continue with next file (if any) */
            }


            point = parse_input_line (&buf);
            if (HUGE_VAL==point.xyz.x) {
                if (buf.comment_or_blank)
                    fputs (buf.buf, stdout);
                else fprintf (stdout, "# *** UNREADABLE ***:   %s", buf.buf);
                continue;
            }

            point = pj_apply_projection (point, (opt_arg['I'] != 0), P);
            pj_show_coordinate ("projected ", point, angular_pipe);

            p = skip_3_columns (buf.buf);
            puts(p);

        }


        if (stdin==in && k==argc)
            break;

        if ((in != stdin) && (0 != in))
            fclose(in);

    }



    return 0;
}