#!/usr/bin/env python
###############################################################################
# $Id$
#
#  Project:  PROJ
#  Purpose:  Build SRS and coordinate transform database from ESRI database
#            provided at https://github.com/Esri/projection-engine-db-doc
#            (from the csv/ directory content)
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

import csv
import os
import sqlite3
import sys

if len(sys.argv) != 3:
    print('Usage: build_db_from_esri.py path_to_esri_csv_dir proj.db')
    print('')
    print('path_to_esri_csv_dir is typically the path to the "csv" directory ')
    print('of a "git clone https://github.com/Esri/projection-engine-db-doc"')
    sys.exit(1)

path_to_csv = sys.argv[1]
proj_db = sys.argv[2]

conn = sqlite3.connect(proj_db)
cursor = conn.cursor()

all_sql = []

# TODO: update this !
version = 'ArcMap 10.8.1'
all_sql.append(
    """INSERT INTO "metadata" VALUES('ESRI.VERSION', '%s');""" % (version))
date = '2020-05-24'
all_sql.append(
    """INSERT INTO "metadata" VALUES('ESRI.DATE', '%s');""" % (date))

manual_grids = """------------------
-- ESRI grid names
------------------
INSERT INTO grid_alternatives(original_grid_name,
                              proj_grid_name,
                              old_proj_grid_name,
                              proj_grid_format,
                              proj_method,
                              inverse_direction,
                              package_name,
                              url, direct_download, open_license, directory)
VALUES
('prvi','us_noaa_prvi.tif','prvi','GTiff','hgridshift',0,NULL,'https://cdn.proj.org/us_noaa_prvi.tif', 1, 1, NULL),
('portugal/DLX_ETRS89_geo','pt_dgt_DLx_ETRS89_geo.tif','DLX_ETRS89_geo.gsb','GTiff','hgridshift',0,NULL,'https://cdn.proj.org/pt_dgt_DLx_ETRS89_geo.tif',1,1,NULL),
('portugal/D73_ETRS89_geo','pt_dgt_D73_ETRS89_geo.tif','D73_ETRS89_geo.gsb','GTiff','hgridshift',0,NULL,'https://cdn.proj.org/pt_dgt_D73_ETRS89_geo.tif',1,1,NULL),
('netherlands/rdtrans2008','','rdtrans2008.gsb','NTv2','hgridshift',0,NULL,'https://salsa.debian.org/debian-gis-team/proj-rdnap/raw/upstream/2008/rdtrans2008.gsb',1,0,NULL),
('uk/OSTN15_NTv2','uk_os_OSTN15_NTv2_OSGBtoETRS.tif','OSTN15_NTv2_OSGBtoETRS.gsb','GTiff','hgridshift',1  -- reverse direction
    ,NULL,'https://cdn.proj.org/uk_os_OSTN15_NTv2_OSGBtoETRS.tif',1,1,NULL),
('canada/GS7783','ca_nrc_GS7783.tif','GS7783.GSB','GTiff','hgridshift',0,NULL,'https://cdn.proj.org/ca_nrc_GS7783.tif',1,1,NULL),
('c1hpgn', 'us_noaa_c1hpgn.tif', 'c1hpgn.gsb', 'GTiff', 'hgridshift', 0, NULL, 'https://cdn.proj.org/us_noaa_c1hpgn.tif', 1, 1, NULL),
('c2hpgn', 'us_noaa_c2hpgn.tif', 'c2hpgn.gsb', 'GTiff', 'hgridshift', 0, NULL, 'https://cdn.proj.org/us_noaa_c2hpgn.tif', 1, 1, NULL),
('spain/100800401','es_cat_icgc_100800401.tif','100800401.gsb','GTiff','hgridshift',0,NULL,'https://cdn.proj.org/es_cat_icgc_100800401.tif',1,1,NULL),
('australia/QLD_0900','au_icsm_National_84_02_07_01.tif','National_84_02_07_01.gsb','GTiff','hgridshift',0,NULL,'https://cdn.proj.org/au_icsm_National_84_02_07_01.tif',1,1,NULL), -- From https://www.dnrme.qld.gov.au/__data/assets/pdf_file/0006/105765/gday-21-user-guide.pdf: "Note that the Queensland grid QLD_0900.gsb produces identical results to the National AGD84 grid for the equivalent coverage."
('spain/PENR2009','es_ign_SPED2ETV2.tif',NULL,'GTiff','hgridshift',0,NULL,'https://cdn.proj.org/es_ign_SPED2ETV2.tif',1,1,NULL),
('spain/BALR2009','es_ign_SPED2ETV2.tif',NULL,'GTiff','hgridshift',0,NULL,'https://cdn.proj.org/es_ign_SPED2ETV2.tif',1,1,NULL);
-- 'france/RGNC1991_IGN72GrandeTerre' : we have a 3D geocentric corresponding one: no need for mapping
-- 'france/RGNC1991_NEA74Noumea' : we have a 3D geocentric corresponding one: no need for mapping
"""

def escape_literal(x):
    return x.replace("'", "''")


########################

map_extentname_to_auth_code = {}
esri_area_counter = 1


def find_extent(extentname, slat, nlat, llon, rlon):

    global esri_area_counter

    if extentname in map_extentname_to_auth_code:
        return map_extentname_to_auth_code[extentname]

    deg = b'\xC2\xB0'.decode('utf-8')
    cursor.execute("SELECT auth_name, code FROM extent WHERE name = ? AND auth_name != 'ESRI'",
                   (extentname.replace('~', deg),))
    row = cursor.fetchone()
    if row is None:
        cursor.execute(
            "SELECT auth_name, code FROM extent WHERE auth_name != 'ESRI' AND south_lat = ? AND north_lat = ? AND west_lon = ? AND east_lon = ?", (slat, nlat, llon, rlon))
        row = cursor.fetchone()

    if row is None:
        #print('unknown extent inserted: ' + extentname)

        if float(rlon) > 180:
            new_rlon = '%s' % (float(rlon) - 360)
            print('Correcting rlon from %s to %s for %s' %
                  (rlon, new_rlon, extentname))
            rlon = new_rlon

        assert float(slat) >= -90 and float(slat) <= 90, (extentname,
                                                          slat, nlat, llon, rlon)
        assert float(nlat) >= -90 and float(nlat) <= 90, (extentname,
                                                          slat, nlat, llon, rlon)
        assert float(nlat) > float(slat), (extentname, slat, nlat, llon, rlon)
        assert float(llon) >= -180 and float(llon) <= 180, (extentname,
                                                            slat, nlat, llon, rlon)
        assert float(rlon) >= -180 and float(rlon) <= 180, (extentname,
                                                            slat, nlat, llon, rlon)

        sql = """INSERT INTO "extent" VALUES('ESRI','%d','%s','%s',%s,%s,%s,%s,0);""" % (
            esri_area_counter, escape_literal(extentname), escape_literal(extentname), slat, nlat, llon, rlon)
        all_sql.append(sql)
        map_extentname_to_auth_code[extentname] = [
            'ESRI', '%d' % esri_area_counter]
        esri_area_counter += 1
    else:
        auth_name = row[0]
        code = row[1]
        map_extentname_to_auth_code[extentname] = [auth_name, code]

    return map_extentname_to_auth_code[extentname]

#################

def import_linunit():
    with open(os.path.join(path_to_csv, 'pe_list_linunit.csv'), 'rt') as csvfile:
        reader = csv.reader(csvfile)
        header = next(reader)
        nfields = len(header)

        idx_wkid = header.index('wkid')
        assert idx_wkid >= 0

        idx_latestWkid = header.index('latestWkid')
        assert idx_latestWkid >= 0

        idx_name = header.index('name')
        assert idx_name >= 0

        idx_wkt = header.index('wkt')
        assert idx_wkt >= 0

        idx_authority = header.index('authority')
        assert idx_authority >= 0

        while True:
            try:
                row = next(reader)
            except StopIteration:
                break
            assert len(row) == nfields, row

            latestWkid = row[idx_latestWkid]
            authority = row[idx_authority]
            esri_name = row[idx_name]

            wkt = row[idx_wkt]
            assert wkt.startswith('UNIT[') and wkt.endswith(']')
            tokens = wkt[len('UNIT['):len(wkt) - 1].split(',')
            assert len(tokens) == 2
            esri_conv_factor = float(tokens[1])

            if authority == 'EPSG':

                cursor.execute(
                    "SELECT name, conv_factor FROM unit_of_measure WHERE code = ? AND auth_name = 'EPSG'", (latestWkid,))
                src_row = cursor.fetchone()
                assert src_row, row
                src_name = src_row[0]
                epsg_conv_factor = src_row[1]
                assert abs(esri_conv_factor - epsg_conv_factor) <= 1e-15 * epsg_conv_factor, (esri_name, esri_conv_factor, epsg_conv_factor)

                if src_name != esri_name:
                    sql = """INSERT INTO alias_name VALUES('unit_of_measure','EPSG','%s','%s','ESRI');""" % (
                        latestWkid, escape_literal(esri_name))
                    all_sql.append(sql)


#################
map_spheroid_esri_name_to_auth_code = {}


