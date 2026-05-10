/******************************************************************************
 *
 * Project:  PROJ
 * Purpose:  Regular tetrahedron inscribed in the unit sphere, oriented with
 *           one vertex at the north pole. The remaining three vertices sit
 *           at latitude -arcsin(1/3) ≈ -19.47°, separated by 120° in
 *           longitude (0°, +120°, -120°). Faces are listed top-to-bottom:
 *           F0..F2 are the three faces sharing V0 (CCW around V0, with V0
 *           in slot 0), and F3 is the south-cap face opposite V0.
 * Author:   Felix Palmer
 *
 ****************************************************************************/

#ifndef POLYHEDRA_TETRAHEDRON_H
#define POLYHEDRA_TETRAHEDRON_H

#include "../conway.h"
#include "validation.h"

namespace polyhedra {
namespace tetrahedron_constants {

constexpr double A = 0.9428090415820635; // sqrt(8/9)
constexpr double B = 0.8164965809277260; // sqrt(2/3)

constexpr polyhedral::Mesh<4, 4, 3> mesh = {
    // Vertices
    {
        {0.0, 0.0, 1.0},
        {A, 0.0, -1.0 / 3.0},
        {-0.5 * A, B, -1.0 / 3.0},
        {-0.5 * A, -B, -1.0 / 3.0},
    },
    // Faces (CCW from outside). F0..F2 share V0, in CCW order around it;
    // F3 is the south-cap face opposite V0.
    {
        {0, 1, 2},
        {0, 2, 3},
        {0, 3, 1},
        {3, 2, 1},
    },
};

[[maybe_unused]] constexpr validate_polyhedron<mesh> _validate{};

} // namespace tetrahedron_constants

constexpr auto &tetrahedron = tetrahedron_constants::mesh;

} // namespace polyhedra

#endif // POLYHEDRA_TETRAHEDRON_H
