/* Determine latitude angle phi-2. */

#include <math.h>

#include "projects.h"

static const double TOL = 1.0e-10;
static const int N_ITER = 15;

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
    double eccnth = .5 * e;
    double Phi = M_HALFPI - 2. * atan(ts);
    double con;
    int i = N_ITER;

    for(;;) {
        double dphi;
        con = e * sin(Phi);
        dphi = M_HALFPI - 2. * atan(ts * pow((1. - con) /
           (1. + con), eccnth)) - Phi;

        Phi += dphi;

        if (fabs(dphi) > TOL && --i)
            continue;
        break;
    }
    if (i <= 0)
        pj_ctx_set_errno(ctx, PJD_ERR_NON_CON_INV_PHI2);
    return Phi;
}
