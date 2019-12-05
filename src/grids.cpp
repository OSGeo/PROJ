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

// ---------------------------------------------------------------------------

HorizontalShiftGrid::HorizontalShiftGrid(int widthIn, int heightIn,
                                         const ExtentAndRes &extentIn)
    : m_width(widthIn), m_height(heightIn), m_extent(extentIn) {}

// ---------------------------------------------------------------------------

HorizontalShiftGrid::~HorizontalShiftGrid() = default;

// ---------------------------------------------------------------------------

HorizontalShiftGridSet::HorizontalShiftGridSet() = default;

// ---------------------------------------------------------------------------

HorizontalShiftGridSet::~HorizontalShiftGridSet() = default;

// ---------------------------------------------------------------------------

class NullHorizontalShiftGrid : public HorizontalShiftGrid {

  public:
    NullHorizontalShiftGrid() : HorizontalShiftGrid(3, 3, globalExtent()) {}

    bool isNullGrid() const override { return true; }

    bool valueAt(int, int, float &lonShift, float &latShift) const override;
};

// ---------------------------------------------------------------------------

bool NullHorizontalShiftGrid::valueAt(int, int, float &lonShift,
                                      float &latShift) const {
    lonShift = 0.0f;
    latShift = 0.0f;
    return true;
}

// ---------------------------------------------------------------------------

static double to_double(const void *data) {
    double d;
    memcpy(&d, data, sizeof(d));
    return d;
}

// ---------------------------------------------------------------------------

class NTv1Grid : public HorizontalShiftGrid {
    PJ_CONTEXT *m_ctx;
    PAFile m_fp;

    NTv1Grid(const NTv1Grid &) = delete;
    NTv1Grid &operator=(const NTv1Grid &) = delete;

  public:
    NTv1Grid(PJ_CONTEXT *ctx, PAFile fp, int widthIn, int heightIn,
             const ExtentAndRes &extentIn)
        : HorizontalShiftGrid(widthIn, heightIn, extentIn), m_ctx(ctx),
          m_fp(fp) {}

    ~NTv1Grid() override;

    bool valueAt(int, int, float &lonShift, float &latShift) const override;

    static NTv1Grid *open(PJ_CONTEXT *ctx, PAFile fp,
                          const std::string &filename);
};

// ---------------------------------------------------------------------------

NTv1Grid::~NTv1Grid() { pj_ctx_fclose(m_ctx, m_fp); }

// ---------------------------------------------------------------------------

NTv1Grid *NTv1Grid::open(PJ_CONTEXT *ctx, PAFile fp,
                         const std::string &filename) {
    unsigned char header[192];

    /* -------------------------------------------------------------------- */
    /*      Read the header.                                                */
    /* -------------------------------------------------------------------- */
    if (pj_ctx_fread(ctx, header, sizeof(header), 1, fp) != 1) {
        pj_ctx_set_errno(ctx, PJD_ERR_FAILED_TO_LOAD_GRID);
        return nullptr;
    }

    /* -------------------------------------------------------------------- */
    /*      Regularize fields of interest.                                  */
    /* -------------------------------------------------------------------- */
    if (IS_LSB) {
        swap_words(header + 8, sizeof(int), 1);
        swap_words(header + 24, sizeof(double), 1);
        swap_words(header + 40, sizeof(double), 1);
        swap_words(header + 56, sizeof(double), 1);
        swap_words(header + 72, sizeof(double), 1);
        swap_words(header + 88, sizeof(double), 1);
        swap_words(header + 104, sizeof(double), 1);
    }

    if (*((int *)(header + 8)) != 12) {
        pj_log(ctx, PJ_LOG_ERROR,
               "NTv1 grid shift file has wrong record count, corrupt?");
        pj_ctx_set_errno(ctx, PJD_ERR_FAILED_TO_LOAD_GRID);
        return nullptr;
    }

    ExtentAndRes extent;
    extent.westLon = -to_double(header + 72) * DEG_TO_RAD;
    extent.southLat = to_double(header + 24) * DEG_TO_RAD;
    extent.eastLon = -to_double(header + 56) * DEG_TO_RAD;
    extent.northLat = to_double(header + 40) * DEG_TO_RAD;
    extent.resLon = to_double(header + 104) * DEG_TO_RAD;
    extent.resLat = to_double(header + 88) * DEG_TO_RAD;
    if (!(fabs(extent.westLon) <= 4 * M_PI &&
          fabs(extent.eastLon) <= 4 * M_PI &&
          fabs(extent.northLat) <= M_PI + 1e-5 &&
          fabs(extent.southLat) <= M_PI + 1e-5 &&
          extent.westLon < extent.eastLon &&
          extent.southLat < extent.northLat && extent.resLon > 1e-10 &&
          extent.resLat > 1e-10)) {
        pj_log(ctx, PJ_LOG_ERROR, "Inconsistent georeferencing for %s",
               filename.c_str());
        pj_ctx_set_errno(ctx, PJD_ERR_FAILED_TO_LOAD_GRID);
        return nullptr;
    }
    const int columns = static_cast<int>(
        fabs((extent.eastLon - extent.westLon) / extent.resLon + 0.5) + 1);
    const int rows = static_cast<int>(
        fabs((extent.northLat - extent.southLat) / extent.resLat + 0.5) + 1);

    return new NTv1Grid(ctx, fp, columns, rows, extent);
}

