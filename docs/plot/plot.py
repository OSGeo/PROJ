#!/usr/bin/env python3
"""
Plot map data in different projections supported by PROJ.

Call with:

> python plot.py plotdefs outdir [plotname, [plotname, [plotname, ...]]]

The output dir is optional. Uses current dir if not set.

Inputs:
------

    plotdefs:       A JSON file with setup for each auto-generated plot.
    outdir:         Directory to put the plots in.
    plotname:       A list of plot names within the plotdefs file. More
                    than one plotname can be given at once.

Plot definitions:
----------------

Plots are defined by a JSON dict. A plot is defined by a name, filename,
bounding box (lat/long space), a proj-string, plottype and resolution.
An example of the setup for the plot for the Airy projection is shown below:

    {
        "filename": "airy.png",
        "latmax": 90,
        "latmin": -90,
        "lonmax": 90,
        "lonmin": -90,
        "name": "airy",
        "projstring": "+proj=airy",
        "res": "low",
        "type": "poly"
    }

"res" can be either "low" or "med" and "type" can be either "poly"(gon)
or "line". The rest of the inputs are fairly free form.


Change PROJ to path on your local system before running the script.
"""

import json
import os
import shutil
import subprocess
import sys
from pathlib import Path

import geojson
import matplotlib.pyplot as plt
import numpy as np
import shapely
from shapely.geometry import box as Box
from shapely.geometry import shape

try:
    from shapely.plotting import patch_from_polygon
except ImportError:
    raise ImportError("shapely>=2 required")

PROJ_COMMAND = os.environ.get("PROJ_EXE", "../../src/proj")
if not os.path.exists(PROJ_COMMAND):
    PROJ = shutil.which("proj")
    if PROJ is None:
        raise FileNotFoundError("specify PROJ_EXE or modify PATH to find proj")
else:
    PROJ = PROJ_COMMAND
PROJ_DATA = os.environ.get("PROJ_DATA", os.environ.get("PROJ_LIB", "../../data"))
PROJ_ENV = {
    "PROJ_DATA": os.path.abspath(PROJ_DATA),
    "PROJ_LIB": os.path.abspath(PROJ_DATA),
}
LINE_LOW = "data/coastline.geojson"
LINE_MED = "data/coastline50.geojson"
POLY_LOW = "data/land.geojson"
POLY_MED = "data/land50.geojson"

GRATICULE_WIDTH = 15
N_POINTS = 1000

# colors
COLOR_LAND = "#000000"
COLOR_COAST = "#000000"
COLOR_GRAT = "#888888"


def run_proj_cmd(args=[], **kwargs):
    """Run PROJ command with optional args.

    Parameters
    ----------
    args : list
        Zero or more arguments.
    **kwargs : dict
        Passed to ``subprocess.run``.

    Returns
    -------
    subprocess.CompletedProcess
    """
    args = [PROJ] + args
    proc = subprocess.run(args, capture_output=True, env=PROJ_ENV, **kwargs)
    proc.check_returncode()
    return proc


def geojson2geom_array(file):
    """Read GeoJSON file, return array of geometries."""
    features = geojson.loads(Path(file).read_text())["features"]
    return np.fromiter((shape(feat["geometry"]) for feat in features), object)


def project(coordinates, proj_string, in_radians=False):
    """Project geographical coordinates.

    Parameters
    ----------
    coordinates : array_like
        NumPy ndarray of size (N, 2) describing longitude, latitude.
    proj_string : str
        Definition of output projection.

    Returns
    -------
    array_like
        NumPy ndarray with shape (N, 2) with N pairs of 2D
        coordinates (x, y).
    """
    # proj expects binary input to be in radians
    if not in_radians:
        coordinates = np.deg2rad(coordinates, order="C", dtype=np.float64)
    else:
        coordinates = coordinates.astype(np.float64, order="C", copy=False)

    # set up cmd call. -b for binary in/out
    args = ["-b"] + proj_string.split(" ")
    proc = run_proj_cmd(args, input=coordinates.tobytes(order="C"))

    out = np.frombuffer(proc.stdout, dtype=np.float64)
    return np.reshape(out, (-1, 2))


