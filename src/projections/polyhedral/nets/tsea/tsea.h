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

// Half side length S and centroid-to-base distance H of each net face.
// The net is composed of 4 equilateral triangles of side 2S; equal-area
// projection from the unit sphere (area 4π) requires
//   4 * sqrt(3) * S^2 = 4π  ⇒  S = sqrt(π / sqrt(3)),  H = S / sqrt(3).
constexpr double S = 1.34677368708859935e+00; // sqrt(pi / sqrt(3))
constexpr double H = 0.77756015077810714e+00; // sqrt(pi / (3 * sqrt(3)))

// 6 vertices of the unfolded net. The central face has its centroid at the
// origin; the three outer faces unfold across each of its edges.
constexpr double V[6][2] = {
    {  -S,    H},  // 0
    { 0.0, -2*H},  // 1
    {   S,    H},  // 2
    {-2*S, -2*H},  // 3
    { 0.0,  4*H},  // 4
    { 2*S, -2*H},  // 5
};

// 4 triangular faces. The vertex ordering of each face matches the matching
// face in polyhedra::tetrahedron::F so that conway_meta produces sub-triangles
// in the same order for both the planar and spherical triangulations.
constexpr int F[4][3] = {
    {3, 1, 0},
    {4, 0, 2},
    {5, 2, 1},
    {0, 1, 2},
};

} } } // namespace nets::tsea::tsea

#endif // NETS_TSEA_TSEA_H
