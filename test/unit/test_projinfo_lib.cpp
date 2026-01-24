/******************************************************************************
 *
 * Project:  PROJ
 * Purpose:  Test projinfo_lib API
 * Author:   Javier Jimenez Shaw
 *
 ******************************************************************************
 * Copyright (c) 2025, Javier Jimenez Shaw
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

#include "apps/projapps_lib.h"

#include <iostream>

// ---------------------------------------------------------------------------

TEST(projinfo_lib, simple) {
    auto dump = [](PJ_PROJINFO_LOG_LEVEL level, const char *s, void *data) {
        bool *local_found = static_cast<bool *>(data);
        if (level == PJ_PROJINFO_LOG_LEVEL_INFO) {
            std::string str(s);
            if (str.find("PROJCS[\"ETRS89 / UTM zone 32N\"") !=
                std::string::npos) {
                *local_found = true;
            }
        }
    };

    const char *argv[] = {"EPSG:25832", "-o", "WKT1_GDAL"};
    constexpr int argc = sizeof(argv) / sizeof(*argv);

    bool found = false;
    int res = projinfo(nullptr, argc, (char **)argv, dump, &found);
    EXPECT_EQ(res, 0);
    EXPECT_EQ(found, true);
}

TEST(projinfo_lib, error) {
    auto dump = [](PJ_PROJINFO_LOG_LEVEL level, const char *s, void *data) {
        bool *local_found = static_cast<bool *>(data);
        if (level == PJ_PROJINFO_LOG_LEVEL_ERR) {
            std::string str(s);
            if (str.find("unrecognized format / unknown name") !=
                std::string::npos) {
                *local_found = true;
            }
        }
    };

    const char *argv[] = {"doesnotwork"};
    constexpr int argc = sizeof(argv) / sizeof(*argv);

    bool found = false;
    int res = projinfo(nullptr, argc, (char **)argv, dump, &found);
    EXPECT_EQ(res, 1);
    EXPECT_EQ(found, true);
}

TEST(projinfo_lib, warning) {
    auto dump = [](PJ_PROJINFO_LOG_LEVEL level, const char *s, void *data) {
        bool *local_found = static_cast<bool *>(data);
        if (level == PJ_PROJINFO_LOG_LEVEL_WARN) {
            std::string str(s);
            if (str.find("Error when exporting to WKT1:GDAL") !=
                std::string::npos) {
                *local_found = true;
            }
        }
    };

    const char *argv[] = {"EPSG:4937", "-o", "WKT1_GDAL"};
    constexpr int argc = sizeof(argv) / sizeof(*argv);

    bool found = false;
    int res = projinfo(nullptr, argc, (char **)argv, dump, &found);
    EXPECT_EQ(res, 0);
    EXPECT_EQ(found, true);
}

TEST(projinfo_lib, use_ctx) {
    auto dump = [](PJ_PROJINFO_LOG_LEVEL level, const char *s, void *data) {
        bool *local_found = static_cast<bool *>(data);
        if (level == PJ_PROJINFO_LOG_LEVEL_INFO) {
            std::string str(s);
            if (str.find("PROJCS[\"ETRS89 / UTM zone 3") != std::string::npos) {
                *local_found = true;
            }
        }
    };

    PJ_CONTEXT *ctx = proj_context_create();
    {
        const char *argv[] = {"EPSG:25832", "-o", "WKT1_GDAL"};
        constexpr int argc = sizeof(argv) / sizeof(*argv);

        bool found = false;
        int res = projinfo(ctx, argc, (char **)argv, dump, &found);
        EXPECT_EQ(res, 0);
        EXPECT_EQ(found, true);
    }
    {
        const char *argv[] = {"EPSG:25833", "-o", "WKT1_GDAL"};
        constexpr int argc = sizeof(argv) / sizeof(*argv);

        bool found = false;
        int res = projinfo(ctx, argc, (char **)argv, dump, &found);
        EXPECT_EQ(res, 0);
        EXPECT_EQ(found, true);
    }
    proj_context_destroy(ctx);
}

TEST(projinfo_lib, works_without_cb) {
    // Testing it doesn't crash.
    const char *argv[] = {"EPSG:25832", "-o", "WKT1_GDAL"};
    constexpr int argc = sizeof(argv) / sizeof(*argv);

    int res = projinfo(nullptr, argc, (char **)argv, nullptr, nullptr);
    EXPECT_EQ(res, 0);
}
