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

using namespace osgeo::proj::common;
using namespace osgeo::proj::crs;
using namespace osgeo::proj::cs;
using namespace osgeo::proj::datum;
using namespace osgeo::proj::io;
using namespace osgeo::proj::metadata;
using namespace osgeo::proj::operation;
using namespace osgeo::proj::util;

// ---------------------------------------------------------------------------

TEST(crs, EPSG_4326_get_components) {
    auto crs = GeographicCRS::EPSG_4326;
    ASSERT_EQ(crs->identifiers().size(), 1);
    EXPECT_EQ(crs->identifiers()[0]->code(), "4326");
    EXPECT_EQ(*(crs->identifiers()[0]->codeSpace()), "EPSG");
    EXPECT_EQ(*(crs->name()->description()), "WGS 84");

    auto datum = crs->datum();
    ASSERT_EQ(datum->identifiers().size(), 1);
    EXPECT_EQ(datum->identifiers()[0]->code(), "6326");
    EXPECT_EQ(*(datum->identifiers()[0]->codeSpace()), "EPSG");
    EXPECT_EQ(*(datum->name()->description()), "WGS_1984");

    auto ellipsoid = datum->ellipsoid();
    EXPECT_EQ(ellipsoid->semiMajorAxis().value(), 6378137.0);
    EXPECT_EQ(ellipsoid->semiMajorAxis().unit(), UnitOfMeasure::METRE);
    EXPECT_EQ(ellipsoid->inverseFlattening()->value(), 298.257223563);
    ASSERT_EQ(ellipsoid->identifiers().size(), 1);
    EXPECT_EQ(ellipsoid->identifiers()[0]->code(), "7030");
    EXPECT_EQ(*(ellipsoid->identifiers()[0]->codeSpace()), "EPSG");
    EXPECT_EQ(*(ellipsoid->name()->description()), "WGS 84");
}

// ---------------------------------------------------------------------------

TEST(crs, EPSG_4326_as_WKT2) {
    auto crs = GeographicCRS::EPSG_4326;
    WKTFormatterNNPtr f(WKTFormatter::create());
    crs->exportToWKT(f);
    EXPECT_EQ(f->toString(),
              "GEODCRS[\"WGS 84\",\n"
              "    DATUM[\"WGS_1984\",\n"
              "        ELLIPSOID[\"WGS 84\",6378137,298.257223563,\n"
              "            LENGTHUNIT[\"metre\",1]]],\n"
              "    PRIMEM[\"Greenwich\",0,\n"
              "        ANGLEUNIT[\"degree\",0.0174532925199433]],\n"
              "    CS[ellipsoidal,2],\n"
              "        AXIS[\"latitude\",north,\n"
              "            ORDER[1],\n"
              "            ANGLEUNIT[\"degree\",0.0174532925199433]],\n"
              "        AXIS[\"longitude\",east,\n"
              "            ORDER[2],\n"
              "            ANGLEUNIT[\"degree\",0.0174532925199433]],\n"
              "    ID[\"EPSG\",4326]]");
}

// ---------------------------------------------------------------------------

TEST(crs, EPSG_4326_as_WKT2_2018) {
    auto crs = GeographicCRS::EPSG_4326;
    WKTFormatterNNPtr f(
        WKTFormatter::create(WKTFormatter::Convention::WKT2_2018));
    crs->exportToWKT(f);
    EXPECT_EQ(f->toString(),
              "GEOGCRS[\"WGS 84\",\n"
              "    DATUM[\"WGS_1984\",\n"
              "        ELLIPSOID[\"WGS 84\",6378137,298.257223563,\n"
              "            LENGTHUNIT[\"metre\",1]]],\n"
              "    PRIMEM[\"Greenwich\",0,\n"
              "        ANGLEUNIT[\"degree\",0.0174532925199433]],\n"
              "    CS[ellipsoidal,2],\n"
              "        AXIS[\"latitude\",north,\n"
              "            ORDER[1],\n"
              "            ANGLEUNIT[\"degree\",0.0174532925199433]],\n"
              "        AXIS[\"longitude\",east,\n"
              "            ORDER[2],\n"
              "            ANGLEUNIT[\"degree\",0.0174532925199433]],\n"
              "    ID[\"EPSG\",4326]]");
}

// ---------------------------------------------------------------------------

TEST(crs, EPSG_4326_as_WKT2_SIMPLIFIED) {
    auto crs = GeographicCRS::EPSG_4326;
    WKTFormatterNNPtr f(
        WKTFormatter::create(WKTFormatter::Convention::WKT2_SIMPLIFIED));
    crs->exportToWKT(f);
    EXPECT_EQ(f->toString(),
              "GEODCRS[\"WGS 84\",\n"
              "    DATUM[\"WGS_1984\",\n"
              "        ELLIPSOID[\"WGS 84\",6378137,298.257223563]],\n"
              "    CS[ellipsoidal,2],\n"
              "        AXIS[\"latitude\",north],\n"
              "        AXIS[\"longitude\",east],\n"
              "        UNIT[\"degree\",0.0174532925199433],\n"
              "    ID[\"EPSG\",4326]]");
}

