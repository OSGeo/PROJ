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
#define LRU11_DO_NOT_DEFINE_OUT_OF_CLASS_METHODS

#include <algorithm>
#include <functional>
#include <limits>

#include "filemanager.hpp"
#include "proj.h"
#include "proj/internal/internal.hpp"
#include "proj/internal/lru_cache.hpp"
#include "proj_internal.h"

#ifdef __MINGW32__
// mingw32-win32 doesn't implement std::mutex
namespace {
class MyMutex {
  public:
    // cppcheck-suppress functionStatic
    void lock() { pj_acquire_lock(); }
    // cppcheck-suppress functionStatic
    void unlock() { pj_release_lock(); }
};
}
#else
#include <mutex>
#define MyMutex std::mutex
#endif

#ifdef CURL_ENABLED
#include <curl/curl.h>
#include <sqlite3.h> // for sqlite3_snprintf
#endif

//! @cond Doxygen_Suppress

using namespace NS_PROJ::internal;

NS_PROJ_START

// ---------------------------------------------------------------------------

File::File(const std::string &name) : name_(name) {}

// ---------------------------------------------------------------------------

File::~File() = default;

// ---------------------------------------------------------------------------

class FileStdio : public File {
    PJ_CONTEXT *m_ctx;
    FILE *m_fp;

    FileStdio(const FileStdio &) = delete;
    FileStdio &operator=(const FileStdio &) = delete;

  protected:
    FileStdio(const std::string &name, PJ_CONTEXT *ctx, FILE *fp)
        : File(name), m_ctx(ctx), m_fp(fp) {}

  public:
    ~FileStdio() override;

    size_t read(void *buffer, size_t sizeBytes) override;
    bool seek(unsigned long long offset, int whence = SEEK_SET) override;
    unsigned long long tell() override;
    void reassign_context(PJ_CONTEXT *ctx) override { m_ctx = ctx; }

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
    return std::unique_ptr<File>(fp ? new FileStdio(filename, ctx, fp)
                                    : nullptr);
}

// ---------------------------------------------------------------------------

#ifndef REMOVE_LEGACY_SUPPORT

class FileLegacyAdapter : public File {
    PJ_CONTEXT *m_ctx;
    PAFile m_fp;

    FileLegacyAdapter(const FileLegacyAdapter &) = delete;
    FileLegacyAdapter &operator=(const FileLegacyAdapter &) = delete;

  protected:
    FileLegacyAdapter(const std::string &name, PJ_CONTEXT *ctx, PAFile fp)
        : File(name), m_ctx(ctx), m_fp(fp) {}

  public:
    ~FileLegacyAdapter() override;

    size_t read(void *buffer, size_t sizeBytes) override;
    bool seek(unsigned long long offset, int whence = SEEK_SET) override;
    unsigned long long tell() override;
    void reassign_context(PJ_CONTEXT *ctx) override { m_ctx = ctx; }

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
    return std::unique_ptr<File>(fid ? new FileLegacyAdapter(filename, ctx, fid)
                                     : nullptr);
}

#endif // REMOVE_LEGACY_SUPPORT

// ---------------------------------------------------------------------------

constexpr size_t DOWNLOAD_CHUNK_SIZE = 16 * 1024;
constexpr int MAX_CHUNKS = 64;

class NetworkChunkCache {
  public:
    void insert(const std::string &url, unsigned long long chunkIdx,
                std::vector<unsigned char> &&data);

    std::shared_ptr<std::vector<unsigned char>>
    get(const std::string &url, unsigned long long chunkIdx);

    void clear();

  private:
    struct Key {
        std::string url;
        unsigned long long chunkIdx;

        Key(const std::string &urlIn, unsigned long long chunkIdxIn)
            : url(urlIn), chunkIdx(chunkIdxIn) {}
        bool operator==(const Key &other) const {
            return url == other.url && chunkIdx == other.chunkIdx;
        }
    };

    struct KeyHasher {
        std::size_t operator()(const Key &k) const {
            return std::hash<std::string>{}(k.url) ^
                   (std::hash<unsigned long long>{}(k.chunkIdx) << 1);
        }
    };

