/******************************************************************************
 *
 * Project:  PROJ
 * Purpose:  ISO19111:2019 implementation
 * Author:   Even Rouault <even dot rouault at spatialys dot com>
 *
 ******************************************************************************
 * Copyright (c) 2018, Even Rouault <even dot rouault at spatialys dot com>
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
 ****************************************************************************/

#ifndef FROM_PROJ_CPP
#define FROM_PROJ_CPP
#endif

#include "proj/common.hpp"
#include "proj/coordinateoperation.hpp"
#include "proj/coordinatesystem.hpp"
#include "proj/crs.hpp"
#include "proj/datum.hpp"
#include "proj/io.hpp"
#include "proj/metadata.hpp"
#include "proj/util.hpp"

#include "proj/internal/coordinateoperation_internal.hpp"
#include "proj/internal/internal.hpp"
#include "proj/internal/io_internal.hpp"
#include "proj/internal/lru_cache.hpp"

#include <cmath>
#include <cstdlib>
#include <cstring>
#include <iomanip>
#include <limits>
#include <map>
#include <memory>
#include <sstream> // std::ostringstream
#include <string>

#include "proj_constants.h"

// PROJ include order is sensitive
// clang-format off
#include "proj.h"
#include "proj_internal.h"
#include "proj_api.h"
// clang-format on

#include <sqlite3.h>

// Custom SQLite VFS as our database is not supposed to be modified in
// parallel. This is slightly faster
#define ENABLE_CUSTOM_LOCKLESS_VFS

using namespace NS_PROJ::internal;
using namespace NS_PROJ::common;

NS_PROJ_START
namespace io {

//! @cond Doxygen_Suppress

// CRS subtypes
#define GEOG_2D "geographic 2D"
#define GEOG_3D "geographic 3D"
#define GEOCENTRIC "geocentric"
#define PROJECTED "projected"
#define VERTICAL "vertical"
#define COMPOUND "compound"

#define GEOG_2D_SINGLE_QUOTED "'geographic 2D'"
#define GEOG_3D_SINGLE_QUOTED "'geographic 3D'"
#define GEOCENTRIC_SINGLE_QUOTED "'geocentric'"

// ---------------------------------------------------------------------------

struct SQLValues {
    enum class Type { STRING, DOUBLE };

    // cppcheck-suppress noExplicitConstructor
    SQLValues(const std::string &value) : type_(Type::STRING), str_(value) {}

    // cppcheck-suppress noExplicitConstructor
    SQLValues(double value) : type_(Type::DOUBLE), double_(value) {}

    const Type &type() const { return type_; }

    // cppcheck-suppress functionStatic
    const std::string &stringValue() const { return str_; }

    // cppcheck-suppress functionStatic
    double doubleValue() const { return double_; }

  private:
    Type type_;
    std::string str_{};
    double double_ = 0.0;
};

// ---------------------------------------------------------------------------

using SQLRow = std::vector<std::string>;
using SQLResultSet = std::list<SQLRow>;
using ListOfParams = std::list<SQLValues>;

// ---------------------------------------------------------------------------

struct DatabaseContext::Private {
    Private();
    ~Private();

    void open(const std::string &databasePath, PJ_CONTEXT *ctx);
    void setHandle(sqlite3 *sqlite_handle);

    sqlite3 *handle() const { return sqlite_handle_; }

    PJ_CONTEXT *pjCtxt() const { return pjCtxt_; }
    void setPjCtxt(PJ_CONTEXT *ctxt) { pjCtxt_ = ctxt; }

    SQLResultSet run(const std::string &sql,
                     const ListOfParams &parameters = ListOfParams());

    std::vector<std::string> getDatabaseStructure();

    // cppcheck-suppress functionStatic
    const std::string &getPath() const { return databasePath_; }

    void attachExtraDatabases(
        const std::vector<std::string> &auxiliaryDatabasePaths);

    // Mechanism to detect recursion in calls from
    // AuthorityFactory::createXXX() -> createFromUserInput() ->
    // AuthorityFactory::createXXX()
    struct RecursionDetector {
        explicit RecursionDetector(const DatabaseContextNNPtr &context)
            : dbContext_(context) {
            if (dbContext_->getPrivate()->recLevel_ == 2) {
                // Throw exception before incrementing, since the destructor
                // will not be called
                throw FactoryException("Too many recursive calls");
            }
            ++dbContext_->getPrivate()->recLevel_;
        }

        ~RecursionDetector() { --dbContext_->getPrivate()->recLevel_; }

      private:
        DatabaseContextNNPtr dbContext_;
    };

    std::map<std::string, std::list<SQLRow>> &getMapCanonicalizeGRFName() {
        return mapCanonicalizeGRFName_;
    }

    // cppcheck-suppress functionStatic
    common::UnitOfMeasurePtr getUOMFromCache(const std::string &code);
    // cppcheck-suppress functionStatic
    void cache(const std::string &code, const common::UnitOfMeasureNNPtr &uom);

    // cppcheck-suppress functionStatic
    crs::CRSPtr getCRSFromCache(const std::string &code);
    // cppcheck-suppress functionStatic
    void cache(const std::string &code, const crs::CRSNNPtr &crs);

    datum::GeodeticReferenceFramePtr
        // cppcheck-suppress functionStatic
        getGeodeticDatumFromCache(const std::string &code);
    // cppcheck-suppress functionStatic
    void cache(const std::string &code,
               const datum::GeodeticReferenceFrameNNPtr &datum);

    datum::PrimeMeridianPtr
        // cppcheck-suppress functionStatic
        getPrimeMeridianFromCache(const std::string &code);
    // cppcheck-suppress functionStatic
    void cache(const std::string &code, const datum::PrimeMeridianNNPtr &pm);

    // cppcheck-suppress functionStatic
    cs::CoordinateSystemPtr
    getCoordinateSystemFromCache(const std::string &code);
    // cppcheck-suppress functionStatic
    void cache(const std::string &code, const cs::CoordinateSystemNNPtr &cs);

    // cppcheck-suppress functionStatic
    metadata::ExtentPtr getExtentFromCache(const std::string &code);
    // cppcheck-suppress functionStatic
    void cache(const std::string &code, const metadata::ExtentNNPtr &extent);

    // cppcheck-suppress functionStatic
    bool getCRSToCRSCoordOpFromCache(
        const std::string &code,
        std::vector<operation::CoordinateOperationNNPtr> &list);
    // cppcheck-suppress functionStatic
    void cache(const std::string &code,
               const std::vector<operation::CoordinateOperationNNPtr> &list);

    struct GridInfoCache {
        std::string fullFilename{};
        std::string packageName{};
        std::string url{};
        bool found = false;
        bool directDownload = false;
        bool openLicense = false;
        bool gridAvailable = false;
    };

    // cppcheck-suppress functionStatic
    bool getGridInfoFromCache(const std::string &code, GridInfoCache &info);
    // cppcheck-suppress functionStatic
    void cache(const std::string &code, const GridInfoCache &info);

  private:
    friend class DatabaseContext;

    std::string databasePath_{};
    bool close_handle_ = true;
    sqlite3 *sqlite_handle_{};
    std::map<std::string, sqlite3_stmt *> mapSqlToStatement_{};
    PJ_CONTEXT *pjCtxt_ = nullptr;
    int recLevel_ = 0;
    bool detach_ = false;
    std::string lastMetadataValue_{};
    std::map<std::string, std::list<SQLRow>> mapCanonicalizeGRFName_{};

    using LRUCacheOfObjects = lru11::Cache<std::string, util::BaseObjectPtr>;

    static constexpr size_t CACHE_SIZE = 128;
    LRUCacheOfObjects cacheUOM_{CACHE_SIZE};
    LRUCacheOfObjects cacheCRS_{CACHE_SIZE};
    LRUCacheOfObjects cacheGeodeticDatum_{CACHE_SIZE};
    LRUCacheOfObjects cachePrimeMeridian_{CACHE_SIZE};
    LRUCacheOfObjects cacheCS_{CACHE_SIZE};
    LRUCacheOfObjects cacheExtent_{CACHE_SIZE};
    lru11::Cache<std::string, std::vector<operation::CoordinateOperationNNPtr>>
        cacheCRSToCrsCoordOp_{CACHE_SIZE};
    lru11::Cache<std::string, GridInfoCache> cacheGridInfo_{CACHE_SIZE};

    static void insertIntoCache(LRUCacheOfObjects &cache,
                                const std::string &code,
                                const util::BaseObjectPtr &obj);

    static void getFromCache(LRUCacheOfObjects &cache, const std::string &code,
                             util::BaseObjectPtr &obj);

    void closeDB() noexcept;

    // cppcheck-suppress functionStatic
    void registerFunctions();

#ifdef ENABLE_CUSTOM_LOCKLESS_VFS
    std::string thisNamePtr_{};
    sqlite3_vfs *vfs_{};
    bool createCustomVFS();
#endif

