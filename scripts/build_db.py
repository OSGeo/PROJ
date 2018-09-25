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

import csv
import os
import sqlite3
import sys

EPSG_AUTHORITY = 'EPSG'

def ingest_sqlite_dump(cursor, filename):
    sql = ''
    for line in open(filename, 'rt').readlines():
        sql += line
        if sqlite3.complete_statement(sql):
            sql = sql.strip()
            if sql != 'COMMIT;':
                cursor.execute(sql)
            sql = ''


def ingest_epsg():

    for f in ['PostgreSQL_Data_Script.sql',  'PostgreSQL_Table_Script.sql']:
        if not os.path.exists(f):
            raise Exception('Missing file: ' + f)

    epsg_tmp_db_filename = 'tmp_epsg.db'

    if os.path.exists(epsg_tmp_db_filename):
        os.unlink(epsg_tmp_db_filename)

    conn = sqlite3.connect(epsg_tmp_db_filename)
    cursor = conn.cursor()
    cursor.execute('PRAGMA journal_mode = OFF;')
    ingest_sqlite_dump(cursor, 'PostgreSQL_Table_Script.sql')
    ingest_sqlite_dump(cursor, 'PostgreSQL_Data_Script.sql')
    cursor.close()
    conn.commit()

    return (conn, epsg_tmp_db_filename)


def fill_unit_of_measure(proj_db_cursor):
    proj_db_cursor.execute(
        "INSERT INTO unit_of_measure SELECT ?, uom_code, unit_of_meas_name, unit_of_meas_type, factor_b / factor_c, deprecated FROM epsg.epsg_unitofmeasure", (EPSG_AUTHORITY,))


def fill_ellipsoid(proj_db_cursor):
    proj_db_cursor.execute(
        "INSERT INTO ellipsoid SELECT ?, ellipsoid_code, ellipsoid_name, semi_major_axis, ?, uom_code, inv_flattening, semi_minor_axis, deprecated FROM epsg.epsg_ellipsoid", (EPSG_AUTHORITY, EPSG_AUTHORITY))


def fill_area(proj_db_cursor):
    proj_db_cursor.execute(
        "INSERT INTO area SELECT ?, area_code, area_name, area_of_use, area_south_bound_lat, area_north_bound_lat, area_west_bound_lon, area_east_bound_lon, deprecated FROM epsg.epsg_area", (EPSG_AUTHORITY,))


def fill_prime_meridian(proj_db_cursor):
    proj_db_cursor.execute(
        "INSERT INTO prime_meridian SELECT ?, prime_meridian_code, prime_meridian_name, greenwich_longitude, ?, uom_code, deprecated FROM epsg.epsg_primemeridian", (EPSG_AUTHORITY, EPSG_AUTHORITY))


def fill_geodetic_datum(proj_db_cursor):
    proj_db_cursor.execute(
        "SELECT DISTINCT * FROM epsg.epsg_datum WHERE datum_type NOT IN ('geodetic', 'vertical', 'engineering')")
    res = proj_db_cursor.fetchall()
    if res:
        raise Exception('Found unexpected datum_type in epsg_datum: %s' % str(res))

    proj_db_cursor.execute(
        "INSERT INTO geodetic_datum SELECT ?, datum_code, datum_name, ?, ellipsoid_code, ?, prime_meridian_code, ?, area_of_use_code, deprecated FROM epsg.epsg_datum WHERE datum_type = 'geodetic'", (EPSG_AUTHORITY, EPSG_AUTHORITY, EPSG_AUTHORITY, EPSG_AUTHORITY))


def fill_vertical_datum(proj_db_cursor):
    proj_db_cursor.execute(
        "INSERT INTO vertical_datum SELECT ?, datum_code, datum_name, ?, area_of_use_code, deprecated FROM epsg.epsg_datum WHERE datum_type = 'vertical'", (EPSG_AUTHORITY,EPSG_AUTHORITY))


