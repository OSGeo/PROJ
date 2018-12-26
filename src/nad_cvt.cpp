#define PJ_LIB__

#include <stdio.h>
#include <stdlib.h>

#include "projects.h"
#include "proj_math.h"

#define MAX_ITERATIONS 10
#define TOL 1e-12

LP nad_cvt(LP in, int inverse, struct CTABLE *ct) {
    LP t, tb,del, dif;
    int i = MAX_ITERATIONS;
    const double toltol = TOL*TOL;

    if (in.lam == HUGE_VAL)
        return in;

    /* normalize input to ll origin */
    tb = in;
    tb.lam -= ct->ll.lam;
    tb.phi -= ct->ll.phi;
    tb.lam = adjlon (tb.lam - M_PI) + M_PI;

    t = nad_intr (tb, ct);
    if (t.lam == HUGE_VAL)
        return t;

    if (!inverse) {
        in.lam -= t.lam;
        in.phi += t.phi;
        return in;
    }

    t.lam = tb.lam + t.lam;
    t.phi = tb.phi - t.phi;

    do {
        del = nad_intr(t, ct);

        /* This case used to return failure, but I have
           changed it to return the first order approximation
           of the inverse shift.  This avoids cases where the
           grid shift *into* this grid came from another grid.
           While we aren't returning optimally correct results
           I feel a close result in this case is better than
           no result.  NFW
           To demonstrate use -112.5839956 49.4914451 against
           the NTv2 grid shift file from Canada. */
        if (del.lam == HUGE_VAL)
            break;

        dif.lam = t.lam - del.lam - tb.lam;
           dif.phi = t.phi + del.phi - tb.phi;
        t.lam -= dif.lam;
        t.phi -= dif.phi;

    } while (--i && (dif.lam*dif.lam + dif.phi*dif.phi > toltol)); /* prob. slightly faster than hypot() */

    if (i==0) {
        /* If we had access to a context, this should go through pj_log, and we should set ctx->errno */
        if (getenv ("PROJ_DEBUG"))
            fprintf( stderr, "Inverse grid shift iterator failed to converge.\n" );
        t.lam = t.phi = HUGE_VAL;
        return t;
    }

    /* and again: pj_log and ctx->errno */
    if (del.lam==HUGE_VAL && getenv ("PROJ_DEBUG"))
        fprintf (stderr, "Inverse grid shift iteration failed, presumably at grid edge.\nUsing first approximation.\n");

    in.lam = adjlon (t.lam + ct->ll.lam);
    in.phi = t.phi + ct->ll.phi;
    return in;
}
