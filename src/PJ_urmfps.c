#define PJ_LIB__

#include <errno.h>
#include <math.h>

#include "proj.h"
#include "projects.h"

PROJ_HEAD(urmfps, "Urmaev Flat-Polar Sinusoidal") "\n\tPCyl, Sph.\n\tn=";
PROJ_HEAD(wag1, "Wagner I (Kavraisky VI)") "\n\tPCyl, Sph.";

struct pj_opaque {
    double  n, C_y;
};

#define C_x 0.8773826753
#define Cy 1.139753528477


static XY s_forward (LP lp, PJ *P) {           /* Spheroidal, forward */
    XY xy = {0.0, 0.0};
    lp.phi = aasin (P->ctx,P->opaque->n * sin (lp.phi));
    xy.x = C_x * lp.lam * cos (lp.phi);
    xy.y = P->opaque->C_y * lp.phi;
    return xy;
}


static LP s_inverse (XY xy, PJ *P) {           /* Spheroidal, inverse */
    LP lp = {0.0, 0.0};
    xy.y /= P->opaque->C_y;
    lp.phi = aasin(P->ctx, sin (xy.y) / P->opaque->n);
    lp.lam = xy.x / (C_x * cos (xy.y));
    return lp;
}


static PJ *setup(PJ *P) {
    P->opaque->C_y = Cy / P->opaque->n;
    P->es = 0.;
    P->inv = s_inverse;
    P->fwd = s_forward;
    return P;
}


PJ *PROJECTION(urmfps) {
    struct pj_opaque *Q = pj_calloc (1, sizeof (struct pj_opaque));
    if (0==Q)
        return pj_default_destructor(P, ENOMEM);

    P->opaque = Q;

    if (pj_param(P->ctx, P->params, "tn").i) {
        P->opaque->n = pj_param(P->ctx, P->params, "dn").f;
        if (P->opaque->n <= 0. || P->opaque->n > 1.)
            return pj_default_destructor(P, PJD_ERR_N_OUT_OF_RANGE);
    } else {
        return pj_default_destructor(P, PJD_ERR_N_OUT_OF_RANGE);
    }

    return setup(P);
}


PJ *PROJECTION(wag1) {
    struct pj_opaque *Q = pj_calloc (1, sizeof (struct pj_opaque));
    if (0==Q)
        return pj_default_destructor(P, ENOMEM);
    P->opaque = Q;

    P->opaque->n = 0.8660254037844386467637231707;
    return setup(P);
}
