/******************************************************************************
 * Project:  PROJ.4
 * Purpose:  This is primarily material originating from pj_obs_api.c
 *           (now proj_4D_api.c), that does not fit into the API
 *           category. Hence this pile of tubings and fittings for
 *           PROJ.4 internal plumbing.
 *
 * Author:   Thomas Knudsen,  thokn@sdfe.dk,  2017-07-05
 *
 ******************************************************************************
 * Copyright (c) 2016, 2017, 2018, Thomas Knudsen/SDFE
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
 *****************************************************************************/

#include <ctype.h>
#include <errno.h>
#include <math.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "geodesic.h"
#include "proj_internal.h"
#include "projects.h"


enum pj_io_units pj_left (PJ *P) {
    enum pj_io_units u = P->inverted? P->right: P->left;
    if (u==PJ_IO_UNITS_CLASSIC)
        return PJ_IO_UNITS_PROJECTED;
    return u;
}

enum pj_io_units pj_right (PJ *P) {
    enum pj_io_units u = P->inverted? P->left: P->right;
    if (u==PJ_IO_UNITS_CLASSIC)
        return PJ_IO_UNITS_PROJECTED;
    return u;
}


/* Work around non-constness of MSVC HUGE_VAL by providing functions rather than constants */
PJ_COORD proj_coord_error (void) {
    PJ_COORD c;
    c.v[0] = c.v[1] = c.v[2] = c.v[3] = HUGE_VAL;
    return c;
}



/**************************************************************************************/
PJ_COORD pj_approx_2D_trans (PJ *P, PJ_DIRECTION direction, PJ_COORD coo) {
/***************************************************************************************
Behave mostly as proj_trans, but attempt to use 2D interfaces only.
Used in gie.c, to enforce testing 2D code, and by PJ_pipeline.c to implement
chained calls starting out with a call to its 2D interface.
***************************************************************************************/
    if (0==P)
        return coo;
    if (P->inverted)
        direction = -direction;
    switch (direction) {
        case PJ_FWD:
            coo.xy = pj_fwd (coo.lp, P);
            return coo;
        case PJ_INV:
            coo.lp = pj_inv (coo.xy, P);
            return coo;
        case PJ_IDENT:
            return coo;
        default:
            break;
    }
    proj_errno_set (P, EINVAL);
    return proj_coord_error ();
}


/**************************************************************************************/
PJ_COORD pj_approx_3D_trans (PJ *P, PJ_DIRECTION direction, PJ_COORD coo) {
/***************************************************************************************
Companion to pj_approx_2D_trans.

Behave mostly as proj_trans, but attempt to use 3D interfaces only.
Used in gie.c, to enforce testing 3D code, and by PJ_pipeline.c to implement
chained calls starting out with a call to its 3D interface.
***************************************************************************************/
    if (0==P)
        return coo;
    if (P->inverted)
        direction = -direction;
    switch (direction) {
        case PJ_FWD:
            coo.xyz = pj_fwd3d (coo.lpz, P);
            return coo;
        case PJ_INV:
            coo.lpz = pj_inv3d (coo.xyz, P);
            return coo;
        case PJ_IDENT:
            return coo;
        default:
            break;
    }
    proj_errno_set (P, EINVAL);
    return proj_coord_error ();
}

/**************************************************************************************/
int pj_has_inverse(PJ *P) {
/***************************************************************************************
Check if a a PJ has an inverse.
***************************************************************************************/
    return ( (P->inverted && (P->fwd || P->fwd3d || P->fwd4d) ) ||
             ( P->inv || P->inv3d || P->inv4d) );
}


/* Move P to a new context - or to the default context if 0 is specified */
void proj_context_set (PJ *P, PJ_CONTEXT *ctx) {
    if (0==ctx)
        ctx = pj_get_default_ctx ();
    pj_set_ctx (P, ctx);
}


void proj_context_inherit (PJ *parent, PJ *child) {
    if (0==parent)
        pj_set_ctx (child, pj_get_default_ctx());
    else
        pj_set_ctx (child, pj_get_ctx(parent));
}



