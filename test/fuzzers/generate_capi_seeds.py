#!/usr/bin/env python3
"""Deterministic seed generator for proj_c_api_fuzzer.

Input layout: definition lines separated by '\n', then a final '\n', then the
op stream (1..64 bytes, op = b % 14, line = (b / 14) % nlines). Seeds cover
creation grammars (EPSG code, +proj pipeline, WKT2, PROJJSON, database codes)
and op streams that touch every opcode.
"""

import os
import sys
import zipfile

LINES = [
    "EPSG:4326",
    "EPSG:3857",
    "+proj=pipeline +step +proj=cart +ellps=WGS84 +step +proj=helmert +x=1",
    'GEOGCRS["WGS 84",DATUM["World Geodetic System 1984",'
    'ELLIPSOID["WGS 84",6378137,298.257223563]],PRIMEM["Greenwich",0],'
    "CS[ellipsoidal,2],AXIS[\"lat\",north],AXIS[\"lon\",east],"
    'UNIT["degree",0.0174532925199433],ID["EPSG",4326]]',
    '{"type":"GeographicCRS","name":"WGS 84","datum":{"type":'
    '"GeodeticReferenceFrame","name":"WGS 84","ellipsoid":{"name":"WGS 84",'
    '"semi_major_axis":6378137,"inverse_flattening":298.257223563}},'
    '"coordinate_system":{"subtype":"ellipsoidal","axis":[{"name":"lon",'
    '"direction":"east","unit":"degree"},{"name":"lat","direction":"north",'
    '"unit":"degree"}]},"id":{"authority":"EPSG","code":4326}}',
    "4326",   # database code for proj_create_from_database
    "3857",
    "EPSG:32631",
]


def ops_bytes(opcode_list):
    """Encode (op, line-index) pairs into op-stream bytes."""
    out = bytearray()
    for op, line_idx in opcode_list:
        b = op
        # find a byte value with b % 14 == op and (b // 14) % 8 == line_idx
        for cand in range(op, 256, 14):
            if (cand // 14) % 8 == line_idx:
                b = cand
                break
        out.append(b)
    return bytes(out)


SEEDS = [
    ("all_ops", ops_bytes([(i, i % 8) for i in range(14)])),
    ("create_then_marshal", ops_bytes([(0, 3), (0, 4), (8, 0), (6, 0), (7, 0), (11, 0)])),
    ("database_and_identify", ops_bytes([(1, 5), (1, 6), (5, 0), (10, 0), (4, 0), (9, 0)])),
    ("clone_normalize_source", ops_bytes([(0, 0), (2, 0), (3, 0), (12, 0), (13, 0), (11, 0)])),
    ("pipeline_ops", ops_bytes([(0, 2), (7, 0), (8, 0), (9, 0)])),
    ("long_sequence", ops_bytes([(i % 14, (i * 3) % 8) for i in range(40)])),
]


def main():
    if len(sys.argv) != 2:
        print("usage: generate_capi_seeds.py <output.zip>")
        return 1
    out = sys.argv[1]
    for line in LINES:
        if len(line) > 400:
            raise SystemExit(f"line too long ({len(line)} > 400)")
    os.makedirs(os.path.dirname(os.path.abspath(out)), exist_ok=True)
    with zipfile.ZipFile(out, "w", zipfile.ZIP_DEFLATED) as zf:
        for name, ops in SEEDS:
            blob = ("\n".join(LINES) + "\n").encode() + ops
            zf.writestr(name, blob)
    print(f"wrote {out} with {len(SEEDS)} seeds")
    return 0


if __name__ == "__main__":
    sys.exit(main())
