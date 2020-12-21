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
#include "proj_config.h"

#include "gtest_include.h"

namespace {

TEST(ProjErrnoStringTest, NoError) { EXPECT_EQ(nullptr, proj_errno_string(0)); }

TEST(ProjErrnoStringTest, ProjErrnos) {
    EXPECT_STREQ("Unknown error (code -1)", proj_errno_string(-1));
    EXPECT_STREQ("Invalid PROJ string syntax",
                 proj_errno_string(PROJ_ERR_INVALID_OP_WRONG_SYNTAX));
    EXPECT_STREQ(
        "Unspecified error related to coordinate operation initialization",
        proj_errno_string(PROJ_ERR_INVALID_OP));
    EXPECT_STREQ("Unspecified error related to coordinate transformation",
                 proj_errno_string(PROJ_ERR_COORD_TRANSFM));
}

TEST(ProjErrnoStringTest, proj_context_errno_string) {
    EXPECT_STREQ("Unknown error (code -1)",
                 proj_context_errno_string(nullptr, -1));
    PJ_CONTEXT *ctx = proj_context_create();
    EXPECT_STREQ("Unknown error (code -999)",
                 proj_context_errno_string(ctx, -999));
    proj_context_destroy(ctx);
}

} // namespace
