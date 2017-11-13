# define HLFPI2	2.46740110027233965467      /* (pi/2)^2 */
# define EPS	1e-10
#define PJ_LIB__
#include	<errno.h>
#include	<projects.h>


struct pj_opaque {
	int bacn;
	int ortl;
};

PROJ_HEAD(apian, "Apian Globular I") "\n\tMisc Sph, no inv.";
PROJ_HEAD(ortel, "Ortelius Oval") "\n\tMisc Sph, no inv.";
PROJ_HEAD(bacon, "Bacon Globular") "\n\tMisc Sph, no inv.";


static XY s_forward (LP lp, PJ *P) {           /* Spheroidal, forward */
    XY xy = {0.0,0.0};
    struct pj_opaque *Q = P->opaque;
	double ax, f;

	xy.y = Q->bacn ? M_HALFPI * sin(lp.phi) : lp.phi;
	if ((ax = fabs(lp.lam)) >= EPS) {
		if (Q->ortl && ax >= M_HALFPI)
			xy.x = sqrt(HLFPI2 - lp.phi * lp.phi + EPS) + ax - M_HALFPI;
		else {
			f = 0.5 * (HLFPI2 / ax + ax);
			xy.x = ax - f + sqrt(f * f - xy.y * xy.y);
		}
		if (lp.lam < 0.) xy.x = - xy.x;
	} else
		xy.x = 0.;
	return (xy);
}



PJ *PROJECTION(bacon) {
    struct pj_opaque *Q = pj_calloc (1, sizeof (struct pj_opaque));
    if (0==Q)
        return pj_default_destructor (P, ENOMEM);
    P->opaque = Q;

	Q->bacn = 1;
	Q->ortl = 0;
	P->es = 0.;
    P->fwd = s_forward;
    return P;
}


PJ *PROJECTION(apian) {
    struct pj_opaque *Q = pj_calloc (1, sizeof (struct pj_opaque));
    if (0==Q)
        return pj_default_destructor (P, ENOMEM);
    P->opaque = Q;

	Q->bacn = Q->ortl = 0;
	P->es = 0.;
    P->fwd = s_forward;
    return P;
}


PJ *PROJECTION(ortel) {
    struct pj_opaque *Q = pj_calloc (1, sizeof (struct pj_opaque));
    if (0==Q)
        return pj_default_destructor (P, ENOMEM);
    P->opaque = Q;

	Q->bacn = 0;
	Q->ortl = 1;
	P->es = 0.;
    P->fwd = s_forward;
    return P;
}


