/******************************************************************************
 *
 * Project:  PROJ
 * Purpose:  Regular tetrahedron inscribed in the unit sphere, oriented with
 *           V0 at the north pole. The remaining vertices sit at latitude
 *           -arcsin(1/3) ≈ -19.47°, separated by 120° in longitude
 *           (V1 at +60°, V2 at -60°, V3 at 180°). The three V0-touching
 *           faces are listed west-to-east, with the south-cap face slotted
 *           in between (so the default unfold strip F1 — F2 — F3 — F4 reads
 *           North America — Europe/Africa — south cap — Asia/Australia):
 *             F1  =  V0 + V3 + V2   covers North America (and the Pacific)
 *             F2  =  V0 + V2 + V1   covers Europe + Africa (and Atlantic)
 *             F3  =  V1 + V2 + V3   south-cap face opposite V0
 *             F4  =  V0 + V1 + V3   covers Asia + Australia (and W Pacific)
 *           The F1–F2 edge V0-V2 lies on the lon=-60° meridian, splitting
 *           South America between the two faces (matching Snyder Figure 8).
 *           F3's vertices are listed V1, V2, V3 so the unfold's polar-face
 *           fallback (which places face[0] along +y) puts V1 — the +60°
 *           meridian — "up" in the Antarctica-centred tsea net.
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
    // Vertices (unit sphere; V0 at +z, V1 at lon=+60°)
    {
        {0.0, 0.0, 1.0},
        {0.5 * A, B, -1.0 / 3.0},
        {0.5 * A, -B, -1.0 / 3.0},
        {-A, 0.0, -1.0 / 3.0},
    },
    // Faces (CCW from outside). F1, F2, F4 share V0; F3 is the south-cap
    // face opposite V0, listed in V1, V2, V3 order so the unfold places V1 up.
    {
        {0, 3, 2},
        {0, 2, 1},
        {1, 2, 3},
        {0, 1, 3},
    },
};

[[maybe_unused]] constexpr validate_polyhedron<mesh> _validate{};

} // namespace tetrahedron_constants

constexpr auto &tetrahedron = tetrahedron_constants::mesh;

} // namespace polyhedra

#endif // POLYHEDRA_TETRAHEDRON_H
