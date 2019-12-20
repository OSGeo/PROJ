#!/usr/bin/env python
###############################################################################
# $Id$
#
#  Project:  PROJ
#  Purpose:  Tool to check consistency of database regarding grids and against
#            what is available in proj-datumgrid
#  Author:   Even Rouault <even.rouault at spatialys.com>
#
###############################################################################
#  Copyright (c) 2019, Even Rouault <even.rouault at spatialys.com>
#
#  Permission is hereby granted, free of charge, to any person obtaining a
#  copy of this software and associated documentation files (the "Software"),
#  to deal in the Software without restriction, including without limitation
#  the rights to use, copy, modify, merge, publish, distribute, sublicense,
#  and/or sell copies of the Software, and to permit persons to whom the
#  Software is furnished to do so, subject to the following conditions:
#
#  The above copyright notice and this permission notice shall be included
#  in all copies or substantial portions of the Software.
#
#  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
#  OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
#  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
#  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
#  DEALINGS IN THE SOFTWARE.
###############################################################################

import argparse
import csv
import fnmatch
import os
import sqlite3

parser = argparse.ArgumentParser(description='Check database and proj-datumgrid consistency.')
parser.add_argument('path_to_proj_db',
                    help='Full pathname to proj.db')
parser.add_argument('path_to_proj_datumgrid',
                    help='Full pathname to the root of the proj_datumgrid git repository')

group = parser.add_mutually_exclusive_group(required=True)
group.add_argument('--not-in-grid-alternatives', dest='not_in_grid_alternatives', action='store_true',
                   help='list grids mentionned in grid_transformation but missing in grid_alternatives')
group.add_argument('--not-in-proj-datumgrid', dest='not_in_proj_datum_grid', action='store_true',
                   help='list grids registered in grid_alternatives, but missing in proj-datumgrid')
group.add_argument('--not-in-db', dest='not_in_db', action='store_true',
                   help='list grids in proj-datumgrid, but not registered in grid_alternatives')
group.add_argument('--check-filelist', dest='check_filelist', action='store_true',
                   help='check consistency of proj-datumgrid filelist.csv')

args = parser.parse_args()

dbname = args.path_to_proj_db
proj_datumgrid = args.path_to_proj_datumgrid

if args.not_in_grid_alternatives:
    conn = sqlite3.connect(dbname)
    print('Authority, code, name, grid_name, is_superseded')
    res = conn.execute("""
        SELECT auth_name, code, name, grid_name, EXISTS (SELECT 1 FROM supersession WHERE superseded_table_name = 'grid_transformation' AND superseded_auth_name = auth_name AND superseded_code = code) AS superseded FROM grid_transformation
        WHERE deprecated = 0 AND
        NOT EXISTS (SELECT 1 FROM grid_alternatives WHERE original_grid_name = grid_name)""")
    for row in res:
        print(row)

elif args.not_in_proj_datum_grid:

    set_grids = set()
    for root, dirnames, filenames in os.walk(proj_datumgrid):
        for filename in fnmatch.filter(filenames, '*'):
            set_grids.add(filename)

    conn = sqlite3.connect(dbname)
    res = conn.execute("SELECT DISTINCT proj_grid_name FROM grid_alternatives WHERE open_license is NULL OR open_license != 0")
    for (grid_name,) in res:
        if grid_name not in set_grids:
            print('ERROR: grid ' + grid_name + ' in grid_alternatives but missing in proj-datumgrid')

elif args.not_in_db:

    set_grids = set()
    for root, dirnames, filenames in os.walk(proj_datumgrid):
        if '.git' in root:
            continue
        for filename in fnmatch.filter(filenames, '*'):
            filename_lower = filename.lower()
            if '.aux.xml' in filename_lower:
                continue
            if '.gsb' in filename_lower or '.gtx' in filename_lower:
                set_grids.add(filename)

    conn = sqlite3.connect(dbname)
    for filename in sorted(set_grids):
        res = conn.execute("SELECT 1 FROM grid_alternatives WHERE proj_grid_name = ?", (filename,))
        if not res.fetchone():
            print('WARNING: grid ' + filename + ' in proj-datumgrid but missing in grid_alternatives')

