#define PJ_LIB__

#include <errno.h>
#include <stddef.h>
#include <string.h>
#include <time.h>

#include "proj_internal.h"

PROJ_HEAD(vgridshift, "Vertical grid shift");

namespace { // anonymous namespace
struct pj_opaque_vgridshift {
    double t_final;
    double t_epoch;
    double forward_multiplier;
};
} // anonymous namespace

static PJ_XYZ forward_3d(PJ_LPZ lpz, PJ *P) {
    struct pj_opaque_vgridshift *Q = (struct pj_opaque_vgridshift *) P->opaque;
    PJ_COORD point = {{0,0,0,0}};
    point.lpz = lpz;

    if (P->vgridlist_geoid != nullptr) {
        /* Only try the gridshift if at least one grid is loaded,
         * otherwise just pass the coordinate through unchanged. */
        point.xyz.z += proj_vgrid_value(P, point.lp, Q->forward_multiplier);
    }

    return point.xyz;
}


static PJ_LPZ reverse_3d(PJ_XYZ xyz, PJ *P) {
    struct pj_opaque_vgridshift *Q = (struct pj_opaque_vgridshift *) P->opaque;
    PJ_COORD point = {{0,0,0,0}};
    point.xyz = xyz;

    if (P->vgridlist_geoid != nullptr) {
        /* Only try the gridshift if at least one grid is loaded,
         * otherwise just pass the coordinate through unchanged. */
        point.xyz.z -= proj_vgrid_value(P, point.lp, Q->forward_multiplier);
    }

    return point.lpz;
}


static PJ_COORD forward_4d(PJ_COORD obs, PJ *P) {
    struct pj_opaque_vgridshift *Q = (struct pj_opaque_vgridshift *) P->opaque;
    PJ_COORD point = obs;

    /* If transformation is not time restricted, we always call it */
    if (Q->t_final==0 || Q->t_epoch==0) {
        point.xyz = forward_3d (obs.lpz, P);
        return point;
    }

    /* Time restricted - only apply transform if within time bracket */
    if (obs.lpzt.t < Q->t_epoch && Q->t_final > Q->t_epoch)
        point.xyz = forward_3d (obs.lpz, P);


    return point;
}

static PJ_COORD reverse_4d(PJ_COORD obs, PJ *P) {
    struct pj_opaque_vgridshift *Q = (struct pj_opaque_vgridshift *) P->opaque;
    PJ_COORD point = obs;

    /* If transformation is not time restricted, we always call it */
    if (Q->t_final==0 || Q->t_epoch==0) {
        point.lpz = reverse_3d (obs.xyz, P);
        return point;
    }

    /* Time restricted - only apply transform if within time bracket */
    if (obs.lpzt.t < Q->t_epoch && Q->t_final > Q->t_epoch)
        point.lpz = reverse_3d (obs.xyz, P);

    return point;
}


PJ *TRANSFORMATION(vgridshift,0) {
    struct pj_opaque_vgridshift *Q = static_cast<struct pj_opaque_vgridshift*>(pj_calloc (1, sizeof (struct pj_opaque_vgridshift)));
    if (nullptr==Q)
        return pj_default_destructor (P, ENOMEM);
    P->opaque = (void *) Q;

   if (!pj_param(P->ctx, P->params, "tgrids").i) {
        proj_log_error(P, "vgridshift: +grids parameter missing.");
        return pj_default_destructor(P, PJD_ERR_NO_ARGS);
    }

   /* TODO: Refactor into shared function that can be used  */
   /* by both vgridshift and hgridshift                     */
   if (pj_param(P->ctx, P->params, "tt_final").i) {
        Q->t_final = pj_param (P->ctx, P->params, "dt_final").f;
        if (Q->t_final == 0) {
            /* a number wasn't passed to +t_final, let's see if it was "now" */
            /* and set the time accordingly.                                 */
            if (!strcmp("now", pj_param(P->ctx, P->params, "st_final").s)) {
                    time_t now;
                    struct tm *date;
                    time(&now);
                    date = localtime(&now);
                    Q->t_final = 1900.0 + date->tm_year + date->tm_yday/365.0;
            }
        }
    }

   if (pj_param(P->ctx, P->params, "tt_epoch").i)
        Q->t_epoch = pj_param (P->ctx, P->params, "dt_epoch").f;

    /* historical: the forward direction subtracts the grid offset. */
    Q->forward_multiplier = -1.0;
    if (pj_param(P->ctx, P->params, "tmultiplier").i) {
        Q->forward_multiplier = pj_param(P->ctx, P->params, "dmultiplier").f;
    }

    /* Build gridlist. P->vgridlist_geoid can be empty if +grids only ask for optional grids. */
    proj_vgrid_init(P, "grids");

    /* Was gridlist compiled properly? */
    if ( proj_errno(P) ) {
        proj_log_error(P, "vgridshift: could not find required grid(s).");
        return pj_default_destructor(P, PJD_ERR_FAILED_TO_LOAD_GRID);
    }

    P->fwd4d = forward_4d;
    P->inv4d = reverse_4d;
    P->fwd3d  = forward_3d;
    P->inv3d  = reverse_3d;
    P->fwd    = nullptr;
    P->inv    = nullptr;

    P->left  = PJ_IO_UNITS_RADIANS;
    P->right = PJ_IO_UNITS_RADIANS;

    return P;
}
