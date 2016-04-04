/*
The Compact Miller projection was designed by Tom Patterson, US National 
Park Service, in 2014. The polynomial equation was developed by Bojan 
Savric and Bernhard Jenny, College of Earth, Ocean, and Atmospheric 
Sciences, Oregon State University.
Port to PROJ.4 by Bojan Savric, 4 April 2016
*/

#define PJ_LIB__
#include	<projects.h>
PROJ_HEAD(comill, "Compact Miller") "\n\tCyl., Sph.";

#define K1 0.9902
#define K2 0.1604
#define K3 -0.03054
#define C1 K1
#define C2 (3 * K2)
#define C3 (5 * K3)
#define EPS 1e-11
#define MAX_Y (0.6000207669862655 * PI)

FORWARD(s_forward); /* spheroid */
	double lat_sq;

	lat_sq = lp.phi * lp.phi;
	xy.x = lp.lam;
	xy.y = lp.phi * (K1 + lat_sq * (K2 + K3 * lat_sq));
    return (xy);
}
INVERSE(s_inverse); /* spheroid */
	double yc, tol, y2, y4, f, fder;

    /* make sure y is inside valid range */
	if (xy.y > MAX_Y) {
		xy.y = MAX_Y;
	} else if (xy.y < -MAX_Y) {
		xy.y = -MAX_Y;
	}

    /* latitude */
	yc = xy.y;
    for (;;) { /* Newton-Raphson */
		y2 = yc * yc;
		f = (yc * (K1 + y2 * (K2 + K3 * y2))) - xy.y;
		fder = C1 + y2 * (C2 + C3 * y2);
		yc -= tol = f / fder;
		if (fabs(tol) < EPS) {
			break;
		}
	}
	lp.phi = yc;

    /* longitude */
	lp.lam = xy.x;

	return (lp);
}
FREEUP; if (P) pj_dalloc(P); }
ENTRY0(comill) P->es = 0; P->inv = s_inverse; P->fwd = s_forward; ENDENTRY(P)
