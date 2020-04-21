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

#include <algorithm>

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
    constexpr double C0 = 2.19174570831038;
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
    double d1 = 0.0;
    double d2 = 0.0;
    for ( double c: C ) {
        double temp = d1;
        d1 = y2 * d1 - d2 + c;
        d2 = temp;
    }

    return phi * (y * d1 - d2 + 0.5 * C0);

}

static PJ_XY adams_forward(PJ_LP lp, PJ *P) {
    double a=0., b=0.;
    bool sm=false, sn=false;
    PJ_XY xy;
    const struct pj_opaque *Q = static_cast<const struct pj_opaque*>(P->opaque);

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
            const double sl = sin(lp.lam);
            const double sp = sin(lp.phi);
            const double cp = cos(lp.phi);
            a = aacos(P->ctx, (cp * sl - sp) * RSQRT2);
            b = aacos(P->ctx, (cp * sl + sp) * RSQRT2);
            sm = lp.lam < 0.;
            sn = lp.phi < 0.;
        }
        break;
    case PEIRCE_Q: {
            const double sl = sin(lp.lam);
            const double cl = cos(lp.lam);
            const double cp = cos(lp.phi);
            a = aacos(P->ctx, cp * (sl + cl) * RSQRT2);
            b = aacos(P->ctx, cp * (sl - cl) * RSQRT2);
            sm = sl < 0.;
            sn = cl > 0.;
        }
        break;
    case ADAMS_HEMI: {
            const double sp = sin(lp.phi);
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
            const double sp = tan(0.5 * lp.phi);
            b = cos(aasin(P->ctx, sp)) * sin(0.5 * lp.lam);
            a = aacos(P->ctx, (b - sp) * RSQRT2);
            b = aacos(P->ctx, (b + sp) * RSQRT2);
            sm = lp.lam < 0.;
            sn = lp.phi < 0.;
        }
        break;
    case ADAMS_WS2: {
            const double spp = tan(0.5 * lp.phi);
            a = cos(aasin(P->ctx, spp)) * sin(0.5 * lp.lam);
            sm = (spp + a) < 0.;
            sn = (spp - a) < 0.;
            b = aacos(P->ctx, spp);
            a = aacos(P->ctx, a);
        }
        break;
    }

    double m = aasin(P->ctx, sqrt((1. + std::min(0.0, cos(a + b)))));
    if (sm) m = -m;

    double n = aasin(P->ctx, sqrt(fabs(1. - std::max(0.0, cos(a - b)))));
    if (sn) n = -n;

    xy.x = ell_int_5(m);
    xy.y = ell_int_5(n);

    if (Q->mode == ADAMS_HEMI || Q->mode == ADAMS_WS2) { /* rotate by 45deg. */
        const double temp = xy.x;
        xy.x = RSQRT2 * (xy.x - xy.y);
        xy.y = RSQRT2 * (temp + xy.y);
    }

    return xy;
}

static PJ_LP inverse_with_newton_raphson(PJ_XY xy,
                                         PJ *P,
                                         PJ_LP lp, // initial guess
                                         PJ_XY  (*fwd)(PJ_LP,    PJ *))
{
    double deriv_lam_X = 0;
    double deriv_lam_Y = 0;
    double deriv_phi_X = 0;
    double deriv_phi_Y = 0;
    for(int i = 0; i < 15; i++ )
    {
        PJ_XY xyApprox = fwd(lp, P);
        const double deltaX = xyApprox.x - xy.x;
        const double deltaY = xyApprox.y - xy.y;
        if( fabs(deltaX) < 1e-10 && fabs(deltaY) < 1e-10 )
        {
            return lp;
        }

        if( fabs(deltaX) > 1e-6 || fabs(deltaY) > 1e-6 )
        {
            // Compute Jacobian matrix (only if we aren't close to the final
            // result to speed things a bit)
            PJ_LP lp2;
            PJ_XY xy2;
            const double dLam = lp.lam > 0 ? -1e-6 : 1e-6;
            lp2.lam = lp.lam + dLam;
            lp2.phi = lp.phi;
            xy2 = fwd(lp2, P);
            const double deriv_X_lam = (xy2.x - xyApprox.x) / dLam;
            const double deriv_Y_lam = (xy2.y - xyApprox.y) / dLam;

            const double dPhi = lp.phi > 0 ? -1e-6 : 1e-6;
            lp2.lam = lp.lam;
            lp2.phi = lp.phi + dPhi;
            xy2 = fwd(lp2, P);
            const double deriv_X_phi = (xy2.x - xyApprox.x) / dPhi;
            const double deriv_Y_phi = (xy2.y - xyApprox.y) / dPhi;

            // Inverse of Jacobian matrix
            const double det = deriv_X_lam * deriv_Y_phi - deriv_X_phi * deriv_Y_lam;
            if( det != 0  )
            {
                deriv_lam_X =  deriv_Y_phi / det;
                deriv_lam_Y = -deriv_X_phi / det;
                deriv_phi_X = -deriv_Y_lam / det;
                deriv_phi_Y =  deriv_X_lam / det;
            }
        }

        if( xy.x != 0 )
        {
            // Limit the amplitude of correction to avoid overshoots due to
            // bad initial guess
            const double delta_lam = std::max(std::min(
                deltaX * deriv_lam_X + deltaY * deriv_lam_Y, 0.3), -0.3);
            lp.lam -= delta_lam;
            if( lp.lam < -M_PI )
                lp.lam = -M_PI;
            else if( lp.lam > M_PI )
                lp.lam = M_PI;
        }

        if( xy.y != 0 )
        {
            const double delta_phi = std::max(std::min(
                deltaX * deriv_phi_X + deltaY * deriv_phi_Y, 0.3), -0.3);
            lp.phi -= delta_phi;
            if( lp.phi < -M_HALFPI )
                lp.phi = -M_HALFPI ;
            else if( lp.phi > M_HALFPI )
                lp.phi = M_HALFPI;
        }
    }
    pj_ctx_set_errno( P->ctx, PJD_ERR_NON_CONVERGENT );
    return lp;
}

static PJ_LP adams_inverse(PJ_XY xy, PJ *P)
{
    PJ_LP lp;

    // Only implemented for ADAMS_WS2
    // Uses Newton-Raphson method on the following pair of functions:
    //      f_x(lam,phi) = adams_forward(lam, phi).x - xy.x
    //      f_y(lam,phi) = adams_forward(lam, phi).y - xy.y

    // Initial guess (very rough, especially at high northings)
    // The magic values are got with:
    //  echo 0   90 | src/proj -f "%.8f" +proj=adams_ws2 +R=1
    //  echo 180 0  | src/proj -f "%.8f" +proj=adams_ws2 +R=1
    lp.phi = std::max(std::min(xy.y / 2.62181347, 1.0), -1.0) * M_HALFPI;
    lp.lam = fabs(lp.phi) >= M_HALFPI ? 0 :
        std::max(std::min(xy.x / 2.62205760 / cos(lp.phi), 1.0), -1.0) * M_PI;

    return inverse_with_newton_raphson(xy, P, lp, adams_forward);
}


static PJ *setup(PJ *P, projection_type mode) {
    struct pj_opaque *Q = static_cast<struct pj_opaque*>(
            pj_calloc (1, sizeof (struct pj_opaque)));

    if (nullptr==Q)
        return pj_default_destructor (P, ENOMEM);
    P->opaque = Q;

    P->es = 0;
    P->fwd = adams_forward;

    Q->mode = mode;
    if( mode == ADAMS_WS2 )
        P->inv = adams_inverse;

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
