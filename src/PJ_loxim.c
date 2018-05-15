#define PJ_LIB__

#include <errno.h>
#include <math.h>

#include "proj.h"
#include "projects.h"

PROJ_HEAD(loxim, "Loximuthal") "\n\tPCyl Sph";

#define EPS 1e-8

struct pj_opaque {
    double phi1;
    double cosphi1;
    double tanphi1;
};


static XY s_forward (LP lp, PJ *P) {           /* Spheroidal, forward */
    XY xy = {0.0,0.0};
    struct pj_opaque *Q = P->opaque;

    xy.y = lp.phi - Q->phi1;
    if (fabs(xy.y) < EPS)
        xy.x = lp.lam * Q->cosphi1;
    else {
        xy.x = M_FORTPI + 0.5 * lp.phi;
        if (fabs(xy.x) < EPS || fabs(fabs(xy.x) - M_HALFPI) < EPS)
            xy.x = 0.;
        else
            xy.x = lp.lam * xy.y / log( tan(xy.x) / Q->tanphi1 );
    }
    return xy;
}


static LP s_inverse (XY xy, PJ *P) {           /* Spheroidal, inverse */
    LP lp = {0.0,0.0};
    struct pj_opaque *Q = P->opaque;

    lp.phi = xy.y + Q->phi1;
    if (fabs(xy.y) < EPS) {
        lp.lam = xy.x / Q->cosphi1;
    } else {
        lp.lam = M_FORTPI + 0.5 * lp.phi;
        if (fabs(lp.lam) < EPS || fabs(fabs(lp.lam) - M_HALFPI) < EPS)
            lp.lam = 0.;
        else
            lp.lam = xy.x * log( tan(lp.lam) / Q->tanphi1 ) / xy.y ;
    }
    return lp;
}


PJ *PROJECTION(loxim) {
    struct pj_opaque *Q = pj_calloc (1, sizeof (struct pj_opaque));
    if (0==Q)
        return pj_default_destructor (P, ENOMEM);
    P->opaque = Q;

    Q->phi1 = pj_param(P->ctx, P->params, "rlat_1").f;
    Q->cosphi1 = cos(Q->phi1);
    if (Q->cosphi1 < EPS)
        return pj_default_destructor(P, PJD_ERR_LAT_LARGER_THAN_90);


    Q->tanphi1 = tan(M_FORTPI + 0.5 * Q->phi1);

    P->inv = s_inverse;
    P->fwd = s_forward;
    P->es = 0.;

   return P;
}
