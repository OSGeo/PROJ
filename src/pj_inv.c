/* general inverse projection */
#define PJ_LIB__
#include <proj.h>
#include <projects.h>
#include <errno.h>
# define EPS 1.0e-12

/* inverse projection entry */
LP pj_inv(XY xy, PJ *P) {
    LP lp;
    LP err;
    int last_errno;

    /* cannot const-initialize this due to MSVC's broken (non const) HUGE_VAL */
    err.lam = err.phi = HUGE_VAL;

    if (0==P->inv)
        return err;

    /* can't do as much preliminary checking as with forward */
    if (xy.x == HUGE_VAL || xy.y == HUGE_VAL) {
        pj_ctx_set_errno( P->ctx, -15);
        return err;
    }

    last_errno = proj_errno_reset (P);

    /* de-scale and de-offset */
    xy.x = (xy.x * P->to_meter - P->x0);
    xy.y = (xy.y * P->to_meter - P->y0);

    /* Classic proj.4 functions expect plane coordinates in units of the semimajor axis */
    /* Multiplying by ra, rather than dividing by a because the CALCOFI projection      */
    /* stomps on a and hence depends on this */
    if (P->right==PJ_IO_UNITS_CLASSIC) {
        xy.x *= P->ra;
        xy.y *= P->ra;
    }

    /* Do inverse transformation */
    lp = (*P->inv) (xy, P);
    if (P->ctx->last_errno)
        return err;

    if ((P->left==PJ_IO_UNITS_CLASSIC)||(P->left==PJ_IO_UNITS_RADIANS)) {
        /* reduce from del lp.lam */
        lp.lam += P->lam0;

         /* adjust longitude to central meridian */
        if (!P->over)
            lp.lam = adjlon(lp.lam);

        /* This may be redundant and never used */
        if (P->geoc && fabs(fabs(lp.phi)-M_HALFPI) > EPS)
            lp.phi = atan(P->one_es * tan(lp.phi));
    }

    proj_errno_restore (P, last_errno);
    return lp;
}