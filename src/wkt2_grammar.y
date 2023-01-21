%{
/******************************************************************************
 * Project:  PROJ
 * Purpose:  WKT2 parser grammar
 * Author:   Even Rouault, <even.rouault at spatialys.com>
 *
 ******************************************************************************
 * Copyright (c) 2018 Even Rouault, <even.rouault at spatialys.com>
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

#include "wkt2_parser.h"

%}

%define api.pure
%define parse.error verbose
%require "3.0"

%parse-param {pj_wkt2_parse_context *context}
%lex-param {pj_wkt2_parse_context *context}

/* From WKT1 */
%token T_PROJECTION             "PROJECTION"
%token T_DATUM                  "DATUM"
%token T_SPHEROID               "SPHEROID"
%token T_PRIMEM                 "PRIMEM"
%token T_UNIT                   "UNIT"
%token T_AXIS                   "AXIS"
%token T_PARAMETER              "PARAMETER"

/* WKT2 preferred */
%token T_GEODCRS                "GEODCRS";
%token T_LENGTHUNIT             "LENGTHUNIT";
%token T_ANGLEUNIT              "ANGLEUNIT";
%token T_SCALEUNIT              "SCALEUNIT";
%token T_TIMEUNIT               "TIMEUNIT";
%token T_ELLIPSOID              "ELLIPSOID";
%token T_CS                     "CS";
%token T_ID                     "ID";
%token T_PROJCRS                "PROJCRS";
%token T_BASEGEODCRS            "BASEGEODCRS";
%token T_MERIDIAN               "MERIDIAN";
%token T_BEARING                "BEARING";
%token T_ORDER                  "ORDER";
%token T_ANCHOR                 "ANCHOR";
%token T_ANCHOREPOCH            "ANCHOREPOCH";
%token T_CONVERSION             "CONVERSION";
%token T_METHOD                 "METHOD";
%token T_REMARK                 "REMARK";
%token T_GEOGCRS                "GEOGCRS";
%token T_BASEGEOGCRS            "BASEGEOGCRS";
%token T_SCOPE                  "SCOPE";
%token T_AREA                   "AREA";
%token T_BBOX                   "BBOX";
%token T_CITATION               "CITATION";
%token T_URI                    "URI";
%token T_VERTCRS                "VERTCRS";
%token T_VDATUM                 "VDATUM";
%token T_GEOIDMODEL             "GEOIDMODEL";
%token T_COMPOUNDCRS            "COMPOUNDCRS";
%token T_PARAMETERFILE          "PARAMETERFILE";
%token T_COORDINATEOPERATION    "COORDINATEOPERATION";
%token T_SOURCECRS              "SOURCECRS";
%token T_TARGETCRS              "TARGETCRS";
%token T_INTERPOLATIONCRS       "INTERPOLATIONCRS";
%token T_OPERATIONACCURACY      "OPERATIONACCURACY";
%token T_CONCATENATEDOPERATION  "CONCATENATEDOPERATION";
%token T_STEP                   "STEP";
%token T_BOUNDCRS               "BOUNDCRS";
%token T_ABRIDGEDTRANSFORMATION "ABRIDGEDTRANSFORMATION";
%token T_DERIVINGCONVERSION     "DERIVINGCONVERSION";
%token T_TDATUM                 "TDATUM";
%token T_CALENDAR               "CALENDAR";
%token T_TIMEORIGIN             "TIMEORIGIN";
%token T_TIMECRS                "TIMECRS";
%token T_VERTICALEXTENT         "VERTICALEXTENT";
%token T_TIMEEXTENT             "TIMEEXTENT";
%token T_USAGE                  "USAGE";
%token T_DYNAMIC                "DYNAMIC";
%token T_FRAMEEPOCH             "FRAMEEPOCH";
%token T_MODEL                  "MODEL";
%token T_VELOCITYGRID           "VELOCITYGRID";
%token T_ENSEMBLE               "ENSEMBLE";
%token T_MEMBER                 "MEMBER";
%token T_ENSEMBLEACCURACY       "ENSEMBLEACCURACY";
%token T_DERIVEDPROJCRS         "DERIVEDPROJCRS";
%token T_BASEPROJCRS            "BASEPROJCRS";
%token T_EDATUM                 "EDATUM";
%token T_ENGCRS                 "ENGCRS";
%token T_PDATUM                 "PDATUM";
%token T_PARAMETRICCRS          "PARAMETRICCRS";
%token T_PARAMETRICUNIT         "PARAMETRICUNIT";
%token T_BASEVERTCRS            "BASEVERTCRS";
%token T_BASEENGCRS             "BASEENGCRS";
%token T_BASEPARAMCRS           "BASEPARAMCRS";
%token T_BASETIMECRS            "BASETIMECRS";
%token T_EPOCH                  "EPOCH"
%token T_COORDEPOCH             "COORDEPOCH"
%token T_COORDINATEMETADATA     "COORDINATEMETADATA"
%token T_POINTMOTIONOPERATION   "POINTMOTIONOPERATION"
%token T_VERSION                "VERSION"
%token T_AXISMINVALUE           "AXISMINVALUE"
%token T_AXISMAXVALUE           "AXISMAXVALUE"
%token T_RANGEMEANING           "RANGEMEANING"
%token T_exact                  "exact"
%token T_wraparound             "wraparound"

/* WKT2 alternate (longer or shorter) */
%token T_GEODETICCRS            "GEODETICCRS";
%token T_GEODETICDATUM          "GEODETICDATUM";
%token T_PROJECTEDCRS           "PROJECTEDCRS";
%token T_PRIMEMERIDIAN          "PRIMEMERIDIAN";
%token T_GEOGRAPHICCRS          "GEOGRAPHICCRS";
%token T_TRF                    "TRF";
%token T_VERTICALCRS            "VERTICALCRS";
%token T_VERTICALDATUM          "VERTICALDATUM";
%token T_VRF                    "VRF";
%token T_TIMEDATUM              "TIMEDATUM";
%token T_TEMPORALQUANTITY       "TEMPORALQUANTITY";
%token T_ENGINEERINGDATUM       "ENGINEERINGDATUM";
%token T_ENGINEERINGCRS         "ENGINEERINGCRS";
%token T_PARAMETRICDATUM        "PARAMETRICDATUM";


/* CS types */
%token T_AFFINE                 "affine";
%token T_CARTESIAN              "Cartesian";
%token T_CYLINDRICAL            "cylindrical";
%token T_ELLIPSOIDAL            "ellipsoidal";
%token T_LINEAR                 "linear";
%token T_PARAMETRIC             "parametric";
%token T_POLAR                  "polar";
%token T_SPHERICAL              "spherical";
%token T_VERTICAL               "vertical";
%token T_TEMPORAL               "temporal"; // WKT2_2015 only
%token T_TEMPORALCOUNT          "temporalCount";
%token T_TEMPORALMEASURE        "temporalMeasure";
%token T_ORDINAL                "ordinal";
%token T_TEMPORALDATETIME       "temporalDateTime";

/* Axis directions */
%token T_NORTH                  "north";
%token T_NORTHNORTHEAST         "northNorthEast";
%token T_NORTHEAST              "northEast";
%token T_EASTNORTHEAST          "eastNorthEast";
%token T_EAST                   "east";
%token T_EASTSOUTHEAST          "eastSouthEast";
%token T_SOUTHEAST              "southEast";
%token T_SOUTHSOUTHEAST         "southSouthEast";
%token T_SOUTH                  "south";
%token T_SOUTHSOUTHWEST         "southSouthWest";
%token T_SOUTHWEST              "southWest";
%token T_WESTSOUTHWEST          "westSouthWest";
%token T_WEST                   "west";
%token T_WESTNORTHWEST          "westNorthWest";
%token T_NORTHWEST              "northWest";
%token T_NORTHNORTHWEST         "northNorthWest";
%token T_UP                     "up";
%token T_DOWN                   "down";
%token T_GEOCENTRICX            "geocentricX";
%token T_GEOCENTRICY            "geocentricY";
%token T_GEOCENTRICZ            "geocentricZ";
%token T_COLUMNPOSITIVE         "columnPositive";
%token T_COLUMNNEGATIVE         "columnNegative";
%token T_ROWPOSITIVE            "rowPositive";
%token T_ROWNEGATIVE            "rowNegative";
%token T_DISPLAYRIGHT           "displayRight";
%token T_DISPLAYLEFT            "displayLeft";
%token T_DISPLAYUP              "displayUp";
%token T_DISPLAYDOWN            "displayDown";
%token T_FORWARD                "forward";
%token T_AFT                    "aft";
%token T_PORT                   "port";
%token T_STARBOARD              "starboard";
%token T_CLOCKWISE              "clockwise";
%token T_COUNTERCLOCKWISE       "counterClockwise";
%token T_TOWARDS                "towards";
%token T_AWAYFROM               "awayFrom";
%token T_FUTURE                 "future";
%token T_PAST                   "part";
%token T_UNSPECIFIED            "unspecified";


%token T_STRING                 "string"
%token T_UNSIGNED_INTEGER_DIFFERENT_ONE_TWO_THREE       "unsigned integer"

%token END 0                    "end of string"

%%

/* Derived from BNF grammar in OGC 18-010r3 (WKT2:2018), with a few back
   additions from GC 12-063r5 (WKT2:2015) */

input:
    identifier | ellipsoid | datum | crs | bound_crs | coordinate_metadata |
    coordinate_operation | point_motion_operation | concatenated_operation |
    map_projection

datum:
    geodetic_reference_frame_with_opt_pm | datum_ensemble |
    vertical_reference_frame | engineering_datum | parametric_datum |
    temporal_datum

crs: single_crs | compound_crs

// Basic characters

period: '.'

// Numbers 

number: signed_numeric_literal_with_sign | unsigned_numeric_literal

signed_numeric_literal_with_sign: sign unsigned_numeric_literal

signed_numeric_literal: opt_sign unsigned_numeric_literal

unsigned_numeric_literal: exact_numeric_literal | approximate_numeric_literal

opt_sign: | sign

approximate_numeric_literal: mantissa 'E' exponent

mantissa: exact_numeric_literal

exponent: signed_integer

signed_integer: opt_sign unsigned_integer

exact_numeric_literal: unsigned_integer opt_period_unsigned_integer | period unsigned_integer

opt_period_unsigned_integer: | period unsigned_integer

unsigned_integer: '1' | '2' | '3' | T_UNSIGNED_INTEGER_DIFFERENT_ONE_TWO_THREE

sign: '+' | '-'

// Date and time

colon: ':'

hyphen: '-'

// Significantly modified to avoid shift-reduce ambiguities for Bison
datetime:
      year opt_24_hour_clock
    | year hyphen unsigned_integer opt_24_hour_clock
    | year hyphen month hyphen day opt_24_hour_clock

opt_24_hour_clock: | _24_hour_clock

year: unsigned_integer

month: unsigned_integer

day: unsigned_integer

_24_hour_clock: time_designator hour
                opt_colon_minute_colon_second_time_zone_designator

opt_colon_minute_colon_second_time_zone_designator:
    colon minute opt_colon_second_time_zone_designator
  | time_zone_designator

opt_colon_second_time_zone_designator:
    colon second_time_zone_designator
  | time_zone_designator

time_designator: 'T'

hour: unsigned_integer

minute: unsigned_integer

second_time_zone_designator:
    seconds_integer period time_zone_designator
  | seconds_integer period seconds_fraction time_zone_designator
  | seconds_integer time_zone_designator

seconds_integer: unsigned_integer

seconds_fraction: unsigned_integer

time_zone_designator: utc_designator | local_time_zone_designator

utc_designator: 'Z'

local_time_zone_designator: 
    sign hour opt_colon_minute
  | hour opt_colon_minute

opt_colon_minute: | colon minute


// CRS WKT characters

left_delimiter: '[' | '('

right_delimiter: ']' | ')'

wkt_separator: ','

quoted_latin_text: T_STRING

quoted_unicode_text: T_STRING

// Scope, extent, identifier and remark

opt_separator_scope_extent_identifier_remark:
    | wkt_separator no_opt_separator_scope_extent_identifier_remark

no_opt_separator_scope_extent_identifier_remark:
      scope_extent_opt_identifier_list_opt_remark
    | identifier opt_identifier_list_remark
    | remark

opt_identifier_list_remark: | wkt_separator identifier opt_identifier_list_remark
                            | wkt_separator remark

scope_extent_opt_identifier_list_opt_remark:
      scope_extent_opt_identifier_list_remark
    | usage_list_opt_identifier_list_remark

// WKT2-2015 way
scope_extent_opt_identifier_list_remark:
    scope wkt_separator extent_opt_identifier_list_remark
  | scope opt_identifier_list_remark
  | extent_opt_identifier_list_remark

// WKT2-2018 way
usage_list_opt_identifier_list_remark:
    usage
  | usage wkt_separator remark
  | usage wkt_separator identifier opt_identifier_list_remark
  | usage wkt_separator usage_list_opt_identifier_list_remark

usage: usage_keyword left_delimiter
       scope wkt_separator extent
       right_delimiter

usage_keyword: T_USAGE

scope: scope_keyword left_delimiter
       scope_text_description right_delimiter

scope_keyword: T_SCOPE

scope_text_description: quoted_latin_text

extent:
    area_description
  | geographic_bounding_box
  | vertical_extent
  | temporal_extent
  | area_description wkt_separator geographic_bounding_box
  | area_description wkt_separator vertical_extent
  | area_description wkt_separator temporal_extent
  | geographic_bounding_box wkt_separator vertical_extent
  | geographic_bounding_box wkt_separator temporal_extent
  | vertical_extent wkt_separator temporal_extent
  | area_description wkt_separator geographic_bounding_box wkt_separator vertical_extent
  | area_description wkt_separator geographic_bounding_box wkt_separator temporal_extent
  | area_description wkt_separator vertical_extent wkt_separator temporal_extent
  | geographic_bounding_box wkt_separator vertical_extent wkt_separator temporal_extent
  | area_description wkt_separator geographic_bounding_box wkt_separator vertical_extent wkt_separator temporal_extent

extent_opt_identifier_list_remark:
    area_description opt_identifier_list_remark
  | geographic_bounding_box opt_identifier_list_remark
  | vertical_extent opt_identifier_list_remark
  | temporal_extent opt_identifier_list_remark
  | area_description wkt_separator geographic_bounding_box opt_identifier_list_remark
  | area_description wkt_separator vertical_extent opt_identifier_list_remark
  | area_description wkt_separator temporal_extent opt_identifier_list_remark
  | geographic_bounding_box wkt_separator vertical_extent opt_identifier_list_remark
  | geographic_bounding_box wkt_separator temporal_extent opt_identifier_list_remark
  | vertical_extent wkt_separator temporal_extent opt_identifier_list_remark
  | area_description wkt_separator geographic_bounding_box wkt_separator vertical_extent opt_identifier_list_remark
  | area_description wkt_separator geographic_bounding_box wkt_separator temporal_extent opt_identifier_list_remark
  | area_description wkt_separator vertical_extent wkt_separator temporal_extent opt_identifier_list_remark
  | geographic_bounding_box wkt_separator vertical_extent wkt_separator temporal_extent opt_identifier_list_remark
  | area_description wkt_separator geographic_bounding_box wkt_separator vertical_extent wkt_separator temporal_extent opt_identifier_list_remark

// Area description

area_description: area_description_keyword left_delimiter area_text_description right_delimiter

area_description_keyword: T_AREA

area_text_description: quoted_latin_text

// Geographic bounding box

geographic_bounding_box: geographic_bounding_box_keyword left_delimiter
                         lower_left_latitude wkt_separator lower_left_longitude
                         wkt_separator upper_right_latitude wkt_separator
                         upper_right_longitude right_delimiter

geographic_bounding_box_keyword: T_BBOX

lower_left_latitude: number

lower_left_longitude: number

upper_right_latitude: number

upper_right_longitude: number

// Vertical extent

vertical_extent: vertical_extent_keyword left_delimiter
                 vertical_extent_minimum_height wkt_separator
                 vertical_extent_maximum_height
                 opt_separator_length_unit right_delimiter

opt_separator_length_unit:
    | wkt_separator length_unit

vertical_extent_keyword: T_VERTICALEXTENT

vertical_extent_minimum_height: number

vertical_extent_maximum_height: number

// Temporal extent

temporal_extent: temporal_extent_keyword left_delimiter
                 temporal_extent_start wkt_separator
                 temporal_extent_end right_delimiter

temporal_extent_keyword: T_TIMEEXTENT

temporal_extent_start: datetime | quoted_latin_text

temporal_extent_end: datetime | quoted_latin_text


// Identifier

identifier: identifier_keyword left_delimiter authority_name
            wkt_separator authority_unique_identifier
            opt_version_authority_citation_uri
            right_delimiter

opt_version_authority_citation_uri:
    | wkt_separator version
    | wkt_separator version wkt_separator authority_citation
    | wkt_separator version wkt_separator authority_citation wkt_separator id_uri
    | wkt_separator authority_citation
    | wkt_separator authority_citation wkt_separator id_uri
    | wkt_separator id_uri

identifier_keyword: T_ID

authority_name: quoted_latin_text

authority_unique_identifier: number | quoted_latin_text

version: number | quoted_latin_text

authority_citation: citation_keyword left_delimiter citation right_delimiter

citation_keyword: T_CITATION

citation: quoted_latin_text

id_uri: uri_keyword left_delimiter uri right_delimiter

uri_keyword: T_URI

uri: quoted_latin_text

// Remark

remark: remark_keyword left_delimiter quoted_unicode_text right_delimiter

remark_keyword: T_REMARK

// Unit

unit: spatial_unit | time_unit

//spatial_unit: angle_unit | length_unit | parametric_unit | scale_unit
spatial_unit: angle_or_length_or_parametric_or_scale_unit

angle_or_length_or_parametric_or_scale_unit:
    angle_or_length_or_parametric_or_scale_unit_keyword
    left_delimiter unit_name
    wkt_separator conversion_factor
    opt_separator_identifier_list right_delimiter

angle_or_length_or_parametric_or_scale_unit_keyword: T_ANGLEUNIT | T_LENGTHUNIT | T_PARAMETRICUNIT | T_SCALEUNIT | T_UNIT

angle_or_length_or_scale_unit:
    angle_or_length_or_scale_unit_keyword
    left_delimiter unit_name
    wkt_separator conversion_factor
    opt_separator_identifier_list right_delimiter

angle_or_length_or_scale_unit_keyword: T_ANGLEUNIT | T_LENGTHUNIT | T_SCALEUNIT | T_UNIT


angle_unit: angle_unit_keyword left_delimiter unit_name
            wkt_separator conversion_factor
            opt_separator_identifier_list right_delimiter

opt_separator_identifier_list:
    | wkt_separator identifier opt_separator_identifier_list

length_unit: length_unit_keyword left_delimiter unit_name
            wkt_separator conversion_factor
            opt_separator_identifier_list right_delimiter

/*
parametric_unit: parametric_unit_keyword left_delimiter unit_name
            wkt_separator conversion_factor
            opt_separator_identifier_list right_delimiter
*/

/*
scale_unit: scale_unit_keyword left_delimiter unit_name
            wkt_separator conversion_factor
            opt_separator_identifier_list right_delimiter
*/

time_unit: time_unit_keyword left_delimiter unit_name
            opt_separator_conversion_factor_identifier_list
            right_delimiter

opt_separator_conversion_factor_identifier_list:
    | wkt_separator conversion_factor opt_separator_identifier_list

angle_unit_keyword: T_ANGLEUNIT | T_UNIT

length_unit_keyword: T_LENGTHUNIT | T_UNIT

// parametric_unit_keyword: T_PARAMETRICUNIT

// scale_unit_keyword: T_SCALEUNIT | T_UNIT

time_unit_keyword: T_TIMEUNIT | T_TEMPORALQUANTITY

unit_name: quoted_latin_text

conversion_factor: unsigned_numeric_literal


// Coordinate system

// coordinate_system: spatial_cs |  temporalcountmeasure_cs | ordinatedatetime_cs

coordinate_system_scope_extent_identifier_remark: spatial_cs_scope_extent_identifier_remark | wkt2015temporal_cs_scope_extent_identifier_remark | temporalcountmeasure_cs_scope_extent_identifier_remark | ordinaldatetime_cs_scope_extent_identifier_remark

spatial_cs_scope_extent_identifier_remark: cs_keyword left_delimiter spatial_cs_type
            wkt_separator dimension
            opt_separator_identifier_list
            right_delimiter
            wkt_separator spatial_axis
            opt_separator_spatial_axis_list_opt_separator_cs_unit_scope_extent_identifier_remark

opt_separator_spatial_axis_list_opt_separator_cs_unit_scope_extent_identifier_remark:
  | wkt_separator cs_unit opt_separator_scope_extent_identifier_remark
  | wkt_separator spatial_axis opt_separator_spatial_axis_list_opt_separator_cs_unit_scope_extent_identifier_remark
  | wkt_separator no_opt_separator_scope_extent_identifier_remark

wkt2015temporal_cs_scope_extent_identifier_remark: cs_keyword left_delimiter T_TEMPORAL
            wkt_separator dimension
            opt_separator_identifier_list
            right_delimiter
            wkt_separator temporalcountmeasure_axis
            opt_separator_cs_unit_scope_extent_identifier_remark

opt_separator_cs_unit_scope_extent_identifier_remark:
    | wkt_separator cs_unit
    | wkt_separator cs_unit wkt_separator no_opt_separator_scope_extent_identifier_remark
    | wkt_separator no_opt_separator_scope_extent_identifier_remark

temporalcountmeasure_cs_scope_extent_identifier_remark: cs_keyword left_delimiter temporalcountmeasure_cs_type
            wkt_separator dimension
            opt_separator_identifier_list
            right_delimiter
            wkt_separator temporalcountmeasure_axis
            opt_separator_scope_extent_identifier_remark

ordinaldatetime_cs_scope_extent_identifier_remark: cs_keyword left_delimiter ordinaldatetime_cs_type
            wkt_separator dimension
            opt_separator_identifier_list
            right_delimiter
            wkt_separator ordinaldatetime_axis
            opt_separator_ordinaldatetime_axis_list_scope_extent_identifier_remark

opt_separator_ordinaldatetime_axis_list_scope_extent_identifier_remark:
  | wkt_separator ordinaldatetime_axis opt_separator_ordinaldatetime_axis_list_scope_extent_identifier_remark
  | wkt_separator no_opt_separator_scope_extent_identifier_remark

cs_keyword: T_CS

spatial_cs_type: T_AFFINE | T_CARTESIAN | T_CYLINDRICAL | T_ELLIPSOIDAL | T_LINEAR | T_PARAMETRIC | T_POLAR | T_SPHERICAL | T_VERTICAL

temporalcountmeasure_cs_type: T_TEMPORALCOUNT | T_TEMPORALMEASURE

ordinaldatetime_cs_type: T_ORDINAL | T_TEMPORALDATETIME

dimension: '1' | '2' | '3'

spatial_axis: axis_keyword left_delimiter axis_name_abbrev
              wkt_separator
              axis_direction_opt_axis_order_spatial_unit_identifier_list
              right_delimiter

temporalcountmeasure_axis: axis_keyword left_delimiter axis_name_abbrev
              wkt_separator
              axis_direction_except_n_s_cw_ccw
              opt_separator_axis_time_unit_identifier_list
              right_delimiter

ordinaldatetime_axis: axis_keyword left_delimiter axis_name_abbrev
              wkt_separator
              axis_direction_opt_axis_order_identifier_list
              right_delimiter

axis_keyword: T_AXIS

// Approximation of <double quote> { <axis name> | <axis abbreviation> | <axis name and abbrev> } <double quote>
axis_name_abbrev: quoted_latin_text

axis_direction_opt_axis_order_spatial_unit_identifier_list:
    axis_direction_except_n_s_cw_ccw_opt_axis_spatial_unit_identifier_list
  | T_NORTH
  | T_NORTH wkt_separator north_south_options_spatial_unit
  | T_SOUTH
  | T_SOUTH wkt_separator north_south_options_spatial_unit
  | T_CLOCKWISE
  | T_CLOCKWISE wkt_separator clockwise_counter_clockwise_options_spatial_unit
  | T_COUNTERCLOCKWISE
  | T_COUNTERCLOCKWISE wkt_separator clockwise_counter_clockwise_options_spatial_unit

north_south_options_spatial_unit:
    identifier opt_separator_identifier_list
  | meridian wkt_separator axis_order opt_separator_identifier_list
  | meridian wkt_separator axis_order wkt_separator spatial_unit opt_separator_identifier_list
  | meridian wkt_separator spatial_unit opt_separator_identifier_list
  | meridian opt_separator_identifier_list
  | axis_order wkt_separator spatial_unit opt_separator_identifier_list
  | axis_order opt_separator_identifier_list
  | spatial_unit opt_separator_identifier_list

clockwise_counter_clockwise_options_spatial_unit:
    identifier opt_separator_identifier_list
  | bearing wkt_separator axis_order opt_separator_identifier_list
  | bearing wkt_separator axis_order wkt_separator spatial_unit opt_separator_identifier_list
  | bearing wkt_separator spatial_unit opt_separator_identifier_list
  | bearing opt_separator_identifier_list
  | axis_order wkt_separator spatial_unit opt_separator_identifier_list
  | axis_order opt_separator_identifier_list
  | spatial_unit opt_separator_identifier_list

axis_direction_except_n_s_cw_ccw_opt_axis_spatial_unit_identifier_list:
      axis_direction_except_n_s_cw_ccw
    | axis_direction_except_n_s_cw_ccw wkt_separator axis_direction_except_n_s_cw_ccw_opt_axis_spatial_unit_identifier_list_options

axis_direction_except_n_s_cw_ccw_opt_axis_spatial_unit_identifier_list_options:
    identifier opt_separator_identifier_list
    | axis_range_opt_separator_identifier_list
    | axis_order opt_separator_axis_range_opt_separator_identifier_list
    | axis_order wkt_separator spatial_unit opt_separator_axis_range_opt_separator_identifier_list
    | spatial_unit opt_separator_axis_range_opt_separator_identifier_list


axis_direction_opt_axis_order_identifier_list:
    axis_direction_except_n_s_cw_ccw_opt_axis_identifier_list
  | T_NORTH
  | T_NORTH wkt_separator north_south_options
  | T_SOUTH
  | T_SOUTH wkt_separator north_south_options_spatial_unit
  | T_CLOCKWISE
  | T_CLOCKWISE wkt_separator clockwise_counter_clockwise_options
  | T_COUNTERCLOCKWISE
  | T_COUNTERCLOCKWISE wkt_separator clockwise_counter_clockwise_options

north_south_options:
    identifier opt_separator_identifier_list
  | meridian wkt_separator axis_order opt_separator_identifier_list
  | meridian opt_separator_identifier_list
  | axis_order opt_separator_identifier_list

clockwise_counter_clockwise_options:
    identifier opt_separator_identifier_list
  | bearing wkt_separator axis_order opt_separator_identifier_list
  | bearing opt_separator_identifier_list
  | axis_order opt_separator_identifier_list

axis_direction_except_n_s_cw_ccw_opt_axis_identifier_list:
      axis_direction_except_n_s_cw_ccw
    | axis_direction_except_n_s_cw_ccw wkt_separator axis_direction_except_n_s_cw_ccw_opt_axis_identifier_list_options

axis_direction_except_n_s_cw_ccw_opt_axis_identifier_list_options:
    identifier opt_separator_identifier_list
    | axis_order opt_separator_axis_range_opt_separator_identifier_list
    | axis_range_opt_separator_identifier_list




opt_separator_axis_time_unit_identifier_list:
    | wkt_separator axis_direction_except_n_s_cw_ccw_opt_axis_time_unit_identifier_list_options

axis_direction_except_n_s_cw_ccw_opt_axis_time_unit_identifier_list_options:
    identifier opt_separator_identifier_list
    | axis_range_opt_separator_identifier_list
    | axis_order opt_separator_axis_range_opt_separator_identifier_list
    | axis_order wkt_separator time_unit opt_separator_axis_range_opt_separator_identifier_list
    | time_unit opt_separator_axis_range_opt_separator_identifier_list

axis_direction_except_n_s_cw_ccw:
                  T_NORTHNORTHEAST
                | T_NORTHEAST
                | T_EASTNORTHEAST
                | T_EAST
                | T_EASTSOUTHEAST
                | T_SOUTHEAST
                | T_SOUTHSOUTHEAST
                | T_SOUTHSOUTHWEST
                | T_SOUTHWEST
                | T_WESTSOUTHWEST
                | T_WEST
                | T_WESTNORTHWEST
                | T_NORTHWEST
                | T_NORTHNORTHWEST
                | T_UP
                | T_DOWN
                | T_GEOCENTRICX
                | T_GEOCENTRICY
                | T_GEOCENTRICZ
                | T_COLUMNPOSITIVE
                | T_COLUMNNEGATIVE
                | T_ROWPOSITIVE
                | T_ROWNEGATIVE
                | T_DISPLAYRIGHT
                | T_DISPLAYLEFT
                | T_DISPLAYUP
                | T_DISPLAYDOWN
                | T_FORWARD
                | T_AFT
                | T_PORT
                | T_STARBOARD
                | T_TOWARDS
                | T_AWAYFROM
                | T_FUTURE
                | T_PAST
                | T_UNSPECIFIED

meridian: meridian_keyword left_delimiter number
          wkt_separator angle_unit right_delimiter

meridian_keyword: T_MERIDIAN

bearing: bearing_keyword left_delimiter number right_delimiter

bearing_keyword: T_BEARING

axis_order: axis_order_keyword left_delimiter unsigned_integer right_delimiter

axis_order_keyword: T_ORDER

axis_range_opt_separator_identifier_list:
      axis_minimum_value opt_separator_identifier_list
    | axis_maximum_value opt_separator_identifier_list
    | axis_minimum_value wkt_separator axis_maximum_value opt_separator_identifier_list
    | axis_minimum_value wkt_separator axis_maximum_value wkt_separator axis_range_meaning opt_separator_identifier_list

opt_separator_axis_range_opt_separator_identifier_list:
    | wkt_separator axis_minimum_value opt_separator_identifier_list
    | wkt_separator axis_maximum_value opt_separator_identifier_list
    | wkt_separator axis_minimum_value wkt_separator axis_maximum_value opt_separator_identifier_list
    | wkt_separator axis_minimum_value wkt_separator axis_maximum_value wkt_separator axis_range_meaning opt_separator_identifier_list

axis_minimum_value: axis_minimum_value_keyword left_delimiter number right_delimiter
axis_minimum_value_keyword: T_AXISMINVALUE

axis_maximum_value: axis_maximum_value_keyword left_delimiter number right_delimiter
axis_maximum_value_keyword: T_AXISMAXVALUE

axis_range_meaning: axis_range_meaning_keyword left_delimiter axis_range_meaning_value right_delimiter
axis_range_meaning_keyword: T_RANGEMEANING
axis_range_meaning_value: T_exact | T_wraparound

cs_unit: unit

/*
ellipsoidal_2D_coordinate_system: cs_keyword left_delimiter ellipsoidal_2D_cs_type
                                  wkt_separator ellipsoidal_2D_dimension
                                  opt_separator_identifier_list right_delimiter
                                  separator_spatial_axis_list
                                  opt_separator_cs_unit

ellipsoidal_2D_cs_type: T_ELLIPSOIDAL

ellipsoidal_2D_dimension: '2'
*/

// Datum ensemble

datum_ensemble: geodetic_datum_ensemble_without_pm | vertical_datum_ensemble

geodetic_datum_ensemble_without_pm: datum_ensemble_keyword
                         left_delimiter
                         datum_ensemble_name
                         wkt_separator datum_ensemble_member
                         datum_ensemble_member_list_ellipsoid_accuracy_identifier_list
                         right_delimiter

datum_ensemble_member_list_ellipsoid_accuracy_identifier_list:
      wkt_separator ellipsoid wkt_separator datum_ensemble_accuracy opt_separator_datum_ensemble_identifier_list
    | wkt_separator datum_ensemble_member datum_ensemble_member_list_ellipsoid_accuracy_identifier_list

opt_separator_datum_ensemble_identifier_list:
    | wkt_separator datum_ensemble_identifier opt_separator_datum_ensemble_identifier_list

vertical_datum_ensemble: datum_ensemble_keyword
                         left_delimiter
                         datum_ensemble_name
                         wkt_separator datum_ensemble_member
                         datum_ensemble_member_list_accuracy_identifier_list
                         right_delimiter

datum_ensemble_member_list_accuracy_identifier_list:
      wkt_separator datum_ensemble_accuracy opt_separator_datum_ensemble_identifier_list
    | wkt_separator datum_ensemble_member datum_ensemble_member_list_accuracy_identifier_list

datum_ensemble_keyword: T_ENSEMBLE

datum_ensemble_name: quoted_latin_text

datum_ensemble_member: datum_ensemble_member_keyword left_delimiter
                       datum_ensemble_member_name
                       opt_datum_ensemble_member_identifier_list
                       right_delimiter

opt_datum_ensemble_member_identifier_list:
    | wkt_separator datum_ensemble_member_identifier opt_datum_ensemble_member_identifier_list

datum_ensemble_member_keyword: T_MEMBER

datum_ensemble_member_name: quoted_latin_text

datum_ensemble_member_identifier: identifier

datum_ensemble_accuracy: datum_ensemble_accuracy_keyword left_delimiter accuracy right_delimiter

datum_ensemble_accuracy_keyword: T_ENSEMBLEACCURACY

accuracy: number

datum_ensemble_identifier: identifier


// Dynamic coordinate reference systems

dynamic_crs: dynamic_crs_keyword left_delimiter frame_reference_epoch
             opt_separator_deformation_model_id
             right_delimiter

dynamic_crs_keyword: T_DYNAMIC

frame_reference_epoch: frame_reference_epoch_keyword left_delimiter
                       reference_epoch right_delimiter

frame_reference_epoch_keyword: T_FRAMEEPOCH

reference_epoch:
    unsigned_integer
  | unsigned_integer period
  | unsigned_integer period unsigned_integer

opt_separator_deformation_model_id:
    | wkt_separator deformation_model_id

deformation_model_id: deformation_model_id_keyword left_delimiter
                      deformation_model_name
                      opt_separator_identifier
                      right_delimiter

opt_separator_identifier:
    | wkt_separator identifier

deformation_model_id_keyword: T_MODEL | T_VELOCITYGRID

deformation_model_name: quoted_latin_text

// Geodetic CRS

geodetic_crs: static_geodetic_crs | dynamic_geodetic_crs | geographic_crs

geographic_crs: static_geographic_crs | dynamic_geographic_crs

static_geodetic_crs: geodetic_crs_keyword
                     left_delimiter crs_name
                     wkt_separator
                     geodetic_reference_frame_or_geodetic_datum_ensemble_without_pm
                     wkt_separator
                     opt_prime_meridian_coordinate_system_scope_extent_identifier_remark
                     right_delimiter

dynamic_geodetic_crs: geodetic_crs_keyword
                     left_delimiter crs_name
                     wkt_separator dynamic_crs
                     wkt_separator
                     geodetic_reference_frame_without_pm
                     wkt_separator
                     opt_prime_meridian_coordinate_system_scope_extent_identifier_remark
                     right_delimiter

static_geographic_crs: geographic_crs_keyword
                     left_delimiter crs_name
                     wkt_separator
                     geodetic_reference_frame_or_geodetic_datum_ensemble_without_pm
                     wkt_separator
                     opt_prime_meridian_coordinate_system_scope_extent_identifier_remark
                     right_delimiter

dynamic_geographic_crs: geographic_crs_keyword
                     left_delimiter crs_name
                     wkt_separator dynamic_crs
                     wkt_separator
                     geodetic_reference_frame_without_pm
                     wkt_separator
                     opt_prime_meridian_coordinate_system_scope_extent_identifier_remark
                     right_delimiter

opt_prime_meridian_coordinate_system_scope_extent_identifier_remark:
      prime_meridian wkt_separator coordinate_system_scope_extent_identifier_remark
    | coordinate_system_scope_extent_identifier_remark

crs_name: quoted_latin_text

geodetic_crs_keyword: T_GEODCRS | T_GEODETICCRS

geographic_crs_keyword: T_GEOGCRS | T_GEOGRAPHICCRS

geodetic_reference_frame_or_geodetic_datum_ensemble_without_pm:
      geodetic_reference_frame_without_pm
    | geodetic_datum_ensemble_without_pm

// Ellipsoid

ellipsoid: ellipsoid_keyword left_delimiter ellipsoid_name
           wkt_separator semi_major_axis
           wkt_separator inverse_flattening
           opt_separator_length_unit_identifier_list
           right_delimiter

opt_separator_length_unit_identifier_list:
    | wkt_separator length_unit opt_separator_identifier_list
    | wkt_separator identifier opt_separator_identifier_list

ellipsoid_keyword: T_ELLIPSOID | T_SPHEROID

ellipsoid_name: quoted_latin_text

semi_major_axis: unsigned_numeric_literal

inverse_flattening: unsigned_numeric_literal

// Prime meridian

prime_meridian: prime_meridian_keyword left_delimiter
                prime_meridian_name wkt_separator
                irm_longitude_opt_separator_identifier_list
                right_delimiter

prime_meridian_keyword: T_PRIMEM | T_PRIMEMERIDIAN

prime_meridian_name: quoted_latin_text

irm_longitude_opt_separator_identifier_list:
    signed_numeric_literal wkt_separator angle_unit opt_separator_identifier_list
  | signed_numeric_literal opt_separator_identifier_list

// Geodetic reference frame

geodetic_reference_frame_with_opt_pm:
    geodetic_reference_frame_without_pm
  | geodetic_reference_frame_without_pm wkt_separator prime_meridian

geodetic_reference_frame_without_pm: geodetic_reference_frame_keyword
                          left_delimiter datum_name wkt_separator ellipsoid
                          opt_separator_datum_anchor_anchor_epoch_identifier_list
                          right_delimiter

geodetic_reference_frame_keyword: T_DATUM | T_TRF | T_GEODETICDATUM

datum_name: quoted_latin_text

opt_separator_datum_anchor_anchor_epoch_identifier_list:
    | wkt_separator datum_anchor
    | wkt_separator datum_anchor_epoch
    | wkt_separator datum_anchor wkt_separator datum_anchor_epoch
    | wkt_separator identifier opt_separator_identifier_list
    | wkt_separator datum_anchor_epoch wkt_separator identifier opt_separator_identifier_list
    | wkt_separator datum_anchor wkt_separator identifier opt_separator_identifier_list
    | wkt_separator datum_anchor wkt_separator datum_anchor_epoch wkt_separator identifier opt_separator_identifier_list

datum_anchor: datum_anchor_keyword left_delimiter
              datum_anchor_description right_delimiter

datum_anchor_keyword: T_ANCHOR

datum_anchor_description: quoted_latin_text

datum_anchor_epoch: datum_anchor_epoch_keyword left_delimiter
                    anchor_epoch right_delimiter

datum_anchor_epoch_keyword: T_ANCHOREPOCH

anchor_epoch: unsigned_integer | unsigned_integer period | unsigned_integer period unsigned_integer

// Projected CRS

projected_crs: projected_crs_keyword left_delimiter crs_name
               wkt_separator base_geodetic_crs
               wkt_separator map_projection
               wkt_separator coordinate_system_scope_extent_identifier_remark
               right_delimiter

projected_crs_keyword: T_PROJCRS | T_PROJECTEDCRS

// Base CRS

base_geodetic_crs: base_static_geodetic_crs | base_dynamic_geodetic_crs |
                   base_static_geographic_crs | base_dynamic_geographic_crs

base_static_geodetic_crs: base_geodetic_crs_keyword left_delimiter base_crs_name
                          wkt_separator geodetic_reference_frame_or_geodetic_datum_ensemble_without_pm
                          opt_separator_pm_ellipsoidal_cs_unit_opt_separator_identifier_list
                          right_delimiter

opt_separator_pm_ellipsoidal_cs_unit_opt_separator_identifier_list:
    | wkt_separator prime_meridian opt_separator_identifier_list
    | wkt_separator prime_meridian wkt_separator ellipsoidal_cs_unit opt_separator_identifier_list
    | wkt_separator ellipsoidal_cs_unit opt_separator_identifier_list
    | wkt_separator identifier opt_separator_identifier_list

base_dynamic_geodetic_crs: base_geodetic_crs_keyword left_delimiter base_crs_name
                           wkt_separator dynamic_crs
                           wkt_separator geodetic_reference_frame_without_pm
                           opt_separator_pm_ellipsoidal_cs_unit_opt_separator_identifier_list
                           right_delimiter

base_static_geographic_crs: base_geographic_crs_keyword left_delimiter base_crs_name
                            wkt_separator geodetic_reference_frame_or_geodetic_datum_ensemble_without_pm
                            opt_separator_pm_ellipsoidal_cs_unit_opt_separator_identifier_list
                            right_delimiter

base_dynamic_geographic_crs: base_geographic_crs_keyword left_delimiter base_crs_name
                             wkt_separator dynamic_crs
                             wkt_separator geodetic_reference_frame_without_pm
                             opt_separator_pm_ellipsoidal_cs_unit_opt_separator_identifier_list
                             right_delimiter

base_geodetic_crs_keyword: T_BASEGEODCRS

base_geographic_crs_keyword: T_BASEGEOGCRS

base_crs_name: quoted_latin_text

ellipsoidal_cs_unit: angle_unit

// Map projection

map_projection: map_projection_keyword left_delimiter
                map_projection_name
                wkt_separator map_projection_method
                opt_separator_parameter_list_identifier_list
                right_delimiter

opt_separator_parameter_list_identifier_list:
  | wkt_separator identifier opt_separator_identifier_list
  | wkt_separator map_projection_parameter opt_separator_parameter_list_identifier_list

map_projection_keyword: T_CONVERSION

map_projection_name: quoted_latin_text

map_projection_method: map_projection_method_keyword left_delimiter
                       map_projection_method_name
                       opt_separator_identifier_list right_delimiter

map_projection_method_keyword: T_METHOD | T_PROJECTION

map_projection_method_name: quoted_latin_text

map_projection_parameter: parameter_keyword left_delimiter parameter_name
                          wkt_separator parameter_value
                          opt_separator_param_unit_identifier_list
                          right_delimiter

opt_separator_param_unit_identifier_list:
  | wkt_separator identifier opt_separator_identifier_list
  | wkt_separator map_projection_parameter_unit opt_separator_identifier_list

parameter_keyword: T_PARAMETER

parameter_name: quoted_latin_text

parameter_value: signed_numeric_literal

map_projection_parameter_unit: angle_or_length_or_scale_unit

// Vertical CRS

vertical_crs: static_vertical_crs | dynamic_vertical_crs

static_vertical_crs: vertical_crs_keyword left_delimiter crs_name
                     wkt_separator vertical_reference_frame_or_vertical_datum_ensemble
                     wkt_separator
                     vertical_cs_opt_geoid_model_id_scope_extent_identifier_remark
                     right_delimiter

dynamic_vertical_crs: vertical_crs_keyword left_delimiter crs_name
                     wkt_separator dynamic_crs
                     wkt_separator vertical_reference_frame
                     wkt_separator
                     vertical_cs_opt_geoid_model_id_scope_extent_identifier_remark
                     right_delimiter

vertical_reference_frame_or_vertical_datum_ensemble: vertical_reference_frame | vertical_datum_ensemble

vertical_cs_opt_geoid_model_id_scope_extent_identifier_remark:
            cs_keyword left_delimiter spatial_cs_type
            wkt_separator dimension
            opt_separator_identifier_list
            right_delimiter
            wkt_separator spatial_axis
            opt_separator_cs_unit_opt_geoid_model_id_scope_extent_identifier_remark

opt_separator_cs_unit_opt_geoid_model_id_scope_extent_identifier_remark:
  | wkt_separator cs_unit opt_separator_scope_extent_identifier_remark
  | wkt_separator cs_unit wkt_separator geoid_model_id opt_geoid_model_id_list_opt_separator_scope_extent_identifier_remark
  | wkt_separator geoid_model_id opt_geoid_model_id_list_opt_separator_scope_extent_identifier_remark
  | wkt_separator no_opt_separator_scope_extent_identifier_remark

opt_geoid_model_id_list_opt_separator_scope_extent_identifier_remark:
  | wkt_separator geoid_model_id opt_geoid_model_id_list_opt_separator_scope_extent_identifier_remark
  | wkt_separator no_opt_separator_scope_extent_identifier_remark

geoid_model_id: geoid_model_keyword left_delimiter
                geoid_model_name opt_separator_identifier
                right_delimiter

geoid_model_keyword: T_GEOIDMODEL

geoid_model_name: quoted_latin_text

vertical_crs_keyword: T_VERTCRS | T_VERTICALCRS

// Vertical reference frame

vertical_reference_frame: vertical_reference_frame_keyword left_delimiter
                          datum_name
                          opt_separator_datum_anchor_anchor_epoch_identifier_list
                          right_delimiter

vertical_reference_frame_keyword: T_VDATUM | T_VRF | T_VERTICALDATUM

// Engineering CRS

engineering_crs: engineering_crs_keyword left_delimiter crs_name
                 wkt_separator engineering_datum
                 wkt_separator coordinate_system_scope_extent_identifier_remark
                 right_delimiter

engineering_crs_keyword: T_ENGCRS | T_ENGINEERINGCRS

engineering_datum: engineering_datum_keyword left_delimiter datum_name
                   opt_separator_datum_anchor_identifier_list
                   right_delimiter

engineering_datum_keyword: T_EDATUM | T_ENGINEERINGDATUM

opt_separator_datum_anchor_identifier_list:
    | wkt_separator datum_anchor
    | wkt_separator identifier opt_separator_identifier_list
    | wkt_separator datum_anchor wkt_separator identifier opt_separator_identifier_list

// Parametric CRS

parametric_crs: parametric_crs_keyword left_delimiter crs_name
                wkt_separator parametric_datum
                wkt_separator coordinate_system_scope_extent_identifier_remark
                right_delimiter

parametric_crs_keyword: T_PARAMETRICCRS

parametric_datum: parametric_datum_keyword left_delimiter datum_name
                  opt_separator_datum_anchor_identifier_list
                  right_delimiter

parametric_datum_keyword: T_PDATUM | T_PARAMETRICDATUM

// Temporal CRS

temporal_crs: temporal_crs_keyword left_delimiter crs_name
              wkt_separator temporal_datum
              wkt_separator coordinate_system_scope_extent_identifier_remark
              right_delimiter

temporal_crs_keyword: T_TIMECRS

temporal_datum: temporal_datum_keyword left_delimiter datum_name
                opt_separator_temporal_datum_end
                right_delimiter

opt_separator_temporal_datum_end:
    | wkt_separator calendar opt_separator_identifier_list
    | wkt_separator calendar wkt_separator temporal_origin opt_separator_identifier_list
    | wkt_separator temporal_origin opt_separator_identifier_list
    | wkt_separator identifier opt_separator_identifier_list

temporal_datum_keyword: T_TDATUM | T_TIMEDATUM

temporal_origin: temporal_origin_keyword left_delimiter
                 temporal_origin_description right_delimiter

temporal_origin_keyword: T_TIMEORIGIN

temporal_origin_description: datetime | quoted_latin_text

calendar: calendar_keyword left_delimiter calendar_identifier right_delimiter

calendar_keyword: T_CALENDAR

calendar_identifier: quoted_latin_text

// Deriving conversion

deriving_conversion: deriving_conversion_keyword left_delimiter
                     deriving_conversion_name wkt_separator
                     operation_method
                     opt_separator_parameter_or_parameter_file_identifier_list
                     right_delimiter

opt_separator_parameter_or_parameter_file_identifier_list:
   | wkt_separator operation_parameter opt_separator_parameter_or_parameter_file_identifier_list
   | wkt_separator operation_parameter_file opt_separator_parameter_or_parameter_file_identifier_list
   | wkt_separator identifier opt_separator_identifier_list

deriving_conversion_keyword: T_DERIVINGCONVERSION

deriving_conversion_name: quoted_latin_text

// Derived CRS conversion method

operation_method: operation_method_keyword left_delimiter
                  operation_method_name
                  opt_separator_identifier
                  right_delimiter

operation_method_keyword: T_METHOD

operation_method_name: quoted_latin_text

// Derived CRS conversion parameter
operation_parameter: parameter_keyword left_delimiter
                     parameter_name
                     wkt_separator parameter_value
                     opt_separator_parameter_unit_identifier_list
                     right_delimiter

opt_separator_parameter_unit_identifier_list:
    | wkt_separator parameter_unit opt_separator_identifier_list
    | wkt_separator identifier opt_separator_identifier_list

parameter_unit: length_or_angle_or_scale_or_time_or_parametric_unit

// Approximate definition: conversion_factor should be optional only for a timeunit (but not easy to detect if UNIT keyword is used!)
length_or_angle_or_scale_or_time_or_parametric_unit:
    length_or_angle_or_scale_or_time_or_parametric_unit_keyword
    left_delimiter unit_name
    opt_separator_conversion_factor_identifier_list
    right_delimiter

length_or_angle_or_scale_or_time_or_parametric_unit_keyword:
    T_LENGTHUNIT | T_ANGLEUNIT | T_SCALEUNIT | T_TIMEUNIT | T_PARAMETRICUNIT | T_UNIT

// Derived CRS conversion parameter file

operation_parameter_file: parameter_file_keyword left_delimiter parameter_name
                          wkt_separator parameter_file_name
                          opt_separator_identifier
                          right_delimiter

parameter_file_keyword: T_PARAMETERFILE

parameter_file_name: quoted_latin_text


// Derived geodetic CRS and derived geographic CRS

derived_geodetic_crs: derived_static_geod_crs | derived_dynamic_geod_crs |
                      derived_geographic_crs

derived_geographic_crs: derived_static_geog_crs | derived_dynamic_geog_crs

derived_static_geod_crs: geodetic_crs_keyword
                      left_delimiter crs_name
                      wkt_separator base_static_geod_crs_or_base_static_geog_crs
                      wkt_separator deriving_conversion
                      wkt_separator coordinate_system_scope_extent_identifier_remark
                      right_delimiter

base_static_geod_crs_or_base_static_geog_crs: base_static_geod_crs | base_static_geog_crs

derived_dynamic_geod_crs: geodetic_crs_keyword
                      left_delimiter crs_name
                      wkt_separator base_dynamic_geod_crs_or_base_dynamic_geog_crs
                      wkt_separator deriving_conversion
                      wkt_separator coordinate_system_scope_extent_identifier_remark
                      right_delimiter

base_dynamic_geod_crs_or_base_dynamic_geog_crs: base_dynamic_geod_crs | base_dynamic_geog_crs

derived_static_geog_crs: geographic_crs_keyword
                      left_delimiter crs_name
                      wkt_separator base_static_geod_crs_or_base_static_geog_crs
                      wkt_separator deriving_conversion
                      wkt_separator coordinate_system_scope_extent_identifier_remark
                      right_delimiter

derived_dynamic_geog_crs: geographic_crs_keyword
                      left_delimiter crs_name
                      wkt_separator base_dynamic_geod_crs_or_base_dynamic_geog_crs
                      wkt_separator deriving_conversion
                      wkt_separator coordinate_system_scope_extent_identifier_remark
                      right_delimiter

base_static_geod_crs: base_geodetic_crs_keyword left_delimiter base_crs_name
                          wkt_separator geodetic_reference_frame_or_geodetic_datum_ensemble_without_pm
                          opt_separator_pm_opt_separator_identifier_list
                          right_delimiter

opt_separator_pm_opt_separator_identifier_list:
    | wkt_separator prime_meridian opt_separator_identifier_list
    | wkt_separator identifier opt_separator_identifier_list

base_dynamic_geod_crs: base_geodetic_crs_keyword left_delimiter base_crs_name
                          wkt_separator dynamic_crs
                          wkt_separator geodetic_reference_frame_without_pm
                          opt_separator_pm_opt_separator_identifier_list
                          right_delimiter

base_static_geog_crs: base_geographic_crs_keyword left_delimiter base_crs_name
                          wkt_separator geodetic_reference_frame_or_geodetic_datum_ensemble_without_pm
                          opt_separator_pm_opt_separator_identifier_list
                          right_delimiter

base_dynamic_geog_crs: base_geographic_crs_keyword left_delimiter base_crs_name
                          wkt_separator dynamic_crs
                          wkt_separator geodetic_reference_frame_without_pm
                          opt_separator_pm_opt_separator_identifier_list
                          right_delimiter

// Derived projected CRS

derived_projected_crs: derived_projected_crs_keyword left_delimiter
                       derived_crs_name wkt_separator base_projected_crs
                       wkt_separator deriving_conversion
                       wkt_separator coordinate_system_scope_extent_identifier_remark
                       right_delimiter

derived_projected_crs_keyword: T_DERIVEDPROJCRS

derived_crs_name: quoted_latin_text

base_projected_crs: base_projected_crs_keyword left_delimiter base_crs_name
                    wkt_separator base_geodetic_geographic_crs
                    wkt_separator map_projection
                    opt_separator_identifier_list
                    right_delimiter

base_projected_crs_keyword: T_BASEPROJCRS

base_geodetic_geographic_crs: base_static_geod_crs | base_dynamic_geod_crs |
                              base_static_geog_crs | base_dynamic_geog_crs

// Derived vertical CRS

derived_vertical_crs: vertical_crs_keyword left_delimiter crs_name
                      wkt_separator base_vertical_crs
                      wkt_separator deriving_conversion
                      wkt_separator coordinate_system_scope_extent_identifier_remark
                      right_delimiter

base_vertical_crs: base_static_vertical_crs | base_dynamic_vertical_crs

base_static_vertical_crs: base_vertical_crs_keyword left_delimiter base_crs_name
                          wkt_separator vertical_reference_frame
                          opt_separator_identifier_list
                          right_delimiter

base_dynamic_vertical_crs: base_vertical_crs_keyword left_delimiter base_crs_name
                           wkt_separator dynamic_crs
                           wkt_separator vertical_reference_frame
                           opt_separator_identifier_list
                           right_delimiter

base_vertical_crs_keyword: T_BASEVERTCRS


// Derived engineering CRS

derived_engineering_crs: engineering_crs_keyword left_delimiter crs_name
                         wkt_separator base_engineering_crs
                         wkt_separator deriving_conversion
                         wkt_separator coordinate_system_scope_extent_identifier_remark
                         right_delimiter

base_engineering_crs: base_engineering_crs_keyword left_delimiter base_crs_name
                      wkt_separator engineering_datum
                      opt_separator_identifier_list
                      right_delimiter

base_engineering_crs_keyword: T_BASEENGCRS


// Derived parametric CRS

derived_parametric_crs: parametric_crs_keyword left_delimiter crs_name
                        wkt_separator base_parametric_crs
                        wkt_separator deriving_conversion
                        wkt_separator coordinate_system_scope_extent_identifier_remark
                        right_delimiter

base_parametric_crs: base_parametric_crs_keyword left_delimiter base_crs_name
                     wkt_separator parametric_datum
                     opt_separator_identifier_list
                     right_delimiter

base_parametric_crs_keyword: T_BASEPARAMCRS


// Derived temporal CRS

derived_temporal_crs: temporal_crs_keyword left_delimiter crs_name
                      wkt_separator base_temporal_crs
                      wkt_separator deriving_conversion
                      wkt_separator coordinate_system_scope_extent_identifier_remark
                      right_delimiter

base_temporal_crs: base_temporal_crs_keyword left_delimiter base_crs_name
                   wkt_separator temporal_datum
                   opt_separator_identifier_list
                   right_delimiter

base_temporal_crs_keyword: T_BASETIMECRS


// Compound CRS

compound_crs: compound_crs_keyword left_delimiter compound_crs_name
              wkt_separator single_crs_or_bound_crs
              wkt_separator single_crs_or_bound_crs
              opt_wkt_separator_single_crs_list_opt_separator_scope_extent_identifier_remark
              right_delimiter

single_crs: geodetic_crs | derived_geodetic_crs |
            projected_crs | derived_projected_crs |
            vertical_crs | derived_vertical_crs |
            engineering_crs | derived_engineering_crs |
            parametric_crs | derived_parametric_crs |
            temporal_crs | derived_temporal_crs

// PROJ extension
single_crs_or_bound_crs: single_crs | bound_crs

opt_wkt_separator_single_crs_list_opt_separator_scope_extent_identifier_remark:
    | wkt_separator single_crs_or_bound_crs opt_wkt_separator_single_crs_list_opt_separator_scope_extent_identifier_remark
    | wkt_separator no_opt_separator_scope_extent_identifier_remark

compound_crs_keyword: T_COMPOUNDCRS

compound_crs_name: quoted_latin_text

// coordinate epoch and coordinate metadata

metadata_coordinate_epoch: coordinate_epoch_keyword left_delimiter
                           coordinate_epoch right_delimiter

coordinate_epoch_keyword: T_EPOCH | T_COORDEPOCH

coordinate_epoch: 
    unsigned_integer
  | unsigned_integer period
  | unsigned_integer period unsigned_integer

coordinate_metadata: coordinate_metadata_keyword left_delimiter
                     coordinate_metadata_crs right_delimiter

coordinate_metadata_crs:
    static_crs_coordinate_metadata
  | dynamic_crs_coordinate_metadata wkt_separator metadata_coordinate_epoch

coordinate_metadata_keyword: T_COORDINATEMETADATA

static_crs_coordinate_metadata: static_geodetic_crs | static_geographic_crs |
            projected_crs | static_vertical_crs |
            engineering_crs | parametric_crs | temporal_crs |
            derived_geodetic_crs |
            derived_projected_crs | derived_vertical_crs |
            derived_engineering_crs | derived_parametric_crs |
            derived_temporal_crs | compound_crs

dynamic_crs_coordinate_metadata: dynamic_geodetic_crs | dynamic_geographic_crs |
                                 projected_crs | dynamic_vertical_crs |
                                 derived_geodetic_crs |
                                 derived_projected_crs | derived_vertical_crs

// Coordinate operations

coordinate_operation: operation_keyword left_delimiter operation_name
                      coordinate_operation_next

coordinate_operation_next:
    wkt_separator operation_version coordinate_operation_end
    | coordinate_operation_end

coordinate_operation_end:
                      wkt_separator source_crs wkt_separator target_crs
                      wkt_separator operation_method
                      opt_parameter_or_parameter_file_list_opt_interpolation_crs_opt_operation_accuracy_opt_separator_scope_extent_identifier_remark
                      right_delimiter

opt_parameter_or_parameter_file_list_opt_interpolation_crs_opt_operation_accuracy_opt_separator_scope_extent_identifier_remark:
   | wkt_separator operation_parameter opt_parameter_or_parameter_file_list_opt_interpolation_crs_opt_operation_accuracy_opt_separator_scope_extent_identifier_remark
   | wkt_separator operation_parameter_file opt_parameter_or_parameter_file_list_opt_interpolation_crs_opt_operation_accuracy_opt_separator_scope_extent_identifier_remark
   | wkt_separator interpolation_crs opt_separator_scope_extent_identifier_remark
   | wkt_separator interpolation_crs wkt_separator operation_accuracy opt_separator_scope_extent_identifier_remark
   | wkt_separator operation_accuracy opt_separator_scope_extent_identifier_remark
   | wkt_separator no_opt_separator_scope_extent_identifier_remark

operation_keyword: T_COORDINATEOPERATION

operation_name: quoted_latin_text

operation_version: operation_version_keyword left_delimiter
                   operation_version_text right_delimiter

operation_version_keyword: T_VERSION

operation_version_text: quoted_latin_text

source_crs: source_crs_keyword left_delimiter crs right_delimiter

source_crs_keyword: T_SOURCECRS

target_crs: target_crs_keyword left_delimiter crs right_delimiter

target_crs_keyword: T_TARGETCRS

interpolation_crs: interpolation_crs_keyword left_delimiter crs right_delimiter

interpolation_crs_keyword: T_INTERPOLATIONCRS

operation_accuracy: operation_accuracy_keyword left_delimiter accuracy right_delimiter

operation_accuracy_keyword: T_OPERATIONACCURACY


// Point motion operation

point_motion_operation: point_motion_keyword left_delimiter operation_name
                        point_motion_operation_next

point_motion_operation_next:
    wkt_separator operation_version point_motion_operation_end
    | point_motion_operation_end

point_motion_operation_end:
                        wkt_separator source_crs
                        wkt_separator operation_method
                        opt_parameter_or_parameter_file_list_opt_operation_accuracy_opt_separator_scope_extent_identifier_remark
                        right_delimiter

opt_parameter_or_parameter_file_list_opt_operation_accuracy_opt_separator_scope_extent_identifier_remark:
   | wkt_separator operation_parameter opt_parameter_or_parameter_file_list_opt_operation_accuracy_opt_separator_scope_extent_identifier_remark
   | wkt_separator operation_parameter_file opt_parameter_or_parameter_file_list_opt_operation_accuracy_opt_separator_scope_extent_identifier_remark
   | wkt_separator operation_accuracy opt_separator_scope_extent_identifier_remark
   | wkt_separator no_opt_separator_scope_extent_identifier_remark

point_motion_keyword: T_POINTMOTIONOPERATION


// Concatenated operation

concatenated_operation: concatenated_operation_keyword left_delimiter
                        operation_name
                        concatenated_operation_next

concatenated_operation_next:
    wkt_separator operation_version concatenated_operation_end
    | concatenated_operation_end

concatenated_operation_end:
                        wkt_separator source_crs wkt_separator target_crs
                        wkt_separator step_keyword left_delimiter step right_delimiter
                        wkt_separator step_keyword left_delimiter step right_delimiter
                        opt_concatenated_operation_end
                        right_delimiter

step: coordinate_operation | point_motion_keyword | map_projection | deriving_conversion

opt_concatenated_operation_end:
   | wkt_separator step_keyword left_delimiter step right_delimiter opt_concatenated_operation_end
   | wkt_separator operation_accuracy opt_separator_scope_extent_identifier_remark
   | wkt_separator no_opt_separator_scope_extent_identifier_remark

concatenated_operation_keyword: T_CONCATENATEDOPERATION

step_keyword: T_STEP


// Bound CRS

bound_crs: bound_crs_keyword left_delimiter
           source_crs wkt_separator target_crs
           wkt_separator abridged_coordinate_transformation
           opt_separator_scope_extent_identifier_remark
           right_delimiter

bound_crs_keyword: T_BOUNDCRS

abridged_coordinate_transformation: abridged_transformation_keyword left_delimiter
                                    operation_name
                                    abridged_coordinate_transformation_next

abridged_coordinate_transformation_next:
    wkt_separator operation_version abridged_coordinate_transformation_end
    | abridged_coordinate_transformation_end

abridged_coordinate_transformation_end:
                                    wkt_separator operation_method
// At least one parameter required by WKT2. But relax that to allow things like METHOD["PROJ-based operation method: +proj=...."]
//                                  wkt_separator abridged_parameter_or_parameter_file
                                    opt_end_abridged_coordinate_transformation
                                    right_delimiter

//abridged_parameter_or_parameter_file: abridged_transformation_parameter | operation_parameter_file

opt_end_abridged_coordinate_transformation:
   | wkt_separator abridged_transformation_parameter opt_end_abridged_coordinate_transformation
   | wkt_separator operation_parameter_file opt_end_abridged_coordinate_transformation
   | wkt_separator no_opt_separator_scope_extent_identifier_remark

abridged_transformation_keyword: T_ABRIDGEDTRANSFORMATION

abridged_transformation_parameter: parameter_keyword left_delimiter
                                   parameter_name wkt_separator parameter_value opt_separator_identifier_list
                                   right_delimiter
