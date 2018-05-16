#define PJ_LIB__

#include <errno.h>
#include <math.h>

#include "projects.h"

struct pj_opaque {
    int vdg3;
};

PROJ_HEAD(vandg2, "van der Grinten II") "\n\tMisc Sph, no inv.";
PROJ_HEAD(vandg3, "van der Grinten III") "\n\tMisc Sph, no inv.";

#define TOL    1e-10


static XY s_forward (LP lp, PJ *P) {           /* Spheroidal, forward */
    XY xy = {0.0,0.0};
    struct pj_opaque *Q = P->opaque;
    double x1, at, bt, ct;

    bt = fabs(M_TWO_D_PI * lp.phi);
    if ((ct = 1. - bt * bt) < 0.)
        ct = 0.;
    else
        ct = sqrt(ct);
    if (fabs(lp.lam) < TOL) {
        xy.x = 0.;
        xy.y = M_PI * (lp.phi < 0. ? -bt : bt) / (1. + ct);
    } else {
        at = 0.5 * fabs(M_PI / lp.lam - lp.lam / M_PI);
        if (Q->vdg3) {
            x1 = bt / (1. + ct);
            xy.x = M_PI * (sqrt(at * at + 1. - x1 * x1) - at);
            xy.y = M_PI * x1;
        } else {
            x1 = (ct * sqrt(1. + at * at) - at * ct * ct) /
                (1. + at * at * bt * bt);
            xy.x = M_PI * x1;
            xy.y = M_PI * sqrt(1. - x1 * (x1 + 2. * at) + TOL);
        }
        if ( lp.lam < 0.) xy.x = -xy.x;
        if ( lp.phi < 0.) xy.y = -xy.y;
    }

    return xy;
}


PJ *PROJECTION(vandg2) {
    struct pj_opaque *Q = pj_calloc (1, sizeof (struct pj_opaque));
    if (0==Q)
        return pj_default_destructor (P, ENOMEM);
    P->opaque = Q;

    Q->vdg3 = 0;
    P->fwd = s_forward;

    return P;
}

PJ *PROJECTION(vandg3) {
    struct pj_opaque *Q = pj_calloc (1, sizeof (struct pj_opaque));
    if (0==Q)
        return pj_default_destructor (P, ENOMEM);
    P->opaque = Q;

    Q->vdg3 = 1;
    P->es = 0.;
    P->fwd = s_forward;

    return P;
}
