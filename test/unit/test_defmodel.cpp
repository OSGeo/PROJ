/******************************************************************************
 *
 * Project:  PROJ
 * Purpose:  Test deformation model
 * Author:   Even Rouault <even dot rouault at spatialys dot com>
 *
 ******************************************************************************
 * Copyright (c) 2020, Even Rouault <even dot rouault at spatialys dot com>
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

// to be able to use internal::toString
#define FROM_PROJ_CPP
#include "proj/internal/internal.hpp"

#include "proj.h"
#include "proj_internal.h"

// Silence C4702 (unreachable code) due to some dummy implementation of the
// interfaces of defmodel.hpp
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4702)
#endif

#define PROJ_COMPILATION
#define DEFORMATON_MODEL_NAMESPACE TestDeformationModel
#include "transformations/defmodel.hpp"

using namespace DEFORMATON_MODEL_NAMESPACE;

namespace {

constexpr double modelMinX = 158;
constexpr double modelMinY = -58;
constexpr double modelMaxX = 194;
constexpr double modelMaxY = -25;

static json getMinValidContent() {
    json j;
    j["file_type"] = "GeoTIFF";
    j["format_version"] = "1.0";
    j["source_crs"] = "EPSG:4959";
    j["target_crs"] = "EPSG:7907";
    j["definition_crs"] = "EPSG:4959";
    j["extent"]["type"] = "bbox";
    j["extent"]
     ["parameters"] = {{"bbox", {modelMinX, modelMinY, modelMaxX, modelMaxY}}};
    j["time_extent"]["first"] = "1900-01-01T00:00:00Z";
    j["time_extent"]["last"] = "2050-01-01T00:00:00Z";
    j["components"] = json::array();

    return j;
}

// ---------------------------------------------------------------------------

constexpr int IDX_CONSTANT = 0;
constexpr int IDX_VELOCITY = 1;
constexpr int IDX_STEP = 2;
constexpr int IDX_REVERSE_STEP = 3;
constexpr int IDX_PIECEWISE = 4;
constexpr int IDX_EXPONENTIAL = 5;

static json getFullValidContent() {
    json j(getMinValidContent());
    j["name"] = "name";
    j["version"] = "version";
    j["publication_date"] = "2018-07-01T00:00:00Z";
    j["license"] = "license";
    j["description"] = "description";
    j["authority"]["name"] = "authority_name";
    j["authority"]["url"] = "authority_url";
    j["authority"]["address"] = "authority_address";
    j["authority"]["email"] = "authority_email";
    j["links"] = {{{"href", "href"},
                   {"rel", "rel"},
                   {"type", "type"},
                   {"title", "title"}}};
    j["reference_epoch"] = "2000-01-01T00:00:00Z";
    j["uncertainty_reference_epoch"] = "2018-12-15T00:00:00Z";
    j["horizontal_offset_method"] = "addition";
    j["horizontal_offset_unit"] = "metre";
    j["vertical_offset_unit"] = "metre";
    j["horizontal_uncertainty_type"] = "circular 95% confidence limit";
    j["horizontal_uncertainty_unit"] = "metre";
    j["vertical_uncertainty_type"] = "95% confidence limit";
    j["vertical_uncertainty_unit"] = "metre";
    j["components"] = {{
        {"description", "description"},
        {"displacement_type", "horizontal"},
        {"uncertainty_type", "none"},
        {"horizontal_uncertainty", 0.01},
        {"vertical_uncertainty", 0.02},
        {"extent",
         {{"type", "bbox"},
          {"parameters",
           {{"bbox", {modelMinX, modelMinY, modelMaxX, modelMaxY}}}}}},
        {"spatial_model",
         {
             {"type", "GeoTIFF"},
             {"interpolation_method", "bilinear"},
             {"filename", "nzgd2000-ndm-grid02.tif"},
             {"md5_checksum", "49fce8ab267be2c8d00d43683060a032"},
         }},
        {"time_function",
         {
             {"type", "constant"}, {"parameters", json::object()},
         }},
    }};

    j["components"].push_back(j["components"][0]);
    j["components"][IDX_VELOCITY]["time_function"] = {
        {"type", "velocity"},
        {"parameters", {{"reference_epoch", "2000-01-01T00:00:00Z"}}},
    };

    j["components"].push_back(j["components"][0]);
    j["components"][IDX_STEP]["time_function"] = {
        {"type", "step"},
        {"parameters", {{"step_epoch", "2000-01-01T00:00:00Z"}}},
    };

    j["components"].push_back(j["components"][0]);
    j["components"][IDX_REVERSE_STEP]["time_function"] = {
        {"type", "reverse_step"},
        {"parameters", {{"step_epoch", "2000-01-01T00:00:00Z"}}},
    };

    j["components"].push_back(j["components"][0]);
    j["components"][IDX_PIECEWISE]["time_function"] = {
        {"type", "piecewise"},
        {"parameters",
         {{"before_first", "zero"},
          {"after_last", "constant"},
          {"model",
           {{{"epoch", "2016-01-01T00:00:00Z"}, {"scale_factor", 0.5}},
            {{"epoch", "2017-01-01T00:00:00Z"}, {"scale_factor", 1.0}},
            {{"epoch", "2017-01-01T00:00:00Z"}, {"scale_factor", 2.0}},
            {{"epoch", "2018-01-01T00:00:00Z"}, {"scale_factor", 1.0}}}}}}};

    j["components"].push_back(j["components"][0]);
    j["components"][IDX_EXPONENTIAL]["time_function"] = {
        {"type", "exponential"},
        {"parameters",
         {
             {"reference_epoch", "2000-01-01T00:00:00Z"},
             {"end_epoch", "2001-01-01T00:00:00Z"},
             {"relaxation_constant", 2.0},
             {"before_scale_factor", 0.0},
             {"initial_scale_factor", 1.0},
             {"final_scale_factor", 3.0},
         }},
    };

    return j;
}

// ---------------------------------------------------------------------------

TEST(defmodel, basic) {
    EXPECT_THROW(MasterFile::parse("foo"), ParsingException);
    EXPECT_THROW(MasterFile::parse("null"), ParsingException);
    EXPECT_THROW(MasterFile::parse("{}"), ParsingException);

    const auto jMinValid(getMinValidContent());
    {
        auto mf = MasterFile::parse(jMinValid.dump());
        EXPECT_EQ(mf->fileType(), "GeoTIFF");
        EXPECT_EQ(mf->formatVersion(), "1.0");
        EXPECT_EQ(mf->sourceCRS(), "EPSG:4959");
        EXPECT_EQ(mf->targetCRS(), "EPSG:7907");
        EXPECT_EQ(mf->definitionCRS(), "EPSG:4959");
        EXPECT_EQ(mf->extent().minx(), modelMinX);
        EXPECT_EQ(mf->extent().miny(), modelMinY);
        EXPECT_EQ(mf->extent().maxx(), modelMaxX);
        EXPECT_EQ(mf->extent().maxy(), modelMaxY);
        EXPECT_EQ(mf->timeExtent().first.toString(), "1900-01-01T00:00:00Z");
        EXPECT_EQ(mf->timeExtent().last.toString(), "2050-01-01T00:00:00Z");
    }

    // Check that removing one of each required key causes an exception
    for (const auto &kv : jMinValid.items()) {
        json jcopy(jMinValid);
        jcopy.erase(kv.key());
        EXPECT_THROW(MasterFile::parse(jcopy.dump()), ParsingException);
    }

    {
        json jcopy(jMinValid);
        jcopy["definition_crs"] = "EPSG:4326";
        EXPECT_THROW(MasterFile::parse(jcopy.dump()), ParsingException);
    }

    {
        json jcopy(jMinValid);
        jcopy["file_type"] = 1;
        EXPECT_THROW(MasterFile::parse(jcopy.dump()), ParsingException);
    }

    {
        json jcopy(jMinValid);
        jcopy["extent"].erase("type");
        EXPECT_THROW(MasterFile::parse(jcopy.dump()), ParsingException);
    }

    {
        json jcopy(jMinValid);
        jcopy["extent"].erase("parameters");
        EXPECT_THROW(MasterFile::parse(jcopy.dump()), ParsingException);
    }

    {
        json jcopy(jMinValid);
        jcopy["extent"]["parameters"].clear();
        EXPECT_THROW(MasterFile::parse(jcopy.dump()), ParsingException);
    }

    {
        json jcopy(jMinValid);
        jcopy["extent"]["parameters"].erase("bbox");
        EXPECT_THROW(MasterFile::parse(jcopy.dump()), ParsingException);
    }

    {
        json jcopy(jMinValid);
        jcopy["extent"]["parameters"]["bbox"] = "foo";
        EXPECT_THROW(MasterFile::parse(jcopy.dump()), ParsingException);
    }

    {
        json jcopy(jMinValid);
        jcopy["extent"]["parameters"]["bbox"] = {0, 1, 2};
        EXPECT_THROW(MasterFile::parse(jcopy.dump()), ParsingException);
    }

    {
        json jcopy(jMinValid);
        jcopy["extent"]["parameters"]["bbox"] = {0, 1, 2, "foo"};
        EXPECT_THROW(MasterFile::parse(jcopy.dump()), ParsingException);
    }

    {
        json jcopy(jMinValid);
        jcopy["time_extent"] = "foo";
        EXPECT_THROW(MasterFile::parse(jcopy.dump()), ParsingException);
    }

    {
        json jcopy(jMinValid);
        jcopy["time_extent"].erase("first");
        EXPECT_THROW(MasterFile::parse(jcopy.dump()), ParsingException);
    }

    {
        json jcopy(jMinValid);
        jcopy["time_extent"].erase("last");
        EXPECT_THROW(MasterFile::parse(jcopy.dump()), ParsingException);
    }
}

// ---------------------------------------------------------------------------

TEST(defmodel, full) {
    const auto jFullValid(getFullValidContent());
    auto mf = MasterFile::parse(jFullValid.dump());
    EXPECT_EQ(mf->name(), "name");
    EXPECT_EQ(mf->version(), "version");
    EXPECT_EQ(mf->publicationDate(), "2018-07-01T00:00:00Z");
    EXPECT_EQ(mf->license(), "license");
    EXPECT_EQ(mf->description(), "description");
    EXPECT_EQ(mf->authority().name, "authority_name");
    EXPECT_EQ(mf->authority().url, "authority_url");
    EXPECT_EQ(mf->authority().address, "authority_address");
    EXPECT_EQ(mf->authority().email, "authority_email");
    EXPECT_EQ(mf->links().size(), 1U);
    EXPECT_EQ(mf->links()[0].href, "href");
    EXPECT_EQ(mf->links()[0].rel, "rel");
    EXPECT_EQ(mf->links()[0].type, "type");
    EXPECT_EQ(mf->links()[0].title, "title");
    EXPECT_EQ(mf->referenceEpoch(), "2000-01-01T00:00:00Z");
    EXPECT_EQ(mf->uncertaintyReferenceEpoch(), "2018-12-15T00:00:00Z");
    EXPECT_EQ(mf->horizontalOffsetUnit(), "metre");
    EXPECT_EQ(mf->verticalOffsetUnit(), "metre");
    EXPECT_EQ(mf->horizontalUncertaintyType(), "circular 95% confidence limit");
    EXPECT_EQ(mf->horizontalUncertaintyUnit(), "metre");
    EXPECT_EQ(mf->verticalUncertaintyType(), "95% confidence limit");
    EXPECT_EQ(mf->verticalUncertaintyUnit(), "metre");
    EXPECT_EQ(mf->horizontalOffsetMethod(), "addition");
    ASSERT_EQ(mf->components().size(), 6U);
    {
        const auto &comp = mf->components()[IDX_CONSTANT];
        EXPECT_EQ(comp.description(), "description");
        EXPECT_EQ(comp.displacementType(), "horizontal");
        EXPECT_EQ(comp.uncertaintyType(), "none");
        EXPECT_EQ(comp.horizontalUncertainty(), 0.01);
        EXPECT_EQ(comp.verticalUncertainty(), 0.02);
        EXPECT_EQ(comp.extent().minx(), modelMinX);
        EXPECT_EQ(comp.extent().miny(), modelMinY);
        EXPECT_EQ(comp.extent().maxx(), modelMaxX);
        EXPECT_EQ(comp.extent().maxy(), modelMaxY);
        EXPECT_EQ(comp.spatialModel().type, "GeoTIFF");
        EXPECT_EQ(comp.spatialModel().interpolationMethod, "bilinear");
        EXPECT_EQ(comp.spatialModel().filename, "nzgd2000-ndm-grid02.tif");
        EXPECT_EQ(comp.spatialModel().md5Checksum,
                  "49fce8ab267be2c8d00d43683060a032");
        ASSERT_NE(comp.timeFunction(), nullptr);
        ASSERT_EQ(comp.timeFunction()->type, "constant");
    }
    {
        const auto &comp = mf->components()[IDX_VELOCITY];
        ASSERT_NE(comp.timeFunction(), nullptr);
        ASSERT_EQ(comp.timeFunction()->type, "velocity");
        const auto velocity =
            static_cast<const Component::VelocityTimeFunction *>(
                comp.timeFunction());
        EXPECT_EQ(velocity->referenceEpoch.toString(), "2000-01-01T00:00:00Z");
    }
    {
        const auto &comp = mf->components()[IDX_STEP];
        ASSERT_NE(comp.timeFunction(), nullptr);
        ASSERT_EQ(comp.timeFunction()->type, "step");
        const auto step = static_cast<const Component::StepTimeFunction *>(
            comp.timeFunction());
        EXPECT_EQ(step->stepEpoch.toString(), "2000-01-01T00:00:00Z");
    }
    {
        const auto &comp = mf->components()[IDX_REVERSE_STEP];
        ASSERT_NE(comp.timeFunction(), nullptr);
        ASSERT_EQ(comp.timeFunction()->type, "reverse_step");
        const auto step =
            static_cast<const Component::ReverseStepTimeFunction *>(
                comp.timeFunction());
        EXPECT_EQ(step->stepEpoch.toString(), "2000-01-01T00:00:00Z");
    }
    {
        const auto &comp = mf->components()[IDX_PIECEWISE];
        ASSERT_NE(comp.timeFunction(), nullptr);
        ASSERT_EQ(comp.timeFunction()->type, "piecewise");
        const auto piecewise =
            static_cast<const Component::PiecewiseTimeFunction *>(
                comp.timeFunction());
        EXPECT_EQ(piecewise->beforeFirst, "zero");
        EXPECT_EQ(piecewise->afterLast, "constant");
        EXPECT_EQ(piecewise->model.size(), 4U);
        EXPECT_EQ(piecewise->model[0].epoch.toString(), "2016-01-01T00:00:00Z");
        EXPECT_EQ(piecewise->model[0].scaleFactor, 0.5);
    }
    {
        const auto &comp = mf->components()[IDX_EXPONENTIAL];
        ASSERT_NE(comp.timeFunction(), nullptr);
        ASSERT_EQ(comp.timeFunction()->type, "exponential");
        const auto exponential =
            static_cast<const Component::ExponentialTimeFunction *>(
                comp.timeFunction());
        EXPECT_EQ(exponential->referenceEpoch.toString(),
                  "2000-01-01T00:00:00Z");
        EXPECT_EQ(exponential->endEpoch.toString(), "2001-01-01T00:00:00Z");
        EXPECT_EQ(exponential->relaxationConstant, 2.0);
        EXPECT_EQ(exponential->beforeScaleFactor, 0.0);
        EXPECT_EQ(exponential->initialScaleFactor, 1.0);
        EXPECT_EQ(exponential->finalScaleFactor, 3.0);
    }
}

// ---------------------------------------------------------------------------

TEST(defmodel, error_cases) {
    const auto jFullValid(getFullValidContent());
    {
        json jcopy(jFullValid);
        jcopy["horizontal_offset_method"] = "unsupported";
        EXPECT_THROW(MasterFile::parse(jcopy.dump()), ParsingException);
    }

    {
        json jcopy(jFullValid);
        jcopy["horizontal_offset_unit"] = "unsupported";
        EXPECT_THROW(MasterFile::parse(jcopy.dump()), ParsingException);
    }

    {
        json jcopy(jFullValid);
        jcopy["vertical_offset_unit"] = "unsupported";
        EXPECT_THROW(MasterFile::parse(jcopy.dump()), ParsingException);
    }

    {
        json jcopy(jFullValid);
        jcopy["components"][IDX_CONSTANT]["spatial_model"]
             ["interpolation_method"] = "unsupported";
        EXPECT_THROW(MasterFile::parse(jcopy.dump()), ParsingException);
    }

    {
        json jcopy(jFullValid);
        jcopy["components"][IDX_CONSTANT]["displacement_type"] = "unsupported";
        EXPECT_THROW(MasterFile::parse(jcopy.dump()), ParsingException);
    }

    {
        json jcopy(jFullValid);
        jcopy["components"][IDX_PIECEWISE]["time_function"]["parameters"]
             ["model"] = "foo";
        EXPECT_THROW(MasterFile::parse(jcopy.dump()), ParsingException);
    }

    {
        json jcopy(jFullValid);
        jcopy["components"][IDX_PIECEWISE]["time_function"]["parameters"]
             ["before_first"] = "illegal";
        EXPECT_THROW(MasterFile::parse(jcopy.dump()), ParsingException);
    }

    {
        json jcopy(jFullValid);
        jcopy["components"][IDX_PIECEWISE]["time_function"]["parameters"]
             ["after_last"] = "illegal";
        EXPECT_THROW(MasterFile::parse(jcopy.dump()), ParsingException);
    }

    {
        json jcopy(jFullValid);
        jcopy["components"][0]["time_function"]["type"] = "unknown";
        EXPECT_THROW(MasterFile::parse(jcopy.dump()), ParsingException);
    }

    // Unsupported combination
    {
        json jcopy(jFullValid);
        jcopy["horizontal_offset_method"] = "geocentric";
        EXPECT_NO_THROW(MasterFile::parse(jcopy.dump()));
    }
    {
        json jcopy(jFullValid);
        jcopy["horizontal_offset_unit"] = "degree";
        EXPECT_NO_THROW(MasterFile::parse(jcopy.dump()));
    }
    {
        json jcopy(jFullValid);
        jcopy["horizontal_offset_method"] = "geocentric";
        jcopy["horizontal_offset_unit"] = "degree";
        EXPECT_THROW(MasterFile::parse(jcopy.dump()), ParsingException);
    }

    // Unsupported combination
    {
        json jcopy(jFullValid);
        jcopy["components"][IDX_VELOCITY]["spatial_model"]
             ["interpolation_method"] = "geocentric_bilinear";
        EXPECT_NO_THROW(MasterFile::parse(jcopy.dump()));
    }
    {
        json jcopy(jFullValid);
        jcopy["horizontal_offset_unit"] = "degree";
        EXPECT_NO_THROW(MasterFile::parse(jcopy.dump()));
    }
    {
        json jcopy(jFullValid);
        jcopy["components"][IDX_VELOCITY]["spatial_model"]
             ["interpolation_method"] = "geocentric_bilinear";
        jcopy["horizontal_offset_unit"] = "degree";
        EXPECT_THROW(MasterFile::parse(jcopy.dump()), ParsingException);
    }
}

// ---------------------------------------------------------------------------

TEST(defmodel, ISO8601ToDecimalYear) {
    EXPECT_EQ(ISO8601ToDecimalYear("2000-01-01T00:00:00Z"), 2000.0);
    EXPECT_EQ(ISO8601ToDecimalYear("2000-02-29T12:00:00Z"),
              2000.0 + ((31 + 28) * 86400. + 12 * 3600) / (366 * 86400));
    EXPECT_EQ(ISO8601ToDecimalYear("2000-12-31T23:59:59Z"),
              2000.0 + (366 * 86400 - 1.) / (366 * 86400));
    EXPECT_EQ(ISO8601ToDecimalYear("2001-01-01T00:00:00Z"), 2001.0);
    EXPECT_EQ(ISO8601ToDecimalYear("2001-12-31T23:59:59Z"),
              2001.0 + (365 * 86400 - 1.) / (365 * 86400));
    EXPECT_THROW(ISO8601ToDecimalYear(""), ParsingException);
    EXPECT_THROW(ISO8601ToDecimalYear("0000-01-01T00:00:00Z"),
                 ParsingException);
    EXPECT_THROW(ISO8601ToDecimalYear("2001-02-29T00:00:00Z"),
                 ParsingException);
    EXPECT_THROW(ISO8601ToDecimalYear("2000-13-01T00:00:00Z"),
                 ParsingException);
    EXPECT_THROW(ISO8601ToDecimalYear("2000-01-32T00:00:00Z"),
                 ParsingException);
    EXPECT_THROW(ISO8601ToDecimalYear("2000-01-01T24:00:00Z"),
                 ParsingException);
    EXPECT_THROW(ISO8601ToDecimalYear("2000-01-01T00:60:00Z"),
                 ParsingException);
    EXPECT_THROW(ISO8601ToDecimalYear("2000-01-01T00:00:61Z"),
                 ParsingException);
}

// ---------------------------------------------------------------------------

TEST(defmodel, evaluate_constant) {
    const auto jFullValid(getFullValidContent());
    const auto mf = MasterFile::parse(jFullValid.dump());
    const auto &comp = mf->components()[IDX_CONSTANT];
    EXPECT_EQ(comp.timeFunction()->evaluateAt(1999.0), 1.0);
    EXPECT_EQ(comp.timeFunction()->evaluateAt(2000.0), 1.0);
    EXPECT_EQ(comp.timeFunction()->evaluateAt(2001.0), 1.0);
}

// ---------------------------------------------------------------------------

TEST(defmodel, evaluate_velocity) {
    const auto jFullValid(getFullValidContent());
    const auto mf = MasterFile::parse(jFullValid.dump());
    const auto &comp = mf->components()[IDX_VELOCITY];
    EXPECT_EQ(comp.timeFunction()->evaluateAt(1999.0), -1.0);
    EXPECT_EQ(comp.timeFunction()->evaluateAt(2000.0), 0.0);
    EXPECT_EQ(comp.timeFunction()->evaluateAt(2001.0), 1.0);
}

// ---------------------------------------------------------------------------

TEST(defmodel, evaluate_step) {
    const auto jFullValid(getFullValidContent());
    const auto mf = MasterFile::parse(jFullValid.dump());
    const auto &comp = mf->components()[IDX_STEP];
    EXPECT_EQ(comp.timeFunction()->evaluateAt(1999.99), 0.0);
    EXPECT_EQ(comp.timeFunction()->evaluateAt(2000.00), 1.0);
    EXPECT_EQ(comp.timeFunction()->evaluateAt(2000.01), 1.0);
}

// ---------------------------------------------------------------------------

TEST(defmodel, evaluate_reverse_step) {
    const auto jFullValid(getFullValidContent());
    const auto mf = MasterFile::parse(jFullValid.dump());
    const auto &comp = mf->components()[IDX_REVERSE_STEP];
    EXPECT_EQ(comp.timeFunction()->evaluateAt(1999.99), -1.0);
    EXPECT_EQ(comp.timeFunction()->evaluateAt(2000.00), 0.0);
    EXPECT_EQ(comp.timeFunction()->evaluateAt(2000.01), 0.0);
}

// ---------------------------------------------------------------------------

TEST(defmodel, evaluate_piecewise) {
    const auto jFullValid(getFullValidContent());
    {
        const auto mf = MasterFile::parse(jFullValid.dump());
        const auto &comp = mf->components()[IDX_PIECEWISE];
        EXPECT_EQ(comp.timeFunction()->evaluateAt(2015.99), 0.0);
        EXPECT_EQ(comp.timeFunction()->evaluateAt(2016.00), 0.5);
        EXPECT_EQ(comp.timeFunction()->evaluateAt(2016.5), 0.75);
        EXPECT_NEAR(comp.timeFunction()->evaluateAt(2017 - 1e-9), 1.0, 1e-9);
        EXPECT_EQ(comp.timeFunction()->evaluateAt(2017.0), 2.0);
        EXPECT_EQ(comp.timeFunction()->evaluateAt(2017.5), 1.5);
        EXPECT_EQ(comp.timeFunction()->evaluateAt(2018.0), 1.0);
        EXPECT_EQ(comp.timeFunction()->evaluateAt(2019.0), 1.0);
    }

    {
        json jcopy(jFullValid);
        jcopy["components"][IDX_PIECEWISE]["time_function"]["parameters"]
             ["before_first"] = "zero";
        const auto mf = MasterFile::parse(jcopy.dump());
        const auto &comp = mf->components()[IDX_PIECEWISE];
        EXPECT_EQ(comp.timeFunction()->evaluateAt(2015.5), 0.0);
    }

    {
        json jcopy(jFullValid);
        jcopy["components"][IDX_PIECEWISE]["time_function"]["parameters"]
             ["before_first"] = "constant";
        const auto mf = MasterFile::parse(jcopy.dump());
        const auto &comp = mf->components()[IDX_PIECEWISE];
        EXPECT_EQ(comp.timeFunction()->evaluateAt(2015.5), 0.5);
    }

    {
        json jcopy(jFullValid);
        jcopy["components"][IDX_PIECEWISE]["time_function"]["parameters"]
             ["before_first"] = "linear";
        const auto mf = MasterFile::parse(jcopy.dump());
        const auto &comp = mf->components()[IDX_PIECEWISE];
        EXPECT_EQ(comp.timeFunction()->evaluateAt(2015.5), 0.25);
    }

    {
        json jcopy(jFullValid);
        jcopy["components"][IDX_PIECEWISE]["time_function"]["parameters"]
             ["after_last"] = "zero";
        const auto mf = MasterFile::parse(jcopy.dump());
        const auto &comp = mf->components()[IDX_PIECEWISE];
        EXPECT_EQ(comp.timeFunction()->evaluateAt(2018.5), 0.0);
    }

    {
        json jcopy(jFullValid);
        jcopy["components"][IDX_PIECEWISE]["time_function"]["parameters"]
             ["after_last"] = "constant";
        const auto mf = MasterFile::parse(jcopy.dump());
        const auto &comp = mf->components()[IDX_PIECEWISE];
        EXPECT_EQ(comp.timeFunction()->evaluateAt(2018.5), 1.0);
    }

    {
        json jcopy(jFullValid);
        jcopy["components"][IDX_PIECEWISE]["time_function"]["parameters"]
             ["after_last"] = "linear";
        const auto mf = MasterFile::parse(jcopy.dump());
        const auto &comp = mf->components()[IDX_PIECEWISE];
        EXPECT_EQ(comp.timeFunction()->evaluateAt(2018.5), 0.5);
    }

    // No epoch
    {
        json jcopy(jFullValid);
        jcopy["components"][IDX_PIECEWISE]["time_function"]["parameters"]
             ["model"]
                 .clear();
        const auto mf = MasterFile::parse(jcopy.dump());
        const auto &comp = mf->components()[IDX_PIECEWISE];
        EXPECT_EQ(comp.timeFunction()->evaluateAt(2015.5), 0.0);
    }

    // Just one epoch
    {
        json jcopy(jFullValid);
        jcopy["components"][IDX_PIECEWISE]["time_function"]["parameters"]
             ["model"] = {
                 {{"epoch", "2016-01-01T00:00:00Z"}, {"scale_factor", 0.5}}};
        jcopy["components"][IDX_PIECEWISE]["time_function"]["parameters"]
             ["before_first"] = "linear";
        jcopy["components"][IDX_PIECEWISE]["time_function"]["parameters"]
             ["after_last"] = "linear";
        const auto mf = MasterFile::parse(jcopy.dump());
        const auto &comp = mf->components()[IDX_PIECEWISE];
        EXPECT_EQ(comp.timeFunction()->evaluateAt(2015.5), 0.5);
        EXPECT_EQ(comp.timeFunction()->evaluateAt(2016.5), 0.5);
    }

    // Two identical epochs
    {
        json jcopy(jFullValid);
        jcopy["components"][IDX_PIECEWISE]["time_function"]["parameters"]
             ["model"] = {
                 {{"epoch", "2016-01-01T00:00:00Z"}, {"scale_factor", 0.5}},
                 {{"epoch", "2016-01-01T00:00:00Z"}, {"scale_factor", 1.0}}};
        jcopy["components"][IDX_PIECEWISE]["time_function"]["parameters"]
             ["before_first"] = "linear";
        jcopy["components"][IDX_PIECEWISE]["time_function"]["parameters"]
             ["after_last"] = "linear";
        const auto mf = MasterFile::parse(jcopy.dump());
        const auto &comp = mf->components()[IDX_PIECEWISE];
        EXPECT_EQ(comp.timeFunction()->evaluateAt(2015.5), 0.5);
        EXPECT_EQ(comp.timeFunction()->evaluateAt(2016.5), 1.0);
    }
}

// ---------------------------------------------------------------------------

TEST(defmodel, evaluate_exponential) {
    const auto jFullValid(getFullValidContent());
    const auto mf = MasterFile::parse(jFullValid.dump());
    const auto &comp = mf->components()[IDX_EXPONENTIAL];
    EXPECT_EQ(comp.timeFunction()->evaluateAt(1999.99), 0.0);
    EXPECT_EQ(comp.timeFunction()->evaluateAt(2000.00), 1.0);
    EXPECT_EQ(comp.timeFunction()->evaluateAt(2000.50),
              1.0 + (3.0 - 1.0) * (1.0 - std::exp(-(2000.50 - 2000.00) / 2.0)));
    EXPECT_EQ(comp.timeFunction()->evaluateAt(2001.00),
              1.0 + (3.0 - 1.0) * (1.0 - std::exp(-(2001.00 - 2000.00) / 2.0)));
    EXPECT_EQ(comp.timeFunction()->evaluateAt(2002.00),
              1.0 + (3.0 - 1.0) * (1.0 - std::exp(-(2001.00 - 2000.00) / 2.0)));
}

// ---------------------------------------------------------------------------

inline double RadToDeg(double d) { return d / DEG_TO_RAD_CONSTANT; }

// ---------------------------------------------------------------------------

TEST(defmodel, evaluator_horizontal_unit_degree) {
    json j(getMinValidContent());
    j["horizontal_offset_method"] = "addition";
    j["horizontal_offset_unit"] = "degree";
    constexpr double tFactor = 0.5;
    constexpr double gridMinX = 160;
    constexpr double gridMinY = -50;
    constexpr double gridMaxX = 190;
    constexpr double gridMaxY = -30;
    j["components"] = {
        {{"displacement_type", "horizontal"},
         {"uncertainty_type", "none"},
         {"extent",
          {{"type", "bbox"},
           {"parameters",
            {{"bbox", {gridMinX, gridMinY, gridMaxX, gridMaxY}}}}}},
         {"spatial_model",
          {
              {"type", "GeoTIFF"},
              {"interpolation_method", "bilinear"},
              {"filename", "bla.tif"},
          }},
         {"time_function",
          {{"type", "piecewise"},
           {"parameters",
            {{"before_first", "zero"},
             {"after_last", "zero"},
             {"model",
              {{{"epoch", "2010-01-01T00:00:00Z"}, {"scale_factor", tFactor}},
               {{"epoch", "2020-01-01T00:00:00Z"},
                {"scale_factor", tFactor}}}}}}}}}};

    constexpr int iQueriedX = 1;
    constexpr int iQueriedY = 3;
    constexpr double lonOffsetQueriedX = 0.01;
    constexpr double lonOffsetQueriedXp1 = 0.02;
    constexpr double latOffsetQueriedY = 0.03;
    constexpr double latOffsetQueriedYp1 = 0.04;
    constexpr double zOffsetQueriedXY = 10.;
    constexpr double zOffsetQueriedXp1Y = 11.;
    constexpr double zOffsetQueriedXYp1 = 11.;
    constexpr double zOffsetQueriedXp1Yp1 = 12.;
    constexpr double gridResX = 2;
    constexpr double gridResY = 0.5;

    struct Grid : public GridPrototype {
        bool getLonLatOffset(int ix, int iy, double &lonOffsetRadian,
                             double &latOffsetRadian) const {
            if (ix == iQueriedX) {
                lonOffsetRadian = DegToRad(lonOffsetQueriedX);
            } else if (ix == iQueriedX + 1) {
                lonOffsetRadian = DegToRad(lonOffsetQueriedXp1);
            } else {
                return false;
            }
            if (iy == iQueriedY) {
                latOffsetRadian = DegToRad(latOffsetQueriedY);
            } else if (iy == iQueriedY + 1) {
                latOffsetRadian = DegToRad(latOffsetQueriedYp1);
            } else {
                return false;
            }
            return true;
        }

        bool getZOffset(int ix, int iy, double &zOffset) const {
            if (ix == iQueriedX && iy == iQueriedY) {
                zOffset = zOffsetQueriedXY;
            } else if (ix == iQueriedX + 1 && iy == iQueriedY) {
                zOffset = zOffsetQueriedXp1Y;
            } else if (ix == iQueriedX && iy == iQueriedY + 1) {
                zOffset = zOffsetQueriedXYp1;
            } else if (ix == iQueriedX + 1 && iy == iQueriedY + 1) {
                zOffset = zOffsetQueriedXp1Yp1;
            } else {
                return false;
            }
            return true;
        }

        bool getLonLatZOffset(int ix, int iy, double &lonOffsetRadian,
                              double &latOffsetRadian, double &zOffset) const {
            return getLonLatOffset(ix, iy, lonOffsetRadian, latOffsetRadian) &&
                   getZOffset(ix, iy, zOffset);
        }

#ifdef DEBUG_DEFMODEL
        std::string name() const { return std::string(); }
#endif
    };

    struct GridSet : public GridSetPrototype<Grid> {

        Grid grid{};

        GridSet() {
            grid.minx = DegToRad(gridMinX);
            grid.miny = DegToRad(gridMinY);
            grid.resx = DegToRad(gridResX);
            grid.resy = DegToRad(gridResY);
            grid.width =
                1 + static_cast<int>(0.5 + (gridMaxX - gridMinX) / gridResX);
            grid.height =
                1 + static_cast<int>(0.5 + (gridMaxY - gridMinY) / gridResY);
        }

        const Grid *gridAt(double /*x */, double /* y */) { return &grid; }
    };

    struct EvaluatorIface : public EvaluatorIfacePrototype<Grid, GridSet> {
        std::unique_ptr<GridSet> open(const std::string &filename) {
            if (filename != "bla.tif")
                return nullptr;
            return std::unique_ptr<GridSet>(new GridSet());
        }

        bool isGeographicCRS(const std::string & /* crsDef */) { return true; }

#ifdef DEBUG_DEFMODEL
        void log(const std::string & /* msg */) {}
#endif
    };

    EvaluatorIface iface;

    Evaluator<Grid, GridSet, EvaluatorIface> eval(MasterFile::parse(j.dump()),
                                                  iface, 1, 1);
    double newLon;
    double newLat;
    double newZ;
    constexpr double tValid = 2018;
    constexpr double EPS = 1e-9;
    constexpr double zVal = 100;

    // Query on exact grid intersection
    {
        const double lon = gridMinX + iQueriedX * gridResX;
        const double lat = gridMinY + iQueriedY * gridResY;
        EXPECT_TRUE(eval.forward(iface, DegToRad(lon), DegToRad(lat), zVal,
                                 tValid, newLon, newLat, newZ));
        EXPECT_NEAR(RadToDeg(newLon), lon + tFactor * lonOffsetQueriedX, EPS);
        EXPECT_NEAR(RadToDeg(newLat), lat + tFactor * latOffsetQueriedY, EPS);
        EXPECT_EQ(newZ, zVal);
    }

    // Query between grid points
    {
        constexpr double alphaX = 0.25;
        constexpr double alphaY = 0.125;
        const double lon = gridMinX + iQueriedX * gridResX + alphaX * gridResX;
        const double lat = gridMinY + iQueriedY * gridResY + alphaY * gridResY;
        EXPECT_TRUE(eval.forward(iface, DegToRad(lon), DegToRad(lat), zVal,
                                 tValid, newLon, newLat, newZ));
        EXPECT_NEAR(
            RadToDeg(newLon),
            lon +
                tFactor * (lonOffsetQueriedX +
                           alphaX * (lonOffsetQueriedXp1 - lonOffsetQueriedX)),
            EPS);
        EXPECT_NEAR(
            RadToDeg(newLat),
            lat +
                tFactor * (latOffsetQueriedY +
                           alphaY * (latOffsetQueriedYp1 - latOffsetQueriedY)),
            EPS);
        EXPECT_EQ(newZ, zVal);
    }

    // Longitude < model min
    {
        const double lon = modelMinX - 1e-1;
        const double lat = gridMinY + iQueriedY * gridResY;
        EXPECT_FALSE(eval.forward(iface, DegToRad(lon), DegToRad(lat), zVal,
                                  tValid, newLon, newLat, newZ));
    }

    // Longitude > model max
    {
        const double lon = modelMaxX + 1e-1;
        const double lat = gridMinY + iQueriedY * gridResY;
        EXPECT_FALSE(eval.forward(iface, DegToRad(lon), DegToRad(lat), zVal,
                                  tValid, newLon, newLat, newZ));
    }

    // Latitude < model min
    {
        const double lon = gridMinX + iQueriedX * gridResX;
        const double lat = modelMinY - 1e-1;
        EXPECT_FALSE(eval.forward(iface, DegToRad(lon), DegToRad(lat), zVal,
                                  tValid, newLon, newLat, newZ));
    }

    // Latitude > model max
    {
        const double lon = gridMinX + iQueriedX * gridResX;
        const double lat = modelMaxY + 1e-1;
        EXPECT_FALSE(eval.forward(iface, DegToRad(lon), DegToRad(lat), zVal,
                                  tValid, newLon, newLat, newZ));
    }

    // Before timeExtent.first
    {
        const double lon = gridMinX + iQueriedX * gridResX;
        const double lat = gridMinY + iQueriedY * gridResY;
        EXPECT_FALSE(eval.forward(iface, DegToRad(lon), DegToRad(lat), zVal,
                                  1000, newLon, newLat, newZ));
    }

    // After timeExtent.last
    {
        const double lon = gridMinX + iQueriedX * gridResX;
        const double lat = gridMinY + iQueriedY * gridResY;
        EXPECT_FALSE(eval.forward(iface, DegToRad(lon), DegToRad(lat), zVal,
                                  3000, newLon, newLat, newZ));
    }

    // Longitude < grid min
    {
        const double lon = gridMinX - 1e-1;
        const double lat = gridMinY + iQueriedY * gridResY;
        EXPECT_TRUE(eval.forward(iface, DegToRad(lon), DegToRad(lat), zVal,
                                 tValid, newLon, newLat, newZ));
        EXPECT_NEAR(RadToDeg(newLon), lon, EPS);
        EXPECT_NEAR(RadToDeg(newLat), lat, EPS);
        EXPECT_EQ(newZ, zVal);
    }

    // Longitude > grid max
    {
        const double lon = gridMaxX + 1e-1;
        const double lat = gridMinY + iQueriedY * gridResY;
        EXPECT_TRUE(eval.forward(iface, DegToRad(lon), DegToRad(lat), zVal,
                                 tValid, newLon, newLat, newZ));
        EXPECT_NEAR(RadToDeg(newLon), lon, EPS);
        EXPECT_NEAR(RadToDeg(newLat), lat, EPS);
        EXPECT_EQ(newZ, zVal);
    }

    // Latitude < grid min
    {
        const double lon = gridMinX + iQueriedX * gridResX;
        const double lat = gridMinY - 1e-1;
        EXPECT_TRUE(eval.forward(iface, DegToRad(lon), DegToRad(lat), zVal,
                                 tValid, newLon, newLat, newZ));
        EXPECT_NEAR(RadToDeg(newLon), lon, EPS);
        EXPECT_NEAR(RadToDeg(newLat), lat, EPS);
        EXPECT_EQ(newZ, zVal);
    }

    // Latitude > grid max
    {
        const double lon = gridMinX + iQueriedX * gridResX;
        const double lat = gridMaxY + 1e-1;
        EXPECT_TRUE(eval.forward(iface, DegToRad(lon), DegToRad(lat), zVal,
                                 tValid, newLon, newLat, newZ));
        EXPECT_NEAR(RadToDeg(newLon), lon, EPS);
        EXPECT_NEAR(RadToDeg(newLat), lat, EPS);
        EXPECT_EQ(newZ, zVal);
    }

    // Time function values to zero
    {
        const double lon = gridMinX + iQueriedX * gridResX;
        const double lat = gridMinY + iQueriedY * gridResY;
        EXPECT_TRUE(eval.forward(iface, DegToRad(lon), DegToRad(lat), zVal,
                                 2000, newLon, newLat, newZ));
        EXPECT_NEAR(RadToDeg(newLon), lon, EPS);
        EXPECT_NEAR(RadToDeg(newLat), lat, EPS);
        EXPECT_EQ(newZ, zVal);
    }

    // Test vertical
    j["components"][0]["displacement_type"] = "vertical";
    j["vertical_offset_unit"] = "metre";
    Evaluator<Grid, GridSet, EvaluatorIface> evalVertical(
        MasterFile::parse(j.dump()), iface, 1, 1);
    {
        constexpr double alphaX = 0.25;
        constexpr double alphaY = 0.125;
        const double lon = gridMinX + iQueriedX * gridResX + alphaX * gridResX;
        const double lat = gridMinY + iQueriedY * gridResY + alphaY * gridResY;
        EXPECT_TRUE(evalVertical.forward(iface, DegToRad(lon), DegToRad(lat),
                                         zVal, tValid, newLon, newLat, newZ));
        EXPECT_NEAR(RadToDeg(newLon), lon, EPS);
        EXPECT_NEAR(RadToDeg(newLat), lat, EPS);

        const double zBottom =
            zOffsetQueriedXY + alphaX * (zOffsetQueriedXp1Y - zOffsetQueriedXY);
        const double zTop =
            zOffsetQueriedXYp1 +
            alphaX * (zOffsetQueriedXp1Yp1 - zOffsetQueriedXYp1);
        EXPECT_NEAR(
            newZ, zVal + tFactor * (zBottom + alphaY * (zTop - zBottom)), EPS);
    }

    // Test 3d
    j["components"][0]["displacement_type"] = "3d";
    j["vertical_offset_unit"] = "metre";
    Evaluator<Grid, GridSet, EvaluatorIface> eval3d(MasterFile::parse(j.dump()),
                                                    iface, 1, 1);
    {
        constexpr double alphaX = 0.25;
        constexpr double alphaY = 0.125;
        const double lon = gridMinX + iQueriedX * gridResX + alphaX * gridResX;
        const double lat = gridMinY + iQueriedY * gridResY + alphaY * gridResY;
        EXPECT_TRUE(eval3d.forward(iface, DegToRad(lon), DegToRad(lat), zVal,
                                   tValid, newLon, newLat, newZ));
        EXPECT_NEAR(
            RadToDeg(newLon),
            lon +
                tFactor * (lonOffsetQueriedX +
                           alphaX * (lonOffsetQueriedXp1 - lonOffsetQueriedX)),
            EPS);
        EXPECT_NEAR(
            RadToDeg(newLat),
            lat +
                tFactor * (latOffsetQueriedY +
                           alphaY * (latOffsetQueriedYp1 - latOffsetQueriedY)),
            EPS);

        const double zBottom =
            zOffsetQueriedXY + alphaX * (zOffsetQueriedXp1Y - zOffsetQueriedXY);
        const double zTop =
            zOffsetQueriedXYp1 +
            alphaX * (zOffsetQueriedXp1Yp1 - zOffsetQueriedXYp1);
        EXPECT_NEAR(
            newZ, zVal + tFactor * (zBottom + alphaY * (zTop - zBottom)), EPS);
    }
}