def project_geoms(geoms, proj_string):
    """Projects array of geometries."""
    in_coords = shapely.get_coordinates(geoms)
    pj_coords = project(in_coords, proj_string=proj_string)
    return shapely.set_coordinates(geoms.copy(), pj_coords)


def meridian(longitude, lat_min, lat_max):
    """Return LineString for a meridian."""
    lons = np.repeat(longitude, N_POINTS)
    lats = np.linspace(lat_min, lat_max, N_POINTS)
    return shapely.linestrings(np.stack((lons, lats)).T)


def parallel(lat, lon_min, lon_max):
    """Return LineString for a parallel."""
    lons = np.linspace(lon_min, lon_max, N_POINTS)
    lats = np.repeat(lat, N_POINTS)
    return shapely.linestrings(np.stack((lons, lats)).T)


def build_graticule(lonmin=-180, lonmax=180, latmin=-85, latmax=85):
    """Return array of LineString for graticule."""
    # we might get unwanted floats
    lonmin = int(lonmin)
    lonmax = int(lonmax)
    latmin = int(latmin)
    latmax = int(latmax)
    graticule = []

    for longitude in range(lonmin, lonmax + 1, GRATICULE_WIDTH):
        graticule.append(meridian(longitude, latmin, latmax))

    for lat in range(latmin, latmax + 1, GRATICULE_WIDTH):
        graticule.append(parallel(lat, lonmin, lonmax))

    return np.array(graticule)


def plot_with_interruptions(axes, x, y, delta_cut=1e100, **kwargs):
    """Plot x/y with proper splitting for interrupted projections."""
    _x = np.atleast_1d(x)
    _y = np.atleast_1d(y)

    dx = np.nan_to_num(_x[1:]) - np.nan_to_num(_x[0:-1])
    dy = np.nan_to_num(_y[1:]) - np.nan_to_num(_y[0:-1])
    (split,) = ((np.abs(dx) > delta_cut) | (np.abs(dy) > delta_cut)).nonzero()
    split = np.append(split, _x.size - 1)
    split += 1

    last_part = 0
    for part in split:
        axes.plot(x[last_part:part], y[last_part:part], **kwargs)
        last_part = part


