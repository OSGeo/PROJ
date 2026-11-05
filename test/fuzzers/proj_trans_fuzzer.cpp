/******************************************************************************
 *
 * Project:  PROJ
 * Purpose:  Fuzzer: exercise the coordinate *transformation* path
 *           (proj_create_crs_to_crs -> proj_trans / proj_trans_array /
 *            proj_trans_bounds / proj_roundtrip) with fuzzed CRS definitions
 *           and fuzzed coordinates. Complements proj_crs_to_crs_fuzzer, which
 *           only ever creates the operation and never transforms points.
 *
 * Input format:
 *   byte 0: flags
 *     bit0: apply proj_normalize_for_visualization() on the operation
 *     bit1: transform with PJ_INV instead of PJ_FWD
 *     bit2: use proj_trans_array() (needs >= 2 coords) instead of a proj_trans loop
 *     bit3: also run proj_trans_bounds() and proj_trans_bounds_3D()
 *     bit4: also run proj_roundtrip()
 *     bits5-7: densify_pts for the bounds calls (0..7)
 *   then:  source CRS definition (one line, <= 400 bytes)
 *   then:  target CRS definition (one line, <= 400 bytes)
 *   then:  up to 8 coordinates as little-endian doubles (x, y, z each)
 *
 * The threat model: CRS strings and coordinate values are attacker-influenced
 * in virtually every GIS consumer (files, URLs, OGC services). This harness
 * drives the transformation math (single operations fwd+inverse, concatenated
 * operations, conversions, bounds densification) on such untrusted input.
 *
 * See the OSS-Fuzz proj4 project for build instructions.
 ******************************************************************************
 * Copyright (c) 2026 and later
 * Author: OSS-Fuzz coverage pipeline
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 ****************************************************************************/

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "proj.h"

#define MAX_DEF 400
#define MAX_COORDS 8
#define MAX_INPUT 4096

extern "C" int LLVMFuzzerInitialize(int *argc, char ***argv) {
    const char *argv0 = (*argv)[0];
    char *path = strdup(argv0);
    char *lastslash = strrchr(path, '/');
    if (lastslash) {
        *lastslash = '\0';
        setenv("PROJ_DATA", path, 1);
    } else {
        setenv("PROJ_DATA", ".", 1);
    }
    free(path);
    return 0;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *buf, size_t len) {
    if (len < 2 || len > MAX_INPUT) return 0;
    const uint8_t flags = buf[0];
    const uint8_t *p = buf + 1;
    const uint8_t *end = buf + len;

    const uint8_t *nl1 = (const uint8_t *)memchr(p, '\n', (size_t)(end - p));
    if (!nl1) return 0;
    size_t src_len = (size_t)(nl1 - p);
    if (src_len == 0 || src_len > MAX_DEF) return 0;

    const uint8_t *rest = nl1 + 1;
    const uint8_t *nl2 = (const uint8_t *)memchr(rest, '\n', (size_t)(end - rest));
    size_t dst_len = nl2 ? (size_t)(nl2 - rest) : (size_t)(end - rest);
    if (dst_len == 0 || dst_len > MAX_DEF) return 0;
    const uint8_t *cb = nl2 ? nl2 + 1 : end;
    size_t clen = (size_t)(end - cb);

    PJ_DIRECTION dir = (flags & 0x02) ? PJ_INV : PJ_FWD;

    PJ_CONTEXT *ctx = proj_context_create();
    if (!ctx) return 0;

    char src[MAX_DEF + 1];
    memcpy(src, p, src_len);
    src[src_len] = '\0';
    char dst[MAX_DEF + 1];
    memcpy(dst, rest, dst_len);
    dst[dst_len] = '\0';

    PJ *P = proj_create_crs_to_crs(ctx, src, dst, nullptr);
    if (!P) {
        /* Error-string probing only; the interesting paths need a valid op. */
        (void)proj_context_errno_string(ctx, proj_context_errno(ctx));
        proj_context_destroy(ctx);
        proj_cleanup();
        return 0;
    }

    if (flags & 0x01) {
        PJ *Pn = proj_normalize_for_visualization(ctx, P);
        if (Pn) {
            proj_destroy(P);
            P = Pn;
        }
    }

    (void)proj_angular_input(P, dir);
    (void)proj_angular_output(P, dir);

    PJ_COORD c[MAX_COORDS];
    size_t n = 0;
    while (n < MAX_COORDS && clen >= 3 * sizeof(double)) {
        double v[3];
        memcpy(v, cb, 3 * sizeof(double));
        c[n] = proj_coord(v[0], v[1], v[2], 0.0);
        n++;
        cb += 3 * sizeof(double);
        clen -= 3 * sizeof(double);
    }

    if ((flags & 0x04) && n >= 2) {
        (void)proj_trans_array(P, dir, n, c);
    } else {
        for (size_t i = 0; i < n; i++) c[i] = proj_trans(P, dir, c[i]);
    }

    if ((flags & 0x08) && n >= 2) {
        double xmin = c[0].xyzt.x, xmax = c[0].xyzt.x;
        double ymin = c[0].xyzt.y, ymax = c[0].xyzt.y;
        for (size_t i = 1; i < n; i++) {
            if (c[i].xyzt.x < xmin) xmin = c[i].xyzt.x;
            if (c[i].xyzt.x > xmax) xmax = c[i].xyzt.x;
            if (c[i].xyzt.y < ymin) ymin = c[i].xyzt.y;
            if (c[i].xyzt.y > ymax) ymax = c[i].xyzt.y;
        }
        if (xmin > xmax) { double t = xmin; xmin = xmax; xmax = t; }
        if (ymin > ymax) { double t = ymin; ymin = ymax; ymax = t; }
        int densify = (int)((flags >> 5) & 0x07); /* 0..7, within documented range */
        double o0, o1, o2, o3, o4, o5;
        (void)proj_trans_bounds(ctx, P, dir, xmin, ymin, xmax, ymax, &o0, &o1,
                                &o2, &o3, densify);
        (void)proj_trans_bounds_3D(ctx, P, dir, xmin, ymin, -100.0, xmax, ymax,
                                   100.0, &o0, &o1, &o2, &o3, &o4, &o5, densify);
    }

    if ((flags & 0x10) && n >= 1) {
        (void)proj_roundtrip(P, dir, 4, &c[0]);
    }

    PJ *last = proj_trans_get_last_used_operation(P);
    if (last) proj_destroy(last);

    proj_destroy(P);
    proj_context_destroy(ctx);
    proj_cleanup();
    return 0;
}
