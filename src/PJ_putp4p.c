#define PJ_LIB__

#include <errno.h>
#include <math.h>

#include "projects.h"

struct pj_opaque {
    double C_x, C_y;
};

PROJ_HEAD(putp4p, "Putnins P4'") "\n\tPCyl., Sph.";
PROJ_HEAD(weren, "Werenskiold I") "\n\tPCyl., Sph.";


static XY s_forward (LP lp, PJ *P) {           /* Spheroidal, forward */
    XY xy = {0.0,0.0};
    struct pj_opaque *Q = P->opaque;

    lp.phi = aasin(P->ctx,0.883883476 * sin(lp.phi));
    xy.x = Q->C_x * lp.lam * cos(lp.phi);
    xy.x /= cos(lp.phi *= 0.333333333333333);
    xy.y = Q->C_y * sin(lp.phi);

    return xy;
}


static LP s_inverse (XY xy, PJ *P) {           /* Spheroidal, inverse */
    LP lp = {0.0,0.0};
    struct pj_opaque *Q = P->opaque;

    lp.phi = aasin(P->ctx,xy.y / Q->C_y);
    lp.lam = xy.x * cos(lp.phi) / Q->C_x;
    lp.phi *= 3.;
    lp.lam /= cos(lp.phi);
    lp.phi = aasin(P->ctx,1.13137085 * sin(lp.phi));

    return lp;
}


PJ *PROJECTION(putp4p) {
    struct pj_opaque *Q = pj_calloc (1, sizeof (struct pj_opaque));
    if (0==Q)
        return pj_default_destructor (P, ENOMEM);
    P->opaque = Q;

    Q->C_x = 0.874038744;
    Q->C_y = 3.883251825;

    P->es = 0.;
    P->inv = s_inverse;
    P->fwd = s_forward;

    return P;
}


PJ *PROJECTION(weren) {
    struct pj_opaque *Q = pj_calloc (1, sizeof (struct pj_opaque));
    if (0==Q)
        return pj_default_destructor (P, ENOMEM);
    P->opaque = Q;

    Q->C_x = 1.;
    Q->C_y = 4.442882938;

    P->es = 0.;
    P->inv = s_inverse;
    P->fwd = s_forward;

    return P;
}
