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

static bool equals(ExtentNNPtr extent1, ExtentNNPtr extent2) {
    return extent1->contains(extent2) && extent2->contains(extent1);
}

static bool equals(GeographicExtentNNPtr extent1,
                   GeographicExtentNNPtr extent2) {
    return extent1->contains(extent2) && extent2->contains(extent1);
}

static GeographicExtentNNPtr getBBox(ExtentNNPtr extent) {
    assert(extent->geographicElements().size() == 1);
    return extent->geographicElements()[0];
}

TEST(metadata, extent) {
    Extent::create(
        optional<std::string>(), std::vector<GeographicExtentNNPtr>(),
        std::vector<VerticalExtentNNPtr>(), std::vector<TemporalExtentNNPtr>());

    auto world = Extent::createFromBBOX(-180, -90, 180, 90);
    EXPECT_TRUE(world->isEquivalentTo(world.get()));
    EXPECT_TRUE(world->contains(world));

    auto west_hemisphere = Extent::createFromBBOX(-180, -90, 0, 90);
    EXPECT_TRUE(!world->isEquivalentTo(west_hemisphere.get()));
    EXPECT_TRUE(world->contains(west_hemisphere));
    EXPECT_TRUE(!west_hemisphere->contains(world));

    auto world_inter_world = world->intersection(world);
    ASSERT_TRUE(world_inter_world != nullptr);
    EXPECT_TRUE(equals(NN_CHECK_ASSERT(world_inter_world), world));

    auto france = Extent::createFromBBOX(-5, 40, 12, 51);
    EXPECT_TRUE(france->contains(france));
    EXPECT_TRUE(world->contains(france));
    EXPECT_TRUE(!france->contains(
        world)); // We are only speaking about geography here ;-)
    EXPECT_TRUE(world->intersects(france));
    EXPECT_TRUE(france->intersects(world));

    auto france_inter_france = france->intersection(france);
    ASSERT_TRUE(france_inter_france != nullptr);
    EXPECT_TRUE(equals(NN_CHECK_ASSERT(france_inter_france), france));

    auto france_inter_world = france->intersection(world);
    ASSERT_TRUE(france_inter_world != nullptr);
    EXPECT_TRUE(equals(NN_CHECK_ASSERT(france_inter_world), france));

    auto world_inter_france = world->intersection(france);
    ASSERT_TRUE(world_inter_france != nullptr);
    EXPECT_TRUE(equals(NN_CHECK_ASSERT(world_inter_france), france));

    auto france_shifted =
        Extent::createFromBBOX(-5 + 5, 40 + 5, 12 + 5, 51 + 5);
    EXPECT_TRUE(france->intersects(france_shifted));
    EXPECT_TRUE(france_shifted->intersects(france));
    EXPECT_TRUE(!france->contains(france_shifted));
    EXPECT_TRUE(!france_shifted->contains(france));

    auto europe = Extent::createFromBBOX(-30, 25, 30, 70);
    EXPECT_TRUE(europe->contains(france));
    EXPECT_TRUE(!france->contains(europe));

    auto france_inter_europe = france->intersection(europe);
    ASSERT_TRUE(france_inter_europe != nullptr);
    EXPECT_TRUE(equals(NN_CHECK_ASSERT(france_inter_europe), france));

    auto europe_intersects_france = europe->intersection(france);
    ASSERT_TRUE(europe_intersects_france != nullptr);
    EXPECT_TRUE(equals(NN_CHECK_ASSERT(europe_intersects_france), france));

    auto nz = Extent::createFromBBOX(155.0, -60.0, -170.0, -25.0);
    EXPECT_TRUE(nz->contains(nz));
    EXPECT_TRUE(world->contains(nz));
    EXPECT_TRUE(nz->intersects(world));
    EXPECT_TRUE(world->intersects(nz));
    EXPECT_TRUE(!nz->contains(world));
    EXPECT_TRUE(!nz->contains(france));
    EXPECT_TRUE(!france->contains(nz));
    EXPECT_TRUE(!nz->intersects(france));
    EXPECT_TRUE(!france->intersects(nz));

    {
        auto nz_inter_world = nz->intersection(world);
        ASSERT_TRUE(nz_inter_world != nullptr);
        EXPECT_TRUE(equals(NN_CHECK_ASSERT(nz_inter_world), nz));
    }

    {
        auto nz_inter_world = getBBox(nz)->intersection(getBBox(world));
        ASSERT_TRUE(nz_inter_world != nullptr);
        EXPECT_TRUE(equals(NN_CHECK_ASSERT(nz_inter_world), getBBox(nz)));
    }

    {
        auto world_inter_nz = world->intersection(nz);
        ASSERT_TRUE(world_inter_nz != nullptr);
        EXPECT_TRUE(equals(NN_CHECK_ASSERT(world_inter_nz), nz));
    }

    {
        auto world_inter_nz = getBBox(world)->intersection(getBBox(nz));
        ASSERT_TRUE(world_inter_nz != nullptr);
        EXPECT_TRUE(equals(NN_CHECK_ASSERT(world_inter_nz), getBBox(nz)));
    }

    EXPECT_TRUE(nz->intersection(france) == nullptr);
    EXPECT_TRUE(france->intersection(nz) == nullptr);

    auto bbox_antimeridian_north =
        Extent::createFromBBOX(155.0, 10.0, -170.0, 30.0);
    EXPECT_TRUE(!nz->contains(bbox_antimeridian_north));
    EXPECT_TRUE(!bbox_antimeridian_north->contains(nz));
    EXPECT_TRUE(!nz->intersects(bbox_antimeridian_north));
    EXPECT_TRUE(!bbox_antimeridian_north->intersects(nz));
    EXPECT_TRUE(!nz->intersection(bbox_antimeridian_north));
    EXPECT_TRUE(!bbox_antimeridian_north->intersection(nz));

    auto nz_pos_long = Extent::createFromBBOX(155.0, -60.0, 180.0, -25.0);
    EXPECT_TRUE(nz->contains(nz_pos_long));
    EXPECT_TRUE(!nz_pos_long->contains(nz));
    EXPECT_TRUE(nz->intersects(nz_pos_long));
    EXPECT_TRUE(nz_pos_long->intersects(nz));
    auto nz_inter_nz_pos_long = nz->intersection(nz_pos_long);
    ASSERT_TRUE(nz_inter_nz_pos_long != nullptr);
    EXPECT_TRUE(equals(NN_CHECK_ASSERT(nz_inter_nz_pos_long), nz_pos_long));
    auto nz_pos_long_inter_nz = nz_pos_long->intersection(nz);
    ASSERT_TRUE(nz_pos_long_inter_nz != nullptr);
    EXPECT_TRUE(equals(NN_CHECK_ASSERT(nz_pos_long_inter_nz), nz_pos_long));

    auto nz_neg_long = Extent::createFromBBOX(-180.0, -60.0, -170.0, -25.0);
    EXPECT_TRUE(nz->contains(nz_neg_long));
    EXPECT_TRUE(!nz_neg_long->contains(nz));
    EXPECT_TRUE(nz->intersects(nz_neg_long));
    EXPECT_TRUE(nz_neg_long->intersects(nz));
    auto nz_inter_nz_neg_long = nz->intersection(nz_neg_long);
    ASSERT_TRUE(nz_inter_nz_neg_long != nullptr);
    EXPECT_TRUE(equals(NN_CHECK_ASSERT(nz_inter_nz_neg_long), nz_neg_long));
    auto nz_neg_long_inter_nz = nz_neg_long->intersection(nz);
    ASSERT_TRUE(nz_neg_long_inter_nz != nullptr);
    EXPECT_TRUE(equals(NN_CHECK_ASSERT(nz_neg_long_inter_nz), nz_neg_long));

    auto nz_smaller = Extent::createFromBBOX(160, -55.0, -175.0, -30.0);
    EXPECT_TRUE(nz->contains(nz_smaller));
    EXPECT_TRUE(!nz_smaller->contains(nz));

    auto nz_pos_long_shifted_west =
        Extent::createFromBBOX(150.0, -60.0, 175.0, -25.0);
    EXPECT_TRUE(!nz->contains(nz_pos_long_shifted_west));
    EXPECT_TRUE(!nz_pos_long_shifted_west->contains(nz));
    EXPECT_TRUE(nz->intersects(nz_pos_long_shifted_west));
    EXPECT_TRUE(nz_pos_long_shifted_west->intersects(nz));

    auto nz_smaller_shifted = Extent::createFromBBOX(165, -60.0, -170.0, -25.0);
    EXPECT_TRUE(!nz_smaller->contains(nz_smaller_shifted));
    EXPECT_TRUE(!nz_smaller_shifted->contains(nz_smaller));
    EXPECT_TRUE(nz_smaller->intersects(nz_smaller_shifted));
    EXPECT_TRUE(nz_smaller_shifted->intersects(nz_smaller));

    auto nz_shifted = Extent::createFromBBOX(165.0, -60.0, -160.0, -25.0);
    auto nz_intersect_nz_shifted = nz->intersection(nz_shifted);
    ASSERT_TRUE(nz_intersect_nz_shifted != nullptr);
    EXPECT_TRUE(equals(NN_CHECK_ASSERT(nz_intersect_nz_shifted),
                       Extent::createFromBBOX(165, -60.0, -170.0, -25.0)));

    auto nz_inter_nz_smaller = nz->intersection(nz_smaller);
    ASSERT_TRUE(nz_inter_nz_smaller != nullptr);
    EXPECT_TRUE(equals(NN_CHECK_ASSERT(nz_inter_nz_smaller), nz_smaller));

    auto nz_smaller_inter_nz = nz_smaller->intersection(nz);
    ASSERT_TRUE(nz_smaller_inter_nz != nullptr);
    EXPECT_TRUE(equals(NN_CHECK_ASSERT(nz_smaller_inter_nz), nz_smaller));

    auto world_smaller = Extent::createFromBBOX(-179, -90, 179, 90);
    EXPECT_TRUE(!world_smaller->contains(nz));
    EXPECT_TRUE(!nz->contains(world_smaller));

    auto nz_inter_world_smaller = nz->intersection(world_smaller);
    ASSERT_TRUE(nz_inter_world_smaller != nullptr);
    EXPECT_TRUE(equals(NN_CHECK_ASSERT(nz_inter_world_smaller),
                       Extent::createFromBBOX(155, -60, 179, -25)));

    auto world_smaller_inter_nz = world_smaller->intersection(nz);
    ASSERT_TRUE(world_smaller_inter_nz != nullptr);
    EXPECT_TRUE(equals(NN_CHECK_ASSERT(world_smaller_inter_nz),
                       Extent::createFromBBOX(155, -60, 179, -25)));

    auto world_smaller_east = Extent::createFromBBOX(-179, -90, 150, 90);
    EXPECT_TRUE(!world_smaller_east->contains(nz));
    EXPECT_TRUE(!nz->contains(world_smaller_east));

    auto nz_inter_world_smaller_east = nz->intersection(world_smaller_east);
    ASSERT_TRUE(nz_inter_world_smaller_east != nullptr);
    EXPECT_EQ(nn_dynamic_pointer_cast<GeographicBoundingBox>(
                  nz_inter_world_smaller_east->geographicElements()[0])
                  ->westBoundLongitude(),
              -179);
    EXPECT_EQ(nn_dynamic_pointer_cast<GeographicBoundingBox>(
                  nz_inter_world_smaller_east->geographicElements()[0])
                  ->eastBoundLongitude(),
              -170);
    EXPECT_TRUE(equals(NN_CHECK_ASSERT(nz_inter_world_smaller_east),
                       Extent::createFromBBOX(-179, -60, -170, -25)));

    auto world_smaller_east_inter_nz = world_smaller_east->intersection(nz);
    ASSERT_TRUE(world_smaller_east_inter_nz != nullptr);
    EXPECT_EQ(nn_dynamic_pointer_cast<GeographicBoundingBox>(
                  world_smaller_east_inter_nz->geographicElements()[0])
                  ->westBoundLongitude(),
              -179);
    EXPECT_EQ(nn_dynamic_pointer_cast<GeographicBoundingBox>(
                  world_smaller_east_inter_nz->geographicElements()[0])
                  ->eastBoundLongitude(),
              -170);
    EXPECT_TRUE(equals(NN_CHECK_ASSERT(world_smaller_east_inter_nz),
                       Extent::createFromBBOX(-179, -60, -170, -25)));

    auto east_hemisphere = Extent::createFromBBOX(0, -90, 180, 90);
    auto east_hemisphere_inter_nz = east_hemisphere->intersection(nz);
    ASSERT_TRUE(east_hemisphere_inter_nz != nullptr);
    EXPECT_TRUE(equals(NN_CHECK_ASSERT(east_hemisphere_inter_nz),
                       Extent::createFromBBOX(155.0, -60.0, 180.0, -25.0)));

    auto minus_180_to_156 = Extent::createFromBBOX(-180, -90, 156, 90);
    auto minus_180_to_156_inter_nz = minus_180_to_156->intersection(nz);
    ASSERT_TRUE(minus_180_to_156_inter_nz != nullptr);
    EXPECT_TRUE(equals(NN_CHECK_ASSERT(minus_180_to_156_inter_nz),
                       Extent::createFromBBOX(-180.0, -60.0, -170.0, -25.0)));
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

    auto got_wkt = id->exportToWKT(WKTFormatter::create().get());
    EXPECT_EQ(got_wkt, in_wkt);
}

// ---------------------------------------------------------------------------

TEST(metadata, Identifier_isEquivalentName) {
    EXPECT_TRUE(Identifier::isEquivalentName("Central_Meridian",
                                             "Central_- ()/Meridian"));

    EXPECT_TRUE(Identifier::isEquivalentName("\xc3\xa1", "a"));

    EXPECT_TRUE(Identifier::isEquivalentName("a", "\xc3\xa1"));

    EXPECT_TRUE(Identifier::isEquivalentName("\xc3\xa4", "\xc3\xa1"));
}
