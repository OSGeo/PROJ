#!/usr/bin/env python3
"""Deterministic seed generator for the existing proj_crs_to_crs_fuzzer.

The production harness takes two lines: source CRS definition, then target
CRS definition (each <= 1000 bytes). Before this generator the fuzzer had NO
seed corpus; these seeds cover the main creation grammars (EPSG codes,
+proj pipelines, WKT2, PROJJSON, compound/Bound CRS) plus error-path
variants (missing code, malformed +proj, junk).

Run from the OSS-Fuzz build.sh: python3 test/fuzzers/generate_crs_seeds.py $OUT/...
"""

import os
import sys
import zipfile

SEEDS = [
    ("epsg_4326_to_3857", "EPSG:4326", "EPSG:3857"),
    ("epsg_4258_to_25832", "EPSG:4258", "EPSG:25832"),
    ("epsg_4326_to_4979", "EPSG:4326", "EPSG:4979"),
    (
        "tmerc_to_longlat",
        "+proj=tmerc +lat_0=49 +lon_0=2 +k=0.9996 +x_0=500000 +ellps=GRS80 +units=m",
        "+proj=longlat +ellps=GRS80 +datum=WGS84",
    ),
    (
        "pipeline_helmert",
        "+proj=pipeline +step +proj=cart +ellps=WGS84 "
        "+step +proj=helmert +x=1 +y=2 +z=3 "
        "+step +proj=cart +inv +ellps=WGS84",
        "+proj=longlat +datum=WGS84",
    ),
    (
        "wkt2_geog_to_epsg",
        'GEOGCRS["WGS 84",DATUM["World Geodetic System 1984",'
        'ELLIPSOID["WGS 84",6378137,298.257223563]],PRIMEM["Greenwich",0],'
        "CS[ellipsoidal,2],AXIS[\"lat\",north],AXIS[\"lon\",east],"
        'UNIT["degree",0.0174532925199433],ID["EPSG",4326]]',
        "EPSG:32631",
    ),
    (
        "wkt2_projcrs",
        'PROJCRS["WGS 84 / UTM zone 31N",CONVERSION["UTM zone 31N",'
        'METHOD["Transverse Mercator"],PARAMETER["Latitude of natural origin",0],'
        'PARAMETER["Longitude of natural origin",3],PARAMETER["Scale factor at '
        'natural origin",0.9996],PARAMETER["False easting",500000],'
        'PARAMETER["False northing",0]],CS[Cartesian,2],'
        'AXIS["(E)",east],AXIS["(N)",north],LENGTHUNIT["metre",1],ID["EPSG",32631]]',
        "EPSG:4326",
    ),
    (
        "projjson_to_epsg",
        '{"type":"GeographicCRS","name":"WGS 84","datum":{"type":'
        '"GeodeticReferenceFrame","name":"WGS 84","ellipsoid":{"name":"WGS 84",'
        '"semi_major_axis":6378137,"inverse_flattening":298.257223563}},'
        '"coordinate_system":{"subtype":"ellipsoidal","axis":[{"name":"lon",'
        '"direction":"east","unit":"degree"},{"name":"lat","direction":"north",'
        '"unit":"degree"}]},"id":{"authority":"EPSG","code":4326}}',
        "EPSG:3857",
    ),
    ("boundcrs_style", "EPSG:4326+3855", "EPSG:4979"),
    ("urn_syntax", "urn:ogc:def:crs:EPSG::4326", "urn:ogc:def:crs:EPSG::3857"),
    ("error_missing_code", "EPSG:999999", "EPSG:4326"),
    ("error_malformed_proj", "+proj=tmerc +invalid_param=@@@",
     "not-a-crs-definition"),
]


def main():
    if len(sys.argv) != 2:
        print("usage: generate_crs_seeds.py <output.zip>")
        return 1
    out = sys.argv[1]
    for name, src, dst in SEEDS:
        if not src or not dst or len(src) > 1000 or len(dst) > 1000:
            raise SystemExit(f"seed {name}: invalid definitions")
    os.makedirs(os.path.dirname(os.path.abspath(out)), exist_ok=True)
    with zipfile.ZipFile(out, "w", zipfile.ZIP_DEFLATED) as zf:
        for name, src, dst in SEEDS:
            zf.writestr(name, src.encode() + b"\n" + dst.encode())
    print(f"wrote {out} with {len(SEEDS)} seeds")
    return 0


if __name__ == "__main__":
    sys.exit(main())