def fill_coordinate_system(proj_db_cursor):
    proj_db_cursor.execute(
        "INSERT INTO coordinate_system SELECT ?, coord_sys_code, coord_sys_type, dimension FROM epsg.epsg_coordinatesystem", (EPSG_AUTHORITY,))


def fill_axis(proj_db_cursor):
    proj_db_cursor.execute("INSERT INTO axis SELECT ?, coord_axis_code, coord_axis_name, coord_axis_abbreviation, coord_axis_orientation, ?, coord_sys_code, coord_axis_order, ?, uom_code FROM epsg.epsg_coordinateaxis ca LEFT JOIN epsg.epsg_coordinateaxisname can ON ca.coord_axis_name_code = can.coord_axis_name_code", (EPSG_AUTHORITY, EPSG_AUTHORITY, EPSG_AUTHORITY))


def fill_geodetic_crs(proj_db_cursor):
    proj_db_cursor.execute(
        "SELECT DISTINCT * FROM epsg.epsg_coordinatereferencesystem WHERE coord_ref_sys_kind NOT IN ('projected', 'geographic 2D', 'geographic 3D', 'geocentric', 'vertical', 'compound', 'engineering')")
    res = proj_db_cursor.fetchall()
    if res:
        raise Exception('Found unexpected coord_ref_sys_kind in epsg_coordinatereferencesystem: %s' % str(res))

    proj_db_cursor.execute(
        "INSERT INTO crs SELECT ?, coord_ref_sys_code, coord_ref_sys_kind FROM epsg.epsg_coordinatereferencesystem WHERE coord_ref_sys_kind IN ('geographic 2D', 'geographic 3D', 'geocentric') AND datum_code IS NOT NULL", (EPSG_AUTHORITY,))
    proj_db_cursor.execute("INSERT INTO geodetic_crs SELECT ?, coord_ref_sys_code, coord_ref_sys_name, coord_ref_sys_kind, ?, coord_sys_code, ?, datum_code, ?, area_of_use_code, deprecated FROM epsg.epsg_coordinatereferencesystem WHERE coord_ref_sys_kind IN ('geographic 2D', 'geographic 3D', 'geocentric') AND datum_code IS NOT NULL", (EPSG_AUTHORITY, EPSG_AUTHORITY, EPSG_AUTHORITY, EPSG_AUTHORITY))


def fill_vertical_crs(proj_db_cursor):
    proj_db_cursor.execute(
        "INSERT INTO crs SELECT ?, coord_ref_sys_code, coord_ref_sys_kind FROM epsg.epsg_coordinatereferencesystem WHERE coord_ref_sys_kind IN ('vertical') AND datum_code IS NOT NULL", (EPSG_AUTHORITY,))
    proj_db_cursor.execute("INSERT INTO vertical_crs SELECT ?, coord_ref_sys_code, coord_ref_sys_name, ?, coord_sys_code, ?, datum_code, ?, area_of_use_code, deprecated FROM epsg.epsg_coordinatereferencesystem WHERE coord_ref_sys_kind IN ('vertical') AND datum_code IS NOT NULL", (EPSG_AUTHORITY, EPSG_AUTHORITY, EPSG_AUTHORITY, EPSG_AUTHORITY))


