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
version = 'ArcMap 10.7.0'
all_sql.append(
    """INSERT INTO "metadata" VALUES('ESRI.VERSION', '%s');""" % (version))
date = '2019-03-25'
all_sql.append(
    """INSERT INTO "metadata" VALUES('ESRI.DATE', '%s');""" % (date))


def escape_literal(x):
    return x.replace("'", "''")


########################

map_areaname_to_auth_code = {}
esri_area_counter = 1


def find_area(areaname, slat, nlat, llon, rlon):

    global esri_area_counter

    if areaname in map_areaname_to_auth_code:
        return map_areaname_to_auth_code[areaname]

    deg = b'\xC2\xB0'.decode('utf-8')
    cursor.execute("SELECT auth_name, code FROM area WHERE name = ? AND auth_name != 'ESRI'",
                   (areaname.replace('~', deg),))
    row = cursor.fetchone()
    if row is None:
        cursor.execute(
            "SELECT auth_name, code FROM area WHERE auth_name != 'ESRI' AND south_lat = ? AND north_lat = ? AND west_lon = ? AND east_lon = ?", (slat, nlat, llon, rlon))
        row = cursor.fetchone()

    if row is None:
        #print('unknown area inserted: ' + areaname)

        if float(rlon) > 180:
            new_rlon = '%s' % (float(rlon) - 360)
            print('Correcting rlon from %s to %s for %s' %
                  (rlon, new_rlon, areaname))
            rlon = new_rlon

        assert float(slat) >= -90 and float(slat) <= 90, (areaname,
                                                          slat, nlat, llon, rlon)
        assert float(nlat) >= -90 and float(nlat) <= 90, (areaname,
                                                          slat, nlat, llon, rlon)
        assert float(nlat) > float(slat), (areaname, slat, nlat, llon, rlon)
        assert float(llon) >= -180 and float(llon) <= 180, (areaname,
                                                            slat, nlat, llon, rlon)
        assert float(rlon) >= -180 and float(rlon) <= 180, (areaname,
                                                            slat, nlat, llon, rlon)

        sql = """INSERT INTO "area" VALUES('ESRI','%d','%s','%s',%s,%s,%s,%s,0);""" % (
            esri_area_counter, escape_literal(areaname), escape_literal(areaname), slat, nlat, llon, rlon)
        all_sql.append(sql)
        map_areaname_to_auth_code[areaname] = [
            'ESRI', '%d' % esri_area_counter]
        esri_area_counter += 1
    else:
        auth_name = row[0]
        code = row[1]
        map_areaname_to_auth_code[areaname] = [auth_name, code]

    return map_areaname_to_auth_code[areaname]

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

                area_auth_name, area_code = find_area(
                    row[idx_areaname], row[idx_slat], row[idx_nlat], row[idx_llon], row[idx_rlon])

                if datum_auth_name == 'ESRI':
                    if datum_code not in datum_written:
                        datum_written.add(datum_code)

                        p = map_datum_esri_to_parameters[datum_code]

                        sql = """INSERT INTO "geodetic_datum" VALUES('ESRI','%s','%s','%s',NULL,'%s','%s','%s','%s','%s','%s',%d);""" % (
                            datum_code, p['esri_name'], p['description'], p['ellps_auth_name'], p['ellps_code'], pm_auth_name, pm_code, area_auth_name, area_code, p['deprecated'])
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

                            sql = """INSERT INTO "geodetic_datum" VALUES('ESRI','%s','%s',NULL,'%s','%s','%s','%s','%s','%s','%s',%d);""" % (
                                datum_code, p['esri_name'], p['description'], p['ellps_auth_name'], p['ellps_code'], pm_auth_name, pm_code, area_auth_name, area_code, p['deprecated'])
                            all_sql.append(sql)
                            p['pm_auth_name'] = pm_auth_name
                            p['pm_code'] = pm_code
                            map_datum_esri_to_parameters[datum_code] = p


                # We may have already the EPSG entry, so use it preferably
                if esri_name not in map_geogcs_esri_name_to_auth_code:
                    map_geogcs_esri_name_to_auth_code[esri_name] = ['ESRI', code]

                sql = """INSERT INTO "geodetic_crs" VALUES('ESRI','%s','%s',NULL,NULL,'geographic 2D','EPSG','%s','%s','%s','%s','%s',NULL,%d);""" % (
                    code, esri_name, cs_code, datum_auth_name, datum_code, area_auth_name, area_code, deprecated)
                all_sql.append(sql)

                if deprecated and code != latestWkid and code not in ('4305', '4812'): # Voirol 1960 no longer in EPSG
                    cursor.execute(
                        "SELECT name FROM geodetic_crs WHERE auth_name = 'EPSG' AND code = ?", (latestWkid,))
                    src_row = cursor.fetchone()
                    assert src_row, (code, latestWkid)

                    sql = """INSERT INTO "supersession" VALUES('geodetic_crs','ESRI','%s','geodetic_crs','EPSG','%s','ESRI');""" % (
                        code, latestWkid)
                    all_sql.append(sql)

