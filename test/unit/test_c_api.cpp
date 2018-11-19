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

#include "proj.h"

#include "proj/common.hpp"
#include "proj/coordinateoperation.hpp"
#include "proj/coordinatesystem.hpp"
#include "proj/crs.hpp"
#include "proj/datum.hpp"
#include "proj/io.hpp"
#include "proj/metadata.hpp"
#include "proj/util.hpp"

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

    void TearDown() override { proj_context_destroy(m_ctxt); }

    static BoundCRSNNPtr createBoundCRS() {
        return BoundCRS::create(
            GeographicCRS::EPSG_4807, GeographicCRS::EPSG_4326,
            Transformation::create(PropertyMap(), GeographicCRS::EPSG_4807,
                                   GeographicCRS::EPSG_4326, nullptr,
                                   PropertyMap(), {}, {}, {}));
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
        PJ_OBJ *m_obj = nullptr;
        explicit ObjectKeeper(PJ_OBJ *obj) : m_obj(obj) {}
        ~ObjectKeeper() { proj_obj_unref(m_obj); }

        ObjectKeeper(const ObjectKeeper &) = delete;
        ObjectKeeper &operator=(const ObjectKeeper &) = delete;
    };

    struct ContextKeeper {
        PJ_OPERATION_FACTORY_CONTEXT *m_op_ctxt = nullptr;
        explicit ContextKeeper(PJ_OPERATION_FACTORY_CONTEXT *op_ctxt)
            : m_op_ctxt(op_ctxt) {}
        ~ContextKeeper() { proj_operation_factory_context_unref(m_op_ctxt); }

        ContextKeeper(const ContextKeeper &) = delete;
        ContextKeeper &operator=(const ContextKeeper &) = delete;
    };

    struct ObjListKeeper {
        PJ_OBJ_LIST *m_res = nullptr;
        explicit ObjListKeeper(PJ_OBJ_LIST *res) : m_res(res) {}
        ~ObjListKeeper() { proj_obj_list_unref(m_res); }

        ObjListKeeper(const ObjListKeeper &) = delete;
        ObjListKeeper &operator=(const ObjListKeeper &) = delete;
    };
};

// ---------------------------------------------------------------------------

TEST_F(CApi, proj_obj_create_from_user_input) {
    proj_obj_unref(nullptr);
    EXPECT_EQ(proj_obj_create_from_user_input(m_ctxt, "invalid", nullptr),
              nullptr);
    {
        auto obj = proj_obj_create_from_user_input(
            m_ctxt,
            GeographicCRS::EPSG_4326->exportToWKT(WKTFormatter::create().get())
                .c_str(),
            nullptr);
        ObjectKeeper keeper(obj);
        EXPECT_NE(obj, nullptr);
    }
    {
        auto obj =
            proj_obj_create_from_user_input(m_ctxt, "EPSG:4326", nullptr);
        ObjectKeeper keeper(obj);
        EXPECT_NE(obj, nullptr);
    }
}

// ---------------------------------------------------------------------------

TEST_F(CApi, proj_obj_create_from_wkt) {
    proj_obj_unref(nullptr);
    EXPECT_EQ(proj_obj_create_from_wkt(m_ctxt, "invalid", nullptr), nullptr);
    auto obj = proj_obj_create_from_wkt(
        m_ctxt,
        GeographicCRS::EPSG_4326->exportToWKT(WKTFormatter::create().get())
            .c_str(),
        nullptr);
    ObjectKeeper keeper(obj);
    EXPECT_NE(obj, nullptr);
}

// ---------------------------------------------------------------------------

TEST_F(CApi, proj_obj_create_from_proj_string) {
    proj_obj_unref(nullptr);
    EXPECT_EQ(proj_obj_create_from_proj_string(m_ctxt, "invalid", nullptr),
              nullptr);
    auto obj =
        proj_obj_create_from_proj_string(m_ctxt, "+proj=longlat", nullptr);
    ObjectKeeper keeper(obj);
    EXPECT_NE(obj, nullptr);
}

// ---------------------------------------------------------------------------

TEST_F(CApi, proj_obj_as_wkt) {
    auto obj = proj_obj_create_from_wkt(
        m_ctxt,
        GeographicCRS::EPSG_4326->exportToWKT(WKTFormatter::create().get())
            .c_str(),
        nullptr);
    ObjectKeeper keeper(obj);
    ASSERT_NE(obj, nullptr);

    {
        auto wkt = proj_obj_as_wkt(obj, PJ_WKT2_2018, nullptr);
        ASSERT_NE(wkt, nullptr);
        EXPECT_TRUE(std::string(wkt).find("GEOGCRS[") == 0) << wkt;
    }

    {
        auto wkt = proj_obj_as_wkt(obj, PJ_WKT2_2018_SIMPLIFIED, nullptr);
        ASSERT_NE(wkt, nullptr);
        EXPECT_TRUE(std::string(wkt).find("GEOGCRS[") == 0) << wkt;
        EXPECT_TRUE(std::string(wkt).find("ANGULARUNIT[") == std::string::npos)
            << wkt;
    }

    {
        auto wkt = proj_obj_as_wkt(obj, PJ_WKT2_2015, nullptr);
        ASSERT_NE(wkt, nullptr);
        EXPECT_TRUE(std::string(wkt).find("GEODCRS[") == 0) << wkt;
    }

    {
        auto wkt = proj_obj_as_wkt(obj, PJ_WKT2_2015_SIMPLIFIED, nullptr);
        ASSERT_NE(wkt, nullptr);
        EXPECT_TRUE(std::string(wkt).find("GEODCRS[") == 0) << wkt;
        EXPECT_TRUE(std::string(wkt).find("ANGULARUNIT[") == std::string::npos)
            << wkt;
    }

    {
        auto wkt = proj_obj_as_wkt(obj, PJ_WKT1_GDAL, nullptr);
        ASSERT_NE(wkt, nullptr);
        EXPECT_TRUE(std::string(wkt).find("GEOGCS[\"WGS 84\"") == 0) << wkt;
    }

    {
        auto wkt = proj_obj_as_wkt(obj, PJ_WKT1_ESRI, nullptr);
        ASSERT_NE(wkt, nullptr);
        EXPECT_TRUE(std::string(wkt).find("GEOGCS[\"GCS_WGS_1984\"") == 0)
            << wkt;
    }

    // MULTILINE=NO
    {
        const char *const options[] = {"MULTILINE=NO", nullptr};
        auto wkt = proj_obj_as_wkt(obj, PJ_WKT1_GDAL, options);
        ASSERT_NE(wkt, nullptr);
        EXPECT_TRUE(std::string(wkt).find("\n") == std::string::npos) << wkt;
    }

    // INDENTATION_WIDTH=2
    {
        const char *const options[] = {"INDENTATION_WIDTH=2", nullptr};
        auto wkt = proj_obj_as_wkt(obj, PJ_WKT1_GDAL, options);
        ASSERT_NE(wkt, nullptr);
        EXPECT_TRUE(std::string(wkt).find("\n  DATUM") != std::string::npos)
            << wkt;
    }

    // OUTPUT_AXIS=NO
    {
        const char *const options[] = {"OUTPUT_AXIS=NO", nullptr};
        auto wkt = proj_obj_as_wkt(obj, PJ_WKT1_GDAL, options);
        ASSERT_NE(wkt, nullptr);
        EXPECT_TRUE(std::string(wkt).find("AXIS") == std::string::npos) << wkt;
    }

    // OUTPUT_AXIS=AUTO
    {
        const char *const options[] = {"OUTPUT_AXIS=AUTO", nullptr};
        auto wkt = proj_obj_as_wkt(obj, PJ_WKT1_GDAL, options);
        ASSERT_NE(wkt, nullptr);
        EXPECT_TRUE(std::string(wkt).find("AXIS") == std::string::npos) << wkt;
    }

    // OUTPUT_AXIS=YES
    {
        const char *const options[] = {"OUTPUT_AXIS=YES", nullptr};
        auto wkt = proj_obj_as_wkt(obj, PJ_WKT1_GDAL, options);
        ASSERT_NE(wkt, nullptr);
        EXPECT_TRUE(std::string(wkt).find("AXIS") != std::string::npos) << wkt;
    }

    // unsupported option
    {
        const char *const options[] = {"unsupported=yes", nullptr};
        auto wkt = proj_obj_as_wkt(obj, PJ_WKT2_2018, options);
        EXPECT_EQ(wkt, nullptr);
    }
}

// ---------------------------------------------------------------------------

