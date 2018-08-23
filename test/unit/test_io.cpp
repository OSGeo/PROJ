/******************************************************************************
 *
 * Project:  PROJ
 * Purpose:  Test ISO19111:2018 implementation
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

#include "gtest_include.h"

#include "proj/common.hpp"
#include "proj/coordinateoperation.hpp"
#include "proj/coordinatesystem.hpp"
#include "proj/crs.hpp"
#include "proj/datum.hpp"
#include "proj/io.hpp"
#include "proj/metadata.hpp"
#include "proj/util.hpp"

#include <string>

using namespace osgeo::proj::common;
using namespace osgeo::proj::crs;
using namespace osgeo::proj::cs;
using namespace osgeo::proj::datum;
using namespace osgeo::proj::io;
using namespace osgeo::proj::metadata;
using namespace osgeo::proj::operation;
using namespace osgeo::proj::util;

// ---------------------------------------------------------------------------

TEST(io, wkt_parsing) {
    {
        auto n = WKTNode::createFrom("MYNODE[]");
        EXPECT_EQ(n->value(), "MYNODE");
        EXPECT_TRUE(n->children().empty());
        EXPECT_EQ(n->toString(), "MYNODE");
    }
    {
        auto n = WKTNode::createFrom("  MYNODE  [  ]  ");
        EXPECT_EQ(n->value(), "MYNODE");
        EXPECT_TRUE(n->children().empty());
    }
    EXPECT_THROW(WKTNode::createFrom(""), ParsingException);
    EXPECT_THROW(WKTNode::createFrom("x"), ParsingException);
    EXPECT_THROW(WKTNode::createFrom("x,"), ParsingException);
    EXPECT_THROW(WKTNode::createFrom("x["), ParsingException);
    EXPECT_THROW(WKTNode::createFrom("["), ParsingException);

    {
        auto n = WKTNode::createFrom("MYNODE[\"x\"]");
        EXPECT_EQ(n->value(), "MYNODE");
        ASSERT_EQ(n->children().size(), 1);
        EXPECT_EQ(n->children()[0]->value(), "\"x\"");
        EXPECT_EQ(n->toString(), "MYNODE[\"x\"]");
    }

    EXPECT_THROW(WKTNode::createFrom("MYNODE[\"x\""), ParsingException);

    {
        auto n = WKTNode::createFrom("MYNODE[  \"x\"   ]");
        EXPECT_EQ(n->value(), "MYNODE");
        ASSERT_EQ(n->children().size(), 1);
        EXPECT_EQ(n->children()[0]->value(), "\"x\"");
    }

    {
        auto n = WKTNode::createFrom("MYNODE[\"x[\",1]");
        EXPECT_EQ(n->value(), "MYNODE");
        ASSERT_EQ(n->children().size(), 2);
        EXPECT_EQ(n->children()[0]->value(), "\"x[\"");
        EXPECT_EQ(n->children()[1]->value(), "1");
        EXPECT_EQ(n->toString(), "MYNODE[\"x[\",1]");
    }

    EXPECT_THROW(WKTNode::createFrom("MYNODE[\"x\","), ParsingException);

    {
        auto n = WKTNode::createFrom("A[B[y]]");
        EXPECT_EQ(n->value(), "A");
        ASSERT_EQ(n->children().size(), 1);
        EXPECT_EQ(n->children()[0]->value(), "B");
        ASSERT_EQ(n->children()[0]->children().size(), 1);
        EXPECT_EQ(n->children()[0]->children()[0]->value(), "y");
        EXPECT_EQ(n->toString(), "A[B[y]]");
    }

    EXPECT_THROW(WKTNode::createFrom("A[B["), ParsingException);

    std::string str;
    for (int i = 0; i < 17; i++) {
        str = "A[" + str + "]";
    }
    EXPECT_THROW(WKTNode::createFrom(str), ParsingException);
}

// ---------------------------------------------------------------------------

TEST(io, wkt_parsing_with_parenthesis) {

    auto n = WKTNode::createFrom("A(\"x\",B(\"y\"))");
    EXPECT_EQ(n->toString(), "A[\"x\",B[\"y\"]]");
}

// ---------------------------------------------------------------------------

TEST(io, wkt_parsing_with_double_quotes_inside) {

    auto n = WKTNode::createFrom("A[\"xy\"\"z\"]");
    EXPECT_EQ(n->children()[0]->value(), "\"xy\"z\"");
    EXPECT_EQ(n->toString(), "A[\"xy\"\"z\"]");

    EXPECT_THROW(WKTNode::createFrom("A[\"x\""), ParsingException);
}

// ---------------------------------------------------------------------------

TEST(io, wkt_parsing_with_printed_quotes) {
    static const std::string startPrintedQuote("\xE2\x80\x9C");
    static const std::string endPrintedQuote("\xE2\x80\x9D");

    auto n = WKTNode::createFrom("A[" + startPrintedQuote + "x" +
                                 endPrintedQuote + "]");
    EXPECT_EQ(n->children()[0]->value(), "\"x\"");
    EXPECT_EQ(n->toString(), "A[\"x\"]");
}

// ---------------------------------------------------------------------------

TEST(wkt_parse, datum_with_ANCHOR) {
    auto obj = WKTParser().createFromWKT(
        "DATUM[\"WGS_1984 with anchor\",\n"
        "    ELLIPSOID[\"WGS 84\",6378137,298.257223563,\n"
        "        LENGTHUNIT[\"metre\",1,\n"
        "            ID[\"EPSG\",9001]],\n"
        "        ID[\"EPSG\",7030]],\n"
        "    ANCHOR[\"My anchor\"]]");
    auto datum = nn_dynamic_pointer_cast<GeodeticReferenceFrame>(obj);
    ASSERT_TRUE(datum != nullptr);
    auto anchor = datum->anchorDefinition();
    EXPECT_TRUE(anchor.has_value());
    EXPECT_EQ(*anchor, "My anchor");
}

// ---------------------------------------------------------------------------

static void checkEPSG_4326(GeographicCRSPtr crs, bool latLong = true,
                           bool checkEPSGCodes = true) {
    if (checkEPSGCodes) {
        ASSERT_EQ(crs->identifiers().size(), 1);
        EXPECT_EQ(crs->identifiers()[0]->code(), "4326");
        EXPECT_EQ(*(crs->identifiers()[0]->codeSpace()), "EPSG");
    }
    EXPECT_EQ(*(crs->name()->description()), "WGS 84");

    auto cs = crs->coordinateSystem();
    ASSERT_EQ(cs->axisList().size(), 2);
    if (latLong) {
        EXPECT_EQ(*(cs->axisList()[0]->name()->description()), "Latitude");
        EXPECT_EQ(cs->axisList()[0]->abbreviation(), "lat");
        EXPECT_EQ(cs->axisList()[0]->direction(), AxisDirection::NORTH);

        EXPECT_EQ(*(cs->axisList()[1]->name()->description()), "Longitude");
        EXPECT_EQ(cs->axisList()[1]->abbreviation(), "lon");
        EXPECT_EQ(cs->axisList()[1]->direction(), AxisDirection::EAST);
    } else {
        EXPECT_EQ(*(cs->axisList()[0]->name()->description()), "Longitude");
        EXPECT_EQ(cs->axisList()[0]->abbreviation(), "lon");
        EXPECT_EQ(cs->axisList()[0]->direction(), AxisDirection::EAST);

        EXPECT_EQ(*(cs->axisList()[1]->name()->description()), "Latitude");
        EXPECT_EQ(cs->axisList()[1]->abbreviation(), "lat");
        EXPECT_EQ(cs->axisList()[1]->direction(), AxisDirection::NORTH);
    }

    auto datum = crs->datum();
    if (checkEPSGCodes) {
        ASSERT_EQ(datum->identifiers().size(), 1);
        EXPECT_EQ(datum->identifiers()[0]->code(), "6326");
        EXPECT_EQ(*(datum->identifiers()[0]->codeSpace()), "EPSG");
    }
    EXPECT_EQ(*(datum->name()->description()), "World Geodetic System 1984");

    auto ellipsoid = datum->ellipsoid();
    EXPECT_EQ(ellipsoid->semiMajorAxis().value(), 6378137.0);
    EXPECT_EQ(ellipsoid->semiMajorAxis().unit(), UnitOfMeasure::METRE);
    EXPECT_EQ(ellipsoid->inverseFlattening()->value(), 298.257223563);
    if (checkEPSGCodes) {
        ASSERT_EQ(ellipsoid->identifiers().size(), 1);
        EXPECT_EQ(ellipsoid->identifiers()[0]->code(), "7030");
        EXPECT_EQ(*(ellipsoid->identifiers()[0]->codeSpace()), "EPSG");
    }
    EXPECT_EQ(*(ellipsoid->name()->description()), "WGS 84");
}

// ---------------------------------------------------------------------------

TEST(wkt_parse, wkt1_EPSG_4326) {
    auto obj = WKTParser().createFromWKT(
        "GEOGCS[\"WGS 84\","
        "    DATUM[\"WGS_1984\","
        "        SPHEROID[\"WGS 84\",6378137,298.257223563,"
        "            AUTHORITY[\"EPSG\",\"7030\"]],"
        "        AUTHORITY[\"EPSG\",\"6326\"]],"
        "    PRIMEM[\"Greenwich\",0,"
        "        AUTHORITY[\"EPSG\",\"8901\"]],"
        "    UNIT[\"degree\",0.0174532925199433,"
        "        AUTHORITY[\"EPSG\",\"9122\"]],"
        "    AUTHORITY[\"EPSG\",\"4326\"]]");
    auto crs = nn_dynamic_pointer_cast<GeographicCRS>(obj);
    ASSERT_TRUE(crs != nullptr);
    checkEPSG_4326(crs, false /* longlat order */);
}

// ---------------------------------------------------------------------------

TEST(wkt_parse, wkt1_EPSG_4807_grad_mess) {
    auto obj = WKTParser().createFromWKT(
        "GEOGCS[\"NTF (Paris)\",\n"
        "    DATUM[\"Nouvelle_Triangulation_Francaise_Paris\",\n"
        "        SPHEROID[\"Clarke 1880 (IGN)\",6378249.2,293.466021293627,\n"
        "            AUTHORITY[\"EPSG\",\"6807\"]],\n"
        "        AUTHORITY[\"EPSG\",\"6807\"]],\n"
        /* WKT1_GDAL weirdness: PRIMEM is converted to degree */
        "    PRIMEM[\"Paris\",2.33722917,\n"
        "        AUTHORITY[\"EPSG\",\"8903\"]],\n"
        "    UNIT[\"grad\",0.015707963267949,\n"
        "        AUTHORITY[\"EPSG\",9105]],\n"
        "    AXIS[\"latitude\",NORTH],\n"
        "    AXIS[\"longitude\",EAST],\n"
        "    AUTHORITY[\"EPSG\",\"4807\"]]");
    auto crs = nn_dynamic_pointer_cast<GeographicCRS>(obj);
    ASSERT_TRUE(crs != nullptr);

    auto datum = crs->datum();
    auto primem = datum->primeMeridian();
    ASSERT_EQ(primem->longitude().unit(), UnitOfMeasure::GRAD);
    // Check that we have corrected the value that was in degree into grad.
    ASSERT_EQ(primem->longitude().value(), 2.5969213);
}

// ---------------------------------------------------------------------------

static std::string contentWKT2_EPSG_4326(
    "[\"WGS 84\",\n"
    "    DATUM[\"World Geodetic System 1984\",\n"
    "        ELLIPSOID[\"WGS 84\",6378137,298.257223563,\n"
    "            LENGTHUNIT[\"metre\",1,\n"
    "                ID[\"EPSG\",9001]],\n"
    "            ID[\"EPSG\",7030]],\n"
    "        ID[\"EPSG\",6326]],\n"
    "    PRIMEM[\"Greenwich\",0,\n"
    "        ANGLEUNIT[\"degree\",0.0174532925199433,\n"
    "            ID[\"EPSG\",9122]],\n"
    "        ID[\"EPSG\",8901]],\n"
    "    CS[ellipsoidal,2],\n"
    "        AXIS[\"latitude\",north,\n"
    "            ORDER[1],\n"
    "            ANGLEUNIT[\"degree\",0.0174532925199433,\n"
    "                ID[\"EPSG\",9122]]],\n"
    "        AXIS[\"longitude\",east,\n"
    "            ORDER[2],\n"
    "            ANGLEUNIT[\"degree\",0.0174532925199433,\n"
    "                ID[\"EPSG\",9122]]],\n"
    "    ID[\"EPSG\",4326]]");

// ---------------------------------------------------------------------------

TEST(wkt_parse, wkt2_GEODCRS_EPSG_4326) {
    auto obj = WKTParser().createFromWKT("GEODCRS" + contentWKT2_EPSG_4326);
    auto crs = nn_dynamic_pointer_cast<GeographicCRS>(obj);
    ASSERT_TRUE(crs != nullptr);
    checkEPSG_4326(crs);
}

// ---------------------------------------------------------------------------

TEST(wkt_parse, wkt2_long_GEODETICCRS_EPSG_4326) {
    auto obj = WKTParser().createFromWKT("GEODETICCRS" + contentWKT2_EPSG_4326);
    auto crs = nn_dynamic_pointer_cast<GeographicCRS>(obj);
    ASSERT_TRUE(crs != nullptr);
    checkEPSG_4326(crs);
}

// ---------------------------------------------------------------------------

TEST(wkt_parse, wkt2_2018_GEOGCRS_EPSG_4326) {
    auto obj = WKTParser().createFromWKT("GEOGCRS" + contentWKT2_EPSG_4326);
    auto crs = nn_dynamic_pointer_cast<GeographicCRS>(obj);
    ASSERT_TRUE(crs != nullptr);
    checkEPSG_4326(crs);
}

// ---------------------------------------------------------------------------

TEST(wkt_parse, wkt2_2018_long_GEOGRAPHICCRS_EPSG_4326) {
    auto obj =
        WKTParser().createFromWKT("GEOGRAPHICCRS" + contentWKT2_EPSG_4326);
    auto crs = nn_dynamic_pointer_cast<GeographicCRS>(obj);
    ASSERT_TRUE(crs != nullptr);
    checkEPSG_4326(crs);
}

// ---------------------------------------------------------------------------

TEST(wkt_parse, wkt2_simplified_EPSG_4326) {
    auto obj = WKTParser().createFromWKT(
        "GEODCRS[\"WGS 84\",\n"
        "    DATUM[\"World Geodetic System 1984\",\n"
        "        ELLIPSOID[\"WGS 84\",6378137,298.257223563]],\n"
        "    CS[ellipsoidal,2],\n"
        "        AXIS[\"latitude (lat)\",north],\n" // test "name
                                                    // (abbreviation)"
        "        AXIS[\"longitude (lon)\",east],\n"
        "        UNIT[\"degree\",0.0174532925199433],\n"
        "    ID[\"EPSG\",4326]]");
    auto crs = nn_dynamic_pointer_cast<GeographicCRS>(obj);
    ASSERT_TRUE(crs != nullptr);
    checkEPSG_4326(crs, true /* latlong */, false /* no EPSG codes */);
}

// ---------------------------------------------------------------------------

TEST(wkt_parse, wkt2_GEODETICDATUM) {
    auto obj = WKTParser().createFromWKT(
        "GEODCRS[\"WGS 84\",\n"
        "    GEODETICDATUM[\"World Geodetic System 1984\",\n"
        "        ELLIPSOID[\"WGS 84\",6378137,298.257223563]],\n"
        "    CS[ellipsoidal,2],\n"
        "        AXIS[\"(lat)\",north],\n" // test "(abbreviation)"
        "        AXIS[\"(lon)\",east],\n"
        "        UNIT[\"degree\",0.0174532925199433],\n"
        "    ID[\"EPSG\",4326]]");
    auto crs = nn_dynamic_pointer_cast<GeographicCRS>(obj);
    ASSERT_TRUE(crs != nullptr);
    checkEPSG_4326(crs, true /* latlong */, false /* no EPSG codes */);
}

// ---------------------------------------------------------------------------

TEST(wkt_parse, wkt2_GEODETICREFERENCEFRAME) {
    auto obj = WKTParser().createFromWKT(
        "GEODCRS[\"WGS 84\",\n"
        "    GEODETICREFERENCEFRAME[\"World Geodetic System 1984\",\n"
        "        ELLIPSOID[\"WGS 84\",6378137,298.257223563]],\n"
        "    CS[ellipsoidal,2],\n"
        "        AXIS[\"latitude\",north],\n"
        "        AXIS[\"longitude\",east],\n"
        "        UNIT[\"degree\",0.0174532925199433],\n"
        "    ID[\"EPSG\",4326]]");
    auto crs = nn_dynamic_pointer_cast<GeographicCRS>(obj);
    ASSERT_TRUE(crs != nullptr);
    checkEPSG_4326(crs, true /* latlong */, false /* no EPSG codes */);
}

// ---------------------------------------------------------------------------

static void checkEPSG_4979(GeographicCRSPtr crs) {
    ASSERT_EQ(crs->identifiers().size(), 1);
    EXPECT_EQ(crs->identifiers()[0]->code(), "4979");
    EXPECT_EQ(*(crs->identifiers()[0]->codeSpace()), "EPSG");
    EXPECT_EQ(*(crs->name()->description()), "WGS 84");

    auto cs = crs->coordinateSystem();
    ASSERT_EQ(cs->axisList().size(), 3);
    EXPECT_EQ(*(cs->axisList()[0]->name()->description()), "Latitude");
    EXPECT_EQ(cs->axisList()[0]->abbreviation(), "lat");
    EXPECT_EQ(cs->axisList()[0]->direction(), AxisDirection::NORTH);

    EXPECT_EQ(*(cs->axisList()[1]->name()->description()), "Longitude");
    EXPECT_EQ(cs->axisList()[1]->abbreviation(), "lon");
    EXPECT_EQ(cs->axisList()[1]->direction(), AxisDirection::EAST);

    EXPECT_EQ(*(cs->axisList()[2]->name()->description()),
              "Ellipsoidal height");
    EXPECT_EQ(cs->axisList()[2]->abbreviation(), "h");
    EXPECT_EQ(cs->axisList()[2]->direction(), AxisDirection::UP);

    auto datum = crs->datum();
    EXPECT_EQ(*(datum->name()->description()), "World Geodetic System 1984");

    auto ellipsoid = datum->ellipsoid();
    EXPECT_EQ(ellipsoid->semiMajorAxis().value(), 6378137.0);
    EXPECT_EQ(ellipsoid->semiMajorAxis().unit(), UnitOfMeasure::METRE);
    EXPECT_EQ(ellipsoid->inverseFlattening()->value(), 298.257223563);
    EXPECT_EQ(*(ellipsoid->name()->description()), "WGS 84");
}

// ---------------------------------------------------------------------------

TEST(wkt_parse, wkt2_EPSG_4979) {
    auto obj = WKTParser().createFromWKT(
        "GEODCRS[\"WGS 84\",\n"
        "    DATUM[\"World Geodetic System 1984\",\n"
        "        ELLIPSOID[\"WGS 84\",6378137,298.257223563]],\n"
        "    CS[ellipsoidal,3],\n"
        "        AXIS[\"latitude\",north,\n"
        "            UNIT[\"degree\",0.0174532925199433]],\n"
        "        AXIS[\"longitude\",east,\n"
        "            UNIT[\"degree\",0.0174532925199433]],\n"
        "        AXIS[\"ellipsoidal height\",up,\n"
        "            UNIT[\"metre\",1]],\n"
        "    ID[\"EPSG\",4979]]");
    auto crs = nn_dynamic_pointer_cast<GeographicCRS>(obj);
    ASSERT_TRUE(crs != nullptr);
    checkEPSG_4979(crs);
}

