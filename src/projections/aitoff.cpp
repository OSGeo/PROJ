/******************************************************************************
 * Project:  PROJ.4
 * Purpose:  Implementation of the aitoff (Aitoff) and wintri (Winkel Tripel)
 *           projections.
 * Author:   Gerald Evenden (1995)
 *           Drazen Tutic, Lovro Gradiser (2015) - add inverse
 *           Thomas Knudsen (2016) - revise/add regression tests
 *           Robert Kleffner (2026) - closed-form Aitoff inverse
 *
 ******************************************************************************
 * Copyright (c) 1995, Gerald Evenden
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *****************************************************************************/

#include <errno.h>
#include <math.h>

#include "proj.h"
#include "proj_internal.h"

namespace pj_aitoff_ns {
enum Mode { AITOFF = 0, WINKEL_TRIPEL = 1 };
}

namespace { // anonymous namespace
struct pj_aitoff_data {
    double cosphi1;
    enum pj_aitoff_ns::Mode mode;
};
} // anonymous namespace

PROJ_HEAD(aitoff, "Aitoff") "\n\tMisc Sph";
PROJ_HEAD(wintri, "Winkel Tripel") "\n\tMisc Sph\n\tlat_1";

#if 0
FORWARD(aitoff_s_forward); /* spheroid */
#endif

static PJ_XY aitoff_s_forward(PJ_LP lp, PJ *P) { /* Spheroidal, forward */
    PJ_XY xy = {0.0, 0.0};
    struct pj_aitoff_data *Q = static_cast<struct pj_aitoff_data *>(P->opaque);

#if 0
    // Likely domain of validity for wintri in +over mode. Should be confirmed
    // Cf https://lists.osgeo.org/pipermail/gdal-dev/2023-April/057164.html
    if (Q->mode == WINKEL_TRIPEL && fabs(lp.lam) > 2 * M_PI) {
        proj_errno_set(P, PROJ_ERR_COORD_TRANSFM_OUTSIDE_PROJECTION_DOMAIN);
        return xy;
    }
#endif
    const double sin_half_lam = sin(0.5 * lp.lam);
    const double sin_phi = sin(lp.phi);
    const double cos_phi = cos(lp.phi);
    /* basic Aitoff; computing it with atan2 (instead of standard acos)
     * keeps relative accuracy near the origin. */
    const double sin_alpha = hypot(cos_phi * sin_half_lam, sin_phi);
    /* alpha / sin(alpha), with limit 1 at the origin */
    const double A =
        sin_alpha == 0
            ? 1.
            : atan2(sin_alpha, cos_phi * cos(0.5 * lp.lam)) / sin_alpha;
    xy.x = 2. * cos_phi * sin_half_lam * A;
    xy.y = sin_phi * A;
    if (Q->mode == pj_aitoff_ns::WINKEL_TRIPEL) {
        xy.x = (xy.x + lp.lam * Q->cosphi1) * 0.5;
        xy.y = (xy.y + lp.phi) * 0.5;
    }
    return (xy);
}

/******************************************************************************
 *
 * Closed-form inverse of the Aitoff projection. Short form equations are:
 *
 *     alpha  = hypot(x/2, y)      (angular distance to (phi, lambda/2))
 *     phi    = asin(y * sin(alpha) / alpha)
 *     lambda = 2 * atan2((x/2) * sin(alpha), alpha * cos(alpha))
 *
 * atan2 places lambda/2 in (-pi, pi]. The domain of the inverse is therefore
 * the whole ellipse x^2 + 4*y^2 <= (2*pi)^2, on which the sphere is double
 * covered; longitudes beyond +/-pi are returned only with +over. The poles
 * need no special case.
 *
 * Reference:
 *   J. P. Snyder (1987). Map Projections -- A Working Manual. USGS
 *   Professional Paper 1395, pp. 195-197.
 *
 *****************************************************************************/

/* Returns false, leaving *lp untouched, when (x, y) is outside the ellipse
 * x^2 + 4*y^2 = (2*pi)^2 and so has no preimage. */
