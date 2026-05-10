/******************************************************************************
 *
 * Project:  PROJ
 * Purpose:  Regular icosahedron inscribed in the unit sphere, oriented with
 *           V0 at the north pole. The remaining 11 vertices form two rings
 *           of 5 (at z = ±1/√5) staggered by 36° in longitude, plus V11 at
 *           the south pole. Faces are listed in 4 groups of 5 — north cap,
 *           upper band (down-pointing on the sphere), lower band (up-pointing
 *           on the sphere), south cap — chosen to match the canonical ISEA
 *           net unfolding.
 * Author:   Felix Palmer
 *
 ****************************************************************************/

#ifndef POLYHEDRA_ICOSAHEDRON_H
#define POLYHEDRA_ICOSAHEDRON_H

#include "../conway.h"
#include "validation.h"

namespace polyhedra {
namespace icosahedron_constants {

constexpr double A = 0.894427190999915878564; // 2/√5  (ring xy radius)
constexpr double B = 0.447213595499957939282; // 1/√5  (ring |z|)

// cos/sin of multiples of 72° and 36° (used for the two rings).
constexpr double C72 = 0.309016994374947424102; // cos 72°
constexpr double S72 = 0.951056516295153572116; // sin 72°
constexpr double C36 = 0.809016994374947424102; // cos 36°
constexpr double S36 = 0.587785252292473129169; // sin 36°

constexpr polyhedral::Mesh<12, 20, 3> mesh = {
    // Vertices (unit sphere; V0 at +z, V1 in +x meridian)
    {
        {0.0, 0.0, 1.0},
        {-A, 0.0, B},
        {-A * C72, -A *S72, B},
        {A * C36, -A *S36, B},
        {A * C36, A *S36, B},
        {-A * C72, A *S72, B},
        {-A * C36, -A *S36, -B},
        {A * C72, -A *S72, -B},
        {A, 0.0, -B},
        {A * C72, A *S72, -B},
        {-A * C36, A *S36, -B},
        {0.0, 0.0, -1.0},
    },
    // Faces (CCW from outside). Ordered north to south, in groups of 5
    //   F0..F4   north cap (around V0)
    //   F5..F9   upper band (down-pointing)
    //   F10..F14 lower band (up-pointing)
    //   F15..F19 south cap (around V11)
    {
        {0, 1, 2},  {0, 2, 3},  {0, 3, 4},   {0, 4, 5},   {0, 5, 1},
        {1, 6, 2},  {2, 7, 3},  {3, 8, 4},   {4, 9, 5},   {5, 10, 1},
        {6, 7, 2},  {7, 8, 3},  {8, 9, 4},   {9, 10, 5},  {10, 6, 1},
        {11, 7, 6}, {11, 8, 7}, {11, 9, 8},  {11, 10, 9}, {11, 6, 10},
    },
};

[[maybe_unused]] constexpr validate_polyhedron<mesh> _validate{};

} // namespace icosahedron_constants

constexpr auto &icosahedron = icosahedron_constants::mesh;

} // namespace polyhedra

#endif // POLYHEDRA_ICOSAHEDRON_H