// ---------------------------------------------------------------------------

static void checkGeocentric(GeodeticCRSPtr crs) {
    // Explicitly check this is NOT a GeographicCRS
    EXPECT_TRUE(!dynamic_cast<GeographicCRS *>(crs.get()));

    EXPECT_EQ(*(crs->name()->description()), "WGS 84 (geocentric)");

    EXPECT_TRUE(crs->isGeocentric());
    auto cs = crs->coordinateSystem();
    ASSERT_EQ(cs->axisList().size(), 3);

    EXPECT_EQ(*(cs->axisList()[0]->name()->description()), "Geocentric X");
    EXPECT_EQ(cs->axisList()[0]->abbreviation(), "X");
    EXPECT_EQ(cs->axisList()[0]->direction(), AxisDirection::GEOCENTRIC_X);

    EXPECT_EQ(*(cs->axisList()[1]->name()->description()), "Geocentric Y");
    EXPECT_EQ(cs->axisList()[1]->abbreviation(), "Y");
    EXPECT_EQ(cs->axisList()[1]->direction(), AxisDirection::GEOCENTRIC_Y);

    EXPECT_EQ(*(cs->axisList()[2]->name()->description()), "Geocentric Z");
    EXPECT_EQ(cs->axisList()[2]->abbreviation(), "Z");
    EXPECT_EQ(cs->axisList()[2]->direction(), AxisDirection::GEOCENTRIC_Z);

    auto datum = crs->datum();
    EXPECT_EQ(*(datum->name()->description()), "World Geodetic System 1984");

    auto ellipsoid = datum->ellipsoid();
    EXPECT_EQ(ellipsoid->semiMajorAxis().value(), 6378137.0);
    EXPECT_EQ(ellipsoid->semiMajorAxis().unit(), UnitOfMeasure::METRE);
    EXPECT_EQ(ellipsoid->inverseFlattening()->value(), 298.257223563);
    EXPECT_EQ(*(ellipsoid->name()->description()), "WGS 84");

    auto primem = datum->primeMeridian();
    ASSERT_EQ(primem->longitude().unit(), UnitOfMeasure::DEGREE);
}

// ---------------------------------------------------------------------------

TEST(wkt_parse, wkt2_geocentric) {
    auto wkt = "GEODCRS[\"WGS 84 (geocentric)\",\n"
               "    DATUM[\"World Geodetic System 1984\",\n"
               "        ELLIPSOID[\"WGS 84\",6378137,298.257223563,\n"
               "            LENGTHUNIT[\"metre\",1,\n"
               "                ID[\"EPSG\",9001]],\n"
               "            ID[\"EPSG\",7030]],\n"
               "        ID[\"EPSG\",6326]],\n"
               "    PRIMEM[\"Greenwich\",0,\n"
               "        ANGLEUNIT[\"degree\",0.0174532925199433,\n"
               "            ID[\"EPSG\",9122]],\n"
               "        ID[\"EPSG\",8901]],\n"
               "    CS[Cartesian,3],\n"
               "        AXIS[\"(X)\",geocentricX,\n"
               "            ORDER[1],\n"
               "            LENGTHUNIT[\"metre\",1,\n"
               "                ID[\"EPSG\",9001]]],\n"
               "        AXIS[\"(Y)\",geocentricY,\n"
               "            ORDER[2],\n"
               "            LENGTHUNIT[\"metre\",1,\n"
               "                ID[\"EPSG\",9001]]],\n"
               "        AXIS[\"(Z)\",geocentricZ,\n"
               "            ORDER[3],\n"
               "            LENGTHUNIT[\"metre\",1,\n"
               "                ID[\"EPSG\",9001]]],\n"
               "    ID[\"EPSG\",4328]]";
    auto obj = WKTParser().createFromWKT(wkt);
    auto crs = nn_dynamic_pointer_cast<GeodeticCRS>(obj);
    ASSERT_TRUE(crs != nullptr);
    checkGeocentric(crs);
}

// ---------------------------------------------------------------------------

TEST(wkt_parse, wkt2_simplified_geocentric) {
    auto wkt = "GEODCRS[\"WGS 84 (geocentric)\",\n"
               "    DATUM[\"World Geodetic System 1984\",\n"
               "        ELLIPSOID[\"WGS 84\",6378137,298.257223563]],\n"
               "    CS[Cartesian,3],\n"
               "        AXIS[\"(X)\",geocentricX],\n"
               "        AXIS[\"(Y)\",geocentricY],\n"
               "        AXIS[\"(Z)\",geocentricZ],\n"
               "        UNIT[\"metre\",1],\n"
               "    ID[\"EPSG\",4328]]";
    auto obj = WKTParser().createFromWKT(wkt);
    auto crs = nn_dynamic_pointer_cast<GeodeticCRS>(obj);
    ASSERT_TRUE(crs != nullptr);
    checkGeocentric(crs);
}

// ---------------------------------------------------------------------------

TEST(wkt_parse, wkt1_geocentric) {
    auto wkt = "GEOCCS[\"WGS 84 (geocentric)\",\n"
               "    DATUM[\"WGS_1984\",\n"
               "        SPHEROID[\"WGS 84\",6378137,298.257223563,\n"
               "            AUTHORITY[\"EPSG\",\"7030\"]],\n"
               "        AUTHORITY[\"EPSG\",\"6326\"]],\n"
               "    PRIMEM[\"Greenwich\",0,\n"
               "        AUTHORITY[\"EPSG\",\"8901\"]],\n"
               "    UNIT[\"metre\",1,\n"
               "        AUTHORITY[\"EPSG\",9001]],\n"
               "    AXIS[\"Geocentric X\",OTHER],\n"
               "    AXIS[\"Geocentric Y\",OTHER],\n"
               "    AXIS[\"Geocentric Z\",NORTH],\n"
               "    AUTHORITY[\"EPSG\",\"4328\"]]";
    auto obj = WKTParser().createFromWKT(wkt);
    auto crs = nn_dynamic_pointer_cast<GeodeticCRS>(obj);
    ASSERT_TRUE(crs != nullptr);
    checkGeocentric(crs);
}

// ---------------------------------------------------------------------------

static void checkProjected(ProjectedCRSPtr crs, bool checkEPSGCodes = true) {
    EXPECT_EQ(*(crs->name()->description()), "WGS 84 / UTM zone 31N");
    ASSERT_EQ(crs->identifiers().size(), 1);
    EXPECT_EQ(crs->identifiers()[0]->code(), "32631");
    EXPECT_EQ(*(crs->identifiers()[0]->codeSpace()), "EPSG");

    auto geogCRS = nn_dynamic_pointer_cast<GeographicCRS>(crs->baseCRS());
    ASSERT_TRUE(geogCRS != nullptr);
    checkEPSG_4326(NN_CHECK_ASSERT(geogCRS), true, checkEPSGCodes);

    auto conversion = crs->derivingConversion();
    EXPECT_EQ(*(conversion->name()->description()), "UTM zone 31N");
    auto method = conversion->method();
    EXPECT_EQ(*(method->name()->description()), "Transverse Mercator");
    auto values = conversion->parameterValues();
    ASSERT_EQ(values.size(), 5);
    {
        const auto &opParamvalue =
            nn_dynamic_pointer_cast<OperationParameterValue>(values[0]);
        ASSERT_TRUE(opParamvalue);
        const auto &paramName =
            *(opParamvalue->parameter()->name()->description());
        const auto &parameterValue = opParamvalue->parameterValue();
        EXPECT_EQ(paramName, "Latitude of natural origin");
        EXPECT_EQ(parameterValue->type(), ParameterValue::Type::MEASURE);
        auto measure = parameterValue->value();
        EXPECT_EQ(measure.unit(), UnitOfMeasure::DEGREE);
        EXPECT_EQ(measure.value(), 0);
    }
    {
        const auto &opParamvalue =
            nn_dynamic_pointer_cast<OperationParameterValue>(values[1]);
        ASSERT_TRUE(opParamvalue);
        const auto &paramName =
            *(opParamvalue->parameter()->name()->description());
        const auto &parameterValue = opParamvalue->parameterValue();
        EXPECT_EQ(paramName, "Longitude of natural origin");
        EXPECT_EQ(parameterValue->type(), ParameterValue::Type::MEASURE);
        auto measure = parameterValue->value();
        EXPECT_EQ(measure.unit(), UnitOfMeasure::DEGREE);
        EXPECT_EQ(measure.value(), 3);
    }
    {
        const auto &opParamvalue =
            nn_dynamic_pointer_cast<OperationParameterValue>(values[2]);
        ASSERT_TRUE(opParamvalue);
        const auto &paramName =
            *(opParamvalue->parameter()->name()->description());
        const auto &parameterValue = opParamvalue->parameterValue();
        EXPECT_EQ(paramName, "Scale factor at natural origin");
        EXPECT_EQ(parameterValue->type(), ParameterValue::Type::MEASURE);
        auto measure = parameterValue->value();
        EXPECT_EQ(measure.unit(), UnitOfMeasure::SCALE_UNITY);
        EXPECT_EQ(measure.value(), 0.9996);
    }
    {
        const auto &opParamvalue =
            nn_dynamic_pointer_cast<OperationParameterValue>(values[3]);
        ASSERT_TRUE(opParamvalue);
        const auto &paramName =
            *(opParamvalue->parameter()->name()->description());
        const auto &parameterValue = opParamvalue->parameterValue();
        EXPECT_EQ(paramName, "False easting");
        EXPECT_EQ(parameterValue->type(), ParameterValue::Type::MEASURE);
        auto measure = parameterValue->value();
        EXPECT_EQ(measure.unit(), UnitOfMeasure::METRE);
        EXPECT_EQ(measure.value(), 500000);
    }
    {
        const auto &opParamvalue =
            nn_dynamic_pointer_cast<OperationParameterValue>(values[4]);
        ASSERT_TRUE(opParamvalue);
        const auto &paramName =
            *(opParamvalue->parameter()->name()->description());
        const auto &parameterValue = opParamvalue->parameterValue();
        EXPECT_EQ(paramName, "False northing");
        EXPECT_EQ(parameterValue->type(), ParameterValue::Type::MEASURE);
        auto measure = parameterValue->value();
        EXPECT_EQ(measure.unit(), UnitOfMeasure::METRE);
        EXPECT_EQ(measure.value(), 0);
    }

    auto cs = crs->coordinateSystem();
    ASSERT_EQ(cs->axisList().size(), 2);

    EXPECT_EQ(*(cs->axisList()[0]->name()->description()), "Easting");
    EXPECT_EQ(cs->axisList()[0]->abbreviation(), "E");
    EXPECT_EQ(cs->axisList()[0]->direction(), AxisDirection::EAST);

    EXPECT_EQ(*(cs->axisList()[1]->name()->description()), "Northing");
    EXPECT_EQ(cs->axisList()[1]->abbreviation(), "N");
    EXPECT_EQ(cs->axisList()[1]->direction(), AxisDirection::NORTH);
}

// ---------------------------------------------------------------------------

TEST(wkt_parse, wkt1_projected) {
    auto wkt = "PROJCS[\"WGS 84 / UTM zone 31N\",\n"
               "    GEOGCS[\"WGS 84\",\n"
               "        DATUM[\"WGS_1984\",\n"
               "            SPHEROID[\"WGS 84\",6378137,298.257223563,\n"
               "                AUTHORITY[\"EPSG\",\"7030\"]],\n"
               "            AUTHORITY[\"EPSG\",\"6326\"]],\n"
               "        PRIMEM[\"Greenwich\",0,\n"
               "            AUTHORITY[\"EPSG\",\"8901\"]],\n"
               "        UNIT[\"degree\",0.0174532925199433,\n"
               "            AUTHORITY[\"EPSG\",9122]],\n"
               "        AXIS[\"latitude\",NORTH],\n"
               "        AXIS[\"longitude\",EAST],\n"
               "        AUTHORITY[\"EPSG\",\"4326\"]],\n"
               "    PROJECTION[\"Transverse_Mercator\"],\n"
               "    PARAMETER[\"latitude_of_origin\",0],\n"
               "    PARAMETER[\"central_meridian\",3],\n"
               "    PARAMETER[\"scale_factor\",0.9996],\n"
               "    PARAMETER[\"false_easting\",500000],\n"
               "    PARAMETER[\"false_northing\",0],\n"
               "    UNIT[\"metre\",1,\n"
               "        AUTHORITY[\"EPSG\",9001]],\n"
               "    AXIS[\"(E)\",EAST],\n"
               "    AXIS[\"(N)\",NORTH],\n"
               "    AUTHORITY[\"EPSG\",\"32631\"]]";
    auto obj = WKTParser().createFromWKT(wkt);
    auto crs = nn_dynamic_pointer_cast<ProjectedCRS>(obj);
    ASSERT_TRUE(crs != nullptr);
    checkProjected(crs);
}

// ---------------------------------------------------------------------------

TEST(wkt_parse, wkt1_projected_no_axis) {
    auto wkt = "PROJCS[\"WGS 84 / UTM zone 31N\",\n"
               "    GEOGCS[\"WGS 84\",\n"
               "        DATUM[\"WGS_1984\",\n"
               "            SPHEROID[\"WGS 84\",6378137,298.257223563,\n"
               "                AUTHORITY[\"EPSG\",\"7030\"]],\n"
               "            AUTHORITY[\"EPSG\",\"6326\"]],\n"
               "        PRIMEM[\"Greenwich\",0,\n"
               "            AUTHORITY[\"EPSG\",\"8901\"]],\n"
               "        UNIT[\"degree\",0.0174532925199433,\n"
               "            AUTHORITY[\"EPSG\",9122]],\n"
               "        AXIS[\"latitude\",NORTH],\n"
               "        AXIS[\"longitude\",EAST],\n"
               "        AUTHORITY[\"EPSG\",\"4326\"]],\n"
               "    PROJECTION[\"Transverse_Mercator\"],\n"
               "    PARAMETER[\"latitude_of_origin\",0],\n"
               "    PARAMETER[\"central_meridian\",3],\n"
               "    PARAMETER[\"scale_factor\",0.9996],\n"
               "    PARAMETER[\"false_easting\",500000],\n"
               "    PARAMETER[\"false_northing\",0],\n"
               "    UNIT[\"metre\",1,\n"
               "        AUTHORITY[\"EPSG\",9001]],\n"
               "    AUTHORITY[\"EPSG\",\"32631\"]]";
    auto obj = WKTParser().createFromWKT(wkt);
    auto crs = nn_dynamic_pointer_cast<ProjectedCRS>(obj);
    ASSERT_TRUE(crs != nullptr);
    checkProjected(crs);
}

// ---------------------------------------------------------------------------

TEST(wkt_parse, wkt2_projected) {
    auto wkt = "PROJCRS[\"WGS 84 / UTM zone 31N\",\n"
               "    BASEGEODCRS[\"WGS 84\",\n"
               "        DATUM[\"World Geodetic System 1984\",\n"
               "            ELLIPSOID[\"WGS 84\",6378137,298.257223563,\n"
               "                LENGTHUNIT[\"metre\",1,\n"
               "                    ID[\"EPSG\",9001]],\n"
               "                ID[\"EPSG\",7030]],\n"
               "            ID[\"EPSG\",6326]],\n"
               "        PRIMEM[\"Greenwich\",0,\n"
               "            ANGLEUNIT[\"degree\",0.0174532925199433,\n"
               "                ID[\"EPSG\",9122]],\n"
               "            ID[\"EPSG\",8901]]],\n"
               "    CONVERSION[\"UTM zone 31N\",\n"
               "        METHOD[\"Transverse Mercator\",\n"
               "            ID[\"EPSG\",9807]],\n"
               "        PARAMETER[\"Latitude of natural origin\",0,\n"
               "            ANGLEUNIT[\"degree\",0.0174532925199433,\n"
               "                ID[\"EPSG\",9122]],\n"
               "            ID[\"EPSG\",8801]],\n"
               "        PARAMETER[\"Longitude of natural origin\",3,\n"
               "            ANGLEUNIT[\"degree\",0.0174532925199433,\n"
               "                ID[\"EPSG\",9122]],\n"
               "            ID[\"EPSG\",8802]],\n"
               "        PARAMETER[\"Scale factor at natural origin\",0.9996,\n"
               "            SCALEUNIT[\"unity\",1,\n"
               "                ID[\"EPSG\",9201]],\n"
               "            ID[\"EPSG\",8805]],\n"
               "        PARAMETER[\"False easting\",500000,\n"
               "            LENGTHUNIT[\"metre\",1,\n"
               "                ID[\"EPSG\",9001]],\n"
               "            ID[\"EPSG\",8806]],\n"
               "        PARAMETER[\"False northing\",0,\n"
               "            LENGTHUNIT[\"metre\",1,\n"
               "                ID[\"EPSG\",9001]],\n"
               "            ID[\"EPSG\",8807]],\n"
               "        ID[\"EPSG\",16031]],\n"
               "    CS[Cartesian,2],\n"
               "        AXIS[\"(E)\",east,\n"
               "            ORDER[1],\n"
               "            LENGTHUNIT[\"metre\",1,\n"
               "                ID[\"EPSG\",9001]]],\n"
               "        AXIS[\"(N)\",north,\n"
               "            ORDER[2],\n"
               "            LENGTHUNIT[\"metre\",1,\n"
               "                ID[\"EPSG\",9001]]],\n"
               "    ID[\"EPSG\",32631]]";
    auto obj = WKTParser().createFromWKT(wkt);
    auto crs = nn_dynamic_pointer_cast<ProjectedCRS>(obj);
    ASSERT_TRUE(crs != nullptr);
    checkProjected(crs, /*checkEPSGCodes = */ false);
}

// ---------------------------------------------------------------------------

TEST(wkt_parse, wkt2_2018_simplified_projected) {
    auto wkt = "PROJCRS[\"WGS 84 / UTM zone 31N\",\n"
               "    BASEGEOGCRS[\"WGS 84\",\n"
               "        DATUM[\"World Geodetic System 1984\",\n"
               "            ELLIPSOID[\"WGS 84\",6378137,298.257223563]],\n"
               "        UNIT[\"degree\",0.0174532925199433]],\n"
               "    CONVERSION[\"UTM zone 31N\",\n"
               "        METHOD[\"Transverse Mercator\"],\n"
               "        PARAMETER[\"Latitude of natural origin\",0],\n"
               "        PARAMETER[\"Longitude of natural origin\",3],\n"
               "        PARAMETER[\"Scale factor at natural origin\",0.9996],\n"
               "        PARAMETER[\"False easting\",500000],\n"
               "        PARAMETER[\"False northing\",0]],\n"
               "    CS[Cartesian,2],\n"
               "        AXIS[\"(E)\",east],\n"
               "        AXIS[\"(N)\",north],\n"
               "        UNIT[\"metre\",1],\n"
               "    ID[\"EPSG\",32631]]";
    auto obj = WKTParser().createFromWKT(wkt);
    auto crs = nn_dynamic_pointer_cast<ProjectedCRS>(obj);
    ASSERT_TRUE(crs != nullptr);
    checkProjected(crs, /*checkEPSGCodes = */ false);
}

