#define PJ_LIB__
#include <projects.h>
#include <errno.h>
# define EPS 1.0e-12

/* 3D inverse transformation */

LPZ pj_inv3d (XYZ xyz, PJ *P) {
    LPZ lpz;
    static const LPZ err = {HUGE_VAL, HUGE_VAL, HUGE_VAL};

    if (0==P->inv3d)
        return err;

    /* can't do as much preliminary checking as with forward */
    if (xyz.x == HUGE_VAL || xyz.y == HUGE_VAL || xyz.z == HUGE_VAL ) {
        pj_ctx_set_errno( P->ctx, -15);
        return err;
    }

    P->ctx->last_errno = errno = pj_errno = 0;

    /* descale and de-offset */
    /* z is not scaled since that is handled by vto_meter before we get here */
    xyz.x = (xyz.x * P->to_meter - P->x0);
    xyz.y = (xyz.y * P->to_meter - P->y0);
    if (P->right==PJ_IO_UNITS_CLASSIC) {
        xyz.x /= P->a;
        xyz.y /= P->a;
    }

    /* Do inverse transformation */
    lpz = (*P->inv3d) (xyz, P);
    if (P->ctx->last_errno)
        return err;

    /* reduce from del lp.lam */
    lpz.lam += P->lam0;

     /* adjust longitude to central meridian */
    if (!P->over)
        lpz.lam = adjlon(lpz.lam);

    /* This may be redundant and never used */
    if (P->geoc && fabs(fabs(lpz.phi)-M_HALFPI) > EPS)
        lpz.phi = atan(P->one_es * tan(lpz.phi));

    return lpz;
}
