/******************************************************************************
 *
 * Project:  PROJ
 * Purpose:  C API wraper of C++ API
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

#include <cassert>
#include <cstdarg>
#include <cstring>
#include <map>
#include <utility>
#include <vector>

#include "proj/common.hpp"
#include "proj/coordinateoperation.hpp"
#include "proj/coordinatesystem.hpp"
#include "proj/crs.hpp"
#include "proj/datum.hpp"
#include "proj/io.hpp"
#include "proj/metadata.hpp"
#include "proj/util.hpp"

#include "proj/internal/internal.hpp"

// PROJ include order is sensitive
// clang-format off
#include "proj_internal.h"
#include "proj.h"
#include "projects.h"
// clang-format on

using namespace NS_PROJ::common;
using namespace NS_PROJ::crs;
using namespace NS_PROJ::cs;
using namespace NS_PROJ::datum;
using namespace NS_PROJ::io;
using namespace NS_PROJ::internal;
using namespace NS_PROJ::metadata;
using namespace NS_PROJ::operation;
using namespace NS_PROJ::util;
using namespace NS_PROJ;

// ---------------------------------------------------------------------------

static void PROJ_NO_INLINE proj_log_error(PJ_CONTEXT *ctx, const char *function,
                                          const char *text) {
    std::string msg(function);
    msg += ": ";
    msg += text;
    ctx->logger(ctx->app_data, PJ_LOG_ERROR, msg.c_str());
}

// ---------------------------------------------------------------------------

static void PROJ_NO_INLINE proj_log_debug(PJ_CONTEXT *ctx, const char *function,
                                          const char *text) {
    std::string msg(function);
    msg += ": ";
    msg += text;
    ctx->logger(ctx->app_data, PJ_LOG_DEBUG, msg.c_str());
}

// ---------------------------------------------------------------------------

/** \brief Opaque object representing a Ellipsoid, Datum, CRS or Coordinate
 * Operation. Should be used by at most one thread at a time. */
struct PJ_OBJ {
    //! @cond Doxygen_Suppress
    PJ_CONTEXT *ctx;
    IdentifiedObjectNNPtr obj;

    // cached results
    mutable std::string lastWKT{};
    mutable std::string lastPROJString{};
    mutable bool gridsNeededAsked = false;
    mutable std::vector<GridDescription> gridsNeeded{};

    explicit PJ_OBJ(PJ_CONTEXT *ctxIn, const IdentifiedObjectNNPtr &objIn)
        : ctx(ctxIn), obj(objIn) {}
    static PJ_OBJ *create(PJ_CONTEXT *ctxIn,
                          const IdentifiedObjectNNPtr &objIn);

    PJ_OBJ(const PJ_OBJ &) = delete;
    PJ_OBJ &operator=(const PJ_OBJ &) = delete;
    //! @endcond
};

//! @cond Doxygen_Suppress
PJ_OBJ *PJ_OBJ::create(PJ_CONTEXT *ctxIn, const IdentifiedObjectNNPtr &objIn) {
    return new PJ_OBJ(ctxIn, objIn);
}
//! @endcond

// ---------------------------------------------------------------------------

/** \brief Opaque object representing a set of operation results. */
struct PJ_OBJ_LIST {
    //! @cond Doxygen_Suppress
    PJ_CONTEXT *ctx;
    std::vector<IdentifiedObjectNNPtr> objects;

    explicit PJ_OBJ_LIST(PJ_CONTEXT *ctxIn,
                         std::vector<IdentifiedObjectNNPtr> &&objectsIn)
        : ctx(ctxIn), objects(std::move(objectsIn)) {}

    PJ_OBJ_LIST(const PJ_OBJ_LIST &) = delete;
    PJ_OBJ_LIST &operator=(const PJ_OBJ_LIST &) = delete;
    //! @endcond
};

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress

/** Auxiliary structure to PJ_CONTEXT storing C++ context stuff. */
struct projCppContext {
    DatabaseContextNNPtr databaseContext;

    explicit projCppContext(PJ_CONTEXT *ctxt, const char *dbPath = nullptr,
                            const char *const *auxDbPaths = nullptr)
        : databaseContext(DatabaseContext::create(
              dbPath ? dbPath : std::string(), toVector(auxDbPaths))) {
        databaseContext->attachPJContext(ctxt);
    }

    static std::vector<std::string> toVector(const char *const *auxDbPaths) {
        std::vector<std::string> res;
        for (auto iter = auxDbPaths; iter && *iter; ++iter) {
            res.emplace_back(std::string(*iter));
        }
        return res;
    }
};

// ---------------------------------------------------------------------------

void proj_context_delete_cpp_context(struct projCppContext *cppContext) {
    delete cppContext;
}

//! @endcond

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress

#define SANITIZE_CTX(ctx)                                                      \
    do {                                                                       \
        if (ctx == nullptr) {                                                  \
            ctx = pj_get_default_ctx();                                        \
        }                                                                      \
    } while (0)

// ---------------------------------------------------------------------------

static PROJ_NO_INLINE const DatabaseContextNNPtr &
getDBcontext(PJ_CONTEXT *ctx) {
    if (ctx->cpp_context == nullptr) {
        ctx->cpp_context = new projCppContext(ctx);
    }
    return ctx->cpp_context->databaseContext;
}

// ---------------------------------------------------------------------------

static PROJ_NO_INLINE DatabaseContextPtr
getDBcontextNoException(PJ_CONTEXT *ctx, const char *function) {
    try {
        return getDBcontext(ctx).as_nullable();
    } catch (const std::exception &e) {
        proj_log_debug(ctx, function, e.what());
        return nullptr;
    }
}

//! @endcond

// ---------------------------------------------------------------------------

/** \brief Explicitly point to the main PROJ CRS and coordinate operation
 * definition database ("proj.db"), and potentially auxiliary databases with
 * same structure.
 *
 * @param ctx PROJ context, or NULL for default context
 * @param dbPath Path to main database, or NULL for default.
 * @param auxDbPaths NULL-terminated list of auxiliary database filenames, or
 * NULL.
 * @param options should be set to NULL for now
 * @return TRUE in case of success
 */
int proj_context_set_database_path(PJ_CONTEXT *ctx, const char *dbPath,
                                   const char *const *auxDbPaths,
                                   const char *const *options) {
    SANITIZE_CTX(ctx);
    (void)options;
    delete ctx->cpp_context;
    ctx->cpp_context = nullptr;
    try {
        ctx->cpp_context = new projCppContext(ctx, dbPath, auxDbPaths);
        return true;
    } catch (const std::exception &e) {
        proj_log_error(ctx, __FUNCTION__, e.what());
        return false;
    }
}

// ---------------------------------------------------------------------------

/** \brief Returns the path to the database.
 *
 * The returned pointer remains valid while ctx is valid, and until
 * proj_context_set_database_path() is called.
 *
 * @param ctx PROJ context, or NULL for default context
 * @return path, or nullptr
 */
const char *proj_context_get_database_path(PJ_CONTEXT *ctx) {
    SANITIZE_CTX(ctx);
    try {
        return getDBcontext(ctx)->getPath().c_str();
    } catch (const std::exception &e) {
        proj_log_error(ctx, __FUNCTION__, e.what());
        return nullptr;
    }
}

// ---------------------------------------------------------------------------

/** \brief Return a metadata from the database.
 *
 * The returned pointer remains valid while ctx is valid, and until
 * proj_context_get_database_metadata() is called.
 *
 * @param ctx PROJ context, or NULL for default context
 * @param key Metadata key. Must not be NULL
 * @return value, or nullptr
 */
const char *proj_context_get_database_metadata(PJ_CONTEXT *ctx,
                                               const char *key) {
    SANITIZE_CTX(ctx);
    try {
        return getDBcontext(ctx)->getMetadata(key);
    } catch (const std::exception &e) {
        proj_log_error(ctx, __FUNCTION__, e.what());
        return nullptr;
    }
}

// ---------------------------------------------------------------------------

/** \brief Guess the "dialect" of the WKT string.
 *
 * @param ctx PROJ context, or NULL for default context
 * @param wkt String (must not be NULL)
 */
