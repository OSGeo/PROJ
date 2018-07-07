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
#include "proj/internal.hpp"
#include "proj/io.hpp"
#include "proj/metadata.hpp"
#include "proj/util.hpp"

#include <string>
#include <vector>

using namespace osgeo::proj::common;
using namespace osgeo::proj::crs;
using namespace osgeo::proj::cs;
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
        PropertyMap(), GeographicCRS::EPSG_4326, GeographicCRS::EPSG_4979, 1.0,
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
}

// ---------------------------------------------------------------------------

TEST(operation, transformation_createPositionVector) {

    auto transf = Transformation::createPositionVector(
        PropertyMap(), GeographicCRS::EPSG_4326, GeographicCRS::EPSG_4979, 1.0,
        2.0, 3.0, 4.0, 5.0, 6.0, 7.0, std::vector<PositionalAccuracyNNPtr>());

    auto expected = std::vector<double>{1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0};
    EXPECT_EQ(transf->getTOWGS84Parameters(), expected);

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
        PropertyMap(), GeographicCRS::EPSG_4326, GeographicCRS::EPSG_4979, 1.0,
        2.0, 3.0, -4.0, -5.0, -6.0, 7.0,
        std::vector<PositionalAccuracyNNPtr>());

    auto params = transf->getTOWGS84Parameters();
    auto expected = std::vector<double>{1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0};
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
        GeographicCRS::EPSG_4326, GeographicCRS::EPSG_4979, nullptr,
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
              "    TARGETCRS[\n"
              "        GEODCRS[\"WGS 84\",\n"
              "            DATUM[\"WGS_1984\",\n"
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

TEST(operation, utm_to_PROJ_string) {
    EXPECT_EQ(Conversion::createUTM(PropertyMap(), 1, false)
                  ->exportToPROJString(PROJStringFormatter::create()),
              "+proj=utm +zone=1 +south");
}

// ---------------------------------------------------------------------------

TEST(operation, tmerc_to_PROJ_string) {
    EXPECT_EQ(Conversion::createTM(PropertyMap(), Angle(1), Angle(2), Scale(3),
                                   Length(4), Length(5))
                  ->exportToPROJString(PROJStringFormatter::create()),
              "+proj=tmerc +lat_0=1 +lon_0=2 +k_0=3 +x_0=4 +y_0=5");
}

// ---------------------------------------------------------------------------

TEST(operation, lcc1sp_to_PROJ_string) {
    EXPECT_EQ(Conversion::createLCC_1SP(PropertyMap(), Angle(1), Angle(2),
                                        Scale(3), Length(4), Length(5))
                  ->exportToPROJString(PROJStringFormatter::create()),
              "+proj=lcc +lat_1=1 +lat_0=1 +lon_0=2 +k_0=3 +x_0=4 +y_0=5");
}

// ---------------------------------------------------------------------------

TEST(operation, nzmg_to_PROJ_string) {
    EXPECT_EQ(Conversion::createNZMG(PropertyMap(), Angle(1), Angle(2),
                                     Length(4), Length(5))
                  ->exportToPROJString(PROJStringFormatter::create()),
              "+proj=nzmg +lat_0=1 +lon_0=2 +x_0=4 +y_0=5");
}

// ---------------------------------------------------------------------------

TEST(operation, conversion_inverse) {
    auto conv = Conversion::createTM(PropertyMap(), Angle(1), Angle(2),
                                     Scale(3), Length(4), Length(5));
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
    auto conv = SingleOperation::createPROJBased("+proj=merc");

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
               "+ellps=clrk80ign +pm=paris +step +inv +proj=longlat "
               "+ellps=WGS84 +step +inv +proj=axisswap +order=2,1";
    EXPECT_EQ(SingleOperation::createPROJBased(str)->exportToPROJString(
                  PROJStringFormatter::create()),
              str);

    EXPECT_THROW(SingleOperation::createPROJBased("+inv")->exportToPROJString(
                     PROJStringFormatter::create()),
                 FormattingException);
    EXPECT_THROW(SingleOperation::createPROJBased("foo")->exportToPROJString(
                     PROJStringFormatter::create()),
                 FormattingException);
}

// ---------------------------------------------------------------------------

TEST(operation, PROJ_based_empty) {
    auto conv = SingleOperation::createPROJBased("");

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
    EXPECT_EQ(op->exportToPROJString(PROJStringFormatter::create()),
              "+proj=pipeline +step +proj=unitconvert +xy_in=grad +xy_out=rad "
              "+step +proj=axisswap +order=2,1 +step +proj=longlat "
              "+ellps=clrk80ign +pm=paris +step +proj=axisswap +order=2,1 "
              "+step +proj=unitconvert +xy_in=rad +xy_out=deg");
}

// ---------------------------------------------------------------------------

