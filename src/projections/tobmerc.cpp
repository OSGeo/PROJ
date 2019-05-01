#define PJ_LIB__

#include <float.h>
#include <math.h>

#include "proj.h"
#include "proj_internal.h"
#include "proj_math.h"

PROJ_HEAD(tobmerc, "Tobler-Mercator") "\n\tCyl, Sph";

#define EPS10 1.e-10
static double logtanpfpim1(double x) {       /* log(tan(x/2 + M_FORTPI)) */
    if (fabs(x) <= DBL_EPSILON) {
        /* tan(M_FORTPI + .5 * x) can be approximated by  1.0 + x */
        return log1p(x);
    }
    return log(tan(M_FORTPI + .5 * x));
}

static PJ_XY tobmerc_s_forward (PJ_LP lp, PJ *P) {           /* Spheroidal, forward */
    PJ_XY xy = {0.0, 0.0};
    double cosphi;

    if (fabs(fabs(lp.phi) - M_HALFPI) <= EPS10) {
        proj_errno_set(P, PJD_ERR_TOLERANCE_CONDITION);
        return xy;
    }

    cosphi = cos(lp.phi);
    xy.x = P->k0 * lp.lam * cosphi * cosphi;
    xy.y = P->k0 * logtanpfpim1(lp.phi);
    return xy;
}

static PJ_LP tobmerc_s_inverse (PJ_XY xy, PJ *P) {           /* Spheroidal, inverse */
    PJ_LP lp = {0.0, 0.0};
    double cosphi;

    lp.phi = atan(sinh(xy.y / P->k0));
    cosphi = cos(lp.phi);
    lp.lam = xy.x / P->k0 / (cosphi * cosphi);
    return lp;
}

PJ *PROJECTION(tobmerc) {
    P->inv = tobmerc_s_inverse;
    P->fwd = tobmerc_s_forward;
    return P;
}
