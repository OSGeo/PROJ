#define PJ_LIB__

#include <math.h>

#include "projects.h"

PROJ_HEAD(eck5, "Eckert V") "\n\tPCyl, Sph.";

#define XF  0.44101277172455148219
#define RXF 2.26750802723822639137
#define YF  0.88202554344910296438
#define RYF 1.13375401361911319568


static XY s_forward (LP lp, PJ *P) {           /* Spheroidal, forward */
    XY xy = {0.0,0.0};
    (void) P;
    xy.x = XF * (1. + cos(lp.phi)) * lp.lam;
    xy.y = YF * lp.phi;

    return xy;
}


static LP s_inverse (XY xy, PJ *P) {           /* Spheroidal, inverse */
    LP lp = {0.0,0.0};
    (void) P;
    lp.lam = RXF * xy.x / (1. + cos( lp.phi = RYF * xy.y));

    return lp;
}


PJ *PROJECTION(eck5) {
    P->es = 0.0;
    P->inv = s_inverse;
    P->fwd = s_forward;

    return P;
}