    lru11::Cache<
        Key, std::shared_ptr<std::vector<unsigned char>>, MyMutex,
        std::unordered_map<
            Key,
            typename std::list<lru11::KeyValuePair<
                Key, std::shared_ptr<std::vector<unsigned char>>>>::iterator,
            KeyHasher>>
        cache_{MAX_CHUNKS};
};

// ---------------------------------------------------------------------------

void NetworkChunkCache::insert(const std::string &url,
                               unsigned long long chunkIdx,
                               std::vector<unsigned char> &&data) {
    cache_.insert(
        Key(url, chunkIdx),
        std::make_shared<std::vector<unsigned char>>(std::move(data)));
}

// ---------------------------------------------------------------------------

std::shared_ptr<std::vector<unsigned char>>
NetworkChunkCache::get(const std::string &url, unsigned long long chunkIdx) {
    std::shared_ptr<std::vector<unsigned char>> ret;
    cache_.tryGet(Key(url, chunkIdx), ret);
    return ret;
}

// ---------------------------------------------------------------------------

void NetworkChunkCache::clear() { cache_.clear(); }

// ---------------------------------------------------------------------------

static NetworkChunkCache gNetworkChunkCache{};

struct FileProperties {
    unsigned long long size;
};

static lru11::Cache<std::string, FileProperties, MyMutex>
    gNetworkFileProperties{};

// ---------------------------------------------------------------------------

class NetworkFile : public File {
    PJ_CONTEXT *m_ctx;
    std::string m_url;
    PROJ_NETWORK_HANDLE *m_handle;
    unsigned long long m_pos = 0;
    size_t m_nBlocksToDownload = 1;
    unsigned long long m_lastDownloadedOffset;
    unsigned long long m_filesize;
    proj_network_close_cbk_type m_closeCbk;

    NetworkFile(const NetworkFile &) = delete;
    NetworkFile &operator=(const NetworkFile &) = delete;

  protected:
    NetworkFile(PJ_CONTEXT *ctx, const std::string &url,
                PROJ_NETWORK_HANDLE *handle,
                unsigned long long lastDownloadOffset,
                unsigned long long filesize)
        : File(url), m_ctx(ctx), m_url(url), m_handle(handle),
          m_lastDownloadedOffset(lastDownloadOffset), m_filesize(filesize),
          m_closeCbk(ctx->networking.close) {}

  public:
    ~NetworkFile() override;

    size_t read(void *buffer, size_t sizeBytes) override;
    bool seek(unsigned long long offset, int whence) override;
    unsigned long long tell() override;
    void reassign_context(PJ_CONTEXT *ctx) override;

    static std::unique_ptr<File> open(PJ_CONTEXT *ctx, const char *filename);
};

// ---------------------------------------------------------------------------

std::unique_ptr<File> NetworkFile::open(PJ_CONTEXT *ctx, const char *filename) {
    if (gNetworkChunkCache.get(filename, 0)) {
        unsigned long long filesize = 0;
        FileProperties props;
        if (gNetworkFileProperties.tryGet(filename, props)) {
            filesize = props.size;
        }
        return std::unique_ptr<File>(new NetworkFile(
            ctx, filename, nullptr,
            std::numeric_limits<unsigned long long>::max(), filesize));
    } else {
        std::vector<unsigned char> buffer(DOWNLOAD_CHUNK_SIZE);
        size_t size_read = 0;
        std::string errorBuffer;
        errorBuffer.resize(1024);

        auto handle = ctx->networking.open(
            ctx, filename, 0, buffer.size(), &buffer[0], &size_read,
            errorBuffer.size(), &errorBuffer[0], ctx->networking.user_data);
        buffer.resize(size_read);
        gNetworkChunkCache.insert(filename, 0, std::move(buffer));
        if (!handle) {
            errorBuffer.resize(strlen(errorBuffer.data()));
            pj_log(ctx, PJ_LOG_ERROR, "Cannot open %s: %s", filename,
                   errorBuffer.c_str());
        }

        unsigned long long filesize = 0;
        if (handle) {
            const char *contentRange = ctx->networking.get_header_value(
                ctx, handle, "Content-Range", ctx->networking.user_data);
            if (contentRange) {
                const char *slash = strchr(contentRange, '/');
                if (slash) {
                    filesize = std::stoull(slash + 1);
                    FileProperties props;
                    props.size = filesize;
                    gNetworkFileProperties.insert(filename, props);
                }
            }
        }

        return std::unique_ptr<File>(
            handle ? new NetworkFile(ctx, filename, handle, size_read, filesize)
                   : nullptr);
    }
}

