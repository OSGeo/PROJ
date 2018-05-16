#define PJ_LIB__

#include <errno.h>
#include <math.h>

#include "projects.h"

PROJ_HEAD(wink1, "Winkel I") "\n\tPCyl., Sph.\n\tlat_ts=";

struct pj_opaque {
	double	cosphi1;
};



static XY s_forward (LP lp, PJ *P) {           /* Spheroidal, forward */
    XY xy = {0.0,0.0};
	xy.x = .5 * lp.lam * (P->opaque->cosphi1 + cos(lp.phi));
	xy.y = lp.phi;
	return (xy);
}


static LP s_inverse (XY xy, PJ *P) {           /* Spheroidal, inverse */
    LP lp = {0.0,0.0};
	lp.phi = xy.y;
	lp.lam = 2. * xy.x / (P->opaque->cosphi1 + cos(lp.phi));
	return (lp);
}


PJ *PROJECTION(wink1) {
    struct pj_opaque *Q = pj_calloc (1, sizeof (struct pj_opaque));
    if (0==Q)
        return pj_default_destructor(P, ENOMEM);
    P->opaque = Q;

	P->opaque->cosphi1 = cos (pj_param(P->ctx, P->params, "rlat_ts").f);
	P->es = 0.;
    P->inv = s_inverse;
    P->fwd = s_forward;

    return P;
}
