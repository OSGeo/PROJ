// Small utility used by test/render_layout.py to dump the unfolded face
// polygons of a polyhedral projection in plain text. Lives under test/
// so it can include the internal polyhedral headers without exposing any
// new symbols from libproj.
//
// Usage:  print_polyhedral_layout "+proj=tsea +R=1"
// Output: first line "<num_faces> <face_vertex_count>", followed by one
// line per face with `nfv` space-separated "x y" pairs (net coordinates,
// no lat_0/lon_0/x_0/y_0 offset applied).

#include "polyhedral/sphere.h"
#include "proj.h"
#include "proj_internal.h"

#include <cstdio>
#include <cstdlib>

int main(int argc, char **argv) {
    if (argc != 2) {
        std::fprintf(stderr, "usage: %s \"+proj=... +R=1\"\n", argv[0]);
        return 1;
    }
    PJ_CONTEXT *ctx = proj_context_create();
    PJ *pj = proj_create(ctx, argv[1]);
    if (!pj) {
        std::fprintf(stderr, "failed to create projection: %s\n", argv[1]);
        return 1;
    }
    auto *Q = static_cast<polyhedral::pj_polyhedral_data *>(pj->opaque);
    if (!Q || Q->face_vertex_count <= 0) {
        std::fprintf(stderr, "not a polyhedral projection: %s\n", argv[1]);
        return 1;
    }
    const int nf = Q->num_faces;
    const int nfv = Q->face_vertex_count;
    const int fan_size = 2 * nfv;
    std::printf("%d %d\n", nf, nfv);
    for (int f = 0; f < nf; f++) {
        for (int k = 0; k < nfv; k++) {
            const auto &v = Q->face_tris[fan_size * f + 2 * k].c;
            std::printf("%.17g %.17g%s", v.x, v.y, k + 1 == nfv ? "\n" : " ");
        }
    }
    proj_destroy(pj);
    proj_context_destroy(ctx);
    return 0;
}
