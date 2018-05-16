#define PJ_LIB__

#include <errno.h>
#include <math.h>

#include "proj.h"
#include "projects.h"

PROJ_HEAD(somerc, "Swiss. Obl. Mercator") "\n\tCyl, Ell\n\tFor CH1903";

struct pj_opaque {
    double K, c, hlf_e, kR, cosp0, sinp0;
};

#define EPS 1.e-10
#define NITER 6


static XY e_forward (LP lp, PJ *P) {          /* Ellipsoidal, forward */
    XY xy = {0.0, 0.0};
    double phip, lamp, phipp, lampp, sp, cp;
    struct pj_opaque *Q = P->opaque;

    sp = P->e * sin (lp.phi);
    phip = 2.* atan ( exp ( Q->c * (
        log (tan (M_FORTPI + 0.5 * lp.phi)) - Q->hlf_e * log ((1. + sp)/(1. - sp)))
        + Q->K)) - M_HALFPI;
    lamp = Q->c * lp.lam;
    cp = cos(phip);
    phipp = aasin (P->ctx, Q->cosp0 * sin (phip) - Q->sinp0 * cp * cos (lamp));
    lampp = aasin (P->ctx, cp * sin (lamp) / cos (phipp));
    xy.x = Q->kR * lampp;
    xy.y = Q->kR * log (tan (M_FORTPI + 0.5 * phipp));
    return xy;
}


static LP e_inverse (XY xy, PJ *P) {          /* Ellipsoidal, inverse */
    LP lp = {0.0,0.0};
    struct pj_opaque *Q = P->opaque;
    double phip, lamp, phipp, lampp, cp, esp, con, delp;
    int i;

    phipp = 2. * (atan (exp (xy.y / Q->kR)) - M_FORTPI);
    lampp = xy.x / Q->kR;
    cp = cos (phipp);
    phip = aasin (P->ctx, Q->cosp0 * sin (phipp) + Q->sinp0 * cp * cos (lampp));
    lamp = aasin (P->ctx, cp * sin (lampp) / cos (phip));
    con = (Q->K - log (tan (M_FORTPI + 0.5 * phip)))/Q->c;
    for (i = NITER; i ; --i) {
        esp = P->e * sin(phip);
        delp = (con + log(tan(M_FORTPI + 0.5 * phip)) - Q->hlf_e *
            log((1. + esp)/(1. - esp)) ) *
            (1. - esp * esp) * cos(phip) * P->rone_es;
        phip -= delp;
        if (fabs(delp) < EPS)
            break;
    }
    if (i) {
        lp.phi = phip;
        lp.lam = lamp / Q->c;
    } else {
        proj_errno_set(P, PJD_ERR_TOLERANCE_CONDITION);
        return lp;
    }
    return (lp);
}


PJ *PROJECTION(somerc) {
    double cp, phip0, sp;
    struct pj_opaque *Q = pj_calloc (1, sizeof (struct pj_opaque));
    if (0==Q)
        return pj_default_destructor (P, ENOMEM);
    P->opaque = Q;


    Q->hlf_e = 0.5 * P->e;
    cp = cos (P->phi0);
    cp *= cp;
    Q->c = sqrt (1 + P->es * cp * cp * P->rone_es);
    sp = sin (P->phi0);
    Q->cosp0 = cos( phip0 = aasin (P->ctx, Q->sinp0 = sp / Q->c) );
    sp *= P->e;
    Q->K = log (tan (M_FORTPI + 0.5 * phip0)) - Q->c * (
        log (tan (M_FORTPI + 0.5 * P->phi0)) - Q->hlf_e *
        log ((1. + sp) / (1. - sp)));
    Q->kR = P->k0 * sqrt(P->one_es) / (1. - sp * sp);
    P->inv = e_inverse;
    P->fwd = e_forward;
    return P;
}
