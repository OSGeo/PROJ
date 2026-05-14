/******************************************************************************
 *
 * Project:  PROJ
 * Purpose:  Unfold a 3D polyhedron Mesh into a planar net Mesh by laying each
 *           face flat in its own local 2D frame and chaining 2D rigid
 *           transforms over a parents-tree of faces.
 * Author:   Felix Palmer
 *
 ****************************************************************************/

#ifndef POLYHEDRAL_UNFOLD_H
#define POLYHEDRAL_UNFOLD_H

#include "conway.h"
#include "vec3.h"

#include "proj_internal.h"

#include <cmath>

namespace polyhedral {

// Number of net vertices produced by unfolding NF faces with NFV vertices each
template <int NF, int NFV> constexpr int unfolded_vertex_count() {
    return NFV + (NFV - 2) * (NF - 1);
}

// For a regular face inscribed on the unit sphere centred at the origin, the
// face's outward unit normal is just the centroid direction, so we don't
// need a separate face_normal helper.
template <int NV, int NFV>
inline Vec3 face_centroid(const Vec3 (&vertices)[NV], const int (&face)[NFV]) {
    Vec3 c{0.0, 0.0, 0.0};
    for (int k = 0; k < NFV; k++)
        c = vec3_add(c, vertices[face[k]]);
    return vec3_scale(c, 1.0 / NFV);
}

// Rigid transform from a polyhedron face's plane to the 2D plane (z=0),
// sending poly_a → net_a and orienting poly_a→poly_b along net_a→net_b.
// poly_a and poly_b must lie on the face plane; only the direction of
// net_b − net_a is used (the 2D distance is derived from the 3D one).
inline Mat4 mat4_rigid_transform(const Vec3 &normal, const Vec3 &poly_a,
                                 const Vec3 &net_a, const Vec3 &poly_b,
                                 const Vec3 &net_b) {
    const Vec3 u = vec3_normalize(vec3_subtract(poly_b, poly_a));
    const Vec3 v = vec3_cross(normal, u);
    const Vec3 edge_direction = vec3_normalize(vec3_subtract(net_b, net_a));
    const Vec3 r0 = vec3_subtract(vec3_scale(u, edge_direction.x),
                                  vec3_scale(v, edge_direction.y));
    const Vec3 r1 = vec3_add(vec3_scale(u, edge_direction.y),
                             vec3_scale(v, edge_direction.x));
    const double t0 = net_a.x - vec3_dot(r0, poly_a);
    const double t1 = net_a.y - vec3_dot(r1, poly_a);
    return {{{r0.x, r0.y, r0.z, t0},
             {r1.x, r1.y, r1.z, t1},
             {0.0, 0.0, 0.0, 0.0},
             {0.0, 0.0, 0.0, 1.0}}};
}

template <int NFV>
inline double polygon_area_2d(const Vec3 *verts, const int (&face)[NFV]) {
    double a = 0.0;
    for (int k = 0; k < NFV; k++)
        a += vec3_cross(verts[face[k]], verts[face[(k + 1) % NFV]]).z;
    return std::fabs(a) * 0.5;
}

// Unit vector in Z
inline constexpr Vec3 z{0.0, 0.0, 1.0};

// Unfold a polyhedron Mesh into a planar net Mesh.
//
// Faces are referenced by 1-based IDs F1..F_NF (matching Snyder's paper
// numbering). `parents[i]` holds the parent of face F(i+1) in the unfolding
// tree as a 1-based face ID, or 0 if F(i+1) is the root. Exactly one entry
// must be 0. The C array indexing stays 0-based; only the values it stores
// are 1-based.
//
// The polyhedron faces must have consistent CCW winding from the outside.
// Each child face is hinged about its shared edge with its parent; the
// algorithm relies on shared edges being traversed in opposite directions
// in the two adjacent faces (which is the case for any closed polyhedron
// with consistent outward winding).
//
// The resulting net is auto-scaled so its total area equals 4π (i.e. the
// area of the unit sphere) and translated so the root face's centroid lies
// at the origin. The root face is laid out "north-up": geographic north
// (+z), projected onto its face plane, points along +y; for a root face
// centred on the rotation axis the slot[0] vertex is placed up instead.
template <int NV_p, int NF, int NFV>
inline Mesh<unfolded_vertex_count<NF, NFV>(), NF, NFV>
unfold_net(const Mesh<NV_p, NF, NFV> &polyhedron, const int (&parents)[NF],
           const Vec3 &up_dir = z) {
    constexpr int NV_n = unfolded_vertex_count<NF, NFV>();
    Mesh<NV_n, NF, NFV> net{};
    int n_verts = 0;

    // Find and place root face (the entry whose 1-based parent ID is 0).
    int root = 0;
    while (parents[root] != 0)
        root++;

    // Place the root face "up-direction-up": project up_dir onto the root
    // face's plane, then orient so that direction lands on +y. up_dir is
    // typically geographic north expressed in polyhedron coordinates (the
    // third column of the orient matrix), so the root face comes out with
    // true geographic north up. Polar fallback (face on the rotation
    // axis): aim from the centroid out toward slot[0] instead.

    // Destination in 2d net (origin and unit step in +y)
    const Vec3 origin_net = {0.0, 0.0, 0.0};
    const Vec3 offset_net = {0.0, 1.0, 0.0};

    const Vec3 origin_poly =
        face_centroid(polyhedron.vertices, polyhedron.faces[root]);
    const Vec3 normal = vec3_normalize(origin_poly);

    // Vector in up_dir direction constrained to the face plane
    const Vec3 step_poly =
        vec3_subtract(up_dir, vec3_scale(normal, vec3_dot(up_dir, normal)));
    Vec3 offset_poly;
    if (vec3_length(step_poly) < 1e-12)
        // Fallback to 1st vertex at pole
        offset_poly = polyhedron.vertices[polyhedron.faces[root][0]];
    else
        offset_poly = vec3_add(origin_poly, step_poly);

    const Mat4 root_face_to_plane = mat4_rigid_transform(
        normal, origin_poly, origin_net, offset_poly, offset_net);
    for (int k = 0; k < NFV; k++) {
        Vec3 face_vertex = polyhedron.vertices[polyhedron.faces[root][k]];
        net.vertices[n_verts] =
            vec3_apply_mat4(root_face_to_plane, face_vertex);
        net.faces[root][k] = n_verts++;
    }

    // Initialize queue with direct children of parent face. `root` is a
    // 0-based array index; `parents[i]` is a 1-based face ID.
    int queue[NF];
    int q_head = 0, q_tail = 0;
    for (int i = 0; i < NF; i++) {
        if (parents[i] == root + 1)
            queue[q_tail++] = i;
        }
    }  

