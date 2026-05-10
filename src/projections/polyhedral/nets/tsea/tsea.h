/******************************************************************************
 *
 * Project:  PROJ
 * Purpose:  Net layout for the TSEA projection — the regular tetrahedron
 *           unfolded into a "star" net with the south-cap face (F3) at the
 *           centre and its three V0-touching neighbours (F0, F1, F2) as
 *           petals.
 * Author:   Felix Palmer
 *
 ****************************************************************************/

#ifndef NETS_TSEA_TSEA_H
#define NETS_TSEA_TSEA_H

namespace nets {
namespace tsea {

constexpr int tsea[4] = {3, 3, 3, -1};

} // namespace tsea
} // namespace nets

#endif // NETS_TSEA_TSEA_H
