#define PJ_LIB__

#include <errno.h>
#include <math.h>

#include "proj.h"
#include "projects.h"

struct pj_opaque {
    double qp;
    double *apa;
};

PROJ_HEAD(cea, "Equal Area Cylindrical") "\n\tCyl, Sph&Ell\n\tlat_ts=";
# define EPS    1e-10


static XY e_forward (LP lp, PJ *P) {          /* Ellipsoidal, forward */
    XY xy = {0.0,0.0};
    xy.x = P->k0 * lp.lam;
    xy.y = 0.5 * pj_qsfn (sin (lp.phi), P->e, P->one_es) / P->k0;
    return xy;
}


static XY s_forward (LP lp, PJ *P) {           /* Spheroidal, forward */
    XY xy = {0.0,0.0};
    xy.x = P->k0 * lp.lam;
    xy.y = sin(lp.phi) / P->k0;
    return xy;
}


static LP e_inverse (XY xy, PJ *P) {          /* Ellipsoidal, inverse */
    LP lp = {0.0,0.0};
    lp.phi = pj_authlat(asin( 2. * xy.y * P->k0 / P->opaque->qp), P->opaque->apa);
    lp.lam = xy.x / P->k0;
    return lp;
}


static LP s_inverse (XY xy, PJ *P) {           /* Spheroidal, inverse */
    LP lp = {0.0,0.0};
    double t;

    if ((t = fabs(xy.y *= P->k0)) - EPS <= 1.) {
        if (t >= 1.)
            lp.phi = xy.y < 0. ? -M_HALFPI : M_HALFPI;
        else
            lp.phi = asin(xy.y);
        lp.lam = xy.x / P->k0;
    } else {
        proj_errno_set(P, PJD_ERR_TOLERANCE_CONDITION);
        return lp;
    }
    return (lp);
}

static void *destructor (PJ *P, int errlev) {                        /* Destructor */
    if (0==P)
        return 0;

    if (0==P->opaque)
        return pj_default_destructor (P, errlev);

    pj_dealloc (P->opaque->apa);
    return pj_default_destructor (P, errlev);
}


PJ *PROJECTION(cea) {
    double t = 0.0;
    struct pj_opaque *Q = pj_calloc (1, sizeof (struct pj_opaque));
    if (0==Q)
        return pj_default_destructor (P, ENOMEM);
    P->opaque = Q;
    P->destructor = destructor;


    if (pj_param(P->ctx, P->params, "tlat_ts").i) {
        P->k0 = cos(t = pj_param(P->ctx, P->params, "rlat_ts").f);
        if (P->k0 < 0.)
            return pj_default_destructor (P, PJD_ERR_LAT_TS_LARGER_THAN_90);
    }
    if (P->es != 0.0) {
        t = sin(t);
        P->k0 /= sqrt(1. - P->es * t * t);
        P->e = sqrt(P->es);
        if (!(Q->apa = pj_authset(P->es)))
            return pj_default_destructor(P, ENOMEM);

        Q->qp = pj_qsfn(1., P->e, P->one_es);
        P->inv = e_inverse;
        P->fwd = e_forward;
    } else {
        P->inv = s_inverse;
        P->fwd = s_forward;
    }

    return P;
}
