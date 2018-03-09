#define PJ_LIB__
#include <errno.h>
#include "projects.h"

struct pj_opaque {
    double  A;
};

PROJ_HEAD(putp3, "Putnins P3") "\n\tPCyl., Sph.";
PROJ_HEAD(putp3p, "Putnins P3'") "\n\tPCyl., Sph.";

#define C       0.79788456
#define RPISQ   0.1013211836


static XY s_forward (LP lp, PJ *P) {           /* Spheroidal, forward */
    XY xy = {0.0,0.0};

    xy.x = C * lp.lam * (1. - P->opaque->A * lp.phi * lp.phi);
    xy.y = C * lp.phi;

    return xy;
}


static LP s_inverse (XY xy, PJ *P) {           /* Spheroidal, inverse */
    LP lp = {0.0,0.0};

    lp.phi = xy.y / C;
    lp.lam = xy.x / (C * (1. - P->opaque->A * lp.phi * lp.phi));

    return lp;
}


PJ *PROJECTION(putp3) {
    struct pj_opaque *Q = pj_calloc (1, sizeof (struct pj_opaque));
    if (0==Q)
        return pj_default_destructor (P, ENOMEM);
    P->opaque = Q;

    Q->A = 4. * RPISQ;

    P->es = 0.;
    P->inv = s_inverse;
    P->fwd = s_forward;

    return P;
}

PJ *PROJECTION(putp3p) {
    struct pj_opaque *Q = pj_calloc (1, sizeof (struct pj_opaque));
    if (0==Q)
        return pj_default_destructor (P, ENOMEM);
    P->opaque = Q;

    Q->A = 2. * RPISQ;

    P->es = 0.;
    P->inv = s_inverse;
    P->fwd = s_forward;

    return P;
}