// ---------------------------------------------------------------------------

TEST(crs, EPSG_4326_as_WKT2_SIMPLIFIED_single_line) {
    auto crs = GeographicCRS::EPSG_4326;
    WKTFormatterNNPtr f(
        WKTFormatter::create(WKTFormatter::Convention::WKT2_SIMPLIFIED));
    f->setMultiLine(false);
    crs->exportToWKT(f);
    EXPECT_EQ(
        f->toString(),
        "GEODCRS[\"WGS 84\",DATUM[\"WGS_1984\",ELLIPSOID[\"WGS "
        "84\",6378137,298.257223563]],"
        "CS[ellipsoidal,2],AXIS[\"latitude\",north],AXIS[\"longitude\",east],"
        "UNIT[\"degree\",0.0174532925199433],ID[\"EPSG\",4326]]");
}

// ---------------------------------------------------------------------------

TEST(crs, EPSG_4326_as_WKT2_2018_SIMPLIFIED) {
    auto crs = GeographicCRS::EPSG_4326;
    WKTFormatterNNPtr f(
        WKTFormatter::create(WKTFormatter::Convention::WKT2_2018_SIMPLIFIED));
    crs->exportToWKT(f);
    EXPECT_EQ(f->toString(),
              "GEOGCRS[\"WGS 84\",\n"
              "    DATUM[\"WGS_1984\",\n"
              "        ELLIPSOID[\"WGS 84\",6378137,298.257223563]],\n"
              "    CS[ellipsoidal,2],\n"
              "        AXIS[\"latitude\",north],\n"
              "        AXIS[\"longitude\",east],\n"
              "        UNIT[\"degree\",0.0174532925199433],\n"
              "    ID[\"EPSG\",4326]]");
}

// ---------------------------------------------------------------------------

TEST(crs, EPSG_4326_as_WKT1_GDAL) {
    auto crs = GeographicCRS::EPSG_4326;
    WKTFormatterNNPtr f(
        WKTFormatter::create(WKTFormatter::Convention::WKT1_GDAL));
    crs->exportToWKT(f);
    EXPECT_EQ(f->toString(),
              "GEOGCS[\"WGS 84\",\n"
              "    DATUM[\"WGS_1984\",\n"
              "        SPHEROID[\"WGS 84\",6378137,298.257223563,\n"
              "            AUTHORITY[\"EPSG\",\"7030\"]],\n"
              "        AUTHORITY[\"EPSG\",\"6326\"]],\n"
              "    PRIMEM[\"Greenwich\",0,\n"
              "        AUTHORITY[\"EPSG\",\"8901\"]],\n"
              "    UNIT[\"degree\",0.0174532925199433,\n"
              "        AUTHORITY[\"EPSG\",9122]],\n"
              "    AXIS[\"Latitude\",NORTH],\n"
              "    AXIS[\"Longitude\",EAST],\n"
              "    AUTHORITY[\"EPSG\",\"4326\"]]");
}

// ---------------------------------------------------------------------------

TEST(crs, EPSG_4979_as_WKT2_SIMPLIFIED) {
    auto crs = GeographicCRS::EPSG_4979;
    WKTFormatterNNPtr f(
        WKTFormatter::create(WKTFormatter::Convention::WKT2_SIMPLIFIED));
    crs->exportToWKT(f);
    EXPECT_EQ(f->toString(),
              "GEODCRS[\"WGS 84\",\n"
              "    DATUM[\"WGS_1984\",\n"
              "        ELLIPSOID[\"WGS 84\",6378137,298.257223563]],\n"
              "    CS[ellipsoidal,3],\n"
              "        AXIS[\"latitude\",north,\n"
              "            UNIT[\"degree\",0.0174532925199433]],\n"
              "        AXIS[\"longitude\",east,\n"
              "            UNIT[\"degree\",0.0174532925199433]],\n"
              "        AXIS[\"ellipsoidal height\",up,\n"
              "            UNIT[\"metre\",1]],\n"
              "    ID[\"EPSG\",4979]]");
}

// ---------------------------------------------------------------------------

