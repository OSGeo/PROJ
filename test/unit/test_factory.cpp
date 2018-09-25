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

#include <sqlite3.h>

using namespace osgeo::proj::common;
using namespace osgeo::proj::crs;
using namespace osgeo::proj::cs;
using namespace osgeo::proj::datum;
using namespace osgeo::proj::io;
using namespace osgeo::proj::metadata;
using namespace osgeo::proj::operation;
using namespace osgeo::proj::util;

namespace {

// ---------------------------------------------------------------------------

TEST(factory, databasecontext_create) {
    DatabaseContext::create();
#ifndef _WIN32
    // For some reason, no exception is thrown on AppVeyor Windows
    EXPECT_THROW(DatabaseContext::create("/i/dont/exist"), FactoryException);
#endif
}

// ---------------------------------------------------------------------------

TEST(factory, AuthorityFactory_createObject) {
    auto factory = AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    EXPECT_THROW(factory->createObject("-1"), NoSuchAuthorityCodeException);
    EXPECT_THROW(factory->createObject("4326"),
                 FactoryException); // area and crs
}

// ---------------------------------------------------------------------------

TEST(factory, AuthorityFactory_createUnitOfMeasure_linear) {
    auto factory = AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    EXPECT_THROW(factory->createUnitOfMeasure("-1"),
                 NoSuchAuthorityCodeException);
    EXPECT_TRUE(nn_dynamic_pointer_cast<UnitOfMeasure>(
                    factory->createObject("9001")) != nullptr);
    auto uom = factory->createUnitOfMeasure("9001");
    EXPECT_EQ(uom->name(), "metre");
    EXPECT_EQ(uom->type(), UnitOfMeasure::Type::LINEAR);
    EXPECT_EQ(uom->conversionToSI(), 1.0);
    EXPECT_EQ(uom->codeSpace(), "EPSG");
    EXPECT_EQ(uom->code(), "9001");
}

// ---------------------------------------------------------------------------

TEST(factory, AuthorityFactory_createUnitOfMeasure_angular) {
    auto factory = AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    auto uom = factory->createUnitOfMeasure("9102");
    EXPECT_EQ(uom->name(), "degree");
    EXPECT_EQ(uom->type(), UnitOfMeasure::Type::ANGULAR);
    EXPECT_EQ(uom->conversionToSI(), 0.0174532925199433);
    EXPECT_EQ(uom->codeSpace(), "EPSG");
    EXPECT_EQ(uom->code(), "9102");
}

// ---------------------------------------------------------------------------

TEST(factory, AuthorityFactory_createUnitOfMeasure_angular_9107) {
    auto factory = AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    auto uom = factory->createUnitOfMeasure("9107");
    EXPECT_EQ(uom->name(), "degree minute second");
    EXPECT_EQ(uom->type(), UnitOfMeasure::Type::ANGULAR);
    EXPECT_EQ(uom->conversionToSI(), UnitOfMeasure::DEGREE.conversionToSI());
    EXPECT_EQ(uom->codeSpace(), "EPSG");
    EXPECT_EQ(uom->code(), "9107");
}

// ---------------------------------------------------------------------------

TEST(factory, AuthorityFactory_createUnitOfMeasure_scale) {
    auto factory = AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    auto uom = factory->createUnitOfMeasure("1028");
    EXPECT_EQ(uom->name(), "parts per billion");
    EXPECT_EQ(uom->type(), UnitOfMeasure::Type::SCALE);
    EXPECT_EQ(uom->conversionToSI(), 1e-9);
    EXPECT_EQ(uom->codeSpace(), "EPSG");
    EXPECT_EQ(uom->code(), "1028");
}

// ---------------------------------------------------------------------------

TEST(factory, AuthorityFactory_createUnitOfMeasure_time) {
    auto factory = AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    auto uom = factory->createUnitOfMeasure("1029");
    EXPECT_EQ(uom->name(), "year");
    EXPECT_EQ(uom->type(), UnitOfMeasure::Type::TIME);
    EXPECT_EQ(uom->conversionToSI(), 31556925.445);
    EXPECT_EQ(uom->codeSpace(), "EPSG");
    EXPECT_EQ(uom->code(), "1029");
}

// ---------------------------------------------------------------------------

TEST(factory, AuthorityFactory_createPrimeMeridian) {
    auto factory = AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    EXPECT_THROW(factory->createPrimeMeridian("-1"),
                 NoSuchAuthorityCodeException);
    EXPECT_TRUE(nn_dynamic_pointer_cast<PrimeMeridian>(
                    factory->createObject("8903")) != nullptr);
    auto pm = factory->createPrimeMeridian("8903");
    ASSERT_EQ(pm->identifiers().size(), 1);
    EXPECT_EQ(pm->identifiers()[0]->code(), "8903");
    EXPECT_EQ(*(pm->identifiers()[0]->codeSpace()), "EPSG");
    EXPECT_EQ(*(pm->name()->description()), "Paris");
    EXPECT_EQ(pm->longitude(), Angle(2.5969213, UnitOfMeasure::GRAD));
}

// ---------------------------------------------------------------------------

TEST(factory, AuthorityFactory_createEllipsoid) {
    auto factory = AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    EXPECT_THROW(factory->createEllipsoid("-1"), NoSuchAuthorityCodeException);
    EXPECT_TRUE(nn_dynamic_pointer_cast<Ellipsoid>(
                    factory->createObject("7030")) != nullptr);
    auto ellipsoid = factory->createEllipsoid("7030");
    ASSERT_EQ(ellipsoid->identifiers().size(), 1);
    EXPECT_EQ(ellipsoid->identifiers()[0]->code(), "7030");
    EXPECT_EQ(*(ellipsoid->identifiers()[0]->codeSpace()), "EPSG");
    EXPECT_EQ(*(ellipsoid->name()->description()), "WGS 84");
    EXPECT_TRUE(ellipsoid->inverseFlattening().has_value());
    EXPECT_EQ(ellipsoid->semiMajorAxis(), Length(6378137));
    EXPECT_EQ(*ellipsoid->inverseFlattening(), Scale(298.257223563));
}

// ---------------------------------------------------------------------------

TEST(factory, AuthorityFactory_createEllipsoid_sphere) {
    auto factory = AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    auto ellipsoid = factory->createEllipsoid("7035");
    EXPECT_TRUE(ellipsoid->isSphere());
    EXPECT_EQ(ellipsoid->semiMajorAxis(), Length(6371000));
}

// ---------------------------------------------------------------------------

TEST(factory, AuthorityFactory_createEllipsoid_with_semi_minor_axis) {
    auto factory = AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    auto ellipsoid = factory->createEllipsoid("7011");
    EXPECT_TRUE(ellipsoid->semiMinorAxis().has_value());
    EXPECT_EQ(ellipsoid->semiMajorAxis(), Length(6378249.2));
    EXPECT_EQ(*ellipsoid->semiMinorAxis(), Length(6356515.0));
}

// ---------------------------------------------------------------------------

TEST(factory, AuthorityFactory_createExtent) {
    auto factory = AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    EXPECT_THROW(factory->createExtent("-1"), NoSuchAuthorityCodeException);
    auto extent = factory->createExtent("1262");
    EXPECT_EQ(*(extent->description()), "World");
    const auto &geogElts = extent->geographicElements();
    ASSERT_EQ(geogElts.size(), 1);
    auto bbox = nn_dynamic_pointer_cast<GeographicBoundingBox>(geogElts[0]);
    ASSERT_TRUE(bbox != nullptr);
    EXPECT_EQ(bbox->westBoundLongitude(), -180);
    EXPECT_EQ(bbox->eastBoundLongitude(), 180);
    EXPECT_EQ(bbox->northBoundLatitude(), 90);
    EXPECT_EQ(bbox->southBoundLatitude(), -90);
}

// ---------------------------------------------------------------------------

TEST(factory, AuthorityFactory_createGeodeticDatum) {
    auto factory = AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    EXPECT_THROW(factory->createGeodeticDatum("-1"),
                 NoSuchAuthorityCodeException);
    auto grf = factory->createGeodeticDatum("6326");
    ASSERT_EQ(grf->identifiers().size(), 1);
    EXPECT_EQ(grf->identifiers()[0]->code(), "6326");
    EXPECT_EQ(*(grf->identifiers()[0]->codeSpace()), "EPSG");
    EXPECT_EQ(*(grf->name()->description()), "World Geodetic System 1984");
    EXPECT_TRUE(
        grf->ellipsoid()->isEquivalentTo(factory->createEllipsoid("7030")));
    EXPECT_TRUE(grf->primeMeridian()->isEquivalentTo(
        factory->createPrimeMeridian("8901")));
    ASSERT_EQ(grf->domains().size(), 1);
    auto domain = grf->domains()[0];
    auto extent = domain->domainOfValidity();
    ASSERT_TRUE(extent != nullptr);
    EXPECT_TRUE(extent->isEquivalentTo(factory->createExtent("1262")));
}

// ---------------------------------------------------------------------------

TEST(factory, AuthorityFactory_createVerticalDatum) {
    auto factory = AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    EXPECT_THROW(factory->createVerticalDatum("-1"),
                 NoSuchAuthorityCodeException);
    auto vrf = factory->createVerticalDatum("1027");
    ASSERT_EQ(vrf->identifiers().size(), 1);
    EXPECT_EQ(vrf->identifiers()[0]->code(), "1027");
    EXPECT_EQ(*(vrf->identifiers()[0]->codeSpace()), "EPSG");
    EXPECT_EQ(*(vrf->name()->description()), "EGM2008 geoid");
    auto domain = vrf->domains()[0];
    auto extent = domain->domainOfValidity();
    ASSERT_TRUE(extent != nullptr);
    EXPECT_TRUE(extent->isEquivalentTo(factory->createExtent("1262")));
}

// ---------------------------------------------------------------------------

TEST(factory, AuthorityFactory_createDatum) {
    auto factory = AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    EXPECT_THROW(factory->createDatum("-1"), NoSuchAuthorityCodeException);
    EXPECT_TRUE(factory->createDatum("6326")->isEquivalentTo(
        factory->createGeodeticDatum("6326")));
    EXPECT_TRUE(factory->createDatum("1027")->isEquivalentTo(
        factory->createVerticalDatum("1027")));
}

// ---------------------------------------------------------------------------

TEST(factory, AuthorityFactory_createCoordinateSystem_ellipsoidal_2_axis) {
    auto factory = AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    EXPECT_THROW(factory->createCoordinateSystem("-1"),
                 NoSuchAuthorityCodeException);
    auto cs = factory->createCoordinateSystem("6422");
    auto ellipsoidal_cs = nn_dynamic_pointer_cast<EllipsoidalCS>(cs);
    ASSERT_TRUE(ellipsoidal_cs != nullptr);

    ASSERT_EQ(ellipsoidal_cs->identifiers().size(), 1);
    EXPECT_EQ(ellipsoidal_cs->identifiers()[0]->code(), "6422");
    EXPECT_EQ(*(ellipsoidal_cs->identifiers()[0]->codeSpace()), "EPSG");

    const auto &axisList = ellipsoidal_cs->axisList();
    EXPECT_EQ(axisList.size(), 2);

    EXPECT_EQ(*(axisList[0]->name()->description()), "Geodetic latitude");
    EXPECT_EQ(axisList[0]->abbreviation(), "Lat");
    EXPECT_EQ(axisList[0]->direction(), AxisDirection::NORTH);
    EXPECT_EQ(axisList[0]->unit(), UnitOfMeasure::DEGREE);

    EXPECT_EQ(*(axisList[1]->name()->description()), "Geodetic longitude");
    EXPECT_EQ(axisList[1]->abbreviation(), "Lon");
    EXPECT_EQ(axisList[1]->direction(), AxisDirection::EAST);
    EXPECT_EQ(axisList[1]->unit(), UnitOfMeasure::DEGREE);
}

// ---------------------------------------------------------------------------

TEST(factory, AuthorityFactory_createCoordinateSystem_ellipsoidal_3_axis) {
    auto factory = AuthorityFactory::create(DatabaseContext::create(), "EPSG");

    auto cs = factory->createCoordinateSystem("6423");
    auto ellipsoidal_cs = nn_dynamic_pointer_cast<EllipsoidalCS>(cs);
    ASSERT_TRUE(ellipsoidal_cs != nullptr);

    ASSERT_EQ(ellipsoidal_cs->identifiers().size(), 1);
    EXPECT_EQ(ellipsoidal_cs->identifiers()[0]->code(), "6423");
    EXPECT_EQ(*(ellipsoidal_cs->identifiers()[0]->codeSpace()), "EPSG");

    const auto &axisList = ellipsoidal_cs->axisList();
    EXPECT_EQ(axisList.size(), 3);

    EXPECT_EQ(*(axisList[0]->name()->description()), "Geodetic latitude");
    EXPECT_EQ(axisList[0]->abbreviation(), "Lat");
    EXPECT_EQ(axisList[0]->direction(), AxisDirection::NORTH);
    EXPECT_EQ(axisList[0]->unit(), UnitOfMeasure::DEGREE);

    EXPECT_EQ(*(axisList[1]->name()->description()), "Geodetic longitude");
    EXPECT_EQ(axisList[1]->abbreviation(), "Lon");
    EXPECT_EQ(axisList[1]->direction(), AxisDirection::EAST);
    EXPECT_EQ(axisList[1]->unit(), UnitOfMeasure::DEGREE);

    EXPECT_EQ(*(axisList[2]->name()->description()), "Ellipsoidal height");
    EXPECT_EQ(axisList[2]->abbreviation(), "h");
    EXPECT_EQ(axisList[2]->direction(), AxisDirection::UP);
    EXPECT_EQ(axisList[2]->unit(), UnitOfMeasure::METRE);
}

// ---------------------------------------------------------------------------

TEST(factory, AuthorityFactory_createCoordinateSystem_geocentric) {
    auto factory = AuthorityFactory::create(DatabaseContext::create(), "EPSG");

    auto cs = factory->createCoordinateSystem("6500");
    auto cartesian_cs = nn_dynamic_pointer_cast<CartesianCS>(cs);
    ASSERT_TRUE(cartesian_cs != nullptr);

    ASSERT_EQ(cartesian_cs->identifiers().size(), 1);
    EXPECT_EQ(cartesian_cs->identifiers()[0]->code(), "6500");
    EXPECT_EQ(*(cartesian_cs->identifiers()[0]->codeSpace()), "EPSG");

    const auto &axisList = cartesian_cs->axisList();
    EXPECT_EQ(axisList.size(), 3);

    EXPECT_EQ(*(axisList[0]->name()->description()), "Geocentric X");
    EXPECT_EQ(axisList[0]->abbreviation(), "X");
    EXPECT_EQ(axisList[0]->direction(), AxisDirection::GEOCENTRIC_X);
    EXPECT_EQ(axisList[0]->unit(), UnitOfMeasure::METRE);

    EXPECT_EQ(*(axisList[1]->name()->description()), "Geocentric Y");
    EXPECT_EQ(axisList[1]->abbreviation(), "Y");
    EXPECT_EQ(axisList[1]->direction(), AxisDirection::GEOCENTRIC_Y);
    EXPECT_EQ(axisList[1]->unit(), UnitOfMeasure::METRE);

    EXPECT_EQ(*(axisList[2]->name()->description()), "Geocentric Z");
    EXPECT_EQ(axisList[2]->abbreviation(), "Z");
    EXPECT_EQ(axisList[2]->direction(), AxisDirection::GEOCENTRIC_Z);
    EXPECT_EQ(axisList[2]->unit(), UnitOfMeasure::METRE);
}

// ---------------------------------------------------------------------------

TEST(factory, AuthorityFactory_createCoordinateSystem_vertical) {
    auto factory = AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    EXPECT_THROW(factory->createCoordinateSystem("-1"),
                 NoSuchAuthorityCodeException);

    auto cs = factory->createCoordinateSystem("6499");
    auto vertical_cs = nn_dynamic_pointer_cast<VerticalCS>(cs);
    ASSERT_TRUE(vertical_cs != nullptr);

    ASSERT_EQ(vertical_cs->identifiers().size(), 1);
    EXPECT_EQ(vertical_cs->identifiers()[0]->code(), "6499");
    EXPECT_EQ(*(vertical_cs->identifiers()[0]->codeSpace()), "EPSG");

    const auto &axisList = vertical_cs->axisList();
    EXPECT_EQ(axisList.size(), 1);

    EXPECT_EQ(*(axisList[0]->name()->description()), "Gravity-related height");
    EXPECT_EQ(axisList[0]->abbreviation(), "H");
    EXPECT_EQ(axisList[0]->direction(), AxisDirection::UP);
    EXPECT_EQ(axisList[0]->unit(), UnitOfMeasure::METRE);
}

// ---------------------------------------------------------------------------

TEST(factory, AuthorityFactory_createGeodeticCRS_geographic2D) {
    auto factory = AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    EXPECT_THROW(factory->createGeodeticCRS("-1"),
                 NoSuchAuthorityCodeException);
    auto crs = factory->createGeodeticCRS("4326");
    auto gcrs = nn_dynamic_pointer_cast<GeographicCRS>(crs);
    ASSERT_TRUE(gcrs != nullptr);
    ASSERT_EQ(gcrs->identifiers().size(), 1);
    EXPECT_EQ(gcrs->identifiers()[0]->code(), "4326");
    EXPECT_EQ(*(gcrs->identifiers()[0]->codeSpace()), "EPSG");
    EXPECT_EQ(*(gcrs->name()->description()), "WGS 84");
    EXPECT_TRUE(gcrs->datum()->isEquivalentTo(factory->createDatum("6326")));
    EXPECT_TRUE(gcrs->coordinateSystem()->isEquivalentTo(
        factory->createCoordinateSystem("6422")));
    auto domain = crs->domains()[0];
    auto extent = domain->domainOfValidity();
    ASSERT_TRUE(extent != nullptr);
    EXPECT_TRUE(extent->isEquivalentTo(factory->createExtent("1262")));
}

// ---------------------------------------------------------------------------

TEST(factory, AuthorityFactory_createGeodeticCRS_geographic3D) {
    auto factory = AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    auto crs = factory->createGeodeticCRS("4979");
    auto gcrs = nn_dynamic_pointer_cast<GeographicCRS>(crs);
    ASSERT_TRUE(gcrs != nullptr);
    ASSERT_EQ(gcrs->identifiers().size(), 1);
    EXPECT_EQ(gcrs->identifiers()[0]->code(), "4979");
    EXPECT_EQ(*(gcrs->identifiers()[0]->codeSpace()), "EPSG");
    EXPECT_EQ(*(gcrs->name()->description()), "WGS 84");
    EXPECT_TRUE(gcrs->datum()->isEquivalentTo(factory->createDatum("6326")));
    EXPECT_TRUE(gcrs->coordinateSystem()->isEquivalentTo(
        factory->createCoordinateSystem("6423")));
}

// ---------------------------------------------------------------------------

TEST(factory, AuthorityFactory_createGeodeticCRS_geocentric) {
    auto factory = AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    auto crs = factory->createGeodeticCRS("4978");
    ASSERT_TRUE(nn_dynamic_pointer_cast<GeographicCRS>(crs) == nullptr);
    ASSERT_EQ(crs->identifiers().size(), 1);
    EXPECT_EQ(crs->identifiers()[0]->code(), "4978");
    EXPECT_EQ(*(crs->identifiers()[0]->codeSpace()), "EPSG");
    EXPECT_EQ(*(crs->name()->description()), "WGS 84");
    EXPECT_TRUE(crs->datum()->isEquivalentTo(factory->createDatum("6326")));
    EXPECT_TRUE(crs->coordinateSystem()->isEquivalentTo(
        factory->createCoordinateSystem("6500")));
}

// ---------------------------------------------------------------------------

TEST(factory, AuthorityFactory_createVerticalCRS) {
    auto factory = AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    EXPECT_THROW(factory->createVerticalCRS("-1"),
                 NoSuchAuthorityCodeException);

    auto crs = factory->createVerticalCRS("3855");
    ASSERT_EQ(crs->identifiers().size(), 1);
    EXPECT_EQ(crs->identifiers()[0]->code(), "3855");
    EXPECT_EQ(*(crs->identifiers()[0]->codeSpace()), "EPSG");
    EXPECT_EQ(*(crs->name()->description()), "EGM2008 height");
    EXPECT_TRUE(crs->datum()->isEquivalentTo(factory->createDatum("1027")));
    EXPECT_TRUE(crs->coordinateSystem()->isEquivalentTo(
        factory->createCoordinateSystem("6499")));

    auto domain = crs->domains()[0];
    auto extent = domain->domainOfValidity();
    ASSERT_TRUE(extent != nullptr);
    EXPECT_TRUE(extent->isEquivalentTo(factory->createExtent("1262")));
}

// ---------------------------------------------------------------------------

TEST(factory, AuthorityFactory_createConversion) {
    auto factory = AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    EXPECT_THROW(factory->createConversion("-1"), NoSuchAuthorityCodeException);

    auto conv = factory->createConversion("16031");
    ASSERT_EQ(conv->identifiers().size(), 1);
    EXPECT_EQ(conv->identifiers()[0]->code(), "16031");
    EXPECT_EQ(*(conv->identifiers()[0]->codeSpace()), "EPSG");
    EXPECT_EQ(*(conv->name()->description()), "UTM zone 31N");

    auto method = conv->method();
    ASSERT_EQ(method->identifiers().size(), 1);
    EXPECT_EQ(method->identifiers()[0]->code(), "9807");
    EXPECT_EQ(*(method->identifiers()[0]->codeSpace()), "EPSG");
    EXPECT_EQ(*(method->name()->description()), "Transverse Mercator");

    const auto &values = conv->parameterValues();
    ASSERT_EQ(values.size(), 5);
    {
        const auto &opParamvalue =
            nn_dynamic_pointer_cast<OperationParameterValue>(values[0]);
        ASSERT_TRUE(opParamvalue);
        const auto &paramName =
            *(opParamvalue->parameter()->name()->description());
        const auto &parameterValue = opParamvalue->parameterValue();
        EXPECT_TRUE(opParamvalue->parameter()->isEPSG(8801));
        EXPECT_EQ(paramName, "Latitude of natural origin");
        EXPECT_EQ(parameterValue->type(), ParameterValue::Type::MEASURE);
        auto measure = parameterValue->value();
        EXPECT_EQ(measure.unit(), UnitOfMeasure::DEGREE);
        EXPECT_EQ(measure.value(), 0.0);
    }
    {
        const auto &opParamvalue =
            nn_dynamic_pointer_cast<OperationParameterValue>(values[1]);
        ASSERT_TRUE(opParamvalue);
        const auto &paramName =
            *(opParamvalue->parameter()->name()->description());
        const auto &parameterValue = opParamvalue->parameterValue();
        EXPECT_TRUE(opParamvalue->parameter()->isEPSG(8802));
        EXPECT_EQ(paramName, "Longitude of natural origin");
        EXPECT_EQ(parameterValue->type(), ParameterValue::Type::MEASURE);
        auto measure = parameterValue->value();
        EXPECT_EQ(measure.unit(), UnitOfMeasure::DEGREE);
        EXPECT_EQ(measure.value(), 3.0);
    }
}

// ---------------------------------------------------------------------------

TEST(factory, AuthorityFactory_createProjectedCRS) {
    auto factory = AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    EXPECT_THROW(factory->createProjectedCRS("-1"),
                 NoSuchAuthorityCodeException);

    auto crs = factory->createProjectedCRS("32631");
    ASSERT_EQ(crs->identifiers().size(), 1);
    EXPECT_EQ(crs->identifiers()[0]->code(), "32631");
    EXPECT_EQ(*(crs->identifiers()[0]->codeSpace()), "EPSG");
    EXPECT_EQ(*(crs->name()->description()), "WGS 84 / UTM zone 31N");
    EXPECT_TRUE(
        crs->baseCRS()->isEquivalentTo(factory->createGeodeticCRS("4326")));
    EXPECT_TRUE(crs->coordinateSystem()->isEquivalentTo(
        factory->createCoordinateSystem("4400")));
    EXPECT_TRUE(crs->derivingConversion()->isEquivalentTo(
        factory->createConversion("16031")));

    auto domain = crs->domains()[0];
    auto extent = domain->domainOfValidity();
    ASSERT_TRUE(extent != nullptr);
    EXPECT_TRUE(extent->isEquivalentTo(factory->createExtent("2060")));
}

// ---------------------------------------------------------------------------

TEST(factory, AuthorityFactory_createCompoundCRS) {
    auto factory = AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    EXPECT_THROW(factory->createCompoundCRS("-1"),
                 NoSuchAuthorityCodeException);

    auto crs = factory->createCompoundCRS("6871");
    ASSERT_EQ(crs->identifiers().size(), 1);
    EXPECT_EQ(crs->identifiers()[0]->code(), "6871");
    EXPECT_EQ(*(crs->identifiers()[0]->codeSpace()), "EPSG");
    EXPECT_EQ(*(crs->name()->description()),
              "WGS 84 / Pseudo-Mercator +  EGM2008 geoid height");

    auto components = crs->componentReferenceSystems();
    ASSERT_EQ(components.size(), 2);
    EXPECT_TRUE(
        components[0]->isEquivalentTo(factory->createProjectedCRS("3857")));
    EXPECT_TRUE(
        components[1]->isEquivalentTo(factory->createVerticalCRS("3855")));

    auto domain = crs->domains()[0];
    auto extent = domain->domainOfValidity();
    ASSERT_TRUE(extent != nullptr);
    EXPECT_TRUE(extent->isEquivalentTo(factory->createExtent("1262")));
}

// ---------------------------------------------------------------------------

TEST(factory, AuthorityFactory_createCoordinateReferenceSystem) {
    auto factory = AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    EXPECT_THROW(factory->createCoordinateReferenceSystem("-1"),
                 NoSuchAuthorityCodeException);
    EXPECT_TRUE(nn_dynamic_pointer_cast<GeographicCRS>(
        factory->createCoordinateReferenceSystem("4326")));
    EXPECT_TRUE(nn_dynamic_pointer_cast<GeographicCRS>(
        factory->createCoordinateReferenceSystem("4979")));
    EXPECT_TRUE(nn_dynamic_pointer_cast<GeodeticCRS>(
        factory->createCoordinateReferenceSystem("4978")));
    EXPECT_TRUE(nn_dynamic_pointer_cast<ProjectedCRS>(
        factory->createCoordinateReferenceSystem("32631")));
    EXPECT_TRUE(nn_dynamic_pointer_cast<VerticalCRS>(
        factory->createCoordinateReferenceSystem("3855")));
    EXPECT_TRUE(nn_dynamic_pointer_cast<CompoundCRS>(
        factory->createCoordinateReferenceSystem("6871")));
}

// ---------------------------------------------------------------------------

class FactoryWithTmpDatabase : public ::testing::Test {
  protected:
    void SetUp() override { sqlite3_open(":memory:", &m_ctxt); }