/*****************************************************************************/
char *pj_chomp (char *c) {
/******************************************************************************
Strip pre- and postfix whitespace. Inline comments (indicated by '#') are
considered whitespace.
******************************************************************************/
    size_t i, n;
    char *comment;
    char *start = c;

    if (0==c)
        return 0;

    comment = strchr (c, '#');
    if (comment)
        *comment = 0;

    n = strlen (c);
    if (0==n)
        return c;

    /* Eliminate postfix whitespace */
    for (i = n - 1;  (i > 0) && (isspace (c[i]) || ';'==c[i]);  i--)
        c[i] = 0;

    /* Find start of non-whitespace */
    while (0 != *start  &&  (';'==*start  ||  isspace (*start)))
        start++;

    n = strlen (start);
    if (0==n) {
        c[0] = 0;
        return c;
    }

    memmove (c, start, n + 1);
    return c;
}



/*****************************************************************************/
char *pj_shrink (char *c) {
/******************************************************************************
Collapse repeated whitespace. Remove '+' and ';'. Make ',' and '=' greedy,
consuming their surrounding whitespace.
******************************************************************************/
    size_t i, j, n;

    /* Flag showing that a whitespace (ws) has been written after last non-ws */
    size_t ws;

    if (0==c)
       return 0;

    pj_chomp (c);
    n = strlen (c);

    /* First collapse repeated whitespace (including +/;) */
    for (i = j = 0, ws = 0;  j < n;  j++) {

        /* Eliminate prefix '+', only if preceded by whitespace */
        /* (i.e. keep it in 1.23e+08) */
        if ((i > 0) && ('+'==c[j]) && ws)
            c[j] = ' ';
        if ((i==0) && ('+'==c[j]))
            c[j] = ' ';

        if (isspace (c[j]) || ';'==c[j]) {
            if (0==ws && (i > 0))
                c[i++] = ' ';
            ws = 1;
            continue;
        }
        else {
            ws = 0;
            c[i++] = c[j];
        }
    }
    c[i] = 0;
    n = strlen(c);

    /* Then make ',' and '=' greedy */
    for (i = j = 0;  j < n;  j++) {
        if (i==0) {
            c[i++] = c[j];
            continue;
        }

        /* Skip space before '='/',' */
        if ('='==c[j] || ','==c[j]) {
            if (c[i - 1]==' ')
               c[i - 1] = c[j];
            else
                c[i++] = c[j];
            continue;
        }

        if (' '==c[j] && ('='==c[i - 1] || ','==c[i - 1]) )
            continue;

        c[i++] = c[j];
    }
    c[i] = 0;
    return c;
}



/*****************************************************************************/
size_t pj_trim_argc (char *args) {
/******************************************************************************
Trim all unnecessary whitespace (and non-essential syntactic tokens) from the
argument string, args, and count its number of elements.
******************************************************************************/
    size_t i, m, n;
    pj_shrink (args);
    n = strlen (args);
    if (n==0)
        return 0;
    for (i = m = 0;  i < n;  i++) {
        if (' '==args[i]) {
            args[i] = 0;
            m++;
        }
    }
    return m + 1;
}



/*****************************************************************************/
char **pj_trim_argv (size_t argc, char *args) {
/******************************************************************************
Create an argv-style array from elements placed in the argument string, args.

args is a trimmed string as returned by pj_trim_argc(), and argc is the number
of trimmed strings found (i.e. the return value of pj_trim_args()). Hence,
    int argc    = pj_trim_argc (args);
    char **argv = pj_trim_argv (argc, args);
will produce a classic style (argc, argv) pair from a string of whitespace
separated args. No new memory is allocated for storing the individual args
(they stay in the args string), but for the pointers to the args a new array
is allocated and returned.

It is the duty of the caller to free this array.
******************************************************************************/
    size_t i, j;
    char **argv;

    if (0==args)
        return 0;
    if (0==argc)
        return 0;


    /* turn the input string into an array of strings */
    argv = (char **) calloc (argc, sizeof (char *));
    if (0==argv)
        return 0;
    argv[0] = args;
    for (i = 0, j = 1;  ;  i++) {
        if (0==args[i]) {
            argv[j++] = args + (i + 1);
        }
        if (j==argc)
            break;
    }
    return argv;
}