// ---------------------------------------------------------------------------

bool NTv1Grid::valueAt(int x, int y, float &lonShift, float &latShift) const {
    assert(x >= 0 && y >= 0 && x < m_width && y < m_height);

    double two_doubles[2];
    // NTv1 is organized from east to west !
    pj_ctx_fseek(m_ctx, m_fp,
                 192 + 2 * sizeof(double) * (y * m_width + m_width - 1 - x),
                 SEEK_SET);
    if (pj_ctx_fread(m_ctx, &two_doubles[0], sizeof(two_doubles), 1, m_fp) !=
        1) {
        pj_ctx_set_errno(m_ctx, PJD_ERR_FAILED_TO_LOAD_GRID);
        return false;
    }
    if (IS_LSB) {
        swap_words(&two_doubles[0], sizeof(double), 2);
    }
    /* convert seconds to radians */
    latShift = static_cast<float>(two_doubles[0] * ((M_PI / 180.0) / 3600.0));
    lonShift = static_cast<float>(two_doubles[1] * ((M_PI / 180.0) / 3600.0));

    return true;
}

// ---------------------------------------------------------------------------

class CTable2Grid : public HorizontalShiftGrid {
    PJ_CONTEXT *m_ctx;
    PAFile m_fp;

    CTable2Grid(const CTable2Grid &) = delete;
    CTable2Grid &operator=(const CTable2Grid &) = delete;

  public:
    CTable2Grid(PJ_CONTEXT *ctx, PAFile fp, int widthIn, int heightIn,
                const ExtentAndRes &extentIn)
        : HorizontalShiftGrid(widthIn, heightIn, extentIn), m_ctx(ctx),
          m_fp(fp) {}

    ~CTable2Grid() override;

    bool valueAt(int, int, float &lonShift, float &latShift) const override;

    static CTable2Grid *open(PJ_CONTEXT *ctx, PAFile fp,
                             const std::string &filename);
};

// ---------------------------------------------------------------------------

CTable2Grid::~CTable2Grid() { pj_ctx_fclose(m_ctx, m_fp); }

// ---------------------------------------------------------------------------

