#!/usr/bin/env python3
"""Deterministic seed generator for proj_trans_fuzzer.

Builds $OUT/proj_trans_fuzzer_seed_corpus.zip with structurally-valid inputs
for the harness input format:

    byte 0 flags | source CRS def \n | target CRS def \n | coords (x, y, z)

Flags: bit0 normalize, bit1 PJ_INV, bit2 proj_trans_array, bit3 proj_trans_bounds,
bit4 proj_roundtrip, bits5-7 densify_pts.

Run from the OSS-Fuzz build.sh: python3 test/fuzzers/generate_proj_trans_seeds.py $OUT/...
"""

import os
import struct
import sys
import zipfile


def coord_list(coords):
    return b"".join(struct.pack("<3d", x, y, z) for (x, y, z) in coords)


SEEDS = [
    ("epsg_4326_to_3857", "EPSG:4326", "EPSG:3857", [(2.0, 49.0, 0.0), (0.0, 0.0, 0.0)], 0x15),
    (
        "pipeline_helmert",
        "+proj=pipeline +step +proj=cart +ellps=WGS84 "
        "+step +proj=helmert +x=1 +y=2 +z=3 "
        "+step +proj=cart +inv +ellps=WGS84",
        "+proj=longlat +datum=WGS84",
        [(1.0, 2.0, 3.0), (0.1, 0.2, 0.3)],
        0x04,
    ),
    (
        "wkt2_geog_to_utm31n",
        'GEOGCRS["WGS 84",DATUM["World Geodetic System 1984",'
        'ELLIPSOID["WGS 84",6378137,298.257223563]],PRIMEM["Greenwich",0],'
        "CS[ellipsoidal,2],AXIS[\"lat\",north],AXIS[\"lon\",east],"
        'UNIT["degree",0.0174532925199433],ID["EPSG",4326]]',
        "EPSG:32631",
        [(12.0, 48.0, 0.0), (6.0, 50.0, 0.0)],
        0x05,
    ),
    (
        "projjson_wgs84",
        '{"type":"GeographicCRS","name":"WGS 84","datum":{"type":'
        '"GeodeticReferenceFrame","name":"WGS 84","ellipsoid":{"name":"WGS 84",'
        '"semi_major_axis":6378137,"inverse_flattening":298.257223563}},'
        '"coordinate_system":{"subtype":"ellipsoidal","axis":[{"name":"lon",'
        '"direction":"east","unit":"degree"},{"name":"lat","direction":"north",'
        '"unit":"degree"}]},"id":{"authority":"EPSG","code":4326}}',
        "EPSG:3857",
        [(3.0, 45.0, 0.0)],
        0x03,
    ),
    (
        "tmerc_grs80",
        "+proj=tmerc +lat_0=49 +lon_0=2 +k=0.9996 +x_0=500000 +ellps=GRS80 +units=m",
        "+proj=longlat +ellps=GRS80",
        [(2.0, 49.0, 0.0), (3.0, 50.0, 0.0)],
        0x0F,
    ),
    (
        "ortho_to_cart",
        "+proj=ortho +lat_0=55 +lon_0=10 +ellps=GRS80",
        "+proj=cart +ellps=GRS80",
        [(10.0, 55.0, 100000.0), (10.0, 89.0, 0.0)],
        0x10,
    ),
    (
        "aea_nad83",
        "+proj=aea +lat_1=20 +lat_2=60 +lat_0=40 +lon_0=-96 +x_0=0 +y_0=0 "
        "+datum=NAD83",
        "+proj=longlat +datum=WGS84",
        [(-96.0, 40.0, 0.0), (0.0, 0.0, 0.0)],
        0x0B,
    ),
    (
        "antipodal_pair",
        "EPSG:4326",
        "EPSG:4326",
        [(-179.999, 0.0, 0.0), (179.999, 0.0, 0.0)],
        0x1F,
    ),
    ("etrs89_to_utm32n", "EPSG:4258", "EPSG:25832", [(52.0, 10.0, 0.0)], 0x05),
    (
        "compound_vertical",
        "EPSG:4326+3855",
        "EPSG:4979",
        [(2.0, 49.0, 100.0)],
        0x14,
    ),
    (
        "geoid_null_grid",
        "+proj=longlat +datum=WGS84",
        "+proj=longlat +datum=WGS84 +geoidgrids=@null",
        [(2.0, 49.0, 10.0)],
        0x06,
    ),
    (
        "degenerate_coords",
        "EPSG:4326",
        "EPSG:3857",
        [(1e300, -1e300, 0.0), (float("inf"), 90.0, 0.0), (5e-324, float("-inf"), 0.0)],
        0x11,
    ),
]


def main():
    if len(sys.argv) != 2:
        print("usage: generate_proj_trans_seeds.py <output.zip>")
        return 1
    out = sys.argv[1]
    for name, src, dst, coords, flags in SEEDS:
        if not src or not dst or len(src) > 400 or len(dst) > 400:
            raise SystemExit(f"seed {name}: invalid CRS definitions "
                             f"(len(src)={len(src)}, len(dst)={len(dst)}, max 400)")
    os.makedirs(os.path.dirname(os.path.abspath(out)), exist_ok=True)
    with zipfile.ZipFile(out, "w", zipfile.ZIP_DEFLATED) as zf:
        for name, src, dst, coords, flags in SEEDS:
            blob = bytes([flags]) + src.encode() + b"\n" + dst.encode() + b"\n"
            blob += coord_list(coords)
            zf.writestr(name, blob)
    print(f"wrote {out} with {len(SEEDS)} seeds")
    return 0


if __name__ == "__main__":
    sys.exit(main())
