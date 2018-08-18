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

namespace {
struct UnrelatedObject : public BaseObject {
    UnrelatedObject() = default;
};

static nn<std::shared_ptr<UnrelatedObject>> createUnrelatedObject() {
    return nn_make_shared<UnrelatedObject>();
}
}

// ---------------------------------------------------------------------------

TEST(operation, method) {

    auto method = OperationMethod::create(
        PropertyMap(), std::vector<OperationParameterNNPtr>{});
    EXPECT_TRUE(method->isEquivalentTo(method));
    EXPECT_FALSE(method->isEquivalentTo(createUnrelatedObject()));
    auto otherMethod = OperationMethod::create(
        PropertyMap(),
        std::vector<OperationParameterNNPtr>{OperationParameter::create(
            PropertyMap().set(IdentifiedObject::NAME_KEY, "paramName"))});
    EXPECT_TRUE(otherMethod->isEquivalentTo(otherMethod));
    EXPECT_FALSE(method->isEquivalentTo(otherMethod));
    auto otherMethod2 = OperationMethod::create(
        PropertyMap(),
        std::vector<OperationParameterNNPtr>{OperationParameter::create(
            PropertyMap().set(IdentifiedObject::NAME_KEY, "paramName2"))});
    EXPECT_FALSE(otherMethod->isEquivalentTo(otherMethod2));
}

// ---------------------------------------------------------------------------

TEST(operation, ParameterValue) {

    auto valStr1 = ParameterValue::create("str1");
    auto valStr2 = ParameterValue::create("str2");
    EXPECT_TRUE(valStr1->isEquivalentTo(valStr1));
    EXPECT_FALSE(valStr1->isEquivalentTo(createUnrelatedObject()));
    EXPECT_FALSE(valStr1->isEquivalentTo(valStr2));

    auto valMeasure1 = ParameterValue::create(Angle(90.0));
    auto valMeasure2 = ParameterValue::create(Angle(89.0));
    EXPECT_TRUE(valMeasure1->isEquivalentTo(valMeasure1));
    EXPECT_FALSE(valMeasure1->isEquivalentTo(valStr1));
    EXPECT_FALSE(valMeasure1->isEquivalentTo(valMeasure2));

    auto valInt1 = ParameterValue::create(1);
    auto valInt2 = ParameterValue::create(2);
    EXPECT_TRUE(valInt1->isEquivalentTo(valInt1));
    EXPECT_FALSE(valInt1->isEquivalentTo(valInt2));

    auto valTrue = ParameterValue::create(true);
    auto valFalse = ParameterValue::create(false);
    EXPECT_TRUE(valTrue->isEquivalentTo(valTrue));
    EXPECT_FALSE(valTrue->isEquivalentTo(valFalse));
}

// ---------------------------------------------------------------------------

TEST(operation, OperationParameter) {

    auto op1 = OperationParameter::create(
        PropertyMap().set(IdentifiedObject::NAME_KEY, "paramName"));
    auto op2 = OperationParameter::create(
        PropertyMap().set(IdentifiedObject::NAME_KEY, "paramName2"));
    EXPECT_TRUE(op1->isEquivalentTo(op1));
    EXPECT_FALSE(op1->isEquivalentTo(createUnrelatedObject()));
    EXPECT_FALSE(op1->isEquivalentTo(op2));
}

// ---------------------------------------------------------------------------

TEST(operation, OperationParameterValue) {

    auto op1 = OperationParameter::create(
        PropertyMap().set(IdentifiedObject::NAME_KEY, "paramName"));
    auto op2 = OperationParameter::create(
        PropertyMap().set(IdentifiedObject::NAME_KEY, "paramName2"));
    auto valStr1 = ParameterValue::create("str1");
    auto valStr2 = ParameterValue::create("str2");
    auto opv11 = OperationParameterValue::create(op1, valStr1);
    EXPECT_TRUE(opv11->isEquivalentTo(opv11));
    EXPECT_FALSE(opv11->isEquivalentTo(createUnrelatedObject()));
    auto opv12 = OperationParameterValue::create(op1, valStr2);
    EXPECT_FALSE(opv11->isEquivalentTo(opv12));
    auto opv21 = OperationParameterValue::create(op2, valStr1);
    EXPECT_FALSE(opv11->isEquivalentTo(opv12));
}

// ---------------------------------------------------------------------------

TEST(operation, SingleOperation) {

    auto sop1 = Transformation::create(
        PropertyMap(), nn_static_pointer_cast<CRS>(GeographicCRS::EPSG_4326),
        nn_static_pointer_cast<CRS>(GeographicCRS::EPSG_4807),
        static_cast<CRSPtr>(GeographicCRS::EPSG_4979.as_nullable()),
        PropertyMap(),
        std::vector<OperationParameterNNPtr>{OperationParameter::create(
            PropertyMap().set(IdentifiedObject::NAME_KEY, "paramName"))},
        std::vector<ParameterValueNNPtr>{
            ParameterValue::createFilename("foo.bin")},
        std::vector<PositionalAccuracyNNPtr>{
            PositionalAccuracy::create("0.1")});

    EXPECT_TRUE(sop1->isEquivalentTo(sop1));
    EXPECT_FALSE(sop1->isEquivalentTo(createUnrelatedObject()));

    auto sop2 = Transformation::create(
        PropertyMap(), nn_static_pointer_cast<CRS>(GeographicCRS::EPSG_4326),
        nn_static_pointer_cast<CRS>(GeographicCRS::EPSG_4807),
        static_cast<CRSPtr>(GeographicCRS::EPSG_4979.as_nullable()),
        PropertyMap(),
        std::vector<OperationParameterNNPtr>{OperationParameter::create(
            PropertyMap().set(IdentifiedObject::NAME_KEY, "paramName2"))},
        std::vector<ParameterValueNNPtr>{
            ParameterValue::createFilename("foo.bin")},
        std::vector<PositionalAccuracyNNPtr>{
            PositionalAccuracy::create("0.1")});
    EXPECT_FALSE(sop1->isEquivalentTo(sop2));

    auto sop3 = Transformation::create(
        PropertyMap(), nn_static_pointer_cast<CRS>(GeographicCRS::EPSG_4326),
        nn_static_pointer_cast<CRS>(GeographicCRS::EPSG_4807),
        static_cast<CRSPtr>(GeographicCRS::EPSG_4979.as_nullable()),
        PropertyMap(),
        std::vector<OperationParameterNNPtr>{
            OperationParameter::create(
                PropertyMap().set(IdentifiedObject::NAME_KEY, "paramName")),
            OperationParameter::create(
                PropertyMap().set(IdentifiedObject::NAME_KEY, "paramName2"))},
        std::vector<ParameterValueNNPtr>{
            ParameterValue::createFilename("foo.bin"),
            ParameterValue::createFilename("foo2.bin")},
        std::vector<PositionalAccuracyNNPtr>{
            PositionalAccuracy::create("0.1")});
    EXPECT_FALSE(sop1->isEquivalentTo(sop3));

    auto sop4 = Transformation::create(
        PropertyMap(), nn_static_pointer_cast<CRS>(GeographicCRS::EPSG_4326),
        nn_static_pointer_cast<CRS>(GeographicCRS::EPSG_4807),
        static_cast<CRSPtr>(GeographicCRS::EPSG_4979.as_nullable()),
        PropertyMap(),
        std::vector<OperationParameterNNPtr>{OperationParameter::create(
            PropertyMap().set(IdentifiedObject::NAME_KEY, "paramName"))},
        std::vector<ParameterValueNNPtr>{
            ParameterValue::createFilename("foo2.bin")},
        std::vector<PositionalAccuracyNNPtr>{
            PositionalAccuracy::create("0.1")});
    EXPECT_FALSE(sop1->isEquivalentTo(sop4));
}

// ---------------------------------------------------------------------------

TEST(operation, transformation_to_wkt) {
    PropertyMap propertiesTransformation;
    propertiesTransformation
        .set(Identifier::CODESPACE_KEY, "codeSpaceTransformation")
        .set(Identifier::CODE_KEY, "codeTransformation")
        .set(IdentifiedObject::NAME_KEY, "transformationName")
        .set(IdentifiedObject::REMARKS_KEY, "my remarks");

    auto transf = Transformation::create(
        propertiesTransformation,
        nn_static_pointer_cast<CRS>(GeographicCRS::EPSG_4326),
        nn_static_pointer_cast<CRS>(GeographicCRS::EPSG_4807),
        static_cast<CRSPtr>(GeographicCRS::EPSG_4979.as_nullable()),
        PropertyMap()
            .set(Identifier::CODESPACE_KEY, "codeSpaceOperationMethod")
            .set(Identifier::CODE_KEY, "codeOperationMethod")
            .set(IdentifiedObject::NAME_KEY, "operationMethodName"),
        std::vector<OperationParameterNNPtr>{OperationParameter::create(
            PropertyMap().set(IdentifiedObject::NAME_KEY, "paramName"))},
        std::vector<ParameterValueNNPtr>{
            ParameterValue::createFilename("foo.bin")},
        std::vector<PositionalAccuracyNNPtr>{
            PositionalAccuracy::create("0.1")});

    std::string src_wkt;
    {
        auto formatter = WKTFormatter::create();
        formatter->setOutputId(false);
        src_wkt = GeographicCRS::EPSG_4326->exportToWKT(formatter);
    }

    std::string dst_wkt;
    {
        auto formatter = WKTFormatter::create();
        formatter->setOutputId(false);
        dst_wkt = GeographicCRS::EPSG_4807->exportToWKT(formatter);
    }

    std::string interpolation_wkt;
    {
        auto formatter = WKTFormatter::create();
        formatter->setOutputId(false);
        interpolation_wkt = GeographicCRS::EPSG_4979->exportToWKT(formatter);
    }

    auto expected =
        "COORDINATEOPERATION[\"transformationName\",\n"
        "    SOURCECRS[" +
        src_wkt + "],\n"
                  "    TARGETCRS[" +
        dst_wkt +
        "],\n"
        "    METHOD[\"operationMethodName\",\n"
        "        ID[\"codeSpaceOperationMethod\",\"codeOperationMethod\"]],\n"
        "    PARAMETERFILE[\"paramName\",\"foo.bin\"],\n"
        "    INTERPOLATIONCRS[" +
        interpolation_wkt +
        "],\n"
        "    OPERATIONACCURACY[0.1],\n"
        "    ID[\"codeSpaceTransformation\",\"codeTransformation\"],\n"
        "    REMARK[\"my remarks\"]]";

    EXPECT_EQ(replaceAll(replaceAll(transf->exportToWKT(WKTFormatter::create()),
                                    " ", ""),
                         "\n", ""),
              replaceAll(replaceAll(expected, " ", ""), "\n", ""));

    EXPECT_THROW(transf->exportToWKT(
                     WKTFormatter::create(WKTFormatter::Convention::WKT1_GDAL)),
                 FormattingException);

    EXPECT_TRUE(transf->isEquivalentTo(transf));
    EXPECT_FALSE(transf->isEquivalentTo(createUnrelatedObject()));
}

// ---------------------------------------------------------------------------

TEST(operation, concatenated_operation) {

    PropertyMap propertiesTransformation;
    propertiesTransformation
        .set(Identifier::CODESPACE_KEY, "codeSpaceTransformation")
        .set(Identifier::CODE_KEY, "codeTransformation")
        .set(IdentifiedObject::NAME_KEY, "transformationName")
        .set(IdentifiedObject::REMARKS_KEY, "my remarks");

    auto transf_1 = Transformation::create(
        propertiesTransformation,
        nn_static_pointer_cast<CRS>(GeographicCRS::EPSG_4326),
        nn_static_pointer_cast<CRS>(GeographicCRS::EPSG_4807), nullptr,
        PropertyMap().set(IdentifiedObject::NAME_KEY, "operationMethodName"),
        std::vector<OperationParameterNNPtr>{OperationParameter::create(
            PropertyMap().set(IdentifiedObject::NAME_KEY, "paramName"))},
        std::vector<ParameterValueNNPtr>{
            ParameterValue::createFilename("foo.bin")},
        std::vector<PositionalAccuracyNNPtr>());

    auto transf_2 = Transformation::create(
        propertiesTransformation,
        nn_static_pointer_cast<CRS>(GeographicCRS::EPSG_4807),
        nn_static_pointer_cast<CRS>(GeographicCRS::EPSG_4979), nullptr,
        PropertyMap().set(IdentifiedObject::NAME_KEY, "operationMethodName"),
        std::vector<OperationParameterNNPtr>{OperationParameter::create(
            PropertyMap().set(IdentifiedObject::NAME_KEY, "paramName"))},
        std::vector<ParameterValueNNPtr>{
            ParameterValue::createFilename("foo.bin")},
        std::vector<PositionalAccuracyNNPtr>());

    auto concat = ConcatenatedOperation::create(
        PropertyMap()
            .set(Identifier::CODESPACE_KEY, "codeSpace")
            .set(Identifier::CODE_KEY, "code")
            .set(IdentifiedObject::NAME_KEY, "name")
            .set(IdentifiedObject::REMARKS_KEY, "my remarks"),
        std::vector<CoordinateOperationNNPtr>{transf_1, transf_2},
        std::vector<PositionalAccuracyNNPtr>{
            PositionalAccuracy::create("0.1")});

    std::string src_wkt;
    {
        auto formatter =
            WKTFormatter::create(WKTFormatter::Convention::WKT2_2018);
        formatter->setOutputId(false);
        src_wkt = GeographicCRS::EPSG_4326->exportToWKT(formatter);
    }

    std::string dst_wkt;
    {
        auto formatter =
            WKTFormatter::create(WKTFormatter::Convention::WKT2_2018);
        formatter->setOutputId(false);
        dst_wkt = GeographicCRS::EPSG_4979->exportToWKT(formatter);
    }

    std::string step1_wkt;
    {
        auto formatter =
            WKTFormatter::create(WKTFormatter::Convention::WKT2_2018);
        formatter->setOutputId(false);
        step1_wkt = transf_1->exportToWKT(formatter);
    }

    std::string step2_wkt;
    {
        auto formatter =
            WKTFormatter::create(WKTFormatter::Convention::WKT2_2018);
        formatter->setOutputId(false);
        step2_wkt = transf_2->exportToWKT(formatter);
    }

    auto expected = "CONCATENATEDOPERATION[\"name\",\n"
                    "    SOURCECRS[" +
                    src_wkt + "],\n"
                              "    TARGETCRS[" +
                    dst_wkt + "],\n"
                              "    STEP[" +
                    step1_wkt + "],\n"
                                "    STEP[" +
                    step2_wkt + "],\n"
                                "    ID[\"codeSpace\",\"code\"],\n"
                                "    REMARK[\"my remarks\"]]";

    EXPECT_EQ(replaceAll(replaceAll(concat->exportToWKT(WKTFormatter::create(
                                        WKTFormatter::Convention::WKT2_2018)),
                                    " ", ""),
                         "\n", ""),
              replaceAll(replaceAll(expected, " ", ""), "\n", ""));

    EXPECT_THROW(concat->exportToWKT(WKTFormatter::create()),
                 FormattingException);

    EXPECT_THROW(ConcatenatedOperation::create(
                     PropertyMap().set(IdentifiedObject::NAME_KEY, "name"),
                     std::vector<CoordinateOperationNNPtr>{transf_1, transf_1},
                     std::vector<PositionalAccuracyNNPtr>()),
                 InvalidOperation);

    auto inv = concat->inverse();
    EXPECT_EQ(*(inv->name()->description()), "Inverse of name");
    EXPECT_EQ(*(inv->sourceCRS()->name()->description()),
              *(concat->targetCRS()->name()->description()));
    EXPECT_EQ(*(inv->targetCRS()->name()->description()),
              *(concat->sourceCRS()->name()->description()));
    auto inv_as_concat = nn_dynamic_pointer_cast<ConcatenatedOperation>(inv);
    ASSERT_TRUE(inv_as_concat != nullptr);

    ASSERT_EQ(inv_as_concat->operations().size(), 2);
    EXPECT_EQ(*(inv_as_concat->operations()[0]->name()->description()),
              "Inverse of " +
                  *(concat->operations()[1]->name()->description()));
    EXPECT_EQ(*(inv_as_concat->operations()[1]->name()->description()),
              "Inverse of " +
                  *(concat->operations()[0]->name()->description()));

    EXPECT_TRUE(concat->isEquivalentTo(concat));
    EXPECT_FALSE(concat->isEquivalentTo(createUnrelatedObject()));
    EXPECT_FALSE(
        ConcatenatedOperation::create(PropertyMap(),
                                      std::vector<CoordinateOperationNNPtr>{
                                          transf_1, transf_1->inverse()},
                                      std::vector<PositionalAccuracyNNPtr>())
            ->isEquivalentTo(ConcatenatedOperation::create(
                PropertyMap(),
                std::vector<CoordinateOperationNNPtr>{transf_1->inverse(),
                                                      transf_1},
                std::vector<PositionalAccuracyNNPtr>())));
    EXPECT_FALSE(
        ConcatenatedOperation::create(PropertyMap(),
                                      std::vector<CoordinateOperationNNPtr>{
                                          transf_1, transf_1->inverse()},
                                      std::vector<PositionalAccuracyNNPtr>())
            ->isEquivalentTo(ConcatenatedOperation::create(
                PropertyMap(),
                std::vector<CoordinateOperationNNPtr>{
                    transf_1, transf_1->inverse(), transf_1},
                std::vector<PositionalAccuracyNNPtr>())));
}

