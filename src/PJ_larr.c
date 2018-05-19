#define PJ_LIB__

#include <math.h>

#include "projects.h"

PROJ_HEAD(larr, "Larrivee") "\n\tMisc Sph, no inv.";

#define SIXTH .16666666666666666


static XY s_forward (LP lp, PJ *P) {           /* Spheroidal, forward */
    XY xy = {0.0,0.0};
    (void) P;

    xy.x = 0.5 * lp.lam * (1. + sqrt(cos(lp.phi)));
    xy.y = lp.phi / (cos(0.5 * lp.phi) * cos(SIXTH * lp.lam));
    return xy;
}


PJ *PROJECTION(larr) {

    P->es = 0;
    P->fwd = s_forward;

    return P;
}
