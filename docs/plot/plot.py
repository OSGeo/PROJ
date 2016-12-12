'''
Plot map data in different projections supported by proj.4

Can be called either as

> python plot.py definitions.txt [out]

or

> python plotproj.py "+proj=lcc +lat_1=20" [out]

The output dir is optional. Uses current dir if not set.

Change PROJ and COASTLINE_DATA to path on your local system
before running the script.
'''

import os
import sys
import json
import subprocess

import numpy as np
import matplotlib.pyplot as plt

PROJ = 'proj'

COASTLINE_DATA = 'data/coastline.geojson'

GRATICULE_WIDTH = 15
N_POINTS = 100

LAT_MIN = -90
LAT_MAX = 90
LON_MIN = -180
LON_MAX = 180

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

    # set up cmd call. -b for binary in/out
    args = [PROJ, '-b']
    args.extend(proj_string.split(' '))

    proc = subprocess.Popen(args, stdin=subprocess.PIPE, stdout=subprocess.PIPE)
    stdout, _ = proc.communicate(coordinates.tobytes())

    out = np.frombuffer(stdout, dtype=np.double)
    return np.reshape(out, coordinates.shape)

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

def plotproj(proj_str, coastline, frame, graticule, outdir):
    '''
    Plot map.
    '''
    fig = plt.figure()
    axes = fig.add_subplot(111)

    # Plot frame
    for line in frame:
        line = project(line, proj_str)
        x, y = zip(*line)
        plt.plot(x, y, '-k')

    # Plot coastline
    for feature in coastline['features']:
        C = np.array(feature['geometry']['coordinates'])

        if np.any(C[:, 0] > 180.0):
            C[C[:, 0] > 180.0, 0] = 180.0

        if np.any(C[:, 0] < -180.0):
            C[C[:, 0] < -180.0, 0] = -180.0

        if np.any(C[:, 1] > 90.0):
            C[C[:, 1] > 90.0, 1] = 90.0

        if np.any(C[:, 1] < -90.0):
            C[C[:, 1] < -90.0, 1] = -90.0

        coords = project(C, proj_str)
        x, y = zip(*coords)
        plt.plot(x, y, '-k', linewidth=0.5)

    # Plot graticule
    graticule = project(graticule, proj_str)
    for feature in graticule:
        x, y = zip(*feature)
        plt.plot(x, y, '-k', linewidth=0.2)

    axes.axis('off')
    font = {'family': 'serif',
            'color': 'black',
            'style': 'italic',
            'size': 12}
    plt.suptitle(proj_str, fontdict=font)

    plt.autoscale(tight=True)
    name = proj_str.split()[0].split('=')[1]
    plt.savefig(outdir + '/' + name + '.png', dpi=300)

    # Clean up
    fig = None
    del fig
    axes = None
    del axes
    plt.close()

if __name__ == "__main__":
    '''
    '''

    with open(COASTLINE_DATA) as data:
        coastline = json.load(data)

    if os.path.exists(sys.argv[1]):
        # it must be a file with proj-strings
        with open(sys.argv[1]) as projs:
            projs = projs.readlines()
    else:
        #assumes it is a single roj string
        projs = [sys.argv[1]]

    outdir = '.'
    if len(sys.argv) > 2:
        outdir = sys.argv[2]

    frame = []
    frame.append(parallel(LAT_MIN, LON_MIN, LON_MAX))
    frame.append(parallel(LAT_MAX, LON_MIN, LON_MAX))

    # build graticule
    graticule = []
    for lon in range(LON_MIN, LON_MAX+1, GRATICULE_WIDTH):
        graticule.append(meridian(lon, LAT_MIN, LAT_MAX))

    for lat in range(LAT_MIN, LAT_MAX+1, GRATICULE_WIDTH):
        graticule.append(parallel(lat, LON_MIN, LON_MAX))

    for i, proj in enumerate(projs):
        proj_str = proj.strip()
        print(i, proj_str)
        plotproj(proj_str, coastline, frame, graticule, outdir)
