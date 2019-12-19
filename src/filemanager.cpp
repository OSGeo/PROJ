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

#ifndef FROM_PROJ_CPP
#define FROM_PROJ_CPP
#endif

#include <algorithm>

#include "filemanager.hpp"
#include "proj.h"
#include "proj/internal/internal.hpp"
#include "proj_internal.h"

#ifdef CURL_ENABLED
#include <curl/curl.h>
#include <sqlite3.h> // for sqlite3_snprintf
#endif

//! @cond Doxygen_Suppress

NS_PROJ_START

using namespace internal;

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

class NetworkFile : public File {
    PJ_CONTEXT *m_ctx;
    PROJ_NETWORK_HANDLE *m_handle;
    unsigned long long m_pos = 0;

    NetworkFile(const NetworkFile &) = delete;
    NetworkFile &operator=(const NetworkFile &) = delete;

  protected:
    NetworkFile(PJ_CONTEXT *ctx, PROJ_NETWORK_HANDLE *handle)
        : m_ctx(ctx), m_handle(handle) {}

  public:
    ~NetworkFile() override;

    size_t read(void *buffer, size_t sizeBytes) override;
    bool seek(unsigned long long offset, int whence) override;
    unsigned long long tell() override;

    static std::unique_ptr<File> open(PJ_CONTEXT *ctx, const char *filename);
};

// ---------------------------------------------------------------------------

std::unique_ptr<File> NetworkFile::open(PJ_CONTEXT *ctx, const char *filename) {
    std::vector<unsigned char> buffer(16 * 1024);
    size_t size_read = 0;
    auto handle = ctx->networking.open(ctx, filename, buffer.size(), &buffer[0],
                                       &size_read, ctx->networking.user_data);
    return std::unique_ptr<File>(handle ? new NetworkFile(ctx, handle)
                                        : nullptr);
}

// ---------------------------------------------------------------------------

size_t NetworkFile::read(void *buffer, size_t sizeBytes) {
    size_t nRead = m_ctx->networking.read_range(
        m_ctx, m_handle, m_pos, sizeBytes, buffer, m_ctx->networking.user_data);
    m_pos += nRead;
    return nRead;
}

// ---------------------------------------------------------------------------

bool NetworkFile::seek(unsigned long long offset, int whence) {
    if (whence == SEEK_SET) {
        m_pos = offset;
    } else if (whence == SEEK_CUR) {
        m_pos += offset;
    } else {
        if (offset != 0)
            return false;
        const auto filesize = m_ctx->networking.get_file_size(
            m_ctx, m_handle, m_ctx->networking.user_data);
        if (filesize == 0)
            return false;
        m_pos = filesize;
    }
    return true;
}

// ---------------------------------------------------------------------------

unsigned long long NetworkFile::tell() { return m_pos; }

// ---------------------------------------------------------------------------

NetworkFile::~NetworkFile() {
    m_ctx->networking.close(m_ctx, m_handle, m_ctx->networking.user_data);
}

// ---------------------------------------------------------------------------

std::unique_ptr<File> FileManager::open(PJ_CONTEXT *ctx, const char *filename) {
#ifndef REMOVE_LEGACY_SUPPORT
    // If the user has specified a legacy fileapi, use it
    if (ctx->fileapi_legacy != pj_get_default_fileapi()) {
        return FileLegacyAdapter::open(ctx, filename);
    }
#endif
    if (starts_with(filename, "http://") || starts_with(filename, "https://")) {
        return NetworkFile::open(ctx, filename);
    }
    return FileStdio::open(ctx, filename);
}

// ---------------------------------------------------------------------------

#ifdef CURL_ENABLED

struct CurlFileHandle {
    CURL *m_handle = nullptr;
    std::string m_headers;

    CurlFileHandle(const CurlFileHandle &) = delete;
    CurlFileHandle &operator=(const CurlFileHandle &) = delete;

    explicit CurlFileHandle(CURL *handle, std::string &&headers)
        : m_handle(handle), m_headers(std::move(headers)) {}
    ~CurlFileHandle();
};

// ---------------------------------------------------------------------------

CurlFileHandle::~CurlFileHandle() { curl_easy_cleanup(m_handle); }

// ---------------------------------------------------------------------------

