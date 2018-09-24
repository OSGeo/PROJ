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
#include "proj/coordinatesystem.hpp"
#include "proj/crs.hpp"
#include "proj/datum.hpp"
#include "proj/internal.hpp"
#include "proj/io.hpp"
#include "proj/io_internal.hpp"
#include "proj/metadata.hpp"
#include "proj/util.hpp"

#include <cstdlib>
#include <memory>
#include <string>

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

    void open();
    SQLResultSet
    run(const std::string &sql,
        const std::vector<SQLValues> &parameters = std::vector<SQLValues>());

  private:
    sqlite3 *sqlite_handle_{};
};

// ---------------------------------------------------------------------------

DatabaseContext::Private::Private() = default;

// ---------------------------------------------------------------------------

DatabaseContext::Private::~Private() {
    //
    sqlite3_close(sqlite_handle_);
}

// ---------------------------------------------------------------------------

void DatabaseContext::Private::open() {
    const char *proj_lib = std::getenv("PROJ_LIB");
#ifdef PROJ_LIB
    if (!proj_lib) {
        proj_lib = PROJ_LIB;
    }
#endif
    if (!proj_lib) {
        throw FactoryException("Cannot find proj.db due to missing PROJ_LIB");
    }
    std::string path(std::string(proj_lib) + DIR_CHAR + "proj.db");
    if (sqlite3_open_v2(path.c_str(), &sqlite_handle_, SQLITE_OPEN_READONLY,
                        nullptr) != SQLITE_OK ||
        !sqlite_handle_) {
        throw FactoryException("Open of " + path + " failed");
    }
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
        throw FactoryException("unusedparameters in substitution list");
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

/** \brief Instanciate a database context.
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

//! @cond Doxygen_Suppress
struct AuthorityFactory::Private {
    Private(DatabaseContextNNPtr contextIn, const std::string &authorityName)
        : context_(contextIn), authority_(authorityName) {}

    const std::string &authority() const { return authority_; }
    DatabaseContextNNPtr context() const { return context_; }

  private:
    DatabaseContextNNPtr context_;
    std::string authority_;
};
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

/** \brief Instanciate a AuthorityFactory.
 *
 * @param context Contexte.
 * @param authorityName Authority name.
 * @return new AuthorityFactory.
 */
AuthorityFactoryNNPtr
AuthorityFactory::create(DatabaseContextNNPtr context,
                         const std::string &authorityName) {
    return AuthorityFactory::nn_make_shared<AuthorityFactory>(context,
                                                              authorityName);
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
    if (res[0][0] == "unit_of_measure") {
        return util::nn_static_pointer_cast<util::BaseObject>(
            createUnitOfMeasure(code));
    }
    if (res[0][0] == "prime_meridian") {
        return util::nn_static_pointer_cast<util::BaseObject>(
            createPrimeMeridian(code));
    }
    if (res[0][0] == "ellipsoid") {
        return util::nn_static_pointer_cast<util::BaseObject>(
            createEllipsoid(code));
    }
    if (res[0][0] == "geodetic_datum") {
        return util::nn_static_pointer_cast<util::BaseObject>(
            createGeodeticDatum(code));
    }
    if (res[0][0] == "vertical_datum") {
        return util::nn_static_pointer_cast<util::BaseObject>(
            createVerticalDatum(code));
    }
    if (res[0][0] == "geodetic_crs") {
        return util::nn_static_pointer_cast<util::BaseObject>(
            createGeodeticCRS(code));
    }
    throw FactoryException("unimplemented factory for" + res[0][0]);
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
        double longitude = c_locale_stod(row[1]);
        const auto &uom_auth_name = row[2];
        const auto &uom_code = row[3];
        const bool deprecated = row[4] == "1";
        auto uom =
            create(d->context(), uom_auth_name)->createUnitOfMeasure(uom_code);
        auto props =
            util::PropertyMap()
                .set(metadata::Identifier::CODESPACE_KEY, getAuthority())
                .set(metadata::Identifier::CODE_KEY, code)
                .set(common::IdentifiedObject::NAME_KEY, name)
                .set(common::IdentifiedObject::DEPRECATED_KEY, deprecated);
        return datum::PrimeMeridian::create(props,
                                            common::Angle(longitude, *uom));
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
            create(d->context(), uom_auth_name)->createUnitOfMeasure(uom_code);
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
        auto ellipsoid = create(d->context(), ellipsoid_auth_name)
                             ->createEllipsoid(ellipsoid_code);
        auto pm = create(d->context(), prime_meridian_auth_name)
                      ->createPrimeMeridian(prime_meridian_code);
        auto extent = create(d->context(), area_of_use_auth_name)
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
        auto extent = create(d->context(), area_of_use_auth_name)
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
            create(d->context(), uom_auth_name)->createUnitOfMeasure(uom_code);
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
    auto res = d->context()->getPrivate()->run(
        "SELECT name, type, coordinate_system_auth_name, "
        "coordinate_system_code, datum_auth_name, datum_code, "
        "area_of_use_auth_name, area_of_use_code, deprecated FROM "
        "geodetic_crs WHERE auth_name = ? AND code = ?",
        {getAuthority(), code});
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
            create(d->context(), cs_auth_name)->createCoordinateSystem(cs_code);
        auto datum = create(d->context(), datum_auth_name)
                         ->createGeodeticDatum(datum_code);
        auto extent = create(d->context(), area_of_use_auth_name)
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
            return crs::GeographicCRS::create(props, datum,
                                              NN_CHECK_ASSERT(ellipsoidalCS));
        }
        auto geocentricCS = util::nn_dynamic_pointer_cast<cs::CartesianCS>(cs);
        if (type == "geocentric" && geocentricCS) {
            return crs::GeodeticCRS::create(props, datum,
                                            NN_CHECK_ASSERT(geocentricCS));
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

/** \brief Returns the authority name associated to this factory.
 * @return name.
 */
const std::string &AuthorityFactory::getAuthority() const {
    return d->authority();
}

// ---------------------------------------------------------------------------

/** \brief Returns the set of authority codes of the given object type.
 *
 * @param type Among "CRS", "Datum", etc...
 * @return the set of authority codes for spatial reference objects of the given
 * type
 * @throw FactoryException
 */
std::set<std::string>
AuthorityFactory::getAuthorityCodes(const std::string &type) const {
    (void)type;
    // TODO
    return std::set<std::string>();
}

// ---------------------------------------------------------------------------

/** \brief Gets a description of the object corresponding to a code.
 * @param code Object code allocated by authority. (e.g. "4326")
 * @return description.
 * @throw NoSuchAuthorityCodeException
 * @throw FactoryException
 */
std::string
AuthorityFactory::getDescriptionText(const std::string &code) const {
    (void)code;
    // TODO
    return std::string();
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
