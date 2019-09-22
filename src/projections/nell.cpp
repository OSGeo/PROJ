#define PJ_LIB__

#include <math.h>

#include "proj.h"
#include "proj_internal.h"

PROJ_HEAD(nell, "Nell") "\n\tPCyl, Sph";

#define MAX_ITER 10
#define LOOP_TOL 1e-7


static PJ_XY nell_s_forward (PJ_LP lp, PJ *P) {           /* Spheroidal, forward */
    PJ_XY xy = {0.0,0.0};
    double k, V;
    int i;
    (void) P;

    k = 2. * sin(lp.phi);
    V = lp.phi * lp.phi;
    lp.phi *= 1.00371 + V * (-0.0935382 + V * -0.011412);
    for (i = MAX_ITER; i ; --i) {
        lp.phi -= V = (lp.phi + sin(lp.phi) - k) /
            (1. + cos(lp.phi));
        if (fabs(V) < LOOP_TOL)
            break;
    }
    xy.x = 0.5 * lp.lam * (1. + cos(lp.phi));
    xy.y = lp.phi;

    return xy;
}


static PJ_LP nell_s_inverse (PJ_XY xy, PJ *P) {           /* Spheroidal, inverse */
    PJ_LP lp = {0.0,0.0};
    lp.lam = 2. * xy.x / (1. + cos(xy.y));
    lp.phi = aasin(P->ctx,0.5 * (xy.y + sin(xy.y)));

    return lp;
}


PJ *PROJECTION(nell) {

    P->es = 0;
    P->inv = nell_s_inverse;
    P->fwd = nell_s_forward;

    return P;
}
