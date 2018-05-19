#define PJ_LIB__
#include <math.h>

#include "projects.h"

PROJ_HEAD(eck1, "Eckert I") "\n\tPCyl., Sph.";
#define FC  0.92131773192356127802
#define RP  0.31830988618379067154


static XY s_forward (LP lp, PJ *P) {           /* Spheroidal, forward */
    XY xy = {0.0,0.0};
    (void) P;

    xy.x = FC * lp.lam * (1. - RP * fabs(lp.phi));
    xy.y = FC * lp.phi;

    return xy;
}


static LP s_inverse (XY xy, PJ *P) {           /* Spheroidal, inverse */
    LP lp = {0.0,0.0};
    (void) P;

    lp.phi = xy.y / FC;
    lp.lam = xy.x / (FC * (1. - RP * fabs(lp.phi)));

    return (lp);
}



PJ *PROJECTION(eck1) {
    P->es = 0.0;
    P->inv = s_inverse;
    P->fwd = s_forward;

    return P
;
}
