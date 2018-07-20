#define PJ_LIB__
#include <errno.h>
#include <math.h>

#include "proj.h"
#include "projects.h"

PROJ_HEAD(lagrng, "Lagrange") "\n\tMisc Sph\n\tW=";

#define TOL 1e-10

struct pj_opaque {
    double  a1;
    double  a2;
    double  hrw;
    double  hw;
    double  rw;
    double  w;
};


static XY s_forward (LP lp, PJ *P) {           /* Spheroidal, forward */
    XY xy = {0.0,0.0};
    struct pj_opaque *Q = P->opaque;
    double v, c;

    if (fabs(fabs(lp.phi) - M_HALFPI) < TOL) {
        xy.x = 0;
        xy.y = lp.phi < 0 ? -2. : 2.;
    } else {
        lp.phi = sin(lp.phi);
        v = Q->a1 * pow((1. + lp.phi)/(1. - lp.phi), Q->hrw);
        lp.lam *= Q->rw;
        c = 0.5 * (v + 1./v) + cos(lp.lam);
        if (c < TOL) {
            proj_errno_set(P, PJD_ERR_TOLERANCE_CONDITION);
            return xy;
        }
        xy.x = 2. * sin(lp.lam) / c;
        xy.y = (v - 1./v) / c;
    }
    return xy;
}


static LP s_inverse (XY xy, PJ *P) {           /* Spheroidal, inverse */
    LP lp = {0.0,0.0};
    struct pj_opaque *Q = P->opaque;
    double c, x2, y2p, y2m;

    if (fabs(fabs(xy.y) - 2.) < TOL) {
        lp.phi = xy.y < 0 ? -M_HALFPI : M_HALFPI;
        lp.lam = 0;
    } else {
        x2 = xy.x * xy.x;
        y2p = 2. + xy.y;
        y2m = 2. - xy.y;
        c = y2p * y2m - x2;
        if (fabs(c) < TOL) {
            proj_errno_set(P, PJD_ERR_TOLERANCE_CONDITION);
            return lp;
        }
        lp.phi = 2. * atan(pow((y2p * y2p + x2) / (Q->a2 * (y2m * y2m + x2)), Q->hw)) - M_HALFPI;
        lp.lam = Q->w * atan2(4. * xy.x, c);
    }
    return lp;
}


PJ *PROJECTION(lagrng) {
    double phi1;
    struct pj_opaque *Q = pj_calloc (1, sizeof (struct pj_opaque));
    if (0==Q)
        return pj_default_destructor (P, ENOMEM);
    P->opaque = Q;

    Q->w = pj_param(P->ctx, P->params, "dW").f;
    if (Q->w <= 0)
        return pj_default_destructor(P, PJD_ERR_W_OR_M_ZERO_OR_LESS);
    Q->hw = 0.5 * Q->w;
    Q->rw = 1. / Q->w;
    Q->hrw = 0.5 * Q->rw;
    phi1 = sin(pj_param(P->ctx, P->params, "rlat_1").f);
    if (fabs(fabs(phi1) - 1.) < TOL)
        return pj_default_destructor(P, PJD_ERR_LAT_LARGER_THAN_90);

    Q->a1 = pow((1. - phi1)/(1. + phi1), Q->hrw);
    Q->a2 = Q->a1 * Q->a1;

    P->es = 0.;
    P->inv = s_inverse;
    P->fwd = s_forward;

    return P;
}