// ---------------------------------------------------------------------------

size_t NetworkFile::read(void *buffer, size_t sizeBytes) {

    if (sizeBytes == 0)
        return 0;

    auto iterOffset = m_pos;
    while (sizeBytes) {
        const auto chunkIdxToDownload = iterOffset / DOWNLOAD_CHUNK_SIZE;
        const auto offsetToDownload = chunkIdxToDownload * DOWNLOAD_CHUNK_SIZE;
        std::vector<unsigned char> region;
        auto pChunk = gNetworkChunkCache.get(m_url, chunkIdxToDownload);
        if (pChunk != nullptr) {
            region = *pChunk;
        } else {
            if (offsetToDownload == m_lastDownloadedOffset) {
                // In case of consecutive reads (of small size), we use a
                // heuristic that we will read the file sequentially, so
                // we double the requested size to decrease the number of
                // client/server roundtrips.
                if (m_nBlocksToDownload < 100)
                    m_nBlocksToDownload *= 2;
            } else {
                // Random reads. Cancel the above heuristics.
                m_nBlocksToDownload = 1;
            }

            // Ensure that we will request at least the number of blocks
            // to satisfy the remaining buffer size to read.
            const auto endOffsetToDownload =
                ((iterOffset + sizeBytes + DOWNLOAD_CHUNK_SIZE - 1) /
                 DOWNLOAD_CHUNK_SIZE) *
                DOWNLOAD_CHUNK_SIZE;
            const auto nMinBlocksToDownload = static_cast<size_t>(
                (endOffsetToDownload - offsetToDownload) / DOWNLOAD_CHUNK_SIZE);
            if (m_nBlocksToDownload < nMinBlocksToDownload)
                m_nBlocksToDownload = nMinBlocksToDownload;

            // Avoid reading already cached data.
            // Note: this might get evicted if concurrent reads are done, but
            // this should not cause bugs. Just missed optimization.
            for (size_t i = 1; i < m_nBlocksToDownload; i++) {
                if (gNetworkChunkCache.get(m_url, chunkIdxToDownload + i) !=
                    nullptr) {
                    m_nBlocksToDownload = i;
                    break;
                }
            }

            if (m_nBlocksToDownload > MAX_CHUNKS)
                m_nBlocksToDownload = MAX_CHUNKS;

            region.resize(m_nBlocksToDownload * DOWNLOAD_CHUNK_SIZE);
            size_t nRead = 0;
            std::string errorBuffer;
            errorBuffer.resize(1024);
            if (!m_handle) {
                m_handle = m_ctx->networking.open(
                    m_ctx, m_url.c_str(), offsetToDownload,
                    m_nBlocksToDownload * DOWNLOAD_CHUNK_SIZE, &region[0],
                    &nRead, errorBuffer.size(), &errorBuffer[0],
                    m_ctx->networking.user_data);
                if (!m_handle) {
                    return 0;
                }
            } else {
                nRead = m_ctx->networking.read_range(
                    m_ctx, m_handle, offsetToDownload,
                    m_nBlocksToDownload * DOWNLOAD_CHUNK_SIZE, &region[0],
                    errorBuffer.size(), &errorBuffer[0],
                    m_ctx->networking.user_data);
            }
            if (nRead == 0) {
                errorBuffer.resize(strlen(errorBuffer.data()));
                if (!errorBuffer.empty()) {
                    pj_log(m_ctx, PJ_LOG_ERROR, "Cannot read in %s: %s",
                           m_url.c_str(), errorBuffer.c_str());
                }
                return 0;
            }
            region.resize(nRead);
            m_lastDownloadedOffset = offsetToDownload + nRead;

            const auto nChunks =
                (region.size() + DOWNLOAD_CHUNK_SIZE - 1) / DOWNLOAD_CHUNK_SIZE;
            for (size_t i = 0; i < nChunks; i++) {
                std::vector<unsigned char> chunk(
                    region.data() + i * DOWNLOAD_CHUNK_SIZE,
                    region.data() +
                        std::min((i + 1) * DOWNLOAD_CHUNK_SIZE, region.size()));
                gNetworkChunkCache.insert(m_url, chunkIdxToDownload + i,
                                          std::move(chunk));
            }
        }
        const size_t nToCopy = static_cast<size_t>(
            std::min(static_cast<unsigned long long>(sizeBytes),
                     region.size() - (iterOffset - offsetToDownload)));
        memcpy(buffer, region.data() + iterOffset - offsetToDownload, nToCopy);
        buffer = static_cast<char *>(buffer) + nToCopy;
        iterOffset += nToCopy;
        sizeBytes -= nToCopy;
        if (region.size() < static_cast<size_t>(DOWNLOAD_CHUNK_SIZE) &&
            sizeBytes != 0) {
            break;
        }
    }

    size_t nRead = static_cast<size_t>(iterOffset - m_pos);
    m_pos = iterOffset;
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
        m_pos = m_filesize;
    }
    return true;
}

