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

// 4 vertices on the unit sphere, with one apex at the north pole.
// Coordinates: (0, 0, 1) and three points at z = -1/3, evenly spaced in
// longitude, with horizontal radius 2*sqrt(2)/3.
constexpr double V[4][3] = {
    { 0.0,                            0.0,                            1.0      }, // 0  apex
    { 0.94280904158206336586779e+00,  0.0,                           -1.0/3.0  }, // 1  2*sqrt(2)/3
    {-0.47140452079103168293390e+00,  0.81649658092772603273243e+00, -1.0/3.0  }, // 2
    {-0.47140452079103168293390e+00, -0.81649658092772603273243e+00, -1.0/3.0  }, // 3
};

// 4 triangular faces, each listed with outward-pointing winding.
// The order here matches the order TSEA's planar net expects.
constexpr int F[4][3] = {
    {0, 3, 1},
    {0, 1, 2},
    {0, 2, 3},
    {1, 3, 2},
};

} // namespace tetrahedron
} // namespace polyhedra

#endif // POLYHEDRA_TETRAHEDRON_H
