#define PJ_LIB__
#include <proj.h>
#include <projects.h>

PROJ_HEAD(vgridshift, "Vertical grid shift");

static void *freeup_msg (PJ *P, int errlev) {
    if (0==P)
        return 0;

    if (0!=P->ctx)
        pj_ctx_set_errno (P->ctx, errlev);

    return pj_dealloc(P);
}


static void freeup (PJ *P) {
    freeup_msg (P, 0);
    return;
}


static XYZ forward_3d(LPZ lpz, PJ *P) {
    PJ_TRIPLET point;
    point.lpz = lpz;

    if (P->gridlist != NULL) {
        /* Only try the gridshift if at least one grid is loaded,
         * otherwise just pass the coordinate through unchanged. */
        pj_apply_vgridshift( P, "sgrids",
                             &(P->gridlist),
                             &(P->gridlist_count),
                             1, 1, 0,
                             &point.xyz.x, &point.xyz.y, &point.xyz.z );
    }

    return point.xyz;
}


static LPZ reverse_3d(XYZ xyz, PJ *P) {
    PJ_TRIPLET point;
    point.xyz = xyz;

    if (P->gridlist != NULL) {
        /* Only try the gridshift if at least one grid is loaded,
         * otherwise just pass the coordinate through unchanged. */
        pj_apply_vgridshift( P, "sgrids",
                             &(P->gridlist),
                             &(P->gridlist_count),
                             0, 1, 0,
                             &point.xyz.x, &point.xyz.y, &point.xyz.z );
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


PJ *PROJECTION(vgridshift) {

   if (!pj_param(P->ctx, P->params, "tgrids").i) {
        pj_log_error(P, "vgridshift: +grids parameter missing.");
        return freeup_msg(P, -1);
    }

    /* Build gridlist. P->gridlist can be empty if +grids only ask for optional grids. */
    P->gridlist = pj_gridlist_from_nadgrids( P->ctx, pj_param(P->ctx, P->params, "sgrids").s,
                                             &(P->gridlist_count) );

    /* Was gridlist compiled properly? */
    if ( pj_ctx_get_errno(P->ctx) ) {
        pj_log_error(P, "vgridshift: could not find required grid(s).");
        pj_dalloc(P->gridlist);
        P->gridlist = NULL;
        return freeup_msg(P, -38);
    }

    P->fwdobs = forward_obs;
    P->invobs = reverse_obs;
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
    PJ_OBS expect, a, b;
    double dist;

    /* fail on purpose: +grids parameter it mandatory*/
    P = pj_create("+proj=vgridshift");
    if (0!=P)
        return 99;

    /* fail on purpose: open non-existing grid */
    P = pj_create("+proj=vgridshift +grids=nonexistinggrid.gtx");
    if (0!=P)
        return 999;

    /* Failure most likely means the grid is missing */
    P = pj_create ("+proj=vgridshift +grids=egm96_15.gtx +ellps=GRS80");
    if (0==P)
        return 10;

    a = pj_obs_null;
    a.coo.lpz.lam = TORAD(12.5);
    a.coo.lpz.phi = TORAD(55.5);

    dist = pj_roundtrip (P, PJ_FWD, 1, a);
    if (dist > 0.00000001)
        return 1;

    expect = a;
    expect.coo.lpz.z   = -36.021305084228515625;
    b = pj_trans(P, PJ_FWD, a);
    if (pj_xyz_dist(expect.coo.xyz, b.coo.xyz) > 1e-10)
        return 2;


    pj_free(P);

    return 0;
}
#endif
