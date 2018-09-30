/******************************************************************************
 *
 * Project:  PROJ
 * Purpose:  ISO19111:2018 implementation
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

#ifndef FROM_PROJ_CPP
#define FROM_PROJ_CPP
#endif

#include "proj/common.hpp"
#include "proj/coordinateoperation.hpp"
#include "proj/coordinatesystem.hpp"
#include "proj/crs.hpp"
#include "proj/datum.hpp"
#include "proj/internal.hpp"
#include "proj/io.hpp"
#include "proj/io_internal.hpp"
#include "proj/metadata.hpp"
#include "proj/util.hpp"

#include <cmath>
#include <cstdlib>
#include <iomanip>
#include <map>
#include <memory>
#include <sstream> // std::ostringstream
#include <string>

#include "lru_cache.hpp"

#include "projects.h" // DIR_CHAR

#include <sqlite3.h>

using namespace NS_PROJ::internal;
using namespace NS_PROJ::common;

NS_PROJ_START
namespace io {

//! @cond Doxygen_Suppress

// ---------------------------------------------------------------------------

struct SQLValues {
    enum class Type { STRING };

    // cppcheck-suppress noExplicitConstructor
    SQLValues(const std::string &value) : type_(Type::STRING), str_(value) {}

    const Type &type() const { return type_; }

    // cppcheck-suppress functionStatic
    const std::string &stringValue() const { return str_; }

  private:
    Type type_;
    std::string str_{};
};

// ---------------------------------------------------------------------------

using SQLRow = std::vector<std::string>;
using SQLResultSet = std::vector<SQLRow>;

// ---------------------------------------------------------------------------

struct DatabaseContext::Private {
    Private();
    ~Private();

    void open(const std::string &databasePath = std::string());
    void setHandle(sqlite3 *sqlite_handle);

    sqlite3 *handle() const { return sqlite_handle_; }

    SQLResultSet
    run(const std::string &sql,
        const std::vector<SQLValues> &parameters = std::vector<SQLValues>());

  private:
    bool close_handle_ = true;
    sqlite3 *sqlite_handle_{};

    void registerFunctions();
};

// ---------------------------------------------------------------------------

DatabaseContext::Private::Private() = default;

// ---------------------------------------------------------------------------

DatabaseContext::Private::~Private() {
    if (close_handle_) {
        sqlite3_close(sqlite_handle_);
    }
}

// ---------------------------------------------------------------------------

void DatabaseContext::Private::open(const std::string &databasePath) {
    std::string path(databasePath);
    if (path.empty()) {
        const char *proj_lib = std::getenv("PROJ_LIB");
#ifdef PROJ_LIB
        if (!proj_lib) {
            proj_lib = PROJ_LIB;
        }
#endif
        if (!proj_lib) {
            throw FactoryException(
                "Cannot find proj.db due to missing PROJ_LIB");
        }
        path = std::string(proj_lib) + DIR_CHAR + "proj.db";
    }
    if (sqlite3_open_v2(path.c_str(), &sqlite_handle_, SQLITE_OPEN_READONLY,
                        nullptr) != SQLITE_OK ||
        !sqlite_handle_) {
        throw FactoryException("Open of " + path + " failed");
    }

    registerFunctions();
}

// ---------------------------------------------------------------------------

void DatabaseContext::Private::setHandle(sqlite3 *sqlite_handle) {
    sqlite_handle_ = sqlite_handle;
    close_handle_ = false;

    registerFunctions();
}

// ---------------------------------------------------------------------------

static double PROJ_SQLITE_GetValAsDouble(sqlite3_value *val, bool &gotVal) {
    switch (sqlite3_value_type(val)) {
    case SQLITE_FLOAT:
        gotVal = true;
        return sqlite3_value_double(val);

    case SQLITE_INTEGER:
        gotVal = true;
        return static_cast<double>(sqlite3_value_int64(val));

    default:
        gotVal = false;
        return 0.0;
    }
}

// ---------------------------------------------------------------------------

static void PROJ_SQLITE_pseudo_area_from_swne(sqlite3_context *pContext,
                                              int /* argc */,
                                              sqlite3_value **argv) {
    bool b0, b1, b2, b3;
    double south_lat = PROJ_SQLITE_GetValAsDouble(argv[0], b0);
    double west_lon = PROJ_SQLITE_GetValAsDouble(argv[1], b1);
    double north_lat = PROJ_SQLITE_GetValAsDouble(argv[2], b2);
    double east_lon = PROJ_SQLITE_GetValAsDouble(argv[3], b3);
    if (!b0 || !b1 || !b2 || !b3) {
        sqlite3_result_null(pContext);
        return;
    }
    // Deal with area crossing antimeridian
    if (east_lon < west_lon) {
        east_lon += 360.0;
    }
    // Integrate cos(lat) between south_lat and north_lat
    double pseudo_area = (east_lon - west_lon) *
                         (std::sin(common::Angle(north_lat).getSIValue()) -
                          std::sin(common::Angle(south_lat).getSIValue()));
    sqlite3_result_double(pContext, pseudo_area);
}

// ---------------------------------------------------------------------------

#ifndef SQLITE_DETERMINISTIC
#define SQLITE_DETERMINISTIC 0
#endif

void DatabaseContext::Private::registerFunctions() {
    sqlite3_create_function(sqlite_handle_, "pseudo_area_from_swne", 4,
                            SQLITE_UTF8 | SQLITE_DETERMINISTIC, nullptr,
                            PROJ_SQLITE_pseudo_area_from_swne, nullptr,
                            nullptr);
}

// ---------------------------------------------------------------------------

SQLResultSet
DatabaseContext::Private::run(const std::string &sql,
                              const std::vector<SQLValues> &parameters) {

    std::string expandSql;
    bool inString = false;
    bool lastWasEscape = false;
    char endString = 0;
    size_t iSubstitution = 0;
    for (const auto &ch : sql) {
        if (inString) {
            expandSql += ch;
            if (lastWasEscape) {
                lastWasEscape = false;
            } else if (ch == '\\') {
                lastWasEscape = true;
            } else if (ch == endString) {
                inString = false;
            }
        } else if (ch == '"' || ch == '\'') {
            expandSql += ch;
            inString = true;
            endString = ch;
        } else if (ch == '?') {
            if (iSubstitution == parameters.size()) {
                throw FactoryException(
                    "not enough parameters in substitution list");
            }
            const auto &param = parameters[iSubstitution];
            if (param.type() == SQLValues::Type::STRING) {
                expandSql += "'";
                expandSql += replaceAll(
                    replaceAll(param.stringValue(), "\\", "\\\\"), "'", "\\'");
                expandSql += "'";
            }
            iSubstitution++;
        } else {
            expandSql += ch;
        }
    }
    if (iSubstitution != parameters.size()) {
        throw FactoryException("unused parameters in substitution list");
    }

    char **papszResult = nullptr;
    int rows = 0;
    int cols = 0;
    char *sqliteErrMsg = nullptr;
    int rc = sqlite3_get_table(sqlite_handle_, expandSql.c_str(), &papszResult,
                               &rows, &cols, &sqliteErrMsg);
    if (rc != SQLITE_OK) {
        auto ex = FactoryException("SQLite error on " + expandSql + ": " +
                                   sqliteErrMsg);
        sqlite3_free(sqliteErrMsg);
        throw ex;
    }
    SQLResultSet result;
    for (int j = 0; j < rows; ++j) {
        SQLRow row;
        for (int i = 0; i < cols; ++i) {
            auto v = papszResult[(j + 1) * cols + i];
            row.emplace_back(v ? v : std::string());
        }
        result.emplace_back(row);
    }
    sqlite3_free_table(papszResult);
    return result;
}

//! @endcond

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
DatabaseContext::~DatabaseContext() = default;
//! @endcond

// ---------------------------------------------------------------------------

DatabaseContext::DatabaseContext() : d(internal::make_unique<Private>()) {}

// ---------------------------------------------------------------------------

/** \brief Instanciate a database context, using the default proj.db file
 *
 * It will be searched in the directory pointed by the PROJ_LIB environment
 * variable. If not found, on Unix builds, it will be then searched first in
 * the pkgdatadir directory of the installation prefix.
 *
 * This database context should be used only by one thread at a time.
 * @throw FactoryException
 */
DatabaseContextNNPtr DatabaseContext::create() {
    auto ctxt = DatabaseContext::nn_make_shared<DatabaseContext>();
    ctxt->getPrivate()->open();
    return ctxt;
}

// ---------------------------------------------------------------------------

/** \brief Instanciate a database context from a full filename.
 *
 * This database context should be used only by one thread at a time.
 * @param databasePath Path and filename of the database.
 * @throw FactoryException
 */
