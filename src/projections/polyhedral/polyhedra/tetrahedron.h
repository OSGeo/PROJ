/******************************************************************************
 *
 * Project:  PROJ
 * Purpose:  Regular tetrahedron inscribed in the unit sphere.
 *           Seed polyhedron for TSEA — the 24 spherical triangles used by
 *           the projection are obtained at setup time by applying the Conway
 *           "meta" operation (see conway.h) to the 4 faces below.
 * Author:   Felix Palmer
 *
 ****************************************************************************/

#ifndef POLYHEDRA_TETRAHEDRON_H
#define POLYHEDRA_TETRAHEDRON_H

namespace polyhedra {
namespace tetrahedron {

// 4 vertices in the symmetric Cartesian form (C0 = sqrt(2)/4). The
// tetrahedron is centered at the origin; vertices lie on a sphere of
// radius sqrt(6)/4 and are normalized to the unit sphere by conway_meta.
constexpr double C0 = 0.35355339059327376220042e+00; // sqrt(2) / 4

constexpr double VERTICES[4][3] = {
    { C0, -C0,  C0}, // 0
    { C0,  C0, -C0}, // 1
    {-C0,  C0,  C0}, // 2
    {-C0, -C0, -C0}, // 3
};

// 4 triangular faces, each listed with outward-pointing winding.
// The order here matches the order TSEA's planar net expects.
constexpr int FACES[4][3] = {
    {0, 3, 1},
    {0, 1, 2},
    {0, 2, 3},
    {1, 3, 2},
};

} // namespace tetrahedron
} // namespace polyhedra

#endif // POLYHEDRA_TETRAHEDRON_H
