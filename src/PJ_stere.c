#define PJ_LIB__
#include <errno.h>
#include "proj.h"
#include "projects.h"
#include "proj_math.h"

PROJ_HEAD(stere, "Stereographic") "\n\tAzi, Sph&Ell\n\tlat_ts=";
PROJ_HEAD(ups, "Universal Polar Stereographic") "\n\tAzi, Sph&Ell\n\tsouth";


enum Mode {
    S_POLE = 0,
    N_POLE = 1,
    OBLIQ  = 2,
    EQUIT  = 3
};

struct pj_opaque {
    double phits;
    double sinX1;
    double cosX1;
    double akm1;
    enum Mode mode;
};

#define sinph0  P->opaque->sinX1
#define cosph0  P->opaque->cosX1
#define EPS10   1.e-10
#define TOL 1.e-8
#define NITER   8
#define CONV    1.e-10

static double ssfn_ (double phit, double sinphi, double eccen) {
    sinphi *= eccen;
    return (tan (.5 * (M_HALFPI + phit)) *
       pow ((1. - sinphi) / (1. + sinphi), .5 * eccen));
}


static XY e_forward (LP lp, PJ *P) {          /* Ellipsoidal, forward */
    XY xy = {0.0,0.0};
    struct pj_opaque *Q = P->opaque;
    double coslam, sinlam, sinX = 0.0, cosX = 0.0, X, A = 0.0, sinphi;

    coslam = cos (lp.lam);
    sinlam = sin (lp.lam);
    sinphi = sin (lp.phi);
    if (Q->mode == OBLIQ || Q->mode == EQUIT) {
        sinX = sin (X = 2. * atan(ssfn_(lp.phi, sinphi, P->e)) - M_HALFPI);
        cosX = cos (X);
    }

    switch (Q->mode) {
    case OBLIQ:
        A = Q->akm1 / (Q->cosX1 * (1. + Q->sinX1 * sinX +
           Q->cosX1 * cosX * coslam));
        xy.y = A * (Q->cosX1 * sinX - Q->sinX1 * cosX * coslam);
        goto xmul; /* but why not just  xy.x = A * cosX; break;  ? */

    case EQUIT:
        /* avoid zero division */
        if (1. + cosX * coslam == 0.0) {
            xy.y = HUGE_VAL;
        } else {
            A = Q->akm1 / (1. + cosX * coslam);
            xy.y = A * sinX;
        }
xmul:
        xy.x = A * cosX;
        break;

    case S_POLE:
        lp.phi = -lp.phi;
        coslam = - coslam;
        sinphi = -sinphi;
        /*-fallthrough*/
    case N_POLE:
        xy.x = Q->akm1 * pj_tsfn (lp.phi, sinphi, P->e);
        xy.y = - xy.x * coslam;
        break;
    }

    xy.x = xy.x * sinlam;
    return xy;
}


static XY s_forward (LP lp, PJ *P) {           /* Spheroidal, forward */
    XY xy = {0.0,0.0};
    struct pj_opaque *Q = P->opaque;
    double  sinphi, cosphi, coslam, sinlam;

    sinphi = sin(lp.phi);
    cosphi = cos(lp.phi);
    coslam = cos(lp.lam);
    sinlam = sin(lp.lam);

    switch (Q->mode) {
    case EQUIT:
        xy.y = 1. + cosphi * coslam;
        goto oblcon;
    case OBLIQ:
        xy.y = 1. + sinph0 * sinphi + cosph0 * cosphi * coslam;
oblcon:
        if (xy.y <= EPS10) {
            proj_errno_set(P, PJD_ERR_TOLERANCE_CONDITION);
            return xy;
        }
        xy.x = (xy.y = Q->akm1 / xy.y) * cosphi * sinlam;
        xy.y *= (Q->mode == EQUIT) ? sinphi :
           cosph0 * sinphi - sinph0 * cosphi * coslam;
        break;
    case N_POLE:
        coslam = - coslam;
        lp.phi = - lp.phi;
        /*-fallthrough*/
    case S_POLE:
        if (fabs (lp.phi - M_HALFPI) < TOL) {
            proj_errno_set(P, PJD_ERR_TOLERANCE_CONDITION);
            return xy;
        }
        xy.x = sinlam * ( xy.y = Q->akm1 * tan (M_FORTPI + .5 * lp.phi) );
        xy.y *= coslam;
        break;
    }
    return xy;
}


static LP e_inverse (XY xy, PJ *P) {          /* Ellipsoidal, inverse */
    LP lp = {0.0,0.0};
    struct pj_opaque *Q = P->opaque;
    double cosphi, sinphi, tp=0.0, phi_l=0.0, rho, halfe=0.0, halfpi=0.0;
    int i;

    rho = hypot (xy.x, xy.y);

    switch (Q->mode) {
    case OBLIQ:
    case EQUIT:
        cosphi = cos ( tp = 2. * atan2 (rho * Q->cosX1 , Q->akm1) );
        sinphi = sin (tp);
                if ( rho == 0.0 )
            phi_l = asin (cosphi * Q->sinX1);
                else
            phi_l = asin (cosphi * Q->sinX1 + (xy.y * sinphi * Q->cosX1 / rho));

        tp = tan (.5 * (M_HALFPI + phi_l));
        xy.x *= sinphi;
        xy.y = rho * Q->cosX1 * cosphi - xy.y * Q->sinX1* sinphi;
        halfpi = M_HALFPI;
        halfe = .5 * P->e;
        break;
    case N_POLE:
        xy.y = -xy.y;
        /*-fallthrough*/
    case S_POLE:
        phi_l = M_HALFPI - 2. * atan (tp = - rho / Q->akm1);
        halfpi = -M_HALFPI;
        halfe = -.5 * P->e;
        break;
    }

    for (i = NITER;  i--;  phi_l = lp.phi) {
        sinphi = P->e * sin(phi_l);
        lp.phi = 2. * atan (tp * pow ((1.+sinphi)/(1.-sinphi), halfe)) - halfpi;
        if (fabs (phi_l - lp.phi) < CONV) {
            if (Q->mode == S_POLE)
                lp.phi = -lp.phi;
            lp.lam = (xy.x == 0. && xy.y == 0.) ? 0. : atan2 (xy.x, xy.y);
            return lp;
        }
    }

    proj_errno_set(P, PJD_ERR_TOLERANCE_CONDITION);
    return lp;
}


