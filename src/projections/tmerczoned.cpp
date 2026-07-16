/*
 * Copyright (c) 2026 Weston James Renoud
 *
 * Implementation of Transverse Mercator Zoned Grid System
 *
 * See IOGP Guidance Note 7-2, Section 3.2.3.2
 */

#include <errno.h>
#include <math.h>

#include "proj.h"
#include "proj_internal.h"

C_NAMESPACE PJ *pj_tmerc(PJ *);

PROJ_HEAD(tmerczoned, "Transverse Mercator Zoned Grid System");

namespace {

struct pj_tmerczoned_data {
    PJ *tmerc;
    double zoneWidth;
    int zoneCount;
    double perZoneEastingOffset;
};

// Compute the central meridian/longitude of natural origin for the zone.
double center_lam(struct pj_tmerczoned_data *Q, int zone) {
    return (zone - 0.5) * Q->zoneWidth;
}

} // namespace

static PJ_XY tmerczoned_forward(PJ_LP lp, PJ *P) {
    auto *Q = static_cast<struct pj_tmerczoned_data *>(P->opaque);

    if (lp.lam < 0)
        lp.lam += 2 * M_PI;

    // We want to nudge the calc slightly to bias the lower bound to be
    // inclusive and upper bound exclusive. It is not specified in IOGP Guidance
    // Note 7-2, but the use of `floor` suggests this is the intent.
    constexpr double eps = 1e-12 * DEG_TO_RAD;

    // Initial Longitude is already accounted for.
    int zone = static_cast<int>(lp.lam / Q->zoneWidth + eps) + 1;

    lp.lam -= center_lam(Q, zone);
    PJ_XY xy = Q->tmerc->fwd(lp, Q->tmerc);
    xy.x += zone * Q->perZoneEastingOffset;

    return xy;
}

static PJ_LP tmerczoned_inverse(PJ_XY xy, PJ *P) {
    auto *Q = static_cast<struct pj_tmerczoned_data *>(P->opaque);

    int zone = static_cast<int>(xy.x / Q->perZoneEastingOffset + 0.5);

    xy.x -= zone * Q->perZoneEastingOffset;
    PJ_LP lp = Q->tmerc->inv(xy, Q->tmerc);
    lp.lam += center_lam(Q, zone);

    return lp;
}

static PJ *tmerczoned_destructor(PJ *P, int errlev) { /* Destructor */
    if (nullptr == P)
        return nullptr;

    if (nullptr == P->opaque)
        return pj_default_destructor(P, errlev);
    proj_destroy(static_cast<struct pj_tmerczoned_data *>(P->opaque)->tmerc);

    return pj_default_destructor(P, errlev);
}

PJ *PJ_PROJECTION(tmerczoned) {
    auto *Q = static_cast<struct pj_tmerczoned_data *>(
        calloc(1, sizeof(struct pj_tmerczoned_data)));
    if (nullptr == Q)
        return pj_default_destructor(P, PROJ_ERR_OTHER /*ENOMEM*/);
    P->opaque = Q;
    P->destructor = tmerczoned_destructor;
    P->fwd = tmerczoned_forward;
    P->inv = tmerczoned_inverse;

    Q->zoneWidth = pj_param(P->ctx, P->params, "dwidth").f * DEG_TO_RAD;
    Q->zoneCount = static_cast<int>(M_PI / Q->zoneWidth);

    // Scaled to unit sphere/ellipsoid
    Q->perZoneEastingOffset = 1'000'000 / P->a;

    // TODO: copied from https://github.com/OSGeo/PROJ/pull/4770 where
    //       there is an open question if this is sufficient/correct.
    {
        Q->tmerc = pj_tmerc(nullptr);
        if (Q->tmerc == nullptr)
            return tmerczoned_destructor(P, PROJ_ERR_OTHER /*ENOMEM*/);

        Q->tmerc->ctx = P->ctx;
        Q->tmerc->k0 = P->k0;

        pj_calc_ellipsoid_params(Q->tmerc, P->a, P->es);

        Q->tmerc = pj_tmerc(Q->tmerc);
        if (Q->tmerc == nullptr)
            return tmerczoned_destructor(P, PROJ_ERR_OTHER /*ENOMEM*/);
    }

    return P;
}
