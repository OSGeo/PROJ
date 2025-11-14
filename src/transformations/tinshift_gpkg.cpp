/******************************************************************************
 * Project:  PROJ
 * Purpose:  Functionality related to TIN based transformations using a
 *GeoPackage file Author:   Even Rouault, <even.rouault at spatialys.com>
 *
 ******************************************************************************
 * Copyright (c) 2025, Even Rouault, <even.rouault at spatialys.com>
 *
 * SPDX-License-Identifier: MIT
 *****************************************************************************/

#define FROM_PROJ_CPP

#include "tinshift_gpkg.hpp"
#include "proj/internal/include_nlohmann_json.hpp"
#include "proj/internal/internal.hpp"

#ifdef EMBED_RESOURCE_FILES
#include "embedded_resources.h"
#endif

#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <cstring>
#include <limits>
#include <sstream> // std::ostringstream

#include <sqlite3.h>

using json = nlohmann::json;
using namespace NS_PROJ;

// ---------------------------------------------------------------------------

const char *TINShiftGeopackageException::what() const noexcept {
    return msg_.c_str();
}

// ---------------------------------------------------------------------------

std::unique_ptr<TINShiftGeopackageFile>
TINShiftGeopackageFile::open(PJ_CONTEXT *ctx, const std::string &filename) {
    std::string sqliteURI(filename);

    auto poTinshift =
        std::unique_ptr<TINShiftGeopackageFile>(new TINShiftGeopackageFile());
    if (internal::starts_with(filename, "http://") ||
        internal::starts_with(filename, "https://")) {
        poTinshift->sqlite_vfs_ = SQLite3VFS::createNetwork(ctx);
    }

    while (true) {
        if (sqlite3_open_v2(sqliteURI.c_str(), &poTinshift->sqlite_handle_,
                            SQLITE_OPEN_READONLY | SQLITE_OPEN_URI,
                            poTinshift->sqlite_vfs_
                                ? poTinshift->sqlite_vfs_->name()
                                : nullptr) != SQLITE_OK ||
            !poTinshift->sqlite_handle_) {

#ifdef EMBED_RESOURCE_FILES
            if (!poTinshift->sqlite_vfs_) {
                unsigned int size = 0;
                const unsigned char *in_memory_data =
                    pj_get_embedded_resource(filename.c_str(), &size);
                if (!in_memory_data) {
                    throw TINShiftGeopackageException(std::string("Open of ")
                                                          .append(filename)
                                                          .append(" failed"));
                }

                poTinshift->sqlite_vfs_ =
                    SQLite3VFS::createMem(in_memory_data, size);
                if (poTinshift->sqlite_vfs_ == nullptr) {
                    throw TINShiftGeopackageException(std::string("Open of ")
                                                          .append(filename)
                                                          .append(" failed"));
                }

                std::ostringstream buffer;
                buffer << "file:/";
                buffer << filename;
                buffer << "?immutable=1&ptr=";
                buffer << reinterpret_cast<uintptr_t>(in_memory_data);
                buffer << "&sz=";
                buffer << size;
                buffer << "&max=";
                buffer << size;
                buffer << "&vfs=";
                buffer << poTinshift->sqlite_vfs_->name();
                sqliteURI = buffer.str();

                continue;
            }
#endif

            throw TINShiftGeopackageException(
                std::string("Open of ").append(sqliteURI).append(" failed"));
        } else {
            break;
        }
    }

    poTinshift->getMetadata();
    poTinshift->prepareDatabaseQuery();

    return poTinshift;
}

// ---------------------------------------------------------------------------

TINShiftGeopackageFile::~TINShiftGeopackageFile() {
    if (select_stmt_)
        sqlite3_finalize(select_stmt_);
    if (sqlite_handle_)
        sqlite3_close(sqlite_handle_);
}

// ---------------------------------------------------------------------------