    // Hinge each child onto its parent at the shared edge.
    while (q_head < q_tail) {
        const int c = queue[q_head++];
        const int p = parents[c] - 1;

        // Find shared edge
        int ia_c = -1, ib_c = -1, ia_p = -1, ib_p = -1;
        for (int kc = 0; kc < NFV; kc++) {
            for (int kp = 0; kp < NFV; kp++) {
                if (polyhedron.faces[c][kc] == polyhedron.faces[p][kp]) {
                    if (ia_c == -1) {
                        ia_c = kc;
                        ia_p = kp;
                    } else if (ib_c == -1) {
                        ib_c = kc;
                        ib_p = kp;
                    }
                }
            }
        }

        // Place face onto net using shared edge for orientation
        const int na = net.faces[p][ia_p];
        const int nb = net.faces[p][ib_p];
        const Vec3 normal_c = vec3_normalize(
            face_centroid(polyhedron.vertices, polyhedron.faces[c]));
        const Mat4 face_to_plane = mat4_rigid_transform(
            normal_c, polyhedron.vertices[polyhedron.faces[c][ia_c]],
            net.vertices[na], polyhedron.vertices[polyhedron.faces[c][ib_c]],
            net.vertices[nb]);
        for (int k = 0; k < NFV; k++) {
            if (k == ia_c) {
                net.faces[c][k] = na;
            } else if (k == ib_c) {
                net.faces[c][k] = nb;
            } else {
                Vec3 face_vertex = polyhedron.vertices[polyhedron.faces[c][k]];
                net.vertices[n_verts] =
                    vec3_apply_mat4(face_to_plane, face_vertex);
                net.faces[c][k] = n_verts++;
            }
        }

        // Append children of this face to queue (parents[i] is 1-based).
        for (int i = 0; i < NF; i++) {
            if (parents[i] == c + 1)
                queue[q_tail++] = i;
        }
    }

    // Rescale so the net's total area equals the unit sphere's (4π).
    double total_area = 0.0;
    for (int f = 0; f < NF; f++)
        total_area += polygon_area_2d(net.vertices, net.faces[f]);
    const double scale = std::sqrt(4.0 * M_PI / total_area);

    // Translate so the root face's centroid is at origin, then scale.
    const Vec3 centroid = face_centroid(net.vertices, net.faces[root]);
    for (int i = 0; i < NV_n; i++)
        net.vertices[i] =
            vec3_scale(vec3_subtract(net.vertices[i], centroid), scale);

    return net;
}

} // namespace polyhedral

#endif // POLYHEDRAL_UNFOLD_H