static bool aitoff_s_inverse_closed(PJ_XY xy, PJ_LP *lp) {
    const double half_x = 0.5 * xy.x;
    const double alpha = hypot(half_x, xy.y);
    const double sin_alpha = sin(alpha);
    const double cos_alpha = cos(alpha);
    /* Testing sin_alpha is needed because with long doubles sin(M_PI) < 0. */
    if (alpha > M_PI || sin_alpha < 0)
        return false;
    /* The asin argument is guaranteed to lie in [-1, 1]: hypot(a, b) >= |b|
     * and 0 <= sin_alpha <= 1. */
    lp->phi = alpha == 0.0 ? xy.y : asin((xy.y / alpha) * sin_alpha);
    lp->lam = 2. * atan2(half_x * sin_alpha, alpha * cos_alpha);
    return true;
}

/******************************************************************************
 *
 * Inverse of the Aitoff and Winkel Tripel projections.
 *
 * Aitoff uses the closed form above. Winkel Tripel, the mean of the Aitoff and
 * equirectangular projections, has no known closed-form inverse; it is inverted
 * with the Newton-Raphson method of Bildirici and Ipbuker (2002), added by
 * Drazen Tutic and Lovro Gradiser (2015). Expected accuracy is defined by
 * EPSILON = 1e-12, appropriate for most applications of the projection.
 *
 * Reference:
 *   I. O. Bildirici and C. Ipbuker (2002). A General Algorithm for the Inverse
 *   Transformation of Map Projections Using Jacobian Matrices. Proceedings of
 *   the Third International Symposium on Mathematical and Computational
 *   Applications, pp. 175-182, Turkey.
 *
 * Contact: dtutic at geof.hr, 2015-02-16
 *
 *****************************************************************************/

