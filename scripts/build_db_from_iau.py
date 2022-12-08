#!/usr/bin/env python
# SPDX-License-Identifier: MIT
# Copyright (c) 2021, Hobu Inc
# Author: Even Rouault, <even.rouault@spatialys.com>

import csv
import os
from pathlib import Path

script_dir_name = os.path.dirname(os.path.realpath(__file__))
all_sql = []

AUTH_IAU2015 = 'IAU_2015'

all_sql.append("INSERT INTO coordinate_system VALUES('PROJ','OCENTRIC_LAT_LON','spherical',2);")
all_sql.append("INSERT INTO axis VALUES('PROJ','OCENTRIC_LAT_LON_LAT','Planetocentric latitude','U','north','PROJ','OCENTRIC_LAT_LON',1,'EPSG','9122');");
all_sql.append("INSERT INTO axis VALUES('PROJ','OCENTRIC_LAT_LON_LON','Planetocentric longitude','V','east','PROJ','OCENTRIC_LAT_LON',2,'EPSG','9122');");

all_sql.append("INSERT INTO coordinate_system VALUES('PROJ','OGRAPHIC_NORTH_WEST','ellipsoidal',2);")
all_sql.append("INSERT INTO axis VALUES('PROJ','OGRAPHIC_NORTH_WEST_LAT','Geodetic latitude','Lat','north','PROJ','OGRAPHIC_NORTH_WEST',1,'EPSG','9122');");
all_sql.append("INSERT INTO axis VALUES('PROJ','OGRAPHIC_NORTH_WEST_LON','Geodetic longitude','Lon','west','PROJ','OGRAPHIC_NORTH_WEST',2,'EPSG','9122');");

all_sql.append("INSERT INTO coordinate_system VALUES('PROJ','PROJECTED_WEST_NORTH','Cartesian',2);")
all_sql.append("INSERT INTO axis VALUES('PROJ','PROJECTED_WEST_NORTH_W','Westing','W','west','PROJ','PROJECTED_WEST_NORTH',1,'EPSG','9001');");
all_sql.append("INSERT INTO axis VALUES('PROJ','PROJECTED_WEST_NORTH_N','Northing','N','north','PROJ','PROJECTED_WEST_NORTH',2,'EPSG','9001');");


SOURCE_IAU = "Source of IAU Coordinate systems: https://doi.org/10.1007/s10569-017-9805-5"


def get_longitude_positive_direction(Body, Naif_id, rotation):
    """Define the positive longitudes in ographic CRS based on the rotation sens.
     The general rule is the following:
    * Direct rotation has longitude positive to West
    * Retrograde rotation has longitude positive to East
    A special case is done for Sun/Earth/Moon for historical reasons for which longitudes
    are positive to East independently of the rotation sens
    Another special case is that longitude ographic is always to East for small bodies, comets, dwarf planets (Naif_id >= 900)
    """
    
    if rotation == 'Direct':
        direction = 'west'
    elif rotation == 'Retrograde':
        direction = 'east'
    elif rotation == '':
        direction = ''
    else:
        assert False, rotation

    if Body in ('Sun', 'Moon', 'Earth'):
        direction = 'east'

    if Naif_id >= 900:
        direction = 'east'

    return direction


def add_usage(table_name, code):
    if table_name == 'geodetic_datum':
        prefix = 'GD'
    elif table_name == 'geodetic_crs':
        prefix = 'GCRS'
    elif table_name == 'projected_crs':
        prefix = 'PCRS'
    elif table_name == 'conversion':
        prefix = 'CONV'
    else:
        assert False
    all_sql.append("INSERT INTO usage VALUES('%s','%s_%d','%s','%s',%d,'PROJ','EXTENT_UNKNOWN','PROJ','SCOPE_UNKNOWN');" % (AUTH_IAU2015, prefix, code, table_name, AUTH_IAU2015, code))

