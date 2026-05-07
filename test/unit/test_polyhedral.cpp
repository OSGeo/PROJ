/******************************************************************************
 * Project:  PROJ
 * Purpose:  Tests for the polyhedral equal-area projections.
 * Author:   Felix Palmer
 *
 ****************************************************************************/

#include "gtest_include.h"

// PROJ include order is sensitive
// clang-format off
#include "proj.h"
#include "proj_internal.h"
// clang-format on

#include <cmath>
#include <string>

namespace {

struct RoundtripPoint {
    double lat;
    double lon;
};

static const RoundtripPoint test_points[] = {
    {0.0, 0.0},     {45.0, 90.0},   {-45.0, -90.0},  {89.0, 0.0},
    {-89.0, 170.0}, {30.0, 120.0},  {-60.0, -150.0}, {15.0, 45.0},
    {-30.0, -60.0}, {70.0, -170.0},
};

struct ProjConfig {
    const char *str;
    double tol;
};

// 1e-13 rad is ~0.6 nm at the equator on Earth — the precision the original
// (no-offset) implementation hits. Configs that compose extra rotations and
// projected-space offsets give up a few bits to cancellation near triangle
// edges; 1e-11 rad is still ~60 μm, well below any practical tolerance.
static const ProjConfig proj_strings[] = {
    {"+proj=tsea +R=1", 1e-13},
    {"+proj=tsea +ellps=WGS84", 1e-13},
    {"+proj=tsea +R=1 +orient_lat=90 +orient_lon=0 +azi=0", 1e-13},
    {"+proj=tsea +R=1 +lat_0=10 +lon_0=20", 1e-11},
    {"+proj=tsea +R=1 +orient_lat=45 +orient_lon=30 +azi=60 +lat_0=15 "
     "+lon_0=-25 +x_0=1000 +y_0=-2000",
     1e-11},
    {"+proj=dsea +R=1", 1e-13},
    {"+proj=dsea +ellps=WGS84", 1e-13},
    {"+proj=dsea +R=1 +lat_0=10 +lon_0=20", 1e-11},
    {"+proj=isea2 +R=1", 1e-13},
    {"+proj=isea2 +ellps=WGS84", 1e-13},
    {"+proj=isea2 +R=1 +lat_0=10 +lon_0=20", 1e-11},
};

static void roundtrip_test(const char *proj_string, double tolerance) {
    PJ *testProj = proj_create(PJ_DEFAULT_CTX, proj_string);
    ASSERT_TRUE(testProj != nullptr) << "Failed to create: " << proj_string;

    for (const auto &point : test_points) {
        PJ_COORD coord = proj_coord(0, 0, 0, 0);
        coord.lp.lam = proj_torad(point.lon);
        coord.lp.phi = proj_torad(point.lat);

        PJ_COORD flat = proj_trans(testProj, PJ_FWD, coord);
        ASSERT_FALSE(flat.lp.lam == HUGE_VAL)
            << "Forward failed for lon=" << point.lon << " lat=" << point.lat;

        PJ_COORD new_coord = proj_trans(testProj, PJ_INV, flat);
        ASSERT_FALSE(new_coord.lp.lam == HUGE_VAL)
            << "Inverse failed for lon=" << point.lon << " lat=" << point.lat;

        EXPECT_NEAR(coord.lp.lam, new_coord.lp.lam, tolerance)
            << "lon roundtrip for lon=" << point.lon << " lat=" << point.lat;
        EXPECT_NEAR(coord.lp.phi, new_coord.lp.phi, tolerance)
            << "lat roundtrip for lon=" << point.lon << " lat=" << point.lat;
    }

    proj_destroy(testProj);
}

TEST(polyhedral, roundtrip) {
    for (const auto &cfg : proj_strings) {
        roundtrip_test(cfg.str, cfg.tol);
    }
}

// +lat_0 / +lon_0 must place the supplied geographic point at the projected
// origin (0, 0). +x_0 / +y_0 then stack on top via PROJ core.
TEST(polyhedral, lat0_lon0_at_origin) {
    PJ *p = proj_create(PJ_DEFAULT_CTX, "+proj=tsea +R=1 +lat_0=10 +lon_0=20");
    ASSERT_TRUE(p != nullptr);

    PJ_COORD c = proj_coord(0, 0, 0, 0);
    c.lp.lam = proj_torad(20.0);
    c.lp.phi = proj_torad(10.0);
    PJ_COORD f = proj_trans(p, PJ_FWD, c);
    EXPECT_NEAR(f.xy.x, 0.0, 1e-13);
    EXPECT_NEAR(f.xy.y, 0.0, 1e-13);

    proj_destroy(p);
}

TEST(polyhedral, dsea_invalid_net_rejected) {
    PJ *p = proj_create(PJ_DEFAULT_CTX, "+proj=dsea +R=1 +net=does_not_exist");
    EXPECT_TRUE(p == nullptr);
    proj_destroy(p);
}

TEST(polyhedral, dsea_known_nets_accepted) {
    for (const char *net : {"dsea", "a5", "crescent", "flower"}) {
        std::string s = std::string("+proj=dsea +R=1 +net=") + net;
        PJ *p = proj_create(PJ_DEFAULT_CTX, s.c_str());
        ASSERT_TRUE(p != nullptr) << "Failed to create: " << s;
        proj_destroy(p);
    }
}

TEST(polyhedral, x0_y0_stacks_on_lat0_lon0) {
    PJ *p = proj_create(PJ_DEFAULT_CTX,
                        "+proj=tsea +R=1 +lat_0=10 +lon_0=20 +x_0=7 +y_0=-3");
    ASSERT_TRUE(p != nullptr);

    PJ_COORD c = proj_coord(0, 0, 0, 0);
    c.lp.lam = proj_torad(20.0);
    c.lp.phi = proj_torad(10.0);
    PJ_COORD f = proj_trans(p, PJ_FWD, c);
    EXPECT_NEAR(f.xy.x, 7.0, 1e-13);
    EXPECT_NEAR(f.xy.y, -3.0, 1e-13);

    proj_destroy(p);
}

} // namespace
