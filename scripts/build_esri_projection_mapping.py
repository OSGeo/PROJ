#!/usr/bin/env python
###############################################################################
# $Id$
#
#  Project:  PROJ
#  Purpose:  Generate mappings between ESRI projection names and parameters and
#            their EPSG equivalents.
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

import yaml

# Map methods from pe_list_projection.csv to WKT2 naming

config_str = """
- Plate_Carree:
    WKT2_name:
        - EPSG_NAME_METHOD_EQUIDISTANT_CYLINDRICAL
        - EPSG_NAME_METHOD_EQUIDISTANT_CYLINDRICAL_SPHERICAL
    Params:
        - False_Easting: EPSG_NAME_PARAMETER_FALSE_EASTING
        - False_Northing: EPSG_NAME_PARAMETER_FALSE_NORTHING
        - Central_Meridian: EPSG_NAME_PARAMETER_LONGITUDE_OF_NATURAL_ORIGIN
    Cond:
        - EPSG_NAME_PARAMETER_LATITUDE_1ST_STD_PARALLEL = 0

- Equidistant_Cylindrical:
    WKT2_name: EPSG_NAME_METHOD_EQUIDISTANT_CYLINDRICAL
    Params:
        - False_Easting: EPSG_NAME_PARAMETER_FALSE_EASTING
        - False_Northing: EPSG_NAME_PARAMETER_FALSE_NORTHING
        - Central_Meridian: EPSG_NAME_PARAMETER_LONGITUDE_OF_NATURAL_ORIGIN
        - Standard_Parallel_1: EPSG_NAME_PARAMETER_LATITUDE_1ST_STD_PARALLEL

- Miller_Cylindrical:
    WKT2_name: PROJ_WKT2_NAME_METHOD_MILLER_CYLINDRICAL
    Params:
        - False_Easting: EPSG_NAME_PARAMETER_FALSE_EASTING
        - False_Northing: EPSG_NAME_PARAMETER_FALSE_NORTHING
        - Central_Meridian: EPSG_NAME_PARAMETER_LONGITUDE_OF_NATURAL_ORIGIN

- Mercator: # Mercator 2SP
    WKT2_name: EPSG_NAME_METHOD_MERCATOR_VARIANT_B
    Params:
        - False_Easting: EPSG_NAME_PARAMETER_FALSE_EASTING
        - False_Northing: EPSG_NAME_PARAMETER_FALSE_NORTHING
        - Central_Meridian: EPSG_NAME_PARAMETER_LONGITUDE_OF_NATURAL_ORIGIN
        - Standard_Parallel_1: EPSG_NAME_PARAMETER_LATITUDE_1ST_STD_PARALLEL

- Gauss_Kruger:
    WKT2_name: EPSG_NAME_METHOD_TRANSVERSE_MERCATOR
    Params:
        - False_Easting: EPSG_NAME_PARAMETER_FALSE_EASTING
        - False_Northing: EPSG_NAME_PARAMETER_FALSE_NORTHING
        - Central_Meridian: EPSG_NAME_PARAMETER_LONGITUDE_OF_NATURAL_ORIGIN
        - Scale_Factor: EPSG_NAME_PARAMETER_SCALE_FACTOR_AT_NATURAL_ORIGIN
        - Latitude_Of_Origin: EPSG_NAME_PARAMETER_LATITUDE_OF_NATURAL_ORIGIN

- Transverse_Mercator:
    WKT2_name: EPSG_NAME_METHOD_TRANSVERSE_MERCATOR
    Params:
        - False_Easting: EPSG_NAME_PARAMETER_FALSE_EASTING
        - False_Northing: EPSG_NAME_PARAMETER_FALSE_NORTHING
        - Central_Meridian: EPSG_NAME_PARAMETER_LONGITUDE_OF_NATURAL_ORIGIN
        - Scale_Factor: EPSG_NAME_PARAMETER_SCALE_FACTOR_AT_NATURAL_ORIGIN
        - Latitude_Of_Origin: EPSG_NAME_PARAMETER_LATITUDE_OF_NATURAL_ORIGIN

- Transverse_Mercator_Complex: # This is likely PROJ etmerc method
    WKT2_name: EPSG_NAME_METHOD_TRANSVERSE_MERCATOR
    Params:
        - False_Easting: EPSG_NAME_PARAMETER_FALSE_EASTING
        - False_Northing: EPSG_NAME_PARAMETER_FALSE_NORTHING
        - Central_Meridian: EPSG_NAME_PARAMETER_LONGITUDE_OF_NATURAL_ORIGIN
        - Scale_Factor: EPSG_NAME_PARAMETER_SCALE_FACTOR_AT_NATURAL_ORIGIN
        - Latitude_Of_Origin: EPSG_NAME_PARAMETER_LATITUDE_OF_NATURAL_ORIGIN

- Albers:
    WKT2_name: EPSG_NAME_METHOD_ALBERS_EQUAL_AREA
    Params:
        - False_Easting: EPSG_NAME_PARAMETER_EASTING_FALSE_ORIGIN
        - False_Northing: EPSG_NAME_PARAMETER_NORTHING_FALSE_ORIGIN
        - Central_Meridian: EPSG_NAME_PARAMETER_LONGITUDE_FALSE_ORIGIN
        - Standard_Parallel_1: EPSG_NAME_PARAMETER_LATITUDE_1ST_STD_PARALLEL
        - Standard_Parallel_2: EPSG_NAME_PARAMETER_LATITUDE_2ND_STD_PARALLEL
        - Latitude_Of_Origin: EPSG_NAME_PARAMETER_LATITUDE_FALSE_ORIGIN

- Sinusoidal:
    WKT2_name: PROJ_WKT2_NAME_METHOD_SINUSOIDAL
    Params:
        - False_Easting: EPSG_NAME_PARAMETER_FALSE_EASTING
        - False_Northing: EPSG_NAME_PARAMETER_FALSE_NORTHING
        - Central_Meridian: EPSG_NAME_PARAMETER_LONGITUDE_OF_NATURAL_ORIGIN

- Mollweide:
    WKT2_name: PROJ_WKT2_NAME_METHOD_MOLLWEIDE
    Params:
        - False_Easting: EPSG_NAME_PARAMETER_FALSE_EASTING
        - False_Northing: EPSG_NAME_PARAMETER_FALSE_NORTHING
        - Central_Meridian: EPSG_NAME_PARAMETER_LONGITUDE_OF_NATURAL_ORIGIN

- Eckert_I:
    WKT2_name: PROJ_WKT2_NAME_METHOD_ECKERT_I
    Params:
        - False_Easting: EPSG_NAME_PARAMETER_FALSE_EASTING
        - False_Northing: EPSG_NAME_PARAMETER_FALSE_NORTHING
        - Central_Meridian: EPSG_NAME_PARAMETER_LONGITUDE_OF_NATURAL_ORIGIN

- Eckert_II:
    WKT2_name: PROJ_WKT2_NAME_METHOD_ECKERT_II
    Params:
        - False_Easting: EPSG_NAME_PARAMETER_FALSE_EASTING
        - False_Northing: EPSG_NAME_PARAMETER_FALSE_NORTHING
        - Central_Meridian: EPSG_NAME_PARAMETER_LONGITUDE_OF_NATURAL_ORIGIN

- Eckert_III:
    WKT2_name: PROJ_WKT2_NAME_METHOD_ECKERT_III
    Params:
        - False_Easting: EPSG_NAME_PARAMETER_FALSE_EASTING
        - False_Northing: EPSG_NAME_PARAMETER_FALSE_NORTHING
        - Central_Meridian: EPSG_NAME_PARAMETER_LONGITUDE_OF_NATURAL_ORIGIN

- Eckert_IV:
    WKT2_name: PROJ_WKT2_NAME_METHOD_ECKERT_IV
    Params:
        - False_Easting: EPSG_NAME_PARAMETER_FALSE_EASTING
        - False_Northing: EPSG_NAME_PARAMETER_FALSE_NORTHING
        - Central_Meridian: EPSG_NAME_PARAMETER_LONGITUDE_OF_NATURAL_ORIGIN

- Eckert_V:
    WKT2_name: PROJ_WKT2_NAME_METHOD_ECKERT_V
    Params:
        - False_Easting: EPSG_NAME_PARAMETER_FALSE_EASTING
        - False_Northing: EPSG_NAME_PARAMETER_FALSE_NORTHING
        - Central_Meridian: EPSG_NAME_PARAMETER_LONGITUDE_OF_NATURAL_ORIGIN

- Eckert_VI:
    WKT2_name: PROJ_WKT2_NAME_METHOD_ECKERT_VI
    Params:
        - False_Easting: EPSG_NAME_PARAMETER_FALSE_EASTING
        - False_Northing: EPSG_NAME_PARAMETER_FALSE_NORTHING
        - Central_Meridian: EPSG_NAME_PARAMETER_LONGITUDE_OF_NATURAL_ORIGIN

- Gall_Stereographic:
    WKT2_name: PROJ_WKT2_NAME_METHOD_GALL_STEREOGRAPHIC
    Params:
        - False_Easting: EPSG_NAME_PARAMETER_FALSE_EASTING
        - False_Northing: EPSG_NAME_PARAMETER_FALSE_NORTHING
        - Central_Meridian: EPSG_NAME_PARAMETER_LONGITUDE_OF_NATURAL_ORIGIN

- Behrmann:
    WKT2_name: EPSG_NAME_METHOD_LAMBERT_CYLINDRICAL_EQUAL_AREA
    Params:
        - False_Easting: EPSG_NAME_PARAMETER_FALSE_EASTING
        - False_Northing: EPSG_NAME_PARAMETER_FALSE_NORTHING
        - Central_Meridian:
            Name: EPSG_NAME_PARAMETER_LONGITUDE_OF_NATURAL_ORIGIN
            Default: 0.0
        - Standard_Parallel_1:
            Name: EPSG_NAME_PARAMETER_LATITUDE_1ST_STD_PARALLEL
            Default: 30.0

- Winkel_I:
    WKT2_name: "Winkel I"
    Params:
        - False_Easting: EPSG_NAME_PARAMETER_FALSE_EASTING
        - False_Northing: EPSG_NAME_PARAMETER_FALSE_NORTHING
        - Central_Meridian: EPSG_NAME_PARAMETER_LONGITUDE_OF_NATURAL_ORIGIN
        - Standard_Parallel_1: EPSG_NAME_PARAMETER_LATITUDE_1ST_STD_PARALLEL

- Winkel_II:
    WKT2_name: "Winkel II"
    Params:
        - False_Easting: EPSG_NAME_PARAMETER_FALSE_EASTING
        - False_Northing: EPSG_NAME_PARAMETER_FALSE_NORTHING
        - Central_Meridian: EPSG_NAME_PARAMETER_LONGITUDE_OF_NATURAL_ORIGIN
        - Standard_Parallel_1: EPSG_NAME_PARAMETER_LATITUDE_1ST_STD_PARALLEL

- Lambert_Conformal_Conic:
    -   WKT2_name: EPSG_NAME_METHOD_LAMBERT_CONIC_CONFORMAL_1SP
        Params:
            - False_Easting: EPSG_NAME_PARAMETER_FALSE_EASTING
            - False_Northing: EPSG_NAME_PARAMETER_FALSE_NORTHING
            - Central_Meridian: EPSG_NAME_PARAMETER_LONGITUDE_OF_NATURAL_ORIGIN
            - Standard_Parallel_1: EPSG_NAME_PARAMETER_LATITUDE_OF_NATURAL_ORIGIN
            - Scale_Factor: EPSG_NAME_PARAMETER_SCALE_FACTOR_AT_NATURAL_ORIGIN
            - Latitude_Of_Origin: EPSG_NAME_PARAMETER_LATITUDE_OF_NATURAL_ORIGIN # should be the same as Standard_Parallel_1

    -   WKT2_name: EPSG_NAME_METHOD_LAMBERT_CONIC_CONFORMAL_2SP
        Params:
            - False_Easting: EPSG_NAME_PARAMETER_EASTING_FALSE_ORIGIN
            - False_Northing: EPSG_NAME_PARAMETER_NORTHING_FALSE_ORIGIN
            - Central_Meridian: EPSG_NAME_PARAMETER_LONGITUDE_FALSE_ORIGIN
            - Standard_Parallel_1: EPSG_NAME_PARAMETER_LATITUDE_1ST_STD_PARALLEL
            - Standard_Parallel_2: EPSG_NAME_PARAMETER_LATITUDE_2ND_STD_PARALLEL
            - Latitude_Of_Origin: EPSG_NAME_PARAMETER_LATITUDE_FALSE_ORIGIN

    # From GDAL autotest 
    -   WKT2_name: EPSG_NAME_METHOD_LAMBERT_CONIC_CONFORMAL_2SP
        Params:
            - False_Easting: EPSG_NAME_PARAMETER_EASTING_FALSE_ORIGIN
            - False_Northing: EPSG_NAME_PARAMETER_NORTHING_FALSE_ORIGIN
            - Central_Meridian: EPSG_NAME_PARAMETER_LONGITUDE_FALSE_ORIGIN
            - Standard_Parallel_1: EPSG_NAME_PARAMETER_LATITUDE_1ST_STD_PARALLEL
            - Standard_Parallel_2: EPSG_NAME_PARAMETER_LATITUDE_2ND_STD_PARALLEL
            - Scale_Factor: 1.0
            - Latitude_Of_Origin: EPSG_NAME_PARAMETER_LATITUDE_FALSE_ORIGIN

    # Tempative mapping. Did not find any example
    -   WKT2_name: EPSG_NAME_METHOD_LAMBERT_CONIC_CONFORMAL_2SP_MICHIGAN
        Params:
            - False_Easting: EPSG_NAME_PARAMETER_EASTING_FALSE_ORIGIN
            - False_Northing: EPSG_NAME_PARAMETER_NORTHING_FALSE_ORIGIN
            - Central_Meridian: EPSG_NAME_PARAMETER_LONGITUDE_FALSE_ORIGIN
            - Standard_Parallel_1: EPSG_NAME_PARAMETER_LATITUDE_1ST_STD_PARALLEL
            - Standard_Parallel_2: EPSG_NAME_PARAMETER_LATITUDE_2ND_STD_PARALLEL
            - Scale_Factor: EPSG_NAME_PARAMETER_ELLIPSOID_SCALE_FACTOR
            - Latitude_Of_Origin: EPSG_NAME_PARAMETER_LATITUDE_FALSE_ORIGIN

- Polyconic:
    WKT2_name: EPSG_NAME_METHOD_AMERICAN_POLYCONIC
    Params:
        - False_Easting: EPSG_NAME_PARAMETER_FALSE_EASTING
        - False_Northing: EPSG_NAME_PARAMETER_FALSE_NORTHING
        - Central_Meridian: EPSG_NAME_PARAMETER_LONGITUDE_OF_NATURAL_ORIGIN
        - Latitude_Of_Origin: EPSG_NAME_PARAMETER_LATITUDE_OF_NATURAL_ORIGIN

- Quartic_Authalic:
    WKT2_name: "Quartic Authalic"
    Params:
        - False_Easting: EPSG_NAME_PARAMETER_FALSE_EASTING
        - False_Northing: EPSG_NAME_PARAMETER_FALSE_NORTHING
        - Central_Meridian: EPSG_NAME_PARAMETER_LONGITUDE_OF_NATURAL_ORIGIN

- Loximuthal:
    WKT2_name: "Loximuthal"
    Params:
        - False_Easting: EPSG_NAME_PARAMETER_FALSE_EASTING
        - False_Northing: EPSG_NAME_PARAMETER_FALSE_NORTHING
        - Central_Meridian: EPSG_NAME_PARAMETER_LONGITUDE_OF_NATURAL_ORIGIN
        - Central_Parallel: EPSG_NAME_PARAMETER_LATITUDE_OF_NATURAL_ORIGIN

- Bonne:
    WKT2_name: EPSG_NAME_METHOD_BONNE
    Params:
        - False_Easting: EPSG_NAME_PARAMETER_FALSE_EASTING
        - False_Northing: EPSG_NAME_PARAMETER_FALSE_NORTHING
        - Central_Meridian: EPSG_NAME_PARAMETER_LONGITUDE_OF_NATURAL_ORIGIN
        - Standard_Parallel_1: EPSG_NAME_PARAMETER_LATITUDE_OF_NATURAL_ORIGIN

- Hotine_Oblique_Mercator_Two_Point_Natural_Origin:
    WKT2_name: PROJ_WKT2_NAME_METHOD_HOTINE_OBLIQUE_MERCATOR_TWO_POINT_NATURAL_ORIGIN
    Params:
        - False_Easting: EPSG_NAME_PARAMETER_EASTING_PROJECTION_CENTRE
        - False_Northing: EPSG_NAME_PARAMETER_NORTHING_PROJECTION_CENTRE
        - Latitude_Of_1st_Point: "Latitude of 1st point"
        - Latitude_Of_2nd_Point: "Latitude of 2nd point"
        - Scale_Factor: EPSG_NAME_PARAMETER_SCALE_FACTOR_INITIAL_LINE
        - Longitude_Of_1st_Point: "Longitude of 1st point"
        - Longitude_Of_2nd_Point: "Longitude of 2nd point"
        - Latitude_Of_Center: EPSG_NAME_PARAMETER_LATITUDE_PROJECTION_CENTRE

- Stereographic:
    WKT2_name: PROJ_WKT2_NAME_METHOD_STEREOGRAPHIC
    Params:
        - False_Easting: EPSG_NAME_PARAMETER_FALSE_EASTING
        - False_Northing: EPSG_NAME_PARAMETER_FALSE_NORTHING
        - Central_Meridian: EPSG_NAME_PARAMETER_LONGITUDE_OF_NATURAL_ORIGIN
        - Scale_Factor: EPSG_NAME_PARAMETER_SCALE_FACTOR_AT_NATURAL_ORIGIN
        - Latitude_Of_Origin: EPSG_NAME_PARAMETER_LATITUDE_OF_NATURAL_ORIGIN

- Equidistant_Conic:
    WKT2_name: PROJ_WKT2_NAME_METHOD_EQUIDISTANT_CONIC
    Params:
        - False_Easting: EPSG_NAME_PARAMETER_FALSE_EASTING
        - False_Northing: EPSG_NAME_PARAMETER_FALSE_NORTHING
        - Central_Meridian: EPSG_NAME_PARAMETER_LONGITUDE_OF_NATURAL_ORIGIN
        - Standard_Parallel_1: EPSG_NAME_PARAMETER_LATITUDE_1ST_STD_PARALLEL
        - Standard_Parallel_2: EPSG_NAME_PARAMETER_LATITUDE_2ND_STD_PARALLEL
        - Latitude_Of_Origin: EPSG_NAME_PARAMETER_LATITUDE_OF_NATURAL_ORIGIN

- Cassini:
    WKT2_name: EPSG_NAME_METHOD_CASSINI_SOLDNER
    Params:
        - False_Easting: EPSG_NAME_PARAMETER_FALSE_EASTING
        - False_Northing: EPSG_NAME_PARAMETER_FALSE_NORTHING
        - Central_Meridian: EPSG_NAME_PARAMETER_LONGITUDE_OF_NATURAL_ORIGIN
        - Scale_Factor: 1.0 # fixed
        - Latitude_Of_Origin: EPSG_NAME_PARAMETER_LATITUDE_OF_NATURAL_ORIGIN

- Van_der_Grinten_I:
    WKT2_name: PROJ_WKT2_NAME_METHOD_VAN_DER_GRINTEN
    Params:
        - False_Easting: EPSG_NAME_PARAMETER_FALSE_EASTING
        - False_Northing: EPSG_NAME_PARAMETER_FALSE_NORTHING
        - Central_Meridian: EPSG_NAME_PARAMETER_LONGITUDE_OF_NATURAL_ORIGIN

- Robinson:
    WKT2_name: PROJ_WKT2_NAME_METHOD_ROBINSON
    Params:
        - False_Easting: EPSG_NAME_PARAMETER_FALSE_EASTING
        - False_Northing: EPSG_NAME_PARAMETER_FALSE_NORTHING
        - Central_Meridian: EPSG_NAME_PARAMETER_LONGITUDE_OF_NATURAL_ORIGIN

- Two_Point_Equidistant:
    WKT2_name: PROJ_WKT2_NAME_METHOD_TWO_POINT_EQUIDISTANT
    Params:
        - False_Easting: EPSG_NAME_PARAMETER_FALSE_EASTING
        - False_Northing: EPSG_NAME_PARAMETER_FALSE_NORTHING
        - Latitude_Of_1st_Point: "Latitude of 1st point"
        - Latitude_Of_2nd_Point: "Latitude of 2nd point"
        - Longitude_Of_1st_Point: "Longitude of 1st point"
        - Longitude_Of_2nd_Point: "Longitude of 2nd point"

- Azimuthal_Equidistant:
    WKT2_name: EPSG_NAME_METHOD_MODIFIED_AZIMUTHAL_EQUIDISTANT
    Params:
        - False_Easting: EPSG_NAME_PARAMETER_FALSE_EASTING
        - False_Northing: EPSG_NAME_PARAMETER_FALSE_NORTHING
        - Central_Meridian: EPSG_NAME_PARAMETER_LONGITUDE_OF_NATURAL_ORIGIN
        - Latitude_Of_Origin: EPSG_NAME_PARAMETER_LATITUDE_OF_NATURAL_ORIGIN

- Lambert_Azimuthal_Equal_Area:
    WKT2_name: EPSG_NAME_METHOD_LAMBERT_AZIMUTHAL_EQUAL_AREA
    Params:
        - False_Easting: EPSG_NAME_PARAMETER_FALSE_EASTING
        - False_Northing: EPSG_NAME_PARAMETER_FALSE_NORTHING
        - Central_Meridian: EPSG_NAME_PARAMETER_LONGITUDE_OF_NATURAL_ORIGIN
        - Latitude_Of_Origin: EPSG_NAME_PARAMETER_LATITUDE_OF_NATURAL_ORIGIN

- Cylindrical_Equal_Area:
    WKT2_name: EPSG_NAME_METHOD_LAMBERT_CYLINDRICAL_EQUAL_AREA
    Params:
        - False_Easting: EPSG_NAME_PARAMETER_FALSE_EASTING
        - False_Northing: EPSG_NAME_PARAMETER_FALSE_NORTHING
        - Central_Meridian: EPSG_NAME_PARAMETER_LONGITUDE_OF_NATURAL_ORIGIN
        - Standard_Parallel_1: EPSG_NAME_PARAMETER_LATITUDE_1ST_STD_PARALLEL

# No example in pe_list_projection.csv: temptative mapping !
- Hotine_Oblique_Mercator_Two_Point_Center:
    WKT2_name: PROJ_WKT2_NAME_METHOD_HOTINE_OBLIQUE_MERCATOR_TWO_POINT_NATURAL_ORIGIN
    Params:
        - False_Easting: EPSG_NAME_PARAMETER_EASTING_PROJECTION_CENTRE
        - False_Northing: EPSG_NAME_PARAMETER_NORTHING_PROJECTION_CENTRE
        - Latitude_Of_1st_Point: "Latitude of 1st point"
        - Latitude_Of_2nd_Point: "Latitude of 2nd point"
        - Scale_Factor: EPSG_NAME_PARAMETER_SCALE_FACTOR_INITIAL_LINE
        - Longitude_Of_1st_Point: "Longitude of 1st point"
        - Longitude_Of_2nd_Point: "Longitude of 2nd point"
        - Latitude_Of_Origin: EPSG_NAME_PARAMETER_LATITUDE_OF_NATURAL_ORIGIN

- Hotine_Oblique_Mercator_Azimuth_Natural_Origin:
    WKT2_name: EPSG_NAME_METHOD_HOTINE_OBLIQUE_MERCATOR_VARIANT_A
    Params:
        - False_Easting: EPSG_NAME_PARAMETER_FALSE_EASTING
        - False_Northing: EPSG_NAME_PARAMETER_FALSE_NORTHING
        - Scale_Factor: EPSG_NAME_PARAMETER_SCALE_FACTOR_INITIAL_LINE
        - Azimuth: EPSG_NAME_PARAMETER_AZIMUTH_INITIAL_LINE
        # No EPSG_NAME_PARAMETER_ANGLE_RECTIFIED_TO_SKEW_GRID
        - Longitude_Of_Center: EPSG_NAME_PARAMETER_LONGITUDE_PROJECTION_CENTRE
        - Latitude_Of_Center: EPSG_NAME_PARAMETER_LATITUDE_PROJECTION_CENTRE


- Hotine_Oblique_Mercator_Azimuth_Center:
    WKT2_name: EPSG_NAME_METHOD_HOTINE_OBLIQUE_MERCATOR_VARIANT_B
    Params:
        - False_Easting: EPSG_NAME_PARAMETER_EASTING_PROJECTION_CENTRE
        - False_Northing: EPSG_NAME_PARAMETER_NORTHING_PROJECTION_CENTRE
        - Scale_Factor: EPSG_NAME_PARAMETER_SCALE_FACTOR_INITIAL_LINE
        - Azimuth: EPSG_NAME_PARAMETER_AZIMUTH_INITIAL_LINE
        # No EPSG_NAME_PARAMETER_ANGLE_RECTIFIED_TO_SKEW_GRID
        - Longitude_Of_Center: EPSG_NAME_PARAMETER_LONGITUDE_PROJECTION_CENTRE
        - Latitude_Of_Center: EPSG_NAME_PARAMETER_LATITUDE_PROJECTION_CENTRE

- Double_Stereographic:
    WKT2_name: EPSG_NAME_METHOD_OBLIQUE_STEREOGRAPHIC
    Params:
        - False_Easting: EPSG_NAME_PARAMETER_FALSE_EASTING
        - False_Northing: EPSG_NAME_PARAMETER_FALSE_NORTHING
        - Central_Meridian: EPSG_NAME_PARAMETER_LONGITUDE_OF_NATURAL_ORIGIN
        - Scale_Factor: EPSG_NAME_PARAMETER_SCALE_FACTOR_AT_NATURAL_ORIGIN
        - Latitude_Of_Origin: EPSG_NAME_PARAMETER_LATITUDE_OF_NATURAL_ORIGIN

- Krovak:
    -   WKT2_name: EPSG_NAME_METHOD_KROVAK
        Params:
            - False_Easting: EPSG_NAME_PARAMETER_FALSE_EASTING
            - False_Northing: EPSG_NAME_PARAMETER_FALSE_NORTHING
            - Pseudo_Standard_Parallel_1: EPSG_NAME_PARAMETER_LATITUDE_PSEUDO_STANDARD_PARALLEL
            - Scale_Factor: EPSG_NAME_PARAMETER_SCALE_FACTOR_PSEUDO_STANDARD_PARALLEL
            - Azimuth: EPSG_NAME_PARAMETER_COLATITUDE_CONE_AXIS
            - Longitude_Of_Center: EPSG_NAME_PARAMETER_LONGITUDE_OF_ORIGIN
            - Latitude_Of_Center: EPSG_NAME_PARAMETER_LATITUDE_PROJECTION_CENTRE
            - X_Scale: 1.0
            - Y_Scale: 1.0
            - XY_Plane_Rotation: 0.0

    -   WKT2_name: EPSG_NAME_METHOD_KROVAK_NORTH_ORIENTED
        Params:
            - False_Easting: EPSG_NAME_PARAMETER_FALSE_EASTING
            - False_Northing: EPSG_NAME_PARAMETER_FALSE_NORTHING
            - Pseudo_Standard_Parallel_1: EPSG_NAME_PARAMETER_LATITUDE_PSEUDO_STANDARD_PARALLEL
            - Scale_Factor: EPSG_NAME_PARAMETER_SCALE_FACTOR_PSEUDO_STANDARD_PARALLEL
            - Azimuth: EPSG_NAME_PARAMETER_COLATITUDE_CONE_AXIS
            - Longitude_Of_Center: EPSG_NAME_PARAMETER_LONGITUDE_OF_ORIGIN
            - Latitude_Of_Center: EPSG_NAME_PARAMETER_LATITUDE_PROJECTION_CENTRE
            - X_Scale: -1.0
            - Y_Scale: 1.0
            - XY_Plane_Rotation: 90.0

- New_Zealand_Map_Grid:
    WKT2_name: EPSG_NAME_METHOD_NZMG
    Params:
        - False_Easting: EPSG_NAME_PARAMETER_FALSE_EASTING
        - False_Northing: EPSG_NAME_PARAMETER_FALSE_NORTHING
        - Longitude_Of_Origin: EPSG_NAME_PARAMETER_LONGITUDE_OF_NATURAL_ORIGIN
        - Latitude_Of_Origin: EPSG_NAME_PARAMETER_LATITUDE_OF_NATURAL_ORIGIN

# ESRI's Orthographic is a spherical-only formulation. The ellipsoidal capable
# name is Local. See below
- Orthographic:
    WKT2_name: PROJ_WKT2_NAME_ORTHOGRAPHIC_SPHERICAL
    Params:
        - False_Easting: EPSG_NAME_PARAMETER_FALSE_EASTING
        - False_Northing: EPSG_NAME_PARAMETER_FALSE_NORTHING
        - Longitude_Of_Center: EPSG_NAME_PARAMETER_LONGITUDE_OF_NATURAL_ORIGIN
        - Latitude_Of_Center: EPSG_NAME_PARAMETER_LATITUDE_OF_NATURAL_ORIGIN

- Local:
    WKT2_name: EPSG_NAME_METHOD_ORTHOGRAPHIC
    Params:
        - False_Easting: EPSG_NAME_PARAMETER_FALSE_EASTING
        - False_Northing: EPSG_NAME_PARAMETER_FALSE_NORTHING
        - Scale_Factor: 1.0
        - Azimuth: 0.0
        - Longitude_Of_Center: EPSG_NAME_PARAMETER_LONGITUDE_OF_NATURAL_ORIGIN
        - Latitude_Of_Center: EPSG_NAME_PARAMETER_LATITUDE_OF_NATURAL_ORIGIN

- Winkel_Tripel:
    WKT2_name: "Winkel Tripel"
    Params:
        - False_Easting: EPSG_NAME_PARAMETER_FALSE_EASTING
        - False_Northing: EPSG_NAME_PARAMETER_FALSE_NORTHING
        - Central_Meridian: EPSG_NAME_PARAMETER_LONGITUDE_OF_NATURAL_ORIGIN
        - Standard_Parallel_1: EPSG_NAME_PARAMETER_LATITUDE_1ST_STD_PARALLEL

- Aitoff:
    WKT2_name: "Aitoff"
    Params:
        - False_Easting: EPSG_NAME_PARAMETER_FALSE_EASTING
        - False_Northing: EPSG_NAME_PARAMETER_FALSE_NORTHING
        - Central_Meridian: EPSG_NAME_PARAMETER_LONGITUDE_OF_NATURAL_ORIGIN

- Flat_Polar_Quartic:
    WKT2_name: PROJ_WKT2_NAME_METHOD_FLAT_POLAR_QUARTIC
    Params:
        - False_Easting: EPSG_NAME_PARAMETER_FALSE_EASTING
        - False_Northing: EPSG_NAME_PARAMETER_FALSE_NORTHING
        - Central_Meridian: EPSG_NAME_PARAMETER_LONGITUDE_OF_NATURAL_ORIGIN

- Craster_Parabolic:
    WKT2_name: "Craster Parabolic"
    Params:
        - False_Easting: EPSG_NAME_PARAMETER_FALSE_EASTING
        - False_Northing: EPSG_NAME_PARAMETER_FALSE_NORTHING
        - Central_Meridian: EPSG_NAME_PARAMETER_LONGITUDE_OF_NATURAL_ORIGIN

- Gnomonic:
    WKT2_name: PROJ_WKT2_NAME_METHOD_GNOMONIC
    Params:
        - False_Easting: EPSG_NAME_PARAMETER_FALSE_EASTING
        - False_Northing: EPSG_NAME_PARAMETER_FALSE_NORTHING
        - Longitude_Of_Center: EPSG_NAME_PARAMETER_LONGITUDE_OF_NATURAL_ORIGIN
        - Latitude_Of_Center: EPSG_NAME_PARAMETER_LATITUDE_OF_NATURAL_ORIGIN

- Times:
    WKT2_name: PROJ_WKT2_NAME_METHOD_TIMES
    Params:
        - False_Easting: EPSG_NAME_PARAMETER_FALSE_EASTING
        - False_Northing: EPSG_NAME_PARAMETER_FALSE_NORTHING
        - Central_Meridian: EPSG_NAME_PARAMETER_LONGITUDE_OF_NATURAL_ORIGIN

- Vertical_Near_Side_Perspective:
    WKT2_name: EPSG_NAME_METHOD_VERTICAL_PERSPECTIVE
    Params:
        - False_Easting: EPSG_NAME_PARAMETER_FALSE_EASTING
        - False_Northing: EPSG_NAME_PARAMETER_FALSE_NORTHING
        - Longitude_Of_Center: EPSG_NAME_PARAMETER_LONGITUDE_TOPOGRAPHIC_ORIGIN
        - Latitude_Of_Center: EPSG_NAME_PARAMETER_LATITUDE_TOPOGRAPHIC_ORIGIN
        - Height: EPSG_NAME_PARAMETER_VIEWPOINT_HEIGHT

- Stereographic_North_Pole:
    WKT2_name: EPSG_NAME_METHOD_POLAR_STEREOGRAPHIC_VARIANT_B
    Params:
        - False_Easting: EPSG_NAME_PARAMETER_FALSE_EASTING
        - False_Northing: EPSG_NAME_PARAMETER_FALSE_NORTHING
        - Central_Meridian: EPSG_NAME_PARAMETER_LONGITUDE_OF_ORIGIN
        - Standard_Parallel_1: EPSG_NAME_PARAMETER_LATITUDE_STD_PARALLEL
    Cond:
        - EPSG_NAME_PARAMETER_LATITUDE_STD_PARALLEL > 0

- Stereographic_South_Pole:
    WKT2_name: EPSG_NAME_METHOD_POLAR_STEREOGRAPHIC_VARIANT_B
    Params:
        - False_Easting: EPSG_NAME_PARAMETER_FALSE_EASTING
        - False_Northing: EPSG_NAME_PARAMETER_FALSE_NORTHING
        - Central_Meridian: EPSG_NAME_PARAMETER_LONGITUDE_OF_ORIGIN
        - Standard_Parallel_1: EPSG_NAME_PARAMETER_LATITUDE_STD_PARALLEL
    Cond:
        - EPSG_NAME_PARAMETER_LATITUDE_STD_PARALLEL < 0

- Rectified_Skew_Orthomorphic_Natural_Origin:
    WKT2_name: EPSG_NAME_METHOD_HOTINE_OBLIQUE_MERCATOR_VARIANT_A
    Params:
        - False_Easting: EPSG_NAME_PARAMETER_FALSE_EASTING
        - False_Northing: EPSG_NAME_PARAMETER_FALSE_NORTHING
        - Scale_Factor: EPSG_NAME_PARAMETER_SCALE_FACTOR_INITIAL_LINE
        - Azimuth: EPSG_NAME_PARAMETER_AZIMUTH_INITIAL_LINE
        - Longitude_Of_Center: EPSG_NAME_PARAMETER_LONGITUDE_PROJECTION_CENTRE
        - Latitude_Of_Center: EPSG_NAME_PARAMETER_LATITUDE_PROJECTION_CENTRE
        - XY_Plane_Rotation: EPSG_NAME_PARAMETER_ANGLE_RECTIFIED_TO_SKEW_GRID

# temptative mapping: no example
- Rectified_Skew_Orthomorphic_Center:
    WKT2_name: EPSG_NAME_METHOD_HOTINE_OBLIQUE_MERCATOR_VARIANT_B
    Params:
        - False_Easting: EPSG_NAME_PARAMETER_EASTING_PROJECTION_CENTRE
        - False_Northing: EPSG_NAME_PARAMETER_NORTHING_PROJECTION_CENTRE
        - Scale_Factor: EPSG_NAME_PARAMETER_SCALE_FACTOR_INITIAL_LINE
        - Azimuth: EPSG_NAME_PARAMETER_AZIMUTH_INITIAL_LINE
        - Longitude_Of_Center: EPSG_NAME_PARAMETER_LONGITUDE_PROJECTION_CENTRE
        - Latitude_Of_Center: EPSG_NAME_PARAMETER_LATITUDE_PROJECTION_CENTRE
        - XY_Plane_Rotation: EPSG_NAME_PARAMETER_ANGLE_RECTIFIED_TO_SKEW_GRID

- Goode_Homolosine:
    - WKT2_name: PROJ_WKT2_NAME_METHOD_INTERRUPTED_GOODE_HOMOLOSINE
      Params:
        - False_Easting: EPSG_NAME_PARAMETER_FALSE_EASTING
        - False_Northing: EPSG_NAME_PARAMETER_FALSE_NORTHING
        - Central_Meridian: EPSG_NAME_PARAMETER_LONGITUDE_OF_NATURAL_ORIGIN
        - Option: 1.0

    - WKT2_name: PROJ_WKT2_NAME_METHOD_INTERRUPTED_GOODE_HOMOLOSINE_OCEAN
      Params:
        - False_Easting: EPSG_NAME_PARAMETER_FALSE_EASTING
        - False_Northing: EPSG_NAME_PARAMETER_FALSE_NORTHING
        - Central_Meridian: EPSG_NAME_PARAMETER_LONGITUDE_OF_NATURAL_ORIGIN
        - Option: 2.0

- Equidistant_Cylindrical_Ellipsoidal:
    WKT2_name: EPSG_NAME_METHOD_EQUIDISTANT_CYLINDRICAL
    Params:
        - False_Easting: EPSG_NAME_PARAMETER_FALSE_EASTING
        - False_Northing: EPSG_NAME_PARAMETER_FALSE_NORTHING
        - Central_Meridian: EPSG_NAME_PARAMETER_LONGITUDE_OF_NATURAL_ORIGIN
        - Standard_Parallel_1: EPSG_NAME_PARAMETER_LATITUDE_1ST_STD_PARALLEL

- Laborde_Oblique_Mercator:
    WKT2_name: EPSG_NAME_METHOD_LABORDE_OBLIQUE_MERCATOR
    Params:
        - False_Easting: EPSG_NAME_PARAMETER_FALSE_EASTING
        - False_Northing: EPSG_NAME_PARAMETER_FALSE_NORTHING
        - Scale_Factor: EPSG_NAME_PARAMETER_SCALE_FACTOR_INITIAL_LINE
        - Azimuth: EPSG_NAME_PARAMETER_AZIMUTH_INITIAL_LINE
        - Longitude_Of_Center: EPSG_NAME_PARAMETER_LONGITUDE_PROJECTION_CENTRE
        - Latitude_Of_Center: EPSG_NAME_PARAMETER_LATITUDE_PROJECTION_CENTRE

- Gnomonic_Ellipsoidal:
    WKT2_name: PROJ_WKT2_NAME_METHOD_GNOMONIC
    Params:
        - False_Easting: EPSG_NAME_PARAMETER_FALSE_EASTING
        - False_Northing: EPSG_NAME_PARAMETER_FALSE_NORTHING
        - Longitude_Of_Center: EPSG_NAME_PARAMETER_LONGITUDE_OF_NATURAL_ORIGIN
        - Latitude_Of_Center: EPSG_NAME_PARAMETER_LATITUDE_OF_NATURAL_ORIGIN

- Wagner_IV:
    WKT2_name: PROJ_WKT2_NAME_METHOD_WAGNER_IV
    Params:
        - False_Easting: EPSG_NAME_PARAMETER_FALSE_EASTING
        - False_Northing: EPSG_NAME_PARAMETER_FALSE_NORTHING
        - Central_Meridian: EPSG_NAME_PARAMETER_LONGITUDE_OF_NATURAL_ORIGIN
        - Latitude_Of_Origin: 0

- Wagner_V:
    WKT2_name: PROJ_WKT2_NAME_METHOD_WAGNER_V
    Params:
        - False_Easting: EPSG_NAME_PARAMETER_FALSE_EASTING
        - False_Northing: EPSG_NAME_PARAMETER_FALSE_NORTHING
        - Central_Meridian: EPSG_NAME_PARAMETER_LONGITUDE_OF_NATURAL_ORIGIN

- Wagner_VII:
    WKT2_name: PROJ_WKT2_NAME_METHOD_WAGNER_VII
    Params:
        - False_Easting: EPSG_NAME_PARAMETER_FALSE_EASTING
        - False_Northing: EPSG_NAME_PARAMETER_FALSE_NORTHING
        - Central_Meridian: EPSG_NAME_PARAMETER_LONGITUDE_OF_NATURAL_ORIGIN

- Natural_Earth:
    WKT2_name: PROJ_WKT2_NAME_METHOD_NATURAL_EARTH
    Params:
        - False_Easting: EPSG_NAME_PARAMETER_FALSE_EASTING
        - False_Northing: EPSG_NAME_PARAMETER_FALSE_NORTHING
        - Central_Meridian: EPSG_NAME_PARAMETER_LONGITUDE_OF_NATURAL_ORIGIN

- Natural_Earth_II:
    WKT2_name: PROJ_WKT2_NAME_METHOD_NATURAL_EARTH_II
    Params:
        - False_Easting: EPSG_NAME_PARAMETER_FALSE_EASTING
        - False_Northing: EPSG_NAME_PARAMETER_FALSE_NORTHING
        - Central_Meridian: EPSG_NAME_PARAMETER_LONGITUDE_OF_NATURAL_ORIGIN

- Patterson:
    WKT2_name: PROJ_WKT2_NAME_METHOD_PATTERSON
    Params:
        - False_Easting: EPSG_NAME_PARAMETER_FALSE_EASTING
        - False_Northing: EPSG_NAME_PARAMETER_FALSE_NORTHING
        - Central_Meridian: EPSG_NAME_PARAMETER_LONGITUDE_OF_NATURAL_ORIGIN

- Compact_Miller:
    WKT2_name: PROJ_WKT2_NAME_METHOD_COMPACT_MILLER
    Params:
        - False_Easting: EPSG_NAME_PARAMETER_FALSE_EASTING
        - False_Northing: EPSG_NAME_PARAMETER_FALSE_NORTHING
        - Central_Meridian: EPSG_NAME_PARAMETER_LONGITUDE_OF_NATURAL_ORIGIN

- Geostationary_Satellite:
    WKT2_name: PROJ_WKT2_NAME_METHOD_GEOSTATIONARY_SATELLITE_SWEEP_Y
    Params:
        - False_Easting: EPSG_NAME_PARAMETER_FALSE_EASTING
        - False_Northing: EPSG_NAME_PARAMETER_FALSE_NORTHING
        - Longitude_Of_Center: EPSG_NAME_PARAMETER_LONGITUDE_OF_NATURAL_ORIGIN
        - Height: "Satellite Height"
        - Option: 0.0

- Mercator_Auxiliary_Sphere:
    WKT2_name: EPSG_NAME_METHOD_POPULAR_VISUALISATION_PSEUDO_MERCATOR
    Params:
        - False_Easting: EPSG_NAME_PARAMETER_FALSE_EASTING
        - False_Northing: EPSG_NAME_PARAMETER_FALSE_NORTHING
        - Central_Meridian: EPSG_NAME_PARAMETER_LONGITUDE_OF_NATURAL_ORIGIN
        - Standard_Parallel_1: EPSG_NAME_PARAMETER_LATITUDE_OF_NATURAL_ORIGIN
        - Auxiliary_Sphere_Type: 0.0

- Mercator_Variant_A:
    WKT2_name: EPSG_NAME_METHOD_MERCATOR_VARIANT_A
    Params:
        - False_Easting: EPSG_NAME_PARAMETER_FALSE_EASTING
        - False_Northing: EPSG_NAME_PARAMETER_FALSE_NORTHING
        - Central_Meridian: EPSG_NAME_PARAMETER_LONGITUDE_OF_NATURAL_ORIGIN
        - Scale_Factor: EPSG_NAME_PARAMETER_SCALE_FACTOR_AT_NATURAL_ORIGIN

- Mercator_Variant_C:
    WKT2_name: EPSG_NAME_METHOD_MERCATOR_VARIANT_B
    Params:
        - False_Easting: EPSG_NAME_PARAMETER_FALSE_EASTING
        - False_Northing: EPSG_NAME_PARAMETER_FALSE_NORTHING
        - Central_Meridian: EPSG_NAME_PARAMETER_LONGITUDE_OF_NATURAL_ORIGIN
        - Standard_Parallel_1: EPSG_NAME_PARAMETER_LATITUDE_1ST_STD_PARALLEL
        - Latitude_Of_Origin: 0

- Transverse_Cylindrical_Equal_Area:
    WKT2_name: Transverse Cylindrical Equal Area
    Params:
        - False_Easting: EPSG_NAME_PARAMETER_FALSE_EASTING
        - False_Northing: EPSG_NAME_PARAMETER_FALSE_NORTHING
        - Central_Meridian: EPSG_NAME_PARAMETER_LONGITUDE_OF_NATURAL_ORIGIN
        - Scale_Factor: EPSG_NAME_PARAMETER_SCALE_FACTOR_AT_NATURAL_ORIGIN
        - Latitude_Of_Origin: EPSG_NAME_PARAMETER_LATITUDE_OF_NATURAL_ORIGIN

- IGAC_Plano_Cartesiano:
    WKT2_name: EPSG_NAME_METHOD_COLOMBIA_URBAN
    Params:
        - False_Easting: EPSG_NAME_PARAMETER_FALSE_EASTING
        - False_Northing: EPSG_NAME_PARAMETER_FALSE_NORTHING
        - Longitude_Of_Center: EPSG_NAME_PARAMETER_LONGITUDE_OF_NATURAL_ORIGIN
        - Latitude_Of_Center: EPSG_NAME_PARAMETER_LATITUDE_OF_NATURAL_ORIGIN
        - Height: EPSG_NAME_PARAMETER_PROJECTION_PLANE_ORIGIN_HEIGHT


# Missing/unclear mappings

# Hammer_Aitoff: possibly hammer?
# Hammer_Ellipsoidal: possibly hammer?
# Eckert_Greifendorff: +proj=hammer +W=0.25 +M=1
# Tobler_Cylindrical_I: likely tobmerc, but parameters TBD
# Tobler_Cylindrical_II: likely tobmerc, but parameters TBD

# Missing mappings

# Transverse_Mercator_NGA_2014: utm -- tricky mapping from Central_Meridian to zone
# Polar_Stereographic_Variant_A: ups -- tricky mapping from Latitude_Of_Origin to "+south" when required
# Transverse Mercator: alias for Transverse_Mercator, as seen in ESRI:102470 - ESRI:102489


# The following methods are not currently possible in PROJ:

# Ney_Modified_Conic
# Fuller
# Berghaus_Star
# Cube
# Robinson_ARC_INFO
# Equidistant_Cylindrical_Auxiliary_Sphere
# Aspect_Adaptive_Cylindrical
# Mollweide_Auxiliary_Sphere
# Eckert_VI_Auxiliary_Sphere
# Eckert_IV_Auxiliary_Sphere
# Stereographic_Auxiliary_Sphere
# Van_der_Grinten_I_Auxiliary_Sphere
# Azimuthal_Equidistant_Auxiliary_Sphere
# Lambert_Azimuthal_Equal_Area_Auxiliary_Sphere
# Orthographic_Auxiliary_Sphere
# Gnomonic_Auxiliary_Sphere
# Polar_Stereographic_Variant_B
# Polar_Stereographic_Variant_C
# Quartic_Authalic_Ellipsoidal
# Adams_Square_II
# Peirce_Quincuncial

"""

