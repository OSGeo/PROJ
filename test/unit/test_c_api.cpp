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

#include <cstdio>
#include <limits>

#include "proj.h"
#include "proj_constants.h"
#include "proj_experimental.h"

#include "proj/common.hpp"
#include "proj/coordinateoperation.hpp"
#include "proj/coordinatesystem.hpp"
#include "proj/crs.hpp"
#include "proj/datum.hpp"
#include "proj/io.hpp"
#include "proj/metadata.hpp"
#include "proj/util.hpp"

#include <sqlite3.h>

using namespace osgeo::proj::common;
using namespace osgeo::proj::crs;
using namespace osgeo::proj::cs;
using namespace osgeo::proj::datum;
using namespace osgeo::proj::io;
using namespace osgeo::proj::metadata;
using namespace osgeo::proj::operation;
using namespace osgeo::proj::util;

namespace {

class CApi : public ::testing::Test {

    static void DummyLogFunction(void *, int, const char *) {}

  protected:
    void SetUp() override {
        m_ctxt = proj_context_create();
        proj_log_func(m_ctxt, nullptr, DummyLogFunction);
    }

    void TearDown() override {
        if (m_ctxt)
            proj_context_destroy(m_ctxt);
    }

    static BoundCRSNNPtr createBoundCRS() {
        return BoundCRS::create(
            GeographicCRS::EPSG_4807, GeographicCRS::EPSG_4326,
            Transformation::create(
                PropertyMap(), GeographicCRS::EPSG_4807,
                GeographicCRS::EPSG_4326, nullptr, PropertyMap(),
                {OperationParameter::create(
                    PropertyMap().set(IdentifiedObject::NAME_KEY, "foo"))},
                {ParameterValue::create(
                    Measure(1.0, UnitOfMeasure::SCALE_UNITY))},
                {}));
    }

    static ProjectedCRSNNPtr createProjectedCRS() {
        PropertyMap propertiesCRS;
        propertiesCRS.set(Identifier::CODESPACE_KEY, "EPSG")
            .set(Identifier::CODE_KEY, 32631)
            .set(IdentifiedObject::NAME_KEY, "WGS 84 / UTM zone 31N");
        return ProjectedCRS::create(
            propertiesCRS, GeographicCRS::EPSG_4326,
            Conversion::createUTM(PropertyMap(), 31, true),
            CartesianCS::createEastingNorthing(UnitOfMeasure::METRE));
    }

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

    static CompoundCRSNNPtr createCompoundCRS() {
        PropertyMap properties;
        properties.set(Identifier::CODESPACE_KEY, "codespace")
            .set(Identifier::CODE_KEY, "code")
            .set(IdentifiedObject::NAME_KEY, "horizontal + vertical");
        return CompoundCRS::create(
            properties,
            std::vector<CRSNNPtr>{createProjectedCRS(), createVerticalCRS()});
    }

    PJ_CONTEXT *m_ctxt = nullptr;

    struct ObjectKeeper {
        PJ *m_obj = nullptr;
        explicit ObjectKeeper(PJ *obj) : m_obj(obj) {}
        ~ObjectKeeper() { proj_destroy(m_obj); }

        ObjectKeeper(const ObjectKeeper &) = delete;
        ObjectKeeper &operator=(const ObjectKeeper &) = delete;
    };

    struct ContextKeeper {
        PJ_OPERATION_FACTORY_CONTEXT *m_op_ctxt = nullptr;
        explicit ContextKeeper(PJ_OPERATION_FACTORY_CONTEXT *op_ctxt)
            : m_op_ctxt(op_ctxt) {}
        ~ContextKeeper() { proj_operation_factory_context_destroy(m_op_ctxt); }

        ContextKeeper(const ContextKeeper &) = delete;
        ContextKeeper &operator=(const ContextKeeper &) = delete;
    };

    struct ObjListKeeper {
        PJ_OBJ_LIST *m_res = nullptr;
        explicit ObjListKeeper(PJ_OBJ_LIST *res) : m_res(res) {}
        ~ObjListKeeper() { proj_list_destroy(m_res); }

