#ifndef lint
static const char SCCSID[]="@(#)PJ_aitoff.c	4.1	94/02/15	GIE	REL";
#endif
#define PROJ_PARMS__ \
	double	cosphi1; \
	int		mode;
#define PJ_LIB__
#include	<projects.h>
PROJ_HEAD(aitoff, "Aitoff") "\n\tMisc Sph";
PROJ_HEAD(wintri, "Winkel Tripel") "\n\tMisc Sph\n\tlat_1";
FORWARD(s_forward); /* spheroid */
	double c, d;

	if((d = acos(cos(lp.phi) * cos(c = 0.5 * lp.lam)))) {/* basic Aitoff */
		xy.x = 2. * d * cos(lp.phi) * sin(c) * (xy.y = 1. / sin(d));
		xy.y *= d * sin(lp.phi);
	} else
		xy.x = xy.y = 0.;
	if (P->mode) { /* Winkel Tripel */
		xy.x = (xy.x + lp.lam * P->cosphi1) * 0.5;
		xy.y = (xy.y + lp.phi) * 0.5;
	}
	return (xy);
}
FREEUP; if (P) pj_dalloc(P); }
	static PJ *
setup(PJ *P) {
	P->inv = 0;
	P->fwd = s_forward;
	P->es = 0.;
	return P;
}
ENTRY0(aitoff)
	P->mode = 0;
ENDENTRY(setup(P))
ENTRY0(wintri)
	P->mode = 1;
	if (pj_param(P->params, "tlat_1").i)
        {
		if ((P->cosphi1 = cos(pj_param(P->params, "rlat_1").f)) == 0.)
			E_ERROR(-22)
        }
	else /* 50d28' or acos(2/pi) */
		P->cosphi1 = 0.636619772367581343;
ENDENTRY(setup(P))
