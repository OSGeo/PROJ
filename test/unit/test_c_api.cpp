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

    struct OperationResultKeeper {
        PJ_OPERATION_RESULT *m_res = nullptr;
        explicit OperationResultKeeper(PJ_OPERATION_RESULT *res) : m_res(res) {}
        ~OperationResultKeeper() { proj_operation_result_unref(m_res); }

        OperationResultKeeper(const OperationResultKeeper &) = delete;
        OperationResultKeeper &
        operator=(const OperationResultKeeper &) = delete;
    };
};

// ---------------------------------------------------------------------------

TEST_F(CApi, proj_obj_create_from_user_input) {
    proj_obj_unref(nullptr);
    EXPECT_EQ(proj_obj_create_from_user_input(m_ctxt, "invalid"), nullptr);
    {
        auto obj = proj_obj_create_from_user_input(
            m_ctxt,
            GeographicCRS::EPSG_4326->exportToWKT(WKTFormatter::create().get())
                .c_str());
        ObjectKeeper keeper(obj);
        EXPECT_NE(obj, nullptr);
    }
    {
        auto obj = proj_obj_create_from_user_input(m_ctxt, "EPSG:4326");
        ObjectKeeper keeper(obj);
        EXPECT_NE(obj, nullptr);
    }
}

// ---------------------------------------------------------------------------

TEST_F(CApi, proj_obj_create_from_wkt) {
    proj_obj_unref(nullptr);
    EXPECT_EQ(proj_obj_create_from_wkt(m_ctxt, "invalid"), nullptr);
    auto obj = proj_obj_create_from_wkt(
        m_ctxt,
        GeographicCRS::EPSG_4326->exportToWKT(WKTFormatter::create().get())
            .c_str());
    ObjectKeeper keeper(obj);
    EXPECT_NE(obj, nullptr);
}

// ---------------------------------------------------------------------------

TEST_F(CApi, proj_obj_create_from_proj_string) {
    proj_obj_unref(nullptr);
    EXPECT_EQ(proj_obj_create_from_proj_string(m_ctxt, "invalid"), nullptr);
    auto obj = proj_obj_create_from_proj_string(m_ctxt, "+proj=longlat");
    ObjectKeeper keeper(obj);
    EXPECT_NE(obj, nullptr);
}

// ---------------------------------------------------------------------------

TEST_F(CApi, proj_obj_as_wkt) {
    auto obj = proj_obj_create_from_wkt(
        m_ctxt,
        GeographicCRS::EPSG_4326->exportToWKT(WKTFormatter::create().get())
            .c_str());
    ObjectKeeper keeper(obj);
    ASSERT_NE(obj, nullptr);

    auto wkt2_2018 = proj_obj_as_wkt(obj, PJ_WKT2_2018, nullptr);
    ASSERT_NE(wkt2_2018, nullptr);

    auto wkt2_2018_bis = proj_obj_as_wkt(obj, PJ_WKT2_2018, nullptr);
    EXPECT_EQ(wkt2_2018_bis, wkt2_2018);

    auto wkt2_2018_simplified =
        proj_obj_as_wkt(obj, PJ_WKT2_2018_SIMPLIFIED, nullptr);
    ASSERT_NE(wkt2_2018_simplified, nullptr);

    auto wkt2_2015 = proj_obj_as_wkt(obj, PJ_WKT2_2015, nullptr);
    ASSERT_NE(wkt2_2015, nullptr);

    auto wkt2_2015_simplified =
        proj_obj_as_wkt(obj, PJ_WKT2_2015_SIMPLIFIED, nullptr);
    ASSERT_NE(wkt2_2015_simplified, nullptr);

    auto wkt1_GDAL = proj_obj_as_wkt(obj, PJ_WKT1_GDAL, nullptr);
    ASSERT_NE(wkt1_GDAL, nullptr);

    EXPECT_TRUE(std::string(wkt2_2018).find("GEOGCRS[") == 0) << wkt2_2018;

    EXPECT_TRUE(std::string(wkt2_2018_simplified).find("GEOGCRS[") == 0)
        << wkt2_2018_simplified;
    EXPECT_TRUE(std::string(wkt2_2018_simplified).find("ANGULARUNIT[") ==
                std::string::npos)
        << wkt2_2018_simplified;

    EXPECT_TRUE(std::string(wkt2_2015).find("GEODCRS[") == 0) << wkt2_2015;

    EXPECT_TRUE(std::string(wkt2_2015_simplified).find("GEODCRS[") == 0)
        << wkt2_2015_simplified;
    EXPECT_TRUE(std::string(wkt2_2015_simplified).find("ANGULARUNIT[") ==
                std::string::npos)
        << wkt2_2015_simplified;

    EXPECT_TRUE(std::string(wkt1_GDAL).find("GEOGCS[") == 0) << wkt1_GDAL;
}