PJ_GUESSED_WKT_DIALECT proj_context_guess_wkt_dialect(PJ_CONTEXT *ctx,
                                                      const char *wkt) {
    (void)ctx;
    assert(wkt);
    switch (WKTParser().guessDialect(wkt)) {
    case WKTParser::WKTGuessedDialect::WKT2_2018:
        return PJ_GUESSED_WKT2_2018;
    case WKTParser::WKTGuessedDialect::WKT2_2015:
        return PJ_GUESSED_WKT2_2015;
    case WKTParser::WKTGuessedDialect::WKT1_GDAL:
        return PJ_GUESSED_WKT1_GDAL;
    case WKTParser::WKTGuessedDialect::WKT1_ESRI:
        return PJ_GUESSED_WKT1_ESRI;
    case WKTParser::WKTGuessedDialect::NOT_WKT:
        break;
    }
    return PJ_GUESSED_NOT_WKT;
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
static const char *getOptionValue(const char *option,
                                  const char *keyWithEqual) noexcept {
    if (ci_starts_with(option, keyWithEqual)) {
        return option + strlen(keyWithEqual);
    }
    return nullptr;
}
//! @endcond

// ---------------------------------------------------------------------------

/** \brief "Clone" an object.
 *
 * Technically this just increases the reference counter on the object, since
 * PJ_OBJ objects are immutable.
 *
 * The returned object must be unreferenced with proj_obj_unref() after use.
 * It should be used by at most one thread at a time.
 *
 * @param obj Object to clone. Must not be NULL.
 * @return Object that must be unreferenced with proj_obj_unref(), or NULL in
 * case of error.
 */
PJ_OBJ *proj_obj_clone(const PJ_OBJ *obj) {
    try {
        return PJ_OBJ::create(obj->ctx, obj->obj);
    } catch (const std::exception &e) {
        proj_log_error(obj->ctx, __FUNCTION__, e.what());
    }
    return nullptr;
}

// ---------------------------------------------------------------------------

/** \brief Instanciate an object from a WKT string, PROJ string or object code
 * (like "EPSG:4326", "urn:ogc:def:crs:EPSG::4326",
 * "urn:ogc:def:coordinateOperation:EPSG::1671").
 *
 * This function calls osgeo::proj::io::createFromUserInput()
 *
 * The returned object must be unreferenced with proj_obj_unref() after use.
 * It should be used by at most one thread at a time.
 *
 * @param ctx PROJ context, or NULL for default context
 * @param text String (must not be NULL)
 * @param options null-terminated list of options, or NULL. Currently
 * supported options are:
 * <ul>
 * <li>USE_PROJ4_INIT_RULES=YES/NO. Defaults to NO. When set to YES,
 * init=epsg:XXXX syntax will be allowed and will be interpreted according to
 * PROJ.4 and PROJ.5 rules, that is geodeticCRS will have longitude, latitude
 * order and will expect/output coordinates in radians. ProjectedCRS will have
 * easting, northing axis order (except the ones with Transverse Mercator South
 * Orientated projection). In that mode, the epsg:XXXX syntax will be also
 * interprated the same way.</li>
 * </ul>
 * @return Object that must be unreferenced with proj_obj_unref(), or NULL in
 * case of error.
 */
PJ_OBJ *proj_obj_create_from_user_input(PJ_CONTEXT *ctx, const char *text,
                                        const char *const *options) {
    SANITIZE_CTX(ctx);
    assert(text);
    (void)options;
    auto dbContext = getDBcontextNoException(ctx, __FUNCTION__);
    try {
        bool usePROJ4InitRules = false;
        for (auto iter = options; iter && iter[0]; ++iter) {
            const char *value;
            if ((value = getOptionValue(*iter, "USE_PROJ4_INIT_RULES="))) {
                usePROJ4InitRules = ci_equal(value, "YES");
            } else {
                std::string msg("Unknown option :");
                msg += *iter;
                proj_log_error(ctx, __FUNCTION__, msg.c_str());
                return nullptr;
            }
        }
        auto identifiedObject = nn_dynamic_pointer_cast<IdentifiedObject>(
            createFromUserInput(text, dbContext, usePROJ4InitRules));
        if (identifiedObject) {
            return PJ_OBJ::create(ctx, NN_NO_CHECK(identifiedObject));
        }
    } catch (const std::exception &e) {
        proj_log_error(ctx, __FUNCTION__, e.what());
    }
    return nullptr;
}

// ---------------------------------------------------------------------------

/** \brief Instanciate an object from a WKT string.
 *
 * This function calls osgeo::proj::io::WKTParser::createFromWKT()
 *
 * The returned object must be unreferenced with proj_obj_unref() after use.
 * It should be used by at most one thread at a time.
 *
 * @param ctx PROJ context, or NULL for default context
 * @param wkt WKT string (must not be NULL)
 * @param options should be set to NULL for now
 * @return Object that must be unreferenced with proj_obj_unref(), or NULL in
 * case of error.
 */
PJ_OBJ *proj_obj_create_from_wkt(PJ_CONTEXT *ctx, const char *wkt,
                                 const char *const *options) {
    SANITIZE_CTX(ctx);
    assert(wkt);
    (void)options;
    try {
        auto identifiedObject = nn_dynamic_pointer_cast<IdentifiedObject>(
            WKTParser().createFromWKT(wkt));
        if (identifiedObject) {
            return PJ_OBJ::create(ctx, NN_NO_CHECK(identifiedObject));
        }
    } catch (const std::exception &e) {
        proj_log_error(ctx, __FUNCTION__, e.what());
    }
    return nullptr;
}

// ---------------------------------------------------------------------------

/** \brief Instanciate an object from a PROJ string.
 *
 * This function calls osgeo::proj::io::PROJStringParser::createFromPROJString()
 *
 * The returned object must be unreferenced with proj_obj_unref() after use.
 * It should be used by at most one thread at a time.
 *
 * @param ctx PROJ context, or NULL for default context
 * @param proj_string PROJ string (must not be NULL)
 * @param options should be set to NULL for now
 * @return Object that must be unreferenced with proj_obj_unref(), or NULL in
 * case of error.
 */
PJ_OBJ *proj_obj_create_from_proj_string(PJ_CONTEXT *ctx,
                                         const char *proj_string,
                                         const char *const *options) {
    SANITIZE_CTX(ctx);
    (void)options;
    assert(proj_string);
    try {
        auto identifiedObject = nn_dynamic_pointer_cast<IdentifiedObject>(
            PROJStringParser().createFromPROJString(proj_string));
        if (identifiedObject) {
            return PJ_OBJ::create(ctx, NN_NO_CHECK(identifiedObject));
        }
    } catch (const std::exception &e) {
        proj_log_error(ctx, __FUNCTION__, e.what());
    }
    return nullptr;
}

// ---------------------------------------------------------------------------

/** \brief Instanciate an object from a database lookup.
 *
 * The returned object must be unreferenced with proj_obj_unref() after use.
 * It should be used by at most one thread at a time.
 *
 * @param ctx Context, or NULL for default context.
 * @param auth_name Authority name (must not be NULL)
 * @param code Object code (must not be NULL)
 * @param category Object category
 * @param usePROJAlternativeGridNames Whether PROJ alternative grid names
 * should be substituted to the official grid names. Only used on
 * transformations
 * @param options should be set to NULL for now
 * @return Object that must be unreferenced with proj_obj_unref(), or NULL in
 * case of error.
 */
PJ_OBJ *proj_obj_create_from_database(PJ_CONTEXT *ctx, const char *auth_name,
                                      const char *code,
                                      PJ_OBJ_CATEGORY category,
                                      int usePROJAlternativeGridNames,
                                      const char *const *options) {
    assert(auth_name);
    assert(code);
    (void)options;
    SANITIZE_CTX(ctx);
    const std::string codeStr(code);
    try {
        auto factory = AuthorityFactory::create(getDBcontext(ctx), auth_name);
        IdentifiedObjectPtr obj;
        switch (category) {
        case PJ_OBJ_CATEGORY_ELLIPSOID:
            obj = factory->createEllipsoid(codeStr).as_nullable();
            break;
        case PJ_OBJ_CATEGORY_DATUM:
            obj = factory->createDatum(codeStr).as_nullable();
            break;
        case PJ_OBJ_CATEGORY_CRS:
            obj =
                factory->createCoordinateReferenceSystem(codeStr).as_nullable();
            break;
        case PJ_OBJ_CATEGORY_COORDINATE_OPERATION:
            obj = factory
                      ->createCoordinateOperation(
                          codeStr, usePROJAlternativeGridNames != 0)
                      .as_nullable();
            break;
        }
        return PJ_OBJ::create(ctx, NN_NO_CHECK(obj));
    } catch (const std::exception &e) {
        proj_log_error(ctx, __FUNCTION__, e.what());
    }
    return nullptr;
}

// ---------------------------------------------------------------------------

/** \brief Drops a reference on an object.
 *
 * This method should be called one and exactly one for each function
 * returning a PJ_OBJ*
 *
 * @param obj Object, or NULL.
 */
void proj_obj_unref(PJ_OBJ *obj) { delete obj; }

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
static AuthorityFactory::ObjectType
convertPJObjectTypeToObjectType(PJ_OBJ_TYPE type, bool &valid) {
    valid = true;
    AuthorityFactory::ObjectType cppType = AuthorityFactory::ObjectType::CRS;
    switch (type) {
    case PJ_OBJ_TYPE_ELLIPSOID:
        cppType = AuthorityFactory::ObjectType::ELLIPSOID;
        break;

    case PJ_OBJ_TYPE_GEODETIC_REFERENCE_FRAME:
    case PJ_OBJ_TYPE_DYNAMIC_GEODETIC_REFERENCE_FRAME:
        cppType = AuthorityFactory::ObjectType::GEODETIC_REFERENCE_FRAME;
        break;

    case PJ_OBJ_TYPE_VERTICAL_REFERENCE_FRAME:
    case PJ_OBJ_TYPE_DYNAMIC_VERTICAL_REFERENCE_FRAME:
        cppType = AuthorityFactory::ObjectType::VERTICAL_REFERENCE_FRAME;
        break;

    case PJ_OBJ_TYPE_DATUM_ENSEMBLE:
        cppType = AuthorityFactory::ObjectType::DATUM;
        break;

    case PJ_OBJ_TYPE_CRS:
        cppType = AuthorityFactory::ObjectType::CRS;
        break;

    case PJ_OBJ_TYPE_GEODETIC_CRS:
        cppType = AuthorityFactory::ObjectType::GEODETIC_CRS;
        break;

    case PJ_OBJ_TYPE_GEOCENTRIC_CRS:
        cppType = AuthorityFactory::ObjectType::GEOCENTRIC_CRS;
        break;

    case PJ_OBJ_TYPE_GEOGRAPHIC_CRS:
        cppType = AuthorityFactory::ObjectType::GEOGRAPHIC_CRS;
        break;

    case PJ_OBJ_TYPE_GEOGRAPHIC_2D_CRS:
        cppType = AuthorityFactory::ObjectType::GEOGRAPHIC_2D_CRS;
        break;

    case PJ_OBJ_TYPE_GEOGRAPHIC_3D_CRS:
        cppType = AuthorityFactory::ObjectType::GEOGRAPHIC_3D_CRS;
        break;

    case PJ_OBJ_TYPE_VERTICAL_CRS:
        cppType = AuthorityFactory::ObjectType::VERTICAL_CRS;
        break;

    case PJ_OBJ_TYPE_PROJECTED_CRS:
        cppType = AuthorityFactory::ObjectType::PROJECTED_CRS;
        break;

    case PJ_OBJ_TYPE_COMPOUND_CRS:
        cppType = AuthorityFactory::ObjectType::COMPOUND_CRS;
        break;

    case PJ_OBJ_TYPE_ENGINEERING_CRS:
        valid = false;
        break;

    case PJ_OBJ_TYPE_TEMPORAL_CRS:
        valid = false;
        break;

    case PJ_OBJ_TYPE_BOUND_CRS:
        valid = false;
        break;

    case PJ_OBJ_TYPE_OTHER_CRS:
        cppType = AuthorityFactory::ObjectType::CRS;
        break;

    case PJ_OBJ_TYPE_CONVERSION:
        cppType = AuthorityFactory::ObjectType::CONVERSION;
        break;

    case PJ_OBJ_TYPE_TRANSFORMATION:
        cppType = AuthorityFactory::ObjectType::TRANSFORMATION;
        break;

    case PJ_OBJ_TYPE_CONCATENATED_OPERATION:
        cppType = AuthorityFactory::ObjectType::CONCATENATED_OPERATION;
        break;

    case PJ_OBJ_TYPE_OTHER_COORDINATE_OPERATION:
        cppType = AuthorityFactory::ObjectType::COORDINATE_OPERATION;
        break;

    case PJ_OBJ_TYPE_UNKNOWN:
        valid = false;
        break;
    }
    return cppType;
}
//! @endcond

// ---------------------------------------------------------------------------

/** \brief Return a list of objects by their name.
 *
 * @param ctx Context, or NULL for default context.
 * @param auth_name Authority name, used to restrict the search.
 * Or NULL for all authorities.
 * @param searchedName Searched name. Must be at least 2 character long.
 * @param types List of object types into which to search. If
 * NULL, all object types will be searched.
 * @param typesCount Number of elements in types, or 0 if types is NULL
 * @param approximateMatch Whether approximate name identification is allowed.
 * @param limitResultCount Maximum number of results to return.
 * Or 0 for unlimited.
 * @param options should be set to NULL for now
 * @return a result set that must be unreferenced with
 * proj_obj_list_unref(), or NULL in case of error.
 */
PJ_OBJ_LIST *proj_obj_create_from_name(PJ_CONTEXT *ctx, const char *auth_name,
                                       const char *searchedName,
                                       const PJ_OBJ_TYPE *types,
                                       size_t typesCount, int approximateMatch,
                                       size_t limitResultCount,
                                       const char *const *options) {
    assert(searchedName);
    assert((types != nullptr && typesCount > 0) ||
           (types == nullptr && typesCount == 0));
    (void)options;
    SANITIZE_CTX(ctx);
    try {
        auto factory = AuthorityFactory::create(getDBcontext(ctx),
                                                auth_name ? auth_name : "");
        std::vector<AuthorityFactory::ObjectType> allowedTypes;
        for (size_t i = 0; i < typesCount; ++i) {
            bool valid = false;
            auto type = convertPJObjectTypeToObjectType(types[i], valid);
            if (valid) {
                allowedTypes.push_back(type);
            }
        }
        auto res = factory->createObjectsFromName(searchedName, allowedTypes,
                                                  approximateMatch != 0,
                                                  limitResultCount);
        std::vector<IdentifiedObjectNNPtr> objects;
        for (const auto &obj : res) {
            objects.push_back(obj);
        }
        return new PJ_OBJ_LIST(ctx, std::move(objects));
    } catch (const std::exception &e) {
        proj_log_error(ctx, __FUNCTION__, e.what());
    }
    return nullptr;
}

// ---------------------------------------------------------------------------

/** \brief Return the type of an object.
 *
 * @param obj Object (must not be NULL)
 * @return its type.
 */
PJ_OBJ_TYPE proj_obj_get_type(const PJ_OBJ *obj) {
    assert(obj);
    auto ptr = obj->obj.get();
    if (dynamic_cast<Ellipsoid *>(ptr)) {
        return PJ_OBJ_TYPE_ELLIPSOID;
    }

    if (dynamic_cast<DynamicGeodeticReferenceFrame *>(ptr)) {
        return PJ_OBJ_TYPE_DYNAMIC_GEODETIC_REFERENCE_FRAME;
    }
    if (dynamic_cast<GeodeticReferenceFrame *>(ptr)) {
        return PJ_OBJ_TYPE_GEODETIC_REFERENCE_FRAME;
    }
    if (dynamic_cast<DynamicVerticalReferenceFrame *>(ptr)) {
        return PJ_OBJ_TYPE_DYNAMIC_VERTICAL_REFERENCE_FRAME;
    }
    if (dynamic_cast<VerticalReferenceFrame *>(ptr)) {
        return PJ_OBJ_TYPE_VERTICAL_REFERENCE_FRAME;
    }
    if (dynamic_cast<DatumEnsemble *>(ptr)) {
        return PJ_OBJ_TYPE_DATUM_ENSEMBLE;
    }

    {
        auto crs = dynamic_cast<GeographicCRS *>(ptr);
        if (crs) {
            if (crs->coordinateSystem()->axisList().size() == 2) {
                return PJ_OBJ_TYPE_GEOGRAPHIC_2D_CRS;
            } else {
                return PJ_OBJ_TYPE_GEOGRAPHIC_3D_CRS;
            }
        }
    }

    {
        auto crs = dynamic_cast<GeodeticCRS *>(ptr);
        if (crs) {
            if (crs->isGeocentric()) {
                return PJ_OBJ_TYPE_GEOCENTRIC_CRS;
            } else {
                return PJ_OBJ_TYPE_GEODETIC_CRS;
            }
        }
    }

    if (dynamic_cast<VerticalCRS *>(ptr)) {
        return PJ_OBJ_TYPE_VERTICAL_CRS;
    }
    if (dynamic_cast<ProjectedCRS *>(ptr)) {
        return PJ_OBJ_TYPE_PROJECTED_CRS;
    }
    if (dynamic_cast<CompoundCRS *>(ptr)) {
        return PJ_OBJ_TYPE_COMPOUND_CRS;
    }
    if (dynamic_cast<TemporalCRS *>(ptr)) {
        return PJ_OBJ_TYPE_TEMPORAL_CRS;
    }
    if (dynamic_cast<EngineeringCRS *>(ptr)) {
        return PJ_OBJ_TYPE_ENGINEERING_CRS;
    }
    if (dynamic_cast<BoundCRS *>(ptr)) {
        return PJ_OBJ_TYPE_BOUND_CRS;
    }
    if (dynamic_cast<CRS *>(ptr)) {
        return PJ_OBJ_TYPE_OTHER_CRS;
    }

    if (dynamic_cast<Conversion *>(ptr)) {
        return PJ_OBJ_TYPE_CONVERSION;
    }
    if (dynamic_cast<Transformation *>(ptr)) {
        return PJ_OBJ_TYPE_TRANSFORMATION;
    }
    if (dynamic_cast<ConcatenatedOperation *>(ptr)) {
        return PJ_OBJ_TYPE_CONCATENATED_OPERATION;
    }
    if (dynamic_cast<CoordinateOperation *>(ptr)) {
        return PJ_OBJ_TYPE_OTHER_COORDINATE_OPERATION;
    }

    return PJ_OBJ_TYPE_UNKNOWN;
}

// ---------------------------------------------------------------------------

/** \brief Return whether an object is deprecated.
 *
 * @param obj Object (must not be NULL)
 * @return TRUE if it is deprecated, FALSE otherwise
 */
int proj_obj_is_deprecated(const PJ_OBJ *obj) {
    assert(obj);
    return obj->obj->isDeprecated();
}

// ---------------------------------------------------------------------------

/** \brief Return whether two objects are equivalent.
 *
 * @param obj Object (must not be NULL)
 * @param other Other object (must not be NULL)
 * @param criterion Comparison criterion
 * @return TRUE if they are equivalent
 */
int proj_obj_is_equivalent_to(const PJ_OBJ *obj, const PJ_OBJ *other,
                              PJ_COMPARISON_CRITERION criterion) {
    assert(obj);
    assert(other);

    // Make sure that the C and C++ enumerations match
    static_assert(static_cast<int>(PJ_COMP_STRICT) ==
                      static_cast<int>(IComparable::Criterion::STRICT),
                  "");
    static_assert(static_cast<int>(PJ_COMP_EQUIVALENT) ==
                      static_cast<int>(IComparable::Criterion::EQUIVALENT),
                  "");
    static_assert(
        static_cast<int>(PJ_COMP_EQUIVALENT_EXCEPT_AXIS_ORDER_GEOGCRS) ==
            static_cast<int>(
                IComparable::Criterion::EQUIVALENT_EXCEPT_AXIS_ORDER_GEOGCRS),
        "");

    // Make sure we enumerate all values. If adding a new value, as we
    // don't have a default clause, the compiler will warn.
    switch (criterion) {
    case PJ_COMP_STRICT:
    case PJ_COMP_EQUIVALENT:
    case PJ_COMP_EQUIVALENT_EXCEPT_AXIS_ORDER_GEOGCRS:
        break;
    }
    const IComparable::Criterion cppCriterion =
        static_cast<IComparable::Criterion>(criterion);
    return obj->obj->isEquivalentTo(other->obj.get(), cppCriterion);
}

// ---------------------------------------------------------------------------

/** \brief Return whether an object is a CRS
 *
 * @param obj Object (must not be NULL)
 */
int proj_obj_is_crs(const PJ_OBJ *obj) {
    assert(obj);
    return dynamic_cast<CRS *>(obj->obj.get()) != nullptr;
}

// ---------------------------------------------------------------------------

/** \brief Get the name of an object.
 *
 * The lifetime of the returned string is the same as the input obj parameter.
 *
 * @param obj Object (must not be NULL)
 * @return a string, or NULL in case of error or missing name.
 */
const char *proj_obj_get_name(const PJ_OBJ *obj) {
    assert(obj);
    const auto &desc = obj->obj->name()->description();
    if (!desc.has_value()) {
        return nullptr;
    }
    // The object will still be alived after the function call.
    // cppcheck-suppress stlcstr
    return desc->c_str();
}

// ---------------------------------------------------------------------------

/** \brief Get the authority name / codespace of an identifier of an object.
 *
 * The lifetime of the returned string is the same as the input obj parameter.
 *
 * @param obj Object (must not be NULL)
 * @param index Index of the identifier. 0 = first identifier
 * @return a string, or NULL in case of error or missing name.
 */
const char *proj_obj_get_id_auth_name(const PJ_OBJ *obj, int index) {
    assert(obj);
    const auto &ids = obj->obj->identifiers();
    if (static_cast<size_t>(index) >= ids.size()) {
        return nullptr;
    }
    const auto &codeSpace = ids[index]->codeSpace();
    if (!codeSpace.has_value()) {
        return nullptr;
    }
    // The object will still be alived after the function call.
    // cppcheck-suppress stlcstr
    return codeSpace->c_str();
}

// ---------------------------------------------------------------------------

/** \brief Get the code of an identifier of an object.
 *
 * The lifetime of the returned string is the same as the input obj parameter.
 *
 * @param obj Object (must not be NULL)
 * @param index Index of the identifier. 0 = first identifier
 * @return a string, or NULL in case of error or missing name.
 */
const char *proj_obj_get_id_code(const PJ_OBJ *obj, int index) {
    assert(obj);
    const auto &ids = obj->obj->identifiers();
    if (static_cast<size_t>(index) >= ids.size()) {
        return nullptr;
    }
    return ids[index]->code().c_str();
}

// ---------------------------------------------------------------------------

/** \brief Get a WKT representation of an object.
 *
 * The returned string is valid while the input obj parameter is valid,
 * and until a next call to proj_obj_as_wkt() with the same input object.
 *
 * This function calls osgeo::proj::io::IWKTExportable::exportToWKT().
 *
 * This function may return NULL if the object is not compatible with an
 * export to the requested type.
 *
 * @param obj Object (must not be NULL)
 * @param type WKT version.
 * @param options null-terminated list of options, or NULL. Currently
 * supported options are:
 * <ul>
 * <li>MULTILINE=YES/NO. Defaults to YES, except for WKT1_ESRI</li>
 * <li>INDENTATION_WIDTH=number. Defauls to 4 (when multiline output is
 * on).</li>
 * <li>OUTPUT_AXIS=AUTO/YES/NO. In AUTO mode, axis will be output for WKT2
 * variants, for WKT1_GDAL for ProjectedCRS with easting/northing ordering
 * (otherwise stripped), but not for WKT1_ESRI. Setting to YES will output
 * them unconditionaly, and to NO will omit them unconditionaly.</li>
 * </ul>
 * @return a string, or NULL in case of error.
 */
const char *proj_obj_as_wkt(const PJ_OBJ *obj, PJ_WKT_TYPE type,
                            const char *const *options) {
    assert(obj);

    // Make sure that the C and C++ enumerations match
    static_assert(static_cast<int>(PJ_WKT2_2015) ==
                      static_cast<int>(WKTFormatter::Convention::WKT2_2015),
                  "");
    static_assert(
        static_cast<int>(PJ_WKT2_2015_SIMPLIFIED) ==
            static_cast<int>(WKTFormatter::Convention::WKT2_2015_SIMPLIFIED),
        "");
    static_assert(static_cast<int>(PJ_WKT2_2018) ==
                      static_cast<int>(WKTFormatter::Convention::WKT2_2018),
                  "");
    static_assert(
        static_cast<int>(PJ_WKT2_2018_SIMPLIFIED) ==
            static_cast<int>(WKTFormatter::Convention::WKT2_2018_SIMPLIFIED),
        "");
    static_assert(static_cast<int>(PJ_WKT1_GDAL) ==
                      static_cast<int>(WKTFormatter::Convention::WKT1_GDAL),
                  "");
    static_assert(static_cast<int>(PJ_WKT1_ESRI) ==
                      static_cast<int>(WKTFormatter::Convention::WKT1_ESRI),
                  "");
    // Make sure we enumerate all values. If adding a new value, as we
    // don't have a default clause, the compiler will warn.
    switch (type) {
    case PJ_WKT2_2015:
    case PJ_WKT2_2015_SIMPLIFIED:
    case PJ_WKT2_2018:
    case PJ_WKT2_2018_SIMPLIFIED:
    case PJ_WKT1_GDAL:
    case PJ_WKT1_ESRI:
        break;
    }
    const WKTFormatter::Convention convention =
        static_cast<WKTFormatter::Convention>(type);
    try {
        auto formatter = WKTFormatter::create(convention);
        for (auto iter = options; iter && iter[0]; ++iter) {
            const char *value;
            if ((value = getOptionValue(*iter, "MULTILINE="))) {
                formatter->setMultiLine(ci_equal(value, "YES"));
            } else if ((value = getOptionValue(*iter, "INDENTATION_WIDTH="))) {
                formatter->setIndentationWidth(std::atoi(value));
            } else if ((value = getOptionValue(*iter, "OUTPUT_AXIS="))) {
                if (!ci_equal(value, "AUTO")) {
                    formatter->setOutputAxis(
                        ci_equal(value, "YES")
                            ? WKTFormatter::OutputAxisRule::YES
                            : WKTFormatter::OutputAxisRule::NO);
                }
            } else {
                std::string msg("Unknown option :");
                msg += *iter;
                proj_log_error(obj->ctx, __FUNCTION__, msg.c_str());
                return nullptr;
            }
        }
        obj->lastWKT = obj->obj->exportToWKT(formatter.get());
        return obj->lastWKT.c_str();
    } catch (const std::exception &e) {
        proj_log_error(obj->ctx, __FUNCTION__, e.what());
        return nullptr;
    }
}

// ---------------------------------------------------------------------------

/** \brief Get a PROJ string representation of an object.
 *
 * The returned string is valid while the input obj parameter is valid,
 * and until a next call to proj_obj_as_proj_string() with the same input
 * object.
 *
 * This function calls
 * osgeo::proj::io::IPROJStringExportable::exportToPROJString().
 *
 * This function may return NULL if the object is not compatible with an
 * export to the requested type.
 *
 * @param obj Object (must not be NULL)
 * @param type PROJ String version.
 * @param options NULL-terminated list of strings with "KEY=VALUE" format. or
 * NULL.
 * The currently recognized option is USE_ETMERC=YES to use
 * +proj=etmerc instead of +proj=tmerc (or USE_ETMERC=NO to disable implicit
 * use of etmerc by utm conversions)
 * @return a string, or NULL in case of error.
 */
const char *proj_obj_as_proj_string(const PJ_OBJ *obj, PJ_PROJ_STRING_TYPE type,
                                    const char *const *options) {
    assert(obj);
    auto exportable =
        dynamic_cast<const IPROJStringExportable *>(obj->obj.get());
    if (!exportable) {
        proj_log_error(obj->ctx, __FUNCTION__,
                       "Object type not exportable to PROJ");
        return nullptr;
    }
    // Make sure that the C and C++ enumeration match
    static_assert(static_cast<int>(PJ_PROJ_5) ==
                      static_cast<int>(PROJStringFormatter::Convention::PROJ_5),
                  "");
    static_assert(static_cast<int>(PJ_PROJ_4) ==
                      static_cast<int>(PROJStringFormatter::Convention::PROJ_4),
                  "");
    // Make sure we enumerate all values. If adding a new value, as we
    // don't have a default clause, the compiler will warn.
    switch (type) {
    case PJ_PROJ_5:
    case PJ_PROJ_4:
        break;
    }
    const PROJStringFormatter::Convention convention =
        static_cast<PROJStringFormatter::Convention>(type);
    auto dbContext = getDBcontextNoException(obj->ctx, __FUNCTION__);
    try {
        auto formatter = PROJStringFormatter::create(convention, dbContext);
        if (options != nullptr && options[0] != nullptr) {
            if (ci_equal(options[0], "USE_ETMERC=YES")) {
                formatter->setUseETMercForTMerc(true);
            } else if (ci_equal(options[0], "USE_ETMERC=NO")) {
                formatter->setUseETMercForTMerc(false);
            }
        }
        obj->lastPROJString = exportable->exportToPROJString(formatter.get());
        return obj->lastPROJString.c_str();
    } catch (const std::exception &e) {
        proj_log_error(obj->ctx, __FUNCTION__, e.what());
        return nullptr;
    }
}

// ---------------------------------------------------------------------------

/** \brief Return the area of use of an object.
 *
 * @param obj Object (must not be NULL)
 * @param p_west_lon_degree Pointer to a double to receive the west longitude
 * (in degrees). Or NULL. If the returned value is -1000, the bounding box is
 * unknown.
 * @param p_south_lat_degree Pointer to a double to receive the south latitude
 * (in degrees). Or NULL. If the returned value is -1000, the bounding box is
 * unknown.
 * @param p_east_lon_degree Pointer to a double to receive the east longitude
 * (in degrees). Or NULL. If the returned value is -1000, the bounding box is
 * unknown.
 * @param p_north_lat_degree Pointer to a double to receive the north latitude
 * (in degrees). Or NULL. If the returned value is -1000, the bounding box is
 * unknown.
 * @param p_area_name Pointer to a string to receive the name of the area of
 * use. Or NULL. *p_area_name is valid while obj is valid itself.
 * @return TRUE in case of success, FALSE in case of error or if the area
 * of use is unknown.
 */
int proj_obj_get_area_of_use(const PJ_OBJ *obj, double *p_west_lon_degree,
                             double *p_south_lat_degree,
                             double *p_east_lon_degree,
                             double *p_north_lat_degree,
                             const char **p_area_name) {
    if (p_area_name) {
        *p_area_name = nullptr;
    }
    auto objectUsage = dynamic_cast<const ObjectUsage *>(obj->obj.get());
    if (!objectUsage) {
        return false;
    }
    const auto &domains = objectUsage->domains();
    if (domains.empty()) {
        return false;
    }
    const auto &extent = domains[0]->domainOfValidity();
    if (!extent) {
        return false;
    }
    const auto &desc = extent->description();
    if (desc.has_value() && p_area_name) {
        *p_area_name = desc->c_str();
    }

    const auto &geogElements = extent->geographicElements();
    if (!geogElements.empty()) {
        auto bbox =
            dynamic_cast<const GeographicBoundingBox *>(geogElements[0].get());
        if (bbox) {
            if (p_west_lon_degree) {
                *p_west_lon_degree = bbox->westBoundLongitude();
            }
            if (p_south_lat_degree) {
                *p_south_lat_degree = bbox->southBoundLatitude();
            }
            if (p_east_lon_degree) {
                *p_east_lon_degree = bbox->eastBoundLongitude();
            }
            if (p_north_lat_degree) {
                *p_north_lat_degree = bbox->northBoundLatitude();
            }
            return true;
        }
    }
    if (p_west_lon_degree) {
        *p_west_lon_degree = -1000;
    }
    if (p_south_lat_degree) {
        *p_south_lat_degree = -1000;
    }
    if (p_east_lon_degree) {
        *p_east_lon_degree = -1000;
    }
    if (p_north_lat_degree) {
        *p_north_lat_degree = -1000;
    }
    return true;
}

// ---------------------------------------------------------------------------

static const GeodeticCRS *extractGeodeticCRS(const PJ_OBJ *crs,
                                             const char *fname) {
    assert(crs);
    auto l_crs = dynamic_cast<const CRS *>(crs->obj.get());
    if (!l_crs) {
        proj_log_error(crs->ctx, fname, "Object is not a CRS");
        return nullptr;
    }
    auto geodCRS = l_crs->extractGeodeticCRSRaw();
    if (!geodCRS) {
        proj_log_error(crs->ctx, fname, "CRS has no geodetic CRS");
    }
    return geodCRS;
}

// ---------------------------------------------------------------------------

/** \brief Get the geodeticCRS / geographicCRS from a CRS
 *
 * The returned object must be unreferenced with proj_obj_unref() after
 * use.
 * It should be used by at most one thread at a time.
 *
 * @param crs Objet of type CRS (must not be NULL)
 * @return Object that must be unreferenced with proj_obj_unref(), or NULL
 * in case of error.
 */
PJ_OBJ *proj_obj_crs_get_geodetic_crs(const PJ_OBJ *crs) {
    auto geodCRS = extractGeodeticCRS(crs, __FUNCTION__);
    if (!geodCRS) {
        return nullptr;
    }
    return PJ_OBJ::create(crs->ctx,
                          NN_NO_CHECK(nn_dynamic_pointer_cast<IdentifiedObject>(
                              geodCRS->shared_from_this())));
}

// ---------------------------------------------------------------------------

/** \brief Get a CRS component from a CompoundCRS
 *
 * The returned object must be unreferenced with proj_obj_unref() after
 * use.
 * It should be used by at most one thread at a time.
 *
 * @param crs Objet of type CRS (must not be NULL)
 * @param index Index of the CRS component (typically 0 = horizontal, 1 =
 * vertical)
 * @return Object that must be unreferenced with proj_obj_unref(), or NULL
 * in case of error.
 */
PJ_OBJ *proj_obj_crs_get_sub_crs(const PJ_OBJ *crs, int index) {
    assert(crs);
    auto l_crs = dynamic_cast<CompoundCRS *>(crs->obj.get());
    if (!l_crs) {
        proj_log_error(crs->ctx, __FUNCTION__, "Object is not a CompoundCRS");
        return nullptr;
    }
    const auto &components = l_crs->componentReferenceSystems();
    if (static_cast<size_t>(index) >= components.size()) {
        return nullptr;
    }
    return PJ_OBJ::create(crs->ctx, components[index]);
}

// ---------------------------------------------------------------------------

/** \brief Returns a BoundCRS
 *
 * The returned object must be unreferenced with proj_obj_unref() after
 * use.
 * It should be used by at most one thread at a time.
 *
 * @param base_crs Base CRS (must not be NULL)
 * @param hub_crs Hub CRS (must not be NULL)
 * @param transformation Transformation (must not be NULL)
 * @return Object that must be unreferenced with proj_obj_unref(), or NULL
 * in case of error.
 */
PJ_OBJ *proj_obj_crs_create_bound_crs(const PJ_OBJ *base_crs,
                                      const PJ_OBJ *hub_crs,
                                      const PJ_OBJ *transformation) {
    assert(base_crs);
    assert(hub_crs);
    assert(transformation);
    auto l_base_crs = util::nn_dynamic_pointer_cast<CRS>(base_crs->obj);
    if (!l_base_crs) {
        proj_log_error(base_crs->ctx, __FUNCTION__, "base_crs is not a CRS");
        return nullptr;
    }
    auto l_hub_crs = util::nn_dynamic_pointer_cast<CRS>(hub_crs->obj);
    if (!l_hub_crs) {
        proj_log_error(base_crs->ctx, __FUNCTION__, "hub_crs is not a CRS");
        return nullptr;
    }
    auto l_transformation =
        util::nn_dynamic_pointer_cast<Transformation>(transformation->obj);
    if (!l_transformation) {
        proj_log_error(base_crs->ctx, __FUNCTION__,
                       "transformation is not a CRS");
        return nullptr;
    }
    try {
        return PJ_OBJ::create(base_crs->ctx,
                              BoundCRS::create(NN_NO_CHECK(l_base_crs),
                                               NN_NO_CHECK(l_hub_crs),
                                               NN_NO_CHECK(l_transformation)));
    } catch (const std::exception &e) {
        proj_log_error(base_crs->ctx, __FUNCTION__, e.what());
        return nullptr;
    }
}

// ---------------------------------------------------------------------------

/** \brief Returns potentially
 * a BoundCRS, with a transformation to EPSG:4326, wrapping this CRS
 *
 * The returned object must be unreferenced with proj_obj_unref() after
 * use.
 * It should be used by at most one thread at a time.
 *
 * This is the same as method
 * osgeo::proj::crs::CRS::createBoundCRSToWGS84IfPossible()
 *
 * @param crs Objet of type CRS (must not be NULL)
 * @return Object that must be unreferenced with proj_obj_unref(), or NULL
 * in case of error.
 */
PJ_OBJ *proj_obj_crs_create_bound_crs_to_WGS84(const PJ_OBJ *crs) {
    assert(crs);
    auto l_crs = dynamic_cast<const CRS *>(crs->obj.get());
    if (!l_crs) {
        proj_log_error(crs->ctx, __FUNCTION__, "Object is not a CRS");
        return nullptr;
    }
    auto dbContext = getDBcontextNoException(crs->ctx, __FUNCTION__);
    try {
        return PJ_OBJ::create(
            crs->ctx, l_crs->createBoundCRSToWGS84IfPossible(dbContext));
    } catch (const std::exception &e) {
        proj_log_error(crs->ctx, __FUNCTION__, e.what());
        return nullptr;
    }
}

// ---------------------------------------------------------------------------

/** \brief Get the ellipsoid from a CRS or a GeodeticReferenceFrame.
 *
 * The returned object must be unreferenced with proj_obj_unref() after
 * use.
 * It should be used by at most one thread at a time.
 *
 * @param obj Objet of type CRS or GeodeticReferenceFrame (must not be NULL)
 * @return Object that must be unreferenced with proj_obj_unref(), or NULL
 * in case of error.
 */
PJ_OBJ *proj_obj_get_ellipsoid(const PJ_OBJ *obj) {
    auto ptr = obj->obj.get();
    if (dynamic_cast<const CRS *>(ptr)) {
        auto geodCRS = extractGeodeticCRS(obj, __FUNCTION__);
        if (geodCRS) {
            return PJ_OBJ::create(obj->ctx, geodCRS->ellipsoid());
        }
    } else {
        auto datum = dynamic_cast<const GeodeticReferenceFrame *>(ptr);
        if (datum) {
            return PJ_OBJ::create(obj->ctx, datum->ellipsoid());
        }
    }
    proj_log_error(obj->ctx, __FUNCTION__,
                   "Object is not a CRS or GeodeticReferenceFrame");
    return nullptr;
}

// ---------------------------------------------------------------------------

/** \brief Get the horizontal datum from a CRS
 *
 * The returned object must be unreferenced with proj_obj_unref() after
 * use.
 * It should be used by at most one thread at a time.
 *
 * @param crs Objet of type CRS (must not be NULL)
 * @return Object that must be unreferenced with proj_obj_unref(), or NULL
 * in case of error.
 */
PJ_OBJ *proj_obj_crs_get_horizontal_datum(const PJ_OBJ *crs) {
    auto geodCRS = extractGeodeticCRS(crs, __FUNCTION__);
    if (!geodCRS) {
        return nullptr;
    }
    const auto &datum = geodCRS->datum();
    if (datum) {
        return PJ_OBJ::create(crs->ctx, NN_NO_CHECK(datum));
    }

    const auto &datumEnsemble = geodCRS->datumEnsemble();
    if (datumEnsemble) {
        return PJ_OBJ::create(crs->ctx, NN_NO_CHECK(datumEnsemble));
    }
    proj_log_error(crs->ctx, __FUNCTION__, "CRS has no datum");
    return nullptr;
}

// ---------------------------------------------------------------------------

/** \brief Return ellipsoid parameters.
 *
 * @param ellipsoid Object of type Ellipsoid (must not be NULL)
 * @param pSemiMajorMetre Pointer to a value to store the semi-major axis in
 * metre. or NULL
 * @param pSemiMinorMetre Pointer to a value to store the semi-minor axis in
 * metre. or NULL
 * @param pIsSemiMinorComputed Pointer to a boolean value to indicate if the
 * semi-minor value was computed. If FALSE, its value comes from the
 * definition. or NULL
 * @param pInverseFlattening Pointer to a value to store the inverse
 * flattening. or NULL
 * @return TRUE in case of success.
 */
int proj_obj_ellipsoid_get_parameters(const PJ_OBJ *ellipsoid,
                                      double *pSemiMajorMetre,
                                      double *pSemiMinorMetre,
                                      int *pIsSemiMinorComputed,
                                      double *pInverseFlattening) {
    assert(ellipsoid);
    auto l_ellipsoid = dynamic_cast<const Ellipsoid *>(ellipsoid->obj.get());
    if (!l_ellipsoid) {
        proj_log_error(ellipsoid->ctx, __FUNCTION__,
                       "Object is not a Ellipsoid");
        return FALSE;
    }

    if (pSemiMajorMetre) {
        *pSemiMajorMetre = l_ellipsoid->semiMajorAxis().getSIValue();
    }
    if (pSemiMinorMetre) {
        *pSemiMinorMetre = l_ellipsoid->computeSemiMinorAxis().getSIValue();
    }
    if (pIsSemiMinorComputed) {
        *pIsSemiMinorComputed = !(l_ellipsoid->semiMinorAxis().has_value());
    }
    if (pInverseFlattening) {
        *pInverseFlattening = l_ellipsoid->computedInverseFlattening();
    }
    return TRUE;
}

// ---------------------------------------------------------------------------

/** \brief Get the prime meridian of a CRS or a GeodeticReferenceFrame.
 *
 * The returned object must be unreferenced with proj_obj_unref() after
 * use.
 * It should be used by at most one thread at a time.
 *
 * @param obj Objet of type CRS or GeodeticReferenceFrame (must not be NULL)
 * @return Object that must be unreferenced with proj_obj_unref(), or NULL
 * in case of error.
 */

PJ_OBJ *proj_obj_get_prime_meridian(const PJ_OBJ *obj) {
    auto ptr = obj->obj.get();
    if (dynamic_cast<CRS *>(ptr)) {
        auto geodCRS = extractGeodeticCRS(obj, __FUNCTION__);
        if (geodCRS) {
            return PJ_OBJ::create(obj->ctx, geodCRS->primeMeridian());
        }
    } else {
        auto datum = dynamic_cast<const GeodeticReferenceFrame *>(ptr);
        if (datum) {
            return PJ_OBJ::create(obj->ctx, datum->primeMeridian());
        }
    }
    proj_log_error(obj->ctx, __FUNCTION__,
                   "Object is not a CRS or GeodeticReferenceFrame");
    return nullptr;
}

// ---------------------------------------------------------------------------

/** \brief Return prime meridian parameters.
 *
 * @param prime_meridian Object of type PrimeMeridian (must not be NULL)
 * @param pLongitude Pointer to a value to store the longitude of the prime
 * meridian, in its native unit. or NULL
 * @param pLongitudeUnitConvFactor Pointer to a value to store the conversion
 * factor of the prime meridian longitude unit to radian. or NULL
 * @param pLongitudeUnitName Pointer to a string value to store the unit name.
 * or NULL
 * @return TRUE in case of success.
 */
int proj_obj_prime_meridian_get_parameters(const PJ_OBJ *prime_meridian,
                                           double *pLongitude,
                                           double *pLongitudeUnitConvFactor,
                                           const char **pLongitudeUnitName) {
    assert(prime_meridian);
    auto l_pm = dynamic_cast<const PrimeMeridian *>(prime_meridian->obj.get());
    if (!l_pm) {
        proj_log_error(prime_meridian->ctx, __FUNCTION__,
                       "Object is not a PrimeMeridian");
        return false;
    }
    const auto &longitude = l_pm->longitude();
    if (pLongitude) {
        *pLongitude = longitude.value();
    }
    const auto &unit = longitude.unit();
    if (pLongitudeUnitConvFactor) {
        *pLongitudeUnitConvFactor = unit.conversionToSI();
    }
    if (pLongitudeUnitName) {
        *pLongitudeUnitName = unit.name().c_str();
    }
    return true;
}

// ---------------------------------------------------------------------------

/** \brief Return the base CRS of a BoundCRS or a DerivedCRS/ProjectedCRS, or
 * the source CRS of a CoordinateOperation.
 *
 * The returned object must be unreferenced with proj_obj_unref() after
 * use.
 * It should be used by at most one thread at a time.
 *
 * @param obj Objet of type BoundCRS or CoordinateOperation (must not be NULL)
 * @return Object that must be unreferenced with proj_obj_unref(), or NULL
 * in case of error, or missing source CRS.
 */
PJ_OBJ *proj_obj_get_source_crs(const PJ_OBJ *obj) {
    assert(obj);
    auto ptr = obj->obj.get();
    auto boundCRS = dynamic_cast<const BoundCRS *>(ptr);
    if (boundCRS) {
        return PJ_OBJ::create(obj->ctx, boundCRS->baseCRS());
    }
    auto derivedCRS = dynamic_cast<const DerivedCRS *>(ptr);
    if (derivedCRS) {
        return PJ_OBJ::create(obj->ctx, derivedCRS->baseCRS());
    }
    auto co = dynamic_cast<const CoordinateOperation *>(ptr);
    if (co) {
        auto sourceCRS = co->sourceCRS();
        if (sourceCRS) {
            return PJ_OBJ::create(obj->ctx, NN_NO_CHECK(sourceCRS));
        }
        return nullptr;
    }
    proj_log_error(obj->ctx, __FUNCTION__,
                   "Object is not a BoundCRS or a CoordinateOperation");
    return nullptr;
}

// ---------------------------------------------------------------------------

/** \brief Return the hub CRS of a BoundCRS or the target CRS of a
 * CoordinateOperation.
 *
 * The returned object must be unreferenced with proj_obj_unref() after
 * use.
 * It should be used by at most one thread at a time.
 *
 * @param obj Objet of type BoundCRS or CoordinateOperation (must not be NULL)
 * @return Object that must be unreferenced with proj_obj_unref(), or NULL
 * in case of error, or missing target CRS.
 */
PJ_OBJ *proj_obj_get_target_crs(const PJ_OBJ *obj) {
    assert(obj);
    auto ptr = obj->obj.get();
    auto boundCRS = dynamic_cast<const BoundCRS *>(ptr);
    if (boundCRS) {
        return PJ_OBJ::create(obj->ctx, boundCRS->hubCRS());
    }
    auto co = dynamic_cast<const CoordinateOperation *>(ptr);
    if (co) {
        auto targetCRS = co->targetCRS();
        if (targetCRS) {
            return PJ_OBJ::create(obj->ctx, NN_NO_CHECK(targetCRS));
        }
        return nullptr;
    }
    proj_log_error(obj->ctx, __FUNCTION__,
                   "Object is not a BoundCRS or a CoordinateOperation");
    return nullptr;
}

// ---------------------------------------------------------------------------

/** \brief Identify the CRS with reference CRSs.
 *
 * The candidate CRSs are either hard-coded, or looked in the database when
 * authorityFactory is not null.
 *
 * The method returns a list of matching reference CRS, and the percentage
 * (0-100) of confidence in the match.
 * 100% means that the name of the reference entry
 * perfectly matches the CRS name, and both are equivalent. In which case a
 * single result is returned.
 * 90% means that CRS are equivalent, but the names are not exactly the same.
 * 70% means that CRS are equivalent), but the names do not match at all.
 * 25% means that the CRS are not equivalent, but there is some similarity in
 * the names.
 * Other confidence values may be returned by some specialized implementations.
 *
 * This is implemented for GeodeticCRS, ProjectedCRS, VerticalCRS and
 * CompoundCRS.
 *
 * @param obj Object of type CRS. Must not be NULL
 * @param auth_name Authority name, or NULL for all authorities
 * @param options Placeholder for future options. Should be set to NULL.
 * @param confidence Output parameter. Pointer to an array of integers that will
 * be allocated by the function and filled with the confidence values
 * (0-100). There are as many elements in this array as
 * proj_obj_list_get_count()
 * returns on the return value of this function. *confidence should be
 * released with proj_free_int_list().
 * @return a list of matching reference CRS, or nullptr in case of error.
 */
PJ_OBJ_LIST *proj_obj_identify(const PJ_OBJ *obj, const char *auth_name,
                               const char *const *options, int **confidence) {
    assert(obj);
    (void)options;
    if (confidence) {
        *confidence = nullptr;
    }
    auto ptr = obj->obj.get();
    auto crs = dynamic_cast<const CRS *>(ptr);
    if (!crs) {
        proj_log_error(obj->ctx, __FUNCTION__, "Object is not a CRS");
    } else {
        int *confidenceTemp = nullptr;
        try {
            auto factory = AuthorityFactory::create(getDBcontext(obj->ctx),
                                                    auth_name ? auth_name : "");
            auto res = crs->identify(factory);
            std::vector<IdentifiedObjectNNPtr> objects;
            confidenceTemp = confidence ? new int[res.size()] : nullptr;
            size_t i = 0;
            for (const auto &pair : res) {
                objects.push_back(pair.first);
                if (confidenceTemp) {
                    confidenceTemp[i] = pair.second;
                    ++i;
                }
            }
            auto ret = internal::make_unique<PJ_OBJ_LIST>(obj->ctx,
                                                          std::move(objects));
            if (confidence) {
                *confidence = confidenceTemp;
                confidenceTemp = nullptr;
            }
            return ret.release();
        } catch (const std::exception &e) {
            delete[] confidenceTemp;
            proj_log_error(obj->ctx, __FUNCTION__, e.what());
        }
    }
    return nullptr;
}

// ---------------------------------------------------------------------------

/** \brief Free an array of integer. */
void proj_free_int_list(int *list) { delete[] list; }

// ---------------------------------------------------------------------------

static PROJ_STRING_LIST set_to_string_list(std::set<std::string> &&set) {
    auto ret = new char *[set.size() + 1];
    size_t i = 0;
    for (const auto &str : set) {
        ret[i] = new char[str.size() + 1];
        std::memcpy(ret[i], str.c_str(), str.size() + 1);
        i++;
    }
    ret[i] = nullptr;
    return ret;
}

// ---------------------------------------------------------------------------

/** \brief Return the list of authorities used in the database.
 *
 * The returned list is NULL terminated and must be freed with
 * proj_free_string_list().
 *
 * @param ctx PROJ context, or NULL for default context
 *
 * @return a NULL terminated list of NUL-terminated strings that must be
 * freed with proj_free_string_list(), or NULL in case of error.
 */
PROJ_STRING_LIST proj_get_authorities_from_database(PJ_CONTEXT *ctx) {
    SANITIZE_CTX(ctx);
    try {
        return set_to_string_list(getDBcontext(ctx)->getAuthorities());
    } catch (const std::exception &e) {
        proj_log_error(ctx, __FUNCTION__, e.what());
    }
    return nullptr;
}

// ---------------------------------------------------------------------------

/** \brief Returns the set of authority codes of the given object type.
 *
 * The returned list is NULL terminated and must be freed with
 * proj_free_string_list().
 *
 * @param ctx PROJ context, or NULL for default context.
 * @param auth_name Authority name (must not be NULL)
 * @param type Object type.
 * @param allow_deprecated whether we should return deprecated objects as well.
 *
 * @return a NULL terminated list of NUL-terminated strings that must be
 * freed with proj_free_string_list(), or NULL in case of error.
 */
PROJ_STRING_LIST proj_get_codes_from_database(PJ_CONTEXT *ctx,
                                              const char *auth_name,
                                              PJ_OBJ_TYPE type,
                                              int allow_deprecated) {
    assert(auth_name);
    SANITIZE_CTX(ctx);
    try {
        auto factory = AuthorityFactory::create(getDBcontext(ctx), auth_name);
        bool valid = false;
        auto typeInternal = convertPJObjectTypeToObjectType(type, valid);
        if (!valid) {
            return nullptr;
        }
        return set_to_string_list(
            factory->getAuthorityCodes(typeInternal, allow_deprecated != 0));

    } catch (const std::exception &e) {
        proj_log_error(ctx, __FUNCTION__, e.what());
    }
    return nullptr;
}

// ---------------------------------------------------------------------------

/** Free a list of NULL terminated strings. */
void proj_free_string_list(PROJ_STRING_LIST list) {
    if (list) {
        for (size_t i = 0; list[i] != nullptr; i++) {
            delete[] list[i];
        }
        delete[] list;
    }
}

// ---------------------------------------------------------------------------

/** \brief Return the Conversion of a DerivedCRS (such as a ProjectedCRS),
 * or the Transformation from the baseCRS to the hubCRS of a BoundCRS
 *
 * The returned object must be unreferenced with proj_obj_unref() after
 * use.
 * It should be used by at most one thread at a time.
 *
 * @param crs Objet of type DerivedCRS or BoundCRSs (must not be NULL)
 * @param pMethodName Pointer to a string value to store the method
 * (projection) name. or NULL
 * @param pMethodAuthorityName Pointer to a string value to store the method
 * authority name. or NULL
 * @param pMethodCode Pointer to a string value to store the method
 * code. or NULL
 * @return Object of type SingleOperation that must be unreferenced with
 * proj_obj_unref(), or NULL in case of error.
 */
PJ_OBJ *proj_obj_crs_get_coordoperation(const PJ_OBJ *crs,
                                        const char **pMethodName,
                                        const char **pMethodAuthorityName,
                                        const char **pMethodCode) {
    assert(crs);
    SingleOperationPtr co;

    auto derivedCRS = dynamic_cast<const DerivedCRS *>(crs->obj.get());
    if (derivedCRS) {
        co = derivedCRS->derivingConversion().as_nullable();
    } else {
        auto boundCRS = dynamic_cast<const BoundCRS *>(crs->obj.get());
        if (boundCRS) {
            co = boundCRS->transformation().as_nullable();
        } else {
            proj_log_error(crs->ctx, __FUNCTION__,
                           "Object is not a DerivedCRS or BoundCRS");
            return nullptr;
        }
    }

    const auto &method = co->method();
    const auto &method_ids = method->identifiers();
    if (pMethodName) {
        *pMethodName = method->name()->description()->c_str();
    }
    if (pMethodAuthorityName) {
        if (!method_ids.empty()) {
            *pMethodAuthorityName = method_ids[0]->codeSpace()->c_str();
        } else {
            *pMethodAuthorityName = nullptr;
        }
    }
    if (pMethodCode) {
        if (!method_ids.empty()) {
            *pMethodCode = method_ids[0]->code().c_str();
        } else {
            *pMethodCode = nullptr;
        }
    }
    return PJ_OBJ::create(crs->ctx, NN_NO_CHECK(co));
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
static PropertyMap createPropertyMapName(const char *name) {
    return PropertyMap().set(common::IdentifiedObject::NAME_KEY,
                             name ? name : "unnamed");
}

// ---------------------------------------------------------------------------

static UnitOfMeasure createLinearUnit(const char *name, double convFactor) {
    return name == nullptr
               ? UnitOfMeasure::METRE
               : UnitOfMeasure(name, convFactor, UnitOfMeasure::Type::LINEAR);
}

// ---------------------------------------------------------------------------

static UnitOfMeasure createAngularUnit(const char *name, double convFactor) {
    return name ? (ci_equal(name, "degree")
                       ? UnitOfMeasure::DEGREE
                       : ci_equal(name, "grad")
                             ? UnitOfMeasure::GRAD
                             : UnitOfMeasure(name, convFactor,
                                             UnitOfMeasure::Type::ANGULAR))
                : UnitOfMeasure::DEGREE;
}

// ---------------------------------------------------------------------------

static GeodeticReferenceFrameNNPtr createGeodeticReferenceFrame(
    PJ_CONTEXT *ctx, const char *datumName, const char *ellipsoidName,
    double semiMajorMetre, double invFlattening, const char *primeMeridianName,
    double primeMeridianOffset, const char *angularUnits,
    double angularUnitsConv) {
    const UnitOfMeasure angUnit(
        createAngularUnit(angularUnits, angularUnitsConv));
    auto dbContext = getDBcontext(ctx);
    auto body = Ellipsoid::guessBodyName(dbContext, semiMajorMetre);
    auto ellpsName = createPropertyMapName(ellipsoidName);
    auto ellps =
        invFlattening != 0.0
            ? Ellipsoid::createFlattenedSphere(
                  ellpsName, Length(semiMajorMetre), Scale(invFlattening), body)
            : Ellipsoid::createSphere(ellpsName, Length(semiMajorMetre), body);
    auto pm = PrimeMeridian::create(
        PropertyMap().set(common::IdentifiedObject::NAME_KEY,
                          primeMeridianName ? primeMeridianName
                                            : primeMeridianOffset == 0.0
                                                  ? (ellps->celestialBody() ==
                                                             Ellipsoid::EARTH
                                                         ? "Greenwich"
                                                         : "Reference meridian")
                                                  : "unnamed"),
        Angle(primeMeridianOffset, angUnit));
    return GeodeticReferenceFrame::create(createPropertyMapName(datumName),
                                          ellps, util::optional<std::string>(),
                                          pm);
}

//! @endcond

// ---------------------------------------------------------------------------

/** \brief Create a GeographicCRS.
 *
 * The returned object must be unreferenced with proj_obj_unref() after
 * use.
 * It should be used by at most one thread at a time.
 *
 * @param ctx PROJ context, or NULL for default context
 * @param crsName Name of the GeographicCRS. Or NULL
 * @param datumName Name of the GeodeticReferenceFrame. Or NULL
 * @param ellipsoidName Name of the Ellipsoid. Or NULL
 * @param semiMajorMetre Ellipsoid semi-major axis, in metres.
 * @param invFlattening Ellipsoid inverse flattening. Or 0 for a sphere.
 * @param primeMeridianName Name of the PrimeMeridian. Or NULL
 * @param primeMeridianOffset Offset of the prime meridian, expressed in the
 * specified angular units.
 * @param pmAngularUnits Name of the angular units. Or NULL for Degree
 * @param pmAngularUnitsConv Conversion factor from the angular unit to radian.
 * Or
 * 0 for Degree if angularUnits == NULL. Otherwise should be not NULL
 * @param ellipsoidalCS Coordinate system. Must not be NULL.
 *
 * @return Object of type GeographicCRS that must be unreferenced with
 * proj_obj_unref(), or NULL in case of error.
 */
PJ_OBJ *proj_obj_create_geographic_crs(
    PJ_CONTEXT *ctx, const char *crsName, const char *datumName,
    const char *ellipsoidName, double semiMajorMetre, double invFlattening,
    const char *primeMeridianName, double primeMeridianOffset,
    const char *pmAngularUnits, double pmAngularUnitsConv,
    PJ_OBJ *ellipsoidalCS) {

    SANITIZE_CTX(ctx);
    auto cs = util::nn_dynamic_pointer_cast<EllipsoidalCS>(ellipsoidalCS->obj);
    if (!cs) {
        return nullptr;
    }
    try {
        auto datum = createGeodeticReferenceFrame(
            ctx, datumName, ellipsoidName, semiMajorMetre, invFlattening,
            primeMeridianName, primeMeridianOffset, pmAngularUnits,
            pmAngularUnitsConv);
        auto geogCRS = GeographicCRS::create(createPropertyMapName(crsName),
                                             datum, NN_NO_CHECK(cs));
        return PJ_OBJ::create(ctx, geogCRS);
    } catch (const std::exception &e) {
        proj_log_error(ctx, __FUNCTION__, e.what());
    }
    return nullptr;
}

// ---------------------------------------------------------------------------

/** \brief Create a GeographicCRS.
 *
 * The returned object must be unreferenced with proj_obj_unref() after
 * use.
 * It should be used by at most one thread at a time.
 *
 * @param crsName Name of the GeographicCRS. Or NULL
 * @param datum Datum. Must not be NULL.
 * @param ellipsoidalCS Coordinate system. Must not be NULL.
 *
 * @return Object of type GeographicCRS that must be unreferenced with
 * proj_obj_unref(), or NULL in case of error.
 */
PJ_OBJ *proj_obj_create_geographic_crs_from_datum(const char *crsName,
                                                  PJ_OBJ *datum,
                                                  PJ_OBJ *ellipsoidalCS) {

    auto l_datum =
        util::nn_dynamic_pointer_cast<GeodeticReferenceFrame>(datum->obj);
    if (!l_datum) {
        proj_log_error(datum->ctx, __FUNCTION__,
                       "datum is not a GeodeticReferenceFrame");
        return nullptr;
    }
    auto cs = util::nn_dynamic_pointer_cast<EllipsoidalCS>(ellipsoidalCS->obj);
    if (!cs) {
        return nullptr;
    }
    try {
        auto geogCRS =
            GeographicCRS::create(createPropertyMapName(crsName),
                                  NN_NO_CHECK(l_datum), NN_NO_CHECK(cs));
        return PJ_OBJ::create(datum->ctx, geogCRS);
    } catch (const std::exception &e) {
        proj_log_error(datum->ctx, __FUNCTION__, e.what());
    }
    return nullptr;
}

// ---------------------------------------------------------------------------

/** \brief Create a GeodeticCRS of geocentric type.
 *
 * The returned object must be unreferenced with proj_obj_unref() after
 * use.
 * It should be used by at most one thread at a time.
 *
 * @param ctx PROJ context, or NULL for default context
 * @param crsName Name of the GeographicCRS. Or NULL
 * @param datumName Name of the GeodeticReferenceFrame. Or NULL
 * @param ellipsoidName Name of the Ellipsoid. Or NULL
 * @param semiMajorMetre Ellipsoid semi-major axis, in metres.
 * @param invFlattening Ellipsoid inverse flattening. Or 0 for a sphere.
 * @param primeMeridianName Name of the PrimeMeridian. Or NULL
 * @param primeMeridianOffset Offset of the prime meridian, expressed in the
 * specified angular units.
 * @param angularUnits Name of the angular units. Or NULL for Degree
 * @param angularUnitsConv Conversion factor from the angular unit to radian. Or
 * 0 for Degree if angularUnits == NULL. Otherwise should be not NULL
 * @param linearUnits Name of the linear units. Or NULL for Metre
 * @param linearUnitsConv Conversion factor from the linear unit to metre. Or
 * 0 for Metre if linearUnits == NULL. Otherwise should be not NULL
 *
 * @return Object of type GeodeticCRS that must be unreferenced with
 * proj_obj_unref(), or NULL in case of error.
 */
PJ_OBJ *proj_obj_create_geocentric_crs(
    PJ_CONTEXT *ctx, const char *crsName, const char *datumName,
    const char *ellipsoidName, double semiMajorMetre, double invFlattening,
    const char *primeMeridianName, double primeMeridianOffset,
    const char *angularUnits, double angularUnitsConv, const char *linearUnits,
    double linearUnitsConv) {

    SANITIZE_CTX(ctx);
    try {
        const UnitOfMeasure linearUnit(
            createLinearUnit(linearUnits, linearUnitsConv));
        auto datum = createGeodeticReferenceFrame(
            ctx, datumName, ellipsoidName, semiMajorMetre, invFlattening,
            primeMeridianName, primeMeridianOffset, angularUnits,
            angularUnitsConv);

        auto geodCRS =
            GeodeticCRS::create(createPropertyMapName(crsName), datum,
                                cs::CartesianCS::createGeocentric(linearUnit));
        return PJ_OBJ::create(ctx, geodCRS);
    } catch (const std::exception &e) {
        proj_log_error(ctx, __FUNCTION__, e.what());
    }
    return nullptr;
}

// ---------------------------------------------------------------------------

/** \brief Create a GeodeticCRS of geocentric type.
 *
 * The returned object must be unreferenced with proj_obj_unref() after
 * use.
 * It should be used by at most one thread at a time.
 *
 * @param crsName Name of the GeographicCRS. Or NULL
 * @param datum Datum. Must not be NULL.
 * @param linearUnits Name of the linear units. Or NULL for Metre
 * @param linearUnitsConv Conversion factor from the linear unit to metre. Or
 * 0 for Metre if linearUnits == NULL. Otherwise should be not NULL
 *
 * @return Object of type GeodeticCRS that must be unreferenced with
 * proj_obj_unref(), or NULL in case of error.
 */
PJ_OBJ *proj_obj_create_geocentric_crs_from_datum(const char *crsName,
                                                  const PJ_OBJ *datum,
                                                  const char *linearUnits,
                                                  double linearUnitsConv) {
    try {
        const UnitOfMeasure linearUnit(
            createLinearUnit(linearUnits, linearUnitsConv));
        auto l_datum =
            util::nn_dynamic_pointer_cast<GeodeticReferenceFrame>(datum->obj);
        if (!l_datum) {
            proj_log_error(datum->ctx, __FUNCTION__,
                           "datum is not a GeodeticReferenceFrame");
            return nullptr;
        }
        auto geodCRS = GeodeticCRS::create(
            createPropertyMapName(crsName), NN_NO_CHECK(l_datum),
            cs::CartesianCS::createGeocentric(linearUnit));
        return PJ_OBJ::create(datum->ctx, geodCRS);
    } catch (const std::exception &e) {
        proj_log_error(datum->ctx, __FUNCTION__, e.what());
    }
    return nullptr;
}

// ---------------------------------------------------------------------------

/** \brief Return a copy of the object with its name changed
 *
 * Currently, only implemented on CRS objects.
 *
 * The returned object must be unreferenced with proj_obj_unref() after
 * use.
 * It should be used by at most one thread at a time.
 *
 * @param obj Object of type CRS. Must not be NULL
 * @param name New name. Must not be NULL
 *
 * @return Object that must be unreferenced with
 * proj_obj_unref(), or NULL in case of error.
 */
PJ_OBJ PROJ_DLL *proj_obj_alter_name(const PJ_OBJ *obj, const char *name) {
    auto crs = dynamic_cast<const CRS *>(obj->obj.get());
    if (!crs) {
        return nullptr;
    }
    try {
        return PJ_OBJ::create(obj->ctx, crs->alterName(name));
    } catch (const std::exception &e) {
        proj_log_error(obj->ctx, __FUNCTION__, e.what());
    }
    return nullptr;
}

// ---------------------------------------------------------------------------

/** \brief Return a copy of the CRS with its geodetic CRS changed
 *
 * Currently, when obj is a GeodeticCRS, it returns a clone of newGeodCRS
 * When obj is a ProjectedCRS, it replaces its base CRS with newGeodCRS.
 * When obj is a CompoundCRS, it replaces the GeodeticCRS part of the horizontal
 * CRS with newGeodCRS.
 * In other cases, it returns a clone of obj.
 *
 * The returned object must be unreferenced with proj_obj_unref() after
 * use.
 * It should be used by at most one thread at a time.
 *
 * @param obj Object of type CRS. Must not be NULL
 * @param newGeodCRS Object of type GeodeticCRS. Must not be NULL
 *
 * @return Object that must be unreferenced with
 * proj_obj_unref(), or NULL in case of error.
 */
PJ_OBJ *proj_obj_crs_alter_geodetic_crs(const PJ_OBJ *obj,
                                        const PJ_OBJ *newGeodCRS) {
    auto l_newGeodCRS =
        util::nn_dynamic_pointer_cast<GeodeticCRS>(newGeodCRS->obj);
    if (!l_newGeodCRS) {
        proj_log_error(obj->ctx, __FUNCTION__,
                       "newGeodCRS is not a GeodeticCRS");
        return nullptr;
    }

    auto crs = dynamic_cast<const CRS *>(obj->obj.get());
    if (!crs) {
        proj_log_error(obj->ctx, __FUNCTION__, "obj is not a CRS");
        return nullptr;
    }

    try {
        return PJ_OBJ::create(obj->ctx,
                              crs->alterGeodeticCRS(NN_NO_CHECK(l_newGeodCRS)));
    } catch (const std::exception &e) {
        proj_log_error(obj->ctx, __FUNCTION__, e.what());
        return nullptr;
    }
}

// ---------------------------------------------------------------------------

/** \brief Return a copy of the CRS with its angular units changed
 *
 * The CRS must be or contain a GeographicCRS.
 *
 * The returned object must be unreferenced with proj_obj_unref() after
 * use.
 * It should be used by at most one thread at a time.
 *
 * @param obj Object of type CRS. Must not be NULL
 * @param angularUnits Name of the angular units. Or NULL for Degree
 * @param angularUnitsConv Conversion factor from the angular unit to radian. Or
 * 0 for Degree if angularUnits == NULL. Otherwise should be not NULL
 *
 * @return Object that must be unreferenced with
 * proj_obj_unref(), or NULL in case of error.
 */
PJ_OBJ *proj_obj_crs_alter_cs_angular_unit(const PJ_OBJ *obj,
                                           const char *angularUnits,
                                           double angularUnitsConv) {

    auto geodCRS = proj_obj_crs_get_geodetic_crs(obj);
    if (!geodCRS) {
        return nullptr;
    }
    auto geogCRS = dynamic_cast<const GeographicCRS *>(geodCRS->obj.get());
    if (!geogCRS) {
        proj_obj_unref(geodCRS);
        return nullptr;
    }

    PJ_OBJ *geogCRSAltered = nullptr;
    try {
        const UnitOfMeasure angUnit(
            createAngularUnit(angularUnits, angularUnitsConv));
        geogCRSAltered = PJ_OBJ::create(
            obj->ctx,
            GeographicCRS::create(
                createPropertyMapName(proj_obj_get_name(geodCRS)),
                geogCRS->datum(), geogCRS->datumEnsemble(),
                geogCRS->coordinateSystem()->alterAngularUnit(angUnit)));
        proj_obj_unref(geodCRS);
    } catch (const std::exception &e) {
        proj_log_error(obj->ctx, __FUNCTION__, e.what());
        proj_obj_unref(geodCRS);
        return nullptr;
    }

    auto ret = proj_obj_crs_alter_geodetic_crs(obj, geogCRSAltered);
    proj_obj_unref(geogCRSAltered);
    return ret;
}

// ---------------------------------------------------------------------------

/** \brief Return a copy of the CRS with the linear units of its coordinate
 * system changed
 *
 * The CRS must be or contain a ProjectedCRS, VerticalCRS or a GeocentricCRS.
 *
 * The returned object must be unreferenced with proj_obj_unref() after
 * use.
 * It should be used by at most one thread at a time.
 *
 * @param obj Object of type CRS. Must not be NULL
 * @param linearUnits Name of the linear units. Or NULL for Metre
 * @param linearUnitsConv Conversion factor from the linear unit to metre. Or
 * 0 for Metre if linearUnits == NULL. Otherwise should be not NULL
 *
 * @return Object that must be unreferenced with
 * proj_obj_unref(), or NULL in case of error.
 */
PJ_OBJ *proj_obj_crs_alter_cs_linear_unit(const PJ_OBJ *obj,
                                          const char *linearUnits,
                                          double linearUnitsConv) {
    auto crs = dynamic_cast<const CRS *>(obj->obj.get());
    if (!crs) {
        return nullptr;
    }

    try {
        const UnitOfMeasure linearUnit(
            createLinearUnit(linearUnits, linearUnitsConv));
        return PJ_OBJ::create(obj->ctx, crs->alterCSLinearUnit(linearUnit));
    } catch (const std::exception &e) {
        proj_log_error(obj->ctx, __FUNCTION__, e.what());
        return nullptr;
    }
}

// ---------------------------------------------------------------------------

/** \brief Return a copy of the CRS with the lineaer units of the parameters
 * of its conversion modified.
 *
 * The CRS must be or contain a ProjectedCRS, VerticalCRS or a GeocentricCRS.
 *
 * The returned object must be unreferenced with proj_obj_unref() after
 * use.
 * It should be used by at most one thread at a time.
 *
 * @param obj Object of type ProjectedCRS. Must not be NULL
 * @param linearUnits Name of the linear units. Or NULL for Metre
 * @param linearUnitsConv Conversion factor from the linear unit to metre. Or
 * 0 for Metre if linearUnits == NULL. Otherwise should be not NULL
 * @param convertToNewUnit TRUE if exisiting values should be converted from
 * their current unit to the new unit. If FALSE, their value will be left
 * unchanged and the unit overriden (so the resulting CRS will not be
 * equivalent to the original one for reprojection purposes).
 *
 * @return Object that must be unreferenced with
 * proj_obj_unref(), or NULL in case of error.
 */
PJ_OBJ *proj_obj_crs_alter_parameters_linear_unit(const PJ_OBJ *obj,
                                                  const char *linearUnits,
                                                  double linearUnitsConv,
                                                  int convertToNewUnit) {
    auto crs = dynamic_cast<const ProjectedCRS *>(obj->obj.get());
    if (!crs) {
        return nullptr;
    }

    try {
        const UnitOfMeasure linearUnit(
            createLinearUnit(linearUnits, linearUnitsConv));
        return PJ_OBJ::create(
            obj->ctx, crs->alterParametersLinearUnit(linearUnit,
                                                     convertToNewUnit == TRUE));
    } catch (const std::exception &e) {
        proj_log_error(obj->ctx, __FUNCTION__, e.what());
        return nullptr;
    }
}

// ---------------------------------------------------------------------------

/** \brief Instanciate a EngineeringCRS with just a name
 *
 * The returned object must be unreferenced with proj_obj_unref() after
 * use.
 * It should be used by at most one thread at a time.
 *
 * @param ctx PROJ context, or NULL for default context
 * @param crsName CRS name. Or NULL.
 *
 * @return Object that must be unreferenced with
 * proj_obj_unref(), or NULL in case of error.
 */
PJ_OBJ PROJ_DLL *proj_obj_create_engineering_crs(PJ_CONTEXT *ctx,
                                                 const char *crsName) {
    try {
        return PJ_OBJ::create(
            ctx, EngineeringCRS::create(
                     createPropertyMapName(crsName),
                     EngineeringDatum::create(PropertyMap()),
                     CartesianCS::createEastingNorthing(UnitOfMeasure::METRE)));
    } catch (const std::exception &e) {
        proj_log_error(ctx, __FUNCTION__, e.what());
        return nullptr;
    }
}

// ---------------------------------------------------------------------------

/** \brief Instanciate a Conversion
 *
 * The returned object must be unreferenced with proj_obj_unref() after
 * use.
 * It should be used by at most one thread at a time.
 *
 * @param ctx PROJ context, or NULL for default context
 * @param name Conversion name. Or NULL.
 * @param auth_name Conversion authority name. Or NULL.
 * @param code Conversion code. Or NULL.
 * @param method_name Method name. Or NULL.
 * @param method_auth_name Method authority name. Or NULL.
 * @param method_code Method code. Or NULL.
 * @param param_count Number of parameters (size of params argument)
 * @param params Parameter descriptions (array of size param_count)
 *
 * @return Object that must be unreferenced with
 * proj_obj_unref(), or NULL in case of error.
 */

PJ_OBJ *proj_obj_create_conversion(PJ_CONTEXT *ctx, const char *name,
                                   const char *auth_name, const char *code,
                                   const char *method_name,
                                   const char *method_auth_name,
                                   const char *method_code, int param_count,
                                   const PJ_PARAM_DESCRIPTION *params) {
    try {
        PropertyMap propConv;
        propConv.set(common::IdentifiedObject::NAME_KEY,
                     name ? name : "unnamed");
        if (auth_name && code) {
            propConv.set(metadata::Identifier::CODESPACE_KEY, auth_name)
                .set(metadata::Identifier::CODE_KEY, code);
        }
        PropertyMap propMethod;
        propMethod.set(common::IdentifiedObject::NAME_KEY,
                       method_name ? method_name : "unnamed");
        if (method_auth_name && method_code) {
            propMethod
                .set(metadata::Identifier::CODESPACE_KEY, method_auth_name)
                .set(metadata::Identifier::CODE_KEY, method_code);
        }
        std::vector<OperationParameterNNPtr> parameters;
        std::vector<ParameterValueNNPtr> values;
        for (int i = 0; i < param_count; i++) {
            PropertyMap propParam;
            propParam.set(common::IdentifiedObject::NAME_KEY,
                          params[i].name ? params[i].name : "unnamed");
            if (params[i].auth_name && params[i].code) {
                propParam
                    .set(metadata::Identifier::CODESPACE_KEY,
                         params[i].auth_name)
                    .set(metadata::Identifier::CODE_KEY, params[i].code);
            }
            parameters.emplace_back(OperationParameter::create(propParam));
            auto unit_type = UnitOfMeasure::Type::UNKNOWN;
            switch (params[i].unit_type) {
            case PJ_UT_ANGULAR:
                unit_type = UnitOfMeasure::Type::ANGULAR;
                break;
            case PJ_UT_LINEAR:
                unit_type = UnitOfMeasure::Type::LINEAR;
                break;
            case PJ_UT_SCALE:
                unit_type = UnitOfMeasure::Type::SCALE;
                break;
            case PJ_UT_TIME:
                unit_type = UnitOfMeasure::Type::TIME;
                break;
            case PJ_UT_PARAMETRIC:
                unit_type = UnitOfMeasure::Type::PARAMETRIC;
                break;
            }

            Measure measure(
                params[i].value,
                params[i].unit_type == PJ_UT_ANGULAR
                    ? createAngularUnit(params[i].unit_name,
                                        params[i].unit_conv_factor)
                    : params[i].unit_type == PJ_UT_LINEAR
                          ? createLinearUnit(params[i].unit_name,
                                             params[i].unit_conv_factor)
                          : UnitOfMeasure(
                                params[i].unit_name ? params[i].unit_name
                                                    : "unnamed",
                                params[i].unit_conv_factor, unit_type));
            values.emplace_back(ParameterValue::create(measure));
        }
        return PJ_OBJ::create(
            ctx, Conversion::create(propConv, propMethod, parameters, values));
    } catch (const std::exception &e) {
        proj_log_error(ctx, __FUNCTION__, e.what());
        return nullptr;
    }
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress

static CoordinateSystemAxisNNPtr createAxis(const PJ_AXIS_DESCRIPTION &axis) {
    const auto dir =
        axis.direction ? AxisDirection::valueOf(axis.direction) : nullptr;
    if (dir == nullptr)
        throw Exception("invalid value for axis direction");
    auto unit_type = UnitOfMeasure::Type::UNKNOWN;
    switch (axis.unit_type) {
    case PJ_UT_ANGULAR:
        unit_type = UnitOfMeasure::Type::ANGULAR;
        break;
    case PJ_UT_LINEAR:
        unit_type = UnitOfMeasure::Type::LINEAR;
        break;
    case PJ_UT_SCALE:
        unit_type = UnitOfMeasure::Type::SCALE;
        break;
    case PJ_UT_TIME:
        unit_type = UnitOfMeasure::Type::TIME;
        break;
    case PJ_UT_PARAMETRIC:
        unit_type = UnitOfMeasure::Type::PARAMETRIC;
        break;
    }
    auto unit =
        axis.unit_type == PJ_UT_ANGULAR
            ? createAngularUnit(axis.unit_name, axis.unit_conv_factor)
            : axis.unit_type == PJ_UT_LINEAR
                  ? createLinearUnit(axis.unit_name, axis.unit_conv_factor)
                  : UnitOfMeasure(axis.unit_name ? axis.unit_name : "unnamed",
                                  axis.unit_conv_factor, unit_type);

    return CoordinateSystemAxis::create(
        createPropertyMapName(axis.name),
        axis.abbreviation ? axis.abbreviation : std::string(), *dir, unit);
}

//! @endcond

// ---------------------------------------------------------------------------

/** \brief Instanciate a CoordinateSystem.
 *
 * The returned object must be unreferenced with proj_obj_unref() after
 * use.
 * It should be used by at most one thread at a time.
 *
 * @param ctx PROJ context, or NULL for default context
 * @param type Coordinate system type.
 * @param axis_count Number of axis
 * @param axis Axis description (array of size axis_count)
 *
 * @return Object that must be unreferenced with
 * proj_obj_unref(), or NULL in case of error.
 */

PJ_OBJ *proj_obj_create_cs(PJ_CONTEXT *ctx, PJ_COORDINATE_SYSTEM_TYPE type,
                           int axis_count, const PJ_AXIS_DESCRIPTION *axis) {
    try {
        switch (type) {
        case PJ_CS_TYPE_UNKNOWN:
            return nullptr;

        case PJ_CS_TYPE_CARTESIAN: {
            if (axis_count == 2) {
                return PJ_OBJ::create(
                    ctx, CartesianCS::create(PropertyMap(), createAxis(axis[0]),
                                             createAxis(axis[1])));
            } else if (axis_count == 3) {
                return PJ_OBJ::create(
                    ctx, CartesianCS::create(PropertyMap(), createAxis(axis[0]),
                                             createAxis(axis[1]),
                                             createAxis(axis[2])));
            }
            break;
        }

        case PJ_CS_TYPE_ELLIPSOIDAL: {
            if (axis_count == 2) {
                return PJ_OBJ::create(
                    ctx,
                    EllipsoidalCS::create(PropertyMap(), createAxis(axis[0]),
                                          createAxis(axis[1])));
            } else if (axis_count == 3) {
                return PJ_OBJ::create(
                    ctx, EllipsoidalCS::create(
                             PropertyMap(), createAxis(axis[0]),
                             createAxis(axis[1]), createAxis(axis[2])));
            }
            break;
        }

        case PJ_CS_TYPE_VERTICAL: {
            if (axis_count == 1) {
                return PJ_OBJ::create(
                    ctx,
                    VerticalCS::create(PropertyMap(), createAxis(axis[0])));
            }
            break;
        }

        case PJ_CS_TYPE_SPHERICAL: {
            if (axis_count == 3) {
                return PJ_OBJ::create(
                    ctx, EllipsoidalCS::create(
                             PropertyMap(), createAxis(axis[0]),
                             createAxis(axis[1]), createAxis(axis[2])));
            }
            break;
        }

        case PJ_CS_TYPE_PARAMETRIC: {
            if (axis_count == 1) {
                return PJ_OBJ::create(
                    ctx,
                    ParametricCS::create(PropertyMap(), createAxis(axis[0])));
            }
            break;
        }

        case PJ_CS_TYPE_ORDINAL: {
            std::vector<CoordinateSystemAxisNNPtr> axisVector;
            for (int i = 0; i < axis_count; i++) {
                axisVector.emplace_back(createAxis(axis[i]));
            }

            return PJ_OBJ::create(ctx,
                                  OrdinalCS::create(PropertyMap(), axisVector));
        }

        case PJ_CS_TYPE_DATETIMETEMPORAL: {
            if (axis_count == 1) {
                return PJ_OBJ::create(
                    ctx, DateTimeTemporalCS::create(PropertyMap(),
                                                    createAxis(axis[0])));
            }
            break;
        }

        case PJ_CS_TYPE_TEMPORALCOUNT: {
            if (axis_count == 1) {
                return PJ_OBJ::create(
                    ctx, TemporalCountCS::create(PropertyMap(),
                                                 createAxis(axis[0])));
            }
            break;
        }

        case PJ_CS_TYPE_TEMPORALMEASURE: {
            if (axis_count == 1) {
                return PJ_OBJ::create(
                    ctx, TemporalMeasureCS::create(PropertyMap(),
                                                   createAxis(axis[0])));
            }
            break;
        }
        }

    } catch (const std::exception &e) {
        proj_log_error(ctx, __FUNCTION__, e.what());
        return nullptr;
    }
    proj_log_error(ctx, __FUNCTION__, "Wrong value for axis_count");
    return nullptr;
}

// ---------------------------------------------------------------------------

/** \brief Instanciate a CartesiansCS 2D
 *
 * The returned object must be unreferenced with proj_obj_unref() after
 * use.
 * It should be used by at most one thread at a time.
 *
 * @param ctx PROJ context, or NULL for default context
 * @param type Coordinate system type.
 * @param unit_name Unit name.
 * @param unit_conv_factor Unit conversion factor to SI.
 *
 * @return Object that must be unreferenced with
 * proj_obj_unref(), or NULL in case of error.
 */

PJ_OBJ *proj_obj_create_cartesian_2D_cs(PJ_CONTEXT *ctx,
                                        PJ_CARTESIAN_CS_2D_TYPE type,
                                        const char *unit_name,
                                        double unit_conv_factor) {
    try {
        switch (type) {
        case PJ_CART2D_EASTING_NORTHING:
            return PJ_OBJ::create(
                ctx, CartesianCS::createEastingNorthing(
                         createLinearUnit(unit_name, unit_conv_factor)));

        case PJ_CART2D_NORTHING_EASTING:
            return PJ_OBJ::create(
                ctx, CartesianCS::createNorthingEasting(
                         createLinearUnit(unit_name, unit_conv_factor)));
        }
    } catch (const std::exception &e) {
        proj_log_error(ctx, __FUNCTION__, e.what());
    }
    return nullptr;
}

// ---------------------------------------------------------------------------

/** \brief Instanciate a Ellipsoidal 2D
 *
 * The returned object must be unreferenced with proj_obj_unref() after
 * use.
 * It should be used by at most one thread at a time.
 *
 * @param ctx PROJ context, or NULL for default context
 * @param type Coordinate system type.
 * @param unit_name Unit name.
 * @param unit_conv_factor Unit conversion factor to SI.
 *
 * @return Object that must be unreferenced with
 * proj_obj_unref(), or NULL in case of error.
 */

PJ_OBJ *proj_obj_create_ellipsoidal_2D_cs(PJ_CONTEXT *ctx,
                                          PJ_ELLIPSOIDAL_CS_2D_TYPE type,
                                          const char *unit_name,
                                          double unit_conv_factor) {
    try {
        switch (type) {
        case PJ_ELLPS2D_LONGITUDE_LATITUDE:
            return PJ_OBJ::create(
                ctx, EllipsoidalCS::createLongitudeLatitude(
                         createAngularUnit(unit_name, unit_conv_factor)));

        case PJ_ELLPS2D_LATITUDE_LONGITUDE:
            return PJ_OBJ::create(
                ctx, EllipsoidalCS::createLatitudeLongitude(
                         createAngularUnit(unit_name, unit_conv_factor)));
        }
    } catch (const std::exception &e) {
        proj_log_error(ctx, __FUNCTION__, e.what());
    }
    return nullptr;
}

// ---------------------------------------------------------------------------

/** \brief Instanciate a ProjectedCRS
 *
 * The returned object must be unreferenced with proj_obj_unref() after
 * use.
 * It should be used by at most one thread at a time.
 *
 * @param crs_name CRS name. Or NULL
 * @param geodetic_crs Base GeodeticCRS. Must not be NULL.
 * @param conversion Conversion. Must not be NULL.
 * @param coordinate_system Cartesian coordinate system. Must not be NULL.
 *
 * @return Object that must be unreferenced with
 * proj_obj_unref(), or NULL in case of error.
 */

PJ_OBJ *proj_obj_create_projected_crs(const char *crs_name,
                                      const PJ_OBJ *geodetic_crs,
                                      const PJ_OBJ *conversion,
                                      const PJ_OBJ *coordinate_system) {
    auto geodCRS =
        util::nn_dynamic_pointer_cast<GeodeticCRS>(geodetic_crs->obj);
    if (!geodCRS) {
        return nullptr;
    }
    auto conv = util::nn_dynamic_pointer_cast<Conversion>(conversion->obj);
    if (!conv) {
        return nullptr;
    }
    auto cs =
        util::nn_dynamic_pointer_cast<CartesianCS>(coordinate_system->obj);
    if (!cs) {
        return nullptr;
    }
    try {
        return PJ_OBJ::create(
            geodetic_crs->ctx,
            ProjectedCRS::create(createPropertyMapName(crs_name),
                                 NN_NO_CHECK(geodCRS), NN_NO_CHECK(conv),
                                 NN_NO_CHECK(cs)));
    } catch (const std::exception &e) {
        proj_log_error(geodetic_crs->ctx, __FUNCTION__, e.what());
    }
    return nullptr;
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress

static PJ_OBJ *proj_obj_create_conversion(PJ_CONTEXT *ctx,
                                          const ConversionNNPtr &conv) {
    return PJ_OBJ::create(ctx, conv);
}

//! @endcond

/* BEGIN: Generated by scripts/create_c_api_projections.py*/

// ---------------------------------------------------------------------------

/** \brief Instanciate a ProjectedCRS with a Universal Transverse Mercator
 * conversion.
 *
 * See osgeo::proj::operation::Conversion::createUTM().
 *
 * Linear parameters are expressed in (linearUnitName, linearUnitConvFactor).
 */
PJ_OBJ *proj_obj_create_conversion_utm(PJ_CONTEXT *ctx, int zone, int north) {
    try {
        auto conv = Conversion::createUTM(PropertyMap(), zone, north != 0);
        return proj_obj_create_conversion(ctx, conv);
    } catch (const std::exception &e) {
        proj_log_error(ctx, __FUNCTION__, e.what());
    }
    return nullptr;
}
// ---------------------------------------------------------------------------

/** \brief Instanciate a ProjectedCRS with a conversion based on the Transverse
 * Mercator projection method.
 *
 * See osgeo::proj::operation::Conversion::createTransverseMercator().
 *
 * Linear parameters are expressed in (linearUnitName, linearUnitConvFactor).
 * Angular parameters are expressed in (angUnitName, angUnitConvFactor).
 */
PJ_OBJ *proj_obj_create_conversion_transverse_mercator(
    PJ_CONTEXT *ctx, double centerLat, double centerLong, double scale,
    double falseEasting, double falseNorthing, const char *angUnitName,
    double angUnitConvFactor, const char *linearUnitName,
    double linearUnitConvFactor) {
    try {
        UnitOfMeasure linearUnit(
            createLinearUnit(linearUnitName, linearUnitConvFactor));
        UnitOfMeasure angUnit(
            createAngularUnit(angUnitName, angUnitConvFactor));
        auto conv = Conversion::createTransverseMercator(
            PropertyMap(), Angle(centerLat, angUnit),
            Angle(centerLong, angUnit), Scale(scale),
            Length(falseEasting, linearUnit),
            Length(falseNorthing, linearUnit));
        return proj_obj_create_conversion(ctx, conv);
    } catch (const std::exception &e) {
        proj_log_error(ctx, __FUNCTION__, e.what());
    }
    return nullptr;
}
// ---------------------------------------------------------------------------

/** \brief Instanciate a ProjectedCRS with a conversion based on the Gauss
 * Schreiber Transverse Mercator projection method.
 *
 * See
 * osgeo::proj::operation::Conversion::createGaussSchreiberTransverseMercator().
 *
 * Linear parameters are expressed in (linearUnitName, linearUnitConvFactor).
 * Angular parameters are expressed in (angUnitName, angUnitConvFactor).
 */
PJ_OBJ *proj_obj_create_conversion_gauss_schreiber_transverse_mercator(
    PJ_CONTEXT *ctx, double centerLat, double centerLong, double scale,
    double falseEasting, double falseNorthing, const char *angUnitName,
    double angUnitConvFactor, const char *linearUnitName,
    double linearUnitConvFactor) {
    try {
        UnitOfMeasure linearUnit(
            createLinearUnit(linearUnitName, linearUnitConvFactor));
        UnitOfMeasure angUnit(
            createAngularUnit(angUnitName, angUnitConvFactor));
        auto conv = Conversion::createGaussSchreiberTransverseMercator(
            PropertyMap(), Angle(centerLat, angUnit),
            Angle(centerLong, angUnit), Scale(scale),
            Length(falseEasting, linearUnit),
            Length(falseNorthing, linearUnit));
        return proj_obj_create_conversion(ctx, conv);
    } catch (const std::exception &e) {
        proj_log_error(ctx, __FUNCTION__, e.what());
    }
    return nullptr;
}
// ---------------------------------------------------------------------------

/** \brief Instanciate a ProjectedCRS with a conversion based on the Transverse
 * Mercator South Orientated projection method.
 *
 * See
 * osgeo::proj::operation::Conversion::createTransverseMercatorSouthOriented().
 *
 * Linear parameters are expressed in (linearUnitName, linearUnitConvFactor).
 * Angular parameters are expressed in (angUnitName, angUnitConvFactor).
 */
PJ_OBJ *proj_obj_create_conversion_transverse_mercator_south_oriented(
    PJ_CONTEXT *ctx, double centerLat, double centerLong, double scale,
    double falseEasting, double falseNorthing, const char *angUnitName,
    double angUnitConvFactor, const char *linearUnitName,
    double linearUnitConvFactor) {
    try {
        UnitOfMeasure linearUnit(
            createLinearUnit(linearUnitName, linearUnitConvFactor));
        UnitOfMeasure angUnit(
            createAngularUnit(angUnitName, angUnitConvFactor));
        auto conv = Conversion::createTransverseMercatorSouthOriented(
            PropertyMap(), Angle(centerLat, angUnit),
            Angle(centerLong, angUnit), Scale(scale),
            Length(falseEasting, linearUnit),
            Length(falseNorthing, linearUnit));
        return proj_obj_create_conversion(ctx, conv);
    } catch (const std::exception &e) {
        proj_log_error(ctx, __FUNCTION__, e.what());
    }
    return nullptr;
}
// ---------------------------------------------------------------------------

/** \brief Instanciate a ProjectedCRS with a conversion based on the Two Point
 * Equidistant projection method.
 *
 * See osgeo::proj::operation::Conversion::createTwoPointEquidistant().
 *
 * Linear parameters are expressed in (linearUnitName, linearUnitConvFactor).
 * Angular parameters are expressed in (angUnitName, angUnitConvFactor).
 */
PJ_OBJ *proj_obj_create_conversion_two_point_equidistant(
    PJ_CONTEXT *ctx, double latitudeFirstPoint, double longitudeFirstPoint,
    double latitudeSecondPoint, double longitudeSeconPoint, double falseEasting,
    double falseNorthing, const char *angUnitName, double angUnitConvFactor,
    const char *linearUnitName, double linearUnitConvFactor) {
    try {
        UnitOfMeasure linearUnit(
            createLinearUnit(linearUnitName, linearUnitConvFactor));
        UnitOfMeasure angUnit(
            createAngularUnit(angUnitName, angUnitConvFactor));
        auto conv = Conversion::createTwoPointEquidistant(
            PropertyMap(), Angle(latitudeFirstPoint, angUnit),
            Angle(longitudeFirstPoint, angUnit),
            Angle(latitudeSecondPoint, angUnit),
            Angle(longitudeSeconPoint, angUnit),
            Length(falseEasting, linearUnit),
            Length(falseNorthing, linearUnit));
        return proj_obj_create_conversion(ctx, conv);
    } catch (const std::exception &e) {
        proj_log_error(ctx, __FUNCTION__, e.what());
    }
    return nullptr;
}
// ---------------------------------------------------------------------------

/** \brief Instanciate a ProjectedCRS with a conversion based on the Tunisia
 * Mapping Grid projection method.
 *
 * See osgeo::proj::operation::Conversion::createTunisiaMappingGrid().
 *
 * Linear parameters are expressed in (linearUnitName, linearUnitConvFactor).
 * Angular parameters are expressed in (angUnitName, angUnitConvFactor).
 */
PJ_OBJ *proj_obj_create_conversion_tunisia_mapping_grid(
    PJ_CONTEXT *ctx, double centerLat, double centerLong, double falseEasting,
    double falseNorthing, const char *angUnitName, double angUnitConvFactor,
    const char *linearUnitName, double linearUnitConvFactor) {
    try {
        UnitOfMeasure linearUnit(
            createLinearUnit(linearUnitName, linearUnitConvFactor));
        UnitOfMeasure angUnit(
            createAngularUnit(angUnitName, angUnitConvFactor));
        auto conv = Conversion::createTunisiaMappingGrid(
            PropertyMap(), Angle(centerLat, angUnit),
            Angle(centerLong, angUnit), Length(falseEasting, linearUnit),
            Length(falseNorthing, linearUnit));
        return proj_obj_create_conversion(ctx, conv);
    } catch (const std::exception &e) {
        proj_log_error(ctx, __FUNCTION__, e.what());
    }
    return nullptr;
}
// ---------------------------------------------------------------------------

/** \brief Instanciate a ProjectedCRS with a conversion based on the Albers
 * Conic Equal Area projection method.
 *
 * See osgeo::proj::operation::Conversion::createAlbersEqualArea().
 *
 * Linear parameters are expressed in (linearUnitName, linearUnitConvFactor).
 * Angular parameters are expressed in (angUnitName, angUnitConvFactor).
 */
PJ_OBJ *proj_obj_create_conversion_albers_equal_area(
    PJ_CONTEXT *ctx, double latitudeFalseOrigin, double longitudeFalseOrigin,
    double latitudeFirstParallel, double latitudeSecondParallel,
    double eastingFalseOrigin, double northingFalseOrigin,
    const char *angUnitName, double angUnitConvFactor,
    const char *linearUnitName, double linearUnitConvFactor) {
    try {
        UnitOfMeasure linearUnit(
            createLinearUnit(linearUnitName, linearUnitConvFactor));
        UnitOfMeasure angUnit(
            createAngularUnit(angUnitName, angUnitConvFactor));
        auto conv = Conversion::createAlbersEqualArea(
            PropertyMap(), Angle(latitudeFalseOrigin, angUnit),
            Angle(longitudeFalseOrigin, angUnit),
            Angle(latitudeFirstParallel, angUnit),
            Angle(latitudeSecondParallel, angUnit),
            Length(eastingFalseOrigin, linearUnit),
            Length(northingFalseOrigin, linearUnit));
        return proj_obj_create_conversion(ctx, conv);
    } catch (const std::exception &e) {
        proj_log_error(ctx, __FUNCTION__, e.what());
    }
    return nullptr;
}
// ---------------------------------------------------------------------------

/** \brief Instanciate a ProjectedCRS with a conversion based on the Lambert
 * Conic Conformal 1SP projection method.
 *
 * See osgeo::proj::operation::Conversion::createLambertConicConformal_1SP().
 *
 * Linear parameters are expressed in (linearUnitName, linearUnitConvFactor).
 * Angular parameters are expressed in (angUnitName, angUnitConvFactor).
 */
PJ_OBJ *proj_obj_create_conversion_lambert_conic_conformal_1sp(
    PJ_CONTEXT *ctx, double centerLat, double centerLong, double scale,
    double falseEasting, double falseNorthing, const char *angUnitName,
    double angUnitConvFactor, const char *linearUnitName,
    double linearUnitConvFactor) {
    try {
        UnitOfMeasure linearUnit(
            createLinearUnit(linearUnitName, linearUnitConvFactor));
        UnitOfMeasure angUnit(
            createAngularUnit(angUnitName, angUnitConvFactor));
        auto conv = Conversion::createLambertConicConformal_1SP(
            PropertyMap(), Angle(centerLat, angUnit),
            Angle(centerLong, angUnit), Scale(scale),
            Length(falseEasting, linearUnit),
            Length(falseNorthing, linearUnit));
        return proj_obj_create_conversion(ctx, conv);
    } catch (const std::exception &e) {
        proj_log_error(ctx, __FUNCTION__, e.what());
    }
    return nullptr;
}
// ---------------------------------------------------------------------------

/** \brief Instanciate a ProjectedCRS with a conversion based on the Lambert
 * Conic Conformal (2SP) projection method.
 *
 * See osgeo::proj::operation::Conversion::createLambertConicConformal_2SP().
 *
 * Linear parameters are expressed in (linearUnitName, linearUnitConvFactor).
 * Angular parameters are expressed in (angUnitName, angUnitConvFactor).
 */
PJ_OBJ *proj_obj_create_conversion_lambert_conic_conformal_2sp(
    PJ_CONTEXT *ctx, double latitudeFalseOrigin, double longitudeFalseOrigin,
    double latitudeFirstParallel, double latitudeSecondParallel,
    double eastingFalseOrigin, double northingFalseOrigin,
    const char *angUnitName, double angUnitConvFactor,
    const char *linearUnitName, double linearUnitConvFactor) {
    try {
        UnitOfMeasure linearUnit(
            createLinearUnit(linearUnitName, linearUnitConvFactor));
        UnitOfMeasure angUnit(
            createAngularUnit(angUnitName, angUnitConvFactor));
        auto conv = Conversion::createLambertConicConformal_2SP(
            PropertyMap(), Angle(latitudeFalseOrigin, angUnit),
            Angle(longitudeFalseOrigin, angUnit),
            Angle(latitudeFirstParallel, angUnit),
            Angle(latitudeSecondParallel, angUnit),
            Length(eastingFalseOrigin, linearUnit),
            Length(northingFalseOrigin, linearUnit));
        return proj_obj_create_conversion(ctx, conv);
    } catch (const std::exception &e) {
        proj_log_error(ctx, __FUNCTION__, e.what());
    }
    return nullptr;
}
// ---------------------------------------------------------------------------

/** \brief Instanciate a ProjectedCRS with a conversion based on the Lambert
 * Conic Conformal (2SP Michigan) projection method.
 *
 * See
 * osgeo::proj::operation::Conversion::createLambertConicConformal_2SP_Michigan().
 *
 * Linear parameters are expressed in (linearUnitName, linearUnitConvFactor).
 * Angular parameters are expressed in (angUnitName, angUnitConvFactor).
 */
PJ_OBJ *proj_obj_create_conversion_lambert_conic_conformal_2sp_michigan(
    PJ_CONTEXT *ctx, double latitudeFalseOrigin, double longitudeFalseOrigin,
    double latitudeFirstParallel, double latitudeSecondParallel,
    double eastingFalseOrigin, double northingFalseOrigin,
    double ellipsoidScalingFactor, const char *angUnitName,
    double angUnitConvFactor, const char *linearUnitName,
    double linearUnitConvFactor) {
    try {
        UnitOfMeasure linearUnit(
            createLinearUnit(linearUnitName, linearUnitConvFactor));
        UnitOfMeasure angUnit(
            createAngularUnit(angUnitName, angUnitConvFactor));
        auto conv = Conversion::createLambertConicConformal_2SP_Michigan(
            PropertyMap(), Angle(latitudeFalseOrigin, angUnit),
            Angle(longitudeFalseOrigin, angUnit),
            Angle(latitudeFirstParallel, angUnit),
            Angle(latitudeSecondParallel, angUnit),
            Length(eastingFalseOrigin, linearUnit),
            Length(northingFalseOrigin, linearUnit),
            Scale(ellipsoidScalingFactor));
        return proj_obj_create_conversion(ctx, conv);
    } catch (const std::exception &e) {
        proj_log_error(ctx, __FUNCTION__, e.what());
    }
    return nullptr;
}
// ---------------------------------------------------------------------------

/** \brief Instanciate a ProjectedCRS with a conversion based on the Lambert
 * Conic Conformal (2SP Belgium) projection method.
 *
 * See
 * osgeo::proj::operation::Conversion::createLambertConicConformal_2SP_Belgium().
 *
 * Linear parameters are expressed in (linearUnitName, linearUnitConvFactor).
 * Angular parameters are expressed in (angUnitName, angUnitConvFactor).
 */
PJ_OBJ *proj_obj_create_conversion_lambert_conic_conformal_2sp_belgium(
    PJ_CONTEXT *ctx, double latitudeFalseOrigin, double longitudeFalseOrigin,
    double latitudeFirstParallel, double latitudeSecondParallel,
    double eastingFalseOrigin, double northingFalseOrigin,
    const char *angUnitName, double angUnitConvFactor,
    const char *linearUnitName, double linearUnitConvFactor) {
    try {
        UnitOfMeasure linearUnit(
            createLinearUnit(linearUnitName, linearUnitConvFactor));
        UnitOfMeasure angUnit(
            createAngularUnit(angUnitName, angUnitConvFactor));
        auto conv = Conversion::createLambertConicConformal_2SP_Belgium(
            PropertyMap(), Angle(latitudeFalseOrigin, angUnit),
            Angle(longitudeFalseOrigin, angUnit),
            Angle(latitudeFirstParallel, angUnit),
            Angle(latitudeSecondParallel, angUnit),
            Length(eastingFalseOrigin, linearUnit),
            Length(northingFalseOrigin, linearUnit));
        return proj_obj_create_conversion(ctx, conv);
    } catch (const std::exception &e) {
        proj_log_error(ctx, __FUNCTION__, e.what());
    }
    return nullptr;
}
// ---------------------------------------------------------------------------

/** \brief Instanciate a ProjectedCRS with a conversion based on the Modified
 * Azimuthal Equidistant projection method.
 *
 * See osgeo::proj::operation::Conversion::createAzimuthalEquidistant().
 *
 * Linear parameters are expressed in (linearUnitName, linearUnitConvFactor).
 * Angular parameters are expressed in (angUnitName, angUnitConvFactor).
 */
PJ_OBJ *proj_obj_create_conversion_azimuthal_equidistant(
    PJ_CONTEXT *ctx, double latitudeNatOrigin, double longitudeNatOrigin,
    double falseEasting, double falseNorthing, const char *angUnitName,
    double angUnitConvFactor, const char *linearUnitName,
    double linearUnitConvFactor) {
    try {
        UnitOfMeasure linearUnit(
            createLinearUnit(linearUnitName, linearUnitConvFactor));
        UnitOfMeasure angUnit(
            createAngularUnit(angUnitName, angUnitConvFactor));
        auto conv = Conversion::createAzimuthalEquidistant(
            PropertyMap(), Angle(latitudeNatOrigin, angUnit),
            Angle(longitudeNatOrigin, angUnit),
            Length(falseEasting, linearUnit),
            Length(falseNorthing, linearUnit));
        return proj_obj_create_conversion(ctx, conv);
    } catch (const std::exception &e) {
        proj_log_error(ctx, __FUNCTION__, e.what());
    }
    return nullptr;
}
// ---------------------------------------------------------------------------

/** \brief Instanciate a ProjectedCRS with a conversion based on the Guam
 * Projection projection method.
 *
 * See osgeo::proj::operation::Conversion::createGuamProjection().
 *
 * Linear parameters are expressed in (linearUnitName, linearUnitConvFactor).
 * Angular parameters are expressed in (angUnitName, angUnitConvFactor).
 */
PJ_OBJ *proj_obj_create_conversion_guam_projection(
    PJ_CONTEXT *ctx, double latitudeNatOrigin, double longitudeNatOrigin,
    double falseEasting, double falseNorthing, const char *angUnitName,
    double angUnitConvFactor, const char *linearUnitName,
    double linearUnitConvFactor) {
    try {
        UnitOfMeasure linearUnit(
            createLinearUnit(linearUnitName, linearUnitConvFactor));
        UnitOfMeasure angUnit(
            createAngularUnit(angUnitName, angUnitConvFactor));
        auto conv = Conversion::createGuamProjection(
            PropertyMap(), Angle(latitudeNatOrigin, angUnit),
            Angle(longitudeNatOrigin, angUnit),
            Length(falseEasting, linearUnit),
            Length(falseNorthing, linearUnit));
        return proj_obj_create_conversion(ctx, conv);
    } catch (const std::exception &e) {
        proj_log_error(ctx, __FUNCTION__, e.what());
    }
    return nullptr;
}
// ---------------------------------------------------------------------------

/** \brief Instanciate a ProjectedCRS with a conversion based on the Bonne
 * projection method.
 *
 * See osgeo::proj::operation::Conversion::createBonne().
 *
 * Linear parameters are expressed in (linearUnitName, linearUnitConvFactor).
 * Angular parameters are expressed in (angUnitName, angUnitConvFactor).
 */
PJ_OBJ *proj_obj_create_conversion_bonne(
    PJ_CONTEXT *ctx, double latitudeNatOrigin, double longitudeNatOrigin,
    double falseEasting, double falseNorthing, const char *angUnitName,
    double angUnitConvFactor, const char *linearUnitName,
    double linearUnitConvFactor) {
    try {
        UnitOfMeasure linearUnit(
            createLinearUnit(linearUnitName, linearUnitConvFactor));
        UnitOfMeasure angUnit(
            createAngularUnit(angUnitName, angUnitConvFactor));
        auto conv = Conversion::createBonne(PropertyMap(),
                                            Angle(latitudeNatOrigin, angUnit),
                                            Angle(longitudeNatOrigin, angUnit),
                                            Length(falseEasting, linearUnit),
                                            Length(falseNorthing, linearUnit));
        return proj_obj_create_conversion(ctx, conv);
    } catch (const std::exception &e) {
        proj_log_error(ctx, __FUNCTION__, e.what());
    }
    return nullptr;
}
// ---------------------------------------------------------------------------

/** \brief Instanciate a ProjectedCRS with a conversion based on the Lambert
 * Cylindrical Equal Area (Spherical) projection method.
 *
 * See
 * osgeo::proj::operation::Conversion::createLambertCylindricalEqualAreaSpherical().
 *
 * Linear parameters are expressed in (linearUnitName, linearUnitConvFactor).
 * Angular parameters are expressed in (angUnitName, angUnitConvFactor).
 */
PJ_OBJ *proj_obj_create_conversion_lambert_cylindrical_equal_area_spherical(
    PJ_CONTEXT *ctx, double latitudeFirstParallel, double longitudeNatOrigin,
    double falseEasting, double falseNorthing, const char *angUnitName,
    double angUnitConvFactor, const char *linearUnitName,
    double linearUnitConvFactor) {
    try {
        UnitOfMeasure linearUnit(
            createLinearUnit(linearUnitName, linearUnitConvFactor));
        UnitOfMeasure angUnit(
            createAngularUnit(angUnitName, angUnitConvFactor));
        auto conv = Conversion::createLambertCylindricalEqualAreaSpherical(
            PropertyMap(), Angle(latitudeFirstParallel, angUnit),
            Angle(longitudeNatOrigin, angUnit),
            Length(falseEasting, linearUnit),
            Length(falseNorthing, linearUnit));
        return proj_obj_create_conversion(ctx, conv);
    } catch (const std::exception &e) {
        proj_log_error(ctx, __FUNCTION__, e.what());
    }
    return nullptr;
}
// ---------------------------------------------------------------------------

/** \brief Instanciate a ProjectedCRS with a conversion based on the Lambert
 * Cylindrical Equal Area (ellipsoidal form) projection method.
 *
 * See osgeo::proj::operation::Conversion::createLambertCylindricalEqualArea().
 *
 * Linear parameters are expressed in (linearUnitName, linearUnitConvFactor).
 * Angular parameters are expressed in (angUnitName, angUnitConvFactor).
 */
PJ_OBJ *proj_obj_create_conversion_lambert_cylindrical_equal_area(
    PJ_CONTEXT *ctx, double latitudeFirstParallel, double longitudeNatOrigin,
    double falseEasting, double falseNorthing, const char *angUnitName,
    double angUnitConvFactor, const char *linearUnitName,
    double linearUnitConvFactor) {
    try {
        UnitOfMeasure linearUnit(
            createLinearUnit(linearUnitName, linearUnitConvFactor));
        UnitOfMeasure angUnit(
            createAngularUnit(angUnitName, angUnitConvFactor));
        auto conv = Conversion::createLambertCylindricalEqualArea(
            PropertyMap(), Angle(latitudeFirstParallel, angUnit),
            Angle(longitudeNatOrigin, angUnit),
            Length(falseEasting, linearUnit),
            Length(falseNorthing, linearUnit));
        return proj_obj_create_conversion(ctx, conv);
    } catch (const std::exception &e) {
        proj_log_error(ctx, __FUNCTION__, e.what());
    }
    return nullptr;
}
// ---------------------------------------------------------------------------

/** \brief Instanciate a ProjectedCRS with a conversion based on the
 * Cassini-Soldner projection method.
 *
 * See osgeo::proj::operation::Conversion::createCassiniSoldner().
 *
 * Linear parameters are expressed in (linearUnitName, linearUnitConvFactor).
 * Angular parameters are expressed in (angUnitName, angUnitConvFactor).
 */
PJ_OBJ *proj_obj_create_conversion_cassini_soldner(
    PJ_CONTEXT *ctx, double centerLat, double centerLong, double falseEasting,
    double falseNorthing, const char *angUnitName, double angUnitConvFactor,
    const char *linearUnitName, double linearUnitConvFactor) {
    try {
        UnitOfMeasure linearUnit(
            createLinearUnit(linearUnitName, linearUnitConvFactor));
        UnitOfMeasure angUnit(
            createAngularUnit(angUnitName, angUnitConvFactor));
        auto conv = Conversion::createCassiniSoldner(
            PropertyMap(), Angle(centerLat, angUnit),
            Angle(centerLong, angUnit), Length(falseEasting, linearUnit),
            Length(falseNorthing, linearUnit));
        return proj_obj_create_conversion(ctx, conv);
    } catch (const std::exception &e) {
        proj_log_error(ctx, __FUNCTION__, e.what());
    }
    return nullptr;
}
// ---------------------------------------------------------------------------

/** \brief Instanciate a ProjectedCRS with a conversion based on the Equidistant
 * Conic projection method.
 *
 * See osgeo::proj::operation::Conversion::createEquidistantConic().
 *
 * Linear parameters are expressed in (linearUnitName, linearUnitConvFactor).
 * Angular parameters are expressed in (angUnitName, angUnitConvFactor).
 */
PJ_OBJ *proj_obj_create_conversion_equidistant_conic(
    PJ_CONTEXT *ctx, double centerLat, double centerLong,
    double latitudeFirstParallel, double latitudeSecondParallel,
    double falseEasting, double falseNorthing, const char *angUnitName,
    double angUnitConvFactor, const char *linearUnitName,
    double linearUnitConvFactor) {
    try {
        UnitOfMeasure linearUnit(
            createLinearUnit(linearUnitName, linearUnitConvFactor));
        UnitOfMeasure angUnit(
            createAngularUnit(angUnitName, angUnitConvFactor));
        auto conv = Conversion::createEquidistantConic(
            PropertyMap(), Angle(centerLat, angUnit),
            Angle(centerLong, angUnit), Angle(latitudeFirstParallel, angUnit),
            Angle(latitudeSecondParallel, angUnit),
            Length(falseEasting, linearUnit),
            Length(falseNorthing, linearUnit));
        return proj_obj_create_conversion(ctx, conv);
    } catch (const std::exception &e) {
        proj_log_error(ctx, __FUNCTION__, e.what());
    }
    return nullptr;
}
// ---------------------------------------------------------------------------

/** \brief Instanciate a ProjectedCRS with a conversion based on the Eckert I
 * projection method.
 *
 * See osgeo::proj::operation::Conversion::createEckertI().
 *
 * Linear parameters are expressed in (linearUnitName, linearUnitConvFactor).
 * Angular parameters are expressed in (angUnitName, angUnitConvFactor).
 */
PJ_OBJ *proj_obj_create_conversion_eckert_i(
    PJ_CONTEXT *ctx, double centerLong, double falseEasting,
    double falseNorthing, const char *angUnitName, double angUnitConvFactor,
    const char *linearUnitName, double linearUnitConvFactor) {
    try {
        UnitOfMeasure linearUnit(
            createLinearUnit(linearUnitName, linearUnitConvFactor));
        UnitOfMeasure angUnit(
            createAngularUnit(angUnitName, angUnitConvFactor));
        auto conv =
            Conversion::createEckertI(PropertyMap(), Angle(centerLong, angUnit),
                                      Length(falseEasting, linearUnit),
                                      Length(falseNorthing, linearUnit));
        return proj_obj_create_conversion(ctx, conv);
    } catch (const std::exception &e) {
        proj_log_error(ctx, __FUNCTION__, e.what());
    }
    return nullptr;
}
// ---------------------------------------------------------------------------

/** \brief Instanciate a ProjectedCRS with a conversion based on the Eckert II
 * projection method.
 *
 * See osgeo::proj::operation::Conversion::createEckertII().
 *
 * Linear parameters are expressed in (linearUnitName, linearUnitConvFactor).
 * Angular parameters are expressed in (angUnitName, angUnitConvFactor).
 */
PJ_OBJ *proj_obj_create_conversion_eckert_ii(
    PJ_CONTEXT *ctx, double centerLong, double falseEasting,
    double falseNorthing, const char *angUnitName, double angUnitConvFactor,
    const char *linearUnitName, double linearUnitConvFactor) {
    try {
        UnitOfMeasure linearUnit(
            createLinearUnit(linearUnitName, linearUnitConvFactor));
        UnitOfMeasure angUnit(
            createAngularUnit(angUnitName, angUnitConvFactor));
        auto conv = Conversion::createEckertII(
            PropertyMap(), Angle(centerLong, angUnit),
            Length(falseEasting, linearUnit),
            Length(falseNorthing, linearUnit));
        return proj_obj_create_conversion(ctx, conv);
    } catch (const std::exception &e) {
        proj_log_error(ctx, __FUNCTION__, e.what());
    }
    return nullptr;
}
// ---------------------------------------------------------------------------

/** \brief Instanciate a ProjectedCRS with a conversion based on the Eckert III
 * projection method.
 *
 * See osgeo::proj::operation::Conversion::createEckertIII().
 *
 * Linear parameters are expressed in (linearUnitName, linearUnitConvFactor).
 * Angular parameters are expressed in (angUnitName, angUnitConvFactor).
 */
PJ_OBJ *proj_obj_create_conversion_eckert_iii(
    PJ_CONTEXT *ctx, double centerLong, double falseEasting,
    double falseNorthing, const char *angUnitName, double angUnitConvFactor,
    const char *linearUnitName, double linearUnitConvFactor) {
    try {
        UnitOfMeasure linearUnit(
            createLinearUnit(linearUnitName, linearUnitConvFactor));
        UnitOfMeasure angUnit(
            createAngularUnit(angUnitName, angUnitConvFactor));
        auto conv = Conversion::createEckertIII(
            PropertyMap(), Angle(centerLong, angUnit),
            Length(falseEasting, linearUnit),
            Length(falseNorthing, linearUnit));
        return proj_obj_create_conversion(ctx, conv);
    } catch (const std::exception &e) {
        proj_log_error(ctx, __FUNCTION__, e.what());
    }
    return nullptr;
}
// ---------------------------------------------------------------------------

/** \brief Instanciate a ProjectedCRS with a conversion based on the Eckert IV
 * projection method.
 *
 * See osgeo::proj::operation::Conversion::createEckertIV().
 *
 * Linear parameters are expressed in (linearUnitName, linearUnitConvFactor).
 * Angular parameters are expressed in (angUnitName, angUnitConvFactor).
 */
PJ_OBJ *proj_obj_create_conversion_eckert_iv(
    PJ_CONTEXT *ctx, double centerLong, double falseEasting,
    double falseNorthing, const char *angUnitName, double angUnitConvFactor,
    const char *linearUnitName, double linearUnitConvFactor) {
    try {
        UnitOfMeasure linearUnit(
            createLinearUnit(linearUnitName, linearUnitConvFactor));
        UnitOfMeasure angUnit(
            createAngularUnit(angUnitName, angUnitConvFactor));
        auto conv = Conversion::createEckertIV(
            PropertyMap(), Angle(centerLong, angUnit),
            Length(falseEasting, linearUnit),
            Length(falseNorthing, linearUnit));
        return proj_obj_create_conversion(ctx, conv);
    } catch (const std::exception &e) {
        proj_log_error(ctx, __FUNCTION__, e.what());
    }
    return nullptr;
}
// ---------------------------------------------------------------------------

/** \brief Instanciate a ProjectedCRS with a conversion based on the Eckert V
 * projection method.
 *
 * See osgeo::proj::operation::Conversion::createEckertV().
 *
 * Linear parameters are expressed in (linearUnitName, linearUnitConvFactor).
 * Angular parameters are expressed in (angUnitName, angUnitConvFactor).
 */
PJ_OBJ *proj_obj_create_conversion_eckert_v(
    PJ_CONTEXT *ctx, double centerLong, double falseEasting,
    double falseNorthing, const char *angUnitName, double angUnitConvFactor,
    const char *linearUnitName, double linearUnitConvFactor) {
    try {
        UnitOfMeasure linearUnit(
            createLinearUnit(linearUnitName, linearUnitConvFactor));
        UnitOfMeasure angUnit(
            createAngularUnit(angUnitName, angUnitConvFactor));
        auto conv =
            Conversion::createEckertV(PropertyMap(), Angle(centerLong, angUnit),
                                      Length(falseEasting, linearUnit),
                                      Length(falseNorthing, linearUnit));
        return proj_obj_create_conversion(ctx, conv);
    } catch (const std::exception &e) {
        proj_log_error(ctx, __FUNCTION__, e.what());
    }
    return nullptr;
}
// ---------------------------------------------------------------------------

/** \brief Instanciate a ProjectedCRS with a conversion based on the Eckert VI
 * projection method.
 *
 * See osgeo::proj::operation::Conversion::createEckertVI().
 *
 * Linear parameters are expressed in (linearUnitName, linearUnitConvFactor).
 * Angular parameters are expressed in (angUnitName, angUnitConvFactor).
 */
PJ_OBJ *proj_obj_create_conversion_eckert_vi(
    PJ_CONTEXT *ctx, double centerLong, double falseEasting,
    double falseNorthing, const char *angUnitName, double angUnitConvFactor,
    const char *linearUnitName, double linearUnitConvFactor) {
    try {
        UnitOfMeasure linearUnit(
            createLinearUnit(linearUnitName, linearUnitConvFactor));
        UnitOfMeasure angUnit(
            createAngularUnit(angUnitName, angUnitConvFactor));
        auto conv = Conversion::createEckertVI(
            PropertyMap(), Angle(centerLong, angUnit),
            Length(falseEasting, linearUnit),
            Length(falseNorthing, linearUnit));
        return proj_obj_create_conversion(ctx, conv);
    } catch (const std::exception &e) {
        proj_log_error(ctx, __FUNCTION__, e.what());
    }
    return nullptr;
}
// ---------------------------------------------------------------------------

/** \brief Instanciate a ProjectedCRS with a conversion based on the Equidistant
 * Cylindrical projection method.
 *
 * See osgeo::proj::operation::Conversion::createEquidistantCylindrical().
 *
 * Linear parameters are expressed in (linearUnitName, linearUnitConvFactor).
 * Angular parameters are expressed in (angUnitName, angUnitConvFactor).
 */
PJ_OBJ *proj_obj_create_conversion_equidistant_cylindrical(
    PJ_CONTEXT *ctx, double latitudeFirstParallel, double longitudeNatOrigin,
    double falseEasting, double falseNorthing, const char *angUnitName,
    double angUnitConvFactor, const char *linearUnitName,
    double linearUnitConvFactor) {
    try {
        UnitOfMeasure linearUnit(
            createLinearUnit(linearUnitName, linearUnitConvFactor));
        UnitOfMeasure angUnit(
            createAngularUnit(angUnitName, angUnitConvFactor));
        auto conv = Conversion::createEquidistantCylindrical(
            PropertyMap(), Angle(latitudeFirstParallel, angUnit),
            Angle(longitudeNatOrigin, angUnit),
            Length(falseEasting, linearUnit),
            Length(falseNorthing, linearUnit));
        return proj_obj_create_conversion(ctx, conv);
    } catch (const std::exception &e) {
        proj_log_error(ctx, __FUNCTION__, e.what());
    }
    return nullptr;
}
// ---------------------------------------------------------------------------

/** \brief Instanciate a ProjectedCRS with a conversion based on the Equidistant
 * Cylindrical (Spherical) projection method.
 *
 * See
 * osgeo::proj::operation::Conversion::createEquidistantCylindricalSpherical().
 *
 * Linear parameters are expressed in (linearUnitName, linearUnitConvFactor).
 * Angular parameters are expressed in (angUnitName, angUnitConvFactor).
 */
PJ_OBJ *proj_obj_create_conversion_equidistant_cylindrical_spherical(
    PJ_CONTEXT *ctx, double latitudeFirstParallel, double longitudeNatOrigin,
    double falseEasting, double falseNorthing, const char *angUnitName,
    double angUnitConvFactor, const char *linearUnitName,
    double linearUnitConvFactor) {
    try {
        UnitOfMeasure linearUnit(
            createLinearUnit(linearUnitName, linearUnitConvFactor));
        UnitOfMeasure angUnit(
            createAngularUnit(angUnitName, angUnitConvFactor));
        auto conv = Conversion::createEquidistantCylindricalSpherical(
            PropertyMap(), Angle(latitudeFirstParallel, angUnit),
            Angle(longitudeNatOrigin, angUnit),
            Length(falseEasting, linearUnit),
            Length(falseNorthing, linearUnit));
        return proj_obj_create_conversion(ctx, conv);
    } catch (const std::exception &e) {
        proj_log_error(ctx, __FUNCTION__, e.what());
    }
    return nullptr;
}
// ---------------------------------------------------------------------------

/** \brief Instanciate a ProjectedCRS with a conversion based on the Gall
 * (Stereographic) projection method.
 *
 * See osgeo::proj::operation::Conversion::createGall().
 *
 * Linear parameters are expressed in (linearUnitName, linearUnitConvFactor).
 * Angular parameters are expressed in (angUnitName, angUnitConvFactor).
 */
PJ_OBJ *proj_obj_create_conversion_gall(
    PJ_CONTEXT *ctx, double centerLong, double falseEasting,
    double falseNorthing, const char *angUnitName, double angUnitConvFactor,
    const char *linearUnitName, double linearUnitConvFactor) {
    try {
        UnitOfMeasure linearUnit(
            createLinearUnit(linearUnitName, linearUnitConvFactor));
        UnitOfMeasure angUnit(
            createAngularUnit(angUnitName, angUnitConvFactor));
        auto conv =
            Conversion::createGall(PropertyMap(), Angle(centerLong, angUnit),
                                   Length(falseEasting, linearUnit),
                                   Length(falseNorthing, linearUnit));
        return proj_obj_create_conversion(ctx, conv);
    } catch (const std::exception &e) {
        proj_log_error(ctx, __FUNCTION__, e.what());
    }
    return nullptr;
}
// ---------------------------------------------------------------------------

/** \brief Instanciate a ProjectedCRS with a conversion based on the Goode
 * Homolosine projection method.
 *
 * See osgeo::proj::operation::Conversion::createGoodeHomolosine().
 *
 * Linear parameters are expressed in (linearUnitName, linearUnitConvFactor).
 * Angular parameters are expressed in (angUnitName, angUnitConvFactor).
 */
PJ_OBJ *proj_obj_create_conversion_goode_homolosine(
    PJ_CONTEXT *ctx, double centerLong, double falseEasting,
    double falseNorthing, const char *angUnitName, double angUnitConvFactor,
    const char *linearUnitName, double linearUnitConvFactor) {
    try {
        UnitOfMeasure linearUnit(
            createLinearUnit(linearUnitName, linearUnitConvFactor));
        UnitOfMeasure angUnit(
            createAngularUnit(angUnitName, angUnitConvFactor));
        auto conv = Conversion::createGoodeHomolosine(
            PropertyMap(), Angle(centerLong, angUnit),
            Length(falseEasting, linearUnit),
            Length(falseNorthing, linearUnit));
        return proj_obj_create_conversion(ctx, conv);
    } catch (const std::exception &e) {
        proj_log_error(ctx, __FUNCTION__, e.what());
    }
    return nullptr;
}
// ---------------------------------------------------------------------------

/** \brief Instanciate a ProjectedCRS with a conversion based on the Interrupted
 * Goode Homolosine projection method.
 *
 * See osgeo::proj::operation::Conversion::createInterruptedGoodeHomolosine().
 *
 * Linear parameters are expressed in (linearUnitName, linearUnitConvFactor).
 * Angular parameters are expressed in (angUnitName, angUnitConvFactor).
 */
PJ_OBJ *proj_obj_create_conversion_interrupted_goode_homolosine(
    PJ_CONTEXT *ctx, double centerLong, double falseEasting,
    double falseNorthing, const char *angUnitName, double angUnitConvFactor,
    const char *linearUnitName, double linearUnitConvFactor) {
    try {
        UnitOfMeasure linearUnit(
            createLinearUnit(linearUnitName, linearUnitConvFactor));
        UnitOfMeasure angUnit(
            createAngularUnit(angUnitName, angUnitConvFactor));
        auto conv = Conversion::createInterruptedGoodeHomolosine(
            PropertyMap(), Angle(centerLong, angUnit),
            Length(falseEasting, linearUnit),
            Length(falseNorthing, linearUnit));
        return proj_obj_create_conversion(ctx, conv);
    } catch (const std::exception &e) {
        proj_log_error(ctx, __FUNCTION__, e.what());
    }
    return nullptr;
}
// ---------------------------------------------------------------------------

/** \brief Instanciate a ProjectedCRS with a conversion based on the
 * Geostationary Satellite View projection method, with the sweep angle axis of
 * the viewing instrument being x.
 *
 * See osgeo::proj::operation::Conversion::createGeostationarySatelliteSweepX().
 *
 * Linear parameters are expressed in (linearUnitName, linearUnitConvFactor).
 * Angular parameters are expressed in (angUnitName, angUnitConvFactor).
 */
PJ_OBJ *proj_obj_create_conversion_geostationary_satellite_sweep_x(
    PJ_CONTEXT *ctx, double centerLong, double height, double falseEasting,
    double falseNorthing, const char *angUnitName, double angUnitConvFactor,
    const char *linearUnitName, double linearUnitConvFactor) {
    try {
        UnitOfMeasure linearUnit(
            createLinearUnit(linearUnitName, linearUnitConvFactor));
        UnitOfMeasure angUnit(
            createAngularUnit(angUnitName, angUnitConvFactor));
        auto conv = Conversion::createGeostationarySatelliteSweepX(
            PropertyMap(), Angle(centerLong, angUnit),
            Length(height, linearUnit), Length(falseEasting, linearUnit),
            Length(falseNorthing, linearUnit));
        return proj_obj_create_conversion(ctx, conv);
    } catch (const std::exception &e) {
        proj_log_error(ctx, __FUNCTION__, e.what());
    }
    return nullptr;
}
// ---------------------------------------------------------------------------

/** \brief Instanciate a ProjectedCRS with a conversion based on the
 * Geostationary Satellite View projection method, with the sweep angle axis of
 * the viewing instrument being y.
 *
 * See osgeo::proj::operation::Conversion::createGeostationarySatelliteSweepY().
 *
 * Linear parameters are expressed in (linearUnitName, linearUnitConvFactor).
 * Angular parameters are expressed in (angUnitName, angUnitConvFactor).
 */
PJ_OBJ *proj_obj_create_conversion_geostationary_satellite_sweep_y(
    PJ_CONTEXT *ctx, double centerLong, double height, double falseEasting,
    double falseNorthing, const char *angUnitName, double angUnitConvFactor,
    const char *linearUnitName, double linearUnitConvFactor) {
    try {
        UnitOfMeasure linearUnit(
            createLinearUnit(linearUnitName, linearUnitConvFactor));
        UnitOfMeasure angUnit(
            createAngularUnit(angUnitName, angUnitConvFactor));
        auto conv = Conversion::createGeostationarySatelliteSweepY(
            PropertyMap(), Angle(centerLong, angUnit),
            Length(height, linearUnit), Length(falseEasting, linearUnit),
            Length(falseNorthing, linearUnit));
        return proj_obj_create_conversion(ctx, conv);
    } catch (const std::exception &e) {
        proj_log_error(ctx, __FUNCTION__, e.what());
    }
    return nullptr;
}
// ---------------------------------------------------------------------------

/** \brief Instanciate a ProjectedCRS with a conversion based on the Gnomonic
 * projection method.
 *
 * See osgeo::proj::operation::Conversion::createGnomonic().
 *
 * Linear parameters are expressed in (linearUnitName, linearUnitConvFactor).
 * Angular parameters are expressed in (angUnitName, angUnitConvFactor).
 */
PJ_OBJ *proj_obj_create_conversion_gnomonic(
    PJ_CONTEXT *ctx, double centerLat, double centerLong, double falseEasting,
    double falseNorthing, const char *angUnitName, double angUnitConvFactor,
    const char *linearUnitName, double linearUnitConvFactor) {
    try {
        UnitOfMeasure linearUnit(
            createLinearUnit(linearUnitName, linearUnitConvFactor));
        UnitOfMeasure angUnit(
            createAngularUnit(angUnitName, angUnitConvFactor));
        auto conv = Conversion::createGnomonic(
            PropertyMap(), Angle(centerLat, angUnit),
            Angle(centerLong, angUnit), Length(falseEasting, linearUnit),
            Length(falseNorthing, linearUnit));
        return proj_obj_create_conversion(ctx, conv);
    } catch (const std::exception &e) {
        proj_log_error(ctx, __FUNCTION__, e.what());
    }
    return nullptr;
}
// ---------------------------------------------------------------------------

/** \brief Instanciate a ProjectedCRS with a conversion based on the Hotine
 * Oblique Mercator (Variant A) projection method.
 *
 * See
 * osgeo::proj::operation::Conversion::createHotineObliqueMercatorVariantA().
 *
 * Linear parameters are expressed in (linearUnitName, linearUnitConvFactor).
 * Angular parameters are expressed in (angUnitName, angUnitConvFactor).
 */
PJ_OBJ *proj_obj_create_conversion_hotine_oblique_mercator_variant_a(
    PJ_CONTEXT *ctx, double latitudeProjectionCentre,
    double longitudeProjectionCentre, double azimuthInitialLine,
    double angleFromRectifiedToSkrewGrid, double scale, double falseEasting,
    double falseNorthing, const char *angUnitName, double angUnitConvFactor,
    const char *linearUnitName, double linearUnitConvFactor) {
    try {
        UnitOfMeasure linearUnit(
            createLinearUnit(linearUnitName, linearUnitConvFactor));
        UnitOfMeasure angUnit(
            createAngularUnit(angUnitName, angUnitConvFactor));
        auto conv = Conversion::createHotineObliqueMercatorVariantA(
            PropertyMap(), Angle(latitudeProjectionCentre, angUnit),
            Angle(longitudeProjectionCentre, angUnit),
            Angle(azimuthInitialLine, angUnit),
            Angle(angleFromRectifiedToSkrewGrid, angUnit), Scale(scale),
            Length(falseEasting, linearUnit),
            Length(falseNorthing, linearUnit));
        return proj_obj_create_conversion(ctx, conv);
    } catch (const std::exception &e) {
        proj_log_error(ctx, __FUNCTION__, e.what());
    }
    return nullptr;
}
// ---------------------------------------------------------------------------

/** \brief Instanciate a ProjectedCRS with a conversion based on the Hotine
 * Oblique Mercator (Variant B) projection method.
 *
 * See
 * osgeo::proj::operation::Conversion::createHotineObliqueMercatorVariantB().
 *
 * Linear parameters are expressed in (linearUnitName, linearUnitConvFactor).
 * Angular parameters are expressed in (angUnitName, angUnitConvFactor).
 */
PJ_OBJ *proj_obj_create_conversion_hotine_oblique_mercator_variant_b(
    PJ_CONTEXT *ctx, double latitudeProjectionCentre,
    double longitudeProjectionCentre, double azimuthInitialLine,
    double angleFromRectifiedToSkrewGrid, double scale,
    double eastingProjectionCentre, double northingProjectionCentre,
    const char *angUnitName, double angUnitConvFactor,
    const char *linearUnitName, double linearUnitConvFactor) {
    try {
        UnitOfMeasure linearUnit(
            createLinearUnit(linearUnitName, linearUnitConvFactor));
        UnitOfMeasure angUnit(
            createAngularUnit(angUnitName, angUnitConvFactor));
        auto conv = Conversion::createHotineObliqueMercatorVariantB(
            PropertyMap(), Angle(latitudeProjectionCentre, angUnit),
            Angle(longitudeProjectionCentre, angUnit),
            Angle(azimuthInitialLine, angUnit),
            Angle(angleFromRectifiedToSkrewGrid, angUnit), Scale(scale),
            Length(eastingProjectionCentre, linearUnit),
            Length(northingProjectionCentre, linearUnit));
        return proj_obj_create_conversion(ctx, conv);
    } catch (const std::exception &e) {
        proj_log_error(ctx, __FUNCTION__, e.what());
    }
    return nullptr;
}
// ---------------------------------------------------------------------------

/** \brief Instanciate a ProjectedCRS with a conversion based on the Hotine
 * Oblique Mercator Two Point Natural Origin projection method.
 *
 * See
 * osgeo::proj::operation::Conversion::createHotineObliqueMercatorTwoPointNaturalOrigin().
 *
 * Linear parameters are expressed in (linearUnitName, linearUnitConvFactor).
 * Angular parameters are expressed in (angUnitName, angUnitConvFactor).
 */
PJ_OBJ *
proj_obj_create_conversion_hotine_oblique_mercator_two_point_natural_origin(
    PJ_CONTEXT *ctx, double latitudeProjectionCentre, double latitudePoint1,
    double longitudePoint1, double latitudePoint2, double longitudePoint2,
    double scale, double eastingProjectionCentre,
    double northingProjectionCentre, const char *angUnitName,
    double angUnitConvFactor, const char *linearUnitName,
    double linearUnitConvFactor) {
    try {
        UnitOfMeasure linearUnit(
            createLinearUnit(linearUnitName, linearUnitConvFactor));
        UnitOfMeasure angUnit(
            createAngularUnit(angUnitName, angUnitConvFactor));
        auto conv =
            Conversion::createHotineObliqueMercatorTwoPointNaturalOrigin(
                PropertyMap(), Angle(latitudeProjectionCentre, angUnit),
                Angle(latitudePoint1, angUnit), Angle(longitudePoint1, angUnit),
                Angle(latitudePoint2, angUnit), Angle(longitudePoint2, angUnit),
                Scale(scale), Length(eastingProjectionCentre, linearUnit),
                Length(northingProjectionCentre, linearUnit));
        return proj_obj_create_conversion(ctx, conv);
    } catch (const std::exception &e) {
        proj_log_error(ctx, __FUNCTION__, e.what());
    }
    return nullptr;
}
// ---------------------------------------------------------------------------

/** \brief Instanciate a ProjectedCRS with a conversion based on the
 * International Map of the World Polyconic projection method.
 *
 * See
 * osgeo::proj::operation::Conversion::createInternationalMapWorldPolyconic().
 *
 * Linear parameters are expressed in (linearUnitName, linearUnitConvFactor).
 * Angular parameters are expressed in (angUnitName, angUnitConvFactor).
 */
PJ_OBJ *proj_obj_create_conversion_international_map_world_polyconic(
    PJ_CONTEXT *ctx, double centerLong, double latitudeFirstParallel,
    double latitudeSecondParallel, double falseEasting, double falseNorthing,
    const char *angUnitName, double angUnitConvFactor,
    const char *linearUnitName, double linearUnitConvFactor) {
    try {
        UnitOfMeasure linearUnit(
            createLinearUnit(linearUnitName, linearUnitConvFactor));
        UnitOfMeasure angUnit(
            createAngularUnit(angUnitName, angUnitConvFactor));
        auto conv = Conversion::createInternationalMapWorldPolyconic(
            PropertyMap(), Angle(centerLong, angUnit),
            Angle(latitudeFirstParallel, angUnit),
            Angle(latitudeSecondParallel, angUnit),
            Length(falseEasting, linearUnit),
            Length(falseNorthing, linearUnit));
        return proj_obj_create_conversion(ctx, conv);
    } catch (const std::exception &e) {
        proj_log_error(ctx, __FUNCTION__, e.what());
    }
    return nullptr;
}
// ---------------------------------------------------------------------------

/** \brief Instanciate a ProjectedCRS with a conversion based on the Krovak
 * (north oriented) projection method.
 *
 * See osgeo::proj::operation::Conversion::createKrovakNorthOriented().
 *
 * Linear parameters are expressed in (linearUnitName, linearUnitConvFactor).
 * Angular parameters are expressed in (angUnitName, angUnitConvFactor).
 */
PJ_OBJ *proj_obj_create_conversion_krovak_north_oriented(
    PJ_CONTEXT *ctx, double latitudeProjectionCentre, double longitudeOfOrigin,
    double colatitudeConeAxis, double latitudePseudoStandardParallel,
    double scaleFactorPseudoStandardParallel, double falseEasting,
    double falseNorthing, const char *angUnitName, double angUnitConvFactor,
    const char *linearUnitName, double linearUnitConvFactor) {
    try {
        UnitOfMeasure linearUnit(
            createLinearUnit(linearUnitName, linearUnitConvFactor));
        UnitOfMeasure angUnit(
            createAngularUnit(angUnitName, angUnitConvFactor));
        auto conv = Conversion::createKrovakNorthOriented(
            PropertyMap(), Angle(latitudeProjectionCentre, angUnit),
            Angle(longitudeOfOrigin, angUnit),
            Angle(colatitudeConeAxis, angUnit),
            Angle(latitudePseudoStandardParallel, angUnit),
            Scale(scaleFactorPseudoStandardParallel),
            Length(falseEasting, linearUnit),
            Length(falseNorthing, linearUnit));
        return proj_obj_create_conversion(ctx, conv);
    } catch (const std::exception &e) {
        proj_log_error(ctx, __FUNCTION__, e.what());
    }
    return nullptr;
}
// ---------------------------------------------------------------------------

/** \brief Instanciate a ProjectedCRS with a conversion based on the Krovak
 * projection method.
 *
 * See osgeo::proj::operation::Conversion::createKrovak().
 *
 * Linear parameters are expressed in (linearUnitName, linearUnitConvFactor).
 * Angular parameters are expressed in (angUnitName, angUnitConvFactor).
 */
PJ_OBJ *proj_obj_create_conversion_krovak(
    PJ_CONTEXT *ctx, double latitudeProjectionCentre, double longitudeOfOrigin,
    double colatitudeConeAxis, double latitudePseudoStandardParallel,
    double scaleFactorPseudoStandardParallel, double falseEasting,
    double falseNorthing, const char *angUnitName, double angUnitConvFactor,
    const char *linearUnitName, double linearUnitConvFactor) {
    try {
        UnitOfMeasure linearUnit(
            createLinearUnit(linearUnitName, linearUnitConvFactor));
        UnitOfMeasure angUnit(
            createAngularUnit(angUnitName, angUnitConvFactor));
        auto conv = Conversion::createKrovak(
            PropertyMap(), Angle(latitudeProjectionCentre, angUnit),
            Angle(longitudeOfOrigin, angUnit),
            Angle(colatitudeConeAxis, angUnit),
            Angle(latitudePseudoStandardParallel, angUnit),
            Scale(scaleFactorPseudoStandardParallel),
            Length(falseEasting, linearUnit),
            Length(falseNorthing, linearUnit));
        return proj_obj_create_conversion(ctx, conv);
    } catch (const std::exception &e) {
        proj_log_error(ctx, __FUNCTION__, e.what());
    }
    return nullptr;
}
// ---------------------------------------------------------------------------

/** \brief Instanciate a ProjectedCRS with a conversion based on the Lambert
 * Azimuthal Equal Area projection method.
 *
 * See osgeo::proj::operation::Conversion::createLambertAzimuthalEqualArea().
 *
 * Linear parameters are expressed in (linearUnitName, linearUnitConvFactor).
 * Angular parameters are expressed in (angUnitName, angUnitConvFactor).
 */
PJ_OBJ *proj_obj_create_conversion_lambert_azimuthal_equal_area(
    PJ_CONTEXT *ctx, double latitudeNatOrigin, double longitudeNatOrigin,
    double falseEasting, double falseNorthing, const char *angUnitName,
    double angUnitConvFactor, const char *linearUnitName,
    double linearUnitConvFactor) {
    try {
        UnitOfMeasure linearUnit(
            createLinearUnit(linearUnitName, linearUnitConvFactor));
        UnitOfMeasure angUnit(
            createAngularUnit(angUnitName, angUnitConvFactor));
        auto conv = Conversion::createLambertAzimuthalEqualArea(
            PropertyMap(), Angle(latitudeNatOrigin, angUnit),
            Angle(longitudeNatOrigin, angUnit),
            Length(falseEasting, linearUnit),
            Length(falseNorthing, linearUnit));
        return proj_obj_create_conversion(ctx, conv);
    } catch (const std::exception &e) {
        proj_log_error(ctx, __FUNCTION__, e.what());
    }
    return nullptr;
}
// ---------------------------------------------------------------------------

/** \brief Instanciate a ProjectedCRS with a conversion based on the Miller
 * Cylindrical projection method.
 *
 * See osgeo::proj::operation::Conversion::createMillerCylindrical().
 *
 * Linear parameters are expressed in (linearUnitName, linearUnitConvFactor).
 * Angular parameters are expressed in (angUnitName, angUnitConvFactor).
 */
PJ_OBJ *proj_obj_create_conversion_miller_cylindrical(
    PJ_CONTEXT *ctx, double centerLong, double falseEasting,
    double falseNorthing, const char *angUnitName, double angUnitConvFactor,
    const char *linearUnitName, double linearUnitConvFactor) {
    try {
        UnitOfMeasure linearUnit(
            createLinearUnit(linearUnitName, linearUnitConvFactor));
        UnitOfMeasure angUnit(
            createAngularUnit(angUnitName, angUnitConvFactor));
        auto conv = Conversion::createMillerCylindrical(
            PropertyMap(), Angle(centerLong, angUnit),
            Length(falseEasting, linearUnit),
            Length(falseNorthing, linearUnit));
        return proj_obj_create_conversion(ctx, conv);
    } catch (const std::exception &e) {
        proj_log_error(ctx, __FUNCTION__, e.what());
    }
    return nullptr;
}
// ---------------------------------------------------------------------------

/** \brief Instanciate a ProjectedCRS with a conversion based on the Mercator
 * projection method.
 *
 * See osgeo::proj::operation::Conversion::createMercatorVariantA().
 *
 * Linear parameters are expressed in (linearUnitName, linearUnitConvFactor).
 * Angular parameters are expressed in (angUnitName, angUnitConvFactor).
 */
PJ_OBJ *proj_obj_create_conversion_mercator_variant_a(
    PJ_CONTEXT *ctx, double centerLat, double centerLong, double scale,
    double falseEasting, double falseNorthing, const char *angUnitName,
    double angUnitConvFactor, const char *linearUnitName,
    double linearUnitConvFactor) {
    try {
        UnitOfMeasure linearUnit(
            createLinearUnit(linearUnitName, linearUnitConvFactor));
        UnitOfMeasure angUnit(
            createAngularUnit(angUnitName, angUnitConvFactor));
        auto conv = Conversion::createMercatorVariantA(
            PropertyMap(), Angle(centerLat, angUnit),
            Angle(centerLong, angUnit), Scale(scale),
            Length(falseEasting, linearUnit),
            Length(falseNorthing, linearUnit));
        return proj_obj_create_conversion(ctx, conv);
    } catch (const std::exception &e) {
        proj_log_error(ctx, __FUNCTION__, e.what());
    }
    return nullptr;
}
// ---------------------------------------------------------------------------

/** \brief Instanciate a ProjectedCRS with a conversion based on the Mercator
 * projection method.
 *
 * See osgeo::proj::operation::Conversion::createMercatorVariantB().
 *
 * Linear parameters are expressed in (linearUnitName, linearUnitConvFactor).
 * Angular parameters are expressed in (angUnitName, angUnitConvFactor).
 */
PJ_OBJ *proj_obj_create_conversion_mercator_variant_b(
    PJ_CONTEXT *ctx, double latitudeFirstParallel, double centerLong,
    double falseEasting, double falseNorthing, const char *angUnitName,
    double angUnitConvFactor, const char *linearUnitName,
    double linearUnitConvFactor) {
    try {
        UnitOfMeasure linearUnit(
            createLinearUnit(linearUnitName, linearUnitConvFactor));
        UnitOfMeasure angUnit(
            createAngularUnit(angUnitName, angUnitConvFactor));
        auto conv = Conversion::createMercatorVariantB(
            PropertyMap(), Angle(latitudeFirstParallel, angUnit),
            Angle(centerLong, angUnit), Length(falseEasting, linearUnit),
            Length(falseNorthing, linearUnit));
        return proj_obj_create_conversion(ctx, conv);
    } catch (const std::exception &e) {
        proj_log_error(ctx, __FUNCTION__, e.what());
    }
    return nullptr;
}
// ---------------------------------------------------------------------------

/** \brief Instanciate a ProjectedCRS with a conversion based on the Popular
 * Visualisation Pseudo Mercator projection method.
 *
 * See
 * osgeo::proj::operation::Conversion::createPopularVisualisationPseudoMercator().
 *
 * Linear parameters are expressed in (linearUnitName, linearUnitConvFactor).
 * Angular parameters are expressed in (angUnitName, angUnitConvFactor).
 */
PJ_OBJ *proj_obj_create_conversion_popular_visualisation_pseudo_mercator(
    PJ_CONTEXT *ctx, double centerLat, double centerLong, double falseEasting,
    double falseNorthing, const char *angUnitName, double angUnitConvFactor,
    const char *linearUnitName, double linearUnitConvFactor) {
    try {
        UnitOfMeasure linearUnit(
            createLinearUnit(linearUnitName, linearUnitConvFactor));
        UnitOfMeasure angUnit(
            createAngularUnit(angUnitName, angUnitConvFactor));
        auto conv = Conversion::createPopularVisualisationPseudoMercator(
            PropertyMap(), Angle(centerLat, angUnit),
            Angle(centerLong, angUnit), Length(falseEasting, linearUnit),
            Length(falseNorthing, linearUnit));
        return proj_obj_create_conversion(ctx, conv);
    } catch (const std::exception &e) {
        proj_log_error(ctx, __FUNCTION__, e.what());
    }
    return nullptr;
}
// ---------------------------------------------------------------------------

/** \brief Instanciate a ProjectedCRS with a conversion based on the Mollweide
 * projection method.
 *
 * See osgeo::proj::operation::Conversion::createMollweide().
 *
 * Linear parameters are expressed in (linearUnitName, linearUnitConvFactor).
 * Angular parameters are expressed in (angUnitName, angUnitConvFactor).
 */
PJ_OBJ *proj_obj_create_conversion_mollweide(
    PJ_CONTEXT *ctx, double centerLong, double falseEasting,
    double falseNorthing, const char *angUnitName, double angUnitConvFactor,
    const char *linearUnitName, double linearUnitConvFactor) {
    try {
        UnitOfMeasure linearUnit(
            createLinearUnit(linearUnitName, linearUnitConvFactor));
        UnitOfMeasure angUnit(
            createAngularUnit(angUnitName, angUnitConvFactor));
        auto conv = Conversion::createMollweide(
            PropertyMap(), Angle(centerLong, angUnit),
            Length(falseEasting, linearUnit),
            Length(falseNorthing, linearUnit));
        return proj_obj_create_conversion(ctx, conv);
    } catch (const std::exception &e) {
        proj_log_error(ctx, __FUNCTION__, e.what());
    }
    return nullptr;
}
// ---------------------------------------------------------------------------

/** \brief Instanciate a ProjectedCRS with a conversion based on the New Zealand
 * Map Grid projection method.
 *
 * See osgeo::proj::operation::Conversion::createNewZealandMappingGrid().
 *
 * Linear parameters are expressed in (linearUnitName, linearUnitConvFactor).
 * Angular parameters are expressed in (angUnitName, angUnitConvFactor).
 */
PJ_OBJ *proj_obj_create_conversion_new_zealand_mapping_grid(
    PJ_CONTEXT *ctx, double centerLat, double centerLong, double falseEasting,
    double falseNorthing, const char *angUnitName, double angUnitConvFactor,
    const char *linearUnitName, double linearUnitConvFactor) {
    try {
        UnitOfMeasure linearUnit(
            createLinearUnit(linearUnitName, linearUnitConvFactor));
        UnitOfMeasure angUnit(
            createAngularUnit(angUnitName, angUnitConvFactor));
        auto conv = Conversion::createNewZealandMappingGrid(
            PropertyMap(), Angle(centerLat, angUnit),
            Angle(centerLong, angUnit), Length(falseEasting, linearUnit),
            Length(falseNorthing, linearUnit));
        return proj_obj_create_conversion(ctx, conv);
    } catch (const std::exception &e) {
        proj_log_error(ctx, __FUNCTION__, e.what());
    }
    return nullptr;
}
// ---------------------------------------------------------------------------

/** \brief Instanciate a ProjectedCRS with a conversion based on the Oblique
 * Stereographic (Alternative) projection method.
 *
 * See osgeo::proj::operation::Conversion::createObliqueStereographic().
 *
 * Linear parameters are expressed in (linearUnitName, linearUnitConvFactor).
 * Angular parameters are expressed in (angUnitName, angUnitConvFactor).
 */
PJ_OBJ *proj_obj_create_conversion_oblique_stereographic(
    PJ_CONTEXT *ctx, double centerLat, double centerLong, double scale,
    double falseEasting, double falseNorthing, const char *angUnitName,
    double angUnitConvFactor, const char *linearUnitName,
    double linearUnitConvFactor) {
    try {
        UnitOfMeasure linearUnit(
            createLinearUnit(linearUnitName, linearUnitConvFactor));
        UnitOfMeasure angUnit(
            createAngularUnit(angUnitName, angUnitConvFactor));
        auto conv = Conversion::createObliqueStereographic(
            PropertyMap(), Angle(centerLat, angUnit),
            Angle(centerLong, angUnit), Scale(scale),
            Length(falseEasting, linearUnit),
            Length(falseNorthing, linearUnit));
        return proj_obj_create_conversion(ctx, conv);
    } catch (const std::exception &e) {
        proj_log_error(ctx, __FUNCTION__, e.what());
    }
    return nullptr;
}
// ---------------------------------------------------------------------------

/** \brief Instanciate a ProjectedCRS with a conversion based on the
 * Orthographic projection method.
 *
 * See osgeo::proj::operation::Conversion::createOrthographic().
 *
 * Linear parameters are expressed in (linearUnitName, linearUnitConvFactor).
 * Angular parameters are expressed in (angUnitName, angUnitConvFactor).
 */
PJ_OBJ *proj_obj_create_conversion_orthographic(
    PJ_CONTEXT *ctx, double centerLat, double centerLong, double falseEasting,
    double falseNorthing, const char *angUnitName, double angUnitConvFactor,
    const char *linearUnitName, double linearUnitConvFactor) {
    try {
        UnitOfMeasure linearUnit(
            createLinearUnit(linearUnitName, linearUnitConvFactor));
        UnitOfMeasure angUnit(
            createAngularUnit(angUnitName, angUnitConvFactor));
        auto conv = Conversion::createOrthographic(
            PropertyMap(), Angle(centerLat, angUnit),
            Angle(centerLong, angUnit), Length(falseEasting, linearUnit),
            Length(falseNorthing, linearUnit));
        return proj_obj_create_conversion(ctx, conv);
    } catch (const std::exception &e) {
        proj_log_error(ctx, __FUNCTION__, e.what());
    }
    return nullptr;
}
// ---------------------------------------------------------------------------

/** \brief Instanciate a ProjectedCRS with a conversion based on the American
 * Polyconic projection method.
 *
 * See osgeo::proj::operation::Conversion::createAmericanPolyconic().
 *
 * Linear parameters are expressed in (linearUnitName, linearUnitConvFactor).
 * Angular parameters are expressed in (angUnitName, angUnitConvFactor).
 */
PJ_OBJ *proj_obj_create_conversion_american_polyconic(
    PJ_CONTEXT *ctx, double centerLat, double centerLong, double falseEasting,
    double falseNorthing, const char *angUnitName, double angUnitConvFactor,
    const char *linearUnitName, double linearUnitConvFactor) {
    try {
        UnitOfMeasure linearUnit(
            createLinearUnit(linearUnitName, linearUnitConvFactor));
        UnitOfMeasure angUnit(
            createAngularUnit(angUnitName, angUnitConvFactor));
        auto conv = Conversion::createAmericanPolyconic(
            PropertyMap(), Angle(centerLat, angUnit),
            Angle(centerLong, angUnit), Length(falseEasting, linearUnit),
            Length(falseNorthing, linearUnit));
        return proj_obj_create_conversion(ctx, conv);
    } catch (const std::exception &e) {
        proj_log_error(ctx, __FUNCTION__, e.what());
    }
    return nullptr;
}
// ---------------------------------------------------------------------------

/** \brief Instanciate a ProjectedCRS with a conversion based on the Polar
 * Stereographic (Variant A) projection method.
 *
 * See osgeo::proj::operation::Conversion::createPolarStereographicVariantA().
 *
 * Linear parameters are expressed in (linearUnitName, linearUnitConvFactor).
 * Angular parameters are expressed in (angUnitName, angUnitConvFactor).
 */
PJ_OBJ *proj_obj_create_conversion_polar_stereographic_variant_a(
    PJ_CONTEXT *ctx, double centerLat, double centerLong, double scale,
    double falseEasting, double falseNorthing, const char *angUnitName,
    double angUnitConvFactor, const char *linearUnitName,
    double linearUnitConvFactor) {
    try {
        UnitOfMeasure linearUnit(
            createLinearUnit(linearUnitName, linearUnitConvFactor));
        UnitOfMeasure angUnit(
            createAngularUnit(angUnitName, angUnitConvFactor));
        auto conv = Conversion::createPolarStereographicVariantA(
            PropertyMap(), Angle(centerLat, angUnit),
            Angle(centerLong, angUnit), Scale(scale),
            Length(falseEasting, linearUnit),
            Length(falseNorthing, linearUnit));
        return proj_obj_create_conversion(ctx, conv);
    } catch (const std::exception &e) {
        proj_log_error(ctx, __FUNCTION__, e.what());
    }
    return nullptr;
}
// ---------------------------------------------------------------------------

/** \brief Instanciate a ProjectedCRS with a conversion based on the Polar
 * Stereographic (Variant B) projection method.
 *
 * See osgeo::proj::operation::Conversion::createPolarStereographicVariantB().
 *
 * Linear parameters are expressed in (linearUnitName, linearUnitConvFactor).
 * Angular parameters are expressed in (angUnitName, angUnitConvFactor).
 */
PJ_OBJ *proj_obj_create_conversion_polar_stereographic_variant_b(
    PJ_CONTEXT *ctx, double latitudeStandardParallel, double longitudeOfOrigin,
    double falseEasting, double falseNorthing, const char *angUnitName,
    double angUnitConvFactor, const char *linearUnitName,
    double linearUnitConvFactor) {
    try {
        UnitOfMeasure linearUnit(
            createLinearUnit(linearUnitName, linearUnitConvFactor));
        UnitOfMeasure angUnit(
            createAngularUnit(angUnitName, angUnitConvFactor));
        auto conv = Conversion::createPolarStereographicVariantB(
            PropertyMap(), Angle(latitudeStandardParallel, angUnit),
            Angle(longitudeOfOrigin, angUnit), Length(falseEasting, linearUnit),
            Length(falseNorthing, linearUnit));
        return proj_obj_create_conversion(ctx, conv);
    } catch (const std::exception &e) {
        proj_log_error(ctx, __FUNCTION__, e.what());
    }
    return nullptr;
}
// ---------------------------------------------------------------------------

/** \brief Instanciate a ProjectedCRS with a conversion based on the Robinson
 * projection method.
 *
 * See osgeo::proj::operation::Conversion::createRobinson().
 *
 * Linear parameters are expressed in (linearUnitName, linearUnitConvFactor).
 * Angular parameters are expressed in (angUnitName, angUnitConvFactor).
 */
PJ_OBJ *proj_obj_create_conversion_robinson(
    PJ_CONTEXT *ctx, double centerLong, double falseEasting,
    double falseNorthing, const char *angUnitName, double angUnitConvFactor,
    const char *linearUnitName, double linearUnitConvFactor) {
    try {
        UnitOfMeasure linearUnit(
            createLinearUnit(linearUnitName, linearUnitConvFactor));
        UnitOfMeasure angUnit(
            createAngularUnit(angUnitName, angUnitConvFactor));
        auto conv = Conversion::createRobinson(
            PropertyMap(), Angle(centerLong, angUnit),
            Length(falseEasting, linearUnit),
            Length(falseNorthing, linearUnit));
        return proj_obj_create_conversion(ctx, conv);
    } catch (const std::exception &e) {
        proj_log_error(ctx, __FUNCTION__, e.what());
    }
    return nullptr;
}
// ---------------------------------------------------------------------------

/** \brief Instanciate a ProjectedCRS with a conversion based on the Sinusoidal
 * projection method.
 *
 * See osgeo::proj::operation::Conversion::createSinusoidal().
 *
 * Linear parameters are expressed in (linearUnitName, linearUnitConvFactor).
 * Angular parameters are expressed in (angUnitName, angUnitConvFactor).
 */
PJ_OBJ *proj_obj_create_conversion_sinusoidal(
    PJ_CONTEXT *ctx, double centerLong, double falseEasting,
    double falseNorthing, const char *angUnitName, double angUnitConvFactor,
    const char *linearUnitName, double linearUnitConvFactor) {
    try {
        UnitOfMeasure linearUnit(
            createLinearUnit(linearUnitName, linearUnitConvFactor));
        UnitOfMeasure angUnit(
            createAngularUnit(angUnitName, angUnitConvFactor));
        auto conv = Conversion::createSinusoidal(
            PropertyMap(), Angle(centerLong, angUnit),
            Length(falseEasting, linearUnit),
            Length(falseNorthing, linearUnit));
        return proj_obj_create_conversion(ctx, conv);
    } catch (const std::exception &e) {
        proj_log_error(ctx, __FUNCTION__, e.what());
    }
    return nullptr;
}
// ---------------------------------------------------------------------------

/** \brief Instanciate a ProjectedCRS with a conversion based on the
 * Stereographic projection method.
 *
 * See osgeo::proj::operation::Conversion::createStereographic().
 *
 * Linear parameters are expressed in (linearUnitName, linearUnitConvFactor).
 * Angular parameters are expressed in (angUnitName, angUnitConvFactor).
 */
PJ_OBJ *proj_obj_create_conversion_stereographic(
    PJ_CONTEXT *ctx, double centerLat, double centerLong, double scale,
    double falseEasting, double falseNorthing, const char *angUnitName,
    double angUnitConvFactor, const char *linearUnitName,
    double linearUnitConvFactor) {
    try {
        UnitOfMeasure linearUnit(
            createLinearUnit(linearUnitName, linearUnitConvFactor));
        UnitOfMeasure angUnit(
            createAngularUnit(angUnitName, angUnitConvFactor));
        auto conv = Conversion::createStereographic(
            PropertyMap(), Angle(centerLat, angUnit),
            Angle(centerLong, angUnit), Scale(scale),
            Length(falseEasting, linearUnit),
            Length(falseNorthing, linearUnit));
        return proj_obj_create_conversion(ctx, conv);
    } catch (const std::exception &e) {
        proj_log_error(ctx, __FUNCTION__, e.what());
    }
    return nullptr;
}
// ---------------------------------------------------------------------------

/** \brief Instanciate a ProjectedCRS with a conversion based on the Van der
 * Grinten projection method.
 *
 * See osgeo::proj::operation::Conversion::createVanDerGrinten().
 *
 * Linear parameters are expressed in (linearUnitName, linearUnitConvFactor).
 * Angular parameters are expressed in (angUnitName, angUnitConvFactor).
 */
PJ_OBJ *proj_obj_create_conversion_van_der_grinten(
    PJ_CONTEXT *ctx, double centerLong, double falseEasting,
    double falseNorthing, const char *angUnitName, double angUnitConvFactor,
    const char *linearUnitName, double linearUnitConvFactor) {
    try {
        UnitOfMeasure linearUnit(
            createLinearUnit(linearUnitName, linearUnitConvFactor));
        UnitOfMeasure angUnit(
            createAngularUnit(angUnitName, angUnitConvFactor));
        auto conv = Conversion::createVanDerGrinten(
            PropertyMap(), Angle(centerLong, angUnit),
            Length(falseEasting, linearUnit),
            Length(falseNorthing, linearUnit));
        return proj_obj_create_conversion(ctx, conv);
    } catch (const std::exception &e) {
        proj_log_error(ctx, __FUNCTION__, e.what());
    }
    return nullptr;
}
// ---------------------------------------------------------------------------

/** \brief Instanciate a ProjectedCRS with a conversion based on the Wagner I
 * projection method.
 *
 * See osgeo::proj::operation::Conversion::createWagnerI().
 *
 * Linear parameters are expressed in (linearUnitName, linearUnitConvFactor).
 * Angular parameters are expressed in (angUnitName, angUnitConvFactor).
 */
PJ_OBJ *proj_obj_create_conversion_wagner_i(
    PJ_CONTEXT *ctx, double centerLong, double falseEasting,
    double falseNorthing, const char *angUnitName, double angUnitConvFactor,
    const char *linearUnitName, double linearUnitConvFactor) {
    try {
        UnitOfMeasure linearUnit(
            createLinearUnit(linearUnitName, linearUnitConvFactor));
        UnitOfMeasure angUnit(
            createAngularUnit(angUnitName, angUnitConvFactor));
        auto conv =
            Conversion::createWagnerI(PropertyMap(), Angle(centerLong, angUnit),
                                      Length(falseEasting, linearUnit),
                                      Length(falseNorthing, linearUnit));
        return proj_obj_create_conversion(ctx, conv);
    } catch (const std::exception &e) {
        proj_log_error(ctx, __FUNCTION__, e.what());
    }
    return nullptr;
}
// ---------------------------------------------------------------------------

/** \brief Instanciate a ProjectedCRS with a conversion based on the Wagner II
 * projection method.
 *
 * See osgeo::proj::operation::Conversion::createWagnerII().
 *
 * Linear parameters are expressed in (linearUnitName, linearUnitConvFactor).
 * Angular parameters are expressed in (angUnitName, angUnitConvFactor).
 */
PJ_OBJ *proj_obj_create_conversion_wagner_ii(
    PJ_CONTEXT *ctx, double centerLong, double falseEasting,
    double falseNorthing, const char *angUnitName, double angUnitConvFactor,
    const char *linearUnitName, double linearUnitConvFactor) {
    try {
        UnitOfMeasure linearUnit(
            createLinearUnit(linearUnitName, linearUnitConvFactor));
        UnitOfMeasure angUnit(
            createAngularUnit(angUnitName, angUnitConvFactor));
        auto conv = Conversion::createWagnerII(
            PropertyMap(), Angle(centerLong, angUnit),
            Length(falseEasting, linearUnit),
            Length(falseNorthing, linearUnit));
        return proj_obj_create_conversion(ctx, conv);
    } catch (const std::exception &e) {
        proj_log_error(ctx, __FUNCTION__, e.what());
    }
    return nullptr;
}
// ---------------------------------------------------------------------------

/** \brief Instanciate a ProjectedCRS with a conversion based on the Wagner III
 * projection method.
 *
 * See osgeo::proj::operation::Conversion::createWagnerIII().
 *
 * Linear parameters are expressed in (linearUnitName, linearUnitConvFactor).
 * Angular parameters are expressed in (angUnitName, angUnitConvFactor).
 */
PJ_OBJ *proj_obj_create_conversion_wagner_iii(
    PJ_CONTEXT *ctx, double latitudeTrueScale, double centerLong,
    double falseEasting, double falseNorthing, const char *angUnitName,
    double angUnitConvFactor, const char *linearUnitName,
    double linearUnitConvFactor) {
    try {
        UnitOfMeasure linearUnit(
            createLinearUnit(linearUnitName, linearUnitConvFactor));
        UnitOfMeasure angUnit(
            createAngularUnit(angUnitName, angUnitConvFactor));
        auto conv = Conversion::createWagnerIII(
            PropertyMap(), Angle(latitudeTrueScale, angUnit),
            Angle(centerLong, angUnit), Length(falseEasting, linearUnit),
            Length(falseNorthing, linearUnit));
        return proj_obj_create_conversion(ctx, conv);
    } catch (const std::exception &e) {
        proj_log_error(ctx, __FUNCTION__, e.what());
    }
    return nullptr;
}
// ---------------------------------------------------------------------------

/** \brief Instanciate a ProjectedCRS with a conversion based on the Wagner IV
 * projection method.
 *
 * See osgeo::proj::operation::Conversion::createWagnerIV().
 *
 * Linear parameters are expressed in (linearUnitName, linearUnitConvFactor).
 * Angular parameters are expressed in (angUnitName, angUnitConvFactor).
 */
PJ_OBJ *proj_obj_create_conversion_wagner_iv(
    PJ_CONTEXT *ctx, double centerLong, double falseEasting,
    double falseNorthing, const char *angUnitName, double angUnitConvFactor,
    const char *linearUnitName, double linearUnitConvFactor) {
    try {
        UnitOfMeasure linearUnit(
            createLinearUnit(linearUnitName, linearUnitConvFactor));
        UnitOfMeasure angUnit(
            createAngularUnit(angUnitName, angUnitConvFactor));
        auto conv = Conversion::createWagnerIV(
            PropertyMap(), Angle(centerLong, angUnit),
            Length(falseEasting, linearUnit),
            Length(falseNorthing, linearUnit));
        return proj_obj_create_conversion(ctx, conv);
    } catch (const std::exception &e) {
        proj_log_error(ctx, __FUNCTION__, e.what());
    }
    return nullptr;
}
// ---------------------------------------------------------------------------

/** \brief Instanciate a ProjectedCRS with a conversion based on the Wagner V
 * projection method.
 *
 * See osgeo::proj::operation::Conversion::createWagnerV().
 *
 * Linear parameters are expressed in (linearUnitName, linearUnitConvFactor).
 * Angular parameters are expressed in (angUnitName, angUnitConvFactor).
 */
PJ_OBJ *proj_obj_create_conversion_wagner_v(
    PJ_CONTEXT *ctx, double centerLong, double falseEasting,
    double falseNorthing, const char *angUnitName, double angUnitConvFactor,
    const char *linearUnitName, double linearUnitConvFactor) {
    try {
        UnitOfMeasure linearUnit(
            createLinearUnit(linearUnitName, linearUnitConvFactor));
        UnitOfMeasure angUnit(
            createAngularUnit(angUnitName, angUnitConvFactor));
        auto conv =
            Conversion::createWagnerV(PropertyMap(), Angle(centerLong, angUnit),
                                      Length(falseEasting, linearUnit),
                                      Length(falseNorthing, linearUnit));
        return proj_obj_create_conversion(ctx, conv);
    } catch (const std::exception &e) {
        proj_log_error(ctx, __FUNCTION__, e.what());
    }
    return nullptr;
}
// ---------------------------------------------------------------------------

/** \brief Instanciate a ProjectedCRS with a conversion based on the Wagner VI
 * projection method.
 *
 * See osgeo::proj::operation::Conversion::createWagnerVI().
 *
 * Linear parameters are expressed in (linearUnitName, linearUnitConvFactor).
 * Angular parameters are expressed in (angUnitName, angUnitConvFactor).
 */
PJ_OBJ *proj_obj_create_conversion_wagner_vi(
    PJ_CONTEXT *ctx, double centerLong, double falseEasting,
    double falseNorthing, const char *angUnitName, double angUnitConvFactor,
    const char *linearUnitName, double linearUnitConvFactor) {
    try {
        UnitOfMeasure linearUnit(
            createLinearUnit(linearUnitName, linearUnitConvFactor));
        UnitOfMeasure angUnit(
            createAngularUnit(angUnitName, angUnitConvFactor));
        auto conv = Conversion::createWagnerVI(
            PropertyMap(), Angle(centerLong, angUnit),
            Length(falseEasting, linearUnit),
            Length(falseNorthing, linearUnit));
        return proj_obj_create_conversion(ctx, conv);
    } catch (const std::exception &e) {
        proj_log_error(ctx, __FUNCTION__, e.what());
    }
    return nullptr;
}
// ---------------------------------------------------------------------------

/** \brief Instanciate a ProjectedCRS with a conversion based on the Wagner VII
 * projection method.
 *
 * See osgeo::proj::operation::Conversion::createWagnerVII().
 *
 * Linear parameters are expressed in (linearUnitName, linearUnitConvFactor).
 * Angular parameters are expressed in (angUnitName, angUnitConvFactor).
 */
PJ_OBJ *proj_obj_create_conversion_wagner_vii(
    PJ_CONTEXT *ctx, double centerLong, double falseEasting,
    double falseNorthing, const char *angUnitName, double angUnitConvFactor,
    const char *linearUnitName, double linearUnitConvFactor) {
    try {
        UnitOfMeasure linearUnit(
            createLinearUnit(linearUnitName, linearUnitConvFactor));
        UnitOfMeasure angUnit(
            createAngularUnit(angUnitName, angUnitConvFactor));
        auto conv = Conversion::createWagnerVII(
            PropertyMap(), Angle(centerLong, angUnit),
            Length(falseEasting, linearUnit),
            Length(falseNorthing, linearUnit));
        return proj_obj_create_conversion(ctx, conv);
    } catch (const std::exception &e) {
        proj_log_error(ctx, __FUNCTION__, e.what());
    }
    return nullptr;
}
// ---------------------------------------------------------------------------

/** \brief Instanciate a ProjectedCRS with a conversion based on the
 * Quadrilateralized Spherical Cube projection method.
 *
 * See
 * osgeo::proj::operation::Conversion::createQuadrilateralizedSphericalCube().
 *
 * Linear parameters are expressed in (linearUnitName, linearUnitConvFactor).
 * Angular parameters are expressed in (angUnitName, angUnitConvFactor).
 */
PJ_OBJ *proj_obj_create_conversion_quadrilateralized_spherical_cube(
    PJ_CONTEXT *ctx, double centerLat, double centerLong, double falseEasting,
    double falseNorthing, const char *angUnitName, double angUnitConvFactor,
    const char *linearUnitName, double linearUnitConvFactor) {
    try {
        UnitOfMeasure linearUnit(
            createLinearUnit(linearUnitName, linearUnitConvFactor));
        UnitOfMeasure angUnit(
            createAngularUnit(angUnitName, angUnitConvFactor));
        auto conv = Conversion::createQuadrilateralizedSphericalCube(
            PropertyMap(), Angle(centerLat, angUnit),
            Angle(centerLong, angUnit), Length(falseEasting, linearUnit),
            Length(falseNorthing, linearUnit));
        return proj_obj_create_conversion(ctx, conv);
    } catch (const std::exception &e) {
        proj_log_error(ctx, __FUNCTION__, e.what());
    }
    return nullptr;
}
// ---------------------------------------------------------------------------

/** \brief Instanciate a ProjectedCRS with a conversion based on the Spherical
 * Cross-Track Height projection method.
 *
 * See osgeo::proj::operation::Conversion::createSphericalCrossTrackHeight().
 *
 * Linear parameters are expressed in (linearUnitName, linearUnitConvFactor).
 * Angular parameters are expressed in (angUnitName, angUnitConvFactor).
 */
PJ_OBJ *proj_obj_create_conversion_spherical_cross_track_height(
    PJ_CONTEXT *ctx, double pegPointLat, double pegPointLong,
    double pegPointHeading, double pegPointHeight, const char *angUnitName,
    double angUnitConvFactor, const char *linearUnitName,
    double linearUnitConvFactor) {
    try {
        UnitOfMeasure linearUnit(
            createLinearUnit(linearUnitName, linearUnitConvFactor));
        UnitOfMeasure angUnit(
            createAngularUnit(angUnitName, angUnitConvFactor));
        auto conv = Conversion::createSphericalCrossTrackHeight(
            PropertyMap(), Angle(pegPointLat, angUnit),
            Angle(pegPointLong, angUnit), Angle(pegPointHeading, angUnit),
            Length(pegPointHeight, linearUnit));
        return proj_obj_create_conversion(ctx, conv);
    } catch (const std::exception &e) {
        proj_log_error(ctx, __FUNCTION__, e.what());
    }
    return nullptr;
}
// ---------------------------------------------------------------------------

/** \brief Instanciate a ProjectedCRS with a conversion based on the Equal Earth
 * projection method.
 *
 * See osgeo::proj::operation::Conversion::createEqualEarth().
 *
 * Linear parameters are expressed in (linearUnitName, linearUnitConvFactor).
 * Angular parameters are expressed in (angUnitName, angUnitConvFactor).
 */
PJ_OBJ *proj_obj_create_conversion_equal_earth(
    PJ_CONTEXT *ctx, double centerLong, double falseEasting,
    double falseNorthing, const char *angUnitName, double angUnitConvFactor,
    const char *linearUnitName, double linearUnitConvFactor) {
    try {
        UnitOfMeasure linearUnit(
            createLinearUnit(linearUnitName, linearUnitConvFactor));
        UnitOfMeasure angUnit(
            createAngularUnit(angUnitName, angUnitConvFactor));
        auto conv = Conversion::createEqualEarth(
            PropertyMap(), Angle(centerLong, angUnit),
            Length(falseEasting, linearUnit),
            Length(falseNorthing, linearUnit));
        return proj_obj_create_conversion(ctx, conv);
    } catch (const std::exception &e) {
        proj_log_error(ctx, __FUNCTION__, e.what());
    }
    return nullptr;
}
/* END: Generated by scripts/create_c_api_projections.py*/

// ---------------------------------------------------------------------------

/** \brief Return whether a coordinate operation can be instanciated as
 * a PROJ pipeline, checking in particular that referenced grids are
 * available.
 *
 * @param coordoperation Objet of type CoordinateOperation or derived classes
 * (must not be NULL)
 * @return TRUE or FALSE.
 */

int proj_coordoperation_is_instanciable(const PJ_OBJ *coordoperation) {
    assert(coordoperation);
    auto op =
        dynamic_cast<const CoordinateOperation *>(coordoperation->obj.get());
    if (!op) {
        proj_log_error(coordoperation->ctx, __FUNCTION__,
                       "Object is not a CoordinateOperation");
        return 0;
    }
    auto dbContext = getDBcontextNoException(coordoperation->ctx, __FUNCTION__);
    try {
        return op->isPROJInstanciable(dbContext) ? 1 : 0;
    } catch (const std::exception &) {
        return 0;
    }
}

// ---------------------------------------------------------------------------

/** \brief Return the number of parameters of a SingleOperation
 *
 * @param coordoperation Objet of type SingleOperation or derived classes
 * (must not be NULL)
 */

int proj_coordoperation_get_param_count(const PJ_OBJ *coordoperation) {
    assert(coordoperation);
    auto op = dynamic_cast<const SingleOperation *>(coordoperation->obj.get());
    if (!op) {
        proj_log_error(coordoperation->ctx, __FUNCTION__,
                       "Object is not a SingleOperation");
        return 0;
    }
    return static_cast<int>(op->parameterValues().size());
}

// ---------------------------------------------------------------------------

/** \brief Return the index of a parameter of a SingleOperation
 *
 * @param coordoperation Objet of type SingleOperation or derived classes
 * (must not be NULL)
 * @param name Parameter name. Must not be NULL
 * @return index (>=0), or -1 in case of error.
 */

int proj_coordoperation_get_param_index(const PJ_OBJ *coordoperation,
                                        const char *name) {
    assert(coordoperation);
    assert(name);
    auto op = dynamic_cast<const SingleOperation *>(coordoperation->obj.get());
    if (!op) {
        proj_log_error(coordoperation->ctx, __FUNCTION__,
                       "Object is not a SingleOperation");
        return -1;
    }
    int index = 0;
    for (const auto &genParam : op->method()->parameters()) {
        if (Identifier::isEquivalentName(genParam->nameStr().c_str(), name)) {
            return index;
        }
        index++;
    }
    return -1;
}

// ---------------------------------------------------------------------------

/** \brief Return a parameter of a SingleOperation
 *
 * @param coordoperation Objet of type SingleOperation or derived classes
 * (must not be NULL)
 * @param index Parameter index.
 * @param pName Pointer to a string value to store the parameter name. or NULL
 * @param pNameAuthorityName Pointer to a string value to store the parameter
 * authority name. or NULL
 * @param pNameCode Pointer to a string value to store the parameter
 * code. or NULL
 * @param pValue Pointer to a double value to store the parameter
 * value (if numeric). or NULL
 * @param pValueString Pointer to a string value to store the parameter
 * value (if of type string). or NULL
 * @param pValueUnitConvFactor Pointer to a double value to store the parameter
 * unit conversion factor. or NULL
 * @param pValueUnitName Pointer to a string value to store the parameter
 * unit name. or NULL
 * @return TRUE in case of success.
 */

int proj_coordoperation_get_param(const PJ_OBJ *coordoperation, int index,
                                  const char **pName,
                                  const char **pNameAuthorityName,
                                  const char **pNameCode, double *pValue,
                                  const char **pValueString,
                                  double *pValueUnitConvFactor,
                                  const char **pValueUnitName) {
    assert(coordoperation);
    auto op = dynamic_cast<const SingleOperation *>(coordoperation->obj.get());
    if (!op) {
        proj_log_error(coordoperation->ctx, __FUNCTION__,
                       "Object is not a SingleOperation");
        return false;
    }
    const auto &parameters = op->method()->parameters();
    const auto &values = op->parameterValues();
    if (static_cast<size_t>(index) >= parameters.size() ||
        static_cast<size_t>(index) >= values.size()) {
        proj_log_error(coordoperation->ctx, __FUNCTION__, "Invalid index");
        return false;
    }

    const auto &param = parameters[index];
    const auto &param_ids = param->identifiers();
    if (pName) {
        *pName = param->name()->description()->c_str();
    }
    if (pNameAuthorityName) {
        if (!param_ids.empty()) {
            *pNameAuthorityName = param_ids[0]->codeSpace()->c_str();
        } else {
            *pNameAuthorityName = nullptr;
        }
    }
    if (pNameCode) {
        if (!param_ids.empty()) {
            *pNameCode = param_ids[0]->code().c_str();
        } else {
            *pNameCode = nullptr;
        }
    }

    const auto &value = values[index];
    ParameterValuePtr paramValue = nullptr;
    auto opParamValue =
        dynamic_cast<const OperationParameterValue *>(value.get());
    if (opParamValue) {
        paramValue = opParamValue->parameterValue().as_nullable();
    }
    if (pValue) {
        *pValue = 0;
        if (paramValue) {
            if (paramValue->type() == ParameterValue::Type::MEASURE) {
                *pValue = paramValue->value().value();
            }
        }
    }
    if (pValueString) {
        *pValueString = nullptr;
        if (paramValue) {
            if (paramValue->type() == ParameterValue::Type::FILENAME) {
                *pValueString = paramValue->valueFile().c_str();
            } else if (paramValue->type() == ParameterValue::Type::STRING) {
                *pValueString = paramValue->stringValue().c_str();
            }
        }
    }
    if (pValueUnitConvFactor) {
        *pValueUnitConvFactor = 0;
        if (paramValue) {
            if (paramValue->type() == ParameterValue::Type::MEASURE) {
                *pValueUnitConvFactor =
                    paramValue->value().unit().conversionToSI();
            }
        }
    }
    if (pValueUnitName) {
        *pValueUnitName = nullptr;
        if (paramValue) {
            if (paramValue->type() == ParameterValue::Type::MEASURE) {
                *pValueUnitName = paramValue->value().unit().name().c_str();
            }
        }
    }

    return true;
}

// ---------------------------------------------------------------------------

/** \brief Return the number of grids used by a CoordinateOperation
 *
 * @param coordoperation Objet of type CoordinateOperation or derived classes
 * (must not be NULL)
 */

int proj_coordoperation_get_grid_used_count(const PJ_OBJ *coordoperation) {
    assert(coordoperation);
    auto co =
        dynamic_cast<const CoordinateOperation *>(coordoperation->obj.get());
    if (!co) {
        proj_log_error(coordoperation->ctx, __FUNCTION__,
                       "Object is not a CoordinateOperation");
        return 0;
    }
    auto dbContext = getDBcontextNoException(coordoperation->ctx, __FUNCTION__);
    try {
        if (!coordoperation->gridsNeededAsked) {
            coordoperation->gridsNeededAsked = true;
            const auto gridsNeeded = co->gridsNeeded(dbContext);
            for (const auto &gridDesc : gridsNeeded) {
                coordoperation->gridsNeeded.emplace_back(gridDesc);
            }
        }
        return static_cast<int>(coordoperation->gridsNeeded.size());
    } catch (const std::exception &e) {
        proj_log_error(coordoperation->ctx, __FUNCTION__, e.what());
        return 0;
    }
}

// ---------------------------------------------------------------------------

/** \brief Return a parameter of a SingleOperation
 *
 * @param coordoperation Objet of type SingleOperation or derived classes
 * (must not be NULL)
 * @param index Parameter index.
 * @param pShortName Pointer to a string value to store the grid short name. or
 * NULL
 * @param pFullName Pointer to a string value to store the grid full filename.
 * or NULL
 * @param pPackageName Pointer to a string value to store the package name where
 * the grid might be found. or NULL
 * @param pURL Pointer to a string value to store the grid URL or the
 * package URL where the grid might be found. or NULL
 * @param pDirectDownload Pointer to a int (boolean) value to store whether
 * *pURL can be downloaded directly. or NULL
 * @param pOpenLicense Pointer to a int (boolean) value to store whether
 * the grid is released with an open license. or NULL
 * @param pAvailable Pointer to a int (boolean) value to store whether the grid
 * is available at runtime. or NULL
 * @return TRUE in case of success.
 */

int proj_coordoperation_get_grid_used(const PJ_OBJ *coordoperation, int index,
                                      const char **pShortName,
                                      const char **pFullName,
                                      const char **pPackageName,
                                      const char **pURL, int *pDirectDownload,
                                      int *pOpenLicense, int *pAvailable) {
    const int count = proj_coordoperation_get_grid_used_count(coordoperation);
    if (index < 0 || index >= count) {
        proj_log_error(coordoperation->ctx, __FUNCTION__, "Invalid index");
        return false;
    }

    const auto &gridDesc = coordoperation->gridsNeeded[index];
    if (pShortName) {
        *pShortName = gridDesc.shortName.c_str();
    }

    if (pFullName) {
        *pFullName = gridDesc.fullName.c_str();
    }

    if (pPackageName) {
        *pPackageName = gridDesc.packageName.c_str();
    }

    if (pURL) {
        *pURL = gridDesc.url.c_str();
    }

    if (pDirectDownload) {
        *pDirectDownload = gridDesc.directDownload;
    }

    if (pOpenLicense) {
        *pOpenLicense = gridDesc.openLicense;
    }

    if (pAvailable) {
        *pAvailable = gridDesc.available;
    }

    return true;
}

// ---------------------------------------------------------------------------

/** \brief Opaque object representing an operation factory context. */
struct PJ_OPERATION_FACTORY_CONTEXT {
    //! @cond Doxygen_Suppress
    PJ_CONTEXT *ctx;
    CoordinateOperationContextNNPtr operationContext;

    explicit PJ_OPERATION_FACTORY_CONTEXT(
        PJ_CONTEXT *ctxIn, CoordinateOperationContextNNPtr &&operationContextIn)
        : ctx(ctxIn), operationContext(std::move(operationContextIn)) {}

    PJ_OPERATION_FACTORY_CONTEXT(const PJ_OPERATION_FACTORY_CONTEXT &) = delete;
    PJ_OPERATION_FACTORY_CONTEXT &
    operator=(const PJ_OPERATION_FACTORY_CONTEXT &) = delete;
    //! @endcond
};

// ---------------------------------------------------------------------------

/** \brief Instanciate a context for building coordinate operations between
 * two CRS.
 *
 * The returned object must be unreferenced with
 * proj_operation_factory_context_unref() after use.
 *
 * @param ctx Context, or NULL for default context.
 * @param authority Name of authority to which to restrict the search of
 *                  canidate operations. Or NULL to allow any authority.
 * @return Object that must be unreferenced with
 * proj_operation_factory_context_unref(), or NULL in
 * case of error.
 */
PJ_OPERATION_FACTORY_CONTEXT *
proj_create_operation_factory_context(PJ_CONTEXT *ctx, const char *authority) {
    SANITIZE_CTX(ctx);
    auto dbContext = getDBcontextNoException(ctx, __FUNCTION__);
    try {
        if (dbContext) {
            auto factory = CoordinateOperationFactory::create();
            auto authFactory = AuthorityFactory::create(
                NN_NO_CHECK(dbContext),
                std::string(authority ? authority : ""));
            auto operationContext =
                CoordinateOperationContext::create(authFactory, nullptr, 0.0);
            return new PJ_OPERATION_FACTORY_CONTEXT(
                ctx, std::move(operationContext));
        } else {
            auto operationContext =
                CoordinateOperationContext::create(nullptr, nullptr, 0.0);
            return new PJ_OPERATION_FACTORY_CONTEXT(
                ctx, std::move(operationContext));
        }
    } catch (const std::exception &e) {
        proj_log_error(ctx, __FUNCTION__, e.what());
    }
    return nullptr;
}

// ---------------------------------------------------------------------------

/** \brief Drops a reference on an object.
 *
 * This method should be called one and exactly one for each function
 * returning a PJ_OPERATION_FACTORY_CONTEXT*
 *
 * @param ctxt Object, or NULL.
 */
void proj_operation_factory_context_unref(PJ_OPERATION_FACTORY_CONTEXT *ctxt) {
    delete ctxt;
}

// ---------------------------------------------------------------------------

/** \brief Set the desired accuracy of the resulting coordinate transformations.
 * @param ctxt Operation factory context. must not be NULL
 * @param accuracy Accuracy in meter (or 0 to disable the filter).
 */
void proj_operation_factory_context_set_desired_accuracy(
    PJ_OPERATION_FACTORY_CONTEXT *ctxt, double accuracy) {
    assert(ctxt);
    ctxt->operationContext->setDesiredAccuracy(accuracy);
}

// ---------------------------------------------------------------------------

/** \brief Set the desired area of interest for the resulting coordinate
 * transformations.
 *
 * For an area of interest crossing the anti-meridian, west_lon_degree will be
 * greater than east_lon_degree.
 *
 * @param ctxt Operation factory context. must not be NULL
 * @param west_lon_degree West longitude (in degrees).
 * @param south_lat_degree South latitude (in degrees).
 * @param east_lon_degree East longitude (in degrees).
 * @param north_lat_degree North latitude (in degrees).
 */
void proj_operation_factory_context_set_area_of_interest(
    PJ_OPERATION_FACTORY_CONTEXT *ctxt, double west_lon_degree,
    double south_lat_degree, double east_lon_degree, double north_lat_degree) {
    assert(ctxt);
    ctxt->operationContext->setAreaOfInterest(Extent::createFromBBOX(
        west_lon_degree, south_lat_degree, east_lon_degree, north_lat_degree));
}

// ---------------------------------------------------------------------------

/** \brief Set how source and target CRS extent should be used
 * when considering if a transformation can be used (only takes effect if
 * no area of interest is explicitly defined).
 *
 * The default is PJ_CRS_EXTENT_SMALLEST.
 *
 * @param ctxt Operation factory context. must not be NULL
 * @param use How source and target CRS extent should be used.
 */
void proj_operation_factory_context_set_crs_extent_use(
    PJ_OPERATION_FACTORY_CONTEXT *ctxt, PROJ_CRS_EXTENT_USE use) {
    assert(ctxt);
    switch (use) {
    case PJ_CRS_EXTENT_NONE:
        ctxt->operationContext->setSourceAndTargetCRSExtentUse(
            CoordinateOperationContext::SourceTargetCRSExtentUse::NONE);
        break;

    case PJ_CRS_EXTENT_BOTH:
        ctxt->operationContext->setSourceAndTargetCRSExtentUse(
            CoordinateOperationContext::SourceTargetCRSExtentUse::BOTH);
        break;

    case PJ_CRS_EXTENT_INTERSECTION:
        ctxt->operationContext->setSourceAndTargetCRSExtentUse(
            CoordinateOperationContext::SourceTargetCRSExtentUse::INTERSECTION);
        break;

    case PJ_CRS_EXTENT_SMALLEST:
        ctxt->operationContext->setSourceAndTargetCRSExtentUse(
            CoordinateOperationContext::SourceTargetCRSExtentUse::SMALLEST);
        break;
    }
}

// ---------------------------------------------------------------------------

/** \brief Set the spatial criterion to use when comparing the area of
 * validity of coordinate operations with the area of interest / area of
 * validity of
 * source and target CRS.
 *
 * The default is PROJ_SPATIAL_CRITERION_STRICT_CONTAINMENT.
 *
 * @param ctxt Operation factory context. must not be NULL
 * @param criterion patial criterion to use
 */
void PROJ_DLL proj_operation_factory_context_set_spatial_criterion(
    PJ_OPERATION_FACTORY_CONTEXT *ctxt, PROJ_SPATIAL_CRITERION criterion) {
    assert(ctxt);
    switch (criterion) {
    case PROJ_SPATIAL_CRITERION_STRICT_CONTAINMENT:
        ctxt->operationContext->setSpatialCriterion(
            CoordinateOperationContext::SpatialCriterion::STRICT_CONTAINMENT);
        break;

    case PROJ_SPATIAL_CRITERION_PARTIAL_INTERSECTION:
        ctxt->operationContext->setSpatialCriterion(
            CoordinateOperationContext::SpatialCriterion::PARTIAL_INTERSECTION);
        break;
    }
}

// ---------------------------------------------------------------------------

/** \brief Set how grid availability is used.
 *
 * The default is USE_FOR_SORTING.
 *
 * @param ctxt Operation factory context. must not be NULL
 * @param use how grid availability is used.
 */
void PROJ_DLL proj_operation_factory_context_set_grid_availability_use(
    PJ_OPERATION_FACTORY_CONTEXT *ctxt, PROJ_GRID_AVAILABILITY_USE use) {
    assert(ctxt);
    switch (use) {
    case PROJ_GRID_AVAILABILITY_USED_FOR_SORTING:
        ctxt->operationContext->setGridAvailabilityUse(
            CoordinateOperationContext::GridAvailabilityUse::USE_FOR_SORTING);
        break;

    case PROJ_GRID_AVAILABILITY_DISCARD_OPERATION_IF_MISSING_GRID:
        ctxt->operationContext->setGridAvailabilityUse(
            CoordinateOperationContext::GridAvailabilityUse::
                DISCARD_OPERATION_IF_MISSING_GRID);
        break;

    case PROJ_GRID_AVAILABILITY_IGNORED:
        ctxt->operationContext->setGridAvailabilityUse(
            CoordinateOperationContext::GridAvailabilityUse::
                IGNORE_GRID_AVAILABILITY);
        break;
    }
}

// ---------------------------------------------------------------------------

/** \brief Set whether PROJ alternative grid names should be substituted to
 * the official authority names.
 *
 * The default is true.
 *
 * @param ctxt Operation factory context. must not be NULL
 * @param usePROJNames whether PROJ alternative grid names should be used
 */
void proj_operation_factory_context_set_use_proj_alternative_grid_names(
    PJ_OPERATION_FACTORY_CONTEXT *ctxt, int usePROJNames) {
    assert(ctxt);
    ctxt->operationContext->setUsePROJAlternativeGridNames(usePROJNames != 0);
}

// ---------------------------------------------------------------------------

/** \brief Set whether an intermediate pivot CRS can be used for researching
 * coordinate operations between a source and target CRS.
 *
 * Concretely if in the database there is an operation from A to C
 * (or C to A), and another one from C to B (or B to C), but no direct
 * operation between A and B, setting this parameter to true, allow
 * chaining both operations.
 *
 * The current implementation is limited to researching one intermediate
 * step.
 *
 * By default, all potential C candidates will be used.
 * proj_operation_factory_context_set_allowed_intermediate_crs()
 * can be used to restrict them.
 *
 * The default is true.
 *
 * @param ctxt Operation factory context. must not be NULL
 * @param allow whether intermediate CRS may be used.
 */
void proj_operation_factory_context_set_allow_use_intermediate_crs(
    PJ_OPERATION_FACTORY_CONTEXT *ctxt, int allow) {
    assert(ctxt);
    ctxt->operationContext->setAllowUseIntermediateCRS(allow != 0);
}

// ---------------------------------------------------------------------------

/** \brief Restrict the potential pivot CRSs that can be used when trying to
 * build a coordinate operation between two CRS that have no direct operation.
 *
 * @param ctxt Operation factory context. must not be NULL
 * @param list_of_auth_name_codes an array of strings NLL terminated,
 * with the format { "auth_name1", "code1", "auth_name2", "code2", ... NULL }
 */
void proj_operation_factory_context_set_allowed_intermediate_crs(
    PJ_OPERATION_FACTORY_CONTEXT *ctxt,
    const char *const *list_of_auth_name_codes) {
    assert(ctxt);
    std::vector<std::pair<std::string, std::string>> pivots;
    for (auto iter = list_of_auth_name_codes; iter && iter[0] && iter[1];
         iter += 2) {
        pivots.emplace_back(std::pair<std::string, std::string>(
            std::string(iter[0]), std::string(iter[1])));
    }
    ctxt->operationContext->setIntermediateCRS(pivots);
}

// ---------------------------------------------------------------------------

/** \brief Find a list of CoordinateOperation from source_crs to target_crs.
 *
 * The operations are sorted with the most relevant ones first: by
 * descending
 * area (intersection of the transformation area with the area of interest,
 * or intersection of the transformation with the area of use of the CRS),
 * and
 * by increasing accuracy. Operations with unknown accuracy are sorted last,
 * whatever their area.
 *
 * @param source_crs source CRS. Must not be NULL.
 * @param target_crs source CRS. Must not be NULL.
 * @param operationContext Search context. Must not be NULL.
 * @return a result set that must be unreferenced with
 * proj_obj_list_unref(), or NULL in case of error.
 */
PJ_OBJ_LIST *proj_obj_create_operations(
    const PJ_OBJ *source_crs, const PJ_OBJ *target_crs,
    const PJ_OPERATION_FACTORY_CONTEXT *operationContext) {
    assert(source_crs);
    assert(target_crs);
    assert(operationContext);

    auto sourceCRS = nn_dynamic_pointer_cast<CRS>(source_crs->obj);
    if (!sourceCRS) {
        proj_log_error(operationContext->ctx, __FUNCTION__,
                       "source_crs is not a CRS");
        return nullptr;
    }
    auto targetCRS = nn_dynamic_pointer_cast<CRS>(target_crs->obj);
    if (!targetCRS) {
        proj_log_error(operationContext->ctx, __FUNCTION__,
                       "target_crs is not a CRS");
        return nullptr;
    }

    try {
        auto factory = CoordinateOperationFactory::create();
        std::vector<IdentifiedObjectNNPtr> objects;
        auto ops = factory->createOperations(
            NN_NO_CHECK(sourceCRS), NN_NO_CHECK(targetCRS),
            operationContext->operationContext);
        for (const auto &op : ops) {
            objects.emplace_back(op);
        }
        return new PJ_OBJ_LIST(operationContext->ctx, std::move(objects));
    } catch (const std::exception &e) {
        proj_log_error(operationContext->ctx, __FUNCTION__, e.what());
        return nullptr;
    }
}

// ---------------------------------------------------------------------------

/** \brief Return the number of objects in the result set
 *
 * @param result Objet of type PJ_OBJ_LIST (must not be NULL)
 */
int proj_obj_list_get_count(const PJ_OBJ_LIST *result) {
    assert(result);
    return static_cast<int>(result->objects.size());
}

// ---------------------------------------------------------------------------

/** \brief Return an object from the result set
 *
 * The returned object must be unreferenced with proj_obj_unref() after
 * use.
 * It should be used by at most one thread at a time.
 *
 * @param result Objet of type PJ_OBJ_LIST (must not be NULL)
 * @param index Index
 * @return a new object that must be unreferenced with proj_obj_unref(),
 * or nullptr in case of error.
 */

PJ_OBJ *proj_obj_list_get(const PJ_OBJ_LIST *result, int index) {
    assert(result);
    if (index < 0 || index >= proj_obj_list_get_count(result)) {
        proj_log_error(result->ctx, __FUNCTION__, "Invalid index");
        return nullptr;
    }
    return PJ_OBJ::create(result->ctx, result->objects[index]);
}

// ---------------------------------------------------------------------------

/** \brief Drops a reference on the result set.
 *
 * This method should be called one and exactly one for each function
 * returning a PJ_OBJ_LIST*
 *
 * @param result Object, or NULL.
 */
void proj_obj_list_unref(PJ_OBJ_LIST *result) { delete result; }

// ---------------------------------------------------------------------------

/** \brief Return the accuracy (in metre) of a coordinate operation.
 *
 * @return the accuracy, or a negative value if unknown or in case of error.
 */
double proj_coordoperation_get_accuracy(const PJ_OBJ *coordoperation) {
    assert(coordoperation);
    auto co =
        dynamic_cast<const CoordinateOperation *>(coordoperation->obj.get());
    if (!co) {
        proj_log_error(coordoperation->ctx, __FUNCTION__,
                       "Object is not a CoordinateOperation");
        return -1;
    }
    const auto &accuracies = co->coordinateOperationAccuracies();
    if (accuracies.empty()) {
        return -1;
    }
    try {
        return c_locale_stod(accuracies[0]->value());
    } catch (const std::exception &) {
    }
    return -1;
}

// ---------------------------------------------------------------------------

/** \brief Returns the datum of a SingleCRS.
 *
 * The returned object must be unreferenced with proj_obj_unref() after
 * use.
 * It should be used by at most one thread at a time.
 *
 * @param crs Objet of type SingleCRS (must not be NULL)
 * @return Object that must be unreferenced with proj_obj_unref(), or NULL
 * in case of error (or if there is no datum)
 */
PJ_OBJ *proj_obj_crs_get_datum(const PJ_OBJ *crs) {
    assert(crs);
    auto l_crs = dynamic_cast<const SingleCRS *>(crs->obj.get());
    if (!l_crs) {
        proj_log_error(crs->ctx, __FUNCTION__, "Object is not a SingleCRS");
        return nullptr;
    }
    const auto &datum = l_crs->datum();
    if (!datum) {
        return nullptr;
    }
    return PJ_OBJ::create(crs->ctx, NN_NO_CHECK(datum));
}

// ---------------------------------------------------------------------------

/** \brief Returns the coordinate system of a SingleCRS.
 *
 * The returned object must be unreferenced with proj_obj_unref() after
 * use.
 * It should be used by at most one thread at a time.
 *
 * @param crs Objet of type SingleCRS (must not be NULL)
 * @return Object that must be unreferenced with proj_obj_unref(), or NULL
 * in case of error.
 */
PJ_OBJ *proj_obj_crs_get_coordinate_system(const PJ_OBJ *crs) {
    assert(crs);
    auto l_crs = dynamic_cast<const SingleCRS *>(crs->obj.get());
    if (!l_crs) {
        proj_log_error(crs->ctx, __FUNCTION__, "Object is not a SingleCRS");
        return nullptr;
    }
    return PJ_OBJ::create(crs->ctx, l_crs->coordinateSystem());
}

// ---------------------------------------------------------------------------

/** \brief Returns the type of the coordinate system.
 *
 * @param cs Objet of type CoordinateSystem (must not be NULL)
 * @return type, or PJ_CS_TYPE_UNKNOWN in case of error.
 */
PJ_COORDINATE_SYSTEM_TYPE proj_obj_cs_get_type(const PJ_OBJ *cs) {
    assert(cs);
    auto l_cs = dynamic_cast<const CoordinateSystem *>(cs->obj.get());
    if (!l_cs) {
        proj_log_error(cs->ctx, __FUNCTION__,
                       "Object is not a CoordinateSystem");
        return PJ_CS_TYPE_UNKNOWN;
    }
    if (dynamic_cast<const CartesianCS *>(l_cs)) {
        return PJ_CS_TYPE_CARTESIAN;
    }
    if (dynamic_cast<const EllipsoidalCS *>(l_cs)) {
        return PJ_CS_TYPE_ELLIPSOIDAL;
    }
    if (dynamic_cast<const VerticalCS *>(l_cs)) {
        return PJ_CS_TYPE_VERTICAL;
    }
    if (dynamic_cast<const SphericalCS *>(l_cs)) {
        return PJ_CS_TYPE_SPHERICAL;
    }
    if (dynamic_cast<const OrdinalCS *>(l_cs)) {
        return PJ_CS_TYPE_ORDINAL;
    }
    if (dynamic_cast<const ParametricCS *>(l_cs)) {
        return PJ_CS_TYPE_PARAMETRIC;
    }
    if (dynamic_cast<const DateTimeTemporalCS *>(l_cs)) {
        return PJ_CS_TYPE_DATETIMETEMPORAL;
    }
    if (dynamic_cast<const TemporalCountCS *>(l_cs)) {
        return PJ_CS_TYPE_TEMPORALCOUNT;
    }
    if (dynamic_cast<const TemporalMeasureCS *>(l_cs)) {
        return PJ_CS_TYPE_TEMPORALMEASURE;
    }
    return PJ_CS_TYPE_UNKNOWN;
}

// ---------------------------------------------------------------------------

/** \brief Returns the number of axis of the coordinate system.
 *
 * @param cs Objet of type CoordinateSystem (must not be NULL)
 * @return number of axis, or -1 in case of error.
 */
int proj_obj_cs_get_axis_count(const PJ_OBJ *cs) {
    assert(cs);
    auto l_cs = dynamic_cast<const CoordinateSystem *>(cs->obj.get());
    if (!l_cs) {
        proj_log_error(cs->ctx, __FUNCTION__,
                       "Object is not a CoordinateSystem");
        return -1;
    }
    return static_cast<int>(l_cs->axisList().size());
}

// ---------------------------------------------------------------------------

/** \brief Returns information on an axis
 *
 * @param cs Objet of type CoordinateSystem (must not be NULL)
 * @param index Index of the coordinate system (between 0 and
 * proj_obj_cs_get_axis_count() - 1)
 * @param pName Pointer to a string value to store the axis name. or NULL
 * @param pAbbrev Pointer to a string value to store the axis abbreviation. or
 * NULL
 * @param pDirection Pointer to a string value to store the axis direction. or
 * NULL
 * @param pUnitConvFactor Pointer to a double value to store the axis
 * unit conversion factor. or NULL
 * @param pUnitName Pointer to a string value to store the axis
 * unit name. or NULL
 * @return TRUE in case of success
 */
int proj_obj_cs_get_axis_info(const PJ_OBJ *cs, int index, const char **pName,
                              const char **pAbbrev, const char **pDirection,
                              double *pUnitConvFactor, const char **pUnitName) {
    assert(cs);
    auto l_cs = dynamic_cast<const CoordinateSystem *>(cs->obj.get());
    if (!l_cs) {
        proj_log_error(cs->ctx, __FUNCTION__,
                       "Object is not a CoordinateSystem");
        return false;
    }
    const auto &axisList = l_cs->axisList();
    if (index < 0 || static_cast<size_t>(index) >= axisList.size()) {
        proj_log_error(cs->ctx, __FUNCTION__, "Invalid index");
        return false;
    }
    const auto &axis = axisList[index];
    if (pName) {
        *pName = axis->nameStr().c_str();
    }
    if (pAbbrev) {
        *pAbbrev = axis->abbreviation().c_str();
    }
    if (pDirection) {
        *pDirection = axis->direction().toString().c_str();
    }
    if (pUnitConvFactor) {
        *pUnitConvFactor = axis->unit().conversionToSI();
    }
    if (pUnitName) {
        *pUnitName = axis->unit().name().c_str();
    }
    return true;
}
