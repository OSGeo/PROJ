#define PJ_LIB__

#include <stdio.h>
#include <stdlib.h>

#include "proj.h"
#include "proj_internal.h"
#include <math.h>

#include <limits>

#define MAX_ITERATIONS 10
#define TOL 1e-12

PJ_LP nad_cvt(projCtx ctx, PJ_LP in, int inverse, struct CTABLE *ct, int grid_count, PJ_GRIDINFO **tables) {
    PJ_LP t, tb,del, dif;
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

        /* In case we are (or have switched) on the null grid, exit now */
        if( del.lam == 0 && del.phi == 0 )
            break;

        /* We can possibly go outside of the initial guessed grid, so try */
        /* to fetch a new grid into which iterate... */
        if (del.lam == HUGE_VAL)
        {
            if( grid_count == 0 )
                break;
            PJ_LP lp;
            lp.lam = t.lam + ct->ll.lam;
            lp.phi = t.phi + ct->ll.phi;
            auto newCt = find_ctable(ctx, lp, grid_count, tables);
            if( newCt == nullptr || newCt == ct )
                break;
            pj_log(ctx, PJ_LOG_DEBUG_MINOR, "Switching from grid %s to grid %s",
                   ct->id,
                   newCt->id);
            ct = newCt;
            t.lam = lp.lam - ct->ll.lam;
            t.phi = lp.phi - ct->ll.phi;
            tb = in;
            tb.lam -= ct->ll.lam;
            tb.phi -= ct->ll.phi;
            tb.lam = adjlon (tb.lam - M_PI) + M_PI;
            dif.lam = std::numeric_limits<double>::max();
            dif.phi = std::numeric_limits<double>::max();
            continue;
        }

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
