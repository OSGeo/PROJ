#define PJ_LIB__

#include <errno.h>
#include <math.h>

#include "proj.h"
#include "proj_internal.h"

namespace { // anonymous namespace
struct pj_opaque {
    double  phi1;
    double  fxa;
    double  fxb;
    int     mode;
};
} // anonymous namespace

PROJ_HEAD(rpoly, "Rectangular Polyconic")
    "\n\tConic, Sph, no inv\n\tlat_ts=";

#define EPS 1e-9

static PJ_XY rpoly_s_forward (PJ_LP lp, PJ *P) {           /* Spheroidal, forward */
    PJ_XY xy = {0.0,0.0};
    struct pj_opaque *Q = static_cast<struct pj_opaque*>(P->opaque);
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
    struct pj_opaque *Q = static_cast<struct pj_opaque*>(pj_calloc (1, sizeof (struct pj_opaque)));
    if (nullptr==Q)
        return pj_default_destructor(P, ENOMEM);
    P->opaque = Q;

    if ((Q->mode = (Q->phi1 = fabs(pj_param(P->ctx, P->params, "rlat_ts").f)) > EPS)) {
        Q->fxb = 0.5 * sin(Q->phi1);
        Q->fxa = 0.5 / Q->fxb;
    }
    P->es = 0.;
    P->fwd = rpoly_s_forward;

    return P;
}
