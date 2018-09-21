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

#include "proj.h"
#include "projects.h"

PROJ_HEAD(bertin1953, "Bertin 1953")
    "\n\tMisc Sph no inv.";

struct pj_opaque {
    double w;
    double m, rm;
    double cosDeltaPhi, sinDeltaPhi, cosDeltaGamma, sinDeltaGamma, deltaLambda;
};


static XY s_forward (LP lp, PJ *P) {
    XY xy = {0.0,0.0};
    struct pj_opaque *Q = P->opaque;

    // Projection constants
    double fu = 1.4, k = 12., w = 1.68;

    // Aliases
    double lambda = lp.lam, phi = lp.phi;
    
    // Variable
    double d;

    // Apply rotation
	lambda += Q->deltaLambda;

    double cosphi = cos(phi),
      x = cos(lambda) * cosphi,
      y = sin(lambda) * cosphi,
      z = sin(phi),
      z0 = z * Q->cosDeltaPhi + x * Q->sinDeltaPhi;
    lambda = atan2(y * Q->cosDeltaGamma - z0 * Q->sinDeltaGamma,
       x * Q->cosDeltaPhi - z * Q->sinDeltaPhi);
    z0 = z0 * Q->cosDeltaGamma + y * Q->sinDeltaGamma;
    phi = asin(z0);

    lambda = adjlon(lambda);

    // Adjust pre-projection
    if (lambda + phi < -fu) {
      double u = (lambda - phi + 1.6) * (lambda + phi + fu) / 8.;
      lambda += u;
      phi -= 0.8 * u * sin(phi + M_PI / 2.);
    }

    // Project with Hammer (1.68,2)
    cosphi = cos(phi);
    d = sqrt(2./(1. + cosphi * cos(lambda / 2.)));
    xy.x = w * d * cosphi * sin(lambda / 2.);
    xy.y = d * sin(phi);

    // Adjust post-projection
    d = (1. - cos(lambda * phi)) / k;
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

    // force +lon_0 = -16.5
    double deltaLambda = P->lam0 -16.5 / 180. * M_PI;

    // force +lat_0=-42
    double deltaPhi = -42. / 180. * M_PI,
      deltaGamma = 0.  / 180. * M_PI;

    Q->deltaLambda = deltaLambda;
    Q->cosDeltaPhi = cos(deltaPhi);
    Q->sinDeltaPhi = sin(deltaPhi);
    Q->cosDeltaGamma = cos(deltaGamma);
    Q->sinDeltaGamma = sin(deltaGamma);

    P->es = 0.;
    P->fwd = s_forward;
    // P->inv = s_inverse;

    return P;
}