// ---------------------------------------------------------------------------

TEST(crs, projected_angular_unit_from_primem) {
    auto obj = WKTParser().createFromWKT(
        "PROJCRS[\"NTF (Paris) / Lambert Nord France\",\n"
        "  BASEGEODCRS[\"NTF (Paris)\",\n"
        "    DATUM[\"Nouvelle Triangulation Francaise (Paris)\",\n"
        "      ELLIPSOID[\"Clarke 1880 "
        "(IGN)\",6378249.2,293.4660213,LENGTHUNIT[\"metre\",1.0]]],\n"
        "    PRIMEM[\"Paris\",2.5969213,ANGLEUNIT[\"grad\",0.015707963268]]],\n"
        "  CONVERSION[\"Lambert Nord France\",\n"
        "    METHOD[\"Lambert Conic Conformal (1SP)\",ID[\"EPSG\",9801]],\n"
        "    PARAMETER[\"Latitude of natural "
        "origin\",55,ANGLEUNIT[\"grad\",0.015707963268]],\n"
        "    PARAMETER[\"Longitude of natural "
        "origin\",0,ANGLEUNIT[\"grad\",0.015707963268]],\n"
        "    PARAMETER[\"Scale factor at natural "
        "origin\",0.999877341,SCALEUNIT[\"unity\",1.0]],\n"
        "    PARAMETER[\"False easting\",600000,LENGTHUNIT[\"metre\",1.0]],\n"
        "    PARAMETER[\"False northing\",200000,LENGTHUNIT[\"metre\",1.0]]],\n"
        "  CS[cartesian,2],\n"
        "    AXIS[\"easting (X)\",east,ORDER[1]],\n"
        "    AXIS[\"northing (Y)\",north,ORDER[2]],\n"
        "    LENGTHUNIT[\"metre\",1.0],\n"
        "  ID[\"EPSG\",27561]]");
    auto crs = nn_dynamic_pointer_cast<ProjectedCRS>(obj);
    ASSERT_TRUE(crs != nullptr);
    EXPECT_EQ(crs->baseCRS()->coordinateSystem()->axisList()[0]->unit(),
              UnitOfMeasure::GRAD);
}

// ---------------------------------------------------------------------------

TEST(wkt_parse, cs_with_MERIDIAN) {
    auto wkt =
        "PROJCRS[\"dummy\",\n"
        "    BASEGEOGCRS[\"WGS 84\",\n"
        "        DATUM[\"World Geodetic System 1984\",\n"
        "            ELLIPSOID[\"WGS 84\",6378137,298.257223563]],\n"
        "        UNIT[\"degree\",0.0174532925199433]],\n"
        "    CONVERSION[\"dummy\",\n"
        "        METHOD[\"dummy\"]]\n"
        "    CS[Cartesian,2],\n"
        "        AXIS[\"easting "
        "(X)\",south,MERIDIAN[90,ANGLEUNIT[\"degree\",0.0174532925199433]]],\n"
        "        AXIS[\"northing (Y)\",north],\n"
        "        UNIT[\"metre\",1],\n"
        "    ID[\"EPSG\",32631]]";
    auto obj = WKTParser().createFromWKT(wkt);
    auto crs = nn_dynamic_pointer_cast<ProjectedCRS>(obj);
    ASSERT_TRUE(crs != nullptr);
    ASSERT_EQ(crs->coordinateSystem()->axisList().size(), 2);
    auto axis = crs->coordinateSystem()->axisList()[0];
    auto meridian = axis->meridian();
    ASSERT_TRUE(meridian != nullptr);
    EXPECT_EQ(meridian->longitude().value(), 90.0);
    EXPECT_EQ(meridian->longitude().unit(), UnitOfMeasure::DEGREE);
    ASSERT_TRUE(crs->coordinateSystem()->axisList()[1]->meridian() == nullptr);
}

// ---------------------------------------------------------------------------

TEST(wkt_parse, cs_with_multiple_ID) {
    auto wkt = "GEODCRS[\"WGS 84\",\n"
               "    DATUM[\"World Geodetic System 1984\",\n"
               "        ELLIPSOID[\"WGS 84\",6378137,298.257223563]],\n"
               "    CS[Cartesian,3],\n"
               "        AXIS[\"(X)\",geocentricX],\n"
               "        AXIS[\"(Y)\",geocentricY],\n"
               "        AXIS[\"(Z)\",geocentricZ],\n"
               "        UNIT[\"metre\",1],\n"
               "    ID[\"authorityA\",\"codeA\"],\n"
               "    ID[\"authorityB\",\"codeB\"]]";

    auto obj = WKTParser().createFromWKT(wkt);
    auto crs = nn_dynamic_pointer_cast<GeodeticCRS>(obj);
    EXPECT_EQ(*(crs->name()->description()), "WGS 84");
    ASSERT_EQ(crs->identifiers().size(), 2);
    EXPECT_EQ(crs->identifiers()[0]->code(), "codeA");
    EXPECT_EQ(*(crs->identifiers()[0]->codeSpace()), "authorityA");
    EXPECT_EQ(crs->identifiers()[1]->code(), "codeB");
    EXPECT_EQ(*(crs->identifiers()[1]->codeSpace()), "authorityB");
}

// ---------------------------------------------------------------------------

TEST(wkt_parse, vertcrs_WKT2) {
    auto wkt = "VERTCRS[\"ODN height\",\n"
               "    VDATUM[\"Ordnance Datum Newlyn\"],\n"
               "    CS[vertical,1],\n"
               "        AXIS[\"gravity-related height (H)\",up,\n"
               "            LENGTHUNIT[\"metre\",1]],\n"
               "    ID[\"EPSG\",5701]]";

    auto obj = WKTParser().createFromWKT(wkt);
    auto crs = nn_dynamic_pointer_cast<VerticalCRS>(obj);
    EXPECT_EQ(*(crs->name()->description()), "ODN height");
    ASSERT_EQ(crs->identifiers().size(), 1);
    EXPECT_EQ(crs->identifiers()[0]->code(), "5701");
    EXPECT_EQ(*(crs->identifiers()[0]->codeSpace()), "EPSG");

    auto datum = crs->datum();
    EXPECT_EQ(*(datum->name()->description()), "Ordnance Datum Newlyn");
    // ASSERT_EQ(datum->identifiers().size(), 1);
    // EXPECT_EQ(datum->identifiers()[0]->code(), "5101");
    // EXPECT_EQ(*(datum->identifiers()[0]->codeSpace()), "EPSG");

    auto cs = crs->coordinateSystem();
    ASSERT_EQ(cs->axisList().size(), 1);
    EXPECT_EQ(*(cs->axisList()[0]->name()->description()),
              "Gravity-related height");
    EXPECT_EQ(cs->axisList()[0]->abbreviation(), "H");
    EXPECT_EQ(cs->axisList()[0]->direction(), AxisDirection::UP);
}

// ---------------------------------------------------------------------------

TEST(wkt_parse, vertcrs_WKT1_GDAL) {
    auto wkt = "VERT_CS[\"ODN height\",\n"
               "    VERT_DATUM[\"Ordnance Datum Newlyn\",2005,\n"
               "        AUTHORITY[\"EPSG\",\"5101\"]],\n"
               "    UNIT[\"metre\",1,\n"
               "        AUTHORITY[\"EPSG\",9001]],\n"
               "    AXIS[\"gravity-related height\",UP],\n"
               "    AUTHORITY[\"EPSG\",\"5701\"]]";

    auto obj = WKTParser().createFromWKT(wkt);
    auto crs = nn_dynamic_pointer_cast<VerticalCRS>(obj);
    EXPECT_EQ(*(crs->name()->description()), "ODN height");
    ASSERT_EQ(crs->identifiers().size(), 1);
    EXPECT_EQ(crs->identifiers()[0]->code(), "5701");
    EXPECT_EQ(*(crs->identifiers()[0]->codeSpace()), "EPSG");

    auto datum = crs->datum();
    EXPECT_EQ(*(datum->name()->description()), "Ordnance Datum Newlyn");
    ASSERT_EQ(datum->identifiers().size(), 1);
    EXPECT_EQ(datum->identifiers()[0]->code(), "5101");
    EXPECT_EQ(*(datum->identifiers()[0]->codeSpace()), "EPSG");

    auto cs = crs->coordinateSystem();
    ASSERT_EQ(cs->axisList().size(), 1);
    EXPECT_EQ(*(cs->axisList()[0]->name()->description()),
              "Gravity-related height");
    EXPECT_EQ(cs->axisList()[0]->abbreviation(), ""); // "H" in WKT2
    EXPECT_EQ(cs->axisList()[0]->direction(), AxisDirection::UP);
}

// ---------------------------------------------------------------------------

TEST(wkt_parse, vertcrs_WKT1_GDAL_minimum) {
    auto wkt = "VERT_CS[\"ODN height\",\n"
               "    VERT_DATUM[\"Ordnance Datum Newlyn\",2005],\n"
               "    UNIT[\"metre\",1]]";

    auto obj = WKTParser().createFromWKT(wkt);
    auto crs = nn_dynamic_pointer_cast<VerticalCRS>(obj);
    EXPECT_EQ(*(crs->name()->description()), "ODN height");

    auto datum = crs->datum();
    EXPECT_EQ(*(datum->name()->description()), "Ordnance Datum Newlyn");

    auto cs = crs->coordinateSystem();
    ASSERT_EQ(cs->axisList().size(), 1);
    EXPECT_EQ(*(cs->axisList()[0]->name()->description()),
              "Gravity-related height");
    EXPECT_EQ(cs->axisList()[0]->direction(), AxisDirection::UP);
}

// ---------------------------------------------------------------------------

TEST(wkt_parse, vdatum_with_ANCHOR) {
    auto obj = WKTParser().createFromWKT("VDATUM[\"Ordnance Datum Newlyn\",\n"
                                         "    ANCHOR[\"my anchor\"],\n"
                                         "    ID[\"EPSG\",5101]]");
    auto datum = nn_dynamic_pointer_cast<VerticalReferenceFrame>(obj);
    ASSERT_TRUE(datum != nullptr);
    auto anchor = datum->anchorDefinition();
    EXPECT_TRUE(anchor.has_value());
    EXPECT_EQ(*anchor, "my anchor");
}

// ---------------------------------------------------------------------------

TEST(wkt_parse, COMPOUNDCRS) {
    auto obj = WKTParser().createFromWKT(
        "COMPOUNDCRS[\"horizontal + vertical\",\n"
        "    PROJCRS[\"WGS 84 / UTM zone 31N\",\n"
        "        BASEGEODCRS[\"WGS 84\",\n"
        "            DATUM[\"World Geodetic System 1984\",\n"
        "                ELLIPSOID[\"WGS 84\",6378137,298.257223563,\n"
        "                    LENGTHUNIT[\"metre\",1]]],\n"
        "            PRIMEM[\"Greenwich\",0,\n"
        "                ANGLEUNIT[\"degree\",0.0174532925199433]]],\n"
        "        CONVERSION[\"UTM zone 31N\",\n"
        "            METHOD[\"Transverse Mercator\",\n"
        "                ID[\"EPSG\",9807]],\n"
        "            PARAMETER[\"Latitude of natural origin\",0,\n"
        "                ANGLEUNIT[\"degree\",0.0174532925199433],\n"
        "                ID[\"EPSG\",8801]],\n"
        "            PARAMETER[\"Longitude of natural origin\",3,\n"
        "                ANGLEUNIT[\"degree\",0.0174532925199433],\n"
        "                ID[\"EPSG\",8802]],\n"
        "            PARAMETER[\"Scale factor at natural origin\",0.9996,\n"
        "                SCALEUNIT[\"unity\",1],\n"
        "                ID[\"EPSG\",8805]],\n"
        "            PARAMETER[\"False easting\",500000,\n"
        "                LENGTHUNIT[\"metre\",1],\n"
        "                ID[\"EPSG\",8806]],\n"
        "            PARAMETER[\"False northing\",0,\n"
        "                LENGTHUNIT[\"metre\",1],\n"
        "                ID[\"EPSG\",8807]]],\n"
        "        CS[Cartesian,2],\n"
        "            AXIS[\"(E)\",east,\n"
        "                ORDER[1],\n"
        "                LENGTHUNIT[\"metre\",1]],\n"
        "            AXIS[\"(N)\",north,\n"
        "                ORDER[2],\n"
        "                LENGTHUNIT[\"metre\",1]]],\n"
        "    VERTCRS[\"ODN height\",\n"
        "        VDATUM[\"Ordnance Datum Newlyn\"],\n"
        "        CS[vertical,1],\n"
        "            AXIS[\"gravity-related height (H)\",up,\n"
        "                LENGTHUNIT[\"metre\",1]]],\n"
        "    ID[\"codespace\",\"code\"]]");
    auto crs = nn_dynamic_pointer_cast<CompoundCRS>(obj);
    ASSERT_TRUE(crs != nullptr);
    EXPECT_EQ(*(crs->name()->description()), "horizontal + vertical");
    EXPECT_EQ(crs->componentReferenceSystems().size(), 2);
    ASSERT_EQ(crs->identifiers().size(), 1);
    EXPECT_EQ(crs->identifiers()[0]->code(), "code");
    EXPECT_EQ(*(crs->identifiers()[0]->codeSpace()), "codespace");
}

// ---------------------------------------------------------------------------

TEST(wkt_parse, COMPD_CS) {
    auto obj = WKTParser().createFromWKT(
        "COMPD_CS[\"horizontal + vertical\",\n"
        "    PROJCS[\"WGS 84 / UTM zone 31N\",\n"
        "        GEOGCS[\"WGS 84\",\n"
        "            DATUM[\"World Geodetic System 1984\",\n"
        "                SPHEROID[\"WGS 84\",6378137,298.257223563,\n"
        "                    AUTHORITY[\"EPSG\",\"7030\"]],\n"
        "                AUTHORITY[\"EPSG\",\"6326\"]],\n"
        "            PRIMEM[\"Greenwich\",0,\n"
        "                AUTHORITY[\"EPSG\",\"8901\"]],\n"
        "            UNIT[\"degree\",0.0174532925199433,\n"
        "                AUTHORITY[\"EPSG\",9122]],\n"
        "            AXIS[\"Latitude\",NORTH],\n"
        "            AXIS[\"Longitude\",EAST],\n"
        "            AUTHORITY[\"EPSG\",\"4326\"]],\n"
        "        PROJECTION[\"Transverse_Mercator\"],\n"
        "        PARAMETER[\"latitude_of_origin\",0],\n"
        "        PARAMETER[\"central_meridian\",3],\n"
        "        PARAMETER[\"scale_factor\",0.9996],\n"
        "        PARAMETER[\"false_easting\",500000],\n"
        "        PARAMETER[\"false_northing\",0],\n"
        "        UNIT[\"metre\",1,\n"
        "            AUTHORITY[\"EPSG\",9001]],\n"
        "        AXIS[\"Easting\",EAST],\n"
        "        AXIS[\"Northing\",NORTH],\n"
        "        AUTHORITY[\"EPSG\",\"32631\"]],\n"
        "    VERT_CS[\"ODN height\",\n"
        "        VERT_DATUM[\"Ordnance Datum Newlyn\",2005,\n"
        "            AUTHORITY[\"EPSG\",\"5101\"]],\n"
        "        UNIT[\"metre\",1,\n"
        "            AUTHORITY[\"EPSG\",9001]],\n"
        "        AXIS[\"Gravity-related height\",UP],\n"
        "        AUTHORITY[\"EPSG\",\"5701\"]],\n"
        "    AUTHORITY[\"codespace\",\"code\"]]");
    auto crs = nn_dynamic_pointer_cast<CompoundCRS>(obj);
    ASSERT_TRUE(crs != nullptr);
    EXPECT_EQ(*(crs->name()->description()), "horizontal + vertical");
    EXPECT_EQ(crs->componentReferenceSystems().size(), 2);
    ASSERT_EQ(crs->identifiers().size(), 1);
    EXPECT_EQ(crs->identifiers()[0]->code(), "code");
    EXPECT_EQ(*(crs->identifiers()[0]->codeSpace()), "codespace");
}

// ---------------------------------------------------------------------------

TEST(wkt_parse, COORDINATEOPERATION) {

    std::string src_wkt;
    {
        auto formatter = WKTFormatter::create();
        formatter->setOutputId(false);
        src_wkt = GeographicCRS::EPSG_4326->exportToWKT(formatter);
    }

    std::string dst_wkt;
    {
        auto formatter = WKTFormatter::create();
        formatter->setOutputId(false);
        dst_wkt = GeographicCRS::EPSG_4807->exportToWKT(formatter);
    }

    std::string interpolation_wkt;
    {
        auto formatter = WKTFormatter::create();
        formatter->setOutputId(false);
        interpolation_wkt = GeographicCRS::EPSG_4979->exportToWKT(formatter);
    }

    auto wkt =
        "COORDINATEOPERATION[\"transformationName\",\n"
        "    SOURCECRS[" +
        src_wkt + "],\n"
                  "    TARGETCRS[" +
        dst_wkt +
        "],\n"
        "    METHOD[\"operationMethodName\",\n"
        "        ID[\"codeSpaceOperationMethod\",\"codeOperationMethod\"]],\n"
        "    PARAMETERFILE[\"paramName\",\"foo.bin\"],\n"
        "    INTERPOLATIONCRS[" +
        interpolation_wkt +
        "],\n"
        "    OPERATIONACCURACY[0.1],\n"
        "    ID[\"codeSpaceTransformation\",\"codeTransformation\"],\n"
        "    REMARK[\"my remarks\"]]";

    auto obj = WKTParser().createFromWKT(wkt);
    auto transf = nn_dynamic_pointer_cast<Transformation>(obj);
    ASSERT_TRUE(transf != nullptr);
    EXPECT_EQ(*(transf->name()->description()), "transformationName");
    ASSERT_EQ(transf->identifiers().size(), 1);
    EXPECT_EQ(transf->identifiers()[0]->code(), "codeTransformation");
    EXPECT_EQ(*(transf->identifiers()[0]->codeSpace()),
              "codeSpaceTransformation");
    ASSERT_EQ(transf->coordinateOperationAccuracies().size(), 1);
    EXPECT_EQ(transf->coordinateOperationAccuracies()[0]->value(), "0.1");
    EXPECT_EQ(*transf->sourceCRS()->name()->description(),
              *(GeographicCRS::EPSG_4326->name()->description()));
    EXPECT_EQ(*transf->targetCRS()->name()->description(),
              *(GeographicCRS::EPSG_4807->name()->description()));
    ASSERT_TRUE(transf->interpolationCRS() != nullptr);
    EXPECT_EQ(*transf->interpolationCRS()->name()->description(),
              *(GeographicCRS::EPSG_4979->name()->description()));
    EXPECT_EQ(*transf->method()->name()->description(), "operationMethodName");
    EXPECT_EQ(transf->parameterValues().size(), 1);
}

// ---------------------------------------------------------------------------

