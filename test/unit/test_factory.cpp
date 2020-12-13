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

#include "test_primitives.hpp"

#include "proj/common.hpp"
#include "proj/coordinateoperation.hpp"
#include "proj/coordinatesystem.hpp"
#include "proj/crs.hpp"
#include "proj/datum.hpp"
#include "proj/io.hpp"
#include "proj/metadata.hpp"
#include "proj/util.hpp"

#include <sqlite3.h>

#ifdef _MSC_VER
#include <stdio.h>
#else
#include <unistd.h>
#endif

//#undef SQLITE_OPEN_URI

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
    EXPECT_THROW(DatabaseContext::create("/i/do_not/exist"), FactoryException);
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
    auto uom = factory->createUnitOfMeasure("9001");
    EXPECT_EQ(uom->name(), "metre");
    EXPECT_EQ(uom->type(), UnitOfMeasure::Type::LINEAR);
    EXPECT_EQ(uom->conversionToSI(), 1.0);
    EXPECT_EQ(uom->codeSpace(), "EPSG");
    EXPECT_EQ(uom->code(), "9001");
}

// ---------------------------------------------------------------------------

TEST(factory, AuthorityFactory_createUnitOfMeasure_linear_us_survey_foot) {
    auto factory = AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    auto uom = factory->createUnitOfMeasure("9003");
    EXPECT_EQ(uom->conversionToSI(), 12. / 39.37);
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
                    AuthorityFactory::create(DatabaseContext::create(), "ESRI")
                        ->createObject("108900")) != nullptr);
    auto pm = factory->createPrimeMeridian("8903");
    ASSERT_EQ(pm->identifiers().size(), 1U);
    EXPECT_EQ(pm->identifiers()[0]->code(), "8903");
    EXPECT_EQ(*(pm->identifiers()[0]->codeSpace()), "EPSG");
    EXPECT_EQ(*(pm->name()->description()), "Paris");
    EXPECT_EQ(pm->longitude(), Angle(2.5969213, UnitOfMeasure::GRAD));
}

// ---------------------------------------------------------------------------

TEST(factory, AuthorityFactory_identifyBodyFromSemiMajorAxis) {
    auto factory = AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    EXPECT_EQ(factory->identifyBodyFromSemiMajorAxis(6378137, 1e-5), "Earth");
    EXPECT_THROW(factory->identifyBodyFromSemiMajorAxis(1, 1e-5),
                 FactoryException);
}

// ---------------------------------------------------------------------------

TEST(factory, AuthorityFactory_createEllipsoid) {
    auto factory = AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    EXPECT_THROW(factory->createEllipsoid("-1"), NoSuchAuthorityCodeException);
    EXPECT_TRUE(nn_dynamic_pointer_cast<Ellipsoid>(
                    factory->createObject("7030")) != nullptr);
    auto ellipsoid = factory->createEllipsoid("7030");
    ASSERT_EQ(ellipsoid->identifiers().size(), 1U);
    EXPECT_EQ(ellipsoid->identifiers()[0]->code(), "7030");
    EXPECT_EQ(*(ellipsoid->identifiers()[0]->codeSpace()), "EPSG");
    EXPECT_EQ(*(ellipsoid->name()->description()), "WGS 84");
    EXPECT_TRUE(ellipsoid->inverseFlattening().has_value());
    EXPECT_EQ(ellipsoid->semiMajorAxis(), Length(6378137));
    EXPECT_EQ(*ellipsoid->inverseFlattening(), Scale(298.257223563));
    EXPECT_EQ(ellipsoid->celestialBody(), "Earth");
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
    EXPECT_EQ(*(extent->description()), "World.");
    const auto &geogElts = extent->geographicElements();
    ASSERT_EQ(geogElts.size(), 1U);
    auto bbox = nn_dynamic_pointer_cast<GeographicBoundingBox>(geogElts[0]);
    ASSERT_TRUE(bbox != nullptr);
    EXPECT_EQ(bbox->westBoundLongitude(), -180);
    EXPECT_EQ(bbox->eastBoundLongitude(), 180);
    EXPECT_EQ(bbox->northBoundLatitude(), 90);
    EXPECT_EQ(bbox->southBoundLatitude(), -90);
}

// ---------------------------------------------------------------------------

TEST(factory, AuthorityFactory_createExtent_no_bbox) {
    auto factory = AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    auto extent = factory->createExtent("1361"); // Sudan - south. Deprecated
    EXPECT_EQ(*(extent->description()), "Sudan - south.");
    const auto &geogElts = extent->geographicElements();
    EXPECT_TRUE(geogElts.empty());
}

// ---------------------------------------------------------------------------

TEST(factory, AuthorityFactory_createGeodeticDatum) {
    auto factory = AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    EXPECT_THROW(factory->createGeodeticDatum("-1"),
                 NoSuchAuthorityCodeException);
    auto grf = factory->createGeodeticDatum("6326");
    EXPECT_TRUE(nn_dynamic_pointer_cast<DynamicGeodeticReferenceFrame>(grf) ==
                nullptr);
    ASSERT_EQ(grf->identifiers().size(), 1U);
    EXPECT_EQ(grf->identifiers()[0]->code(), "6326");
    EXPECT_EQ(*(grf->identifiers()[0]->codeSpace()), "EPSG");
    EXPECT_EQ(*(grf->name()->description()), "World Geodetic System 1984");
    EXPECT_TRUE(grf->ellipsoid()->isEquivalentTo(
        factory->createEllipsoid("7030").get()));
    EXPECT_TRUE(grf->primeMeridian()->isEquivalentTo(
        factory->createPrimeMeridian("8901").get()));
    ASSERT_EQ(grf->domains().size(), 1U);
    auto domain = grf->domains()[0];
    auto extent = domain->domainOfValidity();
    ASSERT_TRUE(extent != nullptr);
    EXPECT_TRUE(extent->isEquivalentTo(factory->createExtent("1262").get()));
    EXPECT_FALSE(grf->publicationDate().has_value());
}

// ---------------------------------------------------------------------------

TEST(factory, AuthorityFactory_createGeodeticDatum_with_publication_date) {
    auto factory = AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    // North American Datum 1983
    auto grf = factory->createGeodeticDatum("6269");
    EXPECT_TRUE(nn_dynamic_pointer_cast<DynamicGeodeticReferenceFrame>(grf) ==
                nullptr);
    EXPECT_TRUE(grf->publicationDate().has_value());
    EXPECT_EQ(grf->publicationDate()->toString(), "1986-01-01");
}

// ---------------------------------------------------------------------------

TEST(factory, AuthorityFactory_createDynamicGeodeticDatum) {
    auto factory = AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    auto grf = factory->createGeodeticDatum("1165"); // ITRF 2014
    auto dgrf = nn_dynamic_pointer_cast<DynamicGeodeticReferenceFrame>(grf);
    ASSERT_TRUE(dgrf != nullptr);
    EXPECT_EQ(dgrf->frameReferenceEpoch().value(), 2010.0);
}

// ---------------------------------------------------------------------------

TEST(factory, AuthorityFactory_createVerticalDatum) {
    auto factory = AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    EXPECT_THROW(factory->createVerticalDatum("-1"),
                 NoSuchAuthorityCodeException);
    auto vrf = factory->createVerticalDatum("1027");
    ASSERT_EQ(vrf->identifiers().size(), 1U);
    EXPECT_EQ(vrf->identifiers()[0]->code(), "1027");
    EXPECT_EQ(*(vrf->identifiers()[0]->codeSpace()), "EPSG");
    EXPECT_EQ(*(vrf->name()->description()), "EGM2008 geoid");
    auto domain = vrf->domains()[0];
    auto extent = domain->domainOfValidity();
    ASSERT_TRUE(extent != nullptr);
    EXPECT_TRUE(extent->isEquivalentTo(factory->createExtent("1262").get()));
    EXPECT_TRUE(vrf->publicationDate().has_value());
    EXPECT_EQ(vrf->publicationDate()->toString(), "2008-01-01");
}

// ---------------------------------------------------------------------------

TEST(factory, AuthorityFactory_createDynamicVerticalDatum) {
    auto factory = AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    auto grf = factory->createVerticalDatum("1096"); // Norway Normal Null 2000
    auto dvrf = nn_dynamic_pointer_cast<DynamicVerticalReferenceFrame>(grf);
    ASSERT_TRUE(dvrf != nullptr);
    EXPECT_EQ(dvrf->frameReferenceEpoch().value(), 2000.0);
}

// ---------------------------------------------------------------------------

TEST(factory, AuthorityFactory_createDatum) {
    auto factory = AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    EXPECT_THROW(factory->createDatum("-1"), NoSuchAuthorityCodeException);
    EXPECT_TRUE(factory->createDatum("6326")->isEquivalentTo(
        factory->createGeodeticDatum("6326").get()));
    EXPECT_TRUE(factory->createDatum("1027")->isEquivalentTo(
        factory->createVerticalDatum("1027").get()));
}

// ---------------------------------------------------------------------------

TEST(factory, AuthorityFactory_createDatumEnsembleGeodetic) {
    auto factory = AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    EXPECT_THROW(factory->createDatumEnsemble("-1"),
                 NoSuchAuthorityCodeException);
    EXPECT_THROW(factory->createDatumEnsemble("6326", "vertical_datum"),
                 NoSuchAuthorityCodeException);
    auto ensemble = factory->createDatumEnsemble("6326");
    EXPECT_EQ(ensemble->nameStr(), "World Geodetic System 1984 ensemble");
    ASSERT_EQ(ensemble->identifiers().size(), 1U);
    EXPECT_EQ(ensemble->identifiers()[0]->code(), "6326");
    EXPECT_EQ(*(ensemble->identifiers()[0]->codeSpace()), "EPSG");
    EXPECT_EQ(ensemble->datums().size(), 6U);
    EXPECT_EQ(ensemble->positionalAccuracy()->value(), "2.0");
    ASSERT_TRUE(!ensemble->domains().empty());
    auto domain = ensemble->domains()[0];
    auto extent = domain->domainOfValidity();
    ASSERT_TRUE(extent != nullptr);
    EXPECT_TRUE(extent->isEquivalentTo(factory->createExtent("1262").get()));

    {
        // Without using db
        auto datum = ensemble->asDatum(nullptr);
        EXPECT_EQ(datum->nameStr(), "World Geodetic System 1984");
        auto grf = dynamic_cast<GeodeticReferenceFrame *>(datum.get());
        ASSERT_TRUE(grf != nullptr);
        EXPECT_TRUE(grf->isEquivalentTo(factory->createDatum("6326").get()));
    }

    {
        // Using db
        auto datum = ensemble->asDatum(DatabaseContext::create());
        EXPECT_EQ(datum->nameStr(), "World Geodetic System 1984");
        auto grf = dynamic_cast<GeodeticReferenceFrame *>(datum.get());
        ASSERT_TRUE(grf != nullptr);
        EXPECT_TRUE(grf->isEquivalentTo(factory->createDatum("6326").get()));
    }
}

// ---------------------------------------------------------------------------

TEST(factory, AuthorityFactory_createDatumEnsembleVertical) {
    auto factory = AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    EXPECT_THROW(factory->createDatumEnsemble("1288", "geodetic_datum"),
                 NoSuchAuthorityCodeException);
    auto ensemble = factory->createDatumEnsemble("1288");
    EXPECT_EQ(ensemble->nameStr(), "British Isles height ensemble");
    ASSERT_EQ(ensemble->identifiers().size(), 1U);
    EXPECT_EQ(ensemble->identifiers()[0]->code(), "1288");
    EXPECT_EQ(*(ensemble->identifiers()[0]->codeSpace()), "EPSG");
    EXPECT_EQ(ensemble->datums().size(), 9U);
    EXPECT_EQ(ensemble->positionalAccuracy()->value(), "0.4");
    ASSERT_TRUE(!ensemble->domains().empty());
    auto domain = ensemble->domains()[0];
    auto extent = domain->domainOfValidity();
    ASSERT_TRUE(extent != nullptr);
    EXPECT_TRUE(extent->isEquivalentTo(factory->createExtent("4606").get()));

    {
        // Without using db
        auto datum = ensemble->asDatum(nullptr);
        auto vrf = dynamic_cast<VerticalReferenceFrame *>(datum.get());
        ASSERT_TRUE(vrf != nullptr);
        EXPECT_TRUE(vrf->isEquivalentTo(factory->createDatum("1288").get()));
    }

    {
        // Using db
        auto datum = ensemble->asDatum(DatabaseContext::create());
        auto vrf = dynamic_cast<VerticalReferenceFrame *>(datum.get());
        ASSERT_TRUE(vrf != nullptr);
        EXPECT_TRUE(vrf->isEquivalentTo(factory->createDatum("1288").get()));
    }
}

// ---------------------------------------------------------------------------

