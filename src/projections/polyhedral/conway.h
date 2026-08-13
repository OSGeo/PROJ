/******************************************************************************
 *
 * Project:  PROJ
 * Purpose:  Conway meta polyhedron operation
 * Author:   Felix Palmer
 *
 ****************************************************************************/

#ifndef POLYHEDRAL_CONWAY_H
#define POLYHEDRAL_CONWAY_H

#include "vec3.h"

namespace polyhedral {

// Mesh struct, both for representing polyhedra & flat nets
template <int NumVertices, int NumFaces, int NumFaceVertices> struct Mesh {
    // List of vertices as 3D vectors
    Vec3 vertices[NumVertices];
    // Faces, indexing above vertices
    int faces[NumFaces][NumFaceVertices];
};

// ConwayMode:
// - Sphere: project centroid + fan vertices onto unit sphere (for polyhedra)
// - Plane: scale centroid to the arithmetic mean (for nets)
enum class ConwayMode { Sphere, Plane };

template <ConwayMode Mode, int NumVertices, int NumFaces, int NumFaceVertices>
inline void conway_meta(const Vec3 (&vertices)[NumVertices],
                        const int (&faces)[NumFaces][NumFaceVertices],
                        Vec3 (&out)[2 * NumFaceVertices * NumFaces][3]) {
    constexpr int FanSize = 2 * NumFaceVertices;
    for (int i = 0; i < NumFaces; i++) {
        Vec3 fan[FanSize];
        Vec3 center{0.0, 0.0, 0.0};
        for (int k = 0; k < NumFaceVertices; k++) {
            const Vec3 va = vertices[faces[i][k]];
            const Vec3 vb = vertices[faces[i][(k + 1) % NumFaceVertices]];
            fan[2 * k] = va;
            fan[2 * k + 1] = vec3_lerp(va, vb, 0.5);
            center = vec3_add(center, va);
        }

        if constexpr (Mode == ConwayMode::Sphere) {
            for (int k = 0; k < FanSize; k++)
                fan[k] = vec3_normalize(fan[k]);
            center = vec3_normalize(center);
        } else {
            center = vec3_scale(center, 1.0 / NumFaceVertices);
        }

        for (int k = 0; k < FanSize; k++) {
            out[FanSize * i + k][0] = center;
            out[FanSize * i + k][1] = fan[(k + 1) % FanSize];
            out[FanSize * i + k][2] = fan[k];
        }
    }
}

} // namespace polyhedral

#endif // POLYHEDRAL_CONWAY_H