TEST(wkt_parse, CONCATENATEDOPERATION) {

    auto transf_1 = Transformation::create(
        PropertyMap().set(IdentifiedObject::NAME_KEY, "transf_1"),
        nn_static_pointer_cast<CRS>(GeographicCRS::EPSG_4326),
        nn_static_pointer_cast<CRS>(GeographicCRS::EPSG_4807), nullptr,
        PropertyMap().set(IdentifiedObject::NAME_KEY, "operationMethodName"),
        std::vector<OperationParameterNNPtr>{OperationParameter::create(
            PropertyMap().set(IdentifiedObject::NAME_KEY, "paramName"))},
        std::vector<ParameterValueNNPtr>{
            ParameterValue::createFilename("foo.bin")},
        std::vector<PositionalAccuracyNNPtr>());

    auto transf_2 = Transformation::create(
        PropertyMap().set(IdentifiedObject::NAME_KEY, "transf_2"),
        nn_static_pointer_cast<CRS>(GeographicCRS::EPSG_4807),
        nn_static_pointer_cast<CRS>(GeographicCRS::EPSG_4979), nullptr,
        PropertyMap().set(IdentifiedObject::NAME_KEY, "operationMethodName"),
        std::vector<OperationParameterNNPtr>{OperationParameter::create(
            PropertyMap().set(IdentifiedObject::NAME_KEY, "paramName"))},
        std::vector<ParameterValueNNPtr>{
            ParameterValue::createFilename("foo.bin")},
        std::vector<PositionalAccuracyNNPtr>());

    auto concat_in = ConcatenatedOperation::create(
        PropertyMap()
            .set(Identifier::CODESPACE_KEY, "codeSpace")
            .set(Identifier::CODE_KEY, "code")
            .set(IdentifiedObject::NAME_KEY, "name")
            .set(IdentifiedObject::REMARKS_KEY, "my remarks"),
        std::vector<CoordinateOperationNNPtr>{transf_1, transf_2},
        std::vector<PositionalAccuracyNNPtr>{
            PositionalAccuracy::create("0.1")});

    auto wkt = concat_in->exportToWKT(
        WKTFormatter::create(WKTFormatter::Convention::WKT2_2018));

    auto obj = WKTParser().createFromWKT(wkt);
    auto concat = nn_dynamic_pointer_cast<ConcatenatedOperation>(obj);
    ASSERT_TRUE(concat != nullptr);
    EXPECT_EQ(*(concat->name()->description()), "name");
    ASSERT_EQ(concat->identifiers().size(), 1);
    EXPECT_EQ(concat->identifiers()[0]->code(), "code");
    EXPECT_EQ(*(concat->identifiers()[0]->codeSpace()), "codeSpace");
    ASSERT_EQ(concat->operations().size(), 2);
    ASSERT_EQ(*(concat->operations()[0]->name()->description()),
              *(transf_1->name()->description()));
    ASSERT_EQ(*(concat->operations()[1]->name()->description()),
              *(transf_2->name()->description()));
    ASSERT_TRUE(concat->sourceCRS() != nullptr);
    ASSERT_TRUE(concat->targetCRS() != nullptr);
    ASSERT_EQ(*(concat->sourceCRS()->name()->description()),
              *(transf_1->sourceCRS()->name()->description()));
    ASSERT_EQ(*(concat->targetCRS()->name()->description()),
              *(transf_2->targetCRS()->name()->description()));
}

// ---------------------------------------------------------------------------

TEST(wkt_parse, BOUNDCRS_transformation_from_names) {

    auto projcrs = ProjectedCRS::create(
        PropertyMap().set(IdentifiedObject::NAME_KEY, "my PROJCRS"),
        GeographicCRS::create(
            PropertyMap().set(IdentifiedObject::NAME_KEY, "my GEOGCRS"),
            GeodeticReferenceFrame::EPSG_6326,
            EllipsoidalCS::createLatitudeLongitude(UnitOfMeasure::DEGREE)),
        Conversion::createUTM(PropertyMap(), 31, true),
        CartesianCS::createEastingNorthing(UnitOfMeasure::METRE));

    auto wkt = "BOUNDCRS[SOURCECRS[" +
               projcrs->exportToWKT(WKTFormatter::create()) + "],\n" +
               "TARGETCRS[" +
               GeographicCRS::EPSG_4326->exportToWKT(WKTFormatter::create()) +
               "],\n"
               "    ABRIDGEDTRANSFORMATION[\"Transformation to WGS84\",\n"
               "        METHOD[\"Coordinate Frame\"],\n"
               "        PARAMETER[\"X-axis translation\",1],\n"
               "        PARAMETER[\"Y-axis translation\",2],\n"
               "        PARAMETER[\"Z-axis translation\",3],\n"
               "        PARAMETER[\"X-axis rotation\",-0.825059224988385],\n"
               "        PARAMETER[\"Y-axis rotation\",-1.03132403123548],\n"
               "        PARAMETER[\"Z-axis rotation\",-1.23758883748258],\n"
               "        PARAMETER[\"Scale difference\",1.000007]]]";

    auto obj = WKTParser().createFromWKT(wkt);
    auto crs = nn_dynamic_pointer_cast<BoundCRS>(obj);
    ASSERT_TRUE(crs != nullptr);

    EXPECT_EQ(*(crs->baseCRS()->name()->description()),
              *(projcrs->name()->description()));

    EXPECT_EQ(*(crs->hubCRS()->name()->description()),
              *(GeographicCRS::EPSG_4326->name()->description()));

    ASSERT_TRUE(crs->transformation()->sourceCRS() != nullptr);
    EXPECT_EQ(*(crs->transformation()->sourceCRS()->name()->description()),
              *(projcrs->baseCRS()->name()->description()));

    auto params = crs->transformation()->getTOWGS84Parameters();
    auto expected = std::vector<double>{1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0};
    ASSERT_EQ(params.size(), expected.size());
    for (int i = 0; i < 7; i++) {
        EXPECT_NEAR(params[i], expected[i], 1e-10);
    }
}

// ---------------------------------------------------------------------------

TEST(wkt_parse, BOUNDCRS_transformation_from_codes) {

    auto projcrs = ProjectedCRS::create(
        PropertyMap().set(IdentifiedObject::NAME_KEY, "my PROJCRS"),
        GeographicCRS::create(
            PropertyMap().set(IdentifiedObject::NAME_KEY, "my GEOGCRS"),
            GeodeticReferenceFrame::EPSG_6326,
            EllipsoidalCS::createLatitudeLongitude(UnitOfMeasure::DEGREE)),
        Conversion::createUTM(PropertyMap(), 31, true),
        CartesianCS::createEastingNorthing(UnitOfMeasure::METRE));

    auto wkt =
        "BOUNDCRS[SOURCECRS[" + projcrs->exportToWKT(WKTFormatter::create()) +
        "],\n" + "TARGETCRS[" +
        GeographicCRS::EPSG_4326->exportToWKT(WKTFormatter::create()) +
        "],\n"
        "    ABRIDGEDTRANSFORMATION[\"Transformation to WGS84\",\n"
        "        METHOD[\"bla\",ID[\"EPSG\",1032]],\n"
        "        PARAMETER[\"tx\",1,ID[\"EPSG\",8605]],\n"
        "        PARAMETER[\"ty\",2,ID[\"EPSG\",8606]],\n"
        "        PARAMETER[\"tz\",3,ID[\"EPSG\",8607]],\n"
        "        PARAMETER[\"rotx\",-0.825059224988385,ID[\"EPSG\",8608]],\n"
        "        PARAMETER[\"roty\",-1.03132403123548,ID[\"EPSG\",8609]],\n"
        "        PARAMETER[\"rotz\",-1.23758883748258,ID[\"EPSG\",8610]],\n"
        "        PARAMETER[\"scale\",1.000007,ID[\"EPSG\",8611]]]]";

    auto obj = WKTParser().createFromWKT(wkt);
    auto crs = nn_dynamic_pointer_cast<BoundCRS>(obj);
    ASSERT_TRUE(crs != nullptr);

    EXPECT_EQ(*(crs->baseCRS()->name()->description()),
              *(projcrs->name()->description()));

    EXPECT_EQ(*(crs->hubCRS()->name()->description()),
              *(GeographicCRS::EPSG_4326->name()->description()));

    ASSERT_TRUE(crs->transformation()->sourceCRS() != nullptr);
    EXPECT_EQ(*(crs->transformation()->sourceCRS()->name()->description()),
              *(projcrs->baseCRS()->name()->description()));

    auto params = crs->transformation()->getTOWGS84Parameters();
    auto expected = std::vector<double>{1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0};
    ASSERT_EQ(params.size(), expected.size());
    for (int i = 0; i < 7; i++) {
        EXPECT_NEAR(params[i], expected[i], 1e-10);
    }
}

// ---------------------------------------------------------------------------

TEST(wkt_parse, TOWGS84) {
    auto wkt = "PROJCS[\"my PROJCRS\",\n"
               "    GEOGCS[\"my GEOGCRS\",\n"
               "        DATUM[\"WGS_1984\",\n"
               "            SPHEROID[\"WGS 84\",6378137,298.257223563,\n"
               "                AUTHORITY[\"EPSG\",\"7030\"]],\n"
               "            TOWGS84[1,2,3,4,5,6,7],\n"
               "            AUTHORITY[\"EPSG\",\"6326\"]],\n"
               "        PRIMEM[\"Greenwich\",0,\n"
               "            AUTHORITY[\"EPSG\",\"8901\"]],\n"
               "        UNIT[\"degree\",0.0174532925199433,\n"
               "            AUTHORITY[\"EPSG\",9122]],\n"
               "        AXIS[\"Latitude\",NORTH],\n"
               "        AXIS[\"Longitude\",EAST]],\n"
               "    PROJECTION[\"Transverse_Mercator\"],\n"
               "    PARAMETER[\"latitude_of_origin\",0],\n"
               "    PARAMETER[\"central_meridian\",3],\n"
               "    PARAMETER[\"scale_factor\",0.9996],\n"
               "    PARAMETER[\"false_easting\",500000],\n"
               "    PARAMETER[\"false_northing\",0],\n"
               "    UNIT[\"metre\",1,\n"
               "        AUTHORITY[\"EPSG\",9001]],\n"
               "    AXIS[\"Easting\",EAST],\n"
               "    AXIS[\"Northing\",NORTH]]";

    auto obj = WKTParser().createFromWKT(wkt);
    auto crs = nn_dynamic_pointer_cast<BoundCRS>(obj);
    ASSERT_TRUE(crs != nullptr);

    EXPECT_EQ(*(crs->baseCRS()->name()->description()), "my PROJCRS");

    EXPECT_EQ(*(crs->hubCRS()->name()->description()),
              *(GeographicCRS::EPSG_4326->name()->description()));

    ASSERT_TRUE(crs->transformation()->sourceCRS() != nullptr);
    EXPECT_EQ(*(crs->transformation()->sourceCRS()->name()->description()),
              "my GEOGCRS");

    auto params = crs->transformation()->getTOWGS84Parameters();
    auto expected = std::vector<double>{1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0};
    ASSERT_EQ(params.size(), expected.size());
    for (int i = 0; i < 7; i++) {
        EXPECT_NEAR(params[i], expected[i], 1e-10);
    }
}

// ---------------------------------------------------------------------------

TEST(wkt_parse, WKT1_VERT_DATUM_EXTENSION) {
    auto wkt = "VERT_CS[\"EGM2008 geoid height\",\n"
               "    VERT_DATUM[\"EGM2008 geoid\",2005,\n"
               "        EXTENSION[\"PROJ4_GRIDS\",\"egm08_25.gtx\"],\n"
               "        AUTHORITY[\"EPSG\",\"1027\"]],\n"
               "    UNIT[\"metre\",1,\n"
               "        AUTHORITY[\"EPSG\",\"9001\"]],\n"
               "    AXIS[\"Up\",UP],\n"
               "    AUTHORITY[\"EPSG\",\"3855\"]]";

    auto obj = WKTParser().createFromWKT(wkt);
    auto crs = nn_dynamic_pointer_cast<BoundCRS>(obj);
    ASSERT_TRUE(crs != nullptr);

    EXPECT_EQ(*(crs->baseCRS()->name()->description()), "EGM2008 geoid height");

    EXPECT_EQ(*(crs->hubCRS()->name()->description()),
              *(GeographicCRS::EPSG_4979->name()->description()));

    ASSERT_TRUE(crs->transformation()->sourceCRS() != nullptr);
    EXPECT_EQ(*(crs->transformation()->sourceCRS()->name()->description()),
              *(crs->baseCRS()->name()->description()));

    ASSERT_TRUE(crs->transformation()->targetCRS() != nullptr);
    EXPECT_EQ(*(crs->transformation()->targetCRS()->name()->description()),
              *(crs->hubCRS()->name()->description()));

    EXPECT_EQ(*crs->transformation()->name()->description(),
              "EGM2008 geoid height to WGS84 ellipsoidal height");
    EXPECT_EQ(*crs->transformation()->method()->name()->description(),
              "GravityRelatedHeight to Geographic3D");
    ASSERT_EQ(crs->transformation()->parameterValues().size(), 1);
    {
        const auto &opParamvalue =
            nn_dynamic_pointer_cast<OperationParameterValue>(
                crs->transformation()->parameterValues()[0]);
        ASSERT_TRUE(opParamvalue);
        const auto &paramName =
            *(opParamvalue->parameter()->name()->description());
        const auto &parameterValue = opParamvalue->parameterValue();
        EXPECT_TRUE(opParamvalue->parameter()->isEPSG(8666));
        EXPECT_EQ(paramName, "Geoid (height correction) model file");
        EXPECT_EQ(parameterValue->type(), ParameterValue::Type::FILENAME);
        EXPECT_EQ(parameterValue->valueFile(), "egm08_25.gtx");
    }
}

// ---------------------------------------------------------------------------

TEST(wkt_parse, WKT1_DATUM_EXTENSION) {
    auto wkt =
        "PROJCS[\"unnamed\",\n"
        "    GEOGCS[\"International 1909 (Hayford)\",\n"
        "        DATUM[\"unknown\",\n"
        "            SPHEROID[\"intl\",6378388,297],\n"
        "            EXTENSION[\"PROJ4_GRIDS\",\"nzgd2kgrid0005.gsb\"]],\n"
        "        PRIMEM[\"Greenwich\",0],\n"
        "        UNIT[\"degree\",0.0174532925199433]],\n"
        "    PROJECTION[\"New_Zealand_Map_Grid\"],\n"
        "    PARAMETER[\"latitude_of_origin\",-41],\n"
        "    PARAMETER[\"central_meridian\",173],\n"
        "    PARAMETER[\"false_easting\",2510000],\n"
        "    PARAMETER[\"false_northing\",6023150],\n"
        "    UNIT[\"Meter\",1]]";

    auto obj = WKTParser().createFromWKT(wkt);
    auto crs = nn_dynamic_pointer_cast<BoundCRS>(obj);
    ASSERT_TRUE(crs != nullptr);

    EXPECT_EQ(*(crs->baseCRS()->name()->description()), "unnamed");

    EXPECT_EQ(*(crs->hubCRS()->name()->description()),
              *(GeographicCRS::EPSG_4326->name()->description()));

    ASSERT_TRUE(crs->transformation()->sourceCRS() != nullptr);
    EXPECT_EQ(*(crs->transformation()->sourceCRS()->name()->description()),
              "International 1909 (Hayford)");

    ASSERT_TRUE(crs->transformation()->targetCRS() != nullptr);
    EXPECT_EQ(*(crs->transformation()->targetCRS()->name()->description()),
              *(crs->hubCRS()->name()->description()));

    EXPECT_EQ(*crs->transformation()->name()->description(),
              "International 1909 (Hayford) to WGS84");
    EXPECT_EQ(*crs->transformation()->method()->name()->description(), "NTv2");
    ASSERT_EQ(crs->transformation()->parameterValues().size(), 1);
    {
        const auto &opParamvalue =
            nn_dynamic_pointer_cast<OperationParameterValue>(
                crs->transformation()->parameterValues()[0]);
        ASSERT_TRUE(opParamvalue);
        const auto &paramName =
            *(opParamvalue->parameter()->name()->description());
        const auto &parameterValue = opParamvalue->parameterValue();
        EXPECT_TRUE(opParamvalue->parameter()->isEPSG(8656));
        EXPECT_EQ(paramName, "Latitude and longitude difference file");
        EXPECT_EQ(parameterValue->type(), ParameterValue::Type::FILENAME);
        EXPECT_EQ(parameterValue->valueFile(), "nzgd2kgrid0005.gsb");
    }
}

// ---------------------------------------------------------------------------

TEST(wkt_parse, DerivedGeographicCRS_WKT2) {
    auto wkt = "GEODCRS[\"WMO Atlantic Pole\",\n"
               "    BASEGEODCRS[\"WGS 84\",\n"
               "        DATUM[\"World Geodetic System 1984\",\n"
               "            ELLIPSOID[\"WGS 84\",6378137,298.257223563,\n"
               "                LENGTHUNIT[\"metre\",1]]],\n"
               "        PRIMEM[\"Greenwich\",0,\n"
               "            ANGLEUNIT[\"degree\",0.0174532925199433]]],\n"
               "    DERIVINGCONVERSION[\"Atlantic pole\",\n"
               "        METHOD[\"Pole rotation\"],\n"
               "        PARAMETER[\"Latitude of rotated pole\",52,\n"
               "            ANGLEUNIT[\"degree\",0.0174532925199433,\n"
               "                ID[\"EPSG\",9122]]],\n"
               "        PARAMETER[\"Longitude of rotated pole\",-30,\n"
               "            ANGLEUNIT[\"degree\",0.0174532925199433,\n"
               "                ID[\"EPSG\",9122]]],\n"
               "        PARAMETER[\"Axis rotation\",-25,\n"
               "            ANGLEUNIT[\"degree\",0.0174532925199433,\n"
               "                ID[\"EPSG\",9122]]]],\n"
               "    CS[ellipsoidal,2],\n"
               "        AXIS[\"latitude\",north,\n"
               "            ORDER[1],\n"
               "            ANGLEUNIT[\"degree\",0.0174532925199433,\n"
               "                ID[\"EPSG\",9122]]],\n"
               "        AXIS[\"longitude\",east,\n"
               "            ORDER[2],\n"
               "            ANGLEUNIT[\"degree\",0.0174532925199433,\n"
               "                ID[\"EPSG\",9122]]]]";

    auto obj = WKTParser().createFromWKT(wkt);
    auto crs = nn_dynamic_pointer_cast<DerivedGeographicCRS>(obj);
    ASSERT_TRUE(crs != nullptr);

    EXPECT_EQ(*(crs->name()->description()), "WMO Atlantic Pole");

    EXPECT_EQ(*(crs->baseCRS()->name()->description()), "WGS 84");
    EXPECT_TRUE(nn_dynamic_pointer_cast<GeographicCRS>(crs->baseCRS()) !=
                nullptr);

    EXPECT_EQ(*(crs->derivingConversion()->name()->description()),
              "Atlantic pole");

    EXPECT_TRUE(nn_dynamic_pointer_cast<EllipsoidalCS>(
                    crs->coordinateSystem()) != nullptr);
}

// ---------------------------------------------------------------------------

