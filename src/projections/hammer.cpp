#define PJ_LIB__

#include <errno.h>
#include <math.h>

#include "proj.h"
#include "proj_internal.h"

PROJ_HEAD(hammer, "Hammer & Eckert-Greifendorff")
    "\n\tMisc Sph, \n\tW= M=";

#define EPS 1.0e-10

namespace { // anonymous namespace
struct pj_opaque {
    double w;
    double m, rm;
};
} // anonymous namespace


static PJ_XY hammer_s_forward (PJ_LP lp, PJ *P) {           /* Spheroidal, forward */
    PJ_XY xy = {0.0,0.0};
    struct pj_opaque *Q = static_cast<struct pj_opaque*>(P->opaque);
    double cosphi, d;

    cosphi = cos(lp.phi);
    lp.lam *= Q->w;
    double denom = 1. + cosphi * cos(lp.lam);
    if( denom == 0.0 ) {
        proj_errno_set(P, PJD_ERR_TOLERANCE_CONDITION);
        return proj_coord_error().xy;
    }
    d = sqrt(2./denom);
    xy.x = Q->m * d * cosphi * sin(lp.lam);
    xy.y = Q->rm * d * sin(lp.phi);
    return xy;
}


static PJ_LP hammer_s_inverse (PJ_XY xy, PJ *P) {           /* Spheroidal, inverse */
    PJ_LP lp = {0.0,0.0};
    struct pj_opaque *Q = static_cast<struct pj_opaque*>(P->opaque);
    double z;

    z = sqrt(1. - 0.25*Q->w*Q->w*xy.x*xy.x - 0.25*xy.y*xy.y);
    if (fabs(2.*z*z-1.) < EPS) {
        lp.lam = HUGE_VAL;
        lp.phi = HUGE_VAL;
        proj_errno_set(P, PJD_ERR_LAT_OR_LON_EXCEED_LIMIT);
    } else {
        lp.lam = aatan2(Q->w * xy.x * z,2. * z * z - 1)/Q->w;
        lp.phi = aasin(P->ctx,z * xy.y);
    }
    return lp;
}


PJ *PROJECTION(hammer) {
    struct pj_opaque *Q = static_cast<struct pj_opaque*>(pj_calloc (1, sizeof (struct pj_opaque)));
    if (nullptr==Q)
        return pj_default_destructor (P, ENOMEM);
    P->opaque = Q;

    if (pj_param(P->ctx, P->params, "tW").i) {
        if ((Q->w = fabs(pj_param(P->ctx, P->params, "dW").f)) <= 0.)
            return pj_default_destructor (P, PJD_ERR_W_OR_M_ZERO_OR_LESS);
    } else
        Q->w = .5;
    if (pj_param(P->ctx, P->params, "tM").i) {
        if ((Q->m = fabs(pj_param(P->ctx, P->params, "dM").f)) <= 0.)
            return pj_default_destructor (P, PJD_ERR_W_OR_M_ZERO_OR_LESS);
    } else
        Q->m = 1.;

    Q->rm = 1. / Q->m;
    Q->m /= Q->w;

    P->es = 0.;
    P->fwd = hammer_s_forward;
    P->inv = hammer_s_inverse;

    return P;
}