CTable2Grid *CTable2Grid::open(PJ_CONTEXT *ctx, PAFile fp,
                               const std::string &filename) {
    unsigned char header[160];

    /* -------------------------------------------------------------------- */
    /*      Read the header.                                                */
    /* -------------------------------------------------------------------- */
    if (pj_ctx_fread(ctx, header, sizeof(header), 1, fp) != 1) {
        pj_ctx_set_errno(ctx, PJD_ERR_FAILED_TO_LOAD_GRID);
        return nullptr;
    }

    /* -------------------------------------------------------------------- */
    /*      Regularize fields of interest.                                  */
    /* -------------------------------------------------------------------- */
    if (!IS_LSB) {
        swap_words(header + 96, sizeof(double), 4);
        swap_words(header + 128, sizeof(int), 2);
    }

    ExtentAndRes extent;
    static_assert(sizeof(extent.westLon) == 8, "wrong sizeof");
    static_assert(sizeof(extent.southLat) == 8, "wrong sizeof");
    static_assert(sizeof(extent.resLon) == 8, "wrong sizeof");
    static_assert(sizeof(extent.resLat) == 8, "wrong sizeof");
    memcpy(&extent.westLon, header + 96, 8);
    memcpy(&extent.southLat, header + 104, 8);
    memcpy(&extent.resLon, header + 112, 8);
    memcpy(&extent.resLat, header + 120, 8);
    if (!(fabs(extent.westLon) <= 4 * M_PI &&
          fabs(extent.southLat) <= M_PI + 1e-5 && extent.resLon > 1e-10 &&
          extent.resLat > 1e-10)) {
        pj_log(ctx, PJ_LOG_ERROR, "Inconsistent georeferencing for %s",
               filename.c_str());
        pj_ctx_set_errno(ctx, PJD_ERR_FAILED_TO_LOAD_GRID);
        return nullptr;
    }
    int width;
    int height;
    memcpy(&width, header + 128, 4);
    memcpy(&height, header + 132, 4);
    if (width <= 0 || height <= 0) {
        pj_ctx_set_errno(ctx, PJD_ERR_FAILED_TO_LOAD_GRID);
        return nullptr;
    }
    extent.eastLon = extent.westLon + (width - 1) * extent.resLon;
    extent.northLat = extent.southLat + (height - 1) * extent.resLon;

    return new CTable2Grid(ctx, fp, width, height, extent);
}

// ---------------------------------------------------------------------------

bool CTable2Grid::valueAt(int x, int y, float &lonShift,
                          float &latShift) const {
    assert(x >= 0 && y >= 0 && x < m_width && y < m_height);

    float two_floats[2];
    pj_ctx_fseek(m_ctx, m_fp, 160 + 2 * sizeof(float) * (y * m_width + x),
                 SEEK_SET);
    if (pj_ctx_fread(m_ctx, &two_floats[0], sizeof(two_floats), 1, m_fp) != 1) {
        pj_ctx_set_errno(m_ctx, PJD_ERR_FAILED_TO_LOAD_GRID);
        return false;
    }
    if (!IS_LSB) {
        swap_words(&two_floats[0], sizeof(float), 2);
    }

    latShift = two_floats[1];
    lonShift = two_floats[0];

    return true;
}

// ---------------------------------------------------------------------------

class NTv2GridSet : public HorizontalShiftGridSet {
    PJ_CONTEXT *m_ctx;
    PAFile m_fp;

    NTv2GridSet(const NTv2GridSet &) = delete;
    NTv2GridSet &operator=(const NTv2GridSet &) = delete;

    NTv2GridSet(PJ_CONTEXT *ctx, PAFile fp) : m_ctx(ctx), m_fp(fp) {}

  public:
    ~NTv2GridSet() override;

    static std::unique_ptr<NTv2GridSet> open(PJ_CONTEXT *ctx, PAFile fp,
                                             const std::string &filename);
};

// ---------------------------------------------------------------------------

class NTv2Grid : public HorizontalShiftGrid {
    friend class NTv2GridSet;

    std::string m_name;
    PJ_CONTEXT *m_ctx; // owned by the parent NTv2GridSet
    PAFile m_fp;       // owned by the parent NTv2GridSet
    unsigned long long m_offset;
    bool m_mustSwap;

    NTv2Grid(const NTv2Grid &) = delete;
    NTv2Grid &operator=(const NTv2Grid &) = delete;

  public:
    NTv2Grid(const std::string &nameIn, PJ_CONTEXT *ctx, PAFile fp,
             unsigned long long offsetIn, bool mustSwapIn, int widthIn,
             int heightIn, const ExtentAndRes &extentIn)
        : HorizontalShiftGrid(widthIn, heightIn, extentIn), m_name(nameIn),
          m_ctx(ctx), m_fp(fp), m_offset(offsetIn), m_mustSwap(mustSwapIn) {}

