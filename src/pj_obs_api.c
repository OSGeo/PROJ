/******************************************************************************
 * Project:  PROJ.4
 * Purpose:  Implement a (currently minimalistic) proj API based primarily
 *           on the PJ_OBS generic geodetic data type.
 *
 *           proj thread contexts have not seen widespread use, so one of the
 *           intentions with this new API is to make them less visible on the
 *           API surface: Contexts do not have a life by themselves, they are
 *           visible only through their associated PJs, and the number of
 *           functions supporting them is limited.
 *
 * Author:   Thomas Knudsen,  thokn@sdfe.dk,  2016-06-09/2016-11-06
 *
 ******************************************************************************
 * Copyright (c) 2016, Thomas Knudsen/SDFE
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
#define PJ_OBS_API_C
#include <proj.h>
#include <projects.h>
#include <geodesic.h>

#include <float.h>
#include <math.h>
#include <stdarg.h>


/* Used for zero-initializing new objects */
const PJ_COORD proj_coord_null = {{0, 0, 0, 0}};
const PJ_OBS   proj_obs_null = {
    {{0, 0, 0, 0}},
    {{0, 0, 0}},
    0, 0
};

/* Magic object signaling proj system shutdown mode to routines taking a PJ * arg */
const PJ *proj_shutdown = (PJ *) &proj_shutdown;

PJ_COORD proj_coord (double x, double y, double z, double t) {
    PJ_COORD res;
    res.v[0] = x;
    res.v[1] = y;
    res.v[2] = z;
    res.v[3] = t;
    return res;
}

/* Geodesic distance between two points with angular 2D coordinates */
double proj_lp_dist (PJ *P, LP a, LP b) {
    double s12, azi1, azi2;
    /* Note: the geodesic code takes arguments in degrees */
    geod_inverse (P->geod, PJ_TODEG(a.phi), PJ_TODEG(a.lam), PJ_TODEG(b.phi), PJ_TODEG(b.lam), &s12, &azi1, &azi2);
    return s12;
}

/* Euclidean distance between two points with linear 2D coordinates */
double proj_xy_dist (XY a, XY b) {
    return hypot (a.x - b.x, a.y - b.y);
}

/* Euclidean distance between two points with linear 3D coordinates */
double proj_xyz_dist (XYZ a, XYZ b) {
    return hypot (hypot (a.x - b.x, a.y - b.y), a.z - b.z);
}


/* Work around non-constness of MSVC HUGE_VAL by providing functions rather than constants */
static PJ_COORD proj_coord_error (void) {
    PJ_COORD c;
    c.v[0] = c.v[1] = c.v[2] = c.v[3] = HUGE_VAL;
    return c;
}

static PJ_OBS proj_obs_error (void) {
    PJ_OBS obs;
    obs.coo = proj_coord_error ();
    obs.anc.v[0] = obs.anc.v[1] = obs.anc.v[2] = HUGE_VAL;
    obs.id = obs.flags = 0;
    return obs;
}



static PJ_OBS pj_fwdobs (PJ_OBS obs, PJ *P) {
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
    proj_err_level (P, EINVAL);
    return proj_obs_error ();
}


static PJ_OBS pj_invobs (PJ_OBS obs, PJ *P) {
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
    proj_err_level (P, EINVAL);
    return proj_obs_error ();
}



static PJ_COORD pj_fwdcoord (PJ_COORD coo, PJ *P) {
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
    proj_err_level (P, EINVAL);
    return proj_coord_error ();
}


static PJ_COORD pj_invcoord (PJ_COORD coo, PJ *P) {
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
    proj_err_level (P, EINVAL);
    return proj_coord_error ();
}


/* Apply the transformation P to the coordinate coo */
PJ_OBS proj_trans_obs (PJ *P, enum proj_direction direction, PJ_OBS obs) {
    if (0==P)
        return obs;

    switch (direction) {
        case PJ_FWD:
            return pj_fwdobs (obs, P);
        case PJ_INV:
            return  pj_invobs (obs, P);
        case PJ_IDENT:
            return obs;
        default:
            break;
    }

    proj_err_level (P, EINVAL);
    return proj_obs_error ();
}



/* Apply the transformation P to the coordinate coo */
PJ_COORD proj_trans_coord (PJ *P, enum proj_direction direction, PJ_COORD coo) {
    if (0==P)
        return coo;

    switch (direction) {
        case PJ_FWD:
            return pj_fwdcoord (coo, P);
        case PJ_INV:
            return  pj_invcoord (coo, P);
        case PJ_IDENT:
            return coo;
        default:
            break;
    }

    proj_err_level (P, EINVAL);
    return proj_coord_error ();
}


