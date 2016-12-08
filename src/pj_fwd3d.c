#define PJ_LIB__
#include <projects.h>
#include <errno.h>
# define EPS 1.0e-12

/* 3D Forward transformation */

XYZ pj_fwd3d(LPZ lpz, PJ *P) {
    XYZ xyz;
    XYZ err;
    double t;

    if (0==P->fwd3d)
        return err;

    /* cannot const-initialize this due to MSVC's broken (non const) HUGE_VAL */
    err.x = err.y = err.z = HUGE_VAL;

    P->ctx->last_errno = pj_errno = errno = 0;

    /* Check input coordinates if angular */
    if ((P->left==PJ_IO_UNITS_CLASSIC)||(P->left==PJ_IO_UNITS_RADIANS)) {

        /* check for forward and latitude or longitude overange */
        t = fabs(lpz.phi)-M_HALFPI;
        if (t > EPS || fabs(lpz.lam) > 10.) {
            pj_ctx_set_errno( P->ctx, -14);
            return err;
        }

        /* Clamp latitude to -90..90 degree range */
        if (fabs(t) <= EPS)
            lpz.phi = lpz.phi < 0. ? -M_HALFPI : M_HALFPI;
        else if (P->geoc)   /* Maybe redundant and never used. */
            lpz.phi = atan(P->rone_es * tan(lpz.phi));
        lpz.lam -= P->lam0;    /* compute del lp.lam */
        if (!P->over)
            lpz.lam = adjlon(lpz.lam); /* adjust del longitude */
    }

    /* Do the transformation */
    xyz = (*P->fwd3d)(lpz, P);
    if ( P->ctx->last_errno )
        return err;

    /* Classic proj.4 functions return plane coordinates in units of the semimajor axis */
    if (P->right==PJ_IO_UNITS_CLASSIC) {
        xyz.x *= P->a;
        xyz.y *= P->a;
    }

    /* Handle false eastings/northings and non-metric linear units */
    xyz.x = P->fr_meter * (xyz.x + P->x0);
    xyz.y = P->fr_meter * (xyz.y + P->y0);
    /* z is not scaled since this is handled by vto_meter outside */

    return xyz;
}
