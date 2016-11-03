/******************************************************************************
 * Project:  PROJ.4
 * Purpose:  Implement some service routines for the PJ_OBSERVATION generic
 *           geodetic data type
 * Author:   Thomas Knudsen,  thokn@sdfe.dk,  2016-06-09/2016-10-11
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
#define PJ_OBSERVATION_C
#include <proj.h>
#include <projects.h>
#include <float.h>
#include <math.h>


const PJ_OBSERVATION pj_observation_error = {
    /* Cannot use HUGE_VAL here: MSVC misimplements HUGE_VAL as something that is not compile time constant */
    {{DBL_MAX,DBL_MAX,DBL_MAX,DBL_MAX}},
    {{DBL_MAX,DBL_MAX,DBL_MAX}},
    0, 0
};


const PJ_OBSERVATION pj_observation_null = {
    {{0, 0, 0, 0}},
    {{0, 0, 0}},
    0, 0
};

const PJ *pj_shutdown = (PJ *) &pj_shutdown;


double pj_obs_dist_2d (PJ_OBSERVATION a, PJ_OBSERVATION b) {
    double *A = a.coo.v, *B = b.coo.v;
    return hypot (A[0] - B[0],  A[1] - B[1]);
}

double pj_obs_dist_3d (PJ_OBSERVATION a, PJ_OBSERVATION b) {
    double *A = a.coo.v, *B = b.coo.v;
    return hypot (hypot (A[0] - B[0],  A[1] - B[1]),  A[2] - B[2]);
}


PJ_OBSERVATION pj_fwdobs (PJ_OBSERVATION obs, PJ *P) {
    if (0!=P->fwd3d) {
        obs.coo.xyz  =  pj_fwd3d (obs.coo.lpz, P);
        return obs;
    }
    if (0!=P->fwd) {
        obs.coo.xy  =  pj_fwd (obs.coo.lp, P);
        return obs;
    }
    pj_error_set (P, EINVAL);
    return pj_observation_error;
}


PJ_OBSERVATION pj_invobs (PJ_OBSERVATION obs, PJ *P) {
    if (0!=P->inv3d) {
        obs.coo.lpz  =  pj_inv3d (obs.coo.xyz, P);
        return obs;
    }
    if (0!=P->inv) {
        obs.coo.lp  =  pj_inv (obs.coo.xy, P);
        return obs;
    }
    pj_error_set (P, EINVAL);
    return pj_observation_error;
}


PJ_OBSERVATION pj_apply (PJ *P, enum pj_direction direction, PJ_OBSERVATION obs) {
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

    pj_error_set (P, EINVAL);
    return pj_observation_error;
}


/* Measure numerical deviation after n roundtrips fwd-inv (or inv-fwd) */
double pj_roundtrip (PJ *P, enum pj_direction direction, int n, PJ_OBSERVATION obs) {
    int i;
    PJ_OBSERVATION o, u;

    if (0==P)
        return HUGE_VAL;

    if (n < 1) {
        pj_error_set (P, EINVAL);
        return HUGE_VAL;
    }

    o.coo = obs.coo;

    for (i = 0;  i < n;  i++) {
        switch (direction) {
            case PJ_FWD:
                u.coo.xyz  =  pj_fwd3d (o.coo.lpz, P);
                o.coo.lpz  =  pj_inv3d (u.coo.xyz, P);
                break;
            case PJ_INV:
                u.coo.lpz  =  pj_inv3d (o.coo.xyz, P);
                o.coo.xyz  =  pj_fwd3d (u.coo.lpz, P);
                break;
            default:
                pj_error_set (P, EINVAL);
                return HUGE_VAL;
        }
    }

    return pj_obs_dist_3d (o, obs);
}


PJ *pj_create (const char *definition) {
    return pj_init_plus (definition);
}

PJ *pj_create_argv (int argc, char **argv) {
    return pj_init (argc, argv);
}

int pj_error (PJ *P) {
    return pj_ctx_get_errno (pj_get_ctx(P));
}

/* From here: Minimum viable support for contexts */
void pj_error_set (PJ *P, int err) {
    pj_ctx_set_errno (pj_get_ctx(P), err);
}

/* Set debug level 0-3. Higher number means more debug info. 0 turns it off */
void pj_debug_set (PJ *P, enum pj_debug_level debuglevel) {
    PJ_CONTEXT *ctx;
    if (0==P)
        ctx = pj_get_default_ctx();
    else
        ctx = pj_get_ctx (P);
    ctx->debug_level = debuglevel;
}

/* Put a new logging function into P's context. The opaque object app_data is passed as first arg at each call to the logger */
void pj_log_set (PJ *P, void *app_data, void (*log)(void *, int, const char *)) {
    PJ_CONTEXT *ctx = pj_get_ctx (P);
    ctx->app_data = app_data;
    if (0!=log)
        ctx->logger = log;
}

/* Move P to a new context - initially a copy of the default context */
int pj_context_renew (PJ *P) {
    PJ_CONTEXT *ctx = pj_ctx_alloc ();
    if (0==ctx) {
        pj_error_set (P, ENOMEM);
        return 1;
    }

    pj_set_ctx (P, ctx);
    return 0;
}

/* Make sure daughter lives in same context as mother */
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
