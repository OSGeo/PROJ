/******************************************************************************
 * Project:  PROJ
 * Purpose:  Generic grid shifting, in particular Geographic 3D offsets
 * Author:   Even Rouault, <even.rouault at spatialys.com>
 *
 ******************************************************************************
 * Copyright (c) 2022, Even Rouault, <even.rouault at spatialys.com>
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

#define PJ_LIB_

#include <errno.h>
#include <mutex>
#include <stddef.h>
#include <string.h>
#include <time.h>

#include "grids.hpp"
#include "proj_internal.h"

#include <cmath>
#include <limits>
#include <map>

PROJ_HEAD(gridshift, "Generic grid shift");

static std::mutex gMutex{};
static std::set<std::string> gKnownGrids{};

using namespace NS_PROJ;

namespace { // anonymous namespace

struct GridInfo {
    int idxSampleLat = -1;
    int idxSampleLon = -1;
    int idxSampleZ = -1;
    bool bilinearInterpolation = true;
};

// ---------------------------------------------------------------------------

struct gridshiftData {
    ListOfGenericGrids m_grids{};
    bool m_defer_grid_opening = false;
    bool m_bHasHorizontalOffset = false;
    bool m_bHasGeographic3DOffset = false;
    bool m_bHasEllipsoidalHeightOffset = false;
    bool m_bHasVerticalToVertical = false;
    bool m_bHasGeographicToVertical = false;
    bool m_mainGridTypeIsGeographic3DOffset = false;
    std::string m_mainGridType{};
    std::string m_auxGridType{};
    std::string m_interpolation{};
    std::map<const GenericShiftGrid *, GridInfo> m_cacheGridInfo{};

    bool checkGridTypes(PJ *P);
    const GenericShiftGrid *findGrid(const std::string &type,
                                     const PJ_LPZ &input,
                                     GenericShiftGridSet *&gridSetOut) const;
    PJ_LPZ grid_interpolate(PJ_CONTEXT *ctx, const std::string &type, PJ_LP lp,
                            const GenericShiftGrid *grid);
    PJ_LPZ grid_apply_internal(PJ_CONTEXT *ctx, const std::string &type,
                               bool isVerticalOnly, const PJ_LPZ in,
                               PJ_DIRECTION direction,
                               const GenericShiftGrid *grid,
                               GenericShiftGridSet *gridset, bool &shouldRetry);

    PJ_LPZ apply(PJ *P, PJ_DIRECTION dir, PJ_LPZ lpz);
};

// ---------------------------------------------------------------------------

bool gridshiftData::checkGridTypes(PJ *P) {
    for (const auto &gridset : m_grids) {
        for (const auto &grid : gridset->grids()) {
            const auto type = grid->metadataItem("TYPE");
            if (type == "HORIZONTAL_OFFSET")
                m_bHasHorizontalOffset = true;
            else if (type == "GEOGRAPHIC_3D_OFFSET")
                m_bHasGeographic3DOffset = true;
            else if (type == "ELLIPSOIDAL_HEIGHT_OFFSET")
                m_bHasEllipsoidalHeightOffset = true;
            else if (type == "VERTICAL_OFFSET_VERTICAL_TO_VERTICAL")
                m_bHasVerticalToVertical = true;
            else if (type == "VERTICAL_OFFSET_GEOGRAPHIC_TO_VERTICAL")
                m_bHasGeographicToVertical = true;
            else if (type.empty()) {
                proj_log_error(P, _("Missing TYPE metadata item in grid(s)."));
                return false;
            } else {
                proj_log_error(
                    P, _("Unhandled value for TYPE metadata item in grid(s)."));
                return false;
            }
        }
    }

    if (((m_bHasEllipsoidalHeightOffset ? 1 : 0) +
         (m_bHasVerticalToVertical ? 1 : 0) +
         (m_bHasGeographicToVertical ? 1 : 0)) > 1) {
        proj_log_error(P, _("Unsupported mix of grid types."));
        return false;
    }

    if (m_bHasGeographic3DOffset) {
        m_mainGridTypeIsGeographic3DOffset = true;
        m_mainGridType = "GEOGRAPHIC_3D_OFFSET";
    } else if (!m_bHasHorizontalOffset) {
        if (m_bHasEllipsoidalHeightOffset)
            m_mainGridType = "ELLIPSOIDAL_HEIGHT_OFFSET";
        else if (m_bHasGeographicToVertical)
            m_mainGridType = "VERTICAL_OFFSET_GEOGRAPHIC_TO_VERTICAL";
        else {
            assert(m_bHasVerticalToVertical);
            m_mainGridType = "VERTICAL_OFFSET_VERTICAL_TO_VERTICAL";
        }
    } else {
        assert(m_bHasHorizontalOffset);
        m_mainGridType = "HORIZONTAL_OFFSET";
    }

    if (m_bHasHorizontalOffset) {
        if (m_bHasEllipsoidalHeightOffset)
            m_auxGridType = "ELLIPSOIDAL_HEIGHT_OFFSET";
        else if (m_bHasGeographicToVertical)
            m_auxGridType = "VERTICAL_OFFSET_GEOGRAPHIC_TO_VERTICAL";
        else if (m_bHasVerticalToVertical) {
            m_auxGridType = "VERTICAL_OFFSET_VERTICAL_TO_VERTICAL";
        }
    }

    return true;
}

// ---------------------------------------------------------------------------

const GenericShiftGrid *
gridshiftData::findGrid(const std::string &type, const PJ_LPZ &input,
                        GenericShiftGridSet *&gridSetOut) const {
    for (const auto &gridset : m_grids) {
        auto grid = gridset->gridAt(type, input.lam, input.phi);
        if (grid) {
            gridSetOut = gridset.get();
            return grid;
        }
    }
    return nullptr;
}

// ---------------------------------------------------------------------------

typedef struct {
    int32_t lam, phi;
} ILP;

#define REL_TOLERANCE_HGRIDSHIFT 1e-5

PJ_LPZ gridshiftData::grid_interpolate(PJ_CONTEXT *ctx, const std::string &type,
                                       PJ_LP lp, const GenericShiftGrid *grid) {
    PJ_LPZ val;

    val.lam = val.phi = HUGE_VAL;
    val.z = 0;

    auto iterCache = m_cacheGridInfo.find(grid);
    if (iterCache == m_cacheGridInfo.end()) {
        const auto samplesPerPixel = grid->samplesPerPixel();
        int idxSampleLat = -1;
        int idxSampleLon = -1;
        int idxSampleZ = -1;
        for (int i = 0; i < samplesPerPixel; i++) {
            const auto desc = grid->description(i);
            if (desc == "latitude_offset") {
                idxSampleLat = i;
                const auto unit = grid->unit(idxSampleLat);
                if (!unit.empty() && unit != "arc-second") {
                    pj_log(ctx, PJ_LOG_ERROR,
                           "gridshift: Only unit=arc-second currently handled");
                    return val;
                }
            } else if (desc == "longitude_offset") {
                idxSampleLon = i;
                const auto unit = grid->unit(idxSampleLon);
                if (!unit.empty() && unit != "arc-second") {
                    pj_log(ctx, PJ_LOG_ERROR,
                           "gridshift: Only unit=arc-second currently handled");
                    return val;
                }
            } else if (desc == "ellipsoidal_height_offset" ||
                       desc == "geoid_undulation" ||
                       desc == "vertical_offset") {
                idxSampleZ = i;
                const auto unit = grid->unit(idxSampleZ);
                if (!unit.empty() && unit != "metre") {
                    pj_log(ctx, PJ_LOG_ERROR,
                           "gridshift: Only unit=metre currently handled");
                    return val;
                }
            }
        }
        if (samplesPerPixel >= 2 && idxSampleLat < 0 && idxSampleLon < 0 &&
            type == "HORIZONTAL_OFFSET") {
            idxSampleLat = 0;
            idxSampleLon = 1;
        }
        if (type == "HORIZONTAL_OFFSET" || type == "GEOGRAPHIC_3D_OFFSET") {
            if (idxSampleLat < 0 || idxSampleLon < 0) {
                pj_log(ctx, PJ_LOG_ERROR,
                       "gridshift: grid has not expected samples");
                return val;
            }
        }
        if (type == "ELLIPSOIDAL_HEIGHT_OFFSET" ||
            type == "VERTICAL_OFFSET_GEOGRAPHIC_TO_VERTICAL" ||
            type == "VERTICAL_OFFSET_VERTICAL_TO_VERTICAL" ||
            type == "GEOGRAPHIC_3D_OFFSET") {
            if (idxSampleZ < 0) {
                pj_log(ctx, PJ_LOG_ERROR,
                       "gridshift: grid has not expected samples");
                return val;
            }
        }

        std::string interpolation(m_interpolation);
        if (interpolation.empty())
            interpolation = grid->metadataItem("interpolation_method");
        if (interpolation.empty())
            interpolation = "bilinear";

        if (interpolation != "bilinear" && interpolation != "biquadratic") {
            pj_log(ctx, PJ_LOG_ERROR,
                   "gridshift: Unsupported interpolation_method in grid");
            return val;
        }

        GridInfo gridInfo;
        gridInfo.idxSampleLat = idxSampleLat;
        gridInfo.idxSampleLon = idxSampleLon;
        gridInfo.idxSampleZ = idxSampleZ;
        gridInfo.bilinearInterpolation =
            (interpolation == "bilinear" || grid->width() < 3 ||
             grid->height() < 3);
        m_cacheGridInfo[grid] = gridInfo;
        iterCache = m_cacheGridInfo.find(grid);
    }
    const GridInfo &gridInfo = iterCache->second;
    const int idxSampleLat = gridInfo.idxSampleLat;
    const int idxSampleLon = gridInfo.idxSampleLon;
    const int idxSampleZ = gridInfo.idxSampleZ;
    const bool bilinearInterpolation = gridInfo.bilinearInterpolation;

    ILP indx;
    const auto &extent = grid->extentAndRes();
    double lam = (lp.lam - extent.west) / extent.resX;
    indx.lam = std::isnan(lam) ? 0 : (int32_t)lround(floor(lam));
    double phi = (lp.phi - extent.south) / extent.resY;
    indx.phi = std::isnan(phi) ? 0 : (int32_t)lround(floor(phi));

    PJ_LP frct;
    frct.lam = lam - indx.lam;
    frct.phi = phi - indx.phi;
    int tmpInt;
    if (indx.lam < 0) {
        if (indx.lam == -1 && frct.lam > 1 - 10 * REL_TOLERANCE_HGRIDSHIFT) {
            ++indx.lam;
            frct.lam = 0.;
        } else
            return val;
    } else if ((tmpInt = indx.lam + 1) >= grid->width()) {
        if (tmpInt == grid->width() &&
            frct.lam < 10 * REL_TOLERANCE_HGRIDSHIFT) {
            --indx.lam;
            frct.lam = 1.;
        } else
            return val;
    }
    if (indx.phi < 0) {
        if (indx.phi == -1 && frct.phi > 1 - 10 * REL_TOLERANCE_HGRIDSHIFT) {
            ++indx.phi;
            frct.phi = 0.;
        } else
            return val;
    } else if ((tmpInt = indx.phi + 1) >= grid->height()) {
        if (tmpInt == grid->height() &&
            frct.phi < 10 * REL_TOLERANCE_HGRIDSHIFT) {
            --indx.phi;
            frct.phi = 1.;
        } else
            return val;
    }

    constexpr double convFactorLatLon = 1. / 3600 / 180 * M_PI;
    if (bilinearInterpolation) {
        float f00Lon = 0, f00Lat = 0;
        float f10Lon = 0, f10Lat = 0;
        float f01Lon = 0, f01Lat = 0;
        float f11Lon = 0, f11Lat = 0;
        float f00Z = 0, f01Z = 0, f10Z = 0, f11Z = 0;
        if (idxSampleLon >= 0 && idxSampleLat >= 0) {
            if (!grid->valueAt(indx.lam, indx.phi, idxSampleLon, f00Lon) ||
                !grid->valueAt(indx.lam, indx.phi, idxSampleLat, f00Lat) ||
                !grid->valueAt(indx.lam + 1, indx.phi, idxSampleLon, f10Lon) ||
                !grid->valueAt(indx.lam + 1, indx.phi, idxSampleLat, f10Lat) ||
                !grid->valueAt(indx.lam, indx.phi + 1, idxSampleLon, f01Lon) ||
                !grid->valueAt(indx.lam, indx.phi + 1, idxSampleLat, f01Lat) ||
                !grid->valueAt(indx.lam + 1, indx.phi + 1, idxSampleLon,
                               f11Lon) ||
                !grid->valueAt(indx.lam + 1, indx.phi + 1, idxSampleLat,
                               f11Lat)) {
                return val;
            }
        }
        if (idxSampleZ >= 0) {
            if (!grid->valueAt(indx.lam, indx.phi, idxSampleZ, f00Z) ||
                !grid->valueAt(indx.lam + 1, indx.phi, idxSampleZ, f10Z) ||
                !grid->valueAt(indx.lam, indx.phi + 1, idxSampleZ, f01Z) ||
                !grid->valueAt(indx.lam + 1, indx.phi + 1, idxSampleZ, f11Z)) {
                return val;
            }
        }

        double m10 = frct.lam;
        double m11 = m10;
        double m01 = 1. - frct.lam;
        double m00 = m01;
        m11 *= frct.phi;
        m01 *= frct.phi;
        frct.phi = 1. - frct.phi;
        m00 *= frct.phi;
        m10 *= frct.phi;
        if (idxSampleLon >= 0) {
            val.lam =
                (m00 * f00Lon + m10 * f10Lon + m01 * f01Lon + m11 * f11Lon) *
                convFactorLatLon;
            val.phi =
                (m00 * f00Lat + m10 * f10Lat + m01 * f01Lat + m11 * f11Lat) *
                convFactorLatLon;
        } else {
            val.lam = 0;
            val.phi = 0;
        }
        if (idxSampleZ >= 0) {
            val.z = m00 * f00Z + m10 * f10Z + m01 * f01Z + m11 * f11Z;
        }
    } else // biquadratic
    {
        // Cf https://geodesy.noaa.gov/library/pdfs/NOAA_TM_NOS_NGS_0084.pdf
        // Depending if we are before or after half-pixel, shift the 3x3 window
        // of interpolation
        if ((frct.lam <= 0.5 && indx.lam > 0) ||
            (indx.lam + 2 == grid->width())) {
            indx.lam -= 1;
            frct.lam += 1;
        }
        if ((frct.phi <= 0.5 && indx.phi > 0) ||
            (indx.phi + 2 == grid->height())) {
            indx.phi -= 1;
            frct.phi += 1;
        }

        // Port of qterp() Fortran function from NOAA
        // x must be in [0,2] range
        // f0 must be f(0), f1 must be f(1), f2 must be f(2)
        // Returns f(x) interpolated value along the parabolic function
        const auto quadraticInterpol = [](double x, double f0, double f1,
                                          double f2) {
            const double df0 = f1 - f0;
            const double df1 = f2 - f1;
            const double d2f0 = df1 - df0;
            return f0 + x * df0 + 0.5 * x * (x - 1.0) * d2f0;
        };

        if (idxSampleLon >= 0 && idxSampleLat >= 0) {
            double lon_shift[3];
            double lat_shift[3];
            float lon_shift_y[3] = {0};
            float lat_shift_y[3] = {0};
            for (int j = 0; j <= 2; ++j) {
                for (int i = 0; i <= 2; ++i) {
                    if (!grid->valueAt(indx.lam + i, indx.phi + j, idxSampleLon,
                                       lon_shift_y[i]))
                        return val;
                    if (!grid->valueAt(indx.lam + i, indx.phi + j, idxSampleLat,
                                       lat_shift_y[i]))
                        return val;
                }
                lon_shift[j] = quadraticInterpol(
                    frct.lam, lon_shift_y[0], lon_shift_y[1], lon_shift_y[2]);
                lat_shift[j] = quadraticInterpol(
                    frct.lam, lat_shift_y[0], lat_shift_y[1], lat_shift_y[2]);
            }
            val.lam = quadraticInterpol(frct.phi, lon_shift[0], lon_shift[1],
                                        lon_shift[2]) *
                      convFactorLatLon;
            val.phi = quadraticInterpol(frct.phi, lat_shift[0], lat_shift[1],
                                        lat_shift[2]) *
                      convFactorLatLon;
        } else {
            val.lam = 0;
            val.phi = 0;
        }
        if (idxSampleZ >= 0) {
            double z_shift[3];
            float z_shift_y[3] = {0};
            for (int j = 0; j <= 2; ++j) {
                for (int i = 0; i <= 2; ++i) {
                    if (!grid->valueAt(indx.lam + i, indx.phi + j, idxSampleZ,
                                       z_shift_y[i])) {
                        val.lam = val.phi = HUGE_VAL;
                        return val;
                    }
                }
                z_shift[j] = quadraticInterpol(frct.lam, z_shift_y[0],
                                               z_shift_y[1], z_shift_y[2]);
            }
            val.z =
                quadraticInterpol(frct.phi, z_shift[0], z_shift[1], z_shift[2]);
        }
    }

    return val;
}

// ---------------------------------------------------------------------------

static PJ_LP normalizeLongitude(const GenericShiftGrid *grid, const PJ_LPZ in,
                                const osgeo::proj::ExtentAndRes *&extentOut) {
    PJ_LP normalized;
    normalized.lam = in.lam;
    normalized.phi = in.phi;
    extentOut = &(grid->extentAndRes());
    const double epsilon =
        (extentOut->resX + extentOut->resY) * REL_TOLERANCE_HGRIDSHIFT;
    if (normalized.lam < extentOut->west - epsilon)
        normalized.lam += 2 * M_PI;
    else if (normalized.lam > extentOut->east + epsilon)
        normalized.lam -= 2 * M_PI;
    return normalized;
}

// ---------------------------------------------------------------------------

#define MAX_ITERATIONS 10
#define TOL 1e-12

PJ_LPZ gridshiftData::grid_apply_internal(
    PJ_CONTEXT *ctx, const std::string &type, bool isVerticalOnly,
    const PJ_LPZ in, PJ_DIRECTION direction, const GenericShiftGrid *grid,
    GenericShiftGridSet *gridset, bool &shouldRetry) {

    shouldRetry = false;
    if (in.lam == HUGE_VAL)
        return in;

    /* normalized longitude of input */
    const osgeo::proj::ExtentAndRes *extent;
    PJ_LP normalized_in = normalizeLongitude(grid, in, extent);

    PJ_LPZ shift = grid_interpolate(ctx, type, normalized_in, grid);
    if (grid->hasChanged()) {
        shouldRetry = gridset->reopen(ctx);
        PJ_LPZ out;
        out.lam = out.phi = out.z = HUGE_VAL;
        return out;
    }
    if (shift.lam == HUGE_VAL)
        return shift;

    if (direction == PJ_FWD) {
        PJ_LPZ out = in;
        out.lam += shift.lam;
        out.phi += shift.phi;
        out.z += shift.z;
        return out;
    }

    if (isVerticalOnly) {
        PJ_LPZ out = in;
        out.z -= shift.z;
        return out;
    }

    PJ_LP guess;
    guess.lam = normalized_in.lam - shift.lam;
    guess.phi = normalized_in.phi - shift.phi;

    int i = MAX_ITERATIONS;
    const double toltol = TOL * TOL;
    PJ_LP diff;
    do {
        shift = grid_interpolate(ctx, type, guess, grid);
        if (grid->hasChanged()) {
            shouldRetry = gridset->reopen(ctx);
            PJ_LPZ out;
            out.lam = out.phi = out.z = HUGE_VAL;
            return out;
        }

        /* We can possibly go outside of the initial guessed grid, so try */
        /* to fetch a new grid into which iterate... */
        if (shift.lam == HUGE_VAL) {
            PJ_LPZ lp;
            lp.lam = guess.lam;
            lp.phi = guess.phi;
            auto newGrid = findGrid(type, lp, gridset);
            if (newGrid == nullptr || newGrid == grid || newGrid->isNullGrid())
                break;
            pj_log(ctx, PJ_LOG_TRACE, "Switching from grid %s to grid %s",
                   grid->name().c_str(), newGrid->name().c_str());
            grid = newGrid;
            normalized_in = normalizeLongitude(grid, in, extent);
            diff.lam = std::numeric_limits<double>::max();
            diff.phi = std::numeric_limits<double>::max();
            continue;
        }

        diff.lam = guess.lam + shift.lam - normalized_in.lam;
        diff.phi = guess.phi + shift.phi - normalized_in.phi;
        guess.lam -= diff.lam;
        guess.phi -= diff.phi;

    } while (--i && (diff.lam * diff.lam + diff.phi * diff.phi >
                     toltol)); /* prob. slightly faster than hypot() */

    if (i == 0) {
        pj_log(ctx, PJ_LOG_TRACE,
               "Inverse grid shift iterator failed to converge.");
        proj_context_errno_set(ctx, PROJ_ERR_COORD_TRANSFM);
        PJ_LPZ out;
        out.lam = out.phi = out.z = HUGE_VAL;
        return out;
    }

    if (shift.lam == HUGE_VAL) {
        pj_log(ctx, PJ_LOG_TRACE,
               "Inverse grid shift iteration failed, presumably at grid edge. "
               "Using first approximation.");
    }

    PJ_LPZ out;
    out.lam = adjlon(guess.lam);
    out.phi = guess.phi;
    out.z = in.z - shift.z;
    return out;
}