// ---------------------------------------------------------------------------

unsigned long long NetworkFile::tell() { return m_pos; }

// ---------------------------------------------------------------------------

NetworkFile::~NetworkFile() {
    if (m_handle) {
        m_ctx->networking.close(m_ctx, m_handle, m_ctx->networking.user_data);
    }
}

// ---------------------------------------------------------------------------

void NetworkFile::reassign_context(PJ_CONTEXT *ctx) {
    m_ctx = ctx;
    if (m_closeCbk != m_ctx->networking.close) {
        pj_log(m_ctx, PJ_LOG_ERROR,
               "Networking close callback has changed following context "
               "reassignment ! This is highly suspicious");
    }
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
        if (!pj_context_is_network_enabled(ctx)) {
            pj_log(
                ctx, PJ_LOG_ERROR,
                "Attempt at accessing remote resource not authorized. Either "
                "set PROJ_NETWORK=ON or "
                "proj_context_set_enable_network(ctx, TRUE)");
            return nullptr;
        }
        return NetworkFile::open(ctx, filename);
    }
    return FileStdio::open(ctx, filename);
}

// ---------------------------------------------------------------------------

#ifdef CURL_ENABLED

struct CurlFileHandle {
    CURL *m_handle = nullptr;
    std::string m_headers;
    std::string m_lastval{};

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

static PROJ_NETWORK_HANDLE *
pj_curl_open(PJ_CONTEXT *, const char *url, unsigned long long offset,
             size_t size_to_read, void *buffer, size_t *out_size_read,
             size_t error_string_max_size, char *out_error_string, void *) {
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
    sqlite3_snprintf(sizeof(szBuffer), szBuffer, "%llu-%llu", offset,
                     offset + size_to_read - 1);
    curl_easy_setopt(hCurlHandle, CURLOPT_RANGE, szBuffer);

    std::string headers;
    headers.reserve(16 * 1024);
    curl_easy_setopt(hCurlHandle, CURLOPT_HEADERDATA, &headers);
    curl_easy_setopt(hCurlHandle, CURLOPT_HEADERFUNCTION, pj_curl_write_func);

    std::string body;
    body.reserve(size_to_read);
    curl_easy_setopt(hCurlHandle, CURLOPT_WRITEDATA, &body);
    curl_easy_setopt(hCurlHandle, CURLOPT_WRITEFUNCTION, pj_curl_write_func);

    char szCurlErrBuf[CURL_ERROR_SIZE + 1] = {};
    szCurlErrBuf[0] = '\0';
    curl_easy_setopt(hCurlHandle, CURLOPT_ERRORBUFFER, szCurlErrBuf);

    curl_easy_perform(hCurlHandle);

    long response_code = 0;
    curl_easy_getinfo(hCurlHandle, CURLINFO_HTTP_CODE, &response_code);

    curl_easy_setopt(hCurlHandle, CURLOPT_HEADERDATA, nullptr);
    curl_easy_setopt(hCurlHandle, CURLOPT_HEADERFUNCTION, nullptr);

    curl_easy_setopt(hCurlHandle, CURLOPT_WRITEDATA, nullptr);
    curl_easy_setopt(hCurlHandle, CURLOPT_WRITEFUNCTION, nullptr);

    curl_easy_setopt(hCurlHandle, CURLOPT_ERRORBUFFER, nullptr);

    if (response_code == 0 || response_code >= 300) {
        if (out_error_string) {
            if (szCurlErrBuf[0]) {
                snprintf(out_error_string, error_string_max_size, "%s",
                         szCurlErrBuf);
            } else {
                snprintf(out_error_string, error_string_max_size,
                         "HTTP error %ld: %s", response_code, body.c_str());
            }
        }
        curl_easy_cleanup(hCurlHandle);
        return nullptr;
    }
    if (out_error_string && error_string_max_size) {
        out_error_string[0] = '\0';
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
                                 void *buffer, size_t error_string_max_size,
                                 char *out_error_string, void *) {
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

    char szCurlErrBuf[CURL_ERROR_SIZE + 1] = {};
    szCurlErrBuf[0] = '\0';
    curl_easy_setopt(hCurlHandle, CURLOPT_ERRORBUFFER, szCurlErrBuf);

    curl_easy_perform(hCurlHandle);

    long response_code = 0;
    curl_easy_getinfo(hCurlHandle, CURLINFO_HTTP_CODE, &response_code);

    curl_easy_setopt(hCurlHandle, CURLOPT_WRITEDATA, nullptr);
    curl_easy_setopt(hCurlHandle, CURLOPT_WRITEFUNCTION, nullptr);

    curl_easy_setopt(hCurlHandle, CURLOPT_ERRORBUFFER, nullptr);

    if (response_code == 0 || response_code >= 300) {
        if (out_error_string) {
            if (szCurlErrBuf[0]) {
                snprintf(out_error_string, error_string_max_size, "%s",
                         szCurlErrBuf);
            } else {
                snprintf(out_error_string, error_string_max_size,
                         "HTTP error %ld: %s", response_code, body.c_str());
            }
        }
        return 0;
    }
    if (out_error_string && error_string_max_size) {
        out_error_string[0] = '\0';
    }

    if (!body.empty()) {
        memcpy(buffer, body.data(), std::min(size_to_read, body.size()));
    }
    return std::min(size_to_read, body.size());
}

// ---------------------------------------------------------------------------

static const char *pj_curl_get_header_value(PJ_CONTEXT *,
                                            PROJ_NETWORK_HANDLE *raw_handle,
                                            const char *header_name, void *) {
    auto handle = reinterpret_cast<CurlFileHandle *>(raw_handle);
    auto pos = ci_find(handle->m_headers, header_name);
    if (pos == std::string::npos)
        return nullptr;
    const char *c_str = handle->m_headers.c_str();
    if (c_str[pos] == ':')
        pos++;
    while (c_str[pos] == ' ')
        pos++;
    auto posEnd = pos;
    while (c_str[posEnd] != '\n' && c_str[posEnd] != '\0')
        posEnd++;
    handle->m_lastval = handle->m_headers.substr(pos, posEnd - pos);
    return handle->m_lastval.c_str();
}

#else

// ---------------------------------------------------------------------------

static PROJ_NETWORK_HANDLE *
no_op_network_open(PJ_CONTEXT *ctx, const char * /* url */,
                   unsigned long long, /* offset */
                   size_t,             /* size to read */
                   void *,             /* buffer to update with bytes read*/
                   size_t *,           /* output: size actually read */
                   size_t error_string_max_size, char *out_error_string,
                   void * /*user_data*/) {
    if (out_error_string) {
        snprintf(out_error_string, error_string_max_size, "%s",
                 "Network functionality not available");
    }
    return nullptr;
}

// ---------------------------------------------------------------------------

static void no_op_network_close(PJ_CONTEXT *, PROJ_NETWORK_HANDLE *,
                                void * /*user_data*/) {}

#endif

// ---------------------------------------------------------------------------

void FileManager::fillDefaultNetworkInterface(PJ_CONTEXT *ctx) {
#ifdef CURL_ENABLED
    ctx->networking.open = pj_curl_open;
    ctx->networking.close = pj_curl_close;
    ctx->networking.read_range = pj_curl_read_range;
    ctx->networking.get_header_value = pj_curl_get_header_value;
#else
    ctx->networking.open = no_op_network_open;
    ctx->networking.close = no_op_network_close;
#endif
}

// ---------------------------------------------------------------------------

void FileManager::clearCache() {
    gNetworkChunkCache.clear();
    gNetworkFileProperties.clear();
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
 * @param read_range_cbk Callback to read a range of bytes inside a remote file.
 * @param user_data Arbitrary pointer provided by the user, and passed to the
 * above callbacks. May be NULL.
 * @return TRUE in case of success.
 */
int proj_context_set_network_callbacks(
    PJ_CONTEXT *ctx, proj_network_open_cbk_type open_cbk,
    proj_network_close_cbk_type close_cbk,
    proj_network_get_header_value_cbk_type get_header_value_cbk,
    proj_network_read_range_type read_range_cbk, void *user_data) {
    if (ctx == nullptr) {
        ctx = pj_get_default_ctx();
    }
    if (!open_cbk || !close_cbk || !get_header_value_cbk || !read_range_cbk) {
        return false;
    }
    ctx->networking.open = open_cbk;
    ctx->networking.close = close_cbk;
    ctx->networking.get_header_value = get_header_value_cbk;
    ctx->networking.read_range = read_range_cbk;
    ctx->networking.user_data = user_data;
    return true;
}

// ---------------------------------------------------------------------------

/** Enable or disable network access.
*
* This overrides the default endpoint in the PROJ configuration file or with
* the PROJ_NETWORK environment variable.
*
* @param ctx PROJ context, or NULL
* @param enable TRUE if network access is allowed.
* @return TRUE if network access is possible. That is either libcurl is
*         available, or an alternate interface has been set.
*/
int proj_context_set_enable_network(PJ_CONTEXT *ctx, int enable) {
    if (ctx == nullptr) {
        ctx = pj_get_default_ctx();
    }
    // Load ini file, now so as to override its network settings
    pj_load_ini(ctx);
    ctx->networking.enabled_env_variable_checked = true;
    ctx->networking.enabled = enable != FALSE;
#ifdef CURL_ENABLED
    return ctx->networking.enabled;
#else
    return ctx->networking.enabled &&
           ctx->networking.open != NS_PROJ::no_op_network_open;
#endif
}

// ---------------------------------------------------------------------------

/** Define the URL endpoint to query for remote grids.
*
* This overrides the default endpoint in the PROJ configuration file or with
* the PROJ_NETWORK_ENDPOINT environment variable.
*
* @param ctx PROJ context, or NULL
* @param url Endpoint URL. Must NOT be NULL.
*/
void proj_context_set_url_endpoint(PJ_CONTEXT *ctx, const char *url) {
    if (ctx == nullptr) {
        ctx = pj_get_default_ctx();
    }
    // Load ini file, now so as to override its network settings
    pj_load_ini(ctx);
    ctx->endpoint = url;
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress

bool pj_context_is_network_enabled(PJ_CONTEXT *ctx) {
    if (ctx == nullptr) {
        ctx = pj_get_default_ctx();
    }
    if (ctx->networking.enabled_env_variable_checked) {
        return ctx->networking.enabled;
    }
    const char *enabled = getenv("PROJ_NETWORK");
    if (enabled && enabled[0] != '\0') {
        ctx->networking.enabled = ci_equal(enabled, "ON") ||
                                  ci_equal(enabled, "YES") ||
                                  ci_equal(enabled, "TRUE");
    }
    pj_load_ini(ctx);
    ctx->networking.enabled_env_variable_checked = true;
    return ctx->networking.enabled;
}

//! @endcond
