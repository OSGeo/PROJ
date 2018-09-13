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

#include "proj/io.hpp"
#include "proj/metadata.hpp"
#include "proj/util.hpp"

using namespace osgeo::proj::io;
using namespace osgeo::proj::metadata;
using namespace osgeo::proj::util;

// ---------------------------------------------------------------------------

TEST(metadata, citation) {
    Citation c("my citation");
    Citation c2(c);
    ASSERT_TRUE(c2.title().has_value());
    ASSERT_EQ(*(c2.title()), "my citation");
}

// ---------------------------------------------------------------------------

TEST(metadata, extent) {
    auto extent = Extent::create(
        optional<std::string>(), std::vector<GeographicExtentNNPtr>(),
        std::vector<VerticalExtentNNPtr>(), std::vector<TemporalExtentNNPtr>());
    // TODO
}

// ---------------------------------------------------------------------------

TEST(metadata, identifier_empty) {
    auto id(Identifier::create());
    Identifier id2(*id);
    ASSERT_TRUE(!id2.authority().has_value());
    ASSERT_TRUE(id2.code().empty());
    ASSERT_TRUE(!id2.codeSpace().has_value());
    ASSERT_TRUE(!id2.version().has_value());
    ASSERT_TRUE(!id2.description().has_value());
}

// ---------------------------------------------------------------------------

TEST(metadata, identifier_properties) {
    PropertyMap properties;
    properties.set(Identifier::AUTHORITY_KEY, "authority");
    properties.set(Identifier::CODESPACE_KEY, "codespace");
    properties.set(Identifier::VERSION_KEY, "version");
    properties.set(Identifier::DESCRIPTION_KEY, "description");
    auto id(Identifier::create("my code", properties));
    Identifier id2(*id);
    ASSERT_TRUE(id2.authority().has_value());
    ASSERT_EQ(*(id2.authority()->title()), "authority");
    ASSERT_EQ(id2.code(), "my code");
    ASSERT_TRUE(id2.codeSpace().has_value());
    ASSERT_EQ(*(id2.codeSpace()), "codespace");
    ASSERT_TRUE(id2.version().has_value());
    ASSERT_EQ(*(id2.version()), "version");
    ASSERT_TRUE(id2.description().has_value());
    ASSERT_EQ(*(id2.description()), "description");
}

// ---------------------------------------------------------------------------

TEST(metadata, identifier_code_integer) {
    PropertyMap properties;
    properties.set(Identifier::CODE_KEY, 1234);
    auto id(Identifier::create(std::string(), properties));
    ASSERT_EQ(id->code(), "1234");
}

// ---------------------------------------------------------------------------

TEST(metadata, identifier_code_string) {
    PropertyMap properties;
    properties.set(Identifier::CODE_KEY, "1234");
    auto id(Identifier::create(std::string(), properties));
    ASSERT_EQ(id->code(), "1234");
}

// ---------------------------------------------------------------------------

TEST(metadata, identifier_code_invalid_type) {
    PropertyMap properties;
    properties.set(Identifier::CODE_KEY, true);
    ASSERT_THROW(Identifier::create(std::string(), properties),
                 InvalidValueTypeException);
}

// ---------------------------------------------------------------------------

TEST(metadata, identifier_authority_citation) {
    PropertyMap properties;
    properties.set(Identifier::AUTHORITY_KEY,
                   nn_make_shared<Citation>("authority"));
    auto id(Identifier::create(std::string(), properties));
    ASSERT_TRUE(id->authority().has_value());
    ASSERT_EQ(*(id->authority()->title()), "authority");
}

// ---------------------------------------------------------------------------

TEST(metadata, identifier_authority_invalid_type) {
    PropertyMap properties;
    properties.set(Identifier::AUTHORITY_KEY, true);
    ASSERT_THROW(Identifier::create(std::string(), properties),
                 InvalidValueTypeException);
}

// ---------------------------------------------------------------------------

TEST(metadata, id) {
    auto in_wkt = "ID[\"EPSG\",4946,1.5,\n"
                  "    CITATION[\"my citation\"],\n"
                  "    URI[\"urn:ogc:def:crs:EPSG::4946\"]]";
    auto id =
        nn_dynamic_pointer_cast<Identifier>(WKTParser().createFromWKT(in_wkt));
    ASSERT_TRUE(id != nullptr);

    EXPECT_TRUE(id->authority().has_value());
    EXPECT_EQ(*(id->authority()->title()), "my citation");
    EXPECT_EQ(*(id->codeSpace()), "EPSG");
    EXPECT_EQ(id->code(), "4946");
    EXPECT_TRUE(id->version().has_value());
    EXPECT_EQ(*(id->version()), "1.5");
    EXPECT_TRUE(id->uri().has_value());
    EXPECT_EQ(*(id->uri()), "urn:ogc:def:crs:EPSG::4946");

    auto got_wkt = id->exportToWKT(WKTFormatter::create());
    EXPECT_EQ(got_wkt, in_wkt);
}

// ---------------------------------------------------------------------------

TEST(metadata, Identifier_isEquivalentName) {
    EXPECT_TRUE(Identifier::isEquivalentName("Central_Meridian",
                                             "Central_- ()/Meridian"));
}