    Private(const Private &) = delete;
    Private &operator=(const Private &) = delete;
};

// ---------------------------------------------------------------------------

DatabaseContext::Private::Private() = default;

// ---------------------------------------------------------------------------

DatabaseContext::Private::~Private() {
    assert(recLevel_ == 0);

    closeDB();

#ifdef ENABLE_CUSTOM_LOCKLESS_VFS
    if (vfs_) {
        sqlite3_vfs_unregister(vfs_);
        delete vfs_;
    }
#endif
}

// ---------------------------------------------------------------------------

void DatabaseContext::Private::closeDB() noexcept {

    if (detach_) {
        // Workaround a bug visible in SQLite 3.8.1 and 3.8.2 that causes
        // a crash in TEST(factory, attachExtraDatabases_auxiliary)
        // due to possible wrong caching of key info.
        // The bug is specific to using a memory file with shared cache as an
        // auxiliary DB.
        // The efinitive fix was likely in 3.8.8
        // https://github.com/mackyle/sqlite/commit/d412d4b8731991ecbd8811874aa463d0821673eb
        // But just after 3.8.2,
        // https://github.com/mackyle/sqlite/commit/ccf328c4318eacedab9ed08c404bc4f402dcad19
        // also seemed to hide the issue.
        // Detaching a database hides the issue, not sure if it is by chance...
        try {
            run("DETACH DATABASE db_0");
        } catch (...) {
        }
        detach_ = false;
    }

    for (auto &pair : mapSqlToStatement_) {
        sqlite3_finalize(pair.second);
    }
    mapSqlToStatement_.clear();

    if (close_handle_ && sqlite_handle_ != nullptr) {
        sqlite3_close(sqlite_handle_);
        sqlite_handle_ = nullptr;
    }
}

// ---------------------------------------------------------------------------

void DatabaseContext::Private::insertIntoCache(LRUCacheOfObjects &cache,
                                               const std::string &code,
                                               const util::BaseObjectPtr &obj) {
    cache.insert(code, obj);
}

// ---------------------------------------------------------------------------

void DatabaseContext::Private::getFromCache(LRUCacheOfObjects &cache,
                                            const std::string &code,
                                            util::BaseObjectPtr &obj) {
    cache.tryGet(code, obj);
}

// ---------------------------------------------------------------------------

bool DatabaseContext::Private::getCRSToCRSCoordOpFromCache(
    const std::string &code,
    std::vector<operation::CoordinateOperationNNPtr> &list) {
    return cacheCRSToCrsCoordOp_.tryGet(code, list);
}

// ---------------------------------------------------------------------------

void DatabaseContext::Private::cache(
    const std::string &code,
    const std::vector<operation::CoordinateOperationNNPtr> &list) {
    cacheCRSToCrsCoordOp_.insert(code, list);
}

// ---------------------------------------------------------------------------

crs::CRSPtr DatabaseContext::Private::getCRSFromCache(const std::string &code) {
    util::BaseObjectPtr obj;
    getFromCache(cacheCRS_, code, obj);
    return std::static_pointer_cast<crs::CRS>(obj);
}

// ---------------------------------------------------------------------------

void DatabaseContext::Private::cache(const std::string &code,
                                     const crs::CRSNNPtr &crs) {
    insertIntoCache(cacheCRS_, code, crs.as_nullable());
}

// ---------------------------------------------------------------------------

common::UnitOfMeasurePtr
DatabaseContext::Private::getUOMFromCache(const std::string &code) {
    util::BaseObjectPtr obj;
    getFromCache(cacheUOM_, code, obj);
    return std::static_pointer_cast<common::UnitOfMeasure>(obj);
}

// ---------------------------------------------------------------------------

void DatabaseContext::Private::cache(const std::string &code,
                                     const common::UnitOfMeasureNNPtr &uom) {
    insertIntoCache(cacheUOM_, code, uom.as_nullable());
}

// ---------------------------------------------------------------------------

datum::GeodeticReferenceFramePtr
DatabaseContext::Private::getGeodeticDatumFromCache(const std::string &code) {
    util::BaseObjectPtr obj;
    getFromCache(cacheGeodeticDatum_, code, obj);
    return std::static_pointer_cast<datum::GeodeticReferenceFrame>(obj);
}

// ---------------------------------------------------------------------------

void DatabaseContext::Private::cache(
    const std::string &code, const datum::GeodeticReferenceFrameNNPtr &datum) {
    insertIntoCache(cacheGeodeticDatum_, code, datum.as_nullable());
}

// ---------------------------------------------------------------------------

datum::PrimeMeridianPtr
DatabaseContext::Private::getPrimeMeridianFromCache(const std::string &code) {
    util::BaseObjectPtr obj;
    getFromCache(cachePrimeMeridian_, code, obj);
    return std::static_pointer_cast<datum::PrimeMeridian>(obj);
}

// ---------------------------------------------------------------------------

void DatabaseContext::Private::cache(const std::string &code,
                                     const datum::PrimeMeridianNNPtr &pm) {
    insertIntoCache(cachePrimeMeridian_, code, pm.as_nullable());
}

// ---------------------------------------------------------------------------

cs::CoordinateSystemPtr DatabaseContext::Private::getCoordinateSystemFromCache(
    const std::string &code) {
    util::BaseObjectPtr obj;
    getFromCache(cacheCS_, code, obj);
    return std::static_pointer_cast<cs::CoordinateSystem>(obj);
}

// ---------------------------------------------------------------------------

void DatabaseContext::Private::cache(const std::string &code,
                                     const cs::CoordinateSystemNNPtr &cs) {
    insertIntoCache(cacheCS_, code, cs.as_nullable());
}

// ---------------------------------------------------------------------------

metadata::ExtentPtr
DatabaseContext::Private::getExtentFromCache(const std::string &code) {
    util::BaseObjectPtr obj;
    getFromCache(cacheExtent_, code, obj);
    return std::static_pointer_cast<metadata::Extent>(obj);
}

// ---------------------------------------------------------------------------

void DatabaseContext::Private::cache(const std::string &code,
                                     const metadata::ExtentNNPtr &extent) {
    insertIntoCache(cacheExtent_, code, extent.as_nullable());
}

// ---------------------------------------------------------------------------

bool DatabaseContext::Private::getGridInfoFromCache(const std::string &code,
                                                    GridInfoCache &info) {
    return cacheGridInfo_.tryGet(code, info);
}

// ---------------------------------------------------------------------------

void DatabaseContext::Private::cache(const std::string &code,
                                     const GridInfoCache &info) {
    cacheGridInfo_.insert(code, info);
}

// ---------------------------------------------------------------------------

#ifdef ENABLE_CUSTOM_LOCKLESS_VFS

typedef int (*ClosePtr)(sqlite3_file *);

static int VFSClose(sqlite3_file *file) {
    sqlite3_vfs *defaultVFS = sqlite3_vfs_find(nullptr);
    assert(defaultVFS);
    ClosePtr defaultClosePtr;
    std::memcpy(&defaultClosePtr,
                reinterpret_cast<char *>(file) + defaultVFS->szOsFile,
                sizeof(ClosePtr));
    void *methods = const_cast<sqlite3_io_methods *>(file->pMethods);
    int ret = defaultClosePtr(file);
    std::free(methods);
    return ret;
}

// No-lock implementation
static int VSFLock(sqlite3_file *, int) { return SQLITE_OK; }

static int VSFUnlock(sqlite3_file *, int) { return SQLITE_OK; }

static int VFSOpen(sqlite3_vfs *vfs, const char *name, sqlite3_file *file,
                   int flags, int *outFlags) {
    sqlite3_vfs *defaultVFS = static_cast<sqlite3_vfs *>(vfs->pAppData);
    int ret = defaultVFS->xOpen(defaultVFS, name, file, flags, outFlags);
    if (ret == SQLITE_OK) {
        ClosePtr defaultClosePtr = file->pMethods->xClose;
        assert(defaultClosePtr);
        sqlite3_io_methods *methods = static_cast<sqlite3_io_methods *>(
            std::malloc(sizeof(sqlite3_io_methods)));
        if (!methods) {
            file->pMethods->xClose(file);
            return SQLITE_NOMEM;
        }
        memcpy(methods, file->pMethods, sizeof(sqlite3_io_methods));
        methods->xClose = VFSClose;
        methods->xLock = VSFLock;
        methods->xUnlock = VSFUnlock;
        file->pMethods = methods;
        // Save original xClose pointer at end of file structure
        std::memcpy(reinterpret_cast<char *>(file) + defaultVFS->szOsFile,
                    &defaultClosePtr, sizeof(ClosePtr));
    }
    return ret;
}

static int VFSAccess(sqlite3_vfs *vfs, const char *zName, int flags,
                     int *pResOut) {
    sqlite3_vfs *defaultVFS = static_cast<sqlite3_vfs *>(vfs->pAppData);
    // Do not bother stat'ing for journal or wal files
    if (std::strstr(zName, "-journal") || std::strstr(zName, "-wal")) {
        *pResOut = false;
        return SQLITE_OK;
    }
    return defaultVFS->xAccess(defaultVFS, zName, flags, pResOut);
}

// ---------------------------------------------------------------------------

bool DatabaseContext::Private::createCustomVFS() {

    sqlite3_vfs *defaultVFS = sqlite3_vfs_find(nullptr);
    assert(defaultVFS);

    std::ostringstream buffer;
    buffer << this;
    thisNamePtr_ = buffer.str();

    vfs_ = new sqlite3_vfs();
    vfs_->iVersion = 1;
    vfs_->szOsFile = defaultVFS->szOsFile + sizeof(ClosePtr);
    vfs_->mxPathname = defaultVFS->mxPathname;
    vfs_->zName = thisNamePtr_.c_str();
    vfs_->pAppData = defaultVFS;
    vfs_->xOpen = VFSOpen;
    vfs_->xDelete = defaultVFS->xDelete;
    vfs_->xAccess = VFSAccess;
    vfs_->xFullPathname = defaultVFS->xFullPathname;
    vfs_->xDlOpen = defaultVFS->xDlOpen;
    vfs_->xDlError = defaultVFS->xDlError;
    vfs_->xDlSym = defaultVFS->xDlSym;
    vfs_->xDlClose = defaultVFS->xDlClose;
    vfs_->xRandomness = defaultVFS->xRandomness;
    vfs_->xSleep = defaultVFS->xSleep;
    vfs_->xCurrentTime = defaultVFS->xCurrentTime;
    vfs_->xGetLastError = defaultVFS->xGetLastError;
    vfs_->xCurrentTimeInt64 = defaultVFS->xCurrentTimeInt64;
    return sqlite3_vfs_register(vfs_, false) == SQLITE_OK;
}

#endif // ENABLE_CUSTOM_LOCKLESS_VFS

// ---------------------------------------------------------------------------

void DatabaseContext::Private::open(const std::string &databasePath,
                                    PJ_CONTEXT *ctx) {
    setPjCtxt(ctx ? ctx : pj_get_default_ctx());
    std::string path(databasePath);
    if (path.empty()) {
        path.resize(2048);
        const bool found =
            pj_find_file(pjCtxt(), "proj.db", &path[0], path.size() - 1) != 0;
        path.resize(strlen(path.c_str()));
        if (!found) {
            throw FactoryException("Cannot find proj.db");
        }
    }

    if (
#ifdef ENABLE_CUSTOM_LOCKLESS_VFS
        !createCustomVFS() ||
#endif
        sqlite3_open_v2(path.c_str(), &sqlite_handle_,
                        SQLITE_OPEN_READONLY | SQLITE_OPEN_NOMUTEX,
#ifdef ENABLE_CUSTOM_LOCKLESS_VFS
                        thisNamePtr_.c_str()
#else
                        nullptr
#endif
                            ) != SQLITE_OK ||
        !sqlite_handle_) {
        throw FactoryException("Open of " + path + " failed");
    }

    databasePath_ = path;
    registerFunctions();
}

// ---------------------------------------------------------------------------

void DatabaseContext::Private::setHandle(sqlite3 *sqlite_handle) {

    assert(sqlite_handle);
    assert(!sqlite_handle_);
    sqlite_handle_ = sqlite_handle;
    close_handle_ = false;

    registerFunctions();
}

// ---------------------------------------------------------------------------

std::vector<std::string> DatabaseContext::Private::getDatabaseStructure() {
    const char *sqls[] = {
        "SELECT sql FROM sqlite_master WHERE type = 'table'",
        "SELECT sql FROM sqlite_master WHERE type = 'view'",
        "SELECT sql FROM sqlite_master WHERE type = 'trigger'"};
    std::vector<std::string> res;
    for (const auto &sql : sqls) {
        auto sqlRes = run(sql);
        for (const auto &row : sqlRes) {
            res.emplace_back(row[0]);
        }
    }
    return res;
}

// ---------------------------------------------------------------------------

void DatabaseContext::Private::attachExtraDatabases(
    const std::vector<std::string> &auxiliaryDatabasePaths) {
    assert(close_handle_);
    assert(sqlite_handle_);

    auto tables =
        run("SELECT name FROM sqlite_master WHERE type IN ('table', 'view')");
    std::map<std::string, std::vector<std::string>> tableStructure;
    for (const auto &rowTable : tables) {
        auto tableName = rowTable[0];
        auto tableInfo = run("PRAGMA table_info(\"" +
                             replaceAll(tableName, "\"", "\"\"") + "\")");
        for (const auto &rowCol : tableInfo) {
            const auto &colName = rowCol[1];
            tableStructure[tableName].push_back(colName);
        }
    }

    closeDB();

    sqlite3_open_v2(":memory:", &sqlite_handle_,
                    SQLITE_OPEN_READWRITE | SQLITE_OPEN_NOMUTEX
#ifdef SQLITE_OPEN_URI
                        | SQLITE_OPEN_URI
#endif
                    ,
                    nullptr);
    if (!sqlite_handle_) {
        throw FactoryException("cannot create in memory database");
    }

    run("ATTACH DATABASE '" + replaceAll(databasePath_, "'", "''") +
        "' AS db_0");
    detach_ = true;
    int count = 1;
    for (const auto &otherDb : auxiliaryDatabasePaths) {
        std::string sql = "ATTACH DATABASE '";
        sql += replaceAll(otherDb, "'", "''");
        sql += "' AS db_";
        sql += toString(static_cast<int>(count));
        count++;
        run(sql);
    }

    for (const auto &pair : tableStructure) {
        std::string sql("CREATE TEMP VIEW ");
        sql += pair.first;
        sql += " AS ";
        for (size_t i = 0; i <= auxiliaryDatabasePaths.size(); ++i) {
            std::string selectFromAux("SELECT ");
            bool firstCol = true;
            for (const auto &colName : pair.second) {
                if (!firstCol) {
                    selectFromAux += ", ";
                }
                firstCol = false;
                selectFromAux += colName;
            }
            selectFromAux += " FROM db_";
            selectFromAux += toString(static_cast<int>(i));
            selectFromAux += ".";
            selectFromAux += pair.first;

            try {
                // Check that the request will succeed. In case of 'sparse'
                // databases...
                run(selectFromAux + " LIMIT 0");

                if (i > 0) {
                    sql += " UNION ALL ";
                }
                sql += selectFromAux;
            } catch (const std::exception &) {
            }
        }
        run(sql);
    }

    registerFunctions();
}

// ---------------------------------------------------------------------------

static double PROJ_SQLITE_GetValAsDouble(sqlite3_value *val, bool &gotVal) {
    switch (sqlite3_value_type(val)) {
    case SQLITE_FLOAT:
        gotVal = true;
        return sqlite3_value_double(val);

    case SQLITE_INTEGER:
        gotVal = true;
        return static_cast<double>(sqlite3_value_int64(val));

    default:
        gotVal = false;
        return 0.0;
    }
}

// ---------------------------------------------------------------------------

static void PROJ_SQLITE_pseudo_area_from_swne(sqlite3_context *pContext,
                                              int /* argc */,
                                              sqlite3_value **argv) {
    bool b0, b1, b2, b3;
    double south_lat = PROJ_SQLITE_GetValAsDouble(argv[0], b0);
    double west_lon = PROJ_SQLITE_GetValAsDouble(argv[1], b1);
    double north_lat = PROJ_SQLITE_GetValAsDouble(argv[2], b2);
    double east_lon = PROJ_SQLITE_GetValAsDouble(argv[3], b3);
    if (!b0 || !b1 || !b2 || !b3) {
        sqlite3_result_null(pContext);
        return;
    }
    // Deal with area crossing antimeridian
    if (east_lon < west_lon) {
        east_lon += 360.0;
    }
    // Integrate cos(lat) between south_lat and north_lat
    double pseudo_area = (east_lon - west_lon) *
                         (std::sin(common::Angle(north_lat).getSIValue()) -
                          std::sin(common::Angle(south_lat).getSIValue()));
    sqlite3_result_double(pContext, pseudo_area);
}

// ---------------------------------------------------------------------------

static void PROJ_SQLITE_intersects_bbox(sqlite3_context *pContext,
                                        int /* argc */, sqlite3_value **argv) {
    bool b0, b1, b2, b3, b4, b5, b6, b7;
    double south_lat1 = PROJ_SQLITE_GetValAsDouble(argv[0], b0);
    double west_lon1 = PROJ_SQLITE_GetValAsDouble(argv[1], b1);
    double north_lat1 = PROJ_SQLITE_GetValAsDouble(argv[2], b2);
    double east_lon1 = PROJ_SQLITE_GetValAsDouble(argv[3], b3);
    double south_lat2 = PROJ_SQLITE_GetValAsDouble(argv[4], b4);
    double west_lon2 = PROJ_SQLITE_GetValAsDouble(argv[5], b5);
    double north_lat2 = PROJ_SQLITE_GetValAsDouble(argv[6], b6);
    double east_lon2 = PROJ_SQLITE_GetValAsDouble(argv[7], b7);
    if (!b0 || !b1 || !b2 || !b3 || !b4 || !b5 || !b6 || !b7) {
        sqlite3_result_null(pContext);
        return;
    }
    auto bbox1 = metadata::GeographicBoundingBox::create(west_lon1, south_lat1,
                                                         east_lon1, north_lat1);
    auto bbox2 = metadata::GeographicBoundingBox::create(west_lon2, south_lat2,
                                                         east_lon2, north_lat2);
    sqlite3_result_int(pContext, bbox1->intersects(bbox2) ? 1 : 0);
}

// ---------------------------------------------------------------------------

#ifndef SQLITE_DETERMINISTIC
#define SQLITE_DETERMINISTIC 0
#endif

void DatabaseContext::Private::registerFunctions() {
    sqlite3_create_function(sqlite_handle_, "pseudo_area_from_swne", 4,
                            SQLITE_UTF8 | SQLITE_DETERMINISTIC, nullptr,
                            PROJ_SQLITE_pseudo_area_from_swne, nullptr,
                            nullptr);

    sqlite3_create_function(sqlite_handle_, "intersects_bbox", 8,
                            SQLITE_UTF8 | SQLITE_DETERMINISTIC, nullptr,
                            PROJ_SQLITE_intersects_bbox, nullptr, nullptr);
}

// ---------------------------------------------------------------------------

SQLResultSet DatabaseContext::Private::run(const std::string &sql,
                                           const ListOfParams &parameters) {

    sqlite3_stmt *stmt = nullptr;
    auto iter = mapSqlToStatement_.find(sql);
    if (iter != mapSqlToStatement_.end()) {
        stmt = iter->second;
        sqlite3_reset(stmt);
    } else {
        if (sqlite3_prepare_v2(sqlite_handle_, sql.c_str(),
                               static_cast<int>(sql.size()), &stmt,
                               nullptr) != SQLITE_OK) {
            throw FactoryException("SQLite error on " + sql + ": " +
                                   sqlite3_errmsg(sqlite_handle_));
        }
        mapSqlToStatement_.insert(
            std::pair<std::string, sqlite3_stmt *>(sql, stmt));
    }

    int nBindField = 1;
    for (const auto &param : parameters) {
        if (param.type() == SQLValues::Type::STRING) {
            auto strValue = param.stringValue();
            sqlite3_bind_text(stmt, nBindField, strValue.c_str(),
                              static_cast<int>(strValue.size()),
                              SQLITE_TRANSIENT);
        } else {
            assert(param.type() == SQLValues::Type::DOUBLE);
            sqlite3_bind_double(stmt, nBindField, param.doubleValue());
        }
        nBindField++;
    }

    SQLResultSet result;
    const int column_count = sqlite3_column_count(stmt);
    while (true) {
        int ret = sqlite3_step(stmt);
        if (ret == SQLITE_ROW) {
            SQLRow row(column_count);
            for (int i = 0; i < column_count; i++) {
                const char *txt = reinterpret_cast<const char *>(
                    sqlite3_column_text(stmt, i));
                if (txt) {
                    row[i] = txt;
                }
            }
            result.emplace_back(std::move(row));
        } else if (ret == SQLITE_DONE) {
            break;
        } else {
            throw FactoryException("SQLite error on " + sql + ": " +
                                   sqlite3_errmsg(sqlite_handle_));
        }
    }
    return result;
}

//! @endcond

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
DatabaseContext::~DatabaseContext() = default;
//! @endcond

// ---------------------------------------------------------------------------

DatabaseContext::DatabaseContext() : d(internal::make_unique<Private>()) {}

// ---------------------------------------------------------------------------

/** \brief Instantiate a database context.
 *
 * This database context should be used only by one thread at a time.
 *
 * @param databasePath Path and filename of the database. Might be empty
 * string for the default rules to locate the default proj.db
 * @param auxiliaryDatabasePaths Path and filename of auxiliary databases.
 * Might be empty.
 * @param ctx Context used for file search.
 * @throw FactoryException
 */
DatabaseContextNNPtr
DatabaseContext::create(const std::string &databasePath,
                        const std::vector<std::string> &auxiliaryDatabasePaths,
                        PJ_CONTEXT *ctx) {
    auto dbCtx = DatabaseContext::nn_make_shared<DatabaseContext>();
    dbCtx->getPrivate()->open(databasePath, ctx);
    if (!auxiliaryDatabasePaths.empty()) {
        dbCtx->getPrivate()->attachExtraDatabases(auxiliaryDatabasePaths);
    }
    return dbCtx;
}

// ---------------------------------------------------------------------------

/** \brief Return the list of authorities used in the database.
 */
std::set<std::string> DatabaseContext::getAuthorities() const {
    auto res = d->run("SELECT auth_name FROM authority_list");
    std::set<std::string> list;
    for (const auto &row : res) {
        list.insert(row[0]);
    }
    return list;
}

// ---------------------------------------------------------------------------

/** \brief Return the list of SQL commands (CREATE TABLE, CREATE TRIGGER,
 * CREATE VIEW) needed to initialize a new database.
 */
std::vector<std::string> DatabaseContext::getDatabaseStructure() const {
    return d->getDatabaseStructure();
}

// ---------------------------------------------------------------------------

/** \brief Return the path to the database.
 */
const std::string &DatabaseContext::getPath() const { return d->getPath(); }

// ---------------------------------------------------------------------------

/** \brief Return a metadata item.
 *
 * Value remains valid while this is alive and to the next call to getMetadata
 */
const char *DatabaseContext::getMetadata(const char *key) const {
    auto res =
        d->run("SELECT value FROM metadata WHERE key = ?", {std::string(key)});
    if (res.empty()) {
        return nullptr;
    }
    d->lastMetadataValue_ = res.front()[0];
    return d->lastMetadataValue_.c_str();
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress

DatabaseContextNNPtr DatabaseContext::create(void *sqlite_handle) {
    auto ctxt = DatabaseContext::nn_make_shared<DatabaseContext>();
    ctxt->getPrivate()->setHandle(static_cast<sqlite3 *>(sqlite_handle));
    return ctxt;
}

// ---------------------------------------------------------------------------

void *DatabaseContext::getSqliteHandle() const {
    return getPrivate()->handle();
}

// ---------------------------------------------------------------------------

bool DatabaseContext::lookForGridAlternative(const std::string &officialName,
                                             std::string &projFilename,
                                             std::string &projFormat,
                                             bool &inverse) const {
    auto res = d->run(
        "SELECT proj_grid_name, proj_grid_format, inverse_direction FROM "
        "grid_alternatives WHERE original_grid_name = ?",
        {officialName});
    if (res.empty()) {
        return false;
    }
    const auto &row = res.front();
    projFilename = row[0];
    projFormat = row[1];
    inverse = row[2] == "1";
    return true;
}

// ---------------------------------------------------------------------------

bool DatabaseContext::lookForGridInfo(const std::string &projFilename,
                                      std::string &fullFilename,
                                      std::string &packageName,
                                      std::string &url, bool &directDownload,
                                      bool &openLicense,
                                      bool &gridAvailable) const {
    Private::GridInfoCache info;
    if (d->getGridInfoFromCache(projFilename, info)) {
        fullFilename = info.fullFilename;
        packageName = info.packageName;
        url = info.url;
        directDownload = info.directDownload;
        openLicense = info.openLicense;
        gridAvailable = info.gridAvailable;
        return info.found;
    }

    fullFilename.clear();
    packageName.clear();
    url.clear();
    openLicense = false;
    directDownload = false;

    fullFilename.resize(2048);
    if (d->pjCtxt() == nullptr) {
        d->setPjCtxt(pj_get_default_ctx());
    }
    gridAvailable =
        pj_find_file(d->pjCtxt(), projFilename.c_str(), &fullFilename[0],
                     fullFilename.size() - 1) != 0;
    fullFilename.resize(strlen(fullFilename.c_str()));

    auto res =
        d->run("SELECT "
               "grid_packages.package_name, "
               "grid_alternatives.url, "
               "grid_packages.url AS package_url, "
               "grid_alternatives.open_license, "
               "grid_packages.open_license AS package_open_license, "
               "grid_alternatives.direct_download, "
               "grid_packages.direct_download AS package_direct_download "
               "FROM grid_alternatives "
               "LEFT JOIN grid_packages ON "
               "grid_alternatives.package_name = grid_packages.package_name "
               "WHERE proj_grid_name = ?",
               {projFilename});
    bool ret = !res.empty();
    if (ret) {
        const auto &row = res.front();
        packageName = std::move(row[0]);
        url = row[1].empty() ? std::move(row[2]) : std::move(row[1]);
        openLicense = (row[3].empty() ? row[4] : row[3]) == "1";
        directDownload = (row[5].empty() ? row[6] : row[5]) == "1";

        info.fullFilename = fullFilename;
        info.packageName = packageName;
        info.url = url;
        info.directDownload = directDownload;
        info.openLicense = openLicense;
    }
    info.gridAvailable = gridAvailable;
    info.found = ret;
    d->cache(projFilename, info);
    return ret;
}

// ---------------------------------------------------------------------------

bool DatabaseContext::isKnownName(const std::string &name,
                                  const std::string &tableName) const {
    std::string sql("SELECT 1 FROM \"");
    sql += replaceAll(tableName, "\"", "\"\"");
    sql += "\" WHERE name = ? LIMIT 1";
    return !d->run(sql, {name}).empty();
}

// ---------------------------------------------------------------------------

/** \brief Gets the alias name from an official name.
 *
 * @param officialName Official name.
 * @param tableName Table name/category. Mandatory
 * @param source Source of the alias. Mandatory
 * @return Alias name (or empty if not found).
 * @throw FactoryException
 */
std::string
DatabaseContext::getAliasFromOfficialName(const std::string &officialName,
                                          const std::string &tableName,
                                          const std::string &source) const {
    std::string sql("SELECT auth_name, code FROM \"");
    sql += replaceAll(tableName, "\"", "\"\"");
    sql += "\" WHERE name = ?";
    if (tableName == "geodetic_crs") {
        sql += " AND type = " GEOG_2D_SINGLE_QUOTED;
    }
    auto res = d->run(sql, {officialName});
    if (res.empty()) {
        return std::string();
    }
    const auto &row = res.front();
    res = d->run("SELECT alt_name FROM alias_name WHERE table_name = ? AND "
                 "auth_name = ? AND code = ? AND source = ?",
                 {tableName, row[0], row[1], source});
    if (res.empty()) {
        return std::string();
    }
    return res.front()[0];
}

// ---------------------------------------------------------------------------

/** \brief Return the 'text_definition' column of a table for an object
 *
 * @param tableName Table name/category.
 * @param authName Authority name of the object.
 * @param code Code of the object
 * @return Text definition (or empty)
 * @throw FactoryException
 */
std::string DatabaseContext::getTextDefinition(const std::string &tableName,
                                               const std::string &authName,
                                               const std::string &code) const {
    std::string sql("SELECT text_definition FROM \"");
    sql += replaceAll(tableName, "\"", "\"\"");
    sql += "\" WHERE auth_name = ? AND code = ?";
    auto res = d->run(sql, {authName, code});
    if (res.empty()) {
        return std::string();
    }
    return res.front()[0];
}

// ---------------------------------------------------------------------------

/** \brief Return the allowed authorities when researching transformations
 * between different authorities.
 *
 * @throw FactoryException
 */
std::vector<std::string> DatabaseContext::getAllowedAuthorities(
    const std::string &sourceAuthName,
    const std::string &targetAuthName) const {
    auto res = d->run(
        "SELECT allowed_authorities FROM authority_to_authority_preference "
        "WHERE source_auth_name = ? AND target_auth_name = ?",
        {sourceAuthName, targetAuthName});
    if (res.empty()) {
        res = d->run(
            "SELECT allowed_authorities FROM authority_to_authority_preference "
            "WHERE source_auth_name = ? AND target_auth_name = 'any'",
            {sourceAuthName});
    }
    if (res.empty()) {
        res = d->run(
            "SELECT allowed_authorities FROM authority_to_authority_preference "
            "WHERE source_auth_name = 'any' AND target_auth_name = ?",
            {targetAuthName});
    }
    if (res.empty()) {
        res = d->run(
            "SELECT allowed_authorities FROM authority_to_authority_preference "
            "WHERE source_auth_name = 'any' AND target_auth_name = 'any'",
            {});
    }
    if (res.empty()) {
        return std::vector<std::string>();
    }
    return split(res.front()[0], ',');
}

// ---------------------------------------------------------------------------

std::list<std::pair<std::string, std::string>>
DatabaseContext::getNonDeprecated(const std::string &tableName,
                                  const std::string &authName,
                                  const std::string &code) const {
    auto sqlRes =
        d->run("SELECT replacement_auth_name, replacement_code, source "
               "FROM deprecation "
               "WHERE table_name = ? AND deprecated_auth_name = ? "
               "AND deprecated_code = ?",
               {tableName, authName, code});
    std::list<std::pair<std::string, std::string>> res;
    for (const auto &row : sqlRes) {
        const auto &source = row[2];
        if (source == "PROJ") {
            const auto &replacement_auth_name = row[0];
            const auto &replacement_code = row[1];
            res.emplace_back(replacement_auth_name, replacement_code);
        }
    }
    if (!res.empty()) {
        return res;
    }
    for (const auto &row : sqlRes) {
        const auto &replacement_auth_name = row[0];
        const auto &replacement_code = row[1];
        res.emplace_back(replacement_auth_name, replacement_code);
    }
    return res;
}

//! @endcond

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
struct AuthorityFactory::Private {
    Private(const DatabaseContextNNPtr &contextIn,
            const std::string &authorityName)
        : context_(contextIn), authority_(authorityName) {}

    inline const std::string &authority() PROJ_PURE_DEFN { return authority_; }

    inline const DatabaseContextNNPtr &context() PROJ_PURE_DEFN {
        return context_;
    }

    // cppcheck-suppress functionStatic
    void setThis(AuthorityFactoryNNPtr factory) {
        thisFactory_ = factory.as_nullable();
    }

    // cppcheck-suppress functionStatic
    AuthorityFactoryPtr getSharedFromThis() { return thisFactory_.lock(); }

    inline AuthorityFactoryNNPtr createFactory(const std::string &auth_name) {
        if (auth_name == authority_) {
            return NN_NO_CHECK(thisFactory_.lock());
        }
        return AuthorityFactory::create(context_, auth_name);
    }

    bool rejectOpDueToMissingGrid(const operation::CoordinateOperationNNPtr &op,
                                  bool discardIfMissingGrid);

    UnitOfMeasure createUnitOfMeasure(const std::string &auth_name,
                                      const std::string &code);

    util::PropertyMap createProperties(const std::string &code,
                                       const std::string &name, bool deprecated,
                                       const metadata::ExtentPtr &extent);

    util::PropertyMap createProperties(const std::string &code,
                                       const std::string &name, bool deprecated,
                                       const std::string &area_of_use_auth_name,
                                       const std::string &area_of_use_code);

    util::PropertyMap createProperties(const std::string &code,
                                       const std::string &name, bool deprecated,
                                       const std::string &remarks,
                                       const std::string &scope,
                                       const std::string &area_of_use_auth_name,
                                       const std::string &area_of_use_code);

    SQLResultSet run(const std::string &sql,
                     const ListOfParams &parameters = ListOfParams());

    SQLResultSet runWithCodeParam(const std::string &sql,
                                  const std::string &code);

    SQLResultSet runWithCodeParam(const char *sql, const std::string &code);

    bool hasAuthorityRestriction() const {
        return !authority_.empty() && authority_ != "any";
    }

  private:
    DatabaseContextNNPtr context_;
    std::string authority_;
    std::weak_ptr<AuthorityFactory> thisFactory_{};
};

// ---------------------------------------------------------------------------

SQLResultSet AuthorityFactory::Private::run(const std::string &sql,
                                            const ListOfParams &parameters) {
    return context()->getPrivate()->run(sql, parameters);
}

// ---------------------------------------------------------------------------

SQLResultSet
AuthorityFactory::Private::runWithCodeParam(const std::string &sql,
                                            const std::string &code) {
    return run(sql, {authority(), code});
}

// ---------------------------------------------------------------------------

SQLResultSet
AuthorityFactory::Private::runWithCodeParam(const char *sql,
                                            const std::string &code) {
    return runWithCodeParam(std::string(sql), code);
}

// ---------------------------------------------------------------------------

UnitOfMeasure
AuthorityFactory::Private::createUnitOfMeasure(const std::string &auth_name,
                                               const std::string &code) {
    return *(createFactory(auth_name)->createUnitOfMeasure(code));
}

// ---------------------------------------------------------------------------

util::PropertyMap AuthorityFactory::Private::createProperties(
    const std::string &code, const std::string &name, bool deprecated,
    const metadata::ExtentPtr &extent) {
    auto props = util::PropertyMap()
                     .set(metadata::Identifier::CODESPACE_KEY, authority())
                     .set(metadata::Identifier::CODE_KEY, code)
                     .set(common::IdentifiedObject::NAME_KEY, name);
    if (deprecated) {
        props.set(common::IdentifiedObject::DEPRECATED_KEY, true);
    }
    if (extent) {
        props.set(
            common::ObjectUsage::DOMAIN_OF_VALIDITY_KEY,
            NN_NO_CHECK(std::static_pointer_cast<util::BaseObject>(extent)));
    }
    return props;
}

// ---------------------------------------------------------------------------

util::PropertyMap AuthorityFactory::Private::createProperties(
    const std::string &code, const std::string &name, bool deprecated,
    const std::string &area_of_use_auth_name,
    const std::string &area_of_use_code) {
    return createProperties(code, name, deprecated,
                            area_of_use_auth_name.empty()
                                ? nullptr
                                : createFactory(area_of_use_auth_name)
                                      ->createExtent(area_of_use_code)
                                      .as_nullable());
}

// ---------------------------------------------------------------------------

util::PropertyMap AuthorityFactory::Private::createProperties(
    const std::string &code, const std::string &name, bool deprecated,
    const std::string &remarks, const std::string &scope,
    const std::string &area_of_use_auth_name,
    const std::string &area_of_use_code) {
    auto props = createProperties(code, name, deprecated,
                                  area_of_use_auth_name.empty()
                                      ? nullptr
                                      : createFactory(area_of_use_auth_name)
                                            ->createExtent(area_of_use_code)
                                            .as_nullable());
    if (!remarks.empty())
        props.set(common::IdentifiedObject::REMARKS_KEY, remarks);
    if (!scope.empty())
        props.set(common::ObjectUsage::SCOPE_KEY, scope);
    return props;
}

// ---------------------------------------------------------------------------

bool AuthorityFactory::Private::rejectOpDueToMissingGrid(
    const operation::CoordinateOperationNNPtr &op, bool discardIfMissingGrid) {
    if (discardIfMissingGrid) {
        for (const auto &gridDesc : op->gridsNeeded(context())) {
            if (!gridDesc.available) {
                return true;
            }
        }
    }
    return false;
}

//! @endcond

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
AuthorityFactory::~AuthorityFactory() = default;
//! @endcond

// ---------------------------------------------------------------------------

AuthorityFactory::AuthorityFactory(const DatabaseContextNNPtr &context,
                                   const std::string &authorityName)
    : d(internal::make_unique<Private>(context, authorityName)) {}

// ---------------------------------------------------------------------------

// clang-format off
/** \brief Instantiate a AuthorityFactory.
 *
 * The authority name might be set to the empty string in the particular case
 * where createFromCoordinateReferenceSystemCodes(const std::string&,const std::string&,const std::string&,const std::string&) const
 * is called.
 *
 * @param context Contexte.
 * @param authorityName Authority name.
 * @return new AuthorityFactory.
 */
// clang-format on

AuthorityFactoryNNPtr
AuthorityFactory::create(const DatabaseContextNNPtr &context,
                         const std::string &authorityName) {

    auto factory = AuthorityFactory::nn_make_shared<AuthorityFactory>(
        context, authorityName);
    factory->d->setThis(factory);
    return factory;
}

// ---------------------------------------------------------------------------

/** \brief Returns the database context. */
const DatabaseContextNNPtr &AuthorityFactory::databaseContext() const {
    return d->context();
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
AuthorityFactory::CRSInfo::CRSInfo()
    : authName{}, code{}, name{}, type{ObjectType::CRS}, deprecated{},
      bbox_valid{}, west_lon_degree{}, south_lat_degree{}, east_lon_degree{},
      north_lat_degree{}, areaName{}, projectionMethodName{} {}
//! @endcond

// ---------------------------------------------------------------------------

/** \brief Returns an arbitrary object from a code.
 *
 * The returned object will typically be an instance of Datum,
 * CoordinateSystem, ReferenceSystem or CoordinateOperation. If the type of
 * the object is know at compile time, it is recommended to invoke the most
 * precise method instead of this one (for example
 * createCoordinateReferenceSystem(code) instead of createObject(code)
 * if the caller know he is asking for a coordinate reference system).
 *
 * If there are several objects with the same code, a FactoryException is
 * thrown.
 *
 * @param code Object code allocated by authority. (e.g. "4326")
 * @return object.
 * @throw NoSuchAuthorityCodeException
 * @throw FactoryException
 */

util::BaseObjectNNPtr
AuthorityFactory::createObject(const std::string &code) const {

    auto res = d->runWithCodeParam(
        "SELECT table_name FROM object_view WHERE auth_name = ? AND code = ?",
        code);
    if (res.empty()) {
        throw NoSuchAuthorityCodeException("not found", d->authority(), code);
    }
    if (res.size() != 1) {
        std::string msg(
            "More than one object matching specified code. Objects found in ");
        bool first = true;
        for (const auto &row : res) {
            if (!first)
                msg += ", ";
            msg += row[0];
            first = false;
        }
        throw FactoryException(msg);
    }
    const auto &table_name = res.front()[0];
    if (table_name == "area") {
        return util::nn_static_pointer_cast<util::BaseObject>(
            createExtent(code));
    }
    if (table_name == "unit_of_measure") {
        return util::nn_static_pointer_cast<util::BaseObject>(
            createUnitOfMeasure(code));
    }
    if (table_name == "prime_meridian") {
        return util::nn_static_pointer_cast<util::BaseObject>(
            createPrimeMeridian(code));
    }
    if (table_name == "ellipsoid") {
        return util::nn_static_pointer_cast<util::BaseObject>(
            createEllipsoid(code));
    }
    if (table_name == "geodetic_datum") {
        return util::nn_static_pointer_cast<util::BaseObject>(
            createGeodeticDatum(code));
    }
    if (table_name == "vertical_datum") {
        return util::nn_static_pointer_cast<util::BaseObject>(
            createVerticalDatum(code));
    }
    if (table_name == "geodetic_crs") {
        return util::nn_static_pointer_cast<util::BaseObject>(
            createGeodeticCRS(code));
    }
    if (table_name == "vertical_crs") {
        return util::nn_static_pointer_cast<util::BaseObject>(
            createVerticalCRS(code));
    }
    if (table_name == "projected_crs") {
        return util::nn_static_pointer_cast<util::BaseObject>(
            createProjectedCRS(code));
    }
    if (table_name == "compound_crs") {
        return util::nn_static_pointer_cast<util::BaseObject>(
            createCompoundCRS(code));
    }
    if (table_name == "conversion") {
        return util::nn_static_pointer_cast<util::BaseObject>(
            createConversion(code));
    }
    if (table_name == "helmert_transformation" ||
        table_name == "grid_transformation" ||
        table_name == "other_transformation" ||
        table_name == "concatenated_operation") {
        return util::nn_static_pointer_cast<util::BaseObject>(
            createCoordinateOperation(code, false));
    }
    throw FactoryException("unimplemented factory for " + res.front()[0]);
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
static FactoryException buildFactoryException(const char *type,
                                              const std::string &code,
                                              const std::exception &ex) {
    return FactoryException(std::string("cannot build ") + type + " " + code +
                            ": " + ex.what());
}
//! @endcond

// ---------------------------------------------------------------------------

/** \brief Returns a metadata::Extent from the specified code.
 *
 * @param code Object code allocated by authority.
 * @return object.
 * @throw NoSuchAuthorityCodeException
 * @throw FactoryException
 */

metadata::ExtentNNPtr
AuthorityFactory::createExtent(const std::string &code) const {
    const auto cacheKey(d->authority() + code);
    {
        auto extent = d->context()->d->getExtentFromCache(cacheKey);
        if (extent) {
            return NN_NO_CHECK(extent);
        }
    }
    auto sql = "SELECT name, south_lat, north_lat, west_lon, east_lon, "
               "deprecated FROM area WHERE auth_name = ? AND code = ?";
    auto res = d->runWithCodeParam(sql, code);
    if (res.empty()) {
        throw NoSuchAuthorityCodeException("area not found", d->authority(),
                                           code);
    }
    try {
        const auto &row = res.front();
        const auto &name = row[0];
        if (row[1].empty()) {
            auto extent = metadata::Extent::create(
                util::optional<std::string>(name), {}, {}, {});
            d->context()->d->cache(code, extent);
            return extent;
        }
        double south_lat = c_locale_stod(row[1]);
        double north_lat = c_locale_stod(row[2]);
        double west_lon = c_locale_stod(row[3]);
        double east_lon = c_locale_stod(row[4]);
        auto bbox = metadata::GeographicBoundingBox::create(
            west_lon, south_lat, east_lon, north_lat);

        auto extent = metadata::Extent::create(
            util::optional<std::string>(name),
            std::vector<metadata::GeographicExtentNNPtr>{bbox},
            std::vector<metadata::VerticalExtentNNPtr>(),
            std::vector<metadata::TemporalExtentNNPtr>());
        d->context()->d->cache(code, extent);
        return extent;

    } catch (const std::exception &ex) {
        throw buildFactoryException("area", code, ex);
    }
}

// ---------------------------------------------------------------------------

/** \brief Returns a common::UnitOfMeasure from the specified code.
 *
 * @param code Object code allocated by authority.
 * @return object.
 * @throw NoSuchAuthorityCodeException
 * @throw FactoryException
 */

UnitOfMeasureNNPtr
AuthorityFactory::createUnitOfMeasure(const std::string &code) const {
    const auto cacheKey(d->authority() + code);
    {
        auto uom = d->context()->d->getUOMFromCache(cacheKey);
        if (uom) {
            return NN_NO_CHECK(uom);
        }
    }
    auto res = d->runWithCodeParam(
        "SELECT name, conv_factor, type, deprecated FROM unit_of_measure WHERE "
        "auth_name = ? AND code = ?",
        code);
    if (res.empty()) {
        throw NoSuchAuthorityCodeException("unit of measure not found",
                                           d->authority(), code);
    }
    try {
        const auto &row = res.front();
        const auto &name =
            (row[0] == "degree (supplier to define representation)")
                ? UnitOfMeasure::DEGREE.name()
                : row[0];
        double conv_factor = (code == "9107" || code == "9108")
                                 ? UnitOfMeasure::DEGREE.conversionToSI()
                                 : c_locale_stod(row[1]);
        constexpr double EPS = 1e-10;
        if (std::fabs(conv_factor - UnitOfMeasure::DEGREE.conversionToSI()) <
            EPS * UnitOfMeasure::DEGREE.conversionToSI()) {
            conv_factor = UnitOfMeasure::DEGREE.conversionToSI();
        }
        if (std::fabs(conv_factor -
                      UnitOfMeasure::ARC_SECOND.conversionToSI()) <
            EPS * UnitOfMeasure::ARC_SECOND.conversionToSI()) {
            conv_factor = UnitOfMeasure::ARC_SECOND.conversionToSI();
        }
        const auto &type_str = row[2];
        UnitOfMeasure::Type unitType = UnitOfMeasure::Type::UNKNOWN;
        if (type_str == "length")
            unitType = UnitOfMeasure::Type::LINEAR;
        else if (type_str == "angle")
            unitType = UnitOfMeasure::Type::ANGULAR;
        else if (type_str == "scale")
            unitType = UnitOfMeasure::Type::SCALE;
        else if (type_str == "time")
            unitType = UnitOfMeasure::Type::TIME;
        auto uom = util::nn_make_shared<UnitOfMeasure>(
            name, conv_factor, unitType, d->authority(), code);
        d->context()->d->cache(cacheKey, uom);
        return uom;
    } catch (const std::exception &ex) {
        throw buildFactoryException("unit of measure", code, ex);
    }
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
static double normalizeMeasure(const std::string &uom_code,
                               const std::string &value,
                               std::string &normalized_uom_code) {
    if (uom_code == "9110") // DDD.MMSSsss.....
    {
        double normalized_value = c_locale_stod(value);
        std::ostringstream buffer;
        buffer.imbue(std::locale::classic());
        constexpr size_t precision = 12;
        buffer << std::fixed << std::setprecision(precision)
               << normalized_value;
        auto formatted = buffer.str();
        size_t dotPos = formatted.find('.');
        assert(dotPos + 1 + precision == formatted.size());
        auto minutes = formatted.substr(dotPos + 1, 2);
        auto seconds = formatted.substr(dotPos + 3);
        assert(seconds.size() == precision - 2);
        normalized_value =
            (normalized_value < 0 ? -1.0 : 1.0) *
            (std::floor(std::fabs(normalized_value)) +
             c_locale_stod(minutes) / 60. +
             (c_locale_stod(seconds) / std::pow(10, seconds.size() - 2)) /
                 3600.);
        normalized_uom_code = common::UnitOfMeasure::DEGREE.code();
        /* coverity[overflow_sink] */
        return normalized_value;
    } else {
        normalized_uom_code = uom_code;
        return c_locale_stod(value);
    }
}
//! @endcond

// ---------------------------------------------------------------------------

/** \brief Returns a datum::PrimeMeridian from the specified code.
 *
 * @param code Object code allocated by authority.
 * @return object.
 * @throw NoSuchAuthorityCodeException
 * @throw FactoryException
 */

datum::PrimeMeridianNNPtr
AuthorityFactory::createPrimeMeridian(const std::string &code) const {
    const auto cacheKey(d->authority() + code);
    {
        auto pm = d->context()->d->getPrimeMeridianFromCache(cacheKey);
        if (pm) {
            return NN_NO_CHECK(pm);
        }
    }
    auto res = d->runWithCodeParam(
        "SELECT name, longitude, uom_auth_name, uom_code, deprecated FROM "
        "prime_meridian WHERE "
        "auth_name = ? AND code = ?",
        code);
    if (res.empty()) {
        throw NoSuchAuthorityCodeException("prime meridian not found",
                                           d->authority(), code);
    }
    try {
        const auto &row = res.front();
        const auto &name = row[0];
        const auto &longitude = row[1];
        const auto &uom_auth_name = row[2];
        const auto &uom_code = row[3];
        const bool deprecated = row[4] == "1";

        std::string normalized_uom_code(uom_code);
        const double normalized_value =
            normalizeMeasure(uom_code, longitude, normalized_uom_code);

        auto uom = d->createUnitOfMeasure(uom_auth_name, normalized_uom_code);
        auto props = d->createProperties(code, name, deprecated, nullptr);
        auto pm = datum::PrimeMeridian::create(
            props, common::Angle(normalized_value, uom));
        d->context()->d->cache(cacheKey, pm);
        return pm;
    } catch (const std::exception &ex) {
        throw buildFactoryException("prime meridian", code, ex);
    }
}

// ---------------------------------------------------------------------------

/** \brief Identify a celestial body from an approximate radius.
 *
 * @param semi_major_axis Approximate semi-major axis.
 * @param tolerance Relative error allowed.
 * @return celestial body name if one single match found.
 * @throw FactoryException
 */

std::string
AuthorityFactory::identifyBodyFromSemiMajorAxis(double semi_major_axis,
                                                double tolerance) const {
    auto res =
        d->run("SELECT name, (ABS(semi_major_axis - ?) / semi_major_axis ) "
               "AS rel_error FROM celestial_body WHERE rel_error <= ?",
               {semi_major_axis, tolerance});
    if (res.empty()) {
        throw FactoryException("no match found");
    }
    if (res.size() > 1) {
        throw FactoryException("more than one match found");
    }
    return res.front()[0];
}

// ---------------------------------------------------------------------------

/** \brief Returns a datum::Ellipsoid from the specified code.
 *
 * @param code Object code allocated by authority.
 * @return object.
 * @throw NoSuchAuthorityCodeException
 * @throw FactoryException
 */

datum::EllipsoidNNPtr
AuthorityFactory::createEllipsoid(const std::string &code) const {
    auto res = d->runWithCodeParam(
        "SELECT ellipsoid.name, ellipsoid.semi_major_axis, "
        "ellipsoid.uom_auth_name, ellipsoid.uom_code, "
        "ellipsoid.inv_flattening, ellipsoid.semi_minor_axis, "
        "celestial_body.name AS body_name, ellipsoid.deprecated FROM "
        "ellipsoid JOIN celestial_body "
        "ON ellipsoid.celestial_body_auth_name = celestial_body.auth_name AND "
        "ellipsoid.celestial_body_code = celestial_body.code WHERE "
        "ellipsoid.auth_name = ? AND ellipsoid.code = ?",
        code);
    if (res.empty()) {
        throw NoSuchAuthorityCodeException("ellipsoid not found",
                                           d->authority(), code);
    }
    try {
        const auto &row = res.front();
        const auto &name = row[0];
        const auto &semi_major_axis_str = row[1];
        double semi_major_axis = c_locale_stod(semi_major_axis_str);
        const auto &uom_auth_name = row[2];
        const auto &uom_code = row[3];
        const auto &inv_flattening_str = row[4];
        const auto &semi_minor_axis_str = row[5];
        const auto &body = row[6];
        const bool deprecated = row[7] == "1";
        auto uom = d->createUnitOfMeasure(uom_auth_name, uom_code);
        auto props = d->createProperties(code, name, deprecated, nullptr);
        if (!inv_flattening_str.empty()) {
            return datum::Ellipsoid::createFlattenedSphere(
                props, common::Length(semi_major_axis, uom),
                common::Scale(c_locale_stod(inv_flattening_str)), body);
        } else if (semi_major_axis_str == semi_minor_axis_str) {
            return datum::Ellipsoid::createSphere(
                props, common::Length(semi_major_axis, uom), body);
        } else {
            return datum::Ellipsoid::createTwoAxis(
                props, common::Length(semi_major_axis, uom),
                common::Length(c_locale_stod(semi_minor_axis_str), uom), body);
        }
    } catch (const std::exception &ex) {
        throw buildFactoryException("ellipsoid", code, ex);
    }
}

// ---------------------------------------------------------------------------

/** \brief Returns a datum::GeodeticReferenceFrame from the specified code.
 *
 * @param code Object code allocated by authority.
 * @return object.
 * @throw NoSuchAuthorityCodeException
 * @throw FactoryException
 */

datum::GeodeticReferenceFrameNNPtr
AuthorityFactory::createGeodeticDatum(const std::string &code) const {
    const auto cacheKey(d->authority() + code);
    {
        auto datum = d->context()->d->getGeodeticDatumFromCache(cacheKey);
        if (datum) {
            return NN_NO_CHECK(datum);
        }
    }
    auto res = d->runWithCodeParam(
        "SELECT name, ellipsoid_auth_name, ellipsoid_code, "
        "prime_meridian_auth_name, prime_meridian_code, area_of_use_auth_name, "
        "area_of_use_code, deprecated FROM geodetic_datum WHERE "
        "auth_name = ? AND code = ?",
        code);
    if (res.empty()) {
        throw NoSuchAuthorityCodeException("geodetic datum not found",
                                           d->authority(), code);
    }
    try {
        const auto &row = res.front();
        const auto &name = row[0];
        const auto &ellipsoid_auth_name = row[1];
        const auto &ellipsoid_code = row[2];
        const auto &prime_meridian_auth_name = row[3];
        const auto &prime_meridian_code = row[4];
        const auto &area_of_use_auth_name = row[5];
        const auto &area_of_use_code = row[6];
        const bool deprecated = row[7] == "1";
        auto ellipsoid = d->createFactory(ellipsoid_auth_name)
                             ->createEllipsoid(ellipsoid_code);
        auto pm = d->createFactory(prime_meridian_auth_name)
                      ->createPrimeMeridian(prime_meridian_code);
        auto props = d->createProperties(
            code, name, deprecated, area_of_use_auth_name, area_of_use_code);
        auto anchor = util::optional<std::string>();
        auto datum =
            datum::GeodeticReferenceFrame::create(props, ellipsoid, anchor, pm);
        d->context()->d->cache(cacheKey, datum);
        return datum;
    } catch (const std::exception &ex) {
        throw buildFactoryException("geodetic reference frame", code, ex);
    }
}

// ---------------------------------------------------------------------------

/** \brief Returns a datum::VerticalReferenceFrame from the specified code.
 *
 * @param code Object code allocated by authority.
 * @return object.
 * @throw NoSuchAuthorityCodeException
 * @throw FactoryException
 */

datum::VerticalReferenceFrameNNPtr
AuthorityFactory::createVerticalDatum(const std::string &code) const {
    auto res = d->runWithCodeParam(
        "SELECT name, area_of_use_auth_name, area_of_use_code, deprecated FROM "
        "vertical_datum WHERE auth_name = ? AND code = ?",
        code);
    if (res.empty()) {
        throw NoSuchAuthorityCodeException("vertical datum not found",
                                           d->authority(), code);
    }
    try {
        const auto &row = res.front();
        const auto &name = row[0];
        const auto &area_of_use_auth_name = row[1];
        const auto &area_of_use_code = row[2];
        const bool deprecated = row[3] == "1";
        auto props = d->createProperties(
            code, name, deprecated, area_of_use_auth_name, area_of_use_code);
        auto anchor = util::optional<std::string>();
        return datum::VerticalReferenceFrame::create(props, anchor);
    } catch (const std::exception &ex) {
        throw buildFactoryException("vertical reference frame", code, ex);
    }
}

// ---------------------------------------------------------------------------

/** \brief Returns a datum::Datum from the specified code.
 *
 * @param code Object code allocated by authority.
 * @return object.
 * @throw NoSuchAuthorityCodeException
 * @throw FactoryException
 */

datum::DatumNNPtr AuthorityFactory::createDatum(const std::string &code) const {
    auto res =
        d->run("SELECT 'geodetic_datum' FROM geodetic_datum WHERE "
               "auth_name = ? AND code = ? "
               "UNION ALL SELECT 'vertical_datum' FROM vertical_datum WHERE "
               "auth_name = ? AND code = ?",
               {d->authority(), code, d->authority(), code});
    if (res.empty()) {
        throw NoSuchAuthorityCodeException("datum not found", d->authority(),
                                           code);
    }
    if (res.front()[0] == "geodetic_datum") {
        return createGeodeticDatum(code);
    }
    return createVerticalDatum(code);
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
static cs::MeridianPtr createMeridian(const std::string &val) {
    try {
        const std::string degW(std::string("\xC2\xB0") + "W");
        if (ends_with(val, degW)) {
            return cs::Meridian::create(common::Angle(
                -c_locale_stod(val.substr(0, val.size() - degW.size()))));
        }
        const std::string degE(std::string("\xC2\xB0") + "E");
        if (ends_with(val, degE)) {
            return cs::Meridian::create(common::Angle(
                c_locale_stod(val.substr(0, val.size() - degE.size()))));
        }
    } catch (const std::exception &) {
    }
    return nullptr;
}
//! @endcond

// ---------------------------------------------------------------------------

/** \brief Returns a cs::CoordinateSystem from the specified code.
 *
 * @param code Object code allocated by authority.
 * @return object.
 * @throw NoSuchAuthorityCodeException
 * @throw FactoryException
 */

cs::CoordinateSystemNNPtr
AuthorityFactory::createCoordinateSystem(const std::string &code) const {
    const auto cacheKey(d->authority() + code);
    {
        auto cs = d->context()->d->getCoordinateSystemFromCache(cacheKey);
        if (cs) {
            return NN_NO_CHECK(cs);
        }
    }
    auto res = d->runWithCodeParam(
        "SELECT axis.name, abbrev, orientation, uom_auth_name, uom_code, "
        "cs.type FROM "
        "axis LEFT JOIN coordinate_system cs ON "
        "axis.coordinate_system_auth_name = cs.auth_name AND "
        "axis.coordinate_system_code = cs.code WHERE "
        "coordinate_system_auth_name = ? AND coordinate_system_code = ? ORDER "
        "BY coordinate_system_order",
        code);
    if (res.empty()) {
        throw NoSuchAuthorityCodeException("coordinate system not found",
                                           d->authority(), code);
    }

    const auto &csType = res.front()[5];
    std::vector<cs::CoordinateSystemAxisNNPtr> axisList;
    for (const auto &row : res) {
        const auto &name = row[0];
        const auto &abbrev = row[1];
        const auto &orientation = row[2];
        const auto &uom_auth_name = row[3];
        const auto &uom_code = row[4];
        auto uom = d->createUnitOfMeasure(uom_auth_name, uom_code);
        auto props =
            util::PropertyMap().set(common::IdentifiedObject::NAME_KEY, name);
        const cs::AxisDirection *direction =
            cs::AxisDirection::valueOf(orientation);
        cs::MeridianPtr meridian;
        if (direction == nullptr) {
            if (orientation == "Geocentre > equator/0"
                               "\xC2\xB0"
                               "E") {
                direction = &(cs::AxisDirection::GEOCENTRIC_X);
            } else if (orientation == "Geocentre > equator/90"
                                      "\xC2\xB0"
                                      "E") {
                direction = &(cs::AxisDirection::GEOCENTRIC_Y);
            } else if (orientation == "Geocentre > north pole") {
                direction = &(cs::AxisDirection::GEOCENTRIC_Z);
            } else if (starts_with(orientation, "North along ")) {
                direction = &(cs::AxisDirection::NORTH);
                meridian =
                    createMeridian(orientation.substr(strlen("North along ")));
            } else if (starts_with(orientation, "South along ")) {
                direction = &(cs::AxisDirection::SOUTH);
                meridian =
                    createMeridian(orientation.substr(strlen("South along ")));
            } else {
                throw FactoryException("unknown axis direction: " +
                                       orientation);
            }
        }
        axisList.emplace_back(cs::CoordinateSystemAxis::create(
            props, abbrev, *direction, uom, meridian));
    }

    const auto cacheAndRet = [this,
                              &cacheKey](const cs::CoordinateSystemNNPtr &cs) {
        d->context()->d->cache(cacheKey, cs);
        return cs;
    };

    auto props = util::PropertyMap()
                     .set(metadata::Identifier::CODESPACE_KEY, d->authority())
                     .set(metadata::Identifier::CODE_KEY, code);
    if (csType == "ellipsoidal") {
        if (axisList.size() == 2) {
            return cacheAndRet(
                cs::EllipsoidalCS::create(props, axisList[0], axisList[1]));
        }
        if (axisList.size() == 3) {
            return cacheAndRet(cs::EllipsoidalCS::create(
                props, axisList[0], axisList[1], axisList[2]));
        }
        throw FactoryException("invalid number of axis for EllipsoidalCS");
    }
    if (csType == "Cartesian") {
        if (axisList.size() == 2) {
            return cacheAndRet(
                cs::CartesianCS::create(props, axisList[0], axisList[1]));
        }
        if (axisList.size() == 3) {
            return cacheAndRet(cs::CartesianCS::create(
                props, axisList[0], axisList[1], axisList[2]));
        }
        throw FactoryException("invalid number of axis for CartesianCS");
    }
    if (csType == "vertical") {
        if (axisList.size() == 1) {
            return cacheAndRet(cs::VerticalCS::create(props, axisList[0]));
        }
        throw FactoryException("invalid number of axis for VerticalCS");
    }
    throw FactoryException("unhandled coordinate system type: " + csType);
}

// ---------------------------------------------------------------------------

/** \brief Returns a crs::GeodeticCRS from the specified code.
 *
 * @param code Object code allocated by authority.
 * @return object.
 * @throw NoSuchAuthorityCodeException
 * @throw FactoryException
 */

crs::GeodeticCRSNNPtr
AuthorityFactory::createGeodeticCRS(const std::string &code) const {
    return createGeodeticCRS(code, false);
}

// ---------------------------------------------------------------------------

/** \brief Returns a crs::GeographicCRS from the specified code.
 *
 * @param code Object code allocated by authority.
 * @return object.
 * @throw NoSuchAuthorityCodeException
 * @throw FactoryException
 */

crs::GeographicCRSNNPtr
AuthorityFactory::createGeographicCRS(const std::string &code) const {
    return NN_NO_CHECK(util::nn_dynamic_pointer_cast<crs::GeographicCRS>(
        createGeodeticCRS(code, true)));
}

// ---------------------------------------------------------------------------

static crs::GeodeticCRSNNPtr
cloneWithProps(const crs::GeodeticCRSNNPtr &geodCRS,
               const util::PropertyMap &props) {
    auto cs = geodCRS->coordinateSystem();
    auto datum = geodCRS->datum();
    if (!datum) {
        return geodCRS;
    }
    auto ellipsoidalCS = util::nn_dynamic_pointer_cast<cs::EllipsoidalCS>(cs);
    if (ellipsoidalCS) {
        return crs::GeographicCRS::create(props, NN_NO_CHECK(datum),
                                          NN_NO_CHECK(ellipsoidalCS));
    }
    auto geocentricCS = util::nn_dynamic_pointer_cast<cs::CartesianCS>(cs);
    if (geocentricCS) {
        return crs::GeodeticCRS::create(props, NN_NO_CHECK(datum),
                                        NN_NO_CHECK(geocentricCS));
    }
    return geodCRS;
}

// ---------------------------------------------------------------------------

crs::GeodeticCRSNNPtr
AuthorityFactory::createGeodeticCRS(const std::string &code,
                                    bool geographicOnly) const {
    const auto cacheKey(d->authority() + code);
    auto crs = std::dynamic_pointer_cast<crs::GeodeticCRS>(
        d->context()->d->getCRSFromCache(cacheKey));
    if (crs) {
        return NN_NO_CHECK(crs);
    }
    std::string sql("SELECT name, type, coordinate_system_auth_name, "
                    "coordinate_system_code, datum_auth_name, datum_code, "
                    "area_of_use_auth_name, area_of_use_code, text_definition, "
                    "deprecated FROM "
                    "geodetic_crs WHERE auth_name = ? AND code = ?");
    if (geographicOnly) {
        sql += " AND type in (" GEOG_2D_SINGLE_QUOTED "," GEOG_3D_SINGLE_QUOTED
               ")";
    }
    auto res = d->runWithCodeParam(sql, code);
    if (res.empty()) {
        throw NoSuchAuthorityCodeException("geodeticCRS not found",
                                           d->authority(), code);
    }
    try {
        const auto &row = res.front();
        const auto &name = row[0];
        const auto &type = row[1];
        const auto &cs_auth_name = row[2];
        const auto &cs_code = row[3];
        const auto &datum_auth_name = row[4];
        const auto &datum_code = row[5];
        const auto &area_of_use_auth_name = row[6];
        const auto &area_of_use_code = row[7];
        const auto &text_definition = row[8];
        const bool deprecated = row[9] == "1";

        auto props = d->createProperties(
            code, name, deprecated, area_of_use_auth_name, area_of_use_code);

        if (!text_definition.empty()) {
            DatabaseContext::Private::RecursionDetector detector(d->context());
            auto obj = createFromUserInput(
                pj_add_type_crs_if_needed(text_definition), d->context());
            auto geodCRS = util::nn_dynamic_pointer_cast<crs::GeodeticCRS>(obj);
            if (geodCRS) {
                return cloneWithProps(NN_NO_CHECK(geodCRS), props);
            }

            auto boundCRS = dynamic_cast<const crs::BoundCRS *>(obj.get());
            if (boundCRS) {
                geodCRS = util::nn_dynamic_pointer_cast<crs::GeodeticCRS>(
                    boundCRS->baseCRS());
                if (geodCRS) {
                    auto newBoundCRS = crs::BoundCRS::create(
                        cloneWithProps(NN_NO_CHECK(geodCRS), props),
                        boundCRS->hubCRS(), boundCRS->transformation());
                    return NN_NO_CHECK(
                        util::nn_dynamic_pointer_cast<crs::GeodeticCRS>(
                            newBoundCRS->baseCRSWithCanonicalBoundCRS()));
                }
            }

            throw FactoryException(
                "text_definition does not define a GeodeticCRS");
        }

        auto cs =
            d->createFactory(cs_auth_name)->createCoordinateSystem(cs_code);
        auto datum =
            d->createFactory(datum_auth_name)->createGeodeticDatum(datum_code);

        auto ellipsoidalCS =
            util::nn_dynamic_pointer_cast<cs::EllipsoidalCS>(cs);
        if ((type == GEOG_2D || type == GEOG_3D) && ellipsoidalCS) {
            auto crsRet = crs::GeographicCRS::create(
                props, datum, NN_NO_CHECK(ellipsoidalCS));
            d->context()->d->cache(cacheKey, crsRet);
            return crsRet;
        }
        auto geocentricCS = util::nn_dynamic_pointer_cast<cs::CartesianCS>(cs);
        if (type == GEOCENTRIC && geocentricCS) {
            auto crsRet = crs::GeodeticCRS::create(props, datum,
                                                   NN_NO_CHECK(geocentricCS));
            d->context()->d->cache(cacheKey, crsRet);
            return crsRet;
        }
        throw FactoryException("unsupported (type, CS type) for geodeticCRS: " +
                               type + ", " + cs->getWKT2Type(true));
    } catch (const std::exception &ex) {
        throw buildFactoryException("geodeticCRS", code, ex);
    }
}

// ---------------------------------------------------------------------------

/** \brief Returns a crs::VerticalCRS from the specified code.
 *
 * @param code Object code allocated by authority.
 * @return object.
 * @throw NoSuchAuthorityCodeException
 * @throw FactoryException
 */

crs::VerticalCRSNNPtr
AuthorityFactory::createVerticalCRS(const std::string &code) const {
    auto res = d->runWithCodeParam(
        "SELECT name, coordinate_system_auth_name, "
        "coordinate_system_code, datum_auth_name, datum_code, "
        "area_of_use_auth_name, area_of_use_code, deprecated FROM "
        "vertical_crs WHERE auth_name = ? AND code = ?",
        code);
    if (res.empty()) {
        throw NoSuchAuthorityCodeException("verticalCRS not found",
                                           d->authority(), code);
    }
    try {
        const auto &row = res.front();
        const auto &name = row[0];
        const auto &cs_auth_name = row[1];
        const auto &cs_code = row[2];
        const auto &datum_auth_name = row[3];
        const auto &datum_code = row[4];
        const auto &area_of_use_auth_name = row[5];
        const auto &area_of_use_code = row[6];
        const bool deprecated = row[7] == "1";
        auto cs =
            d->createFactory(cs_auth_name)->createCoordinateSystem(cs_code);
        auto datum =
            d->createFactory(datum_auth_name)->createVerticalDatum(datum_code);

        auto props = d->createProperties(
            code, name, deprecated, area_of_use_auth_name, area_of_use_code);

        auto verticalCS = util::nn_dynamic_pointer_cast<cs::VerticalCS>(cs);
        if (verticalCS) {
            return crs::VerticalCRS::create(props, datum,
                                            NN_NO_CHECK(verticalCS));
        }
        throw FactoryException("unsupported CS type for verticalCRS: " +
                               cs->getWKT2Type(true));
    } catch (const std::exception &ex) {
        throw buildFactoryException("verticalCRS", code, ex);
    }
}

// ---------------------------------------------------------------------------

/** \brief Returns a operation::Conversion from the specified code.
 *
 * @param code Object code allocated by authority.
 * @return object.
 * @throw NoSuchAuthorityCodeException
 * @throw FactoryException
 */

operation::ConversionNNPtr
AuthorityFactory::createConversion(const std::string &code) const {

    static const char *sql =
        "SELECT name, area_of_use_auth_name, area_of_use_code, "
        "method_auth_name, method_code, method_name, "

        "param1_auth_name, param1_code, param1_name, param1_value, "
        "param1_uom_auth_name, param1_uom_code, "

        "param2_auth_name, param2_code, param2_name, param2_value, "
        "param2_uom_auth_name, param2_uom_code, "

        "param3_auth_name, param3_code, param3_name, param3_value, "
        "param3_uom_auth_name, param3_uom_code, "

        "param4_auth_name, param4_code, param4_name, param4_value, "
        "param4_uom_auth_name, param4_uom_code, "

        "param5_auth_name, param5_code, param5_name, param5_value, "
        "param5_uom_auth_name, param5_uom_code, "

        "param6_auth_name, param6_code, param6_name, param6_value, "
        "param6_uom_auth_name, param6_uom_code, "

        "param7_auth_name, param7_code, param7_name, param7_value, "
        "param7_uom_auth_name, param7_uom_code, "

        "deprecated FROM conversion WHERE auth_name = ? AND code = ?";

    auto res = d->runWithCodeParam(sql, code);
    if (res.empty()) {
        throw NoSuchAuthorityCodeException("conversion not found",
                                           d->authority(), code);
    }
    try {
        const auto &row = res.front();
        size_t idx = 0;
        const auto &name = row[idx++];
        const auto &area_of_use_auth_name = row[idx++];
        const auto &area_of_use_code = row[idx++];
        const auto &method_auth_name = row[idx++];
        const auto &method_code = row[idx++];
        const auto &method_name = row[idx++];
        const size_t base_param_idx = idx;
        std::vector<operation::OperationParameterNNPtr> parameters;
        std::vector<operation::ParameterValueNNPtr> values;
        constexpr int N_MAX_PARAMS = 7;
        for (int i = 0; i < N_MAX_PARAMS; ++i) {
            const auto &param_auth_name = row[base_param_idx + i * 6 + 0];
            if (param_auth_name.empty()) {
                break;
            }
            const auto &param_code = row[base_param_idx + i * 6 + 1];
            const auto &param_name = row[base_param_idx + i * 6 + 2];
            const auto &param_value = row[base_param_idx + i * 6 + 3];
            const auto &param_uom_auth_name = row[base_param_idx + i * 6 + 4];
            const auto &param_uom_code = row[base_param_idx + i * 6 + 5];
            parameters.emplace_back(operation::OperationParameter::create(
                util::PropertyMap()
                    .set(metadata::Identifier::CODESPACE_KEY, param_auth_name)
                    .set(metadata::Identifier::CODE_KEY, param_code)
                    .set(common::IdentifiedObject::NAME_KEY, param_name)));
            std::string normalized_uom_code(param_uom_code);
            const double normalized_value = normalizeMeasure(
                param_uom_code, param_value, normalized_uom_code);
            auto uom = d->createUnitOfMeasure(param_uom_auth_name,
                                              normalized_uom_code);
            values.emplace_back(operation::ParameterValue::create(
                common::Measure(normalized_value, uom)));
        }
        const bool deprecated = row[base_param_idx + N_MAX_PARAMS * 6] == "1";

        auto propConversion = d->createProperties(
            code, name, deprecated, area_of_use_auth_name, area_of_use_code);

        auto propMethod = util::PropertyMap().set(
            common::IdentifiedObject::NAME_KEY, method_name);
        if (!method_auth_name.empty()) {
            propMethod
                .set(metadata::Identifier::CODESPACE_KEY, method_auth_name)
                .set(metadata::Identifier::CODE_KEY, method_code);
        }

        return operation::Conversion::create(propConversion, propMethod,
                                             parameters, values);
    } catch (const std::exception &ex) {
        throw buildFactoryException("conversion", code, ex);
    }
}

// ---------------------------------------------------------------------------

/** \brief Returns a crs::ProjectedCRS from the specified code.
 *
 * @param code Object code allocated by authority.
 * @return object.
 * @throw NoSuchAuthorityCodeException
 * @throw FactoryException
 */

crs::ProjectedCRSNNPtr
AuthorityFactory::createProjectedCRS(const std::string &code) const {
    auto res = d->runWithCodeParam(
        "SELECT name, coordinate_system_auth_name, "
        "coordinate_system_code, geodetic_crs_auth_name, geodetic_crs_code, "
        "conversion_auth_name, conversion_code, "
        "area_of_use_auth_name, area_of_use_code, text_definition, "
        "deprecated FROM projected_crs WHERE auth_name = ? AND code = ?",
        code);
    if (res.empty()) {
        throw NoSuchAuthorityCodeException("projectedCRS not found",
                                           d->authority(), code);
    }
    try {
        const auto &row = res.front();
        const auto &name = row[0];
        const auto &cs_auth_name = row[1];
        const auto &cs_code = row[2];
        const auto &geodetic_crs_auth_name = row[3];
        const auto &geodetic_crs_code = row[4];
        const auto &conversion_auth_name = row[5];
        const auto &conversion_code = row[6];
        const auto &area_of_use_auth_name = row[7];
        const auto &area_of_use_code = row[8];
        const auto &text_definition = row[9];
        const bool deprecated = row[10] == "1";

        auto props = d->createProperties(
            code, name, deprecated, area_of_use_auth_name, area_of_use_code);

        if (!text_definition.empty()) {
            DatabaseContext::Private::RecursionDetector detector(d->context());
            auto obj = createFromUserInput(
                pj_add_type_crs_if_needed(text_definition), d->context());
            auto projCRS = dynamic_cast<const crs::ProjectedCRS *>(obj.get());
            if (projCRS) {
                const auto &conv = projCRS->derivingConversionRef();
                auto newConv =
                    (conv->nameStr() == "unnamed")
                        ? operation::Conversion::create(
                              util::PropertyMap().set(
                                  common::IdentifiedObject::NAME_KEY, name),
                              conv->method(), conv->parameterValues())
                        : conv;
                return crs::ProjectedCRS::create(props, projCRS->baseCRS(),
                                                 newConv,
                                                 projCRS->coordinateSystem());
            }

            auto boundCRS = dynamic_cast<const crs::BoundCRS *>(obj.get());
            if (boundCRS) {
                projCRS = dynamic_cast<const crs::ProjectedCRS *>(
                    boundCRS->baseCRS().get());
                if (projCRS) {
                    auto newBoundCRS = crs::BoundCRS::create(
                        crs::ProjectedCRS::create(
                            props, projCRS->baseCRS(),
                            projCRS->derivingConversionRef(),
                            projCRS->coordinateSystem()),
                        boundCRS->hubCRS(), boundCRS->transformation());
                    return NN_NO_CHECK(
                        util::nn_dynamic_pointer_cast<crs::ProjectedCRS>(
                            newBoundCRS->baseCRSWithCanonicalBoundCRS()));
                }
            }

            throw FactoryException(
                "text_definition does not define a ProjectedCRS");
        }

        auto cs =
            d->createFactory(cs_auth_name)->createCoordinateSystem(cs_code);

        auto baseCRS = d->createFactory(geodetic_crs_auth_name)
                           ->createGeodeticCRS(geodetic_crs_code);

        auto conv = d->createFactory(conversion_auth_name)
                        ->createConversion(conversion_code);
        if (conv->nameStr() == "unnamed") {
            conv = conv->shallowClone();
            conv->setProperties(util::PropertyMap().set(
                common::IdentifiedObject::NAME_KEY, name));
        }

        auto cartesianCS = util::nn_dynamic_pointer_cast<cs::CartesianCS>(cs);
        if (cartesianCS) {
            return crs::ProjectedCRS::create(props, baseCRS, conv,
                                             NN_NO_CHECK(cartesianCS));
        }
        throw FactoryException("unsupported CS type for projectedCRS: " +
                               cs->getWKT2Type(true));
    } catch (const std::exception &ex) {
        throw buildFactoryException("projectedCRS", code, ex);
    }
}

// ---------------------------------------------------------------------------

/** \brief Returns a crs::CompoundCRS from the specified code.
 *
 * @param code Object code allocated by authority.
 * @return object.
 * @throw NoSuchAuthorityCodeException
 * @throw FactoryException
 */

crs::CompoundCRSNNPtr
AuthorityFactory::createCompoundCRS(const std::string &code) const {
    auto res = d->runWithCodeParam(
        "SELECT name, horiz_crs_auth_name, horiz_crs_code, "
        "vertical_crs_auth_name, vertical_crs_code, "
        "area_of_use_auth_name, area_of_use_code, deprecated FROM "
        "compound_crs WHERE auth_name = ? AND code = ?",
        code);
    if (res.empty()) {
        throw NoSuchAuthorityCodeException("compoundCRS not found",
                                           d->authority(), code);
    }
    try {
        const auto &row = res.front();
        const auto &name = row[0];
        const auto &horiz_crs_auth_name = row[1];
        const auto &horiz_crs_code = row[2];
        const auto &vertical_crs_auth_name = row[3];
        const auto &vertical_crs_code = row[4];
        const auto &area_of_use_auth_name = row[5];
        const auto &area_of_use_code = row[6];
        const bool deprecated = row[7] == "1";

        auto horizCRS =
            d->createFactory(horiz_crs_auth_name)
                ->createCoordinateReferenceSystem(horiz_crs_code, false);
        auto vertCRS = d->createFactory(vertical_crs_auth_name)
                           ->createVerticalCRS(vertical_crs_code);

        auto props = d->createProperties(
            code, name, deprecated, area_of_use_auth_name, area_of_use_code);
        return crs::CompoundCRS::create(
            props, std::vector<crs::CRSNNPtr>{horizCRS, vertCRS});
    } catch (const std::exception &ex) {
        throw buildFactoryException("compoundCRS", code, ex);
    }
}

// ---------------------------------------------------------------------------

/** \brief Returns a crs::CRS from the specified code.
 *
 * @param code Object code allocated by authority.
 * @return object.
 * @throw NoSuchAuthorityCodeException
 * @throw FactoryException
 */

crs::CRSNNPtr AuthorityFactory::createCoordinateReferenceSystem(
    const std::string &code) const {
    return createCoordinateReferenceSystem(code, true);
}

//! @cond Doxygen_Suppress

crs::CRSNNPtr
AuthorityFactory::createCoordinateReferenceSystem(const std::string &code,
                                                  bool allowCompound) const {
    const auto cacheKey(d->authority() + code);
    auto crs = d->context()->d->getCRSFromCache(cacheKey);
    if (crs) {
        return NN_NO_CHECK(crs);
    }
    auto res = d->runWithCodeParam(
        "SELECT type FROM crs_view WHERE auth_name = ? AND code = ?", code);
    if (res.empty()) {
        throw NoSuchAuthorityCodeException("crs not found", d->authority(),
                                           code);
    }
    const auto &type = res.front()[0];
    if (type == GEOG_2D || type == GEOG_3D || type == GEOCENTRIC) {
        return createGeodeticCRS(code);
    }
    if (type == VERTICAL) {
        return createVerticalCRS(code);
    }
    if (type == PROJECTED) {
        return createProjectedCRS(code);
    }
    if (allowCompound && type == COMPOUND) {
        return createCompoundCRS(code);
    }
    throw FactoryException("unhandled CRS type: " + type);
}

//! @endcond

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress

static util::PropertyMap createMapNameEPSGCode(const std::string &name,
                                               int code) {
    return util::PropertyMap()
        .set(common::IdentifiedObject::NAME_KEY, name)
        .set(metadata::Identifier::CODESPACE_KEY, metadata::Identifier::EPSG)
        .set(metadata::Identifier::CODE_KEY, code);
}

// ---------------------------------------------------------------------------

static operation::OperationParameterNNPtr createOpParamNameEPSGCode(int code) {
    const char *name = operation::OperationParameter::getNameForEPSGCode(code);
    assert(name);
    return operation::OperationParameter::create(
        createMapNameEPSGCode(name, code));
}

static operation::ParameterValueNNPtr createLength(const std::string &value,
                                                   const UnitOfMeasure &uom) {
    return operation::ParameterValue::create(
        common::Length(c_locale_stod(value), uom));
}

static operation::ParameterValueNNPtr createAngle(const std::string &value,
                                                  const UnitOfMeasure &uom) {
    return operation::ParameterValue::create(
        common::Angle(c_locale_stod(value), uom));
}

//! @endcond

// ---------------------------------------------------------------------------

/** \brief Returns a operation::CoordinateOperation from the specified code.
 *
 * @param code Object code allocated by authority.
 * @param usePROJAlternativeGridNames Whether PROJ alternative grid names
 * should be substituted to the official grid names.
 * @return object.
 * @throw NoSuchAuthorityCodeException
 * @throw FactoryException
 */

operation::CoordinateOperationNNPtr AuthorityFactory::createCoordinateOperation(
    const std::string &code, bool usePROJAlternativeGridNames) const {
    return createCoordinateOperation(code, true, usePROJAlternativeGridNames,
                                     std::string());
}

operation::CoordinateOperationNNPtr AuthorityFactory::createCoordinateOperation(
    const std::string &code, bool allowConcatenated,
    bool usePROJAlternativeGridNames, const std::string &typeIn) const {
    std::string type(typeIn);
    if (type.empty()) {
        auto res = d->runWithCodeParam(
            "SELECT type FROM coordinate_operation_with_conversion_view "
            "WHERE auth_name = ? AND code = ?",
            code);
        if (res.empty()) {
            throw NoSuchAuthorityCodeException("coordinate operation not found",
                                               d->authority(), code);
        }
        type = res.front()[0];
    }

    if (type == "conversion") {
        return createConversion(code);
    }

    if (type == "helmert_transformation") {

        auto res = d->runWithCodeParam(
            "SELECT name, description, scope, "
            "method_auth_name, method_code, method_name, "
            "source_crs_auth_name, source_crs_code, target_crs_auth_name, "
            "target_crs_code, area_of_use_auth_name, area_of_use_code, "
            "accuracy, tx, ty, tz, translation_uom_auth_name, "
            "translation_uom_code, rx, ry, rz, rotation_uom_auth_name, "
            "rotation_uom_code, scale_difference, "
            "scale_difference_uom_auth_name, scale_difference_uom_code, "
            "rate_tx, rate_ty, rate_tz, rate_translation_uom_auth_name, "
            "rate_translation_uom_code, rate_rx, rate_ry, rate_rz, "
            "rate_rotation_uom_auth_name, rate_rotation_uom_code, "
            "rate_scale_difference, rate_scale_difference_uom_auth_name, "
            "rate_scale_difference_uom_code, epoch, epoch_uom_auth_name, "
            "epoch_uom_code, px, py, pz, pivot_uom_auth_name, pivot_uom_code, "
            "operation_version, deprecated FROM "
            "helmert_transformation WHERE auth_name = ? AND code = ?",
            code);
        if (res.empty()) {
            // shouldn't happen if foreign keys are OK
            throw NoSuchAuthorityCodeException(
                "helmert_transformation not found", d->authority(), code);
        }
        try {
            const auto &row = res.front();
            size_t idx = 0;
            const auto &name = row[idx++];
            const auto &description = row[idx++];
            const auto &scope = row[idx++];
            const auto &method_auth_name = row[idx++];
            const auto &method_code = row[idx++];
            const auto &method_name = row[idx++];
            const auto &source_crs_auth_name = row[idx++];
            const auto &source_crs_code = row[idx++];
            const auto &target_crs_auth_name = row[idx++];
            const auto &target_crs_code = row[idx++];
            const auto &area_of_use_auth_name = row[idx++];
            const auto &area_of_use_code = row[idx++];
            const auto &accuracy = row[idx++];

            const auto &tx = row[idx++];
            const auto &ty = row[idx++];
            const auto &tz = row[idx++];
            const auto &translation_uom_auth_name = row[idx++];
            const auto &translation_uom_code = row[idx++];
            const auto &rx = row[idx++];
            const auto &ry = row[idx++];
            const auto &rz = row[idx++];
            const auto &rotation_uom_auth_name = row[idx++];
            const auto &rotation_uom_code = row[idx++];
            const auto &scale_difference = row[idx++];
            const auto &scale_difference_uom_auth_name = row[idx++];
            const auto &scale_difference_uom_code = row[idx++];

            const auto &rate_tx = row[idx++];
            const auto &rate_ty = row[idx++];
            const auto &rate_tz = row[idx++];
            const auto &rate_translation_uom_auth_name = row[idx++];
            const auto &rate_translation_uom_code = row[idx++];
            const auto &rate_rx = row[idx++];
            const auto &rate_ry = row[idx++];
            const auto &rate_rz = row[idx++];
            const auto &rate_rotation_uom_auth_name = row[idx++];
            const auto &rate_rotation_uom_code = row[idx++];
            const auto &rate_scale_difference = row[idx++];
            const auto &rate_scale_difference_uom_auth_name = row[idx++];
            const auto &rate_scale_difference_uom_code = row[idx++];

            const auto &epoch = row[idx++];
            const auto &epoch_uom_auth_name = row[idx++];
            const auto &epoch_uom_code = row[idx++];

            const auto &px = row[idx++];
            const auto &py = row[idx++];
            const auto &pz = row[idx++];
            const auto &pivot_uom_auth_name = row[idx++];
            const auto &pivot_uom_code = row[idx++];

            const auto &operation_version = row[idx++];
            const auto &deprecated_str = row[idx++];
            const bool deprecated = deprecated_str == "1";
            assert(idx == row.size());

            auto uom_translation = d->createUnitOfMeasure(
                translation_uom_auth_name, translation_uom_code);

            auto uom_epoch = epoch_uom_auth_name.empty()
                                 ? common::UnitOfMeasure::NONE
                                 : d->createUnitOfMeasure(epoch_uom_auth_name,
                                                          epoch_uom_code);

            auto sourceCRS =
                d->createFactory(source_crs_auth_name)
                    ->createCoordinateReferenceSystem(source_crs_code);
            auto targetCRS =
                d->createFactory(target_crs_auth_name)
                    ->createCoordinateReferenceSystem(target_crs_code);

            std::vector<operation::OperationParameterNNPtr> parameters;
            std::vector<operation::ParameterValueNNPtr> values;

            parameters.emplace_back(createOpParamNameEPSGCode(
                EPSG_CODE_PARAMETER_X_AXIS_TRANSLATION));
            values.emplace_back(createLength(tx, uom_translation));

            parameters.emplace_back(createOpParamNameEPSGCode(
                EPSG_CODE_PARAMETER_Y_AXIS_TRANSLATION));
            values.emplace_back(createLength(ty, uom_translation));

            parameters.emplace_back(createOpParamNameEPSGCode(
                EPSG_CODE_PARAMETER_Z_AXIS_TRANSLATION));
            values.emplace_back(createLength(tz, uom_translation));

            if (!rx.empty()) {
                // Helmert 7-, 8-, 10- or 15- parameter cases
                auto uom_rotation = d->createUnitOfMeasure(
                    rotation_uom_auth_name, rotation_uom_code);

                parameters.emplace_back(createOpParamNameEPSGCode(
                    EPSG_CODE_PARAMETER_X_AXIS_ROTATION));
                values.emplace_back(createAngle(rx, uom_rotation));

                parameters.emplace_back(createOpParamNameEPSGCode(
                    EPSG_CODE_PARAMETER_Y_AXIS_ROTATION));
                values.emplace_back(createAngle(ry, uom_rotation));

                parameters.emplace_back(createOpParamNameEPSGCode(
                    EPSG_CODE_PARAMETER_Z_AXIS_ROTATION));
                values.emplace_back(createAngle(rz, uom_rotation));

                auto uom_scale_difference =
                    scale_difference_uom_auth_name.empty()
                        ? common::UnitOfMeasure::NONE
                        : d->createUnitOfMeasure(scale_difference_uom_auth_name,
                                                 scale_difference_uom_code);

                parameters.emplace_back(createOpParamNameEPSGCode(
                    EPSG_CODE_PARAMETER_SCALE_DIFFERENCE));
                values.emplace_back(operation::ParameterValue::create(
                    common::Scale(c_locale_stod(scale_difference),
                                  uom_scale_difference)));
            }

            if (!rate_tx.empty()) {
                // Helmert 15-parameter

                auto uom_rate_translation = d->createUnitOfMeasure(
                    rate_translation_uom_auth_name, rate_translation_uom_code);

                parameters.emplace_back(createOpParamNameEPSGCode(
                    EPSG_CODE_PARAMETER_RATE_X_AXIS_TRANSLATION));
                values.emplace_back(
                    createLength(rate_tx, uom_rate_translation));

                parameters.emplace_back(createOpParamNameEPSGCode(
                    EPSG_CODE_PARAMETER_RATE_Y_AXIS_TRANSLATION));
                values.emplace_back(
                    createLength(rate_ty, uom_rate_translation));

                parameters.emplace_back(createOpParamNameEPSGCode(
                    EPSG_CODE_PARAMETER_RATE_Z_AXIS_TRANSLATION));
                values.emplace_back(
                    createLength(rate_tz, uom_rate_translation));

                auto uom_rate_rotation = d->createUnitOfMeasure(
                    rate_rotation_uom_auth_name, rate_rotation_uom_code);

                parameters.emplace_back(createOpParamNameEPSGCode(
                    EPSG_CODE_PARAMETER_RATE_X_AXIS_ROTATION));
                values.emplace_back(createAngle(rate_rx, uom_rate_rotation));

                parameters.emplace_back(createOpParamNameEPSGCode(
                    EPSG_CODE_PARAMETER_RATE_Y_AXIS_ROTATION));
                values.emplace_back(createAngle(rate_ry, uom_rate_rotation));

                parameters.emplace_back(createOpParamNameEPSGCode(
                    EPSG_CODE_PARAMETER_RATE_Z_AXIS_ROTATION));
                values.emplace_back(createAngle(rate_rz, uom_rate_rotation));

                auto uom_rate_scale_difference =
                    d->createUnitOfMeasure(rate_scale_difference_uom_auth_name,
                                           rate_scale_difference_uom_code);
                parameters.emplace_back(createOpParamNameEPSGCode(
                    EPSG_CODE_PARAMETER_RATE_SCALE_DIFFERENCE));
                values.emplace_back(operation::ParameterValue::create(
                    common::Scale(c_locale_stod(rate_scale_difference),
                                  uom_rate_scale_difference)));

                parameters.emplace_back(createOpParamNameEPSGCode(
                    EPSG_CODE_PARAMETER_REFERENCE_EPOCH));
                values.emplace_back(operation::ParameterValue::create(
                    common::Measure(c_locale_stod(epoch), uom_epoch)));
            } else if (uom_epoch != common::UnitOfMeasure::NONE) {
                // Helmert 8-parameter
                parameters.emplace_back(createOpParamNameEPSGCode(
                    EPSG_CODE_PARAMETER_TRANSFORMATION_REFERENCE_EPOCH));
                values.emplace_back(operation::ParameterValue::create(
                    common::Measure(c_locale_stod(epoch), uom_epoch)));
            } else if (!px.empty()) {
                // Molodensky-Badekas case
                auto uom_pivot =
                    d->createUnitOfMeasure(pivot_uom_auth_name, pivot_uom_code);

                parameters.emplace_back(createOpParamNameEPSGCode(
                    EPSG_CODE_PARAMETER_ORDINATE_1_EVAL_POINT));
                values.emplace_back(createLength(px, uom_pivot));

                parameters.emplace_back(createOpParamNameEPSGCode(
                    EPSG_CODE_PARAMETER_ORDINATE_2_EVAL_POINT));
                values.emplace_back(createLength(py, uom_pivot));

                parameters.emplace_back(createOpParamNameEPSGCode(
                    EPSG_CODE_PARAMETER_ORDINATE_3_EVAL_POINT));
                values.emplace_back(createLength(pz, uom_pivot));
            }

            auto props =
                d->createProperties(code, name, deprecated, description, scope,
                                    area_of_use_auth_name, area_of_use_code);
            if (!operation_version.empty()) {
                props.set(operation::CoordinateOperation::OPERATION_VERSION_KEY,
                          operation_version);
            }

            auto propsMethod =
                util::PropertyMap()
                    .set(metadata::Identifier::CODESPACE_KEY, method_auth_name)
                    .set(metadata::Identifier::CODE_KEY, method_code)
                    .set(common::IdentifiedObject::NAME_KEY, method_name);

            std::vector<metadata::PositionalAccuracyNNPtr> accuracies;
            if (!accuracy.empty()) {
                accuracies.emplace_back(
                    metadata::PositionalAccuracy::create(accuracy));
            }
            return operation::Transformation::create(
                props, sourceCRS, targetCRS, nullptr, propsMethod, parameters,
                values, accuracies);

        } catch (const std::exception &ex) {
            throw buildFactoryException("transformation", code, ex);
        }
    }

    if (type == "grid_transformation") {
        auto res = d->runWithCodeParam(
            "SELECT name, description, scope, "
            "method_auth_name, method_code, method_name, "
            "source_crs_auth_name, source_crs_code, target_crs_auth_name, "
            "target_crs_code, area_of_use_auth_name, area_of_use_code, "
            "accuracy, grid_param_auth_name, grid_param_code, grid_param_name, "
            "grid_name, "
            "grid2_param_auth_name, grid2_param_code, grid2_param_name, "
            "grid2_name, "
            "interpolation_crs_auth_name, interpolation_crs_code, "
            "operation_version, deprecated FROM "
            "grid_transformation WHERE auth_name = ? AND code = ?",
            code);
        if (res.empty()) {
            // shouldn't happen if foreign keys are OK
            throw NoSuchAuthorityCodeException("grid_transformation not found",
                                               d->authority(), code);
        }
        try {
            const auto &row = res.front();
            size_t idx = 0;
            const auto &name = row[idx++];
            const auto &description = row[idx++];
            const auto &scope = row[idx++];
            const auto &method_auth_name = row[idx++];
            const auto &method_code = row[idx++];
            const auto &method_name = row[idx++];
            const auto &source_crs_auth_name = row[idx++];
            const auto &source_crs_code = row[idx++];
            const auto &target_crs_auth_name = row[idx++];
            const auto &target_crs_code = row[idx++];
            const auto &area_of_use_auth_name = row[idx++];
            const auto &area_of_use_code = row[idx++];
            const auto &accuracy = row[idx++];
            const auto &grid_param_auth_name = row[idx++];
            const auto &grid_param_code = row[idx++];
            const auto &grid_param_name = row[idx++];
            const auto &grid_name = row[idx++];
            const auto &grid2_param_auth_name = row[idx++];
            const auto &grid2_param_code = row[idx++];
            const auto &grid2_param_name = row[idx++];
            const auto &grid2_name = row[idx++];
            const auto &interpolation_crs_auth_name = row[idx++];
            const auto &interpolation_crs_code = row[idx++];
            const auto &operation_version = row[idx++];
            const auto &deprecated_str = row[idx++];
            const bool deprecated = deprecated_str == "1";
            assert(idx == row.size());

            auto sourceCRS =
                d->createFactory(source_crs_auth_name)
                    ->createCoordinateReferenceSystem(source_crs_code);
            auto targetCRS =
                d->createFactory(target_crs_auth_name)
                    ->createCoordinateReferenceSystem(target_crs_code);
            auto interpolationCRS =
                interpolation_crs_auth_name.empty()
                    ? nullptr
                    : d->createFactory(interpolation_crs_auth_name)
                          ->createCoordinateReferenceSystem(
                              interpolation_crs_code)
                          .as_nullable();

            std::vector<operation::OperationParameterNNPtr> parameters;
            std::vector<operation::ParameterValueNNPtr> values;

            parameters.emplace_back(operation::OperationParameter::create(
                util::PropertyMap()
                    .set(common::IdentifiedObject::NAME_KEY, grid_param_name)
                    .set(metadata::Identifier::CODESPACE_KEY,
                         grid_param_auth_name)
                    .set(metadata::Identifier::CODE_KEY, grid_param_code)));
            values.emplace_back(
                operation::ParameterValue::createFilename(grid_name));
            if (!grid2_name.empty()) {
                parameters.emplace_back(operation::OperationParameter::create(
                    util::PropertyMap()
                        .set(common::IdentifiedObject::NAME_KEY,
                             grid2_param_name)
                        .set(metadata::Identifier::CODESPACE_KEY,
                             grid2_param_auth_name)
                        .set(metadata::Identifier::CODE_KEY,
                             grid2_param_code)));
                values.emplace_back(
                    operation::ParameterValue::createFilename(grid2_name));
            }

            auto props =
                d->createProperties(code, name, deprecated, description, scope,
                                    area_of_use_auth_name, area_of_use_code);
            if (!operation_version.empty()) {
                props.set(operation::CoordinateOperation::OPERATION_VERSION_KEY,
                          operation_version);
            }
            auto propsMethod =
                util::PropertyMap()
                    .set(metadata::Identifier::CODESPACE_KEY, method_auth_name)
                    .set(metadata::Identifier::CODE_KEY, method_code)
                    .set(common::IdentifiedObject::NAME_KEY, method_name);

            std::vector<metadata::PositionalAccuracyNNPtr> accuracies;
            if (!accuracy.empty()) {
                accuracies.emplace_back(
                    metadata::PositionalAccuracy::create(accuracy));
            }
            auto transf = operation::Transformation::create(
                props, sourceCRS, targetCRS, interpolationCRS, propsMethod,
                parameters, values, accuracies);
            if (usePROJAlternativeGridNames) {
                return transf->substitutePROJAlternativeGridNames(d->context());
            }
            return transf;

        } catch (const std::exception &ex) {
            throw buildFactoryException("transformation", code, ex);
        }
    }

    if (type == "other_transformation") {
        std::ostringstream buffer;
        buffer.imbue(std::locale::classic());
        buffer
            << "SELECT name, description, scope, "
               "method_auth_name, method_code, method_name, "
               "source_crs_auth_name, source_crs_code, target_crs_auth_name, "
               "target_crs_code, area_of_use_auth_name, area_of_use_code, "
               "accuracy";
        constexpr int N_MAX_PARAMS = 7;
        for (int i = 1; i <= N_MAX_PARAMS; ++i) {
            buffer << ", param" << i << "_auth_name";
            buffer << ", param" << i << "_code";
            buffer << ", param" << i << "_name";
            buffer << ", param" << i << "_value";
            buffer << ", param" << i << "_uom_auth_name";
            buffer << ", param" << i << "_uom_code";
        }
        buffer << ", operation_version, deprecated FROM other_transformation "
                  "WHERE auth_name = ? AND code = ?";

        auto res = d->runWithCodeParam(buffer.str(), code);
        if (res.empty()) {
            // shouldn't happen if foreign keys are OK
            throw NoSuchAuthorityCodeException("other_transformation not found",
                                               d->authority(), code);
        }
        try {
            const auto &row = res.front();
            size_t idx = 0;
            const auto &name = row[idx++];
            const auto &description = row[idx++];
            const auto &scope = row[idx++];
            const auto &method_auth_name = row[idx++];
            const auto &method_code = row[idx++];
            const auto &method_name = row[idx++];
            const auto &source_crs_auth_name = row[idx++];
            const auto &source_crs_code = row[idx++];
            const auto &target_crs_auth_name = row[idx++];
            const auto &target_crs_code = row[idx++];
            const auto &area_of_use_auth_name = row[idx++];
            const auto &area_of_use_code = row[idx++];
            const auto &accuracy = row[idx++];

            const size_t base_param_idx = idx;
            std::vector<operation::OperationParameterNNPtr> parameters;
            std::vector<operation::ParameterValueNNPtr> values;
            for (int i = 0; i < N_MAX_PARAMS; ++i) {
                const auto &param_auth_name = row[base_param_idx + i * 6 + 0];
                if (param_auth_name.empty()) {
                    break;
                }
                const auto &param_code = row[base_param_idx + i * 6 + 1];
                const auto &param_name = row[base_param_idx + i * 6 + 2];
                const auto &param_value = row[base_param_idx + i * 6 + 3];
                const auto &param_uom_auth_name =
                    row[base_param_idx + i * 6 + 4];
                const auto &param_uom_code = row[base_param_idx + i * 6 + 5];
                parameters.emplace_back(operation::OperationParameter::create(
                    util::PropertyMap()
                        .set(metadata::Identifier::CODESPACE_KEY,
                             param_auth_name)
                        .set(metadata::Identifier::CODE_KEY, param_code)
                        .set(common::IdentifiedObject::NAME_KEY, param_name)));
                std::string normalized_uom_code(param_uom_code);
                const double normalized_value = normalizeMeasure(
                    param_uom_code, param_value, normalized_uom_code);
                auto uom = d->createUnitOfMeasure(param_uom_auth_name,
                                                  normalized_uom_code);
                values.emplace_back(operation::ParameterValue::create(
                    common::Measure(normalized_value, uom)));
            }
            idx = base_param_idx + 6 * N_MAX_PARAMS;

            const auto &operation_version = row[idx++];
            const auto &deprecated_str = row[idx++];
            const bool deprecated = deprecated_str == "1";
            assert(idx == row.size());

            auto sourceCRS =
                d->createFactory(source_crs_auth_name)
                    ->createCoordinateReferenceSystem(source_crs_code);
            auto targetCRS =
                d->createFactory(target_crs_auth_name)
                    ->createCoordinateReferenceSystem(target_crs_code);

            auto props =
                d->createProperties(code, name, deprecated, description, scope,
                                    area_of_use_auth_name, area_of_use_code);
            if (!operation_version.empty()) {
                props.set(operation::CoordinateOperation::OPERATION_VERSION_KEY,
                          operation_version);
            }

            std::vector<metadata::PositionalAccuracyNNPtr> accuracies;
            if (!accuracy.empty()) {
                accuracies.emplace_back(
                    metadata::PositionalAccuracy::create(accuracy));
            }

            if (method_auth_name == "PROJ") {
                if (method_code == "PROJString") {
                    return operation::SingleOperation::createPROJBased(
                        props, method_name, sourceCRS, targetCRS, accuracies);
                } else if (method_code == "WKT") {
                    auto op = util::nn_dynamic_pointer_cast<
                        operation::CoordinateOperation>(
                        WKTParser().createFromWKT(method_name));
                    if (!op) {
                        throw FactoryException("WKT string does not express a "
                                               "coordinate operation");
                    }
                    op->setCRSs(sourceCRS, targetCRS, nullptr);
                    return NN_NO_CHECK(op);
                }
            }

            auto propsMethod =
                util::PropertyMap()
                    .set(metadata::Identifier::CODESPACE_KEY, method_auth_name)
                    .set(metadata::Identifier::CODE_KEY, method_code)
                    .set(common::IdentifiedObject::NAME_KEY, method_name);

            if (method_auth_name == metadata::Identifier::EPSG &&
                operation::isAxisOrderReversal(
                    std::atoi(method_code.c_str()))) {
                auto op = operation::Conversion::create(props, propsMethod,
                                                        parameters, values);
                op->setCRSs(sourceCRS, targetCRS, nullptr);
                return op;
            }
            return operation::Transformation::create(
                props, sourceCRS, targetCRS, nullptr, propsMethod, parameters,
                values, accuracies);

        } catch (const std::exception &ex) {
            throw buildFactoryException("transformation", code, ex);
        }
    }

    if (allowConcatenated && type == "concatenated_operation") {
        auto res = d->runWithCodeParam(
            "SELECT name, description, scope, "
            "source_crs_auth_name, source_crs_code, "
            "target_crs_auth_name, target_crs_code, "
            "area_of_use_auth_name, area_of_use_code, accuracy, "
            "step1_auth_name, step1_code, step2_auth_name, step2_code, "
            "step3_auth_name, step3_code, operation_version, deprecated FROM "
            "concatenated_operation WHERE auth_name = ? AND code = ?",
            code);
        if (res.empty()) {
            // shouldn't happen if foreign keys are OK
            throw NoSuchAuthorityCodeException(
                "concatenated_operation not found", d->authority(), code);
        }
        try {
            const auto &row = res.front();
            size_t idx = 0;
            const auto &name = row[idx++];
            const auto &description = row[idx++];
            const auto &scope = row[idx++];
            const auto &source_crs_auth_name = row[idx++];
            const auto &source_crs_code = row[idx++];
            const auto &target_crs_auth_name = row[idx++];
            const auto &target_crs_code = row[idx++];
            const auto &area_of_use_auth_name = row[idx++];
            const auto &area_of_use_code = row[idx++];
            const auto &accuracy = row[idx++];
            const auto &step1_auth_name = row[idx++];
            const auto &step1_code = row[idx++];
            const auto &step2_auth_name = row[idx++];
            const auto &step2_code = row[idx++];
            const auto &step3_auth_name = row[idx++];
            const auto &step3_code = row[idx++];
            const auto &operation_version = row[idx++];
            const auto &deprecated_str = row[idx++];
            const bool deprecated = deprecated_str == "1";

            std::vector<operation::CoordinateOperationNNPtr> operations;
            operations.push_back(
                d->createFactory(step1_auth_name)
                    ->createCoordinateOperation(step1_code, false,
                                                usePROJAlternativeGridNames,
                                                std::string()));
            operations.push_back(
                d->createFactory(step2_auth_name)
                    ->createCoordinateOperation(step2_code, false,
                                                usePROJAlternativeGridNames,
                                                std::string()));

            if (!step3_auth_name.empty()) {
                operations.push_back(
                    d->createFactory(step3_auth_name)
                        ->createCoordinateOperation(step3_code, false,
                                                    usePROJAlternativeGridNames,
                                                    std::string()));
            }

            operation::ConcatenatedOperation::fixStepsDirection(
                d->createFactory(source_crs_auth_name)
                    ->createCoordinateReferenceSystem(source_crs_code),
                d->createFactory(target_crs_auth_name)
                    ->createCoordinateReferenceSystem(target_crs_code),
                operations);

            auto props =
                d->createProperties(code, name, deprecated, description, scope,
                                    area_of_use_auth_name, area_of_use_code);
            if (!operation_version.empty()) {
                props.set(operation::CoordinateOperation::OPERATION_VERSION_KEY,
                          operation_version);
            }

            std::vector<metadata::PositionalAccuracyNNPtr> accuracies;
            if (!accuracy.empty()) {
                accuracies.emplace_back(
                    metadata::PositionalAccuracy::create(accuracy));
            } else {
                // Try to compute a reasonable accuracy from the members
                double totalAcc = -1;
                try {
                    for (const auto &op : operations) {
                        auto accs = op->coordinateOperationAccuracies();
                        if (accs.size() == 1) {
                            double acc = c_locale_stod(accs[0]->value());
                            if (totalAcc < 0) {
                                totalAcc = acc;
                            } else {
                                totalAcc += acc;
                            }
                        } else {
                            totalAcc = -1;
                            break;
                        }
                    }
                    if (totalAcc >= 0) {
                        accuracies.emplace_back(
                            metadata::PositionalAccuracy::create(
                                toString(totalAcc)));
                    }
                } catch (const std::exception &) {
                }
            }
            return operation::ConcatenatedOperation::create(props, operations,
                                                            accuracies);

        } catch (const std::exception &ex) {
            throw buildFactoryException("transformation", code, ex);
        }
    }

    throw FactoryException("unhandled coordinate operation type: " + type);
}

// ---------------------------------------------------------------------------

/** \brief Returns a list operation::CoordinateOperation between two CRS.
 *
 * The list is ordered with preferred operations first. No attempt is made
 * at inferring operations that are not explicitly in the database.
 *
 * Deprecated operations are rejected.
 *
 * @param sourceCRSCode Source CRS code allocated by authority.
 * @param targetCRSCode Source CRS code allocated by authority.
 * @return list of coordinate operations
 * @throw NoSuchAuthorityCodeException
 * @throw FactoryException
 */

std::vector<operation::CoordinateOperationNNPtr>
AuthorityFactory::createFromCoordinateReferenceSystemCodes(
    const std::string &sourceCRSCode, const std::string &targetCRSCode) const {
    return createFromCoordinateReferenceSystemCodes(
        d->authority(), sourceCRSCode, d->authority(), targetCRSCode, false,
        false, false);
}

// ---------------------------------------------------------------------------

/** \brief Returns a list operation::CoordinateOperation between two CRS.
 *
 * The list is ordered with preferred operations first. No attempt is made
 * at inferring operations that are not explicitly in the database (see
 * createFromCRSCodesWithIntermediates() for that), and only
 * source -> target operations are searched (ie if target -> source is present,
 * you need to call this method with the arguments reversed, and apply the
 * reverse transformations).
 *
 * Deprecated operations are rejected.
 *
 * If getAuthority() returns empty, then coordinate operations from all
 * authorities are considered.
 *
 * @param sourceCRSAuthName Authority name of sourceCRSCode
 * @param sourceCRSCode Source CRS code allocated by authority
 * sourceCRSAuthName.
 * @param targetCRSAuthName Authority name of targetCRSCode
 * @param targetCRSCode Source CRS code allocated by authority
 * targetCRSAuthName.
 * @param usePROJAlternativeGridNames Whether PROJ alternative grid names
 * should be substituted to the official grid names.
 * @param discardIfMissingGrid Whether coordinate operations that reference
 * missing grids should be removed from the result set.
 * @param discardSuperseded Whether cordinate operations that are superseded
 * (but not deprecated) should be removed from the result set.
 * @return list of coordinate operations
 * @throw NoSuchAuthorityCodeException
 * @throw FactoryException
 */

std::vector<operation::CoordinateOperationNNPtr>
AuthorityFactory::createFromCoordinateReferenceSystemCodes(
    const std::string &sourceCRSAuthName, const std::string &sourceCRSCode,
    const std::string &targetCRSAuthName, const std::string &targetCRSCode,
    bool usePROJAlternativeGridNames, bool discardIfMissingGrid,
    bool discardSuperseded) const {

    auto cacheKey(d->authority());
    cacheKey += sourceCRSAuthName;
    cacheKey += sourceCRSCode;
    cacheKey += targetCRSAuthName;
    cacheKey += targetCRSCode;
    cacheKey += (usePROJAlternativeGridNames ? '1' : '0');
    cacheKey += (discardIfMissingGrid ? '1' : '0');
    cacheKey += (discardSuperseded ? '1' : '0');

    std::vector<operation::CoordinateOperationNNPtr> list;

    if (d->context()->d->getCRSToCRSCoordOpFromCache(cacheKey, list)) {
        return list;
    }

    // Look-up first for conversion which is the most precise.
    std::string sql("SELECT conversion_auth_name, "
                    "geodetic_crs_auth_name, geodetic_crs_code FROM "
                    "projected_crs WHERE auth_name = ? AND code = ?");
    auto params = ListOfParams{targetCRSAuthName, targetCRSCode};
    auto res = d->run(sql, params);
    if (!res.empty()) {
        const auto &row = res.front();
        bool ok = row[1] == sourceCRSAuthName && row[2] == sourceCRSCode;
        if (ok && d->hasAuthorityRestriction()) {
            ok = row[0] == d->authority();
        }
        if (ok) {
            auto targetCRS = d->createFactory(targetCRSAuthName)
                                 ->createProjectedCRS(targetCRSCode);
            auto conv = targetCRS->derivingConversion();
            list.emplace_back(conv);
            d->context()->d->cache(cacheKey, list);
            return list;
        }
    }
    if (discardSuperseded) {
        sql = "SELECT cov.auth_name, cov.code, cov.table_name, "
              "ss.replacement_auth_name, ss.replacement_code FROM "
              "coordinate_operation_view cov JOIN area "
              "ON cov.area_of_use_auth_name = area.auth_name AND "
              "cov.area_of_use_code = area.code "
              "LEFT JOIN supersession ss ON "
              "ss.superseded_table_name = cov.table_name AND "
              "ss.superseded_auth_name = cov.auth_name AND "
              "ss.superseded_code = cov.code AND "
              "ss.superseded_table_name = ss.replacement_table_name "
              "WHERE source_crs_auth_name = ? AND source_crs_code = ? AND "
              "target_crs_auth_name = ? AND target_crs_code = ? AND "
              "cov.deprecated = 0";
    } else {
        sql = "SELECT cov.auth_name, cov.code, cov.table_name FROM "
              "coordinate_operation_view cov JOIN area "
              "ON cov.area_of_use_auth_name = area.auth_name AND "
              "cov.area_of_use_code = area.code "
              "WHERE source_crs_auth_name = ? AND source_crs_code = ? AND "
              "target_crs_auth_name = ? AND target_crs_code = ? AND "
              "cov.deprecated = 0";
    }
    params = {sourceCRSAuthName, sourceCRSCode, targetCRSAuthName,
              targetCRSCode};
    if (d->hasAuthorityRestriction()) {
        sql += " AND cov.auth_name = ?";
        params.emplace_back(d->authority());
    }
    sql += " ORDER BY pseudo_area_from_swne(south_lat, west_lon, north_lat, "
           "east_lon) DESC, "
           "(CASE WHEN accuracy is NULL THEN 1 ELSE 0 END), accuracy";
    res = d->run(sql, params);
    std::set<std::pair<std::string, std::string>> setTransf;
    if (discardSuperseded) {
        for (const auto &row : res) {
            const auto &auth_name = row[0];
            const auto &code = row[1];
            setTransf.insert(
                std::pair<std::string, std::string>(auth_name, code));
        }
    }
    for (const auto &row : res) {
        if (discardSuperseded) {
            const auto &replacement_auth_name = row[3];
            const auto &replacement_code = row[4];
            if (!replacement_auth_name.empty() &&
                setTransf.find(std::pair<std::string, std::string>(
                    replacement_auth_name, replacement_code)) !=
                    setTransf.end()) {
                // Skip transformations that are superseded by others that got
                // returned in the result set.
                continue;
            }
        }

        const auto &auth_name = row[0];
        const auto &code = row[1];
        const auto &table_name = row[2];
        auto op = d->createFactory(auth_name)->createCoordinateOperation(
            code, true, usePROJAlternativeGridNames, table_name);
        if (!d->rejectOpDueToMissingGrid(op, discardIfMissingGrid)) {
            list.emplace_back(op);
        }
    }
    d->context()->d->cache(cacheKey, list);
    return list;
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
static std::string
buildIntermediateWhere(const std::vector<std::pair<std::string, std::string>>
                           &intermediateCRSAuthCodes,
                       const std::string &first_field,
                       const std::string &second_field) {
    if (intermediateCRSAuthCodes.empty()) {
        return std::string();
    }
    std::string sql(" AND (");
    for (size_t i = 0; i < intermediateCRSAuthCodes.size(); ++i) {
        if (i > 0) {
            sql += " OR";
        }
        sql += "(v1." + first_field + "_crs_auth_name = ? AND ";
        sql += "v1." + first_field + "_crs_code = ? AND ";
        sql += "v2." + second_field + "_crs_auth_name = ? AND ";
        sql += "v2." + second_field + "_crs_code = ?) ";
    }
    sql += ")";
    return sql;
}
//! @endcond

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
static bool useIrrelevantPivot(const operation::CoordinateOperationNNPtr &op,
                               const std::string &sourceCRSAuthName,
                               const std::string &sourceCRSCode,
                               const std::string &targetCRSAuthName,
                               const std::string &targetCRSCode) {
    auto concat =
        dynamic_cast<const operation::ConcatenatedOperation *>(op.get());
    if (!concat) {
        return false;
    }
    auto ops = concat->operations();
    for (size_t i = 0; i + 1 < ops.size(); i++) {
        auto targetCRS = ops[i]->targetCRS();
        if (targetCRS) {
            const auto &ids = targetCRS->identifiers();
            if (ids.size() == 1 &&
                ((*ids[0]->codeSpace() == sourceCRSAuthName &&
                  ids[0]->code() == sourceCRSCode) ||
                 (*ids[0]->codeSpace() == targetCRSAuthName &&
                  ids[0]->code() == targetCRSCode))) {
                return true;
            }
        }
    }
    return false;
}
//! @endcond

// ---------------------------------------------------------------------------

/** \brief Returns a list operation::CoordinateOperation between two CRS,
 * using intermediate codes.
 *
 * The list is ordered with preferred operations first.
 *
 * Deprecated operations are rejected.
 *
 * The method will take care of considering all potential combinations (ie
 * contrary to createFromCoordinateReferenceSystemCodes(), you do not need to
 * call it with sourceCRS and targetCRS switched)
 *
 * If getAuthority() returns empty, then coordinate operations from all
 * authorities are considered.
 *
 * @param sourceCRSAuthName Authority name of sourceCRSCode
 * @param sourceCRSCode Source CRS code allocated by authority
 * sourceCRSAuthName.
 * @param targetCRSAuthName Authority name of targetCRSCode
 * @param targetCRSCode Source CRS code allocated by authority
 * targetCRSAuthName.
 * @param usePROJAlternativeGridNames Whether PROJ alternative grid names
 * should be substituted to the official grid names.
 * @param discardIfMissingGrid Whether coordinate operations that reference
 * missing grids should be removed from the result set.
 * @param discardSuperseded Whether cordinate operations that are superseded
 * (but not deprecated) should be removed from the result set.
 * @param intermediateCRSAuthCodes List of (auth_name, code) of CRS that can be
 * used as potential intermediate CRS. If the list is empty, the database will
 * be used to find common CRS in operations involving both the source and
 * target CRS.
 * @return list of coordinate operations
 * @throw NoSuchAuthorityCodeException
 * @throw FactoryException
 */

std::vector<operation::CoordinateOperationNNPtr>
AuthorityFactory::createFromCRSCodesWithIntermediates(
    const std::string &sourceCRSAuthName, const std::string &sourceCRSCode,
    const std::string &targetCRSAuthName, const std::string &targetCRSCode,
    bool usePROJAlternativeGridNames, bool discardIfMissingGrid,
    bool discardSuperseded,
    const std::vector<std::pair<std::string, std::string>>
        &intermediateCRSAuthCodes) const {

    std::vector<operation::CoordinateOperationNNPtr> listTmp;

    if (sourceCRSAuthName == targetCRSAuthName &&
        sourceCRSCode == targetCRSCode) {
        return listTmp;
    }

    const std::string sqlProlog(
        discardSuperseded
            ?

            "SELECT v1.table_name as table1, "
            "v1.auth_name AS auth_name1, v1.code AS code1, "
            "v1.accuracy AS accuracy1, "
            "v2.table_name as table2, "
            "v2.auth_name AS auth_name2, v2.code AS code2, "
            "v2.accuracy as accuracy2, "
            "a1.south_lat AS south_lat1, "
            "a1.west_lon AS west_lon1, "
            "a1.north_lat AS north_lat1, "
            "a1.east_lon AS east_lon1, "
            "a2.south_lat AS south_lat2, "
            "a2.west_lon AS west_lon2, "
            "a2.north_lat AS north_lat2, "
            "a2.east_lon AS east_lon2, "
            "ss1.replacement_auth_name AS replacement_auth_name1, "
            "ss1.replacement_code AS replacement_code1, "
            "ss2.replacement_auth_name AS replacement_auth_name2, "
            "ss2.replacement_code AS replacement_code2 "
            "FROM coordinate_operation_view v1 "
            "JOIN coordinate_operation_view v2 "
            :

            "SELECT v1.table_name as table1, "
            "v1.auth_name AS auth_name1, v1.code AS code1, "
            "v1.accuracy AS accuracy1, "
            "v2.table_name as table2, "
            "v2.auth_name AS auth_name2, v2.code AS code2, "
            "v2.accuracy as accuracy2, "
            "a1.south_lat AS south_lat1, "
            "a1.west_lon AS west_lon1, "
            "a1.north_lat AS north_lat1, "
            "a1.east_lon AS east_lon1, "
            "a2.south_lat AS south_lat2, "
            "a2.west_lon AS west_lon2, "
            "a2.north_lat AS north_lat2, "
            "a2.east_lon AS east_lon2 "
            "FROM coordinate_operation_view v1 "
            "JOIN coordinate_operation_view v2 ");

    const std::string joinSupersession(
        "LEFT JOIN supersession ss1 ON "
        "ss1.superseded_table_name = v1.table_name AND "
        "ss1.superseded_auth_name = v1.auth_name AND "
        "ss1.superseded_code = v1.code AND "
        "ss1.superseded_table_name = ss1.replacement_table_name "
        "LEFT JOIN supersession ss2 ON "
        "ss2.superseded_table_name = v2.table_name AND "
        "ss2.superseded_auth_name = v2.auth_name AND "
        "ss2.superseded_code = v2.code AND "
        "ss2.superseded_table_name = ss2.replacement_table_name ");
    const std::string joinArea(
        (discardSuperseded ? joinSupersession : std::string()) +
        "JOIN area a1 ON v1.area_of_use_auth_name = a1.auth_name "
        "AND v1.area_of_use_code = a1.code "
        "JOIN area a2 ON v2.area_of_use_auth_name = a2.auth_name "
        "AND v2.area_of_use_code = a2.code ");
    const std::string orderBy(
        "ORDER BY (CASE WHEN accuracy1 is NULL THEN 1 ELSE 0 END) + "
        "(CASE WHEN accuracy2 is NULL THEN 1 ELSE 0 END), "
        "accuracy1 + accuracy2");

    // Case (source->intermediate) and (intermediate->target)
    std::string sql(
        sqlProlog + "ON v1.target_crs_auth_name = v2.source_crs_auth_name "
                    "AND v1.target_crs_code = v2.source_crs_code " +
        joinArea +
        "WHERE v1.source_crs_auth_name = ? AND v1.source_crs_code = ? "
        "AND v2.target_crs_auth_name = ? AND v2.target_crs_code = ? ");
    auto params = ListOfParams{sourceCRSAuthName, sourceCRSCode,
                               targetCRSAuthName, targetCRSCode};

    std::string additionalWhere(
        "AND v1.deprecated = 0 AND v2.deprecated = 0 "
        "AND intersects_bbox(south_lat1, west_lon1, north_lat1, east_lon1, "
        "south_lat2, west_lon2, north_lat2, east_lon2) == 1 ");
    if (d->hasAuthorityRestriction()) {
        additionalWhere += "AND v1.auth_name = ? AND v2.auth_name = ? ";
        params.emplace_back(d->authority());
        params.emplace_back(d->authority());
    }
    std::string intermediateWhere =
        buildIntermediateWhere(intermediateCRSAuthCodes, "target", "source");
    for (const auto &pair : intermediateCRSAuthCodes) {
        params.emplace_back(pair.first);
        params.emplace_back(pair.second);
        params.emplace_back(pair.first);
        params.emplace_back(pair.second);
    }
    auto res =
        d->run(sql + additionalWhere + intermediateWhere + orderBy, params);

    const auto filterOutSuperseded = [](SQLResultSet &&resultSet) {
        std::set<std::pair<std::string, std::string>> setTransf1;
        std::set<std::pair<std::string, std::string>> setTransf2;
        for (const auto &row : resultSet) {
            // table1
            const auto &auth_name1 = row[1];
            const auto &code1 = row[2];
            // accuracy1
            // table2
            const auto &auth_name2 = row[5];
            const auto &code2 = row[6];
            setTransf1.insert(
                std::pair<std::string, std::string>(auth_name1, code1));
            setTransf2.insert(
                std::pair<std::string, std::string>(auth_name2, code2));
        }
        SQLResultSet filteredResultSet;
        for (const auto &row : resultSet) {
            const auto &replacement_auth_name1 = row[16];
            const auto &replacement_code1 = row[17];
            const auto &replacement_auth_name2 = row[18];
            const auto &replacement_code2 = row[19];
            if (!replacement_auth_name1.empty() &&
                setTransf1.find(std::pair<std::string, std::string>(
                    replacement_auth_name1, replacement_code1)) !=
                    setTransf1.end()) {
                // Skip transformations that are superseded by others that got
                // returned in the result set.
                continue;
            }
            if (!replacement_auth_name2.empty() &&
                setTransf2.find(std::pair<std::string, std::string>(
                    replacement_auth_name2, replacement_code2)) !=
                    setTransf2.end()) {
                // Skip transformations that are superseded by others that got
                // returned in the result set.
                continue;
            }
            filteredResultSet.emplace_back(row);
        }
        return filteredResultSet;
    };

    if (discardSuperseded) {
        res = filterOutSuperseded(std::move(res));
    }
    for (const auto &row : res) {
        const auto &table1 = row[0];
        const auto &auth_name1 = row[1];
        const auto &code1 = row[2];
        // const auto &accuracy1 = row[3];
        const auto &table2 = row[4];
        const auto &auth_name2 = row[5];
        const auto &code2 = row[6];
        // const auto &accuracy2 = row[7];
        auto op1 = d->createFactory(auth_name1)
                       ->createCoordinateOperation(
                           code1, true, usePROJAlternativeGridNames, table1);
        if (useIrrelevantPivot(op1, sourceCRSAuthName, sourceCRSCode,
                               targetCRSAuthName, targetCRSCode)) {
            continue;
        }
        auto op2 = d->createFactory(auth_name2)
                       ->createCoordinateOperation(
                           code2, true, usePROJAlternativeGridNames, table2);
        if (useIrrelevantPivot(op2, sourceCRSAuthName, sourceCRSCode,
                               targetCRSAuthName, targetCRSCode)) {
            continue;
        }

        listTmp.emplace_back(
            operation::ConcatenatedOperation::createComputeMetadata({op1, op2},
                                                                    false));
    }

    // Case (source->intermediate) and (target->intermediate)
    sql = sqlProlog + "ON v1.target_crs_auth_name = v2.target_crs_auth_name "
                      "AND v1.target_crs_code = v2.target_crs_code " +
          joinArea +
          "WHERE v1.source_crs_auth_name = ? AND v1.source_crs_code = ? "
          "AND v2.source_crs_auth_name = ? AND v2.source_crs_code = ? ";
    intermediateWhere =
        buildIntermediateWhere(intermediateCRSAuthCodes, "target", "target");
    res = d->run(sql + additionalWhere + intermediateWhere + orderBy, params);
    if (discardSuperseded) {
        res = filterOutSuperseded(std::move(res));
    }
    for (const auto &row : res) {
        const auto &table1 = row[0];
        const auto &auth_name1 = row[1];
        const auto &code1 = row[2];
        // const auto &accuracy1 = row[3];
        const auto &table2 = row[4];
        const auto &auth_name2 = row[5];
        const auto &code2 = row[6];
        // const auto &accuracy2 = row[7];
        auto op1 = d->createFactory(auth_name1)
                       ->createCoordinateOperation(
                           code1, true, usePROJAlternativeGridNames, table1);
        if (useIrrelevantPivot(op1, sourceCRSAuthName, sourceCRSCode,
                               targetCRSAuthName, targetCRSCode)) {
            continue;
        }
        auto op2 = d->createFactory(auth_name2)
                       ->createCoordinateOperation(
                           code2, true, usePROJAlternativeGridNames, table2);
        if (useIrrelevantPivot(op2, sourceCRSAuthName, sourceCRSCode,
                               targetCRSAuthName, targetCRSCode)) {
            continue;
        }

        listTmp.emplace_back(
            operation::ConcatenatedOperation::createComputeMetadata(
                {op1, op2->inverse()}, false));
    }

    // Case (intermediate->source) and (intermediate->target)
    sql = sqlProlog + "ON v1.source_crs_auth_name = v2.source_crs_auth_name "
                      "AND v1.source_crs_code = v2.source_crs_code " +
          joinArea +
          "WHERE v1.target_crs_auth_name = ? AND v1.target_crs_code = ? "
          "AND v2.target_crs_auth_name = ? AND v2.target_crs_code = ? ";
    intermediateWhere =
        buildIntermediateWhere(intermediateCRSAuthCodes, "source", "source");
    res = d->run(sql + additionalWhere + intermediateWhere + orderBy, params);
    if (discardSuperseded) {
        res = filterOutSuperseded(std::move(res));
    }
    for (const auto &row : res) {
        const auto &table1 = row[0];
        const auto &auth_name1 = row[1];
        const auto &code1 = row[2];
        // const auto &accuracy1 = row[3];
        const auto &table2 = row[4];
        const auto &auth_name2 = row[5];
        const auto &code2 = row[6];
        // const auto &accuracy2 = row[7];
        auto op1 = d->createFactory(auth_name1)
                       ->createCoordinateOperation(
                           code1, true, usePROJAlternativeGridNames, table1);
        if (useIrrelevantPivot(op1, sourceCRSAuthName, sourceCRSCode,
                               targetCRSAuthName, targetCRSCode)) {
            continue;
        }
        auto op2 = d->createFactory(auth_name2)
                       ->createCoordinateOperation(
                           code2, true, usePROJAlternativeGridNames, table2);
        if (useIrrelevantPivot(op2, sourceCRSAuthName, sourceCRSCode,
                               targetCRSAuthName, targetCRSCode)) {
            continue;
        }

        listTmp.emplace_back(
            operation::ConcatenatedOperation::createComputeMetadata(
                {op1->inverse(), op2}, false));
    }

    // Case (intermediate->source) and (target->intermediate)
    sql = sqlProlog + "ON v1.source_crs_auth_name = v2.target_crs_auth_name "
                      "AND v1.source_crs_code = v2.target_crs_code " +
          joinArea +
          "WHERE v1.target_crs_auth_name = ? AND v1.target_crs_code = ? "
          "AND v2.source_crs_auth_name = ? AND v2.source_crs_code = ? ";
    intermediateWhere =
        buildIntermediateWhere(intermediateCRSAuthCodes, "source", "target");
    res = d->run(sql + additionalWhere + intermediateWhere + orderBy, params);
    if (discardSuperseded) {
        res = filterOutSuperseded(std::move(res));
    }
    for (const auto &row : res) {
        const auto &table1 = row[0];
        const auto &auth_name1 = row[1];
        const auto &code1 = row[2];
        // const auto &accuracy1 = row[3];
        const auto &table2 = row[4];
        const auto &auth_name2 = row[5];
        const auto &code2 = row[6];
        // const auto &accuracy2 = row[7];
        auto op1 = d->createFactory(auth_name1)
                       ->createCoordinateOperation(
                           code1, true, usePROJAlternativeGridNames, table1);
        if (useIrrelevantPivot(op1, sourceCRSAuthName, sourceCRSCode,
                               targetCRSAuthName, targetCRSCode)) {
            continue;
        }
        auto op2 = d->createFactory(auth_name2)
                       ->createCoordinateOperation(
                           code2, true, usePROJAlternativeGridNames, table2);
        if (useIrrelevantPivot(op2, sourceCRSAuthName, sourceCRSCode,
                               targetCRSAuthName, targetCRSCode)) {
            continue;
        }

        listTmp.emplace_back(
            operation::ConcatenatedOperation::createComputeMetadata(
                {op1->inverse(), op2->inverse()}, false));
    }

    std::vector<operation::CoordinateOperationNNPtr> list;
    for (const auto &op : listTmp) {
        if (!d->rejectOpDueToMissingGrid(op, discardIfMissingGrid)) {
            list.emplace_back(op);
        }
    }

    return list;
}

// ---------------------------------------------------------------------------

/** \brief Returns the authority name associated to this factory.
 * @return name.
 */
const std::string &AuthorityFactory::getAuthority() PROJ_PURE_DEFN {
    return d->authority();
}

// ---------------------------------------------------------------------------

/** \brief Returns the set of authority codes of the given object type.
 *
 * @param type Object type.
 * @param allowDeprecated whether we should return deprecated objects as well.
 * @return the set of authority codes for spatial reference objects of the given
 * type
 * @throw FactoryException
 */
std::set<std::string>
AuthorityFactory::getAuthorityCodes(const ObjectType &type,
                                    bool allowDeprecated) const {
    std::string sql;
    switch (type) {
    case ObjectType::PRIME_MERIDIAN:
        sql = "SELECT code FROM prime_meridian WHERE ";
        break;
    case ObjectType::ELLIPSOID:
        sql = "SELECT code FROM ellipsoid WHERE ";
        break;
    case ObjectType::DATUM:
        sql = "SELECT code FROM object_view WHERE table_name IN "
              "('geodetic_datum', 'vertical_datum') AND ";
        break;
    case ObjectType::GEODETIC_REFERENCE_FRAME:
        sql = "SELECT code FROM geodetic_datum WHERE ";
        break;
    case ObjectType::VERTICAL_REFERENCE_FRAME:
        sql = "SELECT code FROM vertical_datum WHERE ";
        break;
    case ObjectType::CRS:
        sql = "SELECT code FROM crs_view WHERE ";
        break;
    case ObjectType::GEODETIC_CRS:
        sql = "SELECT code FROM geodetic_crs WHERE ";
        break;
    case ObjectType::GEOCENTRIC_CRS:
        sql = "SELECT code FROM geodetic_crs WHERE type "
              "= " GEOCENTRIC_SINGLE_QUOTED " AND ";
        break;
    case ObjectType::GEOGRAPHIC_CRS:
        sql = "SELECT code FROM geodetic_crs WHERE type IN "
              "(" GEOG_2D_SINGLE_QUOTED "," GEOG_3D_SINGLE_QUOTED ") AND ";
        break;
    case ObjectType::GEOGRAPHIC_2D_CRS:
        sql =
            "SELECT code FROM geodetic_crs WHERE type = " GEOG_2D_SINGLE_QUOTED
            " AND ";
        break;
    case ObjectType::GEOGRAPHIC_3D_CRS:
        sql =
            "SELECT code FROM geodetic_crs WHERE type = " GEOG_3D_SINGLE_QUOTED
            " AND ";
        break;
    case ObjectType::VERTICAL_CRS:
        sql = "SELECT code FROM vertical_crs WHERE ";
        break;
    case ObjectType::PROJECTED_CRS:
        sql = "SELECT code FROM projected_crs WHERE ";
        break;
    case ObjectType::COMPOUND_CRS:
        sql = "SELECT code FROM compound_crs WHERE ";
        break;
    case ObjectType::COORDINATE_OPERATION:
        sql =
            "SELECT code FROM coordinate_operation_with_conversion_view WHERE ";
        break;
    case ObjectType::CONVERSION:
        sql = "SELECT code FROM conversion WHERE ";
        break;
    case ObjectType::TRANSFORMATION:
        sql = "SELECT code FROM coordinate_operation_view WHERE table_name != "
              "'concatenated_operation' AND ";
        break;
    case ObjectType::CONCATENATED_OPERATION:
        sql = "SELECT code FROM concatenated_operation WHERE ";
        break;
    }

    sql += "auth_name = ?";
    if (!allowDeprecated) {
        sql += " AND deprecated = 0";
    }

    auto res = d->run(sql, {d->authority()});
    std::set<std::string> set;
    for (const auto &row : res) {
        set.insert(row[0]);
    }
    return set;
}

// ---------------------------------------------------------------------------

/** \brief Gets a description of the object corresponding to a code.
 *
 * \note In case of several objects of different types with the same code,
 * one of them will be arbitrarily selected.
 *
 * @param code Object code allocated by authority. (e.g. "4326")
 * @return description.
 * @throw NoSuchAuthorityCodeException
 * @throw FactoryException
 */
std::string
AuthorityFactory::getDescriptionText(const std::string &code) const {
    auto sql = "SELECT name FROM object_view WHERE auth_name = ? AND code = "
               "? ORDER BY table_name";
    auto res = d->runWithCodeParam(sql, code);
    if (res.empty()) {
        throw NoSuchAuthorityCodeException("object not found", d->authority(),
                                           code);
    }
    return res.front()[0];
}

// ---------------------------------------------------------------------------

/** \brief Return a list of information on CRS objects
 *
 * This is functionnaly equivalent of listing the codes from an authority,
 * instantiating
 * a CRS object for each of them and getting the information from this CRS
 * object, but this implementation has much less overhead.
 *
 * @throw FactoryException
 */
std::list<AuthorityFactory::CRSInfo> AuthorityFactory::getCRSInfoList() const {
    std::string sql = "SELECT c.auth_name, c.code, c.name, c.type, "
                      "c.deprecated, "
                      "a.west_lon, a.south_lat, a.east_lon, a.north_lat, "
                      "a.name, NULL FROM geodetic_crs c "
                      "JOIN area a ON "
                      "c.area_of_use_auth_name = a.auth_name AND "
                      "c.area_of_use_code = a.code";
    ListOfParams params;
    if (d->hasAuthorityRestriction()) {
        sql += " WHERE c.auth_name = ?";
        params.emplace_back(d->authority());
    }
    sql += " UNION ALL ";
    sql += "SELECT c.auth_name, c.code, c.name, 'projected', "
           "c.deprecated, "
           "a.west_lon, a.south_lat, a.east_lon, a.north_lat, "
           "a.name, cm.name AS conversion_method_name FROM projected_crs c "
           "JOIN area a ON "
           "c.area_of_use_auth_name = a.auth_name AND "
           "c.area_of_use_code = a.code "
           "LEFT JOIN conversion_table conv ON "
           "c.conversion_auth_name = conv.auth_name AND "
           "c.conversion_code = conv.code "
           "LEFT JOIN conversion_method cm ON "
           "conv.method_auth_name = cm.auth_name AND "
           "conv.method_code = cm.code";
    if (d->hasAuthorityRestriction()) {
        sql += " WHERE c.auth_name = ?";
        params.emplace_back(d->authority());
    }
    sql += " UNION ALL ";
    sql += "SELECT c.auth_name, c.code, c.name, 'vertical', "
           "c.deprecated, "
           "a.west_lon, a.south_lat, a.east_lon, a.north_lat, "
           "a.name, NULL FROM vertical_crs c "
           "JOIN area a ON "
           "c.area_of_use_auth_name = a.auth_name AND "
           "c.area_of_use_code = a.code";
    if (d->hasAuthorityRestriction()) {
        sql += " WHERE c.auth_name = ?";
        params.emplace_back(d->authority());
    }
    sql += " UNION ALL ";
    sql += "SELECT c.auth_name, c.code, c.name, 'compound', "
           "c.deprecated, "
           "a.west_lon, a.south_lat, a.east_lon, a.north_lat, "
           "a.name, NULL FROM compound_crs c "
           "JOIN area a ON "
           "c.area_of_use_auth_name = a.auth_name AND "
           "c.area_of_use_code = a.code";
    if (d->hasAuthorityRestriction()) {
        sql += " WHERE c.auth_name = ?";
        params.emplace_back(d->authority());
    }
    auto sqlRes = d->run(sql, params);
    std::list<AuthorityFactory::CRSInfo> res;
    for (const auto &row : sqlRes) {
        AuthorityFactory::CRSInfo info;
        info.authName = row[0];
        info.code = row[1];
        info.name = row[2];
        const auto &type = row[3];
        if (type == GEOG_2D) {
            info.type = AuthorityFactory::ObjectType::GEOGRAPHIC_2D_CRS;
        } else if (type == GEOG_3D) {
            info.type = AuthorityFactory::ObjectType::GEOGRAPHIC_3D_CRS;
        } else if (type == GEOCENTRIC) {
            info.type = AuthorityFactory::ObjectType::GEOCENTRIC_CRS;
        } else if (type == PROJECTED) {
            info.type = AuthorityFactory::ObjectType::PROJECTED_CRS;
        } else if (type == VERTICAL) {
            info.type = AuthorityFactory::ObjectType::VERTICAL_CRS;
        } else if (type == COMPOUND) {
            info.type = AuthorityFactory::ObjectType::COMPOUND_CRS;
        }
        info.deprecated = row[4] == "1";
        if (row[5].empty()) {
            info.bbox_valid = false;
        } else {
            info.bbox_valid = true;
            info.west_lon_degree = c_locale_stod(row[5]);
            info.south_lat_degree = c_locale_stod(row[6]);
            info.east_lon_degree = c_locale_stod(row[7]);
            info.north_lat_degree = c_locale_stod(row[8]);
        }
        info.areaName = row[9];
        info.projectionMethodName = row[10];
        res.emplace_back(info);
    }
    return res;
}

// ---------------------------------------------------------------------------

/** \brief Gets the official name from a possibly alias name.
 *
 * @param aliasedName Alias name.
 * @param tableName Table name/category. Can help in case of ambiguities.
 * Or empty otherwise.
 * @param source Source of the alias. Can help in case of ambiguities.
 * Or empty otherwise.
 * @param tryEquivalentNameSpelling whether the comparison of aliasedName with
 * the alt_name column of the alis_name table should be done with using
 * metadata::Identifier::isEquivalentName() rather than strict string
 * comparison;
 * @param outTableName Table name in which the official name has been found.
 * @param outAuthName Authority name of the official name that has been found.
 * @param outCode Code of the official name that has been found.
 * @return official name (or empty if not found).
 * @throw FactoryException
 */
std::string AuthorityFactory::getOfficialNameFromAlias(
    const std::string &aliasedName, const std::string &tableName,
    const std::string &source, bool tryEquivalentNameSpelling,
    std::string &outTableName, std::string &outAuthName,
    std::string &outCode) const {

    if (tryEquivalentNameSpelling) {
        std::string sql(
            "SELECT table_name, auth_name, code, alt_name FROM alias_name");
        ListOfParams params;
        if (!tableName.empty()) {
            sql += " WHERE table_name = ?";
            params.push_back(tableName);
        }
        if (!source.empty()) {
            if (!tableName.empty()) {
                sql += " AND ";
            } else {
                sql += " WHERE ";
            }
            sql += "source = ?";
            params.push_back(source);
        }
        auto res = d->run(sql, params);
        if (res.empty()) {
            return std::string();
        }
        for (const auto &row : res) {
            const auto &alt_name = row[3];
            if (metadata::Identifier::isEquivalentName(alt_name.c_str(),
                                                       aliasedName.c_str())) {
                outTableName = row[0];
                outAuthName = row[1];
                outCode = row[2];
                sql = "SELECT name FROM \"";
                sql += replaceAll(outTableName, "\"", "\"\"");
                sql += "\" WHERE auth_name = ? AND code = ?";
                res = d->run(sql, {outAuthName, outCode});
                if (res.empty()) { // shouldn't happen normally
                    return std::string();
                }
                return res.front()[0];
            }
        }
        return std::string();
    } else {
        std::string sql(
            "SELECT table_name, auth_name, code FROM alias_name WHERE "
            "alt_name = ?");
        ListOfParams params{aliasedName};
        if (!tableName.empty()) {
            sql += " AND table_name = ?";
            params.push_back(tableName);
        }
        if (!source.empty()) {
            sql += " AND source = ?";
            params.push_back(source);
        }
        auto res = d->run(sql, params);
        if (res.empty()) {
            return std::string();
        }
        const auto &row = res.front();
        outTableName = row[0];
        outAuthName = row[1];
        outCode = row[2];
        sql = "SELECT name FROM \"";
        sql += replaceAll(outTableName, "\"", "\"\"");
        sql += "\" WHERE auth_name = ? AND code = ?";
        res = d->run(sql, {outAuthName, outCode});
        if (res.empty()) { // shouldn't happen normally
            return std::string();
        }
        return res.front()[0];
    }
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress

static void addToListString(std::string &out, const char *in) {
    if (!out.empty()) {
        out += ',';
    }
    out += in;
}

static void addToListStringWithOR(std::string &out, const char *in) {
    if (!out.empty()) {
        out += " OR ";
    }
    out += in;
}

//! @endcond

// ---------------------------------------------------------------------------

/** \brief Return a list of objects by their name
 *
 * @param searchedName Searched name. Must be at least 2 character long.
 * @param allowedObjectTypes List of object types into which to search. If
 * empty, all object types will be searched.
 * @param approximateMatch Whether approximate name identification is allowed.
 * @param limitResultCount Maximum number of results to return.
 * Or 0 for unlimited.
 * @return list of matched objects.
 * @throw FactoryException
 */
std::list<common::IdentifiedObjectNNPtr>
AuthorityFactory::createObjectsFromName(
    const std::string &searchedName,
    const std::vector<ObjectType> &allowedObjectTypes, bool approximateMatch,
    size_t limitResultCount) const {

    std::string searchedNameWithoutDeprecated(searchedName);
    bool deprecated = false;
    if (ends_with(searchedNameWithoutDeprecated, " (deprecated)")) {
        deprecated = true;
        searchedNameWithoutDeprecated.resize(
            searchedNameWithoutDeprecated.size() - strlen(" (deprecated)"));
    }

    const std::string canonicalizedSearchedName(
        metadata::Identifier::canonicalizeName(searchedNameWithoutDeprecated));
    if (canonicalizedSearchedName.size() <= 1) {
        return {};
    }

    std::string sql(
        "SELECT table_name, auth_name, code, name FROM object_view WHERE "
        "deprecated = ? AND ");
    ListOfParams params{deprecated ? 1.0 : 0.0};
    if (!approximateMatch) {
        sql += "name LIKE ? AND ";
        params.push_back(searchedNameWithoutDeprecated);
    }
    if (d->hasAuthorityRestriction()) {
        sql += " auth_name = ? AND ";
        params.emplace_back(d->authority());
    }

    if (allowedObjectTypes.empty()) {
        sql += "table_name IN ("
               "'prime_meridian','ellipsoid','geodetic_datum',"
               "'vertical_datum','geodetic_crs','projected_crs',"
               "'vertical_crs','compound_crs','conversion',"
               "'helmert_transformation','grid_transformation',"
               "'other_transformation','concatenated_operation'"
               ")";
    } else {
        std::string tableNameList;
        std::string otherConditions;
        for (const auto type : allowedObjectTypes) {
            switch (type) {
            case ObjectType::PRIME_MERIDIAN:
                addToListString(tableNameList, "'prime_meridian'");
                break;
            case ObjectType::ELLIPSOID:
                addToListString(tableNameList, "'ellipsoid'");
                break;
            case ObjectType::DATUM:
                addToListString(tableNameList,
                                "'geodetic_datum','vertical_datum'");
                break;
            case ObjectType::GEODETIC_REFERENCE_FRAME:
                addToListString(tableNameList, "'geodetic_datum'");
                break;
            case ObjectType::VERTICAL_REFERENCE_FRAME:
                addToListString(tableNameList, "'vertical_datum'");
                break;
            case ObjectType::CRS:
                addToListString(tableNameList, "'geodetic_crs','projected_crs',"
                                               "'vertical_crs','compound_crs'");
                break;
            case ObjectType::GEODETIC_CRS:
                addToListString(tableNameList, "'geodetic_crs'");
                break;
            case ObjectType::GEOCENTRIC_CRS:
                addToListStringWithOR(otherConditions,
                                      "(table_name = " GEOCENTRIC_SINGLE_QUOTED
                                      " AND "
                                      "type = " GEOCENTRIC_SINGLE_QUOTED ")");
                break;
            case ObjectType::GEOGRAPHIC_CRS:
                addToListStringWithOR(otherConditions,
                                      "(table_name = 'geodetic_crs' AND "
                                      "type IN (" GEOG_2D_SINGLE_QUOTED
                                      "," GEOG_3D_SINGLE_QUOTED "))");
                break;
            case ObjectType::GEOGRAPHIC_2D_CRS:
                addToListStringWithOR(otherConditions,
                                      "(table_name = 'geodetic_crs' AND "
                                      "type = " GEOG_2D_SINGLE_QUOTED ")");
                break;
            case ObjectType::GEOGRAPHIC_3D_CRS:
                addToListStringWithOR(otherConditions,
                                      "(table_name = 'geodetic_crs' AND "
                                      "type = " GEOG_3D_SINGLE_QUOTED ")");
                break;
            case ObjectType::PROJECTED_CRS:
                addToListString(tableNameList, "'projected_crs'");
                break;
            case ObjectType::VERTICAL_CRS:
                addToListString(tableNameList, "'vertical_crs'");
                break;
            case ObjectType::COMPOUND_CRS:
                addToListString(tableNameList, "'compound_crs'");
                break;
            case ObjectType::COORDINATE_OPERATION:
                addToListString(tableNameList,
                                "'conversion','helmert_transformation',"
                                "'grid_transformation','other_transformation',"
                                "'concatenated_operation'");
                break;
            case ObjectType::CONVERSION:
                addToListString(tableNameList, "'conversion'");
                break;
            case ObjectType::TRANSFORMATION:
                addToListString(tableNameList,
                                "'helmert_transformation',"
                                "'grid_transformation','other_transformation'");
                break;
            case ObjectType::CONCATENATED_OPERATION:
                addToListString(tableNameList, "'concatenated_operation'");
                break;
            }
        }
        if (!tableNameList.empty()) {
            sql += "((table_name IN (";
            sql += tableNameList;
            sql += "))";
            if (!otherConditions.empty()) {
                sql += " OR ";
                sql += otherConditions;
            }
            sql += ')';
        } else if (!otherConditions.empty()) {
            sql += "(";
            sql += otherConditions;
            sql += ')';
        }
    }
    sql += " ORDER BY length(name), name";
    if (limitResultCount > 0 &&
        limitResultCount <
            static_cast<size_t>(std::numeric_limits<int>::max()) &&
        !approximateMatch) {
        sql += " LIMIT ";
        sql += toString(static_cast<int>(limitResultCount));
    }

    std::list<common::IdentifiedObjectNNPtr> res;

    // Querying geodetic datum is a super hot path when importing from WKT1
    // so cache results.
    if (allowedObjectTypes.size() == 1 &&
        allowedObjectTypes[0] == ObjectType::GEODETIC_REFERENCE_FRAME &&
        approximateMatch && d->authority().empty()) {
        auto &mapCanonicalizeGRFName =
            d->context()->getPrivate()->getMapCanonicalizeGRFName();
        if (mapCanonicalizeGRFName.empty()) {
            auto sqlRes = d->run(sql, params);
            for (const auto &row : sqlRes) {
                const auto &name = row[3];
                const auto canonicalizedName(
                    metadata::Identifier::canonicalizeName(name));
                mapCanonicalizeGRFName[canonicalizedName].push_back(row);
            }
        }
        auto iter = mapCanonicalizeGRFName.find(canonicalizedSearchedName);
        if (iter != mapCanonicalizeGRFName.end()) {
            const auto &listOfRow = iter->second;
            for (const auto &row : listOfRow) {
                const auto &auth_name = row[1];
                const auto &code = row[2];
                auto factory = d->createFactory(auth_name);
                res.emplace_back(factory->createGeodeticDatum(code));
                if (limitResultCount > 0 && res.size() == limitResultCount) {
                    break;
                }
            }
        } else {
            for (const auto &pair : mapCanonicalizeGRFName) {
                const auto &listOfRow = pair.second;
                for (const auto &row : listOfRow) {
                    const auto &name = row[3];
                    if (approximateMatch) {
                        bool match =
                            ci_find(name, searchedNameWithoutDeprecated) !=
                            std::string::npos;
                        if (!match) {
                            const auto &canonicalizedName(pair.first);
                            match = ci_find(canonicalizedName,
                                            canonicalizedSearchedName) !=
                                    std::string::npos;
                        }
                        if (!match) {
                            continue;
                        }
                    }

                    const auto &auth_name = row[1];
                    const auto &code = row[2];
                    auto factory = d->createFactory(auth_name);
                    res.emplace_back(factory->createGeodeticDatum(code));
                    if (limitResultCount > 0 &&
                        res.size() == limitResultCount) {
                        break;
                    }
                }
                if (limitResultCount > 0 && res.size() == limitResultCount) {
                    break;
                }
            }
        }
    } else {
        auto sqlRes = d->run(sql, params);
        for (const auto &row : sqlRes) {
            const auto &name = row[3];
            if (approximateMatch) {
                bool match = ci_find(name, searchedNameWithoutDeprecated) !=
                             std::string::npos;
                if (!match) {
                    const auto canonicalizedName(
                        metadata::Identifier::canonicalizeName(name));
                    match =
                        ci_find(canonicalizedName, canonicalizedSearchedName) !=
                        std::string::npos;
                }
                if (!match) {
                    continue;
                }
            }
            const auto &table_name = row[0];
            const auto &auth_name = row[1];
            const auto &code = row[2];
            auto factory = d->createFactory(auth_name);
            if (table_name == "prime_meridian") {
                res.emplace_back(factory->createPrimeMeridian(code));
            } else if (table_name == "ellipsoid") {
                res.emplace_back(factory->createEllipsoid(code));
            } else if (table_name == "geodetic_datum") {
                res.emplace_back(factory->createGeodeticDatum(code));
            } else if (table_name == "vertical_datum") {
                res.emplace_back(factory->createVerticalDatum(code));
            } else if (table_name == "geodetic_crs") {
                res.emplace_back(factory->createGeodeticCRS(code));
            } else if (table_name == "projected_crs") {
                res.emplace_back(factory->createProjectedCRS(code));
            } else if (table_name == "vertical_crs") {
                res.emplace_back(factory->createVerticalCRS(code));
            } else if (table_name == "compound_crs") {
                res.emplace_back(factory->createCompoundCRS(code));
            } else if (table_name == "conversion") {
                res.emplace_back(factory->createConversion(code));
            } else if (table_name == "grid_transformation" ||
                       table_name == "helmert_transformation" ||
                       table_name == "other_transformation" ||
                       table_name == "concatenated_operation") {
                res.emplace_back(
                    factory->createCoordinateOperation(code, true));
            } else {
                assert(false);
            }
            if (limitResultCount > 0 && res.size() == limitResultCount) {
                break;
            }
        }
    }

    if (res.empty() && !deprecated) {
        return createObjectsFromName(searchedName + " (deprecated)",
                                     allowedObjectTypes, approximateMatch,
                                     limitResultCount);
    }

    auto sortLambda = [](const common::IdentifiedObjectNNPtr &a,
                         const common::IdentifiedObjectNNPtr &b) {
        const auto &aName = a->nameStr();
        const auto &bName = b->nameStr();
        if (aName.size() < bName.size()) {
            return true;
        }
        if (aName.size() > bName.size()) {
            return false;
        }

        const auto &aIds = a->identifiers();
        const auto &bIds = b->identifiers();
        if (aIds.size() < bIds.size()) {
            return true;
        }
        if (aIds.size() > bIds.size()) {
            return false;
        }
        for (size_t idx = 0; idx < aIds.size(); idx++) {
            const auto &aCodeSpace = *aIds[idx]->codeSpace();
            const auto &bCodeSpace = *bIds[idx]->codeSpace();
            const auto codeSpaceComparison = aCodeSpace.compare(bCodeSpace);
            if (codeSpaceComparison < 0) {
                return true;
            }
            if (codeSpaceComparison > 0) {
                return false;
            }
            const auto &aCode = aIds[idx]->code();
            const auto &bCode = bIds[idx]->code();
            const auto codeComparison = aCode.compare(bCode);
            if (codeComparison < 0) {
                return true;
            }
            if (codeComparison > 0) {
                return false;
            }
        }
        return strcmp(typeid(a.get()).name(), typeid(b.get()).name()) < 0;
    };

    res.sort(sortLambda);

    return res;
}

// ---------------------------------------------------------------------------

/** \brief Return a list of area of use from their name
 *
 * @param name Searched name.
 * @param approximateMatch Whether approximate name identification is allowed.
 * @return list of (auth_name, code) of matched objects.
 * @throw FactoryException
 */
std::list<std::pair<std::string, std::string>>
AuthorityFactory::listAreaOfUseFromName(const std::string &name,
                                        bool approximateMatch) const {
    std::string sql(
        "SELECT auth_name, code FROM area WHERE deprecated = 0 AND ");
    ListOfParams params;
    if (d->hasAuthorityRestriction()) {
        sql += " auth_name = ? AND ";
        params.emplace_back(d->authority());
    }
    sql += "name LIKE ?";
    if (!approximateMatch) {
        params.push_back(name);
    } else {
        params.push_back('%' + name + '%');
    }
    auto sqlRes = d->run(sql, params);
    std::list<std::pair<std::string, std::string>> res;
    for (const auto &row : sqlRes) {
        res.emplace_back(row[0], row[1]);
    }
    return res;
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
std::list<datum::EllipsoidNNPtr> AuthorityFactory::createEllipsoidFromExisting(
    const datum::EllipsoidNNPtr &ellipsoid) const {
    std::string sql(
        "SELECT auth_name, code FROM ellipsoid WHERE "
        "abs(semi_major_axis - ?) < 1e-10 * abs(semi_major_axis) AND "
        "((semi_minor_axis IS NOT NULL AND "
        "abs(semi_minor_axis - ?) < 1e-10 * abs(semi_minor_axis)) OR "
        "((inv_flattening IS NOT NULL AND "
        "abs(inv_flattening - ?) < 1e-10 * abs(inv_flattening))))");
    ListOfParams params{ellipsoid->semiMajorAxis().getSIValue(),
                        ellipsoid->computeSemiMinorAxis().getSIValue(),
                        ellipsoid->computedInverseFlattening()};
    auto sqlRes = d->run(sql, params);
    std::list<datum::EllipsoidNNPtr> res;
    for (const auto &row : sqlRes) {
        const auto &auth_name = row[0];
        const auto &code = row[1];
        res.emplace_back(d->createFactory(auth_name)->createEllipsoid(code));
    }
    return res;
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
std::list<crs::GeodeticCRSNNPtr> AuthorityFactory::createGeodeticCRSFromDatum(
    const std::string &datum_auth_name, const std::string &datum_code,
    const std::string &geodetic_crs_type) const {
    std::string sql(
        "SELECT auth_name, code FROM geodetic_crs WHERE "
        "datum_auth_name = ? AND datum_code = ? AND deprecated = 0");
    ListOfParams params{datum_auth_name, datum_code};
    if (d->hasAuthorityRestriction()) {
        sql += " AND auth_name = ?";
        params.emplace_back(d->authority());
    }
    if (!geodetic_crs_type.empty()) {
        sql += " AND type = ?";
        params.emplace_back(geodetic_crs_type);
    }
    auto sqlRes = d->run(sql, params);
    std::list<crs::GeodeticCRSNNPtr> res;
    for (const auto &row : sqlRes) {
        const auto &auth_name = row[0];
        const auto &code = row[1];
        res.emplace_back(d->createFactory(auth_name)->createGeodeticCRS(code));
    }
    return res;
}
//! @endcond

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
std::list<crs::GeodeticCRSNNPtr>
AuthorityFactory::createGeodeticCRSFromEllipsoid(
    const std::string &ellipsoid_auth_name, const std::string &ellipsoid_code,
    const std::string &geodetic_crs_type) const {
    std::string sql(
        "SELECT geodetic_crs.auth_name, geodetic_crs.code FROM geodetic_crs "
        "JOIN geodetic_datum ON "
        "geodetic_crs.datum_auth_name = geodetic_datum.auth_name AND "
        "geodetic_crs.datum_code = geodetic_datum.code WHERE "
        "geodetic_datum.ellipsoid_auth_name = ? AND "
        "geodetic_datum.ellipsoid_code = ? AND "
        "geodetic_datum.deprecated = 0 AND "
        "geodetic_crs.deprecated = 0");
    ListOfParams params{ellipsoid_auth_name, ellipsoid_code};
    if (d->hasAuthorityRestriction()) {
        sql += " AND geodetic_crs.auth_name = ?";
        params.emplace_back(d->authority());
    }
    if (!geodetic_crs_type.empty()) {
        sql += " AND geodetic_crs.type = ?";
        params.emplace_back(geodetic_crs_type);
    }
    auto sqlRes = d->run(sql, params);
    std::list<crs::GeodeticCRSNNPtr> res;
    for (const auto &row : sqlRes) {
        const auto &auth_name = row[0];
        const auto &code = row[1];
        res.emplace_back(d->createFactory(auth_name)->createGeodeticCRS(code));
    }
    return res;
}
//! @endcond

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
static std::string buildSqlLookForAuthNameCode(
    const std::list<std::pair<crs::CRSNNPtr, int>> &list, ListOfParams &params,
    const char *prefixField) {
    std::string sql("(");

    std::set<std::string> authorities;
    for (const auto &crs : list) {
        const auto &ids = crs.first->identifiers();
        if (!ids.empty()) {
            authorities.insert(*(ids[0]->codeSpace()));
        }
    }
    bool firstAuth = true;
    for (const auto &auth_name : authorities) {
        if (!firstAuth) {
            sql += " OR ";
        }
        firstAuth = false;
        sql += "( ";
        sql += prefixField;
        sql += "auth_name = ? AND ";
        sql += prefixField;
        sql += "code IN (";
        params.emplace_back(auth_name);
        bool firstGeodCRSForAuth = true;
        for (const auto &crs : list) {
            const auto &ids = crs.first->identifiers();
            if (!ids.empty() && *(ids[0]->codeSpace()) == auth_name) {
                if (!firstGeodCRSForAuth) {
                    sql += ',';
                }
                firstGeodCRSForAuth = false;
                sql += '?';
                params.emplace_back(ids[0]->code());
            }
        }
        sql += "))";
    }
    sql += ')';
    return sql;
}
//! @endcond

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
std::list<crs::ProjectedCRSNNPtr>
AuthorityFactory::createProjectedCRSFromExisting(
    const crs::ProjectedCRSNNPtr &crs) const {
    std::list<crs::ProjectedCRSNNPtr> res;

    const auto &conv = crs->derivingConversionRef();
    const auto &method = conv->method();
    const auto methodEPSGCode = method->getEPSGCode();
    if (methodEPSGCode == 0) {
        return res;
    }

    auto lockedThisFactory(d->getSharedFromThis());
    assert(lockedThisFactory);
    const auto &baseCRS(crs->baseCRS());
    auto candidatesGeodCRS = baseCRS->crs::CRS::identify(lockedThisFactory);
    auto geogCRS = dynamic_cast<const crs::GeographicCRS *>(baseCRS.get());
    if (geogCRS) {
        const auto axisOrder = geogCRS->coordinateSystem()->axisOrder();
        if (axisOrder == cs::EllipsoidalCS::AxisOrder::LONG_EAST_LAT_NORTH ||
            axisOrder == cs::EllipsoidalCS::AxisOrder::LAT_NORTH_LONG_EAST) {
            const auto &unit =
                geogCRS->coordinateSystem()->axisList()[0]->unit();
            auto otherOrderGeogCRS = crs::GeographicCRS::create(
                util::PropertyMap().set(common::IdentifiedObject::NAME_KEY,
                                        geogCRS->nameStr()),
                geogCRS->datum(), geogCRS->datumEnsemble(),
                axisOrder == cs::EllipsoidalCS::AxisOrder::LONG_EAST_LAT_NORTH
                    ? cs::EllipsoidalCS::createLatitudeLongitude(unit)
                    : cs::EllipsoidalCS::createLongitudeLatitude(unit));
            auto otherCandidatesGeodCRS =
                otherOrderGeogCRS->crs::CRS::identify(lockedThisFactory);
            candidatesGeodCRS.insert(candidatesGeodCRS.end(),
                                     otherCandidatesGeodCRS.begin(),
                                     otherCandidatesGeodCRS.end());
        }
    }

    std::string sql(
        "SELECT projected_crs.auth_name, projected_crs.code FROM projected_crs "
        "JOIN conversion_table conv ON "
        "projected_crs.conversion_auth_name = conv.auth_name AND "
        "projected_crs.conversion_code = conv.code WHERE "
        "projected_crs.deprecated = 0 AND ");
    ListOfParams params;
    if (!candidatesGeodCRS.empty()) {
        sql += buildSqlLookForAuthNameCode(candidatesGeodCRS, params,
                                           "projected_crs.geodetic_crs_");
        sql += " AND ";
    }
    sql += "conv.method_auth_name = 'EPSG' AND "
           "conv.method_code = ?";
    params.emplace_back(toString(methodEPSGCode));
    if (d->hasAuthorityRestriction()) {
        sql += " AND projected_crs.auth_name = ?";
        params.emplace_back(d->authority());
    }

    int iParam = 1;
    for (const auto &genOpParamvalue : conv->parameterValues()) {
        auto opParamvalue =
            dynamic_cast<const operation::OperationParameterValue *>(
                genOpParamvalue.get());
        if (!opParamvalue) {
            break;
        }
        const auto paramEPSGCode = opParamvalue->parameter()->getEPSGCode();
        const auto &parameterValue = opParamvalue->parameterValue();
        if (!(paramEPSGCode > 0 &&
              parameterValue->type() ==
                  operation::ParameterValue::Type::MEASURE)) {
            break;
        }
        const auto &measure = parameterValue->value();
        const auto &unit = measure.unit();
        if (unit == common::UnitOfMeasure::DEGREE &&
            geogCRS->coordinateSystem()->axisList()[0]->unit() == unit) {
            const auto iParamAsStr(toString(iParam));
            sql += " AND conv.param";
            sql += iParamAsStr;
            sql += "_code = ? AND conv.param";
            sql += iParamAsStr;
            sql += "_auth_name = 'EPSG' AND conv.param";
            sql += iParamAsStr;
            sql += "_value BETWEEN ? AND ?";
            // As angles might be expressed with the odd unit EPSG:9110
            // "sexagesimal DMS", we have to provide a broad range
            params.emplace_back(toString(paramEPSGCode));
            params.emplace_back(measure.value() - 1);
            params.emplace_back(measure.value() + 1);
        }
        iParam++;
    }
    auto sqlRes = d->run(sql, params);

    params.clear();

    sql = "SELECT auth_name, code FROM projected_crs WHERE "
          "deprecated = 0 AND conversion_auth_name IS NULL AND ";
    if (!candidatesGeodCRS.empty()) {
        sql += buildSqlLookForAuthNameCode(candidatesGeodCRS, params,
                                           "geodetic_crs_");
        sql += " AND ";
    }

    const auto escapeLikeStr = [](const std::string &str) {
        return replaceAll(replaceAll(replaceAll(str, "\\", "\\\\"), "_", "\\_"),
                          "%", "\\%");
    };

    const auto ellpsSemiMajorStr =
        toString(baseCRS->ellipsoid()->semiMajorAxis().getSIValue(), 10);

    sql += "(text_definition LIKE ? ESCAPE '\\'";

    // WKT2 definition
    {
        std::string patternVal("%");

        patternVal += ',';
        patternVal += ellpsSemiMajorStr;
        patternVal += '%';

        patternVal += escapeLikeStr(method->nameStr());
        patternVal += '%';

        params.emplace_back(patternVal);
    }

    const auto *mapping = getMapping(method.get());
    if (mapping && mapping->proj_name_main) {
        sql += " OR (text_definition LIKE ? AND (text_definition LIKE ?";

        std::string patternVal("%");
        patternVal += "proj=";
        patternVal += mapping->proj_name_main;
        patternVal += '%';
        params.emplace_back(patternVal);

        // could be a= or R=
        patternVal = "%=";
        patternVal += ellpsSemiMajorStr;
        patternVal += '%';
        params.emplace_back(patternVal);

        std::string projEllpsName;
        std::string ellpsName;
        if (baseCRS->ellipsoid()->lookForProjWellKnownEllps(projEllpsName,
                                                            ellpsName)) {
            sql += " OR text_definition LIKE ?";
            // Could be ellps= or datum=
            patternVal = "%=";
            patternVal += projEllpsName;
            patternVal += '%';
            params.emplace_back(patternVal);
        }

        sql += "))";
    }

    // WKT1_GDAL definition
    const char *wkt1GDALMethodName = conv->getWKT1GDALMethodName();
    if (wkt1GDALMethodName) {
        sql += " OR text_definition LIKE ? ESCAPE '\\'";
        std::string patternVal("%");

        patternVal += ',';
        patternVal += ellpsSemiMajorStr;
        patternVal += '%';

        patternVal += escapeLikeStr(wkt1GDALMethodName);
        patternVal += '%';

        params.emplace_back(patternVal);
    }

    // WKT1_ESRI definition
    const char *esriMethodName = conv->getESRIMethodName();
    if (esriMethodName) {
        sql += " OR text_definition LIKE ? ESCAPE '\\'";
        std::string patternVal("%");

        patternVal += ',';
        patternVal += ellpsSemiMajorStr;
        patternVal += '%';

        patternVal += escapeLikeStr(esriMethodName);
        patternVal += '%';

        auto fe =
            &conv->parameterValueMeasure(EPSG_CODE_PARAMETER_FALSE_EASTING);
        if (*fe == Measure()) {
            fe = &conv->parameterValueMeasure(
                EPSG_CODE_PARAMETER_EASTING_FALSE_ORIGIN);
        }
        if (!(*fe == Measure())) {
            patternVal += "PARAMETER[\"False\\_Easting\",";
            patternVal +=
                toString(fe->convertToUnit(
                             crs->coordinateSystem()->axisList()[0]->unit()),
                         10);
            patternVal += '%';
        }

        auto lat = &conv->parameterValueMeasure(
            EPSG_NAME_PARAMETER_LATITUDE_OF_NATURAL_ORIGIN);
        if (*lat == Measure()) {
            lat = &conv->parameterValueMeasure(
                EPSG_NAME_PARAMETER_LATITUDE_FALSE_ORIGIN);
        }
        if (!(*lat == Measure())) {
            patternVal += "PARAMETER[\"Latitude\\_Of\\_Origin\",";
            const auto &angularUnit =
                dynamic_cast<crs::GeographicCRS *>(crs->baseCRS().get())
                    ? crs->baseCRS()->coordinateSystem()->axisList()[0]->unit()
                    : UnitOfMeasure::DEGREE;
            patternVal += toString(lat->convertToUnit(angularUnit), 10);
            patternVal += '%';
        }

        params.emplace_back(patternVal);
    }
    sql += ")";
    if (d->hasAuthorityRestriction()) {
        sql += " AND auth_name = ?";
        params.emplace_back(d->authority());
    }

    auto sqlRes2 = d->run(sql, params);

    if (sqlRes.size() <= 200) {
        for (const auto &row : sqlRes) {
            const auto &auth_name = row[0];
            const auto &code = row[1];
            res.emplace_back(
                d->createFactory(auth_name)->createProjectedCRS(code));
        }
    }
    if (sqlRes2.size() <= 200) {
        for (const auto &row : sqlRes2) {
            const auto &auth_name = row[0];
            const auto &code = row[1];
            res.emplace_back(
                d->createFactory(auth_name)->createProjectedCRS(code));
        }
    }

    return res;
}

// ---------------------------------------------------------------------------

std::list<crs::CompoundCRSNNPtr>
AuthorityFactory::createCompoundCRSFromExisting(
    const crs::CompoundCRSNNPtr &crs) const {
    std::list<crs::CompoundCRSNNPtr> res;

    auto lockedThisFactory(d->getSharedFromThis());
    assert(lockedThisFactory);

    const auto &components = crs->componentReferenceSystems();
    if (components.size() != 2) {
        return res;
    }
    auto candidatesHorizCRS = components[0]->identify(lockedThisFactory);
    auto candidatesVertCRS = components[1]->identify(lockedThisFactory);
    if (candidatesHorizCRS.empty() && candidatesVertCRS.empty()) {
        return res;
    }

    std::string sql("SELECT auth_name, code FROM compound_crs WHERE "
                    "deprecated = 0 AND ");
    ListOfParams params;
    bool addAnd = false;
    if (!candidatesHorizCRS.empty()) {
        sql += buildSqlLookForAuthNameCode(candidatesHorizCRS, params,
                                           "horiz_crs_");
        addAnd = true;
    }
    if (!candidatesVertCRS.empty()) {
        if (addAnd) {
            sql += " AND ";
        }
        sql += buildSqlLookForAuthNameCode(candidatesVertCRS, params,
                                           "vertical_crs_");
        addAnd = true;
    }
    if (d->hasAuthorityRestriction()) {
        if (addAnd) {
            sql += " AND ";
        }
        sql += "auth_name = ?";
        params.emplace_back(d->authority());
    }

    auto sqlRes = d->run(sql, params);
    for (const auto &row : sqlRes) {
        const auto &auth_name = row[0];
        const auto &code = row[1];
        res.emplace_back(d->createFactory(auth_name)->createCompoundCRS(code));
    }
    return res;
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
FactoryException::FactoryException(const char *message) : Exception(message) {}

// ---------------------------------------------------------------------------

FactoryException::FactoryException(const std::string &message)
    : Exception(message) {}

// ---------------------------------------------------------------------------

FactoryException::~FactoryException() = default;

// ---------------------------------------------------------------------------

FactoryException::FactoryException(const FactoryException &) = default;
//! @endcond

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress

struct NoSuchAuthorityCodeException::Private {
    std::string authority_;
    std::string code_;

    Private(const std::string &authority, const std::string &code)
        : authority_(authority), code_(code) {}
};

// ---------------------------------------------------------------------------

NoSuchAuthorityCodeException::NoSuchAuthorityCodeException(
    const std::string &message, const std::string &authority,
    const std::string &code)
    : FactoryException(message),
      d(internal::make_unique<Private>(authority, code)) {}

// ---------------------------------------------------------------------------

NoSuchAuthorityCodeException::~NoSuchAuthorityCodeException() = default;

// ---------------------------------------------------------------------------

NoSuchAuthorityCodeException::NoSuchAuthorityCodeException(
    const NoSuchAuthorityCodeException &other)
    : FactoryException(other), d(internal::make_unique<Private>(*(other.d))) {}
//! @endcond

// ---------------------------------------------------------------------------

/** \brief Returns authority name. */
const std::string &NoSuchAuthorityCodeException::getAuthority() const {
    return d->authority_;
}

// ---------------------------------------------------------------------------

/** \brief Returns authority code. */
const std::string &NoSuchAuthorityCodeException::getAuthorityCode() const {
    return d->code_;
}

// ---------------------------------------------------------------------------

} // namespace io
NS_PROJ_END