// ---------------------------------------------------------------------------

TEST_F(CApi, proj_obj_as_wkt_incompatible_WKT1) {
    auto obj = proj_obj_create_from_wkt(
        m_ctxt,
        createBoundCRS()->exportToWKT(WKTFormatter::create().get()).c_str());
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
            .c_str());
    ObjectKeeper keeper(obj);
    ASSERT_NE(obj, nullptr);

    auto proj_5 = proj_obj_as_proj_string(obj, PJ_PROJ_5, nullptr);
    ASSERT_NE(proj_5, nullptr);

    auto proj_5_bis = proj_obj_as_proj_string(obj, PJ_PROJ_5, nullptr);
    EXPECT_EQ(proj_5_bis, proj_5);

    auto proj_4 = proj_obj_as_proj_string(obj, PJ_PROJ_4, nullptr);
    ASSERT_NE(proj_4, nullptr);

    EXPECT_EQ(std::string(proj_5), "+proj=pipeline +step +proj=longlat "
                                   "+ellps=WGS84 +step +proj=unitconvert "
                                   "+xy_in=rad +xy_out=deg +step "
                                   "+proj=axisswap +order=2,1");
    EXPECT_EQ(std::string(proj_4), "+proj=longlat +datum=WGS84");
}

// ---------------------------------------------------------------------------

TEST_F(CApi, proj_obj_as_proj_string_incompatible_WKT1) {
    auto obj = proj_obj_create_from_wkt(
        m_ctxt,
        createBoundCRS()->exportToWKT(WKTFormatter::create().get()).c_str());
    ObjectKeeper keeper(obj);
    ASSERT_NE(obj, nullptr);

    auto str = proj_obj_as_proj_string(obj, PJ_PROJ_5, nullptr);
    ASSERT_EQ(str, nullptr);
}

// ---------------------------------------------------------------------------

TEST_F(CApi, proj_obj_as_proj_string_etmerc_option) {
    auto obj = proj_obj_create_from_proj_string(m_ctxt, "+proj=tmerc");
    ObjectKeeper keeper(obj);
    ASSERT_NE(obj, nullptr);

    const char *options[] = {"USE_ETMERC=YES", nullptr};
    auto str = proj_obj_as_proj_string(obj, PJ_PROJ_4, options);
    ASSERT_NE(str, nullptr);
    EXPECT_EQ(str, std::string("+proj=etmerc +lat_0=0 +lon_0=0 +k_0=1 +x_0=0 "
                               "+y_0=0 +datum=WGS84"));
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
              "+towgs84=2.329,-147.042,-92.08,-0.309,0.325,0.497,5.69");
}

// ---------------------------------------------------------------------------