// ---------------------------------------------------------------------------

static const std::string sHORIZONTAL_OFFSET("HORIZONTAL_OFFSET");

PJ_LPZ gridshiftData::apply(PJ *P, PJ_DIRECTION direction, PJ_LPZ lpz) {
    if (m_defer_grid_opening) {
        m_defer_grid_opening = false;
        m_grids = pj_generic_grid_init(P, "grids");
        if (proj_errno(P)) {
            return proj_coord_error().lpz;
        }
        if (!checkGridTypes(P)) {
            return proj_coord_error().lpz;
        }
    }

    PJ_LPZ out;

    out.lam = HUGE_VAL;
    out.phi = HUGE_VAL;
    out.z = HUGE_VAL;

    std::string &type = m_mainGridType;
    bool bFoundGeog3DOffset = false;
    while (true) {
        GenericShiftGridSet *gridset = nullptr;
        const GenericShiftGrid *grid = findGrid(type, lpz, gridset);
        if (!grid) {
            if (m_mainGridTypeIsGeographic3DOffset && m_bHasHorizontalOffset) {
                // If we have a mix of grids with GEOGRAPHIC_3D_OFFSET
                // and HORIZONTAL_OFFSET+ELLIPSOIDAL_HEIGHT_OFFSET
                type = sHORIZONTAL_OFFSET;
                grid = findGrid(type, lpz, gridset);
            }
            if (!grid) {
                proj_context_errno_set(P->ctx,
                                       PROJ_ERR_COORD_TRANSFM_OUTSIDE_GRID);
                return out;
            }
        } else {
            if (m_mainGridTypeIsGeographic3DOffset)
                bFoundGeog3DOffset = true;
        }

        if (grid->isNullGrid()) {
            out = lpz;
            break;
        }
        bool shouldRetry = false;
        out = grid_apply_internal(
            P->ctx, type, !(m_bHasGeographic3DOffset || m_bHasHorizontalOffset),
            lpz, direction, grid, gridset, shouldRetry);
        if (!shouldRetry) {
            break;
        }
    }

    if (out.lam == HUGE_VAL || out.phi == HUGE_VAL) {
        proj_context_errno_set(P->ctx, PROJ_ERR_COORD_TRANSFM_OUTSIDE_GRID);
        return out;
    }

    // Second pass to apply vertical transformation, if it is in a
    // separate grid than lat-lon offsets.
    if (!bFoundGeog3DOffset && !m_auxGridType.empty()) {
        lpz = out;
        while (true) {
            GenericShiftGridSet *gridset = nullptr;
            const auto grid = findGrid(m_auxGridType, lpz, gridset);
            if (!grid) {
                proj_context_errno_set(P->ctx,
                                       PROJ_ERR_COORD_TRANSFM_OUTSIDE_GRID);
                return out;
            }
            if (grid->isNullGrid()) {
                break;
            }

            bool shouldRetry = false;
            out = grid_apply_internal(P->ctx, m_auxGridType, true, lpz,
                                      direction, grid, gridset, shouldRetry);
            if (!shouldRetry) {
                break;
            }
        }

        if (out.lam == HUGE_VAL || out.phi == HUGE_VAL) {
            proj_context_errno_set(P->ctx, PROJ_ERR_COORD_TRANSFM_OUTSIDE_GRID);
            return out;
        }
    }

    return out;
}

} // anonymous namespace