TEST(crs, EPSG_4979_as_WKT2_2018_SIMPLIFIED) {
    auto crs = GeographicCRS::EPSG_4979;
    WKTFormatterNNPtr f(
        WKTFormatter::create(WKTFormatter::Convention::WKT2_2018_SIMPLIFIED));
    crs->exportToWKT(f);
    EXPECT_EQ(f->toString(),
              "GEOGCRS[\"WGS 84\",\n"
              "    DATUM[\"WGS_1984\",\n"
              "        ELLIPSOID[\"WGS 84\",6378137,298.257223563]],\n"
              "    CS[ellipsoidal,3],\n"
              "        AXIS[\"latitude\",north,\n"
              "            UNIT[\"degree\",0.0174532925199433]],\n"
              "        AXIS[\"longitude\",east,\n"
              "            UNIT[\"degree\",0.0174532925199433]],\n"
              "        AXIS[\"ellipsoidal height\",up,\n"
              "            UNIT[\"metre\",1]],\n"
              "    ID[\"EPSG\",4979]]");
}

// ---------------------------------------------------------------------------

TEST(crs, EPSG_4979_as_WKT1_GDAL) {
    auto crs = GeographicCRS::EPSG_4979;
    WKTFormatterNNPtr f(
        WKTFormatter::create(WKTFormatter::Convention::WKT1_GDAL));
    crs->exportToWKT(f);
    // FIXME? WKT1 only supports 2 axis for GEOGCS. So this is an extension of
    // WKT1 as it
    // and GDAL doesn't really export such as beast, although it can import it
    EXPECT_EQ(f->toString(),
              "GEOGCS[\"WGS 84\",\n"
              "    DATUM[\"WGS_1984\",\n"
              "        SPHEROID[\"WGS 84\",6378137,298.257223563,\n"
              "            AUTHORITY[\"EPSG\",\"7030\"]],\n"
              "        AUTHORITY[\"EPSG\",\"6326\"]],\n"
              "    PRIMEM[\"Greenwich\",0,\n"
              "        AUTHORITY[\"EPSG\",\"8901\"]],\n"
              "    UNIT[\"degree\",0.0174532925199433,\n"
              "        AUTHORITY[\"EPSG\",9122]],\n"
              "    AXIS[\"Latitude\",NORTH],\n"
              "    AXIS[\"Longitude\",EAST],\n"
              "    AXIS[\"Ellipsoidal height\",UP],\n"
              "    AUTHORITY[\"EPSG\",\"4979\"]]");
}

// ---------------------------------------------------------------------------

TEST(crs, EPSG_4807_as_WKT2) {
    auto crs = GeographicCRS::EPSG_4807;
    WKTFormatterNNPtr f(WKTFormatter::create(WKTFormatter::Convention::WKT2));
    crs->exportToWKT(f);
    EXPECT_EQ(
        f->toString(),
        "GEODCRS[\"NTF (Paris)\",\n"
        "    DATUM[\"Nouvelle_Triangulation_Francaise_Paris\",\n"
        "        ELLIPSOID[\"Clarke 1880 (IGN)\",6378249.2,293.466021293627,\n"
        "            LENGTHUNIT[\"metre\",1]]],\n"
        "    PRIMEM[\"Paris\",2.5969213,\n"
        "        ANGLEUNIT[\"grad\",0.015707963267949]],\n"
        "    CS[ellipsoidal,2],\n"
        "        AXIS[\"latitude\",north,\n"
        "            ORDER[1],\n"
        "            ANGLEUNIT[\"grad\",0.015707963267949]],\n"
        "        AXIS[\"longitude\",east,\n"
        "            ORDER[2],\n"
        "            ANGLEUNIT[\"grad\",0.015707963267949]],\n"
        "    ID[\"EPSG\",4807]]");
}

// ---------------------------------------------------------------------------

TEST(crs, EPSG_4807_as_WKT2_SIMPLIFIED) {
    auto crs = GeographicCRS::EPSG_4807;
    WKTFormatterNNPtr f(
        WKTFormatter::create(WKTFormatter::Convention::WKT2_SIMPLIFIED));
    crs->exportToWKT(f);
    EXPECT_EQ(f->toString(),
              "GEODCRS[\"NTF (Paris)\",\n"
              "    DATUM[\"Nouvelle_Triangulation_Francaise_Paris\",\n"
              "        ELLIPSOID[\"Clarke 1880 "
              "(IGN)\",6378249.2,293.466021293627]],\n"
              "    PRIMEM[\"Paris\",2.5969213],\n"
              "    CS[ellipsoidal,2],\n"
              "        AXIS[\"latitude\",north],\n"
              "        AXIS[\"longitude\",east],\n"
              "        UNIT[\"grad\",0.015707963267949],\n"
              "    ID[\"EPSG\",4807]]");
}

// ---------------------------------------------------------------------------