namespace {

static std::string getString(const json &j, const char *key, bool optional) {
    if (!j.contains(key)) {
        if (optional) {
            return std::string();
        }
        throw TINShiftGeopackageException(std::string("Missing \"") + key +
                                          "\" key");
    }
    const json v = j[key];
    if (!v.is_string()) {
        throw TINShiftGeopackageException(std::string("The value of \"") + key +
                                          "\" should be a string");
    }
    return v.get<std::string>();
}

static std::string getReqString(const json &j, const char *key) {
    return getString(j, key, false);
}

static double getReqDouble(const json &j, const char *key) {
    if (!j.contains(key)) {
        throw TINShiftGeopackageException(std::string("Missing \"") + key +
                                          "\" key");
    }

    const json v = j[key];
    if (!v.is_number()) {
        throw TINShiftGeopackageException(std::string("The value of \"") + key +
                                          "\" should be a number");
    }
    return v.get<double>();
}

static int getReqInt(const json &j, const char *key) {
    if (!j.contains(key)) {
        throw TINShiftGeopackageException(std::string("Missing \"") + key +
                                          "\" key");
    }

    const json v = j[key];
    if (!v.is_number()) {
        throw TINShiftGeopackageException(std::string("The value of \"") + key +
                                          "\" should be an integer");
    }
    return v.get<int>();
}

// ---------------------------------------------------------------------------

static json getArrayMember(const json &j, const char *key) {
    if (!j.contains(key)) {
        throw TINShiftGeopackageException(std::string("Missing \"") + key +
                                          "\" key");
    }
    const json obj = j[key];
    if (!obj.is_array()) {
        throw TINShiftGeopackageException(std::string("The value of \"") + key +
                                          "\" should be a array");
    }
    return obj;
}

} // namespace
// ---------------------------------------------------------------------------

void TINShiftGeopackageFile::getMetadata() {
    std::string metadata;
    // Get JSON metadata entry from gpkg_metadata table
    {
        char **papszResult = nullptr;
        int nRows = 0;
        char *pszErrMsg = nullptr;
        if (sqlite3_get_table(sqlite_handle_,
                              "SELECT metadata FROM gpkg_metadata WHERE "
                              "md_standard_uri = 'https://proj.org'",
                              &papszResult, &nRows, nullptr,
                              &pszErrMsg) != SQLITE_OK) {
            sqlite3_free_table(papszResult);
            auto ex = TINShiftGeopackageException(
                std::string("Cannot get metadata: ").append(pszErrMsg));
            sqlite3_free(pszErrMsg);
            throw ex;
        }
        if (nRows != 1 || !papszResult[1]) {
            sqlite3_free_table(papszResult);
            throw TINShiftGeopackageException(
                "Cannot get metadata in gpkg_metadata table");
        }
        metadata = papszResult[1];
        sqlite3_free_table(papszResult);
    }

    json j;
    try {
        j = json::parse(metadata);
    } catch (const std::exception &e) {
        throw TINShiftGeopackageException(
            std::string("Cannot parse JSON metadata: ").append(e.what()));
    }
    if (!j.is_object()) {
        throw TINShiftGeopackageException("JSON metadata is not an object");
    }

    if (getReqString(j, "file_type") != "triangulation_file") {
        throw TINShiftGeopackageException("File is not of the expected type");
    }

    const std::string version = getReqString(j, "format_version");
    if (!internal::starts_with(version, "1.")) {
        throw TINShiftGeopackageException(std::string("format_version = ")
                                              .append(version)
                                              .append(" is not supported"));
    }

    const auto jTransformedComponents =
        getArrayMember(j, "transformed_components");
    for (const json &jComp : jTransformedComponents) {
        if (!jComp.is_string()) {
            throw TINShiftGeopackageException(
                "transformed_components[] item is not a string");
        }
        const auto jCompStr = jComp.get<std::string>();
        if (jCompStr == "horizontal") {
            horizontalTransformation_ = true;
        } else if (jCompStr == "vertical") {
            verticalTransformation_ = true;
        } else {
            throw TINShiftGeopackageException(
                "transformed_components[] = " + jCompStr + " is not handled");
        }
    }

    if (horizontalTransformation_) {
        min_shift_x_ = getReqDouble(j, "min_shift_x");
        min_shift_y_ = getReqDouble(j, "min_shift_y");
        max_shift_x_ = getReqDouble(j, "max_shift_x");
        max_shift_y_ = getReqDouble(j, "max_shift_y");
    }

    if (j.contains("fallback_strategy")) {
        const auto fallback_strategy = getReqString(j, "fallback_strategy");
        if (fallback_strategy == "nearest_side") {
            fallbackStrategy_ = FALLBACK_NEAREST_SIDE;
        } else if (fallback_strategy == "nearest_centroid") {
            fallbackStrategy_ = FALLBACK_NEAREST_CENTROID;
        } else if (fallback_strategy == "none") {
            fallbackStrategy_ = FALLBACK_NONE;
        } else {
            throw TINShiftGeopackageException("invalid fallback_strategy");
        }
    }

    if (fallbackStrategy_ != FALLBACK_NONE) {
        numVertices_ = getReqInt(j, "num_vertices");
        if (numVertices_ <= 0) {
            throw TINShiftGeopackageException("invalid value for num_vertices");
        }
    }

    // Get bounding box of vertices table
    {
        char **papszResult = nullptr;
        int nRows = 0;
        char *pszErrMsg = nullptr;
        if (sqlite3_get_table(sqlite_handle_,
                              "SELECT min_x, min_y, max_x, max_y FROM "
                              "gpkg_contents WHERE table_name = 'vertices'",
                              &papszResult, &nRows, nullptr,
                              &pszErrMsg) != SQLITE_OK) {
            sqlite3_free_table(papszResult);
            auto ex = TINShiftGeopackageException(
                std::string("Cannot get bounding box of vertices table: ")
                    .append(pszErrMsg));
            sqlite3_free(pszErrMsg);
            throw ex;
        }
        if (nRows != 1 || !papszResult[4] || !papszResult[5] ||
            !papszResult[6] || !papszResult[7]) {
            sqlite3_free_table(papszResult);
            throw TINShiftGeopackageException(
                "Cannot get bounding box of vertices table");
        }
        min_x_ = internal::c_locale_stod(papszResult[4]);
        min_y_ = internal::c_locale_stod(papszResult[5]);
        max_x_ = internal::c_locale_stod(papszResult[6]);
        max_y_ = internal::c_locale_stod(papszResult[7]);
        sqlite3_free_table(papszResult);
        if (!(min_x_ < max_x_) || !(min_y_ < max_y_)) {
            throw TINShiftGeopackageException(
                "Invalid bounding box of vertices table");
        }
    }
}