// ---------------------------------------------------------------------------

static PJ_XYZ forward_3d(PJ_LPZ lpz, PJ *P) {
    auto Q = static_cast<gridshiftData *>(P->opaque);
    PJ_COORD point = {{0, 0, 0, 0}};

    point.lpz = Q->apply(P, PJ_FWD, lpz);

    return point.xyz;
}

// ---------------------------------------------------------------------------

static PJ_LPZ reverse_3d(PJ_XYZ xyz, PJ *P) {
    auto Q = static_cast<gridshiftData *>(P->opaque);
    PJ_COORD point = {{0, 0, 0, 0}};
    point.xyz = xyz;

    point.lpz = Q->apply(P, PJ_INV, point.lpz);

    return point.lpz;
}

// ---------------------------------------------------------------------------

static PJ *destructor(PJ *P, int errlev) {
    if (nullptr == P)
        return nullptr;

    delete static_cast<struct gridshiftData *>(P->opaque);
    P->opaque = nullptr;

    return pj_default_destructor(P, errlev);
}

// ---------------------------------------------------------------------------

static void reassign_context(PJ *P, PJ_CONTEXT *ctx) {
    auto Q = (struct gridshiftData *)P->opaque;
    for (auto &grid : Q->m_grids) {
        grid->reassign_context(ctx);
    }
}

