/******************************************************************************
 *
 * Project:  PROJ
 * Purpose:  Compile-time validation helper for polyhedron seed meshes.
 * Author:   Felix Palmer
 *
 ****************************************************************************/

#ifndef POLYHEDRA_VALIDATION_H
#define POLYHEDRA_VALIDATION_H

#include "../conway.h"

namespace polyhedra {

template <int NV, int NF, int NFV>
constexpr bool all_vertices_on_unit_sphere(
    const polyhedral::Mesh<NV, NF, NFV> &m) {
    constexpr double tol = 1e-12;
    for (int i = 0; i < NV; i++) {
        const auto &v = m.vertices[i];
        const double err = v.x * v.x + v.y * v.y + v.z * v.z - 1.0;
        if (err < -tol || err > tol)
            return false;
    }
    return true;
}

template <int NV, int NF, int NFV>
constexpr bool face_indices_well_formed(
    const polyhedral::Mesh<NV, NF, NFV> &m) {
    for (int f = 0; f < NF; f++) {
        for (int k = 0; k < NFV; k++) {
            if (m.faces[f][k] < 0 || m.faces[f][k] >= NV)
                return false;
            for (int j = k + 1; j < NFV; j++) {
                if (m.faces[f][k] == m.faces[f][j])
                    return false;
            }
        }
    }
    return true;
}

// Two topological conditions:
// V - E + F = 2 for any closed convex polyhedron,
// E = NF * NFV / 2 (each edge shared by exactly two NFV-gon faces).
template <int NV, int NF, int NFV>
constexpr bool euler_holds(const polyhedral::Mesh<NV, NF, NFV> &) {
    return (NF * NFV) % 2 == 0 && NV - (NF * NFV) / 2 + NF == 2;
}

template <auto &M> struct validate_polyhedron {
    static_assert(M.vertices[0].x == 0.0 && M.vertices[0].y == 0.0 &&
                      M.vertices[0].z == 1.0,
                  "V0 must be at (0, 0, 1)");
    static_assert(all_vertices_on_unit_sphere(M),
                  "all vertices must lie on the unit sphere");
    static_assert(face_indices_well_formed(M),
                  "face vertex indices must be in [0, NV) "
                  "and unique within each face");
    static_assert(euler_holds(M),
                  "polyhedron must satisfy Euler's formula V - E + F = 2");
};

} // namespace polyhedra

#endif // POLYHEDRA_VALIDATION_H
