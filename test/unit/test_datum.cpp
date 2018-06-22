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
#include "proj/datum.hpp"
#include "proj/io.hpp"
#include "proj/metadata.hpp"
#include "proj/util.hpp"

using namespace osgeo::proj::common;
using namespace osgeo::proj::datum;
using namespace osgeo::proj::io;
using namespace osgeo::proj::metadata;
using namespace osgeo::proj::util;

// ---------------------------------------------------------------------------

TEST(datum, ellipsoid_from_sphere) {

    auto ellipsoid = Ellipsoid::createSphere(PropertyMap(), Length(6378137));
    EXPECT_FALSE(ellipsoid->inverseFlattening().has_value());
    EXPECT_FALSE(ellipsoid->semiMinorAxis().has_value());
    EXPECT_FALSE(ellipsoid->semiMedianAxis().has_value());
    EXPECT_TRUE(ellipsoid->isSphere());
    EXPECT_EQ(ellipsoid->semiMajorAxis(), Length(6378137));

    EXPECT_EQ(ellipsoid->computeSemiMinorAxis(), Length(6378137));
    EXPECT_EQ(ellipsoid->computeInverseFlattening(), Scale(0));

    EXPECT_EQ(ellipsoid->exportToPROJString(PROJStringFormatter::create()),
              "+a=6378137 +b=6378137");
}

// ---------------------------------------------------------------------------

TEST(datum, ellipsoid_from_inverse_flattening) {

    auto ellipsoid = Ellipsoid::createFlattenedSphere(
        PropertyMap(), Length(6378137), Scale(298.257223563));
    EXPECT_TRUE(ellipsoid->inverseFlattening().has_value());
    EXPECT_FALSE(ellipsoid->semiMinorAxis().has_value());
    EXPECT_FALSE(ellipsoid->semiMedianAxis().has_value());
    EXPECT_FALSE(ellipsoid->isSphere());
    EXPECT_EQ(ellipsoid->semiMajorAxis(), Length(6378137));
    EXPECT_EQ(*ellipsoid->inverseFlattening(), Scale(298.257223563));

    EXPECT_EQ(ellipsoid->computeSemiMinorAxis().unit(),
              ellipsoid->semiMajorAxis().unit());
    EXPECT_NEAR(ellipsoid->computeSemiMinorAxis().value(),
                Length(6356752.31424518).value(), 1e-9);
    EXPECT_EQ(ellipsoid->computeInverseFlattening(), Scale(298.257223563));

    EXPECT_EQ(ellipsoid->exportToPROJString(PROJStringFormatter::create()),
              "+ellps=WGS84");
}

// ---------------------------------------------------------------------------

TEST(datum, ellipsoid_from_semi_minor_axis) {

    auto ellipsoid = Ellipsoid::createTwoAxis(PropertyMap(), Length(6378137),
                                              Length(6356752.31424518));
    EXPECT_FALSE(ellipsoid->inverseFlattening().has_value());
    EXPECT_TRUE(ellipsoid->semiMinorAxis().has_value());
    EXPECT_FALSE(ellipsoid->semiMedianAxis().has_value());
    EXPECT_FALSE(ellipsoid->isSphere());
    EXPECT_EQ(ellipsoid->semiMajorAxis(), Length(6378137));
    EXPECT_EQ(*ellipsoid->semiMinorAxis(), Length(6356752.31424518));

    EXPECT_EQ(ellipsoid->computeSemiMinorAxis(), Length(6356752.31424518));
    EXPECT_NEAR(ellipsoid->computeInverseFlattening().value(),
                Scale(298.257223563).value(), 1e-10);

    EXPECT_EQ(ellipsoid->exportToPROJString(PROJStringFormatter::create()),
              "+ellps=WGS84");
}

// ---------------------------------------------------------------------------

TEST(datum, prime_meridian_to_PROJString) {

    EXPECT_EQ(PrimeMeridian::GREENWICH->exportToPROJString(
                  PROJStringFormatter::create()),
              "");

    EXPECT_EQ(
        PrimeMeridian::PARIS->exportToPROJString(PROJStringFormatter::create()),
        "+pm=paris");

    EXPECT_EQ(PrimeMeridian::create(PropertyMap(), Angle(3.5))
                  ->exportToPROJString(PROJStringFormatter::create()),
              "+pm=3.5");

    EXPECT_EQ(
        PrimeMeridian::create(PropertyMap(), Angle(100, UnitOfMeasure::GRAD))
            ->exportToPROJString(PROJStringFormatter::create()),
        "+pm=90");

    EXPECT_EQ(
        PrimeMeridian::create(
            PropertyMap().set(IdentifiedObject::NAME_KEY, "Origin meridian"),
            Angle(0))
            ->exportToPROJString(PROJStringFormatter::create()),
        "");
}

