#!/usr/bin/env python
###############################################################################
# $Id$
#
#  Project:  PROJ
#  Purpose:  Build SRS and coordinate transform database
#  Author:   Even Rouault <even.rouault at spatialys.com>
#
###############################################################################
#  Copyright (c) 2018, Even Rouault <even.rouault at spatialys.com>
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

import copy
import json
import os
import sqlite3
import sys

if len(sys.argv) != 3:
    print('Usage: build_db_create_ignf.py path_to_IGNF_file proj.db')
    sys.exit(1)

def escape_literal(x):
    return x.replace("'", "''")

def find_ellipsoid(cursor, line, d):
    for prec in (10,9,8,7,6):
        cursor.execute('SELECT code, name FROM ellipsoid WHERE code != 1024 AND semi_major_axis = ? AND (inv_flattening = ? OR (abs(semi_major_axis / (semi_major_axis - semi_minor_axis)) - ?) < 1e-%d)' % prec, (float(d['a']), float(d['rf']), float(d['rf'])))
        row = cursor.fetchone()
        if prec > 6 and row is None:
            continue
        assert row, line
        ellps_code, ellps_name = row
        return ellps_code, ellps_name

def find_geogcrs_code(line,d):
    pm = None
    if 'pm' in d:
        pm = d['pm']
        assert pm == '2.337229167', pm

    key = { 'a' : d['a'], 'rf' : d['rf'], 'towgs84' : d['towgs84'], 'pm': pm }
    key_geog_crs = json.dumps(key)
    if 'W84' in code:
        geogcrs_code = "WGS84G"
    elif 'RGFG95' in code:
        geogcrs_code = "RGFG95GEO"
    elif 'ETRS89' in code:
        geogcrs_code = "ETRS89GEO"
    elif 'RGF93' in code:
        geogcrs_code = "RGF93G"
    elif 'LAMB93' in code:
        geogcrs_code = "RGF93G"
    elif 'MILLER' in code:
        geogcrs_code = "WGS84G"
    elif code.startswith('GEOPORTAL'):
        geogcrs_code = 'RGF93G'
    elif key_geog_crs not in map_geog_crs:
        max_count_common = 0
        max_common_code = None
        for k in map_geog_crs:
            temp = copy.copy(key)
            temp['code'] = map_geog_crs[k]['code']
            if temp == json.loads(k):
                count_common = 0
                while True:
                    if len(code) <= count_common:
                        break
                    if len(map_geog_crs[k]['code']) <= count_common:
                        break
                    if code[count_common] == map_geog_crs[k]['code'][count_common]:
                        count_common += 1
                    else:
                        break
                if count_common > max_count_common:
                    max_count_common = count_common
                    max_common_code = map_geog_crs[k]['code']
        assert max_count_common >= 4, (max_common_code, max_count_common, line)
        geogcrs_code = max_common_code
    else:
        geogcrs_code = map_geog_crs[key_geog_crs]['code']
    return geogcrs_code


IGNF_file = sys.argv[1]
proj_db = sys.argv[2]

conn = sqlite3.connect(proj_db)
cursor = conn.cursor()

all_sql = []

geocentric_crs_name_to_datum_code = {}
map_geog_crs = {}

