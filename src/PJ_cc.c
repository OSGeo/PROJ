#define PJ_LIB__

#include <math.h>

#include "proj.h"
#include "projects.h"

PROJ_HEAD(cc, "Central Cylindrical") "\n\tCyl, Sph";
#define EPS10 1.e-10


static XY s_forward (LP lp, PJ *P) {           /* Spheroidal, forward */
    XY xy = {0.0,0.0};
    if (fabs (fabs(lp.phi) - M_HALFPI) <= EPS10) {
        proj_errno_set(P, PJD_ERR_TOLERANCE_CONDITION);
        return xy;
    }
    xy.x = lp.lam;
    xy.y = tan(lp.phi);
    return xy;
}


static LP s_inverse (XY xy, PJ *P) {           /* Spheroidal, inverse */
    LP lp = {0.0,0.0};
    (void) P;
    lp.phi = atan(xy.y);
    lp.lam = xy.x;
    return lp;
}



PJ *PROJECTION(cc) {
    P->es = 0.;

    P->inv = s_inverse;
    P->fwd = s_forward;

    return P;
}
