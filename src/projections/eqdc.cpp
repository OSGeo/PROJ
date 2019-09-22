#define PJ_LIB__

#include <errno.h>
#include <math.h>

#include "proj.h"
#include "proj_internal.h"
#include <math.h>

namespace { // anonymous namespace
struct pj_opaque {
    double phi1;
    double phi2;
    double n;
    double rho;
    double rho0;
    double c;
    double *en;
    int     ellips;
};
} // anonymous namespace

PROJ_HEAD(eqdc, "Equidistant Conic")
    "\n\tConic, Sph&Ell\n\tlat_1= lat_2=";
# define EPS10  1.e-10


static PJ_XY eqdc_e_forward (PJ_LP lp, PJ *P) {          /* Ellipsoidal, forward */
    PJ_XY xy = {0.0,0.0};
    struct pj_opaque *Q = static_cast<struct pj_opaque*>(P->opaque);

    Q->rho = Q->c - (Q->ellips ? pj_mlfn(lp.phi, sin(lp.phi),
        cos(lp.phi), Q->en) : lp.phi);
    xy.x = Q->rho * sin( lp.lam *= Q->n );
    xy.y = Q->rho0 - Q->rho * cos(lp.lam);

    return xy;
}


static PJ_LP eqdc_e_inverse (PJ_XY xy, PJ *P) {          /* Ellipsoidal, inverse */
    PJ_LP lp = {0.0,0.0};
    struct pj_opaque *Q = static_cast<struct pj_opaque*>(P->opaque);

    if ((Q->rho = hypot(xy.x, xy.y = Q->rho0 - xy.y)) != 0.0 ) {
        if (Q->n < 0.) {
            Q->rho = -Q->rho;
            xy.x = -xy.x;
            xy.y = -xy.y;
        }
        lp.phi = Q->c - Q->rho;
        if (Q->ellips)
            lp.phi = pj_inv_mlfn(P->ctx, lp.phi, P->es, Q->en);
        lp.lam = atan2(xy.x, xy.y) / Q->n;
    } else {
        lp.lam = 0.;
        lp.phi = Q->n > 0. ? M_HALFPI : -M_HALFPI;
    }
    return lp;
}


static PJ *destructor (PJ *P, int errlev) {                        /* Destructor */
    if (nullptr==P)
        return nullptr;

    if (nullptr==P->opaque)
        return pj_default_destructor (P, errlev);

    pj_dealloc (static_cast<struct pj_opaque*>(P->opaque)->en);
    return pj_default_destructor (P, errlev);
}


PJ *PROJECTION(eqdc) {
    double cosphi, sinphi;
    int secant;

    struct pj_opaque *Q = static_cast<struct pj_opaque*>(pj_calloc (1, sizeof (struct pj_opaque)));
    if (nullptr==Q)
        return pj_default_destructor (P, ENOMEM);
    P->opaque = Q;
    P->destructor = destructor;

    Q->phi1 = pj_param(P->ctx, P->params, "rlat_1").f;
    Q->phi2 = pj_param(P->ctx, P->params, "rlat_2").f;
    if (fabs(Q->phi1) > M_HALFPI || fabs(Q->phi2) > M_HALFPI)
        return destructor(P, PJD_ERR_LAT_LARGER_THAN_90);

    if (fabs(Q->phi1 + Q->phi2) < EPS10)
        return destructor (P, PJD_ERR_CONIC_LAT_EQUAL);

    if (!(Q->en = pj_enfn(P->es)))
        return destructor(P, ENOMEM);

    Q->n = sinphi = sin(Q->phi1);
    cosphi = cos(Q->phi1);
    secant = fabs(Q->phi1 - Q->phi2) >= EPS10;
    if( (Q->ellips = (P->es > 0.)) ) {
        double ml1, m1;

        m1 = pj_msfn(sinphi, cosphi, P->es);
        ml1 = pj_mlfn(Q->phi1, sinphi, cosphi, Q->en);
        if (secant) { /* secant cone */
            sinphi = sin(Q->phi2);
            cosphi = cos(Q->phi2);
            Q->n = (m1 - pj_msfn(sinphi, cosphi, P->es)) /
                (pj_mlfn(Q->phi2, sinphi, cosphi, Q->en) - ml1);
            if (Q->n == 0) {
                // Not quite, but es is very close to 1...
                return destructor(P, PJD_ERR_INVALID_ECCENTRICITY);
            }
        }
        Q->c = ml1 + m1 / Q->n;
        Q->rho0 = Q->c - pj_mlfn(P->phi0, sin(P->phi0),
            cos(P->phi0), Q->en);
    } else {
        if (secant)
            Q->n = (cosphi - cos(Q->phi2)) / (Q->phi2 - Q->phi1);
        if (Q->n == 0)
            return destructor (P, PJD_ERR_CONIC_LAT_EQUAL);
        Q->c = Q->phi1 + cos(Q->phi1) / Q->n;
        Q->rho0 = Q->c - P->phi0;
    }

    P->inv = eqdc_e_inverse;
    P->fwd = eqdc_e_forward;

    return P;
}
