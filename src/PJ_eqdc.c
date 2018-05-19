#define PJ_LIB__

#include <errno.h>
#include <math.h>

#include "proj.h"
#include "projects.h"
#include "proj_math.h"

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

PROJ_HEAD(eqdc, "Equidistant Conic")
    "\n\tConic, Sph&Ell\n\tlat_1= lat_2=";
# define EPS10  1.e-10


static XY e_forward (LP lp, PJ *P) {          /* Ellipsoidal, forward */
    XY xy = {0.0,0.0};
    struct pj_opaque *Q = P->opaque;

    Q->rho = Q->c - (Q->ellips ? pj_mlfn(lp.phi, sin(lp.phi),
        cos(lp.phi), Q->en) : lp.phi);
    xy.x = Q->rho * sin( lp.lam *= Q->n );
    xy.y = Q->rho0 - Q->rho * cos(lp.lam);

    return xy;
}


static LP e_inverse (XY xy, PJ *P) {          /* Ellipsoidal, inverse */
    LP lp = {0.0,0.0};
    struct pj_opaque *Q = P->opaque;

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


static void *destructor (PJ *P, int errlev) {                        /* Destructor */
    if (0==P)
        return 0;

    if (0==P->opaque)
        return pj_default_destructor (P, errlev);

    pj_dealloc (P->opaque->en);
    return pj_default_destructor (P, errlev);
}


PJ *PROJECTION(eqdc) {
    double cosphi, sinphi;
    int secant;

    struct pj_opaque *Q = pj_calloc (1, sizeof (struct pj_opaque));
    if (0==Q)
        return pj_default_destructor (P, ENOMEM);
    P->opaque = Q;
    P->destructor = destructor;

    Q->phi1 = pj_param(P->ctx, P->params, "rlat_1").f;
    Q->phi2 = pj_param(P->ctx, P->params, "rlat_2").f;

    if (fabs(Q->phi1 + Q->phi2) < EPS10)
        return pj_default_destructor (P, PJD_ERR_CONIC_LAT_EQUAL);

    if (!(Q->en = pj_enfn(P->es)))
        return pj_default_destructor(P, ENOMEM);

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
        }
        Q->c = ml1 + m1 / Q->n;
        Q->rho0 = Q->c - pj_mlfn(P->phi0, sin(P->phi0),
            cos(P->phi0), Q->en);
    } else {
        if (secant)
            Q->n = (cosphi - cos(Q->phi2)) / (Q->phi2 - Q->phi1);
        Q->c = Q->phi1 + cos(Q->phi1) / Q->n;
        Q->rho0 = Q->c - P->phi0;
    }

    P->inv = e_inverse;
    P->fwd = e_forward;

    return P;
}