// ---------------------------------------------------------------------------

inline void DeltaLongLatToEastingNorthing(double phi, double dlam, double dphi,
                                          double a, double b, double &de,
                                          double &dn) {
    const double sinphi = sin(phi);
    const double cosphi = cos(phi);
    const double a2 = a * a;
    const double b2 = b * b;
    const double X = a2 * (cosphi * cosphi) + b2 * (sinphi * sinphi);
    const double sqrtX = sqrt(X);
    de = dlam * (a2 * cosphi) / sqrtX;
    dn = dphi * a2 * b2 / (sqrtX * X);
}

// ---------------------------------------------------------------------------

TEST(defmodel, evaluator_horizontal_unit_metre) {

    json j(getMinValidContent());
    j["horizontal_offset_method"] = "addition";
    j["horizontal_offset_unit"] = "metre";
    j["vertical_offset_unit"] = "metre";
    constexpr double tFactor = 0.5;
    constexpr double gridMinX = 165.8;
    constexpr double gridMinY = -37.5;
    constexpr double gridMaxX = 166.2;
    constexpr double gridMaxY = -37.2;
    constexpr double gridResX = gridMaxX - gridMinX;
    constexpr double gridResY = gridMaxY - gridMinY;
    constexpr int extraPointX = 1;
    constexpr int extraPointY = 1;

    j["components"] = {
        {{"displacement_type", "horizontal"},
         {"uncertainty_type", "none"},
         {"extent",
          {{"type", "bbox"},
           {"parameters",
            {{"bbox",
              {gridMinX - extraPointX * gridResX,
               gridMinY - extraPointY * gridResY, gridMaxX, gridMaxY}}}}}},
         {"spatial_model",
          {
              {"type", "GeoTIFF"},
              {"interpolation_method", "XXXXXXX"},
              {"filename", "bla.tif"},
          }},
         {"time_function",
          {{"type", "piecewise"},
           {"parameters",
            {{"before_first", "zero"},
             {"after_last", "zero"},
             {"model",
              {{{"epoch", "2010-01-01T00:00:00Z"}, {"scale_factor", tFactor}},
               {{"epoch", "2020-01-01T00:00:00Z"},
                {"scale_factor", tFactor}}}}}}}}}};

    struct Grid : public GridPrototype {
        bool getEastingNorthingOffset(int ix, int iy, double &eastingOffset,
                                      double &northingOffset) const {
            ix -= extraPointX;
            iy -= extraPointY;
            if (ix == -1)
                ix = 0;
            if (iy == -1)
                iy = 0;
            if (ix == 0 && iy == 0) {
                eastingOffset = 0.4f;
                northingOffset = -0.2f;
            } else if (ix == 1 && iy == 0) {
                eastingOffset = 0.5f;
                northingOffset = -0.25f;
            } else if (ix == 0 && iy == 1) {
                eastingOffset = 0.8f;
                northingOffset = -0.4f;
            } else if (ix == 1 && iy == 1) {
                eastingOffset = 1.f;
                northingOffset = -0.3f;
            } else {
                return false;
            }
            return true;
        }

        bool getZOffset(int ix, int iy, double &zOffset) const {
            ix -= extraPointX;
            iy -= extraPointY;
            if (ix == -1)
                ix = 0;
            if (iy == -1)
                iy = 0;
            if (ix == 0 && iy == 0) {
                zOffset = 0.84f;
            } else if (ix == 1 && iy == 0) {
                zOffset = 0.75f;
            } else if (ix == 0 && iy == 1) {
                zOffset = 0.36f;
            } else if (ix == 1 && iy == 1) {
                zOffset = 0.f;
            } else {
                return false;
            }
            return true;
        }

        bool getEastingNorthingZOffset(int ix, int iy, double &eastingOffset,
                                       double &northingOffset,
                                       double &zOffset) const {
            return getEastingNorthingOffset(ix, iy, eastingOffset,
                                            northingOffset) &&
                   getZOffset(ix, iy, zOffset);
        }

#ifdef DEBUG_DEFMODEL
        std::string name() const { return std::string(); }
#endif
    };

    struct GridSet : public GridSetPrototype<Grid> {

        Grid grid{};

        GridSet() {
            grid.minx = DegToRad(gridMinX - extraPointX * gridResX);
            grid.miny = DegToRad(gridMinY - extraPointY * gridResY);
            grid.resx = DegToRad(gridResX);
            grid.resy = DegToRad(gridResY);
            grid.width = 2 + extraPointX;
            grid.height = 2 + extraPointY;
        }

        const Grid *gridAt(double /*x */, double /* y */) { return &grid; }
    };

    struct EvaluatorIface : public EvaluatorIfacePrototype<Grid, GridSet> {
        std::unique_ptr<GridSet> open(const std::string &filename) {
            if (filename != "bla.tif")
                return nullptr;
            return std::unique_ptr<GridSet>(new GridSet());
        }

        bool isGeographicCRS(const std::string & /* crsDef */) { return true; }

#ifdef DEBUG_DEFMODEL
        void log(const std::string & /* msg */) {}
#endif

        void geographicToGeocentric(double lam, double phi, double height,
                                    double a, double /*b*/, double es,
                                    double &X, double &Y, double &Z) {
            PJ_CONTEXT *ctx = proj_context_create();
            PJ *cart = proj_create(
                ctx,
                ("+proj=cart +a=" + osgeo::proj::internal::toString(a, 18) +
                 " +es=" + osgeo::proj::internal::toString(es, 18))
                    .c_str());
            PJ_LPZ lpz;
            lpz.lam = lam;
            lpz.phi = phi;
            lpz.z = height;
            PJ_XYZ xyz = cart->fwd3d(lpz, cart);
            X = xyz.x;
            Y = xyz.y;
            Z = xyz.z;
            proj_destroy(cart);
            proj_context_destroy(ctx);
        }

        void geocentricToGeographic(double X, double Y, double Z, double a,
                                    double /*b*/, double es, double &lam,
                                    double &phi, double &height) {
            PJ_CONTEXT *ctx = proj_context_create();
            PJ *cart = proj_create(
                ctx,
                ("+proj=cart +a=" + osgeo::proj::internal::toString(a, 18) +
                 " +es=" + osgeo::proj::internal::toString(es, 18))
                    .c_str());
            PJ_XYZ xyz;
            xyz.x = X;
            xyz.y = Y;
            xyz.z = Z;
            PJ_LPZ lpz = cart->inv3d(xyz, cart);
            lam = lpz.lam;
            phi = lpz.phi;
            height = lpz.z;
            proj_destroy(cart);
            proj_context_destroy(ctx);
        }
    };
    EvaluatorIface iface;

    constexpr double a = 6378137;
    constexpr double b = 6356752.314140;
    constexpr double tValid = 2018;
    constexpr double zVal = 100;

    const struct {
        double lon;
        double lat;
        double expected_de;
        double expected_dn;
        double expected_dz;
        const char *displacement_type;
        const char *interpolation_method;
    } testPoints[] = {
        {gridMinX - extraPointX * gridResX - 1e-11,
         gridMinY - extraPointY * gridResY - 1e-11, 0.4, -0.2, 0, "horizontal",
         "bilinear"},
        {gridMinX, gridMinY, 0.4, -0.2, 0, "horizontal", "bilinear"},
        {gridMaxX, gridMinY, 0.5, -0.25, 0, "horizontal", "bilinear"},
        {gridMinX, gridMaxY, 0.8, -0.4, 0, "horizontal", "bilinear"},
        {gridMaxX, gridMaxY, 1, -0.3, 0, "horizontal", "bilinear"},
        {gridMaxX + 1e-11, gridMaxY + 1e-11, 1, -0.3, 0, "horizontal",
         "bilinear"},
        {165.9, -37.3, 0.70833334, -0.32083334, 0, "horizontal", "bilinear"},
        {165.9, -37.3, 0.70833334, -0.32083334, 0.4525, "3d", "bilinear"},

        {gridMinX, gridMinY, 0.4, -0.2, 0, "horizontal", "geocentric_bilinear"},
        {gridMaxX, gridMinY, 0.5, -0.25, 0, "horizontal",
         "geocentric_bilinear"},
        {gridMinX, gridMaxY, 0.8, -0.4, 0, "horizontal", "geocentric_bilinear"},
        {gridMaxX, gridMaxY, 1, -0.3, 0, "horizontal", "geocentric_bilinear"},
        {165.9, -37.3, 0.7083692044608846, -0.3209642339711405, 0, "horizontal",
         "geocentric_bilinear"},
        {165.9, -37.3, 0.7083692044608846, -0.3209642339711405, 0.4525, "3d",
         "geocentric_bilinear"},
    };

    for (const auto &testPoint : testPoints) {
        j["components"][0]["displacement_type"] = testPoint.displacement_type;
        j["components"][0]["spatial_model"]["interpolation_method"] =
            testPoint.interpolation_method;
        Evaluator<Grid, GridSet, EvaluatorIface> eval(
            MasterFile::parse(j.dump()), iface, a, b);

        const double lon = testPoint.lon;
        const double lat = testPoint.lat;
        double newLon;
        double newLat;
        double newZ;
        EXPECT_TRUE(eval.forward(iface, DegToRad(lon), DegToRad(lat), zVal,
                                 tValid, newLon, newLat, newZ))
            << lon << " " << lat << " " << testPoint.displacement_type
            << testPoint.interpolation_method;
        EXPECT_NEAR(newZ - zVal, tFactor * testPoint.expected_dz, 1e-8)
            << lon << " " << lat << " " << testPoint.displacement_type
            << testPoint.interpolation_method;

        double de;
        double dn;
        DeltaLongLatToEastingNorthing(DegToRad(lat), newLon - DegToRad(lon),
                                      newLat - DegToRad(lat), a, b, de, dn);
        EXPECT_NEAR(de, tFactor * testPoint.expected_de, 1e-8)
            << lon << " " << lat << " " << testPoint.displacement_type
            << testPoint.interpolation_method;
        EXPECT_NEAR(dn, tFactor * testPoint.expected_dn, 1e-8)
            << lon << " " << lat << " " << testPoint.displacement_type
            << testPoint.interpolation_method;

        if (lon == gridMinX && lat == gridMinY) {
            // Redo the exact same test, to test caching
            double newLon2;
            double newLat2;
            double newZ2;
            EXPECT_TRUE(eval.forward(iface, DegToRad(lon), DegToRad(lat), zVal,
                                     tValid, newLon2, newLat2, newZ2));
            EXPECT_EQ(newLon2, newLon);
            EXPECT_EQ(newLat2, newLat);
            EXPECT_EQ(newZ2, newZ);

            // Shift in longitude
            EXPECT_TRUE(eval.forward(iface, DegToRad(lon - gridResX / 2),
                                     DegToRad(lat), zVal, tValid, newLon2,
                                     newLat2, newZ2));

            // Redo test at original position
            EXPECT_TRUE(eval.forward(iface, DegToRad(lon), DegToRad(lat), zVal,
                                     tValid, newLon2, newLat2, newZ2));
            EXPECT_EQ(newLon2, newLon);
            EXPECT_EQ(newLat2, newLat);
            EXPECT_EQ(newZ2, newZ);

            // Shift in latitude
            EXPECT_TRUE(eval.forward(iface, DegToRad(lon),
                                     DegToRad(lat - gridResY / 2), zVal, tValid,
                                     newLon2, newLat2, newZ2));

            // Redo test at original position
            EXPECT_TRUE(eval.forward(iface, DegToRad(lon), DegToRad(lat), zVal,
                                     tValid, newLon2, newLat2, newZ2));
            EXPECT_EQ(newLon2, newLon);
            EXPECT_EQ(newLat2, newLat);
            EXPECT_EQ(newZ2, newZ);
        }
    }

    // Test inverse()
    {
        j["horizontal_offset_method"] = "addition";
        j["components"][0]["displacement_type"] = "3d";
        j["components"][0]["spatial_model"]["interpolation_method"] =
            "bilinear";
        Evaluator<Grid, GridSet, EvaluatorIface> eval(
            MasterFile::parse(j.dump()), iface, a, b);

        const double lon = 165.9;
        const double lat = -37.3;
        double newLon;
        double newLat;
        double newZ;
        EXPECT_TRUE(eval.forward(iface, DegToRad(lon), DegToRad(lat), zVal,
                                 tValid, newLon, newLat, newZ));

        double invLon;
        double invLat;
        double invZ;
        EXPECT_TRUE(eval.inverse(iface, newLon, newLat, newZ, tValid, invLon,
                                 invLat, invZ));
        EXPECT_NEAR(RadToDeg(invLon), lon, 1e-10);
        EXPECT_NEAR(RadToDeg(invLat), lat, 1e-10);
        EXPECT_NEAR(invZ, zVal, 1e-4);
    }

    // Test horizontal_offset_method = geocentric
    {
        j["horizontal_offset_method"] = "geocentric";
        j["components"][0]["displacement_type"] = "3d";
        j["components"][0]["spatial_model"]["interpolation_method"] =
            "bilinear";
        Evaluator<Grid, GridSet, EvaluatorIface> eval(
            MasterFile::parse(j.dump()), iface, a, b);

        const double lon = gridMinX;
        const double lat = gridMinY;
        double newLon;
        double newLat;
        double newZ;
        EXPECT_TRUE(eval.forward(iface, DegToRad(lon), DegToRad(lat), zVal,
                                 tValid, newLon, newLat, newZ));

        double de;
        double dn;
        constexpr double expected_de = 0.40000000948081327;
        constexpr double expected_dn = -0.19999999810542682;
        DeltaLongLatToEastingNorthing(DegToRad(lat), newLon - DegToRad(lon),
                                      newLat - DegToRad(lat), a, b, de, dn);
        EXPECT_NEAR(de, tFactor * expected_de, 1e-10);
        EXPECT_NEAR(dn, tFactor * expected_dn, 1e-9);
        EXPECT_NEAR(newZ - zVal, tFactor * 0.84, 1e-4);
    }
}

