#define PJ_LIB__

#include <errno.h>
#include <math.h>

#include "projects.h"

struct pj_opaque {
    double  phi1;
    double  fxa;
    double  fxb;
    int     mode;
};

PROJ_HEAD(rpoly, "Rectangular Polyconic")
    "\n\tConic, Sph., no inv.\n\tlat_ts=";

#define EPS 1e-9

static XY s_forward (LP lp, PJ *P) {           /* Spheroidal, forward */
    XY xy = {0.0,0.0};
    struct pj_opaque *Q = P->opaque;
    double fa;

    if (Q->mode)
        fa = tan(lp.lam * Q->fxb) * Q->fxa;
    else
        fa = 0.5 * lp.lam;
    if (fabs(lp.phi) < EPS) {
        xy.x = fa + fa;
        xy.y = - P->phi0;
    } else {
        xy.y = 1. / tan(lp.phi);
        xy.x = sin(fa = 2. * atan(fa * sin(lp.phi))) * xy.y;
        xy.y = lp.phi - P->phi0 + (1. - cos(fa)) * xy.y;
    }
    return xy;
}



PJ *PROJECTION(rpoly) {
    struct pj_opaque *Q = pj_calloc (1, sizeof (struct pj_opaque));
    if (0==Q)
        return pj_default_destructor(P, ENOMEM);
    P->opaque = Q;

    if ((Q->mode = (Q->phi1 = fabs(pj_param(P->ctx, P->params, "rlat_ts").f)) > EPS)) {
        Q->fxb = 0.5 * sin(Q->phi1);
        Q->fxa = 0.5 / Q->fxb;
    }
    P->es = 0.;
    P->fwd = s_forward;

    return P;
}