TEST_F(CApi, proj_obj_as_wkt_incompatible_WKT1) {
    auto obj = proj_obj_create_from_wkt(
        m_ctxt,
        createBoundCRS()->exportToWKT(WKTFormatter::create().get()).c_str(),
        nullptr);
    ObjectKeeper keeper(obj);
    ASSERT_NE(obj, nullptr);

    auto wkt1_GDAL = proj_obj_as_wkt(obj, PJ_WKT1_GDAL, nullptr);
    ASSERT_EQ(wkt1_GDAL, nullptr);
}

// ---------------------------------------------------------------------------

TEST_F(CApi, proj_obj_as_proj_string) {
    auto obj = proj_obj_create_from_wkt(
        m_ctxt,
        GeographicCRS::EPSG_4326->exportToWKT(WKTFormatter::create().get())
            .c_str(),
        nullptr);
    ObjectKeeper keeper(obj);
    ASSERT_NE(obj, nullptr);

    {
        auto proj_5 = proj_obj_as_proj_string(obj, PJ_PROJ_5, nullptr);
        ASSERT_NE(proj_5, nullptr);
        EXPECT_EQ(std::string(proj_5), "+proj=pipeline +step +proj=longlat "
                                       "+ellps=WGS84 +step +proj=unitconvert "
                                       "+xy_in=rad +xy_out=deg +step "
                                       "+proj=axisswap +order=2,1");
    }
    {
        auto proj_4 = proj_obj_as_proj_string(obj, PJ_PROJ_4, nullptr);
        ASSERT_NE(proj_4, nullptr);
        EXPECT_EQ(std::string(proj_4), "+proj=longlat +datum=WGS84 +no_defs");
    }
}

// ---------------------------------------------------------------------------

TEST_F(CApi, proj_obj_as_proj_string_incompatible_WKT1) {
    auto obj = proj_obj_create_from_wkt(
        m_ctxt,
        createBoundCRS()->exportToWKT(WKTFormatter::create().get()).c_str(),
        nullptr);
    ObjectKeeper keeper(obj);
    ASSERT_NE(obj, nullptr);

    auto str = proj_obj_as_proj_string(obj, PJ_PROJ_5, nullptr);
    ASSERT_EQ(str, nullptr);
}

// ---------------------------------------------------------------------------

TEST_F(CApi, proj_obj_as_proj_string_etmerc_option_yes) {
    auto obj = proj_obj_create_from_proj_string(m_ctxt, "+proj=tmerc", nullptr);
    ObjectKeeper keeper(obj);
    ASSERT_NE(obj, nullptr);

    const char *options[] = {"USE_ETMERC=YES", nullptr};
    auto str = proj_obj_as_proj_string(obj, PJ_PROJ_4, options);
    ASSERT_NE(str, nullptr);
    EXPECT_EQ(str, std::string("+proj=etmerc +lat_0=0 +lon_0=0 +k=1 +x_0=0 "
                               "+y_0=0 +datum=WGS84 +units=m +no_defs"));
}

// ---------------------------------------------------------------------------

TEST_F(CApi, proj_obj_as_proj_string_etmerc_option_no) {
    auto obj =
        proj_obj_create_from_proj_string(m_ctxt, "+proj=utm +zone=31", nullptr);
    ObjectKeeper keeper(obj);
    ASSERT_NE(obj, nullptr);

    const char *options[] = {"USE_ETMERC=NO", nullptr};
    auto str = proj_obj_as_proj_string(obj, PJ_PROJ_4, options);
    ASSERT_NE(str, nullptr);
    EXPECT_EQ(str, std::string("+proj=tmerc +lat_0=0 +lon_0=3 +k=0.9996 "
                               "+x_0=500000 +y_0=0 +datum=WGS84 +units=m "
                               "+no_defs"));
}

// ---------------------------------------------------------------------------

TEST_F(CApi, proj_obj_crs_create_bound_crs_to_WGS84) {
    auto crs = proj_obj_create_from_database(
        m_ctxt, "EPSG", "3844", PJ_OBJ_CATEGORY_CRS, false, nullptr);
    ObjectKeeper keeper(crs);
    ASSERT_NE(crs, nullptr);

    auto res = proj_obj_crs_create_bound_crs_to_WGS84(crs);
    ObjectKeeper keeper_res(res);
    ASSERT_NE(res, nullptr);

    auto proj_4 = proj_obj_as_proj_string(res, PJ_PROJ_4, nullptr);
    ASSERT_NE(proj_4, nullptr);
    EXPECT_EQ(std::string(proj_4),
              "+proj=sterea +lat_0=46 +lon_0=25 +k=0.99975 +x_0=500000 "
              "+y_0=500000 +ellps=krass "
              "+towgs84=2.329,-147.042,-92.08,-0.309,0.325,0.497,5.69 "
              "+units=m +no_defs");
}

// ---------------------------------------------------------------------------

TEST_F(CApi, proj_obj_crs_create_bound_crs_to_WGS84_on_invalid_type) {
    auto obj = proj_obj_create_from_wkt(
        m_ctxt, createProjectedCRS()
                    ->derivingConversion()
                    ->exportToWKT(WKTFormatter::create().get())
                    .c_str(),
        nullptr);
    ObjectKeeper keeper(obj);
    ASSERT_NE(obj, nullptr);

    auto res = proj_obj_crs_create_bound_crs_to_WGS84(obj);
    ASSERT_EQ(res, nullptr);
}

// ---------------------------------------------------------------------------

TEST_F(CApi, proj_obj_get_name) {
    auto obj = proj_obj_create_from_wkt(
        m_ctxt,
        GeographicCRS::EPSG_4326->exportToWKT(WKTFormatter::create().get())
            .c_str(),
        nullptr);
    ObjectKeeper keeper(obj);
    ASSERT_NE(obj, nullptr);
    auto name = proj_obj_get_name(obj);
    ASSERT_TRUE(name != nullptr);
    EXPECT_EQ(name, std::string("WGS 84"));
    EXPECT_EQ(name, proj_obj_get_name(obj));
}

// ---------------------------------------------------------------------------

TEST_F(CApi, proj_obj_get_id_auth_name) {
    auto obj = proj_obj_create_from_wkt(
        m_ctxt,
        GeographicCRS::EPSG_4326->exportToWKT(WKTFormatter::create().get())
            .c_str(),
        nullptr);
    ObjectKeeper keeper(obj);
    ASSERT_NE(obj, nullptr);
    auto auth = proj_obj_get_id_auth_name(obj, 0);
    ASSERT_TRUE(auth != nullptr);
    EXPECT_EQ(auth, std::string("EPSG"));
    EXPECT_EQ(auth, proj_obj_get_id_auth_name(obj, 0));
    EXPECT_EQ(proj_obj_get_id_auth_name(obj, -1), nullptr);
    EXPECT_EQ(proj_obj_get_id_auth_name(obj, 1), nullptr);
}

// ---------------------------------------------------------------------------

TEST_F(CApi, proj_obj_get_id_code) {
    auto obj = proj_obj_create_from_wkt(
        m_ctxt,
        GeographicCRS::EPSG_4326->exportToWKT(WKTFormatter::create().get())
            .c_str(),
        nullptr);
    ObjectKeeper keeper(obj);
    ASSERT_NE(obj, nullptr);
    auto code = proj_obj_get_id_code(obj, 0);
    ASSERT_TRUE(code != nullptr);
    EXPECT_EQ(code, std::string("4326"));
    EXPECT_EQ(code, proj_obj_get_id_code(obj, 0));
    EXPECT_EQ(proj_obj_get_id_code(obj, -1), nullptr);
    EXPECT_EQ(proj_obj_get_id_code(obj, 1), nullptr);
}

// ---------------------------------------------------------------------------

