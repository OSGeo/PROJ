/******************************************************************************
 *
 * Project:  PROJ
 * Purpose:  Net layouts for the TSEA projection — the regular tetrahedron
 *           unfolded into a 2D net.
 *
 *           Face IDs are 1-based (matching Snyder); 0 means "no parent"
 *           (the unfold root).
 *
 *           The default `tsea` net matches Snyder's Figure 8: a horizontal
 *           strip with North America (F1) at the root, then Europe/Africa
 *           (F2), the south-cap face (F4), and Asia/Australia (F3) chained
 *           via the shared edges V0-V3, V2-V3, V1-V2.
 *
 *           The `star` net is the alternative classic layout — the south
 *           face (F4) at the centre with its three V0-touching neighbours
 *           (F1, F2, F3) hung off each of its edges.
 * Author:   Felix Palmer
 *
 ****************************************************************************/

#ifndef NETS_TSEA_TSEA_H
#define NETS_TSEA_TSEA_H

namespace nets {
namespace tsea {

constexpr int tsea[4] = {0, 1, 4, 2};
constexpr int star[4] = {4, 4, 4, 0};

} // namespace tsea
} // namespace nets

#endif // NETS_TSEA_TSEA_H
