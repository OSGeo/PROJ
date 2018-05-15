#define PJ_LIB__
#include <math.h>

#include "projects.h"

PROJ_HEAD(crast, "Craster Parabolic (Putnins P4)") "\n\tPCyl., Sph.";

#define XM  0.97720502380583984317
#define RXM 1.02332670794648848847
#define YM  3.06998012383946546542
#define RYM 0.32573500793527994772
#define THIRD   0.333333333333333333


static XY s_forward (LP lp, PJ *P) {           /* Spheroidal, forward */
    XY xy = {0.0,0.0};
    (void) P;
    lp.phi *= THIRD;
    xy.x = XM * lp.lam * (2. * cos(lp.phi + lp.phi) - 1.);
    xy.y = YM * sin(lp.phi);
    return xy;
}


static LP s_inverse (XY xy, PJ *P) {           /* Spheroidal, inverse */
    LP lp = {0.0,0.0};
    (void) P;
    lp.phi = 3. * asin(xy.y * RYM);
    lp.lam = xy.x * RXM / (2. * cos((lp.phi + lp.phi) * THIRD) - 1);
    return lp;
}


PJ *PROJECTION(crast) {
    P->es = 0.0;
    P->inv = s_inverse;
    P->fwd = s_forward;

    return P;
}
