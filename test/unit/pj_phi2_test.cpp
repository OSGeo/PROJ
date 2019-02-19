/******************************************************************************
 *
 * Project:  PROJ
 * Purpose:  Test pj_phi2 function.
 * Author:   Kurt Schwehr <schwehr@google.com>
 *
 ******************************************************************************
 * Copyright (c) 2018, Google Inc.
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
#include "proj_internal.h"

#include <cmath>
#include <limits>

#include "gtest_include.h"

namespace {

TEST(PjPhi2Test, Basic) {
    projCtx ctx = pj_get_default_ctx();

    EXPECT_DOUBLE_EQ(M_PI_2, pj_phi2(ctx, 0.0, 0.0));

    EXPECT_NEAR(0.0, pj_phi2(ctx, 1.0, 0.0), 1e-16);
    EXPECT_DOUBLE_EQ(M_PI_2, pj_phi2(ctx, 0.0, 1.0));
    EXPECT_DOUBLE_EQ(M_PI, pj_phi2(ctx, -1.0, 0.0));
    EXPECT_DOUBLE_EQ(M_PI_2, pj_phi2(ctx, 0.0, -1.0));

    EXPECT_NEAR(0.0, pj_phi2(ctx, 1.0, 1.0), 1e-16);
    EXPECT_DOUBLE_EQ(M_PI, pj_phi2(ctx, -1.0, -1.0));

    // TODO(schwehr): M_PI_4, M_PI_2, M_PI, M_E
    // https://www.gnu.org/software/libc/manual/html_node/Mathematical-Constants.html

    EXPECT_DOUBLE_EQ(-0.95445818456292697, pj_phi2(ctx, M_PI, 0.0));
    EXPECT_TRUE(std::isnan(pj_phi2(ctx, 0.0, M_PI)));
    EXPECT_DOUBLE_EQ(4.0960508381527205, pj_phi2(ctx, -M_PI, 0.0));
    EXPECT_TRUE(std::isnan(pj_phi2(ctx, 0.0, -M_PI)));

    EXPECT_TRUE(std::isnan(pj_phi2(ctx, M_PI, M_PI)));
    EXPECT_TRUE(std::isnan(pj_phi2(ctx, -M_PI, -M_PI)));
}

TEST(PjPhi2Test, AvoidUndefinedBehavior) {
    auto ctx = pj_get_default_ctx();

    const auto nan = std::numeric_limits<double>::quiet_NaN();
    EXPECT_TRUE(std::isnan(pj_phi2(ctx, nan, 0.0)));
    EXPECT_TRUE(std::isnan(pj_phi2(ctx, 0.0, nan)));
    EXPECT_TRUE(std::isnan(pj_phi2(ctx, nan, nan)));

    // We do not really care about the values that follow.
    const auto inf = std::numeric_limits<double>::infinity();

    EXPECT_DOUBLE_EQ(-M_PI_2, pj_phi2(ctx, inf, 0.0));
    EXPECT_TRUE(std::isnan(pj_phi2(ctx, 0.0, inf)));

    EXPECT_DOUBLE_EQ(4.7123889803846897, pj_phi2(ctx, -inf, 0.0));
    EXPECT_TRUE(std::isnan(pj_phi2(ctx, 0.0, -inf)));

    EXPECT_TRUE(std::isnan(pj_phi2(ctx, inf, inf)));
    EXPECT_TRUE(std::isnan(pj_phi2(ctx, -inf, -inf)));
}

} // namespace
