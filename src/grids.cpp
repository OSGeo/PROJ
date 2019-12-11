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
#define LRU11_DO_NOT_DEFINE_OUT_OF_CLASS_METHODS

#include "grids.hpp"
#include "proj/internal/internal.hpp"
#include "proj/internal/lru_cache.hpp"
#include "proj_internal.h"

#include "tiffio.h"

#include <algorithm>
#include <cmath>

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

// ---------------------------------------------------------------------------

bool ExtentAndRes::fullWorldLongitude() const {
    return eastLon - westLon + resLon >= 2 * M_PI - 1e-10;
}

// ---------------------------------------------------------------------------

bool ExtentAndRes::contains(const ExtentAndRes &other) const {
    return other.westLon >= westLon && other.eastLon <= eastLon &&
           other.southLat >= southLat && other.northLat <= northLat;
}

// ---------------------------------------------------------------------------

bool ExtentAndRes::intersects(const ExtentAndRes &other) const {
    return other.westLon < eastLon && westLon <= other.westLon &&
           other.southLat < northLat && southLat <= other.northLat;
}

// ---------------------------------------------------------------------------

Grid::Grid(int widthIn, int heightIn, const ExtentAndRes &extentIn)
    : m_width(widthIn), m_height(heightIn), m_extent(extentIn) {}

// ---------------------------------------------------------------------------

Grid::~Grid() = default;

// ---------------------------------------------------------------------------

VerticalShiftGrid::VerticalShiftGrid(int widthIn, int heightIn,
                                     const ExtentAndRes &extentIn)
    : Grid(widthIn, heightIn, extentIn) {}

// ---------------------------------------------------------------------------

bool VerticalShiftGrid::isNodata(float /*val*/, double /* multiplier */) const {
    return false;
}

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

    bool isNullGrid() const override { return true; }
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

static bool IsTIFF(size_t header_size, const unsigned char *header) {
    // Test combinations of signature for ClassicTIFF/BigTIFF little/big endian
    return header_size >= 4 && (((header[0] == 'I' && header[1] == 'I') ||
                                 (header[0] == 'M' && header[1] == 'M')) &&
                                ((header[2] == 0x2A && header[3] == 0) ||
                                 (header[3] == 0x2A && header[2] == 0) ||
                                 (header[2] == 0x2B && header[3] == 0) ||
                                 (header[3] == 0x2B && header[2] == 0)));
}

// ---------------------------------------------------------------------------

enum class TIFFDataType { Int16, UInt16, Int32, UInt32, Float32, Float64 };

// ---------------------------------------------------------------------------

constexpr uint16 TIFFTAG_GEOPIXELSCALE = 33550;
constexpr uint16 TIFFTAG_GEOTIEPOINTS = 33922;
constexpr uint16 TIFFTAG_GEOTRANSMATRIX = 34264;
constexpr uint16 TIFFTAG_GEOKEYDIRECTORY = 34735;
constexpr uint16 TIFFTAG_GEODOUBLEPARAMS = 34736;
constexpr uint16 TIFFTAG_GEOASCIIPARAMS = 34737;
constexpr uint16 TIFFTAG_GDAL_METADATA = 42112;
constexpr uint16 TIFFTAG_GDAL_NODATA = 42113;

// ---------------------------------------------------------------------------

class BlockCache {
  public:
    void insert(uint32 ifdIdx, uint32 blockNumber,
                const std::vector<unsigned char> &data);
    std::shared_ptr<std::vector<unsigned char>> get(uint32 ifdIdx,
                                                    uint32 blockNumber);

  private:
    struct Key {
        uint32 ifdIdx;
        uint32 blockNumber;

        Key(uint32 ifdIdxIn, uint32 blockNumberIn)
            : ifdIdx(ifdIdxIn), blockNumber(blockNumberIn) {}
        bool operator==(const Key &other) const {
            return ifdIdx == other.ifdIdx && blockNumber == other.blockNumber;
        }
    };

    struct KeyHasher {
        std::size_t operator()(const Key &k) const {
            return k.ifdIdx ^ (k.blockNumber << 16) ^ (k.blockNumber >> 16);
        }
    };

    static constexpr int NUM_BLOCKS_AT_CROSSING_TILES = 4;
    static constexpr int MAX_SAMPLE_COUNT = 3;
    lru11::Cache<
        Key, std::shared_ptr<std::vector<unsigned char>>, lru11::NullLock,
        std::unordered_map<
            Key,
            typename std::list<lru11::KeyValuePair<
                Key, std::shared_ptr<std::vector<unsigned char>>>>::iterator,
            KeyHasher>>
        cache_{NUM_BLOCKS_AT_CROSSING_TILES * MAX_SAMPLE_COUNT};
};

// ---------------------------------------------------------------------------

void BlockCache::insert(uint32 ifdIdx, uint32 blockNumber,
                        const std::vector<unsigned char> &data) {
    cache_.insert(Key(ifdIdx, blockNumber),
                  std::make_shared<std::vector<unsigned char>>(data));
}

// ---------------------------------------------------------------------------

std::shared_ptr<std::vector<unsigned char>>
BlockCache::get(uint32 ifdIdx, uint32 blockNumber) {
    std::shared_ptr<std::vector<unsigned char>> ret;
    cache_.tryGet(Key(ifdIdx, blockNumber), ret);
    return ret;
}

// ---------------------------------------------------------------------------

class GTiffGrid : public Grid {
    PJ_CONTEXT *m_ctx;   // owned by the belonging GTiffDataset
    TIFF *m_hTIFF;       // owned by the belonging GTiffDataset
    BlockCache &m_cache; // owned by the belonging GTiffDataset
    uint32 m_ifdIdx;
    TIFFDataType m_dt;
    uint16 m_samplesPerPixel;
    uint16 m_planarConfig;
    bool m_bottomUp;
    toff_t m_dirOffset;
    bool m_tiled;
    uint32 m_blockWidth = 0;
    uint32 m_blockHeight = 0;
    mutable std::vector<unsigned char> m_buffer{};
    unsigned m_blocksPerRow = 0;
    unsigned m_blocksPerCol = 0;
    std::map<int, double> m_mapOffset{};
    std::map<int, double> m_mapScale{};
    std::map<std::pair<int, std::string>, std::string> m_metadata{};
    bool m_hasNodata = false;
    float m_noData = 0.0f;
    uint32 m_subfileType = 0;

    GTiffGrid(const GTiffGrid &) = delete;
    GTiffGrid &operator=(const GTiffGrid &) = delete;

    void getScaleOffset(double &scale, double &offset, uint16 sample) const;

    template <class T>
    float readValue(const std::vector<unsigned char> &buffer,
                    uint32 offsetInBlock, uint16 sample) const;

  public:
    GTiffGrid(PJ_CONTEXT *ctx, TIFF *hTIFF, BlockCache &cache, uint32 ifdIdx,
              int widthIn, int heightIn, const ExtentAndRes &extentIn,
              TIFFDataType dtIn, uint16 samplesPerPixelIn, uint16 planarConfig,
              bool bottomUpIn);

    ~GTiffGrid() override;

    uint16 samplesPerPixel() const { return m_samplesPerPixel; }

    bool valueAt(uint16 sample, int x, int y, float &out) const;

    bool isNodata(float val) const;

    std::string metadataItem(const std::string &key, int sample = -1) const;

    uint32 subfileType() const { return m_subfileType; }
};

// ---------------------------------------------------------------------------

