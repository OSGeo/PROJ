/******************************************************************************
 *
 * Project:  PROJ
 * Purpose:  ISO19111:2018 implementation
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
#include <map>
#include <memory>
#include <sstream> // std::ostringstream
#include <string>

// PROJ include order is sensitive
// clang-format off
#include "proj.h"
#include "projects.h"
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
using SQLResultSet = std::vector<SQLRow>;

// ---------------------------------------------------------------------------

struct DatabaseContext::Private {
    Private();
    ~Private();

    void open(const std::string &databasePath = std::string());
    void setHandle(sqlite3 *sqlite_handle);

    sqlite3 *handle() const { return sqlite_handle_; }

    PJ_CONTEXT *pjCtxt() const { return pjCtxt_; }
    void setPjCtxt(PJ_CONTEXT *ctxt) { pjCtxt_ = ctxt; }

    SQLResultSet
    run(const std::string &sql,
        const std::vector<SQLValues> &parameters = std::vector<SQLValues>());

    std::vector<std::string> getDatabaseStructure();

    // cppcheck-suppress functionStatic
    const std::string &getPath() const { return databasePath_; }

    void attachExtraDatabases(
        const std::vector<std::string> &auxiliaryDatabasePaths);

    // Mechanism to detect recursion in calls from
    // AuthorityFactory::createXXX() -> createFromUserInput() ->
    // AuthorityFacotry::createXXX()
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

  private:
    std::string databasePath_{};
    bool close_handle_ = true;
    sqlite3 *sqlite_handle_{};
    std::map<std::string, sqlite3_stmt *> mapSqlToStatement_{};
    PJ_CONTEXT *pjCtxt_ = nullptr;
    int recLevel_ = 0;
    bool detach_ = false;

    void closeDB();

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

void DatabaseContext::Private::closeDB() {

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
        run("DETACH DATABASE db_0");
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

void DatabaseContext::Private::open(const std::string &databasePath) {
    std::string path(databasePath);
    if (path.empty()) {
        const char *proj_lib = std::getenv("PROJ_LIB");
#ifdef PROJ_LIB
        if (!proj_lib) {
            proj_lib = PROJ_LIB;
        }
#endif
        if (!proj_lib) {
            throw FactoryException(
                "Cannot find proj.db due to missing PROJ_LIB");
        }
        path = std::string(proj_lib) + DIR_CHAR + "proj.db";
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
    auto sqlRes = run("SELECT sql FROM sqlite_master WHERE type "
                      "IN ('table', 'trigger', 'view') ORDER BY type");
    std::vector<std::string> res;
    for (const auto &row : sqlRes) {
        res.emplace_back(row[0]);
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

SQLResultSet
DatabaseContext::Private::run(const std::string &sql,
                              const std::vector<SQLValues> &parameters) {

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
            SQLRow row;
            for (int i = 0; i < column_count; i++) {
                const char *txt = reinterpret_cast<const char *>(
                    sqlite3_column_text(stmt, i));
                row.emplace_back(txt ? txt : std::string());
            }
            result.emplace_back(row);
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

/** \brief Instanciate a database context, using the default proj.db file
 *
 * It will be searched in the directory pointed by the PROJ_LIB environment
 * variable. If not found, on Unix builds, it will be then searched first in
 * the pkgdatadir directory of the installation prefix.
 *
 * This database context should be used only by one thread at a time.
 * @throw FactoryException
 */
DatabaseContextNNPtr DatabaseContext::create() {
    return create(std::string(), {});
}

// ---------------------------------------------------------------------------

/** \brief Instanciate a database context from a full filename.
 *
 * This database context should be used only by one thread at a time.
 * @param databasePath Path and filename of the database. Might be empty
 * string for the default rules to locate the default proj.db
 * @throw FactoryException
 */
DatabaseContextNNPtr DatabaseContext::create(const std::string &databasePath) {
    return create(databasePath, {});
}

// ---------------------------------------------------------------------------

/** \brief Instanciate a database context from a full filename, and attach
 * auxiliary databases to it.
 *
 * This database context should be used only by one thread at a time.
 * @param databasePath Path and filename of the database. Might be empty
 * string for the default rules to locate the default proj.db
 * @param auxiliaryDatabasePaths Path and filename of auxiliary databases;
 * @throw FactoryException
 */
