#define PJ_LIB__

#include <errno.h>
#include <math.h>

#include "proj.h"
#include "proj_internal.h"

PROJ_HEAD(wink2, "Winkel II") "\n\tPCyl, Sph, no inv\n\tlat_1=";

namespace { // anonymous namespace
struct pj_opaque {
    double	cosphi1;
};
} // anonymous namespace

#define MAX_ITER    10
#define LOOP_TOL    1e-7


static PJ_XY wink2_s_forward (PJ_LP lp, PJ *P) {           /* Spheroidal, forward */
    PJ_XY xy = {0.0, 0.0};
	double k, V;
	int i;

	xy.y = lp.phi * M_TWO_D_PI;
	k = M_PI * sin (lp.phi);
	lp.phi *= 1.8;
	for (i = MAX_ITER; i ; --i) {
		lp.phi -= V = (lp.phi + sin (lp.phi) - k) /
			(1. + cos (lp.phi));
		if (fabs (V) < LOOP_TOL)
			break;
	}
	if (!i)
		lp.phi = (lp.phi < 0.) ? -M_HALFPI : M_HALFPI;
	else
		lp.phi *= 0.5;
	xy.x = 0.5 * lp.lam * (cos (lp.phi) + static_cast<struct pj_opaque*>(P->opaque)->cosphi1);
	xy.y = M_FORTPI * (sin (lp.phi) + xy.y);
	return xy;
}


PJ *PROJECTION(wink2) {
    struct pj_opaque *Q = static_cast<struct pj_opaque*>(pj_calloc (1, sizeof (struct pj_opaque)));
    if (nullptr==Q)
        return pj_default_destructor(P, ENOMEM);
    P->opaque = Q;

	static_cast<struct pj_opaque*>(P->opaque)->cosphi1 = cos(pj_param(P->ctx, P->params, "rlat_1").f);
	P->es  = 0.;
    P->inv = nullptr;
    P->fwd = wink2_s_forward;

    return P;
}