TEST(operation, geogCRS_to_geogCRS_noop) {

    auto op = CoordinateOperationFactory::create()->createOperation(
        GeographicCRS::EPSG_4326, GeographicCRS::EPSG_4326);
    EXPECT_EQ(op->exportToPROJString(PROJStringFormatter::create()), "");
}

// ---------------------------------------------------------------------------

TEST(operation, geogCRS_to_projCRS) {

    auto projCRS = ProjectedCRS::create(
        PropertyMap(), GeographicCRS::EPSG_4326,
        Conversion::createUTM(PropertyMap(), 31, true),
        CartesianCS::createEastingNorthing(UnitOfMeasure::METRE));

    auto op = CoordinateOperationFactory::create()->createOperation(
        GeographicCRS::EPSG_4326, projCRS);
    EXPECT_EQ(op->exportToPROJString(PROJStringFormatter::create()),
              "+proj=pipeline +step +proj=unitconvert +xy_in=deg +xy_out=rad "
              "+step +proj=axisswap +order=2,1 +step +proj=utm +zone=31 "
              "+ellps=WGS84");
}

// ---------------------------------------------------------------------------

TEST(operation, geogCRS_different_from_baseCRS_to_projCRS) {

    auto projCRS = ProjectedCRS::create(
        PropertyMap(), GeographicCRS::EPSG_4326,
        Conversion::createUTM(PropertyMap(), 31, true),
        CartesianCS::createEastingNorthing(UnitOfMeasure::METRE));

    auto op = CoordinateOperationFactory::create()->createOperation(
        GeographicCRS::EPSG_4807, projCRS);
    EXPECT_EQ(op->exportToPROJString(PROJStringFormatter::create()),
              "+proj=pipeline +step +proj=unitconvert +xy_in=grad +xy_out=rad "
              "+step +proj=axisswap +order=2,1 +step +proj=longlat "
              "+ellps=clrk80ign +pm=paris +step +proj=utm +zone=31 "
              "+ellps=WGS84");
}

// ---------------------------------------------------------------------------

TEST(operation, projCRS_to_geogCRS) {

    auto projCRS = ProjectedCRS::create(
        PropertyMap(), GeographicCRS::EPSG_4326,
        Conversion::createUTM(PropertyMap(), 31, true),
        CartesianCS::createEastingNorthing(UnitOfMeasure::METRE));

    auto op = CoordinateOperationFactory::create()->createOperation(
        projCRS, GeographicCRS::EPSG_4326);
    EXPECT_EQ(op->exportToPROJString(PROJStringFormatter::create()),
              "+proj=pipeline +step +inv +proj=utm +zone=31 +ellps=WGS84 +step "
              "+proj=axisswap +order=2,1 +step +proj=unitconvert +xy_in=rad "
              "+xy_out=deg");
}

// ---------------------------------------------------------------------------

TEST(operation, projCRS_to_projCRS) {

    auto utm31 = ProjectedCRS::create(
        PropertyMap(), GeographicCRS::EPSG_4326,
        Conversion::createUTM(PropertyMap(), 31, true),
        CartesianCS::createEastingNorthing(UnitOfMeasure::METRE));
    auto utm32 = ProjectedCRS::create(
        PropertyMap(), GeographicCRS::EPSG_4326,
        Conversion::createUTM(PropertyMap(), 32, true),
        CartesianCS::createEastingNorthing(UnitOfMeasure::METRE));

    auto op =
        CoordinateOperationFactory::create()->createOperation(utm31, utm32);
    EXPECT_EQ(op->exportToPROJString(PROJStringFormatter::create()),
              "+proj=pipeline +step +inv +proj=utm +zone=31 +ellps=WGS84 +step "
              "+proj=utm +zone=32 +ellps=WGS84");
}

// ---------------------------------------------------------------------------

TEST(operation, projCRS_to_projCRS_different_baseCRS) {

    auto utm31 = ProjectedCRS::create(
        PropertyMap(), GeographicCRS::EPSG_4326,
        Conversion::createUTM(PropertyMap(), 31, true),
        CartesianCS::createEastingNorthing(UnitOfMeasure::METRE));
    auto utm32 = ProjectedCRS::create(
        PropertyMap(), GeographicCRS::EPSG_4807,
        Conversion::createUTM(PropertyMap(), 32, true),
        CartesianCS::createEastingNorthing(UnitOfMeasure::METRE));

    auto op =
        CoordinateOperationFactory::create()->createOperation(utm31, utm32);
    EXPECT_EQ(op->exportToPROJString(PROJStringFormatter::create()),
              "+proj=pipeline +step +inv +proj=utm +zone=31 +ellps=WGS84 +step "
              "+inv +proj=longlat +ellps=clrk80ign +pm=paris +step +proj=utm "
              "+zone=32 +ellps=clrk80ign +pm=paris");
}
