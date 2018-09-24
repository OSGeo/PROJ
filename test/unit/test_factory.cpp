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
using namespace osgeo::proj::util;

// ---------------------------------------------------------------------------

TEST(factory, databasecontext_create) { DatabaseContext::create(); }

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
