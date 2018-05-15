#define PJ_LIB__

#include <errno.h>
#include <math.h>

#include "projects.h"

struct pj_opaque {
    double  A, B;
};

PROJ_HEAD(putp5, "Putnins P5") "\n\tPCyl., Sph.";
PROJ_HEAD(putp5p, "Putnins P5'") "\n\tPCyl., Sph.";

#define C 1.01346
#define D 1.2158542


static XY s_forward (LP lp, PJ *P) {           /* Spheroidal, forward */
    XY xy = {0.0,0.0};
    struct pj_opaque *Q = P->opaque;

    xy.x = C * lp.lam * (Q->A - Q->B * sqrt(1. + D * lp.phi * lp.phi));
    xy.y = C * lp.phi;

    return xy;
}


static LP s_inverse (XY xy, PJ *P) {           /* Spheroidal, inverse */
    LP lp = {0.0,0.0};
    struct pj_opaque *Q = P->opaque;

    lp.phi = xy.y / C;
    lp.lam = xy.x / (C * (Q->A - Q->B * sqrt(1. + D * lp.phi * lp.phi)));

    return lp;
}



PJ *PROJECTION(putp5) {
    struct pj_opaque *Q = pj_calloc (1, sizeof (struct pj_opaque));
    if (0==Q)
        return pj_default_destructor (P, ENOMEM);
    P->opaque = Q;

    Q->A = 2.;
    Q->B = 1.;

    P->es = 0.;
    P->inv = s_inverse;
    P->fwd = s_forward;

    return P;
}


PJ *PROJECTION(putp5p) {
    struct pj_opaque *Q = pj_calloc (1, sizeof (struct pj_opaque));
    if (0==Q)
        return pj_default_destructor (P, ENOMEM);
    P->opaque = Q;

    Q->A = 1.5;
    Q->B = 0.5;

    P->es = 0.;
    P->inv = s_inverse;
    P->fwd = s_forward;

    return P;
}