TEST(crs, EPSG_4807_as_WKT1_GDAL) {
    auto crs = GeographicCRS::EPSG_4807;
    WKTFormatterNNPtr f(
        WKTFormatter::create(WKTFormatter::Convention::WKT1_GDAL));
    crs->exportToWKT(f);
    EXPECT_EQ(
        f->toString(),
        "GEOGCS[\"NTF (Paris)\",\n"
        "    DATUM[\"Nouvelle_Triangulation_Francaise_Paris\",\n"
        "        SPHEROID[\"Clarke 1880 (IGN)\",6378249.2,293.466021293627,\n"
        "            AUTHORITY[\"EPSG\",\"6807\"]],\n"
        "        AUTHORITY[\"EPSG\",\"6807\"]],\n"
        "    PRIMEM[\"Paris\",2.33722917,\n" /* WKT1_GDAL weirdness: PRIMEM is
                                                converted to degree */
        "        AUTHORITY[\"EPSG\",\"8903\"]],\n"
        "    UNIT[\"grad\",0.015707963267949,\n"
        "        AUTHORITY[\"EPSG\",9105]],\n"
        "    AXIS[\"Latitude\",NORTH],\n"
        "    AXIS[\"Longitude\",EAST],\n"
        "    AUTHORITY[\"EPSG\",\"4807\"]]");
}

// ---------------------------------------------------------------------------

static GeodeticCRSNNPtr createGeocentric() {
    PropertyMap propertiesCRS;
    propertiesCRS.set(Identifier::CODESPACE_KEY, "EPSG")
        .set(Identifier::CODE_KEY, 4328)
        .set(IdentifiedObject::NAME_KEY, "WGS 84");
    return GeodeticCRS::create(
        propertiesCRS, GeodeticReferenceFrame::EPSG_6326,
        CartesianCS::createGeocentric(UnitOfMeasure::METRE));
}

// ---------------------------------------------------------------------------

TEST(crs, geocentricCRS_as_WKT2) {
    auto crs = createGeocentric();

    auto expected = "GEODCRS[\"WGS 84\",\n"
                    "    DATUM[\"WGS_1984\",\n"
                    "        ELLIPSOID[\"WGS 84\",6378137,298.257223563,\n"
                    "            LENGTHUNIT[\"metre\",1]]],\n"
                    "    PRIMEM[\"Greenwich\",0,\n"
                    "        ANGLEUNIT[\"degree\",0.0174532925199433]],\n"
                    "    CS[Cartesian,3],\n"
                    "        AXIS[\"(X)\",geocentricX,\n"
                    "            ORDER[1],\n"
                    "            LENGTHUNIT[\"metre\",1]],\n"
                    "        AXIS[\"(Y)\",geocentricY,\n"
                    "            ORDER[2],\n"
                    "            LENGTHUNIT[\"metre\",1]],\n"
                    "        AXIS[\"(Z)\",geocentricZ,\n"
                    "            ORDER[3],\n"
                    "            LENGTHUNIT[\"metre\",1]],\n"
                    "    ID[\"EPSG\",4328]]";

    EXPECT_EQ(crs->exportToWKT(WKTFormatter::create()), expected);
    EXPECT_EQ(crs->exportToWKT(
                  WKTFormatter::create(WKTFormatter::Convention::WKT2_2018)),
              expected);
}

// ---------------------------------------------------------------------------

TEST(crs, geocentricCRS_as_WKT2_simplified) {
    auto crs = createGeocentric();

    auto expected = "GEODCRS[\"WGS 84\",\n"
                    "    DATUM[\"WGS_1984\",\n"
                    "        ELLIPSOID[\"WGS 84\",6378137,298.257223563]],\n"
                    "    CS[Cartesian,3],\n"
                    "        AXIS[\"(X)\",geocentricX],\n"
                    "        AXIS[\"(Y)\",geocentricY],\n"
                    "        AXIS[\"(Z)\",geocentricZ],\n"
                    "        UNIT[\"metre\",1],\n"
                    "    ID[\"EPSG\",4328]]";

    EXPECT_EQ(crs->exportToWKT(WKTFormatter::create(
                  WKTFormatter::Convention::WKT2_SIMPLIFIED)),
              expected);
}

// ---------------------------------------------------------------------------

TEST(crs, geocentricCRS_as_WKT1_GDAL) {
    auto crs = createGeocentric();
    WKTFormatterNNPtr f(
        WKTFormatter::create(WKTFormatter::Convention::WKT1_GDAL));
    crs->exportToWKT(f);
    EXPECT_EQ(f->toString(),
              "GEOCCS[\"WGS 84\",\n"
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
              "    AUTHORITY[\"EPSG\",\"4328\"]]");
}

// ---------------------------------------------------------------------------

static ProjectedCRSNNPtr createProjected() {
    PropertyMap propertiesCRS;
    propertiesCRS.set(Identifier::CODESPACE_KEY, "EPSG")
        .set(Identifier::CODE_KEY, 32631)
        .set(IdentifiedObject::NAME_KEY, "WGS 84 / UTM zone 31N");
    return ProjectedCRS::create(
        propertiesCRS, GeographicCRS::EPSG_4326,
        Conversion::createUTM(31, true),
        CartesianCS::createEastingNorthing(UnitOfMeasure::METRE));
}

