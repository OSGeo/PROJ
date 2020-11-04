#!/usr/bin/env python
###############################################################################
# $Id$
#
#  Project:  PROJ
#  Purpose:  Tool to check consistency of database regarding grids and against
#            what is available in PROJ-data
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
import fnmatch
import os
import sqlite3

parser = argparse.ArgumentParser(description='Check database and PROJ-data consistency.')
parser.add_argument('path_to_proj_db',
                    help='Full pathname to proj.db')
parser.add_argument('path_to_proj_data',
                    help='Full pathname to the root of the proj_data_geotiff git repository')

group = parser.add_mutually_exclusive_group(required=True)
group.add_argument('--not-in-grid-alternatives', dest='not_in_grid_alternatives', action='store_true',
                   help='list grids mentionned in grid_transformation but missing in grid_alternatives')
group.add_argument('--not-in-proj-data', dest='not_in_proj_data', action='store_true',
                   help='list grids registered in grid_alternatives, but missing in PROJ-data repository')
group.add_argument('--not-in-db', dest='not_in_db', action='store_true',
                   help='list grids in PROJ-data repository, but not registered in grid_alternatives')

args = parser.parse_args()

dbname = args.path_to_proj_db
proj_data = args.path_to_proj_data

if args.not_in_grid_alternatives:
    conn = sqlite3.connect(dbname)
    print('Authority, code, name, grid_name, is_superseded')
    res = conn.execute("""
        SELECT auth_name, code, name, grid_name, EXISTS (SELECT 1 FROM supersession WHERE superseded_table_name = 'grid_transformation' AND superseded_auth_name = auth_name AND superseded_code = code) AS superseded FROM grid_transformation
        WHERE deprecated = 0 AND
        NOT EXISTS (SELECT 1 FROM grid_alternatives WHERE original_grid_name = grid_name)""")
    for row in res:
        print(row)

elif args.not_in_proj_data:

    set_grids = set()
    for root, dirnames, filenames in os.walk(proj_data):
        for filename in fnmatch.filter(filenames, '*'):
            set_grids.add(filename)

    conn = sqlite3.connect(dbname)
    res = conn.execute("SELECT DISTINCT proj_grid_name FROM grid_alternatives WHERE open_license is NULL OR open_license != 0")
    for (grid_name,) in res:
        if grid_name not in set_grids:
            print('ERROR: grid ' + grid_name + ' in grid_alternatives but missing in PROJ-data')

elif args.not_in_db:

    set_grids = set()
    for root, dirnames, filenames in os.walk(proj_data):
        if '.git' in root:
            continue
        for filename in fnmatch.filter(filenames, '*'):
            filename_lower = filename.lower()
            if '.aux.xml' in filename_lower:
                continue
            if '.gsb' in filename_lower or '.gtx' in filename_lower or '.tif' in filename_lower:
                set_grids.add(filename)

    conn = sqlite3.connect(dbname)
    for filename in sorted(set_grids):
        res = conn.execute("SELECT 1 FROM grid_alternatives WHERE proj_grid_name = ?", (filename,))
        if not res.fetchone():
            print('WARNING: grid ' + filename + ' in PROJ-data but missing in grid_alternatives')
else:
    raise Exception('unknown mode')
