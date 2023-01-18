#!/usr/bin/env python3
'''
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
'''

import argparse
import os
import shutil
import sys
import json
import subprocess
import functools
from pathlib import Path

import geojson
import numpy as np
import matplotlib.pyplot as plt
from shapely.geometry import MultiPolygon, Polygon, box as Box
from shapely.geometry import shape
from shapely.ops import transform
from descartes import PolygonPatch

PROJ_COMMAND = os.environ.get('PROJ_EXE', '../../src/proj')
if not os.path.exists(PROJ_COMMAND):
    PROJ = shutil.which('proj')
    if PROJ is None:
        raise ValueError("specify PROJ_EXE or modify PATH to find proj")
else:
    PROJ = PROJ_COMMAND
PROJ_DATA = os.environ.get('PROJ_DATA', os.environ.get('PROJ_LIB', '../../data'))

LINE_LOW = 'data/coastline.geojson'
LINE_MED = 'data/coastline50.geojson'
POLY_LOW = 'data/land.geojson'
POLY_MED = 'data/land50.geojson'

GRATICULE_WIDTH = 15
N_POINTS = 1000

# colors
COLOR_LAND = '#000000'
COLOR_COAST = '#000000'
COLOR_GRAT = '#888888'


def interp_coords(coords, tol):
    '''
    Interpolates extra coordinates when the euclidean distance between adjacent
    points are larger than given tolerance.

    The function tries to densify a list of coordinates based on a simple measure.
    If two adjacent points in the input coordinate list are further away from each
    other than the specified tolerance, the list will be densified between those two
    points. Roughly speaking, a point will be inserted every `tol` between the
    points.

    Inputs:
    -------
    coords:         2xN numpy array with geodetic coordinates
    tol:            Euclidean distance tolerance.

    Returns:
    --------
    A densified numpy array of coordinates. If tol < 0 the input array is returned
    unchanged.

    '''
    if tol < 0:
        return coords

    x, y = coords
    x = np.array(x)
    y = np.array(y)

    dsts = np.hypot(np.diff(x), np.diff(y))
    # Points to the *beginning* of the segment.
    I = np.where(dsts > tol)[0]

    offset = 0
    xy = []
    for i in I:
        # Add points that are already below tolerance.
        xy.append((x[offset:i], y[offset:i]))

        # Interpolate between points above tolerance.
        n = int(np.ceil(dsts[i] / tol))
        x1 = np.linspace(x[i], x[i + 1], n + 1)
        y1 = np.linspace(y[i], y[i + 1], n + 1)
        xy.append((x1[:-1], y1[:-1]))

        offset = i + 1

    xy.append((x[offset:], y[offset:]))

    xy = np.concatenate(xy, axis=1)

    return xy.T


def project(coordinates, proj_string, in_radians=False):
    '''
    Project geographical coordinates

    Input:
    ------
        coordinates:        numpy ndarray of size (N,2) and type double.
                            longitude, latitude
        proj_string:        Definition of output projection

    Out:
    ----
        numpy ndarray with shape (N,2) with N pairs of 2D
        coordinates (x, y).
    '''

    # proj expects binary input to be in radians
    if not in_radians:
        coordinates = np.deg2rad(coordinates)
    coordinates = coordinates.astype(np.double, copy=False)

    # set up cmd call. -b for binary in/out
    args = [PROJ, '-b']
    args.extend(proj_string.split(' '))

    try:
        proc = subprocess.Popen(args, stdin=subprocess.PIPE, stdout=subprocess.PIPE,
                                env={'PROJ_DATA': os.path.abspath(PROJ_DATA),
                                     'PROJ_LIB': os.path.abspath(PROJ_DATA)})
    except FileNotFoundError as e:
        print("'proj' binary not found, set the PROJ_EXE environment variable "
              "to point to your local 'proj' binary --%s--" % e)
        exit(1)

    stdout, _ = proc.communicate(coordinates.tobytes(order='C'))

    out = np.frombuffer(stdout, dtype=np.double)
    return np.reshape(out, (-1, 2))


