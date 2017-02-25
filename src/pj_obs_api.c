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


/* Used for zero-initializing new objects */
const PJ_COORD pj_coo_null = {{0, 0, 0, 0}};
const PJ_OBS   pj_obs_null = {
    {{0, 0, 0, 0}},
    {{0, 0, 0}},
    0, 0
};

/* Magic object signaling proj system shutdown mode to routines taking a PJ * arg */
const PJ *pj_shutdown = (PJ *) &pj_shutdown;

/* Geodesic distance between two points with angular 2D coordinates */
double pj_lp_dist (PJ *P, LP a, LP b) {
    double s12, azi1, azi2;
    /* Note: the geodesic code takes arguments in degrees */
    geod_inverse (P->geod, TODEG(a.phi), TODEG(a.lam), TODEG(b.phi), TODEG(b.lam), &s12, &azi1, &azi2);
    return s12;
}

/* Euclidean distance between two points with linear 2D coordinates */
double pj_xy_dist (XY a, XY b) {
    return hypot (a.x - b.x, a.y - b.y);
}

/* Euclidean distance between two points with linear 3D coordinates */
double pj_xyz_dist (XYZ a, XYZ b) {
    return hypot (hypot (a.x - b.x, a.y - b.y), a.z - b.z);
}


/* Work around non-constness of MSVC HUGE_VAL by providing functions rather than constants */
static PJ_COORD pj_coo_error (void) {
    PJ_COORD c;
    c.v[0] = c.v[1] = c.v[2] = c.v[3] = HUGE_VAL;
    return c;
}

static PJ_OBS pj_obs_error (void) {
    PJ_OBS obs;
    obs.coo = pj_coo_error ();
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
    pj_err_level (P, EINVAL);
    return pj_obs_error ();
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
    pj_err_level (P, EINVAL);
    return pj_obs_error ();
}


/* Apply the transformation P to the observation obs */
PJ_OBS pj_trans (PJ *P, enum pj_direction direction, PJ_OBS obs) {
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

    pj_err_level (P, EINVAL);
    return pj_obs_error ();
}


/* Measure numerical deviation after n roundtrips fwd-inv (or inv-fwd) */
double pj_roundtrip (PJ *P, enum pj_direction direction, int n, PJ_OBS obs) {
    int i;
    PJ_OBS o, u;

    if (0==P)
        return HUGE_VAL;

    if (n < 1) {
        pj_err_level (P, EINVAL);
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
                pj_err_level (P, EINVAL);
                return HUGE_VAL;
        }
    }

    return pj_xyz_dist (o.coo.xyz, obs.coo.xyz);
}


PJ *pj_create (const char *definition) {
    return pj_init_plus (definition);
}


PJ *pj_create_argv (int argc, char **argv) {
    return pj_init (argc, argv);
}


/* Below: Minimum viable support for contexts. The first four functions   */
/* relate to error reporting, debugging, and logging, hence being generically */
/* useful. The remaining is a compact implementation of the more low level    */
/* proj_api.h thread contexts, which may or may not be useful  */

/* Set error level 0-3, or query current level */
int pj_err_level (PJ *P, int err_level) {
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
enum pj_log_level pj_log_level (PJ *P, enum pj_log_level log_level) {
    enum pj_log_level previous;
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


/* Put a new logging function into P's context. The opaque object app_data is passed as first arg at each call to the logger */
void pj_log_func (PJ *P, void *app_data, void (*log)(void *, int, const char *)) {
    PJ_CONTEXT *ctx = pj_get_ctx (P);
    ctx->app_data = app_data;
    if (0!=log)
        ctx->logger = log;
}


/* Move P to a new context - initially a copy of the default context */
int pj_context_renew (PJ *P) {
    PJ_CONTEXT *ctx = pj_ctx_alloc ();
    if (0==ctx) {
        pj_err_level (P, ENOMEM);
        return 1;
    }

    pj_set_ctx (P, ctx);
    return 0;
}


/* Move daughter to mother's context */
void pj_context_inherit (PJ *mother, PJ *daughter) {
    pj_set_ctx (daughter, pj_get_ctx (mother));
}


void pj_context_free (const PJ *P) {
    PJ_CONTEXT *ctx;

    if (0==P)
        return;

    /* During shutdown it should be OK to free the default context - but this needs more work */
    if (pj_shutdown==P) {
        /* The obvious solution "pj_ctx_free (pj_get_default_ctx ())" fails */
        return;
    }

    ctx = pj_get_ctx ((PJ *) P);

    /* Otherwise, trying to free the default context is a no-op */
    if (pj_get_default_ctx ()==ctx)
        return;

    /* The common (?) case: free the context and move the PJ to the default context */
    pj_ctx_free (ctx);
    pj_set_ctx ((PJ *) P, pj_get_default_ctx ());
}


/* Minimum support for fileapi - which may never have been used... */
void pj_fileapi_set (PJ *P, void *fileapi) {
    PJ_CONTEXT *ctx = pj_get_ctx (P);
    ctx->fileapi = (projFileAPI *) fileapi;
}
