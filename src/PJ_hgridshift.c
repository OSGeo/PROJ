#define PJ_LIB__
#include "proj_internal.h"
#include "projects.h"

PROJ_HEAD(hgridshift, "Horizontal grid shift");

static XYZ forward_3d(LPZ lpz, PJ *P) {
    PJ_COORD point = {{0,0,0,0}};
    point.lpz = lpz;

    if (P->gridlist != NULL) {
        /* Only try the gridshift if at least one grid is loaded,
         * otherwise just pass the coordinate through unchanged. */
        point.lp = proj_hgrid_apply(P, point.lp, PJ_FWD);
    }

    return point.xyz;
}


static LPZ reverse_3d(XYZ xyz, PJ *P) {
    PJ_COORD point = {{0,0,0,0}};
    point.xyz = xyz;

    if (P->gridlist != NULL) {
        /* Only try the gridshift if at least one grid is loaded,
         * otherwise just pass the coordinate through unchanged. */
        point.lp = proj_hgrid_apply(P, point.lp, PJ_INV);
    }

    return point.lpz;
}


static PJ_COORD forward_4d (PJ_COORD obs, PJ *P) {
    obs.xyz = forward_3d (obs.lpz, P);
    return obs;
}


static PJ_COORD reverse_4d (PJ_COORD obs, PJ *P) {
    obs.lpz = reverse_3d (obs.xyz, P);
    return obs;
}



PJ *TRANSFORMATION(hgridshift,0) {

    P->fwd4d  = forward_4d;
    P->inv4d  = reverse_4d;
    P->fwd3d  = forward_3d;
    P->inv3d  = reverse_3d;
    P->fwd    = 0;
    P->inv    = 0;

    P->left  = PJ_IO_UNITS_ANGULAR;
    P->right = PJ_IO_UNITS_ANGULAR;

    if (0==pj_param(P->ctx, P->params, "tgrids").i) {
        proj_log_error(P, "hgridshift: +grids parameter missing.");
        return pj_default_destructor (P, PJD_ERR_NO_ARGS);
    }


    proj_hgrid_init(P, "grids");
    /* Was gridlist compiled properly? */
    if ( proj_errno(P) ) {
        proj_log_error(P, "hgridshift: could not find required grid(s).");
        return pj_default_destructor(P, PJD_ERR_FAILED_TO_LOAD_GRID);
    }

    return P;
}