TEST_F(CApi, proj_obj_get_type) {
    {
        auto obj = proj_obj_create_from_wkt(
            m_ctxt,
            GeographicCRS::EPSG_4326->exportToWKT(WKTFormatter::create().get())
                .c_str(),
            nullptr);
        ObjectKeeper keeper(obj);
        ASSERT_NE(obj, nullptr);
        EXPECT_EQ(proj_obj_get_type(obj), PJ_OBJ_TYPE_GEOGRAPHIC_2D_CRS);
    }
    {
        auto obj = proj_obj_create_from_wkt(
            m_ctxt,
            GeographicCRS::EPSG_4979->exportToWKT(WKTFormatter::create().get())
                .c_str(),
            nullptr);
        ObjectKeeper keeper(obj);
        ASSERT_NE(obj, nullptr);
        EXPECT_EQ(proj_obj_get_type(obj), PJ_OBJ_TYPE_GEOGRAPHIC_3D_CRS);
    }
    {
        auto obj = proj_obj_create_from_wkt(
            m_ctxt,
            GeographicCRS::EPSG_4978->exportToWKT(WKTFormatter::create().get())
                .c_str(),
            nullptr);
        ObjectKeeper keeper(obj);
        ASSERT_NE(obj, nullptr);
        EXPECT_EQ(proj_obj_get_type(obj), PJ_OBJ_TYPE_GEOCENTRIC_CRS);
    }
    {
        auto obj = proj_obj_create_from_wkt(
            m_ctxt, GeographicCRS::EPSG_4326->datum()
                        ->exportToWKT(WKTFormatter::create().get())
                        .c_str(),
            nullptr);
        ObjectKeeper keeper(obj);
        ASSERT_NE(obj, nullptr);
        EXPECT_EQ(proj_obj_get_type(obj), PJ_OBJ_TYPE_GEODETIC_REFERENCE_FRAME);
    }
    {
        auto obj = proj_obj_create_from_wkt(
            m_ctxt, GeographicCRS::EPSG_4326->ellipsoid()
                        ->exportToWKT(WKTFormatter::create().get())
                        .c_str(),
            nullptr);
        ObjectKeeper keeper(obj);
        ASSERT_NE(obj, nullptr);
        EXPECT_EQ(proj_obj_get_type(obj), PJ_OBJ_TYPE_ELLIPSOID);
    }
    {
        auto obj = proj_obj_create_from_wkt(
            m_ctxt, createProjectedCRS()
                        ->exportToWKT(WKTFormatter::create().get())
                        .c_str(),
            nullptr);
        ObjectKeeper keeper(obj);
        ASSERT_NE(obj, nullptr);
        EXPECT_EQ(proj_obj_get_type(obj), PJ_OBJ_TYPE_PROJECTED_CRS);
    }
    {
        auto obj = proj_obj_create_from_wkt(
            m_ctxt, createVerticalCRS()
                        ->exportToWKT(WKTFormatter::create().get())
                        .c_str(),
            nullptr);
        ObjectKeeper keeper(obj);
        ASSERT_NE(obj, nullptr);
        EXPECT_EQ(proj_obj_get_type(obj), PJ_OBJ_TYPE_VERTICAL_CRS);
    }
    {
        auto obj = proj_obj_create_from_wkt(
            m_ctxt, createVerticalCRS()
                        ->datum()
                        ->exportToWKT(WKTFormatter::create().get())
                        .c_str(),
            nullptr);
        ObjectKeeper keeper(obj);
        ASSERT_NE(obj, nullptr);
        EXPECT_EQ(proj_obj_get_type(obj), PJ_OBJ_TYPE_VERTICAL_REFERENCE_FRAME);
    }
    {
        auto obj = proj_obj_create_from_wkt(
            m_ctxt, createProjectedCRS()
                        ->derivingConversion()
                        ->exportToWKT(WKTFormatter::create().get())
                        .c_str(),
            nullptr);
        ObjectKeeper keeper(obj);
        ASSERT_NE(obj, nullptr);
        EXPECT_EQ(proj_obj_get_type(obj), PJ_OBJ_TYPE_CONVERSION);
    }
    {
        auto obj = proj_obj_create_from_wkt(
            m_ctxt,
            createBoundCRS()->exportToWKT(WKTFormatter::create().get()).c_str(),
            nullptr);
        ObjectKeeper keeper(obj);
        ASSERT_NE(obj, nullptr);
        EXPECT_EQ(proj_obj_get_type(obj), PJ_OBJ_TYPE_BOUND_CRS);
    }
    {
        auto obj = proj_obj_create_from_wkt(
            m_ctxt, createBoundCRS()
                        ->transformation()
                        ->exportToWKT(WKTFormatter::create().get())
                        .c_str(),
            nullptr);
        ObjectKeeper keeper(obj);
        ASSERT_NE(obj, nullptr);
        EXPECT_EQ(proj_obj_get_type(obj), PJ_OBJ_TYPE_TRANSFORMATION);
    }
    {
        auto obj = proj_obj_create_from_wkt(m_ctxt, "AUTHORITY[\"EPSG\", 4326]",
                                            nullptr);
        ObjectKeeper keeper(obj);
        ASSERT_EQ(obj, nullptr);
    }
}

// ---------------------------------------------------------------------------

TEST_F(CApi, proj_obj_create_from_database) {
    {
        auto crs = proj_obj_create_from_database(
            m_ctxt, "EPSG", "-1", PJ_OBJ_CATEGORY_CRS, false, nullptr);
        ASSERT_EQ(crs, nullptr);
    }
    {
        auto crs = proj_obj_create_from_database(
            m_ctxt, "EPSG", "4326", PJ_OBJ_CATEGORY_CRS, false, nullptr);
        ASSERT_NE(crs, nullptr);
        ObjectKeeper keeper(crs);
        EXPECT_TRUE(proj_obj_is_crs(crs));
        EXPECT_FALSE(proj_obj_is_deprecated(crs));
        EXPECT_EQ(proj_obj_get_type(crs), PJ_OBJ_TYPE_GEOGRAPHIC_2D_CRS);
    }
    {
        auto crs = proj_obj_create_from_database(
            m_ctxt, "EPSG", "6871", PJ_OBJ_CATEGORY_CRS, false, nullptr);
        ASSERT_NE(crs, nullptr);
        ObjectKeeper keeper(crs);
        EXPECT_TRUE(proj_obj_is_crs(crs));
        EXPECT_EQ(proj_obj_get_type(crs), PJ_OBJ_TYPE_COMPOUND_CRS);
    }
    {
        auto ellipsoid = proj_obj_create_from_database(
            m_ctxt, "EPSG", "7030", PJ_OBJ_CATEGORY_ELLIPSOID, false, nullptr);
        ASSERT_NE(ellipsoid, nullptr);
        ObjectKeeper keeper(ellipsoid);
        EXPECT_EQ(proj_obj_get_type(ellipsoid), PJ_OBJ_TYPE_ELLIPSOID);
    }
    {
        auto datum = proj_obj_create_from_database(
            m_ctxt, "EPSG", "6326", PJ_OBJ_CATEGORY_DATUM, false, nullptr);
        ASSERT_NE(datum, nullptr);
        ObjectKeeper keeper(datum);
        EXPECT_EQ(proj_obj_get_type(datum),
                  PJ_OBJ_TYPE_GEODETIC_REFERENCE_FRAME);
    }
    {
        auto op = proj_obj_create_from_database(
            m_ctxt, "EPSG", "16031", PJ_OBJ_CATEGORY_COORDINATE_OPERATION,
            false, nullptr);
        ASSERT_NE(op, nullptr);
        ObjectKeeper keeper(op);
        EXPECT_EQ(proj_obj_get_type(op), PJ_OBJ_TYPE_CONVERSION);
    }
}

// ---------------------------------------------------------------------------

TEST_F(CApi, proj_crs) {
    auto crs = proj_obj_create_from_wkt(
        m_ctxt,
        createProjectedCRS()
            ->exportToWKT(
                WKTFormatter::create(WKTFormatter::Convention::WKT1_GDAL).get())
            .c_str(),
        nullptr);
    ASSERT_NE(crs, nullptr);
    ObjectKeeper keeper(crs);
    EXPECT_TRUE(proj_obj_is_crs(crs));

    auto geodCRS = proj_obj_crs_get_geodetic_crs(crs);
    ASSERT_NE(geodCRS, nullptr);
    ObjectKeeper keeper_geogCRS(geodCRS);
    EXPECT_TRUE(proj_obj_is_crs(geodCRS));
    auto geogCRS_name = proj_obj_get_name(geodCRS);
    ASSERT_TRUE(geogCRS_name != nullptr);
    EXPECT_EQ(geogCRS_name, std::string("WGS 84"));

    auto datum = proj_obj_crs_get_horizontal_datum(crs);
    ASSERT_NE(datum, nullptr);
    ObjectKeeper keeper_datum(datum);
    auto datum_name = proj_obj_get_name(datum);
    ASSERT_TRUE(datum_name != nullptr);
    EXPECT_EQ(datum_name, std::string("World Geodetic System 1984"));

    auto ellipsoid = proj_obj_get_ellipsoid(crs);
    ASSERT_NE(ellipsoid, nullptr);
    ObjectKeeper keeper_ellipsoid(ellipsoid);
    auto ellipsoid_name = proj_obj_get_name(ellipsoid);
    ASSERT_TRUE(ellipsoid_name != nullptr);
    EXPECT_EQ(ellipsoid_name, std::string("WGS 84"));

    auto ellipsoid_from_datum = proj_obj_get_ellipsoid(datum);
    ASSERT_NE(ellipsoid_from_datum, nullptr);
    ObjectKeeper keeper_ellipsoid_from_datum(ellipsoid_from_datum);

    EXPECT_EQ(proj_obj_get_ellipsoid(ellipsoid), nullptr);
    EXPECT_FALSE(proj_obj_is_crs(ellipsoid));

    double a;
    double b;
    int b_is_computed;
    double rf;
    EXPECT_TRUE(proj_obj_ellipsoid_get_parameters(ellipsoid, nullptr, nullptr,
                                                  nullptr, nullptr));
    EXPECT_TRUE(proj_obj_ellipsoid_get_parameters(ellipsoid, &a, &b,
                                                  &b_is_computed, &rf));
    EXPECT_FALSE(
        proj_obj_ellipsoid_get_parameters(crs, &a, &b, &b_is_computed, &rf));
    EXPECT_EQ(a, 6378137);
    EXPECT_NEAR(b, 6356752.31424518, 1e-9);
    EXPECT_EQ(b_is_computed, 1);
    EXPECT_EQ(rf, 298.257223563);
    auto id = proj_obj_get_id_code(ellipsoid, 0);
    ASSERT_TRUE(id != nullptr);
    EXPECT_EQ(id, std::string("7030"));
}