projection_data = [
    [10, "Equirectangular, clon = 0", "Equidistant Cylindrical", "Latitude of 1st standard parallel", 0, "Longitude of natural origin", 0, "False easting", 0, "False northing", 0, None, None, None, None],
    [15, "Equirectangular, clon = 180", "Equidistant Cylindrical", "Latitude of 1st standard parallel", 0, "Longitude of natural origin", 180, "False easting", 0, "False northing", 0, None, None, None, None],
    [20, "Sinusoidal, clon = 0", "Sinusoidal",  "Longitude of natural origin", 0, "False easting", 0, "False northing", 0,None, None, None, None, None, None],
    [25, "Sinusoidal, clon = 180", "Sinusoidal", "Longitude of natural origin", 180, "False easting", 0, "False northing", 0, None, None, None, None, None, None],
    [30, "North Polar", "Polar Stereographic (variant A)", "Latitude of natural origin", 90, "Longitude of natural origin", 0, "Scale factor at natural origin", 1, "False easting", 0, "False northing", 0, None, None],
    [35, "South Polar", "Polar Stereographic (variant A)", "Latitude of natural origin", -90, "Longitude of natural origin", 0, "Scale factor at natural origin", 1, "False easting", 0, "False northing", 0, None, None],
    [40, "Mollweide, clon = 0", "Mollweide", "Longitude of natural origin", 0, "False easting", 0, "False northing", 0, None, None, None, None, None, None],
    [45, "Mollweide, clon = 180", "Mollweide", "Longitude of natural origin", 180, "False easting", 0, "False northing", 0, None, None, None, None, None, None],
    [50, "Robinson, clon = 0", "Robinson", "Longitude of natural origin", 0, "False easting", 0, "False northing", 0, None, None, None, None, None, None],
    [55, "Robinson, clon = 180", "Robinson", "Longitude of natural origin", 180, "False easting", 0, "False northing", 0, None, None, None, None, None, None],
    [60, "Tranverse Mercator", "Transverse Mercator", "Latitude of natural origin", 0, "Longitude of natural origin", 0, "Scale factor at natural origin", 1.0, "False easting", 0, "False northing", 0, None, None],           
    [65, "Orthographic, clon = 0", "Orthographic", "Latitude of natural origin", 0, "Longitude of natural origin", 0, "False easting", 0, "False northing", 0, None, None, None, None],
    [70, "Orthographic, clon = 180", "Orthographic", "Latitude of natural origin", 0, "Longitude of natural origin", 180, "False easting", 0, "False northing", 0, None, None, None, None],
    [75, "Lambert Conic Conformal", "Lambert Conic Conformal (2SP)", "Latitude of false origin", 40, "Longitude of false origin", 0, "Latitude of 1st standard parallel", 20, "Latitude of 2nd standard parallel", 60, "Easting at false origin", 0, "Northing at false origin", 0],
    [80, "Lambert Azimuthal Equal Area", "Lambert Azimuthal Equal Area", "Latitude of natural origin", 40, "Longitude of natural origin", 0, "False easting", 0, "False northing", 0,  None, None, None, None],
    [85, "Albers Equal Area", "Albers Equal Area", "Latitude of false origin", 40,"Longitude of false origin", 0,  "Latitude of 1st standard parallel", 20, "Latitude of 2nd standard parallel", 60, "Easting at false origin", 0, "Northing at false origin", 0],
]

method_and_param_mapping = {
    "Lambert Azimuthal Equal Area (Spherical)" : ["EPSG", 1027],
    "Equidistant Cylindrical" : ["EPSG", 1028],
    "Equidistant Cylindrical (Spherical)": ["EPSG", 1029],
    "Scale factor at natural origin": ["EPSG", 8805, "SCALEUNIT[\"unity\",1.0, ID[\"EPSG\", 9201]]"],
    "False easting": ["EPSG", 8806, "LENGTHUNIT[\"metre\", 1, ID[\"EPSG\", 9001]]"],
    "False northing": ["EPSG", 8807, "LENGTHUNIT[\"metre\", 1, ID[\"EPSG\", 9001]]"],
    "Latitude of natural origin": ["EPSG", 8801, "ANGLEUNIT[\"degree\", 0.017453292519943295, ID[\"EPSG\", 9122]]"],
    "Longitude of natural origin": ["EPSG", 8802, "ANGLEUNIT[\"degree\", 0.017453292519943295, ID[\"EPSG\", 9122]]"],
    "Latitude of false origin": ["EPSG", 8821, "ANGLEUNIT[\"degree\", 0.017453292519943295, ID[\"EPSG\", 9122]]"],    
    "Longitude of false origin": ["EPSG", 8822, "ANGLEUNIT[\"degree\", 0.017453292519943295, ID[\"EPSG\", 9122]]"],
    "Latitude of 1st standard parallel": ["EPSG", 8823, "ANGLEUNIT[\"degree\", 0.017453292519943295, ID[\"EPSG\", 9122]]"],
    "Latitude of 2nd standard parallel": ["EPSG", 8824, "ANGLEUNIT[\"degree\", 0.017453292519943295, ID[\"EPSG\", 9122]]"],
    "Easting at false origin": ["EPSG", 8826, "LENGTHUNIT[\"metre\", 1, ID[\"EPSG\", 9001]]"],
    "Northing at false origin": ["EPSG", 8827, "LENGTHUNIT[\"metre\", 1, ID[\"EPSG\", 9001]]"],    
    "Sinusoidal": ["PROJ", 'SINUSOIDAL'],
    "Robinson" : ["PROJ", 'ROBINSON'],
    "Mollweide" : ["PROJ", 'MOLLWEIDE'],
    "Transverse Mercator" : ["EPSG",9807],
    "Lambert Conic Conformal (2SP)": ["EPSG", 9802],
    "Polar Stereographic (variant A)": ["EPSG", 9810],
    "Lambert Azimuthal Equal Area": ["EPSG", 9820],
    "Albers Equal Area" : ["EPSG", 9822],
    "Orthographic" : ["EPSG", 9840]
}

