#define PJ_LIB__
#include <proj.h>
#include <projects.h>
#include <errno.h>
# define EPS 1.0e-12

/* 3D inverse transformation */

LPZ pj_inv3d (XYZ xyz, PJ *P) {
    LPZ lpz;
    LPZ err;
    int last_errno;

    /* cannot const-initialize this due to MSVC's broken (non const) HUGE_VAL */
    err.lam = err.phi = err.z = HUGE_VAL;

    if (0==P->inv3d)
        return err;

    /* can't do as much preliminary checking as with forward */
    if (xyz.x == HUGE_VAL || xyz.y == HUGE_VAL || xyz.z == HUGE_VAL ) {
        pj_ctx_set_errno( P->ctx, -15);
        return err;
    }

    last_errno = proj_errno_reset (P);

    /* de-scale and de-offset */
    /* z is not de-scaled since that is handled by vto_meter before we get here */
    xyz.x = (xyz.x * P->to_meter - P->x0);
    xyz.y = (xyz.y * P->to_meter - P->y0);
    /* Classic proj.4 functions expect plane coordinates in units of the semimajor axis */
    /* Multiplying by ra, rather than dividing by a because the CALCOFI projection      */
    /* stomps on a and hence depends on this */
    if (P->right==PJ_IO_UNITS_CLASSIC) {
        xyz.x *= P->ra;
        xyz.y *= P->ra;
    }

    /* Do inverse transformation */
    lpz = (*P->inv3d) (xyz, P);
    if (P->ctx->last_errno)
        return err;

    if (P->left==PJ_IO_UNITS_RADIANS) {
        /* reduce from del lp.lam */
        lpz.lam += P->lam0;

         /* adjust longitude to central meridian */
        if (!P->over)
            lpz.lam = adjlon(lpz.lam);

        /* This may be redundant and never used */
        if (P->geoc && fabs(fabs(lpz.phi)-M_HALFPI) > EPS)
            lpz.phi = atan(P->one_es * tan(lpz.phi));
    }

    proj_errno_restore (P, last_errno);
    return lpz;
}