DatabaseContextNNPtr DatabaseContext::create(
    const std::string &databasePath,
    const std::vector<std::string> &auxiliaryDatabasePaths) {
    auto ctxt = DatabaseContext::nn_make_shared<DatabaseContext>();
    ctxt->getPrivate()->open(databasePath);
    if (!auxiliaryDatabasePaths.empty()) {
        ctxt->getPrivate()->attachExtraDatabases(auxiliaryDatabasePaths);
    }
    return ctxt;
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

void DatabaseContext::attachPJContext(void *pjCtxt) {
    d->setPjCtxt(static_cast<PJ_CONTEXT *>(pjCtxt));
}

// ---------------------------------------------------------------------------

bool DatabaseContext::lookForGridAlternative(const std::string &officialName,
                                             std::string &projFilename,
                                             std::string &projFormat,
                                             bool &inverse) {
    auto res = d->run(
        "SELECT proj_grid_name, proj_grid_format, inverse_direction FROM "
        "grid_alternatives WHERE original_grid_name = ?",
        {officialName});
    if (res.empty()) {
        return false;
    }
    projFilename = res[0][0];
    projFormat = res[0][1];
    inverse = res[0][2] == "1";
    return true;
}

// ---------------------------------------------------------------------------

bool DatabaseContext::lookForGridInfo(const std::string &projFilename,
                                      std::string &fullFilename,
                                      std::string &packageName,
                                      std::string &url, bool &directDownload,
                                      bool &openLicense, bool &gridAvailable) {
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
    if (res.empty()) {
        return false;
    }
    packageName = std::move(res[0][0]);
    url = res[0][1].empty() ? std::move(res[0][2]) : std::move(res[0][1]);
    openLicense = (res[0][3].empty() ? res[0][4] : res[0][3]) == "1";
    directDownload = (res[0][5].empty() ? res[0][6] : res[0][5]) == "1";
    return true;
}

//! @endcond

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
struct AuthorityFactory::Private {
    Private(const DatabaseContextNNPtr &contextIn,
            const std::string &authorityName)
        : context_(contextIn), authority_(authorityName) {}

    inline const std::string &authority() PROJ_CONST_DEFN { return authority_; }

    inline DatabaseContextNNPtr context() PROJ_CONST_DEFN { return context_; }

    // cppcheck-suppress functionStatic
    void setThis(AuthorityFactoryNNPtr factory) {
        thisFactory_ = factory.as_nullable();
    }

    AuthorityFactoryNNPtr createFactory(const std::string &auth_name);

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

    SQLResultSet
    run(const std::string &sql,
        const std::vector<SQLValues> &parameters = std::vector<SQLValues>());

    SQLResultSet runWithCodeParam(const std::string &sql,
                                  const std::string &code);

    SQLResultSet runWithCodeParam(const char *sql, const std::string &code);

  private:
    DatabaseContextNNPtr context_;
    std::string authority_;
    std::weak_ptr<AuthorityFactory> thisFactory_{};
    std::weak_ptr<AuthorityFactory> parentFactory_{};
    std::map<std::string, AuthorityFactoryNNPtr> mapFactory_{};
    lru11::Cache<std::string, util::BaseObjectPtr> cacheUOM_{};
    lru11::Cache<std::string, util::BaseObjectPtr> cacheCRS_{};
    lru11::Cache<std::string, util::BaseObjectPtr> cacheGeodeticDatum_{};

    static void
    insertIntoCache(lru11::Cache<std::string, util::BaseObjectPtr> &cache,
                    const std::string &code, const util::BaseObjectPtr &obj);

    static void
    getFromCache(lru11::Cache<std::string, util::BaseObjectPtr> &cache,
                 const std::string &code, util::BaseObjectPtr &obj);
};

// ---------------------------------------------------------------------------

AuthorityFactoryNNPtr
AuthorityFactory::Private::createFactory(const std::string &auth_name) {

    // If we are a child factory, then create new factory on the parent
    auto parentFactoryLocked(parentFactory_.lock());
    if (parentFactoryLocked) {
        return parentFactoryLocked->d->createFactory(auth_name);
    }

    // If asked for a factory with our name, return ourselves.
    auto lockedThisFactory(thisFactory_.lock());
    assert(lockedThisFactory);
    if (auth_name == lockedThisFactory->getAuthority()) {
        return NN_NO_CHECK(lockedThisFactory);
    }

    // Find if there is already a child factory with the passed name.
    auto iter = mapFactory_.find(auth_name);
    if (iter == mapFactory_.end()) {
        auto newFactory = AuthorityFactory::create(context_, auth_name);
        newFactory->d->parentFactory_ = thisFactory_;
        mapFactory_.insert(std::pair<std::string, AuthorityFactoryNNPtr>(
            auth_name, newFactory));
        return newFactory;
    }
    return iter->second;
}

// ---------------------------------------------------------------------------

SQLResultSet
AuthorityFactory::Private::run(const std::string &sql,
                               const std::vector<SQLValues> &parameters) {
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
                     .set(common::IdentifiedObject::NAME_KEY, name)
                     .set(common::IdentifiedObject::DEPRECATED_KEY, deprecated);
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

void AuthorityFactory::Private::insertIntoCache(
    lru11::Cache<std::string, util::BaseObjectPtr> &cache,
    const std::string &code, const util::BaseObjectPtr &obj) {
    cache.insert(code, obj);
}

// ---------------------------------------------------------------------------

void AuthorityFactory::Private::getFromCache(
    lru11::Cache<std::string, util::BaseObjectPtr> &cache,
    const std::string &code, util::BaseObjectPtr &obj) {
    cache.tryGet(code, obj);
}

// ---------------------------------------------------------------------------

crs::CRSPtr
AuthorityFactory::Private::getCRSFromCache(const std::string &code) {
    util::BaseObjectPtr obj;
    getFromCache(cacheCRS_, code, obj);
    return std::static_pointer_cast<crs::CRS>(obj);
}

// ---------------------------------------------------------------------------

void AuthorityFactory::Private::cache(const std::string &code,
                                      const crs::CRSNNPtr &crs) {
    insertIntoCache(cacheCRS_, code, crs.as_nullable());
}

// ---------------------------------------------------------------------------

common::UnitOfMeasurePtr
AuthorityFactory::Private::getUOMFromCache(const std::string &code) {
    util::BaseObjectPtr obj;
    getFromCache(cacheUOM_, code, obj);
    return std::static_pointer_cast<common::UnitOfMeasure>(obj);
}

// ---------------------------------------------------------------------------

void AuthorityFactory::Private::cache(const std::string &code,
                                      const common::UnitOfMeasureNNPtr &uom) {
    insertIntoCache(cacheUOM_, code, uom.as_nullable());
}

// ---------------------------------------------------------------------------

datum::GeodeticReferenceFramePtr
AuthorityFactory::Private::getGeodeticDatumFromCache(const std::string &code) {
    util::BaseObjectPtr obj;
    getFromCache(cacheGeodeticDatum_, code, obj);
    return std::static_pointer_cast<datum::GeodeticReferenceFrame>(obj);
}

// ---------------------------------------------------------------------------

void AuthorityFactory::Private::cache(
    const std::string &code, const datum::GeodeticReferenceFrameNNPtr &datum) {
    insertIntoCache(cacheGeodeticDatum_, code, datum.as_nullable());
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
/** \brief Instanciate a AuthorityFactory.
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
AuthorityFactory::create(DatabaseContextNNPtr context,
                         const std::string &authorityName) {
    auto factory = AuthorityFactory::nn_make_shared<AuthorityFactory>(
        context, authorityName);
    factory->d->setThis(factory);
    return factory;
}

// ---------------------------------------------------------------------------

/** \brief Returns the database context. */
DatabaseContextNNPtr AuthorityFactory::databaseContext() const {
    return d->context();
}

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
        throw NoSuchAuthorityCodeException("not found", getAuthority(), code);
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
    const auto &table_name = res[0][0];
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
    throw FactoryException("unimplemented factory for " + res[0][0]);
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
    auto sql = "SELECT name, south_lat, north_lat, west_lon, east_lon, "
               "deprecated FROM area WHERE auth_name = ? AND code = ?";
    auto res = d->runWithCodeParam(sql, code);
    if (res.empty()) {
        throw NoSuchAuthorityCodeException("area not found", getAuthority(),
                                           code);
    }
    try {
        const auto &row = res[0];
        const auto &name = row[0];
        double south_lat = c_locale_stod(row[1]);
        double north_lat = c_locale_stod(row[2]);
        double west_lon = c_locale_stod(row[3]);
        double east_lon = c_locale_stod(row[4]);
        auto bbox = metadata::GeographicBoundingBox::create(
            west_lon, south_lat, east_lon, north_lat);

        return metadata::Extent::create(
            util::optional<std::string>(name),
            std::vector<metadata::GeographicExtentNNPtr>{bbox},
            std::vector<metadata::VerticalExtentNNPtr>(),
            std::vector<metadata::TemporalExtentNNPtr>());

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
    {
        auto uom = d->getUOMFromCache(code);
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
                                           getAuthority(), code);
    }
    try {
        const auto &row = res[0];
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
            name, conv_factor, unitType, getAuthority(), code);
        d->cache(code, uom);
        return uom;
    } catch (const std::exception &ex) {
        throw buildFactoryException("unit of measure", code, ex);
    }
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
static void normalizeMeasure(const std::string &uom_code,
                             const std::string &value,
                             std::string &normalized_uom_code,
                             double &normalized_value) {
    normalized_uom_code = uom_code;
    normalized_value = c_locale_stod(value);
    if (uom_code == "9110") // DDD.MMSSsss
    {
        std::ostringstream buffer;
        buffer.imbue(std::locale::classic());
        buffer << std::fixed << std::setprecision(7) << normalized_value;
        auto formatted = buffer.str();
        size_t dotPos = formatted.find('.');
        assert(dotPos + 1 + 7 == formatted.size());
        auto MM = formatted.substr(dotPos + 1, 2);
        auto SSsss = formatted.substr(dotPos + 3, 2 + 3);
        normalized_value =
            (normalized_value < 0 ? -1.0 : 1.0) *
            (int(std::fabs(normalized_value)) + c_locale_stod(MM) / 60. +
             (c_locale_stod(SSsss) / 1000.0) / 3600.);
        normalized_uom_code = common::UnitOfMeasure::DEGREE.code();
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
    auto res = d->runWithCodeParam(
        "SELECT name, longitude, uom_auth_name, uom_code, deprecated FROM "
        "prime_meridian WHERE "
        "auth_name = ? AND code = ?",
        code);
    if (res.empty()) {
        throw NoSuchAuthorityCodeException("prime meridian not found",
                                           getAuthority(), code);
    }
    try {
        const auto &row = res[0];
        const auto &name = row[0];
        const auto &longitude = row[1];
        const auto &uom_auth_name = row[2];
        const auto &uom_code = row[3];
        const bool deprecated = row[4] == "1";

        std::string normalized_uom_code(uom_code);
        double normalized_value(c_locale_stod(longitude));
        normalizeMeasure(uom_code, longitude, normalized_uom_code,
                         normalized_value);

        auto uom = d->createUnitOfMeasure(uom_auth_name, normalized_uom_code);
        auto props = d->createProperties(code, name, deprecated, nullptr);
        return datum::PrimeMeridian::create(
            props, common::Angle(normalized_value, uom));
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
    return res[0][0];
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
                                           getAuthority(), code);
    }
    try {
        const auto &row = res[0];
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
        throw buildFactoryException("elllipsoid", code, ex);
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
    {
        auto datum = d->getGeodeticDatumFromCache(code);
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
                                           getAuthority(), code);
    }
    try {
        const auto &row = res[0];
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
        d->cache(code, datum);
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
                                           getAuthority(), code);
    }
    try {
        const auto &row = res[0];
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
               {getAuthority(), code, getAuthority(), code});
    if (res.empty()) {
        throw NoSuchAuthorityCodeException("datum not found", getAuthority(),
                                           code);
    }
    if (res[0][0] == "geodetic_datum") {
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
                                           getAuthority(), code);
    }

    const auto &csType = res[0][5];
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
    auto props = util::PropertyMap()
                     .set(metadata::Identifier::CODESPACE_KEY, getAuthority())
                     .set(metadata::Identifier::CODE_KEY, code);
    if (csType == "ellipsoidal") {
        if (axisList.size() == 2) {
            return cs::EllipsoidalCS::create(props, axisList[0], axisList[1]);
        }
        if (axisList.size() == 3) {
            return cs::EllipsoidalCS::create(props, axisList[0], axisList[1],
                                             axisList[2]);
        }
        throw FactoryException("invalid number of axis for EllipsoidalCS");
    }
    if (csType == "Cartesian") {
        if (axisList.size() == 2) {
            return cs::CartesianCS::create(props, axisList[0], axisList[1]);
        }
        if (axisList.size() == 3) {
            return cs::CartesianCS::create(props, axisList[0], axisList[1],
                                           axisList[2]);
        }
        throw FactoryException("invalid number of axis for CartesianCS");
    }
    if (csType == "vertical") {
        if (axisList.size() == 1) {
            return cs::VerticalCS::create(props, axisList[0]);
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
    auto crs =
        std::dynamic_pointer_cast<crs::GeodeticCRS>(d->getCRSFromCache(code));
    if (crs) {
        return NN_NO_CHECK(crs);
    }
    std::string sql("SELECT name, type, coordinate_system_auth_name, "
                    "coordinate_system_code, datum_auth_name, datum_code, "
                    "area_of_use_auth_name, area_of_use_code, text_definition, "
                    "deprecated FROM "
                    "geodetic_crs WHERE auth_name = ? AND code = ?");
    if (geographicOnly) {
        sql += " AND type in ('geographic 2D', 'geographic 3D')";
    }
    auto res = d->runWithCodeParam(sql, code);
    if (res.empty()) {
        throw NoSuchAuthorityCodeException("geodeticCRS not found",
                                           getAuthority(), code);
    }
    try {
        const auto &row = res[0];
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
            auto obj = createFromUserInput(text_definition, d->context());
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
        if ((type == "geographic 2D" || type == "geographic 3D") &&
            ellipsoidalCS) {
            auto crsRet = crs::GeographicCRS::create(
                props, datum, NN_NO_CHECK(ellipsoidalCS));
            d->cache(code, crsRet);
            return crsRet;
        }
        auto geocentricCS = util::nn_dynamic_pointer_cast<cs::CartesianCS>(cs);
        if (type == "geocentric" && geocentricCS) {
            auto crsRet = crs::GeodeticCRS::create(props, datum,
                                                   NN_NO_CHECK(geocentricCS));
            d->cache(code, crsRet);
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
                                           getAuthority(), code);
    }
    try {
        const auto &row = res[0];
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
    std::ostringstream buffer;
    buffer.imbue(std::locale::classic());
    buffer << "SELECT name, area_of_use_auth_name, area_of_use_code, "
              "method_auth_name, method_code, method_name";
    constexpr int N_MAX_PARAMS = 7;
    for (int i = 1; i <= N_MAX_PARAMS; ++i) {
        buffer << ", param" << i << "_auth_name";
        buffer << ", param" << i << "_code";
        buffer << ", param" << i << "_name";
        buffer << ", param" << i << "_value";
        buffer << ", param" << i << "_uom_auth_name";
        buffer << ", param" << i << "_uom_code";
    }
    buffer << ", deprecated FROM conversion WHERE auth_name = ? AND code = ?";

    auto res = d->runWithCodeParam(buffer.str(), code);
    if (res.empty()) {
        throw NoSuchAuthorityCodeException("conversion not found",
                                           getAuthority(), code);
    }
    try {
        const auto &row = res[0];
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
            double normalized_value(c_locale_stod(param_value));
            normalizeMeasure(param_uom_code, param_value, normalized_uom_code,
                             normalized_value);
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
                                           getAuthority(), code);
    }
    try {
        const auto &row = res[0];
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
            auto obj = createFromUserInput(text_definition, d->context());
            auto projCRS = dynamic_cast<const crs::ProjectedCRS *>(obj.get());
            if (projCRS) {
                return crs::ProjectedCRS::create(props, projCRS->baseCRS(),
                                                 projCRS->derivingConversion(),
                                                 projCRS->coordinateSystem());
            }

            auto boundCRS = dynamic_cast<const crs::BoundCRS *>(obj.get());
            if (boundCRS) {
                projCRS = dynamic_cast<const crs::ProjectedCRS *>(
                    boundCRS->baseCRS().get());
                if (projCRS) {
                    auto newBoundCRS = crs::BoundCRS::create(
                        crs::ProjectedCRS::create(props, projCRS->baseCRS(),
                                                  projCRS->derivingConversion(),
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
                                           getAuthority(), code);
    }
    try {
        const auto &row = res[0];
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

crs::CRSNNPtr
AuthorityFactory::createCoordinateReferenceSystem(const std::string &code,
                                                  bool allowCompound) const {
    auto crs = d->getCRSFromCache(code);
    if (crs) {
        return NN_NO_CHECK(crs);
    }
    auto res = d->runWithCodeParam(
        "SELECT type FROM crs_view WHERE auth_name = ? AND code = ?", code);
    if (res.empty()) {
        throw NoSuchAuthorityCodeException("crs not found", getAuthority(),
                                           code);
    }
    const auto &type = res[0][0];
    if (type == "geographic 2D" || type == "geographic 3D" ||
        type == "geocentric") {
        return createGeodeticCRS(code);
    }
    if (type == "vertical") {
        return createVerticalCRS(code);
    }
    if (type == "projected") {
        return createProjectedCRS(code);
    }
    if (allowCompound && type == "compound") {
        return createCompoundCRS(code);
    }
    throw FactoryException("unhandled CRS type: " + type);
}
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

static operation::OperationParameterNNPtr
createOpParamNameEPSGCode(const std::string &name, int code) {
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
    return createCoordinateOperation(code, true, usePROJAlternativeGridNames);
}

operation::CoordinateOperationNNPtr AuthorityFactory::createCoordinateOperation(
    const std::string &code, bool allowConcatenated,
    bool usePROJAlternativeGridNames) const {
    auto res = d->runWithCodeParam(
        "SELECT type FROM coordinate_operation_with_conversion_view "
        "WHERE auth_name = ? AND code = ?",
        code);
    if (res.empty()) {
        throw NoSuchAuthorityCodeException("coordinate operation not found",
                                           getAuthority(), code);
    }
    const auto type = res[0][0];
    if (type == "conversion") {
        return createConversion(code);
    }

    if (type == "helmert_transformation") {

#include "proj/internal/helmert_constants.hpp"

        res = d->runWithCodeParam(
            "SELECT name, method_auth_name, method_code, method_name, "
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
            "epoch_uom_code, deprecated FROM "
            "helmert_transformation WHERE auth_name = ? AND code = ?",
            code);
        if (res.empty()) {
            // shouldn't happen if foreign keys are OK
            throw NoSuchAuthorityCodeException(
                "helmert_transformation not found", getAuthority(), code);
        }
        try {
            const auto &row = res[0];
            size_t idx = 0;
            const auto &name = row[idx++];
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

            const auto &deprecated_str = row[idx++];
            const bool deprecated = deprecated_str == "1";
            assert(idx == row.size());

            auto uom_translation = d->createUnitOfMeasure(
                translation_uom_auth_name, translation_uom_code);
            auto uom_rotation =
                rotation_uom_auth_name.empty()
                    ? common::UnitOfMeasure::NONE
                    : d->createUnitOfMeasure(rotation_uom_auth_name,
                                             rotation_uom_code);
            auto uom_scale_difference =
                scale_difference_uom_auth_name.empty()
                    ? common::UnitOfMeasure::NONE
                    : d->createUnitOfMeasure(scale_difference_uom_auth_name,
                                             scale_difference_uom_code);
            auto uom_rate_translation =
                rate_translation_uom_auth_name.empty()
                    ? common::UnitOfMeasure::NONE
                    : d->createUnitOfMeasure(rate_translation_uom_auth_name,
                                             rate_translation_uom_code);
            auto uom_rate_rotation =
                rate_rotation_uom_auth_name.empty()
                    ? common::UnitOfMeasure::NONE
                    : d->createUnitOfMeasure(rate_rotation_uom_auth_name,
                                             rate_rotation_uom_code);
            auto uom_rate_scale_difference =
                rate_scale_difference_uom_auth_name.empty()
                    ? common::UnitOfMeasure::NONE
                    : d->createUnitOfMeasure(
                          rate_scale_difference_uom_auth_name,
                          rate_scale_difference_uom_code);
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
                EPSG_NAME_PARAMETER_X_AXIS_TRANSLATION,
                EPSG_CODE_PARAMETER_X_AXIS_TRANSLATION));
            values.emplace_back(createLength(tx, uom_translation));

            parameters.emplace_back(createOpParamNameEPSGCode(
                EPSG_NAME_PARAMETER_Y_AXIS_TRANSLATION,
                EPSG_CODE_PARAMETER_Y_AXIS_TRANSLATION));
            values.emplace_back(createLength(ty, uom_translation));

            parameters.emplace_back(createOpParamNameEPSGCode(
                EPSG_NAME_PARAMETER_Z_AXIS_TRANSLATION,
                EPSG_CODE_PARAMETER_Z_AXIS_TRANSLATION));
            values.emplace_back(createLength(tz, uom_translation));

            if (uom_rotation != common::UnitOfMeasure::NONE) {
                parameters.emplace_back(createOpParamNameEPSGCode(
                    EPSG_NAME_PARAMETER_X_AXIS_ROTATION,
                    EPSG_CODE_PARAMETER_X_AXIS_ROTATION));
                values.emplace_back(createAngle(rx, uom_rotation));

                parameters.emplace_back(createOpParamNameEPSGCode(
                    EPSG_NAME_PARAMETER_Y_AXIS_ROTATION,
                    EPSG_CODE_PARAMETER_Y_AXIS_ROTATION));
                values.emplace_back(createAngle(ry, uom_rotation));

                parameters.emplace_back(createOpParamNameEPSGCode(
                    EPSG_NAME_PARAMETER_Z_AXIS_ROTATION,
                    EPSG_CODE_PARAMETER_Z_AXIS_ROTATION));
                values.emplace_back(createAngle(rz, uom_rotation));

                parameters.emplace_back(createOpParamNameEPSGCode(
                    EPSG_NAME_PARAMETER_SCALE_DIFFERENCE,
                    EPSG_CODE_PARAMETER_SCALE_DIFFERENCE));
                values.emplace_back(operation::ParameterValue::create(
                    common::Scale(c_locale_stod(scale_difference),
                                  uom_scale_difference)));
            }

            if (uom_rate_translation != common::UnitOfMeasure::NONE) {
                parameters.emplace_back(createOpParamNameEPSGCode(
                    EPSG_NAME_PARAMETER_RATE_X_AXIS_TRANSLATION,
                    EPSG_CODE_PARAMETER_RATE_X_AXIS_TRANSLATION));
                values.emplace_back(
                    createLength(rate_tx, uom_rate_translation));

                parameters.emplace_back(createOpParamNameEPSGCode(
                    EPSG_NAME_PARAMETER_RATE_Y_AXIS_TRANSLATION,
                    EPSG_CODE_PARAMETER_RATE_Y_AXIS_TRANSLATION));
                values.emplace_back(
                    createLength(rate_ty, uom_rate_translation));

                parameters.emplace_back(createOpParamNameEPSGCode(
                    EPSG_NAME_PARAMETER_RATE_Z_AXIS_TRANSLATION,
                    EPSG_CODE_PARAMETER_RATE_Z_AXIS_TRANSLATION));
                values.emplace_back(
                    createLength(rate_tz, uom_rate_translation));

                parameters.emplace_back(createOpParamNameEPSGCode(
                    EPSG_NAME_PARAMETER_RATE_X_AXIS_ROTATION,
                    EPSG_CODE_PARAMETER_RATE_X_AXIS_ROTATION));
                values.emplace_back(createAngle(rate_rx, uom_rate_rotation));

                parameters.emplace_back(createOpParamNameEPSGCode(
                    EPSG_NAME_PARAMETER_RATE_Y_AXIS_ROTATION,
                    EPSG_CODE_PARAMETER_RATE_Y_AXIS_ROTATION));
                values.emplace_back(createAngle(rate_ry, uom_rate_rotation));

                parameters.emplace_back(createOpParamNameEPSGCode(
                    EPSG_NAME_PARAMETER_RATE_Z_AXIS_ROTATION,
                    EPSG_CODE_PARAMETER_RATE_Z_AXIS_ROTATION));
                values.emplace_back(createAngle(rate_rz, uom_rate_rotation));

                parameters.emplace_back(createOpParamNameEPSGCode(
                    EPSG_NAME_PARAMETER_RATE_SCALE_DIFFERENCE,
                    EPSG_CODE_PARAMETER_RATE_SCALE_DIFFERENCE));
                values.emplace_back(operation::ParameterValue::create(
                    common::Scale(c_locale_stod(rate_scale_difference),
                                  uom_rate_scale_difference)));

                parameters.emplace_back(createOpParamNameEPSGCode(
                    EPSG_NAME_PARAMETER_REFERENCE_EPOCH,
                    EPSG_CODE_PARAMETER_REFERENCE_EPOCH));
                values.emplace_back(operation::ParameterValue::create(
                    common::Measure(c_locale_stod(epoch), uom_epoch)));
            } else if (uom_epoch != common::UnitOfMeasure::NONE) {
                constexpr int
                    EPSG_CODE_PARAMETER_TRANSFORMATION_REFERENCE_EPOCH = 1049;
                static const std::string
                    EPSG_NAME_PARAMETER_TRANSFORMATION_REFERENCE_EPOCH(
                        "Transformation reference epoch");

                parameters.emplace_back(createOpParamNameEPSGCode(
                    EPSG_NAME_PARAMETER_TRANSFORMATION_REFERENCE_EPOCH,
                    EPSG_CODE_PARAMETER_TRANSFORMATION_REFERENCE_EPOCH));
                values.emplace_back(operation::ParameterValue::create(
                    common::Measure(c_locale_stod(epoch), uom_epoch)));
            }

            auto props =
                d->createProperties(code, name, deprecated,
                                    area_of_use_auth_name, area_of_use_code);

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
        res = d->runWithCodeParam(
            "SELECT name, method_auth_name, method_code, method_name, "
            "source_crs_auth_name, source_crs_code, target_crs_auth_name, "
            "target_crs_code, area_of_use_auth_name, area_of_use_code, "
            "accuracy, grid_param_auth_name, grid_param_code, grid_param_name, "
            "grid_name, "
            "grid2_param_auth_name, grid2_param_code, grid2_param_name, "
            "grid2_name, "
            "interpolation_crs_auth_name, interpolation_crs_code, deprecated "
            "FROM "
            "grid_transformation WHERE auth_name = ? AND code = ?",
            code);
        if (res.empty()) {
            // shouldn't happen if foreign keys are OK
            throw NoSuchAuthorityCodeException("grid_transformation not found",
                                               getAuthority(), code);
        }
        try {
            const auto &row = res[0];
            size_t idx = 0;
            const auto &name = row[idx++];
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
                d->createProperties(code, name, deprecated,
                                    area_of_use_auth_name, area_of_use_code);
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
            << "SELECT name, method_auth_name, method_code, method_name, "
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
        buffer << ", deprecated FROM other_transformation WHERE auth_name = ? "
                  "AND code = ?";

        res = d->runWithCodeParam(buffer.str(), code);
        if (res.empty()) {
            // shouldn't happen if foreign keys are OK
            throw NoSuchAuthorityCodeException("other_transformation not found",
                                               getAuthority(), code);
        }
        try {
            const auto &row = res[0];
            size_t idx = 0;
            const auto &name = row[idx++];
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
                double normalized_value(c_locale_stod(param_value));
                normalizeMeasure(param_uom_code, param_value,
                                 normalized_uom_code, normalized_value);
                auto uom = d->createUnitOfMeasure(param_uom_auth_name,
                                                  normalized_uom_code);
                values.emplace_back(operation::ParameterValue::create(
                    common::Measure(normalized_value, uom)));
            }
            idx = base_param_idx + 6 * N_MAX_PARAMS;

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
                d->createProperties(code, name, deprecated,
                                    area_of_use_auth_name, area_of_use_code);

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

            if (operation::isAxisOrderReversal(
                    method_name, method_auth_name == metadata::Identifier::EPSG
                                     ? std::atoi(method_code.c_str())
                                     : 0)) {
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
        res = d->runWithCodeParam(
            "SELECT name, source_crs_auth_name, source_crs_code, "
            "target_crs_auth_name, target_crs_code, "
            "area_of_use_auth_name, area_of_use_code, accuracy, "
            "step1_auth_name, step1_code, step2_auth_name, step2_code, "
            "step3_auth_name, step3_code, deprecated FROM "
            "concatenated_operation WHERE auth_name = ? AND code = ?",
            code);
        if (res.empty()) {
            // shouldn't happen if foreign keys are OK
            throw NoSuchAuthorityCodeException(
                "concatenated_operation not found", getAuthority(), code);
        }
        try {
            const auto &row = res[0];
            size_t idx = 0;
            const auto &name = row[idx++];
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
            const auto &deprecated_str = row[idx++];
            const bool deprecated = deprecated_str == "1";

            std::vector<operation::CoordinateOperationNNPtr> operations;
            operations.push_back(
                d->createFactory(step1_auth_name)
                    ->createCoordinateOperation(step1_code, false,
                                                usePROJAlternativeGridNames));
            operations.push_back(
                d->createFactory(step2_auth_name)
                    ->createCoordinateOperation(step2_code, false,
                                                usePROJAlternativeGridNames));

            if (!step3_auth_name.empty()) {
                operations.push_back(
                    d->createFactory(step3_auth_name)
                        ->createCoordinateOperation(
                            step3_code, false, usePROJAlternativeGridNames));
            }

            // In case the operation is a conversion (we hope this is the
            // forward case!)
            if (!operations[0]->sourceCRS() || !operations[0]->targetCRS()) {
                if (!operations[1]->sourceCRS()) {
                    throw FactoryException(
                        "chaining of conversion not supported");
                }
                operations[0]->setCRSs(
                    d->createFactory(source_crs_auth_name)
                        ->createCoordinateReferenceSystem(source_crs_code),
                    NN_NO_CHECK(operations[1]->sourceCRS()), nullptr);
            }

            // Some concatenated operations, like 8443, might actually chain
            // reverse operations rather than forward operations.
            {
                const auto &op0SrcId =
                    operations[0]->sourceCRS()->identifiers()[0];
                if (op0SrcId->code() != source_crs_code ||
                    *op0SrcId->codeSpace() != source_crs_auth_name) {
                    operations[0] = operations[0]->inverse();
                }
            }

            {
                const auto &op0SrcId =
                    operations[0]->sourceCRS()->identifiers()[0];
                if (op0SrcId->code() != source_crs_code ||
                    *op0SrcId->codeSpace() != source_crs_auth_name) {
                    throw FactoryException(
                        "Source CRS of first operation in concatenated "
                        "operation " +
                        code + " does not match source CRS of "
                               "concatenated operation");
                }
            }

            // In case the operation is a conversion (we hope this is the
            // forward case!)
            if (!operations[1]->sourceCRS() || !operations[1]->targetCRS()) {
                if (step3_auth_name.empty()) {
                    operations[1]->setCRSs(
                        NN_NO_CHECK(operations[0]->targetCRS()),
                        d->createFactory(target_crs_auth_name)
                            ->createCoordinateReferenceSystem(target_crs_code),
                        nullptr);
                } else {
                    if (!operations[2]->sourceCRS()) {
                        throw FactoryException(
                            "chaining of conversion not supported");
                    }
                    operations[1]->setCRSs(
                        NN_NO_CHECK(operations[0]->targetCRS()),
                        NN_NO_CHECK(operations[2]->sourceCRS()), nullptr);
                }
            }

            const auto &op1SrcId = operations[1]->sourceCRS()->identifiers()[0];
            const auto &op0TargetId =
                operations[0]->targetCRS()->identifiers()[0];
            while (true) {
                if (step3_auth_name.empty()) {
                    const auto &opLastTargetId =
                        operations.back()->targetCRS()->identifiers()[0];
                    if (opLastTargetId->code() == target_crs_code &&
                        *opLastTargetId->codeSpace() == target_crs_auth_name) {
                        // in case we have only 2 steps, and
                        // step2.targetCRS == concatenate.targetCRS do nothing,
                        // but ConcatenatedOperation::create() will ultimately
                        // check that step1.targetCRS == step2.sourceCRS
                        break;
                    }
                }
                if (op1SrcId->code() != op0TargetId->code() ||
                    *op1SrcId->codeSpace() != *op0TargetId->codeSpace()) {
                    operations[1] = operations[1]->inverse();
                }
                break;
            }

            if (!step3_auth_name.empty()) {

                const auto &op2Src = operations[2]->sourceCRS();
                // In case the operation is a conversion (we hope this is the
                // forward case!)
                if (!op2Src || !operations[2]->targetCRS()) {
                    operations[2]->setCRSs(
                        NN_NO_CHECK(operations[1]->targetCRS()),
                        d->createFactory(target_crs_auth_name)
                            ->createCoordinateReferenceSystem(target_crs_code),
                        nullptr);
                }

                const auto &op2SrcId = op2Src->identifiers()[0];
                const auto &op1TargetId =
                    operations[1]->targetCRS()->identifiers()[0];
                if (op2SrcId->code() != op1TargetId->code() ||
                    *op2SrcId->codeSpace() != *op1TargetId->codeSpace()) {
                    operations[2] = operations[2]->inverse();
                }
            }

            const auto &opLastTargetId =
                operations.back()->targetCRS()->identifiers()[0];
            if (opLastTargetId->code() != target_crs_code ||
                *opLastTargetId->codeSpace() != target_crs_auth_name) {
                throw FactoryException(
                    "Target CRS of last operation in concatenated operation " +
                    code +
                    " doest not match target CRS of concatenated operation");
            }

            auto props =
                d->createProperties(code, name, deprecated,
                                    area_of_use_auth_name, area_of_use_code);

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
 * at infering operations that are not explicitly in the database.
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
        getAuthority(), sourceCRSCode, getAuthority(), targetCRSCode, false,
        false);
}

// ---------------------------------------------------------------------------

/** \brief Returns a list operation::CoordinateOperation between two CRS.
 *
 * The list is ordered with preferred operations first. No attempt is made
 * at infering operations that are not explicitly in the database (see
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
 * @return list of coordinate operations
 * @throw NoSuchAuthorityCodeException
 * @throw FactoryException
 */

std::vector<operation::CoordinateOperationNNPtr>
AuthorityFactory::createFromCoordinateReferenceSystemCodes(
    const std::string &sourceCRSAuthName, const std::string &sourceCRSCode,
    const std::string &targetCRSAuthName, const std::string &targetCRSCode,
    bool usePROJAlternativeGridNames, bool discardIfMissingGrid) const {
    std::vector<operation::CoordinateOperationNNPtr> list;

    // Look-up first for conversion which is the most precise.
    std::string sql(
        "SELECT conversion_auth_name, conversion_code FROM "
        "projected_crs WHERE geodetic_crs_auth_name = ? AND geodetic_crs_code "
        "= ? AND auth_name = ? AND code = ? AND deprecated != 1");
    auto params = std::vector<SQLValues>{sourceCRSAuthName, sourceCRSCode,
                                         targetCRSAuthName, targetCRSCode};
    if (!getAuthority().empty()) {
        sql += " AND conversion_auth_name = ?";
        params.emplace_back(getAuthority());
    }
    auto res = d->run(sql, params);
    if (!res.empty()) {
        auto targetCRS = d->createFactory(targetCRSAuthName)
                             ->createProjectedCRS(targetCRSCode);
        auto conv = targetCRS->derivingConversion();
        list.emplace_back(conv);
        return list;
    }
    sql =
        "SELECT cov.auth_name, cov.code FROM "
        "coordinate_operation_view cov JOIN area ON cov.area_of_use_auth_name "
        "= area.auth_name AND cov.area_of_use_code = area.code WHERE "
        "source_crs_auth_name = ? AND source_crs_code = ? AND "
        "target_crs_auth_name = ? AND target_crs_code = ? AND "
        "cov.deprecated != 1";
    params = {sourceCRSAuthName, sourceCRSCode, targetCRSAuthName,
              targetCRSCode};
    if (!getAuthority().empty()) {
        sql += " AND cov.auth_name = ?";
        params.emplace_back(getAuthority());
    }
    sql += " ORDER BY pseudo_area_from_swne(south_lat, west_lon, north_lat, "
           "east_lon) DESC, "
           "(CASE WHEN accuracy is NULL THEN 1 ELSE 0 END), accuracy";
    res = d->run(sql, params);
    for (const auto &row : res) {
        const auto &auth_name = row[0];
        const auto &code = row[1];
        auto op = d->createFactory(auth_name)->createCoordinateOperation(
            code, true, usePROJAlternativeGridNames);
        if (!d->rejectOpDueToMissingGrid(op, discardIfMissingGrid)) {
            list.emplace_back(op);
        }
    }
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
    const std::vector<std::pair<std::string, std::string>>
        &intermediateCRSAuthCodes) const {

    const std::string sqlProlog(
        "SELECT v1.auth_name AS auth_name1, v1.code AS code1, "
        "v1.accuracy AS accuracy1, "
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
    const std::string joinArea(
        "JOIN area a1 ON v1.area_of_use_auth_name = a1.auth_name "
        "AND v1.area_of_use_code = a1.code "
        "JOIN area a2 ON v2.area_of_use_auth_name = a2.auth_name "
        "AND v2.area_of_use_code = a2.code ");
    const std::string orderBy(
        "ORDER BY (CASE WHEN accuracy1 is NULL THEN 1 ELSE 0 END) + "
        "(CASE WHEN accuracy2 is NULL THEN 1 ELSE 0 END), "
        "accuracy1 + accuracy2");

    std::vector<operation::CoordinateOperationNNPtr> listTmp;

    // Case (source->intermediate) and (intermediate->target)
    std::string sql(
        sqlProlog + "ON v1.target_crs_auth_name = v2.source_crs_auth_name "
                    "AND v1.target_crs_code = v2.source_crs_code " +
        joinArea +
        "WHERE v1.source_crs_auth_name = ? AND v1.source_crs_code = ? "
        "AND v2.target_crs_auth_name = ? AND v2.target_crs_code = ? ");
    auto params = std::vector<SQLValues>{sourceCRSAuthName, sourceCRSCode,
                                         targetCRSAuthName, targetCRSCode};

    std::string additionalWhere(
        "AND v1.deprecated = 0 AND v2.deprecated = 0 "
        "AND intersects_bbox(south_lat1, west_lon1, north_lat1, east_lon1, "
        "south_lat2, west_lon2, north_lat2, east_lon2) == 1 ");
    if (!getAuthority().empty()) {
        additionalWhere += "AND v1.auth_name = ? AND v2.auth_name = ? ";
        params.emplace_back(getAuthority());
        params.emplace_back(getAuthority());
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

    for (const auto &row : res) {
        const auto &auth_name1 = row[0];
        const auto &code1 = row[1];
        // const auto &accuracy1 = row[2];
        const auto &auth_name2 = row[3];
        const auto &code2 = row[4];
        // const auto &accuracy2 = row[5];
        auto op1 = d->createFactory(auth_name1)
                       ->createCoordinateOperation(code1, true,
                                                   usePROJAlternativeGridNames);
        if (useIrrelevantPivot(op1, sourceCRSAuthName, sourceCRSCode,
                               targetCRSAuthName, targetCRSCode)) {
            continue;
        }
        auto op2 = d->createFactory(auth_name2)
                       ->createCoordinateOperation(code2, true,
                                                   usePROJAlternativeGridNames);
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
    for (const auto &row : res) {
        const auto &auth_name1 = row[0];
        const auto &code1 = row[1];
        // const auto &accuracy1 = row[2];
        const auto &auth_name2 = row[3];
        const auto &code2 = row[4];
        // const auto &accuracy2 = row[5];
        auto op1 = d->createFactory(auth_name1)
                       ->createCoordinateOperation(code1, true,
                                                   usePROJAlternativeGridNames);
        if (useIrrelevantPivot(op1, sourceCRSAuthName, sourceCRSCode,
                               targetCRSAuthName, targetCRSCode)) {
            continue;
        }
        auto op2 = d->createFactory(auth_name2)
                       ->createCoordinateOperation(code2, true,
                                                   usePROJAlternativeGridNames);
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
    for (const auto &row : res) {
        const auto &auth_name1 = row[0];
        const auto &code1 = row[1];
        // const auto &accuracy1 = row[2];
        const auto &auth_name2 = row[3];
        const auto &code2 = row[4];
        // const auto &accuracy2 = row[5];
        auto op1 = d->createFactory(auth_name1)
                       ->createCoordinateOperation(code1, true,
                                                   usePROJAlternativeGridNames);
        if (useIrrelevantPivot(op1, sourceCRSAuthName, sourceCRSCode,
                               targetCRSAuthName, targetCRSCode)) {
            continue;
        }
        auto op2 = d->createFactory(auth_name2)
                       ->createCoordinateOperation(code2, true,
                                                   usePROJAlternativeGridNames);
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
    for (const auto &row : res) {
        const auto &auth_name1 = row[0];
        const auto &code1 = row[1];
        // const auto &accuracy1 = row[2];
        const auto &auth_name2 = row[3];
        const auto &code2 = row[4];
        // const auto &accuracy2 = row[5];
        auto op1 = d->createFactory(auth_name1)
                       ->createCoordinateOperation(code1, true,
                                                   usePROJAlternativeGridNames);
        if (useIrrelevantPivot(op1, sourceCRSAuthName, sourceCRSCode,
                               targetCRSAuthName, targetCRSCode)) {
            continue;
        }
        auto op2 = d->createFactory(auth_name2)
                       ->createCoordinateOperation(code2, true,
                                                   usePROJAlternativeGridNames);
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
const std::string &AuthorityFactory::getAuthority() PROJ_CONST_DEFN {
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
    case ObjectType::GEOGRAPHIC_CRS:
        sql = "SELECT code FROM geodetic_crs WHERE type IN ('geographic 2D', "
              "'geographic 3D') AND ";
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
        sql += " AND deprecated != 1";
    }

    auto res = d->run(sql, {getAuthority()});
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
        throw NoSuchAuthorityCodeException("object not found", getAuthority(),
                                           code);
    }
    return res[0][0];
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