TEST(wkt_parse, DerivedGeographicCRS_WKT2_2018) {
    auto wkt = "GEOGCRS[\"WMO Atlantic Pole\",\n"
               "    BASEGEOGCRS[\"WGS 84\",\n"
               "        DATUM[\"World Geodetic System 1984\",\n"
               "            ELLIPSOID[\"WGS 84\",6378137,298.257223563,\n"
               "                LENGTHUNIT[\"metre\",1]]],\n"
               "        PRIMEM[\"Greenwich\",0,\n"
               "            ANGLEUNIT[\"degree\",0.0174532925199433]]],\n"
               "    DERIVINGCONVERSION[\"Atlantic pole\",\n"
               "        METHOD[\"Pole rotation\"],\n"
               "        PARAMETER[\"Latitude of rotated pole\",52,\n"
               "            ANGLEUNIT[\"degree\",0.0174532925199433,\n"
               "                ID[\"EPSG\",9122]]],\n"
               "        PARAMETER[\"Longitude of rotated pole\",-30,\n"
               "            ANGLEUNIT[\"degree\",0.0174532925199433,\n"
               "                ID[\"EPSG\",9122]]],\n"
               "        PARAMETER[\"Axis rotation\",-25,\n"
               "            ANGLEUNIT[\"degree\",0.0174532925199433,\n"
               "                ID[\"EPSG\",9122]]]],\n"
               "    CS[ellipsoidal,2],\n"
               "        AXIS[\"latitude\",north,\n"
               "            ORDER[1],\n"
               "            ANGLEUNIT[\"degree\",0.0174532925199433,\n"
               "                ID[\"EPSG\",9122]]],\n"
               "        AXIS[\"longitude\",east,\n"
               "            ORDER[2],\n"
               "            ANGLEUNIT[\"degree\",0.0174532925199433,\n"
               "                ID[\"EPSG\",9122]]]]";

    auto obj = WKTParser().createFromWKT(wkt);
    auto crs = nn_dynamic_pointer_cast<DerivedGeographicCRS>(obj);
    ASSERT_TRUE(crs != nullptr);

    EXPECT_EQ(*(crs->name()->description()), "WMO Atlantic Pole");

    EXPECT_EQ(*(crs->baseCRS()->name()->description()), "WGS 84");
    EXPECT_TRUE(nn_dynamic_pointer_cast<GeographicCRS>(crs->baseCRS()) !=
                nullptr);

    EXPECT_EQ(*(crs->derivingConversion()->name()->description()),
              "Atlantic pole");

    EXPECT_TRUE(nn_dynamic_pointer_cast<EllipsoidalCS>(
                    crs->coordinateSystem()) != nullptr);
}

// ---------------------------------------------------------------------------

TEST(wkt_parse, DerivedGeodeticCRS) {
    auto wkt = "GEODCRS[\"Derived geodetic CRS\",\n"
               "    BASEGEODCRS[\"WGS 84\",\n"
               "        DATUM[\"World Geodetic System 1984\",\n"
               "            ELLIPSOID[\"WGS 84\",6378137,298.257223563,\n"
               "                LENGTHUNIT[\"metre\",1]]],\n"
               "        PRIMEM[\"Greenwich\",0,\n"
               "            ANGLEUNIT[\"degree\",0.0174532925199433]]],\n"
               "    DERIVINGCONVERSION[\"Some conversion\",\n"
               "        METHOD[\"Some method\"]],\n"
               "    CS[Cartesian,3],\n"
               "        AXIS[\"(X)\",geocentricX,\n"
               "            ORDER[1],\n"
               "            LENGTHUNIT[\"metre\",1,\n"
               "                ID[\"EPSG\",9001]]],\n"
               "        AXIS[\"(Y)\",geocentricY,\n"
               "            ORDER[2],\n"
               "            LENGTHUNIT[\"metre\",1,\n"
               "                ID[\"EPSG\",9001]]],\n"
               "        AXIS[\"(Z)\",geocentricZ,\n"
               "            ORDER[3],\n"
               "            LENGTHUNIT[\"metre\",1,\n"
               "                ID[\"EPSG\",9001]]]]";
    ;

    auto obj = WKTParser().createFromWKT(wkt);
    auto crs = nn_dynamic_pointer_cast<DerivedGeodeticCRS>(obj);
    ASSERT_TRUE(crs != nullptr);

    EXPECT_EQ(*(crs->name()->description()), "Derived geodetic CRS");

    EXPECT_EQ(*(crs->baseCRS()->name()->description()), "WGS 84");
    EXPECT_TRUE(nn_dynamic_pointer_cast<GeographicCRS>(crs->baseCRS()) !=
                nullptr);

    EXPECT_EQ(*(crs->derivingConversion()->name()->description()),
              "Some conversion");

    EXPECT_TRUE(nn_dynamic_pointer_cast<CartesianCS>(crs->coordinateSystem()) !=
                nullptr);
}

// ---------------------------------------------------------------------------

TEST(wkt_parse, TemporalDatum) {
    auto wkt = "TDATUM[\"Gregorian calendar\",\n"
               "    CALENDAR[\"my calendar\"],\n"
               "    TIMEORIGIN[0000-01-01]]";

    auto obj = WKTParser().createFromWKT(wkt);
    auto tdatum = nn_dynamic_pointer_cast<TemporalDatum>(obj);
    ASSERT_TRUE(tdatum != nullptr);

    EXPECT_EQ(*(tdatum->name()->description()), "Gregorian calendar");
    EXPECT_EQ(tdatum->temporalOrigin().toString(), "0000-01-01");
    EXPECT_EQ(tdatum->calendar(), "my calendar");
}

// ---------------------------------------------------------------------------

TEST(wkt_parse, TemporalDatum_no_calendar) {
    auto wkt = "TDATUM[\"Gregorian calendar\",\n"
               "    TIMEORIGIN[0000-01-01]]";

    auto obj = WKTParser().createFromWKT(wkt);
    auto tdatum = nn_dynamic_pointer_cast<TemporalDatum>(obj);
    ASSERT_TRUE(tdatum != nullptr);

    EXPECT_EQ(*(tdatum->name()->description()), "Gregorian calendar");
    EXPECT_EQ(tdatum->temporalOrigin().toString(), "0000-01-01");
    EXPECT_EQ(tdatum->calendar(), "proleptic Gregorian");
}

// ---------------------------------------------------------------------------

TEST(wkt_parse, dateTimeTemporalCRS_WKT2) {
    auto wkt = "TIMECRS[\"Temporal CRS\",\n"
               "    TDATUM[\"Gregorian calendar\",\n"
               "        TIMEORIGIN[0000-01-01]],\n"
               "    CS[temporal,1],\n"
               "        AXIS[\"time (T)\",future]]";

    auto obj = WKTParser().createFromWKT(wkt);
    auto crs = nn_dynamic_pointer_cast<TemporalCRS>(obj);
    ASSERT_TRUE(crs != nullptr);

    EXPECT_EQ(*(crs->name()->description()), "Temporal CRS");
    auto tdatum = crs->datum();
    EXPECT_EQ(*(tdatum->name()->description()), "Gregorian calendar");
    EXPECT_EQ(tdatum->temporalOrigin().toString(), "0000-01-01");
    EXPECT_EQ(tdatum->calendar(), "proleptic Gregorian");
    EXPECT_TRUE(nn_dynamic_pointer_cast<DateTimeTemporalCS>(
                    crs->coordinateSystem()) != nullptr);
    ASSERT_EQ(crs->coordinateSystem()->axisList().size(), 1);
    EXPECT_EQ(crs->coordinateSystem()->axisList()[0]->unit().type(),
              UnitOfMeasure::Type::NONE);
    EXPECT_EQ(crs->coordinateSystem()->axisList()[0]->unit().name(), "");
}

// ---------------------------------------------------------------------------

TEST(wkt_parse, dateTimeTemporalCRS_WKT2_2018) {
    auto wkt = "TIMECRS[\"Temporal CRS\",\n"
               "    TDATUM[\"Gregorian calendar\",\n"
               "        CALENDAR[\"proleptic Gregorian\"],\n"
               "        TIMEORIGIN[0000-01-01]],\n"
               "    CS[TemporalDateTime,1],\n"
               "        AXIS[\"time (T)\",future]]";

    auto obj = WKTParser().createFromWKT(wkt);
    auto crs = nn_dynamic_pointer_cast<TemporalCRS>(obj);
    ASSERT_TRUE(crs != nullptr);

    EXPECT_EQ(*(crs->name()->description()), "Temporal CRS");
    auto tdatum = crs->datum();
    EXPECT_EQ(*(tdatum->name()->description()), "Gregorian calendar");
    EXPECT_EQ(tdatum->temporalOrigin().toString(), "0000-01-01");
    EXPECT_EQ(tdatum->calendar(), "proleptic Gregorian");
    EXPECT_TRUE(nn_dynamic_pointer_cast<DateTimeTemporalCS>(
                    crs->coordinateSystem()) != nullptr);
    ASSERT_EQ(crs->coordinateSystem()->axisList().size(), 1);
    EXPECT_EQ(crs->coordinateSystem()->axisList()[0]->unit().type(),
              UnitOfMeasure::Type::NONE);
    EXPECT_EQ(crs->coordinateSystem()->axisList()[0]->unit().name(), "");
}

// ---------------------------------------------------------------------------

TEST(wkt_parse, temporalCountCRSWithConvFactor_WKT2_2018) {
    auto wkt = "TIMECRS[\"GPS milliseconds\",\n"
               "    TDATUM[\"GPS time origin\",\n"
               "        TIMEORIGIN[1980-01-01T00:00:00.0Z]],\n"
               "    CS[TemporalCount,1],\n"
               "        AXIS[\"(T)\",future,\n"
               "            TIMEUNIT[\"milliseconds (ms)\",0.001]]]";

    auto obj = WKTParser().createFromWKT(wkt);
    auto crs = nn_dynamic_pointer_cast<TemporalCRS>(obj);
    ASSERT_TRUE(crs != nullptr);

    EXPECT_EQ(*(crs->name()->description()), "GPS milliseconds");
    auto tdatum = crs->datum();
    EXPECT_EQ(*(tdatum->name()->description()), "GPS time origin");
    EXPECT_EQ(tdatum->temporalOrigin().toString(), "1980-01-01T00:00:00.0Z");
    EXPECT_EQ(tdatum->calendar(), "proleptic Gregorian");
    EXPECT_TRUE(nn_dynamic_pointer_cast<TemporalCountCS>(
                    crs->coordinateSystem()) != nullptr);
    ASSERT_EQ(crs->coordinateSystem()->axisList().size(), 1);
    EXPECT_EQ(crs->coordinateSystem()->axisList()[0]->unit().name(),
              "milliseconds (ms)");
    EXPECT_EQ(crs->coordinateSystem()->axisList()[0]->unit().conversionToSI(),
              0.001);
}

// ---------------------------------------------------------------------------

TEST(wkt_parse, temporalCountCRSWithoutConvFactor_WKT2_2018) {
    auto wkt = "TIMECRS[\"Calendar hours from 1979-12-29\",\n"
               "    TDATUM[\"29 December 1979\",\n"
               "        CALENDAR[\"proleptic Gregorian\"],\n"
               "        TIMEORIGIN[1979-12-29T00]],\n"
               "    CS[TemporalCount,1],\n"
               "        AXIS[\"time\",future,\n"
               "            TIMEUNIT[\"hour\"]]]";

    auto obj = WKTParser().createFromWKT(wkt);
    auto crs = nn_dynamic_pointer_cast<TemporalCRS>(obj);
    ASSERT_TRUE(crs != nullptr);

    EXPECT_EQ(*(crs->name()->description()), "Calendar hours from 1979-12-29");
    auto tdatum = crs->datum();
    EXPECT_EQ(*(tdatum->name()->description()), "29 December 1979");
    EXPECT_EQ(tdatum->temporalOrigin().toString(), "1979-12-29T00");
    EXPECT_TRUE(nn_dynamic_pointer_cast<TemporalCountCS>(
                    crs->coordinateSystem()) != nullptr);
    ASSERT_EQ(crs->coordinateSystem()->axisList().size(), 1);
    EXPECT_EQ(crs->coordinateSystem()->axisList()[0]->unit().name(), "hour");
    EXPECT_EQ(crs->coordinateSystem()->axisList()[0]->unit().conversionToSI(),
              0.0);
}

// ---------------------------------------------------------------------------

TEST(wkt_parse, temporalMeasureCRSWithoutConvFactor_WKT2_2018) {
    auto wkt = "TIMECRS[\"Decimal Years CE\",\n"
               "    TIMEDATUM[\"Common Era\",\n"
               "        TIMEORIGIN[0000]],\n"
               "    CS[TemporalMeasure,1],\n"
               "        AXIS[\"decimal years (a)\",future,\n"
               "            TIMEUNIT[\"year\"]]]";

    auto obj = WKTParser().createFromWKT(wkt);
    auto crs = nn_dynamic_pointer_cast<TemporalCRS>(obj);
    ASSERT_TRUE(crs != nullptr);

    EXPECT_EQ(*(crs->name()->description()), "Decimal Years CE");
    auto tdatum = crs->datum();
    EXPECT_EQ(*(tdatum->name()->description()), "Common Era");
    EXPECT_EQ(tdatum->temporalOrigin().toString(), "0000");
    EXPECT_TRUE(nn_dynamic_pointer_cast<TemporalMeasureCS>(
                    crs->coordinateSystem()) != nullptr);
    auto cs = crs->coordinateSystem();
    ASSERT_EQ(cs->axisList().size(), 1);
    auto axis = cs->axisList()[0];
    EXPECT_EQ(*(axis->name()->description()), "Decimal years");
    EXPECT_EQ(axis->unit().name(), "year");
    EXPECT_EQ(axis->unit().conversionToSI(), 0.0);
}

// ---------------------------------------------------------------------------

TEST(wkt_parse, invalid) {
    EXPECT_THROW(WKTParser().createFromWKT(""), ParsingException);
    EXPECT_THROW(WKTParser().createFromWKT("A"), ParsingException);
    EXPECT_THROW(WKTParser().createFromWKT("UNKNOWN[\"foo\"]"),
                 ParsingException);
    EXPECT_THROW(WKTParser().createFromWKT("INVALID["), ParsingException);
    EXPECT_THROW(WKTParser().createFromWKT("INVALID[]"), ParsingException);
}

// ---------------------------------------------------------------------------

TEST(wkt_parse, invalid_SPHEROID) {
    EXPECT_NO_THROW(WKTParser().createFromWKT("SPHEROID[\"x\",1,0.5]"));
    EXPECT_THROW(WKTParser().createFromWKT("SPHEROID[\"x\"]"),
                 ParsingException); // major axis not number
    EXPECT_THROW(WKTParser().createFromWKT("SPHEROID[\"x\",\"1\",0.5]"),
                 ParsingException); // major axis not number
    EXPECT_THROW(WKTParser().createFromWKT("SPHEROID[\"x\",1,\"0.5\"]"),
                 ParsingException); // reverse flatting not number
}

// ---------------------------------------------------------------------------

TEST(wkt_parse, invalid_DATUM) {
    EXPECT_NO_THROW(
        WKTParser().createFromWKT("DATUM[\"x\",SPHEROID[\"x\",1,0.5]]"));
    EXPECT_THROW(WKTParser().createFromWKT("DATUM[\"x\"]"), ParsingException);
    EXPECT_THROW(WKTParser().createFromWKT("DATUM[\"x\",FOO[]]"),
                 ParsingException);
}

// ---------------------------------------------------------------------------

TEST(wkt_parse, invalid_GEOGCS) {
    EXPECT_NO_THROW(WKTParser().createFromWKT(
        "GEOGCS[\"x\",DATUM[\"x\",SPHEROID[\"x\",1,0.5]],PRIMEM[\"x\",0],UNIT["
        "\"degree\",0.0174532925199433]]"));

    // missing PRIMEM
    EXPECT_THROW(
        WKTParser().createFromWKT("GEOGCS[\"x\",DATUM[\"x\",SPHEROID[\"x\",1,0."
                                  "5]],UNIT[\"degree\",0.0174532925199433]]"),
        ParsingException);

    // missing UNIT
    EXPECT_THROW(
        WKTParser().createFromWKT(
            "GEOGCS[\"x\",DATUM[\"x\",SPHEROID[\"x\",1,0.5]],PRIMEM[\"x\",0]]"),
        ParsingException);

    EXPECT_THROW(WKTParser().createFromWKT("GEOGCS[\"x\"]"), ParsingException);
    EXPECT_THROW(WKTParser().createFromWKT("GEOGCS[\"x\",FOO[]]"),
                 ParsingException);

    // not enough children for DATUM
    EXPECT_THROW(
        WKTParser().createFromWKT("GEOGCS[\"x\",DATUM[\"x\"],PRIMEM[\"x\",0],"
                                  "UNIT[\"degree\",0.0174532925199433]]"),
        ParsingException);

    // not enough children for AUTHORITY
    EXPECT_THROW(WKTParser().createFromWKT("GEOGCS[\"x\",DATUM[\"x\",SPHEROID["
                                           "\"x\",1,0.5]],PRIMEM[\"x\",0],UNIT["
                                           "\"degree\",0.0174532925199433],"
                                           "AUTHORITY[\"x\"]]"),
                 ParsingException);

    // not enough children for AUTHORITY, but ignored
    EXPECT_NO_THROW(WKTParser().setStrict(false).createFromWKT(
        "GEOGCS[\"x\",DATUM[\"x\",SPHEROID[\"x\",1,0.5]],PRIMEM[\"x\",0],UNIT["
        "\"degree\",0.0174532925199433],AUTHORITY[\"x\"]]"));

    EXPECT_NO_THROW(WKTParser().createFromWKT(
        "GEOGCS[\"x\",DATUM[\"x\",SPHEROID[\"x\",1,0.5]],PRIMEM[\"x\",0],UNIT["
        "\"degree\",0.0174532925199433]]"));

    // PRIMEM not numeric
    EXPECT_THROW(
        WKTParser().createFromWKT(
            "GEOGCS[\"x\",DATUM[\"x\",SPHEROID[\"x\",1,0."
            "5]],PRIMEM[\"x\",\"a\"],UNIT[\"degree\",0.0174532925199433]]"),
        ParsingException);

    // not enough children for PRIMEM
    EXPECT_THROW(WKTParser().createFromWKT("GEOGCS[\"x\",DATUM[\"x\",SPHEROID["
                                           "\"x\",1,0.5]],PRIMEM[\"x\"],UNIT["
                                           "\"degree\",0.0174532925199433]]"),
                 ParsingException);

    EXPECT_NO_THROW(WKTParser().createFromWKT(
        "GEOGCS[\"x\",DATUM[\"x\",SPHEROID[\"x\",1,0.5]],PRIMEM[\"x\",0],UNIT["
        "\"degree\",0.0174532925199433],AXIS[\"latitude\","
        "NORTH],AXIS[\"longitude\",EAST]]"));

    // one axis only
    EXPECT_THROW(WKTParser().createFromWKT(
                     "GEOGCS[\"x\",DATUM[\"x\",SPHEROID[\"x\",1,0."
                     "5]],PRIMEM[\"x\",0],UNIT[\"degree\",0.0174532925199433],"
                     "AXIS[\"latitude\",NORTH]]"),
                 ParsingException);

    // invalid axis
    EXPECT_THROW(WKTParser().createFromWKT("GEOGCS[\"x\",DATUM[\"x\",SPHEROID["
                                           "\"x\",1,0.5]],PRIMEM[\"x\",0],UNIT["
                                           "\"degree\",0.0174532925199433],"
                                           "AXIS[\"latitude\","
                                           "NORTH],AXIS[\"longitude\"]]"),
                 ParsingException);
}

// ---------------------------------------------------------------------------

