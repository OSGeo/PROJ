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
    EXPECT_EQ(uom->conversionToSI(), UnitOfMeasure::DEGREE.conversionToSI());
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

    EXPECT_EQ(crs->exportToPROJString(PROJStringFormatter::create()),
              "+proj=pipeline +step +proj=longlat +ellps=WGS84 +step "
              "+proj=unitconvert +xy_in=rad +xy_out=deg +step +proj=axisswap "
              "+order=2,1");
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

TEST(factory, AuthorityFactory_createCoordinateOperation_helmert_3) {
    auto factory = AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    EXPECT_THROW(factory->createCoordinateOperation("-1"),
                 NoSuchAuthorityCodeException);
    auto op = factory->createCoordinateOperation("1113");
    EXPECT_EQ(op->exportToPROJString(PROJStringFormatter::create()),
              "+proj=pipeline +step +proj=axisswap +order=2,1 +step "
              "+proj=unitconvert +xy_in=deg +xy_out=rad +step +inv "
              "+proj=longlat +a=6378249.145 +rf=293.4663077 +step +proj=cart "
              "+a=6378249.145 +rf=293.4663077 +step +proj=helmert +x=-143 "
              "+y=-90 +z=-294 +step +inv +proj=cart +ellps=WGS84 +step "
              "+proj=unitconvert +xy_in=rad +xy_out=deg +step +proj=axisswap "
              "+order=2,1");
}

// ---------------------------------------------------------------------------

TEST(factory, AuthorityFactory_createCoordinateOperation_helmert_7) {
    auto factory = AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    auto op = factory->createCoordinateOperation("7676");
    EXPECT_EQ(op->exportToPROJString(PROJStringFormatter::create()),
              "+proj=pipeline +step +proj=axisswap +order=2,1 +step "
              "+proj=unitconvert +xy_in=deg +xy_out=rad +step +proj=cart "
              "+ellps=bessel +step +proj=helmert +x=577.88891 +y=165.22205 "
              "+z=391.18289 +rx=-4.9145 +ry=0.94729 +rz=13.05098 +s=7.78664 "
              "+convention=coordinate_frame +step +inv +proj=cart +ellps=WGS84 "
              "+step +proj=unitconvert +xy_in=rad +xy_out=deg +step "
              "+proj=axisswap +order=2,1");
}

// ---------------------------------------------------------------------------

TEST(factory, AuthorityFactory_createCoordinateOperation_helmert_8) {
    auto factory = AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    auto op = factory->createCoordinateOperation("7702");
    auto expected = "    PARAMETER[\"Transformation reference epoch\",2002,\n"
                    "        TIMEUNIT[\"year\",31556925.445],\n"
                    "        ID[\"EPSG\",1049]],\n";

    auto wkt = op->exportToWKT(
        WKTFormatter::create(WKTFormatter::Convention::WKT2_2018));
    EXPECT_TRUE(wkt.find(expected) != std::string::npos) << wkt;
}

// ---------------------------------------------------------------------------

