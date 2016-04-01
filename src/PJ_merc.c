#define PJ_LIB__
#include	<projects.h>
PROJ_HEAD(merc, "Mercator") "\n\tCyl, Sph&Ell\n\tlat_ts=";

#define EPS10 1.e-10


static XY e_forward (LP lp, PJ *P) {          /* Ellipsoidal, forward */
    XY xy = {0.0,0.0};
	if (fabs(fabs(lp.phi) - HALFPI) <= EPS10)
        F_ERROR;
	xy.x = P->k0 * lp.lam;
	xy.y = - P->k0 * log(pj_tsfn(lp.phi, sin(lp.phi), P->e));
	return xy;
}


static XY s_forward (LP lp, PJ *P) {           /* Spheroidal, forward */
    XY xy = {0.0,0.0};
	if (fabs(fabs(lp.phi) - HALFPI) <= EPS10)
        F_ERROR;
	xy.x = P->k0 * lp.lam;
	xy.y = P->k0 * log(tan(FORTPI + .5 * lp.phi));
	return xy;
}


static LP e_inverse (XY xy, PJ *P) {          /* Ellipsoidal, inverse */
    LP lp = {0.0,0.0};
	if ((lp.phi = pj_phi2(P->ctx, exp(- xy.y / P->k0), P->e)) == HUGE_VAL)
        I_ERROR;
	lp.lam = xy.x / P->k0;
	return lp;
}


static LP s_inverse (XY xy, PJ *P) {           /* Spheroidal, inverse */
    LP lp = {0.0,0.0};
	lp.phi = HALFPI - 2. * atan(exp(-xy.y / P->k0));
	lp.lam = xy.x / P->k0;
	return lp;
}


static void freeup(PJ *P) {                             /* Destructor */
    pj_dealloc(P);
}


PJ *PROJECTION(merc) {
	double phits=0.0;
	int is_phits;

	if( (is_phits = pj_param(P->ctx, P->params, "tlat_ts").i) ) {
		phits = fabs(pj_param(P->ctx, P->params, "rlat_ts").f);
		if (phits >= HALFPI) E_ERROR(-24);
	}

	if (P->es) { /* ellipsoid */
		if (is_phits)
			P->k0 = pj_msfn(sin(phits), cos(phits), P->es);
		P->inv = e_inverse;
		P->fwd = e_forward;
	}

    else { /* sphere */
		if (is_phits)
			P->k0 = cos(phits);
		P->inv = s_inverse;
		P->fwd = s_forward;
	}

    return P;
}
