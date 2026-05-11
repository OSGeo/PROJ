/******************************************************************************
 *
 * Project:  PROJ
 * Purpose:  Net layouts for the DSEA projection. Each constant is a parents
 *           tree over the 12 faces of the dodecahedron: parents[i] stores
 *           the 1-based ID of face F(i+1)'s parent in the unfolding tree,
 *           or 0 if F(i+1) is the root. Face IDs are 1-based to match the
 *           Snyder paper; 0 means "no parent". Every parent–child pair
 *           must share an edge on the dodecahedron.
 *
 *           The dodecahedron face order is top-to-bottom, then left-to-right
 *           within each band:
 *             F1                            north polar cap
 *             F2..F6    upper band          N.Pacific, N.America, Europe,
 *                                           India, Asia
 *             F7..F11   lower band          S.Pacific, S.Atlantic, S.Africa,
 *                                           Indian, S.Pacific E
 *             F12                           south polar cap
 * Author:   Felix Palmer
 *
 ****************************************************************************/

#ifndef NETS_DSEA_DSEA_H
#define NETS_DSEA_DSEA_H

namespace nets {
namespace dsea {

// Snyder's two-rows-of-5 layout (Figure 6): F9 (Africa) is the central face
// of the lower row, with the row extending left and right via zig-zag
// connections through the upper row — directly hinging adjacent lower-row
// faces would curl the band into a crescent, so each next-lower-row face is
// reached through its upper-row neighbour instead.
//   Right:  F9 → F5 → F10 → F6 → F11   (L → U → L → U → L)
//   Left:   F9 → F4 → F8 → F3 → F7 → F2   (L → U → L → U → L → U)
// The two polar caps hang symmetrically above and below the central column:
// F1 (north pole) off F4 (Europe), F12 (south pole) off F9 (Africa).
constexpr int dsea[12] = {4, 7, 8, 0, 9, 10, 3, 4, 4, 5, 6, 9};

// A5 net (re-rooted to match the A5 reference codebase's coordinate origin).
// The visual shape is the same as the published A5 layout; only the parent
// pointers along the path to the new root are reversed.
constexpr int a5[12] = {4, 6, 8, 9, 6, 0, 2, 12, 10, 5, 7, 11};

// Crescent: central spine F1 → F4 → F9 → F12 (north pole → UK/Europe →
// Africa → south pole). Two symmetric "crescent" arcs extend out from F4
// and F9: one through the upper row (F4 → F5 → F6 → F2 → F3) and one
// through the lower row (F9 → F10 → F11 → F7 → F8).
constexpr int crescent[12] = {4, 3, 4, 0, 4, 5, 8, 9, 4, 9, 10, 9};

// Flower: F4 (Europe) at the centre, with F1 (north pole) as one petal and
// F9 (Africa) as the link down to the south pole. F1's four other petals
// (F2, F3, F5, F6) hang off F1; F12's four other petals (F7, F8, F10, F11)
// hang off F12. This routes the polar caps through Africa rather than
// through the Americas.
constexpr int flower[12] = {4, 1, 1, 0, 1, 1, 12, 12, 4, 12, 12, 9};

} // namespace dsea
} // namespace nets

#endif // NETS_DSEA_DSEA_H
