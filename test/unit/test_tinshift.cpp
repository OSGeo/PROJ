/******************************************************************************
 *
 * Project:  PROJ
 * Purpose:  Test TIN shift
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

#define PROJ_COMPILATION
#define TINSHIFT_NAMESPACE TestTINShift
#include "transformations/tinshift.hpp"

using namespace TINSHIFT_NAMESPACE;

namespace {

static json getMinValidContent() {
    json j;
    j["file_type"] = "triangulation_file";
    j["format_version"] = "1.0";
    j["input_crs"] = "EPSG:2393";
    j["output_crs"] = "EPSG:3067";
    j["transformed_components"] = {"horizontal"};
    j["vertices_columns"] = {"source_x", "source_y", "target_x", "target_y"};
    j["triangles_columns"] = {"idx_vertex1", "idx_vertex2", "idx_vertex3"};
    j["vertices"] = {{0, 0, 101, 101}, {0, 1, 100, 101}, {1, 1, 100, 100}};
    j["triangles"] = {{0, 1, 2}};

    return j;
}

// ---------------------------------------------------------------------------

TEST(tinshift, basic) {
    EXPECT_THROW(TINShiftFile::parse("foo"), ParsingException);
    EXPECT_THROW(TINShiftFile::parse("null"), ParsingException);
    EXPECT_THROW(TINShiftFile::parse("{}"), ParsingException);

    const auto jMinValid(getMinValidContent());
    {
        auto f = TINShiftFile::parse(jMinValid.dump());
        EXPECT_EQ(f->fileType(), "triangulation_file");
        EXPECT_EQ(f->formatVersion(), "1.0");
        EXPECT_EQ(f->inputCRS(), "EPSG:2393");
        EXPECT_EQ(f->outputCRS(), "EPSG:3067");

        auto eval = Evaluator(std::move(f));
        double x_out = 0;
        double y_out = 0;
        double z_out = 0;

        EXPECT_FALSE(eval.forward(-0.1, 0.0, 1000.0, x_out, y_out, z_out));

        EXPECT_TRUE(eval.forward(0.0, 0.0, 1000.0, x_out, y_out, z_out));
        EXPECT_EQ(x_out, 101.0);
        EXPECT_EQ(y_out, 101.0);
        EXPECT_EQ(z_out, 1000.0);

        EXPECT_TRUE(eval.forward(0.0, 1.0, 1000.0, x_out, y_out, z_out));
        EXPECT_EQ(x_out, 100.0);
        EXPECT_EQ(y_out, 101.0);
        EXPECT_EQ(z_out, 1000.0);

        EXPECT_TRUE(eval.forward(1.0, 1.0, 1000.0, x_out, y_out, z_out));
        EXPECT_EQ(x_out, 100.0);
        EXPECT_EQ(y_out, 100.0);
        EXPECT_EQ(z_out, 1000.0);

        EXPECT_TRUE(eval.forward(0.0, 0.5, 1000.0, x_out, y_out, z_out));
        EXPECT_EQ(x_out, 100.5);
        EXPECT_EQ(y_out, 101.0);
        EXPECT_EQ(z_out, 1000.0);

        EXPECT_TRUE(eval.forward(0.5, 0.5, 1000.0, x_out, y_out, z_out));
        EXPECT_EQ(x_out, 100.5);
        EXPECT_EQ(y_out, 100.5);
        EXPECT_EQ(z_out, 1000.0);

        EXPECT_TRUE(eval.forward(0.5, 0.75, 1000.0, x_out, y_out, z_out));
        EXPECT_EQ(x_out, 100.25);
        EXPECT_EQ(y_out, 100.5);
        EXPECT_EQ(z_out, 1000.0);

        EXPECT_TRUE(eval.inverse(100.25, 100.5, 1000.0, x_out, y_out, z_out));
        EXPECT_EQ(x_out, 0.5);
        EXPECT_EQ(y_out, 0.75);
        EXPECT_EQ(z_out, 1000.0);
    }

    // Vertical only, through source_z / target_z
    {
        auto j(jMinValid);
        j["transformed_components"] = {"vertical"};
        j["vertices_columns"] = {"source_x", "source_y", "source_z",
                                 "target_z"};
        j["triangles_columns"] = {"idx_vertex1", "idx_vertex2", "idx_vertex3"};
        j["vertices"] = {
            {0, 0, 10.5, 10.6}, {0, 1, 15.0, 15.2}, {1, 1, 17.5, 18.0}};
        j["triangles"] = {{0, 1, 2}};

        auto f = TINShiftFile::parse(j.dump());
        auto eval = Evaluator(std::move(f));
        double x_out = 0;
        double y_out = 0;
        double z_out = 0;

        EXPECT_TRUE(eval.forward(0.0, 0.0, 1000.0, x_out, y_out, z_out));
        EXPECT_EQ(x_out, 0.0);
        EXPECT_EQ(y_out, 0.0);
        EXPECT_EQ(z_out, 1000.1);

        EXPECT_TRUE(eval.forward(0.5, 0.75, 1000.0, x_out, y_out, z_out));
        EXPECT_EQ(x_out, 0.5);
        EXPECT_EQ(y_out, 0.75);
        EXPECT_EQ(z_out, 1000.325);

        EXPECT_TRUE(eval.inverse(0.5, 0.75, 1000.325, x_out, y_out, z_out));
        EXPECT_EQ(x_out, 0.5);
        EXPECT_EQ(y_out, 0.75);
        EXPECT_EQ(z_out, 1000.0);
    }

    // Vertical only, through offset_z
    {
        auto j(jMinValid);
        j["transformed_components"] = {"vertical"};
        j["vertices_columns"] = {"source_x", "source_y", "offset_z"};
        j["triangles_columns"] = {"idx_vertex1", "idx_vertex2", "idx_vertex3"};
        j["vertices"] = {{0, 0, 0.1}, {0, 1, 0.2}, {1, 1, 0.5}};
        j["triangles"] = {{0, 1, 2}};

        auto f = TINShiftFile::parse(j.dump());
        auto eval = Evaluator(std::move(f));
        double x_out = 0;
        double y_out = 0;
        double z_out = 0;

        EXPECT_TRUE(eval.forward(0.0, 0.0, 1000.0, x_out, y_out, z_out));
        EXPECT_EQ(x_out, 0.0);
        EXPECT_EQ(y_out, 0.0);
        EXPECT_EQ(z_out, 1000.1);

        EXPECT_TRUE(eval.forward(0.5, 0.75, 1000.0, x_out, y_out, z_out));
        EXPECT_EQ(x_out, 0.5);
        EXPECT_EQ(y_out, 0.75);
        EXPECT_EQ(z_out, 1000.325);

        EXPECT_TRUE(eval.inverse(0.5, 0.75, 1000.325, x_out, y_out, z_out));
        EXPECT_EQ(x_out, 0.5);
        EXPECT_EQ(y_out, 0.75);
        EXPECT_EQ(z_out, 1000.0);
    }

    // Horizontal + vertical
    {
        auto j(jMinValid);
        j["transformed_components"] = {"horizontal", "vertical"};
        j["vertices_columns"] = {"source_x", "source_y", "target_x", "target_y",
                                 "offset_z"};
        j["triangles_columns"] = {"idx_vertex1", "idx_vertex2", "idx_vertex3"};
        j["vertices"] = {{0, 0, 101, 101, 0.1},
                         {0, 1, 100, 101, 0.2},
                         {1, 1, 100, 100, 0.5}};
        j["triangles"] = {{0, 1, 2}};

        auto f = TINShiftFile::parse(j.dump());
        auto eval = Evaluator(std::move(f));
        double x_out = 0;
        double y_out = 0;
        double z_out = 0;

        EXPECT_TRUE(eval.forward(0.0, 0.0, 1000.0, x_out, y_out, z_out));
        EXPECT_EQ(x_out, 101.0);
        EXPECT_EQ(y_out, 101.0);
        EXPECT_EQ(z_out, 1000.1);

        EXPECT_TRUE(eval.forward(0.5, 0.75, 1000.0, x_out, y_out, z_out));
        EXPECT_EQ(x_out, 100.25);
        EXPECT_EQ(y_out, 100.5);
        EXPECT_EQ(z_out, 1000.325);

        EXPECT_TRUE(eval.inverse(100.25, 100.5, 1000.325, x_out, y_out, z_out));
        EXPECT_EQ(x_out, 0.5);
        EXPECT_EQ(y_out, 0.75);
        EXPECT_EQ(z_out, 1000.0);
    }
}

} // namespace
