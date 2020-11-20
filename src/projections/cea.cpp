#define PJ_LIB__

#include <errno.h>
#include <math.h>

#include "proj.h"
#include "proj_internal.h"

namespace { // anonymous namespace
struct pj_opaque {
    double qp;
    double *apa;
};
} // anonymous namespace

PROJ_HEAD(cea, "Equal Area Cylindrical") "\n\tCyl, Sph&Ell\n\tlat_ts=";
# define EPS    1e-10


static PJ_XY cea_e_forward (PJ_LP lp, PJ *P) {          /* Ellipsoidal, forward */
    PJ_XY xy = {0.0,0.0};
    xy.x = P->k0 * lp.lam;
    xy.y = 0.5 * pj_qsfn (sin (lp.phi), P->e, P->one_es) / P->k0;
    return xy;
}


static PJ_XY cea_s_forward (PJ_LP lp, PJ *P) {           /* Spheroidal, forward */
    PJ_XY xy = {0.0,0.0};
    xy.x = P->k0 * lp.lam;
    xy.y = sin(lp.phi) / P->k0;
    return xy;
}


static PJ_LP cea_e_inverse (PJ_XY xy, PJ *P) {          /* Ellipsoidal, inverse */
    PJ_LP lp = {0.0,0.0};
    lp.phi = pj_authlat(asin( 2. * xy.y * P->k0 / static_cast<struct pj_opaque*>(P->opaque)->qp), static_cast<struct pj_opaque*>(P->opaque)->apa);
    lp.lam = xy.x / P->k0;
    return lp;
}


static PJ_LP cea_s_inverse (PJ_XY xy, PJ *P) {           /* Spheroidal, inverse */
    PJ_LP lp = {0.0,0.0};

    xy.y *= P->k0;
    const double t = fabs(xy.y);
    if (t - EPS <= 1.) {
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

static PJ *destructor (PJ *P, int errlev) {                        /* Destructor */
    if (nullptr==P)
        return nullptr;

    if (nullptr==P->opaque)
        return pj_default_destructor (P, errlev);

    free (static_cast<struct pj_opaque*>(P->opaque)->apa);
    return pj_default_destructor (P, errlev);
}


PJ *PROJECTION(cea) {
    double t = 0.0;
    struct pj_opaque *Q = static_cast<struct pj_opaque*>(calloc (1, sizeof (struct pj_opaque)));
    if (nullptr==Q)
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
        Q->apa = pj_authset(P->es);
        if (!(Q->apa))
            return pj_default_destructor(P, ENOMEM);

        Q->qp = pj_qsfn(1., P->e, P->one_es);
        P->inv = cea_e_inverse;
        P->fwd = cea_e_forward;
    } else {
        P->inv = cea_s_inverse;
        P->fwd = cea_s_forward;
    }

    return P;
}
