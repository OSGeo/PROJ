/* determine latitude angle phi-2 */
#include <proj.h>
#include "projects.h"

#define TOL 1.0e-10
#define N_ITER 15


double pj_phi2(PJ *P, double ts, double tolerance) {
	double ehalf, phi;
	int i;

    ehalf = P->e/2;
    phi = M_HALFPI - 2 * atan (ts);

    for (i = 0;  i < N_ITER;  i++) {
		double previous, con;

        con = P->e * sin (phi);
        previous = phi;
		phi = M_HALFPI - 2 * atan (ts * pow ( (1. - con) / (1. + con), ehalf) );

        if ( fabs (phi - previous) < TOL)
            break;
	}

    if (i==N_ITER)
		proj_errno_set (P, PJD_ERR_NON_CON_INV_PHI2);
	return phi;
}