/* Measure numerical deviation after n roundtrips fwd-inv (or inv-fwd) */
double proj_roundtrip (PJ *P, enum proj_direction direction, int n, PJ_OBS obs) {
    int i;
    PJ_OBS o, u;

    if (0==P)
        return HUGE_VAL;

    if (n < 1) {
        proj_err_level (P, EINVAL);
        return HUGE_VAL;
    }

    o.coo = obs.coo;

    for (i = 0;  i < n;  i++) {
        switch (direction) {
            case PJ_FWD:
                u  =  pj_fwdobs (o, P);
                o  =  pj_invobs (u, P);
                break;
            case PJ_INV:
                u  =  pj_invobs (o, P);
                o  =  pj_fwdobs (u, P);
                break;
            default:
                proj_err_level (P, EINVAL);
                return HUGE_VAL;
        }
    }

    return proj_xyz_dist (o.coo.xyz, obs.coo.xyz);
}


PJ *proj_create (PJ_CONTEXT *ctx, const char *definition) {
    if (0==ctx)
        ctx = pj_get_default_ctx ();
    return pj_init_plus_ctx (ctx, definition);
}

PJ *proj_create_argv (PJ_CONTEXT *ctx, int argc, char **argv) {
    if (0==ctx)
        ctx = pj_get_default_ctx ();
    return pj_init_ctx (ctx, argc, argv);
}

PJ *proj_destroy (PJ *P) {
    pj_free (P);
    return 0;
}

/* Below: Minimum viable support for contexts. The first four functions   */
/* relate to error reporting, debugging, and logging, hence being generically */
/* useful. The remaining is a compact implementation of the more low level    */
/* proj_api.h thread contexts, which may or may not be useful  */

/* Set error level 0-3, or query current level */
int proj_err_level (PJ *P, int err_level) {
    int previous;
    PJ_CONTEXT *ctx;
    if (0==P)
        ctx = pj_get_default_ctx();
    else
        ctx = pj_get_ctx (P);
    previous = pj_ctx_get_errno (ctx);
    if (PJ_ERR_TELL==err_level)
        return previous;
    pj_ctx_set_errno (ctx, err_level);
    return previous;
}


/* Set logging level 0-3. Higher number means more debug info. 0 turns it off */
enum proj_log_level proj_log_level (PJ *P, enum proj_log_level log_level) {
    enum proj_log_level previous;
    PJ_CONTEXT *ctx;
    if (0==P)
        ctx = pj_get_default_ctx();
    else
        ctx = pj_get_ctx (P);
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
void proj_log_func (PJ *P, void *app_data, void (*log)(void *, int, const char *)) {
    PJ_CONTEXT *ctx = pj_get_ctx (P);
    ctx->app_data = app_data;
    if (0!=log)
        ctx->logger = log;
}


/* Move P to a new context - or to the default context i 0 is specified */
void proj_context_set (PJ *P, PJ_CONTEXT *ctx) {
    if (0==ctx)
        ctx = pj_get_default_ctx ();
    pj_set_ctx (P, ctx);
    return;
}


/* Create a new context - or provide a pointer to the default context */
/* Prepared for more elaborate sscanf style setup syntax */
PJ_CONTEXT *proj_context_create (char *setup, ...) {
    PJ_CONTEXT *ctx;
    va_list args;

    /* The classic single threaded, backwards compatible way */
    if (0==setup)
        return pj_get_default_ctx ();

    /* Simplest multi threaded way: ctx is just a container for pj_errno */
    ctx = pj_ctx_alloc ();
    if (0==ctx)
        return 0;
    if (0==strcmp(setup, ""))
        return ctx;

    /* Multi threaded with thread specific logging */
    va_start (args, setup);
    if (0==strcmp(setup, "log"))
        ctx->logger = va_arg (args, (void (*)(void *, int, const char *)));

    /* Additional thread specific setup goes here */
    va_end (args);
    return  ctx;
}


void proj_context_destroy (PJ_CONTEXT *ctx) {
    if (0==ctx)
        return;

    /* Trying to free the default context is a no-op */
    if (pj_get_default_ctx ()==ctx)
        return;

    pj_ctx_free (ctx);
}


/* Minimum support for fileapi - which may never have been used... */
void proj_fileapi_set (PJ *P, void *fileapi) {
    PJ_CONTEXT *ctx = pj_get_ctx (P);
    ctx->fileapi = (projFileAPI *) fileapi;
}