TEST_F(CApi, proj_obj_crs_create_bound_crs_to_WGS84_on_invalid_type) {
    auto obj = proj_obj_create_from_wkt(
        m_ctxt, createProjectedCRS()
                    ->derivingConversion()
                    ->exportToWKT(WKTFormatter::create().get())
                    .c_str());
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
            .c_str());
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
            .c_str());
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
            .c_str());
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
                .c_str());
        ObjectKeeper keeper(obj);
        ASSERT_NE(obj, nullptr);
        EXPECT_EQ(proj_obj_get_type(obj), PJ_OBJ_TYPE_GEOGRAPHIC_CRS);
    }
    {
        auto obj = proj_obj_create_from_wkt(
            m_ctxt, GeographicCRS::EPSG_4326->datum()
                        ->exportToWKT(WKTFormatter::create().get())
                        .c_str());
        ObjectKeeper keeper(obj);
        ASSERT_NE(obj, nullptr);
        EXPECT_EQ(proj_obj_get_type(obj), PJ_OBJ_TYPE_GEODETIC_REFERENCE_FRAME);
    }
    {
        auto obj = proj_obj_create_from_wkt(
            m_ctxt, GeographicCRS::EPSG_4326->ellipsoid()
                        ->exportToWKT(WKTFormatter::create().get())
                        .c_str());
        ObjectKeeper keeper(obj);
        ASSERT_NE(obj, nullptr);
        EXPECT_EQ(proj_obj_get_type(obj), PJ_OBJ_TYPE_ELLIPSOID);
    }
    {
        auto obj = proj_obj_create_from_wkt(
            m_ctxt, createProjectedCRS()
                        ->exportToWKT(WKTFormatter::create().get())
                        .c_str());
        ObjectKeeper keeper(obj);
        ASSERT_NE(obj, nullptr);
        EXPECT_EQ(proj_obj_get_type(obj), PJ_OBJ_TYPE_PROJECTED_CRS);
    }
    {
        auto obj = proj_obj_create_from_wkt(
            m_ctxt, createVerticalCRS()
                        ->exportToWKT(WKTFormatter::create().get())
                        .c_str());
        ObjectKeeper keeper(obj);
        ASSERT_NE(obj, nullptr);
        EXPECT_EQ(proj_obj_get_type(obj), PJ_OBJ_TYPE_VERTICAL_CRS);
    }
    {
        auto obj = proj_obj_create_from_wkt(
            m_ctxt, createVerticalCRS()
                        ->datum()
                        ->exportToWKT(WKTFormatter::create().get())
                        .c_str());
        ObjectKeeper keeper(obj);
        ASSERT_NE(obj, nullptr);
        EXPECT_EQ(proj_obj_get_type(obj), PJ_OBJ_TYPE_VERTICAL_REFERENCE_FRAME);
    }
    {
        auto obj = proj_obj_create_from_wkt(
            m_ctxt, createProjectedCRS()
                        ->derivingConversion()
                        ->exportToWKT(WKTFormatter::create().get())
                        .c_str());
        ObjectKeeper keeper(obj);
        ASSERT_NE(obj, nullptr);
        EXPECT_EQ(proj_obj_get_type(obj), PJ_OBJ_TYPE_CONVERSION);
    }
    {
        auto obj = proj_obj_create_from_wkt(
            m_ctxt, createBoundCRS()
                        ->exportToWKT(WKTFormatter::create().get())
                        .c_str());
        ObjectKeeper keeper(obj);
        ASSERT_NE(obj, nullptr);
        EXPECT_EQ(proj_obj_get_type(obj), PJ_OBJ_TYPE_BOUND_CRS);
    }
    {
        auto obj = proj_obj_create_from_wkt(
            m_ctxt, createBoundCRS()
                        ->transformation()
                        ->exportToWKT(WKTFormatter::create().get())
                        .c_str());
        ObjectKeeper keeper(obj);
        ASSERT_NE(obj, nullptr);
        EXPECT_EQ(proj_obj_get_type(obj), PJ_OBJ_TYPE_TRANSFORMATION);
    }
    {
        auto obj =
            proj_obj_create_from_wkt(m_ctxt, "AUTHORITY[\"EPSG\", 4326]");
        ObjectKeeper keeper(obj);
        ASSERT_NE(obj, nullptr);
        EXPECT_EQ(proj_obj_get_type(obj), PJ_OBJ_TYPE_UNKNOWN);
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
        EXPECT_EQ(proj_obj_get_type(crs), PJ_OBJ_TYPE_GEOGRAPHIC_CRS);
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
            .c_str());
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
            .c_str());
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
        createCompoundCRS()->exportToWKT(WKTFormatter::create().get()).c_str());
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
        createBoundCRS()->exportToWKT(WKTFormatter::create().get()).c_str());
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
                    .c_str());
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
    auto obj = proj_obj_create_from_wkt(m_ctxt, "AUTHORITY[\"EPSG\", 4326]");
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
    EXPECT_EQ(list[1], std::string("IGNF"));
    ASSERT_TRUE(list[2] != nullptr);
    EXPECT_EQ(list[2], std::string("OGC"));
    ASSERT_TRUE(list[3] != nullptr);
    EXPECT_EQ(list[3], std::string("PROJ"));
    EXPECT_EQ(list[4], nullptr);
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

                                 PJ_OBJ_TYPE_GEODETIC_CRS,
                                 PJ_OBJ_TYPE_GEOGRAPHIC_CRS,
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
        m_ctxt, createProjectedCRS()
                    ->exportToWKT(WKTFormatter::create().get())
                    .c_str());
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
        createBoundCRS()->exportToWKT(WKTFormatter::create().get()).c_str());
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
    auto ctxt = proj_create_operation_factory_context(m_ctxt);
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
    OperationResultKeeper keeper_res(res);

    EXPECT_EQ(proj_operation_result_get_count(res), 6);

    EXPECT_EQ(proj_operation_result_get(res, -1), nullptr);
    EXPECT_EQ(
        proj_operation_result_get(res, proj_operation_result_get_count(res)),
        nullptr);
    auto op = proj_operation_result_get(res, 0);
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
        auto ctxt = proj_create_operation_factory_context(m_ctxt);
        ASSERT_NE(ctxt, nullptr);
        ContextKeeper keeper_ctxt(ctxt);

        auto res = proj_obj_create_operations(source_crs, target_crs, ctxt);
        ASSERT_NE(res, nullptr);
        OperationResultKeeper keeper_res(res);
        EXPECT_EQ(proj_operation_result_get_count(res), 1);
        auto op = proj_operation_result_get(res, 0);
        ASSERT_NE(op, nullptr);
        ObjectKeeper keeper_op(op);

        EXPECT_EQ(
            proj_obj_get_name(op),
            std::string(
                "Inverse of JGD2000 to WGS 84 (1) + JGD2000 to JGD2011 (2)"));
    }

    // Disallow pivots
    {
        auto ctxt = proj_create_operation_factory_context(m_ctxt);
        ASSERT_NE(ctxt, nullptr);
        ContextKeeper keeper_ctxt(ctxt);
        proj_operation_factory_context_set_allow_use_intermediate_crs(ctxt,
                                                                      false);

        auto res = proj_obj_create_operations(source_crs, target_crs, ctxt);
        ASSERT_NE(res, nullptr);
        OperationResultKeeper keeper_res(res);
        EXPECT_EQ(proj_operation_result_get_count(res), 1);
        auto op = proj_operation_result_get(res, 0);
        ASSERT_NE(op, nullptr);
        ObjectKeeper keeper_op(op);

        EXPECT_EQ(proj_obj_get_name(op),
                  std::string("Null geographic offset transformation from WGS "
                              "84 to JGD2011"));
    }

    // Restrict pivot to Tokyo CRS
    {
        auto ctxt = proj_create_operation_factory_context(m_ctxt);
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
        OperationResultKeeper keeper_res(res);
        EXPECT_EQ(proj_operation_result_get_count(res), 6);
        auto op = proj_operation_result_get(res, 0);
        ASSERT_NE(op, nullptr);
        ObjectKeeper keeper_op(op);

        EXPECT_EQ(
            proj_obj_get_name(op),
            std::string(
                "Inverse of Tokyo to WGS 84 (108) + Tokyo to JGD2011 (2)"));
    }

    // Restrict pivot to JGD2000
    {
        auto ctxt = proj_create_operation_factory_context(m_ctxt);
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
        OperationResultKeeper keeper_res(res);
        EXPECT_EQ(proj_operation_result_get_count(res), 2);
        auto op = proj_operation_result_get(res, 0);
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

    EXPECT_TRUE(proj_context_set_database_path(m_ctxt, nullptr, nullptr));
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
    EXPECT_TRUE(
        proj_context_set_database_path(m_ctxt, ":memory:", aux_db_list));

    auto source_crs = proj_obj_create_from_database(m_ctxt, "EPSG", "4326",
                                                    PJ_OBJ_CATEGORY_CRS, false,
                                                    nullptr); // WGS84
    ASSERT_NE(source_crs, nullptr);
    ObjectKeeper keeper_source_crs(source_crs);
}

// ---------------------------------------------------------------------------

TEST_F(CApi, proj_context_set_database_path_error_1) {

    EXPECT_FALSE(
        proj_context_set_database_path(m_ctxt, "i_do_not_exist.db", nullptr));

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
    EXPECT_FALSE(proj_context_set_database_path(m_ctxt, nullptr, aux_db_list));

    // We will eventually re-open on the default DB
    auto source_crs = proj_obj_create_from_database(m_ctxt, "EPSG", "4326",
                                                    PJ_OBJ_CATEGORY_CRS, false,
                                                    nullptr); // WGS84
    ASSERT_NE(source_crs, nullptr);
    ObjectKeeper keeper_source_crs(source_crs);
}

} // namespace