// ---------------------------------------------------------------------------

TEST_F(CApi, proj_obj_get_prime_meridian) {
    auto crs = proj_obj_create_from_wkt(
        m_ctxt,
        createProjectedCRS()
            ->exportToWKT(
                WKTFormatter::create(WKTFormatter::Convention::WKT1_GDAL).get())
            .c_str(),
        nullptr);
    ASSERT_NE(crs, nullptr);
    ObjectKeeper keeper(crs);

    auto pm = proj_obj_get_prime_meridian(crs);
    ASSERT_NE(pm, nullptr);
    ObjectKeeper keeper_pm(pm);
    auto pm_name = proj_obj_get_name(pm);
    ASSERT_TRUE(pm_name != nullptr);
    EXPECT_EQ(pm_name, std::string("Greenwich"));

    EXPECT_EQ(proj_obj_get_prime_meridian(pm), nullptr);

    EXPECT_TRUE(
        proj_obj_prime_meridian_get_parameters(pm, nullptr, nullptr, nullptr));
    double longitude = -1;
    double longitude_unit = 0;
    const char *longitude_unit_name = nullptr;
    EXPECT_TRUE(proj_obj_prime_meridian_get_parameters(
        pm, &longitude, &longitude_unit, &longitude_unit_name));
    EXPECT_EQ(longitude, 0);
    EXPECT_NEAR(longitude_unit, UnitOfMeasure::DEGREE.conversionToSI(), 1e-10);
    ASSERT_TRUE(longitude_unit_name != nullptr);
    EXPECT_EQ(longitude_unit_name, std::string("degree"));

    auto datum = proj_obj_crs_get_horizontal_datum(crs);
    ASSERT_NE(datum, nullptr);
    ObjectKeeper keeper_datum(datum);
    auto pm_from_datum = proj_obj_get_prime_meridian(datum);
    ASSERT_NE(pm_from_datum, nullptr);
    ObjectKeeper keeper_pm_from_datum(pm_from_datum);
}

// ---------------------------------------------------------------------------

TEST_F(CApi, proj_crs_compound) {
    auto crs = proj_obj_create_from_wkt(
        m_ctxt,
        createCompoundCRS()->exportToWKT(WKTFormatter::create().get()).c_str(),
        nullptr);
    ASSERT_NE(crs, nullptr);
    ObjectKeeper keeper(crs);
    EXPECT_EQ(proj_obj_get_type(crs), PJ_OBJ_TYPE_COMPOUND_CRS);

    EXPECT_EQ(proj_obj_crs_get_sub_crs(crs, -1), nullptr);
    EXPECT_EQ(proj_obj_crs_get_sub_crs(crs, 2), nullptr);

    auto subcrs_horiz = proj_obj_crs_get_sub_crs(crs, 0);
    ASSERT_NE(subcrs_horiz, nullptr);
    ObjectKeeper keeper_subcrs_horiz(subcrs_horiz);
    EXPECT_EQ(proj_obj_get_type(subcrs_horiz), PJ_OBJ_TYPE_PROJECTED_CRS);
    EXPECT_EQ(proj_obj_crs_get_sub_crs(subcrs_horiz, 0), nullptr);

    auto subcrs_vertical = proj_obj_crs_get_sub_crs(crs, 1);
    ASSERT_NE(subcrs_vertical, nullptr);
    ObjectKeeper keeper_subcrs_vertical(subcrs_vertical);
    EXPECT_EQ(proj_obj_get_type(subcrs_vertical), PJ_OBJ_TYPE_VERTICAL_CRS);
}

// ---------------------------------------------------------------------------

TEST_F(CApi, proj_obj_get_source_target_crs_bound_crs) {
    auto crs = proj_obj_create_from_wkt(
        m_ctxt,
        createBoundCRS()->exportToWKT(WKTFormatter::create().get()).c_str(),
        nullptr);
    ASSERT_NE(crs, nullptr);
    ObjectKeeper keeper(crs);

    auto sourceCRS = proj_obj_get_source_crs(crs);
    ASSERT_NE(sourceCRS, nullptr);
    ObjectKeeper keeper_sourceCRS(sourceCRS);
    EXPECT_EQ(std::string(proj_obj_get_name(sourceCRS)), "NTF (Paris)");

    auto targetCRS = proj_obj_get_target_crs(crs);
    ASSERT_NE(targetCRS, nullptr);
    ObjectKeeper keeper_targetCRS(targetCRS);
    EXPECT_EQ(std::string(proj_obj_get_name(targetCRS)), "WGS 84");
}

// ---------------------------------------------------------------------------

TEST_F(CApi, proj_obj_get_source_target_crs_transformation) {
    auto obj = proj_obj_create_from_wkt(
        m_ctxt, createBoundCRS()
                    ->transformation()
                    ->exportToWKT(WKTFormatter::create().get())
                    .c_str(),
        nullptr);
    ASSERT_NE(obj, nullptr);
    ObjectKeeper keeper(obj);

    auto sourceCRS = proj_obj_get_source_crs(obj);
    ASSERT_NE(sourceCRS, nullptr);
    ObjectKeeper keeper_sourceCRS(sourceCRS);
    EXPECT_EQ(std::string(proj_obj_get_name(sourceCRS)), "NTF (Paris)");

    auto targetCRS = proj_obj_get_target_crs(obj);
    ASSERT_NE(targetCRS, nullptr);
    ObjectKeeper keeper_targetCRS(targetCRS);
    EXPECT_EQ(std::string(proj_obj_get_name(targetCRS)), "WGS 84");
}

// ---------------------------------------------------------------------------

TEST_F(CApi, proj_obj_get_source_target_crs_conversion_without_crs) {
    auto obj = proj_obj_create_from_database(
        m_ctxt, "EPSG", "16031", PJ_OBJ_CATEGORY_COORDINATE_OPERATION, false,
        nullptr);
    ASSERT_NE(obj, nullptr);
    ObjectKeeper keeper(obj);

    auto sourceCRS = proj_obj_get_source_crs(obj);
    ASSERT_EQ(sourceCRS, nullptr);

    auto targetCRS = proj_obj_get_target_crs(obj);
    ASSERT_EQ(targetCRS, nullptr);
}

// ---------------------------------------------------------------------------

TEST_F(CApi, proj_obj_get_source_target_crs_invalid_object) {
    auto obj = proj_obj_create_from_wkt(
        m_ctxt, "ELLIPSOID[\"WGS 84\",6378137,298.257223563]", nullptr);
    ASSERT_NE(obj, nullptr);
    ObjectKeeper keeper(obj);

    auto sourceCRS = proj_obj_get_source_crs(obj);
    ASSERT_EQ(sourceCRS, nullptr);

    auto targetCRS = proj_obj_get_target_crs(obj);
    ASSERT_EQ(targetCRS, nullptr);
}

// ---------------------------------------------------------------------------

