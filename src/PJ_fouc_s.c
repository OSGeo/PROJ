#define PJ_LIB__

#include <errno.h>
#include <math.h>

#include "proj.h"
#include "projects.h"

PROJ_HEAD(fouc_s, "Foucaut Sinusoidal") "\n\tPCyl., Sph.";

#define MAX_ITER    10
#define LOOP_TOL    1e-7

struct pj_opaque {
    double n, n1;
};


static XY s_forward (LP lp, PJ *P) {           /* Spheroidal, forward */
    XY xy = {0.0,0.0};
    struct pj_opaque *Q = P->opaque;
    double t;

    t = cos(lp.phi);
    xy.x = lp.lam * t / (Q->n + Q->n1 * t);
    xy.y = Q->n * lp.phi + Q->n1 * sin(lp.phi);
    return xy;
}


static LP s_inverse (XY xy, PJ *P) {           /* Spheroidal, inverse */
    LP lp = {0.0,0.0};
    struct pj_opaque *Q = P->opaque;
    double V;
    int i;

    if (Q->n != 0.0) {
        lp.phi = xy.y;
        for (i = MAX_ITER; i ; --i) {
            lp.phi -= V = (Q->n * lp.phi + Q->n1 * sin(lp.phi) - xy.y ) /
                (Q->n + Q->n1 * cos(lp.phi));
            if (fabs(V) < LOOP_TOL)
                break;
        }
        if (!i)
            lp.phi = xy.y < 0. ? -M_HALFPI : M_HALFPI;
    } else
        lp.phi = aasin(P->ctx,xy.y);
    V = cos(lp.phi);
    lp.lam = xy.x * (Q->n + Q->n1 * V) / V;
    return lp;
}


PJ *PROJECTION(fouc_s) {
    struct pj_opaque *Q = pj_calloc (1, sizeof (struct pj_opaque));
    if (0==Q)
        return pj_default_destructor (P, ENOMEM);
    P->opaque = Q;

    Q->n = pj_param(P->ctx, P->params, "dn").f;
    if (Q->n < 0. || Q->n > 1.)
        return pj_default_destructor (P, PJD_ERR_N_OUT_OF_RANGE);

    Q->n1 = 1. - Q->n;
    P->es = 0;
    P->inv = s_inverse;
    P->fwd = s_forward;
    return P;
}