GTiffGrid::GTiffGrid(PJ_CONTEXT *ctx, TIFF *hTIFF, BlockCache &cache,
                     uint32 ifdIdx, int widthIn, int heightIn,
                     const ExtentAndRes &extentIn, TIFFDataType dtIn,
                     uint16 samplesPerPixelIn, uint16 planarConfig,
                     bool bottomUpIn)
    : Grid(widthIn, heightIn, extentIn), m_ctx(ctx), m_hTIFF(hTIFF),
      m_cache(cache), m_ifdIdx(ifdIdx), m_dt(dtIn),
      m_samplesPerPixel(samplesPerPixelIn), m_planarConfig(planarConfig),
      m_bottomUp(bottomUpIn), m_dirOffset(TIFFCurrentDirOffset(hTIFF)),
      m_tiled(TIFFIsTiled(hTIFF) != 0) {

    if (m_tiled) {
        TIFFGetField(m_hTIFF, TIFFTAG_TILEWIDTH, &m_blockWidth);
        TIFFGetField(m_hTIFF, TIFFTAG_TILELENGTH, &m_blockHeight);
    } else {
        m_blockWidth = widthIn;
        TIFFGetField(m_hTIFF, TIFFTAG_ROWSPERSTRIP, &m_blockHeight);
        if (m_blockHeight > static_cast<unsigned>(m_height))
            m_blockHeight = m_height;
    }

    TIFFGetField(m_hTIFF, TIFFTAG_SUBFILETYPE, &m_subfileType);

    m_blocksPerRow = (m_width + m_blockWidth - 1) / m_blockWidth;
    m_blocksPerCol = (m_height + m_blockHeight - 1) / m_blockHeight;

    const char *text = nullptr;
    // Poor-man XML parsing of TIFFTAG_GDAL_METADATA tag. Hopefully good
    // enough for our purposes.
    if (TIFFGetField(m_hTIFF, TIFFTAG_GDAL_METADATA, &text)) {
        const char *ptr = text;
        while (true) {
            ptr = strstr(ptr, "<Item ");
            if (ptr == nullptr)
                break;
            const char *endTag = strchr(ptr, '>');
            if (endTag == nullptr)
                break;
            const char *endValue = strchr(endTag, '<');
            if (endValue == nullptr)
                break;

            std::string tag;
            tag.append(ptr, endTag - ptr);

            std::string value;
            value.append(endTag + 1, endValue - (endTag + 1));

            std::string name;
            auto namePos = tag.find("name=\"");
            if (namePos == std::string::npos)
                break;
            {
                namePos += strlen("name=\"");
                const auto endQuote = tag.find('"', namePos);
                if (endQuote == std::string::npos)
                    break;
                name = tag.substr(namePos, endQuote - namePos);
            }

            const auto samplePos = tag.find("sample=\"");
            int sample = -1;
            if (samplePos != std::string::npos) {
                sample = atoi(tag.c_str() + samplePos + strlen("sample=\""));
            }

            m_metadata[std::pair<int, std::string>(sample, name)] = value;

            auto rolePos = tag.find("role=\"");
            if (rolePos != std::string::npos) {
                rolePos += strlen("role=\"");
                const auto endQuote = tag.find('"', rolePos);
                if (endQuote == std::string::npos)
                    break;
                const auto role = tag.substr(rolePos, endQuote - rolePos);
                if (role == "offset") {
                    if (sample >= 0) {
                        try {
                            m_mapOffset[sample] = c_locale_stod(value);
                        } catch (const std::exception &) {
                        }
                    }
                } else if (role == "scale") {
                    if (sample >= 0) {
                        try {
                            m_mapScale[sample] = c_locale_stod(value);
                        } catch (const std::exception &) {
                        }
                    }
                }
            }

            ptr = endValue + 1;
        }
    }

    if (TIFFGetField(m_hTIFF, TIFFTAG_GDAL_NODATA, &text)) {
        try {
            m_noData = static_cast<float>(c_locale_stod(text));
            m_hasNodata = true;
        } catch (const std::exception &) {
        }
    }
}

// ---------------------------------------------------------------------------

GTiffGrid::~GTiffGrid() = default;

// ---------------------------------------------------------------------------

void GTiffGrid::getScaleOffset(double &scale, double &offset,
                               uint16 sample) const {
    {
        auto iter = m_mapScale.find(sample);
        if (iter != m_mapScale.end())
            scale = iter->second;
    }

    {
        auto iter = m_mapOffset.find(sample);
        if (iter != m_mapOffset.end())
            offset = iter->second;
    }
}

// ---------------------------------------------------------------------------

template <class T>
float GTiffGrid::readValue(const std::vector<unsigned char> &buffer,
                           uint32 offsetInBlock, uint16 sample) const {
    const auto ptr = reinterpret_cast<const T *>(buffer.data());
    assert(offsetInBlock < buffer.size() / sizeof(T));
    const auto val = ptr[offsetInBlock];
    if (!m_hasNodata || static_cast<float>(val) != m_noData) {
        double scale = 1;
        double offset = 0;
        getScaleOffset(scale, offset, sample);
        return static_cast<float>(val * scale + offset);
    } else {
        return static_cast<float>(val);
    }
}

// ---------------------------------------------------------------------------

bool GTiffGrid::valueAt(uint16 sample, int x, int yFromBottom,
                        float &out) const {
    assert(x >= 0 && yFromBottom >= 0 && x < m_width && yFromBottom < m_height);
    assert(sample < m_samplesPerPixel);

    const int blockX = x / m_blockWidth;

    // All non-TIFF grids have the first rows in the file being the one
    // corresponding to the southern-most row. In GeoTIFF, the convention is
    // *generally* different (when m_bottomUp == false), TIFF being an
    // image-oriented image. If m_bottomUp == true, then we had GeoTIFF hints
    // that the first row of the image is the southern-most.
    const int yTIFF = m_bottomUp ? yFromBottom : m_height - 1 - yFromBottom;
    const int blockY = yTIFF / m_blockHeight;

    uint32 blockId = blockY * m_blocksPerRow + blockX;
    if (m_planarConfig == PLANARCONFIG_SEPARATE) {
        blockId += sample * m_blocksPerCol * m_blocksPerRow;
    }

    auto cachedBuffer = m_cache.get(m_ifdIdx, blockId);
    std::vector<unsigned char> *pBuffer = &m_buffer;
    if (cachedBuffer != nullptr) {
        // Safe as we don't access the cache before pBuffer is used
        pBuffer = cachedBuffer.get();
    } else {
        if (TIFFCurrentDirOffset(m_hTIFF) != m_dirOffset &&
            !TIFFSetSubDirectory(m_hTIFF, m_dirOffset)) {
            return false;
        }
        if (m_buffer.empty()) {
            const auto blockSize = static_cast<size_t>(
                m_tiled ? TIFFTileSize64(m_hTIFF) : TIFFStripSize64(m_hTIFF));
            try {
                m_buffer.resize(blockSize);
            } catch (const std::exception &e) {
                pj_log(m_ctx, PJ_LOG_ERROR, "Exception %s", e.what());
                return false;
            }
        }

        if (m_tiled) {
            if (TIFFReadEncodedTile(m_hTIFF, blockId, m_buffer.data(),
                                    m_buffer.size()) == -1) {
                return false;
            }
        } else {
            if (TIFFReadEncodedStrip(m_hTIFF, blockId, m_buffer.data(),
                                     m_buffer.size()) == -1) {
                return false;
            }
        }

        try {
            m_cache.insert(m_ifdIdx, blockId, m_buffer);
        } catch (const std::exception &e) {
            // Should normally not happen
            pj_log(m_ctx, PJ_LOG_ERROR, "Exception %s", e.what());
        }
    }

    uint32 offsetInBlock =
        (x % m_blockWidth) + (yTIFF % m_blockHeight) * m_blockWidth;
    if (m_planarConfig == PLANARCONFIG_CONTIG)
        offsetInBlock = offsetInBlock * m_samplesPerPixel + sample;

    switch (m_dt) {
    case TIFFDataType::Int16:
        out = readValue<short>(*pBuffer, offsetInBlock, sample);
        break;

    case TIFFDataType::UInt16:
        out = readValue<unsigned short>(*pBuffer, offsetInBlock, sample);
        break;

    case TIFFDataType::Int32:
        out = readValue<int>(*pBuffer, offsetInBlock, sample);
        break;

    case TIFFDataType::UInt32:
        out = readValue<unsigned int>(*pBuffer, offsetInBlock, sample);
        break;

    case TIFFDataType::Float32:
        out = readValue<float>(*pBuffer, offsetInBlock, sample);
        break;

    case TIFFDataType::Float64:
        out = readValue<double>(*pBuffer, offsetInBlock, sample);
        break;
    }

    return true;
}

