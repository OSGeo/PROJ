/******************************************************************************
 *
 * Project:  PROJ
 * Purpose:  Net layout for the ISEA projection — the regular icosahedron
 *           unfolded into the canonical Snyder 4×5 strip.
 *
 *           The icosahedron face order in `polyhedra::icosahedron` is
 *           grouped into 4 rows of 5:
 *             F0..F4   north cap   (around V0)
 *             F5..F9   upper band  (down-pointing on the sphere)
 *             F10..F14 lower band  (up-pointing on the sphere)
 *             F15..F19 south cap   (around V11)
 *
 *           F7 is the centred upper-band face — the "South Africa" face in
 *           Snyder's Figure 12 (face 8 in his 1-indexed numbering, our F7
 *           in 0-indexed) — and is chosen as the root of the unfolding tree
 *           so that the default projected origin is its centroid / bbox
 *           centre, matching the geometric centre of the legacy ISEA strip.
 *
 *           The unfold tree zig-zags from the root F7 outward across the
 *           upper and lower bands,
 *
 *             F0 ← F5 ← F10 ← F6 ← F11 ← F7 → F12 → F8 → F13 → F9 → F14
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
    5, 6, 7, 8, 9,      // F0..F4 north cap, hung off upper band
    10, 11, -1, 12, 13, // F5, F6 climb back to F0; F7 is root; F8, F9 chain on
    6, 7, 7, 8, 9,      // F10..F14 lower band — left of F7 climbs back, right
                        // chains forward
    10, 11, 12, 13, 14, // F15..F19 south cap, hung off lower band
};

} // namespace isea
} // namespace nets

#endif // NETS_ISEA_ISEA_H