// ---------------------------------------------------------------------------

void TINShiftGeopackageFile::prepareDatabaseQuery() {
    // Get field names of vertices table
    {
        char **papszResult = nullptr;
        int nRows = 0;
        int nCols = 0;
        char *pszErrMsg = nullptr;
        if (sqlite3_get_table(sqlite_handle_, "PRAGMA table_info(vertices)",
                              &papszResult, &nRows, &nCols,
                              &pszErrMsg) != SQLITE_OK) {
            sqlite3_free_table(papszResult);
            auto ex = TINShiftGeopackageException(
                std::string("Cannot get definition of table vertices: ")
                    .append(pszErrMsg));
            sqlite3_free(pszErrMsg);
            throw ex;
        }
        if (nRows == 0 || nCols != 6) {
            sqlite3_free_table(papszResult);
            throw TINShiftGeopackageException(
                "Cannot get definition of table vertices");
        }
        for (int i = 0; i < nRows; ++i) {
            const char *pszColName = papszResult[(i + 1) * nCols + 1];
            if (pszColName && strcmp(pszColName, "fid") != 0 &&
                strcmp(pszColName, "geom") != 0) {
                std::string osColName(pszColName);
                if (osColName == "source_z")
                    sourceZ_ = true;
                else if (osColName == "target_x")
                    targetX_ = true;
                else if (osColName == "target_y")
                    targetY_ = true;
                else if (osColName == "target_z")
                    targetZ_ = true;
                else if (osColName == "offset_z")
                    offsetZ_ = true;
                vertices_cols_.push_back(std::move(osColName));
            }
        }
        sqlite3_free_table(papszResult);
    }

    if (horizontalTransformation_ && !targetX_)
        throw TINShiftGeopackageException(
            "target_x field missing in table vertices");
    if (horizontalTransformation_ && !targetY_)
        throw TINShiftGeopackageException(
            "target_y field missing in table vertices");
    if (verticalTransformation_ && !((sourceZ_ && targetZ_) || offsetZ_))
        throw TINShiftGeopackageException("(source_z and target_z) or offset_z "
                                          "fields missing in table vertices");

    std::vector<std::string> colsToRequest;
    if (horizontalTransformation_) {
        colsToRequest.push_back("target_x");
        colsToRequest.push_back("target_y");
    }
    if (verticalTransformation_) {
        if (sourceZ_) {
            colsToRequest.push_back("source_z");
            colsToRequest.push_back("target_z");
        } else {
            colsToRequest.push_back("offset_z");
        }
    }

    std::string sql(
        "SELECT v1.geom AS v1_geom, v2.geom AS v2_geom, v3.geom AS v3_geom");
    for (const std::string &col : colsToRequest) {
        sql += ", v1.";
        sql += col;
        sql += " AS v1_";
        sql += col;

        sql += ", v2.";
        sql += col;
        sql += " AS v2_";
        sql += col;

        sql += ", v3.";
        sql += col;
        sql += " AS v3_";
        sql += col;
    }
    sql += " FROM triangles_def "
           "LEFT JOIN vertices v1 ON idx_vertex1 = v1.fid "
           "LEFT JOIN vertices v2 ON idx_vertex2 = v2.fid "
           "LEFT JOIN vertices v3 ON idx_vertex3 = v3.fid "
           "WHERE triangles_def.fid IN ("
           "SELECT id FROM rtree_triangles_geom "
           "WHERE maxx >= ? AND minx <= ? AND maxy >= ? AND miny <= ?)";

    sqlite3_prepare_v2(sqlite_handle_, sql.c_str(), -1, &select_stmt_, nullptr);
    if (!select_stmt_) {
        throw TINShiftGeopackageException(sqlite3_errmsg(sqlite_handle_));
    }
}

