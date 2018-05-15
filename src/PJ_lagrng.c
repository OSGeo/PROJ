#define PJ_LIB__

#include <errno.h>
#include <math.h>

#include "proj.h"
#include "projects.h"

PROJ_HEAD(lagrng, "Lagrange") "\n\tMisc Sph, no inv.\n\tW=";

#define TOL 1e-10

struct pj_opaque {
    double  a1;
    double  hrw;
    double  rw;
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
        if ((c = 0.5 * (v + 1./v) + cos(lp.lam *= Q->rw)) < TOL) {
            proj_errno_set(P, PJD_ERR_TOLERANCE_CONDITION);
            return xy;
        }
        xy.x = 2. * sin(lp.lam) / c;
        xy.y = (v - 1./v) / c;
    }
    return xy;
}


PJ *PROJECTION(lagrng) {
    double phi1;
    struct pj_opaque *Q = pj_calloc (1, sizeof (struct pj_opaque));
    if (0==Q)
        return pj_default_destructor (P, ENOMEM);
    P->opaque = Q;

    Q->rw = pj_param(P->ctx, P->params, "dW").f;
    if (Q->rw <= 0)
        return pj_default_destructor(P, PJD_ERR_W_OR_M_ZERO_OR_LESS);

    Q->rw = 1. / Q->rw;
    Q->hrw = 0.5 * Q->rw;
    phi1 = sin(pj_param(P->ctx, P->params, "rlat_1").f);
    if (fabs(fabs(phi1) - 1.) < TOL)
        return pj_default_destructor(P, PJD_ERR_LAT_LARGER_THAN_90);

    Q->a1 = pow((1. - phi1)/(1. + phi1), Q->hrw);

    P->es = 0.;
    P->fwd = s_forward;

    return P;
}
