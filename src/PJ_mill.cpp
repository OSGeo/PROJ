#define PJ_LIB__

#include <math.h>

#include "projects.h"

PROJ_HEAD(mill, "Miller Cylindrical") "\n\tCyl, Sph";

static XY s_forward (LP lp, PJ *P) {           /* Spheroidal, forward */
    XY xy = {0.0,0.0};
    (void) P;

    xy.x = lp.lam;
    xy.y = log(tan(M_FORTPI + lp.phi * .4)) * 1.25;

    return (xy);
}


static LP s_inverse (XY xy, PJ *P) {           /* Spheroidal, inverse */
    LP lp = {0.0,0.0};
    (void) P;

    lp.lam = xy.x;
    lp.phi = 2.5 * (atan(exp(.8 * xy.y)) - M_FORTPI);

    return (lp);
}


PJ *PROJECTION(mill) {
    P->es = 0.;
    P->inv = s_inverse;
    P->fwd = s_forward;

    return P;
}
