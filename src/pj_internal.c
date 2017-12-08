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
 * Copyright (c) 2016, 2017, Thomas Knudsen/SDFE
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
#define PJ_INTERNAL_C
#include "proj_internal.h"
#include "projects.h"
#include <geodesic.h>

#include <ctype.h>
#include <stddef.h>
#include <stdarg.h>
#include <errno.h>


enum pj_io_units pj_left (PJ *P) {
    enum pj_io_units u = P->inverted? P->right: P->left;
    if (u==PJ_IO_UNITS_RADIANS)
        return PJ_IO_UNITS_RADIANS;
    return PJ_IO_UNITS_METERS;
}

enum pj_io_units pj_right (PJ *P) {
    enum pj_io_units u = P->inverted? P->left: P->right;
    if (u==PJ_IO_UNITS_RADIANS)
        return PJ_IO_UNITS_RADIANS;
    return PJ_IO_UNITS_METERS;
}


/* Work around non-constness of MSVC HUGE_VAL by providing functions rather than constants */
PJ_COORD proj_coord_error (void) {
    PJ_COORD c;
    c.v[0] = c.v[1] = c.v[2] = c.v[3] = HUGE_VAL;
    return c;
}


PJ_COORD pj_fwd4d (PJ_COORD coo, PJ *P) {
    if (0!=P->fwd4d)
        return P->fwd4d (coo, P);
    if (0!=P->fwd3d) {
        coo.xyz  =  pj_fwd3d (coo.lpz, P);
        return coo;
    }
    if (0!=P->fwd) {
        coo.xy  =  pj_fwd (coo.lp, P);
        return coo;
    }
    proj_errno_set (P, EINVAL);
    return proj_coord_error ();
}


PJ_COORD pj_inv4d (PJ_COORD coo, PJ *P) {
    if (0!=P->inv4d)
        return P->inv4d (coo, P);
    if (0!=P->inv3d) {
        coo.lpz  =  pj_inv3d (coo.xyz, P);
        return coo;
    }
    if (0!=P->inv) {
        coo.lp  =  pj_inv (coo.xy, P);
        return coo;
    }
    proj_errno_set (P, EINVAL);
    return proj_coord_error ();
}





/* Move P to a new context - or to the default context if 0 is specified */
void proj_context_set (PJ *P, PJ_CONTEXT *ctx) {
    if (0==ctx)
        ctx = pj_get_default_ctx ();
    pj_set_ctx (P, ctx);
    return;
}

void proj_context_inherit (PJ *parent, PJ *child) {
    if (0==parent)
        pj_set_ctx (child, pj_get_default_ctx());
    else
        pj_set_ctx (child, pj_get_ctx(parent));
    return;
}



/**************************************************************************************/
size_t pj_strlcpy(char *dst, const char *src, size_t dsize) {
/***************************************************************************************
   Copy src to string dst of size siz.  At most siz-1 characters
   will be copied.  Always NUL terminates (unless siz == 0).
   Returns strlen(src); if retval >= siz, truncation occurred.

 *
 * Copyright (c) 1998, 2015 Todd C. Miller <Todd.Miller@courtesan.com>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *

  Source: http://cvsweb.openbsd.org/cgi-bin/cvsweb/src/lib/libc/string/strlcpy.c

***************************************************************************************/
    const char *osrc = src;
    size_t nleft = dsize;

    /* Copy as many bytes as will fit. */
    if (nleft != 0) {
        while (--nleft != 0) {
            if ((*dst++ = *src++) == '\0')
                break;
        }
    }

    /* Not enough room in dst, add NUL and traverse rest of src. */
    if (nleft == 0) {
        if (dsize != 0)
            *dst = '\0';		/* NUL-terminate dst */
        while (*src++)
            ;
    }

    return(src - osrc - 1);	/* count does not include NUL */
}



/*****************************************************************************/
char *pj_chomp (char *c) {
/******************************************************************************
Strip pre- and postfix whitespace. Inline comments (indicated by '#') are
considered whitespace.
******************************************************************************/
    size_t i, n;
    char *comment;

    if (0==c)
        return 0;

    comment = strchr (c, '#');
    if (comment)
        *comment = 0;

    n = strlen (c);
    if (0==n)
        return c;

    for (i = n - 1; (i >= 1) && (isspace (c[i]) || ';'==c[i]);  i--)
        c[i] = 0;

    for (n = 0;  (0 != c[n]) && isspace (c[n]);  n++);

    for (i = 0;  0 != c[i + n];  i++)
        c[i] = c[i + n];

    c[i] = 0;
    return c;
}



/*****************************************************************************/
char *pj_shrink (char *c) {
/******************************************************************************
Collapse repeated whitespace, remove '+' and ';', make ',' and '=' greedy,
eating their surrounding whitespace.
******************************************************************************/
    size_t i, j, n, ws;

    if (0==c)
       return 0;

    pj_chomp (c);
    n = strlen (c);

    /* First collapse repeated whitespace (including +/;) */
    for (i = j = ws = 0;  j < n;  j++) {

        /* Blank out prefix '+', only if preceeded by whitespace */
        /* (i.e. keep it in 1.23e+08) */
        if ((i > 0) && ('+'==c[j]) && isspace (c[i]))
            c[j] = ' ';

        if (isspace (c[j]) || ';'==c[j] || '\\'==c[j]) {
            if (0==ws && (i > 0))
                c[i++] = ' ';
            ws = 1;
            continue;
        }
        else
            ws = 0;
        c[i++] = c[j];
    }
    c[i] = 0;
    n = strlen(c);

    /* Then make ',' and '=' greedy */
    for (i = j = 0; j < n; j++) {
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
    /* turn the massaged input into an array of strings */
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
char *pj_make_args (size_t argc, const char **argv) {
/******************************************************************************
pj_make_args is the inverse of the pj_trim_argc/pj_trim_argv combo: It
converts free format command line input to something proj_create can eat.

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

    for (i = n = 0;  i < argc;  i++) {
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
    return;
}






/*  logging  */

/* pj_vlog resides in pj_log.c and relates to pj_log as vsprintf relates to sprintf */
void pj_vlog( projCtx ctx, int level, const char *fmt, va_list args );


/***************************************************************************************/
enum proj_log_level proj_log_level (PJ_CONTEXT *ctx, enum proj_log_level log_level) {
/****************************************************************************************
   Set logging level 0-3. Higher number means more debug info. 0 turns it off
****************************************************************************************/
    enum proj_log_level previous;
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
void proj_log_func (PJ_CONTEXT *ctx, void *app_data, PJ_LOG_FUNCTION log) {
/******************************************************************************
    Put a new logging function into P's context. The opaque object app_data is
    passed as first arg at each call to the logger
******************************************************************************/
    if (0==ctx)
        pj_get_default_ctx ();
    if (0==ctx)
        return;
    ctx->app_data = app_data;
    if (0!=log)
        ctx->logger = log;
}