def import_spheroid():
    with open(os.path.join(path_to_csv, 'pe_list_spheroid.csv'), 'rt') as csvfile:
        reader = csv.reader(csvfile)
        header = next(reader)
        nfields = len(header)

        idx_wkid = header.index('wkid')
        assert idx_wkid >= 0

        idx_latestWkid = header.index('latestWkid')
        assert idx_latestWkid >= 0

        idx_name = header.index('name')
        assert idx_name >= 0

        idx_description = header.index('description')
        assert idx_description >= 0

        idx_wkt = header.index('wkt')
        assert idx_wkt >= 0

        idx_authority = header.index('authority')
        assert idx_authority >= 0

        idx_deprecated = header.index('deprecated')
        assert idx_deprecated >= 0

        body_set = set()
        while True:
            try:
                row = next(reader)
            except StopIteration:
                break
            assert len(row) == nfields, row

            code = row[idx_wkid]
            latestWkid = row[idx_latestWkid]
            authority = row[idx_authority]
            esri_name = row[idx_name]

            if authority == 'EPSG':

                cursor.execute(
                    "SELECT name FROM ellipsoid WHERE code = ? AND auth_name = 'EPSG'", (latestWkid,))
                src_row = cursor.fetchone()
                assert src_row, row
                src_name = src_row[0]

                map_spheroid_esri_name_to_auth_code[esri_name] = [
                    'EPSG', latestWkid]

                if src_name != esri_name:
                    sql = """INSERT INTO alias_name VALUES('ellipsoid','EPSG','%s','%s','ESRI');""" % (
                        code, escape_literal(esri_name))
                    all_sql.append(sql)

            else:
                assert authority.upper() == 'ESRI', row

                wkt = row[idx_wkt]
                assert wkt.startswith('SPHEROID[') and wkt.endswith(']')
                tokens = wkt[len('SPHEROID['):len(wkt) - 1].split(',')
                assert len(tokens) == 3
                a = tokens[1]
                rf = tokens[2]

                description = row[idx_description]
                deprecated = 1 if row[idx_deprecated] == 'yes' else 0

                if esri_name not in map_spheroid_esri_name_to_auth_code:
                    map_spheroid_esri_name_to_auth_code[esri_name] = [
                        'ESRI', code]

                if abs(float(a) - 6375000) > 0.01 * 6375000:
                    pos = esri_name.find('_19')
                    if pos < 0:
                        pos = esri_name.find('_20')
                    assert pos > 0
                    body_code = esri_name[0:pos]

                    if body_code not in body_set:
                        body_set.add(body_code)
                        sql = """INSERT INTO celestial_body VALUES('ESRI', '%s', '%s', %s);""" % (
                            body_code, body_code, a)
                        all_sql.append(sql)
                    body_auth = 'ESRI'
                else:
                    body_auth = 'PROJ'
                    body_code = 'EARTH'

                sql = """INSERT INTO ellipsoid VALUES('ESRI','%s','%s','%s','%s','%s',%s,'EPSG','9001',%s,NULL,%d);""" % (
                    code, esri_name, description, body_auth, body_code, a, rf, deprecated)
                all_sql.append(sql)

########################


map_pm_esri_name_to_auth_code = {}


def import_prime_meridian():
    with open(os.path.join(path_to_csv, 'pe_list_primem.csv'), 'rt') as csvfile:
        reader = csv.reader(csvfile)
        header = next(reader)
        nfields = len(header)

        idx_wkid = header.index('wkid')
        assert idx_wkid >= 0

        idx_latestWkid = header.index('latestWkid')
        assert idx_latestWkid >= 0

        idx_name = header.index('name')
        assert idx_name >= 0

        idx_description = header.index('description')
        assert idx_description >= 0

        idx_wkt = header.index('wkt')
        assert idx_wkt >= 0

        idx_authority = header.index('authority')
        assert idx_authority >= 0

        idx_deprecated = header.index('deprecated')
        assert idx_deprecated >= 0

        while True:
            try:
                row = next(reader)
            except StopIteration:
                break
            assert len(row) == nfields, row

            code = row[idx_wkid]
            latestWkid = row[idx_latestWkid]
            authority = row[idx_authority]
            esri_name = row[idx_name]

            if authority == 'EPSG':

                cursor.execute(
                    "SELECT name FROM prime_meridian WHERE code = ? AND auth_name = 'EPSG'", (latestWkid,))
                src_row = cursor.fetchone()
                assert src_row, row
                src_name = src_row[0]

                map_pm_esri_name_to_auth_code[esri_name] = ['EPSG', latestWkid]

                if src_name != esri_name:
                    sql = """INSERT INTO alias_name VALUES('prime_meridian','EPSG','%s','%s','ESRI');""" % (
                        code, escape_literal(esri_name))
                    all_sql.append(sql)

            else:
                assert authority.upper() == 'ESRI', row

                wkt = row[idx_wkt]
                assert wkt.startswith('PRIMEM[') and wkt.endswith(']')
                tokens = wkt[len('PRIMEM['):len(wkt) - 1].split(',')
                assert len(tokens) == 2
                value = tokens[1]

                deprecated = 1 if row[idx_deprecated] == 'yes' else 0

                if esri_name not in map_pm_esri_name_to_auth_code:
                    map_pm_esri_name_to_auth_code[esri_name] = ['ESRI', code]

                sql = """INSERT INTO "prime_meridian" VALUES('ESRI','%s','%s',%s,'EPSG','9110',%d);""" % (
                    code, esri_name, value, deprecated)
                all_sql.append(sql)


########################

map_datum_esri_name_to_auth_code = {}
map_datum_esri_to_parameters = {}


def import_datum():
    with open(os.path.join(path_to_csv, 'pe_list_datum.csv'), 'rt') as csvfile:
        reader = csv.reader(csvfile)
        header = next(reader)
        nfields = len(header)

        idx_wkid = header.index('wkid')
        assert idx_wkid >= 0

        idx_latestWkid = header.index('latestWkid')
        assert idx_latestWkid >= 0

        idx_name = header.index('name')
        assert idx_name >= 0

        idx_description = header.index('description')
        assert idx_description >= 0

        idx_wkt = header.index('wkt')
        assert idx_wkt >= 0

        idx_authority = header.index('authority')
        assert idx_authority >= 0

        idx_deprecated = header.index('deprecated')
        assert idx_deprecated >= 0

        while True:
            try:
                row = next(reader)
            except StopIteration:
                break
            assert len(row) == nfields, row

            code = row[idx_wkid]
            latestWkid = row[idx_latestWkid]
            authority = row[idx_authority]
            esri_name = row[idx_name]

            if authority == 'EPSG':

                map_datum_esri_name_to_auth_code[esri_name] = [
                    'EPSG', latestWkid]

                cursor.execute(
                    "SELECT name FROM geodetic_datum WHERE auth_name = 'EPSG' AND code = ?", (latestWkid,))
                src_row = cursor.fetchone()
                assert src_row, row
                src_name = src_row[0]
                if src_name != esri_name:
                    sql = """INSERT INTO alias_name VALUES('geodetic_datum','EPSG','%s','%s','ESRI');""" % (
                        code, escape_literal(esri_name))
                    all_sql.append(sql)

                description = row[idx_description]
                deprecated = 1 if row[idx_deprecated] == 'yes' else 0

                map_datum_esri_to_parameters[code] = {
                    'esri_name': esri_name,
                    'description': description,
                    'deprecated': deprecated
                }

            else:
                assert authority.upper() == 'ESRI', row

                map_datum_esri_name_to_auth_code[esri_name] = [
                    'ESRI', code]

                wkt = row[idx_wkt]
                pos = wkt.find('SPHEROID["')
                assert pos >= 0
                pos += len('SPHEROID["')
                end_pos = wkt[pos:].find('"')
                assert end_pos >= 0
                end_pos += pos
                ellps_name = wkt[pos:end_pos]

                assert ellps_name in map_spheroid_esri_name_to_auth_code, (
                    ellps_name, row)
                ellps_auth_name, ellps_code = map_spheroid_esri_name_to_auth_code[ellps_name]

                description = row[idx_description]
                deprecated = 1 if row[idx_deprecated] == 'yes' else 0

                map_datum_esri_to_parameters[code] = {
                    'esri_name': esri_name,
                    'description': description,
                    'ellps_auth_name': ellps_auth_name,
                    'ellps_code': ellps_code,
                    'deprecated': deprecated
                }
                # We cannot write it since we lack the prime meridian !!! (and
                # the area of use)

########################


map_geogcs_esri_name_to_auth_code = {}