def fill_conversion(proj_db_cursor):
    proj_db_cursor.execute("SELECT coord_op_code, coord_op_name, coord_op_method_code, coord_op_method_name FROM epsg.epsg_coordoperation LEFT JOIN epsg.epsg_coordoperationmethod USING (coord_op_method_code) WHERE coord_op_type = 'conversion' AND coord_op_name NOT LIKE '%to DMSH'")
    for (code, name, method_code, method_name) in proj_db_cursor.fetchall():
        expected_order = 1
        param_auth_name = [None for i in range(7)]
        param_code = [None for i in range(7)]
        param_name = [None for i in range(7)]
        param_value = [None for i in range(7)]
        param_uom_auth_name = [None for i in range(7)]
        param_uom_code = [None for i in range(7)]

        iterator = proj_db_cursor.execute("SELECT sort_order, cop.parameter_code, parameter_name, parameter_value, uom_code from epsg_coordoperationparam cop LEFT JOIN epsg_coordoperationparamvalue copv LEFT JOIN epsg_coordoperationparamusage copu ON cop.parameter_code = copv.parameter_code AND copu.parameter_code = copv.parameter_code WHERE copu.coord_op_method_code = copv.coord_op_method_code AND coord_op_code = ? AND copv.coord_op_method_code = ? ORDER BY sort_order", (code, method_code))
        for (order, parameter_code, parameter_name, parameter_value, uom_code) in iterator:
            # Modified Krovak and Krovak North Oriented: keep only the 7 first parameters
            if order == 8 and method_code in (1042, 1043):
                break
            assert order < 8
            assert order == expected_order
            param_auth_name[order - 1] = EPSG_AUTHORITY
            param_code[order - 1] = parameter_code
            param_name[order - 1] = parameter_name
            param_value[order - 1] = parameter_value
            param_uom_auth_name[order - 1] = EPSG_AUTHORITY
            param_uom_code[order - 1] = uom_code
            expected_order += 1

        arg = (EPSG_AUTHORITY, code, name, EPSG_AUTHORITY, method_code, method_name,
               param_auth_name[0], param_code[0], param_name[0],
               param_value[0], param_uom_auth_name[0], param_uom_code[0],
               param_auth_name[1], param_code[1], param_name[1], param_value[1],
               param_uom_auth_name[1], param_uom_code[1], param_auth_name[2],
               param_code[2], param_name[2], param_value[2],
               param_uom_auth_name[2], param_uom_code[2],
               param_auth_name[3], param_code[3], param_name[3], param_value[3],
               param_uom_auth_name[3], param_uom_code[3], param_auth_name[4],
               param_code[4], param_name[4], param_value[4],
               param_uom_auth_name[4], param_uom_code[4], param_auth_name[5],
               param_code[5], param_name[5], param_value[5],
               param_uom_auth_name[5], param_uom_code[5], param_auth_name[6],
               param_code[6], param_name[6], param_value[6],
               param_uom_auth_name[6], param_uom_code[6])

        proj_db_cursor.execute("INSERT INTO coordinate_operation VALUES (?,?,'conversion')", (EPSG_AUTHORITY, code))
        proj_db_cursor.execute('INSERT INTO conversion VALUES (' +
            '?,?,?, ?,?,?, ?,?,?,?,?,?, ?,?,?,?,?,?, ?,?,?,?,?,?, ' +
            '?,?,?,?,?,?, ?,?,?,?,?,?, ?,?,?,?,?,?, ?,?,?,?,?,?)', arg)


def fill_projected_crs(proj_db_cursor):
    #proj_db_cursor.execute(
    #    "INSERT INTO crs SELECT 'EPSG', coord_ref_sys_code, coord_ref_sys_kind FROM epsg.epsg_coordinatereferencesystem WHERE coord_ref_sys_kind IN ('projected')")
    #proj_db_cursor.execute("INSERT INTO projected_crs SELECT 'EPSG', coord_ref_sys_code, coord_ref_sys_name, 'EPSG', coord_sys_code, 'EPSG', source_geogcrs_code, 'EPSG', projection_conv_code, 'EPSG', area_of_use_code, deprecated FROM epsg.epsg_coordinatereferencesystem WHERE coord_ref_sys_kind IN ('projected')")
    proj_db_cursor.execute("SELECT ?, coord_ref_sys_code, coord_ref_sys_name, ?, coord_sys_code, ?, source_geogcrs_code, ?, projection_conv_code, ?, area_of_use_code, deprecated FROM epsg.epsg_coordinatereferencesystem WHERE coord_ref_sys_kind IN ('projected')", (EPSG_AUTHORITY, EPSG_AUTHORITY, EPSG_AUTHORITY, EPSG_AUTHORITY, EPSG_AUTHORITY))
    for row in proj_db_cursor.fetchall():
        (auth_name, code, name, coordinate_system_auth_name, coordinate_system_code, geodetic_crs_auth_name, geodetic_crs_code, conversion_auth_name, conversion_code, area_of_use_auth_name, area_of_use_code, deprecated) = row
        proj_db_cursor.execute("SELECT 1 FROM epsg.epsg_coordinatereferencesystem WHERE coord_ref_sys_code = ? AND coord_ref_sys_kind IN ('geographic 2D', 'geographic 3D', 'geocentric')", (geodetic_crs_code,))
        if proj_db_cursor.fetchone():
            proj_db_cursor.execute("INSERT INTO crs VALUES (?, ?, 'projected')", (EPSG_AUTHORITY, code))
            proj_db_cursor.execute("INSERT INTO projected_crs VALUES (?,?,?,?,?,?,?,?,?,?,?,?)", row)