########################


map_projcs_esri_name_to_auth_code = {}


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

                assert geogcs_name in map_geogcs_esri_name_to_auth_code, (
                    geogcs_name, row)
                geogcs_auth_name, geogcs_code = map_geogcs_esri_name_to_auth_code[geogcs_name]

                area_auth_name, area_code = find_area(
                    row[idx_areaname], row[idx_slat], row[idx_nlat], row[idx_llon], row[idx_rlon])

                map_projcs_esri_name_to_auth_code[esri_name] = ['ESRI', code]

                deprecated = 1 if row[idx_deprecated] == 'yes' else 0

                sql = """INSERT INTO "projected_crs" VALUES('ESRI','%s','%s',NULL,NULL,NULL,NULL,'%s','%s',NULL,NULL,'%s','%s','%s',%d);""" % (
                    code, esri_name, geogcs_auth_name, geogcs_code, area_auth_name, area_code, escape_literal(wkt), deprecated)
                all_sql.append(sql)

                if deprecated and code != latestWkid:
                    mapDeprecatedToNonDeprecated[code] = latestWkid

        for deprecated in mapDeprecatedToNonDeprecated:
            code = deprecated
            latestWkid = mapDeprecatedToNonDeprecated[deprecated]

            if latestWkid in wkid_set:
                sql = """INSERT INTO "supersession" VALUES('projected_crs','ESRI','%s','projected_crs','ESRI','%s','ESRI');""" % (
                    code, latestWkid)
                all_sql.append(sql)
            else:
                cursor.execute(
                    "SELECT name FROM projected_crs WHERE auth_name = 'EPSG' AND code = ?", (latestWkid,))
                src_row = cursor.fetchone()
                assert src_row, row
                sql = """INSERT INTO "supersession" VALUES('projected_crs','ESRI','%s','projected_crs','EPSG','%s','ESRI');""" % (
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

                if ',DATUM[' in wkt:
                    # FIXME ??
                    print('Skipping %s. Should be a CompoundCRS' % (esri_name))
                    sql = """-- Skipping %s. Should be a CompoundCRS""" % (
                        esri_name)
                    all_sql.append(sql)
                    continue

                pos = wkt.find('VDATUM["')
                assert pos >= 0
                pos += len('VDATUM["')
                end_pos = wkt[pos:].find('"')
                assert end_pos >= 0
                end_pos += pos
                datum_name = wkt[pos:end_pos]

                assert 'PARAMETER["Vertical_Shift",0.0]' in wkt, row

                if datum_name not in map_vdatum_esri_name_to_auth_code:
                    print('Skipping vertcs %s. it expresses an ellipsoidal height regarding a horizontal datum' % (
                        str(row)))
                    sql = """-- Skipping vertcs %s. it expresses an ellipsoidal height regarding a horizontal datum""" % (
                        esri_name, datum_name)
                    all_sql.append(sql)
                    continue

                datum_auth_name, datum_code = map_vdatum_esri_name_to_auth_code[datum_name]

                deprecated = 1 if row[idx_deprecated] == 'yes' else 0

                area_auth_name, area_code = find_area(
                    row[idx_areaname], row[idx_slat], row[idx_nlat], row[idx_llon], row[idx_rlon])

                if datum_auth_name == 'ESRI':
                    if datum_code not in datum_written:
                        datum_written.add(datum_code)

                        p = map_vdatum_esri_to_parameters[datum_code]
                        sql = """INSERT INTO "vertical_datum" VALUES('ESRI','%s','%s',NULL,NULL,'%s','%s',%d);""" % (
                            datum_code, p['esri_name'], area_auth_name, area_code, p['deprecated'])
                        all_sql.append(sql)

                map_vertcs_esri_name_to_auth_code[esri_name] = ['ESRI', code]

                if 'PARAMETER["Direction",1.0]' in wkt and 'UNIT["Meter"' in wkt:
                    cs_code = 6499
                elif 'PARAMETER["Direction",1.0]' in wkt and 'UNIT["Foot"' in wkt:
                    cs_code = 1030
                elif 'PARAMETER["Direction",1.0]' in wkt and 'UNIT["Foot_US"' in wkt:
                    cs_code = 6497
                else:
                    assert False, ('unknown coordinate system for %s' %
                                   str(row))

                sql = """INSERT INTO "vertical_crs" VALUES('ESRI','%s','%s',NULL,NULL,'EPSG','%s','%s','%s','%s','%s',%d);""" % (
                    code, esri_name, cs_code, datum_auth_name, datum_code, area_auth_name, area_code, deprecated)
                all_sql.append(sql)

                if deprecated and code != latestWkid:
                    cursor.execute(
                        "SELECT name FROM vertical_crs WHERE auth_name = 'EPSG' AND code = ?", (latestWkid,))
                    src_row = cursor.fetchone()
                    assert src_row

                    sql = """INSERT INTO "supersession" VALUES('vertical_crs','ESRI','%s','vertical_crs','EPSG','%s','ESRI');""" % (
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

                # Don't do anything particular in part of checking we now
                # it
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
                assert is_cf or is_pv or is_geocentric_translation or is_molodensky_badekas or is_nadcon or is_geog2d_offset or is_ntv2 or is_geocon or is_null or is_harn or is_unitchange, (
                    row)

                area_auth_name, area_code = find_area(
                    row[idx_areaname], row[idx_slat], row[idx_nlat], row[idx_llon], row[idx_rlon])

                accuracy = row[idx_accuracy]

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

                    sql = """INSERT INTO "helmert_transformation" VALUES('ESRI','%s','%s',NULL,NULL,'EPSG','%s','%s','%s','%s','%s','%s','%s','%s',%s,%s,%s,%s,'EPSG','9001',%s,%s,%s,'EPSG','9104',%s,'EPSG','9202',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,%d);""" % (
                        wkid, esri_name, method_code, method_name, src_crs_auth_name, src_crs_code, dst_crs_auth_name, dst_crs_code, area_auth_name, area_code, accuracy, x, y, z, rx, ry, rz, s, deprecated)
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

                    sql = """INSERT INTO "helmert_transformation" VALUES('ESRI','%s','%s',NULL,NULL,'EPSG','%s','%s','%s','%s','%s','%s','%s','%s',%s,%s,%s,%s,'EPSG','9001',%s,%s,%s,'EPSG','9104',%s,'EPSG','9202',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,%s,%s,%s,'EPSG','9001',NULL,%d);""" % (
                        wkid, esri_name, method_code, method_name, src_crs_auth_name, src_crs_code, dst_crs_auth_name, dst_crs_code, area_auth_name, area_code, accuracy, x, y, z, rx, ry, rz, s, px, py, pz, deprecated)
                    all_sql.append(sql)

                elif is_geocentric_translation:
                    x = get_parameter(wkt, 'X_Axis_Translation')
                    y = get_parameter(wkt, 'Y_Axis_Translation')
                    z = get_parameter(wkt, 'Z_Axis_Translation')
                    assert wkt.count('PARAMETER[') == 3

                    method_code = '9603'
                    method_name = 'Geocentric translations (geog2D domain)'

                    sql = """INSERT INTO "helmert_transformation" VALUES('ESRI','%s','%s',NULL,NULL,'EPSG','%s','%s','%s','%s','%s','%s','%s','%s',%s,%s,%s,%s,'EPSG','9001',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,%d);""" % (
                        wkid, esri_name, method_code, method_name, src_crs_auth_name, src_crs_code, dst_crs_auth_name, dst_crs_code, area_auth_name, area_code, accuracy, x, y, z, deprecated)
                    all_sql.append(sql)

                elif is_geog2d_offset:

                    # The only occurence is quite boring: from NTF(Paris) to NTF.
                    # But interestingly the longitude offset value is not
                    # completely exactly the value of the Paris prime meridian

                    long_offset = get_parameter(
                        wkt, 'Longitude_Offset')  # in arc second
                    lat_offset = get_parameter(wkt, 'Latitude_Offset')
                    assert wkt.count('PARAMETER[') == 2

                    sql = """INSERT INTO "other_transformation" VALUES('ESRI','%s','%s',NULL,NULL,'EPSG','9619','Geographic2D offsets','%s','%s','%s','%s','%s','%s',%s,'EPSG','8601','Latitude offset',%s,'EPSG','9104','EPSG','8602','Longitude offset',%s,'EPSG','9104',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,%d);""" % (
                        wkid, esri_name, src_crs_auth_name, src_crs_code, dst_crs_auth_name, dst_crs_code, area_auth_name, area_code, accuracy, lat_offset, long_offset, deprecated)
                    all_sql.append(sql)

                elif is_null:
                    long_offset = '0'
                    lat_offset = '0'
                    assert wkt.count('PARAMETER[') == 0

                    sql = """INSERT INTO "other_transformation" VALUES('ESRI','%s','%s',NULL,NULL,'EPSG','9619','Geographic2D offsets','%s','%s','%s','%s','%s','%s',%s,'EPSG','8601','Latitude offset',%s,'EPSG','9104','EPSG','8602','Longitude offset',%s,'EPSG','9104',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,%d);""" % (
                        wkid, esri_name, src_crs_auth_name, src_crs_code, dst_crs_auth_name, dst_crs_code, area_auth_name, area_code, accuracy, lat_offset, long_offset, deprecated)
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
                        "SELECT name, grid_name FROM grid_transformation WHERE auth_name != 'ESRI' AND source_crs_auth_name = ? AND source_crs_code = ? AND target_crs_auth_name = ? AND target_crs_code = ? AND area_of_use_auth_name = ? AND area_of_use_code = ?", (src_crs_auth_name, src_crs_code, dst_crs_auth_name, dst_crs_code, area_auth_name, area_code))
                    src_row = cursor.fetchone()
                    if src_row:
                        print('A grid_transformation (%s, using grid %s) is already known for the equivalent of %s (%s:%s --> %s:%s) for area %s, which uses grid %s. Skipping it' % (src_row[0], src_row[1], esri_name, src_crs_auth_name, src_crs_code, dst_crs_auth_name, dst_crs_code, row[idx_areaname], filename))
                        continue

                    sql = """INSERT INTO "grid_transformation" VALUES('ESRI','%s','%s',NULL,NULL,'EPSG','9615','NTv2','%s','%s','%s','%s','%s','%s',%s,'EPSG','8656','Latitude and longitude difference file','%s',NULL,NULL,NULL,NULL,NULL,NULL,NULL,%d);""" % (
                        wkid, esri_name, src_crs_auth_name, src_crs_code, dst_crs_auth_name, dst_crs_code, area_auth_name, area_code, accuracy, filename, deprecated)
                    all_sql.append(sql)

                    if filename in ('c1hpgn', 'c2hpgn'):
                        sql = """INSERT INTO grid_alternatives VALUES ('%s', '%s', 'NTv2', 'hgridshift', 0, 'proj-datumgrid-north-america', NULL, NULL, NULL, NULL);""" % (
                            filename, filename + '.gsb')
                        all_sql.append(sql)
                    elif filename == 'wohpgn':
                        sql = """INSERT INTO grid_alternatives VALUES ('%s', '%s', 'CTable2', 'hgridshift', 0, 'proj-datumgrid', NULL, NULL, NULL, NULL);""" % (
                            filename, 'WO')
                        all_sql.append(sql)
                    elif filename == 'prvi':
                        continue
                    else:
                        print('not handled grid: ' + filename)


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
f.close()

print('')
print('Finished !')
print('NOTE: adding into metadata: ESRI.VERSION = %s. Update if needed !' % version)
