/******************************************************************************
 *
 * Project:  PROJ
 * Purpose:  Net layouts for the DSEA projection. Each constant is a parents
 *           tree over the 12 faces of the dodecahedron: parents[i] is face
 *           i's parent in the unfolding tree, or -1 if i is the root. Every
 *           parent–child pair must share an edge on the dodecahedron.
 *
 *           The dodecahedron face order is top-to-bottom, then left-to-right
 *           within each band:
 *             F0                            north polar cap
 *             F1..F5    upper band          N.Pacific, N.America, Europe,
 *                                           India, Asia
 *             F6..F10   lower band          S.Pacific, S.Atlantic, S.Africa,
 *                                           Indian, S.Pacific E
 *             F11                           south polar cap
 * Author:   Felix Palmer
 *
 ****************************************************************************/

#ifndef NETS_DSEA_DSEA_H
#define NETS_DSEA_DSEA_H

namespace nets {
namespace dsea {

// Snyder's two-rows-of-5 layout (Figure 6): F8 (Africa) is the central face
// of the lower row, with the row extending left and right via zig-zag
// connections through the upper row — directly hinging adjacent lower-row
// faces would curl the band into a crescent, so each next-lower-row face is
// reached through its upper-row neighbour instead.
//   Right:  F8 → F4 → F9 → F5 → F10   (L → U → L → U → L)
//   Left:   F8 → F3 → F7 → F2 → F6 → F1   (L → U → L → U → L → U)
// The two polar caps hang symmetrically above and below the central column:
// F0 (north pole) off F3 (Europe), F11 (south pole) off F8 (Africa).
constexpr int dsea[12] = {3, 6, 7, 8, 8, 9, 2, 3, -1, 4, 5, 8};

// A5 net (re-rooted to match the A5 reference codebase's coordinate origin).
// The visual shape is the same as the published A5 layout; only the parent
// pointers along the path to the new root are reversed.
constexpr int a5[12] = {3, 5, 7, 8, 5, -1, 1, 11, 9, 4, 6, 10};

// Crescent: central spine F0 → F3 → F8 → F11 (north pole → UK/Europe →
// Africa → south pole). Two symmetric "crescent" arcs extend out from F3
// and F8: one through the upper row (F3 → F4 → F5 → F1 → F2) and one
// through the lower row (F8 → F9 → F10 → F6 → F7).
constexpr int crescent[12] = {3, 2, 3, -1, 3, 4, 7, 8, 3, 8, 9, 8};

// Flower: F3 (Europe) at the centre, with F0 (north pole) as one petal and
// F8 (Africa) as the link down to the south pole. F0's four other petals
// (F1, F2, F4, F5) hang off F0; F11's four other petals (F6, F7, F9, F10)
// hang off F11. This routes the polar caps through Africa rather than
// through the Americas.
constexpr int flower[12] = {3, 0, 0, -1, 0, 0, 11, 11, 3, 11, 11, 8};

} // namespace dsea
} // namespace nets

#endif // NETS_DSEA_DSEA_H
