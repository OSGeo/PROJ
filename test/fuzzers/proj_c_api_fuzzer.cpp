/* proj_c_api_fuzzer.cpp — stateful sequence harness for the PROJ C API.
 *
 * c_api.cpp is the single largest dark file in the coverage report (~5,000
 * uncovered lines): the existing production fuzzer only ever calls
 * proj_create_crs_to_crs. This harness drives a small object session from a
 * fuzzed op stream: create from string/database, clone, normalize,
 * equivalence, identify, accessors, area-of-use, WKT/PROJJSON marshalling
 * (including a marshal->re-parse round trip), source-CRS extraction and
 * destroy.
 *
 * Input format:
 *   [definition lines, one per line, up to 8 lines of <= 400 bytes]
 *   [\n]
 *   [op stream: 1..64 bytes; each byte: op = b % 14, line idx = (b / 14) % nlines]
 *
 * Copyright (c) 2026, OSS-Fuzz coverage pipeline. MIT/Expat like the rest of
 * PROJ's fuzzers (see proj_crs_to_crs_fuzzer.cpp).
 */
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "proj.h"

#define MAX_LINES 8
#define MAX_DEF 400
#define MAX_OBJS 16
#define MAX_OPS 64
#define NUM_OPS 14

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

/* Returns the number of parsed definitions (up to MAX_LINES), or 0 on error. */
static int parse_lines(const uint8_t *p, size_t len, char defs[MAX_LINES][MAX_DEF + 1]) {
    size_t start = 0;
    int n = 0;
    for (size_t i = 0; i <= len; i++) {
        if (i == len || p[i] == '\n') {
            size_t dlen = i - start;
            if (dlen > 0 && dlen <= (size_t)MAX_DEF && n < MAX_LINES) {
                memcpy(defs[n], p + start, dlen);
                defs[n][dlen] = '\0';
                n++;
            }
            if (n == MAX_LINES) break;
            start = i + 1;
        }
    }
    return n;
}

static void push_obj(PJ *objs[MAX_OBJS], size_t *nobjs, PJ *obj) {
    if (!obj) return;
    if (*nobjs == MAX_OBJS) {
        proj_destroy(obj);
        return;
    }
    objs[(*nobjs)++] = obj;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *buf, size_t len) {
    if (len < 4 || len > 4096) return 0;

    /* split: [lines section] '\n' [ops section] */
    const uint8_t *sep = NULL;
    for (const uint8_t *q = buf + len - 1; q >= buf; q--) {
        if (*q == '\n') {
            sep = q;
            break;
        }
    }
    if (!sep || sep == buf) return 0;
    size_t ops_len = (size_t)(buf + len - sep - 1);
    if (ops_len == 0) return 0;

    char defs[MAX_LINES][MAX_DEF + 1];
    int nlines = parse_lines(buf, (size_t)(sep - buf), defs);
    if (nlines == 0) return 0;

    PJ_CONTEXT *ctx = proj_context_create();
    if (!ctx) return 0;
    PJ *objs[MAX_OBJS];
    size_t nobjs = 0;

    const uint8_t *ops = sep + 1;
    size_t nops = ops_len < (size_t)MAX_OPS ? ops_len : (size_t)MAX_OPS;

    for (size_t i = 0; i < nops; i++) {
        uint8_t b = ops[i];
        uint8_t op = (uint8_t)(b % NUM_OPS);
        uint8_t k = (uint8_t)((b / NUM_OPS) % (unsigned)nlines);
        if (nobjs == 0 && op != 0 && op != 1 && op != 13) continue;
        PJ *top = nobjs ? objs[nobjs - 1] : NULL;

        switch (op) {
        case 0: /* create from string */
            push_obj(objs, &nobjs, proj_create(ctx, defs[k]));
            break;
        case 1: /* create from database (defs used as authority code) */
            push_obj(objs, &nobjs, proj_create_from_database(
                                       ctx, "EPSG", defs[k], PJ_CATEGORY_CRS, 0,
                                       NULL));
            break;
        case 2: /* clone */
            push_obj(objs, &nobjs, proj_clone(ctx, top));
            break;
        case 3: { /* normalize */
            PJ *norm = proj_normalize_for_visualization(ctx, top);
            if (norm) {
                proj_destroy(top);
                objs[nobjs - 1] = norm;
            }
            break;
        }
        case 4: /* equivalence between the two top objects */
            if (nobjs >= 2) {
                (void)proj_is_equivalent_to(objs[nobjs - 2], objs[nobjs - 1],
                                            (PJ_COMPARISON_CRITERION)(b % 3));
            }
            break;
        case 5: { /* identify */
            int *conf = NULL;
            PJ_OBJ_LIST *l = proj_identify(ctx, top, "EPSG", NULL, &conf);
            proj_list_destroy(l);
            proj_int_list_destroy(conf);
            break;
        }
        case 6: /* marshal WKT (owned by obj; no free) */
            (void)proj_as_wkt(ctx, top, (b & 0x10) ? PJ_WKT2_2019 : PJ_WKT1_GDAL,
                              NULL);
            break;
        case 7: /* marshal PROJJSON */
            (void)proj_as_projjson(ctx, top, NULL);
            break;
        case 8: { /* marshal -> re-parse round trip */
            const char *s = proj_as_wkt(ctx, top, PJ_WKT2_2019, NULL);
            if (s) push_obj(objs, &nobjs, proj_create(ctx, s));
            break;
        }
        case 9: /* accessors */
            (void)proj_get_name(top);
            (void)proj_get_type(top);
            (void)proj_get_scope(top);
            (void)proj_get_remarks(top);
            (void)proj_get_id_auth_name(top, (int)(b & 1));
            (void)proj_get_id_code(top, (int)(b & 1));
            break;
        case 10: { /* non-deprecated alternatives */
            PJ_OBJ_LIST *l = proj_get_non_deprecated(ctx, top);
            proj_list_destroy(l);
            break;
        }
        case 11: /* destroy top */
            proj_destroy(top);
            nobjs--;
            break;
        case 12: /* source CRS extraction */
            push_obj(objs, &nobjs, proj_get_source_crs(ctx, top));
            break;
        case 13: { /* create from string OR area-of-use when objects exist */
            if (top) {
                double w, s, e, n;
                const char *name = NULL;
                (void)proj_get_area_of_use(ctx, top, &w, &s, &e, &n, &name);
            } else {
                push_obj(objs, &nobjs, proj_create(ctx, defs[k]));
            }
            break;
        }
        }
    }

    for (size_t i = 0; i < nobjs; i++) proj_destroy(objs[i]);
    proj_context_destroy(ctx);
    proj_cleanup();
    return 0;
}