def import_geogcs():
    with open(os.path.join(path_to_csv, 'pe_list_geogcs.csv'), 'rt') as csvfile:
        reader = csv.reader(csvfile)
        header = next(reader)
        nfields = len(header)

        idx_wkid = header.index('wkid')
        assert idx_wkid >= 0

        idx_latestWkid = header.index('latestWkid')
        assert idx_latestWkid >= 0

        idx_name = header.index('name')
        assert idx_name >= 0

        idx_description = header.index('description')
        assert idx_description >= 0

        idx_wkt = header.index('wkt')
        assert idx_wkt >= 0

        idx_authority = header.index('authority')
        assert idx_authority >= 0

        idx_deprecated = header.index('deprecated')
        assert idx_deprecated >= 0

        idx_areaname = header.index('areaname')
        assert idx_areaname >= 0

        idx_slat = header.index('slat')
        assert idx_slat >= 0

        idx_nlat = header.index('nlat')
        assert idx_nlat >= 0

        idx_llon = header.index('llon')
        assert idx_llon >= 0

        idx_rlon = header.index('rlon')
        assert idx_rlon >= 0

        datum_written = set()

        while True:
            try:
                row = next(reader)
            except StopIteration:
                break
            assert len(row) == nfields, row

            code = row[idx_wkid]
            latestWkid = row[idx_latestWkid]
            authority = row[idx_authority]
            esri_name = row[idx_name]

            if authority == 'EPSG':

                map_geogcs_esri_name_to_auth_code[esri_name] = [
                    'EPSG', latestWkid]

                cursor.execute(
                    "SELECT name FROM geodetic_crs WHERE auth_name = 'EPSG' AND code = ?", (latestWkid,))
                src_row = cursor.fetchone()
                assert src_row, row
                src_name = src_row[0]
                if src_name != esri_name:
                    sql = """INSERT INTO alias_name VALUES('geodetic_crs','EPSG','%s','%s','ESRI');""" % (
                        code, escape_literal(esri_name))
                    all_sql.append(sql)
            else:
                assert authority.upper() == 'ESRI', row

                wkt = row[idx_wkt]
                pos = wkt.find('DATUM["')
                assert pos >= 0
                pos += len('DATUM["')
                end_pos = wkt[pos:].find('"')
                assert end_pos >= 0
                end_pos += pos
                datum_name = wkt[pos:end_pos]

                assert datum_name in map_datum_esri_name_to_auth_code, (
                    datum_name, row)
                datum_auth_name, datum_code = map_datum_esri_name_to_auth_code[datum_name]

                pos = wkt.find('PRIMEM["')
                assert pos >= 0
                pos += len('PRIMEM["')
                end_pos = wkt[pos:].find('"')
                assert end_pos >= 0
                end_pos += pos
                pm_name = wkt[pos:end_pos]

                assert pm_name in map_pm_esri_name_to_auth_code, (pm_name, row)
                pm_auth_name, pm_code = map_pm_esri_name_to_auth_code[pm_name]

                is_degree = 'UNIT["Degree' in wkt
                is_grad = 'UNIT["Grad' in wkt
                assert is_degree or is_grad, row
                cs_code = '6422' if is_degree else '6403'

                deprecated = 1 if row[idx_deprecated] == 'yes' else 0

                extent_auth_name, extent_code = find_extent(
                    row[idx_areaname], row[idx_slat], row[idx_nlat], row[idx_llon], row[idx_rlon])

                if datum_auth_name == 'ESRI':
                    if datum_code not in datum_written:
                        datum_written.add(datum_code)

                        p = map_datum_esri_to_parameters[datum_code]

                        sql = """INSERT INTO "geodetic_datum" VALUES('ESRI','%s','%s','%s','%s','%s','%s','%s',NULL,NULL,NULL,%d);""" % (
                            datum_code, p['esri_name'], p['description'], p['ellps_auth_name'], p['ellps_code'], pm_auth_name, pm_code, p['deprecated'])
                        all_sql.append(sql)
                        sql = """INSERT INTO "usage" VALUES('ESRI', '%s_USAGE','geodetic_datum','ESRI','%s','%s','%s','%s','%s');""" % (datum_code, datum_code, extent_auth_name, extent_code, 'EPSG', '1024')
                        all_sql.append(sql)
                        p['pm_auth_name'] = pm_auth_name
                        p['pm_code'] = pm_code
                        map_datum_esri_to_parameters[datum_code] = p
                    else:
                        assert map_datum_esri_to_parameters[datum_code]['pm_auth_name'] == pm_auth_name, (
                            row, map_datum_esri_to_parameters[datum_code]['pm_auth_name'], pm_auth_name)
                        if map_datum_esri_to_parameters[datum_code]['pm_code'] != pm_code:
                            p = map_datum_esri_to_parameters[datum_code]

                            # Case of GCS_Voirol_Unifie_1960 and GCS_Voirol_Unifie_1960_Paris which use the same
                            # datum D_Voirol_Unifie_1960 but with different prime meridian
                            # We create an artificial datum to avoid that issue
                            datum_name += '_' + pm_name
                            datum_code += '_' + pm_name

                            datum_written.add(datum_code)
                            map_datum_esri_name_to_auth_code[datum_name] = [
                                'ESRI', latestWkid]
                            map_datum_esri_to_parameters[datum_code] = {
                                'esri_name': datum_name,
                                'description': p['description'] + ' with ' + pm_name + ' prime meridian',
                                'ellps_auth_name': p['ellps_auth_name'],
                                'ellps_code': p['ellps_code'],
                                'deprecated': p['deprecated']
                            }

                            sql = """INSERT INTO "geodetic_datum" VALUES('ESRI','%s','%s','%s','%s','%s','%s','%s',NULL,NULL,NULL,%d);""" % (
                                datum_code, p['esri_name'], p['description'], p['ellps_auth_name'], p['ellps_code'], pm_auth_name, pm_code, p['deprecated'])
                            all_sql.append(sql)
                            sql = """INSERT INTO "usage" VALUES('ESRI', '%s_USAGE','geodetic_datum','ESRI','%s','%s','%s','%s','%s');""" % (datum_code, datum_code, extent_auth_name, extent_code, 'EPSG', '1024')
                            all_sql.append(sql)
                            p['pm_auth_name'] = pm_auth_name
                            p['pm_code'] = pm_code
                            map_datum_esri_to_parameters[datum_code] = p


                # We may have already the EPSG entry, so use it preferably
                if esri_name not in map_geogcs_esri_name_to_auth_code:
                    map_geogcs_esri_name_to_auth_code[esri_name] = ['ESRI', code]

                sql = """INSERT INTO "geodetic_crs" VALUES('ESRI','%s','%s',NULL,'geographic 2D','EPSG','%s','%s','%s',NULL,%d);""" % (
                    code, esri_name, cs_code, datum_auth_name, datum_code, deprecated)
                all_sql.append(sql)
                sql = """INSERT INTO "usage" VALUES('ESRI', '%s_USAGE','geodetic_crs','ESRI','%s','%s','%s','%s','%s');""" % (code, code, extent_auth_name, extent_code, 'EPSG', '1024')
                all_sql.append(sql)

                if deprecated and code != latestWkid and code not in ('4305', '4812'): # Voirol 1960 no longer in EPSG
                    cursor.execute(
                        "SELECT name FROM geodetic_crs WHERE auth_name = 'EPSG' AND code = ?", (latestWkid,))
                    src_row = cursor.fetchone()
                    assert src_row, (code, latestWkid)

                    sql = """INSERT INTO "supersession" VALUES('geodetic_crs','ESRI','%s','geodetic_crs','EPSG','%s','ESRI',1);""" % (
                        code, latestWkid)
                    all_sql.append(sql)

########################

def parse_wkt(s, level):
    if s[0] == '"':
        return s
    pos = s.find('[')
    if pos < 0:
        return s
    return { s[0:pos] : parse_wkt_array(s[pos+1:-1], level + 1) }

def parse_wkt_array(s, level = 0):
    ar = []
    in_string = False
    cur_token = ''
    indent_level = 0
    for c in s:
        if in_string:
            if c == '"':
                in_string = False
            cur_token += c
        elif c == '"':
            cur_token += c
            in_string = True
        elif c == '[':
            cur_token += c
            indent_level += 1
        elif c == ']':
            cur_token += c
            indent_level -= 1
            assert indent_level >= 0
        elif indent_level == 0 and  c == ',':
            ar.append(parse_wkt(cur_token, level + 1))
            cur_token = ''
        else:
            cur_token += c
    assert indent_level == 0
    if cur_token:
        ar.append(parse_wkt(cur_token, level + 1))

    if level == 0:
        d = {}
        for elt in ar:
            assert type(elt) == type({})
            assert len(elt) == 1
            if 'PROJECTION' in elt:
                assert len(elt['PROJECTION']) == 1, elt['PROJECTION']
                assert 'PROJECTION' not in d
                name = elt['PROJECTION'][0]
                assert name[0] == '"' and name[-1] == '"', name
                name = name[1:-1]
                d['PROJECTION'] = name
            elif 'PARAMETER' in elt:
                assert len(elt['PARAMETER']) == 2, elt['PARAMETER']
                name = elt['PARAMETER'][0]
                assert name[0] == '"' and name[-1] == '"', name
                name = name[1:-1]
                assert name not in d
                d[name] = elt['PARAMETER'][1]
            elif 'UNIT' in elt:
                assert len(elt['UNIT']) == 2, elt['UNIT']
                name = elt['UNIT'][0]
                assert name[0] == '"' and name[-1] == '"', name
                name = name[1:-1]
                assert 'UNIT_NAME' not in d
                d['UNIT_NAME'] = name
                d['UNIT_VALUE'] = elt['UNIT'][1]
            else:
                assert True
        return d
    else:
        return ar

########################

def get_cs(parsed_conv_wkt):

    UNIT_NAME = parsed_conv_wkt['UNIT_NAME']
    UNIT_VALUE = parsed_conv_wkt['UNIT_VALUE']

    if UNIT_NAME == 'Meter':
        uom_code = '9001'
        cs_auth_name = 'EPSG'
        cs_code = '4400'
        assert UNIT_VALUE == '1.0', UNIT_VALUE
    elif UNIT_NAME == 'Foot':
        uom_code = '9002'
        cs_auth_name = 'ESRI'
        cs_code = UNIT_NAME
        assert UNIT_VALUE == '0.3048', UNIT_VALUE
    elif UNIT_NAME == 'Foot_US':
        uom_code = '9003'
        cs_auth_name = 'ESRI'
        cs_code = UNIT_NAME
        assert UNIT_VALUE == '0.3048006096012192', UNIT_VALUE
    elif UNIT_NAME == 'Yard_Indian_1937':
        uom_code = '9085'
        cs_auth_name = 'ESRI'
        cs_code = UNIT_NAME
        assert UNIT_VALUE == '0.91439523', UNIT_VALUE
    else:
        assert False, UNIT_NAME

    if cs_auth_name == 'ESRI' and cs_code not in set_esri_cs_code:
        sql = """INSERT INTO "coordinate_system" VALUES('ESRI','%s','Cartesian',2);""" % cs_code
        all_sql.append(sql)
        sql = """INSERT INTO "axis" VALUES('ESRI','%d','Easting','E','east','ESRI','%s',1,'EPSG','%s');""" % (2 * len(set_esri_cs_code) + 1, cs_code, uom_code)
        all_sql.append(sql)
        sql = """INSERT INTO "axis" VALUES('ESRI','%d','Northing','N','north','ESRI','%s',2,'EPSG','%s');""" % (2 * len(set_esri_cs_code) + 2, cs_code, uom_code)
        all_sql.append(sql)
        set_esri_cs_code.add(cs_code)

    return cs_auth_name, cs_code, uom_code

########################

map_projcs_esri_name_to_auth_code = {}
set_esri_cs_code = set()
map_conversion_sql_to_code = {}