TEST(wkt_parse, invalid_UNIT) {
    std::string startWKT("GEODCRS[\"x\",DATUM[\"x\",SPHEROID[\"x\",1,0.5]],CS["
                         "ellipsoidal,2],AXIS[\"latitude\",north],AXIS["
                         "\"longitude\",east,");

    EXPECT_NO_THROW(WKTParser().createFromWKT(
        startWKT + "UNIT[\"degree\",0.0174532925199433]]]"));

    // not enough children
    EXPECT_THROW(WKTParser().createFromWKT(startWKT + "UNIT[\"x\"]]]"),
                 ParsingException);

    // invalid conversion factor
    EXPECT_THROW(
        WKTParser().createFromWKT(startWKT + "UNIT[\"x\",\"invalid\"]]]"),
        ParsingException);

    // invalid ID
    EXPECT_THROW(
        WKTParser().createFromWKT(startWKT + "UNIT[\"x\",1,ID[\"x\"]]]]"),
        ParsingException);
}

// ---------------------------------------------------------------------------

TEST(wkt_parse, invalid_GEOCCS) {
    EXPECT_NO_THROW(
        WKTParser().createFromWKT("GEOCCS[\"x\",DATUM[\"x\",SPHEROID[\"x\",1,0."
                                  "5]],PRIMEM[\"x\",0],UNIT[\"metre\",1]]"));

    // missing PRIMEM
    EXPECT_THROW(WKTParser().createFromWKT("GEOCCS[\"x\",DATUM[\"x\",SPHEROID["
                                           "\"x\",1,0.5]],UNIT[\"metre\",1]]"),
                 ParsingException);

    // missing UNIT
    EXPECT_THROW(
        WKTParser().createFromWKT(
            "GEOCCS[\"x\",DATUM[\"x\",SPHEROID[\"x\",1,0.5]],PRIMEM[\"x\",0]]"),
        ParsingException);

    // ellipsoidal CS is invalid in a GEOCCS
    EXPECT_THROW(WKTParser().createFromWKT("GEOCCS[\"x\",DATUM[\"x\",SPHEROID["
                                           "\"x\",1,0.5]],PRIMEM[\"x\",0],UNIT["
                                           "\"degree\",0.0174532925199433],"
                                           "AXIS[\"latitude\","
                                           "NORTH],AXIS[\"longitude\",EAST]]"),
                 ParsingException);

    // 3 axis required
    EXPECT_THROW(WKTParser().createFromWKT(
                     "GEOCCS[\"x\",DATUM[\"x\",SPHEROID[\"x\",1,0.5]],PRIMEM["
                     "\"x\",0],UNIT[\"metre\",1],AXIS["
                     "\"Geocentric X\",OTHER],AXIS[\"Geocentric Y\",OTHER]]"),
                 ParsingException);
}

// ---------------------------------------------------------------------------

TEST(wkt_parse, invalid_CS_of_GEODCRS) {

    std::string startWKT("GEODCRS[\"x\",DATUM[\"x\",SPHEROID[\"x\",1,0.5]]");

    // missing CS
    EXPECT_THROW(WKTParser().createFromWKT(startWKT + "]"), ParsingException);

    // CS: not enough children
    EXPECT_THROW(WKTParser().createFromWKT(startWKT + ",CS[x]]"),
                 ParsingException);

    // CS: invalid type
    EXPECT_THROW(WKTParser().createFromWKT(startWKT + ",CS[x,2]]"),
                 ParsingException);

    // CS: invalid number of axis
    EXPECT_THROW(WKTParser().createFromWKT(startWKT + ",CS[ellipsoidal,0]]"),
                 ParsingException);

    // CS: number of axis is not a number
    EXPECT_THROW(
        WKTParser().createFromWKT(startWKT + ",CS[ellipsoidal,\"x\"]]"),
        ParsingException);

    // CS: invalid CS type
    EXPECT_THROW(WKTParser().createFromWKT(startWKT +
                                           ",CS[invalid,2],AXIS[\"latitude\","
                                           "north],AXIS[\"longitude\",east]]"),
                 ParsingException);

    // CS: OK
    EXPECT_NO_THROW(WKTParser().createFromWKT(
        startWKT + ",CS[ellipsoidal,2],AXIS[\"latitude\",north],AXIS["
                   "\"longitude\",east]]"));

    // CS: Cartesian with 2 axis unexpected
    EXPECT_THROW(WKTParser().createFromWKT(startWKT +
                                           ",CS[Cartesian,2],AXIS[\"latitude\","
                                           "north],AXIS[\"longitude\",east]]"),
                 ParsingException);

    // CS: missing axis
    EXPECT_THROW(WKTParser().createFromWKT(
                     startWKT + ",CS[ellipsoidal,2],AXIS[\"latitude\",north]]"),
                 ParsingException);

    // not enough children in AXIS
    EXPECT_THROW(
        WKTParser().createFromWKT(
            startWKT +
            ",CS[ellipsoidal,2],AXIS[\"latitude\",north],AXIS[\"longitude\"]]"),
        ParsingException);

    // not enough children in ORDER
    EXPECT_THROW(WKTParser().createFromWKT(
                     startWKT +
                     ",CS[ellipsoidal,2],AXIS[\"latitude\",north,ORDER[]],AXIS["
                     "\"longitude\",east]]"),
                 ParsingException);

    // invalid value in ORDER
    EXPECT_THROW(
        WKTParser().createFromWKT(
            startWKT +
            ",CS[ellipsoidal,2],AXIS[\"latitude\",north,ORDER[\"x\"]],AXIS["
            "\"longitude\",east]]"),
        ParsingException);

    // unexpected ORDER value
    EXPECT_THROW(
        WKTParser().createFromWKT(
            startWKT +
            ",CS[ellipsoidal,2],AXIS[\"latitude\",north,ORDER[2]],AXIS["
            "\"longitude\",east]]"),
        ParsingException);

    // Invalid CS type
    EXPECT_THROW(WKTParser().createFromWKT(
                     startWKT +
                     ",CS[vertical,1],\n"
                     "        AXIS[\"gravity-related height (H)\",up],\n"
                     "        UNIT[\"metre\",1]]"),
                 ParsingException);
}

// ---------------------------------------------------------------------------

TEST(wkt_parse, invalid_CS_of_GEOGRAPHICCRS) {
    // A GeographicCRS must have an ellipsoidal CS
    EXPECT_THROW(WKTParser().createFromWKT(
                     "GEOGRAPHICCRS[\"x\",DATUM[\"x\",SPHEROID[\"x\",1,0.5]],"
                     "CS[Cartesian,3],AXIS[\"(X)\",geocentricX],AXIS[\"(Y)\","
                     "geocentricY],AXIS[\"(Z)\",geocentricZ]]"),
                 ParsingException);
}

// ---------------------------------------------------------------------------

TEST(wkt_parse, invalid_PROJCRS) {
    // missing BASEGEODCRS
    EXPECT_THROW(
        WKTParser().createFromWKT("PROJCRS[\"WGS 84 / UTM zone 31N\"]"),
        ParsingException);

    std::string startWKT("PROJCRS[\"WGS 84 / UTM zone 31N\",BASEGEOGCRS[\"WGS "
                         "84\",DATUM[\"WGS_1984\",ELLIPSOID[\"WGS "
                         "84\",6378137,298.257223563]],UNIT[\"degree\",0."
                         "0174532925199433]]");

    // missing CONVERSION
    EXPECT_THROW(WKTParser().createFromWKT(startWKT + "]"), ParsingException);

    // not enough children in CONVERSION
    EXPECT_THROW(WKTParser().createFromWKT(startWKT + ",CONVERSION[\"x\"]]"),
                 ParsingException);

    // not enough children in METHOD
    EXPECT_THROW(
        WKTParser().createFromWKT(startWKT + ",CONVERSION[\"x\",METHOD[]]]"),
        ParsingException);

    // not enough children in PARAMETER
    EXPECT_THROW(
        WKTParser().createFromWKT(
            startWKT + ",CONVERSION[\"x\",METHOD[\"y\"],PARAMETER[\"z\"]]]"),
        ParsingException);

    // non numeric value for PARAMETER
    EXPECT_THROW(
        WKTParser().createFromWKT(
            startWKT +
            ",CONVERSION[\"x\",METHOD[\"y\"],PARAMETER[\"z\",\"foo\"]]]"),
        ParsingException);

    // missing CS
    EXPECT_THROW(WKTParser().createFromWKT(startWKT +
                                           ",CONVERSION[\"x\",METHOD[\"y\"]]]"),
                 ParsingException);

    // CS is not Cartesian
    EXPECT_THROW(WKTParser().createFromWKT(
                     startWKT + ",CONVERSION[\"x\",METHOD[\"y\"]],CS["
                                "ellipsoidal,2],AXIS[\"latitude\",north],AXIS["
                                "\"longitude\",east]]"),
                 ParsingException);

    // not enough children in MERIDIAN
    EXPECT_THROW(WKTParser().createFromWKT(
                     startWKT + ",CONVERSION[\"x\",METHOD[\"y\"]],CS["
                                "Cartesian,2],AXIS[\"easting (X)\",south,"
                                "MERIDIAN[90]],AXIS["
                                "\"northing (Y)\",south]]"),
                 ParsingException);

    // non numeric angle value for MERIDIAN
    EXPECT_THROW(
        WKTParser().createFromWKT(
            startWKT +
            ",CONVERSION[\"x\",METHOD[\"y\"]],CS["
            "Cartesian,2],AXIS[\"easting (X)\",south,"
            "MERIDIAN[\"x\",UNIT[\"degree\",0.0174532925199433]]],AXIS["
            "\"northing (Y)\",south]]"),
        ParsingException);
}

// ---------------------------------------------------------------------------

TEST(wkt_parse, invalid_PROJCS) {

    std::string startWKT(
        "PROJCS[\"WGS 84 / UTM zone 31N\",\n"
        "    GEOGCS[\"WGS 84\",\n"
        "        DATUM[\"WGS_1984\",\n"
        "            SPHEROID[\"WGS 84\",6378137,298.257223563,\n"
        "                AUTHORITY[\"EPSG\",\"7030\"]],\n"
        "            AUTHORITY[\"EPSG\",\"6326\"]],\n"
        "        PRIMEM[\"x\",0],\n"
        "        UNIT[\"degree\",0.0174532925199433,\n"
        "            AUTHORITY[\"EPSG\",9122]],\n"
        "        AXIS[\"latitude\",NORTH],\n"
        "        AXIS[\"longitude\",EAST],\n"
        "        AUTHORITY[\"EPSG\",\"4326\"]]\n");

    // missing PROJECTION
    EXPECT_THROW(WKTParser().createFromWKT(startWKT + "]"), ParsingException);

    // not enough children in PROJECTION
    EXPECT_THROW(WKTParser().createFromWKT(startWKT +
                                           ",PROJECTION[],UNIT[\"metre\",1]]"),
                 ParsingException);

    // not enough children in PARAMETER
    EXPECT_THROW(WKTParser().createFromWKT(
                     startWKT +
                     ",PROJECTION[\"x\"],PARAMETER[\"z\"],UNIT[\"metre\",1]]"),
                 ParsingException);

    // not enough children in PARAMETER
    EXPECT_THROW(
        WKTParser().createFromWKT(
            startWKT +
            ",PROJECTION[\"x\"],PARAMETER[\"z\",\"foo\"],UNIT[\"metre\",1]]"),
        ParsingException);

    EXPECT_NO_THROW(WKTParser().createFromWKT(
        startWKT + ",PROJECTION[\"x\"],UNIT[\"metre\",1]]"));

    // missing UNIT
    EXPECT_THROW(WKTParser().createFromWKT(startWKT + ",PROJECTION[\"x\"]]"),
                 ParsingException);
}

// ---------------------------------------------------------------------------

TEST(wkt_parse, invalid_VERTCRS) {

    // missing VDATUM
    EXPECT_THROW(WKTParser().createFromWKT(
                     "VERTCRS[\"foo\",CS[vertical,1],AXIS[\"x\",up]]"),
                 ParsingException);

    // missing CS
    EXPECT_THROW(WKTParser().createFromWKT("VERTCRS[\"foo\",VDATUM[\"bar\"]]"),
                 ParsingException);

    // CS is not of type vertical
    EXPECT_THROW(WKTParser().createFromWKT("VERTCRS[\"foo\",VDATUM[\"bar\"],CS["
                                           "ellipsoidal,2],AXIS[\"latitude\","
                                           "north],AXIS["
                                           "\"longitude\",east]]"),
                 ParsingException);

    // verticalCS should have only 1 axis
    EXPECT_THROW(
        WKTParser().createFromWKT("VERTCRS[\"foo\",VDATUM[\"bar\"],CS[vertical,"
                                  "2],AXIS[\"latitude\",north],AXIS["
                                  "\"longitude\",east]]"),
        ParsingException);
}

// ---------------------------------------------------------------------------

TEST(wkt_parse, invalid_VERT_CS) {

    EXPECT_NO_THROW(WKTParser().createFromWKT(
        "VERT_CS[\"x\",VERT_DATUM[\"y\",2005],UNIT[\"metre\",1]]"));

    // Missing VERT_DATUM
    EXPECT_THROW(WKTParser().createFromWKT("VERT_CS[\"x\",UNIT[\"metre\",1]]"),
                 ParsingException);

    // Missing UNIT
    EXPECT_THROW(
        WKTParser().createFromWKT("VERT_CS[\"x\",VERT_DATUM[\"y\",2005]]"),
        ParsingException);
}

// ---------------------------------------------------------------------------

TEST(wkt_parse, invalid_COORDINATEOPERATION) {

    std::string src_wkt;
    {
        auto formatter = WKTFormatter::create();
        formatter->setOutputId(false);
        src_wkt = GeographicCRS::EPSG_4326->exportToWKT(formatter);
    }

    std::string dst_wkt;
    {
        auto formatter = WKTFormatter::create();
        formatter->setOutputId(false);
        dst_wkt = GeographicCRS::EPSG_4807->exportToWKT(formatter);
    }

    std::string interpolation_wkt;
    {
        auto formatter = WKTFormatter::create();
        formatter->setOutputId(false);
        interpolation_wkt = GeographicCRS::EPSG_4979->exportToWKT(formatter);
    }

    // Valid
    {
        auto wkt = "COORDINATEOPERATION[\"transformationName\",\n"
                   "    SOURCECRS[" +
                   src_wkt + "],\n"
                             "    TARGETCRS[" +
                   dst_wkt + "],\n"
                             "    METHOD[\"operationMethodName\"],\n"
                             "    PARAMETERFILE[\"paramName\",\"foo.bin\"]]";

        EXPECT_NO_THROW(WKTParser().createFromWKT(wkt));
    }

    // Missing SOURCECRS
    {
        auto wkt = "COORDINATEOPERATION[\"transformationName\",\n"
                   "    TARGETCRS[" +
                   dst_wkt + "],\n"
                             "    METHOD[\"operationMethodName\"],\n"
                             "    PARAMETERFILE[\"paramName\",\"foo.bin\"]]";

        EXPECT_THROW(WKTParser().createFromWKT(wkt), ParsingException);
    }

    // Invalid content in SOURCECRS
    {
        auto wkt = "COORDINATEOPERATION[\"transformationName\",\n"
                   "    SOURCECRS[FOO],\n"
                   "    TARGETCRS[" +
                   dst_wkt + "],\n"
                             "    METHOD[\"operationMethodName\"],\n"
                             "    PARAMETERFILE[\"paramName\",\"foo.bin\"]]";

        EXPECT_THROW(WKTParser().createFromWKT(wkt), ParsingException);
    }

    // Missing TARGETCRS
    {
        auto wkt = "COORDINATEOPERATION[\"transformationName\",\n"
                   "    SOURCECRS[" +
                   src_wkt + "],\n"
                             "    METHOD[\"operationMethodName\"],\n"
                             "    PARAMETERFILE[\"paramName\",\"foo.bin\"]]";

        EXPECT_THROW(WKTParser().createFromWKT(wkt), ParsingException);
    }

    // Invalid content in TARGETCRS
    {
        auto wkt = "COORDINATEOPERATION[\"transformationName\",\n"
                   "    SOURCECRS[" +
                   src_wkt + "],\n"
                             "    TARGETCRS[FOO],\n"
                             "    METHOD[\"operationMethodName\"],\n"
                             "    PARAMETERFILE[\"paramName\",\"foo.bin\"]]";

        EXPECT_THROW(WKTParser().createFromWKT(wkt), ParsingException);
    }

    // Missing METHOD
    {
        auto wkt = "COORDINATEOPERATION[\"transformationName\",\n"
                   "    SOURCECRS[" +
                   src_wkt + "],\n"
                             "    TARGETCRS[" +
                   dst_wkt + "]]";

        EXPECT_THROW(WKTParser().createFromWKT(wkt), ParsingException);
    }

    // Invalid METHOD
    {
        auto wkt = "COORDINATEOPERATION[\"transformationName\",\n"
                   "    SOURCECRS[" +
                   src_wkt + "],\n"
                             "    TARGETCRS[" +
                   dst_wkt + "],\n"
                             "    METHOD[],\n"
                             "    PARAMETERFILE[\"paramName\",\"foo.bin\"]]";

        EXPECT_THROW(WKTParser().createFromWKT(wkt), ParsingException);
    }
}

// ---------------------------------------------------------------------------

TEST(wkt_parse, invalid_CONCATENATEDOPERATION) {

    // No STEP
    EXPECT_THROW(WKTParser().createFromWKT("CONCATENATEDOPERATION[\"name\"]"),
                 ParsingException);

    auto transf_1 = Transformation::create(
        PropertyMap().set(IdentifiedObject::NAME_KEY, "transf_1"),
        nn_static_pointer_cast<CRS>(GeographicCRS::EPSG_4326),
        nn_static_pointer_cast<CRS>(GeographicCRS::EPSG_4807), nullptr,
        PropertyMap().set(IdentifiedObject::NAME_KEY, "operationMethodName"),
        std::vector<OperationParameterNNPtr>{OperationParameter::create(
            PropertyMap().set(IdentifiedObject::NAME_KEY, "paramName"))},
        std::vector<ParameterValueNNPtr>{
            ParameterValue::createFilename("foo.bin")},
        std::vector<PositionalAccuracyNNPtr>());

    // One single STEP
    {
        auto wkt = "CONCATENATEDOPERATION[\"name\",\n"
                   "    SOURCECRS[" +
                   transf_1->sourceCRS()->exportToWKT(WKTFormatter::create()) +
                   "],\n"
                   "    TARGETCRS[" +
                   transf_1->targetCRS()->exportToWKT(WKTFormatter::create()) +
                   "],\n"
                   "    STEP[" +
                   transf_1->exportToWKT(WKTFormatter::create()) +
                   "],\n"
                   "    ID[\"codeSpace\",\"code\"],\n"
                   "    REMARK[\"my remarks\"]]";

        EXPECT_THROW(WKTParser().createFromWKT(wkt), ParsingException);
    }

    // empty STEP
    {
        auto wkt = "CONCATENATEDOPERATION[\"name\",\n"
                   "    SOURCECRS[" +
                   transf_1->sourceCRS()->exportToWKT(WKTFormatter::create()) +
                   "],\n"
                   "    TARGETCRS[" +
                   transf_1->targetCRS()->exportToWKT(WKTFormatter::create()) +
                   "],\n"
                   "    STEP[],\n"
                   "    STEP[],\n"
                   "    ID[\"codeSpace\",\"code\"],\n"
                   "    REMARK[\"my remarks\"]]";
        EXPECT_THROW(WKTParser().createFromWKT(wkt), ParsingException);
    }

    // Invalid content in STEP
    {
        auto wkt = "CONCATENATEDOPERATION[\"name\",\n"
                   "    SOURCECRS[" +
                   transf_1->sourceCRS()->exportToWKT(WKTFormatter::create()) +
                   "],\n"
                   "    TARGETCRS[" +
                   transf_1->targetCRS()->exportToWKT(WKTFormatter::create()) +
                   "],\n"
                   "    STEP[" +
                   transf_1->sourceCRS()->exportToWKT(WKTFormatter::create()) +
                   "],\n"
                   "    STEP[" +
                   transf_1->sourceCRS()->exportToWKT(WKTFormatter::create()) +
                   "],\n"
                   "    ID[\"codeSpace\",\"code\"],\n"
                   "    REMARK[\"my remarks\"]]";

        EXPECT_THROW(WKTParser().createFromWKT(wkt), ParsingException);
    }
}

