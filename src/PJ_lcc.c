#define PJ_LIB__
#include <errno.h>
#include "proj.h"
#include "projects.h"
#include "proj_math.h"

PROJ_HEAD(lcc, "Lambert Conformal Conic")
    "\n\tConic, Sph&Ell\n\tlat_1= and lat_2= or lat_0";

# define EPS10  1.e-10

struct pj_opaque {
    double phi1;
    double phi2;
    double n;
    double rho0;
    double c;
    int    ellips;
};


static XY e_forward (LP lp, PJ *P) {          /* Ellipsoidal, forward */
    XY xy = {0.0,0.0};
    struct pj_opaque *Q = P->opaque;
    double rho;

    if (fabs(fabs(lp.phi) - M_HALFPI) < EPS10) {
        if ((lp.phi * Q->n) <= 0.) {
            proj_errno_set(P, PJD_ERR_TOLERANCE_CONDITION);
            return xy;
        }
        rho = 0.;
    } else {
        rho = Q->c * (Q->ellips ? pow(pj_tsfn(lp.phi, sin(lp.phi),
            P->e), Q->n) : pow(tan(M_FORTPI + .5 * lp.phi), -Q->n));
    }
    lp.lam *= Q->n;
    xy.x = P->k0 * (rho * sin( lp.lam) );
    xy.y = P->k0 * (Q->rho0 - rho * cos(lp.lam) );
    return xy;
}


static LP e_inverse (XY xy, PJ *P) {          /* Ellipsoidal, inverse */
    LP lp = {0.0,0.0};
    struct pj_opaque *Q = P->opaque;
    double rho;

    xy.x /= P->k0;
    xy.y /= P->k0;

    xy.y = Q->rho0 - xy.y;
    rho = hypot(xy.x, xy.y);
    if (rho != 0.0) {
        if (Q->n < 0.) {
            rho = -rho;
            xy.x = -xy.x;
            xy.y = -xy.y;
        }
        if (Q->ellips) {
            lp.phi = pj_phi2(P->ctx, pow(rho / Q->c, 1./Q->n), P->e);
            if (lp.phi == HUGE_VAL) {
                proj_errno_set(P, PJD_ERR_TOLERANCE_CONDITION);
                return lp;
            }

        } else
            lp.phi = 2. * atan(pow(Q->c / rho, 1./Q->n)) - M_HALFPI;
        lp.lam = atan2(xy.x, xy.y) / Q->n;
    } else {
        lp.lam = 0.;
        lp.phi = Q->n > 0. ? M_HALFPI : -M_HALFPI;
    }
    return lp;
}


PJ *PROJECTION(lcc) {
    double cosphi, sinphi;
    int secant;
    struct pj_opaque *Q = pj_calloc (1, sizeof (struct pj_opaque));

    if (0==Q)
        return pj_default_destructor (P, ENOMEM);
    P->opaque = Q;


    Q->phi1 = pj_param(P->ctx, P->params, "rlat_1").f;
    if (pj_param(P->ctx, P->params, "tlat_2").i)
        Q->phi2 = pj_param(P->ctx, P->params, "rlat_2").f;
    else {
        Q->phi2 = Q->phi1;
        if (!pj_param(P->ctx, P->params, "tlat_0").i)
            P->phi0 = Q->phi1;
    }
    if (fabs(Q->phi1 + Q->phi2) < EPS10)
        return pj_default_destructor(P, PJD_ERR_CONIC_LAT_EQUAL);

    Q->n = sinphi = sin(Q->phi1);
    cosphi = cos(Q->phi1);
    secant = fabs(Q->phi1 - Q->phi2) >= EPS10;
    if( (Q->ellips = (P->es != 0.)) ) {
        double ml1, m1;

        P->e = sqrt(P->es);
        m1 = pj_msfn(sinphi, cosphi, P->es);
        ml1 = pj_tsfn(Q->phi1, sinphi, P->e);
        if (secant) { /* secant cone */
            sinphi = sin(Q->phi2);
            Q->n = log(m1 / pj_msfn(sinphi, cos(Q->phi2), P->es));
            Q->n /= log(ml1 / pj_tsfn(Q->phi2, sinphi, P->e));
        }
        Q->c = (Q->rho0 = m1 * pow(ml1, -Q->n) / Q->n);
        Q->rho0 *= (fabs(fabs(P->phi0) - M_HALFPI) < EPS10) ? 0. :
            pow(pj_tsfn(P->phi0, sin(P->phi0), P->e), Q->n);
    } else {
        if (secant)
            Q->n = log(cosphi / cos(Q->phi2)) /
               log(tan(M_FORTPI + .5 * Q->phi2) /
               tan(M_FORTPI + .5 * Q->phi1));
        Q->c = cosphi * pow(tan(M_FORTPI + .5 * Q->phi1), Q->n) / Q->n;
        Q->rho0 = (fabs(fabs(P->phi0) - M_HALFPI) < EPS10) ? 0. :
            Q->c * pow(tan(M_FORTPI + .5 * P->phi0), -Q->n);
    }

    P->inv = e_inverse;
    P->fwd = e_forward;

    return P;
}