TEST(factory, AuthorityFactory_createCoordinateOperation_helmert_15) {
    auto factory = AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    auto op = factory->createCoordinateOperation("6276");
    auto expected =
        "COORDINATEOPERATION[\"ITRF2008 to GDA94 (1)\",\n"
        "    SOURCECRS[\n"
        "        GEODCRS[\"ITRF2008\",\n"
        "            DATUM[\"International Terrestrial Reference Frame "
        "2008\",\n"
        "                ELLIPSOID[\"GRS 1980\",6378137,298.257222101,\n"
        "                    LENGTHUNIT[\"metre\",1]]],\n"
        "            PRIMEM[\"Greenwich\",0,\n"
        "                ANGLEUNIT[\"degree\",0.0174532925199433]],\n"
        "            CS[Cartesian,3],\n"
        "                AXIS[\"(X)\",geocentricX,\n"
        "                    ORDER[1],\n"
        "                    LENGTHUNIT[\"metre\",1]],\n"
        "                AXIS[\"(Y)\",geocentricY,\n"
        "                    ORDER[2],\n"
        "                    LENGTHUNIT[\"metre\",1]],\n"
        "                AXIS[\"(Z)\",geocentricZ,\n"
        "                    ORDER[3],\n"
        "                    LENGTHUNIT[\"metre\",1]]]],\n"
        "    TARGETCRS[\n"
        "        GEODCRS[\"GDA94\",\n"
        "            DATUM[\"Geocentric Datum of Australia 1994\",\n"
        "                ELLIPSOID[\"GRS 1980\",6378137,298.257222101,\n"
        "                    LENGTHUNIT[\"metre\",1]]],\n"
        "            PRIMEM[\"Greenwich\",0,\n"
        "                ANGLEUNIT[\"degree\",0.0174532925199433]],\n"
        "            CS[Cartesian,3],\n"
        "                AXIS[\"(X)\",geocentricX,\n"
        "                    ORDER[1],\n"
        "                    LENGTHUNIT[\"metre\",1]],\n"
        "                AXIS[\"(Y)\",geocentricY,\n"
        "                    ORDER[2],\n"
        "                    LENGTHUNIT[\"metre\",1]],\n"
        "                AXIS[\"(Z)\",geocentricZ,\n"
        "                    ORDER[3],\n"
        "                    LENGTHUNIT[\"metre\",1]]]],\n"
        "    METHOD[\"Time-dependent Coordinate Frame rotation (geocen)\",\n"
        "        ID[\"EPSG\",1056]],\n"
        "    PARAMETER[\"X-axis translation\",-84.68,\n"
        "        LENGTHUNIT[\"millimetre\",0.001],\n"
        "        ID[\"EPSG\",8605]],\n"
        "    PARAMETER[\"Y-axis translation\",-19.42,\n"
        "        LENGTHUNIT[\"millimetre\",0.001],\n"
        "        ID[\"EPSG\",8606]],\n"
        "    PARAMETER[\"Z-axis translation\",32.01,\n"
        "        LENGTHUNIT[\"millimetre\",0.001],\n"
        "        ID[\"EPSG\",8607]],\n"
        "    PARAMETER[\"X-axis rotation\",-0.4254,\n"
        "        ANGLEUNIT[\"milliarc-second\",4.84813681109536e-09],\n"
        "        ID[\"EPSG\",8608]],\n"
        "    PARAMETER[\"Y-axis rotation\",2.2578,\n"
        "        ANGLEUNIT[\"milliarc-second\",4.84813681109536e-09],\n"
        "        ID[\"EPSG\",8609]],\n"
        "    PARAMETER[\"Z-axis rotation\",2.4015,\n"
        "        ANGLEUNIT[\"milliarc-second\",4.84813681109536e-09],\n"
        "        ID[\"EPSG\",8610]],\n"
        "    PARAMETER[\"Scale difference\",9.71,\n"
        "        SCALEUNIT[\"parts per billion\",1e-09],\n"
        "        ID[\"EPSG\",8611]],\n"
        "    PARAMETER[\"Rate of change of X-axis translation\",1.42,\n"
        "        LENGTHUNIT[\"millimetres per year\",3.16887651727315e-11],\n"
        "        ID[\"EPSG\",1040]],\n"
        "    PARAMETER[\"Rate of change of Y-axis translation\",1.34,\n"
        "        LENGTHUNIT[\"millimetres per year\",3.16887651727315e-11],\n"
        "        ID[\"EPSG\",1041]],\n"
        "    PARAMETER[\"Rate of change of Z-axis translation\",0.9,\n"
        "        LENGTHUNIT[\"millimetres per year\",3.16887651727315e-11],\n"
        "        ID[\"EPSG\",1042]],\n"
        "    PARAMETER[\"Rate of change of X-axis rotation\",1.5461,\n"
        "        ANGLEUNIT[\"milliarc-seconds per "
        "year\",1.53631468932076e-16],\n"
        "        ID[\"EPSG\",1043]],\n"
        "    PARAMETER[\"Rate of change of Y-axis rotation\",1.182,\n"
        "        ANGLEUNIT[\"milliarc-seconds per "
        "year\",1.53631468932076e-16],\n"
        "        ID[\"EPSG\",1044]],\n"
        "    PARAMETER[\"Rate of change of Z-axis rotation\",1.1551,\n"
        "        ANGLEUNIT[\"milliarc-seconds per "
        "year\",1.53631468932076e-16],\n"
        "        ID[\"EPSG\",1045]],\n"
        "    PARAMETER[\"Rate of change of Scale difference\",0.109,\n"
        "        SCALEUNIT[\"parts per billion per "
        "year\",3.16887651727315e-17],\n"
        "        ID[\"EPSG\",1046]],\n"
        "    PARAMETER[\"Parameter reference epoch\",1994,\n"
        "        TIMEUNIT[\"year\",31556925.445],\n"
        "        ID[\"EPSG\",1047]],\n"
        "    OPERATIONACCURACY[0.03],\n"
        "    USAGE[\n"
        "        SCOPE[\"unknown\"],\n"
        "        AREA[\"Australia - onshore and EEZ\"],\n"
        "        BBOX[-47.2,109.23,-8.88,163.2]],\n"
        "    ID[\"EPSG\",6276]]";

    EXPECT_EQ(op->exportToWKT(
                  WKTFormatter::create(WKTFormatter::Convention::WKT2_2018)),
              expected);
}

// ---------------------------------------------------------------------------

TEST(
    factory,
    AuthorityFactory_createCoordinateOperation_grid_transformation_one_parameter) {
    auto factory = AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    auto op = factory->createCoordinateOperation("1295");
    auto expected =
        "COORDINATEOPERATION[\"RGNC91-93 to NEA74 Noumea (4)\",\n"
        "    SOURCECRS[\n"
        "        GEOGCRS[\"RGNC91-93\",\n"
        "            DATUM[\"Reseau Geodesique de Nouvelle Caledonie 91-93\",\n"
        "                ELLIPSOID[\"GRS 1980\",6378137,298.257222101,\n"
        "                    LENGTHUNIT[\"metre\",1]]],\n"
        "            PRIMEM[\"Greenwich\",0,\n"
        "                ANGLEUNIT[\"degree\",0.0174532925199433]],\n"
        "            CS[ellipsoidal,2],\n"
        "                AXIS[\"geodetic latitude (Lat)\",north,\n"
        "                    ORDER[1],\n"
        "                    ANGLEUNIT[\"degree\",0.0174532925199433]],\n"
        "                AXIS[\"geodetic longitude (Lon)\",east,\n"
        "                    ORDER[2],\n"
        "                    ANGLEUNIT[\"degree\",0.0174532925199433]]]],\n"
        "    TARGETCRS[\n"
        "        GEOGCRS[\"NEA74 Noumea\",\n"
        "            DATUM[\"NEA74 Noumea\",\n"
        "                ELLIPSOID[\"International 1924\",6378388,297,\n"
        "                    LENGTHUNIT[\"metre\",1]]],\n"
        "            PRIMEM[\"Greenwich\",0,\n"
        "                ANGLEUNIT[\"degree\",0.0174532925199433]],\n"
        "            CS[ellipsoidal,2],\n"
        "                AXIS[\"geodetic latitude (Lat)\",north,\n"
        "                    ORDER[1],\n"
        "                    ANGLEUNIT[\"degree\",0.0174532925199433]],\n"
        "                AXIS[\"geodetic longitude (Lon)\",east,\n"
        "                    ORDER[2],\n"
        "                    ANGLEUNIT[\"degree\",0.0174532925199433]]]],\n"
        "    METHOD[\"NTv2\",\n"
        "        ID[\"EPSG\",9615]],\n"
        "    PARAMETERFILE[\"Latitude and longitude difference "
        "file\",\"RGNC1991_NEA74Noumea.gsb\"],\n"
        "    OPERATIONACCURACY[0.05],\n"
        "    USAGE[\n"
        "        SCOPE[\"unknown\"],\n"
        "        AREA[\"New Caledonia - Grande Terre - Noumea\"],\n"
        "        BBOX[-22.37,166.35,-22.19,166.54]],\n"
        "    ID[\"EPSG\",1295]]";
    EXPECT_EQ(op->exportToWKT(
                  WKTFormatter::create(WKTFormatter::Convention::WKT2_2018)),
              expected);
}

