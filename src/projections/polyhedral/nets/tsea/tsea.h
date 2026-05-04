/******************************************************************************
 *
 * Project:  PROJ
 * Purpose:  Planar net for the TSEA projection — a regular tetrahedron
 *           unfolded into the plane. The 24 face triangles used by the
 *           projection are obtained at setup time by applying the Conway
 *           "meta" operation (see conway.h) to the 4 faces below.
 * Author:   Felix Palmer
 *
 ****************************************************************************/

#ifndef NETS_TSEA_TSEA_H
#define NETS_TSEA_TSEA_H

namespace nets { namespace tsea { namespace tsea {

// SCALE fixes the absolute size of the net so that total planar area equals
// the unit sphere's surface area (4π) — preserving PROJ's convention that
// equal-area projections with +R=1 emit unit² on the plane per unit² on the
// sphere. With each face an equilateral triangle of side 6W and altitude 6H:
//   4 * (sqrt(3)/4) * (6W)^2 = 4π  ⇒  6W = SCALE = 2 * sqrt(π / sqrt(3)).
// W and H are 1/6 of the side and altitude respectively, chosen so that the
// VERTICES table below is a clean grid of integer multiples.
constexpr double SCALE = 2.69354737417719870e+00; // 2 * sqrt(pi / sqrt(3))
constexpr double W = SCALE / 6.0;
constexpr double H = 0.86602540378443865e+00 * SCALE / 6.0; // sqrt(3)/12 * SCALE

// 6 vertices of the unfolded net. The central face has its centroid at the
// origin; the three outer faces unfold across each of its edges. Each face's
// centroid sits at 2H from each edge and 4H from each opposite vertex.
constexpr double VERTICES[6][2] = {
    {-3 * W,  2 * H},  // 0
    {   0.0, -4 * H},  // 1
    { 3 * W,  2 * H},  // 2
    {-6 * W, -4 * H},  // 3
    {   0.0,  8 * H},  // 4
    { 6 * W, -4 * H},  // 5
};

// 4 triangular faces. The vertex ordering of each face matches the matching
// face in polyhedra::tetrahedron::FACES so that conway_meta produces
// sub-triangles in the same order for both triangulations.
constexpr int FACES[4][3] = {
    {3, 1, 0},
    {4, 0, 2},
    {5, 2, 1},
    {0, 1, 2},
};

} } } // namespace nets::tsea::tsea

#endif // NETS_TSEA_TSEA_H
