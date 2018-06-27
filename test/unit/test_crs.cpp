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

// to be able to use internal::replaceAll
#define FROM_PROJ_CPP

#include "proj/common.hpp"
#include "proj/coordinateoperation.hpp"
#include "proj/coordinatesystem.hpp"
#include "proj/crs.hpp"
#include "proj/datum.hpp"
#include "proj/internal.hpp"
#include "proj/io.hpp"
#include "proj/metadata.hpp"
#include "proj/util.hpp"

using namespace osgeo::proj::common;
using namespace osgeo::proj::crs;
using namespace osgeo::proj::cs;
using namespace osgeo::proj::datum;
using namespace osgeo::proj::io;
using namespace osgeo::proj::internal;
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
              "        AUTHORITY[\"EPSG\",\"9122\"]],\n"
              "    AXIS[\"Latitude\",NORTH],\n"
              "    AXIS[\"Longitude\",EAST],\n"
              "    AUTHORITY[\"EPSG\",\"4326\"]]");
}

// ---------------------------------------------------------------------------

TEST(crs, EPSG_4326_as_PROJ_string) {
    auto crs = GeographicCRS::EPSG_4326;
    EXPECT_EQ(crs->exportToPROJString(PROJStringFormatter::create()),
              "+proj=pipeline +step +proj=axisswap +order=2,1 +step "
              "+proj=longlat +ellps=WGS84");
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
              "        AUTHORITY[\"EPSG\",\"9122\"]],\n"
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
        "        AUTHORITY[\"EPSG\",\"9105\"]],\n"
        "    AXIS[\"Latitude\",NORTH],\n"
        "    AXIS[\"Longitude\",EAST],\n"
        "    AUTHORITY[\"EPSG\",\"4807\"]]");
}

// ---------------------------------------------------------------------------

TEST(crs, EPSG_4807_as_PROJ_string) {
    auto crs = GeographicCRS::EPSG_4807;
    EXPECT_EQ(crs->exportToPROJString(PROJStringFormatter::create()),
              "+proj=pipeline +step +proj=unitconvert +xy_in=grad +xy_out=rad "
              "+step +proj=axisswap +order=2,1 +step +proj=longlat "
              "+ellps=clrk80ign +pm=paris");
}

// ---------------------------------------------------------------------------

TEST(crs, EPSG_27561_projected_with_geodetic_in_grad_as_PROJ_string) {
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
    EXPECT_EQ(crs->exportToPROJString(PROJStringFormatter::create()),
              "+proj=pipeline +step +proj=unitconvert +xy_in=grad +xy_out=rad "
              "+step +proj=axisswap +order=2,1 +step +proj=lcc +lat_1=49.5 "
              "+lat_0=49.5 +lon_0=0 "
              "+k_0=0.999877341 +x_0=600000 +y_0=200000 +ellps=clrk80ign "
              "+pm=paris");
}

// ---------------------------------------------------------------------------

TEST(crs, EPSG_3040_projected_northing_easting_as_PROJ_string) {
    auto obj = WKTParser().createFromWKT(
        "PROJCRS[\"ETRS89 / UTM zone 28N (N-E)\",\n"
        "  BASEGEODCRS[\"ETRS89\",\n"
        "    DATUM[\"European Terrestrial Reference System 1989\",\n"
        "      ELLIPSOID[\"GRS "
        "1980\",6378137,298.257222101,LENGTHUNIT[\"metre\",1.0]]]],\n"
        "  CONVERSION[\"UTM zone 28N\",\n"
        "    METHOD[\"Transverse Mercator\",ID[\"EPSG\",9807]],\n"
        "    PARAMETER[\"Latitude of natural "
        "origin\",0,ANGLEUNIT[\"degree\",0.01745329252]],\n"
        "    PARAMETER[\"Longitude of natural "
        "origin\",-15,ANGLEUNIT[\"degree\",0.01745329252]],\n"
        "    PARAMETER[\"Scale factor at natural "
        "origin\",0.9996,SCALEUNIT[\"unity\",1.0]],\n"
        "    PARAMETER[\"False easting\",500000,LENGTHUNIT[\"metre\",1.0]],\n"
        "    PARAMETER[\"False northing\",0,LENGTHUNIT[\"metre\",1.0]]],\n"
        "  CS[cartesian,2],\n"
        "    AXIS[\"northing (N)\",north,ORDER[1]],\n"
        "    AXIS[\"easting (E)\",east,ORDER[2]],\n"
        "    LENGTHUNIT[\"metre\",1.0],\n"
        "  ID[\"EPSG\",3040]]");
    auto crs = nn_dynamic_pointer_cast<ProjectedCRS>(obj);
    ASSERT_TRUE(crs != nullptr);
    EXPECT_EQ(crs->exportToPROJString(PROJStringFormatter::create()),
              "+proj=pipeline +step +proj=axisswap +order=2,1 +step +proj=utm "
              "+zone=28 +ellps=GRS80 +step +proj=axisswap +order=2,1");
}

// ---------------------------------------------------------------------------