// ---------------------------------------------------------------------------

TEST(wkt_parse, invalid_BOUNDCRS) {

    auto projcrs = ProjectedCRS::create(
        PropertyMap().set(IdentifiedObject::NAME_KEY, "my PROJCRS"),
        GeographicCRS::create(
            PropertyMap().set(IdentifiedObject::NAME_KEY, "my GEOGCRS"),
            GeodeticReferenceFrame::EPSG_6326,
            EllipsoidalCS::createLatitudeLongitude(UnitOfMeasure::DEGREE)),
        Conversion::createUTM(PropertyMap(), 31, true),
        CartesianCS::createEastingNorthing(UnitOfMeasure::METRE));

    EXPECT_NO_THROW(WKTParser().createFromWKT(
        "BOUNDCRS[SOURCECRS[" + projcrs->exportToWKT(WKTFormatter::create()) +
        "],\n" + "TARGETCRS[" +
        GeographicCRS::EPSG_4326->exportToWKT(WKTFormatter::create()) +
        "],\n"
        "    ABRIDGEDTRANSFORMATION[\"foo\",\n"
        "        METHOD[\"bar\"]]]"));

    // Missing SOURCECRS
    EXPECT_THROW(
        WKTParser().createFromWKT(
            "BOUNDCRS[TARGETCRS[" +
            GeographicCRS::EPSG_4326->exportToWKT(WKTFormatter::create()) +
            "],\n"
            "    ABRIDGEDTRANSFORMATION[\"foo\",\n"
            "        METHOD[\"bar\"]]]"),
        ParsingException);

    // Invalid SOURCECRS
    EXPECT_THROW(
        WKTParser().createFromWKT(
            "BOUNDCRS[SOURCECRS[foo], TARGETCRS[" +
            GeographicCRS::EPSG_4326->exportToWKT(WKTFormatter::create()) +
            "],\n"
            "    ABRIDGEDTRANSFORMATION[\"foo\",\n"
            "        METHOD[\"bar\"]]]"),
        ParsingException);

    // Missing TARGETCRS
    EXPECT_THROW(
        WKTParser().createFromWKT("BOUNDCRS[SOURCECRS[" +
                                  projcrs->exportToWKT(WKTFormatter::create()) +
                                  "],\n"
                                  "    ABRIDGEDTRANSFORMATION[\"foo\",\n"
                                  "        METHOD[\"bar\"]]]"),
        ParsingException);

    // Invalid TARGETCRS
    EXPECT_THROW(
        WKTParser().createFromWKT("BOUNDCRS[SOURCECRS[" +
                                  projcrs->exportToWKT(WKTFormatter::create()) +
                                  "],TARGETCRS[\"foo\"],\n"
                                  "    ABRIDGEDTRANSFORMATION[\"foo\",\n"
                                  "        METHOD[\"bar\"]]]"),
        ParsingException);

    // Missing ABRIDGEDTRANSFORMATION
    EXPECT_THROW(
        WKTParser().createFromWKT(
            "BOUNDCRS[SOURCECRS[" +
            projcrs->exportToWKT(WKTFormatter::create()) + "],\n" +
            "TARGETCRS[" +
            GeographicCRS::EPSG_4326->exportToWKT(WKTFormatter::create()) +
            "]]"),
        ParsingException);

    // Missing METHOD
    EXPECT_THROW(
        WKTParser().createFromWKT(
            "BOUNDCRS[SOURCECRS[" +
            projcrs->exportToWKT(WKTFormatter::create()) + "],\n" +
            "TARGETCRS[" +
            GeographicCRS::EPSG_4326->exportToWKT(WKTFormatter::create()) +
            "],"
            "ABRIDGEDTRANSFORMATION[\"foo\"]]"),
        ParsingException);

    // Invalid METHOD
    EXPECT_THROW(
        WKTParser().createFromWKT(
            "BOUNDCRS[SOURCECRS[" +
            projcrs->exportToWKT(WKTFormatter::create()) + "],\n" +
            "TARGETCRS[" +
            GeographicCRS::EPSG_4326->exportToWKT(WKTFormatter::create()) +
            "],"
            "ABRIDGEDTRANSFORMATION[\"foo\",METHOD[]]]"),
        ParsingException);
}

// ---------------------------------------------------------------------------

TEST(wkt_parse, invalid_TOWGS84) {
    EXPECT_THROW(WKTParser().createFromWKT(
                     "GEOGCS[\"WGS 84\","
                     "    DATUM[\"WGS_1984\","
                     "        SPHEROID[\"WGS 84\",6378137,298.257223563],"
                     "        TOWGS84[0]],"
                     "    PRIMEM[\"Greenwich\",0],"
                     "    UNIT[\"degree\",0.0174532925199433]]"),
                 ParsingException);

    EXPECT_THROW(WKTParser().createFromWKT(
                     "GEOGCS[\"WGS 84\","
                     "    DATUM[\"WGS_1984\","
                     "        SPHEROID[\"WGS 84\",6378137,298.257223563],"
                     "        TOWGS84[0,0,0,0,0,0,\"foo\"]],"
                     "    PRIMEM[\"Greenwich\",0],"
                     "    UNIT[\"degree\",0.0174532925199433]]"),
                 ParsingException);
}

// ---------------------------------------------------------------------------

TEST(wkt_parse, invalid_DerivedGeographicCRS) {

    EXPECT_NO_THROW(WKTParser().createFromWKT(
        "GEODCRS[\"WMO Atlantic Pole\",\n"
        "    BASEGEODCRS[\"WGS 84\",\n"
        "        DATUM[\"World Geodetic System 1984\",\n"
        "            ELLIPSOID[\"WGS 84\",6378137,298.257223563]]],\n"
        "    DERIVINGCONVERSION[\"foo\",\n"
        "        METHOD[\"bar\"]],\n"
        "    CS[ellipsoidal,2],\n"
        "        AXIS[\"latitude\",north],\n"
        "        AXIS[\"longitude\",east]]"));

    // Missing DERIVINGCONVERSION
    EXPECT_THROW(
        WKTParser().createFromWKT(
            "GEODCRS[\"WMO Atlantic Pole\",\n"
            "    BASEGEODCRS[\"WGS 84\",\n"
            "        DATUM[\"World Geodetic System 1984\",\n"
            "            ELLIPSOID[\"WGS 84\",6378137,298.257223563]]],\n"
            "    CS[ellipsoidal,2],\n"
            "        AXIS[\"latitude\",north],\n"
            "        AXIS[\"longitude\",east]]"),
        ParsingException);

    // Missing CS
    EXPECT_THROW(
        WKTParser().createFromWKT(
            "GEODCRS[\"WMO Atlantic Pole\",\n"
            "    BASEGEODCRS[\"WGS 84\",\n"
            "        DATUM[\"World Geodetic System 1984\",\n"
            "            ELLIPSOID[\"WGS 84\",6378137,298.257223563]]],\n"
            "    DERIVINGCONVERSION[\"foo\",\n"
            "        METHOD[\"bar\"]]]"),
        ParsingException);

    // CS should be ellipsoidal given root node is GEOGCRS
    EXPECT_THROW(
        WKTParser().createFromWKT(
            "GEOGCRS[\"WMO Atlantic Pole\",\n"
            "    BASEGEOGCRS[\"WGS 84\",\n"
            "        DATUM[\"World Geodetic System 1984\",\n"
            "            ELLIPSOID[\"WGS 84\",6378137,298.257223563]]],\n"
            "    DERIVINGCONVERSION[\"foo\",\n"
            "        METHOD[\"bar\"]],\n"
            "    CS[Cartesian,3],\n"
            "        AXIS[\"(X)\",geocentricX],\n"
            "        AXIS[\"(Y)\",geocentricY],\n"
            "        AXIS[\"(Z)\",geocentricZ],\n"
            "        UNIT[\"metre\",1]]"),
        ParsingException);

    // CS should have 3 axis
    EXPECT_THROW(
        WKTParser().createFromWKT(
            "GEODCRS[\"WMO Atlantic Pole\",\n"
            "    BASEGEODCRS[\"WGS 84\",\n"
            "        DATUM[\"World Geodetic System 1984\",\n"
            "            ELLIPSOID[\"WGS 84\",6378137,298.257223563]]],\n"
            "    DERIVINGCONVERSION[\"foo\",\n"
            "        METHOD[\"bar\"]],\n"
            "    CS[Cartesian,2],\n"
            "        AXIS[\"(X)\",geocentricX],\n"
            "        AXIS[\"(Y)\",geocentricY],\n"
            "        UNIT[\"metre\",1]]"),
        ParsingException);

    // Invalid CS type
    EXPECT_THROW(
        WKTParser().createFromWKT(
            "GEODCRS[\"WMO Atlantic Pole\",\n"
            "    BASEGEODCRS[\"WGS 84\",\n"
            "        DATUM[\"World Geodetic System 1984\",\n"
            "            ELLIPSOID[\"WGS 84\",6378137,298.257223563]]],\n"
            "    DERIVINGCONVERSION[\"foo\",\n"
            "        METHOD[\"bar\"]],\n"
            "    CS[vertical,1],\n"
            "        AXIS[\"gravity-related height (H)\",up],\n"
            "        UNIT[\"metre\",1]]"),
        ParsingException);
}

// ---------------------------------------------------------------------------

TEST(wkt_parse, invalid_TemporalCRS) {

    EXPECT_NO_THROW(
        WKTParser().createFromWKT("TIMECRS[\"Temporal CRS\",\n"
                                  "    TDATUM[\"Gregorian calendar\",\n"
                                  "        TIMEORIGIN[0000-01-01]],\n"
                                  "    CS[temporal,1],\n"
                                  "        AXIS[\"time (T)\",future]]"));

    // Missing TDATUM
    EXPECT_THROW(
        WKTParser().createFromWKT("TIMECRS[\"Temporal CRS\",\n"
                                  "    CS[temporal,1],\n"
                                  "        AXIS[\"time (T)\",future]]"),
        ParsingException);

    // Missing CS
    EXPECT_THROW(
        WKTParser().createFromWKT("TIMECRS[\"Temporal CRS\",\n"
                                  "    TDATUM[\"Gregorian calendar\",\n"
                                  "        TIMEORIGIN[0000-01-01]]]"),
        ParsingException);

    // CS should be temporal
    EXPECT_THROW(
        WKTParser().createFromWKT("TIMECRS[\"Temporal CRS\",\n"
                                  "    TDATUM[\"Gregorian calendar\",\n"
                                  "        TIMEORIGIN[0000-01-01]],\n"
                                  "    CS[Cartesian,2],\n"
                                  "        AXIS[\"(X)\",geocentricX],\n"
                                  "        AXIS[\"(Y)\",geocentricY],\n"
                                  "        UNIT[\"metre\",1]]"),
        ParsingException);

    // CS should have 1 axis
    EXPECT_THROW(
        WKTParser().createFromWKT("TIMECRS[\"Temporal CRS\",\n"
                                  "    TDATUM[\"Gregorian calendar\",\n"
                                  "        TIMEORIGIN[0000-01-01]],\n"
                                  "    CS[temporal,2],\n"
                                  "        AXIS[\"time (T)\",future],\n"
                                  "        AXIS[\"time2 (T)\",future]]"),
        ParsingException);

    // CS should have 1 axis
    EXPECT_THROW(
        WKTParser().createFromWKT("TIMECRS[\"Temporal CRS\",\n"
                                  "    TDATUM[\"Gregorian calendar\",\n"
                                  "        TIMEORIGIN[0000-01-01]],\n"
                                  "    CS[TemporalDateTime,2],\n"
                                  "        AXIS[\"time (T)\",future],\n"
                                  "        AXIS[\"time2 (T)\",future]]"),
        ParsingException);

    // CS should have 1 axis
    EXPECT_THROW(
        WKTParser().createFromWKT("TIMECRS[\"Temporal CRS\",\n"
                                  "    TDATUM[\"Gregorian calendar\",\n"
                                  "        TIMEORIGIN[0000-01-01]],\n"
                                  "    CS[TemporalCount,2],\n"
                                  "        AXIS[\"time (T)\",future],\n"
                                  "        AXIS[\"time2 (T)\",future]]"),
        ParsingException);

    // CS should have 1 axis
    EXPECT_THROW(
        WKTParser().createFromWKT("TIMECRS[\"Temporal CRS\",\n"
                                  "    TDATUM[\"Gregorian calendar\",\n"
                                  "        TIMEORIGIN[0000-01-01]],\n"
                                  "    CS[TemporalMeasure,2],\n"
                                  "        AXIS[\"time (T)\",future],\n"
                                  "        AXIS[\"time2 (T)\",future]]"),
        ParsingException);
}

// ---------------------------------------------------------------------------

TEST(io, projstringformatter) {

    {
        auto fmt = PROJStringFormatter::create();
        fmt->addStep("my_proj");
        EXPECT_EQ(fmt->toString(), "+proj=my_proj");
    }

    {
        auto fmt = PROJStringFormatter::create();
        fmt->addStep("my_proj");
        fmt->setCurrentStepInverted(true);
        EXPECT_EQ(fmt->toString(), "+proj=pipeline +step +inv +proj=my_proj");
    }

    {
        auto fmt = PROJStringFormatter::create();
        fmt->addStep("my_proj1");
        fmt->addStep("my_proj2");
        EXPECT_EQ(fmt->toString(),
                  "+proj=pipeline +step +proj=my_proj1 +step +proj=my_proj2");
    }

    {
        auto fmt = PROJStringFormatter::create();
        fmt->addStep("my_proj1");
        fmt->setCurrentStepInverted(true);
        fmt->addStep("my_proj2");
        EXPECT_EQ(
            fmt->toString(),
            "+proj=pipeline +step +inv +proj=my_proj1 +step +proj=my_proj2");
    }

    {
        auto fmt = PROJStringFormatter::create();
        fmt->startInversion();
        fmt->addStep("my_proj1");
        fmt->setCurrentStepInverted(true);
        fmt->addStep("my_proj2");
        fmt->stopInversion();
        EXPECT_EQ(
            fmt->toString(),
            "+proj=pipeline +step +inv +proj=my_proj2 +step +proj=my_proj1");
    }

    {
        auto fmt = PROJStringFormatter::create();
        fmt->startInversion();
        fmt->addStep("my_proj1");
        fmt->setCurrentStepInverted(true);
        fmt->startInversion();
        fmt->addStep("my_proj2");
        fmt->stopInversion();
        fmt->stopInversion();
        EXPECT_EQ(fmt->toString(),
                  "+proj=pipeline +step +proj=my_proj2 +step +proj=my_proj1");
    }
}

// ---------------------------------------------------------------------------

TEST(io, projparse_longlat) {

    auto expected = "GEODCRS[\"unknown\",\n"
                    "    DATUM[\"World Geodetic System 1984\",\n"
                    "        ELLIPSOID[\"WGS 84\",6378137,298.257223563,\n"
                    "            LENGTHUNIT[\"metre\",1]],\n"
                    "        ID[\"EPSG\",6326]],\n"
                    "    PRIMEM[\"Greenwich\",0,\n"
                    "        ANGLEUNIT[\"degree\",0.0174532925199433],\n"
                    "        ID[\"EPSG\",8901]],\n"
                    "    CS[ellipsoidal,2],\n"
                    "        AXIS[\"longitude\",east,\n"
                    "            ORDER[1],\n"
                    "            ANGLEUNIT[\"degree\",0.0174532925199433,\n"
                    "                ID[\"EPSG\",9122]]],\n"
                    "        AXIS[\"latitude\",north,\n"
                    "            ORDER[2],\n"
                    "            ANGLEUNIT[\"degree\",0.0174532925199433,\n"
                    "                ID[\"EPSG\",9122]]]]";
    {
        auto obj = PROJStringParser().createFromPROJString("+proj=longlat");
        auto crs = nn_dynamic_pointer_cast<GeographicCRS>(obj);
        ASSERT_TRUE(crs != nullptr);
        WKTFormatterNNPtr f(WKTFormatter::create());
        crs->exportToWKT(f);
        EXPECT_EQ(f->toString(), expected);
    }

    {
        auto obj = PROJStringParser().createFromPROJString(
            "+proj=longlat +datum=WGS84");
        auto crs = nn_dynamic_pointer_cast<GeographicCRS>(obj);
        ASSERT_TRUE(crs != nullptr);
        WKTFormatterNNPtr f(WKTFormatter::create());
        crs->exportToWKT(f);
        EXPECT_EQ(f->toString(), expected);
    }
}

// ---------------------------------------------------------------------------

TEST(io, projparse_longlat_datum_NAD83) {
    auto obj =
        PROJStringParser().createFromPROJString("+proj=longlat +datum=NAD83");
    auto crs = nn_dynamic_pointer_cast<GeographicCRS>(obj);
    ASSERT_TRUE(crs != nullptr);
    WKTFormatterNNPtr f(WKTFormatter::create());
    crs->exportToWKT(f);
    EXPECT_EQ(f->toString(),
              "GEODCRS[\"unknown\",\n"
              "    DATUM[\"North American Datum 1983\",\n"
              "        ELLIPSOID[\"GRS 1980\",6378137,298.257222101,\n"
              "            LENGTHUNIT[\"metre\",1]],\n"
              "        ID[\"EPSG\",6269]],\n"
              "    PRIMEM[\"Greenwich\",0,\n"
              "        ANGLEUNIT[\"degree\",0.0174532925199433],\n"
              "        ID[\"EPSG\",8901]],\n"
              "    CS[ellipsoidal,2],\n"
              "        AXIS[\"longitude\",east,\n"
              "            ORDER[1],\n"
              "            ANGLEUNIT[\"degree\",0.0174532925199433,\n"
              "                ID[\"EPSG\",9122]]],\n"
              "        AXIS[\"latitude\",north,\n"
              "            ORDER[2],\n"
              "            ANGLEUNIT[\"degree\",0.0174532925199433,\n"
              "                ID[\"EPSG\",9122]]]]");
}

// ---------------------------------------------------------------------------

