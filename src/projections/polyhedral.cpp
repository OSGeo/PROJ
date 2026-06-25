/******************************************************************************
 * Project:  PROJ
 * Purpose:  Polyhedral Snyder equal-area map projections.
 * Author:   Felix Palmer
 *
 ******************************************************************************
 * Derived from A5 (Apache-2.0).
 * https://github.com/felixpalmer/a5
 *
 * Copyright (c) A5 contributors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ****************************************************************************/

#include "polyhedral/conway.h"
#include "polyhedral/nets/dsea/dsea.h"
#include "polyhedral/nets/isea/isea.h"
#include "polyhedral/nets/tsea/tsea.h"
#include "polyhedral/polyhedra/dodecahedron.h"
#include "polyhedral/polyhedra/icosahedron.h"
#include "polyhedral/polyhedra/tetrahedron.h"
#include "polyhedral/state.h"

#include "proj.h"
#include "proj_internal.h"

#include <cmath>
#include <cstdlib>
#include <cstring>

using polyhedral::pj_polyhedral_data;

static PJ_XY polyhedral_fwd(PJ_LP lp, PJ *P) {
    PJ_XY xy = {0.0, 0.0};
    auto *Q = static_cast<pj_polyhedral_data *>(P->opaque);

    Vec3 v = polyhedral::lonlat_to_cart(Q, P, lp.lam, lp.phi);

    int tri_idx = polyhedral::find_triangle(Q, v);
    if (tri_idx < 0) {
        proj_errno_set(P, PROJ_ERR_COORD_TRANSFM_OUTSIDE_PROJECTION_DOMAIN);
        return proj_coord_error().xy;
    }

    polyhedral::Face2D result =
        polyhedral::snyder_fwd(v, Q->sph_tris[tri_idx], Q->face_tris[tri_idx]);

    xy.x = result.x - Q->x_offset;
    xy.y = result.y - Q->y_offset;

    return xy;
}

static PJ_LP polyhedral_inv(PJ_XY xy, PJ *P) {
    PJ_LP lp = {0.0, 0.0};
    auto *Q = static_cast<pj_polyhedral_data *>(P->opaque);

    const double x = xy.x + Q->x_offset;
    const double y = xy.y + Q->y_offset;

    for (int i = 0; i < Q->n_triangles; i++) {
        polyhedral::Barycentric bary =
            polyhedral::face_to_barycentric({x, y}, Q->face_tris[i]);
        if (bary.u >= -1e-10 && bary.v >= -1e-10 && bary.w >= -1e-10) {
            Vec3 v =
                polyhedral::snyder_inv({x, y}, Q->face_tris[i], Q->sph_tris[i]);
            polyhedral::cart_to_lonlat(Q, P, v, lp.lam, lp.phi);
            return lp;
        }
    }

    proj_errno_set(P, PROJ_ERR_COORD_TRANSFM_OUTSIDE_PROJECTION_DOMAIN);
    return proj_coord_error().lp;
}

static PJ *polyhedral_destructor(PJ *P, int errlev) {
    if (nullptr == P)
        return nullptr;
    if (nullptr == P->opaque)
        return pj_default_destructor(P, errlev);
    free(static_cast<pj_polyhedral_data *>(P->opaque)->apa);
    return pj_default_destructor(P, errlev);
}

static PJ *polyhedral_setup(PJ *P, const polyhedral::PolyhedralDefaults &d) {
    auto *Q = static_cast<pj_polyhedral_data *>(P->opaque);
    if (P->es != 0.0) {
        Q->apa = pj_authalic_lat_compute_coeffs(P->n);
        if (nullptr == Q->apa)
            return polyhedral_destructor(P, PROJ_ERR_OTHER /*ENOMEM*/);
        Q->qp = pj_authalic_lat_q(1.0, P);
    }
    if (!polyhedral::apply_polyhedral_params(Q, P, d))
        return polyhedral_destructor(P, PROJ_ERR_INVALID_OP_ILLEGAL_ARG_VALUE);
    P->fwd = polyhedral_fwd;
    P->inv = polyhedral_inv;
    return P;
}

PROJ_HEAD(tsea, "Tetrahedral Snyder Equal Area")
"\n\tSph&Ell\n\tnet= orient_lat= orient_lon= azi= lat_0= lon_0=";
PJ *PJ_PROJECTION(tsea) {
    auto *Q = static_cast<pj_polyhedral_data *>(
        calloc(1, sizeof(pj_polyhedral_data)));
    if (nullptr == Q)
        return pj_default_destructor(P, PROJ_ERR_OTHER /*ENOMEM*/);
    P->opaque = Q;
    P->destructor = polyhedral_destructor;

    const char *net_name = pj_param(P->ctx, P->params, "snet").s;
    const int(*parents)[4] = &nets::tsea::tsea;
    if (net_name != nullptr) {
        if (strcmp(net_name, "tsea") == 0) {
            parents = &nets::tsea::tsea;
        } else if (strcmp(net_name, "star") == 0) {
            parents = &nets::tsea::star;
        } else {
            proj_log_error(P, _("invalid +net (expected tsea or star)"));
            return polyhedral_destructor(P,
                                         PROJ_ERR_INVALID_OP_ILLEGAL_ARG_VALUE);
        }
    }

    const polyhedral::PolyhedralDefaults d = {90.0, 0.0, 0.0};
    polyhedral::load_meshes(Q, polyhedra::tetrahedron, *parents, d);
    return polyhedral_setup(P, d);
}