// ---------------------------------------------------------------------------

TEST(
    factory,
    AuthorityFactory_createCoordinateOperation_grid_transformation_two_parameter) {
    auto factory = AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    auto op = factory->createCoordinateOperation("15864");
    auto expected =
        "    PARAMETERFILE[\"Latitude difference file\",\"alaska.las\"],\n"
        "    PARAMETERFILE[\"Longitude difference file\",\"alaska.los\"],\n";

    auto wkt = op->exportToWKT(
        WKTFormatter::create(WKTFormatter::Convention::WKT2_2018));
    EXPECT_TRUE(wkt.find(expected) != std::string::npos) << wkt;
}

// ---------------------------------------------------------------------------

TEST(factory, AuthorityFactory_createCoordinateOperation_other_transformation) {
    auto factory = AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    auto op = factory->createCoordinateOperation("1884");
    auto expected =
        "COORDINATEOPERATION[\"S-JTSK (Ferro) to S-JTSK (1)\",\n"
        "    SOURCECRS[\n"
        "        GEOGCRS[\"S-JTSK (Ferro)\",\n"
        "            DATUM[\"System of the Unified Trigonometrical Cadastral "
        "Network (Ferro)\",\n"
        "                ELLIPSOID[\"Bessel 1841\",6377397.155,299.1528128,\n"
        "                    LENGTHUNIT[\"metre\",1]]],\n"
        "            PRIMEM[\"Ferro\",-17.6666666666667,\n"
        "                ANGLEUNIT[\"degree\",0.0174532925199433]],\n"
        "            CS[ellipsoidal,2],\n"
        "                AXIS[\"geodetic latitude (Lat)\",north,\n"
        "                    ORDER[1],\n"
        "                    ANGLEUNIT[\"degree\",0.0174532925199433]],\n"
        "                AXIS[\"geodetic longitude (Lon)\",east,\n"
        "                    ORDER[2],\n"
        "                    ANGLEUNIT[\"degree\",0.0174532925199433]]]],\n"
        "    TARGETCRS[\n"
        "        GEOGCRS[\"S-JTSK\",\n"
        "            DATUM[\"System of the Unified Trigonometrical Cadastral "
        "Network\",\n"
        "                ELLIPSOID[\"Bessel 1841\",6377397.155,299.1528128,\n"
        "                    LENGTHUNIT[\"metre\",1]]],\n"
        "            PRIMEM[\"Greenwich\",0,\n"
        "                ANGLEUNIT[\"degree\",0.0174532925199433]],\n"
        "            CS[ellipsoidal,2],\n"
        "                AXIS[\"geodetic latitude (Lat)\",north,\n"
        "                    ORDER[1],\n"
        "                    ANGLEUNIT[\"degree\",0.0174532925199433]],\n"
        "                AXIS[\"geodetic longitude (Lon)\",east,\n"
        "                    ORDER[2],\n"
        "                    ANGLEUNIT[\"degree\",0.0174532925199433]]]],\n"
        "    METHOD[\"Longitude rotation\",\n"
        "        ID[\"EPSG\",9601]],\n"
        "    PARAMETER[\"Longitude offset\",-17.6666666666667,\n"
        "        ANGLEUNIT[\"degree\",0.0174532925199433],\n"
        "        ID[\"EPSG\",8602]],\n"
        "    OPERATIONACCURACY[0.0],\n"
        "    USAGE[\n"
        "        SCOPE[\"unknown\"],\n"
        "        AREA[\"Europe - Czechoslovakia\"],\n"
        "        BBOX[47.73,12.09,51.06,22.56]],\n"
        "    ID[\"EPSG\",1884]]";

    EXPECT_EQ(op->exportToWKT(
                  WKTFormatter::create(WKTFormatter::Convention::WKT2_2018)),
              expected);
}

// ---------------------------------------------------------------------------

TEST(factory, AuthorityFactory_test_uom_9110) {
    auto factory = AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    // This tests conversion from unit of measure EPSG:9110 DDD.MMSSsss
    auto crs = factory->createProjectedCRS("2172");
    EXPECT_EQ(crs->exportToPROJString(PROJStringFormatter::create()),
              "+proj=pipeline +step +proj=axisswap +order=2,1 +step "
              "+proj=unitconvert +xy_in=deg +xy_out=rad +step +proj=sterea "
              "+lat_0=53.0019444444444 +lon_0=21.5027777777778 +k=0.9998 "
              "+x_0=4603000 +y_0=5806000 +ellps=krass +step +proj=axisswap "
              "+order=2,1");
}

// ---------------------------------------------------------------------------

TEST(factory,
     AuthorityFactory_createCoordinateOperation_concatenated_operation) {
    auto factory = AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    auto op = factory->createCoordinateOperation("3896");
    auto concatenated = nn_dynamic_pointer_cast<ConcatenatedOperation>(op);
    ASSERT_TRUE(concatenated != nullptr);
    auto operations = concatenated->operations();
    ASSERT_EQ(operations.size(), 2);
    EXPECT_TRUE(operations[0]->isEquivalentTo(
        factory->createCoordinateOperation("3895")));
    EXPECT_TRUE(operations[1]->isEquivalentTo(
        factory->createCoordinateOperation("1618")));
}

// ---------------------------------------------------------------------------