def import_projcs():
    with open(os.path.join(path_to_csv, 'pe_list_projcs.csv'), 'rt') as csvfile:
        reader = csv.reader(csvfile)
        header = next(reader)
        nfields = len(header)

        idx_wkid = header.index('wkid')
        assert idx_wkid >= 0

        idx_latestWkid = header.index('latestWkid')
        assert idx_latestWkid >= 0

        idx_name = header.index('name')
        assert idx_name >= 0

        idx_description = header.index('description')
        assert idx_description >= 0

        idx_wkt = header.index('wkt')
        assert idx_wkt >= 0

        idx_authority = header.index('authority')
        assert idx_authority >= 0

        idx_deprecated = header.index('deprecated')
        assert idx_deprecated >= 0

        idx_areaname = header.index('areaname')
        assert idx_areaname >= 0

        idx_slat = header.index('slat')
        assert idx_slat >= 0

        idx_nlat = header.index('nlat')
        assert idx_nlat >= 0

        idx_llon = header.index('llon')
        assert idx_llon >= 0

        idx_rlon = header.index('rlon')
        assert idx_rlon >= 0

        wkid_set = set()
        mapDeprecatedToNonDeprecated = {}

        while True:
            try:
                row = next(reader)
            except StopIteration:
                break
            assert len(row) == nfields, row

            code = row[idx_wkid]
            latestWkid = row[idx_latestWkid]
            authority = row[idx_authority]
            esri_name = row[idx_name]

            if authority == 'EPSG':

                map_projcs_esri_name_to_auth_code[esri_name] = [
                    'EPSG', latestWkid]

                cursor.execute(
                    "SELECT name FROM projected_crs WHERE auth_name = 'EPSG' AND code = ?", (latestWkid,))
                src_row = cursor.fetchone()
                assert src_row, row
                src_name = src_row[0]
                esri_name = row[idx_name]
                if src_name != esri_name:
                    sql = """INSERT INTO alias_name VALUES('projected_crs','EPSG','%s','%s','ESRI');""" % (
                        code, escape_literal(esri_name))
                    all_sql.append(sql)
            else:
                assert authority.upper() == 'ESRI', row

                wkid_set.add(code)

                wkt = row[idx_wkt]
                pos = wkt.find('GEOGCS["')
                assert pos >= 0
                pos += len('GEOGCS["')
                end_pos = wkt[pos:].find('"')
                assert end_pos >= 0
                end_pos += pos
                geogcs_name = wkt[pos:end_pos]

                pos = wkt.find('PROJECTION[')
                assert pos >= 0
                parsed_conv_wkt = parse_wkt_array(wkt[pos:-1])

                assert geogcs_name in map_geogcs_esri_name_to_auth_code, (
                    geogcs_name, row)
                geogcs_auth_name, geogcs_code = map_geogcs_esri_name_to_auth_code[geogcs_name]

                extent_auth_name, extent_code = find_extent(
                    row[idx_areaname], row[idx_slat], row[idx_nlat], row[idx_llon], row[idx_rlon])

                map_projcs_esri_name_to_auth_code[esri_name] = ['ESRI', code]

                deprecated = 1 if row[idx_deprecated] == 'yes' else 0

                method = parsed_conv_wkt['PROJECTION']

                if 'UNIT["Degree",' in wkt:
                    ang_uom_code = '9102'
                elif 'UNIT["Grad",' in wkt:
                    ang_uom_code = '9105'
                else:
                    assert False, wkt

                if method in ('Transverse_Mercator', 'Gauss_Kruger'):
                    assert len(parsed_conv_wkt) == 1 + 5 + 2
                    False_Easting = parsed_conv_wkt['False_Easting']
                    False_Northing = parsed_conv_wkt['False_Northing']
                    Central_Meridian = parsed_conv_wkt['Central_Meridian']
                    Scale_Factor = parsed_conv_wkt['Scale_Factor']
                    Latitude_Of_Origin = parsed_conv_wkt['Latitude_Of_Origin']

                    cs_auth_name, cs_code, uom_code = get_cs(parsed_conv_wkt)

                    conv_name = 'unnamed'
                    if method == 'Gauss_Kruger' and 'GK_' not in esri_name and 'Gauss' not in esri_name:
                        conv_name = esri_name + " (Gauss Kruger)"

                    cursor.execute(
                        """SELECT code FROM conversion WHERE auth_name = 'EPSG' AND
                        method_code = '9807' AND
                        param1_code = '8801' AND param1_value = ? AND param1_uom_code = ? AND
                        param2_code = '8802' AND param2_value = ? AND param2_uom_code = ? AND
                        param3_code = '8805' AND param3_value = ? AND param3_uom_code = '9201' AND
                        param4_code = '8806' AND param4_value = ? AND param4_uom_code = ? AND
                        param5_code = '8807' AND param5_value = ? AND param5_uom_code = ?""", (Latitude_Of_Origin, ang_uom_code, Central_Meridian, ang_uom_code, Scale_Factor, False_Easting, uom_code, False_Northing, uom_code))
                    src_row = cursor.fetchone()
                    if conv_name == 'unnamed' and src_row:
                        conv_auth_name = 'EPSG'
                        conv_code = src_row[0]
                    else:
                        conv_auth_name = 'ESRI'
                        conv_code = code

                        sql = """INSERT INTO "conversion" VALUES('ESRI','%s','%s',NULL,'EPSG','9807','Transverse Mercator','EPSG','8801','Latitude of natural origin',%s,'EPSG','%s','EPSG','8802','Longitude of natural origin',%s,'EPSG','%s','EPSG','8805','Scale factor at natural origin',%s,'EPSG','9201','EPSG','8806','False easting',%s,'EPSG','%s','EPSG','8807','False northing',%s,'EPSG','%s',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,%d);""" % (
                            code, conv_name, Latitude_Of_Origin, ang_uom_code, Central_Meridian, ang_uom_code, Scale_Factor, False_Easting, uom_code, False_Northing, uom_code, deprecated)

                        sql_extract = sql[sql.find('NULL'):]
                        if conv_name != 'unnamed' or sql_extract not in map_conversion_sql_to_code:
                            all_sql.append(sql)

                            sql = """INSERT INTO "usage" VALUES('ESRI', 'CONV_%s_USAGE','conversion','ESRI','%s','%s','%s','%s','%s');""" % (code, code, extent_auth_name, extent_code, 'EPSG', '1024')
                            all_sql.append(sql)

                            map_conversion_sql_to_code[sql_extract] = conv_code
                        else:
                            conv_code = map_conversion_sql_to_code[sql_extract]

                    sql = """INSERT INTO "projected_crs" VALUES('ESRI','%s','%s',NULL,'%s','%s','%s','%s','%s','%s',NULL,%d);""" % (
                        code, esri_name, cs_auth_name, cs_code, geogcs_auth_name, geogcs_code, conv_auth_name, conv_code, deprecated)
                    all_sql.append(sql)
                    sql = """INSERT INTO "usage" VALUES('ESRI', 'PCRS_%s_USAGE','projected_crs','ESRI','%s','%s','%s','%s','%s');""" % (code, code, extent_auth_name, extent_code, 'EPSG', '1024')
                    all_sql.append(sql)

                elif method == 'Hotine_Oblique_Mercator_Azimuth_Natural_Origin':
                    assert len(parsed_conv_wkt) == 1 + 6 + 2
                    False_Easting = parsed_conv_wkt['False_Easting']
                    False_Northing = parsed_conv_wkt['False_Northing']
                    Scale_Factor = parsed_conv_wkt['Scale_Factor']
                    Azimuth = parsed_conv_wkt['Azimuth']
                    Longitude_Of_Center = parsed_conv_wkt['Longitude_Of_Center']
                    Latitude_Of_Center = parsed_conv_wkt['Latitude_Of_Center']

                    cs_auth_name, cs_code, uom_code = get_cs(parsed_conv_wkt)

                    conv_name = 'unnamed'
                    conv_auth_name = 'ESRI'
                    conv_code = code

                    sql = """INSERT INTO "conversion" VALUES('ESRI','%s','%s',NULL,'EPSG','9812','Hotine Oblique Mercator (variant A)','EPSG','8811','Latitude of projection centre',%s,'EPSG','%s','EPSG','8812','Longitude of projection centre',%s,'EPSG','%s','EPSG','8813','Azimuth of initial line',%s,'EPSG','%s','EPSG','8814','Angle from Rectified to Skew Grid',%s,'EPSG','%s','EPSG','8815','Scale factor on initial line',%s,'EPSG','9201','EPSG','8806','False easting',%s,'EPSG','%s','EPSG','8807','False northing',%s,'EPSG','%s',%d);""" % (
                        code, conv_name, Latitude_Of_Center, ang_uom_code, Longitude_Of_Center, ang_uom_code, Azimuth, ang_uom_code, Azimuth, ang_uom_code, Scale_Factor, False_Easting, uom_code, False_Northing, uom_code, deprecated)

                    sql_extract = sql[sql.find('NULL'):]
                    if conv_name != 'unnamed' or sql_extract not in map_conversion_sql_to_code:
                        all_sql.append(sql)

                        sql = """INSERT INTO "usage" VALUES('ESRI', 'CONV_%s_USAGE','conversion','ESRI','%s','%s','%s','%s','%s');""" % (code, code, extent_auth_name, extent_code, 'EPSG', '1024')
                        all_sql.append(sql)

                        map_conversion_sql_to_code[sql_extract] = conv_code
                    else:
                        conv_code = map_conversion_sql_to_code[sql_extract]

                    sql = """INSERT INTO "projected_crs" VALUES('ESRI','%s','%s',NULL,'%s','%s','%s','%s','%s','%s',NULL,%d);""" % (
                        code, esri_name, cs_auth_name, cs_code, geogcs_auth_name, geogcs_code, conv_auth_name, conv_code, deprecated)
                    all_sql.append(sql)
                    sql = """INSERT INTO "usage" VALUES('ESRI', 'PCRS_%s_USAGE','projected_crs','ESRI','%s','%s','%s','%s','%s');""" % (code, code, extent_auth_name, extent_code, 'EPSG', '1024')
                    all_sql.append(sql)

                elif method == 'Lambert_Conformal_Conic' and 'Standard_Parallel_2' in parsed_conv_wkt:
                    assert len(parsed_conv_wkt) == 1 + 6 + 2
                    False_Easting = parsed_conv_wkt['False_Easting']
                    False_Northing = parsed_conv_wkt['False_Northing']
                    Central_Meridian = parsed_conv_wkt['Central_Meridian']
                    Standard_Parallel_1 = parsed_conv_wkt['Standard_Parallel_1']
                    Standard_Parallel_2 = parsed_conv_wkt['Standard_Parallel_2']
                    Latitude_Of_Origin = parsed_conv_wkt['Latitude_Of_Origin']

                    cs_auth_name, cs_code, uom_code = get_cs(parsed_conv_wkt)

                    conv_name = 'unnamed'
                    conv_auth_name = 'ESRI'
                    conv_code = code

                    sql = """INSERT INTO "conversion" VALUES('ESRI','%s','%s',NULL,'EPSG','9802','Lambert Conic Conformal (2SP)','EPSG','8821','Latitude of false origin',%s,'EPSG','%s','EPSG','8822','Longitude of false origin',%s,'EPSG','%s','EPSG','8823','Latitude of 1st standard parallel',%s,'EPSG','%s','EPSG','8824','Latitude of 2nd standard parallel',%s,'EPSG','%s','EPSG','8826','Easting at false origin',%s,'EPSG','%s','EPSG','8827','Northing at false origin',%s,'EPSG','%s',NULL,NULL,NULL,NULL,NULL,NULL,%d);""" % (
                        code, conv_name, Latitude_Of_Origin, ang_uom_code, Central_Meridian, ang_uom_code, Standard_Parallel_1, ang_uom_code, Standard_Parallel_2, ang_uom_code, False_Easting, uom_code, False_Northing, uom_code, deprecated)

                    sql_extract = sql[sql.find('NULL'):]
                    if conv_name != 'unnamed' or sql_extract not in map_conversion_sql_to_code:
                        all_sql.append(sql)

                        sql = """INSERT INTO "usage" VALUES('ESRI', 'CONV_%s_USAGE','conversion','ESRI','%s','%s','%s','%s','%s');""" % (code, code, extent_auth_name, extent_code, 'EPSG', '1024')
                        all_sql.append(sql)

                        map_conversion_sql_to_code[sql_extract] = conv_code
                    else:
                        conv_code = map_conversion_sql_to_code[sql_extract]

                    sql = """INSERT INTO "projected_crs" VALUES('ESRI','%s','%s',NULL,'%s','%s','%s','%s','%s','%s',NULL,%d);""" % (
                        code, esri_name, cs_auth_name, cs_code, geogcs_auth_name, geogcs_code, conv_auth_name, conv_code, deprecated)
                    all_sql.append(sql)
                    sql = """INSERT INTO "usage" VALUES('ESRI', 'PCRS_%s_USAGE','projected_crs','ESRI','%s','%s','%s','%s','%s');""" % (code, code, extent_auth_name, extent_code, 'EPSG', '1024')
                    all_sql.append(sql)

                elif method == 'Lambert_Conformal_Conic' and 'Scale_Factor' in parsed_conv_wkt:
                    assert len(parsed_conv_wkt) == 1 + 6 + 2
                    False_Easting = parsed_conv_wkt['False_Easting']
                    False_Northing = parsed_conv_wkt['False_Northing']
                    Central_Meridian = parsed_conv_wkt['Central_Meridian']
                    Standard_Parallel_1 = parsed_conv_wkt['Standard_Parallel_1']
                    Scale_Factor = parsed_conv_wkt['Scale_Factor']
                    Latitude_Of_Origin = parsed_conv_wkt['Latitude_Of_Origin']
                    assert Standard_Parallel_1 == Latitude_Of_Origin

                    cs_auth_name, cs_code, uom_code = get_cs(parsed_conv_wkt)

                    conv_name = 'unnamed'
                    conv_auth_name = 'ESRI'
                    conv_code = code

                    sql = """INSERT INTO "conversion" VALUES('ESRI','%s','%s',NULL,'EPSG','9801','Lambert Conic Conformal (1SP)','EPSG','8801','Latitude of natural origin',%s,'EPSG','%s','EPSG','8802','Longitude of natural origin',%s,'EPSG','%s','EPSG','8805','Scale factor at natural origin',%s,'EPSG','9201','EPSG','8806','False easting',%s,'EPSG','%s','EPSG','8807','False northing',%s,'EPSG','%s',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,%d);""" % (
                        code, conv_name, Latitude_Of_Origin, ang_uom_code, Central_Meridian, ang_uom_code, Scale_Factor, False_Easting, uom_code, False_Northing, uom_code, deprecated)

                    sql_extract = sql[sql.find('NULL'):]
                    if conv_name != 'unnamed' or sql_extract not in map_conversion_sql_to_code:
                        all_sql.append(sql)

                        sql = """INSERT INTO "usage" VALUES('ESRI', 'CONV_%s_USAGE','conversion','ESRI','%s','%s','%s','%s','%s');""" % (code, code, extent_auth_name, extent_code, 'EPSG', '1024')
                        all_sql.append(sql)

                        map_conversion_sql_to_code[sql_extract] = conv_code
                    else:
                        conv_code = map_conversion_sql_to_code[sql_extract]

                    sql = """INSERT INTO "projected_crs" VALUES('ESRI','%s','%s',NULL,'%s','%s','%s','%s','%s','%s',NULL,%d);""" % (
                        code, esri_name, cs_auth_name, cs_code, geogcs_auth_name, geogcs_code, conv_auth_name, conv_code, deprecated)
                    all_sql.append(sql)

                    sql = """INSERT INTO "usage" VALUES('ESRI', 'PCRS_%s_USAGE','projected_crs','ESRI','%s','%s','%s','%s','%s');""" % (code, code, extent_auth_name, extent_code, 'EPSG', '1024')
                    all_sql.append(sql)

                else:

                    sql = """INSERT INTO "projected_crs" VALUES('ESRI','%s','%s',NULL,NULL,NULL,'%s','%s',NULL,NULL,'%s',%d);""" % (
                        code, esri_name, geogcs_auth_name, geogcs_code, escape_literal(wkt), deprecated)
                    all_sql.append(sql)

                    sql = """INSERT INTO "usage" VALUES('ESRI', 'PCRS_%s_USAGE','projected_crs','ESRI','%s','%s','%s','%s','%s');""" % (code, code, extent_auth_name, extent_code, 'EPSG', '1024')
                    all_sql.append(sql)

                if deprecated and code != latestWkid:
                    mapDeprecatedToNonDeprecated[code] = latestWkid

        for deprecated in mapDeprecatedToNonDeprecated:
            code = deprecated
            latestWkid = mapDeprecatedToNonDeprecated[deprecated]

            if latestWkid in wkid_set:
                sql = """INSERT INTO "supersession" VALUES('projected_crs','ESRI','%s','projected_crs','ESRI','%s','ESRI',1);""" % (
                    code, latestWkid)
                all_sql.append(sql)
            else:
                cursor.execute(
                    "SELECT name FROM projected_crs WHERE auth_name = 'EPSG' AND code = ?", (latestWkid,))
                src_row = cursor.fetchone()
                assert src_row, row
                sql = """INSERT INTO "supersession" VALUES('projected_crs','ESRI','%s','projected_crs','EPSG','%s','ESRI',1);""" % (
                    code, latestWkid)
                all_sql.append(sql)


