#define PJ_LIB__
#include "proj_internal.h"
#include "projects.h"

PROJ_HEAD(vgridshift, "Vertical grid shift");


static XYZ forward_3d(LPZ lpz, PJ *P) {
    PJ_TRIPLET point;
    point.lpz = lpz;

    if (P->vgridlist_geoid != NULL) {
        /* Only try the gridshift if at least one grid is loaded,
         * otherwise just pass the coordinate through unchanged. */
        point.xyz.z -= proj_vgrid_value(P, point.lp);
    }

    return point.xyz;
}


static LPZ reverse_3d(XYZ xyz, PJ *P) {
    PJ_TRIPLET point;
    point.xyz = xyz;

    if (P->vgridlist_geoid != NULL) {
        /* Only try the gridshift if at least one grid is loaded,
         * otherwise just pass the coordinate through unchanged. */
        point.xyz.z += proj_vgrid_value(P, point.lp);
    }

    return point.lpz;
}


static PJ_COORD forward_4d(PJ_COORD obs, PJ *P) {
    PJ_COORD point;
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

    P->left  = PJ_IO_UNITS_RADIANS;
    P->right = PJ_IO_UNITS_RADIANS;

    return P;
}


#ifndef PJ_SELFTEST
/* selftest stub */
int pj_vgridshift_selftest (void) {return 0;}
#else
int pj_vgridshift_selftest (void) {
    PJ *P;
    PJ_COORD expect, a, b;
    double dist;
    int failures = 0;

    /* fail on purpose: +grids parameter is mandatory*/
    P = proj_create(PJ_DEFAULT_CTX, "+proj=vgridshift");
    if (0!=P) {
        proj_destroy (P);
        return 99;
    }

    /* fail on purpose: open non-existing grid */
    P = proj_create(PJ_DEFAULT_CTX, "+proj=vgridshift +grids=nonexistinggrid.gtx");
    if (0!=P) {
        proj_destroy (P);
        return 999;
    }

    /* Failure most likely means the grid is missing */
    P = proj_create(PJ_DEFAULT_CTX, "+proj=vgridshift +grids=egm96_15.gtx +ellps=GRS80");
    if (0==P)
        return 10;

    a = proj_coord(0,0,0,0);
    a.lpz.lam = PJ_TORAD(12.5);
    a.lpz.phi = PJ_TORAD(55.5);
    b = a;
    dist = proj_roundtrip (P, PJ_FWD, 1, &b);
    if (dist > 0.00000001)
        return 1;

    expect = a;
    /* Appears there is a difference between the egm96_15.gtx distributed by OSGeo4W,  */
    /* and the one from http://download.osgeo.org/proj/vdatum/egm96_15/egm96_15.gtx    */
    /* Was: expect.lpz.z   = -36.021305084228515625;  (download.osgeo.org)         */
    /* Was: expect.lpz.z   = -35.880001068115234000;  (OSGeo4W)                    */
    /* This is annoying, but must be handled elsewhere. So for now, we check for both. */
    expect.lpz.z   = -36.021305084228516;
    failures = 0;
    b = proj_trans(P, PJ_FWD, a);
    if (proj_xyz_dist(expect.xyz, b.xyz) > 1e-4)  failures++;
    expect.lpz.z   = -35.880001068115234000;
    if (proj_xyz_dist(expect.xyz, b.xyz) > 1e-4)  failures++;
    if (failures > 1)
        return 2;

    proj_destroy (P);

    return 0;
}
#endif
