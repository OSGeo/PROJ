#!/usr/bin/env -S uv run --script
# /// script
# requires-python = ">=3.9"
# dependencies = [
#     "numpy",
#     "matplotlib",
# ]
# ///
"""Render a polyhedral net projection to an image.

Usage:
    uv run test/render_net.py --proj="+proj=tsea +R=1" \\
        --title="TSEA" --figsize=16,10 -o tsea_net.png
"""

import argparse
import ctypes
import json
import math
import os

import numpy as np

# ============================================================
# ctypes setup
# ============================================================

_lib = ctypes.CDLL(os.path.join(os.path.dirname(__file__), "..", "build",
                                 "lib", "libproj.dylib"))
_lib.proj_context_create.restype = ctypes.c_void_p
_lib.proj_create.restype = ctypes.c_void_p
_lib.proj_create.argtypes = [ctypes.c_void_p, ctypes.c_char_p]


class PJ_UV(ctypes.Structure):
    _fields_ = [("u", ctypes.c_double), ("v", ctypes.c_double)]


class PJ_COORD(ctypes.Union):
    _fields_ = [("lp", PJ_UV), ("xy", PJ_UV), ("v", ctypes.c_double * 4)]


_lib.proj_trans.restype = PJ_COORD
_lib.proj_trans.argtypes = [ctypes.c_void_p, ctypes.c_int, PJ_COORD]
PJ_FWD = 1

# ============================================================
# Projection and segment extraction
# ============================================================

# Densification step in degrees. With 0.5° steps, within-face
# projected distances are ~0.008 while cross-face jumps are >0.5,
# so a gap threshold of 0.05 cleanly separates them.
DENSIFY_STEP = 0.5
MAX_GAP = 0.05


def proj_forward_array(pj, lons, lats):
    n = len(lons)
    xs = np.empty(n)
    ys = np.empty(n)
    c = PJ_COORD()
    for i in range(n):
        c.lp.u = math.radians(lons[i])
        c.lp.v = math.radians(lats[i])
        r = _lib.proj_trans(pj, PJ_FWD, c)
        xs[i] = r.xy.u
        ys[i] = r.xy.v
    return xs, ys


def densify(lons, lats, max_step=DENSIFY_STEP):
    out_lon = [lons[0]]
    out_lat = [lats[0]]
    for i in range(1, len(lons)):
        d = max(abs(lons[i] - lons[i - 1]), abs(lats[i] - lats[i - 1]))
        n = max(1, int(math.ceil(d / max_step)))
        for j in range(1, n + 1):
            t = j / n
            out_lon.append(lons[i - 1] + t * (lons[i] - lons[i - 1]))
            out_lat.append(lats[i - 1] + t * (lats[i] - lats[i - 1]))
    return np.array(out_lon), np.array(out_lat)


def extract_segments(xs, ys, max_gap=MAX_GAP):
    """Break line into segments, cutting at large gaps (face boundaries)."""
    segments = []
    current = []
    max_gap_sq = max_gap * max_gap
    for i in range(len(xs)):
        if np.isnan(xs[i]) or np.isinf(xs[i]):
            if len(current) > 1:
                segments.append(np.array(current))
            current = []
            continue
        if current and ((xs[i] - current[-1][0]) ** 2 +
                        (ys[i] - current[-1][1]) ** 2) > max_gap_sq:
            if len(current) > 1:
                segments.append(np.array(current))
            current = []
        current.append([xs[i], ys[i]])
    if len(current) > 1:
        segments.append(np.array(current))
    return segments


def process_line(pj, lons, lats):
    lons, lats = densify(lons, lats)
    xs, ys = proj_forward_array(pj, lons, lats)
    return extract_segments(xs, ys)


# ============================================================
# Main
# ============================================================

def main():
    parser = argparse.ArgumentParser(description="Render polyhedral net projection")
    parser.add_argument("--proj", required=True,
                        help="Full PROJ string, e.g. '+proj=tsea +R=1'")
    parser.add_argument("--title", required=True, help="Plot title")
    parser.add_argument("--resolution", type=float, default=1.0,
                        help="Output resolution multiplier (default: 1)")
    parser.add_argument("-o", "--output", required=True, help="Output filename")
    args = parser.parse_args()

    figsize = (10 * args.resolution, 10 * args.resolution)

    import matplotlib
    matplotlib.use("Agg")
    import matplotlib.pyplot as plt
    from matplotlib.collections import LineCollection

    ctx = _lib.proj_context_create()
    pj = _lib.proj_create(ctx, args.proj.encode())
    assert pj, f"Failed to create projection: {args.proj}"

    fig, ax = plt.subplots(1, 1, figsize=figsize)

    # Graticule
    print("Projecting graticule...")
    grat_segs = []
    for lat in range(-80, 90, 10):
        lo = np.linspace(-180, 180, 720)
        la = np.full_like(lo, float(lat))
        grat_segs.extend(process_line(pj, lo, la))
    for lon in range(-180, 180, 10):
        la = np.linspace(-89, 89, 360)
        lo = np.full_like(la, float(lon))
        grat_segs.extend(process_line(pj, lo, la))
    print(f"  {len(grat_segs)} graticule segments")

    if grat_segs:
        lc = LineCollection(grat_segs, colors="lightgray", linewidths=0.3)
        ax.add_collection(lc)

    # Coastlines
    coast_path = os.path.join(os.path.dirname(__file__), "..", "docs", "plot",
                              "data", "coastline.geojson")
    print("Projecting coastlines...")
    coast_segs = []
    if os.path.exists(coast_path):
        with open(coast_path) as f:
            gj = json.load(f)
        for feat in gj["features"]:
            geom = feat["geometry"]
            rings = []
            if geom["type"] == "LineString":
                rings.append(geom["coordinates"])
            elif geom["type"] == "MultiLineString":
                rings.extend(geom["coordinates"])
            for ring in rings:
                coords = np.array(ring)
                coast_segs.extend(
                    process_line(pj, coords[:, 0], coords[:, 1]))
    print(f"  {len(coast_segs)} coastline segments")

    if coast_segs:
        lc = LineCollection(coast_segs, colors="black", linewidths=0.5)
        ax.add_collection(lc)

    ax.set_aspect("equal")
    ax.autoscale_view()
    ax.set_title(args.title, fontsize=14, pad=12)
    ax.axis("off")
    fig.tight_layout()

    fig.savefig(args.output, dpi=200, bbox_inches="tight")
    print(f"Saved {args.output}")


if __name__ == "__main__":
    main()
