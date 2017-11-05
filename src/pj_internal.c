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
    if (0!=P->fwdobs)
        return P->fwdobs (coo, P);
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
    if (0!=P->invobs)
        return P->invobs (coo, P);
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



size_t pj_strlcpy(char *dst, const char *src, size_t siz) {
/*******************************************************************
   Copy src to string dst of size siz.  At most siz-1 characters
   will be copied.  Always NUL terminates (unless siz == 0).
   Returns strlen(src); if retval >= siz, truncation occurred.


 * Copyright (c) 1998 Todd C. Miller <Todd.Miller@courtesan.com>
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

  Source: http://www.i-pi.com/Training/EthicalHacking/Solutions/strlcpy.c

********************************************************************/
    register char *d = dst;
    register const char *s = src;
    register size_t n = siz;

    /* Copy as many bytes as will fit */
    if (n != 0 && --n != 0) {
        do {
            if ((*d++ = *s++) == 0)
                break;
        } while (--n != 0);
    }

    /* Not enough room in dst, add NUL and traverse rest of src */
    if (n == 0) {
        if (siz != 0)
            *d = '\0';      /* NUL-terminate dst */
        while (*s++)
            ;
    }

    return(s - src - 1);    /* count does not include NUL */
}



/* stuff below is *not* considered API, and will be moved to an "internal plumbing toolset" */



void proj_context_errno_set (PJ_CONTEXT *ctx, int err) {
    if (0==ctx)
        ctx = pj_get_default_ctx();
    pj_ctx_set_errno (ctx, err);
    return;
}



/* Set logging level 0-3. Higher number means more debug info. 0 turns it off */
enum proj_log_level proj_log_level (PJ_CONTEXT *ctx, enum proj_log_level log_level) {
    enum proj_log_level previous;
    if (0==ctx)
        ctx = pj_get_default_ctx();
    if (0==ctx)
        return PJ_LOG_TELL;
    previous = ctx->debug_level;
    if (PJ_LOG_TELL==log_level)
        return previous;
    ctx->debug_level = log_level;
    return previous;
}



/*  logging  */

/* pj_vlog resides in pj_log.c and relates to pj_log as vsprintf relates to sprintf */
void pj_vlog( projCtx ctx, int level, const char *fmt, va_list args );

void proj_log_error (PJ *P, const char *fmt, ...) {
    va_list args;
    va_start( args, fmt );
    pj_vlog (pj_get_ctx (P), PJ_LOG_ERROR , fmt, args);
    va_end( args );
}

void proj_log_debug (PJ *P, const char *fmt, ...) {
    va_list args;
    va_start( args, fmt );
    pj_vlog (pj_get_ctx (P), PJ_LOG_DEBUG_MAJOR , fmt, args);
    va_end( args );
}

void proj_log_trace (PJ *P, const char *fmt, ...) {
    va_list args;
    va_start( args, fmt );
    pj_vlog (pj_get_ctx (P), PJ_LOG_DEBUG_MINOR , fmt, args);
    va_end( args );
}

/* Put a new logging function into P's context. The opaque object app_data is passed as first arg at each call to the logger */
void proj_log_func (PJ_CONTEXT *ctx, void *app_data, PJ_LOG_FUNCTION log) {
    if (0==ctx)
        pj_get_default_ctx ();
    if (0==ctx)
        return;
    ctx->app_data = app_data;
    if (0!=log)
        ctx->logger = log;
}
