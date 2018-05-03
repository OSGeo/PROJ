/* Determine latitude angle phi-2. */

#include <math.h>

#include "projects.h"

/*****************************************************************************/
double pj_phi2(projCtx ctx, double ts, double e) {
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
    const double TOL = 1.0e-10;
    const int N_ITER = 15;
    double phi = M_HALFPI - 2.0 * atan(ts);
    int i;

    for (i = N_ITER;  i > 0;  i--) {
        const double con = e * sin(phi);
        const double dphi =
            M_HALFPI -
            2.0 * atan(ts * pow((1.0 - con) / (1.0 + con), 0.5 * e)) - phi;

        phi += dphi;

        if (fabs(dphi) < TOL)
            return phi;
    }

    if (i == 0)
        pj_ctx_set_errno(ctx, PJD_ERR_NON_CON_INV_PHI2);

    return HUGE_VAL;
}