// ---------------------------------------------------------------------------

TEST(crs, projectedCRS_as_WKT2) {
    auto crs = createProjected();

    auto expected =
        "PROJCRS[\"WGS 84 / UTM zone 31N\",\n"
        "    BASEGEODCRS[\"WGS 84\",\n"
        "        DATUM[\"WGS_1984\",\n"
        "            ELLIPSOID[\"WGS 84\",6378137,298.257223563,\n"
        "                LENGTHUNIT[\"metre\",1]]],\n"
        "        PRIMEM[\"Greenwich\",0,\n"
        "            ANGLEUNIT[\"degree\",0.0174532925199433]]],\n"
        "    CONVERSION[\"UTM zone 31N\",\n"
        "        METHOD[\"Transverse Mercator\",\n"
        "            ID[\"EPSG\",9807]],\n"
        "        PARAMETER[\"Latitude of natural origin\",0,\n"
        "            ANGLEUNIT[\"degree\",0.0174532925199433],\n"
        "            ID[\"EPSG\",8801]],\n"
        "        PARAMETER[\"Longitude of natural origin\",3,\n"
        "            ANGLEUNIT[\"degree\",0.0174532925199433],\n"
        "            ID[\"EPSG\",8802]],\n"
        "        PARAMETER[\"Scale factor at natural origin\",0.9996,\n"
        "            SCALEUNIT[\"unity\",1],\n"
        "            ID[\"EPSG\",8805]],\n"
        "        PARAMETER[\"False easting\",500000,\n"
        "            LENGTHUNIT[\"metre\",1],\n"
        "            ID[\"EPSG\",8806]],\n"
        "        PARAMETER[\"False northing\",0,\n"
        "            LENGTHUNIT[\"metre\",1],\n"
        "            ID[\"EPSG\",8807]]],\n"
        "    CS[Cartesian,2],\n"
        "        AXIS[\"(E)\",east,\n"
        "            ORDER[1],\n"
        "            LENGTHUNIT[\"metre\",1]],\n"
        "        AXIS[\"(N)\",north,\n"
        "            ORDER[2],\n"
        "            LENGTHUNIT[\"metre\",1]],\n"
        "    ID[\"EPSG\",32631]]";

    EXPECT_EQ(crs->exportToWKT(WKTFormatter::create()), expected);
}

// ---------------------------------------------------------------------------

TEST(crs, projectedCRS_as_WKT2_simplified) {
    auto crs = createProjected();

    auto expected =
        "PROJCRS[\"WGS 84 / UTM zone 31N\",\n"
        "    BASEGEODCRS[\"WGS 84\",\n"
        "        DATUM[\"WGS_1984\",\n"
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

    EXPECT_EQ(crs->exportToWKT(WKTFormatter::create(
                  WKTFormatter::Convention::WKT2_SIMPLIFIED)),
              expected);
}

// ---------------------------------------------------------------------------

TEST(crs, projectedCRS_as_WKT2_2018_simplified) {
    auto crs = createProjected();

    auto expected =
        "PROJCRS[\"WGS 84 / UTM zone 31N\",\n"
        "    BASEGEOGCRS[\"WGS 84\",\n"
        "        DATUM[\"WGS_1984\",\n"
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

    EXPECT_EQ(crs->exportToWKT(WKTFormatter::create(
                  WKTFormatter::Convention::WKT2_2018_SIMPLIFIED)),
              expected);
}

// ---------------------------------------------------------------------------

TEST(crs, projectedCRS_as_WKT1_GDAL) {
    auto crs = createProjected();

    auto expected = "PROJCS[\"WGS 84 / UTM zone 31N\",\n"
                    "    GEOGCS[\"WGS 84\",\n"
                    "        DATUM[\"WGS_1984\",\n"
                    "            SPHEROID[\"WGS 84\",6378137,298.257223563,\n"
                    "                AUTHORITY[\"EPSG\",\"7030\"]],\n"
                    "            AUTHORITY[\"EPSG\",\"6326\"]],\n"
                    "        PRIMEM[\"Greenwich\",0,\n"
                    "            AUTHORITY[\"EPSG\",\"8901\"]],\n"
                    "        UNIT[\"degree\",0.0174532925199433,\n"
                    "            AUTHORITY[\"EPSG\",9122]],\n"
                    "        AXIS[\"Latitude\",NORTH],\n"
                    "        AXIS[\"Longitude\",EAST],\n"
                    "        AUTHORITY[\"EPSG\",\"4326\"]],\n"
                    "    PROJECTION[\"Transverse_Mercator\"],\n"
                    "    PARAMETER[\"latitude_of_origin\",0],\n"
                    "    PARAMETER[\"central_meridian\",3],\n"
                    "    PARAMETER[\"scale_factor\",0.9996],\n"
                    "    PARAMETER[\"false_easting\",500000],\n"
                    "    PARAMETER[\"false_northing\",0],\n"
                    "    UNIT[\"metre\",1,\n"
                    "        AUTHORITY[\"EPSG\",9001]],\n"
                    "    AXIS[\"Easting\",EAST],\n"
                    "    AXIS[\"Northing\",NORTH],\n"
                    "    AUTHORITY[\"EPSG\",\"32631\"]]";

    EXPECT_EQ(crs->exportToWKT(
                  WKTFormatter::create(WKTFormatter::Convention::WKT1_GDAL)),
              expected);
}

