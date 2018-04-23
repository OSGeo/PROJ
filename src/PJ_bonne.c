#define PJ_LIB__
#include <errno.h>
#include "proj.h"
#include "projects.h"
#include "proj_math.h"


PROJ_HEAD(bonne, "Bonne (Werner lat_1=90)")
    "\n\tConic Sph&Ell\n\tlat_1=";
#define EPS10   1e-10

struct pj_opaque {
    double phi1;
    double cphi1;
    double am1;
    double m1;
    double *en;
};


static XY e_forward (LP lp, PJ *P) {          /* Ellipsoidal, forward */
    XY xy = {0.0,0.0};
    struct pj_opaque *Q = P->opaque;
    double rh, E, c;

    rh = Q->am1 + Q->m1 - pj_mlfn(lp.phi, E = sin(lp.phi), c = cos(lp.phi), Q->en);
    E = c * lp.lam / (rh * sqrt(1. - P->es * E * E));
    xy.x = rh * sin(E);
    xy.y = Q->am1 - rh * cos(E);
    return xy;
}


static XY s_forward (LP lp, PJ *P) {           /* Spheroidal, forward */
    XY xy = {0.0,0.0};
    struct pj_opaque *Q = P->opaque;
    double E, rh;

    rh = Q->cphi1 + Q->phi1 - lp.phi;
    if (fabs(rh) > EPS10) {
        xy.x = rh * sin(E = lp.lam * cos(lp.phi) / rh);
        xy.y = Q->cphi1 - rh * cos(E);
    } else
        xy.x = xy.y = 0.;
    return xy;
}


static LP s_inverse (XY xy, PJ *P) {           /* Spheroidal, inverse */
    LP lp = {0.0,0.0};
    struct pj_opaque *Q = P->opaque;
    double rh;

    rh = hypot(xy.x, xy.y = Q->cphi1 - xy.y);
    lp.phi = Q->cphi1 + Q->phi1 - rh;
    if (fabs(lp.phi) > M_HALFPI) {
        proj_errno_set(P, PJD_ERR_TOLERANCE_CONDITION);
        return lp;
    }
    if (fabs(fabs(lp.phi) - M_HALFPI) <= EPS10)
        lp.lam = 0.;
    else
        lp.lam = rh * atan2(xy.x, xy.y) / cos(lp.phi);
    return lp;
}


static LP e_inverse (XY xy, PJ *P) {          /* Ellipsoidal, inverse */
    LP lp = {0.0,0.0};
    struct pj_opaque *Q = P->opaque;
    double s, rh;

    rh = hypot(xy.x, xy.y = Q->am1 - xy.y);
    lp.phi = pj_inv_mlfn(P->ctx, Q->am1 + Q->m1 - rh, P->es, Q->en);
    if ((s = fabs(lp.phi)) < M_HALFPI) {
        s = sin(lp.phi);
        lp.lam = rh * atan2(xy.x, xy.y) *
           sqrt(1. - P->es * s * s) / cos(lp.phi);
    } else if (fabs(s - M_HALFPI) <= EPS10)
        lp.lam = 0.;
    else {
        proj_errno_set(P, PJD_ERR_TOLERANCE_CONDITION);
        return lp;
    }
    return lp;
}



static void *destructor (PJ *P, int errlev) {                        /* Destructor */
    if (0==P)
        return 0;

    if (0==P->opaque)
        return pj_default_destructor (P, errlev);

    pj_dealloc (P->opaque->en);
    return pj_default_destructor (P, errlev);
}


PJ *PROJECTION(bonne) {
    double c;
    struct pj_opaque *Q = pj_calloc (1, sizeof (struct pj_opaque));
    if (0==Q)
        return pj_default_destructor (P, ENOMEM);
    P->opaque = Q;
    P->destructor = destructor;

    Q->phi1 = pj_param(P->ctx, P->params, "rlat_1").f;
    if (fabs(Q->phi1) < EPS10)
        return destructor (P, PJD_ERR_LAT1_IS_ZERO);

    if (P->es != 0.0) {
        Q->en = pj_enfn(P->es);
        if (0==Q->en)
            return destructor(P, ENOMEM);
        Q->m1 = pj_mlfn(Q->phi1, Q->am1 = sin(Q->phi1),
            c = cos(Q->phi1), Q->en);
        Q->am1 = c / (sqrt(1. - P->es * Q->am1 * Q->am1) * Q->am1);
        P->inv = e_inverse;
        P->fwd = e_forward;
    } else {
        if (fabs(Q->phi1) + EPS10 >= M_HALFPI)
            Q->cphi1 = 0.;
        else
            Q->cphi1 = 1. / tan(Q->phi1);
        P->inv = s_inverse;
        P->fwd = s_forward;
    }
    return P;
}