TEST(
    factory,
    AuthorityFactory_createCoordinateOperation_concatenated_operation_three_steps) {
    auto factory = AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    auto op = factory->createCoordinateOperation("8647");
    auto concatenated = nn_dynamic_pointer_cast<ConcatenatedOperation>(op);
    ASSERT_TRUE(concatenated != nullptr);
    auto operations = concatenated->operations();
    ASSERT_EQ(operations.size(), 3);
    EXPECT_TRUE(operations[0]->isEquivalentTo(
        factory->createCoordinateOperation("1313")));
    EXPECT_TRUE(operations[1]->isEquivalentTo(
        factory->createCoordinateOperation("1950")));
    EXPECT_TRUE(operations[2]->isEquivalentTo(
        factory->createCoordinateOperation("1946")));
}

// ---------------------------------------------------------------------------

TEST(
    factory,
    AuthorityFactory_createCoordinateOperation_concatenated_operation_inverse_step1) {
    auto factory = AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    auto op = factory->createCoordinateOperation("8443");
    auto concatenated = nn_dynamic_pointer_cast<ConcatenatedOperation>(op);
    ASSERT_TRUE(concatenated != nullptr);
    auto operations = concatenated->operations();
    ASSERT_EQ(operations.size(), 2);
    EXPECT_TRUE(operations[0]->isEquivalentTo(
        factory->createCoordinateOperation("8364")->inverse()));
    EXPECT_TRUE(operations[1]->isEquivalentTo(
        factory->createCoordinateOperation("8367")));
}

// ---------------------------------------------------------------------------

TEST(
    factory,
    AuthorityFactory_createCoordinateOperation_concatenated_operation_inverse_step2) {
    auto factory = AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    auto op = factory->createCoordinateOperation("7811");
    auto concatenated = nn_dynamic_pointer_cast<ConcatenatedOperation>(op);
    ASSERT_TRUE(concatenated != nullptr);
    auto operations = concatenated->operations();
    ASSERT_EQ(operations.size(), 2);
    EXPECT_TRUE(operations[0]->isEquivalentTo(
        factory->createCoordinateOperation("1763")));
    EXPECT_TRUE(operations[1]->isEquivalentTo(
        factory->createCoordinateOperation("15958")->inverse()));
}

// ---------------------------------------------------------------------------

TEST(
    factory,
    AuthorityFactory_createCoordinateOperation_concatenated_operation_step1_is_conversion) {
    auto factory = AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    auto op = factory->createCoordinateOperation("7973");
    auto concatenated = nn_dynamic_pointer_cast<ConcatenatedOperation>(op);
    ASSERT_TRUE(concatenated != nullptr);
    auto operations = concatenated->operations();
    ASSERT_EQ(operations.size(), 2);
    EXPECT_TRUE(operations[0]->isEquivalentTo(
        factory->createCoordinateOperation("7972")));
    EXPECT_TRUE(operations[1]->isEquivalentTo(
        factory->createCoordinateOperation("7969")));
}

// ---------------------------------------------------------------------------

static bool in(const std::string &str, const std::vector<std::string> &list) {
    for (const auto &listItem : list) {
        if (str == listItem) {
            return true;
        }
    }
    return false;
}

// ---------------------------------------------------------------------------

TEST(factory, AuthorityFactory_build_all_concatenated) {
    auto factory = AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    auto setConcatenated = factory->getAuthorityCodes(
        AuthorityFactory::ObjectType::CONCATENATED_OPERATION);
    auto setConcatenatedNoDeprecated = factory->getAuthorityCodes(
        AuthorityFactory::ObjectType::CONCATENATED_OPERATION, false);
    EXPECT_LT(setConcatenatedNoDeprecated.size(), setConcatenated.size());
    for (const auto &code : setConcatenated) {
        if (in(code, {"8422", "8481", "8482", "8565", "8566", "8572"})) {
            EXPECT_THROW(factory->createCoordinateOperation(code),
                         FactoryException);
        } else {
            factory->createCoordinateOperation(code);
        }
    }
}

// ---------------------------------------------------------------------------

TEST(factory, AuthorityFactory_createCoordinateOperation_conversion) {
    auto factory = AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    auto op = factory->createCoordinateOperation("16031");
    auto conversion = nn_dynamic_pointer_cast<Conversion>(op);
    ASSERT_TRUE(conversion != nullptr);
}

// ---------------------------------------------------------------------------

