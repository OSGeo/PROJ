/******************************************************************************
 *
 * Project:  PROJ
 * Purpose:  Test grids.hpp
 * Author:   Even Rouault <even dot rouault at spatialys dot com>
 *
 ******************************************************************************
 * Copyright (c) 2020, Even Rouault <even dot rouault at spatialys dot com>
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

#include "gtest_include.h"

#include "grids.hpp"

#include "proj_internal.h" // M_PI

namespace {

// ---------------------------------------------------------------------------

class GridTest : public ::testing::Test {

    static void DummyLogFunction(void *, int, const char *) {}

  protected:
    void SetUp() override {
        m_ctxt = proj_context_create();
        proj_log_func(m_ctxt, nullptr, DummyLogFunction);
        m_ctxt2 = proj_context_create();
        proj_log_func(m_ctxt2, nullptr, DummyLogFunction);
    }

    void TearDown() override {
        proj_context_destroy(m_ctxt);
        proj_context_destroy(m_ctxt2);
    }

    PJ_CONTEXT *m_ctxt = nullptr;
    PJ_CONTEXT *m_ctxt2 = nullptr;
};

// ---------------------------------------------------------------------------

TEST_F(GridTest, VerticalShiftGridSet_null) {
    auto gridSet = NS_PROJ::VerticalShiftGridSet::open(m_ctxt, "null");
    ASSERT_NE(gridSet, nullptr);
    auto grid = gridSet->gridAt(0.0, 0.0);
    ASSERT_NE(grid, nullptr);
    EXPECT_EQ(grid->width(), 3);
    EXPECT_EQ(grid->height(), 3);
    EXPECT_EQ(grid->extentAndRes().west, -M_PI);
    EXPECT_TRUE(grid->isNullGrid());
    EXPECT_FALSE(grid->hasChanged());
    float out = -1.0f;
    EXPECT_TRUE(grid->valueAt(0, 0, out));
    EXPECT_EQ(out, 0.0f);
    EXPECT_FALSE(grid->isNodata(0.0f, 0.0));
    gridSet->reassign_context(m_ctxt2);
    gridSet->reopen(m_ctxt2);
}

// ---------------------------------------------------------------------------

TEST_F(GridTest, VerticalShiftGridSet_gtx) {
    ASSERT_EQ(NS_PROJ::VerticalShiftGridSet::open(m_ctxt, "foobar"), nullptr);
    auto gridSet =
        NS_PROJ::VerticalShiftGridSet::open(m_ctxt, "tests/test_nodata.gtx");
    ASSERT_NE(gridSet, nullptr);
    ASSERT_EQ(gridSet->gridAt(-100, -100), nullptr);
    auto grid = gridSet->gridAt(4.15 / 180 * M_PI, 52.15 / 180 * M_PI);
    ASSERT_NE(grid, nullptr);
    EXPECT_TRUE(grid->isNodata(-88.8888f, 1.0));
    gridSet->reassign_context(m_ctxt2);
    gridSet->reopen(m_ctxt2);
    grid = gridSet->gridAt(4.15 / 180 * M_PI, 52.15 / 180 * M_PI);
    EXPECT_NE(grid, nullptr);
}

// ---------------------------------------------------------------------------

TEST_F(GridTest, HorizontalShiftGridSet_null) {
    auto gridSet = NS_PROJ::HorizontalShiftGridSet::open(m_ctxt, "null");
    ASSERT_NE(gridSet, nullptr);
    auto grid = gridSet->gridAt(0.0, 0.0);
    ASSERT_NE(grid, nullptr);
    EXPECT_EQ(grid->width(), 3);
    EXPECT_EQ(grid->height(), 3);
    EXPECT_EQ(grid->extentAndRes().west, -M_PI);
    EXPECT_TRUE(grid->isNullGrid());
    EXPECT_FALSE(grid->hasChanged());
    float out1 = -1.0f;
    float out2 = -1.0f;
    EXPECT_TRUE(grid->valueAt(0, 0, false, out1, out2));
    EXPECT_EQ(out1, 0.0f);
    EXPECT_EQ(out2, 0.0f);
    gridSet->reassign_context(m_ctxt2);
    gridSet->reopen(m_ctxt2);
}

// ---------------------------------------------------------------------------

TEST_F(GridTest, HorizontalShiftGridSet_gtiff) {
    auto gridSet =
        NS_PROJ::HorizontalShiftGridSet::open(m_ctxt, "tests/test_hgrid.tif");
    ASSERT_NE(gridSet, nullptr);
    EXPECT_EQ(gridSet->format(), "gtiff");
    EXPECT_TRUE(gridSet->name().find("tests/test_hgrid.tif") !=
                std::string::npos)
        << gridSet->name();
    EXPECT_EQ(gridSet->grids().size(), 1U);
    ASSERT_EQ(gridSet->gridAt(-100, -100), nullptr);
    auto grid = gridSet->gridAt(5.5 / 180 * M_PI, 53.5 / 180 * M_PI);
    ASSERT_NE(grid, nullptr);
    EXPECT_EQ(grid->width(), 4);
    EXPECT_EQ(grid->height(), 4);
    EXPECT_EQ(grid->extentAndRes().west, 4.0 / 180 * M_PI);
    EXPECT_FALSE(grid->isNullGrid());
    EXPECT_FALSE(grid->hasChanged());
    float out1 = -1.0f;
    float out2 = -1.0f;
    EXPECT_TRUE(grid->valueAt(0, 3, false, out1, out2));
    EXPECT_EQ(out1, static_cast<float>(14400.0 / 3600. / 180 * M_PI));
    EXPECT_EQ(out2, static_cast<float>(900.0 / 3600. / 180 * M_PI));
    gridSet->reassign_context(m_ctxt2);
    gridSet->reopen(m_ctxt2);
    grid = gridSet->gridAt(5.5 / 180 * M_PI, 53.5 / 180 * M_PI);
    EXPECT_NE(grid, nullptr);
}

// ---------------------------------------------------------------------------

TEST_F(GridTest, GenericShiftGridSet_null) {
    auto gridSet = NS_PROJ::GenericShiftGridSet::open(m_ctxt, "null");
    ASSERT_NE(gridSet, nullptr);
    auto grid = gridSet->gridAt(0.0, 0.0);
    ASSERT_NE(grid, nullptr);
    EXPECT_EQ(grid->width(), 3);
    EXPECT_EQ(grid->height(), 3);
    EXPECT_EQ(grid->extentAndRes().west, -M_PI);
    EXPECT_TRUE(grid->isNullGrid());
    EXPECT_FALSE(grid->hasChanged());
    float out = -1.0f;
    EXPECT_TRUE(grid->valueAt(0, 0, 0, out));
    EXPECT_EQ(out, 0.0f);
    EXPECT_EQ(grid->unit(0), "");
    EXPECT_EQ(grid->description(0), "");
    EXPECT_EQ(grid->metadataItem("foo"), "");
    EXPECT_EQ(grid->samplesPerPixel(), 0);
    gridSet->reassign_context(m_ctxt2);
    gridSet->reopen(m_ctxt2);
}

// ---------------------------------------------------------------------------

TEST_F(GridTest, GenericShiftGridSet_gtiff) {
    ASSERT_EQ(NS_PROJ::GenericShiftGridSet::open(m_ctxt, "foobar"), nullptr);
    auto gridSet = NS_PROJ::GenericShiftGridSet::open(
        m_ctxt, "tests/nkgrf03vel_realigned_extract.tif");
    ASSERT_NE(gridSet, nullptr);
    ASSERT_EQ(gridSet->gridAt(-100, -100), nullptr);
    auto grid = gridSet->gridAt(21.3333333 / 180 * M_PI, 63.0 / 180 * M_PI);
    ASSERT_NE(grid, nullptr);
    EXPECT_EQ(grid->width(), 5);
    EXPECT_EQ(grid->height(), 5);
    EXPECT_EQ(grid->extentAndRes().isGeographic, true);
    EXPECT_EQ(grid->extentAndRes().west, 21.0 / 180 * M_PI);
    EXPECT_FALSE(grid->isNullGrid());
    EXPECT_FALSE(grid->hasChanged());
    float out = -1.0f;
    EXPECT_FALSE(grid->valueAt(0, 0, grid->samplesPerPixel(), out));
    EXPECT_TRUE(grid->valueAt(0, 0, 0, out));
    EXPECT_EQ(out, 0.20783890783786773682f);
    EXPECT_TRUE(grid->valueAt(1, 0, 0, out));
    EXPECT_EQ(out, 0.22427035868167877197);
    EXPECT_TRUE(grid->valueAt(0, 1, 0, out));
    EXPECT_EQ(out, 0.19718019664287567139f);
    int bandZero = 0;
    EXPECT_TRUE(grid->valuesAt(0, 0, 1, 1, 1, &bandZero, &out));
    EXPECT_EQ(out, 0.20783890783786773682f);
    constexpr int COUNT_X = 2;
    constexpr int COUNT_Y = 4;
    constexpr int COUNT_BANDS = 3;
    float values[COUNT_Y * COUNT_X * COUNT_BANDS];
    const int bands[] = {0, 1, 2};
    constexpr int OFFSET_X = 2;
    constexpr int OFFSET_Y = 1;
    EXPECT_TRUE(grid->valuesAt(OFFSET_X, OFFSET_Y, COUNT_X, COUNT_Y,
                               COUNT_BANDS, bands, values));
    int valuesIdx = 0;
    for (int y = 0; y < COUNT_Y; ++y) {
        for (int x = 0; x < COUNT_X; ++x) {
            for (int band = 0; band < COUNT_BANDS; ++band) {
                EXPECT_TRUE(
                    grid->valueAt(OFFSET_X + x, OFFSET_Y + y, band, out));
                EXPECT_EQ(out, values[valuesIdx]);
                ++valuesIdx;
            }
        }
    }
    EXPECT_EQ(grid->metadataItem("area_of_use"), "Nordic and Baltic countries");
    EXPECT_EQ(grid->metadataItem("non_existing"), std::string());
    EXPECT_EQ(grid->metadataItem("non_existing", 1), std::string());
    EXPECT_EQ(grid->metadataItem("non_existing", 10), std::string());
    gridSet->reassign_context(m_ctxt2);
    gridSet->reopen(m_ctxt2);
    grid = gridSet->gridAt(21.3333333 / 180 * M_PI, 63.0 / 180 * M_PI);
    EXPECT_NE(grid, nullptr);
}

// ---------------------------------------------------------------------------

TEST_F(GridTest, GenericShiftGridSet_gtiff_valuesAt_tiled_optim) {
    auto gridSet = NS_PROJ::GenericShiftGridSet::open(
        m_ctxt, "tests/nkgrf03vel_realigned_extract_tiled_256x256.tif");
    ASSERT_NE(gridSet, nullptr);
    ASSERT_EQ(gridSet->gridAt(-100, -100), nullptr);
    auto grid = gridSet->gridAt(21.3333333 / 180 * M_PI, 63.0 / 180 * M_PI);
    ASSERT_NE(grid, nullptr);
    EXPECT_EQ(grid->width(), 5);
    EXPECT_EQ(grid->height(), 5);
    EXPECT_EQ(grid->extentAndRes().isGeographic, true);
    EXPECT_EQ(grid->extentAndRes().west, 21.0 / 180 * M_PI);
    EXPECT_FALSE(grid->isNullGrid());
    EXPECT_FALSE(grid->hasChanged());
    float out = -1.0f;
    EXPECT_FALSE(grid->valueAt(0, 0, grid->samplesPerPixel(), out));
    EXPECT_TRUE(grid->valueAt(0, 0, 0, out));
    EXPECT_EQ(out, 0.20783890783786773682f);
    EXPECT_TRUE(grid->valueAt(1, 0, 0, out));
    EXPECT_EQ(out, 0.22427035868167877197);
    EXPECT_TRUE(grid->valueAt(0, 1, 0, out));
    EXPECT_EQ(out, 0.19718019664287567139f);
    int bandZero = 0;
    EXPECT_TRUE(grid->valuesAt(0, 0, 1, 1, 1, &bandZero, &out));
    EXPECT_EQ(out, 0.20783890783786773682f);
    constexpr int COUNT_X = 2;
    constexpr int COUNT_Y = 4;
    constexpr int COUNT_BANDS_THREE = 3;
    float values[COUNT_Y * COUNT_X * COUNT_BANDS_THREE];
    const int bands[] = {0, 1, 2};
    constexpr int OFFSET_X = 2;
    constexpr int OFFSET_Y = 1;
    EXPECT_TRUE(grid->valuesAt(OFFSET_X, OFFSET_Y, COUNT_X, COUNT_Y,
                               COUNT_BANDS_THREE, bands, values));
    for (int y = 0, valuesIdx = 0; y < COUNT_Y; ++y) {
        for (int x = 0; x < COUNT_X; ++x) {
            for (int band = 0; band < COUNT_BANDS_THREE; ++band) {
                EXPECT_TRUE(
                    grid->valueAt(OFFSET_X + x, OFFSET_Y + y, band, out));
                EXPECT_EQ(out, values[valuesIdx]);
                ++valuesIdx;
            }
        }
    }

    constexpr int COUNT_BANDS_TWO = 2;
    EXPECT_TRUE(grid->valuesAt(OFFSET_X, OFFSET_Y, COUNT_X, COUNT_Y,
                               COUNT_BANDS_TWO, bands, values));
    for (int y = 0, valuesIdx = 0; y < COUNT_Y; ++y) {
        for (int x = 0; x < COUNT_X; ++x) {
            for (int band = 0; band < COUNT_BANDS_TWO; ++band) {
                EXPECT_TRUE(
                    grid->valueAt(OFFSET_X + x, OFFSET_Y + y, band, out));
                EXPECT_EQ(out, values[valuesIdx]);
                ++valuesIdx;
            }
        }
    }
}

// ---------------------------------------------------------------------------

TEST_F(GridTest, GenericShiftGridSet_gtiff_with_subgrid) {
    auto gridSet = NS_PROJ::GenericShiftGridSet::open(
        m_ctxt, "tests/test_hgrid_with_subgrid.tif");
    ASSERT_NE(gridSet, nullptr);
    ASSERT_EQ(gridSet->gridAt(-100, -100), nullptr);
    auto grid =
        gridSet->gridAt(-115.5416667 / 180 * M_PI, 51.1666667 / 180 * M_PI);
    ASSERT_NE(grid, nullptr);
    EXPECT_EQ(grid->width(), 11);
    EXPECT_EQ(grid->height(), 21);
    EXPECT_EQ(grid->metadataItem("grid_name"), "ALbanff");
}

// ---------------------------------------------------------------------------

TEST_F(GridTest,
       GenericShiftGridSet_gtiff_with_two_level_of_subgrids_no_grid_name) {
    auto gridSet = NS_PROJ::GenericShiftGridSet::open(
        m_ctxt, "tests/test_hgrid_with_two_level_of_subgrids_no_grid_name.tif");
    ASSERT_NE(gridSet, nullptr);
    ASSERT_EQ(gridSet->gridAt(-100, -100), nullptr);
    auto grid = gridSet->gridAt(-45.5 / 180 * M_PI, 22.5 / 180 * M_PI);
    ASSERT_NE(grid, nullptr);
    EXPECT_EQ(grid->width(), 8);
    EXPECT_EQ(grid->height(), 8);
}

// ---------------------------------------------------------------------------

TEST_F(GridTest, GenericShiftGridSet_gtiff_projected) {
    auto gridSet = NS_PROJ::GenericShiftGridSet::open(
        m_ctxt, "tests/test_3d_grid_projected.tif");
    ASSERT_NE(gridSet, nullptr);
    ASSERT_EQ(gridSet->gridAt(-1000, -1000), nullptr);
    auto grid = gridSet->gridAt(1500300.0, 5400300.0);
    ASSERT_NE(grid, nullptr);
    EXPECT_EQ(grid->width(), 2);
    EXPECT_EQ(grid->height(), 2);
    EXPECT_EQ(grid->extentAndRes().isGeographic, false);
    EXPECT_EQ(grid->extentAndRes().west, 1500000.0);
    EXPECT_EQ(grid->extentAndRes().east, 1501000.0);
    EXPECT_EQ(grid->extentAndRes().south, 5400000.0);
    EXPECT_EQ(grid->extentAndRes().north, 5401000.0);
    EXPECT_EQ(grid->extentAndRes().resX, 1000);
    EXPECT_EQ(grid->extentAndRes().resY, 1000);
}

} // namespace