def plotproj(plotdef, geoms, outdir):
    """Plot map."""
    axes = plt.axes()

    box = Box(
        plotdef["lonmin"], plotdef["latmin"], plotdef["lonmax"], plotdef["latmax"]
    )

    # Filter to geometries that intersect the bounds; evaluate intersection
    geoms = geoms[box.intersects(geoms)]
    geoms = box.intersection(geoms)

    # Add extra vertices for segments greater than 2.0 degrees
    geoms = shapely.segmentize(geoms, 2.0)

    # Reproject all geometries
    geoms = project_geoms(geoms, proj_string=plotdef["projstring"])

    # Plot each geometry
    for geom in geoms:
        if plotdef["type"] == "poly":
            patch = patch_from_polygon(geom, color=COLOR_LAND, zorder=0)
            axes.add_patch(patch)
        else:
            plot_with_interruptions(
                axes,
                *geom.xy,
                color=COLOR_COAST,
                linewidth=0.5,
                delta_cut=plotdef.get("delta_cut", 1e100),
            )

    # Plot frame
    frame = np.array(
        [
            parallel(plotdef["latmin"], plotdef["lonmin"], plotdef["lonmax"]),
            parallel(plotdef["latmax"], plotdef["lonmin"], plotdef["lonmax"]),
        ]
    )
    frame = project_geoms(frame, proj_string=plotdef["projstring"])
    for line in frame:
        plot_with_interruptions(
            axes,
            *line.xy,
            color="black",
            linestyle="-",
            delta_cut=plotdef.get("delta_cut", 1e6),
        )

    # Plot graticule
    graticule = build_graticule(
        plotdef["lonmin"],
        plotdef["lonmax"],
        plotdef["latmin"],
        plotdef["latmax"],
    )
    graticule = project_geoms(graticule, proj_string=plotdef["projstring"])
    for line in graticule:
        plot_with_interruptions(
            axes,
            *line.xy,
            color=COLOR_GRAT,
            linewidth=0.4,
            delta_cut=plotdef.get("delta_cut", 1e6),
        )

    # Plot interrupted boundaries if necessary
    interrupted_lines = []
    if "top_interrupted_lons" in plotdef:
        for longitude in plotdef["top_interrupted_lons"]:
            for delta in [-0.0001, 0.0001]:
                merid = meridian(longitude + delta, 0.0, plotdef["latmax"])
                interrupted_lines.append(merid)

    if "bottom_interrupted_lons" in plotdef:
        for longitude in plotdef["bottom_interrupted_lons"]:
            for delta in [-0.0001, 0.0001]:
                merid = meridian(longitude + delta, plotdef["latmin"], 0)
                interrupted_lines.append(merid)

    if interrupted_lines:
        interrupted_lines = project_geoms(
            np.array(interrupted_lines), proj_string=plotdef["projstring"]
        )
        for line in interrupted_lines:
            plot_with_interruptions(
                axes,
                *line.xy,
                color=COLOR_GRAT,
                linewidth=0.4,
                delta_cut=plotdef.get("delta_cut", 1e100),
            )

    # Switch off the axis lines...
    plt.axis("off")
    # ... and additionally switch off the visibility of the axis lines and
    # labels so they can be removed by "bbox_inches='tight'" when saving
    axes.get_xaxis().set_visible(False)
    axes.get_yaxis().set_visible(False)

    # Make sure the plot is not stretched
    axes.set_aspect("equal")

    plt.savefig(outdir / plotdef["filename"], dpi=400, bbox_inches="tight")

    # Clean up
    axes = None
    del axes
    plt.close()


def main(plotdefs, outdir, plots=[]):
    """Run main function of plotting script.

    Parses json-file with plot setups and runs the plotting
    for each plot setup.
    """
    try:
        # Check 'proj' command, show version
        proc = run_proj_cmd(text=True)
        print(proc.stderr.splitlines()[0])
    except (FileNotFoundError, OSError) as e:
        sys.stderr.write(
            "'proj' binary not found, set the PROJ_EXE environment variable "
            "to point to your local 'proj' binary\n{}\n".format(e)
        )
        return 1

    outdir = Path(outdir)
    if not outdir.exists():
        outdir.mkdir(parents=True)
    if not outdir.is_dir():
        raise OSError("outdir is not a directory")

    plotdefs = json.loads(Path(plotdefs).read_text())

    data = {
        ("line", "low"): geojson2geom_array(LINE_LOW),
        ("line", "med"): geojson2geom_array(LINE_MED),
        ("poly", "low"): geojson2geom_array(POLY_LOW),
        ("poly", "med"): geojson2geom_array(POLY_MED),
    }

    for i, plotdef in enumerate(plotdefs):
        if plots != [] and plotdef["name"] not in plots:
            continue

        print("{}: {!r} {}".format(i, plotdef["filename"], plotdef["projstring"]))
        if "skip" in plotdef.keys():
            print("skipping")
            continue

        plotproj(plotdef, data[(plotdef["type"], plotdef["res"])], outdir)


if __name__ == "__main__":
    import argparse

    parser = argparse.ArgumentParser(
        description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter
    )
    parser.add_argument(
        "plotdefs", help="A JSON file with setup for each auto-generated plot"
    )
    parser.add_argument("outdir", help="Directory to put the plots in.")
    parser.add_argument(
        "plots",
        nargs="*",
        help="A list of plot names within the plotdefs file. "
        "More than one plotname can be given at once.",
    )
    args = parser.parse_args()
    sys.exit(main(**vars(args)))
