/*
Equal Earth is a projection inspired by the Robinson projection, but unlike
the Robinson projection retains the relative size of areas. The projection
was designed in 2018 by Bojan Savric, Tom Patterson and Bernhard Jenny.

Publication:
Bojan Savric, Tom Patterson & Bernhard Jenny (2018). The Equal Earth map
projection, International Journal of Geographical Information Science,
DOI: 10.1080/13658816.2018.1504949

Port to PROJ by Juernjakob Dugge, 16 August 2018
Added ellipsoidal equations by Bojan Savric, 22 August 2018
*/
#define PJ_LIB__

#include <errno.h>
#include <math.h>

#include "projects.h"

PROJ_HEAD(eqearth, "Equal Earth") "\n\tPCyl., Sph&Ell";

/* A1..A4, polynomial coefficients */
#define A1 1.340264
#define A2 -0.081106
#define A3 0.000893
#define A4 0.003796
#define M (sqrt(3) / 2.0)

#define MAX_Y 1.3173627591574  /* 90° latitude on a sphere with radius 1 */
#define EPS 1e-11
#define MAX_ITER 12

struct pj_opaque {
    double  qp;
    double  rqda;
    double *apa;
};

static XY e_forward (LP lp, PJ *P) {           /* Ellipsoidal/spheroidal, forward */
    XY xy = {0.0,0.0};
    struct pj_opaque *Q = P->opaque;
    double sbeta;
    double psi, psi2, psi6;

    /* Spheroidal case, using sine latitude */
    sbeta = sin(lp.phi);

    /* In the ellipsoidal case, we convert sbeta to sine of authalic latitude */
    if (P->es != 0.0) {
       sbeta = pj_qsfn(sbeta, P->e, 1.0 - P->es) / Q->qp;

       /* Rounding error. */
       if (fabs(sbeta) > 1)
          sbeta = sbeta > 0 ? 1 : -1;
    }

    /* Equal Earth projection */
    psi  = asin(M * sbeta);
    psi2 = psi  * psi;
    psi6 = psi2 * psi2 * psi2;

    xy.x = lp.lam * cos(psi) / (M * (A1 + 3 * A2 * psi2 + psi6 * (7 * A3 + 9 * A4 * psi2)));
    xy.y = psi * (A1 + A2 * psi2 + psi6 * (A3 + A4 * psi2));
    
    /* Adjusting x and y for authalic radius */
    xy.x *= Q->rqda;
    xy.y *= Q->rqda;

    return xy;
}


static LP e_inverse (XY xy, PJ *P) {           /* Ellipsoidal/spheroidal, inverse */
    LP lp = {0.0,0.0};
    struct pj_opaque *Q = P->opaque;
    double yc, y2, y6;
    int i;

    /* Adjusting x and y for authalic radius */
    xy.x /= Q->rqda;
    xy.y /= Q->rqda;

    /* Make sure y is inside valid range */
    if (xy.y > MAX_Y)
        xy.y = MAX_Y;
    else if (xy.y < -MAX_Y)
        xy.y = -MAX_Y;

    yc = xy.y;

    /* Newton-Raphson */
    for (i = MAX_ITER; i ; --i) {
        double f, fder, tol;

        y2 = yc * yc;
        y6 = y2 * y2 * y2;

        f    = yc * (A1 + A2 * y2 + y6 * (A3 + A4 * y2)) - xy.y;
        fder = A1 + 3 * A2 * y2 + y6 * (7 * A3 + 9 * A4 * y2);

        tol  = f / fder;
        yc  -= tol;

        if (fabs(tol) < EPS)
            break;
    }

    if( i == 0 ) {
        pj_ctx_set_errno( P->ctx, PJD_ERR_NON_CONVERGENT );
        return lp;
    }

    /* Longitude */
    y2 = yc * yc;
    y6 = y2 * y2 * y2;

    lp.lam = M * xy.x * (A1 + 3 * A2 * y2 + y6 * (7 * A3 + 9 * A4 * y2)) / cos(yc);

    /* Latitude (for spheroidal case, this is latitude */
    lp.phi = asin(sin(yc) / M);

    /* Ellipsoidal case, converting auth. latitude */
    if (P->es != 0.0)
       lp.phi = pj_authlat(lp.phi, Q->apa);

    return lp;
}

static void *destructor (PJ *P, int errlev) { /* Destructor */
    if (0==P)
        return 0;

    if (0==P->opaque)
        return pj_default_destructor (P, errlev);

    pj_dealloc (P->opaque->apa);
    return pj_default_destructor (P, errlev);
}


PJ *PROJECTION(eqearth) {
    struct pj_opaque *Q = pj_calloc (1, sizeof (struct pj_opaque));
    if (0==Q)
        return pj_default_destructor (P, ENOMEM);
    P->opaque = Q;
    P->destructor = destructor;
    P->fwd = e_forward;
    P->inv = e_inverse;
    Q->rqda = 1.0;

    /* Ellipsoidal case */
    if (P->es != 0.0) {
       Q->apa = pj_authset(P->es); /* For auth_lat(). */
       if (0 == Q->apa)
          return destructor(P, ENOMEM);
       Q->qp = pj_qsfn(1.0, P->e, P->one_es); /* For auth_lat(). */
       Q->rqda = sqrt(0.5*Q->qp); /* Authalic radius devided by major axis */
    }

    return P;
}
