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
    std::string lastWKT{};
    std::string lastPROJString{};
    bool gridsNeededAsked = false;
    std::vector<GridDescription> gridsNeeded{};

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
 * @param options should be set to NULL for now
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
        auto identifiedObject = nn_dynamic_pointer_cast<IdentifiedObject>(
            createFromUserInput(text, dbContext));
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
PJ_OBJ_TYPE proj_obj_get_type(PJ_OBJ *obj) {
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
int proj_obj_is_deprecated(PJ_OBJ *obj) {
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
int proj_obj_is_equivalent_to(PJ_OBJ *obj, PJ_OBJ *other,
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
int proj_obj_is_crs(PJ_OBJ *obj) {
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
const char *proj_obj_get_name(PJ_OBJ *obj) {
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
const char *proj_obj_get_id_auth_name(PJ_OBJ *obj, int index) {
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
const char *proj_obj_get_id_code(PJ_OBJ *obj, int index) {
    assert(obj);
    const auto &ids = obj->obj->identifiers();
    if (static_cast<size_t>(index) >= ids.size()) {
        return nullptr;
    }
    return ids[index]->code().c_str();
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
const char *proj_obj_as_wkt(PJ_OBJ *obj, PJ_WKT_TYPE type,
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
const char *proj_obj_as_proj_string(PJ_OBJ *obj, PJ_PROJ_STRING_TYPE type,
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
 * @param p_west_lon Pointer to a double to receive the west longitude (in
 * degrees). Or NULL. If the returned value is -1000, the bounding box is
 * unknown.
 * @param p_south_lat Pointer to a double to receive the south latitude (in
 * degrees). Or NULL. If the returned value is -1000, the bounding box is
 * unknown.
  * @param p_east_lon Pointer to a double to receive the east longitude (in
 * degrees). Or NULL. If the returned value is -1000, the bounding box is
 * unknown.
 * @param p_north_lat Pointer to a double to receive the north latitude (in
 * degrees). Or NULL. If the returned value is -1000, the bounding box is
 * unknown.
 * @param p_area_name Pointer to a string to receive the name of the area of
 * use. Or NULL. *p_area_name is valid while obj is valid itself.
 * @return TRUE in case of success, FALSE in case of error or if the area
 * of use is unknown.
 */
int proj_obj_get_area_of_use(PJ_OBJ *obj, double *p_west_lon,
                             double *p_south_lat, double *p_east_lon,
                             double *p_north_lat, const char **p_area_name) {
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
            if (p_west_lon) {
                *p_west_lon = bbox->westBoundLongitude();
            }
            if (p_south_lat) {
                *p_south_lat = bbox->southBoundLatitude();
            }
            if (p_east_lon) {
                *p_east_lon = bbox->eastBoundLongitude();
            }
            if (p_north_lat) {
                *p_north_lat = bbox->northBoundLatitude();
            }
            return true;
        }
    }
    if (p_west_lon) {
        *p_west_lon = -1000;
    }
    if (p_south_lat) {
        *p_south_lat = -1000;
    }
    if (p_east_lon) {
        *p_east_lon = -1000;
    }
    if (p_north_lat) {
        *p_north_lat = -1000;
    }
    return true;
}

// ---------------------------------------------------------------------------

static const GeodeticCRS *extractGeodeticCRS(PJ_OBJ *crs, const char *fname) {
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
PJ_OBJ *proj_obj_crs_get_geodetic_crs(PJ_OBJ *crs) {
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
PJ_OBJ *proj_obj_crs_get_sub_crs(PJ_OBJ *crs, int index) {
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
PJ_OBJ *proj_obj_crs_create_bound_crs_to_WGS84(PJ_OBJ *crs) {
    assert(crs);
    auto l_crs = dynamic_cast<const CRS *>(crs->obj.get());
    if (!l_crs) {
        proj_log_error(crs->ctx, __FUNCTION__, "Object is not a CRS");
        return nullptr;
    }
    auto dbContext = getDBcontextNoException(crs->ctx, __FUNCTION__);
    return PJ_OBJ::create(crs->ctx,
                          l_crs->createBoundCRSToWGS84IfPossible(dbContext));
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
PJ_OBJ *proj_obj_get_ellipsoid(PJ_OBJ *obj) {
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
PJ_OBJ *proj_obj_crs_get_horizontal_datum(PJ_OBJ *crs) {
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
int proj_obj_ellipsoid_get_parameters(PJ_OBJ *ellipsoid,
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

PJ_OBJ *proj_obj_get_prime_meridian(PJ_OBJ *obj) {
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
int proj_obj_prime_meridian_get_parameters(PJ_OBJ *prime_meridian,
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

/** \brief Return the base CRS of a BoundCRS or the source CRS of a
 * CoordinateOperation.
 *
 * The returned object must be unreferenced with proj_obj_unref() after
 * use.
 * It should be used by at most one thread at a time.
 *
 * @param obj Objet of type BoundCRS or CoordinateOperation (must not be NULL)
 * @return Object that must be unreferenced with proj_obj_unref(), or NULL
 * in case of error, or missing source CRS.
 */
PJ_OBJ *proj_obj_get_source_crs(PJ_OBJ *obj) {
    assert(obj);
    auto ptr = obj->obj.get();
    auto boundCRS = dynamic_cast<const BoundCRS *>(ptr);
    if (boundCRS) {
        return PJ_OBJ::create(obj->ctx, boundCRS->baseCRS());
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
PJ_OBJ *proj_obj_get_target_crs(PJ_OBJ *obj) {
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
PJ_OBJ_LIST *proj_obj_identify(PJ_OBJ *obj, const char *auth_name,
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
PJ_OBJ *proj_obj_crs_get_coordoperation(PJ_OBJ *crs, const char **pMethodName,
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
    return name == nullptr ? UnitOfMeasure::METRE
                           : UnitOfMeasure(name, convFactor);
}

// ---------------------------------------------------------------------------

static UnitOfMeasure createAngularUnit(const char *name, double convFactor) {
    return name ? (ci_equal(name, "degree")
                       ? UnitOfMeasure::DEGREE
                       : ci_equal(name, "grad")
                             ? UnitOfMeasure::GRAD
                             : UnitOfMeasure(name, convFactor))
                : UnitOfMeasure::DEGREE;
}
//! @endcond

// ---------------------------------------------------------------------------

/** \brief Create a GeographicCRS 2D from its definition.
 *
 * The returned object must be unreferenced with proj_obj_unref() after
 * use.
 * It should be used by at most one thread at a time.
 *
 * @param ctx PROJ context, or NULL for default context
 * @param geogName Name of the GeographicCRS. Or NULL
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
 * @param latLongOrder TRUE for Latitude Longitude axis order.
 *
 * @return Object of type GeographicCRS that must be unreferenced with
 * proj_obj_unref(), or NULL in case of error.
 */
PJ_OBJ *proj_obj_create_geographic_crs(
    PJ_CONTEXT *ctx, const char *geogName, const char *datumName,
    const char *ellipsoidName, double semiMajorMetre, double invFlattening,
    const char *primeMeridianName, double primeMeridianOffset,
    const char *angularUnits, double angularUnitsConv, int latLongOrder) {

    SANITIZE_CTX(ctx);
    try {
        const UnitOfMeasure angUnit(
            createAngularUnit(angularUnits, angularUnitsConv));
        auto dbContext = getDBcontext(ctx);
        auto body = Ellipsoid::guessBodyName(dbContext, semiMajorMetre);
        auto ellpsName = createPropertyMapName(ellipsoidName);
        auto ellps =
            invFlattening != 0.0
                ? Ellipsoid::createFlattenedSphere(ellpsName,
                                                   Length(semiMajorMetre),
                                                   Scale(invFlattening), body)
                : Ellipsoid::createSphere(ellpsName, Length(semiMajorMetre),
                                          body);
        auto pm = PrimeMeridian::create(
            PropertyMap().set(
                common::IdentifiedObject::NAME_KEY,
                primeMeridianName
                    ? primeMeridianName
                    : primeMeridianOffset == 0.0
                          ? (ellps->celestialBody() == Ellipsoid::EARTH
                                 ? "Greenwich"
                                 : "Reference meridian")
                          : "unnamed"),
            Angle(primeMeridianOffset, angUnit));
        auto datum = GeodeticReferenceFrame::create(
            createPropertyMapName(datumName), ellps,
            util::optional<std::string>(), pm);
        auto geogCRS = GeographicCRS::create(
            createPropertyMapName(geogName), datum,
            latLongOrder ? cs::EllipsoidalCS::createLatitudeLongitude(angUnit)
                         : cs::EllipsoidalCS::createLongitudeLatitude(angUnit));
        return PJ_OBJ::create(ctx, geogCRS);
    } catch (const std::exception &e) {
        proj_log_error(ctx, __FUNCTION__, e.what());
    }
    return nullptr;
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress

static PJ_OBJ *proj_obj_create_projected_crs(PJ_OBJ *geodetic_crs,
                                             const char *crs_name,
                                             const ConversionNNPtr &conv,
                                             const UnitOfMeasure &linearUnit) {
    assert(geodetic_crs);
    auto geogCRS =
        util::nn_dynamic_pointer_cast<GeodeticCRS>(geodetic_crs->obj);
    if (!geogCRS) {
        return nullptr;
    }
    auto crs = ProjectedCRS::create(
        createPropertyMapName(crs_name), NN_NO_CHECK(geogCRS), conv,
        cs::CartesianCS::createEastingNorthing(linearUnit));
    return PJ_OBJ::create(geodetic_crs->ctx, crs);
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
PJ_OBJ *proj_obj_create_projected_crs_UTM(PJ_OBJ *geodetic_crs,
                                          const char *crs_name, int zone,
                                          int north) {
    const auto &linearUnit = UnitOfMeasure::METRE;
    auto conv = Conversion::createUTM(PropertyMap(), zone, north != 0);
    return proj_obj_create_projected_crs(geodetic_crs, crs_name, conv,
                                         linearUnit);
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
PJ_OBJ *proj_obj_create_projected_crs_TransverseMercator(
    PJ_OBJ *geodetic_crs, const char *crs_name, double centerLat,
    double centerLong, double scale, double falseEasting, double falseNorthing,
    const char *angUnitName, double angUnitConvFactor,
    const char *linearUnitName, double linearUnitConvFactor) {
    UnitOfMeasure linearUnit(
        createLinearUnit(linearUnitName, linearUnitConvFactor));
    UnitOfMeasure angUnit(createAngularUnit(angUnitName, angUnitConvFactor));
    auto conv = Conversion::createTransverseMercator(
        PropertyMap(), Angle(centerLat, angUnit), Angle(centerLong, angUnit),
        Scale(scale), Length(falseEasting, linearUnit),
        Length(falseNorthing, linearUnit));
    return proj_obj_create_projected_crs(geodetic_crs, crs_name, conv,
                                         linearUnit);
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
PJ_OBJ *proj_obj_create_projected_crs_GaussSchreiberTransverseMercator(
    PJ_OBJ *geodetic_crs, const char *crs_name, double centerLat,
    double centerLong, double scale, double falseEasting, double falseNorthing,
    const char *angUnitName, double angUnitConvFactor,
    const char *linearUnitName, double linearUnitConvFactor) {
    UnitOfMeasure linearUnit(
        createLinearUnit(linearUnitName, linearUnitConvFactor));
    UnitOfMeasure angUnit(createAngularUnit(angUnitName, angUnitConvFactor));
    auto conv = Conversion::createGaussSchreiberTransverseMercator(
        PropertyMap(), Angle(centerLat, angUnit), Angle(centerLong, angUnit),
        Scale(scale), Length(falseEasting, linearUnit),
        Length(falseNorthing, linearUnit));
    return proj_obj_create_projected_crs(geodetic_crs, crs_name, conv,
                                         linearUnit);
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
PJ_OBJ *proj_obj_create_projected_crs_TransverseMercatorSouthOriented(
    PJ_OBJ *geodetic_crs, const char *crs_name, double centerLat,
    double centerLong, double scale, double falseEasting, double falseNorthing,
    const char *angUnitName, double angUnitConvFactor,
    const char *linearUnitName, double linearUnitConvFactor) {
    UnitOfMeasure linearUnit(
        createLinearUnit(linearUnitName, linearUnitConvFactor));
    UnitOfMeasure angUnit(createAngularUnit(angUnitName, angUnitConvFactor));
    auto conv = Conversion::createTransverseMercatorSouthOriented(
        PropertyMap(), Angle(centerLat, angUnit), Angle(centerLong, angUnit),
        Scale(scale), Length(falseEasting, linearUnit),
        Length(falseNorthing, linearUnit));
    return proj_obj_create_projected_crs(geodetic_crs, crs_name, conv,
                                         linearUnit);
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
PJ_OBJ *proj_obj_create_projected_crs_TwoPointEquidistant(
    PJ_OBJ *geodetic_crs, const char *crs_name, double latitudeFirstPoint,
    double longitudeFirstPoint, double latitudeSecondPoint,
    double longitudeSeconPoint, double falseEasting, double falseNorthing,
    const char *angUnitName, double angUnitConvFactor,
    const char *linearUnitName, double linearUnitConvFactor) {
    UnitOfMeasure linearUnit(
        createLinearUnit(linearUnitName, linearUnitConvFactor));
    UnitOfMeasure angUnit(createAngularUnit(angUnitName, angUnitConvFactor));
    auto conv = Conversion::createTwoPointEquidistant(
        PropertyMap(), Angle(latitudeFirstPoint, angUnit),
        Angle(longitudeFirstPoint, angUnit),
        Angle(latitudeSecondPoint, angUnit),
        Angle(longitudeSeconPoint, angUnit), Length(falseEasting, linearUnit),
        Length(falseNorthing, linearUnit));
    return proj_obj_create_projected_crs(geodetic_crs, crs_name, conv,
                                         linearUnit);
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
PJ_OBJ *proj_obj_create_projected_crs_TunisiaMappingGrid(
    PJ_OBJ *geodetic_crs, const char *crs_name, double centerLat,
    double centerLong, double falseEasting, double falseNorthing,
    const char *angUnitName, double angUnitConvFactor,
    const char *linearUnitName, double linearUnitConvFactor) {
    UnitOfMeasure linearUnit(
        createLinearUnit(linearUnitName, linearUnitConvFactor));
    UnitOfMeasure angUnit(createAngularUnit(angUnitName, angUnitConvFactor));
    auto conv = Conversion::createTunisiaMappingGrid(
        PropertyMap(), Angle(centerLat, angUnit), Angle(centerLong, angUnit),
        Length(falseEasting, linearUnit), Length(falseNorthing, linearUnit));
    return proj_obj_create_projected_crs(geodetic_crs, crs_name, conv,
                                         linearUnit);
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
PJ_OBJ *proj_obj_create_projected_crs_AlbersEqualArea(
    PJ_OBJ *geodetic_crs, const char *crs_name, double latitudeFalseOrigin,
    double longitudeFalseOrigin, double latitudeFirstParallel,
    double latitudeSecondParallel, double eastingFalseOrigin,
    double northingFalseOrigin, const char *angUnitName,
    double angUnitConvFactor, const char *linearUnitName,
    double linearUnitConvFactor) {
    UnitOfMeasure linearUnit(
        createLinearUnit(linearUnitName, linearUnitConvFactor));
    UnitOfMeasure angUnit(createAngularUnit(angUnitName, angUnitConvFactor));
    auto conv = Conversion::createAlbersEqualArea(
        PropertyMap(), Angle(latitudeFalseOrigin, angUnit),
        Angle(longitudeFalseOrigin, angUnit),
        Angle(latitudeFirstParallel, angUnit),
        Angle(latitudeSecondParallel, angUnit),
        Length(eastingFalseOrigin, linearUnit),
        Length(northingFalseOrigin, linearUnit));
    return proj_obj_create_projected_crs(geodetic_crs, crs_name, conv,
                                         linearUnit);
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
PJ_OBJ *proj_obj_create_projected_crs_LambertConicConformal_1SP(
    PJ_OBJ *geodetic_crs, const char *crs_name, double centerLat,
    double centerLong, double scale, double falseEasting, double falseNorthing,
    const char *angUnitName, double angUnitConvFactor,
    const char *linearUnitName, double linearUnitConvFactor) {
    UnitOfMeasure linearUnit(
        createLinearUnit(linearUnitName, linearUnitConvFactor));
    UnitOfMeasure angUnit(createAngularUnit(angUnitName, angUnitConvFactor));
    auto conv = Conversion::createLambertConicConformal_1SP(
        PropertyMap(), Angle(centerLat, angUnit), Angle(centerLong, angUnit),
        Scale(scale), Length(falseEasting, linearUnit),
        Length(falseNorthing, linearUnit));
    return proj_obj_create_projected_crs(geodetic_crs, crs_name, conv,
                                         linearUnit);
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
PJ_OBJ *proj_obj_create_projected_crs_LambertConicConformal_2SP(
    PJ_OBJ *geodetic_crs, const char *crs_name, double latitudeFalseOrigin,
    double longitudeFalseOrigin, double latitudeFirstParallel,
    double latitudeSecondParallel, double eastingFalseOrigin,
    double northingFalseOrigin, const char *angUnitName,
    double angUnitConvFactor, const char *linearUnitName,
    double linearUnitConvFactor) {
    UnitOfMeasure linearUnit(
        createLinearUnit(linearUnitName, linearUnitConvFactor));
    UnitOfMeasure angUnit(createAngularUnit(angUnitName, angUnitConvFactor));
    auto conv = Conversion::createLambertConicConformal_2SP(
        PropertyMap(), Angle(latitudeFalseOrigin, angUnit),
        Angle(longitudeFalseOrigin, angUnit),
        Angle(latitudeFirstParallel, angUnit),
        Angle(latitudeSecondParallel, angUnit),
        Length(eastingFalseOrigin, linearUnit),
        Length(northingFalseOrigin, linearUnit));
    return proj_obj_create_projected_crs(geodetic_crs, crs_name, conv,
                                         linearUnit);
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
PJ_OBJ *proj_obj_create_projected_crs_LambertConicConformal_2SP_Michigan(
    PJ_OBJ *geodetic_crs, const char *crs_name, double latitudeFalseOrigin,
    double longitudeFalseOrigin, double latitudeFirstParallel,
    double latitudeSecondParallel, double eastingFalseOrigin,
    double northingFalseOrigin, double ellipsoidScalingFactor,
    const char *angUnitName, double angUnitConvFactor,
    const char *linearUnitName, double linearUnitConvFactor) {
    UnitOfMeasure linearUnit(
        createLinearUnit(linearUnitName, linearUnitConvFactor));
    UnitOfMeasure angUnit(createAngularUnit(angUnitName, angUnitConvFactor));
    auto conv = Conversion::createLambertConicConformal_2SP_Michigan(
        PropertyMap(), Angle(latitudeFalseOrigin, angUnit),
        Angle(longitudeFalseOrigin, angUnit),
        Angle(latitudeFirstParallel, angUnit),
        Angle(latitudeSecondParallel, angUnit),
        Length(eastingFalseOrigin, linearUnit),
        Length(northingFalseOrigin, linearUnit), Scale(ellipsoidScalingFactor));
    return proj_obj_create_projected_crs(geodetic_crs, crs_name, conv,
                                         linearUnit);
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
PJ_OBJ *proj_obj_create_projected_crs_LambertConicConformal_2SP_Belgium(
    PJ_OBJ *geodetic_crs, const char *crs_name, double latitudeFalseOrigin,
    double longitudeFalseOrigin, double latitudeFirstParallel,
    double latitudeSecondParallel, double eastingFalseOrigin,
    double northingFalseOrigin, const char *angUnitName,
    double angUnitConvFactor, const char *linearUnitName,
    double linearUnitConvFactor) {
    UnitOfMeasure linearUnit(
        createLinearUnit(linearUnitName, linearUnitConvFactor));
    UnitOfMeasure angUnit(createAngularUnit(angUnitName, angUnitConvFactor));
    auto conv = Conversion::createLambertConicConformal_2SP_Belgium(
        PropertyMap(), Angle(latitudeFalseOrigin, angUnit),
        Angle(longitudeFalseOrigin, angUnit),
        Angle(latitudeFirstParallel, angUnit),
        Angle(latitudeSecondParallel, angUnit),
        Length(eastingFalseOrigin, linearUnit),
        Length(northingFalseOrigin, linearUnit));
    return proj_obj_create_projected_crs(geodetic_crs, crs_name, conv,
                                         linearUnit);
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
PJ_OBJ *proj_obj_create_projected_crs_AzimuthalEquidistant(
    PJ_OBJ *geodetic_crs, const char *crs_name, double latitudeNatOrigin,
    double longitudeNatOrigin, double falseEasting, double falseNorthing,
    const char *angUnitName, double angUnitConvFactor,
    const char *linearUnitName, double linearUnitConvFactor) {
    UnitOfMeasure linearUnit(
        createLinearUnit(linearUnitName, linearUnitConvFactor));
    UnitOfMeasure angUnit(createAngularUnit(angUnitName, angUnitConvFactor));
    auto conv = Conversion::createAzimuthalEquidistant(
        PropertyMap(), Angle(latitudeNatOrigin, angUnit),
        Angle(longitudeNatOrigin, angUnit), Length(falseEasting, linearUnit),
        Length(falseNorthing, linearUnit));
    return proj_obj_create_projected_crs(geodetic_crs, crs_name, conv,
                                         linearUnit);
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
PJ_OBJ *proj_obj_create_projected_crs_GuamProjection(
    PJ_OBJ *geodetic_crs, const char *crs_name, double latitudeNatOrigin,
    double longitudeNatOrigin, double falseEasting, double falseNorthing,
    const char *angUnitName, double angUnitConvFactor,
    const char *linearUnitName, double linearUnitConvFactor) {
    UnitOfMeasure linearUnit(
        createLinearUnit(linearUnitName, linearUnitConvFactor));
    UnitOfMeasure angUnit(createAngularUnit(angUnitName, angUnitConvFactor));
    auto conv = Conversion::createGuamProjection(
        PropertyMap(), Angle(latitudeNatOrigin, angUnit),
        Angle(longitudeNatOrigin, angUnit), Length(falseEasting, linearUnit),
        Length(falseNorthing, linearUnit));
    return proj_obj_create_projected_crs(geodetic_crs, crs_name, conv,
                                         linearUnit);
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
PJ_OBJ *proj_obj_create_projected_crs_Bonne(
    PJ_OBJ *geodetic_crs, const char *crs_name, double latitudeNatOrigin,
    double longitudeNatOrigin, double falseEasting, double falseNorthing,
    const char *angUnitName, double angUnitConvFactor,
    const char *linearUnitName, double linearUnitConvFactor) {
    UnitOfMeasure linearUnit(
        createLinearUnit(linearUnitName, linearUnitConvFactor));
    UnitOfMeasure angUnit(createAngularUnit(angUnitName, angUnitConvFactor));
    auto conv = Conversion::createBonne(
        PropertyMap(), Angle(latitudeNatOrigin, angUnit),
        Angle(longitudeNatOrigin, angUnit), Length(falseEasting, linearUnit),
        Length(falseNorthing, linearUnit));
    return proj_obj_create_projected_crs(geodetic_crs, crs_name, conv,
                                         linearUnit);
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
PJ_OBJ *proj_obj_create_projected_crs_LambertCylindricalEqualAreaSpherical(
    PJ_OBJ *geodetic_crs, const char *crs_name, double latitudeFirstParallel,
    double longitudeNatOrigin, double falseEasting, double falseNorthing,
    const char *angUnitName, double angUnitConvFactor,
    const char *linearUnitName, double linearUnitConvFactor) {
    UnitOfMeasure linearUnit(
        createLinearUnit(linearUnitName, linearUnitConvFactor));
    UnitOfMeasure angUnit(createAngularUnit(angUnitName, angUnitConvFactor));
    auto conv = Conversion::createLambertCylindricalEqualAreaSpherical(
        PropertyMap(), Angle(latitudeFirstParallel, angUnit),
        Angle(longitudeNatOrigin, angUnit), Length(falseEasting, linearUnit),
        Length(falseNorthing, linearUnit));
    return proj_obj_create_projected_crs(geodetic_crs, crs_name, conv,
                                         linearUnit);
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
PJ_OBJ *proj_obj_create_projected_crs_LambertCylindricalEqualArea(
    PJ_OBJ *geodetic_crs, const char *crs_name, double latitudeFirstParallel,
    double longitudeNatOrigin, double falseEasting, double falseNorthing,
    const char *angUnitName, double angUnitConvFactor,
    const char *linearUnitName, double linearUnitConvFactor) {
    UnitOfMeasure linearUnit(
        createLinearUnit(linearUnitName, linearUnitConvFactor));
    UnitOfMeasure angUnit(createAngularUnit(angUnitName, angUnitConvFactor));
    auto conv = Conversion::createLambertCylindricalEqualArea(
        PropertyMap(), Angle(latitudeFirstParallel, angUnit),
        Angle(longitudeNatOrigin, angUnit), Length(falseEasting, linearUnit),
        Length(falseNorthing, linearUnit));
    return proj_obj_create_projected_crs(geodetic_crs, crs_name, conv,
                                         linearUnit);
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
PJ_OBJ *proj_obj_create_projected_crs_CassiniSoldner(
    PJ_OBJ *geodetic_crs, const char *crs_name, double centerLat,
    double centerLong, double falseEasting, double falseNorthing,
    const char *angUnitName, double angUnitConvFactor,
    const char *linearUnitName, double linearUnitConvFactor) {
    UnitOfMeasure linearUnit(
        createLinearUnit(linearUnitName, linearUnitConvFactor));
    UnitOfMeasure angUnit(createAngularUnit(angUnitName, angUnitConvFactor));
    auto conv = Conversion::createCassiniSoldner(
        PropertyMap(), Angle(centerLat, angUnit), Angle(centerLong, angUnit),
        Length(falseEasting, linearUnit), Length(falseNorthing, linearUnit));
    return proj_obj_create_projected_crs(geodetic_crs, crs_name, conv,
                                         linearUnit);
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
PJ_OBJ *proj_obj_create_projected_crs_EquidistantConic(
    PJ_OBJ *geodetic_crs, const char *crs_name, double centerLat,
    double centerLong, double latitudeFirstParallel,
    double latitudeSecondParallel, double falseEasting, double falseNorthing,
    const char *angUnitName, double angUnitConvFactor,
    const char *linearUnitName, double linearUnitConvFactor) {
    UnitOfMeasure linearUnit(
        createLinearUnit(linearUnitName, linearUnitConvFactor));
    UnitOfMeasure angUnit(createAngularUnit(angUnitName, angUnitConvFactor));
    auto conv = Conversion::createEquidistantConic(
        PropertyMap(), Angle(centerLat, angUnit), Angle(centerLong, angUnit),
        Angle(latitudeFirstParallel, angUnit),
        Angle(latitudeSecondParallel, angUnit),
        Length(falseEasting, linearUnit), Length(falseNorthing, linearUnit));
    return proj_obj_create_projected_crs(geodetic_crs, crs_name, conv,
                                         linearUnit);
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
PJ_OBJ *proj_obj_create_projected_crs_EckertI(
    PJ_OBJ *geodetic_crs, const char *crs_name, double centerLong,
    double falseEasting, double falseNorthing, const char *angUnitName,
    double angUnitConvFactor, const char *linearUnitName,
    double linearUnitConvFactor) {
    UnitOfMeasure linearUnit(
        createLinearUnit(linearUnitName, linearUnitConvFactor));
    UnitOfMeasure angUnit(createAngularUnit(angUnitName, angUnitConvFactor));
    auto conv = Conversion::createEckertI(
        PropertyMap(), Angle(centerLong, angUnit),
        Length(falseEasting, linearUnit), Length(falseNorthing, linearUnit));
    return proj_obj_create_projected_crs(geodetic_crs, crs_name, conv,
                                         linearUnit);
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
PJ_OBJ *proj_obj_create_projected_crs_EckertII(
    PJ_OBJ *geodetic_crs, const char *crs_name, double centerLong,
    double falseEasting, double falseNorthing, const char *angUnitName,
    double angUnitConvFactor, const char *linearUnitName,
    double linearUnitConvFactor) {
    UnitOfMeasure linearUnit(
        createLinearUnit(linearUnitName, linearUnitConvFactor));
    UnitOfMeasure angUnit(createAngularUnit(angUnitName, angUnitConvFactor));
    auto conv = Conversion::createEckertII(
        PropertyMap(), Angle(centerLong, angUnit),
        Length(falseEasting, linearUnit), Length(falseNorthing, linearUnit));
    return proj_obj_create_projected_crs(geodetic_crs, crs_name, conv,
                                         linearUnit);
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
PJ_OBJ *proj_obj_create_projected_crs_EckertIII(
    PJ_OBJ *geodetic_crs, const char *crs_name, double centerLong,
    double falseEasting, double falseNorthing, const char *angUnitName,
    double angUnitConvFactor, const char *linearUnitName,
    double linearUnitConvFactor) {
    UnitOfMeasure linearUnit(
        createLinearUnit(linearUnitName, linearUnitConvFactor));
    UnitOfMeasure angUnit(createAngularUnit(angUnitName, angUnitConvFactor));
    auto conv = Conversion::createEckertIII(
        PropertyMap(), Angle(centerLong, angUnit),
        Length(falseEasting, linearUnit), Length(falseNorthing, linearUnit));
    return proj_obj_create_projected_crs(geodetic_crs, crs_name, conv,
                                         linearUnit);
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
PJ_OBJ *proj_obj_create_projected_crs_EckertIV(
    PJ_OBJ *geodetic_crs, const char *crs_name, double centerLong,
    double falseEasting, double falseNorthing, const char *angUnitName,
    double angUnitConvFactor, const char *linearUnitName,
    double linearUnitConvFactor) {
    UnitOfMeasure linearUnit(
        createLinearUnit(linearUnitName, linearUnitConvFactor));
    UnitOfMeasure angUnit(createAngularUnit(angUnitName, angUnitConvFactor));
    auto conv = Conversion::createEckertIV(
        PropertyMap(), Angle(centerLong, angUnit),
        Length(falseEasting, linearUnit), Length(falseNorthing, linearUnit));
    return proj_obj_create_projected_crs(geodetic_crs, crs_name, conv,
                                         linearUnit);
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
PJ_OBJ *proj_obj_create_projected_crs_EckertV(
    PJ_OBJ *geodetic_crs, const char *crs_name, double centerLong,
    double falseEasting, double falseNorthing, const char *angUnitName,
    double angUnitConvFactor, const char *linearUnitName,
    double linearUnitConvFactor) {
    UnitOfMeasure linearUnit(
        createLinearUnit(linearUnitName, linearUnitConvFactor));
    UnitOfMeasure angUnit(createAngularUnit(angUnitName, angUnitConvFactor));
    auto conv = Conversion::createEckertV(
        PropertyMap(), Angle(centerLong, angUnit),
        Length(falseEasting, linearUnit), Length(falseNorthing, linearUnit));
    return proj_obj_create_projected_crs(geodetic_crs, crs_name, conv,
                                         linearUnit);
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
PJ_OBJ *proj_obj_create_projected_crs_EckertVI(
    PJ_OBJ *geodetic_crs, const char *crs_name, double centerLong,
    double falseEasting, double falseNorthing, const char *angUnitName,
    double angUnitConvFactor, const char *linearUnitName,
    double linearUnitConvFactor) {
    UnitOfMeasure linearUnit(
        createLinearUnit(linearUnitName, linearUnitConvFactor));
    UnitOfMeasure angUnit(createAngularUnit(angUnitName, angUnitConvFactor));
    auto conv = Conversion::createEckertVI(
        PropertyMap(), Angle(centerLong, angUnit),
        Length(falseEasting, linearUnit), Length(falseNorthing, linearUnit));
    return proj_obj_create_projected_crs(geodetic_crs, crs_name, conv,
                                         linearUnit);
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
PJ_OBJ *proj_obj_create_projected_crs_EquidistantCylindrical(
    PJ_OBJ *geodetic_crs, const char *crs_name, double latitudeFirstParallel,
    double longitudeNatOrigin, double falseEasting, double falseNorthing,
    const char *angUnitName, double angUnitConvFactor,
    const char *linearUnitName, double linearUnitConvFactor) {
    UnitOfMeasure linearUnit(
        createLinearUnit(linearUnitName, linearUnitConvFactor));
    UnitOfMeasure angUnit(createAngularUnit(angUnitName, angUnitConvFactor));
    auto conv = Conversion::createEquidistantCylindrical(
        PropertyMap(), Angle(latitudeFirstParallel, angUnit),
        Angle(longitudeNatOrigin, angUnit), Length(falseEasting, linearUnit),
        Length(falseNorthing, linearUnit));
    return proj_obj_create_projected_crs(geodetic_crs, crs_name, conv,
                                         linearUnit);
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
PJ_OBJ *proj_obj_create_projected_crs_EquidistantCylindricalSpherical(
    PJ_OBJ *geodetic_crs, const char *crs_name, double latitudeFirstParallel,
    double longitudeNatOrigin, double falseEasting, double falseNorthing,
    const char *angUnitName, double angUnitConvFactor,
    const char *linearUnitName, double linearUnitConvFactor) {
    UnitOfMeasure linearUnit(
        createLinearUnit(linearUnitName, linearUnitConvFactor));
    UnitOfMeasure angUnit(createAngularUnit(angUnitName, angUnitConvFactor));
    auto conv = Conversion::createEquidistantCylindricalSpherical(
        PropertyMap(), Angle(latitudeFirstParallel, angUnit),
        Angle(longitudeNatOrigin, angUnit), Length(falseEasting, linearUnit),
        Length(falseNorthing, linearUnit));
    return proj_obj_create_projected_crs(geodetic_crs, crs_name, conv,
                                         linearUnit);
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
PJ_OBJ *proj_obj_create_projected_crs_Gall(
    PJ_OBJ *geodetic_crs, const char *crs_name, double centerLong,
    double falseEasting, double falseNorthing, const char *angUnitName,
    double angUnitConvFactor, const char *linearUnitName,
    double linearUnitConvFactor) {
    UnitOfMeasure linearUnit(
        createLinearUnit(linearUnitName, linearUnitConvFactor));
    UnitOfMeasure angUnit(createAngularUnit(angUnitName, angUnitConvFactor));
    auto conv = Conversion::createGall(
        PropertyMap(), Angle(centerLong, angUnit),
        Length(falseEasting, linearUnit), Length(falseNorthing, linearUnit));
    return proj_obj_create_projected_crs(geodetic_crs, crs_name, conv,
                                         linearUnit);
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
PJ_OBJ *proj_obj_create_projected_crs_GoodeHomolosine(
    PJ_OBJ *geodetic_crs, const char *crs_name, double centerLong,
    double falseEasting, double falseNorthing, const char *angUnitName,
    double angUnitConvFactor, const char *linearUnitName,
    double linearUnitConvFactor) {
    UnitOfMeasure linearUnit(
        createLinearUnit(linearUnitName, linearUnitConvFactor));
    UnitOfMeasure angUnit(createAngularUnit(angUnitName, angUnitConvFactor));
    auto conv = Conversion::createGoodeHomolosine(
        PropertyMap(), Angle(centerLong, angUnit),
        Length(falseEasting, linearUnit), Length(falseNorthing, linearUnit));
    return proj_obj_create_projected_crs(geodetic_crs, crs_name, conv,
                                         linearUnit);
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
PJ_OBJ *proj_obj_create_projected_crs_InterruptedGoodeHomolosine(
    PJ_OBJ *geodetic_crs, const char *crs_name, double centerLong,
    double falseEasting, double falseNorthing, const char *angUnitName,
    double angUnitConvFactor, const char *linearUnitName,
    double linearUnitConvFactor) {
    UnitOfMeasure linearUnit(
        createLinearUnit(linearUnitName, linearUnitConvFactor));
    UnitOfMeasure angUnit(createAngularUnit(angUnitName, angUnitConvFactor));
    auto conv = Conversion::createInterruptedGoodeHomolosine(
        PropertyMap(), Angle(centerLong, angUnit),
        Length(falseEasting, linearUnit), Length(falseNorthing, linearUnit));
    return proj_obj_create_projected_crs(geodetic_crs, crs_name, conv,
                                         linearUnit);
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
PJ_OBJ *proj_obj_create_projected_crs_GeostationarySatelliteSweepX(
    PJ_OBJ *geodetic_crs, const char *crs_name, double centerLong,
    double height, double falseEasting, double falseNorthing,
    const char *angUnitName, double angUnitConvFactor,
    const char *linearUnitName, double linearUnitConvFactor) {
    UnitOfMeasure linearUnit(
        createLinearUnit(linearUnitName, linearUnitConvFactor));
    UnitOfMeasure angUnit(createAngularUnit(angUnitName, angUnitConvFactor));
    auto conv = Conversion::createGeostationarySatelliteSweepX(
        PropertyMap(), Angle(centerLong, angUnit), Length(height, linearUnit),
        Length(falseEasting, linearUnit), Length(falseNorthing, linearUnit));
    return proj_obj_create_projected_crs(geodetic_crs, crs_name, conv,
                                         linearUnit);
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
PJ_OBJ *proj_obj_create_projected_crs_GeostationarySatelliteSweepY(
    PJ_OBJ *geodetic_crs, const char *crs_name, double centerLong,
    double height, double falseEasting, double falseNorthing,
    const char *angUnitName, double angUnitConvFactor,
    const char *linearUnitName, double linearUnitConvFactor) {
    UnitOfMeasure linearUnit(
        createLinearUnit(linearUnitName, linearUnitConvFactor));
    UnitOfMeasure angUnit(createAngularUnit(angUnitName, angUnitConvFactor));
    auto conv = Conversion::createGeostationarySatelliteSweepY(
        PropertyMap(), Angle(centerLong, angUnit), Length(height, linearUnit),
        Length(falseEasting, linearUnit), Length(falseNorthing, linearUnit));
    return proj_obj_create_projected_crs(geodetic_crs, crs_name, conv,
                                         linearUnit);
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
PJ_OBJ *proj_obj_create_projected_crs_Gnomonic(
    PJ_OBJ *geodetic_crs, const char *crs_name, double centerLat,
    double centerLong, double falseEasting, double falseNorthing,
    const char *angUnitName, double angUnitConvFactor,
    const char *linearUnitName, double linearUnitConvFactor) {
    UnitOfMeasure linearUnit(
        createLinearUnit(linearUnitName, linearUnitConvFactor));
    UnitOfMeasure angUnit(createAngularUnit(angUnitName, angUnitConvFactor));
    auto conv = Conversion::createGnomonic(
        PropertyMap(), Angle(centerLat, angUnit), Angle(centerLong, angUnit),
        Length(falseEasting, linearUnit), Length(falseNorthing, linearUnit));
    return proj_obj_create_projected_crs(geodetic_crs, crs_name, conv,
                                         linearUnit);
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
PJ_OBJ *proj_obj_create_projected_crs_HotineObliqueMercatorVariantA(
    PJ_OBJ *geodetic_crs, const char *crs_name, double latitudeProjectionCentre,
    double longitudeProjectionCentre, double azimuthInitialLine,
    double angleFromRectifiedToSkrewGrid, double scale, double falseEasting,
    double falseNorthing, const char *angUnitName, double angUnitConvFactor,
    const char *linearUnitName, double linearUnitConvFactor) {
    UnitOfMeasure linearUnit(
        createLinearUnit(linearUnitName, linearUnitConvFactor));
    UnitOfMeasure angUnit(createAngularUnit(angUnitName, angUnitConvFactor));
    auto conv = Conversion::createHotineObliqueMercatorVariantA(
        PropertyMap(), Angle(latitudeProjectionCentre, angUnit),
        Angle(longitudeProjectionCentre, angUnit),
        Angle(azimuthInitialLine, angUnit),
        Angle(angleFromRectifiedToSkrewGrid, angUnit), Scale(scale),
        Length(falseEasting, linearUnit), Length(falseNorthing, linearUnit));
    return proj_obj_create_projected_crs(geodetic_crs, crs_name, conv,
                                         linearUnit);
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
PJ_OBJ *proj_obj_create_projected_crs_HotineObliqueMercatorVariantB(
    PJ_OBJ *geodetic_crs, const char *crs_name, double latitudeProjectionCentre,
    double longitudeProjectionCentre, double azimuthInitialLine,
    double angleFromRectifiedToSkrewGrid, double scale,
    double eastingProjectionCentre, double northingProjectionCentre,
    const char *angUnitName, double angUnitConvFactor,
    const char *linearUnitName, double linearUnitConvFactor) {
    UnitOfMeasure linearUnit(
        createLinearUnit(linearUnitName, linearUnitConvFactor));
    UnitOfMeasure angUnit(createAngularUnit(angUnitName, angUnitConvFactor));
    auto conv = Conversion::createHotineObliqueMercatorVariantB(
        PropertyMap(), Angle(latitudeProjectionCentre, angUnit),
        Angle(longitudeProjectionCentre, angUnit),
        Angle(azimuthInitialLine, angUnit),
        Angle(angleFromRectifiedToSkrewGrid, angUnit), Scale(scale),
        Length(eastingProjectionCentre, linearUnit),
        Length(northingProjectionCentre, linearUnit));
    return proj_obj_create_projected_crs(geodetic_crs, crs_name, conv,
                                         linearUnit);
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
proj_obj_create_projected_crs_HotineObliqueMercatorTwoPointNaturalOrigin(
    PJ_OBJ *geodetic_crs, const char *crs_name, double latitudeProjectionCentre,
    double latitudePoint1, double longitudePoint1, double latitudePoint2,
    double longitudePoint2, double scale, double eastingProjectionCentre,
    double northingProjectionCentre, const char *angUnitName,
    double angUnitConvFactor, const char *linearUnitName,
    double linearUnitConvFactor) {
    UnitOfMeasure linearUnit(
        createLinearUnit(linearUnitName, linearUnitConvFactor));
    UnitOfMeasure angUnit(createAngularUnit(angUnitName, angUnitConvFactor));
    auto conv = Conversion::createHotineObliqueMercatorTwoPointNaturalOrigin(
        PropertyMap(), Angle(latitudeProjectionCentre, angUnit),
        Angle(latitudePoint1, angUnit), Angle(longitudePoint1, angUnit),
        Angle(latitudePoint2, angUnit), Angle(longitudePoint2, angUnit),
        Scale(scale), Length(eastingProjectionCentre, linearUnit),
        Length(northingProjectionCentre, linearUnit));
    return proj_obj_create_projected_crs(geodetic_crs, crs_name, conv,
                                         linearUnit);
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
PJ_OBJ *proj_obj_create_projected_crs_InternationalMapWorldPolyconic(
    PJ_OBJ *geodetic_crs, const char *crs_name, double centerLong,
    double latitudeFirstParallel, double latitudeSecondParallel,
    double falseEasting, double falseNorthing, const char *angUnitName,
    double angUnitConvFactor, const char *linearUnitName,
    double linearUnitConvFactor) {
    UnitOfMeasure linearUnit(
        createLinearUnit(linearUnitName, linearUnitConvFactor));
    UnitOfMeasure angUnit(createAngularUnit(angUnitName, angUnitConvFactor));
    auto conv = Conversion::createInternationalMapWorldPolyconic(
        PropertyMap(), Angle(centerLong, angUnit),
        Angle(latitudeFirstParallel, angUnit),
        Angle(latitudeSecondParallel, angUnit),
        Length(falseEasting, linearUnit), Length(falseNorthing, linearUnit));
    return proj_obj_create_projected_crs(geodetic_crs, crs_name, conv,
                                         linearUnit);
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
PJ_OBJ *proj_obj_create_projected_crs_KrovakNorthOriented(
    PJ_OBJ *geodetic_crs, const char *crs_name, double latitudeProjectionCentre,
    double longitudeOfOrigin, double colatitudeConeAxis,
    double latitudePseudoStandardParallel,
    double scaleFactorPseudoStandardParallel, double falseEasting,
    double falseNorthing, const char *angUnitName, double angUnitConvFactor,
    const char *linearUnitName, double linearUnitConvFactor) {
    UnitOfMeasure linearUnit(
        createLinearUnit(linearUnitName, linearUnitConvFactor));
    UnitOfMeasure angUnit(createAngularUnit(angUnitName, angUnitConvFactor));
    auto conv = Conversion::createKrovakNorthOriented(
        PropertyMap(), Angle(latitudeProjectionCentre, angUnit),
        Angle(longitudeOfOrigin, angUnit), Angle(colatitudeConeAxis, angUnit),
        Angle(latitudePseudoStandardParallel, angUnit),
        Scale(scaleFactorPseudoStandardParallel),
        Length(falseEasting, linearUnit), Length(falseNorthing, linearUnit));
    return proj_obj_create_projected_crs(geodetic_crs, crs_name, conv,
                                         linearUnit);
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
PJ_OBJ *proj_obj_create_projected_crs_Krovak(
    PJ_OBJ *geodetic_crs, const char *crs_name, double latitudeProjectionCentre,
    double longitudeOfOrigin, double colatitudeConeAxis,
    double latitudePseudoStandardParallel,
    double scaleFactorPseudoStandardParallel, double falseEasting,
    double falseNorthing, const char *angUnitName, double angUnitConvFactor,
    const char *linearUnitName, double linearUnitConvFactor) {
    UnitOfMeasure linearUnit(
        createLinearUnit(linearUnitName, linearUnitConvFactor));
    UnitOfMeasure angUnit(createAngularUnit(angUnitName, angUnitConvFactor));
    auto conv = Conversion::createKrovak(
        PropertyMap(), Angle(latitudeProjectionCentre, angUnit),
        Angle(longitudeOfOrigin, angUnit), Angle(colatitudeConeAxis, angUnit),
        Angle(latitudePseudoStandardParallel, angUnit),
        Scale(scaleFactorPseudoStandardParallel),
        Length(falseEasting, linearUnit), Length(falseNorthing, linearUnit));
    return proj_obj_create_projected_crs(geodetic_crs, crs_name, conv,
                                         linearUnit);
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
PJ_OBJ *proj_obj_create_projected_crs_LambertAzimuthalEqualArea(
    PJ_OBJ *geodetic_crs, const char *crs_name, double latitudeNatOrigin,
    double longitudeNatOrigin, double falseEasting, double falseNorthing,
    const char *angUnitName, double angUnitConvFactor,
    const char *linearUnitName, double linearUnitConvFactor) {
    UnitOfMeasure linearUnit(
        createLinearUnit(linearUnitName, linearUnitConvFactor));
    UnitOfMeasure angUnit(createAngularUnit(angUnitName, angUnitConvFactor));
    auto conv = Conversion::createLambertAzimuthalEqualArea(
        PropertyMap(), Angle(latitudeNatOrigin, angUnit),
        Angle(longitudeNatOrigin, angUnit), Length(falseEasting, linearUnit),
        Length(falseNorthing, linearUnit));
    return proj_obj_create_projected_crs(geodetic_crs, crs_name, conv,
                                         linearUnit);
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
PJ_OBJ *proj_obj_create_projected_crs_MillerCylindrical(
    PJ_OBJ *geodetic_crs, const char *crs_name, double centerLong,
    double falseEasting, double falseNorthing, const char *angUnitName,
    double angUnitConvFactor, const char *linearUnitName,
    double linearUnitConvFactor) {
    UnitOfMeasure linearUnit(
        createLinearUnit(linearUnitName, linearUnitConvFactor));
    UnitOfMeasure angUnit(createAngularUnit(angUnitName, angUnitConvFactor));
    auto conv = Conversion::createMillerCylindrical(
        PropertyMap(), Angle(centerLong, angUnit),
        Length(falseEasting, linearUnit), Length(falseNorthing, linearUnit));
    return proj_obj_create_projected_crs(geodetic_crs, crs_name, conv,
                                         linearUnit);
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
PJ_OBJ *proj_obj_create_projected_crs_MercatorVariantA(
    PJ_OBJ *geodetic_crs, const char *crs_name, double centerLat,
    double centerLong, double scale, double falseEasting, double falseNorthing,
    const char *angUnitName, double angUnitConvFactor,
    const char *linearUnitName, double linearUnitConvFactor) {
    UnitOfMeasure linearUnit(
        createLinearUnit(linearUnitName, linearUnitConvFactor));
    UnitOfMeasure angUnit(createAngularUnit(angUnitName, angUnitConvFactor));
    auto conv = Conversion::createMercatorVariantA(
        PropertyMap(), Angle(centerLat, angUnit), Angle(centerLong, angUnit),
        Scale(scale), Length(falseEasting, linearUnit),
        Length(falseNorthing, linearUnit));
    return proj_obj_create_projected_crs(geodetic_crs, crs_name, conv,
                                         linearUnit);
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
PJ_OBJ *proj_obj_create_projected_crs_MercatorVariantB(
    PJ_OBJ *geodetic_crs, const char *crs_name, double latitudeFirstParallel,
    double centerLong, double falseEasting, double falseNorthing,
    const char *angUnitName, double angUnitConvFactor,
    const char *linearUnitName, double linearUnitConvFactor) {
    UnitOfMeasure linearUnit(
        createLinearUnit(linearUnitName, linearUnitConvFactor));
    UnitOfMeasure angUnit(createAngularUnit(angUnitName, angUnitConvFactor));
    auto conv = Conversion::createMercatorVariantB(
        PropertyMap(), Angle(latitudeFirstParallel, angUnit),
        Angle(centerLong, angUnit), Length(falseEasting, linearUnit),
        Length(falseNorthing, linearUnit));
    return proj_obj_create_projected_crs(geodetic_crs, crs_name, conv,
                                         linearUnit);
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
PJ_OBJ *proj_obj_create_projected_crs_PopularVisualisationPseudoMercator(
    PJ_OBJ *geodetic_crs, const char *crs_name, double centerLat,
    double centerLong, double falseEasting, double falseNorthing,
    const char *angUnitName, double angUnitConvFactor,
    const char *linearUnitName, double linearUnitConvFactor) {
    UnitOfMeasure linearUnit(
        createLinearUnit(linearUnitName, linearUnitConvFactor));
    UnitOfMeasure angUnit(createAngularUnit(angUnitName, angUnitConvFactor));
    auto conv = Conversion::createPopularVisualisationPseudoMercator(
        PropertyMap(), Angle(centerLat, angUnit), Angle(centerLong, angUnit),
        Length(falseEasting, linearUnit), Length(falseNorthing, linearUnit));
    return proj_obj_create_projected_crs(geodetic_crs, crs_name, conv,
                                         linearUnit);
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
PJ_OBJ *proj_obj_create_projected_crs_Mollweide(
    PJ_OBJ *geodetic_crs, const char *crs_name, double centerLong,
    double falseEasting, double falseNorthing, const char *angUnitName,
    double angUnitConvFactor, const char *linearUnitName,
    double linearUnitConvFactor) {
    UnitOfMeasure linearUnit(
        createLinearUnit(linearUnitName, linearUnitConvFactor));
    UnitOfMeasure angUnit(createAngularUnit(angUnitName, angUnitConvFactor));
    auto conv = Conversion::createMollweide(
        PropertyMap(), Angle(centerLong, angUnit),
        Length(falseEasting, linearUnit), Length(falseNorthing, linearUnit));
    return proj_obj_create_projected_crs(geodetic_crs, crs_name, conv,
                                         linearUnit);
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
PJ_OBJ *proj_obj_create_projected_crs_NewZealandMappingGrid(
    PJ_OBJ *geodetic_crs, const char *crs_name, double centerLat,
    double centerLong, double falseEasting, double falseNorthing,
    const char *angUnitName, double angUnitConvFactor,
    const char *linearUnitName, double linearUnitConvFactor) {
    UnitOfMeasure linearUnit(
        createLinearUnit(linearUnitName, linearUnitConvFactor));
    UnitOfMeasure angUnit(createAngularUnit(angUnitName, angUnitConvFactor));
    auto conv = Conversion::createNewZealandMappingGrid(
        PropertyMap(), Angle(centerLat, angUnit), Angle(centerLong, angUnit),
        Length(falseEasting, linearUnit), Length(falseNorthing, linearUnit));
    return proj_obj_create_projected_crs(geodetic_crs, crs_name, conv,
                                         linearUnit);
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
PJ_OBJ *proj_obj_create_projected_crs_ObliqueStereographic(
    PJ_OBJ *geodetic_crs, const char *crs_name, double centerLat,
    double centerLong, double scale, double falseEasting, double falseNorthing,
    const char *angUnitName, double angUnitConvFactor,
    const char *linearUnitName, double linearUnitConvFactor) {
    UnitOfMeasure linearUnit(
        createLinearUnit(linearUnitName, linearUnitConvFactor));
    UnitOfMeasure angUnit(createAngularUnit(angUnitName, angUnitConvFactor));
    auto conv = Conversion::createObliqueStereographic(
        PropertyMap(), Angle(centerLat, angUnit), Angle(centerLong, angUnit),
        Scale(scale), Length(falseEasting, linearUnit),
        Length(falseNorthing, linearUnit));
    return proj_obj_create_projected_crs(geodetic_crs, crs_name, conv,
                                         linearUnit);
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
PJ_OBJ *proj_obj_create_projected_crs_Orthographic(
    PJ_OBJ *geodetic_crs, const char *crs_name, double centerLat,
    double centerLong, double falseEasting, double falseNorthing,
    const char *angUnitName, double angUnitConvFactor,
    const char *linearUnitName, double linearUnitConvFactor) {
    UnitOfMeasure linearUnit(
        createLinearUnit(linearUnitName, linearUnitConvFactor));
    UnitOfMeasure angUnit(createAngularUnit(angUnitName, angUnitConvFactor));
    auto conv = Conversion::createOrthographic(
        PropertyMap(), Angle(centerLat, angUnit), Angle(centerLong, angUnit),
        Length(falseEasting, linearUnit), Length(falseNorthing, linearUnit));
    return proj_obj_create_projected_crs(geodetic_crs, crs_name, conv,
                                         linearUnit);
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
PJ_OBJ *proj_obj_create_projected_crs_AmericanPolyconic(
    PJ_OBJ *geodetic_crs, const char *crs_name, double centerLat,
    double centerLong, double falseEasting, double falseNorthing,
    const char *angUnitName, double angUnitConvFactor,
    const char *linearUnitName, double linearUnitConvFactor) {
    UnitOfMeasure linearUnit(
        createLinearUnit(linearUnitName, linearUnitConvFactor));
    UnitOfMeasure angUnit(createAngularUnit(angUnitName, angUnitConvFactor));
    auto conv = Conversion::createAmericanPolyconic(
        PropertyMap(), Angle(centerLat, angUnit), Angle(centerLong, angUnit),
        Length(falseEasting, linearUnit), Length(falseNorthing, linearUnit));
    return proj_obj_create_projected_crs(geodetic_crs, crs_name, conv,
                                         linearUnit);
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
PJ_OBJ *proj_obj_create_projected_crs_PolarStereographicVariantA(
    PJ_OBJ *geodetic_crs, const char *crs_name, double centerLat,
    double centerLong, double scale, double falseEasting, double falseNorthing,
    const char *angUnitName, double angUnitConvFactor,
    const char *linearUnitName, double linearUnitConvFactor) {
    UnitOfMeasure linearUnit(
        createLinearUnit(linearUnitName, linearUnitConvFactor));
    UnitOfMeasure angUnit(createAngularUnit(angUnitName, angUnitConvFactor));
    auto conv = Conversion::createPolarStereographicVariantA(
        PropertyMap(), Angle(centerLat, angUnit), Angle(centerLong, angUnit),
        Scale(scale), Length(falseEasting, linearUnit),
        Length(falseNorthing, linearUnit));
    return proj_obj_create_projected_crs(geodetic_crs, crs_name, conv,
                                         linearUnit);
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
PJ_OBJ *proj_obj_create_projected_crs_PolarStereographicVariantB(
    PJ_OBJ *geodetic_crs, const char *crs_name, double latitudeStandardParallel,
    double longitudeOfOrigin, double falseEasting, double falseNorthing,
    const char *angUnitName, double angUnitConvFactor,
    const char *linearUnitName, double linearUnitConvFactor) {
    UnitOfMeasure linearUnit(
        createLinearUnit(linearUnitName, linearUnitConvFactor));
    UnitOfMeasure angUnit(createAngularUnit(angUnitName, angUnitConvFactor));
    auto conv = Conversion::createPolarStereographicVariantB(
        PropertyMap(), Angle(latitudeStandardParallel, angUnit),
        Angle(longitudeOfOrigin, angUnit), Length(falseEasting, linearUnit),
        Length(falseNorthing, linearUnit));
    return proj_obj_create_projected_crs(geodetic_crs, crs_name, conv,
                                         linearUnit);
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
PJ_OBJ *proj_obj_create_projected_crs_Robinson(
    PJ_OBJ *geodetic_crs, const char *crs_name, double centerLong,
    double falseEasting, double falseNorthing, const char *angUnitName,
    double angUnitConvFactor, const char *linearUnitName,
    double linearUnitConvFactor) {
    UnitOfMeasure linearUnit(
        createLinearUnit(linearUnitName, linearUnitConvFactor));
    UnitOfMeasure angUnit(createAngularUnit(angUnitName, angUnitConvFactor));
    auto conv = Conversion::createRobinson(
        PropertyMap(), Angle(centerLong, angUnit),
        Length(falseEasting, linearUnit), Length(falseNorthing, linearUnit));
    return proj_obj_create_projected_crs(geodetic_crs, crs_name, conv,
                                         linearUnit);
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
PJ_OBJ *proj_obj_create_projected_crs_Sinusoidal(
    PJ_OBJ *geodetic_crs, const char *crs_name, double centerLong,
    double falseEasting, double falseNorthing, const char *angUnitName,
    double angUnitConvFactor, const char *linearUnitName,
    double linearUnitConvFactor) {
    UnitOfMeasure linearUnit(
        createLinearUnit(linearUnitName, linearUnitConvFactor));
    UnitOfMeasure angUnit(createAngularUnit(angUnitName, angUnitConvFactor));
    auto conv = Conversion::createSinusoidal(
        PropertyMap(), Angle(centerLong, angUnit),
        Length(falseEasting, linearUnit), Length(falseNorthing, linearUnit));
    return proj_obj_create_projected_crs(geodetic_crs, crs_name, conv,
                                         linearUnit);
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
PJ_OBJ *proj_obj_create_projected_crs_Stereographic(
    PJ_OBJ *geodetic_crs, const char *crs_name, double centerLat,
    double centerLong, double scale, double falseEasting, double falseNorthing,
    const char *angUnitName, double angUnitConvFactor,
    const char *linearUnitName, double linearUnitConvFactor) {
    UnitOfMeasure linearUnit(
        createLinearUnit(linearUnitName, linearUnitConvFactor));
    UnitOfMeasure angUnit(createAngularUnit(angUnitName, angUnitConvFactor));
    auto conv = Conversion::createStereographic(
        PropertyMap(), Angle(centerLat, angUnit), Angle(centerLong, angUnit),
        Scale(scale), Length(falseEasting, linearUnit),
        Length(falseNorthing, linearUnit));
    return proj_obj_create_projected_crs(geodetic_crs, crs_name, conv,
                                         linearUnit);
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
PJ_OBJ *proj_obj_create_projected_crs_VanDerGrinten(
    PJ_OBJ *geodetic_crs, const char *crs_name, double centerLong,
    double falseEasting, double falseNorthing, const char *angUnitName,
    double angUnitConvFactor, const char *linearUnitName,
    double linearUnitConvFactor) {
    UnitOfMeasure linearUnit(
        createLinearUnit(linearUnitName, linearUnitConvFactor));
    UnitOfMeasure angUnit(createAngularUnit(angUnitName, angUnitConvFactor));
    auto conv = Conversion::createVanDerGrinten(
        PropertyMap(), Angle(centerLong, angUnit),
        Length(falseEasting, linearUnit), Length(falseNorthing, linearUnit));
    return proj_obj_create_projected_crs(geodetic_crs, crs_name, conv,
                                         linearUnit);
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
PJ_OBJ *proj_obj_create_projected_crs_WagnerI(
    PJ_OBJ *geodetic_crs, const char *crs_name, double centerLong,
    double falseEasting, double falseNorthing, const char *angUnitName,
    double angUnitConvFactor, const char *linearUnitName,
    double linearUnitConvFactor) {
    UnitOfMeasure linearUnit(
        createLinearUnit(linearUnitName, linearUnitConvFactor));
    UnitOfMeasure angUnit(createAngularUnit(angUnitName, angUnitConvFactor));
    auto conv = Conversion::createWagnerI(
        PropertyMap(), Angle(centerLong, angUnit),
        Length(falseEasting, linearUnit), Length(falseNorthing, linearUnit));
    return proj_obj_create_projected_crs(geodetic_crs, crs_name, conv,
                                         linearUnit);
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
PJ_OBJ *proj_obj_create_projected_crs_WagnerII(
    PJ_OBJ *geodetic_crs, const char *crs_name, double centerLong,
    double falseEasting, double falseNorthing, const char *angUnitName,
    double angUnitConvFactor, const char *linearUnitName,
    double linearUnitConvFactor) {
    UnitOfMeasure linearUnit(
        createLinearUnit(linearUnitName, linearUnitConvFactor));
    UnitOfMeasure angUnit(createAngularUnit(angUnitName, angUnitConvFactor));
    auto conv = Conversion::createWagnerII(
        PropertyMap(), Angle(centerLong, angUnit),
        Length(falseEasting, linearUnit), Length(falseNorthing, linearUnit));
    return proj_obj_create_projected_crs(geodetic_crs, crs_name, conv,
                                         linearUnit);
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
PJ_OBJ *proj_obj_create_projected_crs_WagnerIII(
    PJ_OBJ *geodetic_crs, const char *crs_name, double latitudeTrueScale,
    double centerLong, double falseEasting, double falseNorthing,
    const char *angUnitName, double angUnitConvFactor,
    const char *linearUnitName, double linearUnitConvFactor) {
    UnitOfMeasure linearUnit(
        createLinearUnit(linearUnitName, linearUnitConvFactor));
    UnitOfMeasure angUnit(createAngularUnit(angUnitName, angUnitConvFactor));
    auto conv = Conversion::createWagnerIII(
        PropertyMap(), Angle(latitudeTrueScale, angUnit),
        Angle(centerLong, angUnit), Length(falseEasting, linearUnit),
        Length(falseNorthing, linearUnit));
    return proj_obj_create_projected_crs(geodetic_crs, crs_name, conv,
                                         linearUnit);
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
PJ_OBJ *proj_obj_create_projected_crs_WagnerIV(
    PJ_OBJ *geodetic_crs, const char *crs_name, double centerLong,
    double falseEasting, double falseNorthing, const char *angUnitName,
    double angUnitConvFactor, const char *linearUnitName,
    double linearUnitConvFactor) {
    UnitOfMeasure linearUnit(
        createLinearUnit(linearUnitName, linearUnitConvFactor));
    UnitOfMeasure angUnit(createAngularUnit(angUnitName, angUnitConvFactor));
    auto conv = Conversion::createWagnerIV(
        PropertyMap(), Angle(centerLong, angUnit),
        Length(falseEasting, linearUnit), Length(falseNorthing, linearUnit));
    return proj_obj_create_projected_crs(geodetic_crs, crs_name, conv,
                                         linearUnit);
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
PJ_OBJ *proj_obj_create_projected_crs_WagnerV(
    PJ_OBJ *geodetic_crs, const char *crs_name, double centerLong,
    double falseEasting, double falseNorthing, const char *angUnitName,
    double angUnitConvFactor, const char *linearUnitName,
    double linearUnitConvFactor) {
    UnitOfMeasure linearUnit(
        createLinearUnit(linearUnitName, linearUnitConvFactor));
    UnitOfMeasure angUnit(createAngularUnit(angUnitName, angUnitConvFactor));
    auto conv = Conversion::createWagnerV(
        PropertyMap(), Angle(centerLong, angUnit),
        Length(falseEasting, linearUnit), Length(falseNorthing, linearUnit));
    return proj_obj_create_projected_crs(geodetic_crs, crs_name, conv,
                                         linearUnit);
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
PJ_OBJ *proj_obj_create_projected_crs_WagnerVI(
    PJ_OBJ *geodetic_crs, const char *crs_name, double centerLong,
    double falseEasting, double falseNorthing, const char *angUnitName,
    double angUnitConvFactor, const char *linearUnitName,
    double linearUnitConvFactor) {
    UnitOfMeasure linearUnit(
        createLinearUnit(linearUnitName, linearUnitConvFactor));
    UnitOfMeasure angUnit(createAngularUnit(angUnitName, angUnitConvFactor));
    auto conv = Conversion::createWagnerVI(
        PropertyMap(), Angle(centerLong, angUnit),
        Length(falseEasting, linearUnit), Length(falseNorthing, linearUnit));
    return proj_obj_create_projected_crs(geodetic_crs, crs_name, conv,
                                         linearUnit);
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
PJ_OBJ *proj_obj_create_projected_crs_WagnerVII(
    PJ_OBJ *geodetic_crs, const char *crs_name, double centerLong,
    double falseEasting, double falseNorthing, const char *angUnitName,
    double angUnitConvFactor, const char *linearUnitName,
    double linearUnitConvFactor) {
    UnitOfMeasure linearUnit(
        createLinearUnit(linearUnitName, linearUnitConvFactor));
    UnitOfMeasure angUnit(createAngularUnit(angUnitName, angUnitConvFactor));
    auto conv = Conversion::createWagnerVII(
        PropertyMap(), Angle(centerLong, angUnit),
        Length(falseEasting, linearUnit), Length(falseNorthing, linearUnit));
    return proj_obj_create_projected_crs(geodetic_crs, crs_name, conv,
                                         linearUnit);
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
PJ_OBJ *proj_obj_create_projected_crs_QuadrilateralizedSphericalCube(
    PJ_OBJ *geodetic_crs, const char *crs_name, double centerLat,
    double centerLong, double falseEasting, double falseNorthing,
    const char *angUnitName, double angUnitConvFactor,
    const char *linearUnitName, double linearUnitConvFactor) {
    UnitOfMeasure linearUnit(
        createLinearUnit(linearUnitName, linearUnitConvFactor));
    UnitOfMeasure angUnit(createAngularUnit(angUnitName, angUnitConvFactor));
    auto conv = Conversion::createQuadrilateralizedSphericalCube(
        PropertyMap(), Angle(centerLat, angUnit), Angle(centerLong, angUnit),
        Length(falseEasting, linearUnit), Length(falseNorthing, linearUnit));
    return proj_obj_create_projected_crs(geodetic_crs, crs_name, conv,
                                         linearUnit);
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
PJ_OBJ *proj_obj_create_projected_crs_SphericalCrossTrackHeight(
    PJ_OBJ *geodetic_crs, const char *crs_name, double pegPointLat,
    double pegPointLong, double pegPointHeading, double pegPointHeight,
    const char *angUnitName, double angUnitConvFactor,
    const char *linearUnitName, double linearUnitConvFactor) {
    UnitOfMeasure linearUnit(
        createLinearUnit(linearUnitName, linearUnitConvFactor));
    UnitOfMeasure angUnit(createAngularUnit(angUnitName, angUnitConvFactor));
    auto conv = Conversion::createSphericalCrossTrackHeight(
        PropertyMap(), Angle(pegPointLat, angUnit),
        Angle(pegPointLong, angUnit), Angle(pegPointHeading, angUnit),
        Length(pegPointHeight, linearUnit));
    return proj_obj_create_projected_crs(geodetic_crs, crs_name, conv,
                                         linearUnit);
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
PJ_OBJ *proj_obj_create_projected_crs_EqualEarth(
    PJ_OBJ *geodetic_crs, const char *crs_name, double centerLong,
    double falseEasting, double falseNorthing, const char *angUnitName,
    double angUnitConvFactor, const char *linearUnitName,
    double linearUnitConvFactor) {
    UnitOfMeasure linearUnit(
        createLinearUnit(linearUnitName, linearUnitConvFactor));
    UnitOfMeasure angUnit(createAngularUnit(angUnitName, angUnitConvFactor));
    auto conv = Conversion::createEqualEarth(
        PropertyMap(), Angle(centerLong, angUnit),
        Length(falseEasting, linearUnit), Length(falseNorthing, linearUnit));
    return proj_obj_create_projected_crs(geodetic_crs, crs_name, conv,
                                         linearUnit);
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

int proj_coordoperation_is_instanciable(PJ_OBJ *coordoperation) {
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

int proj_coordoperation_get_param_count(PJ_OBJ *coordoperation) {
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

int proj_coordoperation_get_param_index(PJ_OBJ *coordoperation,
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

int proj_coordoperation_get_param(PJ_OBJ *coordoperation, int index,
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

int proj_coordoperation_get_grid_used_count(PJ_OBJ *coordoperation) {
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

int proj_coordoperation_get_grid_used(PJ_OBJ *coordoperation, int index,
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
 * For an area of interest crossing the anti-meridian, west_lon will be
 * greater than east_lon.
 *
 * @param ctxt Operation factory context. must not be NULL
 * @param west_lon West longitude (in degrees).
 * @param south_lat South latitude (in degrees).
 * @param east_lon East longitude (in degrees).
 * @param north_lat North latitude (in degrees).
 */
void proj_operation_factory_context_set_area_of_interest(
    PJ_OPERATION_FACTORY_CONTEXT *ctxt, double west_lon, double south_lat,
    double east_lon, double north_lat) {
    assert(ctxt);
    ctxt->operationContext->setAreaOfInterest(
        Extent::createFromBBOX(west_lon, south_lat, east_lon, north_lat));
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
PJ_OBJ_LIST *
proj_obj_create_operations(PJ_OBJ *source_crs, PJ_OBJ *target_crs,
                           PJ_OPERATION_FACTORY_CONTEXT *operationContext) {
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
int proj_obj_list_get_count(PJ_OBJ_LIST *result) {
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

PJ_OBJ *proj_obj_list_get(PJ_OBJ_LIST *result, int index) {
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
double proj_coordoperation_get_accuracy(PJ_OBJ *coordoperation) {
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
