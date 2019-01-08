/******************************************************************************
 *
 * Project:  PROJ
 * Purpose:  Test that in- and output units behave correctly, especially
 *           angular units that need special treatment.
 * Author:   Kristian Evers <kristianevers@gmail.com>
 *
 ******************************************************************************
 * Copyright (c) 2019, Kristian Evers
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

#include "proj.h"

#include "gtest_include.h"

namespace {

TEST(AngularUnits, Basic) {
    auto ctx = proj_context_create();
    auto P = proj_create(ctx, "proj=latlong");

    EXPECT_TRUE(proj_angular_input(P, PJ_FWD));
    EXPECT_TRUE(proj_angular_output(P, PJ_FWD));
    EXPECT_TRUE(proj_angular_input(P, PJ_INV));
    EXPECT_TRUE(proj_angular_output(P, PJ_INV));

    proj_destroy(P);
    proj_context_destroy(ctx);
}

TEST(AngularUnits, Pipelines) {
    auto ctx = proj_context_create();
    auto P =
        proj_create(ctx, "proj=pipeline +step +proj=axisswap +order=2,1 +step "
                         "+proj=latlong +step +proj=axisswap +order=2,1");

    EXPECT_TRUE(proj_angular_input(P, PJ_FWD));
    EXPECT_TRUE(proj_angular_output(P, PJ_FWD));
    EXPECT_TRUE(proj_angular_input(P, PJ_INV));
    EXPECT_TRUE(proj_angular_output(P, PJ_INV));

    proj_destroy(P);
    proj_context_destroy(ctx);
}

TEST(AngularUnits, Pipelines2) {
    auto ctx = proj_context_create();
    auto P = proj_create(
        ctx,
        "+proj=pipeline "
        "+step +proj=axisswap +order=2,1 "
        "+step +proj=unitconvert +xy_in=deg +xy_out=rad "
        "+step +proj=tmerc +lat_0=0 +lon_0=-81 +k=0.9996 +x_0=500000.001016002 "
        "+y_0=0 +ellps=WGS84 "
        "+step +proj=axisswap +order=2,1 "
        "+step +proj=unitconvert +xy_in=m +z_in=m +xy_out=us-ft +z_out=us-ft");

    EXPECT_FALSE(proj_angular_input(P, PJ_FWD));
    EXPECT_FALSE(proj_angular_output(P, PJ_FWD));

    proj_destroy(P);
    proj_context_destroy(ctx);
}

TEST(AngularUnits, Pipelines3) {
    auto ctx = proj_context_create();
    auto P = proj_create(
        ctx,
        "+proj=pipeline "
        "+step +proj=axisswap +order=2,1 "
        "+step +proj=tmerc +lat_0=0 +lon_0=-81 +k=0.9996 +x_0=500000.001016002 "
        "+y_0=0 +ellps=WGS84 "
        "+step +proj=axisswap +order=2,1 "
        "+step +proj=unitconvert +xy_in=m +z_in=m +xy_out=us-ft +z_out=us-ft");

    EXPECT_TRUE(proj_angular_input(P, PJ_FWD));
    EXPECT_FALSE(proj_angular_output(P, PJ_FWD));

    proj_destroy(P);
    proj_context_destroy(ctx);
}

} // namespace