def project_xy(x, y, proj_string):
    '''
    Wrapper for project() that works with shapely.ops.transform().
    '''
    a = project(np.column_stack((x, y)), proj_string)
    return a.T


def meridian(longitude, lat_min, lat_max):
    '''
    Calculate meridian coordinates.
    '''
    coords = np.zeros((N_POINTS, 2))
    coords[:, 0] = longitude
    coords[:, 1] = np.linspace(lat_min, lat_max, N_POINTS)
    return coords


def parallel(lat, lon_min, lon_max):
    '''
    Calculate parallel coordinates.
    '''
    coords = np.zeros((N_POINTS, 2))
    coords[:, 0] = np.linspace(lon_min, lon_max, N_POINTS)
    coords[:, 1] = lat
    return coords


def build_graticule(lonmin=-180, lonmax=180, latmin=-85, latmax=85):
    '''
    Build graticule
    '''
    # we might get unwanted floats
    lonmin = int(lonmin)
    lonmax = int(lonmax)
    latmin = int(latmin)
    latmax = int(latmax)
    graticule = []

    for longitude in range(lonmin, lonmax+1, GRATICULE_WIDTH):
        graticule.append(meridian(longitude, latmin, latmax))

    for lat in range(latmin, latmax+1, GRATICULE_WIDTH):
        graticule.append(parallel(lat, lonmin, lonmax))

    return graticule


def resample_polygon(polygon):
    '''
    Use interp_coords() to resample (multi)polygons.
    '''
    try:
        xy = polygon.exterior.coords.xy
    except AttributeError: #no xy's
        return polygon
    ext = interp_coords(xy, 2)
    # interiors
    rings = []
    for int_ring in polygon.interiors:
        rings.append(interp_coords(int_ring.coords.xy, 2))
    return Polygon(ext, rings)


def plot_with_interruptions(axes, x, y, delta_cut=1e100, **kwargs):
    """
    Plot x/y with proper splitting for interrupted projections.
    """
    _x = np.atleast_1d(x)
    _y = np.atleast_1d(y)

    dx = np.nan_to_num(_x[1: ]) - np.nan_to_num(_x[0: -1])
    dy = np.nan_to_num(_y[1: ]) - np.nan_to_num(_y[0: -1])
    split, = ((np.abs(dx) > delta_cut) | (np.abs(dy) > delta_cut)).nonzero()
    split = np.append(split, _x.size - 1)
    split += 1

    last_part = 0
    for part in split:
        axes.plot(x[last_part: part],
                  y[last_part: part],
                  **kwargs)
        last_part = part


