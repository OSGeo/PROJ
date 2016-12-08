/* general forward projection */
#define PJ_LIB__
#include <projects.h>
#include <errno.h>
# define EPS 1.0e-12
	XY /* forward projection entry */
pj_fwd(LP lp, PJ *P) {
	XY xy;
    XY err;
    double t;

    if (0==P->fwd)
        return err;

    /* cannot const-initialize this due to MSVC's broken (non const) HUGE_VAL */
    err.x = err.y = HUGE_VAL;

    P->ctx->last_errno = pj_errno = errno = 0;

    /* Check input coordinates if angular */
    if ((P->left==PJ_IO_UNITS_CLASSIC)||(P->left==PJ_IO_UNITS_RADIANS)) {

        /* check for forward and latitude or longitude overange */
        t = fabs(lp.phi)-M_HALFPI;
        if (t > EPS || fabs(lp.lam) > 10.) {
            pj_ctx_set_errno( P->ctx, -14);
            return err;
        }

        /* Clamp latitude to -90..90 degree range */
        if (fabs(t) <= EPS)
            lp.phi = lp.phi < 0. ? -M_HALFPI : M_HALFPI;
        else if (P->geoc)   /* Maybe redundant and never used. */
            lp.phi = atan(P->rone_es * tan(lp.phi));
        lp.lam -= P->lam0;    /* compute del lp.lam */
        if (!P->over)
            lp.lam = adjlon(lp.lam); /* adjust del longitude */
    }

    /* Do the transformation */
    xy = (*P->fwd)(lp, P);
    if ( P->ctx->last_errno )
        return err;

    /* Classic proj.4 functions return plane coordinates in units of the semimajor axis */
    if (P->right==PJ_IO_UNITS_CLASSIC) {
        xy.x *= P->a;
        xy.y *= P->a;
    }

    /* Handle false eastings/northings and non-metric linear units */
    xy.x = P->fr_meter * (xy.x + P->x0);
    xy.y = P->fr_meter * (xy.y + P->y0);
    /* z is not scaled since this is handled by vto_meter outside */

    return xy;
}