static LP s_inverse (XY xy, PJ *P) {           /* Spheroidal, inverse */
    LP lp = {0.0,0.0};
    struct pj_opaque *Q = P->opaque;
    double  c, rh, sinc, cosc;

    sinc = sin (c = 2. * atan ((rh = hypot (xy.x, xy.y)) / Q->akm1));
    cosc = cos (c);
    lp.lam = 0.;

    switch (Q->mode) {
    case EQUIT:
        if (fabs (rh) <= EPS10)
            lp.phi = 0.;
        else
            lp.phi = asin (xy.y * sinc / rh);
        if (cosc != 0. || xy.x != 0.)
            lp.lam = atan2 (xy.x * sinc, cosc * rh);
        break;
    case OBLIQ:
        if (fabs (rh) <= EPS10)
            lp.phi = P->phi0;
        else
            lp.phi = asin (cosc * sinph0 + xy.y * sinc * cosph0 / rh);
        if ((c = cosc - sinph0 * sin (lp.phi)) != 0. || xy.x != 0.)
            lp.lam = atan2 (xy.x * sinc * cosph0, c * rh);
        break;
    case N_POLE:
        xy.y = -xy.y;
        /*-fallthrough*/
    case S_POLE:
        if (fabs (rh) <= EPS10)
            lp.phi = P->phi0;
        else
            lp.phi = asin (Q->mode == S_POLE ? - cosc : cosc);
        lp.lam = (xy.x == 0. && xy.y == 0.) ? 0. : atan2 (xy.x, xy.y);
        break;
    }
    return lp;
}


static PJ *setup(PJ *P) {                   /* general initialization */
    double t;
    struct pj_opaque *Q = P->opaque;

    if (fabs ((t = fabs (P->phi0)) - M_HALFPI) < EPS10)
        Q->mode = P->phi0 < 0. ? S_POLE : N_POLE;
    else
        Q->mode = t > EPS10 ? OBLIQ : EQUIT;
    Q->phits = fabs (Q->phits);

    if (P->es != 0.0) {
        double X;

        switch (Q->mode) {
        case N_POLE:
        case S_POLE:
            if (fabs (Q->phits - M_HALFPI) < EPS10)
                Q->akm1 = 2. * P->k0 /
                   sqrt (pow (1+P->e,1+P->e) * pow (1-P->e,1-P->e));
            else {
                Q->akm1 = cos (Q->phits) /
                   pj_tsfn (Q->phits, t = sin (Q->phits), P->e);
                t *= P->e;
                Q->akm1 /= sqrt(1. - t * t);
            }
            break;
        case EQUIT:
        case OBLIQ:
            t = sin (P->phi0);
            X = 2. * atan (ssfn_(P->phi0, t, P->e)) - M_HALFPI;
            t *= P->e;
            Q->akm1 = 2. * P->k0 * cos (P->phi0) / sqrt(1. - t * t);
            Q->sinX1 = sin (X);
            Q->cosX1 = cos (X);
            break;
        }
        P->inv = e_inverse;
        P->fwd = e_forward;
    } else {
        switch (Q->mode) {
        case OBLIQ:
            sinph0 = sin (P->phi0);
            cosph0 = cos (P->phi0);
            /*-fallthrough*/
        case EQUIT:
            Q->akm1 = 2. * P->k0;
            break;
        case S_POLE:
        case N_POLE:
            Q->akm1 = fabs (Q->phits - M_HALFPI) >= EPS10 ?
               cos (Q->phits) / tan (M_FORTPI - .5 * Q->phits) :
               2. * P->k0 ;
            break;
        }

        P->inv = s_inverse;
        P->fwd = s_forward;
    }
    return P;
}


PJ *PROJECTION(stere) {
    struct pj_opaque *Q = pj_calloc (1, sizeof (struct pj_opaque));
    if (0==Q)
        return pj_default_destructor (P, ENOMEM);
    P->opaque = Q;

    Q->phits = pj_param (P->ctx, P->params, "tlat_ts").i ?
               pj_param (P->ctx, P->params, "rlat_ts").f : M_HALFPI;

    return setup(P);
}


PJ *PROJECTION(ups) {
    struct pj_opaque *Q = pj_calloc (1, sizeof (struct pj_opaque));
    if (0==Q)
        return pj_default_destructor (P, ENOMEM);
    P->opaque = Q;

    /* International Ellipsoid */
    P->phi0 = pj_param(P->ctx, P->params, "bsouth").i ? - M_HALFPI: M_HALFPI;
    if (P->es == 0.0) {
        proj_errno_set(P, PJD_ERR_ELLIPSOID_USE_REQUIRED);
        return pj_default_destructor (P, ENOMEM);
    }
    P->k0 = .994;
    P->x0 = 2000000.;
    P->y0 = 2000000.;
    Q->phits = M_HALFPI;
    P->lam0 = 0.;

    return setup(P);
}

