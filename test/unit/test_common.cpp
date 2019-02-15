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
#include "proj/metadata.hpp"
#include "proj/util.hpp"

using namespace osgeo::proj::common;
using namespace osgeo::proj::metadata;
using namespace osgeo::proj::operation;
using namespace osgeo::proj::util;

// ---------------------------------------------------------------------------

TEST(common, unit_of_measure) {
    EXPECT_EQ(UnitOfMeasure::METRE.name(), "metre");

    EXPECT_EQ(UnitOfMeasure::METRE.conversionToSI(), 1.0);
    EXPECT_EQ(UnitOfMeasure::METRE.type(), UnitOfMeasure::Type::LINEAR);

    EXPECT_EQ(UnitOfMeasure::DEGREE.name(), "degree");
    EXPECT_EQ(UnitOfMeasure::DEGREE.conversionToSI(), 0.017453292519943295);
    EXPECT_EQ(UnitOfMeasure::DEGREE.type(), UnitOfMeasure::Type::ANGULAR);

    EXPECT_EQ(UnitOfMeasure::RADIAN.name(), "radian");
    EXPECT_EQ(UnitOfMeasure::RADIAN.conversionToSI(), 1.0);
    EXPECT_EQ(UnitOfMeasure::RADIAN.type(), UnitOfMeasure::Type::ANGULAR);

    EXPECT_EQ(Length(2.0, UnitOfMeasure("km", 1000.0))
                  .convertToUnit(UnitOfMeasure::METRE),
              2000.0);

    EXPECT_EQ(
        Angle(2.0, UnitOfMeasure::DEGREE).convertToUnit(UnitOfMeasure::RADIAN),
        2 * 0.017453292519943295);

    EXPECT_EQ(Angle(2.5969213, UnitOfMeasure::GRAD)
                  .convertToUnit(UnitOfMeasure::DEGREE),
              2.5969213 / 100.0 * 90.0);
}

// ---------------------------------------------------------------------------

TEST(common, measure) { EXPECT_TRUE(Measure(1.0) == Measure(1.0)); }

// ---------------------------------------------------------------------------

TEST(common, identifiedobject_empty) {
    PropertyMap properties;
    auto obj = OperationParameter::create(properties);
    EXPECT_TRUE(obj->name()->code().empty());
    EXPECT_TRUE(obj->identifiers().empty());
    EXPECT_TRUE(obj->aliases().empty());
    EXPECT_TRUE(obj->remarks().empty());
    EXPECT_TRUE(!obj->isDeprecated());
    EXPECT_TRUE(obj->alias().empty());
}

// ---------------------------------------------------------------------------

TEST(common, identifiedobject) {
    PropertyMap properties;
    properties.set(IdentifiedObject::NAME_KEY, "name");
    properties.set(IdentifiedObject::IDENTIFIERS_KEY,
                   Identifier::create("identifier_code"));
    properties.set(IdentifiedObject::ALIAS_KEY, "alias");
    properties.set(IdentifiedObject::REMARKS_KEY, "remarks");
    properties.set(IdentifiedObject::DEPRECATED_KEY, true);
    auto obj = OperationParameter::create(properties);
    EXPECT_EQ(*(obj->name()->description()), "name");
    ASSERT_EQ(obj->identifiers().size(), 1U);
    EXPECT_EQ(obj->identifiers()[0]->code(), "identifier_code");
    ASSERT_EQ(obj->aliases().size(), 1U);
    EXPECT_EQ(obj->aliases()[0]->toString(), "alias");
    EXPECT_EQ(obj->remarks(), "remarks");
    EXPECT_TRUE(obj->isDeprecated());
}

// ---------------------------------------------------------------------------

TEST(common, identifiedobject_name_invalid_type_integer) {
    PropertyMap properties;
    properties.set(IdentifiedObject::NAME_KEY, 123);
    ASSERT_THROW(OperationParameter::create(properties),
                 InvalidValueTypeException);
}

// ---------------------------------------------------------------------------

TEST(common, identifiedobject_name_invalid_type_citation) {
    PropertyMap properties;
    properties.set(IdentifiedObject::NAME_KEY,
                   nn_make_shared<Citation>("invalid"));
    ASSERT_THROW(OperationParameter::create(properties),
                 InvalidValueTypeException);
}

// ---------------------------------------------------------------------------

TEST(common, identifiedobject_identifier_invalid_type) {
    PropertyMap properties;
    properties.set(IdentifiedObject::IDENTIFIERS_KEY, "string not allowed");
    ASSERT_THROW(OperationParameter::create(properties),
                 InvalidValueTypeException);
}

// ---------------------------------------------------------------------------

TEST(common, identifiedobject_identifier_array_of_identifier) {
    PropertyMap properties;
    auto array = ArrayOfBaseObject::create();
    array->add(Identifier::create("identifier_code1"));
    array->add(Identifier::create("identifier_code2"));
    properties.set(IdentifiedObject::IDENTIFIERS_KEY, array);
    auto obj = OperationParameter::create(properties);
    ASSERT_EQ(obj->identifiers().size(), 2U);
    EXPECT_EQ(obj->identifiers()[0]->code(), "identifier_code1");
    EXPECT_EQ(obj->identifiers()[1]->code(), "identifier_code2");
}

// ---------------------------------------------------------------------------

TEST(common, identifiedobject_identifier_array_of_invalid_type) {
    PropertyMap properties;
    auto array = ArrayOfBaseObject::create();
    array->add(nn_make_shared<Citation>("unexpected type"));
    properties.set(IdentifiedObject::IDENTIFIERS_KEY, array);
    ASSERT_THROW(OperationParameter::create(properties),
                 InvalidValueTypeException);
}

// ---------------------------------------------------------------------------

TEST(common, identifiedobject_alias_array_of_string) {
    PropertyMap properties;
    properties.set(IdentifiedObject::ALIAS_KEY,
                   std::vector<std::string>{"alias1", "alias2"});
    auto obj = OperationParameter::create(properties);
    ASSERT_EQ(obj->aliases().size(), 2U);
    EXPECT_EQ(obj->aliases()[0]->toString(), "alias1");
    EXPECT_EQ(obj->aliases()[1]->toString(), "alias2");
}

// ---------------------------------------------------------------------------

TEST(common, identifiedobject_alias_invalid_type) {
    PropertyMap properties;
    properties.set(IdentifiedObject::ALIAS_KEY,
                   nn_make_shared<Citation>("unexpected type"));
    ASSERT_THROW(OperationParameter::create(properties),
                 InvalidValueTypeException);
}

// ---------------------------------------------------------------------------

TEST(common, identifiedobject_alias_array_of_invalid_type) {
    PropertyMap properties;
    auto array = ArrayOfBaseObject::create();
    array->add(nn_make_shared<Citation>("unexpected type"));
    properties.set(IdentifiedObject::ALIAS_KEY, array);
    ASSERT_THROW(OperationParameter::create(properties),
                 InvalidValueTypeException);
}

// ---------------------------------------------------------------------------

TEST(common, DataEpoch) {
    DataEpoch epochSrc(Measure(2010.5, UnitOfMeasure::YEAR));
    DataEpoch epoch(epochSrc);
    EXPECT_EQ(epoch.coordinateEpoch().value(), 2010.5);
    EXPECT_EQ(epoch.coordinateEpoch().unit(), UnitOfMeasure::YEAR);
}