// ---------------------------------------------------------------------------

bool GTiffGrid::isNodata(float val) const {
    return (m_hasNodata && val == m_noData) || std::isnan(val);
}

// ---------------------------------------------------------------------------

std::string GTiffGrid::metadataItem(const std::string &key, int sample) const {
    auto iter = m_metadata.find(std::pair<int, std::string>(sample, key));
    if (iter == m_metadata.end()) {
        return std::string();
    }
    return iter->second;
}

// ---------------------------------------------------------------------------

class GTiffDataset {
    PJ_CONTEXT *m_ctx;
    PAFile m_fp;
    TIFF *m_hTIFF = nullptr;
    bool m_hasNextGrid = false;
    uint32 m_ifdIdx = 0;
    toff_t m_nextDirOffset = 0;
    std::string m_filename{};
    BlockCache m_cache{};

    GTiffDataset(const GTiffDataset &) = delete;
    GTiffDataset &operator=(const GTiffDataset &) = delete;

    // libtiff I/O routines
    static tsize_t tiffReadProc(thandle_t fd, tdata_t buf, tsize_t size) {
        GTiffDataset *self = static_cast<GTiffDataset *>(fd);
        return pj_ctx_fread(self->m_ctx, buf, 1, size, self->m_fp);
    }

    static tsize_t tiffWriteProc(thandle_t, tdata_t, tsize_t) {
        assert(false);
        return 0;
    }

    static toff_t tiffSeekProc(thandle_t fd, toff_t off, int whence) {
        GTiffDataset *self = static_cast<GTiffDataset *>(fd);
        // FIXME Remove cast to long when pj_ctx_fseek supports unsigned long
        // long
        if (pj_ctx_fseek(self->m_ctx, self->m_fp, static_cast<long>(off),
                         whence) == 0)
            return static_cast<toff_t>(pj_ctx_ftell(self->m_ctx, self->m_fp));
        else
            return static_cast<toff_t>(-1);
    }

    static int tiffCloseProc(thandle_t) {
        // done in destructor
        return 0;
    }

    static toff_t tiffSizeProc(thandle_t fd) {
        GTiffDataset *self = static_cast<GTiffDataset *>(fd);
        const auto old_off = pj_ctx_ftell(self->m_ctx, self->m_fp);
        pj_ctx_fseek(self->m_ctx, self->m_fp, 0, SEEK_END);
        const auto file_size =
            static_cast<toff_t>(pj_ctx_ftell(self->m_ctx, self->m_fp));
        pj_ctx_fseek(self->m_ctx, self->m_fp, old_off, SEEK_SET);
        return file_size;
    }

    static int tiffMapProc(thandle_t, tdata_t *, toff_t *) { return (0); }

    static void tiffUnmapProc(thandle_t, tdata_t, toff_t) {}

  public:
    GTiffDataset(PJ_CONTEXT *ctx, PAFile fp) : m_ctx(ctx), m_fp(fp) {}
    virtual ~GTiffDataset();

    bool openTIFF(const std::string &filename);

    std::unique_ptr<GTiffGrid> nextGrid();
};

// ---------------------------------------------------------------------------

GTiffDataset::~GTiffDataset() {
    if (m_hTIFF)
        TIFFClose(m_hTIFF);
    pj_ctx_fclose(m_ctx, m_fp);
}

// ---------------------------------------------------------------------------
class OneTimeTIFFTagInit {

    static TIFFExtendProc ParentExtender;

    // Function called by libtiff when initializing a TIFF directory
    static void GTiffTagExtender(TIFF *tif) {
        static const TIFFFieldInfo xtiffFieldInfo[] = {
            // GeoTIFF tags
            {TIFFTAG_GEOPIXELSCALE, -1, -1, TIFF_DOUBLE, FIELD_CUSTOM, TRUE,
             TRUE, const_cast<char *>("GeoPixelScale")},
            {TIFFTAG_GEOTIEPOINTS, -1, -1, TIFF_DOUBLE, FIELD_CUSTOM, TRUE,
             TRUE, const_cast<char *>("GeoTiePoints")},
            {TIFFTAG_GEOTRANSMATRIX, -1, -1, TIFF_DOUBLE, FIELD_CUSTOM, TRUE,
             TRUE, const_cast<char *>("GeoTransformationMatrix")},

            {TIFFTAG_GEOKEYDIRECTORY, -1, -1, TIFF_SHORT, FIELD_CUSTOM, TRUE,
             TRUE, const_cast<char *>("GeoKeyDirectory")},
            {TIFFTAG_GEODOUBLEPARAMS, -1, -1, TIFF_DOUBLE, FIELD_CUSTOM, TRUE,
             TRUE, const_cast<char *>("GeoDoubleParams")},
            {TIFFTAG_GEOASCIIPARAMS, -1, -1, TIFF_ASCII, FIELD_CUSTOM, TRUE,
             FALSE, const_cast<char *>("GeoASCIIParams")},

            // GDAL tags
            {TIFFTAG_GDAL_METADATA, -1, -1, TIFF_ASCII, FIELD_CUSTOM, TRUE,
             FALSE, const_cast<char *>("GDALMetadata")},
            {TIFFTAG_GDAL_NODATA, -1, -1, TIFF_ASCII, FIELD_CUSTOM, TRUE, FALSE,
             const_cast<char *>("GDALNoDataValue")},

        };

        if (ParentExtender)
            (*ParentExtender)(tif);

        TIFFMergeFieldInfo(tif, xtiffFieldInfo,
                           sizeof(xtiffFieldInfo) / sizeof(xtiffFieldInfo[0]));
    }

  public:
    OneTimeTIFFTagInit() {
        assert(ParentExtender == nullptr);
        // Install our TIFF tag extender
        ParentExtender = TIFFSetTagExtender(GTiffTagExtender);
    }
};

TIFFExtendProc OneTimeTIFFTagInit::ParentExtender = nullptr;

// ---------------------------------------------------------------------------

bool GTiffDataset::openTIFF(const std::string &filename) {
    static OneTimeTIFFTagInit oneTimeTIFFTagInit;
    m_hTIFF =
        TIFFClientOpen(filename.c_str(), "r", static_cast<thandle_t>(this),
                       GTiffDataset::tiffReadProc, GTiffDataset::tiffWriteProc,
                       GTiffDataset::tiffSeekProc, GTiffDataset::tiffCloseProc,
                       GTiffDataset::tiffSizeProc, GTiffDataset::tiffMapProc,
                       GTiffDataset::tiffUnmapProc);

    m_filename = filename;
    m_hasNextGrid = true;
    return m_hTIFF != nullptr;
}
// ---------------------------------------------------------------------------

