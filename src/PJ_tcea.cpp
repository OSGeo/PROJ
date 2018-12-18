#define PJ_LIB__

#include <math.h>

#include "projects.h"

PROJ_HEAD(tcea, "Transverse Cylindrical Equal Area") "\n\tCyl, Sph";


static XY s_forward (LP lp, PJ *P) {           /* Spheroidal, forward */
    XY xy = {0.0,0.0};
    xy.x = cos (lp.phi) * sin (lp.lam) / P->k0;
    xy.y = P->k0 * (atan2 (tan (lp.phi), cos (lp.lam)) - P->phi0);
    return xy;
}


static LP s_inverse (XY xy, PJ *P) {           /* Spheroidal, inverse */
    LP lp = {0.0, 0.0};
    double t;

    xy.y = xy.y / P->k0 + P->phi0;
    xy.x *= P->k0;
    t = sqrt (1. - xy.x * xy.x);
    lp.phi = asin (t * sin (xy.y));
    lp.lam = atan2 (xy.x, t * cos (xy.y));
    return lp;
}


PJ *PROJECTION(tcea) {
    P->inv = s_inverse;
    P->fwd = s_forward;
    P->es = 0.;
    return P;
}
