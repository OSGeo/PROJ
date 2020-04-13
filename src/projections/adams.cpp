/*
* Implementation of the Guyou, Pierce Quincuncial, Adams Hemisphere in a Square,
* Adams World in a Square I & II projections.
*
* Based on original code from libproj4 written by Gerald Evenden. Adapted to modern
* PROJ by Kristian Evers. Original code found in file src/proj_guyou.c, see
* https://github.com/rouault/libproj4/blob/master/libproject-1.01/src/proj_guyou.c
* for reference.
*
* Copyright (c) 2005, 2006, 2009 Gerald I. Evenden
* Copyright (c) 2020 Kristian Evers
*
* Related material
* ----------------
*
*  CONFORMAL PROJECTION OF THE SPHERE WITHIN A SQUARE, 1929, OSCAR S. ADAMS,
*  U.S. COAST AND GEODETIC SURVEY, Special Publication No.153,
*  ftp://ftp.library.noaa.gov/docs.lib/htdocs/rescue/cgs_specpubs/QB275U35no1531929.pdf
*
*  https://en.wikipedia.org/wiki/Guyou_hemisphere-in-a-square_projection
*  https://en.wikipedia.org/wiki/Adams_hemisphere-in-a-square_projection
*  https://en.wikipedia.org/wiki/Peirce_quincuncial_projection
*/

#define PJ_LIB__

#include <math.h>
#include <errno.h>

#include "proj.h"
#include "proj_internal.h"

PROJ_HEAD(guyou, "Guyou") "\n\tMisc Sph No inv";
PROJ_HEAD(peirce_q, "Peirce Quincuncial") "\n\tMisc Sph No inv";
PROJ_HEAD(adams_hemi, "Adams Hemisphere in a Square") "\n\tMisc Sph No inv";
PROJ_HEAD(adams_ws1, "Adams World in a Square I") "\n\tMisc Sph No inv";
PROJ_HEAD(adams_ws2, "Adams World in a Square II") "\n\tMisc Sph No inv";

namespace { // anonymous namespace

enum projection_type {
    GUYOU,
    PEIRCE_Q,
    ADAMS_HEMI,
    ADAMS_WS1,
    ADAMS_WS2,
};

struct pj_opaque {
    projection_type mode;
};

} // anonymous namespace


#define TOL 1e-9
#define RSQRT2 0.7071067811865475244008443620

static double ell_int_5(double phi) {
/* Procedure to compute elliptic integral of the first kind
 * where k^2=0.5.  Precision good to better than 1e-7
 * The approximation is performed with an even Chebyshev
 * series, thus the coefficients below are the even values
 * and where series evaluation  must be multiplied by the argument. */
    double d1 = 0.0;
    double d2 = 0.0;
    static double C0 = 2.19174570831038;
    static const double C[] = {
        -8.58691003636495e-07,
        2.02692115653689e-07,
        3.12960480765314e-05,
        5.30394739921063e-05,
        -0.0012804644680613,
        -0.00575574836830288,
        0.0914203033408211,
    };

    double y = phi * M_2_PI;
    y = 2. * y * y - 1.;
    double y2 = 2. * y;
    for ( double c: C ) {
        double temp = d1;
        d1 = y2 * d1 - d2 + c;
        d2 = temp;
    }

    return phi * (y * d1 - d2 + 0.5 * C0);

}

static PJ_XY forward(PJ_LP lp, PJ *P) {
    double a=0., b=0.;
    bool sm=false, sn=false;
    PJ_XY xy;
    struct pj_opaque *Q = static_cast<struct pj_opaque*>(P->opaque);

    switch (Q->mode) {
    case GUYOU:
        if ((fabs(lp.lam) - TOL) > M_PI_2) {
            proj_errno_set(P, PJD_ERR_TOLERANCE_CONDITION);
            return proj_coord_error().xy;
        }

        if (fabs(fabs(lp.phi) - M_PI_2) < TOL) {
            xy.x = 0;
            xy.y = lp.phi < 0 ? -1.85407 : 1.85407;
            return xy;
        } else {
            double sl = sin(lp.lam);
            double sp = sin(lp.phi);
            double cp = cos(lp.phi);
            a = aacos(P->ctx, (cp * sl - sp) * RSQRT2);
            b = aacos(P->ctx, (cp * sl + sp) * RSQRT2);
            sm = lp.lam < 0.;
            sn = lp.phi < 0.;
        }
        break;
    case PEIRCE_Q: {
            double sl = sin(lp.lam);
            double cl = cos(lp.lam);
            double cp = cos(lp.phi);
            a = aacos(P->ctx, cp * (sl + cl) * RSQRT2);
            b = aacos(P->ctx, cp * (sl - cl) * RSQRT2);
            sm = sl < 0.;
            sn = cl > 0.;
        }
        break;
    case ADAMS_HEMI: {
            double sp = sin(lp.phi);
            if ((fabs(lp.lam) - TOL) > M_PI_2) {
                proj_errno_set(P, PJD_ERR_TOLERANCE_CONDITION);
                return proj_coord_error().xy;
            }
            a = cos(lp.phi) * sin(lp.lam);
            sm = (sp + a) < 0.;
            sn = (sp - a) < 0.;
            a = aacos(P->ctx, a);
            b = M_PI_2 - lp.phi;
        }
        break;
    case ADAMS_WS1: {
            double sp = tan(0.5 * lp.phi);
            b = cos(aasin(P->ctx, sp)) * sin(0.5 * lp.lam);
            a = aacos(P->ctx, (b - sp) * RSQRT2);
            b = aacos(P->ctx, (b + sp) * RSQRT2);
            sm = lp.lam < 0.;
            sn = lp.phi < 0.;
        }
        break;
    case ADAMS_WS2: {
            double spp = tan(0.5 * lp.phi);
            a = cos(aasin(P->ctx, spp)) * sin(0.5 * lp.lam);
            sm = (spp + a) < 0.;
            sn = (spp - a) < 0.;
            b = aacos(P->ctx, spp);
            a = aacos(P->ctx, a);
        }
        break;
    }

    double m = aasin(P->ctx, sqrt((1. + cos(a + b))));
    if (sm) m = -m;

    double n = aasin(P->ctx, sqrt(fabs(1. - cos(a - b))));
    if (sn) n = -n;

    xy.x = ell_int_5(m);
    xy.y = ell_int_5(n);

    if (Q->mode == ADAMS_HEMI || Q->mode == ADAMS_WS2) { /* rotate by 45deg. */
        double temp = xy.x;
        xy.x = RSQRT2 * (xy.x - xy.y);
        xy.y = RSQRT2 * (temp + xy.y);
    }

    return xy;
}


static PJ *setup(PJ *P, projection_type mode) {
    struct pj_opaque *Q = static_cast<struct pj_opaque*>(
            pj_calloc (1, sizeof (struct pj_opaque)));

    if (nullptr==Q)
        return pj_default_destructor (P, ENOMEM);
    P->opaque = Q;

    P->es = 0;
    P->fwd = forward;

    Q->mode = mode;

    return P;
}


PJ *PROJECTION(guyou) {
    return setup(P, GUYOU);
}

PJ *PROJECTION(peirce_q) {
    return setup(P, PEIRCE_Q);
}

PJ *PROJECTION(adams_hemi) {
    return setup(P, ADAMS_HEMI);
}

PJ *PROJECTION(adams_ws1) {
    return setup(P, ADAMS_WS1);
}

PJ *PROJECTION(adams_ws2) {
    return setup(P, ADAMS_WS2);
}