        ObjListKeeper(const ObjListKeeper &) = delete;
        ObjListKeeper &operator=(const ObjListKeeper &) = delete;
    };
};

// ---------------------------------------------------------------------------

TEST_F(CApi, proj_create) {
    proj_destroy(nullptr);
    EXPECT_EQ(proj_create(m_ctxt, "invalid"), nullptr);
    {
        auto obj =
            proj_create(m_ctxt, GeographicCRS::EPSG_4326
                                    ->exportToWKT(WKTFormatter::create().get())
                                    .c_str());
        ObjectKeeper keeper(obj);
        EXPECT_NE(obj, nullptr);

        // Check that functions that operate on 'non-C++' PJ don't crash
        PJ_COORD coord;
        coord.xyzt.x = 0;
        coord.xyzt.y = 0;
        coord.xyzt.z = 0;
        coord.xyzt.t = 0;
        EXPECT_EQ(proj_trans(obj, PJ_FWD, coord).xyzt.x,
                  std::numeric_limits<double>::infinity());

        EXPECT_EQ(proj_geod(obj, coord, coord).xyzt.x,
                  std::numeric_limits<double>::infinity());
        EXPECT_EQ(proj_lp_dist(obj, coord, coord),
                  std::numeric_limits<double>::infinity());

        auto info = proj_pj_info(obj);
        EXPECT_EQ(info.id, nullptr);
        ASSERT_NE(info.description, nullptr);
        EXPECT_EQ(info.description, std::string("WGS 84"));
        ASSERT_NE(info.definition, nullptr);
        EXPECT_EQ(info.definition, std::string(""));
    }
    {
        auto obj = proj_create(m_ctxt, "EPSG:4326");
        ObjectKeeper keeper(obj);
        EXPECT_NE(obj, nullptr);
    }
}

// ---------------------------------------------------------------------------

TEST_F(CApi, proj_create_from_wkt) {

    {
        EXPECT_EQ(
            proj_create_from_wkt(m_ctxt, "invalid", nullptr, nullptr, nullptr),
            nullptr);
    }
    {
        PROJ_STRING_LIST warningList = nullptr;
        PROJ_STRING_LIST errorList = nullptr;
        EXPECT_EQ(proj_create_from_wkt(m_ctxt, "invalid", nullptr, &warningList,
                                       &errorList),
                  nullptr);
        EXPECT_EQ(warningList, nullptr);
        proj_string_list_destroy(warningList);
        EXPECT_NE(errorList, nullptr);
        proj_string_list_destroy(errorList);
    }
    {
        auto obj = proj_create_from_wkt(
            m_ctxt,
            GeographicCRS::EPSG_4326->exportToWKT(WKTFormatter::create().get())
                .c_str(),
            nullptr, nullptr, nullptr);
        ObjectKeeper keeper(obj);
        EXPECT_NE(obj, nullptr);
    }
    {
        auto obj = proj_create_from_wkt(
            m_ctxt,
            "GEOGCS[\"WGS 84\",\n"
            "    DATUM[\"WGS_1984\",\n"
            "        SPHEROID[\"WGS 84\",6378137,298.257223563,\"unused\"]],\n"
            "    PRIMEM[\"Greenwich\",0],\n"
            "    UNIT[\"degree\",0.0174532925199433]]",
            nullptr, nullptr, nullptr);
        EXPECT_EQ(obj, nullptr);
    }
    {
        PROJ_STRING_LIST warningList = nullptr;
        PROJ_STRING_LIST errorList = nullptr;
        auto obj = proj_create_from_wkt(
            m_ctxt,
            "GEOGCS[\"WGS 84\",\n"
            "    DATUM[\"WGS_1984\",\n"
            "        SPHEROID[\"WGS 84\",6378137,298.257223563,\"unused\"]],\n"
            "    PRIMEM[\"Greenwich\",0],\n"
            "    UNIT[\"degree\",0.0174532925199433]]",
            nullptr, &warningList, &errorList);
        EXPECT_EQ(obj, nullptr);
        EXPECT_EQ(warningList, nullptr);
        proj_string_list_destroy(warningList);
        EXPECT_NE(errorList, nullptr);
        proj_string_list_destroy(errorList);
    }
    {
        PROJ_STRING_LIST warningList = nullptr;
        PROJ_STRING_LIST errorList = nullptr;
        const char *const options[] = {"STRICT=NO", nullptr};
        auto obj = proj_create_from_wkt(
            m_ctxt,
            "GEOGCS[\"WGS 84\",\n"
            "    DATUM[\"WGS_1984\",\n"
            "        SPHEROID[\"WGS 84\",6378137,298.257223563,\"unused\"]],\n"
            "    PRIMEM[\"Greenwich\",0],\n"
            "    UNIT[\"degree\",0.0174532925199433]]",
            options, &warningList, &errorList);
        ObjectKeeper keeper(obj);
        EXPECT_NE(obj, nullptr);
        EXPECT_EQ(warningList, nullptr);
        proj_string_list_destroy(warningList);
        EXPECT_NE(errorList, nullptr);
        proj_string_list_destroy(errorList);
    }
    {
        PROJ_STRING_LIST warningList = nullptr;
        PROJ_STRING_LIST errorList = nullptr;
        auto obj = proj_create_from_wkt(
            m_ctxt,
            GeographicCRS::EPSG_4326->exportToWKT(WKTFormatter::create().get())
                .c_str(),
            nullptr, &warningList, &errorList);
        ObjectKeeper keeper(obj);
        EXPECT_NE(obj, nullptr);
        EXPECT_EQ(warningList, nullptr);
        EXPECT_EQ(errorList, nullptr);
    }
    // Warnings: missing projection parameters
    {
        PROJ_STRING_LIST warningList = nullptr;
        PROJ_STRING_LIST errorList = nullptr;
        auto obj = proj_create_from_wkt(
            m_ctxt, "PROJCS[\"test\",\n"
                    "  GEOGCS[\"WGS 84\",\n"
                    "    DATUM[\"WGS_1984\",\n"
                    "        SPHEROID[\"WGS 84\",6378137,298.257223563]],\n"
                    "    PRIMEM[\"Greenwich\",0],\n"
                    "    UNIT[\"degree\",0.0174532925199433]],\n"
                    "  PROJECTION[\"Transverse_Mercator\"],\n"
                    "  PARAMETER[\"latitude_of_origin\",31],\n"
                    "  UNIT[\"metre\",1]]",
            nullptr, &warningList, &errorList);
        ObjectKeeper keeper(obj);
        EXPECT_NE(obj, nullptr);
        EXPECT_NE(warningList, nullptr);
        proj_string_list_destroy(warningList);
        EXPECT_EQ(errorList, nullptr);
        proj_string_list_destroy(errorList);
    }
    {
        auto obj = proj_create_from_wkt(
            m_ctxt, "PROJCS[\"test\",\n"
                    "  GEOGCS[\"WGS 84\",\n"
                    "    DATUM[\"WGS_1984\",\n"
                    "        SPHEROID[\"WGS 84\",6378137,298.257223563]],\n"
                    "    PRIMEM[\"Greenwich\",0],\n"
                    "    UNIT[\"degree\",0.0174532925199433]],\n"
                    "  PROJECTION[\"Transverse_Mercator\"],\n"
                    "  PARAMETER[\"latitude_of_origin\",31],\n"
                    "  UNIT[\"metre\",1]]",
            nullptr, nullptr, nullptr);
        ObjectKeeper keeper(obj);
        EXPECT_NE(obj, nullptr);
    }
}

// ---------------------------------------------------------------------------

TEST_F(CApi, proj_as_wkt) {
    auto obj = proj_create_from_wkt(
        m_ctxt,
        GeographicCRS::EPSG_4326->exportToWKT(WKTFormatter::create().get())
            .c_str(),
        nullptr, nullptr, nullptr);
    ObjectKeeper keeper(obj);
    ASSERT_NE(obj, nullptr);

    {
        auto wkt = proj_as_wkt(m_ctxt, obj, PJ_WKT2_2018, nullptr);
        ASSERT_NE(wkt, nullptr);
        EXPECT_TRUE(std::string(wkt).find("GEOGCRS[") == 0) << wkt;
    }

    {
        auto wkt = proj_as_wkt(m_ctxt, obj, PJ_WKT2_2018_SIMPLIFIED, nullptr);
        ASSERT_NE(wkt, nullptr);
        EXPECT_TRUE(std::string(wkt).find("GEOGCRS[") == 0) << wkt;
        EXPECT_TRUE(std::string(wkt).find("ANGULARUNIT[") == std::string::npos)
            << wkt;
    }

    {
        auto wkt = proj_as_wkt(m_ctxt, obj, PJ_WKT2_2015, nullptr);
        ASSERT_NE(wkt, nullptr);
        EXPECT_TRUE(std::string(wkt).find("GEODCRS[") == 0) << wkt;
    }

    {
        auto wkt = proj_as_wkt(m_ctxt, obj, PJ_WKT2_2015_SIMPLIFIED, nullptr);
        ASSERT_NE(wkt, nullptr);
        EXPECT_TRUE(std::string(wkt).find("GEODCRS[") == 0) << wkt;
        EXPECT_TRUE(std::string(wkt).find("ANGULARUNIT[") == std::string::npos)
            << wkt;
    }

    {
        auto wkt = proj_as_wkt(m_ctxt, obj, PJ_WKT1_GDAL, nullptr);
        ASSERT_NE(wkt, nullptr);
        EXPECT_TRUE(std::string(wkt).find("GEOGCS[\"WGS 84\"") == 0) << wkt;
    }

    {
        auto wkt = proj_as_wkt(m_ctxt, obj, PJ_WKT1_ESRI, nullptr);
        ASSERT_NE(wkt, nullptr);
        EXPECT_TRUE(std::string(wkt).find("GEOGCS[\"GCS_WGS_1984\"") == 0)
            << wkt;
    }

    // MULTILINE=NO
    {
        const char *const options[] = {"MULTILINE=NO", nullptr};
        auto wkt = proj_as_wkt(m_ctxt, obj, PJ_WKT1_GDAL, options);
        ASSERT_NE(wkt, nullptr);
        EXPECT_TRUE(std::string(wkt).find("\n") == std::string::npos) << wkt;
    }

    // INDENTATION_WIDTH=2
    {
        const char *const options[] = {"INDENTATION_WIDTH=2", nullptr};
        auto wkt = proj_as_wkt(m_ctxt, obj, PJ_WKT1_GDAL, options);
        ASSERT_NE(wkt, nullptr);
        EXPECT_TRUE(std::string(wkt).find("\n  DATUM") != std::string::npos)
            << wkt;
    }

    // OUTPUT_AXIS=NO
    {
        const char *const options[] = {"OUTPUT_AXIS=NO", nullptr};
        auto wkt = proj_as_wkt(m_ctxt, obj, PJ_WKT1_GDAL, options);
        ASSERT_NE(wkt, nullptr);
        EXPECT_TRUE(std::string(wkt).find("AXIS") == std::string::npos) << wkt;
    }

    // OUTPUT_AXIS=AUTO
    {
        const char *const options[] = {"OUTPUT_AXIS=AUTO", nullptr};
        auto wkt = proj_as_wkt(m_ctxt, obj, PJ_WKT1_GDAL, options);
        ASSERT_NE(wkt, nullptr);
        EXPECT_TRUE(std::string(wkt).find("AXIS") == std::string::npos) << wkt;
    }

    // OUTPUT_AXIS=YES
    {
        const char *const options[] = {"OUTPUT_AXIS=YES", nullptr};
        auto wkt = proj_as_wkt(m_ctxt, obj, PJ_WKT1_GDAL, options);
        ASSERT_NE(wkt, nullptr);
        EXPECT_TRUE(std::string(wkt).find("AXIS") != std::string::npos) << wkt;
    }

    auto crs4979 = proj_create_from_wkt(
        m_ctxt,
        GeographicCRS::EPSG_4979->exportToWKT(WKTFormatter::create().get())
            .c_str(),
        nullptr, nullptr, nullptr);
    ObjectKeeper keeper_crs4979(crs4979);
    ASSERT_NE(crs4979, nullptr);

    // STRICT=NO
    {
        EXPECT_EQ(proj_as_wkt(m_ctxt, crs4979, PJ_WKT1_GDAL, nullptr), nullptr);

        const char *const options[] = {"STRICT=NO", nullptr};
        auto wkt = proj_as_wkt(m_ctxt, crs4979, PJ_WKT1_GDAL, options);
        ASSERT_NE(wkt, nullptr);
        EXPECT_TRUE(std::string(wkt).find("GEOGCS[\"WGS 84\"") == 0) << wkt;
    }

    // unsupported option
    {
        const char *const options[] = {"unsupported=yes", nullptr};
        auto wkt = proj_as_wkt(m_ctxt, obj, PJ_WKT2_2018, options);
        EXPECT_EQ(wkt, nullptr);
    }
}

// ---------------------------------------------------------------------------

TEST_F(CApi, proj_as_wkt_check_db_use) {
    auto obj = proj_create_from_wkt(
        m_ctxt, "GEOGCS[\"AGD66\",DATUM[\"Australian_Geodetic_Datum_1966\","
                "SPHEROID[\"Australian National Spheroid\",6378160,298.25]],"
                "PRIMEM[\"Greenwich\",0],UNIT[\"degree\",0.0174532925199433]]",
        nullptr, nullptr, nullptr);
    ObjectKeeper keeper(obj);
    ASSERT_NE(obj, nullptr);

    auto wkt = proj_as_wkt(m_ctxt, obj, PJ_WKT1_ESRI, nullptr);
    EXPECT_EQ(std::string(wkt),
              "GEOGCS[\"GCS_Australian_1966\",DATUM[\"D_Australian_1966\","
              "SPHEROID[\"Australian\",6378160.0,298.25]],"
              "PRIMEM[\"Greenwich\",0.0],"
              "UNIT[\"Degree\",0.0174532925199433]]");
}

// ---------------------------------------------------------------------------

TEST_F(CApi, proj_as_wkt_incompatible_WKT1) {
    auto wkt = createBoundCRS()->exportToWKT(WKTFormatter::create().get());
    auto obj =
        proj_create_from_wkt(m_ctxt, wkt.c_str(), nullptr, nullptr, nullptr);
    ObjectKeeper keeper(obj);
    ASSERT_NE(obj, nullptr) << wkt;

    auto wkt1_GDAL = proj_as_wkt(m_ctxt, obj, PJ_WKT1_GDAL, nullptr);
    ASSERT_EQ(wkt1_GDAL, nullptr);
}

// ---------------------------------------------------------------------------

TEST_F(CApi, proj_as_proj_string) {
    auto obj = proj_create_from_wkt(
        m_ctxt,
        GeographicCRS::EPSG_4326->exportToWKT(WKTFormatter::create().get())
            .c_str(),
        nullptr, nullptr, nullptr);
    ObjectKeeper keeper(obj);
    ASSERT_NE(obj, nullptr);

    {
        auto proj_5 = proj_as_proj_string(m_ctxt, obj, PJ_PROJ_5, nullptr);
        ASSERT_NE(proj_5, nullptr);
        EXPECT_EQ(std::string(proj_5),
                  "+proj=longlat +datum=WGS84 +no_defs +type=crs");
    }
    {
        auto proj_4 = proj_as_proj_string(m_ctxt, obj, PJ_PROJ_4, nullptr);
        ASSERT_NE(proj_4, nullptr);
        EXPECT_EQ(std::string(proj_4),
                  "+proj=longlat +datum=WGS84 +no_defs +type=crs");
    }
}

// ---------------------------------------------------------------------------

TEST_F(CApi, proj_as_proj_string_incompatible_WKT1) {
    auto obj = proj_create_from_wkt(
        m_ctxt,
        createBoundCRS()->exportToWKT(WKTFormatter::create().get()).c_str(),
        nullptr, nullptr, nullptr);
    ObjectKeeper keeper(obj);
    ASSERT_NE(obj, nullptr);

    auto str = proj_as_proj_string(m_ctxt, obj, PJ_PROJ_5, nullptr);
    ASSERT_EQ(str, nullptr);
}

// ---------------------------------------------------------------------------

TEST_F(CApi, proj_as_proj_string_approx_tmerc_option_yes) {
    auto obj = proj_create(m_ctxt, "+proj=tmerc +type=crs");
    ObjectKeeper keeper(obj);
    ASSERT_NE(obj, nullptr);

    const char *options[] = {"USE_APPROX_TMERC=YES", nullptr};
    auto str = proj_as_proj_string(m_ctxt, obj, PJ_PROJ_4, options);
    ASSERT_NE(str, nullptr);
    EXPECT_EQ(str,
              std::string("+proj=tmerc +approx +lat_0=0 +lon_0=0 +k=1 +x_0=0 "
                          "+y_0=0 +datum=WGS84 +units=m +no_defs +type=crs"));
}

// ---------------------------------------------------------------------------

TEST_F(CApi, proj_crs_create_bound_crs_to_WGS84) {
    auto crs = proj_create_from_database(m_ctxt, "EPSG", "3844",
                                         PJ_CATEGORY_CRS, false, nullptr);
    ObjectKeeper keeper(crs);
    ASSERT_NE(crs, nullptr);

    auto res = proj_crs_create_bound_crs_to_WGS84(m_ctxt, crs, nullptr);
    ObjectKeeper keeper_res(res);
    ASSERT_NE(res, nullptr);

    auto proj_4 = proj_as_proj_string(m_ctxt, res, PJ_PROJ_4, nullptr);
    ASSERT_NE(proj_4, nullptr);
    EXPECT_EQ(std::string(proj_4),
              "+proj=sterea +lat_0=46 +lon_0=25 +k=0.99975 +x_0=500000 "
              "+y_0=500000 +ellps=krass "
              "+towgs84=2.329,-147.042,-92.08,-0.309,0.325,0.497,5.69 "
              "+units=m +no_defs +type=crs");

    auto base_crs = proj_get_source_crs(m_ctxt, res);
    ObjectKeeper keeper_base_crs(base_crs);
    ASSERT_NE(base_crs, nullptr);

    auto hub_crs = proj_get_target_crs(m_ctxt, res);
    ObjectKeeper keeper_hub_crs(hub_crs);
    ASSERT_NE(hub_crs, nullptr);

    auto transf = proj_crs_get_coordoperation(m_ctxt, res);
    ObjectKeeper keeper_transf(transf);
    ASSERT_NE(transf, nullptr);

    std::vector<double> values(7, 0);
    EXPECT_TRUE(proj_coordoperation_get_towgs84_values(m_ctxt, transf,
                                                       values.data(), 7, true));
    auto expected = std::vector<double>{2.329, -147.042, -92.08, -0.309,
                                        0.325, 0.497,    5.69};
    EXPECT_EQ(values, expected);

    auto res2 = proj_crs_create_bound_crs(m_ctxt, base_crs, hub_crs, transf);
    ObjectKeeper keeper_res2(res2);
    ASSERT_NE(res2, nullptr);

    EXPECT_TRUE(proj_is_equivalent_to(res, res2, PJ_COMP_STRICT));
}

// ---------------------------------------------------------------------------

TEST_F(CApi, proj_crs_create_bound_crs_to_WGS84_on_invalid_type) {
    auto wkt = createProjectedCRS()->derivingConversion()->exportToWKT(
        WKTFormatter::create().get());
    auto obj =
        proj_create_from_wkt(m_ctxt, wkt.c_str(), nullptr, nullptr, nullptr);
    ObjectKeeper keeper(obj);
    ASSERT_NE(obj, nullptr) << wkt;

    auto res = proj_crs_create_bound_crs_to_WGS84(m_ctxt, obj, nullptr);
    ASSERT_EQ(res, nullptr);
}

// ---------------------------------------------------------------------------

TEST_F(CApi, proj_get_name) {
    auto obj = proj_create_from_wkt(
        m_ctxt,
        GeographicCRS::EPSG_4326->exportToWKT(WKTFormatter::create().get())
            .c_str(),
        nullptr, nullptr, nullptr);
    ObjectKeeper keeper(obj);
    ASSERT_NE(obj, nullptr);
    auto name = proj_get_name(obj);
    ASSERT_TRUE(name != nullptr);
    EXPECT_EQ(name, std::string("WGS 84"));
    EXPECT_EQ(name, proj_get_name(obj));
}

// ---------------------------------------------------------------------------

TEST_F(CApi, proj_get_id_auth_name) {
    auto obj = proj_create_from_wkt(
        m_ctxt,
        GeographicCRS::EPSG_4326->exportToWKT(WKTFormatter::create().get())
            .c_str(),
        nullptr, nullptr, nullptr);
    ObjectKeeper keeper(obj);
    ASSERT_NE(obj, nullptr);
    auto auth = proj_get_id_auth_name(obj, 0);
    ASSERT_TRUE(auth != nullptr);
    EXPECT_EQ(auth, std::string("EPSG"));
    EXPECT_EQ(auth, proj_get_id_auth_name(obj, 0));
    EXPECT_EQ(proj_get_id_auth_name(obj, -1), nullptr);
    EXPECT_EQ(proj_get_id_auth_name(obj, 1), nullptr);
}

// ---------------------------------------------------------------------------

TEST_F(CApi, proj_get_id_code) {
    auto obj = proj_create_from_wkt(
        m_ctxt,
        GeographicCRS::EPSG_4326->exportToWKT(WKTFormatter::create().get())
            .c_str(),
        nullptr, nullptr, nullptr);
    ObjectKeeper keeper(obj);
    ASSERT_NE(obj, nullptr);
    auto code = proj_get_id_code(obj, 0);
    ASSERT_TRUE(code != nullptr);
    EXPECT_EQ(code, std::string("4326"));
    EXPECT_EQ(code, proj_get_id_code(obj, 0));
    EXPECT_EQ(proj_get_id_code(obj, -1), nullptr);
    EXPECT_EQ(proj_get_id_code(obj, 1), nullptr);
}

// ---------------------------------------------------------------------------

TEST_F(CApi, proj_get_type) {
    {
        auto obj = proj_create_from_wkt(
            m_ctxt,
            GeographicCRS::EPSG_4326->exportToWKT(WKTFormatter::create().get())
                .c_str(),
            nullptr, nullptr, nullptr);
        ObjectKeeper keeper(obj);
        ASSERT_NE(obj, nullptr);
        EXPECT_EQ(proj_get_type(obj), PJ_TYPE_GEOGRAPHIC_2D_CRS);
    }
    {
        auto obj = proj_create_from_wkt(
            m_ctxt,
            GeographicCRS::EPSG_4979->exportToWKT(WKTFormatter::create().get())
                .c_str(),
            nullptr, nullptr, nullptr);
        ObjectKeeper keeper(obj);
        ASSERT_NE(obj, nullptr);
        EXPECT_EQ(proj_get_type(obj), PJ_TYPE_GEOGRAPHIC_3D_CRS);
    }
    {
        auto obj = proj_create_from_wkt(
            m_ctxt,
            GeographicCRS::EPSG_4978->exportToWKT(WKTFormatter::create().get())
                .c_str(),
            nullptr, nullptr, nullptr);
        ObjectKeeper keeper(obj);
        ASSERT_NE(obj, nullptr);
        EXPECT_EQ(proj_get_type(obj), PJ_TYPE_GEOCENTRIC_CRS);
    }
    {
        auto obj = proj_create_from_wkt(
            m_ctxt, GeographicCRS::EPSG_4326->datum()
                        ->exportToWKT(WKTFormatter::create().get())
                        .c_str(),
            nullptr, nullptr, nullptr);
        ObjectKeeper keeper(obj);
        ASSERT_NE(obj, nullptr);
        EXPECT_EQ(proj_get_type(obj), PJ_TYPE_GEODETIC_REFERENCE_FRAME);
    }
    {
        auto obj = proj_create_from_wkt(
            m_ctxt, GeographicCRS::EPSG_4326->ellipsoid()
                        ->exportToWKT(WKTFormatter::create().get())
                        .c_str(),
            nullptr, nullptr, nullptr);
        ObjectKeeper keeper(obj);
        ASSERT_NE(obj, nullptr);
        EXPECT_EQ(proj_get_type(obj), PJ_TYPE_ELLIPSOID);
    }
    {
        auto obj = proj_create_from_wkt(
            m_ctxt, createProjectedCRS()
                        ->exportToWKT(WKTFormatter::create().get())
                        .c_str(),
            nullptr, nullptr, nullptr);
        ObjectKeeper keeper(obj);
        ASSERT_NE(obj, nullptr);
        EXPECT_EQ(proj_get_type(obj), PJ_TYPE_PROJECTED_CRS);
    }
    {
        auto obj = proj_create_from_wkt(
            m_ctxt, createVerticalCRS()
                        ->exportToWKT(WKTFormatter::create().get())
                        .c_str(),
            nullptr, nullptr, nullptr);
        ObjectKeeper keeper(obj);
        ASSERT_NE(obj, nullptr);
        EXPECT_EQ(proj_get_type(obj), PJ_TYPE_VERTICAL_CRS);
    }
    {
        auto obj = proj_create_from_wkt(
            m_ctxt, createVerticalCRS()
                        ->datum()
                        ->exportToWKT(WKTFormatter::create().get())
                        .c_str(),
            nullptr, nullptr, nullptr);
        ObjectKeeper keeper(obj);
        ASSERT_NE(obj, nullptr);
        EXPECT_EQ(proj_get_type(obj), PJ_TYPE_VERTICAL_REFERENCE_FRAME);
    }
    {
        auto obj = proj_create_from_wkt(
            m_ctxt, createProjectedCRS()
                        ->derivingConversion()
                        ->exportToWKT(WKTFormatter::create().get())
                        .c_str(),
            nullptr, nullptr, nullptr);
        ObjectKeeper keeper(obj);
        ASSERT_NE(obj, nullptr);
        EXPECT_EQ(proj_get_type(obj), PJ_TYPE_CONVERSION);
    }
    {
        auto obj = proj_create_from_wkt(
            m_ctxt,
            createBoundCRS()->exportToWKT(WKTFormatter::create().get()).c_str(),
            nullptr, nullptr, nullptr);
        ObjectKeeper keeper(obj);
        ASSERT_NE(obj, nullptr);
        EXPECT_EQ(proj_get_type(obj), PJ_TYPE_BOUND_CRS);
    }
    {
        auto obj = proj_create_from_wkt(
            m_ctxt, createBoundCRS()
                        ->transformation()
                        ->exportToWKT(WKTFormatter::create().get())
                        .c_str(),
            nullptr, nullptr, nullptr);
        ObjectKeeper keeper(obj);
        ASSERT_NE(obj, nullptr);
        EXPECT_EQ(proj_get_type(obj), PJ_TYPE_TRANSFORMATION);
    }
    {
        auto obj = proj_create_from_wkt(m_ctxt, "AUTHORITY[\"EPSG\", 4326]",
                                        nullptr, nullptr, nullptr);
        ObjectKeeper keeper(obj);
        ASSERT_EQ(obj, nullptr);
    }
}

// ---------------------------------------------------------------------------

TEST_F(CApi, proj_create_from_database) {
    {
        auto crs = proj_create_from_database(m_ctxt, "EPSG", "-1",
                                             PJ_CATEGORY_CRS, false, nullptr);
        ASSERT_EQ(crs, nullptr);
    }
    {
        auto crs = proj_create_from_database(m_ctxt, "EPSG", "4326",
                                             PJ_CATEGORY_CRS, false, nullptr);
        ASSERT_NE(crs, nullptr);
        ObjectKeeper keeper(crs);
        EXPECT_TRUE(proj_is_crs(crs));
        EXPECT_FALSE(proj_is_deprecated(crs));
        EXPECT_EQ(proj_get_type(crs), PJ_TYPE_GEOGRAPHIC_2D_CRS);

        const char *code = proj_get_id_code(crs, 0);
        ASSERT_NE(code, nullptr);
        EXPECT_EQ(std::string(code), "4326");
    }
    {
        auto crs = proj_create_from_database(m_ctxt, "EPSG", "6871",
                                             PJ_CATEGORY_CRS, false, nullptr);
        ASSERT_NE(crs, nullptr);
        ObjectKeeper keeper(crs);
        EXPECT_TRUE(proj_is_crs(crs));
        EXPECT_EQ(proj_get_type(crs), PJ_TYPE_COMPOUND_CRS);
    }
    {
        auto ellipsoid = proj_create_from_database(
            m_ctxt, "EPSG", "7030", PJ_CATEGORY_ELLIPSOID, false, nullptr);
        ASSERT_NE(ellipsoid, nullptr);
        ObjectKeeper keeper(ellipsoid);
        EXPECT_EQ(proj_get_type(ellipsoid), PJ_TYPE_ELLIPSOID);
    }
    {
        auto pm = proj_create_from_database(
            m_ctxt, "EPSG", "8903", PJ_CATEGORY_PRIME_MERIDIAN, false, nullptr);
        ASSERT_NE(pm, nullptr);
        ObjectKeeper keeper(pm);
        EXPECT_EQ(proj_get_type(pm), PJ_TYPE_PRIME_MERIDIAN);
    }
    {
        auto datum = proj_create_from_database(
            m_ctxt, "EPSG", "6326", PJ_CATEGORY_DATUM, false, nullptr);
        ASSERT_NE(datum, nullptr);
        ObjectKeeper keeper(datum);
        EXPECT_EQ(proj_get_type(datum), PJ_TYPE_GEODETIC_REFERENCE_FRAME);
    }
    {
        auto op = proj_create_from_database(m_ctxt, "EPSG", "16031",
                                            PJ_CATEGORY_COORDINATE_OPERATION,
                                            false, nullptr);
        ASSERT_NE(op, nullptr);
        ObjectKeeper keeper(op);
        EXPECT_EQ(proj_get_type(op), PJ_TYPE_CONVERSION);

        auto info = proj_pj_info(op);
        EXPECT_NE(info.id, nullptr);
        EXPECT_EQ(info.id, std::string("utm"));
        ASSERT_NE(info.description, nullptr);
        EXPECT_EQ(info.description, std::string("UTM zone 31N"));
        ASSERT_NE(info.definition, nullptr);
        EXPECT_EQ(info.definition, std::string("proj=utm zone=31 ellps=GRS80"));
        EXPECT_EQ(info.accuracy, 0);
    }
    {
        auto op = proj_create_from_database(m_ctxt, "EPSG", "1024",
                                            PJ_CATEGORY_COORDINATE_OPERATION,
                                            false, nullptr);
        ASSERT_NE(op, nullptr);
        ObjectKeeper keeper(op);
        EXPECT_EQ(proj_get_type(op), PJ_TYPE_TRANSFORMATION);

        auto info = proj_pj_info(op);
        EXPECT_NE(info.id, nullptr);
        EXPECT_EQ(info.id, std::string("pipeline"));
        ASSERT_NE(info.description, nullptr);
        EXPECT_EQ(info.description, std::string("MGI to ETRS89 (4)"));
        ASSERT_NE(info.definition, nullptr);
        EXPECT_EQ(
            info.definition,
            std::string("proj=pipeline step proj=axisswap "
                        "order=2,1 step proj=unitconvert xy_in=deg xy_out=rad "
                        "step proj=push v_3 "
                        "step proj=cart ellps=bessel step proj=helmert "
                        "x=601.705 y=84.263 z=485.227 rx=-4.7354 ry=-1.3145 "
                        "rz=-5.393 s=-2.3887 convention=coordinate_frame "
                        "step inv proj=cart ellps=GRS80 "
                        "step proj=pop v_3 "
                        "step proj=unitconvert xy_in=rad xy_out=deg "
                        "step proj=axisswap order=2,1"));
        EXPECT_EQ(info.accuracy, 1);
    }
}

// ---------------------------------------------------------------------------

TEST_F(CApi, proj_crs) {
    auto crs = proj_create_from_wkt(
        m_ctxt,
        createProjectedCRS()
            ->exportToWKT(
                WKTFormatter::create(WKTFormatter::Convention::WKT1_GDAL).get())
            .c_str(),
        nullptr, nullptr, nullptr);
    ASSERT_NE(crs, nullptr);
    ObjectKeeper keeper(crs);
    EXPECT_TRUE(proj_is_crs(crs));

    auto geodCRS = proj_crs_get_geodetic_crs(m_ctxt, crs);
    ASSERT_NE(geodCRS, nullptr);
    ObjectKeeper keeper_geogCRS(geodCRS);
    EXPECT_TRUE(proj_is_crs(geodCRS));
    auto geogCRS_name = proj_get_name(geodCRS);
    ASSERT_TRUE(geogCRS_name != nullptr);
    EXPECT_EQ(geogCRS_name, std::string("WGS 84"));

    auto h_datum = proj_crs_get_horizontal_datum(m_ctxt, crs);
    ASSERT_NE(h_datum, nullptr);
    ObjectKeeper keeper_h_datum(h_datum);

    auto datum = proj_crs_get_datum(m_ctxt, crs);
    ASSERT_NE(datum, nullptr);
    ObjectKeeper keeper_datum(datum);

    EXPECT_TRUE(proj_is_equivalent_to(h_datum, datum, PJ_COMP_STRICT));

    auto datum_name = proj_get_name(datum);
    ASSERT_TRUE(datum_name != nullptr);
    EXPECT_EQ(datum_name, std::string("World Geodetic System 1984"));

    auto ellipsoid = proj_get_ellipsoid(m_ctxt, crs);
    ASSERT_NE(ellipsoid, nullptr);
    ObjectKeeper keeper_ellipsoid(ellipsoid);
    auto ellipsoid_name = proj_get_name(ellipsoid);
    ASSERT_TRUE(ellipsoid_name != nullptr);
    EXPECT_EQ(ellipsoid_name, std::string("WGS 84"));

    auto ellipsoid_from_datum = proj_get_ellipsoid(m_ctxt, datum);
    ASSERT_NE(ellipsoid_from_datum, nullptr);
    ObjectKeeper keeper_ellipsoid_from_datum(ellipsoid_from_datum);

    EXPECT_EQ(proj_get_ellipsoid(m_ctxt, ellipsoid), nullptr);
    EXPECT_FALSE(proj_is_crs(ellipsoid));

    double a;
    double b;
    int b_is_computed;
    double rf;
    EXPECT_TRUE(proj_ellipsoid_get_parameters(m_ctxt, ellipsoid, nullptr,
                                              nullptr, nullptr, nullptr));
    EXPECT_TRUE(proj_ellipsoid_get_parameters(m_ctxt, ellipsoid, &a, &b,
                                              &b_is_computed, &rf));
    EXPECT_FALSE(proj_ellipsoid_get_parameters(m_ctxt, crs, &a, &b,
                                               &b_is_computed, &rf));
    EXPECT_EQ(a, 6378137);
    EXPECT_NEAR(b, 6356752.31424518, 1e-9);
    EXPECT_EQ(b_is_computed, 1);
    EXPECT_EQ(rf, 298.257223563);
    auto id = proj_get_id_code(ellipsoid, 0);
    ASSERT_TRUE(id != nullptr);
    EXPECT_EQ(id, std::string("7030"));
}

// ---------------------------------------------------------------------------

TEST_F(CApi, proj_get_prime_meridian) {
    auto crs = proj_create_from_wkt(
        m_ctxt,
        createProjectedCRS()
            ->exportToWKT(
                WKTFormatter::create(WKTFormatter::Convention::WKT1_GDAL).get())
            .c_str(),
        nullptr, nullptr, nullptr);
    ASSERT_NE(crs, nullptr);
    ObjectKeeper keeper(crs);

    auto pm = proj_get_prime_meridian(m_ctxt, crs);
    ASSERT_NE(pm, nullptr);
    ObjectKeeper keeper_pm(pm);
    auto pm_name = proj_get_name(pm);
    ASSERT_TRUE(pm_name != nullptr);
    EXPECT_EQ(pm_name, std::string("Greenwich"));

    EXPECT_EQ(proj_get_prime_meridian(m_ctxt, pm), nullptr);

    EXPECT_TRUE(proj_prime_meridian_get_parameters(m_ctxt, pm, nullptr, nullptr,
                                                   nullptr));
    double longitude = -1;
    double longitude_unit = 0;
    const char *longitude_unit_name = nullptr;
    EXPECT_TRUE(proj_prime_meridian_get_parameters(
        m_ctxt, pm, &longitude, &longitude_unit, &longitude_unit_name));
    EXPECT_EQ(longitude, 0);
    EXPECT_NEAR(longitude_unit, UnitOfMeasure::DEGREE.conversionToSI(), 1e-10);
    ASSERT_TRUE(longitude_unit_name != nullptr);
    EXPECT_EQ(longitude_unit_name, std::string("degree"));

    auto datum = proj_crs_get_horizontal_datum(m_ctxt, crs);
    ASSERT_NE(datum, nullptr);
    ObjectKeeper keeper_datum(datum);
    auto pm_from_datum = proj_get_prime_meridian(m_ctxt, datum);
    ASSERT_NE(pm_from_datum, nullptr);
    ObjectKeeper keeper_pm_from_datum(pm_from_datum);
}

// ---------------------------------------------------------------------------

TEST_F(CApi, proj_crs_compound) {
    auto crs = proj_create_from_wkt(
        m_ctxt,
        createCompoundCRS()->exportToWKT(WKTFormatter::create().get()).c_str(),
        nullptr, nullptr, nullptr);
    ASSERT_NE(crs, nullptr);
    ObjectKeeper keeper(crs);
    EXPECT_EQ(proj_get_type(crs), PJ_TYPE_COMPOUND_CRS);

    EXPECT_EQ(proj_crs_get_sub_crs(m_ctxt, crs, -1), nullptr);
    EXPECT_EQ(proj_crs_get_sub_crs(m_ctxt, crs, 2), nullptr);

    auto subcrs_horiz = proj_crs_get_sub_crs(m_ctxt, crs, 0);
    ASSERT_NE(subcrs_horiz, nullptr);
    ObjectKeeper keeper_subcrs_horiz(subcrs_horiz);
    EXPECT_EQ(proj_get_type(subcrs_horiz), PJ_TYPE_PROJECTED_CRS);
    EXPECT_EQ(proj_crs_get_sub_crs(m_ctxt, subcrs_horiz, 0), nullptr);

    auto subcrs_vertical = proj_crs_get_sub_crs(m_ctxt, crs, 1);
    ASSERT_NE(subcrs_vertical, nullptr);
    ObjectKeeper keeper_subcrs_vertical(subcrs_vertical);
    EXPECT_EQ(proj_get_type(subcrs_vertical), PJ_TYPE_VERTICAL_CRS);
}

// ---------------------------------------------------------------------------

TEST_F(CApi, proj_get_source_target_crs_bound_crs) {
    auto crs = proj_create_from_wkt(
        m_ctxt,
        createBoundCRS()->exportToWKT(WKTFormatter::create().get()).c_str(),
        nullptr, nullptr, nullptr);
    ASSERT_NE(crs, nullptr);
    ObjectKeeper keeper(crs);

    auto sourceCRS = proj_get_source_crs(m_ctxt, crs);
    ASSERT_NE(sourceCRS, nullptr);
    ObjectKeeper keeper_sourceCRS(sourceCRS);
    EXPECT_EQ(std::string(proj_get_name(sourceCRS)), "NTF (Paris)");

    auto targetCRS = proj_get_target_crs(m_ctxt, crs);
    ASSERT_NE(targetCRS, nullptr);
    ObjectKeeper keeper_targetCRS(targetCRS);
    EXPECT_EQ(std::string(proj_get_name(targetCRS)), "WGS 84");
}

// ---------------------------------------------------------------------------

TEST_F(CApi, proj_get_source_target_crs_transformation) {
    auto obj = proj_create_from_wkt(
        m_ctxt, createBoundCRS()
                    ->transformation()
                    ->exportToWKT(WKTFormatter::create().get())
                    .c_str(),
        nullptr, nullptr, nullptr);
    ASSERT_NE(obj, nullptr);
    ObjectKeeper keeper(obj);

    auto sourceCRS = proj_get_source_crs(m_ctxt, obj);
    ASSERT_NE(sourceCRS, nullptr);
    ObjectKeeper keeper_sourceCRS(sourceCRS);
    EXPECT_EQ(std::string(proj_get_name(sourceCRS)), "NTF (Paris)");

    auto targetCRS = proj_get_target_crs(m_ctxt, obj);
    ASSERT_NE(targetCRS, nullptr);
    ObjectKeeper keeper_targetCRS(targetCRS);
    EXPECT_EQ(std::string(proj_get_name(targetCRS)), "WGS 84");
}

// ---------------------------------------------------------------------------

TEST_F(CApi, proj_get_source_crs_of_projected_crs) {
    auto crs = proj_create_from_wkt(
        m_ctxt,
        createProjectedCRS()->exportToWKT(WKTFormatter::create().get()).c_str(),
        nullptr, nullptr, nullptr);
    ASSERT_NE(crs, nullptr);
    ObjectKeeper keeper(crs);

    auto sourceCRS = proj_get_source_crs(m_ctxt, crs);
    ASSERT_NE(sourceCRS, nullptr);
    ObjectKeeper keeper_sourceCRS(sourceCRS);
    EXPECT_EQ(std::string(proj_get_name(sourceCRS)), "WGS 84");
}

// ---------------------------------------------------------------------------

TEST_F(CApi, proj_get_source_target_crs_conversion_without_crs) {
    auto obj = proj_create_from_database(m_ctxt, "EPSG", "16031",
                                         PJ_CATEGORY_COORDINATE_OPERATION,
                                         false, nullptr);
    ASSERT_NE(obj, nullptr);
    ObjectKeeper keeper(obj);

    auto sourceCRS = proj_get_source_crs(m_ctxt, obj);
    ASSERT_EQ(sourceCRS, nullptr);

    auto targetCRS = proj_get_target_crs(m_ctxt, obj);
    ASSERT_EQ(targetCRS, nullptr);
}

// ---------------------------------------------------------------------------

TEST_F(CApi, proj_get_source_target_crs_invalid_object) {
    auto obj = proj_create_from_wkt(
        m_ctxt, "ELLIPSOID[\"WGS 84\",6378137,298.257223563]", nullptr, nullptr,
        nullptr);
    ASSERT_NE(obj, nullptr);
    ObjectKeeper keeper(obj);

    auto sourceCRS = proj_get_source_crs(m_ctxt, obj);
    ASSERT_EQ(sourceCRS, nullptr);

    auto targetCRS = proj_get_target_crs(m_ctxt, obj);
    ASSERT_EQ(targetCRS, nullptr);
}

// ---------------------------------------------------------------------------

struct ListFreer {
    PROJ_STRING_LIST list;
    ListFreer(PROJ_STRING_LIST ptrIn) : list(ptrIn) {}
    ~ListFreer() { proj_string_list_destroy(list); }
    ListFreer(const ListFreer &) = delete;
    ListFreer &operator=(const ListFreer &) = delete;
};

// ---------------------------------------------------------------------------

TEST_F(CApi, proj_get_authorities_from_database) {
    auto list = proj_get_authorities_from_database(m_ctxt);
    ListFreer feer(list);
    ASSERT_NE(list, nullptr);
    ASSERT_TRUE(list[0] != nullptr);
    EXPECT_EQ(list[0], std::string("EPSG"));
    ASSERT_TRUE(list[1] != nullptr);
    EXPECT_EQ(list[1], std::string("ESRI"));
    ASSERT_TRUE(list[2] != nullptr);
    EXPECT_EQ(list[2], std::string("IGNF"));
    ASSERT_TRUE(list[3] != nullptr);
    EXPECT_EQ(list[3], std::string("OGC"));
    ASSERT_TRUE(list[4] != nullptr);
    EXPECT_EQ(list[4], std::string("PROJ"));
    EXPECT_EQ(list[5], nullptr);
}

// ---------------------------------------------------------------------------

TEST_F(CApi, proj_get_codes_from_database) {

    auto listTypes =
        std::vector<PJ_TYPE>{PJ_TYPE_ELLIPSOID,

                             PJ_TYPE_PRIME_MERIDIAN,

                             PJ_TYPE_GEODETIC_REFERENCE_FRAME,
                             PJ_TYPE_DYNAMIC_GEODETIC_REFERENCE_FRAME,
                             PJ_TYPE_VERTICAL_REFERENCE_FRAME,
                             PJ_TYPE_DYNAMIC_VERTICAL_REFERENCE_FRAME,
                             PJ_TYPE_DATUM_ENSEMBLE,

                             PJ_TYPE_CRS,
                             PJ_TYPE_GEODETIC_CRS,
                             PJ_TYPE_GEOCENTRIC_CRS,
                             PJ_TYPE_GEOGRAPHIC_CRS,
                             PJ_TYPE_GEOGRAPHIC_2D_CRS,
                             PJ_TYPE_GEOGRAPHIC_3D_CRS,
                             PJ_TYPE_VERTICAL_CRS,
                             PJ_TYPE_PROJECTED_CRS,
                             PJ_TYPE_COMPOUND_CRS,
                             PJ_TYPE_TEMPORAL_CRS,
                             PJ_TYPE_BOUND_CRS,
                             PJ_TYPE_OTHER_CRS,

                             PJ_TYPE_CONVERSION,
                             PJ_TYPE_TRANSFORMATION,
                             PJ_TYPE_CONCATENATED_OPERATION,
                             PJ_TYPE_OTHER_COORDINATE_OPERATION,

                             PJ_TYPE_UNKNOWN};
    for (const auto &type : listTypes) {
        auto list = proj_get_codes_from_database(m_ctxt, "EPSG", type, true);
        ListFreer feer(list);
        if (type == PJ_TYPE_TEMPORAL_CRS || type == PJ_TYPE_BOUND_CRS ||
            type == PJ_TYPE_UNKNOWN) {
            EXPECT_EQ(list, nullptr) << type;
        } else {
            ASSERT_NE(list, nullptr) << type;
            ASSERT_NE(list[0], nullptr) << type;
        }
    }
}

// ---------------------------------------------------------------------------

TEST_F(CApi, conversion) {
    auto crs = proj_create_from_database(m_ctxt, "EPSG", "32631",
                                         PJ_CATEGORY_CRS, false, nullptr);
    ASSERT_NE(crs, nullptr);
    ObjectKeeper keeper(crs);

    // invalid object type
    EXPECT_FALSE(proj_coordoperation_get_method_info(m_ctxt, crs, nullptr,
                                                     nullptr, nullptr));

    {
        auto conv = proj_crs_get_coordoperation(m_ctxt, crs);
        ASSERT_NE(conv, nullptr);
        ObjectKeeper keeper_conv(conv);

        ASSERT_EQ(proj_crs_get_coordoperation(m_ctxt, conv), nullptr);
    }

    auto conv = proj_crs_get_coordoperation(m_ctxt, crs);
    ASSERT_NE(conv, nullptr);
    ObjectKeeper keeper_conv(conv);

    EXPECT_TRUE(proj_coordoperation_get_method_info(m_ctxt, conv, nullptr,
                                                    nullptr, nullptr));

    const char *methodName = nullptr;
    const char *methodAuthorityName = nullptr;
    const char *methodCode = nullptr;
    EXPECT_TRUE(proj_coordoperation_get_method_info(
        m_ctxt, conv, &methodName, &methodAuthorityName, &methodCode));

    ASSERT_NE(methodName, nullptr);
    ASSERT_NE(methodAuthorityName, nullptr);
    ASSERT_NE(methodCode, nullptr);
    EXPECT_EQ(methodName, std::string("Transverse Mercator"));
    EXPECT_EQ(methodAuthorityName, std::string("EPSG"));
    EXPECT_EQ(methodCode, std::string("9807"));

    EXPECT_EQ(proj_coordoperation_get_param_count(m_ctxt, conv), 5);
    EXPECT_EQ(proj_coordoperation_get_param_index(m_ctxt, conv, "foo"), -1);
    EXPECT_EQ(
        proj_coordoperation_get_param_index(m_ctxt, conv, "False easting"), 3);

    EXPECT_FALSE(proj_coordoperation_get_param(
        m_ctxt, conv, -1, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
        nullptr, nullptr, nullptr, nullptr));
    EXPECT_FALSE(proj_coordoperation_get_param(
        m_ctxt, conv, 5, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
        nullptr, nullptr, nullptr, nullptr));

    const char *name = nullptr;
    const char *nameAuthorityName = nullptr;
    const char *nameCode = nullptr;
    double value = 0;
    const char *valueString = nullptr;
    double valueUnitConvFactor = 0;
    const char *valueUnitName = nullptr;
    const char *unitAuthName = nullptr;
    const char *unitCode = nullptr;
    const char *unitCategory = nullptr;
    EXPECT_TRUE(proj_coordoperation_get_param(
        m_ctxt, conv, 3, &name, &nameAuthorityName, &nameCode, &value,
        &valueString, &valueUnitConvFactor, &valueUnitName, &unitAuthName,
        &unitCode, &unitCategory));
    ASSERT_NE(name, nullptr);
    ASSERT_NE(nameAuthorityName, nullptr);
    ASSERT_NE(nameCode, nullptr);
    EXPECT_EQ(valueString, nullptr);
    ASSERT_NE(valueUnitName, nullptr);
    ASSERT_NE(unitAuthName, nullptr);
    ASSERT_NE(unitCategory, nullptr);
    ASSERT_NE(unitCategory, nullptr);
    EXPECT_EQ(name, std::string("False easting"));
    EXPECT_EQ(nameAuthorityName, std::string("EPSG"));
    EXPECT_EQ(nameCode, std::string("8806"));
    EXPECT_EQ(value, 500000.0);
    EXPECT_EQ(valueUnitConvFactor, 1.0);
    EXPECT_EQ(valueUnitName, std::string("metre"));
    EXPECT_EQ(unitAuthName, std::string("EPSG"));
    EXPECT_EQ(unitCode, std::string("9001"));
    EXPECT_EQ(unitCategory, std::string("linear"));
}

// ---------------------------------------------------------------------------

TEST_F(CApi, transformation_from_boundCRS) {
    auto crs = proj_create_from_wkt(
        m_ctxt,
        createBoundCRS()->exportToWKT(WKTFormatter::create().get()).c_str(),
        nullptr, nullptr, nullptr);
    ASSERT_NE(crs, nullptr);
    ObjectKeeper keeper(crs);

    auto transf = proj_crs_get_coordoperation(m_ctxt, crs);
    ASSERT_NE(transf, nullptr);
    ObjectKeeper keeper_transf(transf);
}

// ---------------------------------------------------------------------------

TEST_F(CApi, proj_coordoperation_get_grid_used) {
    auto op = proj_create_from_database(m_ctxt, "EPSG", "1312",
                                        PJ_CATEGORY_COORDINATE_OPERATION, true,
                                        nullptr);
    ASSERT_NE(op, nullptr);
    ObjectKeeper keeper(op);

    EXPECT_EQ(proj_coordoperation_get_grid_used_count(m_ctxt, op), 1);
    const char *shortName = nullptr;
    const char *fullName = nullptr;
    const char *packageName = nullptr;
    const char *url = nullptr;
    int directDownload = 0;
    int openLicense = 0;
    int available = 0;
    EXPECT_EQ(proj_coordoperation_get_grid_used(m_ctxt, op, -1, nullptr,
                                                nullptr, nullptr, nullptr,
                                                nullptr, nullptr, nullptr),
              0);
    EXPECT_EQ(proj_coordoperation_get_grid_used(m_ctxt, op, 1, nullptr, nullptr,
                                                nullptr, nullptr, nullptr,
                                                nullptr, nullptr),
              0);
    EXPECT_EQ(proj_coordoperation_get_grid_used(
                  m_ctxt, op, 0, &shortName, &fullName, &packageName, &url,
                  &directDownload, &openLicense, &available),
              1);
    ASSERT_NE(shortName, nullptr);
    ASSERT_NE(fullName, nullptr);
    ASSERT_NE(packageName, nullptr);
    ASSERT_NE(url, nullptr);
    EXPECT_EQ(shortName, std::string("ntv1_can.dat"));
    // EXPECT_EQ(fullName, std::string(""));
    EXPECT_EQ(packageName, std::string("proj-datumgrid"));
    EXPECT_TRUE(std::string(url).find(
                    "https://download.osgeo.org/proj/proj-datumgrid-") == 0)
        << std::string(url);
    EXPECT_EQ(directDownload, 1);
    EXPECT_EQ(openLicense, 1);
}

// ---------------------------------------------------------------------------

TEST_F(CApi, proj_coordoperation_is_instantiable) {
    auto op = proj_create_from_database(m_ctxt, "EPSG", "1671",
                                        PJ_CATEGORY_COORDINATE_OPERATION, true,
                                        nullptr);
    ASSERT_NE(op, nullptr);
    ObjectKeeper keeper(op);
    EXPECT_EQ(proj_coordoperation_is_instantiable(m_ctxt, op), 1);
}

// ---------------------------------------------------------------------------

TEST_F(CApi, proj_create_operations) {
    auto ctxt = proj_create_operation_factory_context(m_ctxt, nullptr);
    ASSERT_NE(ctxt, nullptr);
    ContextKeeper keeper_ctxt(ctxt);

    auto source_crs = proj_create_from_database(
        m_ctxt, "EPSG", "4267", PJ_CATEGORY_CRS, false, nullptr); // NAD27
    ASSERT_NE(source_crs, nullptr);
    ObjectKeeper keeper_source_crs(source_crs);

    auto target_crs = proj_create_from_database(
        m_ctxt, "EPSG", "4269", PJ_CATEGORY_CRS, false, nullptr); // NAD83
    ASSERT_NE(target_crs, nullptr);
    ObjectKeeper keeper_target_crs(target_crs);

    proj_operation_factory_context_set_spatial_criterion(
        m_ctxt, ctxt, PROJ_SPATIAL_CRITERION_PARTIAL_INTERSECTION);

    proj_operation_factory_context_set_grid_availability_use(
        m_ctxt, ctxt, PROJ_GRID_AVAILABILITY_IGNORED);

    auto res = proj_create_operations(m_ctxt, source_crs, target_crs, ctxt);
    ASSERT_NE(res, nullptr);
    ObjListKeeper keeper_res(res);

    EXPECT_EQ(proj_list_get_count(res), 8);

    EXPECT_EQ(proj_list_get(m_ctxt, res, -1), nullptr);
    EXPECT_EQ(proj_list_get(m_ctxt, res, proj_list_get_count(res)), nullptr);
    auto op = proj_list_get(m_ctxt, res, 0);
    ASSERT_NE(op, nullptr);
    ObjectKeeper keeper_op(op);
    EXPECT_FALSE(proj_coordoperation_has_ballpark_transformation(m_ctxt, op));

    EXPECT_EQ(proj_get_name(op), std::string("NAD27 to NAD83 (3)"));
}

// ---------------------------------------------------------------------------

TEST_F(CApi, proj_create_operations_discard_superseded) {
    auto ctxt = proj_create_operation_factory_context(m_ctxt, nullptr);
    ASSERT_NE(ctxt, nullptr);
    ContextKeeper keeper_ctxt(ctxt);

    auto source_crs = proj_create_from_database(
        m_ctxt, "EPSG", "4203", PJ_CATEGORY_CRS, false, nullptr); // AGD84
    ASSERT_NE(source_crs, nullptr);
    ObjectKeeper keeper_source_crs(source_crs);

    auto target_crs = proj_create_from_database(
        m_ctxt, "EPSG", "4326", PJ_CATEGORY_CRS, false, nullptr); // WGS84
    ASSERT_NE(target_crs, nullptr);
    ObjectKeeper keeper_target_crs(target_crs);

    proj_operation_factory_context_set_spatial_criterion(
        m_ctxt, ctxt, PROJ_SPATIAL_CRITERION_PARTIAL_INTERSECTION);

    proj_operation_factory_context_set_grid_availability_use(
        m_ctxt, ctxt, PROJ_GRID_AVAILABILITY_IGNORED);

    proj_operation_factory_context_set_discard_superseded(m_ctxt, ctxt, true);

    auto res = proj_create_operations(m_ctxt, source_crs, target_crs, ctxt);
    ASSERT_NE(res, nullptr);
    ObjListKeeper keeper_res(res);

    EXPECT_EQ(proj_list_get_count(res), 2);
}

// ---------------------------------------------------------------------------

TEST_F(CApi, proj_create_operations_dont_discard_superseded) {
    auto ctxt = proj_create_operation_factory_context(m_ctxt, nullptr);
    ASSERT_NE(ctxt, nullptr);
    ContextKeeper keeper_ctxt(ctxt);

    auto source_crs = proj_create_from_database(
        m_ctxt, "EPSG", "4203", PJ_CATEGORY_CRS, false, nullptr); // AGD84
    ASSERT_NE(source_crs, nullptr);
    ObjectKeeper keeper_source_crs(source_crs);

    auto target_crs = proj_create_from_database(
        m_ctxt, "EPSG", "4326", PJ_CATEGORY_CRS, false, nullptr); // WGS84
    ASSERT_NE(target_crs, nullptr);
    ObjectKeeper keeper_target_crs(target_crs);

    proj_operation_factory_context_set_spatial_criterion(
        m_ctxt, ctxt, PROJ_SPATIAL_CRITERION_PARTIAL_INTERSECTION);

    proj_operation_factory_context_set_grid_availability_use(
        m_ctxt, ctxt, PROJ_GRID_AVAILABILITY_IGNORED);

    proj_operation_factory_context_set_discard_superseded(m_ctxt, ctxt, false);

    auto res = proj_create_operations(m_ctxt, source_crs, target_crs, ctxt);
    ASSERT_NE(res, nullptr);
    ObjListKeeper keeper_res(res);

    EXPECT_EQ(proj_list_get_count(res), 5);
}

// ---------------------------------------------------------------------------

TEST_F(CApi, proj_create_operations_with_pivot) {

    auto source_crs = proj_create_from_database(
        m_ctxt, "EPSG", "4326", PJ_CATEGORY_CRS, false, nullptr); // WGS84
    ASSERT_NE(source_crs, nullptr);
    ObjectKeeper keeper_source_crs(source_crs);

    auto target_crs = proj_create_from_database(
        m_ctxt, "EPSG", "6668", PJ_CATEGORY_CRS, false, nullptr); // JGD2011
    ASSERT_NE(target_crs, nullptr);
    ObjectKeeper keeper_target_crs(target_crs);

    // There is no direct transformations between both

    // Default behaviour: allow any pivot
    {
        auto ctxt = proj_create_operation_factory_context(m_ctxt, nullptr);
        ASSERT_NE(ctxt, nullptr);
        ContextKeeper keeper_ctxt(ctxt);

        auto res = proj_create_operations(m_ctxt, source_crs, target_crs, ctxt);
        ASSERT_NE(res, nullptr);
        ObjListKeeper keeper_res(res);
        EXPECT_EQ(proj_list_get_count(res), 1);
        auto op = proj_list_get(m_ctxt, res, 0);
        ASSERT_NE(op, nullptr);
        ObjectKeeper keeper_op(op);

        EXPECT_EQ(
            proj_get_name(op),
            std::string(
                "Inverse of JGD2000 to WGS 84 (1) + JGD2000 to JGD2011 (2)"));
    }

    // Disallow pivots
    {
        auto ctxt = proj_create_operation_factory_context(m_ctxt, nullptr);
        ASSERT_NE(ctxt, nullptr);
        ContextKeeper keeper_ctxt(ctxt);
        proj_operation_factory_context_set_allow_use_intermediate_crs(
            m_ctxt, ctxt, PROJ_INTERMEDIATE_CRS_USE_NEVER);

        auto res = proj_create_operations(m_ctxt, source_crs, target_crs, ctxt);
        ASSERT_NE(res, nullptr);
        ObjListKeeper keeper_res(res);
        EXPECT_EQ(proj_list_get_count(res), 1);
        auto op = proj_list_get(m_ctxt, res, 0);
        ASSERT_NE(op, nullptr);
        ObjectKeeper keeper_op(op);

        EXPECT_EQ(
            proj_get_name(op),
            std::string("Ballpark geographic offset from WGS 84 to JGD2011"));
    }

    // Restrict pivot to Tokyo CRS
    {
        auto ctxt = proj_create_operation_factory_context(m_ctxt, "EPSG");
        ASSERT_NE(ctxt, nullptr);
        ContextKeeper keeper_ctxt(ctxt);

        const char *pivots[] = {"EPSG", "4301", nullptr};
        proj_operation_factory_context_set_allowed_intermediate_crs(
            m_ctxt, ctxt, pivots);
        proj_operation_factory_context_set_spatial_criterion(
            m_ctxt, ctxt, PROJ_SPATIAL_CRITERION_PARTIAL_INTERSECTION);
        proj_operation_factory_context_set_grid_availability_use(
            m_ctxt, ctxt, PROJ_GRID_AVAILABILITY_IGNORED);

        auto res = proj_create_operations(m_ctxt, source_crs, target_crs, ctxt);
        ASSERT_NE(res, nullptr);
        ObjListKeeper keeper_res(res);
        EXPECT_EQ(proj_list_get_count(res), 7);
        auto op = proj_list_get(m_ctxt, res, 0);
        ASSERT_NE(op, nullptr);
        ObjectKeeper keeper_op(op);

        EXPECT_EQ(
            proj_get_name(op),
            std::string(
                "Inverse of Tokyo to WGS 84 (108) + Tokyo to JGD2011 (2)"));
    }

    // Restrict pivot to JGD2000
    {
        auto ctxt = proj_create_operation_factory_context(m_ctxt, "any");
        ASSERT_NE(ctxt, nullptr);
        ContextKeeper keeper_ctxt(ctxt);

        const char *pivots[] = {"EPSG", "4612", nullptr};
        proj_operation_factory_context_set_allowed_intermediate_crs(
            m_ctxt, ctxt, pivots);
        proj_operation_factory_context_set_spatial_criterion(
            m_ctxt, ctxt, PROJ_SPATIAL_CRITERION_PARTIAL_INTERSECTION);
        proj_operation_factory_context_set_grid_availability_use(
            m_ctxt, ctxt, PROJ_GRID_AVAILABILITY_IGNORED);
        proj_operation_factory_context_set_allow_use_intermediate_crs(
            m_ctxt, ctxt, PROJ_INTERMEDIATE_CRS_USE_ALWAYS);

        auto res = proj_create_operations(m_ctxt, source_crs, target_crs, ctxt);
        ASSERT_NE(res, nullptr);
        ObjListKeeper keeper_res(res);
        // includes results from ESRI
        EXPECT_EQ(proj_list_get_count(res), 4);
        auto op = proj_list_get(m_ctxt, res, 0);
        ASSERT_NE(op, nullptr);
        ObjectKeeper keeper_op(op);

        EXPECT_EQ(
            proj_get_name(op),
            std::string(
                "Inverse of JGD2000 to WGS 84 (1) + JGD2000 to JGD2011 (2)"));
    }
}

// ---------------------------------------------------------------------------

TEST_F(CApi, proj_context_set_database_path_null) {

    EXPECT_TRUE(
        proj_context_set_database_path(m_ctxt, nullptr, nullptr, nullptr));
    auto source_crs = proj_create_from_database(m_ctxt, "EPSG", "4326",
                                                PJ_CATEGORY_CRS, false,
                                                nullptr); // WGS84
    ASSERT_NE(source_crs, nullptr);
    ObjectKeeper keeper_source_crs(source_crs);
}

// ---------------------------------------------------------------------------

TEST_F(CApi, proj_context_set_database_path_main_memory_one_aux) {

    auto c_path = proj_context_get_database_path(m_ctxt);
    ASSERT_TRUE(c_path != nullptr);
    std::string path(c_path);
    const char *aux_db_list[] = {path.c_str(), nullptr};

    // This is super exotic and a miracle that it works. :memory: as the
    // main DB is empty. The real stuff is in the aux_db_list. No view
    // is created in the ':memory:' internal DB, but as there's only one
    // aux DB its tables and views can be directly queried...
    // If that breaks at some point, that wouldn't be a big issue.
    // Keeping that one as I had a hard time figuring out why it worked !
    // The real thing is tested by the C++
    // factory::attachExtraDatabases_auxiliary
    EXPECT_TRUE(proj_context_set_database_path(m_ctxt, ":memory:", aux_db_list,
                                               nullptr));

    auto source_crs = proj_create_from_database(m_ctxt, "EPSG", "4326",
                                                PJ_CATEGORY_CRS, false,
                                                nullptr); // WGS84
    ASSERT_NE(source_crs, nullptr);
    ObjectKeeper keeper_source_crs(source_crs);
}

// ---------------------------------------------------------------------------

TEST_F(CApi, proj_context_set_database_path_error_1) {

    EXPECT_FALSE(proj_context_set_database_path(m_ctxt, "i_do_not_exist.db",
                                                nullptr, nullptr));

    // We will eventually re-open on the default DB
    auto source_crs = proj_create_from_database(m_ctxt, "EPSG", "4326",
                                                PJ_CATEGORY_CRS, false,
                                                nullptr); // WGS84
    ASSERT_NE(source_crs, nullptr);
    ObjectKeeper keeper_source_crs(source_crs);
}

// ---------------------------------------------------------------------------

TEST_F(CApi, proj_context_set_database_path_error_2) {

    const char *aux_db_list[] = {"i_do_not_exist.db", nullptr};
    EXPECT_FALSE(
        proj_context_set_database_path(m_ctxt, nullptr, aux_db_list, nullptr));

    // We will eventually re-open on the default DB
    auto source_crs = proj_create_from_database(m_ctxt, "EPSG", "4326",
                                                PJ_CATEGORY_CRS, false,
                                                nullptr); // WGS84
    ASSERT_NE(source_crs, nullptr);
    ObjectKeeper keeper_source_crs(source_crs);
}

// ---------------------------------------------------------------------------

TEST_F(CApi, proj_context_guess_wkt_dialect) {

    EXPECT_EQ(proj_context_guess_wkt_dialect(nullptr, "LOCAL_CS[\"foo\"]"),
              PJ_GUESSED_WKT1_GDAL);

    EXPECT_EQ(proj_context_guess_wkt_dialect(
                  nullptr,
                  "GEOGCS[\"GCS_WGS_1984\",DATUM[\"D_WGS_1984\",SPHEROID[\"WGS_"
                  "1984\",6378137.0,298.257223563]],PRIMEM[\"Greenwich\",0.0],"
                  "UNIT[\"Degree\",0.0174532925199433]]"),
              PJ_GUESSED_WKT1_ESRI);

    EXPECT_EQ(proj_context_guess_wkt_dialect(
                  nullptr,
                  "GEOGCRS[\"WGS 84\",\n"
                  "    DATUM[\"World Geodetic System 1984\",\n"
                  "        ELLIPSOID[\"WGS 84\",6378137,298.257223563]],\n"
                  "    CS[ellipsoidal,2],\n"
                  "        AXIS[\"geodetic latitude (Lat)\",north],\n"
                  "        AXIS[\"geodetic longitude (Lon)\",east],\n"
                  "        UNIT[\"degree\",0.0174532925199433]]"),
              PJ_GUESSED_WKT2_2018);

    EXPECT_EQ(proj_context_guess_wkt_dialect(
                  nullptr,
                  "GEODCRS[\"WGS 84\",\n"
                  "    DATUM[\"World Geodetic System 1984\",\n"
                  "        ELLIPSOID[\"WGS 84\",6378137,298.257223563]],\n"
                  "    CS[ellipsoidal,2],\n"
                  "        AXIS[\"geodetic latitude (Lat)\",north],\n"
                  "        AXIS[\"geodetic longitude (Lon)\",east],\n"
                  "        UNIT[\"degree\",0.0174532925199433]]"),
              PJ_GUESSED_WKT2_2015);

    EXPECT_EQ(proj_context_guess_wkt_dialect(nullptr, "foo"),
              PJ_GUESSED_NOT_WKT);
}

// ---------------------------------------------------------------------------

TEST_F(CApi, proj_create_from_name) {
    /*
        PJ_OBJ_LIST PROJ_DLL *proj_create_from_name(
            PJ_CONTEXT *ctx,
            const char *auth_name,
            const char *searchedName,
            const PJ_TYPE* types,
            size_t typesCount,
            int approximateMatch,
            size_t limitResultCount,
            const char* const *options); */
    {
        auto res = proj_create_from_name(m_ctxt, nullptr, "WGS 84", nullptr, 0,
                                         false, 0, nullptr);
        ASSERT_NE(res, nullptr);
        ObjListKeeper keeper_res(res);
        EXPECT_EQ(proj_list_get_count(res), 4);
    }
    {
        auto res = proj_create_from_name(m_ctxt, "xx", "WGS 84", nullptr, 0,
                                         false, 0, nullptr);
        ASSERT_NE(res, nullptr);
        ObjListKeeper keeper_res(res);
        EXPECT_EQ(proj_list_get_count(res), 0);
    }
    {
        const PJ_TYPE types[] = {PJ_TYPE_GEODETIC_CRS, PJ_TYPE_PROJECTED_CRS};
        auto res = proj_create_from_name(m_ctxt, nullptr, "WGS 84", types, 2,
                                         true, 10, nullptr);
        ASSERT_NE(res, nullptr);
        ObjListKeeper keeper_res(res);
        EXPECT_EQ(proj_list_get_count(res), 10);
    }
}

// ---------------------------------------------------------------------------

TEST_F(CApi, proj_identify) {
    auto obj = proj_create_from_wkt(
        m_ctxt,
        GeographicCRS::EPSG_4807->exportToWKT(WKTFormatter::create().get())
            .c_str(),
        nullptr, nullptr, nullptr);
    ObjectKeeper keeper(obj);
    ASSERT_NE(obj, nullptr);
    {
        auto res = proj_identify(m_ctxt, obj, nullptr, nullptr, nullptr);
        ObjListKeeper keeper_res(res);
        EXPECT_EQ(proj_list_get_count(res), 1);
    }
    {
        int *confidence = nullptr;
        auto res = proj_identify(m_ctxt, obj, "EPSG", nullptr, &confidence);
        ObjListKeeper keeper_res(res);
        EXPECT_EQ(proj_list_get_count(res), 1);
        EXPECT_EQ(confidence[0], 100);
        proj_int_list_destroy(confidence);
    }
    {
        auto objEllps = proj_create_from_wkt(
            m_ctxt,
            Ellipsoid::GRS1980->exportToWKT(WKTFormatter::create().get())
                .c_str(),
            nullptr, nullptr, nullptr);
        ObjectKeeper keeperEllps(objEllps);
        ASSERT_NE(objEllps, nullptr);
        auto res = proj_identify(m_ctxt, objEllps, nullptr, nullptr, nullptr);
        ObjListKeeper keeper_res(res);
        EXPECT_EQ(res, nullptr);
    }
    {
        auto obj2 = proj_create(
            m_ctxt, "+proj=longlat +datum=WGS84 +no_defs +type=crs");
        ObjectKeeper keeper2(obj2);
        ASSERT_NE(obj2, nullptr);
        int *confidence = nullptr;
        auto res = proj_identify(m_ctxt, obj2, nullptr, nullptr, &confidence);
        ObjListKeeper keeper_res(res);
        EXPECT_EQ(proj_list_get_count(res), 4);
        proj_int_list_destroy(confidence);
    }
    {
        auto obj2 = proj_create_from_database(m_ctxt, "IGNF", "ETRS89UTM28",
                                              PJ_CATEGORY_CRS, false, nullptr);
        ObjectKeeper keeper2(obj2);
        ASSERT_NE(obj2, nullptr);
        int *confidence = nullptr;
        auto res = proj_identify(m_ctxt, obj2, "EPSG", nullptr, &confidence);
        ObjListKeeper keeper_res(res);
        EXPECT_EQ(proj_list_get_count(res), 1);
        auto gotCRS = proj_list_get(m_ctxt, res, 0);
        ASSERT_NE(gotCRS, nullptr);
        ObjectKeeper keeper_gotCRS(gotCRS);
        auto auth = proj_get_id_auth_name(gotCRS, 0);
        ASSERT_TRUE(auth != nullptr);
        EXPECT_EQ(auth, std::string("EPSG"));
        auto code = proj_get_id_code(gotCRS, 0);
        ASSERT_TRUE(code != nullptr);
        EXPECT_EQ(code, std::string("25828"));
        EXPECT_EQ(confidence[0], 70);
        proj_int_list_destroy(confidence);
    }
}

// ---------------------------------------------------------------------------

TEST_F(CApi, proj_get_area_of_use) {
    {
        auto crs = proj_create_from_database(m_ctxt, "EPSG", "4326",
                                             PJ_CATEGORY_CRS, false, nullptr);
        ASSERT_NE(crs, nullptr);
        ObjectKeeper keeper(crs);
        EXPECT_TRUE(proj_get_area_of_use(m_ctxt, crs, nullptr, nullptr, nullptr,
                                         nullptr, nullptr));
        const char *name = nullptr;
        double w;
        double s;
        double e;
        double n;
        EXPECT_TRUE(proj_get_area_of_use(m_ctxt, crs, &w, &s, &e, &n, &name));
        EXPECT_EQ(w, -180);
        EXPECT_EQ(s, -90);
        EXPECT_EQ(e, 180);
        EXPECT_EQ(n, 90);
        ASSERT_TRUE(name != nullptr);
        EXPECT_EQ(std::string(name), "World");
    }
    {
        auto obj = proj_create(m_ctxt, "+proj=longlat +type=crs");
        ObjectKeeper keeper(obj);
        ASSERT_NE(obj, nullptr);
        EXPECT_FALSE(proj_get_area_of_use(m_ctxt, obj, nullptr, nullptr,
                                          nullptr, nullptr, nullptr));
    }
}

// ---------------------------------------------------------------------------

TEST_F(CApi, proj_coordoperation_get_accuracy) {
    {
        auto crs = proj_create_from_database(m_ctxt, "EPSG", "4326",
                                             PJ_CATEGORY_CRS, false, nullptr);
        ASSERT_NE(crs, nullptr);
        ObjectKeeper keeper(crs);
        EXPECT_EQ(proj_coordoperation_get_accuracy(m_ctxt, crs), -1.0);
    }
    {
        auto obj = proj_create_from_database(m_ctxt, "EPSG", "1170",
                                             PJ_CATEGORY_COORDINATE_OPERATION,
                                             false, nullptr);
        ASSERT_NE(obj, nullptr);
        ObjectKeeper keeper(obj);
        EXPECT_EQ(proj_coordoperation_get_accuracy(m_ctxt, obj), 16.0);
    }
    {
        auto obj = proj_create(m_ctxt, "+proj=helmert");
        ObjectKeeper keeper(obj);
        ASSERT_NE(obj, nullptr);
        EXPECT_EQ(proj_coordoperation_get_accuracy(m_ctxt, obj), -1.0);
    }
}

// ---------------------------------------------------------------------------

TEST_F(CApi, proj_create_geographic_crs) {

    auto cs = proj_create_ellipsoidal_2D_cs(
        m_ctxt, PJ_ELLPS2D_LATITUDE_LONGITUDE, nullptr, 0);
    ObjectKeeper keeper_cs(cs);
    ASSERT_NE(cs, nullptr);

    {
        auto obj = proj_create_geographic_crs(
            m_ctxt, "WGS 84", "World Geodetic System 1984", "WGS 84", 6378137,
            298.257223563, "Greenwich", 0.0, "Degree", 0.0174532925199433, cs);
        ObjectKeeper keeper(obj);
        ASSERT_NE(obj, nullptr);

        auto objRef =
            proj_create(m_ctxt, GeographicCRS::EPSG_4326
                                    ->exportToWKT(WKTFormatter::create().get())
                                    .c_str());
        ObjectKeeper keeperobjRef(objRef);
        EXPECT_NE(objRef, nullptr);

        EXPECT_TRUE(proj_is_equivalent_to(obj, objRef, PJ_COMP_EQUIVALENT));

        auto datum = proj_crs_get_datum(m_ctxt, obj);
        ObjectKeeper keeper_datum(datum);
        ASSERT_NE(datum, nullptr);

        auto obj2 =
            proj_create_geographic_crs_from_datum(m_ctxt, "WGS 84", datum, cs);
        ObjectKeeper keeperObj(obj2);
        ASSERT_NE(obj2, nullptr);

        EXPECT_TRUE(proj_is_equivalent_to(obj, obj2, PJ_COMP_STRICT));
    }
    {
        auto obj =
            proj_create_geographic_crs(m_ctxt, nullptr, nullptr, nullptr, 1.0,
                                       0.0, nullptr, 0.0, nullptr, 0.0, cs);
        ObjectKeeper keeper(obj);
        ASSERT_NE(obj, nullptr);
    }

    // Datum with GDAL_WKT1 spelling: special case of WGS_1984
    {
        auto obj = proj_create_geographic_crs(
            m_ctxt, "WGS 84", "WGS_1984", "WGS 84", 6378137, 298.257223563,
            "Greenwich", 0.0, "Degree", 0.0174532925199433, cs);
        ObjectKeeper keeper(obj);
        ASSERT_NE(obj, nullptr);

        auto objRef =
            proj_create(m_ctxt, GeographicCRS::EPSG_4326
                                    ->exportToWKT(WKTFormatter::create().get())
                                    .c_str());
        ObjectKeeper keeperobjRef(objRef);
        EXPECT_NE(objRef, nullptr);

        EXPECT_TRUE(proj_is_equivalent_to(obj, objRef, PJ_COMP_EQUIVALENT));
    }

    // Datum with GDAL_WKT1 spelling: database query
    {
        auto obj = proj_create_geographic_crs(
            m_ctxt, "NAD83", "North_American_Datum_1983", "GRS 1980", 6378137,
            298.257222101, "Greenwich", 0.0, "Degree", 0.0174532925199433, cs);
        ObjectKeeper keeper(obj);
        ASSERT_NE(obj, nullptr);

        auto objRef =
            proj_create(m_ctxt, GeographicCRS::EPSG_4269
                                    ->exportToWKT(WKTFormatter::create().get())
                                    .c_str());
        ObjectKeeper keeperobjRef(objRef);
        EXPECT_NE(objRef, nullptr);

        EXPECT_TRUE(proj_is_equivalent_to(obj, objRef, PJ_COMP_EQUIVALENT));
    }

    // Datum with GDAL_WKT1 spelling: database query in alias_name table
    {
        auto crs = proj_create_geographic_crs(
            m_ctxt, "S-JTSK (Ferro)",
            "System_Jednotne_Trigonometricke_Site_Katastralni_Ferro",
            "Bessel 1841", 6377397.155, 299.1528128, "Ferro",
            -17.66666666666667, "Degree", 0.0174532925199433, cs);
        ObjectKeeper keeper(crs);
        ASSERT_NE(crs, nullptr);

        auto datum = proj_crs_get_datum(m_ctxt, crs);
        ASSERT_NE(datum, nullptr);
        ObjectKeeper keeper_datum(datum);

        auto datum_name = proj_get_name(datum);
        ASSERT_TRUE(datum_name != nullptr);
        EXPECT_EQ(datum_name,
                  std::string("System of the Unified Trigonometrical Cadastral "
                              "Network (Ferro)"));
    }

    // WKT1 with (deprecated)
    {
        auto crs = proj_create_geographic_crs(
            m_ctxt, "SAD69 (deprecated)", "South_American_Datum_1969",
            "GRS 1967", 6378160, 298.247167427, "Greenwich", 0, "Degree",
            0.0174532925199433, cs);
        ObjectKeeper keeper(crs);
        ASSERT_NE(crs, nullptr);

        auto name = proj_get_name(crs);
        ASSERT_TRUE(name != nullptr);
        EXPECT_EQ(name, std::string("SAD69"));
        EXPECT_TRUE(proj_is_deprecated(crs));
    }
}

// ---------------------------------------------------------------------------

TEST_F(CApi, proj_create_geocentric_crs) {
    {
        auto obj = proj_create_geocentric_crs(
            m_ctxt, "WGS 84", "World Geodetic System 1984", "WGS 84", 6378137,
            298.257223563, "Greenwich", 0.0, "Degree", 0.0174532925199433,
            "Metre", 1.0);
        ObjectKeeper keeper(obj);
        ASSERT_NE(obj, nullptr);

        auto objRef =
            proj_create(m_ctxt, GeographicCRS::EPSG_4978
                                    ->exportToWKT(WKTFormatter::create().get())
                                    .c_str());
        ObjectKeeper keeperobjRef(objRef);
        EXPECT_NE(objRef, nullptr);

        EXPECT_TRUE(proj_is_equivalent_to(obj, objRef, PJ_COMP_EQUIVALENT));

        auto datum = proj_crs_get_datum(m_ctxt, obj);
        ObjectKeeper keeper_datum(datum);
        ASSERT_NE(datum, nullptr);

        auto obj2 = proj_create_geocentric_crs_from_datum(m_ctxt, "WGS 84",
                                                          datum, "Metre", 1.0);
        ObjectKeeper keeperObj(obj2);
        ASSERT_NE(obj2, nullptr);

        EXPECT_TRUE(proj_is_equivalent_to(obj, obj2, PJ_COMP_STRICT));
    }
    {
        auto obj = proj_create_geocentric_crs(m_ctxt, nullptr, nullptr, nullptr,
                                              1.0, 0.0, nullptr, 0.0, nullptr,
                                              0.0, nullptr, 0.0);
        ObjectKeeper keeper(obj);
        ASSERT_NE(obj, nullptr);
    }
}

// ---------------------------------------------------------------------------

TEST_F(CApi, check_coord_op_obj_can_be_used_with_proj_trans) {

    {
        auto projCRS = proj_create_conversion_utm(m_ctxt, 31, true);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);

        PJ_COORD coord;
        coord.xyzt.x = proj_torad(3.0);
        coord.xyzt.y = 0;
        coord.xyzt.z = 0;
        coord.xyzt.t = 0;
        EXPECT_NEAR(proj_trans(projCRS, PJ_FWD, coord).xyzt.x, 500000.0, 1e-9);
    }
}

