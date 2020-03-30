/* Determine latitude angle phi-2. */

#include <math.h>

#include "proj.h"
#include "proj_internal.h"

static const double TOL = 1.0e-10;
static const int N_ITER = 15;

/*****************************************************************************/
double pj_phi2(projCtx ctx, const double ts0, const double e) {
/******************************************************************************
Determine latitude angle phi-2.
Inputs:
  ts = exp(-psi) where psi is the isometric latitude (dimensionless)
  e = eccentricity of the ellipsoid (dimensionless)
Output:
  phi = geographic latitude (radians)
Here isometric latitude is defined by
  psi = log( tan(pi/4 + phi/2) *
             ( (1 - e*sin(phi)) / (1 + e*sin(phi)) )^(e/2) )
      = asinh(tan(phi)) - e * atanh(e * sin(phi))
This routine inverts this relation using the iterative scheme given
by Snyder (1987), Eqs. (7-9) - (7-11)
*******************************************************************************/
    const double eccnth = .5 * e;
    double ts = ts0;
#ifdef no_longer_used_original_convergence_on_exact_dphi
    double Phi = M_HALFPI - 2. * atan(ts);
#endif
    int i = N_ITER;

    for(;;) {
        /*
         * sin(Phi) = sin(PI/2 - 2* atan(ts))
         *          = cos(2*atan(ts))
         *          = 2*cos^2(atan(ts)) - 1
         *          = 2 / (1 + ts^2) - 1
         *          = (1 - ts^2) / (1 + ts^2)
         */
        const double sinPhi = (1 - ts * ts) / (1 + ts * ts);
        const double con = e * sinPhi;
        double old_ts = ts;
        ts = ts0 * pow((1. - con) / (1. + con), eccnth);
#ifdef no_longer_used_original_convergence_on_exact_dphi
        /* The convergence criterion is nominally on exact dphi */
        const double newPhi = M_HALFPI - 2. * atan(ts);
        const double dphi = newPhi - Phi;
        Phi = newPhi;
#else
        /* If we don't immediately update Phi from this, we can
         * change the conversion criterion to save us computing atan() at each step.
         * Particularly we can observe that:
         * |atan(ts) - atan(old_ts)| <= |ts - old_ts|
         * So if |ts - old_ts| matches our convergence criterion, we're good.
         */
        const double dphi = 2 * (ts - old_ts);
#endif
        if (fabs(dphi) > TOL && --i) {
            continue;
        }
        break;
    }
    if (i <= 0)
        pj_ctx_set_errno(ctx, PJD_ERR_NON_CON_INV_PHI2);
    return M_HALFPI - 2. * atan(ts);
}
