#define PJ_LIB__

#include <errno.h>
#include <stddef.h>
#include <string.h>
#include <time.h>

#include "proj_internal.h"
#include "projects.h"

PROJ_HEAD(vgridshift, "Vertical grid shift");

struct pj_opaque_vgridshift {
    double t_final;
    double t_epoch;
    double forward_multiplier;
};

static XYZ forward_3d(LPZ lpz, PJ *P) {
    struct pj_opaque_vgridshift *Q = (struct pj_opaque_vgridshift *) P->opaque;
    PJ_COORD point = {{0,0,0,0}};
    point.lpz = lpz;

    if (P->vgridlist_geoid != NULL) {
        /* Only try the gridshift if at least one grid is loaded,
         * otherwise just pass the coordinate through unchanged. */
        point.xyz.z += Q->forward_multiplier * proj_vgrid_value(P, point.lp);
    }

    return point.xyz;
}


static LPZ reverse_3d(XYZ xyz, PJ *P) {
    struct pj_opaque_vgridshift *Q = (struct pj_opaque_vgridshift *) P->opaque;
    PJ_COORD point = {{0,0,0,0}};
    point.xyz = xyz;

    if (P->vgridlist_geoid != NULL) {
        /* Only try the gridshift if at least one grid is loaded,
         * otherwise just pass the coordinate through unchanged. */
        point.xyz.z -= Q->forward_multiplier * proj_vgrid_value(P, point.lp);
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
    struct pj_opaque_vgridshift *Q = pj_calloc (1, sizeof (struct pj_opaque_vgridshift));
    if (0==Q)
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

    /* historical: the forward direction substracts the grid offset. */
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
    P->fwd    = 0;
    P->inv    = 0;

    P->left  = PJ_IO_UNITS_ANGULAR;
    P->right = PJ_IO_UNITS_ANGULAR;

    return P;
}
