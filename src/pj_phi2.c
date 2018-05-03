/* determine latitude angle phi-2 */

#include <math.h>

#include "projects.h"

static const double TOL = 1.0e-10;
static const int N_ITER = 15;

double
pj_phi2(projCtx ctx, double ts, double e) {
    const double eccnth = 0.5 * e;
    double phi = M_HALFPI - 2.0 * atan(ts);
    int i = N_ITER;
    for(;;) {
        const double con = e * sin(phi);
        const double dphi = M_HALFPI - 2.0 * atan(ts * pow((1.0 - con) /
           (1.0 + con), eccnth)) - phi;
        phi += dphi;
        if( !(fabs(dphi) > TOL && --i) )
            break;
    }
    if (i <= 0)
        pj_ctx_set_errno(ctx, PJD_ERR_NON_CON_INV_PHI2);
    return phi;
}