std::unique_ptr<GTiffGrid> GTiffDataset::nextGrid() {
    if (!m_hasNextGrid)
        return nullptr;
    if (m_nextDirOffset) {
        TIFFSetSubDirectory(m_hTIFF, m_nextDirOffset);
    }

    uint32 width = 0;
    uint32 height = 0;
    TIFFGetField(m_hTIFF, TIFFTAG_IMAGEWIDTH, &width);
    TIFFGetField(m_hTIFF, TIFFTAG_IMAGELENGTH, &height);
    if (width == 0 || height == 0 || width > INT_MAX || height > INT_MAX) {
        pj_log(m_ctx, PJ_LOG_ERROR, "Invalid image size");
        return nullptr;
    }

    uint16 samplesPerPixel = 0;
    if (!TIFFGetField(m_hTIFF, TIFFTAG_SAMPLESPERPIXEL, &samplesPerPixel)) {
        pj_log(m_ctx, PJ_LOG_ERROR, "Missing SamplesPerPixel tag");
        return nullptr;
    }
    if (samplesPerPixel == 0) {
        pj_log(m_ctx, PJ_LOG_ERROR, "Invalid SamplesPerPixel value");
        return nullptr;
    }

    uint16 bitsPerSample = 0;
    if (!TIFFGetField(m_hTIFF, TIFFTAG_BITSPERSAMPLE, &bitsPerSample)) {
        pj_log(m_ctx, PJ_LOG_ERROR, "Missing BitsPerSample tag");
        return nullptr;
    }

    uint16 planarConfig = 0;
    if (!TIFFGetField(m_hTIFF, TIFFTAG_PLANARCONFIG, &planarConfig)) {
        pj_log(m_ctx, PJ_LOG_ERROR, "Missing PlanarConfig tag");
        return nullptr;
    }

    uint16 sampleFormat = 0;
    if (!TIFFGetField(m_hTIFF, TIFFTAG_SAMPLEFORMAT, &sampleFormat)) {
        pj_log(m_ctx, PJ_LOG_ERROR, "Missing SampleFormat tag");
        return nullptr;
    }

    TIFFDataType dt;
    if (sampleFormat == SAMPLEFORMAT_INT && bitsPerSample == 16)
        dt = TIFFDataType::Int16;
    else if (sampleFormat == SAMPLEFORMAT_UINT && bitsPerSample == 16)
        dt = TIFFDataType::UInt16;
    else if (sampleFormat == SAMPLEFORMAT_INT && bitsPerSample == 32)
        dt = TIFFDataType::Int32;
    else if (sampleFormat == SAMPLEFORMAT_UINT && bitsPerSample == 32)
        dt = TIFFDataType::UInt32;
    else if (sampleFormat == SAMPLEFORMAT_IEEEFP && bitsPerSample == 32)
        dt = TIFFDataType::Float32;
    else if (sampleFormat == SAMPLEFORMAT_IEEEFP && bitsPerSample == 64)
        dt = TIFFDataType::Float64;
    else {
        pj_log(
            m_ctx, PJ_LOG_ERROR,
            "Unsupported combination of SampleFormat and BitsPerSample values");
        return nullptr;
    }

    uint16 photometric = PHOTOMETRIC_MINISBLACK;
    if (!TIFFGetField(m_hTIFF, TIFFTAG_PHOTOMETRIC, &photometric))
        photometric = PHOTOMETRIC_MINISBLACK;
    if (photometric != PHOTOMETRIC_MINISBLACK) {
        pj_log(m_ctx, PJ_LOG_ERROR, "Unsupported Photometric value");
        return nullptr;
    }

    uint16 compression = COMPRESSION_NONE;
    if (!TIFFGetField(m_hTIFF, TIFFTAG_COMPRESSION, &compression))
        compression = COMPRESSION_NONE;

    if (compression != COMPRESSION_NONE &&
        !TIFFIsCODECConfigured(compression)) {
        pj_log(m_ctx, PJ_LOG_ERROR,
               "Cannot open TIFF file due to missing codec.");
        return nullptr;
    }
    // We really don't want to try dealing with old-JPEG images
    if (compression == COMPRESSION_OJPEG) {
        pj_log(m_ctx, PJ_LOG_ERROR, "Unsupported compression method.");
        return nullptr;
    }

    const auto blockSize = TIFFIsTiled(m_hTIFF) ? TIFFTileSize64(m_hTIFF)
                                                : TIFFStripSize64(m_hTIFF);
    if (blockSize == 0 || blockSize > 64 * 1024 * 2014) {
        pj_log(m_ctx, PJ_LOG_ERROR, "Unsupported block size.");
        return nullptr;
    }

    unsigned short count = 0;
    unsigned short *geokeys = nullptr;
    bool pixelIsArea = false;
    if (!TIFFGetField(m_hTIFF, TIFFTAG_GEOKEYDIRECTORY, &count, &geokeys)) {
        pj_log(m_ctx, PJ_LOG_DEBUG_MINOR, "No GeoKeys tag");
    } else {
        if (count < 4 || (count % 4) != 0) {
            pj_log(m_ctx, PJ_LOG_ERROR,
                   "Wrong number of values in GeoKeys tag");
            return nullptr;
        }

        if (geokeys[0] != 1) {
            pj_log(m_ctx, PJ_LOG_ERROR, "Unsupported GeoTIFF major version");
            return nullptr;
        }
        // We only know that we support GeoTIFF 1.0 and 1.1 at that time
        if (geokeys[1] != 1 || geokeys[2] > 1) {
            pj_log(m_ctx, PJ_LOG_DEBUG_MINOR,
                   "GeoTIFF %d.%d possibly not handled", geokeys[1],
                   geokeys[2]);
        }

        for (unsigned int i = 4; i + 3 < count; i += 4) {
            constexpr unsigned short GTModelTypeGeoKey = 1024;
            constexpr unsigned short ModelTypeGeographic = 2;

            constexpr unsigned short GTRasterTypeGeoKey = 1025;
            constexpr unsigned short RasterPixelIsArea = 1;
            // constexpr unsigned short RasterPixelIsPoint = 2;

            if (geokeys[i] == GTModelTypeGeoKey) {
                if (geokeys[i + 3] != ModelTypeGeographic) {
                    pj_log(m_ctx, PJ_LOG_ERROR, "Only GTModelTypeGeoKey = "
                                                "ModelTypeGeographic is "
                                                "supported");
                    return nullptr;
                }
            } else if (geokeys[i] == GTRasterTypeGeoKey) {
                if (geokeys[i + 3] == RasterPixelIsArea) {
                    pixelIsArea = true;
                }
            }
        }
    }

    double hRes = 0;
    double vRes = 0;
    double westLon = 0;
    double northLat = 0;

    double *matrix = nullptr;
    if (TIFFGetField(m_hTIFF, TIFFTAG_GEOTRANSMATRIX, &count, &matrix) &&
        count == 16) {
        // If using GDAL to produce a bottom-up georeferencing, it will produce
        // a GeoTransformationMatrix, since negative values in GeoPixelScale
        // have historically been implementation bugs.
        if (matrix[1] != 0 || matrix[4] != 0) {
            pj_log(m_ctx, PJ_LOG_ERROR, "Rotational terms not supported in "
                                        "GeoTransformationMatrix tag");
            return nullptr;
        }

        westLon = matrix[3];
        hRes = matrix[0];
        northLat = matrix[7];
        vRes = -matrix[5]; // negation to simulate GeoPixelScale convention
    } else {
        double *geopixelscale = nullptr;
        if (TIFFGetField(m_hTIFF, TIFFTAG_GEOPIXELSCALE, &count,
                         &geopixelscale) != 1) {
            pj_log(m_ctx, PJ_LOG_ERROR, "No GeoPixelScale tag");
            return nullptr;
        }
        if (count != 3) {
            pj_log(m_ctx, PJ_LOG_ERROR,
                   "Wrong number of values in GeoPixelScale tag");
            return nullptr;
        }
        hRes = geopixelscale[0];
        vRes = geopixelscale[1];

        double *geotiepoints = nullptr;
        if (TIFFGetField(m_hTIFF, TIFFTAG_GEOTIEPOINTS, &count,
                         &geotiepoints) != 1) {
            pj_log(m_ctx, PJ_LOG_ERROR, "No GeoTiePoints tag");
            return nullptr;
        }
        if (count != 6) {
            pj_log(m_ctx, PJ_LOG_ERROR,
                   "Wrong number of values in GeoTiePoints tag");
            return nullptr;
        }

        westLon = geotiepoints[3] - geotiepoints[0] * hRes;
        northLat = geotiepoints[4] + geotiepoints[1] * vRes;
    }

    if (pixelIsArea) {
        westLon += 0.5 * hRes;
        northLat -= 0.5 * vRes;
    }

    ExtentAndRes extent;
    extent.westLon = westLon * DEG_TO_RAD;
    extent.northLat = northLat * DEG_TO_RAD;
    extent.resLon = hRes * DEG_TO_RAD;
    extent.resLat = fabs(vRes) * DEG_TO_RAD;
    extent.eastLon = (westLon + hRes * (width - 1)) * DEG_TO_RAD;
    extent.southLat = (northLat - vRes * (height - 1)) * DEG_TO_RAD;

    if (vRes < 0) {
        std::swap(extent.northLat, extent.southLat);
    }

    if (!(fabs(extent.westLon) <= 4 * M_PI &&
          fabs(extent.eastLon) <= 4 * M_PI &&
          fabs(extent.northLat) <= M_PI + 1e-5 &&
          fabs(extent.southLat) <= M_PI + 1e-5 &&
          extent.westLon < extent.eastLon &&
          extent.southLat < extent.northLat && extent.resLon > 1e-10 &&
          extent.resLat > 1e-10)) {
        pj_log(m_ctx, PJ_LOG_ERROR, "Inconsistent georeferencing for %s",
               m_filename.c_str());
        return nullptr;
    }

    auto ret = std::unique_ptr<GTiffGrid>(
        new GTiffGrid(m_ctx, m_hTIFF, m_cache, m_ifdIdx, width, height, extent,
                      dt, samplesPerPixel, planarConfig, vRes < 0));
    m_ifdIdx++;
    m_hasNextGrid = TIFFReadDirectory(m_hTIFF) != 0;
    m_nextDirOffset = TIFFCurrentDirOffset(m_hTIFF);
    return ret;
}