// ---------------------------------------------------------------------------

/************************************************************************/
/*                             swap_words()                             */
/*                                                                      */
/*      Convert the byte order of the given word(s) in place.           */
/************************************************************************/

static const int byte_order_test = 1;
#define IS_LSB                                                                 \
    (1 == (reinterpret_cast<const unsigned char *>(&byte_order_test))[0])

static void swap_words(void *dataIn, size_t word_size, size_t word_count)

{
    unsigned char *data = static_cast<unsigned char *>(dataIn);
    for (size_t word = 0; word < word_count; word++) {
        for (size_t i = 0; i < word_size / 2; i++) {
            unsigned char t;

            t = data[i];
            data[i] = data[word_size - i - 1];
            data[word_size - i - 1] = t;
        }

        data += word_size;
    }
}

// ---------------------------------------------------------------------------

static inline double sqr(double x) { return x * x; }
static inline double squared_distance(double x1, double y1, double x2,
                                      double y2) {
    return sqr(x1 - x2) + sqr(y1 - y2);
}
static double sq_distance_point_segment(double x, double y, double x1,
                                        double y1, double x2, double y2,
                                        double dist12) {
    // squared distance of point x/y to line segment x1/y1 -- x2/y2
    const double t = ((x - x1) * (x2 - x1) + (y - y1) * (y2 - y1)) / dist12;
    if (t <= 0.0) {
        // closest to x1/y1
        return squared_distance(x, y, x1, y1);
    }
    if (t >= 1.0) {
        // closest to y2/y2
        return squared_distance(x, y, x2, y2);
    }

    // closest to line segment x1/y1 -- x2/y2
    return squared_distance(x, y, x1 + t * (x2 - x1), y1 + t * (y2 - y1));
}

// ---------------------------------------------------------------------------

