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
 *           The unfold tree zig-zags through the upper and lower bands,
 *
 *             F0 → F5 → F10 → F6 → F11 → F7 → F12 → F8 → F13 → F9 → F14
 *
 *           and hangs each remaining north-cap face off its upper-band
 *           neighbour and each south-cap face off its lower-band neighbour.
 * Author:   Felix Palmer
 *
 ****************************************************************************/

#ifndef NETS_ISEA_ISEA_H
#define NETS_ISEA_ISEA_H

namespace nets {
namespace isea {

constexpr int isea[20] = {
    -1,  6,  7,  8,  9, // F0 root; F1..F4 north cap, hung off upper band
     0, 10, 11, 12, 13, // F5 child of F0; F6..F9 chained via lower band
     5,  6,  7,  8,  9, // F10 child of F5; F11..F14 chained via upper band
    10, 11, 12, 13, 14, // F15..F19 south cap, hung off lower band
};

} // namespace isea
} // namespace nets

#endif // NETS_ISEA_ISEA_H