// ---------------------------------------------------------------------------

class GTiffVGridShiftSet : public VerticalShiftGridSet, public GTiffDataset {

    GTiffVGridShiftSet(PJ_CONTEXT *ctx, PAFile fp) : GTiffDataset(ctx, fp) {}

  public:
    ~GTiffVGridShiftSet() override;

    static std::unique_ptr<GTiffVGridShiftSet>
    open(PJ_CONTEXT *ctx, PAFile fp, const std::string &filename);
};

// ---------------------------------------------------------------------------

template <class GridType, class GenericGridType>
static void
insertIntoHierarchy(PJ_CONTEXT *ctx, std::unique_ptr<GridType> &&grid,
                    const std::string &gridName, const std::string &parentName,
                    std::vector<std::unique_ptr<GenericGridType>> &topGrids,
                    std::map<std::string, GridType *> &mapGrids) {
    const auto &extent = grid->extentAndRes();

    // If we have one or both of grid_name and parent_name, try to use
    // the names to recreate the hiearchy
    if (!gridName.empty()) {
        if (mapGrids.find(gridName) != mapGrids.end()) {
            pj_log(ctx, PJ_LOG_DEBUG_MAJOR, "Several grids called %s found!",
                   gridName.c_str());
        }
        mapGrids[gridName] = grid.get();
    }
    bool gridInserted = false;
    if (!parentName.empty()) {
        auto iter = mapGrids.find(parentName);
        if (iter == mapGrids.end()) {
            pj_log(ctx, PJ_LOG_DEBUG_MAJOR,
                   "Grid %s refers to non-existing parent %s. "
                   "Using bounding-box method.",
                   gridName.c_str(), parentName.c_str());
        } else {
            if (iter->second->extentAndRes().contains(extent)) {
                iter->second->m_children.emplace_back(std::move(grid));
                gridInserted = true;
            } else {
                pj_log(ctx, PJ_LOG_DEBUG_MAJOR,
                       "Grid %s refers to parent %s, but its extent is "
                       "not included in it. Using bounding-box method.",
                       gridName.c_str(), parentName.c_str());
            }
        }
    } else if (!gridName.empty()) {
        topGrids.emplace_back(std::move(grid));
        gridInserted = true;
    }

    // Fallback to analyzing spatial extents
    if (!gridInserted) {
        for (const auto &candidateParent : topGrids) {
            const auto &candidateParentExtent = candidateParent->extentAndRes();
            if (candidateParentExtent.contains(extent)) {
                static_cast<GridType *>(candidateParent.get())
                    ->insertGrid(ctx, std::move(grid));
                gridInserted = true;
                break;
            } else if (candidateParentExtent.intersects(extent)) {
                pj_log(ctx, PJ_LOG_DEBUG_MAJOR,
                       "Partially intersecting grids found!");
            }
        }
        if (!gridInserted) {
            topGrids.emplace_back(std::move(grid));
        }
    }
}

// ---------------------------------------------------------------------------

class GTiffVGrid : public VerticalShiftGrid {
    friend void insertIntoHierarchy<GTiffVGrid, VerticalShiftGrid>(
        PJ_CONTEXT *ctx, std::unique_ptr<GTiffVGrid> &&grid,
        const std::string &gridName, const std::string &parentName,
        std::vector<std::unique_ptr<VerticalShiftGrid>> &topGrids,
        std::map<std::string, GTiffVGrid *> &mapGrids);

    std::unique_ptr<GTiffGrid> m_grid;
    uint16 m_idxSample;

  public:
    GTiffVGrid(std::unique_ptr<GTiffGrid> &&grid, uint16 idxSample);

    ~GTiffVGrid() override;

    bool valueAt(int x, int y, float &out) const override {
        return m_grid->valueAt(m_idxSample, x, y, out);
    }

    bool isNodata(float val, double /* multiplier */) const override {
        return m_grid->isNodata(val);
    }

    void insertGrid(PJ_CONTEXT *ctx, std::unique_ptr<GTiffVGrid> &&subgrid);
};

// ---------------------------------------------------------------------------

GTiffVGridShiftSet::~GTiffVGridShiftSet() = default;

// ---------------------------------------------------------------------------

GTiffVGrid::GTiffVGrid(std::unique_ptr<GTiffGrid> &&grid, uint16 idxSample)
    : VerticalShiftGrid(grid->width(), grid->height(), grid->extentAndRes()),
      m_grid(std::move(grid)), m_idxSample(idxSample) {}

// ---------------------------------------------------------------------------

GTiffVGrid::~GTiffVGrid() = default;

// ---------------------------------------------------------------------------

void GTiffVGrid::insertGrid(PJ_CONTEXT *ctx,
                            std::unique_ptr<GTiffVGrid> &&subgrid) {
    bool gridInserted = false;
    const auto &extent = subgrid->extentAndRes();
    for (const auto &candidateParent : m_children) {
        const auto &candidateParentExtent = candidateParent->extentAndRes();
        if (candidateParentExtent.contains(extent)) {
            static_cast<GTiffVGrid *>(candidateParent.get())
                ->insertGrid(ctx, std::move(subgrid));
            gridInserted = true;
            break;
        } else if (candidateParentExtent.intersects(extent)) {
            pj_log(ctx, PJ_LOG_DEBUG_MAJOR,
                   "Partially intersecting grids found!");
        }
    }
    if (!gridInserted) {
        m_children.emplace_back(std::move(subgrid));
    }
}

// ---------------------------------------------------------------------------