bool TINShiftGeopackageFile::findTriangle(
    double x, double y, bool forwardDirection, double &lambda1, double &lambda2,
    double &lambda3, std::vector<double> &valsVertex1,
    std::vector<double> &valsVertex2, std::vector<double> &valsVertex3) {

    const bool useForwardDirection =
        forwardDirection || !horizontalTransformation_;

    // Compute barycentric coefficients
    const auto computeLambdas = [x, y, &lambda1, &lambda2,
                                 &lambda3](const std::array<double, 3> &vX,
                                           const std::array<double, 3> &vY,
                                           bool checkWithinTriangle) {
        constexpr double EPSILON = 1e-10;
        const double det_T = (vY[1] - vY[2]) * (vX[0] - vX[2]) +
                             (vX[2] - vX[1]) * (vY[0] - vY[2]);
        if (std::fabs(det_T) < EPSILON) {
            // Degenerate triangle
            return false;
        }
        lambda1 =
            ((vY[1] - vY[2]) * (x - vX[2]) + (vX[2] - vX[1]) * (y - vY[2])) /
            det_T;
        lambda2 =
            ((vY[2] - vY[0]) * (x - vX[2]) + (vX[0] - vX[2]) * (y - vY[2])) /
            det_T;
        lambda3 = 1 - lambda1 - lambda2;
        return !checkWithinTriangle ||
               (lambda1 >= -EPSILON && lambda1 <= 1 + EPSILON &&
                lambda2 >= -EPSILON && lambda2 <= 1 + EPSILON &&
                lambda3 >= -EPSILON && lambda3 <= 1 + EPSILON);
    };

    // Get the current triangle's vertices coordinates
    const auto getXY = [this, useForwardDirection](std::array<double, 3> &vX,
                                                   std::array<double, 3> &vY) {
        if (useForwardDirection) {
            const unsigned char *v1_geom = static_cast<const unsigned char *>(
                sqlite3_column_blob(select_stmt_, 0));
            const unsigned char *v2_geom = static_cast<const unsigned char *>(
                sqlite3_column_blob(select_stmt_, 1));
            const unsigned char *v3_geom = static_cast<const unsigned char *>(
                sqlite3_column_blob(select_stmt_, 2));
            constexpr int SIZEOF_GPKG_POINT_PREFIX = 8 + 1 + 4;
            constexpr int SIZEOF_GPKG_POINT =
                SIZEOF_GPKG_POINT_PREFIX + 2 * static_cast<int>(sizeof(double));
            if (sqlite3_column_bytes(select_stmt_, 0) != SIZEOF_GPKG_POINT ||
                sqlite3_column_bytes(select_stmt_, 1) != SIZEOF_GPKG_POINT ||
                sqlite3_column_bytes(select_stmt_, 2) != SIZEOF_GPKG_POINT) {
                return false;
            }

            memcpy(&vX[0], v1_geom + SIZEOF_GPKG_POINT_PREFIX, sizeof(double));
            memcpy(&vY[0], v1_geom + SIZEOF_GPKG_POINT_PREFIX + sizeof(double),
                   sizeof(double));
            memcpy(&vX[1], v2_geom + SIZEOF_GPKG_POINT_PREFIX, sizeof(double));
            memcpy(&vY[1], v2_geom + SIZEOF_GPKG_POINT_PREFIX + sizeof(double),
                   sizeof(double));
            memcpy(&vX[2], v3_geom + SIZEOF_GPKG_POINT_PREFIX, sizeof(double));
            memcpy(&vY[2], v3_geom + SIZEOF_GPKG_POINT_PREFIX + sizeof(double),
                   sizeof(double));

            if (!IS_LSB) {
                swap_words(vX.data(), sizeof(double), vX.size());
                swap_words(vY.data(), sizeof(double), vY.size());
            }
        } else {
            vX[0] = sqlite3_column_double(select_stmt_, 3);
            vX[1] = sqlite3_column_double(select_stmt_, 4);
            vX[2] = sqlite3_column_double(select_stmt_, 5);
            vY[0] = sqlite3_column_double(select_stmt_, 6);
            vY[1] = sqlite3_column_double(select_stmt_, 7);
            vY[2] = sqlite3_column_double(select_stmt_, 8);
        }
        return true;
    };

    // Get the values at the vertices of the current triangle
    const auto collectValsVertex = [this, useForwardDirection, &valsVertex1,
                                    &valsVertex2, &valsVertex3]() {
        if (useForwardDirection) {
            int nextAttrIdx = 3;

            if (horizontalTransformation_) {
                // target_x
                valsVertex1.push_back(sqlite3_column_double(select_stmt_, 3));
                // target_y
                valsVertex1.push_back(sqlite3_column_double(select_stmt_, 6));

                // target_x
                valsVertex2.push_back(sqlite3_column_double(select_stmt_, 4));
                // target_y
                valsVertex2.push_back(sqlite3_column_double(select_stmt_, 7));

                // target_x
                valsVertex3.push_back(sqlite3_column_double(select_stmt_, 5));
                // target_y
                valsVertex3.push_back(sqlite3_column_double(select_stmt_, 8));

                nextAttrIdx = 9;
            }

            if (verticalTransformation_) {
                if (sourceZ_) {
                    // source_z
                    valsVertex1.push_back(
                        sqlite3_column_double(select_stmt_, nextAttrIdx));
                    // target_z
                    valsVertex1.push_back(
                        sqlite3_column_double(select_stmt_, nextAttrIdx + 3));

                    // source_z
                    valsVertex2.push_back(
                        sqlite3_column_double(select_stmt_, nextAttrIdx + 1));
                    // target_z
                    valsVertex2.push_back(
                        sqlite3_column_double(select_stmt_, nextAttrIdx + 4));

                    // source_z
                    valsVertex3.push_back(
                        sqlite3_column_double(select_stmt_, nextAttrIdx + 2));
                    // target_z
                    valsVertex3.push_back(
                        sqlite3_column_double(select_stmt_, nextAttrIdx + 5));
                } else {
                    // offset_z
                    valsVertex1.push_back(
                        sqlite3_column_double(select_stmt_, nextAttrIdx));
                    valsVertex2.push_back(
                        sqlite3_column_double(select_stmt_, nextAttrIdx + 1));
                    valsVertex3.push_back(
                        sqlite3_column_double(select_stmt_, nextAttrIdx + 2));
                }
            }
        } else {
            int nextAttrIdx = 3;

            if (horizontalTransformation_) {

                const unsigned char *v1_geom =
                    static_cast<const unsigned char *>(
                        sqlite3_column_blob(select_stmt_, 0));
                const unsigned char *v2_geom =
                    static_cast<const unsigned char *>(
                        sqlite3_column_blob(select_stmt_, 1));
                const unsigned char *v3_geom =
                    static_cast<const unsigned char *>(
                        sqlite3_column_blob(select_stmt_, 2));
                constexpr int SIZEOF_GPKG_POINT_PREFIX = 8 + 1 + 4;
                constexpr int SIZEOF_GPKG_POINT =
                    SIZEOF_GPKG_POINT_PREFIX +
                    2 * static_cast<int>(sizeof(double));
                if (sqlite3_column_bytes(select_stmt_, 0) !=
                        SIZEOF_GPKG_POINT ||
                    sqlite3_column_bytes(select_stmt_, 1) !=
                        SIZEOF_GPKG_POINT ||
                    sqlite3_column_bytes(select_stmt_, 2) !=
                        SIZEOF_GPKG_POINT) {
                    return false;
                }
                double vX[3], vY[3];
                memcpy(&vX[0], v1_geom + SIZEOF_GPKG_POINT_PREFIX,
                       sizeof(double));
                memcpy(&vY[0],
                       v1_geom + SIZEOF_GPKG_POINT_PREFIX + sizeof(double),
                       sizeof(double));
                memcpy(&vX[1], v2_geom + SIZEOF_GPKG_POINT_PREFIX,
                       sizeof(double));
                memcpy(&vY[1],
                       v2_geom + SIZEOF_GPKG_POINT_PREFIX + sizeof(double),
                       sizeof(double));
                memcpy(&vX[2], v3_geom + SIZEOF_GPKG_POINT_PREFIX,
                       sizeof(double));
                memcpy(&vY[2],
                       v3_geom + SIZEOF_GPKG_POINT_PREFIX + sizeof(double),
                       sizeof(double));

                if (!IS_LSB) {
                    swap_words(vX, sizeof(double), 3);
                    swap_words(vY, sizeof(double), 3);
                }

                // source_x
                valsVertex1.push_back(vX[0]);
                // source_y
                valsVertex1.push_back(vY[0]);

                // source_x
                valsVertex2.push_back(vX[1]);
                // source_y
                valsVertex2.push_back(vY[1]);

                // source_x
                valsVertex3.push_back(vX[2]);
                // source_y
                valsVertex3.push_back(vY[2]);

                nextAttrIdx = 9;
            }

            if (verticalTransformation_) {
                if (sourceZ_) {
                    // source_z
                    valsVertex1.push_back(
                        sqlite3_column_double(select_stmt_, nextAttrIdx));
                    // target_z
                    valsVertex1.push_back(
                        sqlite3_column_double(select_stmt_, nextAttrIdx + 3));

                    // source_z
                    valsVertex2.push_back(
                        sqlite3_column_double(select_stmt_, nextAttrIdx + 1));
                    // target_z
                    valsVertex2.push_back(
                        sqlite3_column_double(select_stmt_, nextAttrIdx + 4));

                    // source_z
                    valsVertex3.push_back(
                        sqlite3_column_double(select_stmt_, nextAttrIdx + 2));
                    // target_z
                    valsVertex3.push_back(
                        sqlite3_column_double(select_stmt_, nextAttrIdx + 5));
                } else {
                    // offset_z
                    valsVertex1.push_back(
                        sqlite3_column_double(select_stmt_, nextAttrIdx));
                    valsVertex2.push_back(
                        sqlite3_column_double(select_stmt_, nextAttrIdx + 1));
                    valsVertex3.push_back(
                        sqlite3_column_double(select_stmt_, nextAttrIdx + 2));
                }
            }
        }
        return true;
    };

    // Try to use last triangle
    if (cachedForwardDirection_ == forwardDirection &&
        !cachedValsVertex1_.empty() &&
        computeLambdas(cachedVerticesX_, cachedVerticesY_,
                       /* checkWithinTriangle = */ true)) {
        valsVertex1 = cachedValsVertex1_;
        valsVertex2 = cachedValsVertex2_;
        valsVertex3 = cachedValsVertex3_;
        return true;
    }

    constexpr double EPSILON = 1e-10;

    sqlite3_reset(select_stmt_);
    if (useForwardDirection) {
        sqlite3_bind_double(select_stmt_, 1, x - EPSILON);
        sqlite3_bind_double(select_stmt_, 2, x + EPSILON);
        sqlite3_bind_double(select_stmt_, 3, y - EPSILON);
        sqlite3_bind_double(select_stmt_, 4, y + EPSILON);
    } else {
        // Take into account the shift between source and target horizontal
        // coordinates when searching in the reverse direction.
        sqlite3_bind_double(select_stmt_, 1, x - max_shift_x_ - EPSILON);
        sqlite3_bind_double(select_stmt_, 2, x - min_shift_x_ + EPSILON);
        sqlite3_bind_double(select_stmt_, 3, y - max_shift_y_ - EPSILON);
        sqlite3_bind_double(select_stmt_, 4, y - min_shift_y_ + EPSILON);
    }

    // Iterate over triangles whose bounding box intersecs the point of interest
    while (sqlite3_step(select_stmt_) == SQLITE_ROW) {
        std::array<double, 3> vX, vY;
        if (!getXY(vX, vY))
            return false;

        if (computeLambdas(vX, vY, /* checkWithinTriangle = */ true)) {
            const bool status = collectValsVertex();
            if (status) {
                cachedForwardDirection_ = forwardDirection;
                cachedVerticesX_ = vX;
                cachedVerticesY_ = vY;
                cachedValsVertex1_ = valsVertex1;
                cachedValsVertex2_ = valsVertex2;
                cachedValsVertex3_ = valsVertex3;
            }
            return status;
        }
    }

    if (fallbackStrategy_ == FALLBACK_NONE) {
        return false;
    }

    const double x_search = std::clamp(
        x + (useForwardDirection ? 0 : -(min_shift_x_ + max_shift_x_) * 0.5),
        min_x_, max_x_);
    const double y_search = std::clamp(
        y + (useForwardDirection ? 0 : -(min_shift_y_ + max_shift_y_) * 0.5),
        min_y_, max_y_);
    double closest_sq_dist = std::numeric_limits<double>::infinity();
    double closest_dist = std::numeric_limits<double>::infinity();
    std::array<double, 3> closest_vX{0, 0, 0};
    std::array<double, 3> closest_vY{0, 0, 0};

    // Initiate the search radius from the density of points
    // and double it if there is no match.
    const double bbox_w = max_x_ - min_x_;
    const double bbox_h = max_y_ - min_y_;
    double radius = sqrt(bbox_w * bbox_h / numVertices_);
    constexpr int MAX_ITERS = 20;
    for (int iter = 0; iter <= MAX_ITERS && radius <= std::max(bbox_w, bbox_h);
         ++iter) {
        sqlite3_reset(select_stmt_);
        sqlite3_bind_double(select_stmt_, 1, x_search - radius);
        sqlite3_bind_double(select_stmt_, 2, x_search + radius);
        sqlite3_bind_double(select_stmt_, 3, y_search - radius);
        sqlite3_bind_double(select_stmt_, 4, y_search + radius);

        // Iterate over triangles in the current area of search
        while (sqlite3_step(select_stmt_) == SQLITE_ROW) {
            std::array<double, 3> vX, vY;
            if (!getXY(vX, vY))
                return false;

            // don't check this triangle if the query point plusminus the
            // currently closest found distance is outside the triangle's
            // bounding box
            if (x + closest_dist < std::min(vX[0], std::min(vX[1], vX[2])) ||
                x - closest_dist > std::max(vX[0], std::max(vX[1], vX[2])) ||
                y + closest_dist < std::min(vY[0], std::min(vY[1], vY[2])) ||
                y - closest_dist > std::max(vY[0], std::min(vY[1], vY[2]))) {
                continue;
            }
            const double dist12 = squared_distance(vX[0], vY[0], vX[1], vY[1]);
            const double dist23 = squared_distance(vX[1], vY[1], vX[2], vY[2]);
            const double dist13 = squared_distance(vX[0], vY[0], vX[2], vY[2]);
            if (dist12 < EPSILON || dist23 < EPSILON || dist13 < EPSILON) {
                // do not use degenerate triangles
                continue;
            }

            bool isClosest = false;
            if (fallbackStrategy_ == FALLBACK_NEAREST_SIDE) {
                // we don't know whether the points of the triangle are given
                // clockwise or counter-clockwise, so we have to check the
                // distance of the point to all three sides of the triangle
                double sq_dist = sq_distance_point_segment(
                    x, y, vX[0], vY[0], vX[1], vY[1], dist12);
                if (sq_dist < closest_sq_dist) {
                    closest_sq_dist = sq_dist;
                    isClosest = true;
                }
                sq_dist = sq_distance_point_segment(x, y, vX[1], vY[1], vX[2],
                                                    vY[2], dist23);
                if (sq_dist < closest_sq_dist) {
                    closest_sq_dist = sq_dist;
                    isClosest = true;
                }
                sq_dist = sq_distance_point_segment(x, y, vX[0], vY[0], vX[2],
                                                    vY[2], dist13);
                if (sq_dist < closest_sq_dist) {
                    closest_sq_dist = sq_dist;
                    isClosest = true;
                }
            } else {
                assert(fallbackStrategy_ == FALLBACK_NEAREST_CENTROID);
                const double c_x = (vX[0] + vX[1] + vX[2]) / 3.0;
                const double c_y = (vY[0] + vY[1] + vY[2]) / 3.0;
                const double sq_dist = squared_distance(x, y, c_x, c_y);
                if (sq_dist < closest_sq_dist) {
                    closest_sq_dist = sq_dist;
                    isClosest = true;
                }
            }
            if (isClosest) {
                closest_dist = sqrt(closest_sq_dist);
                closest_vX = vX;
                closest_vY = vY;
                valsVertex1.clear();
                valsVertex2.clear();
                valsVertex3.clear();
                if (!collectValsVertex())
                    return false;
            }
        }

        if (std::isinf(closest_sq_dist)) {
            // No match: increase search radius
            radius *= 2;
        } else {
            break;
        }
    }
    if (std::isinf(closest_sq_dist)) {
        // nothing was found due to empty triangle list or only degenerate
        // triangles
        return false;
    }

    return computeLambdas(closest_vX, closest_vY,
                          /* checkWithinTriangle = */ false);
}