// ---------------------------------------------------------------------------

TEST(datum, datum_with_ANCHOR) {
    auto datum = GeodeticReferenceFrame::create(
        PropertyMap().set(IdentifiedObject::NAME_KEY, "WGS_1984 with anchor"),
        Ellipsoid::EPSG_7030, optional<std::string>("My anchor"),
        PrimeMeridian::GREENWICH);

    auto expected = "DATUM[\"WGS_1984 with anchor\",\n"
                    "    ELLIPSOID[\"WGS 84\",6378137,298.257223563,\n"
                    "        LENGTHUNIT[\"metre\",1]],\n"
                    "    ANCHOR[\"My anchor\"]]";

    EXPECT_EQ(datum->exportToWKT(WKTFormatter::create()), expected);
}

// ---------------------------------------------------------------------------

TEST(datum, cs_with_MERIDIAN) {
    std::vector<CoordinateSystemAxisNNPtr> axis{
        CoordinateSystemAxis::create(
            PropertyMap().set(IdentifiedObject::NAME_KEY, "Easting"), "X",
            AxisDirection::SOUTH, UnitOfMeasure::METRE,
            Meridian::create(Angle(90.0))),
        CoordinateSystemAxis::create(
            PropertyMap().set(IdentifiedObject::NAME_KEY, "Northing"), "Y",
            AxisDirection::SOUTH, UnitOfMeasure::METRE,
            Meridian::create(Angle(180.0)))};
    auto cs(CartesianCS::create(PropertyMap(), axis[0], axis[1]));

    auto expected = "CS[Cartesian,2]\n"
                    "    AXIS[\"easting (X)\",south,\n"
                    "        MERIDIAN[90,\n"
                    "            ANGLEUNIT[\"degree\",0.0174532925199433]],\n"
                    "        ORDER[1],\n"
                    "        LENGTHUNIT[\"metre\",1]],\n"
                    "    AXIS[\"northing (Y)\",south,\n"
                    "        MERIDIAN[180,\n"
                    "            ANGLEUNIT[\"degree\",0.0174532925199433]],\n"
                    "        ORDER[2],\n"
                    "        LENGTHUNIT[\"metre\",1]]";

    EXPECT_EQ(cs->exportToWKT(WKTFormatter::create()), expected);
}

// ---------------------------------------------------------------------------

TEST(crs, scope_area_bbox_remark) {
    auto in_wkt = "GEODETICCRS[\"JGD2000\","
                  "DATUM[\"Japanese Geodetic Datum 2000\","
                  "  ELLIPSOID[\"GRS 1980\",6378137,298.257222101]],"
                  "CS[Cartesian,3],"
                  "  AXIS[\"(X)\",geocentricX],"
                  "  AXIS[\"(Y)\",geocentricY],"
                  "  AXIS[\"(Z)\",geocentricZ],"
                  "  LENGTHUNIT[\"metre\",1.0],"
                  "SCOPE[\"Geodesy, topographic mapping and cadastre\"],"
                  "AREA[\"Japan\"],"
                  "BBOX[17.09,122.38,46.05,157.64],"
                  "TIMEEXTENT[2002-04-01,2011-10-21]," // TODO TIMEEXTENT
                  "REMARK[\"some_remark\"]]";
    auto crs =
        nn_dynamic_pointer_cast<GeodeticCRS>(WKTParser().createFromWKT(in_wkt));
    ASSERT_TRUE(crs != nullptr);

    ASSERT_EQ(crs->domains().size(), 1);
    auto domain = crs->domains()[0];
    EXPECT_TRUE(domain->scope().has_value());
    EXPECT_EQ(*(domain->scope()), "Geodesy, topographic mapping and cadastre");
    ASSERT_TRUE(domain->domainOfValidity() != nullptr);
    EXPECT_TRUE(domain->domainOfValidity()->description().has_value());
    EXPECT_EQ(*(domain->domainOfValidity()->description()), "Japan");
    ASSERT_EQ(domain->domainOfValidity()->geographicElements().size(), 1);
    auto geogElement = domain->domainOfValidity()->geographicElements()[0];
    auto bbox = nn_dynamic_pointer_cast<GeographicBoundingBox>(geogElement);
    ASSERT_TRUE(bbox != nullptr);
    EXPECT_EQ(bbox->southBoundLongitude(), 17.09);
    EXPECT_EQ(bbox->westBoundLongitude(), 122.38);
    EXPECT_EQ(bbox->northBoundLongitude(), 46.05);
    EXPECT_EQ(bbox->eastBoundLongitude(), 157.64);

    auto got_wkt = crs->exportToWKT(WKTFormatter::create());
    auto expected =
        "GEODCRS[\"JGD2000\",\n"
        "    DATUM[\"Japanese Geodetic Datum 2000\",\n"
        "        ELLIPSOID[\"GRS 1980\",6378137,298.257222101,\n"
        "            LENGTHUNIT[\"metre\",1]]],\n"
        "    PRIMEM[\"Greenwich\",0,\n"
        "        ANGLEUNIT[\"degree\",0.0174532925199433]],\n"
        "    CS[Cartesian,3],\n"
        "        AXIS[\"(X)\",geocentricX,\n"
        "            ORDER[1],\n"
        "            LENGTHUNIT[\"metre\",1]],\n"
        "        AXIS[\"(Y)\",geocentricY,\n"
        "            ORDER[2],\n"
        "            LENGTHUNIT[\"metre\",1]],\n"
        "        AXIS[\"(Z)\",geocentricZ,\n"
        "            ORDER[3],\n"
        "            LENGTHUNIT[\"metre\",1]],\n"
        "    SCOPE[\"Geodesy, topographic mapping and cadastre\"],\n"
        "    AREA[\"Japan\"],\n"
        "    BBOX[17.09,122.38,46.05,157.64],\n"
        "    REMARK[\"some_remark\"]]";

    EXPECT_EQ(got_wkt, expected);
}

