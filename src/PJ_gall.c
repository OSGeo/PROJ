#define PJ_LIB__

#include <math.h>

#include "projects.h"

PROJ_HEAD(gall, "Gall (Gall Stereographic)") "\n\tCyl, Sph";

#define YF  1.70710678118654752440
#define XF  0.70710678118654752440
#define RYF 0.58578643762690495119
#define RXF 1.41421356237309504880


static XY s_forward (LP lp, PJ *P) {           /* Spheroidal, forward */
    XY xy = {0.0,0.0};
    (void) P;

    xy.x = XF * lp.lam;
    xy.y = YF * tan(.5 * lp.phi);

    return xy;
}


static LP s_inverse (XY xy, PJ *P) {           /* Spheroidal, inverse */
    LP lp = {0.0,0.0};
    (void) P;

    lp.lam = RXF * xy.x;
    lp.phi = 2. * atan(xy.y * RYF);

    return lp;
}


PJ *PROJECTION(gall) {
    P->es = 0.0;

    P->inv = s_inverse;
    P->fwd = s_forward;

    return P;
}