    void TearDown() override {
        sqlite3_free_table(m_papszResult);
        sqlite3_close(m_ctxt);
    }

    void createStructure() {
        auto referenceDb = DatabaseContext::create();
        auto referenceDbHandle =
            static_cast<sqlite3 *>(referenceDb->getSqliteHandle());
        ASSERT_TRUE(m_ctxt != nullptr);
        ASSERT_TRUE(get_table(
            "SELECT sql FROM sqlite_master WHERE type IN ('table', 'view')",
            referenceDbHandle));
        for (int i = 0; i < m_nRows; i++) {
            ASSERT_TRUE(execute(m_papszResult[i + 1])) << last_error();
        }
    }

    bool get_table(const char *sql, sqlite3 *db = nullptr) {
        sqlite3_free_table(m_papszResult);
        m_papszResult = nullptr;
        m_nRows = 0;
        m_nCols = 0;
        return sqlite3_get_table(db ? db : m_ctxt, sql, &m_papszResult,
                                 &m_nRows, &m_nCols, nullptr) == SQLITE_OK;
    }

    bool execute(const char *sql) {
        return sqlite3_exec(m_ctxt, sql, nullptr, nullptr, nullptr) ==
               SQLITE_OK;
    }

    std::string last_error() {
        const char *msg = sqlite3_errmsg(m_ctxt);
        return msg ? msg : std::string();
    }

