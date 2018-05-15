#define PJ_LIB__

#include <errno.h>
#include <math.h>

#include "projects.h"

PROJ_HEAD(kav5,    "Kavraisky V")         "\n\tPCyl., Sph.";
PROJ_HEAD(qua_aut, "Quartic Authalic")    "\n\tPCyl., Sph.";
PROJ_HEAD(fouc,    "Foucaut")             "\n\tPCyl., Sph.";
PROJ_HEAD(mbt_s,   "McBryde-Thomas Flat-Polar Sine (No. 1)") "\n\tPCyl., Sph.";


struct pj_opaque {
	double C_x, C_y, C_p;
	int tan_mode;
};


static XY s_forward (LP lp, PJ *P) {           /* Spheroidal, forward */
    XY xy = {0.0,0.0};
    struct pj_opaque *Q = P->opaque;
	double c;

	xy.x = Q->C_x * lp.lam * cos(lp.phi);
	xy.y = Q->C_y;
	lp.phi *= Q->C_p;
	c = cos(lp.phi);
	if (Q->tan_mode) {
		xy.x *= c * c;
		xy.y *= tan (lp.phi);
	} else {
		xy.x /= c;
		xy.y *= sin (lp.phi);
	}
	return xy;
}


static LP s_inverse (XY xy, PJ *P) {           /* Spheroidal, inverse */
    LP lp = {0.0,0.0};
    struct pj_opaque *Q = P->opaque;
	double c;

	xy.y /= Q->C_y;
	c = cos (lp.phi = Q->tan_mode ? atan (xy.y) : aasin (P->ctx, xy.y));
	lp.phi /= Q->C_p;
	lp.lam = xy.x / (Q->C_x * cos(lp.phi));
	if (Q->tan_mode)
		lp.lam /= c * c;
	else
		lp.lam *= c;
	return lp;
}


static PJ *setup(PJ *P, double p, double q, int mode) {
	P->es  = 0.;
	P->inv = s_inverse;
	P->fwd = s_forward;
	P->opaque->C_x = q / p;
	P->opaque->C_y = p;
	P->opaque->C_p = 1/ q;
	P->opaque->tan_mode = mode;
	return P;
}



PJ *PROJECTION(fouc) {
    struct pj_opaque *Q = pj_calloc (1, sizeof (struct pj_opaque));
    if (0==Q)
        return pj_default_destructor(P, ENOMEM);
    P->opaque = Q;
    return setup(P, 2., 2., 1);
}



PJ *PROJECTION(kav5) {
    struct pj_opaque *Q = pj_calloc (1, sizeof (struct pj_opaque));
    if (0==Q)
        return pj_default_destructor(P, ENOMEM);
    P->opaque = Q;

    return setup(P, 1.50488, 1.35439, 0);
}



PJ *PROJECTION(qua_aut) {
    struct pj_opaque *Q = pj_calloc (1, sizeof (struct pj_opaque));
    if (0==Q)
        return pj_default_destructor(P, ENOMEM);
    P->opaque = Q;
    return setup(P, 2., 2., 0);
}



PJ *PROJECTION(mbt_s) {
    struct pj_opaque *Q = pj_calloc (1, sizeof (struct pj_opaque));
    if (0==Q)
        return pj_default_destructor(P, ENOMEM);
    P->opaque = Q;
    return setup(P, 1.48875, 1.36509, 0);
}
