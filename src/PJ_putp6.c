#define PJ_LIB__

#include <errno.h>
#include <math.h>

#include "projects.h"

struct pj_opaque {
    double C_x, C_y, A, B, D;
};

PROJ_HEAD(putp6, "Putnins P6") "\n\tPCyl., Sph.";
PROJ_HEAD(putp6p, "Putnins P6'") "\n\tPCyl., Sph.";

#define EPS      1e-10
#define NITER    10
#define CON_POLE 1.732050807568877


static XY s_forward (LP lp, PJ *P) {           /* Spheroidal, forward */
    XY xy = {0.0,0.0};
    struct pj_opaque *Q = P->opaque;
    double p, r, V;
    int i;

    p = Q->B * sin(lp.phi);
    lp.phi *=  1.10265779;
    for (i = NITER; i ; --i) {
        r = sqrt(1. + lp.phi * lp.phi);
        lp.phi -= V = ( (Q->A - r) * lp.phi - log(lp.phi + r) - p ) /
            (Q->A - 2. * r);
        if (fabs(V) < EPS)
            break;
    }
    if (!i)
        lp.phi = p < 0. ? -CON_POLE : CON_POLE;
    xy.x = Q->C_x * lp.lam * (Q->D - sqrt(1. + lp.phi * lp.phi));
    xy.y = Q->C_y * lp.phi;

    return xy;
}


static LP s_inverse (XY xy, PJ *P) {           /* Spheroidal, inverse */
    LP lp = {0.0,0.0};
    struct pj_opaque *Q = P->opaque;
    double r;

    lp.phi = xy.y / Q->C_y;
    r = sqrt(1. + lp.phi * lp.phi);
    lp.lam = xy.x / (Q->C_x * (Q->D - r));
    lp.phi = aasin( P->ctx, ( (Q->A - r) * lp.phi - log(lp.phi + r) ) / Q->B);

    return lp;
}


PJ *PROJECTION(putp6) {
    struct pj_opaque *Q = pj_calloc (1, sizeof (struct pj_opaque));
    if (0==Q)
        return pj_default_destructor(P, ENOMEM);
    P->opaque = Q;

    Q->C_x = 1.01346;
    Q->C_y = 0.91910;
    Q->A   = 4.;
    Q->B   = 2.1471437182129378784;
    Q->D   = 2.;

    P->es = 0.;
    P->inv = s_inverse;
    P->fwd = s_forward;

    return P;
}


PJ *PROJECTION(putp6p) {
    struct pj_opaque *Q = pj_calloc (1, sizeof (struct pj_opaque));
    if (0==Q)
        return pj_default_destructor(P, ENOMEM);
    P->opaque = Q;

    Q->C_x = 0.44329;
    Q->C_y = 0.80404;
    Q->A   = 6.;
    Q->B   = 5.61125;
    Q->D   = 3.;

    P->es = 0.;
    P->inv = s_inverse;
    P->fwd = s_forward;

    return P;
}
