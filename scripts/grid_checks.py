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

parser = argparse.ArgumentParser(
    description='Check database and PROJ-data consistency.')
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

parser.add_argument('--show-superseded', dest='show_superseded', action='store_true',
                    help='Show superseded grid transformations in --not-in-grid-alternatives mode')

args = parser.parse_args()

dbname = args.path_to_proj_db
proj_data = args.path_to_proj_data

if args.not_in_grid_alternatives:
    conn = sqlite3.connect(dbname)
    header = '"Authority","code","name","extent_name","grid_name"'
    if args.show_superseded:
        header += ',"is_superseded"'
    print(header)
    sql = """
        SELECT gt.auth_name, gt.code, gt.name, e.name, gt.grid_name,
        EXISTS (SELECT 1 FROM supersession WHERE
                superseded_table_name = 'grid_transformation' AND
                superseded_auth_name = gt.auth_name AND
                superseded_code = gt.code) AS superseded
        FROM grid_transformation gt
        JOIN usage u
        ON u.object_table_name = 'grid_transformation' AND
           u.object_auth_name = gt.auth_name AND
           u.object_code = gt.code
        JOIN extent e
        ON e.auth_name = u.extent_auth_name AND
           e.code = u.extent_code
        WHERE gt.deprecated = 0 AND
        NOT EXISTS (SELECT 1 FROM grid_alternatives WHERE original_grid_name = gt.grid_name)"""
    if not args.show_superseded:
        sql += " AND superseded = 0"
    res = conn.execute(sql)
    for row in res:
        if not args.show_superseded:
            row = [x for x in row][0:-1]
        print(','.join(['"' + str(x) + '"' for x in row]))

elif args.not_in_proj_data:

    set_grids = set()
    for root, dirnames, filenames in os.walk(proj_data):
        for filename in fnmatch.filter(filenames, '*'):
            set_grids.add(filename)

    conn = sqlite3.connect(dbname)
    res = conn.execute(
        "SELECT DISTINCT proj_grid_name FROM grid_alternatives WHERE open_license is NULL OR open_license != 0")
    for (grid_name,) in res:
        if grid_name not in set_grids:
            print('ERROR: grid ' + grid_name +
                  ' in grid_alternatives but missing in PROJ-data')

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
        res = conn.execute(
            "SELECT 1 FROM grid_alternatives WHERE proj_grid_name = ?", (filename,))
        if not res.fetchone():
            print('WARNING: grid ' + filename +
                  ' in PROJ-data but missing in grid_alternatives')
else:
    raise Exception('unknown mode')