config = yaml.load(config_str)

all_projs = []


def generate_mapping(WKT2_name, esri_proj_name, Params, suffix=''):

    c_name = 'paramsESRI_%s%s' % (esri_proj_name, suffix)
    if isinstance(WKT2_name, list):
        for WKT2_name_s in WKT2_name:
            all_projs.append([esri_proj_name, WKT2_name_s, c_name])
    else:
        all_projs.append([esri_proj_name, WKT2_name, c_name])
    print('static const ESRIParamMapping %s[] = { ' % c_name)
    for param in Params:
        for param_name in param:
            param_value = param[param_name]

            default_value = None
            if isinstance(param_value, dict):
                default_value = param_value.get('Default', None)
                param_value = param_value['Name']

            if isinstance(param_value, str):
                if param_value.startswith('EPSG_'):
                    print('  { "%s", %s, %s, "%.1f", %s },' % (param_name, param_value, param_value.replace('_NAME_', '_CODE_'), default_value or 0.0, "true" if default_value is not None else "false"))
                else:
                    print('  { "%s", "%s", 0, "%.1f", %s },' % (param_name, param_value, default_value or 0.0, "true" if default_value is not None else "false"))
            else:
                print('  { "%s", nullptr, 0, "%.1f", false },' % (param_name, param_value))
    print('  { nullptr, nullptr, 0, "0.0", false }')
    print('};')


