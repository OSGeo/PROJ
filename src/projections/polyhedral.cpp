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
#include "polyhedral/nets/tsea/tsea.h"
#include "polyhedral/polyhedra/tetrahedron.h"
#include "polyhedral/sphere.h"

#include "proj.h"
#include "proj_internal.h"

#include <cmath>
#include <cstdlib>

using polyhedral::pj_polyhedral_data;

// Shared forward projection
static PJ_XY polyhedral_fwd(PJ_LP lp, PJ *P) {
    PJ_XY xy = {0.0, 0.0};
    auto *Q = static_cast<pj_polyhedral_data *>(P->opaque);

    polyhedral::Vec3 v = polyhedral::lonlat_to_cart(Q, P, lp.lam, lp.phi);

    int tri_idx = polyhedral::find_triangle(Q, v);
    if (tri_idx < 0) {
        proj_errno_set(P, PROJ_ERR_COORD_TRANSFM_OUTSIDE_PROJECTION_DOMAIN);
        return proj_coord_error().xy;
    }

    polyhedral::Face2D result =
        polyhedral::snyder_fwd(v, Q->sph_tris[tri_idx], Q->face_tris[tri_idx]);

    xy.x = result.x;
    xy.y = result.y;

    return xy;
}

// Shared inverse projection
static PJ_LP polyhedral_inv(PJ_XY xy, PJ *P) {
    PJ_LP lp = {0.0, 0.0};
    auto *Q = static_cast<pj_polyhedral_data *>(P->opaque);

    for (int i = 0; i < Q->n_triangles; i++) {
        polyhedral::Barycentric bary =
            polyhedral::face_to_barycentric({xy.x, xy.y}, Q->face_tris[i]);
        if (bary.u >= -1e-10 && bary.v >= -1e-10 && bary.w >= -1e-10) {
            polyhedral::Vec3 v = polyhedral::snyder_inv(
                {xy.x, xy.y}, Q->face_tris[i], Q->sph_tris[i]);
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

static PJ *polyhedral_setup(PJ *P) {
    auto *Q = static_cast<pj_polyhedral_data *>(P->opaque);
    if (P->es != 0.0) {
        Q->apa = pj_authalic_lat_compute_coeffs(P->n);
        if (nullptr == Q->apa)
            return polyhedral_destructor(P, PROJ_ERR_OTHER /*ENOMEM*/);
        Q->qp = pj_authalic_lat_q(1.0, P);
    }
    P->fwd = polyhedral_fwd;
    P->inv = polyhedral_inv;
    return P;
}

PROJ_HEAD(tsea, "Tetrahedral Snyder Equal Area") "\n\tSph&Ell";
PJ *PJ_PROJECTION(tsea) {
    auto *Q = static_cast<pj_polyhedral_data *>(
        calloc(1, sizeof(pj_polyhedral_data)));
    if (nullptr == Q)
        return pj_default_destructor(P, PROJ_ERR_OTHER /*ENOMEM*/);
    P->opaque = Q;
    P->destructor = polyhedral_destructor;

    double spherical_triangles[24][3][3];
    polyhedral::conway_meta(polyhedra::tetrahedron::VERTICES,
                            polyhedra::tetrahedron::FACES,
                            spherical_triangles);

    double face_triangles[24][3][2];
    polyhedral::conway_meta(nets::tsea::tsea::VERTICES,
                            nets::tsea::tsea::FACES, face_triangles);

    polyhedral::load_triangles(Q, spherical_triangles, face_triangles);

    const double apex_lat_deg = std::asin(1.0 / std::sqrt(3.0)) * RAD_TO_DEG;
    polyhedral::set_orient_from_angles(Q, apex_lat_deg, 120.0, 135.0);

    return polyhedral_setup(P);
}
