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

// to be able to use internal::replaceAll
#ifndef FROM_PROJ_CPP
#define FROM_PROJ_CPP
#endif

#include "proj/common.hpp"
#include "proj/coordinateoperation.hpp"
#include "proj/coordinatesystem.hpp"
#include "proj/crs.hpp"
#include "proj/datum.hpp"
#include "proj/io.hpp"
#include "proj/metadata.hpp"
#include "proj/util.hpp"

#include "proj/internal/internal.hpp"

#include "proj_constants.h"

#include <string>
#include <vector>

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

TEST(operation, geogCRS_to_geogCRS) {

    auto op = CoordinateOperationFactory::create()->createOperation(
        GeographicCRS::EPSG_4807, GeographicCRS::EPSG_4326);
    ASSERT_TRUE(op != nullptr);
    EXPECT_EQ(
        op->exportToPROJString(PROJStringFormatter::create().get()),
        "+proj=pipeline +step +proj=axisswap +order=2,1 +step "
        "+proj=unitconvert +xy_in=grad +xy_out=rad +step +inv +proj=longlat "
        "+ellps=clrk80ign +pm=paris +step +proj=unitconvert +xy_in=rad "
        "+xy_out=deg +step +proj=axisswap +order=2,1");
}

// ---------------------------------------------------------------------------

TEST(operation, geogCRS_to_geogCRS_context_default) {
    auto authFactory =
        AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    auto ctxt = CoordinateOperationContext::create(authFactory, nullptr, 0);
    ctxt->setSpatialCriterion(
        CoordinateOperationContext::SpatialCriterion::PARTIAL_INTERSECTION);
    ctxt->setAllowUseIntermediateCRS(
        CoordinateOperationContext::IntermediateCRSUse::NEVER);

    // Directly found in database
    {
        auto list = CoordinateOperationFactory::create()->createOperations(
            authFactory->createCoordinateReferenceSystem("4179"), // Pulkovo 42
            authFactory->createCoordinateReferenceSystem("4258"), // ETRS89
            ctxt);
        ASSERT_EQ(list.size(), 3U);
        // Romania has a larger area than Poland (given our approx formula)
        EXPECT_EQ(list[0]->getEPSGCode(), 15994); // Romania - 3m
        EXPECT_EQ(list[1]->getEPSGCode(), 1644);  // Poland - 1m
        EXPECT_EQ(list[2]->nameStr(),
                  "Ballpark geographic offset from Pulkovo 1942(58) to ETRS89");

        EXPECT_EQ(
            list[0]->exportToPROJString(PROJStringFormatter::create().get()),
            "+proj=pipeline +step +proj=axisswap +order=2,1 +step "
            "+proj=unitconvert +xy_in=deg +xy_out=rad +step +proj=push +v_3 "
            "+step +proj=cart +ellps=krass +step +proj=helmert +x=2.3287 "
            "+y=-147.0425 +z=-92.0802 +rx=0.3092483 +ry=-0.32482185 "
            "+rz=-0.49729934 +s=5.68906266 +convention=coordinate_frame +step "
            "+inv +proj=cart +ellps=GRS80 +step +proj=pop +v_3 +step "
            "+proj=unitconvert +xy_in=rad +xy_out=deg +step +proj=axisswap "
            "+order=2,1");
    }

    // Reverse case
    {
        auto list = CoordinateOperationFactory::create()->createOperations(
            authFactory->createCoordinateReferenceSystem("4258"),
            authFactory->createCoordinateReferenceSystem("4179"), ctxt);
        ASSERT_EQ(list.size(), 3U);
        // Romania has a larger area than Poland (given our approx formula)
        EXPECT_EQ(list[0]->nameStr(),
                  "Inverse of Pulkovo 1942(58) to ETRS89 (4)"); // Romania - 3m

        EXPECT_EQ(
            list[0]->exportToPROJString(PROJStringFormatter::create().get()),
            "+proj=pipeline +step +proj=axisswap +order=2,1 +step "
            "+proj=unitconvert +xy_in=deg +xy_out=rad +step +proj=push +v_3 "
            "+step +proj=cart +ellps=GRS80 +step +inv +proj=helmert +x=2.3287 "
            "+y=-147.0425 +z=-92.0802 +rx=0.3092483 +ry=-0.32482185 "
            "+rz=-0.49729934 +s=5.68906266 +convention=coordinate_frame +step "
            "+inv +proj=cart +ellps=krass +step +proj=pop +v_3 +step "
            "+proj=unitconvert +xy_in=rad +xy_out=deg +step +proj=axisswap "
            "+order=2,1");
    }
}

// ---------------------------------------------------------------------------

TEST(operation, geogCRS_to_geogCRS_context_match_by_name) {
    auto authFactory =
        AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    auto ctxt = CoordinateOperationContext::create(authFactory, nullptr, 0);
    ctxt->setSpatialCriterion(
        CoordinateOperationContext::SpatialCriterion::PARTIAL_INTERSECTION);
    ctxt->setAllowUseIntermediateCRS(
        CoordinateOperationContext::IntermediateCRSUse::NEVER);
    auto NAD27 = GeographicCRS::create(
        PropertyMap().set(IdentifiedObject::NAME_KEY,
                          GeographicCRS::EPSG_4267->nameStr()),
        GeographicCRS::EPSG_4267->datum(),
        GeographicCRS::EPSG_4267->datumEnsemble(),
        GeographicCRS::EPSG_4267->coordinateSystem());
    auto list = CoordinateOperationFactory::create()->createOperations(
        NAD27, GeographicCRS::EPSG_4326, ctxt);
    auto listInv = CoordinateOperationFactory::create()->createOperations(
        GeographicCRS::EPSG_4326, NAD27, ctxt);
    auto listRef = CoordinateOperationFactory::create()->createOperations(
        GeographicCRS::EPSG_4267, GeographicCRS::EPSG_4326, ctxt);
    EXPECT_EQ(list.size(), listRef.size());
    EXPECT_EQ(listInv.size(), listRef.size());
    EXPECT_GE(listRef.size(), 2U);
}

// ---------------------------------------------------------------------------

TEST(operation, geogCRS_to_geogCRS_context_filter_accuracy) {
    auto authFactory =
        AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    {
        auto ctxt =
            CoordinateOperationContext::create(authFactory, nullptr, 1.0);
        ctxt->setSpatialCriterion(
            CoordinateOperationContext::SpatialCriterion::PARTIAL_INTERSECTION);

        auto list = CoordinateOperationFactory::create()->createOperations(
            authFactory->createCoordinateReferenceSystem("4179"),
            authFactory->createCoordinateReferenceSystem("4258"), ctxt);
        ASSERT_EQ(list.size(), 1U);
        EXPECT_EQ(list[0]->getEPSGCode(), 1644); // Poland - 1m
    }
    {
        auto ctxt =
            CoordinateOperationContext::create(authFactory, nullptr, 0.9);
        ctxt->setSpatialCriterion(
            CoordinateOperationContext::SpatialCriterion::PARTIAL_INTERSECTION);

        auto list = CoordinateOperationFactory::create()->createOperations(
            authFactory->createCoordinateReferenceSystem("4179"),
            authFactory->createCoordinateReferenceSystem("4258"), ctxt);
        ASSERT_EQ(list.size(), 0U);
    }
}

// ---------------------------------------------------------------------------

TEST(operation, geogCRS_to_geogCRS_context_filter_bbox) {
    auto authFactory =
        AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    // INSERT INTO "area" VALUES('EPSG','1197','Romania','Romania - onshore and
    // offshore.',43.44,48.27,20.26,31.41,0);
    {
        auto ctxt = CoordinateOperationContext::create(
            authFactory, Extent::createFromBBOX(20.26, 43.44, 31.41, 48.27),
            0.0);
        auto list = CoordinateOperationFactory::create()->createOperations(
            authFactory->createCoordinateReferenceSystem("4179"),
            authFactory->createCoordinateReferenceSystem("4258"), ctxt);
        ASSERT_EQ(list.size(), 1U);
        EXPECT_EQ(list[0]->getEPSGCode(), 15994); // Romania - 3m
    }
    {
        auto ctxt = CoordinateOperationContext::create(
            authFactory,
            Extent::createFromBBOX(20.26 + .1, 43.44 + .1, 31.41 - .1,
                                   48.27 - .1),
            0.0);
        auto list = CoordinateOperationFactory::create()->createOperations(
            authFactory->createCoordinateReferenceSystem("4179"),
            authFactory->createCoordinateReferenceSystem("4258"), ctxt);
        ASSERT_EQ(list.size(), 1U);
        EXPECT_EQ(list[0]->getEPSGCode(), 15994); // Romania - 3m
    }
    {
        auto ctxt = CoordinateOperationContext::create(
            authFactory,
            Extent::createFromBBOX(20.26 - .1, 43.44 - .1, 31.41 + .1,
                                   48.27 + .1),
            0.0);
        auto list = CoordinateOperationFactory::create()->createOperations(
            authFactory->createCoordinateReferenceSystem("4179"),
            authFactory->createCoordinateReferenceSystem("4258"), ctxt);
        ASSERT_EQ(list.size(), 1U);
        EXPECT_EQ(
            list[0]->exportToPROJString(PROJStringFormatter::create().get()),
            "+proj=noop");
    }
}

// ---------------------------------------------------------------------------

TEST(operation, geogCRS_to_geogCRS_context_incompatible_area) {
    auto authFactory =
        AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    auto ctxt = CoordinateOperationContext::create(authFactory, nullptr, 0.0);
    auto list = CoordinateOperationFactory::create()->createOperations(
        authFactory->createCoordinateReferenceSystem("4267"), // NAD27
        authFactory->createCoordinateReferenceSystem("4258"), // ETRS 89
        ctxt);
    ASSERT_EQ(list.size(), 1U);
    EXPECT_EQ(list[0]->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=noop");
}

// ---------------------------------------------------------------------------

TEST(operation, geogCRS_to_geogCRS_context_inverse_needed) {
    auto authFactory =
        AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    {
        auto ctxt =
            CoordinateOperationContext::create(authFactory, nullptr, 0.0);
        ctxt->setGridAvailabilityUse(
            CoordinateOperationContext::GridAvailabilityUse::
                IGNORE_GRID_AVAILABILITY);
        ctxt->setUsePROJAlternativeGridNames(false);
        auto list = CoordinateOperationFactory::create()->createOperations(
            authFactory->createCoordinateReferenceSystem("4275"), // NTF
            authFactory->createCoordinateReferenceSystem("4258"), // ETRS89
            ctxt);
        ASSERT_EQ(list.size(), 2U);
        EXPECT_EQ(
            list[0]->exportToPROJString(PROJStringFormatter::create().get()),
            "+proj=pipeline +step +proj=axisswap +order=2,1 +step "
            "+proj=unitconvert +xy_in=deg +xy_out=rad +step +proj=push +v_3 "
            "+step +proj=cart +ellps=clrk80ign +step +proj=helmert +x=-168 "
            "+y=-60 +z=320 +step +inv +proj=cart +ellps=GRS80 +step +proj=pop "
            "+v_3 +step +proj=unitconvert +xy_in=rad +xy_out=deg +step "
            "+proj=axisswap +order=2,1");
        EXPECT_EQ(list[1]->exportToPROJString(
                      PROJStringFormatter::create(
                          PROJStringFormatter::Convention::PROJ_5,
                          authFactory->databaseContext())
                          .get()),
                  "+proj=pipeline +step +proj=axisswap +order=2,1 +step "
                  "+proj=unitconvert +xy_in=deg +xy_out=rad +step "
                  "+proj=hgridshift +grids=fr_ign_ntf_r93.tif +step "
                  "+proj=unitconvert "
                  "+xy_in=rad +xy_out=deg +step +proj=axisswap +order=2,1");
    }
    {
        auto ctxt =
            CoordinateOperationContext::create(authFactory, nullptr, 0.0);
        ctxt->setGridAvailabilityUse(
            CoordinateOperationContext::GridAvailabilityUse::
                IGNORE_GRID_AVAILABILITY);
        auto list = CoordinateOperationFactory::create()->createOperations(
            authFactory->createCoordinateReferenceSystem("4275"), // NTF
            authFactory->createCoordinateReferenceSystem("4258"), // ETRS89
            ctxt);
        ASSERT_EQ(list.size(), 2U);
        EXPECT_EQ(
            list[0]->exportToPROJString(PROJStringFormatter::create().get()),
            "+proj=pipeline +step +proj=axisswap +order=2,1 +step "
            "+proj=unitconvert +xy_in=deg +xy_out=rad +step "
            "+proj=hgridshift +grids=fr_ign_ntf_r93.tif +step "
            "+proj=unitconvert "
            "+xy_in=rad +xy_out=deg +step +proj=axisswap +order=2,1");
    }
    {
        auto ctxt =
            CoordinateOperationContext::create(authFactory, nullptr, 0.0);
        ctxt->setGridAvailabilityUse(
            CoordinateOperationContext::GridAvailabilityUse::
                IGNORE_GRID_AVAILABILITY);
        auto list = CoordinateOperationFactory::create()->createOperations(
            authFactory->createCoordinateReferenceSystem("4258"), // ETRS89
            authFactory->createCoordinateReferenceSystem("4275"), // NTF
            ctxt);
        ASSERT_EQ(list.size(), 2U);
        EXPECT_EQ(
            list[0]->exportToPROJString(PROJStringFormatter::create().get()),
            "+proj=pipeline +step +proj=axisswap +order=2,1 +step "
            "+proj=unitconvert +xy_in=deg +xy_out=rad +step +inv "
            "+proj=hgridshift +grids=fr_ign_ntf_r93.tif +step "
            "+proj=unitconvert "
            "+xy_in=rad +xy_out=deg +step +proj=axisswap +order=2,1");
    }
}

// ---------------------------------------------------------------------------

TEST(operation, geogCRS_to_geogCRS_context_ntv1_ntv2_ctable2) {
    auto authFactory =
        AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    auto ctxt = CoordinateOperationContext::create(authFactory, nullptr, 0.0);
    ctxt->setSpatialCriterion(
        CoordinateOperationContext::SpatialCriterion::PARTIAL_INTERSECTION);
    ctxt->setGridAvailabilityUse(
        CoordinateOperationContext::GridAvailabilityUse::
            IGNORE_GRID_AVAILABILITY);

    auto list = CoordinateOperationFactory::create()->createOperations(
        authFactory->createCoordinateReferenceSystem("4267"), // NAD27
        authFactory->createCoordinateReferenceSystem("4269"), // NAD83
        ctxt);
    ASSERT_EQ(list.size(), 10U);
    EXPECT_EQ(list[0]->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=pipeline +step +proj=axisswap +order=2,1 +step "
              "+proj=unitconvert +xy_in=deg +xy_out=rad +step +proj=hgridshift "
              "+grids=ca_nrc_ntv2_0.tif +step +proj=unitconvert +xy_in=rad "
              "+xy_out=deg +step +proj=axisswap +order=2,1");
    EXPECT_EQ(list[1]->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=pipeline +step +proj=axisswap +order=2,1 +step "
              "+proj=unitconvert +xy_in=deg +xy_out=rad +step +proj=hgridshift "
              "+grids=ca_nrc_ntv1_can.tif +step +proj=unitconvert +xy_in=rad "
              "+xy_out=deg +step +proj=axisswap +order=2,1");
    EXPECT_EQ(list[2]->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=pipeline +step +proj=axisswap +order=2,1 +step "
              "+proj=unitconvert +xy_in=deg +xy_out=rad +step +proj=hgridshift "
              "+grids=us_noaa_conus.tif +step +proj=unitconvert +xy_in=rad "
              "+xy_out=deg "
              "+step +proj=axisswap +order=2,1");
}

// ---------------------------------------------------------------------------

TEST(operation, geogCRS_to_geogCRS_context_NAD27_to_WGS84) {
    auto authFactory =
        AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    auto ctxt = CoordinateOperationContext::create(authFactory, nullptr, 0.0);
    ctxt->setSpatialCriterion(
        CoordinateOperationContext::SpatialCriterion::PARTIAL_INTERSECTION);
    ctxt->setGridAvailabilityUse(
        CoordinateOperationContext::GridAvailabilityUse::
            IGNORE_GRID_AVAILABILITY);

    auto list = CoordinateOperationFactory::create()->createOperations(
        authFactory->createCoordinateReferenceSystem("4267"), // NAD27
        authFactory->createCoordinateReferenceSystem("4326"), // WGS84
        ctxt);
    ASSERT_EQ(list.size(), 79U);
    EXPECT_EQ(list[0]->nameStr(),
              "NAD27 to WGS 84 (33)"); // 1.0 m, Canada - NAD27
    EXPECT_EQ(list[1]->nameStr(),
              "NAD27 to WGS 84 (3)"); // 20.0 m, Canada - NAD27
    EXPECT_EQ(list[2]->nameStr(),
              "NAD27 to WGS 84 (79)"); // 5.0 m, USA - CONUS including EEZ
    EXPECT_EQ(list[3]->nameStr(),
              "NAD27 to WGS 84 (4)"); // 10.0 m, USA - CONUS - onshore
}

// ---------------------------------------------------------------------------

TEST(operation, geogCRS_to_geogCRS_context_NAD27_to_WGS84_G1762) {
    auto authFactory =
        AuthorityFactory::create(DatabaseContext::create(), std::string());
    auto ctxt = CoordinateOperationContext::create(authFactory, nullptr, 0.0);
    ctxt->setSpatialCriterion(
        CoordinateOperationContext::SpatialCriterion::PARTIAL_INTERSECTION);
    ctxt->setGridAvailabilityUse(
        CoordinateOperationContext::GridAvailabilityUse::
            IGNORE_GRID_AVAILABILITY);

    auto authFactoryEPSG =
        AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    auto list = CoordinateOperationFactory::create()->createOperations(
        // NAD27
        authFactoryEPSG->createCoordinateReferenceSystem("4267"),
        // WGS84 (G1762)
        authFactoryEPSG->createCoordinateReferenceSystem("9057"), ctxt);
    ASSERT_GE(list.size(), 78U);
    EXPECT_EQ(list[0]->nameStr(),
              "NAD27 to WGS 84 (33) + WGS 84 to WGS 84 (G1762)");
    EXPECT_EQ(list[0]->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=pipeline +step +proj=axisswap +order=2,1 "
              "+step +proj=unitconvert +xy_in=deg +xy_out=rad "
              "+step +proj=hgridshift +grids=ca_nrc_ntv2_0.tif "
              "+step +proj=unitconvert +xy_in=rad +xy_out=deg "
              "+step +proj=axisswap +order=2,1");
    EXPECT_EQ(list[1]->nameStr(),
              "NAD27 to WGS 84 (3) + WGS 84 to WGS 84 (G1762)");
    EXPECT_EQ(list[2]->nameStr(),
              "NAD27 to WGS 84 (79) + WGS 84 to WGS 84 (G1762)");
    EXPECT_EQ(list[3]->nameStr(),
              "NAD27 to WGS 84 (4) + WGS 84 to WGS 84 (G1762)");
}

// ---------------------------------------------------------------------------

TEST(operation, geogCRS_to_geogCRS_context_WGS84_G1674_to_WGS84_G1762) {
    // Check that particular behavior with WGS 84 (Gxxx) related to
    // 'geodetic_datum_preferred_hub' table and custom no-op transformations
    // between WGS 84 and WGS 84 (Gxxx) doesn't affect direct transformations
    // to those realizations.
    auto authFactory =
        AuthorityFactory::create(DatabaseContext::create(), std::string());
    auto ctxt = CoordinateOperationContext::create(authFactory, nullptr, 0.0);

    auto authFactoryEPSG =
        AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    auto list = CoordinateOperationFactory::create()->createOperations(
        // WGS84 (G1674)
        authFactoryEPSG->createCoordinateReferenceSystem("9056"),
        // WGS84 (G1762)
        authFactoryEPSG->createCoordinateReferenceSystem("9057"), ctxt);
    ASSERT_EQ(list.size(), 1U);
    EXPECT_EQ(list[0]->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=pipeline +step +proj=axisswap +order=2,1 "
              "+step +proj=unitconvert +xy_in=deg +xy_out=rad "
              "+step +proj=cart +ellps=WGS84 "
              "+step +proj=helmert +x=-0.004 +y=0.003 +z=0.004 +rx=0.00027 "
              "+ry=-0.00027 +rz=0.00038 +s=-0.0069 "
              "+convention=coordinate_frame "
              "+step +inv +proj=cart +ellps=WGS84 "
              "+step +proj=unitconvert +xy_in=rad +xy_out=deg "
              "+step +proj=axisswap +order=2,1");
}

// ---------------------------------------------------------------------------

TEST(operation, geogCRS_to_geogCRS_context_EPSG_4240_Indian1975_to_EPSG_4326) {
    auto authFactory =
        AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    auto ctxt = CoordinateOperationContext::create(authFactory, nullptr, 0);
    ctxt->setSpatialCriterion(
        CoordinateOperationContext::SpatialCriterion::PARTIAL_INTERSECTION);

    auto list = CoordinateOperationFactory::create()->createOperations(
        authFactory->createCoordinateReferenceSystem("4240"), // Indian 1975
        authFactory->createCoordinateReferenceSystem("4326"), ctxt);
    ASSERT_EQ(list.size(), 3U);

    // Indian 1975 to WGS 84 (4), 3.0 m, Thailand - onshore
    EXPECT_EQ(list[0]->getEPSGCode(), 1812);

    // The following is the one we want to see. It has a lesser accuracy than
    // the above one and the same bbox, but the name of its area of use is
    // slightly different
    // Indian 1975 to WGS 84 (2), 5.0 m, Thailand - onshore and Gulf of Thailand
    EXPECT_EQ(list[1]->getEPSGCode(), 1304);

    // Indian 1975 to WGS 84 (3), 1.0 m, Thailand - Bongkot field
    EXPECT_EQ(list[2]->getEPSGCode(), 1537);
}

// ---------------------------------------------------------------------------

TEST(operation, geogCRS_to_geogCRS_context_helmert_geog3D_crs) {
    auto authFactory =
        AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    auto ctxt = CoordinateOperationContext::create(authFactory, nullptr, 0);

    auto list = CoordinateOperationFactory::create()->createOperations(
        authFactory->createCoordinateReferenceSystem("4939"), // GDA94 3D
        authFactory->createCoordinateReferenceSystem("7843"), // GDA2020 3D
        ctxt);
    ASSERT_EQ(list.size(), 1U);

    // Check there is no push / pop of v_3
    EXPECT_EQ(list[0]->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=pipeline "
              "+step +proj=axisswap +order=2,1 "
              "+step +proj=unitconvert +xy_in=deg +z_in=m +xy_out=rad +z_out=m "
              "+step +proj=cart +ellps=GRS80 "
              "+step +proj=helmert +x=0.06155 +y=-0.01087 +z=-0.04019 "
              "+rx=-0.0394924 +ry=-0.0327221 +rz=-0.0328979 +s=-0.009994 "
              "+convention=coordinate_frame "
              "+step +inv +proj=cart +ellps=GRS80 "
              "+step +proj=unitconvert +xy_in=rad +z_in=m +xy_out=deg +z_out=m "
              "+step +proj=axisswap +order=2,1");
}

// ---------------------------------------------------------------------------

TEST(operation, geogCRS_to_geogCRS_context_helmert_geocentric_3D) {
    auto authFactory =
        AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    auto ctxt = CoordinateOperationContext::create(authFactory, nullptr, 0);

    auto list = CoordinateOperationFactory::create()->createOperations(
        // GDA94 geocentric
        authFactory->createCoordinateReferenceSystem("4348"),
        // GDA2020 geocentric
        authFactory->createCoordinateReferenceSystem("7842"), ctxt);
    ASSERT_EQ(list.size(), 1U);

    // Check there is no push / pop of v_3
    EXPECT_EQ(list[0]->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=helmert +x=0.06155 +y=-0.01087 +z=-0.04019 "
              "+rx=-0.0394924 +ry=-0.0327221 +rz=-0.0328979 +s=-0.009994 "
              "+convention=coordinate_frame");
    EXPECT_EQ(list[0]->inverse()->exportToPROJString(
                  PROJStringFormatter::create().get()),
              "+proj=pipeline "
              "+step +inv +proj=helmert +x=0.06155 +y=-0.01087 +z=-0.04019 "
              "+rx=-0.0394924 +ry=-0.0327221 +rz=-0.0328979 +s=-0.009994 "
              "+convention=coordinate_frame");
}

// ---------------------------------------------------------------------------

TEST(operation, geogCRS_to_geogCRS_context_helmert_geog3D_to_geocentirc) {
    auto authFactory =
        AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    auto ctxt = CoordinateOperationContext::create(authFactory, nullptr, 0);

    auto list = CoordinateOperationFactory::create()->createOperations(
        // GDA94 3D
        authFactory->createCoordinateReferenceSystem("4939"),
        // GDA2020 geocentric
        authFactory->createCoordinateReferenceSystem("7842"), ctxt);
    ASSERT_GE(list.size(), 1U);

    // Check there is no push / pop of v_3
    EXPECT_EQ(list[0]->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=pipeline "
              "+step +proj=axisswap +order=2,1 "
              "+step +proj=unitconvert +xy_in=deg +z_in=m +xy_out=rad +z_out=m "
              "+step +proj=cart +ellps=GRS80 "
              "+step +proj=helmert +x=0.06155 +y=-0.01087 +z=-0.04019 "
              "+rx=-0.0394924 +ry=-0.0327221 +rz=-0.0328979 +s=-0.009994 "
              "+convention=coordinate_frame");
}

// ---------------------------------------------------------------------------

TEST(operation, geogCRS_to_geogCRS_context_invalid_EPSG_ID) {
    auto authFactory =
        AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    auto ctxt = CoordinateOperationContext::create(authFactory, nullptr, 0);
    // EPSG:4656 is incorrect. Should be EPSG:8997
    auto obj = WKTParser().createFromWKT(
        "GEOGCS[\"ITRF2000\","
        "DATUM[\"International_Terrestrial_Reference_Frame_2000\","
        "SPHEROID[\"GRS 1980\",6378137,298.257222101,"
        "AUTHORITY[\"EPSG\",\"7019\"]],AUTHORITY[\"EPSG\",\"6656\"]],"
        "PRIMEM[\"Greenwich\",0],UNIT[\"Degree\",0.0174532925199433],"
        "AUTHORITY[\"EPSG\",\"4656\"]]");
    auto crs = nn_dynamic_pointer_cast<GeographicCRS>(obj);
    ASSERT_TRUE(crs != nullptr);

    auto list = CoordinateOperationFactory::create()->createOperations(
        NN_NO_CHECK(crs), GeographicCRS::EPSG_4326, ctxt);
    ASSERT_EQ(list.size(), 1U);
}

// ---------------------------------------------------------------------------

TEST(operation, geogCRS_to_geogCRS_context_datum_ensemble) {
    auto authFactory =
        AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    auto ctxt = CoordinateOperationContext::create(authFactory, nullptr, 0);

    auto dst_wkt =
        "GEOGCRS[\"unknown\","
        "  ENSEMBLE[\"World Geodetic System 1984 ensemble\","
        "     MEMBER[\"World Geodetic System 1984 (Transit)\","
        "       ID[\"EPSG\",1166]],"
        "     MEMBER[\"World Geodetic System 1984 (G730)\","
        "       ID[\"EPSG\",1152]],"
        "     MEMBER[\"World Geodetic System 1984 (G873)\","
        "       ID[\"EPSG\",1153]],"
        "     MEMBER[\"World Geodetic System 1984 (G1150)\","
        "       ID[\"EPSG\",1154]],"
        "     MEMBER[\"World Geodetic System 1984 (G1674)\","
        "       ID[\"EPSG\",1155]],"
        "     MEMBER[\"World Geodetic System 1984 (G1762)\","
        "       ID[\"EPSG\",1156]],"
        "     ELLIPSOID[\"WGS 84\",6378137,298.257223563,"
        "      LENGTHUNIT[\"metre\",1,ID[\"EPSG\",9001]],"
        "      ID[\"EPSG\",7030]],"
        "     ENSEMBLEACCURACY[2]],"
        "  PRIMEM[\"Greenwich\",0,"
        "    ANGLEUNIT[\"degree\",0.0174532925199433,ID[\"EPSG\",9102]],"
        "    ID[\"EPSG\",8901]],"
        "  CS[ellipsoidal,2,"
        "    ID[\"EPSG\",6422]],"
        "  AXIS[\"Geodetic latitude (Lat)\",north,"
        "    ORDER[1]],"
        "  AXIS[\"Geodetic longitude (Lon)\",east,"
        "    ORDER[2]],"
        "  ANGLEUNIT[\"degree (supplier to define representation)\","
        "0.0174532925199433,ID[\"EPSG\",9122]]]";
    auto dstObj = WKTParser().createFromWKT(dst_wkt);
    auto dstCRS = nn_dynamic_pointer_cast<CRS>(dstObj);
    ASSERT_TRUE(dstCRS != nullptr);

    auto list = CoordinateOperationFactory::create()->createOperations(
        authFactory->createCoordinateReferenceSystem("4258"), // ETRS89
        NN_NO_CHECK(dstCRS), ctxt);
    ASSERT_EQ(list.size(), 1U);
    EXPECT_EQ(list[0]->nameStr(), "ETRS89 to WGS 84 (1)");
}

// ---------------------------------------------------------------------------

TEST(operation, geogCRS_to_derived_geogCRS_3D) {
    auto authFactory =
        AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    auto ctxt = CoordinateOperationContext::create(authFactory, nullptr, 0);
    ctxt->setSpatialCriterion(
        CoordinateOperationContext::SpatialCriterion::PARTIAL_INTERSECTION);

    auto dst_wkt =
        "GEOGCRS[\"CH1903+ with 10m offset on ellipsoidal height\",\n"
        "    BASEGEOGCRS[\"CH1903+\",\n"
        "        DATUM[\"CH1903+\",\n"
        "            ELLIPSOID[\"Bessel 1841\",6377397.155,299.1528128,\n"
        "                LENGTHUNIT[\"metre\",1]],\n"
        "            ID[\"EPSG\",6150]],\n"
        "        PRIMEM[\"Greenwich\",0,\n"
        "            ANGLEUNIT[\"degree\",0.0174532925199433],\n"
        "            ID[\"EPSG\",8901]]],\n"
        "    DERIVINGCONVERSION[\"Offset on ellipsoidal height\",\n"
        "        METHOD[\"Geographic3D offsets\",\n"
        "            ID[\"EPSG\",9660]],\n"
        "        PARAMETER[\"Latitude offset\",0,\n"
        "            ANGLEUNIT[\"degree\",0.0174532925199433],\n"
        "            ID[\"EPSG\",8601]],\n"
        "        PARAMETER[\"Longitude offset\",0,\n"
        "            ANGLEUNIT[\"degree\",0.0174532925199433],\n"
        "            ID[\"EPSG\",8602]],\n"
        "        PARAMETER[\"Vertical Offset\",10,\n"
        "            LENGTHUNIT[\"metre\",1],\n"
        "            ID[\"EPSG\",8603]]],\n"
        "    CS[ellipsoidal,3],\n"
        "        AXIS[\"geodetic latitude (Lat)\",north,\n"
        "            ORDER[1],\n"
        "            ANGLEUNIT[\"degree\",0.0174532925199433,\n"
        "                ID[\"EPSG\",9122]]],\n"
        "        AXIS[\"geodetic longitude (Lon)\",east,\n"
        "            ORDER[2],\n"
        "            ANGLEUNIT[\"degree\",0.0174532925199433,\n"
        "                ID[\"EPSG\",9122]]],\n"
        "        AXIS[\"ellipsoidal height (h)\",up,\n"
        "            ORDER[3],\n"
        "            LENGTHUNIT[\"metre\",1,\n"
        "                ID[\"EPSG\",9001]]]]";
    auto dstObj = WKTParser().createFromWKT(dst_wkt);
    auto dstCRS = nn_dynamic_pointer_cast<CRS>(dstObj);
    ASSERT_TRUE(dstCRS != nullptr);

    auto list = CoordinateOperationFactory::create()->createOperations(
        authFactory->createCoordinateReferenceSystem("4979"), // WGS 84 3D
        NN_NO_CHECK(dstCRS), ctxt);
    ASSERT_GE(list.size(), 1U);
    EXPECT_EQ(
        list[0]->nameStr(),
        "Inverse of CH1903+ to WGS 84 (1) + Offset on ellipsoidal height");
    EXPECT_EQ(list[0]->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=pipeline "
              "+step +proj=axisswap +order=2,1 "
              "+step +proj=unitconvert +xy_in=deg +z_in=m +xy_out=rad +z_out=m "
              "+step +proj=cart +ellps=WGS84 "
              "+step +proj=helmert +x=-674.374 +y=-15.056 +z=-405.346 "
              "+step +inv +proj=cart +ellps=bessel "
              "+step +proj=geogoffset +dlat=0 +dlon=0 +dh=10 "
              "+step +proj=unitconvert +xy_in=rad +z_in=m +xy_out=deg +z_out=m "
              "+step +proj=axisswap +order=2,1");
}

// ---------------------------------------------------------------------------

TEST(operation, vertCRS_to_geogCRS_context) {
    auto authFactory =
        AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    {
        auto ctxt =
            CoordinateOperationContext::create(authFactory, nullptr, 0.0);
        ctxt->setUsePROJAlternativeGridNames(false);
        auto list = CoordinateOperationFactory::create()->createOperations(
            authFactory->createCoordinateReferenceSystem(
                "3855"), // EGM2008 height
            authFactory->createCoordinateReferenceSystem("4979"), // WGS 84
            ctxt);
        ASSERT_EQ(list.size(), 3U);
        EXPECT_EQ(
            list[1]->exportToPROJString(
                PROJStringFormatter::create(
                    PROJStringFormatter::Convention::PROJ_5,
                    authFactory->databaseContext())
                    .get()),
            "+proj=pipeline "
            "+step +proj=axisswap +order=2,1 "
            "+step +proj=unitconvert +xy_in=deg +xy_out=rad "
            "+step +proj=vgridshift +grids=us_nga_egm08_25.tif +multiplier=1 "
            "+step +proj=unitconvert +xy_in=rad +xy_out=deg "
            "+step +proj=axisswap +order=2,1");
    }
    {
        auto ctxt =
            CoordinateOperationContext::create(authFactory, nullptr, 0.0);
        auto list = CoordinateOperationFactory::create()->createOperations(
            authFactory->createCoordinateReferenceSystem(
                "3855"), // EGM2008 height
            authFactory->createCoordinateReferenceSystem("4979"), // WGS 84
            ctxt);
        ASSERT_EQ(list.size(), 3U);
        EXPECT_EQ(
            list[0]->exportToPROJString(PROJStringFormatter::create().get()),
            "+proj=pipeline "
            "+step +proj=axisswap +order=2,1 "
            "+step +proj=unitconvert +xy_in=deg +xy_out=rad "
            "+step +proj=vgridshift +grids=us_nga_egm08_25.tif +multiplier=1 "
            "+step +proj=unitconvert +xy_in=rad +xy_out=deg "
            "+step +proj=axisswap +order=2,1");
    }
    {
        auto ctxt =
            CoordinateOperationContext::create(authFactory, nullptr, 0.0);
        auto list = CoordinateOperationFactory::create()->createOperations(
            authFactory->createCoordinateReferenceSystem("4979"), // WGS 84
            authFactory->createCoordinateReferenceSystem(
                "3855"), // EGM2008 height
            ctxt);
        ASSERT_EQ(list.size(), 2U);
        EXPECT_EQ(
            list[0]->exportToPROJString(PROJStringFormatter::create().get()),
            "+proj=pipeline "
            "+step +proj=axisswap +order=2,1 "
            "+step +proj=unitconvert +xy_in=deg +xy_out=rad "
            "+step +inv +proj=vgridshift +grids=us_nga_egm08_25.tif "
            "+multiplier=1 "
            "+step +proj=unitconvert +xy_in=rad +xy_out=deg "
            "+step +proj=axisswap +order=2,1");
    }
    {
        auto ctxt =
            CoordinateOperationContext::create(authFactory, nullptr, 0.0);
        auto list = CoordinateOperationFactory::create()->createOperations(
            // NGVD29 depth (ftUS)
            authFactory->createCoordinateReferenceSystem("6359"),
            authFactory->createCoordinateReferenceSystem("4326"), ctxt);
        ASSERT_EQ(list.size(), 1U);
        EXPECT_EQ(
            list[0]->exportToPROJString(PROJStringFormatter::create().get()),
            "+proj=affine +s33=-0.304800609601219");
    }
    {
        auto ctxt =
            CoordinateOperationContext::create(authFactory, nullptr, 0.0);
        auto list = CoordinateOperationFactory::create()->createOperations(
            // NZVD2016 height
            authFactory->createCoordinateReferenceSystem("7839"),
            // NZGD2000
            authFactory->createCoordinateReferenceSystem("4959"), ctxt);
        ASSERT_EQ(list.size(), 2U);
        EXPECT_EQ(
            list[0]->exportToPROJString(PROJStringFormatter::create().get()),
            "+proj=pipeline "
            "+step +proj=axisswap +order=2,1 "
            "+step +proj=unitconvert +xy_in=deg +xy_out=rad "
            "+step +proj=vgridshift +grids=nz_linz_nzgeoid2016.tif "
            "+multiplier=1 "
            "+step +proj=unitconvert +xy_in=rad +xy_out=deg "
            "+step +proj=axisswap +order=2,1");
    }
    {
        // Test actually the database where we derive records using the more
        // classic 'Geographic3D to GravityRelatedHeight' method from
        // records using EPSG:1088
        //'Geog3D to Geog2D+GravityRelatedHeight (gtx)' method
        auto ctxt = CoordinateOperationContext::create(
            AuthorityFactory::create(DatabaseContext::create(), std::string()),
            nullptr, 0.0);
        ctxt->setSpatialCriterion(
            CoordinateOperationContext::SpatialCriterion::PARTIAL_INTERSECTION);
        auto list = CoordinateOperationFactory::create()->createOperations(
            // Baltic 1957 height
            authFactory->createCoordinateReferenceSystem("8357"),
            // ETRS89
            authFactory->createCoordinateReferenceSystem("4937"), ctxt);
        ASSERT_EQ(list.size(), 2U);
        EXPECT_EQ(
            list[0]->exportToPROJString(PROJStringFormatter::create().get()),
            "+proj=pipeline "
            "+step +proj=axisswap +order=2,1 "
            "+step +proj=unitconvert +xy_in=deg +xy_out=rad "
            "+step +proj=vgridshift "
            "+grids=sk_gku_Slovakia_ETRS89h_to_Baltic1957.tif "
            "+multiplier=1 "
            "+step +proj=unitconvert +xy_in=rad +xy_out=deg "
            "+step +proj=axisswap +order=2,1");
    }
}

// ---------------------------------------------------------------------------

TEST(operation, geog3DCRS_to_geog2DCRS_plus_vertCRS_context) {
    auto authFactory =
        AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    {
        auto ctxt =
            CoordinateOperationContext::create(authFactory, nullptr, 0.0);
        ctxt->setSpatialCriterion(
            CoordinateOperationContext::SpatialCriterion::PARTIAL_INTERSECTION);
        auto list = CoordinateOperationFactory::create()->createOperations(
            // ETRS89 (3D)
            authFactory->createCoordinateReferenceSystem("4937"),
            // ETRS89 + Baltic 1957 height
            authFactory->createCoordinateReferenceSystem("8360"), ctxt);
        ASSERT_GE(list.size(), 1U);
        EXPECT_EQ(
            list[0]->exportToPROJString(PROJStringFormatter::create().get()),
            "+proj=pipeline "
            "+step +proj=axisswap +order=2,1 "
            "+step +proj=unitconvert +xy_in=deg +xy_out=rad "
            "+step +inv +proj=vgridshift "
            "+grids=sk_gku_Slovakia_ETRS89h_to_Baltic1957.tif +multiplier=1 "
            "+step +proj=unitconvert +xy_in=rad +xy_out=deg "
            "+step +proj=axisswap +order=2,1");

        EXPECT_EQ(list[0]->inverse()->nameStr(),
                  "Inverse of 'ETRS89 to ETRS89 + Baltic 1957 height (1)'");
    }
}

// ---------------------------------------------------------------------------

TEST(operation, geog3DCRS_to_vertCRS_depth_context) {
    auto authFactory =
        AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    {
        auto ctxt =
            CoordinateOperationContext::create(authFactory, nullptr, 0.0);
        ctxt->setSpatialCriterion(
            CoordinateOperationContext::SpatialCriterion::PARTIAL_INTERSECTION);
        auto list = CoordinateOperationFactory::create()->createOperations(
            authFactory->createCoordinateReferenceSystem("4937"), // ETRS89
            authFactory->createCoordinateReferenceSystem("9672"),
            // CD Norway deph
            ctxt);
        ASSERT_GE(list.size(), 1U);
        EXPECT_EQ(list[0]->exportToPROJString(
                      PROJStringFormatter::create(
                          PROJStringFormatter::Convention::PROJ_5,
                          authFactory->databaseContext())
                          .get()),
                  "+proj=pipeline "
                  "+step +proj=axisswap +order=2,1 "
                  "+step +proj=unitconvert +xy_in=deg +xy_out=rad "
                  "+step +inv +proj=vgridshift "
                  "+grids=no_kv_CD_above_Ell_ETRS89_v2021a.tif +multiplier=1 "
                  "+step +proj=axisswap +order=1,2,-3 "
                  "+step +proj=unitconvert +xy_in=rad +xy_out=deg "
                  "+step +proj=axisswap +order=2,1");
    }
    {
        auto ctxt =
            CoordinateOperationContext::create(authFactory, nullptr, 0.0);
        ctxt->setSpatialCriterion(
            CoordinateOperationContext::SpatialCriterion::PARTIAL_INTERSECTION);
        auto list = CoordinateOperationFactory::create()->createOperations(
            authFactory->createCoordinateReferenceSystem("9672"),
            // CD Norway deph
            authFactory->createCoordinateReferenceSystem("4937"), // ETRS89
            ctxt);
        ASSERT_GE(list.size(), 1U);
        EXPECT_EQ(list[0]->exportToPROJString(
                      PROJStringFormatter::create(
                          PROJStringFormatter::Convention::PROJ_5,
                          authFactory->databaseContext())
                          .get()),
                  "+proj=pipeline "
                  "+step +proj=axisswap +order=2,1 "
                  "+step +proj=unitconvert +xy_in=deg +xy_out=rad "
                  "+step +proj=axisswap +order=1,2,-3 "
                  "+step +proj=vgridshift "
                  "+grids=no_kv_CD_above_Ell_ETRS89_v2021a.tif +multiplier=1 "
                  "+step +proj=unitconvert +xy_in=rad +xy_out=deg "
                  "+step +proj=axisswap +order=2,1");
    }
}

// ---------------------------------------------------------------------------

TEST(operation, geog3DCRS_to_geog2DCRS_plus_vertCRS_depth_context) {
    auto authFactory =
        AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    {
        auto ctxt =
            CoordinateOperationContext::create(authFactory, nullptr, 0.0);
        ctxt->setSpatialCriterion(
            CoordinateOperationContext::SpatialCriterion::PARTIAL_INTERSECTION);
        auto list = CoordinateOperationFactory::create()->createOperations(
            authFactory->createCoordinateReferenceSystem("4937"), // ETRS89
            authFactory->createCoordinateReferenceSystem("9883"),
            // ETRS89 + CD Norway deph
            ctxt);
        ASSERT_GE(list.size(), 1U);
        EXPECT_EQ(list[0]->exportToPROJString(
                      PROJStringFormatter::create(
                          PROJStringFormatter::Convention::PROJ_5,
                          authFactory->databaseContext())
                          .get()),
                  "+proj=pipeline "
                  "+step +proj=axisswap +order=2,1 "
                  "+step +proj=unitconvert +xy_in=deg +xy_out=rad "
                  "+step +inv +proj=vgridshift "
                  "+grids=no_kv_CD_above_Ell_ETRS89_v2021a.tif +multiplier=1 "
                  "+step +proj=axisswap +order=1,2,-3 "
                  "+step +proj=unitconvert +xy_in=rad +xy_out=deg "
                  "+step +proj=axisswap +order=2,1");
    }
    {
        auto ctxt =
            CoordinateOperationContext::create(authFactory, nullptr, 0.0);
        ctxt->setSpatialCriterion(
            CoordinateOperationContext::SpatialCriterion::PARTIAL_INTERSECTION);
        auto list = CoordinateOperationFactory::create()->createOperations(
            authFactory->createCoordinateReferenceSystem("9883"),
            // ETRS89 + CD Norway deph
            authFactory->createCoordinateReferenceSystem("4937"), // ETRS89
            ctxt);
        ASSERT_GE(list.size(), 1U);
        EXPECT_EQ(list[0]->exportToPROJString(
                      PROJStringFormatter::create(
                          PROJStringFormatter::Convention::PROJ_5,
                          authFactory->databaseContext())
                          .get()),
                  "+proj=pipeline "
                  "+step +proj=axisswap +order=2,1 "
                  "+step +proj=unitconvert +xy_in=deg +xy_out=rad "
                  "+step +proj=axisswap +order=1,2,-3 "
                  "+step +proj=vgridshift "
                  "+grids=no_kv_CD_above_Ell_ETRS89_v2021a.tif +multiplier=1 "
                  "+step +proj=unitconvert +xy_in=rad +xy_out=deg "
                  "+step +proj=axisswap +order=2,1");
    }
}

// ---------------------------------------------------------------------------

TEST(operation, geogCRS_to_geogCRS_noop) {

    auto op = CoordinateOperationFactory::create()->createOperation(
        GeographicCRS::EPSG_4326, GeographicCRS::EPSG_4326);
    ASSERT_TRUE(op != nullptr);
    EXPECT_EQ(op->nameStr(), "Null geographic offset from WGS 84 to WGS 84");
    EXPECT_EQ(op->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=noop");
    EXPECT_EQ(op->inverse()->nameStr(), op->nameStr());
}

// ---------------------------------------------------------------------------

TEST(operation, geogCRS_to_geogCRS_longitude_rotation) {

    auto src = GeographicCRS::create(
        PropertyMap().set(IdentifiedObject::NAME_KEY, "A"),
        GeodeticReferenceFrame::create(PropertyMap(), Ellipsoid::WGS84,
                                       optional<std::string>(),
                                       PrimeMeridian::GREENWICH),
        EllipsoidalCS::createLatitudeLongitude(UnitOfMeasure::DEGREE));
    auto dest = GeographicCRS::create(
        PropertyMap().set(IdentifiedObject::NAME_KEY, "B"),
        GeodeticReferenceFrame::create(PropertyMap(), Ellipsoid::WGS84,
                                       optional<std::string>(),
                                       PrimeMeridian::PARIS),
        EllipsoidalCS::createLatitudeLongitude(UnitOfMeasure::DEGREE));

    auto op = CoordinateOperationFactory::create()->createOperation(src, dest);
    ASSERT_TRUE(op != nullptr);
    EXPECT_EQ(op->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=pipeline +step +proj=axisswap +order=2,1 +step "
              "+proj=unitconvert +xy_in=deg +xy_out=rad +step +proj=longlat "
              "+ellps=WGS84 +pm=paris +step +proj=unitconvert +xy_in=rad "
              "+xy_out=deg +step +proj=axisswap +order=2,1");
    EXPECT_EQ(op->inverse()->exportToWKT(WKTFormatter::create().get()),
              CoordinateOperationFactory::create()
                  ->createOperation(dest, src)
                  ->exportToWKT(WKTFormatter::create().get()));
    EXPECT_TRUE(
        op->inverse()->isEquivalentTo(CoordinateOperationFactory::create()
                                          ->createOperation(dest, src)
                                          .get()));
}

// ---------------------------------------------------------------------------

TEST(operation, geogCRS_to_geogCRS_longitude_rotation_context) {
    auto authFactory =
        AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    auto ctxt = CoordinateOperationContext::create(authFactory, nullptr, 0.0);
    auto list = CoordinateOperationFactory::create()->createOperations(
        authFactory->createCoordinateReferenceSystem("4807"), // NTF(Paris)
        authFactory->createCoordinateReferenceSystem("4275"), // NTF
        ctxt);
    ASSERT_EQ(list.size(), 2U);
    EXPECT_EQ(list[0]->nameStr(), "NTF (Paris) to NTF (1)");
    EXPECT_EQ(list[0]->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=pipeline +step +proj=axisswap +order=2,1 +step "
              "+proj=unitconvert +xy_in=grad +xy_out=rad +step +inv "
              "+proj=longlat +ellps=clrk80ign +pm=paris +step "
              "+proj=unitconvert +xy_in=rad +xy_out=deg +step +proj=axisswap "
              "+order=2,1");
    EXPECT_EQ(list[1]->nameStr(), "NTF (Paris) to NTF (2)");
    EXPECT_EQ(list[1]->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=pipeline +step +proj=axisswap +order=2,1 +step "
              "+proj=unitconvert +xy_in=grad +xy_out=rad +step +inv "
              "+proj=longlat +ellps=clrk80ign +pm=2.33720833333333 +step "
              "+proj=unitconvert +xy_in=rad +xy_out=deg +step +proj=axisswap "
              "+order=2,1");
}

// ---------------------------------------------------------------------------

TEST(operation, geogCRS_to_geogCRS_context_concatenated_operation) {
    auto authFactory =
        AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    auto ctxt = CoordinateOperationContext::create(authFactory, nullptr, 0.0);
    ctxt->setGridAvailabilityUse(
        CoordinateOperationContext::GridAvailabilityUse::
            IGNORE_GRID_AVAILABILITY);
    ctxt->setAllowUseIntermediateCRS(
        CoordinateOperationContext::IntermediateCRSUse::ALWAYS);
    auto list = CoordinateOperationFactory::create()->createOperations(
        authFactory->createCoordinateReferenceSystem("4807"), // NTF(Paris)
        authFactory->createCoordinateReferenceSystem("4171"), // RGF93
        ctxt);
    ASSERT_EQ(list.size(), 4U);

    EXPECT_EQ(list[0]->nameStr(), "NTF (Paris) to RGF93 v1 (1)");
    EXPECT_EQ(list[0]->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=pipeline "
              "+step +proj=axisswap +order=2,1 "
              "+step +proj=unitconvert +xy_in=grad +xy_out=rad "
              "+step +inv +proj=longlat +ellps=clrk80ign +pm=paris "
              "+step +proj=push +v_3 "
              "+step +proj=cart +ellps=clrk80ign "
              "+step +proj=xyzgridshift +grids=fr_ign_gr3df97a.tif "
              "+grid_ref=output_crs +ellps=GRS80 "
              "+step +inv +proj=cart +ellps=GRS80 "
              "+step +proj=pop +v_3 "
              "+step +proj=unitconvert +xy_in=rad +xy_out=deg "
              "+step +proj=axisswap +order=2,1");

    EXPECT_EQ(list[1]->nameStr(), "NTF (Paris) to RGF93 v1 (2)");
    EXPECT_EQ(list[1]->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=pipeline +step +proj=axisswap +order=2,1 +step "
              "+proj=unitconvert +xy_in=grad +xy_out=rad +step +inv "
              "+proj=longlat +ellps=clrk80ign +pm=paris +step +proj=hgridshift "
              "+grids=fr_ign_ntf_r93.tif +step +proj=unitconvert +xy_in=rad "
              "+xy_out=deg +step +proj=axisswap +order=2,1");

    EXPECT_TRUE(nn_dynamic_pointer_cast<ConcatenatedOperation>(list[0]) !=
                nullptr);
    auto grids = list[0]->gridsNeeded(DatabaseContext::create(), false);
    EXPECT_EQ(grids.size(), 1U);
}

// ---------------------------------------------------------------------------

TEST(operation, geogCRS_to_geogCRS_context_ED50_to_WGS72_no_NTF_intermediate) {
    auto authFactory =
        AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    auto ctxt = CoordinateOperationContext::create(authFactory, nullptr, 0.0);
    ctxt->setSpatialCriterion(
        CoordinateOperationContext::SpatialCriterion::PARTIAL_INTERSECTION);
    auto list = CoordinateOperationFactory::create()->createOperations(
        authFactory->createCoordinateReferenceSystem("4230"), // ED50
        authFactory->createCoordinateReferenceSystem("4322"), // WGS 72
        ctxt);
    ASSERT_GE(list.size(), 2U);
    // We should not use the ancient NTF as an intermediate when looking for
    // ED50 -> WGS 72 operations.
    for (const auto &op : list) {
        EXPECT_TRUE(op->nameStr().find("NTF") == std::string::npos)
            << op->nameStr();
    }
}

// ---------------------------------------------------------------------------

TEST(operation, geogCRS_to_geogCRS_context_same_grid_name) {
    auto authFactory =
        AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    auto ctxt = CoordinateOperationContext::create(authFactory, nullptr, 0.0);
    ctxt->setGridAvailabilityUse(
        CoordinateOperationContext::GridAvailabilityUse::
            IGNORE_GRID_AVAILABILITY);
    auto list = CoordinateOperationFactory::create()->createOperations(
        authFactory->createCoordinateReferenceSystem("4314"), // DHDN
        authFactory->createCoordinateReferenceSystem("4258"), // ETRS89
        ctxt);
    ASSERT_TRUE(!list.empty());
    EXPECT_EQ(list[0]->nameStr(), "DHDN to ETRS89 (8)");
    EXPECT_EQ(list[0]->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=pipeline +step +proj=axisswap +order=2,1 +step "
              "+proj=unitconvert +xy_in=deg +xy_out=rad +step +proj=hgridshift "
              "+grids=de_adv_BETA2007.tif +step +proj=unitconvert +xy_in=rad "
              "+xy_out=deg +step +proj=axisswap +order=2,1");
}

// ---------------------------------------------------------------------------

TEST(operation, geogCRS_to_geogCRS_geographic_offset_context) {
    auto authFactory =
        AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    auto ctxt = CoordinateOperationContext::create(authFactory, nullptr, 0.0);
    auto list = CoordinateOperationFactory::create()->createOperations(
        authFactory->createCoordinateReferenceSystem("4120"), // NTF(Paris)
        authFactory->createCoordinateReferenceSystem("4121"), // NTF
        ctxt);
    ASSERT_EQ(list.size(), 1U);
    EXPECT_EQ(list[0]->nameStr(), "Greek to GGRS87 (1)");
    EXPECT_EQ(list[0]->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=pipeline +step +proj=axisswap +order=2,1 +step "
              "+proj=unitconvert +xy_in=deg +xy_out=rad +step +proj=geogoffset "
              "+dlat=-5.86 +dlon=0.28 +step +proj=unitconvert +xy_in=rad "
              "+xy_out=deg +step +proj=axisswap +order=2,1");
}

// ---------------------------------------------------------------------------

TEST(operation, geogCRS_to_geogCRS_CH1903_to_CH1903plus_context) {
    auto authFactory =
        AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    auto ctxt = CoordinateOperationContext::create(authFactory, nullptr, 0.0);
    ctxt->setAllowUseIntermediateCRS(
        CoordinateOperationContext::IntermediateCRSUse::ALWAYS);
    ctxt->setGridAvailabilityUse(
        CoordinateOperationContext::GridAvailabilityUse::
            IGNORE_GRID_AVAILABILITY);
    auto list = CoordinateOperationFactory::create()->createOperations(
        authFactory->createCoordinateReferenceSystem("4149"), // CH1903
        authFactory->createCoordinateReferenceSystem("4150"), // CH1903+
        ctxt);
    ASSERT_TRUE(list.size() == 1U);

    EXPECT_EQ(list[0]->nameStr(), "CH1903 to CH1903+ (1)");
    EXPECT_EQ(list[0]->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=pipeline +step +proj=axisswap +order=2,1 "
              "+step +proj=unitconvert +xy_in=deg +xy_out=rad "
              "+step +proj=hgridshift +grids=ch_swisstopo_CHENyx06a.tif "
              "+step +proj=unitconvert +xy_in=rad +xy_out=deg "
              "+step +proj=axisswap +order=2,1");
}

// ---------------------------------------------------------------------------

TEST(operation, geogCRS_to_geogCRS_init_IGNF_to_init_IGNF_context) {

    auto dbContext = DatabaseContext::create();

    auto sourceCRS_obj = PROJStringParser()
                             .attachDatabaseContext(dbContext)
                             .setUsePROJ4InitRules(true)
                             .createFromPROJString("+init=IGNF:NTFG");
    auto sourceCRS = nn_dynamic_pointer_cast<CRS>(sourceCRS_obj);
    ASSERT_TRUE(sourceCRS != nullptr);

    auto targetCRS_obj = PROJStringParser()
                             .attachDatabaseContext(dbContext)
                             .setUsePROJ4InitRules(true)
                             .createFromPROJString("+init=IGNF:RGF93G");
    auto targetCRS = nn_dynamic_pointer_cast<CRS>(targetCRS_obj);
    ASSERT_TRUE(targetCRS != nullptr);

    auto authFactory = AuthorityFactory::create(dbContext, std::string());
    auto ctxt = CoordinateOperationContext::create(authFactory, nullptr, 0.0);
    auto list = CoordinateOperationFactory::create()->createOperations(
        NN_CHECK_ASSERT(sourceCRS), NN_CHECK_ASSERT(targetCRS), ctxt);
    ASSERT_EQ(list.size(), 2U);

    EXPECT_EQ(list[0]->nameStr(),
              "NOUVELLE TRIANGULATION DE LA FRANCE (NTF) vers RGF93 (ETRS89)");
    EXPECT_EQ(list[0]->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=pipeline +step +proj=unitconvert +xy_in=deg +xy_out=rad "
              "+step +proj=hgridshift +grids=fr_ign_ntf_r93.tif +step "
              "+proj=unitconvert +xy_in=rad +xy_out=deg");
}

// ---------------------------------------------------------------------------

TEST(operation, geogCRS_to_geogCRS_context_deprecated) {
    auto authFactory =
        AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    auto ctxt = CoordinateOperationContext::create(authFactory, nullptr, 0.0);
    auto list = CoordinateOperationFactory::create()->createOperations(
        authFactory->createCoordinateReferenceSystem(
            "4226"), // Cote d'Ivoire (deprecated)
        authFactory->createCoordinateReferenceSystem("4258"), // ETRS89
        ctxt);
    ASSERT_TRUE(!list.empty());
    EXPECT_EQ(list[0]->nameStr(),
              "Ballpark geographic offset from Cote d'Ivoire to ETRS89");
}

// ---------------------------------------------------------------------------

TEST(operation, geogCRS_to_geogCRS_3D) {

    auto geogcrs_m_obj = PROJStringParser().createFromPROJString(
        "+proj=longlat +vunits=m +type=crs");
    auto geogcrs_m = nn_dynamic_pointer_cast<CRS>(geogcrs_m_obj);
    ASSERT_TRUE(geogcrs_m != nullptr);

    auto geogcrs_ft_obj = PROJStringParser().createFromPROJString(
        "+proj=longlat +vunits=ft +type=crs");
    auto geogcrs_ft = nn_dynamic_pointer_cast<CRS>(geogcrs_ft_obj);
    ASSERT_TRUE(geogcrs_ft != nullptr);

    {
        auto op = CoordinateOperationFactory::create()->createOperation(
            NN_CHECK_ASSERT(geogcrs_m), NN_CHECK_ASSERT(geogcrs_ft));
        ASSERT_TRUE(op != nullptr);
        EXPECT_EQ(op->exportToPROJString(PROJStringFormatter::create().get()),
                  "+proj=unitconvert +z_in=m +z_out=ft");
    }

    {
        auto op = CoordinateOperationFactory::create()->createOperation(
            NN_CHECK_ASSERT(geogcrs_ft), NN_CHECK_ASSERT(geogcrs_m));
        ASSERT_TRUE(op != nullptr);
        EXPECT_EQ(op->exportToPROJString(PROJStringFormatter::create().get()),
                  "+proj=unitconvert +z_in=ft +z_out=m");
    }

    auto geogcrs_m_with_pm_obj = PROJStringParser().createFromPROJString(
        "+proj=longlat +pm=paris +vunits=m +type=crs");
    auto geogcrs_m_with_pm =
        nn_dynamic_pointer_cast<CRS>(geogcrs_m_with_pm_obj);
    ASSERT_TRUE(geogcrs_m_with_pm != nullptr);

    {
        auto op = CoordinateOperationFactory::create()->createOperation(
            NN_CHECK_ASSERT(geogcrs_m_with_pm), NN_CHECK_ASSERT(geogcrs_ft));
        ASSERT_TRUE(op != nullptr);
        EXPECT_EQ(op->exportToPROJString(PROJStringFormatter::create().get()),
                  "+proj=pipeline +step +proj=unitconvert +xy_in=deg +z_in=m "
                  "+xy_out=rad +z_out=m +step +inv +proj=longlat +ellps=WGS84 "
                  "+pm=paris +step +proj=unitconvert +xy_in=rad +z_in=m "
                  "+xy_out=deg +z_out=ft");
    }
}

// ---------------------------------------------------------------------------

TEST(operation, geogCRS_3D_lat_long_non_metre_to_geogCRS_longlat) {

    auto wkt = "GEOGCRS[\"my CRS\",\n"
               "    DATUM[\"World Geodetic System 1984\",\n"
               "        ELLIPSOID[\"WGS 84\",6378137,298.257223563],\n"
               "        ID[\"EPSG\",6326]],\n"
               "    CS[ellipsoidal,3],\n"
               "        AXIS[\"latitude\",north,\n"
               "            ANGLEUNIT[\"degree\",0.0174532925199433]],\n"
               "        AXIS[\"longitude\",east,\n"
               "            ANGLEUNIT[\"degree\",0.0174532925199433]],\n"
               "        AXIS[\"ellipsoidal height\",up,\n"
               "            LENGTHUNIT[\"my_vunit\",0.3]]]";
    auto srcCRS_obj = WKTParser().createFromWKT(wkt);
    auto srcCRS = nn_dynamic_pointer_cast<CRS>(srcCRS_obj);
    ASSERT_TRUE(srcCRS != nullptr);

    auto dstCRS_obj = PROJStringParser().createFromPROJString(
        "+proj=longlat +datum=WGS84 +type=crs");
    auto dstCRS = nn_dynamic_pointer_cast<CRS>(dstCRS_obj);
    ASSERT_TRUE(dstCRS != nullptr);

    auto op = CoordinateOperationFactory::create()->createOperation(
        NN_CHECK_ASSERT(srcCRS), NN_CHECK_ASSERT(dstCRS));
    ASSERT_TRUE(op != nullptr);
    EXPECT_EQ(op->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=pipeline +step +proj=axisswap +order=2,1 +step "
              "+proj=unitconvert +z_in=0.3 +z_out=m");
}

// ---------------------------------------------------------------------------

TEST(operation, geogCRS_without_id_to_geogCRS_3D_context) {
    auto authFactory =
        AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    auto ctxt = CoordinateOperationContext::create(authFactory, nullptr, 0.0);
    auto src =
        authFactory->createCoordinateReferenceSystem("4289"); // Amersfoort
    auto dst =
        authFactory->createCoordinateReferenceSystem("4937"); // ETRS89 3D
    auto list =
        CoordinateOperationFactory::create()->createOperations(src, dst, ctxt);
    ASSERT_GE(list.size(), 1U);
    auto wkt2 = "GEOGCRS[\"unnamed\",\n"
                "    DATUM[\"Amersfoort\",\n"
                "        ELLIPSOID[\"Bessel 1841\",6377397.155,299.1528128,\n"
                "            LENGTHUNIT[\"metre\",1]]],\n"
                "    PRIMEM[\"Greenwich\",0,\n"
                "        ANGLEUNIT[\"degree\",0.0174532925199433]],\n"
                "    CS[ellipsoidal,2],\n"
                "        AXIS[\"geodetic latitude (Lat)\",north,\n"
                "            ORDER[1],\n"
                "            ANGLEUNIT[\"degree\",0.0174532925199433]],\n"
                "        AXIS[\"geodetic longitude (Lon)\",east,\n"
                "            ORDER[2],\n"
                "            ANGLEUNIT[\"degree\",0.0174532925199433]],"
                "    USAGE[\n"
                "        SCOPE[\"unknown\"],\n"
                "        AREA[\"Netherlands - onshore\"],\n"
                "        BBOX[50.75,3.2,53.7,7.22]]]\n";

    auto obj = WKTParser().createFromWKT(wkt2);
    auto src_from_wkt2 = nn_dynamic_pointer_cast<CRS>(obj);
    ASSERT_TRUE(src_from_wkt2 != nullptr);
    auto list2 = CoordinateOperationFactory::create()->createOperations(
        NN_NO_CHECK(src_from_wkt2), dst, ctxt);
    ASSERT_GE(list.size(), list2.size());
    for (size_t i = 0; i < list.size(); i++) {
        const auto &op = list[i];
        const auto &op2 = list2[i];
        EXPECT_TRUE(
            op->isEquivalentTo(op2.get(), IComparable::Criterion::EQUIVALENT))
            << op->nameStr() << " " << op2->nameStr();
    }
}

// ---------------------------------------------------------------------------

static GeodeticCRSNNPtr createGeocentricDatumWGS84() {
    PropertyMap propertiesCRS;
    propertiesCRS.set(Identifier::CODESPACE_KEY, "EPSG")
        .set(Identifier::CODE_KEY, 4328)
        .set(IdentifiedObject::NAME_KEY, "WGS 84");
    return GeodeticCRS::create(
        propertiesCRS, GeodeticReferenceFrame::EPSG_6326,
        CartesianCS::createGeocentric(UnitOfMeasure::METRE));
}

// ---------------------------------------------------------------------------

static GeodeticCRSNNPtr createGeocentricKM() {
    PropertyMap propertiesCRS;
    propertiesCRS.set(IdentifiedObject::NAME_KEY, "Based on WGS 84");
    return GeodeticCRS::create(
        propertiesCRS, GeodeticReferenceFrame::EPSG_6326,
        CartesianCS::createGeocentric(
            UnitOfMeasure("kilometre", 1000.0, UnitOfMeasure::Type::LINEAR)));
}

// ---------------------------------------------------------------------------

TEST(operation, geocentricCRS_to_geogCRS_same_datum) {

    auto op = CoordinateOperationFactory::create()->createOperation(
        createGeocentricDatumWGS84(), GeographicCRS::EPSG_4326);
    ASSERT_TRUE(op != nullptr);
    EXPECT_EQ(op->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=pipeline +step +inv +proj=cart +ellps=WGS84 +step "
              "+proj=unitconvert +xy_in=rad +xy_out=deg +step +proj=axisswap "
              "+order=2,1");
}

// ---------------------------------------------------------------------------

TEST(operation, geocentricCRS_to_geogCRS_different_datum) {

    auto op = CoordinateOperationFactory::create()->createOperation(
        createGeocentricDatumWGS84(), GeographicCRS::EPSG_4269);
    ASSERT_TRUE(op != nullptr);
    EXPECT_EQ(op->nameStr(),
              "Conversion from WGS 84 to WGS 84 (geographic) + "
              "Ballpark geographic offset from WGS 84 (geographic) to NAD83");
    EXPECT_EQ(op->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=pipeline "
              "+step +inv +proj=cart +ellps=WGS84 "
              "+step +proj=unitconvert +xy_in=rad +xy_out=deg "
              "+step +proj=axisswap +order=2,1");
}

// ---------------------------------------------------------------------------

TEST(operation, geogCRS_to_geocentricCRS_different_datum) {

    auto op = CoordinateOperationFactory::create()->createOperation(
        GeographicCRS::EPSG_4269, createGeocentricDatumWGS84());
    ASSERT_TRUE(op != nullptr);
    EXPECT_EQ(op->nameStr(),
              "Ballpark geographic offset from NAD83 to WGS 84 (geographic) + "
              "Conversion from WGS 84 (geographic) to WGS 84");
    EXPECT_EQ(op->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=pipeline "
              "+step +proj=axisswap +order=2,1 "
              "+step +proj=unitconvert +xy_in=deg +z_in=m +xy_out=rad +z_out=m "
              "+step +proj=cart +ellps=WGS84");
}

// ---------------------------------------------------------------------------

TEST(operation, geocentricCRS_to_geocentricCRS_same_noop) {

    auto op = CoordinateOperationFactory::create()->createOperation(
        createGeocentricDatumWGS84(), createGeocentricDatumWGS84());
    ASSERT_TRUE(op != nullptr);
    EXPECT_EQ(op->nameStr(),
              "Null geocentric translation from WGS 84 to WGS 84");
    EXPECT_EQ(op->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=noop");
    EXPECT_EQ(op->inverse()->nameStr(), op->nameStr());
}

// ---------------------------------------------------------------------------

TEST(operation, geocentricCRS_to_geocentricCRS_different_ballpark) {

    PropertyMap propertiesCRS;
    propertiesCRS.set(Identifier::CODESPACE_KEY, "EPSG")
        .set(Identifier::CODE_KEY, 4328)
        .set(IdentifiedObject::NAME_KEY, "unknown");
    auto otherGeocentricCRS = GeodeticCRS::create(
        propertiesCRS, GeodeticReferenceFrame::EPSG_6269,
        CartesianCS::createGeocentric(UnitOfMeasure::METRE));

    auto op = CoordinateOperationFactory::create()->createOperation(
        createGeocentricKM(), otherGeocentricCRS);
    ASSERT_TRUE(op != nullptr);
    EXPECT_EQ(
        op->nameStr(),
        "Ballpark geocentric translation from Based on WGS 84 to unknown");
    EXPECT_EQ(op->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=unitconvert +xy_in=km +z_in=km +xy_out=m +z_out=m");
}

// ---------------------------------------------------------------------------

TEST(operation, geocentricCRS_to_geogCRS_same_datum_context) {
    auto authFactory =
        AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    auto ctxt = CoordinateOperationContext::create(authFactory, nullptr, 0.0);
    auto list = CoordinateOperationFactory::create()->createOperations(
        authFactory->createCoordinateReferenceSystem("4326"),
        // WGS84 geocentric
        authFactory->createCoordinateReferenceSystem("4978"), ctxt);
    ASSERT_EQ(list.size(), 1U);

    EXPECT_EQ(list[0]->nameStr(),
              "Conversion from WGS 84 (geog2D) to WGS 84 (geocentric)");
    EXPECT_EQ(list[0]->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=pipeline +step +proj=axisswap +order=2,1 +step "
              "+proj=unitconvert +xy_in=deg +xy_out=rad +step +proj=cart "
              "+ellps=WGS84");

    EXPECT_EQ(list[0]->inverse()->nameStr(),
              "Conversion from WGS 84 (geocentric) to WGS 84 (geog2D)");
    EXPECT_EQ(list[0]->inverse()->exportToPROJString(
                  PROJStringFormatter::create().get()),
              "+proj=pipeline +step +inv +proj=cart +ellps=WGS84 +step "
              "+proj=unitconvert +xy_in=rad +xy_out=deg +step +proj=axisswap "
              "+order=2,1");
}

// ---------------------------------------------------------------------------

TEST(operation, geocentricCRS_to_geogCRS_same_datum_context_all_auth) {
    // This is to check we don't use OGC:CRS84 as a pivot
    auto authFactoryEPSG =
        AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    auto authFactoryAll =
        AuthorityFactory::create(DatabaseContext::create(), std::string());
    auto ctxt =
        CoordinateOperationContext::create(authFactoryAll, nullptr, 0.0);
    auto list = CoordinateOperationFactory::create()->createOperations(
        authFactoryEPSG->createCoordinateReferenceSystem("4326"),
        // WGS84 geocentric
        authFactoryEPSG->createCoordinateReferenceSystem("4978"), ctxt);
    ASSERT_EQ(list.size(), 1U);

    EXPECT_EQ(list[0]->nameStr(),
              "Conversion from WGS 84 (geog2D) to WGS 84 (geocentric)");
}

// ---------------------------------------------------------------------------

TEST(operation, geocentricCRS_to_geocentricCRS_different_datum_context) {
    auto authFactory =
        AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    auto ctxt = CoordinateOperationContext::create(authFactory, nullptr, 0.0);
    auto list = CoordinateOperationFactory::create()->createOperations(
        // ITRF2000 (geocentric)
        authFactory->createCoordinateReferenceSystem("4919"),
        // ITRF2005 (geocentric)
        authFactory->createCoordinateReferenceSystem("4896"), ctxt);
    ASSERT_EQ(list.size(), 1U);
    EXPECT_EQ(list[0]->nameStr(), "ITRF2000 to ITRF2005 (1)");
    EXPECT_PRED_FORMAT2(
        ComparePROJString,
        list[0]->exportToPROJString(PROJStringFormatter::create().get()),
        "+proj=helmert +x=-0.0001 "
        "+y=0.0008 +z=0.0058 +rx=0 +ry=0 +rz=0 +s=-0.0004 +dx=0.0002 "
        "+dy=-0.0001 +dz=0.0018 +drx=0 +dry=0 +drz=0 +ds=-8e-05 "
        "+t_epoch=2000 +convention=position_vector");
}

// ---------------------------------------------------------------------------

TEST(operation, geogCRS_geocentricCRS_same_datum_to_context) {
    auto authFactory =
        AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    auto ctxt = CoordinateOperationContext::create(authFactory, nullptr, 0.0);
    auto list = CoordinateOperationFactory::create()->createOperations(
        // WGS84 geocentric
        authFactory->createCoordinateReferenceSystem("4978"),
        authFactory->createCoordinateReferenceSystem("4326"), ctxt);
    ASSERT_EQ(list.size(), 1U);
    EXPECT_EQ(list[0]->nameStr(),
              "Conversion from WGS 84 (geocentric) to WGS 84 (geog2D)");
    EXPECT_EQ(list[0]->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=pipeline +step +inv +proj=cart +ellps=WGS84 +step "
              "+proj=unitconvert +xy_in=rad +xy_out=deg +step +proj=axisswap "
              "+order=2,1");
}

// ---------------------------------------------------------------------------

TEST(operation,
     geog2D_to_geog3D_same_datum_but_with_potential_other_pivot_context) {
    // Check that when going from geog2D to geog3D of same datum, we don't
    // try to go through a WGS84 pivot...
    auto authFactory =
        AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    auto ctxt = CoordinateOperationContext::create(authFactory, nullptr, 0.0);
    auto list = CoordinateOperationFactory::create()->createOperations(
        authFactory->createCoordinateReferenceSystem("5365"), // CR 05 2D
        authFactory->createCoordinateReferenceSystem("5364"), // CR 05 3D
        ctxt);
    ASSERT_EQ(list.size(), 1U);
    EXPECT_EQ(list[0]->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=noop");
}

// ---------------------------------------------------------------------------

TEST(operation,
     geogCRS_to_geogCRS_different_datum_though_geocentric_transform_context) {
    auto authFactory =
        AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    auto ctxt = CoordinateOperationContext::create(authFactory, nullptr, 0.0);
    auto list = CoordinateOperationFactory::create()->createOperations(
        // ITRF2000 (geog3D)
        authFactory->createCoordinateReferenceSystem("7909"),
        // ITRF2005 (geog3D)
        authFactory->createCoordinateReferenceSystem("7910"), ctxt);
    ASSERT_EQ(list.size(), 1U);
    EXPECT_EQ(list[0]->nameStr(),
              "Conversion from ITRF2000 (geog3D) to ITRF2000 (geocentric) + "
              "ITRF2000 to ITRF2005 (1) + "
              "Conversion from ITRF2005 (geocentric) to ITRF2005 (geog3D)");
    EXPECT_PRED_FORMAT2(
        ComparePROJString,
        list[0]->exportToPROJString(PROJStringFormatter::create().get()),
        "+proj=pipeline +step +proj=axisswap +order=2,1 +step "
        "+proj=unitconvert +xy_in=deg +z_in=m +xy_out=rad +z_out=m "
        "+step +proj=cart +ellps=GRS80 +step +proj=helmert +x=-0.0001 "
        "+y=0.0008 +z=0.0058 +rx=0 +ry=0 +rz=0 +s=-0.0004 +dx=0.0002 "
        "+dy=-0.0001 +dz=0.0018 +drx=0 +dry=0 +drz=0 +ds=-8e-05 "
        "+t_epoch=2000 +convention=position_vector +step +inv "
        "+proj=cart +ellps=GRS80 +step +proj=unitconvert +xy_in=rad "
        "+z_in=m +xy_out=deg +z_out=m +step +proj=axisswap +order=2,1");
}

// ---------------------------------------------------------------------------

TEST(operation, geogCRS_to_geocentricCRS_different_datum_context) {
    auto authFactory =
        AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    auto ctxt = CoordinateOperationContext::create(authFactory, nullptr, 0.0);
    auto list = CoordinateOperationFactory::create()->createOperations(
        // ITRF2000 (geog3D)
        authFactory->createCoordinateReferenceSystem("7909"),
        // ITRF2005 (geocentric)
        authFactory->createCoordinateReferenceSystem("4896"), ctxt);
    ASSERT_EQ(list.size(), 1U);
    EXPECT_EQ(list[0]->nameStr(),
              "Conversion from ITRF2000 (geog3D) to ITRF2000 (geocentric) + "
              "ITRF2000 to ITRF2005 (1)");
    EXPECT_PRED_FORMAT2(
        ComparePROJString,
        list[0]->exportToPROJString(PROJStringFormatter::create().get()),
        "+proj=pipeline +step +proj=axisswap +order=2,1 +step "
        "+proj=unitconvert +xy_in=deg +z_in=m +xy_out=rad +z_out=m "
        "+step +proj=cart +ellps=GRS80 +step +proj=helmert +x=-0.0001 "
        "+y=0.0008 +z=0.0058 +rx=0 +ry=0 +rz=0 +s=-0.0004 +dx=0.0002 "
        "+dy=-0.0001 +dz=0.0018 +drx=0 +dry=0 +drz=0 +ds=-8e-05 "
        "+t_epoch=2000 +convention=position_vector");
}

// ---------------------------------------------------------------------------

TEST(operation, geocentricCRS_to_geogCRS_different_datum_context) {
    auto authFactory =
        AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    auto ctxt = CoordinateOperationContext::create(authFactory, nullptr, 0.0);
    auto list = CoordinateOperationFactory::create()->createOperations(
        // ITRF2000 (geocentric)
        authFactory->createCoordinateReferenceSystem("4919"),
        // ITRF2005 (geog3D)
        authFactory->createCoordinateReferenceSystem("7910"), ctxt);
    ASSERT_EQ(list.size(), 1U);
    EXPECT_EQ(list[0]->nameStr(),
              "ITRF2000 to ITRF2005 (1) + "
              "Conversion from ITRF2005 (geocentric) to ITRF2005 (geog3D)");
    EXPECT_PRED_FORMAT2(
        ComparePROJString,
        list[0]->exportToPROJString(PROJStringFormatter::create().get()),
        "+proj=pipeline +step +proj=helmert +x=-0.0001 "
        "+y=0.0008 +z=0.0058 +rx=0 +ry=0 +rz=0 +s=-0.0004 +dx=0.0002 "
        "+dy=-0.0001 +dz=0.0018 +drx=0 +dry=0 +drz=0 +ds=-8e-05 "
        "+t_epoch=2000 +convention=position_vector +step +inv "
        "+proj=cart +ellps=GRS80 +step +proj=unitconvert +xy_in=rad "
        "+z_in=m +xy_out=deg +z_out=m +step +proj=axisswap +order=2,1");
}

// ---------------------------------------------------------------------------

TEST(operation, geogCRS_3D_to_geogCRS_3D_different_datum_context) {
    // Test for https://github.com/OSGeo/PROJ/issues/2541
    auto dbContext = DatabaseContext::create();
    auto authFactory = AuthorityFactory::create(dbContext, "EPSG");
    auto ctxt = CoordinateOperationContext::create(authFactory, nullptr, 0.0);
    ctxt->setSpatialCriterion(
        CoordinateOperationContext::SpatialCriterion::PARTIAL_INTERSECTION);
    auto list = CoordinateOperationFactory::create()->createOperations(
        // RGF93 (3D)
        authFactory->createCoordinateReferenceSystem("4965"),
        // CH1903+ promoted to 3D
        authFactory->createCoordinateReferenceSystem("4150")->promoteTo3D(
            std::string(), dbContext),
        ctxt);
    ASSERT_GE(list.size(), 1U);
    EXPECT_EQ(list[0]->nameStr(),
              "RGF93 v1 to ETRS89 (1) + Inverse of CH1903+ to ETRS89 (1)");
    // Check that there is no +push +v_3
    EXPECT_EQ(list[0]->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=pipeline "
              "+step +proj=axisswap +order=2,1 "
              "+step +proj=unitconvert +xy_in=deg +z_in=m +xy_out=rad +z_out=m "
              "+step +proj=cart +ellps=GRS80 "
              "+step +proj=helmert +x=-674.374 +y=-15.056 +z=-405.346 "
              "+step +inv +proj=cart +ellps=bessel "
              "+step +proj=unitconvert +xy_in=rad +z_in=m +xy_out=deg +z_out=m "
              "+step +proj=axisswap +order=2,1");
    EXPECT_EQ(list[0]->inverse()->exportToPROJString(
                  PROJStringFormatter::create().get()),
              "+proj=pipeline "
              "+step +proj=axisswap +order=2,1 "
              "+step +proj=unitconvert +xy_in=deg +z_in=m +xy_out=rad +z_out=m "
              "+step +proj=cart +ellps=bessel "
              "+step +proj=helmert +x=674.374 +y=15.056 +z=405.346 "
              "+step +inv +proj=cart +ellps=GRS80 "
              "+step +proj=unitconvert +xy_in=rad +z_in=m +xy_out=deg +z_out=m "
              "+step +proj=axisswap +order=2,1");
}

// ---------------------------------------------------------------------------

TEST(operation, geocentric_to_geogCRS_3D_different_datum_context) {
    // Test variant of https://github.com/OSGeo/PROJ/issues/2541
    auto dbContext = DatabaseContext::create();
    auto authFactory = AuthorityFactory::create(dbContext, "EPSG");
    auto ctxt = CoordinateOperationContext::create(authFactory, nullptr, 0.0);
    ctxt->setSpatialCriterion(
        CoordinateOperationContext::SpatialCriterion::PARTIAL_INTERSECTION);
    auto list = CoordinateOperationFactory::create()->createOperations(
        // RGF93 (geocentric)
        authFactory->createCoordinateReferenceSystem("4964"),
        // CH1903+ promoted to 3D
        authFactory->createCoordinateReferenceSystem("4150")->promoteTo3D(
            std::string(), dbContext),
        ctxt);
    ASSERT_GE(list.size(), 1U);
    EXPECT_EQ(list[0]->nameStr(),
              "Conversion from RGF93 v1 (geocentric) to RGF93 v1 (geog3D) + "
              "RGF93 v1 to ETRS89 (1) + "
              "Inverse of CH1903+ to ETRS89 (1)");
    // Check that there is no +push +v_3
    EXPECT_EQ(list[0]->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=pipeline "
              "+step +proj=helmert +x=-674.374 +y=-15.056 +z=-405.346 "
              "+step +inv +proj=cart +ellps=bessel "
              "+step +proj=unitconvert +xy_in=rad +z_in=m +xy_out=deg +z_out=m "
              "+step +proj=axisswap +order=2,1");
    EXPECT_EQ(list[0]->inverse()->exportToPROJString(
                  PROJStringFormatter::create().get()),
              "+proj=pipeline "
              "+step +proj=axisswap +order=2,1 "
              "+step +proj=unitconvert +xy_in=deg +z_in=m +xy_out=rad +z_out=m "
              "+step +proj=cart +ellps=bessel "
              "+step +proj=helmert +x=674.374 +y=15.056 +z=405.346");
}

// ---------------------------------------------------------------------------

TEST(operation, createBetweenGeodeticCRSWithDatumBasedIntermediates) {
    auto dbContext = DatabaseContext::create();
    auto authFactoryEPSG = AuthorityFactory::create(dbContext, "EPSG");
    auto ctxt =
        CoordinateOperationContext::create(authFactoryEPSG, nullptr, 0.0);
    auto list = CoordinateOperationFactory::create()->createOperations(
        // IG05/12 Intermediate CRS
        authFactoryEPSG->createCoordinateReferenceSystem("6990"),
        // ITRF2014
        authFactoryEPSG->createCoordinateReferenceSystem("9000"), ctxt);
    ASSERT_EQ(list.size(), 1U);
    EXPECT_EQ(list[0]->nameStr(),
              "Inverse of ITRF2008 to IG05/12 Intermediate CRS + "
              "Conversion from ITRF2008 (geog2D) to ITRF2008 (geocentric) + "
              "ITRF2008 to ITRF2014 (1) + "
              "Conversion from ITRF2014 (geocentric) to ITRF2014 (geog2D)");

    auto listInv = CoordinateOperationFactory::create()->createOperations(
        // ITRF2014
        authFactoryEPSG->createCoordinateReferenceSystem("9000"),
        // IG05/12 Intermediate CRS
        authFactoryEPSG->createCoordinateReferenceSystem("6990"), ctxt);
    ASSERT_EQ(listInv.size(), 1U);
    EXPECT_EQ(listInv[0]->nameStr(),
              "Conversion from ITRF2014 (geog2D) to ITRF2014 (geocentric) + "
              "Inverse of ITRF2008 to ITRF2014 (1) + "
              "Conversion from ITRF2008 (geocentric) to ITRF2008 (geog2D) + "
              "ITRF2008 to IG05/12 Intermediate CRS");
}

// ---------------------------------------------------------------------------
TEST(operation, esri_projectedCRS_to_geogCRS_with_ITRF_intermediate_context) {
    auto dbContext = DatabaseContext::create();
    auto authFactoryEPSG = AuthorityFactory::create(dbContext, "EPSG");
    auto authFactoryESRI = AuthorityFactory::create(dbContext, "ESRI");
    auto ctxt =
        CoordinateOperationContext::create(authFactoryEPSG, nullptr, 0.0);
    auto list = CoordinateOperationFactory::create()->createOperations(
        // NAD_1983_CORS96_StatePlane_North_Carolina_FIPS_3200_Ft_US (projected)
        authFactoryESRI->createCoordinateReferenceSystem("103501"),
        // ITRF2005 (geog3D)
        authFactoryEPSG->createCoordinateReferenceSystem("7910"), ctxt);
    ASSERT_EQ(list.size(), 1U);
    EXPECT_EQ(list[0]->nameStr(),
              "Inverse of NAD_1983_CORS96_StatePlane_North_Carolina_"
              "FIPS_3200_Ft_US + "
              "Conversion from NAD83(CORS96) (geog2D) to NAD83(CORS96) "
              "(geocentric) + Inverse of ITRF2000 to NAD83(CORS96) (1) + "
              "ITRF2000 to ITRF2005 (1) + "
              "Conversion from ITRF2005 (geocentric) to ITRF2005 (geog3D)");
    EXPECT_PRED_FORMAT2(
        ComparePROJString,
        list[0]->exportToPROJString(PROJStringFormatter::create().get()),
        "+proj=pipeline +step +proj=unitconvert +xy_in=us-ft "
        "+xy_out=m +step +inv +proj=lcc +lat_0=33.75 +lon_0=-79 "
        "+lat_1=34.3333333333333 +lat_2=36.1666666666667 "
        "+x_0=609601.219202438 +y_0=0 +ellps=GRS80 +step +proj=cart "
        "+ellps=GRS80 +step +inv +proj=helmert +x=0.9956 +y=-1.9013 "
        "+z=-0.5215 +rx=0.025915 +ry=0.009426 +rz=0.011599 +s=0.00062 "
        "+dx=0.0007 +dy=-0.0007 +dz=0.0005 +drx=6.7e-05 +dry=-0.000757 "
        "+drz=-5.1e-05 +ds=-0.00018 +t_epoch=1997 "
        "+convention=coordinate_frame +step +proj=helmert +x=-0.0001 "
        "+y=0.0008 +z=0.0058 +rx=0 +ry=0 +rz=0 +s=-0.0004 +dx=0.0002 "
        "+dy=-0.0001 +dz=0.0018 +drx=0 +dry=0 +drz=0 +ds=-8e-05 "
        "+t_epoch=2000 +convention=position_vector +step +inv +proj=cart "
        "+ellps=GRS80 +step +proj=unitconvert +xy_in=rad +z_in=m "
        "+xy_out=deg +z_out=m +step +proj=axisswap +order=2,1");
}

// ---------------------------------------------------------------------------

TEST(operation, geogCRS_to_geogCRS_WGS84_to_GDA2020) {
    // 2D reduction of use case of https://github.com/OSGeo/PROJ/issues/2348
    auto dbContext = DatabaseContext::create();
    auto authFactory = AuthorityFactory::create(dbContext, "EPSG");
    auto ctxt = CoordinateOperationContext::create(authFactory, nullptr, 0.0);
    ctxt->setSpatialCriterion(
        CoordinateOperationContext::SpatialCriterion::PARTIAL_INTERSECTION);
    ctxt->setGridAvailabilityUse(
        CoordinateOperationContext::GridAvailabilityUse::
            IGNORE_GRID_AVAILABILITY);
    {
        auto list = CoordinateOperationFactory::create()->createOperations(
            // GDA2020
            authFactory->createCoordinateReferenceSystem("7844"),
            // WGS 84
            authFactory->createCoordinateReferenceSystem("4326"), ctxt);
        ASSERT_GE(list.size(), 1U);
        EXPECT_EQ(list[0]->nameStr(), "GDA2020 to WGS 84 (2)");
    }

    // Inverse
    {
        auto list = CoordinateOperationFactory::create()->createOperations(
            // WGS 84
            authFactory->createCoordinateReferenceSystem("4326"),
            // GDA2020
            authFactory->createCoordinateReferenceSystem("7844"), ctxt);
        ASSERT_GE(list.size(), 1U);
        EXPECT_EQ(list[0]->nameStr(), "Inverse of GDA2020 to WGS 84 (2)");
    }
}

// ---------------------------------------------------------------------------

TEST(operation, geogCRS_to_geogCRS_with_intermediate_no_ids) {

    auto dbContext = DatabaseContext::create();
    auto authFactory = AuthorityFactory::create(dbContext, std::string());
    auto ctxt = CoordinateOperationContext::create(authFactory, nullptr, 0.0);
    ctxt->setSpatialCriterion(
        CoordinateOperationContext::SpatialCriterion::PARTIAL_INTERSECTION);
    ctxt->setGridAvailabilityUse(
        CoordinateOperationContext::GridAvailabilityUse::
            IGNORE_GRID_AVAILABILITY);

    auto objSrc = WKTParser().createFromWKT(
        "GEOGCRS[\"input\",\n"
        "    DATUM[\"International Terrestrial Reference Frame 2014\",\n"
        "        ELLIPSOID[\"GRS 1980\",6378137,298.257222101,\n"
        "            LENGTHUNIT[\"metre\",1]],\n"
        "        ID[\"EPSG\",1165]],\n"
        "    PRIMEM[\"Greenwich\",0,\n"
        "        ANGLEUNIT[\"degree\",0.0174532925199433],\n"
        "        ID[\"EPSG\",8901]],\n"
        "    CS[ellipsoidal,3],\n"
        "        AXIS[\"longitude\",east,\n"
        "            ORDER[1],\n"
        "            ANGLEUNIT[\"degree\",0.0174532925199433,\n"
        "                ID[\"EPSG\",9122]]],\n"
        "        AXIS[\"latitude\",north,\n"
        "            ORDER[2],\n"
        "            ANGLEUNIT[\"degree\",0.0174532925199433,\n"
        "                ID[\"EPSG\",9122]]],\n"
        "        AXIS[\"ellipsoidal height (h)\",up,\n"
        "            ORDER[3],\n"
        "            LENGTHUNIT[\"metre\",1]]]");
    auto src = nn_dynamic_pointer_cast<CRS>(objSrc);
    ASSERT_TRUE(src != nullptr);

    auto objDest = WKTParser().createFromWKT(
        "GEOGCRS[\"output\",\n"
        "    DATUM[\"Estonia 1997\",\n"
        "        ELLIPSOID[\"GRS 1980\",6378137,298.257222101,\n"
        "            LENGTHUNIT[\"metre\",1]],\n"
        "        ID[\"EPSG\",6180]],\n"
        "    PRIMEM[\"Greenwich\",0,\n"
        "        ANGLEUNIT[\"degree\",0.0174532925199433],\n"
        "        ID[\"EPSG\",8901]],\n"
        "    CS[ellipsoidal,3],\n"
        "        AXIS[\"longitude\",east,\n"
        "            ORDER[1],\n"
        "            ANGLEUNIT[\"degree\",0.0174532925199433,\n"
        "                ID[\"EPSG\",9122]]],\n"
        "        AXIS[\"latitude\",north,\n"
        "            ORDER[2],\n"
        "            ANGLEUNIT[\"degree\",0.0174532925199433,\n"
        "                ID[\"EPSG\",9122]]],\n"
        "        AXIS[\"ellipsoidal height (h)\",up,\n"
        "            ORDER[3],\n"
        "            LENGTHUNIT[\"metre\",1]]]");
    auto dest = nn_dynamic_pointer_cast<CRS>(objDest);
    ASSERT_TRUE(dest != nullptr);

    {
        auto list = CoordinateOperationFactory::create()->createOperations(
            NN_NO_CHECK(src), NN_NO_CHECK(dest), ctxt);
        ASSERT_GE(list.size(), 1U);
        // Test that a non-noop operation is returned
        EXPECT_EQ(
            list[0]->nameStr(),
            "axis order change (geographic3D horizontal) + "
            "Conversion from ITRF2014 (geog3D) to ITRF2014 (geocentric) + "
            "ITRF2014 to ETRF2014 (1) + "
            "Inverse of NKG_ETRF14 to ETRF2014 + "
            "NKG_ETRF14 to ETRF96@2000.0 + "
            "ETRF96@2000.0 to ETRF96@1997.56 + "
            "Conversion from ETRS89 (geocentric) to ETRS89 (geog2D) + "
            "Inverse of EST97 to ETRS89 (1) + "
            "Null geographic offset from EST97 (geog2D) to EST97 (geog3D) + "
            "axis order change (geographic3D horizontal)");
    }
}

// ---------------------------------------------------------------------------

static ProjectedCRSNNPtr createUTM31_WGS84() {
    return ProjectedCRS::create(
        PropertyMap(), GeographicCRS::EPSG_4326,
        Conversion::createUTM(PropertyMap(), 31, true),
        CartesianCS::createEastingNorthing(UnitOfMeasure::METRE));
}

// ---------------------------------------------------------------------------

static ProjectedCRSNNPtr createUTM32_WGS84() {
    return ProjectedCRS::create(
        PropertyMap(), GeographicCRS::EPSG_4326,
        Conversion::createUTM(PropertyMap(), 32, true),
        CartesianCS::createEastingNorthing(UnitOfMeasure::METRE));
}

// ---------------------------------------------------------------------------

TEST(operation, geogCRS_to_projCRS) {

    auto op = CoordinateOperationFactory::create()->createOperation(
        GeographicCRS::EPSG_4326, createUTM31_WGS84());
    ASSERT_TRUE(op != nullptr);
    EXPECT_TRUE(std::dynamic_pointer_cast<Conversion>(op) != nullptr);
    EXPECT_EQ(op->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=pipeline +step +proj=axisswap +order=2,1 +step "
              "+proj=unitconvert +xy_in=deg +xy_out=rad +step +proj=utm "
              "+zone=31 +ellps=WGS84");
}

// ---------------------------------------------------------------------------

TEST(operation, geogCRS_longlat_to_geogCS_latlong) {

    auto sourceCRS = GeographicCRS::OGC_CRS84;
    auto targetCRS = GeographicCRS::EPSG_4326;
    auto op = CoordinateOperationFactory::create()->createOperation(sourceCRS,
                                                                    targetCRS);
    ASSERT_TRUE(op != nullptr);
    auto conv = std::dynamic_pointer_cast<Conversion>(op);
    ASSERT_TRUE(conv != nullptr);
    EXPECT_TRUE(op->sourceCRS() &&
                op->sourceCRS()->isEquivalentTo(sourceCRS.get()));
    EXPECT_TRUE(op->targetCRS() &&
                op->targetCRS()->isEquivalentTo(targetCRS.get()));
    EXPECT_EQ(op->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=axisswap +order=2,1");
    auto convInverse = nn_dynamic_pointer_cast<Conversion>(conv->inverse());
    ASSERT_TRUE(convInverse != nullptr);
    EXPECT_TRUE(convInverse->sourceCRS() &&
                convInverse->sourceCRS()->isEquivalentTo(targetCRS.get()));
    EXPECT_TRUE(convInverse->targetCRS() &&
                convInverse->targetCRS()->isEquivalentTo(sourceCRS.get()));
    EXPECT_EQ(conv->method()->exportToWKT(WKTFormatter::create().get()),
              convInverse->method()->exportToWKT(WKTFormatter::create().get()));
    EXPECT_TRUE(conv->method()->isEquivalentTo(convInverse->method().get()));
}

// ---------------------------------------------------------------------------

TEST(operation, geogCRS_longlat_to_geogCS_latlong_database) {

    auto authFactory =
        AuthorityFactory::create(DatabaseContext::create(), std::string());
    auto ctxt = CoordinateOperationContext::create(authFactory, nullptr, 0.0);
    auto list = CoordinateOperationFactory::create()->createOperations(
        AuthorityFactory::create(DatabaseContext::create(), "OGC")
            ->createCoordinateReferenceSystem("CRS84"),
        AuthorityFactory::create(DatabaseContext::create(), "EPSG")
            ->createCoordinateReferenceSystem("4326"),
        ctxt);
    ASSERT_EQ(list.size(), 1U);
    EXPECT_EQ(list[0]->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=axisswap +order=2,1");
}

// ---------------------------------------------------------------------------

TEST(operation, geogCRS_longlat_to_projCRS) {

    auto op = CoordinateOperationFactory::create()->createOperation(
        GeographicCRS::OGC_CRS84, createUTM31_WGS84());
    ASSERT_TRUE(op != nullptr);
    EXPECT_EQ(op->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=pipeline +step +proj=unitconvert +xy_in=deg +xy_out=rad "
              "+step +proj=utm +zone=31 +ellps=WGS84");
}

// ---------------------------------------------------------------------------

TEST(operation, geogCRS_different_from_baseCRS_to_projCRS) {

    auto op = CoordinateOperationFactory::create()->createOperation(
        GeographicCRS::EPSG_4807, createUTM31_WGS84());
    ASSERT_TRUE(op != nullptr);
    EXPECT_EQ(
        op->exportToPROJString(PROJStringFormatter::create().get()),
        "+proj=pipeline +step +proj=axisswap +order=2,1 +step "
        "+proj=unitconvert +xy_in=grad +xy_out=rad +step +inv +proj=longlat "
        "+ellps=clrk80ign +pm=paris +step +proj=utm +zone=31 "
        "+ellps=WGS84");
}

// ---------------------------------------------------------------------------

TEST(operation,
     geogCRS_different_from_baseCRS_to_projCRS_context_compatible_area) {
    auto authFactory =
        AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    auto ctxt = CoordinateOperationContext::create(authFactory, nullptr, 0.0);
    ctxt->setGridAvailabilityUse(
        CoordinateOperationContext::GridAvailabilityUse::
            IGNORE_GRID_AVAILABILITY);
    ctxt->setAllowUseIntermediateCRS(
        CoordinateOperationContext::IntermediateCRSUse::ALWAYS);
    auto list = CoordinateOperationFactory::create()->createOperations(
        authFactory->createCoordinateReferenceSystem("4807"),  // NTF(Paris)
        authFactory->createCoordinateReferenceSystem("32631"), // UTM31 WGS84
        ctxt);
    ASSERT_EQ(list.size(), 4U);
    EXPECT_EQ(
        list[0]->nameStr(),
        "NTF (Paris) to NTF (1) + Inverse of WGS 84 to NTF (3) + UTM zone 31N");
    ASSERT_EQ(list[0]->coordinateOperationAccuracies().size(), 1U);
    EXPECT_EQ(list[0]->coordinateOperationAccuracies()[0]->value(), "1");
    EXPECT_EQ(
        list[0]->exportToPROJString(PROJStringFormatter::create().get()),
        "+proj=pipeline +step +proj=axisswap +order=2,1 +step "
        "+proj=unitconvert +xy_in=grad +xy_out=rad +step +inv "
        "+proj=longlat +ellps=clrk80ign +pm=paris +step +proj=hgridshift "
        "+grids=fr_ign_ntf_r93.tif +step +proj=utm +zone=31 +ellps=WGS84");
}

// ---------------------------------------------------------------------------

TEST(operation, geocentricCRS_to_projCRS) {

    auto op = CoordinateOperationFactory::create()->createOperation(
        createGeocentricDatumWGS84(), createUTM31_WGS84());
    ASSERT_TRUE(op != nullptr);
    EXPECT_EQ(op->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=pipeline +step +inv +proj=cart +ellps=WGS84 +step "
              "+proj=utm +zone=31 +ellps=WGS84");
}

// ---------------------------------------------------------------------------

TEST(operation, projCRS_to_geogCRS) {

    auto op = CoordinateOperationFactory::create()->createOperation(
        createUTM31_WGS84(), GeographicCRS::EPSG_4326);
    ASSERT_TRUE(op != nullptr);
    EXPECT_EQ(op->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=pipeline +step +inv +proj=utm +zone=31 +ellps=WGS84 +step "
              "+proj=unitconvert +xy_in=rad +xy_out=deg +step +proj=axisswap "
              "+order=2,1");
}

// ---------------------------------------------------------------------------

TEST(operation, projCRS_no_id_to_geogCRS_context) {
    auto authFactory =
        AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    auto ctxt = CoordinateOperationContext::create(authFactory, nullptr, 0.0);
    auto src = authFactory->createCoordinateReferenceSystem(
        "28992"); // Amersfoort / RD New
    auto dst =
        authFactory->createCoordinateReferenceSystem("4258"); // ETRS89 2D
    auto list =
        CoordinateOperationFactory::create()->createOperations(src, dst, ctxt);
    ASSERT_GE(list.size(), 1U);
    auto wkt2 =
        "PROJCRS[\"unknown\",\n"
        "    BASEGEOGCRS[\"Amersfoort\",\n"
        "       DATUM[\"Amersfoort\",\n"
        "            ELLIPSOID[\"Bessel 1841\",6377397.155,299.1528128]]],\n"
        "    CONVERSION[\"unknown\",\n"
        "        METHOD[\"Oblique Stereographic\"],\n"
        "        PARAMETER[\"Latitude of natural origin\",52.1561605555556],\n"
        "        PARAMETER[\"Longitude of natural origin\",5.38763888888889],\n"
        "        PARAMETER[\"Scale factor at natural origin\",0.9999079],\n"
        "        PARAMETER[\"False easting\",155000],\n"
        "        PARAMETER[\"False northing\",463000]],\n"
        "    CS[Cartesian,2],\n"
        "        AXIS[\"(E)\",east],\n"
        "        AXIS[\"(N)\",north],\n"
        "    LENGTHUNIT[\"metre\",1],\n"
        "    ID[\"EPSG\",28992]]";
    auto obj = WKTParser().createFromWKT(wkt2);
    auto src_from_wkt2 = nn_dynamic_pointer_cast<CRS>(obj);
    ASSERT_TRUE(src_from_wkt2 != nullptr);
    auto list2 = CoordinateOperationFactory::create()->createOperations(
        NN_NO_CHECK(src_from_wkt2), dst, ctxt);
    ASSERT_GE(list.size(), list2.size() - 1);
    for (size_t i = 0; i < list.size(); i++) {
        const auto &op = list[i];
        const auto &op2 = list2[i];
        EXPECT_TRUE(
            op->isEquivalentTo(op2.get(), IComparable::Criterion::EQUIVALENT));
    }
}

// ---------------------------------------------------------------------------

TEST(operation, projCRS_3D_to_geogCRS_3D_context) {
    auto authFactory =
        AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    auto ctxt = CoordinateOperationContext::create(authFactory, nullptr, 0.0);
    ctxt->setSpatialCriterion(
        CoordinateOperationContext::SpatialCriterion::PARTIAL_INTERSECTION);
    auto wkt = "PROJCRS[\"NAD83(HARN) / Oregon GIC Lambert (ft)\",\n"
               "    BASEGEOGCRS[\"NAD83(HARN)\",\n"
               "        DATUM[\"NAD83 (High Accuracy Reference Network)\",\n"
               "            ELLIPSOID[\"GRS 1980\",6378137,298.257222101,\n"
               "                LENGTHUNIT[\"metre\",1]]],\n"
               "        PRIMEM[\"Greenwich\",0,\n"
               "            ANGLEUNIT[\"degree\",0.0174532925199433]],\n"
               "        ID[\"EPSG\",4957]],\n"
               "    CONVERSION[\"unnamed\",\n"
               "        METHOD[\"Lambert Conic Conformal (2SP)\",\n"
               "            ID[\"EPSG\",9802]],\n"
               "        PARAMETER[\"Latitude of false origin\",41.75,\n"
               "            ANGLEUNIT[\"degree\",0.0174532925199433],\n"
               "            ID[\"EPSG\",8821]],\n"
               "        PARAMETER[\"Longitude of false origin\",-120.5,\n"
               "            ANGLEUNIT[\"degree\",0.0174532925199433],\n"
               "            ID[\"EPSG\",8822]],\n"
               "        PARAMETER[\"Latitude of 1st standard parallel\",43,\n"
               "            ANGLEUNIT[\"degree\",0.0174532925199433],\n"
               "            ID[\"EPSG\",8823]],\n"
               "        PARAMETER[\"Latitude of 2nd standard parallel\",45.5,\n"
               "            ANGLEUNIT[\"degree\",0.0174532925199433],\n"
               "            ID[\"EPSG\",8824]],\n"
               "        PARAMETER[\"Easting at false origin\",1312335.958,\n"
               "            LENGTHUNIT[\"foot\",0.3048],\n"
               "            ID[\"EPSG\",8826]],\n"
               "        PARAMETER[\"Northing at false origin\",0,\n"
               "            LENGTHUNIT[\"foot\",0.3048],\n"
               "            ID[\"EPSG\",8827]]],\n"
               "    CS[Cartesian,3],\n"
               "        AXIS[\"easting\",east,\n"
               "            ORDER[1],\n"
               "            LENGTHUNIT[\"foot\",0.3048]],\n"
               "        AXIS[\"northing\",north,\n"
               "            ORDER[2],\n"
               "            LENGTHUNIT[\"foot\",0.3048]],\n"
               "        AXIS[\"ellipsoidal height (h)\",up,\n"
               "            ORDER[3],\n"
               "            LENGTHUNIT[\"foot\",0.3048]]]";
    auto obj = WKTParser().createFromWKT(wkt);
    auto src = NN_CHECK_ASSERT(nn_dynamic_pointer_cast<CRS>(obj));
    auto dst = authFactory->createCoordinateReferenceSystem(
        "4957"); // NAD83(HARN) (3D)
    auto list =
        CoordinateOperationFactory::create()->createOperations(src, dst, ctxt);
    ASSERT_EQ(list.size(), 1U);
    EXPECT_EQ(list[0]->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=pipeline "
              // Check that z ft->m conversion is done (and just once)
              "+step +proj=unitconvert +xy_in=ft +z_in=ft +xy_out=m +z_out=m "
              "+step +inv +proj=lcc +lat_0=41.75 +lon_0=-120.5 +lat_1=43 "
              "+lat_2=45.5 +x_0=399999.9999984 +y_0=0 +ellps=GRS80 "
              "+step +proj=unitconvert +xy_in=rad +z_in=m +xy_out=deg +z_out=m "
              "+step +proj=axisswap +order=2,1");
}
// ---------------------------------------------------------------------------

TEST(operation, projCRS_3D_to_projCRS_2D_context) {
    auto authFactory =
        AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    auto ctxt = CoordinateOperationContext::create(authFactory, nullptr, 0.0);
    ctxt->setSpatialCriterion(
        CoordinateOperationContext::SpatialCriterion::PARTIAL_INTERSECTION);
    auto wkt =
        "PROJCRS[\"Projected 3d CRS\",\n"
        "    BASEGEOGCRS[\"JGD2000\",\n"
        "        DATUM[\"Japanese Geodetic Datum 2000\",\n"
        "            ELLIPSOID[\"GRS 1980\",6378137,298.257222101,\n"
        "                LENGTHUNIT[\"metre\",1]]],\n"
        "        PRIMEM[\"Greenwich\",0,\n"
        "            ANGLEUNIT[\"degree\",0.0174532925199433]],\n"
        "        ID[\"EPSG\",4947]],\n" // the code is what triggered the bug
        "    CONVERSION[\"Japan Plane Rectangular CS zone VII\",\n"
        "        METHOD[\"Transverse Mercator\",\n"
        "            ID[\"EPSG\",9807]],\n"
        "        PARAMETER[\"Latitude of natural origin\",36,\n"
        "            ANGLEUNIT[\"degree\",0.0174532925199433],\n"
        "            ID[\"EPSG\",8801]],\n"
        "        PARAMETER[\"Longitude of natural origin\",137.166666666667,\n"
        "            ANGLEUNIT[\"degree\",0.0174532925199433],\n"
        "            ID[\"EPSG\",8802]],\n"
        "        PARAMETER[\"Scale factor at natural origin\",0.9999,\n"
        "            SCALEUNIT[\"unity\",1],\n"
        "            ID[\"EPSG\",8805]],\n"
        "        PARAMETER[\"False easting\",0,\n"
        "            LENGTHUNIT[\"metre\",1],\n"
        "            ID[\"EPSG\",8806]],\n"
        "        PARAMETER[\"False northing\",0,\n"
        "            LENGTHUNIT[\"metre\",1],\n"
        "            ID[\"EPSG\",8807]],\n"
        "        ID[\"EPSG\",17807]],\n"
        "    CS[Cartesian,3],\n"
        "        AXIS[\"northing (X)\",north,\n"
        "            ORDER[1],\n"
        "            LENGTHUNIT[\"metre\",1,\n"
        "                ID[\"EPSG\",9001]]],\n"
        "        AXIS[\"easting (Y)\",east,\n"
        "            ORDER[2],\n"
        "            LENGTHUNIT[\"metre\",1,\n"
        "                ID[\"EPSG\",9001]]],\n"
        "        AXIS[\"ellipsoidal height (h)\",up,\n"
        "            ORDER[3],\n"
        "            LENGTHUNIT[\"metre\",1,\n"
        "                ID[\"EPSG\",9001]]]]";
    auto obj = WKTParser().createFromWKT(wkt);
    auto src = NN_CHECK_ASSERT(nn_dynamic_pointer_cast<CRS>(obj));
    auto dst =
        authFactory->createCoordinateReferenceSystem("32653"); // WGS 84 UTM 53
    // We just want to check that we don't get inconsistent chaining exception
    auto list =
        CoordinateOperationFactory::create()->createOperations(src, dst, ctxt);
    ASSERT_GE(list.size(), 1U);
}

// ---------------------------------------------------------------------------

TEST(operation, geogCRS_3D_to_projCRS_with_2D_geocentric_translation) {

    auto authFactory =
        AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    auto ctxt = CoordinateOperationContext::create(authFactory, nullptr, 0.0);
    auto src =
        authFactory->createCoordinateReferenceSystem("4979"); // WGS 84 3D

    // Azores Central 1948 / UTM zone 26N
    auto dst = authFactory->createCoordinateReferenceSystem("2189");

    auto list =
        CoordinateOperationFactory::create()->createOperations(src, dst, ctxt);
    ASSERT_GE(list.size(), 1U);
    EXPECT_EQ(list[0]->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=pipeline "
              "+step +proj=axisswap +order=2,1 "
              "+step +proj=unitconvert +xy_in=deg +z_in=m +xy_out=rad +z_out=m "
              "+step +proj=push +v_3 " // this is what we check. Due to the
                                       // target system being 2D only
              "+step +proj=cart +ellps=WGS84 "
              "+step +proj=helmert +x=104 +y=-167 +z=38 "
              "+step +inv +proj=cart +ellps=intl "
              "+step +proj=pop +v_3 " // this is what we check
              "+step +proj=utm +zone=26 +ellps=intl");

    auto listReverse =
        CoordinateOperationFactory::create()->createOperations(dst, src, ctxt);
    ASSERT_GE(listReverse.size(), 1U);
    EXPECT_EQ(
        listReverse[0]->exportToPROJString(PROJStringFormatter::create().get()),
        "+proj=pipeline "
        "+step +inv +proj=utm +zone=26 +ellps=intl "
        "+step +proj=push +v_3 " // this is what we check
        "+step +proj=cart +ellps=intl "
        "+step +proj=helmert +x=-104 +y=167 +z=-38 "
        "+step +inv +proj=cart +ellps=WGS84 "
        "+step +proj=pop +v_3 " // this is what we check
        "+step +proj=unitconvert +xy_in=rad +z_in=m +xy_out=deg +z_out=m "
        "+step +proj=axisswap +order=2,1");
}

// ---------------------------------------------------------------------------

TEST(operation, projCRS_to_projCRS) {

    auto op = CoordinateOperationFactory::create()->createOperation(
        createUTM31_WGS84(), createUTM32_WGS84());
    ASSERT_TRUE(op != nullptr);
    EXPECT_EQ(op->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=pipeline +step +inv +proj=utm +zone=31 +ellps=WGS84 +step "
              "+proj=utm +zone=32 +ellps=WGS84");
}

// ---------------------------------------------------------------------------

TEST(operation, projCRS_to_projCRS_different_baseCRS) {

    auto utm32 = ProjectedCRS::create(
        PropertyMap(), GeographicCRS::EPSG_4807,
        Conversion::createUTM(PropertyMap(), 32, true),
        CartesianCS::createEastingNorthing(UnitOfMeasure::METRE));

    auto op = CoordinateOperationFactory::create()->createOperation(
        createUTM31_WGS84(), utm32);
    ASSERT_TRUE(op != nullptr);
    EXPECT_EQ(op->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=pipeline +step +inv +proj=utm +zone=31 +ellps=WGS84 +step "
              "+proj=utm +zone=32 +ellps=clrk80ign +pm=paris");
}

// ---------------------------------------------------------------------------

TEST(operation, projCRS_to_projCRS_context_compatible_area) {
    auto authFactory =
        AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    auto ctxt = CoordinateOperationContext::create(authFactory, nullptr, 0.0);
    auto list = CoordinateOperationFactory::create()->createOperations(
        authFactory->createCoordinateReferenceSystem("32634"), // UTM 34
        authFactory->createCoordinateReferenceSystem(
            "2171"), // Pulkovo 42 Poland I
        ctxt);
    ASSERT_EQ(list.size(), 2U);
    EXPECT_EQ(list[0]->nameStr(),
              "Inverse of UTM zone 34N + Inverse of Pulkovo 1942(58) to WGS 84 "
              "(1) + Poland zone I");
    ASSERT_EQ(list[0]->coordinateOperationAccuracies().size(), 1U);
    EXPECT_EQ(list[0]->coordinateOperationAccuracies()[0]->value(), "1");
}

// ---------------------------------------------------------------------------

TEST(operation, projCRS_to_projCRS_context_compatible_area_bis) {
    auto authFactory =
        AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    auto ctxt = CoordinateOperationContext::create(authFactory, nullptr, 0.0);
    auto list = CoordinateOperationFactory::create()->createOperations(
        authFactory->createCoordinateReferenceSystem(
            "3844"), // Pulkovo 42 Stereo 70 (Romania)
        authFactory->createCoordinateReferenceSystem("32634"), // UTM 34
        ctxt);
    ASSERT_EQ(list.size(), 3U);
    EXPECT_EQ(list[0]->nameStr(), "Inverse of Stereo 70 + "
                                  "Pulkovo 1942(58) to WGS 84 "
                                  "(19) + UTM zone 34N");
    ASSERT_EQ(list[0]->coordinateOperationAccuracies().size(), 1U);
    EXPECT_EQ(list[0]->coordinateOperationAccuracies()[0]->value(), "3");
}

// ---------------------------------------------------------------------------

TEST(operation, projCRS_to_projCRS_context_one_incompatible_area) {
    auto authFactory =
        AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    auto ctxt = CoordinateOperationContext::create(authFactory, nullptr, 0.0);
    auto list = CoordinateOperationFactory::create()->createOperations(
        authFactory->createCoordinateReferenceSystem("32631"), // UTM 31
        authFactory->createCoordinateReferenceSystem(
            "2171"), // Pulkovo 42 Poland I
        ctxt);
    ASSERT_EQ(list.size(), 2U);
    EXPECT_EQ(list[0]->nameStr(),
              "Inverse of UTM zone 31N + Inverse of Pulkovo 1942(58) to WGS 84 "
              "(1) + Poland zone I");
    ASSERT_EQ(list[0]->coordinateOperationAccuracies().size(), 1U);
    EXPECT_EQ(list[0]->coordinateOperationAccuracies()[0]->value(), "1");
}

// ---------------------------------------------------------------------------

TEST(operation, projCRS_to_projCRS_context_incompatible_areas) {
    auto authFactory =
        AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    auto ctxt = CoordinateOperationContext::create(authFactory, nullptr, 0.0);
    auto list = CoordinateOperationFactory::create()->createOperations(
        authFactory->createCoordinateReferenceSystem("32631"), // UTM 31
        authFactory->createCoordinateReferenceSystem("32633"), // UTM 33
        ctxt);
    ASSERT_EQ(list.size(), 1U);
    EXPECT_EQ(list[0]->nameStr(), "Inverse of UTM zone 31N + UTM zone 33N");
    ASSERT_EQ(list[0]->coordinateOperationAccuracies().size(), 1U);
    EXPECT_EQ(list[0]->coordinateOperationAccuracies()[0]->value(), "0");
}

// ---------------------------------------------------------------------------

TEST(operation, projCRS_to_projCRS_context_incompatible_areas_ballpark) {
    auto authFactory =
        AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    auto ctxt = CoordinateOperationContext::create(authFactory, nullptr, 0.0);
    auto list = CoordinateOperationFactory::create()->createOperations(
        authFactory->createCoordinateReferenceSystem("26711"), // UTM 11 NAD27
        authFactory->createCoordinateReferenceSystem(
            "3034"), // ETRS89 / LCC Europe
        ctxt);
    ASSERT_GE(list.size(), 1U);
    EXPECT_TRUE(list[0]->hasBallparkTransformation());
}

// ---------------------------------------------------------------------------

TEST(
    operation,
    projCRS_to_projCRS_context_incompatible_areas_crs_extent_use_intersection) {
    auto authFactory =
        AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    auto ctxt = CoordinateOperationContext::create(authFactory, nullptr, 0.0);
    ctxt->setSourceAndTargetCRSExtentUse(
        CoordinateOperationContext::SourceTargetCRSExtentUse::INTERSECTION);
    auto list = CoordinateOperationFactory::create()->createOperations(
        authFactory->createCoordinateReferenceSystem("26711"), // UTM 11 NAD27
        authFactory->createCoordinateReferenceSystem(
            "3034"), // ETRS89 / LCC Europe
        ctxt);
    ASSERT_GE(list.size(), 0U);
}

// ---------------------------------------------------------------------------

TEST(operation, projCRS_to_geogCRS_crs_extent_use_none) {
    auto authFactory =
        AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    {
        auto ctxt =
            CoordinateOperationContext::create(authFactory, nullptr, 0.0);
        auto list = CoordinateOperationFactory::create()->createOperations(
            authFactory->createCoordinateReferenceSystem("23031"), // ED50 UTM31
            authFactory->createCoordinateReferenceSystem("4326"), ctxt);
        bool found_EPSG_15964 = false;
        for (const auto &op : list) {
            if (op->nameStr().find("ED50 to WGS 84 (42)") !=
                std::string::npos) {
                found_EPSG_15964 = true;
            }
        }
        // not expected since doesn't intersect EPSG:23031 area of use
        EXPECT_FALSE(found_EPSG_15964);
    }
    {
        auto ctxt =
            CoordinateOperationContext::create(authFactory, nullptr, 0.0);
        // Ignore source and target CRS extent
        ctxt->setSourceAndTargetCRSExtentUse(
            CoordinateOperationContext::SourceTargetCRSExtentUse::NONE);
        auto list = CoordinateOperationFactory::create()->createOperations(
            authFactory->createCoordinateReferenceSystem("23031"), // ED50 UTM31
            authFactory->createCoordinateReferenceSystem("4326"), ctxt);
        bool found_EPSG_15964 = false;
        for (const auto &op : list) {
            if (op->nameStr().find("ED50 to WGS 84 (42)") !=
                std::string::npos) {
                found_EPSG_15964 = true;
            }
        }
        EXPECT_TRUE(found_EPSG_15964);
    }
}

// ---------------------------------------------------------------------------

TEST(operation, projCRS_to_projCRS_north_pole_inverted_axis) {

    auto authFactory =
        AuthorityFactory::create(DatabaseContext::create(), std::string());
    auto ctxt = CoordinateOperationContext::create(authFactory, nullptr, 0.0);
    auto list = CoordinateOperationFactory::create()->createOperations(
        AuthorityFactory::create(DatabaseContext::create(), "EPSG")
            ->createCoordinateReferenceSystem("32661"),
        AuthorityFactory::create(DatabaseContext::create(), "EPSG")
            ->createCoordinateReferenceSystem("5041"),
        ctxt);
    ASSERT_EQ(list.size(), 1U);
    EXPECT_EQ(list[0]->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=axisswap +order=2,1");
}

// ---------------------------------------------------------------------------

TEST(operation, projCRS_to_projCRS_south_pole_inverted_axis) {

    auto authFactory =
        AuthorityFactory::create(DatabaseContext::create(), std::string());
    auto ctxt = CoordinateOperationContext::create(authFactory, nullptr, 0.0);
    auto list = CoordinateOperationFactory::create()->createOperations(
        AuthorityFactory::create(DatabaseContext::create(), "EPSG")
            ->createCoordinateReferenceSystem("32761"),
        AuthorityFactory::create(DatabaseContext::create(), "EPSG")
            ->createCoordinateReferenceSystem("5042"),
        ctxt);
    ASSERT_EQ(list.size(), 1U);
    EXPECT_EQ(list[0]->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=axisswap +order=2,1");
}

// ---------------------------------------------------------------------------

TEST(operation, projCRS_to_projCRS_through_geog3D) {
    // Check that when going from projCRS to projCRS, using
    // geog2D-->geog3D-->geog3D-->geog2D we do not have issues with
    // inconsistent CRS chaining, due to how we 'hack' a bit some intermediate
    // steps
    auto authFactory =
        AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    auto ctxt = CoordinateOperationContext::create(authFactory, nullptr, 0.0);
    auto list = CoordinateOperationFactory::create()->createOperations(
        authFactory->createCoordinateReferenceSystem("5367"), // CR05 / CRTM05
        authFactory->createCoordinateReferenceSystem(
            "8908"), // CR-SIRGAS / CRTM05
        ctxt);
    ASSERT_EQ(list.size(), 1U);
    EXPECT_EQ(list[0]->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=pipeline +step +proj=axisswap +order=2,1 "
              "+step +inv +proj=tmerc +lat_0=0 +lon_0=-84 +k=0.9999 "
              "+x_0=500000 +y_0=0 +ellps=WGS84 "
              "+step +proj=push +v_3 "
              "+step +proj=cart +ellps=WGS84 "
              "+step +proj=helmert +x=-0.16959 +y=0.35312 +z=0.51846 "
              "+rx=-0.03385 +ry=0.16325 +rz=-0.03446 +s=0.03693 "
              "+convention=coordinate_frame "
              "+step +inv +proj=cart +ellps=GRS80 "
              "+step +proj=pop +v_3 "
              "+step +proj=tmerc +lat_0=0 +lon_0=-84 +k=0.9999 +x_0=500000 "
              "+y_0=0 +ellps=GRS80");
}

// ---------------------------------------------------------------------------

TEST(operation, transform_from_amersfoort_rd_new_to_epsg_4326) {
    auto authFactory =
        AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    auto ctxt = CoordinateOperationContext::create(authFactory, nullptr, 0.0);
    auto list = CoordinateOperationFactory::create()->createOperations(
        authFactory->createCoordinateReferenceSystem("28992"),
        authFactory->createCoordinateReferenceSystem("4326"), ctxt);
    ASSERT_EQ(list.size(), 2U);
    // The order matters: "Amersfoort to WGS 84 (4)" replaces "Amersfoort to WGS
    // 84 (3)"
    EXPECT_EQ(list[0]->nameStr(),
              "Inverse of RD New + Amersfoort to WGS 84 (4)");
    EXPECT_EQ(list[1]->nameStr(),
              "Inverse of RD New + Amersfoort to WGS 84 (3)");
}

// ---------------------------------------------------------------------------

TEST(operation, boundCRS_of_geogCRS_to_geogCRS) {
    auto boundCRS = BoundCRS::createFromTOWGS84(
        GeographicCRS::EPSG_4807, std::vector<double>{1, 2, 3, 4, 5, 6, 7});
    auto op = CoordinateOperationFactory::create()->createOperation(
        boundCRS, GeographicCRS::EPSG_4326);
    ASSERT_TRUE(op != nullptr);
    EXPECT_EQ(op->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=pipeline +step +proj=axisswap +order=2,1 +step "
              "+proj=unitconvert +xy_in=grad +xy_out=rad +step +inv "
              "+proj=longlat +ellps=clrk80ign +pm=paris +step +proj=push +v_3 "
              "+step +proj=cart +ellps=clrk80ign +step +proj=helmert +x=1 +y=2 "
              "+z=3 +rx=4 +ry=5 +rz=6 +s=7 +convention=position_vector +step "
              "+inv +proj=cart +ellps=WGS84 +step +proj=pop +v_3 +step "
              "+proj=unitconvert +xy_in=rad +xy_out=deg +step +proj=axisswap "
              "+order=2,1");
}

// ---------------------------------------------------------------------------

TEST(operation, boundCRS_of_geogCRS_to_geodCRS) {
    auto boundCRS = BoundCRS::createFromTOWGS84(
        GeographicCRS::EPSG_4807, std::vector<double>{1, 2, 3, 4, 5, 6, 7});
    auto op = CoordinateOperationFactory::create()->createOperation(
        boundCRS, GeodeticCRS::EPSG_4978);
    ASSERT_TRUE(op != nullptr);
    EXPECT_EQ(op->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=pipeline +step "
              "+proj=axisswap +order=2,1 "
              "+step +proj=unitconvert +xy_in=grad +xy_out=rad "
              "+step +inv +proj=longlat +ellps=clrk80ign +pm=paris "
              "+step +proj=cart +ellps=clrk80ign "
              "+step +proj=helmert +x=1 +y=2 +z=3 +rx=4 +ry=5 +rz=6 +s=7 "
              "+convention=position_vector");
}

// ---------------------------------------------------------------------------

TEST(operation, boundCRS_of_geogCRS_to_geodCRS_not_related_to_hub) {
    auto authFactory =
        AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    auto boundCRS = BoundCRS::createFromTOWGS84(
        GeographicCRS::EPSG_4807, std::vector<double>{1, 2, 3, 4, 5, 6, 7});
    auto ctxt = CoordinateOperationContext::create(authFactory, nullptr, 0.0);
    auto list = CoordinateOperationFactory::create()->createOperations(
        boundCRS,
        // ETRS89 geocentric
        authFactory->createCoordinateReferenceSystem("4936"), ctxt);
    ASSERT_EQ(list.size(), 1U);
    EXPECT_EQ(list[0]->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=pipeline +step "
              "+proj=axisswap +order=2,1 "
              "+step +proj=unitconvert +xy_in=grad +xy_out=rad "
              "+step +inv +proj=longlat +ellps=clrk80ign +pm=paris "
              "+step +proj=push +v_3 "
              "+step +proj=cart +ellps=clrk80ign "
              "+step +proj=helmert +x=1 +y=2 +z=3 +rx=4 +ry=5 +rz=6 +s=7 "
              "+convention=position_vector "
              "+step +inv +proj=cart +ellps=GRS80 "
              "+step +proj=pop +v_3 "
              "+step +proj=cart +ellps=GRS80");
}

// ---------------------------------------------------------------------------

TEST(operation, boundCRS_of_geogCRS_to_geogCRS_with_area) {
    auto boundCRS = BoundCRS::createFromTOWGS84(
        GeographicCRS::EPSG_4267, std::vector<double>{1, 2, 3, 4, 5, 6, 7});
    auto authFactory =
        AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    auto op = CoordinateOperationFactory::create()->createOperation(
        boundCRS, authFactory->createCoordinateReferenceSystem("4326"));
    ASSERT_TRUE(op != nullptr);
    EXPECT_EQ(op->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=pipeline +step +proj=axisswap +order=2,1 +step "
              "+proj=unitconvert +xy_in=deg +xy_out=rad +step +proj=push +v_3 "
              "+step +proj=cart +ellps=clrk66 +step +proj=helmert +x=1 +y=2 "
              "+z=3 +rx=4 +ry=5 +rz=6 +s=7 +convention=position_vector +step "
              "+inv +proj=cart +ellps=WGS84 +step +proj=pop +v_3 +step "
              "+proj=unitconvert +xy_in=rad +xy_out=deg +step +proj=axisswap "
              "+order=2,1");
}

// ---------------------------------------------------------------------------

TEST(operation, boundCRS_of_geogCRS_to_unrelated_geogCRS) {
    auto boundCRS = BoundCRS::createFromTOWGS84(
        GeographicCRS::EPSG_4807, std::vector<double>{1, 2, 3, 4, 5, 6, 7});
    auto op = CoordinateOperationFactory::create()->createOperation(
        boundCRS, GeographicCRS::EPSG_4269);
    ASSERT_TRUE(op != nullptr);
    EXPECT_EQ(op->exportToPROJString(PROJStringFormatter::create().get()),
              CoordinateOperationFactory::create()
                  ->createOperation(GeographicCRS::EPSG_4807,
                                    GeographicCRS::EPSG_4269)
                  ->exportToPROJString(PROJStringFormatter::create().get()));
}

// ---------------------------------------------------------------------------

TEST(operation, createOperation_boundCRS_identified_by_datum) {
    auto objSrc = PROJStringParser().createFromPROJString(
        "+proj=longlat +datum=WGS84 +type=crs");
    auto src = nn_dynamic_pointer_cast<GeographicCRS>(objSrc);
    ASSERT_TRUE(src != nullptr);

    auto objDest = PROJStringParser().createFromPROJString(
        "+proj=utm +zone=32 +a=6378249.2 +b=6356515 "
        "+towgs84=-263.0,6.0,431.0 +no_defs +type=crs");
    auto dest = nn_dynamic_pointer_cast<BoundCRS>(objDest);
    ASSERT_TRUE(dest != nullptr);

    auto op = CoordinateOperationFactory::create()->createOperation(
        NN_CHECK_ASSERT(src), NN_CHECK_ASSERT(dest));
    ASSERT_TRUE(op != nullptr);
    EXPECT_EQ(op->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=pipeline +step +proj=unitconvert +xy_in=deg +xy_out=rad "
              "+step +proj=push +v_3 +step +proj=cart +ellps=WGS84 +step "
              "+proj=helmert +x=263 +y=-6 +z=-431 +step +inv +proj=cart "
              "+ellps=clrk80ign +step +proj=pop +v_3 +step +proj=utm +zone=32 "
              "+ellps=clrk80ign");

    auto authFactory =
        AuthorityFactory::create(DatabaseContext::create(), std::string());
    auto ctxt = CoordinateOperationContext::create(authFactory, nullptr, 0.0);
    auto list = CoordinateOperationFactory::create()->createOperations(
        NN_CHECK_ASSERT(src), NN_CHECK_ASSERT(dest), ctxt);
    ASSERT_EQ(list.size(), 1U);
    EXPECT_TRUE(list[0]->isEquivalentTo(op.get()));
}

// ---------------------------------------------------------------------------

TEST(operation, boundCRS_of_clrk_66_geogCRS_to_nad83_geogCRS) {
    auto objSrc = PROJStringParser().createFromPROJString(
        "+proj=latlong +ellps=clrk66 +nadgrids=ntv1_can.dat,conus +type=crs");
    auto src = nn_dynamic_pointer_cast<CRS>(objSrc);
    ASSERT_TRUE(src != nullptr);

    auto objDest = PROJStringParser().createFromPROJString(
        "+proj=latlong +datum=NAD83 +type=crs");
    auto dest = nn_dynamic_pointer_cast<CRS>(objDest);
    ASSERT_TRUE(dest != nullptr);

    auto op = CoordinateOperationFactory::create()->createOperation(
        NN_CHECK_ASSERT(src), NN_CHECK_ASSERT(dest));
    ASSERT_TRUE(op != nullptr);
    EXPECT_EQ(op->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=pipeline +step +proj=unitconvert +xy_in=deg +xy_out=rad "
              "+step +proj=hgridshift +grids=ntv1_can.dat,conus "
              "+step +proj=unitconvert +xy_in=rad +xy_out=deg");
}

// ---------------------------------------------------------------------------

TEST(operation, boundCRS_of_clrk_66_projCRS_to_nad83_geogCRS) {
    auto objSrc = PROJStringParser().createFromPROJString(
        "+proj=utm +zone=17 +ellps=clrk66 +nadgrids=ntv1_can.dat,conus "
        "+type=crs");
    auto src = nn_dynamic_pointer_cast<CRS>(objSrc);
    ASSERT_TRUE(src != nullptr);

    auto objDest = PROJStringParser().createFromPROJString(
        "+proj=latlong +datum=NAD83 +type=crs");
    auto dest = nn_dynamic_pointer_cast<CRS>(objDest);
    ASSERT_TRUE(dest != nullptr);

    auto op = CoordinateOperationFactory::create()->createOperation(
        NN_CHECK_ASSERT(src), NN_CHECK_ASSERT(dest));
    ASSERT_TRUE(op != nullptr);
    EXPECT_EQ(op->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=pipeline +step +inv +proj=utm +zone=17 +ellps=clrk66 "
              "+step +proj=hgridshift +grids=ntv1_can.dat,conus "
              "+step +proj=unitconvert +xy_in=rad +xy_out=deg");
}

// ---------------------------------------------------------------------------

TEST(operation, boundCRS_of_projCRS_to_geogCRS) {
    auto utm31 = ProjectedCRS::create(
        PropertyMap(), GeographicCRS::EPSG_4807,
        Conversion::createUTM(PropertyMap(), 31, true),
        CartesianCS::createEastingNorthing(UnitOfMeasure::METRE));
    auto boundCRS = BoundCRS::createFromTOWGS84(
        utm31, std::vector<double>{1, 2, 3, 4, 5, 6, 7});
    auto op = CoordinateOperationFactory::create()->createOperation(
        boundCRS, GeographicCRS::EPSG_4326);
    ASSERT_TRUE(op != nullptr);
    EXPECT_EQ(op->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=pipeline +step +inv +proj=utm +zone=31 +ellps=clrk80ign "
              "+pm=paris +step +proj=push +v_3 +step +proj=cart "
              "+ellps=clrk80ign +step +proj=helmert +x=1 +y=2 +z=3 +rx=4 +ry=5 "
              "+rz=6 +s=7 +convention=position_vector +step +inv +proj=cart "
              "+ellps=WGS84 +step +proj=pop +v_3 +step +proj=unitconvert "
              "+xy_in=rad +xy_out=deg +step +proj=axisswap +order=2,1");
}

// ---------------------------------------------------------------------------

TEST(operation, boundCRS_of_geogCRS_to_projCRS) {
    auto boundCRS = BoundCRS::createFromTOWGS84(
        GeographicCRS::EPSG_4807, std::vector<double>{1, 2, 3, 4, 5, 6, 7});
    auto utm31 = ProjectedCRS::create(
        PropertyMap(), GeographicCRS::EPSG_4326,
        Conversion::createUTM(PropertyMap(), 31, true),
        CartesianCS::createEastingNorthing(UnitOfMeasure::METRE));
    auto op =
        CoordinateOperationFactory::create()->createOperation(boundCRS, utm31);
    ASSERT_TRUE(op != nullptr);
    EXPECT_EQ(op->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=pipeline +step +proj=axisswap +order=2,1 +step "
              "+proj=unitconvert +xy_in=grad +xy_out=rad +step +inv "
              "+proj=longlat +ellps=clrk80ign +pm=paris +step +proj=push +v_3 "
              "+step +proj=cart +ellps=clrk80ign +step +proj=helmert +x=1 +y=2 "
              "+z=3 +rx=4 +ry=5 +rz=6 +s=7 +convention=position_vector +step "
              "+inv +proj=cart +ellps=WGS84 +step +proj=pop +v_3 +step "
              "+proj=utm +zone=31 +ellps=WGS84");
}

// ---------------------------------------------------------------------------

TEST(operation, boundCRS_of_geogCRS_to_unrelated_geogCRS_context) {
    auto src = BoundCRS::createFromTOWGS84(
        GeographicCRS::EPSG_4807, std::vector<double>{1, 2, 3, 4, 5, 6, 7});
    auto authFactory =
        AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    // ETRS89
    auto dst = authFactory->createCoordinateReferenceSystem("4258");
    auto ctxt = CoordinateOperationContext::create(authFactory, nullptr, 0.0);
    auto list =
        CoordinateOperationFactory::create()->createOperations(src, dst, ctxt);
    ASSERT_EQ(list.size(), 1U);
    // Check with it is a concatenated operation, since it doesn't particularly
    // show up in the PROJ string
    EXPECT_TRUE(dynamic_cast<ConcatenatedOperation *>(list[0].get()) !=
                nullptr);
    EXPECT_EQ(list[0]->nameStr(), "Transformation from NTF (Paris) to WGS84 + "
                                  "Inverse of ETRS89 to WGS 84 (1)");
    EXPECT_EQ(list[0]->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=pipeline "
              "+step +proj=axisswap +order=2,1 "
              "+step +proj=unitconvert +xy_in=grad +xy_out=rad "
              "+step +inv +proj=longlat +ellps=clrk80ign +pm=paris "
              "+step +proj=push +v_3 +step +proj=cart +ellps=clrk80ign "
              "+step +proj=helmert +x=1 +y=2 +z=3 +rx=4 +ry=5 +rz=6 +s=7 "
              "+convention=position_vector "
              "+step +inv +proj=cart +ellps=GRS80 +step +proj=pop +v_3 "
              "+step +proj=unitconvert +xy_in=rad +xy_out=deg "
              "+step +proj=axisswap +order=2,1");
}

// ---------------------------------------------------------------------------

TEST(operation, geogCRS_to_boundCRS_of_geogCRS) {
    auto boundCRS = BoundCRS::createFromTOWGS84(
        GeographicCRS::EPSG_4807, std::vector<double>{1, 2, 3, 4, 5, 6, 7});
    auto op = CoordinateOperationFactory::create()->createOperation(
        GeographicCRS::EPSG_4326, boundCRS);
    ASSERT_TRUE(op != nullptr);
    EXPECT_EQ(op->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=pipeline +step +proj=axisswap +order=2,1 +step "
              "+proj=unitconvert +xy_in=deg +xy_out=rad +step +proj=push +v_3 "
              "+step +proj=cart +ellps=WGS84 +step +inv +proj=helmert +x=1 "
              "+y=2 +z=3 +rx=4 +ry=5 +rz=6 +s=7 +convention=position_vector "
              "+step +inv +proj=cart +ellps=clrk80ign +step +proj=pop +v_3 "
              "+step +proj=longlat +ellps=clrk80ign +pm=paris +step "
              "+proj=unitconvert +xy_in=rad +xy_out=grad +step +proj=axisswap "
              "+order=2,1");
}

// ---------------------------------------------------------------------------

TEST(operation, boundCRS_to_geogCRS_same_datum_context) {
    auto boundCRS = BoundCRS::createFromTOWGS84(
        GeographicCRS::EPSG_4269, std::vector<double>{1, 2, 3, 4, 5, 6, 7});
    auto dbContext = DatabaseContext::create();
    auto authFactory = AuthorityFactory::create(dbContext, "EPSG");
    auto ctxt = CoordinateOperationContext::create(authFactory, nullptr, 0.0);
    ctxt->setGridAvailabilityUse(
        CoordinateOperationContext::GridAvailabilityUse::
            IGNORE_GRID_AVAILABILITY);
    ctxt->setSpatialCriterion(
        CoordinateOperationContext::SpatialCriterion::PARTIAL_INTERSECTION);
    auto list = CoordinateOperationFactory::create()->createOperations(
        boundCRS, GeographicCRS::EPSG_4269, ctxt);
    ASSERT_EQ(list.size(), 1U);
    EXPECT_EQ(list[0]->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=noop");
}

// ---------------------------------------------------------------------------

TEST(operation, boundCRS_to_geogCRS_hubCRS_and_targetCRS_same_but_baseCRS_not) {
    const char *wkt =
        "COMPD_CS[\"NAD83 + Ellipsoid (US Feet)\",\n"
        "    GEOGCS[\"NAD83\",\n"
        "        DATUM[\"North_American_Datum_1983\",\n"
        "            SPHEROID[\"GRS 1980\",6378137,298.257222101,\n"
        "                AUTHORITY[\"EPSG\",\"7019\"]],\n"
        "                TOWGS84[0,0,0,0,0,0,0],\n"
        "            AUTHORITY[\"EPSG\",\"6269\"]],\n"
        "        PRIMEM[\"Greenwich\",0,\n"
        "            AUTHORITY[\"EPSG\",\"8901\"]],\n"
        "        UNIT[\"degree\",0.0174532925199433,\n"
        "            AUTHORITY[\"EPSG\",\"9122\"]],\n"
        "        AUTHORITY[\"EPSG\",\"4269\"]],\n"
        "    VERT_CS[\"Ellipsoid (US Feet)\",\n"
        "        VERT_DATUM[\"Ellipsoid\",2002],\n"
        "        UNIT[\"US survey foot\",0.304800609601219,\n"
        "            AUTHORITY[\"EPSG\",\"9003\"]],\n"
        "        AXIS[\"Up\",UP]]]";

    auto dbContext = DatabaseContext::create();
    auto obj = WKTParser().attachDatabaseContext(dbContext).createFromWKT(wkt);
    auto boundCRS = nn_dynamic_pointer_cast<BoundCRS>(obj);
    ASSERT_TRUE(boundCRS != nullptr);
    auto authFactory = AuthorityFactory::create(dbContext, "EPSG");
    auto ctxt = CoordinateOperationContext::create(authFactory, nullptr, 0.0);
    ctxt->setGridAvailabilityUse(
        CoordinateOperationContext::GridAvailabilityUse::
            IGNORE_GRID_AVAILABILITY);
    ctxt->setSpatialCriterion(
        CoordinateOperationContext::SpatialCriterion::PARTIAL_INTERSECTION);
    auto list = CoordinateOperationFactory::create()->createOperations(
        NN_NO_CHECK(boundCRS), GeographicCRS::EPSG_4979, ctxt);
    ASSERT_EQ(list.size(), 1U);
    EXPECT_EQ(list[0]->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=unitconvert +z_in=us-ft +z_out=m");
}

// ---------------------------------------------------------------------------

TEST(
    operation,
    boundCRS_to_derived_geog_with_transformation_with_source_crs_being_base_crs_of_source_crs) {
    auto json =
        "{\n"
        "  \"$schema\": \"foo\",\n"
        "  \"type\": \"BoundCRS\",\n"
        "  \"source_crs\": {\n"
        "    \"type\": \"DerivedGeographicCRS\",\n"
        "    \"name\": \"CH1903+ with height offset\",\n"
        "    \"base_crs\": {\n"
        "      \"type\": \"GeographicCRS\",\n"
        "      \"name\": \"CH1903+\",\n"
        "      \"datum\": {\n"
        "        \"type\": \"GeodeticReferenceFrame\",\n"
        "        \"name\": \"CH1903+\",\n"
        "        \"ellipsoid\": {\n"
        "          \"name\": \"Bessel 1841\",\n"
        "          \"semi_major_axis\": 6377397.155,\n"
        "          \"inverse_flattening\": 299.1528128\n"
        "        }\n"
        "      },\n"
        "      \"coordinate_system\": {\n"
        "        \"subtype\": \"ellipsoidal\",\n"
        "        \"axis\": [\n"
        "          {\n"
        "            \"name\": \"Latitude\",\n"
        "            \"abbreviation\": \"lat\",\n"
        "            \"direction\": \"north\",\n"
        "            \"unit\": \"degree\"\n"
        "          },\n"
        "          {\n"
        "            \"name\": \"Longitude\",\n"
        "            \"abbreviation\": \"lon\",\n"
        "            \"direction\": \"east\",\n"
        "            \"unit\": \"degree\"\n"
        "          },\n"
        "          {\n"
        "            \"name\": \"Ellipsoidal height\",\n"
        "            \"abbreviation\": \"h\",\n"
        "            \"direction\": \"up\",\n"
        "            \"unit\": \"metre\"\n"
        "          }\n"
        "        ]\n"
        "      }\n"
        "    },\n"
        "    \"conversion\": {\n"
        "      \"name\": \"Ellipsoidal to gravity related height\",\n"
        "      \"method\": {\n"
        "        \"name\": \"Geographic3D offsets\",\n"
        "        \"id\": {\n"
        "          \"authority\": \"EPSG\",\n"
        "          \"code\": 9660\n"
        "        }\n"
        "      },\n"
        "      \"parameters\": [\n"
        "        {\n"
        "          \"name\": \"Latitude offset\",\n"
        "          \"value\": 0,\n"
        "          \"unit\": \"degree\",\n"
        "          \"id\": {\n"
        "            \"authority\": \"EPSG\",\n"
        "            \"code\": 8601\n"
        "          }\n"
        "        },\n"
        "        {\n"
        "          \"name\": \"Longitude offset\",\n"
        "          \"value\": 0,\n"
        "          \"unit\": \"degree\",\n"
        "          \"id\": {\n"
        "            \"authority\": \"EPSG\",\n"
        "            \"code\": 8602\n"
        "          }\n"
        "        },\n"
        "        {\n"
        "          \"name\": \"Vertical Offset\",\n"
        "          \"value\": 10,\n"
        "          \"unit\": \"metre\",\n"
        "          \"id\": {\n"
        "            \"authority\": \"EPSG\",\n"
        "            \"code\": 8603\n"
        "          }\n"
        "        }\n"
        "      ]\n"
        "    },\n"
        "    \"coordinate_system\": {\n"
        "      \"subtype\": \"ellipsoidal\",\n"
        "      \"axis\": [\n"
        "        {\n"
        "          \"name\": \"Geodetic latitude\",\n"
        "          \"abbreviation\": \"Lat\",\n"
        "          \"direction\": \"north\",\n"
        "          \"unit\": \"degree\"\n"
        "        },\n"
        "        {\n"
        "          \"name\": \"Geodetic longitude\",\n"
        "          \"abbreviation\": \"Lon\",\n"
        "          \"direction\": \"east\",\n"
        "          \"unit\": \"degree\"\n"
        "        },\n"
        "        {\n"
        "          \"name\": \"Ellipsoidal height\",\n"
        "          \"abbreviation\": \"h\",\n"
        "          \"direction\": \"up\",\n"
        "          \"unit\": \"metre\"\n"
        "        }\n"
        "      ]\n"
        "    }\n"
        "  },\n"
        "  \"target_crs\": {\n"
        "    \"type\": \"GeographicCRS\",\n"
        "    \"name\": \"WGS 84\",\n"
        "    \"datum_ensemble\": {\n"
        "      \"name\": \"World Geodetic System 1984 ensemble\",\n"
        "      \"members\": [\n"
        "        {\n"
        "          \"name\": \"World Geodetic System 1984 (Transit)\",\n"
        "          \"id\": {\n"
        "            \"authority\": \"EPSG\",\n"
        "            \"code\": 1166\n"
        "          }\n"
        "        },\n"
        "        {\n"
        "          \"name\": \"World Geodetic System 1984 (G730)\",\n"
        "          \"id\": {\n"
        "            \"authority\": \"EPSG\",\n"
        "            \"code\": 1152\n"
        "          }\n"
        "        },\n"
        "        {\n"
        "          \"name\": \"World Geodetic System 1984 (G873)\",\n"
        "          \"id\": {\n"
        "            \"authority\": \"EPSG\",\n"
        "            \"code\": 1153\n"
        "          }\n"
        "        },\n"
        "        {\n"
        "          \"name\": \"World Geodetic System 1984 (G1150)\",\n"
        "          \"id\": {\n"
        "            \"authority\": \"EPSG\",\n"
        "            \"code\": 1154\n"
        "          }\n"
        "        },\n"
        "        {\n"
        "          \"name\": \"World Geodetic System 1984 (G1674)\",\n"
        "          \"id\": {\n"
        "            \"authority\": \"EPSG\",\n"
        "            \"code\": 1155\n"
        "          }\n"
        "        },\n"
        "        {\n"
        "          \"name\": \"World Geodetic System 1984 (G1762)\",\n"
        "          \"id\": {\n"
        "            \"authority\": \"EPSG\",\n"
        "            \"code\": 1156\n"
        "          }\n"
        "        },\n"
        "        {\n"
        "          \"name\": \"World Geodetic System 1984 (G2139)\",\n"
        "          \"id\": {\n"
        "            \"authority\": \"EPSG\",\n"
        "            \"code\": 1309\n"
        "          }\n"
        "        }\n"
        "      ],\n"
        "      \"ellipsoid\": {\n"
        "        \"name\": \"WGS 84\",\n"
        "        \"semi_major_axis\": 6378137,\n"
        "        \"inverse_flattening\": 298.257223563\n"
        "      },\n"
        "      \"accuracy\": \"2.0\"\n"
        "    },\n"
        "    \"coordinate_system\": {\n"
        "      \"subtype\": \"ellipsoidal\",\n"
        "      \"axis\": [\n"
        "        {\n"
        "          \"name\": \"Geodetic latitude\",\n"
        "          \"abbreviation\": \"Lat\",\n"
        "          \"direction\": \"north\",\n"
        "          \"unit\": \"degree\"\n"
        "        },\n"
        "        {\n"
        "          \"name\": \"Geodetic longitude\",\n"
        "          \"abbreviation\": \"Lon\",\n"
        "          \"direction\": \"east\",\n"
        "          \"unit\": \"degree\"\n"
        "        },\n"
        "        {\n"
        "          \"name\": \"Ellipsoidal height\",\n"
        "          \"abbreviation\": \"h\",\n"
        "          \"direction\": \"up\",\n"
        "          \"unit\": \"metre\"\n"
        "        }\n"
        "      ]\n"
        "    },\n"
        "    \"id\": {\n"
        "      \"authority\": \"EPSG\",\n"
        "      \"code\": 4979\n"
        "    }\n"
        "  },\n"
        "  \"transformation\": {\n"
        "    \"name\": \"CH1903+ to WGS 84 (1)\",\n"
        "    \"source_crs\": {\n"
        "      \"type\": \"GeographicCRS\",\n"
        "      \"name\": \"CH1903+\",\n"
        "      \"datum\": {\n"
        "        \"type\": \"GeodeticReferenceFrame\",\n"
        "        \"name\": \"CH1903+\",\n"
        "        \"ellipsoid\": {\n"
        "          \"name\": \"Bessel 1841\",\n"
        "          \"semi_major_axis\": 6377397.155,\n"
        "          \"inverse_flattening\": 299.1528128\n"
        "        }\n"
        "      },\n"
        "      \"coordinate_system\": {\n"
        "        \"subtype\": \"ellipsoidal\",\n"
        "        \"axis\": [\n"
        "          {\n"
        "            \"name\": \"Latitude\",\n"
        "            \"abbreviation\": \"lat\",\n"
        "            \"direction\": \"north\",\n"
        "            \"unit\": \"degree\"\n"
        "          },\n"
        "          {\n"
        "            \"name\": \"Longitude\",\n"
        "            \"abbreviation\": \"lon\",\n"
        "            \"direction\": \"east\",\n"
        "            \"unit\": \"degree\"\n"
        "          },\n"
        "          {\n"
        "            \"name\": \"Ellipsoidal height\",\n"
        "            \"abbreviation\": \"h\",\n"
        "            \"direction\": \"up\",\n"
        "            \"unit\": \"metre\"\n"
        "          }\n"
        "        ]\n"
        "      }\n"
        "    },\n"
        "    \"method\": {\n"
        "      \"name\": \"Geocentric translations (geog2D domain)\",\n"
        "      \"id\": {\n"
        "        \"authority\": \"EPSG\",\n"
        "        \"code\": 9603\n"
        "      }\n"
        "    },\n"
        "    \"parameters\": [\n"
        "      {\n"
        "        \"name\": \"X-axis translation\",\n"
        "        \"value\": 674.374,\n"
        "        \"unit\": \"metre\",\n"
        "        \"id\": {\n"
        "          \"authority\": \"EPSG\",\n"
        "          \"code\": 8605\n"
        "        }\n"
        "      },\n"
        "      {\n"
        "        \"name\": \"Y-axis translation\",\n"
        "        \"value\": 15.056,\n"
        "        \"unit\": \"metre\",\n"
        "        \"id\": {\n"
        "          \"authority\": \"EPSG\",\n"
        "          \"code\": 8606\n"
        "        }\n"
        "      },\n"
        "      {\n"
        "        \"name\": \"Z-axis translation\",\n"
        "        \"value\": 405.346,\n"
        "        \"unit\": \"metre\",\n"
        "        \"id\": {\n"
        "          \"authority\": \"EPSG\",\n"
        "          \"code\": 8607\n"
        "        }\n"
        "      }\n"
        "    ],\n"
        "    \"id\": {\n"
        "      \"authority\": \"EPSG\",\n"
        "      \"code\": 1676\n"
        "    }\n"
        "  }\n"
        "}";
    auto obj = createFromUserInput(json, nullptr);
    auto boundCRS = nn_dynamic_pointer_cast<BoundCRS>(obj);
    ASSERT_TRUE(boundCRS != nullptr);
    auto dbContext = DatabaseContext::create();
    auto authFactory = AuthorityFactory::create(dbContext, "EPSG");
    auto ctxt = CoordinateOperationContext::create(authFactory, nullptr, 0.0);
    ctxt->setGridAvailabilityUse(
        CoordinateOperationContext::GridAvailabilityUse::
            IGNORE_GRID_AVAILABILITY);
    ctxt->setSpatialCriterion(
        CoordinateOperationContext::SpatialCriterion::PARTIAL_INTERSECTION);
    auto list = CoordinateOperationFactory::create()->createOperations(
        NN_NO_CHECK(boundCRS), GeographicCRS::EPSG_4979, ctxt);
    ASSERT_EQ(list.size(), 1U);
    EXPECT_EQ(list[0]->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=pipeline "
              "+step +proj=axisswap +order=2,1 "
              "+step +proj=unitconvert +xy_in=deg +z_in=m +xy_out=rad +z_out=m "
              "+step +inv +proj=geogoffset +dlat=0 +dlon=0 +dh=10 "
              "+step +proj=cart +ellps=bessel "
              "+step +proj=helmert +x=674.374 +y=15.056 +z=405.346 "
              "+step +inv +proj=cart +ellps=WGS84 "
              "+step +proj=unitconvert +xy_in=rad +z_in=m +xy_out=deg +z_out=m "
              "+step +proj=axisswap +order=2,1");
}

// ---------------------------------------------------------------------------

TEST(operation, boundCRS_to_boundCRS) {
    auto utm31 = ProjectedCRS::create(
        PropertyMap(), GeographicCRS::EPSG_4807,
        Conversion::createUTM(PropertyMap(), 31, true),
        CartesianCS::createEastingNorthing(UnitOfMeasure::METRE));
    auto utm32 = ProjectedCRS::create(
        PropertyMap(), GeographicCRS::EPSG_4269,
        Conversion::createUTM(PropertyMap(), 32, true),
        CartesianCS::createEastingNorthing(UnitOfMeasure::METRE));
    auto boundCRS1 = BoundCRS::createFromTOWGS84(
        utm31, std::vector<double>{1, 2, 3, 4, 5, 6, 7});
    auto boundCRS2 = BoundCRS::createFromTOWGS84(
        utm32, std::vector<double>{8, 9, 10, 11, 12, 13, 14});
    auto op = CoordinateOperationFactory::create()->createOperation(boundCRS1,
                                                                    boundCRS2);
    ASSERT_TRUE(op != nullptr);
    EXPECT_EQ(op->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=pipeline +step +inv +proj=utm +zone=31 +ellps=clrk80ign "
              "+pm=paris +step +proj=push +v_3 +step +proj=cart "
              "+ellps=clrk80ign +step +proj=helmert +x=1 +y=2 +z=3 +rx=4 +ry=5 "
              "+rz=6 +s=7 +convention=position_vector +step +inv +proj=helmert "
              "+x=8 +y=9 +z=10 +rx=11 +ry=12 +rz=13 +s=14 "
              "+convention=position_vector +step +inv +proj=cart +ellps=GRS80 "
              "+step +proj=pop +v_3 +step +proj=utm +zone=32 +ellps=GRS80");
}

// ---------------------------------------------------------------------------

TEST(operation, boundCRS_to_boundCRS_noop_for_TOWGS84) {
    auto boundCRS1 = BoundCRS::createFromTOWGS84(
        GeographicCRS::EPSG_4807, std::vector<double>{1, 2, 3, 4, 5, 6, 7});
    auto boundCRS2 = BoundCRS::createFromTOWGS84(
        GeographicCRS::EPSG_4269, std::vector<double>{1, 2, 3, 4, 5, 6, 7});
    auto op = CoordinateOperationFactory::create()->createOperation(boundCRS1,
                                                                    boundCRS2);
    ASSERT_TRUE(op != nullptr);
    EXPECT_EQ(op->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=pipeline +step +proj=axisswap +order=2,1 +step "
              "+proj=unitconvert +xy_in=grad +xy_out=rad +step +inv "
              "+proj=longlat +ellps=clrk80ign +pm=paris +step +proj=push +v_3 "
              "+step +proj=cart +ellps=clrk80ign +step +inv +proj=cart "
              "+ellps=GRS80 +step +proj=pop +v_3 +step +proj=unitconvert "
              "+xy_in=rad +xy_out=deg +step +proj=axisswap +order=2,1");
}

// ---------------------------------------------------------------------------

TEST(operation, boundCRS_to_boundCRS_unralated_hub) {
    auto boundCRS1 = BoundCRS::createFromTOWGS84(
        GeographicCRS::EPSG_4807, std::vector<double>{1, 2, 3, 4, 5, 6, 7});
    auto boundCRS2 = BoundCRS::create(
        GeographicCRS::EPSG_4269, GeographicCRS::EPSG_4979,
        Transformation::createGeocentricTranslations(
            PropertyMap(), GeographicCRS::EPSG_4269, GeographicCRS::EPSG_4979,
            1.0, 2.0, 3.0, std::vector<PositionalAccuracyNNPtr>()));
    auto op = CoordinateOperationFactory::create()->createOperation(boundCRS1,
                                                                    boundCRS2);
    ASSERT_TRUE(op != nullptr);
    EXPECT_EQ(op->exportToPROJString(PROJStringFormatter::create().get()),
              CoordinateOperationFactory::create()
                  ->createOperation(boundCRS1->baseCRS(), boundCRS2->baseCRS())
                  ->exportToPROJString(PROJStringFormatter::create().get()));
}

// ---------------------------------------------------------------------------

TEST(operation, boundCRS_of_projCRS_towgs84_to_boundCRS_of_projCRS_nadgrids) {
    auto objSrc = PROJStringParser().createFromPROJString(
        "+proj=utm +zone=15 +datum=NAD83 +units=m +no_defs +ellps=GRS80 "
        "+towgs84=0,0,0 +type=crs");
    auto src = nn_dynamic_pointer_cast<CRS>(objSrc);
    ASSERT_TRUE(src != nullptr);
    auto objDst = PROJStringParser().createFromPROJString(
        "+proj=utm +zone=15 +datum=NAD27 +units=m +no_defs +ellps=clrk66 "
        "+nadgrids=@conus,@alaska,@ntv2_0.gsb,@ntv1_can.dat +type=crs");
    auto dst = nn_dynamic_pointer_cast<CRS>(objDst);
    ASSERT_TRUE(dst != nullptr);
    auto op = CoordinateOperationFactory::create()->createOperation(
        NN_CHECK_ASSERT(src), NN_CHECK_ASSERT(dst));
    ASSERT_TRUE(op != nullptr);
    EXPECT_EQ(op->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=pipeline +step +inv +proj=utm +zone=15 +ellps=GRS80 +step "
              "+inv +proj=hgridshift "
              "+grids=@conus,@alaska,@ntv2_0.gsb,@ntv1_can.dat +step +proj=utm "
              "+zone=15 +ellps=clrk66");
}

// ---------------------------------------------------------------------------

TEST(operation, boundCRS_of_projCRS_towgs84_non_metre_unit_to_geocentric) {
    auto objSrc = PROJStringParser().createFromPROJString(
        "+proj=merc +ellps=GRS80 +towgs84=0,0,0 +units=ft +vunits=ft "
        "+type=crs");
    auto src = nn_dynamic_pointer_cast<CRS>(objSrc);
    ASSERT_TRUE(src != nullptr);
    auto objDst = PROJStringParser().createFromPROJString(
        "+proj=geocent +datum=WGS84 +type=crs");
    auto dst = nn_dynamic_pointer_cast<CRS>(objDst);
    ASSERT_TRUE(dst != nullptr);
    auto op = CoordinateOperationFactory::create()->createOperation(
        NN_CHECK_ASSERT(src), NN_CHECK_ASSERT(dst));
    ASSERT_TRUE(op != nullptr);
    EXPECT_EQ(op->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=pipeline "
              "+step +proj=unitconvert +xy_in=ft +z_in=ft +xy_out=m +z_out=m "
              "+step +inv +proj=merc +lon_0=0 +k=1 +x_0=0 +y_0=0 +ellps=GRS80 "
              "+step +proj=cart +ellps=WGS84");
}

// ---------------------------------------------------------------------------

static CRSNNPtr buildCRSFromProjStrThroughWKT(const std::string &projStr) {
    auto crsFromProj = nn_dynamic_pointer_cast<CRS>(
        PROJStringParser().createFromPROJString(projStr));
    if (crsFromProj == nullptr) {
        throw "crsFromProj == nullptr";
    }
    auto crsFromWkt = nn_dynamic_pointer_cast<CRS>(
        WKTParser().createFromWKT(crsFromProj->exportToWKT(
            WKTFormatter::create(WKTFormatter::Convention::WKT2_2019).get())));
    if (crsFromWkt == nullptr) {
        throw "crsFromWkt == nullptr";
    }
    return NN_NO_CHECK(crsFromWkt);
}

TEST(operation,
     boundCRS_to_boundCRS_with_base_geog_crs_different_from_source_of_transf) {

    auto src = buildCRSFromProjStrThroughWKT(
        "+proj=lcc +lat_1=49 +lat_0=49 +lon_0=0 +k_0=0.999877499 +x_0=600000 "
        "+y_0=200000 +ellps=clrk80ign +pm=paris +towgs84=-168,-60,320,0,0,0,0 "
        "+units=m +no_defs +type=crs");
    auto dst = buildCRSFromProjStrThroughWKT(
        "+proj=longlat +ellps=clrk80ign +pm=paris "
        "+towgs84=-168,-60,320,0,0,0,0 +no_defs +type=crs");

    auto op = CoordinateOperationFactory::create()->createOperation(src, dst);
    ASSERT_TRUE(op != nullptr);
    EXPECT_EQ(op->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=pipeline "
              "+step +inv +proj=lcc +lat_1=49 +lat_0=49 +lon_0=0 "
              "+k_0=0.999877499 +x_0=600000 +y_0=200000 +ellps=clrk80ign "
              "+pm=paris "
              "+step +proj=longlat +ellps=clrk80ign +pm=paris "
              "+step +proj=unitconvert +xy_in=rad +xy_out=deg");
}

// ---------------------------------------------------------------------------

TEST(operation, boundCRS_with_basecrs_with_extent_to_geogCRS) {

    auto wkt =
        "BOUNDCRS[\n"
        "    SOURCECRS[\n"
        "        PROJCRS[\"NAD83 / California zone 3 (ftUS)\",\n"
        "            BASEGEODCRS[\"NAD83\",\n"
        "                DATUM[\"North American Datum 1983\",\n"
        "                    ELLIPSOID[\"GRS 1980\",6378137,298.257222101,\n"
        "                        LENGTHUNIT[\"metre\",1]]],\n"
        "                PRIMEM[\"Greenwich\",0,\n"
        "                    ANGLEUNIT[\"degree\",0.0174532925199433]]],\n"
        "            CONVERSION[\"SPCS83 California zone 3 (US Survey "
        "feet)\",\n"
        "                METHOD[\"Lambert Conic Conformal (2SP)\",\n"
        "                    ID[\"EPSG\",9802]],\n"
        "                PARAMETER[\"Latitude of false origin\",36.5,\n"
        "                    ANGLEUNIT[\"degree\",0.0174532925199433],\n"
        "                    ID[\"EPSG\",8821]],\n"
        "                PARAMETER[\"Longitude of false origin\",-120.5,\n"
        "                    ANGLEUNIT[\"degree\",0.0174532925199433],\n"
        "                    ID[\"EPSG\",8822]],\n"
        "                PARAMETER[\"Latitude of 1st standard parallel\","
        "                    38.4333333333333,\n"
        "                    ANGLEUNIT[\"degree\",0.0174532925199433],\n"
        "                    ID[\"EPSG\",8823]],\n"
        "                PARAMETER[\"Latitude of 2nd standard parallel\","
        "                    37.0666666666667,\n"
        "                    ANGLEUNIT[\"degree\",0.0174532925199433],\n"
        "                    ID[\"EPSG\",8824]],\n"
        "                PARAMETER[\"Easting at false origin\",6561666.667,\n"
        "                    LENGTHUNIT[\"US survey foot\","
        "                    0.304800609601219],\n"
        "                    ID[\"EPSG\",8826]],\n"
        "                PARAMETER[\"Northing at false origin\",1640416.667,\n"
        "                    LENGTHUNIT[\"US survey foot\","
        "                    0.304800609601219],\n"
        "                    ID[\"EPSG\",8827]]],\n"
        "            CS[Cartesian,2],\n"
        "                AXIS[\"easting (X)\",east,\n"
        "                    ORDER[1],\n"
        "                    LENGTHUNIT[\"US survey foot\","
        "                    0.304800609601219]],\n"
        "                AXIS[\"northing (Y)\",north,\n"
        "                    ORDER[2],\n"
        "                    LENGTHUNIT[\"US survey foot\","
        "                    0.304800609601219]],\n"
        "            SCOPE[\"unknown\"],\n"
        "            AREA[\"USA - California - SPCS - 3\"],\n"
        "            BBOX[36.73,-123.02,38.71,-117.83],\n"
        "            ID[\"EPSG\",2227]]],\n"
        "    TARGETCRS[\n"
        "        GEODCRS[\"WGS 84\",\n"
        "            DATUM[\"World Geodetic System 1984\",\n"
        "                ELLIPSOID[\"WGS 84\",6378137,298.257223563,\n"
        "                    LENGTHUNIT[\"metre\",1]]],\n"
        "            PRIMEM[\"Greenwich\",0,\n"
        "                ANGLEUNIT[\"degree\",0.0174532925199433]],\n"
        "            CS[ellipsoidal,2],\n"
        "                AXIS[\"latitude\",north,\n"
        "                    ORDER[1],\n"
        "                    ANGLEUNIT[\"degree\",0.0174532925199433]],\n"
        "                AXIS[\"longitude\",east,\n"
        "                    ORDER[2],\n"
        "                    ANGLEUNIT[\"degree\",0.0174532925199433]],\n"
        "            ID[\"EPSG\",4326]]],\n"
        "    ABRIDGEDTRANSFORMATION[\"NAD83 to WGS 84 (1)\",\n"
        "        METHOD[\"Geocentric translations (geog2D domain)\",\n"
        "            ID[\"EPSG\",9603]],\n"
        "        PARAMETER[\"X-axis translation\",0,\n"
        "            ID[\"EPSG\",8605]],\n"
        "        PARAMETER[\"Y-axis translation\",0,\n"
        "            ID[\"EPSG\",8606]],\n"
        "        PARAMETER[\"Z-axis translation\",0,\n"
        "            ID[\"EPSG\",8607]],\n"
        "        SCOPE[\"unknown\"],\n"
        "        AREA[\"North America - Canada and USA (CONUS, Alaska "
        "mainland)\"],\n"
        "        BBOX[23.81,-172.54,86.46,-47.74],\n"
        "        ID[\"EPSG\",1188]]]";
    auto obj = WKTParser().createFromWKT(wkt);
    auto boundCRS = nn_dynamic_pointer_cast<BoundCRS>(obj);
    ASSERT_TRUE(boundCRS != nullptr);
    auto op = CoordinateOperationFactory::create()->createOperation(
        NN_CHECK_ASSERT(boundCRS), GeographicCRS::EPSG_4326);
    ASSERT_TRUE(op != nullptr);
    EXPECT_EQ(op->nameStr(), "Inverse of SPCS83 California zone 3 (US Survey "
                             "feet) + NAD83 to WGS 84 (1)");
}

// ---------------------------------------------------------------------------

TEST(operation, ETRS89_3D_to_proj_string_with_geoidgrids_nadgrids) {
    auto authFactory =
        AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    // ETRS89 3D
    auto src = authFactory->createCoordinateReferenceSystem("4937");
    auto objDst = PROJStringParser().createFromPROJString(
        "+proj=sterea +lat_0=52.15616055555555 +lon_0=5.38763888888889 "
        "+k=0.9999079 +x_0=155000 +y_0=463000 +ellps=bessel "
        "+nadgrids=rdtrans2008.gsb +geoidgrids=naptrans2008.gtx "
        "+geoid_crs=horizontal_crs +units=m "
        "+type=crs");
    auto dst = nn_dynamic_pointer_cast<CRS>(objDst);
    ASSERT_TRUE(dst != nullptr);
    auto ctxt = CoordinateOperationContext::create(authFactory, nullptr, 0.0);
    auto list = CoordinateOperationFactory::create()->createOperations(
        src, NN_NO_CHECK(dst), ctxt);
    ASSERT_EQ(list.size(), 2U);
    EXPECT_EQ(list[0]->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=pipeline "
              "+step +proj=axisswap +order=2,1 "
              "+step +proj=unitconvert +xy_in=deg +z_in=m +xy_out=rad +z_out=m "
              "+step +inv +proj=hgridshift +grids=rdtrans2008.gsb "
              "+step +inv +proj=vgridshift +grids=naptrans2008.gtx "
              "+multiplier=1 "
              "+step +proj=sterea +lat_0=52.1561605555556 "
              "+lon_0=5.38763888888889 +k=0.9999079 +x_0=155000 "
              "+y_0=463000 +ellps=bessel");
}

// ---------------------------------------------------------------------------

TEST(operation, nadgrids_with_pm) {
    auto authFactory =
        AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    auto objSrc = PROJStringParser().createFromPROJString(
        "+proj=tmerc +lat_0=39.66666666666666 +lon_0=1 +k=1 +x_0=200000 "
        "+y_0=300000 +ellps=intl +nadgrids=foo.gsb +pm=lisbon "
        "+units=m +type=crs");
    auto src = nn_dynamic_pointer_cast<CRS>(objSrc);
    ASSERT_TRUE(src != nullptr);
    auto dst = authFactory->createCoordinateReferenceSystem("4326");
    auto ctxt = CoordinateOperationContext::create(authFactory, nullptr, 0.0);
    auto list = CoordinateOperationFactory::create()->createOperations(
        NN_NO_CHECK(src), dst, ctxt);
    ASSERT_EQ(list.size(), 1U);
    EXPECT_EQ(list[0]->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=pipeline "
              "+step +inv +proj=tmerc +lat_0=39.6666666666667 +lon_0=1 "
              "+k=1 +x_0=200000 +y_0=300000 +ellps=intl +pm=lisbon "
              // Check that there is no extra +step +proj=longlat +pm=lisbon
              "+step +proj=hgridshift +grids=foo.gsb "
              "+step +proj=unitconvert +xy_in=rad +xy_out=deg "
              "+step +proj=axisswap +order=2,1");

    // ETRS89
    dst = authFactory->createCoordinateReferenceSystem("4258");
    list = CoordinateOperationFactory::create()->createOperations(
        NN_NO_CHECK(src), dst, ctxt);
    ASSERT_GE(list.size(), 1U);
    EXPECT_EQ(list[0]->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=pipeline "
              "+step +inv +proj=tmerc +lat_0=39.6666666666667 +lon_0=1 "
              "+k=1 +x_0=200000 +y_0=300000 +ellps=intl +pm=lisbon "
              // Check that there is no extra +step +proj=longlat +pm=lisbon
              "+step +proj=hgridshift +grids=foo.gsb "
              "+step +proj=unitconvert +xy_in=rad +xy_out=deg "
              "+step +proj=axisswap +order=2,1");

    // From WKT BOUNDCRS
    auto formatter = WKTFormatter::create(WKTFormatter::Convention::WKT2_2019);
    auto src_wkt = src->exportToWKT(formatter.get());
    auto objFromWkt = WKTParser().createFromWKT(src_wkt);
    auto crsFromWkt = nn_dynamic_pointer_cast<BoundCRS>(objFromWkt);
    ASSERT_TRUE(crsFromWkt);
    list = CoordinateOperationFactory::create()->createOperations(
        NN_NO_CHECK(crsFromWkt), dst, ctxt);
    ASSERT_GE(list.size(), 1U);
    EXPECT_EQ(list[0]->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=pipeline "
              "+step +inv +proj=tmerc +lat_0=39.6666666666667 +lon_0=1 "
              "+k=1 +x_0=200000 +y_0=300000 +ellps=intl +pm=lisbon "
              // Check that there is no extra +step +proj=longlat +pm=lisbon
              "+step +proj=hgridshift +grids=foo.gsb "
              "+step +proj=unitconvert +xy_in=rad +xy_out=deg "
              "+step +proj=axisswap +order=2,1");
}

// ---------------------------------------------------------------------------

TEST(operation, towgs84_pm_3d) {
    // Test fix for https://github.com/OSGeo/gdal/issues/5408

    auto dbContext = DatabaseContext::create();
    auto authFactory = AuthorityFactory::create(dbContext, std::string());

    auto objSrc = PROJStringParser().createFromPROJString(
        "+proj=tmerc +lat_0=0 +lon_0=34 +k=1 +x_0=0 +y_0=-5000000 "
        "+ellps=bessel +pm=ferro "
        "+towgs84=1,2,3,4,5,6,7 "
        "+units=m +no_defs +type=crs");
    auto src = nn_dynamic_pointer_cast<CRS>(objSrc);
    ASSERT_TRUE(src != nullptr);
    auto src3D = src->promoteTo3D(std::string(), dbContext);

    auto objDst = PROJStringParser().createFromPROJString(
        "+proj=longlat +ellps=GRS80 +towgs84=0,0,0,0,0,0,0 +no_defs +type=crs");
    auto dst = nn_dynamic_pointer_cast<CRS>(objDst);
    ASSERT_TRUE(dst != nullptr);
    auto dst3D = dst->promoteTo3D(std::string(), dbContext);

    // Import thing to check is that there's no push/pop v_3
    const std::string expected_pipeline =
        "+proj=pipeline "
        "+step +inv +proj=tmerc +lat_0=0 +lon_0=34 +k=1 +x_0=0 +y_0=-5000000 "
        "+ellps=bessel +pm=ferro "
        "+step +proj=cart +ellps=bessel "
        "+step +proj=helmert +x=1 +y=2 +z=3 +rx=4 "
        "+ry=5 +rz=6 +s=7 +convention=position_vector "
        "+step +inv +proj=cart +ellps=GRS80 "
        "+step +proj=unitconvert +xy_in=rad +z_in=m +xy_out=deg +z_out=m";

    auto ctxt = CoordinateOperationContext::create(authFactory, nullptr, 0.0);
    {
        auto list = CoordinateOperationFactory::create()->createOperations(
            src3D, dst3D, ctxt);
        ASSERT_EQ(list.size(), 1U);
        EXPECT_EQ(
            list[0]->exportToPROJString(PROJStringFormatter::create().get()),
            expected_pipeline);
    }

    // Retry when creating objects from WKT
    {
        auto objSrcFromWkt = WKTParser().createFromWKT(src3D->exportToWKT(
            WKTFormatter::create(WKTFormatter::Convention::WKT2_2019).get()));
        auto srcFromWkt = nn_dynamic_pointer_cast<CRS>(objSrcFromWkt);
        ASSERT_TRUE(srcFromWkt != nullptr);
        auto objDstFromWkt = WKTParser().createFromWKT(dst3D->exportToWKT(
            WKTFormatter::create(WKTFormatter::Convention::WKT2_2019).get()));
        auto dstFromWkt = nn_dynamic_pointer_cast<CRS>(objDstFromWkt);
        ASSERT_TRUE(dstFromWkt != nullptr);
        auto list = CoordinateOperationFactory::create()->createOperations(
            NN_NO_CHECK(srcFromWkt), NN_NO_CHECK(dstFromWkt), ctxt);
        ASSERT_EQ(list.size(), 1U);
        EXPECT_EQ(
            list[0]->exportToPROJString(PROJStringFormatter::create().get()),
            expected_pipeline);
    }
}

// ---------------------------------------------------------------------------

TEST(operation, WGS84_G1762_to_compoundCRS_with_bound_vertCRS) {
    auto authFactoryEPSG =
        AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    // WGS 84 (G1762) 3D
    auto src = authFactoryEPSG->createCoordinateReferenceSystem("7665");
    auto objDst = PROJStringParser().createFromPROJString(
        "+proj=longlat +datum=NAD83 +geoidgrids=@foo.gtx +type=crs");
    auto dst = nn_dynamic_pointer_cast<CRS>(objDst);
    ASSERT_TRUE(dst != nullptr);
    auto authFactory =
        AuthorityFactory::create(DatabaseContext::create(), std::string());
    auto ctxt = CoordinateOperationContext::create(authFactory, nullptr, 0.0);
    ctxt->setSpatialCriterion(
        CoordinateOperationContext::SpatialCriterion::PARTIAL_INTERSECTION);
    ctxt->setGridAvailabilityUse(
        CoordinateOperationContext::GridAvailabilityUse::
            IGNORE_GRID_AVAILABILITY);
    auto list = CoordinateOperationFactory::create()->createOperations(
        src, NN_NO_CHECK(dst), ctxt);
    ASSERT_GE(list.size(), 53U);
    EXPECT_EQ(list[0]->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=pipeline "
              "+step +proj=axisswap +order=2,1 "
              "+step +proj=unitconvert +xy_in=deg +xy_out=rad "
              "+step +inv +proj=vgridshift +grids=@foo.gtx +multiplier=1 "
              "+step +proj=unitconvert +xy_in=rad +xy_out=deg");
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

TEST(operation, compoundCRS_to_geogCRS) {

    auto compound = CompoundCRS::create(
        PropertyMap(),
        std::vector<CRSNNPtr>{GeographicCRS::EPSG_4326, createVerticalCRS()});
    auto op = CoordinateOperationFactory::create()->createOperation(
        compound, GeographicCRS::EPSG_4807);
    ASSERT_TRUE(op != nullptr);
    EXPECT_EQ(op->exportToPROJString(PROJStringFormatter::create().get()),
              CoordinateOperationFactory::create()
                  ->createOperation(GeographicCRS::EPSG_4326,
                                    GeographicCRS::EPSG_4807)
                  ->exportToPROJString(PROJStringFormatter::create().get()));
}

// ---------------------------------------------------------------------------

static BoundCRSNNPtr createBoundVerticalCRS() {
    auto vertCRS = createVerticalCRS();
    auto transformation =
        Transformation::createGravityRelatedHeightToGeographic3D(
            PropertyMap(), vertCRS, GeographicCRS::EPSG_4979, nullptr,
            "us_nga_egm08_25.tif", std::vector<PositionalAccuracyNNPtr>());
    return BoundCRS::create(vertCRS, GeographicCRS::EPSG_4979, transformation);
}

// ---------------------------------------------------------------------------

TEST(operation, transformation_height_to_PROJ_string) {
    auto transf = createBoundVerticalCRS()->transformation();
    EXPECT_EQ(transf->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=pipeline "
              "+step +proj=axisswap +order=2,1 "
              "+step +proj=unitconvert +xy_in=deg +xy_out=rad "
              "+step +proj=vgridshift +grids=us_nga_egm08_25.tif "
              "+multiplier=1 "
              "+step +proj=unitconvert +xy_in=rad +xy_out=deg "
              "+step +proj=axisswap +order=2,1");

    auto grids = transf->gridsNeeded(DatabaseContext::create(), false);
    ASSERT_EQ(grids.size(), 1U);
    auto gridDesc = *(grids.begin());
    EXPECT_EQ(gridDesc.shortName, "us_nga_egm08_25.tif");
    EXPECT_TRUE(gridDesc.packageName.empty());
    EXPECT_EQ(gridDesc.url, "https://cdn.proj.org/us_nga_egm08_25.tif");
    if (gridDesc.available) {
        EXPECT_TRUE(!gridDesc.fullName.empty()) << gridDesc.fullName;
        EXPECT_TRUE(gridDesc.fullName.find(gridDesc.shortName) !=
                    std::string::npos)
            << gridDesc.fullName;
    } else {
        EXPECT_TRUE(gridDesc.fullName.empty()) << gridDesc.fullName;
    }
    EXPECT_EQ(gridDesc.directDownload, true);
    EXPECT_EQ(gridDesc.openLicense, true);
}

// ---------------------------------------------------------------------------

TEST(operation, transformation_Geographic3D_to_GravityRelatedHeight_gtx) {
    auto wkt =
        "COORDINATEOPERATION[\"ETRS89 to NAP height (1)\",\n"
        "    VERSION[\"RDNAP-Nld 2008\"],\n"
        "    SOURCECRS[\n"
        "        GEOGCRS[\"ETRS89\",\n"
        "            DATUM[\"European Terrestrial Reference System 1989\",\n"
        "                ELLIPSOID[\"GRS 1980\",6378137,298.257222101,\n"
        "                    LENGTHUNIT[\"metre\",1]]],\n"
        "            PRIMEM[\"Greenwich\",0,\n"
        "                ANGLEUNIT[\"degree\",0.0174532925199433]],\n"
        "            CS[ellipsoidal,3],\n"
        "                AXIS[\"geodetic latitude (Lat)\",north,\n"
        "                    ORDER[1],\n"
        "                    ANGLEUNIT[\"degree\",0.0174532925199433]],\n"
        "                AXIS[\"geodetic longitude (Lon)\",east,\n"
        "                    ORDER[2],\n"
        "                    ANGLEUNIT[\"degree\",0.0174532925199433]],\n"
        "                AXIS[\"ellipsoidal height (h)\",up,\n"
        "                    ORDER[3],\n"
        "                    LENGTHUNIT[\"metre\",1]],\n"
        "            ID[\"EPSG\",4937]]],\n"
        "    TARGETCRS[\n"
        "        VERTCRS[\"NAP height\",\n"
        "            VDATUM[\"Normaal Amsterdams Peil\"],\n"
        "            CS[vertical,1],\n"
        "                AXIS[\"gravity-related height (H)\",up,\n"
        "                    LENGTHUNIT[\"metre\",1]],\n"
        "            ID[\"EPSG\",5709]]],\n"
        "    METHOD[\"Geographic3D to GravityRelatedHeight (US .gtx)\",\n"
        "        ID[\"EPSG\",9665]],\n"
        "    PARAMETERFILE[\"Geoid (height correction) model "
        "file\",\"naptrans2008.gtx\"],\n"
        "    OPERATIONACCURACY[0.01],\n"
        "    USAGE[\n"
        "        SCOPE[\"unknown\"],\n"
        "        AREA[\"Netherlands - onshore\"],\n"
        "        BBOX[50.75,3.2,53.7,7.22]],\n"
        "    ID[\"EPSG\",7001]]";

    auto obj = WKTParser().createFromWKT(wkt);
    auto transf = nn_dynamic_pointer_cast<Transformation>(obj);
    ASSERT_TRUE(transf != nullptr);

    // Check that we correctly inverse files in the case of
    // "Geographic3D to GravityRelatedHeight (US .gtx)"
    EXPECT_EQ(transf->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=pipeline "
              "+step +proj=axisswap +order=2,1 "
              "+step +proj=unitconvert +xy_in=deg +xy_out=rad "
              "+step +inv +proj=vgridshift "
              "+grids=naptrans2008.gtx +multiplier=1 "
              "+step +proj=unitconvert +xy_in=rad +xy_out=deg "
              "+step +proj=axisswap +order=2,1");
}

// ---------------------------------------------------------------------------

TEST(operation, transformation_ntv2_to_PROJ_string) {
    auto transformation = Transformation::createNTv2(
        PropertyMap(), GeographicCRS::EPSG_4807, GeographicCRS::EPSG_4326,
        "foo.gsb", std::vector<PositionalAccuracyNNPtr>());
    EXPECT_EQ(
        transformation->exportToPROJString(PROJStringFormatter::create().get()),
        "+proj=pipeline +step +proj=axisswap +order=2,1 +step "
        "+proj=unitconvert +xy_in=grad +xy_out=rad +step "
        "+proj=hgridshift +grids=foo.gsb +step +proj=unitconvert "
        "+xy_in=rad +xy_out=deg +step +proj=axisswap +order=2,1");
}

// ---------------------------------------------------------------------------

TEST(operation, transformation_VERTCON_to_PROJ_string) {
    auto verticalCRS1 = createVerticalCRS();

    auto verticalCRS2 = VerticalCRS::create(
        PropertyMap(), VerticalReferenceFrame::create(PropertyMap()),
        VerticalCS::createGravityRelatedHeight(UnitOfMeasure::METRE));

    // Use of this type of transformation is a bit of nonsense here
    // since it should normally be used with NGVD29 and NAVD88 for VerticalCRS,
    // and NAD27/NAD83 as horizontal CRS...
    auto vtransformation = Transformation::createVERTCON(
        PropertyMap(), verticalCRS1, verticalCRS2, "bla.gtx",
        std::vector<PositionalAccuracyNNPtr>());
    EXPECT_EQ(vtransformation->exportToPROJString(
                  PROJStringFormatter::create().get()),
              "+proj=vgridshift +grids=bla.gtx +multiplier=0.001");
}
// ---------------------------------------------------------------------------

TEST(operation, transformation_NZLVD_to_PROJ_string) {
    auto dbContext = DatabaseContext::create();
    auto factory = AuthorityFactory::create(dbContext, "EPSG");
    auto op = factory->createCoordinateOperation("7860", false);
    EXPECT_EQ(op->exportToPROJString(
                  PROJStringFormatter::create(
                      PROJStringFormatter::Convention::PROJ_5, dbContext)
                      .get()),
              "+proj=vgridshift +grids=nz_linz_auckht1946-nzvd2016.tif "
              "+multiplier=1");
}

// ---------------------------------------------------------------------------

TEST(operation, transformation_BEV_AT_to_PROJ_string) {
    auto dbContext = DatabaseContext::create();
    auto factory = AuthorityFactory::create(dbContext, "EPSG");
    auto op = factory->createCoordinateOperation("9275", false);
    EXPECT_EQ(op->exportToPROJString(
                  PROJStringFormatter::create(
                      PROJStringFormatter::Convention::PROJ_5, dbContext)
                      .get()),
              "+proj=vgridshift +grids=at_bev_GV_Hoehengrid_V1.tif "
              "+multiplier=1");
}

// ---------------------------------------------------------------------------

TEST(operation, transformation_longitude_rotation_to_PROJ_string) {

    auto src = GeographicCRS::create(
        PropertyMap(),
        GeodeticReferenceFrame::create(PropertyMap(), Ellipsoid::WGS84,
                                       optional<std::string>(),
                                       PrimeMeridian::GREENWICH),
        EllipsoidalCS::createLatitudeLongitude(UnitOfMeasure::DEGREE));
    auto dest = GeographicCRS::create(
        PropertyMap(),
        GeodeticReferenceFrame::create(PropertyMap(), Ellipsoid::WGS84,
                                       optional<std::string>(),
                                       PrimeMeridian::PARIS),
        EllipsoidalCS::createLatitudeLongitude(UnitOfMeasure::DEGREE));
    auto transformation = Transformation::createLongitudeRotation(
        PropertyMap(), src, dest, Angle(10));
    EXPECT_TRUE(transformation->validateParameters().empty());
    EXPECT_EQ(
        transformation->exportToPROJString(PROJStringFormatter::create().get()),
        "+proj=pipeline +step +proj=axisswap +order=2,1 +step "
        "+proj=unitconvert +xy_in=deg +xy_out=rad +step +inv "
        "+proj=longlat +ellps=WGS84 +pm=10 +step +proj=unitconvert "
        "+xy_in=rad +xy_out=deg +step +proj=axisswap +order=2,1");
    EXPECT_EQ(transformation->inverse()->exportToPROJString(
                  PROJStringFormatter::create().get()),
              "+proj=pipeline +step +proj=axisswap +order=2,1 +step "
              "+proj=unitconvert +xy_in=deg +xy_out=rad +step +inv "
              "+proj=longlat +ellps=WGS84 +pm=-10 +step +proj=unitconvert "
              "+xy_in=rad +xy_out=deg +step +proj=axisswap +order=2,1");
}

// ---------------------------------------------------------------------------

TEST(operation, transformation_Geographic2D_offsets_to_PROJ_string) {

    auto transformation = Transformation::createGeographic2DOffsets(
        PropertyMap(), GeographicCRS::EPSG_4326, GeographicCRS::EPSG_4326,
        Angle(0.5), Angle(-1), {});
    EXPECT_TRUE(transformation->validateParameters().empty());

    EXPECT_EQ(
        transformation->exportToPROJString(PROJStringFormatter::create().get()),
        "+proj=pipeline +step +proj=axisswap +order=2,1 +step "
        "+proj=unitconvert +xy_in=deg +xy_out=rad +step +proj=geogoffset "
        "+dlat=1800 +dlon=-3600 +step +proj=unitconvert +xy_in=rad "
        "+xy_out=deg +step +proj=axisswap +order=2,1");
    EXPECT_EQ(transformation->inverse()->exportToPROJString(
                  PROJStringFormatter::create().get()),
              "+proj=pipeline +step +proj=axisswap +order=2,1 +step "
              "+proj=unitconvert +xy_in=deg +xy_out=rad +step +proj=geogoffset "
              "+dlat=-1800 +dlon=3600 +step +proj=unitconvert +xy_in=rad "
              "+xy_out=deg +step +proj=axisswap +order=2,1");
}

// ---------------------------------------------------------------------------

TEST(operation, transformation_Geographic3D_offsets_to_PROJ_string) {

    auto transformation = Transformation::createGeographic3DOffsets(
        PropertyMap(), GeographicCRS::EPSG_4326, GeographicCRS::EPSG_4326,
        Angle(0.5), Angle(-1), Length(2), {});
    EXPECT_TRUE(transformation->validateParameters().empty());

    EXPECT_EQ(
        transformation->exportToPROJString(PROJStringFormatter::create().get()),
        "+proj=pipeline +step +proj=axisswap +order=2,1 +step "
        "+proj=unitconvert +xy_in=deg +xy_out=rad +step +proj=geogoffset "
        "+dlat=1800 +dlon=-3600 +dh=2 +step +proj=unitconvert +xy_in=rad "
        "+xy_out=deg +step +proj=axisswap +order=2,1");
    EXPECT_EQ(transformation->inverse()->exportToPROJString(
                  PROJStringFormatter::create().get()),
              "+proj=pipeline +step +proj=axisswap +order=2,1 +step "
              "+proj=unitconvert +xy_in=deg +xy_out=rad +step +proj=geogoffset "
              "+dlat=-1800 +dlon=3600 +dh=-2 +step +proj=unitconvert "
              "+xy_in=rad +xy_out=deg +step +proj=axisswap +order=2,1");
}

// ---------------------------------------------------------------------------

TEST(operation,
     transformation_Geographic2D_with_height_offsets_to_PROJ_string) {

    auto transformation = Transformation::createGeographic2DWithHeightOffsets(
        PropertyMap(),
        CompoundCRS::create(PropertyMap(),
                            {GeographicCRS::EPSG_4326, createVerticalCRS()}),
        GeographicCRS::EPSG_4326, Angle(0.5), Angle(-1), Length(2), {});
    EXPECT_TRUE(transformation->validateParameters().empty());

    EXPECT_EQ(
        transformation->exportToPROJString(PROJStringFormatter::create().get()),
        "+proj=pipeline +step +proj=axisswap +order=2,1 +step "
        "+proj=unitconvert +xy_in=deg +xy_out=rad +step +proj=geogoffset "
        "+dlat=1800 +dlon=-3600 +dh=2 +step +proj=unitconvert +xy_in=rad "
        "+xy_out=deg +step +proj=axisswap +order=2,1");
    EXPECT_EQ(transformation->inverse()->exportToPROJString(
                  PROJStringFormatter::create().get()),
              "+proj=pipeline +step +proj=axisswap +order=2,1 +step "
              "+proj=unitconvert +xy_in=deg +xy_out=rad +step +proj=geogoffset "
              "+dlat=-1800 +dlon=3600 +dh=-2 +step +proj=unitconvert "
              "+xy_in=rad +xy_out=deg +step +proj=axisswap +order=2,1");
}

// ---------------------------------------------------------------------------

TEST(operation, transformation_vertical_offset_to_PROJ_string) {

    auto transformation = Transformation::createVerticalOffset(
        PropertyMap(), createVerticalCRS(), createVerticalCRS(), Length(1), {});
    EXPECT_TRUE(transformation->validateParameters().empty());

    EXPECT_EQ(
        transformation->exportToPROJString(PROJStringFormatter::create().get()),
        "+proj=geogoffset +dh=1");
    EXPECT_EQ(transformation->inverse()->exportToPROJString(
                  PROJStringFormatter::create().get()),
              "+proj=geogoffset +dh=-1");
}

// ---------------------------------------------------------------------------

TEST(operation, compoundCRS_with_boundVerticalCRS_to_geogCRS) {

    auto compound = CompoundCRS::create(
        PropertyMap(), std::vector<CRSNNPtr>{GeographicCRS::EPSG_4326,
                                             createBoundVerticalCRS()});
    auto op = CoordinateOperationFactory::create()->createOperation(
        compound, GeographicCRS::EPSG_4979);
    ASSERT_TRUE(op != nullptr);
    EXPECT_EQ(
        op->exportToPROJString(PROJStringFormatter::create().get()),
        "+proj=pipeline +step +proj=axisswap +order=2,1 +step "
        "+proj=unitconvert +xy_in=deg +xy_out=rad +step +proj=vgridshift "
        "+grids=us_nga_egm08_25.tif +multiplier=1 +step +proj=unitconvert "
        "+xy_in=rad +xy_out=deg +step +proj=axisswap +order=2,1");
}

// ---------------------------------------------------------------------------

TEST(operation, compoundCRS_with_boundGeogCRS_to_geogCRS) {

    auto geogCRS = GeographicCRS::create(
        PropertyMap(),
        GeodeticReferenceFrame::create(PropertyMap(), Ellipsoid::WGS84,
                                       optional<std::string>(),
                                       PrimeMeridian::GREENWICH),
        EllipsoidalCS::createLatitudeLongitude(UnitOfMeasure::DEGREE));
    auto horizBoundCRS = BoundCRS::createFromTOWGS84(
        geogCRS, std::vector<double>{1, 2, 3, 4, 5, 6, 7});
    auto compound = CompoundCRS::create(
        PropertyMap(),
        std::vector<CRSNNPtr>{horizBoundCRS, createVerticalCRS()});
    auto op = CoordinateOperationFactory::create()->createOperation(
        compound, GeographicCRS::EPSG_4979);
    ASSERT_TRUE(op != nullptr);
    EXPECT_EQ(op->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=pipeline "
              "+step +proj=axisswap +order=2,1 "
              "+step +proj=unitconvert +xy_in=deg +xy_out=rad "
              "+step +proj=push +v_3 "
              "+step +proj=cart +ellps=WGS84 "
              "+step +proj=helmert +x=1 +y=2 +z=3 +rx=4 +ry=5 +rz=6 +s=7 "
              "+convention=position_vector "
              "+step +inv +proj=cart +ellps=WGS84 "
              "+step +proj=pop +v_3 "
              "+step +proj=unitconvert +xy_in=rad +xy_out=deg "
              "+step +proj=axisswap +order=2,1");
}

// ---------------------------------------------------------------------------

TEST(operation, compoundCRS_with_boundGeogCRS_and_boundVerticalCRS_to_geogCRS) {

    auto horizBoundCRS = BoundCRS::createFromTOWGS84(
        GeographicCRS::EPSG_4807, std::vector<double>{1, 2, 3, 4, 5, 6, 7});
    auto compound = CompoundCRS::create(
        PropertyMap(),
        std::vector<CRSNNPtr>{horizBoundCRS, createBoundVerticalCRS()});
    auto op = CoordinateOperationFactory::create()->createOperation(
        compound, GeographicCRS::EPSG_4979);
    ASSERT_TRUE(op != nullptr);
    // Not completely sure the order of horizontal and vertical operations
    // makes sense
    EXPECT_EQ(op->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=pipeline "
              "+step +proj=axisswap +order=2,1 "
              "+step +proj=unitconvert +xy_in=grad +xy_out=rad "
              "+step +inv +proj=longlat +ellps=clrk80ign +pm=paris "
              "+step +proj=push +v_3 "
              "+step +proj=cart +ellps=clrk80ign "
              "+step +proj=helmert +x=1 +y=2 +z=3 +rx=4 +ry=5 +rz=6 +s=7 "
              "+convention=position_vector "
              "+step +inv +proj=cart +ellps=WGS84 "
              "+step +proj=pop +v_3 "
              "+step +proj=vgridshift +grids=us_nga_egm08_25.tif +multiplier=1 "
              "+step +proj=unitconvert +xy_in=rad +xy_out=deg "
              "+step +proj=axisswap +order=2,1");

    auto grids = op->gridsNeeded(DatabaseContext::create(), false);
    EXPECT_EQ(grids.size(), 1U);

    auto opInverse = CoordinateOperationFactory::create()->createOperation(
        GeographicCRS::EPSG_4979, compound);
    ASSERT_TRUE(opInverse != nullptr);
    EXPECT_TRUE(opInverse->inverse()->isEquivalentTo(op.get()));
}

// ---------------------------------------------------------------------------

TEST(operation, compoundCRS_with_boundProjCRS_and_boundVerticalCRS_to_geogCRS) {

    auto horizBoundCRS = BoundCRS::createFromTOWGS84(
        ProjectedCRS::create(
            PropertyMap(), GeographicCRS::EPSG_4807,
            Conversion::createUTM(PropertyMap(), 31, true),
            CartesianCS::createEastingNorthing(UnitOfMeasure::METRE)),
        std::vector<double>{1, 2, 3, 4, 5, 6, 7});
    auto compound = CompoundCRS::create(
        PropertyMap(),
        std::vector<CRSNNPtr>{horizBoundCRS, createBoundVerticalCRS()});
    auto op = CoordinateOperationFactory::create()->createOperation(
        compound, GeographicCRS::EPSG_4979);
    ASSERT_TRUE(op != nullptr);
    // Not completely sure the order of horizontal and vertical operations
    // makes sense
    EXPECT_EQ(op->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=pipeline "
              "+step +inv +proj=utm +zone=31 +ellps=clrk80ign +pm=paris "
              "+step +proj=push +v_3 "
              "+step +proj=cart +ellps=clrk80ign "
              "+step +proj=helmert +x=1 +y=2 +z=3 +rx=4 +ry=5 +rz=6 +s=7 "
              "+convention=position_vector "
              "+step +inv +proj=cart +ellps=WGS84 "
              "+step +proj=pop +v_3 "
              "+step +proj=vgridshift +grids=us_nga_egm08_25.tif +multiplier=1 "
              "+step +proj=unitconvert +xy_in=rad +xy_out=deg "
              "+step +proj=axisswap +order=2,1");

    auto opInverse = CoordinateOperationFactory::create()->createOperation(
        GeographicCRS::EPSG_4979, compound);
    ASSERT_TRUE(opInverse != nullptr);
    EXPECT_TRUE(opInverse->inverse()->isEquivalentTo(op.get()));
}

// ---------------------------------------------------------------------------

TEST(operation,
     compoundCRS_with_boundVerticalCRS_from_geoidgrids_with_m_to_geogCRS) {

    auto objSrc = PROJStringParser().createFromPROJString(
        "+proj=longlat +datum=WGS84 +geoidgrids=@foo.gtx +type=crs");
    auto src = nn_dynamic_pointer_cast<CRS>(objSrc);
    ASSERT_TRUE(src != nullptr);
    auto op = CoordinateOperationFactory::create()->createOperation(
        NN_NO_CHECK(src), GeographicCRS::EPSG_4979);
    ASSERT_TRUE(op != nullptr);
    EXPECT_EQ(op->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=pipeline "
              "+step +proj=unitconvert +xy_in=deg +xy_out=rad "
              "+step +proj=vgridshift +grids=@foo.gtx +multiplier=1 "
              "+step +proj=unitconvert +xy_in=rad +xy_out=deg "
              "+step +proj=axisswap +order=2,1");
}

// ---------------------------------------------------------------------------

TEST(operation,
     compoundCRS_with_boundVerticalCRS_from_geoidgrids_with_ftus_to_geogCRS) {

    auto objSrc = PROJStringParser().createFromPROJString(
        "+proj=longlat +datum=WGS84 +geoidgrids=@foo.gtx +vunits=us-ft "
        "+type=crs");
    auto src = nn_dynamic_pointer_cast<CRS>(objSrc);
    ASSERT_TRUE(src != nullptr);
    auto op = CoordinateOperationFactory::create()->createOperation(
        NN_NO_CHECK(src), GeographicCRS::EPSG_4979);
    ASSERT_TRUE(op != nullptr);
    EXPECT_EQ(
        op->exportToPROJString(PROJStringFormatter::create().get()),
        "+proj=pipeline "
        "+step +proj=unitconvert +xy_in=deg +z_in=us-ft +xy_out=rad +z_out=m "
        "+step +proj=vgridshift +grids=@foo.gtx +multiplier=1 "
        "+step +proj=unitconvert +xy_in=rad +xy_out=deg "
        "+step +proj=axisswap +order=2,1");
}

// ---------------------------------------------------------------------------

TEST(operation,
     compoundCRS_with_boundProjCRS_with_ftus_and_boundVerticalCRS_to_geogCRS) {

    auto wkt =
        "COMPD_CS[\"NAD_1983_StatePlane_Alabama_West_FIPS_0102_Feet + "
        "NAVD88 height - Geoid12B (US Feet)\",\n"
        "    PROJCS[\"NAD_1983_StatePlane_Alabama_West_FIPS_0102_Feet\",\n"
        "        GEOGCS[\"NAD83\",\n"
        "            DATUM[\"North_American_Datum_1983\",\n"
        "                SPHEROID[\"GRS 1980\",6378137,298.257222101,\n"
        "                    AUTHORITY[\"EPSG\",\"7019\"]],\n"
        "                TOWGS84[0,0,0,0,0,0,0],\n"
        "                AUTHORITY[\"EPSG\",\"6269\"]],\n"
        "            PRIMEM[\"Greenwich\",0],\n"
        "            UNIT[\"Degree\",0.0174532925199433]],\n"
        "        PROJECTION[\"Transverse_Mercator\"],\n"
        "        PARAMETER[\"latitude_of_origin\",30],\n"
        "        PARAMETER[\"central_meridian\",-87.5],\n"
        "        PARAMETER[\"scale_factor\",0.999933333333333],\n"
        "        PARAMETER[\"false_easting\",1968500],\n"
        "        PARAMETER[\"false_northing\",0],\n"
        "        UNIT[\"US survey foot\",0.304800609601219,\n"
        "            AUTHORITY[\"EPSG\",\"9003\"]],\n"
        "        AXIS[\"Easting\",EAST],\n"
        "        AXIS[\"Northing\",NORTH],\n"
        "        AUTHORITY[\"ESRI\",\"102630\"]],\n"
        "    VERT_CS[\"NAVD88 height (ftUS)\",\n"
        "        VERT_DATUM[\"North American Vertical Datum 1988\",2005,\n"
        "            EXTENSION[\"PROJ4_GRIDS\",\"foo.gtx\"],\n"
        "            AUTHORITY[\"EPSG\",\"5103\"]],\n"
        "        UNIT[\"US survey foot\",0.304800609601219,\n"
        "            AUTHORITY[\"EPSG\",\"9003\"]],\n"
        "        AXIS[\"Gravity-related height\",UP],\n"
        "        AUTHORITY[\"EPSG\",\"6360\"]]]";

    auto obj = WKTParser().createFromWKT(wkt);
    auto crs = nn_dynamic_pointer_cast<CompoundCRS>(obj);
    ASSERT_TRUE(crs != nullptr);
    auto op = CoordinateOperationFactory::create()->createOperation(
        NN_NO_CHECK(crs), GeographicCRS::EPSG_4979);
    ASSERT_TRUE(op != nullptr);

    EXPECT_EQ(op->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=pipeline "
              "+step +proj=unitconvert +xy_in=us-ft +xy_out=m "
              "+step +inv +proj=tmerc +lat_0=30 +lon_0=-87.5 "
              "+k=0.999933333333333 +x_0=600000 +y_0=0 +ellps=GRS80 "
              "+step +proj=unitconvert +z_in=us-ft +z_out=m "
              "+step +proj=vgridshift +grids=foo.gtx +multiplier=1 "
              "+step +proj=unitconvert +xy_in=rad +xy_out=deg "
              "+step +proj=axisswap +order=2,1");
}

// ---------------------------------------------------------------------------

TEST(operation,
     compoundCRS_with_boundVerticalCRS_from_grids_to_geogCRS_with_ftus_ctxt) {

    auto dbContext = DatabaseContext::create();

    const char *wktSrc =
        "COMPD_CS[\"NAD83 + NAVD88 height - Geoid12B (Meters)\",\n"
        "    GEOGCS[\"NAD83\",\n"
        "        DATUM[\"North_American_Datum_1983\",\n"
        "            SPHEROID[\"GRS 1980\",6378137,298.257222101,\n"
        "                AUTHORITY[\"EPSG\",\"7019\"]],\n"
        "            AUTHORITY[\"EPSG\",\"6269\"]],\n"
        "        PRIMEM[\"Greenwich\",0,\n"
        "            AUTHORITY[\"EPSG\",\"8901\"]],\n"
        "        UNIT[\"degree\",0.0174532925199433,\n"
        "            AUTHORITY[\"EPSG\",\"9122\"]],\n"
        "        AUTHORITY[\"EPSG\",\"4269\"]],\n"
        "    VERT_CS[\"NAVD88 height - Geoid12B (Meters)\",\n"
        "        VERT_DATUM[\"North American Vertical Datum 1988\",2005,\n"
        "            EXTENSION[\"PROJ4_GRIDS\",\"@foo.gtx\"],\n"
        "            AUTHORITY[\"EPSG\",\"5103\"]],\n"
        "        UNIT[\"metre\",1.0,\n"
        "            AUTHORITY[\"EPSG\",\"9001\"]],\n"
        "        AXIS[\"Gravity-related height\",UP],\n"
        "        AUTHORITY[\"EPSG\",\"5703\"]]]";
    auto objSrc =
        WKTParser().attachDatabaseContext(dbContext).createFromWKT(wktSrc);
    auto srcCRS = nn_dynamic_pointer_cast<CompoundCRS>(objSrc);
    ASSERT_TRUE(srcCRS != nullptr);

    const char *wktDst =
        "COMPD_CS[\"NAD83 + Ellipsoid (US Feet)\",\n"
        "    GEOGCS[\"NAD83\",\n"
        "        DATUM[\"North_American_Datum_1983\",\n"
        "            SPHEROID[\"GRS 1980\",6378137,298.257222101,\n"
        "                AUTHORITY[\"EPSG\",\"7019\"]],\n"
        "            AUTHORITY[\"EPSG\",\"6269\"]],\n"
        "        PRIMEM[\"Greenwich\",0,\n"
        "            AUTHORITY[\"EPSG\",\"8901\"]],\n"
        "        UNIT[\"degree\",0.0174532925199433,\n"
        "            AUTHORITY[\"EPSG\",\"9122\"]],\n"
        "        AUTHORITY[\"EPSG\",\"4269\"]],\n"
        "    VERT_CS[\"Ellipsoid (US Feet)\",\n"
        "        VERT_DATUM[\"Ellipsoid\",2002],\n"
        "        UNIT[\"US survey foot\",0.304800609601219,\n"
        "            AUTHORITY[\"EPSG\",\"9003\"]],\n"
        "        AXIS[\"Up\",UP]]]";
    auto objDst =
        WKTParser().attachDatabaseContext(dbContext).createFromWKT(wktDst);
    auto dstCRS = nn_dynamic_pointer_cast<GeographicCRS>(objDst);
    ASSERT_TRUE(dstCRS != nullptr);

    auto authFactory = AuthorityFactory::create(dbContext, "EPSG");
    auto ctxt = CoordinateOperationContext::create(authFactory, nullptr, 0.0);
    ctxt->setGridAvailabilityUse(
        CoordinateOperationContext::GridAvailabilityUse::
            IGNORE_GRID_AVAILABILITY);
    ctxt->setSpatialCriterion(
        CoordinateOperationContext::SpatialCriterion::PARTIAL_INTERSECTION);
    auto list = CoordinateOperationFactory::create()->createOperations(
        NN_NO_CHECK(srcCRS), NN_NO_CHECK(dstCRS), ctxt);
    ASSERT_GE(list.size(), 1U);
    EXPECT_EQ(list[0]->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=pipeline "
              "+step +proj=axisswap +order=2,1 "
              "+step +proj=unitconvert +xy_in=deg +xy_out=rad "
              "+step +proj=vgridshift +grids=@foo.gtx +multiplier=1 "
              "+step +proj=unitconvert +xy_in=rad +z_in=m "
              "+xy_out=deg +z_out=us-ft "
              "+step +proj=axisswap +order=2,1");
}

// ---------------------------------------------------------------------------

TEST(
    operation,
    compoundCRS_with_boundGeogCRS_boundVerticalCRS_from_grids_to_boundGeogCRS_with_ftus_ctxt) {

    // Variant of above but with TOWGS84 in source & target CRS

    auto dbContext = DatabaseContext::create();

    const char *wktSrc =
        "COMPD_CS[\"NAD83 + NAVD88 height - Geoid12B (Meters)\",\n"
        "    GEOGCS[\"NAD83\",\n"
        "        DATUM[\"North_American_Datum_1983\",\n"
        "            SPHEROID[\"GRS 1980\",6378137,298.257222101,\n"
        "                AUTHORITY[\"EPSG\",\"7019\"]],\n"
        "            TOWGS84[0,0,0,0,0,0,0],\n"
        "            AUTHORITY[\"EPSG\",\"6269\"]],\n"
        "        PRIMEM[\"Greenwich\",0,\n"
        "            AUTHORITY[\"EPSG\",\"8901\"]],\n"
        "        UNIT[\"degree\",0.0174532925199433,\n"
        "            AUTHORITY[\"EPSG\",\"9122\"]],\n"
        "        AUTHORITY[\"EPSG\",\"4269\"]],\n"
        "    VERT_CS[\"NAVD88 height - Geoid12B (Meters)\",\n"
        "        VERT_DATUM[\"North American Vertical Datum 1988\",2005,\n"
        "            EXTENSION[\"PROJ4_GRIDS\",\"@foo.gtx\"],\n"
        "            AUTHORITY[\"EPSG\",\"5103\"]],\n"
        "        UNIT[\"metre\",1.0,\n"
        "            AUTHORITY[\"EPSG\",\"9001\"]],\n"
        "        AXIS[\"Gravity-related height\",UP],\n"
        "        AUTHORITY[\"EPSG\",\"5703\"]]]";
    auto objSrc =
        WKTParser().attachDatabaseContext(dbContext).createFromWKT(wktSrc);
    auto srcCRS = nn_dynamic_pointer_cast<CompoundCRS>(objSrc);
    ASSERT_TRUE(srcCRS != nullptr);

    const char *wktDst =
        "COMPD_CS[\"NAD83 + Ellipsoid (US Feet)\",\n"
        "    GEOGCS[\"NAD83\",\n"
        "        DATUM[\"North_American_Datum_1983\",\n"
        "            SPHEROID[\"GRS 1980\",6378137,298.257222101,\n"
        "                AUTHORITY[\"EPSG\",\"7019\"]],\n"
        "            TOWGS84[0,0,0,0,0,0,0],\n"
        "            AUTHORITY[\"EPSG\",\"6269\"]],\n"
        "        PRIMEM[\"Greenwich\",0,\n"
        "            AUTHORITY[\"EPSG\",\"8901\"]],\n"
        "        UNIT[\"degree\",0.0174532925199433,\n"
        "            AUTHORITY[\"EPSG\",\"9122\"]],\n"
        "        AUTHORITY[\"EPSG\",\"4269\"]],\n"
        "    VERT_CS[\"Ellipsoid (US Feet)\",\n"
        "        VERT_DATUM[\"Ellipsoid\",2002],\n"
        "        UNIT[\"US survey foot\",0.304800609601219,\n"
        "            AUTHORITY[\"EPSG\",\"9003\"]],\n"
        "        AXIS[\"Up\",UP]]]";
    auto objDst =
        WKTParser().attachDatabaseContext(dbContext).createFromWKT(wktDst);
    auto dstCRS = nn_dynamic_pointer_cast<BoundCRS>(objDst);
    ASSERT_TRUE(dstCRS != nullptr);

    auto authFactory = AuthorityFactory::create(dbContext, "EPSG");
    auto ctxt = CoordinateOperationContext::create(authFactory, nullptr, 0.0);
    ctxt->setGridAvailabilityUse(
        CoordinateOperationContext::GridAvailabilityUse::
            IGNORE_GRID_AVAILABILITY);
    ctxt->setSpatialCriterion(
        CoordinateOperationContext::SpatialCriterion::PARTIAL_INTERSECTION);
    auto list = CoordinateOperationFactory::create()->createOperations(
        NN_NO_CHECK(srcCRS), NN_NO_CHECK(dstCRS), ctxt);
    ASSERT_EQ(list.size(), 1U);
    EXPECT_EQ(list[0]->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=pipeline "
              "+step +proj=axisswap +order=2,1 "
              "+step +proj=unitconvert +xy_in=deg +xy_out=rad "
              "+step +proj=vgridshift +grids=@foo.gtx +multiplier=1 "
              "+step +proj=unitconvert +xy_in=rad +z_in=m "
              "+xy_out=deg +z_out=us-ft "
              "+step +proj=axisswap +order=2,1");
}

// ---------------------------------------------------------------------------

TEST(
    operation,
    compoundCRS_with_boundVerticalCRS_from_grids_to_boundGeogCRS_with_ftus_ctxt) {

    // Variant of above but with TOWGS84 in target CRS only

    auto dbContext = DatabaseContext::create();

    const char *wktSrc =
        "COMPD_CS[\"NAD83 + NAVD88 height - Geoid12B (Meters)\",\n"
        "    GEOGCS[\"NAD83\",\n"
        "        DATUM[\"North_American_Datum_1983\",\n"
        "            SPHEROID[\"GRS 1980\",6378137,298.257222101,\n"
        "                AUTHORITY[\"EPSG\",\"7019\"]],\n"
        "            AUTHORITY[\"EPSG\",\"6269\"]],\n"
        "        PRIMEM[\"Greenwich\",0,\n"
        "            AUTHORITY[\"EPSG\",\"8901\"]],\n"
        "        UNIT[\"degree\",0.0174532925199433,\n"
        "            AUTHORITY[\"EPSG\",\"9122\"]],\n"
        "        AUTHORITY[\"EPSG\",\"4269\"]],\n"
        "    VERT_CS[\"NAVD88 height - Geoid12B (Meters)\",\n"
        "        VERT_DATUM[\"North American Vertical Datum 1988\",2005,\n"
        "            EXTENSION[\"PROJ4_GRIDS\",\"@foo.gtx\"],\n"
        "            AUTHORITY[\"EPSG\",\"5103\"]],\n"
        "        UNIT[\"metre\",1.0,\n"
        "            AUTHORITY[\"EPSG\",\"9001\"]],\n"
        "        AXIS[\"Gravity-related height\",UP],\n"
        "        AUTHORITY[\"EPSG\",\"5703\"]]]";
    auto objSrc =
        WKTParser().attachDatabaseContext(dbContext).createFromWKT(wktSrc);
    auto srcCRS = nn_dynamic_pointer_cast<CompoundCRS>(objSrc);
    ASSERT_TRUE(srcCRS != nullptr);

    const char *wktDst =
        "COMPD_CS[\"NAD83 + Ellipsoid (US Feet)\",\n"
        "    GEOGCS[\"NAD83\",\n"
        "        DATUM[\"North_American_Datum_1983\",\n"
        "            SPHEROID[\"GRS 1980\",6378137,298.257222101,\n"
        "                AUTHORITY[\"EPSG\",\"7019\"]],\n"
        "            TOWGS84[0,0,0,0,0,0,0],\n"
        "            AUTHORITY[\"EPSG\",\"6269\"]],\n"
        "        PRIMEM[\"Greenwich\",0,\n"
        "            AUTHORITY[\"EPSG\",\"8901\"]],\n"
        "        UNIT[\"degree\",0.0174532925199433,\n"
        "            AUTHORITY[\"EPSG\",\"9122\"]],\n"
        "        AUTHORITY[\"EPSG\",\"4269\"]],\n"
        "    VERT_CS[\"Ellipsoid (US Feet)\",\n"
        "        VERT_DATUM[\"Ellipsoid\",2002],\n"
        "        UNIT[\"US survey foot\",0.304800609601219,\n"
        "            AUTHORITY[\"EPSG\",\"9003\"]],\n"
        "        AXIS[\"Up\",UP]]]";
    auto objDst =
        WKTParser().attachDatabaseContext(dbContext).createFromWKT(wktDst);
    auto dstCRS = nn_dynamic_pointer_cast<BoundCRS>(objDst);
    ASSERT_TRUE(dstCRS != nullptr);

    auto authFactory = AuthorityFactory::create(dbContext, "EPSG");
    auto ctxt = CoordinateOperationContext::create(authFactory, nullptr, 0.0);
    ctxt->setGridAvailabilityUse(
        CoordinateOperationContext::GridAvailabilityUse::
            IGNORE_GRID_AVAILABILITY);
    ctxt->setSpatialCriterion(
        CoordinateOperationContext::SpatialCriterion::PARTIAL_INTERSECTION);
    auto list = CoordinateOperationFactory::create()->createOperations(
        NN_NO_CHECK(srcCRS), NN_NO_CHECK(dstCRS), ctxt);
    ASSERT_GE(list.size(), 1U);
    EXPECT_EQ(list[0]->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=pipeline "
              "+step +proj=axisswap +order=2,1 "
              "+step +proj=unitconvert +xy_in=deg +xy_out=rad "
              "+step +proj=vgridshift +grids=@foo.gtx +multiplier=1 "
              "+step +proj=unitconvert +xy_in=rad +z_in=m "
              "+xy_out=deg +z_out=us-ft "
              "+step +proj=axisswap +order=2,1");
}

// ---------------------------------------------------------------------------

TEST(operation,
     compoundCRS_with_bound_of_projected_and_bound_of_vertical_to_geog3D) {

    auto dbContext = DatabaseContext::create();

    const char *wktSrc =
        "COMPD_CS[\"TempTM + CGVD28 height - HT2_0\",\n"
        "    PROJCS[\"Custom\",\n"
        "        GEOGCS[\"NAD83(CSRS)\",\n"
        "            DATUM[\"NAD83_Canadian_Spatial_Reference_System\",\n"
        "                SPHEROID[\"GRS 1980\",6378137,298.257222101,\n"
        "                    AUTHORITY[\"EPSG\",\"7019\"]],\n"
        "                TOWGS84[0,0,0,0,0,0,0],\n"
        "                AUTHORITY[\"EPSG\",\"6140\"]],\n"
        "            PRIMEM[\"Greenwich\",0,\n"
        "                AUTHORITY[\"EPSG\",\"8901\"]],\n"
        "            UNIT[\"degree\",0.0174532925199433,\n"
        "                AUTHORITY[\"EPSG\",\"9122\"]],\n"
        "            AUTHORITY[\"EPSG\",\"4617\"]],\n"
        "        PROJECTION[\"Transverse_Mercator\"],\n"
        "        PARAMETER[\"latitude_of_origin\",49.351346659616],\n"
        "        PARAMETER[\"central_meridian\",-123.20266499149],\n"
        "        PARAMETER[\"scale_factor\",1],\n"
        "        PARAMETER[\"false_easting\",15307.188],\n"
        "        PARAMETER[\"false_northing\",6540.975],\n"
        "        UNIT[\"Meters\",1],\n"
        "        AXIS[\"Easting\",EAST],\n"
        "        AXIS[\"Northing\",NORTH]],\n"
        "    VERT_CS[\"CGVD28 height - HT2_0\",\n"
        "        VERT_DATUM[\"Canadian Geodetic Vertical Datum of "
        "1928\",2005,\n"
        "            EXTENSION[\"PROJ4_GRIDS\",\"HT2_0.gtx\"],\n"
        "            AUTHORITY[\"EPSG\",\"5114\"]],\n"
        "        UNIT[\"metre\",1,\n"
        "            AUTHORITY[\"EPSG\",\"9001\"]],\n"
        "        AXIS[\"Gravity-related height\",UP],\n"
        "        AUTHORITY[\"EPSG\",\"5713\"]]]";
    auto objSrc =
        WKTParser().attachDatabaseContext(dbContext).createFromWKT(wktSrc);
    auto srcCRS = nn_dynamic_pointer_cast<CompoundCRS>(objSrc);
    ASSERT_TRUE(srcCRS != nullptr);

    auto authFactoryEPSG = AuthorityFactory::create(dbContext, "EPSG");
    // NAD83(CSRS) 3D
    auto dstCRS = authFactoryEPSG->createCoordinateReferenceSystem("4955");

    auto ctxt =
        CoordinateOperationContext::create(authFactoryEPSG, nullptr, 0.0);
    ctxt->setGridAvailabilityUse(
        CoordinateOperationContext::GridAvailabilityUse::
            IGNORE_GRID_AVAILABILITY);
    ctxt->setSpatialCriterion(
        CoordinateOperationContext::SpatialCriterion::PARTIAL_INTERSECTION);
    auto list = CoordinateOperationFactory::create()->createOperations(
        NN_NO_CHECK(srcCRS), dstCRS, ctxt);
    ASSERT_EQ(list.size(), 1U);
    EXPECT_EQ(list[0]->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=pipeline "
              "+step +inv +proj=tmerc +lat_0=49.351346659616 "
              "+lon_0=-123.20266499149 +k=1 "
              "+x_0=15307.188 +y_0=6540.975 +ellps=GRS80 "
              "+step +proj=vgridshift +grids=HT2_0.gtx +multiplier=1 "
              "+step +proj=unitconvert +xy_in=rad +xy_out=deg "
              "+step +proj=axisswap +order=2,1");
}

// ---------------------------------------------------------------------------

TEST(operation,
     compoundCRS_with_boundGeogCRS_and_geoid_to_geodCRS_NAD2011_ctxt) {

    auto dbContext = DatabaseContext::create();

    const char *wktSrc =
        "COMPD_CS[\"NAD83 / California zone 5 (ftUS) + "
        "NAVD88 height - Geoid12B (ftUS)\","
        "   PROJCS[\"NAD83 / California zone 5 (ftUS)\","
        "       GEOGCS[\"NAD83\","
        "           DATUM[\"North_American_Datum_1983\","
        "               SPHEROID[\"GRS 1980\",6378137,298.257222101,"
        "                   AUTHORITY[\"EPSG\",\"7019\"]],"
        "               TOWGS84[0,0,0,0,0,0,0],"
        "               AUTHORITY[\"EPSG\",\"6269\"]],"
        "           PRIMEM[\"Greenwich\",0,"
        "               AUTHORITY[\"EPSG\",\"8901\"]],"
        "           UNIT[\"degree\",0.0174532925199433,"
        "               AUTHORITY[\"EPSG\",\"9122\"]],"
        "           AUTHORITY[\"EPSG\",\"4269\"]],"
        "       PROJECTION[\"Lambert_Conformal_Conic_2SP\"],"
        "       PARAMETER[\"standard_parallel_1\",35.46666666666667],"
        "       PARAMETER[\"standard_parallel_2\",34.03333333333333],"
        "       PARAMETER[\"latitude_of_origin\",33.5],"
        "       PARAMETER[\"central_meridian\",-118],"
        "       PARAMETER[\"false_easting\",6561666.667],"
        "       PARAMETER[\"false_northing\",1640416.667],"
        "       UNIT[\"US survey foot\",0.3048006096012192,"
        "           AUTHORITY[\"EPSG\",\"9003\"]],"
        "       AXIS[\"X\",EAST],"
        "       AXIS[\"Y\",NORTH],"
        "       AUTHORITY[\"EPSG\",\"2229\"]],"
        "VERT_CS[\"NAVD88 height - Geoid12B (ftUS)\","
        "   VERT_DATUM[\"North American Vertical Datum 1988\",2005,"
        "       AUTHORITY[\"EPSG\",\"5103\"]],"
        "   UNIT[\"US survey foot\",0.3048006096012192,"
        "       AUTHORITY[\"EPSG\",\"9003\"]],"
        "   AXIS[\"Gravity-related height\",UP],"
        "   AUTHORITY[\"EPSG\",\"6360\"]]]";
    auto objSrc =
        WKTParser().attachDatabaseContext(dbContext).createFromWKT(wktSrc);
    auto srcCRS = nn_dynamic_pointer_cast<CompoundCRS>(objSrc);
    ASSERT_TRUE(srcCRS != nullptr);

    auto authFactoryEPSG = AuthorityFactory::create(dbContext, "EPSG");
    // NAD83(2011) geocentric
    auto dstCRS = authFactoryEPSG->createCoordinateReferenceSystem("6317");

    auto authFactory = AuthorityFactory::create(dbContext, std::string());
    auto ctxt = CoordinateOperationContext::create(authFactory, nullptr, 0.0);
    ctxt->setGridAvailabilityUse(
        CoordinateOperationContext::GridAvailabilityUse::
            IGNORE_GRID_AVAILABILITY);
    ctxt->setSpatialCriterion(
        CoordinateOperationContext::SpatialCriterion::PARTIAL_INTERSECTION);
    auto list = CoordinateOperationFactory::create()->createOperations(
        NN_NO_CHECK(srcCRS), dstCRS, ctxt);
    bool found = false;
    for (const auto &op : list) {
        if (op->nameStr() ==
            "Inverse of unnamed + "
            "Transformation from NAD83 to WGS84 + "
            "Inverse of NAD83(2011) to WGS 84 (1) + "
            "Conversion from NAVD88 height (ftUS) to NAVD88 height + "
            "Inverse of NAD83(2011) to NAVD88 height (1) + "
            "Conversion from NAD83(2011) (geog3D) to NAD83(2011) "
            "(geocentric)") {
            found = true;
            EXPECT_EQ(
                op->exportToPROJString(PROJStringFormatter::create().get()),
                "+proj=pipeline "
                "+step +proj=unitconvert +xy_in=us-ft +xy_out=m "
                "+step +inv +proj=lcc +lat_0=33.5 +lon_0=-118 "
                "+lat_1=35.4666666666667 +lat_2=34.0333333333333 "
                "+x_0=2000000.0001016 +y_0=500000.0001016 +ellps=GRS80 "
                "+step +proj=unitconvert +z_in=us-ft +z_out=m "
                "+step +proj=vgridshift +grids=us_noaa_g2012bu0.tif "
                "+multiplier=1 "
                "+step +proj=cart +ellps=GRS80");
        }
    }
    EXPECT_TRUE(found);
    if (!found) {
        for (const auto &op : list) {
            std::cerr << op->nameStr() << std::endl;
        }
    }
}

// ---------------------------------------------------------------------------

TEST(operation, geocent_to_compoundCRS) {
    auto objSrc = PROJStringParser().createFromPROJString(
        "+proj=geocent +datum=WGS84 +units=m +type=crs");
    auto src = nn_dynamic_pointer_cast<CRS>(objSrc);
    ASSERT_TRUE(src != nullptr);
    auto objDst = PROJStringParser().createFromPROJString(
        "+proj=longlat +ellps=GRS67 +nadgrids=@foo.gsb +geoidgrids=@foo.gtx "
        "+geoid_crs=horizontal_crs "
        "+type=crs");
    auto dst = nn_dynamic_pointer_cast<CRS>(objDst);
    ASSERT_TRUE(dst != nullptr);
    auto op = CoordinateOperationFactory::create()->createOperation(
        NN_CHECK_ASSERT(src), NN_CHECK_ASSERT(dst));
    ASSERT_TRUE(op != nullptr);
    EXPECT_EQ(op->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=pipeline "
              "+step +inv +proj=cart +ellps=WGS84 "
              "+step +inv +proj=hgridshift +grids=@foo.gsb "
              "+step +inv +proj=vgridshift +grids=@foo.gtx +multiplier=1 "
              "+step +proj=unitconvert +xy_in=rad +xy_out=deg");
}

// ---------------------------------------------------------------------------

TEST(operation, geocent_to_compoundCRS_context) {
    auto authFactory =
        AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    // WGS84 geocentric
    auto src = authFactory->createCoordinateReferenceSystem("4978");
    auto objDst = PROJStringParser().createFromPROJString(
        "+proj=longlat +ellps=GRS67 +nadgrids=@foo.gsb +geoidgrids=@foo.gtx "
        "+geoid_crs=horizontal_crs "
        "+type=crs");
    auto dst = nn_dynamic_pointer_cast<CRS>(objDst);
    ASSERT_TRUE(dst != nullptr);
    auto ctxt = CoordinateOperationContext::create(authFactory, nullptr, 0.0);
    auto list = CoordinateOperationFactory::create()->createOperations(
        src, NN_CHECK_ASSERT(dst), ctxt);
    ASSERT_EQ(list.size(), 1U);
    EXPECT_EQ(list[0]->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=pipeline "
              "+step +inv +proj=cart +ellps=WGS84 "
              "+step +inv +proj=hgridshift +grids=@foo.gsb "
              "+step +inv +proj=vgridshift +grids=@foo.gtx +multiplier=1 "
              "+step +proj=unitconvert +xy_in=rad +xy_out=deg");
}

// ---------------------------------------------------------------------------

TEST(operation, compoundCRS_to_compoundCRS) {
    auto compound1 = CompoundCRS::create(
        PropertyMap(),
        std::vector<CRSNNPtr>{createUTM31_WGS84(), createVerticalCRS()});
    auto compound2 = CompoundCRS::create(
        PropertyMap(),
        std::vector<CRSNNPtr>{createUTM32_WGS84(), createVerticalCRS()});
    auto op = CoordinateOperationFactory::create()->createOperation(compound1,
                                                                    compound2);
    ASSERT_TRUE(op != nullptr);
    auto opRef = CoordinateOperationFactory::create()->createOperation(
        createUTM31_WGS84(), createUTM32_WGS84());
    ASSERT_TRUE(opRef != nullptr);
    EXPECT_TRUE(op->isEquivalentTo(opRef.get()));
}

// ---------------------------------------------------------------------------

TEST(operation, compoundCRS_to_compoundCRS_with_vertical_transform) {
    auto verticalCRS1 = createVerticalCRS();

    auto verticalCRS2 = VerticalCRS::create(
        PropertyMap(), VerticalReferenceFrame::create(PropertyMap()),
        VerticalCS::createGravityRelatedHeight(UnitOfMeasure::METRE));

    // Use of this type of transformation is a bit of nonsense here
    // since it should normally be used with NGVD29 and NAVD88 for VerticalCRS,
    // and NAD27/NAD83 as horizontal CRS...
    auto vtransformation = Transformation::createVERTCON(
        PropertyMap(), verticalCRS1, verticalCRS2, "bla.gtx",
        std::vector<PositionalAccuracyNNPtr>());

    auto compound1 = CompoundCRS::create(
        PropertyMap(),
        std::vector<CRSNNPtr>{
            ProjectedCRS::create(
                PropertyMap(), GeographicCRS::EPSG_4326,
                Conversion::createTransverseMercator(PropertyMap(), Angle(1),
                                                     Angle(2), Scale(3),
                                                     Length(4), Length(5)),
                CartesianCS::createEastingNorthing(UnitOfMeasure::METRE)),
            BoundCRS::create(verticalCRS1, verticalCRS2, vtransformation)});
    auto compound2 = CompoundCRS::create(
        PropertyMap(),
        std::vector<CRSNNPtr>{createUTM32_WGS84(), verticalCRS2});

    auto op = CoordinateOperationFactory::create()->createOperation(compound1,
                                                                    compound2);
    ASSERT_TRUE(op != nullptr);
    EXPECT_EQ(op->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=pipeline +step +inv +proj=tmerc +lat_0=1 +lon_0=2 +k=3 "
              "+x_0=4 +y_0=5 +ellps=WGS84 +step "
              "+proj=vgridshift +grids=bla.gtx +multiplier=0.001 +step "
              "+proj=utm +zone=32 "
              "+ellps=WGS84");
    {
        auto formatter = PROJStringFormatter::create();
        formatter->setUseApproxTMerc(true);
        EXPECT_EQ(
            op->exportToPROJString(formatter.get()),
            "+proj=pipeline +step +inv +proj=tmerc +approx +lat_0=1 +lon_0=2 "
            "+k=3 +x_0=4 +y_0=5 +ellps=WGS84 +step "
            "+proj=vgridshift +grids=bla.gtx +multiplier=0.001 +step "
            "+proj=utm +approx +zone=32 "
            "+ellps=WGS84");
    }
    {
        auto formatter = PROJStringFormatter::create();
        formatter->setUseApproxTMerc(true);
        EXPECT_EQ(
            op->inverse()->exportToPROJString(formatter.get()),
            "+proj=pipeline +step +inv +proj=utm +approx +zone=32 +ellps=WGS84 "
            "+step +inv +proj=vgridshift +grids=bla.gtx "
            "+multiplier=0.001 +step +proj=tmerc +approx +lat_0=1 +lon_0=2 "
            "+k=3 +x_0=4 +y_0=5 +ellps=WGS84");
    }

    auto opInverse = CoordinateOperationFactory::create()->createOperation(
        compound2, compound1);
    ASSERT_TRUE(opInverse != nullptr);
    {
        auto formatter = PROJStringFormatter::create();
        auto formatter2 = PROJStringFormatter::create();
        EXPECT_EQ(opInverse->inverse()->exportToPROJString(formatter.get()),
                  op->exportToPROJString(formatter2.get()));
    }
}

// ---------------------------------------------------------------------------

TEST(operation, compoundCRS_to_compoundCRS_with_bound_crs_in_horiz_and_vert) {
    auto objSrc = PROJStringParser().createFromPROJString(
        "+proj=longlat +ellps=GRS67 +nadgrids=@foo.gsb +geoidgrids=@foo.gtx "
        "+geoid_crs=horizontal_crs "
        "+type=crs");
    auto src = nn_dynamic_pointer_cast<CRS>(objSrc);
    ASSERT_TRUE(src != nullptr);
    auto objDst = PROJStringParser().createFromPROJString(
        "+proj=longlat +ellps=GRS80 +nadgrids=@bar.gsb +geoidgrids=@bar.gtx "
        "+geoid_crs=horizontal_crs "
        "+type=crs");
    auto dst = nn_dynamic_pointer_cast<CRS>(objDst);
    ASSERT_TRUE(dst != nullptr);
    auto op = CoordinateOperationFactory::create()->createOperation(
        NN_CHECK_ASSERT(src), NN_CHECK_ASSERT(dst));
    ASSERT_TRUE(op != nullptr);
    EXPECT_EQ(op->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=pipeline +step +proj=unitconvert +xy_in=deg +xy_out=rad "
              "+step +proj=vgridshift +grids=@foo.gtx +multiplier=1 "
              "+step +proj=hgridshift +grids=@foo.gsb "
              "+step +inv +proj=hgridshift +grids=@bar.gsb "
              "+step +inv +proj=vgridshift +grids=@bar.gtx +multiplier=1 "
              "+step +proj=unitconvert +xy_in=rad +xy_out=deg");
}

// ---------------------------------------------------------------------------

TEST(
    operation,
    compoundCRS_to_compoundCRS_with_bound_crs_in_horiz_and_vert_same_geoidgrids) {
    auto objSrc = PROJStringParser().createFromPROJString(
        "+proj=longlat +ellps=GRS67 +nadgrids=@foo.gsb +geoidgrids=@foo.gtx "
        "+geoid_crs=horizontal_crs "
        "+type=crs");
    auto src = nn_dynamic_pointer_cast<CRS>(objSrc);
    ASSERT_TRUE(src != nullptr);
    auto objDst = PROJStringParser().createFromPROJString(
        "+proj=longlat +ellps=GRS80 +nadgrids=@bar.gsb +geoidgrids=@foo.gtx "
        "+geoid_crs=horizontal_crs "
        "+type=crs");
    auto dst = nn_dynamic_pointer_cast<CRS>(objDst);
    ASSERT_TRUE(dst != nullptr);
    auto op = CoordinateOperationFactory::create()->createOperation(
        NN_CHECK_ASSERT(src), NN_CHECK_ASSERT(dst));
    ASSERT_TRUE(op != nullptr);
    EXPECT_EQ(op->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=pipeline "
              "+step +proj=unitconvert +xy_in=deg +xy_out=rad "
              "+step +proj=vgridshift +grids=@foo.gtx +multiplier=1 "
              "+step +proj=hgridshift +grids=@foo.gsb "
              "+step +inv +proj=hgridshift +grids=@bar.gsb "
              "+step +inv +proj=vgridshift +grids=@foo.gtx +multiplier=1 "
              "+step +proj=unitconvert +xy_in=rad +xy_out=deg");
}

// ---------------------------------------------------------------------------

TEST(
    operation,
    compoundCRS_to_compoundCRS_with_bound_crs_in_horiz_and_vert_same_geoidgrids_different_vunits) {
    auto objSrc = PROJStringParser().createFromPROJString(
        "+proj=longlat +ellps=GRS67 +nadgrids=@foo.gsb +geoidgrids=@foo.gtx "
        "+geoid_crs=horizontal_crs "
        "+type=crs");
    auto src = nn_dynamic_pointer_cast<CRS>(objSrc);
    ASSERT_TRUE(src != nullptr);
    auto objDst = PROJStringParser().createFromPROJString(
        "+proj=longlat +ellps=GRS80 +nadgrids=@bar.gsb +geoidgrids=@foo.gtx "
        "+geoid_crs=horizontal_crs "
        "+vunits=us-ft +type=crs");
    auto dst = nn_dynamic_pointer_cast<CRS>(objDst);
    ASSERT_TRUE(dst != nullptr);
    auto op = CoordinateOperationFactory::create()->createOperation(
        NN_CHECK_ASSERT(src), NN_CHECK_ASSERT(dst));
    ASSERT_TRUE(op != nullptr);
    EXPECT_EQ(op->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=pipeline "
              "+step +proj=unitconvert +xy_in=deg +xy_out=rad "
              "+step +proj=vgridshift +grids=@foo.gtx +multiplier=1 "
              "+step +proj=hgridshift +grids=@foo.gsb "
              "+step +inv +proj=hgridshift +grids=@bar.gsb "
              "+step +inv +proj=vgridshift +grids=@foo.gtx +multiplier=1 "
              "+step +proj=unitconvert +xy_in=rad +z_in=m "
              "+xy_out=deg +z_out=us-ft");
}

// ---------------------------------------------------------------------------

TEST(
    operation,
    compoundCRS_to_compoundCRS_with_bound_crs_in_horiz_and_vert_same_nadgrids_same_geoidgrids) {
    auto objSrc = PROJStringParser().createFromPROJString(
        "+proj=longlat +ellps=GRS67 +nadgrids=@foo.gsb +geoidgrids=@foo.gtx "
        "+geoid_crs=horizontal_crs "
        "+type=crs");
    auto src = nn_dynamic_pointer_cast<CRS>(objSrc);
    ASSERT_TRUE(src != nullptr);
    auto objDst = PROJStringParser().createFromPROJString(
        "+proj=longlat +ellps=GRS80 +nadgrids=@foo.gsb +geoidgrids=@foo.gtx "
        "+geoid_crs=horizontal_crs "
        "+type=crs");
    auto dst = nn_dynamic_pointer_cast<CRS>(objDst);
    ASSERT_TRUE(dst != nullptr);
    auto op = CoordinateOperationFactory::create()->createOperation(
        NN_CHECK_ASSERT(src), NN_CHECK_ASSERT(dst));
    ASSERT_TRUE(op != nullptr);
    EXPECT_EQ(op->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=noop");
}

// ---------------------------------------------------------------------------

TEST(
    operation,
    compoundCRS_to_compoundCRS_with_bound_crs_in_horiz_and_vert_same_towgs84_same_geoidgrids) {
    auto objSrc = PROJStringParser().createFromPROJString(
        "+proj=longlat +ellps=GRS67 +towgs84=0,0,0 +geoidgrids=@foo.gtx "
        "+geoid_crs=horizontal_crs "
        "+type=crs");
    auto src = nn_dynamic_pointer_cast<CRS>(objSrc);
    ASSERT_TRUE(src != nullptr);
    auto objDst = PROJStringParser().createFromPROJString(
        "+proj=longlat +ellps=GRS80 +towgs84=0,0,0 +geoidgrids=@foo.gtx "
        "+geoid_crs=horizontal_crs "
        "+type=crs");
    auto dst = nn_dynamic_pointer_cast<CRS>(objDst);
    ASSERT_TRUE(dst != nullptr);
    auto op = CoordinateOperationFactory::create()->createOperation(
        NN_CHECK_ASSERT(src), NN_CHECK_ASSERT(dst));
    ASSERT_TRUE(op != nullptr);
    EXPECT_EQ(op->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=pipeline "
              "+step +proj=unitconvert +xy_in=deg +xy_out=rad "
              "+step +proj=vgridshift +grids=@foo.gtx +multiplier=1 "
              "+step +proj=cart +ellps=GRS67 "
              "+step +inv +proj=cart +ellps=GRS80 "
              "+step +inv +proj=vgridshift +grids=@foo.gtx +multiplier=1 "
              "+step +proj=unitconvert +xy_in=rad +xy_out=deg");
}

// ---------------------------------------------------------------------------

TEST(
    operation,
    compoundCRS_to_compoundCRS_with_bound_crs_in_horiz_and_vert_same_ellsp_but_different_towgs84_different_geoidgrids) {
    auto objSrc = PROJStringParser().createFromPROJString(
        "+proj=longlat +ellps=GRS80 +towgs84=1,2,3 +geoidgrids=@foo.gtx "
        "+geoid_crs=horizontal_crs "
        "+type=crs");
    auto src = nn_dynamic_pointer_cast<CRS>(objSrc);
    ASSERT_TRUE(src != nullptr);
    auto objDst = PROJStringParser().createFromPROJString(
        "+proj=longlat +ellps=GRS80 +towgs84=4,5,6 +geoidgrids=@bar.gtx "
        "+geoid_crs=horizontal_crs "
        "+type=crs");
    auto dst = nn_dynamic_pointer_cast<CRS>(objDst);
    ASSERT_TRUE(dst != nullptr);

    auto srcGeog = src->extractGeographicCRS();
    ASSERT_TRUE(srcGeog != nullptr);
    ASSERT_TRUE(srcGeog->datum() != nullptr);
    auto dstGeog = dst->extractGeographicCRS();
    ASSERT_TRUE(dstGeog != nullptr);
    ASSERT_TRUE(dstGeog->datum() != nullptr);
    EXPECT_FALSE(srcGeog->datum()->isEquivalentTo(
        dstGeog->datum().get(), IComparable::Criterion::EQUIVALENT));

    auto op = CoordinateOperationFactory::create()->createOperation(
        NN_CHECK_ASSERT(src), NN_CHECK_ASSERT(dst));
    ASSERT_TRUE(op != nullptr);
    // Check there's no proj=push +v_1 +v_2
    EXPECT_EQ(op->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=pipeline "
              "+step +proj=unitconvert +xy_in=deg +xy_out=rad "
              "+step +proj=vgridshift +grids=@foo.gtx +multiplier=1 "
              "+step +proj=cart +ellps=GRS80 "
              "+step +proj=helmert +x=-3 +y=-3 +z=-3 "
              "+step +inv +proj=cart +ellps=GRS80 "
              "+step +inv +proj=vgridshift +grids=@bar.gtx +multiplier=1 "
              "+step +proj=unitconvert +xy_in=rad +xy_out=deg");
}

// ---------------------------------------------------------------------------

TEST(
    operation,
    compoundCRS_to_compoundCRS_with_bound_crs_in_horiz_and_vert_WKT1_same_geoidgrids_context) {
    auto objSrc = WKTParser().createFromWKT(
        "COMPD_CS[\"NAD83 / Alabama West + NAVD88 height - Geoid12B "
        "(Meters)\",\n"
        "    PROJCS[\"NAD83 / Alabama West\",\n"
        "        GEOGCS[\"NAD83\",\n"
        "            DATUM[\"North_American_Datum_1983\",\n"
        "                SPHEROID[\"GRS 1980\",6378137,298.257222101,\n"
        "                    AUTHORITY[\"EPSG\",\"7019\"]],\n"
        "                TOWGS84[0,0,0,0,0,0,0],\n"
        "                AUTHORITY[\"EPSG\",\"6269\"]],\n"
        "            PRIMEM[\"Greenwich\",0,\n"
        "                AUTHORITY[\"EPSG\",\"8901\"]],\n"
        "            UNIT[\"degree\",0.0174532925199433,\n"
        "                AUTHORITY[\"EPSG\",\"9122\"]],\n"
        "            AUTHORITY[\"EPSG\",\"4269\"]],\n"
        "        PROJECTION[\"Transverse_Mercator\"],\n"
        "        PARAMETER[\"latitude_of_origin\",30],\n"
        "        PARAMETER[\"central_meridian\",-87.5],\n"
        "        PARAMETER[\"scale_factor\",0.999933333],\n"
        "        PARAMETER[\"false_easting\",600000],\n"
        "        PARAMETER[\"false_northing\",0],\n"
        "        UNIT[\"metre\",1,\n"
        "            AUTHORITY[\"EPSG\",\"9001\"]],\n"
        "        AXIS[\"X\",EAST],\n"
        "        AXIS[\"Y\",NORTH],\n"
        "        AUTHORITY[\"EPSG\",\"26930\"]],\n"
        "    VERT_CS[\"NAVD88 height\",\n"
        "        VERT_DATUM[\"North American Vertical Datum 1988\",2005,\n"
        "            "
        "EXTENSION[\"PROJ4_GRIDS\",\"g2012a_alaska.gtx,g2012a_hawaii.gtx,"
        "g2012a_conus.gtx\"],\n"
        "            AUTHORITY[\"EPSG\",\"5103\"]],\n"
        "        UNIT[\"metre\",1,\n"
        "            AUTHORITY[\"EPSG\",\"9001\"]],\n"
        "        AXIS[\"Gravity-related height\",UP],\n"
        "        AUTHORITY[\"EPSG\",\"5703\"]]]");
    auto src = nn_dynamic_pointer_cast<CRS>(objSrc);
    ASSERT_TRUE(src != nullptr);
    auto objDst = WKTParser().createFromWKT(
        "COMPD_CS[\"NAD_1983_StatePlane_Alabama_West_FIPS_0102_Feet + NAVD88 "
        "height - Geoid12B (US Feet)\",\n"
        "    PROJCS[\"NAD_1983_StatePlane_Alabama_West_FIPS_0102_Feet\",\n"
        "        GEOGCS[\"NAD83\",\n"
        "            DATUM[\"North_American_Datum_1983\",\n"
        "                SPHEROID[\"GRS 1980\",6378137,298.257222101,\n"
        "                    AUTHORITY[\"EPSG\",\"7019\"]],\n"
        "                TOWGS84[0,0,0,0,0,0,0],\n"
        "                AUTHORITY[\"EPSG\",\"6269\"]],\n"
        "            PRIMEM[\"Greenwich\",0],\n"
        "            UNIT[\"Degree\",0.0174532925199433]],\n"
        "        PROJECTION[\"Transverse_Mercator\"],\n"
        "        PARAMETER[\"latitude_of_origin\",30],\n"
        "        PARAMETER[\"central_meridian\",-87.5],\n"
        "        PARAMETER[\"scale_factor\",0.999933333333333],\n"
        "        PARAMETER[\"false_easting\",1968500],\n"
        "        PARAMETER[\"false_northing\",0],\n"
        "        UNIT[\"US survey foot\",0.304800609601219,\n"
        "            AUTHORITY[\"EPSG\",\"9003\"]],\n"
        "        AXIS[\"Easting\",EAST],\n"
        "        AXIS[\"Northing\",NORTH],\n"
        "        AUTHORITY[\"ESRI\",\"102630\"]],\n"
        "    VERT_CS[\"NAVD88 height (ftUS)\",\n"
        "        VERT_DATUM[\"North American Vertical Datum 1988\",2005,\n"
        "            "
        "EXTENSION[\"PROJ4_GRIDS\",\"g2012a_alaska.gtx,g2012a_hawaii.gtx,"
        "g2012a_conus.gtx\"],\n"
        "            AUTHORITY[\"EPSG\",\"5103\"]],\n"
        "        UNIT[\"US survey foot\",0.304800609601219,\n"
        "            AUTHORITY[\"EPSG\",\"9003\"]],\n"
        "        AXIS[\"Gravity-related height\",UP],\n"
        "        AUTHORITY[\"EPSG\",\"6360\"]]]");
    auto dst = nn_dynamic_pointer_cast<CRS>(objDst);
    ASSERT_TRUE(dst != nullptr);

    auto dbContext = DatabaseContext::create();
    auto authFactory = AuthorityFactory::create(dbContext, "EPSG");
    auto ctxt = CoordinateOperationContext::create(authFactory, nullptr, 0.0);
    ctxt->setGridAvailabilityUse(
        CoordinateOperationContext::GridAvailabilityUse::
            IGNORE_GRID_AVAILABILITY);
    ctxt->setSpatialCriterion(
        CoordinateOperationContext::SpatialCriterion::PARTIAL_INTERSECTION);
    auto list = CoordinateOperationFactory::create()->createOperations(
        NN_CHECK_ASSERT(src), NN_CHECK_ASSERT(dst), ctxt);
    ASSERT_EQ(list.size(), 1U);
    EXPECT_EQ(list[0]->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=pipeline "
              "+step +inv +proj=tmerc +lat_0=30 +lon_0=-87.5 +k=0.999933333 "
              "+x_0=600000 +y_0=0 +ellps=GRS80 "
              "+step +proj=unitconvert +z_in=m +z_out=us-ft "
              "+step +proj=tmerc +lat_0=30 +lon_0=-87.5 +k=0.999933333333333 "
              "+x_0=600000 +y_0=0 +ellps=GRS80 "
              "+step +proj=unitconvert +xy_in=m +xy_out=us-ft");
}

// ---------------------------------------------------------------------------

TEST(operation, compoundCRS_to_compoundCRS_issue_2232) {
    auto objSrc = WKTParser().createFromWKT(
        "COMPD_CS[\"NAD83 / Alabama West + NAVD88 height - Geoid12B "
        "(Meters)\",\n"
        "    PROJCS[\"NAD83 / Alabama West\",\n"
        "        GEOGCS[\"NAD83\",\n"
        "            DATUM[\"North_American_Datum_1983\",\n"
        "                SPHEROID[\"GRS 1980\",6378137,298.257222101,\n"
        "                    AUTHORITY[\"EPSG\",\"7019\"]],\n"
        "                TOWGS84[0,0,0,0,0,0,0],\n"
        "                AUTHORITY[\"EPSG\",\"6269\"]],\n"
        "            PRIMEM[\"Greenwich\",0,\n"
        "                AUTHORITY[\"EPSG\",\"8901\"]],\n"
        "            UNIT[\"degree\",0.0174532925199433,\n"
        "                AUTHORITY[\"EPSG\",\"9122\"]],\n"
        "            AUTHORITY[\"EPSG\",\"4269\"]],\n"
        "        PROJECTION[\"Transverse_Mercator\"],\n"
        "        PARAMETER[\"latitude_of_origin\",30],\n"
        "        PARAMETER[\"central_meridian\",-87.5],\n"
        "        PARAMETER[\"scale_factor\",0.999933333],\n"
        "        PARAMETER[\"false_easting\",600000],\n"
        "        PARAMETER[\"false_northing\",0],\n"
        "        UNIT[\"metre\",1,\n"
        "            AUTHORITY[\"EPSG\",\"9001\"]],\n"
        "        AXIS[\"X\",EAST],\n"
        "        AXIS[\"Y\",NORTH],\n"
        "        AUTHORITY[\"EPSG\",\"26930\"]],\n"
        "    VERT_CS[\"NAVD88 height - Geoid12B (Meters)\",\n"
        "        VERT_DATUM[\"North American Vertical Datum 1988\",2005,\n"
        "            EXTENSION[\"PROJ4_GRIDS\",\"foo.gtx\"],\n"
        "            AUTHORITY[\"EPSG\",\"5103\"]],\n"
        "        UNIT[\"metre\",1.0,\n"
        "            AUTHORITY[\"EPSG\",\"9001\"]],\n"
        "        AXIS[\"Gravity-related height\",UP],\n"
        "        AUTHORITY[\"EPSG\",\"5703\"]]]");
    auto src = nn_dynamic_pointer_cast<CRS>(objSrc);
    ASSERT_TRUE(src != nullptr);

    auto objDst = WKTParser().createFromWKT(
        "COMPD_CS[\"NAD83 + some CRS (US Feet)\",\n"
        "    GEOGCS[\"NAD83\",\n"
        "        DATUM[\"North_American_Datum_1983\",\n"
        "            SPHEROID[\"GRS 1980\",6378137,298.257222101,\n"
        "                AUTHORITY[\"EPSG\",\"7019\"]],\n"
        "            TOWGS84[0,0,0,0,0,0,0],\n"
        "            AUTHORITY[\"EPSG\",\"6269\"]],\n"
        "        PRIMEM[\"Greenwich\",0,\n"
        "            AUTHORITY[\"EPSG\",\"8901\"]],\n"
        "        UNIT[\"degree\",0.0174532925199433,\n"
        "            AUTHORITY[\"EPSG\",\"9122\"]],\n"
        "        AUTHORITY[\"EPSG\",\"4269\"]],\n"
        "    VERT_CS[\"some CRS (US Feet)\",\n"
        "        VERT_DATUM[\"some datum\",2005],\n"
        "        UNIT[\"US survey foot\",0.3048006096012192,\n"
        "            AUTHORITY[\"EPSG\",\"9003\"]],\n"
        "        AXIS[\"Up\",UP]]]");
    auto dst = nn_dynamic_pointer_cast<CRS>(objDst);
    ASSERT_TRUE(dst != nullptr);

    auto dbContext = DatabaseContext::create();
    auto authFactory = AuthorityFactory::create(dbContext, "EPSG");
    auto ctxt = CoordinateOperationContext::create(authFactory, nullptr, 0.0);
    ctxt->setGridAvailabilityUse(
        CoordinateOperationContext::GridAvailabilityUse::
            IGNORE_GRID_AVAILABILITY);
    ctxt->setSpatialCriterion(
        CoordinateOperationContext::SpatialCriterion::PARTIAL_INTERSECTION);

    auto list = CoordinateOperationFactory::create()->createOperations(
        NN_CHECK_ASSERT(src), NN_CHECK_ASSERT(dst), ctxt);
    EXPECT_GE(list.size(), 1U);

    auto list2 = CoordinateOperationFactory::create()->createOperations(
        NN_CHECK_ASSERT(dst), NN_CHECK_ASSERT(src), ctxt);
    EXPECT_EQ(list2.size(), list.size());
}

// ---------------------------------------------------------------------------

TEST(operation, compoundCRS_to_compoundCRS_context) {
    auto authFactory =
        AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    auto ctxt = CoordinateOperationContext::create(authFactory, nullptr, 0.0);
    ctxt->setGridAvailabilityUse(
        CoordinateOperationContext::GridAvailabilityUse::
            IGNORE_GRID_AVAILABILITY);
    ctxt->setSpatialCriterion(
        CoordinateOperationContext::SpatialCriterion::PARTIAL_INTERSECTION);
    auto list = CoordinateOperationFactory::create()->createOperations(
        // NAD27 + NGVD29 height (ftUS)
        authFactory->createCoordinateReferenceSystem("7406"),
        // NAD83(NSRS2007) + NAVD88 height
        authFactory->createCoordinateReferenceSystem("5500"), ctxt);
    // 152 or 155 depending if the VERTCON grids are there
    ASSERT_GE(list.size(), 152U);
    EXPECT_FALSE(list[0]->hasBallparkTransformation());
    EXPECT_EQ(list[0]->nameStr(), "NGVD29 height (ftUS) to NAVD88 height (3) + "
                                  "NAD27 to WGS 84 (79) + Inverse of "
                                  "NAD83(NSRS2007) to WGS 84 (1)");
    EXPECT_EQ(
        list[0]->exportToPROJString(PROJStringFormatter::create().get()),
        "+proj=pipeline +step +proj=axisswap +order=2,1 +step "
        "+proj=unitconvert +xy_in=deg +z_in=us-ft +xy_out=rad +z_out=m "
        "+step +proj=vgridshift +grids=us_noaa_vertcone.tif +multiplier=1 "
        "+step +proj=hgridshift +grids=us_noaa_conus.tif +step "
        "+proj=unitconvert +xy_in=rad +xy_out=deg +step +proj=axisswap "
        "+order=2,1");
    {
        // Test that we can round-trip this through WKT and still get the same
        // PROJ string.
        auto wkt = list[0]->exportToWKT(
            WKTFormatter::create(WKTFormatter::Convention::WKT2_2019).get());
        auto obj = WKTParser().createFromWKT(wkt);
        auto co = nn_dynamic_pointer_cast<CoordinateOperation>(obj);
        ASSERT_TRUE(co != nullptr);
        EXPECT_EQ(
            list[0]->exportToPROJString(PROJStringFormatter::create().get()),
            co->exportToPROJString(PROJStringFormatter::create().get()));
    }

    bool foundApprox = false;
    for (size_t i = 0; i < list.size(); i++) {
        auto projString =
            list[i]->exportToPROJString(PROJStringFormatter::create().get());
        EXPECT_TRUE(
            projString.find("+proj=pipeline +step +proj=axisswap +order=2,1 "
                            "+step +proj=unitconvert +xy_in=deg +z_in=us-ft "
                            "+xy_out=rad +z_out=m") == 0)
            << list[i]->nameStr();
        if (list[i]->nameStr().find("Transformation from NGVD29 height (ftUS) "
                                    "to NAVD88 height (ballpark vertical "
                                    "transformation)") == 0) {
            EXPECT_TRUE(list[i]->hasBallparkTransformation());
            EXPECT_EQ(list[i]->nameStr(),
                      "Transformation from NGVD29 height (ftUS) to NAVD88 "
                      "height (ballpark vertical transformation) + NAD27 to "
                      "WGS 84 (79) + Inverse of NAD83(NSRS2007) to WGS 84 (1)");
            EXPECT_EQ(
                projString,
                "+proj=pipeline +step +proj=axisswap +order=2,1 +step "
                "+proj=unitconvert +xy_in=deg +z_in=us-ft +xy_out=rad "
                "+z_out=m +step +proj=hgridshift +grids=us_noaa_conus.tif "
                "+step +proj=unitconvert +xy_in=rad "
                "+xy_out=deg +step +proj=axisswap +order=2,1");
            foundApprox = true;
            break;
        }
    }
    EXPECT_TRUE(foundApprox);
}

// ---------------------------------------------------------------------------

TEST(operation, compoundCRS_to_compoundCRS_context_helmert_noop) {
    auto dbContext = DatabaseContext::create();
    auto authFactory = AuthorityFactory::create(dbContext, "EPSG");
    auto ctxt = CoordinateOperationContext::create(authFactory, nullptr, 0.0);
    ctxt->setGridAvailabilityUse(
        CoordinateOperationContext::GridAvailabilityUse::
            IGNORE_GRID_AVAILABILITY);
    ctxt->setSpatialCriterion(
        CoordinateOperationContext::SpatialCriterion::PARTIAL_INTERSECTION);
    // WGS84 + EGM96
    auto objSrc = createFromUserInput("EPSG:4326+5773", dbContext);
    auto srcCrs = nn_dynamic_pointer_cast<CompoundCRS>(objSrc);
    ASSERT_TRUE(srcCrs != nullptr);
    // ETRS89 + EGM96
    auto objDest = createFromUserInput("EPSG:4258+5773", dbContext);
    auto destCrs = nn_dynamic_pointer_cast<CompoundCRS>(objDest);
    ASSERT_TRUE(destCrs != nullptr);
    auto list = CoordinateOperationFactory::create()->createOperations(
        NN_NO_CHECK(srcCrs), NN_NO_CHECK(destCrs), ctxt);
    ASSERT_GE(list.size(), 1U);
    EXPECT_EQ(list[0]->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=noop");
}

// ---------------------------------------------------------------------------

// EGM96 has a geoid model referenced to WGS84, and Belfast height has a
// geoid model referenced to ETRS89
TEST(operation, compoundCRS_to_compoundCRS_WGS84_EGM96_to_ETRS89_Belfast) {
    auto dbContext = DatabaseContext::create();
    auto authFactory = AuthorityFactory::create(dbContext, "EPSG");
    auto ctxt = CoordinateOperationContext::create(authFactory, nullptr, 0.0);
    ctxt->setGridAvailabilityUse(
        CoordinateOperationContext::GridAvailabilityUse::
            IGNORE_GRID_AVAILABILITY);
    ctxt->setSpatialCriterion(
        CoordinateOperationContext::SpatialCriterion::PARTIAL_INTERSECTION);
    // WGS84 + EGM96
    auto objSrc = createFromUserInput("EPSG:4326+5773", dbContext);
    auto srcCrs = nn_dynamic_pointer_cast<CompoundCRS>(objSrc);
    ASSERT_TRUE(srcCrs != nullptr);
    // ETRS89 + Belfast height
    auto objDest = createFromUserInput("EPSG:4258+5732", dbContext);
    auto destCrs = nn_dynamic_pointer_cast<CompoundCRS>(objDest);
    ASSERT_TRUE(destCrs != nullptr);
    auto list = CoordinateOperationFactory::create()->createOperations(
        NN_NO_CHECK(srcCrs), NN_NO_CHECK(destCrs), ctxt);
    ASSERT_GE(list.size(), 1U);
    EXPECT_EQ(list[0]->nameStr(), "Inverse of WGS 84 to EGM96 height (1) + "
                                  "Inverse of ETRS89 to WGS 84 (1) + "
                                  "ETRS89 to Belfast height (2)");
    EXPECT_EQ(list[0]->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=pipeline +step +proj=axisswap +order=2,1 "
              "+step +proj=unitconvert +xy_in=deg +xy_out=rad "
              "+step +proj=vgridshift +grids=us_nga_egm96_15.tif +multiplier=1 "
              "+step +inv +proj=vgridshift +grids=uk_os_OSGM15_Belfast.tif "
              "+multiplier=1 +step "
              "+proj=unitconvert +xy_in=rad +xy_out=deg "
              "+step +proj=axisswap +order=2,1");
}

// ---------------------------------------------------------------------------

// Variant of above where source intermediate geog3D CRS == target intermediate
// geog3D CRS
TEST(operation, compoundCRS_to_compoundCRS_WGS84_EGM96_to_WGS84_Belfast) {
    auto dbContext = DatabaseContext::create();
    auto authFactory = AuthorityFactory::create(dbContext, "EPSG");
    auto ctxt = CoordinateOperationContext::create(authFactory, nullptr, 0.0);
    ctxt->setGridAvailabilityUse(
        CoordinateOperationContext::GridAvailabilityUse::
            IGNORE_GRID_AVAILABILITY);
    ctxt->setSpatialCriterion(
        CoordinateOperationContext::SpatialCriterion::PARTIAL_INTERSECTION);
    // WGS84 + EGM96
    auto objSrc = createFromUserInput("EPSG:4326+5773", dbContext);
    auto srcCrs = nn_dynamic_pointer_cast<CompoundCRS>(objSrc);
    ASSERT_TRUE(srcCrs != nullptr);
    // WGS84 + Belfast height
    auto objDest = createFromUserInput("EPSG:4326+5732", dbContext);
    auto destCrs = nn_dynamic_pointer_cast<CompoundCRS>(objDest);
    ASSERT_TRUE(destCrs != nullptr);
    auto list = CoordinateOperationFactory::create()->createOperations(
        NN_NO_CHECK(srcCrs), NN_NO_CHECK(destCrs), ctxt);
    ASSERT_GE(list.size(), 1U);
    EXPECT_EQ(list[0]->nameStr(),
              "Inverse of WGS 84 to EGM96 height (1) + "
              "ETRS89 to Belfast height (2) using ETRS89 to WGS 84 (1)");
    EXPECT_EQ(list[0]->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=pipeline +step +proj=axisswap +order=2,1 "
              "+step +proj=unitconvert +xy_in=deg +xy_out=rad "
              "+step +proj=vgridshift +grids=us_nga_egm96_15.tif +multiplier=1 "
              "+step +inv +proj=vgridshift +grids=uk_os_OSGM15_Belfast.tif "
              "+multiplier=1 +step "
              "+proj=unitconvert +xy_in=rad +xy_out=deg "
              "+step +proj=axisswap +order=2,1");
}

// ---------------------------------------------------------------------------

TEST(operation,
     compoundCRS_to_compoundCRS_OSGB36_BNG_ODN_height_to_WGS84_EGM96) {
    auto dbContext = DatabaseContext::create();
    auto authFactory = AuthorityFactory::create(dbContext, std::string());
    auto ctxt = CoordinateOperationContext::create(authFactory, nullptr, 0.0);
    ctxt->setGridAvailabilityUse(
        CoordinateOperationContext::GridAvailabilityUse::
            IGNORE_GRID_AVAILABILITY);
    // "OSGB36 / British National Grid + ODN height
    auto srcObj = createFromUserInput("EPSG:27700+5701", dbContext, false);
    auto src = nn_dynamic_pointer_cast<CRS>(srcObj);
    ASSERT_TRUE(src != nullptr);
    auto authFactoryEPSG =
        AuthorityFactory::create(dbContext, std::string("EPSG"));
    auto dst = authFactoryEPSG->createCoordinateReferenceSystem(
        "9707"); // "WGS 84 + EGM96 height"

    {
        auto list = CoordinateOperationFactory::create()->createOperations(
            NN_NO_CHECK(src), dst, ctxt);
        ASSERT_GE(list.size(), 1U);
        EXPECT_EQ(list[0]->nameStr(), "Inverse of British National Grid + "
                                      "OSGB36 to ETRS89 (2) + "
                                      "Inverse of ETRS89 to ODN height (2) + "
                                      "ETRS89 to WGS 84 (1) + "
                                      "WGS 84 to EGM96 height (1)");
        const char *expected_proj =
            "+proj=pipeline "
            "+step +inv +proj=tmerc +lat_0=49 +lon_0=-2 +k=0.9996012717 "
            "+x_0=400000 +y_0=-100000 +ellps=airy "
            "+step +proj=hgridshift +grids=uk_os_OSTN15_NTv2_OSGBtoETRS.tif "
            "+step +proj=vgridshift +grids=uk_os_OSGM15_GB.tif +multiplier=1 "
            "+step +inv +proj=vgridshift +grids=us_nga_egm96_15.tif "
            "+multiplier=1 "
            "+step +proj=unitconvert +xy_in=rad +xy_out=deg "
            "+step +proj=axisswap +order=2,1";
        EXPECT_EQ(
            list[0]->exportToPROJString(PROJStringFormatter::create().get()),
            expected_proj);
    }

    {
        auto list = CoordinateOperationFactory::create()->createOperations(
            dst, NN_NO_CHECK(src), ctxt);
        ASSERT_GE(list.size(), 1U);
        EXPECT_EQ(list[0]->nameStr(), "Inverse of WGS 84 to EGM96 height (1) + "
                                      "Inverse of ETRS89 to WGS 84 (1) + "
                                      "ETRS89 to ODN height (2) + "
                                      "Inverse of OSGB36 to ETRS89 (2) + "
                                      "British National Grid");
        const char *expected_proj =
            "+proj=pipeline "
            "+step +proj=axisswap +order=2,1 "
            "+step +proj=unitconvert +xy_in=deg +xy_out=rad "
            "+step +proj=vgridshift +grids=us_nga_egm96_15.tif +multiplier=1 "
            "+step +inv +proj=vgridshift +grids=uk_os_OSGM15_GB.tif "
            "+multiplier=1 "
            "+step +inv +proj=hgridshift "
            "+grids=uk_os_OSTN15_NTv2_OSGBtoETRS.tif "
            "+step +proj=tmerc +lat_0=49 +lon_0=-2 +k=0.9996012717 "
            "+x_0=400000 +y_0=-100000 +ellps=airy";
        EXPECT_EQ(
            list[0]->exportToPROJString(PROJStringFormatter::create().get()),
            expected_proj);
    }
}

// ---------------------------------------------------------------------------

TEST(
    operation,
    compoundCRS_to_compoundCRS_concatenated_operation_with_two_vert_transformation) {
    auto authFactory =
        AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    {
        auto ctxt =
            CoordinateOperationContext::create(authFactory, nullptr, 0.0);
        ctxt->setSpatialCriterion(
            CoordinateOperationContext::SpatialCriterion::PARTIAL_INTERSECTION);
        auto list = CoordinateOperationFactory::create()->createOperations(
            // ETRS89 + Baltic 1957 height
            authFactory->createCoordinateReferenceSystem("8360"),
            // ETRS89 + EVRF2007 height
            authFactory->createCoordinateReferenceSystem("7423"), ctxt);
        ASSERT_GE(list.size(), 2U);

        // For Czechia
        EXPECT_EQ(
            list[0]->exportToPROJString(PROJStringFormatter::create().get()),
            "+proj=pipeline "
            "+step +proj=axisswap +order=2,1 "
            "+step +proj=unitconvert +xy_in=deg +xy_out=rad "
            "+step +proj=vertoffset +lat_0=49.9166666666667 "
            "+lon_0=15.25 +dh=0.13 +slope_lat=0.026 +slope_lon=0 "
            "+step +proj=unitconvert +xy_in=rad +xy_out=deg "
            "+step +proj=axisswap +order=2,1");
        EXPECT_EQ(list[0]->nameStr(),
                  "Baltic 1957 height to EVRF2007 height (1)");

        // For Slovakia
        EXPECT_EQ(
            list[1]->exportToPROJString(PROJStringFormatter::create().get()),
            "+proj=pipeline "
            "+step +proj=axisswap +order=2,1 "
            "+step +proj=unitconvert +xy_in=deg +xy_out=rad "
            "+step +proj=vgridshift "
            "+grids=sk_gku_Slovakia_ETRS89h_to_Baltic1957.tif +multiplier=1 "
            "+step +inv +proj=vgridshift "
            "+grids=sk_gku_Slovakia_ETRS89h_to_EVRF2007.tif +multiplier=1 "
            "+step +proj=unitconvert +xy_in=rad +xy_out=deg "
            "+step +proj=axisswap +order=2,1");
        EXPECT_EQ(
            list[1]->nameStr(),
            "ETRS89 + Baltic 1957 height to ETRS89 + EVRF2007 height (1)");
        EXPECT_EQ(list[1]->inverse()->nameStr(), "Inverse of 'ETRS89 + Baltic "
                                                 "1957 height to ETRS89 + "
                                                 "EVRF2007 height (1)'");
    }
}

// ---------------------------------------------------------------------------

TEST(
    operation,
    compoundCRS_to_compoundCRS_concatenated_operation_with_two_vert_transformation_and_different_source_dest_interp) {
    auto authFactory =
        AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    // "BD72 + Ostend height"
    auto srcObj = createFromUserInput("EPSG:4313+5710",
                                      authFactory->databaseContext(), false);
    auto src = nn_dynamic_pointer_cast<CRS>(srcObj);
    ASSERT_TRUE(src != nullptr);

    // "Amersfoort + NAP height"
    auto dstObj = createFromUserInput("EPSG:4289+5709",
                                      authFactory->databaseContext(), false);
    auto dst = nn_dynamic_pointer_cast<CRS>(dstObj);
    ASSERT_TRUE(dst != nullptr);

    auto ctxt = CoordinateOperationContext::create(authFactory, nullptr, 0.0);
    ctxt->setGridAvailabilityUse(
        CoordinateOperationContext::GridAvailabilityUse::
            IGNORE_GRID_AVAILABILITY);
    ctxt->setSpatialCriterion(
        CoordinateOperationContext::SpatialCriterion::PARTIAL_INTERSECTION);
    auto list = CoordinateOperationFactory::create()->createOperations(
        NN_NO_CHECK(src), NN_NO_CHECK(dst), ctxt);
    ASSERT_GE(list.size(), 1U);
    EXPECT_EQ(list[0]->nameStr(), "BD72 to ETRS89 (3) + "
                                  "Inverse of ETRS89 to Ostend height (1) + "
                                  "ETRS89 to NAP height (2) + "
                                  "Inverse of Amersfoort to ETRS89 (9)");
    EXPECT_EQ(list[0]->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=pipeline "
              "+step +proj=axisswap +order=2,1 "
              "+step +proj=unitconvert +xy_in=deg +xy_out=rad "
              "+step +proj=hgridshift +grids=be_ign_bd72lb72_etrs89lb08.tif "
              "+step +proj=vgridshift +grids=be_ign_hBG18.tif +multiplier=1 "
              "+step +inv +proj=vgridshift +grids=nl_nsgi_nlgeo2018.tif "
              "+multiplier=1 "
              "+step +inv +proj=hgridshift +grids=nl_nsgi_rdtrans2018.tif "
              "+step +proj=unitconvert +xy_in=rad +xy_out=deg "
              "+step +proj=axisswap +order=2,1");
}

// ---------------------------------------------------------------------------

TEST(operation, compoundCRS_to_compoundCRS_issue_2720) {
    auto dbContext = DatabaseContext::create();
    auto objSrc = WKTParser().attachDatabaseContext(dbContext).createFromWKT(
        "COMPD_CS[\"Orthographic + EGM96 geoid height\","
        "PROJCS[\"Orthographic\","
        "GEOGCS[\"GCS_WGS_1984\","
        "DATUM[\"D_unknown\","
        "SPHEROID[\"WGS84\",6378137,298.257223563]],"
        "PRIMEM[\"Greenwich\",0],UNIT[\"Degree\",0.017453292519943295]],"
        "PROJECTION[\"Orthographic\"],"
        "PARAMETER[\"Latitude_Of_Center\",36.1754430555555000],"
        "PARAMETER[\"Longitude_Of_Center\",-86.7740944444444000],"
        "PARAMETER[\"false_easting\",0],"
        "PARAMETER[\"false_northing\",0],"
        "UNIT[\"Meter\",1]],"
        "VERT_CS[\"EGM96 geoid height\","
        "VERT_DATUM[\"EGM96 geoid\",2005,"
        "EXTENSION[\"PROJ4_GRIDS\",\"egm96_15.gtx\"],"
        "AUTHORITY[\"EPSG\",\"5171\"]],"
        "UNIT[\"metre\",1,"
        "AUTHORITY[\"EPSG\",\"9001\"]],"
        "AXIS[\"Up\",UP],"
        "AUTHORITY[\"EPSG\",\"5773\"]]]");
    auto src = nn_dynamic_pointer_cast<CRS>(objSrc);
    ASSERT_TRUE(src != nullptr);

    auto objDst = WKTParser().attachDatabaseContext(dbContext).createFromWKT(
        "COMPD_CS[\"WGS84 Coordinate System + EGM96 geoid height\","
        "GEOGCS[\"WGS84 Coordinate System\","
        "DATUM[\"WGS 1984\","
        "SPHEROID[\"WGS 1984\",6378137,298.257223563],"
        "TOWGS84[0,0,0,0,0,0,0],"
        "AUTHORITY[\"EPSG\",\"6326\"]],"
        "PRIMEM[\"Greenwich\",0],"
        "UNIT[\"degree\",0.0174532925199433],"
        "AUTHORITY[\"EPSG\",\"4326\"]],"
        "VERT_CS[\"EGM96 geoid height\","
        "VERT_DATUM[\"EGM96 geoid\",2005,"
        "EXTENSION[\"PROJ4_GRIDS\",\"egm96_15.gtx\"],"
        "AUTHORITY[\"EPSG\",\"5171\"]],"
        "UNIT[\"metre\",1,"
        "AUTHORITY[\"EPSG\",\"9001\"]],"
        "AXIS[\"Up\",UP],"
        "AUTHORITY[\"EPSG\",\"5773\"]]]");
    auto dst = nn_dynamic_pointer_cast<CRS>(objDst);
    ASSERT_TRUE(dst != nullptr);

    auto authFactory = AuthorityFactory::create(dbContext, "EPSG");
    auto ctxt = CoordinateOperationContext::create(authFactory, nullptr, 0.0);
    ctxt->setGridAvailabilityUse(
        CoordinateOperationContext::GridAvailabilityUse::
            IGNORE_GRID_AVAILABILITY);

    auto list = CoordinateOperationFactory::create()->createOperations(
        NN_CHECK_ASSERT(src), NN_CHECK_ASSERT(dst), ctxt);
    EXPECT_EQ(list.size(), 1U);

    EXPECT_EQ(list[0]->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=pipeline "
              "+step +inv +proj=ortho +f=0 +lat_0=36.1754430555555 "
              "+lon_0=-86.7740944444444 +x_0=0 +y_0=0 +ellps=WGS84 "
              "+step +proj=unitconvert +xy_in=rad +xy_out=deg "
              "+step +proj=axisswap +order=2,1");
}

// ---------------------------------------------------------------------------

TEST(operation, compoundCRS_to_compoundCRS_issue_3328) {
    auto authFactory =
        AuthorityFactory::create(DatabaseContext::create(), std::string());
    // "WGS 84 + EGM96 height"
    auto srcObj = createFromUserInput("EPSG:4326+5773",
                                      authFactory->databaseContext(), false);
    auto src = nn_dynamic_pointer_cast<CRS>(srcObj);
    ASSERT_TRUE(src != nullptr);

    // "WGS 84 + CGVD28 height"
    auto dstObj = createFromUserInput("EPSG:4326+5713",
                                      authFactory->databaseContext(), false);
    auto dst = nn_dynamic_pointer_cast<CRS>(dstObj);
    ASSERT_TRUE(dst != nullptr);

    auto ctxt = CoordinateOperationContext::create(authFactory, nullptr, 0.0);
    ctxt->setGridAvailabilityUse(
        CoordinateOperationContext::GridAvailabilityUse::
            IGNORE_GRID_AVAILABILITY);
    ctxt->setSpatialCriterion(
        CoordinateOperationContext::SpatialCriterion::PARTIAL_INTERSECTION);
    auto list = CoordinateOperationFactory::create()->createOperations(
        NN_NO_CHECK(src), NN_NO_CHECK(dst), ctxt);
    ASSERT_GE(list.size(), 1U);
    EXPECT_EQ(list[0]->nameStr(), "Inverse of WGS 84 to EGM96 height (1) + "
                                  "NAD83(CSRS) to CGVD28 height (1) "
                                  "using NAD83(CSRS) to WGS 84 (2)");
    EXPECT_EQ(list[0]->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=pipeline "
              "+step +proj=push +v_1 +v_2 "
              "+step +proj=axisswap +order=2,1 "
              "+step +proj=unitconvert +xy_in=deg +xy_out=rad "
              "+step +proj=vgridshift +grids=us_nga_egm96_15.tif +multiplier=1 "
              "+step +proj=cart +ellps=WGS84 "
              "+step +inv +proj=helmert +x=-0.991 +y=1.9072 +z=0.5129 "
              "+rx=-0.0257899075194932 "
              "+ry=-0.0096500989602704 +rz=-0.0116599432323421 +s=0 "
              "+convention=coordinate_frame "
              "+step +inv +proj=cart +ellps=GRS80 "
              "+step +inv +proj=vgridshift +grids=ca_nrc_HT2_2010v70.tif "
              "+multiplier=1 "
              "+step +proj=push +v_3 "
              "+step +proj=cart +ellps=GRS80 "
              "+step +proj=helmert +x=-0.991 +y=1.9072 +z=0.5129 "
              "+rx=-0.0257899075194932 "
              "+ry=-0.0096500989602704 +rz=-0.0116599432323421 +s=0 "
              "+convention=coordinate_frame "
              "+step +inv +proj=cart +ellps=WGS84 "
              "+step +proj=pop +v_3 "
              "+step +proj=unitconvert +xy_in=rad +xy_out=deg "
              "+step +proj=axisswap +order=2,1 "
              "+step +proj=pop +v_1 +v_2");
}

// ---------------------------------------------------------------------------

TEST(
    operation,
    compoundCRS_to_compoundCRS_concatenated_operation_with_two_vert_transformation_and_ballpark_geog) {
    auto authFactory =
        AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    // "NAD83(CSRS) + CGVD28 height"
    auto srcObj = createFromUserInput("EPSG:4617+5713",
                                      authFactory->databaseContext(), false);
    auto src = nn_dynamic_pointer_cast<CRS>(srcObj);
    ASSERT_TRUE(src != nullptr);

    // "NAD83(CSRS) + CGVD2013(CGG2013) height"
    auto dstObj = createFromUserInput("EPSG:4617+6647",
                                      authFactory->databaseContext(), false);
    auto dst = nn_dynamic_pointer_cast<CRS>(dstObj);
    ASSERT_TRUE(dst != nullptr);

    auto ctxt = CoordinateOperationContext::create(authFactory, nullptr, 0.0);
    ctxt->setSpatialCriterion(
        CoordinateOperationContext::SpatialCriterion::PARTIAL_INTERSECTION);
    {
        auto list = CoordinateOperationFactory::create()->createOperations(
            NN_NO_CHECK(src), NN_NO_CHECK(dst), ctxt);
        ASSERT_GE(list.size(), 1U);
        EXPECT_EQ(list[0]->nameStr(),
                  "Inverse of NAD83(CSRS)v6 to CGVD28 height (1) + "
                  "NAD83(CSRS)v6 to CGVD2013(CGG2013) height (1) "
                  "using Ballpark geographic offset "
                  "from NAD83(CSRS) to NAD83(CSRS)v6");
    }
#if 0
    // Note: below situation is no longer triggered since EPSG v10.066 update
    // Not obvious to find an equivalent one.

    // That transformation involves doing CGVD28 height to CGVD2013(CGG2013)
    // height by doing:
    // - CGVD28 height to NAD83(CSRS): EPSG registered operation
    // - NAD83(CSRS) to CGVD2013(CGG2013) height by doing:
    //   * NAD83(CSRS) to NAD83(CSRS)v6: ballpark
    //   * NAD83(CSRS)v6 to CGVD2013(CGG2013): EPSG registered operation
    auto ctxt = CoordinateOperationContext::create(authFactory, nullptr, 0.0);
    ctxt->setSpatialCriterion(
        CoordinateOperationContext::SpatialCriterion::PARTIAL_INTERSECTION);
    {
        auto list = CoordinateOperationFactory::create()->createOperations(
            NN_NO_CHECK(src), NN_NO_CHECK(dst), ctxt);
        ASSERT_GE(list.size(), 1U);
        // Check that we have the transformation using NAD83(CSRS)v6 first
        // (as well as the one between NAD83(CSRS) to CGVD28 height)
        EXPECT_EQ(list[0]->nameStr(),
                  "Inverse of NAD83(CSRS) to CGVD28 height (1) + "
                  "Inverse of Ballpark geographic offset from NAD83(CSRS)v6 to "
                  "NAD83(CSRS) + "
                  "NAD83(CSRS)v6 to CGVD2013(CGG2013) height (1) + "
                  "Inverse of Ballpark geographic offset from NAD83(CSRS) to "
                  "NAD83(CSRS)v6");
    }
    {
        auto list = CoordinateOperationFactory::create()->createOperations(
            NN_NO_CHECK(dst), NN_NO_CHECK(src), ctxt);
        ASSERT_GE(list.size(), 1U);
        // Check that we have the transformation using NAD83(CSRS)v6 first
        // (as well as the one between NAD83(CSRS) to CGVD28 height)
        EXPECT_EQ(
            list[0]->nameStr(),
            "Ballpark geographic offset from NAD83(CSRS) to NAD83(CSRS)v6 + "
            "Inverse of NAD83(CSRS)v6 to CGVD2013(CGG2013) height (1) + "
            "Ballpark geographic offset from NAD83(CSRS)v6 to NAD83(CSRS) + "
            "NAD83(CSRS) to CGVD28 height (1)");
    }
#endif
}

// ---------------------------------------------------------------------------

TEST(operation, compoundCRS_to_compoundCRS_issue_3152_ch1903lv03_ln02_bound) {
    auto dbContext = DatabaseContext::create();
    auto authFactory = AuthorityFactory::create(dbContext, std::string());
    auto ctxt = CoordinateOperationContext::create(authFactory, nullptr, 0.0);
    ctxt->setSpatialCriterion(
        CoordinateOperationContext::SpatialCriterion::PARTIAL_INTERSECTION);
    ctxt->setGridAvailabilityUse(
        CoordinateOperationContext::GridAvailabilityUse::
            IGNORE_GRID_AVAILABILITY);
    auto wkt =
        "COMPOUNDCRS[\"CH1903 / LV03 + LN02 height\",\n"
        "    BOUNDCRS[\n"
        "        SOURCECRS[\n"
        "            PROJCRS[\"CH1903 / LV03\",\n"
        "                BASEGEOGCRS[\"CH1903\",\n"
        "                    DATUM[\"CH1903\",\n"
        "                        ELLIPSOID[\"Bessel "
        "1841\",6377397.155,299.1528128,\n"
        "                            LENGTHUNIT[\"metre\",1]]],\n"
        "                    PRIMEM[\"Greenwich\",0,\n"
        "                        ANGLEUNIT[\"degree\",0.0174532925199433]],\n"
        "                    ID[\"EPSG\",4149]],\n"
        "                CONVERSION[\"Map projection of CH1903 / LV03\",\n"
        "                    METHOD[\"Hotine Oblique Mercator (variant B)\",\n"
        "                        ID[\"EPSG\",9815]],\n"
        "                    PARAMETER[\"Latitude of projection "
        "centre\",46.9524055555556,\n"
        "                        ANGLEUNIT[\"degree\",0.0174532925199433],\n"
        "                        ID[\"EPSG\",8811]],\n"
        "                    PARAMETER[\"Longitude of projection "
        "centre\",7.43958333333333,\n"
        "                        ANGLEUNIT[\"degree\",0.0174532925199433],\n"
        "                        ID[\"EPSG\",8812]],\n"
        "                    PARAMETER[\"Azimuth of initial line\",90,\n"
        "                        ANGLEUNIT[\"degree\",0.0174532925199433],\n"
        "                        ID[\"EPSG\",8813]],\n"
        "                    PARAMETER[\"Angle from Rectified to Skew "
        "Grid\",90,\n"
        "                        ANGLEUNIT[\"degree\",0.0174532925199433],\n"
        "                        ID[\"EPSG\",8814]],\n"
        "                    PARAMETER[\"Scale factor on initial line\",1,\n"
        "                        SCALEUNIT[\"unity\",1],\n"
        "                        ID[\"EPSG\",8815]],\n"
        "                    PARAMETER[\"Easting at projection "
        "centre\",600000,\n"
        "                        LENGTHUNIT[\"metre\",1],\n"
        "                        ID[\"EPSG\",8816]],\n"
        "                    PARAMETER[\"Northing at projection "
        "centre\",200000,\n"
        "                        LENGTHUNIT[\"metre\",1],\n"
        "                        ID[\"EPSG\",8817]]],\n"
        "                CS[Cartesian,2],\n"
        "                    AXIS[\"easting\",east,\n"
        "                        ORDER[1],\n"
        "                        LENGTHUNIT[\"metre\",1]],\n"
        "                    AXIS[\"northing\",north,\n"
        "                        ORDER[2],\n"
        "                        LENGTHUNIT[\"metre\",1]],\n"
        "                ID[\"EPSG\",21781]]],\n"
        "        TARGETCRS[\n"
        "            GEOGCRS[\"WGS 84\",\n"
        "                DATUM[\"World Geodetic System 1984\",\n"
        "                    ELLIPSOID[\"WGS 84\",6378137,298.257223563,\n"
        "                        LENGTHUNIT[\"metre\",1]]],\n"
        "                PRIMEM[\"Greenwich\",0,\n"
        "                    ANGLEUNIT[\"degree\",0.0174532925199433]],\n"
        "                CS[ellipsoidal,2],\n"
        "                    AXIS[\"latitude\",north,\n"
        "                        ORDER[1],\n"
        "                        ANGLEUNIT[\"degree\",0.0174532925199433]],\n"
        "                    AXIS[\"longitude\",east,\n"
        "                        ORDER[2],\n"
        "                        ANGLEUNIT[\"degree\",0.0174532925199433]],\n"
        "                ID[\"EPSG\",4326]]],\n"
        "        ABRIDGEDTRANSFORMATION[\"MyTransformation from CH1903 to "
        "WGS84\",\n"
        "            METHOD[\"Position Vector transformation (geog2D "
        "domain)\",\n"
        "                ID[\"EPSG\",9606]],\n"
        "            PARAMETER[\"X-axis translation\",674.374,\n"
        "                ID[\"EPSG\",8605]],\n"
        "            PARAMETER[\"Y-axis translation\",15.056,\n"
        "                ID[\"EPSG\",8606]],\n"
        "            PARAMETER[\"Z-axis translation\",405.346,\n"
        "                ID[\"EPSG\",8607]],\n"
        "            PARAMETER[\"X-axis rotation\",0,\n"
        "                ID[\"EPSG\",8608]],\n"
        "            PARAMETER[\"Y-axis rotation\",0,\n"
        "                ID[\"EPSG\",8609]],\n"
        "            PARAMETER[\"Z-axis rotation\",0,\n"
        "                ID[\"EPSG\",8610]],\n"
        "            PARAMETER[\"Scale difference\",1,\n"
        "                ID[\"EPSG\",8611]]]],\n"
        "    VERTCRS[\"LN02 height\",\n"
        "        VDATUM[\"Landesnivellement 1902\"],\n"
        "        CS[vertical,1],\n"
        "            AXIS[\"gravity-related height\",up,\n"
        "                LENGTHUNIT[\"metre\",1]],\n"
        "        ID[\"EPSG\",5728]]]";
    auto srcObj = createFromUserInput(wkt, dbContext, false);
    auto src = nn_dynamic_pointer_cast<CRS>(srcObj);
    ASSERT_TRUE(src != nullptr);
    auto authFactoryEPSG =
        AuthorityFactory::create(dbContext, std::string("EPSG"));
    auto dst = authFactoryEPSG->createCoordinateReferenceSystem(
        "9518"); // "WGS 84 + EGM2008 height"

    auto list = CoordinateOperationFactory::create()->createOperations(
        NN_NO_CHECK(src), dst, ctxt);
    ASSERT_GE(list.size(), 1U);
    // Check that BoundCRS helmert transformation is used
    EXPECT_EQ(list[0]->nameStr(),
              "Inverse of Map projection of CH1903 / LV03 + "
              "MyTransformation from CH1903 to WGS84 + "
              "Inverse of ETRS89 to WGS 84 (1) + "
              "Inverse of ETRS89 to LN02 height + "
              "ETRS89 to WGS 84 (1) + "
              "WGS 84 to EGM2008 height (1)");
    EXPECT_EQ(list[0]->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=pipeline "
              "+step +inv +proj=somerc +lat_0=46.9524055555556 "
              "+lon_0=7.43958333333333 +k_0=1 "
              "+x_0=600000 +y_0=200000 +ellps=bessel "
              "+step +proj=push +v_3 "
              "+step +proj=cart +ellps=bessel "
              "+step +proj=helmert +x=674.374 +y=15.056 +z=405.346 "
              "+rx=0 +ry=0 +rz=0 +s=0 +convention=position_vector "
              "+step +inv +proj=cart +ellps=GRS80 "
              "+step +proj=pop +v_3 "
              "+step +proj=vgridshift "
              "+grids=ch_swisstopo_chgeo2004_ETRS89_LN02.tif "
              "+multiplier=1 "
              "+step +inv +proj=vgridshift +grids=us_nga_egm08_25.tif "
              "+multiplier=1 "
              "+step +proj=unitconvert +xy_in=rad +xy_out=deg "
              "+step +proj=axisswap +order=2,1");

    auto listInv = CoordinateOperationFactory::create()->createOperations(
        dst, NN_NO_CHECK(src), ctxt);
    ASSERT_GE(listInv.size(), 1U);
    EXPECT_EQ(listInv[0]->nameStr(),
              "Inverse of WGS 84 to EGM2008 height (1) + "
              "Inverse of ETRS89 to WGS 84 (1) + "
              "ETRS89 to LN02 height + "
              "ETRS89 to WGS 84 (1) + "
              "Inverse of MyTransformation from CH1903 to WGS84 + "
              "Map projection of CH1903 / LV03");
}

// ---------------------------------------------------------------------------

TEST(operation,
     compoundCRS_to_compoundCRS_issue_3191_BD72_Ostend_height_to_WGS84_EGM96) {
    auto dbContext = DatabaseContext::create();
    auto authFactory = AuthorityFactory::create(dbContext, std::string());
    auto ctxt = CoordinateOperationContext::create(authFactory, nullptr, 0.0);
    ctxt->setGridAvailabilityUse(
        CoordinateOperationContext::GridAvailabilityUse::
            IGNORE_GRID_AVAILABILITY);
    // BD72 + Ostend height
    auto srcObj = createFromUserInput("EPSG:4313+5710", dbContext, false);
    auto src = nn_dynamic_pointer_cast<CRS>(srcObj);
    ASSERT_TRUE(src != nullptr);
    auto authFactoryEPSG =
        AuthorityFactory::create(dbContext, std::string("EPSG"));
    auto dst = authFactoryEPSG->createCoordinateReferenceSystem(
        "9707"); // "WGS 84 + EGM96 height"

    {
        auto list = CoordinateOperationFactory::create()->createOperations(
            NN_NO_CHECK(src), dst, ctxt);
        ASSERT_GE(list.size(), 1U);
        EXPECT_EQ(list[0]->nameStr(),
                  "BD72 to ETRS89 (3) + "
                  "Inverse of ETRS89 to Ostend height (1) + "
                  "ETRS89 to WGS 84 (1) + "
                  "WGS 84 to EGM96 height (1)");
        const char *expected_proj =
            "+proj=pipeline "
            "+step +proj=axisswap +order=2,1 "
            "+step +proj=unitconvert +xy_in=deg +xy_out=rad "
            "+step +proj=hgridshift +grids=be_ign_bd72lb72_etrs89lb08.tif "
            "+step +proj=vgridshift +grids=be_ign_hBG18.tif +multiplier=1 "
            "+step +inv +proj=vgridshift +grids=us_nga_egm96_15.tif "
            "+multiplier=1 "
            "+step +proj=unitconvert +xy_in=rad +xy_out=deg "
            "+step +proj=axisswap +order=2,1";
        EXPECT_EQ(
            list[0]->exportToPROJString(PROJStringFormatter::create().get()),
            expected_proj);
    }

    {
        auto list = CoordinateOperationFactory::create()->createOperations(
            dst, NN_NO_CHECK(src), ctxt);
        ASSERT_GE(list.size(), 1U);
        EXPECT_EQ(list[0]->nameStr(), "Inverse of WGS 84 to EGM96 height (1) + "
                                      "Inverse of ETRS89 to WGS 84 (1) + "
                                      "ETRS89 to Ostend height (1) + "
                                      "Inverse of BD72 to ETRS89 (3)");
        const char *expected_proj =
            "+proj=pipeline "
            "+step +proj=axisswap +order=2,1 "
            "+step +proj=unitconvert +xy_in=deg +xy_out=rad "
            "+step +proj=vgridshift +grids=us_nga_egm96_15.tif +multiplier=1 "
            "+step +inv +proj=vgridshift +grids=be_ign_hBG18.tif +multiplier=1 "
            "+step +inv +proj=hgridshift +grids=be_ign_bd72lb72_etrs89lb08.tif "
            "+step +proj=unitconvert +xy_in=rad +xy_out=deg "
            "+step +proj=axisswap +order=2,1";
        EXPECT_EQ(
            list[0]->exportToPROJString(PROJStringFormatter::create().get()),
            expected_proj);
    }
}

// ---------------------------------------------------------------------------

TEST(operation, vertCRS_to_vertCRS) {

    auto vertcrs_m_obj = PROJStringParser().createFromPROJString("+vunits=m");
    auto vertcrs_m = nn_dynamic_pointer_cast<VerticalCRS>(vertcrs_m_obj);
    ASSERT_TRUE(vertcrs_m != nullptr);

    auto vertcrs_ft_obj = PROJStringParser().createFromPROJString("+vunits=ft");
    auto vertcrs_ft = nn_dynamic_pointer_cast<VerticalCRS>(vertcrs_ft_obj);
    ASSERT_TRUE(vertcrs_ft != nullptr);

    auto vertcrs_us_ft_obj =
        PROJStringParser().createFromPROJString("+vunits=us-ft");
    auto vertcrs_us_ft =
        nn_dynamic_pointer_cast<VerticalCRS>(vertcrs_us_ft_obj);
    ASSERT_TRUE(vertcrs_us_ft != nullptr);

    {
        auto op = CoordinateOperationFactory::create()->createOperation(
            NN_CHECK_ASSERT(vertcrs_m), NN_CHECK_ASSERT(vertcrs_ft));
        ASSERT_TRUE(op != nullptr);
        EXPECT_EQ(op->exportToPROJString(PROJStringFormatter::create().get()),
                  "+proj=unitconvert +z_in=m +z_out=ft");
    }
    {
        auto op = CoordinateOperationFactory::create()->createOperation(
            NN_CHECK_ASSERT(vertcrs_m), NN_CHECK_ASSERT(vertcrs_ft));
        ASSERT_TRUE(op != nullptr);
        EXPECT_EQ(op->inverse()->exportToPROJString(
                      PROJStringFormatter::create().get()),
                  "+proj=unitconvert +z_in=ft +z_out=m");
    }
    {
        auto op = CoordinateOperationFactory::create()->createOperation(
            NN_CHECK_ASSERT(vertcrs_ft), NN_CHECK_ASSERT(vertcrs_m));
        ASSERT_TRUE(op != nullptr);
        EXPECT_EQ(op->exportToPROJString(PROJStringFormatter::create().get()),
                  "+proj=unitconvert +z_in=ft +z_out=m");
    }
    {
        auto op = CoordinateOperationFactory::create()->createOperation(
            NN_CHECK_ASSERT(vertcrs_ft), NN_CHECK_ASSERT(vertcrs_us_ft));
        ASSERT_TRUE(op != nullptr);
        EXPECT_EQ(op->exportToPROJString(PROJStringFormatter::create().get()),
                  "+proj=affine +s33=0.999998");
    }

    auto vertCRSMetreUp =
        nn_dynamic_pointer_cast<VerticalCRS>(WKTParser().createFromWKT(
            "VERTCRS[\"my height\",VDATUM[\"my datum\"],CS[vertical,1],"
            "AXIS[\"gravity-related height (H)\",up,"
            "LENGTHUNIT[\"metre\",1]]]"));
    ASSERT_TRUE(vertCRSMetreUp != nullptr);

    auto vertCRSMetreDown =
        nn_dynamic_pointer_cast<VerticalCRS>(WKTParser().createFromWKT(
            "VERTCRS[\"my depth\",VDATUM[\"my datum\"],CS[vertical,1],"
            "AXIS[\"depth (D)\",down,LENGTHUNIT[\"metre\",1]]]"));
    ASSERT_TRUE(vertCRSMetreDown != nullptr);

    auto vertCRSMetreDownFtUS =
        nn_dynamic_pointer_cast<VerticalCRS>(WKTParser().createFromWKT(
            "VERTCRS[\"my depth (ftUS)\",VDATUM[\"my datum\"],CS[vertical,1],"
            "AXIS[\"depth (D)\",down,LENGTHUNIT[\"US survey "
            "foot\",0.304800609601219]]]"));
    ASSERT_TRUE(vertCRSMetreDownFtUS != nullptr);

    {
        auto op = CoordinateOperationFactory::create()->createOperation(
            NN_CHECK_ASSERT(vertCRSMetreUp), NN_CHECK_ASSERT(vertCRSMetreDown));
        ASSERT_TRUE(op != nullptr);
        EXPECT_EQ(op->exportToPROJString(PROJStringFormatter::create().get()),
                  "+proj=axisswap +order=1,2,-3");
    }

    {
        auto op = CoordinateOperationFactory::create()->createOperation(
            NN_CHECK_ASSERT(vertCRSMetreUp),
            NN_CHECK_ASSERT(vertCRSMetreDownFtUS));
        ASSERT_TRUE(op != nullptr);
        EXPECT_EQ(op->exportToPROJString(PROJStringFormatter::create().get()),
                  "+proj=affine +s33=-3.28083333333333");
    }
}

// ---------------------------------------------------------------------------

TEST(operation, vertCRS_to_vertCRS_context) {
    auto authFactory =
        AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    auto ctxt = CoordinateOperationContext::create(authFactory, nullptr, 0.0);
    ctxt->setSpatialCriterion(
        CoordinateOperationContext::SpatialCriterion::PARTIAL_INTERSECTION);
    auto list = CoordinateOperationFactory::create()->createOperations(
        // NGVD29 height (m)
        authFactory->createCoordinateReferenceSystem("7968"),
        // NAVD88 height (1)
        authFactory->createCoordinateReferenceSystem("5703"), ctxt);
    ASSERT_EQ(list.size(), 3U);
    EXPECT_EQ(list[0]->nameStr(), "NGVD29 height (m) to NAVD88 height (3)");
    EXPECT_EQ(list[0]->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=vgridshift +grids=us_noaa_vertcone.tif +multiplier=1");
}

// ---------------------------------------------------------------------------

TEST(operation, vertCRS_to_vertCRS_New_Zealand_context) {
    auto authFactory =
        AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    auto ctxt = CoordinateOperationContext::create(authFactory, nullptr, 0.0);
    auto list = CoordinateOperationFactory::create()->createOperations(
        // NZVD2016 height
        authFactory->createCoordinateReferenceSystem("7839"),
        // Auckland 1946 height
        authFactory->createCoordinateReferenceSystem("5759"), ctxt);
    ASSERT_EQ(list.size(), 1U);
    EXPECT_EQ(list[0]->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=vgridshift +grids=nz_linz_auckht1946-nzvd2016.tif "
              "+multiplier=1");
}

// ---------------------------------------------------------------------------

TEST(operation, projCRS_3D_to_geogCRS_3D) {

    auto compoundcrs_ft_obj = PROJStringParser().createFromPROJString(
        "+proj=merc +vunits=ft +type=crs");
    auto proj3DCRS_ft = nn_dynamic_pointer_cast<CRS>(compoundcrs_ft_obj);
    ASSERT_TRUE(proj3DCRS_ft != nullptr);

    auto geogcrs_m_obj = PROJStringParser().createFromPROJString(
        "+proj=longlat +vunits=m +type=crs");
    auto geogcrs_m = nn_dynamic_pointer_cast<CRS>(geogcrs_m_obj);
    ASSERT_TRUE(geogcrs_m != nullptr);

    {
        auto op = CoordinateOperationFactory::create()->createOperation(
            NN_CHECK_ASSERT(proj3DCRS_ft), NN_CHECK_ASSERT(geogcrs_m));
        ASSERT_TRUE(op != nullptr);
        EXPECT_FALSE(op->hasBallparkTransformation());
        EXPECT_EQ(op->exportToPROJString(PROJStringFormatter::create().get()),
                  "+proj=pipeline "
                  "+step +proj=unitconvert +xy_in=m +z_in=ft "
                  "+xy_out=m +z_out=m "
                  "+step +inv +proj=merc +lon_0=0 +k=1 +x_0=0 +y_0=0 "
                  "+ellps=WGS84 "
                  "+step +proj=unitconvert +xy_in=rad +z_in=m "
                  "+xy_out=deg +z_out=m");
    }

    {
        auto op = CoordinateOperationFactory::create()->createOperation(
            NN_CHECK_ASSERT(geogcrs_m), NN_CHECK_ASSERT(proj3DCRS_ft));
        ASSERT_TRUE(op != nullptr);
        EXPECT_FALSE(op->hasBallparkTransformation());
        EXPECT_EQ(op->exportToPROJString(PROJStringFormatter::create().get()),
                  "+proj=pipeline "
                  "+step +proj=unitconvert +z_in=m +z_out=ft "
                  "+step +proj=unitconvert +xy_in=deg +z_in=ft "
                  "+xy_out=rad +z_out=m "
                  "+step +proj=merc +lon_0=0 +k=1 +x_0=0 +y_0=0 +ellps=WGS84 "
                  "+step +proj=unitconvert +xy_in=m +z_in=m "
                  "+xy_out=m +z_out=ft");
    }
}

// ---------------------------------------------------------------------------

TEST(operation, compoundCRS_to_geogCRS_3D) {

    auto compoundcrs_ft_obj = WKTParser().createFromWKT(
        "COMPOUNDCRS[\"unknown\",\n"
        "    PROJCRS[\"unknown\",\n"
        "        BASEGEOGCRS[\"unknown\",\n"
        "            DATUM[\"World Geodetic System 1984\",\n"
        "                ELLIPSOID[\"WGS 84\",6378137,298.257223563,\n"
        "                    LENGTHUNIT[\"metre\",1]],\n"
        "                ID[\"EPSG\",6326]],\n"
        "            PRIMEM[\"Greenwich\",0,\n"
        "                ANGLEUNIT[\"degree\",0.0174532925199433],\n"
        "                ID[\"EPSG\",8901]]],\n"
        "        CONVERSION[\"unknown\",\n"
        "            METHOD[\"Mercator (variant A)\",\n"
        "                ID[\"EPSG\",9804]],\n"
        "            PARAMETER[\"Latitude of natural origin\",0,\n"
        "                ANGLEUNIT[\"degree\",0.0174532925199433],\n"
        "                ID[\"EPSG\",8801]],\n"
        "            PARAMETER[\"Longitude of natural origin\",0,\n"
        "                ANGLEUNIT[\"degree\",0.0174532925199433],\n"
        "                ID[\"EPSG\",8802]],\n"
        "            PARAMETER[\"Scale factor at natural origin\",1,\n"
        "                SCALEUNIT[\"unity\",1],\n"
        "                ID[\"EPSG\",8805]],\n"
        "            PARAMETER[\"False easting\",0,\n"
        "                LENGTHUNIT[\"metre\",1],\n"
        "                ID[\"EPSG\",8806]],\n"
        "            PARAMETER[\"False northing\",0,\n"
        "                LENGTHUNIT[\"metre\",1],\n"
        "                ID[\"EPSG\",8807]]],\n"
        "        CS[Cartesian,2],\n"
        "            AXIS[\"(E)\",east,\n"
        "                ORDER[1],\n"
        "                LENGTHUNIT[\"metre\",1,\n"
        "                    ID[\"EPSG\",9001]]],\n"
        "            AXIS[\"(N)\",north,\n"
        "                ORDER[2],\n"
        "                LENGTHUNIT[\"metre\",1,\n"
        "                    ID[\"EPSG\",9001]]]],\n"
        "    VERTCRS[\"unknown\",\n"
        "        VDATUM[\"unknown\"],\n"
        "        CS[vertical,1],\n"
        "            AXIS[\"gravity-related height (H)\",up,\n"
        "                LENGTHUNIT[\"foot\",0.3048,\n"
        "                    ID[\"EPSG\",9002]]]]]");
    auto compoundcrs_ft = nn_dynamic_pointer_cast<CRS>(compoundcrs_ft_obj);
    ASSERT_TRUE(compoundcrs_ft != nullptr);

    auto geogcrs_m_obj = PROJStringParser().createFromPROJString(
        "+proj=longlat +vunits=m +type=crs");
    auto geogcrs_m = nn_dynamic_pointer_cast<CRS>(geogcrs_m_obj);
    ASSERT_TRUE(geogcrs_m != nullptr);

    {
        auto op = CoordinateOperationFactory::create()->createOperation(
            NN_CHECK_ASSERT(compoundcrs_ft), NN_CHECK_ASSERT(geogcrs_m));
        ASSERT_TRUE(op != nullptr);
        EXPECT_TRUE(op->hasBallparkTransformation());
        EXPECT_EQ(op->exportToPROJString(PROJStringFormatter::create().get()),
                  "+proj=pipeline +step +inv +proj=merc +lon_0=0 +k=1 +x_0=0 "
                  "+y_0=0 +ellps=WGS84 +step +proj=unitconvert +xy_in=rad "
                  "+z_in=ft +xy_out=deg +z_out=m");
    }

    {
        auto op = CoordinateOperationFactory::create()->createOperation(
            NN_CHECK_ASSERT(geogcrs_m), NN_CHECK_ASSERT(compoundcrs_ft));
        ASSERT_TRUE(op != nullptr);
        EXPECT_TRUE(op->hasBallparkTransformation());
        EXPECT_EQ(op->exportToPROJString(PROJStringFormatter::create().get()),
                  "+proj=pipeline +step +proj=unitconvert +xy_in=deg +z_in=m "
                  "+xy_out=rad +z_out=ft +step +proj=merc +lon_0=0 +k=1 +x_0=0 "
                  "+y_0=0 +ellps=WGS84");
    }
}

// ---------------------------------------------------------------------------

TEST(operation, compoundCRS_to_geogCRS_3D_context) {
    auto authFactory =
        AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    // CompoundCRS to Geog3DCRS, with vertical unit change, but without
    // ellipsoid height <--> vertical height correction
    {
        auto ctxt =
            CoordinateOperationContext::create(authFactory, nullptr, 0.0);
        ctxt->setGridAvailabilityUse(
            CoordinateOperationContext::GridAvailabilityUse::
                IGNORE_GRID_AVAILABILITY);
        auto list = CoordinateOperationFactory::create()->createOperations(
            authFactory->createCoordinateReferenceSystem(
                "7406"), // NAD27 + NGVD29 height (ftUS)
            authFactory->createCoordinateReferenceSystem("4979"), // WGS 84
            ctxt);
        ASSERT_GE(list.size(), 1U);
        EXPECT_TRUE(list[0]->hasBallparkTransformation());
        EXPECT_EQ(list[0]->nameStr(),
                  "NAD27 to WGS 84 (79) + Transformation from NGVD29 height "
                  "(ftUS) to WGS 84 (ballpark vertical transformation, without "
                  "ellipsoid height to vertical height correction)");
        EXPECT_EQ(list[0]->exportToPROJString(
                      PROJStringFormatter::create(
                          PROJStringFormatter::Convention::PROJ_5,
                          authFactory->databaseContext())
                          .get()),
                  "+proj=pipeline +step +proj=axisswap +order=2,1 +step "
                  "+proj=unitconvert +xy_in=deg +xy_out=rad +step "
                  "+proj=hgridshift +grids=us_noaa_conus.tif "
                  "+step +proj=unitconvert "
                  "+xy_in=rad +z_in=us-ft +xy_out=deg +z_out=m +step "
                  "+proj=axisswap +order=2,1");
    }

    // CompoundCRS to Geog3DCRS, with same vertical unit, and with
    // direct ellipsoid height <--> vertical height correction and
    // direct horizontal transform (no-op here)
    {
        auto ctxt =
            CoordinateOperationContext::create(authFactory, nullptr, 0.0);
        ctxt->setGridAvailabilityUse(
            CoordinateOperationContext::GridAvailabilityUse::
                IGNORE_GRID_AVAILABILITY);
        auto list = CoordinateOperationFactory::create()->createOperations(
            authFactory->createCoordinateReferenceSystem(
                "5500"), // NAD83(NSRS2007) + NAVD88 height
            authFactory->createCoordinateReferenceSystem("4979"), // WGS 84
            ctxt);
        ASSERT_GE(list.size(), 2U);
        EXPECT_EQ(list[1]->nameStr(),
                  "Inverse of NAD83(NSRS2007) to NAVD88 height (1) + "
                  "NAD83(NSRS2007) to WGS 84 (1)");
        EXPECT_EQ(list[1]->exportToPROJString(
                      PROJStringFormatter::create(
                          PROJStringFormatter::Convention::PROJ_5,
                          authFactory->databaseContext())
                          .get()),
                  "+proj=pipeline "
                  "+step +proj=axisswap +order=2,1 "
                  "+step +proj=unitconvert +xy_in=deg +xy_out=rad "
                  "+step +proj=vgridshift +grids=us_noaa_geoid09_conus.tif "
                  "+multiplier=1 "
                  "+step +proj=unitconvert +xy_in=rad +xy_out=deg "
                  "+step +proj=axisswap +order=2,1");
        EXPECT_EQ(list[1]->remarks(),
                  "For NAD83(NSRS2007) to NAVD88 height (1) (EPSG:9173): Uses "
                  "Geoid09 hybrid model. Replaced by 2012 model (CT code 6326)."
                  "\n"
                  "For NAD83(NSRS2007) to WGS 84 (1) (EPSG:15931): "
                  "Approximation assuming that NAD83(NSRS2007) is equivalent "
                  "to WGS 84 within the accuracy of the transformation.");
    }

    // NAD83 + NAVD88 height --> WGS 84
    {
        auto ctxt =
            CoordinateOperationContext::create(authFactory, nullptr, 0.0);
        ctxt->setSpatialCriterion(
            CoordinateOperationContext::SpatialCriterion::PARTIAL_INTERSECTION);
        ctxt->setGridAvailabilityUse(
            CoordinateOperationContext::GridAvailabilityUse::
                IGNORE_GRID_AVAILABILITY);
        // NAD83 + NAVD88 height
        auto srcObj = createFromUserInput(
            "EPSG:4269+5703", authFactory->databaseContext(), false);
        auto src = nn_dynamic_pointer_cast<CRS>(srcObj);
        ASSERT_TRUE(src != nullptr);
        auto nnSrc = NN_NO_CHECK(src);

        auto list = CoordinateOperationFactory::create()->createOperations(
            nnSrc,
            authFactory->createCoordinateReferenceSystem("4979"), // WGS 84
            ctxt);
        ASSERT_GE(list.size(), 2U);

        EXPECT_EQ(list[0]->nameStr(),
                  "NAD83 to NAD83(HARN) (47) + "
                  "NAD83(HARN) to NAD83(FBN) (1) + "
                  "Inverse of NAD83(FBN) to NAVD88 height (1) + "
                  "Inverse of NAD83(HARN) to NAD83(FBN) (1) + "
                  "NAD83(HARN) to WGS 84 (3)");
        EXPECT_EQ(list[0]->exportToPROJString(
                      PROJStringFormatter::create(
                          PROJStringFormatter::Convention::PROJ_5,
                          authFactory->databaseContext())
                          .get()),
                  "+proj=pipeline "
                  "+step +proj=axisswap +order=2,1 "
                  "+step +proj=unitconvert +xy_in=deg +xy_out=rad "
                  "+step +proj=gridshift "
                  "+grids=us_noaa_nadcon5_nad83_1986_nad83_harn_conus.tif "
                  "+step +proj=gridshift +no_z_transform "
                  "+grids=us_noaa_nadcon5_nad83_harn_nad83_fbn_conus.tif "
                  "+step +proj=vgridshift +grids=us_noaa_geoid03_conus.tif "
                  "+multiplier=1 "
                  "+step +inv +proj=gridshift "
                  "+grids=us_noaa_nadcon5_nad83_harn_nad83_fbn_conus.tif "
                  "+step +proj=cart +ellps=GRS80 "
                  "+step +proj=helmert +x=-0.991 +y=1.9072 +z=0.5129 "
                  "+rx=-0.0257899075194932 "
                  "+ry=-0.0096500989602704 +rz=-0.0116599432323421 +s=0 "
                  "+convention=coordinate_frame "
                  "+step +inv +proj=cart +ellps=WGS84 "
                  "+step +proj=unitconvert +xy_in=rad +xy_out=deg "
                  "+step +proj=axisswap +order=2,1");
    }

    // Another variation, but post horizontal adjustment is in two steps
    {
        auto ctxt =
            CoordinateOperationContext::create(authFactory, nullptr, 0.0);
        ctxt->setSpatialCriterion(
            CoordinateOperationContext::SpatialCriterion::PARTIAL_INTERSECTION);
        ctxt->setGridAvailabilityUse(
            CoordinateOperationContext::GridAvailabilityUse::
                IGNORE_GRID_AVAILABILITY);
        // NAD83(2011) + NAVD88 height
        auto srcObj = createFromUserInput(
            "EPSG:6318+5703", authFactory->databaseContext(), false);
        auto src = nn_dynamic_pointer_cast<CRS>(srcObj);
        ASSERT_TRUE(src != nullptr);
        auto nnSrc = NN_NO_CHECK(src);

        auto list = CoordinateOperationFactory::create()->createOperations(
            nnSrc,
            authFactory->createCoordinateReferenceSystem("4985"), // WGS 72 3D
            ctxt);
        ASSERT_GE(list.size(), 1U);

        EXPECT_EQ(list[0]->nameStr(),
                  "Inverse of NAD83(2011) to NAVD88 height (3) + "
                  "NAD83(2011) to WGS 84 (1) + "
                  "Inverse of WGS 72 to WGS 84 (2)");
        EXPECT_EQ(list[0]->exportToPROJString(
                      PROJStringFormatter::create(
                          PROJStringFormatter::Convention::PROJ_5,
                          authFactory->databaseContext())
                          .get()),
                  "+proj=pipeline "
                  "+step +proj=axisswap +order=2,1 "
                  "+step +proj=unitconvert +xy_in=deg +xy_out=rad "
                  "+step +proj=vgridshift +grids=us_noaa_g2018u0.tif "
                  "+multiplier=1 "
                  "+step +proj=cart +ellps=WGS84 "
                  "+step +inv +proj=helmert +x=0 +y=0 +z=4.5 +rx=0 +ry=0 "
                  "+rz=0.554 +s=0.219 +convention=position_vector "
                  "+step +inv +proj=cart +ellps=WGS72 "
                  "+step +proj=unitconvert +xy_in=rad +xy_out=deg "
                  "+step +proj=axisswap +order=2,1");
    }

    // Check that we can handle vertical transformations where there is a
    // mix of available ones in the PROJ namespace (mx_inegi_ggm10) and in
    // in the EPSG namespace (us_noaa_g2018u0)
    // This test might no longer test this scenario if mx_inegi_ggm10 is
    // referenced one day by EPSG, but at least this tests a common use case.
    {
        auto authFactoryAll =
            AuthorityFactory::create(DatabaseContext::create(), std::string());
        auto ctxt =
            CoordinateOperationContext::create(authFactoryAll, nullptr, 0.0);
        ctxt->setSpatialCriterion(
            CoordinateOperationContext::SpatialCriterion::PARTIAL_INTERSECTION);
        ctxt->setGridAvailabilityUse(
            CoordinateOperationContext::GridAvailabilityUse::
                IGNORE_GRID_AVAILABILITY);
        // NAD83(2011) + NAVD88 height
        auto srcObj = createFromUserInput(
            "EPSG:6318+5703", authFactory->databaseContext(), false);
        auto src = nn_dynamic_pointer_cast<CRS>(srcObj);
        ASSERT_TRUE(src != nullptr);
        auto nnSrc = NN_NO_CHECK(src);

        auto list = CoordinateOperationFactory::create()->createOperations(
            nnSrc,
            authFactory->createCoordinateReferenceSystem("4979"), // WGS 84 3D
            ctxt);
        bool foundGeoid2018 = false;
        bool foundGGM10 = false;
        for (const auto &op : list) {
            try {
                const auto projString = op->exportToPROJString(
                    PROJStringFormatter::create(
                        PROJStringFormatter::Convention::PROJ_5,
                        authFactory->databaseContext())
                        .get());
                if (projString.find("us_noaa_g2018u0.tif") != std::string::npos)
                    foundGeoid2018 = true;
                else if (projString.find("mx_inegi_ggm10.tif") !=
                         std::string::npos)
                    foundGGM10 = true;
            } catch (const std::exception &) {
            }
        }
        EXPECT_TRUE(foundGeoid2018);
        EXPECT_TRUE(foundGGM10);
    }
}

// ---------------------------------------------------------------------------

TEST(operation, compoundCRS_to_geogCRS_3D_with_3D_helmert_context) {
    // Use case of https://github.com/OSGeo/PROJ/issues/2225
    auto dbContext = DatabaseContext::create();
    auto authFactory = AuthorityFactory::create(dbContext, "EPSG");
    auto ctxt = CoordinateOperationContext::create(authFactory, nullptr, 0.0);
    ctxt->setSpatialCriterion(
        CoordinateOperationContext::SpatialCriterion::PARTIAL_INTERSECTION);
    ctxt->setGridAvailabilityUse(
        CoordinateOperationContext::GridAvailabilityUse::
            IGNORE_GRID_AVAILABILITY);
    // WGS84 + EGM96 height
    auto srcObj = createFromUserInput("EPSG:4326+5773", dbContext, false);
    auto src = nn_dynamic_pointer_cast<CRS>(srcObj);
    ASSERT_TRUE(src != nullptr);
    auto list = CoordinateOperationFactory::create()->createOperations(
        NN_NO_CHECK(src),
        // CH1903+
        authFactory->createCoordinateReferenceSystem("4150")->promoteTo3D(
            std::string(), dbContext),
        ctxt);
    ASSERT_GE(list.size(), 1U);
    EXPECT_EQ(list[0]->nameStr(), "Inverse of WGS 84 to EGM96 height (1) + "
                                  "Inverse of CH1903+ to WGS 84 (1)");
    // Check that there is no push v_3 / pop v_3
    const char *expected_proj =
        "+proj=pipeline "
        "+step +proj=axisswap +order=2,1 "
        "+step +proj=unitconvert +xy_in=deg +xy_out=rad "
        "+step +proj=vgridshift +grids=us_nga_egm96_15.tif +multiplier=1 "
        "+step +proj=cart +ellps=WGS84 "
        "+step +proj=helmert +x=-674.374 +y=-15.056 +z=-405.346 "
        "+step +inv +proj=cart +ellps=bessel "
        "+step +proj=unitconvert +xy_in=rad +xy_out=deg "
        "+step +proj=axisswap +order=2,1";
    EXPECT_EQ(list[0]->exportToPROJString(
                  PROJStringFormatter::create(
                      PROJStringFormatter::Convention::PROJ_5, dbContext)
                      .get()),
              expected_proj);
}

// ---------------------------------------------------------------------------

TEST(operation,
     compoundCRS_to_geogCRS_3D_with_3D_helmert_same_geog_src_target_context) {
    // Use case of https://github.com/OSGeo/PROJ/pull/2584
    // From EPSG:XXXX+YYYY to EPSG:XXXX (3D), with a vertical shift grid
    // operation in another datum ZZZZ, and the XXXX<--->ZZZZ being an Helmert
    auto dbContext = DatabaseContext::create();
    auto authFactory = AuthorityFactory::create(dbContext, "EPSG");
    auto ctxt = CoordinateOperationContext::create(authFactory, nullptr, 0.0);
    ctxt->setSpatialCriterion(
        CoordinateOperationContext::SpatialCriterion::PARTIAL_INTERSECTION);
    ctxt->setGridAvailabilityUse(
        CoordinateOperationContext::GridAvailabilityUse::
            IGNORE_GRID_AVAILABILITY);
    // CH1903+ + EGM96 height
    auto srcObj = createFromUserInput("EPSG:4150+5773", dbContext, false);
    auto src = nn_dynamic_pointer_cast<CRS>(srcObj);
    ASSERT_TRUE(src != nullptr);
    auto list = CoordinateOperationFactory::create()->createOperations(
        NN_NO_CHECK(src),
        // CH1903+
        authFactory->createCoordinateReferenceSystem("4150")->promoteTo3D(
            std::string(), dbContext),
        ctxt);
    ASSERT_GE(list.size(), 1U);
    // Check that there is push v_3 / pop v_3 in the step before vgridshift
    // Check that there is *no* push v_3 / pop v_3 after vgridshift
    const char *expected_proj =
        "+proj=pipeline "
        "+step +proj=push +v_1 +v_2 " // avoid any horizontal change
        "+step +proj=axisswap +order=2,1 "
        "+step +proj=unitconvert +xy_in=deg +xy_out=rad "
        "+step +proj=push +v_3 "
        "+step +proj=cart +ellps=bessel "
        "+step +proj=helmert +x=674.374 +y=15.056 +z=405.346 "
        "+step +inv +proj=cart +ellps=WGS84 "
        "+step +proj=pop +v_3 "
        "+step +proj=vgridshift +grids=us_nga_egm96_15.tif +multiplier=1 "
        "+step +proj=cart +ellps=WGS84 "
        "+step +proj=helmert +x=-674.374 +y=-15.056 +z=-405.346 "
        "+step +inv +proj=cart +ellps=bessel "
        "+step +proj=unitconvert +xy_in=rad +xy_out=deg "
        "+step +proj=axisswap +order=2,1 "
        "+step +proj=pop +v_1 +v_2" // avoid any horizontal change
        ;
    EXPECT_EQ(list[0]->exportToPROJString(
                  PROJStringFormatter::create(
                      PROJStringFormatter::Convention::PROJ_5, dbContext)
                      .get()),
              expected_proj);
}

// ---------------------------------------------------------------------------

TEST(operation,
     compoundCRS_to_geogCRS_3D_with_null_helmert_same_geog_src_target_context) {
    // Variation of previous case
    // From EPSG:XXXX+YYYY to EPSG:XXXX (3D), with a vertical shift grid
    // operation in another datum ZZZZ, and the XXXX<--->ZZZZ being a
    // null Helmert
    auto dbContext = DatabaseContext::create();
    auto authFactory = AuthorityFactory::create(dbContext, "EPSG");
    auto ctxt = CoordinateOperationContext::create(authFactory, nullptr, 0.0);
    ctxt->setSpatialCriterion(
        CoordinateOperationContext::SpatialCriterion::PARTIAL_INTERSECTION);
    ctxt->setGridAvailabilityUse(
        CoordinateOperationContext::GridAvailabilityUse::
            IGNORE_GRID_AVAILABILITY);
    // ETRS89 + EGM96 height
    auto srcObj = createFromUserInput("EPSG:4258+5773", dbContext, false);
    auto src = nn_dynamic_pointer_cast<CRS>(srcObj);
    ASSERT_TRUE(src != nullptr);
    auto list = CoordinateOperationFactory::create()->createOperations(
        NN_NO_CHECK(src),
        // ETRS89 3D
        authFactory->createCoordinateReferenceSystem("4937"), ctxt);
    ASSERT_GE(list.size(), 1U);
    // No push/pop needed
    const char *expected_proj =
        "+proj=pipeline "
        "+step +proj=axisswap +order=2,1 "
        "+step +proj=unitconvert +xy_in=deg +xy_out=rad "
        "+step +proj=vgridshift +grids=us_nga_egm96_15.tif +multiplier=1 "
        "+step +proj=unitconvert +xy_in=rad +xy_out=deg "
        "+step +proj=axisswap +order=2,1";
    EXPECT_EQ(list[0]->exportToPROJString(
                  PROJStringFormatter::create(
                      PROJStringFormatter::Convention::PROJ_5, dbContext)
                      .get()),
              expected_proj);
}

// ---------------------------------------------------------------------------

TEST(operation,
     compoundCRS_to_geogCRS_3D_with_same_geog_src_target_interp_context) {
    auto dbContext = DatabaseContext::create();
    // Tests a mix of Datum and DatumEnsemble regarding WGS 84 when we compare
    // the datums used in the source -> interpolation_crs and
    // interpolation_crs -> target transformations.
    auto authFactory = AuthorityFactory::create(dbContext, "EPSG");
    auto ctxt = CoordinateOperationContext::create(authFactory, nullptr, 0.0);
    ctxt->setSpatialCriterion(
        CoordinateOperationContext::SpatialCriterion::PARTIAL_INTERSECTION);
    ctxt->setGridAvailabilityUse(
        CoordinateOperationContext::GridAvailabilityUse::
            IGNORE_GRID_AVAILABILITY);
    auto dstObj = WKTParser().createFromWKT(
        "COMPOUNDCRS[\"WGS 84 + my_height\",\n"
        "    GEOGCRS[\"WGS 84\",\n"
        "        DATUM[\"World Geodetic System 1984\",\n"
        "            ELLIPSOID[\"WGS 84\",6378137,298.257223563,\n"
        "                LENGTHUNIT[\"metre\",1]]],\n"
        "        PRIMEM[\"Greenwich\",0,\n"
        "            ANGLEUNIT[\"degree\",0.0174532925199433]],\n"
        "        CS[ellipsoidal,2],\n"
        "            AXIS[\"geodetic latitude (Lat)\",north,\n"
        "                ORDER[1],\n"
        "                ANGLEUNIT[\"degree\",0.0174532925199433]],\n"
        "            AXIS[\"geodetic longitude (Lon)\",east,\n"
        "                ORDER[2],\n"
        "                ANGLEUNIT[\"degree\",0.0174532925199433]],\n"
        "        ID[\"EPSG\",4326]],\n"
        "    BOUNDCRS[\n"
        "        SOURCECRS[\n"
        "            VERTCRS[\"my_height\",\n"
        "                VDATUM[\"my_height\"],\n"
        "                CS[vertical,1],\n"
        "                    AXIS[\"up\",up,\n"
        "                        LENGTHUNIT[\"metre\",1,\n"
        "                            ID[\"EPSG\",9001]]]]],\n"
        "        TARGETCRS[\n"
        "            GEOGCRS[\"WGS 84\",\n"
        "                DATUM[\"World Geodetic System 1984\",\n"
        "                    ELLIPSOID[\"WGS 84\",6378137,298.257223563,\n"
        "                        LENGTHUNIT[\"metre\",1]]],\n"
        "                PRIMEM[\"Greenwich\",0,\n"
        "                    ANGLEUNIT[\"degree\",0.0174532925199433]],\n"
        "                CS[ellipsoidal,3],\n"
        "                    AXIS[\"geodetic latitude (Lat)\",north,\n"
        "                        ORDER[1],\n"
        "                        ANGLEUNIT[\"degree\",0.0174532925199433]],\n"
        "                    AXIS[\"geodetic longitude (Lon)\",east,\n"
        "                        ORDER[2],\n"
        "                        ANGLEUNIT[\"degree\",0.0174532925199433]],\n"
        "                    AXIS[\"ellipsoidal height (h)\",up,\n"
        "                        ORDER[3],\n"
        "                        LENGTHUNIT[\"metre\",1]],\n"
        "                ID[\"EPSG\",4979]]],\n"
        "        ABRIDGEDTRANSFORMATION["
        "\"my_height to WGS84 ellipsoidal height\",\n"
        "            METHOD[\"GravityRelatedHeight to Geographic3D\"],\n"
        "            PARAMETERFILE[\"Geoid (height correction) model file\","
        "\"fake.gtx\",\n"
        "                ID[\"EPSG\",8666]]]]]");
    auto dst = nn_dynamic_pointer_cast<CRS>(dstObj);
    ASSERT_TRUE(dst != nullptr);
    auto list = CoordinateOperationFactory::create()->createOperations(
        authFactory->createCoordinateReferenceSystem("4979"), // WGS 84 3D
        NN_NO_CHECK(dst), ctxt);
    ASSERT_EQ(list.size(), 1U);
    const char *expected_proj =
        "+proj=pipeline "
        "+step +proj=axisswap +order=2,1 "
        "+step +proj=unitconvert +xy_in=deg +xy_out=rad "
        "+step +inv +proj=vgridshift +grids=fake.gtx +multiplier=1 "
        "+step +proj=unitconvert +xy_in=rad +xy_out=deg "
        "+step +proj=axisswap +order=2,1";
    EXPECT_EQ(list[0]->exportToPROJString(
                  PROJStringFormatter::create(
                      PROJStringFormatter::Convention::PROJ_5, dbContext)
                      .get()),
              expected_proj);
}

// ---------------------------------------------------------------------------

TEST(operation, compoundCRS_to_geogCRS_3D_WGS84_to_GDA2020_AHD_Height) {
    // Use case of https://github.com/OSGeo/PROJ/issues/2348
    auto dbContext = DatabaseContext::create();
    auto authFactory = AuthorityFactory::create(dbContext, "EPSG");
    auto ctxt = CoordinateOperationContext::create(authFactory, nullptr, 0.0);
    ctxt->setSpatialCriterion(
        CoordinateOperationContext::SpatialCriterion::PARTIAL_INTERSECTION);
    ctxt->setGridAvailabilityUse(
        CoordinateOperationContext::GridAvailabilityUse::
            IGNORE_GRID_AVAILABILITY);
    {
        auto list = CoordinateOperationFactory::create()->createOperations(
            // GDA2020 + AHD height
            authFactory->createCoordinateReferenceSystem("9463"),
            // WGS 84 3D
            authFactory->createCoordinateReferenceSystem("4979"), ctxt);
        ASSERT_GE(list.size(), 1U);
        EXPECT_EQ(list[0]->nameStr(), "Inverse of GDA2020 to AHD height (1) + "
                                      "GDA2020 to WGS 84 (2)");
    }

    // Inverse
    {
        auto list = CoordinateOperationFactory::create()->createOperations(
            // WGS 84 3D
            authFactory->createCoordinateReferenceSystem("4979"),
            // GDA2020 + AHD height
            authFactory->createCoordinateReferenceSystem("9463"), ctxt);
        ASSERT_GE(list.size(), 1U);
        EXPECT_EQ(list[0]->nameStr(), "Inverse of GDA2020 to WGS 84 (2) + "
                                      "GDA2020 to AHD height (1)");
    }
}

// ---------------------------------------------------------------------------

TEST(operation, compoundCRS_to_geogCRS_2D_promote_to_3D_context) {
    auto authFactory =
        AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    auto ctxt = CoordinateOperationContext::create(authFactory, nullptr, 0.0);
    ctxt->setSpatialCriterion(
        CoordinateOperationContext::SpatialCriterion::PARTIAL_INTERSECTION);
    ctxt->setGridAvailabilityUse(
        CoordinateOperationContext::GridAvailabilityUse::
            IGNORE_GRID_AVAILABILITY);
    // NAD83 + NAVD88 height
    auto srcObj = createFromUserInput("EPSG:4269+5703",
                                      authFactory->databaseContext(), false);
    auto src = nn_dynamic_pointer_cast<CRS>(srcObj);
    ASSERT_TRUE(src != nullptr);
    auto nnSrc = NN_NO_CHECK(src);
    auto dst =
        authFactory->createCoordinateReferenceSystem("4269")->promoteTo3D(
            std::string(), authFactory->databaseContext()); // NAD83

    auto listCompoundToGeog2D =
        CoordinateOperationFactory::create()->createOperations(nnSrc, dst,
                                                               ctxt);
    // The checked value is not that important, but in case this changes,
    // likely due to a EPSG upgrade, worth checking
    EXPECT_EQ(listCompoundToGeog2D.size(), 199U);

    auto listGeog2DToCompound =
        CoordinateOperationFactory::create()->createOperations(dst, nnSrc,
                                                               ctxt);
    EXPECT_EQ(listGeog2DToCompound.size(), listCompoundToGeog2D.size());
}

// ---------------------------------------------------------------------------

TEST(operation, compoundCRS_of_projCRS_to_geogCRS_3D_context) {
    auto authFactory =
        AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    auto ctxt = CoordinateOperationContext::create(authFactory, nullptr, 0.0);
    ctxt->setSpatialCriterion(
        CoordinateOperationContext::SpatialCriterion::PARTIAL_INTERSECTION);
    ctxt->setGridAvailabilityUse(
        CoordinateOperationContext::GridAvailabilityUse::
            IGNORE_GRID_AVAILABILITY);
    // SPCS83 California zone 1 (US Survey feet) + NAVD88 height (ftUS)
    auto srcObj = createFromUserInput("EPSG:2225+6360",
                                      authFactory->databaseContext(), false);
    auto src = nn_dynamic_pointer_cast<CRS>(srcObj);
    ASSERT_TRUE(src != nullptr);
    auto nnSrc = NN_NO_CHECK(src);
    auto dst =
        authFactory->createCoordinateReferenceSystem("4269")->promoteTo3D(
            std::string(), authFactory->databaseContext()); // NAD83

    auto list = CoordinateOperationFactory::create()->createOperations(
        nnSrc, dst, ctxt);
    // The checked value is not that important, but in case this changes,
    // likely due to a EPSG upgrade, worth checking
    // We want to make sure that the horizontal adjustments before and after
    // the vertical transformation are the reverse of each other, and there are
    // not mixes with different alternative operations (like California grid
    // forward and Nevada grid reverse)
    ASSERT_EQ(list.size(), 21U);

    // Check that unit conversion is OK
    auto op_proj =
        list[0]->exportToPROJString(PROJStringFormatter::create().get());
    EXPECT_EQ(op_proj,
              "+proj=pipeline "
              "+step +proj=unitconvert +xy_in=us-ft +xy_out=m "
              "+step +inv +proj=lcc +lat_0=39.3333333333333 +lon_0=-122 "
              "+lat_1=41.6666666666667 +lat_2=40 +x_0=2000000.0001016 "
              "+y_0=500000.0001016 +ellps=GRS80 "
              "+step +proj=hgridshift +grids=us_noaa_cnhpgn.tif "
              "+step +proj=gridshift +no_z_transform "
              "+grids=us_noaa_nadcon5_nad83_harn_nad83_fbn_conus.tif "
              "+step +proj=unitconvert +z_in=us-ft +z_out=m "
              "+step +proj=vgridshift +grids=us_noaa_geoid03_conus.tif "
              "+multiplier=1 "
              "+step +inv +proj=gridshift "
              "+grids=us_noaa_nadcon5_nad83_harn_nad83_fbn_conus.tif "
              "+step +inv +proj=hgridshift +grids=us_noaa_cnhpgn.tif "
              "+step +proj=unitconvert +xy_in=rad +xy_out=deg "
              "+step +proj=axisswap +order=2,1");
}

// ---------------------------------------------------------------------------

TEST(operation, compoundCRS_from_wkt_without_id_to_geogCRS) {
    auto authFactory =
        AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    auto ctxt = CoordinateOperationContext::create(authFactory, nullptr, 0.0);
    ctxt->setSpatialCriterion(
        CoordinateOperationContext::SpatialCriterion::PARTIAL_INTERSECTION);
    ctxt->setGridAvailabilityUse(
        CoordinateOperationContext::GridAvailabilityUse::
            IGNORE_GRID_AVAILABILITY);
    auto wkt =
        "COMPOUNDCRS[\"NAD83(2011) + NAVD88 height\",\n"
        "    GEOGCRS[\"NAD83(2011)\",\n"
        "        DATUM[\"NAD83 (National Spatial Reference System 2011)\",\n"
        "            ELLIPSOID[\"GRS 1980\",6378137,298.257222101,\n"
        "                LENGTHUNIT[\"metre\",1]]],\n"
        "        PRIMEM[\"Greenwich\",0,\n"
        "            ANGLEUNIT[\"degree\",0.0174532925199433]],\n"
        "        CS[ellipsoidal,2],\n"
        "            AXIS[\"geodetic latitude (Lat)\",north,\n"
        "                ORDER[1],\n"
        "                ANGLEUNIT[\"degree\",0.0174532925199433]],\n"
        "            AXIS[\"geodetic longitude (Lon)\",east,\n"
        "                ORDER[2],\n"
        "                ANGLEUNIT[\"degree\",0.0174532925199433]]],\n"
        "    VERTCRS[\"NAVD88 height\",\n"
        "        VDATUM[\"North American Vertical Datum 1988\"],\n"
        "        CS[vertical,1],\n"
        "            AXIS[\"gravity-related height (H)\",up,\n"
        "                LENGTHUNIT[\"metre\",1]]]]";
    auto srcObj =
        createFromUserInput(wkt, authFactory->databaseContext(), false);
    auto src = nn_dynamic_pointer_cast<CRS>(srcObj);
    ASSERT_TRUE(src != nullptr);
    auto dst =
        authFactory->createCoordinateReferenceSystem("6319"); // NAD83(2011)

    auto list = CoordinateOperationFactory::create()->createOperations(
        NN_NO_CHECK(src), dst, ctxt);
    // NAD83(2011) + NAVD88 height
    auto srcRefObj = createFromUserInput("EPSG:6318+5703",
                                         authFactory->databaseContext(), false);
    auto srcRef = nn_dynamic_pointer_cast<CRS>(srcRefObj);
    ASSERT_TRUE(srcRef != nullptr);
    ASSERT_TRUE(
        src->isEquivalentTo(srcRef.get(), IComparable::Criterion::EQUIVALENT));
    auto listRef = CoordinateOperationFactory::create()->createOperations(
        NN_NO_CHECK(srcRef), dst, ctxt);

    EXPECT_EQ(list.size(), listRef.size());
}

// ---------------------------------------------------------------------------

TEST(operation,
     compoundCRS_of_projCRS_from_wkt_without_id_or_extent_to_geogCRS) {
    auto authFactory =
        AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    auto ctxt = CoordinateOperationContext::create(authFactory, nullptr, 0.0);
    ctxt->setSpatialCriterion(
        CoordinateOperationContext::SpatialCriterion::PARTIAL_INTERSECTION);
    ctxt->setGridAvailabilityUse(
        CoordinateOperationContext::GridAvailabilityUse::
            IGNORE_GRID_AVAILABILITY);
    auto wkt =
        "COMPOUNDCRS[\"NAD83 / Pennsylvania South + NAVD88 height\",\n"
        "    PROJCRS[\"NAD83 / Pennsylvania South\",\n"
        "        BASEGEOGCRS[\"NAD83\",\n"
        "            DATUM[\"North American Datum 1983\",\n"
        "                ELLIPSOID[\"GRS 1980\",6378137,298.257222101,\n"
        "                    LENGTHUNIT[\"metre\",1]]],\n"
        "            PRIMEM[\"Greenwich\",0,\n"
        "                ANGLEUNIT[\"degree\",0.0174532925199433]]],\n"
        "        CONVERSION[\"SPCS83 Pennsylvania South zone (meters)\",\n"
        "            METHOD[\"Lambert Conic Conformal (2SP)\",\n"
        "                ID[\"EPSG\",9802]],\n"
        "            PARAMETER[\"Latitude of false origin\",39.3333333333333,\n"
        "                ANGLEUNIT[\"degree\",0.0174532925199433],\n"
        "                ID[\"EPSG\",8821]],\n"
        "            PARAMETER[\"Longitude of false origin\",-77.75,\n"
        "                ANGLEUNIT[\"degree\",0.0174532925199433],\n"
        "                ID[\"EPSG\",8822]],\n"
        "            PARAMETER[\"Latitude of 1st standard "
        "parallel\",40.9666666666667,\n"
        "                ANGLEUNIT[\"degree\",0.0174532925199433],\n"
        "                ID[\"EPSG\",8823]],\n"
        "            PARAMETER[\"Latitude of 2nd standard "
        "parallel\",39.9333333333333,\n"
        "                ANGLEUNIT[\"degree\",0.0174532925199433],\n"
        "                ID[\"EPSG\",8824]],\n"
        "            PARAMETER[\"Easting at false origin\",600000,\n"
        "                LENGTHUNIT[\"metre\",1],\n"
        "                ID[\"EPSG\",8826]],\n"
        "            PARAMETER[\"Northing at false origin\",0,\n"
        "                LENGTHUNIT[\"metre\",1],\n"
        "                ID[\"EPSG\",8827]]],\n"
        "        CS[Cartesian,2],\n"
        "            AXIS[\"easting (X)\",east,\n"
        "                ORDER[1],\n"
        "                LENGTHUNIT[\"metre\",1]],\n"
        "            AXIS[\"northing (Y)\",north,\n"
        "                ORDER[2],\n"
        "                LENGTHUNIT[\"metre\",1]]],\n"
        "    VERTCRS[\"NAVD88 height\",\n"
        "        VDATUM[\"North American Vertical Datum 1988\"],\n"
        "        CS[vertical,1],\n"
        "            AXIS[\"gravity-related height (H)\",up,\n"
        "                LENGTHUNIT[\"metre\",1]]]]";
    auto srcObj =
        createFromUserInput(wkt, authFactory->databaseContext(), false);
    auto src = nn_dynamic_pointer_cast<CRS>(srcObj);
    ASSERT_TRUE(src != nullptr);
    auto dst = authFactory->createCoordinateReferenceSystem("4269"); // NAD83

    auto list = CoordinateOperationFactory::create()->createOperations(
        NN_NO_CHECK(src), dst, ctxt);
    // NAD83 / Pennsylvania South + NAVD88 height
    auto srcRefObj = createFromUserInput("EPSG:32129+5703",
                                         authFactory->databaseContext(), false);
    auto srcRef = nn_dynamic_pointer_cast<CRS>(srcRefObj);
    ASSERT_TRUE(srcRef != nullptr);
    ASSERT_TRUE(
        src->isEquivalentTo(srcRef.get(), IComparable::Criterion::EQUIVALENT));
    auto listRef = CoordinateOperationFactory::create()->createOperations(
        NN_NO_CHECK(srcRef), dst, ctxt);

    EXPECT_EQ(list.size(), listRef.size());
}

// ---------------------------------------------------------------------------

TEST(operation, compoundCRS_to_geogCRS_with_vertical_unit_change) {
    auto authFactory =
        AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    auto ctxt = CoordinateOperationContext::create(authFactory, nullptr, 0.0);
    ctxt->setSpatialCriterion(
        CoordinateOperationContext::SpatialCriterion::PARTIAL_INTERSECTION);
    ctxt->setGridAvailabilityUse(
        CoordinateOperationContext::GridAvailabilityUse::
            IGNORE_GRID_AVAILABILITY);
    // NAD83(2011) + NAVD88 height (ftUS)
    auto srcObj = createFromUserInput("EPSG:6318+6360",
                                      authFactory->databaseContext(), false);
    auto src = nn_dynamic_pointer_cast<CRS>(srcObj);
    ASSERT_TRUE(src != nullptr);
    auto nnSrc = NN_NO_CHECK(src);
    auto dst =
        authFactory->createCoordinateReferenceSystem("6319"); // NAD83(2011) 3D

    auto listCompoundToGeog =
        CoordinateOperationFactory::create()->createOperations(nnSrc, dst,
                                                               ctxt);
    ASSERT_TRUE(!listCompoundToGeog.empty());

    // NAD83(2011) + NAVD88 height
    auto srcObjCompoundVMetre = createFromUserInput(
        "EPSG:6318+5703", authFactory->databaseContext(), false);
    auto srcCompoundVMetre = nn_dynamic_pointer_cast<CRS>(srcObjCompoundVMetre);
    ASSERT_TRUE(srcCompoundVMetre != nullptr);
    auto listCompoundMetreToGeog =
        CoordinateOperationFactory::create()->createOperations(
            NN_NO_CHECK(srcCompoundVMetre), dst, ctxt);

    // Check that we get the same and similar results whether we start from
    // regular NAVD88 height or its ftUs variant
    ASSERT_EQ(listCompoundToGeog.size(), listCompoundMetreToGeog.size());

    EXPECT_EQ(listCompoundToGeog[0]->nameStr(),
              "Conversion from NAVD88 height (ftUS) to NAVD88 height + " +
                  listCompoundMetreToGeog[0]->nameStr());
    EXPECT_EQ(
        listCompoundToGeog[0]->exportToPROJString(
            PROJStringFormatter::create(PROJStringFormatter::Convention::PROJ_5,
                                        authFactory->databaseContext())
                .get()),
        replaceAll(listCompoundMetreToGeog[0]->exportToPROJString(
                       PROJStringFormatter::create(
                           PROJStringFormatter::Convention::PROJ_5,
                           authFactory->databaseContext())
                           .get()),
                   "+step +proj=unitconvert +xy_in=deg +xy_out=rad",
                   "+step +proj=unitconvert +xy_in=deg +z_in=us-ft +xy_out=rad "
                   "+z_out=m"));

    // Check reverse path
    auto listGeogToCompound =
        CoordinateOperationFactory::create()->createOperations(dst, nnSrc,
                                                               ctxt);
    EXPECT_EQ(listGeogToCompound.size(), listCompoundToGeog.size());
}

// ---------------------------------------------------------------------------

TEST(
    operation,
    compoundCRS_to_geogCRS_with_vertical_unit_change_and_complex_horizontal_change) {
    auto authFactory =
        AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    auto ctxt = CoordinateOperationContext::create(authFactory, nullptr, 0.0);
    ctxt->setSpatialCriterion(
        CoordinateOperationContext::SpatialCriterion::PARTIAL_INTERSECTION);
    ctxt->setGridAvailabilityUse(
        CoordinateOperationContext::GridAvailabilityUse::
            IGNORE_GRID_AVAILABILITY);
    // NAD83(2011) + NAVD88 height (ftUS)
    auto srcObj = createFromUserInput("EPSG:6318+6360",
                                      authFactory->databaseContext(), false);
    auto src = nn_dynamic_pointer_cast<CRS>(srcObj);
    ASSERT_TRUE(src != nullptr);
    auto nnSrc = NN_NO_CHECK(src);
    auto dst =
        authFactory->createCoordinateReferenceSystem("7665"); // WGS84(G1762) 3D

    auto listCompoundToGeog =
        CoordinateOperationFactory::create()->createOperations(nnSrc, dst,
                                                               ctxt);

    // NAD83(2011) + NAVD88 height
    auto srcObjCompoundVMetre = createFromUserInput(
        "EPSG:6318+5703", authFactory->databaseContext(), false);
    auto srcCompoundVMetre = nn_dynamic_pointer_cast<CRS>(srcObjCompoundVMetre);
    ASSERT_TRUE(srcCompoundVMetre != nullptr);
    auto listCompoundMetreToGeog =
        CoordinateOperationFactory::create()->createOperations(
            NN_NO_CHECK(srcCompoundVMetre), dst, ctxt);

    // Check that we get the same and similar results whether we start from
    // regular NAVD88 height or its ftUs variant
    ASSERT_EQ(listCompoundToGeog.size(), listCompoundMetreToGeog.size());

    ASSERT_GE(listCompoundToGeog.size(), 1U);

    EXPECT_EQ(listCompoundToGeog[0]->nameStr(),
              "Conversion from NAVD88 height (ftUS) to NAVD88 height + " +
                  listCompoundMetreToGeog[0]->nameStr());
    EXPECT_EQ(
        listCompoundToGeog[0]->exportToPROJString(
            PROJStringFormatter::create(PROJStringFormatter::Convention::PROJ_5,
                                        authFactory->databaseContext())
                .get()),
        replaceAll(listCompoundMetreToGeog[0]->exportToPROJString(
                       PROJStringFormatter::create(
                           PROJStringFormatter::Convention::PROJ_5,
                           authFactory->databaseContext())
                           .get()),
                   "+step +proj=unitconvert +xy_in=deg +xy_out=rad",
                   "+step +proj=unitconvert +xy_in=deg +z_in=us-ft +xy_out=rad "
                   "+z_out=m"));
}

// ---------------------------------------------------------------------------

TEST(operation, compoundCRS_to_geogCRS_with_height_depth_reversal) {
    auto authFactory =
        AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    auto ctxt = CoordinateOperationContext::create(authFactory, nullptr, 0.0);
    ctxt->setSpatialCriterion(
        CoordinateOperationContext::SpatialCriterion::PARTIAL_INTERSECTION);
    ctxt->setGridAvailabilityUse(
        CoordinateOperationContext::GridAvailabilityUse::
            IGNORE_GRID_AVAILABILITY);
    // NAD83(2011) + NAVD88 depth
    auto srcObj = createFromUserInput("EPSG:6318+6357",
                                      authFactory->databaseContext(), false);
    auto src = nn_dynamic_pointer_cast<CRS>(srcObj);
    ASSERT_TRUE(src != nullptr);
    auto nnSrc = NN_NO_CHECK(src);
    auto dst =
        authFactory->createCoordinateReferenceSystem("6319"); // NAD83(2011) 3D

    auto listCompoundToGeog =
        CoordinateOperationFactory::create()->createOperations(nnSrc, dst,
                                                               ctxt);
    ASSERT_TRUE(!listCompoundToGeog.empty());

    // NAD83(2011) + NAVD88 height
    auto srcObjCompoundVMetre = createFromUserInput(
        "EPSG:6318+5703", authFactory->databaseContext(), false);
    auto srcCompoundVMetre = nn_dynamic_pointer_cast<CRS>(srcObjCompoundVMetre);
    ASSERT_TRUE(srcCompoundVMetre != nullptr);
    auto listCompoundMetreToGeog =
        CoordinateOperationFactory::create()->createOperations(
            NN_NO_CHECK(srcCompoundVMetre), dst, ctxt);

    // Check that we get the same and similar results whether we start from
    // regular NAVD88 height or its depth variant
    ASSERT_EQ(listCompoundToGeog.size(), listCompoundMetreToGeog.size());

    EXPECT_EQ(listCompoundToGeog[0]->nameStr(),
              "Conversion from NAVD88 depth to NAVD88 height + " +
                  listCompoundMetreToGeog[0]->nameStr());
    EXPECT_EQ(
        listCompoundToGeog[0]->exportToPROJString(
            PROJStringFormatter::create(PROJStringFormatter::Convention::PROJ_5,
                                        authFactory->databaseContext())
                .get()),
        replaceAll(listCompoundMetreToGeog[0]->exportToPROJString(
                       PROJStringFormatter::create(
                           PROJStringFormatter::Convention::PROJ_5,
                           authFactory->databaseContext())
                           .get()),
                   "+step +proj=unitconvert +xy_in=deg +xy_out=rad",
                   "+step +proj=unitconvert +xy_in=deg +xy_out=rad "
                   "+step +proj=axisswap +order=1,2,-3"));

    // Check reverse path
    auto listGeogToCompound =
        CoordinateOperationFactory::create()->createOperations(dst, nnSrc,
                                                               ctxt);
    EXPECT_EQ(listGeogToCompound.size(), listCompoundToGeog.size());
}

// ---------------------------------------------------------------------------

TEST(
    operation,
    compoundCRS_to_geogCRS_with_vertical_unit_change_and_height_depth_reversal) {
    auto authFactory =
        AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    auto ctxt = CoordinateOperationContext::create(authFactory, nullptr, 0.0);
    ctxt->setSpatialCriterion(
        CoordinateOperationContext::SpatialCriterion::PARTIAL_INTERSECTION);
    ctxt->setGridAvailabilityUse(
        CoordinateOperationContext::GridAvailabilityUse::
            IGNORE_GRID_AVAILABILITY);
    // NAD83(2011) + NAVD88 depth (ftUS)
    auto srcObj = createFromUserInput("EPSG:6318+6358",
                                      authFactory->databaseContext(), false);
    auto src = nn_dynamic_pointer_cast<CRS>(srcObj);
    ASSERT_TRUE(src != nullptr);
    auto nnSrc = NN_NO_CHECK(src);
    auto dst =
        authFactory->createCoordinateReferenceSystem("6319"); // NAD83(2011) 3D

    auto listCompoundToGeog =
        CoordinateOperationFactory::create()->createOperations(nnSrc, dst,
                                                               ctxt);
    ASSERT_TRUE(!listCompoundToGeog.empty());

    // NAD83(2011) + NAVD88 height
    auto srcObjCompoundVMetre = createFromUserInput(
        "EPSG:6318+5703", authFactory->databaseContext(), false);
    auto srcCompoundVMetre = nn_dynamic_pointer_cast<CRS>(srcObjCompoundVMetre);
    ASSERT_TRUE(srcCompoundVMetre != nullptr);
    auto listCompoundMetreToGeog =
        CoordinateOperationFactory::create()->createOperations(
            NN_NO_CHECK(srcCompoundVMetre), dst, ctxt);

    // Check that we get the same and similar results whether we start from
    // regular NAVD88 height or its depth (ftUS) variant
    ASSERT_EQ(listCompoundToGeog.size(), listCompoundMetreToGeog.size());

    EXPECT_EQ(listCompoundToGeog[0]->nameStr(),
              "Conversion from NAVD88 depth (ftUS) to NAVD88 height + " +
                  listCompoundMetreToGeog[0]->nameStr());
    EXPECT_EQ(
        listCompoundToGeog[0]->exportToPROJString(
            PROJStringFormatter::create(PROJStringFormatter::Convention::PROJ_5,
                                        authFactory->databaseContext())
                .get()),
        replaceAll(listCompoundMetreToGeog[0]->exportToPROJString(
                       PROJStringFormatter::create(
                           PROJStringFormatter::Convention::PROJ_5,
                           authFactory->databaseContext())
                           .get()),
                   "+step +proj=unitconvert +xy_in=deg +xy_out=rad",
                   "+step +proj=unitconvert +xy_in=deg +xy_out=rad "
                   "+step +proj=affine +s33=-0.304800609601219"));

    // Check reverse path
    auto listGeogToCompound =
        CoordinateOperationFactory::create()->createOperations(dst, nnSrc,
                                                               ctxt);
    EXPECT_EQ(listGeogToCompound.size(), listCompoundToGeog.size());
}

// ---------------------------------------------------------------------------

TEST(operation, compoundCRS_of_vertCRS_with_geoid_model_to_geogCRS) {
    auto authFactory =
        AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    auto ctxt = CoordinateOperationContext::create(authFactory, nullptr, 0.0);
    ctxt->setSpatialCriterion(
        CoordinateOperationContext::SpatialCriterion::PARTIAL_INTERSECTION);
    ctxt->setGridAvailabilityUse(
        CoordinateOperationContext::GridAvailabilityUse::
            IGNORE_GRID_AVAILABILITY);
    auto wkt =
        "COMPOUNDCRS[\"NAD83 / Pennsylvania South + NAVD88 height\",\n"
        "    PROJCRS[\"NAD83 / Pennsylvania South\",\n"
        "        BASEGEOGCRS[\"NAD83\",\n"
        "            DATUM[\"North American Datum 1983\",\n"
        "                ELLIPSOID[\"GRS 1980\",6378137,298.257222101,\n"
        "                    LENGTHUNIT[\"metre\",1]]],\n"
        "            PRIMEM[\"Greenwich\",0,\n"
        "                ANGLEUNIT[\"degree\",0.0174532925199433]]],\n"
        "        CONVERSION[\"SPCS83 Pennsylvania South zone (meters)\",\n"
        "            METHOD[\"Lambert Conic Conformal (2SP)\",\n"
        "                ID[\"EPSG\",9802]],\n"
        "            PARAMETER[\"Latitude of false origin\",39.3333333333333,\n"
        "                ANGLEUNIT[\"degree\",0.0174532925199433],\n"
        "                ID[\"EPSG\",8821]],\n"
        "            PARAMETER[\"Longitude of false origin\",-77.75,\n"
        "                ANGLEUNIT[\"degree\",0.0174532925199433],\n"
        "                ID[\"EPSG\",8822]],\n"
        "            PARAMETER[\"Latitude of 1st standard "
        "parallel\",40.9666666666667,\n"
        "                ANGLEUNIT[\"degree\",0.0174532925199433],\n"
        "                ID[\"EPSG\",8823]],\n"
        "            PARAMETER[\"Latitude of 2nd standard "
        "parallel\",39.9333333333333,\n"
        "                ANGLEUNIT[\"degree\",0.0174532925199433],\n"
        "                ID[\"EPSG\",8824]],\n"
        "            PARAMETER[\"Easting at false origin\",600000,\n"
        "                LENGTHUNIT[\"metre\",1],\n"
        "                ID[\"EPSG\",8826]],\n"
        "            PARAMETER[\"Northing at false origin\",0,\n"
        "                LENGTHUNIT[\"metre\",1],\n"
        "                ID[\"EPSG\",8827]]],\n"
        "        CS[Cartesian,2],\n"
        "            AXIS[\"easting (X)\",east,\n"
        "                ORDER[1],\n"
        "                LENGTHUNIT[\"metre\",1]],\n"
        "            AXIS[\"northing (Y)\",north,\n"
        "                ORDER[2],\n"
        "                LENGTHUNIT[\"metre\",1]]],\n"
        "    VERTCRS[\"NAVD88 height\",\n"
        "        VDATUM[\"North American Vertical Datum 1988\"],\n"
        "        CS[vertical,1],\n"
        "            AXIS[\"gravity-related height (H)\",up,\n"
        "                LENGTHUNIT[\"metre\",1]],\n"
        "        GEOIDMODEL[\"GEOID12B\"]]]";
    auto srcObj =
        createFromUserInput(wkt, authFactory->databaseContext(), false);
    auto src = nn_dynamic_pointer_cast<CRS>(srcObj);
    ASSERT_TRUE(src != nullptr);
    auto dst =
        authFactory->createCoordinateReferenceSystem("4269")->promoteTo3D(
            std::string(), authFactory->databaseContext()); // NAD83

    auto list = CoordinateOperationFactory::create()->createOperations(
        NN_NO_CHECK(src), dst, ctxt);
    ASSERT_TRUE(!list.empty());
    EXPECT_EQ(list[0]->nameStr(),
              "Inverse of SPCS83 Pennsylvania South zone (meters) + "
              "Ballpark geographic offset from NAD83 to NAD83(2011) + "
              "Inverse of NAD83(2011) to NAVD88 height (1) + "
              "Ballpark geographic offset from NAD83(2011) to NAD83");
    auto op_proj =
        list[0]->exportToPROJString(PROJStringFormatter::create().get());
    EXPECT_EQ(
        op_proj,
        "+proj=pipeline "
        "+step +inv +proj=lcc +lat_0=39.3333333333333 +lon_0=-77.75 "
        "+lat_1=40.9666666666667 +lat_2=39.9333333333333 +x_0=600000 "
        "+y_0=0 +ellps=GRS80 "
        "+step +proj=vgridshift +grids=us_noaa_g2012bu0.tif +multiplier=1 "
        "+step +proj=unitconvert +xy_in=rad +xy_out=deg "
        "+step +proj=axisswap +order=2,1");
}

// ---------------------------------------------------------------------------

TEST(operation,
     compoundCRS_of_horizCRS_with_TOWGS84_vertCRS_with_geoid_model_to_geogCRS) {
    auto authFactory =
        AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    auto ctxt = CoordinateOperationContext::create(authFactory, nullptr, 0.0);
    ctxt->setSpatialCriterion(
        CoordinateOperationContext::SpatialCriterion::PARTIAL_INTERSECTION);
    ctxt->setGridAvailabilityUse(
        CoordinateOperationContext::GridAvailabilityUse::
            IGNORE_GRID_AVAILABILITY);
    auto wkt = "COMPD_CS[\"NAD83(CSRS) + CGVD28 height - HT2_0\",\n"
               "    GEOGCS[\"NAD83(CSRS)\",\n"
               "        DATUM[\"NAD83_Canadian_Spatial_Reference_System\",\n"
               "            SPHEROID[\"GRS 1980\",6378137,298.257222101,\n"
               "                AUTHORITY[\"EPSG\",\"7019\"]],\n"
               "            TOWGS84[0,0,0,0,0,0,0],\n"
               "            AUTHORITY[\"EPSG\",\"6140\"]],\n"
               "        PRIMEM[\"Greenwich\",0,\n"
               "            AUTHORITY[\"EPSG\",\"8901\"]],\n"
               "        UNIT[\"degree\",0.0174532925199433,\n"
               "            AUTHORITY[\"EPSG\",\"9122\"]],\n"
               "        AUTHORITY[\"EPSG\",\"4617\"]],\n"
               "    VERT_CS[\"CGVD28 height - HT2_0\",\n"
               "        VERT_DATUM[\"Canadian Geodetic Vertical Datum of "
               "1928\",2005,\n"
               "            EXTENSION[\"PROJ4_GRIDS\",\"HT2_0.gtx\"],\n"
               "            AUTHORITY[\"EPSG\",\"5114\"]],\n"
               "        UNIT[\"metre\",1,\n"
               "            AUTHORITY[\"EPSG\",\"9001\"]],\n"
               "        AXIS[\"Gravity-related height\",UP],\n"
               "        AUTHORITY[\"EPSG\",\"5713\"]]]";
    auto srcObj =
        createFromUserInput(wkt, authFactory->databaseContext(), false);
    auto src = nn_dynamic_pointer_cast<CRS>(srcObj);
    ASSERT_TRUE(src != nullptr);
    // NAD83(CSRS) 3D
    auto dst = authFactory->createCoordinateReferenceSystem("4955");

    auto list = CoordinateOperationFactory::create()->createOperations(
        NN_NO_CHECK(src), dst, ctxt);
    ASSERT_EQ(list.size(), 1U);
    auto op_proj =
        list[0]->exportToPROJString(PROJStringFormatter::create().get());
    EXPECT_EQ(op_proj, "+proj=pipeline "
                       "+step +proj=axisswap +order=2,1 "
                       "+step +proj=unitconvert +xy_in=deg +xy_out=rad "
                       "+step +proj=vgridshift +grids=HT2_0.gtx +multiplier=1 "
                       "+step +proj=unitconvert +xy_in=rad +xy_out=deg "
                       "+step +proj=axisswap +order=2,1");
}

// ---------------------------------------------------------------------------

TEST(operation, compoundCRS_of_bound_horizCRS_and_bound_vertCRS_to_geogCRS) {
    auto authFactory =
        AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    auto ctxt = CoordinateOperationContext::create(authFactory, nullptr, 0.0);
    ctxt->setGridAvailabilityUse(
        CoordinateOperationContext::GridAvailabilityUse::
            IGNORE_GRID_AVAILABILITY);
    auto wkt =
        "COMPOUNDCRS[\"CH1903 / LV03 + EGM96 height\",\n"
        "    BOUNDCRS[\n"
        "        SOURCECRS[\n"
        "            PROJCRS[\"CH1903 / LV03\",\n"
        "                BASEGEOGCRS[\"CH1903\",\n"
        "                    DATUM[\"CH1903\",\n"
        "                        ELLIPSOID[\"Bessel "
        "1841\",6377397.155,299.1528128,\n"
        "                            LENGTHUNIT[\"metre\",1]]],\n"
        "                    PRIMEM[\"Greenwich\",0,\n"
        "                        ANGLEUNIT[\"degree\",0.0174532925199433]],\n"
        "                    ID[\"EPSG\",4149]],\n"
        "                CONVERSION[\"Swiss Oblique Mercator 1903M\",\n"
        "                    METHOD[\"Hotine Oblique Mercator (variant B)\",\n"
        "                        ID[\"EPSG\",9815]],\n"
        "                    PARAMETER[\"Latitude of projection "
        "centre\",46.9524055555556,\n"
        "                        ANGLEUNIT[\"degree\",0.0174532925199433],\n"
        "                        ID[\"EPSG\",8811]],\n"
        "                    PARAMETER[\"Longitude of projection "
        "centre\",7.43958333333333,\n"
        "                        ANGLEUNIT[\"degree\",0.0174532925199433],\n"
        "                        ID[\"EPSG\",8812]],\n"
        "                    PARAMETER[\"Azimuth of initial line\",90,\n"
        "                        ANGLEUNIT[\"degree\",0.0174532925199433],\n"
        "                        ID[\"EPSG\",8813]],\n"
        "                    PARAMETER[\"Angle from Rectified to Skew "
        "Grid\",90,\n"
        "                        ANGLEUNIT[\"degree\",0.0174532925199433],\n"
        "                        ID[\"EPSG\",8814]],\n"
        "                    PARAMETER[\"Scale factor on initial line\",1,\n"
        "                        SCALEUNIT[\"unity\",1],\n"
        "                        ID[\"EPSG\",8815]],\n"
        "                    PARAMETER[\"Easting at projection "
        "centre\",600000,\n"
        "                        LENGTHUNIT[\"metre\",1],\n"
        "                        ID[\"EPSG\",8816]],\n"
        "                    PARAMETER[\"Northing at projection "
        "centre\",200000,\n"
        "                        LENGTHUNIT[\"metre\",1],\n"
        "                        ID[\"EPSG\",8817]]],\n"
        "                CS[Cartesian,2],\n"
        "                    AXIS[\"easting (Y)\",east,\n"
        "                        ORDER[1],\n"
        "                        LENGTHUNIT[\"metre\",1]],\n"
        "                    AXIS[\"northing (X)\",north,\n"
        "                        ORDER[2],\n"
        "                        LENGTHUNIT[\"metre\",1]]]],\n"
        "        TARGETCRS[\n"
        "            GEOGCRS[\"WGS 84\",\n"
        "                DATUM[\"World Geodetic System 1984\",\n"
        "                    ELLIPSOID[\"WGS 84\",6378137,298.257223563,\n"
        "                        LENGTHUNIT[\"metre\",1]]],\n"
        "                PRIMEM[\"Greenwich\",0,\n"
        "                    ANGLEUNIT[\"degree\",0.0174532925199433]],\n"
        "                CS[ellipsoidal,2],\n"
        "                    AXIS[\"latitude\",north,\n"
        "                        ORDER[1],\n"
        "                        ANGLEUNIT[\"degree\",0.0174532925199433]],\n"
        "                    AXIS[\"longitude\",east,\n"
        "                        ORDER[2],\n"
        "                        ANGLEUNIT[\"degree\",0.0174532925199433]],\n"
        "                ID[\"EPSG\",4326]]],\n"
        "        ABRIDGEDTRANSFORMATION[\"CH1903 to WGS 84 (2)\",\n"
        "            VERSION[\"BfL-CH 2\"],\n"
        "            METHOD[\"Geocentric translations (geog2D domain)\",\n"
        "                ID[\"EPSG\",9603]],\n"
        "            PARAMETER[\"X-axis translation\",674.374,\n"
        "                ID[\"EPSG\",8605]],\n"
        "            PARAMETER[\"Y-axis translation\",15.056,\n"
        "                ID[\"EPSG\",8606]],\n"
        "            PARAMETER[\"Z-axis translation\",405.346,\n"
        "                ID[\"EPSG\",8607]]]],\n"
        "    BOUNDCRS[\n"
        "        SOURCECRS[\n"
        "            VERTCRS[\"EGM96 height\",\n"
        "                VDATUM[\"EGM96 geoid\"],\n"
        "                CS[vertical,1],\n"
        "                    AXIS[\"gravity-related height (H)\",up,\n"
        "                        LENGTHUNIT[\"metre\",1]],\n"
        "                USAGE[\n"
        "                    SCOPE[\"Geodesy.\"],\n"
        "                    AREA[\"World.\"],\n"
        "                    BBOX[-90,-180,90,180]],\n"
        "                ID[\"EPSG\",5773]]],\n"
        "        TARGETCRS[\n"
        "            GEOGCRS[\"WGS 84\",\n"
        "                DATUM[\"World Geodetic System 1984\",\n"
        "                    ELLIPSOID[\"WGS 84\",6378137,298.257223563,\n"
        "                        LENGTHUNIT[\"metre\",1]]],\n"
        "                PRIMEM[\"Greenwich\",0,\n"
        "                    ANGLEUNIT[\"degree\",0.0174532925199433]],\n"
        "                CS[ellipsoidal,3],\n"
        "                    AXIS[\"latitude\",north,\n"
        "                        ORDER[1],\n"
        "                        ANGLEUNIT[\"degree\",0.0174532925199433]],\n"
        "                    AXIS[\"longitude\",east,\n"
        "                        ORDER[2],\n"
        "                        ANGLEUNIT[\"degree\",0.0174532925199433]],\n"
        "                    AXIS[\"ellipsoidal height\",up,\n"
        "                        ORDER[3],\n"
        "                        LENGTHUNIT[\"metre\",1]],\n"
        "                ID[\"EPSG\",4979]]],\n"
        "        ABRIDGEDTRANSFORMATION[\"WGS 84 to EGM96 height (1)\",\n"
        "            METHOD[\"Geographic3D to GravityRelatedHeight (EGM)\",\n"
        "                ID[\"EPSG\",9661]],\n"
        "            PARAMETERFILE[\"Geoid (height correction) model "
        "file\",\"us_nga_egm96_15.tif\"]]]]";
    auto srcObj =
        createFromUserInput(wkt, authFactory->databaseContext(), false);
    auto src = nn_dynamic_pointer_cast<CRS>(srcObj);
    ASSERT_TRUE(src != nullptr);
    // WGS 84 3D
    auto dst = authFactory->createCoordinateReferenceSystem("4979");

    auto list = CoordinateOperationFactory::create()->createOperations(
        NN_NO_CHECK(src), dst, ctxt);
    ASSERT_EQ(list.size(), 1U);
    auto op_proj =
        list[0]->exportToPROJString(PROJStringFormatter::create().get());
    EXPECT_EQ(op_proj,
              "+proj=pipeline "
              "+step +inv +proj=somerc +lat_0=46.9524055555556 "
              "+lon_0=7.43958333333333 +k_0=1 "
              "+x_0=600000 +y_0=200000 +ellps=bessel "
              "+step +proj=push +v_3 "
              "+step +proj=cart +ellps=bessel "
              "+step +proj=helmert +x=674.374 +y=15.056 +z=405.346 "
              "+step +inv +proj=cart +ellps=WGS84 "
              "+step +proj=pop +v_3 "
              "+step +proj=vgridshift +grids=us_nga_egm96_15.tif +multiplier=1 "
              "+step +proj=unitconvert +xy_in=rad +xy_out=deg "
              "+step +proj=axisswap +order=2,1");
}

// ---------------------------------------------------------------------------

TEST(operation, compoundCRS_from_WKT2_to_geogCRS_3D_context) {
    auto authFactory =
        AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    auto ctxt = CoordinateOperationContext::create(authFactory, nullptr, 0.0);
    auto src = authFactory->createCoordinateReferenceSystem(
        "7415"); // Amersfoort / RD New + NAP height
    auto dst =
        authFactory->createCoordinateReferenceSystem("4937"); // ETRS89 3D
    auto list =
        CoordinateOperationFactory::create()->createOperations(src, dst, ctxt);
    ASSERT_GE(list.size(), 1U);
    auto wkt2 = src->exportToWKT(
        WKTFormatter::create(WKTFormatter::Convention::WKT2_2019).get());
    auto obj = WKTParser().createFromWKT(wkt2);
    auto src_from_wkt2 = nn_dynamic_pointer_cast<CRS>(obj);
    ASSERT_TRUE(src_from_wkt2 != nullptr);
    auto list2 = CoordinateOperationFactory::create()->createOperations(
        NN_NO_CHECK(src_from_wkt2), dst, ctxt);
    ASSERT_GE(list.size(), list2.size());
    for (size_t i = 0; i < list.size(); i++) {
        const auto &op = list[i];
        const auto &op2 = list2[i];
        EXPECT_TRUE(
            op->isEquivalentTo(op2.get(), IComparable::Criterion::EQUIVALENT));
    }
}

// ---------------------------------------------------------------------------

TEST(operation, compoundCRS_from_WKT2_no_id_to_geogCRS_3D_context) {
    auto authFactory =
        AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    auto ctxt = CoordinateOperationContext::create(authFactory, nullptr, 0.0);
    ctxt->setSpatialCriterion(
        CoordinateOperationContext::SpatialCriterion::PARTIAL_INTERSECTION);
    auto src = authFactory->createCoordinateReferenceSystem(
        "7415"); // Amersfoort / RD New + NAP height
    auto dst =
        authFactory->createCoordinateReferenceSystem("4937"); // ETRS89 3D
    auto list =
        CoordinateOperationFactory::create()->createOperations(src, dst, ctxt);
    ASSERT_GE(list.size(), 1U);

    {
        auto op_proj =
            list[0]->exportToPROJString(PROJStringFormatter::create().get());
        EXPECT_EQ(
            op_proj,
            "+proj=pipeline +step +inv +proj=sterea +lat_0=52.1561605555556 "
            "+lon_0=5.38763888888889 +k=0.9999079 +x_0=155000 +y_0=463000 "
            "+ellps=bessel "
            "+step +proj=hgridshift +grids=nl_nsgi_rdtrans2018.tif "
            "+step +proj=vgridshift +grids=nl_nsgi_nlgeo2018.tif +multiplier=1 "
            "+step +proj=unitconvert +xy_in=rad +xy_out=deg "
            "+step +proj=axisswap +order=2,1");
    }

    auto wkt2 =
        "COMPOUNDCRS[\"unknown\",\n"
        "  PROJCRS[\"unknown\",\n"
        "    BASEGEOGCRS[\"Amersfoort\",\n"
        "            DATUM[\"Amersfoort\",\n"
        "                ELLIPSOID[\"Bessel "
        "1841\",6377397.155,299.1528128]]],\n"
        "    CONVERSION[\"unknown\",\n"
        "        METHOD[\"Oblique Stereographic\"],\n"
        "        PARAMETER[\"Latitude of natural origin\",52.1561605555556],\n"
        "        PARAMETER[\"Longitude of natural origin\",5.38763888888889],\n"
        "        PARAMETER[\"Scale factor at natural origin\",0.9999079],\n"
        "        PARAMETER[\"False easting\",155000],\n"
        "        PARAMETER[\"False northing\",463000]],\n"
        "    CS[Cartesian,2],\n"
        "        AXIS[\"(E)\",east],\n"
        "        AXIS[\"(N)\",north],\n"
        "    LENGTHUNIT[\"metre\",1]],\n"
        "  VERTCRS[\"NAP height\",\n"
        "    VDATUM[\"Normaal Amsterdams Peil\"],\n"
        "    CS[vertical,1],\n"
        "        AXIS[\"gravity-related height (H)\",up,\n"
        "        LENGTHUNIT[\"metre\",1]]],\n"
        "    USAGE[\n"
        "        SCOPE[\"unknown\"],\n"
        "        AREA[\"Netherlands - onshore\"],\n"
        "        BBOX[50.75,3.2,53.7,7.22]]]";

    auto obj = WKTParser().createFromWKT(wkt2);
    auto src_from_wkt2 = nn_dynamic_pointer_cast<CRS>(obj);
    ASSERT_TRUE(src_from_wkt2 != nullptr);
    auto list2 = CoordinateOperationFactory::create()->createOperations(
        NN_NO_CHECK(src_from_wkt2), dst, ctxt);
    ASSERT_EQ(list.size(), list2.size());
    for (size_t i = 0; i < list.size(); i++) {
        const auto &op = list[i];
        const auto &op2 = list2[i];
        auto op_proj =
            op->exportToPROJString(PROJStringFormatter::create().get());
        auto op2_proj =
            op2->exportToPROJString(PROJStringFormatter::create().get());
        EXPECT_EQ(op_proj, op2_proj)
            << "op=" << op->nameStr() << " op2=" << op2->nameStr();
    }
}

// ---------------------------------------------------------------------------

TEST(operation, proj3DCRS_with_non_meter_horiz_and_vertical_to_geog) {
    auto objSrc = PROJStringParser().createFromPROJString(
        "+proj=utm +zone=31 +datum=WGS84 +units=us-ft +vunits=us-ft +type=crs");
    auto src = nn_dynamic_pointer_cast<CRS>(objSrc);
    ASSERT_TRUE(src != nullptr);
    auto authFactory =
        AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    auto ctxt = CoordinateOperationContext::create(authFactory, nullptr, 0.0);
    auto list = CoordinateOperationFactory::create()->createOperations(
        NN_NO_CHECK(src), authFactory->createCoordinateReferenceSystem("4326"),
        ctxt);
    ASSERT_EQ(list.size(), 1U);
    // Check that vertical unit conversion is done just once
    EXPECT_EQ(list[0]->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=pipeline "
              "+step +proj=unitconvert +xy_in=us-ft +z_in=us-ft "
              "+xy_out=m +z_out=m "
              "+step +inv +proj=utm +zone=31 +ellps=WGS84 "
              "+step +proj=unitconvert +xy_in=rad +xy_out=deg "
              "+step +proj=axisswap +order=2,1");
}

// ---------------------------------------------------------------------------

TEST(operation, compoundCRS_with_non_meter_horiz_and_vertical_to_geog) {
    auto objSrc = WKTParser().createFromWKT(
        "COMPOUNDCRS[\"unknown\",\n"
        "    PROJCRS[\"unknown\",\n"
        "        BASEGEOGCRS[\"unknown\",\n"
        "            DATUM[\"World Geodetic System 1984\",\n"
        "                ELLIPSOID[\"WGS 84\",6378137,298.257223563,\n"
        "                    LENGTHUNIT[\"metre\",1]],\n"
        "                ID[\"EPSG\",6326]],\n"
        "            PRIMEM[\"Greenwich\",0,\n"
        "                ANGLEUNIT[\"degree\",0.0174532925199433],\n"
        "                ID[\"EPSG\",8901]]],\n"
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
        "                ID[\"EPSG\",8807]],\n"
        "            ID[\"EPSG\",16031]],\n"
        "        CS[Cartesian,2],\n"
        "            AXIS[\"(E)\",east,\n"
        "                ORDER[1],\n"
        "                LENGTHUNIT[\"US survey foot\",0.304800609601219,\n"
        "                    ID[\"EPSG\",9003]]],\n"
        "            AXIS[\"(N)\",north,\n"
        "                ORDER[2],\n"
        "                LENGTHUNIT[\"US survey foot\",0.304800609601219,\n"
        "                    ID[\"EPSG\",9003]]]],\n"
        "    VERTCRS[\"unknown\",\n"
        "        VDATUM[\"unknown\"],\n"
        "        CS[vertical,1],\n"
        "            AXIS[\"gravity-related height (H)\",up,\n"
        "                LENGTHUNIT[\"US survey foot\",0.304800609601219,\n"
        "                    ID[\"EPSG\",9003]]]]]"

    );
    auto src = nn_dynamic_pointer_cast<CRS>(objSrc);
    ASSERT_TRUE(src != nullptr);
    auto authFactory =
        AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    auto ctxt = CoordinateOperationContext::create(authFactory, nullptr, 0.0);
    auto list = CoordinateOperationFactory::create()->createOperations(
        NN_NO_CHECK(src), authFactory->createCoordinateReferenceSystem("4979"),
        ctxt);
    ASSERT_EQ(list.size(), 1U);
    // Check that vertical unit conversion is done just once
    EXPECT_EQ(list[0]->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=pipeline "
              "+step +proj=unitconvert +xy_in=us-ft +xy_out=m "
              "+step +inv +proj=utm +zone=31 +ellps=WGS84 "
              "+step +proj=unitconvert +xy_in=rad +z_in=us-ft "
              "+xy_out=deg +z_out=m "
              "+step +proj=axisswap +order=2,1");
}

// ---------------------------------------------------------------------------

TEST(operation, boundCRS_to_compoundCRS) {
    auto objSrc = PROJStringParser().createFromPROJString(
        "+proj=longlat +ellps=GRS67 +nadgrids=@foo.gsb +type=crs");
    auto src = nn_dynamic_pointer_cast<CRS>(objSrc);
    ASSERT_TRUE(src != nullptr);
    auto objDst = PROJStringParser().createFromPROJString(
        "+proj=longlat +ellps=GRS80 +nadgrids=@bar.gsb +geoidgrids=@bar.gtx "
        "+geoid_crs=horizontal_crs "
        "+type=crs");
    auto dst = nn_dynamic_pointer_cast<CRS>(objDst);
    ASSERT_TRUE(dst != nullptr);

    auto op = CoordinateOperationFactory::create()->createOperation(
        NN_CHECK_ASSERT(src), NN_CHECK_ASSERT(dst));
    ASSERT_TRUE(op != nullptr);
    EXPECT_EQ(op->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=pipeline "
              "+step +proj=unitconvert +xy_in=deg +xy_out=rad "
              "+step +proj=hgridshift +grids=@foo.gsb "
              "+step +inv +proj=hgridshift +grids=@bar.gsb "
              "+step +inv +proj=vgridshift +grids=@bar.gtx +multiplier=1 "
              "+step +proj=unitconvert +xy_in=rad +xy_out=deg");

    auto opInverse = CoordinateOperationFactory::create()->createOperation(
        NN_CHECK_ASSERT(dst), NN_CHECK_ASSERT(src));
    ASSERT_TRUE(opInverse != nullptr);
    EXPECT_TRUE(opInverse->inverse()->_isEquivalentTo(op.get()));
}

// ---------------------------------------------------------------------------

TEST(operation,
     boundCRS_to_compoundCRS_with_hubCRS_same_as_compound_geographicCRS) {
    auto authFactory =
        AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    auto ctxt = CoordinateOperationContext::create(authFactory, nullptr, 0.0);
    ctxt->setSpatialCriterion(
        CoordinateOperationContext::SpatialCriterion::PARTIAL_INTERSECTION);
    ctxt->setGridAvailabilityUse(
        CoordinateOperationContext::GridAvailabilityUse::
            IGNORE_GRID_AVAILABILITY);
    auto wkt =
        "BOUNDCRS[\n"
        "    SOURCECRS[\n"
        "        PROJCRS[\"CH1903 / LV03\",\n"
        "            BASEGEOGCRS[\"CH1903\",\n"
        "                DATUM[\"CH1903\",\n"
        "                    ELLIPSOID[\"Bessel "
        "1841\",6377397.155,299.1528128,\n"
        "                        LENGTHUNIT[\"metre\",1]],\n"
        "                    ID[\"EPSG\",6149]],\n"
        "                PRIMEM[\"Greenwich\",0,\n"
        "                    ANGLEUNIT[\"degree\",0.0174532925199433],\n"
        "                    ID[\"EPSG\",8901]]],\n"
        "            CONVERSION[\"unnamed\",\n"
        "                METHOD[\"Hotine Oblique Mercator (variant B)\",\n"
        "                    ID[\"EPSG\",9815]],\n"
        "                PARAMETER[\"Latitude of projection "
        "centre\",46.9524055555556,\n"
        "                    ANGLEUNIT[\"degree\",0.0174532925199433],\n"
        "                    ID[\"EPSG\",8811]],\n"
        "                PARAMETER[\"Longitude of projection "
        "centre\",7.43958333333333,\n"
        "                    ANGLEUNIT[\"degree\",0.0174532925199433],\n"
        "                    ID[\"EPSG\",8812]],\n"
        "                PARAMETER[\"Azimuth of initial line\",90,\n"
        "                    ANGLEUNIT[\"degree\",0.0174532925199433],\n"
        "                    ID[\"EPSG\",8813]],\n"
        "                PARAMETER[\"Angle from Rectified to Skew Grid\",90,\n"
        "                    ANGLEUNIT[\"degree\",0.0174532925199433],\n"
        "                    ID[\"EPSG\",8814]],\n"
        "                PARAMETER[\"Scale factor on initial line\",1,\n"
        "                    SCALEUNIT[\"unity\",1],\n"
        "                    ID[\"EPSG\",8815]],\n"
        "                PARAMETER[\"Easting at projection centre\",600000,\n"
        "                    LENGTHUNIT[\"metre\",1],\n"
        "                    ID[\"EPSG\",8816]],\n"
        "                PARAMETER[\"Northing at projection centre\",200000,\n"
        "                    LENGTHUNIT[\"metre\",1],\n"
        "                    ID[\"EPSG\",8817]]],\n"
        "            CS[Cartesian,3],\n"
        "                AXIS[\"y\",east,\n"
        "                    ORDER[1],\n"
        "                    LENGTHUNIT[\"metre\",1,\n"
        "                        ID[\"EPSG\",9001]]],\n"
        "                AXIS[\"x\",north,\n"
        "                    ORDER[2],\n"
        "                    LENGTHUNIT[\"metre\",1,\n"
        "                        ID[\"EPSG\",9001]]],\n"
        "                AXIS[\"ellipsoidal height (h)\",up,\n"
        "                    ORDER[3],\n"
        "                    LENGTHUNIT[\"metre\",1,\n"
        "                        ID[\"EPSG\",9001]]]]],\n"
        "    TARGETCRS[\n"
        "        GEOGCRS[\"WGS 84\",\n"
        "            ENSEMBLE[\"World Geodetic System 1984 ensemble\",\n"
        "                MEMBER[\"World Geodetic System 1984 (Transit)\"],\n"
        "                MEMBER[\"World Geodetic System 1984 (G730)\"],\n"
        "                MEMBER[\"World Geodetic System 1984 (G873)\"],\n"
        "                MEMBER[\"World Geodetic System 1984 (G1150)\"],\n"
        "                MEMBER[\"World Geodetic System 1984 (G1674)\"],\n"
        "                MEMBER[\"World Geodetic System 1984 (G1762)\"],\n"
        "                MEMBER[\"World Geodetic System 1984 (G2139)\"],\n"
        "                ELLIPSOID[\"WGS 84\",6378137,298.257223563,\n"
        "                    LENGTHUNIT[\"metre\",1]],\n"
        "                ENSEMBLEACCURACY[2.0]],\n"
        "            PRIMEM[\"Greenwich\",0,\n"
        "                ANGLEUNIT[\"degree\",0.0174532925199433]],\n"
        "            CS[ellipsoidal,3],\n"
        "                AXIS[\"geodetic latitude (Lat)\",north,\n"
        "                    ORDER[1],\n"
        "                    ANGLEUNIT[\"degree\",0.0174532925199433]],\n"
        "                AXIS[\"geodetic longitude (Lon)\",east,\n"
        "                    ORDER[2],\n"
        "                    ANGLEUNIT[\"degree\",0.0174532925199433]],\n"
        "                AXIS[\"ellipsoidal height (h)\",up,\n"
        "                    ORDER[3],\n"
        "                    LENGTHUNIT[\"metre\",1]],\n"
        "            ID[\"EPSG\",4979]]],\n"
        "    ABRIDGEDTRANSFORMATION[\"Transformation from CH1903 to WGS84\",\n"
        "        METHOD[\"Position Vector transformation (geog2D domain)\",\n"
        "            ID[\"EPSG\",9606]],\n"
        "        PARAMETER[\"X-axis translation\",674.4,\n"
        "            ID[\"EPSG\",8605]],\n"
        "        PARAMETER[\"Y-axis translation\",15.1,\n"
        "            ID[\"EPSG\",8606]],\n"
        "        PARAMETER[\"Z-axis translation\",405.3,\n"
        "            ID[\"EPSG\",8607]],\n"
        "        PARAMETER[\"X-axis rotation\",0,\n"
        "            ID[\"EPSG\",8608]],\n"
        "        PARAMETER[\"Y-axis rotation\",0,\n"
        "            ID[\"EPSG\",8609]],\n"
        "        PARAMETER[\"Z-axis rotation\",0,\n"
        "            ID[\"EPSG\",8610]],\n"
        "        PARAMETER[\"Scale difference\",1,\n"
        "            ID[\"EPSG\",8611]]]]";
    auto srcObj =
        createFromUserInput(wkt, authFactory->databaseContext(), false);
    auto src = nn_dynamic_pointer_cast<CRS>(srcObj);
    ASSERT_TRUE(src != nullptr);
    auto dst = authFactory->createCoordinateReferenceSystem(
        "9518"); // "WGS 84 + EGM2008 height"

    auto list = CoordinateOperationFactory::create()->createOperations(
        NN_NO_CHECK(src), dst, ctxt);
    ASSERT_GE(list.size(), 1U);
    // Check that BoundCRS helmert transformation is used
    EXPECT_EQ(list[0]->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=pipeline "
              "+step +inv +proj=somerc +lat_0=46.9524055555556 "
              "+lon_0=7.43958333333333 +k_0=1 "
              "+x_0=600000 +y_0=200000 +ellps=bessel "
              "+step +proj=cart +ellps=bessel "
              "+step +proj=helmert +x=674.4 +y=15.1 +z=405.3 +rx=0 +ry=0 +rz=0 "
              "+s=0 +convention=position_vector "
              "+step +inv +proj=cart +ellps=WGS84 "
              "+step +inv +proj=vgridshift +grids=us_nga_egm08_25.tif "
              "+multiplier=1 "
              "+step +proj=unitconvert +xy_in=rad +xy_out=deg "
              "+step +proj=axisswap +order=2,1");
}

// ---------------------------------------------------------------------------

TEST(operation, IGNF_LAMB1_TO_EPSG_4326) {
    auto authFactory =
        AuthorityFactory::create(DatabaseContext::create(), std::string());
    auto ctxt = CoordinateOperationContext::create(authFactory, nullptr, 0.0);
    ctxt->setGridAvailabilityUse(
        CoordinateOperationContext::GridAvailabilityUse::
            IGNORE_GRID_AVAILABILITY);
    ctxt->setAllowUseIntermediateCRS(
        CoordinateOperationContext::IntermediateCRSUse::ALWAYS);
    auto list = CoordinateOperationFactory::create()->createOperations(
        AuthorityFactory::create(DatabaseContext::create(), "IGNF")
            ->createCoordinateReferenceSystem("LAMB1"),
        AuthorityFactory::create(DatabaseContext::create(), "EPSG")
            ->createCoordinateReferenceSystem("4326"),
        ctxt);
    ASSERT_EQ(list.size(), 2U);

    EXPECT_FALSE(list[0]->hasBallparkTransformation());
    EXPECT_EQ(list[0]->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=pipeline +step +inv +proj=lcc +lat_1=49.5 +lat_0=49.5 "
              "+lon_0=0 +k_0=0.99987734 +x_0=600000 +y_0=200000 "
              "+ellps=clrk80ign +pm=paris +step +proj=hgridshift "
              "+grids=fr_ign_ntf_r93.tif +step +proj=unitconvert +xy_in=rad "
              "+xy_out=deg +step +proj=axisswap +order=2,1");

    EXPECT_FALSE(list[1]->hasBallparkTransformation());
    EXPECT_EQ(list[1]->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=pipeline +step +inv +proj=lcc +lat_1=49.5 +lat_0=49.5 "
              "+lon_0=0 +k_0=0.99987734 +x_0=600000 +y_0=200000 "
              "+ellps=clrk80ign +pm=paris +step +proj=push +v_3 +step "
              "+proj=cart +ellps=clrk80ign +step +proj=helmert +x=-168 +y=-60 "
              "+z=320 +step +inv +proj=cart +ellps=WGS84 +step +proj=pop +v_3 "
              "+step +proj=unitconvert +xy_in=rad +xy_out=deg +step "
              "+proj=axisswap +order=2,1");

    auto list2 = CoordinateOperationFactory::create()->createOperations(
        AuthorityFactory::create(DatabaseContext::create(), "EPSG")
            // NTF (Paris) / Lambert Nord France equivalent to IGNF:LAMB1
            ->createCoordinateReferenceSystem("27561"),
        AuthorityFactory::create(DatabaseContext::create(), "EPSG")
            ->createCoordinateReferenceSystem("4326"),
        ctxt);
    ASSERT_GE(list2.size(), 3U);

    EXPECT_EQ(replaceAll(list2[0]->exportToPROJString(
                             PROJStringFormatter::create().get()),
                         "0.999877341", "0.99987734"),
              list[0]->exportToPROJString(PROJStringFormatter::create().get()));

    // The second entry in list2 (list2[1]) uses the
    // weird +pm=2.33720833333333 from "NTF (Paris) to NTF (2)"
    // so skip to the 3th method
    EXPECT_EQ(replaceAll(list2[2]->exportToPROJString(
                             PROJStringFormatter::create().get()),
                         "0.999877341", "0.99987734"),
              list[1]->exportToPROJString(PROJStringFormatter::create().get()));
}

// ---------------------------------------------------------------------------

TEST(operation, NAD83_to_projeted_CRS_based_on_NAD83_2011) {
    auto authFactory =
        AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    auto ctxt = CoordinateOperationContext::create(authFactory, nullptr, 0.0);
    ctxt->setSpatialCriterion(
        CoordinateOperationContext::SpatialCriterion::PARTIAL_INTERSECTION);
    auto list = CoordinateOperationFactory::create()->createOperations(
        // NAD83
        authFactory->createCoordinateReferenceSystem("4269"),
        // NAD83(2011) / California Albers
        authFactory->createCoordinateReferenceSystem("6414"), ctxt);
    ASSERT_EQ(list.size(), 1U);
    EXPECT_EQ(list[0]->nameStr(), "Ballpark geographic offset from NAD83 to "
                                  "NAD83(2011) + California Albers");
    EXPECT_EQ(list[0]->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=pipeline +step +proj=axisswap +order=2,1 "
              "+step +proj=unitconvert +xy_in=deg +xy_out=rad "
              "+step +proj=aea +lat_0=0 +lon_0=-120 +lat_1=34 "
              "+lat_2=40.5 +x_0=0 +y_0=-4000000 +ellps=GRS80");
}

// ---------------------------------------------------------------------------

TEST(operation, isPROJInstantiable) {

    {
        auto transformation = Transformation::createGeocentricTranslations(
            PropertyMap(), GeographicCRS::EPSG_4269, GeographicCRS::EPSG_4326,
            1.0, 2.0, 3.0, {});
        EXPECT_TRUE(transformation->isPROJInstantiable(
            DatabaseContext::create(), false));
    }

    // Missing grid
    {
        auto transformation = Transformation::createNTv2(
            PropertyMap(), GeographicCRS::EPSG_4807, GeographicCRS::EPSG_4326,
            "foo.gsb", std::vector<PositionalAccuracyNNPtr>());
        EXPECT_FALSE(transformation->isPROJInstantiable(
            DatabaseContext::create(), false));
    }

    // Unsupported method
    {
        auto transformation = Transformation::create(
            PropertyMap(), GeographicCRS::EPSG_4269, GeographicCRS::EPSG_4326,
            nullptr,
            OperationMethod::create(PropertyMap(),
                                    std::vector<OperationParameterNNPtr>{}),
            std::vector<GeneralParameterValueNNPtr>{},
            std::vector<PositionalAccuracyNNPtr>{});
        EXPECT_FALSE(transformation->isPROJInstantiable(
            DatabaseContext::create(), false));
    }
}

// ---------------------------------------------------------------------------

TEST(operation, createOperation_on_crs_with_canonical_bound_crs) {
    auto boundCRS = BoundCRS::createFromTOWGS84(
        GeographicCRS::EPSG_4267, std::vector<double>{1, 2, 3, 4, 5, 6, 7});
    auto crs = boundCRS->baseCRSWithCanonicalBoundCRS();
    {
        auto op = CoordinateOperationFactory::create()->createOperation(
            crs, GeographicCRS::EPSG_4326);
        ASSERT_TRUE(op != nullptr);
        EXPECT_TRUE(op->isEquivalentTo(boundCRS->transformation().get()));
        {
            auto wkt1 = op->exportToWKT(
                WKTFormatter::create(WKTFormatter::Convention::WKT2_2019)
                    .get());
            auto wkt2 = boundCRS->transformation()->exportToWKT(
                WKTFormatter::create(WKTFormatter::Convention::WKT2_2019)
                    .get());
            EXPECT_EQ(wkt1, wkt2);
        }
    }
    {
        auto op = CoordinateOperationFactory::create()->createOperation(
            GeographicCRS::EPSG_4326, crs);
        ASSERT_TRUE(op != nullptr);
        EXPECT_TRUE(
            op->isEquivalentTo(boundCRS->transformation()->inverse().get()));
        {
            auto wkt1 = op->exportToWKT(
                WKTFormatter::create(WKTFormatter::Convention::WKT2_2019)
                    .get());
            auto wkt2 = boundCRS->transformation()->inverse()->exportToWKT(
                WKTFormatter::create(WKTFormatter::Convention::WKT2_2019)
                    .get());
            EXPECT_EQ(wkt1, wkt2);
        }
    }
}

// ---------------------------------------------------------------------------

TEST(operation, createOperation_fallback_to_proj4_strings) {
    auto objDest = PROJStringParser().createFromPROJString(
        "+proj=longlat +over +datum=WGS84 +type=crs");
    auto dest = nn_dynamic_pointer_cast<GeographicCRS>(objDest);
    ASSERT_TRUE(dest != nullptr);

    auto op = CoordinateOperationFactory::create()->createOperation(
        GeographicCRS::EPSG_4326, NN_CHECK_ASSERT(dest));
    ASSERT_TRUE(op != nullptr);
    EXPECT_EQ(op->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=pipeline +step +proj=axisswap +order=2,1 "
              "+step +proj=unitconvert +xy_in=deg +xy_out=rad "
              "+step +proj=longlat +over +datum=WGS84 "
              "+step +proj=unitconvert +xy_in=rad +xy_out=deg");
}

// ---------------------------------------------------------------------------

TEST(operation, createOperation_fallback_to_proj4_strings_bound_of_geog) {
    auto objSrc = PROJStringParser().createFromPROJString(
        "+proj=longlat +over +ellps=GRS80 +towgs84=0,0,0 +type=crs");
    auto src = nn_dynamic_pointer_cast<BoundCRS>(objSrc);
    ASSERT_TRUE(src != nullptr);

    auto objDest = PROJStringParser().createFromPROJString(
        "+proj=longlat +over +ellps=clrk66 +towgs84=0,0,0 +type=crs");
    auto dest = nn_dynamic_pointer_cast<BoundCRS>(objDest);
    ASSERT_TRUE(dest != nullptr);

    auto op = CoordinateOperationFactory::create()->createOperation(
        NN_CHECK_ASSERT(src), NN_CHECK_ASSERT(dest));
    ASSERT_TRUE(op != nullptr);
    EXPECT_EQ(op->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=pipeline "
              "+step +proj=unitconvert +xy_in=deg +xy_out=rad "
              "+step +inv +proj=longlat +over +ellps=GRS80 +towgs84=0,0,0 "
              "+step +proj=longlat +over +ellps=clrk66 +towgs84=0,0,0 "
              "+step +proj=unitconvert +xy_in=rad +xy_out=deg");
}

// ---------------------------------------------------------------------------

TEST(
    operation,
    createOperation_fallback_to_proj4_strings_regular_with_datum_to_projliteral) {
    auto objSrc = PROJStringParser().createFromPROJString(
        "+proj=utm +zone=11 +datum=NAD27 +type=crs");
    auto src = nn_dynamic_pointer_cast<CRS>(objSrc);
    ASSERT_TRUE(src != nullptr);

    auto objDst = PROJStringParser().createFromPROJString(
        "+proj=longlat +datum=WGS84 +over +type=crs");
    auto dst = nn_dynamic_pointer_cast<CRS>(objDst);
    ASSERT_TRUE(dst != nullptr);

    auto op = CoordinateOperationFactory::create()->createOperation(
        NN_CHECK_ASSERT(src), NN_CHECK_ASSERT(dst));
    ASSERT_TRUE(op != nullptr);
    EXPECT_EQ(op->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=pipeline "
              "+step +inv +proj=utm +zone=11 +datum=NAD27 "
              "+step +proj=longlat +datum=WGS84 +over "
              "+step +proj=unitconvert +xy_in=rad +xy_out=deg");
}

// ---------------------------------------------------------------------------

TEST(operation,
     createOperation_fallback_to_proj4_strings_proj_NAD83_to_projliteral) {
    auto objSrc = PROJStringParser().createFromPROJString(
        "+proj=utm +zone=11 +datum=NAD83 +type=crs");
    auto src = nn_dynamic_pointer_cast<CRS>(objSrc);
    ASSERT_TRUE(src != nullptr);

    auto objDst = PROJStringParser().createFromPROJString(
        "+proj=longlat +datum=WGS84 +over +type=crs");
    auto dst = nn_dynamic_pointer_cast<CRS>(objDst);
    ASSERT_TRUE(dst != nullptr);

    auto op = CoordinateOperationFactory::create()->createOperation(
        NN_CHECK_ASSERT(src), NN_CHECK_ASSERT(dst));
    ASSERT_TRUE(op != nullptr);
    EXPECT_EQ(op->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=pipeline "
              "+step +inv +proj=utm +zone=11 +ellps=GRS80 "
              "+step +proj=longlat +datum=WGS84 +over "
              "+step +proj=unitconvert +xy_in=rad +xy_out=deg");
}

// ---------------------------------------------------------------------------

TEST(operation,
     createOperation_fallback_to_proj4_strings_geog_NAD83_to_projliteral) {
    auto objSrc = PROJStringParser().createFromPROJString(
        "+proj=longlat +datum=NAD83 +type=crs");
    auto src = nn_dynamic_pointer_cast<CRS>(objSrc);
    ASSERT_TRUE(src != nullptr);

    auto objDst = PROJStringParser().createFromPROJString(
        "+proj=longlat +datum=WGS84 +over +type=crs");
    auto dst = nn_dynamic_pointer_cast<CRS>(objDst);
    ASSERT_TRUE(dst != nullptr);

    auto op = CoordinateOperationFactory::create()->createOperation(
        NN_CHECK_ASSERT(src), NN_CHECK_ASSERT(dst));
    ASSERT_TRUE(op != nullptr);
    EXPECT_EQ(op->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=pipeline "
              "+step +proj=unitconvert +xy_in=deg +xy_out=rad "
              "+step +proj=longlat +datum=WGS84 +over "
              "+step +proj=unitconvert +xy_in=rad +xy_out=deg");
}

// ---------------------------------------------------------------------------

TEST(
    operation,
    createOperation_fallback_to_proj4_strings_regular_with_nadgrids_to_projliteral) {
    auto objSrc = PROJStringParser().createFromPROJString(
        "+proj=utm +zone=11 +ellps=clrk66 +nadgrids=@conus +type=crs");
    auto src = nn_dynamic_pointer_cast<CRS>(objSrc);
    ASSERT_TRUE(src != nullptr);

    auto objDst = PROJStringParser().createFromPROJString(
        "+proj=longlat +datum=WGS84 +over +type=crs");
    auto dst = nn_dynamic_pointer_cast<CRS>(objDst);
    ASSERT_TRUE(dst != nullptr);

    auto op = CoordinateOperationFactory::create()->createOperation(
        NN_CHECK_ASSERT(src), NN_CHECK_ASSERT(dst));
    ASSERT_TRUE(op != nullptr);
    EXPECT_EQ(op->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=pipeline "
              "+step +inv +proj=utm +zone=11 +ellps=clrk66 +nadgrids=@conus "
              "+step +proj=longlat +datum=WGS84 +over "
              "+step +proj=unitconvert +xy_in=rad +xy_out=deg");
}

// ---------------------------------------------------------------------------

TEST(operation,
     createOperation_fallback_to_proj4_strings_projliteral_to_projliteral) {
    auto objSrc = PROJStringParser().createFromPROJString(
        "+proj=utm +zone=11 +datum=NAD27 +over +type=crs");
    auto src = nn_dynamic_pointer_cast<CRS>(objSrc);
    ASSERT_TRUE(src != nullptr);

    auto objDst = PROJStringParser().createFromPROJString(
        "+proj=longlat +datum=WGS84 +over +type=crs");
    auto dst = nn_dynamic_pointer_cast<CRS>(objDst);
    ASSERT_TRUE(dst != nullptr);

    auto op = CoordinateOperationFactory::create()->createOperation(
        NN_CHECK_ASSERT(src), NN_CHECK_ASSERT(dst));
    ASSERT_TRUE(op != nullptr);
    EXPECT_EQ(op->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=pipeline "
              "+step +inv +proj=utm +zone=11 +datum=NAD27 +over "
              "+step +proj=longlat +datum=WGS84 +over "
              "+step +proj=unitconvert +xy_in=rad +xy_out=deg");
}

// ---------------------------------------------------------------------------

TEST(
    operation,
    createOperation_fallback_to_proj4_strings_regular_to_projliteral_with_towgs84) {
    auto objSrc =
        createFromUserInput("EPSG:4326", DatabaseContext::create(), false);
    auto src = nn_dynamic_pointer_cast<CRS>(objSrc);
    ASSERT_TRUE(src != nullptr);

    auto objDst = PROJStringParser().createFromPROJString(
        "+proj=utm +zone=31 +ellps=GRS80 +towgs84=1,2,3 +over +type=crs");
    auto dst = nn_dynamic_pointer_cast<CRS>(objDst);
    ASSERT_TRUE(dst != nullptr);

    auto op = CoordinateOperationFactory::create()->createOperation(
        NN_CHECK_ASSERT(src), NN_CHECK_ASSERT(dst));
    ASSERT_TRUE(op != nullptr);
    EXPECT_EQ(op->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=pipeline +step +proj=axisswap +order=2,1 "
              "+step +proj=unitconvert +xy_in=deg +xy_out=rad "
              "+step +proj=utm +zone=31 +ellps=GRS80 +towgs84=1,2,3 +over");
}

// ---------------------------------------------------------------------------

TEST(operation, createOperation_on_crs_with_bound_crs_and_wktext) {
    auto objSrc = PROJStringParser().createFromPROJString(
        "+proj=utm +zone=55 +south +ellps=GRS80 +towgs84=0,0,0,0,0,0,0 "
        "+units=m +no_defs +nadgrids=@GDA94_GDA2020_conformal.gsb +ignored1 "
        "+ignored2=val +wktext +type=crs");
    auto src = nn_dynamic_pointer_cast<CRS>(objSrc);
    ASSERT_TRUE(src != nullptr);

    auto objDst = PROJStringParser().createFromPROJString(
        "+proj=utm +zone=55 +south +ellps=GRS80 +towgs84=0,0,0,0,0,0,0 "
        "+units=m +no_defs +type=crs");
    auto dst = nn_dynamic_pointer_cast<CRS>(objDst);
    ASSERT_TRUE(dst != nullptr);

    auto op = CoordinateOperationFactory::create()->createOperation(
        NN_CHECK_ASSERT(src), NN_CHECK_ASSERT(dst));
    ASSERT_TRUE(op != nullptr);
    EXPECT_EQ(op->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=pipeline +step +inv +proj=utm +zone=55 +south "
              "+ellps=GRS80 +step +proj=hgridshift "
              "+grids=@GDA94_GDA2020_conformal.gsb +step +proj=utm +zone=55 "
              "+south +ellps=GRS80");
}

// ---------------------------------------------------------------------------

TEST(operation,
     createOperation_fallback_to_proj4_strings_with_axis_inverted_projCRS) {
    auto objSrc =
        createFromUserInput("EPSG:2193", DatabaseContext::create(), false);
    auto src = nn_dynamic_pointer_cast<CRS>(objSrc);
    ASSERT_TRUE(src != nullptr);

    auto objDest = PROJStringParser().createFromPROJString(
        "+proj=longlat +ellps=WGS84 +lon_wrap=180 +type=crs");
    auto dest = nn_dynamic_pointer_cast<GeographicCRS>(objDest);
    ASSERT_TRUE(dest != nullptr);

    auto op = CoordinateOperationFactory::create()->createOperation(
        NN_CHECK_ASSERT(src), NN_CHECK_ASSERT(dest));
    ASSERT_TRUE(op != nullptr);
    EXPECT_EQ(op->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=pipeline "
              "+step +proj=axisswap +order=2,1 "
              "+step +inv +proj=tmerc +lat_0=0 +lon_0=173 +k=0.9996 "
              "+x_0=1600000 +y_0=10000000 +ellps=GRS80 "
              "+step +proj=longlat +ellps=WGS84 +lon_wrap=180 "
              "+step +proj=unitconvert +xy_in=rad +xy_out=deg");
}

// ---------------------------------------------------------------------------

TEST(operation, compoundCRS_to_proj_string_with_non_metre_height) {
    auto objSrc =
        createFromUserInput("EPSG:6318+5703", DatabaseContext::create(), false);
    auto src = nn_dynamic_pointer_cast<CRS>(objSrc);
    ASSERT_TRUE(src != nullptr);

    auto objDst = PROJStringParser().createFromPROJString(
        "+proj=longlat +ellps=GRS80 +vunits=us-ft +type=crs");
    auto dst = nn_dynamic_pointer_cast<CRS>(objDst);
    ASSERT_TRUE(dst != nullptr);

    auto authFactory =
        AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    auto ctxt = CoordinateOperationContext::create(authFactory, nullptr, 0.0);
    ctxt->setSpatialCriterion(
        CoordinateOperationContext::SpatialCriterion::PARTIAL_INTERSECTION);
    auto list = CoordinateOperationFactory::create()->createOperations(
        NN_NO_CHECK(src), NN_NO_CHECK(dst), ctxt);
    ASSERT_GT(list.size(), 1U);
    // What is important to check here is the vertical unit conversion
    EXPECT_EQ(
        list[0]->exportToPROJString(PROJStringFormatter::create().get()),
        "+proj=pipeline "
        "+step +proj=axisswap +order=2,1 "
        "+step +proj=unitconvert +xy_in=deg +xy_out=rad "
        "+step +proj=vgridshift +grids=us_noaa_g2018u0.tif +multiplier=1 "
        "+step +proj=unitconvert +xy_in=rad +z_in=m +xy_out=deg +z_out=us-ft");
}

// ---------------------------------------------------------------------------

TEST(operation, compoundCRS_with_derived_vertical_CRS) {

    auto dbContext = DatabaseContext::create();
    auto authFactory = AuthorityFactory::create(dbContext, "EPSG");

    // ETRS89 + EGM2008 height
    auto objSrc = createFromUserInput("EPSG:4258+3855", dbContext, false);
    auto src = nn_dynamic_pointer_cast<CRS>(objSrc);
    ASSERT_TRUE(src != nullptr);

    auto wkt =
        "COMPOUNDCRS[\"WGS 84 + Custom Vertical\",\n"
        "    GEOGCRS[\"WGS 84\",\n"
        "        ENSEMBLE[\"World Geodetic System 1984 ensemble\",\n"
        "            MEMBER[\"World Geodetic System 1984 (Transit)\"],\n"
        "            MEMBER[\"World Geodetic System 1984 (G730)\"],\n"
        "            MEMBER[\"World Geodetic System 1984 (G873)\"],\n"
        "            MEMBER[\"World Geodetic System 1984 (G1150)\"],\n"
        "            MEMBER[\"World Geodetic System 1984 (G1674)\"],\n"
        "            MEMBER[\"World Geodetic System 1984 (G1762)\"],\n"
        "            MEMBER[\"World Geodetic System 1984 (G2139)\"],\n"
        "            ELLIPSOID[\"WGS 84\",6378137,298.257223563,\n"
        "                LENGTHUNIT[\"metre\",1]],\n"
        "            ENSEMBLEACCURACY[2.0]],\n"
        "        PRIMEM[\"Greenwich\",0,\n"
        "            ANGLEUNIT[\"degree\",0.0174532925199433]],\n"
        "        CS[ellipsoidal,2],\n"
        "            AXIS[\"geodetic latitude (Lat)\",north,\n"
        "                ORDER[1],\n"
        "                ANGLEUNIT[\"degree\",0.0174532925199433]],\n"
        "            AXIS[\"geodetic longitude (Lon)\",east,\n"
        "                ORDER[2],\n"
        "                ANGLEUNIT[\"degree\",0.0174532925199433]]],\n"
        "    VERTCRS[\"Custom Vertical\",\n"
        "        BASEVERTCRS[\"EGM2008 height\",\n"
        "            VDATUM[\"EGM2008 geoid\"]],\n"
        "        DERIVINGCONVERSION[\"vertical offs. and slope\",\n"
        "            METHOD[\"Vertical Offset and Slope\",\n"
        "                ID[\"EPSG\",1046]],\n"
        "            PARAMETER[\"Ordinate 1 of evaluation point\",47,\n"
        "                ANGLEUNIT[\"degree\",0.0174532925199433],\n"
        "                ID[\"EPSG\",8617]],\n"
        "            PARAMETER[\"Ordinate 2 of evaluation point\",8,\n"
        "                ANGLEUNIT[\"degree\",0.0174532925199433],\n"
        "                ID[\"EPSG\",8618]],\n"
        "            PARAMETER[\"Vertical Offset\",-0.245,\n"
        "                LENGTHUNIT[\"metre\",1],\n"
        "                ID[\"EPSG\",8603]],\n"
        "            PARAMETER[\"Inclination in latitude\",-0.21,\n"
        "                ANGLEUNIT[\"arc-second\",4.84813681109536E-06],\n"
        "                ID[\"EPSG\",8730]],\n"
        "            PARAMETER[\"Inclination in longitude\",-0.032,\n"
        "                ANGLEUNIT[\"arc-second\",4.84813681109536E-06],\n"
        "                ID[\"EPSG\",8731]],\n"
        "            PARAMETER[\"EPSG code for Horizontal CRS\",4326,\n"
        "                ID[\"EPSG\",1037]]],\n"
        "        CS[vertical,1],\n"
        "            AXIS[\"gravity-related height (H)\",up,\n"
        "                LENGTHUNIT[\"metre\",1]],\n"
        "        USAGE[\n"
        "            SCOPE[\"unknown\"],\n"
        "            AREA[\"World\"],\n"
        "            BBOX[-90,-180,90,180]]]]";
    auto objDst = createFromUserInput(wkt, dbContext, false);
    auto dst = nn_dynamic_pointer_cast<CRS>(objDst);
    ASSERT_TRUE(dst != nullptr);

    auto ctxt = CoordinateOperationContext::create(authFactory, nullptr, 0.0);
    ctxt->setSpatialCriterion(
        CoordinateOperationContext::SpatialCriterion::PARTIAL_INTERSECTION);
    auto list = CoordinateOperationFactory::create()->createOperations(
        NN_NO_CHECK(src), NN_NO_CHECK(dst), ctxt);
    ASSERT_EQ(list.size(), 1U);
    EXPECT_EQ(list[0]->nameStr(),
              "ETRS89 to WGS 84 (1) + vertical offs. and slope");
    EXPECT_EQ(
        list[0]->exportToPROJString(PROJStringFormatter::create().get()),
        "+proj=pipeline "
        "+step +proj=axisswap +order=2,1 "
        "+step +proj=unitconvert +xy_in=deg +xy_out=rad "
        "+step +proj=vertoffset +lat_0=47 +lon_0=8 +dh=-0.245 +slope_lat=-0.21 "
        "+slope_lon=-0.032 "
        "+step +proj=unitconvert +xy_in=rad +xy_out=deg "
        "+step +proj=axisswap +order=2,1");
}

// ---------------------------------------------------------------------------

TEST(operation, compoundCRS_to_PROJJSON_with_non_metre_height) {
    auto srcPROJJSON =
        "{\n"
        "  \"$schema\": "
        "\"https://proj.org/schemas/v0.2/projjson.schema.json\",\n"
        "  \"type\": \"CompoundCRS\",\n"
        "  \"name\": \"Compound CRS NAD83(2011) / Nebraska (ftUS) + North "
        "American Vertical Datum 1988 + PROJ us_noaa_g2012bu0.tif\",\n"
        "  \"components\": [\n"
        "    {\n"
        "      \"type\": \"ProjectedCRS\",\n"
        "      \"name\": \"NAD83(2011) / Nebraska (ftUS)\",\n"
        "      \"base_crs\": {\n"
        "        \"name\": \"NAD83(2011)\",\n"
        "        \"datum\": {\n"
        "          \"type\": \"GeodeticReferenceFrame\",\n"
        "          \"name\": \"NAD83 (National Spatial Reference System "
        "2011)\",\n"
        "          \"ellipsoid\": {\n"
        "            \"name\": \"GRS 1980\",\n"
        "            \"semi_major_axis\": 6378137,\n"
        "            \"inverse_flattening\": 298.257222101\n"
        "          }\n"
        "        },\n"
        "        \"coordinate_system\": {\n"
        "          \"subtype\": \"ellipsoidal\",\n"
        "          \"axis\": [\n"
        "            {\n"
        "              \"name\": \"Geodetic latitude\",\n"
        "              \"abbreviation\": \"Lat\",\n"
        "              \"direction\": \"north\",\n"
        "              \"unit\": \"degree\"\n"
        "            },\n"
        "            {\n"
        "              \"name\": \"Geodetic longitude\",\n"
        "              \"abbreviation\": \"Lon\",\n"
        "              \"direction\": \"east\",\n"
        "              \"unit\": \"degree\"\n"
        "            }\n"
        "          ]\n"
        "        },\n"
        "        \"id\": {\n"
        "          \"authority\": \"EPSG\",\n"
        "          \"code\": 6318\n"
        "        }\n"
        "      },\n"
        "      \"conversion\": {\n"
        "        \"name\": \"SPCS83 Nebraska zone (US Survey feet)\",\n"
        "        \"method\": {\n"
        "          \"name\": \"Lambert Conic Conformal (2SP)\",\n"
        "          \"id\": {\n"
        "            \"authority\": \"EPSG\",\n"
        "            \"code\": 9802\n"
        "          }\n"
        "        },\n"
        "        \"parameters\": [\n"
        "          {\n"
        "            \"name\": \"Latitude of false origin\",\n"
        "            \"value\": 39.8333333333333,\n"
        "            \"unit\": \"degree\",\n"
        "            \"id\": {\n"
        "              \"authority\": \"EPSG\",\n"
        "              \"code\": 8821\n"
        "            }\n"
        "          },\n"
        "          {\n"
        "            \"name\": \"Longitude of false origin\",\n"
        "            \"value\": -100,\n"
        "            \"unit\": \"degree\",\n"
        "            \"id\": {\n"
        "              \"authority\": \"EPSG\",\n"
        "              \"code\": 8822\n"
        "            }\n"
        "          },\n"
        "          {\n"
        "            \"name\": \"Latitude of 1st standard parallel\",\n"
        "            \"value\": 43,\n"
        "            \"unit\": \"degree\",\n"
        "            \"id\": {\n"
        "              \"authority\": \"EPSG\",\n"
        "              \"code\": 8823\n"
        "            }\n"
        "          },\n"
        "          {\n"
        "            \"name\": \"Latitude of 2nd standard parallel\",\n"
        "            \"value\": 40,\n"
        "            \"unit\": \"degree\",\n"
        "            \"id\": {\n"
        "              \"authority\": \"EPSG\",\n"
        "              \"code\": 8824\n"
        "            }\n"
        "          },\n"
        "          {\n"
        "            \"name\": \"Easting at false origin\",\n"
        "            \"value\": 1640416.6667,\n"
        "            \"unit\": {\n"
        "              \"type\": \"LinearUnit\",\n"
        "              \"name\": \"US survey foot\",\n"
        "              \"conversion_factor\": 0.304800609601219\n"
        "            },\n"
        "            \"id\": {\n"
        "              \"authority\": \"EPSG\",\n"
        "              \"code\": 8826\n"
        "            }\n"
        "          },\n"
        "          {\n"
        "            \"name\": \"Northing at false origin\",\n"
        "            \"value\": 0,\n"
        "            \"unit\": {\n"
        "              \"type\": \"LinearUnit\",\n"
        "              \"name\": \"US survey foot\",\n"
        "              \"conversion_factor\": 0.304800609601219\n"
        "            },\n"
        "            \"id\": {\n"
        "              \"authority\": \"EPSG\",\n"
        "              \"code\": 8827\n"
        "            }\n"
        "          }\n"
        "        ],\n"
        "        \"id\": {\n"
        "          \"authority\": \"EPSG\",\n"
        "          \"code\": 15396\n"
        "        }\n"
        "      },\n"
        "      \"coordinate_system\": {\n"
        "        \"subtype\": \"Cartesian\",\n"
        "        \"axis\": [\n"
        "          {\n"
        "            \"name\": \"Easting\",\n"
        "            \"abbreviation\": \"X\",\n"
        "            \"direction\": \"east\",\n"
        "            \"unit\": {\n"
        "              \"type\": \"LinearUnit\",\n"
        "              \"name\": \"US survey foot\",\n"
        "              \"conversion_factor\": 0.304800609601219\n"
        "            }\n"
        "          },\n"
        "          {\n"
        "            \"name\": \"Northing\",\n"
        "            \"abbreviation\": \"Y\",\n"
        "            \"direction\": \"north\",\n"
        "            \"unit\": {\n"
        "              \"type\": \"LinearUnit\",\n"
        "              \"name\": \"US survey foot\",\n"
        "              \"conversion_factor\": 0.304800609601219\n"
        "            }\n"
        "          }\n"
        "        ]\n"
        "      }\n"
        "    },\n"
        "    {\n"
        "      \"type\": \"VerticalCRS\",\n"
        "      \"name\": \"North American Vertical Datum 1988 + PROJ "
        "us_noaa_g2012bu0.tif\",\n"
        "      \"datum\": {\n"
        "        \"type\": \"VerticalReferenceFrame\",\n"
        "        \"name\": \"North American Vertical Datum 1988\",\n"
        "        \"id\": {\n"
        "          \"authority\": \"EPSG\",\n"
        "          \"code\": 5103\n"
        "        }\n"
        "      },\n"
        "      \"coordinate_system\": {\n"
        "        \"subtype\": \"vertical\",\n"
        "        \"axis\": [\n"
        "          {\n"
        "            \"name\": \"Gravity-related height\",\n"
        "            \"abbreviation\": \"H\",\n"
        "            \"direction\": \"up\",\n"
        "            \"unit\": {\n"
        "              \"type\": \"LinearUnit\",\n"
        "              \"name\": \"US survey foot\",\n"
        "              \"conversion_factor\": 0.304800609601219\n"
        "            }\n"
        "          }\n"
        "        ]\n"
        "      },\n"
        "      \"geoid_model\": {\n"
        "        \"name\": \"PROJ us_noaa_g2012bu0.tif\",\n"
        "        \"interpolation_crs\": {\n"
        "          \"type\": \"GeographicCRS\",\n"
        "          \"name\": \"NAD83(2011)\",\n"
        "          \"datum\": {\n"
        "            \"type\": \"GeodeticReferenceFrame\",\n"
        "            \"name\": \"NAD83 (National Spatial Reference System "
        "2011)\",\n"
        "            \"ellipsoid\": {\n"
        "              \"name\": \"GRS 1980\",\n"
        "              \"semi_major_axis\": 6378137,\n"
        "              \"inverse_flattening\": 298.257222101\n"
        "            }\n"
        "          },\n"
        "          \"coordinate_system\": {\n"
        "            \"subtype\": \"ellipsoidal\",\n"
        "            \"axis\": [\n"
        "              {\n"
        "                \"name\": \"Geodetic latitude\",\n"
        "                \"abbreviation\": \"Lat\",\n"
        "                \"direction\": \"north\",\n"
        "                \"unit\": \"degree\"\n"
        "              },\n"
        "              {\n"
        "                \"name\": \"Geodetic longitude\",\n"
        "                \"abbreviation\": \"Lon\",\n"
        "                \"direction\": \"east\",\n"
        "                \"unit\": \"degree\"\n"
        "              },\n"
        "              {\n"
        "                \"name\": \"Ellipsoidal height\",\n"
        "                \"abbreviation\": \"h\",\n"
        "                \"direction\": \"up\",\n"
        "                \"unit\": \"metre\"\n"
        "              }\n"
        "            ]\n"
        "          },\n"
        "          \"id\": {\n"
        "            \"authority\": \"EPSG\",\n"
        "            \"code\": 6319\n"
        "          }\n"
        "        }\n"
        "      }\n"
        "    }\n"
        "  ]\n"
        "}";
    auto objSrc =
        createFromUserInput(srcPROJJSON, DatabaseContext::create(), false);
    auto src = nn_dynamic_pointer_cast<CRS>(objSrc);
    ASSERT_TRUE(src != nullptr);

    // The untypical potentially a bit buggy thing (and what caused a bug)
    // is the US-ft unit for the vertical axis of the base CRS ...
    // When outputting that to WKT, and
    // re-exporting to PROJJSON, one gets metre, which conforms more to the
    // official definition of NAD83(2011) 3D.
    // The vertical unit of the base CRS shouldn't matter much anyway, so this
    // is valid.
    auto dstPROJJSON =
        "{\n"
        "  \"$schema\": "
        "\"https://proj.org/schemas/v0.2/projjson.schema.json\",\n"
        "  \"type\": \"ProjectedCRS\",\n"
        "  \"name\": \"Projected CRS NAD83(2011) / UTM zone 14N with "
        "ellipsoidal NAD83(2011) height\",\n"
        "  \"base_crs\": {\n"
        "    \"name\": \"NAD83(2011)\",\n"
        "    \"datum\": {\n"
        "      \"type\": \"GeodeticReferenceFrame\",\n"
        "      \"name\": \"NAD83 (National Spatial Reference System 2011)\",\n"
        "      \"ellipsoid\": {\n"
        "        \"name\": \"GRS 1980\",\n"
        "        \"semi_major_axis\": 6378137,\n"
        "        \"inverse_flattening\": 298.257222101\n"
        "      },\n"
        "      \"id\": {\n"
        "        \"authority\": \"EPSG\",\n"
        "        \"code\": 1116\n"
        "      }\n"
        "    },\n"
        "    \"coordinate_system\": {\n"
        "      \"subtype\": \"ellipsoidal\",\n"
        "      \"axis\": [\n"
        "        {\n"
        "          \"name\": \"Geodetic latitude\",\n"
        "          \"abbreviation\": \"Lat\",\n"
        "          \"direction\": \"north\",\n"
        "          \"unit\": \"degree\"\n"
        "        },\n"
        "        {\n"
        "          \"name\": \"Geodetic longitude\",\n"
        "          \"abbreviation\": \"Lon\",\n"
        "          \"direction\": \"east\",\n"
        "          \"unit\": \"degree\"\n"
        "        },\n"
        "        {\n"
        "          \"name\": \"Ellipsoidal height\",\n"
        "          \"abbreviation\": \"h\",\n"
        "          \"direction\": \"up\",\n"
        "          \"unit\": {\n"
        "            \"type\": \"LinearUnit\",\n"
        "            \"name\": \"US survey foot\",\n"
        "            \"conversion_factor\": 0.304800609601219\n"
        "          }\n"
        "        }\n"
        "      ]\n"
        "    }\n"
        "  },\n"
        "  \"conversion\": {\n"
        "    \"name\": \"UTM zone 14N\",\n"
        "    \"method\": {\n"
        "      \"name\": \"Transverse Mercator\",\n"
        "      \"id\": {\n"
        "        \"authority\": \"EPSG\",\n"
        "        \"code\": 9807\n"
        "      }\n"
        "    },\n"
        "    \"parameters\": [\n"
        "      {\n"
        "        \"name\": \"Latitude of natural origin\",\n"
        "        \"value\": 0,\n"
        "        \"unit\": \"degree\",\n"
        "        \"id\": {\n"
        "          \"authority\": \"EPSG\",\n"
        "          \"code\": 8801\n"
        "        }\n"
        "      },\n"
        "      {\n"
        "        \"name\": \"Longitude of natural origin\",\n"
        "        \"value\": -99,\n"
        "        \"unit\": \"degree\",\n"
        "        \"id\": {\n"
        "          \"authority\": \"EPSG\",\n"
        "          \"code\": 8802\n"
        "        }\n"
        "      },\n"
        "      {\n"
        "        \"name\": \"Scale factor at natural origin\",\n"
        "        \"value\": 0.9996,\n"
        "        \"unit\": \"unity\",\n"
        "        \"id\": {\n"
        "          \"authority\": \"EPSG\",\n"
        "          \"code\": 8805\n"
        "        }\n"
        "      },\n"
        "      {\n"
        "        \"name\": \"False easting\",\n"
        "        \"value\": 500000,\n"
        "        \"unit\": \"metre\",\n"
        "        \"id\": {\n"
        "          \"authority\": \"EPSG\",\n"
        "          \"code\": 8806\n"
        "        }\n"
        "      },\n"
        "      {\n"
        "        \"name\": \"False northing\",\n"
        "        \"value\": 0,\n"
        "        \"unit\": \"metre\",\n"
        "        \"id\": {\n"
        "          \"authority\": \"EPSG\",\n"
        "          \"code\": 8807\n"
        "        }\n"
        "      }\n"
        "    ],\n"
        "    \"id\": {\n"
        "      \"authority\": \"EPSG\",\n"
        "      \"code\": 16014\n"
        "    }\n"
        "  },\n"
        "  \"coordinate_system\": {\n"
        "    \"subtype\": \"Cartesian\",\n"
        "    \"axis\": [\n"
        "      {\n"
        "        \"name\": \"Easting\",\n"
        "        \"abbreviation\": \"E\",\n"
        "        \"direction\": \"east\",\n"
        "        \"unit\": {\n"
        "          \"type\": \"LinearUnit\",\n"
        "          \"name\": \"US survey foot\",\n"
        "          \"conversion_factor\": 0.304800609601219\n"
        "        }\n"
        "      },\n"
        "      {\n"
        "        \"name\": \"Northing\",\n"
        "        \"abbreviation\": \"N\",\n"
        "        \"direction\": \"north\",\n"
        "        \"unit\": {\n"
        "          \"type\": \"LinearUnit\",\n"
        "          \"name\": \"US survey foot\",\n"
        "          \"conversion_factor\": 0.304800609601219\n"
        "        }\n"
        "      },\n"
        "      {\n"
        "        \"name\": \"Ellipsoidal height\",\n"
        "        \"abbreviation\": \"h\",\n"
        "        \"direction\": \"up\",\n"
        "        \"unit\": {\n"
        "          \"type\": \"LinearUnit\",\n"
        "          \"name\": \"US survey foot\",\n"
        "          \"conversion_factor\": 0.304800609601219\n"
        "        }\n"
        "      }\n"
        "    ]\n"
        "  }\n"
        "}";

    auto objDst =
        createFromUserInput(dstPROJJSON, DatabaseContext::create(), false);
    auto dst = nn_dynamic_pointer_cast<CRS>(objDst);
    ASSERT_TRUE(dst != nullptr);

    auto authFactory =
        AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    auto ctxt = CoordinateOperationContext::create(authFactory, nullptr, 0.0);
    ctxt->setSpatialCriterion(
        CoordinateOperationContext::SpatialCriterion::PARTIAL_INTERSECTION);
    auto list = CoordinateOperationFactory::create()->createOperations(
        NN_NO_CHECK(src), NN_NO_CHECK(dst), ctxt);
    ASSERT_GT(list.size(), 1U);
    // What is important to check here is the vertical unit conversion
    EXPECT_EQ(
        list[0]->exportToPROJString(PROJStringFormatter::create().get()),
        "+proj=pipeline "
        "+step +proj=unitconvert +xy_in=us-ft +xy_out=m "
        "+step +inv +proj=lcc +lat_0=39.8333333333333 +lon_0=-100 +lat_1=43 "
        "+lat_2=40 +x_0=500000.00001016 +y_0=0 +ellps=GRS80 "
        "+step +proj=unitconvert +z_in=us-ft +z_out=m "
        "+step +proj=vgridshift +grids=us_noaa_g2012bu0.tif +multiplier=1 "
        "+step +proj=utm +zone=14 +ellps=GRS80 "
        "+step +proj=unitconvert +xy_in=m +z_in=m +xy_out=us-ft +z_out=us-ft");
}

// ---------------------------------------------------------------------------

TEST(operation, createOperation_ossfuzz_18587) {
    auto objSrc =
        createFromUserInput("EPSG:4326", DatabaseContext::create(), false);
    auto src = nn_dynamic_pointer_cast<CRS>(objSrc);
    ASSERT_TRUE(src != nullptr);

    // Extremely weird string ! We should likely reject it
    auto objDst = PROJStringParser().createFromPROJString(
        "type=crs proj=pipeline step proj=merc vunits=m nadgrids=@x "
        "proj=\"\nproj=pipeline step\n\"");
    auto dst = nn_dynamic_pointer_cast<CRS>(objDst);
    ASSERT_TRUE(dst != nullptr);

    // Just check that we don't go into an infinite recursion
    try {
        CoordinateOperationFactory::create()->createOperation(
            NN_CHECK_ASSERT(src), NN_CHECK_ASSERT(dst));
    } catch (const std::exception &) {
    }
}

// ---------------------------------------------------------------------------

class derivedGeographicCRS_with_to_wgs84_to_geographicCRS
    : public ::testing::Test {
  protected:
    void run(const CRSNNPtr &src) {
        auto objDst = PROJStringParser().createFromPROJString(
            "+proj=longlat +datum=WGS84 +type=crs");
        auto dst = nn_dynamic_pointer_cast<CRS>(objDst);
        ASSERT_TRUE(dst != nullptr);

        {
            auto op = CoordinateOperationFactory::create()->createOperation(
                src, NN_CHECK_ASSERT(dst));
            ASSERT_TRUE(op != nullptr);
            std::string pipeline(
                "+proj=pipeline "
                "+step +proj=unitconvert +xy_in=deg +xy_out=rad "
                "+step +inv +proj=ob_tran +o_proj=latlon +over +lat_0=0 "
                "+lon_0=180 "
                "+o_lat_p=18 +o_lon_p=-200 +ellps=WGS84 "
                "+step +proj=push +v_3 "
                "+step +proj=cart +ellps=WGS84 "
                "+step +proj=helmert +x=1 +y=2 +z=3 "
                "+step +inv +proj=cart +ellps=WGS84 "
                "+step +proj=pop +v_3 "
                "+step +proj=unitconvert +xy_in=rad +xy_out=deg");
            EXPECT_EQ(
                op->exportToPROJString(PROJStringFormatter::create().get()),
                pipeline);

            auto op2 = CoordinateOperationFactory::create()->createOperation(
                src, nn_static_pointer_cast<CRS>(GeographicCRS::EPSG_4326));
            ASSERT_TRUE(op2 != nullptr);
            EXPECT_EQ(
                op2->exportToPROJString(PROJStringFormatter::create().get()),
                pipeline + " +step +proj=axisswap +order=2,1");
        }

        {
            auto op = CoordinateOperationFactory::create()->createOperation(
                NN_CHECK_ASSERT(dst), src);
            ASSERT_TRUE(op != nullptr);
            std::string pipeline(
                "+step +proj=unitconvert +xy_in=deg +xy_out=rad "
                "+step +proj=push +v_3 "
                "+step +proj=cart +ellps=WGS84 "
                "+step +proj=helmert +x=-1 +y=-2 +z=-3 "
                "+step +inv +proj=cart +ellps=WGS84 "
                "+step +proj=pop +v_3 "
                "+step +proj=ob_tran +o_proj=latlon +over +lat_0=0 +lon_0=180 "
                "+o_lat_p=18 +o_lon_p=-200 +ellps=WGS84 "
                "+step +proj=unitconvert +xy_in=rad +xy_out=deg");
            EXPECT_EQ(
                op->exportToPROJString(PROJStringFormatter::create().get()),
                "+proj=pipeline " + pipeline);

            auto op2 = CoordinateOperationFactory::create()->createOperation(
                nn_static_pointer_cast<CRS>(GeographicCRS::EPSG_4326), src);
            ASSERT_TRUE(op2 != nullptr);
            EXPECT_EQ(
                op2->exportToPROJString(PROJStringFormatter::create().get()),
                "+proj=pipeline +step +proj=axisswap +order=2,1 " + pipeline);
        }
    }
};

// ---------------------------------------------------------------------------

TEST_F(derivedGeographicCRS_with_to_wgs84_to_geographicCRS, src_from_proj) {
    auto objSrc = PROJStringParser().createFromPROJString(
        "+proj=ob_tran +o_proj=latlon +lat_0=0 +lon_0=180 +o_lat_p=18.0 "
        "+o_lon_p=-200.0 +ellps=WGS84 +towgs84=1,2,3 +over +type=crs");
    auto src = nn_dynamic_pointer_cast<CRS>(objSrc);
    ASSERT_TRUE(src != nullptr);
    run(NN_CHECK_ASSERT(src));
}

// ---------------------------------------------------------------------------

TEST_F(derivedGeographicCRS_with_to_wgs84_to_geographicCRS, src_from_wkt2) {
    // Same as above, but testing with a WKT CRS source
    // The subtle difference is that the base CRS of the DerivedGeographicCRS
    // will have a lat, long axis order
    auto objSrcProj = PROJStringParser().createFromPROJString(
        "+proj=ob_tran +o_proj=latlon +lat_0=0 +lon_0=180 +o_lat_p=18.0 "
        "+o_lon_p=-200.0 +ellps=WGS84 +towgs84=1,2,3 +over +type=crs");
    auto srcFromProj = nn_dynamic_pointer_cast<CRS>(objSrcProj);
    ASSERT_TRUE(srcFromProj != nullptr);
    auto srcWkt = srcFromProj->exportToWKT(WKTFormatter::create().get());
    auto objSrc = createFromUserInput(srcWkt, DatabaseContext::create(), false);
    auto src = nn_dynamic_pointer_cast<CRS>(objSrc);
    ASSERT_TRUE(src != nullptr);
    run(NN_CHECK_ASSERT(src));
}

// ---------------------------------------------------------------------------

TEST(operation, createOperation_spherical_ocentric_to_geographic) {
    auto objSrc = PROJStringParser().createFromPROJString(
        "+proj=longlat +geoc +datum=WGS84 +type=crs");
    auto src = nn_dynamic_pointer_cast<GeodeticCRS>(objSrc);
    ASSERT_TRUE(src != nullptr);

    auto op = CoordinateOperationFactory::create()->createOperation(
        NN_CHECK_ASSERT(src), GeographicCRS::EPSG_4326);
    ASSERT_TRUE(op != nullptr);
    EXPECT_EQ(op->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=pipeline "
              "+step +proj=unitconvert +xy_in=deg +xy_out=rad "
              "+step +inv +proj=geoc +ellps=WGS84 "
              "+step +proj=unitconvert +xy_in=rad +xy_out=deg "
              "+step +proj=axisswap +order=2,1");
}

// ---------------------------------------------------------------------------

TEST(operation, createOperation_geographic_to_spherical_ocentric) {
    auto objDest = PROJStringParser().createFromPROJString(
        "+proj=longlat +geoc +datum=WGS84 +type=crs");
    auto dest = nn_dynamic_pointer_cast<GeodeticCRS>(objDest);
    ASSERT_TRUE(dest != nullptr);

    auto op = CoordinateOperationFactory::create()->createOperation(
        GeographicCRS::EPSG_4326, NN_CHECK_ASSERT(dest));
    ASSERT_TRUE(op != nullptr);
    EXPECT_EQ(op->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=pipeline "
              "+step +proj=axisswap +order=2,1 "
              "+step +proj=unitconvert +xy_in=deg +xy_out=rad "
              "+step +proj=geoc +ellps=WGS84 "
              "+step +proj=unitconvert +xy_in=rad +xy_out=deg");
}

// ---------------------------------------------------------------------------

TEST(operation, createOperation_spherical_ocentric_to_geocentric) {
    auto objSrc = PROJStringParser().createFromPROJString(
        "+proj=longlat +geoc +datum=WGS84 +type=crs");
    auto src = nn_dynamic_pointer_cast<GeodeticCRS>(objSrc);
    ASSERT_TRUE(src != nullptr);

    auto objDest = PROJStringParser().createFromPROJString(
        "+proj=geocent +datum=WGS84 +type=crs");
    auto dest = nn_dynamic_pointer_cast<GeodeticCRS>(objDest);
    ASSERT_TRUE(dest != nullptr);

    auto op = CoordinateOperationFactory::create()->createOperation(
        NN_CHECK_ASSERT(src), NN_CHECK_ASSERT(dest));
    ASSERT_TRUE(op != nullptr);
    EXPECT_EQ(op->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=pipeline "
              "+step +proj=unitconvert +xy_in=deg +xy_out=rad "
              "+step +inv +proj=geoc +ellps=WGS84 "
              "+step +proj=cart +ellps=WGS84");
}

// ---------------------------------------------------------------------------

TEST(operation, createOperation_bound_of_spherical_ocentric_to_same_type) {
    auto objSrc = PROJStringParser().createFromPROJString(
        "+proj=longlat +geoc +ellps=GRS80 +towgs84=1,2,3 +type=crs");
    auto src = nn_dynamic_pointer_cast<BoundCRS>(objSrc);
    ASSERT_TRUE(src != nullptr);

    auto objDest = PROJStringParser().createFromPROJString(
        "+proj=longlat +geoc +ellps=clrk66 +towgs84=4,5,6 +type=crs");
    auto dest = nn_dynamic_pointer_cast<BoundCRS>(objDest);
    ASSERT_TRUE(dest != nullptr);

    auto op = CoordinateOperationFactory::create()->createOperation(
        NN_CHECK_ASSERT(src), NN_CHECK_ASSERT(dest));
    ASSERT_TRUE(op != nullptr);
    EXPECT_EQ(op->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=pipeline "
              "+step +proj=unitconvert +xy_in=deg +xy_out=rad "
              "+step +inv +proj=geoc +ellps=GRS80 "
              "+step +proj=push +v_3 "
              "+step +proj=cart +ellps=GRS80 "
              "+step +proj=helmert +x=-3 +y=-3 +z=-3 "
              "+step +inv +proj=cart +ellps=clrk66 "
              "+step +proj=pop +v_3 "
              "+step +proj=geoc +ellps=clrk66 "
              "+step +proj=unitconvert +xy_in=rad +xy_out=deg");
}

// ---------------------------------------------------------------------------

TEST(operation, createOperation_spherical_ocentric_to_projected) {
    auto objSrc = PROJStringParser().createFromPROJString(
        "+proj=longlat +geoc +datum=WGS84 +type=crs");
    auto src = nn_dynamic_pointer_cast<CRS>(objSrc);
    ASSERT_TRUE(src != nullptr);

    auto objDest = PROJStringParser().createFromPROJString(
        "+proj=utm +zone=11 +datum=WGS84 +type=crs");
    auto dest = nn_dynamic_pointer_cast<CRS>(objDest);
    ASSERT_TRUE(dest != nullptr);

    auto op = CoordinateOperationFactory::create()->createOperation(
        NN_CHECK_ASSERT(src), NN_CHECK_ASSERT(dest));
    ASSERT_TRUE(op != nullptr);
    EXPECT_EQ(op->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=pipeline "
              "+step +proj=unitconvert +xy_in=deg +xy_out=rad "
              "+step +inv +proj=geoc +ellps=WGS84 "
              "+step +proj=utm +zone=11 +ellps=WGS84");
}

// ---------------------------------------------------------------------------

TEST(operation,
     createOperation_spherical_ocentric_to_projected_of_spherical_ocentric) {
    auto objSrc = PROJStringParser().createFromPROJString(
        "+proj=longlat +geoc +datum=WGS84 +type=crs");
    auto src = nn_dynamic_pointer_cast<CRS>(objSrc);
    ASSERT_TRUE(src != nullptr);

    auto objDest = PROJStringParser().createFromPROJString(
        "+proj=utm +geoc +zone=11 +datum=WGS84 +type=crs");
    auto dest = nn_dynamic_pointer_cast<CRS>(objDest);
    ASSERT_TRUE(dest != nullptr);

    auto op = CoordinateOperationFactory::create()->createOperation(
        NN_CHECK_ASSERT(src), NN_CHECK_ASSERT(dest));
    ASSERT_TRUE(op != nullptr);
    EXPECT_EQ(op->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=pipeline "
              "+step +proj=unitconvert +xy_in=deg +xy_out=rad "
              "+step +inv +proj=geoc +ellps=WGS84 "
              "+step +proj=utm +zone=11 +ellps=WGS84");
}

// ---------------------------------------------------------------------------

TEST(operation,
     createOperation_spherical_ocentric_spherical_to_ellipsoidal_north_west) {
    auto objSrc = WKTParser().createFromWKT(
        "GEODCRS[\"Mercury (2015) - Sphere / Ocentric\",\n"
        "    DATUM[\"Mercury (2015) - Sphere\",\n"
        "        ELLIPSOID[\"Mercury (2015) - Sphere\",2440530,0,\n"
        "            LENGTHUNIT[\"metre\",1]]],\n"
        "    PRIMEM[\"Reference Meridian\",0,\n"
        "        ANGLEUNIT[\"degree\",0.0174532925199433]],\n"
        "    CS[spherical,2],\n"
        "        AXIS[\"planetocentric latitude (U)\",north,\n"
        "            ORDER[1],\n"
        "            ANGLEUNIT[\"degree\",0.0174532925199433]],\n"
        "        AXIS[\"planetocentric longitude (V)\",east,\n"
        "            ORDER[2],\n"
        "            ANGLEUNIT[\"degree\",0.0174532925199433]]]");
    auto src = nn_dynamic_pointer_cast<CRS>(objSrc);
    ASSERT_TRUE(src != nullptr);

    auto objDest = WKTParser().createFromWKT(
        "GEOGCRS[\"Mercury (2015) / Ographic\",\n"
        "    DATUM[\"Mercury (2015)\",\n"
        "        ELLIPSOID[\"Mercury (2015)\",2440530,1075.12334801762,\n"
        "            LENGTHUNIT[\"metre\",1]]],\n"
        "    PRIMEM[\"Reference Meridian\",0,\n"
        "        ANGLEUNIT[\"degree\",0.0174532925199433]],\n"
        "    CS[ellipsoidal,2],\n"
        "        AXIS[\"latitude\",north,\n"
        "            ORDER[1],\n"
        "            ANGLEUNIT[\"degree\",0.0174532925199433]],\n"
        "        AXIS[\"longitude\",west,\n"
        "            ORDER[2],\n"
        "            ANGLEUNIT[\"degree\",0.0174532925199433]]]"

    );
    auto dest = nn_dynamic_pointer_cast<CRS>(objDest);
    ASSERT_TRUE(dest != nullptr);

    {
        auto op = CoordinateOperationFactory::create()->createOperation(
            NN_CHECK_ASSERT(src), NN_CHECK_ASSERT(dest));
        ASSERT_TRUE(op != nullptr);
        EXPECT_EQ(op->exportToPROJString(PROJStringFormatter::create().get()),
                  "+proj=axisswap +order=1,-2");
    }

    {
        auto op = CoordinateOperationFactory::create()->createOperation(
            NN_CHECK_ASSERT(dest), NN_CHECK_ASSERT(src));
        ASSERT_TRUE(op != nullptr);
        EXPECT_EQ(op->exportToPROJString(PROJStringFormatter::create().get()),
                  "+proj=axisswap +order=1,-2");
    }
}

// ---------------------------------------------------------------------------

TEST(
    operation,
    createOperation_ellipsoidal_ographic_west_to_projected_of_ellipsoidal_ographic_west) {
    auto authFactory =
        AuthorityFactory::create(DatabaseContext::create(), "IAU_2015");
    auto op = CoordinateOperationFactory::create()->createOperation(
        authFactory->createCoordinateReferenceSystem("19901"),
        authFactory->createCoordinateReferenceSystem("19911"));
    ASSERT_TRUE(op != nullptr);
    EXPECT_EQ(op->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=pipeline "
              "+step +proj=axisswap +order=-2,1 "
              "+step +proj=unitconvert +xy_in=deg +xy_out=rad "
              "+step +proj=eqc +lat_ts=0 +lat_0=0 +lon_0=0 +x_0=0 +y_0=0 "
              "+a=2440530 +b=2438260 "
              "+step +proj=axisswap +order=-1,2");

    // Inverse
    auto op2 = CoordinateOperationFactory::create()->createOperation(
        authFactory->createCoordinateReferenceSystem("19911"),
        authFactory->createCoordinateReferenceSystem("19901"));
    ASSERT_TRUE(op2 != nullptr);
    EXPECT_EQ(op2->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=pipeline "
              "+step +inv +proj=axisswap +order=-1,2 "
              "+step +inv +proj=eqc +lat_ts=0 +lat_0=0 +lon_0=0 +x_0=0 +y_0=0 "
              "+a=2440530 +b=2438260 "
              "+step +proj=unitconvert +xy_in=rad +xy_out=deg "
              "+step +proj=axisswap +order=2,-1");
}

// ---------------------------------------------------------------------------

TEST(
    operation,
    createOperation_ellipsoidal_ographic_west_to_projected_of_ellipsoidal_ocentric) {
    auto authFactory =
        AuthorityFactory::create(DatabaseContext::create(), "IAU_2015");
    auto op = CoordinateOperationFactory::create()->createOperation(
        authFactory->createCoordinateReferenceSystem("19901"),
        authFactory->createCoordinateReferenceSystem("19912"));
    ASSERT_TRUE(op != nullptr);
    EXPECT_EQ(op->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=pipeline "
              "+step +proj=axisswap +order=-2,1 "
              "+step +proj=unitconvert +xy_in=deg +xy_out=rad "
              "+step +proj=eqc +lat_ts=0 +lat_0=0 +lon_0=0 +x_0=0 +y_0=0 "
              "+a=2440530 +b=2438260");

    // Inverse
    auto op2 = CoordinateOperationFactory::create()->createOperation(
        authFactory->createCoordinateReferenceSystem("19912"),
        authFactory->createCoordinateReferenceSystem("19901"));
    ASSERT_TRUE(op2 != nullptr);
    EXPECT_EQ(op2->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=pipeline "
              "+step +inv +proj=eqc +lat_ts=0 +lat_0=0 +lon_0=0 +x_0=0 +y_0=0 "
              "+a=2440530 +b=2438260 "
              "+step +proj=unitconvert +xy_in=rad +xy_out=deg "
              "+step +proj=axisswap +order=2,-1");
}

// ---------------------------------------------------------------------------

TEST(
    operation,
    createOperation_ellipsoidal_ocentric_to_projected_of_ellipsoidal_ocentric) {
    auto authFactory =
        AuthorityFactory::create(DatabaseContext::create(), "IAU_2015");
    auto op = CoordinateOperationFactory::create()->createOperation(
        authFactory->createCoordinateReferenceSystem("19902"),
        authFactory->createCoordinateReferenceSystem("19912"));
    ASSERT_TRUE(op != nullptr);
    EXPECT_EQ(op->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=pipeline "
              "+step +proj=axisswap +order=2,1 "
              "+step +proj=unitconvert +xy_in=deg +xy_out=rad "
              "+step +inv +proj=geoc +a=2440530 +b=2438260 "
              "+step +proj=eqc +lat_ts=0 +lat_0=0 +lon_0=0 +x_0=0 +y_0=0 "
              "+a=2440530 +b=2438260");

    // Inverse
    auto op2 = CoordinateOperationFactory::create()->createOperation(
        authFactory->createCoordinateReferenceSystem("19912"),
        authFactory->createCoordinateReferenceSystem("19902"));
    ASSERT_TRUE(op2 != nullptr);
    EXPECT_EQ(op2->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=pipeline "
              "+step +inv +proj=eqc +lat_ts=0 +lat_0=0 +lon_0=0 +x_0=0 +y_0=0 "
              "+a=2440530 +b=2438260 "
              "+step +proj=geoc +a=2440530 +b=2438260 "
              "+step +proj=unitconvert +xy_in=rad +xy_out=deg "
              "+step +proj=axisswap +order=2,1");
}

// ---------------------------------------------------------------------------

TEST(
    operation,
    createOperation_ellipsoidal_ocentric_to_projected_of_ellipsoidal_ographic_west) {
    auto authFactory =
        AuthorityFactory::create(DatabaseContext::create(), "IAU_2015");
    auto op = CoordinateOperationFactory::create()->createOperation(
        authFactory->createCoordinateReferenceSystem("19902"),
        authFactory->createCoordinateReferenceSystem("19911"));
    ASSERT_TRUE(op != nullptr);
    EXPECT_EQ(op->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=pipeline "
              "+step +proj=axisswap +order=2,1 "
              "+step +proj=unitconvert +xy_in=deg +xy_out=rad "
              "+step +inv +proj=geoc +a=2440530 +b=2438260 "
              "+step +proj=eqc +lat_ts=0 +lat_0=0 +lon_0=0 +x_0=0 +y_0=0 "
              "+a=2440530 +b=2438260 "
              "+step +proj=axisswap +order=-1,2");

    // Inverse
    auto op2 = CoordinateOperationFactory::create()->createOperation(
        authFactory->createCoordinateReferenceSystem("19911"),
        authFactory->createCoordinateReferenceSystem("19902"));
    ASSERT_TRUE(op2 != nullptr);
    EXPECT_EQ(op2->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=pipeline "
              "+step +inv +proj=axisswap +order=-1,2 "
              "+step +inv +proj=eqc +lat_ts=0 +lat_0=0 +lon_0=0 +x_0=0 +y_0=0 "
              "+a=2440530 +b=2438260 "
              "+step +proj=geoc +a=2440530 +b=2438260 "
              "+step +proj=unitconvert +xy_in=rad +xy_out=deg "
              "+step +proj=axisswap +order=2,1");
}

// ---------------------------------------------------------------------------

TEST(operation, createOperation_ossfuzz_47873) {
    auto objSrc = PROJStringParser().createFromPROJString(
        "+proj=ob_tran +o_proj=longlat +o_lat_1=1 +o_lat_2=2 +datum=WGS84 "
        "+geoidgrids=@x +geoid_crs=horizontal_crs +type=crs");
    auto src = nn_dynamic_pointer_cast<CRS>(objSrc);
    ASSERT_TRUE(src != nullptr);
    auto objDst = PROJStringParser().createFromPROJString(
        "+proj=longlat +datum=WGS84 +geoidgrids=@y +type=crs");
    auto dst = nn_dynamic_pointer_cast<CRS>(objDst);
    ASSERT_TRUE(dst != nullptr);

    // Just check that we don't go into an infinite recursion
    try {
        CoordinateOperationFactory::create()->createOperation(
            NN_CHECK_ASSERT(src), NN_CHECK_ASSERT(dst));
    } catch (const std::exception &) {
    }
}

// ---------------------------------------------------------------------------

TEST(operation, createOperation_ossfuzz_47873_simplified_if_i_might_say) {
    auto wkt =
        "BOUNDCRS[\n"
        "    SOURCECRS[\n"
        "        VERTCRS[\"unknown\",\n"
        "            VDATUM[\"unknown using geoidgrids=@x\"],\n"
        "            CS[vertical,1],\n"
        "                AXIS[\"gravity-related height (H)\",up,\n"
        "                    LENGTHUNIT[\"metre\",1,\n"
        "                        ID[\"EPSG\",9001]]]]],\n"
        "    TARGETCRS[\n"
        "        GEOGCRS[\"unnamed\",\n"
        "            BASEGEOGCRS[\"unknown\",\n"
        "                DATUM[\"World Geodetic System 1984\",\n"
        "                    ELLIPSOID[\"WGS 84\",6378137,298.257223563,\n"
        "                        LENGTHUNIT[\"metre\",1]],\n"
        "                    ID[\"EPSG\",6326]],\n"
        "                PRIMEM[\"Greenwich\",0,\n"
        "                    ANGLEUNIT[\"degree\",0.0174532925199433],\n"
        "                    ID[\"EPSG\",8901]]],\n"
        "            DERIVINGCONVERSION[\"unknown\",\n"
        "                METHOD[\"PROJ ob_tran o_proj=longlat\"],\n"
        "                PARAMETER[\"o_lat_1\",1,\n"
        "                    ANGLEUNIT[\"degree\",0.0174532925199433,\n"
        "                        ID[\"EPSG\",9122]]],\n"
        "                PARAMETER[\"o_lat_2\",2,\n"
        "                    ANGLEUNIT[\"degree\",0.0174532925199433,\n"
        "                        ID[\"EPSG\",9122]]]],\n"
        "            CS[ellipsoidal,3],\n"
        "                AXIS[\"longitude\",east,\n"
        "                    ORDER[1],\n"
        "                    ANGLEUNIT[\"degree\",0.0174532925199433,\n"
        "                        ID[\"EPSG\",9122]]],\n"
        "                AXIS[\"latitude\",north,\n"
        "                    ORDER[2],\n"
        "                    ANGLEUNIT[\"degree\",0.0174532925199433,\n"
        "                        ID[\"EPSG\",9122]]],\n"
        "                AXIS[\"ellipsoidal height (h)\",up,\n"
        "                    ORDER[3],\n"
        "                    LENGTHUNIT[\"metre\",1,\n"
        "                        ID[\"EPSG\",9001]]]]],\n"
        "    ABRIDGEDTRANSFORMATION[\"unknown to unnamed ellipsoidal "
        "height\",\n"
        "        METHOD[\"GravityRelatedHeight to Geographic3D\"],\n"
        "        PARAMETERFILE[\"Geoid (height correction) model "
        "file\",\"@x\",\n"
        "            ID[\"EPSG\",8666]]]]";
    auto objSrc = WKTParser().createFromWKT(wkt);
    auto src = nn_dynamic_pointer_cast<CRS>(objSrc);
    ASSERT_TRUE(src != nullptr);

    auto authFactory =
        AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    auto dst = authFactory->createCoordinateReferenceSystem("4979");

    // Just check that we don't go into an infinite recursion
    try {
        CoordinateOperationFactory::create()->createOperation(
            NN_CHECK_ASSERT(src), dst);
    } catch (const std::exception &) {
    }
}

// ---------------------------------------------------------------------------

TEST(operation, createOperation_derived_projected_crs) {

    auto authFactory =
        AuthorityFactory::create(DatabaseContext::create(), "EPSG");
    auto src = authFactory->createCoordinateReferenceSystem("6507");

    auto wkt =
        "DERIVEDPROJCRS[\"Custom Site Calibrated CRS\",\n"
        "    BASEPROJCRS[\"NAD83(2011) / Mississippi East (ftUS)\",\n"
        "        BASEGEOGCRS[\"NAD83(2011)\",\n"
        "            DATUM[\"NAD83 (National Spatial Reference System "
        "2011)\",\n"
        "                ELLIPSOID[\"GRS 1980\",6378137,298.257222101,\n"
        "                    LENGTHUNIT[\"metre\",1]]],\n"
        "            PRIMEM[\"Greenwich\",0,\n"
        "                ANGLEUNIT[\"degree\",0.0174532925199433]]],\n"
        "        CONVERSION[\"SPCS83 Mississippi East zone (US Survey "
        "feet)\",\n"
        "            METHOD[\"Transverse Mercator\",\n"
        "                ID[\"EPSG\",9807]],\n"
        "            PARAMETER[\"Latitude of natural origin\",29.5,\n"
        "                ANGLEUNIT[\"degree\",0.0174532925199433],\n"
        "                ID[\"EPSG\",8801]],\n"
        "            PARAMETER[\"Longitude of natural "
        "origin\",-88.8333333333333,\n"
        "                ANGLEUNIT[\"degree\",0.0174532925199433],\n"
        "                ID[\"EPSG\",8802]],\n"
        "            PARAMETER[\"Scale factor at natural origin\",0.99995,\n"
        "                SCALEUNIT[\"unity\",1],\n"
        "                ID[\"EPSG\",8805]],\n"
        "            PARAMETER[\"False easting\",984250,\n"
        "                LENGTHUNIT[\"US survey foot\",0.304800609601219],\n"
        "                ID[\"EPSG\",8806]],\n"
        "            PARAMETER[\"False northing\",0,\n"
        "                LENGTHUNIT[\"US survey foot\",0.304800609601219],\n"
        "                ID[\"EPSG\",8807]]]],\n"
        "    DERIVINGCONVERSION[\"Affine transformation as PROJ-based\",\n"
        "        METHOD[\"PROJ-based operation method: "
        "+proj=pipeline +step +proj=unitconvert +xy_in=m +xy_out=us-ft "
        "+step +proj=affine +xoff=20 "
        "+step +proj=unitconvert +xy_in=us-ft +xy_out=m\"]],\n"
        "    CS[Cartesian,2],\n"
        "        AXIS[\"northing (Y)\",north,\n"
        "            LENGTHUNIT[\"US survey foot\",0.304800609601219]],\n"
        "        AXIS[\"easting (X)\",east,\n"
        "            LENGTHUNIT[\"US survey foot\",0.304800609601219]],\n"
        "    REMARK[\"EPSG:6507 with 20 feet offset and axis inversion\"]]";
    auto objDst = WKTParser().createFromWKT(wkt);
    auto dst = nn_dynamic_pointer_cast<CRS>(objDst);
    ASSERT_TRUE(dst != nullptr);

    auto op = CoordinateOperationFactory::create()->createOperation(
        src, NN_NO_CHECK(dst));
    ASSERT_TRUE(op != nullptr);
    EXPECT_EQ(op->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=pipeline "
              "+step +proj=affine +xoff=20 "
              "+step +proj=axisswap +order=2,1");
}

TEST(operation,
     geogCRS_to_compoundCRS_with_boundVerticalCRS_and_derivedProjected) {
    auto wkt =
        "DERIVEDPROJCRS[\"Custom Site Calibrated CRS\",\n"
        "    BASEPROJCRS[\"NAD83(2011) / Mississippi East (ftUS)\",\n"
        "        BASEGEOGCRS[\"NAD83(2011)\",\n"
        "            DATUM[\"NAD83 (National Spatial Reference System "
        "2011)\",\n"
        "                ELLIPSOID[\"GRS 1980\",6378137,298.257222101,\n"
        "                    LENGTHUNIT[\"metre\",1]]],\n"
        "            PRIMEM[\"Greenwich\",0,\n"
        "                ANGLEUNIT[\"degree\",0.0174532925199433]]],\n"
        "        CONVERSION[\"SPCS83 Mississippi East zone (US Survey "
        "feet)\",\n"
        "            METHOD[\"Transverse Mercator\",\n"
        "                ID[\"EPSG\",9807]],\n"
        "            PARAMETER[\"Latitude of natural origin\",29.5,\n"
        "                ANGLEUNIT[\"degree\",0.0174532925199433],\n"
        "                ID[\"EPSG\",8801]],\n"
        "            PARAMETER[\"Longitude of natural "
        "origin\",-88.8333333333333,\n"
        "                ANGLEUNIT[\"degree\",0.0174532925199433],\n"
        "                ID[\"EPSG\",8802]],\n"
        "            PARAMETER[\"Scale factor at natural origin\",0.99995,\n"
        "                SCALEUNIT[\"unity\",1],\n"
        "                ID[\"EPSG\",8805]],\n"
        "            PARAMETER[\"False easting\",984250,\n"
        "                LENGTHUNIT[\"US survey foot\",0.304800609601219],\n"
        "                ID[\"EPSG\",8806]],\n"
        "            PARAMETER[\"False northing\",0,\n"
        "                LENGTHUNIT[\"US survey foot\",0.304800609601219],\n"
        "                ID[\"EPSG\",8807]]]],\n"
        "    DERIVINGCONVERSION[\"Affine transformation as PROJ-based\",\n"
        "        METHOD[\"PROJ-based operation method: "
        "+proj=pipeline +step +proj=unitconvert +xy_in=m +xy_out=us-ft "
        "+step +proj=affine +xoff=20 "
        "+step +proj=unitconvert +xy_in=us-ft +xy_out=m\"]],\n"
        "    CS[Cartesian,2],\n"
        "        AXIS[\"northing (Y)\",north,\n"
        "            LENGTHUNIT[\"US survey foot\",0.304800609601219]],\n"
        "        AXIS[\"easting (X)\",east,\n"
        "            LENGTHUNIT[\"US survey foot\",0.304800609601219]],\n"
        "    REMARK[\"EPSG:6507 with 20 feet offset and axis inversion\"]]";
    auto objDst = WKTParser().createFromWKT(wkt);
    auto dst = nn_dynamic_pointer_cast<CRS>(objDst);
    ASSERT_TRUE(dst != nullptr);

    auto compound = CompoundCRS::create(
        PropertyMap(),
        std::vector<CRSNNPtr>{NN_NO_CHECK(dst), createBoundVerticalCRS()});
    auto op = CoordinateOperationFactory::create()->createOperation(
        GeographicCRS::EPSG_4979, compound);
    ASSERT_TRUE(op != nullptr);
    EXPECT_EQ(op->exportToPROJString(PROJStringFormatter::create().get()),
              "+proj=pipeline "
              "+step +proj=axisswap +order=2,1 "
              "+step +proj=unitconvert +xy_in=deg +xy_out=rad "
              "+step +inv +proj=vgridshift +grids=us_nga_egm08_25.tif "
              "+multiplier=1 "
              "+step +proj=tmerc +lat_0=29.5 +lon_0=-88.8333333333333 "
              "+k=0.99995 +x_0=300000 +y_0=0 +ellps=GRS80 "
              "+step +proj=unitconvert +xy_in=m +xy_out=us-ft "
              "+step +proj=affine +xoff=20 "
              "+step +proj=axisswap +order=2,1");
}