########################

map_vdatum_esri_name_to_auth_code = {}
map_vdatum_esri_to_parameters = {}


def import_vdatum():
    with open(os.path.join(path_to_csv, 'pe_list_vdatum.csv'), 'rt') as csvfile:
        reader = csv.reader(csvfile)
        header = next(reader)
        nfields = len(header)

        idx_wkid = header.index('wkid')
        assert idx_wkid >= 0

        idx_latestWkid = header.index('latestWkid')
        assert idx_latestWkid >= 0

        idx_name = header.index('name')
        assert idx_name >= 0

        idx_description = header.index('description')
        assert idx_description >= 0

        idx_wkt = header.index('wkt')
        assert idx_wkt >= 0

        idx_authority = header.index('authority')
        assert idx_authority >= 0

        idx_deprecated = header.index('deprecated')
        assert idx_deprecated >= 0

        while True:
            try:
                row = next(reader)
            except StopIteration:
                break
            assert len(row) == nfields, row

            wkid = row[idx_wkid]
            latestWkid = row[idx_latestWkid]
            authority = row[idx_authority]
            esri_name = row[idx_name]

            if authority == 'EPSG':

                map_vdatum_esri_name_to_auth_code[esri_name] = [
                    'EPSG', latestWkid]

                cursor.execute(
                    "SELECT name FROM vertical_datum WHERE auth_name = 'EPSG' AND code = ?", (latestWkid,))
                src_row = cursor.fetchone()
                assert src_row, row
                src_name = src_row[0]
                esri_name = row[idx_name]
                if src_name != esri_name:
                    sql = """INSERT INTO alias_name VALUES('vertical_datum','EPSG','%s','%s','ESRI');""" % (
                        wkid, escape_literal(esri_name))
                    all_sql.append(sql)
            else:
                assert authority.upper() == 'ESRI', row

                map_vdatum_esri_name_to_auth_code[esri_name] = ['ESRI', wkid]

                description = row[idx_description]
                deprecated = 1 if row[idx_deprecated] == 'yes' else 0

                map_vdatum_esri_to_parameters[wkid] = {
                    'esri_name': esri_name,
                    'description': description,
                    'deprecated': deprecated
                }
                # We cannot write it since we lack the area of use

