#ifndef lint
static const char SCCSID[]="@(#)PJ_eqr.c	4.5	07/07/18	???	???";
#endif
#define PROJ_PARMS__ \
	double rc;

#define PJ_LIB__
# include	<projects.h>
PROJ_HEAD(eqr, "Equidistant Cylindrical (Generalized)")
	"\n\tCyl, Sph\n\tk_0= lat_ts=";
FORWARD(s_forward); /* spheroid */
	xy.x = P->rc * lp.lam * P->k0 * P->ra;
	xy.y = (lp.phi - P->phi0) * P->k0 * P->ra;
	return (xy);
}
INVERSE(s_inverse); /* spheroid */
	lp.lam = (xy.x * P->a) / (P->k0 * P->rc);
	lp.phi = ((xy.y * P->a) / P->k0) + P->phi0;
	return (lp);
}
FREEUP; if (P) pj_dalloc(P); }
ENTRY0(eqr)
	/* DGR, 2007-08-29 : due to Geoconcept bad implementation */
        /*                   phi0 no longer use lat_ts !          */
	if ((P->rc = cos(pj_param(P->params, "rlat_ts").f)) <= 0.) E_ERROR(-24);
	P->inv = s_inverse;
	P->fwd = s_forward;
#if 0
        /*
         * if set to 0. the computation is being made on a sphere!
         * We simply approximate this as the projection is only
         * used for displaying
         */
	P->es = 0.;
#endif
ENDENTRY(P)