def fill_compound_crs(proj_db_cursor):
    proj_db_cursor.execute(
        "INSERT INTO crs SELECT ?, coord_ref_sys_code, coord_ref_sys_kind FROM epsg.epsg_coordinatereferencesystem WHERE coord_ref_sys_kind IN ('compound')", (EPSG_AUTHORITY,))
    proj_db_cursor.execute("INSERT INTO compound_crs SELECT ?, coord_ref_sys_code, coord_ref_sys_name, ?, cmpd_horizcrs_code, ?, cmpd_vertcrs_code, ?, area_of_use_code, deprecated FROM epsg.epsg_coordinatereferencesystem WHERE coord_ref_sys_kind IN ('compound')", (EPSG_AUTHORITY, EPSG_AUTHORITY, EPSG_AUTHORITY, EPSG_AUTHORITY))

def fill_helmert_transformation(proj_db_cursor):
    proj_db_cursor.execute("SELECT coord_op_code, coord_op_name, coord_op_method_code, coord_op_method_name, source_crs_code, target_crs_code, area_of_use_code, coord_op_accuracy, epsg_coordoperation.deprecated FROM epsg.epsg_coordoperation LEFT JOIN epsg.epsg_coordoperationmethod USING (coord_op_method_code) WHERE coord_op_type = 'transformation' AND coord_op_method_code IN (1031, 1032, 1033, 1035, 1037, 1038, 1053, 1054, 1055, 1056, 1057, 1058, 1065, 1066, 9603, 9606, 9607) ")
    for (code, name, method_code, method_name, source_crs_code, target_crs_code, area_of_use_code, coord_op_accuracy, deprecated) in proj_db_cursor.fetchall():
        expected_order = 1
        max_n_params = 15
        param_auth_name = [None for i in range(max_n_params)]
        param_code = [None for i in range(max_n_params)]
        param_name = [None for i in range(max_n_params)]
        param_value = [None for i in range(max_n_params)]
        param_uom_code = [None for i in range(max_n_params)]

        iterator = proj_db_cursor.execute("SELECT sort_order, cop.parameter_code, parameter_name, parameter_value, uom_code from epsg_coordoperationparam cop LEFT JOIN epsg_coordoperationparamvalue copv LEFT JOIN epsg_coordoperationparamusage copu ON cop.parameter_code = copv.parameter_code AND copu.parameter_code = copv.parameter_code WHERE copu.coord_op_method_code = copv.coord_op_method_code AND coord_op_code = ? AND copv.coord_op_method_code = ? ORDER BY sort_order", (code, method_code))
        for (order, parameter_code, parameter_name, parameter_value, uom_code) in iterator:
            assert order <= max_n_params
            assert order == expected_order
            param_auth_name[order - 1] = EPSG_AUTHORITY
            param_code[order - 1] = parameter_code
            param_name[order - 1] = parameter_name
            param_value[order - 1] = parameter_value
            param_uom_code[order - 1] = uom_code
            expected_order += 1
        n_params = expected_order - 1

        assert param_code[0] == 8605
        assert param_code[1] == 8606
        assert param_code[2] == 8607
        assert param_uom_code[0] == param_uom_code[1]
        assert param_uom_code[0] == param_uom_code[2]
        if n_params > 3:
            assert param_code[3] == 8608
            assert param_code[4] == 8609
            assert param_code[5] == 8610
            assert param_code[6] == 8611
            assert param_uom_code[3] == param_uom_code[4]
            assert param_uom_code[3] == param_uom_code[5]
        if n_params == 8:
            assert param_code[7] == 1049, (code, name, param_code[7])
            param_value[14] = param_value[7]
            param_uom_code[14] = param_uom_code[7]
            param_value[7] = None
            param_uom_code[7] = None
        elif n_params > 7:
            assert param_code[7] == 1040, (code, name, param_code[7])
            assert param_code[8] == 1041
            assert param_code[9] == 1042
            assert param_code[10] == 1043
            assert param_code[11] == 1044
            assert param_code[12] == 1045
            assert param_code[13] == 1046
            assert param_code[14] == 1047
            assert param_uom_code[7] == param_uom_code[8]
            assert param_uom_code[7] == param_uom_code[9]
            assert param_uom_code[10] == param_uom_code[11]
            assert param_uom_code[10] == param_uom_code[12]

        arg = (EPSG_AUTHORITY, code, name,
               EPSG_AUTHORITY, method_code, method_name,
               EPSG_AUTHORITY, source_crs_code,
               EPSG_AUTHORITY, target_crs_code,
               EPSG_AUTHORITY, area_of_use_code,
               coord_op_accuracy,
               param_value[0], param_value[1], param_value[2], EPSG_AUTHORITY, param_uom_code[0],
               param_value[3], param_value[4], param_value[5], EPSG_AUTHORITY if param_uom_code[3] else None, param_uom_code[3],
               param_value[6], EPSG_AUTHORITY if param_uom_code[6] else None, param_uom_code[6],
               param_value[7], param_value[8], param_value[9], EPSG_AUTHORITY if param_uom_code[7] else None, param_uom_code[7],
               param_value[10], param_value[11], param_value[12], EPSG_AUTHORITY if param_uom_code[10] else None, param_uom_code[10],
               param_value[13], EPSG_AUTHORITY if param_uom_code[13] else None, param_uom_code[13],
               param_value[14], EPSG_AUTHORITY if param_uom_code[14] else None, param_uom_code[14],
               deprecated
               )

        proj_db_cursor.execute("INSERT INTO coordinate_operation VALUES (?,?,'helmert_transformation')", (EPSG_AUTHORITY, code))
        proj_db_cursor.execute('INSERT INTO helmert_transformation VALUES (' +
            '?,?,?, ?,?,?, ?,?, ?,?, ?,?, ?, ?,?,?,?,?, ?,?,?,?,?, ?,?,?, ?,?,?,?,?, ?,?,?,?,?, ?,?,?, ?,?,?, ?)', arg)

