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

#ifndef FROM_PROJ_CPP
#define FROM_PROJ_CPP
#endif

#include "proj/common.hpp"
#include "proj/coordinatesystem.hpp"
#include "proj/coordinatesystem_internal.hpp"
#include "proj/crs.hpp"
#include "proj/datum.hpp"
#include "proj/io.hpp"
#include "proj/io_internal.hpp"
#include "proj/metadata.hpp"
#include "proj/util.hpp"

#include <map>
#include <set>
#include <string>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// We put all static definitions in the same compilation unit, and in
// increasing order of dependency, to avoid the "static initialization fiasco"
// See https://isocpp.org/wiki/faq/ctors#static-init-order

using namespace NS_PROJ::common;
using namespace NS_PROJ::cs;
using namespace NS_PROJ::crs;
using namespace NS_PROJ::datum;
using namespace NS_PROJ::io;
using namespace NS_PROJ::metadata;
using namespace NS_PROJ::util;

// ---------------------------------------------------------------------------

const std::string IdentifiedObject::NAME_KEY("name");
const std::string IdentifiedObject::IDENTIFIER_KEY("identifier");
const std::string IdentifiedObject::ALIAS_KEY("alias");
const std::string IdentifiedObject::REMARKS_KEY("remarks");
const std::string IdentifiedObject::DEPRECATED_KEY("deprecated");

// ---------------------------------------------------------------------------

const std::string Identifier::AUTHORITY_KEY("authority");
const std::string Identifier::CODE_KEY("code");
const std::string Identifier::CODESPACE_KEY("codespace");
const std::string Identifier::VERSION_KEY("version");
const std::string Identifier::DESCRIPTION_KEY("description");

// ---------------------------------------------------------------------------

const std::string ObjectUsage::SCOPE_KEY("scope");
const std::string ObjectUsage::DOMAIN_OF_VALIDITY_KEY("domainOfValidity");
const std::string ObjectUsage::OBJECT_DOMAIN_KEY("objectUsage");

// ---------------------------------------------------------------------------

#define DEFINE_WKT_CONSTANT(x) const std::string WKTConstants::x(#x)

DEFINE_WKT_CONSTANT(GEOCCS);
DEFINE_WKT_CONSTANT(GEOGCS);
DEFINE_WKT_CONSTANT(DATUM);
DEFINE_WKT_CONSTANT(UNIT);
DEFINE_WKT_CONSTANT(SPHEROID);
DEFINE_WKT_CONSTANT(AXIS);
DEFINE_WKT_CONSTANT(PRIMEM);
DEFINE_WKT_CONSTANT(AUTHORITY);
DEFINE_WKT_CONSTANT(PROJCS);
DEFINE_WKT_CONSTANT(PROJECTION);
DEFINE_WKT_CONSTANT(PARAMETER);

DEFINE_WKT_CONSTANT(GEODCRS);
DEFINE_WKT_CONSTANT(LENGTHUNIT);
DEFINE_WKT_CONSTANT(ANGLEUNIT);
DEFINE_WKT_CONSTANT(SCALEUNIT);
DEFINE_WKT_CONSTANT(ELLIPSOID);
DEFINE_WKT_CONSTANT(CS);
DEFINE_WKT_CONSTANT(ID);
DEFINE_WKT_CONSTANT(PROJCRS);
DEFINE_WKT_CONSTANT(BASEGEODCRS);
DEFINE_WKT_CONSTANT(MERIDIAN);
DEFINE_WKT_CONSTANT(ORDER);
DEFINE_WKT_CONSTANT(ANCHOR);
DEFINE_WKT_CONSTANT(CONVERSION);
DEFINE_WKT_CONSTANT(METHOD);
DEFINE_WKT_CONSTANT(REMARK);
DEFINE_WKT_CONSTANT(GEOGCRS);
DEFINE_WKT_CONSTANT(BASEGEOGCRS);
DEFINE_WKT_CONSTANT(SCOPE);
DEFINE_WKT_CONSTANT(AREA);
DEFINE_WKT_CONSTANT(BBOX);

