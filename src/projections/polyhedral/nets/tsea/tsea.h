/******************************************************************************
 *
 * Project:  PROJ
 * Purpose:  Net for TSEA projection — a regular tetrahedron unfolded
 *           into the plane.
 * Author:   Felix Palmer
 *
 ****************************************************************************/

#ifndef NETS_TSEA_TSEA_H
#define NETS_TSEA_TSEA_H

namespace nets {
namespace tsea {
namespace tsea {

// SCALE factor to normalize net area to unit sphere surface area (4π).
constexpr double SCALE = 0.448924562362866; // sqrt(π / sqrt(3)) / 3
constexpr double W = SCALE;
constexpr double H = 0.86602540378443865e+00 * W; // cos(30)

// 6 vertices of the unfolded net, centered on origin.
constexpr double VERTICES[6][2] = {
    {-3 * W, 2 * H},  {0.0, -4 * H}, {3 * W, 2 * H},
    {-6 * W, -4 * H}, {0.0, 8 * H},  {6 * W, -4 * H},
};

// 4 triangular faces, referencing above vertices
constexpr int FACES[4][3] = {
    {4, 0, 2},
    {0, 3, 1},
    {2, 1, 5},
    {1, 2, 0},
};

} // namespace tsea
} // namespace tsea
} // namespace nets

#endif // NETS_TSEA_TSEA_H
