/******************************************************************************
 *
 * Project:  PROJ
 * Purpose:  Net layouts for the TSEA projection — the regular tetrahedron
 *           unfolded into a 2D net.
 *
 *           The default `tsea` net matches Snyder's Figure 8: a horizontal
 *           strip with North America (F0) at the root, then Europe/Africa
 *           (F1), the south-cap face (F3), and Asia/Australia (F2) chained
 *           via the shared edges V0-V3, V2-V3, V1-V2.
 *
 *           The `star` net is the alternative classic layout — the south
 *           face (F3) at the centre with its three V0-touching neighbours
 *           (F0, F1, F2) hung off each of its edges.
 * Author:   Felix Palmer
 *
 ****************************************************************************/

#ifndef NETS_TSEA_TSEA_H
#define NETS_TSEA_TSEA_H

namespace nets {
namespace tsea {

constexpr int tsea[4] = {-1, 0, 3, 1};
constexpr int star[4] = {3, 3, 3, -1};

} // namespace tsea
} // namespace nets

#endif // NETS_TSEA_TSEA_H
