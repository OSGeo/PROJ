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
    PJ_CONTEXT *ctx = pj_get_default_ctx();

    // Expectation is that only sane values of e (and nan is here reckoned to
    // be sane) are passed to pj_phi2.  Thus the return value with other values
    // of e is "implementation dependent".

    constexpr auto inf = std::numeric_limits<double>::infinity();
    constexpr auto nan = std::numeric_limits<double>::quiet_NaN();

    // Strict equality is demanded here.
    EXPECT_EQ(M_PI_2, pj_phi2(ctx, +0.0, 0.0));
    EXPECT_EQ(0.0, pj_phi2(ctx, 1.0, 0.0));
    EXPECT_EQ(-M_PI_2, pj_phi2(ctx, inf, 0.0));
    // We don't expect pj_phi2 to be called with negative ts (since ts =
    // exp(-psi)).  However, in the current implementation it is odd in ts.
    // N.B. ts = +0.0 and ts = -0.0 return different results.
    EXPECT_EQ(-M_PI_2, pj_phi2(ctx, -0.0, 0.0));
    EXPECT_EQ(0.0, pj_phi2(ctx, -1.0, 0.0));
    EXPECT_EQ(+M_PI_2, pj_phi2(ctx, -inf, 0.0));

    constexpr double e = 0.2;
    EXPECT_EQ(M_PI_2, pj_phi2(ctx, +0.0, e));
    EXPECT_EQ(0.0, pj_phi2(ctx, 1.0, e));
    EXPECT_EQ(-M_PI_2, pj_phi2(ctx, inf, e));
    EXPECT_EQ(-M_PI_2, pj_phi2(ctx, -0.0, e));
    EXPECT_EQ(0.0, pj_phi2(ctx, -1.0, e));
    EXPECT_EQ(+M_PI_2, pj_phi2(ctx, -inf, e));

    EXPECT_TRUE(std::isnan(pj_phi2(ctx, nan, 0.0)));
    EXPECT_TRUE(std::isnan(pj_phi2(ctx, nan, e)));
    EXPECT_TRUE(std::isnan(pj_phi2(ctx, +0.0, nan)));
    EXPECT_TRUE(std::isnan(pj_phi2(ctx, 1.0, nan)));
    EXPECT_TRUE(std::isnan(pj_phi2(ctx, inf, nan)));
    EXPECT_TRUE(std::isnan(pj_phi2(ctx, -0.0, nan)));
    EXPECT_TRUE(std::isnan(pj_phi2(ctx, -1.0, nan)));
    EXPECT_TRUE(std::isnan(pj_phi2(ctx, -inf, nan)));
    EXPECT_TRUE(std::isnan(pj_phi2(ctx, nan, nan)));

    EXPECT_DOUBLE_EQ(M_PI / 3, pj_phi2(ctx, 1 / (sqrt(3.0) + 2), 0.0));
    EXPECT_DOUBLE_EQ(M_PI / 4, pj_phi2(ctx, 1 / (sqrt(2.0) + 1), 0.0));
    EXPECT_DOUBLE_EQ(M_PI / 6, pj_phi2(ctx, 1 / sqrt(3.0), 0.0));
    EXPECT_DOUBLE_EQ(-M_PI / 3, pj_phi2(ctx, sqrt(3.0) + 2, 0.0));
    EXPECT_DOUBLE_EQ(-M_PI / 4, pj_phi2(ctx, sqrt(2.0) + 1, 0.0));
    EXPECT_DOUBLE_EQ(-M_PI / 6, pj_phi2(ctx, sqrt(3.0), 0.0));

    // Generated with exp(e * atanh(e * sin(phi))) / (tan(phi) + sec(phi))
    EXPECT_DOUBLE_EQ(M_PI / 3, pj_phi2(ctx, 0.27749174377027023413, e));
    EXPECT_DOUBLE_EQ(M_PI / 4, pj_phi2(ctx, 0.42617788119104192995, e));
    EXPECT_DOUBLE_EQ(M_PI / 6, pj_phi2(ctx, 0.58905302448626726064, e));
    EXPECT_DOUBLE_EQ(-M_PI / 3, pj_phi2(ctx, 3.6037108218537833089, e));
    EXPECT_DOUBLE_EQ(-M_PI / 4, pj_phi2(ctx, 2.3464380582241712935, e));
    EXPECT_DOUBLE_EQ(-M_PI / 6, pj_phi2(ctx, 1.6976400399134411849, e));
}

} // namespace
