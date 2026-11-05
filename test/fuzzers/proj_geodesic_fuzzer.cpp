/* proj_geodesic_fuzzer.cpp — component-API harness for the geodesic.c kernel.
 *
 * Drives geod_init / geod_direct / geod_gendirect / geod_inverse /
 * geod_lineinit / geod_position / geod_polygonarea with fuzzed ellipsoid
 * parameters and coordinates. The geodesic solver is used on untrusted
 * coordinates by many GIS consumers; the documented preconditions (a > 0,
 * -1 < f < 1) are enforced in the harness so findings are real.
 *
 * Input: byte 0 flags (bit0 arc-mode for lineinit/position, bits1-2 unroll
 * bits for geod_gendirect), then 8 little-endian doubles:
 * a, f, lat1, lon1, az1, lat2, lon2, s12.
 * Copyright (c) 2026, OSS-Fuzz coverage pipeline. MIT/Expat like the rest of
 * PROJ's fuzzers (see proj_crs_to_crs_fuzzer.cpp).
 */
#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "geodesic.h"

#define MIN_INPUT (1 + 8 * (int)sizeof(double))

static double clamp_param(double v, double lo, double hi, double dflt) {
    if (!(v == v) || v <= lo || v >= hi) return dflt; /* NaN / out of contract */
    return v;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *buf, size_t len) {
    if (len < (size_t)MIN_INPUT) return 0;
    const uint8_t flags = buf[0];
    double v[8];
    memcpy(v, buf + 1, sizeof(v));

    struct geod_geodesic g;
    double a = clamp_param(v[0], 0.0, 1e30, 6378137.0);
    double f = clamp_param(v[1], -1.0, 1.0, 1.0 / 298.257223563);
    geod_init(&g, a, f);

    double lat1 = v[2], lon1 = v[3], az1 = v[4];
    double lat2 = v[5], lon2 = v[6], s12 = v[7];

    double plat, plon, paz;
    geod_direct(&g, lat1, lon1, az1, s12, &plat, &plon, &paz);
    double ps12, paz1, paz2;
    geod_inverse(&g, lat1, lon1, lat2, lon2, &ps12, &paz1, &paz2);

    unsigned gflags = ((flags & 0x01) ? (unsigned)GEOD_ARCMODE
                                      : (unsigned)GEOD_NOFLAGS) |
                      ((flags & 0x06) ? (unsigned)GEOD_LONG_UNROLL : 0U);
    double gplat, gplon, gpaz, gps12;
    (void)geod_gendirect(&g, lat1, lon1, az1, gflags, fabs(s12), &gplat, &gplon,
                         &gpaz, &gps12, NULL, NULL, NULL, NULL);

    struct geod_geodesicline line;
    unsigned caps = (unsigned)(GEOD_LATITUDE | GEOD_LONGITUDE | GEOD_AZIMUTH |
                               GEOD_DISTANCE | GEOD_DISTANCE_IN);
    geod_lineinit(&line, &g, lat1, lon1, az1, caps);
    double llat, llon, laz;
    geod_position(&line, fabs(s12), &llat, &llon, &laz);

    double lats[3] = {lat1, lat2, plat};
    double lons[3] = {lon1, lon2, plon};
    double area, perim;
    geod_polygonarea(&g, lats, lons, 3, &area, &perim);

    return 0;
}