DatabaseContextNNPtr DatabaseContext::create(const std::string &databasePath) {
    auto ctxt = DatabaseContext::nn_make_shared<DatabaseContext>();
    ctxt->getPrivate()->open(databasePath);
    return ctxt;
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress

DatabaseContextNNPtr DatabaseContext::create(void *sqlite_handle) {
    auto ctxt = DatabaseContext::nn_make_shared<DatabaseContext>();
    ctxt->getPrivate()->setHandle(static_cast<sqlite3 *>(sqlite_handle));
    return ctxt;
}

// ---------------------------------------------------------------------------

void *DatabaseContext::getSqliteHandle() const {
    return getPrivate()->handle();
}

//! @endcond

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
struct AuthorityFactory::Private {
    Private(DatabaseContextNNPtr contextIn, const std::string &authorityName)
        : context_(contextIn), authority_(authorityName) {}

    const std::string &authority() const { return authority_; }

    DatabaseContextNNPtr context() const { return context_; }

    // cppcheck-suppress functionStatic
    void setThis(AuthorityFactoryNNPtr factory) {
        thisFactory_ = factory.as_nullable();
    }

    AuthorityFactoryNNPtr createFactory(const std::string &auth_name);

    crs::CRSPtr getCRSFromCache(const std::string &code);
    void cache(const std::string &code, const crs::CRSNNPtr &crs);

  private:
    DatabaseContextNNPtr context_;
    std::string authority_;
    std::weak_ptr<AuthorityFactory> thisFactory_{};
    std::weak_ptr<AuthorityFactory> parentFactory_{};
    std::map<std::string, AuthorityFactoryNNPtr> mapFactory_{};
    lru11::Cache<std::string, crs::CRSPtr> cacheCRS_{};
};

// ---------------------------------------------------------------------------

AuthorityFactoryNNPtr
AuthorityFactory::Private::createFactory(const std::string &auth_name) {

    // If we are a child factory, then create new factory on the parent
    auto parentFactoryLocked(parentFactory_.lock());
    if (parentFactoryLocked) {
        return parentFactoryLocked->d->createFactory(auth_name);
    }

    // If asked for a factory with our name, return ourselves.
    auto lockedThisFactory(thisFactory_.lock());
    assert(lockedThisFactory);
    if (auth_name == lockedThisFactory->getAuthority()) {
        return NN_CHECK_ASSERT(lockedThisFactory);
    }

    // Find if there is already a child factory with the passed name.
    auto iter = mapFactory_.find(auth_name);
    if (iter == mapFactory_.end()) {
        auto newFactory = AuthorityFactory::create(context_, auth_name);
        newFactory->d->parentFactory_ = thisFactory_;
        mapFactory_.insert(std::pair<std::string, AuthorityFactoryNNPtr>(
            auth_name, newFactory));
        return newFactory;
    }
    return iter->second;
}

// ---------------------------------------------------------------------------

crs::CRSPtr
AuthorityFactory::Private::getCRSFromCache(const std::string &code) {
    crs::CRSPtr crs = nullptr;
    cacheCRS_.tryGet(code, crs);
    return crs;
}

// ---------------------------------------------------------------------------

void AuthorityFactory::Private::cache(const std::string &code,
                                      const crs::CRSNNPtr &crs) {
    cacheCRS_.insert(code, crs.as_nullable());
}

//! @endcond

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
AuthorityFactory::~AuthorityFactory() = default;
//! @endcond

// ---------------------------------------------------------------------------

AuthorityFactory::AuthorityFactory(DatabaseContextNNPtr context,
                                   const std::string &authorityName)
    : d(internal::make_unique<Private>(context, authorityName)) {}

// ---------------------------------------------------------------------------

// clang-format off
/** \brief Instanciate a AuthorityFactory.
 *
 * The authority name might be set to the empty string in the particular case
 * where createFromCoordinateReferenceSystemCodes(const std::string&,const std::string&,const std::string&,const std::string&) const
 * is called.
 *
 * @param context Contexte.
 * @param authorityName Authority name.
 * @return new AuthorityFactory.
 */
// clang-format on

AuthorityFactoryNNPtr
AuthorityFactory::create(DatabaseContextNNPtr context,
                         const std::string &authorityName) {
    auto factory = AuthorityFactory::nn_make_shared<AuthorityFactory>(
        context, authorityName);
    factory->d->setThis(factory);
    return factory;
}

// ---------------------------------------------------------------------------

/** \brief Returns an arbitrary object from a code.
 *
 * The returned object will typically be an instance of Datum,
 * CoordinateSystem, ReferenceSystem or CoordinateOperation. If the type of
 * the object is know at compile time, it is recommended to invoke the most
 * precise method instead of this one (for example
 * createCoordinateReferenceSystem(code) instead of createObject(code)
 * if the caller know he is asking for a coordinate reference system).
 *
 * If there are several objects with the same code, a FactoryException is
 * thrown.
 *
 * @param code Object code allocated by authority. (e.g. "4326")
 * @return object.
 * @throw NoSuchAuthorityCodeException
 * @throw FactoryException
 */

util::BaseObjectNNPtr
AuthorityFactory::createObject(const std::string &code) const {

    auto res = d->context()->getPrivate()->run(
        "SELECT table_name FROM object_view WHERE auth_name = ? AND code = ?",
        {getAuthority(), code});
    if (res.empty()) {
        throw NoSuchAuthorityCodeException("not found", getAuthority(), code);
    }
    if (res.size() != 1) {
        std::string msg(
            "More than one object matching specified code. Objects found in ");
        bool first = true;
        for (const auto &row : res) {
            if (!first)
                msg += ", ";
            msg += row[0];
            first = false;
        }
        throw FactoryException(msg);
    }
    const auto &table_name = res[0][0];
    if (table_name == "area") {
        return util::nn_static_pointer_cast<util::BaseObject>(
            createExtent(code));
    }
    if (table_name == "unit_of_measure") {
        return util::nn_static_pointer_cast<util::BaseObject>(
            createUnitOfMeasure(code));
    }
    if (table_name == "prime_meridian") {
        return util::nn_static_pointer_cast<util::BaseObject>(
            createPrimeMeridian(code));
    }
    if (table_name == "ellipsoid") {
        return util::nn_static_pointer_cast<util::BaseObject>(
            createEllipsoid(code));
    }
    if (table_name == "geodetic_datum") {
        return util::nn_static_pointer_cast<util::BaseObject>(
            createGeodeticDatum(code));
    }
    if (table_name == "vertical_datum") {
        return util::nn_static_pointer_cast<util::BaseObject>(
            createVerticalDatum(code));
    }
    if (table_name == "geodetic_crs") {
        return util::nn_static_pointer_cast<util::BaseObject>(
            createGeodeticCRS(code));
    }
    if (table_name == "vertical_crs") {
        return util::nn_static_pointer_cast<util::BaseObject>(
            createVerticalCRS(code));
    }
    if (table_name == "projected_crs") {
        return util::nn_static_pointer_cast<util::BaseObject>(
            createProjectedCRS(code));
    }
    if (table_name == "compound_crs") {
        return util::nn_static_pointer_cast<util::BaseObject>(
            createCompoundCRS(code));
    }
    if (table_name == "conversion") {
        return util::nn_static_pointer_cast<util::BaseObject>(
            createConversion(code));
    }
    if (table_name == "helmert_transformation" ||
        table_name == "grid_transformation" ||
        table_name == "other_transformation" ||
        table_name == "concatenated_operation") {
        return util::nn_static_pointer_cast<util::BaseObject>(
            createCoordinateOperation(code));
    }
    throw FactoryException("unimplemented factory for " + res[0][0]);
}

// ---------------------------------------------------------------------------

/** \brief Returns a metadata::Extent from the specified code.
 *
 * @param code Object code allocated by authority.
 * @return object.
 * @throw NoSuchAuthorityCodeException
 * @throw FactoryException
 */

metadata::ExtentNNPtr
AuthorityFactory::createExtent(const std::string &code) const {
    auto sql = "SELECT name, south_lat, north_lat, west_lon, east_lon, "
               "deprecated FROM area WHERE auth_name = ? AND code = ?";
    auto res = d->context()->getPrivate()->run(sql, {getAuthority(), code});
    if (res.empty()) {
        throw NoSuchAuthorityCodeException("area not found", getAuthority(),
                                           code);
    }
    try {
        const auto &row = res[0];
        const auto &name = row[0];
        double south_lat = c_locale_stod(row[1]);
        double north_lat = c_locale_stod(row[2]);
        double west_lon = c_locale_stod(row[3]);
        double east_lon = c_locale_stod(row[4]);
        auto bbox = metadata::GeographicBoundingBox::create(
            west_lon, south_lat, east_lon, north_lat);

        return metadata::Extent::create(
            util::optional<std::string>(name),
            std::vector<metadata::GeographicExtentNNPtr>{bbox},
            std::vector<metadata::VerticalExtentNNPtr>(),
            std::vector<metadata::TemporalExtentNNPtr>());

    } catch (const std::exception &ex) {
        throw FactoryException("cannot build area " + code + ": " + ex.what());
    }
}

// ---------------------------------------------------------------------------

/** \brief Returns a common::UnitOfMeasure from the specified code.
 *
 * @param code Object code allocated by authority.
 * @return object.
 * @throw NoSuchAuthorityCodeException
 * @throw FactoryException
 */

UnitOfMeasureNNPtr
AuthorityFactory::createUnitOfMeasure(const std::string &code) const {
    auto res = d->context()->getPrivate()->run(
        "SELECT name, conv_factor, type, deprecated FROM unit_of_measure WHERE "
        "auth_name = ? AND code = ?",
        {getAuthority(), code});
    if (res.empty()) {
        throw NoSuchAuthorityCodeException("unit of measure not found",
                                           getAuthority(), code);
    }
    try {
        const auto &row = res[0];
        const auto &name =
            (row[0] == "degree (supplier to define representation)")
                ? UnitOfMeasure::DEGREE.name()
                : row[0];
        double conv_factor = (code == "9107" || code == "9108")
                                 ? UnitOfMeasure::DEGREE.conversionToSI()
                                 : c_locale_stod(row[1]);
        constexpr double EPS = 1e-10;
        if (std::fabs(conv_factor - UnitOfMeasure::DEGREE.conversionToSI()) <
            EPS * UnitOfMeasure::DEGREE.conversionToSI()) {
            conv_factor = UnitOfMeasure::DEGREE.conversionToSI();
        }
        if (std::fabs(conv_factor -
                      UnitOfMeasure::ARC_SECOND.conversionToSI()) <
            EPS * UnitOfMeasure::ARC_SECOND.conversionToSI()) {
            conv_factor = UnitOfMeasure::ARC_SECOND.conversionToSI();
        }
        const auto &type_str = row[2];
        UnitOfMeasure::Type unitType = UnitOfMeasure::Type::UNKNOWN;
        if (type_str == "length")
            unitType = UnitOfMeasure::Type::LINEAR;
        else if (type_str == "angle")
            unitType = UnitOfMeasure::Type::ANGULAR;
        else if (type_str == "scale")
            unitType = UnitOfMeasure::Type::SCALE;
        else if (type_str == "time")
            unitType = UnitOfMeasure::Type::TIME;
        return util::nn_make_shared<UnitOfMeasure>(name, conv_factor, unitType,
                                                   getAuthority(), code);
    } catch (const std::exception &ex) {
        throw FactoryException("cannot build unit of measure " + code + ": " +
                               ex.what());
    }
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
static void normalizeMeasure(const std::string &uom_code,
                             const std::string &value,
                             std::string &normalized_uom_code,
                             double &normalized_value) {
    normalized_uom_code = uom_code;
    normalized_value = c_locale_stod(value);
    if (uom_code == "9110") // DDD.MMSSsss
    {
        std::ostringstream buffer;
        buffer.imbue(std::locale::classic());
        buffer << std::fixed << std::setprecision(7) << normalized_value;
        auto formatted = buffer.str();
        size_t dotPos = formatted.find('.');
        assert(dotPos + 1 + 7 == formatted.size());
        auto MM = formatted.substr(dotPos + 1, 2);
        auto SSsss = formatted.substr(dotPos + 3, 2 + 3);
        normalized_value =
            (normalized_value < 0 ? -1.0 : 1.0) *
            (int(std::fabs(normalized_value)) + c_locale_stod(MM) / 60. +
             (c_locale_stod(SSsss) / 1000.0) / 3600.);
        normalized_uom_code = common::UnitOfMeasure::DEGREE.code();
    }
}
//! @endcond

// ---------------------------------------------------------------------------

/** \brief Returns a datum::PrimeMeridian from the specified code.
 *
 * @param code Object code allocated by authority.
 * @return object.
 * @throw NoSuchAuthorityCodeException
 * @throw FactoryException
 */

datum::PrimeMeridianNNPtr
AuthorityFactory::createPrimeMeridian(const std::string &code) const {
    auto res = d->context()->getPrivate()->run(
        "SELECT name, longitude, uom_auth_name, uom_code, deprecated FROM "
        "prime_meridian WHERE "
        "auth_name = ? AND code = ?",
        {getAuthority(), code});
    if (res.empty()) {
        throw NoSuchAuthorityCodeException("prime meridian not found",
                                           getAuthority(), code);
    }
    try {
        const auto &row = res[0];
        const auto &name = row[0];
        const auto &longitude = row[1];
        const auto &uom_auth_name = row[2];
        const auto &uom_code = row[3];
        const bool deprecated = row[4] == "1";

        std::string normalized_uom_code(uom_code);
        double normalized_value(c_locale_stod(longitude));
        normalizeMeasure(uom_code, longitude, normalized_uom_code,
                         normalized_value);

        auto uom = d->createFactory(uom_auth_name)
                       ->createUnitOfMeasure(normalized_uom_code);
        auto props =
            util::PropertyMap()
                .set(metadata::Identifier::CODESPACE_KEY, getAuthority())
                .set(metadata::Identifier::CODE_KEY, code)
                .set(common::IdentifiedObject::NAME_KEY, name)
                .set(common::IdentifiedObject::DEPRECATED_KEY, deprecated);
        return datum::PrimeMeridian::create(
            props, common::Angle(normalized_value, *uom));
    } catch (const std::exception &ex) {
        throw FactoryException("cannot build prime meridian " + code + ": " +
                               ex.what());
    }
}

// ---------------------------------------------------------------------------

/** \brief Returns a datum::Ellipsoid from the specified code.
 *
 * @param code Object code allocated by authority.
 * @return object.
 * @throw NoSuchAuthorityCodeException
 * @throw FactoryException
 */

datum::EllipsoidNNPtr
AuthorityFactory::createEllipsoid(const std::string &code) const {
    auto res = d->context()->getPrivate()->run(
        "SELECT name, semi_major_axis, uom_auth_name, uom_code, "
        "inv_flattening, semi_minor_axis, deprecated FROM "
        "ellipsoid WHERE "
        "auth_name = ? AND code = ?",
        {getAuthority(), code});
    if (res.empty()) {
        throw NoSuchAuthorityCodeException("ellipsoid not found",
                                           getAuthority(), code);
    }
    try {
        const auto &row = res[0];
        const auto &name = row[0];
        const auto &semi_major_axis_str = row[1];
        double semi_major_axis = c_locale_stod(semi_major_axis_str);
        const auto &uom_auth_name = row[2];
        const auto &uom_code = row[3];
        const auto &inv_flattening_str = row[4];
        const auto &semi_minor_axis_str = row[5];
        const bool deprecated = row[6] == "1";
        auto uom =
            d->createFactory(uom_auth_name)->createUnitOfMeasure(uom_code);
        auto props =
            util::PropertyMap()
                .set(metadata::Identifier::CODESPACE_KEY, getAuthority())
                .set(metadata::Identifier::CODE_KEY, code)
                .set(common::IdentifiedObject::NAME_KEY, name)
                .set(common::IdentifiedObject::DEPRECATED_KEY, deprecated);
        if (!inv_flattening_str.empty()) {
            return datum::Ellipsoid::createFlattenedSphere(
                props, common::Length(semi_major_axis, *uom),
                common::Scale(c_locale_stod(inv_flattening_str)));
        } else if (semi_major_axis_str == semi_minor_axis_str) {
            return datum::Ellipsoid::createSphere(
                props, common::Length(semi_major_axis, *uom));
        } else {
            return datum::Ellipsoid::createTwoAxis(
                props, common::Length(semi_major_axis, *uom),
                common::Length(c_locale_stod(semi_minor_axis_str), *uom));
        }
    } catch (const std::exception &ex) {
        throw FactoryException("cannot build ellipsoid " + code + ": " +
                               ex.what());
    }
}

// ---------------------------------------------------------------------------

/** \brief Returns a datum::GeodeticReferenceFrame from the specified code.
 *
 * @param code Object code allocated by authority.
 * @return object.
 * @throw NoSuchAuthorityCodeException
 * @throw FactoryException
 */

datum::GeodeticReferenceFrameNNPtr
AuthorityFactory::createGeodeticDatum(const std::string &code) const {
    auto res = d->context()->getPrivate()->run(
        "SELECT name, ellipsoid_auth_name, ellipsoid_code, "
        "prime_meridian_auth_name, prime_meridian_code, area_of_use_auth_name, "
        "area_of_use_code, deprecated FROM geodetic_datum WHERE "
        "auth_name = ? AND code = ?",
        {getAuthority(), code});
    if (res.empty()) {
        throw NoSuchAuthorityCodeException("geodetic datum not found",
                                           getAuthority(), code);
    }
    try {
        const auto &row = res[0];
        const auto &name = row[0];
        const auto &ellipsoid_auth_name = row[1];
        const auto &ellipsoid_code = row[2];
        const auto &prime_meridian_auth_name = row[3];
        const auto &prime_meridian_code = row[4];
        const auto &area_of_use_auth_name = row[5];
        const auto &area_of_use_code = row[6];
        const bool deprecated = row[7] == "1";
        auto ellipsoid = d->createFactory(ellipsoid_auth_name)
                             ->createEllipsoid(ellipsoid_code);
        auto pm = d->createFactory(prime_meridian_auth_name)
                      ->createPrimeMeridian(prime_meridian_code);
        auto extent = d->createFactory(area_of_use_auth_name)
                          ->createExtent(area_of_use_code);
        auto props =
            util::PropertyMap()
                .set(metadata::Identifier::CODESPACE_KEY, getAuthority())
                .set(metadata::Identifier::CODE_KEY, code)
                .set(common::IdentifiedObject::NAME_KEY, name)
                .set(common::IdentifiedObject::DEPRECATED_KEY, deprecated)
                .set(common::ObjectUsage::DOMAIN_OF_VALIDITY_KEY, extent);
        auto anchor = util::optional<std::string>();
        return datum::GeodeticReferenceFrame::create(props, ellipsoid, anchor,
                                                     pm);
    } catch (const std::exception &ex) {
        throw FactoryException("cannot build geodetic reference frame " + code +
                               ": " + ex.what());
    }
}

// ---------------------------------------------------------------------------

/** \brief Returns a datum::VerticalReferenceFrame from the specified code.
 *
 * @param code Object code allocated by authority.
 * @return object.
 * @throw NoSuchAuthorityCodeException
 * @throw FactoryException
 */

datum::VerticalReferenceFrameNNPtr
AuthorityFactory::createVerticalDatum(const std::string &code) const {
    auto res = d->context()->getPrivate()->run(
        "SELECT name, area_of_use_auth_name, area_of_use_code, deprecated FROM "
        "vertical_datum WHERE auth_name = ? AND code = ?",
        {getAuthority(), code});
    if (res.empty()) {
        throw NoSuchAuthorityCodeException("vertical datum not found",
                                           getAuthority(), code);
    }
    try {
        const auto &row = res[0];
        const auto &name = row[0];
        const auto &area_of_use_auth_name = row[1];
        const auto &area_of_use_code = row[2];
        const bool deprecated = row[3] == "1";
        auto extent = d->createFactory(area_of_use_auth_name)
                          ->createExtent(area_of_use_code);
        auto props =
            util::PropertyMap()
                .set(metadata::Identifier::CODESPACE_KEY, getAuthority())
                .set(metadata::Identifier::CODE_KEY, code)
                .set(common::IdentifiedObject::NAME_KEY, name)
                .set(common::IdentifiedObject::DEPRECATED_KEY, deprecated)
                .set(common::ObjectUsage::DOMAIN_OF_VALIDITY_KEY, extent);
        auto anchor = util::optional<std::string>();
        return datum::VerticalReferenceFrame::create(props, anchor);
    } catch (const std::exception &ex) {
        throw FactoryException("cannot build vertical reference frame " + code +
                               ": " + ex.what());
    }
}

// ---------------------------------------------------------------------------

/** \brief Returns a datum::Datum from the specified code.
 *
 * @param code Object code allocated by authority.
 * @return object.
 * @throw NoSuchAuthorityCodeException
 * @throw FactoryException
 */

datum::DatumNNPtr AuthorityFactory::createDatum(const std::string &code) const {
    auto res = d->context()->getPrivate()->run(
        "SELECT 'geodetic_datum' FROM geodetic_datum WHERE "
        "auth_name = ? AND code = ? "
        "UNION ALL SELECT 'vertical_datum' FROM vertical_datum WHERE "
        "auth_name = ? AND code = ?",
        {getAuthority(), code, getAuthority(), code});
    if (res.empty()) {
        throw NoSuchAuthorityCodeException("datum not found", getAuthority(),
                                           code);
    }
    if (res[0][0] == "geodetic_datum") {
        return createGeodeticDatum(code);
    }
    return createVerticalDatum(code);
}

// ---------------------------------------------------------------------------

/** \brief Returns a cs::CoordinateSystem from the specified code.
 *
 * @param code Object code allocated by authority.
 * @return object.
 * @throw NoSuchAuthorityCodeException
 * @throw FactoryException
 */

cs::CoordinateSystemNNPtr
AuthorityFactory::createCoordinateSystem(const std::string &code) const {
    auto res = d->context()->getPrivate()->run(
        "SELECT axis.name, abbrev, orientation, uom_auth_name, uom_code, "
        "cs.type FROM "
        "axis LEFT JOIN coordinate_system cs ON "
        "axis.coordinate_system_auth_name = cs.auth_name AND "
        "axis.coordinate_system_code = cs.code WHERE "
        "coordinate_system_auth_name = ? AND coordinate_system_code = ? ORDER "
        "BY coordinate_system_order",
        {getAuthority(), code});
    if (res.empty()) {
        throw NoSuchAuthorityCodeException("coordinate system not found",
                                           getAuthority(), code);
    }

    const auto &csType = res[0][5];
    std::vector<cs::CoordinateSystemAxisNNPtr> axisList;
    for (const auto &row : res) {
        const auto &name = row[0];
        const auto &abbrev = row[1];
        const auto &orientation = row[2];
        const auto &uom_auth_name = row[3];
        const auto &uom_code = row[4];
        auto uom =
            d->createFactory(uom_auth_name)->createUnitOfMeasure(uom_code);
        auto props =
            util::PropertyMap().set(common::IdentifiedObject::NAME_KEY, name);
        const cs::AxisDirection *direction =
            cs::AxisDirection::valueOf(orientation);
        if (direction == nullptr) {
            if (orientation == "Geocentre > equator/0°E") {
                direction = &(cs::AxisDirection::GEOCENTRIC_X);
            } else if (orientation == "Geocentre > equator/90°E") {
                direction = &(cs::AxisDirection::GEOCENTRIC_Y);
            } else if (orientation == "Geocentre > north pole") {
                direction = &(cs::AxisDirection::GEOCENTRIC_Z);
            } else {
                throw FactoryException("unknown axis direction: " +
                                       orientation);
            }
        }
        axisList.emplace_back(
            cs::CoordinateSystemAxis::create(props, abbrev, *direction, *uom));
    }
    auto props = util::PropertyMap()
                     .set(metadata::Identifier::CODESPACE_KEY, getAuthority())
                     .set(metadata::Identifier::CODE_KEY, code);
    if (csType == "ellipsoidal") {
        if (axisList.size() == 2) {
            return cs::EllipsoidalCS::create(props, axisList[0], axisList[1]);
        }
        if (axisList.size() == 3) {
            return cs::EllipsoidalCS::create(props, axisList[0], axisList[1],
                                             axisList[2]);
        }
        throw FactoryException("invalid number of axis for EllipsoidalCS");
    }
    if (csType == "Cartesian") {
        if (axisList.size() == 2) {
            return cs::CartesianCS::create(props, axisList[0], axisList[1]);
        }
        if (axisList.size() == 3) {
            return cs::CartesianCS::create(props, axisList[0], axisList[1],
                                           axisList[2]);
        }
        throw FactoryException("invalid number of axis for CartesianCS");
    }
    if (csType == "vertical") {
        if (axisList.size() == 1) {
            return cs::VerticalCS::create(props, axisList[0]);
        }
        throw FactoryException("invalid number of axis for VerticalCS");
    }
    throw FactoryException("unhandled coordinate system type: " + csType);
}

// ---------------------------------------------------------------------------

/** \brief Returns a crs::GeodeticCRS from the specified code.
 *
 * @param code Object code allocated by authority.
 * @return object.
 * @throw NoSuchAuthorityCodeException
 * @throw FactoryException
 */

crs::GeodeticCRSNNPtr
AuthorityFactory::createGeodeticCRS(const std::string &code) const {
    return createGeodeticCRS(code, false);
}

// ---------------------------------------------------------------------------

/** \brief Returns a crs::GeographicCRS from the specified code.
 *
 * @param code Object code allocated by authority.
 * @return object.
 * @throw NoSuchAuthorityCodeException
 * @throw FactoryException
 */

crs::GeographicCRSNNPtr
AuthorityFactory::createGeographicCRS(const std::string &code) const {
    return NN_CHECK_ASSERT(util::nn_dynamic_pointer_cast<crs::GeographicCRS>(
        createGeodeticCRS(code, true)));
}

// ---------------------------------------------------------------------------

crs::GeodeticCRSNNPtr
AuthorityFactory::createGeodeticCRS(const std::string &code,
                                    bool geographicOnly) const {
    auto crs =
        std::dynamic_pointer_cast<crs::GeodeticCRS>(d->getCRSFromCache(code));
    if (crs) {
        return NN_CHECK_ASSERT(crs);
    }
    std::string sql("SELECT name, type, coordinate_system_auth_name, "
                    "coordinate_system_code, datum_auth_name, datum_code, "
                    "area_of_use_auth_name, area_of_use_code, deprecated FROM "
                    "geodetic_crs WHERE auth_name = ? AND code = ?");
    if (geographicOnly) {
        sql += " AND type in ('geographic 2D', 'geographic 3D')";
    }
    auto res = d->context()->getPrivate()->run(sql, {getAuthority(), code});
    if (res.empty()) {
        throw NoSuchAuthorityCodeException("geodeticCRS not found",
                                           getAuthority(), code);
    }
    try {
        const auto &row = res[0];
        const auto &name = row[0];
        const auto &type = row[1];
        const auto &cs_auth_name = row[2];
        const auto &cs_code = row[3];
        const auto &datum_auth_name = row[4];
        const auto &datum_code = row[5];
        const auto &area_of_use_auth_name = row[6];
        const auto &area_of_use_code = row[7];
        const bool deprecated = row[8] == "1";
        auto cs =
            d->createFactory(cs_auth_name)->createCoordinateSystem(cs_code);
        auto datum =
            d->createFactory(datum_auth_name)->createGeodeticDatum(datum_code);
        auto extent = d->createFactory(area_of_use_auth_name)
                          ->createExtent(area_of_use_code);
        auto props =
            util::PropertyMap()
                .set(metadata::Identifier::CODESPACE_KEY, getAuthority())
                .set(metadata::Identifier::CODE_KEY, code)
                .set(common::IdentifiedObject::NAME_KEY, name)
                .set(common::IdentifiedObject::DEPRECATED_KEY, deprecated)
                .set(common::ObjectUsage::DOMAIN_OF_VALIDITY_KEY, extent);
        auto ellipsoidalCS =
            util::nn_dynamic_pointer_cast<cs::EllipsoidalCS>(cs);
        if ((type == "geographic 2D" || type == "geographic 3D") &&
            ellipsoidalCS) {
            auto crsRet = crs::GeographicCRS::create(
                props, datum, NN_CHECK_ASSERT(ellipsoidalCS));
            d->cache(code, util::nn_static_pointer_cast<crs::CRS>(crsRet));
            return crsRet;
        }
        auto geocentricCS = util::nn_dynamic_pointer_cast<cs::CartesianCS>(cs);
        if (type == "geocentric" && geocentricCS) {
            auto crsRet = crs::GeodeticCRS::create(
                props, datum, NN_CHECK_ASSERT(geocentricCS));
            d->cache(code, util::nn_static_pointer_cast<crs::CRS>(crsRet));
            return crsRet;
        }
        throw FactoryException("unsupported (type, CS type) for geodeticCRS: " +
                               type + ", " +
                               cs->getWKT2Type(WKTFormatter::create()));
    } catch (const std::exception &ex) {
        throw FactoryException("cannot build geodeticCRS " + code + ": " +
                               ex.what());
    }
}

// ---------------------------------------------------------------------------

/** \brief Returns a crs::VerticalCRS from the specified code.
 *
 * @param code Object code allocated by authority.
 * @return object.
 * @throw NoSuchAuthorityCodeException
 * @throw FactoryException
 */

crs::VerticalCRSNNPtr
AuthorityFactory::createVerticalCRS(const std::string &code) const {
    auto res = d->context()->getPrivate()->run(
        "SELECT name, coordinate_system_auth_name, "
        "coordinate_system_code, datum_auth_name, datum_code, "
        "area_of_use_auth_name, area_of_use_code, deprecated FROM "
        "vertical_crs WHERE auth_name = ? AND code = ?",
        {getAuthority(), code});
    if (res.empty()) {
        throw NoSuchAuthorityCodeException("verticalCRS not found",
                                           getAuthority(), code);
    }
    try {
        const auto &row = res[0];
        const auto &name = row[0];
        const auto &cs_auth_name = row[1];
        const auto &cs_code = row[2];
        const auto &datum_auth_name = row[3];
        const auto &datum_code = row[4];
        const auto &area_of_use_auth_name = row[5];
        const auto &area_of_use_code = row[6];
        const bool deprecated = row[7] == "1";
        auto cs =
            d->createFactory(cs_auth_name)->createCoordinateSystem(cs_code);
        auto datum =
            d->createFactory(datum_auth_name)->createVerticalDatum(datum_code);
        auto extent = d->createFactory(area_of_use_auth_name)
                          ->createExtent(area_of_use_code);
        auto props =
            util::PropertyMap()
                .set(metadata::Identifier::CODESPACE_KEY, getAuthority())
                .set(metadata::Identifier::CODE_KEY, code)
                .set(common::IdentifiedObject::NAME_KEY, name)
                .set(common::IdentifiedObject::DEPRECATED_KEY, deprecated)
                .set(common::ObjectUsage::DOMAIN_OF_VALIDITY_KEY, extent);
        auto verticalCS = util::nn_dynamic_pointer_cast<cs::VerticalCS>(cs);
        if (verticalCS) {
            return crs::VerticalCRS::create(props, datum,
                                            NN_CHECK_ASSERT(verticalCS));
        }
        throw FactoryException("unsupported CS type for verticalCRS: " +
                               cs->getWKT2Type(WKTFormatter::create()));
    } catch (const std::exception &ex) {
        throw FactoryException("cannot build verticalCRS " + code + ": " +
                               ex.what());
    }
}

// ---------------------------------------------------------------------------

/** \brief Returns a operation::Conversion from the specified code.
 *
 * @param code Object code allocated by authority.
 * @return object.
 * @throw NoSuchAuthorityCodeException
 * @throw FactoryException
 */

operation::ConversionNNPtr
AuthorityFactory::createConversion(const std::string &code) const {
    std::ostringstream buffer;
    buffer.imbue(std::locale::classic());
    buffer << "SELECT name, area_of_use_auth_name, area_of_use_code, "
              "method_auth_name, method_code, method_name";
    constexpr int N_MAX_PARAMS = 7;
    for (int i = 1; i <= N_MAX_PARAMS; ++i) {
        buffer << ", param" << i << "_auth_name";
        buffer << ", param" << i << "_code";
        buffer << ", param" << i << "_name";
        buffer << ", param" << i << "_value";
        buffer << ", param" << i << "_uom_auth_name";
        buffer << ", param" << i << "_uom_code";
    }
    buffer << " FROM conversion WHERE auth_name = ? AND code = ?";

    auto res =
        d->context()->getPrivate()->run(buffer.str(), {getAuthority(), code});
    if (res.empty()) {
        throw NoSuchAuthorityCodeException("conversion not found",
                                           getAuthority(), code);
    }
    try {
        const auto &row = res[0];
        size_t idx = 0;
        const auto &name = row[idx++];
        const auto &area_of_use_auth_name = row[idx++];
        const auto &area_of_use_code = row[idx++];
        const auto &method_auth_name = row[idx++];
        const auto &method_code = row[idx++];
        const auto &method_name = row[idx++];
        const size_t base_param_idx = idx;
        std::vector<operation::OperationParameterNNPtr> parameters;
        std::vector<operation::ParameterValueNNPtr> values;
        for (int i = 0; i < N_MAX_PARAMS; ++i) {
            const auto &param_auth_name = row[base_param_idx + i * 6 + 0];
            if (param_auth_name.empty()) {
                break;
            }
            const auto &param_code = row[base_param_idx + i * 6 + 1];
            const auto &param_name = row[base_param_idx + i * 6 + 2];
            const auto &param_value = row[base_param_idx + i * 6 + 3];
            const auto &param_uom_auth_name = row[base_param_idx + i * 6 + 4];
            const auto &param_uom_code = row[base_param_idx + i * 6 + 5];
            parameters.emplace_back(operation::OperationParameter::create(
                util::PropertyMap()
                    .set(metadata::Identifier::CODESPACE_KEY, param_auth_name)
                    .set(metadata::Identifier::CODE_KEY, param_code)
                    .set(common::IdentifiedObject::NAME_KEY, param_name)));
            std::string normalized_uom_code(param_uom_code);
            double normalized_value(c_locale_stod(param_value));
            normalizeMeasure(param_uom_code, param_value, normalized_uom_code,
                             normalized_value);
            auto uom = d->createFactory(param_uom_auth_name)
                           ->createUnitOfMeasure(normalized_uom_code);
            values.emplace_back(operation::ParameterValue::create(
                common::Measure(normalized_value, *uom)));
        }

        auto extent = d->createFactory(area_of_use_auth_name)
                          ->createExtent(area_of_use_code);

        auto propConversion =
            util::PropertyMap()
                .set(metadata::Identifier::CODESPACE_KEY, getAuthority())
                .set(metadata::Identifier::CODE_KEY, code)
                .set(common::IdentifiedObject::NAME_KEY, name)
                .set(common::ObjectUsage::DOMAIN_OF_VALIDITY_KEY, extent);

        auto propMethod =
            util::PropertyMap()
                .set(metadata::Identifier::CODESPACE_KEY, method_auth_name)
                .set(metadata::Identifier::CODE_KEY, method_code)
                .set(common::IdentifiedObject::NAME_KEY, method_name);

        return operation::Conversion::create(propConversion, propMethod,
                                             parameters, values);
    } catch (const std::exception &ex) {
        throw FactoryException("cannot build conversion " + code + ": " +
                               ex.what());
    }
}

// ---------------------------------------------------------------------------

/** \brief Returns a crs::ProjectedCRS from the specified code.
 *
 * @param code Object code allocated by authority.
 * @return object.
 * @throw NoSuchAuthorityCodeException
 * @throw FactoryException
 */

crs::ProjectedCRSNNPtr
AuthorityFactory::createProjectedCRS(const std::string &code) const {
    auto res = d->context()->getPrivate()->run(
        "SELECT name, coordinate_system_auth_name, "
        "coordinate_system_code, geodetic_crs_auth_name, geodetic_crs_code, "
        "conversion_auth_name, conversion_code, "
        "area_of_use_auth_name, area_of_use_code, deprecated FROM "
        "projected_crs WHERE auth_name = ? AND code = ?",
        {getAuthority(), code});
    if (res.empty()) {
        throw NoSuchAuthorityCodeException("projectedCRS not found",
                                           getAuthority(), code);
    }
    try {
        const auto &row = res[0];
        const auto &name = row[0];
        const auto &cs_auth_name = row[1];
        const auto &cs_code = row[2];
        const auto &geodetic_crs_auth_name = row[3];
        const auto &geodetic_crs_code = row[4];
        const auto &conversion_auth_name = row[5];
        const auto &conversion_code = row[6];
        const auto &area_of_use_auth_name = row[7];
        const auto &area_of_use_code = row[8];
        const bool deprecated = row[9] == "1";
        auto cs =
            d->createFactory(cs_auth_name)->createCoordinateSystem(cs_code);

        auto baseCRS = d->createFactory(geodetic_crs_auth_name)
                           ->createGeodeticCRS(geodetic_crs_code);

        auto conv = d->createFactory(conversion_auth_name)
                        ->createConversion(conversion_code);

        auto extent = d->createFactory(area_of_use_auth_name)
                          ->createExtent(area_of_use_code);
        auto props =
            util::PropertyMap()
                .set(metadata::Identifier::CODESPACE_KEY, getAuthority())
                .set(metadata::Identifier::CODE_KEY, code)
                .set(common::IdentifiedObject::NAME_KEY, name)
                .set(common::IdentifiedObject::DEPRECATED_KEY, deprecated)
                .set(common::ObjectUsage::DOMAIN_OF_VALIDITY_KEY, extent);
        auto cartesianCS = util::nn_dynamic_pointer_cast<cs::CartesianCS>(cs);
        if (cartesianCS) {
            return crs::ProjectedCRS::create(props, baseCRS, conv,
                                             NN_CHECK_ASSERT(cartesianCS));
        }
        throw FactoryException("unsupported CS type for projectedCRS: " +
                               cs->getWKT2Type(WKTFormatter::create()));
    } catch (const std::exception &ex) {
        throw FactoryException("cannot build projectedCRS " + code + ": " +
                               ex.what());
    }
}

// ---------------------------------------------------------------------------

/** \brief Returns a crs::CompoundCRS from the specified code.
 *
 * @param code Object code allocated by authority.
 * @return object.
 * @throw NoSuchAuthorityCodeException
 * @throw FactoryException
 */

crs::CompoundCRSNNPtr
AuthorityFactory::createCompoundCRS(const std::string &code) const {
    auto res = d->context()->getPrivate()->run(
        "SELECT name, horiz_crs_auth_name, horiz_crs_code, "
        "vertical_crs_auth_name, vertical_crs_code, "
        "area_of_use_auth_name, area_of_use_code, deprecated FROM "
        "compound_crs WHERE auth_name = ? AND code = ?",
        {getAuthority(), code});
    if (res.empty()) {
        throw NoSuchAuthorityCodeException("compoundCRS not found",
                                           getAuthority(), code);
    }
    try {
        const auto &row = res[0];
        const auto &name = row[0];
        const auto &horiz_crs_auth_name = row[1];
        const auto &horiz_crs_code = row[2];
        const auto &vertical_crs_auth_name = row[3];
        const auto &vertical_crs_code = row[4];
        const auto &area_of_use_auth_name = row[5];
        const auto &area_of_use_code = row[6];
        const bool deprecated = row[7] == "1";

        auto horizCRS =
            d->createFactory(horiz_crs_auth_name)
                ->createCoordinateReferenceSystem(horiz_crs_code, false);
        auto vertCRS = d->createFactory(vertical_crs_auth_name)
                           ->createVerticalCRS(vertical_crs_code);
        auto extent = d->createFactory(area_of_use_auth_name)
                          ->createExtent(area_of_use_code);
        auto props =
            util::PropertyMap()
                .set(metadata::Identifier::CODESPACE_KEY, getAuthority())
                .set(metadata::Identifier::CODE_KEY, code)
                .set(common::IdentifiedObject::NAME_KEY, name)
                .set(common::IdentifiedObject::DEPRECATED_KEY, deprecated)
                .set(common::ObjectUsage::DOMAIN_OF_VALIDITY_KEY, extent);
        return crs::CompoundCRS::create(
            props, std::vector<crs::CRSNNPtr>{horizCRS, vertCRS});
    } catch (const std::exception &ex) {
        throw FactoryException("cannot build compoundCRS " + code + ": " +
                               ex.what());
    }
}

// ---------------------------------------------------------------------------

/** \brief Returns a crs::CRS from the specified code.
 *
 * @param code Object code allocated by authority.
 * @return object.
 * @throw NoSuchAuthorityCodeException
 * @throw FactoryException
 */

crs::CRSNNPtr AuthorityFactory::createCoordinateReferenceSystem(
    const std::string &code) const {
    return createCoordinateReferenceSystem(code, true);
}

crs::CRSNNPtr
AuthorityFactory::createCoordinateReferenceSystem(const std::string &code,
                                                  bool allowCompound) const {
    auto crs = d->getCRSFromCache(code);
    if (crs) {
        return NN_CHECK_ASSERT(crs);
    }
    auto res = d->context()->getPrivate()->run(
        "SELECT type FROM crs WHERE auth_name = ? AND code = ?",
        {getAuthority(), code});
    if (res.empty()) {
        throw NoSuchAuthorityCodeException("crs not found", getAuthority(),
                                           code);
    }
    const auto &type = res[0][0];
    if (type == "geographic 2D" || type == "geographic 3D" ||
        type == "geocentric") {
        return createGeodeticCRS(code);
    }
    if (type == "vertical") {
        return createVerticalCRS(code);
    }
    if (type == "projected") {
        return createProjectedCRS(code);
    }
    if (allowCompound && type == "compound") {
        return createCompoundCRS(code);
    }
    throw FactoryException("unhandled CRS type: " + type);
}

// ---------------------------------------------------------------------------

/** \brief Returns a operation::CoordinateOperation from the specified code.
 *
 * @param code Object code allocated by authority.
 * @return object.
 * @throw NoSuchAuthorityCodeException
 * @throw FactoryException
 */

operation::CoordinateOperationNNPtr
AuthorityFactory::createCoordinateOperation(const std::string &code) const {
    return createCoordinateOperation(code, true);
}

operation::CoordinateOperationNNPtr
AuthorityFactory::createCoordinateOperation(const std::string &code,
                                            bool allowConcatenated) const {
    auto res =
        d->context()->getPrivate()->run("SELECT type FROM coordinate_operation "
                                        "WHERE auth_name = ? AND code = ?",
                                        {getAuthority(), code});
    if (res.empty()) {
        throw NoSuchAuthorityCodeException("coordinate operation not found",
                                           getAuthority(), code);
    }
    const auto type = res[0][0];
    if (type == "conversion") {
        return createConversion(code);
    }

    if (type == "helmert_transformation") {

#include "helmert_constants.hpp"

        res = d->context()->getPrivate()->run(
            "SELECT name, method_auth_name, method_code, method_name, "
            "source_crs_auth_name, source_crs_code, target_crs_auth_name, "
            "target_crs_code, area_of_use_auth_name, area_of_use_code, "
            "accuracy, tx, ty, tz, translation_uom_auth_name, "
            "translation_uom_code, rx, ry, rz, rotation_uom_auth_name, "
            "rotation_uom_code, scale_difference, "
            "scale_difference_uom_auth_name, scale_difference_uom_code, "
            "rate_tx, rate_ty, rate_tz, rate_translation_uom_auth_name, "
            "rate_translation_uom_code, rate_rx, rate_ry, rate_rz, "
            "rate_rotation_uom_auth_name, rate_rotation_uom_code, "
            "rate_scale_difference, rate_scale_difference_uom_auth_name, "
            "rate_scale_difference_uom_code, epoch, epoch_uom_auth_name, "
            "epoch_uom_code, deprecated FROM "
            "helmert_transformation WHERE auth_name = ? AND code = ?",
            {getAuthority(), code});
        if (res.empty()) {
            // shouldn't happen if foreign keys are OK
            throw NoSuchAuthorityCodeException(
                "helmert_transformation not found", getAuthority(), code);
        }
        try {
            const auto &row = res[0];
            size_t idx = 0;
            const auto &name = row[idx++];
            const auto &method_auth_name = row[idx++];
            const auto &method_code = row[idx++];
            const auto &method_name = row[idx++];
            const auto &source_crs_auth_name = row[idx++];
            const auto &source_crs_code = row[idx++];
            const auto &target_crs_auth_name = row[idx++];
            const auto &target_crs_code = row[idx++];
            const auto &area_of_use_auth_name = row[idx++];
            const auto &area_of_use_code = row[idx++];
            const auto &accuracy = row[idx++];

            const auto &tx = row[idx++];
            const auto &ty = row[idx++];
            const auto &tz = row[idx++];
            const auto &translation_uom_auth_name = row[idx++];
            const auto &translation_uom_code = row[idx++];
            const auto &rx = row[idx++];
            const auto &ry = row[idx++];
            const auto &rz = row[idx++];
            const auto &rotation_uom_auth_name = row[idx++];
            const auto &rotation_uom_code = row[idx++];
            const auto &scale_difference = row[idx++];
            const auto &scale_difference_uom_auth_name = row[idx++];
            const auto &scale_difference_uom_code = row[idx++];

            const auto &rate_tx = row[idx++];
            const auto &rate_ty = row[idx++];
            const auto &rate_tz = row[idx++];
            const auto &rate_translation_uom_auth_name = row[idx++];
            const auto &rate_translation_uom_code = row[idx++];
            const auto &rate_rx = row[idx++];
            const auto &rate_ry = row[idx++];
            const auto &rate_rz = row[idx++];
            const auto &rate_rotation_uom_auth_name = row[idx++];
            const auto &rate_rotation_uom_code = row[idx++];
            const auto &rate_scale_difference = row[idx++];
            const auto &rate_scale_difference_uom_auth_name = row[idx++];
            const auto &rate_scale_difference_uom_code = row[idx++];

            const auto &epoch = row[idx++];
            const auto &epoch_uom_auth_name = row[idx++];
            const auto &epoch_uom_code = row[idx++];

            const auto &deprecated_str = row[idx++];
            const bool deprecated = deprecated_str == "1";
            assert(idx == row.size());

            auto uom_translation =
                *d->createFactory(translation_uom_auth_name)
                     ->createUnitOfMeasure(translation_uom_code);
            auto uom_rotation =
                rotation_uom_auth_name.empty()
                    ? common::UnitOfMeasure::NONE
                    : *d->createFactory(rotation_uom_auth_name)
                           ->createUnitOfMeasure(rotation_uom_code);
            auto uom_scale_difference =
                scale_difference_uom_auth_name.empty()
                    ? common::UnitOfMeasure::NONE
                    : *d->createFactory(scale_difference_uom_auth_name)
                           ->createUnitOfMeasure(scale_difference_uom_code);
            auto uom_rate_translation =
                rate_translation_uom_auth_name.empty()
                    ? common::UnitOfMeasure::NONE
                    : *d->createFactory(rate_translation_uom_auth_name)
                           ->createUnitOfMeasure(rate_translation_uom_code);
            auto uom_rate_rotation =
                rate_rotation_uom_auth_name.empty()
                    ? common::UnitOfMeasure::NONE
                    : *d->createFactory(rate_rotation_uom_auth_name)
                           ->createUnitOfMeasure(rate_rotation_uom_code);
            auto uom_rate_scale_difference =
                rate_scale_difference_uom_auth_name.empty()
                    ? common::UnitOfMeasure::NONE
                    : *d->createFactory(rate_scale_difference_uom_auth_name)
                           ->createUnitOfMeasure(
                               rate_scale_difference_uom_code);
            auto uom_epoch = epoch_uom_auth_name.empty()
                                 ? common::UnitOfMeasure::NONE
                                 : *d->createFactory(epoch_uom_auth_name)
                                        ->createUnitOfMeasure(epoch_uom_code);

            auto sourceCRS =
                d->createFactory(source_crs_auth_name)
                    ->createCoordinateReferenceSystem(source_crs_code);
            auto targetCRS =
                d->createFactory(target_crs_auth_name)
                    ->createCoordinateReferenceSystem(target_crs_code);
            auto extent = d->createFactory(area_of_use_auth_name)
                              ->createExtent(area_of_use_code);

            std::vector<operation::OperationParameterNNPtr> parameters;
            std::vector<operation::ParameterValueNNPtr> values;

            parameters.emplace_back(operation::OperationParameter::create(
                util::PropertyMap()
                    .set(common::IdentifiedObject::NAME_KEY,
                         EPSG_NAME_PARAMETER_X_AXIS_TRANSLATION)
                    .set(metadata::Identifier::CODESPACE_KEY,
                         metadata::Identifier::EPSG)
                    .set(metadata::Identifier::CODE_KEY,
                         EPSG_CODE_PARAMETER_X_AXIS_TRANSLATION)));
            values.emplace_back(operation::ParameterValue::create(
                common::Length(c_locale_stod(tx), uom_translation)));

            parameters.emplace_back(operation::OperationParameter::create(
                util::PropertyMap()
                    .set(common::IdentifiedObject::NAME_KEY,
                         EPSG_NAME_PARAMETER_Y_AXIS_TRANSLATION)
                    .set(metadata::Identifier::CODESPACE_KEY,
                         metadata::Identifier::EPSG)
                    .set(metadata::Identifier::CODE_KEY,
                         EPSG_CODE_PARAMETER_Y_AXIS_TRANSLATION)));
            values.emplace_back(operation::ParameterValue::create(
                common::Length(c_locale_stod(ty), uom_translation)));

            parameters.emplace_back(operation::OperationParameter::create(
                util::PropertyMap()
                    .set(common::IdentifiedObject::NAME_KEY,
                         EPSG_NAME_PARAMETER_Z_AXIS_TRANSLATION)
                    .set(metadata::Identifier::CODESPACE_KEY,
                         metadata::Identifier::EPSG)
                    .set(metadata::Identifier::CODE_KEY,
                         EPSG_CODE_PARAMETER_Z_AXIS_TRANSLATION)));
            values.emplace_back(operation::ParameterValue::create(
                common::Length(c_locale_stod(tz), uom_translation)));

            if (uom_rotation != common::UnitOfMeasure::NONE) {
                parameters.emplace_back(operation::OperationParameter::create(
                    util::PropertyMap()
                        .set(common::IdentifiedObject::NAME_KEY,
                             EPSG_NAME_PARAMETER_X_AXIS_ROTATION)
                        .set(metadata::Identifier::CODESPACE_KEY,
                             metadata::Identifier::EPSG)
                        .set(metadata::Identifier::CODE_KEY,
                             EPSG_CODE_PARAMETER_X_AXIS_ROTATION)));
                values.emplace_back(operation::ParameterValue::create(
                    common::Angle(c_locale_stod(rx), uom_rotation)));

                parameters.emplace_back(operation::OperationParameter::create(
                    util::PropertyMap()
                        .set(common::IdentifiedObject::NAME_KEY,
                             EPSG_NAME_PARAMETER_Y_AXIS_ROTATION)
                        .set(metadata::Identifier::CODESPACE_KEY,
                             metadata::Identifier::EPSG)
                        .set(metadata::Identifier::CODE_KEY,
                             EPSG_CODE_PARAMETER_Y_AXIS_ROTATION)));
                values.emplace_back(operation::ParameterValue::create(
                    common::Angle(c_locale_stod(ry), uom_rotation)));

                parameters.emplace_back(operation::OperationParameter::create(
                    util::PropertyMap()
                        .set(common::IdentifiedObject::NAME_KEY,
                             EPSG_NAME_PARAMETER_Z_AXIS_ROTATION)
                        .set(metadata::Identifier::CODESPACE_KEY,
                             metadata::Identifier::EPSG)
                        .set(metadata::Identifier::CODE_KEY,
                             EPSG_CODE_PARAMETER_Z_AXIS_ROTATION)));
                values.emplace_back(operation::ParameterValue::create(
                    common::Angle(c_locale_stod(rz), uom_rotation)));

                parameters.emplace_back(operation::OperationParameter::create(
                    util::PropertyMap()
                        .set(common::IdentifiedObject::NAME_KEY,
                             EPSG_NAME_PARAMETER_SCALE_DIFFERENCE)
                        .set(metadata::Identifier::CODESPACE_KEY,
                             metadata::Identifier::EPSG)
                        .set(metadata::Identifier::CODE_KEY,
                             EPSG_CODE_PARAMETER_SCALE_DIFFERENCE)));
                values.emplace_back(operation::ParameterValue::create(
                    common::Scale(c_locale_stod(scale_difference),
                                  uom_scale_difference)));
            }

            if (uom_rate_translation != common::UnitOfMeasure::NONE) {
                parameters.emplace_back(operation::OperationParameter::create(
                    util::PropertyMap()
                        .set(common::IdentifiedObject::NAME_KEY,
                             EPSG_NAME_PARAMETER_RATE_X_AXIS_TRANSLATION)
                        .set(metadata::Identifier::CODESPACE_KEY,
                             metadata::Identifier::EPSG)
                        .set(metadata::Identifier::CODE_KEY,
                             EPSG_CODE_PARAMETER_RATE_X_AXIS_TRANSLATION)));
                values.emplace_back(
                    operation::ParameterValue::create(common::Length(
                        c_locale_stod(rate_tx), uom_rate_translation)));

                parameters.emplace_back(operation::OperationParameter::create(
                    util::PropertyMap()
                        .set(common::IdentifiedObject::NAME_KEY,
                             EPSG_NAME_PARAMETER_RATE_Y_AXIS_TRANSLATION)
                        .set(metadata::Identifier::CODESPACE_KEY,
                             metadata::Identifier::EPSG)
                        .set(metadata::Identifier::CODE_KEY,
                             EPSG_CODE_PARAMETER_RATE_Y_AXIS_TRANSLATION)));
                values.emplace_back(
                    operation::ParameterValue::create(common::Length(
                        c_locale_stod(rate_ty), uom_rate_translation)));

                parameters.emplace_back(operation::OperationParameter::create(
                    util::PropertyMap()
                        .set(common::IdentifiedObject::NAME_KEY,
                             EPSG_NAME_PARAMETER_RATE_Z_AXIS_TRANSLATION)
                        .set(metadata::Identifier::CODESPACE_KEY,
                             metadata::Identifier::EPSG)
                        .set(metadata::Identifier::CODE_KEY,
                             EPSG_CODE_PARAMETER_RATE_Z_AXIS_TRANSLATION)));
                values.emplace_back(
                    operation::ParameterValue::create(common::Length(
                        c_locale_stod(rate_tz), uom_rate_translation)));

                parameters.emplace_back(operation::OperationParameter::create(
                    util::PropertyMap()
                        .set(common::IdentifiedObject::NAME_KEY,
                             EPSG_NAME_PARAMETER_RATE_X_AXIS_ROTATION)
                        .set(metadata::Identifier::CODESPACE_KEY,
                             metadata::Identifier::EPSG)
                        .set(metadata::Identifier::CODE_KEY,
                             EPSG_CODE_PARAMETER_RATE_X_AXIS_ROTATION)));
                values.emplace_back(operation::ParameterValue::create(
                    common::Angle(c_locale_stod(rate_rx), uom_rate_rotation)));

                parameters.emplace_back(operation::OperationParameter::create(
                    util::PropertyMap()
                        .set(common::IdentifiedObject::NAME_KEY,
                             EPSG_NAME_PARAMETER_RATE_Y_AXIS_ROTATION)
                        .set(metadata::Identifier::CODESPACE_KEY,
                             metadata::Identifier::EPSG)
                        .set(metadata::Identifier::CODE_KEY,
                             EPSG_CODE_PARAMETER_RATE_Y_AXIS_ROTATION)));
                values.emplace_back(operation::ParameterValue::create(
                    common::Angle(c_locale_stod(rate_ry), uom_rate_rotation)));

                parameters.emplace_back(operation::OperationParameter::create(
                    util::PropertyMap()
                        .set(common::IdentifiedObject::NAME_KEY,
                             EPSG_NAME_PARAMETER_RATE_Z_AXIS_ROTATION)
                        .set(metadata::Identifier::CODESPACE_KEY,
                             metadata::Identifier::EPSG)
                        .set(metadata::Identifier::CODE_KEY,
                             EPSG_CODE_PARAMETER_RATE_Z_AXIS_ROTATION)));
                values.emplace_back(operation::ParameterValue::create(
                    common::Angle(c_locale_stod(rate_rz), uom_rate_rotation)));

                parameters.emplace_back(operation::OperationParameter::create(
                    util::PropertyMap()
                        .set(common::IdentifiedObject::NAME_KEY,
                             EPSG_NAME_PARAMETER_RATE_SCALE_DIFFERENCE)
                        .set(metadata::Identifier::CODESPACE_KEY,
                             metadata::Identifier::EPSG)
                        .set(metadata::Identifier::CODE_KEY,
                             EPSG_CODE_PARAMETER_RATE_SCALE_DIFFERENCE)));
                values.emplace_back(operation::ParameterValue::create(
                    common::Scale(c_locale_stod(rate_scale_difference),
                                  uom_rate_scale_difference)));

                parameters.emplace_back(operation::OperationParameter::create(
                    util::PropertyMap()
                        .set(common::IdentifiedObject::NAME_KEY,
                             EPSG_NAME_PARAMETER_REFERENCE_EPOCH)
                        .set(metadata::Identifier::CODESPACE_KEY,
                             metadata::Identifier::EPSG)
                        .set(metadata::Identifier::CODE_KEY,
                             EPSG_CODE_PARAMETER_REFERENCE_EPOCH)));
                values.emplace_back(operation::ParameterValue::create(
                    common::Measure(c_locale_stod(epoch), uom_epoch)));
            } else if (uom_epoch != common::UnitOfMeasure::NONE) {
                constexpr int
                    EPSG_CODE_PARAMETER_TRANSFORMATION_REFERENCE_EPOCH = 1049;
                static const std::string
                    EPSG_NAME_PARAMETER_TRANSFORMATION_REFERENCE_EPOCH(
                        "Transformation reference epoch");

                parameters.emplace_back(operation::OperationParameter::create(
                    util::PropertyMap()
                        .set(common::IdentifiedObject::NAME_KEY,
                             EPSG_NAME_PARAMETER_TRANSFORMATION_REFERENCE_EPOCH)
                        .set(metadata::Identifier::CODESPACE_KEY,
                             metadata::Identifier::EPSG)
                        .set(
                            metadata::Identifier::CODE_KEY,
                            EPSG_CODE_PARAMETER_TRANSFORMATION_REFERENCE_EPOCH)));
                values.emplace_back(operation::ParameterValue::create(
                    common::Measure(c_locale_stod(epoch), uom_epoch)));
            }

            auto props =
                util::PropertyMap()
                    .set(metadata::Identifier::CODESPACE_KEY, getAuthority())
                    .set(metadata::Identifier::CODE_KEY, code)
                    .set(common::IdentifiedObject::NAME_KEY, name)
                    .set(common::IdentifiedObject::DEPRECATED_KEY, deprecated)
                    .set(common::ObjectUsage::DOMAIN_OF_VALIDITY_KEY, extent);

            auto propsMethod =
                util::PropertyMap()
                    .set(metadata::Identifier::CODESPACE_KEY, method_auth_name)
                    .set(metadata::Identifier::CODE_KEY, method_code)
                    .set(common::IdentifiedObject::NAME_KEY, method_name);

            std::vector<metadata::PositionalAccuracyNNPtr> accuracies;
            if (!accuracy.empty()) {
                accuracies.emplace_back(
                    metadata::PositionalAccuracy::create(accuracy));
            }
            return operation::Transformation::create(
                props, sourceCRS, targetCRS, nullptr, propsMethod, parameters,
                values, accuracies);

        } catch (const std::exception &ex) {
            throw FactoryException("cannot build transformation " + code +
                                   ": " + ex.what());
        }
    }

    if (type == "grid_transformation") {
        res = d->context()->getPrivate()->run(
            "SELECT name, method_auth_name, method_code, method_name, "
            "source_crs_auth_name, source_crs_code, target_crs_auth_name, "
            "target_crs_code, area_of_use_auth_name, area_of_use_code, "
            "accuracy, grid_param_auth_name, grid_param_code, grid_param_name, "
            "grid_name, "
            "grid2_param_auth_name, grid2_param_code, grid2_param_name, "
            "grid2_name, "
            "interpolation_crs_auth_name, interpolation_crs_code, deprecated "
            "FROM "
            "grid_transformation WHERE auth_name = ? AND code = ?",
            {getAuthority(), code});
        if (res.empty()) {
            // shouldn't happen if foreign keys are OK
            throw NoSuchAuthorityCodeException("grid_transformation not found",
                                               getAuthority(), code);
        }
        try {
            const auto &row = res[0];
            size_t idx = 0;
            const auto &name = row[idx++];
            const auto &method_auth_name = row[idx++];
            const auto &method_code = row[idx++];
            const auto &method_name = row[idx++];
            const auto &source_crs_auth_name = row[idx++];
            const auto &source_crs_code = row[idx++];
            const auto &target_crs_auth_name = row[idx++];
            const auto &target_crs_code = row[idx++];
            const auto &area_of_use_auth_name = row[idx++];
            const auto &area_of_use_code = row[idx++];
            const auto &accuracy = row[idx++];
            const auto &grid_param_auth_name = row[idx++];
            const auto &grid_param_code = row[idx++];
            const auto &grid_param_name = row[idx++];
            const auto &grid_name = row[idx++];
            const auto &grid2_param_auth_name = row[idx++];
            const auto &grid2_param_code = row[idx++];
            const auto &grid2_param_name = row[idx++];
            const auto &grid2_name = row[idx++];
            const auto &interpolation_crs_auth_name = row[idx++];
            const auto &interpolation_crs_code = row[idx++];

            const auto &deprecated_str = row[idx++];
            const bool deprecated = deprecated_str == "1";
            assert(idx == row.size());

            auto sourceCRS =
                d->createFactory(source_crs_auth_name)
                    ->createCoordinateReferenceSystem(source_crs_code);
            auto targetCRS =
                d->createFactory(target_crs_auth_name)
                    ->createCoordinateReferenceSystem(target_crs_code);
            auto interpolationCRS =
                interpolation_crs_auth_name.empty()
                    ? nullptr
                    : d->createFactory(interpolation_crs_auth_name)
                          ->createCoordinateReferenceSystem(
                              interpolation_crs_code)
                          .as_nullable();
            auto extent = d->createFactory(area_of_use_auth_name)
                              ->createExtent(area_of_use_code);

            std::vector<operation::OperationParameterNNPtr> parameters;
            std::vector<operation::ParameterValueNNPtr> values;

            parameters.emplace_back(operation::OperationParameter::create(
                util::PropertyMap()
                    .set(common::IdentifiedObject::NAME_KEY, grid_param_name)
                    .set(metadata::Identifier::CODESPACE_KEY,
                         grid_param_auth_name)
                    .set(metadata::Identifier::CODE_KEY, grid_param_code)));
            values.emplace_back(
                operation::ParameterValue::createFilename(grid_name));
            if (!grid2_name.empty()) {
                parameters.emplace_back(operation::OperationParameter::create(
                    util::PropertyMap()
                        .set(common::IdentifiedObject::NAME_KEY,
                             grid2_param_name)
                        .set(metadata::Identifier::CODESPACE_KEY,
                             grid2_param_auth_name)
                        .set(metadata::Identifier::CODE_KEY,
                             grid2_param_code)));
                values.emplace_back(
                    operation::ParameterValue::createFilename(grid2_name));
            }

            auto props =
                util::PropertyMap()
                    .set(metadata::Identifier::CODESPACE_KEY, getAuthority())
                    .set(metadata::Identifier::CODE_KEY, code)
                    .set(common::IdentifiedObject::NAME_KEY, name)
                    .set(common::IdentifiedObject::DEPRECATED_KEY, deprecated)
                    .set(common::ObjectUsage::DOMAIN_OF_VALIDITY_KEY, extent);

            auto propsMethod =
                util::PropertyMap()
                    .set(metadata::Identifier::CODESPACE_KEY, method_auth_name)
                    .set(metadata::Identifier::CODE_KEY, method_code)
                    .set(common::IdentifiedObject::NAME_KEY, method_name);

            std::vector<metadata::PositionalAccuracyNNPtr> accuracies;
            if (!accuracy.empty()) {
                accuracies.emplace_back(
                    metadata::PositionalAccuracy::create(accuracy));
            }
            return operation::Transformation::create(
                props, sourceCRS, targetCRS, interpolationCRS, propsMethod,
                parameters, values, accuracies);

        } catch (const std::exception &ex) {
            throw FactoryException("cannot build transformation " + code +
                                   ": " + ex.what());
        }
    }

    if (type == "other_transformation") {
        std::ostringstream buffer;
        buffer.imbue(std::locale::classic());
        buffer
            << "SELECT name, method_auth_name, method_code, method_name, "
               "source_crs_auth_name, source_crs_code, target_crs_auth_name, "
               "target_crs_code, area_of_use_auth_name, area_of_use_code, "
               "accuracy";
        constexpr int N_MAX_PARAMS = 7;
        for (int i = 1; i <= N_MAX_PARAMS; ++i) {
            buffer << ", param" << i << "_auth_name";
            buffer << ", param" << i << "_code";
            buffer << ", param" << i << "_name";
            buffer << ", param" << i << "_value";
            buffer << ", param" << i << "_uom_auth_name";
            buffer << ", param" << i << "_uom_code";
        }
        buffer << ", deprecated FROM other_transformation WHERE auth_name = ? "
                  "AND code = ?";

        res = d->context()->getPrivate()->run(buffer.str(),
                                              {getAuthority(), code});
        if (res.empty()) {
            // shouldn't happen if foreign keys are OK
            throw NoSuchAuthorityCodeException("other_transformation not found",
                                               getAuthority(), code);
        }
        try {
            const auto &row = res[0];
            size_t idx = 0;
            const auto &name = row[idx++];
            const auto &method_auth_name = row[idx++];
            const auto &method_code = row[idx++];
            const auto &method_name = row[idx++];
            const auto &source_crs_auth_name = row[idx++];
            const auto &source_crs_code = row[idx++];
            const auto &target_crs_auth_name = row[idx++];
            const auto &target_crs_code = row[idx++];
            const auto &area_of_use_auth_name = row[idx++];
            const auto &area_of_use_code = row[idx++];
            const auto &accuracy = row[idx++];

            const size_t base_param_idx = idx;
            std::vector<operation::OperationParameterNNPtr> parameters;
            std::vector<operation::ParameterValueNNPtr> values;
            for (int i = 0; i < N_MAX_PARAMS; ++i) {
                const auto &param_auth_name = row[base_param_idx + i * 6 + 0];
                if (param_auth_name.empty()) {
                    break;
                }
                const auto &param_code = row[base_param_idx + i * 6 + 1];
                const auto &param_name = row[base_param_idx + i * 6 + 2];
                const auto &param_value = row[base_param_idx + i * 6 + 3];
                const auto &param_uom_auth_name =
                    row[base_param_idx + i * 6 + 4];
                const auto &param_uom_code = row[base_param_idx + i * 6 + 5];
                parameters.emplace_back(operation::OperationParameter::create(
                    util::PropertyMap()
                        .set(metadata::Identifier::CODESPACE_KEY,
                             param_auth_name)
                        .set(metadata::Identifier::CODE_KEY, param_code)
                        .set(common::IdentifiedObject::NAME_KEY, param_name)));
                std::string normalized_uom_code(param_uom_code);
                double normalized_value(c_locale_stod(param_value));
                normalizeMeasure(param_uom_code, param_value,
                                 normalized_uom_code, normalized_value);
                auto uom = d->createFactory(param_uom_auth_name)
                               ->createUnitOfMeasure(normalized_uom_code);
                values.emplace_back(operation::ParameterValue::create(
                    common::Measure(normalized_value, *uom)));
            }
            idx = base_param_idx + 6 * N_MAX_PARAMS;

            const auto &deprecated_str = row[idx++];
            const bool deprecated = deprecated_str == "1";
            assert(idx == row.size());

            auto sourceCRS =
                d->createFactory(source_crs_auth_name)
                    ->createCoordinateReferenceSystem(source_crs_code);
            auto targetCRS =
                d->createFactory(target_crs_auth_name)
                    ->createCoordinateReferenceSystem(target_crs_code);

            auto extent = d->createFactory(area_of_use_auth_name)
                              ->createExtent(area_of_use_code);

            auto props =
                util::PropertyMap()
                    .set(metadata::Identifier::CODESPACE_KEY, getAuthority())
                    .set(metadata::Identifier::CODE_KEY, code)
                    .set(common::IdentifiedObject::NAME_KEY, name)
                    .set(common::IdentifiedObject::DEPRECATED_KEY, deprecated)
                    .set(common::ObjectUsage::DOMAIN_OF_VALIDITY_KEY, extent);

            auto propsMethod =
                util::PropertyMap()
                    .set(metadata::Identifier::CODESPACE_KEY, method_auth_name)
                    .set(metadata::Identifier::CODE_KEY, method_code)
                    .set(common::IdentifiedObject::NAME_KEY, method_name);

            std::vector<metadata::PositionalAccuracyNNPtr> accuracies;
            if (!accuracy.empty()) {
                accuracies.emplace_back(
                    metadata::PositionalAccuracy::create(accuracy));
            }
            return operation::Transformation::create(
                props, sourceCRS, targetCRS, nullptr, propsMethod, parameters,
                values, accuracies);

        } catch (const std::exception &ex) {
            throw FactoryException("cannot build transformation " + code +
                                   ": " + ex.what());
        }
    }

    if (allowConcatenated && type == "concatenated_operation") {
        res = d->context()->getPrivate()->run(
            "SELECT name, area_of_use_auth_name, area_of_use_code, accuracy, "
            "step1_auth_name, step1_code, step2_auth_name, step2_code, "
            "step3_auth_name, step3_code, deprecated FROM "
            "concatenated_operation WHERE auth_name = ? AND code = ?",
            {getAuthority(), code});
        if (res.empty()) {
            // shouldn't happen if foreign keys are OK
            throw NoSuchAuthorityCodeException(
                "concatenated_operation not found", getAuthority(), code);
        }
        try {
            const auto &row = res[0];
            size_t idx = 0;
            const auto &name = row[idx++];
            const auto &area_of_use_auth_name = row[idx++];
            const auto &area_of_use_code = row[idx++];
            const auto &accuracy = row[idx++];
            const auto &step1_auth_name = row[idx++];
            const auto &step1_code = row[idx++];
            const auto &step2_auth_name = row[idx++];
            const auto &step2_code = row[idx++];
            const auto &step3_auth_name = row[idx++];
            const auto &step3_code = row[idx++];
            const auto &deprecated_str = row[idx++];
            const bool deprecated = deprecated_str == "1";

            std::vector<operation::CoordinateOperationNNPtr> operations;
            operations.push_back(
                d->createFactory(step1_auth_name)
                    ->createCoordinateOperation(step1_code, false));
            operations.push_back(
                d->createFactory(step2_auth_name)
                    ->createCoordinateOperation(step2_code, false));
            if (!step3_auth_name.empty()) {
                operations.push_back(
                    d->createFactory(step3_auth_name)
                        ->createCoordinateOperation(step3_code, false));
            }

            auto extent = d->createFactory(area_of_use_auth_name)
                              ->createExtent(area_of_use_code);

            auto props =
                util::PropertyMap()
                    .set(metadata::Identifier::CODESPACE_KEY, getAuthority())
                    .set(metadata::Identifier::CODE_KEY, code)
                    .set(common::IdentifiedObject::NAME_KEY, name)
                    .set(common::IdentifiedObject::DEPRECATED_KEY, deprecated)
                    .set(common::ObjectUsage::DOMAIN_OF_VALIDITY_KEY, extent);

            std::vector<metadata::PositionalAccuracyNNPtr> accuracies;
            if (!accuracy.empty()) {
                accuracies.emplace_back(
                    metadata::PositionalAccuracy::create(accuracy));
            }
            return operation::ConcatenatedOperation::create(props, operations,
                                                            accuracies);

        } catch (const std::exception &ex) {
            throw FactoryException("cannot build transformation " + code +
                                   ": " + ex.what());
        }
    }

    throw FactoryException("unhandled coordinate operation type: " + type);
}

// ---------------------------------------------------------------------------

/** \brief Returns a list operation::CoordinateOperation between two CRS.
 *
 * The list is ordered with preferred operations first. No attempt is made
 * at infering operations that are not explicitly in the database.
 *
 * Deprecated operations are rejected.
 *
 * @param sourceCRSCode Source CRS code allocated by authority.
 * @param targetCRSCode Source CRS code allocated by authority.
 * @return list of coordinate operations
 * @throw NoSuchAuthorityCodeException
 * @throw FactoryException
 */

std::vector<operation::CoordinateOperationNNPtr>
AuthorityFactory::createFromCoordinateReferenceSystemCodes(
    const std::string &sourceCRSCode, const std::string &targetCRSCode) const {
    return createFromCoordinateReferenceSystemCodes(
        getAuthority(), sourceCRSCode, getAuthority(), targetCRSCode);
}

// ---------------------------------------------------------------------------

/** \brief Returns a list operation::CoordinateOperation between two CRS.
 *
 * The list is ordered with preferred operations first. No attempt is made
 * at infering operations that are not explicitly in the database.
 *
 * Deprecated operations are rejected.
 *
 * If getAuthority() returns empty, then coordinate operations from all
 * authorities are considered.
 *
 * @param sourceCRSAuthName Authority name of sourceCRSCode
 * @param sourceCRSCode Source CRS code allocated by authority
 * sourceCRSAuthName.
 * @param targetCRSAuthName Authority name of targetCRSCode
 * @param targetCRSCode Source CRS code allocated by authority
 * targetCRSAuthName.
 * @return list of coordinate operations
 * @throw NoSuchAuthorityCodeException
 * @throw FactoryException
 */

std::vector<operation::CoordinateOperationNNPtr>
AuthorityFactory::createFromCoordinateReferenceSystemCodes(
    const std::string &sourceCRSAuthName, const std::string &sourceCRSCode,
    const std::string &targetCRSAuthName,
    const std::string &targetCRSCode) const {
    std::vector<operation::CoordinateOperationNNPtr> list;

    // Look-up first for conversion which is the most precise.
    std::string sql(
        "SELECT conversion_auth_name, conversion_code FROM "
        "projected_crs WHERE geodetic_crs_auth_name = ? AND geodetic_crs_code "
        "= ? AND auth_name = ? AND code = ? AND deprecated != 1");
    auto params = std::vector<SQLValues>{sourceCRSAuthName, sourceCRSCode,
                                         targetCRSAuthName, targetCRSCode};
    if (!getAuthority().empty()) {
        sql += " AND conversion_auth_name = ?";
        params.emplace_back(getAuthority());
    }
    auto res = d->context()->getPrivate()->run(sql, params);
    if (!res.empty()) {
        auto targetCRS = d->createFactory(targetCRSAuthName)
                             ->createProjectedCRS(targetCRSCode);
        auto conv = targetCRS->derivingConversion();
        list.emplace_back(conv);
        return list;
    }
    sql =
        "SELECT cov.auth_name, cov.code FROM "
        "coordinate_operation_view cov JOIN area ON cov.area_of_use_auth_name "
        "= area.auth_name AND cov.area_of_use_code = area.code WHERE "
        "source_crs_auth_name = ? AND source_crs_code = ? AND "
        "target_crs_auth_name = ? AND target_crs_code = ? AND "
        "cov.deprecated != 1";
    params = {sourceCRSAuthName, sourceCRSCode, targetCRSAuthName,
              targetCRSCode};
    if (!getAuthority().empty()) {
        sql += " AND cov.auth_name = ?";
        params.emplace_back(getAuthority());
    }
    sql += " ORDER BY pseudo_area_from_swne(south_lat, west_lon, north_lat, "
           "east_lon) DESC, "
           "(CASE WHEN accuracy is NULL THEN 1 ELSE 0 END), accuracy";
    res = d->context()->getPrivate()->run(sql, params);
    for (const auto &row : res) {
        const auto &auth_name = row[0];
        const auto &code = row[1];
        list.emplace_back(
            d->createFactory(auth_name)->createCoordinateOperation(code));
    }
    return list;
}

// ---------------------------------------------------------------------------

/** \brief Returns the authority name associated to this factory.
 * @return name.
 */
const std::string &AuthorityFactory::getAuthority() const {
    return d->authority();
}

// ---------------------------------------------------------------------------

/** \brief Returns the set of authority codes of the given object type.
 *
 * @param type Objec type.
 * @return the set of authority codes for spatial reference objects of the given
 * type
 * @throw FactoryException
 */
std::set<std::string>
AuthorityFactory::getAuthorityCodes(const ObjectType &type) const {
    std::string sql;
    switch (type) {
    case ObjectType::PRIME_MERIDIAN:
        sql = "SELECT code FROM prime_meridian WHERE ";
        break;
    case ObjectType::ELLIPSOID:
        sql = "SELECT code FROM ellipsoid WHERE ";
        break;
    case ObjectType::DATUM:
        sql = "SELECT code FROM object_view WHERE table_name IN "
              "('geodetic_datum', 'vertical_datum') AND ";
        break;
    case ObjectType::GEODETIC_REFERENCE_FRAME:
        sql = "SELECT code FROM geodetic_datum WHERE ";
        break;
    case ObjectType::VERTICAL_REFERENCE_FRAME:
        sql = "SELECT code FROM vertical_datum WHERE ";
        break;
    case ObjectType::CRS:
        sql = "SELECT code FROM crs WHERE ";
        break;
    case ObjectType::GEODETIC_CRS:
        sql = "SELECT code FROM geodetic_crs WHERE ";
        break;
    case ObjectType::GEOGRAPHIC_CRS:
        sql = "SELECT code FROM geodetic_crs WHERE type IN ('geographic 2D', "
              "'geographic 3D') AND ";
        break;
    case ObjectType::VERTICAL_CRS:
        sql = "SELECT code FROM vertical_crs WHERE ";
        break;
    case ObjectType::PROJECTED_CRS:
        sql = "SELECT code FROM projected_crs WHERE ";
        break;
    case ObjectType::COMPOUND_CRS:
        sql = "SELECT code FROM compound_crs WHERE ";
        break;
    case ObjectType::COORDINATE_OPERATION:
        sql = "SELECT code FROM coordinate_operation WHERE ";
        break;
    case ObjectType::CONVERSION:
        sql = "SELECT code FROM conversion WHERE ";
        break;
    case ObjectType::TRANSFORMATION:
        sql = "SELECT code FROM coordinate_operation_view WHERE table_name != "
              "'concatenated_operation' AND ";
        break;
    case ObjectType::CONCATENATED_OPERATION:
        sql = "SELECT code FROM concatenated_operation WHERE ";
        break;
    }
    auto res = d->context()->getPrivate()->run(sql + "auth_name = ?",
                                               {getAuthority()});
    std::set<std::string> set;
    for (const auto &row : res) {
        set.insert(row[0]);
    }
    return set;
}

// ---------------------------------------------------------------------------

/** \brief Gets a description of the object corresponding to a code.
 *
 * \note In case of several objects of different types with the same code,
 * one of them will be arbitrarily selected.
 *
 * @param code Object code allocated by authority. (e.g. "4326")
 * @return description.
 * @throw NoSuchAuthorityCodeException
 * @throw FactoryException
 */
std::string
AuthorityFactory::getDescriptionText(const std::string &code) const {
    auto sql = "SELECT name FROM object_view WHERE auth_name = ? AND code = "
               "? ORDER BY table_name";
    auto res = d->context()->getPrivate()->run(sql, {getAuthority(), code});
    if (res.empty()) {
        throw NoSuchAuthorityCodeException("object not found", getAuthority(),
                                           code);
    }
    return res[0][0];
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
FactoryException::FactoryException(const char *message) : Exception(message) {}

// ---------------------------------------------------------------------------

FactoryException::FactoryException(const std::string &message)
    : Exception(message) {}

// ---------------------------------------------------------------------------

FactoryException::~FactoryException() = default;

// ---------------------------------------------------------------------------

FactoryException::FactoryException(const FactoryException &) = default;
//! @endcond

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress

struct NoSuchAuthorityCodeException::Private {
    std::string authority_;
    std::string code_;

    Private(const std::string &authority, const std::string &code)
        : authority_(authority), code_(code) {}
};

// ---------------------------------------------------------------------------

NoSuchAuthorityCodeException::NoSuchAuthorityCodeException(
    const std::string &message, const std::string &authority,
    const std::string &code)
    : FactoryException(message),
      d(internal::make_unique<Private>(authority, code)) {}

// ---------------------------------------------------------------------------

NoSuchAuthorityCodeException::~NoSuchAuthorityCodeException() = default;

// ---------------------------------------------------------------------------

NoSuchAuthorityCodeException::NoSuchAuthorityCodeException(
    const NoSuchAuthorityCodeException &other)
    : FactoryException(other), d(internal::make_unique<Private>(*(other.d))) {}
//! @endcond

// ---------------------------------------------------------------------------

/** \brief Returns authority name. */
const std::string &NoSuchAuthorityCodeException::getAuthority() const {
    return d->authority_;
}

// ---------------------------------------------------------------------------

/** \brief Returns authority code. */
const std::string &NoSuchAuthorityCodeException::getAuthorityCode() const {
    return d->code_;
}

// ---------------------------------------------------------------------------

} // namespace io
NS_PROJ_END
