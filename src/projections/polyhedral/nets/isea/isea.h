/******************************************************************************
 *
 * Project:  PROJ
 * Purpose:  Net layout for the ISEA projection — the regular icosahedron
 *           unfolded into the canonical Snyder 4×5 strip.
 *
 *           Face IDs are 1-based (matching Snyder); 0 means "no parent"
 *           (the unfold root).
 *
 *           The icosahedron face order in `polyhedra::icosahedron` is
 *           grouped into 4 rows of 5:
 *             F1..F5   north cap   (around V0)
 *             F6..F10  upper band  (down-pointing on the sphere)
 *             F11..F15 lower band  (up-pointing on the sphere)
 *             F16..F20 south cap   (around V11)
 *
 *           F8 is the centred upper-band face — the "South Africa" face in
 *           Snyder's Figure 12 — and is chosen as the root of the unfolding
 *           tree so that the default projected origin is its centroid /
 *           bbox centre, matching the geometric centre of the legacy ISEA
 *           strip.
 *
 *           The unfold tree zig-zags from the root F8 outward across the
 *           upper and lower bands,
 *
 *             F1 ← F6 ← F11 ← F7 ← F12 ← F8 → F13 → F9 → F14 → F10 → F15
 *
 *           with each remaining north-cap face hung off its upper-band
 *           neighbour and each south-cap face hung off its lower-band
 *           neighbour.
 * Author:   Felix Palmer
 *
 ****************************************************************************/

#ifndef NETS_ISEA_ISEA_H
#define NETS_ISEA_ISEA_H

namespace nets {
namespace isea {

constexpr int isea[20] = {
    6, 7, 8, 9, 10,     // F1..F5 north cap, hung off upper band
    11, 12, 0, 13, 14,  // F6, F7 climb back to F1; F8 is root; F9, F10 chain on
    7, 8, 8, 9, 10,     // F11..F15 lower band — left of F8 climbs back, right
                        // chains forward
    11, 12, 13, 14, 15, // F16..F20 south cap, hung off lower band
};

} // namespace isea
} // namespace nets

#endif // NETS_ISEA_ISEA_H
