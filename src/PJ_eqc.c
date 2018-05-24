#define PJ_LIB__

#include <errno.h>
#include <math.h>

#include "proj.h"
#include "projects.h"

struct pj_opaque {
    double rc;
};

PROJ_HEAD(eqc, "Equidistant Cylindrical (Plate Caree)")
    "\n\tCyl, Sph\n\tlat_ts=[, lat_0=0]";


static XY s_forward (LP lp, PJ *P) {           /* Spheroidal, forward */
    XY xy = {0.0,0.0};
    struct pj_opaque *Q = P->opaque;

    xy.x = Q->rc * lp.lam;
    xy.y = lp.phi - P->phi0;

    return xy;
}


static LP s_inverse (XY xy, PJ *P) {           /* Spheroidal, inverse */
    LP lp = {0.0,0.0};
    struct pj_opaque *Q = P->opaque;

    lp.lam = xy.x / Q->rc;
    lp.phi = xy.y + P->phi0;

    return lp;
}


PJ *PROJECTION(eqc) {
    struct pj_opaque *Q = pj_calloc (1, sizeof (struct pj_opaque));
    if (0==Q)
        return pj_default_destructor (P, ENOMEM);
    P->opaque = Q;

    if ((Q->rc = cos(pj_param(P->ctx, P->params, "rlat_ts").f)) <= 0.)
        return pj_default_destructor (P, PJD_ERR_LAT_TS_LARGER_THAN_90);
    P->inv = s_inverse;
    P->fwd = s_forward;
    P->es = 0.;

    return P;
}