struct ListFreer {
    PROJ_STRING_LIST list;
    ListFreer(PROJ_STRING_LIST ptrIn) : list(ptrIn) {}
    ~ListFreer() { proj_free_string_list(list); }
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
        std::vector<PJ_OBJ_TYPE>{PJ_OBJ_TYPE_ELLIPSOID,

                                 PJ_OBJ_TYPE_GEODETIC_REFERENCE_FRAME,
                                 PJ_OBJ_TYPE_DYNAMIC_GEODETIC_REFERENCE_FRAME,
                                 PJ_OBJ_TYPE_VERTICAL_REFERENCE_FRAME,
                                 PJ_OBJ_TYPE_DYNAMIC_VERTICAL_REFERENCE_FRAME,
                                 PJ_OBJ_TYPE_DATUM_ENSEMBLE,

                                 PJ_OBJ_TYPE_CRS,
                                 PJ_OBJ_TYPE_GEODETIC_CRS,
                                 PJ_OBJ_TYPE_GEOCENTRIC_CRS,
                                 PJ_OBJ_TYPE_GEOGRAPHIC_CRS,
                                 PJ_OBJ_TYPE_GEOGRAPHIC_2D_CRS,
                                 PJ_OBJ_TYPE_GEOGRAPHIC_3D_CRS,
                                 PJ_OBJ_TYPE_VERTICAL_CRS,
                                 PJ_OBJ_TYPE_PROJECTED_CRS,
                                 PJ_OBJ_TYPE_COMPOUND_CRS,
                                 PJ_OBJ_TYPE_TEMPORAL_CRS,
                                 PJ_OBJ_TYPE_BOUND_CRS,
                                 PJ_OBJ_TYPE_OTHER_CRS,

                                 PJ_OBJ_TYPE_CONVERSION,
                                 PJ_OBJ_TYPE_TRANSFORMATION,
                                 PJ_OBJ_TYPE_CONCATENATED_OPERATION,
                                 PJ_OBJ_TYPE_OTHER_COORDINATE_OPERATION,

                                 PJ_OBJ_TYPE_UNKNOWN};
    for (const auto &type : listTypes) {
        auto list = proj_get_codes_from_database(m_ctxt, "EPSG", type, true);
        ListFreer feer(list);
        if (type == PJ_OBJ_TYPE_TEMPORAL_CRS || type == PJ_OBJ_TYPE_BOUND_CRS ||
            type == PJ_OBJ_TYPE_UNKNOWN) {
            EXPECT_EQ(list, nullptr) << type;
        } else {
            ASSERT_NE(list, nullptr) << type;
            ASSERT_NE(list[0], nullptr) << type;
        }
    }
}

// ---------------------------------------------------------------------------

TEST_F(CApi, conversion) {
    auto crs = proj_obj_create_from_wkt(
        m_ctxt,
        createProjectedCRS()->exportToWKT(WKTFormatter::create().get()).c_str(),
        nullptr);
    ASSERT_NE(crs, nullptr);
    ObjectKeeper keeper(crs);

    {
        auto conv =
            proj_obj_crs_get_coordoperation(crs, nullptr, nullptr, nullptr);
        ASSERT_NE(conv, nullptr);
        ObjectKeeper keeper_conv(conv);

        ASSERT_EQ(
            proj_obj_crs_get_coordoperation(conv, nullptr, nullptr, nullptr),
            nullptr);
    }

    const char *methodName = nullptr;
    const char *methodAuthorityName = nullptr;
    const char *methodCode = nullptr;
    auto conv = proj_obj_crs_get_coordoperation(
        crs, &methodName, &methodAuthorityName, &methodCode);
    ASSERT_NE(conv, nullptr);
    ObjectKeeper keeper_conv(conv);

    ASSERT_NE(methodName, nullptr);
    ASSERT_NE(methodAuthorityName, nullptr);
    ASSERT_NE(methodCode, nullptr);
    EXPECT_EQ(methodName, std::string("Transverse Mercator"));
    EXPECT_EQ(methodAuthorityName, std::string("EPSG"));
    EXPECT_EQ(methodCode, std::string("9807"));

    EXPECT_EQ(proj_coordoperation_get_param_count(conv), 5);
    EXPECT_EQ(proj_coordoperation_get_param_index(conv, "foo"), -1);
    EXPECT_EQ(proj_coordoperation_get_param_index(conv, "False easting"), 3);

    EXPECT_FALSE(proj_coordoperation_get_param(conv, -1, nullptr, nullptr,
                                               nullptr, nullptr, nullptr,
                                               nullptr, nullptr));
    EXPECT_FALSE(proj_coordoperation_get_param(conv, 5, nullptr, nullptr,
                                               nullptr, nullptr, nullptr,
                                               nullptr, nullptr));

    const char *name = nullptr;
    const char *nameAuthorityName = nullptr;
    const char *nameCode = nullptr;
    double value = 0;
    const char *valueString = nullptr;
    double valueUnitConvFactor = 0;
    const char *valueUnitName = nullptr;
    EXPECT_TRUE(proj_coordoperation_get_param(
        conv, 3, &name, &nameAuthorityName, &nameCode, &value, &valueString,
        &valueUnitConvFactor, &valueUnitName));
    ASSERT_NE(name, nullptr);
    ASSERT_NE(nameAuthorityName, nullptr);
    ASSERT_NE(nameCode, nullptr);
    EXPECT_EQ(valueString, nullptr);
    ASSERT_NE(valueUnitName, nullptr);
    EXPECT_EQ(name, std::string("False easting"));
    EXPECT_EQ(nameAuthorityName, std::string("EPSG"));
    EXPECT_EQ(nameCode, std::string("8806"));
    EXPECT_EQ(value, 500000.0);
    EXPECT_EQ(valueUnitConvFactor, 1.0);
    EXPECT_EQ(valueUnitName, std::string("metre"));
}

// ---------------------------------------------------------------------------

TEST_F(CApi, transformation_from_boundCRS) {
    auto crs = proj_obj_create_from_wkt(
        m_ctxt,
        createBoundCRS()->exportToWKT(WKTFormatter::create().get()).c_str(),
        nullptr);
    ASSERT_NE(crs, nullptr);
    ObjectKeeper keeper(crs);

    auto transf =
        proj_obj_crs_get_coordoperation(crs, nullptr, nullptr, nullptr);
    ASSERT_NE(transf, nullptr);
    ObjectKeeper keeper_transf(transf);
}

// ---------------------------------------------------------------------------

