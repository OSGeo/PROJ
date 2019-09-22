#define PJ_LIB__
#include <errno.h>

#include "proj.h"
#include "proj_internal.h"

namespace { // anonymous namespace
struct pj_opaque {
    double  A;
};
} // anonymous namespace

PROJ_HEAD(putp3, "Putnins P3") "\n\tPCyl, Sph";
PROJ_HEAD(putp3p, "Putnins P3'") "\n\tPCyl, Sph";

#define C       0.79788456
#define RPISQ   0.1013211836


static PJ_XY putp3_s_forward (PJ_LP lp, PJ *P) {           /* Spheroidal, forward */
    PJ_XY xy = {0.0,0.0};

    xy.x = C * lp.lam * (1. - static_cast<struct pj_opaque*>(P->opaque)->A * lp.phi * lp.phi);
    xy.y = C * lp.phi;

    return xy;
}


static PJ_LP putp3_s_inverse (PJ_XY xy, PJ *P) {           /* Spheroidal, inverse */
    PJ_LP lp = {0.0,0.0};

    lp.phi = xy.y / C;
    lp.lam = xy.x / (C * (1. - static_cast<struct pj_opaque*>(P->opaque)->A * lp.phi * lp.phi));

    return lp;
}


PJ *PROJECTION(putp3) {
    struct pj_opaque *Q = static_cast<struct pj_opaque*>(pj_calloc (1, sizeof (struct pj_opaque)));
    if (nullptr==Q)
        return pj_default_destructor (P, ENOMEM);
    P->opaque = Q;

    Q->A = 4. * RPISQ;

    P->es = 0.;
    P->inv = putp3_s_inverse;
    P->fwd = putp3_s_forward;

    return P;
}

PJ *PROJECTION(putp3p) {
    struct pj_opaque *Q = static_cast<struct pj_opaque*>(pj_calloc (1, sizeof (struct pj_opaque)));
    if (nullptr==Q)
        return pj_default_destructor (P, ENOMEM);
    P->opaque = Q;

    Q->A = 2. * RPISQ;

    P->es = 0.;
    P->inv = putp3_s_inverse;
    P->fwd = putp3_s_forward;

    return P;
}

