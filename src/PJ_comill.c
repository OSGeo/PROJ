/*
The Compact Miller projection was designed by Tom Patterson, US National
Park Service, in 2014. The polynomial equation was developed by Bojan
Savric and Bernhard Jenny, College of Earth, Ocean, and Atmospheric
Sciences, Oregon State University.
Port to PROJ.4 by Bojan Savric, 4 April 2016
*/

#define PJ_LIB__

#include <math.h>

#include "projects.h"

PROJ_HEAD(comill, "Compact Miller") "\n\tCyl., Sph.";

#define K1 0.9902
#define K2 0.1604
#define K3 -0.03054
#define C1 K1
#define C2 (3 * K2)
#define C3 (5 * K3)
#define EPS 1e-11
#define MAX_Y (0.6000207669862655 * M_PI)
/* Not sure at all of the appropriate number for MAX_ITER... */
#define MAX_ITER 100

static XY s_forward (LP lp, PJ *P) {           /* Spheroidal, forward */
    XY xy = {0.0,0.0};
    double lat_sq;

    (void) P;   /* silence unused parameter warnings */

    lat_sq = lp.phi * lp.phi;
    xy.x = lp.lam;
    xy.y = lp.phi * (K1 + lat_sq * (K2 + K3 * lat_sq));
    return xy;
}


static LP s_inverse (XY xy, PJ *P) {           /* Spheroidal, inverse */
    LP lp = {0.0,0.0};
    double yc, tol, y2, f, fder;
    int i;

    (void) P;   /* silence unused parameter warnings */

    /* make sure y is inside valid range */
    if (xy.y > MAX_Y) {
        xy.y = MAX_Y;
    } else if (xy.y < -MAX_Y) {
        xy.y = -MAX_Y;
    }

    /* latitude */
    yc = xy.y;
    for (i = MAX_ITER; i ; --i) { /* Newton-Raphson */
        y2 = yc * yc;
        f = (yc * (K1 + y2 * (K2 + K3 * y2))) - xy.y;
        fder = C1 + y2 * (C2 + C3 * y2);
        yc -= tol = f / fder;
        if (fabs(tol) < EPS) {
            break;
        }
    }
    if( i == 0 )
        pj_ctx_set_errno( P->ctx, PJD_ERR_NON_CONVERGENT );
    lp.phi = yc;

    /* longitude */
    lp.lam = xy.x;

    return lp;
}


PJ *PROJECTION(comill) {
    P->es = 0;

    P->inv = s_inverse;
    P->fwd = s_forward;

    return P;
}
