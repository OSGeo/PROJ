#define PJ_LIB__
#include "proj_internal.h"
#include "projects.h"

PROJ_HEAD(hgridshift, "Horizontal grid shift");

static XYZ forward_3d(LPZ lpz, PJ *P) {
    PJ_TRIPLET point;
    point.lpz = lpz;

    if (P->gridlist != NULL) {
        /* Only try the gridshift if at least one grid is loaded,
         * otherwise just pass the coordinate through unchanged. */
        point.lp = proj_hgrid_apply(P, point.lp, PJ_FWD);
    }

    return point.xyz;
}


static LPZ reverse_3d(XYZ xyz, PJ *P) {
    PJ_TRIPLET point;
    point.xyz = xyz;

    if (P->gridlist != NULL) {
        /* Only try the gridshift if at least one grid is loaded,
         * otherwise just pass the coordinate through unchanged. */
        point.lp = proj_hgrid_apply(P, point.lp, PJ_INV);
    }

    return point.lpz;
}


static PJ_OBS forward_obs(PJ_OBS obs, PJ *P) {
    PJ_OBS point;
    point.coo.xyz = forward_3d (obs.coo.lpz, P);
    return point;
}


static PJ_OBS reverse_obs(PJ_OBS obs, PJ *P) {
    PJ_OBS point;
    point.coo.lpz = reverse_3d (obs.coo.xyz, P);
    return point;
}



PJ *PROJECTION(hgridshift) {

    P->fwdobs = forward_obs;
    P->invobs = reverse_obs;
    P->fwd3d  = forward_3d;
    P->inv3d  = reverse_3d;
    P->fwd    = 0;
    P->inv    = 0;

    P->left  = PJ_IO_UNITS_RADIANS;
    P->right = PJ_IO_UNITS_RADIANS;

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


#ifndef PJ_SELFTEST
/* selftest stub */
int pj_hgridshift_selftest (void) {return 0;}
#else
int pj_hgridshift_selftest (void) {
    PJ *P;
    PJ_OBS expect, a, b;
    double dist;

    /* fail on purpose: +grids parameter is mandatory*/
    P = proj_create(PJ_DEFAULT_CTX, "+proj=hgridshift");
    if (0!=P) {
        proj_destroy (P);
        return 99;
    }

    /* fail on purpose: open non-existing grid */
    P = proj_create(PJ_DEFAULT_CTX, "+proj=hgridshift +grids=@nonexistinggrid.gsb,anothernonexistinggrid.gsb");
    if (0!=P) {
        proj_destroy (P);
        return 999;
    }

    /* Failure most likely means the grid is missing */
    P = proj_create(PJ_DEFAULT_CTX, "+proj=hgridshift +grids=nzgd2kgrid0005.gsb +ellps=GRS80");
    if (0==P)
        return 10;

    a = proj_obs_null;
    a.coo.lpz.lam = PJ_TORAD(173);
    a.coo.lpz.phi = PJ_TORAD(-45);

    dist = proj_roundtrip (P, PJ_FWD, 1, a.coo);
    if (dist > 0.00000001) {
        printf("dist: %f\n",dist);
        return 1;
    }

    expect.coo.lpz.lam = PJ_TORAD(172.999892181021551);
    expect.coo.lpz.phi = PJ_TORAD(-45.001620431954613);
    b = proj_trans_obs(P, PJ_FWD, a);
    if (proj_xy_dist(expect.coo.xy, b.coo.xy) > 1e-4)
        return 2;

    proj_destroy(P);
    return 0;
}
#endif
