#define PJ_LIB__
#include <errno.h>
#include <proj.h>
#include "projects.h"

PROJ_HEAD(gnom, "Gnomonic") "\n\tAzi, Sph.";

#define EPS10  1.e-10

enum Mode {
    N_POLE = 0,
    S_POLE = 1,
    EQUIT  = 2,
    OBLIQ  = 3
};

struct pj_opaque {
    double  sinph0;
    double  cosph0;
    enum Mode mode;
};


static XY s_forward (LP lp, PJ *P) {           /* Spheroidal, forward */
    XY xy = {0.0,0.0};
    struct pj_opaque *Q = P->opaque;
    double  coslam, cosphi, sinphi;

    sinphi = sin(lp.phi);
    cosphi = cos(lp.phi);
    coslam = cos(lp.lam);

    switch (Q->mode) {
        case EQUIT:
            xy.y = cosphi * coslam;
            break;
        case OBLIQ:
            xy.y = Q->sinph0 * sinphi + Q->cosph0 * cosphi * coslam;
            break;
        case S_POLE:
            xy.y = - sinphi;
            break;
        case N_POLE:
            xy.y = sinphi;
            break;
    }

    if (xy.y <= EPS10) {
        proj_errno_set(P, PJD_ERR_TOLERANCE_CONDITION);
        return xy;
    }

    xy.x = (xy.y = 1. / xy.y) * cosphi * sin(lp.lam);
    switch (Q->mode) {
        case EQUIT:
            xy.y *= sinphi;
            break;
        case OBLIQ:
            xy.y *= Q->cosph0 * sinphi - Q->sinph0 * cosphi * coslam;
            break;
        case N_POLE:
            coslam = - coslam;
            /*-fallthrough*/
        case S_POLE:
            xy.y *= cosphi * coslam;
            break;
    }
    return xy;
}


static LP s_inverse (XY xy, PJ *P) {           /* Spheroidal, inverse */
    LP lp = {0.0,0.0};
    struct pj_opaque *Q = P->opaque;
    double  rh, cosz, sinz;

    rh = hypot(xy.x, xy.y);
    sinz = sin(lp.phi = atan(rh));
    cosz = sqrt(1. - sinz * sinz);

    if (fabs(rh) <= EPS10) {
        lp.phi = P->phi0;
        lp.lam = 0.;
    } else {
        switch (Q->mode) {
            case OBLIQ:
                lp.phi = cosz * Q->sinph0 + xy.y * sinz * Q->cosph0 / rh;
                if (fabs(lp.phi) >= 1.)
                    lp.phi = lp.phi > 0. ? M_HALFPI : - M_HALFPI;
                else
                    lp.phi = asin(lp.phi);
                xy.y = (cosz - Q->sinph0 * sin(lp.phi)) * rh;
                xy.x *= sinz * Q->cosph0;
                break;
            case EQUIT:
                lp.phi = xy.y * sinz / rh;
                if (fabs(lp.phi) >= 1.)
                    lp.phi = lp.phi > 0. ? M_HALFPI : - M_HALFPI;
                else
                    lp.phi = asin(lp.phi);
                xy.y = cosz * rh;
                xy.x *= sinz;
                break;
            case S_POLE:
                lp.phi -= M_HALFPI;
                break;
            case N_POLE:
                lp.phi = M_HALFPI - lp.phi;
                xy.y = -xy.y;
                break;
        }
        lp.lam = atan2(xy.x, xy.y);
    }
    return lp;
}


PJ *PROJECTION(gnom) {
    struct pj_opaque *Q = pj_calloc (1, sizeof (struct pj_opaque));
    if (0==Q)
        return pj_default_destructor (P, ENOMEM);
    P->opaque = Q;

    if (fabs(fabs(P->phi0) - M_HALFPI) < EPS10) {
        Q->mode = P->phi0 < 0. ? S_POLE : N_POLE;
    } else if (fabs(P->phi0) < EPS10) {
        Q->mode = EQUIT;
    } else {
        Q->mode = OBLIQ;
        Q->sinph0 = sin(P->phi0);
        Q->cosph0 = cos(P->phi0);
    }

    P->inv = s_inverse;
    P->fwd = s_forward;
    P->es = 0.;

    return P;
}

