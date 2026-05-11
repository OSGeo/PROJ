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
 *           F1 — F2 — F4 — F3 (North America — Europe/Africa — south cap —
 *           Asia/Australia), but rerooted at F4 so the projected (0, 0)
 *           lands on the south-cap face (Antarctica) — more central than
 *           Snyder's original F1 (West-coast-of-Mexico) origin. The strip
 *           shape and shared edges are identical to Snyder's figure; only
 *           the choice of root face moves.
 *
 *           The `star` net is the alternative layout — F4 (south cap) at
 *           the centre with its three V0-touching neighbours (F1, F2, F3)
 *           hung off each of its edges in a three-way fan.
 * Author:   Felix Palmer
 *
 ****************************************************************************/

#ifndef NETS_TSEA_TSEA_H
#define NETS_TSEA_TSEA_H

namespace nets {
namespace tsea {

constexpr int tsea[4] = {2, 4, 4, 0};
constexpr int star[4] = {4, 4, 4, 0};

} // namespace tsea
} // namespace nets

#endif // NETS_TSEA_TSEA_H
