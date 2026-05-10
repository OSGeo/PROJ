/******************************************************************************
 *
 * Project:  PROJ
 * Purpose:  Regular tetrahedron inscribed in the unit sphere, oriented with
 *           V0 at the north pole and V1 on the −x meridian (the same
 *           convention used by the icosahedron). The remaining vertices sit
 *           at latitude -arcsin(1/3) ≈ -19.47°, separated by 120° in
 *           longitude (V1 at 180°, V3 at -60°, V2 at +60°). Faces are
 *           listed top-to-bottom and west-to-east:
 *             F0  =  V0 + V1 + V3   covers North America (and the Pacific)
 *             F1  =  V0 + V3 + V2   covers Europe + Africa (and Atlantic)
 *             F2  =  V0 + V2 + V1   covers Asia + Australia (and W Pacific)
 *             F3  =  V1 + V2 + V3   south-cap face opposite V0
 *           The F0–F1 edge V0-V3 lies on the lon=-60° meridian, splitting
 *           South America between the two faces (matching Snyder Figure 8).
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
    // Vertices (unit sphere; V0 at +z, V1 on -x meridian)
    {
        {0.0, 0.0, 1.0},
        {-A, 0.0, -1.0 / 3.0},
        {0.5 * A, B, -1.0 / 3.0},
        {0.5 * A, -B, -1.0 / 3.0},
    },
    // Faces (CCW from outside). F0..F2 share V0, in CCW order around it
    // starting from V1; F3 is the south-cap face opposite V0.
    {
        {0, 1, 3},
        {0, 3, 2},
        {0, 2, 1},
        {1, 2, 3},
    },
};

[[maybe_unused]] constexpr validate_polyhedron<mesh> _validate{};

} // namespace tetrahedron_constants

constexpr auto &tetrahedron = tetrahedron_constants::mesh;

} // namespace polyhedra

#endif // POLYHEDRA_TETRAHEDRON_H