def fill_grid_transformation(proj_db_cursor):
    proj_db_cursor.execute("SELECT coord_op_code, coord_op_name, coord_op_method_code, coord_op_method_name, source_crs_code, target_crs_code, area_of_use_code, coord_op_accuracy, epsg_coordoperation.deprecated FROM epsg.epsg_coordoperation LEFT JOIN epsg.epsg_coordoperationmethod USING (coord_op_method_code) WHERE coord_op_type = 'transformation' AND (coord_op_method_name LIKE 'Geographic3D to%' OR coord_op_method_name LIKE 'Geog3D to%' OR coord_op_method_name LIKE 'Point motion by grid%' OR coord_op_method_name LIKE 'Vertical Offset by Grid Interpolation%' OR coord_op_method_name IN ('NADCON', 'NTv1', 'NTv2', 'VERTCON'))")
    for (code, name, method_code, method_name, source_crs_code, target_crs_code, area_of_use_code, coord_op_accuracy, deprecated) in proj_db_cursor.fetchall():
        expected_order = 1
        max_n_params = 2
        param_auth_name = [None for i in range(max_n_params)]
        param_code = [None for i in range(max_n_params)]
        param_name = [None for i in range(max_n_params)]
        param_value = [None for i in range(max_n_params)]
        param_uom_code = [None for i in range(max_n_params)]

        iterator = proj_db_cursor.execute("SELECT sort_order, cop.parameter_code, parameter_name, parameter_value, param_value_file_ref, uom_code from epsg_coordoperationparam cop LEFT JOIN epsg_coordoperationparamvalue copv LEFT JOIN epsg_coordoperationparamusage copu ON cop.parameter_code = copv.parameter_code AND copu.parameter_code = copv.parameter_code WHERE copu.coord_op_method_code = copv.coord_op_method_code AND coord_op_code = ? AND copv.coord_op_method_code = ? ORDER BY sort_order", (code, method_code))
        for (order, parameter_code, parameter_name, parameter_value, param_value_file_ref, uom_code) in iterator:
            assert order <= max_n_params
            assert order == expected_order
            if parameter_value is not None:
                assert param_value_file_ref is None or len(param_value_file_ref) == 0, (order, parameter_code, parameter_name, parameter_value, param_value_file_ref, uom_code)
            if param_value_file_ref is not None and len(param_value_file_ref) != 0:
                assert parameter_value is None, (order, parameter_code, parameter_name, parameter_value, param_value_file_ref, uom_code)
            param_auth_name[order - 1] = EPSG_AUTHORITY
            param_code[order - 1] = parameter_code
            param_name[order - 1] = parameter_name
            param_value[order - 1] = parameter_value if parameter_value else param_value_file_ref
            param_uom_code[order - 1] = uom_code
            expected_order += 1
        n_params = expected_order - 1

        assert param_code[0] in (1050, 8656, 8657, 8666, 8732), (code, param_code[0])

        grid2_param_auth_name = None
        grid2_param_code = None
        grid2_value = None
        interpolation_crs_auth_name = None
        interpolation_crs_code = None

        if method_code == 9613: # NADCON
            assert param_code[1] == 8658, param_code[1]
            grid2_param_auth_name = EPSG_AUTHORITY
            grid2_param_code = param_code[1]
            grid2_value = param_value[1]
        elif method_code == 1071: # Vertical Offset by Grid Interpolation (NZLVD) 
            assert param_code[1] == 1048, param_code[1]
            interpolation_crs_auth_name = EPSG_AUTHORITY
            interpolation_crs_code = str(int(param_value[1])) # needed to avoid codes like XXXX.0
        else:
            assert n_params == 1, (code, method_code)


        arg = (EPSG_AUTHORITY, code, name,
               EPSG_AUTHORITY, method_code, method_name,
               EPSG_AUTHORITY, source_crs_code,
               EPSG_AUTHORITY, target_crs_code,
               EPSG_AUTHORITY, area_of_use_code,
               coord_op_accuracy,
               EPSG_AUTHORITY, param_code[0], param_value[0],
               grid2_param_auth_name, grid2_param_code, grid2_value,
               interpolation_crs_auth_name, interpolation_crs_code,
               deprecated
               )

        proj_db_cursor.execute("INSERT INTO coordinate_operation VALUES (?,?,'grid_transformation')", (EPSG_AUTHORITY, code))
        proj_db_cursor.execute('INSERT INTO grid_transformation VALUES (' +
            '?,?,?, ?,?,?, ?,?, ?,?, ?,?, ?, ?,?,?, ?,?,?, ?,?, ?)', arg)