    bool valueAt(int, int, float &lonShift, float &latShift) const override;
};

// ---------------------------------------------------------------------------

bool NTv2Grid::valueAt(int x, int y, float &lonShift, float &latShift) const {
    assert(x >= 0 && y >= 0 && x < m_width && y < m_height);

    float two_float[2];
    // NTv2 is organized from east to west !
    // there are 4 components: lat shift, lon shift, lat error, lon error
    pj_ctx_fseek(
        m_ctx, m_fp,
        // FIXME when fseek support unsigned long long
        static_cast<long>(m_offset +
                          4 * sizeof(float) * (y * m_width + m_width - 1 - x)),
        SEEK_SET);
    if (pj_ctx_fread(m_ctx, &two_float[0], sizeof(two_float), 1, m_fp) != 1) {
        pj_ctx_set_errno(m_ctx, PJD_ERR_FAILED_TO_LOAD_GRID);
        return false;
    }
    if (m_mustSwap) {
        swap_words(&two_float[0], sizeof(float), 2);
    }
    /* convert seconds to radians */
    latShift = static_cast<float>(two_float[0] * ((M_PI / 180.0) / 3600.0));
    lonShift = static_cast<float>(two_float[1] * ((M_PI / 180.0) / 3600.0));
    return true;
}

// ---------------------------------------------------------------------------

NTv2GridSet::~NTv2GridSet() { pj_ctx_fclose(m_ctx, m_fp); }

// ---------------------------------------------------------------------------

