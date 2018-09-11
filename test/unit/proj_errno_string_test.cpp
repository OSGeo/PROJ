/******************************************************************************
 *
 * Project:  PROJ
 * Purpose:  Unit test for proj_errno_string()
 * Author:   Kristian Evers <kristianevers@gmail.com>
 *
 ******************************************************************************
 * Copyright (c) 2018, Kristian Evers.
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

#include <cstring>

#include "proj.h"

#include "gtest_include.h"

namespace {

TEST(ProjErrnoStringTest, NoError) {
    EXPECT_EQ(0, proj_errno_string(0));
}

TEST(ProjErrnoStringTest, ProjErrnos) {
    EXPECT_STREQ("no arguments in initialization list", proj_errno_string(-1));
    EXPECT_STREQ("invalid projection system error (-1000)", proj_errno_string(-1000));
    EXPECT_STREQ("invalid projection system error (-9999)", proj_errno_string(-9999));
    // for errnos < -9999, -9999 is always returned
    const int min = std::numeric_limits<int>::min();
    EXPECT_STREQ("invalid projection system error (-9999)",proj_errno_string(min));
    EXPECT_STREQ("invalid projection system error (-9999)", proj_errno_string(-10000));
}

TEST(ProjErrnoStringTest, SystemErrnos) {
    const int max = std::numeric_limits<int>::max();

#ifdef HAVE_STRERROR
    EXPECT_STREQ(strerror(5), proj_errno_string(5));
    EXPECT_STREQ(strerror(9999), proj_errno_string(9999));
    EXPECT_STREQ(strerror(10000), proj_errno_string(10000));
    EXPECT_STREQ(strerror(max), proj_errno_string(max));
#else
    EXPECT_STREQ("no system list, errno: 5\n", proj_errno_string(5));
    EXPECT_STREQ("no system list, errno: 9999\n", proj_errno_string(9999));
    // for errnos > 9999, 9999 is always returned
    EXPECT_STREQ("no system list, errno: 9999\n", proj_errno_string(10000));
    EXPECT_STREQ("no system list, errno: 9999\n", proj_errno_string(max));
#endif
}

}  // namespace
