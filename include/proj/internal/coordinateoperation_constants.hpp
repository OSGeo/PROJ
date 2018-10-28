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

#include "coordinateoperation_internal.hpp"
#include <string>

//! @cond Doxygen_Suppress

// ---------------------------------------------------------------------------

NS_PROJ_START
namespace operation {

static const char *EPSG_NAME_METHOD_TRANSVERSE_MERCATOR("Transverse Mercator");
constexpr int EPSG_CODE_METHOD_TRANSVERSE_MERCATOR = 9807;

static const char *EPSG_NAME_METHOD_TRANSVERSE_MERCATOR_SOUTH_ORIENTATED(
    "Transverse Mercator (South Orientated)");
constexpr int EPSG_CODE_METHOD_TRANSVERSE_MERCATOR_SOUTH_ORIENTATED = 9808;

static const char *
    PROJ_WKT2_NAME_METHOD_TWO_POINT_EQUIDISTANT("Two Point Equidistant");

static const char *EPSG_NAME_METHOD_LAMBERT_CONIC_CONFORMAL_1SP(
    "Lambert Conic Conformal (1SP)");
constexpr int EPSG_CODE_METHOD_LAMBERT_CONIC_CONFORMAL_1SP = 9801;

static const char *EPSG_NAME_METHOD_NZMG("New Zealand Map Grid");
constexpr int EPSG_CODE_METHOD_NZMG = 9811;

static const char *
    EPSG_NAME_METHOD_TUNISIA_MAPPING_GRID("Tunisia Mapping Grid");
constexpr int EPSG_CODE_METHOD_TUNISIA_MAPPING_GRID = 9816;

static const char *EPSG_NAME_METHOD_ALBERS_EQUAL_AREA("Albers Equal Area");
constexpr int EPSG_CODE_METHOD_ALBERS_EQUAL_AREA = 9822;

static const char *EPSG_NAME_METHOD_LAMBERT_CONIC_CONFORMAL_2SP(
    "Lambert Conic Conformal (2SP)");
constexpr int EPSG_CODE_METHOD_LAMBERT_CONIC_CONFORMAL_2SP = 9802;

static const char *EPSG_NAME_METHOD_LAMBERT_CONIC_CONFORMAL_2SP_BELGIUM(
    "Lambert Conic Conformal (2SP Belgium)");
constexpr int EPSG_CODE_METHOD_LAMBERT_CONIC_CONFORMAL_2SP_BELGIUM = 9803;

static const char *EPSG_NAME_METHOD_LAMBERT_CONIC_CONFORMAL_2SP_MICHIGAN(
    "Lambert Conic Conformal (2SP Michigan)");
constexpr int EPSG_CODE_METHOD_LAMBERT_CONIC_CONFORMAL_2SP_MICHIGAN = 1051;

static const char *EPSG_NAME_METHOD_MODIFIED_AZIMUTHAL_EQUIDISTANT(
    "Modified Azimuthal Equidistant");
constexpr int EPSG_CODE_METHOD_MODIFIED_AZIMUTHAL_EQUIDISTANT = 9832;

static const char *EPSG_NAME_METHOD_GUAM_PROJECTION("Guam Projection");
constexpr int EPSG_CODE_METHOD_GUAM_PROJECTION = 9831;

static const char *EPSG_NAME_METHOD_BONNE("Bonne");
constexpr int EPSG_CODE_METHOD_BONNE = 9827;

static const char *EPSG_NAME_METHOD_LAMBERT_CYLINDRICAL_EQUAL_AREA_SPHERICAL(
    "Lambert Cylindrical Equal Area (Spherical)");
constexpr int EPSG_CODE_METHOD_LAMBERT_CYLINDRICAL_EQUAL_AREA_SPHERICAL = 9834;

static const char *EPSG_NAME_METHOD_LAMBERT_CYLINDRICAL_EQUAL_AREA(
    "Lambert Cylindrical Equal Area");
constexpr int EPSG_CODE_METHOD_LAMBERT_CYLINDRICAL_EQUAL_AREA = 9835;

static const char *EPSG_NAME_METHOD_CASSINI_SOLDNER("Cassini-Soldner");
constexpr int EPSG_CODE_METHOD_CASSINI_SOLDNER = 9806;

static const char *PROJ_WKT2_NAME_METHOD_EQUIDISTANT_CONIC("Equidistant Conic");

static const char *PROJ_WKT2_NAME_METHOD_ECKERT_I("Eckert I");

static const char *PROJ_WKT2_NAME_METHOD_ECKERT_II("Eckert II");

static const char *PROJ_WKT2_NAME_METHOD_ECKERT_III("Eckert III");

static const char *PROJ_WKT2_NAME_METHOD_ECKERT_IV("Eckert IV");

static const char *PROJ_WKT2_NAME_METHOD_ECKERT_V("Eckert V");

static const char *PROJ_WKT2_NAME_METHOD_ECKERT_VI("Eckert VI");

static const char *
    EPSG_NAME_METHOD_EQUIDISTANT_CYLINDRICAL("Equidistant Cylindrical");
constexpr int EPSG_CODE_METHOD_EQUIDISTANT_CYLINDRICAL = 1028;

static const char *EPSG_NAME_METHOD_EQUIDISTANT_CYLINDRICAL_SPHERICAL(
    "Equidistant Cylindrical (Spherical)");
constexpr int EPSG_CODE_METHOD_EQUIDISTANT_CYLINDRICAL_SPHERICAL = 1029;

static const char *PROJ_WKT2_NAME_METHOD_GALL("Gall");

static const char *PROJ_WKT2_NAME_METHOD_GOODE_HOMOLOSINE("Goode Homolosine");

static const char *PROJ_WKT2_NAME_METHOD_INTERRUPTED_GOODE_HOMOLOSINE(
    "Interrupted Goode Homolosine");

static const char *PROJ_WKT2_NAME_METHOD_GEOSTATIONARY_SATELLITE_SWEEP_X(
    "Geostationary Satellite (Sweep X)");

static const char *PROJ_WKT2_NAME_METHOD_GEOSTATIONARY_SATELLITE_SWEEP_Y(
    "Geostationary Satellite (Sweep Y)");

static const char *PROJ_WKT2_NAME_METHOD_GAUSS_SCHREIBER_TRANSVERSE_MERCATOR(
    "Gauss Schreiber Transverse Mercator");

static const char *PROJ_WKT2_NAME_METHOD_GNOMONIC("Gnomonic");

static const char *EPSG_NAME_METHOD_HOTINE_OBLIQUE_MERCATOR_VARIANT_A(
    "Hotine Oblique Mercator (variant A)");
constexpr int EPSG_CODE_METHOD_HOTINE_OBLIQUE_MERCATOR_VARIANT_A = 9812;

static const char *EPSG_NAME_METHOD_HOTINE_OBLIQUE_MERCATOR_VARIANT_B(
    "Hotine Oblique Mercator (variant B)");
constexpr int EPSG_CODE_METHOD_HOTINE_OBLIQUE_MERCATOR_VARIANT_B = 9815;

static const char *
    PROJ_WKT2_NAME_METHOD_HOTINE_OBLIQUE_MERCATOR_TWO_POINT_NATURAL_ORIGIN(
        "Hotine Oblique Mercator Two Point Natural Origin");

static const char *PROJ_WKT2_NAME_INTERNATIONAL_MAP_WORLD_POLYCONIC(
    "International Map of the World Polyconic");

static const char *
    EPSG_NAME_METHOD_KROVAK_NORTH_ORIENTED("Krovak (North Orientated)");
constexpr int EPSG_CODE_METHOD_KROVAK_NORTH_ORIENTED = 1041;

static const char *EPSG_NAME_METHOD_KROVAK("Krovak");
constexpr int EPSG_CODE_METHOD_KROVAK = 9819;

static const char *EPSG_NAME_METHOD_LAMBERT_AZIMUTHAL_EQUAL_AREA(
    "Lambert Azimuthal Equal Area");
constexpr int EPSG_CODE_METHOD_LAMBERT_AZIMUTHAL_EQUAL_AREA = 9820;

static const char *
    PROJ_WKT2_NAME_METHOD_MILLER_CYLINDRICAL("Miller Cylindrical");

static const char *EPSG_NAME_METHOD_MERCATOR_VARIANT_A("Mercator (variant A)");
constexpr int EPSG_CODE_METHOD_MERCATOR_VARIANT_A = 9804;

static const char *EPSG_NAME_METHOD_MERCATOR_VARIANT_B("Mercator (variant B)");
constexpr int EPSG_CODE_METHOD_MERCATOR_VARIANT_B = 9805;

static const char *EPSG_NAME_METHOD_POPULAR_VISUALISATION_PSEUDO_MERCATOR(
    "Popular Visualisation Pseudo Mercator");
constexpr int EPSG_CODE_METHOD_POPULAR_VISUALISATION_PSEUDO_MERCATOR = 1024;

static const char *PROJ_WKT2_NAME_METHOD_MOLLWEIDE("Mollweide");

static const char *
    EPSG_NAME_METHOD_OBLIQUE_STEREOGRAPHIC("Oblique Stereographic");
constexpr int EPSG_CODE_METHOD_OBLIQUE_STEREOGRAPHIC = 9809;

static const char *EPSG_NAME_METHOD_ORTHOGRAPHIC("Orthographic");
constexpr int EPSG_CODE_METHOD_ORTHOGRAPHIC = 9840;

static const char *EPSG_NAME_METHOD_AMERICAN_POLYCONIC("American Polyconic");
constexpr int EPSG_CODE_METHOD_AMERICAN_POLYCONIC = 9818;

static const char *EPSG_NAME_METHOD_POLAR_STEREOGRAPHIC_VARIANT_A(
    "Polar Stereographic (variant A)");
constexpr int EPSG_CODE_METHOD_POLAR_STEREOGRAPHIC_VARIANT_A = 9810;

static const char *EPSG_NAME_METHOD_POLAR_STEREOGRAPHIC_VARIANT_B(
    "Polar Stereographic (variant B)");
constexpr int EPSG_CODE_METHOD_POLAR_STEREOGRAPHIC_VARIANT_B = 9829;

static const char *PROJ_WKT2_NAME_METHOD_ROBINSON("Robinson");

static const char *PROJ_WKT2_NAME_METHOD_SINUSOIDAL("Sinusoidal");

static const char *PROJ_WKT2_NAME_METHOD_STEREOGRAPHIC("Stereographic");

static const char *PROJ_WKT2_NAME_METHOD_VAN_DER_GRINTEN("VanDerGrinten");

static const char *PROJ_WKT2_NAME_METHOD_WAGNER_I("Wagner I");
static const char *PROJ_WKT2_NAME_METHOD_WAGNER_II("Wagner II");
static const char *PROJ_WKT2_NAME_METHOD_WAGNER_III("Wagner III");
static const char *PROJ_WKT2_NAME_METHOD_WAGNER_IV("Wagner IV");
static const char *PROJ_WKT2_NAME_METHOD_WAGNER_V("Wagner V");
static const char *PROJ_WKT2_NAME_METHOD_WAGNER_VI("Wagner VI");
static const char *PROJ_WKT2_NAME_METHOD_WAGNER_VII("Wagner VII");

static const char *PROJ_WKT2_NAME_METHOD_QUADRILATERALIZED_SPHERICAL_CUBE(
    "Quadrilateralized Spherical Cube");

static const char *PROJ_WKT2_NAME_METHOD_SPHERICAL_CROSS_TRACK_HEIGHT(
    "Spherical Cross-Track Height");

static const char *EPSG_NAME_METHOD_EQUAL_EARTH("Equal Earth");
constexpr int EPSG_CODE_METHOD_EQUAL_EARTH = 1078;

// ---------------------------------------------------------------------------

static const char *
    EPSG_NAME_PARAMETER_COLATITUDE_CONE_AXIS("Co-latitude of cone axis");
constexpr int EPSG_CODE_PARAMETER_COLATITUDE_CONE_AXIS = 1036;

static const char *EPSG_NAME_PARAMETER_LATITUDE_OF_NATURAL_ORIGIN(
    "Latitude of natural origin");
constexpr int EPSG_CODE_PARAMETER_LATITUDE_OF_NATURAL_ORIGIN = 8801;

static const char *EPSG_NAME_PARAMETER_LONGITUDE_OF_NATURAL_ORIGIN(
    "Longitude of natural origin");
constexpr int EPSG_CODE_PARAMETER_LONGITUDE_OF_NATURAL_ORIGIN = 8802;

static const char *EPSG_NAME_PARAMETER_SCALE_FACTOR_AT_NATURAL_ORIGIN(
    "Scale factor at natural origin");
constexpr int EPSG_CODE_PARAMETER_SCALE_FACTOR_AT_NATURAL_ORIGIN = 8805;

static const char *EPSG_NAME_PARAMETER_FALSE_EASTING("False easting");
constexpr int EPSG_CODE_PARAMETER_FALSE_EASTING = 8806;

static const char *EPSG_NAME_PARAMETER_FALSE_NORTHING("False northing");
constexpr int EPSG_CODE_PARAMETER_FALSE_NORTHING = 8807;

static const char *EPSG_NAME_PARAMETER_LATITUDE_PROJECTION_CENTRE(
    "Latitude of projection centre");
constexpr int EPSG_CODE_PARAMETER_LATITUDE_PROJECTION_CENTRE = 8811;

static const char *EPSG_NAME_PARAMETER_LONGITUDE_PROJECTION_CENTRE(
    "Longitude of projection centre");
constexpr int EPSG_CODE_PARAMETER_LONGITUDE_PROJECTION_CENTRE = 8812;

static const char *
    EPSG_NAME_PARAMETER_AZIMUTH_INITIAL_LINE("Azimuth of initial line");
constexpr int EPSG_CODE_PARAMETER_AZIMUTH_INITIAL_LINE = 8813;

static const char *EPSG_NAME_PARAMETER_ANGLE_RECTIFIED_TO_SKEW_GRID(
    "Angle from Rectified to Skew Grid");
constexpr int EPSG_CODE_PARAMETER_ANGLE_RECTIFIED_TO_SKEW_GRID = 8814;

static const char *EPSG_NAME_PARAMETER_SCALE_FACTOR_INITIAL_LINE(
    "Scale factor on initial line");
constexpr int EPSG_CODE_PARAMETER_SCALE_FACTOR_INITIAL_LINE = 8815;

static const char *EPSG_NAME_PARAMETER_EASTING_PROJECTION_CENTRE(
    "Easting at projection centre");
constexpr int EPSG_CODE_PARAMETER_EASTING_PROJECTION_CENTRE = 8816;

static const char *EPSG_NAME_PARAMETER_NORTHING_PROJECTION_CENTRE(
    "Northing at projection centre");
constexpr int EPSG_CODE_PARAMETER_NORTHING_PROJECTION_CENTRE = 8817;

static const char *EPSG_NAME_PARAMETER_LATITUDE_PSEUDO_STANDARD_PARALLEL(
    "Latitude of pseudo standard parallel");
constexpr int EPSG_CODE_PARAMETER_LATITUDE_PSEUDO_STANDARD_PARALLEL = 8818;

static const char *EPSG_NAME_PARAMETER_SCALE_FACTOR_PSEUDO_STANDARD_PARALLEL(
    "Scale factor on pseudo standard parallel");
constexpr int EPSG_CODE_PARAMETER_SCALE_FACTOR_PSEUDO_STANDARD_PARALLEL = 8819;

static const char *
    EPSG_NAME_PARAMETER_LATITUDE_FALSE_ORIGIN("Latitude of false origin");
constexpr int EPSG_CODE_PARAMETER_LATITUDE_FALSE_ORIGIN = 8821;

static const char *
    EPSG_NAME_PARAMETER_LONGITUDE_FALSE_ORIGIN("Longitude of false origin");
constexpr int EPSG_CODE_PARAMETER_LONGITUDE_FALSE_ORIGIN = 8822;

static const char *EPSG_NAME_PARAMETER_LATITUDE_1ST_STD_PARALLEL(
    "Latitude of 1st standard parallel");
constexpr int EPSG_CODE_PARAMETER_LATITUDE_1ST_STD_PARALLEL = 8823;

static const char *EPSG_NAME_PARAMETER_LATITUDE_2ND_STD_PARALLEL(
    "Latitude of 2nd standard parallel");
constexpr int EPSG_CODE_PARAMETER_LATITUDE_2ND_STD_PARALLEL = 8824;

static const char *
    EPSG_NAME_PARAMETER_EASTING_FALSE_ORIGIN("Easting of false origin");
constexpr int EPSG_CODE_PARAMETER_EASTING_FALSE_ORIGIN = 8826;

static const char *
    EPSG_NAME_PARAMETER_NORTHING_FALSE_ORIGIN("Northing of false origin");
constexpr int EPSG_CODE_PARAMETER_NORTHING_FALSE_ORIGIN = 8827;

static const char *
    EPSG_NAME_PARAMETER_LATITUDE_STD_PARALLEL("Latitude of standard parallel");
constexpr int EPSG_CODE_PARAMETER_LATITUDE_STD_PARALLEL = 8832;

static const char *
    EPSG_NAME_PARAMETER_LONGITUDE_OF_ORIGIN("Longitude of origin");
constexpr int EPSG_CODE_PARAMETER_LONGITUDE_OF_ORIGIN = 8833;

static const char *
    EPSG_NAME_PARAMETER_ELLIPSOID_SCALE_FACTOR("Ellipsoid scaling factor");
constexpr int EPSG_CODE_PARAMETER_ELLIPSOID_SCALE_FACTOR = 1038;

static const char *WKT1_LATITUDE_OF_ORIGIN("latitude_of_origin");
static const char *WKT1_CENTRAL_MERIDIAN("central_meridian");
static const char *WKT1_SCALE_FACTOR("scale_factor");
static const char *WKT1_FALSE_EASTING("false_easting");
static const char *WKT1_FALSE_NORTHING("false_northing");
static const char *WKT1_STANDARD_PARALLEL_1("standard_parallel_1");

constexpr double UTM_LATITUDE_OF_NATURAL_ORIGIN = 0.0;
constexpr double UTM_SCALE_FACTOR = 0.9996;
constexpr double UTM_FALSE_EASTING = 500000.0;
constexpr double UTM_NORTH_FALSE_NORTHING = 0.0;
constexpr double UTM_SOUTH_FALSE_NORTHING = 10000000.0;

// ---------------------------------------------------------------------------

static const char *EPSG_NAME_METHOD_COORDINATE_FRAME_GEOCENTRIC(
    "Coordinate Frame rotation (geocentric domain)");
constexpr int EPSG_CODE_METHOD_COORDINATE_FRAME_GEOCENTRIC = 1032;

static const char *EPSG_NAME_METHOD_COORDINATE_FRAME_GEOGRAPHIC_2D(
    "Coordinate Frame rotation (geog2D domain)");
constexpr int EPSG_CODE_METHOD_COORDINATE_FRAME_GEOGRAPHIC_2D = 9607;

static const char *EPSG_NAME_METHOD_COORDINATE_FRAME_GEOGRAPHIC_3D(
    "Coordinate Frame rotation (geog3D domain)");
constexpr int EPSG_CODE_METHOD_COORDINATE_FRAME_GEOGRAPHIC_3D = 1038;

static const char *EPSG_NAME_METHOD_POSITION_VECTOR_GEOCENTRIC(
    "Position Vector transformation (geocentric domain)");
constexpr int EPSG_CODE_METHOD_POSITION_VECTOR_GEOCENTRIC = 1033;

static const char *EPSG_NAME_METHOD_POSITION_VECTOR_GEOGRAPHIC_2D(
    "Position Vector transformation (geog2D domain)");
constexpr int EPSG_CODE_METHOD_POSITION_VECTOR_GEOGRAPHIC_2D = 9606;

static const char *EPSG_NAME_METHOD_POSITION_VECTOR_GEOGRAPHIC_3D(
    "Position Vector transformation (geog3D domain)");
constexpr int EPSG_CODE_METHOD_POSITION_VECTOR_GEOGRAPHIC_3D = 1037;

static const char *EPSG_NAME_METHOD_GEOCENTRIC_TRANSLATION_GEOCENTRIC(
    "Geocentric translations (geocentric domain)");
constexpr int EPSG_CODE_METHOD_GEOCENTRIC_TRANSLATION_GEOCENTRIC = 1031;

static const char *EPSG_NAME_METHOD_GEOCENTRIC_TRANSLATION_GEOGRAPHIC_2D(
    "Geocentric translations (geog2D domain)");
constexpr int EPSG_CODE_METHOD_GEOCENTRIC_TRANSLATION_GEOGRAPHIC_2D = 9603;

static const char *EPSG_NAME_METHOD_GEOCENTRIC_TRANSLATION_GEOGRAPHIC_3D(
    "Geocentric translations (geog3D domain)");
constexpr int EPSG_CODE_METHOD_GEOCENTRIC_TRANSLATION_GEOGRAPHIC_3D = 1035;

static const char *EPSG_NAME_METHOD_TIME_DEPENDENT_POSITION_VECTOR_GEOCENTRIC(
    "Time-dependent Position Vector tfm (geocentric)");
constexpr int EPSG_CODE_METHOD_TIME_DEPENDENT_POSITION_VECTOR_GEOCENTRIC = 1053;

static const char *
    EPSG_NAME_METHOD_TIME_DEPENDENT_POSITION_VECTOR_GEOGRAPHIC_2D(
        "Time-dependent Position Vector tfm (geog2D)");
constexpr int EPSG_CODE_METHOD_TIME_DEPENDENT_POSITION_VECTOR_GEOGRAPHIC_2D =
    1054;

static const char *
    EPSG_NAME_METHOD_TIME_DEPENDENT_POSITION_VECTOR_GEOGRAPHIC_3D(
        "Time-dependent Position Vector tfm (geog3D)");
constexpr int EPSG_CODE_METHOD_TIME_DEPENDENT_POSITION_VECTOR_GEOGRAPHIC_3D =
    1055;

static const char *EPSG_NAME_METHOD_TIME_DEPENDENT_COORDINATE_FRAME_GEOCENTRIC(
    "Time-dependent Coordinate Frame rotation (geocen)");
constexpr int EPSG_CODE_METHOD_TIME_DEPENDENT_COORDINATE_FRAME_GEOCENTRIC =
    1056;

static const char *
    EPSG_NAME_METHOD_TIME_DEPENDENT_COORDINATE_FRAME_GEOGRAPHIC_2D(
        "Time-dependent Coordinate Frame rotation (geog2D)");
constexpr int EPSG_CODE_METHOD_TIME_DEPENDENT_COORDINATE_FRAME_GEOGRAPHIC_2D =
    1057;

static const char *
    EPSG_NAME_METHOD_TIME_DEPENDENT_COORDINATE_FRAME_GEOGRAPHIC_3D(
        "Time-dependent Coordinate Frame rotation (geog3D)");
constexpr int EPSG_CODE_METHOD_TIME_DEPENDENT_COORDINATE_FRAME_GEOGRAPHIC_3D =
    1058;

static const char *EPSG_NAME_METHOD_MOLODENSKY_BADEKAS_CF_GEOCENTRIC(
    "Molodensky-Badekas (CF geocentric domain)");
constexpr int EPSG_CODE_METHOD_MOLODENSKY_BADEKAS_CF_GEOCENTRIC = 1034;

static const char *EPSG_NAME_METHOD_MOLODENSKY_BADEKAS_PV_GEOCENTRIC(
    "Molodensky-Badekas (PV geocentric domain)");
constexpr int EPSG_CODE_METHOD_MOLODENSKY_BADEKAS_PV_GEOCENTRIC = 1061;

static const char *EPSG_NAME_METHOD_MOLODENSKY_BADEKAS_CF_GEOGRAPHIC_3D(
    "Molodensky-Badekas (CF geog3D domain)");
constexpr int EPSG_CODE_METHOD_MOLODENSKY_BADEKAS_CF_GEOGRAPHIC_3D = 1039;

static const char *EPSG_NAME_METHOD_MOLODENSKY_BADEKAS_PV_GEOGRAPHIC_3D(
    "Molodensky-Badekas (PV geog3D domain)");
constexpr int EPSG_CODE_METHOD_MOLODENSKY_BADEKAS_PV_GEOGRAPHIC_3D = 1062;

static const char *EPSG_NAME_METHOD_MOLODENSKY_BADEKAS_CF_GEOGRAPHIC_2D(
    "Molodensky-Badekas (CF geog2D domain)");
constexpr int EPSG_CODE_METHOD_MOLODENSKY_BADEKAS_CF_GEOGRAPHIC_2D = 9636;

static const char *EPSG_NAME_METHOD_MOLODENSKY_BADEKAS_PV_GEOGRAPHIC_2D(
    "Molodensky-Badekas (PV geog2D domain)");
constexpr int EPSG_CODE_METHOD_MOLODENSKY_BADEKAS_PV_GEOGRAPHIC_2D = 1063;

#include "helmert_constants.hpp"

static const char *EPSG_NAME_METHOD_MOLODENSKY("Molodensky");
constexpr int EPSG_CODE_METHOD_MOLODENSKY = 9604;

static const char *EPSG_NAME_METHOD_ABRIDGED_MOLODENSKY("Abridged Molodensky");
constexpr int EPSG_CODE_METHOD_ABRIDGED_MOLODENSKY = 9605;

constexpr int EPSG_CODE_PARAMETER_SEMI_MAJOR_AXIS_DIFFERENCE = 8654;
constexpr int EPSG_CODE_PARAMETER_FLATTENING_DIFFERENCE = 8655;

static const char *EPSG_NAME_PARAMETER_SEMI_MAJOR_AXIS_DIFFERENCE(
    "Semi-major axis length difference");
static const char *
    EPSG_NAME_PARAMETER_FLATTENING_DIFFERENCE("Flattening difference");

// ---------------------------------------------------------------------------

constexpr int EPSG_CODE_METHOD_NTV1 = 9614;
static const char *EPSG_NAME_METHOD_NTV1("NTv1");

constexpr int EPSG_CODE_METHOD_NTV2 = 9615;
static const char *EPSG_NAME_METHOD_NTV2("NTv2");

constexpr int EPSG_CODE_PARAMETER_LATITUDE_LONGITUDE_DIFFERENCE_FILE = 8656;
static const char *EPSG_NAME_PARAMETER_LATITUDE_LONGITUDE_DIFFERENCE_FILE(
    "Latitude and longitude difference file");

static const char *EPSG_NAME_PARAMETER_GEOID_CORRECTION_FILENAME(
    "Geoid (height correction) model file");
constexpr int EPSG_CODE_PARAMETER_GEOID_CORRECTION_FILENAME = 8666;

// ---------------------------------------------------------------------------

static const char *PROJ_WKT2_NAME_METHOD_HEIGHT_TO_GEOG3D(
    "GravityRelatedHeight to Geographic3D");

static const char *PROJ_WKT2_NAME_METHOD_CTABLE2("CTABLE2");

// ---------------------------------------------------------------------------

constexpr int EPSG_CODE_METHOD_VERTCON = 9658;
static const char *EPSG_NAME_METHOD_VERTCON("VERTCON");

static const char *
    EPSG_NAME_PARAMETER_VERTICAL_OFFSET_FILE("Vertical offset file");
constexpr int EPSG_CODE_PARAMETER_VERTICAL_OFFSET_FILE = 8732;

// ---------------------------------------------------------------------------

constexpr int EPSG_CODE_METHOD_NADCON = 9613;
static const char *EPSG_NAME_METHOD_NADCON("NADCON");

static const char *
    EPSG_NAME_PARAMETER_LATITUDE_DIFFERENCE_FILE("Latitude difference file");
constexpr int EPSG_CODE_PARAMETER_LATITUDE_DIFFERENCE_FILE = 8657;

static const char *
    EPSG_NAME_PARAMETER_LONGITUDE_DIFFERENCE_FILE("Longitude difference file");
constexpr int EPSG_CODE_PARAMETER_LONGITUDE_DIFFERENCE_FILE = 8658;

// ---------------------------------------------------------------------------

constexpr int EPSG_CODE_METHOD_CHANGE_VERTICAL_UNIT = 1069;
static const char *
    EPSG_NAME_METHOD_CHANGE_VERTICAL_UNIT("Change of Vertical Unit");

static const char *
    EPSG_NAME_PARAMETER_UNIT_CONVERSION_SCALAR("Unit conversion scalar");
constexpr int EPSG_CODE_PARAMETER_UNIT_CONVERSION_SCALAR = 1051;

// ---------------------------------------------------------------------------

constexpr int EPSG_CODE_METHOD_LONGITUDE_ROTATION = 9601;
static const char *EPSG_NAME_METHOD_LONGITUDE_ROTATION("Longitude rotation");

constexpr int EPSG_CODE_METHOD_VERTICAL_OFFSET = 9616;
static const char *EPSG_NAME_METHOD_VERTICAL_OFFSET("Vertical Offset");

constexpr int EPSG_CODE_METHOD_GEOGRAPHIC2D_OFFSETS = 9619;
static const char *
    EPSG_NAME_METHOD_GEOGRAPHIC2D_OFFSETS("Geographic2D offsets");

constexpr int EPSG_CODE_METHOD_GEOGRAPHIC2D_WITH_HEIGHT_OFFSETS = 9618;
static const char *EPSG_NAME_METHOD_GEOGRAPHIC2D_WITH_HEIGHT_OFFSETS(
    "Geographic2D with Height Offsets");

constexpr int EPSG_CODE_METHOD_GEOGRAPHIC3D_OFFSETS = 9660;
static const char *
    EPSG_NAME_METHOD_GEOGRAPHIC3D_OFFSETS("Geographic3D offsets");

constexpr int EPSG_CODE_METHOD_GEOGRAPHIC_GEOCENTRIC = 9602;
static const char *
    EPSG_NAME_METHOD_GEOGRAPHIC_GEOCENTRIC("Geographic/geocentric conversions");

static const char *EPSG_NAME_PARAMETER_LATITUDE_OFFSET("Latitude offset");
constexpr int EPSG_CODE_PARAMETER_LATITUDE_OFFSET = 8601;

static const char *EPSG_NAME_PARAMETER_LONGITUDE_OFFSET("Longitude offset");
constexpr int EPSG_CODE_PARAMETER_LONGITUDE_OFFSET = 8602;

static const char *EPSG_NAME_PARAMETER_VERTICAL_OFFSET("Vertical Offset");
constexpr int EPSG_CODE_PARAMETER_VERTICAL_OFFSET = 8603;

static const char *EPSG_NAME_PARAMETER_GEOID_UNDULATION("Geoid undulation");
constexpr int EPSG_CODE_PARAMETER_GEOID_UNDULATION = 8604;

// ---------------------------------------------------------------------------

constexpr int EPSG_CODE_METHOD_AFFINE_PARAMETRIC_TRANSFORMATION = 9624;
static const char *EPSG_NAME_METHOD_AFFINE_PARAMETRIC_TRANSFORMATION(
    "Affine parametric transformation");

static const char *EPSG_NAME_PARAMETER_A0("A0");
constexpr int EPSG_CODE_PARAMETER_A0 = 8623;

static const char *EPSG_NAME_PARAMETER_A1("A1");
constexpr int EPSG_CODE_PARAMETER_A1 = 8624;

static const char *EPSG_NAME_PARAMETER_A2("A2");
constexpr int EPSG_CODE_PARAMETER_A2 = 8625;

static const char *EPSG_NAME_PARAMETER_B0("B0");
constexpr int EPSG_CODE_PARAMETER_B0 = 8639;

static const char *EPSG_NAME_PARAMETER_B1("B1");
constexpr int EPSG_CODE_PARAMETER_B1 = 8640;

static const char *EPSG_NAME_PARAMETER_B2("B2");
constexpr int EPSG_CODE_PARAMETER_B2 = 8641;

// ---------------------------------------------------------------------------

constexpr int EPSG_CODE_METHOD_AXIS_ORDER_REVERSAL_2D = 9843;
static const char *
    EPSG_NAME_METHOD_AXIS_ORDER_REVERSAL_2D("Axis Order Reversal (2D)");

constexpr int EPSG_CODE_METHOD_AXIS_ORDER_REVERSAL_3D = 9844;
static const char *EPSG_NAME_METHOD_AXIS_ORDER_REVERSAL_3D(
    "Axis Order Reversal (Geographic3D horizontal)");

// ---------------------------------------------------------------------------

} // namespace operation
NS_PROJ_END