DEFINE_WKT_CONSTANT(GEODETICCRS);
DEFINE_WKT_CONSTANT(GEODETICDATUM);
DEFINE_WKT_CONSTANT(PROJECTEDCRS);
DEFINE_WKT_CONSTANT(PRIMEMERIDIAN);
DEFINE_WKT_CONSTANT(GEOGRAPHICCRS);
DEFINE_WKT_CONSTANT(GEODETICREFERENCEFRAME);

// ---------------------------------------------------------------------------

const UnitOfMeasure UnitOfMeasure::NONE("", 1.0, UnitOfMeasure::Type::NONE);
const UnitOfMeasure UnitOfMeasure::SCALE_UNITY("unity", 1.0,
                                               UnitOfMeasure::Type::SCALE,
                                               "EPSG", "9201");
const UnitOfMeasure UnitOfMeasure::METRE("metre", 1.0,
                                         UnitOfMeasure::Type::LINEAR, "EPSG",
                                         "9001");
const UnitOfMeasure UnitOfMeasure::DEGREE("degree", M_PI / 180.,
                                          UnitOfMeasure::Type::ANGULAR, "EPSG",
                                          "9122");
const UnitOfMeasure UnitOfMeasure::GRAD("grad", M_PI / 200.,
                                        UnitOfMeasure::Type::ANGULAR, "EPSG",
                                        "9105");
const UnitOfMeasure UnitOfMeasure::RADIAN("radian", 1.0,
                                          UnitOfMeasure::Type::ANGULAR, "EPSG",
                                          "9101");

// ---------------------------------------------------------------------------

std::map<std::string, const AxisDirection *>
    AxisDirection::axisDirectionRegistry;
std::set<std::string> AxisDirection::axisDirectionKeys;

const AxisDirection AxisDirection::NORTH("north");
const AxisDirection AxisDirection::NORTH_NORTH_EAST("northNorthEast");
const AxisDirection AxisDirection::NORTH_EAST("northEast");
const AxisDirection AxisDirection::EAST_NORTH_EAST("eastNorthEast");
const AxisDirection AxisDirection::EAST("east");
const AxisDirection AxisDirection::EAST_SOUTH_EAST("eastSouthEast");
const AxisDirection AxisDirection::SOUTH_EAST("southEast");
const AxisDirection AxisDirection::SOUTH_SOUTH_EAST("southSouthEast");
const AxisDirection AxisDirection::SOUTH("south");
const AxisDirection AxisDirection::SOUTH_SOUTH_WEST("southSouthWest");
const AxisDirection AxisDirection::SOUTH_WEST("southWest");
const AxisDirection AxisDirection::WEST_SOUTH_WEST("westSouthWest");
const AxisDirection AxisDirection::WEST("west");
const AxisDirection AxisDirection::WEST_NORTH_WEST("westNorthWest");
const AxisDirection AxisDirection::NORTH_WEST("northWest");
const AxisDirection AxisDirection::NORTH_NORTH_WEST("northNorthWest");
const AxisDirection AxisDirection::UP("up");
const AxisDirection AxisDirection::DOWN("down");
const AxisDirection AxisDirection::GEOCENTRIC_X("geocentricX");
const AxisDirection AxisDirection::GEOCENTRIC_Y("geocentricY");
const AxisDirection AxisDirection::GEOCENTRIC_Z("geocentricZ");
const AxisDirection AxisDirection::COLUMN_POSITIVE("columnPositive");
const AxisDirection AxisDirection::COLUMN_NEGATIVE("columnNegative");
const AxisDirection AxisDirection::ROW_POSITIVE("rowPositive");
const AxisDirection AxisDirection::ROW_NEGATIVE("rowNegative");
const AxisDirection AxisDirection::DISPLAY_RIGHT("displayRight");
const AxisDirection AxisDirection::DISPLAY_LEFT("displayLeft");
const AxisDirection AxisDirection::DISPLAY_UP("displayUp");
const AxisDirection AxisDirection::DISPLAY_DOWN("displayDown");
const AxisDirection AxisDirection::FORWARD("forward");
const AxisDirection AxisDirection::AFT("aft");
const AxisDirection AxisDirection::PORT("port");
const AxisDirection AxisDirection::STARBOARD("starboard");
const AxisDirection AxisDirection::CLOCKWISE("clockwise");
const AxisDirection AxisDirection::COUNTER_CLOCKWISE("counterClockwise");
const AxisDirection AxisDirection::TOWARDS("towards");
const AxisDirection AxisDirection::AWAY_FROM("awayFrom");
const AxisDirection AxisDirection::FUTURE("future");
const AxisDirection AxisDirection::PAST("past");
const AxisDirection AxisDirection::UNSPECIFIED("unspecified");

