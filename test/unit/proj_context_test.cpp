/******************************************************************************
 *
 * Project:  PROJ
 * Purpose:  Test functions in proj_context namespae
 * Author:   Even Rouault <even dot rouault at spatialys dot com>
 *
 ******************************************************************************
 * Copyright (c) 2019, Even Rouault <even dot rouault at spatialys dot com>
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

#include <stdlib.h>
#ifdef _MSC_VER
#include <io.h>
#else
#include <unistd.h>
#endif

#include "proj.h"
#include "proj_internal.h"

#include "gtest_include.h"

namespace {

static bool createTmpFile(const std::string &filename) {
    FILE *f = fopen(filename.c_str(), "wt");
    if (!f)
        return false;
    fprintf(
        f,
        "<MY_PIPELINE> +proj=pipeline +step +proj=utm +zone=31 +ellps=GRS80\n");
    fclose(f);
    return true;
}

// ---------------------------------------------------------------------------

static std::string createTempDict(std::string &dirname, const char *filename) {
    const char *temp_dir = getenv("TEMP");
    if (!temp_dir) {
        temp_dir = getenv("TMP");
    }
#ifndef WIN32
    if (!temp_dir) {
        temp_dir = "/tmp";
    }
#endif
    if (!temp_dir)
        return std::string();

    dirname = temp_dir;

    std::string tmpFilename;
    tmpFilename = temp_dir;
    tmpFilename += DIR_CHAR;
    tmpFilename += filename;

    return createTmpFile(tmpFilename) ? tmpFilename : std::string();
}

// ---------------------------------------------------------------------------

static int MyUnlink(const std::string &filename) {
#ifdef _MSC_VER
    return _unlink(filename.c_str());
#else
    return unlink(filename.c_str());
#endif
}

// ---------------------------------------------------------------------------

TEST(proj_context, proj_context_set_file_finder) {

    std::string dirname;
    auto filename = createTempDict(dirname, "temp_proj_dic1");
    if (filename.empty())
        return;

    auto ctx = proj_context_create();

    struct FinderData {
        PJ_CONTEXT *got_ctx = nullptr;
        std::string dirname{};
        std::string tmpFilename{};
    };

    const auto finder = [](PJ_CONTEXT *got_ctx, const char *file,
                           void *user_data) -> const char * {
        auto finderData = static_cast<FinderData *>(user_data);
        finderData->got_ctx = got_ctx;
        finderData->tmpFilename = finderData->dirname;
        finderData->tmpFilename += DIR_CHAR;
        finderData->tmpFilename += file;
        return finderData->tmpFilename.c_str();
    };

    FinderData finderData;
    finderData.dirname = dirname;
    proj_context_set_file_finder(ctx, finder, &finderData);

    auto P = proj_create(ctx, "+init=temp_proj_dic1:MY_PIPELINE");
    EXPECT_NE(P, nullptr);
    proj_destroy(P);

    EXPECT_EQ(finderData.got_ctx, ctx);

    proj_context_destroy(ctx);
}

// ---------------------------------------------------------------------------

TEST(proj_context, proj_context_set_search_paths) {

    std::string dirname;
    auto filename = createTempDict(dirname, "temp_proj_dic2");
    if (filename.empty())
        return;

    auto ctx = proj_context_create();

    const char *path = dirname.c_str();
    proj_context_set_search_paths(ctx, 1, &path);

    auto P = proj_create(ctx, "+init=temp_proj_dic2:MY_PIPELINE");
    EXPECT_NE(P, nullptr);
    proj_destroy(P);

    proj_context_destroy(ctx);

    MyUnlink(filename);
}

// ---------------------------------------------------------------------------

TEST(proj_context, read_grid_from_user_writable_directory) {

    auto ctx = proj_context_create();
    auto path =
        std::string(proj_context_get_user_writable_directory(ctx, true));
    EXPECT_TRUE(!path.empty());
    auto filename = path + DIR_CHAR + "temp_proj_dic3";
    EXPECT_TRUE(createTmpFile(filename));
    {
        // Check that with PROJ_SKIP_READ_USER_WRITABLE_DIRECTORY=YES (set by
        // calling script), we cannot find the file
        auto P = proj_create(ctx, "+init=temp_proj_dic3:MY_PIPELINE");
        EXPECT_EQ(P, nullptr);
        proj_destroy(P);
    }
    {
        // Cancel the effect of PROJ_SKIP_READ_USER_WRITABLE_DIRECTORY
        putenv(const_cast<char *>("PROJ_SKIP_READ_USER_WRITABLE_DIRECTORY="));
        auto P = proj_create(ctx, "+init=temp_proj_dic3:MY_PIPELINE");
        EXPECT_NE(P, nullptr);
        putenv(
            const_cast<char *>("PROJ_SKIP_READ_USER_WRITABLE_DIRECTORY=YES"));
        proj_destroy(P);
    }
    proj_context_destroy(ctx);
    MyUnlink(filename);
}

// ---------------------------------------------------------------------------

TEST(proj_context, proj_context_set_ca_bundle_path) {
    std::string dirname("/tmp/dummmy/path/cacert.pem");
    auto ctx = proj_context_create();
    proj_context_set_ca_bundle_path(ctx, dirname.c_str());
    ASSERT_EQ(ctx->ca_bundle_path, dirname);
    proj_context_destroy(ctx);
}

} // namespace
