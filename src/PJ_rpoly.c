#define PJ_LIB__
#include <projects.h>

struct pj_opaque {
    double  phi1;
    double  fxa;
    double  fxb;
    int     mode;
};

PROJ_HEAD(rpoly, "Rectangular Polyconic")
    "\n\tConic, Sph\n\tlat_ts=";

#define TOL	1e-10
#define EPS 1e-9
#define CONV	1e-10
#define N_ITER	10


static XY s_forward (LP lp, PJ *P) {           /* Spheroidal, forward */
    XY xy = {0.0,0.0};
    struct pj_opaque *Q = P->opaque;
    double fa;

    if (Q->mode)
        fa = tan(lp.lam * Q->fxb) * Q->fxa;
    else
        fa = 0.5 * lp.lam;
    if (fabs(lp.phi) < EPS) {
        xy.x = fa + fa;
        xy.y = - P->phi0;
    } else {
        xy.y = 1. / tan(lp.phi);
        xy.x = sin(fa = 2. * atan(fa * sin(lp.phi))) * xy.y;
        xy.y = lp.phi - P->phi0 + (1. - cos(fa)) * xy.y;
    }
    return xy;
}


static LP s_inverse (XY xy, PJ *P) {           /* Spheroidal, inverse */
    LP lp = {0.0,0.0};
    struct pj_opaque *Q = P->opaque;
    double B, dphi, tp;
    int i;

    if (fabs(xy.y = P->phi0 + xy.y) <= TOL) {
        lp.lam = xy.x;
        lp.phi = 0.;
    } else {
        lp.phi = xy.y;
        B = xy.x * xy.x + xy.y * xy.y;
        i = N_ITER;
        do {
            tp = tan(lp.phi);
            lp.phi -= (dphi = (xy.y * (lp.phi * tp + 1.) - lp.phi -
                .5 * (lp.phi * lp.phi + B) * tp) /
                ((lp.phi - xy.y) / tp - 1.));
        } while (fabs(dphi) > CONV && --i);
        if (! i) I_ERROR;

        tp = tan(lp.phi);
        lp.lam = 2. * tan(asin(xy.x * tp) * 0.5) / sin(lp.phi);
    }

    return lp;
}


static void *freeup_new (PJ *P) {                       /* Destructor */
    if (0==P)
        return 0;
    if (0==P->opaque)
        return pj_dealloc (P);

    pj_dealloc (P->opaque);
    return pj_dealloc(P);
}


static void freeup (PJ *P) {
    freeup_new (P);
    return;
}


PJ *PROJECTION(rpoly) {
    struct pj_opaque *Q = pj_calloc (1, sizeof (struct pj_opaque));
    if (0==Q)
        return freeup_new (P);
    P->opaque = Q;

    if ((Q->mode = (Q->phi1 = fabs(pj_param(P->ctx, P->params, "rlat_ts").f)) > EPS)) {
        Q->fxb = 0.5 * sin(Q->phi1);
        Q->fxa = 0.5 / Q->fxb;
    }
    P->es = 0.;
    P->fwd = s_forward;
    P->inv = s_inverse;
    
    return P;
}


#ifndef PJ_SELFTEST
int pj_rpoly_selftest (void) {return 0;}
#else

int pj_rpoly_selftest (void) {
    double tolerance_lp = 1e-10;
    double tolerance_xy = 1e-7;

    char s_args[] = {"+proj=rpoly   +a=6400000    +lat_1=0.5 +lat_2=2"};

    LP fwd_in[] = {
        { 2, 1},
        { 2,-1},
        {-2, 1},
        {-2,-1}
    };

    XY s_fwd_expect[] = {
        { 223368.09830201423,  111769.110486991223},
        { 223368.09830201423, -111769.110486991223},
        {-223368.09830201423,  111769.110486991223},
        {-223368.09830201423, -111769.110486991223},
    };

    return pj_generic_selftest (0, s_args, tolerance_xy, tolerance_lp, 4, 4, fwd_in, 0, s_fwd_expect, 0, 0, 0);
}


#endif
