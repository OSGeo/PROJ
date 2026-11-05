/* proj_grid_fuzzer.cpp — fuzz the grid stack: grid file parsing + grid-driven
 * coordinate transforms.
 *
 * Every grid format PROJ reads (NTv2 .gsb, GTX, GeoTIFF grids via libtiff,
 * ctable2, deformation-model JSON, tinshift GeoPackage) is currently reached
 * by NO fuzz target. This harness serves the fuzzed bytes as an in-memory
 * file through the PROJ_FILE_API custom file manager (requests whose name
 * contains "fuzz" get the blob; everything else, e.g. proj.db, falls back to
 * the real filesystem), then creates a grid-driven operation from the fuzzed
 * definition string and transforms fuzzed points.
 *
 * Input format:
 *   byte 0: flags (bit0 = PJ_INV direction)
 *   then:  operation definition line, <= 200 bytes (e.g.
 *          "+proj=hgridshift +grids=fuzzgrid.gsb +ellps=GRS80")
 *   then:  48 bytes: two (x, y, z) little-endian double triples
 *   rest:  the grid/model file blob (served for "fuzz*" file requests)
 *
 * Copyright (c) 2026, OSS-Fuzz coverage pipeline. MIT/Expat like the rest of
 * PROJ's fuzzers (see proj_crs_to_crs_fuzzer.cpp).
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "proj.h"

#define MAX_DEF 200
#define MIN_TAIL 49 /* 48 bytes coords + at least 1 blob byte */

typedef struct {
    size_t pos;
    FILE *fp;
    int is_blob;
} FuzzHandle;

typedef struct {
    const uint8_t *blob;
    size_t len;
} BlobData;

static int is_fuzz_file(const char *filename) {
    return filename && strstr(filename, "fuzz") != NULL;
}

static PROJ_FILE_HANDLE *fuzz_open(PJ_CONTEXT *ctx, const char *filename,
                                   PROJ_OPEN_ACCESS access, void *user_data) {
    (void)ctx;
    BlobData *bd = (BlobData *)user_data;
    FuzzHandle *h = (FuzzHandle *)malloc(sizeof(FuzzHandle));
    if (!h) return NULL;
    h->pos = 0;
    h->fp = NULL;
    if (access == PROJ_OPEN_ACCESS_READ_ONLY && is_fuzz_file(filename)) {
        if (bd->len == 0) {
            free(h);
            return NULL;
        }
        h->is_blob = 1;
        return (PROJ_FILE_HANDLE *)h;
    }
    h->is_blob = 0;
    h->fp = fopen(filename, access == PROJ_OPEN_ACCESS_READ_UPDATE
                             ? "r+b"
                             : (access == PROJ_OPEN_ACCESS_CREATE ? "w+b" : "rb"));
    if (!h->fp) {
        free(h);
        return NULL;
    }
    return (PROJ_FILE_HANDLE *)h;
}

static size_t fuzz_read(PJ_CONTEXT *ctx, PROJ_FILE_HANDLE *handle,
                        void *buffer, size_t sizeBytes, void *user_data) {
    (void)ctx;
    BlobData *bd = (BlobData *)user_data;
    FuzzHandle *h = (FuzzHandle *)handle;
    if (!h) return 0;
    if (h->is_blob) {
        size_t avail = h->pos < bd->len ? bd->len - h->pos : 0;
        size_t n = sizeBytes < avail ? sizeBytes : avail;
        memcpy(buffer, bd->blob + h->pos, n);
        h->pos += n;
        return n;
    }
    return fread(buffer, 1, sizeBytes, h->fp);
}

static int fuzz_seek(PJ_CONTEXT *ctx, PROJ_FILE_HANDLE *handle,
                     long long offset, int whence, void *user_data) {
    (void)ctx;
    BlobData *bd = (BlobData *)user_data;
    FuzzHandle *h = (FuzzHandle *)handle;
    if (!h) return 0;
    if (h->is_blob) {
        long long target;
        if (whence == SEEK_SET) target = offset;
        else if (whence == SEEK_CUR) target = (long long)h->pos + offset;
        else if (whence == SEEK_END) target = (long long)bd->len + offset;
        else return 0;
        if (target < 0) target = 0;
        if (target > (long long)bd->len) target = (long long)bd->len;
        h->pos = (size_t)target;
        return 1;
    }
    return fseek(h->fp, (long)offset, whence) == 0;
}