TEST(factory, AuthorityFactory_createCoordinateSystem_ellipsoidal_2_axis) {
    auto factory = AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    EXPECT_THROW(factory->createCoordinateSystem("-1"),
                 NoSuchAuthorityCodeException);
    auto cs = factory->createCoordinateSystem("6422");
    auto ellipsoidal_cs = nn_dynamic_pointer_cast<EllipsoidalCS>(cs);
    ASSERT_TRUE(ellipsoidal_cs != nullptr);

    ASSERT_EQ(ellipsoidal_cs->identifiers().size(), 1U);
    EXPECT_EQ(ellipsoidal_cs->identifiers()[0]->code(), "6422");
    EXPECT_EQ(*(ellipsoidal_cs->identifiers()[0]->codeSpace()), "EPSG");

    const auto &axisList = ellipsoidal_cs->axisList();
    EXPECT_EQ(axisList.size(), 2U);

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

    ASSERT_EQ(ellipsoidal_cs->identifiers().size(), 1U);
    EXPECT_EQ(ellipsoidal_cs->identifiers()[0]->code(), "6423");
    EXPECT_EQ(*(ellipsoidal_cs->identifiers()[0]->codeSpace()), "EPSG");

    const auto &axisList = ellipsoidal_cs->axisList();
    EXPECT_EQ(axisList.size(), 3U);

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

    ASSERT_EQ(cartesian_cs->identifiers().size(), 1U);
    EXPECT_EQ(cartesian_cs->identifiers()[0]->code(), "6500");
    EXPECT_EQ(*(cartesian_cs->identifiers()[0]->codeSpace()), "EPSG");

    const auto &axisList = cartesian_cs->axisList();
    EXPECT_EQ(axisList.size(), 3U);

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

    ASSERT_EQ(vertical_cs->identifiers().size(), 1U);
    EXPECT_EQ(vertical_cs->identifiers()[0]->code(), "6499");
    EXPECT_EQ(*(vertical_cs->identifiers()[0]->codeSpace()), "EPSG");

    const auto &axisList = vertical_cs->axisList();
    EXPECT_EQ(axisList.size(), 1U);

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
    ASSERT_EQ(gcrs->identifiers().size(), 1U);
    EXPECT_EQ(gcrs->identifiers()[0]->code(), "4326");
    EXPECT_EQ(*(gcrs->identifiers()[0]->codeSpace()), "EPSG");
    EXPECT_EQ(*(gcrs->name()->description()), "WGS 84");
    ASSERT_TRUE(gcrs->datum() == nullptr);
    ASSERT_TRUE(gcrs->datumEnsemble() != nullptr);
    EXPECT_TRUE(gcrs->datumEnsemble()->isEquivalentTo(
        factory->createDatumEnsemble("6326").get()));
    EXPECT_TRUE(gcrs->coordinateSystem()->isEquivalentTo(
        factory->createCoordinateSystem("6422").get()));
    auto domain = crs->domains()[0];
    auto extent = domain->domainOfValidity();
    ASSERT_TRUE(extent != nullptr);
    EXPECT_TRUE(extent->isEquivalentTo(factory->createExtent("1262").get()));

    EXPECT_EQ(crs->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=longlat +datum=WGS84 +no_defs +type=crs");
}

// ---------------------------------------------------------------------------

TEST(factory, AuthorityFactory_createGeodeticCRS_geographic2D_area_no_bbox) {
    auto factory = AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    auto crs = factory->createGeodeticCRS("4296"); // Sudan - deprecated
    auto domain = crs->domains()[0];
    auto extent = domain->domainOfValidity();
    ASSERT_TRUE(extent != nullptr);
    EXPECT_TRUE(extent->isEquivalentTo(factory->createExtent("1361").get()));
}

// ---------------------------------------------------------------------------

TEST(factory, AuthorityFactory_createGeodeticCRS_geographic3D) {
    auto factory = AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    auto crs = factory->createGeodeticCRS("4979");
    auto gcrs = nn_dynamic_pointer_cast<GeographicCRS>(crs);
    ASSERT_TRUE(gcrs != nullptr);
    ASSERT_EQ(gcrs->identifiers().size(), 1U);
    EXPECT_EQ(gcrs->identifiers()[0]->code(), "4979");
    EXPECT_EQ(*(gcrs->identifiers()[0]->codeSpace()), "EPSG");
    EXPECT_EQ(*(gcrs->name()->description()), "WGS 84");
    ASSERT_TRUE(gcrs->datum() == nullptr);
    ASSERT_TRUE(gcrs->datumEnsemble() != nullptr);
    EXPECT_TRUE(gcrs->datumEnsemble()->isEquivalentTo(
        factory->createDatumEnsemble("6326").get()));
    EXPECT_TRUE(gcrs->coordinateSystem()->isEquivalentTo(
        factory->createCoordinateSystem("6423").get()));
}

// ---------------------------------------------------------------------------

TEST(factory, AuthorityFactory_createGeodeticCRS_geocentric) {
    auto factory = AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    auto crs = factory->createGeodeticCRS("4978");
    ASSERT_TRUE(nn_dynamic_pointer_cast<GeographicCRS>(crs) == nullptr);
    ASSERT_EQ(crs->identifiers().size(), 1U);
    EXPECT_EQ(crs->identifiers()[0]->code(), "4978");
    EXPECT_EQ(*(crs->identifiers()[0]->codeSpace()), "EPSG");
    EXPECT_EQ(*(crs->name()->description()), "WGS 84");
    ASSERT_TRUE(crs->datum() == nullptr);
    ASSERT_TRUE(crs->datumEnsemble() != nullptr);
    EXPECT_TRUE(crs->datumEnsemble()->isEquivalentTo(
        factory->createDatumEnsemble("6326").get()));
    EXPECT_TRUE(crs->coordinateSystem()->isEquivalentTo(
        factory->createCoordinateSystem("6500").get()));
}

// ---------------------------------------------------------------------------

TEST(factory, AuthorityFactory_createVerticalCRS) {
    auto factory = AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    EXPECT_THROW(factory->createVerticalCRS("-1"),
                 NoSuchAuthorityCodeException);

    auto crs = factory->createVerticalCRS("3855");
    ASSERT_EQ(crs->identifiers().size(), 1U);
    EXPECT_EQ(crs->identifiers()[0]->code(), "3855");
    EXPECT_EQ(*(crs->identifiers()[0]->codeSpace()), "EPSG");
    EXPECT_EQ(*(crs->name()->description()), "EGM2008 height");
    EXPECT_TRUE(
        crs->datum()->isEquivalentTo(factory->createDatum("1027").get()));
    EXPECT_TRUE(crs->coordinateSystem()->isEquivalentTo(
        factory->createCoordinateSystem("6499").get()));

    auto domain = crs->domains()[0];
    auto extent = domain->domainOfValidity();
    ASSERT_TRUE(extent != nullptr);
    EXPECT_TRUE(extent->isEquivalentTo(factory->createExtent("1262").get()));
}

// ---------------------------------------------------------------------------

TEST(factory, AuthorityFactory_createVerticalCRS_with_datum_ensemble) {
    auto factory = AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    EXPECT_THROW(factory->createVerticalCRS("-1"),
                 NoSuchAuthorityCodeException);

    auto crs = factory->createVerticalCRS("9451"); // BI height
    ASSERT_TRUE(crs->datum() == nullptr);
    ASSERT_TRUE(crs->datumEnsemble() != nullptr);
    EXPECT_TRUE(crs->datumEnsemble()->isEquivalentTo(
        factory->createDatumEnsemble("1288").get()));
}

// ---------------------------------------------------------------------------

TEST(factory, AuthorityFactory_createConversion) {
    auto factory = AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    EXPECT_THROW(factory->createConversion("-1"), NoSuchAuthorityCodeException);

    auto conv = factory->createConversion("16031");
    ASSERT_EQ(conv->identifiers().size(), 1U);
    EXPECT_EQ(conv->identifiers()[0]->code(), "16031");
    EXPECT_EQ(*(conv->identifiers()[0]->codeSpace()), "EPSG");
    EXPECT_EQ(*(conv->name()->description()), "UTM zone 31N");

    auto method = conv->method();
    ASSERT_EQ(method->identifiers().size(), 1U);
    EXPECT_EQ(method->identifiers()[0]->code(), "9807");
    EXPECT_EQ(*(method->identifiers()[0]->codeSpace()), "EPSG");
    EXPECT_EQ(*(method->name()->description()), "Transverse Mercator");

    const auto &values = conv->parameterValues();
    ASSERT_EQ(values.size(), 5U);
    {
        const auto &opParamvalue =
            nn_dynamic_pointer_cast<OperationParameterValue>(values[0]);
        ASSERT_TRUE(opParamvalue);
        const auto &paramName =
            *(opParamvalue->parameter()->name()->description());
        const auto &parameterValue = opParamvalue->parameterValue();
        EXPECT_TRUE(opParamvalue->parameter()->getEPSGCode() == 8801);
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
        EXPECT_TRUE(opParamvalue->parameter()->getEPSGCode() == 8802);
        EXPECT_EQ(paramName, "Longitude of natural origin");
        EXPECT_EQ(parameterValue->type(), ParameterValue::Type::MEASURE);
        auto measure = parameterValue->value();
        EXPECT_EQ(measure.unit(), UnitOfMeasure::DEGREE);
        EXPECT_EQ(measure.value(), 3.0);
    }
}

// ---------------------------------------------------------------------------

TEST(factory, AuthorityFactory_createConversion_from_other_transformation) {
    auto factory = AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    auto op = factory->createCoordinateOperation("7984", false);
    auto conversion = nn_dynamic_pointer_cast<Conversion>(op);
    ASSERT_TRUE(conversion != nullptr);
}

// ---------------------------------------------------------------------------

TEST(factory, AuthorityFactory_createProjectedCRS) {
    auto factory = AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    EXPECT_THROW(factory->createProjectedCRS("-1"),
                 NoSuchAuthorityCodeException);

    auto crs = factory->createProjectedCRS("32631");
    ASSERT_EQ(crs->identifiers().size(), 1U);
    EXPECT_EQ(crs->identifiers()[0]->code(), "32631");
    EXPECT_EQ(*(crs->identifiers()[0]->codeSpace()), "EPSG");
    EXPECT_EQ(*(crs->name()->description()), "WGS 84 / UTM zone 31N");
    EXPECT_TRUE(crs->baseCRS()->isEquivalentTo(
        factory->createGeodeticCRS("4326").get()));
    EXPECT_TRUE(crs->coordinateSystem()->isEquivalentTo(
        factory->createCoordinateSystem("4400").get()));
    EXPECT_TRUE(crs->derivingConversion()->isEquivalentTo(
        factory->createConversion("16031").get()));

    auto domain = crs->domains()[0];
    auto extent = domain->domainOfValidity();
    ASSERT_TRUE(extent != nullptr);
    EXPECT_TRUE(extent->isEquivalentTo(factory->createExtent("2060").get()));
}

// ---------------------------------------------------------------------------

TEST(factory, AuthorityFactory_createProjectedCRS_south_pole) {
    auto factory = AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    EXPECT_THROW(factory->createProjectedCRS("-1"),
                 NoSuchAuthorityCodeException);

    auto crs = factory->createProjectedCRS("32761");
    auto csList = crs->coordinateSystem()->axisList();
    ASSERT_EQ(csList.size(), 2U);
    EXPECT_TRUE(csList[0]->meridian() != nullptr);
    EXPECT_EQ(csList[0]->direction(), AxisDirection::NORTH);
    EXPECT_EQ(
        csList[0]->meridian()->longitude().convertToUnit(UnitOfMeasure::DEGREE),
        0);
    EXPECT_EQ(csList[1]->direction(), AxisDirection::NORTH);
    EXPECT_EQ(
        csList[1]->meridian()->longitude().convertToUnit(UnitOfMeasure::DEGREE),
        90);
}

// ---------------------------------------------------------------------------

TEST(factory, AuthorityFactory_createProjectedCRS_north_pole) {
    auto factory = AuthorityFactory::create(DatabaseContext::create(), "EPSG");

    auto crs = factory->createProjectedCRS("32661");
    auto csList = crs->coordinateSystem()->axisList();
    ASSERT_EQ(csList.size(), 2U);
    EXPECT_TRUE(csList[0]->meridian() != nullptr);
    EXPECT_EQ(csList[0]->direction(), AxisDirection::SOUTH);
    EXPECT_EQ(
        csList[0]->meridian()->longitude().convertToUnit(UnitOfMeasure::DEGREE),
        180);
    EXPECT_EQ(csList[1]->direction(), AxisDirection::SOUTH);
    EXPECT_EQ(
        csList[1]->meridian()->longitude().convertToUnit(UnitOfMeasure::DEGREE),
        90);
}

// ---------------------------------------------------------------------------

TEST(factory, AuthorityFactory_createCompoundCRS) {
    auto factory = AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    EXPECT_THROW(factory->createCompoundCRS("-1"),
                 NoSuchAuthorityCodeException);

    auto crs = factory->createCompoundCRS("6871");
    ASSERT_EQ(crs->identifiers().size(), 1U);
    EXPECT_EQ(crs->identifiers()[0]->code(), "6871");
    EXPECT_EQ(*(crs->identifiers()[0]->codeSpace()), "EPSG");
    EXPECT_EQ(*(crs->name()->description()),
              "WGS 84 / Pseudo-Mercator +  EGM2008 geoid height");

    auto components = crs->componentReferenceSystems();
    ASSERT_EQ(components.size(), 2U);
    EXPECT_TRUE(components[0]->isEquivalentTo(
        factory->createProjectedCRS("3857").get()));
    EXPECT_TRUE(components[1]->isEquivalentTo(
        factory->createVerticalCRS("3855").get()));

    auto domain = crs->domains()[0];
    auto extent = domain->domainOfValidity();
    ASSERT_TRUE(extent != nullptr);
    EXPECT_TRUE(extent->isEquivalentTo(factory->createExtent("1262").get()));
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
    EXPECT_THROW(factory->createCoordinateOperation("-1", false),
                 NoSuchAuthorityCodeException);
    auto op = factory->createCoordinateOperation("1113", false);
    EXPECT_EQ(op->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=pipeline +step +proj=axisswap +order=2,1 +step "
              "+proj=unitconvert +xy_in=deg +xy_out=rad +step +inv "
              "+proj=longlat +a=6378249.145 +rf=293.4663077 +step +proj=push "
              "+v_3 +step +proj=cart +a=6378249.145 +rf=293.4663077 +step "
              "+proj=helmert +x=-143 +y=-90 +z=-294 +step +inv +proj=cart "
              "+ellps=WGS84 +step +proj=pop +v_3 +step +proj=unitconvert "
              "+xy_in=rad +xy_out=deg +step +proj=axisswap +order=2,1");
}

// ---------------------------------------------------------------------------

TEST(factory, AuthorityFactory_createCoordinateOperation_helmert_7_CF) {
    auto factory = AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    auto op = factory->createCoordinateOperation("7676", false);
    EXPECT_EQ(op->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=pipeline +step +proj=axisswap +order=2,1 +step "
              "+proj=unitconvert +xy_in=deg +xy_out=rad +step +proj=push +v_3 "
              "+step +proj=cart +ellps=bessel +step +proj=helmert +x=577.88891 "
              "+y=165.22205 +z=391.18289 +rx=-4.9145 +ry=0.94729 +rz=13.05098 "
              "+s=7.78664 +convention=coordinate_frame +step +inv +proj=cart "
              "+ellps=WGS84 +step +proj=pop +v_3 +step +proj=unitconvert "
              "+xy_in=rad +xy_out=deg +step +proj=axisswap +order=2,1");
}

// ---------------------------------------------------------------------------

TEST(factory, AuthorityFactory_createCoordinateOperation_helmert_7_PV) {
    auto factory = AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    auto op = factory->createCoordinateOperation("1074", false);
    auto wkt = op->exportToPROJString(PROJStringFormatter::create().get());
    EXPECT_TRUE(wkt.find("+proj=helmert +x=-275.7224 +y=94.7824 +z=340.8944 "
                         "+rx=-8.001 +ry=-4.42 +rz=-11.821 +s=1 "
                         "+convention=position_vector") != std::string::npos)
        << wkt;
}

// ---------------------------------------------------------------------------

TEST(factory, AuthorityFactory_createCoordinateOperation_helmert_8_CF) {
    auto factory = AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    auto op = factory->createCoordinateOperation("7702", false);
    auto expected = "    PARAMETER[\"Transformation reference epoch\",2002,\n"
                    "        TIMEUNIT[\"year\",31556925.445],\n"
                    "        ID[\"EPSG\",1049]],\n";

    auto wkt = op->exportToWKT(
        WKTFormatter::create(WKTFormatter::Convention::WKT2_2019).get());
    EXPECT_TRUE(wkt.find(expected) != std::string::npos) << wkt;
}

// ---------------------------------------------------------------------------

TEST(factory, AuthorityFactory_createCoordinateOperation_helmert_15_CF) {
    auto factory = AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    auto op = factory->createCoordinateOperation("6276", false);
    auto expected =
        "COORDINATEOPERATION[\"ITRF2008 to GDA94 (1)\",\n"
        "    VERSION[\"GA-Aus 2010\"],\n"
        "    SOURCECRS[\n"
        "        GEODCRS[\"ITRF2008\",\n"
        "            DYNAMIC[\n"
        "                FRAMEEPOCH[2005]],\n"
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
        "                    LENGTHUNIT[\"metre\",1]],\n"
        "            ID[\"EPSG\",5332]]],\n"
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
        "                    LENGTHUNIT[\"metre\",1]],\n"
        "            ID[\"EPSG\",4938]]],\n"
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
        "        ANGLEUNIT[\"milliarc-second\",4.84813681109536E-09],\n"
        "        ID[\"EPSG\",8608]],\n"
        "    PARAMETER[\"Y-axis rotation\",2.2578,\n"
        "        ANGLEUNIT[\"milliarc-second\",4.84813681109536E-09],\n"
        "        ID[\"EPSG\",8609]],\n"
        "    PARAMETER[\"Z-axis rotation\",2.4015,\n"
        "        ANGLEUNIT[\"milliarc-second\",4.84813681109536E-09],\n"
        "        ID[\"EPSG\",8610]],\n"
        "    PARAMETER[\"Scale difference\",9.71,\n"
        "        SCALEUNIT[\"parts per billion\",1E-09],\n"
        "        ID[\"EPSG\",8611]],\n"
        "    PARAMETER[\"Rate of change of X-axis translation\",1.42,\n"
        "        LENGTHUNIT[\"millimetres per year\",3.16887651727315E-11],\n"
        "        ID[\"EPSG\",1040]],\n"
        "    PARAMETER[\"Rate of change of Y-axis translation\",1.34,\n"
        "        LENGTHUNIT[\"millimetres per year\",3.16887651727315E-11],\n"
        "        ID[\"EPSG\",1041]],\n"
        "    PARAMETER[\"Rate of change of Z-axis translation\",0.9,\n"
        "        LENGTHUNIT[\"millimetres per year\",3.16887651727315E-11],\n"
        "        ID[\"EPSG\",1042]],\n"
        "    PARAMETER[\"Rate of change of X-axis rotation\",1.5461,\n"
        "        ANGLEUNIT[\"milliarc-seconds per "
        "year\",1.53631468932076E-16],\n"
        "        ID[\"EPSG\",1043]],\n"
        "    PARAMETER[\"Rate of change of Y-axis rotation\",1.182,\n"
        "        ANGLEUNIT[\"milliarc-seconds per "
        "year\",1.53631468932076E-16],\n"
        "        ID[\"EPSG\",1044]],\n"
        "    PARAMETER[\"Rate of change of Z-axis rotation\",1.1551,\n"
        "        ANGLEUNIT[\"milliarc-seconds per "
        "year\",1.53631468932076E-16],\n"
        "        ID[\"EPSG\",1045]],\n"
        "    PARAMETER[\"Rate of change of Scale difference\",0.109,\n"
        "        SCALEUNIT[\"parts per billion per "
        "year\",3.16887651727315E-17],\n"
        "        ID[\"EPSG\",1046]],\n"
        "    PARAMETER[\"Parameter reference epoch\",1994,\n"
        "        TIMEUNIT[\"year\",31556925.445],\n"
        "        ID[\"EPSG\",1047]],\n"
        "    OPERATIONACCURACY[0.03],\n"
        "    USAGE[\n"
        "        SCOPE[\"Geodesy.\"],\n"
        "        AREA[\"Australia - onshore and offshore to 200 nautical mile "
        "EEZ boundary. Includes Lord Howe Island, Ashmore and Cartier "
        "Islands.\"],\n"
        "        BBOX[-47.2,109.23,-8.88,163.2]],\n"
        "    ID[\"EPSG\",6276],\n"
        "    REMARK[\"RMS residuals 5mm north, 8mm east and 28mm vertical, "
        "maximum residuals 10mm north, 13mm east and 51mm vertical. Scale "
        "difference in ppb and scale difference rate in ppb/yr where "
        "1/billion = 1E-9 or nm/m.\"]]";

    EXPECT_EQ(
        op->exportToWKT(
            WKTFormatter::create(WKTFormatter::Convention::WKT2_2019).get()),
        expected);
}

// ---------------------------------------------------------------------------

TEST(factory, AuthorityFactory_createCoordinateOperation_helmert_15_PV) {
    auto factory = AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    auto op = factory->createCoordinateOperation("8069", false);
    EXPECT_EQ(op->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=helmert +x=-0.0254 +y=0.0005 +z=0.1548 +rx=-0.0001 +ry=0 "
              "+rz=-0.00026 +s=-0.01129 +dx=-0.0001 +dy=0.0005 +dz=0.0033 "
              "+drx=0 +dry=0 +drz=-2e-05 +ds=-0.00012 +t_epoch=2010 "
              "+convention=position_vector");
}

// ---------------------------------------------------------------------------

TEST(factory,
     AuthorityFactory_createCoordinateOperation_helmert_15_PV_rounding_of_drz) {
    auto factory = AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    auto op = factory->createCoordinateOperation("7932", false);
    EXPECT_EQ(op->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=helmert +x=0 +y=0 +z=0 +rx=0 +ry=0 +rz=0 +s=0 +dx=0 +dy=0 "
              "+dz=0 +drx=0.00011 +dry=0.00057 +drz=-0.00071 +ds=0 "
              "+t_epoch=1989 +convention=position_vector");
}

// ---------------------------------------------------------------------------

TEST(factory,
     AuthorityFactory_createCoordinateOperation_molodensky_badekas_PV) {
    auto factory = AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    auto op = factory->createCoordinateOperation("1066", false);

    auto so = nn_dynamic_pointer_cast<SingleOperation>(op);
    ASSERT_TRUE(so != nullptr);
    EXPECT_TRUE(so->validateParameters().empty());

    EXPECT_EQ(op->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=pipeline +step +proj=axisswap +order=2,1 +step "
              "+proj=unitconvert +xy_in=deg +xy_out=rad +step +proj=push +v_3 "
              "+step +proj=cart +ellps=bessel +step +proj=molobadekas "
              "+x=593.032 +y=26 +z=478.741 +rx=0.409394387439237 "
              "+ry=-0.359705195614311 +rz=1.86849100035057 +s=4.0772 "
              "+px=3903453.148 +py=368135.313 +pz=5012970.306 "
              "+convention=coordinate_frame +step +inv +proj=cart +ellps=GRS80 "
              "+step +proj=pop +v_3 +step +proj=unitconvert +xy_in=rad "
              "+xy_out=deg +step +proj=axisswap +order=2,1");
}

// ---------------------------------------------------------------------------

TEST(
    factory,
    AuthorityFactory_createCoordinateOperation_grid_transformation_one_parameter) {
    auto factory = AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    auto op = factory->createCoordinateOperation("1295", false);
    auto expected =
        "COORDINATEOPERATION[\"RGNC91-93 to NEA74 Noumea (4)\",\n"
        "    VERSION[\"ESRI-Ncl 0.05m\"],\n"
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
        "                    ANGLEUNIT[\"degree\",0.0174532925199433]],\n"
        "            ID[\"EPSG\",4749]]],\n"
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
        "                    ANGLEUNIT[\"degree\",0.0174532925199433]],\n"
        "            ID[\"EPSG\",4644]]],\n"
        "    METHOD[\"NTv2\",\n"
        "        ID[\"EPSG\",9615]],\n"
        "    PARAMETERFILE[\"Latitude and longitude difference "
        "file\",\"RGNC1991_NEA74Noumea.gsb\"],\n"
        "    OPERATIONACCURACY[0.05],\n"
        "    USAGE[\n"
        "        SCOPE[\"Geodesy.\"],\n"
        "        AREA[\"New Caledonia - Grande Terre - Noumea district.\"],\n"
        "        BBOX[-22.37,166.35,-22.19,166.54]],\n"
        "    ID[\"EPSG\",1295],\n"
        "    REMARK[\"Emulation using NTv2 method of tfm NEA74 Noumea to "
        "RGNC91-93 (3) (code 15943). Note reversal of sign of parameter values "
        "in grid file.\"]]";
    EXPECT_EQ(
        op->exportToWKT(
            WKTFormatter::create(WKTFormatter::Convention::WKT2_2019).get()),
        expected);
}

// ---------------------------------------------------------------------------

TEST(
    factory,
    AuthorityFactory_createCoordinateOperation_grid_transformation_two_parameter) {
    auto factory = AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    auto op = factory->createCoordinateOperation("15864", false);
    auto expected =
        "    PARAMETERFILE[\"Latitude difference file\",\"alaska.las\"],\n"
        "    PARAMETERFILE[\"Longitude difference file\",\"alaska.los\"],\n";

    auto wkt = op->exportToWKT(
        WKTFormatter::create(WKTFormatter::Convention::WKT2_2019).get());
    EXPECT_TRUE(wkt.find(expected) != std::string::npos) << wkt;
}

// ---------------------------------------------------------------------------

TEST(factory, AuthorityFactory_createCoordinateOperation_other_transformation) {
    auto factory = AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    auto op = factory->createCoordinateOperation("1884", false);
    auto expected =
        "COORDINATEOPERATION[\"S-JTSK (Ferro) to S-JTSK (1)\",\n"
        "    VERSION[\"EPSG-Cze\"],\n"
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
        "                    ANGLEUNIT[\"degree\",0.0174532925199433]],\n"
        "            ID[\"EPSG\",4818]]],\n"
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
        "                    ANGLEUNIT[\"degree\",0.0174532925199433]],\n"
        "            ID[\"EPSG\",4156]]],\n"
        "    METHOD[\"Longitude rotation\",\n"
        "        ID[\"EPSG\",9601]],\n"
        "    PARAMETER[\"Longitude offset\",-17.6666666666667,\n"
        "        ANGLEUNIT[\"degree\",0.0174532925199433],\n"
        "        ID[\"EPSG\",8602]],\n"
        "    OPERATIONACCURACY[0.0],\n"
        "    USAGE[\n"
        "        SCOPE[\"Change of prime meridian.\"],\n"
        "        AREA[\"Czechia; Slovakia.\"],\n"
        "        BBOX[47.73,12.09,51.06,22.56]],\n"
        "    ID[\"EPSG\",1884]]";

    EXPECT_EQ(
        op->exportToWKT(
            WKTFormatter::create(WKTFormatter::Convention::WKT2_2019).get()),
        expected);
}

// ---------------------------------------------------------------------------

TEST(factory, AuthorityFactory_test_uom_9110) {
    auto factory = AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    // This tests conversion from unit of measure EPSG:9110 DDD.MMSSsss
    auto crs = factory->createProjectedCRS("2172");
    EXPECT_PRED_FORMAT2(
        ComparePROJString,
        crs->exportToPROJString(PROJStringFormatter::create().get()),
        "+proj=sterea +lat_0=53.0019444444444 +lon_0=21.5027777777778 "
        "+k=0.9998 +x_0=4603000 +y_0=5806000 +ellps=krass +units=m "
        "+no_defs +type=crs");
}

// ---------------------------------------------------------------------------

TEST(factory, AuthorityFactory_affine_parametric_transform) {
    auto factory = AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    auto op = factory->createCoordinateOperation("10087", false);
    // Do not do axis unit change
    EXPECT_EQ(op->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=affine +xoff=82357.457 +s11=0.304794369 "
              "+s12=1.5417425e-05 +yoff=28091.324 +s21=-1.5417425e-05 "
              "+s22=0.304794369");
}

// ---------------------------------------------------------------------------

TEST(factory,
     AuthorityFactory_createCoordinateOperation_concatenated_operation) {
    auto factory = AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    auto op = factory->createCoordinateOperation("3896", false);
    auto concatenated = nn_dynamic_pointer_cast<ConcatenatedOperation>(op);
    ASSERT_TRUE(concatenated != nullptr);
    auto operations = concatenated->operations();
    ASSERT_EQ(operations.size(), 2U);
    EXPECT_TRUE(operations[0]->isEquivalentTo(
        factory->createCoordinateOperation("3895", false).get()));
    EXPECT_TRUE(operations[1]->isEquivalentTo(
        factory->createCoordinateOperation("1618", false).get()));
}

// ---------------------------------------------------------------------------

TEST(
    factory,
    AuthorityFactory_createCoordinateOperation_concatenated_operation_three_steps) {
    auto factory = AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    auto op = factory->createCoordinateOperation("8647", false);
    auto concatenated = nn_dynamic_pointer_cast<ConcatenatedOperation>(op);
    ASSERT_TRUE(concatenated != nullptr);
    auto operations = concatenated->operations();
    ASSERT_EQ(operations.size(), 3U);
    EXPECT_TRUE(operations[0]->isEquivalentTo(
        factory->createCoordinateOperation("1313", false).get()));
    EXPECT_TRUE(operations[1]->isEquivalentTo(
        factory->createCoordinateOperation("1950", false).get()));
    EXPECT_TRUE(operations[2]->isEquivalentTo(
        factory->createCoordinateOperation("1946", false).get()));
}

// ---------------------------------------------------------------------------

TEST(
    factory,
    AuthorityFactory_createCoordinateOperation_concatenated_operation_inverse_step1) {
    auto factory = AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    auto op = factory->createCoordinateOperation("8443", false);
    auto concatenated = nn_dynamic_pointer_cast<ConcatenatedOperation>(op);
    ASSERT_TRUE(concatenated != nullptr);
    auto operations = concatenated->operations();
    ASSERT_EQ(operations.size(), 2U);
    EXPECT_TRUE(operations[0]->isEquivalentTo(
        factory->createCoordinateOperation("8364", false)->inverse().get()));
    EXPECT_TRUE(operations[1]->isEquivalentTo(
        factory->createCoordinateOperation("8367", false).get()));
}

// ---------------------------------------------------------------------------

TEST(
    factory,
    AuthorityFactory_createCoordinateOperation_concatenated_operation_inverse_step2) {
    auto factory = AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    auto op = factory->createCoordinateOperation("7811", false);
    auto concatenated = nn_dynamic_pointer_cast<ConcatenatedOperation>(op);
    ASSERT_TRUE(concatenated != nullptr);
    auto operations = concatenated->operations();
    ASSERT_EQ(operations.size(), 2U);
    EXPECT_TRUE(operations[0]->isEquivalentTo(
        factory->createCoordinateOperation("1763", false).get()));
    EXPECT_TRUE(operations[1]->isEquivalentTo(
        factory->createCoordinateOperation("15958", false)->inverse().get()));
}

// ---------------------------------------------------------------------------

TEST(
    factory,
    AuthorityFactory_createCoordinateOperation_concatenated_operation_step1_is_conversion) {
    auto factory = AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    auto op = factory->createCoordinateOperation("7973", false);
    auto concatenated = nn_dynamic_pointer_cast<ConcatenatedOperation>(op);
    ASSERT_TRUE(concatenated != nullptr);
    auto operations = concatenated->operations();
    ASSERT_EQ(operations.size(), 2U);
    EXPECT_TRUE(operations[0]->isEquivalentTo(
        factory->createCoordinateOperation("7972", false).get()));
    EXPECT_TRUE(operations[1]->isEquivalentTo(
        factory->createCoordinateOperation("7969", false).get()));
}

// ---------------------------------------------------------------------------

TEST(
    factory,
    AuthorityFactory_createCoordinateOperation_concatenated_operation_epsg_9103) {
    auto factory = AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    auto op = factory->createCoordinateOperation("9103", false);
    auto concatenated = nn_dynamic_pointer_cast<ConcatenatedOperation>(op);
    ASSERT_TRUE(concatenated != nullptr);
    auto operations = concatenated->operations();
    ASSERT_EQ(operations.size(),
              5U); // we've added an explicit geographic -> geocentric step
    EXPECT_EQ(operations[0]->nameStr(), "NAD27 to NAD83 (1)");
    EXPECT_EQ(operations[1]->nameStr(), "NAD83 to NAD83(2011) (1)");
    EXPECT_EQ(
        operations[2]->nameStr(),
        "Conversion from NAD83(2011) (geog2D) to NAD83(2011) (geocentric)");
    EXPECT_EQ(operations[3]->nameStr(),
              "Inverse of ITRF2008 to NAD83(2011) (1)");
    EXPECT_EQ(operations[4]->nameStr(), "ITRF2008 to ITRF2014 (1)");
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
            EXPECT_THROW(factory->createCoordinateOperation(code, false),
                         FactoryException)
                << code;
        } else {
            factory->createCoordinateOperation(code, false);
        }
    }
}

