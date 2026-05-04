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
// NumFaceVertices (NFV) vertices. Each face yields 2*NFV triangles fanning
// out from the face centroid. For a face (v_0, ..., v_{NFV-1}) the 2*NFV
// fan vertices are
//
//     v_0, mid(v_0,v_1), v_1, mid(v_1,v_2), ..., v_{NFV-1}, mid(v_{NFV-1},v_0)
//
// and the emitted triangles are (centroid, fan[k+1], fan[k]) for k in
// [0, 2*NFV). This preserves the outward winding of the input face, so
// containment tests on the resulting triangulation behave consistently.
//
// Dim == 3: vertices are projected onto the unit sphere (so callers can
// supply unnormalized symmetric Cartesian coordinates, e.g. McCooey-style).
// Dim == 2: planar input — the centroid is the unweighted average of the
// face vertices and no normalization is applied.
//
// Output buffer is sized 2 * NFV * NumFaces triangles. Templated on every
// array dimension so they stay self-describing at the call site.
template <int NumVertices, int NumFaces, int NumFaceVertices, int Dim>
inline void conway_meta(
    const double (&vertices)[NumVertices][Dim],
    const int (&faces)[NumFaces][NumFaceVertices],
    double (&out)[2 * NumFaceVertices * NumFaces][3][Dim]) {
    constexpr int FanSize = 2 * NumFaceVertices;
    for (int i = 0; i < NumFaces; i++) {
        double fan[FanSize][Dim];
        double center[Dim] = {};
        for (int k = 0; k < NumFaceVertices; k++) {
            const int ia = faces[i][k];
            const int ib = faces[i][(k + 1) % NumFaceVertices];
            for (int d = 0; d < Dim; d++) {
                fan[2 * k][d] = vertices[ia][d];
                fan[2 * k + 1][d] = 0.5 * (vertices[ia][d] + vertices[ib][d]);
                center[d] += vertices[ia][d];
            }
        }

        if constexpr (Dim == 3) {
            for (int k = 0; k < FanSize; k++)
                normalize3(fan[k]);
            normalize3(center);
        } else {
            for (int d = 0; d < Dim; d++)
                center[d] /= NumFaceVertices;
        }

        for (int k = 0; k < FanSize; k++) {
            const double *p1 = fan[(k + 1) % FanSize];
            const double *p2 = fan[k];
            for (int d = 0; d < Dim; d++) {
                out[FanSize * i + k][0][d] = center[d];
                out[FanSize * i + k][1][d] = p1[d];
                out[FanSize * i + k][2][d] = p2[d];
            }
        }
    }
}

} // namespace polyhedral

#endif // POLYHEDRAL_CONWAY_H