static unsigned long long fuzz_tell(PJ_CONTEXT *ctx, PROJ_FILE_HANDLE *handle,
                                    void *user_data) {
    (void)ctx;
    (void)user_data;
    FuzzHandle *h = (FuzzHandle *)handle;
    if (!h) return 0;
    if (h->is_blob) return (unsigned long long)h->pos;
    return (unsigned long long)ftell(h->fp);
}

static void fuzz_close(PJ_CONTEXT *ctx, PROJ_FILE_HANDLE *handle,
                       void *user_data) {
    (void)ctx;
    (void)user_data;
    FuzzHandle *h = (FuzzHandle *)handle;
    if (!h) return;
    if (h->fp) fclose(h->fp);
    free(h);
}

static int fuzz_exists(PJ_CONTEXT *ctx, const char *filename,
                       void *user_data) {
    (void)ctx;
    (void)user_data;
    if (is_fuzz_file(filename)) return 1;
    FILE *f = fopen(filename, "rb");
    if (!f) return 0;
    fclose(f);
    return 1;
}

static int fuzz_mkdir(PJ_CONTEXT *ctx, const char *filename, void *user_data) {
    (void)ctx;
    (void)user_data;
    (void)filename;
    return 0; /* no directories are needed from the fuzzed side */
}

static int fuzz_unlink(PJ_CONTEXT *ctx, const char *filename, void *user_data) {
    (void)ctx;
    (void)user_data;
    if (is_fuzz_file(filename)) return 1;
    return remove(filename) == 0;
}

static int fuzz_rename(PJ_CONTEXT *ctx, const char *oldPath, const char *newPath,
                       void *user_data) {
    (void)ctx;
    (void)user_data;
    if (is_fuzz_file(oldPath) || is_fuzz_file(newPath)) return 0;
    return rename(oldPath, newPath) == 0;
}

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
    if (len < 2 + MIN_TAIL) return 0;
    const uint8_t flags = buf[0];
    const uint8_t *p = buf + 1;
    const uint8_t *end = buf + len;

    const uint8_t *nl = (const uint8_t *)memchr(p, '\n', (size_t)(end - p));
    if (!nl) return 0;
    size_t def_len = (size_t)(nl - p);
    if (def_len == 0 || def_len > MAX_DEF) return 0;
    if ((size_t)(end - nl) < MIN_TAIL + 1) return 0;

    char def[MAX_DEF + 1];
    memcpy(def, p, def_len);
    def[def_len] = '\0';

    PJ_DIRECTION dir = (flags & 0x01) ? PJ_INV : PJ_FWD;

    double coords[6];
    memcpy(coords, nl + 1, sizeof(coords));
    const uint8_t *blob = nl + 1 + sizeof(coords);
    size_t blob_len = (size_t)(end - blob);

    BlobData bd = {blob, blob_len};
    PROJ_FILE_API api;
    memset(&api, 0, sizeof(api));
    api.version = 1;
    api.open_cbk = fuzz_open;
    api.read_cbk = fuzz_read;
    api.write_cbk = NULL;
    api.seek_cbk = fuzz_seek;
    api.tell_cbk = fuzz_tell;
    api.close_cbk = fuzz_close;
    api.exists_cbk = fuzz_exists;
    api.mkdir_cbk = fuzz_mkdir;
    api.unlink_cbk = fuzz_unlink;
    api.rename_cbk = fuzz_rename;

    PJ_CONTEXT *ctx = proj_context_create();
    if (!ctx) return 0;
    proj_context_set_fileapi(ctx, &api, &bd);

    PJ *P = proj_create(ctx, def);
    if (!P) {
        proj_context_destroy(ctx);
        proj_cleanup();
        return 0;
    }

    PJ_COORD a = proj_coord(coords[0], coords[1], coords[2], 0.0);
    PJ_COORD b = proj_coord(coords[3], coords[4], coords[5], 0.0);
    a = proj_trans(P, dir, a);
    b = proj_trans(P, dir, b);
    PJ_COORD arr[2] = {a, b};
    (void)proj_trans_array(P, dir, 2, arr);

    proj_destroy(P);
    proj_context_destroy(ctx);
    proj_cleanup();
    return 0;
}