########################


map_vertcs_esri_name_to_auth_code = {}


def import_vertcs():
    with open(os.path.join(path_to_csv, 'pe_list_vertcs.csv'), 'rt') as csvfile:
        reader = csv.reader(csvfile)
        header = next(reader)
        nfields = len(header)

        idx_wkid = header.index('wkid')
        assert idx_wkid >= 0

        idx_latestWkid = header.index('latestWkid')
        assert idx_latestWkid >= 0

        idx_name = header.index('name')
        assert idx_name >= 0

        idx_description = header.index('description')
        assert idx_description >= 0

        idx_wkt = header.index('wkt')
        assert idx_wkt >= 0

        idx_authority = header.index('authority')
        assert idx_authority >= 0

        idx_deprecated = header.index('deprecated')
        assert idx_deprecated >= 0

        idx_areaname = header.index('areaname')
        assert idx_areaname >= 0

        idx_slat = header.index('slat')
        assert idx_slat >= 0

        idx_nlat = header.index('nlat')
        assert idx_nlat >= 0

        idx_llon = header.index('llon')
        assert idx_llon >= 0

        idx_rlon = header.index('rlon')
        assert idx_rlon >= 0

        vdatum_written = set()

        sql = """-- vertical coordinate system for ellipsoidal height. Not really ISO 19111 valid..."""
        all_sql.append(sql)
        sql = """INSERT INTO "coordinate_system" VALUES('ESRI','ELLPS_HEIGHT_METRE','vertical',1);"""
        all_sql.append(sql)
        sql = """INSERT INTO "axis" VALUES('ESRI','ELLPS_HEIGHT_METRE','Ellipsoidal height','h','up','ESRI','ELLPS_HEIGHT_METRE',1,'EPSG','9001');"""
        all_sql.append(sql)

        while True:
            try:
                row = next(reader)
            except StopIteration:
                break
            assert len(row) == nfields, row

            code = row[idx_wkid]
            latestWkid = row[idx_latestWkid]
            authority = row[idx_authority]
            esri_name = row[idx_name]

            if authority == 'EPSG':

                map_vertcs_esri_name_to_auth_code[esri_name] = [
                    'EPSG', latestWkid]

                cursor.execute(
                    "SELECT name FROM vertical_crs WHERE auth_name = 'EPSG' AND code = ?", (latestWkid,))
                src_row = cursor.fetchone()
                assert src_row, row
                src_name = src_row[0]
                esri_name = row[idx_name]
                if src_name != esri_name:
                    sql = """INSERT INTO alias_name VALUES('vertical_crs','EPSG','%s','%s','ESRI');""" % (
                        code, escape_literal(esri_name))
                    all_sql.append(sql)
            else:
                assert authority.upper() == 'ESRI', row

                wkt = row[idx_wkt]

                pos = wkt.find('VDATUM["')
                is_vdatum = True
                if pos > 0:
                    pos += len('VDATUM["')
                    end_pos = wkt[pos:].find('"')
                    assert end_pos >= 0
                    end_pos += pos
                    datum_name = wkt[pos:end_pos]
                    if datum_name not in map_vdatum_esri_name_to_auth_code:
                        print('Skipping vertcs %s. Cannot find vertical datum %s' % (
                            str(row), datum_name))
                        sql = """-- Skipping vertcs %s. Cannot find vertical datum %s""" % (
                            esri_name, datum_name)
                        all_sql.append(sql)
                        continue
                    datum_auth_name, datum_code = map_vdatum_esri_name_to_auth_code[datum_name]
                else:
                    pos = wkt.find(',DATUM[')
                    assert pos > 0
                    pos += len(',DATUM["')
                    is_vdatum = False
                    end_pos = wkt[pos:].find('"')
                    assert end_pos >= 0
                    end_pos += pos
                    datum_name = wkt[pos:end_pos]
                    if datum_name not in map_datum_esri_name_to_auth_code:
                        print('Skipping vertcs %s. Cannot find geodetic datum %s' % (
                            str(row), datum_name))
                        sql = """-- Skipping vertcs %s. Cannot find geodetic datum %s""" % (
                            esri_name, datum_name)
                        all_sql.append(sql)
                        continue
                    datum_auth_name, datum_code = map_datum_esri_name_to_auth_code[datum_name]

                assert 'PARAMETER["Vertical_Shift",0.0]' in wkt, row

                deprecated = 1 if row[idx_deprecated] == 'yes' else 0

                extent_auth_name, extent_code = find_extent(
                    row[idx_areaname], row[idx_slat], row[idx_nlat], row[idx_llon], row[idx_rlon])

                if not is_vdatum:
                    new_datum_code = 'from_geogdatum_' + datum_auth_name + '_' + datum_code
                    if new_datum_code not in vdatum_written:
                        vdatum_written.add(new_datum_code)

                        p = map_datum_esri_to_parameters[datum_code]

                        datum_code = new_datum_code

                        sql = """INSERT INTO "vertical_datum" VALUES('ESRI','%s','%s',NULL,NULL,NULL,NULL,%d);""" % (
                            datum_code, p['esri_name'], p['deprecated'])
                        all_sql.append(sql)
                        sql = """INSERT INTO "usage" VALUES('ESRI', '%s_USAGE','vertical_datum','ESRI','%s','%s','%s','%s','%s');""" % (datum_code, datum_code, extent_auth_name, extent_code, 'EPSG', '1024')
                        all_sql.append(sql)
                    else:
                        datum_code = new_datum_code

                    datum_auth_name = 'ESRI'

                elif datum_auth_name == 'ESRI':
                    assert datum_code not in vdatum_written

                    vdatum_written.add(datum_code)

                    p = map_vdatum_esri_to_parameters[datum_code]
                    sql = """INSERT INTO "vertical_datum" VALUES('ESRI','%s','%s',NULL,NULL,NULL,NULL,%d);""" % (
                        datum_code, p['esri_name'], p['deprecated'])
                    all_sql.append(sql)
                    sql = """INSERT INTO "usage" VALUES('ESRI', '%s_USAGE','vertical_datum','ESRI','%s','%s','%s','%s','%s');""" % (datum_code, datum_code, extent_auth_name, extent_code, 'EPSG', '1024')
                    all_sql.append(sql)

                map_vertcs_esri_name_to_auth_code[esri_name] = ['ESRI', code]

                cs_auth = 'EPSG'
                if 'PARAMETER["Direction",1.0]' in wkt and 'UNIT["Meter"' in wkt:
                    cs_code = 6499
                elif 'PARAMETER["Direction",1.0]' in wkt and 'UNIT["Foot"' in wkt:
                    cs_code = 1030
                elif 'PARAMETER["Direction",1.0]' in wkt and 'UNIT["Foot_US"' in wkt:
                    cs_code = 6497
                else:
                    assert False, ('unknown coordinate system for %s' %
                                   str(row))
                if not is_vdatum:
                    assert cs_code == 6499
                    cs_auth = 'ESRI'
                    cs_code = 'ELLPS_HEIGHT_METRE'

                sql = """INSERT INTO "vertical_crs" VALUES('ESRI','%s','%s',NULL,'%s','%s','%s','%s',%d);""" % (
                    code, esri_name, cs_auth, cs_code, datum_auth_name, datum_code, deprecated)
                all_sql.append(sql)
                sql = """INSERT INTO "usage" VALUES('ESRI', '%s_USAGE','vertical_crs','ESRI','%s','%s','%s','%s','%s');""" % (code, code, extent_auth_name, extent_code, 'EPSG', '1024')
                all_sql.append(sql)

                if deprecated and code != latestWkid:
                    cursor.execute(
                        "SELECT name FROM vertical_crs WHERE auth_name = 'EPSG' AND code = ?", (latestWkid,))
                    src_row = cursor.fetchone()
                    assert src_row

                    sql = """INSERT INTO "supersession" VALUES('vertical_crs','ESRI','%s','vertical_crs','EPSG','%s','ESRI',1);""" % (
                        code, latestWkid)
                    all_sql.append(sql)


########################

map_compoundcrs_esri_name_to_auth_code = {}


def import_hvcoordsys():
    with open(os.path.join(path_to_csv, 'pe_list_hvcoordsys.csv'), 'rt') as csvfile:
        reader = csv.reader(csvfile)
        header = next(reader)
        nfields = len(header)

        idx_wkid = header.index('wkid')
        assert idx_wkid >= 0

        idx_latestWkid = header.index('latestWkid')
        assert idx_latestWkid >= 0

        idx_name = header.index('name')
        assert idx_name >= 0

        idx_description = header.index('description')
        assert idx_description >= 0

        idx_wkt = header.index('wkt')
        assert idx_wkt >= 0

        idx_authority = header.index('authority')
        assert idx_authority >= 0

        idx_deprecated = header.index('deprecated')
        assert idx_deprecated >= 0

        idx_areaname = header.index('areaname')
        assert idx_areaname >= 0

        idx_slat = header.index('slat')
        assert idx_slat >= 0

        idx_nlat = header.index('nlat')
        assert idx_nlat >= 0

        idx_llon = header.index('llon')
        assert idx_llon >= 0

        idx_rlon = header.index('rlon')
        assert idx_rlon >= 0

        while True:
            try:
                row = next(reader)
            except StopIteration:
                break
            assert len(row) == nfields, row

            code = row[idx_wkid]
            latestWkid = row[idx_latestWkid]
            authority = row[idx_authority]
            esri_name = row[idx_name]

            if authority == 'EPSG':

                map_compoundcrs_esri_name_to_auth_code[esri_name] = [
                    'EPSG', latestWkid]

                cursor.execute(
                    "SELECT name FROM compound_crs WHERE auth_name = 'EPSG' AND code = ?", (latestWkid,))
                src_row = cursor.fetchone()
                assert src_row, row
                src_name = src_row[0]
                esri_name = row[idx_name]
                if src_name != esri_name:
                    sql = """INSERT INTO alias_name VALUES('compound_crs','EPSG','%s','%s','ESRI');""" % (
                        code, escape_literal(esri_name))
                    all_sql.append(sql)
            else:
                assert False, row  # no ESRI specific entries at that time !


