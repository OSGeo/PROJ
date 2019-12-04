/******************************************************************************
 * Project:  PROJ
 * Purpose:  Grid management
 * Author:   Even Rouault, <even.rouault at spatialys.com>
 *
 ******************************************************************************
 * Copyright (c) 2000, Frank Warmerdam <warmerdam@pobox.com>
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

#include "grids.hpp"
#include "proj/internal/internal.hpp"
#include "proj_internal.h"

NS_PROJ_START

using namespace internal;

/************************************************************************/
/*                             swap_words()                             */
/*                                                                      */
/*      Convert the byte order of the given word(s) in place.           */
/************************************************************************/

static const int byte_order_test = 1;
#define IS_LSB (1 == ((const unsigned char *)(&byte_order_test))[0])

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

bool ExtentAndRes::fullWorldLongitude() const {
    return eastLon - westLon + resLon >= 2 * M_PI - 1e-10;
}

// ---------------------------------------------------------------------------

VerticalShiftGrid::VerticalShiftGrid(int widthIn, int heightIn,
                                     const ExtentAndRes &extentIn)
    : m_width(widthIn), m_height(heightIn), m_extent(extentIn) {}

// ---------------------------------------------------------------------------

VerticalShiftGrid::~VerticalShiftGrid() = default;

// ---------------------------------------------------------------------------

static ExtentAndRes globalExtent() {
    ExtentAndRes extent;
    extent.westLon = -M_PI;
    extent.southLat = -M_PI / 2;
    extent.eastLon = M_PI;
    extent.northLat = M_PI / 2;
    extent.resLon = M_PI;
    extent.resLat = M_PI / 2;
    return extent;
}

// ---------------------------------------------------------------------------

class NullVerticalShiftGrid : public VerticalShiftGrid {

  public:
    NullVerticalShiftGrid() : VerticalShiftGrid(3, 3, globalExtent()) {}

    bool valueAt(int, int, float &out) const override;
    bool isNodata(float, double) const override { return false; }
};

// ---------------------------------------------------------------------------

bool NullVerticalShiftGrid::valueAt(int, int, float &out) const {
    out = 0.0f;
    return true;
}

// ---------------------------------------------------------------------------

class GTXVerticalShiftGrid : public VerticalShiftGrid {
    PJ_CONTEXT *m_ctx;
    PAFile m_fp;

    GTXVerticalShiftGrid(const GTXVerticalShiftGrid &) = delete;
    GTXVerticalShiftGrid &operator=(const GTXVerticalShiftGrid &) = delete;

  public:
    GTXVerticalShiftGrid(PJ_CONTEXT *ctx, PAFile fp, int widthIn, int heightIn,
                         const ExtentAndRes &extentIn)
        : VerticalShiftGrid(widthIn, heightIn, extentIn), m_ctx(ctx), m_fp(fp) {
    }

    ~GTXVerticalShiftGrid() override;

    bool valueAt(int x, int y, float &out) const override;
    bool isNodata(float val, double multiplier) const override;

    static GTXVerticalShiftGrid *open(PJ_CONTEXT *ctx, PAFile fp);
};

// ---------------------------------------------------------------------------

GTXVerticalShiftGrid::~GTXVerticalShiftGrid() { pj_ctx_fclose(m_ctx, m_fp); }

// ---------------------------------------------------------------------------

GTXVerticalShiftGrid *GTXVerticalShiftGrid::open(PJ_CONTEXT *ctx, PAFile fp) {
    unsigned char header[40];

    /* -------------------------------------------------------------------- */
    /*      Read the header.                                                */
    /* -------------------------------------------------------------------- */
    if (pj_ctx_fread(ctx, header, sizeof(header), 1, fp) != 1) {
        pj_ctx_set_errno(ctx, PJD_ERR_FAILED_TO_LOAD_GRID);
        return nullptr;
    }

    /* -------------------------------------------------------------------- */
    /*      Regularize fields of interest and extract.                      */
    /* -------------------------------------------------------------------- */
    if (IS_LSB) {
        swap_words(header + 0, 8, 4);
        swap_words(header + 32, 4, 2);
    }

    double xorigin, yorigin, xstep, ystep;
    int rows, columns;

    memcpy(&yorigin, header + 0, 8);
    memcpy(&xorigin, header + 8, 8);
    memcpy(&ystep, header + 16, 8);
    memcpy(&xstep, header + 24, 8);

    memcpy(&rows, header + 32, 4);
    memcpy(&columns, header + 36, 4);

    if (xorigin < -360 || xorigin > 360 || yorigin < -90 || yorigin > 90) {
        pj_log(ctx, PJ_LOG_ERROR,
               "gtx file header has invalid extents, corrupt?");
        pj_ctx_set_errno(ctx, PJD_ERR_FAILED_TO_LOAD_GRID);
        return nullptr;
    }

    /* some GTX files come in 0-360 and we shift them back into the
       expected -180 to 180 range if possible.  This does not solve
       problems with grids spanning the dateline. */
    if (xorigin >= 180.0)
        xorigin -= 360.0;

    if (xorigin >= 0.0 && xorigin + xstep * columns > 180.0) {
        pj_log(ctx, PJ_LOG_DEBUG_MAJOR,
               "This GTX spans the dateline!  This will cause problems.");
    }

    ExtentAndRes extent;
    extent.westLon = xorigin * DEG_TO_RAD;
    extent.southLat = yorigin * DEG_TO_RAD;
    extent.resLon = xstep * DEG_TO_RAD;
    extent.resLat = ystep * DEG_TO_RAD;
    extent.eastLon = (xorigin + xstep * (columns - 1)) * DEG_TO_RAD;
    extent.northLat = (yorigin + ystep * (rows - 1)) * DEG_TO_RAD;

    return new GTXVerticalShiftGrid(ctx, fp, columns, rows, extent);
}