// ---------------------------------------------------------------------------

std::map<std::string, const AxisDirectionWKT1 *>
    AxisDirectionWKT1::axisDirectionWKT1Registry;
std::set<std::string> AxisDirectionWKT1::axisDirectionWKT1Keys;

const AxisDirectionWKT1 AxisDirectionWKT1::NORTH("NORTH");
const AxisDirectionWKT1 AxisDirectionWKT1::EAST("EAST");
const AxisDirectionWKT1 AxisDirectionWKT1::SOUTH("SOUTH");
const AxisDirectionWKT1 AxisDirectionWKT1::WEST("WEST");
const AxisDirectionWKT1 AxisDirectionWKT1::UP("UP");
const AxisDirectionWKT1 AxisDirectionWKT1::DOWN("DOWN");
const AxisDirectionWKT1 AxisDirectionWKT1::OTHER("OTHER");

// ---------------------------------------------------------------------------

const std::string AxisName::Longitude("Longitude");
const std::string AxisName::Latitude("Latitude");
const std::string AxisName::Easting("Easting");
const std::string AxisName::Northing("Northing");
const std::string AxisName::Ellipsoidal_height("Ellipsoidal height");
const std::string AxisName::Geocentric_X("Geocentric X");
const std::string AxisName::Geocentric_Y("Geocentric Y");
const std::string AxisName::Geocentric_Z("Geocentric Z");

// ---------------------------------------------------------------------------

const std::string AxisAbbreviation::lon("lon");
const std::string AxisAbbreviation::lat("lat");
const std::string AxisAbbreviation::E("E");
const std::string AxisAbbreviation::N("N");
const std::string AxisAbbreviation::h("h");
const std::string AxisAbbreviation::X("X");
const std::string AxisAbbreviation::Y("Y");
const std::string AxisAbbreviation::Z("Z");

// ---------------------------------------------------------------------------

const PrimeMeridianNNPtr
    PrimeMeridian::GREENWICH(PrimeMeridian::createGREENWICH());

// ---------------------------------------------------------------------------

const EllipsoidNNPtr Ellipsoid::EPSG_7030(Ellipsoid::createEPSG_7030());

// ---------------------------------------------------------------------------

const GeodeticReferenceFrameNNPtr GeodeticReferenceFrame::EPSG_6326(
    GeodeticReferenceFrame::createEPSG_6326());

// ---------------------------------------------------------------------------

const GeographicCRSNNPtr
    GeographicCRS::EPSG_4326(GeographicCRS::createEPSG_4326());
const GeographicCRSNNPtr
    GeographicCRS::EPSG_4807(GeographicCRS::createEPSG_4807());
const GeographicCRSNNPtr
    GeographicCRS::EPSG_4979(GeographicCRS::createEPSG_4979());

// ---------------------------------------------------------------------------

// Make sure to instanciate all used instances of optional
NS_PROJ_START
namespace util {
template class optional<std::string>;
template class optional<double>;
template class optional<Citation>;
template class optional<Identifier>;
template class optional<DataEpoch>;
template class optional<IdentifiedObject>;
template class optional<Scale>;
template class optional<Length>;
}
NS_PROJ_END