std::unique_ptr<NTv2GridSet> NTv2GridSet::open(PJ_CONTEXT *ctx, PAFile fp,
                                               const std::string &filename) {
    auto set = std::unique_ptr<NTv2GridSet>(new NTv2GridSet(ctx, fp));
    set->m_name = filename;
    set->m_format = "ntv2";

    char header[11 * 16];

    /* -------------------------------------------------------------------- */
    /*      Read the header.                                                */
    /* -------------------------------------------------------------------- */
    if (pj_ctx_fread(ctx, header, sizeof(header), 1, fp) != 1) {
        pj_ctx_set_errno(ctx, PJD_ERR_FAILED_TO_LOAD_GRID);
        return nullptr;
    }

    const bool must_swap = (header[8] == 11) ? !IS_LSB : IS_LSB;
    if (must_swap) {
        // swap_words( header+8, 4, 1 );
        // swap_words( header+8+16, 4, 1 );
        swap_words(header + 8 + 32, 4, 1);
        // swap_words( header+8+7*16, 8, 1 );
        // swap_words( header+8+8*16, 8, 1 );
        // swap_words( header+8+9*16, 8, 1 );
        // swap_words( header+8+10*16, 8, 1 );
    }

    /* -------------------------------------------------------------------- */
    /*      Get the subfile count out ... all we really use for now.        */
    /* -------------------------------------------------------------------- */
    unsigned int num_subfiles;
    memcpy(&num_subfiles, header + 8 + 32, 4);

    std::map<std::string, NTv2Grid *> mapGrids;

    /* ==================================================================== */
    /*      Step through the subfiles, creating a grid for each.            */
    /* ==================================================================== */
    for (unsigned subfile = 0; subfile < num_subfiles; subfile++) {
        // Read header
        if (pj_ctx_fread(ctx, header, sizeof(header), 1, fp) != 1) {
            pj_ctx_set_errno(ctx, PJD_ERR_FAILED_TO_LOAD_GRID);
            return nullptr;
        }

        if (strncmp(header, "SUB_NAME", 8) != 0) {
            pj_ctx_set_errno(ctx, PJD_ERR_FAILED_TO_LOAD_GRID);
            return nullptr;
        }

        // Byte swap interesting fields if needed.
        if (must_swap) {
            swap_words(header + 8 + 16 * 4, sizeof(double), 6);
            swap_words(header + 8 + 16 * 10, sizeof(int), 1);
        }

        std::string gridName;
        gridName.append(header + 8, 8);

        ExtentAndRes extent;
        extent.westLon =
            -to_double(header + 7 * 16 + 8) * DEG_TO_RAD / 3600.0; /* W_LONG */
        extent.southLat =
            to_double(header + 4 * 16 + 8) * DEG_TO_RAD / 3600.0; /* S_LAT */
        extent.eastLon =
            -to_double(header + 6 * 16 + 8) * DEG_TO_RAD / 3600.0; /* E_LONG */
        extent.northLat =
            to_double(header + 5 * 16 + 8) * DEG_TO_RAD / 3600.0; /* N_LAT */
        extent.resLon = to_double(header + 9 * 16 + 8) * DEG_TO_RAD / 3600.0;
        extent.resLat = to_double(header + 8 * 16 + 8) * DEG_TO_RAD / 3600.0;

        if (!(fabs(extent.westLon) <= 4 * M_PI &&
              fabs(extent.eastLon) <= 4 * M_PI &&
              fabs(extent.northLat) <= M_PI + 1e-5 &&
              fabs(extent.southLat) <= M_PI + 1e-5 &&
              extent.westLon < extent.eastLon &&
              extent.southLat < extent.northLat && extent.resLon > 1e-10 &&
              extent.resLat > 1e-10)) {
            pj_log(ctx, PJ_LOG_ERROR, "Inconsistent georeferencing for %s",
                   filename.c_str());
            pj_ctx_set_errno(ctx, PJD_ERR_FAILED_TO_LOAD_GRID);
            return nullptr;
        }
        const int columns = static_cast<int>(
            fabs((extent.eastLon - extent.westLon) / extent.resLon + 0.5) + 1);
        const int rows = static_cast<int>(
            fabs((extent.northLat - extent.southLat) / extent.resLat + 0.5) +
            1);

        pj_log(ctx, PJ_LOG_DEBUG_MINOR,
               "NTv2 %s %dx%d: LL=(%.9g,%.9g) UR=(%.9g,%.9g)", gridName.c_str(),
               columns, rows, extent.westLon * RAD_TO_DEG,
               extent.southLat * RAD_TO_DEG, extent.eastLon * RAD_TO_DEG,
               extent.northLat * RAD_TO_DEG);

        unsigned int gs_count;
        memcpy(&gs_count, header + 8 + 16 * 10, 4);
        if (gs_count / columns != static_cast<unsigned>(rows)) {
            pj_log(ctx, PJ_LOG_ERROR,
                   "GS_COUNT(%u) does not match expected cells (%dx%d)",
                   gs_count, columns, rows);
            pj_ctx_set_errno(ctx, PJD_ERR_FAILED_TO_LOAD_GRID);
            return nullptr;
        }

        auto offset = pj_ctx_ftell(ctx, fp);
        auto grid = std::unique_ptr<NTv2Grid>(new NTv2Grid(
            gridName, ctx, fp, offset, must_swap, columns, rows, extent));
        std::string parentName;
        parentName.assign(header + 24, 8);
        auto iter = mapGrids.find(parentName);
        auto gridPtr = grid.get();
        if (iter == mapGrids.end()) {
            set->m_grids.emplace_back(std::move(grid));
        } else {
            iter->second->m_children.emplace_back(std::move(grid));
        }
        mapGrids[gridName] = gridPtr;

        // Skip grid data. 4 components of size float
        pj_ctx_fseek(ctx, fp, gs_count * 4 * 4, SEEK_CUR);
    }
    return set;
}

// ---------------------------------------------------------------------------