def append_proj_paramater(name, value):
    if name is None:
        return ',NULL,NULL,NULL,NULL,NULL,NULL'

    sql = ',' + "'%s'" % method_and_param_mapping[name][0]
    sql += ',' + "'%s'" %str(method_and_param_mapping[name][1])
    sql += ',' + "'%s'" % name
    sql += ',' + str(value)
    sql += ",'EPSG'"
    if 'unity' in method_and_param_mapping[name][2]:
        sql += ',9201'
    elif 'degree' in method_and_param_mapping[name][2]:
        sql += ',9122'
    elif 'metre' in method_and_param_mapping[name][2]:
        sql += ',9001'
    else:
        assert False
    return sql

for row in projection_data:
    conv_code, conv_name, method_name, p1n, p1v, p2n, p2v, p3n, p3v, p4n, p4v, p5n, p5v, p6n, p6v = row
    method_auth = method_and_param_mapping[method_name][0]
    method_code = str(method_and_param_mapping[method_name][1])
    sql = "INSERT INTO conversion VALUES ('%s',%d,'%s','','%s','%s','%s'" % (AUTH_IAU2015, conv_code, conv_name, method_auth, method_code, method_name)
    sql += append_proj_paramater(p1n, p1v)
    sql += append_proj_paramater(p2n, p2v)
    sql += append_proj_paramater(p3n, p3v)
    sql += append_proj_paramater(p4n, p4v)
    sql += append_proj_paramater(p5n, p5v)
    sql += append_proj_paramater(p6n, p6v)
    sql += append_proj_paramater(None, None)
    sql += ",0);"
    all_sql.append(sql)
    add_usage('conversion', conv_code)


def generate_projected_crs(geod_crs_code, geod_crs_name, is_west):

    for row in projection_data:
        conv_code, conv_name, method_name, p1n, p1v, p2n, p2v, p3n, p3v, p4n, p4v, p5n, p5v, p6n, p6v = row
        pcrs_code = geod_crs_code + conv_code
        pcrs_name = geod_crs_name + ' / ' + conv_name

        if is_west:
            cs_auth = 'PROJ'
            cs_code = 'PROJECTED_WEST_NORTH'
        else:
            cs_auth = 'EPSG'
            cs_code = '4400'

        all_sql.append("INSERT INTO projected_crs VALUES('%s',%d,'%s',NULL,'%s','%s','%s',%d,'%s',%d,NULL,0);" % (AUTH_IAU2015, pcrs_code, pcrs_name, cs_auth, cs_code, AUTH_IAU2015, geod_crs_code, AUTH_IAU2015, conv_code))
        add_usage('projected_crs', pcrs_code)