TEST(io, projparse_longlat_datum_NAD27) {
    auto obj =
        PROJStringParser().createFromPROJString("+proj=longlat +datum=NAD27");
    auto crs = nn_dynamic_pointer_cast<GeographicCRS>(obj);
    ASSERT_TRUE(crs != nullptr);
    WKTFormatterNNPtr f(WKTFormatter::create());
    crs->exportToWKT(f);
    EXPECT_EQ(f->toString(),
              "GEODCRS[\"unknown\",\n"
              "    DATUM[\"North American Datum 1927\",\n"
              "        ELLIPSOID[\"Clarke 1866\",6378206.4,294.978698213898,\n"
              "            LENGTHUNIT[\"metre\",1]],\n"
              "        ID[\"EPSG\",6267]],\n"
              "    PRIMEM[\"Greenwich\",0,\n"
              "        ANGLEUNIT[\"degree\",0.0174532925199433],\n"
              "        ID[\"EPSG\",8901]],\n"
              "    CS[ellipsoidal,2],\n"
              "        AXIS[\"longitude\",east,\n"
              "            ORDER[1],\n"
              "            ANGLEUNIT[\"degree\",0.0174532925199433,\n"
              "                ID[\"EPSG\",9122]]],\n"
              "        AXIS[\"latitude\",north,\n"
              "            ORDER[2],\n"
              "            ANGLEUNIT[\"degree\",0.0174532925199433,\n"
              "                ID[\"EPSG\",9122]]]]");
}

// ---------------------------------------------------------------------------

TEST(io, projparse_longlat_datum_other) {
    auto obj = PROJStringParser().createFromPROJString(
        "+proj=longlat +datum=carthage");
    auto crs = nn_dynamic_pointer_cast<GeographicCRS>(obj);
    ASSERT_TRUE(crs != nullptr);
    WKTFormatterNNPtr f(WKTFormatter::create());
    crs->exportToWKT(f);
    EXPECT_EQ(f->toString(),
              "GEODCRS[\"unknown\",\n"
              "    DATUM[\"Carthage\",\n"
              "        ELLIPSOID[\"Clarke 1880 (IGN)\",6378249.2,293.4660213,\n"
              "            LENGTHUNIT[\"metre\",1]],\n"
              "        ID[\"EPSG\",6223]],\n"
              "    PRIMEM[\"Greenwich\",0,\n"
              "        ANGLEUNIT[\"degree\",0.0174532925199433],\n"
              "        ID[\"EPSG\",8901]],\n"
              "    CS[ellipsoidal,2],\n"
              "        AXIS[\"longitude\",east,\n"
              "            ORDER[1],\n"
              "            ANGLEUNIT[\"degree\",0.0174532925199433,\n"
              "                ID[\"EPSG\",9122]]],\n"
              "        AXIS[\"latitude\",north,\n"
              "            ORDER[2],\n"
              "            ANGLEUNIT[\"degree\",0.0174532925199433,\n"
              "                ID[\"EPSG\",9122]]]]");
}

// ---------------------------------------------------------------------------

TEST(io, projparse_longlat_ellps_WGS84) {
    auto obj =
        PROJStringParser().createFromPROJString("+proj=longlat +ellps=WGS84");
    auto crs = nn_dynamic_pointer_cast<GeographicCRS>(obj);
    ASSERT_TRUE(crs != nullptr);
    WKTFormatterNNPtr f(WKTFormatter::create());
    f->simulCurNodeHasId();
    crs->exportToWKT(f);
    auto expected = "GEODCRS[\"unknown\",\n"
                    "    DATUM[\"Unknown based on WGS84 ellipsoid\",\n"
                    "        ELLIPSOID[\"WGS 84\",6378137,298.257223563,\n"
                    "            LENGTHUNIT[\"metre\",1]]],\n"
                    "    PRIMEM[\"Greenwich\",0,\n"
                    "        ANGLEUNIT[\"degree\",0.0174532925199433]],\n"
                    "    CS[ellipsoidal,2],\n"
                    "        AXIS[\"longitude\",east,\n"
                    "            ORDER[1],\n"
                    "            ANGLEUNIT[\"degree\",0.0174532925199433]],\n"
                    "        AXIS[\"latitude\",north,\n"
                    "            ORDER[2],\n"
                    "            ANGLEUNIT[\"degree\",0.0174532925199433]]]";
    EXPECT_EQ(f->toString(), expected);
}

// ---------------------------------------------------------------------------

TEST(io, projparse_longlat_ellps_GRS80) {
    auto obj =
        PROJStringParser().createFromPROJString("+proj=longlat +ellps=GRS80");
    auto crs = nn_dynamic_pointer_cast<GeographicCRS>(obj);
    ASSERT_TRUE(crs != nullptr);
    WKTFormatterNNPtr f(WKTFormatter::create());
    f->simulCurNodeHasId();
    crs->exportToWKT(f);
    auto expected = "GEODCRS[\"unknown\",\n"
                    "    DATUM[\"Unknown based on GRS80 ellipsoid\",\n"
                    "        ELLIPSOID[\"GRS 1980\",6378137,298.257222101,\n"
                    "            LENGTHUNIT[\"metre\",1]]],\n"
                    "    PRIMEM[\"Greenwich\",0,\n"
                    "        ANGLEUNIT[\"degree\",0.0174532925199433]],\n"
                    "    CS[ellipsoidal,2],\n"
                    "        AXIS[\"longitude\",east,\n"
                    "            ORDER[1],\n"
                    "            ANGLEUNIT[\"degree\",0.0174532925199433]],\n"
                    "        AXIS[\"latitude\",north,\n"
                    "            ORDER[2],\n"
                    "            ANGLEUNIT[\"degree\",0.0174532925199433]]]";
    EXPECT_EQ(f->toString(), expected);
}

// ---------------------------------------------------------------------------

TEST(io, projparse_longlat_a_b) {
    auto obj =
        PROJStringParser().createFromPROJString("+proj=longlat +a=2 +b=1.5");
    auto crs = nn_dynamic_pointer_cast<GeographicCRS>(obj);
    ASSERT_TRUE(crs != nullptr);
    WKTFormatterNNPtr f(WKTFormatter::create());
    f->simulCurNodeHasId();
    crs->exportToWKT(f);
    auto expected = "GEODCRS[\"unknown\",\n"
                    "    DATUM[\"unknown\",\n"
                    "        ELLIPSOID[\"unknown\",2,4,\n"
                    "            LENGTHUNIT[\"metre\",1]]],\n"
                    "    PRIMEM[\"Greenwich\",0,\n"
                    "        ANGLEUNIT[\"degree\",0.0174532925199433]],\n"
                    "    CS[ellipsoidal,2],\n"
                    "        AXIS[\"longitude\",east,\n"
                    "            ORDER[1],\n"
                    "            ANGLEUNIT[\"degree\",0.0174532925199433]],\n"
                    "        AXIS[\"latitude\",north,\n"
                    "            ORDER[2],\n"
                    "            ANGLEUNIT[\"degree\",0.0174532925199433]]]";
    EXPECT_EQ(f->toString(), expected);
}

// ---------------------------------------------------------------------------

TEST(io, projparse_longlat_a_rf_WGS84) {
    auto obj = PROJStringParser().createFromPROJString(
        "+proj=longlat +a=6378137 +rf=298.257223563");
    auto crs = nn_dynamic_pointer_cast<GeographicCRS>(obj);
    ASSERT_TRUE(crs != nullptr);
    WKTFormatterNNPtr f(WKTFormatter::create());
    f->simulCurNodeHasId();
    crs->exportToWKT(f);
    auto expected = "GEODCRS[\"unknown\",\n"
                    "    DATUM[\"unknown\",\n"
                    "        ELLIPSOID[\"WGS 84\",6378137,298.257223563,\n"
                    "            LENGTHUNIT[\"metre\",1]]],\n"
                    "    PRIMEM[\"Greenwich\",0,\n"
                    "        ANGLEUNIT[\"degree\",0.0174532925199433]],\n"
                    "    CS[ellipsoidal,2],\n"
                    "        AXIS[\"longitude\",east,\n"
                    "            ORDER[1],\n"
                    "            ANGLEUNIT[\"degree\",0.0174532925199433]],\n"
                    "        AXIS[\"latitude\",north,\n"
                    "            ORDER[2],\n"
                    "            ANGLEUNIT[\"degree\",0.0174532925199433]]]";
    EXPECT_EQ(f->toString(), expected);
}

// ---------------------------------------------------------------------------

TEST(io, projparse_longlat_a_rf) {
    auto obj =
        PROJStringParser().createFromPROJString("+proj=longlat +a=2 +rf=4");
    auto crs = nn_dynamic_pointer_cast<GeographicCRS>(obj);
    ASSERT_TRUE(crs != nullptr);
    WKTFormatterNNPtr f(WKTFormatter::create());
    f->simulCurNodeHasId();
    crs->exportToWKT(f);
    auto expected = "GEODCRS[\"unknown\",\n"
                    "    DATUM[\"unknown\",\n"
                    "        ELLIPSOID[\"unknown\",2,4,\n"
                    "            LENGTHUNIT[\"metre\",1]]],\n"
                    "    PRIMEM[\"Greenwich\",0,\n"
                    "        ANGLEUNIT[\"degree\",0.0174532925199433]],\n"
                    "    CS[ellipsoidal,2],\n"
                    "        AXIS[\"longitude\",east,\n"
                    "            ORDER[1],\n"
                    "            ANGLEUNIT[\"degree\",0.0174532925199433]],\n"
                    "        AXIS[\"latitude\",north,\n"
                    "            ORDER[2],\n"
                    "            ANGLEUNIT[\"degree\",0.0174532925199433]]]";
    EXPECT_EQ(f->toString(), expected);
}

// ---------------------------------------------------------------------------

TEST(io, projparse_longlat_R) {
    auto obj = PROJStringParser().createFromPROJString("+proj=longlat +R=2");
    auto crs = nn_dynamic_pointer_cast<GeographicCRS>(obj);
    ASSERT_TRUE(crs != nullptr);
    WKTFormatterNNPtr f(WKTFormatter::create());
    f->simulCurNodeHasId();
    crs->exportToWKT(f);
    auto expected = "GEODCRS[\"unknown\",\n"
                    "    DATUM[\"unknown\",\n"
                    "        ELLIPSOID[\"unknown\",2,0,\n"
                    "            LENGTHUNIT[\"metre\",1]]],\n"
                    "    PRIMEM[\"Greenwich\",0,\n"
                    "        ANGLEUNIT[\"degree\",0.0174532925199433]],\n"
                    "    CS[ellipsoidal,2],\n"
                    "        AXIS[\"longitude\",east,\n"
                    "            ORDER[1],\n"
                    "            ANGLEUNIT[\"degree\",0.0174532925199433]],\n"
                    "        AXIS[\"latitude\",north,\n"
                    "            ORDER[2],\n"
                    "            ANGLEUNIT[\"degree\",0.0174532925199433]]]";
    EXPECT_EQ(f->toString(), expected);
}

// ---------------------------------------------------------------------------

TEST(io, projparse_longlat_pm_paris) {
    auto obj =
        PROJStringParser().createFromPROJString("+proj=longlat +pm=paris");
    auto crs = nn_dynamic_pointer_cast<GeographicCRS>(obj);
    ASSERT_TRUE(crs != nullptr);
    WKTFormatterNNPtr f(WKTFormatter::create());
    f->simulCurNodeHasId();
    crs->exportToWKT(f);
    auto expected = "GEODCRS[\"unknown\",\n"
                    "    DATUM[\"Unknown based on WGS84 ellipsoid\",\n"
                    "        ELLIPSOID[\"WGS 84\",6378137,298.257223563,\n"
                    "            LENGTHUNIT[\"metre\",1]]],\n"
                    "    PRIMEM[\"Paris\",2.5969213,\n"
                    "        ANGLEUNIT[\"grad\",0.015707963267949]],\n"
                    "    CS[ellipsoidal,2],\n"
                    "        AXIS[\"longitude\",east,\n"
                    "            ORDER[1],\n"
                    "            ANGLEUNIT[\"degree\",0.0174532925199433]],\n"
                    "        AXIS[\"latitude\",north,\n"
                    "            ORDER[2],\n"
                    "            ANGLEUNIT[\"degree\",0.0174532925199433]]]";

    EXPECT_EQ(f->toString(), expected);
}

// ---------------------------------------------------------------------------

TEST(io, projparse_longlat_pm_ferro) {
    auto obj = PROJStringParser().createFromPROJString(
        "+proj=longlat +ellps=bessel +pm=ferro");
    auto crs = nn_dynamic_pointer_cast<GeographicCRS>(obj);
    ASSERT_TRUE(crs != nullptr);
    WKTFormatterNNPtr f(WKTFormatter::create());
    f->simulCurNodeHasId();
    crs->exportToWKT(f);
    auto expected =
        "GEODCRS[\"unknown\",\n"
        "    DATUM[\"Unknown based on Bessel 1841 ellipsoid\",\n"
        "        ELLIPSOID[\"Bessel 1841\",6377397.155,299.1528128,\n"
        "            LENGTHUNIT[\"metre\",1]]],\n"
        "    PRIMEM[\"Ferro\",-17.6666666666667,\n"
        "        ANGLEUNIT[\"degree\",0.0174532925199433]],\n"
        "    CS[ellipsoidal,2],\n"
        "        AXIS[\"longitude\",east,\n"
        "            ORDER[1],\n"
        "            ANGLEUNIT[\"degree\",0.0174532925199433]],\n"
        "        AXIS[\"latitude\",north,\n"
        "            ORDER[2],\n"
        "            ANGLEUNIT[\"degree\",0.0174532925199433]]]";

    EXPECT_EQ(f->toString(), expected);
}

// ---------------------------------------------------------------------------

TEST(io, projparse_longlat_pm_numeric) {
    auto obj = PROJStringParser().createFromPROJString("+proj=longlat +pm=2.5");
    auto crs = nn_dynamic_pointer_cast<GeographicCRS>(obj);
    ASSERT_TRUE(crs != nullptr);
    WKTFormatterNNPtr f(WKTFormatter::create());
    f->simulCurNodeHasId();
    crs->exportToWKT(f);
    auto expected = "GEODCRS[\"unknown\",\n"
                    "    DATUM[\"Unknown based on WGS84 ellipsoid\",\n"
                    "        ELLIPSOID[\"WGS 84\",6378137,298.257223563,\n"
                    "            LENGTHUNIT[\"metre\",1]]],\n"
                    "    PRIMEM[\"unknown\",2.5,\n"
                    "        ANGLEUNIT[\"degree\",0.0174532925199433]],\n"
                    "    CS[ellipsoidal,2],\n"
                    "        AXIS[\"longitude\",east,\n"
                    "            ORDER[1],\n"
                    "            ANGLEUNIT[\"degree\",0.0174532925199433]],\n"
                    "        AXIS[\"latitude\",north,\n"
                    "            ORDER[2],\n"
                    "            ANGLEUNIT[\"degree\",0.0174532925199433]]]";

    EXPECT_EQ(f->toString(), expected);
}

// ---------------------------------------------------------------------------

TEST(io, projparse_longlat_towgs84_3_terms) {
    auto obj = PROJStringParser().createFromPROJString(
        "+proj=longlat +ellps=GRS80 +towgs84=1.2,2,3");
    auto crs = nn_dynamic_pointer_cast<BoundCRS>(obj);
    ASSERT_TRUE(crs != nullptr);
    WKTFormatterNNPtr f(WKTFormatter::create());
    f->simulCurNodeHasId();
    crs->exportToWKT(f);

    auto wkt = f->toString();
    EXPECT_TRUE(wkt.find("METHOD[\"Geocentric translations") !=
                std::string::npos)
        << wkt;
    EXPECT_TRUE(wkt.find("PARAMETER[\"X-axis translation\",1.2") !=
                std::string::npos)
        << wkt;
    EXPECT_TRUE(wkt.find("PARAMETER[\"Y-axis translation\",2") !=
                std::string::npos)
        << wkt;
    EXPECT_TRUE(wkt.find("PARAMETER[\"Z-axis translation\",3") !=
                std::string::npos)
        << wkt;
}

// ---------------------------------------------------------------------------

TEST(io, projparse_longlat_towgs84_7_terms) {
    auto obj = PROJStringParser().createFromPROJString(
        "+proj=longlat +ellps=GRS80 +towgs84=1.2,2,3,4,5,6,7");
    auto crs = nn_dynamic_pointer_cast<BoundCRS>(obj);
    ASSERT_TRUE(crs != nullptr);
    WKTFormatterNNPtr f(WKTFormatter::create());
    f->simulCurNodeHasId();
    crs->exportToWKT(f);

    auto wkt = f->toString();
    EXPECT_TRUE(wkt.find("METHOD[\"Position Vector transformation") !=
                std::string::npos)
        << wkt;
    EXPECT_TRUE(wkt.find("PARAMETER[\"X-axis translation\",1.2") !=
                std::string::npos)
        << wkt;
    EXPECT_TRUE(wkt.find("PARAMETER[\"Y-axis translation\",2") !=
                std::string::npos)
        << wkt;
    EXPECT_TRUE(wkt.find("PARAMETER[\"Z-axis translation\",3") !=
                std::string::npos)
        << wkt;
    EXPECT_TRUE(wkt.find("PARAMETER[\"Scale difference\",1.000007") !=
                std::string::npos)
        << wkt;
}

// ---------------------------------------------------------------------------

TEST(io, projparse_errors) {
    EXPECT_THROW(PROJStringParser().createFromPROJString(""), ParsingException);

    EXPECT_THROW(PROJStringParser().createFromPROJString("foo"),
                 ParsingException);

    EXPECT_THROW(PROJStringParser().createFromPROJString("inv"),
                 ParsingException);

    EXPECT_THROW(PROJStringParser().createFromPROJString("step"),
                 ParsingException);

    EXPECT_THROW(PROJStringParser().createFromPROJString(
                     "proj=pipeline step proj=pipeline"),
                 ParsingException);
}

// ---------------------------------------------------------------------------

TEST(io, projparse_longlat_errors) {
    EXPECT_THROW(
        PROJStringParser().createFromPROJString("+proj=longlat +datum=unknown"),
        ParsingException);

    EXPECT_THROW(
        PROJStringParser().createFromPROJString("+proj=longlat +ellps=unknown"),
        ParsingException);

    EXPECT_THROW(PROJStringParser().createFromPROJString(
                     "+proj=longlat +a=invalid +b=1"),
                 ParsingException);

    EXPECT_THROW(PROJStringParser().createFromPROJString(
                     "+proj=longlat +a=1 +b=invalid"),
                 ParsingException);

    EXPECT_THROW(PROJStringParser().createFromPROJString(
                     "+proj=longlat +a=invalid +rf=1"),
                 ParsingException);

    EXPECT_THROW(PROJStringParser().createFromPROJString(
                     "+proj=longlat +a=1 +rf=invalid"),
                 ParsingException);

    EXPECT_THROW(
        PROJStringParser().createFromPROJString("+proj=longlat +R=invalid"),
        ParsingException);

    EXPECT_THROW(PROJStringParser().createFromPROJString("+proj=longlat +a=1"),
                 ParsingException);

    EXPECT_THROW(PROJStringParser().createFromPROJString("+proj=longlat +b=1"),
                 ParsingException);

    EXPECT_THROW(PROJStringParser().createFromPROJString("+proj=longlat +rf=1"),
                 ParsingException);

    EXPECT_THROW(
        PROJStringParser().createFromPROJString("+proj=longlat +pm=unknown"),
        ParsingException);

    EXPECT_THROW(
        PROJStringParser().createFromPROJString(
            "+proj=longlat +ellps=GRS80 +towgs84=1.2,2,3,4,5,6,invalid"),
        ParsingException);
}