TEST(factory, AuthorityFactory_getAuthorityCodes) {
    auto factory = AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    {
        auto set = factory->getAuthorityCodes(
            AuthorityFactory::ObjectType::PRIME_MERIDIAN);
        ASSERT_TRUE(!set.empty());
        factory->createPrimeMeridian(*(set.begin()));
    }
    {
        auto set =
            factory->getAuthorityCodes(AuthorityFactory::ObjectType::ELLIPSOID);
        ASSERT_TRUE(!set.empty());
        factory->createEllipsoid(*(set.begin()));
    }
    {
        auto setDatum =
            factory->getAuthorityCodes(AuthorityFactory::ObjectType::DATUM);
        ASSERT_TRUE(!setDatum.empty());
        factory->createDatum(*(setDatum.begin()));

        auto setGeodeticDatum = factory->getAuthorityCodes(
            AuthorityFactory::ObjectType::GEODETIC_REFERENCE_FRAME);
        ASSERT_TRUE(!setGeodeticDatum.empty());
        factory->createGeodeticDatum(*(setGeodeticDatum.begin()));

        auto setVerticalDatum = factory->getAuthorityCodes(
            AuthorityFactory::ObjectType::VERTICAL_REFERENCE_FRAME);
        ASSERT_TRUE(!setVerticalDatum.empty());
        factory->createVerticalDatum(*(setVerticalDatum.begin()));

        std::set<std::string> setMerged;
        for (const auto &v : setGeodeticDatum) {
            setMerged.insert(v);
        }
        for (const auto &v : setVerticalDatum) {
            setMerged.insert(v);
        }
        EXPECT_EQ(setDatum, setMerged);
    }
    {
        auto setCRS =
            factory->getAuthorityCodes(AuthorityFactory::ObjectType::CRS);
        ASSERT_TRUE(!setCRS.empty());
        factory->createCoordinateReferenceSystem(*(setCRS.begin()));

        auto setGeodeticCRS = factory->getAuthorityCodes(
            AuthorityFactory::ObjectType::GEODETIC_CRS);
        ASSERT_TRUE(!setGeodeticCRS.empty());
        factory->createGeodeticCRS(*(setGeodeticCRS.begin()));

        auto setGeographicCRS = factory->getAuthorityCodes(
            AuthorityFactory::ObjectType::GEOGRAPHIC_CRS);
        ASSERT_TRUE(!setGeographicCRS.empty());
        factory->createGeographicCRS(*(setGeographicCRS.begin()));
        EXPECT_LT(setGeographicCRS.size(), setGeodeticCRS.size());
        for (const auto &v : setGeographicCRS) {
            EXPECT_TRUE(setGeodeticCRS.find(v) != setGeodeticCRS.end());
        }

        auto setVerticalCRS = factory->getAuthorityCodes(
            AuthorityFactory::ObjectType::VERTICAL_CRS);
        ASSERT_TRUE(!setVerticalCRS.empty());
        factory->createVerticalCRS(*(setVerticalCRS.begin()));

        auto setProjectedCRS = factory->getAuthorityCodes(
            AuthorityFactory::ObjectType::PROJECTED_CRS);
        ASSERT_TRUE(!setProjectedCRS.empty());
        factory->createProjectedCRS(*(setProjectedCRS.begin()));

        auto setCompoundCRS = factory->getAuthorityCodes(
            AuthorityFactory::ObjectType::COMPOUND_CRS);
        ASSERT_TRUE(!setCompoundCRS.empty());
        factory->createCompoundCRS(*(setCompoundCRS.begin()));

        std::set<std::string> setMerged;
        for (const auto &v : setGeodeticCRS) {
            setMerged.insert(v);
        }
        for (const auto &v : setVerticalCRS) {
            setMerged.insert(v);
        }
        for (const auto &v : setProjectedCRS) {
            setMerged.insert(v);
        }
        for (const auto &v : setCompoundCRS) {
            setMerged.insert(v);
        }
        EXPECT_EQ(setCRS, setMerged);
    }
    {
        auto setCO = factory->getAuthorityCodes(
            AuthorityFactory::ObjectType::COORDINATE_OPERATION);
        ASSERT_TRUE(!setCO.empty());
        factory->createCoordinateOperation(*(setCO.begin()));

        auto setConversion = factory->getAuthorityCodes(
            AuthorityFactory::ObjectType::CONVERSION);
        ASSERT_TRUE(!setConversion.empty());
        factory->createConversion(*(setConversion.begin()));

        auto setTransformation = factory->getAuthorityCodes(
            AuthorityFactory::ObjectType::TRANSFORMATION);
        ASSERT_TRUE(!setTransformation.empty());
        ASSERT_TRUE(nn_dynamic_pointer_cast<Transformation>(
                        factory->createCoordinateOperation(
                            *(setTransformation.begin()))) != nullptr);

        auto setConcatenated = factory->getAuthorityCodes(
            AuthorityFactory::ObjectType::CONCATENATED_OPERATION);
        ASSERT_TRUE(!setConcatenated.empty());
        ASSERT_TRUE(nn_dynamic_pointer_cast<ConcatenatedOperation>(
                        factory->createCoordinateOperation(
                            *(setConcatenated.begin()))) != nullptr);

        std::set<std::string> setMerged;
        for (const auto &v : setConversion) {
            setMerged.insert(v);
        }
        for (const auto &v : setTransformation) {
            setMerged.insert(v);
        }
        for (const auto &v : setConcatenated) {
            setMerged.insert(v);
        }
        EXPECT_EQ(setCO.size(), setMerged.size());
        std::set<std::string> setMissing;
        for (const auto &v : setCO) {
            if (setMerged.find(v) == setMerged.end()) {
                setMissing.insert(v);
            }
        }
        EXPECT_EQ(setMissing, std::set<std::string>());
        EXPECT_EQ(setCO, setMerged);
    }
}

// ---------------------------------------------------------------------------

