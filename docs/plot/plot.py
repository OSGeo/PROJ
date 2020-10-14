'''
Plot map data in different projections supported by PROJ.4

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

from __future__ import print_function
from __future__ import division

import os
import os.path
import shutil
import sys
import json
import subprocess
import functools

import numpy as np
import matplotlib.pyplot as plt
from matplotlib.patches import Polygon
import fiona
from shapely.geometry import Polygon
from shapely.geometry import MultiPolygon
from shapely.geometry import shape
from shapely.ops import transform
from descartes import PolygonPatch

PROJ_COMMAND = os.environ.get('PROJ_EXE', '../../src/proj')
if not os.path.exists(PROJ_COMMAND):
    PROJ = shutil.which(PROJ_COMMAND)
else:
    PROJ = PROJ_COMMAND
PROJ_LIB = os.environ.get('PROJ_LIB', '../../data')

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
                                env={'PROJ_LIB': os.path.abspath(PROJ_LIB)})
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


def meridian(lon, lat_min, lat_max):
    '''
    Calculate meridian coordinates.
    '''
    coords = np.zeros((N_POINTS, 2))
    coords[:, 0] = lon
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

    for lon in range(lonmin, lonmax+1, GRATICULE_WIDTH):
        graticule.append(meridian(lon, latmin, latmax))

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


def plotproj(plotdef, data, outdir):
    '''
    Plot map.
    '''
    axes = plt.axes()

    bounds = (plotdef['lonmin'], plotdef['latmin'], plotdef['lonmax'], plotdef['latmax'])
    for geom in data.filter(bbox=bounds):
        temp_pol = shape(geom['geometry'])

        box = Polygon([
            (plotdef['lonmin'], plotdef['latmin']),
            (plotdef['lonmin'], plotdef['latmax']),
            (plotdef['lonmax'], plotdef['latmax']),
            (plotdef['lonmax'], plotdef['latmin']),
        ])
        try:
            temp_pol = temp_pol.intersection(box)
        except Exception as e:
            continue


        if plotdef['type'] == 'poly':
            if isinstance(temp_pol, MultiPolygon):
                polys = [resample_polygon(polygon) for polygon in temp_pol]
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
            axes.plot(x, y, color=COLOR_COAST, linewidth=0.5)

    # Plot frame
    frame = [
        parallel(plotdef['latmin'], plotdef['lonmin'], plotdef['lonmax']),
        parallel(plotdef['latmax'], plotdef['lonmin'], plotdef['lonmax']),
    ]
    for line in frame:
        line = project(line, plotdef['projstring'])
        x = line[:, 0]
        y = line[:, 1]
        axes.plot(x, y, '-k')

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
        axes.plot(x, y, color=COLOR_GRAT, linewidth=0.4)

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

    if not os.path.exists(outdir):
        os.makedirs(outdir)
    plt.savefig(outdir + '/' + plotdef['filename'],
                dpi=400,
                bbox_inches='tight')

    # Clean up
    axes = None
    del axes
    plt.close()


def main():
    '''
    Main function of plotting script.

    Parses json-file with plot setups and runs the plotting
    for each plot setup.
    '''

    data = {
        ('line', 'low'): fiona.open(LINE_LOW),
        ('line', 'med'): fiona.open(LINE_MED),
        ('poly', 'low'): fiona.open(POLY_LOW),
        ('poly', 'med'): fiona.open(POLY_MED),
    }

    if os.path.exists(sys.argv[1]):
        # first argument is the JSON plot definition setup file
        with open(sys.argv[1]) as plotsetup:
            plotdefs = json.load(plotsetup)
    else:
        raise ValueError('No plot definition file entered')

    plots = []
    # second argument is the output dir
    outdir = sys.argv[2]

    # subsecond arguments are (optional) names of plot in plotdef.json
    if len(sys.argv) > 3:
        plots = sys.argv[3:len(sys.argv)]

    for i, plotdef in enumerate(plotdefs):
        if plots != [] and plotdef['name'] not in plots:
            continue

        print(i, plotdef['projstring'])
        if 'skip' in plotdef.keys():
            print('skipping')
            continue

        plotproj(plotdef, data[(plotdef['type'], plotdef['res'])], outdir)

    for key in data:
        data[key].close()


if __name__ == "__main__":
    sys.exit(main())
