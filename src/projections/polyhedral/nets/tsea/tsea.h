/******************************************************************************
 *
 * Project:  PROJ
 * Purpose:  Net layouts for the TSEA projection — the regular tetrahedron
 *           unfolded into a 2D net.
 *
 *           Face IDs are 1-based (matching Snyder); 0 means "no parent"
 *           (the unfold root).
 *
 *           The default `tsea` net is Snyder's Figure 8 horizontal strip
 *           F1 — F2 — F3 — F4 (North America — Europe/Africa — south cap —
 *           Asia/Australia), rerooted at F3 so the projected (0, 0) lands
 *           on the south-cap face (Antarctica).
 *
 *           The `star` net is the alternative layout — F3 (south cap) at
 *           the centre with its three V0-touching neighbours (F1, F2, F4)
 *           hung off each of its edges in a three-way fan.
 * Author:   Felix Palmer
 *
 ****************************************************************************/

#ifndef NETS_TSEA_TSEA_H
#define NETS_TSEA_TSEA_H

namespace nets {
namespace tsea {

constexpr int tsea[4] = {2, 3, 0, 3};
constexpr int star[4] = {3, 3, 0, 3};

} // namespace tsea
} // namespace nets

#endif // NETS_TSEA_TSEA_H
