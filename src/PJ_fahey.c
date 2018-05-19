#define PJ_LIB__

#include <math.h>

#include "projects.h"

PROJ_HEAD(fahey, "Fahey") "\n\tPcyl, Sph.";

#define TOL 1e-6


static XY s_forward (LP lp, PJ *P) {           /* Spheroidal, forward */
    XY xy = {0.0,0.0};
    (void) P;

    xy.x = tan(0.5 * lp.phi);
    xy.y = 1.819152 * xy.x;
    xy.x = 0.819152 * lp.lam * asqrt(1 - xy.x * xy.x);
    return xy;
}


static LP s_inverse (XY xy, PJ *P) {           /* Spheroidal, inverse */
    LP lp = {0.0,0.0};
    (void) P;

    xy.y /= 1.819152;
    lp.phi = 2. * atan(xy.y);
    xy.y = 1. - xy.y * xy.y;
    lp.lam = fabs(xy.y) < TOL ? 0. : xy.x / (0.819152 * sqrt(xy.y));
    return lp;
}


PJ *PROJECTION(fahey) {
    P->es = 0.;
    P->inv = s_inverse;
    P->fwd = s_forward;

    return P;
}
