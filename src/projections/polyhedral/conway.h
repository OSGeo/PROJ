/******************************************************************************
 *
 * Project:  PROJ
 * Purpose:  Conway polyhedron operations.
 *           Currently provides the "meta" (a.k.a. bevel) operation specialised
 *           to triangular-faced polyhedra projected onto the unit sphere.
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

// Apply the Conway "meta" operation to a polyhedron whose faces are triangles,
// projecting every derived point onto the unit sphere. Each face yields 6
// right triangles, each spanning (face center, edge midpoint, original vertex).
//
// For a face (a, b, c) we walk the boundary
//
//     a  →  mid(a,b)  →  b  →  mid(b,c)  →  c  →  mid(c,a)  →  a
//
// and emit 6 triangles (center, walk[k+1], walk[k]) for k = 0..5. This
// preserves the outward winding of the input face, so spherical containment
// tests on the resulting triangulation behave consistently across faces.
//
// Output buffer is sized 6 * NF triangles. Caller passes V (NV vertices) and
// F (NF triangular faces) as flat arrays; templated on NV/NF so the array
// dimensions stay self-describing at the call site.
template <int NV, int NF>
inline void meta_spherical(const double (&V)[NV][3], const int (&F)[NF][3],
                           double (&out)[6 * NF][3][3]) {
    for (int i = 0; i < NF; i++) {
        const int ia = F[i][0], ib = F[i][1], ic = F[i][2];
        const double *Va = V[ia], *Vb = V[ib], *Vc = V[ic];

        double walk[6][3];
        for (int d = 0; d < 3; d++) {
            walk[0][d] = Va[d];
            walk[2][d] = Vb[d];
            walk[4][d] = Vc[d];
            walk[1][d] = 0.5 * (Va[d] + Vb[d]);
            walk[3][d] = 0.5 * (Vb[d] + Vc[d]);
            walk[5][d] = 0.5 * (Vc[d] + Va[d]);
        }
        normalize3(walk[1]);
        normalize3(walk[3]);
        normalize3(walk[5]);

        double center[3] = {(Va[0] + Vb[0] + Vc[0]) / 3.0,
                            (Va[1] + Vb[1] + Vc[1]) / 3.0,
                            (Va[2] + Vb[2] + Vc[2]) / 3.0};
        normalize3(center);

        for (int k = 0; k < 6; k++) {
            const double *p1 = walk[(k + 1) % 6];
            const double *p2 = walk[k];
            for (int d = 0; d < 3; d++) {
                out[6 * i + k][0][d] = center[d];
                out[6 * i + k][1][d] = p1[d];
                out[6 * i + k][2][d] = p2[d];
            }
        }
    }
}

} // namespace polyhedral

#endif // POLYHEDRAL_CONWAY_H