TEST_F(CApi, proj_coordoperation_get_grid_used) {
    auto op = proj_obj_create_from_database(
        m_ctxt, "EPSG", "1312", PJ_OBJ_CATEGORY_COORDINATE_OPERATION, true,
        nullptr);
    ASSERT_NE(op, nullptr);
    ObjectKeeper keeper(op);

    EXPECT_EQ(proj_coordoperation_get_grid_used_count(op), 1);
    const char *shortName = nullptr;
    const char *fullName = nullptr;
    const char *packageName = nullptr;
    const char *url = nullptr;
    int directDownload = 0;
    int openLicense = 0;
    int available = 0;
    EXPECT_EQ(proj_coordoperation_get_grid_used(op, -1, nullptr, nullptr,
                                                nullptr, nullptr, nullptr,
                                                nullptr, nullptr),
              0);
    EXPECT_EQ(proj_coordoperation_get_grid_used(op, 1, nullptr, nullptr,
                                                nullptr, nullptr, nullptr,
                                                nullptr, nullptr),
              0);
    EXPECT_EQ(proj_coordoperation_get_grid_used(
                  op, 0, &shortName, &fullName, &packageName, &url,
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

TEST_F(CApi, proj_coordoperation_is_instanciable) {
    auto op = proj_obj_create_from_database(
        m_ctxt, "EPSG", "1671", PJ_OBJ_CATEGORY_COORDINATE_OPERATION, true,
        nullptr);
    ASSERT_NE(op, nullptr);
    ObjectKeeper keeper(op);
    EXPECT_EQ(proj_coordoperation_is_instanciable(op), 1);
}

// ---------------------------------------------------------------------------

TEST_F(CApi, proj_obj_create_operations) {
    auto ctxt = proj_create_operation_factory_context(m_ctxt, nullptr);
    ASSERT_NE(ctxt, nullptr);
    ContextKeeper keeper_ctxt(ctxt);

    auto source_crs = proj_obj_create_from_database(
        m_ctxt, "EPSG", "4267", PJ_OBJ_CATEGORY_CRS, false, nullptr); // NAD27
    ASSERT_NE(source_crs, nullptr);
    ObjectKeeper keeper_source_crs(source_crs);

    auto target_crs = proj_obj_create_from_database(
        m_ctxt, "EPSG", "4269", PJ_OBJ_CATEGORY_CRS, false, nullptr); // NAD83
    ASSERT_NE(target_crs, nullptr);
    ObjectKeeper keeper_target_crs(target_crs);

    proj_operation_factory_context_set_spatial_criterion(
        ctxt, PROJ_SPATIAL_CRITERION_PARTIAL_INTERSECTION);

    proj_operation_factory_context_set_grid_availability_use(
        ctxt, PROJ_GRID_AVAILABILITY_IGNORED);

    auto res = proj_obj_create_operations(source_crs, target_crs, ctxt);
    ASSERT_NE(res, nullptr);
    ObjListKeeper keeper_res(res);

    EXPECT_EQ(proj_obj_list_get_count(res), 7);

    EXPECT_EQ(proj_obj_list_get(res, -1), nullptr);
    EXPECT_EQ(proj_obj_list_get(res, proj_obj_list_get_count(res)), nullptr);
    auto op = proj_obj_list_get(res, 0);
    ASSERT_NE(op, nullptr);
    ObjectKeeper keeper_op(op);

    EXPECT_EQ(proj_obj_get_name(op), std::string("NAD27 to NAD83 (3)"));
}

// ---------------------------------------------------------------------------

TEST_F(CApi, proj_obj_create_operations_with_pivot) {

    auto source_crs = proj_obj_create_from_database(
        m_ctxt, "EPSG", "4326", PJ_OBJ_CATEGORY_CRS, false, nullptr); // WGS84
    ASSERT_NE(source_crs, nullptr);
    ObjectKeeper keeper_source_crs(source_crs);

    auto target_crs = proj_obj_create_from_database(
        m_ctxt, "EPSG", "6668", PJ_OBJ_CATEGORY_CRS, false, nullptr); // JGD2011
    ASSERT_NE(target_crs, nullptr);
    ObjectKeeper keeper_target_crs(target_crs);

    // There is no direct transformations between both

    // Default behaviour: allow any pivot
    {
        auto ctxt = proj_create_operation_factory_context(m_ctxt, nullptr);
        ASSERT_NE(ctxt, nullptr);
        ContextKeeper keeper_ctxt(ctxt);

        auto res = proj_obj_create_operations(source_crs, target_crs, ctxt);
        ASSERT_NE(res, nullptr);
        ObjListKeeper keeper_res(res);
        EXPECT_EQ(proj_obj_list_get_count(res), 1);
        auto op = proj_obj_list_get(res, 0);
        ASSERT_NE(op, nullptr);
        ObjectKeeper keeper_op(op);

        EXPECT_EQ(
            proj_obj_get_name(op),
            std::string(
                "Inverse of JGD2000 to WGS 84 (1) + JGD2000 to JGD2011 (2)"));
    }

    // Disallow pivots
    {
        auto ctxt = proj_create_operation_factory_context(m_ctxt, nullptr);
        ASSERT_NE(ctxt, nullptr);
        ContextKeeper keeper_ctxt(ctxt);
        proj_operation_factory_context_set_allow_use_intermediate_crs(ctxt,
                                                                      false);

        auto res = proj_obj_create_operations(source_crs, target_crs, ctxt);
        ASSERT_NE(res, nullptr);
        ObjListKeeper keeper_res(res);
        EXPECT_EQ(proj_obj_list_get_count(res), 1);
        auto op = proj_obj_list_get(res, 0);
        ASSERT_NE(op, nullptr);
        ObjectKeeper keeper_op(op);

        EXPECT_EQ(proj_obj_get_name(op),
                  std::string("Null geographic offset from WGS 84 to JGD2011"));
    }

    // Restrict pivot to Tokyo CRS
    {
        auto ctxt = proj_create_operation_factory_context(m_ctxt, "EPSG");
        ASSERT_NE(ctxt, nullptr);
        ContextKeeper keeper_ctxt(ctxt);

        const char *pivots[] = {"EPSG", "4301", nullptr};
        proj_operation_factory_context_set_allowed_intermediate_crs(ctxt,
                                                                    pivots);
        proj_operation_factory_context_set_spatial_criterion(
            ctxt, PROJ_SPATIAL_CRITERION_PARTIAL_INTERSECTION);
        proj_operation_factory_context_set_grid_availability_use(
            ctxt, PROJ_GRID_AVAILABILITY_IGNORED);

        auto res = proj_obj_create_operations(source_crs, target_crs, ctxt);
        ASSERT_NE(res, nullptr);
        ObjListKeeper keeper_res(res);
        EXPECT_EQ(proj_obj_list_get_count(res), 6);
        auto op = proj_obj_list_get(res, 0);
        ASSERT_NE(op, nullptr);
        ObjectKeeper keeper_op(op);

        EXPECT_EQ(
            proj_obj_get_name(op),
            std::string(
                "Inverse of Tokyo to WGS 84 (108) + Tokyo to JGD2011 (2)"));
    }

    // Restrict pivot to JGD2000
    {
        auto ctxt = proj_create_operation_factory_context(m_ctxt, nullptr);
        ASSERT_NE(ctxt, nullptr);
        ContextKeeper keeper_ctxt(ctxt);

        const char *pivots[] = {"EPSG", "4612", nullptr};
        proj_operation_factory_context_set_allowed_intermediate_crs(ctxt,
                                                                    pivots);
        proj_operation_factory_context_set_spatial_criterion(
            ctxt, PROJ_SPATIAL_CRITERION_PARTIAL_INTERSECTION);
        proj_operation_factory_context_set_grid_availability_use(
            ctxt, PROJ_GRID_AVAILABILITY_IGNORED);

        auto res = proj_obj_create_operations(source_crs, target_crs, ctxt);
        ASSERT_NE(res, nullptr);
        ObjListKeeper keeper_res(res);
        // includes 2 results from ESRI
        EXPECT_EQ(proj_obj_list_get_count(res), 4);
        auto op = proj_obj_list_get(res, 0);
        ASSERT_NE(op, nullptr);
        ObjectKeeper keeper_op(op);

        EXPECT_EQ(
            proj_obj_get_name(op),
            std::string(
                "Inverse of JGD2000 to WGS 84 (1) + JGD2000 to JGD2011 (2)"));
    }
}

// ---------------------------------------------------------------------------

TEST_F(CApi, proj_context_set_database_path_null) {

    EXPECT_TRUE(
        proj_context_set_database_path(m_ctxt, nullptr, nullptr, nullptr));
    auto source_crs = proj_obj_create_from_database(m_ctxt, "EPSG", "4326",
                                                    PJ_OBJ_CATEGORY_CRS, false,
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

    auto source_crs = proj_obj_create_from_database(m_ctxt, "EPSG", "4326",
                                                    PJ_OBJ_CATEGORY_CRS, false,
                                                    nullptr); // WGS84
    ASSERT_NE(source_crs, nullptr);
    ObjectKeeper keeper_source_crs(source_crs);
}

// ---------------------------------------------------------------------------

TEST_F(CApi, proj_context_set_database_path_error_1) {

    EXPECT_FALSE(proj_context_set_database_path(m_ctxt, "i_do_not_exist.db",
                                                nullptr, nullptr));

    // We will eventually re-open on the default DB
    auto source_crs = proj_obj_create_from_database(m_ctxt, "EPSG", "4326",
                                                    PJ_OBJ_CATEGORY_CRS, false,
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
    auto source_crs = proj_obj_create_from_database(m_ctxt, "EPSG", "4326",
                                                    PJ_OBJ_CATEGORY_CRS, false,
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

TEST_F(CApi, proj_obj_create_from_name) {
    /*
        PJ_OBJ_LIST PROJ_DLL *proj_obj_create_from_name(
            PJ_CONTEXT *ctx,
            const char *auth_name,
            const char *searchedName,
            const PJ_OBJ_TYPE* types,
            size_t typesCount,
            int approximateMatch,
            size_t limitResultCount,
            const char* const *options); */
    {
        auto res = proj_obj_create_from_name(m_ctxt, nullptr, "WGS 84", nullptr,
                                             0, false, 0, nullptr);
        ASSERT_NE(res, nullptr);
        ObjListKeeper keeper_res(res);
        EXPECT_EQ(proj_obj_list_get_count(res), 4);
    }
    {
        auto res = proj_obj_create_from_name(m_ctxt, "xx", "WGS 84", nullptr, 0,
                                             false, 0, nullptr);
        ASSERT_NE(res, nullptr);
        ObjListKeeper keeper_res(res);
        EXPECT_EQ(proj_obj_list_get_count(res), 0);
    }
    {
        const PJ_OBJ_TYPE types[] = {PJ_OBJ_TYPE_GEODETIC_CRS,
                                     PJ_OBJ_TYPE_PROJECTED_CRS};
        auto res = proj_obj_create_from_name(m_ctxt, nullptr, "WGS 84", types,
                                             2, true, 10, nullptr);
        ASSERT_NE(res, nullptr);
        ObjListKeeper keeper_res(res);
        EXPECT_EQ(proj_obj_list_get_count(res), 10);
    }
}

// ---------------------------------------------------------------------------

TEST_F(CApi, proj_obj_identify) {
    auto obj = proj_obj_create_from_wkt(
        m_ctxt,
        GeographicCRS::EPSG_4807->exportToWKT(WKTFormatter::create().get())
            .c_str(),
        nullptr);
    ObjectKeeper keeper(obj);
    ASSERT_NE(obj, nullptr);
    {
        auto res = proj_obj_identify(obj, nullptr, nullptr, nullptr);
        ObjListKeeper keeper_res(res);
        EXPECT_EQ(proj_obj_list_get_count(res), 1);
    }
    {
        int *confidence = nullptr;
        auto res = proj_obj_identify(obj, "EPSG", nullptr, &confidence);
        ObjListKeeper keeper_res(res);
        EXPECT_EQ(proj_obj_list_get_count(res), 1);
        EXPECT_EQ(confidence[0], 100);
        proj_free_int_list(confidence);
    }
    {
        auto objEllps = proj_obj_create_from_wkt(
            m_ctxt,
            Ellipsoid::GRS1980->exportToWKT(WKTFormatter::create().get())
                .c_str(),
            nullptr);
        ObjectKeeper keeperEllps(objEllps);
        auto res = proj_obj_identify(objEllps, nullptr, nullptr, nullptr);
        ObjListKeeper keeper_res(res);
        EXPECT_EQ(res, nullptr);
    }
}

// ---------------------------------------------------------------------------

TEST_F(CApi, proj_obj_get_area_of_use) {
    {
        auto crs = proj_obj_create_from_database(
            m_ctxt, "EPSG", "4326", PJ_OBJ_CATEGORY_CRS, false, nullptr);
        ASSERT_NE(crs, nullptr);
        ObjectKeeper keeper(crs);
        EXPECT_TRUE(proj_obj_get_area_of_use(crs, nullptr, nullptr, nullptr,
                                             nullptr, nullptr));
        const char *name = nullptr;
        double w;
        double s;
        double e;
        double n;
        EXPECT_TRUE(proj_obj_get_area_of_use(crs, &w, &s, &e, &n, &name));
        EXPECT_EQ(w, -180);
        EXPECT_EQ(s, -90);
        EXPECT_EQ(e, 180);
        EXPECT_EQ(n, 90);
        ASSERT_TRUE(name != nullptr);
        EXPECT_EQ(std::string(name), "World");
    }
    {
        auto obj =
            proj_obj_create_from_user_input(m_ctxt, "+proj=longlat", nullptr);
        ObjectKeeper keeper(obj);
        ASSERT_NE(obj, nullptr);
        EXPECT_FALSE(proj_obj_get_area_of_use(obj, nullptr, nullptr, nullptr,
                                              nullptr, nullptr));
    }
}

// ---------------------------------------------------------------------------

TEST_F(CApi, proj_coordoperation_get_accuracy) {
    {
        auto crs = proj_obj_create_from_database(
            m_ctxt, "EPSG", "4326", PJ_OBJ_CATEGORY_CRS, false, nullptr);
        ASSERT_NE(crs, nullptr);
        ObjectKeeper keeper(crs);
        EXPECT_EQ(proj_coordoperation_get_accuracy(crs), -1.0);
    }
    {
        auto obj = proj_obj_create_from_database(
            m_ctxt, "EPSG", "1170", PJ_OBJ_CATEGORY_COORDINATE_OPERATION, false,
            nullptr);
        ASSERT_NE(obj, nullptr);
        ObjectKeeper keeper(obj);
        EXPECT_EQ(proj_coordoperation_get_accuracy(obj), 16.0);
    }
    {
        auto obj =
            proj_obj_create_from_user_input(m_ctxt, "+proj=helmert", nullptr);
        ObjectKeeper keeper(obj);
        ASSERT_NE(obj, nullptr);
        EXPECT_EQ(proj_coordoperation_get_accuracy(obj), -1.0);
    }
}

// ---------------------------------------------------------------------------

TEST_F(CApi, proj_obj_create_geographic_crs) {
    {
        auto obj = proj_obj_create_geographic_crs(
            m_ctxt, "WGS 84", "World Geodetic System 1984", "WGS 84", 6378137,
            298.257223563, "Greenwich", 0.0, "Degree", 0.0174532925199433,
            true);
        ObjectKeeper keeper(obj);
        ASSERT_NE(obj, nullptr);

        auto objRef = proj_obj_create_from_user_input(
            m_ctxt,
            GeographicCRS::EPSG_4326->exportToWKT(WKTFormatter::create().get())
                .c_str(),
            nullptr);
        ObjectKeeper keeperobjRef(objRef);
        EXPECT_NE(objRef, nullptr);

        EXPECT_TRUE(proj_obj_is_equivalent_to(obj, objRef, PJ_COMP_EQUIVALENT));
    }
    {
        auto obj = proj_obj_create_geographic_crs(m_ctxt, nullptr, nullptr,
                                                  nullptr, 1.0, 0.0, nullptr,
                                                  0.0, nullptr, 0.0, false);
        ObjectKeeper keeper(obj);
        ASSERT_NE(obj, nullptr);
    }
}

// ---------------------------------------------------------------------------

TEST_F(CApi, proj_obj_create_projections) {

    auto geogCRS = proj_obj_create_geographic_crs(
        m_ctxt, "WGS 84", "World Geodetic System 1984", "WGS 84", 6378137,
        298.257223563, "Greenwich", 0.0, "Degree", 0.0174532925199433, true);
    ObjectKeeper keepergeogCRS(geogCRS);
    ASSERT_NE(geogCRS, nullptr);

    /* BEGIN: Generated by scripts/create_c_api_projections.py*/
    {
        auto projCRS =
            proj_obj_create_projected_crs_UTM(geogCRS, nullptr, 0, 0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS = proj_obj_create_projected_crs_TransverseMercator(
            geogCRS, nullptr, 0, 0, 0, 0, 0, "Degree", 0.0174532925199433,
            "Metre", 1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS =
            proj_obj_create_projected_crs_GaussSchreiberTransverseMercator(
                geogCRS, nullptr, 0, 0, 0, 0, 0, "Degree", 0.0174532925199433,
                "Metre", 1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS =
            proj_obj_create_projected_crs_TransverseMercatorSouthOriented(
                geogCRS, nullptr, 0, 0, 0, 0, 0, "Degree", 0.0174532925199433,
                "Metre", 1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS = proj_obj_create_projected_crs_TwoPointEquidistant(
            geogCRS, nullptr, 0, 0, 0, 0, 0, 0, "Degree", 0.0174532925199433,
            "Metre", 1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS = proj_obj_create_projected_crs_TunisiaMappingGrid(
            geogCRS, nullptr, 0, 0, 0, 0, "Degree", 0.0174532925199433, "Metre",
            1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS = proj_obj_create_projected_crs_AlbersEqualArea(
            geogCRS, nullptr, 0, 0, 0, 0, 0, 0, "Degree", 0.0174532925199433,
            "Metre", 1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS = proj_obj_create_projected_crs_LambertConicConformal_1SP(
            geogCRS, nullptr, 0, 0, 0, 0, 0, "Degree", 0.0174532925199433,
            "Metre", 1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS = proj_obj_create_projected_crs_LambertConicConformal_2SP(
            geogCRS, nullptr, 0, 0, 0, 0, 0, 0, "Degree", 0.0174532925199433,
            "Metre", 1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS =
            proj_obj_create_projected_crs_LambertConicConformal_2SP_Michigan(
                geogCRS, nullptr, 0, 0, 0, 0, 0, 0, 0, "Degree",
                0.0174532925199433, "Metre", 1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS =
            proj_obj_create_projected_crs_LambertConicConformal_2SP_Belgium(
                geogCRS, nullptr, 0, 0, 0, 0, 0, 0, "Degree",
                0.0174532925199433, "Metre", 1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS = proj_obj_create_projected_crs_AzimuthalEquidistant(
            geogCRS, nullptr, 0, 0, 0, 0, "Degree", 0.0174532925199433, "Metre",
            1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS = proj_obj_create_projected_crs_GuamProjection(
            geogCRS, nullptr, 0, 0, 0, 0, "Degree", 0.0174532925199433, "Metre",
            1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS = proj_obj_create_projected_crs_Bonne(
            geogCRS, nullptr, 0, 0, 0, 0, "Degree", 0.0174532925199433, "Metre",
            1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS =
            proj_obj_create_projected_crs_LambertCylindricalEqualAreaSpherical(
                geogCRS, nullptr, 0, 0, 0, 0, "Degree", 0.0174532925199433,
                "Metre", 1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS =
            proj_obj_create_projected_crs_LambertCylindricalEqualArea(
                geogCRS, nullptr, 0, 0, 0, 0, "Degree", 0.0174532925199433,
                "Metre", 1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS = proj_obj_create_projected_crs_CassiniSoldner(
            geogCRS, nullptr, 0, 0, 0, 0, "Degree", 0.0174532925199433, "Metre",
            1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS = proj_obj_create_projected_crs_EquidistantConic(
            geogCRS, nullptr, 0, 0, 0, 0, 0, 0, "Degree", 0.0174532925199433,
            "Metre", 1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS = proj_obj_create_projected_crs_EckertI(
            geogCRS, nullptr, 0, 0, 0, "Degree", 0.0174532925199433, "Metre",
            1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS = proj_obj_create_projected_crs_EckertII(
            geogCRS, nullptr, 0, 0, 0, "Degree", 0.0174532925199433, "Metre",
            1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS = proj_obj_create_projected_crs_EckertIII(
            geogCRS, nullptr, 0, 0, 0, "Degree", 0.0174532925199433, "Metre",
            1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS = proj_obj_create_projected_crs_EckertIV(
            geogCRS, nullptr, 0, 0, 0, "Degree", 0.0174532925199433, "Metre",
            1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS = proj_obj_create_projected_crs_EckertV(
            geogCRS, nullptr, 0, 0, 0, "Degree", 0.0174532925199433, "Metre",
            1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS = proj_obj_create_projected_crs_EckertVI(
            geogCRS, nullptr, 0, 0, 0, "Degree", 0.0174532925199433, "Metre",
            1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS = proj_obj_create_projected_crs_EquidistantCylindrical(
            geogCRS, nullptr, 0, 0, 0, 0, "Degree", 0.0174532925199433, "Metre",
            1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS =
            proj_obj_create_projected_crs_EquidistantCylindricalSpherical(
                geogCRS, nullptr, 0, 0, 0, 0, "Degree", 0.0174532925199433,
                "Metre", 1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS = proj_obj_create_projected_crs_Gall(
            geogCRS, nullptr, 0, 0, 0, "Degree", 0.0174532925199433, "Metre",
            1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS = proj_obj_create_projected_crs_GoodeHomolosine(
            geogCRS, nullptr, 0, 0, 0, "Degree", 0.0174532925199433, "Metre",
            1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS = proj_obj_create_projected_crs_InterruptedGoodeHomolosine(
            geogCRS, nullptr, 0, 0, 0, "Degree", 0.0174532925199433, "Metre",
            1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS =
            proj_obj_create_projected_crs_GeostationarySatelliteSweepX(
                geogCRS, nullptr, 0, 0, 0, 0, "Degree", 0.0174532925199433,
                "Metre", 1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS =
            proj_obj_create_projected_crs_GeostationarySatelliteSweepY(
                geogCRS, nullptr, 0, 0, 0, 0, "Degree", 0.0174532925199433,
                "Metre", 1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS = proj_obj_create_projected_crs_Gnomonic(
            geogCRS, nullptr, 0, 0, 0, 0, "Degree", 0.0174532925199433, "Metre",
            1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS =
            proj_obj_create_projected_crs_HotineObliqueMercatorVariantA(
                geogCRS, nullptr, 0, 0, 0, 0, 0, 0, 0, "Degree",
                0.0174532925199433, "Metre", 1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS =
            proj_obj_create_projected_crs_HotineObliqueMercatorVariantB(
                geogCRS, nullptr, 0, 0, 0, 0, 0, 0, 0, "Degree",
                0.0174532925199433, "Metre", 1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS =
            proj_obj_create_projected_crs_HotineObliqueMercatorTwoPointNaturalOrigin(
                geogCRS, nullptr, 0, 0, 0, 0, 0, 0, 0, 0, "Degree",
                0.0174532925199433, "Metre", 1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS =
            proj_obj_create_projected_crs_InternationalMapWorldPolyconic(
                geogCRS, nullptr, 0, 0, 0, 0, 0, "Degree", 0.0174532925199433,
                "Metre", 1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS = proj_obj_create_projected_crs_KrovakNorthOriented(
            geogCRS, nullptr, 0, 0, 0, 0, 0, 0, 0, "Degree", 0.0174532925199433,
            "Metre", 1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS = proj_obj_create_projected_crs_Krovak(
            geogCRS, nullptr, 0, 0, 0, 0, 0, 0, 0, "Degree", 0.0174532925199433,
            "Metre", 1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS = proj_obj_create_projected_crs_LambertAzimuthalEqualArea(
            geogCRS, nullptr, 0, 0, 0, 0, "Degree", 0.0174532925199433, "Metre",
            1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS = proj_obj_create_projected_crs_MillerCylindrical(
            geogCRS, nullptr, 0, 0, 0, "Degree", 0.0174532925199433, "Metre",
            1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS = proj_obj_create_projected_crs_MercatorVariantA(
            geogCRS, nullptr, 0, 0, 0, 0, 0, "Degree", 0.0174532925199433,
            "Metre", 1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS = proj_obj_create_projected_crs_MercatorVariantB(
            geogCRS, nullptr, 0, 0, 0, 0, "Degree", 0.0174532925199433, "Metre",
            1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS =
            proj_obj_create_projected_crs_PopularVisualisationPseudoMercator(
                geogCRS, nullptr, 0, 0, 0, 0, "Degree", 0.0174532925199433,
                "Metre", 1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS = proj_obj_create_projected_crs_Mollweide(
            geogCRS, nullptr, 0, 0, 0, "Degree", 0.0174532925199433, "Metre",
            1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS = proj_obj_create_projected_crs_NewZealandMappingGrid(
            geogCRS, nullptr, 0, 0, 0, 0, "Degree", 0.0174532925199433, "Metre",
            1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS = proj_obj_create_projected_crs_ObliqueStereographic(
            geogCRS, nullptr, 0, 0, 0, 0, 0, "Degree", 0.0174532925199433,
            "Metre", 1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS = proj_obj_create_projected_crs_Orthographic(
            geogCRS, nullptr, 0, 0, 0, 0, "Degree", 0.0174532925199433, "Metre",
            1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS = proj_obj_create_projected_crs_AmericanPolyconic(
            geogCRS, nullptr, 0, 0, 0, 0, "Degree", 0.0174532925199433, "Metre",
            1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS = proj_obj_create_projected_crs_PolarStereographicVariantA(
            geogCRS, nullptr, 0, 0, 0, 0, 0, "Degree", 0.0174532925199433,
            "Metre", 1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS = proj_obj_create_projected_crs_PolarStereographicVariantB(
            geogCRS, nullptr, 0, 0, 0, 0, "Degree", 0.0174532925199433, "Metre",
            1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS = proj_obj_create_projected_crs_Robinson(
            geogCRS, nullptr, 0, 0, 0, "Degree", 0.0174532925199433, "Metre",
            1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS = proj_obj_create_projected_crs_Sinusoidal(
            geogCRS, nullptr, 0, 0, 0, "Degree", 0.0174532925199433, "Metre",
            1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS = proj_obj_create_projected_crs_Stereographic(
            geogCRS, nullptr, 0, 0, 0, 0, 0, "Degree", 0.0174532925199433,
            "Metre", 1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS = proj_obj_create_projected_crs_VanDerGrinten(
            geogCRS, nullptr, 0, 0, 0, "Degree", 0.0174532925199433, "Metre",
            1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS = proj_obj_create_projected_crs_WagnerI(
            geogCRS, nullptr, 0, 0, 0, "Degree", 0.0174532925199433, "Metre",
            1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS = proj_obj_create_projected_crs_WagnerII(
            geogCRS, nullptr, 0, 0, 0, "Degree", 0.0174532925199433, "Metre",
            1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS = proj_obj_create_projected_crs_WagnerIII(
            geogCRS, nullptr, 0, 0, 0, 0, "Degree", 0.0174532925199433, "Metre",
            1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS = proj_obj_create_projected_crs_WagnerIV(
            geogCRS, nullptr, 0, 0, 0, "Degree", 0.0174532925199433, "Metre",
            1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS = proj_obj_create_projected_crs_WagnerV(
            geogCRS, nullptr, 0, 0, 0, "Degree", 0.0174532925199433, "Metre",
            1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS = proj_obj_create_projected_crs_WagnerVI(
            geogCRS, nullptr, 0, 0, 0, "Degree", 0.0174532925199433, "Metre",
            1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS = proj_obj_create_projected_crs_WagnerVII(
            geogCRS, nullptr, 0, 0, 0, "Degree", 0.0174532925199433, "Metre",
            1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS =
            proj_obj_create_projected_crs_QuadrilateralizedSphericalCube(
                geogCRS, nullptr, 0, 0, 0, 0, "Degree", 0.0174532925199433,
                "Metre", 1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS = proj_obj_create_projected_crs_SphericalCrossTrackHeight(
            geogCRS, nullptr, 0, 0, 0, 0, "Degree", 0.0174532925199433, "Metre",
            1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    {
        auto projCRS = proj_obj_create_projected_crs_EqualEarth(
            geogCRS, nullptr, 0, 0, 0, "Degree", 0.0174532925199433, "Metre",
            1.0);
        ObjectKeeper keeper_projCRS(projCRS);
        ASSERT_NE(projCRS, nullptr);
    }
    /* END: Generated by scripts/create_c_api_projections.py*/
}

} // namespace
