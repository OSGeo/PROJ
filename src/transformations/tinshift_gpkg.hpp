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

#ifndef TINSHIFT_GEOPACKAGE_H
#define TINSHIFT_GEOPACKAGE_H

#include <array>
#include <exception>
#include <memory>
#include <string>
#include <vector>

#include "sqlite3_utils.hpp"
#include "tinshift_iface.hpp"

struct sqlite3;
struct sqlite3_stmt;

// ---------------------------------------------------------------------------

class TINShiftGeopackageException : public std::exception {
  public:
    explicit TINShiftGeopackageException(const std::string &msg) : msg_(msg) {}
    const char *what() const noexcept override;

  private:
    std::string msg_;
};

// ---------------------------------------------------------------------------

class TINShiftGeopackageFile {
  public:
    /** Open the provided GeoPackage and return an object.
     *
     * @throws TINShiftGeopackageException in case of error.
     */
    static std::unique_ptr<TINShiftGeopackageFile>
    open(PJ_CONTEXT *ctx, const std::string &filename);

    /** Destructor */
    ~TINShiftGeopackageFile();

    /** Find the triangle into which (x, y) is located */
    bool findTriangle(double x, double y, bool forwardDirection,
                      double &lambda1, double &lambda2, double &lambda3,
                      std::vector<double> &valsVertex1,
                      std::vector<double> &valsVertex2,
                      std::vector<double> &valsVertex3);

    bool hasHorizontalTransformation() const {
        return horizontalTransformation_;
    }
    bool hasVerticalTransformation() const { return verticalTransformation_; }
    bool hasOffsetZ() const { return offsetZ_; }

  private:
    std::unique_ptr<NS_PROJ::SQLite3VFS> sqlite_vfs_{};
    sqlite3 *sqlite_handle_ = nullptr;
    sqlite3_stmt *select_stmt_ = nullptr;
    std::vector<std::string> vertices_cols_{};
    int numVertices_ = 0;
    bool horizontalTransformation_ = false;
    bool verticalTransformation_ = false;
    bool sourceZ_ = false;
    bool targetX_ = false;
    bool targetY_ = false;
    bool targetZ_ = false;
    bool offsetZ_ = false;
    double min_x_ = 0;
    double min_y_ = 0;
    double max_x_ = 0;
    double max_y_ = 0;
    double min_shift_x_ = 0;
    double min_shift_y_ = 0;
    double max_shift_x_ = 0;
    double max_shift_y_ = 0;

    enum FallbackStrategy {
        FALLBACK_NONE,
        FALLBACK_NEAREST_SIDE,
        FALLBACK_NEAREST_CENTROID,
    };
    FallbackStrategy fallbackStrategy_ = FALLBACK_NONE;

    // Cached results
    bool cachedForwardDirection_ = false;
    std::array<double, 3> cachedVerticesX_{0, 0, 0};
    std::array<double, 3> cachedVerticesY_{0, 0, 0};
    std::vector<double> cachedValsVertex1_{};
    std::vector<double> cachedValsVertex2_{};
    std::vector<double> cachedValsVertex3_{};

  private:
    TINShiftGeopackageFile() = default;
    TINShiftGeopackageFile(const TINShiftGeopackageFile &) = delete;
    TINShiftGeopackageFile &operator=(const TINShiftGeopackageFile &) = delete;

    void getMetadata();
    void prepareDatabaseQuery();
};

// ---------------------------------------------------------------------------

/** Class to evaluate the transformation of a coordinate */
class TINShiftGeopackageEvaluator : public TINShiftEvaluator {
  public:
    /** Constructor. */
    explicit TINShiftGeopackageEvaluator(
        std::unique_ptr<TINShiftGeopackageFile> fileIn);

    bool forward(double x, double y, double z, double &x_out, double &y_out,
                 double &z_out) override;

    bool inverse(double x, double y, double z, double &x_out, double &y_out,
                 double &z_out) override;

  private:
    std::unique_ptr<TINShiftGeopackageFile> file_;

    bool transform(bool forwardDirection, double x, double y, double z,
                   double &x_out, double &y_out, double &z_out);
};

// ---------------------------------------------------------------------------

#endif
