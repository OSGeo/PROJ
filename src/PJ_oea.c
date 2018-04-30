#define PJ_LIB__
#include <errno.h>
#include "proj.h"
#include "projects.h"
#include "proj_math.h"

PROJ_HEAD(oea, "Oblated Equal Area") "\n\tMisc Sph\n\tn= m= theta=";

struct pj_opaque {
    double  theta;
    double  m, n;
    double  two_r_m, two_r_n, rm, rn, hm, hn;
    double  cp0, sp0;
};


static XY s_forward (LP lp, PJ *P) {           /* Spheroidal, forward */
    XY xy = {0.0,0.0};
    struct pj_opaque *Q = P->opaque;
    double Az, M, N, cp, sp, cl, shz;

    cp = cos(lp.phi);
    sp = sin(lp.phi);
    cl = cos(lp.lam);
    Az = aatan2(cp * sin(lp.lam), Q->cp0 * sp - Q->sp0 * cp * cl) + Q->theta;
    shz = sin(0.5 * aacos(P->ctx, Q->sp0 * sp + Q->cp0 * cp * cl));
    M = aasin(P->ctx, shz * sin(Az));
    N = aasin(P->ctx, shz * cos(Az) * cos(M) / cos(M * Q->two_r_m));
    xy.y = Q->n * sin(N * Q->two_r_n);
    xy.x = Q->m * sin(M * Q->two_r_m) * cos(N) / cos(N * Q->two_r_n);

    return xy;
}


static LP s_inverse (XY xy, PJ *P) {           /* Spheroidal, inverse */
    LP lp = {0.0,0.0};
    struct pj_opaque *Q = P->opaque;
    double N, M, xp, yp, z, Az, cz, sz, cAz;

    N = Q->hn * aasin(P->ctx,xy.y * Q->rn);
    M = Q->hm * aasin(P->ctx,xy.x * Q->rm * cos(N * Q->two_r_n) / cos(N));
    xp = 2. * sin(M);
    yp = 2. * sin(N) * cos(M * Q->two_r_m) / cos(M);
    cAz = cos(Az = aatan2(xp, yp) - Q->theta);
    z = 2. * aasin(P->ctx, 0.5 * hypot(xp, yp));
    sz = sin(z);
    cz = cos(z);
    lp.phi = aasin(P->ctx, Q->sp0 * cz + Q->cp0 * sz * cAz);
    lp.lam = aatan2(sz * sin(Az),
        Q->cp0 * cz - Q->sp0 * sz * cAz);

    return lp;
}




PJ *PROJECTION(oea) {
    struct pj_opaque *Q = pj_calloc (1, sizeof (struct pj_opaque));
    if (0==Q)
        return pj_default_destructor (P, ENOMEM);
    P->opaque = Q;

    if (((Q->n = pj_param(P->ctx, P->params, "dn").f) <= 0.) ||
        ((Q->m = pj_param(P->ctx, P->params, "dm").f) <= 0.)) {
            return pj_default_destructor(P, PJD_ERR_INVALID_M_OR_N);
    } else {
        Q->theta = pj_param(P->ctx, P->params, "rtheta").f;
        Q->sp0 = sin(P->phi0);
        Q->cp0 = cos(P->phi0);
        Q->rn = 1./ Q->n;
        Q->rm = 1./ Q->m;
        Q->two_r_n = 2. * Q->rn;
        Q->two_r_m = 2. * Q->rm;
        Q->hm = 0.5 * Q->m;
        Q->hn = 0.5 * Q->n;
        P->fwd = s_forward;
        P->inv = s_inverse;
        P->es = 0.;
    }

    return P;
}