PROJ_HEAD(dsea, "Dodecahedral Snyder Equal Area")
"\n\tSph&Ell\n\tnet= orient_lat= orient_lon= azi= lat_0= lon_0=";
PJ *PJ_PROJECTION(dsea) {
    auto *Q = static_cast<pj_polyhedral_data *>(
        calloc(1, sizeof(pj_polyhedral_data)));
    if (nullptr == Q)
        return pj_default_destructor(P, PROJ_ERR_OTHER /*ENOMEM*/);
    P->opaque = Q;
    P->destructor = polyhedral_destructor;

    // +net= selects the unfolding tree. The `a5` net shifts orient_lon by
    // -93° to match the LONGITUDE_OFFSET used by the A5 reference codebase.
    const char *net_name = pj_param(P->ctx, P->params, "snet").s;
    const int(*parents)[12] = &nets::dsea::dsea;
    double default_orient_lon = -36.0;
    double default_azi = 240.0;
    if (net_name != nullptr) {
        if (strcmp(net_name, "dsea") == 0) {
            parents = &nets::dsea::dsea;
        } else if (strcmp(net_name, "a5") == 0) {
            parents = &nets::dsea::a5;
            default_orient_lon = -36.0 - 93.0;
        } else if (strcmp(net_name, "crescent") == 0) {
            parents = &nets::dsea::crescent;
        } else if (strcmp(net_name, "flower") == 0) {
            parents = &nets::dsea::flower;
        } else {
            proj_log_error(P, _("invalid +net (expected dsea, a5, crescent, "
                                "or flower)"));
            return polyhedral_destructor(P,
                                         PROJ_ERR_INVALID_OP_ILLEGAL_ARG_VALUE);
        }
    }

    // Defaults rotate the polyhedron so F1's centroid sits at the geographic
    // north pole. orient_lat = atan((1+2A)/2) places V0 on the F1 axis.
    using polyhedra::dodecahedron_constants::A;
    const double default_orient_lat =
        std::atan((1 + 2 * A) / 2.0) * 180.0 / M_PI;
    const polyhedral::PolyhedralDefaults d = {default_orient_lat,
                                              default_orient_lon, default_azi};
    polyhedral::load_meshes(Q, polyhedra::dodecahedron, *parents, d);
    return polyhedral_setup(P, d);
}

PROJ_HEAD(isea, "Icosahedral Snyder Equal Area")
"\n\tSph&Ell\n\torient= orient_lat= orient_lon= azi= lat_0= lon_0=";
PJ *PJ_PROJECTION(isea) {
    auto *Q = static_cast<pj_polyhedral_data *>(
        calloc(1, sizeof(pj_polyhedral_data)));
    if (nullptr == Q)
        return pj_default_destructor(P, PROJ_ERR_OTHER /*ENOMEM*/);
    P->opaque = Q;
    P->destructor = polyhedral_destructor;

    // Standard Snyder ISEA orientation: V0 at authalic latitude arctan(φ) ≈
    // 58.28°N (φ = golden ratio). On WGS84 this corresponds to ~58.40°
    // geodetic — PolyhedralDefaults stores the authalic value; user
    // +orient_lat is geodetic and converted internally. orient_lon = 11.25°
    // on a sphere and 11.20° on an ellipsoid matches Snyder's Figure 12.
    const bool sphere = (P->es == 0.0);
    double orient_lat = 58.282525588539;
    double orient_lon = sphere ? 11.25 : 11.20;
    double azi = 0.0;

    // +orient= shorthand for the two named orientations from Snyder's paper.
    // Individual +orient_lat / +orient_lon / +azi still override.
    const char *orient_name = pj_param(P->ctx, P->params, "sorient").s;
    if (orient_name != nullptr) {
        if (strcmp(orient_name, "isea") == 0) {
            // Defaults above already match.
        } else if (strcmp(orient_name, "pole") == 0) {
            orient_lat = 90.0;
            orient_lon = 0.0;
            azi = 0.0;
        } else {
            proj_log_error(P, _("invalid +orient (expected isea or pole)"));
            return polyhedral_destructor(P,
                                         PROJ_ERR_INVALID_OP_ILLEGAL_ARG_VALUE);
        }
    }

    // Default projected origin is the bbox center (mid-width, mid-height) of
    // the unfold's root face — the "South Africa" upper-band face F8 — rather
    // than its centroid. This places the geometric centre of the 4×5 strip
    // layout at (0, 0). Other polyhedra default to the centroid of the root
    // face.
    const polyhedral::PolyhedralDefaults d = {
        orient_lat, orient_lon, azi, polyhedral::DefaultOrigin::FaceBboxCenter};
    polyhedral::load_meshes(Q, polyhedra::icosahedron, nets::isea::isea, d);
    return polyhedral_setup(P, d);
}
