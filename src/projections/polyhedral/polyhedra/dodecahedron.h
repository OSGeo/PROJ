/******************************************************************************
 *
 * Project:  PROJ
 * Purpose:  Regular dodecahedron — 20 vertices, 12 pentagonal faces,
 *           inscribed in the unit sphere with V0 at (0, 0, 1) and V1 along
 *           the +x meridian. The 20 vertices form six layers along z:
 *
 *             z =  1                   V0           (1 vertex)
 *             z =  √5/3   = (4A−1)/3   V1, V4, V5   (layer 1, 3 vertices)
 *             z =  1/3                 6 vertices   (layer 2)
 *             z = −1/3                 6 vertices   (layer 3)
 *             z = −√5/3                3 vertices   (layer 4)
 *             z = −1                   V17          (1 vertex)
 *
 *           where A = (1+√5)/4 = cos(36°) and B = √3/3 = 1/√3 are the only
 *           algebraic constants needed (every other coordinate is a rational
 *           combination of A, B and 1/3).
 * Author:   Felix Palmer
 *
 ****************************************************************************/

#ifndef POLYHEDRA_DODECAHEDRON_H
#define POLYHEDRA_DODECAHEDRON_H

#include "../conway.h"
#include "validation.h"

namespace polyhedra {
namespace dodecahedron_constants {

constexpr double A = 0.809016994374947424102293417183; // (1+√5)/4 = cos(36°)
constexpr double B = 0.577350269189625764509148780502; // √3/3 = 1/√3

constexpr polyhedral::Mesh<20, 12, 5> mesh = {
    // Vertices (unit sphere; V0 at +z, V1 in +x meridian)
    {
        {0.0, 0.0, 1.0},
        {2.0 / 3.0, 0.0, (4 * A - 1) / 3.0},
        {(4 * A - 1) / 3.0, B, 1.0 / 3.0},
        {(2 - 2 * A) / 3.0, 2 * A *B, 1.0 / 3.0},
        {-1.0 / 3.0, B, (4 * A - 1) / 3.0},
        {-1.0 / 3.0, -B, (4 * A - 1) / 3.0},
        {(4 * A - 1) / 3.0, -B, 1.0 / 3.0},
        {(1 + 2 * A) / 3.0, (2 * A - 1) * B, -1.0 / 3.0},
        {(2 * A - 2) / 3.0, 2 * A *B, -1.0 / 3.0},
        {-(1 + 2 * A) / 3.0, (2 * A - 1) * B, 1.0 / 3.0},
        {(2 - 2 * A) / 3.0, -2 * A *B, 1.0 / 3.0},
        {(1 + 2 * A) / 3.0, -(2 * A - 1) * B, -1.0 / 3.0},
        {1.0 / 3.0, B, -(4 * A - 1) / 3.0},
        {-(4 * A - 1) / 3.0, B, -1.0 / 3.0},
        {-(1 + 2 * A) / 3.0, -(2 * A - 1) * B, 1.0 / 3.0},
        {(2 * A - 2) / 3.0, -2 * A *B, -1.0 / 3.0},
        {1.0 / 3.0, -B, -(4 * A - 1) / 3.0},
        {0.0, 0.0, -1.0},
        {-2.0 / 3.0, 0.0, -(4 * A - 1) / 3.0},
        {-(4 * A - 1) / 3.0, -B, -1.0 / 3.0},
    },
    // Faces (CCW from outside). Ordered top-to-bottom, then left-to-right
    // within each band, so the unfolded DSEA net comes out as:
    //   F1                            (north polar cap, around V0)
    //   F2  F3  F4  F5  F6            (upper band: N.Pacific, N.America,
    //                                   Europe, India, Asia)
    //   F7  F8  F9  F10 F11           (lower band: S.Pacific, S.Atlantic,
    //                                   S.Africa, Indian, S.Pacific E)
    //   F12                           (south polar cap, around V17)
    {
        {0, 1, 2, 3, 4},
        {3, 8, 13, 9, 4},
        {0, 4, 9, 14, 5},
        {0, 5, 10, 6, 1},
        {1, 6, 11, 7, 2},
        {2, 7, 12, 8, 3},
        {9, 13, 18, 19, 14},
        {5, 14, 19, 15, 10},
        {6, 10, 15, 16, 11},
        {7, 11, 16, 17, 12},
        {8, 12, 17, 18, 13},
        {15, 19, 18, 17, 16},
    },
};

[[maybe_unused]] constexpr validate_polyhedron<mesh> _validate{};

} // namespace dodecahedron_constants

constexpr auto &dodecahedron = dodecahedron_constants::mesh;

} // namespace polyhedra

#endif // POLYHEDRA_DODECAHEDRON_H
