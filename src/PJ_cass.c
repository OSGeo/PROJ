#define PJ_LIB__
# include	<projects.h>
PROJ_HEAD(cass, "Cassini") "\n\tCyl, Sph&Ell";


# define EPS10	1e-10
# define C1	.16666666666666666666
# define C2	.00833333333333333333
# define C3	.04166666666666666666
# define C4	.33333333333333333333
# define C5	.06666666666666666666


struct opaque {
    /* These are the only opaque members actually initialized */
	double *en;
	double m0;

    /* Apparently this group of opaque members should be demoted to local variables in e_forward and e_inverse */
	double n;
	double t;
	double a1;
	double c;
	double r;
	double dd;
	double d2;
	double a2;
	double tn;
};



static XY e_forward (LP lp, PJ *P) {          /* Ellipsoidal, forward */
    XY xy = {0.0,0.0};
    struct opaque *O = P->opaq;
	xy.y = pj_mlfn(lp.phi, O->n = sin(lp.phi), O->c = cos(lp.phi), O->en);
	O->n = 1./sqrt(1. - P->es * O->n * O->n);
	O->tn = tan(lp.phi); O->t = O->tn * O->tn;
	O->a1 = lp.lam * O->c;
	O->c *= P->es * O->c / (1 - P->es);
	O->a2 = O->a1 * O->a1;
	xy.x = O->n * O->a1 * (1. - O->a2 * O->t *
		(C1 - (8. - O->t + 8. * O->c) * O->a2 * C2));
	xy.y -= O->m0 - O->n * O->tn * O->a2 *
		(.5 + (5. - O->t + 6. * O->c) * O->a2 * C3);
	return xy;
}


static XY s_forward (LP lp, PJ *P) {           /* Spheroidal, forward */
    XY xy = {0.0,0.0};
	xy.x = asin(cos(lp.phi) * sin(lp.lam));
	xy.y = atan2(tan(lp.phi) , cos(lp.lam)) - P->phi0;
	return xy;
}


static LP e_inverse (XY xy, PJ *P) {          /* Ellipsoidal, inverse */
    LP lp = {0.0,0.0};
    struct opaque *O = P->opaq;
	double ph1;

	ph1 = pj_inv_mlfn(P->ctx, O->m0 + xy.y, P->es, O->en);
	O->tn = tan(ph1); O->t = O->tn * O->tn;
	O->n = sin(ph1);
	O->r = 1. / (1. - P->es * O->n * O->n);
	O->n = sqrt(O->r);
	O->r *= (1. - P->es) * O->n;
	O->dd = xy.x / O->n;
	O->d2 = O->dd * O->dd;
	lp.phi = ph1 - (O->n * O->tn / O->r) * O->d2 *
		(.5 - (1. + 3. * O->t) * O->d2 * C3);
	lp.lam = O->dd * (1. + O->t * O->d2 *
		(-C4 + (1. + 3. * O->t) * O->d2 * C5)) / cos(ph1);
	return lp;
}


static LP s_inverse (XY xy, PJ *P) {           /* Spheroidal, inverse */
    LP lp = {0.0,0.0};
	lp.phi = asin(sin(P->opaq->dd = xy.y + P->phi0) * cos(xy.x));
	lp.lam = atan2(tan(xy.x), cos(P->opaq->dd));
	return lp;
}


static void freeup(PJ *P) {                             /* Destructor */
	if (0==P)
        return;
    if (0==P->opaq) {
        pj_dealloc (P);
        return;
    }
	pj_dealloc(P->opaq->en);
    pj_dealloc(P->opaq);
    pj_dealloc(P);
}


PJ *PROJECTION(cass) {

    /* Spheroidal? */
	if (0==P->es) {
		P->inv = s_inverse;
		P->fwd = s_forward;
        return P;
    }

    /* Otherwise it's ellipsoidal */
    P->opaq = pj_calloc (1, sizeof (struct opaque));
    if (0==P->opaq) {
        freeup (P);
        return 0;
    }

    P->opaq->en = pj_enfn(P->es);
	if (0==P->opaq->en) {
        freeup (P);
        return 0;
    }

	P->opaq->m0 = pj_mlfn(P->phi0, sin(P->phi0), cos(P->phi0), P->opaq->en);
	P->inv = e_inverse;
	P->fwd = e_forward;

    return P;
}
