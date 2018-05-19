#define PJ_LIB__

#include <math.h>

#include "projects.h"

PROJ_HEAD(putp2, "Putnins P2") "\n\tPCyl., Sph.";

#define C_x	1.89490
#define C_y	1.71848
#define C_p 0.6141848493043784
#define EPS	1e-10
#define NITER	10
#define PI_DIV_3	1.0471975511965977


static XY s_forward (LP lp, PJ *P) {           /* Spheroidal, forward */
    XY xy = {0.0,0.0};
	double p, c, s, V;
	int i;
	(void) P;

	p = C_p * sin(lp.phi);
	s = lp.phi * lp.phi;
	lp.phi *= 0.615709 + s * ( 0.00909953 + s * 0.0046292 );
	for (i = NITER; i ; --i) {
		c = cos(lp.phi);
		s = sin(lp.phi);
		lp.phi -= V = (lp.phi + s * (c - 1.) - p) /
			(1. + c * (c - 1.) - s * s);
		if (fabs(V) < EPS)
			break;
	}
	if (!i)
		lp.phi = lp.phi < 0 ? - PI_DIV_3 : PI_DIV_3;
	xy.x = C_x * lp.lam * (cos(lp.phi) - 0.5);
	xy.y = C_y * sin(lp.phi);

	return xy;
}


static LP s_inverse (XY xy, PJ *P) {           /* Spheroidal, inverse */
    LP lp = {0.0,0.0};
	double c;

	lp.phi = aasin(P->ctx,xy.y / C_y);
	lp.lam = xy.x / (C_x * ((c = cos(lp.phi)) - 0.5));
	lp.phi = aasin(P->ctx,(lp.phi + sin(lp.phi) * (c - 1.)) / C_p);

	return lp;
}


PJ *PROJECTION(putp2) {
    P->es = 0.;
    P->inv = s_inverse;
    P->fwd = s_forward;

    return P;
}
