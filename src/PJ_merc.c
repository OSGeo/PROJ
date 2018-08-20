#define PJ_LIB__

#include <float.h>
#include <math.h>

#include "proj_internal.h"
#include "proj.h"
#include "proj_math.h"
#include "projects.h"

PROJ_HEAD(merc, "Mercator") "\n\tCyl, Sph&Ell\n\tlat_ts=";
PROJ_HEAD(webmerc, "WGS 84 Web Mercator / WGS 84 Pseudo Mercator") "\n\tCyl, Ell\n\t";

#define EPS10 1.e-10
static double logtanpfpim1(double x) {       /* log(tan(x/2 + M_FORTPI)) */
    if (fabs(x) <= DBL_EPSILON) {
        /* tan(M_FORTPI + .5 * x) can be approximated by  1.0 + x */
        return log1p(x);
    }
    return log(tan(M_FORTPI + .5 * x));
}

static XY e_forward (LP lp, PJ *P) {          /* Ellipsoidal, forward */
    XY xy = {0.0,0.0};
    if (fabs(fabs(lp.phi) - M_HALFPI) <= EPS10) {
        proj_errno_set(P, PJD_ERR_TOLERANCE_CONDITION);
        return xy;
    }
    xy.x = P->k0 * lp.lam;
    xy.y = - P->k0 * log(pj_tsfn(lp.phi, sin(lp.phi), P->e));
    return xy;
}


static XY s_forward (LP lp, PJ *P) {           /* Spheroidal, forward */
    XY xy = {0.0,0.0};
    if (fabs(fabs(lp.phi) - M_HALFPI) <= EPS10) {
        proj_errno_set(P, PJD_ERR_TOLERANCE_CONDITION);
        return xy;
}
    xy.x = P->k0 * lp.lam;
    xy.y = P->k0 * logtanpfpim1(lp.phi);
    return xy;
}


static LP e_inverse (XY xy, PJ *P) {          /* Ellipsoidal, inverse */
    LP lp = {0.0,0.0};
    if ((lp.phi = pj_phi2(P->ctx, exp(- xy.y / P->k0), P->e)) == HUGE_VAL) {
        proj_errno_set(P, PJD_ERR_TOLERANCE_CONDITION);
        return lp;
}
    lp.lam = xy.x / P->k0;
    return lp;
}


static LP s_inverse (XY xy, PJ *P) {           /* Spheroidal, inverse */
    LP lp = {0.0,0.0};
    lp.phi = M_HALFPI - 2. * atan(exp(-xy.y / P->k0));
    lp.lam = xy.x / P->k0;
    return lp;
}


PJ *PROJECTION(merc) {
    double phits=0.0;
    int is_phits;

    if( (is_phits = pj_param(P->ctx, P->params, "tlat_ts").i) ) {
        phits = fabs(pj_param(P->ctx, P->params, "rlat_ts").f);
        if (phits >= M_HALFPI)
            return pj_default_destructor(P, PJD_ERR_LAT_TS_LARGER_THAN_90);
    }

    if (P->es != 0.0) { /* ellipsoid */
        if (is_phits)
            P->k0 = pj_msfn(sin(phits), cos(phits), P->es);
        P->inv = e_inverse;
        P->fwd = e_forward;
    }

    else { /* sphere */
        if (is_phits)
            P->k0 = cos(phits);
        P->inv = s_inverse;
        P->fwd = s_forward;
    }

    return P;
}

PJ *PROJECTION(webmerc) {

    /* Overriding k_0, lat_0, lon_0, x_0 and y_0 with fixed parameters */
    P->k0 = 1.0;
    P->phi0 = 0.0;
    P->lam0 = 0.0;
    P->x0 = 0;
    P->y0 = 0;

    /* Overriding ellipsoid and using WGS 84 */
    P->f = 1.0/298.257223563;
    P->a  = 6378137.0;
    P->es = P->f*(2-P->f);
    pj_calc_ellipsoid_params (P, P->a, P->es);

    P->inv = s_inverse;
    P->fwd = s_forward;
    return P;
}
