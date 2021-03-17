/******************************************************************************
 *
 * Project:  PROJ
 * Purpose:  Test
 * Author:   Even Rouault <even dot rouault at spatialys dot com>
 *
 ******************************************************************************
 * Copyright (c) 2021, Even Rouault <even dot rouault at spatialys dot com>
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

#include "proj.h"

namespace {

TEST(misc, version) {
    EXPECT_EQ(PROJ_COMPUTE_VERSION(2200, 98, 76), 22009876);
    EXPECT_EQ(PROJ_VERSION_NUMBER, PROJ_VERSION_MAJOR * 10000 +
                                       PROJ_VERSION_MINOR * 100 +
                                       PROJ_VERSION_PATCH);
    EXPECT_TRUE(PROJ_AT_LEAST_VERSION(PROJ_VERSION_MAJOR, PROJ_VERSION_MINOR,
                                      PROJ_VERSION_PATCH));
    EXPECT_TRUE(PROJ_AT_LEAST_VERSION(PROJ_VERSION_MAJOR - 1,
                                      PROJ_VERSION_MINOR, PROJ_VERSION_PATCH));
    EXPECT_FALSE(PROJ_AT_LEAST_VERSION(PROJ_VERSION_MAJOR, PROJ_VERSION_MINOR,
                                       PROJ_VERSION_PATCH + 1));
}

} // namespace