// ---------------------------------------------------------------------------

PJ *TRANSFORMATION(gridshift, 0) {
    auto Q = new gridshiftData;
    P->opaque = (void *)Q;
    P->destructor = destructor;
    P->reassign_context = reassign_context;

    P->fwd3d = forward_3d;
    P->inv3d = reverse_3d;
    P->fwd = nullptr;
    P->inv = nullptr;

    P->left = PJ_IO_UNITS_RADIANS;
    P->right = PJ_IO_UNITS_RADIANS;

    if (0 == pj_param(P->ctx, P->params, "tgrids").i) {
        proj_log_error(P, _("+grids parameter missing."));
        return destructor(P, PROJ_ERR_INVALID_OP_MISSING_ARG);
    }

    if (P->ctx->defer_grid_opening) {
        Q->m_defer_grid_opening = true;
    } else {
        const char *gridnames = pj_param(P->ctx, P->params, "sgrids").s;
        gMutex.lock();
        const bool isKnownGrid =
            gKnownGrids.find(gridnames) != gKnownGrids.end();
        gMutex.unlock();
        if (isKnownGrid) {
            Q->m_defer_grid_opening = true;
        } else {
            Q->m_grids = pj_generic_grid_init(P, "grids");
            /* Was gridlist compiled properly? */
            if (proj_errno(P)) {
                proj_log_error(P, _("could not find required grid(s)."));
                return destructor(
                    P, PROJ_ERR_INVALID_OP_FILE_NOT_FOUND_OR_INVALID);
            }
            if (!Q->checkGridTypes(P)) {
                return destructor(
                    P, PROJ_ERR_INVALID_OP_FILE_NOT_FOUND_OR_INVALID);
            }

            gMutex.lock();
            gKnownGrids.insert(gridnames);
            gMutex.unlock();
        }
    }

    if (pj_param(P->ctx, P->params, "tinterpolation").i) {
        const char *interpolation =
            pj_param(P->ctx, P->params, "sinterpolation").s;
        if (strcmp(interpolation, "bilinear") == 0 ||
            strcmp(interpolation, "biquadratic") == 0) {
            Q->m_interpolation = interpolation;
        } else {
            proj_log_error(P, _("Unsupported value for +interpolation."));
            return destructor(P, PROJ_ERR_INVALID_OP_ILLEGAL_ARG_VALUE);
        }
    }

    return P;
}

// ---------------------------------------------------------------------------

void pj_clear_gridshift_knowngrids_cache() {
    std::lock_guard<std::mutex> lock(gMutex);
    gKnownGrids.clear();
}