/*****************************************************************************/
char *pj_make_args (size_t argc, char **argv) {
/******************************************************************************
pj_make_args is the inverse of the pj_trim_argc/pj_trim_argv combo: It
converts free format command line input to something proj_create can consume.

Allocates, and returns, an array of char, large enough to hold a whitespace
separated copy of the args in argv. It is the duty of the caller to free this
array.
******************************************************************************/
    size_t i, n;
    char *p;

    for (i = n = 0;  i < argc;  i++)
        n += strlen (argv[i]);

    p = pj_calloc (n + argc + 1, sizeof (char));
    if (0==p)
        return 0;
    if (0==argc)
        return p;

    for (i = 0;  i < argc;  i++) {
        strcat (p, argv[i]);
        strcat (p, " ");
    }
    return pj_shrink (p);
}



/*****************************************************************************/
void proj_context_errno_set (PJ_CONTEXT *ctx, int err) {
/******************************************************************************
Raise an error directly on a context, without going through a PJ belonging
to that context.
******************************************************************************/
    if (0==ctx)
        ctx = pj_get_default_ctx();
    pj_ctx_set_errno (ctx, err);
}

/*  logging  */

/* pj_vlog resides in pj_log.c and relates to pj_log as vsprintf relates to sprintf */
void pj_vlog( projCtx ctx, int level, const char *fmt, va_list args );


/***************************************************************************************/
PJ_LOG_LEVEL proj_log_level (PJ_CONTEXT *ctx, PJ_LOG_LEVEL log_level) {
/****************************************************************************************
   Set logging level 0-3. Higher number means more debug info. 0 turns it off
****************************************************************************************/
    PJ_LOG_LEVEL previous;
    if (0==ctx)
        ctx = pj_get_default_ctx();
    if (0==ctx)
        return PJ_LOG_TELL;
    previous = abs (ctx->debug_level);
    if (PJ_LOG_TELL==log_level)
        return previous;
    ctx->debug_level = log_level;
    return previous;
}


/*****************************************************************************/
void proj_log_error (PJ *P, const char *fmt, ...) {
/******************************************************************************
   For reporting the most severe events.
******************************************************************************/
    va_list args;
    va_start( args, fmt );
    pj_vlog (pj_get_ctx (P), PJ_LOG_ERROR , fmt, args);
    va_end( args );
}


/*****************************************************************************/
void proj_log_debug (PJ *P, const char *fmt, ...) {
/******************************************************************************
   For reporting debugging information.
******************************************************************************/
    va_list args;
    va_start( args, fmt );
    pj_vlog (pj_get_ctx (P), PJ_LOG_DEBUG_MAJOR , fmt, args);
    va_end( args );
}


/*****************************************************************************/
void proj_log_trace (PJ *P, const char *fmt, ...) {
/******************************************************************************
   For reporting embarrasingly detailed debugging information.
******************************************************************************/
    va_list args;
    va_start( args, fmt );
    pj_vlog (pj_get_ctx (P), PJ_LOG_DEBUG_MINOR , fmt, args);
    va_end( args );
}


/*****************************************************************************/
void proj_log_func (PJ_CONTEXT *ctx, void *app_data, PJ_LOG_FUNCTION logf) {
/******************************************************************************
    Put a new logging function into P's context. The opaque object app_data is
    passed as first arg at each call to the logger
******************************************************************************/
    if (0==ctx)
        pj_get_default_ctx ();
    if (0==ctx)
        return;
    ctx->app_data = app_data;
    if (0!=logf)
        ctx->logger = logf;
}
