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

#include <stdlib.h>

#include <algorithm>
#include <codecvt>
#include <functional>
#include <limits>
#include <locale>
#include <string>

#include "filemanager.hpp"
#include "proj.h"
#include "proj/internal/internal.hpp"
#include "proj/internal/lru_cache.hpp"
#include "proj_internal.h"
#include "sqlite3.hpp"

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

#include <sys/stat.h>

#ifdef _WIN32
#include <shlobj.h>
#else
#include <sys/types.h>
#include <unistd.h>
#endif

#if defined(_WIN32)
#include <windows.h>
#elif defined(__MACH__) && defined(__APPLE__)
#include <mach-o/dyld.h>
#elif defined(__FreeBSD__)
#include <sys/sysctl.h>
#include <sys/types.h>
#endif

#include <time.h>

//! @cond Doxygen_Suppress

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

using namespace NS_PROJ::internal;

NS_PROJ_START

// ---------------------------------------------------------------------------

static void proj_sleep_ms(int ms) {
#ifdef _WIN32
    Sleep(ms);
#else
    usleep(ms * 1000);
#endif
}

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

struct FileProperties {
    unsigned long long size = 0;
    time_t lastChecked = 0;
    std::string lastModified{};
    std::string etag{};
};

class NetworkChunkCache {
  public:
    void insert(PJ_CONTEXT *ctx, const std::string &url,
                unsigned long long chunkIdx, std::vector<unsigned char> &&data);

    std::shared_ptr<std::vector<unsigned char>>
    get(PJ_CONTEXT *ctx, const std::string &url, unsigned long long chunkIdx);

    std::shared_ptr<std::vector<unsigned char>> get(PJ_CONTEXT *ctx,
                                                    const std::string &url,
                                                    unsigned long long chunkIdx,
                                                    FileProperties &props);

    void clearMemoryCache();

    static void clearDiskChunkCache(PJ_CONTEXT *ctx);

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

static NetworkChunkCache gNetworkChunkCache{};

// ---------------------------------------------------------------------------

class NetworkFilePropertiesCache {
  public:
    void insert(PJ_CONTEXT *ctx, const std::string &url, FileProperties &props);

    bool tryGet(PJ_CONTEXT *ctx, const std::string &url, FileProperties &props);

    void clearMemoryCache();

  private:
    lru11::Cache<std::string, FileProperties, MyMutex> cache_{};
};

// ---------------------------------------------------------------------------

static NetworkFilePropertiesCache gNetworkFileProperties{};

// ---------------------------------------------------------------------------

class DiskChunkCache {
    PJ_CONTEXT *ctx_ = nullptr;
    std::string path_{};
    sqlite3 *hDB_ = nullptr;
    std::string thisNamePtr_{};
    std::unique_ptr<SQLite3VFS> vfs_{};

    explicit DiskChunkCache(PJ_CONTEXT *ctx, const std::string &path);

    bool createDBStructure();
    bool checkConsistency();
    bool get_links(sqlite3_int64 chunk_id, sqlite3_int64 &link_id,
                   sqlite3_int64 &prev, sqlite3_int64 &next,
                   sqlite3_int64 &head, sqlite3_int64 &tail);
    bool update_links_of_prev_and_next_links(sqlite3_int64 prev,
                                             sqlite3_int64 next);
    bool update_linked_chunks(sqlite3_int64 link_id, sqlite3_int64 prev,
                              sqlite3_int64 next);
    bool update_linked_chunks_head_tail(sqlite3_int64 head, sqlite3_int64 tail);

    DiskChunkCache(const DiskChunkCache &) = delete;
    DiskChunkCache &operator=(const DiskChunkCache &) = delete;

  public:
    static std::unique_ptr<DiskChunkCache> open(PJ_CONTEXT *ctx);
    ~DiskChunkCache();