print('// This file was generated by scripts/build_esri_projection_mapping.py. DO NOT EDIT !')
print('')
print("""
/******************************************************************************
 *
 * Project:  PROJ
 * Purpose:  Mappings between ESRI projection and parameters names and WKT2
 * Author:   Even Rouault <even dot rouault at spatialys dot com>
 *
 ******************************************************************************
 * Copyright (c) 2019, Even Rouault <even dot rouault at spatialys dot com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 ****************************************************************************/

#ifndef FROM_COORDINATE_OPERATION_CPP
#error This file should only be included from coordinateoperation.cpp
#endif

#ifndef ESRI_PROJECTION_MAPPINGS_HH_INCLUDED
#define ESRI_PROJECTION_MAPPINGS_HH_INCLUDED

#include "coordinateoperation_internal.hpp"

//! @cond Doxygen_Suppress

// ---------------------------------------------------------------------------

// anonymous namespace
namespace {

using namespace ::NS_PROJ;
using namespace ::NS_PROJ::operation;

""")

for item in config:
    for esri_proj_name in item:
        proj_config = item[esri_proj_name]
        if isinstance(proj_config, dict):
            WKT2_name = proj_config['WKT2_name']
            Params = proj_config['Params']
            generate_mapping(WKT2_name, esri_proj_name, Params)
        else:
            count = 1
            for subconfig in proj_config:
                WKT2_name = subconfig['WKT2_name']
                Params = subconfig['Params']
                generate_mapping(WKT2_name, esri_proj_name, Params,
                                 suffix='_alt%d' % count)
                count += 1
    print('')

print('static const ESRIMethodMapping esriMappings[] = {')
for esri_proj_name, WKT2_name, c_name in all_projs:
    if WKT2_name.startswith('EPSG_'):
        print('  { "%s", %s, %s, %s },' % (esri_proj_name, WKT2_name, WKT2_name.replace('_NAME_', '_CODE_'), c_name))
    elif WKT2_name.startswith('PROJ_'):
        print('  { "%s", %s, 0, %s },' % (esri_proj_name, WKT2_name, c_name))
    else:
        print('  { "%s", "%s", 0, %s },' % (esri_proj_name, WKT2_name, c_name))
print('};')

print("""
// ---------------------------------------------------------------------------

} // namespace {

//! @endcond

#endif // ESRI_PROJECTION_MAPPINGS_HH_INCLUDED
""")