########################


def get_parameter(wkt, param_name):
    needle = ',PARAMETER["' + param_name + '",'
    pos = wkt.find(needle)
    assert pos >= 0, wkt
    pos += len(needle)
    end_pos = wkt[pos:].find(']')
    assert end_pos >= 0, (wkt, wkt[pos:])
    end_pos += pos
    return wkt[pos:end_pos]


def import_geogtran():
    with open(os.path.join(path_to_csv, 'pe_list_geogtran.csv'), 'rt') as csvfile:
        reader = csv.reader(csvfile)
        header = next(reader)
        nfields = len(header)

        idx_wkid = header.index('wkid')
        assert idx_wkid >= 0

        idx_latestWkid = header.index('latestWkid')
        assert idx_latestWkid >= 0

        idx_name = header.index('name')
        assert idx_name >= 0

        idx_description = header.index('description')
        assert idx_description >= 0

        idx_wkt = header.index('wkt')
        assert idx_wkt >= 0

        idx_authority = header.index('authority')
        assert idx_authority >= 0

        idx_deprecated = header.index('deprecated')
        assert idx_deprecated >= 0

        idx_areaname = header.index('areaname')
        assert idx_areaname >= 0

        idx_slat = header.index('slat')
        assert idx_slat >= 0

        idx_nlat = header.index('nlat')
        assert idx_nlat >= 0

        idx_llon = header.index('llon')
        assert idx_llon >= 0

        idx_rlon = header.index('rlon')
        assert idx_rlon >= 0

        idx_accuracy = header.index('accuracy')
        assert idx_accuracy >= 0

        while True:
            try:
                row = next(reader)
            except StopIteration:
                break
            assert len(row) == nfields, row

            wkid = row[idx_wkid]
            authority = row[idx_authority]
            esri_name = row[idx_name]
            wkt = row[idx_wkt]
            deprecated = 1 if row[idx_deprecated] == 'yes' else 0

            if authority == 'EPSG':

                map_compoundcrs_esri_name_to_auth_code[esri_name] = [
                    'EPSG', wkid]

                cursor.execute(
                    "SELECT name FROM coordinate_operation_view WHERE auth_name = 'EPSG' AND code = ?", (wkid,))
                src_row = cursor.fetchone()

                if not src_row:
                    if 'Molodensky_Badekas' in wkt:
                        # print('Skipping GEOGTRAN %s (EPSG source) since it uses a non-supported yet suported method'% esri_name)
                        continue
                    if 'NADCON5' in wkt:
                        print('Skipping NADCON5 %s (EPSG source) since it uses a non-supported yet suported method'% esri_name)
                        continue

                # Don't do anything particular except checking we know it
                assert src_row, row

            else:

                # We don't want to import ESRI deprecated transformations
                # (there are a lot), do we ?
                if deprecated:
                    #print('Skipping deprecated GEOGTRAN %s' % esri_name)
                    continue

                assert wkt.startswith('GEOGTRAN')

                pos = wkt.find(',GEOGCS["')
                assert pos >= 0
                pos += len(',GEOGCS["')
                end_pos = wkt[pos:].find('"')
                assert end_pos >= 0
                end_pos += pos
                src_crs_name = wkt[pos:end_pos]

                pos = wkt[end_pos:].find(',GEOGCS["')
                assert pos >= 0
                pos += end_pos + len(',GEOGCS["')
                end_pos = wkt[pos:].find('"')
                assert end_pos >= 0
                end_pos += pos
                dst_crs_name = wkt[pos:end_pos]

                assert src_crs_name in map_geogcs_esri_name_to_auth_code, (
                    src_crs_name, row)
                src_crs_auth_name, src_crs_code = map_geogcs_esri_name_to_auth_code[src_crs_name]

                assert dst_crs_name in map_geogcs_esri_name_to_auth_code, (
                    dst_crs_name, row)
                dst_crs_auth_name, dst_crs_code = map_geogcs_esri_name_to_auth_code[dst_crs_name]

                is_longitude_rotation = 'METHOD["Longitude_Rotation"]' in wkt
                if is_longitude_rotation:
                    # skip it as it is automatically handled by PROJ
                    continue

                is_cf = 'METHOD["Coordinate_Frame"]' in wkt
                is_pv = 'METHOD["Position_Vector"]' in wkt
                is_geocentric_translation = 'METHOD["Geocentric_Translation"]' in wkt
                is_geog2d_offset = 'METHOD["Geographic_2D_Offset"]' in wkt
                is_null = 'METHOD["Null"]' in wkt
                is_unitchange = 'METHOD["Unit_Change"]' in wkt
                is_nadcon = 'METHOD["NADCON"]' in wkt
                is_ntv2 = 'METHOD["NTv2"]' in wkt
                is_geocon = 'METHOD["GEOCON"]' in wkt
                is_harn = 'METHOD["HARN"]' in wkt
                is_molodensky_badekas = 'METHOD["Molodensky_Badekas"]' in wkt
                is_Time_Based_Helmert_Position_Vector = 'METHOD["Time_Based_Helmert_Position_Vector"]' in wkt
                is_Time_Based_Helmert_Coordinate_Frame = 'METHOD["Time_Based_Helmert_Coordinate_Frame"]' in wkt
                assert is_cf or is_pv or is_geocentric_translation or is_molodensky_badekas or is_nadcon or is_geog2d_offset or is_ntv2 or is_geocon or is_null or is_harn or is_unitchange or is_Time_Based_Helmert_Position_Vector or is_Time_Based_Helmert_Coordinate_Frame, row

                extent_auth_name, extent_code = find_extent(
                    row[idx_areaname], row[idx_slat], row[idx_nlat], row[idx_llon], row[idx_rlon])

                accuracy = row[idx_accuracy]
                if float(accuracy) == 999:
                    accuracy = 'NULL'

                if is_cf or is_pv:
                    x = get_parameter(wkt, 'X_Axis_Translation')
                    y = get_parameter(wkt, 'Y_Axis_Translation')
                    z = get_parameter(wkt, 'Z_Axis_Translation')
                    rx = get_parameter(wkt, 'X_Axis_Rotation')  # in arc second
                    ry = get_parameter(wkt, 'Y_Axis_Rotation')
                    rz = get_parameter(wkt, 'Z_Axis_Rotation')
                    s = get_parameter(wkt, 'Scale_Difference')  # in ppm
                    assert wkt.count('PARAMETER[') == 7

                    if is_cf:
                        method_code = '9607'
                        method_name = 'Coordinate Frame rotation (geog2D domain)'
                    else:
                        method_code = '9606'
                        method_name = 'Position Vector transformation (geog2D domain)'

                    sql = """INSERT INTO "helmert_transformation" VALUES('ESRI','%s','%s',NULL,'EPSG','%s','%s','%s','%s','%s','%s',%s,%s,%s,%s,'EPSG','9001',%s,%s,%s,'EPSG','9104',%s,'EPSG','9202',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,%d);""" % (
                        wkid, esri_name, method_code, method_name, src_crs_auth_name, src_crs_code, dst_crs_auth_name, dst_crs_code, accuracy, x, y, z, rx, ry, rz, s, deprecated)
                    all_sql.append(sql)
                    sql = """INSERT INTO "usage" VALUES('ESRI', '%s_USAGE','helmert_transformation','ESRI','%s','%s','%s','%s','%s');""" % (wkid, wkid, extent_auth_name, extent_code, 'EPSG', '1024')
                    all_sql.append(sql)

                elif is_molodensky_badekas:
                    x = get_parameter(wkt, 'X_Axis_Translation')
                    y = get_parameter(wkt, 'Y_Axis_Translation')
                    z = get_parameter(wkt, 'Z_Axis_Translation')
                    rx = get_parameter(wkt, 'X_Axis_Rotation')  # in arc second
                    ry = get_parameter(wkt, 'Y_Axis_Rotation')
                    rz = get_parameter(wkt, 'Z_Axis_Rotation')
                    s = get_parameter(wkt, 'Scale_Difference')  # in ppm
                    px = get_parameter(wkt, 'X_Coordinate_of_Rotation_Origin')
                    py = get_parameter(wkt, 'Y_Coordinate_of_Rotation_Origin')
                    pz = get_parameter(wkt, 'Z_Coordinate_of_Rotation_Origin')
                    assert wkt.count('PARAMETER[') == 10

                    # The ESRI naming is not really clear about the convention
                    # but it looks like it is Coordinate Frame when comparing ESRI:1066 (Amersfoort_To_ETRS_1989_MB) with EPSG:1066
                    method_code = '9636'
                    method_name = 'Molodensky-Badekas (CF geog2D domain)'

                    sql = """INSERT INTO "helmert_transformation" VALUES('ESRI','%s','%s',NULL,'EPSG','%s','%s','%s','%s','%s','%s',%s,%s,%s,%s,'EPSG','9001',%s,%s,%s,'EPSG','9104',%s,'EPSG','9202',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,%s,%s,%s,'EPSG','9001',NULL,%d);""" % (
                        wkid, esri_name, method_code, method_name, src_crs_auth_name, src_crs_code, dst_crs_auth_name, dst_crs_code, accuracy, x, y, z, rx, ry, rz, s, px, py, pz, deprecated)
                    all_sql.append(sql)
                    sql = """INSERT INTO "usage" VALUES('ESRI', '%s_USAGE','helmert_transformation','ESRI','%s','%s','%s','%s','%s');""" % (wkid, wkid, extent_auth_name, extent_code, 'EPSG', '1024')
                    all_sql.append(sql)

                elif is_geocentric_translation:
                    x = get_parameter(wkt, 'X_Axis_Translation')
                    y = get_parameter(wkt, 'Y_Axis_Translation')
                    z = get_parameter(wkt, 'Z_Axis_Translation')
                    assert wkt.count('PARAMETER[') == 3

                    method_code = '9603'
                    method_name = 'Geocentric translations (geog2D domain)'

                    sql = """INSERT INTO "helmert_transformation" VALUES('ESRI','%s','%s',NULL,'EPSG','%s','%s','%s','%s','%s','%s',%s,%s,%s,%s,'EPSG','9001',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,%d);""" % (
                        wkid, esri_name, method_code, method_name, src_crs_auth_name, src_crs_code, dst_crs_auth_name, dst_crs_code, accuracy, x, y, z, deprecated)
                    all_sql.append(sql)
                    sql = """INSERT INTO "usage" VALUES('ESRI', '%s_USAGE','helmert_transformation','ESRI','%s','%s','%s','%s','%s');""" % (wkid, wkid, extent_auth_name, extent_code, 'EPSG', '1024')
                    all_sql.append(sql)

                elif is_Time_Based_Helmert_Position_Vector or is_Time_Based_Helmert_Coordinate_Frame:

                    x = get_parameter(wkt, 'X_Axis_Translation')
                    y = get_parameter(wkt, 'Y_Axis_Translation')
                    z = get_parameter(wkt, 'Z_Axis_Translation')
                    rx = get_parameter(wkt, 'X_Axis_Rotation')  # in arc second
                    ry = get_parameter(wkt, 'Y_Axis_Rotation')
                    rz = get_parameter(wkt, 'Z_Axis_Rotation')
                    s = get_parameter(wkt, 'Scale_Difference')  # in ppm
                    rate_x = get_parameter(wkt, 'X_Axis_Translation_Rate')
                    rate_y = get_parameter(wkt, 'Y_Axis_Translation_Rate')
                    rate_z = get_parameter(wkt, 'Z_Axis_Translation_Rate')
                    rate_rx = get_parameter(wkt, 'X_Axis_Rotation_Rate')  # in arc second / year
                    rate_ry = get_parameter(wkt, 'Y_Axis_Rotation_Rate')
                    rate_rz = get_parameter(wkt, 'Z_Axis_Rotation_Rate')
                    rate_s = get_parameter(wkt, 'Scale_Difference_Rate')  # in ppm / year
                    reference_time = get_parameter(wkt, 'Reference_Time')
                    assert wkt.count('PARAMETER[') == 15

                    if is_Time_Based_Helmert_Coordinate_Frame:
                        method_code = '1057'
                        method_name = 'Time-dependent Coordinate Frame rotation (geog2D)'
                    else:
                        method_code = '1054'
                        method_name = 'Time-dependent Position Vector tfm (geog2D)'

                    sql = """INSERT INTO "helmert_transformation" VALUES('ESRI','%s','%s',NULL,'EPSG','%s','%s','%s','%s','%s','%s',%s,%s,%s,%s,'EPSG','9001',%s,%s,%s,'EPSG','9104',%s,'EPSG','9202',%s,%s,%s,'EPSG','1042',%s,%s,%s,'EPSG','1043',%s,'EPSG','1041',%s,'EPSG','1029',NULL,NULL,NULL,NULL,NULL,NULL,%d);""" % (
                        wkid, esri_name, method_code, method_name, src_crs_auth_name, src_crs_code, dst_crs_auth_name, dst_crs_code, accuracy, x, y, z, rx, ry, rz, s, rate_x, rate_y, rate_z, rate_rx, rate_ry, rate_rz, rate_s, reference_time, deprecated)
                    all_sql.append(sql)
                    sql = """INSERT INTO "usage" VALUES('ESRI', '%s_USAGE','helmert_transformation','ESRI','%s','%s','%s','%s','%s');""" % (wkid, wkid, extent_auth_name, extent_code, 'EPSG', '1024')
                    all_sql.append(sql)

                elif is_geog2d_offset:

                    # The only occurence is quite boring: from NTF(Paris) to NTF.
                    # But interestingly the longitude offset value is not
                    # completely exactly the value of the Paris prime meridian

                    long_offset = get_parameter(
                        wkt, 'Longitude_Offset')  # in arc second
                    lat_offset = get_parameter(wkt, 'Latitude_Offset')
                    assert wkt.count('PARAMETER[') == 2

                    sql = """INSERT INTO "other_transformation" VALUES('ESRI','%s','%s',NULL,'EPSG','9619','Geographic2D offsets','%s','%s','%s','%s',%s,'EPSG','8601','Latitude offset',%s,'EPSG','9104','EPSG','8602','Longitude offset',%s,'EPSG','9104',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,%d);""" % (
                        wkid, esri_name, src_crs_auth_name, src_crs_code, dst_crs_auth_name, dst_crs_code, accuracy, lat_offset, long_offset, deprecated)
                    all_sql.append(sql)
                    sql = """INSERT INTO "usage" VALUES('ESRI', '%s_USAGE','other_transformation','ESRI','%s','%s','%s','%s','%s');""" % (wkid, wkid, extent_auth_name, extent_code, 'EPSG', '1024')
                    all_sql.append(sql)

                elif is_null:
                    long_offset = '0'
                    lat_offset = '0'
                    assert wkt.count('PARAMETER[') == 0

                    sql = """INSERT INTO "other_transformation" VALUES('ESRI','%s','%s',NULL,'EPSG','9619','Geographic2D offsets','%s','%s','%s','%s',%s,'EPSG','8601','Latitude offset',%s,'EPSG','9104','EPSG','8602','Longitude offset',%s,'EPSG','9104',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,%d);""" % (
                        wkid, esri_name, src_crs_auth_name, src_crs_code, dst_crs_auth_name, dst_crs_code, accuracy, lat_offset, long_offset, deprecated)
                    all_sql.append(sql)
                    sql = """INSERT INTO "usage" VALUES('ESRI', '%s_USAGE','other_transformation','ESRI','%s','%s','%s','%s','%s');""" % (wkid, wkid, extent_auth_name, extent_code, 'EPSG', '1024')
                    all_sql.append(sql)

                elif is_unitchange:

                    # Automatically handled by PROJ. Not worth importing
                    continue

                else:
                    assert wkt.count('PARAMETER[') == 1
                    needle = ',PARAMETER["'
                    pos = wkt.find(needle)
                    assert pos >= 0, wkt
                    pos += len(needle)
                    end_pos = wkt[pos:].find('"')
                    assert end_pos >= 0, (wkt, wkt[pos:])
                    end_pos += pos
                    filename = wkt[pos:end_pos]
                    assert filename.startswith('Dataset_')
                    filename = filename[len('Dataset_'):]


                    cursor.execute(
                        "SELECT g.name, g.grid_name FROM grid_transformation g JOIN usage u ON u.object_table_name = 'grid_transformation' AND u.object_auth_name = g.auth_name AND u.object_code = g.code JOIN extent e ON u.extent_auth_name = e.auth_name AND u.extent_code = e.code WHERE g.auth_name != 'ESRI' AND g.source_crs_auth_name = ? AND g.source_crs_code = ? AND g.target_crs_auth_name = ? AND g.target_crs_code = ? AND e.auth_name = ? AND e.code = ?", (src_crs_auth_name, src_crs_code, dst_crs_auth_name, dst_crs_code, extent_auth_name, extent_code))
                    src_row = cursor.fetchone()
                    if src_row:
                        print('A grid_transformation (%s, using grid %s) is already known for the equivalent of %s (%s:%s --> %s:%s) for area %s, which uses grid %s. Skipping it' % (src_row[0], src_row[1], esri_name, src_crs_auth_name, src_crs_code, dst_crs_auth_name, dst_crs_code, row[idx_areaname], filename))
                        continue

                    sql = """INSERT INTO "grid_transformation" VALUES('ESRI','%s','%s',NULL,'EPSG','9615','NTv2','%s','%s','%s','%s',%s,'EPSG','8656','Latitude and longitude difference file','%s',NULL,NULL,NULL,NULL,NULL,NULL,NULL,%d);""" % (
                        wkid, esri_name, src_crs_auth_name, src_crs_code, dst_crs_auth_name, dst_crs_code, accuracy, filename, deprecated)
                    all_sql.append(sql)
                    sql = """INSERT INTO "usage" VALUES('ESRI', '%s_USAGE','grid_transformation','ESRI','%s','%s','%s','%s','%s');""" % (wkid, wkid, extent_auth_name, extent_code, 'EPSG', '1024')
                    all_sql.append(sql)

                    global manual_grids
                    if ("'" + filename + "'") not in manual_grids:
                        print('not handled grid: ' + filename)
                        manual_grids += "-- '" + filename + "': no mapping\n"


import_linunit()
import_spheroid()
import_prime_meridian()
import_datum()
import_geogcs()
import_projcs()
import_vdatum()
import_vertcs()
import_hvcoordsys()  # compoundcrs
import_geogtran()  # transformations between GeogCRS

script_dir_name = os.path.dirname(os.path.realpath(__file__))
sql_dir_name = os.path.join(os.path.dirname(script_dir_name), 'data', 'sql')

f = open(os.path.join(sql_dir_name, 'esri') + '.sql', 'wb')
f.write("--- This file has been generated by scripts/build_db_from_esri.py. DO NOT EDIT !\n\n".encode('UTF-8'))
for sql in all_sql:
    f.write((sql + '\n').encode('UTF-8'))
f.write(manual_grids.encode('UTF-8'))
f.close()

print('')
print('Finished !')
print('NOTE: adding into metadata: ESRI.VERSION = %s. Update if needed !' % version)
