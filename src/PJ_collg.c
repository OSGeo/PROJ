#define PJ_LIB__

#include <math.h>

#include "proj.h"
#include "projects.h"

PROJ_HEAD(collg, "Collignon") "\n\tPCyl, Sph.";
#define FXC 1.12837916709551257390
#define FYC 1.77245385090551602729
#define ONEEPS  1.0000001


static XY s_forward (LP lp, PJ *P) {           /* Spheroidal, forward */
    XY xy = {0.0,0.0};
    (void) P;
    if ((xy.y = 1. - sin(lp.phi)) <= 0.)
        xy.y = 0.;
    else
        xy.y = sqrt(xy.y);
    xy.x = FXC * lp.lam * xy.y;
    xy.y = FYC * (1. - xy.y);
    return (xy);
}


static LP s_inverse (XY xy, PJ *P) {           /* Spheroidal, inverse */
    LP lp = {0.0,0.0};
    lp.phi = xy.y / FYC - 1.;
    if (fabs(lp.phi = 1. - lp.phi * lp.phi) < 1.)
        lp.phi = asin(lp.phi);
    else if (fabs(lp.phi) > ONEEPS) {
        proj_errno_set(P, PJD_ERR_TOLERANCE_CONDITION);
        return lp;
    } else {
        lp.phi = lp.phi < 0. ? -M_HALFPI : M_HALFPI;
    }

    if ((lp.lam = 1. - sin(lp.phi)) <= 0.)
        lp.lam = 0.;
    else
        lp.lam = xy.x / (FXC * sqrt(lp.lam));
    return (lp);
}


PJ *PROJECTION(collg) {
    P->es = 0.0;
    P->inv = s_inverse;
    P->fwd = s_forward;

    return P;
}