static size_t pj_curl_write_func(void *buffer, size_t count, size_t nmemb,
                                 void *req) {
    const size_t nSize = count * nmemb;
    auto pStr = static_cast<std::string *>(req);
    if (pStr->size() + nSize > pStr->capacity()) {
        // to avoid servers not honouring Range to cause excessive memory
        // allocation
        return 0;
    }
    pStr->append(static_cast<const char *>(buffer), nSize);
    return nmemb;
}

// ---------------------------------------------------------------------------

static PROJ_NETWORK_HANDLE *pj_curl_open(PJ_CONTEXT *, const char *url,
                                         size_t size_to_read, void *buffer,
                                         size_t *out_size_read, void *) {
    CURL *hCurlHandle = curl_easy_init();
    if (!hCurlHandle)
        return nullptr;
    curl_easy_setopt(hCurlHandle, CURLOPT_URL, url);

    if (getenv("PROJ_CURL_VERBOSE"))
        curl_easy_setopt(hCurlHandle, CURLOPT_VERBOSE, 1);

// CURLOPT_SUPPRESS_CONNECT_HEADERS is defined in curl 7.54.0 or newer.
#if LIBCURL_VERSION_NUM >= 0x073600
    curl_easy_setopt(hCurlHandle, CURLOPT_SUPPRESS_CONNECT_HEADERS, 1L);
#endif

    // Enable following redirections.  Requires libcurl 7.10.1 at least.
    curl_easy_setopt(hCurlHandle, CURLOPT_FOLLOWLOCATION, 1);
    curl_easy_setopt(hCurlHandle, CURLOPT_MAXREDIRS, 10);

    if (getenv("PROJ_UNSAFE_SSL")) {
        curl_easy_setopt(hCurlHandle, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(hCurlHandle, CURLOPT_SSL_VERIFYHOST, 0L);
    }

    char szBuffer[128];
    sqlite3_snprintf(sizeof(szBuffer), szBuffer, "0-%llu", size_to_read - 1);
    curl_easy_setopt(hCurlHandle, CURLOPT_RANGE, szBuffer);

    std::string headers;
    headers.reserve(16 * 1024);
    curl_easy_setopt(hCurlHandle, CURLOPT_HEADERDATA, &headers);
    curl_easy_setopt(hCurlHandle, CURLOPT_HEADERFUNCTION, pj_curl_write_func);

    std::string body;
    body.reserve(size_to_read);
    curl_easy_setopt(hCurlHandle, CURLOPT_WRITEDATA, &body);
    curl_easy_setopt(hCurlHandle, CURLOPT_WRITEFUNCTION, pj_curl_write_func);

    curl_easy_perform(hCurlHandle);

    long response_code = 0;
    curl_easy_getinfo(hCurlHandle, CURLINFO_HTTP_CODE, &response_code);

    curl_easy_setopt(hCurlHandle, CURLOPT_HEADERDATA, nullptr);
    curl_easy_setopt(hCurlHandle, CURLOPT_HEADERFUNCTION, nullptr);

    curl_easy_setopt(hCurlHandle, CURLOPT_WRITEDATA, nullptr);
    curl_easy_setopt(hCurlHandle, CURLOPT_WRITEFUNCTION, nullptr);

    if (response_code == 0 || response_code >= 300) {
        curl_easy_cleanup(hCurlHandle);
        return nullptr;
    }

    if (!body.empty()) {
        memcpy(buffer, body.data(), std::min(size_to_read, body.size()));
    }
    *out_size_read = std::min(size_to_read, body.size());

    return reinterpret_cast<PROJ_NETWORK_HANDLE *>(
        new CurlFileHandle(hCurlHandle, std::move(headers)));
}

// ---------------------------------------------------------------------------

static void pj_curl_close(PJ_CONTEXT *, PROJ_NETWORK_HANDLE *handle,
                          void * /*user_data*/) {
    delete reinterpret_cast<CurlFileHandle *>(handle);
}

// ---------------------------------------------------------------------------

static size_t pj_curl_read_range(PJ_CONTEXT *, PROJ_NETWORK_HANDLE *raw_handle,
                                 unsigned long long offset, size_t size_to_read,
                                 void *buffer, void *) {
    auto handle = reinterpret_cast<CurlFileHandle *>(raw_handle);
    auto hCurlHandle = handle->m_handle;

    char szBuffer[128];
    sqlite3_snprintf(sizeof(szBuffer), szBuffer, "%llu-%llu", offset,
                     offset + size_to_read - 1);
    curl_easy_setopt(hCurlHandle, CURLOPT_RANGE, szBuffer);

    std::string body;
    body.reserve(size_to_read);
    curl_easy_setopt(hCurlHandle, CURLOPT_WRITEDATA, &body);
    curl_easy_setopt(hCurlHandle, CURLOPT_WRITEFUNCTION, pj_curl_write_func);

    curl_easy_perform(hCurlHandle);

    long response_code = 0;
    curl_easy_getinfo(hCurlHandle, CURLINFO_HTTP_CODE, &response_code);

    curl_easy_setopt(hCurlHandle, CURLOPT_WRITEDATA, nullptr);
    curl_easy_setopt(hCurlHandle, CURLOPT_WRITEFUNCTION, nullptr);

    if (response_code == 0 || response_code >= 300) {
        return 0;
    }

    if (!body.empty()) {
        memcpy(buffer, body.data(), std::min(size_to_read, body.size()));
    }
    return std::min(size_to_read, body.size());
}

#else

// ---------------------------------------------------------------------------

static PROJ_NETWORK_HANDLE *
no_op_network_open(PJ_CONTEXT *ctx, const char * /* url */,
                   size_t,   /* size to read */
                   void *,   /* buffer to update with bytes read*/
                   size_t *, /* output: size actually read */
                   void * /*user_data*/) {
    pj_log(ctx, PJ_LOG_DEBUG_MAJOR, "Network functionality not available");
    return nullptr;
}

// ---------------------------------------------------------------------------

static void no_op_network_close(PJ_CONTEXT *, PROJ_NETWORK_HANDLE *,
                                void * /*user_data*/) {}

// ---------------------------------------------------------------------------

static const char *no_op_network_get_last_error(PJ_CONTEXT *,
                                                PROJ_NETWORK_HANDLE *,
                                                void * /*user_data*/) {
    return "Network functionality not available";
}

#endif

// ---------------------------------------------------------------------------

void FileManager::fillDefaultNetworkInterface(PJ_CONTEXT *ctx) {
#ifdef CURL_ENABLED
    ctx->networking.open = pj_curl_open;
    ctx->networking.close = pj_curl_close;
    ctx->networking.read_range = pj_curl_read_range;
#else
    ctx->networking.open = no_op_network_open;
    ctx->networking.close = no_op_network_close;
    ctx->networking.get_last_error = no_op_network_get_last_error;
#endif
}

// ---------------------------------------------------------------------------

NS_PROJ_END

//! @endcond

// ---------------------------------------------------------------------------

/** Define a custom set of callbacks for network access.
 *
 * All callbacks should be provided (non NULL pointers).
 *
 * @param ctx PROJ context, or NULL
 * @param open_cbk Callback to open a remote file given its URL
 * @param close_cbk Callback to close a remote file.
 * @param get_header_value_cbk Callback to get HTTP headers
 * @param get_file_size_cbk Callback to get the size of the remote file.
 * @param read_range_cbk Callback to read a range of bytes inside a remote file.
 * @param get_last_error_cbk Callback to get last error message.
 * @param user_data Arbitrary pointer provided by the user, and passed to the
 * above callbacks. May be NULL.
 * @return TRUE in case of success.
 */
int proj_context_set_network_callbacks(
    PJ_CONTEXT *ctx, proj_network_open_cbk_type open_cbk,
    proj_network_close_cbk_type close_cbk,
    proj_network_get_header_value_cbk_type get_header_value_cbk,
    proj_network_get_file_size_cbk_type get_file_size_cbk,
    proj_network_read_range_type read_range_cbk,
    proj_network_get_last_error_type get_last_error_cbk, void *user_data) {
    if (ctx == nullptr) {
        ctx = pj_get_default_ctx();
    }
    if (!open_cbk || !close_cbk || !get_header_value_cbk ||
        !get_file_size_cbk || !read_range_cbk || !get_last_error_cbk) {
        return false;
    }
    ctx->networking.open = open_cbk;
    ctx->networking.close = close_cbk;
    ctx->networking.get_header_value = get_header_value_cbk;
    ctx->networking.get_file_size = get_file_size_cbk;
    ctx->networking.read_range = read_range_cbk;
    ctx->networking.get_last_error = get_last_error_cbk;
    ctx->networking.user_data = user_data;
    return true;
}