// ---------------------------------------------------------------------------

TINShiftGeopackageEvaluator::TINShiftGeopackageEvaluator(
    std::unique_ptr<TINShiftGeopackageFile> fileIn)
    : file_(std::move(fileIn)) {}

// ---------------------------------------------------------------------------

bool TINShiftGeopackageEvaluator::transform(bool forwardDirection, double x,
                                            double y, double z, double &x_out,
                                            double &y_out, double &z_out) {
    double lambda1 = 0;
    double lambda2 = 0;
    double lambda3 = 0;
    std::vector<double> valsVertex1;
    std::vector<double> valsVertex2;
    std::vector<double> valsVertex3;
    if (!file_->findTriangle(x, y, forwardDirection, lambda1, lambda2, lambda3,
                             valsVertex1, valsVertex2, valsVertex3))
        return false;

    x_out = x;
    y_out = y;
    z_out = z;

    if (file_->hasHorizontalTransformation()) {
        x_out = lambda1 * valsVertex1[0] + lambda2 * valsVertex2[0] +
                lambda3 * valsVertex3[0];
        y_out = lambda1 * valsVertex1[1] + lambda2 * valsVertex2[1] +
                lambda3 * valsVertex3[1];
    }
    if (file_->hasVerticalTransformation()) {
        const int nextAttrIdx = file_->hasHorizontalTransformation() ? 2 : 0;
        const double sign = forwardDirection ? 1 : -1;
        if (file_->hasOffsetZ()) {
            z_out += sign * (lambda1 * valsVertex1[nextAttrIdx] +
                             lambda2 * valsVertex2[nextAttrIdx] +
                             lambda3 * valsVertex3[nextAttrIdx]);
        } else {
            const double offset1 =
                valsVertex1[nextAttrIdx + 1] - valsVertex1[nextAttrIdx];
            const double offset2 =
                valsVertex2[nextAttrIdx + 1] - valsVertex2[nextAttrIdx];
            const double offset3 =
                valsVertex3[nextAttrIdx + 1] - valsVertex3[nextAttrIdx];
            z_out += sign * (lambda1 * offset1 + lambda2 * offset2 +
                             lambda3 * offset3);
        }
    }

    return true;
}

// ---------------------------------------------------------------------------

bool TINShiftGeopackageEvaluator::forward(double x, double y, double z,
                                          double &x_out, double &y_out,
                                          double &z_out) {
    return transform(/* forwardDirection = */ true, x, y, z, x_out, y_out,
                     z_out);
}

// ---------------------------------------------------------------------------

bool TINShiftGeopackageEvaluator::inverse(double x, double y, double z,
                                          double &x_out, double &y_out,
                                          double &z_out) {
    return transform(/* forwardDirection = */ false, x, y, z, x_out, y_out,
                     z_out);
}

// ---------------------------------------------------------------------------
