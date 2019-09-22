#define PJ_LIB__

#include <errno.h>
#include <math.h>

#include "proj.h"
#include "proj_internal.h"

namespace { // anonymous namespace
struct pj_opaque {
    double  A, B;
};
} // anonymous namespace

PROJ_HEAD(putp5, "Putnins P5") "\n\tPCyl, Sph";
PROJ_HEAD(putp5p, "Putnins P5'") "\n\tPCyl, Sph";

#define C 1.01346
#define D 1.2158542


static PJ_XY putp5_s_forward (PJ_LP lp, PJ *P) {           /* Spheroidal, forward */
    PJ_XY xy = {0.0,0.0};
    struct pj_opaque *Q = static_cast<struct pj_opaque*>(P->opaque);

    xy.x = C * lp.lam * (Q->A - Q->B * sqrt(1. + D * lp.phi * lp.phi));
    xy.y = C * lp.phi;

    return xy;
}


static PJ_LP putp5_s_inverse (PJ_XY xy, PJ *P) {           /* Spheroidal, inverse */
    PJ_LP lp = {0.0,0.0};
    struct pj_opaque *Q = static_cast<struct pj_opaque*>(P->opaque);

    lp.phi = xy.y / C;
    lp.lam = xy.x / (C * (Q->A - Q->B * sqrt(1. + D * lp.phi * lp.phi)));

    return lp;
}



PJ *PROJECTION(putp5) {
    struct pj_opaque *Q = static_cast<struct pj_opaque*>(pj_calloc (1, sizeof (struct pj_opaque)));
    if (nullptr==Q)
        return pj_default_destructor (P, ENOMEM);
    P->opaque = Q;

    Q->A = 2.;
    Q->B = 1.;

    P->es = 0.;
    P->inv = putp5_s_inverse;
    P->fwd = putp5_s_forward;

    return P;
}


PJ *PROJECTION(putp5p) {
    struct pj_opaque *Q = static_cast<struct pj_opaque*>(pj_calloc (1, sizeof (struct pj_opaque)));
    if (nullptr==Q)
        return pj_default_destructor (P, ENOMEM);
    P->opaque = Q;

    Q->A = 1.5;
    Q->B = 0.5;

    P->es = 0.;
    P->inv = putp5_s_inverse;
    P->fwd = putp5_s_forward;

    return P;
}
