/******************************************************************************
 *
 * Project:  PROJ
 * Purpose:  Polyhedral projection runtime state — `pj_polyhedral_data`
 *           struct, mesh loading, orientation, triangle containment, and
 *           lon/lat ↔ cartesian conversion with orientation applied.
 * Author:   Felix Palmer
 *
 ****************************************************************************/

#ifndef POLYHEDRAL_STATE_H
#define POLYHEDRAL_STATE_H

#include "conway.h"
#include "snyder.h"
#include "unfold.h"

#include "proj.h"
#include "proj_internal.h"

#include <cmath>
#include <limits>

namespace polyhedral {

constexpr int MAX_TRIANGLES = 120;

struct pj_polyhedral_data {
    int n_triangles;
    int face_vertex_count; // NFV: 3 for triangular faces, 5 for pentagonal
    int num_faces;

    SphericalTriangle sph_tris[MAX_TRIANGLES];
    FaceTriangle face_tris[MAX_TRIANGLES];

    double orient[3][3];
    double orient_inv[3][3];

    int root_face_index; // unfold-tree root; default reference face for
                         // lat_0/lon_0

    // Translation in projected space so the geographic anchor named by
    // +lat_0 / +lon_0 (or its dynamic default) lands at (0, 0).
    // +x_0/+y_0 stack on top via PROJ core.
    double x_offset;
    double y_offset;