TEST(crs, EPSG_2222_projected_unit_foot_as_PROJ_string) {
    auto obj = WKTParser().createFromWKT(
        "PROJCRS[\"NAD83 / Arizona East (ft)\",\n"
        "  BASEGEODCRS[\"NAD83\",\n"
        "    DATUM[\"North American Datum 1983\",\n"
        "      ELLIPSOID[\"GRS "
        "1980\",6378137,298.257222101,LENGTHUNIT[\"metre\",1.0]]]],\n"
        "  CONVERSION[\"SPCS83 Arizona East zone (International feet)\",\n"
        "    METHOD[\"Transverse Mercator\",ID[\"EPSG\",9807]],\n"
        "    PARAMETER[\"Latitude of natural "
        "origin\",31,ANGLEUNIT[\"degree\",0.01745329252]],\n"
        "    PARAMETER[\"Longitude of natural "
        "origin\",-110.166666666667,ANGLEUNIT[\"degree\",0.01745329252]],\n"
        "    PARAMETER[\"Scale factor at natural "
        "origin\",0.9999,SCALEUNIT[\"unity\",1.0]],\n"
        "    PARAMETER[\"False easting\",700000,LENGTHUNIT[\"foot\",0.3048]],\n"
        "    PARAMETER[\"False northing\",0,LENGTHUNIT[\"foot\",0.3048]]],\n"
        "  CS[cartesian,2],\n"
        "    AXIS[\"easting (X)\",east,ORDER[1]],\n"
        "    AXIS[\"northing (Y)\",north,ORDER[2]],\n"
        "    LENGTHUNIT[\"foot\",0.3048],\n"
        "  ID[\"EPSG\",2222]]");
    auto crs = nn_dynamic_pointer_cast<ProjectedCRS>(obj);
    ASSERT_TRUE(crs != nullptr);
    EXPECT_EQ(crs->exportToPROJString(PROJStringFormatter::create()),
              "+proj=pipeline +step +proj=axisswap +order=2,1 +step "
              "+proj=tmerc +lat_0=31 +lon_0=-110.166666667025 +k_0=0.9999 "
              "+x_0=213360 +y_0=0 +ellps=GRS80 +units=ft");
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
              "        AUTHORITY[\"EPSG\",\"9001\"]],\n"
              "    AXIS[\"Geocentric X\",OTHER],\n"
              "    AXIS[\"Geocentric Y\",OTHER],\n"
              "    AXIS[\"Geocentric Z\",NORTH],\n"
              "    AUTHORITY[\"EPSG\",\"4328\"]]");
}

// ---------------------------------------------------------------------------

TEST(crs, geocentricCRS_as_PROJ_string) {
    auto crs = createGeocentric();
    EXPECT_EQ(crs->exportToPROJString(PROJStringFormatter::create()),
              "+proj=cart +ellps=WGS84");
}

// ---------------------------------------------------------------------------

TEST(crs, geocentricCRS_non_meter_unit_as_PROJ_string) {
    auto crs = GeodeticCRS::create(
        PropertyMap(), GeodeticReferenceFrame::EPSG_6326,
        CartesianCS::createGeocentric(
            UnitOfMeasure("kilometre", 1000.0, UnitOfMeasure::Type::LINEAR)));

    EXPECT_EQ(crs->exportToPROJString(PROJStringFormatter::create()),
              "+proj=cart +ellps=WGS84 +units=km");
}

// ---------------------------------------------------------------------------

TEST(crs, geocentricCRS_unsupported_unit_as_PROJ_string) {
    auto crs = GeodeticCRS::create(
        PropertyMap(), GeodeticReferenceFrame::EPSG_6326,
        CartesianCS::createGeocentric(
            UnitOfMeasure("my unit", 500.0, UnitOfMeasure::Type::LINEAR)));

    EXPECT_EQ(crs->exportToPROJString(PROJStringFormatter::create()),
              "+proj=cart +ellps=WGS84 +to_meter=500");
}

// ---------------------------------------------------------------------------