// ---------------------------------------------------------------------------

// anonymous namespace
namespace {

using namespace ::NS_PROJ;
using namespace ::NS_PROJ::operation;

static const char *lat_0 = "lat_0";
static const char *lat_1 = "lat_1";
static const char *lat_2 = "lat_2";
static const char *lat_ts = "lat_ts";
static const char *lon_0 = "lon_0";
static const char *lon_1 = "lon_1";
static const char *lon_2 = "lon_2";
static const char *lonc = "lonc";
static const char *alpha = "alpha";
static const char *gamma = "gamma";
static const char *k_0 = "k_0";
static const char *k = "k";
static const char *x_0 = "x_0";
static const char *y_0 = "y_0";
static const char *h = "h";

static const ParamMapping paramLatitudeNatOrigin = {
    EPSG_NAME_PARAMETER_LATITUDE_OF_NATURAL_ORIGIN,
    EPSG_CODE_PARAMETER_LATITUDE_OF_NATURAL_ORIGIN, WKT1_LATITUDE_OF_ORIGIN,
    common::UnitOfMeasure::Type::ANGULAR, lat_0};

static const ParamMapping paramLongitudeNatOrigin = {
    EPSG_NAME_PARAMETER_LONGITUDE_OF_NATURAL_ORIGIN,
    EPSG_CODE_PARAMETER_LONGITUDE_OF_NATURAL_ORIGIN, WKT1_CENTRAL_MERIDIAN,
    common::UnitOfMeasure::Type::ANGULAR, lon_0};

static const ParamMapping paramScaleFactor = {
    EPSG_NAME_PARAMETER_SCALE_FACTOR_AT_NATURAL_ORIGIN,
    EPSG_CODE_PARAMETER_SCALE_FACTOR_AT_NATURAL_ORIGIN, WKT1_SCALE_FACTOR,
    common::UnitOfMeasure::Type::SCALE, k_0};

static const ParamMapping paramFalseEasting = {
    EPSG_NAME_PARAMETER_FALSE_EASTING, EPSG_CODE_PARAMETER_FALSE_EASTING,
    WKT1_FALSE_EASTING, common::UnitOfMeasure::Type::LINEAR, x_0};

static const ParamMapping paramFalseNorthing = {
    EPSG_NAME_PARAMETER_FALSE_NORTHING, EPSG_CODE_PARAMETER_FALSE_NORTHING,
    WKT1_FALSE_NORTHING, common::UnitOfMeasure::Type::LINEAR, y_0};

static const ParamMapping paramLatitudeFalseOrigin = {
    EPSG_NAME_PARAMETER_LATITUDE_FALSE_ORIGIN,
    EPSG_CODE_PARAMETER_LATITUDE_FALSE_ORIGIN, WKT1_LATITUDE_OF_ORIGIN,
    common::UnitOfMeasure::Type::ANGULAR, lat_0};

static const ParamMapping paramLongitudeFalseOrigin = {
    EPSG_NAME_PARAMETER_LONGITUDE_FALSE_ORIGIN,
    EPSG_CODE_PARAMETER_LONGITUDE_FALSE_ORIGIN, WKT1_CENTRAL_MERIDIAN,
    common::UnitOfMeasure::Type::ANGULAR, lon_0};

static const ParamMapping paramFalseEastingOrigin = {
    EPSG_NAME_PARAMETER_EASTING_FALSE_ORIGIN,
    EPSG_CODE_PARAMETER_EASTING_FALSE_ORIGIN, WKT1_FALSE_EASTING,
    common::UnitOfMeasure::Type::LINEAR, x_0};

static const ParamMapping paramFalseNorthingOrigin = {
    EPSG_NAME_PARAMETER_NORTHING_FALSE_ORIGIN,
    EPSG_CODE_PARAMETER_NORTHING_FALSE_ORIGIN, WKT1_FALSE_NORTHING,
    common::UnitOfMeasure::Type::LINEAR, y_0};

static const ParamMapping paramLatitude1stStdParallel = {
    EPSG_NAME_PARAMETER_LATITUDE_1ST_STD_PARALLEL,
    EPSG_CODE_PARAMETER_LATITUDE_1ST_STD_PARALLEL, WKT1_STANDARD_PARALLEL_1,
    common::UnitOfMeasure::Type::ANGULAR, lat_1};

static const ParamMapping paramLatitude2ndStdParallel = {
    EPSG_NAME_PARAMETER_LATITUDE_2ND_STD_PARALLEL,
    EPSG_CODE_PARAMETER_LATITUDE_2ND_STD_PARALLEL, "standard_parallel_2",
    common::UnitOfMeasure::Type::ANGULAR, lat_2};

static const ParamMapping *const paramsNatOriginScale[] = {
    &paramLatitudeNatOrigin, &paramLongitudeNatOrigin, &paramScaleFactor,
    &paramFalseEasting,      &paramFalseNorthing,      nullptr};

static const ParamMapping paramLatFirstPoint = {
    "Latitude of first point", 0, "Latitude_Of_1st_Point",
    common::UnitOfMeasure::Type::ANGULAR, lat_1};
static const ParamMapping paramLongFirstPoint = {
    "Longitude of first point", 0, "Longitude_Of_1st_Point",
    common::UnitOfMeasure::Type::ANGULAR, lon_1};
static const ParamMapping paramLatSecondPoint = {
    "Latitude of second point", 0, "Latitude_Of_2nd_Point",
    common::UnitOfMeasure::Type::ANGULAR, lat_2};
static const ParamMapping paramLongSecondPoint = {
    "Longitude of second point", 0, "Longitude_Of_2nd_Point",
    common::UnitOfMeasure::Type::ANGULAR, lon_2};

static const ParamMapping *const paramsTPEQD[] = {&paramLatFirstPoint,
                                                  &paramLongFirstPoint,
                                                  &paramLatSecondPoint,
                                                  &paramLongSecondPoint,
                                                  &paramFalseEasting,
                                                  &paramFalseNorthing,
                                                  nullptr};

static const ParamMapping *const paramsTMG[] = {
    &paramLatitudeFalseOrigin, &paramLongitudeFalseOrigin,
    &paramFalseEastingOrigin, &paramFalseNorthingOrigin, nullptr};

static const ParamMapping paramLatFalseOriginLatOfCenter = {
    EPSG_NAME_PARAMETER_LATITUDE_FALSE_ORIGIN,
    EPSG_CODE_PARAMETER_LATITUDE_FALSE_ORIGIN, "latitude_of_center",
    common::UnitOfMeasure::Type::ANGULAR, lat_0};

static const ParamMapping paramLongFalseOriginLongOfCenter = {
    EPSG_NAME_PARAMETER_LONGITUDE_FALSE_ORIGIN,
    EPSG_CODE_PARAMETER_LONGITUDE_FALSE_ORIGIN, "longitude_of_center",
    common::UnitOfMeasure::Type::ANGULAR, lon_0};

static const ParamMapping *const paramsAEA[] = {
    &paramLatFalseOriginLatOfCenter,
    &paramLongFalseOriginLongOfCenter,
    &paramLatitude1stStdParallel,
    &paramLatitude2ndStdParallel,
    &paramFalseEastingOrigin,
    &paramFalseNorthingOrigin,
    nullptr};

static const ParamMapping *const paramsLCC2SP[] = {
    &paramLatitudeFalseOrigin,
    &paramLongitudeFalseOrigin,
    &paramLatitude1stStdParallel,
    &paramLatitude2ndStdParallel,
    &paramFalseEastingOrigin,
    &paramFalseNorthingOrigin,
    nullptr,
};

static const ParamMapping paramEllipsoidScaleFactor = {
    EPSG_NAME_PARAMETER_ELLIPSOID_SCALE_FACTOR,
    EPSG_CODE_PARAMETER_ELLIPSOID_SCALE_FACTOR, nullptr,
    common::UnitOfMeasure::Type::SCALE, k_0};

static const ParamMapping *const paramsLCC2SPMichigan[] = {
    &paramLatitudeFalseOrigin,    &paramLongitudeFalseOrigin,
    &paramLatitude1stStdParallel, &paramLatitude2ndStdParallel,
    &paramFalseEastingOrigin,     &paramFalseNorthingOrigin,
    &paramEllipsoidScaleFactor,   nullptr,
};

static const ParamMapping paramLatNatLatCenter = {
    EPSG_NAME_PARAMETER_LATITUDE_OF_NATURAL_ORIGIN,
    EPSG_CODE_PARAMETER_LATITUDE_OF_NATURAL_ORIGIN, "latitude_of_center",
    common::UnitOfMeasure::Type::ANGULAR, lat_0};

static const ParamMapping paramLonNatLonCenter = {
    EPSG_NAME_PARAMETER_LONGITUDE_OF_NATURAL_ORIGIN,
    EPSG_CODE_PARAMETER_LONGITUDE_OF_NATURAL_ORIGIN, "longitude_of_center",
    common::UnitOfMeasure::Type::ANGULAR, lon_0};

static const ParamMapping *const paramsAEQD[]{
    &paramLatNatLatCenter, &paramLonNatLonCenter, &paramFalseEasting,
    &paramFalseNorthing, nullptr};

static const ParamMapping *const paramsNatOrigin[] = {
    &paramLatitudeNatOrigin, &paramLongitudeNatOrigin, &paramFalseEasting,
    &paramFalseNorthing, nullptr};

static const ParamMapping paramLatNatOriginLat1 = {
    EPSG_NAME_PARAMETER_LATITUDE_OF_NATURAL_ORIGIN,
    EPSG_CODE_PARAMETER_LATITUDE_OF_NATURAL_ORIGIN, WKT1_STANDARD_PARALLEL_1,
    common::UnitOfMeasure::Type::ANGULAR, lat_1};

static const ParamMapping *const paramsBonne[] = {
    &paramLatNatOriginLat1, &paramLongitudeNatOrigin, &paramFalseEasting,
    &paramFalseNorthing, nullptr};

static const ParamMapping paramLat1stParallelLatTs = {
    EPSG_NAME_PARAMETER_LATITUDE_1ST_STD_PARALLEL,
    EPSG_CODE_PARAMETER_LATITUDE_1ST_STD_PARALLEL, WKT1_STANDARD_PARALLEL_1,
    common::UnitOfMeasure::Type::ANGULAR, lat_ts};

static const ParamMapping *const paramsCEA[] = {
    &paramLat1stParallelLatTs, &paramLongitudeNatOrigin, &paramFalseEasting,
    &paramFalseNorthing, nullptr};

static const ParamMapping *const paramsEQDC[] = {&paramLatNatLatCenter,
                                                 &paramLonNatLonCenter,
                                                 &paramLatitude1stStdParallel,
                                                 &paramLatitude2ndStdParallel,
                                                 &paramFalseEasting,
                                                 &paramFalseNorthing,
                                                 nullptr};

static const ParamMapping *const paramsLonNatOrigin[] = {
    &paramLongitudeNatOrigin, &paramFalseEasting, &paramFalseNorthing, nullptr};

static const ParamMapping *const paramsEqc[] = // same as paramsCEA
    {&paramLat1stParallelLatTs, &paramLongitudeNatOrigin, &paramFalseEasting,
     &paramFalseNorthing, nullptr};

static const ParamMapping paramSatelliteHeight = {
    "Satellite Height", 0, "satellite_height",
    common::UnitOfMeasure::Type::LINEAR, h};

static const ParamMapping *const paramsGeos[] = {
    &paramLongitudeNatOrigin, &paramSatelliteHeight, &paramFalseEasting,
    &paramFalseNorthing, nullptr};

static const ParamMapping paramLatCentreLatOrigin = {
    EPSG_NAME_PARAMETER_LATITUDE_PROJECTION_CENTRE,
    EPSG_CODE_PARAMETER_LATITUDE_PROJECTION_CENTRE, WKT1_LATITUDE_OF_ORIGIN,
    common::UnitOfMeasure::Type::ANGULAR, lat_0};
static const ParamMapping paramLonCentreCentralMeridian = {
    EPSG_NAME_PARAMETER_LONGITUDE_PROJECTION_CENTRE,
    EPSG_CODE_PARAMETER_LONGITUDE_PROJECTION_CENTRE, WKT1_CENTRAL_MERIDIAN,
    common::UnitOfMeasure::Type::ANGULAR, lonc};

static const ParamMapping paramAzimuth = {
    EPSG_NAME_PARAMETER_AZIMUTH_INITIAL_LINE,
    EPSG_CODE_PARAMETER_AZIMUTH_INITIAL_LINE, "azimuth",
    common::UnitOfMeasure::Type::ANGULAR, alpha};

static const ParamMapping paramAngleToSkewGrid = {
    EPSG_NAME_PARAMETER_ANGLE_RECTIFIED_TO_SKEW_GRID,
    EPSG_CODE_PARAMETER_ANGLE_RECTIFIED_TO_SKEW_GRID, "rectified_grid_angle",
    common::UnitOfMeasure::Type::ANGULAR, gamma};
static const ParamMapping paramScaleFactorInitialLine = {
    EPSG_NAME_PARAMETER_SCALE_FACTOR_INITIAL_LINE,
    EPSG_CODE_PARAMETER_SCALE_FACTOR_INITIAL_LINE, WKT1_SCALE_FACTOR,
    common::UnitOfMeasure::Type::SCALE, k};

static const ParamMapping *const paramsHomVariantA[] = {
    &paramLatCentreLatOrigin,
    &paramLonCentreCentralMeridian,
    &paramAzimuth,
    &paramAngleToSkewGrid,
    &paramScaleFactorInitialLine,
    &paramFalseEasting,
    &paramFalseNorthing,
    nullptr};

static const ParamMapping paramFalseEastingProjectionCentre = {
    EPSG_NAME_PARAMETER_EASTING_PROJECTION_CENTRE,
    EPSG_CODE_PARAMETER_EASTING_PROJECTION_CENTRE, WKT1_FALSE_EASTING,
    common::UnitOfMeasure::Type::LINEAR, x_0};

static const ParamMapping paramFalseNorthingProjectionCentre = {
    EPSG_NAME_PARAMETER_NORTHING_PROJECTION_CENTRE,
    EPSG_CODE_PARAMETER_NORTHING_PROJECTION_CENTRE, WKT1_FALSE_NORTHING,
    common::UnitOfMeasure::Type::LINEAR, y_0};

static const ParamMapping *const paramsHomVariantB[] = {
    &paramLatCentreLatOrigin,
    &paramLonCentreCentralMeridian,
    &paramAzimuth,
    &paramAngleToSkewGrid,
    &paramScaleFactorInitialLine,
    &paramFalseEastingProjectionCentre,
    &paramFalseNorthingProjectionCentre,
    nullptr};

static const ParamMapping paramLatPoint1 = {
    "Latitude of point 1", 0, "latitude_of_point_1",
    common::UnitOfMeasure::Type::ANGULAR, lat_1};

static const ParamMapping paramLonPoint1 = {
    "Longitude of point 1", 0, "longitude_of_point_1",
    common::UnitOfMeasure::Type::ANGULAR, lon_1};

static const ParamMapping paramLatPoint2 = {
    "Latitude of point 2", 0, "latitude_of_point_2",
    common::UnitOfMeasure::Type::ANGULAR, lat_2};

static const ParamMapping paramLonPoint2 = {
    "Longitude of point 2", 0, "longitude_of_point_2",
    common::UnitOfMeasure::Type::ANGULAR, lon_2};

static const ParamMapping *const paramsHomTwoPoint[] = {
    &paramLatCentreLatOrigin,
    &paramLatPoint1,
    &paramLonPoint1,
    &paramLatPoint2,
    &paramLonPoint2,
    &paramScaleFactorInitialLine,
    &paramFalseEastingProjectionCentre,
    &paramFalseNorthingProjectionCentre,
    nullptr};

static const ParamMapping *const paramsIMWP[] = {
    &paramLongitudeNatOrigin,     &paramLatitude1stStdParallel,
    &paramLatitude2ndStdParallel, &paramFalseEasting,
    &paramFalseNorthing,          nullptr};

static const ParamMapping paramLatCentreLatCenter = {
    EPSG_NAME_PARAMETER_LATITUDE_PROJECTION_CENTRE,
    EPSG_CODE_PARAMETER_LATITUDE_PROJECTION_CENTRE, "latitude_of_center",
    common::UnitOfMeasure::Type::ANGULAR, lat_0};

static const ParamMapping paramLonCentreLonCenter = {
    EPSG_NAME_PARAMETER_LONGITUDE_OF_ORIGIN,
    EPSG_CODE_PARAMETER_LONGITUDE_OF_ORIGIN, "longitude_of_center",
    common::UnitOfMeasure::Type::ANGULAR, lon_0};

static const ParamMapping paramColatitudeConeAxis = {
    EPSG_NAME_PARAMETER_COLATITUDE_CONE_AXIS,
    EPSG_CODE_PARAMETER_COLATITUDE_CONE_AXIS, "azimuth",
    common::UnitOfMeasure::Type::ANGULAR,
    nullptr}; /* ignored by PROJ currently */

static const ParamMapping paramLatitudePseudoStdParallel = {
    EPSG_NAME_PARAMETER_LATITUDE_PSEUDO_STANDARD_PARALLEL,
    EPSG_CODE_PARAMETER_LATITUDE_PSEUDO_STANDARD_PARALLEL,
    "pseudo_standard_parallel_1", common::UnitOfMeasure::Type::ANGULAR,
    nullptr}; /* ignored by PROJ currently */

static const ParamMapping paramScaleFactorPseudoStdParallel = {
    EPSG_NAME_PARAMETER_SCALE_FACTOR_PSEUDO_STANDARD_PARALLEL,
    EPSG_CODE_PARAMETER_SCALE_FACTOR_PSEUDO_STANDARD_PARALLEL,
    WKT1_SCALE_FACTOR, common::UnitOfMeasure::Type::SCALE,
    k}; /* ignored by PROJ currently */

static const ParamMapping *const krovakParameters[] = {
    &paramLatCentreLatCenter,
    &paramLonCentreLonCenter,
    &paramColatitudeConeAxis,
    &paramLatitudePseudoStdParallel,
    &paramScaleFactorPseudoStdParallel,
    &paramFalseEasting,
    &paramFalseNorthing,
    nullptr};

static const ParamMapping *const paramsLaea[] = {
    &paramLatNatLatCenter, &paramLonNatLonCenter, &paramFalseEasting,
    &paramFalseNorthing, nullptr};

static const ParamMapping *const paramsMiller[] = {
    &paramLatNatLatCenter, &paramLonNatLonCenter, &paramFalseEasting,
    &paramFalseNorthing, nullptr};

static const ParamMapping paramLonMerc1SP = {
    EPSG_NAME_PARAMETER_LATITUDE_OF_NATURAL_ORIGIN,
    EPSG_CODE_PARAMETER_LATITUDE_OF_NATURAL_ORIGIN,
    nullptr, // always set to zero, not to be exported in WKT1
    common::UnitOfMeasure::Type::ANGULAR,
    nullptr}; // always set to zero, not to be exported in PROJ strings

static const ParamMapping paramScaleFactorK = {
    EPSG_NAME_PARAMETER_SCALE_FACTOR_AT_NATURAL_ORIGIN,
    EPSG_CODE_PARAMETER_SCALE_FACTOR_AT_NATURAL_ORIGIN, WKT1_SCALE_FACTOR,
    common::UnitOfMeasure::Type::SCALE, k};

static const ParamMapping *const paramsMerc1SP[] = {
    &paramLonMerc1SP,   &paramLongitudeNatOrigin, &paramScaleFactorK,
    &paramFalseEasting, &paramFalseNorthing,      nullptr};

static const ParamMapping *const paramsMerc2SP[] = {
    &paramLat1stParallelLatTs, &paramLongitudeNatOrigin, &paramFalseEasting,
    &paramFalseNorthing, nullptr};

static const ParamMapping *const paramsObliqueStereo[] = {
    &paramLatitudeNatOrigin, &paramLongitudeNatOrigin, &paramScaleFactorK,
    &paramFalseEasting,      &paramFalseNorthing,      nullptr};

static const ParamMapping paramLatStdParallel = {
    EPSG_NAME_PARAMETER_LATITUDE_STD_PARALLEL,
    EPSG_CODE_PARAMETER_LATITUDE_STD_PARALLEL, WKT1_LATITUDE_OF_ORIGIN,
    common::UnitOfMeasure::Type::ANGULAR, lat_ts};

static const ParamMapping paramsLonOrigin = {
    EPSG_NAME_PARAMETER_LONGITUDE_OF_ORIGIN,
    EPSG_CODE_PARAMETER_LONGITUDE_OF_ORIGIN, WKT1_CENTRAL_MERIDIAN,
    common::UnitOfMeasure::Type::ANGULAR, lon_0};

static const ParamMapping *const paramsPolarStereo[] = {
    &paramLatStdParallel, &paramsLonOrigin, &paramFalseEasting,
    &paramFalseNorthing, nullptr};

static const ParamMapping *const paramsLonNatOriginLongitudeCentre[] = {
    &paramLonNatLonCenter, &paramFalseEasting, &paramFalseNorthing, nullptr};

static const ParamMapping paramLatTrueScaleWag3 = {
    "Latitude of true scale", 0, "latitude_of_origin",
    common::UnitOfMeasure::Type::ANGULAR, lat_ts};

static const ParamMapping *const paramsWag3[] = {
    &paramLatTrueScaleWag3, &paramLongitudeNatOrigin, &paramFalseEasting,
    &paramFalseNorthing, nullptr};

static const ParamMapping paramPegLat = {
    "Peg point latitude", 0, "peg_point_latitude",
    common::UnitOfMeasure::Type::ANGULAR, "plat_0"};

static const ParamMapping paramPegLon = {
    "Peg point longitude", 0, "peg_point_longitude",
    common::UnitOfMeasure::Type::ANGULAR, "plon_0"};

static const ParamMapping paramPegHeading = {
    "Peg point heading", 0, "peg_point_heading",
    common::UnitOfMeasure::Type::ANGULAR, "phdg_0"};

static const ParamMapping paramPegHeight = {
    "Peg point height", 0, "peg_point_height",
    common::UnitOfMeasure::Type::LINEAR, "h_0"};

static const ParamMapping *const paramsSch[] = {
    &paramPegLat, &paramPegLon, &paramPegHeading, &paramPegHeight, nullptr};

static const ParamMapping *const paramsWink1[] = {
    &paramLatitudeNatOrigin,   &paramLongitudeNatOrigin,
    &paramLat1stParallelLatTs, &paramFalseEasting,
    &paramFalseNorthing,       nullptr};

static const ParamMapping *const paramsWink2[] = {
    &paramLatitudeNatOrigin,      &paramLongitudeNatOrigin,
    &paramLatitude1stStdParallel, &paramFalseEasting,
    &paramFalseNorthing,          nullptr};

static const ParamMapping paramLatLoxim = {
    EPSG_NAME_PARAMETER_LATITUDE_OF_NATURAL_ORIGIN,
    EPSG_CODE_PARAMETER_LATITUDE_OF_NATURAL_ORIGIN, WKT1_LATITUDE_OF_ORIGIN,
    common::UnitOfMeasure::Type::ANGULAR, lat_1};

static const ParamMapping *const paramsLoxim[] = {
    &paramLatLoxim, &paramLongitudeNatOrigin, &paramFalseEasting,
    &paramFalseNorthing, nullptr};

static const MethodMapping methodMappings[] = {
    {EPSG_NAME_METHOD_TRANSVERSE_MERCATOR, EPSG_CODE_METHOD_TRANSVERSE_MERCATOR,
     "Transverse_Mercator", "tmerc", nullptr, paramsNatOriginScale},

    {EPSG_NAME_METHOD_TRANSVERSE_MERCATOR_SOUTH_ORIENTATED,
     EPSG_CODE_METHOD_TRANSVERSE_MERCATOR_SOUTH_ORIENTATED,
     "Transverse_Mercator_South_Orientated", "tmerc", "axis=wsu",
     paramsNatOriginScale},

    {PROJ_WKT2_NAME_METHOD_TWO_POINT_EQUIDISTANT, 0, "Two_Point_Equidistant",
     "tpeqd", nullptr, paramsTPEQD},

    {EPSG_NAME_METHOD_TUNISIA_MAPPING_GRID,
     EPSG_CODE_METHOD_TUNISIA_MAPPING_GRID, "Tunisia_Mapping_Grid", nullptr,
     nullptr, // no proj equivalent
     paramsTMG},

    {EPSG_NAME_METHOD_ALBERS_EQUAL_AREA, EPSG_CODE_METHOD_ALBERS_EQUAL_AREA,
     "Albers_Conic_Equal_Area", "aea", nullptr, paramsAEA},

    {EPSG_NAME_METHOD_LAMBERT_CONIC_CONFORMAL_1SP,
     EPSG_CODE_METHOD_LAMBERT_CONIC_CONFORMAL_1SP,
     "Lambert_Conformal_Conic_1SP", "lcc", nullptr,
     []() {
         static const ParamMapping paramLatLCC1SP = {
             EPSG_NAME_PARAMETER_LATITUDE_OF_NATURAL_ORIGIN,
             EPSG_CODE_PARAMETER_LATITUDE_OF_NATURAL_ORIGIN,
             WKT1_LATITUDE_OF_ORIGIN, common::UnitOfMeasure::Type::ANGULAR,
             lat_1};

         static const ParamMapping *const x[] = {
             &paramLatLCC1SP,    &paramLongitudeNatOrigin, &paramScaleFactor,
             &paramFalseEasting, &paramFalseNorthing,      nullptr,
         };
         return x;
     }()},

    {EPSG_NAME_METHOD_LAMBERT_CONIC_CONFORMAL_2SP,
     EPSG_CODE_METHOD_LAMBERT_CONIC_CONFORMAL_2SP,
     "Lambert_Conformal_Conic_2SP", "lcc", nullptr, paramsLCC2SP},

    {EPSG_NAME_METHOD_LAMBERT_CONIC_CONFORMAL_2SP_MICHIGAN,
     EPSG_CODE_METHOD_LAMBERT_CONIC_CONFORMAL_2SP_MICHIGAN,
     nullptr, // no mapping to WKT1_GDAL
     "lcc", nullptr, paramsLCC2SPMichigan},

    {EPSG_NAME_METHOD_LAMBERT_CONIC_CONFORMAL_2SP_BELGIUM,
     EPSG_CODE_METHOD_LAMBERT_CONIC_CONFORMAL_2SP_BELGIUM,
     "Lambert_Conformal_Conic_2SP_Belgium", "lcc",
     nullptr, // FIXME: this is what is done in GDAL, but the formula of
              // LCC 2SP
     // Belgium in the EPSG 7.2 guidance is difference from the regular
     // LCC 2SP
     paramsLCC2SP},

    {EPSG_NAME_METHOD_MODIFIED_AZIMUTHAL_EQUIDISTANT,
     EPSG_CODE_METHOD_MODIFIED_AZIMUTHAL_EQUIDISTANT, "Azimuthal_Equidistant",
     "aeqd", nullptr, paramsAEQD},

    {EPSG_NAME_METHOD_GUAM_PROJECTION, EPSG_CODE_METHOD_GUAM_PROJECTION,
     nullptr, // no mapping to GDAL WKT1
     "aeqd", "guam", paramsNatOrigin},

    {EPSG_NAME_METHOD_BONNE, EPSG_CODE_METHOD_BONNE, "Bonne", "bonne", nullptr,
     paramsBonne},

    {EPSG_NAME_METHOD_LAMBERT_CYLINDRICAL_EQUAL_AREA_SPHERICAL,
     EPSG_CODE_METHOD_LAMBERT_CYLINDRICAL_EQUAL_AREA_SPHERICAL,
     "Cylindrical_Equal_Area", "cea", nullptr, paramsCEA},

    {EPSG_NAME_METHOD_LAMBERT_CYLINDRICAL_EQUAL_AREA,
     EPSG_CODE_METHOD_LAMBERT_CYLINDRICAL_EQUAL_AREA, "Cylindrical_Equal_Area",
     "cea", nullptr, paramsCEA},

    {EPSG_NAME_METHOD_CASSINI_SOLDNER, EPSG_CODE_METHOD_CASSINI_SOLDNER,
     "Cassini_Soldner", "cass", nullptr, paramsNatOrigin},

    {PROJ_WKT2_NAME_METHOD_EQUIDISTANT_CONIC, 0, "Equidistant_Conic", "eqdc",
     nullptr, paramsEQDC},

    {PROJ_WKT2_NAME_METHOD_ECKERT_I, 0, "Eckert_I", "eck1", nullptr,
     paramsLonNatOrigin},

    {PROJ_WKT2_NAME_METHOD_ECKERT_II, 0, "Eckert_II", "eck2", nullptr,
     paramsLonNatOrigin},

    {PROJ_WKT2_NAME_METHOD_ECKERT_III, 0, "Eckert_III", "eck3", nullptr,
     paramsLonNatOrigin},

    {PROJ_WKT2_NAME_METHOD_ECKERT_IV, 0, "Eckert_IV", "eck4", nullptr,
     paramsLonNatOrigin},

    {PROJ_WKT2_NAME_METHOD_ECKERT_V, 0, "Eckert_V", "eck5", nullptr,
     paramsLonNatOrigin},

    {PROJ_WKT2_NAME_METHOD_ECKERT_VI, 0, "Eckert_VI", "eck6", nullptr,
     paramsLonNatOrigin},

    {EPSG_NAME_METHOD_EQUIDISTANT_CYLINDRICAL,
     EPSG_CODE_METHOD_EQUIDISTANT_CYLINDRICAL, "Equirectangular", "eqc",
     nullptr, paramsEqc},

    {EPSG_NAME_METHOD_EQUIDISTANT_CYLINDRICAL_SPHERICAL,
     EPSG_CODE_METHOD_EQUIDISTANT_CYLINDRICAL_SPHERICAL, "Equirectangular",
     "eqc", nullptr, paramsEqc},

    {PROJ_WKT2_NAME_METHOD_GALL, 0, "Gall_Stereographic", "gall", nullptr,
     paramsLonNatOrigin},

    {PROJ_WKT2_NAME_METHOD_GOODE_HOMOLOSINE, 0, "Goode_Homolosine", "goode",
     nullptr, paramsLonNatOrigin},

    {PROJ_WKT2_NAME_METHOD_INTERRUPTED_GOODE_HOMOLOSINE, 0,
     "Interrupted_Goode_Homolosine", "igh", nullptr, paramsLonNatOrigin},

    // No WKT1 representation fr sweep=x
    {PROJ_WKT2_NAME_METHOD_GEOSTATIONARY_SATELLITE_SWEEP_X, 0, nullptr, "geos",
     "sweep=x", paramsGeos},

    {PROJ_WKT2_NAME_METHOD_GEOSTATIONARY_SATELLITE_SWEEP_Y, 0,
     "Geostationary_Satellite", "geos", nullptr, paramsGeos},

    {PROJ_WKT2_NAME_METHOD_GAUSS_SCHREIBER_TRANSVERSE_MERCATOR, 0,
     "Gauss_Schreiber_Transverse_Mercator", "gstmerc", nullptr,
     paramsNatOriginScale},

    {PROJ_WKT2_NAME_METHOD_GNOMONIC, 0, "Gnomonic", "gnom", nullptr,
     paramsNatOrigin},

    {EPSG_NAME_METHOD_HOTINE_OBLIQUE_MERCATOR_VARIANT_A,
     EPSG_CODE_METHOD_HOTINE_OBLIQUE_MERCATOR_VARIANT_A,
     "Hotine_Oblique_Mercator", "omerc", "no_uoff", paramsHomVariantA},

    {EPSG_NAME_METHOD_HOTINE_OBLIQUE_MERCATOR_VARIANT_B,
     EPSG_CODE_METHOD_HOTINE_OBLIQUE_MERCATOR_VARIANT_B,
     "Hotine_Oblique_Mercator_Azimuth_Center", "omerc", nullptr,
     paramsHomVariantB},

    {PROJ_WKT2_NAME_METHOD_HOTINE_OBLIQUE_MERCATOR_TWO_POINT_NATURAL_ORIGIN, 0,
     "Hotine_Oblique_Mercator_Two_Point_Natural_Origin", "omerc", nullptr,
     paramsHomTwoPoint},

    {PROJ_WKT2_NAME_INTERNATIONAL_MAP_WORLD_POLYCONIC, 0,
     "International_Map_of_the_World_Polyconic", "imw_p", nullptr, paramsIMWP},

    {EPSG_NAME_METHOD_KROVAK_NORTH_ORIENTED,
     EPSG_CODE_METHOD_KROVAK_NORTH_ORIENTED, "Krovak", "krovak", nullptr,
     krovakParameters},

    {EPSG_NAME_METHOD_KROVAK, EPSG_CODE_METHOD_KROVAK, nullptr, "krovak",
     "axis=swu", krovakParameters},

    {EPSG_NAME_METHOD_LAMBERT_AZIMUTHAL_EQUAL_AREA,
     EPSG_CODE_METHOD_LAMBERT_AZIMUTHAL_EQUAL_AREA,
     "Lambert_Azimuthal_Equal_Area", "laea", nullptr, paramsLaea},

    {PROJ_WKT2_NAME_METHOD_MILLER_CYLINDRICAL, 0, "Miller_Cylindrical", "mill",
     "R_A", paramsMiller},

    {EPSG_NAME_METHOD_MERCATOR_VARIANT_A, EPSG_CODE_METHOD_MERCATOR_VARIANT_A,
     "Mercator_1SP", "merc", nullptr, paramsMerc1SP},

    {EPSG_NAME_METHOD_MERCATOR_VARIANT_B, EPSG_CODE_METHOD_MERCATOR_VARIANT_B,
     "Mercator_2SP", "merc", nullptr, paramsMerc2SP},

    {EPSG_NAME_METHOD_POPULAR_VISUALISATION_PSEUDO_MERCATOR,
     EPSG_CODE_METHOD_POPULAR_VISUALISATION_PSEUDO_MERCATOR,
     "Popular_Visualisation_Pseudo_Mercator", // particular case actually
                                              // handled manually
     "webmerc", nullptr, paramsNatOrigin},

    {PROJ_WKT2_NAME_METHOD_MOLLWEIDE, 0, "Mollweide", "moll", nullptr,
     paramsLonNatOrigin},

    {EPSG_NAME_METHOD_NZMG, EPSG_CODE_METHOD_NZMG, "New_Zealand_Map_Grid",
     "nzmg", nullptr, paramsNatOrigin},

    {
        EPSG_NAME_METHOD_OBLIQUE_STEREOGRAPHIC,
        EPSG_CODE_METHOD_OBLIQUE_STEREOGRAPHIC, "Oblique_Stereographic",
        "sterea", nullptr, paramsObliqueStereo,
    },

    {EPSG_NAME_METHOD_ORTHOGRAPHIC, EPSG_CODE_METHOD_ORTHOGRAPHIC,
     "Orthographic", "ortho", nullptr, paramsNatOrigin},

    {EPSG_NAME_METHOD_AMERICAN_POLYCONIC, EPSG_CODE_METHOD_AMERICAN_POLYCONIC,
     "Polyconic", "poly", nullptr, paramsNatOrigin},

    {EPSG_NAME_METHOD_POLAR_STEREOGRAPHIC_VARIANT_A,
     EPSG_CODE_METHOD_POLAR_STEREOGRAPHIC_VARIANT_A, "Polar_Stereographic",
     "stere", nullptr, paramsObliqueStereo},

    {EPSG_NAME_METHOD_POLAR_STEREOGRAPHIC_VARIANT_B,
     EPSG_CODE_METHOD_POLAR_STEREOGRAPHIC_VARIANT_B, "Polar_Stereographic",
     "stere", nullptr, paramsPolarStereo},

    {PROJ_WKT2_NAME_METHOD_ROBINSON, 0, "Robinson", "robin", nullptr,
     paramsLonNatOriginLongitudeCentre},

    {PROJ_WKT2_NAME_METHOD_SINUSOIDAL, 0, "Sinusoidal", "sinu", nullptr,
     paramsLonNatOriginLongitudeCentre},

    {PROJ_WKT2_NAME_METHOD_STEREOGRAPHIC, 0, "Stereographic", "stere", nullptr,
     paramsObliqueStereo},

    {PROJ_WKT2_NAME_METHOD_VAN_DER_GRINTEN, 0, "VanDerGrinten", "vandg", "R_A",
     paramsLonNatOrigin},

    {PROJ_WKT2_NAME_METHOD_WAGNER_I, 0, "Wagner_I", "wag1", nullptr,
     paramsLonNatOrigin},

    {PROJ_WKT2_NAME_METHOD_WAGNER_II, 0, "Wagner_II", "wag2", nullptr,
     paramsLonNatOrigin},

    {PROJ_WKT2_NAME_METHOD_WAGNER_III, 0, "Wagner_III", "wag3", nullptr,
     paramsWag3},

    {PROJ_WKT2_NAME_METHOD_WAGNER_IV, 0, "Wagner_IV", "wag4", nullptr,
     paramsLonNatOrigin},

    {PROJ_WKT2_NAME_METHOD_WAGNER_V, 0, "Wagner_V", "wag5", nullptr,
     paramsLonNatOrigin},

    {PROJ_WKT2_NAME_METHOD_WAGNER_VI, 0, "Wagner_VI", "wag6", nullptr,
     paramsLonNatOrigin},

    {PROJ_WKT2_NAME_METHOD_WAGNER_VII, 0, "Wagner_VII", "wag7", nullptr,
     paramsLonNatOrigin},

    {PROJ_WKT2_NAME_METHOD_QUADRILATERALIZED_SPHERICAL_CUBE, 0,
     "Quadrilateralized_Spherical_Cube", "qsc", nullptr, paramsNatOrigin},

    {PROJ_WKT2_NAME_METHOD_SPHERICAL_CROSS_TRACK_HEIGHT, 0,
     "Spherical_Cross_Track_Height", "sch", nullptr, paramsSch},

    // The following methods have just the WKT <--> PROJ string mapping, but
    // no setter. Similarly to GDAL

    {"Aitoff", 0, "Aitoff", "aitoff", nullptr, paramsNatOrigin},

    {"Winkel I", 0, "Winkel_I", "wink1", nullptr, paramsWink1},

    {"Winkel II", 0, "Winkel_II", "wink2", nullptr, paramsWink2},

    {"Winkel Tripel", 0, "Winkel_Tripel", "wintri", nullptr, paramsWink2},

    {"Craster Parabolic", 0, "Craster_Parabolic", "crast", nullptr,
     paramsNatOrigin},

    {"Loximuthal", 0, "Loximuthal", "loxim", nullptr, paramsLoxim},

    {"Quartic Authalic", 0, "Quartic_Authalic", "qua_aut", nullptr,
     paramsNatOrigin},

    {EPSG_NAME_METHOD_EQUAL_EARTH, EPSG_CODE_METHOD_EQUAL_EARTH, nullptr,
     "eqearth", nullptr, paramsLonNatOrigin},

};

// end of anonymous namespace
}

// ---------------------------------------------------------------------------

//! @endcond

#endif // COORDINATEOPERATION_CONSTANTS_HH_INCLUDED
