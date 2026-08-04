#include "proj.h"

#include "gtest_include.h"

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
}

TEST(tmerczoned, west_boundary_equator_32600) {
    PJ_COORD test;
    test.lp.lam = 0.0;
    test.lp.phi = -180.0;

    PJ_COORD expected;
    expected.xy.x = 166021.4431;
    expected.xy.y = 0;

    test_all_zones("epsg:32600", test, expected);
}

TEST(tmerczoned, west_boundary_high_latitude_32600) {
    PJ_COORD test;
    test.lp.lam = 80.0;
    test.lp.phi = -180.0;

    PJ_COORD expected;
    expected.xy.x = 441867.7849;
    expected.xy.y = 8883084.9559;

    test_all_zones("epsg:32600", test, expected);
}

TEST(tmerczoned, east_boundary_equator_32600) {
    PJ_COORD test;
    test.lp.lam = 0.0;
    test.lp.phi = -174.000000001;

    PJ_COORD expected;
    expected.xy.x = 833978.5568;
    expected.xy.y = 0;

    test_all_zones("epsg:32600", test, expected);
}

TEST(tmerczoned, east_boundary_high_latitude_32600) {
    PJ_COORD test;
    test.lp.lam = 80.0;
    test.lp.phi = -174.000000001;

    PJ_COORD expected;
    expected.xy.x = 558132.2151;
    expected.xy.y = 8883084.9559;

    test_all_zones("epsg:32600", test, expected);
}

TEST(tmerczoned, west_boundary_equator_32700) {
    PJ_COORD test;
    test.lp.lam = 0.0;
    test.lp.phi = -180.0;

    PJ_COORD expected;
    expected.xy.x = 166021.4431;
    expected.xy.y = 10000000.0000;

    test_all_zones("epsg:32700", test, expected);
}

TEST(tmerczoned, west_boundary_low_latitude_32700) {
    PJ_COORD test;
    test.lp.lam = -80.0;
    test.lp.phi = -180.0;

    PJ_COORD expected;
    expected.xy.x = 441867.7849;
    expected.xy.y = 1116915.0441;

    test_all_zones("epsg:32700", test, expected);
}

TEST(tmerczoned, east_boundary_equator_32700) {
    PJ_COORD test;
    test.lp.lam = 0.0;
    test.lp.phi = -174.000000001;

    PJ_COORD expected;
    expected.xy.x = 833978.5568;
    expected.xy.y = 10000000.0000;

    test_all_zones("epsg:32700", test, expected);
}

TEST(tmerczoned, east_boundary_low_latitude_32700) {
    PJ_COORD test;
    test.lp.lam = -80.0;
    test.lp.phi = -174.000000001;

    PJ_COORD expected;
    expected.xy.x = 558132.2151;
    expected.xy.y = 1116915.0441;

    test_all_zones("epsg:32700", test, expected);
}
