#ifndef lint
static const char SCCSID[]="@(#)nad_cvt.c	4.3   95/09/23 GIE REL";
#endif
#define PJ_LIB__
#include <projects.h>
#define MAX_TRY 9
#define TOL 1e-12
	LP
nad_cvt(LP in, int inverse, struct CTABLE *ct) {
	LP t, tb;

	if (in.lam == HUGE_VAL)
		return in;
	/* normalize input to ll origin */
	tb = in;
	tb.lam -= ct->ll.lam;
	tb.phi -= ct->ll.phi;
	tb.lam = adjlon(tb.lam);
	t = nad_intr(tb, ct);
	if (inverse) {
		LP del, dif;
		int i = MAX_TRY;

		if (t.lam == HUGE_VAL) return t;
		t.lam = tb.lam + t.lam;
		t.phi = tb.phi - t.phi;

		do {
			del = nad_intr(t, ct);
			if (del.lam == HUGE_VAL) return del;
			t.lam -= dif.lam = t.lam - del.lam - tb.lam;
			t.phi -= dif.phi = t.phi + del.phi - tb.phi;
		} while (i-- && fabs(dif.lam) > TOL && fabs(dif.phi) > TOL);
		if (i < 0) {
			t.lam = t.phi = HUGE_VAL;
			return t;
		}
		in.lam = adjlon(t.lam + ct->ll.lam);
		in.phi = t.phi + ct->ll.phi;
	} else {
		if (t.lam == HUGE_VAL)
			in = t;
		else {
			in.lam -= t.lam;
			in.phi += t.phi;
		}
	}
	return in;
}
