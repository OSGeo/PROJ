/******************************************************************************
 *
 * Project:  PROJ
 * Purpose:  Conway polyhedron operations.
 *           Provides the "meta" (a.k.a. bevel) operation generalised to
 *           polyhedra with NVF-gon faces, in either 2D (planar net) or
 *           3D (vertices projected onto the unit sphere).
 *           See https://en.wikipedia.org/wiki/Conway_polyhedron_notation
 * Author:   Felix Palmer
 *
 ****************************************************************************/

#ifndef POLYHEDRAL_CONWAY_H
#define POLYHEDRAL_CONWAY_H

#include <cmath>

namespace polyhedral {

// Project a 3-vector onto the unit sphere in place.
inline void normalize3(double v[3]) {
    const double n = std::sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
    v[0] /= n;
    v[1] /= n;
    v[2] /= n;
}

// Apply the Conway "meta" operation to a polyhedron whose faces all have
// NVF vertices. Each face yields 2*NVF triangles fanning out from the face
// centroid. For a face (v_0, ..., v_{NVF-1}) the 2*NVF fan vertices are
//
//     v_0, mid(v_0,v_1), v_1, mid(v_1,v_2), ..., v_{NVF-1}, mid(v_{NVF-1},v_0)
//
// and the emitted triangles are (centroid, fan[k+1], fan[k]) for k = 0..2*NVF-1.
// This preserves the outward winding of the input face, so containment tests
// on the resulting triangulation behave consistently across faces.
//
// DIM == 3: vertices are projected onto the unit sphere (so callers can
// supply unnormalized symmetric Cartesian coordinates, e.g. McCooey-style).
// DIM == 2: planar input — the centroid is the unweighted average of the
// face vertices and no normalization is applied.
//
// Output buffer is sized 2 * NVF * NF triangles. Templated on every array
// dimension so they stay self-describing at the call site.
template <int NV, int NF, int NVF, int DIM>
inline void conway_meta(const double (&V)[NV][DIM], const int (&F)[NF][NVF],
                        double (&out)[2 * NVF * NF][3][DIM]) {
    constexpr int N = 2 * NVF;
    for (int i = 0; i < NF; i++) {
        double fan[N][DIM];
        double center[DIM] = {};
        for (int k = 0; k < NVF; k++) {
            const int ia = F[i][k];
            const int ib = F[i][(k + 1) % NVF];
            for (int d = 0; d < DIM; d++) {
                fan[2 * k][d] = V[ia][d];
                fan[2 * k + 1][d] = 0.5 * (V[ia][d] + V[ib][d]);
                center[d] += V[ia][d];
            }
        }

        if constexpr (DIM == 3) {
            for (int k = 0; k < N; k++)
                normalize3(fan[k]);
            normalize3(center);
        } else {
            for (int d = 0; d < DIM; d++)
                center[d] /= NVF;
        }

        for (int k = 0; k < N; k++) {
            const double *p1 = fan[(k + 1) % N];
            const double *p2 = fan[k];
            for (int d = 0; d < DIM; d++) {
                out[N * i + k][0][d] = center[d];
                out[N * i + k][1][d] = p1[d];
                out[N * i + k][2][d] = p2[d];
            }
        }
    }
}

} // namespace polyhedral

#endif // POLYHEDRAL_CONWAY_H
