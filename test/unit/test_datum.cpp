/******************************************************************************
 *
 * Project:  PROJ
 * Purpose:  Test ISO19111:2019 implementation
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

namespace {
struct UnrelatedObject : public IComparable {
    UnrelatedObject() = default;

    bool _isEquivalentTo(const IComparable *, Criterion,
                         const DatabaseContextPtr &) const override {
        assert(false);
        return false;
    }
};

static nn<std::shared_ptr<UnrelatedObject>> createUnrelatedObject() {
    return nn_make_shared<UnrelatedObject>();
}
} // namespace

// ---------------------------------------------------------------------------

TEST(datum, ellipsoid_from_sphere) {

    auto ellipsoid = Ellipsoid::createSphere(PropertyMap(), Length(6378137));
    EXPECT_FALSE(ellipsoid->inverseFlattening().has_value());
    EXPECT_FALSE(ellipsoid->semiMinorAxis().has_value());
    EXPECT_FALSE(ellipsoid->semiMedianAxis().has_value());
    EXPECT_TRUE(ellipsoid->isSphere());
    EXPECT_EQ(ellipsoid->semiMajorAxis(), Length(6378137));
    EXPECT_EQ(ellipsoid->celestialBody(), "Earth");

    EXPECT_EQ(ellipsoid->computeSemiMinorAxis(), Length(6378137));
    EXPECT_EQ(ellipsoid->computedInverseFlattening(), 0);

    EXPECT_EQ(
        ellipsoid->exportToPROJString(PROJStringFormatter::create().get()),
        "+R=6378137");

    EXPECT_TRUE(ellipsoid->isEquivalentTo(ellipsoid.get()));
    EXPECT_FALSE(ellipsoid->isEquivalentTo(createUnrelatedObject().get()));
}

// ---------------------------------------------------------------------------

TEST(datum, ellipsoid_non_earth) {

    auto ellipsoid =
        Ellipsoid::createSphere(PropertyMap(), Length(1), "Unity sphere");
    EXPECT_EQ(ellipsoid->celestialBody(), "Unity sphere");
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
    EXPECT_EQ(ellipsoid->computedInverseFlattening(), 298.257223563);

    EXPECT_EQ(
        ellipsoid->exportToPROJString(PROJStringFormatter::create().get()),
        "+ellps=WGS84");

    EXPECT_TRUE(ellipsoid->isEquivalentTo(ellipsoid.get()));
    EXPECT_FALSE(ellipsoid->isEquivalentTo(
        Ellipsoid::createTwoAxis(PropertyMap(), Length(6378137),
                                 Length(6356752.31424518))
            .get()));
    EXPECT_TRUE(ellipsoid->isEquivalentTo(
        Ellipsoid::createTwoAxis(PropertyMap(), Length(6378137),
                                 Length(6356752.31424518))
            .get(),
        IComparable::Criterion::EQUIVALENT));

    EXPECT_FALSE(Ellipsoid::WGS84->isEquivalentTo(
        Ellipsoid::GRS1980.get(), IComparable::Criterion::EQUIVALENT));
}

// ---------------------------------------------------------------------------

TEST(datum, ellipsoid_from_null_inverse_flattening) {

    auto ellipsoid = Ellipsoid::createFlattenedSphere(
        PropertyMap(), Length(6378137), Scale(0));
    EXPECT_FALSE(ellipsoid->inverseFlattening().has_value());
    EXPECT_FALSE(ellipsoid->semiMinorAxis().has_value());
    EXPECT_FALSE(ellipsoid->semiMedianAxis().has_value());
    EXPECT_TRUE(ellipsoid->isSphere());
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
    EXPECT_NEAR(ellipsoid->computedInverseFlattening(), 298.257223563, 1e-10);

    EXPECT_EQ(
        ellipsoid->exportToPROJString(PROJStringFormatter::create().get()),
        "+ellps=WGS84");

    EXPECT_TRUE(ellipsoid->isEquivalentTo(ellipsoid.get()));
    EXPECT_FALSE(ellipsoid->isEquivalentTo(
        Ellipsoid::createFlattenedSphere(PropertyMap(), Length(6378137),
                                         Scale(298.257223563))
            .get()));
    EXPECT_TRUE(ellipsoid->isEquivalentTo(
        Ellipsoid::createFlattenedSphere(PropertyMap(), Length(6378137),
                                         Scale(298.257223563))
            .get(),
        IComparable::Criterion::EQUIVALENT));
}

// ---------------------------------------------------------------------------

TEST(datum, prime_meridian_to_PROJString) {

    EXPECT_EQ(PrimeMeridian::GREENWICH->exportToPROJString(
                  PROJStringFormatter::create().get()),
              "+proj=noop");

    EXPECT_EQ(PrimeMeridian::PARIS->exportToPROJString(
                  PROJStringFormatter::create().get()),
              "+pm=paris");

    EXPECT_EQ(PrimeMeridian::create(PropertyMap(), Angle(3.5))
                  ->exportToPROJString(PROJStringFormatter::create().get()),
              "+pm=3.5");

    EXPECT_EQ(
        PrimeMeridian::create(PropertyMap(), Angle(100, UnitOfMeasure::GRAD))
            ->exportToPROJString(PROJStringFormatter::create().get()),
        "+pm=90");

    EXPECT_EQ(
        PrimeMeridian::create(
            PropertyMap().set(IdentifiedObject::NAME_KEY, "Origin meridian"),
            Angle(0))
            ->exportToPROJString(PROJStringFormatter::create().get()),
        "+proj=noop");

    EXPECT_TRUE(PrimeMeridian::GREENWICH->isEquivalentTo(
        PrimeMeridian::GREENWICH.get()));
    EXPECT_FALSE(PrimeMeridian::GREENWICH->isEquivalentTo(
        createUnrelatedObject().get()));
}

// ---------------------------------------------------------------------------

TEST(datum, prime_meridian_to_JSON) {

    EXPECT_EQ(PrimeMeridian::GREENWICH->exportToJSON(
                  &(JSONFormatter::create()->setSchema(""))),
              "{\n"
              "  \"type\": \"PrimeMeridian\",\n"
              "  \"name\": \"Greenwich\",\n"
              "  \"longitude\": 0,\n"
              "  \"id\": {\n"
              "    \"authority\": \"EPSG\",\n"
              "    \"code\": 8901\n"
              "  }\n"
              "}");

    EXPECT_EQ(PrimeMeridian::PARIS->exportToJSON(
                  &(JSONFormatter::create()->setSchema(""))),
              "{\n"
              "  \"type\": \"PrimeMeridian\",\n"
              "  \"name\": \"Paris\",\n"
              "  \"longitude\": {\n"
              "    \"value\": 2.5969213,\n"
              "    \"unit\": {\n"
              "      \"type\": \"AngularUnit\",\n"
              "      \"name\": \"grad\",\n"
              "      \"conversion_factor\": 0.015707963267949\n"
              "    }\n"
              "  },\n"
              "  \"id\": {\n"
              "    \"authority\": \"EPSG\",\n"
              "    \"code\": 8903\n"
              "  }\n"
              "}");
}

// ---------------------------------------------------------------------------

TEST(datum, datum_with_ANCHOR) {
    auto datum = GeodeticReferenceFrame::create(
        PropertyMap().set(IdentifiedObject::NAME_KEY, "WGS_1984 with anchor"),
        Ellipsoid::WGS84, optional<std::string>("My anchor"),
        PrimeMeridian::GREENWICH);

    ASSERT_TRUE(datum->anchorDefinition());
    EXPECT_EQ(*datum->anchorDefinition(), "My anchor");
    ASSERT_FALSE(datum->anchorEpoch());

    auto expected = "DATUM[\"WGS_1984 with anchor\",\n"
                    "    ELLIPSOID[\"WGS 84\",6378137,298.257223563,\n"
                    "        LENGTHUNIT[\"metre\",1],\n"
                    "        ID[\"EPSG\",7030]],\n"
                    "    ANCHOR[\"My anchor\"]]";

    EXPECT_EQ(datum->exportToWKT(WKTFormatter::create().get()), expected);
}

// ---------------------------------------------------------------------------

TEST(datum, datum_with_ANCHOREPOCH) {
    auto datum = GeodeticReferenceFrame::create(
        PropertyMap().set(IdentifiedObject::NAME_KEY, "my_datum"),
        Ellipsoid::WGS84, optional<std::string>(),
        optional<Measure>(Measure(2002.5, UnitOfMeasure::YEAR)),
        PrimeMeridian::GREENWICH);

    ASSERT_FALSE(datum->anchorDefinition());
    ASSERT_TRUE(datum->anchorEpoch());
    EXPECT_NEAR(datum->anchorEpoch()->convertToUnit(UnitOfMeasure::YEAR),
                2002.5, 1e-8);

    auto expected = "DATUM[\"my_datum\",\n"
                    "    ELLIPSOID[\"WGS 84\",6378137,298.257223563,\n"
                    "        LENGTHUNIT[\"metre\",1],\n"
                    "        ID[\"EPSG\",7030]],\n"
                    "    ANCHOREPOCH[2002.5]]";

    EXPECT_EQ(
        datum->exportToWKT(
            WKTFormatter::create(WKTFormatter::Convention::WKT2_2019).get()),
        expected);
}

// ---------------------------------------------------------------------------

TEST(datum, dynamic_geodetic_reference_frame) {
    auto drf = DynamicGeodeticReferenceFrame::create(
        PropertyMap().set(IdentifiedObject::NAME_KEY, "test"), Ellipsoid::WGS84,
        optional<std::string>("My anchor"), PrimeMeridian::GREENWICH,
        Measure(2018.5, UnitOfMeasure::YEAR),
        optional<std::string>("My model"));

    auto expected = "DATUM[\"test\",\n"
                    "    ELLIPSOID[\"WGS 84\",6378137,298.257223563,\n"
                    "        LENGTHUNIT[\"metre\",1],\n"
                    "        ID[\"EPSG\",7030]],\n"
                    "    ANCHOR[\"My anchor\"]]";

    EXPECT_EQ(drf->exportToWKT(WKTFormatter::create().get()), expected);

    auto expected_wtk2_2019 =
        "DYNAMIC[\n"
        "    FRAMEEPOCH[2018.5],\n"
        "    MODEL[\"My model\"]],\n"
        "DATUM[\"test\",\n"
        "    ELLIPSOID[\"WGS 84\",6378137,298.257223563,\n"
        "        LENGTHUNIT[\"metre\",1],\n"
        "        ID[\"EPSG\",7030]],\n"
        "    ANCHOR[\"My anchor\"]]";
    EXPECT_EQ(
        drf->exportToWKT(
            WKTFormatter::create(WKTFormatter::Convention::WKT2_2019).get()),
        expected_wtk2_2019);

    EXPECT_TRUE(drf->isEquivalentTo(drf.get()));
    EXPECT_TRUE(
        drf->isEquivalentTo(drf.get(), IComparable::Criterion::EQUIVALENT));
    EXPECT_FALSE(drf->isEquivalentTo(createUnrelatedObject().get()));

    // "Same" datum, except that it is a non-dynamic one
    auto datum = GeodeticReferenceFrame::create(
        PropertyMap().set(IdentifiedObject::NAME_KEY, "test"), Ellipsoid::WGS84,
        optional<std::string>("My anchor"), PrimeMeridian::GREENWICH);
    EXPECT_FALSE(datum->isEquivalentTo(drf.get()));
    EXPECT_FALSE(drf->isEquivalentTo(datum.get()));
    EXPECT_TRUE(
        datum->isEquivalentTo(drf.get(), IComparable::Criterion::EQUIVALENT));
    EXPECT_TRUE(
        drf->isEquivalentTo(datum.get(), IComparable::Criterion::EQUIVALENT));

    auto unrelated_datum = GeodeticReferenceFrame::create(
        PropertyMap().set(IdentifiedObject::NAME_KEY, "test2"),
        Ellipsoid::WGS84, optional<std::string>("My anchor"),
        PrimeMeridian::GREENWICH);
    EXPECT_FALSE(unrelated_datum->isEquivalentTo(drf.get()));
    EXPECT_FALSE(drf->isEquivalentTo(unrelated_datum.get()));
    EXPECT_FALSE(unrelated_datum->isEquivalentTo(
        drf.get(), IComparable::Criterion::EQUIVALENT));
    EXPECT_FALSE(drf->isEquivalentTo(unrelated_datum.get(),
                                     IComparable::Criterion::EQUIVALENT));
}

// ---------------------------------------------------------------------------

TEST(datum, ellipsoid_to_PROJString) {

    EXPECT_EQ(Ellipsoid::WGS84->exportToPROJString(
                  PROJStringFormatter::create().get()),
              "+ellps=WGS84");

    EXPECT_EQ(Ellipsoid::GRS1980->exportToPROJString(
                  PROJStringFormatter::create().get()),
              "+ellps=GRS80");

    EXPECT_EQ(
        Ellipsoid::createFlattenedSphere(
            PropertyMap(), Length(10, UnitOfMeasure("km", 1000)), Scale(0.5))
            ->exportToPROJString(PROJStringFormatter::create().get()),
        "+a=10000 +rf=0.5");

    EXPECT_EQ(Ellipsoid::createTwoAxis(PropertyMap(),
                                       Length(10, UnitOfMeasure("km", 1000)),
                                       Length(5, UnitOfMeasure("km", 1000)))
                  ->exportToPROJString(PROJStringFormatter::create().get()),
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
                  WKTFormatter::create(WKTFormatter::Convention::WKT2).get()),
              expected);

    EXPECT_THROW(
        datum->exportToWKT(
            WKTFormatter::create(WKTFormatter::Convention::WKT1_GDAL).get()),
        FormattingException);

    EXPECT_TRUE(datum->isEquivalentTo(datum.get()));
    EXPECT_FALSE(datum->isEquivalentTo(createUnrelatedObject().get()));
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
                  WKTFormatter::create(WKTFormatter::Convention::WKT2).get()),
              expected);
}

// ---------------------------------------------------------------------------

TEST(datum, temporal_datum_WKT2_2019) {
    auto datum = TemporalDatum::create(
        PropertyMap().set(IdentifiedObject::NAME_KEY, "Gregorian calendar"),
        DateTime::create("0000-01-01"),
        TemporalDatum::CALENDAR_PROLEPTIC_GREGORIAN);

    auto expected = "TDATUM[\"Gregorian calendar\",\n"
                    "    CALENDAR[\"proleptic Gregorian\"],\n"
                    "    TIMEORIGIN[0000-01-01]]";

    EXPECT_EQ(
        datum->exportToWKT(
            WKTFormatter::create(WKTFormatter::Convention::WKT2_2019).get()),
        expected);
}

// ---------------------------------------------------------------------------

TEST(datum, dynamic_vertical_reference_frame) {
    auto drf = DynamicVerticalReferenceFrame::create(
        PropertyMap().set(IdentifiedObject::NAME_KEY, "test"),
        optional<std::string>("My anchor"), optional<RealizationMethod>(),
        Measure(2018.5, UnitOfMeasure::YEAR),
        optional<std::string>("My model"));

    auto expected = "VDATUM[\"test\",\n"
                    "    ANCHOR[\"My anchor\"]]";

    EXPECT_EQ(drf->exportToWKT(WKTFormatter::create().get()), expected);

    auto expected_wtk2_2019 = "DYNAMIC[\n"
                              "    FRAMEEPOCH[2018.5],\n"
                              "    MODEL[\"My model\"]],\n"
                              "VDATUM[\"test\",\n"
                              "    ANCHOR[\"My anchor\"]]";
    EXPECT_EQ(
        drf->exportToWKT(
            WKTFormatter::create(WKTFormatter::Convention::WKT2_2019).get()),
        expected_wtk2_2019);

    EXPECT_TRUE(drf->isEquivalentTo(drf.get()));
    EXPECT_TRUE(
        drf->isEquivalentTo(drf.get(), IComparable::Criterion::EQUIVALENT));
    EXPECT_FALSE(drf->isEquivalentTo(createUnrelatedObject().get()));

    // "Same" datum, except that it is a non-dynamic one
    auto datum = VerticalReferenceFrame::create(
        PropertyMap().set(IdentifiedObject::NAME_KEY, "test"),
        optional<std::string>("My anchor"), optional<RealizationMethod>());
    EXPECT_FALSE(datum->isEquivalentTo(drf.get()));
    EXPECT_FALSE(drf->isEquivalentTo(datum.get()));
    EXPECT_TRUE(
        datum->isEquivalentTo(drf.get(), IComparable::Criterion::EQUIVALENT));
    EXPECT_TRUE(
        drf->isEquivalentTo(datum.get(), IComparable::Criterion::EQUIVALENT));

    auto unrelated_datum = VerticalReferenceFrame::create(
        PropertyMap().set(IdentifiedObject::NAME_KEY, "test2"),
        optional<std::string>("My anchor"), optional<RealizationMethod>());
    EXPECT_FALSE(unrelated_datum->isEquivalentTo(drf.get()));
    EXPECT_FALSE(drf->isEquivalentTo(unrelated_datum.get()));
    EXPECT_FALSE(unrelated_datum->isEquivalentTo(
        drf.get(), IComparable::Criterion::EQUIVALENT));
    EXPECT_FALSE(drf->isEquivalentTo(unrelated_datum.get(),
                                     IComparable::Criterion::EQUIVALENT));
}

// ---------------------------------------------------------------------------

TEST(datum, datum_ensemble) {
    auto otherDatum = GeodeticReferenceFrame::create(
        PropertyMap().set(IdentifiedObject::NAME_KEY, "other datum"),
        Ellipsoid::WGS84, optional<std::string>(), PrimeMeridian::GREENWICH);
    auto ensemble = DatumEnsemble::create(
        PropertyMap().set(IdentifiedObject::NAME_KEY, "test"),
        std::vector<DatumNNPtr>{GeodeticReferenceFrame::EPSG_6326, otherDatum},
        PositionalAccuracy::create("100"));
    EXPECT_EQ(ensemble->datums().size(), 2U);
    EXPECT_EQ(ensemble->positionalAccuracy()->value(), "100");

    EXPECT_EQ(
        ensemble->exportToWKT(
            WKTFormatter::create(WKTFormatter::Convention::WKT2_2019).get()),
        "ENSEMBLE[\"test\",\n"
        "    MEMBER[\"World Geodetic System 1984\",\n"
        "        ID[\"EPSG\",6326]],\n"
        "    MEMBER[\"other datum\"],\n"
        "    ELLIPSOID[\"WGS 84\",6378137,298.257223563,\n"
        "        LENGTHUNIT[\"metre\",1],\n"
        "        ID[\"EPSG\",7030]],\n"
        "    ENSEMBLEACCURACY[100]]");

    EXPECT_EQ(
        ensemble->exportToWKT(
            WKTFormatter::create(WKTFormatter::Convention::WKT2_2015).get()),
        "DATUM[\"test\",\n"
        "    ELLIPSOID[\"WGS 84\",6378137,298.257223563,\n"
        "        LENGTHUNIT[\"metre\",1],\n"
        "        ID[\"EPSG\",7030]]]");
}

// ---------------------------------------------------------------------------

TEST(datum, datum_ensemble_vertical) {
    auto ensemble = DatumEnsemble::create(
        PropertyMap(),
        std::vector<DatumNNPtr>{
            VerticalReferenceFrame::create(
                PropertyMap().set(IdentifiedObject::NAME_KEY, "vdatum1")),
            VerticalReferenceFrame::create(
                PropertyMap().set(IdentifiedObject::NAME_KEY, "vdatum2"))},
        PositionalAccuracy::create("100"));
    EXPECT_EQ(
        ensemble->exportToWKT(
            WKTFormatter::create(WKTFormatter::Convention::WKT2_2019).get()),
        "ENSEMBLE[\"unnamed\",\n"
        "    MEMBER[\"vdatum1\"],\n"
        "    MEMBER[\"vdatum2\"],\n"
        "    ENSEMBLEACCURACY[100]]");
}

// ---------------------------------------------------------------------------

TEST(datum, datum_ensemble_exceptions) {
    // No datum
    EXPECT_THROW(DatumEnsemble::create(PropertyMap(), std::vector<DatumNNPtr>{},
                                       PositionalAccuracy::create("100")),
                 Exception);

    // Single datum
    EXPECT_THROW(DatumEnsemble::create(
                     PropertyMap(),
                     std::vector<DatumNNPtr>{GeodeticReferenceFrame::EPSG_6326},
                     PositionalAccuracy::create("100")),
                 Exception);

    auto vdatum = VerticalReferenceFrame::create(
        PropertyMap().set(IdentifiedObject::NAME_KEY, "vdatum1"));

    // Different datum type
    EXPECT_THROW(
        DatumEnsemble::create(
            PropertyMap(),
            std::vector<DatumNNPtr>{GeodeticReferenceFrame::EPSG_6326, vdatum},
            PositionalAccuracy::create("100")),
        Exception);

    // Different datum type
    EXPECT_THROW(
        DatumEnsemble::create(
            PropertyMap(),
            std::vector<DatumNNPtr>{vdatum, GeodeticReferenceFrame::EPSG_6326},
            PositionalAccuracy::create("100")),
        Exception);

    // Different ellipsoid
    EXPECT_THROW(DatumEnsemble::create(
                     PropertyMap(),
                     std::vector<DatumNNPtr>{GeodeticReferenceFrame::EPSG_6326,
                                             GeodeticReferenceFrame::EPSG_6267},
                     PositionalAccuracy::create("100")),
                 Exception);

    // Different prime meridian
    EXPECT_THROW(DatumEnsemble::create(
                     PropertyMap(),
                     std::vector<DatumNNPtr>{
                         GeodeticReferenceFrame::EPSG_6326,
                         GeodeticReferenceFrame::create(
                             PropertyMap().set(IdentifiedObject::NAME_KEY,
                                               "other datum"),
                             Ellipsoid::WGS84, optional<std::string>(),
                             PrimeMeridian::PARIS)},
                     PositionalAccuracy::create("100")),
                 Exception);
}

// ---------------------------------------------------------------------------

TEST(datum, edatum) {
    auto datum = EngineeringDatum::create(
        PropertyMap().set(IdentifiedObject::NAME_KEY, "Engineering datum"),
        optional<std::string>("my anchor"));

    auto expected = "EDATUM[\"Engineering datum\",\n"
                    "    ANCHOR[\"my anchor\"]]";

    EXPECT_EQ(datum->exportToWKT(
                  WKTFormatter::create(WKTFormatter::Convention::WKT2).get()),
              expected);
}

// ---------------------------------------------------------------------------

TEST(datum, pdatum) {
    auto datum = ParametricDatum::create(
        PropertyMap().set(IdentifiedObject::NAME_KEY, "Parametric datum"),
        optional<std::string>("my anchor"));

    auto expected = "PDATUM[\"Parametric datum\",\n"
                    "    ANCHOR[\"my anchor\"]]";

    EXPECT_EQ(datum->exportToWKT(
                  WKTFormatter::create(WKTFormatter::Convention::WKT2).get()),
              expected);
}
