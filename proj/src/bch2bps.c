/* convert bivariate w Chebyshev series to w Power series */
#ifndef lint
static const char SCCSID[]="@(#)bch2bps.c	4.5	94/03/22	GIE	REL";
#endif
#include <projects.h>
/* basic support procedures */
	static void /* clear vector to zero */
clear(UV *p, int n) { static const UV c = {0., 0.}; while (n--) *p++ = c; }
	static void /* clear matrix rows to zero */
bclear(UV **p, int n, int m) { while (n--) clear(*p++, m); }
	static void /* move vector */
bmove(UV *a, UV *b, int n) { while (n--) *a++ = *b++; }
	static void /* a <- m * b - c */
submop(UV *a, double m, UV *b, UV *c, int n) {
	while (n--) {
		a->u = m * b->u - c->u;
		a++->v = m * b++->v - c++->v;
	}
}
	static void /* a <- b - c */
subop(UV *a, UV *b, UV *c, int n) {
	while (n--) {
		a->u = b->u - c->u;
		a++->v = b++->v - c++->v;
	}
}
	static void /* multiply vector a by scalar m */
dmult(UV *a, double m, int n) { while(n--) { a->u *= m; a->v *= m; ++a; } }
	static void /* row adjust a[] <- a[] - m * b[] */
dadd(UV *a, UV *b, double m, int n) {
	while(n--) {
		a->u -= m * b->u;
		a++->v -= m * b++->v;
	}
}
	static void /* convert row to pover series */
rows(UV *c, UV *d, int n) {
	UV sv, *dd;
	int j, k;

	dd = (UV *)vector1(n-1, sizeof(UV));
	sv.u = sv.v = 0.;
	for (j = 0; j < n; ++j) d[j] = dd[j] = sv;
	d[0] = c[n-1];
	for (j = n-2; j >= 1; --j) {
		for (k = n-j; k >= 1; --k) {
			sv = d[k];
			d[k].u = 2. * d[k-1].u - dd[k].u;
			d[k].v = 2. * d[k-1].v - dd[k].v;
			dd[k] = sv;
		}
		sv = d[0];
		d[0].u = -dd[0].u + c[j].u;
		d[0].v = -dd[0].v + c[j].v;
		dd[0] = sv;
	}
	for (j = n-1; j >= 1; --j) {
		d[j].u = d[j-1].u - dd[j].u;
		d[j].v = d[j-1].v - dd[j].v;
	}
	d[0].u = -dd[0].u + .5 * c[0].u;
	d[0].v = -dd[0].v + .5 * c[0].v;
	pj_dalloc(dd);
}
	static void /* convert columns to power series */
cols(UV **c, UV **d, int nu, int nv) {
	UV *sv, **dd;
	int j, k;

	dd = (UV **)vector2(nu, nv, sizeof(UV));
	sv = (UV *)vector1(nv, sizeof(UV));
	bclear(d, nu, nv);
	bclear(dd, nu, nv);
	bmove(d[0], c[nu-1], nv);
	for (j = nu-2; j >= 1; --j) {
		for (k = nu-j; k >= 1; --k) {
			bmove(sv, d[k], nv);
			submop(d[k], 2., d[k-1], dd[k], nv);
			bmove(dd[k], sv, nv);
		}
		bmove(sv, d[0], nv);
		subop(d[0], c[j], dd[0], nv);
		bmove(dd[0], sv, nv);
	}
	for (j = nu-1; j >= 1; --j)
		subop(d[j], d[j-1], dd[j], nv);
	submop(d[0], .5, c[0], dd[0], nv);
	freev2(dd, nu);
	pj_dalloc(sv);
}
	static void /* row adjust for range -1 to 1 to a to b */
rowshft(double a, double b, UV *d, int n) {
	int k, j;
	double fac, cnst;

	cnst = 2. / (b - a);
	fac = cnst;
	for (j = 1; j < n; ++j) {
		d[j].u *= fac;
		d[j].v *= fac;
		fac *= cnst;
	}
	cnst = .5 * (a + b);
	for (j = 0; j <= n-2; ++j)
		for (k = n - 2; k >= j; --k) {
			d[k].u -= cnst * d[k+1].u;
			d[k].v -= cnst * d[k+1].v;
		}
}
	static void /* column adjust for range -1 to 1 to a to b */
colshft(double a, double b, UV **d, int n, int m) {
	int k, j;
	double fac, cnst;

	cnst = 2. / (b - a);
	fac = cnst;
	for (j = 1; j < n; ++j) {
		dmult(d[j], fac, m);
		fac *= cnst;
	}
	cnst = .5 * (a + b);
	for (j = 0; j <= n-2; ++j)
		for (k = n - 2; k >= j; --k)
			dadd(d[k], d[k+1], cnst, m);
}
	int /* entry point */
bch2bps(UV a, UV b, UV **c, int nu, int nv) {
	UV **d;
	int i;

	if (nu < 1 || nv < 1 || !(d = (UV **)vector2(nu, nv, sizeof(UV))))
		return 0;
	/* do rows to power series */
	for (i = 0; i < nu; ++i) {
		rows(c[i], d[i], nv);
		rowshft(a.v, b.v, d[i], nv);
	}
	/* do columns to power series */
	cols(d, c, nu, nv);
	colshft(a.u, b.u, c, nu, nv);
	freev2(d, nu);
	return 1;
}
