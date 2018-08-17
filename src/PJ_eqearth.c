/*
Equal Earth is a projection inspired by the Robinson projection, but unlike
the Robinson projection retains the relative size of areas. The projection
was designed in 2018 by Bojan Savric, Tom Patterson and Bernhard Jenny.

Publication:
Bojan Savric, Tom Patterson & Bernhard Jenny (2018). The Equal Earth map
projection, International Journal of Geographical Information Science,
DOI: 10.1080/13658816.2018.1504949

Port to PROJ by Juernjakob Dugge, 16 August 2018
*/
#define PJ_LIB__

#include <math.h>

#include "projects.h"

PROJ_HEAD(eqearth, "Equal Earth") "\n\tPCyl., Sph.";

#define A1 1.340264
#define A2 -0.081106
#define A3 0.000893
#define A4 0.003796
#define M (sqrt(3) / 2.0)

#define EPS 1e-11
#define MAX_ITER 12

static XY s_forward (LP lp, PJ *P) {           /* Spheroidal, forward */
    XY xy = {0.0,0.0};
    double phi, phi2, phi6;
    (void) P;

    phi = asin(M * sin(lp.phi));
    phi2 = phi * phi;
    phi6 = phi2 * phi2 * phi2;

    xy.x = lp.lam * cos(phi) / (M * (A1 + 3 * A2 * phi2 + phi6 * (7 * A3 + 9 * A4 * phi2)));
    xy.y = phi * (A1 + A2 * phi2 + phi6 * (A3 + A4 * phi2));
    return xy;
}


static LP s_inverse (XY xy, PJ *P) {           /* Spheroidal, inverse */
    LP lp = {0.0,0.0};
    double yc, tol, y2, y6, f, fder;
    int i;
    (void) P;


    yc = xy.y;
    for (i = MAX_ITER; i ; --i) { /* Newton-Raphson */
        y2 = yc * yc;
        y6 = y2 * y2 * y2;
        f = yc * (A1 + A2 * y2 + y6 * (A3 + A4 * y2)) - xy.y;
        fder = A1 + 3 * A2 * y2 + y6 * (7 * A3 + 9 * A4 * y2);
        tol = f / fder;
        yc -= tol;
        if (fabs(tol) < EPS) {
            break;
        }
    }
    if( i == 0 )
        pj_ctx_set_errno( P->ctx, PJD_ERR_NON_CONVERGENT );

    y2 = yc * yc;
    y6 = y2 * y2 * y2;

    lp.lam = M * xy.x * (A1 + 3 * A2 * y2 + y6 * (7 * A3 + 9 * A4 * y2)) / cos(yc);
    lp.phi = asin(sin(yc) / M);
    
    return lp;
}


PJ *PROJECTION(eqearth) {
    P->es = 0;
    P->inv = s_inverse;
    P->fwd = s_forward;

    return P;
}
