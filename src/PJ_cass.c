#define PJ_LIB__

#include <errno.h>
#include <math.h>

#include "projects.h"

PROJ_HEAD(cass, "Cassini") "\n\tCyl, Sph&Ell";


# define C1 .16666666666666666666
# define C2 .00833333333333333333
# define C3 .04166666666666666666
# define C4 .33333333333333333333
# define C5 .06666666666666666666


struct pj_opaque {
    double *en;
    double m0;
};



static XY e_forward (LP lp, PJ *P) {          /* Ellipsoidal, forward */
    double n, t, a1, c, a2, tn;
    XY xy = {0.0, 0.0};
    struct pj_opaque *Q = P->opaque;

    xy.y = pj_mlfn (lp.phi, n = sin (lp.phi), c = cos (lp.phi), Q->en);

    n  = 1./sqrt(1. - P->es * n*n);
    tn = tan(lp.phi); t = tn * tn;
    a1 = lp.lam * c;
    c *= P->es * c / (1 - P->es);
    a2 = a1 * a1;

    xy.x = n * a1 * (1. - a2 * t *
        (C1 - (8. - t + 8. * c) * a2 * C2));
    xy.y -= Q->m0 - n * tn * a2 *
        (.5 + (5. - t + 6. * c) * a2 * C3);

    return xy;
}


static XY s_forward (LP lp, PJ *P) {           /* Spheroidal, forward */
    XY xy = {0.0, 0.0};
    xy.x  =  asin (cos (lp.phi) * sin (lp.lam));
    xy.y  =  atan2 (tan (lp.phi), cos (lp.lam)) - P->phi0;
    return xy;
}


static LP e_inverse (XY xy, PJ *P) {          /* Ellipsoidal, inverse */
    double n, t, r, dd, d2, tn, ph1;
    LP lp = {0.0, 0.0};
    struct pj_opaque *Q = P->opaque;

    ph1 = pj_inv_mlfn (P->ctx, Q->m0 + xy.y, P->es, Q->en);
    tn  = tan (ph1); t = tn*tn;
    n   = sin (ph1);
    r   = 1. / (1. - P->es * n * n);
    n   = sqrt (r);
    r  *= (1. - P->es) * n;
    dd  = xy.x / n;
    d2  = dd * dd;
    lp.phi = ph1 - (n * tn / r) * d2 *
        (.5 - (1. + 3. * t) * d2 * C3);
    lp.lam = dd * (1. + t * d2 *
        (-C4 + (1. + 3. * t) * d2 * C5)) / cos (ph1);
    return lp;
}


static LP s_inverse (XY xy, PJ *P) {           /* Spheroidal, inverse */
    LP lp = {0.0,0.0};
    double dd;
    lp.phi = asin(sin(dd = xy.y + P->phi0) * cos(xy.x));
    lp.lam = atan2(tan(xy.x), cos(dd));
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



PJ *PROJECTION(cass) {

    /* Spheroidal? */
    if (0==P->es) {
        P->inv = s_inverse;
        P->fwd = s_forward;
        return P;
    }

    /* otherwise it's ellipsoidal */
    P->opaque = pj_calloc (1, sizeof (struct pj_opaque));
    if (0==P->opaque)
        return pj_default_destructor (P, ENOMEM);
    P->destructor = destructor;

    P->opaque->en = pj_enfn (P->es);
    if (0==P->opaque->en)
        return pj_default_destructor (P, ENOMEM);

    P->opaque->m0 = pj_mlfn (P->phi0,  sin (P->phi0),  cos (P->phi0),  P->opaque->en);
    P->inv = e_inverse;
    P->fwd = e_forward;

    return P;
}
