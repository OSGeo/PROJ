/* generates 'T' option output */
#define PJ_LIB__
#include "projects.h"
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "emess.h"
#ifndef COEF_LINE_MAX
#define COEF_LINE_MAX 50
#endif

/* FIXME: put the declaration in a header. Also used in proj.c */
void gen_cheb(int inverse, projUV (*proj)(projUV), char *s, PJ *P,
              int iargc, char **iargv);
extern void p_series(Tseries *, FILE *, char *);

void gen_cheb(int inverse, projUV (*proj)(projUV), char *s, PJ *P,
              int iargc, char **iargv) {
	long NU = 15, NV = 15;
	int errin = 0, pwr;
	long res = -1;
	char *arg, fmt[32];
	projUV low, upp, resid;
	Tseries *F;
	double (*input)(const char *, char **);

	input = inverse ? strtod : dmstor;
	if (*s) low.u = input(s, &s); else { low.u = 0; ++errin; }
	if (*s == ',') upp.u = input(s+1, &s); else { upp.u = 0; ++errin; }
	if (*s == ',') low.v = input(s+1, &s); else { low.v = 0; ++errin; }
	if (*s == ',') upp.v = input(s+1, &s); else { upp.v = 0; ++errin; }
	if (errin)
		emess(16,"null or absent -T parameters");
	if (*s == ',') if (*++s != ',') res = strtol(s, &s, 10);
	if (*s == ',') if (*++s != ',') NU = strtol(s, &s, 10);
	if (*s == ',') if (*++s != ',') NV = strtol(s, &s, 10);
	pwr = s && *s && !strcmp(s, ",P");
	(void)printf("#proj_%s\n#    run-line:\n",
		pwr ? "Power" : "Chebyshev");
	if (iargc > 0) { /* proj execution audit trail */
		int n = 0, L;

		for ( ; iargc ; --iargc) {
			arg = *iargv++;
			if (*arg != '+') {
				if (!n) { putchar('#'); ++n; }
				(void)printf(" %s%n",arg, &L);
				if ((n += L) > COEF_LINE_MAX) { putchar('\n'); n = 0; }
			}
		}
		if (n) putchar('\n');
	}
	(void)printf("# projection parameters\n");
	pj_pr_list(P);
	if (low.u == upp.u || low.v >= upp.v)
		emess(16,"approx. argument range error");
	if (low.u > upp.u)
		low.u -= M_TWOPI;
	if (NU < 2 || NV < 2 || NU > INT_MAX || NV > INT_MAX)
		emess(16,"approx. work dimensions (%ld %ld) too small or large",NU,NV);
	if (!(F = mk_cheby(low, upp, pow(10., (double)res)*.5, &resid, proj,
		(int)NU, (int)NV, pwr)))
		emess(16,"generation of approx failed\nreason: %s\n",
			pj_strerrno(errno));
	(void)printf("%c,%.12g,%.12g,%.12g,%.12g,%.12g\n",inverse?'I':'F',
		P->lam0*RAD_TO_DEG,
		low.u*(inverse?1.:RAD_TO_DEG),upp.u*(inverse?1.:RAD_TO_DEG),
		low.v*(inverse?1.:RAD_TO_DEG),upp.v*(inverse?1.:RAD_TO_DEG));
	if (pwr)
		strcpy(fmt, "%.15g");
	else if (res <= 0)
		(void)sprintf(fmt,"%%.%ldf",-res+1);
	else
		(void)strcpy(fmt,"%.0f");
	p_series(F, stdout, fmt);
	(void)printf("# |u,v| sums %g %g\n#end_proj_%s\n",
		resid.u, resid.v, pwr ? "Power" : "Chebyshev");
}
