#define PJ_LIB__
# include	<projects.h>
PROJ_HEAD(cass, "Cassini") "\n\tCyl, Sph&Ell";


# define EPS10	1e-10
# define C1	.16666666666666666666
# define C2	.00833333333333333333
# define C3	.04166666666666666666
# define C4	.33333333333333333333
# define C5	.06666666666666666666


struct pj_opaque {
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
    struct pj_opaque *Q = P->opaque;
	xy.y = pj_mlfn(lp.phi, Q->n = sin(lp.phi), Q->c = cos(lp.phi), Q->en);
	Q->n = 1./sqrt(1. - P->es * Q->n * Q->n);
	Q->tn = tan(lp.phi); Q->t = Q->tn * Q->tn;
	Q->a1 = lp.lam * Q->c;
	Q->c *= P->es * Q->c / (1 - P->es);
	Q->a2 = Q->a1 * Q->a1;
	xy.x = Q->n * Q->a1 * (1. - Q->a2 * Q->t *
		(C1 - (8. - Q->t + 8. * Q->c) * Q->a2 * C2));
	xy.y -= Q->m0 - Q->n * Q->tn * Q->a2 *
		(.5 + (5. - Q->t + 6. * Q->c) * Q->a2 * C3);
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
    struct pj_opaque *Q = P->opaque;
	double ph1;

	ph1 = pj_inv_mlfn(P->ctx, Q->m0 + xy.y, P->es, Q->en);
	Q->tn = tan(ph1); Q->t = Q->tn * Q->tn;
	Q->n = sin(ph1);
	Q->r = 1. / (1. - P->es * Q->n * Q->n);
	Q->n = sqrt(Q->r);
	Q->r *= (1. - P->es) * Q->n;
	Q->dd = xy.x / Q->n;
	Q->d2 = Q->dd * Q->dd;
	lp.phi = ph1 - (Q->n * Q->tn / Q->r) * Q->d2 *
		(.5 - (1. + 3. * Q->t) * Q->d2 * C3);
	lp.lam = Q->dd * (1. + Q->t * Q->d2 *
		(-C4 + (1. + 3. * Q->t) * Q->d2 * C5)) / cos(ph1);
	return lp;
}


static LP s_inverse (XY xy, PJ *P) {           /* Spheroidal, inverse */
    LP lp = {0.0,0.0};
	lp.phi = asin(sin(P->opaque->dd = xy.y + P->phi0) * cos(xy.x));
	lp.lam = atan2(tan(xy.x), cos(P->opaque->dd));
	return lp;
}


static void *freeup_new(PJ *P) {                        /* Destructor */
	if (0==P)
        return 0;
    if (0==P->opaque)
        return pj_dealloc (P);

	pj_dealloc(P->opaque->en);
    pj_dealloc(P->opaque);
    return pj_dealloc(P);
}

static void freeup(PJ *P) {                             /* Destructor */
    freeup_new (P);
    return;
}

PJ *PROJECTION(cass) {

    /* Spheroidal? */
	if (0==P->es) {
		P->inv = s_inverse;
		P->fwd = s_forward;
        return P;
    }

    /* Otherwise it's ellipsoidal */
    P->opaque = pj_calloc (1, sizeof (struct pj_opaque));
    if (0==P->opaque) {
        freeup (P);
        return 0;
    }

    P->opaque->en = pj_enfn(P->es);
	if (0==P->opaque->en) {
        freeup (P);
        return 0;
    }

	P->opaque->m0 = pj_mlfn(P->phi0, sin(P->phi0), cos(P->phi0), P->opaque->en);
	P->inv = e_inverse;
	P->fwd = e_forward;

    return P;
}