TEST(factory, AuthorityFactory_getDescriptionText) {
    auto factory = AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    EXPECT_THROW(factory->getDescriptionText("-1"),
                 NoSuchAuthorityCodeException);
    EXPECT_EQ(factory->getDescriptionText("10000"),
              "RGF93 to NGF IGN69 height (1)");
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
        ASSERT_TRUE(execute("PRAGMA foreign_keys = 1;")) << last_error();
    }

    void populateWithFakeEPSG() {

        ASSERT_TRUE(execute("INSERT INTO unit_of_measure "
                            "VALUES('EPSG','9001','metre','length',1.0,0);"))
            << last_error();
        ASSERT_TRUE(execute("INSERT INTO unit_of_measure "
                            "VALUES('EPSG','9102','degree','angle',1."
                            "74532925199432781271e-02,0);"))
            << last_error();
        ASSERT_TRUE(execute(
            "INSERT INTO unit_of_measure VALUES('EPSG','9122','degree "
            "(supplier to "
            "define representation)','angle',1.74532925199432781271e-02,0);"))
            << last_error();
        ASSERT_TRUE(
            execute("INSERT INTO area "
                    "VALUES('EPSG','1262','World','World.',-90.0,90.0,-180."
                    "0,180.0,0);"))
            << last_error();
        ASSERT_TRUE(
            execute("INSERT INTO prime_meridian "
                    "VALUES('EPSG','8901','Greenwich',0.0,'EPSG','9102',0);"))
            << last_error();
        ASSERT_TRUE(
            execute("INSERT INTO ellipsoid VALUES('EPSG','7030','WGS "
                    "84',6378137.0,'EPSG','9001',298.257223563,NULL,0);"))
            << last_error();
        ASSERT_TRUE(
            execute("INSERT INTO geodetic_datum "
                    "VALUES('EPSG','6326','World Geodetic System "
                    "1984','EPSG','7030','EPSG','8901','EPSG','1262',0);"))
            << last_error();
        ASSERT_TRUE(
            execute("INSERT INTO vertical_datum VALUES('EPSG','1027','EGM2008 "
                    "geoid','EPSG','1262',0);"))
            << last_error();
        ASSERT_TRUE(execute("INSERT INTO coordinate_system "
                            "VALUES('EPSG','6422','ellipsoidal',2);"))
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
        ASSERT_TRUE(
            execute("INSERT INTO geodetic_crs VALUES('EPSG','4326','WGS "
                    "84','geographic "
                    "2D','EPSG','6422','EPSG','6326','EPSG','1262',0);"))
            << last_error();

        ASSERT_TRUE(execute("INSERT INTO coordinate_system "
                            "VALUES('EPSG','6499','vertical',1);"))
            << last_error();
        ASSERT_TRUE(
            execute("INSERT INTO axis VALUES('EPSG','114','Gravity-related "
                    "height','H','up','EPSG','6499',1,'EPSG','9001');"))
            << last_error();
        ASSERT_TRUE(
            execute("INSERT INTO crs VALUES('EPSG','3855','vertical');"))
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
            "INSERT INTO area VALUES('EPSG','1933','World - N hemisphere - "
            "0°E to 6°E','',0.0,84.0,0.0,6.0,0);"))
            << last_error();

        ASSERT_TRUE(execute(
            "INSERT INTO conversion VALUES('EPSG','16031','UTM zone "
            "31N','EPSG','1933','EPSG','9807','Transverse "
            "Mercator','EPSG','8801','Latitude "
            "of "
            "natural origin',0.0,'EPSG','9102','EPSG','8802','Longitude of "
            "natural "
            "origin',3.0,'EPSG','9102','EPSG','8805','Scale factor at natural "
            "origin',0.9996,'EPSG','9201','EPSG','8806','False "
            "easting',500000.0,'EPSG','9001','EPSG','8807','False "
            "northing',0.0,'EPSG','9001',NULL,NULL,NULL,NULL,NULL,NULL,NULL,"
            "NULL,"
            "NULL,NULL,NULL,NULL);"))
            << last_error();

        ASSERT_TRUE(execute(
            "INSERT INTO area VALUES('EPSG','2060','World - N hemisphere - "
            "0°E to 6°E - by country','',0.0,84.0,0.0,6.0,0);"))
            << last_error();

        ASSERT_TRUE(execute("INSERT INTO coordinate_system "
                            "VALUES('EPSG','4400','Cartesian',2);"))
            << last_error();
        ASSERT_TRUE(
            execute("INSERT INTO axis "
                    "VALUES('EPSG','1','Easting','E','east','EPSG','4400',"
                    "1,'EPSG','9001');"))
            << last_error();
        ASSERT_TRUE(
            execute("INSERT INTO axis "
                    "VALUES('EPSG','2','Northing','N','north','EPSG','4400'"
                    ",2,'EPSG','9001');"))
            << last_error();
        ASSERT_TRUE(
            execute("INSERT INTO crs VALUES('EPSG','32631','projected');"))
            << last_error();
        ASSERT_TRUE(execute("INSERT INTO projected_crs "
                            "VALUES('EPSG','32631','WGS 84 / UTM zone "
                            "31N','EPSG','4400','EPSG','4326','EPSG','16031','"
                            "EPSG','2060',0);"))
            << last_error();

        ASSERT_TRUE(
            execute("INSERT INTO crs VALUES('EPSG','MY_COMPOUND','compound');"))
            << last_error();
        ASSERT_TRUE(execute(
            "INSERT INTO compound_crs VALUES('EPSG','MY_COMPOUND','WGS 84 + "
            "EGM2008 geoid "
            "height','EPSG','4326','EPSG','3855','EPSG','1262',0);"))
            << last_error();

        ASSERT_TRUE(
            execute("INSERT INTO coordinate_operation "
                    "VALUES('EPSG','DUMMY_HELMERT','helmert_transformation');"))
            << last_error();
        ASSERT_TRUE(execute(
            "INSERT INTO helmert_transformation "
            "VALUES('EPSG','DUMMY_HELMERT','name','EPSG','9603','"
            "Geocentric translations (geog2D "
            "domain)','EPSG','4326','EPSG','4326','EPSG','1262',44.0,-143."
            "0,-90.0,-294.0,'EPSG','9001',NULL,NULL,NULL,NULL,NULL,NULL,"
            "NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,"
            "NULL,NULL,NULL,NULL,NULL,NULL,0);"))
            << last_error();

        ASSERT_TRUE(execute("INSERT INTO coordinate_operation "
                            "VALUES('EPSG','DUMMY_GRID_TRANSFORMATION','grid_"
                            "transformation');"))
            << last_error();
        ASSERT_TRUE(execute(
            "INSERT INTO grid_transformation "
            "VALUES('EPSG','DUMMY_GRID_TRANSFORMATION','name','EPSG','9615'"
            ",'NTv2','EPSG','4326','EPSG','4326','EPSG','1262',1.0,'EPSG','"
            "8656','Latitude and longitude difference "
            "file','nzgd2kgrid0005.gsb',NULL,NULL,NULL,NULL,NULL,NULL,0);"))
            << last_error();

        ASSERT_TRUE(execute(
            "INSERT INTO unit_of_measure VALUES('EPSG','9110','sexagesimal "
            "DMS','angle',NULL,0);"))
            << last_error();
        ASSERT_TRUE(execute("INSERT INTO coordinate_operation "
                            "VALUES('EPSG','DUMMY_OTHER_TRANSFORMATION','other_"
                            "transformation');"))
            << last_error();
        ASSERT_TRUE(execute(
            "INSERT INTO other_transformation "
            "VALUES('EPSG','DUMMY_OTHER_TRANSFORMATION','name','EPSG','"
            "9601','Longitude "
            "rotation','EPSG','4326','EPSG','4326','EPSG','1262',0.0,'EPSG'"
            ",'8602','Longitude "
            "offset',-17.4,'EPSG','9110',NULL,NULL,NULL,NULL,NULL,NULL,"
            "NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,"
            "NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,"
            "NULL,NULL,NULL,NULL,NULL,NULL,0);"))
            << last_error();

        ASSERT_TRUE(execute(
            "INSERT INTO coordinate_operation "
            "VALUES('EPSG','DUMMY_CONCATENATED','concatenated_operation');"))
            << last_error();
        ASSERT_TRUE(execute(
            "INSERT INTO concatenated_operation "
            "VALUES('EPSG','DUMMY_CONCATENATED','name','EPSG','4326','EPSG'"
            ",'4326','EPSG','1262',NULL,'EPSG','DUMMY_OTHER_TRANSFORMATION'"
            ",'EPSG','DUMMY_OTHER_TRANSFORMATION',NULL,NULL,0);"))
            << last_error();
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

