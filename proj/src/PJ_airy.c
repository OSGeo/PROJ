#ifndef lint
static const char SCCSID[]="@(#)PJ_airy.c	4.1	94/02/15	GIE	REL";
#endif
#define PROJ_PARMS__ \
	double	p_halfpi; \
	double	sinph0; \
	double	cosph0; \
	double	Cb; \
	int		mode; \
	int		no_cut;	/* do not cut at hemisphere limit */
#define PJ_LIB__
#include	<projects.h>
PROJ_HEAD(airy, "Airy") "\n\tMisc Sph, no inv.\n\tno_cut lat_b=";
# define EPS 1.e-10
# define N_POLE	0
# define S_POLE 1
# define EQUIT	2
# define OBLIQ	3
FORWARD(s_forward); /* spheroid */
	double  sinlam, coslam, cosphi, sinphi, t, s, Krho, cosz;

	sinlam = sin(lp.lam);
	coslam = cos(lp.lam);
	switch (P->mode) {
	case EQUIT:
	case OBLIQ:
		sinphi = sin(lp.phi);
		cosphi = cos(lp.phi);
		cosz = cosphi * coslam;
		if (P->mode == OBLIQ)
			cosz = P->sinph0 * sinphi + P->cosph0 * cosz;
		if (!P->no_cut && cosz < -EPS)
			F_ERROR;
		if (fabs(s = 1. - cosz) > EPS) {
			t = 0.5 * (1. + cosz);
			Krho = -log(t)/s - P->Cb / t;
		} else
			Krho = 0.5 - P->Cb;
		xy.x = Krho * cosphi * sinlam;
		if (P->mode == OBLIQ)
			xy.y = Krho * (P->cosph0 * sinphi -
				P->sinph0 * cosphi * coslam);
		else
			xy.y = Krho * sinphi;
		break;
	case S_POLE:
	case N_POLE:
		lp.phi = fabs(P->p_halfpi - lp.phi);
		if (!P->no_cut && (lp.phi - EPS) > HALFPI)
			F_ERROR;
		if ((lp.phi *= 0.5) > EPS) {
			t = tan(lp.phi);
			Krho = -2.*(log(cos(lp.phi)) / t + t * P->Cb);
			xy.x = Krho * sinlam;
			xy.y = Krho * coslam;
			if (P->mode == N_POLE)
				xy.y = -xy.y;
		} else
			xy.x = xy.y = 0.;
	}
	return (xy);
}
FREEUP; if (P) pj_dalloc(P); }
ENTRY0(airy)
	double beta;

	P->no_cut = pj_param(P->params, "bno_cut").i;
	beta = 0.5 * (HALFPI - pj_param(P->params, "rlat_b").f);
	if (fabs(beta) < EPS)
		P->Cb = -0.5;
	else {
		P->Cb = 1./tan(beta);
		P->Cb *= P->Cb * log(cos(beta));
	}
	if (fabs(fabs(P->phi0) - HALFPI) < EPS)
		if (P->phi0 < 0.) {
			P->p_halfpi = -HALFPI;
			P->mode = S_POLE;
		} else {
			P->p_halfpi =  HALFPI;
			P->mode = N_POLE;
		}
	else {
		if (fabs(P->phi0) < EPS)
			P->mode = EQUIT;
		else {
			P->mode = OBLIQ;
			P->sinph0 = sin(P->phi0);
			P->cosph0 = cos(P->phi0);
		}
	}
	P->fwd = s_forward;
	P->es = 0.;
ENDENTRY(P)
