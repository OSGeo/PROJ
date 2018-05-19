#define PJ_LIB__

#include <math.h>

#include "projects.h"

PROJ_HEAD(wag7, "Wagner VII") "\n\tMisc Sph, no inv.";



static XY s_forward (LP lp, PJ *P) {           /* Spheroidal, forward */
    XY xy = {0.0, 0.0};
	double theta, ct, D;

	(void) P; /* Shut up compiler warnnings about unused P */

	theta = asin (xy.y = 0.90630778703664996 * sin(lp.phi));
	xy.x  = 2.66723 * (ct = cos (theta)) * sin (lp.lam /= 3.);
	xy.y *= 1.24104 * (D = 1/(sqrt (0.5 * (1 + ct * cos (lp.lam)))));
	xy.x *= D;
	return (xy);
}


PJ *PROJECTION(wag7) {
    P->fwd = s_forward;
    P->inv = 0;
    P->es = 0.;
    return P;
}
