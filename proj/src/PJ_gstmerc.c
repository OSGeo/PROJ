#ifndef lint
static const char SCCSID[]="@(#)PJ_gstmerc.c	4.6	08/07/28	DGR	IGN";
#endif
#define PROJ_PARMS__ \
	double lamc;\
	double phic;\
	double c;\
	double n1;\
	double n2;\
    double XS;\
    double YS;

#define PJ_LIB__
# include	<projects.h>
PROJ_HEAD(gstmerc, "Gauss-Schreiber Transverse Mercator (aka Gauss-Laborde Reunion)")
	"\n\tCyl, Sph&Ell\n\tlat_0= lon_0= k_0=";
FORWARD(s_forward); /* spheroid */
	double L, Ls, sinLs1, Ls1;
	L= P->n1*lp.lam;
    Ls= P->c+P->n1*log(pj_tsfn(-1.0*lp.phi,-1.0*sin(lp.phi),P->e));
    sinLs1= sin(L)/cosh(Ls);
    Ls1= log(pj_tsfn(-1.0*asin(sinLs1),0.0,0.0));
    xy.x= (P->XS + P->n2*Ls1)*P->ra;
    xy.y= (P->YS + P->n2*atan(sinh(Ls)/cos(L)))*P->ra;
	return (xy);
}
INVERSE(s_inverse); /* spheroid */
	double L, LC, sinC;
	L= atan(sinh((xy.x - P->XS)*P->a/P->n2)/cos((xy.y - P->YS)*P->a/P->n2));
    sinC= sin((xy.y - P->YS)*P->a/P->n2)/cosh((xy.x - P->XS)*P->a/P->n2);
    LC= log(pj_tsfn(-1.0*asin(sinC),0.0,0.0));
    lp.lam= L/P->n1;
    lp.phi= -1.0*pj_phi2(exp((LC-P->c)/P->n1),P->e);
	return (lp);
}
FREEUP; if (P) pj_dalloc(P); }
ENTRY0(gstmerc)
    P->n1= sqrt(1.0+P->es*pow(cos(P->phi0),4.0)/(1.0-P->es));
    P->phic= asin(sin(P->phi0)/P->n1);
    P->c=       log(pj_tsfn(-1.0*P->phic,0.0,0.0))
         -P->n1*log(pj_tsfn(-1.0*P->phi0,-1.0*sin(P->phi0),P->e));
    P->n2= P->k0*P->a*sqrt(1.0-P->es)/(1.0-P->es*sin(P->phi0)*sin(P->phi0));
    P->XS= 0;
    P->YS= -1.0*P->n2*P->phic;
	P->inv= s_inverse;
	P->fwd= s_forward;
ENDENTRY(P)
