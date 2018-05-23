#define PJ_LIB__

#include <errno.h>
#include <math.h>

#include "proj.h"
#include "projects.h"

PROJ_HEAD(goode, "Goode Homolosine") "\n\tPCyl, Sph.";

#define Y_COR   0.05280
#define PHI_LIM 0.71093078197902358062

C_NAMESPACE PJ *pj_sinu(PJ *), *pj_moll(PJ *);

struct pj_opaque {
    PJ *sinu;
    PJ *moll;
};


static XY s_forward (LP lp, PJ *P) {           /* Spheroidal, forward */
    XY xy;
    struct pj_opaque *Q = P->opaque;

    if (fabs(lp.phi) <= PHI_LIM)
        xy = Q->sinu->fwd(lp, Q->sinu);
    else {
        xy = Q->moll->fwd(lp, Q->moll);
        xy.y -= lp.phi >= 0.0 ? Y_COR : -Y_COR;
    }
    return xy;
}


static LP s_inverse (XY xy, PJ *P) {           /* Spheroidal, inverse */
    LP lp;
    struct pj_opaque *Q = P->opaque;

    if (fabs(xy.y) <= PHI_LIM)
        lp = Q->sinu->inv(xy, Q->sinu);
    else {
        xy.y += xy.y >= 0.0 ? Y_COR : -Y_COR;
        lp = Q->moll->inv(xy, Q->moll);
    }
    return lp;
}


static void *destructor (PJ *P, int errlev) {              /* Destructor */
    if (0==P)
        return 0;
    if (0==P->opaque)
        return pj_default_destructor (P, errlev);
    pj_free (P->opaque->sinu);
    pj_free (P->opaque->moll);
    return pj_default_destructor (P, errlev);
}



PJ *PROJECTION(goode) {
    struct pj_opaque *Q = pj_calloc (1, sizeof (struct pj_opaque));
    if (0==Q)
        return pj_default_destructor (P, ENOMEM);
    P->opaque = Q;
    P->destructor = destructor;

    P->es = 0.;
    if (!(Q->sinu = pj_sinu(0)) || !(Q->moll = pj_moll(0)))
        return destructor (P, ENOMEM);
    Q->sinu->es = 0.;
    Q->sinu->ctx = P->ctx;
    Q->moll->ctx = P->ctx;
    if (!(Q->sinu = pj_sinu(Q->sinu)) || !(Q->moll = pj_moll(Q->moll)))
        return destructor (P, ENOMEM);

    P->fwd = s_forward;
    P->inv = s_inverse;

    return P;
}
