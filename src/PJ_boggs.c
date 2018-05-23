#define PJ_LIB__
#include <math.h>

#include "projects.h"

PROJ_HEAD(boggs, "Boggs Eumorphic") "\n\tPCyl., no inv., Sph.";
# define NITER	20
# define EPS	1e-7
# define FXC	2.00276
# define FXC2	1.11072
# define FYC	0.49931


static XY s_forward (LP lp, PJ *P) {           /* Spheroidal, forward */
    XY xy = {0.0,0.0};
	double theta, th1, c;
	int i;
	(void) P;

	theta = lp.phi;
	if (fabs(fabs(lp.phi) - M_HALFPI) < EPS)
		xy.x = 0.;
	else {
		c = sin(theta) * M_PI;
		for (i = NITER; i; --i) {
			theta -= th1 = (theta + sin(theta) - c) /
				(1. + cos(theta));
			if (fabs(th1) < EPS) break;
		}
		theta *= 0.5;
		xy.x = FXC * lp.lam / (1. / cos(lp.phi) + FXC2 / cos(theta));
	}
	xy.y = FYC * (lp.phi + M_SQRT2 * sin(theta));
	return (xy);
}



PJ *PROJECTION(boggs) {
    P->es = 0.;
    P->fwd = s_forward;
    return P;
}
