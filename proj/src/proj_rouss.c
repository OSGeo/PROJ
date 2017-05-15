/*
** libproj -- library of cartographic projections
**
** Copyright (c) 2003, 2006   Gerald I. Evenden
*/
static const char
LIBPROJ_ID[] = "$Id$";
/*
** Permission is hereby granted, free of charge, to any person obtaining
** a copy of this software and associated documentation files (the
** "Software"), to deal in the Software without restriction, including
** without limitation the rights to use, copy, modify, merge, publish,
** distribute, sublicense, and/or sell copies of the Software, and to
** permit persons to whom the Software is furnished to do so, subject to
** the following conditions:
**
** The above copyright notice and this permission notice shall be
** included in all copies or substantial portions of the Software.
**
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
** EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
** MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
** IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
** CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
** TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
** SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#define PROJ_PARMS__ \
	double s0; \
	double A1, A2, A3, A4, A5, A6; \
	double B1, B2, B3, B4, B5, B6, B7, B8; \
	double C1, C2, C3, C4, C5, C6, C7, C8; \
	double D1, D2, D3, D4, D5, D6, D7, D8, D9, D10, D11; \
	void *en;
#define PJ_LIB__
# include <projects.h>
PROJ_HEAD(rouss, "Roussilhe Stereographic") "\n\tAzi., Ellps.";
FORWARD(e_forward); /* ellipsoid */
	double s, al, cp, sp, al2, s2;
	
	cp = cos(lp.phi);
	sp = sin(lp.phi);
	s = proj_mdist(lp.phi, sp, cp,  P->en) - P->s0;
	s2 = s * s;
	al = lp.lam * cp / sqrt(1. - P->es * sp * sp);
	al2 = al * al;
	xy.x = P->k0 * al*(1.+s2*(P->A1+s2*P->A4)-al2*(P->A2+s*P->A3+s2*P->A5
				+al2*P->A6));
	xy.y = P->k0 * (al2*(P->B1+al2*P->B4)+
		s*(1.+al2*(P->B3-al2*P->B6)+s2*(P->B2+s2*P->B8)+
		s*al2*(P->B5+s*P->B7)));
	return (xy);
}
INVERSE(e_inverse); /* ellipsoid */
	double s, al, x = xy.x / P->k0, y = xy.y / P->k0, x2, y2;;

	x2 = x * x;
	y2 = y * y;
	al = x*(1.-P->C1*y2+x2*(P->C2+P->C3*y-P->C4*x2+P->C5*y2-P->C7*x2*y)
		+y2*(P->C6*y2-P->C8*x2*y));
	s = P->s0 + y*(1.+y2*(-P->D2+P->D8*y2))+
		x2*(-P->D1+y*(-P->D3+y*(-P->D5+y*(-P->D7+y*P->D11)))+
		x2*(P->D4+y*(P->D6+y*P->D10)-x2*P->D9));
	lp.phi=proj_inv_mdist(P->ctx, s, P->en);
	s = sin(lp.phi);
	lp.lam=al * sqrt(1. - P->es * s * s)/cos(lp.phi);
	return (lp);
}
FREEUP;
	if (P) {
		if (P->en)
			free(P->en);
		free(P);
	}
}
ENTRY1(rouss, en)
	double N0, es2, t, t2, R_R0_2, R_R0_4;

	if (!((P->en = proj_mdist_ini(P->es))))
		E_ERROR_0;
	es2 = sin(P->phi0);
	P->s0 = proj_mdist(P->phi0, es2, cos(P->phi0), P->en);
	t = 1. - (es2 = P->es * es2 * es2);
	N0 = 1./sqrt(t);
	R_R0_2 = t * t / P->one_es;
	R_R0_4 = R_R0_2 * R_R0_2;
	t = tan(P->phi0);
	t2 = t * t;
	P->C1 = P->A1 = R_R0_2 / 4.;
	P->C2 = P->A2 = R_R0_2 * (2 * t2 - 1. - 2. * es2) / 12.;
	P->A3 = R_R0_2 * t * (1. + 4. * t2)/ ( 12. * N0);
	P->A4 = R_R0_4 / 24.;
	P->A5 = R_R0_4 * ( -1. + t2 * (11. + 12. * t2))/24.;
	P->A6 = R_R0_4 * ( -2. + t2 * (11. - 2. * t2))/240.;
	P->B1 = t / (2. * N0);
	P->B2 = R_R0_2 / 12.;
	P->B3 = R_R0_2 * (1. + 2. * t2 - 2. * es2)/4.;
	P->B4 = R_R0_2 * t * (2. - t2)/(24. * N0);
	P->B5 = R_R0_2 * t * (5. + 4.* t2)/(8. * N0);
	P->B6 = R_R0_4 * (-2. + t2 * (-5. + 6. * t2))/48.;
	P->B7 = R_R0_4 * (5. + t2 * (19. + 12. * t2))/24.;
	P->B8 = R_R0_4 / 120.;
	P->C3 = R_R0_2 * t * (1. + t2)/(3. * N0);
	P->C4 = R_R0_4 * (-3. + t2 * (34. + 22. * t2))/240.;
	P->C5 = R_R0_4 * (4. + t2 * (13. + 12. * t2))/24.;
	P->C6 = R_R0_4 / 16.;
	P->C7 = R_R0_4 * t * (11. + t2 * (33. + t2 * 16.))/(48. * N0);
	P->C8 = R_R0_4 * t * (1. + t2 * 4.)/(36. * N0);
	P->D1 = t / (2. * N0);
	P->D2 = R_R0_2 / 12.;
	P->D3 = R_R0_2 * (2 * t2 + 1. - 2. * es2) / 4.;
	P->D4 = R_R0_2 * t * (1. + t2)/(8. * N0);
	P->D5 = R_R0_2 * t * (1. + t2 * 2.)/(4. * N0);
	P->D6 = R_R0_4 * (1. + t2 * (6. + t2 * 6.))/16.;
	P->D7 = R_R0_4 * t2 * (3. + t2 * 4.)/8.;
	P->D8 = R_R0_4 / 80.;
	P->D9 = R_R0_4 * t * (-21. + t2 * (178. - t2 * 26.))/720.;
	P->D10 = R_R0_4 * t * (29. + t2 * (86. + t2 * 48.))/(96. * N0);
	P->D11 = R_R0_4 * t * (37. + t2 * 44.)/(96. * N0);
	P->fwd = e_forward;
	P->inv = e_inverse;
	ENDENTRY(P)
