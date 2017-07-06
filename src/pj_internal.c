/******************************************************************************
 * Project:  PROJ.4
 * Purpose:  This is primarily material originating from pj_obs_api.c,
 *           that does not fit into the API category. Hence this pile of
 *           tubings and fittings for PROJ.4 internal plumbing.
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
#include <projects.h>
#include <geodesic.h>

#include <stddef.h>
#include <stdarg.h>
#include <errno.h>




/* Used for zero-initializing new objects */
const PJ_COORD proj_coord_null = {{0, 0, 0, 0}};
const PJ_OBS   proj_obs_null = {
    {{0, 0, 0, 0}},
    {{0, 0, 0}},
    0, 0
};


/* Work around non-constness of MSVC HUGE_VAL by providing functions rather than constants */
PJ_COORD proj_coord_error (void) {
    PJ_COORD c;
    c.v[0] = c.v[1] = c.v[2] = c.v[3] = HUGE_VAL;
    return c;
}

PJ_OBS proj_obs_error (void) {
    PJ_OBS obs;
    obs.coo = proj_coord_error ();
    obs.anc.v[0] = obs.anc.v[1] = obs.anc.v[2] = HUGE_VAL;
    obs.id = obs.flags = 0;
    return obs;
}



PJ_OBS pj_fwdobs (PJ_OBS obs, PJ *P) {
    if (0!=P->fwdobs) {
        obs  =  P->fwdobs (obs, P);
        return obs;
    }
    if (0!=P->fwd3d) {
        obs.coo.xyz  =  pj_fwd3d (obs.coo.lpz, P);
        return obs;
    }
    if (0!=P->fwd) {
        obs.coo.xy  =  pj_fwd (obs.coo.lp, P);
        return obs;
    }
    proj_errno_set (P, EINVAL);
    return proj_obs_error ();
}


PJ_OBS pj_invobs (PJ_OBS obs, PJ *P) {
    if (0!=P->invobs) {
        obs  =  P->invobs (obs, P);
        return obs;
    }
    if (0!=P->inv3d) {
        obs.coo.lpz  =  pj_inv3d (obs.coo.xyz, P);
        return obs;
    }
    if (0!=P->inv) {
        obs.coo.lp  =  pj_inv (obs.coo.xy, P);
        return obs;
    }
    proj_errno_set (P, EINVAL);
    return proj_obs_error ();
}



PJ_COORD pj_fwdcoord (PJ_COORD coo, PJ *P) {
    if (0!=P->fwdcoord)
        return P->fwdcoord (coo, P);
    if (0!=P->fwdobs) {
        PJ_OBS obs = proj_obs_null;
        obs.coo = coo;
        obs  =  P->fwdobs (obs, P);
        return obs.coo;
    }
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


PJ_COORD pj_invcoord (PJ_COORD coo, PJ *P) {
    if (0!=P->invcoord)
        return P->invcoord (coo, P);
    if (0!=P->invobs) {
        PJ_OBS obs = proj_obs_null;
        obs.coo = coo;
        obs  =  P->invobs (obs, P);
        return obs.coo;
    }
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
