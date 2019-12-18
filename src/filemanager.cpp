/******************************************************************************
 * Project:  PROJ
 * Purpose:  File manager
 * Author:   Even Rouault, <even.rouault at spatialys.com>
 *
 ******************************************************************************
 * Copyright (c) 2019, Even Rouault, <even.rouault at spatialys.com>
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
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *****************************************************************************/

#include "filemanager.hpp"
#include "proj_internal.h"

NS_PROJ_START

// ---------------------------------------------------------------------------

File::File() = default;

// ---------------------------------------------------------------------------

File::~File() = default;

// ---------------------------------------------------------------------------

class FileStdio : public File {
    PJ_CONTEXT *m_ctx;
    FILE *m_fp;

    FileStdio(const FileStdio &) = delete;
    FileStdio &operator=(const FileStdio &) = delete;

  protected:
    FileStdio(PJ_CONTEXT *ctx, FILE *fp) : m_ctx(ctx), m_fp(fp) {}

  public:
    ~FileStdio() override;

    size_t read(void *buffer, size_t sizeBytes) override;
    bool seek(unsigned long long offset, int whence = SEEK_SET) override;
    unsigned long long tell() override;

    static std::unique_ptr<File> open(PJ_CONTEXT *ctx, const char *filename);
};

// ---------------------------------------------------------------------------

FileStdio::~FileStdio() { fclose(m_fp); }

// ---------------------------------------------------------------------------

size_t FileStdio::read(void *buffer, size_t sizeBytes) {
    return fread(buffer, 1, sizeBytes, m_fp);
}

// ---------------------------------------------------------------------------

bool FileStdio::seek(unsigned long long offset, int whence) {
    // TODO one day: use 64-bit offset compatible API
    if (offset != static_cast<unsigned long long>(static_cast<long>(offset))) {
        pj_log(m_ctx, PJ_LOG_ERROR,
               "Attempt at seeking to a 64 bit offset. Not supported yet");
        return false;
    }
    return fseek(m_fp, static_cast<long>(offset), whence) == 0;
}

// ---------------------------------------------------------------------------

unsigned long long FileStdio::tell() {
    // TODO one day: use 64-bit offset compatible API
    return ftell(m_fp);
}

// ---------------------------------------------------------------------------

std::unique_ptr<File> FileStdio::open(PJ_CONTEXT *ctx, const char *filename) {
    auto fp = fopen(filename, "rb");
    return std::unique_ptr<File>(fp ? new FileStdio(ctx, fp) : nullptr);
}

// ---------------------------------------------------------------------------

#ifndef REMOVE_LEGACY_SUPPORT

class FileLegacyAdapter : public File {
    PJ_CONTEXT *m_ctx;
    PAFile m_fp;

    FileLegacyAdapter(const FileLegacyAdapter &) = delete;
    FileLegacyAdapter &operator=(const FileLegacyAdapter &) = delete;

  protected:
    FileLegacyAdapter(PJ_CONTEXT *ctx, PAFile fp) : m_ctx(ctx), m_fp(fp) {}

  public:
    ~FileLegacyAdapter() override;

    size_t read(void *buffer, size_t sizeBytes) override;
    bool seek(unsigned long long offset, int whence = SEEK_SET) override;
    unsigned long long tell() override;

    static std::unique_ptr<File> open(PJ_CONTEXT *ctx, const char *filename);
};

// ---------------------------------------------------------------------------

FileLegacyAdapter::~FileLegacyAdapter() { pj_ctx_fclose(m_ctx, m_fp); }

// ---------------------------------------------------------------------------

size_t FileLegacyAdapter::read(void *buffer, size_t sizeBytes) {
    return pj_ctx_fread(m_ctx, buffer, 1, sizeBytes, m_fp);
}

// ---------------------------------------------------------------------------

bool FileLegacyAdapter::seek(unsigned long long offset, int whence) {
    if (offset != static_cast<unsigned long long>(static_cast<long>(offset))) {
        pj_log(m_ctx, PJ_LOG_ERROR,
               "Attempt at seeking to a 64 bit offset. Not supported yet");
        return false;
    }
    return pj_ctx_fseek(m_ctx, m_fp, static_cast<long>(offset), whence) == 0;
}

// ---------------------------------------------------------------------------

unsigned long long FileLegacyAdapter::tell() {
    return pj_ctx_ftell(m_ctx, m_fp);
}

// ---------------------------------------------------------------------------

std::unique_ptr<File> FileLegacyAdapter::open(PJ_CONTEXT *ctx,
                                              const char *filename) {
    auto fid = pj_ctx_fopen(ctx, filename, "rb");
    return std::unique_ptr<File>(fid ? new FileLegacyAdapter(ctx, fid)
                                     : nullptr);
}

#endif // REMOVE_LEGACY_SUPPORT

// ---------------------------------------------------------------------------

std::unique_ptr<File> FileManager::open(PJ_CONTEXT *ctx, const char *filename) {
#ifndef REMOVE_LEGACY_SUPPORT
    // If the user has specified a legacy fileapi, use it
    if (ctx->fileapi_legacy != pj_get_default_fileapi()) {
        return FileLegacyAdapter::open(ctx, filename);
    }
#endif
    return FileStdio::open(ctx, filename);
}

NS_PROJ_END