// ---------------------------------------------------------------------------

bool GTXVerticalShiftGrid::valueAt(int x, int y, float &out) const {
    assert(x >= 0 && y >= 0 && x < m_width && y < m_height);

    pj_ctx_fseek(m_ctx, m_fp, 40 + sizeof(float) * (y * m_width + x), SEEK_SET);
    if (pj_ctx_fread(m_ctx, &out, sizeof(out), 1, m_fp) != 1) {
        pj_ctx_set_errno(m_ctx, PJD_ERR_FAILED_TO_LOAD_GRID);
        return false;
    }
    if (IS_LSB) {
        swap_words(&out, sizeof(float), 1);
    }
    return true;
}

// ---------------------------------------------------------------------------

bool GTXVerticalShiftGrid::isNodata(float val, double multiplier) const {
    /* nodata?  */
    /* GTX official nodata value if  -88.88880f, but some grids also */
    /* use other  big values for nodata (e.g naptrans2008.gtx has */
    /* nodata values like -2147479936), so test them too */
    return val * multiplier > 1000 || val * multiplier < -1000 ||
           val == -88.88880f;
}

// ---------------------------------------------------------------------------

VerticalShiftGridSet::VerticalShiftGridSet() = default;

// ---------------------------------------------------------------------------

VerticalShiftGridSet::~VerticalShiftGridSet() = default;

// ---------------------------------------------------------------------------

std::unique_ptr<VerticalShiftGridSet>
VerticalShiftGridSet::open(PJ_CONTEXT *ctx, const std::string &filename) {
    if (filename == "null") {
        auto set =
            std::unique_ptr<VerticalShiftGridSet>(new VerticalShiftGridSet());
        set->m_name = filename;
        set->m_format = "null";
        set->m_grids.push_back(std::unique_ptr<NullVerticalShiftGrid>(
            new NullVerticalShiftGrid()));
        return set;
    }

    PAFile fp;
    if (!(fp = pj_open_lib(ctx, filename.c_str(), "rb"))) {
        ctx->last_errno = 0; /* don't treat as a persistent error */
        return nullptr;
    }
    if (ends_with(filename, "gtx") || ends_with(filename, "GTX")) {
        auto grid = GTXVerticalShiftGrid::open(ctx, fp);
        if (!grid) {
            pj_ctx_fclose(ctx, fp);
            return nullptr;
        }
        auto set =
            std::unique_ptr<VerticalShiftGridSet>(new VerticalShiftGridSet());
        set->m_name = filename;
        set->m_format = "gtx";
        set->m_grids.push_back(std::unique_ptr<VerticalShiftGrid>(grid));
        return set;
    }

    pj_log(ctx, PJ_LOG_DEBUG_MAJOR, "Unrecognized vertical grid format");
    pj_ctx_fclose(ctx, fp);
    return nullptr;
}

// ---------------------------------------------------------------------------

const VerticalShiftGrid *VerticalShiftGrid::gridAt(double lon,
                                                   double lat) const {
    for (const auto &child : m_children) {
        const auto &extentChild = child->extentAndRes();
        if ((extentChild.fullWorldLongitude() ||
             (lon >= extentChild.westLon && lon <= extentChild.eastLon)) &&
            lat >= extentChild.southLat && lat <= extentChild.northLat) {
            return child->gridAt(lon, lat);
        }
    }
    return this;
}
// ---------------------------------------------------------------------------

const VerticalShiftGrid *VerticalShiftGridSet::gridAt(double lon,
                                                      double lat) const {
    for (const auto &grid : m_grids) {
        if (dynamic_cast<NullVerticalShiftGrid *>(grid.get())) {
            return grid.get();
        }
        const auto &extent = grid->extentAndRes();
        if ((extent.fullWorldLongitude() ||
             (lon >= extent.westLon && lon <= extent.eastLon)) &&
            lat >= extent.southLat && lat <= extent.northLat) {
            return grid->gridAt(lon, lat);
        }
    }
    return nullptr;
}

NS_PROJ_END
