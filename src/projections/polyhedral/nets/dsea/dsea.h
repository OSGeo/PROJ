/******************************************************************************
 *
 * Project:  PROJ
 * Purpose:  Net layouts for the DSEA projection. Each constant is a parents
 *           tree over the 12 faces of the dodecahedron: parents[i] is face
 *           i's parent in the unfolding tree, or -1 if i is the root. Every
 *           parent–child pair must share an edge on the dodecahedron.
 *
 *           All four layouts are rooted at F2 — the equatorial face whose
 *           centroid lands over Southern Africa under the default DSEA
 *           orient. F0 (the polar face around V0) and F11 (around V17) are
 *           hung off F1 and F8 respectively, the upper- and lower-band faces
 *           that cover Europe and the South Pacific.
 * Author:   Felix Palmer
 *
 ****************************************************************************/

#ifndef NETS_DSEA_DSEA_H
#define NETS_DSEA_DSEA_H

namespace nets {
namespace dsea {

// Snyder's two-rows-of-5 layout (Figure 6): F2 (Africa) is the central face
// of the lower row, with the row extending left and right via zig-zag
// connections through the upper row — directly hinging adjacent lower-row
// faces would curl the band into a crescent, so each next-lower-row face is
// reached through its upper-row neighbour instead.
//   Right:  F2 → F3 → F4 → F5 → F6     (L → U → L → U → L)
//   Left:   F2 → F1 → F10 → F9 → F8 → F7   (L → U → L → U → L → U)
// The two polar caps hang symmetrically above and below the central column:
// F0 (north pole) off F1 (Europe), F11 (south pole) off F2 (Africa).
constexpr int dsea[12] = {1, 2, -1, 2, 3, 4, 5, 8, 9, 10, 1, 2};

// The other layouts are the originals re-rooted at F2 (parent pointers
// reversed along the F2 → F0 path in the original tree); their visual
// "shape" is otherwise unchanged.
constexpr int a5[12] = {1, 2, -1, 4, 2, 3, 8, 5, 7, 10, 11, 6};
constexpr int crescent[12] = {3, 3, -1, 4, 2, 3, 4, 5, 6, 1, 2, 4};
constexpr int flower[12] = {9, 0, -1, 0, 11, 0, 11, 0, 11, 10, 11, 2};

} // namespace dsea
} // namespace nets

#endif // NETS_DSEA_DSEA_H
