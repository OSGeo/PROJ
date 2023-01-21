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

import argparse
import csv
import os
import re
import sqlite3
from dataclasses import dataclass
from enum import Enum
from pathlib import Path
from typing import Optional, List, Dict

parser = argparse.ArgumentParser()
parser.add_argument('esri_csv_dir', help='Path to ESRI CSV dir, typically the path '
                                         'to the "csv" directory of a "git clone '
                                         'https://github.com/Esri/projection-engine-db-doc',
                    type=Path)
parser.add_argument('proj_db', help='Path to current proj.db file', type=Path)
parser.add_argument('version', help='ArcMap version string, e.g. "ArcMap 10.8.1"')
parser.add_argument('date', help='ArcMap version date as a yyyy-MM-dd string, e.g. "2020-05-24"')
args = parser.parse_args()

path_to_csv = args.esri_csv_dir
proj_db = args.proj_db
version = args.version
date = args.date

conn = sqlite3.connect(proj_db)
cursor = conn.cursor()

all_sql = ["""INSERT INTO "metadata" VALUES('ESRI.VERSION', '{}');""".format(version),
           """INSERT INTO "metadata" VALUES('ESRI.DATE', '{}');""".format(date)]

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
('spain/BALR2009','es_ign_SPED2ETV2.tif',NULL,'GTiff','hgridshift',0,NULL,'https://cdn.proj.org/es_ign_SPED2ETV2.tif',1,1,NULL),
('spain/peninsula','es_ign_SPED2ETV2.tif',NULL,'GTiff','hgridshift',0,NULL,'https://cdn.proj.org/es_ign_SPED2ETV2.tif',1,1,NULL),
('spain/baleares','es_ign_SPED2ETV2.tif',NULL,'GTiff','hgridshift',0,NULL,'https://cdn.proj.org/es_ign_SPED2ETV2.tif',1,1,NULL);
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
        # print('unknown extent inserted: ' + extentname)

        if float(rlon) > 180:
            new_rlon = '%s' % (float(rlon) - 360)
            print('Correcting rlon from %s to %s for %s' %
                  (rlon, new_rlon, extentname))
            rlon = new_rlon

        assert -90 <= float(slat) <= 90, (extentname,
                                          slat, nlat, llon, rlon)
        assert -90 <= float(nlat) <= 90, (extentname,
                                          slat, nlat, llon, rlon)
        assert float(nlat) > float(slat), (extentname, slat, nlat, llon, rlon)
        assert -180 <= float(llon) <= 180, (extentname,
                                            slat, nlat, llon, rlon)
        assert -180 <= float(rlon) <= 180, (extentname,
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
    with open(path_to_csv / 'pe_list_linunit.csv', 'rt') as csvfile:
        reader = csv.reader(csvfile)
        header = next(reader)
        nfields = len(header)

        idx_wkid = header.index('wkid')
        assert idx_wkid >= 0

        idx_latestWkid = header.index('latestWkid')
        assert idx_latestWkid >= 0

        idx_name = header.index('name')
        assert idx_name >= 0

        idx_wkt2 = header.index('wkt2')
        assert idx_wkt2 >= 0

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

            wkt = row[idx_wkt2]
            assert wkt.startswith('LENGTHUNIT[') and wkt.endswith(']')
            tokens = wkt[len('LENGTHUNIT['):len(wkt) - 1].split(',')
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
    with open(path_to_csv / 'pe_list_spheroid.csv', 'rt') as csvfile:
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

        idx_wkt2 = header.index('wkt2')
        assert idx_wkt2 >= 0

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

                wkt2 = row[idx_wkt2]
                wkt2_tokens_re = re.compile(r'ELLIPSOID\[""?(.*?)""?,(-?[\d]+(?:\.[\d]*)?),(-?[\d]+(?:\.[\d]*)?),LENGTHUNIT\[""?(.*?)""?,(-?[\d]+(?:\.[\d]*)?)]]')
                match = wkt2_tokens_re.match(wkt2)
                assert match, wkt2

                a = match.group(2)
                rf = match.group(3)
                length_unit = match.group(4)
                unit_size = float(match.group(5))
                assert length_unit == 'Meter', 'Unhandled spheroid unit: {}'.format(length_unit)
                assert unit_size == 1, 'Unhandled spheroid unit size: {}'.format(unit_size)

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
    with open(path_to_csv / 'pe_list_primem.csv', 'rt') as csvfile:
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

        idx_wkt2 = header.index('wkt2')
        assert idx_wkt2 >= 0

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

                wkt2 = row[idx_wkt2]
                wkt2_tokens_re = re.compile(r'PRIMEM\[""?(.*?)""?,(-?[\d]+(?:\.[\d]*)?),ANGLEUNIT\[""?(.*?)""?,(-?[\d]+(?:\.[\d]*)?)]]')
                match = wkt2_tokens_re.match(wkt2)
                assert match, wkt2

                value = match.group(2)
                angle_unit = match.group(3)
                unit_size = float(match.group(4))
                assert angle_unit == 'Degree', 'Unhandled prime meridian unit: {}'.format(angle_unit)
                assert unit_size == 0.0174532925199433, 'Unhandled prime meridian unit size: {}'.format(unit_size)

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
    with open(path_to_csv / 'pe_list_datum.csv', 'rt') as csvfile:
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

        idx_wkt2 = header.index('wkt2')
        assert idx_wkt2 >= 0

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

                wkt2 = row[idx_wkt2]
                wkt2_tokens_re = re.compile(
                    r'DATUM\[""?(.*?)""?,\s*ELLIPSOID\[""?(.*?)""?,\s*(-?[\d]+(?:\.[\d]*)?),\s*(-?[\d]+(?:\.[\d]*)?),\s*LENGTHUNIT\[""?(.*?)""?,\s*(-?[\d]+(?:\.[\d]*)?)]]]')
                match = wkt2_tokens_re.match(wkt2)
                ellps_name = match.group(2)

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
    with open(path_to_csv / 'pe_list_geogcs.csv', 'rt') as csvfile:
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

        idx_wkt2 = header.index('wkt2')
        assert idx_wkt2 >= 0

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

            if code == latestWkid and authority.upper() == 'ESRI':
                cursor.execute(
                    "SELECT name FROM geodetic_crs WHERE auth_name = 'EPSG' AND code = ?", (latestWkid,))
                src_row = cursor.fetchone()
                if src_row:
                    src_name = src_row[0]
                    modified_epsg_name = src_name.replace(' ', '_')
                    if modified_epsg_name.upper() == esri_name.upper() or modified_epsg_name.upper() + "_3D" == esri_name.upper() or modified_epsg_name.upper() + "_(3D)" == esri_name.upper():
                        print("GeogCRS ESRI:%s (%s) has the same name as EPSG:%s. Fixing authority to be EPSG" % (latestWkid, esri_name, latestWkid))
                        authority = "EPSG"

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

                wkt2 = row[idx_wkt2]
                wkt2_datum_re = re.compile(r'.*DATUM\[""?(.*?)""?.*')
                match = wkt2_datum_re.match(wkt2)
                assert match, wkt2
                datum_name = match.group(1)

                # strip datum out of wkt
                wkt2 = re.sub(r'DATUM\[""?(.*?)""?,\s*ELLIPSOID\[""?(.*?)""?,\s*(-?[\d]+(?:\.[\d]*)?),\s*(-?[\d]+(?:\.[\d]*)?),\s*LENGTHUNIT\[""?(.*?)""?,\s*(-?[\d]+(?:\.[\d]*)?)]]],?', '', wkt2)

                assert datum_name in map_datum_esri_name_to_auth_code, (
                    datum_name, row)
                datum_auth_name, datum_code = map_datum_esri_name_to_auth_code[datum_name]

                wkt2_datum_re = re.compile(r'.*PRIMEM\[""?(.*?)""?.*')
                match = wkt2_datum_re.match(wkt2)
                assert match, wkt2
                pm_name = match.group(1)

                # strip prime meridian out of wkt
                wkt2 = re.sub(r'PRIMEM\[""?(.*?)""?,(-?[\d]+(?:\.[\d]*)?),ANGLEUNIT\[""?(.*?)""?,(-?[\d]+(?:\.[\d]*)?)]],?', '', wkt2)

                assert pm_name in map_pm_esri_name_to_auth_code, (pm_name, row)
                pm_auth_name, pm_code = map_pm_esri_name_to_auth_code[pm_name]

                wkt2_angle_unit_re = re.compile(
                    r'.*ANGLEUNIT\[""?(.*?)""?,(-?[\d]+(?:\.[\d]*)?)].*')
                match = wkt2_angle_unit_re.match(wkt2)
                assert match, wkt2

                angle_unit = match.group(1)
                assert angle_unit in ('Degree', 'Grad'), 'Unhandled angle unit {}'.format(angle_unit)

                is_degree = angle_unit == 'Degree'
                is_grad = angle_unit == 'Grad'

                assert is_degree or is_grad, row
                cs_code = '6422' if is_degree else '6403'

                deprecated = 1 if row[idx_deprecated] == 'yes' else 0

                extent_auth_name, extent_code = find_extent(
                    row[idx_areaname], row[idx_slat], row[idx_nlat], row[idx_llon], row[idx_rlon])

                if datum_auth_name == 'ESRI':
                    if datum_code not in datum_written:
                        datum_written.add(datum_code)

                        p = map_datum_esri_to_parameters[datum_code]

                        sql = """INSERT INTO "geodetic_datum" VALUES('ESRI','%s','%s','%s','%s','%s','%s','%s',NULL,NULL,NULL,NULL,%d);""" % (
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

                            sql = """INSERT INTO "geodetic_datum" VALUES('ESRI','%s','%s','%s','%s','%s','%s','%s',NULL,NULL,NULL,NULL,%d);""" % (
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

                if deprecated and code != latestWkid and code not in ('4305', '4812'):  # Voirol 1960 no longer in EPSG
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
    return {s[0:pos]: parse_wkt_array(s[pos+1:-1], level + 1)}


def parse_wkt_array(s, level=0):
    ar = []
    in_string = False
    cur_token = ''
    indent_level = 0

    for c in s:
        if in_string:
            if c == '"':
                in_string = False
            else:
                cur_token += c
        elif c == '"':
            in_string = True
        elif c == '[':
            cur_token += c
            indent_level += 1
        elif c == ']':
            cur_token += c
            indent_level -= 1
            assert indent_level >= 0
        elif indent_level == 0 and c == ',':
            ar.append(parse_wkt(cur_token, level + 1))
            cur_token = ''
        else:
            cur_token += c
    assert indent_level == 0
    if cur_token:
        ar.append(parse_wkt(cur_token, level + 1))

    if level == 0:
        return wkt_array_to_dict(ar)
    else:
        return ar


def wkt_array_to_dict(ar):
    d = {}
    for elt in ar:
        assert isinstance(elt, dict), elt
        assert len(elt) == 1
        if 'PROJECTION' in elt:
            assert len(elt['PROJECTION']) == 1, elt['PROJECTION']
            assert 'PROJECTION' not in d
            name = elt['PROJECTION'][0]
            d['PROJECTION'] = name
        elif 'CONVERSION' in elt:
            d['CONVERSION'] = [elt['CONVERSION'][0], wkt_array_to_dict(elt['CONVERSION'][1:])]
        elif 'COORDINATEOPERATION' in elt:
            d['COORDINATEOPERATION'] = [elt['COORDINATEOPERATION'][0], wkt_array_to_dict(elt['COORDINATEOPERATION'][1:])]
        elif 'SOURCECRS' in elt:
            assert len(elt['SOURCECRS']) == 1, elt['SOURCECRS']
            d['SOURCECRS'] = elt['SOURCECRS'][0]
        elif 'TARGETCRS' in elt:
            assert len(elt['TARGETCRS']) == 1, elt['TARGETCRS']
            d['TARGETCRS'] = elt['TARGETCRS'][0]
        elif 'VERTCRS' in elt:
            d['VERTCRS'] = [elt['VERTCRS'][0], wkt_array_to_dict(elt['VERTCRS'][1:])]
        elif 'VDATUM' in elt:
            assert len(elt['VDATUM']) == 1, elt['VDATUM']
            d['VDATUM'] = elt['VDATUM'][0]
        elif 'CS' in elt:
            assert len(elt['CS']) == 2, elt['CS']
            d['CS'] = elt['CS']
        elif 'AXIS' in elt:
            assert len(elt['AXIS']) == 3
            d['AXIS'] = [elt['AXIS'][0], elt['AXIS'][1], wkt_array_to_dict(elt['AXIS'][2:])]
        elif 'DATUM' in elt:
            d['DATUM'] = [elt['DATUM'][0], wkt_array_to_dict(elt['DATUM'][1:])]
        elif 'METHOD' in elt:
            assert len(elt['METHOD']) == 1, elt['METHOD']
            d['METHOD'] = elt['METHOD'][0]
        elif 'PARAMETERFILE' in elt:
            assert len(elt['PARAMETERFILE']) == 1, elt['PARAMETERFILE']
            d['PARAMETERFILE'] = elt['PARAMETERFILE'][0]
        elif 'OPERATIONACCURACY' in elt:
            assert len(elt['OPERATIONACCURACY']) == 1, elt['OPERATIONACCURACY']
            d['OPERATIONACCURACY'] = elt['OPERATIONACCURACY'][0]
        #elif 'ELLIPSOID' in elt:
        #    d['ELLIPSOID'] = [elt['ELLIPSOID'][0], wkt_array_to_dict(elt['ELLIPSOID'][1:])]
        elif 'PARAMETER' in elt:
            assert len(elt['PARAMETER']) >= 2, elt['PARAMETER']
            name = elt['PARAMETER'][0]
            assert name not in d
            d[name] = elt['PARAMETER'][1] if len(elt['PARAMETER']) == 2 else elt['PARAMETER'][1:]
        elif 'UNIT' in elt:
            assert len(elt['UNIT']) == 2, elt['UNIT']
            name = elt['UNIT'][0]
            assert 'UNIT_NAME' not in d
            d['UNIT_NAME'] = name
            d['UNIT_VALUE'] = elt['UNIT'][1]
        elif 'LENGTHUNIT' in elt:
            assert len(elt['LENGTHUNIT']) == 2, elt['LENGTHUNIT']
            name = elt['LENGTHUNIT'][0]
            assert 'UNIT_NAME' not in d
            d['UNIT_NAME'] = name
            d['UNIT_VALUE'] = elt['LENGTHUNIT'][1]
        else:
            assert True
    return d


########################

@dataclass
class CoordinateSystem:
    """
    Encapsulates a coordinate system
    """
    auth_name: str
    code: str


@dataclass
class Unit:
    """
    Encapsulates a WKT unit
    """
    uom_auth_name: str
    uom_code: str
    cs: Optional[CoordinateSystem] = None


def get_wkt_unit(UNIT_NAME, UNIT_VALUE, is_rate=False) -> Unit:
    cs = None
    if UNIT_NAME == 'Meter':
        uom_auth_name = 'EPSG'
        uom_code = '1042' if is_rate else '9001'
        cs = CoordinateSystem('EPSG', '4400')
        assert UNIT_VALUE == '1.0', UNIT_VALUE
    elif UNIT_NAME == 'Millimeter':
        uom_auth_name = 'EPSG'
        uom_code = '1027' if is_rate else '1025'
        assert UNIT_VALUE == '0.001', UNIT_VALUE
    elif UNIT_NAME == 'Chain':
        uom_auth_name = 'EPSG'
        assert not is_rate
        uom_code = '9097'
        cs = CoordinateSystem('ESRI', UNIT_NAME)
        assert UNIT_VALUE == '20.1168', UNIT_VALUE
    elif UNIT_NAME == 'Degree':
        assert not is_rate
        uom_auth_name = 'EPSG'
        uom_code = '9102'
        assert UNIT_VALUE == '0.0174532925199433', UNIT_VALUE
    elif UNIT_NAME == 'Arcsecond':
        uom_auth_name = 'EPSG'
        uom_code = '1043' if is_rate else '9104'
        assert UNIT_VALUE == '0.00000484813681109536', UNIT_VALUE
    elif UNIT_NAME == 'Milliarcsecond':
        uom_auth_name = 'EPSG'
        uom_code = '1032' if is_rate else '1031'
        assert UNIT_VALUE == '4.84813681109536e-09', UNIT_VALUE
    elif UNIT_NAME == 'Grad':
        assert not is_rate
        uom_auth_name = 'EPSG'
        uom_code = '9105'
        assert UNIT_VALUE == '0.01570796326794897', UNIT_VALUE
    elif UNIT_NAME == 'Foot':
        assert not is_rate
        uom_auth_name = 'EPSG'
        uom_code = '9002'
        cs = CoordinateSystem('EPSG', '4495')
        assert UNIT_VALUE == '0.3048', UNIT_VALUE
    elif UNIT_NAME == 'Foot_US':
        assert not is_rate
        uom_auth_name = 'EPSG'
        uom_code = '9003'
        cs = CoordinateSystem('EPSG', '4497')
        assert UNIT_VALUE == '0.3048006096012192', UNIT_VALUE
    elif UNIT_NAME == 'Yard_Indian_1937':
        assert not is_rate
        uom_auth_name = 'EPSG'
        uom_code = '9085'
        cs = CoordinateSystem('ESRI', UNIT_NAME)
        assert UNIT_VALUE == '0.91439523', UNIT_VALUE
    elif UNIT_NAME == 'Parts_Per_Million':
        uom_auth_name = 'EPSG'
        uom_code = '1041' if is_rate else '9202'
        assert UNIT_VALUE == '0.000001', UNIT_VALUE
    elif UNIT_NAME == 'Unity':
        assert not is_rate
        uom_auth_name = 'EPSG'
        uom_code = '9201'
        assert UNIT_VALUE == '1.0', UNIT_VALUE
    else:
        assert False, UNIT_NAME

    if cs is not None and cs.auth_name == 'ESRI' and cs.code not in set_esri_cs_code:
        sql = f"""INSERT INTO "coordinate_system" VALUES('ESRI','{cs.code}','Cartesian',2);"""
        all_sql.append(sql)
        sql = f"""INSERT INTO "axis" VALUES('ESRI','{2 * len(set_esri_cs_code) + 1}','Easting','E','east','ESRI','{cs.code}',1,'EPSG','{uom_code}');"""
        all_sql.append(sql)
        sql = f"""INSERT INTO "axis" VALUES('ESRI','{2 * len(set_esri_cs_code) + 2}','Northing','N','north','ESRI','{cs.code}',2,'EPSG','{uom_code}');"""
        all_sql.append(sql)
        set_esri_cs_code.add(cs.code)

    return Unit(uom_auth_name=uom_auth_name,
                uom_code=uom_code,
                cs=cs)


class UnitType(Enum):
    """
    WKT unit types
    """
    Angle = 0
    Length = 1
    Scale = 2


STRING_TO_UNIT_TYPE = {
    'ANGLEUNIT': UnitType.Angle,
    'LENGTHUNIT': UnitType.Length,
    'SCALEUNIT': UnitType.Scale
}


@dataclass
class ParameterValue:
    """
    Encapsulates a WKT parameter value
    """
    value: float
    unit_type: UnitType
    unit: Unit


def get_parameter_value(wkt_definition: List) -> ParameterValue:
    """
    Retrieves a WKT parameter value from its definition
    """
    value = wkt_definition[0]
    assert len(wkt_definition[1]) == 1
    unit_type_str = list(wkt_definition[1].keys())[0]
    unit_type = STRING_TO_UNIT_TYPE[unit_type_str]

    return ParameterValue(value=value,
                          unit_type=unit_type,
                          unit=get_wkt_unit(*wkt_definition[1][unit_type_str]))


def get_parameter_values(wkt_definition: dict) -> Dict[str, ParameterValue]:
    """
    Retrieves all WKT parameter values from a WKT string
    """
    res = {}
    for param, value in wkt_definition.items():
        if isinstance(value, str):
            res[param] = value
        else:
            try:
                res[param] = get_parameter_value(value)
            except AssertionError:
                print(wkt_definition)
                raise

    return res


########################

def get_cs_from_false_easting_and_northing(params: Dict[str, ParameterValue]) -> CoordinateSystem:
    """
    Extracts the CoordinateSystem from False_Easting and False_Northing parameters
    """
    assert params['False_Easting'].unit.cs.auth_name == params[
        'False_Northing'].unit.cs.auth_name, 'Cannot handle False_Easting CS auth {} != False_Northing CS auth {}'.format(
        params['False_Easting'].unit.cs.auth_name, params['False_Northing'].unit.cs.auth_name)
    cs_auth_name = params['False_Easting'].unit.cs.auth_name

    assert params['False_Easting'].unit.cs.code == params[
        'False_Northing'].unit.cs.code, 'Cannot handle False_Easting CS code {} != False_Northing CS code {}'.format(
        params['False_Easting'].unit.cs.code, params['False_Northing'].unit.cs.code)
    cs_code = params['False_Easting'].unit.cs.code

    return CoordinateSystem(cs_auth_name, cs_code)


map_projcs_esri_name_to_auth_code = {}
set_esri_cs_code = set()
MAP_CONVERSION_SQL_TO_CODE = {}

EPSG_CONVERSION_PARAM_NAMES = {
    1039: 'Projection plane origin height',
    8801: 'Latitude of natural origin',
    8802: 'Longitude of natural origin',
    8805: 'Scale factor at natural origin',
    8806: 'False easting',
    8807: 'False northing',
    8811: 'Latitude of projection centre',
    8812: 'Longitude of projection centre',
    8813: 'Azimuth of initial line',
    8814: 'Angle from Rectified to Skew Grid',
    8815: 'Scale factor on initial line',
    8821: 'Latitude of false origin',
    8822: 'Longitude of false origin',
    8823: 'Latitude of 1st standard parallel',
    8824: 'Latitude of 2nd standard parallel',
    8826: 'Easting at false origin',
    8827: 'Northing at false origin'
}


@dataclass
class ConversionMapping:
    """
    Encapsulates a mapping from an ESRI projcs
    """
    epsg_code: str
    epsg_name: str
    param_mapping: Dict[int, str]


# special cases

REQUIRES_ORIGINAL_WKT_DEF = {
    '102113'  # see https://github.com/OSGeo/PROJ/pull/2954#issuecomment-977771912 for rationale
}

MAPPED_PROJCS: Dict[str, ConversionMapping] = {
    'Hotine_Oblique_Mercator_Azimuth_Natural_Origin': ConversionMapping(
        epsg_code='9812',
        epsg_name='Hotine Oblique Mercator (variant A)',
        param_mapping={
            8811: 'Latitude_Of_Center',
            8812: 'Longitude_Of_Center',
            8813: 'Azimuth',
            8814: 'Azimuth',
            8815: 'Scale_Factor',
            8806: 'False_Easting',
            8807: 'False_Northing'
        }
    ),
    'Equal_Earth': ConversionMapping(
        epsg_code='1078',
        epsg_name='Equal Earth',
        param_mapping={
            8802: 'Central_Meridian',
            8806: 'False_Easting',
            8807: 'False_Northing'
        }
    ),
    'Albers': ConversionMapping(
        epsg_code='9822',
        epsg_name='Albers Equal Area',
        param_mapping={
            8821: 'Latitude_Of_Origin',
            8822: 'Central_Meridian',
            8823: 'Standard_Parallel_1',
            8824: 'Standard_Parallel_2',
            8826: 'False_Easting',
            8827: 'False_Northing',
        }
    ),
    'Cassini': ConversionMapping(
        epsg_code='9806',
        epsg_name='Cassini-Soldner',
        param_mapping={
            8801: 'Latitude_Of_Origin',
            8802: 'Central_Meridian',
            8806: 'False_Easting',
            8807: 'False_Northing',
        }
    ),
    'IGAC_Plano_Cartesiano': ConversionMapping(
        epsg_code='1052',
        epsg_name='Colombia Urban',
        param_mapping={
            8801: 'Latitude_Of_Center',
            8802: 'Longitude_Of_Center',
            8806: 'False_Easting',
            8807: 'False_Northing',
            1039: 'Height'
        }
    ),
    'Equidistant_Cylindrical': ConversionMapping(
        epsg_code='1029',
        epsg_name='Equidistant Cylindrical (Spherical)',
        param_mapping={
            8823: 'Standard_Parallel_1',
            8802: 'Central_Meridian',
            8806: 'False_Easting',
            8807: 'False_Northing',
        }
    ),
    'Mercator': ConversionMapping(
        epsg_code='9805',
        epsg_name='Mercator (variant B)',
        param_mapping={
            8823: 'Standard_Parallel_1',
            8802: 'Central_Meridian',
            8806: 'False_Easting',
            8807: 'False_Northing',
        }
    ),
    'Polyconic': ConversionMapping(
        epsg_code='9818',
        epsg_name='American Polyconic',
        param_mapping={
            8801: 'Latitude_Of_Origin',
            8802: 'Central_Meridian',
            8806: 'False_Easting',
            8807: 'False_Northing',
        }
    ),

}

MAPPED_PROJCS_WITH_EXTRA_LOGIC: Dict[str, Dict[str, ConversionMapping]] = {
    'Lambert_Conformal_Conic': {
        'Lambert Conic Conformal (1SP)': ConversionMapping(
            epsg_code='9801',
            epsg_name='Lambert Conic Conformal (1SP)',
            param_mapping={
                8801: 'Latitude_Of_Origin',
                8802: 'Central_Meridian',
                8805: 'Scale_Factor',
                8806: 'False_Easting',
                8807: 'False_Northing'
            }
        ),
        'Lambert Conic Conformal (2SP)': ConversionMapping(
            epsg_code='9802',
            epsg_name='Lambert Conic Conformal (2SP)',
            param_mapping={
                8821: 'Latitude_Of_Origin',
                8822: 'Central_Meridian',
                8823: 'Standard_Parallel_1',
                8824: 'Standard_Parallel_2',
                8826: 'False_Easting',
                8827: 'False_Northing'
            }
        )
    }

}


def insert_conversion_sql(esri_code: str, esri_name: str, epsg_code: str, epsg_name: str,
                          params: Dict[str, ParameterValue],
                          param_mapping: Dict[int, str],
                          extent_auth_name: str, extent_code: str,
                          geogcs_auth_name: str, geogcs_code: str,
                          cs: CoordinateSystem,
                          deprecated: bool = False) -> List[str]:
    """
    Generates INSERT sql command for conversion
    """
    param_strings = []
    for param_epsg_code, param_name in param_mapping.items():
        param_strings.append("'EPSG','{}','{}',{},'{}','{}'".format(
            param_epsg_code,
            EPSG_CONVERSION_PARAM_NAMES[param_epsg_code],
            params[param_name].value,
            params[param_name].unit.uom_auth_name,
            params[param_name].unit.uom_code
        ))

    if len(param_strings) < 7:
        for _ in range(len(param_strings), 7):
            param_strings.append('NULL,NULL,NULL,NULL,NULL,NULL')

    assert len(param_strings) == 7, 'Too many parameters'

    sql = "INSERT INTO \"conversion\" VALUES('ESRI','{code}','{name}',NULL,'{method_auth_name}','{method_code}','{method_name}'," \
          "{param1},{param2},{param3},{param4},{param5},{param6},{param7},{deprecated});".format(
        code=esri_code,
        name='unnamed',
        method_auth_name='EPSG',
        method_code=epsg_code,
        method_name=epsg_name,
        param1=param_strings[0],
        param2=param_strings[1],
        param3=param_strings[2],
        param4=param_strings[3],
        param5=param_strings[4],
        param6=param_strings[5],
        param7=param_strings[6],
        deprecated=1 if deprecated else 0)

    conv_name = 'unnamed'
    conv_auth_name = 'ESRI'
    conv_code = esri_code

    results = []
    sql_extract = sql[sql.find('NULL'):]
    if conv_name != 'unnamed' or sql_extract not in MAP_CONVERSION_SQL_TO_CODE:
        results.append(sql)

        sql = f"""INSERT INTO "usage" VALUES('ESRI', 'CONV_{esri_code}_USAGE','conversion','ESRI','{esri_code}','{extent_auth_name}','{extent_code}','EPSG','1024');"""
        results.append(sql)

        MAP_CONVERSION_SQL_TO_CODE[sql_extract] = conv_code
    else:
        conv_code = MAP_CONVERSION_SQL_TO_CODE[sql_extract]

    sql = f"""INSERT INTO "projected_crs" VALUES('ESRI','{esri_code}','{esri_name}',NULL,'{cs.auth_name}','{cs.code}','{geogcs_auth_name}',""" \
          f"""'{geogcs_code}','{conv_auth_name}','{conv_code}',NULL,{1 if deprecated else 0});"""
    results.append(sql)
    sql = f"""INSERT INTO "usage" VALUES('ESRI', 'PCRS_{esri_code}_USAGE','projected_crs','ESRI','{esri_code}','{extent_auth_name}','{extent_code}','EPSG','1024');"""
    results.append(sql)

    return results


def import_projcs():
    with open(path_to_csv / 'pe_list_projcs.csv', 'rt') as csvfile:
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

        idx_wkt2 = header.index('wkt2')
        assert idx_wkt2 >= 0

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

            if code == latestWkid and authority.upper() == 'ESRI':
                cursor.execute(
                    "SELECT name FROM projected_crs WHERE auth_name = 'EPSG' AND code = ?", (latestWkid,))
                src_row = cursor.fetchone()
                if src_row:
                    src_name = src_row[0]
                    modified_epsg_name = src_name.replace(' / ', '_').replace(' ', '_')
                    if modified_epsg_name.upper() == esri_name.upper() or modified_epsg_name.upper() + "_3D" == esri_name.upper() or modified_epsg_name.upper() + "_(3D)" == esri_name.upper():
                        print("ProjCRS ESRI:%s (%s) has the same name as EPSG:%s. Fixing authority to be EPSG" % (latestWkid, esri_name, latestWkid))
                        authority = "EPSG"

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

                wkt2 = row[idx_wkt2]
                wkt2_basegeogcrs_re = re.compile(r'.*BASEGEOGCRS\[""?(.*?)""?.*')
                match = wkt2_basegeogcrs_re.match(wkt2)
                assert match, wkt2
                geogcs_name = match.group(1)

                pos = wkt2.find('CONVERSION[')
                assert pos >= 0
                parsed_conv_wkt2 = parse_wkt_array(wkt2[pos:-1])

                assert geogcs_name in map_geogcs_esri_name_to_auth_code, (
                    geogcs_name, row)
                geogcs_auth_name, geogcs_code = map_geogcs_esri_name_to_auth_code[geogcs_name]

                extent_auth_name, extent_code = find_extent(
                    row[idx_areaname], row[idx_slat], row[idx_nlat], row[idx_llon], row[idx_rlon])

                map_projcs_esri_name_to_auth_code[esri_name] = ['ESRI', code]

                deprecated = 1 if row[idx_deprecated] == 'yes' else 0

                method = parsed_conv_wkt2['CONVERSION'][0]

                if method in ('Transverse_Mercator', 'Gauss_Kruger'):
                    params = get_parameter_values(parsed_conv_wkt2['CONVERSION'][1])
                    cs = get_cs_from_false_easting_and_northing(params)

                    assert params['Central_Meridian'].unit.uom_auth_name == 'EPSG', 'Unhandled Central_Meridian authority {}'.format(params['Central_Meridian'].unit.uom_auth_name)
                    assert params['Scale_Factor'].unit.uom_code == '9201', 'Unhandled scale factor unit {}'.format(params['Scale_Factor'].unit.uom_code)
                    assert params['Latitude_Of_Origin'].unit.uom_auth_name == 'EPSG', 'Unhandled Latitude_Of_Origin authority {}'.format(params['Latitude_Of_Origin'].unit.uom_auth_name)

                    conv_name = 'unnamed'
                    if method == 'Gauss_Kruger' and 'GK_' not in esri_name and 'Gauss' not in esri_name:
                        conv_name = esri_name + " (Gauss Kruger)"

                    cursor.execute(
                        """SELECT code, deprecated FROM conversion WHERE auth_name = 'EPSG' AND
                        method_code = '9807' AND
                        param1_code = '8801' AND param1_value = ? AND param1_uom_code = ? AND
                        param2_code = '8802' AND param2_value = ? AND param2_uom_code = ? AND
                        param3_code = '8805' AND param3_value = ? AND param3_uom_code = '9201' AND
                        param4_code = '8806' AND param4_value = ? AND param4_uom_code = ? AND
                        param5_code = '8807' AND param5_value = ? AND param5_uom_code = ?""", (params['Latitude_Of_Origin'].value,
                                                                                               params['Latitude_Of_Origin'].unit.uom_code,
                                                                                               params['Central_Meridian'].value,
                                                                                               params['Central_Meridian'].unit.uom_code,
                                                                                               params['Scale_Factor'].value,
                                                                                               params['False_Easting'].value,
                                                                                               params['False_Easting'].unit.uom_code,
                                                                                               params['False_Northing'].value,
                                                                                               params['False_Northing'].unit.uom_code))
                    src_row = cursor.fetchone()
                    if conv_name == 'unnamed' and src_row:
                        conv_auth_name = 'EPSG'
                        conv_code = src_row[0]
                        conv_is_deprecated = bool(src_row[1])
                        while not deprecated and conv_is_deprecated:
                            # if we found a deprecated conversion but the CRS isn't deprecated, keep looking...
                            src_row = cursor.fetchone()
                            if not src_row:
                                break

                            conv_code = src_row[0]
                            conv_is_deprecated = bool(src_row[1])

                        if conv_is_deprecated and not deprecated:
                            # if conversion is marked as deprecated, we have to deprecate the CRS also
                            assert False, 'ESRI:{} ({}) is NOT marked as deprecated but conversion EPSG:{} is deprecated'.format(code, esri_name, conv_code)
                    else:
                        conv_auth_name = 'ESRI'
                        conv_code = code

                        sql = """INSERT INTO "conversion" VALUES('ESRI','%s','%s',NULL,'EPSG','9807','Transverse Mercator','EPSG','8801','Latitude of natural origin',%s,'EPSG','%s','EPSG','8802','Longitude of natural origin',%s,'EPSG','%s','EPSG','8805','Scale factor at natural origin',%s,'EPSG','9201','EPSG','8806','False easting',%s,'EPSG','%s','EPSG','8807','False northing',%s,'EPSG','%s',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,%d);""" % (
                            code, conv_name,
                            params['Latitude_Of_Origin'].value, params['Latitude_Of_Origin'].unit.uom_code,
                            params['Central_Meridian'].value, params['Central_Meridian'].unit.uom_code,
                            params['Scale_Factor'].value,
                            params['False_Easting'].value, params['False_Easting'].unit.uom_code,
                            params['False_Northing'].value, params['False_Northing'].unit.uom_code,
                            deprecated)

                        sql_extract = sql[sql.find('NULL'):]
                        if conv_name != 'unnamed' or sql_extract not in MAP_CONVERSION_SQL_TO_CODE:
                            all_sql.append(sql)

                            sql = """INSERT INTO "usage" VALUES('ESRI', 'CONV_%s_USAGE','conversion','ESRI','%s','%s','%s','%s','%s');""" % (code, code, extent_auth_name, extent_code, 'EPSG', '1024')
                            all_sql.append(sql)

                            MAP_CONVERSION_SQL_TO_CODE[sql_extract] = conv_code
                        else:
                            conv_code = MAP_CONVERSION_SQL_TO_CODE[sql_extract]

                    sql = """INSERT INTO "projected_crs" VALUES('ESRI','%s','%s',NULL,'%s','%s','%s','%s','%s','%s',NULL,%d);""" % (
                        code, esri_name, cs.auth_name, cs.code, geogcs_auth_name, geogcs_code, conv_auth_name, conv_code, deprecated)
                    all_sql.append(sql)
                    sql = """INSERT INTO "usage" VALUES('ESRI', 'PCRS_%s_USAGE','projected_crs','ESRI','%s','%s','%s','%s','%s');""" % (code, code, extent_auth_name, extent_code, 'EPSG', '1024')
                    all_sql.append(sql)

                elif (method in MAPPED_PROJCS or method in MAPPED_PROJCS_WITH_EXTRA_LOGIC) and code not in REQUIRES_ORIGINAL_WKT_DEF:
                    params = get_parameter_values(parsed_conv_wkt2['CONVERSION'][1])
                    cs = get_cs_from_false_easting_and_northing(params)
                    if method in MAPPED_PROJCS:
                        conversion_mapping = MAPPED_PROJCS[method]
                    elif method == 'Lambert_Conformal_Conic' and 'Scale_Factor' in params:
                        conversion_mapping = MAPPED_PROJCS_WITH_EXTRA_LOGIC['Lambert_Conformal_Conic'][
                            'Lambert Conic Conformal (1SP)']
                        assert params['Scale_Factor'].unit.uom_code == '9201', 'Unhandled scale unit {}'.format(
                            params['Scale_Factor'].unit.uom_code)
                        assert params['Standard_Parallel_1'].value == params['Latitude_Of_Origin'].value
                        assert params['Standard_Parallel_1'].unit == params['Latitude_Of_Origin'].unit

                    elif method == 'Lambert_Conformal_Conic' and 'Standard_Parallel_2' in params:
                        conversion_mapping = MAPPED_PROJCS_WITH_EXTRA_LOGIC['Lambert_Conformal_Conic']['Lambert Conic Conformal (2SP)']

                    else:
                        assert False

                    # additional validation required for these methods:
                    if method == 'Hotine_Oblique_Mercator_Azimuth_Natural_Origin':
                        assert params['Scale_Factor'].unit.uom_code == '9201', 'Unhandled scale unit {}'.format(params['Scale_Factor'].unit.uom_code)
                    elif method == 'Cassini':
                        assert params['Scale_Factor'].unit.uom_code == '9201', 'Unhandled scale unit {}'.format(
                            params['Scale_Factor'].unit.uom_code)
                        assert params['Scale_Factor'].value == '1.0'

                    sql = insert_conversion_sql(esri_code=code, esri_name=esri_name,
                                                epsg_code=conversion_mapping.epsg_code,
                                                epsg_name=conversion_mapping.epsg_name,
                                                params=params,
                                                param_mapping=conversion_mapping.param_mapping,
                                                extent_auth_name=extent_auth_name,
                                                extent_code=extent_code,
                                                geogcs_auth_name=geogcs_auth_name,
                                                geogcs_code=geogcs_code,
                                                cs=cs,
                                                deprecated=bool(deprecated)
                                                )
                    all_sql.extend(sql)

                else:
                    # TODO -- add more method mapping!
                    print('Direct mapping for {} not yet implemented, falling back to default handling'.format(method))

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
    with open(path_to_csv / 'pe_list_vdatum.csv', 'rt') as csvfile:
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

        idx_wkt2 = header.index('wkt2')
        assert idx_wkt2 >= 0

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
    with open(path_to_csv / 'pe_list_vertcs.csv', 'rt') as csvfile:
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

        idx_wkt2 = header.index('wkt2')
        assert idx_wkt2 >= 0

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

            if code == latestWkid and authority.upper() == 'ESRI':
                cursor.execute(
                    "SELECT name FROM vertical_crs WHERE auth_name = 'EPSG' AND code = ?", (latestWkid,))
                src_row = cursor.fetchone()
                if src_row:
                    src_name = src_row[0]
                    modified_epsg_name = src_name.replace(' ', '_')
                    if modified_epsg_name.upper() == esri_name.upper():
                        print("VertCRS ESRI:%s (%s) has the same name as EPSG:%s. Fixing authority to be EPSG" % (latestWkid, esri_name, latestWkid))
                        authority = "EPSG"

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

                wkt2 = row[idx_wkt2]

                vdatum_re = re.compile(r'.*VDATUM\[""?(.*?)""?.*')
                match = vdatum_re.match(wkt2)

                is_vdatum = True
                if match:
                    datum_name = match.group(1)
                    if datum_name not in map_vdatum_esri_name_to_auth_code:
                        print('Skipping vertcs %s. Cannot find vertical datum %s' % (
                            str(row), datum_name))
                        sql = """-- Skipping vertcs %s. Cannot find vertical datum %s""" % (
                            esri_name, datum_name)
                        all_sql.append(sql)
                        continue
                    datum_auth_name, datum_code = map_vdatum_esri_name_to_auth_code[datum_name]
                else:
                    datum_re = re.compile(r'.*DATUM\[""?(.*?)""?.*')
                    match = datum_re.match(wkt2)
                    assert match

                    is_vdatum = False
                    datum_name = match.group(1)

                    if datum_name not in map_datum_esri_name_to_auth_code:
                        print('Skipping vertcs %s. Cannot find geodetic datum %s' % (
                            str(row), datum_name))
                        sql = """-- Skipping vertcs %s. Cannot find geodetic datum %s""" % (
                            esri_name, datum_name)
                        all_sql.append(sql)
                        continue
                    datum_auth_name, datum_code = map_datum_esri_name_to_auth_code[datum_name]

                deprecated = 1 if row[idx_deprecated] == 'yes' else 0

                extent_auth_name, extent_code = find_extent(
                    row[idx_areaname], row[idx_slat], row[idx_nlat], row[idx_llon], row[idx_rlon])

                if not is_vdatum:
                    new_datum_code = 'from_geogdatum_' + datum_auth_name + '_' + datum_code
                    if new_datum_code not in vdatum_written:
                        vdatum_written.add(new_datum_code)

                        p = map_datum_esri_to_parameters[datum_code]

                        datum_code = new_datum_code

                        sql = """INSERT INTO "vertical_datum" VALUES('ESRI','%s','%s',NULL,NULL,NULL,NULL,NULL,%d);""" % (
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
                    sql = """INSERT INTO "vertical_datum" VALUES('ESRI','%s','%s',NULL,NULL,NULL,NULL,NULL,%d);""" % (
                        datum_code, p['esri_name'], p['deprecated'])
                    all_sql.append(sql)
                    sql = """INSERT INTO "usage" VALUES('ESRI', '%s_USAGE','vertical_datum','ESRI','%s','%s','%s','%s','%s');""" % (datum_code, datum_code, extent_auth_name, extent_code, 'EPSG', '1024')
                    all_sql.append(sql)

                #map_vertcs_esri_name_to_auth_code[esri_name] = ['ESRI', code]

                parsed_wkt2 = parse_wkt_array(wkt2)

                assert not set(k for k in parsed_wkt2['VERTCRS'][1].keys() if k not in ('CS','AXIS','VDATUM','DATUM')), 'Unhandled parameter in VERTCRS: {}'.format(list(parsed_wkt2['VERTCRS'][1].keys()))
                assert parsed_wkt2['VERTCRS'][1]['CS'] == ['vertical', '1'], 'Unhandled vertcrs CS: {}'.format(parsed_wkt2['VERTCRS'][1]['CS'])
                axis = parsed_wkt2['VERTCRS'][1]['AXIS']
                is_ellipsoidal_height = (axis[:2] == ['Ellipsoidal height (h)', 'up'])
                assert axis[:2] == ['Gravity-related height (H)', 'up'] or is_ellipsoidal_height, 'Unhandled vertcrs AXIS: {}'.format(axis)

                vertical_unit = axis[2]['UNIT_NAME']
                cs_auth = 'EPSG'
                if vertical_unit == 'Meter':
                    cs_code = 6499
                elif vertical_unit == 'Foot':
                    cs_code = 1030
                elif vertical_unit == 'Foot_US':
                    cs_code = 6497
                else:
                    assert False, ('unknown coordinate system for %s' %
                                   str(row))
                if is_ellipsoidal_height or not is_vdatum:
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
    with open(path_to_csv / 'pe_list_hvcoordsys.csv', 'rt') as csvfile:
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

        idx_wkt2 = header.index('wkt2')
        assert idx_wkt2 >= 0

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

            if authority == 'Esri' and code in ('9897',):
                # .csv file pretents this is a Esri code, but it is a EPSG one
                authority = 'EPSG'

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
    with open(path_to_csv / 'pe_list_geogtran.csv', 'rt') as csvfile:
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

        idx_wkt2 = header.index('wkt2')
        assert idx_wkt2 >= 0

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
            wkt2 = row[idx_wkt2]
            deprecated = 1 if row[idx_deprecated] == 'yes' else 0

            if authority == 'EPSG':

                map_compoundcrs_esri_name_to_auth_code[esri_name] = [
                    'EPSG', wkid]

                cursor.execute(
                    "SELECT name, table_name FROM coordinate_operation_view WHERE auth_name = 'EPSG' AND code = ?", (wkid,))
                src_row = cursor.fetchone()

                if not src_row:
                    if 'Molodensky_Badekas' in wkt2:
                        # print('Skipping GEOGTRAN %s (EPSG source) since it uses a non-supported yet suported method'% esri_name)
                        assert False  # no longer present in db
                    if 'NADCON5' in wkt2:
                        print('Skipping NADCON5 %s (EPSG source) since it uses a non-supported yet suported method' % esri_name)
                        continue

                assert src_row, row

                _, table_name = src_row
                # Insert alias
                sql = f"INSERT INTO \"alias_name\" VALUES('{table_name}','EPSG',{wkid},'{esri_name}','ESRI');"
                all_sql.append(sql)

            else:

                # We don't want to import ESRI deprecated transformations
                # (there are a lot), do we ?
                if deprecated:
                    # print('Skipping deprecated GEOGTRAN %s' % esri_name)
                    continue

                parsed_wkt2 = parse_wkt_array(wkt2)
                assert 'COORDINATEOPERATION' in parsed_wkt2

                src_crs_name = parsed_wkt2['COORDINATEOPERATION'][1]['SOURCECRS']['GEOGCRS'][0]
                dst_crs_name = parsed_wkt2['COORDINATEOPERATION'][1]['TARGETCRS']['GEOGCRS'][0]

                assert src_crs_name in map_geogcs_esri_name_to_auth_code, (
                    src_crs_name, row)
                src_crs_auth_name, src_crs_code = map_geogcs_esri_name_to_auth_code[src_crs_name]

                assert dst_crs_name in map_geogcs_esri_name_to_auth_code, (
                    dst_crs_name, row)
                dst_crs_auth_name, dst_crs_code = map_geogcs_esri_name_to_auth_code[dst_crs_name]

                method = parsed_wkt2['COORDINATEOPERATION'][1]['METHOD']
                is_longitude_rotation = method == "Longitude_Rotation"
                if is_longitude_rotation:
                    # skip it as it is automatically handled by PROJ
                    continue

                is_cf = method == "Coordinate_Frame"
                is_pv = method == "Position_Vector"
                is_geocentric_translation = method == "Geocentric_Translation"
                is_geog2d_offset = method == "Geographic_2D_Offset"
                is_null = method == "Null"
                is_unitchange = method == "Unit_Change"
                is_nadcon = method == "NADCON"
                is_ntv2 = method == "NTv2"
                is_geocon = method == "GEOCON"
                is_harn = method == "HARN"
                is_molodensky_badekas = method == "Molodensky_Badekas"
                is_Time_Based_Helmert_Position_Vector = method == "Time_Based_Helmert_Position_Vector"
                is_Time_Based_Helmert_Coordinate_Frame = method == "Time_Based_Helmert_Coordinate_Frame"
                assert is_cf or is_pv or is_geocentric_translation or is_molodensky_badekas or is_nadcon or is_geog2d_offset or is_ntv2 or is_geocon or is_null or is_harn or is_unitchange or is_Time_Based_Helmert_Position_Vector or is_Time_Based_Helmert_Coordinate_Frame, row

                extent_auth_name, extent_code = find_extent(
                    row[idx_areaname], row[idx_slat], row[idx_nlat], row[idx_llon], row[idx_rlon])

                accuracy = row[idx_accuracy]
                if float(accuracy) == 999:
                    accuracy = 'NULL'

                if is_cf or is_pv:
                    x = parsed_wkt2['COORDINATEOPERATION'][1]['X_Axis_Translation'][0]
                    x_axis_translation_unit = get_wkt_unit(*parsed_wkt2['COORDINATEOPERATION'][1]['X_Axis_Translation'][1]['LENGTHUNIT'])
                    y = parsed_wkt2['COORDINATEOPERATION'][1]['Y_Axis_Translation'][0]
                    y_axis_translation_unit = get_wkt_unit(*parsed_wkt2['COORDINATEOPERATION'][1]['Y_Axis_Translation'][1]['LENGTHUNIT'])
                    z = parsed_wkt2['COORDINATEOPERATION'][1]['Z_Axis_Translation'][0]
                    z_axis_translation_unit = get_wkt_unit(*parsed_wkt2['COORDINATEOPERATION'][1]['Z_Axis_Translation'][1]['LENGTHUNIT'])
                    assert x_axis_translation_unit.uom_auth_name == y_axis_translation_unit.uom_auth_name == z_axis_translation_unit.uom_auth_name, 'Cannot handle different translation axis authorities'
                    assert x_axis_translation_unit.uom_code == y_axis_translation_unit.uom_code == z_axis_translation_unit.uom_code, 'Cannot handle different translation axis unit codes'

                    rx = parsed_wkt2['COORDINATEOPERATION'][1]['X_Axis_Rotation'][0]
                    x_axis_rotation_unit = get_wkt_unit(
                                *parsed_wkt2['COORDINATEOPERATION'][1]['X_Axis_Rotation'][1]['ANGLEUNIT'])
                    ry = parsed_wkt2['COORDINATEOPERATION'][1]['Y_Axis_Rotation'][0]
                    y_axis_rotation_unit = get_wkt_unit(
                                *parsed_wkt2['COORDINATEOPERATION'][1]['Y_Axis_Rotation'][1]['ANGLEUNIT'])
                    rz = parsed_wkt2['COORDINATEOPERATION'][1]['Z_Axis_Rotation'][0]
                    z_axis_rotation_unit = get_wkt_unit(
                                *parsed_wkt2['COORDINATEOPERATION'][1]['Z_Axis_Rotation'][1]['ANGLEUNIT'])
                    assert x_axis_rotation_unit.uom_auth_name == y_axis_rotation_unit.uom_auth_name == z_axis_rotation_unit.uom_auth_name, 'Cannot handle different rotation axis authorities'
                    assert x_axis_rotation_unit.uom_code == y_axis_rotation_unit.uom_code == z_axis_rotation_unit.uom_code, 'Cannot handle different rotation axis unit codes'

                    s = parsed_wkt2['COORDINATEOPERATION'][1]['Scale_Difference'][0]
                    scale_difference_unit = get_wkt_unit(
                                *parsed_wkt2['COORDINATEOPERATION'][1]['Scale_Difference'][1]['SCALEUNIT'])

                    if is_cf:
                        method_code = '9607'
                        method_name = 'Coordinate Frame rotation (geog2D domain)'
                    else:
                        method_code = '9606'
                        method_name = 'Position Vector transformation (geog2D domain)'

                    sql = "INSERT INTO \"helmert_transformation\" VALUES('ESRI','{code}','{name}',NULL,'EPSG','{method_code}','{method_name}','{source_crs_auth_name}'," \
                            "'{source_crs_code}','{target_crs_auth_name}','{target_crs_code}',{accuracy},{tx},{ty},{tz},'{translation_uom_auth_name}','{translation_uom_code}'," \
                            "{rx},{ry},{rz},'{rotation_uom_auth_name}','{rotation_uom_code}',{scale_difference},'{scale_difference_uom_auth_name}','{scale_difference_uom_code}'," \
                            "NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,{deprecated});".format(
                        code=wkid,
                        name=esri_name,
                        method_code=method_code,
                        method_name=method_name,
                        source_crs_auth_name=src_crs_auth_name,
                        source_crs_code=src_crs_code,
                        target_crs_auth_name=dst_crs_auth_name,
                        target_crs_code=dst_crs_code,
                        accuracy=accuracy,
                        tx=x,
                        ty=y,
                        tz=z,
                        translation_uom_auth_name=x_axis_translation_unit.uom_auth_name,
                        translation_uom_code=x_axis_translation_unit.uom_code,
                        rx=rx,
                        ry=ry,
                        rz=rz,
                        rotation_uom_auth_name=x_axis_rotation_unit.uom_auth_name,
                        rotation_uom_code=x_axis_rotation_unit.uom_code,
                        scale_difference=s,
                        scale_difference_uom_auth_name=scale_difference_unit.uom_auth_name,
                        scale_difference_uom_code=scale_difference_unit.uom_code,
                        deprecated=deprecated)
                    all_sql.append(sql)
                    sql = """INSERT INTO "usage" VALUES('ESRI', '%s_USAGE','helmert_transformation','ESRI','%s','%s','%s','%s','%s');""" % (wkid, wkid, extent_auth_name, extent_code, 'EPSG', '1024')
                    all_sql.append(sql)

                elif is_molodensky_badekas:
                    x = parsed_wkt2['COORDINATEOPERATION'][1]['X_Axis_Translation'][0]
                    x_axis_translation_unit = get_wkt_unit(
                        *parsed_wkt2['COORDINATEOPERATION'][1]['X_Axis_Translation'][1]['LENGTHUNIT'])
                    y = parsed_wkt2['COORDINATEOPERATION'][1]['Y_Axis_Translation'][0]
                    y_axis_translation_unit = get_wkt_unit(
                        *parsed_wkt2['COORDINATEOPERATION'][1]['Y_Axis_Translation'][1]['LENGTHUNIT'])
                    z = parsed_wkt2['COORDINATEOPERATION'][1]['Z_Axis_Translation'][0]
                    z_axis_translation_unit = get_wkt_unit(
                        *parsed_wkt2['COORDINATEOPERATION'][1]['Z_Axis_Translation'][1]['LENGTHUNIT'])
                    assert x_axis_translation_unit.uom_auth_name == y_axis_translation_unit.uom_auth_name == z_axis_translation_unit.uom_auth_name, 'Cannot handle different translation axis authorities'
                    assert x_axis_translation_unit.uom_code == y_axis_translation_unit.uom_code == z_axis_translation_unit.uom_code, 'Cannot handle different translation axis unit codes'

                    rx = parsed_wkt2['COORDINATEOPERATION'][1]['X_Axis_Rotation'][0]
                    x_axis_rotation_unit = get_wkt_unit(
                        *parsed_wkt2['COORDINATEOPERATION'][1]['X_Axis_Rotation'][1]['ANGLEUNIT'])
                    ry = parsed_wkt2['COORDINATEOPERATION'][1]['Y_Axis_Rotation'][0]
                    y_axis_rotation_unit = get_wkt_unit(
                        *parsed_wkt2['COORDINATEOPERATION'][1]['Y_Axis_Rotation'][1]['ANGLEUNIT'])
                    rz = parsed_wkt2['COORDINATEOPERATION'][1]['Z_Axis_Rotation'][0]
                    z_axis_rotation_unit = get_wkt_unit(
                        *parsed_wkt2['COORDINATEOPERATION'][1]['Z_Axis_Rotation'][1]['ANGLEUNIT'])
                    assert x_axis_rotation_unit.uom_auth_name == y_axis_rotation_unit.uom_auth_name == z_axis_rotation_unit.uom_auth_name, 'Cannot handle different rotation axis authorities'
                    assert x_axis_rotation_unit.uom_code == y_axis_rotation_unit.uom_code == z_axis_rotation_unit.uom_code, 'Cannot handle different rotation axis unit codes'

                    s = parsed_wkt2['COORDINATEOPERATION'][1]['Scale_Difference'][0]
                    scale_difference_unit = get_wkt_unit(
                        *parsed_wkt2['COORDINATEOPERATION'][1]['Scale_Difference'][1]['SCALEUNIT'])

                    px = parsed_wkt2['COORDINATEOPERATION'][1]['X_Coordinate_of_Rotation_Origin'][0]
                    x_coordinate_of_rotation_origin_unit = get_wkt_unit(
                        *parsed_wkt2['COORDINATEOPERATION'][1]['X_Coordinate_of_Rotation_Origin'][1]['LENGTHUNIT'])
                    py = parsed_wkt2['COORDINATEOPERATION'][1]['Y_Coordinate_of_Rotation_Origin'][0]
                    y_coordinate_of_rotation_origin_unit = get_wkt_unit(
                        *parsed_wkt2['COORDINATEOPERATION'][1]['Y_Coordinate_of_Rotation_Origin'][1]['LENGTHUNIT'])
                    pz = parsed_wkt2['COORDINATEOPERATION'][1]['Z_Coordinate_of_Rotation_Origin'][0]
                    z_coordinate_of_rotation_origin_unit = get_wkt_unit(
                        *parsed_wkt2['COORDINATEOPERATION'][1]['Z_Coordinate_of_Rotation_Origin'][1]['LENGTHUNIT'])
                    assert x_coordinate_of_rotation_origin_unit.uom_auth_name == y_coordinate_of_rotation_origin_unit.uom_auth_name == z_coordinate_of_rotation_origin_unit.uom_auth_name, 'Cannot handle different coordinate of rotation axis authorities'
                    assert x_coordinate_of_rotation_origin_unit.uom_code == y_coordinate_of_rotation_origin_unit.uom_code == z_coordinate_of_rotation_origin_unit.uom_code, 'Cannot handle different coordinate of rotation axis unit codes'

                    # The ESRI naming is not really clear about the convention
                    # but it looks like it is Coordinate Frame when comparing ESRI:1066 (Amersfoort_To_ETRS_1989_MB) with EPSG:1066
                    method_code = '9636'
                    method_name = 'Molodensky-Badekas (CF geog2D domain)'

                    sql = "INSERT INTO \"helmert_transformation\" VALUES('ESRI','{code}','{name}',NULL,'EPSG','{method_code}','{method_name}'," \
                          "'{source_crs_auth_name}','{source_crs_code}','{target_crs_auth_name}','{target_crs_code}',{accuracy},{tx},{ty},{tz}," \
                          "'{translation_uom_auth_name}','{translation_uom_code}',{rx},{ry},{rz},'{rotation_uom_auth_name}','{rotation_uom_code}'," \
                          "{scale_difference},'{scale_difference_uom_auth_name}','{scale_difference_uom_code}',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL," \
                            "NULL,NULL,NULL,NULL,{px},{py},{pz},'{pivot_uom_auth_name}','{pivot_uom_code}',NULL,{deprecated});".format(
                        code=wkid,
                        name=esri_name,
                        method_code=method_code,
                        method_name=method_name,
                        source_crs_auth_name=src_crs_auth_name,
                        source_crs_code=src_crs_code,
                        target_crs_auth_name=dst_crs_auth_name,
                        target_crs_code=dst_crs_code,
                        accuracy=accuracy,
                        tx=x,
                        ty=y,
                        tz=z,
                        translation_uom_auth_name=x_axis_translation_unit.uom_auth_name,
                        translation_uom_code=x_axis_translation_unit.uom_code,
                        rx=rx,
                        ry=ry,
                        rz=rz,
                        rotation_uom_auth_name=x_axis_rotation_unit.uom_auth_name,
                        rotation_uom_code=x_axis_rotation_unit.uom_code,
                        scale_difference=s,
                        scale_difference_uom_auth_name=scale_difference_unit.uom_auth_name,
                        scale_difference_uom_code=scale_difference_unit.uom_code,
                        px=px,
                        py=py,
                        pz=pz,
                        pivot_uom_auth_name=x_coordinate_of_rotation_origin_unit.uom_auth_name,
                        pivot_uom_code=x_coordinate_of_rotation_origin_unit.uom_code,
                        deprecated=deprecated)
                    all_sql.append(sql)
                    sql = """INSERT INTO "usage" VALUES('ESRI', '%s_USAGE','helmert_transformation','ESRI','%s','%s','%s','%s','%s');""" % (wkid, wkid, extent_auth_name, extent_code, 'EPSG', '1024')
                    all_sql.append(sql)

                elif is_geocentric_translation:
                    x = parsed_wkt2['COORDINATEOPERATION'][1]['X_Axis_Translation'][0]
                    x_axis_translation_unit = get_wkt_unit(
                        *parsed_wkt2['COORDINATEOPERATION'][1]['X_Axis_Translation'][1]['LENGTHUNIT'])
                    y = parsed_wkt2['COORDINATEOPERATION'][1]['Y_Axis_Translation'][0]
                    y_axis_translation_unit = get_wkt_unit(
                        *parsed_wkt2['COORDINATEOPERATION'][1]['Y_Axis_Translation'][1]['LENGTHUNIT'])
                    z = parsed_wkt2['COORDINATEOPERATION'][1]['Z_Axis_Translation'][0]
                    z_axis_translation_unit = get_wkt_unit(
                        *parsed_wkt2['COORDINATEOPERATION'][1]['Z_Axis_Translation'][1]['LENGTHUNIT'])
                    assert x_axis_translation_unit.cs.auth_name == y_axis_translation_unit.cs.auth_name == z_axis_translation_unit.cs.auth_name, 'Cannot handle different translation axis authorities'
                    assert x_axis_translation_unit.uom_code == y_axis_translation_unit.uom_code == z_axis_translation_unit.uom_code, 'Cannot handle different translation axis unit codes'

                    method_code = '9603'
                    method_name = 'Geocentric translations (geog2D domain)'

                    sql = "INSERT INTO \"helmert_transformation\" VALUES('ESRI','{code}','{name}',NULL,'EPSG','{method_code}','{method_name}','{source_crs_auth_name}'," \
                          "'{source_crs_code}','{target_crs_auth_name}','{target_crs_code}',{accuracy},{tx},{ty},{tz},'{translation_uom_auth_name}','{translation_uom_code}',"\
                            "NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL," \
                            "NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,{deprecated});".format(
                        code=wkid,
                        name=esri_name,
                        method_code=method_code,
                        method_name=method_name,
                        source_crs_auth_name=src_crs_auth_name,
                        source_crs_code=src_crs_code,
                        target_crs_auth_name=dst_crs_auth_name,
                        target_crs_code=dst_crs_code,
                        accuracy=accuracy,
                        tx=x,
                        ty=y,
                        tz=z,
                        translation_uom_auth_name=x_axis_translation_unit.uom_auth_name,
                        translation_uom_code=x_axis_translation_unit.uom_code,
                        deprecated=deprecated)
                    all_sql.append(sql)
                    sql = """INSERT INTO "usage" VALUES('ESRI', '%s_USAGE','helmert_transformation','ESRI','%s','%s','%s','%s','%s');""" % (wkid, wkid, extent_auth_name, extent_code, 'EPSG', '1024')
                    all_sql.append(sql)

                elif is_Time_Based_Helmert_Position_Vector or is_Time_Based_Helmert_Coordinate_Frame:
                    x = parsed_wkt2['COORDINATEOPERATION'][1]['X_Axis_Translation'][0]
                    x_axis_translation_unit = get_wkt_unit(
                        *parsed_wkt2['COORDINATEOPERATION'][1]['X_Axis_Translation'][1]['LENGTHUNIT'])
                    y = parsed_wkt2['COORDINATEOPERATION'][1]['Y_Axis_Translation'][0]
                    y_axis_translation_unit = get_wkt_unit(
                        *parsed_wkt2['COORDINATEOPERATION'][1]['Y_Axis_Translation'][1]['LENGTHUNIT'])
                    z = parsed_wkt2['COORDINATEOPERATION'][1]['Z_Axis_Translation'][0]
                    z_axis_translation_unit = get_wkt_unit(
                        *parsed_wkt2['COORDINATEOPERATION'][1]['Z_Axis_Translation'][1]['LENGTHUNIT'])
                    assert x_axis_translation_unit.uom_auth_name == y_axis_translation_unit.uom_auth_name == z_axis_translation_unit.uom_auth_name, 'Cannot handle different translation axis authorities'
                    assert x_axis_translation_unit.uom_code == y_axis_translation_unit.uom_code == z_axis_translation_unit.uom_code, 'Cannot handle different translation axis unit codes'

                    rx = parsed_wkt2['COORDINATEOPERATION'][1]['X_Axis_Rotation'][0]
                    x_axis_rotation_unit = get_wkt_unit(
                        *parsed_wkt2['COORDINATEOPERATION'][1]['X_Axis_Rotation'][1]['ANGLEUNIT'])
                    ry = parsed_wkt2['COORDINATEOPERATION'][1]['Y_Axis_Rotation'][0]
                    y_axis_rotation_unit = get_wkt_unit(
                        *parsed_wkt2['COORDINATEOPERATION'][1]['Y_Axis_Rotation'][1]['ANGLEUNIT'])
                    rz = parsed_wkt2['COORDINATEOPERATION'][1]['Z_Axis_Rotation'][0]
                    z_axis_rotation_unit = get_wkt_unit(
                        *parsed_wkt2['COORDINATEOPERATION'][1]['Z_Axis_Rotation'][1]['ANGLEUNIT'])
                    assert x_axis_rotation_unit.uom_auth_name == y_axis_rotation_unit.uom_auth_name == z_axis_rotation_unit.uom_auth_name, 'Cannot handle different rotation axis authorities'
                    assert x_axis_rotation_unit.uom_code == y_axis_rotation_unit.uom_code == z_axis_rotation_unit.uom_code, 'Cannot handle different rotation axis unit codes'

                    s = parsed_wkt2['COORDINATEOPERATION'][1]['Scale_Difference'][0]
                    scale_difference_unit = get_wkt_unit(
                        *parsed_wkt2['COORDINATEOPERATION'][1]['Scale_Difference'][1]['SCALEUNIT'])

                    rate_x = parsed_wkt2['COORDINATEOPERATION'][1]['X_Axis_Translation_Rate'][0]
                    x_axis_translation_rate_unit = get_wkt_unit(
                        *parsed_wkt2['COORDINATEOPERATION'][1]['X_Axis_Translation_Rate'][1]['LENGTHUNIT'], is_rate=True)
                    rate_y = parsed_wkt2['COORDINATEOPERATION'][1]['Y_Axis_Translation_Rate'][0]
                    y_axis_translation_rate_unit = get_wkt_unit(
                        *parsed_wkt2['COORDINATEOPERATION'][1]['Y_Axis_Translation_Rate'][1]['LENGTHUNIT'], is_rate=True)
                    rate_z = parsed_wkt2['COORDINATEOPERATION'][1]['Z_Axis_Translation_Rate'][0]
                    z_axis_translation_rate_unit = get_wkt_unit(
                        *parsed_wkt2['COORDINATEOPERATION'][1]['Z_Axis_Translation_Rate'][1]['LENGTHUNIT'], is_rate=True)
                    assert x_axis_translation_rate_unit.uom_auth_name == y_axis_translation_rate_unit.uom_auth_name == z_axis_translation_rate_unit.uom_auth_name, 'Cannot handle different translation rate axis authorities'
                    assert x_axis_translation_rate_unit.uom_code == y_axis_translation_rate_unit.uom_code == z_axis_translation_rate_unit.uom_code, 'Cannot handle different translation rate axis unit codes'

                    rate_rx = parsed_wkt2['COORDINATEOPERATION'][1]['X_Axis_Rotation_Rate'][0]
                    x_axis_rotation_rate_unit = get_wkt_unit(
                        *parsed_wkt2['COORDINATEOPERATION'][1]['X_Axis_Rotation_Rate'][1]['ANGLEUNIT'], is_rate=True)
                    rate_ry = parsed_wkt2['COORDINATEOPERATION'][1]['Y_Axis_Rotation_Rate'][0]
                    y_axis_rotation_rate_unit = get_wkt_unit(
                        *parsed_wkt2['COORDINATEOPERATION'][1]['Y_Axis_Rotation_Rate'][1]['ANGLEUNIT'], is_rate=True)
                    rate_rz = parsed_wkt2['COORDINATEOPERATION'][1]['Z_Axis_Rotation_Rate'][0]
                    z_axis_rotation_rate_unit = get_wkt_unit(
                        *parsed_wkt2['COORDINATEOPERATION'][1]['Z_Axis_Rotation_Rate'][1]['ANGLEUNIT'], is_rate=True)
                    assert x_axis_rotation_rate_unit.uom_auth_name == y_axis_rotation_rate_unit.uom_auth_name == z_axis_rotation_rate_unit.uom_auth_name, 'Cannot handle different rotation rate axis authorities'
                    assert x_axis_rotation_rate_unit.uom_code == y_axis_rotation_rate_unit.uom_code == z_axis_rotation_rate_unit.uom_code, 'Cannot handle different rotation rate axis unit codes'

                    rate_s = parsed_wkt2['COORDINATEOPERATION'][1]['Scale_Difference_Rate'][0]
                    scale_difference_rate_unit = get_wkt_unit(
                        *parsed_wkt2['COORDINATEOPERATION'][1]['Scale_Difference_Rate'][1]['SCALEUNIT'], is_rate=True)

                    reference_time = parsed_wkt2['COORDINATEOPERATION'][1]['Reference_Time'][0]
                    reference_time_unit = get_wkt_unit(
                        *parsed_wkt2['COORDINATEOPERATION'][1]['Reference_Time'][1]['SCALEUNIT'])
                    if reference_time_unit.uom_auth_name == 'EPSG' and reference_time_unit.uom_code == '9201':
                        # convert "Unity" values for Reference_Time to "years". The helmert_transformation table requires
                        # a time type uom for epochs
                        reference_time_unit.uom_code = '1029'

                    if is_Time_Based_Helmert_Coordinate_Frame:
                        method_code = '1057'
                        method_name = 'Time-dependent Coordinate Frame rotation (geog2D)'
                    else:
                        method_code = '1054'
                        method_name = 'Time-dependent Position Vector tfm (geog2D)'

                    sql = "INSERT INTO \"helmert_transformation\" VALUES('ESRI','{code}','{name}',NULL,'EPSG','{method_code}','{method_name}'," \
                                                              "'{source_crs_auth_name}','{source_crs_code}','{target_crs_auth_name}','{target_crs_code}'," \
                                                              "{accuracy},{tx},{ty},{tz},'{translation_uom_auth_name}','{translation_uom_code}'," \
                                                               "{rx},{ry},{rz},'{rotation_uom_auth_name}','{rotation_uom_code}',{scale_difference}," \
                                                              "'{scale_difference_uom_auth_name}','{scale_difference_uom_code}',{rate_tx},{rate_ty},{rate_tz}," \
                                                               "'{rate_translation_uom_auth_name}','{rate_translation_uom_code}',{rate_rx},{rate_ry},{rate_rz}," \
                                                                 "'{rate_rotation_uom_auth_name}','{rate_rotation_uom_code}',{rate_scale_difference},"\
                                                                "'{rate_scale_difference_uom_auth_name}','{rate_scale_difference_uom_code}',{epoch},"\
                          "'{epoch_uom_auth_name}','{epoch_uom_code}',NULL,NULL,NULL,NULL,NULL,NULL,{deprecated});".format(
                        code=wkid,
                        name=esri_name,
                        method_code=method_code,
                        method_name=method_name,
                        source_crs_auth_name=src_crs_auth_name,
                        source_crs_code=src_crs_code,
                        target_crs_auth_name=dst_crs_auth_name,
                        target_crs_code=dst_crs_code,
                        accuracy=accuracy,
                        tx=x,
                        ty=y,
                        tz=z,
                        translation_uom_auth_name=x_axis_translation_unit.uom_auth_name,
                        translation_uom_code=x_axis_translation_unit.uom_code,
                        rx=rx,
                        ry=ry,
                        rz=rz,
                        rotation_uom_auth_name=x_axis_rotation_unit.uom_auth_name,
                        rotation_uom_code=x_axis_rotation_unit.uom_code,
                        scale_difference=s,
                        scale_difference_uom_auth_name=scale_difference_unit.uom_auth_name,
                        scale_difference_uom_code=scale_difference_unit.uom_code,
                        rate_tx=rate_x,
                        rate_ty=rate_y,
                        rate_tz=rate_z,
                        rate_translation_uom_auth_name=x_axis_translation_rate_unit.uom_auth_name,
                        rate_translation_uom_code=x_axis_translation_rate_unit.uom_code,
                        rate_rx=rate_rx,
                        rate_ry=rate_ry,
                        rate_rz=rate_rz,
                        rate_rotation_uom_auth_name=x_axis_rotation_rate_unit.uom_auth_name,
                        rate_rotation_uom_code=x_axis_rotation_rate_unit.uom_code,
                        rate_scale_difference=rate_s,
                        rate_scale_difference_uom_auth_name=scale_difference_rate_unit.uom_auth_name,
                        rate_scale_difference_uom_code=scale_difference_rate_unit.uom_code,
                        epoch=reference_time,
                        epoch_uom_auth_name=reference_time_unit.uom_auth_name,
                        epoch_uom_code=reference_time_unit.uom_code,
                        deprecated=deprecated)
                    all_sql.append(sql)
                    sql = """INSERT INTO "usage" VALUES('ESRI', '%s_USAGE','helmert_transformation','ESRI','%s','%s','%s','%s','%s');""" % (wkid, wkid, extent_auth_name, extent_code, 'EPSG', '1024')
                    all_sql.append(sql)

                elif is_geog2d_offset:

                    # The only occurrence is quite boring: from NTF(Paris) to NTF.
                    # But interestingly the longitude offset value is not
                    # completely exactly the value of the Paris prime meridian

                    long_offset = parsed_wkt2['COORDINATEOPERATION'][1]['Longitude_Offset'][0]
                    longitude_offset_unit = get_wkt_unit(
                        *parsed_wkt2['COORDINATEOPERATION'][1]['Longitude_Offset'][1]['ANGLEUNIT'])

                    lat_offset = parsed_wkt2['COORDINATEOPERATION'][1]['Latitude_Offset'][0]
                    latitude_offset_unit = get_wkt_unit(
                        *parsed_wkt2['COORDINATEOPERATION'][1]['Latitude_Offset'][1]['ANGLEUNIT'])

                    sql = "INSERT INTO \"other_transformation\" VALUES('ESRI','{code}','{name}',NULL,'EPSG','9619','Geographic2D offsets',"\
                          "'{source_crs_auth_name}','{source_crs_code}','{target_crs_auth_name}','{target_crs_code}',{accuracy},"\
                          "'EPSG','8601','Latitude offset',{param1_value},'{param1_uom_auth_name}','{param1_uom_code}'," \
                          "'EPSG','8602','Longitude offset',{param2_value},'{param2_uom_auth_name}','{param2_uom_code}',NULL,NULL,NULL,NULL,NULL,NULL,"\
                            "NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,"\
                            "NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,{deprecated});".format(
                        code=wkid,
                        name=esri_name,
                        source_crs_auth_name=src_crs_auth_name,
                        source_crs_code=src_crs_code,
                        target_crs_auth_name=dst_crs_auth_name,
                        target_crs_code=dst_crs_code,
                        accuracy=accuracy,
                        param1_value=lat_offset,
                        param1_uom_auth_name=latitude_offset_unit.uom_auth_name,
                        param1_uom_code=latitude_offset_unit.uom_code,
                        param2_value=long_offset,
                        param2_uom_auth_name=longitude_offset_unit.uom_auth_name,
                        param2_uom_code=longitude_offset_unit.uom_code,
                        deprecated=deprecated)
                    all_sql.append(sql)
                    sql = """INSERT INTO "usage" VALUES('ESRI', '%s_USAGE','other_transformation','ESRI','%s','%s','%s','%s','%s');""" % (wkid, wkid, extent_auth_name, extent_code, 'EPSG', '1024')
                    all_sql.append(sql)

                elif is_null:
                    long_offset = '0'
                    lat_offset = '0'

                    assert set(parsed_wkt2['COORDINATEOPERATION'][1].keys()) == {'SOURCECRS', 'METHOD', 'TARGETCRS', 'OPERATIONACCURACY'}, set(parsed_wkt2['COORDINATEOPERATION'][1].keys())

                    sql = """INSERT INTO "other_transformation" VALUES('ESRI','%s','%s',NULL,'EPSG','9619','Geographic2D offsets','%s','%s','%s','%s',%s,'EPSG','8601','Latitude offset',%s,'EPSG','9104','EPSG','8602','Longitude offset',%s,'EPSG','9104',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,%d);""" % (
                        wkid, esri_name, src_crs_auth_name, src_crs_code, dst_crs_auth_name, dst_crs_code, accuracy, lat_offset, long_offset, deprecated)
                    all_sql.append(sql)
                    sql = """INSERT INTO "usage" VALUES('ESRI', '%s_USAGE','other_transformation','ESRI','%s','%s','%s','%s','%s');""" % (wkid, wkid, extent_auth_name, extent_code, 'EPSG', '1024')
                    all_sql.append(sql)

                elif is_unitchange:

                    # Automatically handled by PROJ. Not worth importing
                    continue

                else:
                    assert set(k for k in parsed_wkt2['COORDINATEOPERATION'][1].keys() if k != 'OPERATIONACCURACY') == {'SOURCECRS', 'METHOD', 'TARGETCRS', 'PARAMETERFILE'}, set(parsed_wkt2['COORDINATEOPERATION'][1].keys())

                    filename = parsed_wkt2['COORDINATEOPERATION'][1]['PARAMETERFILE']

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