TEST_F(FactoryWithTmpDatabase, AuthorityFactory_test_with_fake_EPSG_database) {
    createStructure();
    populateWithFakeEPSG();

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

    EXPECT_TRUE(nn_dynamic_pointer_cast<Transformation>(
                    factory->createObject("DUMMY_HELMERT")) != nullptr);

    EXPECT_TRUE(nn_dynamic_pointer_cast<Transformation>(factory->createObject(
                    "DUMMY_GRID_TRANSFORMATION")) != nullptr);

    EXPECT_TRUE(nn_dynamic_pointer_cast<Transformation>(factory->createObject(
                    "DUMMY_OTHER_TRANSFORMATION")) != nullptr);

    EXPECT_TRUE(nn_dynamic_pointer_cast<ConcatenatedOperation>(
                    factory->createObject("DUMMY_CONCATENATED")) != nullptr);
}

// ---------------------------------------------------------------------------

TEST(factory, AuthorityFactory_createFromCoordinateReferenceSystemCodes) {
    auto factory = AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    EXPECT_TRUE(
        factory->createFromCoordinateReferenceSystemCodes("-1", "-1").empty());
    {
        auto res =
            factory->createFromCoordinateReferenceSystemCodes("4326", "32631");
        ASSERT_EQ(res.size(), 1);
        EXPECT_TRUE(res[0]->sourceCRS() != nullptr);
        EXPECT_TRUE(res[0]->targetCRS() != nullptr);
        EXPECT_TRUE(res[0]->isEquivalentTo(factory->createConversion("16031")));
    }
    {
        auto res =
            factory->createFromCoordinateReferenceSystemCodes("4209", "4326");
        EXPECT_TRUE(!res.empty());
        for (const auto &conv : res) {
            EXPECT_TRUE(conv->sourceCRS()->isEPSG(4209));
            EXPECT_TRUE(conv->targetCRS()->isEPSG(4326));
            EXPECT_FALSE(conv->isDeprecated());
        }
    }
    {
        auto list =
            factory->createFromCoordinateReferenceSystemCodes("4179", "4258");
        ASSERT_EQ(list.size(), 3);
        // Romania has a larger area than Poland (given our approx formula)
        EXPECT_EQ(list[0]->getEPSGCode(), 15994); // Romania - 3m
        EXPECT_EQ(list[1]->getEPSGCode(), 15993); // Romania - 10m
        EXPECT_EQ(list[2]->getEPSGCode(), 1644);  // Poland - 1m
    }
}

// ---------------------------------------------------------------------------

TEST(
    factory,
    AuthorityFactory_createFromCoordinateReferenceSystemCodes_anonymous_authority) {
    auto factory =
        AuthorityFactory::create(DatabaseContext::create(), std::string());

    {
        auto res = factory->createFromCoordinateReferenceSystemCodes(
            "EPSG", "4326", "EPSG", "32631");
        ASSERT_EQ(res.size(), 1);
    }
    {
        auto res = factory->createFromCoordinateReferenceSystemCodes(
            "EPSG", "4209", "EPSG", "4326");
        EXPECT_TRUE(!res.empty());
        for (const auto &conv : res) {
            EXPECT_TRUE(conv->sourceCRS()->isEPSG(4209));
            EXPECT_TRUE(conv->targetCRS()->isEPSG(4326));
            EXPECT_FALSE(conv->isDeprecated());
        }
    }
}

// ---------------------------------------------------------------------------

