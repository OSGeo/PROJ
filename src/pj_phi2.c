/* Determine latitude angle phi-2. */

#include <math.h>

#include "projects.h"

static const double TOL = 1.0e-10;
static const int N_ITER = 15;

double pj_phi2(projCtx ctx, double ts, double e) {
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
