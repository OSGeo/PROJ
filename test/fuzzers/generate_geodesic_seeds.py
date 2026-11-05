#!/usr/bin/env python3
"""Deterministic seed generator for proj_geodesic_fuzzer.

Input layout: byte 0 flags, then 8 LE doubles: a, f, lat1, lon1, az1,
lat2, lon2, s12. Seeds cover the documented preconditions and the
hard cases of the geodesic solver: normal ellipsoid, sphere (f=0),
extreme flattening, antipodal points, near-singular azimuths.
"""

import os
import struct
import sys
import zipfile

WGS84_A = 6378137.0
WGS84_F = 1.0 / 298.257223563

SEEDS = [
    ("wgs84_direct_inverse", 0x00, WGS84_A, WGS84_F, 49.0, 2.0, 45.0, 50.0, 3.0, 1.0e6),
    ("sphere_f0", 0x00, WGS84_A, 0.0, 0.0, 0.0, 90.0, -90.0, 0.0, 2.0e7),
    ("extreme_flattening", 0x00, WGS84_A, 0.999, 10.0, 10.0, 200.0, -10.0, -10.0, 1.0e5),
    ("antipodal", 0x00, WGS84_A, WGS84_F, 0.0, 0.0, 90.0, 0.0, 180.0, 0.0),
    ("near_singular_azimuth", 0x00, WGS84_A, WGS84_F, 89.9999999, 179.9999999, 1e-9,
     89.9999999, -179.9999999, 1.0),
    ("arc_mode_line", 0x01, WGS84_A, WGS84_F, -33.0, 151.0, 22.0, -34.0, 150.0, 7.5e6),
    ("unroll_gendirect", 0x04, WGS84_A, WGS84_F, 0.0, 0.0, 1.0, 1.0, 1.0, 4.0e7),
    ("equator_line", 0x00, WGS84_A, WGS84_F, 0.0, 0.0, 90.0, 0.0, 90.0, 1.0e7),
]


def main():
    if len(sys.argv) != 2:
        print("usage: generate_geodesic_seeds.py <output.zip>")
        return 1
    out = sys.argv[1]
    os.makedirs(os.path.dirname(os.path.abspath(out)), exist_ok=True)
    with zipfile.ZipFile(out, "w", zipfile.ZIP_DEFLATED) as zf:
        for entry in SEEDS:
            (name, flags) = entry[:2]
            vals = entry[2:]
            blob = bytes([flags]) + struct.pack("<8d", *vals)
            zf.writestr(name, blob)
    print(f"wrote {out} with {len(SEEDS)} seeds")
    return 0


if __name__ == "__main__":
    sys.exit(main())
