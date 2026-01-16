/******************************************************************************
 * Project:  PROJ
 * Purpose:  Implementation of the eqc (Equidistant Cylindrical) projection.
 *           Also known as Plate Carree when lat_ts=0.
 *
 * This file implements both the spherical (EPSG:1029) and ellipsoidal
 * (EPSG:1028) forms of the Equidistant Cylindrical projection.
 *
 * Spherical formulas (EPSG:1029):
 *   E = FE + R cos(lat_ts) (lon - lon_0)
 *   N = FN + R lat
 *
 * Ellipsoidal formulas (EPSG:1028) from IOGP Guidance Note 7-2:
 *   E = FE + nu1 cos(lat_ts) (lon - lon_0)
 *   N = FN + M
 *
 *
 * Reference: IOGP Publication 373-7-2, Section 3.2.5
 *

 *****************************************************************************/

#include <errno.h>
#include <math.h>
#include <stdlib.h>

#include "proj.h"
#include "proj_internal.h"

namespace { // anonymous namespace
struct pj_eqc_data {
    double rc;  // Spherical: cos(lat_ts); Ellipsoidal: nu1 * cos(lat_ts)
    double M0;  // Meridional arc at latitude of origin (lat_0)
    double *en; // Coefficients for meridional arc computation
};
} // anonymous namespace

PROJ_HEAD(eqc, "Equidistant Cylindrical (Plate Carree)")
"\n\tCyl, Sph&Ell\n\tlat_ts=[, lat_0=0]";

static PJ_XY eqc_s_forward(PJ_LP lp, PJ *P) { /* Spheroidal, forward */
    PJ_XY xy = {0.0, 0.0};
    struct pj_eqc_data *Q = static_cast<struct pj_eqc_data *>(P->opaque);

    xy.x = Q->rc * lp.lam;
    xy.y = lp.phi - P->phi0;

    return xy;
}

static PJ_LP eqc_s_inverse(PJ_XY xy, PJ *P) { /* Spheroidal, inverse */
    PJ_LP lp = {0.0, 0.0};
    struct pj_eqc_data *Q = static_cast<struct pj_eqc_data *>(P->opaque);

    lp.lam = xy.x / Q->rc;
    lp.phi = xy.y + P->phi0;

    return lp;
}

// Ellipsoidal forward (EPSG method 1028)
// E = FE + nu1 cos(lat_ts) (lon - lon_0)
// N = FN + M - M0
// where M is the meridional arc from equator to latitude
static PJ_XY eqc_e_forward(PJ_LP lp, PJ *P) { /* Ellipsoidal, forward */
    PJ_XY xy = {0.0, 0.0};
    struct pj_eqc_data *Q = static_cast<struct pj_eqc_data *>(P->opaque);

    const double sinphi = sin(lp.phi);
    const double cosphi = cos(lp.phi);

    xy.x = Q->rc * lp.lam;
    xy.y = pj_mlfn(lp.phi, sinphi, cosphi, Q->en) - Q->M0;

    return xy;
}

// Ellipsoidal inverse (EPSG method 1028)
// lon = lon_0 + (E - FE) / (nu1 cos lat_ts)
// lat is found by inverting M = N - FN + M0
static PJ_LP eqc_e_inverse(PJ_XY xy, PJ *P) { /* Ellipsoidal, inverse */
    PJ_LP lp = {0.0, 0.0};
    struct pj_eqc_data *Q = static_cast<struct pj_eqc_data *>(P->opaque);

    lp.lam = xy.x / Q->rc;
    lp.phi = pj_inv_mlfn(xy.y + Q->M0, Q->en);

    return lp;
}

static PJ *pj_eqc_destructor(PJ *P, int errlev) { /* Destructor */
    if (nullptr == P)
        return nullptr;

    if (nullptr == P->opaque)
        return pj_default_destructor(P, errlev);

    free(static_cast<struct pj_eqc_data *>(P->opaque)->en);
    return pj_default_destructor(P, errlev);
}

PJ *PJ_PROJECTION(eqc) {
    struct pj_eqc_data *Q = static_cast<struct pj_eqc_data *>(
        calloc(1, sizeof(struct pj_eqc_data)));
    if (nullptr == Q)
        return pj_default_destructor(P, PROJ_ERR_OTHER /*ENOMEM*/);
    P->opaque = Q;
    P->destructor = pj_eqc_destructor;

    const double phi1 = pj_param(P->ctx, P->params, "rlat_ts").f;
    const double cos_phi1 = cos(phi1);

    if (cos_phi1 <= 0.) {
        proj_log_error(
            P, _("Invalid value for lat_ts: |lat_ts| should be <= 90°"));
        return pj_default_destructor(P, PROJ_ERR_INVALID_OP_ILLEGAL_ARG_VALUE);
    }

    if (P->es != 0.0) {
        // Ellipsoidal case (EPSG:1028)
        const double sin_phi1 = sin(phi1);
        // nu1 = a / sqrt(1 - e^2 sin^2(lat_ts)), normalized by a
        const double nu1 = 1.0 / sqrt(1.0 - P->es * sin_phi1 * sin_phi1);
        Q->rc = nu1 * cos_phi1;

        Q->en = pj_enfn(P->n);
        if (nullptr == Q->en)
            return pj_default_destructor(P, PROJ_ERR_OTHER /*ENOMEM*/);

        Q->M0 = pj_mlfn(P->phi0, sin(P->phi0), cos(P->phi0), Q->en);

        P->inv = eqc_e_inverse;
        P->fwd = eqc_e_forward;
    } else {
        // Spheroidal case (EPSG:1029)
        Q->rc = cos_phi1;
        Q->en = nullptr;
        Q->M0 = 0.0;

        P->inv = eqc_s_inverse;
        P->fwd = eqc_s_forward;
    }

    return P;
}