// ---------------------------------------------------------------------------

TEST(defmodel, evaluator_projected_crs) {

    json j(getMinValidContent());
    j["horizontal_offset_method"] = "addition";
    j["horizontal_offset_unit"] = "metre";
    j["vertical_offset_unit"] = "metre";
    constexpr double gridMinX = 10000;
    constexpr double gridMinY = 20000;
    constexpr double gridMaxX = 30000;
    constexpr double gridMaxY = 40000;
    constexpr double gridResX = gridMaxX - gridMinX;
    constexpr double gridResY = gridMaxY - gridMinY;

    j["extent"]
     ["parameters"] = {{"bbox", {gridMinX, gridMinY, gridMaxX, gridMaxY}}};
    j["components"] = {
        {{"displacement_type", "horizontal"},
         {"uncertainty_type", "none"},
         {"extent",
          {{"type", "bbox"},
           {"parameters",
            {{"bbox", {gridMinX, gridMinY, gridMaxX, gridMaxY}}}}}},
         {"spatial_model",
          {
              {"type", "GeoTIFF"},
              {"interpolation_method", "bilinear"},
              {"filename", "bla.tif"},
          }},
         {"time_function", {{"type", "constant"}}}}};

    struct Grid : public GridPrototype {
        bool getEastingNorthingOffset(int ix, int iy, double &eastingOffset,
                                      double &northingOffset) const {
            if (ix == 0 && iy == 0) {
                eastingOffset = 0.4;
                northingOffset = -0.2;
            } else if (ix == 1 && iy == 0) {
                eastingOffset = 0.5;
                northingOffset = -0.25;
            } else if (ix == 0 && iy == 1) {
                eastingOffset = 0.8;
                northingOffset = -0.4;
            } else if (ix == 1 && iy == 1) {
                eastingOffset = 1.;
                northingOffset = -0.3;
            } else {
                return false;
            }
            return true;
        }

#ifdef DEBUG_DEFMODEL
        std::string name() const { return std::string(); }
#endif
    };

    struct GridSet : public GridSetPrototype<Grid> {

        Grid grid{};

        GridSet() {
            grid.minx = gridMinX;
            grid.miny = gridMinY;
            grid.resx = gridResX;
            grid.resy = gridResY;
            grid.width = 2;
            grid.height = 2;
        }

        const Grid *gridAt(double /*x */, double /* y */) { return &grid; }
    };

    struct EvaluatorIface : public EvaluatorIfacePrototype<Grid, GridSet> {
        std::unique_ptr<GridSet> open(const std::string &filename) {
            if (filename != "bla.tif")
                return nullptr;
            return std::unique_ptr<GridSet>(new GridSet());
        }

        bool isGeographicCRS(const std::string & /* crsDef */) { return false; }

#ifdef DEBUG_DEFMODEL
        void log(const std::string & /* msg */) {}
#endif
    };
    EvaluatorIface iface;

    constexpr double a = 6378137;
    constexpr double b = 6356752.314140;
    constexpr double tValid = 2018;
    constexpr double zVal = 100;

    Evaluator<Grid, GridSet, EvaluatorIface> eval(MasterFile::parse(j.dump()),
                                                  iface, a, b);

    double newX;
    double newY;
    double newZ;
    EXPECT_TRUE(eval.forward(iface, gridMinX, gridMinY, zVal, tValid, newX,
                             newY, newZ));
    EXPECT_NEAR(newX - gridMinX, 0.4, 1e-8);
    EXPECT_NEAR(newY - gridMinY, -0.2, 1e-8);
    EXPECT_NEAR(newZ - zVal, 0, 1e-8);

    {
        json jcopy(j);
        jcopy["horizontal_offset_unit"] = "degree";
        EXPECT_THROW((Evaluator<Grid, GridSet, EvaluatorIface>(
                         MasterFile::parse(jcopy.dump()), iface, a, b)),
                     EvaluatorException);
    }

    {
        json jcopy(j);
        jcopy["horizontal_offset_method"] = "geocentric";
        EXPECT_THROW((Evaluator<Grid, GridSet, EvaluatorIface>(
                         MasterFile::parse(jcopy.dump()), iface, a, b)),
                     EvaluatorException);
    }

    {
        json jcopy(j);
        jcopy["components"][0]["spatial_model"]["interpolation_method"] =
            "geocentric_bilinear";
        EXPECT_THROW((Evaluator<Grid, GridSet, EvaluatorIface>(
                         MasterFile::parse(jcopy.dump()), iface, a, b)),
                     EvaluatorException);
    }
}

} // namespace

#ifdef _MSC_VER
#pragma warning(pop)
#endif
