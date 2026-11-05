#!/usr/bin/env python3
"""Deterministic seed generator for proj_grid_fuzzer.

Wraps REAL test grids from the PROJ source tree (data/tests/) into harness
inputs so the fuzzer starts from valid NTv2/GTX/GeoTIFF/ctable2/defmodel/
tinshift files, plus two error-path seeds (truncated TIFF, junk bytes).

Input layout: flags byte | definition line \n | 48 bytes (2 x,y,z triples) |
grid blob.

Run from the OSS-Fuzz build.sh:
    python3 test/fuzzers/generate_grid_seeds.py $OUT/proj_grid_fuzzer_seed_corpus.zip data/tests
"""

import os
import struct
import sys
import zipfile

# (seed name, source file in data/tests/, definition, (x, y, z), direction-inv)
SEEDS = [
    ("ntv2_be", "test_hgrid_big_endian.gsb",
     "+proj=hgridshift +grids=fuzzgrid.gsb +ellps=GRS80", (2.0, 49.0, 0.0), 0),
    ("ntv2_downsampled", "ntv2_0_downsampled.gsb",
     "+proj=hgridshift +grids=fuzzgrid.gsb +ellps=GRS80", (2.0, 49.0, 0.0), 0),
    ("gtx_egm96", "egm96_15_downsampled.gtx",
     "+proj=vgridshift +grids=fuzzgrid.gtx +ellps=GRS80", (2.0, 49.0, 0.0), 0),
    ("gtx_tiny", "nkgrf03vel_realigned_z_extract.gtx",
     "+proj=vgridshift +grids=fuzzgrid.gtx +ellps=GRS80", (25.0, 60.0, 0.0), 0),
    ("geotiff_hgrid", "subset_of_gr3df97a.tif",
     "+proj=hgridshift +grids=fuzzgrid.tif +ellps=GRS80", (3.0, 46.0, 0.0), 0),
    ("geotiff_projected", "test_gridshift_projected.tif",
     "+proj=hgridshift +grids=fuzzgrid.tif +ellps=GRS80", (700000.0, 4600000.0, 0.0), 0),
    ("ctable2_xy", "nkgrf03vel_realigned_xy_extract.ct2",
     "+proj=hgridshift +grids=fuzzgrid.ct2 +ellps=GRS80", (25.0, 60.0, 0.0), 0),
    ("defmodel_degree", "simple_model_degree_horizontal.json",
     "+proj=defmodel +model=fuzzgrid.json +ellps=GRS80", (2.0, 49.0, 0.0), 0),
    ("defmodel_metre_inv", "simple_model_metre_horizontal.json",
     "+proj=defmodel +model=fuzzgrid.json +ellps=GRS80", (700000.0, 4600000.0, 0.0), 1),
    ("tinshift_gpkg", "tinshift_simplified_n60_n2000.gpkg",
     "+proj=tinshift +type=gpkg +model=fuzzgrid.gpkg +ellps=GRS80", (25.0, 60.0, 0.0), 0),
    ("truncated_tif_error", "egm96_15_uncompressed_truncated.tif",
     "+proj=vgridshift +grids=fuzzgrid.tif +ellps=GRS80", (2.0, 49.0, 0.0), 0),
    ("junk_blob_error", None,
     "+proj=hgridshift +grids=fuzzgrid.gsb +ellps=GRS80", (2.0, 49.0, 0.0), 0),
]


def main():
    if len(sys.argv) != 3:
        print("usage: generate_grid_seeds.py <output.zip> <data/tests dir>")
        return 1
    out, tests_dir = sys.argv[1], sys.argv[2]
    os.makedirs(os.path.dirname(os.path.abspath(out)), exist_ok=True)
    with zipfile.ZipFile(out, "w", zipfile.ZIP_DEFLATED) as zf:
        for name, src, defn, (x, y, z), inv in SEEDS:
            if src is not None:
                path = os.path.join(tests_dir, src)
                if not os.path.exists(path):
                    continue  # shipped grid set may evolve; skip silently
                with open(path, "rb") as f:
                    blob = f.read()
            else:
                blob = bytes(range(256)) * 4  # junk that is no valid grid
            head = bytes([inv]) + defn.encode() + b"\n"
            head += struct.pack("<6d", x, y, z, x + 0.5, y + 0.5, 0.0)
            zf.writestr(name, head + blob)
    print(f"wrote {out} from {tests_dir}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