// ---------------------------------------------------------------------------

TEST(factory, AuthorityFactory_createCoordinateOperation_conversion) {
    auto factory = AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    auto op = factory->createCoordinateOperation("16031", false);
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

        auto setDynamicGeodeticDatum = factory->getAuthorityCodes(
            AuthorityFactory::ObjectType::DYNAMIC_GEODETIC_REFERENCE_FRAME);
        ASSERT_TRUE(!setDynamicGeodeticDatum.empty());
        auto dgrf =
            factory->createGeodeticDatum(*(setDynamicGeodeticDatum.begin()));
        EXPECT_TRUE(dynamic_cast<DynamicGeodeticReferenceFrame *>(dgrf.get()) !=
                    nullptr);
        EXPECT_LT(setDynamicGeodeticDatum.size(), setGeodeticDatum.size());

        auto setVerticalDatum = factory->getAuthorityCodes(
            AuthorityFactory::ObjectType::VERTICAL_REFERENCE_FRAME);
        ASSERT_TRUE(!setVerticalDatum.empty());
        factory->createVerticalDatum(*(setVerticalDatum.begin()));

        auto setDynamicVerticalDatum = factory->getAuthorityCodes(
            AuthorityFactory::ObjectType::DYNAMIC_VERTICAL_REFERENCE_FRAME);
        ASSERT_TRUE(!setDynamicVerticalDatum.empty());
        auto dvrf =
            factory->createVerticalDatum(*(setDynamicVerticalDatum.begin()));
        EXPECT_TRUE(dynamic_cast<DynamicVerticalReferenceFrame *>(dvrf.get()) !=
                    nullptr);
        EXPECT_LT(setDynamicVerticalDatum.size(), setVerticalDatum.size());

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

        auto setGeocentricCRS = factory->getAuthorityCodes(
            AuthorityFactory::ObjectType::GEOCENTRIC_CRS);
        ASSERT_TRUE(!setGeocentricCRS.empty());
        factory->createGeodeticCRS(*(setGeocentricCRS.begin()));
        EXPECT_LT(setGeocentricCRS.size(), setGeodeticCRS.size());

        auto setGeographicCRS = factory->getAuthorityCodes(
            AuthorityFactory::ObjectType::GEOGRAPHIC_CRS);
        ASSERT_TRUE(!setGeographicCRS.empty());
        factory->createGeographicCRS(*(setGeographicCRS.begin()));
        EXPECT_LT(setGeographicCRS.size(), setGeodeticCRS.size());
        for (const auto &v : setGeographicCRS) {
            EXPECT_TRUE(setGeodeticCRS.find(v) != setGeodeticCRS.end());
        }

        auto setGeographic2DCRS = factory->getAuthorityCodes(
            AuthorityFactory::ObjectType::GEOGRAPHIC_2D_CRS);
        ASSERT_TRUE(!setGeographic2DCRS.empty());
        factory->createGeographicCRS(*(setGeographic2DCRS.begin()));

        auto setGeographic3DCRS = factory->getAuthorityCodes(
            AuthorityFactory::ObjectType::GEOGRAPHIC_3D_CRS);
        ASSERT_TRUE(!setGeographic3DCRS.empty());
        factory->createGeographicCRS(*(setGeographic3DCRS.begin()));

        EXPECT_EQ(setGeographic2DCRS.size() + setGeographic3DCRS.size(),
                  setGeographicCRS.size());

        EXPECT_EQ(setGeocentricCRS.size() + setGeographicCRS.size(),
                  setGeodeticCRS.size());

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
        factory->createCoordinateOperation(*(setCO.begin()), false);

        auto setConversion = factory->getAuthorityCodes(
            AuthorityFactory::ObjectType::CONVERSION);
        ASSERT_TRUE(!setConversion.empty());
        factory->createConversion(*(setConversion.begin()));

        auto setTransformation = factory->getAuthorityCodes(
            AuthorityFactory::ObjectType::TRANSFORMATION);
        ASSERT_TRUE(!setTransformation.empty());
        ASSERT_TRUE(nn_dynamic_pointer_cast<Transformation>(
                        factory->createCoordinateOperation(
                            *(setTransformation.begin()), false)) != nullptr);

        auto setConcatenated = factory->getAuthorityCodes(
            AuthorityFactory::ObjectType::CONCATENATED_OPERATION);
        ASSERT_TRUE(!setConcatenated.empty());
        ASSERT_TRUE(nn_dynamic_pointer_cast<ConcatenatedOperation>(
                        factory->createCoordinateOperation(
                            *(setConcatenated.begin()), false)) != nullptr);

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
              "RGF93 to NGF-IGN69 height (1)");

    // Several objects have 4326 code, including an area of use, but return
    // the CRS one.
    EXPECT_EQ(factory->getDescriptionText("4326"), "WGS 84");
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
        const auto dbStructure = referenceDb->getDatabaseStructure();
        for (const auto &sql : dbStructure) {
            ASSERT_TRUE(execute(sql)) << last_error();
        }
        ASSERT_TRUE(execute("PRAGMA foreign_keys = 1;")) << last_error();
    }

    void populateWithFakeEPSG() {

        ASSERT_TRUE(execute("INSERT INTO unit_of_measure "
                            "VALUES('EPSG','9001','metre','length',1.0,NULL,"
                            "0);"))
            << last_error();
        ASSERT_TRUE(execute("INSERT INTO unit_of_measure "
                            "VALUES('EPSG','9102','degree','angle',1."
                            "74532925199432781271e-02,NULL,0);"))
            << last_error();
        ASSERT_TRUE(execute(
            "INSERT INTO unit_of_measure VALUES('EPSG','9122','degree "
            "(supplier to "
            "define representation)','angle',1.74532925199432781271e-02,NULL,"
            "0);"))
            << last_error();
        ASSERT_TRUE(
            execute("INSERT INTO extent "
                    "VALUES('EPSG','1262','World','World.',-90.0,90.0,-180."
                    "0,180.0,0);"))
            << last_error();
        ASSERT_TRUE(
            execute("INSERT INTO scope VALUES('EPSG','1024','Not known.',0);"))
            << last_error();
        ASSERT_TRUE(
            execute("INSERT INTO prime_meridian "
                    "VALUES('EPSG','8901','Greenwich',0.0,'EPSG','9102',0);"))
            << last_error();
        ASSERT_TRUE(
            execute("INSERT INTO celestial_body VALUES('PROJ','EARTH','Earth',"
                    "6378137.0);"))
            << last_error();
        ASSERT_TRUE(
            execute("INSERT INTO ellipsoid VALUES('EPSG','7030','WGS 84','',"
                    "'PROJ','EARTH',6378137.0,'EPSG','9001',298.257223563,"
                    "NULL,0);"))
            << last_error();
        ASSERT_TRUE(
            execute("INSERT INTO geodetic_datum "
                    "VALUES('EPSG','6326','World Geodetic System 1984','',"
                    "'EPSG','7030','EPSG','8901',NULL,NULL,NULL,0);"))
            << last_error();
        ASSERT_TRUE(execute("INSERT INTO usage VALUES('EPSG',"
                            "'geodetic_datum_6326_usage','geodetic_datum',"
                            "'EPSG','6326','EPSG','1262','EPSG','1024');"))
            << last_error();
        ASSERT_TRUE(
            execute("INSERT INTO vertical_datum VALUES('EPSG','1027','EGM2008 "
                    "geoid',NULL,NULL,NULL,NULL,0);"))
            << last_error();
        ASSERT_TRUE(execute("INSERT INTO usage VALUES('EPSG',"
                            "'vertical_datum_1027_usage','vertical_datum',"
                            "'EPSG','1027','EPSG','1262','EPSG','1024');"))
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
            execute("INSERT INTO geodetic_crs VALUES('EPSG','4326','WGS "
                    "84',NULL,'geographic "
                    "2D','EPSG','6422','EPSG','6326',NULL,0);"))
            << last_error();
        ASSERT_TRUE(execute("INSERT INTO usage VALUES('EPSG',"
                            "'geodetic_crs4326_usage','geodetic_crs',"
                            "'EPSG','4326','EPSG','1262','EPSG','1024');"))
            << last_error();

        ASSERT_TRUE(execute("INSERT INTO coordinate_system "
                            "VALUES('EPSG','6499','vertical',1);"))
            << last_error();
        ASSERT_TRUE(
            execute("INSERT INTO axis VALUES('EPSG','114','Gravity-related "
                    "height','H','up','EPSG','6499',1,'EPSG','9001');"))
            << last_error();
        ASSERT_TRUE(
            execute("INSERT INTO vertical_crs VALUES('EPSG','3855','EGM2008 "
                    "height',NULL,'EPSG','6499','EPSG','1027',0);"))
            << last_error();
        ASSERT_TRUE(execute("INSERT INTO usage VALUES('EPSG',"
                            "'vertical_crs3855_usage','vertical_crs',"
                            "'EPSG','3855','EPSG','1262','EPSG','1024');"))
            << last_error();

        ASSERT_TRUE(execute("INSERT INTO unit_of_measure "
                            "VALUES('EPSG','9201','unity','scale',1.0,"
                            "NULL,0);"))
            << last_error();

        ASSERT_TRUE(execute(
            "INSERT INTO extent VALUES('EPSG','1933','World - N hemisphere - "
            "0°E to 6°E','',0.0,84.0,0.0,6.0,0);"))
            << last_error();

        ASSERT_TRUE(execute(
            "INSERT INTO conversion VALUES('EPSG','16031','UTM zone "
            "31N',NULL,'EPSG','9807','Transverse "
            "Mercator','EPSG','8801','Latitude "
            "of "
            "natural origin',0.0,'EPSG','9102','EPSG','8802','Longitude of "
            "natural "
            "origin',3.0,'EPSG','9102','EPSG','8805','Scale factor at natural "
            "origin',0.9996,'EPSG','9201','EPSG','8806','False "
            "easting',500000.0,'EPSG','9001','EPSG','8807','False "
            "northing',0.0,'EPSG','9001',NULL,NULL,NULL,NULL,NULL,NULL,NULL,"
            "NULL,"
            "NULL,NULL,NULL,NULL,0);"))
            << last_error();
        ASSERT_TRUE(execute("INSERT INTO usage VALUES('EPSG',"
                            "'conversion16031_usage','conversion',"
                            "'EPSG','16031','EPSG','1933','EPSG','1024');"))
            << last_error();

        ASSERT_TRUE(execute(
            "INSERT INTO extent VALUES('EPSG','2060','World - N hemisphere - "
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

        ASSERT_TRUE(execute("INSERT INTO projected_crs "
                            "VALUES('EPSG','32631','WGS 84 / UTM zone "
                            "31N',NULL,'EPSG','4400','EPSG','4326',"
                            "'EPSG','16031',NULL,0);"))
            << last_error();
        ASSERT_TRUE(execute("INSERT INTO usage VALUES('EPSG',"
                            "'projected_crs32631_usage','projected_crs',"
                            "'EPSG','32631','EPSG','2060','EPSG','1024');"))
            << last_error();

        ASSERT_TRUE(execute(
            "INSERT INTO compound_crs VALUES('EPSG','MY_COMPOUND','WGS 84 + "
            "EGM2008 geoid height',NULL,'EPSG','4326','EPSG','3855',0);"))
            << last_error();
        ASSERT_TRUE(
            execute("INSERT INTO usage VALUES('EPSG',"
                    "'compound_crsMY_COMPOUND_usage','compound_crs',"
                    "'EPSG','MY_COMPOUND','EPSG','1262','EPSG','1024');"))
            << last_error();

        ASSERT_TRUE(execute(
            "INSERT INTO helmert_transformation "
            "VALUES('EPSG','DUMMY_HELMERT','name',NULL,'EPSG','9603','"
            "Geocentric translations (geog2D domain)','EPSG','4326',"
            "'EPSG','4326',44.0,-143."
            "0,-90.0,-294.0,'EPSG','9001',NULL,NULL,NULL,NULL,NULL,NULL,"
            "NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,"
            "NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0);"))
            << last_error();
        ASSERT_TRUE(
            execute("INSERT INTO usage VALUES('EPSG',"
                    "'helmert_transformation_DUMMY_HELMERT_usage',"
                    "'helmert_transformation',"
                    "'EPSG','DUMMY_HELMERT','EPSG','1262','EPSG','1024');"))
            << last_error();

        ASSERT_TRUE(execute(
            "INSERT INTO grid_transformation "
            "VALUES('EPSG','DUMMY_GRID_TRANSFORMATION','name',NULL,"
            "'EPSG','9615'"
            ",'NTv2','EPSG','4326','EPSG','4326',1.0,'EPSG','"
            "8656','Latitude and longitude difference "
            "file','nzgd2kgrid0005.gsb',NULL,NULL,NULL,NULL,NULL,NULL,NULL,"
            "0);"))
            << last_error();
        ASSERT_TRUE(
            execute("INSERT INTO usage VALUES('EPSG',"
                    "'grid_transformation_DUMMY_GRID_TRANSFORMATION_usage',"
                    "'grid_transformation',"
                    "'EPSG','DUMMY_GRID_TRANSFORMATION',"
                    "'EPSG','1262','EPSG','1024');"))
            << last_error();

        ASSERT_TRUE(execute(
            "INSERT INTO unit_of_measure VALUES('EPSG','9110','sexagesimal "
            "DMS','angle',NULL,NULL,0);"))
            << last_error();

        ASSERT_TRUE(execute(
            "INSERT INTO other_transformation "
            "VALUES('EPSG','DUMMY_OTHER_TRANSFORMATION','name',NULL,"
            "'EPSG','9601','Longitude rotation',"
            "'EPSG','4326','EPSG','4326',0.0,'EPSG'"
            ",'8602','Longitude "
            "offset',-17.4,'EPSG','9110',NULL,NULL,NULL,NULL,NULL,NULL,"
            "NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,"
            "NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,"
            "NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0);"))
            << last_error();
        ASSERT_TRUE(
            execute("INSERT INTO usage VALUES('EPSG',"
                    "'other_transformation_DUMMY_OTHER_TRANSFORMATION_usage',"
                    "'other_transformation',"
                    "'EPSG','DUMMY_OTHER_TRANSFORMATION',"
                    "'EPSG','1262','EPSG','1024');"))
            << last_error();

        ASSERT_TRUE(execute("INSERT INTO concatenated_operation "
                            "VALUES('EPSG','DUMMY_CONCATENATED','name',NULL,"
                            "'EPSG','4326','EPSG'"
                            ",'4326',NULL,NULL,0);"))
            << last_error();
        ASSERT_TRUE(execute("INSERT INTO usage VALUES('EPSG',"
                            "'concatenated_operation_DUMMY_CONCATENATED_usage',"
                            "'concatenated_operation',"
                            "'EPSG','DUMMY_CONCATENATED',"
                            "'EPSG','1262','EPSG','1024');"))
            << last_error();

        ASSERT_TRUE(execute("INSERT INTO concatenated_operation_step "
                            "VALUES('EPSG','DUMMY_CONCATENATED',1,"
                            "'EPSG','DUMMY_OTHER_TRANSFORMATION');"))
            << last_error();

        ASSERT_TRUE(execute("INSERT INTO concatenated_operation_step "
                            "VALUES('EPSG','DUMMY_CONCATENATED',2,"
                            "'EPSG','DUMMY_OTHER_TRANSFORMATION');"))
            << last_error();
    }

    void createSourceTargetPivotCRS() {
        const auto vals = std::vector<std::string>{"SOURCE", "TARGET", "PIVOT"};
        for (const auto &val : vals) {

            ASSERT_TRUE(execute("INSERT INTO geodetic_datum "
                                "VALUES('FOO','" +
                                val + "','" + val +
                                "','',"
                                "'EPSG','7030','EPSG','8901',"
                                "NULL,NULL,NULL,0);"))
                << last_error();
            ASSERT_TRUE(execute("INSERT INTO usage VALUES('FOO',"
                                "'geodetic_datum_" +
                                val + "_usage',"
                                      "'geodetic_datum',"
                                      "'FOO','" +
                                val + "',"
                                      "'EPSG','1262','EPSG','1024');"))
                << last_error();

            ASSERT_TRUE(execute("INSERT INTO geodetic_crs "
                                "VALUES('NS_" +
                                val + "','" + val + "','" + val +
                                "',NULL,'geographic 2D','EPSG','6422',"
                                "'FOO','" +
                                val + "',NULL,0);"))
                << last_error();
            ASSERT_TRUE(execute("INSERT INTO usage VALUES('FOO',"
                                "'geodetic_crs_" +
                                val + "_usage',"
                                      "'geodetic_crs',"
                                      "'NS_" +
                                val + "','" + val +
                                "','EPSG','1262','EPSG','1024');"))
                << last_error();
        }
    }

    void createTransformationForPivotTesting(const std::string &src,
                                             const std::string &dst) {

        ASSERT_TRUE(execute(
            "INSERT INTO helmert_transformation "
            "VALUES('OTHER','" +
            src + "_" + dst + "','Transformation from " + src + " to " + dst +
            "',NULL,'EPSG','9603','"
            "Geocentric translations (geog2D domain)','NS_" +
            src + "','" + src + "','NS_" + dst + "','" + dst +
            "',1.0,0,0,0,'EPSG','9001',NULL,NULL,NULL,NULL,NULL,NULL,"
            "NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,"
            "NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0);"))
            << last_error();
        ASSERT_TRUE(execute("INSERT INTO usage VALUES('OTHER',"
                            "'helmert_transformation" +
                            src + '_' + dst + "_usage',"
                                              "'helmert_transformation',"
                                              "'OTHER','" +
                            src + "_" + dst + "',"
                                              "'EPSG','1262','EPSG','1024');"))
            << last_error();
    }

    void checkSourceToOther() {
        {
            auto factoryOTHER = AuthorityFactory::create(
                DatabaseContext::create(m_ctxt), "OTHER");
            auto res = factoryOTHER->createFromCRSCodesWithIntermediates(
                "NS_SOURCE", "SOURCE", "NS_TARGET", "TARGET", false, false,
                false, false, {});
            EXPECT_EQ(res.size(), 1U);
            EXPECT_TRUE(res.empty() ||
                        nn_dynamic_pointer_cast<ConcatenatedOperation>(res[0]));

            res = factoryOTHER->createFromCRSCodesWithIntermediates(
                "NS_SOURCE", "SOURCE", "NS_TARGET", "TARGET", false, false,
                false, false, {std::make_pair(std::string("NS_PIVOT"),
                                              std::string("PIVOT"))});
            EXPECT_EQ(res.size(), 1U);
            EXPECT_TRUE(res.empty() ||
                        nn_dynamic_pointer_cast<ConcatenatedOperation>(res[0]));

            res = factoryOTHER->createFromCRSCodesWithIntermediates(
                "NS_SOURCE", "SOURCE", "NS_TARGET", "TARGET", false, false,
                false, false, {std::make_pair(std::string("NS_PIVOT"),
                                              std::string("NOT_EXISTING"))});
            EXPECT_EQ(res.size(), 0U);

            res = factoryOTHER->createFromCRSCodesWithIntermediates(
                "NS_SOURCE", "SOURCE", "NS_TARGET", "TARGET", false, false,
                false, false,
                {std::make_pair(std::string("BAD_NS"), std::string("PIVOT"))});
            EXPECT_EQ(res.size(), 0U);

            res = factoryOTHER->createFromCRSCodesWithIntermediates(
                "NS_TARGET", "TARGET", "NS_SOURCE", "SOURCE", false, false,
                false, false, {});
            EXPECT_EQ(res.size(), 1U);
            EXPECT_TRUE(res.empty() ||
                        nn_dynamic_pointer_cast<ConcatenatedOperation>(res[0]));
        }
        {
            auto factory = AuthorityFactory::create(
                DatabaseContext::create(m_ctxt), std::string());
            auto res = factory->createFromCRSCodesWithIntermediates(
                "NS_SOURCE", "SOURCE", "NS_TARGET", "TARGET", false, false,
                false, false, {});
            EXPECT_EQ(res.size(), 1U);
            EXPECT_TRUE(res.empty() ||
                        nn_dynamic_pointer_cast<ConcatenatedOperation>(res[0]));

            auto srcCRS = AuthorityFactory::create(
                              DatabaseContext::create(m_ctxt), "NS_SOURCE")
                              ->createCoordinateReferenceSystem("SOURCE");
            auto targetCRS = AuthorityFactory::create(
                                 DatabaseContext::create(m_ctxt), "NS_TARGET")
                                 ->createCoordinateReferenceSystem("TARGET");

            {
                auto ctxt =
                    CoordinateOperationContext::create(factory, nullptr, 0);
                res = CoordinateOperationFactory::create()->createOperations(
                    srcCRS, targetCRS, ctxt);
                EXPECT_EQ(res.size(), 1U);
                EXPECT_TRUE(
                    res.empty() ||
                    nn_dynamic_pointer_cast<ConcatenatedOperation>(res[0]));
            }

            {
                auto ctxt =
                    CoordinateOperationContext::create(factory, nullptr, 0);
                ctxt->setIntermediateCRS({std::make_pair(
                    std::string("NS_PIVOT"), std::string("PIVOT"))});
                res = CoordinateOperationFactory::create()->createOperations(
                    srcCRS, targetCRS, ctxt);
                EXPECT_EQ(res.size(), 1U);
                EXPECT_TRUE(
                    res.empty() ||
                    nn_dynamic_pointer_cast<ConcatenatedOperation>(res[0]));
            }

            {
                auto ctxt =
                    CoordinateOperationContext::create(factory, nullptr, 0);
                ctxt->setAllowUseIntermediateCRS(
                    CoordinateOperationContext::IntermediateCRSUse::NEVER);
                res = CoordinateOperationFactory::create()->createOperations(
                    srcCRS, targetCRS, ctxt);
                EXPECT_EQ(res.size(), 1U);
                EXPECT_TRUE(res.empty() ||
                            nn_dynamic_pointer_cast<Transformation>(res[0]));
            }

            {
                auto ctxt =
                    CoordinateOperationContext::create(factory, nullptr, 0);
                ctxt->setIntermediateCRS({std::make_pair(
                    std::string("NS_PIVOT"), std::string("NOT_EXISTING"))});
                res = CoordinateOperationFactory::create()->createOperations(
                    srcCRS, targetCRS, ctxt);
                EXPECT_EQ(res.size(), 1U);
                EXPECT_TRUE(res.empty() ||
                            nn_dynamic_pointer_cast<Transformation>(res[0]));
            }
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

    bool execute(const std::string &sql) {
        return sqlite3_exec(m_ctxt, sql.c_str(), nullptr, nullptr, nullptr) ==
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
        ASSERT_EQ(res.size(), 1U);
        EXPECT_TRUE(res[0]->sourceCRS() != nullptr);
        EXPECT_TRUE(res[0]->targetCRS() != nullptr);
        EXPECT_TRUE(
            res[0]->isEquivalentTo(factory->createConversion("16031").get()));
    }
    {
        auto res =
            factory->createFromCoordinateReferenceSystemCodes("4209", "4326");
        EXPECT_TRUE(!res.empty());
        for (const auto &conv : res) {
            EXPECT_TRUE(conv->sourceCRS()->getEPSGCode() == 4209);
            EXPECT_TRUE(conv->targetCRS()->getEPSGCode() == 4326);
            EXPECT_FALSE(conv->isDeprecated());
        }
    }
    {
        auto list =
            factory->createFromCoordinateReferenceSystemCodes("4179", "4258");
        ASSERT_EQ(list.size(), 3U);
        // Romania has a larger area than Poland (given our approx formula)
        EXPECT_EQ(list[0]->getEPSGCode(), 15994); // Romania - 3m
        EXPECT_EQ(list[1]->getEPSGCode(), 15993); // Romania - 10m
        EXPECT_EQ(list[2]->getEPSGCode(), 1644);  // Poland - 1m
    }
    {
        // Test removal of superseded transform
        auto list = factory->createFromCoordinateReferenceSystemCodes(
            "EPSG", "4179", "EPSG", "4258", false, false, false, true);
        ASSERT_EQ(list.size(), 2U);
        // Romania has a larger area than Poland (given our approx formula)
        EXPECT_EQ(list[0]->getEPSGCode(), 15994); // Romania - 3m
        EXPECT_EQ(list[1]->getEPSGCode(), 1644);  // Poland - 1m
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
            "EPSG", "4326", "EPSG", "32631", false, false, false, false);
        ASSERT_EQ(res.size(), 1U);
    }
    {
        auto res = factory->createFromCoordinateReferenceSystemCodes(
            "EPSG", "4209", "EPSG", "4326", false, false, false, false);
        EXPECT_TRUE(!res.empty());
        for (const auto &conv : res) {
            EXPECT_TRUE(conv->sourceCRS()->getEPSGCode() == 4209);
            EXPECT_TRUE(conv->targetCRS()->getEPSGCode() == 4326);
            EXPECT_FALSE(conv->isDeprecated());
        }
    }
}

// ---------------------------------------------------------------------------

TEST_F(FactoryWithTmpDatabase,
       AuthorityFactory_test_with_fake_EPSG_and_OTHER_database) {
    createStructure();
    populateWithFakeEPSG();

    ASSERT_TRUE(
        execute("INSERT INTO geodetic_crs VALUES('OTHER','OTHER_4326','WGS "
                "84',NULL,'geographic "
                "2D','EPSG','6422','EPSG','6326',NULL,0);"))
        << last_error();
    ASSERT_TRUE(execute("INSERT INTO usage VALUES('OTHER',"
                        "'geodetic_crs_OTHER_4326_usage','geodetic_crs',"
                        "'OTHER','OTHER_4326','EPSG','1262','EPSG','1024');"))
        << last_error();

    ASSERT_TRUE(execute("INSERT INTO projected_crs "
                        "VALUES('OTHER','OTHER_32631','WGS 84 / UTM zone "
                        "31N',NULL,'EPSG','4400','OTHER','OTHER_4326',"
                        "'EPSG','16031',NULL,0);"))
        << last_error();
    ASSERT_TRUE(execute("INSERT INTO usage VALUES('OTHER',"
                        "'projected_crs_OTHER_32631_usage','projected_crs',"
                        "'OTHER','OTHER_32631','EPSG','2060','EPSG','1024');"))
        << last_error();

    auto factoryGeneral = AuthorityFactory::create(
        DatabaseContext::create(m_ctxt), std::string());
    {
        auto res = factoryGeneral->createFromCoordinateReferenceSystemCodes(
            "OTHER", "OTHER_4326", "OTHER", "OTHER_32631", false, false, false,
            false);
        ASSERT_EQ(res.size(), 1U);
    }

    auto factoryEPSG =
        AuthorityFactory::create(DatabaseContext::create(m_ctxt), "EPSG");
    {
        auto res = factoryEPSG->createFromCoordinateReferenceSystemCodes(
            "OTHER", "OTHER_4326", "OTHER", "OTHER_32631", false, false, false,
            false);
        ASSERT_EQ(res.size(), 1U);
    }

    auto factoryOTHER =
        AuthorityFactory::create(DatabaseContext::create(m_ctxt), "OTHER");
    {
        auto res = factoryOTHER->createFromCoordinateReferenceSystemCodes(
            "OTHER_4326", "OTHER_32631");
        ASSERT_EQ(res.size(), 0U); // the conversion is in the EPSG space
    }

    ASSERT_TRUE(execute(
        "INSERT INTO grid_transformation "
        "VALUES('OTHER','OTHER_GRID_TRANSFORMATION','name',NULL,"
        "'EPSG','9615'"
        ",'NTv2','EPSG','4326','OTHER','OTHER_4326',1.0,'EPSG','"
        "8656','Latitude and longitude difference "
        "file','nzgd2kgrid0005.gsb',NULL,NULL,NULL,NULL,NULL,NULL,NULL,0);"))
        << last_error();
    ASSERT_TRUE(execute(
        "INSERT INTO usage VALUES('OTHER',"
        "'grid_transformation_OTHER_GRID_TRANSFORMATION_usage',"
        "'grid_transformation',"
        "'OTHER','OTHER_GRID_TRANSFORMATION','EPSG','1262','EPSG','1024');"))
        << last_error();

    {
        auto res = factoryGeneral->createFromCoordinateReferenceSystemCodes(
            "EPSG", "4326", "OTHER", "OTHER_4326", false, false, false, false);
        ASSERT_EQ(res.size(), 1U);
    }
    {
        auto res = factoryEPSG->createFromCoordinateReferenceSystemCodes(
            "EPSG", "4326", "OTHER", "OTHER_4326", false, false, false, false);
        ASSERT_EQ(res.size(), 0U);
    }
    {
        auto res = factoryOTHER->createFromCoordinateReferenceSystemCodes(
            "EPSG", "4326", "OTHER", "OTHER_4326", false, false, false, false);
        ASSERT_EQ(res.size(), 1U);
    }
}

// ---------------------------------------------------------------------------

TEST_F(FactoryWithTmpDatabase,
       AuthorityFactory_test_sorting_of_coordinate_operations) {
    createStructure();
    populateWithFakeEPSG();

    ASSERT_TRUE(execute(
        "INSERT INTO grid_transformation "
        "VALUES('OTHER','TRANSFORMATION_10M','TRANSFORMATION_10M',NULL,"
        "'EPSG','9615'"
        ",'NTv2','EPSG','4326','EPSG','4326',10.0,'EPSG','"
        "8656','Latitude and longitude difference "
        "file','nzgd2kgrid0005.gsb',NULL,NULL,NULL,NULL,NULL,NULL,NULL,0);"))
        << last_error();
    ASSERT_TRUE(
        execute("INSERT INTO usage VALUES('OTHER',"
                "'grid_transformation_TTRANSFORMATION_10M_usage',"
                "'grid_transformation',"
                "'OTHER','TRANSFORMATION_10M','EPSG','1262','EPSG','1024');"))
        << last_error();

    ASSERT_TRUE(
        execute("INSERT INTO grid_transformation "
                "VALUES('OTHER','TRANSFORMATION_1M_SMALL_EXTENT','"
                "TRANSFORMATION_1M_SMALL_EXTENT',NULL,'EPSG','9615'"
                ",'NTv2','EPSG','4326','EPSG','4326',1.0,'EPSG','"
                "8656','Latitude and longitude difference "
                "file','nzgd2kgrid0005.gsb',NULL,NULL,NULL,NULL,NULL,NULL,"
                "NULL,0);"))
        << last_error();
    ASSERT_TRUE(
        execute("INSERT INTO usage VALUES('OTHER',"
                "'grid_transformation_TRANSFORMATION_1M_SMALL_EXTENT_usage',"
                "'grid_transformation',"
                "'OTHER','TRANSFORMATION_1M_SMALL_EXTENT',"
                "'EPSG','2060','EPSG','1024');"))
        << last_error();

    ASSERT_TRUE(execute(
        "INSERT INTO grid_transformation "
        "VALUES('OTHER','TRANSFORMATION_1M','TRANSFORMATION_1M',NULL,"
        "'EPSG','9615'"
        ",'NTv2','EPSG','4326','EPSG','4326',1.0,'EPSG','"
        "8656','Latitude and longitude difference "
        "file','nzgd2kgrid0005.gsb',NULL,NULL,NULL,NULL,NULL,NULL,NULL,0);"))
        << last_error();
    ASSERT_TRUE(
        execute("INSERT INTO usage VALUES('OTHER',"
                "'grid_transformation_TRANSFORMATION_1M_usage',"
                "'grid_transformation',"
                "'OTHER','TRANSFORMATION_1M','EPSG','1262','EPSG','1024');"))
        << last_error();

    ASSERT_TRUE(
        execute("INSERT INTO grid_transformation "
                "VALUES('OTHER','TRANSFORMATION_0.5M_DEPRECATED','"
                "TRANSFORMATION_0.5M_DEPRECATED',NULL,'EPSG','9615'"
                ",'NTv2','EPSG','4326','EPSG','4326',1.0,'EPSG','"
                "8656','Latitude and longitude difference "
                "file','nzgd2kgrid0005.gsb',NULL,NULL,NULL,NULL,NULL,NULL,"
                "NULL,1);"))
        << last_error();
    ASSERT_TRUE(
        execute("INSERT INTO usage VALUES('OTHER',"
                "'grid_transformation_TRANSFORMATION_0.5M_DEPRECATED_usage',"
                "'grid_transformation',"
                "'OTHER','TRANSFORMATION_0.5M_DEPRECATED',"
                "'EPSG','1262','EPSG','1024');"))
        << last_error();

    auto factoryOTHER =
        AuthorityFactory::create(DatabaseContext::create(m_ctxt), "OTHER");
    auto res = factoryOTHER->createFromCoordinateReferenceSystemCodes(
        "EPSG", "4326", "EPSG", "4326", false, false, false, false);
    ASSERT_EQ(res.size(), 3U);
    EXPECT_EQ(*(res[0]->name()->description()), "TRANSFORMATION_1M");
    EXPECT_EQ(*(res[1]->name()->description()), "TRANSFORMATION_10M");
    EXPECT_EQ(*(res[2]->name()->description()),
              "TRANSFORMATION_1M_SMALL_EXTENT");
}

// ---------------------------------------------------------------------------

TEST_F(
    FactoryWithTmpDatabase,
    AuthorityFactory_createFromCRSCodesWithIntermediates_source_equals_target) {
    createStructure();
    populateWithFakeEPSG();

    auto factory = AuthorityFactory::create(DatabaseContext::create(m_ctxt),
                                            std::string());
    auto res = factory->createFromCRSCodesWithIntermediates(
        "EPSG", "4326", "EPSG", "4326", false, false, false, false, {});
    EXPECT_EQ(res.size(), 0U);
}

// ---------------------------------------------------------------------------

TEST_F(
    FactoryWithTmpDatabase,
    AuthorityFactory_createFromCRSCodesWithIntermediates_case_source_pivot_target_pivot) {
    createStructure();
    populateWithFakeEPSG();
    createSourceTargetPivotCRS();

    createTransformationForPivotTesting("SOURCE", "PIVOT");
    createTransformationForPivotTesting("TARGET", "PIVOT");

    checkSourceToOther();
}

// ---------------------------------------------------------------------------

TEST_F(
    FactoryWithTmpDatabase,
    AuthorityFactory_createFromCRSCodesWithIntermediates_case_source_pivot_pivot_target) {
    createStructure();
    populateWithFakeEPSG();
    createSourceTargetPivotCRS();

    createTransformationForPivotTesting("SOURCE", "PIVOT");
    createTransformationForPivotTesting("PIVOT", "TARGET");

    checkSourceToOther();
}

// ---------------------------------------------------------------------------

TEST_F(
    FactoryWithTmpDatabase,
    AuthorityFactory_createFromCRSCodesWithIntermediates_case_pivot_source_pivot_target) {
    createStructure();
    populateWithFakeEPSG();
    createSourceTargetPivotCRS();

    createTransformationForPivotTesting("PIVOT", "SOURCE");
    createTransformationForPivotTesting("PIVOT", "TARGET");

    checkSourceToOther();
}

// ---------------------------------------------------------------------------

TEST_F(
    FactoryWithTmpDatabase,
    AuthorityFactory_createFromCRSCodesWithIntermediates_case_pivot_source_target_pivot) {
    createStructure();
    populateWithFakeEPSG();
    createSourceTargetPivotCRS();

    createTransformationForPivotTesting("PIVOT", "SOURCE");
    createTransformationForPivotTesting("TARGET", "PIVOT");

    checkSourceToOther();
}

// ---------------------------------------------------------------------------

TEST_F(FactoryWithTmpDatabase, AuthorityFactory_proj_based_transformation) {
    createStructure();
    populateWithFakeEPSG();

    ASSERT_TRUE(execute(
        "INSERT INTO other_transformation "
        "VALUES('OTHER','FOO','My PROJ string based op',NULL,'PROJ',"
        "'PROJString','+proj=pipeline +ellps=WGS84 +step +proj=longlat',"
        "'EPSG','4326','EPSG','4326',0.0,NULL,NULL,NULL,"
        "NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,"
        "NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,"
        "NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,"
        "NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0);"))
        << last_error();
    ASSERT_TRUE(execute("INSERT INTO usage VALUES('OTHER',"
                        "'other_transformation_FOO_usage',"
                        "'other_transformation',"
                        "'OTHER','FOO',"
                        "'EPSG','1262','EPSG','1024');"))
        << last_error();

    auto factoryOTHER =
        AuthorityFactory::create(DatabaseContext::create(m_ctxt), "OTHER");
    auto res = factoryOTHER->createFromCoordinateReferenceSystemCodes(
        "EPSG", "4326", "EPSG", "4326", false, false, false, false);
    ASSERT_EQ(res.size(), 1U);
    EXPECT_EQ(res[0]->nameStr(), "My PROJ string based op");
    EXPECT_EQ(res[0]->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=pipeline +ellps=WGS84 +step +proj=longlat");
}

// ---------------------------------------------------------------------------

TEST_F(FactoryWithTmpDatabase, AuthorityFactory_wkt_based_transformation) {
    createStructure();
    populateWithFakeEPSG();

    auto wkt = "COORDINATEOPERATION[\"My WKT string based op\",\n"
               "    SOURCECRS[\n"
               "        GEODCRS[\"unknown\",\n"
               "            DATUM[\"World Geodetic System 1984\",\n"
               "                ELLIPSOID[\"WGS 84\",6378137,298.257223563,\n"
               "                    LENGTHUNIT[\"metre\",1]],\n"
               "                ID[\"EPSG\",6326]],\n"
               "            PRIMEM[\"Greenwich\",0,\n"
               "                ANGLEUNIT[\"degree\",0.0174532925199433],\n"
               "                ID[\"EPSG\",8901]],\n"
               "            CS[ellipsoidal,2],\n"
               "                AXIS[\"geodetic latitude (Lat)\",north],\n"
               "                AXIS[\"geodetic longitude (Lon)\",east],\n"
               "                ANGLEUNIT[\"degree\",0.0174532925199433]]],\n"
               "    TARGETCRS[\n"
               "        GEODCRS[\"unknown\",\n"
               "            DATUM[\"World Geodetic System 1984\",\n"
               "                ELLIPSOID[\"WGS 84\",6378137,298.257223563,\n"
               "                    LENGTHUNIT[\"metre\",1]],\n"
               "                ID[\"EPSG\",6326]],\n"
               "            PRIMEM[\"Greenwich\",0,\n"
               "                ANGLEUNIT[\"degree\",0.0174532925199433],\n"
               "                ID[\"EPSG\",8901]],\n"
               "            CS[ellipsoidal,2],\n"
               "                AXIS[\"geodetic latitude (Lat)\",north],\n"
               "                AXIS[\"geodetic longitude (Lon)\",east],\n"
               "                ANGLEUNIT[\"degree\",0.0174532925199433]]],\n"
               "    METHOD[\"Geocentric translations (geog2D domain)\"],\n"
               "    PARAMETER[\"X-axis translation\",1,UNIT[\"metre\",1]],\n"
               "    PARAMETER[\"Y-axis translation\",2,UNIT[\"metre\",1]],\n"
               "    PARAMETER[\"Z-axis translation\",3,UNIT[\"metre\",1]]]";

    ASSERT_TRUE(
        execute("INSERT INTO other_transformation "
                "VALUES('OTHER','FOO','My WKT string based op',NULL,"
                "'PROJ','WKT','" +
                std::string(wkt) +
                "',"
                "'EPSG','4326','EPSG','4326',0.0,NULL,NULL,NULL,"
                "NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,"
                "NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,"
                "NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,"
                "NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0);"))
        << last_error();
    ASSERT_TRUE(execute("INSERT INTO usage VALUES('OTHER',"
                        "'other_transformation_FOO_usage',"
                        "'other_transformation',"
                        "'OTHER','FOO',"
                        "'EPSG','1262','EPSG','1024');"))
        << last_error();

    auto factoryOTHER =
        AuthorityFactory::create(DatabaseContext::create(m_ctxt), "OTHER");
    auto res = factoryOTHER->createFromCoordinateReferenceSystemCodes(
        "EPSG", "4326", "EPSG", "4326", false, false, false, false);
    ASSERT_EQ(res.size(), 1U);
    EXPECT_EQ(res[0]->nameStr(), "My WKT string based op");
    EXPECT_EQ(res[0]->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=pipeline +step +proj=axisswap +order=2,1 +step "
              "+proj=unitconvert +xy_in=deg +xy_out=rad +step +proj=push +v_3 "
              "+step +proj=cart +ellps=WGS84 +step +proj=helmert +x=1 +y=2 "
              "+z=3 +step +inv +proj=cart +ellps=WGS84 +step +proj=pop +v_3 "
              "+step +proj=unitconvert +xy_in=rad +xy_out=deg +step "
              "+proj=axisswap +order=2,1");
}

// ---------------------------------------------------------------------------

TEST_F(FactoryWithTmpDatabase,
       AuthorityFactory_wkt_based_transformation_not_wkt) {
    createStructure();
    populateWithFakeEPSG();

    ASSERT_TRUE(
        execute("INSERT INTO other_transformation "
                "VALUES('OTHER','FOO','My WKT string based op',NULL,"
                "'PROJ','WKT','" +
                std::string("invalid_wkt") +
                "',"
                "'EPSG','4326','EPSG','4326',0.0,NULL,NULL,NULL,"
                "NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,"
                "NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,"
                "NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,"
                "NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0);"))
        << last_error();
    ASSERT_TRUE(execute("INSERT INTO usage VALUES('OTHER',"
                        "'other_transformation_FOO_usage',"
                        "'other_transformation',"
                        "'OTHER','FOO',"
                        "'EPSG','1262','EPSG','1024');"))
        << last_error();

    auto factoryOTHER =
        AuthorityFactory::create(DatabaseContext::create(m_ctxt), "OTHER");
    EXPECT_THROW(
        factoryOTHER->createFromCoordinateReferenceSystemCodes(
            "EPSG", "4326", "EPSG", "4326", false, false, false, false),
        FactoryException);
}

// ---------------------------------------------------------------------------

TEST_F(FactoryWithTmpDatabase,
       AuthorityFactory_wkt_based_transformation_not_co_wkt) {
    createStructure();
    populateWithFakeEPSG();

    ASSERT_TRUE(
        execute("INSERT INTO other_transformation "
                "VALUES('OTHER','FOO','My WKT string based op',NULL,"
                "'PROJ','WKT','" +
                std::string("LOCAL_CS[\"foo\"]") +
                "',"
                "'EPSG','4326','EPSG','4326',0.0,NULL,NULL,NULL,"
                "NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,"
                "NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,"
                "NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,"
                "NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0);"))
        << last_error();
    ASSERT_TRUE(execute("INSERT INTO usage VALUES('OTHER',"
                        "'other_transformation_FOO_usage',"
                        "'other_transformation',"
                        "'OTHER','FOO',"
                        "'EPSG','1262','EPSG','1024');"))
        << last_error();

    auto factoryOTHER =
        AuthorityFactory::create(DatabaseContext::create(m_ctxt), "OTHER");
    EXPECT_THROW(
        factoryOTHER->createFromCoordinateReferenceSystemCodes(
            "EPSG", "4326", "EPSG", "4326", false, false, false, false),
        FactoryException);
}

// ---------------------------------------------------------------------------

TEST(factory, AuthorityFactory_EPSG_4326_approximate_equivalent_to_builtin) {
    auto factory = AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    auto crs = nn_dynamic_pointer_cast<GeographicCRS>(
        factory->createCoordinateReferenceSystem("4326"));
    EXPECT_TRUE(crs->isEquivalentTo(GeographicCRS::EPSG_4326.get(),
                                    IComparable::Criterion::EQUIVALENT));
}

// ---------------------------------------------------------------------------

TEST_F(FactoryWithTmpDatabase, getAuthorities) {
    createStructure();
    populateWithFakeEPSG();

    auto res = DatabaseContext::create(m_ctxt)->getAuthorities();
    EXPECT_EQ(res.size(), 2U);
    EXPECT_TRUE(res.find("EPSG") != res.end());
    EXPECT_TRUE(res.find("PROJ") != res.end());
}

// ---------------------------------------------------------------------------

TEST_F(FactoryWithTmpDatabase, lookForGridInfo) {
    createStructure();

    ASSERT_TRUE(execute("INSERT INTO grid_alternatives(original_grid_name,"
                        "proj_grid_name, "
                        "old_proj_grid_name, "
                        "proj_grid_format, "
                        "proj_method, "
                        "inverse_direction, "
                        "package_name, "
                        "url, direct_download, open_license, directory) "
                        "VALUES ("
                        "'NOT-YET-IN-GRID-TRANSFORMATION-PROJ_fake_grid', "
                        "'PROJ_fake_grid', "
                        "'old_PROJ_fake_grid', "
                        "'NTv2', "
                        "'hgridshift', "
                        "0, "
                        "NULL, "
                        "'url', 1, 1, NULL);"))
        << last_error();

    std::string fullFilename;
    std::string packageName;
    std::string url;
    bool directDownload = false;
    bool openLicense = false;
    bool gridAvailable = false;
    EXPECT_TRUE(DatabaseContext::create(m_ctxt)->lookForGridInfo(
        "PROJ_fake_grid", false, fullFilename, packageName, url, directDownload,
        openLicense, gridAvailable));
    EXPECT_TRUE(fullFilename.empty());
    EXPECT_TRUE(packageName.empty());
    EXPECT_EQ(url, "url");
    EXPECT_EQ(directDownload, true);
    EXPECT_EQ(openLicense, true);
    EXPECT_EQ(gridAvailable, false);
}

// ---------------------------------------------------------------------------

TEST_F(FactoryWithTmpDatabase, custom_geodetic_crs) {
    createStructure();
    populateWithFakeEPSG();

    ASSERT_TRUE(execute("INSERT INTO geodetic_crs VALUES('TEST_NS','TEST','my "
                        "name TEST',NULL,'geographic 2D',"
                        "NULL,NULL,NULL,NULL,'+proj=longlat +a=2 "
                        "+rf=300',0);"))
        << last_error();

    ASSERT_TRUE(execute("INSERT INTO geodetic_crs VALUES"
                        "('TEST_NS','TEST_BOUND',"
                        "'my name TEST',NULL,'geographic 2D',"
                        "NULL,NULL,NULL,NULL,'+proj=longlat +a=2 "
                        "+rf=300 +towgs84=1,2,3',0);"))
        << last_error();

    ASSERT_TRUE(execute("INSERT INTO geodetic_crs VALUES('TEST_NS','TEST_GC',"
                        "'my name',NULL,'geocentric',NULL,NULL,"
                        "NULL,NULL,'+proj=geocent +a=2 +rf=300',0);"))
        << last_error();

    ASSERT_TRUE(execute(
        "INSERT INTO geodetic_crs "
        "VALUES('TEST_NS','TEST_REF_ANOTHER','my name TEST_REF_ANOTHER',"
        "NULL,"
        "'geographic 2D',NULL,NULL,NULL,NULL,'TEST_NS:TEST',0);"))
        << last_error();

    ASSERT_TRUE(execute("INSERT INTO geodetic_crs "
                        "VALUES('TEST_NS','TEST_WRONG','my name',NULL,"
                        "'geographic 2D',NULL,NULL,NULL,NULL,"
                        "'+proj=merc',0);"))
        << last_error();

    ASSERT_TRUE(execute(
        "INSERT INTO geodetic_crs "
        "VALUES('TEST_NS','TEST_RECURSIVE','my name',NULL,'geographic 2D',"
        "NULL,NULL,NULL,NULL,'TEST_NS:TEST_RECURSIVE',0);"))
        << last_error();

    auto factory =
        AuthorityFactory::create(DatabaseContext::create(m_ctxt), "TEST_NS");
    {
        auto crs = factory->createGeodeticCRS("TEST");
        EXPECT_TRUE(nn_dynamic_pointer_cast<GeographicCRS>(crs) != nullptr);
        EXPECT_EQ(*(crs->name()->description()), "my name TEST");
        EXPECT_EQ(crs->identifiers().size(), 1U);
        EXPECT_EQ(crs->ellipsoid()->semiMajorAxis(), Length(2));
        EXPECT_EQ(*(crs->ellipsoid()->inverseFlattening()), Scale(300));
        EXPECT_TRUE(crs->canonicalBoundCRS() == nullptr);
    }
    {
        auto crs = factory->createGeodeticCRS("TEST_BOUND");
        EXPECT_TRUE(nn_dynamic_pointer_cast<GeographicCRS>(crs) != nullptr);
        EXPECT_EQ(*(crs->name()->description()), "my name TEST");
        EXPECT_EQ(crs->identifiers().size(), 1U);
        EXPECT_EQ(crs->ellipsoid()->semiMajorAxis(), Length(2));
        EXPECT_EQ(*(crs->ellipsoid()->inverseFlattening()), Scale(300));
        EXPECT_TRUE(crs->canonicalBoundCRS() != nullptr);
    }
    {
        auto crs = factory->createGeodeticCRS("TEST_GC");
        EXPECT_TRUE(nn_dynamic_pointer_cast<GeographicCRS>(crs) == nullptr);
        EXPECT_EQ(*(crs->name()->description()), "my name");
        EXPECT_EQ(crs->identifiers().size(), 1U);
        EXPECT_EQ(crs->ellipsoid()->semiMajorAxis(), Length(2));
        EXPECT_EQ(*(crs->ellipsoid()->inverseFlattening()), Scale(300));
    }
    {
        auto crs = factory->createGeodeticCRS("TEST_REF_ANOTHER");
        EXPECT_TRUE(nn_dynamic_pointer_cast<GeographicCRS>(crs) != nullptr);
        EXPECT_EQ(*(crs->name()->description()), "my name TEST_REF_ANOTHER");
        EXPECT_EQ(crs->identifiers().size(), 1U);
        EXPECT_EQ(crs->ellipsoid()->semiMajorAxis(), Length(2));
        EXPECT_EQ(*(crs->ellipsoid()->inverseFlattening()), Scale(300));
    }

    EXPECT_THROW(factory->createGeodeticCRS("TEST_WRONG"), FactoryException);

    EXPECT_THROW(factory->createGeodeticCRS("TEST_RECURSIVE"),
                 FactoryException);
}

// ---------------------------------------------------------------------------

TEST_F(FactoryWithTmpDatabase, custom_projected_crs) {
    createStructure();
    populateWithFakeEPSG();

    ASSERT_TRUE(execute("INSERT INTO projected_crs "
                        "VALUES('TEST_NS','TEST','my name',NULL,NULL,"
                        "NULL,NULL,NULL,NULL,NULL,"
                        "'+proj=mbt_s +unused_flag',0);"))
        << last_error();

    ASSERT_TRUE(execute("INSERT INTO projected_crs "
                        "VALUES('TEST_NS','TEST_BOUND','my name',NULL,"
                        "NULL,NULL,NULL,NULL,NULL,NULL,"
                        "'+proj=mbt_s +unused_flag +towgs84=1,2,3',0);"))
        << last_error();

    ASSERT_TRUE(execute("INSERT INTO projected_crs "
                        "VALUES('TEST_NS','TEST_WRONG','my name',NULL,"
                        "NULL,NULL,NULL,NULL,NULL,NULL,"
                        "'+proj=longlat',0);"))
        << last_error();

    // Unknown ellipsoid
    ASSERT_TRUE(execute("INSERT INTO projected_crs "
                        "VALUES('TEST_NS','TEST_MERC','merc',NULL,NULL,"
                        "NULL,NULL,NULL,NULL,NULL,"
                        "'+proj=merc +x_0=0 +R=1',0);"))
        << last_error();

    // Well-known ellipsoid
    ASSERT_TRUE(execute("INSERT INTO projected_crs "
                        "VALUES('TEST_NS','TEST_MERC2','merc2',NULL,NULL,"
                        "NULL,NULL,NULL,NULL,NULL,"
                        "'+proj=merc +x_0=0 +ellps=GRS80',0);"))
        << last_error();

    // WKT1_GDAL
    ASSERT_TRUE(
        execute("INSERT INTO projected_crs "
                "VALUES('TEST_NS','TEST_WKT1_GDAL','WKT1_GDAL',NULL,NULL,"
                "NULL,NULL,NULL,NULL,NULL,"
                "'"
                "PROJCS[\"unknown\",\n"
                "    GEOGCS[\"unknown\",\n"
                "        DATUM[\"Unknown_based_on_WGS84_ellipsoid\",\n"
                "            SPHEROID[\"WGS 84\",6378137,298.257223563,\n"
                "                AUTHORITY[\"EPSG\",\"7030\"]]],\n"
                "        PRIMEM[\"Greenwich\",0,\n"
                "            AUTHORITY[\"EPSG\",\"8901\"]],\n"
                "        UNIT[\"degree\",0.0174532925199433,\n"
                "            AUTHORITY[\"EPSG\",\"9122\"]]],\n"
                "    PROJECTION[\"Mercator_1SP\"],\n"
                "    PARAMETER[\"central_meridian\",0],\n"
                "    PARAMETER[\"scale_factor\",1],\n"
                "    PARAMETER[\"false_easting\",0],\n"
                "    PARAMETER[\"false_northing\",0],\n"
                "    UNIT[\"metre\",1,\n"
                "        AUTHORITY[\"EPSG\",\"9001\"]],\n"
                "    AXIS[\"Easting\",EAST],\n"
                "    AXIS[\"Northing\",NORTH]]"
                "',0);"))
        << last_error();

    auto factory =
        AuthorityFactory::create(DatabaseContext::create(m_ctxt), "TEST_NS");
    {
        auto crs = factory->createProjectedCRS("TEST");
        EXPECT_EQ(*(crs->name()->description()), "my name");
        EXPECT_EQ(crs->identifiers().size(), 1U);
        EXPECT_EQ(crs->derivingConversion()->targetCRS().get(), crs.get());
        EXPECT_EQ(crs->exportToPROJString(PROJStringFormatter::create().get()),
                  "+proj=mbt_s +datum=WGS84 +units=m +no_defs +type=crs");
        EXPECT_TRUE(crs->canonicalBoundCRS() == nullptr);
    }
    {
        auto crs = factory->createProjectedCRS("TEST_BOUND");
        EXPECT_EQ(*(crs->name()->description()), "my name");
        EXPECT_EQ(crs->identifiers().size(), 1U);
        EXPECT_EQ(crs->derivingConversion()->targetCRS().get(), crs.get());
        EXPECT_EQ(crs->exportToPROJString(PROJStringFormatter::create().get()),
                  "+proj=mbt_s +datum=WGS84 +units=m +no_defs +type=crs");
        EXPECT_TRUE(crs->canonicalBoundCRS() != nullptr);
    }

    EXPECT_THROW(factory->createProjectedCRS("TEST_WRONG"), FactoryException);

    {
        auto obj = PROJStringParser().createFromPROJString(
            "+proj=merc +a=1 +b=1 +type=crs");
        auto crs = nn_dynamic_pointer_cast<ProjectedCRS>(obj);
        ASSERT_TRUE(crs != nullptr);
        auto res = crs->identify(factory);
        EXPECT_EQ(res.size(), 1U);
        if (!res.empty()) {
            EXPECT_EQ(res.front().first->nameStr(), "merc");
        }
    }

    {
        auto obj = PROJStringParser().createFromPROJString(
            "+proj=merc +ellps=GRS80 +type=crs");
        auto crs = nn_dynamic_pointer_cast<ProjectedCRS>(obj);
        ASSERT_TRUE(crs != nullptr);
        auto res = crs->identify(factory);
        EXPECT_EQ(res.size(), 1U);
        if (!res.empty()) {
            EXPECT_EQ(res.front().first->nameStr(), "merc2");
        }
    }

    {
        auto obj = PROJStringParser().createFromPROJString(
            "+proj=merc +a=6378137 +rf=298.257222101 +type=crs");
        auto crs = nn_dynamic_pointer_cast<ProjectedCRS>(obj);
        ASSERT_TRUE(crs != nullptr);
        auto res = crs->identify(factory);
        EXPECT_EQ(res.size(), 1U);
        if (!res.empty()) {
            EXPECT_EQ(res.front().first->nameStr(), "merc2");
        }
    }

    {
        auto obj = PROJStringParser().createFromPROJString(
            "+proj=merc +ellps=WGS84 +type=crs");
        auto crs = nn_dynamic_pointer_cast<ProjectedCRS>(obj);
        ASSERT_TRUE(crs != nullptr);
        auto res = crs->identify(factory);
        EXPECT_EQ(res.size(), 1U);
        if (!res.empty()) {
            EXPECT_EQ(res.front().first->nameStr(), "WKT1_GDAL");
        }
    }
}

// ---------------------------------------------------------------------------

TEST(factory, attachExtraDatabases_none) {
    auto ctxt = DatabaseContext::create(std::string(), {});
    auto factory = AuthorityFactory::create(ctxt, "EPSG");
    auto crs = factory->createGeodeticCRS("4979");
    auto gcrs = nn_dynamic_pointer_cast<GeographicCRS>(crs);
}

// ---------------------------------------------------------------------------

#ifndef SQLITE_OPEN_URI
static int MyUnlink(const std::string &filename) {
#ifdef _MSC_VER
    return _unlink(filename.c_str());
#else
    return unlink(filename.c_str());
#endif
}
#endif

// ---------------------------------------------------------------------------

TEST(factory, attachExtraDatabases_auxiliary) {

#ifdef SQLITE_OPEN_URI
    std::string auxDbName("file:proj_test_aux.db?mode=memory&cache=shared");
#else
    const char *temp = getenv("TEMP");
    if (!temp) {
        temp = getenv("TMP");
    }
    if (!temp) {
        temp = "/tmp";
    }
    std::string auxDbName(std::string(temp) + "/proj_test_aux.db");
    MyUnlink(auxDbName);
#endif
    {
        sqlite3 *dbAux = nullptr;
        sqlite3_open_v2(auxDbName.c_str(), &dbAux,
                        SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE
#ifdef SQLITE_OPEN_URI
                            | SQLITE_OPEN_URI
#endif
                        ,
                        nullptr);
        ASSERT_TRUE(dbAux != nullptr);
        ASSERT_TRUE(sqlite3_exec(dbAux, "BEGIN", nullptr, nullptr, nullptr) ==
                    SQLITE_OK);
        {
            auto ctxt = DatabaseContext::create();
            const auto dbStructure = ctxt->getDatabaseStructure();
            for (const auto &sql : dbStructure) {
                if (sql.find("CREATE TRIGGER") == std::string::npos) {
                    ASSERT_TRUE(sqlite3_exec(dbAux, sql.c_str(), nullptr,
                                             nullptr, nullptr) == SQLITE_OK);
                }
            }
        }

        ASSERT_TRUE(
            sqlite3_exec(
                dbAux,
                "INSERT INTO geodetic_crs VALUES('OTHER','OTHER_4326','WGS "
                "84',NULL,'geographic 2D','EPSG','6422','EPSG','6326',"
                "NULL,0);",
                nullptr, nullptr, nullptr) == SQLITE_OK);
        ASSERT_TRUE(sqlite3_exec(dbAux, "COMMIT", nullptr, nullptr, nullptr) ==
                    SQLITE_OK);

        {
            auto ctxt = DatabaseContext::create(std::string(), {auxDbName});
            // Look for object located in main DB
            {
                auto factory = AuthorityFactory::create(ctxt, "EPSG");
                auto crs = factory->createGeodeticCRS("4326");
                auto gcrs = nn_dynamic_pointer_cast<GeographicCRS>(crs);
            }
            // Look for object located in auxiliary DB
            {
                auto factory = AuthorityFactory::create(ctxt, "OTHER");
                auto crs = factory->createGeodeticCRS("OTHER_4326");
                auto gcrs = nn_dynamic_pointer_cast<GeographicCRS>(crs);
            }
        }

        {
            auto ctxt =
                DatabaseContext::create(std::string(), {auxDbName, ":memory:"});
            // Look for object located in main DB
            {
                auto factory = AuthorityFactory::create(ctxt, "EPSG");
                auto crs = factory->createGeodeticCRS("4326");
                auto gcrs = nn_dynamic_pointer_cast<GeographicCRS>(crs);
            }
            // Look for object located in auxiliary DB
            {
                auto factory = AuthorityFactory::create(ctxt, "OTHER");
                auto crs = factory->createGeodeticCRS("OTHER_4326");
                auto gcrs = nn_dynamic_pointer_cast<GeographicCRS>(crs);
            }
        }

        {
            auto ctxt = DatabaseContext::create(std::string(), {":memory:"});
            // Look for object located in main DB
            {
                auto factory = AuthorityFactory::create(ctxt, "EPSG");
                auto crs = factory->createGeodeticCRS("4326");
                auto gcrs = nn_dynamic_pointer_cast<GeographicCRS>(crs);
            }
            // Look for object located in auxiliary DB
            {
                auto factory = AuthorityFactory::create(ctxt, "OTHER");
                EXPECT_THROW(factory->createGeodeticCRS("OTHER_4326"),
                             FactoryException);
            }
        }

        sqlite3_close(dbAux);
    }
#ifndef SQLITE_OPEN_URI
    MyUnlink(auxDbName);
#endif
}

// ---------------------------------------------------------------------------

TEST(factory, attachExtraDatabases_auxiliary_error) {
    EXPECT_THROW(DatabaseContext::create(std::string(), {"i_dont_exist_db"}),
                 FactoryException);
}

// ---------------------------------------------------------------------------

TEST(factory, getOfficialNameFromAlias) {
    auto ctxt = DatabaseContext::create(std::string(), {});
    auto factory = AuthorityFactory::create(ctxt, std::string());
    std::string outTableName;
    std::string outAuthName;
    std::string outCode;

    {
        auto officialName = factory->getOfficialNameFromAlias(
            "GCS_WGS_1984", std::string(), std::string(), false, outTableName,
            outAuthName, outCode);
        EXPECT_EQ(officialName, "WGS 84");
        EXPECT_EQ(outTableName, "geodetic_crs");
        EXPECT_EQ(outAuthName, "EPSG");
        EXPECT_EQ(outCode, "4326");
    }

    {
        auto officialName = factory->getOfficialNameFromAlias(
            "GCS_WGS_1984", "geodetic_crs", "ESRI", false, outTableName,
            outAuthName, outCode);
        EXPECT_EQ(officialName, "WGS 84");
        EXPECT_EQ(outTableName, "geodetic_crs");
        EXPECT_EQ(outAuthName, "EPSG");
        EXPECT_EQ(outCode, "4326");
    }

    {
        auto officialName = factory->getOfficialNameFromAlias(
            "no match", std::string(), std::string(), false, outTableName,
            outAuthName, outCode);
        EXPECT_EQ(officialName, "");
    }

    {
        auto officialName = factory->getOfficialNameFromAlias(
            "System_Jednotne_Trigonometricke_Site_Katastralni_Ferro",
            "geodetic_datum", std::string(), true, outTableName, outAuthName,
            outCode);
        EXPECT_EQ(
            officialName,
            "System of the Unified Trigonometrical Cadastral Network (Ferro)");
    }
}

// ---------------------------------------------------------------------------

TEST_F(FactoryWithTmpDatabase,
       createOperations_exact_transform_not_whole_area) {
    createStructure();
    populateWithFakeEPSG();

    ASSERT_TRUE(
        execute("INSERT INTO other_transformation "
                "VALUES('OTHER','PARTIAL_AREA_PERFECT_ACCURACY',"
                "'PARTIAL_AREA_PERFECT_ACCURACY',NULL,'PROJ',"
                "'PROJString','+proj=helmert +x=1',"
                "'EPSG','4326','EPSG','4326',0.0,NULL,NULL,NULL,"
                "NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,"
                "NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,"
                "NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,"
                "NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0);"))
        << last_error();
    ASSERT_TRUE(execute("INSERT INTO usage VALUES('OTHER', "
                        "'1','other_transformation','OTHER','PARTIAL_AREA_"
                        "PERFECT_ACCURACY','EPSG','1933','EPSG','1024')"))
        << last_error();

    ASSERT_TRUE(
        execute("INSERT INTO other_transformation "
                "VALUES('OTHER','WHOLE_AREA_APPROX_ACCURACY',"
                "'WHOLE_AREA_APPROX_ACCURACY',NULL,'PROJ',"
                "'PROJString','+proj=helmert +x=2',"
                "'EPSG','4326','EPSG','4326',1.0,NULL,NULL,NULL,"
                "NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,"
                "NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,"
                "NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,"
                "NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0);"))
        << last_error();
    ASSERT_TRUE(execute("INSERT INTO usage VALUES('OTHER', "
                        "'2','other_transformation','OTHER','WHOLE_AREA_APPROX_"
                        "ACCURACY','EPSG','1262','EPSG','1024')"))
        << last_error();

    auto dbContext = DatabaseContext::create(m_ctxt);
    auto authFactory =
        AuthorityFactory::create(dbContext, std::string("OTHER"));
    auto ctxt = CoordinateOperationContext::create(authFactory, nullptr, 0.0);
    ctxt->setSpatialCriterion(
        CoordinateOperationContext::SpatialCriterion::PARTIAL_INTERSECTION);
    auto list = CoordinateOperationFactory::create()->createOperations(
        AuthorityFactory::create(dbContext, "EPSG")
            ->createCoordinateReferenceSystem("4326"),
        AuthorityFactory::create(dbContext, "EPSG")
            ->createCoordinateReferenceSystem("4326"),
        ctxt);
    ASSERT_EQ(list.size(), 2U);
    EXPECT_EQ(list[0]->nameStr(), "WHOLE_AREA_APPROX_ACCURACY");
    EXPECT_EQ(list[1]->nameStr(), "PARTIAL_AREA_PERFECT_ACCURACY");
}

// ---------------------------------------------------------------------------

TEST(factory, createObjectsFromName) {
    auto ctxt = DatabaseContext::create();
    auto factory = AuthorityFactory::create(ctxt, std::string());
    auto factoryEPSG = AuthorityFactory::create(ctxt, "EPSG");

    EXPECT_EQ(factory->createObjectsFromName("").size(), 0U);

    // ellipsoid + datum + 3 geodeticCRS
    EXPECT_EQ(factory->createObjectsFromName("WGS 84", {}, false).size(), 5U);

    EXPECT_EQ(factory->createObjectsFromName("WGS 84", {}, true, 10).size(),
              10U);

    EXPECT_EQ(factory
                  ->createObjectsFromName(
                      "WGS 84", {AuthorityFactory::ObjectType::CRS}, false)
                  .size(),
              3U);

    EXPECT_EQ(
        factory
            ->createObjectsFromName(
                "WGS 84", {AuthorityFactory::ObjectType::GEOCENTRIC_CRS}, false)
            .size(),
        1U);

    {
        auto res = factoryEPSG->createObjectsFromName(
            "WGS84", {AuthorityFactory::ObjectType::GEOGRAPHIC_2D_CRS}, true);
        // EPSG:4326 and the 6 WGS84 realizations
        // and EPSG:7881 'Tritan St. Helena'' whose alias is
        // 'WGS 84 Tritan St. Helena'
        EXPECT_EQ(res.size(), 8U);
        if (!res.empty()) {
            EXPECT_EQ(res.front()->getEPSGCode(), 4326);
        }
    }

    // Exact name, but just not the official case ==> should match with exact
    // match
    EXPECT_EQ(factory->createObjectsFromName("WGS 84 / utm zone 31n", {}, false)
                  .size(),
              1U);

    // Exact name, but with other CRS that have an aliases to it ==> should
    // match only the CRS with the given name, not those other CRS.
    EXPECT_EQ(factory->createObjectsFromName("ETRS89 / UTM zone 32N", {}, false)
                  .size(),
              1U);

    // Prime meridian
    EXPECT_EQ(factoryEPSG->createObjectsFromName("Paris", {}, false, 2).size(),
              1U);
    // Ellipsoid
    EXPECT_EQ(
        factoryEPSG->createObjectsFromName("Clarke 1880 (IGN)", {}, false, 2)
            .size(),
        1U);
    // Geodetic datum
    EXPECT_EQ(
        factoryEPSG->createObjectsFromName("Hungarian Datum 1909", {}, false, 2)
            .size(),
        1U);
    // Vertical datum
    EXPECT_EQ(factoryEPSG->createObjectsFromName("EGM2008 geoid", {}, false, 2)
                  .size(),
              1U);
    // Geodetic CRS
    EXPECT_EQ(factoryEPSG
                  ->createObjectsFromName(
                      "Unknown datum based upon the Airy 1830 ellipsoid", {},
                      false, 2)
                  .size(),
              1U);
    // Projected CRS
    EXPECT_EQ(factoryEPSG
                  ->createObjectsFromName(
                      "Anguilla 1957 / British West Indies Grid", {}, false, 2)
                  .size(),
              1U);
    // Vertical CRS
    EXPECT_EQ(factoryEPSG->createObjectsFromName("EGM2008 height", {}, false, 2)
                  .size(),
              1U);
    // Compound CRS
    EXPECT_EQ(factoryEPSG
                  ->createObjectsFromName(
                      "KKJ / Finland Uniform Coordinate System + N60 height",
                      {}, false, 2)
                  .size(),
              1U);
    // Conversion
    EXPECT_EQ(
        factoryEPSG->createObjectsFromName("Belgian Lambert 2008", {}, false, 2)
            .size(),
        1U);
    // Helmert transform
    EXPECT_EQ(
        factoryEPSG->createObjectsFromName("MGI to ETRS89 (4)", {}, false, 2)
            .size(),
        1U);
    // Grid transform
    EXPECT_EQ(factoryEPSG
                  ->createObjectsFromName("Guam 1963 to NAD83(HARN) (1)", {},
                                          false, 2)
                  .size(),
              1U);
    // Other transform
    EXPECT_EQ(factoryEPSG
                  ->createObjectsFromName(
                      "Monte Mario (Rome) to Monte Mario (1)", {}, false, 2)
                  .size(),
              1U);
    // Concatenated operation
    EXPECT_EQ(
        factoryEPSG
            ->createObjectsFromName("MGI (Ferro) to WGS 84 (2)", {}, false, 2)
            .size(),
        1U);

    // Deprecated object
    EXPECT_EQ(factoryEPSG
                  ->createObjectsFromName(
                      "NAD27(CGQ77) / SCoPQ zone 2 (deprecated)", {}, false, 2)
                  .size(),
              1U);

    // Deprecated object (but without explicit deprecated)
    EXPECT_EQ(
        factoryEPSG
            ->createObjectsFromName("NAD27(CGQ77) / SCoPQ zone 2", {}, false, 2)
            .size(),
        1U);

    // Dynamic Geodetic datum
    EXPECT_EQ(factoryEPSG
                  ->createObjectsFromName(
                      "International Terrestrial Reference Frame 2008",
                      {AuthorityFactory::ObjectType::
                           DYNAMIC_GEODETIC_REFERENCE_FRAME},
                      false, 2)
                  .size(),
              1U);

    // Dynamic Vertical datum
    EXPECT_EQ(
        factoryEPSG
            ->createObjectsFromName("Norway Normal Null 2000",
                                    {AuthorityFactory::ObjectType::
                                         DYNAMIC_VERTICAL_REFERENCE_FRAME},
                                    false, 2)
            .size(),
        1U);

    {
        auto res = factory->createObjectsFromName(
            "World Geodetic System 1984 ensemble",
            {AuthorityFactory::ObjectType::DATUM_ENSEMBLE}, false);
        EXPECT_EQ(res.size(), 1U);
        if (!res.empty()) {
            EXPECT_EQ(res.front()->getEPSGCode(), 6326);
            EXPECT_TRUE(dynamic_cast<DatumEnsemble *>(res.front().get()) !=
                        nullptr);
        }
    }

    {
        auto res = factory->createObjectsFromName(
            "World Geodetic System 1984 ensemble", {}, false);
        EXPECT_EQ(res.size(), 1U);
        if (!res.empty()) {
            EXPECT_EQ(res.front()->getEPSGCode(), 6326);
            EXPECT_TRUE(dynamic_cast<DatumEnsemble *>(res.front().get()) !=
                        nullptr);
        }
    }

    const auto types = std::vector<AuthorityFactory::ObjectType>{
        AuthorityFactory::ObjectType::PRIME_MERIDIAN,
        AuthorityFactory::ObjectType::ELLIPSOID,
        AuthorityFactory::ObjectType::DATUM,
        AuthorityFactory::ObjectType::GEODETIC_REFERENCE_FRAME,
        AuthorityFactory::ObjectType::DYNAMIC_GEODETIC_REFERENCE_FRAME,
        AuthorityFactory::ObjectType::VERTICAL_REFERENCE_FRAME,
        AuthorityFactory::ObjectType::DYNAMIC_VERTICAL_REFERENCE_FRAME,
        AuthorityFactory::ObjectType::CRS,
        AuthorityFactory::ObjectType::GEODETIC_CRS,
        AuthorityFactory::ObjectType::GEOCENTRIC_CRS,
        AuthorityFactory::ObjectType::GEOGRAPHIC_CRS,
        AuthorityFactory::ObjectType::GEOGRAPHIC_2D_CRS,
        AuthorityFactory::ObjectType::GEOGRAPHIC_3D_CRS,
        AuthorityFactory::ObjectType::PROJECTED_CRS,
        AuthorityFactory::ObjectType::VERTICAL_CRS,
        AuthorityFactory::ObjectType::COMPOUND_CRS,
        AuthorityFactory::ObjectType::COORDINATE_OPERATION,
        AuthorityFactory::ObjectType::CONVERSION,
        AuthorityFactory::ObjectType::TRANSFORMATION,
        AuthorityFactory::ObjectType::CONCATENATED_OPERATION,
        AuthorityFactory::ObjectType::DATUM_ENSEMBLE,
    };
    for (const auto type : types) {
        factory->createObjectsFromName("i_dont_exist", {type}, false, 1);
    }
    factory->createObjectsFromName("i_dont_exist", types, false, 1);

    {
        auto res = factoryEPSG->createObjectsFromName(
            "ETRS89", {AuthorityFactory::ObjectType::GEOGRAPHIC_2D_CRS}, false,
            1);
        EXPECT_EQ(res.size(), 1U);
        if (!res.empty()) {
            EXPECT_EQ(res.front()->getEPSGCode(), 4258);
        }
    }
}

// ---------------------------------------------------------------------------

TEST(factory, getMetadata) {
    auto ctxt = DatabaseContext::create();
    EXPECT_EQ(ctxt->getMetadata("i_do_not_exist"), nullptr);
    const char *IGNF_VERSION = ctxt->getMetadata("IGNF.VERSION");
    ASSERT_TRUE(IGNF_VERSION != nullptr);
    EXPECT_EQ(std::string(IGNF_VERSION), "3.1.0");
}

// ---------------------------------------------------------------------------

TEST(factory, listAreaOfUseFromName) {
    auto ctxt = DatabaseContext::create();
    auto factory = AuthorityFactory::create(ctxt, std::string());
    auto factoryEPSG = AuthorityFactory::create(ctxt, "EPSG");
    {
        auto res = factory->listAreaOfUseFromName("Denmark - onshore", false);
        ASSERT_EQ(res.size(), 1U);
        EXPECT_EQ(res.front().first, "EPSG");
        EXPECT_EQ(res.front().second, "3237");
    }
    {
        auto res = factory->listAreaOfUseFromName("Denmark", true);
        EXPECT_GT(res.size(), 1U);
    }
    {
        auto res = factory->listAreaOfUseFromName("no where land", false);
        ASSERT_EQ(res.size(), 0U);
    }
}

// ---------------------------------------------------------------------------

TEST(factory, getCRSInfoList) {
    auto ctxt = DatabaseContext::create();
    {
        auto factory = AuthorityFactory::create(ctxt, std::string());
        auto list = factory->getCRSInfoList();
        EXPECT_GT(list.size(), 1U);
        bool foundEPSG = false;
        bool foundIGNF = false;
        bool found4326 = false;
        for (const auto &info : list) {
            foundEPSG |= info.authName == "EPSG";
            foundIGNF |= info.authName == "IGNF";
            if (info.authName == "EPSG" && info.code == "4326") {
                found4326 = true;
            }
        }
        EXPECT_TRUE(foundEPSG);
        EXPECT_TRUE(foundIGNF);
        EXPECT_TRUE(found4326);
    }
    {
        auto factory = AuthorityFactory::create(ctxt, "EPSG");
        auto list = factory->getCRSInfoList();
        EXPECT_GT(list.size(), 1U);
        bool found4326 = false;
        bool found4978 = false;
        bool found4979 = false;
        bool found32631 = false;
        bool found3855 = false;
        bool found6871 = false;
        for (const auto &info : list) {
            EXPECT_EQ(info.authName, "EPSG");
            if (info.code == "4326") {
                EXPECT_EQ(info.name, "WGS 84");
                EXPECT_EQ(info.type,
                          AuthorityFactory::ObjectType::GEOGRAPHIC_2D_CRS);
                EXPECT_EQ(info.deprecated, false);
                EXPECT_EQ(info.bbox_valid, true);
                EXPECT_EQ(info.west_lon_degree, -180.0);
                EXPECT_EQ(info.south_lat_degree, -90.0);
                EXPECT_EQ(info.east_lon_degree, 180.0);
                EXPECT_EQ(info.north_lat_degree, 90.0);
                EXPECT_EQ(info.areaName, "World.");
                EXPECT_TRUE(info.projectionMethodName.empty());
                found4326 = true;
            } else if (info.code == "4296") { // Soudan - deprecated
                EXPECT_EQ(info.bbox_valid, false);
                EXPECT_EQ(info.west_lon_degree, 0.0);
                EXPECT_EQ(info.south_lat_degree, 0.0);
                EXPECT_EQ(info.east_lon_degree, 0.0);
                EXPECT_EQ(info.north_lat_degree, 0.0);
            } else if (info.code == "4978") {
                EXPECT_EQ(info.name, "WGS 84");
                EXPECT_EQ(info.type,
                          AuthorityFactory::ObjectType::GEOCENTRIC_CRS);
                found4978 = true;
            } else if (info.code == "4979") {
                EXPECT_EQ(info.name, "WGS 84");
                EXPECT_EQ(info.type,
                          AuthorityFactory::ObjectType::GEOGRAPHIC_3D_CRS);
                found4979 = true;
            } else if (info.code == "32631") {
                EXPECT_EQ(info.name, "WGS 84 / UTM zone 31N");
                EXPECT_EQ(info.type,
                          AuthorityFactory::ObjectType::PROJECTED_CRS);
                EXPECT_EQ(info.deprecated, false);
                EXPECT_EQ(info.bbox_valid, true);
                EXPECT_EQ(info.west_lon_degree, 0.0);
                EXPECT_EQ(info.south_lat_degree, 0.0);
                EXPECT_EQ(info.east_lon_degree, 6.0);
                EXPECT_EQ(info.north_lat_degree, 84.0);
                EXPECT_TRUE(info.areaName.find("Between 0\xC2\xB0"
                                               "E and 6\xC2\xB0"
                                               "E, northern hemisphere") == 0)
                    << info.areaName;
                EXPECT_EQ(info.projectionMethodName, "Transverse Mercator");
                found32631 = true;
            } else if (info.code == "3855") {
                EXPECT_EQ(info.name, "EGM2008 height");
                EXPECT_EQ(info.type,
                          AuthorityFactory::ObjectType::VERTICAL_CRS);
                found3855 = true;
            } else if (info.code == "6871") {
                EXPECT_EQ(info.name,
                          "WGS 84 / Pseudo-Mercator +  EGM2008 geoid height");
                EXPECT_EQ(info.type,
                          AuthorityFactory::ObjectType::COMPOUND_CRS);
                found6871 = true;
            }
        }
        EXPECT_TRUE(found4326);
        EXPECT_TRUE(found4978);
        EXPECT_TRUE(found4979);
        EXPECT_TRUE(found32631);
        EXPECT_TRUE(found3855);
        EXPECT_TRUE(found6871);
    }
}

// ---------------------------------------------------------------------------

TEST(factory, getUnitList) {
    auto ctxt = DatabaseContext::create();
    {
        auto factory = AuthorityFactory::create(ctxt, std::string());
        auto list = factory->getUnitList();
        EXPECT_GT(list.size(), 1U);
        bool foundEPSG = false;
        bool foundPROJ = false;
        bool found1027 = false;
        bool found1028 = false;
        bool found1032 = false;
        bool found1036 = false;
        bool found9001 = false;
        bool found9101 = false;
        for (const auto &info : list) {
            foundEPSG |= info.authName == "EPSG";
            foundPROJ |= info.authName == "PROJ";
            if (info.authName == "EPSG" && info.code == "1027") {
                EXPECT_EQ(info.name, "millimetres per year");
                EXPECT_EQ(info.category, "linear_per_time");
                found1027 = true;
            } else if (info.authName == "EPSG" && info.code == "1028") {
                EXPECT_EQ(info.name, "parts per billion");
                EXPECT_EQ(info.category, "scale");
                found1028 = true;
            } else if (info.authName == "EPSG" && info.code == "1032") {
                EXPECT_EQ(info.name, "milliarc-seconds per year");
                EXPECT_EQ(info.category, "angular_per_time");
                found1032 = true;
            } else if (info.authName == "EPSG" && info.code == "1036") {
                EXPECT_EQ(info.name, "unity per second");
                EXPECT_EQ(info.category, "scale_per_time");
                found1036 = true;
            } else if (info.authName == "EPSG" && info.code == "9001") {
                EXPECT_EQ(info.name, "metre");
                EXPECT_EQ(info.category, "linear");
                EXPECT_EQ(info.convFactor, 1.0);
                EXPECT_EQ(info.projShortName, "m");
                EXPECT_FALSE(info.deprecated);
                found9001 = true;
            } else if (info.authName == "EPSG" && info.code == "9101") {
                EXPECT_EQ(info.name, "radian");
                EXPECT_EQ(info.category, "angular");
                EXPECT_FALSE(info.deprecated);
                found9101 = true;
            }
        }
        EXPECT_TRUE(foundEPSG);
        EXPECT_TRUE(foundPROJ);
        EXPECT_TRUE(found1027);
        EXPECT_TRUE(found1028);
        EXPECT_TRUE(found1032);
        EXPECT_TRUE(found1036);
        EXPECT_TRUE(found9001);
        EXPECT_TRUE(found9101);
    }
    {
        auto factory = AuthorityFactory::create(ctxt, "EPSG");
        auto list = factory->getUnitList();
        EXPECT_GT(list.size(), 1U);
        for (const auto &info : list) {
            EXPECT_EQ(info.authName, "EPSG");
        }
    }
}

} // namespace
