#define PJ_LIB__

#include <errno.h>
#include <math.h>

#include "proj.h"
#include "projects.h"

PROJ_HEAD(tmerc, "Transverse Mercator") "\n\tCyl, Sph&Ell";


struct pj_opaque {
    double  esp;
    double  ml0;
    double  *en;
};

#define EPS10   1.e-10
#define FC1 1.
#define FC2 .5
#define FC3 .16666666666666666666
#define FC4 .08333333333333333333
#define FC5 .05
#define FC6 .03333333333333333333
#define FC7 .02380952380952380952
#define FC8 .01785714285714285714


static XY e_forward (LP lp, PJ *P) {          /* Ellipsoidal, forward */
    XY xy = {0.0, 0.0};
    struct pj_opaque *Q = P->opaque;
    double al, als, n, cosphi, sinphi, t;

    /*
     * Fail if our longitude is more than 90 degrees from the
     * central meridian since the results are essentially garbage.
     * Is error -20 really an appropriate return value?
     *
     *  http://trac.osgeo.org/proj/ticket/5
     */
    if( lp.lam < -M_HALFPI || lp.lam > M_HALFPI ) {
        xy.x = HUGE_VAL;
        xy.y = HUGE_VAL;
        pj_ctx_set_errno( P->ctx, PJD_ERR_LAT_OR_LON_EXCEED_LIMIT );
        return xy;
    }

    sinphi = sin (lp.phi);
    cosphi = cos (lp.phi);
    t = fabs (cosphi) > 1e-10 ? sinphi/cosphi : 0.;
    t *= t;
    al = cosphi * lp.lam;
    als = al * al;
    al /= sqrt (1. - P->es * sinphi * sinphi);
    n = Q->esp * cosphi * cosphi;
    xy.x = P->k0 * al * (FC1 +
        FC3 * als * (1. - t + n +
        FC5 * als * (5. + t * (t - 18.) + n * (14. - 58. * t)
        + FC7 * als * (61. + t * ( t * (179. - t) - 479. ) )
        )));
    xy.y = P->k0 * (pj_mlfn(lp.phi, sinphi, cosphi, Q->en) - Q->ml0 +
        sinphi * al * lp.lam * FC2 * ( 1. +
        FC4 * als * (5. - t + n * (9. + 4. * n) +
        FC6 * als * (61. + t * (t - 58.) + n * (270. - 330 * t)
        + FC8 * als * (1385. + t * ( t * (543. - t) - 3111.) )
        ))));
    return (xy);
}


static XY s_forward (LP lp, PJ *P) {           /* Spheroidal, forward */
    XY xy = {0.0,0.0};
    double b, cosphi;

    /*
     * Fail if our longitude is more than 90 degrees from the
     * central meridian since the results are essentially garbage.
     * Is error -20 really an appropriate return value?
     *
     *  http://trac.osgeo.org/proj/ticket/5
     */
    if( lp.lam < -M_HALFPI || lp.lam > M_HALFPI ) {
        xy.x = HUGE_VAL;
        xy.y = HUGE_VAL;
        pj_ctx_set_errno( P->ctx, PJD_ERR_LAT_OR_LON_EXCEED_LIMIT );
        return xy;
    }

    cosphi = cos(lp.phi);
    b = cosphi * sin (lp.lam);
    if (fabs (fabs (b) - 1.) <= EPS10) {
        proj_errno_set(P, PJD_ERR_TOLERANCE_CONDITION);
        return xy;
    }

    xy.x = P->opaque->ml0 * log ((1. + b) / (1. - b));
    xy.y = cosphi * cos (lp.lam) / sqrt (1. - b * b);

    b = fabs ( xy.y );
    if (b >= 1.) {
        if ((b - 1.) > EPS10) {
            proj_errno_set(P, PJD_ERR_TOLERANCE_CONDITION);
            return xy;
        }
        else xy.y = 0.;
    } else
        xy.y = acos (xy.y);

    if (lp.phi < 0.)
        xy.y = -xy.y;
    xy.y = P->opaque->esp * (xy.y - P->phi0);
    return xy;
}


static LP e_inverse (XY xy, PJ *P) {          /* Ellipsoidal, inverse */
    LP lp = {0.0,0.0};
    struct pj_opaque *Q = P->opaque;
    double n, con, cosphi, d, ds, sinphi, t;

    lp.phi = pj_inv_mlfn(P->ctx, Q->ml0 + xy.y / P->k0, P->es, Q->en);
    if (fabs(lp.phi) >= M_HALFPI) {
        lp.phi = xy.y < 0. ? -M_HALFPI : M_HALFPI;
        lp.lam = 0.;
    } else {
        sinphi = sin(lp.phi);
        cosphi = cos(lp.phi);
        t = fabs (cosphi) > 1e-10 ? sinphi/cosphi : 0.;
        n = Q->esp * cosphi * cosphi;
        d = xy.x * sqrt (con = 1. - P->es * sinphi * sinphi) / P->k0;
        con *= t;
        t *= t;
        ds = d * d;
        lp.phi -= (con * ds / (1.-P->es)) * FC2 * (1. -
            ds * FC4 * (5. + t * (3. - 9. *  n) + n * (1. - 4 * n) -
            ds * FC6 * (61. + t * (90. - 252. * n +
                45. * t) + 46. * n
           - ds * FC8 * (1385. + t * (3633. + t * (4095. + 1575. * t)) )
            )));
        lp.lam = d*(FC1 -
            ds*FC3*( 1. + 2.*t + n -
            ds*FC5*(5. + t*(28. + 24.*t + 8.*n) + 6.*n
           - ds * FC7 * (61. + t * (662. + t * (1320. + 720. * t)) )
        ))) / cosphi;
    }
    return lp;
}


static LP s_inverse (XY xy, PJ *P) {           /* Spheroidal, inverse */
    LP lp = {0.0, 0.0};
    double h, g;

    h = exp(xy.x / P->opaque->esp);
    g = .5 * (h - 1. / h);
    h = cos (P->phi0 + xy.y / P->opaque->esp);
    lp.phi = asin(sqrt((1. - h * h) / (1. + g * g)));

    /* Make sure that phi is on the correct hemisphere when false northing is used */
    if (xy.y < 0. && -lp.phi+P->phi0 < 0.0) lp.phi = -lp.phi;

    lp.lam = (g != 0.0 || h != 0.0) ? atan2 (g, h) : 0.;
    return lp;
}


static void *destructor(PJ *P, int errlev) {                       /* Destructor */
    if (0==P)
        return 0;

    if (0==P->opaque)
        return pj_default_destructor(P, errlev);

    pj_dealloc (P->opaque->en);
    return pj_default_destructor(P, errlev);
}


static PJ *setup(PJ *P) {                   /* general initialization */
    struct pj_opaque *Q = P->opaque;
    if (P->es != 0.0) {
        if (!(Q->en = pj_enfn(P->es)))
            return pj_default_destructor(P, ENOMEM);

        Q->ml0 = pj_mlfn(P->phi0, sin(P->phi0), cos(P->phi0), Q->en);
        Q->esp = P->es / (1. - P->es);
        P->inv = e_inverse;
        P->fwd = e_forward;
    } else {
        Q->esp = P->k0;
        Q->ml0 = .5 * Q->esp;
        P->inv = s_inverse;
        P->fwd = s_forward;
    }
    return P;
}


PJ *PROJECTION(tmerc) {
    struct pj_opaque *Q = pj_calloc (1, sizeof (struct pj_opaque));
    if (0==Q)
        return pj_default_destructor (P, ENOMEM);

    P->opaque = Q;
    P->destructor = destructor;

    return setup(P);
}