static ProjectedCRSNNPtr createProjected() {
    PropertyMap propertiesCRS;
    propertiesCRS.set(Identifier::CODESPACE_KEY, "EPSG")
        .set(Identifier::CODE_KEY, 32631)
        .set(IdentifiedObject::NAME_KEY, "WGS 84 / UTM zone 31N");
    return ProjectedCRS::create(
        propertiesCRS, GeographicCRS::EPSG_4326,
        Conversion::createUTM(PropertyMap(), 31, true),
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
                    "            AUTHORITY[\"EPSG\",\"9122\"]],\n"
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
                    "        AUTHORITY[\"EPSG\",\"9001\"]],\n"
                    "    AXIS[\"Easting\",EAST],\n"
                    "    AXIS[\"Northing\",NORTH],\n"
                    "    AUTHORITY[\"EPSG\",\"32631\"]]";

    EXPECT_EQ(crs->exportToWKT(
                  WKTFormatter::create(WKTFormatter::Convention::WKT1_GDAL)),
              expected);
}

// ---------------------------------------------------------------------------

TEST(crs, projectedCRS_as_PROJ_string) {
    auto crs = createProjected();
    EXPECT_EQ(crs->exportToPROJString(PROJStringFormatter::create()),
              "+proj=pipeline +step +proj=axisswap +order=2,1 +step +proj=utm "
              "+zone=31 +ellps=WGS84");
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

    auto formatter = WKTFormatter::create();
    formatter->setOutputId(false);
    EXPECT_EQ(cs->exportToWKT(formatter), expected);
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
                  "ID[\"EPSG\",4946],\n"
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
        "    ID[\"EPSG\",4946],\n"
        "    REMARK[\"some_remark\"]]";

    EXPECT_EQ(got_wkt, expected);
}

// ---------------------------------------------------------------------------

TEST(crs, multiple_ID) {

    PropertyMap propertiesCRS;
    propertiesCRS.set(IdentifiedObject::NAME_KEY, "WGS 84");
    auto identifiers = ArrayOfBaseObject::create();
    identifiers->add(Identifier::create(
        "codeA", PropertyMap().set(Identifier::CODESPACE_KEY, "authorityA")));
    identifiers->add(Identifier::create(
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
                    "        AUTHORITY[\"EPSG\",\"9001\"]],\n"
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
        "                AUTHORITY[\"EPSG\",\"9122\"]],\n"
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
        "            AUTHORITY[\"EPSG\",\"9001\"]],\n"
        "        AXIS[\"Easting\",EAST],\n"
        "        AXIS[\"Northing\",NORTH],\n"
        "        AUTHORITY[\"EPSG\",\"32631\"]],\n"
        "    VERT_CS[\"ODN height\",\n"
        "        VERT_DATUM[\"Ordnance Datum Newlyn\",2005,\n"
        "            AUTHORITY[\"EPSG\",\"5101\"]],\n"
        "        UNIT[\"metre\",1,\n"
        "            AUTHORITY[\"EPSG\",\"9001\"]],\n"
        "        AXIS[\"Gravity-related height\",UP],\n"
        "        AUTHORITY[\"EPSG\",\"5701\"]],\n"
        "    AUTHORITY[\"codespace\",\"code\"]]";

    EXPECT_EQ(crs->exportToWKT(
                  WKTFormatter::create(WKTFormatter::Convention::WKT1_GDAL)),
              expected);
}

// ---------------------------------------------------------------------------

TEST(crs, compoundCRS_as_PROJ_string) {
    auto crs = createCompoundCRS();
    auto expected = "+proj=pipeline +step +proj=axisswap +order=2,1 +step "
                    "+proj=utm +zone=31 +ellps=WGS84";

    EXPECT_EQ(crs->exportToPROJString(PROJStringFormatter::create()), expected);
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

// ---------------------------------------------------------------------------

TEST(crs, boundCRS_to_WKT2) {

    auto projcrs = ProjectedCRS::create(
        PropertyMap().set(IdentifiedObject::NAME_KEY, "my PROJCRS"),
        GeographicCRS::create(
            PropertyMap().set(IdentifiedObject::NAME_KEY, "my GEOGCRS"),
            GeodeticReferenceFrame::EPSG_6326,
            EllipsoidalCS::createLatitudeLongitude(UnitOfMeasure::DEGREE)),
        Conversion::createUTM(PropertyMap(), 31, true),
        CartesianCS::createEastingNorthing(UnitOfMeasure::METRE));

    auto crs = BoundCRS::createFromTOWGS84(
        projcrs, std::vector<double>{1, 2, 3, 4, 5, 6, 7});

    EXPECT_EQ(*(crs->baseCRS()->name()->description()),
              *(projcrs->name()->description()));

    EXPECT_EQ(*(crs->hubCRS()->name()->description()),
              *(GeographicCRS::EPSG_4326->name()->description()));

    ASSERT_TRUE(crs->transformation()->sourceCRS() != nullptr);
    EXPECT_EQ(*(crs->transformation()->sourceCRS()->name()->description()),
              *(projcrs->baseCRS()->name()->description()));

    ASSERT_TRUE(crs->transformation()->targetCRS() != nullptr);
    EXPECT_EQ(*(crs->transformation()->targetCRS()->name()->description()),
              *(GeographicCRS::EPSG_4326->name()->description()));

    auto values = crs->transformation()->parameterValues();
    ASSERT_EQ(values.size(), 7);
    {
        const auto &opParamvalue =
            nn_dynamic_pointer_cast<OperationParameterValue>(values[0]);
        ASSERT_TRUE(opParamvalue);
        const auto &paramName =
            *(opParamvalue->parameter()->name()->description());
        const auto &parameterValue = opParamvalue->parameterValue();
        EXPECT_TRUE(opParamvalue->parameter()->isEPSG(8605));
        EXPECT_EQ(paramName, "X-axis translation");
        EXPECT_EQ(parameterValue->type(), ParameterValue::Type::MEASURE);
        auto measure = parameterValue->value();
        EXPECT_EQ(measure.unit(), UnitOfMeasure::METRE);
        EXPECT_EQ(measure.value(), 1.0);
    }
    {
        const auto &opParamvalue =
            nn_dynamic_pointer_cast<OperationParameterValue>(values[1]);
        ASSERT_TRUE(opParamvalue);
        const auto &paramName =
            *(opParamvalue->parameter()->name()->description());
        const auto &parameterValue = opParamvalue->parameterValue();
        EXPECT_TRUE(opParamvalue->parameter()->isEPSG(8606));
        EXPECT_EQ(paramName, "Y-axis translation");
        EXPECT_EQ(parameterValue->type(), ParameterValue::Type::MEASURE);
        auto measure = parameterValue->value();
        EXPECT_EQ(measure.unit(), UnitOfMeasure::METRE);
        EXPECT_EQ(measure.value(), 2.0);
    }
    {
        const auto &opParamvalue =
            nn_dynamic_pointer_cast<OperationParameterValue>(values[2]);
        ASSERT_TRUE(opParamvalue);
        const auto &paramName =
            *(opParamvalue->parameter()->name()->description());
        const auto &parameterValue = opParamvalue->parameterValue();
        EXPECT_TRUE(opParamvalue->parameter()->isEPSG(8607));
        EXPECT_EQ(paramName, "Z-axis translation");
        EXPECT_EQ(parameterValue->type(), ParameterValue::Type::MEASURE);
        auto measure = parameterValue->value();
        EXPECT_EQ(measure.unit(), UnitOfMeasure::METRE);
        EXPECT_EQ(measure.value(), 3.0);
    }
    {
        const auto &opParamvalue =
            nn_dynamic_pointer_cast<OperationParameterValue>(values[3]);
        ASSERT_TRUE(opParamvalue);
        const auto &paramName =
            *(opParamvalue->parameter()->name()->description());
        const auto &parameterValue = opParamvalue->parameterValue();
        EXPECT_TRUE(opParamvalue->parameter()->isEPSG(8608));
        EXPECT_EQ(paramName, "X-axis rotation");
        EXPECT_EQ(parameterValue->type(), ParameterValue::Type::MEASURE);
        auto measure = parameterValue->value();
        EXPECT_EQ(measure.unit(), UnitOfMeasure::MICRORADIAN);
        EXPECT_EQ(measure.value(), 4.0);
    }
    {
        const auto &opParamvalue =
            nn_dynamic_pointer_cast<OperationParameterValue>(values[4]);
        ASSERT_TRUE(opParamvalue);
        const auto &paramName =
            *(opParamvalue->parameter()->name()->description());
        const auto &parameterValue = opParamvalue->parameterValue();
        EXPECT_TRUE(opParamvalue->parameter()->isEPSG(8609));
        EXPECT_EQ(paramName, "Y-axis rotation");
        EXPECT_EQ(parameterValue->type(), ParameterValue::Type::MEASURE);
        auto measure = parameterValue->value();
        EXPECT_EQ(measure.unit(), UnitOfMeasure::MICRORADIAN);
        EXPECT_EQ(measure.value(), 5.0);
    }
    {
        const auto &opParamvalue =
            nn_dynamic_pointer_cast<OperationParameterValue>(values[5]);
        ASSERT_TRUE(opParamvalue);
        const auto &paramName =
            *(opParamvalue->parameter()->name()->description());
        const auto &parameterValue = opParamvalue->parameterValue();
        EXPECT_TRUE(opParamvalue->parameter()->isEPSG(8610));
        EXPECT_EQ(paramName, "Z-axis rotation");
        EXPECT_EQ(parameterValue->type(), ParameterValue::Type::MEASURE);
        auto measure = parameterValue->value();
        EXPECT_EQ(measure.unit(), UnitOfMeasure::MICRORADIAN);
        EXPECT_EQ(measure.value(), 6.0);
    }
    {
        const auto &opParamvalue =
            nn_dynamic_pointer_cast<OperationParameterValue>(values[6]);
        ASSERT_TRUE(opParamvalue);
        const auto &paramName =
            *(opParamvalue->parameter()->name()->description());
        const auto &parameterValue = opParamvalue->parameterValue();
        EXPECT_TRUE(opParamvalue->parameter()->isEPSG(8611));
        EXPECT_EQ(paramName, "Scale difference");
        EXPECT_EQ(parameterValue->type(), ParameterValue::Type::MEASURE);
        auto measure = parameterValue->value();
        EXPECT_EQ(measure.unit(), UnitOfMeasure::PARTS_PER_MILLION);
        EXPECT_EQ(measure.value(), 7.0);
    }

    auto expected =
        "BOUNDCRS[SOURCECRS[" + projcrs->exportToWKT(WKTFormatter::create()) +
        "],\n" + "TARGETCRS[" +
        GeographicCRS::EPSG_4326->exportToWKT(WKTFormatter::create()) +
        "],\n"
        "    ABRIDGEDTRANSFORMATION[\"Transformation to WGS84\",\n"
        "        METHOD[\"Position Vector transformation (geocentric "
        "domain)\",\n"
        "            ID[\"EPSG\",1033]],\n"
        "        PARAMETER[\"X-axis translation\",1,\n"
        "            ID[\"EPSG\",8605]],\n"
        "        PARAMETER[\"Y-axis translation\",2,\n"
        "            ID[\"EPSG\",8606]],\n"
        "        PARAMETER[\"Z-axis translation\",3,\n"
        "            ID[\"EPSG\",8607]],\n"
        "        PARAMETER[\"X-axis rotation\",0.825059224988385,\n"
        "            ID[\"EPSG\",8608]],\n"
        "        PARAMETER[\"Y-axis rotation\",1.03132403123548,\n"
        "            ID[\"EPSG\",8609]],\n"
        "        PARAMETER[\"Z-axis rotation\",1.23758883748258,\n"
        "            ID[\"EPSG\",8610]],\n"
        "        PARAMETER[\"Scale difference\",1.000007,\n"
        "            ID[\"EPSG\",8611]]]]";

    EXPECT_EQ(replaceAll(
                  replaceAll(crs->exportToWKT(WKTFormatter::create()), " ", ""),
                  "\n", ""),
              replaceAll(replaceAll(expected, " ", ""), "\n", ""));
}

// ---------------------------------------------------------------------------

TEST(crs, boundCRS_to_WKT1) {

    auto projcrs = ProjectedCRS::create(
        PropertyMap().set(IdentifiedObject::NAME_KEY, "my PROJCRS"),
        GeographicCRS::create(
            PropertyMap().set(IdentifiedObject::NAME_KEY, "my GEOGCRS"),
            GeodeticReferenceFrame::EPSG_6326,
            EllipsoidalCS::createLatitudeLongitude(UnitOfMeasure::DEGREE)),
        Conversion::createUTM(PropertyMap(), 31, true),
        CartesianCS::createEastingNorthing(UnitOfMeasure::METRE));

    auto crs = BoundCRS::createFromTOWGS84(
        projcrs, std::vector<double>{1, 2, 3, 4, 5, 6, 7});
    auto expected = "PROJCS[\"my PROJCRS\",\n"
                    "    GEOGCS[\"my GEOGCRS\",\n"
                    "        DATUM[\"WGS_1984\",\n"
                    "            SPHEROID[\"WGS 84\",6378137,298.257223563,\n"
                    "                AUTHORITY[\"EPSG\",\"7030\"]],\n"
                    "            TOWGS84[1,2,3,4,5,6,7],\n"
                    "            AUTHORITY[\"EPSG\",\"6326\"]],\n"
                    "        PRIMEM[\"Greenwich\",0,\n"
                    "            AUTHORITY[\"EPSG\",\"8901\"]],\n"
                    "        UNIT[\"degree\",0.0174532925199433,\n"
                    "            AUTHORITY[\"EPSG\",\"9122\"]],\n"
                    "        AXIS[\"Latitude\",NORTH],\n"
                    "        AXIS[\"Longitude\",EAST]],\n"
                    "    PROJECTION[\"Transverse_Mercator\"],\n"
                    "    PARAMETER[\"latitude_of_origin\",0],\n"
                    "    PARAMETER[\"central_meridian\",3],\n"
                    "    PARAMETER[\"scale_factor\",0.9996],\n"
                    "    PARAMETER[\"false_easting\",500000],\n"
                    "    PARAMETER[\"false_northing\",0],\n"
                    "    UNIT[\"metre\",1,\n"
                    "        AUTHORITY[\"EPSG\",\"9001\"]],\n"
                    "    AXIS[\"Easting\",EAST],\n"
                    "    AXIS[\"Northing\",NORTH]]";

    EXPECT_EQ(crs->exportToWKT(
                  WKTFormatter::create(WKTFormatter::Convention::WKT1_GDAL)),
              expected);
}

// ---------------------------------------------------------------------------

TEST(crs, boundCRS_geographicCRS_to_PROJ_string) {

    auto basecrs = GeographicCRS::create(
        PropertyMap().set(IdentifiedObject::NAME_KEY, "my GEOGCRS"),
        GeodeticReferenceFrame::EPSG_6326,
        EllipsoidalCS::createLatitudeLongitude(UnitOfMeasure::DEGREE));

    auto crs = BoundCRS::createFromTOWGS84(
        basecrs, std::vector<double>{1, 2, 3, 4, 5, 6, 7});

    EXPECT_EQ(crs->exportToPROJString(PROJStringFormatter::create()),
              "+proj=pipeline +step +proj=axisswap +order=2,1 +step "
              "+proj=longlat +ellps=WGS84 +towgs84=1,2,3,4,5,6,7");
}

// ---------------------------------------------------------------------------

TEST(crs, boundCRS_projectedCRS_to_PROJ_string) {

    auto projcrs = ProjectedCRS::create(
        PropertyMap().set(IdentifiedObject::NAME_KEY, "my PROJCRS"),
        GeographicCRS::create(
            PropertyMap().set(IdentifiedObject::NAME_KEY, "my GEOGCRS"),
            GeodeticReferenceFrame::EPSG_6326,
            EllipsoidalCS::createLatitudeLongitude(UnitOfMeasure::DEGREE)),
        Conversion::createUTM(PropertyMap(), 31, true),
        CartesianCS::createEastingNorthing(UnitOfMeasure::METRE));

    auto crs = BoundCRS::createFromTOWGS84(
        projcrs, std::vector<double>{1, 2, 3, 4, 5, 6, 7});

    EXPECT_EQ(crs->exportToPROJString(PROJStringFormatter::create()),
              "+proj=pipeline +step +proj=axisswap +order=2,1 +step +proj=utm "
              "+zone=31 +ellps=WGS84 +towgs84=1,2,3,4,5,6,7");
}

// ---------------------------------------------------------------------------

TEST(crs, incompatible_boundCRS_hubCRS_to_WKT1) {

    auto crs = BoundCRS::create(
        GeographicCRS::EPSG_4326, GeographicCRS::EPSG_4807,
        Transformation::createGeocentricTranslations(
            PropertyMap(), GeographicCRS::EPSG_4326, GeographicCRS::EPSG_4807,
            1.0, 2.0, 3.0, std::vector<PositionalAccuracyNNPtr>()));

    EXPECT_THROW(crs->exportToWKT(
                     WKTFormatter::create(WKTFormatter::Convention::WKT1_GDAL)),
                 FormattingException);
}

// ---------------------------------------------------------------------------

TEST(crs, incompatible_boundCRS_transformation_to_WKT1) {

    auto crs = BoundCRS::create(
        GeographicCRS::EPSG_4807, GeographicCRS::EPSG_4326,
        Transformation::create(PropertyMap(), GeographicCRS::EPSG_4807,
                               GeographicCRS::EPSG_4326, nullptr, PropertyMap(),
                               std::vector<OperationParameterNNPtr>(),
                               std::vector<ParameterValueNNPtr>(),
                               std::vector<PositionalAccuracyNNPtr>()));

    EXPECT_THROW(crs->exportToWKT(
                     WKTFormatter::create(WKTFormatter::Convention::WKT1_GDAL)),
                 FormattingException);
}

// ---------------------------------------------------------------------------

TEST(crs, WKT1_DATUM_EXTENSION_to_WKT1_and_PROJ_string) {
    auto wkt =
        "PROJCS[\"unnamed\",\n"
        "    GEOGCS[\"International 1909 (Hayford)\",\n"
        "        DATUM[\"unknown\",\n"
        "            SPHEROID[\"intl\",6378388,297],\n"
        "            EXTENSION[\"PROJ4_GRIDS\",\"nzgd2kgrid0005.gsb\"]],\n"
        "        PRIMEM[\"Greenwich\",0],\n"
        "        UNIT[\"degree\",0.0174532925199433],\n"
        "        AXIS[\"Longitude\",EAST],\n"
        "        AXIS[\"Latitude\",NORTH]],\n"
        "    PROJECTION[\"New_Zealand_Map_Grid\"],\n"
        "    PARAMETER[\"latitude_of_origin\",-41],\n"
        "    PARAMETER[\"central_meridian\",173],\n"
        "    PARAMETER[\"false_easting\",2510000],\n"
        "    PARAMETER[\"false_northing\",6023150],\n"
        "    UNIT[\"Meter\",1],\n"
        "    AXIS[\"Easting\",EAST],\n"
        "    AXIS[\"Northing\",NORTH]]";

    auto obj = WKTParser().createFromWKT(wkt);
    auto crs = nn_dynamic_pointer_cast<BoundCRS>(obj);
    ASSERT_TRUE(crs != nullptr);

    EXPECT_EQ(crs->exportToWKT(
                  WKTFormatter::create(WKTFormatter::Convention::WKT1_GDAL)),
              wkt);

    EXPECT_EQ(crs->exportToPROJString(PROJStringFormatter::create()),
              "+proj=nzmg +lat_0=-41 +lon_0=173 +x_0=2510000 +y_0=6023150 "
              "+ellps=intl +nadgrids=nzgd2kgrid0005.gsb +units=m");
}

// ---------------------------------------------------------------------------

TEST(crs, WKT1_VERT_DATUM_EXTENSION_to_WKT1) {
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

    EXPECT_EQ(crs->exportToWKT(
                  WKTFormatter::create(WKTFormatter::Convention::WKT1_GDAL)),
              wkt);
}

// ---------------------------------------------------------------------------

TEST(crs, WKT1_VERT_DATUM_EXTENSION_to_WKT2) {
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

    auto wkt2 =
        "BOUNDCRS[\n"
        "    SOURCECRS[\n"
        "        VERTCRS[\"EGM2008 geoid height\",\n"
        "            VDATUM[\"EGM2008 geoid\"],\n"
        "            CS[vertical,1],\n"
        "                AXIS[\"up\",up,\n"
        "                    LENGTHUNIT[\"metre\",1]],\n"
        "            ID[\"EPSG\",3855]]],\n"
        "    TARGETCRS[\n"
        "        GEODCRS[\"WGS 84\",\n"
        "            DATUM[\"WGS_1984\",\n"
        "                ELLIPSOID[\"WGS 84\",6378137,298.257223563,\n"
        "                    LENGTHUNIT[\"metre\",1]]],\n"
        "            PRIMEM[\"Greenwich\",0,\n"
        "                ANGLEUNIT[\"degree\",0.0174532925199433]],\n"
        "            CS[ellipsoidal,3],\n"
        "                AXIS[\"latitude\",north,\n"
        "                    ORDER[1],\n"
        "                    ANGLEUNIT[\"degree\",0.0174532925199433]],\n"
        "                AXIS[\"longitude\",east,\n"
        "                    ORDER[2],\n"
        "                    ANGLEUNIT[\"degree\",0.0174532925199433]],\n"
        "                AXIS[\"ellipsoidal height\",up,\n"
        "                    ORDER[3],\n"
        "                    LENGTHUNIT[\"metre\",1]],\n"
        "            ID[\"EPSG\",4979]]],\n"
        "    ABRIDGEDTRANSFORMATION[\"EGM2008 geoid height to WGS84 "
        "ellipsoidal height\",\n"
        "        METHOD[\"GravityRelatedHeight to Geographic3D\"],\n"
        "        PARAMETERFILE[\"Geoid (height correction) model "
        "file\",\"egm08_25.gtx\",\n"
        "            ID[\"EPSG\",8666]]]]";

    EXPECT_EQ(
        crs->exportToWKT(WKTFormatter::create(WKTFormatter::Convention::WKT2)),
        wkt2);
}

// ---------------------------------------------------------------------------

TEST(crs, WKT1_VERT_DATUM_EXTENSION_to_PROJ_string) {
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

    EXPECT_EQ(crs->exportToPROJString(PROJStringFormatter::create()),
              "+nadgrids=egm08_25.gtx");
}

// ---------------------------------------------------------------------------

TEST(crs, extractGeographicCRS) {
    EXPECT_EQ(GeographicCRS::EPSG_4326->extractGeographicCRS(),
              GeographicCRS::EPSG_4326);
    EXPECT_EQ(createProjected()->extractGeographicCRS(),
              GeographicCRS::EPSG_4326);
    EXPECT_EQ(
        CompoundCRS::create(PropertyMap(),
                            std::vector<CRSNNPtr>{GeographicCRS::EPSG_4326})
            ->extractGeographicCRS(),
        GeographicCRS::EPSG_4326);
}

// ---------------------------------------------------------------------------

static DerivedGeographicCRSNNPtr createDerivedGeographicCRS() {

    auto derivingConversion = Conversion::create(
        PropertyMap().set(IdentifiedObject::NAME_KEY, "Atlantic pole"),
        PropertyMap().set(IdentifiedObject::NAME_KEY, "Pole rotation"),
        std::vector<OperationParameterNNPtr>{
            OperationParameter::create(PropertyMap().set(
                IdentifiedObject::NAME_KEY, "Latitude of rotated pole")),
            OperationParameter::create(PropertyMap().set(
                IdentifiedObject::NAME_KEY, "Longitude of rotated pole")),
            OperationParameter::create(
                PropertyMap().set(IdentifiedObject::NAME_KEY, "Axis rotation")),
        },
        std::vector<ParameterValueNNPtr>{
            ParameterValue::create(Angle(52.0)),
            ParameterValue::create(Angle(-30.0)),
            ParameterValue::create(Angle(-25.0)),
        });

    return DerivedGeographicCRS::create(
        PropertyMap().set(IdentifiedObject::NAME_KEY, "WMO Atlantic Pole"),
        GeographicCRS::EPSG_4326, derivingConversion,
        EllipsoidalCS::createLatitudeLongitude(UnitOfMeasure::DEGREE));
}

// ---------------------------------------------------------------------------

TEST(crs, derivedGeographicCRS_WKT2) {

    auto expected = "GEODCRS[\"WMO Atlantic Pole\",\n"
                    "    BASEGEODCRS[\"WGS 84\",\n"
                    "        DATUM[\"WGS_1984\",\n"
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

    EXPECT_EQ(createDerivedGeographicCRS()->exportToWKT(
                  WKTFormatter::create(WKTFormatter::Convention::WKT2)),
              expected);
}

// ---------------------------------------------------------------------------

TEST(crs, derivedGeographicCRS_WKT2_2018) {

    auto expected = "GEOGCRS[\"WMO Atlantic Pole\",\n"
                    "    BASEGEOGCRS[\"WGS 84\",\n"
                    "        DATUM[\"WGS_1984\",\n"
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

    EXPECT_EQ(createDerivedGeographicCRS()->exportToWKT(
                  WKTFormatter::create(WKTFormatter::Convention::WKT2_2018)),
              expected);
}

// ---------------------------------------------------------------------------

TEST(crs, derivedGeographicCRS_WKT1) {

    EXPECT_THROW(createDerivedGeographicCRS()->exportToWKT(
                     WKTFormatter::create(WKTFormatter::Convention::WKT1_GDAL)),
                 FormattingException);
}

// ---------------------------------------------------------------------------

static DerivedGeodeticCRSNNPtr createDerivedGeodeticCRS() {

    auto derivingConversion = Conversion::create(
        PropertyMap().set(IdentifiedObject::NAME_KEY, "Some conversion"),
        PropertyMap().set(IdentifiedObject::NAME_KEY, "Some method"),
        std::vector<OperationParameterNNPtr>{},
        std::vector<ParameterValueNNPtr>{});

    return DerivedGeodeticCRS::create(
        PropertyMap().set(IdentifiedObject::NAME_KEY, "Derived geodetic CRS"),
        GeographicCRS::EPSG_4326, derivingConversion,
        CartesianCS::createGeocentric(UnitOfMeasure::METRE));
}

// ---------------------------------------------------------------------------

TEST(crs, derivedGeodeticCRS_WKT2) {

    auto expected = "GEODCRS[\"Derived geodetic CRS\",\n"
                    "    BASEGEODCRS[\"WGS 84\",\n"
                    "        DATUM[\"WGS_1984\",\n"
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

    EXPECT_EQ(createDerivedGeodeticCRS()->exportToWKT(
                  WKTFormatter::create(WKTFormatter::Convention::WKT2)),
              expected);
}

// ---------------------------------------------------------------------------

TEST(crs, derivedGeodeticCRS_WKT2_2018) {

    auto expected = "GEODCRS[\"Derived geodetic CRS\",\n"
                    "    BASEGEOGCRS[\"WGS 84\",\n"
                    "        DATUM[\"WGS_1984\",\n"
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

    EXPECT_EQ(createDerivedGeodeticCRS()->exportToWKT(
                  WKTFormatter::create(WKTFormatter::Convention::WKT2_2018)),
              expected);
}

// ---------------------------------------------------------------------------

TEST(crs, derivedGeodeticCRS_WKT1) {

    EXPECT_THROW(createDerivedGeodeticCRS()->exportToWKT(
                     WKTFormatter::create(WKTFormatter::Convention::WKT1_GDAL)),
                 FormattingException);
}

// ---------------------------------------------------------------------------

static TemporalCRSNNPtr createDateTimeTemporalCRS() {

    auto datum = TemporalDatum::create(
        PropertyMap().set(IdentifiedObject::NAME_KEY, "Gregorian calendar"),
        DateTime::create("0000-01-01"),
        TemporalDatum::CALENDAR_PROLEPTIC_GREGORIAN);

    auto cs = DateTimeTemporalCS::create(
        PropertyMap(),
        CoordinateSystemAxis::create(
            PropertyMap().set(IdentifiedObject::NAME_KEY, "Time"), "T",
            AxisDirection::FUTURE, UnitOfMeasure::NONE));

    return TemporalCRS::create(
        PropertyMap().set(IdentifiedObject::NAME_KEY, "Temporal CRS"), datum,
        cs);
}

// ---------------------------------------------------------------------------

TEST(crs, dateTimeTemporalCRS_WKT2) {

    auto expected = "TIMECRS[\"Temporal CRS\",\n"
                    "    TDATUM[\"Gregorian calendar\",\n"
                    "        TIMEORIGIN[0000-01-01]],\n"
                    "    CS[temporal,1],\n"
                    "        AXIS[\"time (T)\",future]]";

    EXPECT_EQ(createDateTimeTemporalCRS()->exportToWKT(
                  WKTFormatter::create(WKTFormatter::Convention::WKT2)),
              expected);

    EXPECT_THROW(createDateTimeTemporalCRS()->exportToWKT(
                     WKTFormatter::create(WKTFormatter::Convention::WKT1_GDAL)),
                 FormattingException);
}

// ---------------------------------------------------------------------------

TEST(crs, dateTimeTemporalCRS_WKT2_2018) {

    auto expected = "TIMECRS[\"Temporal CRS\",\n"
                    "    TDATUM[\"Gregorian calendar\",\n"
                    "        CALENDAR[\"proleptic Gregorian\"],\n"
                    "        TIMEORIGIN[0000-01-01]],\n"
                    "    CS[TemporalDateTime,1],\n"
                    "        AXIS[\"time (T)\",future]]";

    EXPECT_EQ(createDateTimeTemporalCRS()->exportToWKT(
                  WKTFormatter::create(WKTFormatter::Convention::WKT2_2018)),
              expected);
}

// ---------------------------------------------------------------------------

static TemporalCRSNNPtr createTemporalCountCRSWithConvFactor() {

    auto datum = TemporalDatum::create(
        PropertyMap().set(IdentifiedObject::NAME_KEY, "GPS time origin"),
        DateTime::create("1980-01-01T00:00:00.0Z"),
        TemporalDatum::CALENDAR_PROLEPTIC_GREGORIAN);

    auto cs = TemporalCountCS::create(
        PropertyMap(),
        CoordinateSystemAxis::create(PropertyMap(), "T", AxisDirection::FUTURE,
                                     UnitOfMeasure("milliseconds (ms)", 0.001,
                                                   UnitOfMeasure::Type::TIME)));

    return TemporalCRS::create(
        PropertyMap().set(IdentifiedObject::NAME_KEY, "GPS milliseconds"),
        datum, cs);
}

// ---------------------------------------------------------------------------

TEST(crs, temporalCountCRSWithConvFactor_WKT2_2018) {

    auto expected = "TIMECRS[\"GPS milliseconds\",\n"
                    "    TDATUM[\"GPS time origin\",\n"
                    "        CALENDAR[\"proleptic Gregorian\"],\n"
                    "        TIMEORIGIN[1980-01-01T00:00:00.0Z]],\n"
                    "    CS[TemporalCount,1],\n"
                    "        AXIS[\"(T)\",future,\n"
                    "            TIMEUNIT[\"milliseconds (ms)\",0.001]]]";

    EXPECT_EQ(createTemporalCountCRSWithConvFactor()->exportToWKT(
                  WKTFormatter::create(WKTFormatter::Convention::WKT2_2018)),
              expected);
}

// ---------------------------------------------------------------------------

static TemporalCRSNNPtr createTemporalCountCRSWithoutConvFactor() {

    auto datum = TemporalDatum::create(
        PropertyMap().set(IdentifiedObject::NAME_KEY, "29 December 1979"),
        DateTime::create("1979-12-29T00"),
        TemporalDatum::CALENDAR_PROLEPTIC_GREGORIAN);

    auto cs = TemporalCountCS::create(
        PropertyMap(),
        CoordinateSystemAxis::create(
            PropertyMap().set(IdentifiedObject::NAME_KEY, "Time"), "",
            AxisDirection::FUTURE,
            UnitOfMeasure("hour", 0, UnitOfMeasure::Type::TIME)));

    return TemporalCRS::create(
        PropertyMap().set(IdentifiedObject::NAME_KEY,
                          "Calendar hours from 1979-12-29"),
        datum, cs);
}

// ---------------------------------------------------------------------------

TEST(crs, temporalCountCRSWithoutConvFactor_WKT2_2018) {

    auto expected = "TIMECRS[\"Calendar hours from 1979-12-29\",\n"
                    "    TDATUM[\"29 December 1979\",\n"
                    "        CALENDAR[\"proleptic Gregorian\"],\n"
                    "        TIMEORIGIN[1979-12-29T00]],\n"
                    "    CS[TemporalCount,1],\n"
                    "        AXIS[\"time\",future,\n"
                    "            TIMEUNIT[\"hour\"]]]";

    EXPECT_EQ(createTemporalCountCRSWithoutConvFactor()->exportToWKT(
                  WKTFormatter::create(WKTFormatter::Convention::WKT2_2018)),
              expected);
}

// ---------------------------------------------------------------------------

static TemporalCRSNNPtr createTemporalMeasureCRSWithoutConvFactor() {

    auto datum = TemporalDatum::create(
        PropertyMap().set(IdentifiedObject::NAME_KEY, "Common Era"),
        DateTime::create("0000"), TemporalDatum::CALENDAR_PROLEPTIC_GREGORIAN);

    auto cs = TemporalMeasureCS::create(
        PropertyMap(),
        CoordinateSystemAxis::create(
            PropertyMap().set(IdentifiedObject::NAME_KEY, "Decimal years"), "a",
            AxisDirection::FUTURE,
            UnitOfMeasure("year", 0, UnitOfMeasure::Type::TIME)));

    return TemporalCRS::create(
        PropertyMap().set(IdentifiedObject::NAME_KEY, "Decimal Years CE"),
        datum, cs);
}

// ---------------------------------------------------------------------------

TEST(crs, temporalMeasureCRSWithoutConvFactor_WKT2_2018) {

    auto expected = "TIMECRS[\"Decimal Years CE\",\n"
                    "    TDATUM[\"Common Era\",\n"
                    "        CALENDAR[\"proleptic Gregorian\"],\n"
                    "        TIMEORIGIN[0000]],\n"
                    "    CS[TemporalMeasure,1],\n"
                    "        AXIS[\"decimal years (a)\",future,\n"
                    "            TIMEUNIT[\"year\"]]]";

    EXPECT_EQ(createTemporalMeasureCRSWithoutConvFactor()->exportToWKT(
                  WKTFormatter::create(WKTFormatter::Convention::WKT2_2018)),
              expected);
}