for line in open(IGNF_file, 'rt').readlines():
    line = line.strip('\n')
    if line[0] == '#' or line.startswith('<metadata>'):
        continue
    assert line[0] == '<'
    code = line[1:line.find('>')]
    proj4_string = line[line.find('>')+1:]
    #print(code, proj4_string)
    tokens = proj4_string.split(' +')
    assert tokens[0] == ''
    tokens = tokens[1:]
    d = {}
    for token in tokens:
        if token == 'no_defs <>' or token == 'wktext':
            continue
        pos_equal = token.find('=')
        assert pos_equal > 0, line
        key = token[0:pos_equal]
        value = token[pos_equal+1:]
        d[key] = value
    assert 'proj' in d, line
    assert 'title' in d, line
    proj = d['proj']

    assert 'a' in d
    assert 'rf' in d
    assert (not 'nadgrids' in d) or d['nadgrids'] in ('null', '@null', 'ntf_r93.gsb,null'), line
    assert 'towgs84' in d

    if proj == 'geocent':

        ellps_code, ellps_name = find_ellipsoid(cursor, line, d)

        datum_code = 'DATUM_' + code

        geocentric_crs_name_to_datum_code[d['title']] = datum_code

        sql = """INSERT INTO "geodetic_datum" VALUES('IGNF','%s','%s',NULL,NULL,'EPSG','%s','EPSG','8901','EPSG','1262',0);""" % (datum_code, escape_literal(d['title']), ellps_code)
        all_sql.append(sql)

        #sql = """INSERT INTO "crs" VALUES('IGNF','%s','geocentric');""" % (code)
        #all_sql.append(sql)

        sql = """INSERT INTO "geodetic_crs" VALUES('IGNF','%s','%s',NULL,NULL,'geocentric','EPSG','6500','IGNF','%s','EPSG','1262',NULL,0);""" % (code, escape_literal(d['title']), datum_code)
        all_sql.append(sql)

        #sql = """INSERT INTO "coordinate_operation" VALUES('IGNF','IGNF_%s_TO_EPSG_4978','helmert_transformation');""" % (code)
        #all_sql.append(sql)

        towgs84 = d['towgs84'].split(',')
        assert len(towgs84) in (3, 7), towgs84
        x = towgs84[0]
        y = towgs84[1]
        z = towgs84[2]
        if len(towgs84) == 3:
            rx = 'NULL'
            ry = 'NULL'
            rz = 'NULL'
            s = 'NULL'
            r_uom_auth_name = 'NULL'
            r_uom_code = 'NULL'
            s_uom_auth_name = 'NULL'
            s_uom_code = 'NULL'
            method_code = "'1031'"
            method_name = "'Geocentric translations (geocentric domain)'"
        else:
            rx = towgs84[3]
            ry = towgs84[4]
            rz = towgs84[5]
            s = towgs84[6]
            r_uom_auth_name = "'EPSG'"
            r_uom_code = "'9104'"
            s_uom_auth_name = "'EPSG'"
            s_uom_code = "'9202'"
            method_code = "'1033'"
            method_name = "'Position Vector transformation (geocentric domain)'"

        sql = """INSERT INTO "helmert_transformation" VALUES('IGNF','IGNF_%s_TO_EPSG_4978','%s to WGS 84',NULL,NULL,'EPSG',%s,%s,'IGNF','%s','EPSG','4978','EPSG','1262',NULL,%s,%s,%s,'EPSG','9001',%s,%s,%s,%s,%s,%s,%s, %s,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0);""" % (code, escape_literal(d['title']), method_code, method_name, code, x, y, z, rx, ry, rz, r_uom_auth_name, r_uom_code, s, s_uom_auth_name, s_uom_code)
        all_sql.append(sql)

        if 'nadgrids' in d and d['nadgrids'] == 'ntf_r93.gsb,null':

            #sql = """INSERT INTO "coordinate_operation" VALUES('IGNF','IGNF_%s_TO_EPSG_4978_GRID','grid_transformation');""" % (code)
            #all_sql.append(sql)

            sql = """INSERT INTO "grid_transformation" VALUES('IGNF','IGNF_%s_TO_EPSG_4978_GRID','%s to WGS 84 (2)',NULL,NULL,'EPSG','9615','NTv2','IGNF','%s','EPSG','4978','EPSG','1262',NULL,'EPSG','8656','Latitude and longitude difference file','ntf_r93.gsb',NULL,NULL,NULL,NULL,NULL,NULL,0);""" % (code, escape_literal(d['title']), code)
            all_sql.append(sql)

        all_sql.append('')

    elif proj == 'longlat':

        pm = None
        pm_code = "'8901'" # Greenwich
        if 'pm' in d:
            pm = d['pm']
            assert pm == '2.337229167', pm
            pm_code = "'8903'" # Paris

        ellps_code, ellps_name = find_ellipsoid(cursor, line, d)

        if d['title'] in geocentric_crs_name_to_datum_code:
            datum_code = geocentric_crs_name_to_datum_code[d['title']]
        else:
            datum_code = 'DATUM_' + code
            sql = """INSERT INTO "geodetic_datum" VALUES('IGNF','%s','%s',NULL,NULL,'EPSG','%s','EPSG',%s,'EPSG','1262',0);""" % (datum_code, escape_literal(d['title']), ellps_code, pm_code)
            all_sql.append(sql)

        #sql = """INSERT INTO "crs" VALUES('IGNF','%s','geographic 2D');""" % (code)
        #all_sql.append(sql)

        cs_code = "'6424'" # Long Lat deg
        if 'grades' in d['title']:
            cs_code = "'6425'" # Long Lat grad

        sql = """INSERT INTO "geodetic_crs" VALUES('IGNF','%s','%s',NULL,NULL,'geographic 2D','EPSG',%s,'IGNF','%s','EPSG','1262',NULL,0);""" % (code, escape_literal(d['title']), cs_code, datum_code)
        all_sql.append(sql)

        key = { 'a' : d['a'], 'rf' : d['rf'], 'towgs84' : d['towgs84'], 'pm': pm }
        if "+towgs84=0.0000,0.0000,0.0000 +a=6378137.0000 +rf=298.2572221010000" in line or code == 'WGS84G':
            key['code'] = code
        key_geog_crs = json.dumps(key)
        assert key_geog_crs not in map_geog_crs, (line, map_geog_crs[key_geog_crs])
        map_geog_crs[ key_geog_crs ] = { 'code': code, 'title': d['title'] }

        if code == 'NTFP': # Grades
            assert cs_code == "'6425'" # Long Lat grad

            #sql = """INSERT INTO "coordinate_operation" VALUES('IGNF','IGNF_NTFP_TO_IGNF_NTFG','other_transformation');"""
            #all_sql.append(sql)

            sql = """INSERT INTO "other_transformation" VALUES('IGNF','IGNF_NTFP_TO_IGNF_NTFG','Nouvelle Triangulation Francaise Paris grades to Nouvelle Triangulation Francaise Greenwich degres sexagesimaux',NULL,NULL,'EPSG','9601','Longitude rotation','IGNF','NTFP','IGNF','NTFG','EPSG','1262',0.0,'EPSG','8602','Longitude offset',2.5969213,'EPSG','9105',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0);"""
            all_sql.append(sql)

            #sql = """INSERT INTO "coordinate_operation" VALUES('IGNF','IGNF_%s_TO_EPSG_4326','concatenated_operation');""" % (code)
            #all_sql.append(sql)

            sql = """INSERT INTO "concatenated_operation" VALUES('IGNF','IGNF_NTFP_TO_EPSG_4326','Nouvelle Triangulation Francaise Paris grades to WGS 84',NULL,NULL,'IGNF','NTFP','EPSG','4326','EPSG','1262',NULL,'IGNF','IGNF_NTFP_TO_IGNF_NTFG','IGNF','IGNF_NTFG_TO_EPSG_4326',NULL,NULL,0);"""
            all_sql.append(sql)

            #sql = """INSERT INTO "coordinate_operation" VALUES('IGNF','IGNF_%s_TO_EPSG_4326_GRID','concatenated_operation');""" % (code)
            #all_sql.append(sql)

            sql = """INSERT INTO "concatenated_operation" VALUES('IGNF','IGNF_NTFP_TO_EPSG_4326_GRID','Nouvelle Triangulation Francaise Paris grades to WGS 84 (2)',NULL,NULL,'IGNF','NTFP','EPSG','4326','EPSG','1262',NULL,'IGNF','IGNF_NTFP_TO_IGNF_NTFG','IGNF','IGNF_NTFG_TO_EPSG_4326_GRID',NULL,NULL,0);"""
            all_sql.append(sql)

            continue

        assert cs_code ==  "'6424'" # Long Lat deg

        #sql = """INSERT INTO "coordinate_operation" VALUES('IGNF','IGNF_%s_TO_EPSG_4326','helmert_transformation');""" % (code)
        #all_sql.append(sql)

        towgs84 = d['towgs84'].split(',')
        assert len(towgs84) in (3, 7), towgs84
        x = towgs84[0]
        y = towgs84[1]
        z = towgs84[2]
        if len(towgs84) == 3:
            rx = 'NULL'
            ry = 'NULL'
            rz = 'NULL'
            s = 'NULL'
            r_uom_auth_name = 'NULL'
            r_uom_code = 'NULL'
            s_uom_auth_name = 'NULL'
            s_uom_code = 'NULL'
            method_code = "'9603'"
            method_name = "'Geocentric translations (geog2D domain)'"
 
        else:
            rx = towgs84[3]
            ry = towgs84[4]
            rz = towgs84[5]
            s = towgs84[6]
            r_uom_auth_name = "'EPSG'"
            r_uom_code = "'9104'"
            s_uom_auth_name = "'EPSG'"
            s_uom_code = "'9202'"
            method_code = "'9606'"
            method_name = "'Position Vector transformation (geog2D domain)'"
 
        sql = """INSERT INTO "helmert_transformation" VALUES('IGNF','IGNF_%s_TO_EPSG_4326','%s to WGS 84',NULL,NULL,'EPSG',%s,%s,'IGNF','%s','EPSG','4326','EPSG','1262',NULL,%s,%s,%s,'EPSG','9001',%s,%s,%s,%s,%s,%s,%s, %s,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0);""" % (code, escape_literal(d['title']), method_code, method_name, code, x, y, z, rx, ry, rz, r_uom_auth_name, r_uom_code, s, s_uom_auth_name, s_uom_code)
        all_sql.append(sql)

        if 'nadgrids' in d and d['nadgrids'] == 'ntf_r93.gsb,null':

            #sql = """INSERT INTO "coordinate_operation" VALUES('IGNF','IGNF_%s_TO_EPSG_4326_GRID','grid_transformation');""" % (code)
            #all_sql.append(sql)

            sql = """INSERT INTO "grid_transformation" VALUES('IGNF','IGNF_%s_TO_EPSG_4326_GRID','%s to WGS 84 (2)',NULL,NULL,'EPSG','9615','NTv2','IGNF','%s','EPSG','4326','EPSG','1262',NULL,'EPSG','8656','Latitude and longitude difference file','ntf_r93.gsb',NULL,NULL,NULL,NULL,NULL,NULL,0);""" % (code, escape_literal(d['title']), code)
            all_sql.append(sql)

        all_sql.append('')

    elif proj == 'tmerc':

        geogcrs_code = find_geogcrs_code(line,d)

        assert 'x_0' in d
        assert 'y_0' in d
        assert 'k_0' in d
        assert 'lat_0' in d
        assert 'lon_0' in d
        assert 'units' in d and d['units'] == 'm'

        #sql = """INSERT INTO "coordinate_operation" VALUES('IGNF','CONV_%s','conversion');""" % (code)
        #all_sql.append(sql)

        sql = """INSERT INTO "conversion" VALUES('IGNF','CONV_%s','Conversion for %s',NULL,NULL,'EPSG','1262','EPSG','9807','Transverse Mercator','EPSG','8801','Latitude of natural origin',%s,'EPSG','9102','EPSG','8802','Longitude of natural origin',%s,'EPSG','9102','EPSG','8805','Scale factor at natural origin',%s,'EPSG','9201','EPSG','8806','False easting',%s,'EPSG','9001','EPSG','8807','False northing',%s,'EPSG','9001',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0);""" % (code, escape_literal(d['title']), d['lat_0'], d['lon_0'], d['k_0'], d['x_0'], d['y_0'])
        all_sql.append(sql)

        #sql = """INSERT INTO "crs" VALUES('IGNF','%s','projected');""" % (code)
        #all_sql.append(sql)

        sql = """INSERT INTO "projected_crs" VALUES('IGNF','%s','%s',NULL,NULL,'EPSG','4499','IGNF','%s','IGNF','CONV_%s','EPSG','1262',NULL,0);""" % (code, escape_literal(d['title']), geogcrs_code, code)
        all_sql.append(sql)

        all_sql.append('')

    elif proj == 'laea':

        geogcrs_code = find_geogcrs_code(line,d)

        assert 'x_0' in d
        assert 'y_0' in d
        assert 'lat_0' in d
        assert 'lon_0' in d
        assert 'units' in d and d['units'] == 'm'

        #sql = """INSERT INTO "coordinate_operation" VALUES('IGNF','CONV_%s','conversion');""" % (code)
        #all_sql.append(sql)

        sql = """INSERT INTO "conversion" VALUES('IGNF','CONV_%s','Conversion for %s',NULL,NULL,'EPSG','1262','EPSG','9820','Lambert Azimuthal Equal Area','EPSG','8801','Latitude of natural origin',%s,'EPSG','9102','EPSG','8802','Longitude of natural origin',%s,'EPSG','9102','EPSG','8806','False easting',%s,'EPSG','9001','EPSG','8807','False northing',%s,'EPSG','9001',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0);""" % (code, escape_literal(d['title']), d['lat_0'], d['lon_0'], d['x_0'], d['y_0'])
        all_sql.append(sql)

        #sql = """INSERT INTO "crs" VALUES('IGNF','%s','projected');""" % (code)
        #all_sql.append(sql)

        sql = """INSERT INTO "projected_crs" VALUES('IGNF','%s','%s',NULL,NULL,'EPSG','4499','IGNF','%s','IGNF','CONV_%s','EPSG','1262',NULL,0);""" % (code, escape_literal(d['title']), geogcrs_code, code)
        all_sql.append(sql)

        all_sql.append('')

    elif proj == 'lcc':

        geogcrs_code = find_geogcrs_code(line,d)

        assert 'x_0' in d
        assert 'y_0' in d
        assert 'lat_0' in d
        assert 'lat_1' in d
        assert 'k_0' in d or 'lat_2' in d, line
        assert 'lon_0' in d
        assert 'units' in d and d['units'] == 'm'

        #sql = """INSERT INTO "coordinate_operation" VALUES('IGNF','CONV_%s','conversion');""" % (code)
        #all_sql.append(sql)

        if 'lat_2' in d:
            sql = """INSERT INTO "conversion" VALUES('IGNF','CONV_%s','Conversion for %s',NULL,NULL,'EPSG','1262','EPSG','9802','Lambert Conic Conformal (2SP)','EPSG','8821','Latitude of false origin',%s,'EPSG','9102','EPSG','8822','Longitude of false origin',%s,'EPSG','9102','EPSG','8823','Latitude of 1st standard parallel',%s,'EPSG','9102','EPSG','8824','Latitude of 2nd standard parallel',%s,'EPSG','9102','EPSG','8826','Easting at false origin',%s,'EPSG','9001','EPSG','8827','Northing at false origin',%s,'EPSG','9001',NULL,NULL,NULL,NULL,NULL,NULL,0);""" % (code, escape_literal(d['title']), d['lat_0'], d['lon_0'], d['lat_1'], d['lat_2'], d['x_0'], d['y_0'])
        else:
            assert d['lat_0'] == d['lat_1'], line
            sql = """INSERT INTO "conversion" VALUES('IGNF','CONV_%s','Conversion for %s',NULL,NULL,'EPSG','1262','EPSG','9801','Lambert Conic Conformal (1SP)','EPSG','8801','Latitude of natural origin',%s,'EPSG','9102','EPSG','8802','Longitude of natural origin',%s,'EPSG','9102','EPSG','8805','Scale factor at natural origin',%s,'EPSG','9201','EPSG','8806','False easting',%s,'EPSG','9001','EPSG','8807','False northing',%s,'EPSG','9001',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0);""" % (code, escape_literal(d['title']), d['lat_0'], d['lon_0'], d['k_0'], d['x_0'], d['y_0'])

        all_sql.append(sql)

        #sql = """INSERT INTO "crs" VALUES('IGNF','%s','projected');""" % (code)
        #all_sql.append(sql)

        sql = """INSERT INTO "projected_crs" VALUES('IGNF','%s','%s',NULL,NULL,'EPSG','4499','IGNF','%s','IGNF','CONV_%s','EPSG','1262',NULL,0);""" % (code, escape_literal(d['title']), geogcrs_code, code)
        all_sql.append(sql)

        all_sql.append('')

    elif proj == 'eqc':

        geogcrs_code = find_geogcrs_code(line,d)

        assert 'x_0' in d, line
        assert 'y_0' in d, line
        assert 'lat_0' in d, line
        assert 'lat_ts' in d, line
        assert 'lon_0' in d, line
        assert 'units' in d and d['units'] == 'm', line
        assert float(d['lat_0']) == 0, line

        #sql = """INSERT INTO "coordinate_operation" VALUES('IGNF','CONV_%s','conversion');""" % (code)
        #all_sql.append(sql)

        sql = """INSERT INTO "conversion" VALUES('IGNF','CONV_%s','Conversion for %s',NULL,NULL,'EPSG','1262','EPSG','1028','Equidistant Cylindrical','EPSG','8823','Latitude of 1st standard parallel',%s,'EPSG','9102','EPSG','8802','Longitude of natural origin',%s,'EPSG','9102','EPSG','8806','False easting',%s,'EPSG','9001','EPSG','8807','False northing',%s,'EPSG','9001',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0);""" % (code, escape_literal(d['title']), d['lat_ts'], d['lon_0'], d['x_0'], d['y_0'])
        all_sql.append(sql)

        #sql = """INSERT INTO "crs" VALUES('IGNF','%s','projected');""" % (code)
        #all_sql.append(sql)

        sql = """INSERT INTO "projected_crs" VALUES('IGNF','%s','%s',NULL,NULL,'EPSG','4499','IGNF','%s','IGNF','CONV_%s','EPSG','1262',NULL,0);""" % (code, escape_literal(d['title']), geogcrs_code, code)
        all_sql.append(sql)

        all_sql.append('')

    elif proj == 'gstmerc':

        geogcrs_code = find_geogcrs_code(line,d)

        assert 'x_0' in d
        assert 'y_0' in d
        assert 'k_0' in d
        assert 'lat_0' in d
        assert 'lon_0' in d
        assert 'units' in d and d['units'] == 'm'

        #sql = """INSERT INTO "coordinate_operation" VALUES('IGNF','CONV_%s','conversion');""" % (code)
        #all_sql.append(sql)

        sql = """INSERT INTO "conversion" VALUES('IGNF','CONV_%s','Conversion for %s',NULL,NULL,'EPSG','1262',NULL,NULL,'Gauss Schreiber Transverse Mercator','EPSG','8801','Latitude of natural origin',%s,'EPSG','9102','EPSG','8802','Longitude of natural origin',%s,'EPSG','9102','EPSG','8805','Scale factor at natural origin',%s,'EPSG','9201','EPSG','8806','False easting',%s,'EPSG','9001','EPSG','8807','False northing',%s,'EPSG','9001',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0);""" % (code, escape_literal(d['title']), d['lat_0'], d['lon_0'], d['k_0'], d['x_0'], d['y_0'])
        all_sql.append(sql)

        #sql = """INSERT INTO "crs" VALUES('IGNF','%s','projected');""" % (code)
        #all_sql.append(sql)

        sql = """INSERT INTO "projected_crs" VALUES('IGNF','%s','%s',NULL,NULL,'EPSG','4499','IGNF','%s','IGNF','CONV_%s','EPSG','1262',NULL,0);""" % (code, escape_literal(d['title']), geogcrs_code, code)
        all_sql.append(sql)

        all_sql.append('')

    elif proj == 'stere':

        geogcrs_code = find_geogcrs_code(line,d)

        assert 'x_0' in d
        assert 'y_0' in d
        if 'k' in d or 'k_0' in d:
            print('k/k_0 ignored for ' + line)
        assert 'lat_0' in d
        assert float(d['lat_0']) == -90, line
        assert 'lat_ts' in d
        assert 'lon_0' in d
        assert float(d['lon_0']) == 140, line
        assert 'units' in d and d['units'] == 'm'

        #sql = """INSERT INTO "coordinate_operation" VALUES('IGNF','CONV_%s','conversion');""" % (code)
        #all_sql.append(sql)

        sql = """INSERT INTO "conversion" VALUES('IGNF','CONV_%s','Conversion for %s',NULL,NULL,'EPSG','1262','EPSG','9829','Polar Stereographic (variant B)','EPSG','8832','Latitude of standard parallel',%s,'EPSG','9102','EPSG','8833','Longitude of origin',%s,'EPSG','9102','EPSG','8806','False easting',%s,'EPSG','9001','EPSG','8807','False northing',%s,'EPSG','9001',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0);""" % (code, escape_literal(d['title']), d['lat_ts'], d['lon_0'], d['x_0'], d['y_0'])
        all_sql.append(sql)

        #sql = """INSERT INTO "crs" VALUES('IGNF','%s','projected');""" % (code)
        #all_sql.append(sql)

        # Hardcoding 1025 for coordinate system since the only instance uses it
        sql = """INSERT INTO "projected_crs" VALUES('IGNF','%s','%s',NULL,NULL,'EPSG','1025','IGNF','%s','IGNF','CONV_%s','EPSG','1262',NULL,0);""" % (code, escape_literal(d['title']), geogcrs_code, code)
        all_sql.append(sql)

        all_sql.append('')

    elif proj == 'mill':

        geogcrs_code = find_geogcrs_code(line,d)

        assert 'x_0' in d
        assert 'y_0' in d
        assert 'lon_0' in d
        assert 'units' in d and d['units'] == 'm'
        assert float(d['x_0']) == 0, d
        assert float(d['y_0']) == 0, d
        assert float(d['lon_0']) == 0, d

        #sql = """INSERT INTO "coordinate_operation" VALUES('IGNF','CONV_%s','conversion');""" % (code)
        #all_sql.append(sql)

        sql = """INSERT INTO "conversion" VALUES('IGNF','CONV_%s','Conversion for %s',NULL,NULL,'EPSG','1262',NULL,NULL,'PROJ mill',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0);""" % (code, escape_literal(d['title']))
        all_sql.append(sql)

        #sql = """INSERT INTO "crs" VALUES('IGNF','%s','projected');""" % (code)
        #all_sql.append(sql)

        sql = """INSERT INTO "projected_crs" VALUES('IGNF','%s','%s',NULL,NULL,'EPSG','4499','IGNF','%s','IGNF','CONV_%s','EPSG','1262',NULL,0);""" % (code, escape_literal(d['title']), geogcrs_code, code)
        all_sql.append(sql)

        all_sql.append('')

    else:
        assert False, line


all_sql.append("""INSERT INTO grid_alternatives(original_grid_name,
                              proj_grid_name,
                              proj_grid_format,
                              proj_method,
                              inverse_direction,
                              package_name,
                              url, direct_download, open_license, directory)
                      VALUES ('ntf_r93.gsb',    -- as referenced by the IGNF registry
                              'ntf_r93.gsb',
                              'NTv2',
                              'hgridshift',
                              0,
                              'proj-datumgrid',
                              NULL, NULL, NULL, NULL);
""")

#for k in map_geog_crs:
#    print k, map_geog_crs[k]

script_dir_name = os.path.dirname(os.path.realpath(__file__))
sql_dir_name = os.path.join(os.path.dirname(script_dir_name), 'data', 'sql')

f = open(os.path.join(sql_dir_name, 'ignf') + '.sql', 'wb')
f.write("--- This file has been generated by scripts/build_db_create_ignf.py from the PROJ4 IGNF definition file. DO NOT EDIT !\n\n".encode('UTF-8'))
for sql in all_sql:
    f.write((sql + '\n').encode('UTF-8'))
f.close()

