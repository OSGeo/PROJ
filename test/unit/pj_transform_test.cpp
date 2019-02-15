/******************************************************************************
 *
 * Project:  PROJ
 * Purpose:  Test pj_transform() legacy interface
 * Author:   Even Rouault <even dot rouault at spatialys dot com>
 *
 ******************************************************************************
 * Copyright (c) 2018, Even Rouault <even dot rouault at spatialys dot com>
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

#define ACCEPT_USE_OF_DEPRECATED_PROJ_API_H

#include "gtest_include.h"
#include <memory>

// PROJ include order is sensitive
// clang-format off
#include <proj.h>
#include "proj_internal.h"
#include <proj_api.h>
// clang-format on

namespace {

// ---------------------------------------------------------------------------

TEST(pj_transform_test, longlat_to_longlat) {
    auto src = pj_init_plus("+proj=longlat +datum=WGS84");
    auto dst = pj_init_plus("+proj=longlat +datum=WGS84");
    double x = 2 * DEG_TO_RAD;
    double y = 49 * DEG_TO_RAD;
    EXPECT_EQ(pj_transform(src, dst, 1, 0, &x, &y, nullptr), 0);
    EXPECT_EQ(x, 2 * DEG_TO_RAD);
    EXPECT_EQ(y, 49 * DEG_TO_RAD);

    x = 182 * DEG_TO_RAD;
    EXPECT_EQ(pj_transform(src, dst, 1, 0, &x, &y, nullptr), 0);
    EXPECT_EQ(x, 182 * DEG_TO_RAD);
    EXPECT_EQ(y, 49 * DEG_TO_RAD);

    pj_free(src);
    pj_free(dst);
}

// ---------------------------------------------------------------------------

TEST(pj_transform_test, longlat_to_proj) {
    auto src = pj_init_plus("+proj=longlat +datum=WGS84");
    auto dst = pj_init_plus("+proj=utm +zone=31 +datum=WGS84");
    double x = 3 * DEG_TO_RAD;
    double y = 0 * DEG_TO_RAD;
    EXPECT_EQ(pj_transform(src, dst, 1, 0, &x, &y, nullptr), 0);
    EXPECT_NEAR(x, 500000, 1e-8);
    EXPECT_NEAR(y, 0, 1e-8);
    pj_free(src);
    pj_free(dst);
}

// ---------------------------------------------------------------------------

TEST(pj_transform_test, longlat_to_proj_tometer) {
    auto src = pj_init_plus("+proj=longlat +datum=WGS84");
    auto dst = pj_init_plus("+proj=utm +zone=31 +datum=WGS84 +to_meter=1000");
    double x = 3 * DEG_TO_RAD;
    double y = 0 * DEG_TO_RAD;
    EXPECT_EQ(pj_transform(src, dst, 1, 0, &x, &y, nullptr), 0);
    EXPECT_NEAR(x, 500, 1e-8);
    EXPECT_NEAR(y, 0, 1e-8);
    pj_free(src);
    pj_free(dst);
}

// ---------------------------------------------------------------------------

TEST(pj_transform_test, proj_to_longlat) {
    auto src = pj_init_plus("+proj=utm +zone=31 +datum=WGS84");
    auto dst = pj_init_plus("+proj=longlat +datum=WGS84");
    double x = 500000;
    double y = 0;
    EXPECT_EQ(pj_transform(src, dst, 1, 0, &x, &y, nullptr), 0);
    EXPECT_NEAR(x, 3 * DEG_TO_RAD, 1e-12);
    EXPECT_NEAR(y, 0 * DEG_TO_RAD, 1e-12);
    pj_free(src);
    pj_free(dst);
}

// ---------------------------------------------------------------------------

TEST(pj_transform_test, proj_to_proj) {
    auto src = pj_init_plus("+proj=utm +zone=31 +datum=WGS84");
    auto dst = pj_init_plus("+proj=utm +zone=31 +datum=WGS84");
    double x = 500000;
    double y = 0;
    EXPECT_EQ(pj_transform(src, dst, 1, 0, &x, &y, nullptr), 0);
    EXPECT_NEAR(x, 500000, 1e-8);
    EXPECT_NEAR(y, 0, 1e-8);
    pj_free(src);
    pj_free(dst);
}

// ---------------------------------------------------------------------------

TEST(pj_transform_test, longlat_to_geocent) {
    auto src = pj_init_plus("+proj=longlat +R=2");
    auto dst = pj_init_plus("+proj=geocent +R=2");
    double x = 0;
    double y = 0;
    double z = 0;
    EXPECT_EQ(pj_transform(src, dst, 1, 0, &x, &y, &z), 0);
    EXPECT_NEAR(x, 2, 1e-8);
    EXPECT_NEAR(y, 0, 1e-8);
    EXPECT_NEAR(z, 0, 1e-8);

    x = 90 * DEG_TO_RAD;
    y = 0;
    z = 0;
    EXPECT_EQ(pj_transform(src, dst, 1, 0, &x, &y, &z), 0);
    EXPECT_NEAR(x, 0, 1e-8);
    EXPECT_NEAR(y, 2, 1e-8);
    EXPECT_NEAR(z, 0, 1e-8);

    x = 0;
    y = 90 * DEG_TO_RAD;
    z = 0.1;
    EXPECT_EQ(pj_transform(src, dst, 1, 0, &x, &y, &z), 0);
    EXPECT_NEAR(x, 0, 1e-8);
    EXPECT_NEAR(y, 0, 1e-8);
    EXPECT_NEAR(z, 2 + 0.1, 1e-8);

    pj_free(src);
    pj_free(dst);
}

// ---------------------------------------------------------------------------

TEST(pj_transform_test, longlat_to_geocent_to_meter) {
    auto src = pj_init_plus("+proj=longlat +R=2");
    auto dst = pj_init_plus("+proj=geocent +R=2 +to_meter=1000");
    double x = 0;
    double y = 0;
    double z = 0;
    EXPECT_EQ(pj_transform(src, dst, 1, 0, &x, &y, &z), 0);
    EXPECT_NEAR(x, 2e-3, 1e-8);
    EXPECT_NEAR(y, 0, 1e-8);
    EXPECT_NEAR(z, 0, 1e-8);
    pj_free(src);
    pj_free(dst);
}

// ---------------------------------------------------------------------------

TEST(pj_transform_test, geocent_to_longlat) {
    auto src = pj_init_plus("+proj=geocent +R=2");
    auto dst = pj_init_plus("+proj=longlat +R=2");
    double x = 0;
    double y = 2;
    double z = 0;
    EXPECT_EQ(pj_transform(src, dst, 1, 0, &x, &y, &z), 0);
    EXPECT_NEAR(x, 90 * DEG_TO_RAD, 1e-12) << x / DEG_TO_RAD;
    EXPECT_NEAR(y, 0, 1e-12) << y / DEG_TO_RAD;
    EXPECT_NEAR(z, 0, 1e-12);

    pj_free(src);
    pj_free(dst);
}

// ---------------------------------------------------------------------------

TEST(pj_transform_test, geocent_to_meter_to_longlat) {
    auto src = pj_init_plus("+proj=geocent +to_meter=1000 +R=2");
    auto dst = pj_init_plus("+proj=longlat +R=2");
    double x = 0;
    double y = 2e-3;
    double z = 0;
    EXPECT_EQ(pj_transform(src, dst, 1, 0, &x, &y, &z), 0);
    EXPECT_NEAR(x, 90 * DEG_TO_RAD, 1e-12) << x / DEG_TO_RAD;
    EXPECT_NEAR(y, 0, 1e-12) << y / DEG_TO_RAD;
    EXPECT_NEAR(z, 0, 1e-12);
    pj_free(src);
    pj_free(dst);
}

// ---------------------------------------------------------------------------

TEST(pj_transform_test, pm) {
    auto src = pj_init_plus("+proj=longlat +pm=3 +datum=WGS84");
    auto dst = pj_init_plus("+proj=longlat +pm=1 +datum=WGS84");
    double x = 2 * DEG_TO_RAD;
    double y = 49 * DEG_TO_RAD;
    EXPECT_EQ(pj_transform(src, dst, 1, 0, &x, &y, nullptr), 0);
    EXPECT_NEAR(x, (2 + 3 - 1) * DEG_TO_RAD, 1e-12) << x / DEG_TO_RAD;
    EXPECT_EQ(y, 49 * DEG_TO_RAD) << y / DEG_TO_RAD;
    pj_free(src);
    pj_free(dst);
}

// ---------------------------------------------------------------------------

TEST(pj_transform_test, longlat_geoc_to_longlat) {
    auto src = pj_init_plus("+proj=longlat +geoc +datum=WGS84");
    auto dst = pj_init_plus("+proj=longlat +datum=WGS84");
    double x = 2 * DEG_TO_RAD;
    double y = 49 * DEG_TO_RAD;
    EXPECT_EQ(pj_transform(src, dst, 1, 0, &x, &y, nullptr), 0);
    EXPECT_NEAR(x, 2 * DEG_TO_RAD, 1e-12) << x / DEG_TO_RAD;
    EXPECT_NEAR(y, 48.809360314691766 * DEG_TO_RAD, 1e-12) << y / DEG_TO_RAD;
    pj_free(src);
    pj_free(dst);
}

// ---------------------------------------------------------------------------

TEST(pj_transform_test, longlat_to_longlat_geoc) {
    auto src = pj_init_plus("+proj=longlat +datum=WGS84");
    auto dst = pj_init_plus("+proj=longlat +geoc +datum=WGS84");
    double x = 2 * DEG_TO_RAD;
    double y = 48.809360314691766 * DEG_TO_RAD;
    EXPECT_EQ(pj_transform(src, dst, 1, 0, &x, &y, nullptr), 0);
    EXPECT_NEAR(x, 2 * DEG_TO_RAD, 1e-12) << x / DEG_TO_RAD;
    EXPECT_NEAR(y, 49 * DEG_TO_RAD, 1e-12) << y / DEG_TO_RAD;
    pj_free(src);
    pj_free(dst);
}

// ---------------------------------------------------------------------------

TEST(pj_transform_test, ellps_to_ellps_noop) {
    auto src = pj_init_plus("+proj=longlat +ellps=clrk66");
    auto dst = pj_init_plus("+proj=longlat +ellps=WGS84");
    double x = 2 * DEG_TO_RAD;
    double y = 49 * DEG_TO_RAD;
    EXPECT_EQ(pj_transform(src, dst, 1, 0, &x, &y, nullptr), 0);
    EXPECT_NEAR(x, 2 * DEG_TO_RAD, 1e-12) << x / DEG_TO_RAD;
    EXPECT_NEAR(y, 49 * DEG_TO_RAD, 1e-12) << y / DEG_TO_RAD;
    pj_free(src);
    pj_free(dst);
}

// ---------------------------------------------------------------------------

TEST(pj_transform_test, towgs84_3param_noop) {
    auto src = pj_init_plus("+proj=longlat +ellps=WGS84 +towgs84=1,2,3");
    auto dst = pj_init_plus("+proj=longlat +ellps=WGS84 +towgs84=1,2,3");
    double x = 2 * DEG_TO_RAD;
    double y = 49 * DEG_TO_RAD;
    double z = 10;
    EXPECT_EQ(pj_transform(src, dst, 1, 0, &x, &y, &z), 0);
    EXPECT_NEAR(x, 2 * DEG_TO_RAD, 1e-12) << x / DEG_TO_RAD;
    EXPECT_NEAR(y, 49 * DEG_TO_RAD, 1e-12) << y / DEG_TO_RAD;
    EXPECT_NEAR(z, 10, 1e-8);
    pj_free(src);
    pj_free(dst);
}

// ---------------------------------------------------------------------------

TEST(pj_transform_test, towgs84_7param_noop) {
    auto src =
        pj_init_plus("+proj=longlat +ellps=WGS84 +towgs84=1,2,3,4,5,6,7");
    auto dst =
        pj_init_plus("+proj=longlat +ellps=WGS84 +towgs84=1,2,3,4,5,6,7");
    double x = 2 * DEG_TO_RAD;
    double y = 49 * DEG_TO_RAD;
    double z = 10;
    EXPECT_EQ(pj_transform(src, dst, 1, 0, &x, &y, &z), 0);
    EXPECT_NEAR(x, 2 * DEG_TO_RAD, 1e-12) << x / DEG_TO_RAD;
    EXPECT_NEAR(y, 49 * DEG_TO_RAD, 1e-12) << y / DEG_TO_RAD;
    EXPECT_NEAR(z, 10, 1e-8);
    pj_free(src);
    pj_free(dst);
}

// ---------------------------------------------------------------------------

TEST(pj_transform_test, longlat_towgs84_3param_to_datum) {
    auto src = pj_init_plus("+proj=longlat +ellps=WGS84 +towgs84=0,1,0");
    auto dst = pj_init_plus("+proj=longlat +datum=WGS84");
    double x = 90 * DEG_TO_RAD;
    double y = 0 * DEG_TO_RAD;
    double z = 10;
    EXPECT_EQ(pj_transform(src, dst, 1, 0, &x, &y, &z), 0);
    EXPECT_NEAR(x, 90 * DEG_TO_RAD, 1e-12) << x / DEG_TO_RAD;
    EXPECT_NEAR(y, 0 * DEG_TO_RAD, 1e-12) << y / DEG_TO_RAD;
    EXPECT_NEAR(z, 10 + 1, 1e-8);
    pj_free(src);
    pj_free(dst);
}

// ---------------------------------------------------------------------------

TEST(pj_transform_test, longlat_towgs84_3param_to_datum_no_z) {
    auto src = pj_init_plus("+proj=longlat +ellps=WGS84 +towgs84=0,1,0");
    auto dst = pj_init_plus("+proj=longlat +datum=WGS84");
    double x = 90 * DEG_TO_RAD;
    double y = 0 * DEG_TO_RAD;
    EXPECT_EQ(pj_transform(src, dst, 1, 0, &x, &y, nullptr), 0);
    EXPECT_NEAR(x, 90 * DEG_TO_RAD, 1e-12) << x / DEG_TO_RAD;
    EXPECT_NEAR(y, 0 * DEG_TO_RAD, 1e-12) << y / DEG_TO_RAD;
    pj_free(src);
    pj_free(dst);
}

// ---------------------------------------------------------------------------

TEST(pj_transform_test, longlat_towgs84_7param_to_datum) {
    auto src =
        pj_init_plus("+proj=longlat +ellps=WGS84 +towgs84=0,1,0,0,0,0,0.5");
    auto dst = pj_init_plus("+proj=longlat +datum=WGS84");
    double x = 90 * DEG_TO_RAD;
    double y = 0 * DEG_TO_RAD;
    double z = 10;
    EXPECT_EQ(pj_transform(src, dst, 1, 0, &x, &y, &z), 0);
    EXPECT_NEAR(x, 90 * DEG_TO_RAD, 1e-12) << x / DEG_TO_RAD;
    EXPECT_NEAR(y, 0 * DEG_TO_RAD, 1e-12) << y / DEG_TO_RAD;
    EXPECT_NEAR(z, 14.189073500223458, 1e-8);
    pj_free(src);
    pj_free(dst);
}

// ---------------------------------------------------------------------------

TEST(pj_transform_test, datum_to_longlat_towgs84_3param) {
    auto src = pj_init_plus("+proj=longlat +datum=WGS84");
    auto dst = pj_init_plus("+proj=longlat +ellps=WGS84 +towgs84=0,1,0");
    double x = 90 * DEG_TO_RAD;
    double y = 0 * DEG_TO_RAD;
    double z = 10 + 1;
    EXPECT_EQ(pj_transform(src, dst, 1, 0, &x, &y, &z), 0);
    EXPECT_NEAR(x, 90 * DEG_TO_RAD, 1e-12) << x / DEG_TO_RAD;
    EXPECT_NEAR(y, 0 * DEG_TO_RAD, 1e-12) << y / DEG_TO_RAD;
    EXPECT_NEAR(z, 10, 1e-8);
    pj_free(src);
    pj_free(dst);
}

// ---------------------------------------------------------------------------

TEST(pj_transform_test, datum_to_longlat_towgs84_7param) {
    auto src = pj_init_plus("+proj=longlat +datum=WGS84");
    auto dst =
        pj_init_plus("+proj=longlat +ellps=WGS84 +towgs84=0,1,0,0,0,0,0.5");
    double x = 90 * DEG_TO_RAD;
    double y = 0 * DEG_TO_RAD;
    double z = 14.189073500223458;
    EXPECT_EQ(pj_transform(src, dst, 1, 0, &x, &y, &z), 0);
    EXPECT_NEAR(x, 90 * DEG_TO_RAD, 1e-12) << x / DEG_TO_RAD;
    EXPECT_NEAR(y, 0 * DEG_TO_RAD, 1e-12) << y / DEG_TO_RAD;
    EXPECT_NEAR(z, 10, 1e-8);
    pj_free(src);
    pj_free(dst);
}

// ---------------------------------------------------------------------------

TEST(pj_transform_test, ellps_grs80_towgs84_to_datum_wgs84) {
    auto src = pj_init_plus("+proj=longlat +ellps=GRS80 +towgs84=0,0,0");
    auto dst = pj_init_plus("+proj=longlat +datum=WGS84");
    double x = 2 * DEG_TO_RAD;
    double y = 49 * DEG_TO_RAD;
    double z = 10;
    EXPECT_EQ(pj_transform(src, dst, 1, 0, &x, &y, &z), 0);
    EXPECT_NEAR(x, 2 * DEG_TO_RAD, 1e-15) << x / DEG_TO_RAD;
    EXPECT_NEAR(y, 49 * DEG_TO_RAD, 1e-15) << y / DEG_TO_RAD;
    EXPECT_NEAR(z, 10, 1e-8);
    pj_free(src);
    pj_free(dst);
}

// ---------------------------------------------------------------------------

TEST(pj_transform_test, longlat_nadgrids_to_datum) {
    auto src = pj_init_plus("+proj=longlat +ellps=clrk66 +nadgrids=conus");
    auto dst = pj_init_plus("+proj=longlat +datum=NAD83");
    double x = -100 * DEG_TO_RAD;
    double y = 40 * DEG_TO_RAD;
    double z = 10;
    int ret = pj_transform(src, dst, 1, 0, &x, &y, &z);
    EXPECT_TRUE(ret == 0 || ret == PJD_ERR_FAILED_TO_LOAD_GRID);
    if (ret == 0) {
        EXPECT_NEAR(x, -100.00040583667015 * DEG_TO_RAD, 1e-12)
            << x / DEG_TO_RAD;
        EXPECT_NEAR(y, 40.000005895651363 * DEG_TO_RAD, 1e-12)
            << y / DEG_TO_RAD;
        EXPECT_NEAR(z, 10.000043224543333, 1e-8);
    }
    pj_free(src);
    pj_free(dst);
}

// ---------------------------------------------------------------------------

TEST(pj_transform_test, nadgrids_noop) {
    auto src = pj_init_plus("+proj=longlat +ellps=clrk66 +nadgrids=conus");
    auto dst = pj_init_plus("+proj=longlat +ellps=clrk66 +nadgrids=conus");
    double x = -100 * DEG_TO_RAD;
    double y = 40 * DEG_TO_RAD;
    double z = 10;
    int ret = pj_transform(src, dst, 1, 0, &x, &y, &z);
    EXPECT_TRUE(ret == 0);
    if (ret == 0) {
        EXPECT_NEAR(x, -100 * DEG_TO_RAD, 1e-15) << x / DEG_TO_RAD;
        EXPECT_NEAR(y, 40 * DEG_TO_RAD, 1e-15) << y / DEG_TO_RAD;
        EXPECT_NEAR(z, 10, 1e-8);
    }
    pj_free(src);
    pj_free(dst);
}

// ---------------------------------------------------------------------------

TEST(pj_transform_test, datum_to_longlat_nadgrids) {
    auto src = pj_init_plus("+proj=longlat +datum=NAD83");
    auto dst = pj_init_plus("+proj=longlat +ellps=clrk66 +nadgrids=conus");
    double x = -100.00040583667015 * DEG_TO_RAD;
    double y = 40.000005895651363 * DEG_TO_RAD;
    double z = 10.000043224543333;
    int ret = pj_transform(src, dst, 1, 0, &x, &y, &z);
    EXPECT_TRUE(ret == 0 || ret == PJD_ERR_FAILED_TO_LOAD_GRID);
    if (ret == 0) {
        EXPECT_NEAR(x, -100 * DEG_TO_RAD, 1e-12) << x / DEG_TO_RAD;
        EXPECT_NEAR(y, 40 * DEG_TO_RAD, 1e-12) << y / DEG_TO_RAD;
        EXPECT_NEAR(z, 10, 1e-8);
    }
    pj_free(src);
    pj_free(dst);
}

// ---------------------------------------------------------------------------

TEST(pj_transform_test, long_wrap) {
    auto src = pj_init_plus("+proj=longlat +datum=WGS84");
    auto dst = pj_init_plus("+proj=longlat +datum=WGS84 +lon_wrap=180");
    double x = -1 * DEG_TO_RAD;
    double y = 0 * DEG_TO_RAD;
    EXPECT_EQ(pj_transform(src, dst, 1, 0, &x, &y, nullptr), 0);
    EXPECT_NEAR(x, 359 * DEG_TO_RAD, 1e-12) << x / DEG_TO_RAD;
    EXPECT_NEAR(y, 0 * DEG_TO_RAD, 1e-12) << y / DEG_TO_RAD;
    pj_free(src);
    pj_free(dst);
}

// ---------------------------------------------------------------------------

TEST(pj_transform_test, src_vto_meter) {
    auto src = pj_init_plus("+proj=longlat +datum=WGS84 +vto_meter=1000");
    auto dst = pj_init_plus("+proj=longlat +datum=WGS84");
    double x = 2 * DEG_TO_RAD;
    double y = 49 * DEG_TO_RAD;
    double z = 1;
    EXPECT_EQ(pj_transform(src, dst, 1, 0, &x, &y, &z), 0);
    EXPECT_NEAR(x, 2 * DEG_TO_RAD, 1e-12) << x / DEG_TO_RAD;
    EXPECT_NEAR(y, 49 * DEG_TO_RAD, 1e-12) << y / DEG_TO_RAD;
    EXPECT_NEAR(z, 1000, 1e-8);
    pj_free(src);
    pj_free(dst);
}

// ---------------------------------------------------------------------------

TEST(pj_transform_test, dest_vto_meter) {
    auto src = pj_init_plus("+proj=longlat +datum=WGS84");
    auto dst = pj_init_plus("+proj=longlat +datum=WGS84 +vto_meter=1000");
    double x = 2 * DEG_TO_RAD;
    double y = 49 * DEG_TO_RAD;
    double z = 1000;
    EXPECT_EQ(pj_transform(src, dst, 1, 0, &x, &y, &z), 0);
    EXPECT_NEAR(x, 2 * DEG_TO_RAD, 1e-12) << x / DEG_TO_RAD;
    EXPECT_NEAR(y, 49 * DEG_TO_RAD, 1e-12) << y / DEG_TO_RAD;
    EXPECT_NEAR(z, 1, 1e-8);
    pj_free(src);
    pj_free(dst);
}

// ---------------------------------------------------------------------------

TEST(pj_transform_test, src_axis_neu_with_z) {
    auto src = pj_init_plus("+proj=longlat +datum=WGS84 +axis=neu");
    auto dst = pj_init_plus("+proj=longlat +datum=WGS84");
    double x = 49 * DEG_TO_RAD;
    double y = 2 * DEG_TO_RAD;
    double z = 1;
    EXPECT_EQ(pj_transform(src, dst, 1, 0, &x, &y, &z), 0);
    EXPECT_NEAR(x, 2 * DEG_TO_RAD, 1e-12) << x / DEG_TO_RAD;
    EXPECT_NEAR(y, 49 * DEG_TO_RAD, 1e-12) << y / DEG_TO_RAD;
    EXPECT_NEAR(z, 1, 1e-8);
    pj_free(src);
    pj_free(dst);
}

// ---------------------------------------------------------------------------

TEST(pj_transform_test, src_axis_neu_without_z) {
    auto src = pj_init_plus("+proj=longlat +datum=WGS84 +axis=neu");
    auto dst = pj_init_plus("+proj=longlat +datum=WGS84");
    double x = 49 * DEG_TO_RAD;
    double y = 2 * DEG_TO_RAD;
    EXPECT_EQ(pj_transform(src, dst, 1, 0, &x, &y, nullptr), 0);
    EXPECT_NEAR(x, 2 * DEG_TO_RAD, 1e-12) << x / DEG_TO_RAD;
    EXPECT_NEAR(y, 49 * DEG_TO_RAD, 1e-12) << y / DEG_TO_RAD;
    pj_free(src);
    pj_free(dst);
}

// ---------------------------------------------------------------------------

TEST(pj_transform_test, src_axis_swd) {
    auto src = pj_init_plus("+proj=longlat +datum=WGS84 +axis=swd");
    auto dst = pj_init_plus("+proj=longlat +datum=WGS84");
    double x = 49 * DEG_TO_RAD;
    double y = 2 * DEG_TO_RAD;
    double z = -1;
    EXPECT_EQ(pj_transform(src, dst, 1, 0, &x, &y, &z), 0);
    EXPECT_NEAR(x, -2 * DEG_TO_RAD, 1e-12) << x / DEG_TO_RAD;
    EXPECT_NEAR(y, -49 * DEG_TO_RAD, 1e-12) << y / DEG_TO_RAD;
    EXPECT_NEAR(z, 1, 1e-8);
    pj_free(src);
    pj_free(dst);
}

// ---------------------------------------------------------------------------

TEST(pj_transform_test, dst_axis_neu) {
    auto src = pj_init_plus("+proj=longlat +datum=WGS84");
    auto dst = pj_init_plus("+proj=longlat +datum=WGS84 +axis=neu");
    double x = 2 * DEG_TO_RAD;
    double y = 49 * DEG_TO_RAD;
    double z = 1;
    EXPECT_EQ(pj_transform(src, dst, 1, 0, &x, &y, &z), 0);
    EXPECT_NEAR(x, 49 * DEG_TO_RAD, 1e-12) << x / DEG_TO_RAD;
    EXPECT_NEAR(y, 2 * DEG_TO_RAD, 1e-12) << y / DEG_TO_RAD;
    EXPECT_NEAR(z, 1, 1e-8);
    pj_free(src);
    pj_free(dst);
}
// ---------------------------------------------------------------------------

TEST(pj_transform_test, dst_axis_swd) {
    auto src = pj_init_plus("+proj=longlat +datum=WGS84");
    auto dst = pj_init_plus("+proj=longlat +datum=WGS84 +axis=swd");
    double x = 2 * DEG_TO_RAD;
    double y = 49 * DEG_TO_RAD;
    double z = 1;
    EXPECT_EQ(pj_transform(src, dst, 1, 0, &x, &y, &z), 0);
    EXPECT_NEAR(x, -49 * DEG_TO_RAD, 1e-12) << x / DEG_TO_RAD;
    EXPECT_NEAR(y, -2 * DEG_TO_RAD, 1e-12) << y / DEG_TO_RAD;
    EXPECT_NEAR(z, -1, 1e-8);
    pj_free(src);
    pj_free(dst);
}

// ---------------------------------------------------------------------------

TEST(pj_transform_test, init_epsg) {
    auto src = pj_init_plus("+init=epsg:4326");
    ASSERT_TRUE(src != nullptr);
    auto dst = pj_init_plus("+init=epsg:32631");
    ASSERT_TRUE(dst != nullptr);
    double x = 3 * DEG_TO_RAD;
    double y = 0 * DEG_TO_RAD;
    EXPECT_EQ(pj_transform(src, dst, 1, 0, &x, &y, nullptr), 0);
    EXPECT_NEAR(x, 500000, 1e-8);
    EXPECT_NEAR(y, 0, 1e-8);
    pj_free(src);
    pj_free(dst);
}

// ---------------------------------------------------------------------------

TEST(proj_api_h, pj_set_searchpath) {

    const char *path = "/i_do/not/exit";
    pj_set_searchpath(1, &path);
    {
        auto info = proj_info();
        EXPECT_EQ(info.path_count, 1U);
        ASSERT_NE(info.paths, nullptr);
        ASSERT_NE(info.paths[0], nullptr);
        EXPECT_EQ(std::string(info.paths[0]), path);
    }

    pj_set_searchpath(0, nullptr);
    {
        auto info = proj_info();
        EXPECT_EQ(info.path_count, 0U);
        EXPECT_EQ(info.paths, nullptr);
    }
}

// ---------------------------------------------------------------------------

TEST(proj_api_h, pj_set_finder) {

    const auto myfinder = [](const char *) -> const char * { return nullptr; };
    pj_set_finder(myfinder);

    pj_set_finder(nullptr);
}

} // namespace