def plotproj(plotdef, data, outdir):
    '''
    Plot map.
    '''
    axes = plt.axes()

    box = Box(
        plotdef['lonmin'], plotdef['latmin'],
        plotdef['lonmax'], plotdef['latmax'])
    for feat in data["features"]:
        geom = shape(feat["geometry"])
        if not geom.intersects(box):
            continue

        temp_pol = geom.intersection(box)

        if plotdef['type'] == 'poly':
            if isinstance(temp_pol, MultiPolygon):
                polys = [resample_polygon(polygon) for polygon in temp_pol.geoms]
                pol = MultiPolygon(polys)
            else:
                pol = resample_polygon(temp_pol)
        else:
            pol = temp_pol

        trans = functools.partial(project_xy, proj_string=plotdef['projstring'])
        proj_geom = transform(trans, pol)

        if plotdef['type'] == 'poly':
            try:
                patch = PolygonPatch(proj_geom, fc=COLOR_LAND, zorder=0)
                axes.add_patch(patch)
            except TypeError:
                pass
        else:
            x, y = proj_geom.xy
            plot_with_interruptions(axes, x, y, color=COLOR_COAST, linewidth=0.5,
                                    delta_cut=plotdef.get('delta_cut', 1e100))

    # Plot frame
    frame = [
        parallel(plotdef['latmin'], plotdef['lonmin'], plotdef['lonmax']),
        parallel(plotdef['latmax'], plotdef['lonmin'], plotdef['lonmax']),
    ]
    for line in frame:
        line = project(line, plotdef['projstring'])
        x = line[:, 0]
        y = line[:, 1]
        plot_with_interruptions(axes, x, y, color='black', linestyle='-',
                                delta_cut=plotdef.get('delta_cut', 1e6))

    graticule = build_graticule(
        plotdef['lonmin'],
        plotdef['lonmax'],
        plotdef['latmin'],
        plotdef['latmax'],
    )

    # Plot graticule
    for feature in graticule:
        feature = project(feature, plotdef['projstring'])
        x = feature[:, 0]
        y = feature[:, 1]
        plot_with_interruptions(axes, x, y, color=COLOR_GRAT, linewidth=0.4,
                                delta_cut=plotdef.get('delta_cut', 1e6))

    # Plot interrupted boundaries if necessary
    interrupted_lines = []
    if 'top_interrupted_lons' in plotdef:
        for longitude in plotdef['top_interrupted_lons']:
            for delta in [-0.0001, 0.0001]:
                merid = meridian(longitude + delta, 0.0, plotdef['latmax'])
                interrupted_lines.append(project(merid, plotdef['projstring']))

    if 'bottom_interrupted_lons' in plotdef:
        for longitude in plotdef['bottom_interrupted_lons']:
            for delta in [-0.0001, 0.0001]:
                merid = meridian(longitude + delta, plotdef['latmin'], 0)
                interrupted_lines.append(project(merid, plotdef['projstring']))

    for line in interrupted_lines:
        x = line[:, 0]
        y = line[:, 1]
        plot_with_interruptions(axes, x, y, color=COLOR_GRAT, linewidth=0.4,
                                delta_cut=plotdef.get('delta_cut', 1e100))

    # Switch off the axis lines...
    plt.axis('off')
    # ... and additionally switch off the visibility of the axis lines and
    # labels so they can be removed by "bbox_inches='tight'" when saving
    axes.get_xaxis().set_visible(False)
    axes.get_yaxis().set_visible(False)

    font = {
        'family': 'serif',
        'color': 'black',
        'style': 'italic',
        'size': 12
    }

    # Make sure the plot is not stretched
    axes.set_aspect('equal')

    outdir = Path(outdir)
    if not outdir.exists():
        outdir.mkdir(parents=True)
    if not outdir.is_dir():
        raise OSError("outdir is not a directory")
    plt.savefig(outdir / plotdef['filename'],
                dpi=400,
                bbox_inches='tight')

    # Clean up
    axes = None
    del axes
    plt.close()


def main(plotdefs, outdir, plots=[]):
    '''
    Main function of plotting script.

    Parses json-file with plot setups and runs the plotting
    for each plot setup.
    '''
    plotdefs = json.loads(Path(plotdefs).read_text())

    data = {
        ('line', 'low'): geojson.loads(Path(LINE_LOW).read_text()),
        ('line', 'med'): geojson.loads(Path(LINE_MED).read_text()),
        ('poly', 'low'): geojson.loads(Path(POLY_LOW).read_text()),
        ('poly', 'med'): geojson.loads(Path(POLY_MED).read_text()),
    }

    for i, plotdef in enumerate(plotdefs):
        if plots != [] and plotdef['name'] not in plots:
            continue

        print(i, plotdef['projstring'])
        if 'skip' in plotdef.keys():
            print('skipping')
            continue

        plotproj(plotdef, data[(plotdef['type'], plotdef['res'])], outdir)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description=__doc__,
        formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument(
        'plotdefs',
        help='A JSON file with setup for each auto-generated plot')
    parser.add_argument(
        'outdir',
        help='Directory to put the plots in.')
    parser.add_argument(
        'plots', nargs='*',
        help='A list of plot names within the plotdefs file. '
        'More than one plotname can be given at once.')
    args = parser.parse_args()
    sys.exit(main(**vars(args)))