std::unique_ptr<GTiffVGridShiftSet>
GTiffVGridShiftSet::open(PJ_CONTEXT *ctx, PAFile fp,
                         const std::string &filename) {
    auto set =
        std::unique_ptr<GTiffVGridShiftSet>(new GTiffVGridShiftSet(ctx, fp));
    set->m_name = filename;
    set->m_format = "gtiff";
    if (!set->openTIFF(filename)) {
        return nullptr;
    }
    uint16 idxSample = 0;

    std::map<std::string, GTiffVGrid *> mapGrids;
    for (int ifd = 0;; ++ifd) {
        auto grid = set->nextGrid();
        if (!grid) {
            if (ifd == 0) {
                return nullptr;
            }
            break;
        }

        const auto subfileType = grid->subfileType();
        if (subfileType != 0 && subfileType != FILETYPE_PAGE) {
            if (ifd == 0) {
                pj_log(ctx, PJ_LOG_ERROR, "Invalid subfileType");
                return nullptr;
            } else {
                pj_log(ctx, PJ_LOG_DEBUG_MAJOR,
                       "Ignoring IFD %d as it has a unsupported subfileType",
                       ifd);
                continue;
            }
        }

        // Identify the index of the geoid_undulation/vertical_offset
        bool foundDescriptionForAtLeastOneSample = false;
        bool foundDescriptionForShift = false;
        for (int i = 0; i < static_cast<int>(grid->samplesPerPixel()); ++i) {
            const auto desc = grid->metadataItem("DESCRIPTION", i);
            if (!desc.empty()) {
                foundDescriptionForAtLeastOneSample = true;
            }
            if (desc == "geoid_undulation" || desc == "vertical_offset") {
                idxSample = static_cast<uint16>(i);
                foundDescriptionForShift = true;
            }
        }

        if (foundDescriptionForAtLeastOneSample) {
            if (!foundDescriptionForShift) {
                if (ifd > 0) {
                    // Assuming that extra IFD without our channel of interest
                    // can be ignored
                    // One could imagine to put the accuracy values in separate
                    // IFD for example
                    pj_log(ctx, PJ_LOG_DEBUG_MAJOR,
                           "Ignoring IFD %d as it has no "
                           "geoid_undulation/vertical_offset channel",
                           ifd);
                    continue;
                } else {
                    pj_log(ctx, PJ_LOG_DEBUG_MAJOR,
                           "IFD 0 has channel descriptions, but no "
                           "geoid_undulation/vertical_offset channel");
                    return nullptr;
                }
            }
        }

        if (idxSample >= grid->samplesPerPixel()) {
            pj_log(ctx, PJ_LOG_ERROR, "Invalid sample index");
            return nullptr;
        }

        const std::string gridName = grid->metadataItem("grid_name");
        const std::string parentName = grid->metadataItem("parent_name");

        auto vgrid =
            internal::make_unique<GTiffVGrid>(std::move(grid), idxSample);

        insertIntoHierarchy(ctx, std::move(vgrid), gridName, parentName,
                            set->m_grids, mapGrids);
    }
    return set;
}

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

    /* -------------------------------------------------------------------- */
    /*      Load a header, to determine the file type.                      */
    /* -------------------------------------------------------------------- */
    unsigned char header[4];
    size_t header_size;
    if ((header_size = pj_ctx_fread(ctx, header, 1, sizeof(header), fp)) !=
        sizeof(header)) {
        pj_ctx_fclose(ctx, fp);
        return nullptr;
    }
    pj_ctx_fseek(ctx, fp, SEEK_SET, 0);

    if (IsTIFF(header_size, header)) {
        auto set = GTiffVGridShiftSet::open(ctx, fp, filename);
        if (!set)
            pj_ctx_set_errno(ctx, PJD_ERR_FAILED_TO_LOAD_GRID);
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
    : Grid(widthIn, heightIn, extentIn) {}

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
    // west longitude positive convention !
    lonShift = -static_cast<float>(two_doubles[1] * ((M_PI / 180.0) / 3600.0));

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
    // west longitude positive convention !
    lonShift = -two_floats[0];

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
    // west longitude positive convention !
    lonShift = -static_cast<float>(two_float[1] * ((M_PI / 180.0) / 3600.0));
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

class GTiffHGridShiftSet : public HorizontalShiftGridSet, public GTiffDataset {

    GTiffHGridShiftSet(PJ_CONTEXT *ctx, PAFile fp) : GTiffDataset(ctx, fp) {}

  public:
    ~GTiffHGridShiftSet() override;

    static std::unique_ptr<GTiffHGridShiftSet>
    open(PJ_CONTEXT *ctx, PAFile fp, const std::string &filename);
};

// ---------------------------------------------------------------------------

class GTiffHGrid : public HorizontalShiftGrid {
    friend void insertIntoHierarchy<GTiffHGrid, HorizontalShiftGrid>(
        PJ_CONTEXT *ctx, std::unique_ptr<GTiffHGrid> &&grid,
        const std::string &gridName, const std::string &parentName,
        std::vector<std::unique_ptr<HorizontalShiftGrid>> &topGrids,
        std::map<std::string, GTiffHGrid *> &mapGrids);

    std::unique_ptr<GTiffGrid> m_grid;
    uint16 m_idxLatShift;
    uint16 m_idxLonShift;
    double m_convFactorToRadian;
    bool m_positiveEast;

  public:
    GTiffHGrid(std::unique_ptr<GTiffGrid> &&grid, uint16 idxLatShift,
               uint16 idxLonShift, double convFactorToRadian,
               bool positiveEast);

    ~GTiffHGrid() override;

    bool valueAt(int x, int y, float &lonShift, float &latShift) const override;

    void insertGrid(PJ_CONTEXT *ctx, std::unique_ptr<GTiffHGrid> &&subgrid);
};

// ---------------------------------------------------------------------------

GTiffHGridShiftSet::~GTiffHGridShiftSet() = default;

// ---------------------------------------------------------------------------

GTiffHGrid::GTiffHGrid(std::unique_ptr<GTiffGrid> &&grid, uint16 idxLatShift,
                       uint16 idxLonShift, double convFactorToRadian,
                       bool positiveEast)
    : HorizontalShiftGrid(grid->width(), grid->height(), grid->extentAndRes()),
      m_grid(std::move(grid)), m_idxLatShift(idxLatShift),
      m_idxLonShift(idxLonShift), m_convFactorToRadian(convFactorToRadian),
      m_positiveEast(positiveEast) {}

// ---------------------------------------------------------------------------

GTiffHGrid::~GTiffHGrid() = default;

// ---------------------------------------------------------------------------

bool GTiffHGrid::valueAt(int x, int y, float &lonShift, float &latShift) const {
    if (!m_grid->valueAt(m_idxLatShift, x, y, latShift) ||
        !m_grid->valueAt(m_idxLonShift, x, y, lonShift)) {
        return false;
    }
    // From arc-seconds to radians
    latShift = static_cast<float>(latShift * m_convFactorToRadian);
    lonShift = static_cast<float>(lonShift * m_convFactorToRadian);
    if (!m_positiveEast) {
        lonShift = -lonShift;
    }
    return true;
}

// ---------------------------------------------------------------------------

void GTiffHGrid::insertGrid(PJ_CONTEXT *ctx,
                            std::unique_ptr<GTiffHGrid> &&subgrid) {
    bool gridInserted = false;
    const auto &extent = subgrid->extentAndRes();
    for (const auto &candidateParent : m_children) {
        const auto &candidateParentExtent = candidateParent->extentAndRes();
        if (candidateParentExtent.contains(extent)) {
            static_cast<GTiffHGrid *>(candidateParent.get())
                ->insertGrid(ctx, std::move(subgrid));
            gridInserted = true;
            break;
        } else if (candidateParentExtent.intersects(extent)) {
            pj_log(ctx, PJ_LOG_DEBUG_MAJOR,
                   "Partially intersecting grids found!");
        }
    }
    if (!gridInserted) {
        m_children.emplace_back(std::move(subgrid));
    }
}

// ---------------------------------------------------------------------------

std::unique_ptr<GTiffHGridShiftSet>
GTiffHGridShiftSet::open(PJ_CONTEXT *ctx, PAFile fp,
                         const std::string &filename) {
    auto set =
        std::unique_ptr<GTiffHGridShiftSet>(new GTiffHGridShiftSet(ctx, fp));
    set->m_name = filename;
    set->m_format = "gtiff";
    if (!set->openTIFF(filename)) {
        return nullptr;
    }

    // Defaults inspired from NTv2
    uint16 idxLatShift = 0;
    uint16 idxLonShift = 1;
    constexpr double ARC_SECOND_TO_RADIAN = (M_PI / 180.0) / 3600.0;
    double convFactorToRadian = ARC_SECOND_TO_RADIAN;
    bool positiveEast = true;

    std::map<std::string, GTiffHGrid *> mapGrids;
    for (int ifd = 0;; ++ifd) {
        auto grid = set->nextGrid();
        if (!grid) {
            if (ifd == 0) {
                return nullptr;
            }
            break;
        }

        const auto subfileType = grid->subfileType();
        if (subfileType != 0 && subfileType != FILETYPE_PAGE) {
            if (ifd == 0) {
                pj_log(ctx, PJ_LOG_ERROR, "Invalid subfileType");
                return nullptr;
            } else {
                pj_log(ctx, PJ_LOG_DEBUG_MAJOR,
                       "Ignoring IFD %d as it has a unsupported subfileType",
                       ifd);
                continue;
            }
        }

        if (grid->samplesPerPixel() < 2) {
            if (ifd == 0) {
                pj_log(ctx, PJ_LOG_ERROR,
                       "At least 2 samples per pixel needed");
                return nullptr;
            } else {
                pj_log(ctx, PJ_LOG_DEBUG_MAJOR,
                       "Ignoring IFD %d as it has not at least 2 samples", ifd);
                continue;
            }
        }

        // Identify the index of the latitude and longitude offset channels
        bool foundDescriptionForAtLeastOneSample = false;
        bool foundDescriptionForLatOffset = false;
        bool foundDescriptionForLonOffset = false;
        for (int i = 0; i < static_cast<int>(grid->samplesPerPixel()); ++i) {
            const auto desc = grid->metadataItem("DESCRIPTION", i);
            if (!desc.empty()) {
                foundDescriptionForAtLeastOneSample = true;
            }
            if (desc == "latitude_offset") {
                idxLatShift = static_cast<uint16>(i);
                foundDescriptionForLatOffset = true;
            } else if (desc == "longitude_offset") {
                idxLonShift = static_cast<uint16>(i);
                foundDescriptionForLonOffset = true;
            }
        }

        if (foundDescriptionForAtLeastOneSample) {
            if (!foundDescriptionForLonOffset &&
                !foundDescriptionForLatOffset) {
                if (ifd > 0) {
                    // Assuming that extra IFD without
                    // longitude_offset/latitude_offset can be ignored
                    // One could imagine to put the accuracy values in separate
                    // IFD for example
                    pj_log(ctx, PJ_LOG_DEBUG_MAJOR,
                           "Ignoring IFD %d as it has no "
                           "longitude_offset/latitude_offset channel",
                           ifd);
                    continue;
                } else {
                    pj_log(ctx, PJ_LOG_DEBUG_MAJOR,
                           "IFD 0 has channel descriptions, but no "
                           "longitude_offset/latitude_offset channel");
                    return nullptr;
                }
            }
        }
        if (foundDescriptionForLatOffset && !foundDescriptionForLonOffset) {
            pj_log(ctx, PJ_LOG_ERROR,
                   "Found latitude_offset channel, but not longitude_offset");
            return nullptr;
        } else if (foundDescriptionForLonOffset &&
                   !foundDescriptionForLatOffset) {
            pj_log(ctx, PJ_LOG_ERROR,
                   "Found longitude_offset channel, but not latitude_offset");
            return nullptr;
        }

        if (idxLatShift >= grid->samplesPerPixel() ||
            idxLonShift >= grid->samplesPerPixel()) {
            pj_log(ctx, PJ_LOG_ERROR, "Invalid sample index");
            return nullptr;
        }

        if (foundDescriptionForLonOffset) {
            const std::string positiveValue =
                grid->metadataItem("positive_value", idxLonShift);
            if (!positiveValue.empty()) {
                if (positiveValue == "west") {
                    positiveEast = false;
                } else if (positiveValue == "east") {
                    positiveEast = true;
                } else {
                    pj_log(ctx, PJ_LOG_ERROR,
                           "Unsupported value %s for 'positive_value'",
                           positiveValue.c_str());
                    return nullptr;
                }
            }
        }

        // Identify their unit
        {
            const auto unitLatShift =
                grid->metadataItem("UNITTYPE", idxLatShift);
            const auto unitLonShift =
                grid->metadataItem("UNITTYPE", idxLonShift);
            if (unitLatShift != unitLonShift) {
                pj_log(ctx, PJ_LOG_ERROR,
                       "Different unit for longitude and latitude offset");
                return nullptr;
            }
            if (!unitLatShift.empty()) {
                if (unitLatShift == "arc-second") {
                    convFactorToRadian = ARC_SECOND_TO_RADIAN;
                } else if (unitLatShift == "radian") {
                    convFactorToRadian = 1.0;
                } else if (unitLatShift == "degree") {
                    convFactorToRadian = M_PI / 180.0;
                } else {
                    pj_log(ctx, PJ_LOG_ERROR, "Unsupported unit %s",
                           unitLatShift.c_str());
                    return nullptr;
                }
            }
        }

        const std::string gridName = grid->metadataItem("grid_name");
        const std::string parentName = grid->metadataItem("parent_name");

        auto hgrid = internal::make_unique<GTiffHGrid>(
            std::move(grid), idxLatShift, idxLonShift, convFactorToRadian,
            positiveEast);

        insertIntoHierarchy(ctx, std::move(hgrid), gridName, parentName,
                            set->m_grids, mapGrids);
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
    } else if (IsTIFF(header_size,
                      reinterpret_cast<const unsigned char *>(header))) {
        auto set = GTiffHGridShiftSet::open(ctx, fp, filename);
        if (!set)
            pj_ctx_set_errno(ctx, PJD_ERR_FAILED_TO_LOAD_GRID);
        return set;
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

// ---------------------------------------------------------------------------

class GTiffGenericGridShiftSet : public GenericShiftGridSet,
                                 public GTiffDataset {

    GTiffGenericGridShiftSet(PJ_CONTEXT *ctx, PAFile fp)
        : GTiffDataset(ctx, fp) {}

  public:
    ~GTiffGenericGridShiftSet() override;

    static std::unique_ptr<GTiffGenericGridShiftSet>
    open(PJ_CONTEXT *ctx, PAFile fp, const std::string &filename);
};

// ---------------------------------------------------------------------------

class GTiffGenericGrid : public GenericShiftGrid {
    friend void insertIntoHierarchy<GTiffGenericGrid, GenericShiftGrid>(
        PJ_CONTEXT *ctx, std::unique_ptr<GTiffGenericGrid> &&grid,
        const std::string &gridName, const std::string &parentName,
        std::vector<std::unique_ptr<GenericShiftGrid>> &topGrids,
        std::map<std::string, GTiffGenericGrid *> &mapGrids);

    std::unique_ptr<GTiffGrid> m_grid;

  public:
    GTiffGenericGrid(std::unique_ptr<GTiffGrid> &&grid);

    ~GTiffGenericGrid() override;

    bool valueAt(int x, int y, int sample, float &out) const override;

    int samplesPerPixel() const override { return m_grid->samplesPerPixel(); }

    std::string unit(int sample) const override {
        return m_grid->metadataItem("UNITTYPE", sample);
    }

    std::string description(int sample) const override {
        return m_grid->metadataItem("DESCRIPTION", sample);
    }

    std::string metadataItem(const std::string &key,
                             int sample = -1) const override {
        return m_grid->metadataItem(key, sample);
    }

    void insertGrid(PJ_CONTEXT *ctx,
                    std::unique_ptr<GTiffGenericGrid> &&subgrid);
};

// ---------------------------------------------------------------------------

GTiffGenericGridShiftSet::~GTiffGenericGridShiftSet() = default;

// ---------------------------------------------------------------------------

GTiffGenericGrid::GTiffGenericGrid(std::unique_ptr<GTiffGrid> &&grid)
    : GenericShiftGrid(grid->width(), grid->height(), grid->extentAndRes()),
      m_grid(std::move(grid)) {}

// ---------------------------------------------------------------------------

GTiffGenericGrid::~GTiffGenericGrid() = default;

// ---------------------------------------------------------------------------

bool GTiffGenericGrid::valueAt(int x, int y, int sample, float &out) const {
    if (sample < 0 ||
        static_cast<unsigned>(sample) >= m_grid->samplesPerPixel())
        return false;
    return m_grid->valueAt(static_cast<uint16>(sample), x, y, out);
}

// ---------------------------------------------------------------------------

void GTiffGenericGrid::insertGrid(PJ_CONTEXT *ctx,
                                  std::unique_ptr<GTiffGenericGrid> &&subgrid) {
    bool gridInserted = false;
    const auto &extent = subgrid->extentAndRes();
    for (const auto &candidateParent : m_children) {
        const auto &candidateParentExtent = candidateParent->extentAndRes();
        if (candidateParentExtent.contains(extent)) {
            static_cast<GTiffGenericGrid *>(candidateParent.get())
                ->insertGrid(ctx, std::move(subgrid));
            gridInserted = true;
            break;
        } else if (candidateParentExtent.intersects(extent)) {
            pj_log(ctx, PJ_LOG_DEBUG_MAJOR,
                   "Partially intersecting grids found!");
        }
    }
    if (!gridInserted) {
        m_children.emplace_back(std::move(subgrid));
    }
}

// ---------------------------------------------------------------------------

class NullGenericShiftGrid : public GenericShiftGrid {

  public:
    NullGenericShiftGrid() : GenericShiftGrid(3, 3, globalExtent()) {}

    bool isNullGrid() const override { return true; }
    bool valueAt(int, int, int, float &out) const override;

    int samplesPerPixel() const override { return 0; }

    std::string unit(int) const override { return std::string(); }

    std::string description(int) const override { return std::string(); }

    std::string metadataItem(const std::string &, int) const override {
        return std::string();
    }
};

// ---------------------------------------------------------------------------

bool NullGenericShiftGrid::valueAt(int, int, int, float &out) const {
    out = 0.0f;
    return true;
}

// ---------------------------------------------------------------------------

std::unique_ptr<GTiffGenericGridShiftSet>
GTiffGenericGridShiftSet::open(PJ_CONTEXT *ctx, PAFile fp,
                               const std::string &filename) {
    auto set = std::unique_ptr<GTiffGenericGridShiftSet>(
        new GTiffGenericGridShiftSet(ctx, fp));
    set->m_name = filename;
    set->m_format = "gtiff";
    if (!set->openTIFF(filename)) {
        return nullptr;
    }

    std::map<std::string, GTiffGenericGrid *> mapGrids;
    for (int ifd = 0;; ++ifd) {
        auto grid = set->nextGrid();
        if (!grid) {
            if (ifd == 0) {
                return nullptr;
            }
            break;
        }

        const auto subfileType = grid->subfileType();
        if (subfileType != 0 && subfileType != FILETYPE_PAGE) {
            if (ifd == 0) {
                pj_log(ctx, PJ_LOG_ERROR, "Invalid subfileType");
                return nullptr;
            } else {
                pj_log(ctx, PJ_LOG_DEBUG_MAJOR,
                       "Ignoring IFD %d as it has a unsupported subfileType",
                       ifd);
                continue;
            }
        }

        const std::string gridName = grid->metadataItem("grid_name");
        const std::string parentName = grid->metadataItem("parent_name");

        auto hgrid = internal::make_unique<GTiffGenericGrid>(std::move(grid));

        insertIntoHierarchy(ctx, std::move(hgrid), gridName, parentName,
                            set->m_grids, mapGrids);
    }
    return set;
}

// ---------------------------------------------------------------------------

GenericShiftGrid::GenericShiftGrid(int widthIn, int heightIn,
                                   const ExtentAndRes &extentIn)
    : Grid(widthIn, heightIn, extentIn) {}

// ---------------------------------------------------------------------------

GenericShiftGrid::~GenericShiftGrid() = default;

// ---------------------------------------------------------------------------

GenericShiftGridSet::GenericShiftGridSet() = default;

// ---------------------------------------------------------------------------

GenericShiftGridSet::~GenericShiftGridSet() = default;

// ---------------------------------------------------------------------------

std::unique_ptr<GenericShiftGridSet>
GenericShiftGridSet::open(PJ_CONTEXT *ctx, const std::string &filename) {
    if (filename == "null") {
        auto set =
            std::unique_ptr<GenericShiftGridSet>(new GenericShiftGridSet());
        set->m_name = filename;
        set->m_format = "null";
        set->m_grids.push_back(
            std::unique_ptr<NullGenericShiftGrid>(new NullGenericShiftGrid()));
        return set;
    }

    PAFile fp;
    if (!(fp = pj_open_lib(ctx, filename.c_str(), "rb"))) {
        ctx->last_errno = 0; /* don't treat as a persistent error */
        return nullptr;
    }

    /* -------------------------------------------------------------------- */
    /*      Load a header, to determine the file type.                      */
    /* -------------------------------------------------------------------- */
    unsigned char header[4];
    size_t header_size;
    if ((header_size = pj_ctx_fread(ctx, header, 1, sizeof(header), fp)) !=
        sizeof(header)) {
        pj_ctx_fclose(ctx, fp);
        return nullptr;
    }
    pj_ctx_fseek(ctx, fp, SEEK_SET, 0);

    if (IsTIFF(header_size, header)) {
        auto set = GTiffGenericGridShiftSet::open(ctx, fp, filename);
        if (!set)
            pj_ctx_set_errno(ctx, PJD_ERR_FAILED_TO_LOAD_GRID);
        return set;
    }

    pj_log(ctx, PJ_LOG_DEBUG_MAJOR, "Unrecognized generic grid format");
    pj_ctx_fclose(ctx, fp);
    return nullptr;
}

// ---------------------------------------------------------------------------

const GenericShiftGrid *GenericShiftGrid::gridAt(double lon, double lat) const {
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

const GenericShiftGrid *GenericShiftGridSet::gridAt(double lon,
                                                    double lat) const {
    for (const auto &grid : m_grids) {
        if (dynamic_cast<NullGenericShiftGrid *>(grid.get())) {
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

ListOfGenericGrids proj_generic_grid_init(PJ *P, const char *gridkey) {
    std::string key("s");
    key += gridkey;
    const char *gridnames = pj_param(P->ctx, P->params, key.c_str()).s;
    if (gridnames == nullptr)
        return {};

    auto listOfGridNames = internal::split(std::string(gridnames), ',');
    ListOfGenericGrids grids;
    for (const auto &gridnameStr : listOfGridNames) {
        const char *gridname = gridnameStr.c_str();
        bool canFail = false;
        if (gridname[0] == '@') {
            canFail = true;
            gridname++;
        }
        auto gridSet = GenericShiftGridSet::open(P->ctx, gridname);
        if (!gridSet) {
            if (!canFail) {
                pj_ctx_set_errno(P->ctx, PJD_ERR_FAILED_TO_LOAD_GRID);
                return {};
            }
        } else {
            grids.emplace_back(std::move(gridSet));
        }
    }

    return grids;
}

NS_PROJ_END
