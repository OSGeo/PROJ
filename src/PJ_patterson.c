/*
 * The Patterson projection was designed by Tom Patterson, US National
 * Park Service, in 2014, using Flex Projector. The polynomial equations for the
 * projection were developed by Bojan Savric, Oregon State University, in
 * collaboration with Tom Patterson and Bernhard Jenny, Oregon State University.
 *
 * Java reference algorithm implemented by Bojan Savric in JMapProjLib
 * project (file PattersonProjection.java)
 *
 * Port to Proj.4 by Micah Cochran, 26 March 2016
 */

#define PJ_LIB__
# include	<projects.h>
PROJ_HEAD(patterson, "Patterson Cylindrical") "\n\tCyl.";
#define K1 1.0148
#define K2 0.23185
#define K3 -0.14499
#define K4 0.02406
#define C1 K1
#define C2 (5.0 * K2)
#define C3 (7.0 * K3)
#define C4 (9.0 * K4)
#define EPS11 1.0e-11
#define MAX_Y 1.790857183

FORWARD(s_forward); /* spheroid */
    double phi2;
	(void) P;
    phi2 = lp.phi * lp.phi;
	xy.x = lp.lam;
    xy.y = lp.phi * (K1 + phi2 * phi2 * (K2 + phi2 * (K3 + K4 * phi2)));
	return (xy);
}
INVERSE(s_inverse); /* spheroid */
    double yc, tol, y2, f, fder;
	(void) P;
    yc = xy.y;
    
    /* make sure y is inside valid range */
    if (xy.y > MAX_Y) {
        xy.y = MAX_Y;
    } else if (xy.y < -MAX_Y) {
        xy.y = -MAX_Y;
    }
    
    for (;;) { /* Newton-Raphson */
		y2 = yc * yc;
		f = (yc * (K1 + y2 * y2 * (K2 + y2 * (K3 + K4 * y2)))) - xy.y;
		fder = C1 + y2 * y2 * (C2 + y2 * (C3 + C4 * y2));
		yc -= tol = f / fder;
		if (fabs(tol) < EPS11) {
			break;
		}
	}
    lp.phi = yc;
    
    /* longitude */
	lp.lam = xy.x;
	
	return (lp);
}
FREEUP; if (P) pj_dalloc(P); }
ENTRY0(patterson) P->es = 0.; P->inv = s_inverse; P->fwd = s_forward; ENDENTRY(P)
