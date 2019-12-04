/******************************************************************************
 * Project:  PROJ
 * Purpose:  Grid management
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

#ifndef GRIDS_HPP_INCLUDED
#define GRIDS_HPP_INCLUDED

#include <memory>
#include <vector>

#include "proj/util.hpp"

typedef struct projCtx_t PJ_CONTEXT;

NS_PROJ_START

struct ExtentAndRes {
    double westLon;  // in radian
    double southLat; // in radian
    double eastLon;  // in radian
    double northLat; // in radian
    double resLon;   // in radian
    double resLat;   // in radian

    bool fullWorldLongitude() const;
};

// ---------------------------------------------------------------------------

class VerticalShiftGrid {
  protected:
    int m_width;
    int m_height;
    ExtentAndRes m_extent;
    std::vector<std::unique_ptr<VerticalShiftGrid>> m_children{};

  public:
    VerticalShiftGrid(int widthIn, int heightIn, const ExtentAndRes &extentIn);
    virtual ~VerticalShiftGrid();

    int width() const { return m_width; }
    int height() const { return m_height; }
    const ExtentAndRes &extentAndRes() const { return m_extent; }

    const VerticalShiftGrid *gridAt(double lon, double lat) const;

    virtual bool isNodata(float /*val*/, double /* multiplier */) const {
        return false;
    }

    // x = 0 is western-most column, y = 0 is southern-most line
    virtual bool valueAt(int x, int y, float &out) const = 0;
};

// ---------------------------------------------------------------------------

class VerticalShiftGridSet {
    std::string m_name{};
    std::string m_format{};
    std::vector<std::unique_ptr<VerticalShiftGrid>> m_grids{};

    VerticalShiftGridSet();

  public:
    virtual ~VerticalShiftGridSet();

    static std::unique_ptr<VerticalShiftGridSet>
    open(PJ_CONTEXT *ctx, const std::string &filename);

    const std::string &name() const { return m_name; }
    const std::string &format() const { return m_format; }
    const std::vector<std::unique_ptr<VerticalShiftGrid>> &grids() const {
        return m_grids;
    }
    const VerticalShiftGrid *gridAt(double lon, double lat) const;
};

NS_PROJ_END

#endif // GRIDS_HPP_INCLUDED