// ---------------------------------------------------------------------------

TEST(operation, transformation_createGeocentricTranslations) {

    auto transf = Transformation::createGeocentricTranslations(
        PropertyMap(), GeographicCRS::EPSG_4326, GeographicCRS::EPSG_4269, 1.0,
        2.0, 3.0, std::vector<PositionalAccuracyNNPtr>());

    auto params = transf->getTOWGS84Parameters();
    auto expected = std::vector<double>{1.0, 2.0, 3.0, 0.0, 0.0, 0.0, 0.0};
    EXPECT_EQ(params, expected);

    auto inv_transf = transf->inverse();
    auto inv_transf_as_transf =
        nn_dynamic_pointer_cast<Transformation>(inv_transf);
    ASSERT_TRUE(inv_transf_as_transf != nullptr);

    EXPECT_EQ(*(transf->sourceCRS()->name()->description()),
              *(inv_transf_as_transf->targetCRS()->name()->description()));
    EXPECT_EQ(*(transf->targetCRS()->name()->description()),
              *(inv_transf_as_transf->sourceCRS()->name()->description()));
    auto expected_inv =
        std::vector<double>{-1.0, -2.0, -3.0, 0.0, 0.0, 0.0, 0.0};
    EXPECT_EQ(inv_transf_as_transf->getTOWGS84Parameters(), expected_inv);

    EXPECT_EQ(transf->exportToPROJString(PROJStringFormatter::create()),
              "+proj=pipeline +step +proj=axisswap +order=2,1 +step "
              "+proj=unitconvert +xy_in=deg +xy_out=rad +step +proj=cart "
              "+ellps=WGS84 +step +proj=helmert +x=1 +y=2 +z=3 +step +inv "
              "+proj=cart +ellps=GRS80 +step +proj=unitconvert +xy_in=rad "
              "+xy_out=deg +step +proj=axisswap +order=2,1");
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

static GeodeticCRSNNPtr createGeocentricKM() {
    PropertyMap propertiesCRS;
    propertiesCRS.set(Identifier::CODESPACE_KEY, "EPSG")
        .set(Identifier::CODE_KEY, 4328)
        .set(IdentifiedObject::NAME_KEY, "WGS 84");
    return GeodeticCRS::create(
        propertiesCRS, GeodeticReferenceFrame::EPSG_6326,
        CartesianCS::createGeocentric(
            UnitOfMeasure("kilometre", 1000.0, UnitOfMeasure::Type::LINEAR)));
}

// ---------------------------------------------------------------------------

TEST(operation,
     transformation_createGeocentricTranslations_between_geocentricCRS) {

    auto transf1 = Transformation::createGeocentricTranslations(
        PropertyMap(), createGeocentric(), createGeocentricKM(), 1.0, 2.0, 3.0,
        std::vector<PositionalAccuracyNNPtr>());

    EXPECT_EQ(transf1->exportToPROJString(PROJStringFormatter::create()),
              "+proj=pipeline +step +proj=helmert +x=1 +y=2 +z=3 +step "
              "+proj=unitconvert +xy_in=m +z_in=m +xy_out=km +z_out=km");

    auto transf2 = Transformation::createGeocentricTranslations(
        PropertyMap(), createGeocentricKM(), createGeocentric(), 1.0, 2.0, 3.0,
        std::vector<PositionalAccuracyNNPtr>());

    EXPECT_EQ(transf2->exportToPROJString(PROJStringFormatter::create()),
              "+proj=pipeline +step +inv +proj=unitconvert +xy_in=m +z_in=m "
              "+xy_out=km +z_out=km +step +proj=helmert +x=1 +y=2 +z=3");
}

// ---------------------------------------------------------------------------

TEST(operation, transformation_createPositionVector) {

    auto transf = Transformation::createPositionVector(
        PropertyMap(), GeographicCRS::EPSG_4326, GeographicCRS::EPSG_4269, 1.0,
        2.0, 3.0, 4.0, 5.0, 6.0, 7.0, std::vector<PositionalAccuracyNNPtr>());

    auto expected = std::vector<double>{1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0};
    EXPECT_EQ(transf->getTOWGS84Parameters(), expected);

    EXPECT_EQ(transf->exportToPROJString(PROJStringFormatter::create()),
              "+proj=pipeline +step +proj=axisswap +order=2,1 +step "
              "+proj=unitconvert +xy_in=deg +xy_out=rad +step +proj=cart "
              "+ellps=WGS84 +step +proj=helmert +x=1 +y=2 +z=3 +rx=4 +ry=5 "
              "+rz=6 +s=7 +step +inv +proj=cart +ellps=GRS80 +step "
              "+proj=unitconvert +xy_in=rad +xy_out=deg +step +proj=axisswap "
              "+order=2,1");

    auto inv_transf = transf->inverse();
    auto inv_transf_as_transf =
        nn_dynamic_pointer_cast<Transformation>(inv_transf);
    ASSERT_TRUE(inv_transf_as_transf != nullptr);

    EXPECT_EQ(*(transf->sourceCRS()->name()->description()),
              *(inv_transf_as_transf->targetCRS()->name()->description()));
    EXPECT_EQ(*(transf->targetCRS()->name()->description()),
              *(inv_transf_as_transf->sourceCRS()->name()->description()));
    auto expected_inv =
        std::vector<double>{-1.0, -2.0, -3.0, -4.0, -5.0, -6.0, 7.0};
    EXPECT_EQ(inv_transf_as_transf->getTOWGS84Parameters(), expected_inv);
}

// ---------------------------------------------------------------------------

TEST(operation, transformation_createCoordinateFrameRotation) {

    auto transf = Transformation::createCoordinateFrameRotation(
        PropertyMap(), GeographicCRS::EPSG_4326, GeographicCRS::EPSG_4269, 1.0,
        2.0, 3.0, -4.0, -5.0, -6.0, 7.0,
        std::vector<PositionalAccuracyNNPtr>());

    auto params = transf->getTOWGS84Parameters();
    auto expected = std::vector<double>{1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0};
    EXPECT_EQ(params, expected);

    EXPECT_EQ(transf->exportToPROJString(PROJStringFormatter::create()),
              "+proj=pipeline +step +proj=axisswap +order=2,1 +step "
              "+proj=unitconvert +xy_in=deg +xy_out=rad +step +proj=cart "
              "+ellps=WGS84 +step +proj=helmert +x=1 +y=2 +z=3 +rx=-4 +ry=-5 "
              "+rz=-6 +s=7 +transpose +step +inv +proj=cart +ellps=GRS80 +step "
              "+proj=unitconvert +xy_in=rad +xy_out=deg +step +proj=axisswap "
              "+order=2,1");

    auto inv_transf = transf->inverse();
    auto inv_transf_as_transf =
        nn_dynamic_pointer_cast<Transformation>(inv_transf);
    ASSERT_TRUE(inv_transf_as_transf != nullptr);

    EXPECT_EQ(*(transf->sourceCRS()->name()->description()),
              *(inv_transf_as_transf->targetCRS()->name()->description()));
    EXPECT_EQ(*(transf->targetCRS()->name()->description()),
              *(inv_transf_as_transf->sourceCRS()->name()->description()));
    auto expected_inv =
        std::vector<double>{-1.0, -2.0, -3.0, -4.0, -5.0, -6.0, 7.0};
    EXPECT_EQ(inv_transf_as_transf->getTOWGS84Parameters(), expected_inv);
}

// ---------------------------------------------------------------------------

TEST(operation, transformation_inverse) {

    auto transf = Transformation::create(
        PropertyMap()
            .set(IdentifiedObject::NAME_KEY, "my transformation")
            .set(Identifier::CODESPACE_KEY, "my codeSpace")
            .set(Identifier::CODE_KEY, "my code"),
        GeographicCRS::EPSG_4326, GeographicCRS::EPSG_4269, nullptr,
        PropertyMap()
            .set(IdentifiedObject::NAME_KEY, "my operation")
            .set(Identifier::CODESPACE_KEY, "my codeSpace")
            .set(Identifier::CODE_KEY, "my code"),
        std::vector<OperationParameterNNPtr>{OperationParameter::create(
            PropertyMap().set(IdentifiedObject::NAME_KEY, "paramName"))},
        std::vector<ParameterValueNNPtr>{
            ParameterValue::createFilename("foo.bin")},
        std::vector<PositionalAccuracyNNPtr>{
            PositionalAccuracy::create("0.1")});
    auto inv = transf->inverse();
    EXPECT_EQ(inv->inverse(), transf);
    EXPECT_EQ(inv->exportToWKT(WKTFormatter::create()),
              "COORDINATEOPERATION[\"Inverse of my transformation\",\n"
              "    SOURCECRS[\n"
              "        GEODCRS[\"NAD83\",\n"
              "            DATUM[\"North American Datum 1983\",\n"
              "                ELLIPSOID[\"GRS 1980\",6378137,298.257222101,\n"
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
              "            ID[\"EPSG\",4269]]],\n"
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
              "    METHOD[\"Inverse of my operation\"],\n"
              "    PARAMETERFILE[\"paramName\",\"foo.bin\"],\n"
              "    OPERATIONACCURACY[0.1]]");

    EXPECT_THROW(inv->exportToPROJString(PROJStringFormatter::create()),
                 FormattingException);
}

// ---------------------------------------------------------------------------

TEST(operation, transformation_createTOWGS84) {

    EXPECT_THROW(Transformation::createTOWGS84(GeographicCRS::EPSG_4326,
                                               std::vector<double>()),
                 InvalidOperation);

    auto crsIn = CompoundCRS::create(PropertyMap(), std::vector<CRSNNPtr>{});
    EXPECT_THROW(
        Transformation::createTOWGS84(crsIn, std::vector<double>(7, 0)),
        InvalidOperation);
}

// ---------------------------------------------------------------------------

TEST(operation, utm_export) {
    auto conv = Conversion::createUTM(PropertyMap(), 1, false);
    EXPECT_EQ(conv->exportToPROJString(PROJStringFormatter::create()),
              "+proj=utm +zone=1 +south");

    EXPECT_EQ(conv->exportToWKT(WKTFormatter::create()),
              "CONVERSION[\"UTM zone 1S\",\n"
              "    METHOD[\"Transverse Mercator\",\n"
              "        ID[\"EPSG\",9807]],\n"
              "    PARAMETER[\"Latitude of natural origin\",0,\n"
              "        ANGLEUNIT[\"degree\",0.0174532925199433],\n"
              "        ID[\"EPSG\",8801]],\n"
              "    PARAMETER[\"Longitude of natural origin\",-177,\n"
              "        ANGLEUNIT[\"degree\",0.0174532925199433],\n"
              "        ID[\"EPSG\",8802]],\n"
              "    PARAMETER[\"Scale factor at natural origin\",0.9996,\n"
              "        SCALEUNIT[\"unity\",1],\n"
              "        ID[\"EPSG\",8805]],\n"
              "    PARAMETER[\"False easting\",500000,\n"
              "        LENGTHUNIT[\"metre\",1],\n"
              "        ID[\"EPSG\",8806]],\n"
              "    PARAMETER[\"False northing\",10000000,\n"
              "        LENGTHUNIT[\"metre\",1],\n"
              "        ID[\"EPSG\",8807]],\n"
              "    ID[\"EPSG\",17001]]");

    EXPECT_EQ(conv->exportToWKT(
                  WKTFormatter::create(WKTFormatter::Convention::WKT1_GDAL)),
              "PROJECTION[\"Transverse_Mercator\"],\n"
              "PARAMETER[\"latitude_of_origin\",0],\n"
              "PARAMETER[\"central_meridian\",-177],\n"
              "PARAMETER[\"scale_factor\",0.9996],\n"
              "PARAMETER[\"false_easting\",500000],\n"
              "PARAMETER[\"false_northing\",10000000]");
}

// ---------------------------------------------------------------------------

TEST(operation, tmerc_export) {
    auto conv = Conversion::createTransverseMercator(
        PropertyMap(), Angle(1), Angle(2), Scale(3), Length(4), Length(5));
    EXPECT_EQ(conv->exportToPROJString(PROJStringFormatter::create()),
              "+proj=tmerc +lat_0=1 +lon_0=2 +k_0=3 +x_0=4 +y_0=5");

    EXPECT_EQ(conv->exportToWKT(WKTFormatter::create()),
              "CONVERSION[\"Transverse Mercator\",\n"
              "    METHOD[\"Transverse Mercator\",\n"
              "        ID[\"EPSG\",9807]],\n"
              "    PARAMETER[\"Latitude of natural origin\",1,\n"
              "        ANGLEUNIT[\"degree\",0.0174532925199433],\n"
              "        ID[\"EPSG\",8801]],\n"
              "    PARAMETER[\"Longitude of natural origin\",2,\n"
              "        ANGLEUNIT[\"degree\",0.0174532925199433],\n"
              "        ID[\"EPSG\",8802]],\n"
              "    PARAMETER[\"Scale factor at natural origin\",3,\n"
              "        SCALEUNIT[\"unity\",1],\n"
              "        ID[\"EPSG\",8805]],\n"
              "    PARAMETER[\"False easting\",4,\n"
              "        LENGTHUNIT[\"metre\",1],\n"
              "        ID[\"EPSG\",8806]],\n"
              "    PARAMETER[\"False northing\",5,\n"
              "        LENGTHUNIT[\"metre\",1],\n"
              "        ID[\"EPSG\",8807]]]");

    EXPECT_EQ(conv->exportToWKT(
                  WKTFormatter::create(WKTFormatter::Convention::WKT1_GDAL)),
              "PROJECTION[\"Transverse_Mercator\"],\n"
              "PARAMETER[\"latitude_of_origin\",1],\n"
              "PARAMETER[\"central_meridian\",2],\n"
              "PARAMETER[\"scale_factor\",3],\n"
              "PARAMETER[\"false_easting\",4],\n"
              "PARAMETER[\"false_northing\",5]");
}

// ---------------------------------------------------------------------------

TEST(operation, gstmerc_export) {
    auto conv = Conversion::createGaussSchreiberTransverseMercator(
        PropertyMap(), Angle(1), Angle(2), Scale(3), Length(4), Length(5));
    EXPECT_EQ(conv->exportToPROJString(PROJStringFormatter::create()),
              "+proj=gstmerc +lat_0=1 +lon_0=2 +k_0=3 +x_0=4 +y_0=5");

    EXPECT_EQ(conv->exportToWKT(WKTFormatter::create()),
              "CONVERSION[\"Gauss Schreiber Transverse Mercator\",\n"
              "    METHOD[\"Gauss Schreiber Transverse Mercator\"],\n"
              "    PARAMETER[\"Latitude of natural origin\",1,\n"
              "        ANGLEUNIT[\"degree\",0.0174532925199433],\n"
              "        ID[\"EPSG\",8801]],\n"
              "    PARAMETER[\"Longitude of natural origin\",2,\n"
              "        ANGLEUNIT[\"degree\",0.0174532925199433],\n"
              "        ID[\"EPSG\",8802]],\n"
              "    PARAMETER[\"Scale factor at natural origin\",3,\n"
              "        SCALEUNIT[\"unity\",1],\n"
              "        ID[\"EPSG\",8805]],\n"
              "    PARAMETER[\"False easting\",4,\n"
              "        LENGTHUNIT[\"metre\",1],\n"
              "        ID[\"EPSG\",8806]],\n"
              "    PARAMETER[\"False northing\",5,\n"
              "        LENGTHUNIT[\"metre\",1],\n"
              "        ID[\"EPSG\",8807]]]");

    EXPECT_EQ(conv->exportToWKT(
                  WKTFormatter::create(WKTFormatter::Convention::WKT1_GDAL)),
              "PROJECTION[\"Gauss_Schreiber_Transverse_Mercator\"],\n"
              "PARAMETER[\"latitude_of_origin\",1],\n"
              "PARAMETER[\"central_meridian\",2],\n"
              "PARAMETER[\"scale_factor\",3],\n"
              "PARAMETER[\"false_easting\",4],\n"
              "PARAMETER[\"false_northing\",5]");
}

// ---------------------------------------------------------------------------

TEST(operation, tmerc_south_oriented_export) {
    auto conv = Conversion::createTransverseMercatorSouthOriented(
        PropertyMap(), Angle(1), Angle(2), Scale(3), Length(4), Length(5));

    EXPECT_EQ(conv->exportToPROJString(PROJStringFormatter::create()),
              "+proj=tmerc +axis=wsu +lat_0=1 +lon_0=2 +k_0=3 +x_0=4 +y_0=5");

    EXPECT_EQ(conv->exportToWKT(WKTFormatter::create()),
              "CONVERSION[\"Transverse Mercator (South Orientated)\",\n"
              "    METHOD[\"Transverse Mercator (South Orientated)\",\n"
              "        ID[\"EPSG\",9808]],\n"
              "    PARAMETER[\"Latitude of natural origin\",1,\n"
              "        ANGLEUNIT[\"degree\",0.0174532925199433],\n"
              "        ID[\"EPSG\",8801]],\n"
              "    PARAMETER[\"Longitude of natural origin\",2,\n"
              "        ANGLEUNIT[\"degree\",0.0174532925199433],\n"
              "        ID[\"EPSG\",8802]],\n"
              "    PARAMETER[\"Scale factor at natural origin\",3,\n"
              "        SCALEUNIT[\"unity\",1],\n"
              "        ID[\"EPSG\",8805]],\n"
              "    PARAMETER[\"False easting\",4,\n"
              "        LENGTHUNIT[\"metre\",1],\n"
              "        ID[\"EPSG\",8806]],\n"
              "    PARAMETER[\"False northing\",5,\n"
              "        LENGTHUNIT[\"metre\",1],\n"
              "        ID[\"EPSG\",8807]]]");

    EXPECT_EQ(conv->exportToWKT(
                  WKTFormatter::create(WKTFormatter::Convention::WKT1_GDAL)),
              "PROJECTION[\"Transverse_Mercator_South_Orientated\"],\n"
              "PARAMETER[\"latitude_of_origin\",1],\n"
              "PARAMETER[\"central_meridian\",2],\n"
              "PARAMETER[\"scale_factor\",3],\n"
              "PARAMETER[\"false_easting\",4],\n"
              "PARAMETER[\"false_northing\",5]");

    auto wkt = "PROJCRS[\"Hartebeesthoek94 / Lo29\","
               "  BASEGEODCRS[\"Hartebeesthoek94\","
               "    DATUM[\"Hartebeesthoek94\","
               "      ELLIPSOID[\"WGS "
               "84\",6378137,298.257223563,LENGTHUNIT[\"metre\",1.0]]]],"
               "  CONVERSION[\"South African Survey Grid zone 29\","
               "    METHOD[\"Transverse Mercator (South "
               "Orientated)\",ID[\"EPSG\",9808]],"
               "    PARAMETER[\"Latitude of natural "
               "origin\",0,ANGLEUNIT[\"degree\",0.01745329252]],"
               "    PARAMETER[\"Longitude of natural "
               "origin\",29,ANGLEUNIT[\"degree\",0.01745329252]],"
               "    PARAMETER[\"Scale factor at natural "
               "origin\",1,SCALEUNIT[\"unity\",1.0]],"
               "    PARAMETER[\"False easting\",0,LENGTHUNIT[\"metre\",1.0]],"
               "    PARAMETER[\"False northing\",0,LENGTHUNIT[\"metre\",1.0]]],"
               "  CS[cartesian,2],"
               "    AXIS[\"westing (Y)\",west,ORDER[1]],"
               "    AXIS[\"southing (X)\",south,ORDER[2]],"
               "    LENGTHUNIT[\"metre\",1.0],"
               "  ID[\"EPSG\",2053]]";
    auto obj = WKTParser().createFromWKT(wkt);
    auto crs = nn_dynamic_pointer_cast<ProjectedCRS>(obj);
    ASSERT_TRUE(crs != nullptr);
    EXPECT_EQ(crs->exportToPROJString(PROJStringFormatter::create()),
              "+proj=pipeline +step +proj=axisswap +order=2,1 +step "
              "+proj=unitconvert +xy_in=deg +xy_out=rad +step +proj=tmerc "
              "+axis=wsu +lat_0=0 +lon_0=29 +k_0=1 +x_0=0 +y_0=0 +ellps=WGS84");
}

// ---------------------------------------------------------------------------

TEST(operation, tped_export) {
    auto conv = Conversion::createTwoPointEquidistant(
        PropertyMap(), Angle(1), Angle(2), Angle(3), Angle(4), Length(5),
        Length(6));
    EXPECT_EQ(conv->exportToPROJString(PROJStringFormatter::create()),
              "+proj=tpeqd +lat_1=1 +lon_1=2 +lat_2=3 +lon_2=4 +x_0=5 +y_0=6");

    auto formatter = WKTFormatter::create();
    formatter->simulCurNodeHasId();
    EXPECT_EQ(conv->exportToWKT(formatter),
              "CONVERSION[\"Two Point Equidistant\",\n"
              "    METHOD[\"Two Point Equidistant\"],\n"
              "    PARAMETER[\"Latitude of first point\",1,\n"
              "        ANGLEUNIT[\"degree\",0.0174532925199433]],\n"
              "    PARAMETER[\"Longitude of first point\",2,\n"
              "        ANGLEUNIT[\"degree\",0.0174532925199433]],\n"
              "    PARAMETER[\"Latitude of second point\",3,\n"
              "        ANGLEUNIT[\"degree\",0.0174532925199433]],\n"
              "    PARAMETER[\"Longitude of second point\",4,\n"
              "        ANGLEUNIT[\"degree\",0.0174532925199433]],\n"
              "    PARAMETER[\"False easting\",5,\n"
              "        LENGTHUNIT[\"metre\",1],\n"
              "        ID[\"EPSG\",8806]],\n"
              "    PARAMETER[\"False northing\",6,\n"
              "        LENGTHUNIT[\"metre\",1],\n"
              "        ID[\"EPSG\",8807]]]");

    EXPECT_EQ(conv->exportToWKT(
                  WKTFormatter::create(WKTFormatter::Convention::WKT1_GDAL)),
              "PROJECTION[\"Two_Point_Equidistant\"],\n"
              "PARAMETER[\"Latitude_Of_1st_Point\",1],\n"
              "PARAMETER[\"Longitude_Of_1st_Point\",2],\n"
              "PARAMETER[\"Latitude_Of_2nd_Point\",3],\n"
              "PARAMETER[\"Longitude_Of_2nd_Point\",4],\n"
              "PARAMETER[\"false_easting\",5],\n"
              "PARAMETER[\"false_northing\",6]");
}

// ---------------------------------------------------------------------------

TEST(operation, tmg_export) {
    auto conv = Conversion::createTunisiaMappingGrid(
        PropertyMap(), Angle(1), Angle(2), Length(3), Length(4));
    EXPECT_THROW(conv->exportToPROJString(PROJStringFormatter::create()),
                 FormattingException);

    EXPECT_EQ(conv->exportToWKT(WKTFormatter::create()),
              "CONVERSION[\"Tunisia Mapping Grid\",\n"
              "    METHOD[\"Tunisia Mapping Grid\",\n"
              "        ID[\"EPSG\",9816]],\n"
              "    PARAMETER[\"Latitude of false origin\",1,\n"
              "        ANGLEUNIT[\"degree\",0.0174532925199433],\n"
              "        ID[\"EPSG\",8821]],\n"
              "    PARAMETER[\"Longitude of false origin\",2,\n"
              "        ANGLEUNIT[\"degree\",0.0174532925199433],\n"
              "        ID[\"EPSG\",8822]],\n"
              "    PARAMETER[\"Easting of false origin\",3,\n"
              "        LENGTHUNIT[\"metre\",1],\n"
              "        ID[\"EPSG\",8826]],\n"
              "    PARAMETER[\"Northing of false origin\",4,\n"
              "        LENGTHUNIT[\"metre\",1],\n"
              "        ID[\"EPSG\",8827]]]");

    EXPECT_EQ(conv->exportToWKT(
                  WKTFormatter::create(WKTFormatter::Convention::WKT1_GDAL)),
              "PROJECTION[\"Tunisia_Mapping_Grid\"],\n"
              "PARAMETER[\"latitude_of_origin\",1],\n"
              "PARAMETER[\"central_meridian\",2],\n"
              "PARAMETER[\"false_easting\",3],\n"
              "PARAMETER[\"false_northing\",4]");
}

// ---------------------------------------------------------------------------

TEST(operation, aea_export) {
    auto conv = Conversion::createAlbersEqualArea(PropertyMap(), Angle(1),
                                                  Angle(2), Angle(3), Angle(4),
                                                  Length(5), Length(6));

    EXPECT_EQ(conv->exportToPROJString(PROJStringFormatter::create()),
              "+proj=aea +lat_0=1 +lon_0=2 +lat_1=3 +lat_2=4 +x_0=5 +y_0=6");

    EXPECT_EQ(conv->exportToWKT(WKTFormatter::create()),
              "CONVERSION[\"Albers Equal Area\",\n"
              "    METHOD[\"Albers Equal Area\",\n"
              "        ID[\"EPSG\",9822]],\n"
              "    PARAMETER[\"Latitude of false origin\",1,\n"
              "        ANGLEUNIT[\"degree\",0.0174532925199433],\n"
              "        ID[\"EPSG\",8821]],\n"
              "    PARAMETER[\"Longitude of false origin\",2,\n"
              "        ANGLEUNIT[\"degree\",0.0174532925199433],\n"
              "        ID[\"EPSG\",8822]],\n"
              "    PARAMETER[\"Latitude of 1st standard parallel\",3,\n"
              "        ANGLEUNIT[\"degree\",0.0174532925199433],\n"
              "        ID[\"EPSG\",8823]],\n"
              "    PARAMETER[\"Latitude of 2nd standard parallel\",4,\n"
              "        ANGLEUNIT[\"degree\",0.0174532925199433],\n"
              "        ID[\"EPSG\",8824]],\n"
              "    PARAMETER[\"Easting of false origin\",5,\n"
              "        LENGTHUNIT[\"metre\",1],\n"
              "        ID[\"EPSG\",8826]],\n"
              "    PARAMETER[\"Northing of false origin\",6,\n"
              "        LENGTHUNIT[\"metre\",1],\n"
              "        ID[\"EPSG\",8827]]]");

    EXPECT_EQ(conv->exportToWKT(
                  WKTFormatter::create(WKTFormatter::Convention::WKT1_GDAL)),
              "PROJECTION[\"Albers_Conic_Equal_Area\"],\n"
              "PARAMETER[\"latitude_of_center\",1],\n"
              "PARAMETER[\"longitude_of_center\",2],\n"
              "PARAMETER[\"standard_parallel_1\",3],\n"
              "PARAMETER[\"standard_parallel_2\",4],\n"
              "PARAMETER[\"false_easting\",5],\n"
              "PARAMETER[\"false_northing\",6]");
}

// ---------------------------------------------------------------------------

TEST(operation, azimuthal_equidistant_export) {
    auto conv = Conversion::createAzimuthalEquidistant(
        PropertyMap(), Angle(1), Angle(2), Length(3), Length(4));

    EXPECT_EQ(conv->exportToPROJString(PROJStringFormatter::create()),
              "+proj=aeqd +lat_0=1 +lon_0=2 +x_0=3 +y_0=4");

    EXPECT_EQ(conv->exportToWKT(WKTFormatter::create()),
              "CONVERSION[\"Modified Azimuthal Equidistant\",\n"
              "    METHOD[\"Modified Azimuthal Equidistant\",\n"
              "        ID[\"EPSG\",9832]],\n"
              "    PARAMETER[\"Latitude of natural origin\",1,\n"
              "        ANGLEUNIT[\"degree\",0.0174532925199433],\n"
              "        ID[\"EPSG\",8801]],\n"
              "    PARAMETER[\"Longitude of natural origin\",2,\n"
              "        ANGLEUNIT[\"degree\",0.0174532925199433],\n"
              "        ID[\"EPSG\",8802]],\n"
              "    PARAMETER[\"False easting\",3,\n"
              "        LENGTHUNIT[\"metre\",1],\n"
              "        ID[\"EPSG\",8806]],\n"
              "    PARAMETER[\"False northing\",4,\n"
              "        LENGTHUNIT[\"metre\",1],\n"
              "        ID[\"EPSG\",8807]]]");

    EXPECT_EQ(conv->exportToWKT(
                  WKTFormatter::create(WKTFormatter::Convention::WKT1_GDAL)),
              "PROJECTION[\"Azimuthal_Equidistant\"],\n"
              "PARAMETER[\"latitude_of_center\",1],\n"
              "PARAMETER[\"longitude_of_center\",2],\n"
              "PARAMETER[\"false_easting\",3],\n"
              "PARAMETER[\"false_northing\",4]");
}

// ---------------------------------------------------------------------------

TEST(operation, guam_projection_export) {
    auto conv = Conversion::createGuamProjection(
        PropertyMap(), Angle(1), Angle(2), Length(3), Length(4));

    EXPECT_EQ(conv->exportToPROJString(PROJStringFormatter::create()),
              "+proj=aeqd +guam +lat_0=1 +lon_0=2 +x_0=3 +y_0=4");

    EXPECT_EQ(conv->exportToWKT(WKTFormatter::create()),
              "CONVERSION[\"Guam Projection\",\n"
              "    METHOD[\"Guam Projection\",\n"
              "        ID[\"EPSG\",9831]],\n"
              "    PARAMETER[\"Latitude of natural origin\",1,\n"
              "        ANGLEUNIT[\"degree\",0.0174532925199433],\n"
              "        ID[\"EPSG\",8801]],\n"
              "    PARAMETER[\"Longitude of natural origin\",2,\n"
              "        ANGLEUNIT[\"degree\",0.0174532925199433],\n"
              "        ID[\"EPSG\",8802]],\n"
              "    PARAMETER[\"False easting\",3,\n"
              "        LENGTHUNIT[\"metre\",1],\n"
              "        ID[\"EPSG\",8806]],\n"
              "    PARAMETER[\"False northing\",4,\n"
              "        LENGTHUNIT[\"metre\",1],\n"
              "        ID[\"EPSG\",8807]]]");

    EXPECT_THROW(conv->exportToWKT(
                     WKTFormatter::create(WKTFormatter::Convention::WKT1_GDAL)),
                 FormattingException);
}

// ---------------------------------------------------------------------------

TEST(operation, bonne_export) {
    auto conv = Conversion::createBonne(PropertyMap(), Angle(1), Angle(2),
                                        Length(3), Length(4));

    EXPECT_EQ(conv->exportToPROJString(PROJStringFormatter::create()),
              "+proj=bonne +lat_1=1 +lon_0=2 +x_0=3 +y_0=4");

    EXPECT_EQ(conv->exportToWKT(WKTFormatter::create()),
              "CONVERSION[\"Bonne\",\n"
              "    METHOD[\"Bonne\",\n"
              "        ID[\"EPSG\",9827]],\n"
              "    PARAMETER[\"Latitude of natural origin\",1,\n"
              "        ANGLEUNIT[\"degree\",0.0174532925199433],\n"
              "        ID[\"EPSG\",8801]],\n"
              "    PARAMETER[\"Longitude of natural origin\",2,\n"
              "        ANGLEUNIT[\"degree\",0.0174532925199433],\n"
              "        ID[\"EPSG\",8802]],\n"
              "    PARAMETER[\"False easting\",3,\n"
              "        LENGTHUNIT[\"metre\",1],\n"
              "        ID[\"EPSG\",8806]],\n"
              "    PARAMETER[\"False northing\",4,\n"
              "        LENGTHUNIT[\"metre\",1],\n"
              "        ID[\"EPSG\",8807]]]");

    EXPECT_EQ(conv->exportToWKT(
                  WKTFormatter::create(WKTFormatter::Convention::WKT1_GDAL)),
              "PROJECTION[\"Bonne\"],\n"
              "PARAMETER[\"standard_parallel_1\",1],\n"
              "PARAMETER[\"central_meridian\",2],\n"
              "PARAMETER[\"false_easting\",3],\n"
              "PARAMETER[\"false_northing\",4]");

    auto obj = WKTParser().createFromWKT(
        "PROJCS[\"unnamed\","
        "GEOGCS[\"unnamed ellipse\","
        "    DATUM[\"unknown\","
        "        SPHEROID[\"unnamed\",6378137,298.257223563]],"
        "    PRIMEM[\"Greenwich\",0],"
        "    UNIT[\"degree\",0.0174532925199433]],"
        "PROJECTION[\"Bonne\"],"
        "PARAMETER[\"standard_parallel_1\",1],"
        "PARAMETER[\"central_meridian\",2],"
        "PARAMETER[\"false_easting\",3],"
        "PARAMETER[\"false_northing\",4],"
        "UNIT[\"metre\",1]]");
    auto crs = nn_dynamic_pointer_cast<ProjectedCRS>(obj);
    ASSERT_TRUE(crs != nullptr);
    EXPECT_EQ(crs->exportToPROJString(PROJStringFormatter::create()),
              "+proj=pipeline +step +proj=unitconvert +xy_in=deg +xy_out=rad "
              "+step +proj=bonne +lat_1=1 +lon_0=2 +x_0=3 +y_0=4 +ellps=WGS84");
}

// ---------------------------------------------------------------------------

TEST(operation, lambert_cylindrical_equal_area_spherical_export) {
    auto conv = Conversion::createLambertCylindricalEqualAreaSpherical(
        PropertyMap(), Angle(1), Angle(2), Length(3), Length(4));

    EXPECT_EQ(conv->exportToPROJString(PROJStringFormatter::create()),
              "+proj=cea +lat_ts=1 +lon_0=2 +x_0=3 +y_0=4");

    EXPECT_EQ(conv->exportToWKT(WKTFormatter::create()),
              "CONVERSION[\"Lambert Cylindrical Equal Area (Spherical)\",\n"
              "    METHOD[\"Lambert Cylindrical Equal Area (Spherical)\",\n"
              "        ID[\"EPSG\",9834]],\n"
              "    PARAMETER[\"Latitude of 1st standard parallel\",1,\n"
              "        ANGLEUNIT[\"degree\",0.0174532925199433],\n"
              "        ID[\"EPSG\",8823]],\n"
              "    PARAMETER[\"Longitude of natural origin\",2,\n"
              "        ANGLEUNIT[\"degree\",0.0174532925199433],\n"
              "        ID[\"EPSG\",8802]],\n"
              "    PARAMETER[\"False easting\",3,\n"
              "        LENGTHUNIT[\"metre\",1],\n"
              "        ID[\"EPSG\",8806]],\n"
              "    PARAMETER[\"False northing\",4,\n"
              "        LENGTHUNIT[\"metre\",1],\n"
              "        ID[\"EPSG\",8807]]]");

    EXPECT_EQ(conv->exportToWKT(
                  WKTFormatter::create(WKTFormatter::Convention::WKT1_GDAL)),
              "PROJECTION[\"Cylindrical_Equal_Area\"],\n"
              "PARAMETER[\"standard_parallel_1\",1],\n"
              "PARAMETER[\"central_meridian\",2],\n"
              "PARAMETER[\"false_easting\",3],\n"
              "PARAMETER[\"false_northing\",4]");
}

// ---------------------------------------------------------------------------

TEST(operation, lambert_cylindrical_equal_area_export) {
    auto conv = Conversion::createLambertCylindricalEqualArea(
        PropertyMap(), Angle(1), Angle(2), Length(3), Length(4));

    EXPECT_EQ(conv->exportToPROJString(PROJStringFormatter::create()),
              "+proj=cea +lat_ts=1 +lon_0=2 +x_0=3 +y_0=4");

    EXPECT_EQ(conv->exportToWKT(WKTFormatter::create()),
              "CONVERSION[\"Lambert Cylindrical Equal Area\",\n"
              "    METHOD[\"Lambert Cylindrical Equal Area\",\n"
              "        ID[\"EPSG\",9835]],\n"
              "    PARAMETER[\"Latitude of 1st standard parallel\",1,\n"
              "        ANGLEUNIT[\"degree\",0.0174532925199433],\n"
              "        ID[\"EPSG\",8823]],\n"
              "    PARAMETER[\"Longitude of natural origin\",2,\n"
              "        ANGLEUNIT[\"degree\",0.0174532925199433],\n"
              "        ID[\"EPSG\",8802]],\n"
              "    PARAMETER[\"False easting\",3,\n"
              "        LENGTHUNIT[\"metre\",1],\n"
              "        ID[\"EPSG\",8806]],\n"
              "    PARAMETER[\"False northing\",4,\n"
              "        LENGTHUNIT[\"metre\",1],\n"
              "        ID[\"EPSG\",8807]]]");

    EXPECT_EQ(conv->exportToWKT(
                  WKTFormatter::create(WKTFormatter::Convention::WKT1_GDAL)),
              "PROJECTION[\"Cylindrical_Equal_Area\"],\n"
              "PARAMETER[\"standard_parallel_1\",1],\n"
              "PARAMETER[\"central_meridian\",2],\n"
              "PARAMETER[\"false_easting\",3],\n"
              "PARAMETER[\"false_northing\",4]");
}

// ---------------------------------------------------------------------------

TEST(operation, lcc1sp_export) {
    auto conv = Conversion::createLambertConicConformal_1SP(
        PropertyMap(), Angle(1), Angle(2), Scale(3), Length(4), Length(5));
    EXPECT_EQ(conv->exportToPROJString(PROJStringFormatter::create()),
              "+proj=lcc +lat_1=1 +lat_0=1 +lon_0=2 +k_0=3 +x_0=4 +y_0=5");

    EXPECT_EQ(conv->exportToWKT(WKTFormatter::create()),
              "CONVERSION[\"Lambert Conic Conformal (1SP)\",\n"
              "    METHOD[\"Lambert Conic Conformal (1SP)\",\n"
              "        ID[\"EPSG\",9801]],\n"
              "    PARAMETER[\"Latitude of natural origin\",1,\n"
              "        ANGLEUNIT[\"degree\",0.0174532925199433],\n"
              "        ID[\"EPSG\",8801]],\n"
              "    PARAMETER[\"Longitude of natural origin\",2,\n"
              "        ANGLEUNIT[\"degree\",0.0174532925199433],\n"
              "        ID[\"EPSG\",8802]],\n"
              "    PARAMETER[\"Scale factor at natural origin\",3,\n"
              "        SCALEUNIT[\"unity\",1],\n"
              "        ID[\"EPSG\",8805]],\n"
              "    PARAMETER[\"False easting\",4,\n"
              "        LENGTHUNIT[\"metre\",1],\n"
              "        ID[\"EPSG\",8806]],\n"
              "    PARAMETER[\"False northing\",5,\n"
              "        LENGTHUNIT[\"metre\",1],\n"
              "        ID[\"EPSG\",8807]]]");

    EXPECT_EQ(conv->exportToWKT(
                  WKTFormatter::create(WKTFormatter::Convention::WKT1_GDAL)),
              "PROJECTION[\"Lambert_Conformal_Conic_1SP\"],\n"
              "PARAMETER[\"latitude_of_origin\",1],\n"
              "PARAMETER[\"central_meridian\",2],\n"
              "PARAMETER[\"scale_factor\",3],\n"
              "PARAMETER[\"false_easting\",4],\n"
              "PARAMETER[\"false_northing\",5]");
}

// ---------------------------------------------------------------------------

TEST(operation, lcc2sp_export) {
    auto conv = Conversion::createLambertConicConformal_2SP(
        PropertyMap(), Angle(1), Angle(2), Angle(3), Angle(4), Length(5),
        Length(6));
    EXPECT_EQ(conv->exportToPROJString(PROJStringFormatter::create()),
              "+proj=lcc +lat_0=1 +lon_0=2 +lat_1=3 +lat_2=4 +x_0=5 +y_0=6");

    EXPECT_EQ(conv->exportToWKT(WKTFormatter::create()),
              "CONVERSION[\"Lambert Conic Conformal (2SP)\",\n"
              "    METHOD[\"Lambert Conic Conformal (2SP)\",\n"
              "        ID[\"EPSG\",9802]],\n"
              "    PARAMETER[\"Latitude of false origin\",1,\n"
              "        ANGLEUNIT[\"degree\",0.0174532925199433],\n"
              "        ID[\"EPSG\",8821]],\n"
              "    PARAMETER[\"Longitude of false origin\",2,\n"
              "        ANGLEUNIT[\"degree\",0.0174532925199433],\n"
              "        ID[\"EPSG\",8822]],\n"
              "    PARAMETER[\"Latitude of 1st standard parallel\",3,\n"
              "        ANGLEUNIT[\"degree\",0.0174532925199433],\n"
              "        ID[\"EPSG\",8823]],\n"
              "    PARAMETER[\"Latitude of 2nd standard parallel\",4,\n"
              "        ANGLEUNIT[\"degree\",0.0174532925199433],\n"
              "        ID[\"EPSG\",8824]],\n"
              "    PARAMETER[\"Easting of false origin\",5,\n"
              "        LENGTHUNIT[\"metre\",1],\n"
              "        ID[\"EPSG\",8826]],\n"
              "    PARAMETER[\"Northing of false origin\",6,\n"
              "        LENGTHUNIT[\"metre\",1],\n"
              "        ID[\"EPSG\",8827]]]");

    EXPECT_EQ(conv->exportToWKT(
                  WKTFormatter::create(WKTFormatter::Convention::WKT1_GDAL)),
              "PROJECTION[\"Lambert_Conformal_Conic_2SP\"],\n"
              "PARAMETER[\"latitude_of_origin\",1],\n"
              "PARAMETER[\"central_meridian\",2],\n"
              "PARAMETER[\"standard_parallel_1\",3],\n"
              "PARAMETER[\"standard_parallel_2\",4],\n"
              "PARAMETER[\"false_easting\",5],\n"
              "PARAMETER[\"false_northing\",6]");
}

// ---------------------------------------------------------------------------

TEST(operation, lcc2sp_belgium_export) {
    auto conv = Conversion::createLambertConicConformal_2SP_Belgium(
        PropertyMap(), Angle(1), Angle(2), Angle(3), Angle(4), Length(5),
        Length(6));
    EXPECT_EQ(conv->exportToPROJString(PROJStringFormatter::create()),
              "+proj=lcc +lat_0=1 +lon_0=2 +lat_1=3 +lat_2=4 +x_0=5 +y_0=6");

    EXPECT_EQ(conv->exportToWKT(WKTFormatter::create()),
              "CONVERSION[\"Lambert Conic Conformal (2SP Belgium)\",\n"
              "    METHOD[\"Lambert Conic Conformal (2SP Belgium)\",\n"
              "        ID[\"EPSG\",9803]],\n"
              "    PARAMETER[\"Latitude of false origin\",1,\n"
              "        ANGLEUNIT[\"degree\",0.0174532925199433],\n"
              "        ID[\"EPSG\",8821]],\n"
              "    PARAMETER[\"Longitude of false origin\",2,\n"
              "        ANGLEUNIT[\"degree\",0.0174532925199433],\n"
              "        ID[\"EPSG\",8822]],\n"
              "    PARAMETER[\"Latitude of 1st standard parallel\",3,\n"
              "        ANGLEUNIT[\"degree\",0.0174532925199433],\n"
              "        ID[\"EPSG\",8823]],\n"
              "    PARAMETER[\"Latitude of 2nd standard parallel\",4,\n"
              "        ANGLEUNIT[\"degree\",0.0174532925199433],\n"
              "        ID[\"EPSG\",8824]],\n"
              "    PARAMETER[\"Easting of false origin\",5,\n"
              "        LENGTHUNIT[\"metre\",1],\n"
              "        ID[\"EPSG\",8826]],\n"
              "    PARAMETER[\"Northing of false origin\",6,\n"
              "        LENGTHUNIT[\"metre\",1],\n"
              "        ID[\"EPSG\",8827]]]");

    EXPECT_EQ(conv->exportToWKT(
                  WKTFormatter::create(WKTFormatter::Convention::WKT1_GDAL)),
              "PROJECTION[\"Lambert_Conformal_Conic_2SP_Belgium\"],\n"
              "PARAMETER[\"latitude_of_origin\",1],\n"
              "PARAMETER[\"central_meridian\",2],\n"
              "PARAMETER[\"standard_parallel_1\",3],\n"
              "PARAMETER[\"standard_parallel_2\",4],\n"
              "PARAMETER[\"false_easting\",5],\n"
              "PARAMETER[\"false_northing\",6]");
}

// ---------------------------------------------------------------------------

TEST(operation, cassini_soldner_export) {
    auto conv = Conversion::createCassiniSoldner(
        PropertyMap(), Angle(1), Angle(2), Length(4), Length(5));

    EXPECT_EQ(conv->exportToPROJString(PROJStringFormatter::create()),
              "+proj=cass +lat_0=1 +lon_0=2 +x_0=4 +y_0=5");

    EXPECT_EQ(conv->exportToWKT(WKTFormatter::create()),
              "CONVERSION[\"Cassini-Soldner\",\n"
              "    METHOD[\"Cassini-Soldner\",\n"
              "        ID[\"EPSG\",9806]],\n"
              "    PARAMETER[\"Latitude of natural origin\",1,\n"
              "        ANGLEUNIT[\"degree\",0.0174532925199433],\n"
              "        ID[\"EPSG\",8801]],\n"
              "    PARAMETER[\"Longitude of natural origin\",2,\n"
              "        ANGLEUNIT[\"degree\",0.0174532925199433],\n"
              "        ID[\"EPSG\",8802]],\n"
              "    PARAMETER[\"False easting\",4,\n"
              "        LENGTHUNIT[\"metre\",1],\n"
              "        ID[\"EPSG\",8806]],\n"
              "    PARAMETER[\"False northing\",5,\n"
              "        LENGTHUNIT[\"metre\",1],\n"
              "        ID[\"EPSG\",8807]]]");

    EXPECT_EQ(conv->exportToWKT(
                  WKTFormatter::create(WKTFormatter::Convention::WKT1_GDAL)),
              "PROJECTION[\"Cassini_Soldner\"],\n"
              "PARAMETER[\"latitude_of_origin\",1],\n"
              "PARAMETER[\"central_meridian\",2],\n"
              "PARAMETER[\"false_easting\",4],\n"
              "PARAMETER[\"false_northing\",5]");
}

// ---------------------------------------------------------------------------

TEST(operation, equidistant_conic_export) {
    auto conv = Conversion::createEquidistantConic(PropertyMap(), Angle(1),
                                                   Angle(2), Angle(3), Angle(4),
                                                   Length(5), Length(6));

    EXPECT_EQ(conv->exportToPROJString(PROJStringFormatter::create()),
              "+proj=eqdc +lat_0=1 +lon_0=2 +lat_1=3 +lat_2=4 +x_0=5 +y_0=6");

    EXPECT_EQ(conv->exportToWKT(WKTFormatter::create()),
              "CONVERSION[\"Equidistant Conic\",\n"
              "    METHOD[\"Equidistant Conic\"],\n"
              "    PARAMETER[\"Latitude of natural origin\",1,\n"
              "        ANGLEUNIT[\"degree\",0.0174532925199433],\n"
              "        ID[\"EPSG\",8801]],\n"
              "    PARAMETER[\"Longitude of natural origin\",2,\n"
              "        ANGLEUNIT[\"degree\",0.0174532925199433],\n"
              "        ID[\"EPSG\",8802]],\n"
              "    PARAMETER[\"Latitude of 1st standard parallel\",3,\n"
              "        ANGLEUNIT[\"degree\",0.0174532925199433],\n"
              "        ID[\"EPSG\",8823]],\n"
              "    PARAMETER[\"Latitude of 2nd standard parallel\",4,\n"
              "        ANGLEUNIT[\"degree\",0.0174532925199433],\n"
              "        ID[\"EPSG\",8824]],\n"
              "    PARAMETER[\"False easting\",5,\n"
              "        LENGTHUNIT[\"metre\",1],\n"
              "        ID[\"EPSG\",8806]],\n"
              "    PARAMETER[\"False northing\",6,\n"
              "        LENGTHUNIT[\"metre\",1],\n"
              "        ID[\"EPSG\",8807]]]");

    EXPECT_EQ(conv->exportToWKT(
                  WKTFormatter::create(WKTFormatter::Convention::WKT1_GDAL)),
              "PROJECTION[\"Equidistant_Conic\"],\n"
              "PARAMETER[\"latitude_of_center\",1],\n"
              "PARAMETER[\"longitude_of_center\",2],\n"
              "PARAMETER[\"standard_parallel_1\",3],\n"
              "PARAMETER[\"standard_parallel_2\",4],\n"
              "PARAMETER[\"false_easting\",5],\n"
              "PARAMETER[\"false_northing\",6]");
}

// ---------------------------------------------------------------------------

TEST(operation, eckert_export) {

    std::vector<std::string> numbers{"", "1", "2", "3", "4", "5", "6"};
    std::vector<std::string> latinNumbers{"",   "I", "II", "III",
                                          "IV", "V", "VI"};

    for (int i = 1; i <= 6; i++) {
        auto conv =
            (i == 1)
                ? Conversion::createEckertI(PropertyMap(), Angle(1), Length(2),
                                            Length(3))
                : (i == 2)
                      ? Conversion::createEckertII(PropertyMap(), Angle(1),
                                                   Length(2), Length(3))
                      : (i == 3)
                            ? Conversion::createEckertIII(
                                  PropertyMap(), Angle(1), Length(2), Length(3))
                            : (i == 4) ? Conversion::createEckertIV(
                                             PropertyMap(), Angle(1), Length(2),
                                             Length(3))
                                       : (i == 5) ? Conversion::createEckertV(
                                                        PropertyMap(), Angle(1),
                                                        Length(2), Length(3))
                                                  :

                                                  Conversion::createEckertVI(
                                                      PropertyMap(), Angle(1),
                                                      Length(2), Length(3));

        EXPECT_EQ(conv->exportToPROJString(PROJStringFormatter::create()),
                  "+proj=eck" + numbers[i] + " +lon_0=1 +x_0=2 +y_0=3");

        EXPECT_EQ(conv->exportToWKT(WKTFormatter::create()),
                  "CONVERSION[\"Eckert " + latinNumbers[i] +
                      "\",\n"
                      "    METHOD[\"Eckert " +
                      latinNumbers[i] +
                      "\"],\n"
                      "    PARAMETER[\"Longitude of natural origin\",1,\n"
                      "        ANGLEUNIT[\"degree\",0.0174532925199433],\n"
                      "        ID[\"EPSG\",8802]],\n"
                      "    PARAMETER[\"False easting\",2,\n"
                      "        LENGTHUNIT[\"metre\",1],\n"
                      "        ID[\"EPSG\",8806]],\n"
                      "    PARAMETER[\"False northing\",3,\n"
                      "        LENGTHUNIT[\"metre\",1],\n"
                      "        ID[\"EPSG\",8807]]]");

        EXPECT_EQ(conv->exportToWKT(WKTFormatter::create(
                      WKTFormatter::Convention::WKT1_GDAL)),
                  "PROJECTION[\"Eckert_" + latinNumbers[i] +
                      "\"],\n"
                      "PARAMETER[\"central_meridian\",1],\n"
                      "PARAMETER[\"false_easting\",2],\n"
                      "PARAMETER[\"false_northing\",3]");
    }
}

// ---------------------------------------------------------------------------

TEST(operation, createEquidistantCylindricalSpherical) {
    auto conv = Conversion::createEquidistantCylindricalSpherical(
        PropertyMap(), Angle(1), Angle(2), Length(3), Length(4));

    EXPECT_EQ(conv->exportToPROJString(PROJStringFormatter::create()),
              "+proj=eqc +lat_ts=1 +lon_0=2 +x_0=3 +y_0=4");

    EXPECT_EQ(conv->exportToWKT(WKTFormatter::create()),
              "CONVERSION[\"Equidistant Cylindrical (Spherical)\",\n"
              "    METHOD[\"Equidistant Cylindrical (Spherical)\",\n"
              "        ID[\"EPSG\",1019]],\n"
              "    PARAMETER[\"Latitude of 1st standard parallel\",1,\n"
              "        ANGLEUNIT[\"degree\",0.0174532925199433],\n"
              "        ID[\"EPSG\",8823]],\n"
              "    PARAMETER[\"Longitude of natural origin\",2,\n"
              "        ANGLEUNIT[\"degree\",0.0174532925199433],\n"
              "        ID[\"EPSG\",8802]],\n"
              "    PARAMETER[\"False easting\",3,\n"
              "        LENGTHUNIT[\"metre\",1],\n"
              "        ID[\"EPSG\",8806]],\n"
              "    PARAMETER[\"False northing\",4,\n"
              "        LENGTHUNIT[\"metre\",1],\n"
              "        ID[\"EPSG\",8807]]]");

    EXPECT_EQ(conv->exportToWKT(
                  WKTFormatter::create(WKTFormatter::Convention::WKT1_GDAL)),
              "PROJECTION[\"Equirectangular\"],\n"
              "PARAMETER[\"standard_parallel_1\",1],\n"
              "PARAMETER[\"central_meridian\",2],\n"
              "PARAMETER[\"false_easting\",3],\n"
              "PARAMETER[\"false_northing\",4]");
}

// ---------------------------------------------------------------------------

TEST(operation, gall_export) {

    auto conv =
        Conversion::createGall(PropertyMap(), Angle(1), Length(2), Length(3));

    EXPECT_EQ(conv->exportToPROJString(PROJStringFormatter::create()),
              "+proj=gall +lon_0=1 +x_0=2 +y_0=3");

    EXPECT_EQ(conv->exportToWKT(WKTFormatter::create()),
              "CONVERSION[\"Gall\",\n"
              "    METHOD[\"Gall\"],\n"
              "    PARAMETER[\"Longitude of natural origin\",1,\n"
              "        ANGLEUNIT[\"degree\",0.0174532925199433],\n"
              "        ID[\"EPSG\",8802]],\n"
              "    PARAMETER[\"False easting\",2,\n"
              "        LENGTHUNIT[\"metre\",1],\n"
              "        ID[\"EPSG\",8806]],\n"
              "    PARAMETER[\"False northing\",3,\n"
              "        LENGTHUNIT[\"metre\",1],\n"
              "        ID[\"EPSG\",8807]]]");

    EXPECT_EQ(conv->exportToWKT(
                  WKTFormatter::create(WKTFormatter::Convention::WKT1_GDAL)),
              "PROJECTION[\"Gall_Stereographic\"],\n"
              "PARAMETER[\"central_meridian\",1],\n"
              "PARAMETER[\"false_easting\",2],\n"
              "PARAMETER[\"false_northing\",3]");
}

// ---------------------------------------------------------------------------

TEST(operation, goode_homolosine_export) {

    auto conv = Conversion::createGoodeHomolosine(PropertyMap(), Angle(1),
                                                  Length(2), Length(3));

    EXPECT_EQ(conv->exportToPROJString(PROJStringFormatter::create()),
              "+proj=goode +lon_0=1 +x_0=2 +y_0=3");

    EXPECT_EQ(conv->exportToWKT(WKTFormatter::create()),
              "CONVERSION[\"Goode Homolosine\",\n"
              "    METHOD[\"Goode Homolosine\"],\n"
              "    PARAMETER[\"Longitude of natural origin\",1,\n"
              "        ANGLEUNIT[\"degree\",0.0174532925199433],\n"
              "        ID[\"EPSG\",8802]],\n"
              "    PARAMETER[\"False easting\",2,\n"
              "        LENGTHUNIT[\"metre\",1],\n"
              "        ID[\"EPSG\",8806]],\n"
              "    PARAMETER[\"False northing\",3,\n"
              "        LENGTHUNIT[\"metre\",1],\n"
              "        ID[\"EPSG\",8807]]]");

    EXPECT_EQ(conv->exportToWKT(
                  WKTFormatter::create(WKTFormatter::Convention::WKT1_GDAL)),
              "PROJECTION[\"Goode_Homolosine\"],\n"
              "PARAMETER[\"central_meridian\",1],\n"
              "PARAMETER[\"false_easting\",2],\n"
              "PARAMETER[\"false_northing\",3]");
}

// ---------------------------------------------------------------------------

TEST(operation, interrupted_goode_homolosine_export) {

    auto conv = Conversion::createInterruptedGoodeHomolosine(
        PropertyMap(), Angle(1), Length(2), Length(3));

    EXPECT_EQ(conv->exportToPROJString(PROJStringFormatter::create()),
              "+proj=igh +lon_0=1 +x_0=2 +y_0=3");

    EXPECT_EQ(conv->exportToWKT(WKTFormatter::create()),
              "CONVERSION[\"Interrupted Goode Homolosine\",\n"
              "    METHOD[\"Interrupted Goode Homolosine\"],\n"
              "    PARAMETER[\"Longitude of natural origin\",1,\n"
              "        ANGLEUNIT[\"degree\",0.0174532925199433],\n"
              "        ID[\"EPSG\",8802]],\n"
              "    PARAMETER[\"False easting\",2,\n"
              "        LENGTHUNIT[\"metre\",1],\n"
              "        ID[\"EPSG\",8806]],\n"
              "    PARAMETER[\"False northing\",3,\n"
              "        LENGTHUNIT[\"metre\",1],\n"
              "        ID[\"EPSG\",8807]]]");

    EXPECT_EQ(conv->exportToWKT(
                  WKTFormatter::create(WKTFormatter::Convention::WKT1_GDAL)),
              "PROJECTION[\"Interrupted_Goode_Homolosine\"],\n"
              "PARAMETER[\"central_meridian\",1],\n"
              "PARAMETER[\"false_easting\",2],\n"
              "PARAMETER[\"false_northing\",3]");
}

// ---------------------------------------------------------------------------

TEST(operation, geostationary_satellite_sweep_x_export) {

    auto conv = Conversion::createGeostationarySatelliteSweepX(
        PropertyMap(), Angle(1), Length(2), Length(3), Length(4));

    EXPECT_EQ(conv->exportToPROJString(PROJStringFormatter::create()),
              "+proj=geos +sweep=x +lon_0=1 +h=2 +x_0=3 +y_0=4");

    EXPECT_EQ(conv->exportToWKT(WKTFormatter::create()),
              "CONVERSION[\"Geostationary Satellite (Sweep X)\",\n"
              "    METHOD[\"Geostationary Satellite (Sweep X)\"],\n"
              "    PARAMETER[\"Longitude of natural origin\",1,\n"
              "        ANGLEUNIT[\"degree\",0.0174532925199433],\n"
              "        ID[\"EPSG\",8802]],\n"
              "    PARAMETER[\"Satellite Height\",2,\n"
              "        LENGTHUNIT[\"metre\",1,\n"
              "            ID[\"EPSG\",9001]]],\n"
              "    PARAMETER[\"False easting\",3,\n"
              "        LENGTHUNIT[\"metre\",1],\n"
              "        ID[\"EPSG\",8806]],\n"
              "    PARAMETER[\"False northing\",4,\n"
              "        LENGTHUNIT[\"metre\",1],\n"
              "        ID[\"EPSG\",8807]]]");

    EXPECT_THROW(conv->exportToWKT(
                     WKTFormatter::create(WKTFormatter::Convention::WKT1_GDAL)),
                 FormattingException);
}

// ---------------------------------------------------------------------------

TEST(operation, geostationary_satellite_sweep_y_export) {

    auto conv = Conversion::createGeostationarySatelliteSweepY(
        PropertyMap(), Angle(1), Length(2), Length(3), Length(4));

    EXPECT_EQ(conv->exportToPROJString(PROJStringFormatter::create()),
              "+proj=geos +lon_0=1 +h=2 +x_0=3 +y_0=4");

    EXPECT_EQ(conv->exportToWKT(WKTFormatter::create()),
              "CONVERSION[\"Geostationary Satellite (Sweep Y)\",\n"
              "    METHOD[\"Geostationary Satellite (Sweep Y)\"],\n"
              "    PARAMETER[\"Longitude of natural origin\",1,\n"
              "        ANGLEUNIT[\"degree\",0.0174532925199433],\n"
              "        ID[\"EPSG\",8802]],\n"
              "    PARAMETER[\"Satellite Height\",2,\n"
              "        LENGTHUNIT[\"metre\",1,\n"
              "            ID[\"EPSG\",9001]]],\n"
              "    PARAMETER[\"False easting\",3,\n"
              "        LENGTHUNIT[\"metre\",1],\n"
              "        ID[\"EPSG\",8806]],\n"
              "    PARAMETER[\"False northing\",4,\n"
              "        LENGTHUNIT[\"metre\",1],\n"
              "        ID[\"EPSG\",8807]]]");

    EXPECT_EQ(conv->exportToWKT(
                  WKTFormatter::create(WKTFormatter::Convention::WKT1_GDAL)),
              "PROJECTION[\"Geostationary_Satellite\"],\n"
              "PARAMETER[\"central_meridian\",1],\n"
              "PARAMETER[\"satellite_height\",2],\n"
              "PARAMETER[\"false_easting\",3],\n"
              "PARAMETER[\"false_northing\",4]");
}

// ---------------------------------------------------------------------------

TEST(operation, gnomonic_export) {
    auto conv = Conversion::createGnomonic(PropertyMap(), Angle(1), Angle(2),
                                           Length(4), Length(5));
    EXPECT_EQ(conv->exportToPROJString(PROJStringFormatter::create()),
              "+proj=gnom +lat_0=1 +lon_0=2 +x_0=4 +y_0=5");

    EXPECT_EQ(conv->exportToWKT(WKTFormatter::create()),
              "CONVERSION[\"Gnomonic\",\n"
              "    METHOD[\"Gnomonic\"],\n"
              "    PARAMETER[\"Latitude of natural origin\",1,\n"
              "        ANGLEUNIT[\"degree\",0.0174532925199433],\n"
              "        ID[\"EPSG\",8801]],\n"
              "    PARAMETER[\"Longitude of natural origin\",2,\n"
              "        ANGLEUNIT[\"degree\",0.0174532925199433],\n"
              "        ID[\"EPSG\",8802]],\n"
              "    PARAMETER[\"False easting\",4,\n"
              "        LENGTHUNIT[\"metre\",1],\n"
              "        ID[\"EPSG\",8806]],\n"
              "    PARAMETER[\"False northing\",5,\n"
              "        LENGTHUNIT[\"metre\",1],\n"
              "        ID[\"EPSG\",8807]]]");

    EXPECT_EQ(conv->exportToWKT(
                  WKTFormatter::create(WKTFormatter::Convention::WKT1_GDAL)),
              "PROJECTION[\"Gnomonic\"],\n"
              "PARAMETER[\"latitude_of_origin\",1],\n"
              "PARAMETER[\"central_meridian\",2],\n"
              "PARAMETER[\"false_easting\",4],\n"
              "PARAMETER[\"false_northing\",5]");
}

// ---------------------------------------------------------------------------

TEST(operation, hotine_oblique_mercator_variant_A_export) {
    auto conv = Conversion::createHotineObliqueMercatorVariantA(
        PropertyMap(), Angle(1), Angle(2), Angle(3), Angle(4), Scale(5),
        Length(6), Length(7));
    EXPECT_EQ(conv->exportToPROJString(PROJStringFormatter::create()),
              "+proj=omerc +no_uoff +lat_0=1 +lonc=2 +alpha=3 +gamma=4 +k=5 "
              "+x_0=6 +y_0=7");

    EXPECT_EQ(conv->exportToWKT(WKTFormatter::create()),
              "CONVERSION[\"Hotine Oblique Mercator (variant A)\",\n"
              "    METHOD[\"Hotine Oblique Mercator (variant A)\",\n"
              "        ID[\"EPSG\",9812]],\n"
              "    PARAMETER[\"Latitude of projection centre\",1,\n"
              "        ANGLEUNIT[\"degree\",0.0174532925199433],\n"
              "        ID[\"EPSG\",8811]],\n"
              "    PARAMETER[\"Longitude of projection centre\",2,\n"
              "        ANGLEUNIT[\"degree\",0.0174532925199433],\n"
              "        ID[\"EPSG\",8812]],\n"
              "    PARAMETER[\"Azimuth of initial line\",3,\n"
              "        ANGLEUNIT[\"degree\",0.0174532925199433],\n"
              "        ID[\"EPSG\",8813]],\n"
              "    PARAMETER[\"Angle from Rectified to Skew Grid\",4,\n"
              "        ANGLEUNIT[\"degree\",0.0174532925199433],\n"
              "        ID[\"EPSG\",8814]],\n"
              "    PARAMETER[\"Scale factor on initial line\",5,\n"
              "        SCALEUNIT[\"unity\",1],\n"
              "        ID[\"EPSG\",8815]],\n"
              "    PARAMETER[\"False easting\",6,\n"
              "        LENGTHUNIT[\"metre\",1],\n"
              "        ID[\"EPSG\",8806]],\n"
              "    PARAMETER[\"False northing\",7,\n"
              "        LENGTHUNIT[\"metre\",1],\n"
              "        ID[\"EPSG\",8807]]]");

    EXPECT_EQ(conv->exportToWKT(
                  WKTFormatter::create(WKTFormatter::Convention::WKT1_GDAL)),
              "PROJECTION[\"Hotine_Oblique_Mercator\"],\n"
              "PARAMETER[\"latitude_of_origin\",1],\n"
              "PARAMETER[\"central_meridian\",2],\n"
              "PARAMETER[\"azimuth\",3],\n"
              "PARAMETER[\"rectified_grid_angle\",4],\n"
              "PARAMETER[\"scale_factor\",5],\n"
              "PARAMETER[\"false_easting\",6],\n"
              "PARAMETER[\"false_northing\",7]");
}

// ---------------------------------------------------------------------------

TEST(operation, hotine_oblique_mercator_variant_A_export_swiss_mercator) {
    auto conv = Conversion::createHotineObliqueMercatorVariantA(
        PropertyMap(), Angle(1), Angle(2), Angle(90), Angle(90), Scale(5),
        Length(6), Length(7));
    EXPECT_EQ(conv->exportToPROJString(PROJStringFormatter::create()),
              "+proj=somerc +lat_0=1 +lon_0=2 +k_0=5 "
              "+x_0=6 +y_0=7");
}

// ---------------------------------------------------------------------------

TEST(operation, hotine_oblique_mercator_variant_B_export) {
    auto conv = Conversion::createHotineObliqueMercatorVariantB(
        PropertyMap(), Angle(1), Angle(2), Angle(3), Angle(4), Scale(5),
        Length(6), Length(7));
    EXPECT_EQ(conv->exportToPROJString(PROJStringFormatter::create()),
              "+proj=omerc +lat_0=1 +lonc=2 +alpha=3 +gamma=4 +k=5 "
              "+x_0=6 +y_0=7");

    EXPECT_EQ(conv->exportToWKT(WKTFormatter::create()),
              "CONVERSION[\"Hotine Oblique Mercator (variant B)\",\n"
              "    METHOD[\"Hotine Oblique Mercator (variant B)\",\n"
              "        ID[\"EPSG\",9815]],\n"
              "    PARAMETER[\"Latitude of projection centre\",1,\n"
              "        ANGLEUNIT[\"degree\",0.0174532925199433],\n"
              "        ID[\"EPSG\",8811]],\n"
              "    PARAMETER[\"Longitude of projection centre\",2,\n"
              "        ANGLEUNIT[\"degree\",0.0174532925199433],\n"
              "        ID[\"EPSG\",8812]],\n"
              "    PARAMETER[\"Azimuth of initial line\",3,\n"
              "        ANGLEUNIT[\"degree\",0.0174532925199433],\n"
              "        ID[\"EPSG\",8813]],\n"
              "    PARAMETER[\"Angle from Rectified to Skew Grid\",4,\n"
              "        ANGLEUNIT[\"degree\",0.0174532925199433],\n"
              "        ID[\"EPSG\",8814]],\n"
              "    PARAMETER[\"Scale factor on initial line\",5,\n"
              "        SCALEUNIT[\"unity\",1],\n"
              "        ID[\"EPSG\",8815]],\n"
              "    PARAMETER[\"Easting at projection centre\",6,\n"
              "        LENGTHUNIT[\"metre\",1],\n"
              "        ID[\"EPSG\",8816]],\n"
              "    PARAMETER[\"Northing at projection centre\",7,\n"
              "        LENGTHUNIT[\"metre\",1],\n"
              "        ID[\"EPSG\",8817]]]");

    EXPECT_EQ(conv->exportToWKT(
                  WKTFormatter::create(WKTFormatter::Convention::WKT1_GDAL)),
              "PROJECTION[\"Hotine_Oblique_Mercator_Azimuth_Center\"],\n"
              "PARAMETER[\"latitude_of_origin\",1],\n"
              "PARAMETER[\"central_meridian\",2],\n"
              "PARAMETER[\"azimuth\",3],\n"
              "PARAMETER[\"rectified_grid_angle\",4],\n"
              "PARAMETER[\"scale_factor\",5],\n"
              "PARAMETER[\"false_easting\",6],\n"
              "PARAMETER[\"false_northing\",7]");
}

// ---------------------------------------------------------------------------

TEST(operation, hotine_oblique_mercator_variant_B_export_swiss_mercator) {
    auto conv = Conversion::createHotineObliqueMercatorVariantB(
        PropertyMap(), Angle(1), Angle(2), Angle(90), Angle(90), Scale(5),
        Length(6), Length(7));
    EXPECT_EQ(conv->exportToPROJString(PROJStringFormatter::create()),
              "+proj=somerc +lat_0=1 +lon_0=2 +k_0=5 "
              "+x_0=6 +y_0=7");
}

// ---------------------------------------------------------------------------

TEST(operation, hotine_oblique_mercator_two_point_natural_origin_export) {
    auto conv = Conversion::createHotineObliqueMercatorTwoPointNaturalOrigin(
        PropertyMap(), Angle(1), Angle(2), Angle(3), Angle(4), Angle(5),
        Scale(6), Length(7), Length(8));
    EXPECT_EQ(conv->exportToPROJString(PROJStringFormatter::create()),
              "+proj=omerc +lat_0=1 +lat_1=2 +lon_1=3 +lat_2=4 +lon_2=5 +k=6 "
              "+x_0=7 +y_0=8");

    auto formatter = WKTFormatter::create();
    formatter->simulCurNodeHasId();
    EXPECT_EQ(
        conv->exportToWKT(formatter),
        "CONVERSION[\"Hotine Oblique Mercator Two Point Natural Origin\",\n"
        "    METHOD[\"Hotine Oblique Mercator Two Point Natural Origin\"],\n"
        "    PARAMETER[\"Latitude of projection centre\",1,\n"
        "        ANGLEUNIT[\"degree\",0.0174532925199433],\n"
        "        ID[\"EPSG\",8811]],\n"
        "    PARAMETER[\"Latitude of point 1\",2,\n"
        "        ANGLEUNIT[\"degree\",0.0174532925199433]],\n"
        "    PARAMETER[\"Longitude of point 1\",3,\n"
        "        ANGLEUNIT[\"degree\",0.0174532925199433]],\n"
        "    PARAMETER[\"Latitude of point 2\",4,\n"
        "        ANGLEUNIT[\"degree\",0.0174532925199433]],\n"
        "    PARAMETER[\"Longitude of point 2\",5,\n"
        "        ANGLEUNIT[\"degree\",0.0174532925199433]],\n"
        "    PARAMETER[\"Scale factor on initial line\",6,\n"
        "        SCALEUNIT[\"unity\",1],\n"
        "        ID[\"EPSG\",8815]],\n"
        "    PARAMETER[\"Easting at projection centre\",7,\n"
        "        LENGTHUNIT[\"metre\",1],\n"
        "        ID[\"EPSG\",8816]],\n"
        "    PARAMETER[\"Northing at projection centre\",8,\n"
        "        LENGTHUNIT[\"metre\",1],\n"
        "        ID[\"EPSG\",8817]]]");

    EXPECT_EQ(
        conv->exportToWKT(
            WKTFormatter::create(WKTFormatter::Convention::WKT1_GDAL)),
        "PROJECTION[\"Hotine_Oblique_Mercator_Two_Point_Natural_Origin\"],\n"
        "PARAMETER[\"latitude_of_origin\",1],\n"
        "PARAMETER[\"latitude_of_point_1\",2],\n"
        "PARAMETER[\"longitude_of_point_1\",3],\n"
        "PARAMETER[\"latitude_of_point_2\",4],\n"
        "PARAMETER[\"longitude_of_point_2\",5],\n"
        "PARAMETER[\"scale_factor\",6],\n"
        "PARAMETER[\"false_easting\",7],\n"
        "PARAMETER[\"false_northing\",8]");
}

// ---------------------------------------------------------------------------

TEST(operation, imw_polyconic_export) {
    auto conv = Conversion::createInternationalMapWorldPolyconic(
        PropertyMap(), Angle(1), Angle(3), Angle(4), Length(5), Length(6));

    EXPECT_EQ(conv->exportToPROJString(PROJStringFormatter::create()),
              "+proj=imw_p +lon_0=1 +lat_1=3 +lat_2=4 +x_0=5 +y_0=6");

    EXPECT_EQ(conv->exportToWKT(WKTFormatter::create()),
              "CONVERSION[\"International Map of the World Polyconic\",\n"
              "    METHOD[\"International Map of the World Polyconic\"],\n"
              "    PARAMETER[\"Longitude of natural origin\",1,\n"
              "        ANGLEUNIT[\"degree\",0.0174532925199433],\n"
              "        ID[\"EPSG\",8802]],\n"
              "    PARAMETER[\"Latitude of 1st standard parallel\",3,\n"
              "        ANGLEUNIT[\"degree\",0.0174532925199433],\n"
              "        ID[\"EPSG\",8823]],\n"
              "    PARAMETER[\"Latitude of 2nd standard parallel\",4,\n"
              "        ANGLEUNIT[\"degree\",0.0174532925199433],\n"
              "        ID[\"EPSG\",8824]],\n"
              "    PARAMETER[\"False easting\",5,\n"
              "        LENGTHUNIT[\"metre\",1],\n"
              "        ID[\"EPSG\",8806]],\n"
              "    PARAMETER[\"False northing\",6,\n"
              "        LENGTHUNIT[\"metre\",1],\n"
              "        ID[\"EPSG\",8807]]]");

    EXPECT_EQ(conv->exportToWKT(
                  WKTFormatter::create(WKTFormatter::Convention::WKT1_GDAL)),
              "PROJECTION[\"International_Map_of_the_World_Polyconic\"],\n"
              "PARAMETER[\"central_meridian\",1],\n"
              "PARAMETER[\"standard_parallel_1\",3],\n"
              "PARAMETER[\"standard_parallel_2\",4],\n"
              "PARAMETER[\"false_easting\",5],\n"
              "PARAMETER[\"false_northing\",6]");
}

// ---------------------------------------------------------------------------

TEST(operation, krovak_north_oriented_export) {
    auto conv = Conversion::createKrovakNorthOriented(
        PropertyMap(), Angle(49.5), Angle(42.5), Angle(30.28813972222222),
        Angle(78.5), Scale(0.9999), Length(5), Length(6));

    EXPECT_EQ(conv->exportToPROJString(PROJStringFormatter::create()),
              "+proj=krovak +lat_0=49.5 +lon_0=42.5 +k=0.9999 +x_0=5 +y_0=6");

    EXPECT_EQ(
        conv->exportToWKT(WKTFormatter::create()),
        "CONVERSION[\"Krovak (North Orientated)\",\n"
        "    METHOD[\"Krovak (North Orientated)\",\n"
        "        ID[\"EPSG\",1041]],\n"
        "    PARAMETER[\"Latitude of projection centre\",49.5,\n"
        "        ANGLEUNIT[\"degree\",0.0174532925199433],\n"
        "        ID[\"EPSG\",8811]],\n"
        "    PARAMETER[\"Longitude of origin\",42.5,\n"
        "        ANGLEUNIT[\"degree\",0.0174532925199433],\n"
        "        ID[\"EPSG\",8833]],\n"
        "    PARAMETER[\"Co-latitude of cone axis\",30.2881397222222,\n"
        "        ANGLEUNIT[\"degree\",0.0174532925199433],\n"
        "        ID[\"EPSG\",1036]],\n"
        "    PARAMETER[\"Latitude of pseudo standard parallel\",78.5,\n"
        "        ANGLEUNIT[\"degree\",0.0174532925199433],\n"
        "        ID[\"EPSG\",8818]],\n"
        "    PARAMETER[\"Scale factor on pseudo standard parallel\",0.9999,\n"
        "        SCALEUNIT[\"unity\",1],\n"
        "        ID[\"EPSG\",8819]],\n"
        "    PARAMETER[\"False easting\",5,\n"
        "        LENGTHUNIT[\"metre\",1],\n"
        "        ID[\"EPSG\",8806]],\n"
        "    PARAMETER[\"False northing\",6,\n"
        "        LENGTHUNIT[\"metre\",1],\n"
        "        ID[\"EPSG\",8807]]]");

    EXPECT_EQ(conv->exportToWKT(
                  WKTFormatter::create(WKTFormatter::Convention::WKT1_GDAL)),
              "PROJECTION[\"Krovak\"],\n"
              "PARAMETER[\"latitude_of_center\",49.5],\n"
              "PARAMETER[\"longitude_of_center\",42.5],\n"
              "PARAMETER[\"azimuth\",30.2881397222222],\n"
              "PARAMETER[\"pseudo_standard_parallel_1\",78.5],\n"
              "PARAMETER[\"scale_factor\",0.9999],\n"
              "PARAMETER[\"false_easting\",5],\n"
              "PARAMETER[\"false_northing\",6]");
}

// ---------------------------------------------------------------------------

TEST(operation, krovak_export) {
    auto conv = Conversion::createKrovak(
        PropertyMap(), Angle(49.5), Angle(42.5), Angle(30.28813972222222),
        Angle(78.5), Scale(0.9999), Length(5), Length(6));

    EXPECT_EQ(conv->exportToPROJString(PROJStringFormatter::create()),
              "+proj=krovak +axis=swu +lat_0=49.5 +lon_0=42.5 +k=0.9999 +x_0=5 "
              "+y_0=6");

    EXPECT_EQ(
        conv->exportToWKT(WKTFormatter::create()),
        "CONVERSION[\"Krovak\",\n"
        "    METHOD[\"Krovak\",\n"
        "        ID[\"EPSG\",9819]],\n"
        "    PARAMETER[\"Latitude of projection centre\",49.5,\n"
        "        ANGLEUNIT[\"degree\",0.0174532925199433],\n"
        "        ID[\"EPSG\",8811]],\n"
        "    PARAMETER[\"Longitude of origin\",42.5,\n"
        "        ANGLEUNIT[\"degree\",0.0174532925199433],\n"
        "        ID[\"EPSG\",8833]],\n"
        "    PARAMETER[\"Co-latitude of cone axis\",30.2881397222222,\n"
        "        ANGLEUNIT[\"degree\",0.0174532925199433],\n"
        "        ID[\"EPSG\",1036]],\n"
        "    PARAMETER[\"Latitude of pseudo standard parallel\",78.5,\n"
        "        ANGLEUNIT[\"degree\",0.0174532925199433],\n"
        "        ID[\"EPSG\",8818]],\n"
        "    PARAMETER[\"Scale factor on pseudo standard parallel\",0.9999,\n"
        "        SCALEUNIT[\"unity\",1],\n"
        "        ID[\"EPSG\",8819]],\n"
        "    PARAMETER[\"False easting\",5,\n"
        "        LENGTHUNIT[\"metre\",1],\n"
        "        ID[\"EPSG\",8806]],\n"
        "    PARAMETER[\"False northing\",6,\n"
        "        LENGTHUNIT[\"metre\",1],\n"
        "        ID[\"EPSG\",8807]]]");

    EXPECT_THROW(conv->exportToWKT(
                     WKTFormatter::create(WKTFormatter::Convention::WKT1_GDAL)),
                 FormattingException);
}

// ---------------------------------------------------------------------------

TEST(operation, nzmg_export) {
    auto conv = Conversion::createNewZealandMappingGrid(
        PropertyMap(), Angle(1), Angle(2), Length(4), Length(5));

    EXPECT_EQ(conv->exportToPROJString(PROJStringFormatter::create()),
              "+proj=nzmg +lat_0=1 +lon_0=2 +x_0=4 +y_0=5");

    EXPECT_EQ(conv->exportToWKT(WKTFormatter::create()),
              "CONVERSION[\"New Zealand Map Grid\",\n"
              "    METHOD[\"New Zealand Map Grid\",\n"
              "        ID[\"EPSG\",9811]],\n"
              "    PARAMETER[\"Latitude of natural origin\",1,\n"
              "        ANGLEUNIT[\"degree\",0.0174532925199433],\n"
              "        ID[\"EPSG\",8801]],\n"
              "    PARAMETER[\"Longitude of natural origin\",2,\n"
              "        ANGLEUNIT[\"degree\",0.0174532925199433],\n"
              "        ID[\"EPSG\",8802]],\n"
              "    PARAMETER[\"False easting\",4,\n"
              "        LENGTHUNIT[\"metre\",1],\n"
              "        ID[\"EPSG\",8806]],\n"
              "    PARAMETER[\"False northing\",5,\n"
              "        LENGTHUNIT[\"metre\",1],\n"
              "        ID[\"EPSG\",8807]]]");

    EXPECT_EQ(conv->exportToWKT(
                  WKTFormatter::create(WKTFormatter::Convention::WKT1_GDAL)),
              "PROJECTION[\"New_Zealand_Map_Grid\"],\n"
              "PARAMETER[\"latitude_of_origin\",1],\n"
              "PARAMETER[\"central_meridian\",2],\n"
              "PARAMETER[\"false_easting\",4],\n"
              "PARAMETER[\"false_northing\",5]");
}

// ---------------------------------------------------------------------------

TEST(operation, wkt1_import_equivalent_parameters) {
    auto wkt = "PROJCS[\"test\",\n"
               "    GEOGCS[\"WGS 84\",\n"
               "        DATUM[\"WGS 1984\",\n"
               "            SPHEROID[\"WGS 84\",6378137,298.257223563]],\n"
               "        PRIMEM[\"Greenwich\",0],\n"
               "        UNIT[\"degree\",0.0174532925199433]],\n"
               "    PROJECTION[\"Hotine Oblique Mercator Two Point Natural "
               "Origin\"],\n"
               "    PARAMETER[\"latitude_of_origin\",1],\n"
               "    PARAMETER[\"Latitude_Of_1st_Point\",2],\n"
               "    PARAMETER[\"Longitude_Of_1st_Point\",3],\n"
               "    PARAMETER[\"Latitude_Of_2nd_Point\",4],\n"
               "    PARAMETER[\"Longitude_Of 2nd_Point\",5],\n"
               "    PARAMETER[\"scale_factor\",6],\n"
               "    PARAMETER[\"false_easting\",7],\n"
               "    PARAMETER[\"false_northing\",8],\n"
               "    UNIT[\"metre\",1]]";
    auto obj = WKTParser().createFromWKT(wkt);
    auto crs = nn_dynamic_pointer_cast<ProjectedCRS>(obj);
    ASSERT_TRUE(crs != nullptr);

    auto conversion = crs->derivingConversion();
    auto convRef = Conversion::createHotineObliqueMercatorTwoPointNaturalOrigin(
        PropertyMap(), Angle(1), Angle(2), Angle(3), Angle(4), Angle(5),
        Scale(6), Length(7), Length(8));

    EXPECT_EQ(conversion->exportToWKT(
                  WKTFormatter::create(WKTFormatter::Convention::WKT1_GDAL)),
              convRef->exportToWKT(
                  WKTFormatter::create(WKTFormatter::Convention::WKT1_GDAL)));
}

// ---------------------------------------------------------------------------

TEST(operation, conversion_inverse) {
    auto conv = Conversion::createTransverseMercator(
        PropertyMap(), Angle(1), Angle(2), Scale(3), Length(4), Length(5));
    auto inv = conv->inverse();
    EXPECT_EQ(inv->inverse(), conv);
    EXPECT_EQ(inv->exportToWKT(WKTFormatter::create()),
              "CONVERSION[\"Inverse of Transverse Mercator\",\n"
              "    METHOD[\"Inverse of Transverse Mercator\"],\n"
              "    PARAMETER[\"Latitude of natural origin\",1,\n"
              "        ANGLEUNIT[\"degree\",0.0174532925199433],\n"
              "        ID[\"EPSG\",8801]],\n"
              "    PARAMETER[\"Longitude of natural origin\",2,\n"
              "        ANGLEUNIT[\"degree\",0.0174532925199433],\n"
              "        ID[\"EPSG\",8802]],\n"
              "    PARAMETER[\"Scale factor at natural origin\",3,\n"
              "        SCALEUNIT[\"unity\",1],\n"
              "        ID[\"EPSG\",8805]],\n"
              "    PARAMETER[\"False easting\",4,\n"
              "        LENGTHUNIT[\"metre\",1],\n"
              "        ID[\"EPSG\",8806]],\n"
              "    PARAMETER[\"False northing\",5,\n"
              "        LENGTHUNIT[\"metre\",1],\n"
              "        ID[\"EPSG\",8807]]]");

    EXPECT_EQ(inv->exportToPROJString(PROJStringFormatter::create()),
              "+proj=pipeline +step +inv +proj=tmerc +lat_0=1 +lon_0=2 +k_0=3 "
              "+x_0=4 +y_0=5");

    EXPECT_TRUE(inv->isEquivalentTo(inv));
    EXPECT_FALSE(inv->isEquivalentTo(createUnrelatedObject()));
}

// ---------------------------------------------------------------------------

TEST(operation, PROJ_based) {
    auto conv =
        SingleOperation::createPROJBased("+proj=merc", nullptr, nullptr);

    EXPECT_EQ(conv->exportToPROJString(PROJStringFormatter::create()),
              "+proj=merc");

    EXPECT_EQ(conv->exportToWKT(WKTFormatter::create()),
              "CONVERSION[\"PROJ-based coordinate operation\",\n"
              "    METHOD[\"PROJ-based operation method\"],\n"
              "    PARAMETER[\"PROJ string\",\"+proj=merc\"]]");

    EXPECT_EQ(
        conv->inverse()->exportToPROJString(PROJStringFormatter::create()),
        "+proj=pipeline +step +inv +proj=merc");

    auto str = "+proj=pipeline +step +proj=unitconvert +xy_in=grad +xy_out=rad "
               "+step +proj=axisswap +order=2,1 +step +proj=longlat "
               "+ellps=clrk80ign +pm=paris +step +proj=axisswap +order=2,1";
    EXPECT_EQ(SingleOperation::createPROJBased(str, nullptr, nullptr)
                  ->exportToPROJString(PROJStringFormatter::create()),
              str);

    EXPECT_THROW(SingleOperation::createPROJBased("+inv", nullptr, nullptr)
                     ->exportToPROJString(PROJStringFormatter::create()),
                 FormattingException);
    EXPECT_THROW(SingleOperation::createPROJBased("foo", nullptr, nullptr)
                     ->exportToPROJString(PROJStringFormatter::create()),
                 FormattingException);
}

// ---------------------------------------------------------------------------

TEST(operation, PROJ_based_empty) {
    auto conv = SingleOperation::createPROJBased("", nullptr, nullptr);

    EXPECT_EQ(conv->exportToPROJString(PROJStringFormatter::create()), "");

    EXPECT_EQ(conv->exportToWKT(WKTFormatter::create()),
              "CONVERSION[\"PROJ-based coordinate operation\",\n"
              "    METHOD[\"PROJ-based operation method\"],\n"
              "    PARAMETER[\"PROJ string\",\"\"]]");

    EXPECT_THROW(conv->exportToWKT(
                     WKTFormatter::create(WKTFormatter::Convention::WKT1_GDAL)),
                 FormattingException);

    EXPECT_EQ(
        conv->inverse()->exportToPROJString(PROJStringFormatter::create()), "");
}

// ---------------------------------------------------------------------------

TEST(operation, geogCRS_to_geogCRS) {

    auto op = CoordinateOperationFactory::create()->createOperation(
        GeographicCRS::EPSG_4807, GeographicCRS::EPSG_4326);
    ASSERT_TRUE(op != nullptr);
    EXPECT_EQ(op->exportToPROJString(PROJStringFormatter::create()),
              "+proj=pipeline +step +proj=axisswap +order=2,1 +step "
              "+proj=unitconvert +xy_in=grad +xy_out=rad +step +proj=longlat "
              "+ellps=clrk80ign +pm=paris +step +proj=unitconvert +xy_in=rad "
              "+xy_out=deg +step +proj=axisswap +order=2,1");
}

// ---------------------------------------------------------------------------

TEST(operation, geogCRS_to_geogCRS_noop) {

    auto op = CoordinateOperationFactory::create()->createOperation(
        GeographicCRS::EPSG_4326, GeographicCRS::EPSG_4326);
    ASSERT_TRUE(op != nullptr);
    EXPECT_EQ(op->exportToPROJString(PROJStringFormatter::create()), "");
}

// ---------------------------------------------------------------------------

TEST(operation, geocentricCRS_to_geogCRS) {

    auto op = CoordinateOperationFactory::create()->createOperation(
        createGeocentric(), GeographicCRS::EPSG_4326);
    ASSERT_TRUE(op != nullptr);
    EXPECT_EQ(op->exportToPROJString(PROJStringFormatter::create()),
              "+proj=pipeline +step +inv +proj=cart +ellps=WGS84 +step "
              "+proj=unitconvert +xy_in=rad +xy_out=deg +step +proj=axisswap "
              "+order=2,1");
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
    EXPECT_EQ(op->exportToPROJString(PROJStringFormatter::create()),
              "+proj=pipeline +step +proj=axisswap +order=2,1 +step "
              "+proj=unitconvert +xy_in=deg +xy_out=rad +step +proj=utm "
              "+zone=31 +ellps=WGS84");
}

// ---------------------------------------------------------------------------

TEST(operation, geogCRS_different_from_baseCRS_to_projCRS) {

    auto op = CoordinateOperationFactory::create()->createOperation(
        GeographicCRS::EPSG_4807, createUTM31_WGS84());
    ASSERT_TRUE(op != nullptr);
    EXPECT_EQ(op->exportToPROJString(PROJStringFormatter::create()),
              "+proj=pipeline +step +proj=axisswap +order=2,1 +step "
              "+proj=unitconvert +xy_in=grad +xy_out=rad +step +proj=longlat "
              "+ellps=clrk80ign +pm=paris +step +proj=utm +zone=31 "
              "+ellps=WGS84");
}

// ---------------------------------------------------------------------------

TEST(operation, geocentricCRS_to_projCRS) {

    auto op = CoordinateOperationFactory::create()->createOperation(
        createGeocentric(), createUTM31_WGS84());
    ASSERT_TRUE(op != nullptr);
    EXPECT_EQ(op->exportToPROJString(PROJStringFormatter::create()),
              "+proj=pipeline +step +inv +proj=cart +ellps=WGS84 +step "
              "+proj=utm +zone=31 +ellps=WGS84");
}

// ---------------------------------------------------------------------------

TEST(operation, projCRS_to_geogCRS) {

    auto op = CoordinateOperationFactory::create()->createOperation(
        createUTM31_WGS84(), GeographicCRS::EPSG_4326);
    ASSERT_TRUE(op != nullptr);
    EXPECT_EQ(op->exportToPROJString(PROJStringFormatter::create()),
              "+proj=pipeline +step +inv +proj=utm +zone=31 +ellps=WGS84 +step "
              "+proj=unitconvert +xy_in=rad +xy_out=deg +step +proj=axisswap "
              "+order=2,1");
}

// ---------------------------------------------------------------------------

TEST(operation, projCRS_to_projCRS) {

    auto op = CoordinateOperationFactory::create()->createOperation(
        createUTM31_WGS84(), createUTM32_WGS84());
    ASSERT_TRUE(op != nullptr);
    EXPECT_EQ(op->exportToPROJString(PROJStringFormatter::create()),
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
    EXPECT_EQ(op->exportToPROJString(PROJStringFormatter::create()),
              "+proj=pipeline +step +inv +proj=utm +zone=31 +ellps=WGS84 +step "
              "+proj=utm +zone=32 +ellps=clrk80ign");
}

// ---------------------------------------------------------------------------

TEST(operation, boundCRS_of_geogCRS_to_geogCRS) {
    auto boundCRS = BoundCRS::createFromTOWGS84(
        GeographicCRS::EPSG_4807, std::vector<double>{1, 2, 3, 4, 5, 6, 7});
    auto op = CoordinateOperationFactory::create()->createOperation(
        boundCRS, GeographicCRS::EPSG_4326);
    ASSERT_TRUE(op != nullptr);
    EXPECT_EQ(op->exportToPROJString(PROJStringFormatter::create()),
              "+proj=pipeline +step +proj=axisswap +order=2,1 +step "
              "+proj=unitconvert +xy_in=grad +xy_out=rad +step +proj=longlat "
              "+ellps=clrk80ign +pm=paris +step +proj=cart +ellps=clrk80ign "
              "+step +proj=helmert +x=1 +y=2 +z=3 +rx=4 +ry=5 +rz=6 +s=7 +step "
              "+inv +proj=cart +ellps=WGS84 +step +proj=unitconvert +xy_in=rad "
              "+xy_out=deg +step +proj=axisswap +order=2,1");
}

// ---------------------------------------------------------------------------

TEST(operation, boundCRS_of_geogCRS_to_unrelated_geogCRS) {
    auto boundCRS = BoundCRS::createFromTOWGS84(
        GeographicCRS::EPSG_4807, std::vector<double>{1, 2, 3, 4, 5, 6, 7});
    auto op = CoordinateOperationFactory::create()->createOperation(
        boundCRS, GeographicCRS::EPSG_4269);
    ASSERT_TRUE(op != nullptr);
    EXPECT_EQ(op->exportToPROJString(PROJStringFormatter::create()),
              CoordinateOperationFactory::create()
                  ->createOperation(GeographicCRS::EPSG_4807,
                                    GeographicCRS::EPSG_4269)
                  ->exportToPROJString(PROJStringFormatter::create()));
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
    EXPECT_EQ(op->exportToPROJString(PROJStringFormatter::create()),
              "+proj=pipeline +step +inv +proj=utm +zone=31 +ellps=clrk80ign "
              "+step +proj=cart +ellps=clrk80ign +step +proj=helmert +x=1 +y=2 "
              "+z=3 +rx=4 +ry=5 +rz=6 +s=7 +step +inv +proj=cart +ellps=WGS84 "
              "+step +proj=unitconvert +xy_in=rad +xy_out=deg +step "
              "+proj=axisswap +order=2,1");
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
    EXPECT_EQ(op->exportToPROJString(PROJStringFormatter::create()),
              "+proj=pipeline +step +proj=axisswap +order=2,1 +step "
              "+proj=unitconvert +xy_in=grad +xy_out=rad +step +proj=longlat "
              "+ellps=clrk80ign +pm=paris +step +proj=cart +ellps=clrk80ign "
              "+step +proj=helmert +x=1 +y=2 +z=3 +rx=4 +ry=5 +rz=6 +s=7 +step "
              "+inv +proj=cart +ellps=WGS84 +step +proj=utm +zone=31 "
              "+ellps=WGS84");
}

// ---------------------------------------------------------------------------

TEST(operation, geogCRS_to_boundCRS_of_geogCRS) {
    auto boundCRS = BoundCRS::createFromTOWGS84(
        GeographicCRS::EPSG_4807, std::vector<double>{1, 2, 3, 4, 5, 6, 7});
    auto op = CoordinateOperationFactory::create()->createOperation(
        GeographicCRS::EPSG_4326, boundCRS);
    ASSERT_TRUE(op != nullptr);
    EXPECT_EQ(op->exportToPROJString(PROJStringFormatter::create()),
              "+proj=pipeline +step +proj=axisswap +order=2,1 +step "
              "+proj=unitconvert +xy_in=deg +xy_out=rad +step +proj=cart "
              "+ellps=WGS84 +step +proj=helmert +x=-1 +y=-2 +z=-3 +rx=-4 "
              "+ry=-5 +rz=-6 +s=7 +step +inv +proj=cart +ellps=clrk80ign +step "
              "+inv +proj=longlat +ellps=clrk80ign +pm=paris +step "
              "+proj=unitconvert +xy_in=rad +xy_out=grad +step +proj=axisswap "
              "+order=2,1");
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
    EXPECT_EQ(op->exportToPROJString(PROJStringFormatter::create()),
              "+proj=pipeline +step +inv +proj=utm +zone=31 +ellps=clrk80ign "
              "+step +proj=cart +ellps=clrk80ign +step +proj=helmert "
              "+x=1 +y=2 +z=3 +rx=4 +ry=5 +rz=6 +s=7 +step +proj=helmert +x=-8 "
              "+y=-9 +z=-10 +rx=-11 +ry=-12 +rz=-13 +s=14 +step +inv "
              "+proj=cart +ellps=GRS80 +step +proj=utm +zone=32 "
              "+ellps=GRS80");
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
    EXPECT_EQ(op->exportToPROJString(PROJStringFormatter::create()),
              "+proj=pipeline +step +proj=axisswap +order=2,1 +step "
              "+proj=unitconvert +xy_in=grad +xy_out=rad +step +proj=longlat "
              "+ellps=clrk80ign +pm=paris +step +proj=cart +ellps=clrk80ign "
              "+step +inv +proj=cart +ellps=GRS80 +step +proj=unitconvert "
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
    EXPECT_EQ(op->exportToPROJString(PROJStringFormatter::create()),
              CoordinateOperationFactory::create()
                  ->createOperation(boundCRS1->baseCRS(), boundCRS2->baseCRS())
                  ->exportToPROJString(PROJStringFormatter::create()));
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
    EXPECT_EQ(op->exportToPROJString(PROJStringFormatter::create()),
              CoordinateOperationFactory::create()
                  ->createOperation(GeographicCRS::EPSG_4326,
                                    GeographicCRS::EPSG_4807)
                  ->exportToPROJString(PROJStringFormatter::create()));
}

// ---------------------------------------------------------------------------

static BoundCRSNNPtr createBoundVerticalCRS() {
    auto vertCRS = createVerticalCRS();
    auto transformation =
        Transformation::createGravityRelatedHeightToGeographic3D(
            PropertyMap(), vertCRS, GeographicCRS::EPSG_4979, "egm08_25.gtx",
            std::vector<PositionalAccuracyNNPtr>());
    return BoundCRS::create(vertCRS, GeographicCRS::EPSG_4979, transformation);
}

// ---------------------------------------------------------------------------

TEST(operation, transformation_height_to_PROJ_string) {
    EXPECT_EQ(createBoundVerticalCRS()->transformation()->exportToPROJString(
                  PROJStringFormatter::create()),
              "+proj=vgridshift +grids=egm08_25.gtx");
}

// ---------------------------------------------------------------------------

TEST(operation, transformation_ntv2_to_PROJ_string) {
    auto transformation = Transformation::createNTv2(
        PropertyMap(), GeographicCRS::EPSG_4807, GeographicCRS::EPSG_4326,
        "foo.gsb", std::vector<PositionalAccuracyNNPtr>());
    EXPECT_EQ(transformation->exportToPROJString(PROJStringFormatter::create()),
              "+proj=hgridshift +grids=foo.gsb");
}

// ---------------------------------------------------------------------------

TEST(operation, transformation_VERTCON_to_PROJ_string) {
    auto verticalCRS1 = createVerticalCRS();

    auto verticalCRS2 = VerticalCRS::create(
        PropertyMap(), VerticalReferenceFrame::create(PropertyMap()),
        VerticalCS::createGravityRelatedHeight(UnitOfMeasure::METRE));

    // Use of this type of transformation is a bit of non-sense here
    // since it should normally be used with NGVD29 and NAVD88 for VerticalCRS,
    // and NAD27/NAD83 as horizontal CRS...
    auto vtransformation = Transformation::createVERTCON(
        PropertyMap(), verticalCRS1, verticalCRS2, "bla.gtx",
        std::vector<PositionalAccuracyNNPtr>());
    EXPECT_EQ(
        vtransformation->exportToPROJString(PROJStringFormatter::create()),
        "+proj=vgridshift +grids=bla.gtx +multiplier=0.001");
}

// ---------------------------------------------------------------------------

TEST(operation, compoundCRS_with_boundVerticalCRS_to_geogCRS) {

    auto compound = CompoundCRS::create(
        PropertyMap(), std::vector<CRSNNPtr>{GeographicCRS::EPSG_4326,
                                             createBoundVerticalCRS()});
    auto op = CoordinateOperationFactory::create()->createOperation(
        compound, GeographicCRS::EPSG_4979);
    ASSERT_TRUE(op != nullptr);
    EXPECT_EQ(op->exportToPROJString(PROJStringFormatter::create()),
              "+proj=pipeline +step +proj=axisswap +order=2,1 +step "
              "+proj=unitconvert +xy_in=deg +xy_out=rad +step +proj=vgridshift "
              "+grids=egm08_25.gtx +step +proj=unitconvert +xy_in=rad "
              "+xy_out=deg +step +proj=axisswap +order=2,1");
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
    EXPECT_EQ(op->exportToPROJString(PROJStringFormatter::create()),
              "+proj=pipeline +step +proj=axisswap +order=2,1 +step "
              "+proj=unitconvert +xy_in=grad +xy_out=rad +step +proj=longlat "
              "+ellps=clrk80ign +pm=paris +step +proj=cart +ellps=clrk80ign "
              "+step +proj=helmert +x=1 +y=2 +z=3 +rx=4 +ry=5 +rz=6 +s=7 +step "
              "+inv +proj=cart +ellps=WGS84 +step +proj=vgridshift "
              "+grids=egm08_25.gtx +step +proj=unitconvert +xy_in=rad "
              "+xy_out=deg +step +proj=axisswap +order=2,1");

    auto opInverse = CoordinateOperationFactory::create()->createOperation(
        GeographicCRS::EPSG_4979, compound);
    ASSERT_TRUE(opInverse != nullptr);
    EXPECT_TRUE(opInverse->inverse()->isEquivalentTo(NN_CHECK_ASSERT(op)));
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
    EXPECT_EQ(op->exportToPROJString(PROJStringFormatter::create()),
              "+proj=pipeline +step +inv +proj=utm +zone=31 +ellps=clrk80ign "
              "+step +proj=cart +ellps=clrk80ign +step +proj=helmert +x=1 +y=2 "
              "+z=3 +rx=4 +ry=5 +rz=6 +s=7 +step +inv +proj=cart +ellps=WGS84 "
              "+step +proj=vgridshift +grids=egm08_25.gtx +step "
              "+proj=unitconvert +xy_in=rad +xy_out=deg +step +proj=axisswap "
              "+order=2,1");

    auto opInverse = CoordinateOperationFactory::create()->createOperation(
        GeographicCRS::EPSG_4979, compound);
    ASSERT_TRUE(opInverse != nullptr);
    EXPECT_TRUE(opInverse->inverse()->isEquivalentTo(NN_CHECK_ASSERT(op)));
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
    EXPECT_TRUE(op->isEquivalentTo(NN_CHECK_ASSERT(opRef)));
}

// ---------------------------------------------------------------------------

TEST(operation, compoundCRS_to_compoundCRS_with_vertical_transform) {
    auto verticalCRS1 = createVerticalCRS();

    auto verticalCRS2 = VerticalCRS::create(
        PropertyMap(), VerticalReferenceFrame::create(PropertyMap()),
        VerticalCS::createGravityRelatedHeight(UnitOfMeasure::METRE));

    // Use of this type of transformation is a bit of non-sense here
    // since it should normally be used with NGVD29 and NAVD88 for VerticalCRS,
    // and NAD27/NAD83 as horizontal CRS...
    auto vtransformation = Transformation::createVERTCON(
        PropertyMap(), verticalCRS1, verticalCRS2, "bla.gtx",
        std::vector<PositionalAccuracyNNPtr>());

    auto compound1 = CompoundCRS::create(
        PropertyMap(),
        std::vector<CRSNNPtr>{
            createUTM31_WGS84(),
            BoundCRS::create(verticalCRS1, verticalCRS2, vtransformation)});
    auto compound2 = CompoundCRS::create(
        PropertyMap(),
        std::vector<CRSNNPtr>{createUTM32_WGS84(), verticalCRS2});

    auto op = CoordinateOperationFactory::create()->createOperation(compound1,
                                                                    compound2);
    ASSERT_TRUE(op != nullptr);
    EXPECT_EQ(op->exportToPROJString(PROJStringFormatter::create()),
              "+proj=pipeline +step +inv +proj=utm +zone=31 +ellps=WGS84 +step "
              "+proj=vgridshift +grids=bla.gtx +multiplier=0.001 +step "
              "+proj=utm +zone=32 "
              "+ellps=WGS84");

    auto opInverse = CoordinateOperationFactory::create()->createOperation(
        compound2, compound1);
    ASSERT_TRUE(opInverse != nullptr);
    EXPECT_TRUE(opInverse->inverse()->isEquivalentTo(NN_CHECK_ASSERT(op)));
}