// ---------------------------------------------------------------------------

TEST(crs, multiple_ID) {

    PropertyMap propertiesCRS;
    propertiesCRS.set(IdentifiedObject::NAME_KEY, "WGS 84");
    auto identifiers = ArrayOfBaseObject::create();
    identifiers->values.push_back(Identifier::create(
        "codeA", PropertyMap().set(Identifier::CODESPACE_KEY, "authorityA")));
    identifiers->values.push_back(Identifier::create(
        "codeB", PropertyMap().set(Identifier::CODESPACE_KEY, "authorityB")));
    propertiesCRS.set(IdentifiedObject::IDENTIFIERS_KEY, identifiers);
    auto crs = GeodeticCRS::create(
        propertiesCRS, GeodeticReferenceFrame::EPSG_6326,
        CartesianCS::createGeocentric(UnitOfMeasure::METRE));

    auto got_wkt = crs->exportToWKT(
        WKTFormatter::create(WKTFormatter::Convention::WKT2_SIMPLIFIED));
    auto expected = "GEODCRS[\"WGS 84\",\n"
                    "    DATUM[\"WGS_1984\",\n"
                    "        ELLIPSOID[\"WGS 84\",6378137,298.257223563]],\n"
                    "    CS[Cartesian,3],\n"
                    "        AXIS[\"(X)\",geocentricX],\n"
                    "        AXIS[\"(Y)\",geocentricY],\n"
                    "        AXIS[\"(Z)\",geocentricZ],\n"
                    "        UNIT[\"metre\",1],\n"
                    "    ID[\"authorityA\",\"codeA\"],\n"
                    "    ID[\"authorityB\",\"codeB\"]]";

    EXPECT_EQ(got_wkt, expected);
}

// ---------------------------------------------------------------------------

static VerticalCRSNNPtr createVerticalCRS() {
    PropertyMap propertiesVDatum;
    propertiesVDatum.set(Identifier::CODESPACE_KEY, "EPSG")
        .set(Identifier::CODE_KEY, 5101)
        .set(IdentifiedObject::NAME_KEY, "Ordnance Datum Newlyn");
    auto vdatum = VerticalReferenceFrame::create(propertiesVDatum);
    PropertyMap propertiesCRS;
    propertiesCRS.set(Identifier::CODESPACE_KEY, "EPSG")
        .set(Identifier::CODE_KEY, 5701)
        .set(IdentifiedObject::NAME_KEY, "ODN height");
    return VerticalCRS::create(
        propertiesCRS, vdatum,
        VerticalCS::createGravityRelatedHeight(UnitOfMeasure::METRE));
}

// ---------------------------------------------------------------------------

TEST(crs, verticalCRS_as_WKT2) {
    auto crs = createVerticalCRS();
    auto expected = "VERTCRS[\"ODN height\",\n"
                    "    VDATUM[\"Ordnance Datum Newlyn\"],\n"
                    "    CS[vertical,1],\n"
                    "        AXIS[\"gravity-related height (H)\",up,\n"
                    "            LENGTHUNIT[\"metre\",1]],\n"
                    "    ID[\"EPSG\",5701]]";

    EXPECT_EQ(crs->exportToWKT(WKTFormatter::create()), expected);
}

