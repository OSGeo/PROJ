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
#include "proj/crs.hpp"
#include "proj/internal.hpp"
#include "proj/io.hpp"
#include "proj/metadata.hpp"
#include "proj/util.hpp"

#include <string>
#include <vector>

using namespace osgeo::proj::common;
using namespace osgeo::proj::crs;
using namespace osgeo::proj::io;
using namespace osgeo::proj::internal;
using namespace osgeo::proj::metadata;
using namespace osgeo::proj::operation;
using namespace osgeo::proj::util;

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
}
