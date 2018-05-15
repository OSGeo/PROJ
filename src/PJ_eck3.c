#define PJ_LIB__

#include <errno.h>
#include <math.h>

#include "projects.h"

PROJ_HEAD(eck3, "Eckert III") "\n\tPCyl, Sph.";
PROJ_HEAD(putp1, "Putnins P1") "\n\tPCyl, Sph.";
PROJ_HEAD(wag6, "Wagner VI") "\n\tPCyl, Sph.";
PROJ_HEAD(kav7, "Kavraisky VII") "\n\tPCyl, Sph.";

struct pj_opaque {
    double C_x, C_y, A, B;
};


static XY s_forward (LP lp, PJ *P) {           /* Spheroidal, forward */
    XY xy = {0.0,0.0};
    struct pj_opaque *Q = P->opaque;

    xy.y = Q->C_y * lp.phi;
    xy.x = Q->C_x * lp.lam * (Q->A + asqrt(1. - Q->B * lp.phi * lp.phi));
    return xy;
}


static LP s_inverse (XY xy, PJ *P) {           /* Spheroidal, inverse */
    LP lp = {0.0,0.0};
    struct pj_opaque *Q = P->opaque;
    double denominator;

    lp.phi = xy.y / Q->C_y;
    denominator = (Q->C_x * (Q->A + asqrt(1. - Q->B * lp.phi * lp.phi)));
    if ( denominator == 0.0)
        lp.lam = HUGE_VAL;
    else
        lp.lam = xy.x / denominator;
    return lp;
}


static PJ *setup(PJ *P) {
    P->es = 0.;
    P->inv = s_inverse;
    P->fwd = s_forward;
    return P;
}


PJ *PROJECTION(eck3) {
    struct pj_opaque *Q = pj_calloc (1, sizeof (struct pj_opaque));
    if (0==Q)
        return pj_default_destructor (P, ENOMEM);
    P->opaque = Q;

    Q->C_x = 0.42223820031577120149;
    Q->C_y = 0.84447640063154240298;
    Q->A = 1.0;
    Q->B = 0.4052847345693510857755;

    return setup(P);
}


PJ *PROJECTION(kav7) {
    struct pj_opaque *Q = pj_calloc (1, sizeof (struct pj_opaque));
    if (0==Q)
        return pj_default_destructor (P, ENOMEM);
    P->opaque = Q;

    /* Defined twice in original code - Using 0.866...,
     * but leaving the other one here as a safety measure.
     * Q->C_x = 0.2632401569273184856851; */
    Q->C_x = 0.8660254037844;
    Q->C_y = 1.;
    Q->A = 0.;
    Q->B = 0.30396355092701331433;

    return setup(P);
}


PJ *PROJECTION(wag6) {
    struct pj_opaque *Q = pj_calloc (1, sizeof (struct pj_opaque));
    if (0==Q)
        return pj_default_destructor (P, ENOMEM);
    P->opaque = Q;

    Q->C_x = Q->C_y = 0.94745;
    Q->A = 0.0;
    Q->B = 0.30396355092701331433;

    return setup(P);
}


PJ *PROJECTION(putp1) {
    struct pj_opaque *Q = pj_calloc (1, sizeof (struct pj_opaque));
    if (0==Q)
        return pj_default_destructor (P, ENOMEM);
    P->opaque = Q;

    Q->C_x = 1.89490;
    Q->C_y = 0.94745;
    Q->A = -0.5;
    Q->B = 0.30396355092701331433;

    return setup(P);
}
