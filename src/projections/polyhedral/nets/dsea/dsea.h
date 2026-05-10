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

// A5 net (re-rooted at F3 — the Middle East face — to match the A5
// reference codebase's coordinate origin). The visual shape is the same as
// the published A5 layout; only the parent pointers along the F3 → F0 path
// of the original tree are reversed.
constexpr int a5[12] = {1, 2, 4, 5, 3, -1, 8, 5, 7, 10, 11, 6};

// Crescent: central spine F0 → F1 → F2 → F11 (north pole → UK/Europe →
// Africa → south pole). Two symmetric "crescent" arcs extend out from F1
// and F2: one through the upper row (F1 → F3 → F5 → F7 → F9) and one
// through the lower row (F2 → F4 → F6 → F8 → F10).
constexpr int crescent[12] = {1, -1, 1, 1, 2, 3, 4, 9, 10, 1, 2, 2};

// Flower: F2 (Africa) at the centre. Both polar caps hang via Africa —
// F11 (south pole) directly off F2, F0 (north pole) via F1 (Europe). F0's
// four other petals (F3, F5, F7, F9) hang off F0; F11's four other petals
// (F4, F6, F8, F10) hang off F11. This avoids routing the polar caps
// through the Americas (as the original flower did).
constexpr int flower[12] = {1, -1, 1, 0, 11, 0, 11, 0, 11, 0, 11, 2};

} // namespace dsea
} // namespace nets

#endif // NETS_DSEA_DSEA_H
