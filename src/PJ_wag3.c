#define PJ_LIB__

#include <errno.h>
#include <math.h>

#include "projects.h"

PROJ_HEAD(wag3, "Wagner III") "\n\tPCyl., Sph.\n\tlat_ts=";

#define TWOTHIRD 0.6666666666666666666667

struct pj_opaque {
	double	C_x;
};


static XY s_forward (LP lp, PJ *P) {           /* Spheroidal, forward */
    XY xy = {0.0,0.0};
	xy.x = P->opaque->C_x * lp.lam * cos(TWOTHIRD * lp.phi);
	xy.y = lp.phi;
	return xy;
}


static LP s_inverse (XY xy, PJ *P) {           /* Spheroidal, inverse */
    LP lp = {0.0,0.0};
	lp.phi = xy.y;
	lp.lam = xy.x / (P->opaque->C_x * cos(TWOTHIRD * lp.phi));
	return lp;
}


PJ *PROJECTION(wag3) {
	double ts;
    struct pj_opaque *Q = pj_calloc (1, sizeof (struct pj_opaque));
    if (0==Q)
        return pj_default_destructor(P, ENOMEM);

    P->opaque = Q;

	ts = pj_param (P->ctx, P->params, "rlat_ts").f;
	P->opaque->C_x = cos (ts) / cos (2.*ts/3.);
	P->es = 0.;
    P->inv = s_inverse;
    P->fwd = s_forward;

    return P;
}