    // Authalic latitude state (ellipsoidal case only; apa is null on spheres).
    double *apa;
    double qp;
};

// Projected origin location on chosen reference face when the user does not
// supply +lat_0 / +lon_0.
enum class DefaultOrigin {
    FaceCentroid,   // arithmetic mean of the face's outer vertices in the net
    FaceBboxCenter, // axis-aligned bbox center of the face's outer vertices
};

// Defaults supplied per-projection; user params override these where present.
//
// orient_lat_deg is in *authalic* latitude — this is the frame the rotation
// operates in (the unit sphere is the authalic sphere for equal-area
// projections). User-supplied +orient_lat is geodetic and gets converted to
// authalic before use. On a sphere the two are identical.
struct PolyhedralDefaults {
    double orient_lat_deg;
    double orient_lon_deg;
    double azi_deg;
    DefaultOrigin default_origin = DefaultOrigin::FaceCentroid;
};

// Geographic +z (the orient frame's "up") expressed in polyhedron
// coordinates — the third column of the orient rotation matrix. The unfold
// uses this so the root face comes out with true geographic north up.
inline Vec3 orient_up_in_poly(const PolyhedralDefaults &d) {
    const double lat = d.orient_lat_deg * DEG_TO_RAD;
    const double az = d.azi_deg * DEG_TO_RAD;
    return {-std::cos(az) * std::cos(lat), -std::sin(az) * std::cos(lat),
            std::sin(lat)};
}

// Build triangle data from a polyhedron and the unfold-tree `parents` array.
// The unfold orients the root face so that geographic north (derived from
// the orient defaults) lands on +y in the net.
template <int NV_p, int NF, int NFV>
inline void load_meshes(pj_polyhedral_data *Q,
                        const Mesh<NV_p, NF, NFV> &polyhedron,
                        const int (&parents)[NF], const PolyhedralDefaults &d) {
    const auto net = unfold_net(polyhedron, parents, orient_up_in_poly(d));
    constexpr int N = 2 * NFV * NF;
    Vec3 sph[N][3];
    Vec3 face[N][3];
    conway_meta<ConwayMode::Sphere>(polyhedron.vertices, polyhedron.faces, sph);
    conway_meta<ConwayMode::Plane>(net.vertices, net.faces, face);

    Q->n_triangles = N;
    Q->face_vertex_count = NFV;
    Q->num_faces = NF;
    Q->root_face_index = 0;
    while (Q->root_face_index < NF && parents[Q->root_face_index] != 0)
        Q->root_face_index++;
    for (int i = 0; i < N; i++) {
        Q->sph_tris[i] = {sph[i][0], sph[i][1], sph[i][2]};
        Q->face_tris[i] = {
            {face[i][0].x, face[i][0].y},
            {face[i][1].x, face[i][1].y},
            {face[i][2].x, face[i][2].y},
        };
    }
}

// Reference point on face_idx in the unfolded planar net (centroid or bbox
// center)
inline Face2D face_reference_point(const pj_polyhedral_data *Q, int face_idx,
                                   DefaultOrigin kind) {
    const int nfv = Q->face_vertex_count;
    const int fan_size = 2 * nfv;
    double sum_x = 0.0, sum_y = 0.0;
    double min_x = std::numeric_limits<double>::infinity();
    double max_x = -std::numeric_limits<double>::infinity();
    double min_y = std::numeric_limits<double>::infinity();
    double max_y = -std::numeric_limits<double>::infinity();
    for (int k = 0; k < nfv; k++) {
        const Face2D &v = Q->face_tris[fan_size * face_idx + 2 * k].c;
        sum_x += v.x;
        sum_y += v.y;
        if (v.x < min_x)
            min_x = v.x;
        if (v.x > max_x)
            max_x = v.x;
        if (v.y < min_y)
            min_y = v.y;
        if (v.y > max_y)
            max_y = v.y;
    }
    if (kind == DefaultOrigin::FaceBboxCenter)
        return {0.5 * (min_x + max_x), 0.5 * (min_y + max_y)};
    return {sum_x / nfv, sum_y / nfv};
}

// Build rotation matrix that takes the (lat, lon) point on the authalic unit
// sphere to the north pole, followed by an azimuthal rotation about the
// z-axis. All angles are in degrees; lat is authalic latitude (callers must
// convert from geodetic if needed).
inline void set_orient_from_angles(pj_polyhedral_data *Q, double lat_deg,
                                   double lon_deg, double az_deg) {
    const double lat = lat_deg * DEG_TO_RAD;
    const double lon = lon_deg * DEG_TO_RAD;
    const double az = az_deg * DEG_TO_RAD;
    const double alpha = M_HALFPI - lat;
    const double ca = std::cos(alpha), sa = std::sin(alpha);
    const double cl = std::cos(lon), sl = std::sin(lon);
    const double cz = std::cos(az), sz = std::sin(az);

    const double r00 = ca * cl, r01 = ca * sl, r02 = -sa;
    const double r10 = -sl, r11 = cl, r12 = 0.0;
    const double r20 = sa * cl, r21 = sa * sl, r22 = ca;

    double m[3][3];
    m[0][0] = cz * r00 - sz * r10;
    m[0][1] = cz * r01 - sz * r11;
    m[0][2] = cz * r02 - sz * r12;
    m[1][0] = sz * r00 + cz * r10;
    m[1][1] = sz * r01 + cz * r11;
    m[1][2] = sz * r02 + cz * r12;
    m[2][0] = r20;
    m[2][1] = r21;
    m[2][2] = r22;

    for (int r = 0; r < 3; r++)
        for (int c = 0; c < 3; c++) {
            Q->orient[r][c] = m[r][c];
            Q->orient_inv[r][c] = m[c][r];
        }
}

// Find the triangle containing point v by linear search.
inline int find_triangle(const pj_polyhedral_data *Q, const Vec3 &v) {
    for (int i = 0; i < Q->n_triangles; i++) {
        if (vec3_dot(v, Q->sph_tris[i].a) <= 0)
            continue;
        double t1 = triple_product(v, Q->sph_tris[i].a, Q->sph_tris[i].b);
        double t2 = triple_product(v, Q->sph_tris[i].b, Q->sph_tris[i].c);
        double t3 = triple_product(v, Q->sph_tris[i].c, Q->sph_tris[i].a);
        if ((t1 >= 0 && t2 >= 0 && t3 >= 0) || (t1 <= 0 && t2 <= 0 && t3 <= 0))
            return i;
    }
    return -1;
}

// Convert geodetic lon/lat to a unit vector and apply the orientation
// rotation. On an ellipsoid, latitude is first mapped to the authalic
// latitude so that equal-area is preserved on the sphere.
inline Vec3 lonlat_to_cart(const pj_polyhedral_data *Q, const PJ *P, double lam,
                           double phi) {
    double auth_lat = phi;
    if (P->es != 0.0) {
        auth_lat = pj_authalic_lat(phi, std::sin(phi), std::cos(phi), Q->apa, P,
                                   Q->qp);
    }
    double ca = std::cos(auth_lat), sa = std::sin(auth_lat);
    double cl = std::cos(lam), sl = std::sin(lam);
    double gx = ca * cl, gy = ca * sl, gz = sa;
    return {Q->orient[0][0] * gx + Q->orient[0][1] * gy + Q->orient[0][2] * gz,
            Q->orient[1][0] * gx + Q->orient[1][1] * gy + Q->orient[1][2] * gz,
            Q->orient[2][0] * gx + Q->orient[2][1] * gy + Q->orient[2][2] * gz};
}

// Convert unit vector back to lon/lat, applying inverse orientation.
inline void cart_to_lonlat(const pj_polyhedral_data *Q, const PJ *P,
                           const Vec3 &v, double &lam, double &phi) {
    double gx = Q->orient_inv[0][0] * v.x + Q->orient_inv[0][1] * v.y +
                Q->orient_inv[0][2] * v.z;
    double gy = Q->orient_inv[1][0] * v.x + Q->orient_inv[1][1] * v.y +
                Q->orient_inv[1][2] * v.z;
    double gz = Q->orient_inv[2][0] * v.x + Q->orient_inv[2][1] * v.y +
                Q->orient_inv[2][2] * v.z;

    double auth_lat = aasin(P->ctx, gz);
    phi = (P->es != 0.0) ? pj_authalic_lat_inverse(auth_lat, Q->apa, P, Q->qp)
                         : auth_lat;
    lam = adjlon(std::atan2(gy, gx));
}

// Read +orient_lat / +orient_lon / +azi / +lat_0 / +lon_0 (falling back to
// derived defaults) and build the orientation matrix plus the projected-space
// offset so the chosen geographic anchor lands at (0, 0).
inline bool apply_polyhedral_params(pj_polyhedral_data *Q, PJ *P,
                                    const PolyhedralDefaults &d) {
    // Default is authalic; user input is geodetic and converted below.
    double orient_lat = d.orient_lat_deg;
    double orient_lon = d.orient_lon_deg;
    double azi = d.azi_deg;
    if (pj_param(P->ctx, P->params, "torient_lat").i) {
        const double user_lat_deg =
            pj_param(P->ctx, P->params, "dorient_lat").f;
        if (P->es != 0.0) {
            const double phi = user_lat_deg * DEG_TO_RAD;
            const double auth = pj_authalic_lat(
                phi, std::sin(phi), std::cos(phi), Q->apa, P, Q->qp);
            orient_lat = auth / DEG_TO_RAD;
        } else {
            orient_lat = user_lat_deg;
        }
    }
    if (pj_param(P->ctx, P->params, "torient_lon").i)
        orient_lon = pj_param(P->ctx, P->params, "dorient_lon").f;
    if (pj_param(P->ctx, P->params, "tazi").i)
        azi = pj_param(P->ctx, P->params, "dazi").f;

    set_orient_from_angles(Q, orient_lat, orient_lon, azi);

    // Bypass PROJ core's automatic +lon_0 subtraction. +lat_0/+lon_0 names a
    // geographic point that must be applied through orient, not a plain
    // longitude shift on the input.
    P->lam0 = 0.0;
    P->phi0 = 0.0;
    Q->x_offset = 0.0;
    Q->y_offset = 0.0;

    const bool user_lat_0 = pj_param(P->ctx, P->params, "tlat_0").i != 0;
    const bool user_lon_0 = pj_param(P->ctx, P->params, "tlon_0").i != 0;

    if (user_lat_0 || user_lon_0) {
        // User-supplied geographic anchor: project it through the full orient.
        const double lat_0 =
            user_lat_0 ? pj_param(P->ctx, P->params, "rlat_0").f : 0.0;
        const double lon_0 =
            user_lon_0 ? pj_param(P->ctx, P->params, "rlon_0").f : 0.0;
        Vec3 v = lonlat_to_cart(Q, P, lon_0, lat_0);
        int tri_idx = find_triangle(Q, v);
        if (tri_idx < 0) {
            proj_log_error(P,
                           _("+lat_0 / +lon_0 lies outside the polyhedron's "
                             "coverage; cannot be used as projected origin"));
            return false;
        }
        Face2D r = snyder_fwd(v, Q->sph_tris[tri_idx], Q->face_tris[tri_idx]);
        Q->x_offset = r.x;
        Q->y_offset = r.y;
        return true;
    }

    // Default anchor: a planar reference point on the unfold's root face.
    const Face2D ref =
        face_reference_point(Q, Q->root_face_index, d.default_origin);
    Q->x_offset = ref.x;
    Q->y_offset = ref.y;
    return true;
}

} // namespace polyhedral

#endif // POLYHEDRAL_STATE_H