static PJ_LP aitoff_s_inverse(PJ_XY xy, PJ *P) { /* Spheroidal, inverse */
    PJ_LP lp = {0.0, 0.0};
    struct pj_aitoff_data *Q = static_cast<struct pj_aitoff_data *>(P->opaque);

    if (Q->mode == pj_aitoff_ns::AITOFF) {
        if (!aitoff_s_inverse_closed(xy, &lp)) {
            proj_errno_set(P, PROJ_ERR_COORD_TRANSFM_OUTSIDE_PROJECTION_DOMAIN);
            lp.lam = lp.phi = HUGE_VAL;
        }
        return lp;
    }

    int iter, MAXITER = 10, round = 0, MAXROUND = 20;
    double EPSILON = 1e-12, D, C, f1, f2, f1p, f1l, f2p, f2l, dp, dl, sl, sp,
           cp, cl, x, y;

    if ((fabs(xy.x) < EPSILON) && (fabs(xy.y) < EPSILON)) {
        lp.phi = 0.;
        lp.lam = 0.;
        return lp;
    }

    /* initial values for Newton-Raphson method */
    lp.phi = xy.y;
    lp.lam = xy.x;
    do {
        iter = 0;
        do {
            sl = sin(lp.lam * 0.5);
            cl = cos(lp.lam * 0.5);
            sp = sin(lp.phi);
            cp = cos(lp.phi);
            D = cp * cl;
            C = 1. - D * D;
            const double denom = pow(C, 1.5);
            if (denom == 0) {
                proj_errno_set(
                    P, PROJ_ERR_COORD_TRANSFM_OUTSIDE_PROJECTION_DOMAIN);
                return lp;
            }
            D = acos(D) / denom;
            f1 = 2. * D * C * cp * sl;
            f2 = D * C * sp;
            f1p = 2. * (sl * cl * sp * cp / C - D * sp * sl);
            f1l = cp * cp * sl * sl / C + D * cp * cl * sp * sp;
            f2p = sp * sp * cl / C + D * sl * sl * cp;
            f2l = 0.5 * (sp * cp * sl / C - D * sp * cp * cp * sl * cl);
            if (Q->mode == pj_aitoff_ns::WINKEL_TRIPEL) {
                f1 = 0.5 * (f1 + lp.lam * Q->cosphi1);
                f2 = 0.5 * (f2 + lp.phi);
                f1p *= 0.5;
                f1l = 0.5 * (f1l + Q->cosphi1);
                f2p = 0.5 * (f2p + 1.);
                f2l *= 0.5;
            }
            f1 -= xy.x;
            f2 -= xy.y;
            dp = f1p * f2l - f2p * f1l;
            dl = (f2 * f1p - f1 * f2p) / dp;
            dp = (f1 * f2l - f2 * f1l) / dp;
            dl = fmod(dl, M_PI); /* set to interval [-M_PI, M_PI] */
            lp.phi -= dp;
            lp.lam -= dl;
        } while ((fabs(dp) > EPSILON || fabs(dl) > EPSILON) &&
                 (iter++ < MAXITER));
        if (lp.phi > M_PI_2)
            lp.phi -=
                2. * (lp.phi -
                      M_PI_2); /* correct if symmetrical solution for Aitoff */
        if (lp.phi < -M_PI_2)
            lp.phi -=
                2. * (lp.phi +
                      M_PI_2); /* correct if symmetrical solution for Aitoff */

        /* calculate x,y coordinates with solution obtained */
        if ((D = acos(cos(lp.phi) * cos(C = 0.5 * lp.lam))) !=
            0.0) { /* Aitoff */
            y = 1. / sin(D);
            x = 2. * D * cos(lp.phi) * sin(C) * y;
            y *= D * sin(lp.phi);
        } else
            x = y = 0.;
        if (Q->mode == pj_aitoff_ns::WINKEL_TRIPEL) {
            x = (x + lp.lam * Q->cosphi1) * 0.5;
            y = (y + lp.phi) * 0.5;
        }
        /* if too far from given values of x,y, repeat with better approximation
         * of phi,lam */
    } while (((fabs(xy.x - x) > EPSILON) || (fabs(xy.y - y) > EPSILON)) &&
             (round++ < MAXROUND));

    if (iter == MAXITER && round == MAXROUND) {
        proj_context_errno_set(
            P->ctx, PROJ_ERR_COORD_TRANSFM_OUTSIDE_PROJECTION_DOMAIN);
        /* fprintf(stderr, "Warning: Accuracy of 1e-12 not reached. Last
         * increments: dlat=%e and dlon=%e\n", dp, dl); */
    }

    return lp;
}

static PJ *pj_aitoff_setup(PJ *P) {
    P->inv = aitoff_s_inverse;
    P->fwd = aitoff_s_forward;
    P->es = 0.;
    return P;
}

PJ *PJ_PROJECTION(aitoff) {
    struct pj_aitoff_data *Q = static_cast<struct pj_aitoff_data *>(
        calloc(1, sizeof(struct pj_aitoff_data)));
    if (nullptr == Q)
        return pj_default_destructor(P, PROJ_ERR_OTHER /*ENOMEM*/);
    P->opaque = Q;

    Q->mode = pj_aitoff_ns::AITOFF;
    return pj_aitoff_setup(P);
}

PJ *PJ_PROJECTION(wintri) {
    struct pj_aitoff_data *Q = static_cast<struct pj_aitoff_data *>(
        calloc(1, sizeof(struct pj_aitoff_data)));
    if (nullptr == Q)
        return pj_default_destructor(P, PROJ_ERR_OTHER /*ENOMEM*/);
    P->opaque = Q;

    Q->mode = pj_aitoff_ns::WINKEL_TRIPEL;
    if (pj_param(P->ctx, P->params, "tlat_1").i) {
        if ((Q->cosphi1 = cos(pj_param(P->ctx, P->params, "rlat_1").f)) == 0.) {
            proj_log_error(
                P, _("Invalid value for lat_1: |lat_1| should be < 90°"));
            return pj_default_destructor(P,
                                         PROJ_ERR_INVALID_OP_ILLEGAL_ARG_VALUE);
        }
    } else /* 50d28' or acos(2/pi) */
        Q->cosphi1 = 0.636619772367581343;
    return pj_aitoff_setup(P);
}