std::unique_ptr<HorizontalShiftGridSet>
HorizontalShiftGridSet::open(PJ_CONTEXT *ctx, const std::string &filename) {
    if (filename == "null") {
        auto set = std::unique_ptr<HorizontalShiftGridSet>(
            new HorizontalShiftGridSet());
        set->m_name = filename;
        set->m_format = "null";
        set->m_grids.push_back(std::unique_ptr<NullHorizontalShiftGrid>(
            new NullHorizontalShiftGrid()));
        return set;
    }

    PAFile fp;
    if (!(fp = pj_open_lib(ctx, filename.c_str(), "rb"))) {
        ctx->last_errno = 0; /* don't treat as a persistent error */
        return nullptr;
    }

    char header[160];
    /* -------------------------------------------------------------------- */
    /*      Load a header, to determine the file type.                      */
    /* -------------------------------------------------------------------- */
    size_t header_size;
    if ((header_size = pj_ctx_fread(ctx, header, 1, sizeof(header), fp)) !=
        sizeof(header)) {
        /* some files may be smaller that sizeof(header), eg 160, so */
        ctx->last_errno = 0; /* don't treat as a persistent error */
        pj_log(ctx, PJ_LOG_DEBUG_MAJOR,
               "pj_gridinfo_init: short header read of %d bytes",
               (int)header_size);
    }

    pj_ctx_fseek(ctx, fp, SEEK_SET, 0);

    /* -------------------------------------------------------------------- */
    /*      Determine file type.                                            */
    /* -------------------------------------------------------------------- */
    if (header_size >= 144 + 16 && strncmp(header + 0, "HEADER", 6) == 0 &&
        strncmp(header + 96, "W GRID", 6) == 0 &&
        strncmp(header + 144, "TO      NAD83   ", 16) == 0) {
        auto grid = NTv1Grid::open(ctx, fp, filename);
        if (!grid) {
            pj_ctx_fclose(ctx, fp);
            return nullptr;
        }
        auto set = std::unique_ptr<HorizontalShiftGridSet>(
            new HorizontalShiftGridSet());
        set->m_name = filename;
        set->m_format = "ntv1";
        set->m_grids.push_back(std::unique_ptr<HorizontalShiftGrid>(grid));
        return set;
    } else if (header_size >= 9 && strncmp(header + 0, "CTABLE V2", 9) == 0) {
        auto grid = CTable2Grid::open(ctx, fp, filename);
        if (!grid) {
            pj_ctx_fclose(ctx, fp);
            return nullptr;
        }
        auto set = std::unique_ptr<HorizontalShiftGridSet>(
            new HorizontalShiftGridSet());
        set->m_name = filename;
        set->m_format = "ctable2";
        set->m_grids.push_back(std::unique_ptr<HorizontalShiftGrid>(grid));
        return set;
    } else if (header_size >= 48 + 7 &&
               strncmp(header + 0, "NUM_OREC", 8) == 0 &&
               strncmp(header + 48, "GS_TYPE", 7) == 0) {
        return NTv2GridSet::open(ctx, fp, filename);
    }

    pj_log(ctx, PJ_LOG_DEBUG_MAJOR, "Unrecognized horizontal grid format");
    pj_ctx_fclose(ctx, fp);
    return nullptr;
}

// ---------------------------------------------------------------------------

const HorizontalShiftGrid *HorizontalShiftGrid::gridAt(double lon,
                                                       double lat) const {
    for (const auto &child : m_children) {
        const auto &extentChild = child->extentAndRes();
        const double epsilon =
            (extentChild.resLon + extentChild.resLat) / 10000.0;
        if ((extentChild.fullWorldLongitude() ||
             (lon + epsilon >= extentChild.westLon &&
              lon - epsilon <= extentChild.eastLon)) &&
            lat + epsilon >= extentChild.southLat &&
            lat - epsilon <= extentChild.northLat) {
            return child->gridAt(lon, lat);
        }
    }
    return this;
}
// ---------------------------------------------------------------------------

const HorizontalShiftGrid *HorizontalShiftGridSet::gridAt(double lon,
                                                          double lat) const {
    for (const auto &grid : m_grids) {
        if (dynamic_cast<NullHorizontalShiftGrid *>(grid.get())) {
            return grid.get();
        }
        const auto &extent = grid->extentAndRes();
        const double epsilon = (extent.resLon + extent.resLat) / 10000.0;
        if ((extent.fullWorldLongitude() ||
             (lon + epsilon >= extent.westLon &&
              lon - epsilon <= extent.eastLon)) &&
            lat + epsilon >= extent.southLat &&
            lat - epsilon <= extent.northLat) {
            return grid->gridAt(lon, lat);
        }
    }
    return nullptr;
}

NS_PROJ_END