def fill_concatenated_operation(proj_db_cursor):
    proj_db_cursor.execute("SELECT coord_op_code, coord_op_name, coord_op_method_code, coord_op_method_name, source_crs_code, target_crs_code, area_of_use_code, coord_op_accuracy, epsg_coordoperation.deprecated FROM epsg.epsg_coordoperation LEFT JOIN epsg.epsg_coordoperationmethod USING (coord_op_method_code) WHERE coord_op_type = 'concatenated operation'")
    for (code, name, method_code, method_name, source_crs_code, target_crs_code, area_of_use_code, coord_op_accuracy, deprecated) in proj_db_cursor.fetchall():
        expected_order = 1
        max_n_params = 3
        step_code = [None for i in range(max_n_params)]

        iterator = proj_db_cursor.execute("SELECT op_path_step, single_operation_code FROM epsg_coordoperationpath WHERE concat_operation_code = ? ORDER BY op_path_step", (code,))
        for (order, single_operation_code) in iterator:
            assert order <= max_n_params
            assert order == expected_order
            step_code[order - 1] = single_operation_code
            expected_order += 1
        n_params = expected_order - 1
        if n_params == 0:  # For example http://www.epsg-registry.org//export.htm?gml=urn:ogc:def:coordinateOperation:EPSG::8658
            continue
        assert n_params in (2, 3), (code, n_params)

        arg = (EPSG_AUTHORITY, code, name,
               EPSG_AUTHORITY, source_crs_code,
               EPSG_AUTHORITY, target_crs_code,
               EPSG_AUTHORITY, area_of_use_code,
               coord_op_accuracy,
               EPSG_AUTHORITY, step_code[0],
               EPSG_AUTHORITY, step_code[1],
               EPSG_AUTHORITY if step_code[2] else None, step_code[2],
               deprecated
               )

        proj_db_cursor.execute("INSERT INTO coordinate_operation VALUES (?,?,'concatenated_operation')", (EPSG_AUTHORITY, code))
        try:
            proj_db_cursor.execute('INSERT INTO concatenated_operation VALUES (' +
            '?,?,?, ?,?, ?,?, ?,?, ?, ?,?, ?,?, ?,?, ?)', arg)
        except Exception as e:
            print(e)
            print(arg)