with open(Path(script_dir_name) / 'data/naifcodes_radii_m_wAsteroids_IAU2015.csv', 'rt') as csvfile:
    reader = csv.reader(csvfile)
    header = next(reader)
    assert header == ['Naif_id', 'Body', 'IAU2015_Mean', 'IAU2015_Semimajor', 'IAU2015_Axisb', 'IAU2015_Semiminor', 'rotation', 'origin_long_name', 'origin_lon_pos']
    nfields = len(header)

    while True:
        try:
            row = next(reader)
        except StopIteration:
            break
        assert len(row) == nfields, row

        Naif_id, Body, IAU2015_Mean, IAU2015_Semimajor, IAU2015_Axisb, IAU2015_Semiminor, rotation, origin_long_name, origin_lon_pos = row
        Naif_id = int(Naif_id)
        IAU2015_Mean = float(IAU2015_Mean)
        IAU2015_Semimajor = float(IAU2015_Semimajor)
        IAU2015_Axisb = float(IAU2015_Axisb)
        IAU2015_Semiminor = float(IAU2015_Semiminor)

        all_axis_known_and_consistent = True
        if IAU2015_Mean <= 0:
            if IAU2015_Semimajor <= 0 or IAU2015_Axisb <= 0 or IAU2015_Semiminor <= 0:
                reason = 'Skip %s has it lacks all axis information' % Body
                print(reason)
                all_sql.append('-- ' + reason)
                continue
        elif IAU2015_Semimajor <= 0 or IAU2015_Axisb <= 0 or IAU2015_Semiminor <= 0:
            all_sql.append('-- %s lacks value for semimajor and/or axisb and/or semiminor. Only consider its mean value.' % Body)
            all_axis_known_and_consistent = False
        elif IAU2015_Semimajor < IAU2015_Axisb or IAU2015_Axisb < IAU2015_Semiminor:
            all_sql.append('-- %s has inconsistent values: semimajor < axisb or axis < semiminor. Only consider its mean value.' % Body)
            all_axis_known_and_consistent = False

        if all_axis_known_and_consistent:
            is_spherical = IAU2015_Semimajor == IAU2015_Axisb and IAU2015_Axisb == IAU2015_Semiminor
            is_triaxial = IAU2015_Semimajor != IAU2015_Axisb and IAU2015_Semiminor != IAU2015_Axisb
        else:
            is_spherical = False
            is_triaxial = True

        sphere_remark = ''
        if is_triaxial:
            if IAU2015_Mean <= 0:
                assert IAU2015_Semimajor > 0
                assert IAU2015_Axisb > 0
                assert IAU2015_Semiminor > 0
                # Use R_m = (a+b+c)/3 as mean radius when mean radius is not defined ( = -1)
                sphere_radius = (IAU2015_Semimajor + IAU2015_Axisb + IAU2015_Semiminor) / 3
                sphere_remark = "Use R_m = (a+b+c)/3 as mean radius. "
            else:
                sphere_radius = IAU2015_Mean
            sphere_remark += "Use mean radius as sphere radius for interoperability. "
        elif not is_spherical: # biaxial case
            assert IAU2015_Semimajor > 0
            sphere_radius = IAU2015_Semimajor
            sphere_remark = "Use semi-major radius as sphere for interoperability. "
        else:
            assert IAU2015_Mean == IAU2015_Semimajor
            assert IAU2015_Semimajor > 0
            sphere_radius = IAU2015_Mean
        sphere_remark += SOURCE_IAU

        all_sql.append("INSERT INTO celestial_body VALUES('%s', %d, '%s', %f);" % (AUTH_IAU2015, Naif_id, Body, sphere_radius))

        prime_meridian_code = Naif_id * 100
        all_sql.append("INSERT INTO prime_meridian VALUES('%s', %d, 'Reference Meridian', 0.0, 'EPSG', 9102, 0);" % (AUTH_IAU2015, prime_meridian_code))

        anchor = 'NULL'
        if origin_long_name:
            if '.' not in origin_lon_pos:
                origin_lon_pos += '.0'
            anchor = "'%s: %s'" % (origin_long_name, origin_lon_pos)

        spherical_ellipsoid_code = Naif_id * 100
        spherical_ellipsoid_name = '%s (2015) - Sphere' % Body
        all_sql.append("INSERT INTO ellipsoid VALUES('%s',%d,'%s',NULL,'%s',%d,%f,'EPSG','9001',NULL,%f,0);" % (AUTH_IAU2015, spherical_ellipsoid_code, spherical_ellipsoid_name, AUTH_IAU2015, Naif_id, sphere_radius, sphere_radius))

        spherical_datum_code = spherical_ellipsoid_code
        spherical_datum_name = spherical_ellipsoid_name
        all_sql.append("INSERT INTO geodetic_datum VALUES('%s',%d,'%s','','%s',%d,'%s',%d,NULL,NULL,NULL,%s,0);" % (AUTH_IAU2015, spherical_datum_code, spherical_datum_name, AUTH_IAU2015, spherical_ellipsoid_code, AUTH_IAU2015, prime_meridian_code, anchor))
        add_usage('geodetic_datum', spherical_datum_code)

        spherical_crs_code = Naif_id * 100
        spherical_crs_name = '%s / Ocentric' % spherical_datum_name
        all_sql.append("INSERT INTO geodetic_crs VALUES('%s',%d,'%s','%s','geographic 2D','EPSG','6422','%s',%d,NULL,0);" % (AUTH_IAU2015, spherical_crs_code, spherical_crs_name, sphere_remark, AUTH_IAU2015, spherical_datum_code))
        add_usage('geodetic_crs', spherical_crs_code)

        generate_projected_crs(spherical_crs_code, spherical_crs_name, False)

        if not all_axis_known_and_consistent:
            continue

        # Sun and Moon have only the Sphere Ocentric description
        if Body in ('Sun', 'Moon'):
            continue

        # Unsupported by PROJ
        if is_triaxial:
            continue

        direction = get_longitude_positive_direction(Body, Naif_id, rotation)
        has_ographic = direction != ''
        has_ocentric = True

        if is_spherical:
            # The ocentric CRS will be identical to the spherical ocentric one on a sphere
            has_ocentric = False

            # and if the ographic one is east oriented, it will also be redundant
            if direction == 'east':
                has_ographic = False

        if not has_ographic and not has_ocentric:
            continue

        ellipsoidal_ellipsoid_code = Naif_id * 100 + 1
        ellipsoidal_ellipsoid_name = '%s (2015)' % Body
        all_sql.append("INSERT INTO ellipsoid VALUES('%s',%d,'%s',NULL,'%s',%d,%f,'EPSG','9001',NULL,%f,0);" % (AUTH_IAU2015, ellipsoidal_ellipsoid_code, ellipsoidal_ellipsoid_name, AUTH_IAU2015, Naif_id, IAU2015_Semimajor, IAU2015_Semiminor))

        ellipsoidal_datum_code = ellipsoidal_ellipsoid_code
        ellipsoidal_datum_name = ellipsoidal_ellipsoid_name
        all_sql.append("INSERT INTO geodetic_datum VALUES('%s',%d,'%s','','%s',%d,'%s',%d,NULL,NULL,NULL,%s,0);" % (AUTH_IAU2015, ellipsoidal_datum_code, ellipsoidal_datum_name, AUTH_IAU2015, ellipsoidal_ellipsoid_code, AUTH_IAU2015, prime_meridian_code, anchor))
        add_usage('geodetic_datum', ellipsoidal_datum_code)

        if has_ographic:
            # ographic
            crs_code = Naif_id * 100 + 1
            crs_name = '%s / Ographic' % ellipsoidal_datum_name

            if direction == 'west':
                cs_auth = 'PROJ'
                cs_code = 'OGRAPHIC_NORTH_WEST'
            else:
                assert direction == 'east'
                cs_auth = 'EPSG'
                cs_code = '6422'

            all_sql.append("INSERT INTO geodetic_crs VALUES('%s',%d,'%s','%s','geographic 2D','%s','%s','%s',%d,NULL,0);" % (AUTH_IAU2015,crs_code, crs_name, SOURCE_IAU, cs_auth, cs_code, AUTH_IAU2015, ellipsoidal_datum_code))
            add_usage('geodetic_crs', crs_code)

            generate_projected_crs(crs_code, crs_name, direction == 'west')

        if has_ocentric:
            # ocentric
            crs_code = Naif_id * 100 + 2
            crs_name = '%s / Ocentric' % ellipsoidal_datum_name

            cs_auth = 'PROJ'
            cs_code = 'OCENTRIC_LAT_LON'

            all_sql.append("INSERT INTO geodetic_crs VALUES('%s',%d,'%s','%s','other','%s','%s','%s',%d,NULL,0);" % (AUTH_IAU2015, crs_code, crs_name, SOURCE_IAU, cs_auth, cs_code, AUTH_IAU2015, ellipsoidal_datum_code))
            add_usage('geodetic_crs', crs_code)

            if not (has_ographic and direction == 'east'):
                # We don't need to generate projected CRS based on the ocentric one, if we have
                # generated them for the ographic CRS and that it is east oriented
                generate_projected_crs(crs_code, crs_name, False)


sql_dir_name = os.path.join(os.path.dirname(script_dir_name), 'data', 'sql')
f = open(os.path.join(sql_dir_name, 'iau') + '.sql', 'wb')
f.write("--- This file has been generated by scripts/build_db_from_iau.py. DO NOT EDIT !\n\n".encode('UTF-8'))
for sql in all_sql:
    f.write((sql + '\n').encode('UTF-8'))
f.close()

print('')
print('Finished !')