// ---------------------------------------------------------------------------

TEST(crs, verticalCRS_as_WKT1_GDAL) {
    auto crs = createVerticalCRS();
    auto expected = "VERT_CS[\"ODN height\",\n"
                    "    VERT_DATUM[\"Ordnance Datum Newlyn\",2005,\n"
                    "        AUTHORITY[\"EPSG\",\"5101\"]],\n"
                    "    UNIT[\"metre\",1,\n"
                    "        AUTHORITY[\"EPSG\",9001]],\n"
                    "    AXIS[\"Gravity-related height\",UP],\n"
                    "    AUTHORITY[\"EPSG\",\"5701\"]]";

    EXPECT_EQ(crs->exportToWKT(
                  WKTFormatter::create(WKTFormatter::Convention::WKT1_GDAL)),
              expected);
}

// ---------------------------------------------------------------------------

TEST(datum, vdatum_with_anchor) {
    PropertyMap propertiesVDatum;
    propertiesVDatum.set(Identifier::CODESPACE_KEY, "EPSG")
        .set(Identifier::CODE_KEY, 5101)
        .set(IdentifiedObject::NAME_KEY, "Ordnance Datum Newlyn");
    auto vdatum = VerticalReferenceFrame::create(
        propertiesVDatum, optional<std::string>("my anchor"),
        optional<RealizationMethod>(RealizationMethod::LEVELLING));
    EXPECT_TRUE(vdatum->realizationMethod().has_value());
    EXPECT_EQ(*(vdatum->realizationMethod()), RealizationMethod::LEVELLING);

    auto expected = "VDATUM[\"Ordnance Datum Newlyn\",\n"
                    "    ANCHOR[\"my anchor\"],\n"
                    "    ID[\"EPSG\",5101]]";

    EXPECT_EQ(vdatum->exportToWKT(WKTFormatter::create()), expected);
}

// ---------------------------------------------------------------------------

static CompoundCRSNNPtr createCompoundCRS() {
    PropertyMap properties;
    properties.set(Identifier::CODESPACE_KEY, "codespace")
        .set(Identifier::CODE_KEY, "code")
        .set(IdentifiedObject::NAME_KEY, "horizontal + vertical");
    return CompoundCRS::create(
        properties,
        std::vector<CRSNNPtr>{createProjected(), createVerticalCRS()});
}

// ---------------------------------------------------------------------------

TEST(crs, compoundCRS_as_WKT2) {
    auto crs = createCompoundCRS();
    auto expected =
        "COMPOUNDCRS[\"horizontal + vertical\",\n"
        "    PROJCRS[\"WGS 84 / UTM zone 31N\",\n"
        "        BASEGEODCRS[\"WGS 84\",\n"
        "            DATUM[\"WGS_1984\",\n"
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
        "    ID[\"codespace\",\"code\"]]";

    EXPECT_EQ(crs->exportToWKT(WKTFormatter::create()), expected);
}

// ---------------------------------------------------------------------------

TEST(crs, compoundCRS_as_WKT1_GDAL) {
    auto crs = createCompoundCRS();
    auto expected =
        "COMPD_CS[\"horizontal + vertical\",\n"
        "    PROJCS[\"WGS 84 / UTM zone 31N\",\n"
        "        GEOGCS[\"WGS 84\",\n"
        "            DATUM[\"WGS_1984\",\n"
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
        "    AUTHORITY[\"codespace\",\"code\"]]";

    EXPECT_EQ(crs->exportToWKT(
                  WKTFormatter::create(WKTFormatter::Convention::WKT1_GDAL)),
              expected);
}

// ---------------------------------------------------------------------------

TEST(crs, compoundCRS_of_compoundCRS) {
    auto crs = CompoundCRS::create(
        PropertyMap(),
        std::vector<CRSNNPtr>{
            createProjected(),
            // we are cheating here with a compoundCRS made of only one CRS
            CompoundCRS::create(PropertyMap(),
                                std::vector<CRSNNPtr>{createVerticalCRS()})});
    EXPECT_EQ(crs->componentReferenceSystems().size(), 2);
    EXPECT_TRUE(nn_dynamic_pointer_cast<VerticalCRS>(
                    crs->componentReferenceSystems()[1]) != nullptr);
}

// ---------------------------------------------------------------------------

TEST(crs, compoundCRS_no_name_proided) {
    auto crs = CompoundCRS::create(
        PropertyMap(),
        std::vector<CRSNNPtr>{createProjected(), createVerticalCRS()});
    EXPECT_EQ(*(crs->name()->description()),
              "WGS 84 / UTM zone 31N + ODN height");
}
