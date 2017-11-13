/*****************************************************************************

               Lambert Conformal Conic Alternative
               -----------------------------------

    This is Gerald Evenden's 2003 implementation of an alternative
    "almost" LCC, which has been in use historically, but which
    should NOT be used for new projects - i.e: use this implementation
    if you need interoperability with old data represented in this
    projection, but not in any other case.

    The code was originally discussed on the PROJ.4 mailing list in
    a thread archived over at

    http://lists.maptools.org/pipermail/proj/2003-March/000644.html

    It was discussed again in the thread starting at

    http://lists.maptools.org/pipermail/proj/2017-October/007828.html
        and continuing at
    http://lists.maptools.org/pipermail/proj/2017-November/007831.html

    which prompted Clifford J. Mugnier to add these clarifying notes:

    The French Army Truncated Cubic Lambert (partially conformal) Conic
    projection is the Legal system for the projection in France between
    the late 1800s and 1948 when the French Legislature changed the law
    to recognize the fully conformal version.

    It was (might still be in one or two North African prior French
    Colonies) used in North Africa in Algeria, Tunisia, & Morocco, as
    well as in Syria during the Levant.

    Last time I have seen it used was about 30+ years ago in
    Algeria when it was used to define Lease Block boundaries for
    Petroleum Exploration & Production.

    (signed)

    Clifford J. Mugnier, c.p., c.m.s.
    Chief of Geodesy
    LSU Center for GeoInformatics
    Dept. of Civil Engineering
    LOUISIANA STATE UNIVERSITY

*****************************************************************************/

#define PJ_LIB__
#include <errno.h>
#include <proj.h>
#include "projects.h"

PROJ_HEAD(lcca, "Lambert Conformal Conic Alternative")
    "\n\tConic, Sph&Ell\n\tlat_0=";

#define MAX_ITER 10
#define DEL_TOL 1e-12

struct pj_opaque {
    double *en;
    double r0, l, M0;
    double C;
};


static double fS(double S, double C) {        /* func to compute dr */

    return S * ( 1. + S * S * C);
}


static double fSp(double S, double C) {       /* deriv of fs */

    return 1. + 3.* S * S * C;
}


static XY e_forward (LP lp, PJ *P) {          /* Ellipsoidal, forward */
    XY xy = {0.0,0.0};
    struct pj_opaque *Q = P->opaque;
    double S, r, dr;

    S = pj_mlfn(lp.phi, sin(lp.phi), cos(lp.phi), Q->en) - Q->M0;
    dr = fS(S, Q->C);
    r = Q->r0 - dr;
    xy.x = P->k0 * (r * sin( lp.lam *= Q->l ) );
    xy.y = P->k0 * (Q->r0 - r * cos(lp.lam) );
    return xy;
}


static LP e_inverse (XY xy, PJ *P) {          /* Ellipsoidal, inverse */
    LP lp = {0.0,0.0};
    struct pj_opaque *Q = P->opaque;
    double theta, dr, S, dif;
    int i;

    xy.x /= P->k0;
    xy.y /= P->k0;
    theta = atan2(xy.x , Q->r0 - xy.y);
    dr = xy.y - xy.x * tan(0.5 * theta);
    lp.lam = theta / Q->l;
    S = dr;
    for (i = MAX_ITER; i ; --i) {
        S -= (dif = (fS(S, Q->C) - dr) / fSp(S, Q->C));
        if (fabs(dif) < DEL_TOL) break;
    }
    if (!i) {
        proj_errno_set(P, PJD_ERR_TOLERANCE_CONDITION);
        return lp;
    }
    lp.phi = pj_inv_mlfn(P->ctx, S + Q->M0, P->es, Q->en);

    return lp;
}


static void *destructor (PJ *P, int errlev) {
    if (0==P)
        return 0;

    if (0==P->opaque)
        return pj_default_destructor (P, errlev);

    pj_dealloc (P->opaque->en);
    return pj_default_destructor (P, errlev);
}


PJ *PROJECTION(lcca) {
    double s2p0, N0, R0, tan0;
    struct pj_opaque *Q = pj_calloc (1, sizeof (struct pj_opaque));
    if (0==Q)
        return pj_default_destructor (P, ENOMEM);
    P->opaque = Q;

    (Q->en = pj_enfn(P->es));
    if (!Q->en)
        return pj_default_destructor (P, ENOMEM);

    if (P->phi0 == 0.) {
        return destructor(P, PJD_ERR_LAT_0_IS_ZERO);
    }
    Q->l = sin(P->phi0);
    Q->M0 = pj_mlfn(P->phi0, Q->l, cos(P->phi0), Q->en);
    s2p0 = Q->l * Q->l;
    R0 = 1. / (1. - P->es * s2p0);
    N0 = sqrt(R0);
    R0 *= P->one_es * N0;
    tan0 = tan(P->phi0);
    Q->r0 = N0 / tan0;
    Q->C = 1. / (6. * R0 * N0);

    P->inv = e_inverse;
    P->fwd = e_forward;
    P->destructor = destructor;

    return P;
}

