/******************************************************************************
 *
 * Project:  PROJ
 * Purpose:  ISO19111:2018 implementation
 * Author:   Even Rouault <even dot rouault at spatialys dot com>
 *
 ******************************************************************************
 * Copyright (c) 2018, Even Rouault <even dot rouault at spatialys dot com>
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

#ifndef COORDINATEOPERATION_CONSTANTS_HH_INCLUDED
#define COORDINATEOPERATION_CONSTANTS_HH_INCLUDED

#include "proj/coordinateoperation_internal.hpp"
#include <string>

// ---------------------------------------------------------------------------

NS_PROJ_START
namespace operation {

//! @cond Doxygen_Suppress

static const std::string
    EPSG_NAME_METHOD_TRANSVERSE_MERCATOR("Transverse Mercator");
constexpr int EPSG_CODE_METHOD_TRANSVERSE_MERCATOR = 9807;

static const std::string EPSG_NAME_METHOD_TRANSVERSE_MERCATOR_SOUTH_ORIENTATED(
    "Transverse Mercator (South Orientated)");
constexpr int EPSG_CODE_METHOD_TRANSVERSE_MERCATOR_SOUTH_ORIENTATED = 9808;

static const std::string
    PROJ_WKT2_NAME_METHOD_TWO_POINT_EQUIDISTANT("Two Point Equidistant");

static const std::string EPSG_NAME_METHOD_LAMBERT_CONIC_CONFORMAL_1SP(
    "Lambert Conic Conformal (1SP)");
constexpr int EPSG_CODE_METHOD_LAMBERT_CONIC_CONFORMAL_1SP = 9801;

static const std::string EPSG_NAME_METHOD_NZMG("New Zealand Map Grid");
constexpr int EPSG_CODE_METHOD_NZMG = 9811;

static const std::string
    EPSG_NAME_METHOD_TUNISIA_MAPPING_GRID("Tunisia Mapping Grid");
constexpr int EPSG_CODE_METHOD_TUNISIA_MAPPING_GRID = 9816;

static const std::string
    EPSG_NAME_METHOD_ALBERS_EQUAL_AREA("Albers Equal Area");
constexpr int EPSG_CODE_METHOD_ALBERS_EQUAL_AREA = 9822;

static const std::string EPSG_NAME_METHOD_LAMBERT_CONIC_CONFORMAL_2SP(
    "Lambert Conic Conformal (2SP)");
constexpr int EPSG_CODE_METHOD_LAMBERT_CONIC_CONFORMAL_2SP = 9802;

static const std::string EPSG_NAME_METHOD_LAMBERT_CONIC_CONFORMAL_2SP_BELGIUM(
    "Lambert Conic Conformal (2SP Belgium)");
constexpr int EPSG_CODE_METHOD_LAMBERT_CONIC_CONFORMAL_2SP_BELGIUM = 9803;

static const std::string EPSG_NAME_METHOD_MODIFIED_AZIMUTHAL_EQUIDISTANT(
    "Modified Azimuthal Equidistant");
constexpr int EPSG_CODE_METHOD_MODIFIED_AZIMUTHAL_EQUIDISTANT = 9832;

static const std::string EPSG_NAME_METHOD_GUAM_PROJECTION("Guam Projection");
constexpr int EPSG_CODE_METHOD_GUAM_PROJECTION = 9831;

static const std::string EPSG_NAME_METHOD_BONNE("Bonne");
constexpr int EPSG_CODE_METHOD_BONNE = 9827;

static const std::string
    EPSG_NAME_METHOD_LAMBERT_CYLINDRICAL_EQUAL_AREA_SPHERICAL(
        "Lambert Cylindrical Equal Area (Spherical)");
constexpr int EPSG_CODE_METHOD_LAMBERT_CYLINDRICAL_EQUAL_AREA_SPHERICAL = 9834;

static const std::string EPSG_NAME_METHOD_LAMBERT_CYLINDRICAL_EQUAL_AREA(
    "Lambert Cylindrical Equal Area");
constexpr int EPSG_CODE_METHOD_LAMBERT_CYLINDRICAL_EQUAL_AREA = 9835;

static const std::string EPSG_NAME_METHOD_CASSINI_SOLDNER("Cassini-Soldner");
constexpr int EPSG_CODE_METHOD_CASSINI_SOLDNER = 9806;

static const std::string
    PROJ_WKT2_NAME_METHOD_EQUIDISTANT_CONIC("Equidistant Conic");

static const std::string PROJ_WKT2_NAME_METHOD_ECKERT_I("Eckert I");

static const std::string PROJ_WKT2_NAME_METHOD_ECKERT_II("Eckert II");

static const std::string PROJ_WKT2_NAME_METHOD_ECKERT_III("Eckert III");

static const std::string PROJ_WKT2_NAME_METHOD_ECKERT_IV("Eckert IV");

static const std::string PROJ_WKT2_NAME_METHOD_ECKERT_V("Eckert V");

static const std::string PROJ_WKT2_NAME_METHOD_ECKERT_VI("Eckert VI");

static const std::string EPSG_NAME_METHOD_EQUIDISTANT_CYLINDRICAL_SPHERICAL(
    "Equidistant Cylindrical (Spherical)");
constexpr int EPSG_CODE_METHOD_EQUIDISTANT_CYLINDRICAL_SPHERICAL = 1019;

static const std::string PROJ_WKT2_NAME_METHOD_GALL("Gall");

static const std::string
    PROJ_WKT2_NAME_METHOD_GOODE_HOMOLOSINE("Goode Homolosine");

static const std::string PROJ_WKT2_NAME_METHOD_INTERRUPTED_GOODE_HOMOLOSINE(
    "Interrupted Goode Homolosine");

static const std::string PROJ_WKT2_NAME_METHOD_GEOSTATIONARY_SATELLITE_SWEEP_X(
    "Geostationary Satellite (Sweep X)");

static const std::string PROJ_WKT2_NAME_METHOD_GEOSTATIONARY_SATELLITE_SWEEP_Y(
    "Geostationary Satellite (Sweep Y)");

static const std::string
    PROJ_WKT2_NAME_METHOD_GAUSS_SCHREIBER_TRANSVERSE_MERCATOR(
        "Gauss Schreiber Transverse Mercator");
// ---------------------------------------------------------------------------

static const std::string EPSG_NAME_PARAMETER_LATITUDE_OF_NATURAL_ORIGIN(
    "Latitude of natural origin");
constexpr int EPSG_CODE_PARAMETER_LATITUDE_OF_NATURAL_ORIGIN = 8801;

static const std::string EPSG_NAME_PARAMETER_LONGITUDE_OF_NATURAL_ORIGIN(
    "Longitude of natural origin");
constexpr int EPSG_CODE_PARAMETER_LONGITUDE_OF_NATURAL_ORIGIN = 8802;

static const std::string EPSG_NAME_PARAMETER_SCALE_FACTOR_AT_NATURAL_ORIGIN(
    "Scale factor at natural origin");
constexpr int EPSG_CODE_PARAMETER_SCALE_FACTOR_AT_NATURAL_ORIGIN = 8805;

static const std::string EPSG_NAME_PARAMETER_FALSE_EASTING("False easting");
constexpr int EPSG_CODE_PARAMETER_FALSE_EASTING = 8806;

static const std::string EPSG_NAME_PARAMETER_FALSE_NORTHING("False northing");
constexpr int EPSG_CODE_PARAMETER_FALSE_NORTHING = 8807;

static const std::string
    EPSG_NAME_PARAMETER_LATITUDE_FALSE_ORIGIN("Latitude of false origin");
constexpr int EPSG_CODE_PARAMETER_LATITUDE_FALSE_ORIGIN = 8821;

static const std::string
    EPSG_NAME_PARAMETER_LONGITUDE_FALSE_ORIGIN("Longitude of false origin");
constexpr int EPSG_CODE_PARAMETER_LONGITUDE_FALSE_ORIGIN = 8822;

static const std::string EPSG_NAME_PARAMETER_LATITUDE_1ST_STD_PARALLEL(
    "Latitude of 1st standard parallel");
constexpr int EPSG_CODE_PARAMETER_LATITUDE_1ST_STD_PARALLEL = 8823;

static const std::string EPSG_NAME_PARAMETER_LATITUDE_2ND_STD_PARALLEL(
    "Latitude of 2nd standard parallel");
constexpr int EPSG_CODE_PARAMETER_LATITUDE_2ND_STD_PARALLEL = 8824;

static const std::string
    EPSG_NAME_PARAMETER_EASTING_FALSE_ORIGIN("Easting of false origin");
constexpr int EPSG_CODE_PARAMETER_EASTING_FALSE_ORIGIN = 8826;

static const std::string
    EPSG_NAME_PARAMETER_NORTHING_FALSE_ORIGIN("Northing of false origin");
constexpr int EPSG_CODE_PARAMETER_NORTHING_FALSE_ORIGIN = 8827;

static const std::string WKT1_LATITUDE_OF_ORIGIN("latitude_of_origin");
static const std::string WKT1_CENTRAL_MERIDIAN("central_meridian");
static const std::string WKT1_SCALE_FACTOR("scale_factor");
static const std::string WKT1_FALSE_EASTING("false_easting");
static const std::string WKT1_FALSE_NORTHING("false_northing");
static const std::string WKT1_STANDARD_PARALLEL_1("standard_parallel_1");

constexpr double UTM_LATITUDE_OF_NATURAL_ORIGIN = 0.0;
constexpr double UTM_SCALE_FACTOR = 0.9996;
constexpr double UTM_FALSE_EASTING = 500000.0;
constexpr double UTM_NORTH_FALSE_NORTHING = 0.0;
constexpr double UTM_SOUTH_FALSE_NORTHING = 10000000.0;

// ---------------------------------------------------------------------------

static const std::string EPSG_NAME_METHOD_COORDINATE_FRAME_GEOCENTRIC(
    "Coordinate Frame rotation (geocentric domain)");
constexpr int EPSG_CODE_METHOD_COORDINATE_FRAME_GEOCENTRIC = 1032;
constexpr int EPSG_CODE_METHOD_COORDINATE_FRAME_GEOGRAPHIC = 9607;

static const std::string EPSG_NAME_METHOD_POSITION_VECTOR_GEOCENTRIC(
    "Position Vector transformation (geocentric domain)");
constexpr int EPSG_CODE_METHOD_POSITION_VECTOR_GEOCENTRIC = 1033;
constexpr int EPSG_CODE_METHOD_POSITION_VECTOR_GEOGRAPHIC = 9606;

static const std::string EPSG_NAME_METHOD_GEOCENTRIC_TRANSLATION_GEOCENTRIC(
    "Geocentric translations (geocentric domain)");
constexpr int EPSG_CODE_METHOD_GEOCENTRIC_TRANSLATION_GEOCENTRIC = 1031;
constexpr int EPSG_CODE_METHOD_GEOCENTRIC_TRANSLATION_GEOGRAPHIC = 9603;

constexpr int EPSG_CODE_PARAMETER_X_AXIS_TRANSLATION = 8605;
constexpr int EPSG_CODE_PARAMETER_Y_AXIS_TRANSLATION = 8606;
constexpr int EPSG_CODE_PARAMETER_Z_AXIS_TRANSLATION = 8607;
constexpr int EPSG_CODE_PARAMETER_X_AXIS_ROTATION = 8608;
constexpr int EPSG_CODE_PARAMETER_Y_AXIS_ROTATION = 8609;
constexpr int EPSG_CODE_PARAMETER_Z_AXIS_ROTATION = 8610;
constexpr int EPSG_CODE_PARAMETER_SCALE_DIFFERENCE = 8611;

static const std::string
    EPSG_NAME_PARAMETER_X_AXIS_TRANSLATION("X-axis translation");
static const std::string
    EPSG_NAME_PARAMETER_Y_AXIS_TRANSLATION("Y-axis translation");
static const std::string
    EPSG_NAME_PARAMETER_Z_AXIS_TRANSLATION("Z-axis translation");
static const std::string EPSG_NAME_PARAMETER_X_AXIS_ROTATION("X-axis rotation");
static const std::string EPSG_NAME_PARAMETER_Y_AXIS_ROTATION("Y-axis rotation");
static const std::string EPSG_NAME_PARAMETER_Z_AXIS_ROTATION("Z-axis rotation");
static const std::string
    EPSG_NAME_PARAMETER_SCALE_DIFFERENCE("Scale difference");

// ---------------------------------------------------------------------------

constexpr int EPSG_CODE_METHOD_NTV2 = 9615;
static const std::string EPSG_NAME_METHOD_NTV2("NTv2");

constexpr int EPSG_CODE_PARAMETER_NTV2_FILENAME = 8656;
static const std::string
    EPSG_NAME_PARAMETER_NTV2_FILENAME("Latitude and longitude difference file");

static const std::string EPSG_NAME_PARAMETER_GEOID_CORRECTION_FILENAME(
    "Geoid (height correction) model file");
constexpr int EPSG_CODE_PARAMETER_GEOID_CORRECTION_FILENAME = 8666;

// ---------------------------------------------------------------------------

static const std::string PROJ_WKT2_NAME_METHOD_HEIGHT_TO_GEOG3D(
    "GravityRelatedHeight to Geographic3D");

// ---------------------------------------------------------------------------

constexpr int EPSG_CODE_METHOD_VERTCON = 9658;
static const std::string EPSG_NAME_METHOD_VERTCON("VERTCON");

static const std::string
    EPSG_NAME_PARAMETER_VERTICAL_OFFSET_FILE("Vertical offset file");
constexpr int EPSG_CODE_PARAMETER_VERTICAL_OFFSET_FILE = 8732;

// ---------------------------------------------------------------------------

static const ParamMapping paramLatitudeNatOrigin = {
    EPSG_NAME_PARAMETER_LATITUDE_OF_NATURAL_ORIGIN,
    EPSG_CODE_PARAMETER_LATITUDE_OF_NATURAL_ORIGIN, WKT1_LATITUDE_OF_ORIGIN,
    common::UnitOfMeasure::Type::ANGULAR, "lat_0"};

static const ParamMapping paramLongitudeNatOrigin = {
    EPSG_NAME_PARAMETER_LONGITUDE_OF_NATURAL_ORIGIN,
    EPSG_CODE_PARAMETER_LONGITUDE_OF_NATURAL_ORIGIN, WKT1_CENTRAL_MERIDIAN,
    common::UnitOfMeasure::Type::ANGULAR, "lon_0"};

static const ParamMapping paramScaleFactor = {
    EPSG_NAME_PARAMETER_SCALE_FACTOR_AT_NATURAL_ORIGIN,
    EPSG_CODE_PARAMETER_SCALE_FACTOR_AT_NATURAL_ORIGIN, WKT1_SCALE_FACTOR,
    common::UnitOfMeasure::Type::SCALE, "k_0"};

static const ParamMapping paramFalseEasting = {
    EPSG_NAME_PARAMETER_FALSE_EASTING, EPSG_CODE_PARAMETER_FALSE_EASTING,
    WKT1_FALSE_EASTING, common::UnitOfMeasure::Type::LINEAR, "x_0"};

static const ParamMapping paramFalseNorthing = {
    EPSG_NAME_PARAMETER_FALSE_NORTHING, EPSG_CODE_PARAMETER_FALSE_NORTHING,
    WKT1_FALSE_NORTHING, common::UnitOfMeasure::Type::LINEAR, "y_0"};

static const ParamMapping paramLatitudeFalseOrigin = {
    EPSG_NAME_PARAMETER_LATITUDE_FALSE_ORIGIN,
    EPSG_CODE_PARAMETER_LATITUDE_FALSE_ORIGIN, WKT1_LATITUDE_OF_ORIGIN,
    common::UnitOfMeasure::Type::ANGULAR, "lat_0"};

static const ParamMapping paramLongitudeFalseOrigin = {
    EPSG_NAME_PARAMETER_LONGITUDE_FALSE_ORIGIN,
    EPSG_CODE_PARAMETER_LONGITUDE_FALSE_ORIGIN, WKT1_CENTRAL_MERIDIAN,
    common::UnitOfMeasure::Type::ANGULAR, "lon_0"};

static const ParamMapping paramFalseEastingOrigin = {
    EPSG_NAME_PARAMETER_EASTING_FALSE_ORIGIN,
    EPSG_CODE_PARAMETER_EASTING_FALSE_ORIGIN, WKT1_FALSE_EASTING,
    common::UnitOfMeasure::Type::LINEAR, "x_0"};

static const ParamMapping paramFalseNorthingOrigin = {
    EPSG_NAME_PARAMETER_NORTHING_FALSE_ORIGIN,
    EPSG_CODE_PARAMETER_NORTHING_FALSE_ORIGIN, WKT1_FALSE_NORTHING,
    common::UnitOfMeasure::Type::LINEAR, "y_0"};

static const ParamMapping paramLatitude1stStdParallel = {
    EPSG_NAME_PARAMETER_LATITUDE_1ST_STD_PARALLEL,
    EPSG_CODE_PARAMETER_LATITUDE_1ST_STD_PARALLEL, WKT1_STANDARD_PARALLEL_1,
    common::UnitOfMeasure::Type::ANGULAR, "lat_1"};

static const ParamMapping paramLatitude2ndStdParallel = {
    EPSG_NAME_PARAMETER_LATITUDE_2ND_STD_PARALLEL,
    EPSG_CODE_PARAMETER_LATITUDE_2ND_STD_PARALLEL, "standard_parallel_2",
    common::UnitOfMeasure::Type::ANGULAR, "lat_2"};

static const MethodMapping methodMappings[] = {
    {EPSG_NAME_METHOD_TRANSVERSE_MERCATOR,
     EPSG_CODE_METHOD_TRANSVERSE_MERCATOR,
     "Transverse_Mercator",
     "tmerc",
     {
         paramLatitudeNatOrigin, paramLongitudeNatOrigin, paramScaleFactor,
         paramFalseEasting, paramFalseNorthing,
     }},

    {EPSG_NAME_METHOD_TRANSVERSE_MERCATOR_SOUTH_ORIENTATED,
     EPSG_CODE_METHOD_TRANSVERSE_MERCATOR_SOUTH_ORIENTATED,
     "Transverse_Mercator_South_Orientated",
     {"tmerc", "axis=wsu"},
     {
         paramLatitudeNatOrigin, paramLongitudeNatOrigin, paramScaleFactor,
         paramFalseEasting, paramFalseNorthing,
     }},

    {PROJ_WKT2_NAME_METHOD_TWO_POINT_EQUIDISTANT,
     0,
     "Two_Point_Equidistant",
     "tpeqd",
     {
         {"Latitude of first point", 0, "Latitude_Of_1st_Point",
          common::UnitOfMeasure::Type::ANGULAR, "lat_1"},
         {"Longitude of first point", 0, "Longitude_Of_1st_Point",
          common::UnitOfMeasure::Type::ANGULAR, "lon_1"},
         {"Latitude of second point", 0, "Latitude_Of_2nd_Point",
          common::UnitOfMeasure::Type::ANGULAR, "lat_2"},
         {"Longitude of second point", 0, "Longitude_Of_2nd_Point",
          common::UnitOfMeasure::Type::ANGULAR, "lon_2"},
         paramFalseEasting,
         paramFalseNorthing,
     }},

    {EPSG_NAME_METHOD_TUNISIA_MAPPING_GRID,
     EPSG_CODE_METHOD_TUNISIA_MAPPING_GRID,
     "Tunisia_Mapping_Grid",
     "", // no proj equivalent
     {
         paramLatitudeFalseOrigin, paramLongitudeFalseOrigin,
         paramFalseEastingOrigin, paramFalseNorthingOrigin,
     }},

    {EPSG_NAME_METHOD_ALBERS_EQUAL_AREA,
     EPSG_CODE_METHOD_ALBERS_EQUAL_AREA,
     "Albers_Conic_Equal_Area",
     "aea",
     {
         {EPSG_NAME_PARAMETER_LATITUDE_FALSE_ORIGIN,
          EPSG_CODE_PARAMETER_LATITUDE_FALSE_ORIGIN, "latitude_of_center",
          common::UnitOfMeasure::Type::ANGULAR, "lat_0"},
         {EPSG_NAME_PARAMETER_LONGITUDE_FALSE_ORIGIN,
          EPSG_CODE_PARAMETER_LONGITUDE_FALSE_ORIGIN, "longitude_of_center",
          common::UnitOfMeasure::Type::ANGULAR, "lon_0"},

         paramLatitude1stStdParallel,
         paramLatitude2ndStdParallel,
         paramFalseEastingOrigin,
         paramFalseNorthingOrigin,
     }},

    {EPSG_NAME_METHOD_LAMBERT_CONIC_CONFORMAL_1SP,
     EPSG_CODE_METHOD_LAMBERT_CONIC_CONFORMAL_1SP,
     "Lambert_Conformal_Conic_1SP",
     "lcc",
     {
         {EPSG_NAME_PARAMETER_LATITUDE_OF_NATURAL_ORIGIN,
          EPSG_CODE_PARAMETER_LATITUDE_OF_NATURAL_ORIGIN,
          WKT1_LATITUDE_OF_ORIGIN,
          common::UnitOfMeasure::Type::ANGULAR,
          {"lat_1", "lat_0"}},
         paramLongitudeNatOrigin,
         paramScaleFactor,
         paramFalseEasting,
         paramFalseNorthing,
     }},

    {EPSG_NAME_METHOD_LAMBERT_CONIC_CONFORMAL_2SP,
     EPSG_CODE_METHOD_LAMBERT_CONIC_CONFORMAL_2SP,
     "Lambert_Conformal_Conic_2SP",
     "lcc",
     {
         paramLatitudeFalseOrigin, paramLongitudeFalseOrigin,
         paramLatitude1stStdParallel, paramLatitude2ndStdParallel,
         paramFalseEastingOrigin, paramFalseNorthingOrigin,
     }},

    {EPSG_NAME_METHOD_LAMBERT_CONIC_CONFORMAL_2SP_BELGIUM,
     EPSG_CODE_METHOD_LAMBERT_CONIC_CONFORMAL_2SP_BELGIUM,
     "Lambert_Conformal_Conic_2SP_Belgium",
     "lcc", // FIXME: this is what is done in GDAL, but the formula of LCC 2SP
            // Belgium in the EPSG 7.2 guidance is difference from the regular
            // LCC 2SP
     {
         paramLatitudeFalseOrigin, paramLongitudeFalseOrigin,
         paramLatitude1stStdParallel, paramLatitude2ndStdParallel,
         paramFalseEastingOrigin, paramFalseNorthingOrigin,
     }},

    {EPSG_NAME_METHOD_MODIFIED_AZIMUTHAL_EQUIDISTANT,
     EPSG_CODE_METHOD_MODIFIED_AZIMUTHAL_EQUIDISTANT,
     "Azimuthal_Equidistant",
     "aeqd",
     {
         {EPSG_NAME_PARAMETER_LATITUDE_OF_NATURAL_ORIGIN,
          EPSG_CODE_PARAMETER_LATITUDE_OF_NATURAL_ORIGIN, "latitude_of_center",
          common::UnitOfMeasure::Type::ANGULAR, "lat_0"},
         {EPSG_NAME_PARAMETER_LONGITUDE_OF_NATURAL_ORIGIN,
          EPSG_CODE_PARAMETER_LONGITUDE_OF_NATURAL_ORIGIN,
          "longitude_of_center", common::UnitOfMeasure::Type::ANGULAR, "lon_0"},
         paramFalseEasting,
         paramFalseNorthing,
     }},

    {EPSG_NAME_METHOD_GUAM_PROJECTION,
     EPSG_CODE_METHOD_GUAM_PROJECTION,
     "", // no mapping to GDAL WKT1
     {"aeqd", "guam"},
     {
         paramLatitudeNatOrigin, paramLongitudeNatOrigin, paramFalseEasting,
         paramFalseNorthing,
     }},

    {EPSG_NAME_METHOD_BONNE,
     EPSG_CODE_METHOD_BONNE,
     "Bonne",
     "bonne",
     {
         {EPSG_NAME_PARAMETER_LATITUDE_OF_NATURAL_ORIGIN,
          EPSG_CODE_PARAMETER_LATITUDE_OF_NATURAL_ORIGIN,
          WKT1_STANDARD_PARALLEL_1, common::UnitOfMeasure::Type::ANGULAR,
          "lat_1"},
         paramLongitudeNatOrigin,
         paramFalseEasting,
         paramFalseNorthing,
     }},

    {EPSG_NAME_METHOD_LAMBERT_CYLINDRICAL_EQUAL_AREA_SPHERICAL,
     EPSG_CODE_METHOD_LAMBERT_CYLINDRICAL_EQUAL_AREA_SPHERICAL,
     "Cylindrical_Equal_Area",
     "cea",
     {
         {EPSG_NAME_PARAMETER_LATITUDE_1ST_STD_PARALLEL,
          EPSG_CODE_PARAMETER_LATITUDE_1ST_STD_PARALLEL,
          WKT1_STANDARD_PARALLEL_1, common::UnitOfMeasure::Type::ANGULAR,
          "lat_ts"},
         paramLongitudeNatOrigin,
         paramFalseEasting,
         paramFalseNorthing,
     }},

    {EPSG_NAME_METHOD_LAMBERT_CYLINDRICAL_EQUAL_AREA,
     EPSG_CODE_METHOD_LAMBERT_CYLINDRICAL_EQUAL_AREA,
     "Cylindrical_Equal_Area",
     "cea",
     {
         {EPSG_NAME_PARAMETER_LATITUDE_1ST_STD_PARALLEL,
          EPSG_CODE_PARAMETER_LATITUDE_1ST_STD_PARALLEL,
          WKT1_STANDARD_PARALLEL_1, common::UnitOfMeasure::Type::ANGULAR,
          "lat_ts"},
         paramLongitudeNatOrigin,
         paramFalseEasting,
         paramFalseNorthing,
     }},

    {EPSG_NAME_METHOD_CASSINI_SOLDNER,
     EPSG_CODE_METHOD_CASSINI_SOLDNER,
     "Cassini_Soldner",
     "cass",
     {
         paramLatitudeNatOrigin, paramLongitudeNatOrigin, paramFalseEasting,
         paramFalseNorthing,
     }},

    {PROJ_WKT2_NAME_METHOD_EQUIDISTANT_CONIC,
     0,
     "Equidistant_Conic",
     "eqdc",
     {
         {EPSG_NAME_PARAMETER_LATITUDE_OF_NATURAL_ORIGIN,
          EPSG_CODE_PARAMETER_LATITUDE_OF_NATURAL_ORIGIN, "latitude_of_center",
          common::UnitOfMeasure::Type::ANGULAR, "lat_0"},
         {EPSG_NAME_PARAMETER_LONGITUDE_OF_NATURAL_ORIGIN,
          EPSG_CODE_PARAMETER_LONGITUDE_OF_NATURAL_ORIGIN,
          "longitude_of_center", common::UnitOfMeasure::Type::ANGULAR, "lon_0"},
         paramLatitude1stStdParallel,
         paramLatitude2ndStdParallel,
         paramFalseEasting,
         paramFalseNorthing,
     }},

    {PROJ_WKT2_NAME_METHOD_ECKERT_I,
     0,
     "Eckert_I",
     "eck1",
     {
         paramLongitudeNatOrigin, paramFalseEasting, paramFalseNorthing,
     }},

    {PROJ_WKT2_NAME_METHOD_ECKERT_II,
     0,
     "Eckert_II",
     "eck2",
     {
         paramLongitudeNatOrigin, paramFalseEasting, paramFalseNorthing,
     }},

    {PROJ_WKT2_NAME_METHOD_ECKERT_III,
     0,
     "Eckert_III",
     "eck3",
     {
         paramLongitudeNatOrigin, paramFalseEasting, paramFalseNorthing,
     }},

    {PROJ_WKT2_NAME_METHOD_ECKERT_IV,
     0,
     "Eckert_IV",
     "eck4",
     {
         paramLongitudeNatOrigin, paramFalseEasting, paramFalseNorthing,
     }},

    {PROJ_WKT2_NAME_METHOD_ECKERT_V,
     0,
     "Eckert_V",
     "eck5",
     {
         paramLongitudeNatOrigin, paramFalseEasting, paramFalseNorthing,
     }},

    {PROJ_WKT2_NAME_METHOD_ECKERT_VI,
     0,
     "Eckert_VI",
     "eck6",
     {
         paramLongitudeNatOrigin, paramFalseEasting, paramFalseNorthing,
     }},

    {EPSG_NAME_METHOD_EQUIDISTANT_CYLINDRICAL_SPHERICAL,
     EPSG_CODE_METHOD_EQUIDISTANT_CYLINDRICAL_SPHERICAL,
     "Equirectangular",
     "eqc",
     {
         {EPSG_NAME_PARAMETER_LATITUDE_1ST_STD_PARALLEL,
          EPSG_CODE_PARAMETER_LATITUDE_1ST_STD_PARALLEL,
          WKT1_STANDARD_PARALLEL_1, common::UnitOfMeasure::Type::ANGULAR,
          "lat_ts"},
         paramLongitudeNatOrigin,
         paramFalseEasting,
         paramFalseNorthing,
     }},

    {PROJ_WKT2_NAME_METHOD_GALL,
     0,
     "Gall_Stereographic",
     "gall",
     {
         paramLongitudeNatOrigin, paramFalseEasting, paramFalseNorthing,
     }},

    {PROJ_WKT2_NAME_METHOD_GOODE_HOMOLOSINE,
     0,
     "Goode_Homolosine",
     "goode",
     {
         paramLongitudeNatOrigin, paramFalseEasting, paramFalseNorthing,
     }},

    {PROJ_WKT2_NAME_METHOD_INTERRUPTED_GOODE_HOMOLOSINE,
     0,
     "Interrupted_Goode_Homolosine",
     "igh",
     {
         paramLongitudeNatOrigin, paramFalseEasting, paramFalseNorthing,
     }},

    // No WKT1 representation fr sweep=x
    {PROJ_WKT2_NAME_METHOD_GEOSTATIONARY_SATELLITE_SWEEP_X,
     0,
     "",
     {"geos", "sweep=x"},
     {
         paramLongitudeNatOrigin,
         {"Satellite Height", 0, "satellite_height",
          common::UnitOfMeasure::Type::LINEAR, "h"},
         paramFalseEasting,
         paramFalseNorthing,
     }},

    {PROJ_WKT2_NAME_METHOD_GEOSTATIONARY_SATELLITE_SWEEP_Y,
     0,
     "Geostationary_Satellite",
     {"geos"},
     {
         paramLongitudeNatOrigin,
         {"Satellite Height", 0, "satellite_height",
          common::UnitOfMeasure::Type::LINEAR, "h"},
         paramFalseEasting,
         paramFalseNorthing,
     }},

    {PROJ_WKT2_NAME_METHOD_GAUSS_SCHREIBER_TRANSVERSE_MERCATOR,
     0,
     "Gauss_Schreiber_Transverse_Mercator",
     "gstmerc",
     {
         paramLatitudeNatOrigin, paramLongitudeNatOrigin, paramScaleFactor,
         paramFalseEasting, paramFalseNorthing,
     }},

    {EPSG_NAME_METHOD_NZMG,
     EPSG_CODE_METHOD_NZMG,
     "New_Zealand_Map_Grid",
     "nzmg",
     {
         paramLatitudeNatOrigin, paramLongitudeNatOrigin, paramFalseEasting,
         paramFalseNorthing,
     }},
    // TODO: add at least all GDAL supported methods !!!
};

//! @endcond

// ---------------------------------------------------------------------------

} // namespace operation
NS_PROJ_END

#endif // COORDINATEOPERATION_CONSTANTS_HH_INCLUDED
