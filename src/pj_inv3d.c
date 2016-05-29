/* general inverse projection */
#define PJ_LIB__
#include <projects.h>
#include <errno.h>
# define EPS 1.0e-12

static LPZ lpz_error = {HUGE_VAL, HUGE_VAL, HUGE_VAL};

LPZ pj_inv3d(XYZ xyz, PJ *P) {
	LPZ lpz;

	if (0==P->fwd3d)
    	return lpz_error;

	if (pj_is_isomorphic (P))
	    return  (*P->inv3d)(xyz, P);

	/* can't do as much preliminary checking as with forward */
	if (xyz.x == HUGE_VAL || xyz.y == HUGE_VAL || xyz.z == HUGE_VAL ) {
		lpz.lam = lpz.phi = lpz.z = HUGE_VAL;
		pj_ctx_set_errno( P->ctx, -15);
                return lpz;
	}

	errno = pj_errno = 0;
    P->ctx->last_errno = 0;

	xyz.x = (xyz.x * P->to_meter - P->x0) * P->ra; /* descale and de-offset */
	xyz.y = (xyz.y * P->to_meter - P->y0) * P->ra;
    /* z is not scaled since that is handled by vto_meter before we get here */

    lpz = (*P->inv3d)(xyz, P); /* inverse project */
    if (P->ctx->last_errno )
	    return lpz_error;

	lpz.lam += P->lam0; /* reduce from del lp.lam */

	if (!P->over)
		lpz.lam = adjlon(lpz.lam); /* adjust longitude to CM */

    /* This may be redundant and never used */
	if (P->geoc && fabs(fabs(lpz.phi)-HALFPI) > EPS)
		lpz.phi = atan(P->one_es * tan(lpz.phi));

	return lpz;
}
