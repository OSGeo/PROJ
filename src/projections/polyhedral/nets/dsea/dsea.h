/******************************************************************************
 *
 * Project:  PROJ
 * Purpose:  Net layouts for the DSEA projection. Each constant is a parents
 *           tree over the 12 faces of the dodecahedron: parents[i] is face
 *           i's parent in the unfolding tree, or -1 if i is the root. Every
 *           parent–child pair must share an edge on the dodecahedron.
 *
 *           The dodecahedron face order in `polyhedra::dodecahedron` is
 *           chosen so the canonical DSEA net is a simple chain — F0 is the
 *           cap around V0, F1..F10 zig-zag around the middle band, F11 is
 *           the cap around V17 — so dsea[i] = i - 1. The other layouts
 *           (a5, crescent, flower) are valid parents trees over the
 *           same face indices.
 * Author:   Felix Palmer
 *
 ****************************************************************************/

#ifndef NETS_DSEA_DSEA_H
#define NETS_DSEA_DSEA_H

namespace nets {
namespace dsea {

constexpr int dsea[12] = {-1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
constexpr int a5[12] = {-1, 0, 1, 4, 2, 3, 8, 5, 7, 10, 11, 6};
constexpr int crescent[12] = {-1, 3, 4, 0, 3, 3, 4, 5, 6, 1, 2, 4};
constexpr int flower[12] = {-1, 0, 11, 0, 11, 0, 11, 0, 11, 0, 9, 10};

} // namespace dsea
} // namespace nets

#endif // NETS_DSEA_DSEA_H
