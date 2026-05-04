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

static const char *proj_strings[] = {
    "+proj=tsea +R=1",
    "+proj=tsea +ellps=WGS84",
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
    for (const auto *proj_string : proj_strings) {
        // 1e-13 radians corresponds to accuracy to ~0.6nm (worst case at
        // equator)
        roundtrip_test(proj_string, 1e-13);
    }
}

} // namespace
