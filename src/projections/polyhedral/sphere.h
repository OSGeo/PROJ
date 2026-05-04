/******************************************************************************
 *
 * Project:  PROJ
 * Purpose:  Spherical coordinate helpers for polyhedral projections —
 *           triangle containment, lon/lat ↔ cartesian with orientation.
 * Author:   Felix Palmer
 *
 ****************************************************************************/

#ifndef POLYHEDRAL_SPHERE_H
#define POLYHEDRAL_SPHERE_H

#include "conway.h"
#include "snyder.h"

#include "proj.h"
#include "proj_internal.h"

#include <cmath>

namespace polyhedral {

constexpr int MAX_TRIANGLES = 120;

struct pj_polyhedral_data {
    int n_triangles;

    SphericalTriangle sph_tris[MAX_TRIANGLES];
    FaceTriangle face_tris[MAX_TRIANGLES];

    double orient[3][3];
    double orient_inv[3][3];

    // Authalic latitude conversion (ellipsoidal case only; apa is null for
    // spheres, where geodetic and authalic latitudes coincide).
    double *apa;
    double qp;
};

// Build triangle data from a polyhedron + matching net.
// The `meta` Conway operation is used to cut up the faces
template <int NV_p, int NV_n, int NF, int NFV>
inline void load_meshes(pj_polyhedral_data *Q,
                        const Mesh<NV_p, NF, NFV> &polyhedron,
                        const Mesh<NV_n, NF, NFV> &net) {
    constexpr int N = 2 * NFV * NF;
    Vec3 sph[N][3];
    Vec3 face[N][3];
    conway_meta<ConwayMode::Sphere>(polyhedron.vertices, polyhedron.faces, sph);
    conway_meta<ConwayMode::Plane>(net.vertices, net.faces, face);

    Q->n_triangles = N;
    for (int i = 0; i < N; i++) {
        Q->sph_tris[i] = {sph[i][0], sph[i][1], sph[i][2]};
        Q->face_tris[i] = {
            {face[i][0].x, face[i][0].y},
            {face[i][1].x, face[i][1].y},
            {face[i][2].x, face[i][2].y},
        };
    }
}

// Build rotation matrix from three angles (degrees):
//   lat, lon: the geographic point to rotate to the north pole
//   az:       an azimuthal (z-axis) rotation applied after the above
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

// Convert geodetic lon/lat to unit vector, applying orientation rotation.
// In the ellipsoidal case the latitude is first converted to the authalic
// latitude so that equal-area properties are preserved on the sphere.
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

} // namespace polyhedral

#endif // POLYHEDRAL_SPHERE_H