    int m_nRows = 0;
    int m_nCols = 0;
    char **m_papszResult = nullptr;
    sqlite3 *m_ctxt = nullptr;
};

// ---------------------------------------------------------------------------

TEST_F(FactoryWithTmpDatabase, AuthorityFactory_test_with_fake_database) {
    createStructure();
    ASSERT_TRUE(execute("PRAGMA foreign_keys = 1;")) << last_error();
    ASSERT_TRUE(execute("INSERT INTO unit_of_measure "
                        "VALUES('EPSG','9001','metre','length',1.0,0);"))
        << last_error();
    ASSERT_TRUE(execute("INSERT INTO unit_of_measure "
                        "VALUES('EPSG','9102','degree','angle',1."
                        "74532925199432781271e-02,0);"))
        << last_error();
    ASSERT_TRUE(execute(
        "INSERT INTO unit_of_measure VALUES('EPSG','9122','degree (supplier to "
        "define representation)','angle',1.74532925199432781271e-02,0);"))
        << last_error();
    ASSERT_TRUE(execute("INSERT INTO area "
                        "VALUES('EPSG','1262','World','World.',-90.0,90.0,-180."
                        "0,180.0,0);"))
        << last_error();
    ASSERT_TRUE(
        execute("INSERT INTO prime_meridian "
                "VALUES('EPSG','8901','Greenwich',0.0,'EPSG','9102',0);"))
        << last_error();
    ASSERT_TRUE(execute("INSERT INTO ellipsoid VALUES('EPSG','7030','WGS "
                        "84',6378137.0,'EPSG','9001',298.257223563,NULL,0);"))
        << last_error();
    ASSERT_TRUE(execute("INSERT INTO geodetic_datum "
                        "VALUES('EPSG','6326','World Geodetic System "
                        "1984','EPSG','7030','EPSG','8901','EPSG','1262',0);"))
        << last_error();
    ASSERT_TRUE(
        execute("INSERT INTO vertical_datum VALUES('EPSG','1027','EGM2008 "
                "geoid','EPSG','1262',0);"))
        << last_error();
    ASSERT_TRUE(execute(
        "INSERT INTO coordinate_system VALUES('EPSG','6422','ellipsoidal',2);"))
        << last_error();
    ASSERT_TRUE(
        execute("INSERT INTO axis VALUES('EPSG','106','Geodetic "
                "latitude','Lat','north','EPSG','6422',1,'EPSG','9122');"))
        << last_error();
    ASSERT_TRUE(
        execute("INSERT INTO axis VALUES('EPSG','107','Geodetic "
                "longitude','Lon','east','EPSG','6422',2,'EPSG','9122');"))
        << last_error();
    ASSERT_TRUE(
        execute("INSERT INTO crs VALUES('EPSG','4326','geographic 2D');"))
        << last_error();
    ASSERT_TRUE(execute("INSERT INTO geodetic_crs VALUES('EPSG','4326','WGS "
                        "84','geographic "
                        "2D','EPSG','6422','EPSG','6326','EPSG','1262',0);"))
        << last_error();

    ASSERT_TRUE(execute(
        "INSERT INTO coordinate_system VALUES('EPSG','6499','vertical',1);"))
        << last_error();
    ASSERT_TRUE(execute("INSERT INTO axis VALUES('EPSG','114','Gravity-related "
                        "height','H','up','EPSG','6499',1,'EPSG','9001');"))
        << last_error();
    ASSERT_TRUE(execute("INSERT INTO crs VALUES('EPSG','3855','vertical');"))
        << last_error();
    ASSERT_TRUE(
        execute("INSERT INTO vertical_crs VALUES('EPSG','3855','EGM2008 "
                "height','EPSG','6499','EPSG','1027','EPSG','1262',0);"))
        << last_error();

    ASSERT_TRUE(execute("INSERT INTO unit_of_measure "
                        "VALUES('EPSG','9201','unity','scale',1.0,0);"))
        << last_error();
    ASSERT_TRUE(execute("INSERT INTO coordinate_operation "
                        "VALUES('EPSG','16031','conversion');"))
        << last_error();
    ASSERT_TRUE(execute(
        "INSERT INTO conversion VALUES('EPSG','16031','UTM zone "
        "31N','EPSG','9807','Transverse Mercator','EPSG','8801','Latitude of "
        "natural origin',0.0,'EPSG','9102','EPSG','8802','Longitude of natural "
        "origin',3.0,'EPSG','9102','EPSG','8805','Scale factor at natural "
        "origin',0.9996,'EPSG','9201','EPSG','8806','False "
        "easting',500000.0,'EPSG','9001','EPSG','8807','False "
        "northing',0.0,'EPSG','9001',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,"
        "NULL,NULL,NULL,NULL);"))
        << last_error();

    ASSERT_TRUE(
        execute("INSERT INTO area VALUES('EPSG','2060','World - N hemisphere - "
                "0°E to 6°E - by country','',0.0,84.0,0.0,6.0,0);"))
        << last_error();
    ASSERT_TRUE(execute(
        "INSERT INTO coordinate_system VALUES('EPSG','4400','Cartesian',2);"))
        << last_error();
    ASSERT_TRUE(execute("INSERT INTO axis "
                        "VALUES('EPSG','1','Easting','E','east','EPSG','4400',"
                        "1,'EPSG','9001');"))
        << last_error();
    ASSERT_TRUE(execute("INSERT INTO axis "
                        "VALUES('EPSG','2','Northing','N','north','EPSG','4400'"
                        ",2,'EPSG','9001');"))
        << last_error();
    ASSERT_TRUE(execute("INSERT INTO crs VALUES('EPSG','32631','projected');"))
        << last_error();
    ASSERT_TRUE(execute(
        "INSERT INTO projected_crs VALUES('EPSG','32631','WGS 84 / UTM zone "
        "31N','EPSG','4400','EPSG','4326','EPSG','16031','EPSG','2060',0);"))
        << last_error();

    ASSERT_TRUE(
        execute("INSERT INTO crs VALUES('EPSG','MY_COMPOUND','compound');"))
        << last_error();
    ASSERT_TRUE(execute(
        "INSERT INTO compound_crs VALUES('EPSG','MY_COMPOUND','WGS 84 + "
        "EGM2008 geoid height','EPSG','4326','EPSG','3855','EPSG','1262',0);"))
        << last_error();

    auto factory =
        AuthorityFactory::create(DatabaseContext::create(m_ctxt), "EPSG");

    EXPECT_TRUE(nn_dynamic_pointer_cast<UnitOfMeasure>(
                    factory->createObject("9001")) != nullptr);

    EXPECT_TRUE(nn_dynamic_pointer_cast<Extent>(
                    factory->createObject("1262")) != nullptr);

    EXPECT_TRUE(nn_dynamic_pointer_cast<PrimeMeridian>(
                    factory->createObject("8901")) != nullptr);

    EXPECT_TRUE(nn_dynamic_pointer_cast<Ellipsoid>(
                    factory->createObject("7030")) != nullptr);

    EXPECT_TRUE(nn_dynamic_pointer_cast<GeodeticReferenceFrame>(
                    factory->createObject("6326")) != nullptr);

    EXPECT_TRUE(nn_dynamic_pointer_cast<VerticalReferenceFrame>(
                    factory->createObject("1027")) != nullptr);

    EXPECT_TRUE(nn_dynamic_pointer_cast<GeographicCRS>(
                    factory->createObject("4326")) != nullptr);

    EXPECT_TRUE(nn_dynamic_pointer_cast<VerticalCRS>(
                    factory->createObject("3855")) != nullptr);

    EXPECT_TRUE(nn_dynamic_pointer_cast<Conversion>(
                    factory->createObject("16031")) != nullptr);

    EXPECT_TRUE(nn_dynamic_pointer_cast<ProjectedCRS>(
                    factory->createObject("32631")) != nullptr);

    EXPECT_TRUE(nn_dynamic_pointer_cast<CompoundCRS>(
                    factory->createObject("MY_COMPOUND")) != nullptr);
}

} // namespace