// ---------------------------------------------------------------------------

TEST(datum, datum_with_ANCHOR) {
    auto datum = GeodeticReferenceFrame::create(
        PropertyMap().set(IdentifiedObject::NAME_KEY, "WGS_1984 with anchor"),
        Ellipsoid::WGS84, optional<std::string>("My anchor"),
        PrimeMeridian::GREENWICH);

    auto expected = "DATUM[\"WGS_1984 with anchor\",\n"
                    "    ELLIPSOID[\"WGS 84\",6378137,298.257223563,\n"
                    "        LENGTHUNIT[\"metre\",1],\n"
                    "        ID[\"EPSG\",7030]],\n"
                    "    ANCHOR[\"My anchor\"]]";

    EXPECT_EQ(datum->exportToWKT(WKTFormatter::create()), expected);
}

// ---------------------------------------------------------------------------

TEST(datum, ellipsoid_to_PROJString) {

    EXPECT_EQ(
        Ellipsoid::WGS84->exportToPROJString(PROJStringFormatter::create()),
        "+ellps=WGS84");

    EXPECT_EQ(
        Ellipsoid::GRS1980->exportToPROJString(PROJStringFormatter::create()),
        "+ellps=GRS80");

    EXPECT_EQ(
        Ellipsoid::createFlattenedSphere(
            PropertyMap(), Length(10, UnitOfMeasure("km", 1000)), Scale(0.5))
            ->exportToPROJString(PROJStringFormatter::create()),
        "+a=10000 +rf=0.5");

    EXPECT_EQ(Ellipsoid::createTwoAxis(PropertyMap(),
                                       Length(10, UnitOfMeasure("km", 1000)),
                                       Length(5, UnitOfMeasure("km", 1000)))
                  ->exportToPROJString(PROJStringFormatter::create()),
              "+a=10000 +b=5000");
}

// ---------------------------------------------------------------------------

TEST(datum, temporal_datum_WKT2) {
    auto datum = TemporalDatum::create(
        PropertyMap().set(IdentifiedObject::NAME_KEY, "Gregorian calendar"),
        DateTime::create("0000-01-01"),
        TemporalDatum::CALENDAR_PROLEPTIC_GREGORIAN);

    auto expected = "TDATUM[\"Gregorian calendar\",\n"
                    "    TIMEORIGIN[0000-01-01]]";

    EXPECT_EQ(datum->exportToWKT(
                  WKTFormatter::create(WKTFormatter::Convention::WKT2)),
              expected);

    EXPECT_THROW(datum->exportToWKT(
                     WKTFormatter::create(WKTFormatter::Convention::WKT1_GDAL)),
                 FormattingException);
}

// ---------------------------------------------------------------------------

TEST(datum, temporal_datum_time_origin_non_ISO8601) {
    auto datum = TemporalDatum::create(
        PropertyMap().set(IdentifiedObject::NAME_KEY, "Gregorian calendar"),
        DateTime::create("0001 January 1st"),
        TemporalDatum::CALENDAR_PROLEPTIC_GREGORIAN);

    auto expected = "TDATUM[\"Gregorian calendar\",\n"
                    "    TIMEORIGIN[\"0001 January 1st\"]]";

    EXPECT_EQ(datum->exportToWKT(
                  WKTFormatter::create(WKTFormatter::Convention::WKT2)),
              expected);
}

// ---------------------------------------------------------------------------

TEST(datum, temporal_datum_WKT2_2018) {
    auto datum = TemporalDatum::create(
        PropertyMap().set(IdentifiedObject::NAME_KEY, "Gregorian calendar"),
        DateTime::create("0000-01-01"),
        TemporalDatum::CALENDAR_PROLEPTIC_GREGORIAN);

    auto expected = "TDATUM[\"Gregorian calendar\",\n"
                    "    CALENDAR[\"proleptic Gregorian\"],\n"
                    "    TIMEORIGIN[0000-01-01]]";

    EXPECT_EQ(datum->exportToWKT(
                  WKTFormatter::create(WKTFormatter::Convention::WKT2_2018)),
              expected);
}