TEST_F(FactoryWithTmpDatabase,
       AuthorityFactory_test_with_fake_EPSG_and_OTHER_database) {
    createStructure();
    populateWithFakeEPSG();

    ASSERT_TRUE(execute(
        "INSERT INTO crs VALUES('OTHER','OTHER_4326','geographic 2D');"))
        << last_error();
    ASSERT_TRUE(
        execute("INSERT INTO geodetic_crs VALUES('OTHER','OTHER_4326','WGS "
                "84','geographic "
                "2D','EPSG','6422','EPSG','6326','EPSG','1262',0);"))
        << last_error();

    ASSERT_TRUE(
        execute("INSERT INTO crs VALUES('OTHER','OTHER_32631','projected');"))
        << last_error();
    ASSERT_TRUE(execute("INSERT INTO projected_crs "
                        "VALUES('OTHER','OTHER_32631','WGS 84 / UTM zone "
                        "31N','EPSG','4400','OTHER','OTHER_4326','EPSG','16031'"
                        ",'EPSG','2060',0);"))
        << last_error();

    auto factoryGeneral = AuthorityFactory::create(
        DatabaseContext::create(m_ctxt), std::string());
    {
        auto res = factoryGeneral->createFromCoordinateReferenceSystemCodes(
            "OTHER", "OTHER_4326", "OTHER", "OTHER_32631");
        ASSERT_EQ(res.size(), 1);
    }

    auto factoryEPSG =
        AuthorityFactory::create(DatabaseContext::create(m_ctxt), "EPSG");
    {
        auto res = factoryEPSG->createFromCoordinateReferenceSystemCodes(
            "OTHER", "OTHER_4326", "OTHER", "OTHER_32631");
        ASSERT_EQ(res.size(), 1);
    }

    auto factoryOTHER =
        AuthorityFactory::create(DatabaseContext::create(m_ctxt), "OTHER");
    {
        auto res = factoryOTHER->createFromCoordinateReferenceSystemCodes(
            "OTHER_4326", "OTHER_32631");
        ASSERT_EQ(res.size(), 0); // the conversion is in the EPSG space
    }

    ASSERT_TRUE(execute(
        "INSERT INTO coordinate_operation "
        "VALUES('OTHER','OTHER_GRID_TRANSFORMATION','grid_transformation');"))
        << last_error();
    ASSERT_TRUE(execute(
        "INSERT INTO grid_transformation "
        "VALUES('OTHER','OTHER_GRID_TRANSFORMATION','name','EPSG','9615'"
        ",'NTv2','EPSG','4326','OTHER','OTHER_4326','EPSG','1262',1.0,'EPSG','"
        "8656','Latitude and longitude difference "
        "file','nzgd2kgrid0005.gsb',NULL,NULL,NULL,NULL,NULL,NULL,0);"))
        << last_error();
    {
        auto res = factoryGeneral->createFromCoordinateReferenceSystemCodes(
            "EPSG", "4326", "OTHER", "OTHER_4326");
        ASSERT_EQ(res.size(), 1);
    }
    {
        auto res = factoryEPSG->createFromCoordinateReferenceSystemCodes(
            "EPSG", "4326", "OTHER", "OTHER_4326");
        ASSERT_EQ(res.size(), 0);
    }
    {
        auto res = factoryOTHER->createFromCoordinateReferenceSystemCodes(
            "EPSG", "4326", "OTHER", "OTHER_4326");
        ASSERT_EQ(res.size(), 1);
    }
}

// ---------------------------------------------------------------------------

TEST_F(FactoryWithTmpDatabase,
       AuthorityFactory_test_sorting_of_coordinate_operations) {
    createStructure();
    populateWithFakeEPSG();

    ASSERT_TRUE(
        execute("INSERT INTO coordinate_operation "
                "VALUES('OTHER','TRANSFORMATION_10M','grid_transformation');"))
        << last_error();
    ASSERT_TRUE(execute(
        "INSERT INTO grid_transformation "
        "VALUES('OTHER','TRANSFORMATION_10M','TRANSFORMATION_10M','EPSG','9615'"
        ",'NTv2','EPSG','4326','EPSG','4326','EPSG','1262',10.0,'EPSG','"
        "8656','Latitude and longitude difference "
        "file','nzgd2kgrid0005.gsb',NULL,NULL,NULL,NULL,NULL,NULL,0);"))
        << last_error();

    ASSERT_TRUE(execute("INSERT INTO coordinate_operation "
                        "VALUES('OTHER','TRANSFORMATION_1M_SMALL_EXTENT','grid_"
                        "transformation');"))
        << last_error();
    ASSERT_TRUE(
        execute("INSERT INTO grid_transformation "
                "VALUES('OTHER','TRANSFORMATION_1M_SMALL_EXTENT','"
                "TRANSFORMATION_1M_SMALL_EXTENT','EPSG','9615'"
                ",'NTv2','EPSG','4326','EPSG','4326','EPSG','2060',1.0,'EPSG','"
                "8656','Latitude and longitude difference "
                "file','nzgd2kgrid0005.gsb',NULL,NULL,NULL,NULL,NULL,NULL,0);"))
        << last_error();

    ASSERT_TRUE(
        execute("INSERT INTO coordinate_operation "
                "VALUES('OTHER','TRANSFORMATION_1M','grid_transformation');"))
        << last_error();
    ASSERT_TRUE(execute(
        "INSERT INTO grid_transformation "
        "VALUES('OTHER','TRANSFORMATION_1M','TRANSFORMATION_1M','EPSG','9615'"
        ",'NTv2','EPSG','4326','EPSG','4326','EPSG','1262',1.0,'EPSG','"
        "8656','Latitude and longitude difference "
        "file','nzgd2kgrid0005.gsb',NULL,NULL,NULL,NULL,NULL,NULL,0);"))
        << last_error();

    ASSERT_TRUE(execute("INSERT INTO coordinate_operation "
                        "VALUES('OTHER','TRANSFORMATION_0.5M_DEPRECATED','grid_"
                        "transformation');"))
        << last_error();
    ASSERT_TRUE(
        execute("INSERT INTO grid_transformation "
                "VALUES('OTHER','TRANSFORMATION_0.5M_DEPRECATED','"
                "TRANSFORMATION_0.5M_DEPRECATED','EPSG','9615'"
                ",'NTv2','EPSG','4326','EPSG','4326','EPSG','1262',1.0,'EPSG','"
                "8656','Latitude and longitude difference "
                "file','nzgd2kgrid0005.gsb',NULL,NULL,NULL,NULL,NULL,NULL,1);"))
        << last_error();

    auto factoryOTHER =
        AuthorityFactory::create(DatabaseContext::create(m_ctxt), "OTHER");
    auto res = factoryOTHER->createFromCoordinateReferenceSystemCodes(
        "EPSG", "4326", "EPSG", "4326");
    ASSERT_EQ(res.size(), 3);
    EXPECT_EQ(*(res[0]->name()->description()), "TRANSFORMATION_1M");
    EXPECT_EQ(*(res[1]->name()->description()), "TRANSFORMATION_10M");
    EXPECT_EQ(*(res[2]->name()->description()),
              "TRANSFORMATION_1M_SMALL_EXTENT");
}

} // namespace
