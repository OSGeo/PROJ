#define PJ_LIB__

#include <errno.h>
#include <math.h>

#include "proj.h"
#include "projects.h"

PROJ_HEAD(poly, "Polyconic (American)")
    "\n\tConic, Sph&Ell";

struct pj_opaque {
    double ml0; \
    double *en;
};

#define TOL 1e-10
#define CONV    1e-10
#define N_ITER  10
#define I_ITER 20
#define ITOL 1.e-12


static XY e_forward (LP lp, PJ *P) {          /* Ellipsoidal, forward */
    XY xy = {0.0,0.0};
    struct pj_opaque *Q = P->opaque;
    double  ms, sp, cp;

    if (fabs(lp.phi) <= TOL) {
        xy.x = lp.lam;
        xy.y = -Q->ml0;
    } else {
        sp = sin(lp.phi);
        ms = fabs(cp = cos(lp.phi)) > TOL ? pj_msfn(sp, cp, P->es) / sp : 0.;
        xy.x = ms * sin(lp.lam *= sp);
        xy.y = (pj_mlfn(lp.phi, sp, cp, Q->en) - Q->ml0) + ms * (1. - cos(lp.lam));
    }

    return xy;
}


static XY s_forward (LP lp, PJ *P) {           /* Spheroidal, forward */
    XY xy = {0.0,0.0};
    struct pj_opaque *Q = P->opaque;
    double  cot, E;

    if (fabs(lp.phi) <= TOL) {
        xy.x = lp.lam;
        xy.y = Q->ml0;
    } else {
        cot = 1. / tan(lp.phi);
        xy.x = sin(E = lp.lam * sin(lp.phi)) * cot;
        xy.y = lp.phi - P->phi0 + cot * (1. - cos(E));
    }

    return xy;
}


static LP e_inverse (XY xy, PJ *P) {          /* Ellipsoidal, inverse */
    LP lp = {0.0,0.0};
    struct pj_opaque *Q = P->opaque;

    xy.y += Q->ml0;
    if (fabs(xy.y) <= TOL) {
        lp.lam = xy.x;
        lp.phi = 0.;
    } else {
        double r, c, sp, cp, s2ph, ml, mlb, mlp, dPhi;
        int i;

        r = xy.y * xy.y + xy.x * xy.x;
        for (lp.phi = xy.y, i = I_ITER; i ; --i) {
            sp = sin(lp.phi);
            s2ph = sp * ( cp = cos(lp.phi));
            if (fabs(cp) < ITOL) {
                proj_errno_set(P, PJD_ERR_TOLERANCE_CONDITION);
                return lp;
            }
            c = sp * (mlp = sqrt(1. - P->es * sp * sp)) / cp;
            ml = pj_mlfn(lp.phi, sp, cp, Q->en);
            mlb = ml * ml + r;
            mlp = P->one_es / (mlp * mlp * mlp);
            lp.phi += ( dPhi =
                ( ml + ml + c * mlb - 2. * xy.y * (c * ml + 1.) ) / (
                P->es * s2ph * (mlb - 2. * xy.y * ml) / c +
                2.* (xy.y - ml) * (c * mlp - 1. / s2ph) - mlp - mlp ));
            if (fabs(dPhi) <= ITOL)
                break;
        }
        if (!i) {
            proj_errno_set(P, PJD_ERR_TOLERANCE_CONDITION);
            return lp;
        }
        c = sin(lp.phi);
        lp.lam = asin(xy.x * tan(lp.phi) * sqrt(1. - P->es * c * c)) / sin(lp.phi);
    }

    return lp;
}


static LP s_inverse (XY xy, PJ *P) {           /* Spheroidal, inverse */
    LP lp = {0.0,0.0};
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
                .5 * ( lp.phi * lp.phi + B) * tp) /
                ((lp.phi - xy.y) / tp - 1.));
        } while (fabs(dphi) > CONV && --i);
        if (! i) {
            proj_errno_set(P, PJD_ERR_TOLERANCE_CONDITION);
            return lp;
        }
        lp.lam = asin(xy.x * tan(lp.phi)) / sin(lp.phi);
    }

    return lp;
}


static void *destructor(PJ *P, int errlev) {
    if (0==P)
        return 0;

    if (0==P->opaque)
        return pj_default_destructor (P, errlev);

    if (P->opaque->en)
        pj_dealloc (P->opaque->en);

    return pj_default_destructor(P, errlev);
}


PJ *PROJECTION(poly) {
    struct pj_opaque *Q = pj_calloc (1, sizeof (struct pj_opaque));
    if (0==Q)
        return pj_default_destructor (P, ENOMEM);

    P->opaque = Q;
    P->destructor = destructor;

    if (P->es != 0.0) {
        if (!(Q->en = pj_enfn(P->es)))
            return pj_default_destructor (P, ENOMEM);
        Q->ml0 = pj_mlfn(P->phi0, sin(P->phi0), cos(P->phi0), Q->en);
        P->inv = e_inverse;
        P->fwd = e_forward;
    } else {
        Q->ml0 = -P->phi0;
        P->inv = s_inverse;
        P->fwd = s_forward;
    }

    return P;
}