epsg_db_conn, epsg_tmp_db_filename = ingest_epsg()

script_dir_name = os.path.dirname(os.path.realpath(__file__))
sql_dir_name = os.path.join(os.path.dirname(script_dir_name), 'data', 'sql')

proj_db_filename = ':memory:'
if os.path.exists(proj_db_filename):
    os.unlink(proj_db_filename)
proj_db_conn = sqlite3.connect(proj_db_filename)
proj_db_cursor = proj_db_conn.cursor()
proj_db_cursor.execute('PRAGMA foreign_keys = 1;')

ingest_sqlite_dump(proj_db_cursor, os.path.join(sql_dir_name, 'proj_db_table_defs.sql'))
proj_db_cursor.execute("ATTACH DATABASE '%s' AS epsg;" % epsg_tmp_db_filename)

fill_unit_of_measure(proj_db_cursor)
fill_ellipsoid(proj_db_cursor)
fill_area(proj_db_cursor)
fill_prime_meridian(proj_db_cursor)
fill_geodetic_datum(proj_db_cursor)
fill_vertical_datum(proj_db_cursor)
fill_coordinate_system(proj_db_cursor)
fill_axis(proj_db_cursor)
fill_geodetic_crs(proj_db_cursor)
fill_vertical_crs(proj_db_cursor)
fill_conversion(proj_db_cursor)
fill_projected_crs(proj_db_cursor)
fill_compound_crs(proj_db_cursor)
fill_helmert_transformation(proj_db_cursor)
fill_grid_transformation(proj_db_cursor)
fill_concatenated_operation(proj_db_cursor)

proj_db_cursor.close()
proj_db_conn.commit()

files = {}

# Dump the generated database and split it one .sql file per table
for line in proj_db_conn.iterdump():
    if line.startswith('INSERT INTO "'):
        table_name = line[len('INSERT INTO "'):]
        table_name = table_name[0:table_name.find('"')]
        if table_name in files:
            f = files[table_name]
        else:
            f = open(os.path.join(sql_dir_name, table_name) + '.sql', 'wb')
            f.write("--- This file has been generated by scripts/build_db.py. DO NOT EDIT !\n\n")
            files[table_name] = f
        f.write((line + '\n').encode('UTF-8'))
del files

proj_db_conn = None

epsg_db_conn = None
if os.path.exists(epsg_tmp_db_filename):
    os.unlink(epsg_tmp_db_filename)
