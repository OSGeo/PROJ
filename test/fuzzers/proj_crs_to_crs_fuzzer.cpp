/******************************************************************************
 *
 * Project:  proj.4
 * Purpose:  Fuzzer
 * Author:   Even Rouault, even.rouault at spatialys.com
 *
 ******************************************************************************
 * Copyright (c) 2017, Even Rouault <even.rouault at spatialys.com>
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

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

#include "proj.h"

/* Standalone build:
g++ -g -std=c++11 proj_crs_to_crs_fuzzer.cpp -o proj_crs_to_crs_fuzzer -fvisibility=hidden -DSTANDALONE ../../src/.libs/libproj.a -lpthread -lsqlite3 -I../../src -I../../include
*/

extern "C" int LLVMFuzzerInitialize(int* argc, char*** argv);
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *buf, size_t len);

int LLVMFuzzerInitialize(int* /*argc*/, char*** argv)
{
    const char* argv0 = (*argv)[0];
    char* path = strdup(argv0);
    char* lastslash = strrchr(path, '/');
    if( lastslash )
    {
        *lastslash = '\0';
        setenv("PROJ_LIB", path, 1);
    }
    else
    {
        setenv("PROJ_LIB", ".", 1);
    }
    free(path);
    return 0;
}

int LLVMFuzzerTestOneInput(const uint8_t *buf, size_t len)
{
    if( len > 1000 )
    {
#ifdef STANDALONE
        fprintf(stderr, "Input too large\n");
#endif
        return 0;
    }

    /* We expect the blob to be 2 lines: */
    /* source_string\ndestination_string */
    char* buf_dup = (char*)malloc(len+1);
    memcpy(buf_dup, buf, len);
    buf_dup[len] = 0;
    char* first_line = buf_dup;
    char* first_newline = strchr(first_line, '\n');
    if( !first_newline )
    {
        free(buf_dup);
        return 0;
    }
    first_newline[0] = 0;
    char* second_line = first_newline + 1;

#ifdef STANDALONE
    fprintf(stderr, "src=%s\n", first_line);
    fprintf(stderr, "dst=%s\n", second_line);
#endif

    proj_destroy(
        proj_create_crs_to_crs(nullptr, first_line, second_line, nullptr));

    free(buf_dup);
    proj_cleanup();
    return 0;
}

#ifdef STANDALONE

int main(int argc, char* argv[])
{
    if( argc < 2 )
    {
        const char str[] =
            "+proj=longlat +datum=WGS84 +nodefs\n+proj=longlat +datum=WGS84 +nodefs";
        int ret = LLVMFuzzerTestOneInput((const uint8_t*)(str), sizeof(str) - 1);
        if( ret )
            return ret;

        return 0;
    }
    else
    {
        int nRet = 0;
        void* buf = NULL;
        int nLen = 0;
        FILE* f = fopen(argv[1], "rb");
        if( !f )
        {
            fprintf(stderr, "%s does not exist.\n", argv[1]);
            exit(1);
        }
        fseek(f, 0, SEEK_END);
        nLen = (int)ftell(f);
        fseek(f, 0, SEEK_SET);
        buf = malloc(nLen);
        if( !buf )
        {
            fprintf(stderr, "malloc failed.\n");
            fclose(f);
            exit(1);
        }
        fread(buf, nLen, 1, f);
        fclose(f);
        nRet = LLVMFuzzerTestOneInput((const uint8_t*)(buf), nLen);
        free(buf);
        return nRet;
    }
}

#endif // STANDALONE
