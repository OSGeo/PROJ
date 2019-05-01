#define PJ_LIB__

#include <math.h>

#include "proj.h"
#include "proj_internal.h"

PROJ_HEAD(mbtfpp, "McBride-Thomas Flat-Polar Parabolic") "\n\tCyl, Sph";

#define CS  .95257934441568037152
#define FXC .92582009977255146156
#define FYC 3.40168025708304504493
#define C23 .66666666666666666666
#define C13 .33333333333333333333
#define ONEEPS  1.0000001


static PJ_XY mbtfpp_s_forward (PJ_LP lp, PJ *P) {           /* Spheroidal, forward */
    PJ_XY xy = {0.0,0.0};
    (void) P;

    lp.phi = asin(CS * sin(lp.phi));
    xy.x = FXC * lp.lam * (2. * cos(C23 * lp.phi) - 1.);
    xy.y = FYC * sin(C13 * lp.phi);
    return xy;
}


static PJ_LP mbtfpp_s_inverse (PJ_XY xy, PJ *P) {           /* Spheroidal, inverse */
    PJ_LP lp = {0.0,0.0};

    lp.phi = xy.y / FYC;
    if (fabs(lp.phi) >= 1.) {
        if (fabs(lp.phi) > ONEEPS) {
            proj_errno_set(P, PJD_ERR_TOLERANCE_CONDITION);
            return lp;
        } else {
            lp.phi = (lp.phi < 0.) ? -M_HALFPI : M_HALFPI;
        }
    } else
        lp.phi = asin(lp.phi);

    lp.lam = xy.x / ( FXC * (2. * cos(C23 * (lp.phi *= 3.)) - 1.) );
    if (fabs(lp.phi = sin(lp.phi) / CS) >= 1.) {
        if (fabs(lp.phi) > ONEEPS) {
            proj_errno_set(P, PJD_ERR_TOLERANCE_CONDITION);
            return lp;
        } else {
            lp.phi = (lp.phi < 0.) ? -M_HALFPI : M_HALFPI;
        }
    } else
        lp.phi = asin(lp.phi);

    return lp;
}


PJ *PROJECTION(mbtfpp) {

    P->es = 0.;
    P->inv = mbtfpp_s_inverse;
    P->fwd = mbtfpp_s_forward;

    return P;
}
