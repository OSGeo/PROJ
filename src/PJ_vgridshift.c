#define PJ_LIB__
#include "proj_internal.h"
#include "projects.h"

PROJ_HEAD(vgridshift, "Vertical grid shift");


static XYZ forward_3d(LPZ lpz, PJ *P) {
    PJ_COORD point = {{0,0,0,0}};
    point.lpz = lpz;

    if (P->vgridlist_geoid != NULL) {
        /* Only try the gridshift if at least one grid is loaded,
         * otherwise just pass the coordinate through unchanged. */
        point.xyz.z -= proj_vgrid_value(P, point.lp);
    }

    return point.xyz;
}


static LPZ reverse_3d(XYZ xyz, PJ *P) {
    PJ_COORD point = {{0,0,0,0}};
    point.xyz = xyz;

    if (P->vgridlist_geoid != NULL) {
        /* Only try the gridshift if at least one grid is loaded,
         * otherwise just pass the coordinate through unchanged. */
        point.xyz.z += proj_vgrid_value(P, point.lp);
    }

    return point.lpz;
}


static PJ_COORD forward_4d(PJ_COORD obs, PJ *P) {
    PJ_COORD point = {{0,0,0,0}};
    point.xyz = forward_3d (obs.lpz, P);
    return point;
}

static PJ_COORD reverse_4d(PJ_COORD obs, PJ *P) {
    PJ_COORD point;
    point.lpz = reverse_3d (obs.xyz, P);
    return point;
}


PJ *TRANSFORMATION(vgridshift,0) {

   if (!pj_param(P->ctx, P->params, "tgrids").i) {
        proj_log_error(P, "vgridshift: +grids parameter missing.");
        return pj_default_destructor(P, PJD_ERR_NO_ARGS);
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