    sqlite3 *handle() { return hDB_; }
    std::unique_ptr<SQLiteStatement> prepare(const char *sql);
    bool move_to_head(sqlite3_int64 chunk_id);
    bool move_to_tail(sqlite3_int64 chunk_id);
    void closeAndUnlink();
};

// ---------------------------------------------------------------------------

static bool pj_context_get_grid_cache_is_enabled(PJ_CONTEXT *ctx) {
    pj_load_ini(ctx);
    return ctx->gridChunkCache.enabled;
}

// ---------------------------------------------------------------------------

static long long pj_context_get_grid_cache_max_size(PJ_CONTEXT *ctx) {
    pj_load_ini(ctx);
    return ctx->gridChunkCache.max_size;
}

// ---------------------------------------------------------------------------

static int pj_context_get_grid_cache_ttl(PJ_CONTEXT *ctx) {
    pj_load_ini(ctx);
    return ctx->gridChunkCache.ttl;
}

// ---------------------------------------------------------------------------

std::unique_ptr<DiskChunkCache> DiskChunkCache::open(PJ_CONTEXT *ctx) {
    if (!pj_context_get_grid_cache_is_enabled(ctx)) {
        return nullptr;
    }
    const auto cachePath = pj_context_get_grid_cache_filename(ctx);
    if (cachePath.empty()) {
        return nullptr;
    }

    auto diskCache =
        std::unique_ptr<DiskChunkCache>(new DiskChunkCache(ctx, cachePath));
    if (!diskCache->hDB_)
        diskCache.reset();
    return diskCache;
}

// ---------------------------------------------------------------------------

DiskChunkCache::DiskChunkCache(PJ_CONTEXT *ctx, const std::string &path)
    : ctx_(ctx), path_(path), vfs_(SQLite3VFS::create(true, false, false)) {
    if (vfs_ == nullptr) {
        return;
    }
    sqlite3_open_v2(path.c_str(), &hDB_,
                    SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, vfs_->name());
    if (!hDB_) {
        return;
    }
    for (int i = 0;; i++) {
        int ret =
            sqlite3_exec(hDB_, "BEGIN EXCLUSIVE", nullptr, nullptr, nullptr);
        if (ret == SQLITE_OK) {
            break;
        }
        if (ret != SQLITE_BUSY) {
            pj_log(ctx_, PJ_LOG_ERROR, "%s", sqlite3_errmsg(hDB_));
            sqlite3_close(hDB_);
            hDB_ = nullptr;
            return;
        }
        const char *max_iters = getenv("PROJ_LOCK_MAX_ITERS");
        if (i >= (max_iters && max_iters[0] ? atoi(max_iters)
                                            : 30)) { // A bit more than 1 second
            pj_log(ctx_, PJ_LOG_ERROR, "Cannot take exclusive lock on %s",
                   path.c_str());
            sqlite3_close(hDB_);
            hDB_ = nullptr;
            return;
        }
        pj_log(ctx, PJ_LOG_TRACE, "Lock taken on cache. Waiting a bit...");
        // Retry every 5 ms for 50 ms, then every 10 ms for 100 ms, then
        // every 100 ms
        proj_sleep_ms(i < 10 ? 5 : i < 20 ? 10 : 100);
    }
    char **pasResult = nullptr;
    int nRows = 0;
    int nCols = 0;
    sqlite3_get_table(hDB_,
                      "SELECT 1 FROM sqlite_master WHERE name = 'properties'",
                      &pasResult, &nRows, &nCols, nullptr);
    sqlite3_free_table(pasResult);
    if (nRows == 0) {
        if (!createDBStructure()) {
            sqlite3_close(hDB_);
            hDB_ = nullptr;
            return;
        }
    }

    if (getenv("PROJ_CHECK_CACHE_CONSISTENCY")) {
        checkConsistency();
    }
}

// ---------------------------------------------------------------------------

static const char *cache_db_structure_sql =
    "CREATE TABLE properties("
    " url          TEXT PRIMARY KEY NOT NULL,"
    " lastChecked  TIMESTAMP NOT NULL,"
    " fileSize     INTEGER NOT NULL,"
    " lastModified TEXT,"
    " etag         TEXT"
    ");"
    "CREATE TABLE chunk_data("
    " id        INTEGER PRIMARY KEY AUTOINCREMENT CHECK (id > 0),"
    " data      BLOB NOT NULL"
    ");"
    "CREATE TABLE chunks("
    " id        INTEGER PRIMARY KEY AUTOINCREMENT CHECK (id > 0),"
    " url       TEXT NOT NULL,"
    " offset    INTEGER NOT NULL,"
    " data_id   INTEGER NOT NULL,"
    " data_size INTEGER NOT NULL,"
    " CONSTRAINT fk_chunks_url FOREIGN KEY (url) REFERENCES properties(url),"
    " CONSTRAINT fk_chunks_data FOREIGN KEY (data_id) REFERENCES chunk_data(id)"
    ");"
    "CREATE INDEX idx_chunks ON chunks(url, offset);"
    "CREATE TABLE linked_chunks("
    " id        INTEGER PRIMARY KEY AUTOINCREMENT CHECK (id > 0),"
    " chunk_id  INTEGER NOT NULL,"
    " prev      INTEGER,"
    " next      INTEGER,"
    " CONSTRAINT fk_links_chunkid FOREIGN KEY (chunk_id) REFERENCES chunks(id),"
    " CONSTRAINT fk_links_prev FOREIGN KEY (prev) REFERENCES linked_chunks(id),"
    " CONSTRAINT fk_links_next FOREIGN KEY (next) REFERENCES linked_chunks(id)"
    ");"
    "CREATE INDEX idx_linked_chunks_chunk_id ON linked_chunks(chunk_id);"
    "CREATE TABLE linked_chunks_head_tail("
    "  head       INTEGER,"
    "  tail       INTEGER,"
    "  CONSTRAINT lht_head FOREIGN KEY (head) REFERENCES linked_chunks(id),"
    "  CONSTRAINT lht_tail FOREIGN KEY (tail) REFERENCES linked_chunks(id)"
    ");"
    "INSERT INTO linked_chunks_head_tail VALUES (NULL, NULL);";

bool DiskChunkCache::createDBStructure() {

    pj_log(ctx_, PJ_LOG_TRACE, "Creating cache DB structure");
    if (sqlite3_exec(hDB_, cache_db_structure_sql, nullptr, nullptr, nullptr) !=
        SQLITE_OK) {
        pj_log(ctx_, PJ_LOG_ERROR, "%s", sqlite3_errmsg(hDB_));
        return false;
    }
    return true;
}

// ---------------------------------------------------------------------------

#define INVALIDATED_SQL_LITERAL "'invalidated'"

bool DiskChunkCache::checkConsistency() {

    auto stmt = prepare("SELECT * FROM chunk_data WHERE id NOT IN (SELECT "
                        "data_id FROM chunks)");
    if (!stmt) {
        return false;
    }
    if (stmt->execute() != SQLITE_DONE) {
        fprintf(stderr, "Rows in chunk_data not referenced by chunks.\n");
        return false;
    }

    stmt = prepare("SELECT * FROM chunks WHERE id NOT IN (SELECT chunk_id FROM "
                   "linked_chunks)");
    if (!stmt) {
        return false;
    }
    if (stmt->execute() != SQLITE_DONE) {
        fprintf(stderr, "Rows in chunks not referenced by linked_chunks.\n");
        return false;
    }

    stmt = prepare("SELECT * FROM chunks WHERE url <> " INVALIDATED_SQL_LITERAL
                   " AND url "
                   "NOT IN (SELECT url FROM properties)");
    if (!stmt) {
        return false;
    }
    if (stmt->execute() != SQLITE_DONE) {
        fprintf(stderr, "url values in chunks not referenced by properties.\n");
        return false;
    }

    stmt = prepare("SELECT head, tail FROM linked_chunks_head_tail");
    if (!stmt) {
        return false;
    }
    if (stmt->execute() != SQLITE_ROW) {
        fprintf(stderr, "linked_chunks_head_tail empty.\n");
        return false;
    }
    const auto head = stmt->getInt64();
    const auto tail = stmt->getInt64();
    if (stmt->execute() != SQLITE_DONE) {
        fprintf(stderr, "linked_chunks_head_tail has more than one row.\n");
        return false;
    }

    stmt = prepare("SELECT COUNT(*) FROM linked_chunks");
    if (!stmt) {
        return false;
    }
    if (stmt->execute() != SQLITE_ROW) {
        fprintf(stderr, "linked_chunks_head_tail empty.\n");
        return false;
    }
    const auto count_linked_chunks = stmt->getInt64();

    if (head) {
        auto id = head;
        std::set<sqlite3_int64> visitedIds;
        stmt = prepare("SELECT next FROM linked_chunks WHERE id = ?");
        if (!stmt) {
            return false;
        }
        while (true) {
            visitedIds.insert(id);
            stmt->reset();
            stmt->bindInt64(id);
            if (stmt->execute() != SQLITE_ROW) {
                fprintf(stderr, "cannot find linked_chunks.id = %d.\n",
                        static_cast<int>(id));
                return false;
            }
            auto next = stmt->getInt64();
            if (next == 0) {
                if (id != tail) {
                    fprintf(stderr,
                            "last item when following next is not tail.\n");
                    return false;
                }
                break;
            }
            if (visitedIds.find(next) != visitedIds.end()) {
                fprintf(stderr, "found cycle on linked_chunks.next = %d.\n",
                        static_cast<int>(next));
                return false;
            }
            id = next;
        }
        if (visitedIds.size() != static_cast<size_t>(count_linked_chunks)) {
            fprintf(stderr,
                    "ghost items in linked_chunks when following next.\n");
            return false;
        }
    } else if (count_linked_chunks) {
        fprintf(stderr, "linked_chunks_head_tail.head = NULL but linked_chunks "
                        "not empty.\n");
        return false;
    }

    if (tail) {
        auto id = tail;
        std::set<sqlite3_int64> visitedIds;
        stmt = prepare("SELECT prev FROM linked_chunks WHERE id = ?");
        if (!stmt) {
            return false;
        }
        while (true) {
            visitedIds.insert(id);
            stmt->reset();
            stmt->bindInt64(id);
            if (stmt->execute() != SQLITE_ROW) {
                fprintf(stderr, "cannot find linked_chunks.id = %d.\n",
                        static_cast<int>(id));
                return false;
            }
            auto prev = stmt->getInt64();
            if (prev == 0) {
                if (id != head) {
                    fprintf(stderr,
                            "last item when following prev is not head.\n");
                    return false;
                }
                break;
            }
            if (visitedIds.find(prev) != visitedIds.end()) {
                fprintf(stderr, "found cycle on linked_chunks.prev = %d.\n",
                        static_cast<int>(prev));
                return false;
            }
            id = prev;
        }
        if (visitedIds.size() != static_cast<size_t>(count_linked_chunks)) {
            fprintf(stderr,
                    "ghost items in linked_chunks when following prev.\n");
            return false;
        }
    } else if (count_linked_chunks) {
        fprintf(stderr, "linked_chunks_head_tail.tail = NULL but linked_chunks "
                        "not empty.\n");
        return false;
    }

    fprintf(stderr, "check ok\n");
    return true;
}

// ---------------------------------------------------------------------------

DiskChunkCache::~DiskChunkCache() {
    if (hDB_) {
        sqlite3_exec(hDB_, "COMMIT", nullptr, nullptr, nullptr);
        sqlite3_close(hDB_);
    }
}

// ---------------------------------------------------------------------------

void DiskChunkCache::closeAndUnlink() {
    if (hDB_) {
        sqlite3_exec(hDB_, "COMMIT", nullptr, nullptr, nullptr);
        sqlite3_close(hDB_);
    }
    if (vfs_) {
        vfs_->raw()->xDelete(vfs_->raw(), path_.c_str(), 0);
    }
}

// ---------------------------------------------------------------------------

std::unique_ptr<SQLiteStatement> DiskChunkCache::prepare(const char *sql) {
    sqlite3_stmt *hStmt = nullptr;
    sqlite3_prepare_v2(hDB_, sql, -1, &hStmt, nullptr);
    if (!hStmt) {
        pj_log(ctx_, PJ_LOG_ERROR, "%s", sqlite3_errmsg(hDB_));
        return nullptr;
    }
    return std::unique_ptr<SQLiteStatement>(new SQLiteStatement(hStmt));
}

// ---------------------------------------------------------------------------

bool DiskChunkCache::get_links(sqlite3_int64 chunk_id, sqlite3_int64 &link_id,
                               sqlite3_int64 &prev, sqlite3_int64 &next,
                               sqlite3_int64 &head, sqlite3_int64 &tail) {
    auto stmt =
        prepare("SELECT id, prev, next FROM linked_chunks WHERE chunk_id = ?");
    if (!stmt)
        return false;
    stmt->bindInt64(chunk_id);
    {
        const auto ret = stmt->execute();
        if (ret != SQLITE_ROW) {
            pj_log(ctx_, PJ_LOG_ERROR, "%s", sqlite3_errmsg(hDB_));
            return false;
        }
    }
    link_id = stmt->getInt64();
    prev = stmt->getInt64();
    next = stmt->getInt64();

    stmt = prepare("SELECT head, tail FROM linked_chunks_head_tail");
    {
        const auto ret = stmt->execute();
        if (ret != SQLITE_ROW) {
            pj_log(ctx_, PJ_LOG_ERROR, "%s", sqlite3_errmsg(hDB_));
            return false;
        }
    }
    head = stmt->getInt64();
    tail = stmt->getInt64();
    return true;
}

// ---------------------------------------------------------------------------

bool DiskChunkCache::update_links_of_prev_and_next_links(sqlite3_int64 prev,
                                                         sqlite3_int64 next) {
    if (prev) {
        auto stmt = prepare("UPDATE linked_chunks SET next = ? WHERE id = ?");
        if (!stmt)
            return false;
        if (next)
            stmt->bindInt64(next);
        else
            stmt->bindNull();
        stmt->bindInt64(prev);
        const auto ret = stmt->execute();
        if (ret != SQLITE_DONE) {
            pj_log(ctx_, PJ_LOG_ERROR, "%s", sqlite3_errmsg(hDB_));
            return false;
        }
    }

    if (next) {
        auto stmt = prepare("UPDATE linked_chunks SET prev = ? WHERE id = ?");
        if (!stmt)
            return false;
        if (prev)
            stmt->bindInt64(prev);
        else
            stmt->bindNull();
        stmt->bindInt64(next);
        const auto ret = stmt->execute();
        if (ret != SQLITE_DONE) {
            pj_log(ctx_, PJ_LOG_ERROR, "%s", sqlite3_errmsg(hDB_));
            return false;
        }
    }
    return true;
}

// ---------------------------------------------------------------------------

bool DiskChunkCache::update_linked_chunks(sqlite3_int64 link_id,
                                          sqlite3_int64 prev,
                                          sqlite3_int64 next) {
    auto stmt =
        prepare("UPDATE linked_chunks SET prev = ?, next = ? WHERE id = ?");
    if (!stmt)
        return false;
    if (prev)
        stmt->bindInt64(prev);
    else
        stmt->bindNull();
    if (next)
        stmt->bindInt64(next);
    else
        stmt->bindNull();
    stmt->bindInt64(link_id);
    const auto ret = stmt->execute();
    if (ret != SQLITE_DONE) {
        pj_log(ctx_, PJ_LOG_ERROR, "%s", sqlite3_errmsg(hDB_));
        return false;
    }
    return true;
}

// ---------------------------------------------------------------------------

bool DiskChunkCache::update_linked_chunks_head_tail(sqlite3_int64 head,
                                                    sqlite3_int64 tail) {
    auto stmt =
        prepare("UPDATE linked_chunks_head_tail SET head = ?, tail = ?");
    if (!stmt)
        return false;
    if (head)
        stmt->bindInt64(head);
    else
        stmt->bindNull(); // shouldn't happen normally
    if (tail)
        stmt->bindInt64(tail);
    else
        stmt->bindNull(); // shouldn't happen normally
    const auto ret = stmt->execute();
    if (ret != SQLITE_DONE) {
        pj_log(ctx_, PJ_LOG_ERROR, "%s", sqlite3_errmsg(hDB_));
        return false;
    }
    return true;
}

// ---------------------------------------------------------------------------

bool DiskChunkCache::move_to_head(sqlite3_int64 chunk_id) {

    sqlite3_int64 link_id = 0;
    sqlite3_int64 prev = 0;
    sqlite3_int64 next = 0;
    sqlite3_int64 head = 0;
    sqlite3_int64 tail = 0;
    if (!get_links(chunk_id, link_id, prev, next, head, tail)) {
        return false;
    }

    if (link_id == head) {
        return true;
    }

    if (!update_links_of_prev_and_next_links(prev, next)) {
        return false;
    }

    if (head) {
        auto stmt = prepare("UPDATE linked_chunks SET prev = ? WHERE id = ?");
        if (!stmt)
            return false;
        stmt->bindInt64(link_id);
        stmt->bindInt64(head);
        const auto ret = stmt->execute();
        if (ret != SQLITE_DONE) {
            pj_log(ctx_, PJ_LOG_ERROR, "%s", sqlite3_errmsg(hDB_));
            return false;
        }
    }

    return update_linked_chunks(link_id, 0, head) &&
           update_linked_chunks_head_tail(link_id,
                                          (link_id == tail) ? prev : tail);
}

// ---------------------------------------------------------------------------

bool DiskChunkCache::move_to_tail(sqlite3_int64 chunk_id) {
    sqlite3_int64 link_id = 0;
    sqlite3_int64 prev = 0;
    sqlite3_int64 next = 0;
    sqlite3_int64 head = 0;
    sqlite3_int64 tail = 0;
    if (!get_links(chunk_id, link_id, prev, next, head, tail)) {
        return false;
    }

    if (link_id == tail) {
        return true;
    }

    if (!update_links_of_prev_and_next_links(prev, next)) {
        return false;
    }

    if (tail) {
        auto stmt = prepare("UPDATE linked_chunks SET next = ? WHERE id = ?");
        if (!stmt)
            return false;
        stmt->bindInt64(link_id);
        stmt->bindInt64(tail);
        const auto ret = stmt->execute();
        if (ret != SQLITE_DONE) {
            pj_log(ctx_, PJ_LOG_ERROR, "%s", sqlite3_errmsg(hDB_));
            return false;
        }
    }

    return update_linked_chunks(link_id, tail, 0) &&
           update_linked_chunks_head_tail((link_id == head) ? next : head,
                                          link_id);
}

// ---------------------------------------------------------------------------

void NetworkChunkCache::insert(PJ_CONTEXT *ctx, const std::string &url,
                               unsigned long long chunkIdx,
                               std::vector<unsigned char> &&data) {
    auto dataPtr(std::make_shared<std::vector<unsigned char>>(std::move(data)));
    cache_.insert(Key(url, chunkIdx), dataPtr);

    auto diskCache = DiskChunkCache::open(ctx);
    if (!diskCache)
        return;
    auto hDB = diskCache->handle();

    // Always insert DOWNLOAD_CHUNK_SIZE bytes to avoid fragmentation
    std::vector<unsigned char> blob(*dataPtr);
    assert(blob.size() <= DOWNLOAD_CHUNK_SIZE);
    blob.resize(DOWNLOAD_CHUNK_SIZE);

    // Check if there is an existing entry for that URL and offset
    auto stmt = diskCache->prepare(
        "SELECT id, data_id FROM chunks WHERE url = ? AND offset = ?");
    if (!stmt)
        return;
    stmt->bindText(url.c_str());
    stmt->bindInt64(chunkIdx * DOWNLOAD_CHUNK_SIZE);

    const auto mainRet = stmt->execute();
    if (mainRet == SQLITE_ROW) {
        const auto chunk_id = stmt->getInt64();
        const auto data_id = stmt->getInt64();
        stmt =
            diskCache->prepare("UPDATE chunk_data SET data = ? WHERE id = ?");
        if (!stmt)
            return;
        stmt->bindBlob(blob.data(), blob.size());
        stmt->bindInt64(data_id);
        {
            const auto ret = stmt->execute();
            if (ret != SQLITE_DONE) {
                pj_log(ctx, PJ_LOG_ERROR, "%s", sqlite3_errmsg(hDB));
                return;
            }
        }

        diskCache->move_to_head(chunk_id);

        return;
    } else if (mainRet != SQLITE_DONE) {
        pj_log(ctx, PJ_LOG_ERROR, "%s", sqlite3_errmsg(hDB));
        return;
    }

    // Lambda to recycle an existing entry that was either invalidated, or
    // least recently used.
    const auto reuseExistingEntry = [ctx, &blob, &diskCache, hDB, &url,
                                     chunkIdx, &dataPtr](
        std::unique_ptr<SQLiteStatement> &stmtIn) {
        const auto chunk_id = stmtIn->getInt64();
        const auto data_id = stmtIn->getInt64();
        if (data_id <= 0) {
            pj_log(ctx, PJ_LOG_ERROR, "data_id <= 0");
            return;
        }

        auto l_stmt =
            diskCache->prepare("UPDATE chunk_data SET data = ? WHERE id = ?");
        if (!l_stmt)
            return;
        l_stmt->bindBlob(blob.data(), blob.size());
        l_stmt->bindInt64(data_id);
        {
            const auto ret2 = l_stmt->execute();
            if (ret2 != SQLITE_DONE) {
                pj_log(ctx, PJ_LOG_ERROR, "%s", sqlite3_errmsg(hDB));
                return;
            }
        }

        l_stmt = diskCache->prepare("UPDATE chunks SET url = ?, "
                                    "offset = ?, data_size = ?, data_id = ? "
                                    "WHERE id = ?");
        if (!l_stmt)
            return;
        l_stmt->bindText(url.c_str());
        l_stmt->bindInt64(chunkIdx * DOWNLOAD_CHUNK_SIZE);
        l_stmt->bindInt64(dataPtr->size());
        l_stmt->bindInt64(data_id);
        l_stmt->bindInt64(chunk_id);
        {
            const auto ret2 = l_stmt->execute();
            if (ret2 != SQLITE_DONE) {
                pj_log(ctx, PJ_LOG_ERROR, "%s", sqlite3_errmsg(hDB));
                return;
            }
        }

        diskCache->move_to_head(chunk_id);
    };

    // Find if there is an invalidated chunk we can reuse
    stmt = diskCache->prepare(
        "SELECT id, data_id FROM chunks "
        "WHERE id = (SELECT tail FROM linked_chunks_head_tail) AND "
        "url = " INVALIDATED_SQL_LITERAL);
    if (!stmt)
        return;
    {
        const auto ret = stmt->execute();
        if (ret == SQLITE_ROW) {
            reuseExistingEntry(stmt);
            return;
        } else if (ret != SQLITE_DONE) {
            pj_log(ctx, PJ_LOG_ERROR, "%s", sqlite3_errmsg(hDB));
            return;
        }
    }

    // Check if we have not reached the max size of the cache
    stmt = diskCache->prepare("SELECT COUNT(*) FROM chunks");
    if (!stmt)
        return;
    {
        const auto ret = stmt->execute();
        if (ret != SQLITE_ROW) {
            pj_log(ctx, PJ_LOG_ERROR, "%s", sqlite3_errmsg(hDB));
            return;
        }
    }

    const auto max_size = pj_context_get_grid_cache_max_size(ctx);
    if (max_size > 0 &&
        static_cast<long long>(stmt->getInt64() * DOWNLOAD_CHUNK_SIZE) >=
            max_size) {
        stmt = diskCache->prepare(
            "SELECT id, data_id FROM chunks "
            "WHERE id = (SELECT tail FROM linked_chunks_head_tail)");
        if (!stmt)
            return;

        const auto ret = stmt->execute();
        if (ret != SQLITE_ROW) {
            pj_log(ctx, PJ_LOG_ERROR, "%s", sqlite3_errmsg(hDB));
            return;
        }
        reuseExistingEntry(stmt);
        return;
    }

    // Otherwise just append a new entry
    stmt = diskCache->prepare("INSERT INTO chunk_data(data) VALUES (?)");
    if (!stmt)
        return;
    stmt->bindBlob(blob.data(), blob.size());
    {
        const auto ret = stmt->execute();
        if (ret != SQLITE_DONE) {
            pj_log(ctx, PJ_LOG_ERROR, "%s", sqlite3_errmsg(hDB));
            return;
        }
    }

    const auto chunk_data_id = sqlite3_last_insert_rowid(hDB);

    stmt = diskCache->prepare("INSERT INTO chunks(url, offset, data_id, "
                              "data_size) VALUES (?,?,?,?)");
    if (!stmt)
        return;
    stmt->bindText(url.c_str());
    stmt->bindInt64(chunkIdx * DOWNLOAD_CHUNK_SIZE);
    stmt->bindInt64(chunk_data_id);
    stmt->bindInt64(dataPtr->size());
    {
        const auto ret = stmt->execute();
        if (ret != SQLITE_DONE) {
            pj_log(ctx, PJ_LOG_ERROR, "%s", sqlite3_errmsg(hDB));
            return;
        }
    }

    const auto chunk_id = sqlite3_last_insert_rowid(hDB);

    stmt = diskCache->prepare(
        "INSERT INTO linked_chunks(chunk_id, prev, next) VALUES (?,NULL,NULL)");
    if (!stmt)
        return;
    stmt->bindInt64(chunk_id);
    if (stmt->execute() != SQLITE_DONE) {
        pj_log(ctx, PJ_LOG_ERROR, "%s", sqlite3_errmsg(hDB));
        return;
    }

    stmt = diskCache->prepare("SELECT head FROM linked_chunks_head_tail");
    if (!stmt)
        return;
    if (stmt->execute() != SQLITE_ROW) {
        pj_log(ctx, PJ_LOG_ERROR, "%s", sqlite3_errmsg(hDB));
        return;
    }
    if (stmt->getInt64() == 0) {
        stmt = diskCache->prepare(
            "UPDATE linked_chunks_head_tail SET head = ?, tail = ?");
        if (!stmt)
            return;
        stmt->bindInt64(chunk_id);
        stmt->bindInt64(chunk_id);
        if (stmt->execute() != SQLITE_DONE) {
            pj_log(ctx, PJ_LOG_ERROR, "%s", sqlite3_errmsg(hDB));
            return;
        }
    }

    diskCache->move_to_head(chunk_id);
}

// ---------------------------------------------------------------------------

std::shared_ptr<std::vector<unsigned char>>
NetworkChunkCache::get(PJ_CONTEXT *ctx, const std::string &url,
                       unsigned long long chunkIdx) {
    std::shared_ptr<std::vector<unsigned char>> ret;
    if (cache_.tryGet(Key(url, chunkIdx), ret)) {
        return ret;
    }

    auto diskCache = DiskChunkCache::open(ctx);
    if (!diskCache)
        return ret;
    auto hDB = diskCache->handle();

    auto stmt = diskCache->prepare(
        "SELECT chunks.id, chunks.data_size, chunk_data.data FROM chunks "
        "JOIN chunk_data ON chunks.id = chunk_data.id "
        "WHERE chunks.url = ? AND chunks.offset = ?");
    if (!stmt)
        return ret;

    stmt->bindText(url.c_str());
    stmt->bindInt64(chunkIdx * DOWNLOAD_CHUNK_SIZE);

    const auto mainRet = stmt->execute();
    if (mainRet == SQLITE_ROW) {
        const auto chunk_id = stmt->getInt64();
        const auto data_size = stmt->getInt64();
        int blob_size = 0;
        const void *blob = stmt->getBlob(blob_size);
        if (blob_size < data_size) {
            pj_log(ctx, PJ_LOG_ERROR,
                   "blob_size=%d < data_size for chunk_id=%d", blob_size,
                   static_cast<int>(chunk_id));
            return ret;
        }
        if (data_size > static_cast<sqlite3_int64>(DOWNLOAD_CHUNK_SIZE)) {
            pj_log(ctx, PJ_LOG_ERROR, "data_size > DOWNLOAD_CHUNK_SIZE");
            return ret;
        }
        ret.reset(new std::vector<unsigned char>());
        ret->assign(reinterpret_cast<const unsigned char *>(blob),
                    reinterpret_cast<const unsigned char *>(blob) +
                        static_cast<size_t>(data_size));
        cache_.insert(Key(url, chunkIdx), ret);

        if (!diskCache->move_to_head(chunk_id))
            return ret;
    } else if (mainRet != SQLITE_DONE) {
        pj_log(ctx, PJ_LOG_ERROR, "%s", sqlite3_errmsg(hDB));
    }

    return ret;
}

// ---------------------------------------------------------------------------

std::shared_ptr<std::vector<unsigned char>>
NetworkChunkCache::get(PJ_CONTEXT *ctx, const std::string &url,
                       unsigned long long chunkIdx, FileProperties &props) {
    if (!gNetworkFileProperties.tryGet(ctx, url, props)) {
        return nullptr;
    }

    return get(ctx, url, chunkIdx);
}

// ---------------------------------------------------------------------------

void NetworkChunkCache::clearMemoryCache() { cache_.clear(); }

// ---------------------------------------------------------------------------

void NetworkChunkCache::clearDiskChunkCache(PJ_CONTEXT *ctx) {
    auto diskCache = DiskChunkCache::open(ctx);
    if (!diskCache)
        return;
    diskCache->closeAndUnlink();
}

// ---------------------------------------------------------------------------

void NetworkFilePropertiesCache::insert(PJ_CONTEXT *ctx, const std::string &url,
                                        FileProperties &props) {
    time(&props.lastChecked);
    cache_.insert(url, props);

    auto diskCache = DiskChunkCache::open(ctx);
    if (!diskCache)
        return;
    auto hDB = diskCache->handle();
    auto stmt = diskCache->prepare("SELECT fileSize, lastModified, etag "
                                   "FROM properties WHERE url = ?");
    if (!stmt)
        return;
    stmt->bindText(url.c_str());
    if (stmt->execute() == SQLITE_ROW) {
        FileProperties cachedProps;
        cachedProps.size = stmt->getInt64();
        const char *lastModified = stmt->getText();
        cachedProps.lastModified = lastModified ? lastModified : std::string();
        const char *etag = stmt->getText();
        cachedProps.etag = etag ? etag : std::string();
        if (props.size != cachedProps.size ||
            props.lastModified != cachedProps.lastModified ||
            props.etag != cachedProps.etag) {

            // If cached properties don't match recent fresh ones, invalidate
            // cached chunks
            stmt = diskCache->prepare("SELECT id FROM chunks WHERE url = ?");
            if (!stmt)
                return;
            stmt->bindText(url.c_str());
            std::vector<sqlite3_int64> ids;
            while (stmt->execute() == SQLITE_ROW) {
                ids.emplace_back(stmt->getInt64());
                stmt->resetResIndex();
            }

            for (const auto id : ids) {
                diskCache->move_to_tail(id);
            }

            stmt = diskCache->prepare(
                "UPDATE chunks SET url = " INVALIDATED_SQL_LITERAL ", "
                "offset = -1, data_size = 0 WHERE url = ?");
            if (!stmt)
                return;
            stmt->bindText(url.c_str());
            if (stmt->execute() != SQLITE_DONE) {
                pj_log(ctx, PJ_LOG_ERROR, "%s", sqlite3_errmsg(hDB));
                return;
            }
        }

        stmt = diskCache->prepare("UPDATE properties SET lastChecked = ?, "
                                  "fileSize = ?, lastModified = ?, etag = ? "
                                  "WHERE url = ?");
        if (!stmt)
            return;
        stmt->bindInt64(props.lastChecked);
        stmt->bindInt64(props.size);
        if (props.lastModified.empty())
            stmt->bindNull();
        else
            stmt->bindText(props.lastModified.c_str());
        if (props.etag.empty())
            stmt->bindNull();
        else
            stmt->bindText(props.etag.c_str());
        stmt->bindText(url.c_str());
        if (stmt->execute() != SQLITE_DONE) {
            pj_log(ctx, PJ_LOG_ERROR, "%s", sqlite3_errmsg(hDB));
            return;
        }
    } else {
        stmt = diskCache->prepare("INSERT INTO properties (url, lastChecked, "
                                  "fileSize, lastModified, etag) VALUES "
                                  "(?,?,?,?,?)");
        if (!stmt)
            return;
        stmt->bindText(url.c_str());
        stmt->bindInt64(props.lastChecked);
        stmt->bindInt64(props.size);
        if (props.lastModified.empty())
            stmt->bindNull();
        else
            stmt->bindText(props.lastModified.c_str());
        if (props.etag.empty())
            stmt->bindNull();
        else
            stmt->bindText(props.etag.c_str());
        if (stmt->execute() != SQLITE_DONE) {
            pj_log(ctx, PJ_LOG_ERROR, "%s", sqlite3_errmsg(hDB));
            return;
        }
    }
}

// ---------------------------------------------------------------------------

bool NetworkFilePropertiesCache::tryGet(PJ_CONTEXT *ctx, const std::string &url,
                                        FileProperties &props) {
    if (cache_.tryGet(url, props)) {
        return true;
    }

    auto diskCache = DiskChunkCache::open(ctx);
    if (!diskCache)
        return false;
    auto stmt =
        diskCache->prepare("SELECT lastChecked, fileSize, lastModified, etag "
                           "FROM properties WHERE url = ?");
    if (!stmt)
        return false;
    stmt->bindText(url.c_str());
    if (stmt->execute() != SQLITE_ROW) {
        return false;
    }
    props.lastChecked = stmt->getInt64();
    props.size = stmt->getInt64();
    const char *lastModified = stmt->getText();
    props.lastModified = lastModified ? lastModified : std::string();
    const char *etag = stmt->getText();
    props.etag = etag ? etag : std::string();

    const auto ttl = pj_context_get_grid_cache_ttl(ctx);
    if (ttl > 0) {
        time_t curTime;
        time(&curTime);
        if (curTime > props.lastChecked + ttl) {
            props = FileProperties();
            return false;
        }
    }
    cache_.insert(url, props);
    return true;
}

// ---------------------------------------------------------------------------

void NetworkFilePropertiesCache::clearMemoryCache() { cache_.clear(); }

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
    FileProperties props;
    if (gNetworkChunkCache.get(ctx, filename, 0, props)) {
        return std::unique_ptr<File>(new NetworkFile(
            ctx, filename, nullptr,
            std::numeric_limits<unsigned long long>::max(), props.size));
    } else {
        std::vector<unsigned char> buffer(DOWNLOAD_CHUNK_SIZE);
        size_t size_read = 0;
        std::string errorBuffer;
        errorBuffer.resize(1024);

        auto handle = ctx->networking.open(
            ctx, filename, 0, buffer.size(), &buffer[0], &size_read,
            errorBuffer.size(), &errorBuffer[0], ctx->networking.user_data);
        buffer.resize(size_read);
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

                    props.size = filesize;

                    const char *lastModified = ctx->networking.get_header_value(
                        ctx, handle, "Last-Modified",
                        ctx->networking.user_data);
                    if (lastModified)
                        props.lastModified = lastModified;

                    const char *etag = ctx->networking.get_header_value(
                        ctx, handle, "ETag", ctx->networking.user_data);
                    if (etag)
                        props.etag = etag;

                    gNetworkFileProperties.insert(ctx, filename, props);
                }
            }
            if (filesize != 0) {
                gNetworkChunkCache.insert(ctx, filename, 0, std::move(buffer));
            }
        }

        return std::unique_ptr<File>(
            handle != nullptr && filesize != 0
                ? new NetworkFile(ctx, filename, handle, size_read, filesize)
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
        auto pChunk = gNetworkChunkCache.get(m_ctx, m_url, chunkIdxToDownload);
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
                if (gNetworkChunkCache.get(m_ctx, m_url,
                                           chunkIdxToDownload + i) != nullptr) {
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
                gNetworkChunkCache.insert(m_ctx, m_url, chunkIdxToDownload + i,
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
    std::string m_url;
    CURL *m_handle;
    std::string m_headers{};
    std::string m_lastval{};
    std::string m_useragent{};
    char m_szCurlErrBuf[CURL_ERROR_SIZE + 1] = {};

    CurlFileHandle(const CurlFileHandle &) = delete;
    CurlFileHandle &operator=(const CurlFileHandle &) = delete;

    explicit CurlFileHandle(const char *url, CURL *handle);
    ~CurlFileHandle();

    static PROJ_NETWORK_HANDLE *
    open(PJ_CONTEXT *, const char *url, unsigned long long offset,
         size_t size_to_read, void *buffer, size_t *out_size_read,
         size_t error_string_max_size, char *out_error_string, void *);
};

// ---------------------------------------------------------------------------

static std::string GetExecutableName() {
#if defined(__linux)
    std::string path;
    path.resize(1024);
    const auto ret = readlink("/proc/self/exe", &path[0], path.size());
    if (ret > 0) {
        path.resize(ret);
        const auto pos = path.rfind('/');
        if (pos != std::string::npos) {
            path = path.substr(pos + 1);
        }
        return path;
    }
#elif defined(_WIN32)
    std::string path;
    path.resize(1024);
    if (GetModuleFileNameA(nullptr, &path[0],
                           static_cast<DWORD>(path.size()))) {
        path.resize(strlen(path.c_str()));
        const auto pos = path.rfind('\\');
        if (pos != std::string::npos) {
            path = path.substr(pos + 1);
        }
        return path;
    }
#elif defined(__MACH__) && defined(__APPLE__)
    std::string path;
    path.resize(1024);
    uint32_t size = static_cast<uint32_t>(path.size());
    if (_NSGetExecutablePath(&path[0], &size) == 0) {
        path.resize(strlen(path.c_str()));
        const auto pos = path.rfind('/');
        if (pos != std::string::npos) {
            path = path.substr(pos + 1);
        }
        return path;
    }
#elif defined(__FreeBSD__)
    int mib[4];
    mib[0] = CTL_KERN;
    mib[1] = KERN_PROC;
    mib[2] = KERN_PROC_PATHNAME;
    mib[3] = -1;
    std::string path;
    path.resize(1024);
    size_t size = path.size();
    if (sysctl(mib, 4, &path[0], &size, nullptr, 0) == 0) {
        path.resize(strlen(path.c_str()));
        const auto pos = path.rfind('/');
        if (pos != std::string::npos) {
            path = path.substr(pos + 1);
        }
        return path;
    }
#endif

    return std::string();
}

// ---------------------------------------------------------------------------

CurlFileHandle::CurlFileHandle(const char *url, CURL *handle)
    : m_url(url), m_handle(handle) {
    curl_easy_setopt(handle, CURLOPT_URL, m_url.c_str());

    if (getenv("PROJ_CURL_VERBOSE"))
        curl_easy_setopt(handle, CURLOPT_VERBOSE, 1);

// CURLOPT_SUPPRESS_CONNECT_HEADERS is defined in curl 7.54.0 or newer.
#if LIBCURL_VERSION_NUM >= 0x073600
    curl_easy_setopt(handle, CURLOPT_SUPPRESS_CONNECT_HEADERS, 1L);
#endif

    // Enable following redirections.  Requires libcurl 7.10.1 at least.
    curl_easy_setopt(handle, CURLOPT_FOLLOWLOCATION, 1);
    curl_easy_setopt(handle, CURLOPT_MAXREDIRS, 10);

    if (getenv("PROJ_UNSAFE_SSL")) {
        curl_easy_setopt(handle, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(handle, CURLOPT_SSL_VERIFYHOST, 0L);
    }

    curl_easy_setopt(handle, CURLOPT_ERRORBUFFER, m_szCurlErrBuf);

    if (getenv("PROJ_NO_USERAGENT") == nullptr) {
        m_useragent = "PROJ " STR(PROJ_VERSION_MAJOR) "." STR(
            PROJ_VERSION_MINOR) "." STR(PROJ_VERSION_PATCH);
        const auto exeName = GetExecutableName();
        if (!exeName.empty()) {
            m_useragent = exeName + " using " + m_useragent;
        }
        curl_easy_setopt(handle, CURLOPT_USERAGENT, m_useragent.data());
    }
}

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

static double GetNewRetryDelay(int response_code, double dfOldDelay,
                               const char *pszErrBuf,
                               const char *pszCurlError) {
    if (response_code == 429 || response_code == 500 ||
        (response_code >= 502 && response_code <= 504) ||
        // S3 sends some client timeout errors as 400 Client Error
        (response_code == 400 && pszErrBuf &&
         strstr(pszErrBuf, "RequestTimeout")) ||
        (pszCurlError && strstr(pszCurlError, "Connection timed out"))) {
        // Use an exponential backoff factor of 2 plus some random jitter
        // We don't care about cryptographic quality randomness, hence:
        // coverity[dont_call]
        return dfOldDelay * (2 + rand() * 0.5 / RAND_MAX);
    } else {
        return 0;
    }
}

// ---------------------------------------------------------------------------

constexpr double MIN_RETRY_DELAY_MS = 500;
constexpr double MAX_RETRY_DELAY_MS = 60000;

PROJ_NETWORK_HANDLE *CurlFileHandle::open(PJ_CONTEXT *ctx, const char *url,
                                          unsigned long long offset,
                                          size_t size_to_read, void *buffer,
                                          size_t *out_size_read,
                                          size_t error_string_max_size,
                                          char *out_error_string, void *) {
    CURL *hCurlHandle = curl_easy_init();
    if (!hCurlHandle)
        return nullptr;

    auto file =
        std::unique_ptr<CurlFileHandle>(new CurlFileHandle(url, hCurlHandle));

    double oldDelay = MIN_RETRY_DELAY_MS;
    std::string headers;
    std::string body;

    char szBuffer[128];
    sqlite3_snprintf(sizeof(szBuffer), szBuffer, "%llu-%llu", offset,
                     offset + size_to_read - 1);

    while (true) {
        curl_easy_setopt(hCurlHandle, CURLOPT_RANGE, szBuffer);

        headers.clear();
        headers.reserve(16 * 1024);
        curl_easy_setopt(hCurlHandle, CURLOPT_HEADERDATA, &headers);
        curl_easy_setopt(hCurlHandle, CURLOPT_HEADERFUNCTION,
                         pj_curl_write_func);

        body.clear();
        body.reserve(size_to_read);
        curl_easy_setopt(hCurlHandle, CURLOPT_WRITEDATA, &body);
        curl_easy_setopt(hCurlHandle, CURLOPT_WRITEFUNCTION,
                         pj_curl_write_func);

        file->m_szCurlErrBuf[0] = '\0';

        curl_easy_perform(hCurlHandle);

        long response_code = 0;
        curl_easy_getinfo(hCurlHandle, CURLINFO_HTTP_CODE, &response_code);

        curl_easy_setopt(hCurlHandle, CURLOPT_HEADERDATA, nullptr);
        curl_easy_setopt(hCurlHandle, CURLOPT_HEADERFUNCTION, nullptr);

        curl_easy_setopt(hCurlHandle, CURLOPT_WRITEDATA, nullptr);
        curl_easy_setopt(hCurlHandle, CURLOPT_WRITEFUNCTION, nullptr);

        if (response_code == 0 || response_code >= 300) {
            const double delay =
                GetNewRetryDelay(static_cast<int>(response_code), oldDelay,
                                 body.c_str(), file->m_szCurlErrBuf);
            if (delay != 0 && delay < MAX_RETRY_DELAY_MS) {
                pj_log(ctx, PJ_LOG_TRACE,
                       "Got a HTTP %ld error. Retrying in %d ms", response_code,
                       static_cast<int>(delay));
                proj_sleep_ms(static_cast<int>(delay));
                oldDelay = delay;
            } else {
                if (out_error_string) {
                    if (file->m_szCurlErrBuf[0]) {
                        snprintf(out_error_string, error_string_max_size, "%s",
                                 file->m_szCurlErrBuf);
                    } else {
                        snprintf(out_error_string, error_string_max_size,
                                 "HTTP error %ld: %s", response_code,
                                 body.c_str());
                    }
                }
                return nullptr;
            }
        } else {
            break;
        }
    }

    if (out_error_string && error_string_max_size) {
        out_error_string[0] = '\0';
    }

    if (!body.empty()) {
        memcpy(buffer, body.data(), std::min(size_to_read, body.size()));
    }
    *out_size_read = std::min(size_to_read, body.size());

    file->m_headers = std::move(headers);
    return reinterpret_cast<PROJ_NETWORK_HANDLE *>(file.release());
}

// ---------------------------------------------------------------------------

static void pj_curl_close(PJ_CONTEXT *, PROJ_NETWORK_HANDLE *handle,
                          void * /*user_data*/) {
    delete reinterpret_cast<CurlFileHandle *>(handle);
}

// ---------------------------------------------------------------------------

static size_t pj_curl_read_range(PJ_CONTEXT *ctx,
                                 PROJ_NETWORK_HANDLE *raw_handle,
                                 unsigned long long offset, size_t size_to_read,
                                 void *buffer, size_t error_string_max_size,
                                 char *out_error_string, void *) {
    auto handle = reinterpret_cast<CurlFileHandle *>(raw_handle);
    auto hCurlHandle = handle->m_handle;

    double oldDelay = MIN_RETRY_DELAY_MS;
    std::string body;

    char szBuffer[128];
    sqlite3_snprintf(sizeof(szBuffer), szBuffer, "%llu-%llu", offset,
                     offset + size_to_read - 1);

    while (true) {
        curl_easy_setopt(hCurlHandle, CURLOPT_RANGE, szBuffer);

        body.clear();
        body.reserve(size_to_read);
        curl_easy_setopt(hCurlHandle, CURLOPT_WRITEDATA, &body);
        curl_easy_setopt(hCurlHandle, CURLOPT_WRITEFUNCTION,
                         pj_curl_write_func);

        handle->m_szCurlErrBuf[0] = '\0';

        curl_easy_perform(hCurlHandle);

        long response_code = 0;
        curl_easy_getinfo(hCurlHandle, CURLINFO_HTTP_CODE, &response_code);

        curl_easy_setopt(hCurlHandle, CURLOPT_WRITEDATA, nullptr);
        curl_easy_setopt(hCurlHandle, CURLOPT_WRITEFUNCTION, nullptr);

        if (response_code == 0 || response_code >= 300) {
            const double delay =
                GetNewRetryDelay(static_cast<int>(response_code), oldDelay,
                                 body.c_str(), handle->m_szCurlErrBuf);
            if (delay != 0 && delay < MAX_RETRY_DELAY_MS) {
                pj_log(ctx, PJ_LOG_TRACE,
                       "Got a HTTP %ld error. Retrying in %d ms", response_code,
                       static_cast<int>(delay));
                proj_sleep_ms(static_cast<int>(delay));
                oldDelay = delay;
            } else {
                if (out_error_string) {
                    if (handle->m_szCurlErrBuf[0]) {
                        snprintf(out_error_string, error_string_max_size, "%s",
                                 handle->m_szCurlErrBuf);
                    } else {
                        snprintf(out_error_string, error_string_max_size,
                                 "HTTP error %ld: %s", response_code,
                                 body.c_str());
                    }
                }
                return 0;
            }
        } else {
            break;
        }
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
    pos += strlen(header_name);
    const char *c_str = handle->m_headers.c_str();
    if (c_str[pos] == ':')
        pos++;
    while (c_str[pos] == ' ')
        pos++;
    auto posEnd = pos;
    while (c_str[posEnd] != '\r' && c_str[posEnd] != '\n' &&
           c_str[posEnd] != '\0')
        posEnd++;
    handle->m_lastval = handle->m_headers.substr(pos, posEnd - pos);
    return handle->m_lastval.c_str();
}

#else

// ---------------------------------------------------------------------------

static PROJ_NETWORK_HANDLE *
no_op_network_open(PJ_CONTEXT *, const char * /* url */,
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
    ctx->networking.open = CurlFileHandle::open;
    ctx->networking.close = pj_curl_close;
    ctx->networking.read_range = pj_curl_read_range;
    ctx->networking.get_header_value = pj_curl_get_header_value;
#else
    ctx->networking.open = no_op_network_open;
    ctx->networking.close = no_op_network_close;
#endif
}

// ---------------------------------------------------------------------------

void FileManager::clearMemoryCache() {
    gNetworkChunkCache.clearMemoryCache();
    gNetworkFileProperties.clearMemoryCache();
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

/** Enable or disable the local cache of grid chunks
*
* This overrides the setting in the PROJ configuration file.
*
* @param ctx PROJ context, or NULL
* @param enabled TRUE if the cache is enabled.
*/
void proj_grid_cache_set_enable(PJ_CONTEXT *ctx, int enabled) {
    if (ctx == nullptr) {
        ctx = pj_get_default_ctx();
    }
    // Load ini file, now so as to override its settings
    pj_load_ini(ctx);
    ctx->gridChunkCache.enabled = enabled != FALSE;
}

// ---------------------------------------------------------------------------

/** Override, for the considered context, the path and file of the local
* cache of grid chunks.
*
* @param ctx PROJ context, or NULL
* @param fullname Full name to the cache (encoded in UTF-8). If set to NULL,
*                 caching will be disabled.
*/
void proj_grid_cache_set_filename(PJ_CONTEXT *ctx, const char *fullname) {
    if (ctx == nullptr) {
        ctx = pj_get_default_ctx();
    }
    // Load ini file, now so as to override its settings
    pj_load_ini(ctx);
    ctx->gridChunkCache.filename = fullname ? fullname : std::string();
}

// ---------------------------------------------------------------------------

/** Override, for the considered context, the maximum size of the local
* cache of grid chunks.
*
* @param ctx PROJ context, or NULL
* @param max_size_MB Maximum size, in mega-bytes (1024*1024 bytes), or
*                    negative value to set unlimited size.
*/
void proj_grid_cache_set_max_size(PJ_CONTEXT *ctx, int max_size_MB) {
    if (ctx == nullptr) {
        ctx = pj_get_default_ctx();
    }
    // Load ini file, now so as to override its settings
    pj_load_ini(ctx);
    ctx->gridChunkCache.max_size =
        max_size_MB < 0 ? -1
                        : static_cast<long long>(max_size_MB) * 1024 * 1024;
    if (max_size_MB == 0) {
        // For debug purposes only
        const char *env_var = getenv("PROJ_GRID_CACHE_MAX_SIZE_BYTES");
        if (env_var && env_var[0] != '\0') {
            ctx->gridChunkCache.max_size = atoi(env_var);
        }
    }
}

// ---------------------------------------------------------------------------

/** Override, for the considered context, the time-to-live delay for
* re-checking if the cached properties of files are still up-to-date.
*
* @param ctx PROJ context, or NULL
* @param ttl_seconds Delay in seconds. Use negative value for no expiration.
*/
void proj_grid_cache_set_ttl(PJ_CONTEXT *ctx, int ttl_seconds) {
    if (ctx == nullptr) {
        ctx = pj_get_default_ctx();
    }
    // Load ini file, now so as to override its settings
    pj_load_ini(ctx);
    ctx->gridChunkCache.ttl = ttl_seconds;
}

// ---------------------------------------------------------------------------

/** Clear the local cache of grid chunks.
*
* @param ctx PROJ context, or NULL
*/
void proj_grid_cache_clear(PJ_CONTEXT *ctx) {
    if (ctx == nullptr) {
        ctx = pj_get_default_ctx();
    }
    NS_PROJ::gNetworkChunkCache.clearDiskChunkCache(ctx);
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

// ---------------------------------------------------------------------------

#ifdef _WIN32

static std::wstring UTF8ToWString(const std::string &str) {
    using convert_typeX = std::codecvt_utf8<wchar_t>;
    std::wstring_convert<convert_typeX, wchar_t> converterX;

    return converterX.from_bytes(str);
}

// ---------------------------------------------------------------------------

static std::string WStringToUTF8(const std::wstring &wstr) {
    using convert_typeX = std::codecvt_utf8<wchar_t>;
    std::wstring_convert<convert_typeX, wchar_t> converterX;

    return converterX.to_bytes(wstr);
}
#endif

// ---------------------------------------------------------------------------

static void CreateDirectory(const std::string &path) {
#ifdef _WIN32
    struct __stat64 buf;
    const auto wpath = UTF8ToWString(path);
    if (_wstat64(wpath.c_str(), &buf) == 0)
        return;
    auto pos = path.find_last_of("/\\");
    if (pos == 0 || pos == std::string::npos)
        return;
    CreateDirectory(path.substr(0, pos));
    _wmkdir(wpath.c_str());
#else
    struct stat buf;
    if (stat(path.c_str(), &buf) == 0)
        return;
    auto pos = path.find_last_of("/\\");
    if (pos == 0 || pos == std::string::npos)
        return;
    CreateDirectory(path.substr(0, pos));
    mkdir(path.c_str(), 0755);
#endif
}

// ---------------------------------------------------------------------------

std::string pj_context_get_grid_cache_filename(PJ_CONTEXT *ctx) {
    pj_load_ini(ctx);
    if (!ctx->gridChunkCache.filename.empty()) {
        return ctx->gridChunkCache.filename;
    }
    std::string path;
#ifdef _WIN32
    std::wstring wPath;
    wPath.resize(MAX_PATH);
    if (SHGetFolderPathW(nullptr, CSIDL_LOCAL_APPDATA, nullptr, 0, &wPath[0]) ==
        S_OK) {
        wPath.resize(wcslen(wPath.data()));
        path = WStringToUTF8(wPath);
    } else {
        const char *local_app_data = getenv("LOCALAPPDATA");
        if (!local_app_data) {
            local_app_data = getenv("TEMP");
            if (!local_app_data) {
                local_app_data = "c:/users";
            }
        }
        path = local_app_data;
    }
#else
    const char *xdg_data_home = getenv("XDG_DATA_HOME");
    if (xdg_data_home != nullptr) {
        path = xdg_data_home;
    } else {
        const char *home = getenv("HOME");
        if (home) {
#if defined(__MACH__) && defined(__APPLE__)
            path = std::string(home) + "/Library/Logs";
#else
            path = std::string(home) + "/.local/share";
#endif
        } else {
            path = "/tmp";
        }
    }
#endif
    path += "/proj";
    CreateDirectory(path);
    ctx->gridChunkCache.filename = path + "/cache.db";
    return ctx->gridChunkCache.filename;
}

//! @endcond
