#define PJ_LIB__

#include <errno.h>
#include <math.h>

#include "proj.h"
#include "proj_internal.h"

PROJ_HEAD(urmfps, "Urmaev Flat-Polar Sinusoidal") "\n\tPCyl, Sph\n\tn=";
PROJ_HEAD(wag1, "Wagner I (Kavraisky VI)") "\n\tPCyl, Sph";

namespace { // anonymous namespace
struct pj_opaque {
    double  n, C_y;
};
} // anonymous namespace

#define C_x 0.8773826753
#define Cy 1.139753528477


static PJ_XY urmfps_s_forward (PJ_LP lp, PJ *P) {           /* Spheroidal, forward */
    PJ_XY xy = {0.0, 0.0};
    lp.phi = aasin (P->ctx,static_cast<struct pj_opaque*>(P->opaque)->n * sin (lp.phi));
    xy.x = C_x * lp.lam * cos (lp.phi);
    xy.y = static_cast<struct pj_opaque*>(P->opaque)->C_y * lp.phi;
    return xy;
}


static PJ_LP urmfps_s_inverse (PJ_XY xy, PJ *P) {           /* Spheroidal, inverse */
    PJ_LP lp = {0.0, 0.0};
    xy.y /= static_cast<struct pj_opaque*>(P->opaque)->C_y;
    lp.phi = aasin(P->ctx, sin (xy.y) / static_cast<struct pj_opaque*>(P->opaque)->n);
    lp.lam = xy.x / (C_x * cos (xy.y));
    return lp;
}


static PJ *setup(PJ *P) {
    static_cast<struct pj_opaque*>(P->opaque)->C_y = Cy / static_cast<struct pj_opaque*>(P->opaque)->n;
    P->es = 0.;
    P->inv = urmfps_s_inverse;
    P->fwd = urmfps_s_forward;
    return P;
}


PJ *PROJECTION(urmfps) {
    struct pj_opaque *Q = static_cast<struct pj_opaque*>(calloc (1, sizeof (struct pj_opaque)));
    if (nullptr==Q)
        return pj_default_destructor(P, ENOMEM);

    P->opaque = Q;

    if (pj_param(P->ctx, P->params, "tn").i) {
        static_cast<struct pj_opaque*>(P->opaque)->n = pj_param(P->ctx, P->params, "dn").f;
        if (static_cast<struct pj_opaque*>(P->opaque)->n <= 0. || static_cast<struct pj_opaque*>(P->opaque)->n > 1.)
            return pj_default_destructor(P, PJD_ERR_N_OUT_OF_RANGE);
    } else {
        return pj_default_destructor(P, PJD_ERR_N_OUT_OF_RANGE);
    }

    return setup(P);
}


PJ *PROJECTION(wag1) {
    struct pj_opaque *Q = static_cast<struct pj_opaque*>(calloc (1, sizeof (struct pj_opaque)));
    if (nullptr==Q)
        return pj_default_destructor(P, ENOMEM);
    P->opaque = Q;

    static_cast<struct pj_opaque*>(P->opaque)->n = 0.8660254037844386467637231707;
    return setup(P);
}
