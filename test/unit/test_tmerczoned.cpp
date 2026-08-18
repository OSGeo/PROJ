#include "proj.h"

#include "gtest_include.h"

#include <math.h>

namespace {

void test_all_zones(const char *target, const PJ_COORD expected_geographic,
                    const PJ_COORD expected_projected) {
    auto P =
        proj_create_crs_to_crs(PJ_DEFAULT_CTX, "epsg:4326", target, nullptr);

    constexpr double zone_width = 6;
    constexpr double easting_offset = 1e6;

    auto in_fwd = expected_geographic;
    for (int zone = 1; zone < 61; ++zone) {
        // Offset the test longitude to the specific zone.
        in_fwd.lp.phi = expected_geographic.lp.phi + (zone - 1) * zone_width;

        // Forward
        auto out_fwd = proj_trans(P, PJ_FWD, in_fwd);
        EXPECT_NEAR(out_fwd.xy.x,
                    expected_projected.xy.x + zone * easting_offset, 1e-4);
        EXPECT_NEAR(out_fwd.xy.y, expected_projected.xy.y, 1e-4);

        // Inverse (Roundtrip)
        auto out_inv = proj_trans(P, PJ_INV, out_fwd);
        EXPECT_NEAR(out_inv.lp.lam, expected_geographic.lp.lam, 1e-10);
        EXPECT_NEAR(out_inv.lp.phi,
                    expected_geographic.lp.phi + (zone - 1) * zone_width,
                    1e-10);
    }

    proj_destroy(P);
}

} // namespace

TEST(tmerczoned, west_boundary_equator_32600) {
    const auto test = proj_coord(0, -180.0, 0, HUGE_VAL);
    const auto expected = proj_coord(166021.4431, 0, 0, HUGE_VAL);

    test_all_zones("epsg:32600", test, expected);
}

TEST(tmerczoned, west_boundary_high_latitude_32600) {
    const auto test = proj_coord(80.0, -180.0, 0, HUGE_VAL);
    const auto expected = proj_coord(441867.7849, 8883084.9559, 0, HUGE_VAL);

    test_all_zones("epsg:32600", test, expected);
}

TEST(tmerczoned, east_boundary_equator_32600) {
    const auto test = proj_coord(0, -174.000000001, 0, HUGE_VAL);
    const auto expected = proj_coord(833978.5568, 0, 0, HUGE_VAL);

    test_all_zones("epsg:32600", test, expected);
}

TEST(tmerczoned, east_boundary_high_latitude_32600) {
    const auto test = proj_coord(80.0, -174.000000001, 0, HUGE_VAL);
    const auto expected = proj_coord(558132.2151, 8883084.9559, 0, HUGE_VAL);

    test_all_zones("epsg:32600", test, expected);
}

TEST(tmerczoned, west_boundary_equator_32700) {
    const auto test = proj_coord(0, -180.0, 0, HUGE_VAL);
    const auto expected = proj_coord(166021.4431, 10000000.0000, 0, HUGE_VAL);

    test_all_zones("epsg:32700", test, expected);
}

TEST(tmerczoned, west_boundary_low_latitude_32700) {
    const auto test = proj_coord(-80.0, -180.0, 0, HUGE_VAL);
    const auto expected = proj_coord(441867.7849, 1116915.0441, 0, HUGE_VAL);

    test_all_zones("epsg:32700", test, expected);
}

TEST(tmerczoned, east_boundary_equator_32700) {
    const auto test = proj_coord(0, -174.000000001, 0, HUGE_VAL);
    const auto expected = proj_coord(833978.5568, 10000000.0000, 0, HUGE_VAL);

    test_all_zones("epsg:32700", test, expected);
}

TEST(tmerczoned, east_boundary_low_latitude_32700) {
    const auto test = proj_coord(-80.0, -174.000000001, 0, HUGE_VAL);
    const auto expected = proj_coord(558132.2151, 1116915.0441, 0, HUGE_VAL);

    test_all_zones("epsg:32700", test, expected);
}
