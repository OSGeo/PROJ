#define PJ_LIB__

#include <errno.h>
#include <math.h>

#include "proj.h"
#include "projects.h"

PROJ_HEAD(gn_sinu, "General Sinusoidal Series") "\n\tPCyl, Sph.\n\tm= n=";
PROJ_HEAD(sinu, "Sinusoidal (Sanson-Flamsteed)") "\n\tPCyl, Sph&Ell";
PROJ_HEAD(eck6, "Eckert VI") "\n\tPCyl, Sph.";
PROJ_HEAD(mbtfps, "McBryde-Thomas Flat-Polar Sinusoidal") "\n\tPCyl, Sph.";

#define EPS10    1e-10
#define MAX_ITER 8
#define LOOP_TOL 1e-7

struct pj_opaque {
    double *en;
    double m, n, C_x, C_y;
};


static XY e_forward (LP lp, PJ *P) {          /* Ellipsoidal, forward */
    XY xy = {0.0,0.0};
    double s, c;

    xy.y = pj_mlfn(lp.phi, s = sin(lp.phi), c = cos(lp.phi), P->opaque->en);
    xy.x = lp.lam * c / sqrt(1. - P->es * s * s);
    return xy;
}


static LP e_inverse (XY xy, PJ *P) {          /* Ellipsoidal, inverse */
    LP lp = {0.0,0.0};
    double s;

    if ((s = fabs(lp.phi = pj_inv_mlfn(P->ctx, xy.y, P->es, P->opaque->en))) < M_HALFPI) {
        s = sin(lp.phi);
        lp.lam = xy.x * sqrt(1. - P->es * s * s) / cos(lp.phi);
    } else if ((s - EPS10) < M_HALFPI) {
        lp.lam = 0.;
    } else {
        proj_errno_set(P, PJD_ERR_TOLERANCE_CONDITION);
    }

    return lp;
}


static XY s_forward (LP lp, PJ *P) {           /* Spheroidal, forward */
    XY xy = {0.0,0.0};
    struct pj_opaque *Q = P->opaque;

    if (Q->m == 0.0)
        lp.phi = Q->n != 1. ? aasin(P->ctx,Q->n * sin(lp.phi)): lp.phi;
    else {
        double k, V;
        int i;

        k = Q->n * sin(lp.phi);
        for (i = MAX_ITER; i ; --i) {
            lp.phi -= V = (Q->m * lp.phi + sin(lp.phi) - k) /
                (Q->m + cos(lp.phi));
            if (fabs(V) < LOOP_TOL)
                break;
        }
        if (!i) {
            proj_errno_set(P, PJD_ERR_TOLERANCE_CONDITION);
            return xy;
        }

    }
    xy.x = Q->C_x * lp.lam * (Q->m + cos(lp.phi));
    xy.y = Q->C_y * lp.phi;

    return xy;
}


static LP s_inverse (XY xy, PJ *P) {           /* Spheroidal, inverse */
    LP lp = {0.0,0.0};
    struct pj_opaque *Q = P->opaque;

    xy.y /= Q->C_y;
    lp.phi = (Q->m != 0.0) ? aasin(P->ctx,(Q->m * xy.y + sin(xy.y)) / Q->n) :
        ( Q->n != 1. ? aasin(P->ctx,sin(xy.y) / Q->n) : xy.y );
    lp.lam = xy.x / (Q->C_x * (Q->m + cos(xy.y)));
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



/* for spheres, only */
static void setup(PJ *P) {
    struct pj_opaque *Q = P->opaque;
    P->es = 0;
    P->inv = s_inverse;
    P->fwd = s_forward;

    Q->C_x = (Q->C_y = sqrt((Q->m + 1.) / Q->n))/(Q->m + 1.);
}


PJ *PROJECTION(sinu) {
    struct pj_opaque *Q = pj_calloc (1, sizeof (struct pj_opaque));
    if (0==Q)
        return pj_default_destructor (P, ENOMEM);
    P->opaque = Q;
    P->destructor = destructor;

    if (!(Q->en = pj_enfn(P->es)))
        return pj_default_destructor (P, ENOMEM);

    if (P->es != 0.0) {
        P->inv = e_inverse;
        P->fwd = e_forward;
    } else {
        Q->n = 1.;
        Q->m = 0.;
        setup(P);
    }
    return P;
}


PJ *PROJECTION(eck6) {
    struct pj_opaque *Q = pj_calloc (1, sizeof (struct pj_opaque));
    if (0==Q)
        return pj_default_destructor (P, ENOMEM);
    P->opaque = Q;
    P->destructor = destructor;

    Q->m = 1.;
    Q->n = 2.570796326794896619231321691;
    setup(P);

    return P;
}


PJ *PROJECTION(mbtfps) {
    struct pj_opaque *Q = pj_calloc (1, sizeof (struct pj_opaque));
    if (0==Q)
        return pj_default_destructor (P, ENOMEM);
    P->opaque = Q;
    P->destructor = destructor;

    Q->m = 0.5;
    Q->n = 1.785398163397448309615660845;
    setup(P);

    return P;
}


PJ *PROJECTION(gn_sinu) {
    struct pj_opaque *Q = pj_calloc (1, sizeof (struct pj_opaque));
    if (0==Q)
        return pj_default_destructor (P, ENOMEM);
    P->opaque = Q;
    P->destructor = destructor;

    if (pj_param(P->ctx, P->params, "tn").i && pj_param(P->ctx, P->params, "tm").i) {
        Q->n = pj_param(P->ctx, P->params, "dn").f;
        Q->m = pj_param(P->ctx, P->params, "dm").f;
        if (Q->n <= 0 || Q->m < 0)
            return destructor (P, PJD_ERR_INVALID_M_OR_N);
    } else
        return destructor (P, PJD_ERR_INVALID_M_OR_N);

    setup(P);

    return P;
}