// ---------------------------------------------------------------------------

TEST_F(CApi, proj_create_projections) {

    /* BEGIN: Generated by scripts/create_c_api_projections.py*/
    {
        auto projCRS = proj_create_conversion_utm(m_ctxt, 0, 0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS = proj_create_conversion_transverse_mercator(
            m_ctxt, 0, 0, 0, 0, 0, "Degree", 0.0174532925199433, "Metre", 1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS =
            proj_create_conversion_gauss_schreiber_transverse_mercator(
                m_ctxt, 0, 0, 0, 0, 0, "Degree", 0.0174532925199433, "Metre",
                1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS =
            proj_create_conversion_transverse_mercator_south_oriented(
                m_ctxt, 0, 0, 0, 0, 0, "Degree", 0.0174532925199433, "Metre",
                1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS = proj_create_conversion_two_point_equidistant(
            m_ctxt, 0, 0, 0, 0, 0, 0, "Degree", 0.0174532925199433, "Metre",
            1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS = proj_create_conversion_tunisia_mapping_grid(
            m_ctxt, 0, 0, 0, 0, "Degree", 0.0174532925199433, "Metre", 1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS = proj_create_conversion_albers_equal_area(
            m_ctxt, 0, 0, 0, 0, 0, 0, "Degree", 0.0174532925199433, "Metre",
            1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS = proj_create_conversion_lambert_conic_conformal_1sp(
            m_ctxt, 0, 0, 0, 0, 0, "Degree", 0.0174532925199433, "Metre", 1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS = proj_create_conversion_lambert_conic_conformal_2sp(
            m_ctxt, 0, 0, 0, 0, 0, 0, "Degree", 0.0174532925199433, "Metre",
            1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS =
            proj_create_conversion_lambert_conic_conformal_2sp_michigan(
                m_ctxt, 0, 0, 0, 0, 0, 0, 0, "Degree", 0.0174532925199433,
                "Metre", 1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS =
            proj_create_conversion_lambert_conic_conformal_2sp_belgium(
                m_ctxt, 0, 0, 0, 0, 0, 0, "Degree", 0.0174532925199433, "Metre",
                1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS = proj_create_conversion_azimuthal_equidistant(
            m_ctxt, 0, 0, 0, 0, "Degree", 0.0174532925199433, "Metre", 1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS = proj_create_conversion_guam_projection(
            m_ctxt, 0, 0, 0, 0, "Degree", 0.0174532925199433, "Metre", 1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS = proj_create_conversion_bonne(
            m_ctxt, 0, 0, 0, 0, "Degree", 0.0174532925199433, "Metre", 1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS =
            proj_create_conversion_lambert_cylindrical_equal_area_spherical(
                m_ctxt, 0, 0, 0, 0, "Degree", 0.0174532925199433, "Metre", 1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS = proj_create_conversion_lambert_cylindrical_equal_area(
            m_ctxt, 0, 0, 0, 0, "Degree", 0.0174532925199433, "Metre", 1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS = proj_create_conversion_cassini_soldner(
            m_ctxt, 0, 0, 0, 0, "Degree", 0.0174532925199433, "Metre", 1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS = proj_create_conversion_equidistant_conic(
            m_ctxt, 0, 0, 0, 0, 0, 0, "Degree", 0.0174532925199433, "Metre",
            1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS = proj_create_conversion_eckert_i(
            m_ctxt, 0, 0, 0, "Degree", 0.0174532925199433, "Metre", 1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS = proj_create_conversion_eckert_ii(
            m_ctxt, 0, 0, 0, "Degree", 0.0174532925199433, "Metre", 1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS = proj_create_conversion_eckert_iii(
            m_ctxt, 0, 0, 0, "Degree", 0.0174532925199433, "Metre", 1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS = proj_create_conversion_eckert_iv(
            m_ctxt, 0, 0, 0, "Degree", 0.0174532925199433, "Metre", 1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS = proj_create_conversion_eckert_v(
            m_ctxt, 0, 0, 0, "Degree", 0.0174532925199433, "Metre", 1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS = proj_create_conversion_eckert_vi(
            m_ctxt, 0, 0, 0, "Degree", 0.0174532925199433, "Metre", 1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS = proj_create_conversion_equidistant_cylindrical(
            m_ctxt, 0, 0, 0, 0, "Degree", 0.0174532925199433, "Metre", 1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS = proj_create_conversion_equidistant_cylindrical_spherical(
            m_ctxt, 0, 0, 0, 0, "Degree", 0.0174532925199433, "Metre", 1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS = proj_create_conversion_gall(
            m_ctxt, 0, 0, 0, "Degree", 0.0174532925199433, "Metre", 1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS = proj_create_conversion_goode_homolosine(
            m_ctxt, 0, 0, 0, "Degree", 0.0174532925199433, "Metre", 1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS = proj_create_conversion_interrupted_goode_homolosine(
            m_ctxt, 0, 0, 0, "Degree", 0.0174532925199433, "Metre", 1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS = proj_create_conversion_geostationary_satellite_sweep_x(
            m_ctxt, 0, 0, 0, 0, "Degree", 0.0174532925199433, "Metre", 1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS = proj_create_conversion_geostationary_satellite_sweep_y(
            m_ctxt, 0, 0, 0, 0, "Degree", 0.0174532925199433, "Metre", 1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS = proj_create_conversion_gnomonic(
            m_ctxt, 0, 0, 0, 0, "Degree", 0.0174532925199433, "Metre", 1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS = proj_create_conversion_hotine_oblique_mercator_variant_a(
            m_ctxt, 0, 0, 0, 0, 0, 0, 0, "Degree", 0.0174532925199433, "Metre",
            1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS = proj_create_conversion_hotine_oblique_mercator_variant_b(
            m_ctxt, 0, 0, 0, 0, 0, 0, 0, "Degree", 0.0174532925199433, "Metre",
            1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS =
            proj_create_conversion_hotine_oblique_mercator_two_point_natural_origin(
                m_ctxt, 0, 0, 0, 0, 0, 0, 0, 0, "Degree", 0.0174532925199433,
                "Metre", 1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS = proj_create_conversion_laborde_oblique_mercator(
            m_ctxt, 0, 0, 0, 0, 0, 0, "Degree", 0.0174532925199433, "Metre",
            1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS = proj_create_conversion_international_map_world_polyconic(
            m_ctxt, 0, 0, 0, 0, 0, "Degree", 0.0174532925199433, "Metre", 1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS = proj_create_conversion_krovak_north_oriented(
            m_ctxt, 0, 0, 0, 0, 0, 0, 0, "Degree", 0.0174532925199433, "Metre",
            1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS =
            proj_create_conversion_krovak(m_ctxt, 0, 0, 0, 0, 0, 0, 0, "Degree",
                                          0.0174532925199433, "Metre", 1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS = proj_create_conversion_lambert_azimuthal_equal_area(
            m_ctxt, 0, 0, 0, 0, "Degree", 0.0174532925199433, "Metre", 1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS = proj_create_conversion_miller_cylindrical(
            m_ctxt, 0, 0, 0, "Degree", 0.0174532925199433, "Metre", 1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS = proj_create_conversion_mercator_variant_a(
            m_ctxt, 0, 0, 0, 0, 0, "Degree", 0.0174532925199433, "Metre", 1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS = proj_create_conversion_mercator_variant_b(
            m_ctxt, 0, 0, 0, 0, "Degree", 0.0174532925199433, "Metre", 1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS =
            proj_create_conversion_popular_visualisation_pseudo_mercator(
                m_ctxt, 0, 0, 0, 0, "Degree", 0.0174532925199433, "Metre", 1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS = proj_create_conversion_mollweide(
            m_ctxt, 0, 0, 0, "Degree", 0.0174532925199433, "Metre", 1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS = proj_create_conversion_new_zealand_mapping_grid(
            m_ctxt, 0, 0, 0, 0, "Degree", 0.0174532925199433, "Metre", 1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS = proj_create_conversion_oblique_stereographic(
            m_ctxt, 0, 0, 0, 0, 0, "Degree", 0.0174532925199433, "Metre", 1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS = proj_create_conversion_orthographic(
            m_ctxt, 0, 0, 0, 0, "Degree", 0.0174532925199433, "Metre", 1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS = proj_create_conversion_american_polyconic(
            m_ctxt, 0, 0, 0, 0, "Degree", 0.0174532925199433, "Metre", 1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS = proj_create_conversion_polar_stereographic_variant_a(
            m_ctxt, 0, 0, 0, 0, 0, "Degree", 0.0174532925199433, "Metre", 1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS = proj_create_conversion_polar_stereographic_variant_b(
            m_ctxt, 0, 0, 0, 0, "Degree", 0.0174532925199433, "Metre", 1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS = proj_create_conversion_robinson(
            m_ctxt, 0, 0, 0, "Degree", 0.0174532925199433, "Metre", 1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS = proj_create_conversion_sinusoidal(
            m_ctxt, 0, 0, 0, "Degree", 0.0174532925199433, "Metre", 1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS = proj_create_conversion_stereographic(
            m_ctxt, 0, 0, 0, 0, 0, "Degree", 0.0174532925199433, "Metre", 1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS = proj_create_conversion_van_der_grinten(
            m_ctxt, 0, 0, 0, "Degree", 0.0174532925199433, "Metre", 1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS = proj_create_conversion_wagner_i(
            m_ctxt, 0, 0, 0, "Degree", 0.0174532925199433, "Metre", 1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS = proj_create_conversion_wagner_ii(
            m_ctxt, 0, 0, 0, "Degree", 0.0174532925199433, "Metre", 1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS = proj_create_conversion_wagner_iii(
            m_ctxt, 0, 0, 0, 0, "Degree", 0.0174532925199433, "Metre", 1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS = proj_create_conversion_wagner_iv(
            m_ctxt, 0, 0, 0, "Degree", 0.0174532925199433, "Metre", 1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS = proj_create_conversion_wagner_v(
            m_ctxt, 0, 0, 0, "Degree", 0.0174532925199433, "Metre", 1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS = proj_create_conversion_wagner_vi(
            m_ctxt, 0, 0, 0, "Degree", 0.0174532925199433, "Metre", 1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS = proj_create_conversion_wagner_vii(
            m_ctxt, 0, 0, 0, "Degree", 0.0174532925199433, "Metre", 1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS = proj_create_conversion_quadrilateralized_spherical_cube(
            m_ctxt, 0, 0, 0, 0, "Degree", 0.0174532925199433, "Metre", 1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS = proj_create_conversion_spherical_cross_track_height(
            m_ctxt, 0, 0, 0, 0, "Degree", 0.0174532925199433, "Metre", 1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS = proj_create_conversion_equal_earth(
            m_ctxt, 0, 0, 0, "Degree", 0.0174532925199433, "Metre", 1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    /* END: Generated by scripts/create_c_api_projections.py*/
}

// ---------------------------------------------------------------------------

TEST_F(CApi, proj_cs_get_axis_info) {
    {
        auto crs = proj_create_from_database(m_ctxt, "EPSG", "4326",
                                             PJ_CATEGORY_CRS, false, nullptr);
        ASSERT_NE(crs, nullptr);
        ObjectKeeper keeper(crs);

        auto cs = proj_crs_get_coordinate_system(m_ctxt, crs);
        ASSERT_NE(cs, nullptr);
        ObjectKeeper keeperCs(cs);

        EXPECT_EQ(proj_cs_get_type(m_ctxt, cs), PJ_CS_TYPE_ELLIPSOIDAL);

        EXPECT_EQ(proj_cs_get_axis_count(m_ctxt, cs), 2);

        EXPECT_FALSE(proj_cs_get_axis_info(m_ctxt, cs, -1, nullptr, nullptr,
                                           nullptr, nullptr, nullptr, nullptr,
                                           nullptr));

        EXPECT_FALSE(proj_cs_get_axis_info(m_ctxt, cs, 2, nullptr, nullptr,
                                           nullptr, nullptr, nullptr, nullptr,
                                           nullptr));

        EXPECT_TRUE(proj_cs_get_axis_info(m_ctxt, cs, 0, nullptr, nullptr,
                                          nullptr, nullptr, nullptr, nullptr,
                                          nullptr));

        const char *name = nullptr;
        const char *abbrev = nullptr;
        const char *direction = nullptr;
        double unitConvFactor = 0.0;
        const char *unitName = nullptr;
        const char *unitAuthority = nullptr;
        const char *unitCode = nullptr;

        EXPECT_TRUE(proj_cs_get_axis_info(
            m_ctxt, cs, 0, &name, &abbrev, &direction, &unitConvFactor,
            &unitName, &unitAuthority, &unitCode));
        ASSERT_NE(name, nullptr);
        ASSERT_NE(abbrev, nullptr);
        ASSERT_NE(direction, nullptr);
        ASSERT_NE(unitName, nullptr);
        ASSERT_NE(unitAuthority, nullptr);
        ASSERT_NE(unitCode, nullptr);
        EXPECT_EQ(std::string(name), "Geodetic latitude");
        EXPECT_EQ(std::string(abbrev), "Lat");
        EXPECT_EQ(std::string(direction), "north");
        EXPECT_EQ(unitConvFactor, 0.017453292519943295) << unitConvFactor;
        EXPECT_EQ(std::string(unitName), "degree");
        EXPECT_EQ(std::string(unitAuthority), "EPSG");
        EXPECT_EQ(std::string(unitCode), "9122");
    }

    // Non CRS object
    {
        auto obj = proj_create_from_database(m_ctxt, "EPSG", "1170",
                                             PJ_CATEGORY_COORDINATE_OPERATION,
                                             false, nullptr);
        ASSERT_NE(obj, nullptr);
        ObjectKeeper keeper(obj);
        EXPECT_EQ(proj_crs_get_coordinate_system(m_ctxt, obj), nullptr);

        EXPECT_EQ(proj_cs_get_type(m_ctxt, obj), PJ_CS_TYPE_UNKNOWN);

        EXPECT_EQ(proj_cs_get_axis_count(m_ctxt, obj), -1);

        EXPECT_FALSE(proj_cs_get_axis_info(m_ctxt, obj, 0, nullptr, nullptr,
                                           nullptr, nullptr, nullptr, nullptr,
                                           nullptr));
    }
}

// ---------------------------------------------------------------------------

TEST_F(CApi, proj_context_get_database_metadata) {
    EXPECT_TRUE(proj_context_get_database_metadata(m_ctxt, "IGNF.VERSION") !=
                nullptr);
}

// ---------------------------------------------------------------------------

TEST_F(CApi, proj_clone) {
    auto obj = proj_create(m_ctxt, "+proj=longlat");
    ObjectKeeper keeper(obj);
    ASSERT_NE(obj, nullptr);

    auto clone = proj_clone(m_ctxt, obj);
    ObjectKeeper keeperClone(clone);
    ASSERT_NE(clone, nullptr);

    EXPECT_TRUE(proj_is_equivalent_to(obj, clone, PJ_COMP_STRICT));
}

// ---------------------------------------------------------------------------

TEST_F(CApi, proj_crs_alter_geodetic_crs) {
    auto projCRS = proj_create_from_wkt(
        m_ctxt,
        createProjectedCRS()->exportToWKT(WKTFormatter::create().get()).c_str(),
        nullptr, nullptr, nullptr);
    ObjectKeeper keeper(projCRS);
    ASSERT_NE(projCRS, nullptr);

    auto newGeodCRS = proj_create(m_ctxt, "+proj=longlat +type=crs");
    ObjectKeeper keeper_newGeodCRS(newGeodCRS);
    ASSERT_NE(newGeodCRS, nullptr);

    auto geodCRS = proj_crs_get_geodetic_crs(m_ctxt, projCRS);
    ObjectKeeper keeper_geodCRS(geodCRS);
    ASSERT_NE(geodCRS, nullptr);

    auto geodCRSAltered =
        proj_crs_alter_geodetic_crs(m_ctxt, geodCRS, newGeodCRS);
    ObjectKeeper keeper_geodCRSAltered(geodCRSAltered);
    ASSERT_NE(geodCRSAltered, nullptr);
    EXPECT_TRUE(
        proj_is_equivalent_to(geodCRSAltered, newGeodCRS, PJ_COMP_STRICT));

    {
        auto projCRSAltered =
            proj_crs_alter_geodetic_crs(m_ctxt, projCRS, newGeodCRS);
        ObjectKeeper keeper_projCRSAltered(projCRSAltered);
        ASSERT_NE(projCRSAltered, nullptr);

        EXPECT_EQ(proj_get_type(projCRSAltered), PJ_TYPE_PROJECTED_CRS);

        auto projCRSAltered_geodCRS =
            proj_crs_get_geodetic_crs(m_ctxt, projCRSAltered);
        ObjectKeeper keeper_projCRSAltered_geodCRS(projCRSAltered_geodCRS);
        ASSERT_NE(projCRSAltered_geodCRS, nullptr);

        EXPECT_TRUE(proj_is_equivalent_to(projCRSAltered_geodCRS, newGeodCRS,
                                          PJ_COMP_STRICT));
    }

    // Check that proj_crs_alter_geodetic_crs preserves deprecation flag
    {
        auto projCRSDeprecated =
            proj_alter_name(m_ctxt, projCRS, "new name (deprecated)");
        ObjectKeeper keeper_projCRSDeprecated(projCRSDeprecated);
        ASSERT_NE(projCRSDeprecated, nullptr);

        auto projCRSAltered =
            proj_crs_alter_geodetic_crs(m_ctxt, projCRSDeprecated, newGeodCRS);
        ObjectKeeper keeper_projCRSAltered(projCRSAltered);
        ASSERT_NE(projCRSAltered, nullptr);

        EXPECT_EQ(proj_get_name(projCRSAltered), std::string("new name"));
        EXPECT_TRUE(proj_is_deprecated(projCRSAltered));
    }
}

// ---------------------------------------------------------------------------

TEST_F(CApi, proj_crs_alter_cs_angular_unit) {
    auto crs = proj_create_from_wkt(
        m_ctxt,
        GeographicCRS::EPSG_4326->exportToWKT(WKTFormatter::create().get())
            .c_str(),
        nullptr, nullptr, nullptr);
    ObjectKeeper keeper(crs);
    ASSERT_NE(crs, nullptr);

    {
        auto alteredCRS = proj_crs_alter_cs_angular_unit(m_ctxt, crs, "my unit",
                                                         2, nullptr, nullptr);
        ObjectKeeper keeper_alteredCRS(alteredCRS);
        ASSERT_NE(alteredCRS, nullptr);

        auto cs = proj_crs_get_coordinate_system(m_ctxt, alteredCRS);
        ASSERT_NE(cs, nullptr);
        ObjectKeeper keeperCs(cs);
        double unitConvFactor = 0.0;
        const char *unitName = nullptr;

        EXPECT_TRUE(proj_cs_get_axis_info(m_ctxt, cs, 0, nullptr, nullptr,
                                          nullptr, &unitConvFactor, &unitName,
                                          nullptr, nullptr));
        ASSERT_NE(unitName, nullptr);
        EXPECT_EQ(unitConvFactor, 2) << unitConvFactor;
        EXPECT_EQ(std::string(unitName), "my unit");
    }

    {
        auto alteredCRS = proj_crs_alter_cs_angular_unit(
            m_ctxt, crs, "my unit", 2, "my auth", "my code");
        ObjectKeeper keeper_alteredCRS(alteredCRS);
        ASSERT_NE(alteredCRS, nullptr);

        auto cs = proj_crs_get_coordinate_system(m_ctxt, alteredCRS);
        ASSERT_NE(cs, nullptr);
        ObjectKeeper keeperCs(cs);
        double unitConvFactor = 0.0;
        const char *unitName = nullptr;
        const char *unitAuthName = nullptr;
        const char *unitCode = nullptr;

        EXPECT_TRUE(proj_cs_get_axis_info(m_ctxt, cs, 0, nullptr, nullptr,
                                          nullptr, &unitConvFactor, &unitName,
                                          &unitAuthName, &unitCode));
        ASSERT_NE(unitName, nullptr);
        EXPECT_EQ(unitConvFactor, 2) << unitConvFactor;
        EXPECT_EQ(std::string(unitName), "my unit");
        ASSERT_NE(unitAuthName, nullptr);
        EXPECT_EQ(std::string(unitAuthName), "my auth");
        ASSERT_NE(unitCode, nullptr);
        EXPECT_EQ(std::string(unitCode), "my code");
    }
}

// ---------------------------------------------------------------------------

TEST_F(CApi, proj_crs_alter_cs_linear_unit) {
    auto crs = proj_create_from_wkt(
        m_ctxt,
        createProjectedCRS()->exportToWKT(WKTFormatter::create().get()).c_str(),
        nullptr, nullptr, nullptr);
    ObjectKeeper keeper(crs);
    ASSERT_NE(crs, nullptr);

    {
        auto alteredCRS = proj_crs_alter_cs_linear_unit(m_ctxt, crs, "my unit",
                                                        2, nullptr, nullptr);
        ObjectKeeper keeper_alteredCRS(alteredCRS);
        ASSERT_NE(alteredCRS, nullptr);

        auto cs = proj_crs_get_coordinate_system(m_ctxt, alteredCRS);
        ASSERT_NE(cs, nullptr);
        ObjectKeeper keeperCs(cs);
        double unitConvFactor = 0.0;
        const char *unitName = nullptr;

        EXPECT_TRUE(proj_cs_get_axis_info(m_ctxt, cs, 0, nullptr, nullptr,
                                          nullptr, &unitConvFactor, &unitName,
                                          nullptr, nullptr));
        ASSERT_NE(unitName, nullptr);
        EXPECT_EQ(unitConvFactor, 2) << unitConvFactor;
        EXPECT_EQ(std::string(unitName), "my unit");
    }

    {
        auto alteredCRS = proj_crs_alter_cs_linear_unit(
            m_ctxt, crs, "my unit", 2, "my auth", "my code");
        ObjectKeeper keeper_alteredCRS(alteredCRS);
        ASSERT_NE(alteredCRS, nullptr);

        auto cs = proj_crs_get_coordinate_system(m_ctxt, alteredCRS);
        ASSERT_NE(cs, nullptr);
        ObjectKeeper keeperCs(cs);
        double unitConvFactor = 0.0;
        const char *unitName = nullptr;
        const char *unitAuthName = nullptr;
        const char *unitCode = nullptr;

        EXPECT_TRUE(proj_cs_get_axis_info(m_ctxt, cs, 0, nullptr, nullptr,
                                          nullptr, &unitConvFactor, &unitName,
                                          &unitAuthName, &unitCode));
        ASSERT_NE(unitName, nullptr);
        EXPECT_EQ(unitConvFactor, 2) << unitConvFactor;
        EXPECT_EQ(std::string(unitName), "my unit");
        ASSERT_NE(unitAuthName, nullptr);
        EXPECT_EQ(std::string(unitAuthName), "my auth");
        ASSERT_NE(unitCode, nullptr);
        EXPECT_EQ(std::string(unitCode), "my code");
    }
}

// ---------------------------------------------------------------------------

TEST_F(CApi, proj_crs_alter_parameters_linear_unit) {
    auto crs = proj_create_from_wkt(
        m_ctxt,
        createProjectedCRS()->exportToWKT(WKTFormatter::create().get()).c_str(),
        nullptr, nullptr, nullptr);
    ObjectKeeper keeper(crs);
    ASSERT_NE(crs, nullptr);

    {
        auto alteredCRS = proj_crs_alter_parameters_linear_unit(
            m_ctxt, crs, "my unit", 2, nullptr, nullptr, false);
        ObjectKeeper keeper_alteredCRS(alteredCRS);
        ASSERT_NE(alteredCRS, nullptr);

        auto wkt = proj_as_wkt(m_ctxt, alteredCRS, PJ_WKT2_2018, nullptr);
        ASSERT_NE(wkt, nullptr);
        EXPECT_TRUE(std::string(wkt).find("500000") != std::string::npos)
            << wkt;
        EXPECT_TRUE(std::string(wkt).find("\"my unit\",2") != std::string::npos)
            << wkt;
    }

    {
        auto alteredCRS = proj_crs_alter_parameters_linear_unit(
            m_ctxt, crs, "my unit", 2, nullptr, nullptr, true);
        ObjectKeeper keeper_alteredCRS(alteredCRS);
        ASSERT_NE(alteredCRS, nullptr);

        auto wkt = proj_as_wkt(m_ctxt, alteredCRS, PJ_WKT2_2018, nullptr);
        ASSERT_NE(wkt, nullptr);
        EXPECT_TRUE(std::string(wkt).find("250000") != std::string::npos)
            << wkt;
        EXPECT_TRUE(std::string(wkt).find("\"my unit\",2") != std::string::npos)
            << wkt;
    }
}

// ---------------------------------------------------------------------------

TEST_F(CApi, proj_create_engineering_crs) {

    auto crs = proj_create_engineering_crs(m_ctxt, "name");
    ObjectKeeper keeper(crs);
    ASSERT_NE(crs, nullptr);
    auto wkt = proj_as_wkt(m_ctxt, crs, PJ_WKT1_GDAL, nullptr);
    ASSERT_NE(wkt, nullptr);
    EXPECT_EQ(std::string(wkt), "LOCAL_CS[\"name\"]") << wkt;
}

// ---------------------------------------------------------------------------

TEST_F(CApi, proj_alter_name) {

    auto cs = proj_create_ellipsoidal_2D_cs(
        m_ctxt, PJ_ELLPS2D_LONGITUDE_LATITUDE, nullptr, 0);
    ObjectKeeper keeper_cs(cs);
    ASSERT_NE(cs, nullptr);

    auto obj = proj_create_geographic_crs(
        m_ctxt, "WGS 84", "World Geodetic System 1984", "WGS 84", 6378137,
        298.257223563, "Greenwich", 0.0, "Degree", 0.0174532925199433, cs);
    ObjectKeeper keeper(obj);
    ASSERT_NE(obj, nullptr);

    {
        auto alteredObj = proj_alter_name(m_ctxt, obj, "new name");
        ObjectKeeper keeper_alteredObj(alteredObj);
        ASSERT_NE(alteredObj, nullptr);

        EXPECT_EQ(std::string(proj_get_name(alteredObj)), "new name");
        EXPECT_FALSE(proj_is_deprecated(alteredObj));
    }

    {
        auto alteredObj = proj_alter_name(m_ctxt, obj, "new name (deprecated)");
        ObjectKeeper keeper_alteredObj(alteredObj);
        ASSERT_NE(alteredObj, nullptr);

        EXPECT_EQ(std::string(proj_get_name(alteredObj)), "new name");
        EXPECT_TRUE(proj_is_deprecated(alteredObj));
    }
}

// ---------------------------------------------------------------------------

TEST_F(CApi, proj_alter_id) {

    auto cs = proj_create_ellipsoidal_2D_cs(
        m_ctxt, PJ_ELLPS2D_LONGITUDE_LATITUDE, nullptr, 0);
    ObjectKeeper keeper_cs(cs);
    ASSERT_NE(cs, nullptr);

    auto obj = proj_create_geographic_crs(
        m_ctxt, "WGS 84", "World Geodetic System 1984", "WGS 84", 6378137,
        298.257223563, "Greenwich", 0.0, "Degree", 0.0174532925199433, cs);
    ObjectKeeper keeper(obj);
    ASSERT_NE(obj, nullptr);

    auto alteredObj = proj_alter_id(m_ctxt, obj, "auth", "code");
    ObjectKeeper keeper_alteredObj(alteredObj);
    ASSERT_NE(alteredObj, nullptr);

    EXPECT_EQ(std::string(proj_get_id_auth_name(alteredObj, 0)), "auth");
    EXPECT_EQ(std::string(proj_get_id_code(alteredObj, 0)), "code");
}

// ---------------------------------------------------------------------------

TEST_F(CApi, proj_create_projected_crs) {

    PJ_PARAM_DESCRIPTION param;
    param.name = "param name";
    param.auth_name = nullptr;
    param.code = nullptr;
    param.value = 0.99;
    param.unit_name = nullptr;
    param.unit_conv_factor = 1.0;
    param.unit_type = PJ_UT_SCALE;

    auto conv = proj_create_conversion(m_ctxt, "conv", "conv auth", "conv code",
                                       "method", "method auth", "method code",
                                       1, &param);
    ObjectKeeper keeper_conv(conv);
    ASSERT_NE(conv, nullptr);

    auto geog_cs = proj_create_ellipsoidal_2D_cs(
        m_ctxt, PJ_ELLPS2D_LONGITUDE_LATITUDE, nullptr, 0);
    ObjectKeeper keeper_geog_cs(geog_cs);
    ASSERT_NE(geog_cs, nullptr);

    auto geogCRS = proj_create_geographic_crs(
        m_ctxt, "WGS 84", "World Geodetic System 1984", "WGS 84", 6378137,
        298.257223563, "Greenwich", 0.0, "Degree", 0.0174532925199433, geog_cs);
    ObjectKeeper keeper_geogCRS(geogCRS);
    ASSERT_NE(geogCRS, nullptr);

    auto cs = proj_create_cartesian_2D_cs(m_ctxt, PJ_CART2D_EASTING_NORTHING,
                                          nullptr, 0);
    ObjectKeeper keeper_cs(cs);
    ASSERT_NE(cs, nullptr);

    auto projCRS =
        proj_create_projected_crs(m_ctxt, "my CRS", geogCRS, conv, cs);
    ObjectKeeper keeper_projCRS(projCRS);
    ASSERT_NE(projCRS, nullptr);
}

// ---------------------------------------------------------------------------

TEST_F(CApi, proj_create_transformation) {

    PJ_PARAM_DESCRIPTION param;
    param.name = "param name";
    param.auth_name = nullptr;
    param.code = nullptr;
    param.value = 0.99;
    param.unit_name = nullptr;
    param.unit_conv_factor = 1.0;
    param.unit_type = PJ_UT_SCALE;

    auto geog_cs = proj_create_ellipsoidal_2D_cs(
        m_ctxt, PJ_ELLPS2D_LONGITUDE_LATITUDE, nullptr, 0);
    ObjectKeeper keeper_geog_cs(geog_cs);
    ASSERT_NE(geog_cs, nullptr);

    auto source_crs = proj_create_geographic_crs(
        m_ctxt, "Source CRS", "World Geodetic System 1984", "WGS 84", 6378137,
        298.257223563, "Greenwich", 0.0, "Degree", 0.0174532925199433, geog_cs);
    ObjectKeeper keeper_source_crs(source_crs);
    ASSERT_NE(source_crs, nullptr);

    auto target_crs = proj_create_geographic_crs(
        m_ctxt, "WGS 84", "World Geodetic System 1984", "WGS 84", 6378137,
        298.257223563, "Greenwich", 0.0, "Degree", 0.0174532925199433, geog_cs);
    ObjectKeeper keeper_target_crs(target_crs);
    ASSERT_NE(target_crs, nullptr);

    auto interp_crs = proj_create_geographic_crs(
        m_ctxt, "Interpolation CRS", "World Geodetic System 1984", "WGS 84",
        6378137, 298.257223563, "Greenwich", 0.0, "Degree", 0.0174532925199433,
        geog_cs);
    ObjectKeeper keeper_interp_crs(interp_crs);
    ASSERT_NE(interp_crs, nullptr);

    {
        auto transf = proj_create_transformation(
            m_ctxt, "transf", "transf auth", "transf code", source_crs,
            target_crs, interp_crs, "method", "method auth", "method code", 1,
            &param, 0);
        ObjectKeeper keeper_transf(transf);
        ASSERT_NE(transf, nullptr);

        EXPECT_EQ(proj_coordoperation_get_param_count(m_ctxt, transf), 1);

        auto got_source_crs = proj_get_source_crs(m_ctxt, transf);
        ObjectKeeper keeper_got_source_crs(got_source_crs);
        ASSERT_NE(got_source_crs, nullptr);
        EXPECT_TRUE(
            proj_is_equivalent_to(source_crs, got_source_crs, PJ_COMP_STRICT));

        auto got_target_crs = proj_get_target_crs(m_ctxt, transf);
        ObjectKeeper keeper_got_target_crs(got_target_crs);
        ASSERT_NE(got_target_crs, nullptr);
        EXPECT_TRUE(
            proj_is_equivalent_to(target_crs, got_target_crs, PJ_COMP_STRICT));
    }

    {
        auto transf = proj_create_transformation(
            m_ctxt, "transf", "transf auth", "transf code", source_crs,
            target_crs, nullptr, "method", "method auth", "method code", 1,
            &param, -1);
        ObjectKeeper keeper_transf(transf);
        ASSERT_NE(transf, nullptr);
    }
}

// ---------------------------------------------------------------------------

TEST_F(CApi, proj_create_compound_crs) {

    auto horiz_cs = proj_create_ellipsoidal_2D_cs(
        m_ctxt, PJ_ELLPS2D_LONGITUDE_LATITUDE, nullptr, 0);
    ObjectKeeper keeper_horiz_cs(horiz_cs);
    ASSERT_NE(horiz_cs, nullptr);

    auto horiz_crs = proj_create_geographic_crs(
        m_ctxt, "WGS 84", "World Geodetic System 1984", "WGS 84", 6378137,
        298.257223563, "Greenwich", 0.0, "Degree", 0.0174532925199433,
        horiz_cs);
    ObjectKeeper keeper_horiz_crs(horiz_crs);
    ASSERT_NE(horiz_crs, nullptr);

    auto vert_crs = proj_create_vertical_crs(m_ctxt, "myVertCRS", "myVertDatum",
                                             nullptr, 0.0);
    ObjectKeeper keeper_vert_crs(vert_crs);
    ASSERT_NE(vert_crs, nullptr);

    EXPECT_EQ(proj_get_name(vert_crs), std::string("myVertCRS"));

    auto compound_crs =
        proj_create_compound_crs(m_ctxt, "myCompoundCRS", horiz_crs, vert_crs);
    ObjectKeeper keeper_compound_crss(compound_crs);
    ASSERT_NE(compound_crs, nullptr);

    EXPECT_EQ(proj_get_name(compound_crs), std::string("myCompoundCRS"));

    auto subcrs_horiz = proj_crs_get_sub_crs(m_ctxt, compound_crs, 0);
    ASSERT_NE(subcrs_horiz, nullptr);
    ObjectKeeper keeper_subcrs_horiz(subcrs_horiz);
    EXPECT_TRUE(proj_is_equivalent_to(subcrs_horiz, horiz_crs, PJ_COMP_STRICT));

    auto subcrs_vert = proj_crs_get_sub_crs(m_ctxt, compound_crs, 1);
    ASSERT_NE(subcrs_vert, nullptr);
    ObjectKeeper keeper_subcrs_vert(subcrs_vert);
    EXPECT_TRUE(proj_is_equivalent_to(subcrs_vert, vert_crs, PJ_COMP_STRICT));
}

// ---------------------------------------------------------------------------

TEST_F(CApi, proj_convert_conversion_to_other_method) {
    {
        auto geog_cs = proj_create_ellipsoidal_2D_cs(
            m_ctxt, PJ_ELLPS2D_LONGITUDE_LATITUDE, nullptr, 0);
        ObjectKeeper keeper_geog_cs(geog_cs);
        ASSERT_NE(geog_cs, nullptr);

        auto geogCRS = proj_create_geographic_crs(
            m_ctxt, "WGS 84", "World Geodetic System 1984", "WGS 84", 6378137,
            298.257223563, "Greenwich", 0.0, "Degree", 0.0174532925199433,
            geog_cs);
        ObjectKeeper keeper_geogCRS(geogCRS);
        ASSERT_NE(geogCRS, nullptr);

        auto cs = proj_create_cartesian_2D_cs(
            m_ctxt, PJ_CART2D_EASTING_NORTHING, nullptr, 0);
        ObjectKeeper keeper_cs(cs);
        ASSERT_NE(cs, nullptr);

        auto conv = proj_create_conversion_mercator_variant_a(
            m_ctxt, 0, 1, 0.99, 2, 3, "Degree", 0.0174532925199433, "Metre",
            1.0);
        ObjectKeeper keeper_conv(conv);
        ASSERT_NE(conv, nullptr);

        auto projCRS =
            proj_create_projected_crs(m_ctxt, "my CRS", geogCRS, conv, cs);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);

        // Wrong object type
        EXPECT_EQ(
            proj_convert_conversion_to_other_method(
                m_ctxt, projCRS, EPSG_CODE_METHOD_MERCATOR_VARIANT_B, nullptr),
            nullptr);

        auto conv_in_proj = proj_crs_get_coordoperation(m_ctxt, projCRS);
        ObjectKeeper keeper_conv_in_proj(conv_in_proj);
        ASSERT_NE(conv_in_proj, nullptr);

        // 3rd and 4th argument both 0/null
        EXPECT_EQ(proj_convert_conversion_to_other_method(m_ctxt, conv_in_proj,
                                                          0, nullptr),
                  nullptr);

        auto new_conv = proj_convert_conversion_to_other_method(
            m_ctxt, conv_in_proj, EPSG_CODE_METHOD_MERCATOR_VARIANT_B, nullptr);
        ObjectKeeper keeper_new_conv(new_conv);
        ASSERT_NE(new_conv, nullptr);

        EXPECT_FALSE(
            proj_is_equivalent_to(new_conv, conv_in_proj, PJ_COMP_STRICT));
        EXPECT_TRUE(
            proj_is_equivalent_to(new_conv, conv_in_proj, PJ_COMP_EQUIVALENT));

        auto new_conv_from_name = proj_convert_conversion_to_other_method(
            m_ctxt, conv_in_proj, 0, EPSG_NAME_METHOD_MERCATOR_VARIANT_B);
        ObjectKeeper keeper_new_conv_from_name(new_conv_from_name);
        ASSERT_NE(new_conv_from_name, nullptr);

        EXPECT_TRUE(proj_is_equivalent_to(new_conv, new_conv_from_name,
                                          PJ_COMP_STRICT));

        auto new_conv_back = proj_convert_conversion_to_other_method(
            m_ctxt, conv_in_proj, 0, EPSG_NAME_METHOD_MERCATOR_VARIANT_A);
        ObjectKeeper keeper_new_conv_back(new_conv_back);
        ASSERT_NE(new_conv_back, nullptr);

        EXPECT_TRUE(
            proj_is_equivalent_to(conv_in_proj, new_conv_back, PJ_COMP_STRICT));
    }
}

// ---------------------------------------------------------------------------

TEST_F(CApi, proj_get_non_deprecated) {
    auto crs = proj_create_from_database(m_ctxt, "EPSG", "4226",
                                         PJ_CATEGORY_CRS, false, nullptr);
    ObjectKeeper keeper(crs);
    ASSERT_NE(crs, nullptr);

    auto list = proj_get_non_deprecated(m_ctxt, crs);
    ASSERT_NE(list, nullptr);
    ObjListKeeper keeper_list(list);
    EXPECT_EQ(proj_list_get_count(list), 2);
}

// ---------------------------------------------------------------------------

TEST_F(CApi, proj_query_geodetic_crs_from_datum) {
    {
        auto list = proj_query_geodetic_crs_from_datum(m_ctxt, nullptr, "EPSG",
                                                       "6326", nullptr);
        ASSERT_NE(list, nullptr);
        ObjListKeeper keeper_list(list);
        EXPECT_GE(proj_list_get_count(list), 3);
    }
    {
        auto list = proj_query_geodetic_crs_from_datum(m_ctxt, "EPSG", "EPSG",
                                                       "6326", "geographic 2D");
        ASSERT_NE(list, nullptr);
        ObjListKeeper keeper_list(list);
        EXPECT_EQ(proj_list_get_count(list), 1);
    }
}

// ---------------------------------------------------------------------------

TEST_F(CApi, proj_uom_get_info_from_database) {
    {
        EXPECT_FALSE(proj_uom_get_info_from_database(
            m_ctxt, "auth", "code", nullptr, nullptr, nullptr));
    }
    {
        EXPECT_TRUE(proj_uom_get_info_from_database(m_ctxt, "EPSG", "9001",
                                                    nullptr, nullptr, nullptr));
    }
    {
        const char *name = nullptr;
        double conv_factor = 0.0;
        const char *category = nullptr;
        EXPECT_TRUE(proj_uom_get_info_from_database(
            m_ctxt, "EPSG", "9001", &name, &conv_factor, &category));
        ASSERT_NE(name, nullptr);
        ASSERT_NE(category, nullptr);
        EXPECT_EQ(std::string(name), "metre");
        EXPECT_EQ(conv_factor, 1.0);
        EXPECT_EQ(std::string(category), "linear");
    }
    {
        const char *name = nullptr;
        double conv_factor = 0.0;
        const char *category = nullptr;
        EXPECT_TRUE(proj_uom_get_info_from_database(
            m_ctxt, "EPSG", "9102", &name, &conv_factor, &category));
        ASSERT_NE(name, nullptr);
        ASSERT_NE(category, nullptr);
        EXPECT_EQ(std::string(name), "degree");
        EXPECT_NEAR(conv_factor, UnitOfMeasure::DEGREE.conversionToSI(), 1e-10);
        EXPECT_EQ(std::string(category), "angular");
    }
}

// ---------------------------------------------------------------------------

TEST_F(CApi, proj_grid_get_info_from_database) {
    {
        EXPECT_FALSE(proj_grid_get_info_from_database(m_ctxt, "xxx", nullptr,
                                                      nullptr, nullptr, nullptr,
                                                      nullptr, nullptr));
    }
    {
        EXPECT_TRUE(proj_grid_get_info_from_database(
            m_ctxt, "GDA94_GDA2020_conformal.gsb", nullptr, nullptr, nullptr,
            nullptr, nullptr, nullptr));
    }
    {
        const char *name = nullptr;
        const char *package_name = nullptr;
        const char *url = nullptr;
        int direct_download = 0;
        int open_license = 0;
        int available = 0;
        EXPECT_TRUE(proj_grid_get_info_from_database(
            m_ctxt, "GDA94_GDA2020_conformal.gsb", &name, &package_name, &url,
            &direct_download, &open_license, &available));
        ASSERT_NE(name, nullptr);
        ASSERT_NE(package_name, nullptr);
        ASSERT_NE(url, nullptr);
        EXPECT_EQ(direct_download, 1);
        EXPECT_EQ(open_license, 1);
    }
}

// ---------------------------------------------------------------------------

TEST_F(CApi, proj_create_cartesian_2D_cs) {
    {
        auto cs = proj_create_cartesian_2D_cs(
            m_ctxt, PJ_CART2D_EASTING_NORTHING, nullptr, 0);
        ObjectKeeper keeper_cs(cs);
        ASSERT_NE(cs, nullptr);
    }
    {
        auto cs = proj_create_cartesian_2D_cs(
            m_ctxt, PJ_CART2D_NORTHING_EASTING, nullptr, 0);
        ObjectKeeper keeper_cs(cs);
        ASSERT_NE(cs, nullptr);
    }
    {
        auto cs = proj_create_cartesian_2D_cs(
            m_ctxt, PJ_CART2D_NORTH_POLE_EASTING_SOUTH_NORTHING_SOUTH, nullptr,
            0);
        ObjectKeeper keeper_cs(cs);
        ASSERT_NE(cs, nullptr);
    }
    {
        auto cs = proj_create_cartesian_2D_cs(
            m_ctxt, PJ_CART2D_SOUTH_POLE_EASTING_NORTH_NORTHING_NORTH, nullptr,
            0);
        ObjectKeeper keeper_cs(cs);
        ASSERT_NE(cs, nullptr);
    }
    {
        auto cs = proj_create_cartesian_2D_cs(
            m_ctxt, PJ_CART2D_WESTING_SOUTHING, nullptr, 0);
        ObjectKeeper keeper_cs(cs);
        ASSERT_NE(cs, nullptr);
    }
}

// ---------------------------------------------------------------------------

TEST_F(CApi, proj_get_crs_info_list_from_database) {
    { proj_crs_info_list_destroy(nullptr); }

    { proj_get_crs_list_parameters_destroy(nullptr); }

    // All null parameters
    {
        auto list = proj_get_crs_info_list_from_database(nullptr, nullptr,
                                                         nullptr, nullptr);
        ASSERT_NE(list, nullptr);
        ASSERT_NE(list[0], nullptr);
        EXPECT_NE(list[0]->auth_name, nullptr);
        EXPECT_NE(list[0]->code, nullptr);
        EXPECT_NE(list[0]->name, nullptr);
        proj_crs_info_list_destroy(list);
    }

    // Default parameters
    {
        int result_count = 0;
        auto params = proj_get_crs_list_parameters_create();
        auto list = proj_get_crs_info_list_from_database(m_ctxt, "EPSG", params,
                                                         &result_count);
        proj_get_crs_list_parameters_destroy(params);
        ASSERT_NE(list, nullptr);
        EXPECT_GT(result_count, 1);
        EXPECT_EQ(list[result_count], nullptr);
        bool found4326 = false;
        bool found4978 = false;
        bool found4979 = false;
        bool found32631 = false;
        bool found3855 = false;
        bool found3901 = false;
        for (int i = 0; i < result_count; i++) {
            auto code = std::string(list[i]->code);
            if (code == "4326") {
                found4326 = true;
                EXPECT_EQ(std::string(list[i]->auth_name), "EPSG");
                EXPECT_EQ(std::string(list[i]->name), "WGS 84");
                EXPECT_EQ(list[i]->type, PJ_TYPE_GEOGRAPHIC_2D_CRS);
                EXPECT_EQ(list[i]->deprecated, 0);
                EXPECT_EQ(list[i]->bbox_valid, 1);
                EXPECT_EQ(list[i]->west_lon_degree, -180.0);
                EXPECT_EQ(list[i]->south_lat_degree, -90.0);
                EXPECT_EQ(list[i]->east_lon_degree, 180.0);
                EXPECT_EQ(list[i]->north_lat_degree, 90.0);
                EXPECT_EQ(std::string(list[i]->area_name), "World");
                EXPECT_EQ(list[i]->projection_method_name, nullptr);
            } else if (code == "4978") {
                found4978 = true;
                EXPECT_EQ(list[i]->type, PJ_TYPE_GEOCENTRIC_CRS);
            } else if (code == "4979") {
                found4979 = true;
                EXPECT_EQ(list[i]->type, PJ_TYPE_GEOGRAPHIC_3D_CRS);
            } else if (code == "32631") {
                found32631 = true;
                EXPECT_EQ(list[i]->type, PJ_TYPE_PROJECTED_CRS);
                EXPECT_EQ(std::string(list[i]->projection_method_name),
                          "Transverse Mercator");
            } else if (code == "3855") {
                found3855 = true;
                EXPECT_EQ(list[i]->type, PJ_TYPE_VERTICAL_CRS);
            } else if (code == "3901") {
                found3901 = true;
                EXPECT_EQ(list[i]->type, PJ_TYPE_COMPOUND_CRS);
            }
            EXPECT_EQ(list[i]->deprecated, 0);
        }
        EXPECT_TRUE(found4326);
        EXPECT_TRUE(found4978);
        EXPECT_TRUE(found4979);
        EXPECT_TRUE(found32631);
        EXPECT_TRUE(found3855);
        EXPECT_TRUE(found3901);
        proj_crs_info_list_destroy(list);
    }

    // Filter on only geodetic crs
    {
        int result_count = 0;
        auto params = proj_get_crs_list_parameters_create();
        params->typesCount = 1;
        auto type = PJ_TYPE_GEODETIC_CRS;
        params->types = &type;
        auto list = proj_get_crs_info_list_from_database(m_ctxt, "EPSG", params,
                                                         &result_count);
        bool foundGeog2D = false;
        bool foundGeog3D = false;
        bool foundGeocentric = false;
        for (int i = 0; i < result_count; i++) {
            foundGeog2D |= list[i]->type == PJ_TYPE_GEOGRAPHIC_2D_CRS;
            foundGeog3D |= list[i]->type == PJ_TYPE_GEOGRAPHIC_3D_CRS;
            foundGeocentric |= list[i]->type == PJ_TYPE_GEOCENTRIC_CRS;
            EXPECT_TRUE(list[i]->type == PJ_TYPE_GEOGRAPHIC_2D_CRS ||
                        list[i]->type == PJ_TYPE_GEOGRAPHIC_3D_CRS ||
                        list[i]->type == PJ_TYPE_GEOCENTRIC_CRS);
        }
        EXPECT_TRUE(foundGeog2D);
        EXPECT_TRUE(foundGeog3D);
        EXPECT_TRUE(foundGeocentric);
        proj_get_crs_list_parameters_destroy(params);
        proj_crs_info_list_destroy(list);
    }

    // Filter on only geographic crs
    {
        int result_count = 0;
        auto params = proj_get_crs_list_parameters_create();
        params->typesCount = 1;
        auto type = PJ_TYPE_GEOGRAPHIC_CRS;
        params->types = &type;
        auto list = proj_get_crs_info_list_from_database(m_ctxt, "EPSG", params,
                                                         &result_count);
        bool foundGeog2D = false;
        bool foundGeog3D = false;
        for (int i = 0; i < result_count; i++) {
            foundGeog2D |= list[i]->type == PJ_TYPE_GEOGRAPHIC_2D_CRS;
            foundGeog3D |= list[i]->type == PJ_TYPE_GEOGRAPHIC_3D_CRS;
            EXPECT_TRUE(list[i]->type == PJ_TYPE_GEOGRAPHIC_2D_CRS ||
                        list[i]->type == PJ_TYPE_GEOGRAPHIC_3D_CRS);
        }
        EXPECT_TRUE(foundGeog2D);
        EXPECT_TRUE(foundGeog3D);
        proj_get_crs_list_parameters_destroy(params);
        proj_crs_info_list_destroy(list);
    }

    // Filter on only geographic 2D crs and projected CRS
    {
        int result_count = 0;
        auto params = proj_get_crs_list_parameters_create();
        params->typesCount = 2;
        const PJ_TYPE types[] = {PJ_TYPE_GEOGRAPHIC_2D_CRS,
                                 PJ_TYPE_PROJECTED_CRS};
        params->types = types;
        auto list = proj_get_crs_info_list_from_database(m_ctxt, "EPSG", params,
                                                         &result_count);
        bool foundGeog2D = false;
        bool foundProjected = false;
        for (int i = 0; i < result_count; i++) {
            foundGeog2D |= list[i]->type == PJ_TYPE_GEOGRAPHIC_2D_CRS;
            foundProjected |= list[i]->type == PJ_TYPE_PROJECTED_CRS;
            EXPECT_TRUE(list[i]->type == PJ_TYPE_GEOGRAPHIC_2D_CRS ||
                        list[i]->type == PJ_TYPE_PROJECTED_CRS);
        }
        EXPECT_TRUE(foundGeog2D);
        EXPECT_TRUE(foundProjected);
        proj_get_crs_list_parameters_destroy(params);
        proj_crs_info_list_destroy(list);
    }

    // Filter on bbox (inclusion)
    {
        int result_count = 0;
        auto params = proj_get_crs_list_parameters_create();
        params->bbox_valid = 1;
        params->west_lon_degree = 2;
        params->south_lat_degree = 49;
        params->east_lon_degree = 2.1;
        params->north_lat_degree = 49.1;
        params->typesCount = 1;
        auto type = PJ_TYPE_PROJECTED_CRS;
        params->types = &type;
        auto list = proj_get_crs_info_list_from_database(m_ctxt, "EPSG", params,
                                                         &result_count);
        ASSERT_NE(list, nullptr);
        EXPECT_GT(result_count, 1);
        for (int i = 0; i < result_count; i++) {
            EXPECT_LE(list[i]->west_lon_degree, params->west_lon_degree);
            EXPECT_LE(list[i]->south_lat_degree, params->south_lat_degree);
            EXPECT_GE(list[i]->east_lon_degree, params->east_lon_degree);
            EXPECT_GE(list[i]->north_lat_degree, params->north_lat_degree);
        }
        proj_get_crs_list_parameters_destroy(params);
        proj_crs_info_list_destroy(list);
    }

    // Filter on bbox (intersection)
    {
        int result_count = 0;
        auto params = proj_get_crs_list_parameters_create();
        params->bbox_valid = 1;
        params->west_lon_degree = 2;
        params->south_lat_degree = 49;
        params->east_lon_degree = 2.1;
        params->north_lat_degree = 49.1;
        params->crs_area_of_use_contains_bbox = 0;
        params->typesCount = 1;
        auto type = PJ_TYPE_PROJECTED_CRS;
        params->types = &type;
        auto list = proj_get_crs_info_list_from_database(m_ctxt, "EPSG", params,
                                                         &result_count);
        ASSERT_NE(list, nullptr);
        EXPECT_GT(result_count, 1);
        for (int i = 0; i < result_count; i++) {
            EXPECT_LE(list[i]->west_lon_degree, params->east_lon_degree);
            EXPECT_LE(list[i]->south_lat_degree, params->north_lat_degree);
            EXPECT_GE(list[i]->east_lon_degree, params->west_lon_degree);
            EXPECT_GE(list[i]->north_lat_degree, params->south_lat_degree);
        }
        proj_get_crs_list_parameters_destroy(params);
        proj_crs_info_list_destroy(list);
    }
}

// ---------------------------------------------------------------------------

TEST_F(CApi, proj_normalize_for_visualization) {

    {
        auto P = proj_create(m_ctxt, "+proj=utm +zone=31 +ellps=WGS84");
        ObjectKeeper keeper_P(P);
        ASSERT_NE(P, nullptr);
        auto Pnormalized = proj_normalize_for_visualization(m_ctxt, P);
        ObjectKeeper keeper_Pnormalized(Pnormalized);
        EXPECT_EQ(Pnormalized, nullptr);
    }

    auto P = proj_create_crs_to_crs(m_ctxt, "EPSG:4326", "EPSG:32631", nullptr);
    ObjectKeeper keeper_P(P);
    ASSERT_NE(P, nullptr);
    auto Pnormalized = proj_normalize_for_visualization(m_ctxt, P);
    ObjectKeeper keeper_Pnormalized(Pnormalized);
    ASSERT_NE(Pnormalized, nullptr);
    auto projstr = proj_as_proj_string(m_ctxt, Pnormalized, PJ_PROJ_5, nullptr);
    ASSERT_NE(projstr, nullptr);
    EXPECT_EQ(std::string(projstr),
              "+proj=pipeline +step +proj=unitconvert +xy_in=deg +xy_out=rad "
              "+step +proj=utm +zone=31 +ellps=WGS84");
}

// ---------------------------------------------------------------------------

TEST_F(CApi, proj_normalize_for_visualization_with_alternatives) {

    auto P = proj_create_crs_to_crs(m_ctxt, "EPSG:4326", "EPSG:3003", nullptr);
    ObjectKeeper keeper_P(P);
    ASSERT_NE(P, nullptr);
    auto Pnormalized = proj_normalize_for_visualization(m_ctxt, P);
    ObjectKeeper keeper_Pnormalized(Pnormalized);
    ASSERT_NE(Pnormalized, nullptr);

    {
        PJ_COORD c;
        // Approximately Roma
        c.lpz.lam = 12.5;
        c.lpz.phi = 42;
        c.lpz.z = 0;
        c = proj_trans(Pnormalized, PJ_FWD, c);
        EXPECT_NEAR(c.xy.x, 1789912.46264783037, 1e-8);
        EXPECT_NEAR(c.xy.y, 4655716.25402576849, 1e-8);
        auto projstr = proj_pj_info(Pnormalized).definition;
        ASSERT_NE(projstr, nullptr);
        EXPECT_EQ(std::string(projstr),
                  "proj=pipeline step proj=unitconvert xy_in=deg xy_out=rad "
                  "step proj=push v_3 step proj=cart ellps=WGS84 "
                  "step inv proj=helmert x=-104.1 y=-49.1 z=-9.9 rx=0.971 "
                  "ry=-2.917 rz=0.714 s=-11.68 convention=position_vector "
                  "step inv proj=cart ellps=intl step proj=pop v_3 "
                  "step proj=tmerc lat_0=0 lon_0=9 k=0.9996 x_0=1500000 "
                  "y_0=0 ellps=intl");
    }

    {
        PJ_COORD c;
        // Approximately Roma
        c.xyz.x = 1789912.46264783037;
        c.xyz.y = 4655716.25402576849;
        c.xyz.z = 0;
        c = proj_trans(Pnormalized, PJ_INV, c);
        EXPECT_NEAR(c.lp.lam, 12.5, 1e-8);
        EXPECT_NEAR(c.lp.phi, 42, 1e-8);
    }
}

// ---------------------------------------------------------------------------

TEST_F(CApi, proj_normalize_for_visualization_with_alternatives_reverse) {

    auto P = proj_create_crs_to_crs(m_ctxt, "EPSG:3003", "EPSG:4326", nullptr);
    ObjectKeeper keeper_P(P);
    ASSERT_NE(P, nullptr);
    auto Pnormalized = proj_normalize_for_visualization(m_ctxt, P);
    ObjectKeeper keeper_Pnormalized(Pnormalized);
    ASSERT_NE(Pnormalized, nullptr);

    PJ_COORD c;
    // Approximately Roma
    c.xyz.x = 1789912.46264783037;
    c.xyz.y = 4655716.25402576849;
    c.xyz.z = 0;
    c = proj_trans(Pnormalized, PJ_FWD, c);
    EXPECT_NEAR(c.lp.lam, 12.5, 1e-8);
    EXPECT_NEAR(c.lp.phi, 42, 1e-8);
}

// ---------------------------------------------------------------------------

TEST_F(CApi, proj_get_remarks) {
    auto co = proj_create_from_database(m_ctxt, "EPSG", "8048",
                                        PJ_CATEGORY_COORDINATE_OPERATION, false,
                                        nullptr);
    ObjectKeeper keeper(co);
    ASSERT_NE(co, nullptr);

    auto remarks = proj_get_remarks(co);
    ASSERT_NE(remarks, nullptr);
    EXPECT_EQ(
        remarks,
        std::string("Scale difference in ppb where 1/billion = 1E-9. "
                    "Derivation excluded Cocos, Christmas and Macquarie "
                    "Islands but is applied there. See codes 8444-46 for "
                    "equivalents using NTv2 method. See code 8447 for "
                    "alternative including distortion model for Aus only."));
}

// ---------------------------------------------------------------------------

TEST_F(CApi, proj_get_scope) {
    {
        auto co = proj_create_from_database(m_ctxt, "EPSG", "8048",
                                            PJ_CATEGORY_COORDINATE_OPERATION,
                                            false, nullptr);
        ObjectKeeper keeper(co);
        ASSERT_NE(co, nullptr);

        auto scope = proj_get_scope(co);
        ASSERT_NE(scope, nullptr);
        EXPECT_EQ(scope,
                  std::string("Conformal transformation of GDA94 coordinates "
                              "that have been derived through GNSS CORS."));
    }
    {
        auto P = proj_create(m_ctxt, "+proj=noop");
        ObjectKeeper keeper(P);
        ASSERT_NE(P, nullptr);
        auto scope = proj_get_scope(P);
        ASSERT_EQ(scope, nullptr);
    }
}

// ---------------------------------------------------------------------------

TEST_F(CApi, proj_concatoperation_get_step) {
    // Test on a non concatenated operation
    {
        auto co = proj_create_from_database(m_ctxt, "EPSG", "8048",
                                            PJ_CATEGORY_COORDINATE_OPERATION,
                                            false, nullptr);
        ObjectKeeper keeper(co);
        ASSERT_NE(co, nullptr);
        ASSERT_NE(proj_get_type(co), PJ_TYPE_CONCATENATED_OPERATION);

        ASSERT_EQ(proj_concatoperation_get_step_count(m_ctxt, co), 0);
        ASSERT_EQ(proj_concatoperation_get_step(m_ctxt, co, 0), nullptr);
    }
    // Test on a concatenated operation
    {
        auto ctxt = proj_create_operation_factory_context(m_ctxt, nullptr);
        ASSERT_NE(ctxt, nullptr);
        ContextKeeper keeper_ctxt(ctxt);

        // GDA94 / MGA zone 56
        auto source_crs = proj_create_from_database(
            m_ctxt, "EPSG", "28356", PJ_CATEGORY_CRS, false, nullptr);
        ASSERT_NE(source_crs, nullptr);
        ObjectKeeper keeper_source_crs(source_crs);

        // GDA2020 / MGA zone 56
        auto target_crs = proj_create_from_database(
            m_ctxt, "EPSG", "7856", PJ_CATEGORY_CRS, false, nullptr);
        ASSERT_NE(target_crs, nullptr);
        ObjectKeeper keeper_target_crs(target_crs);

        proj_operation_factory_context_set_spatial_criterion(
            m_ctxt, ctxt, PROJ_SPATIAL_CRITERION_PARTIAL_INTERSECTION);

        proj_operation_factory_context_set_grid_availability_use(
            m_ctxt, ctxt, PROJ_GRID_AVAILABILITY_IGNORED);

        auto res = proj_create_operations(m_ctxt, source_crs, target_crs, ctxt);
        ASSERT_NE(res, nullptr);
        ObjListKeeper keeper_res(res);

        ASSERT_GT(proj_list_get_count(res), 0);

        auto op = proj_list_get(m_ctxt, res, 0);
        ASSERT_NE(op, nullptr);
        ObjectKeeper keeper_op(op);

        ASSERT_EQ(proj_get_type(op), PJ_TYPE_CONCATENATED_OPERATION);
        ASSERT_EQ(proj_concatoperation_get_step_count(m_ctxt, op), 3);

        EXPECT_EQ(proj_concatoperation_get_step(m_ctxt, op, -1), nullptr);
        EXPECT_EQ(proj_concatoperation_get_step(m_ctxt, op, 3), nullptr);

        auto step = proj_concatoperation_get_step(m_ctxt, op, 1);
        ASSERT_NE(step, nullptr);
        ObjectKeeper keeper_step(step);

        const char *scope = proj_get_scope(step);
        EXPECT_NE(scope, nullptr);
        EXPECT_NE(std::string(scope), std::string());

        const char *remarks = proj_get_remarks(step);
        EXPECT_NE(remarks, nullptr);
        EXPECT_NE(std::string(remarks), std::string());
    }
}

// ---------------------------------------------------------------------------

TEST_F(CApi, proj_as_projjson) {
    auto obj = proj_create(
        m_ctxt,
        Ellipsoid::WGS84->exportToJSON(JSONFormatter::create().get()).c_str());
    ObjectKeeper keeper(obj);
    ASSERT_NE(obj, nullptr);

    {
        auto projjson = proj_as_projjson(m_ctxt, obj, nullptr);
        ASSERT_NE(projjson, nullptr);
        EXPECT_EQ(std::string(projjson),
                  "{\n"
                  "  \"$schema\": "
                  "\"https://proj.org/schemas/v0.1/projjson.schema.json\",\n"
                  "  \"type\": \"Ellipsoid\",\n"
                  "  \"name\": \"WGS 84\",\n"
                  "  \"semi_major_axis\": 6378137,\n"
                  "  \"inverse_flattening\": 298.257223563,\n"
                  "  \"id\": {\n"
                  "    \"authority\": \"EPSG\",\n"
                  "    \"code\": 7030\n"
                  "  }\n"
                  "}");
    }
    {
        const char *const options[] = {"INDENTATION_WIDTH=4", "SCHEMA=",
                                       nullptr};
        auto projjson = proj_as_projjson(m_ctxt, obj, options);
        ASSERT_NE(projjson, nullptr);
        EXPECT_EQ(std::string(projjson),
                  "{\n"
                  "    \"type\": \"Ellipsoid\",\n"
                  "    \"name\": \"WGS 84\",\n"
                  "    \"semi_major_axis\": 6378137,\n"
                  "    \"inverse_flattening\": 298.257223563,\n"
                  "    \"id\": {\n"
                  "        \"authority\": \"EPSG\",\n"
                  "        \"code\": 7030\n"
                  "    }\n"
                  "}");
    }
    {
        const char *const options[] = {"MULTILINE=NO", "SCHEMA=", nullptr};
        auto projjson = proj_as_projjson(m_ctxt, obj, options);
        ASSERT_NE(projjson, nullptr);
        EXPECT_EQ(std::string(projjson),
                  "{\"type\":\"Ellipsoid\",\"name\":\"WGS 84\","
                  "\"semi_major_axis\":6378137,"
                  "\"inverse_flattening\":298.257223563,"
                  "\"id\":{\"authority\":\"EPSG\",\"code\":7030}}");
    }
}

// ---------------------------------------------------------------------------

struct Fixture_proj_context_set_autoclose_database : public CApi {
    void test(bool autoclose) {
        proj_context_set_autoclose_database(m_ctxt, autoclose);

        auto c_path = proj_context_get_database_path(m_ctxt);
        ASSERT_TRUE(c_path != nullptr);
        std::string path(c_path);

        FILE *f = fopen(path.c_str(), "rb");
        ASSERT_NE(f, nullptr);
        fseek(f, 0, SEEK_END);
        auto length = ftell(f);
        std::string content;
        content.resize(static_cast<size_t>(length));
        fseek(f, 0, SEEK_SET);
        auto read_bytes = fread(&content[0], 1, content.size(), f);
        ASSERT_EQ(read_bytes, content.size());
        fclose(f);
        const char *tempdir = getenv("TEMP");
        if (!tempdir) {
            tempdir = getenv("TMP");
        }
        if (!tempdir) {
            tempdir = "/tmp";
        }
        std::string tmp_filename(
            std::string(tempdir) +
            "/test_proj_context_set_autoclose_database.db");
        f = fopen(tmp_filename.c_str(), "wb");
        if (!f) {
            std::cerr << "Cannot create " << tmp_filename << std::endl;
            return;
        }
        fwrite(content.data(), 1, content.size(), f);
        fclose(f);

        {
            sqlite3 *db = nullptr;
            sqlite3_open_v2(tmp_filename.c_str(), &db, SQLITE_OPEN_READWRITE,
                            nullptr);
            ASSERT_NE(db, nullptr);
            ASSERT_TRUE(sqlite3_exec(db, "UPDATE geodetic_crs SET name = 'foo' "
                                         "WHERE auth_name = 'EPSG' and code = "
                                         "'4326'",
                                     nullptr, nullptr, nullptr) == SQLITE_OK);
            sqlite3_close(db);
        }

        EXPECT_TRUE(proj_context_set_database_path(m_ctxt, tmp_filename.c_str(),
                                                   nullptr, nullptr));
        {
            auto crs = proj_create_from_database(
                m_ctxt, "EPSG", "4326", PJ_CATEGORY_CRS, false, nullptr);
            ObjectKeeper keeper(crs);
            ASSERT_NE(crs, nullptr);
            EXPECT_EQ(proj_get_name(crs), std::string("foo"));
        }

        {
            sqlite3 *db = nullptr;
            sqlite3_open_v2(tmp_filename.c_str(), &db, SQLITE_OPEN_READWRITE,
                            nullptr);
            ASSERT_NE(db, nullptr);
            ASSERT_TRUE(sqlite3_exec(db, "UPDATE geodetic_crs SET name = 'bar' "
                                         "WHERE auth_name = 'EPSG' and code = "
                                         "'4326'",
                                     nullptr, nullptr, nullptr) == SQLITE_OK);
            sqlite3_close(db);
        }
        {
            auto crs = proj_create_from_database(
                m_ctxt, "EPSG", "4326", PJ_CATEGORY_CRS, false, nullptr);
            ObjectKeeper keeper(crs);
            ASSERT_NE(crs, nullptr);
            EXPECT_EQ(proj_get_name(crs),
                      std::string(autoclose ? "bar" : "foo"));
        }

        if (!autoclose) {
            proj_context_destroy(m_ctxt);
            m_ctxt = nullptr;
        }
        std::remove(tmp_filename.c_str());
    }
};

TEST_F(Fixture_proj_context_set_autoclose_database,
       proj_context_set_autoclose_database_true) {
    test(true);
}

TEST_F(Fixture_proj_context_set_autoclose_database,
       proj_context_set_autoclose_database_false) {
    test(false);
}

// ---------------------------------------------------------------------------

TEST_F(CApi, proj_create_crs_to_crs_from_pj) {

    auto src = proj_create(m_ctxt, "EPSG:4326");
    ObjectKeeper keeper_src(src);
    ASSERT_NE(src, nullptr);

    auto dst = proj_create(m_ctxt, "EPSG:32631");
    ObjectKeeper keeper_dst(dst);
    ASSERT_NE(dst, nullptr);

    auto P = proj_create_crs_to_crs_from_pj(m_ctxt, src, dst, nullptr, nullptr);
    ObjectKeeper keeper_P(P);
    ASSERT_NE(P, nullptr);
    auto Pnormalized = proj_normalize_for_visualization(m_ctxt, P);
    ObjectKeeper keeper_Pnormalized(Pnormalized);
    ASSERT_NE(Pnormalized, nullptr);
    auto projstr = proj_as_proj_string(m_ctxt, Pnormalized, PJ_PROJ_5, nullptr);
    ASSERT_NE(projstr, nullptr);
    EXPECT_EQ(std::string(projstr),
              "+proj=pipeline +step +proj=unitconvert +xy_in=deg +xy_out=rad "
              "+step +proj=utm +zone=31 +ellps=WGS84");
}

// ---------------------------------------------------------------------------

TEST_F(CApi, proj_crs_promote_to_3D) {

    auto crs2D =
        proj_create(m_ctxt, GeographicCRS::EPSG_4326
                                ->exportToWKT(WKTFormatter::create().get())
                                .c_str());
    ObjectKeeper keeper_crs2D(crs2D);
    EXPECT_NE(crs2D, nullptr);

    auto crs3D = proj_crs_promote_to_3D(m_ctxt, nullptr, crs2D);
    ObjectKeeper keeper_crs3D(crs3D);
    EXPECT_NE(crs3D, nullptr);

    auto cs = proj_crs_get_coordinate_system(m_ctxt, crs3D);
    ASSERT_NE(cs, nullptr);
    ObjectKeeper keeperCs(cs);
    EXPECT_EQ(proj_cs_get_axis_count(m_ctxt, cs), 3);

    auto code = proj_get_id_code(crs3D, 0);
    ASSERT_TRUE(code != nullptr);
    EXPECT_EQ(code, std::string("4979"));
}

// ---------------------------------------------------------------------------

TEST_F(CApi, proj_crs_create_projected_3D_crs_from_2D) {

    auto projCRS = proj_create_from_database(m_ctxt, "EPSG", "32631",
                                             PJ_CATEGORY_CRS, false, nullptr);
    ASSERT_NE(projCRS, nullptr);
    ObjectKeeper keeper_projCRS(projCRS);

    {
        auto geog3DCRS = proj_create_from_database(
            m_ctxt, "EPSG", "4979", PJ_CATEGORY_CRS, false, nullptr);
        ASSERT_NE(geog3DCRS, nullptr);
        ObjectKeeper keeper_geog3DCRS(geog3DCRS);

        auto crs3D = proj_crs_create_projected_3D_crs_from_2D(
            m_ctxt, nullptr, projCRS, geog3DCRS);
        ObjectKeeper keeper_crs3D(crs3D);
        EXPECT_NE(crs3D, nullptr);

        EXPECT_EQ(proj_get_type(crs3D), PJ_TYPE_PROJECTED_CRS);

        EXPECT_EQ(std::string(proj_get_name(crs3D)),
                  std::string(proj_get_name(projCRS)));

        auto cs = proj_crs_get_coordinate_system(m_ctxt, crs3D);
        ASSERT_NE(cs, nullptr);
        ObjectKeeper keeperCs(cs);
        EXPECT_EQ(proj_cs_get_axis_count(m_ctxt, cs), 3);
    }

    {
        auto crs3D = proj_crs_create_projected_3D_crs_from_2D(m_ctxt, nullptr,
                                                              projCRS, nullptr);
        ObjectKeeper keeper_crs3D(crs3D);
        EXPECT_NE(crs3D, nullptr);

        EXPECT_EQ(proj_get_type(crs3D), PJ_TYPE_PROJECTED_CRS);

        EXPECT_EQ(std::string(proj_get_name(crs3D)),
                  std::string(proj_get_name(projCRS)));

        auto cs = proj_crs_get_coordinate_system(m_ctxt, crs3D);
        ASSERT_NE(cs, nullptr);
        ObjectKeeper keeperCs(cs);
        EXPECT_EQ(proj_cs_get_axis_count(m_ctxt, cs), 3);
    }
}

// ---------------------------------------------------------------------------

TEST_F(CApi, proj_create_crs_to_crs_with_only_ballpark_transformations) {
    // ETRS89 / UTM zone 31N + EGM96 height to WGS 84 (G1762)
    auto P =
        proj_create_crs_to_crs(m_ctxt, "EPSG:25831+5773", "EPSG:7665", nullptr);
    ObjectKeeper keeper_P(P);
    ASSERT_NE(P, nullptr);
    auto Pnormalized = proj_normalize_for_visualization(m_ctxt, P);
    ObjectKeeper keeper_Pnormalized(Pnormalized);
    ASSERT_NE(Pnormalized, nullptr);

    PJ_COORD coord;
    coord.xyzt.x = 500000;
    coord.xyzt.y = 4500000;
    coord.xyzt.z = 0;
    coord.xyzt.t = 0;
    coord = proj_trans(Pnormalized, PJ_FWD, coord);
    EXPECT_NEAR(coord.xyzt.x, 3.0, 1e-9);
    EXPECT_NEAR(coord.xyzt.y, 40.65085651660555, 1e-9);
    if (coord.xyzt.z != 0) {
        // z will depend if the egm96_15.gtx grid is there or not
        EXPECT_NEAR(coord.xyzt.z, 47.04784081844435, 1e-3);
    }
}

} // namespace