elif args.check_filelist:

    from osgeo import gdal

    set_grids = set()
    non_gsb_hgrids = ('ntv1_can.dat',
                      'alaska',
                      'conus',
                      'hawaii',
                      'prvi',
                      'stgeorge',
                      'stlrnc',
                      'stpaul',
                      'FL'.lower(),
                      'MD'.lower(),
                      'TN'.lower(),
                      'WI'.lower(),
                      'WO'.lower(),)
    for root, dirnames, filenames in os.walk(proj_datumgrid):
        if '.git' in root:
            continue
        for filename in fnmatch.filter(filenames, '*'):
            filename_lower = filename.lower()
            if '.aux.xml' in filename_lower:
                continue
            if '.gsb' in filename_lower or '.gtx' in filename_lower:
                set_grids.add(filename)
            elif filename_lower in non_gsb_hgrids:
                set_grids.add(filename)

    conn = sqlite3.connect(dbname)

    set_filenames_from_csv = set()
    with open(os.path.join(proj_datumgrid,'filelist.csv')) as f:
        reader = csv.reader(f)
        first_line = True
        for row in reader:
            if first_line:
                assert row == ['filename', 'type', 'unit', 'source_crs', 'target_crs', 'interpolation_crs', 'agency_name', 'source', 'licence']
                first_line = False
                continue
            filename, type, unit, source_crs, target_crs, interpolation_crs, _, _, _ = row
            if type in ('DEFORMATION_MODEL', 'VELOCITY_MODEL'):
                continue
            assert type in ('HORIZONTAL_OFFSET',
                            'VERTICAL_OFFSET_GEOGRAPHIC_TO_VERTICAL',
                            'VERTICAL_OFFSET_VERTICAL_TO_VERTICAL'), type
            set_filenames_from_csv.add(filename)

            assert filename in set_grids, filename
            if filename.lower().endswith('.gsb') or filename.lower() in non_gsb_hgrids:
                assert type == 'HORIZONTAL_OFFSET', (filename, type)
            else:
                assert type in ('VERTICAL_OFFSET_GEOGRAPHIC_TO_VERTICAL',
                                'VERTICAL_OFFSET_VERTICAL_TO_VERTICAL'), (filename, type)

            for dirname in ('.', 'europe', 'north-america', 'oceania', 'world'):
                filename_with_path_tmp = os.path.join(proj_datumgrid, dirname, filename)
                if os.path.exists(filename_with_path_tmp):
                    filename_with_path = filename_with_path_tmp
                    break
            assert filename_with_path

            ds = gdal.Open(filename_with_path)
            assert ds, filename
            gt = ds.GetGeoTransform()
            grid_w = gt[0]
            grid_n = gt[3]
            grid_e = gt[0] + gt[1] * ds.RasterXSize
            grid_s = gt[3] + gt[5] * ds.RasterYSize
            if grid_w > 180:
                grid_w -= 360
                grid_e -= 360

            source_crs_name = None
            target_crs_name = None

            if source_crs.startswith('EPSG:') or source_crs.startswith('IGNF:'):
                auth_name = source_crs[0:4]
                code = source_crs[len('EPSG:'):]
                res = conn.execute("SELECT name, table_name FROM crs_view WHERE auth_name = ? AND code = ?", (auth_name, code))
                source_crs_name, table_name = res.fetchone()
                if type == 'HORIZONTAL_OFFSET':
                    assert table_name == 'geodetic_crs', (filename, table_name, code)
                    res = conn.execute("SELECT type FROM geodetic_crs WHERE auth_name = ? AND code = ?", (auth_name, code))
                    geodetic_crs_type, = res.fetchone()
                    assert geodetic_crs_type == 'geographic 2D', (filename, geodetic_crs_type, code)
                elif type == 'VERTICAL_OFFSET_GEOGRAPHIC_TO_VERTICAL':
                    assert table_name == 'geodetic_crs', (filename, table_name, code)
                    res = conn.execute("SELECT type FROM geodetic_crs WHERE auth_name = ? AND code = ?", (auth_name, code))
                    geodetic_crs_type, = res.fetchone()
                    if code == '4269': # NAD 83
                        assert geodetic_crs_type == 'geographic 2D', (filename, geodetic_crs_type, code)
                    else:
                        assert geodetic_crs_type == 'geographic 3D', (filename, geodetic_crs_type, code)
                elif type == 'VERTICAL_OFFSET_VERTICAL_TO_VERTICAL':
                    assert table_name == 'vertical_crs', (filename, table_name, code)

                res = conn.execute("SELECT south_lat, north_lat, west_lon, east_lon FROM crs_view c, area a WHERE c.area_of_use_auth_name = a.auth_name AND c.area_of_use_code = a.code AND c.auth_name = ? AND c.code = ?", (auth_name, code))
                s, n, w, e = res.fetchone()
                if w > e:
                    if grid_w > 0:
                        e += 360
                    else:
                        w -= 360
                if filename not in ('c1hpgn.gsb', 'c2hpgn.gsb', 'guhpgn.gsb', 'g2009g01.gtx','g2009s01.gtx','g2012bg0.gtx', 'MAY76V20.gsb', ):
                    assert grid_w < e, (filename, source_crs, grid_w, e)
                    assert grid_e > w, (filename, source_crs, grid_e, w)
                    assert grid_s < n, (filename, source_crs, grid_s, n)
                    assert grid_n > s, (filename, source_crs, grid_n, s)

            else:
                assert False, (filename, source_crs)

            if target_crs.startswith('EPSG:') or target_crs.startswith('IGNF:'):
                auth_name = target_crs[0:4]
                code = target_crs[len('EPSG:'):]
                res = conn.execute("SELECT name, table_name FROM crs_view WHERE auth_name = ? AND code = ?", (auth_name, code))
                target_crs_name, table_name = res.fetchone()
                if type == 'HORIZONTAL_OFFSET':
                    assert table_name == 'geodetic_crs', (filename, table_name, code)
                    res = conn.execute("SELECT type FROM geodetic_crs WHERE auth_name = ? AND code = ?", (auth_name, code))
                    geodetic_crs_type, = res.fetchone()
                    assert geodetic_crs_type == 'geographic 2D', (filename, geodetic_crs_type, code)
                elif type in ('VERTICAL_OFFSET_GEOGRAPHIC_TO_VERTICAL', 'VERTICAL_OFFSET_VERTICAL_TO_VERTICAL'):
                    assert table_name == 'vertical_crs', (filename, table_name, code)

                res = conn.execute("SELECT south_lat, north_lat, west_lon, east_lon FROM crs_view c, area a WHERE c.area_of_use_auth_name = a.auth_name AND c.area_of_use_code = a.code AND c.auth_name = ? AND c.code = ?", (auth_name, code))
                s, n, w, e = res.fetchone()
                if w > e:
                    if grid_w > 0:
                        e += 360
                    else:
                        w -= 360
                if filename not in ('c1hpgn.gsb', 'c2hpgn.gsb', 'guhpgn.gsb', 'ggpf08-Fakarava.gtx'):
                    assert grid_w < e, (filename, target_crs, grid_w, e)
                    assert grid_e > w, (filename, target_crs, grid_e, w)
                    assert grid_s < n, (filename, target_crs, grid_s, n)
                    assert grid_n > s, (filename, target_crs, grid_n, s)

            elif target_crs.startswith('VERTCRS['):
                assert type == 'VERTICAL_OFFSET_GEOGRAPHIC_TO_VERTICAL', (filename, target_crs)
            else:
                assert False, (filename, target_crs)

            #print(filename, source_crs_name, target_crs_name)

    for f in set_grids:
        if f not in set_filenames_from_csv:
            print(f + ' is missing in filelist.csv')

else:
    raise Exception('unknown mode')
