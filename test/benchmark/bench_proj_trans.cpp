/******************************************************************************
 * Project:  PROJ
 * Purpose:  Benchmark
 * Author:   Even Rouault, <even.rouault at spatialys.com>
 *
 ******************************************************************************
 * Copyright (c) 2022, Even Rouault, <even.rouault at spatialys.com>
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
 *****************************************************************************/

#include "proj.h"

#include <stdlib.h> // rand()

#include <chrono>
#include <cmath> // HUGE_VAL
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

static void usage() {
    printf("Usage: bench_proj_trans [(--source-crs|-s) string]\n");
    printf("                        [(--target-crs|-t) string]\n");
    printf("                        [(--pipeline|-p) string]\n");
    printf("                        [(--loops|-l) number]\n");
    printf("                        [--noise-x number] [--noise-y number]\n");
    printf("                        coord_comp_1 coord_comp_2 [coord_comp_3] "
           "[coord_comp_4]\n");
    printf("\n");
    printf("Both of --source-crs and --target_crs, or --pipeline must be "
           "specified.\n");
    printf("\n");
    printf("Example: bench_proj_trans -s EPSG:4326 -t EPSG:32631 49 2\n");
    exit(1);
}

int main(int argc, char *argv[]) {
    std::string sourceCRS;
    std::string targetCRS;
    std::string pipeline;
    int loops = 5 * 1000 * 1000;
    double coord_comp[4] = {0, 0, 0, HUGE_VAL};
    int coord_comp_counter = 0;
    double noiseX = 0;
    double noiseY = 0;
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--source-crs") == 0 ||
            strcmp(argv[i], "-s") == 0) {
            if (i + 1 >= argc)
                usage();
            sourceCRS = argv[i + 1];
            ++i;
        } else if (strcmp(argv[i], "--target-crs") == 0 ||
                   strcmp(argv[i], "-t") == 0) {
            if (i + 1 >= argc)
                usage();
            targetCRS = argv[i + 1];
            ++i;
        } else if (strcmp(argv[i], "--pipeline") == 0 ||
                   strcmp(argv[i], "-p") == 0) {
            if (i + 1 >= argc)
                usage();
            pipeline = argv[i + 1];
            ++i;
        } else if (strcmp(argv[i], "--loops") == 0 ||
                   strcmp(argv[i], "-l") == 0) {
            if (i + 1 >= argc)
                usage();
            loops = atoi(argv[i + 1]);
            ++i;
        } else if (strcmp(argv[i], "--noise-x") == 0) {
            if (i + 1 >= argc)
                usage();
            noiseX = atof(argv[i + 1]);
            ++i;
        } else if (strcmp(argv[i], "--noise-y") == 0) {
            if (i + 1 >= argc)
                usage();
            noiseY = atof(argv[i + 1]);
            ++i;
        } else if (argv[i][0] == '-' &&
                   !(argv[i][1] >= '0' && argv[i][1] <= '9')) {
            usage();
        } else if (coord_comp_counter < 4) {
            coord_comp[coord_comp_counter++] = atof(argv[i]);
        } else {
            usage();
        }
    }
    if (coord_comp_counter < 2)
        usage();

    PJ_CONTEXT *ctxt = proj_context_create();
    PJ *P = nullptr;
    if (!pipeline.empty()) {
        P = proj_create(ctxt, pipeline.c_str());
    } else if (!sourceCRS.empty() && !targetCRS.empty()) {
        P = proj_create_crs_to_crs(ctxt, sourceCRS.c_str(), targetCRS.c_str(),
                                   nullptr);
    } else {
        usage();
    }
    if (P == nullptr) {
        exit(1);
    }
    PJ_COORD c;
    c.v[0] = coord_comp[0];
    c.v[1] = coord_comp[1];
    c.v[2] = coord_comp[2];
    c.v[3] = coord_comp[3];
    PJ_COORD c_ori = c;
    auto res = proj_trans(P, PJ_FWD, c);
    if (coord_comp_counter == 2) {
        printf("%.15g %.15g -> %.15g %.15g\n", c.v[0], c.v[1], res.v[0],
               res.v[1]);
    } else if (coord_comp_counter == 3) {
        printf("%.15g %.15g %.15g -> %.15g %.15g %.15g\n", c.v[0], c.v[1],
               c.v[2], res.v[0], res.v[1], res.v[2]);
    } else {
        printf("%.15g %.15g %.15g %.15g -> %.15g %.15g %.15g %.15g\n", c.v[0],
               c.v[1], c.v[2], c.v[3], res.v[0], res.v[1], res.v[2], res.v[3]);
    }

    // Start by timing just noise generation
    double dummy = 0;
    auto start_noise = std::chrono::system_clock::now();
    for (int i = 0; i < loops; ++i) {
        if (noiseX != 0)
            c.v[0] = c_ori.v[0] + noiseX * (2 * double(rand()) / RAND_MAX - 1);
        if (noiseY != 0)
            c.v[1] = c_ori.v[1] + noiseY * (2 * double(rand()) / RAND_MAX - 1);
        dummy += c.v[0];
        dummy += c.v[1];
    }
    auto end_noise = std::chrono::system_clock::now();

    auto start = std::chrono::system_clock::now();
    for (int i = 0; i < loops; ++i) {
        if (noiseX != 0)
            c.v[0] = c_ori.v[0] + noiseX * (2 * double(rand()) / RAND_MAX - 1);
        if (noiseY != 0)
            c.v[1] = c_ori.v[1] + noiseY * (2 * double(rand()) / RAND_MAX - 1);
        dummy += c.v[0];
        dummy += c.v[1];
        proj_trans(P, PJ_FWD, c);
    }
    auto end = std::chrono::system_clock::now();

    proj_destroy(P);
    proj_context_destroy(ctxt);

    auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        (end - start) - (end_noise - start_noise));
    printf("Duration: %d ms\n", static_cast<int>(elapsed_ms.count()));
    printf("Throughput: %.02f million coordinates/s\n",
           1e-3 * static_cast<double>(loops) /
                   static_cast<double>(elapsed_ms.count()) +
               dummy * 1e-300);

    return 0;
}
