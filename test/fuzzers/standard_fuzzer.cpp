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
#include "proj_internal.h"

/* Standalone build:
g++ -g -std=c++11 standard_fuzzer.cpp -o standard_fuzzer -fvisibility=hidden -DSTANDALONE ../../src/.libs/libproj.a -lpthread -lsqlite3 -I../../src -I../../include
*/

extern "C" int LLVMFuzzerInitialize(int* argc, char*** argv);
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *buf, size_t len);

int LLVMFuzzerInitialize(int* /*argc*/, char*** argv)
{
    const char* argv0 = (*argv)[0];
    char* path = pj_strdup(argv0);
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

    /* We expect the blob to be 3 lines: */
    /* source proj string\ndestination proj string\nx y */
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
    char* second_newline = strchr(second_line, '\n');
    if( !second_newline )
    {
        free(buf_dup);
        return 0;
    }
    second_newline[0] = 0;
    char* third_line = second_newline + 1;
    PJ *pj_src = pj_init_plus(first_line);
    if( !pj_src )
    {
        free(buf_dup);
        return 0;
    }
    PJ *pj_dst = pj_init_plus(second_line);
    if( !pj_dst )
    {
        free(buf_dup);
        pj_free(pj_src);
        proj_cleanup();
        return 0;
    }
    double x = 0, y = 0, z = 9;
    bool from_binary = false;
    bool has_z = false;
    if( strncmp(third_line, "BINARY_2D:", strlen("BINARY_2D:")) == 0 &&
        third_line - first_line + strlen("BINARY_2D:") + 2 * sizeof(double) <= len )
    {
        from_binary = true;
        memcpy(&x, third_line + strlen("BINARY_2D:"), sizeof(double));
        memcpy(&y, third_line + strlen("BINARY_2D:") + sizeof(double), sizeof(double));
    }
    else if( strncmp(third_line, "BINARY_3D:", strlen("BINARY_3D:")) == 0 &&
             third_line - first_line + strlen("BINARY_3D:") + 3 * sizeof(double) <= len )
    {
        from_binary = true;
        has_z = true;
        memcpy(&x, third_line + strlen("BINARY_3D:"), sizeof(double));
        memcpy(&y, third_line + strlen("BINARY_3D:") + sizeof(double), sizeof(double));
        memcpy(&z, third_line + strlen("BINARY_3D:") + 2 * sizeof(double), sizeof(double));
    }
    else if( sscanf(third_line, "%lf %lf", &x, &y) != 2 )
    {
        free(buf_dup);
        pj_free(pj_src);
        pj_free(pj_dst);
        proj_cleanup();
        return 0;
    }
#ifdef STANDALONE
    fprintf(stderr, "src=%s\n", first_line);
    fprintf(stderr, "dst=%s\n", second_line);
    if( from_binary )
    {
        if( has_z )
            fprintf(stderr, "coord (from binary)=%.18g %.18g %.18g\n", x, y, z);
        else
            fprintf(stderr, "coord (from binary)=%.18g %.18g\n", x, y);
    }
    else
        fprintf(stderr, "coord=%s\n", third_line);
#endif
    if( has_z )
        pj_transform( pj_src, pj_dst, 1, 0, &x, &y, &z );
    else
        pj_transform( pj_src, pj_dst, 1, 0, &x, &y, NULL );
    free(buf_dup);
    pj_free(pj_src);
    pj_free(pj_dst);
    proj_cleanup();
    return 0;
}

#ifdef STANDALONE

int main(int argc, char* argv[])
{
    if( argc < 2 )
    {
        const char str[] =
            "+proj=longlat +datum=WGS84 +nodefs\n+proj=longlat +datum=WGS84 +nodefs\n2 49";
        int ret = LLVMFuzzerTestOneInput((const uint8_t*)(str), sizeof(str) - 1);
        if( ret )
            return ret;

        const char str2[] =
            "+proj=longlat +datum=WGS84 +nodefs\n+proj=longlat +datum=WGS84 +nodefs\nBINARY_2D:\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xFF\xFF";
        ret = LLVMFuzzerTestOneInput((const uint8_t*)(str2), sizeof(str2) - 1);
        if( ret )
            return ret;

        const char str3[] =
            "+proj=longlat +datum=WGS84 +nodefs\n+proj=longlat +datum=WGS84 +nodefs\nBINARY_3D:\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xFF\xFF\x00\x00\x00\x00\x00\x00\x00\x00";
        ret = LLVMFuzzerTestOneInput((const uint8_t*)(str3), sizeof(str3) - 1);
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
