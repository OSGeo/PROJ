/*
  Created by Jacques Bertin in 1953, this projection was the go-to choice
  of the French cartographic school when they wished to represent phenomena
  on a global scale.
  
  Formula designed by Philippe Rivière, 2017.
  https://visionscarto.net/bertin-projection-1953

  Port to PROJ by Philippe Rivière, 21 September 2018
*/

#define PJ_LIB__

#include <errno.h>
#include <math.h>

#include "proj_internal.h"
#include "proj.h"
#include "projects.h"

PROJ_HEAD(bertin1953, "Bertin 1953")
    "\n\tMisc Sph no inv.";

struct pj_opaque {
    double cosDeltaPhi, sinDeltaPhi, cosDeltaGamma, sinDeltaGamma, deltaLambda;
};


static XY s_forward (LP lp, PJ *P) {
    XY xy = {0.0,0.0};
    struct pj_opaque *Q = P->opaque;

    double fu = 1.4, k = 12., w = 1.68, d;

    /* Rotate */
    double cosphi, x, y, z, z0;
    lp.lam += PJ_TORAD(-16.5);
    cosphi = cos(lp.phi);
    x = cos(lp.lam) * cosphi;
    y = sin(lp.lam) * cosphi;
    z = sin(lp.phi);
    z0 = z * Q->cosDeltaPhi + x * Q->sinDeltaPhi;
    lp.lam = atan2(y * Q->cosDeltaGamma - z0 * Q->sinDeltaGamma,
       x * Q->cosDeltaPhi - z * Q->sinDeltaPhi);
    z0 = z0 * Q->cosDeltaGamma + y * Q->sinDeltaGamma;
    lp.phi = asin(z0);

    lp.lam = adjlon(lp.lam);

    /* Adjust pre-projection */
    if (lp.lam + lp.phi < -fu) {
      d = (lp.lam - lp.phi + 1.6) * (lp.lam + lp.phi + fu) / 8.;
      lp.lam += d;
      lp.phi -= 0.8 * d * sin(lp.phi + M_PI / 2.);
    }

    /* Project with Hammer (1.68,2) */
    cosphi = cos(lp.phi);
    d = sqrt(2./(1. + cosphi * cos(lp.lam / 2.)));
    xy.x = w * d * cosphi * sin(lp.lam / 2.);
    xy.y = d * sin(lp.phi);

    /* Adjust post-projection */
    d = (1. - cos(lp.lam * lp.phi)) / k;
    if (xy.y < 0.) {
      xy.x *= 1. + d;
    }
    if (xy.y > 0.) {
      xy.x *= 1. + d / 1.5 * xy.x * xy.x;
    }

    return xy;
}


PJ *PROJECTION(bertin1953) {
    struct pj_opaque *Q = pj_calloc (1, sizeof (struct pj_opaque));
    if (0==Q)
        return pj_default_destructor (P, ENOMEM);
    P->opaque = Q;

    P->lam0 = 0;
    P->phi0 = PJ_TORAD(-42.);

    Q->cosDeltaPhi = cos(P->phi0);
    Q->sinDeltaPhi = sin(P->phi0);
    Q->cosDeltaGamma = 1.;
    Q->sinDeltaGamma = 0.;

    P->es = 0.;
    P->fwd = s_forward;

    return P;
}
