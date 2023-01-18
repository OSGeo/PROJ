/******************************************************************************
 *
 * Project:  PROJ
 * Purpose:  Test ISO19111:2019 implementation
 * Author:   Even Rouault <even dot rouault at spatialys dot com>
 *
 ******************************************************************************
 * Copyright (c) 2023, Even Rouault <even dot rouault at spatialys dot com>
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
#ifndef FROM_PROJ_CPP
#define FROM_PROJ_CPP
#endif

#include "proj/common.hpp"
#include "proj/coordinates.hpp"
#include "proj/coordinatesystem.hpp"
#include "proj/crs.hpp"
#include "proj/datum.hpp"
#include "proj/io.hpp"
#include "proj/metadata.hpp"
#include "proj/util.hpp"

#include <cmath>
#include <string>
#include <vector>

using namespace osgeo::proj::common;
using namespace osgeo::proj::coordinates;
using namespace osgeo::proj::crs;
using namespace osgeo::proj::cs;
using namespace osgeo::proj::datum;
using namespace osgeo::proj::io;
using namespace osgeo::proj::metadata;
using namespace osgeo::proj::util;

namespace {
struct ObjectKeeper {
    PJ *m_obj = nullptr;
    explicit ObjectKeeper(PJ *obj) : m_obj(obj) {}
    ~ObjectKeeper() { proj_destroy(m_obj); }
    void clear() {
        proj_destroy(m_obj);
        m_obj = nullptr;
    }

    ObjectKeeper(const ObjectKeeper &) = delete;
    ObjectKeeper &operator=(const ObjectKeeper &) = delete;
};

struct PjContextKeeper {
    PJ_CONTEXT *m_ctxt = nullptr;
    explicit PjContextKeeper(PJ_CONTEXT *ctxt) : m_ctxt(ctxt) {}
    ~PjContextKeeper() { proj_context_destroy(m_ctxt); }

    PjContextKeeper(const PjContextKeeper &) = delete;
    PjContextKeeper &operator=(const PjContextKeeper &) = delete;
};
} // namespace

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

TEST(coordinateMetadata, static_crs) {
    auto coordinateMetadata =
        CoordinateMetadata::create(GeographicCRS::EPSG_4326);
    EXPECT_TRUE(coordinateMetadata->crs()->isEquivalentTo(
        GeographicCRS::EPSG_4326.get()));
    EXPECT_FALSE(coordinateMetadata->coordinateEpoch().has_value());

    // We tolerate coordinate epochs for EPSG:4326
    EXPECT_NO_THROW(
        CoordinateMetadata::create(GeographicCRS::EPSG_4326, 2025.0));

    // A coordinate epoch should NOT be provided
    EXPECT_THROW(CoordinateMetadata::create(createVerticalCRS(), 2025.0),
                 Exception);

    WKTFormatterNNPtr f(
        WKTFormatter::create(WKTFormatter::Convention::WKT2_2019));
    auto wkt = coordinateMetadata->exportToWKT(f.get());
    auto obj = WKTParser().createFromWKT(wkt);
    auto coordinateMetadataFromWkt =
        nn_dynamic_pointer_cast<CoordinateMetadata>(obj);
    ASSERT_TRUE(coordinateMetadataFromWkt != nullptr);
    EXPECT_TRUE(coordinateMetadataFromWkt->crs()->isEquivalentTo(
        GeographicCRS::EPSG_4326.get()));
    EXPECT_FALSE(coordinateMetadataFromWkt->coordinateEpoch().has_value());

    auto ctxt = proj_context_create();
    PjContextKeeper ctxtKeeper(ctxt);
    auto pjObj = proj_create(ctxt, wkt.c_str());
    ObjectKeeper objKeeper(pjObj);
    ASSERT_TRUE(pjObj != nullptr);
    EXPECT_EQ(proj_get_type(pjObj), PJ_TYPE_COORDINATE_METADATA);
    EXPECT_TRUE(std::isnan(proj_coordinate_metadata_get_epoch(ctxt, pjObj)));
    auto pjObj2 = proj_get_source_crs(ctxt, pjObj);
    ObjectKeeper objKeeper2(pjObj2);
    EXPECT_TRUE(pjObj2 != nullptr);

    auto projjson =
        coordinateMetadata->exportToJSON(JSONFormatter::create(nullptr).get());
    auto obj2 = createFromUserInput(projjson, nullptr);
    auto coordinateMetadataFromJson =
        nn_dynamic_pointer_cast<CoordinateMetadata>(obj2);
    ASSERT_TRUE(coordinateMetadataFromJson != nullptr);
    EXPECT_TRUE(coordinateMetadataFromJson->crs()->isEquivalentTo(
        GeographicCRS::EPSG_4326.get()));
    EXPECT_FALSE(coordinateMetadataFromJson->coordinateEpoch().has_value());
}

// ---------------------------------------------------------------------------

TEST(coordinateMetadata, dynamic_crs) {
    auto drf = DynamicGeodeticReferenceFrame::create(
        PropertyMap().set(IdentifiedObject::NAME_KEY, "test"), Ellipsoid::WGS84,
        optional<std::string>("My anchor"), PrimeMeridian::GREENWICH,
        Measure(2018.5, UnitOfMeasure::YEAR),
        optional<std::string>("My model"));
    auto crs = GeographicCRS::create(
        PropertyMap(), drf,
        EllipsoidalCS::createLatitudeLongitude(UnitOfMeasure::DEGREE));
    auto coordinateMetadata = CoordinateMetadata::create(crs, 2023.5);
    EXPECT_TRUE(coordinateMetadata->crs()->isEquivalentTo(crs.get()));
    EXPECT_TRUE(coordinateMetadata->coordinateEpoch().has_value());
    EXPECT_NEAR(coordinateMetadata->coordinateEpochAsDecimalYear(), 2023.5,
                1e-10);

    // A coordinate epoch should be provided
    EXPECT_THROW(CoordinateMetadata::create(crs), Exception);

    WKTFormatterNNPtr f(
        WKTFormatter::create(WKTFormatter::Convention::WKT2_2019));
    auto wkt = coordinateMetadata->exportToWKT(f.get());
    auto obj = WKTParser().createFromWKT(wkt);
    auto coordinateMetadataFromWkt =
        nn_dynamic_pointer_cast<CoordinateMetadata>(obj);
    EXPECT_TRUE(coordinateMetadataFromWkt->crs()->isEquivalentTo(crs.get()));
    EXPECT_TRUE(coordinateMetadataFromWkt->coordinateEpoch().has_value());
    EXPECT_NEAR(coordinateMetadataFromWkt->coordinateEpochAsDecimalYear(),
                2023.5, 1e-10);

    auto ctxt = proj_context_create();
    PjContextKeeper ctxtKeeper(ctxt);
    auto pjObj = proj_create(ctxt, wkt.c_str());
    ObjectKeeper objKeeper(pjObj);
    ASSERT_TRUE(pjObj != nullptr);
    EXPECT_EQ(proj_get_type(pjObj), PJ_TYPE_COORDINATE_METADATA);
    EXPECT_NEAR(proj_coordinate_metadata_get_epoch(ctxt, pjObj), 2023.5, 1e-10);

    auto projjson =
        coordinateMetadata->exportToJSON(JSONFormatter::create(nullptr).get());
    auto obj2 = createFromUserInput(projjson, nullptr);
    auto coordinateMetadataFromJson =
        nn_dynamic_pointer_cast<CoordinateMetadata>(obj2);
    EXPECT_TRUE(coordinateMetadataFromJson->crs()->isEquivalentTo(
        crs.get(), IComparable::Criterion::EQUIVALENT));
    EXPECT_TRUE(coordinateMetadataFromJson->coordinateEpoch().has_value());
    EXPECT_NEAR(coordinateMetadataFromJson->coordinateEpochAsDecimalYear(),
                2023.5, 1e-10);
}
