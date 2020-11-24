/******************************************************************************
 *
 * Project:  PROJ
 * Purpose:  ISO19111:2019 implementation
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
#define FROM_COORDINATE_OPERATION_CPP

#include "proj/coordinateoperation.hpp"
#include "proj/common.hpp"
#include "proj/crs.hpp"
#include "proj/io.hpp"
#include "proj/metadata.hpp"
#include "proj/util.hpp"

#include "proj/internal/internal.hpp"
#include "proj/internal/io_internal.hpp"
#include "proj/internal/tracing.hpp"

// PROJ include order is sensitive
// clang-format off
#include "proj.h"
#include "proj_internal.h" // M_PI
// clang-format on

#include "proj_json_streaming_writer.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstring>
#include <memory>
#include <set>
#include <string>
#include <vector>

// #define TRACE_CREATE_OPERATIONS
// #define DEBUG_SORT
// #define DEBUG_CONCATENATED_OPERATION
#if defined(DEBUG_SORT) || defined(DEBUG_CONCATENATED_OPERATION)
#include <iostream>

void dumpWKT(const NS_PROJ::crs::CRS *crs);
void dumpWKT(const NS_PROJ::crs::CRS *crs) {
    auto f(NS_PROJ::io::WKTFormatter::create(
        NS_PROJ::io::WKTFormatter::Convention::WKT2_2019));
    std::cerr << crs->exportToWKT(f.get()) << std::endl;
}

void dumpWKT(const NS_PROJ::crs::CRSPtr &crs);
void dumpWKT(const NS_PROJ::crs::CRSPtr &crs) { dumpWKT(crs.get()); }

void dumpWKT(const NS_PROJ::crs::CRSNNPtr &crs);
void dumpWKT(const NS_PROJ::crs::CRSNNPtr &crs) {
    dumpWKT(crs.as_nullable().get());
}

void dumpWKT(const NS_PROJ::crs::GeographicCRSPtr &crs);
void dumpWKT(const NS_PROJ::crs::GeographicCRSPtr &crs) { dumpWKT(crs.get()); }

void dumpWKT(const NS_PROJ::crs::GeographicCRSNNPtr &crs);
void dumpWKT(const NS_PROJ::crs::GeographicCRSNNPtr &crs) {
    dumpWKT(crs.as_nullable().get());
}

#endif

using namespace NS_PROJ::internal;

#if 0
namespace dropbox{ namespace oxygen {
template<> nn<NS_PROJ::operation::CoordinateOperationPtr>::~nn() = default;
template<> nn<NS_PROJ::operation::SingleOperationPtr>::~nn() = default;
template<> nn<NS_PROJ::operation::ConversionPtr>::~nn() = default;
template<> nn<NS_PROJ::operation::TransformationPtr>::~nn() = default;
template<> nn<NS_PROJ::operation::ConcatenatedOperationPtr>::~nn() = default;
template<> nn<NS_PROJ::operation::PointMotionOperationPtr>::~nn() = default;
template<> nn<NS_PROJ::operation::GeneralOperationParameterPtr>::~nn() = default;
template<> nn<NS_PROJ::operation::OperationParameterPtr>::~nn() = default;
template<> nn<NS_PROJ::operation::GeneralParameterValuePtr>::~nn() = default;
template<> nn<NS_PROJ::operation::ParameterValuePtr>::~nn() = default;
template<> nn<NS_PROJ::operation::OperationMethodPtr>::~nn() = default;
template<> nn<NS_PROJ::operation::OperationParameterValuePtr>::~nn() = default;
template<> nn<std::unique_ptr<NS_PROJ::operation::CoordinateOperationFactory, std::default_delete<NS_PROJ::operation::CoordinateOperationFactory> > >::~nn() = default;
template<> nn<std::unique_ptr<NS_PROJ::operation::CoordinateOperationContext, std::default_delete<NS_PROJ::operation::CoordinateOperationContext> > >::~nn() = default;
}}
#endif

#include "proj/internal/coordinateoperation_constants.hpp"
#include "proj/internal/coordinateoperation_internal.hpp"
#include "proj/internal/esri_projection_mappings.hpp"

#if 0
namespace dropbox{ namespace oxygen {
template<> nn<std::shared_ptr<NS_PROJ::operation::InverseCoordinateOperation>>::~nn() = default;
template<> nn<std::shared_ptr<NS_PROJ::operation::InverseConversion>>::~nn() = default;
template<> nn<std::shared_ptr<NS_PROJ::operation::InverseTransformation>>::~nn() = default;
template<> nn<NS_PROJ::operation::PROJBasedOperationPtr>::~nn() = default;
}}
#endif

// ---------------------------------------------------------------------------

NS_PROJ_START
namespace operation {

//! @cond Doxygen_Suppress

constexpr double UTM_LATITUDE_OF_NATURAL_ORIGIN = 0.0;
constexpr double UTM_SCALE_FACTOR = 0.9996;
constexpr double UTM_FALSE_EASTING = 500000.0;
constexpr double UTM_NORTH_FALSE_NORTHING = 0.0;
constexpr double UTM_SOUTH_FALSE_NORTHING = 10000000.0;

static const std::string INVERSE_OF = "Inverse of ";
static const char *BALLPARK_GEOCENTRIC_TRANSLATION =
    "Ballpark geocentric translation";
static const char *NULL_GEOGRAPHIC_OFFSET = "Null geographic offset";
static const char *NULL_GEOCENTRIC_TRANSLATION = "Null geocentric translation";
static const char *BALLPARK_GEOGRAPHIC_OFFSET = "Ballpark geographic offset";
static const char *BALLPARK_VERTICAL_TRANSFORMATION =
    " (ballpark vertical transformation)";
static const char *BALLPARK_VERTICAL_TRANSFORMATION_NO_ELLIPSOID_VERT_HEIGHT =
    " (ballpark vertical transformation, without ellipsoid height to vertical "
    "height correction)";

static const std::string AXIS_ORDER_CHANGE_2D_NAME = "axis order change (2D)";
static const std::string AXIS_ORDER_CHANGE_3D_NAME =
    "axis order change (geographic3D horizontal)";
//! @endcond

//! @cond Doxygen_Suppress
static util::PropertyMap
createPropertiesForInverse(const CoordinateOperation *op, bool derivedFrom,
                           bool approximateInversion);
//! @endcond

// ---------------------------------------------------------------------------

#ifdef TRACE_CREATE_OPERATIONS

//! @cond Doxygen_Suppress

static std::string objectAsStr(const common::IdentifiedObject *obj) {
    std::string ret(obj->nameStr());
    const auto &ids = obj->identifiers();
    if (!ids.empty()) {
        ret += " (";
        ret += (*ids[0]->codeSpace()) + ":" + ids[0]->code();
        ret += ")";
    }
    return ret;
}
//! @endcond

#endif

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress

class InvalidOperationEmptyIntersection : public InvalidOperation {
  public:
    explicit InvalidOperationEmptyIntersection(const std::string &message);
    InvalidOperationEmptyIntersection(
        const InvalidOperationEmptyIntersection &other);
    ~InvalidOperationEmptyIntersection() override;
};

InvalidOperationEmptyIntersection::InvalidOperationEmptyIntersection(
    const std::string &message)
    : InvalidOperation(message) {}

InvalidOperationEmptyIntersection::InvalidOperationEmptyIntersection(
    const InvalidOperationEmptyIntersection &) = default;

InvalidOperationEmptyIntersection::~InvalidOperationEmptyIntersection() =
    default;

// ---------------------------------------------------------------------------

static std::string createEntryEqParam(const std::string &a,
                                      const std::string &b) {
    return a < b ? a + b : b + a;
}

static std::set<std::string> buildSetEquivalentParameters() {

    std::set<std::string> set;

    const char *const listOfEquivalentParameterNames[][7] = {
        {"latitude_of_point_1", "Latitude_Of_1st_Point", nullptr},
        {"longitude_of_point_1", "Longitude_Of_1st_Point", nullptr},
        {"latitude_of_point_2", "Latitude_Of_2nd_Point", nullptr},
        {"longitude_of_point_2", "Longitude_Of_2nd_Point", nullptr},

        {"satellite_height", "height", nullptr},

        {EPSG_NAME_PARAMETER_FALSE_EASTING,
         EPSG_NAME_PARAMETER_EASTING_FALSE_ORIGIN,
         EPSG_NAME_PARAMETER_EASTING_PROJECTION_CENTRE, nullptr},

        {EPSG_NAME_PARAMETER_FALSE_NORTHING,
         EPSG_NAME_PARAMETER_NORTHING_FALSE_ORIGIN,
         EPSG_NAME_PARAMETER_NORTHING_PROJECTION_CENTRE, nullptr},

        {EPSG_NAME_PARAMETER_SCALE_FACTOR_AT_NATURAL_ORIGIN, WKT1_SCALE_FACTOR,
         EPSG_NAME_PARAMETER_SCALE_FACTOR_INITIAL_LINE,
         EPSG_NAME_PARAMETER_SCALE_FACTOR_PSEUDO_STANDARD_PARALLEL, nullptr},

        {WKT1_LATITUDE_OF_ORIGIN, WKT1_LATITUDE_OF_CENTER,
         EPSG_NAME_PARAMETER_LATITUDE_OF_NATURAL_ORIGIN,
         EPSG_NAME_PARAMETER_LATITUDE_FALSE_ORIGIN,
         EPSG_NAME_PARAMETER_LATITUDE_PROJECTION_CENTRE, "Central_Parallel",
         nullptr},

        {WKT1_CENTRAL_MERIDIAN, WKT1_LONGITUDE_OF_CENTER,
         EPSG_NAME_PARAMETER_LONGITUDE_OF_NATURAL_ORIGIN,
         EPSG_NAME_PARAMETER_LONGITUDE_FALSE_ORIGIN,
         EPSG_NAME_PARAMETER_LONGITUDE_PROJECTION_CENTRE,
         EPSG_NAME_PARAMETER_LONGITUDE_OF_ORIGIN, nullptr},

        {"pseudo_standard_parallel_1", WKT1_STANDARD_PARALLEL_1, nullptr},
    };

    for (const auto &paramList : listOfEquivalentParameterNames) {
        for (size_t i = 0; paramList[i]; i++) {
            auto a = metadata::Identifier::canonicalizeName(paramList[i]);
            for (size_t j = i + 1; paramList[j]; j++) {
                auto b = metadata::Identifier::canonicalizeName(paramList[j]);
                set.insert(createEntryEqParam(a, b));
            }
        }
    }
    return set;
}

bool areEquivalentParameters(const std::string &a, const std::string &b) {

    static const std::set<std::string> setEquivalentParameters =
        buildSetEquivalentParameters();

    auto a_can = metadata::Identifier::canonicalizeName(a);
    auto b_can = metadata::Identifier::canonicalizeName(b);
    return setEquivalentParameters.find(createEntryEqParam(a_can, b_can)) !=
           setEquivalentParameters.end();
}

// ---------------------------------------------------------------------------

PROJ_NO_INLINE const MethodMapping *getMapping(int epsg_code) noexcept {
    for (const auto &mapping : projectionMethodMappings) {
        if (mapping.epsg_code == epsg_code) {
            return &mapping;
        }
    }
    return nullptr;
}

// ---------------------------------------------------------------------------

const MethodMapping *getMapping(const OperationMethod *method) noexcept {
    const std::string &name(method->nameStr());
    const int epsg_code = method->getEPSGCode();
    for (const auto &mapping : projectionMethodMappings) {
        if ((epsg_code != 0 && mapping.epsg_code == epsg_code) ||
            metadata::Identifier::isEquivalentName(mapping.wkt2_name,
                                                   name.c_str())) {
            return &mapping;
        }
    }
    return nullptr;
}

// ---------------------------------------------------------------------------

const MethodMapping *getMappingFromWKT1(const std::string &wkt1_name) noexcept {
    // Unusual for a WKT1 projection name, but mentioned in OGC 12-063r5 C.4.2
    if (ci_starts_with(wkt1_name, "UTM zone")) {
        return getMapping(EPSG_CODE_METHOD_TRANSVERSE_MERCATOR);
    }

    for (const auto &mapping : projectionMethodMappings) {
        if (mapping.wkt1_name && metadata::Identifier::isEquivalentName(
                                     mapping.wkt1_name, wkt1_name.c_str())) {
            return &mapping;
        }
    }
    return nullptr;
}
// ---------------------------------------------------------------------------

const MethodMapping *getMapping(const char *wkt2_name) noexcept {
    for (const auto &mapping : projectionMethodMappings) {
        if (metadata::Identifier::isEquivalentName(mapping.wkt2_name,
                                                   wkt2_name)) {
            return &mapping;
        }
    }
    for (const auto &mapping : otherMethodMappings) {
        if (metadata::Identifier::isEquivalentName(mapping.wkt2_name,
                                                   wkt2_name)) {
            return &mapping;
        }
    }
    return nullptr;
}

// ---------------------------------------------------------------------------

std::vector<const MethodMapping *>
getMappingsFromPROJName(const std::string &projName) {
    std::vector<const MethodMapping *> res;
    for (const auto &mapping : projectionMethodMappings) {
        if (mapping.proj_name_main && projName == mapping.proj_name_main) {
            res.push_back(&mapping);
        }
    }
    return res;
}

// ---------------------------------------------------------------------------

static const ParamMapping *getMapping(const MethodMapping *mapping,
                                      const OperationParameterNNPtr &param) {
    if (mapping->params == nullptr) {
        return nullptr;
    }

    // First try with id
    const int epsg_code = param->getEPSGCode();
    if (epsg_code) {
        for (int i = 0; mapping->params[i] != nullptr; ++i) {
            const auto *paramMapping = mapping->params[i];
            if (paramMapping->epsg_code == epsg_code) {
                return paramMapping;
            }
        }
    }

    // then equivalent name
    const std::string &name = param->nameStr();
    for (int i = 0; mapping->params[i] != nullptr; ++i) {
        const auto *paramMapping = mapping->params[i];
        if (metadata::Identifier::isEquivalentName(paramMapping->wkt2_name,
                                                   name.c_str())) {
            return paramMapping;
        }
    }

    // and finally different name, but equivalent parameter
    for (int i = 0; mapping->params[i] != nullptr; ++i) {
        const auto *paramMapping = mapping->params[i];
        if (areEquivalentParameters(paramMapping->wkt2_name, name)) {
            return paramMapping;
        }
    }

    return nullptr;
}

// ---------------------------------------------------------------------------

const ParamMapping *getMappingFromWKT1(const MethodMapping *mapping,
                                       const std::string &wkt1_name) {
    for (int i = 0; mapping->params[i] != nullptr; ++i) {
        const auto *paramMapping = mapping->params[i];
        if (paramMapping->wkt1_name &&
            (metadata::Identifier::isEquivalentName(paramMapping->wkt1_name,
                                                    wkt1_name.c_str()) ||
             areEquivalentParameters(paramMapping->wkt1_name, wkt1_name))) {
            return paramMapping;
        }
    }
    return nullptr;
}

// ---------------------------------------------------------------------------

std::vector<const ESRIMethodMapping *>
getMappingsFromESRI(const std::string &esri_name) {
    std::vector<const ESRIMethodMapping *> res;
    for (const auto &mapping : esriMappings) {
        if (ci_equal(esri_name, mapping.esri_name)) {
            res.push_back(&mapping);
        }
    }
    return res;
}

// ---------------------------------------------------------------------------

static const ESRIMethodMapping *getESRIMapping(const std::string &wkt2_name,
                                               int epsg_code) {
    for (const auto &mapping : esriMappings) {
        if ((epsg_code != 0 && mapping.epsg_code == epsg_code) ||
            ci_equal(wkt2_name, mapping.wkt2_name)) {
            return &mapping;
        }
    }
    return nullptr;
}

// ---------------------------------------------------------------------------

static double getAccuracy(const std::vector<CoordinateOperationNNPtr> &ops);

// Returns the accuracy of an operation, or -1 if unknown
static double getAccuracy(const CoordinateOperationNNPtr &op) {

    if (dynamic_cast<const Conversion *>(op.get())) {
        // A conversion is perfectly accurate.
        return 0.0;
    }

    double accuracy = -1.0;
    const auto &accuracies = op->coordinateOperationAccuracies();
    if (!accuracies.empty()) {
        try {
            accuracy = c_locale_stod(accuracies[0]->value());
        } catch (const std::exception &) {
        }
    } else {
        auto concatenated =
            dynamic_cast<const ConcatenatedOperation *>(op.get());
        if (concatenated) {
            accuracy = getAccuracy(concatenated->operations());
        }
    }
    return accuracy;
}

// ---------------------------------------------------------------------------

// Returns the accuracy of a set of concatenated operations, or -1 if unknown
static double getAccuracy(const std::vector<CoordinateOperationNNPtr> &ops) {
    double accuracy = -1.0;
    for (const auto &subop : ops) {
        const double subops_accuracy = getAccuracy(subop);
        if (subops_accuracy < 0.0) {
            return -1.0;
        }
        if (accuracy < 0.0) {
            accuracy = 0.0;
        }
        accuracy += subops_accuracy;
    }
    return accuracy;
}

// ---------------------------------------------------------------------------

static metadata::ExtentPtr
getExtent(const std::vector<CoordinateOperationNNPtr> &ops,
          bool conversionExtentIsWorld, bool &emptyIntersection);

static metadata::ExtentPtr getExtent(const CoordinateOperationNNPtr &op,
                                     bool conversionExtentIsWorld,
                                     bool &emptyIntersection) {
    auto conv = dynamic_cast<const Conversion *>(op.get());
    if (conv) {
        emptyIntersection = false;
        return metadata::Extent::WORLD;
    }
    const auto &domains = op->domains();
    if (!domains.empty()) {
        emptyIntersection = false;
        return domains[0]->domainOfValidity();
    }
    auto concatenated = dynamic_cast<const ConcatenatedOperation *>(op.get());
    if (!concatenated) {
        emptyIntersection = false;
        return nullptr;
    }
    return getExtent(concatenated->operations(), conversionExtentIsWorld,
                     emptyIntersection);
}

// ---------------------------------------------------------------------------

static const metadata::ExtentPtr nullExtent{};

static const metadata::ExtentPtr &getExtent(const crs::CRSNNPtr &crs) {
    const auto &domains = crs->domains();
    if (!domains.empty()) {
        return domains[0]->domainOfValidity();
    }
    const auto *boundCRS = dynamic_cast<const crs::BoundCRS *>(crs.get());
    if (boundCRS) {
        return getExtent(boundCRS->baseCRS());
    }
    return nullExtent;
}

static const metadata::ExtentPtr
getExtentPossiblySynthetized(const crs::CRSNNPtr &crs, bool &approxOut) {
    const auto &rawExtent(getExtent(crs));
    approxOut = false;
    if (rawExtent)
        return rawExtent;
    const auto compoundCRS = dynamic_cast<const crs::CompoundCRS *>(crs.get());
    if (compoundCRS) {
        // For a compoundCRS, take the intersection of the extent of its
        // components.
        const auto &components = compoundCRS->componentReferenceSystems();
        metadata::ExtentPtr extent;
        approxOut = true;
        for (const auto &component : components) {
            const auto &componentExtent(getExtent(component));
            if (extent && componentExtent)
                extent = extent->intersection(NN_NO_CHECK(componentExtent));
            else if (componentExtent)
                extent = componentExtent;
        }
        return extent;
    }
    return rawExtent;
}

// ---------------------------------------------------------------------------

static metadata::ExtentPtr
getExtent(const std::vector<CoordinateOperationNNPtr> &ops,
          bool conversionExtentIsWorld, bool &emptyIntersection) {
    metadata::ExtentPtr res = nullptr;
    for (const auto &subop : ops) {

        const auto &subExtent =
            getExtent(subop, conversionExtentIsWorld, emptyIntersection);
        if (!subExtent) {
            if (emptyIntersection) {
                return nullptr;
            }
            continue;
        }
        if (res == nullptr) {
            res = subExtent;
        } else {
            res = res->intersection(NN_NO_CHECK(subExtent));
            if (!res) {
                emptyIntersection = true;
                return nullptr;
            }
        }
    }
    emptyIntersection = false;
    return res;
}

// ---------------------------------------------------------------------------

static double getPseudoArea(const metadata::ExtentPtr &extent) {
    if (!extent)
        return 0.0;
    const auto &geographicElements = extent->geographicElements();
    if (geographicElements.empty())
        return 0.0;
    auto bbox = dynamic_cast<const metadata::GeographicBoundingBox *>(
        geographicElements[0].get());
    if (!bbox)
        return 0;
    double w = bbox->westBoundLongitude();
    double s = bbox->southBoundLatitude();
    double e = bbox->eastBoundLongitude();
    double n = bbox->northBoundLatitude();
    if (w > e) {
        e += 360.0;
    }
    // Integrate cos(lat) between south_lat and north_lat
    return (e - w) * (std::sin(common::Angle(n).getSIValue()) -
                      std::sin(common::Angle(s).getSIValue()));
}

//! @endcond

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
struct CoordinateOperation::Private {
    util::optional<std::string> operationVersion_{};
    std::vector<metadata::PositionalAccuracyNNPtr>
        coordinateOperationAccuracies_{};
    std::weak_ptr<crs::CRS> sourceCRSWeak_{};
    std::weak_ptr<crs::CRS> targetCRSWeak_{};
    crs::CRSPtr interpolationCRS_{};
    util::optional<common::DataEpoch> sourceCoordinateEpoch_{};
    util::optional<common::DataEpoch> targetCoordinateEpoch_{};
    bool hasBallparkTransformation_ = false;
    bool use3DHelmert_ = false;

    // do not set this for a ProjectedCRS.definingConversion
    struct CRSStrongRef {
        crs::CRSNNPtr sourceCRS_;
        crs::CRSNNPtr targetCRS_;
        CRSStrongRef(const crs::CRSNNPtr &sourceCRSIn,
                     const crs::CRSNNPtr &targetCRSIn)
            : sourceCRS_(sourceCRSIn), targetCRS_(targetCRSIn) {}
    };
    std::unique_ptr<CRSStrongRef> strongRef_{};

    Private() = default;
    Private(const Private &other)
        : operationVersion_(other.operationVersion_),
          coordinateOperationAccuracies_(other.coordinateOperationAccuracies_),
          sourceCRSWeak_(other.sourceCRSWeak_),
          targetCRSWeak_(other.targetCRSWeak_),
          interpolationCRS_(other.interpolationCRS_),
          sourceCoordinateEpoch_(other.sourceCoordinateEpoch_),
          targetCoordinateEpoch_(other.targetCoordinateEpoch_),
          hasBallparkTransformation_(other.hasBallparkTransformation_),
          strongRef_(other.strongRef_ ? internal::make_unique<CRSStrongRef>(
                                            *(other.strongRef_))
                                      : nullptr) {}

    Private &operator=(const Private &) = delete;
};

// ---------------------------------------------------------------------------

GridDescription::GridDescription()
    : shortName{}, fullName{}, packageName{}, url{}, directDownload(false),
      openLicense(false), available(false) {}

GridDescription::~GridDescription() = default;

GridDescription::GridDescription(const GridDescription &) = default;

GridDescription::GridDescription(GridDescription &&other) noexcept
    : shortName(std::move(other.shortName)),
      fullName(std::move(other.fullName)),
      packageName(std::move(other.packageName)),
      url(std::move(other.url)),
      directDownload(other.directDownload),
      openLicense(other.openLicense),
      available(other.available) {}

//! @endcond

// ---------------------------------------------------------------------------

CoordinateOperation::CoordinateOperation()
    : d(internal::make_unique<Private>()) {}

// ---------------------------------------------------------------------------

CoordinateOperation::CoordinateOperation(const CoordinateOperation &other)
    : ObjectUsage(other), d(internal::make_unique<Private>(*other.d)) {}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
CoordinateOperation::~CoordinateOperation() = default;
//! @endcond

// ---------------------------------------------------------------------------

/** \brief Return the version of the coordinate transformation (i.e.
 * instantiation
 * due to the stochastic nature of the parameters).
 *
 * Mandatory when describing a coordinate transformation or point motion
 * operation, and should not be supplied for a coordinate conversion.
 *
 * @return version or empty.
 */
const util::optional<std::string> &
CoordinateOperation::operationVersion() const {
    return d->operationVersion_;
}

// ---------------------------------------------------------------------------

/** \brief Return estimate(s) of the impact of this coordinate operation on
 * point accuracy.
 *
 * Gives position error estimates for target coordinates of this coordinate
 * operation, assuming no errors in source coordinates.
 *
 * @return estimate(s) or empty vector.
 */
const std::vector<metadata::PositionalAccuracyNNPtr> &
CoordinateOperation::coordinateOperationAccuracies() const {
    return d->coordinateOperationAccuracies_;
}

// ---------------------------------------------------------------------------

/** \brief Return the source CRS of this coordinate operation.
 *
 * This should not be null, expect for of a derivingConversion of a DerivedCRS
 * when the owning DerivedCRS has been destroyed.
 *
 * @return source CRS, or null.
 */
const crs::CRSPtr CoordinateOperation::sourceCRS() const {
    return d->sourceCRSWeak_.lock();
}

// ---------------------------------------------------------------------------

/** \brief Return the target CRS of this coordinate operation.
 *
 * This should not be null, expect for of a derivingConversion of a DerivedCRS
 * when the owning DerivedCRS has been destroyed.
 *
 * @return target CRS, or null.
 */
const crs::CRSPtr CoordinateOperation::targetCRS() const {
    return d->targetCRSWeak_.lock();
}

// ---------------------------------------------------------------------------

/** \brief Return the interpolation CRS of this coordinate operation.
 *
 * @return interpolation CRS, or null.
 */
const crs::CRSPtr &CoordinateOperation::interpolationCRS() const {
    return d->interpolationCRS_;
}

// ---------------------------------------------------------------------------

/** \brief Return the source epoch of coordinates.
 *
 * @return source epoch of coordinates, or empty.
 */
const util::optional<common::DataEpoch> &
CoordinateOperation::sourceCoordinateEpoch() const {
    return d->sourceCoordinateEpoch_;
}

// ---------------------------------------------------------------------------

/** \brief Return the target epoch of coordinates.
 *
 * @return target epoch of coordinates, or empty.
 */
const util::optional<common::DataEpoch> &
CoordinateOperation::targetCoordinateEpoch() const {
    return d->targetCoordinateEpoch_;
}

// ---------------------------------------------------------------------------

void CoordinateOperation::setWeakSourceTargetCRS(
    std::weak_ptr<crs::CRS> sourceCRSIn, std::weak_ptr<crs::CRS> targetCRSIn) {
    d->sourceCRSWeak_ = sourceCRSIn;
    d->targetCRSWeak_ = targetCRSIn;
}

// ---------------------------------------------------------------------------

void CoordinateOperation::setCRSs(const crs::CRSNNPtr &sourceCRSIn,
                                  const crs::CRSNNPtr &targetCRSIn,
                                  const crs::CRSPtr &interpolationCRSIn) {
    d->strongRef_ =
        internal::make_unique<Private::CRSStrongRef>(sourceCRSIn, targetCRSIn);
    d->sourceCRSWeak_ = sourceCRSIn.as_nullable();
    d->targetCRSWeak_ = targetCRSIn.as_nullable();
    d->interpolationCRS_ = interpolationCRSIn;
}
// ---------------------------------------------------------------------------

void CoordinateOperation::setCRSs(const CoordinateOperation *in,
                                  bool inverseSourceTarget) {
    auto l_sourceCRS = in->sourceCRS();
    auto l_targetCRS = in->targetCRS();
    if (l_sourceCRS && l_targetCRS) {
        auto nn_sourceCRS = NN_NO_CHECK(l_sourceCRS);
        auto nn_targetCRS = NN_NO_CHECK(l_targetCRS);
        if (inverseSourceTarget) {
            setCRSs(nn_targetCRS, nn_sourceCRS, in->interpolationCRS());
        } else {
            setCRSs(nn_sourceCRS, nn_targetCRS, in->interpolationCRS());
        }
    }
}

// ---------------------------------------------------------------------------

void CoordinateOperation::setAccuracies(
    const std::vector<metadata::PositionalAccuracyNNPtr> &accuracies) {
    d->coordinateOperationAccuracies_ = accuracies;
}

// ---------------------------------------------------------------------------

/** \brief Return whether a coordinate operation can be instantiated as
 * a PROJ pipeline, checking in particular that referenced grids are
 * available.
 */
bool CoordinateOperation::isPROJInstantiable(
    const io::DatabaseContextPtr &databaseContext,
    bool considerKnownGridsAsAvailable) const {
    try {
        exportToPROJString(io::PROJStringFormatter::create().get());
    } catch (const std::exception &) {
        return false;
    }
    for (const auto &gridDesc :
         gridsNeeded(databaseContext, considerKnownGridsAsAvailable)) {
        if (!gridDesc.available) {
            return false;
        }
    }
    return true;
}

// ---------------------------------------------------------------------------

/** \brief Return whether a coordinate operation has a "ballpark"
 * transformation,
 * that is a very approximate one, due to lack of more accurate transformations.
 *
 * Typically a null geographic offset between two horizontal datum, or a
 * null vertical offset (or limited to unit changes) between two vertical
 * datum. Errors of several tens to one hundred meters might be expected,
 * compared to more accurate transformations.
 */
bool CoordinateOperation::hasBallparkTransformation() const {
    return d->hasBallparkTransformation_;
}

// ---------------------------------------------------------------------------

void CoordinateOperation::setHasBallparkTransformation(bool b) {
    d->hasBallparkTransformation_ = b;
}

// ---------------------------------------------------------------------------

void CoordinateOperation::setProperties(
    const util::PropertyMap &properties) // throw(InvalidValueTypeException)
{
    ObjectUsage::setProperties(properties);
    properties.getStringValue(OPERATION_VERSION_KEY, d->operationVersion_);
}

// ---------------------------------------------------------------------------

/** \brief Return a variation of the current coordinate operation whose axis
 * order is the one expected for visualization purposes.
 */
CoordinateOperationNNPtr
CoordinateOperation::normalizeForVisualization() const {
    auto l_sourceCRS = sourceCRS();
    auto l_targetCRS = targetCRS();
    if (!l_sourceCRS || !l_targetCRS) {
        throw util::UnsupportedOperationException(
            "Cannot retrieve source or target CRS");
    }
    const bool swapSource =
        l_sourceCRS->mustAxisOrderBeSwitchedForVisualization();
    const bool swapTarget =
        l_targetCRS->mustAxisOrderBeSwitchedForVisualization();
    auto l_this = NN_NO_CHECK(std::dynamic_pointer_cast<CoordinateOperation>(
        shared_from_this().as_nullable()));
    if (!swapSource && !swapTarget) {
        return l_this;
    }
    std::vector<CoordinateOperationNNPtr> subOps;
    if (swapSource) {
        auto op = Conversion::createAxisOrderReversal(false);
        op->setCRSs(l_sourceCRS->normalizeForVisualization(),
                    NN_NO_CHECK(l_sourceCRS), nullptr);
        subOps.emplace_back(op);
    }
    subOps.emplace_back(l_this);
    if (swapTarget) {
        auto op = Conversion::createAxisOrderReversal(false);
        op->setCRSs(NN_NO_CHECK(l_targetCRS),
                    l_targetCRS->normalizeForVisualization(), nullptr);
        subOps.emplace_back(op);
    }
    return util::nn_static_pointer_cast<CoordinateOperation>(
        ConcatenatedOperation::createComputeMetadata(subOps, true));
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
CoordinateOperationNNPtr CoordinateOperation::shallowClone() const {
    return _shallowClone();
}
//! @endcond

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
struct OperationMethod::Private {
    util::optional<std::string> formula_{};
    util::optional<metadata::Citation> formulaCitation_{};
    std::vector<GeneralOperationParameterNNPtr> parameters_{};
    std::string projMethodOverride_{};
};
//! @endcond

// ---------------------------------------------------------------------------

OperationMethod::OperationMethod() : d(internal::make_unique<Private>()) {}

// ---------------------------------------------------------------------------

OperationMethod::OperationMethod(const OperationMethod &other)
    : IdentifiedObject(other), d(internal::make_unique<Private>(*other.d)) {}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
OperationMethod::~OperationMethod() = default;
//! @endcond

// ---------------------------------------------------------------------------

/** \brief Return the formula(s) or procedure used by this coordinate operation
 * method.
 *
 * This may be a reference to a publication (in which case use
 * formulaCitation()).
 *
 * Note that the operation method may not be analytic, in which case this
 * attribute references or contains the procedure, not an analytic formula.
 *
 * @return the formula, or empty.
 */
const util::optional<std::string> &OperationMethod::formula() PROJ_PURE_DEFN {
    return d->formula_;
}

// ---------------------------------------------------------------------------

/** \brief Return a reference to a publication giving the formula(s) or
 * procedure
 * used by the coordinate operation method.
 *
 * @return the formula citation, or empty.
 */
const util::optional<metadata::Citation> &
OperationMethod::formulaCitation() PROJ_PURE_DEFN {
    return d->formulaCitation_;
}

// ---------------------------------------------------------------------------

/** \brief Return the parameters of this operation method.
 *
 * @return the parameters.
 */
const std::vector<GeneralOperationParameterNNPtr> &
OperationMethod::parameters() PROJ_PURE_DEFN {
    return d->parameters_;
}

// ---------------------------------------------------------------------------

/** \brief Instantiate a operation method from a vector of
 * GeneralOperationParameter.
 *
 * @param properties See \ref general_properties. At minimum the name should be
 * defined.
 * @param parameters Vector of GeneralOperationParameterNNPtr.
 * @return a new OperationMethod.
 */
OperationMethodNNPtr OperationMethod::create(
    const util::PropertyMap &properties,
    const std::vector<GeneralOperationParameterNNPtr> &parameters) {
    OperationMethodNNPtr method(
        OperationMethod::nn_make_shared<OperationMethod>());
    method->assignSelf(method);
    method->setProperties(properties);
    method->d->parameters_ = parameters;
    properties.getStringValue("proj_method", method->d->projMethodOverride_);
    return method;
}

// ---------------------------------------------------------------------------

/** \brief Instantiate a operation method from a vector of OperationParameter.
 *
 * @param properties See \ref general_properties. At minimum the name should be
 * defined.
 * @param parameters Vector of OperationParameterNNPtr.
 * @return a new OperationMethod.
 */
OperationMethodNNPtr OperationMethod::create(
    const util::PropertyMap &properties,
    const std::vector<OperationParameterNNPtr> &parameters) {
    std::vector<GeneralOperationParameterNNPtr> parametersGeneral;
    parametersGeneral.reserve(parameters.size());
    for (const auto &p : parameters) {
        parametersGeneral.push_back(p);
    }
    return create(properties, parametersGeneral);
}

// ---------------------------------------------------------------------------

/** \brief Return the EPSG code, either directly, or through the name
 * @return code, or 0 if not found
 */
int OperationMethod::getEPSGCode() PROJ_PURE_DEFN {
    int epsg_code = IdentifiedObject::getEPSGCode();
    if (epsg_code == 0) {
        auto l_name = nameStr();
        if (ends_with(l_name, " (3D)")) {
            l_name.resize(l_name.size() - strlen(" (3D)"));
        }
        for (const auto &tuple : methodNameCodes) {
            if (metadata::Identifier::isEquivalentName(l_name.c_str(),
                                                       tuple.name)) {
                return tuple.epsg_code;
            }
        }
    }
    return epsg_code;
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
void OperationMethod::_exportToWKT(io::WKTFormatter *formatter) const {
    const bool isWKT2 = formatter->version() == io::WKTFormatter::Version::WKT2;
    formatter->startNode(isWKT2 ? io::WKTConstants::METHOD
                                : io::WKTConstants::PROJECTION,
                         !identifiers().empty());
    std::string l_name(nameStr());
    if (!isWKT2) {
        const MethodMapping *mapping = getMapping(this);
        if (mapping == nullptr) {
            l_name = replaceAll(l_name, " ", "_");
        } else {
            if (l_name ==
                PROJ_WKT2_NAME_METHOD_GEOSTATIONARY_SATELLITE_SWEEP_X) {
                l_name = "Geostationary_Satellite";
            } else {
                if (mapping->wkt1_name == nullptr) {
                    throw io::FormattingException(
                        std::string("Unsupported conversion method: ") +
                        mapping->wkt2_name);
                }
                l_name = mapping->wkt1_name;
            }
        }
    }
    formatter->addQuotedString(l_name);
    if (formatter->outputId()) {
        formatID(formatter);
    }
    formatter->endNode();
}
//! @endcond

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
void OperationMethod::_exportToJSON(
    io::JSONFormatter *formatter) const // throw(FormattingException)
{
    auto writer = formatter->writer();
    auto objectContext(formatter->MakeObjectContext("OperationMethod",
                                                    !identifiers().empty()));

    writer->AddObjKey("name");
    writer->Add(nameStr());

    if (formatter->outputId()) {
        formatID(formatter);
    }
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
bool OperationMethod::_isEquivalentTo(
    const util::IComparable *other, util::IComparable::Criterion criterion,
    const io::DatabaseContextPtr &dbContext) const {
    auto otherOM = dynamic_cast<const OperationMethod *>(other);
    if (otherOM == nullptr ||
        !IdentifiedObject::_isEquivalentTo(other, criterion, dbContext)) {
        return false;
    }
    // TODO test formula and formulaCitation
    const auto &params = parameters();
    const auto &otherParams = otherOM->parameters();
    const auto paramsSize = params.size();
    if (paramsSize != otherParams.size()) {
        return false;
    }
    if (criterion == util::IComparable::Criterion::STRICT) {
        for (size_t i = 0; i < paramsSize; i++) {
            if (!params[i]->_isEquivalentTo(otherParams[i].get(), criterion,
                                            dbContext)) {
                return false;
            }
        }
    } else {
        std::vector<bool> candidateIndices(paramsSize, true);
        for (size_t i = 0; i < paramsSize; i++) {
            bool found = false;
            for (size_t j = 0; j < paramsSize; j++) {
                if (candidateIndices[j] &&
                    params[i]->_isEquivalentTo(otherParams[j].get(), criterion,
                                               dbContext)) {
                    candidateIndices[j] = false;
                    found = true;
                    break;
                }
            }
            if (!found) {
                return false;
            }
        }
    }
    return true;
}
//! @endcond

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
struct GeneralParameterValue::Private {};
//! @endcond

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
GeneralParameterValue::GeneralParameterValue() : d(nullptr) {}

// ---------------------------------------------------------------------------

GeneralParameterValue::GeneralParameterValue(const GeneralParameterValue &)
    : d(nullptr) {}
//! @endcond

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
GeneralParameterValue::~GeneralParameterValue() = default;
//! @endcond

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
struct OperationParameterValue::Private {
    OperationParameterNNPtr parameter;
    ParameterValueNNPtr parameterValue;

    Private(const OperationParameterNNPtr &parameterIn,
            const ParameterValueNNPtr &valueIn)
        : parameter(parameterIn), parameterValue(valueIn) {}
};
//! @endcond

// ---------------------------------------------------------------------------

OperationParameterValue::OperationParameterValue(
    const OperationParameterValue &other)
    : GeneralParameterValue(other),
      d(internal::make_unique<Private>(*other.d)) {}

// ---------------------------------------------------------------------------

OperationParameterValue::OperationParameterValue(
    const OperationParameterNNPtr &parameterIn,
    const ParameterValueNNPtr &valueIn)
    : GeneralParameterValue(),
      d(internal::make_unique<Private>(parameterIn, valueIn)) {}

// ---------------------------------------------------------------------------

/** \brief Instantiate a OperationParameterValue.
 *
 * @param parameterIn Parameter (definition).
 * @param valueIn Parameter value.
 * @return a new OperationParameterValue.
 */
OperationParameterValueNNPtr
OperationParameterValue::create(const OperationParameterNNPtr &parameterIn,
                                const ParameterValueNNPtr &valueIn) {
    return OperationParameterValue::nn_make_shared<OperationParameterValue>(
        parameterIn, valueIn);
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
OperationParameterValue::~OperationParameterValue() = default;
//! @endcond

// ---------------------------------------------------------------------------

/** \brief Return the parameter (definition)
 *
 * @return the parameter (definition).
 */
const OperationParameterNNPtr &
OperationParameterValue::parameter() PROJ_PURE_DEFN {
    return d->parameter;
}

// ---------------------------------------------------------------------------

/** \brief Return the parameter value.
 *
 * @return the parameter value.
 */
const ParameterValueNNPtr &
OperationParameterValue::parameterValue() PROJ_PURE_DEFN {
    return d->parameterValue;
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
void OperationParameterValue::_exportToWKT(
    // cppcheck-suppress passedByValue
    io::WKTFormatter *formatter) const {
    _exportToWKT(formatter, nullptr);
}

void OperationParameterValue::_exportToWKT(io::WKTFormatter *formatter,
                                           const MethodMapping *mapping) const {
    const ParamMapping *paramMapping =
        mapping ? getMapping(mapping, d->parameter) : nullptr;
    if (paramMapping && paramMapping->wkt1_name == nullptr) {
        return;
    }
    const bool isWKT2 = formatter->version() == io::WKTFormatter::Version::WKT2;
    if (isWKT2 && parameterValue()->type() == ParameterValue::Type::FILENAME) {
        formatter->startNode(io::WKTConstants::PARAMETERFILE,
                             !parameter()->identifiers().empty());
    } else {
        formatter->startNode(io::WKTConstants::PARAMETER,
                             !parameter()->identifiers().empty());
    }
    if (paramMapping) {
        formatter->addQuotedString(paramMapping->wkt1_name);
    } else {
        formatter->addQuotedString(parameter()->nameStr());
    }
    parameterValue()->_exportToWKT(formatter);
    if (formatter->outputId()) {
        parameter()->formatID(formatter);
    }
    formatter->endNode();
}
//! @endcond

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
void OperationParameterValue::_exportToJSON(
    io::JSONFormatter *formatter) const {
    auto writer = formatter->writer();
    auto objectContext(formatter->MakeObjectContext(
        "ParameterValue", !parameter()->identifiers().empty()));

    writer->AddObjKey("name");
    writer->Add(parameter()->nameStr());

    const auto &l_value(parameterValue());
    if (l_value->type() == ParameterValue::Type::MEASURE) {
        writer->AddObjKey("value");
        writer->Add(l_value->value().value(), 15);
        writer->AddObjKey("unit");
        const auto &l_unit(l_value->value().unit());
        if (l_unit == common::UnitOfMeasure::METRE ||
            l_unit == common::UnitOfMeasure::DEGREE ||
            l_unit == common::UnitOfMeasure::SCALE_UNITY) {
            writer->Add(l_unit.name());
        } else {
            l_unit._exportToJSON(formatter);
        }
    } else if (l_value->type() == ParameterValue::Type::FILENAME) {
        writer->AddObjKey("value");
        writer->Add(l_value->valueFile());
    }

    if (formatter->outputId()) {
        parameter()->formatID(formatter);
    }
}
//! @endcond

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress

/** Utility method used on WKT2 import to convert from abridged transformation
 * to "normal" transformation parameters.
 */
bool OperationParameterValue::convertFromAbridged(
    const std::string &paramName, double &val,
    const common::UnitOfMeasure *&unit, int &paramEPSGCode) {
    if (metadata::Identifier::isEquivalentName(
            paramName.c_str(), EPSG_NAME_PARAMETER_X_AXIS_TRANSLATION) ||
        paramEPSGCode == EPSG_CODE_PARAMETER_X_AXIS_TRANSLATION) {
        unit = &common::UnitOfMeasure::METRE;
        paramEPSGCode = EPSG_CODE_PARAMETER_X_AXIS_TRANSLATION;
        return true;
    } else if (metadata::Identifier::isEquivalentName(
                   paramName.c_str(), EPSG_NAME_PARAMETER_Y_AXIS_TRANSLATION) ||
               paramEPSGCode == EPSG_CODE_PARAMETER_Y_AXIS_TRANSLATION) {
        unit = &common::UnitOfMeasure::METRE;
        paramEPSGCode = EPSG_CODE_PARAMETER_Y_AXIS_TRANSLATION;
        return true;
    } else if (metadata::Identifier::isEquivalentName(
                   paramName.c_str(), EPSG_NAME_PARAMETER_Z_AXIS_TRANSLATION) ||
               paramEPSGCode == EPSG_CODE_PARAMETER_Z_AXIS_TRANSLATION) {
        unit = &common::UnitOfMeasure::METRE;
        paramEPSGCode = EPSG_CODE_PARAMETER_Z_AXIS_TRANSLATION;
        return true;
    } else if (metadata::Identifier::isEquivalentName(
                   paramName.c_str(), EPSG_NAME_PARAMETER_X_AXIS_ROTATION) ||
               paramEPSGCode == EPSG_CODE_PARAMETER_X_AXIS_ROTATION) {
        unit = &common::UnitOfMeasure::ARC_SECOND;
        paramEPSGCode = EPSG_CODE_PARAMETER_X_AXIS_ROTATION;
        return true;
    } else if (metadata::Identifier::isEquivalentName(
                   paramName.c_str(), EPSG_NAME_PARAMETER_Y_AXIS_ROTATION) ||
               paramEPSGCode == EPSG_CODE_PARAMETER_Y_AXIS_ROTATION) {
        unit = &common::UnitOfMeasure::ARC_SECOND;
        paramEPSGCode = EPSG_CODE_PARAMETER_Y_AXIS_ROTATION;
        return true;

    } else if (metadata::Identifier::isEquivalentName(
                   paramName.c_str(), EPSG_NAME_PARAMETER_Z_AXIS_ROTATION) ||
               paramEPSGCode == EPSG_CODE_PARAMETER_Z_AXIS_ROTATION) {
        unit = &common::UnitOfMeasure::ARC_SECOND;
        paramEPSGCode = EPSG_CODE_PARAMETER_Z_AXIS_ROTATION;
        return true;

    } else if (metadata::Identifier::isEquivalentName(
                   paramName.c_str(), EPSG_NAME_PARAMETER_SCALE_DIFFERENCE) ||
               paramEPSGCode == EPSG_CODE_PARAMETER_SCALE_DIFFERENCE) {
        val = (val - 1.0) * 1e6;
        unit = &common::UnitOfMeasure::PARTS_PER_MILLION;
        paramEPSGCode = EPSG_CODE_PARAMETER_SCALE_DIFFERENCE;
        return true;
    }
    return false;
}
//! @endcond

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
bool OperationParameterValue::_isEquivalentTo(
    const util::IComparable *other, util::IComparable::Criterion criterion,
    const io::DatabaseContextPtr &dbContext) const {
    auto otherOPV = dynamic_cast<const OperationParameterValue *>(other);
    if (otherOPV == nullptr) {
        return false;
    }
    if (!d->parameter->_isEquivalentTo(otherOPV->d->parameter.get(), criterion,
                                       dbContext)) {
        return false;
    }
    if (criterion == util::IComparable::Criterion::STRICT) {
        return d->parameterValue->_isEquivalentTo(
            otherOPV->d->parameterValue.get(), criterion);
    }
    if (d->parameterValue->_isEquivalentTo(otherOPV->d->parameterValue.get(),
                                           criterion, dbContext)) {
        return true;
    }
    if (d->parameter->getEPSGCode() ==
            EPSG_CODE_PARAMETER_AZIMUTH_INITIAL_LINE ||
        d->parameter->getEPSGCode() ==
            EPSG_CODE_PARAMETER_ANGLE_RECTIFIED_TO_SKEW_GRID) {
        if (parameterValue()->type() == ParameterValue::Type::MEASURE &&
            otherOPV->parameterValue()->type() ==
                ParameterValue::Type::MEASURE) {
            const double a = std::fmod(parameterValue()->value().convertToUnit(
                                           common::UnitOfMeasure::DEGREE) +
                                           360.0,
                                       360.0);
            const double b =
                std::fmod(otherOPV->parameterValue()->value().convertToUnit(
                              common::UnitOfMeasure::DEGREE) +
                              360.0,
                          360.0);
            return std::fabs(a - b) <= 1e-10 * std::fabs(a);
        }
    }
    return false;
}
//! @endcond

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
struct GeneralOperationParameter::Private {};
//! @endcond

// ---------------------------------------------------------------------------

GeneralOperationParameter::GeneralOperationParameter() : d(nullptr) {}

// ---------------------------------------------------------------------------

GeneralOperationParameter::GeneralOperationParameter(
    const GeneralOperationParameter &other)
    : IdentifiedObject(other), d(nullptr) {}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
GeneralOperationParameter::~GeneralOperationParameter() = default;
//! @endcond

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
struct OperationParameter::Private {};
//! @endcond

// ---------------------------------------------------------------------------

OperationParameter::OperationParameter() : d(nullptr) {}

// ---------------------------------------------------------------------------

OperationParameter::OperationParameter(const OperationParameter &other)
    : GeneralOperationParameter(other), d(nullptr) {}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
OperationParameter::~OperationParameter() = default;
//! @endcond

// ---------------------------------------------------------------------------

/** \brief Instantiate a OperationParameter.
 *
 * @param properties See \ref general_properties. At minimum the name should be
 * defined.
 * @return a new OperationParameter.
 */
OperationParameterNNPtr
OperationParameter::create(const util::PropertyMap &properties) {
    OperationParameterNNPtr op(
        OperationParameter::nn_make_shared<OperationParameter>());
    op->assignSelf(op);
    op->setProperties(properties);
    return op;
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
bool OperationParameter::_isEquivalentTo(
    const util::IComparable *other, util::IComparable::Criterion criterion,
    const io::DatabaseContextPtr &dbContext) const {
    auto otherOP = dynamic_cast<const OperationParameter *>(other);
    if (otherOP == nullptr) {
        return false;
    }
    if (criterion == util::IComparable::Criterion::STRICT) {
        return IdentifiedObject::_isEquivalentTo(other, criterion, dbContext);
    }
    if (IdentifiedObject::_isEquivalentTo(other, criterion, dbContext)) {
        return true;
    }
    auto l_epsgCode = getEPSGCode();
    return l_epsgCode != 0 && l_epsgCode == otherOP->getEPSGCode();
}
//! @endcond

// ---------------------------------------------------------------------------

void OperationParameter::_exportToWKT(io::WKTFormatter *) const {}

// ---------------------------------------------------------------------------

/** \brief Return the name of a parameter designed by its EPSG code
 * @return name, or nullptr if not found
 */
const char *OperationParameter::getNameForEPSGCode(int epsg_code) noexcept {
    for (const auto &tuple : paramNameCodes) {
        if (tuple.epsg_code == epsg_code) {
            return tuple.name;
        }
    }
    return nullptr;
}

// ---------------------------------------------------------------------------

/** \brief Return the EPSG code, either directly, or through the name
 * @return code, or 0 if not found
 */
int OperationParameter::getEPSGCode() PROJ_PURE_DEFN {
    int epsg_code = IdentifiedObject::getEPSGCode();
    if (epsg_code == 0) {
        const auto &l_name = nameStr();
        for (const auto &tuple : paramNameCodes) {
            if (metadata::Identifier::isEquivalentName(l_name.c_str(),
                                                       tuple.name)) {
                return tuple.epsg_code;
            }
        }
        if (metadata::Identifier::isEquivalentName(l_name.c_str(),
                                                   "Latitude of origin")) {
            return EPSG_CODE_PARAMETER_LATITUDE_OF_NATURAL_ORIGIN;
        }
        if (metadata::Identifier::isEquivalentName(l_name.c_str(),
                                                   "Scale factor")) {
            return EPSG_CODE_PARAMETER_SCALE_FACTOR_AT_NATURAL_ORIGIN;
        }
    }
    return epsg_code;
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
struct SingleOperation::Private {
    std::vector<GeneralParameterValueNNPtr> parameterValues_{};
    OperationMethodNNPtr method_;

    explicit Private(const OperationMethodNNPtr &methodIn)
        : method_(methodIn) {}
};
//! @endcond

// ---------------------------------------------------------------------------

SingleOperation::SingleOperation(const OperationMethodNNPtr &methodIn)
    : d(internal::make_unique<Private>(methodIn)) {}

// ---------------------------------------------------------------------------

SingleOperation::SingleOperation(const SingleOperation &other)
    :
#if !defined(COMPILER_WARNS_ABOUT_ABSTRACT_VBASE_INIT)
      CoordinateOperation(other),
#endif
      d(internal::make_unique<Private>(*other.d)) {
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
SingleOperation::~SingleOperation() = default;
//! @endcond

// ---------------------------------------------------------------------------

/** \brief Return the parameter values.
 *
 * @return the parameter values.
 */
const std::vector<GeneralParameterValueNNPtr> &
SingleOperation::parameterValues() PROJ_PURE_DEFN {
    return d->parameterValues_;
}

// ---------------------------------------------------------------------------

/** \brief Return the operation method associated to the operation.
 *
 * @return the operation method.
 */
const OperationMethodNNPtr &SingleOperation::method() PROJ_PURE_DEFN {
    return d->method_;
}

// ---------------------------------------------------------------------------

void SingleOperation::setParameterValues(
    const std::vector<GeneralParameterValueNNPtr> &values) {
    d->parameterValues_ = values;
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
static const ParameterValuePtr nullParameterValue;
//! @endcond

/** \brief Return the parameter value corresponding to a parameter name or
 * EPSG code
 *
 * @param paramName the parameter name (or empty, in which case epsg_code
 *                  should be non zero)
 * @param epsg_code the parameter EPSG code (possibly zero)
 * @return the value, or nullptr if not found.
 */
const ParameterValuePtr &
SingleOperation::parameterValue(const std::string &paramName,
                                int epsg_code) const noexcept {
    if (epsg_code) {
        for (const auto &genOpParamvalue : parameterValues()) {
            auto opParamvalue = dynamic_cast<const OperationParameterValue *>(
                genOpParamvalue.get());
            if (opParamvalue) {
                const auto &parameter = opParamvalue->parameter();
                if (parameter->getEPSGCode() == epsg_code) {
                    return opParamvalue->parameterValue();
                }
            }
        }
    }
    for (const auto &genOpParamvalue : parameterValues()) {
        auto opParamvalue = dynamic_cast<const OperationParameterValue *>(
            genOpParamvalue.get());
        if (opParamvalue) {
            const auto &parameter = opParamvalue->parameter();
            if (metadata::Identifier::isEquivalentName(
                    paramName.c_str(), parameter->nameStr().c_str())) {
                return opParamvalue->parameterValue();
            }
        }
    }
    for (const auto &genOpParamvalue : parameterValues()) {
        auto opParamvalue = dynamic_cast<const OperationParameterValue *>(
            genOpParamvalue.get());
        if (opParamvalue) {
            const auto &parameter = opParamvalue->parameter();
            if (areEquivalentParameters(paramName, parameter->nameStr())) {
                return opParamvalue->parameterValue();
            }
        }
    }
    return nullParameterValue;
}

// ---------------------------------------------------------------------------

/** \brief Return the parameter value corresponding to a EPSG code
 *
 * @param epsg_code the parameter EPSG code
 * @return the value, or nullptr if not found.
 */
const ParameterValuePtr &SingleOperation::parameterValue(int epsg_code) const
    noexcept {
    for (const auto &genOpParamvalue : parameterValues()) {
        auto opParamvalue = dynamic_cast<const OperationParameterValue *>(
            genOpParamvalue.get());
        if (opParamvalue) {
            const auto &parameter = opParamvalue->parameter();
            if (parameter->getEPSGCode() == epsg_code) {
                return opParamvalue->parameterValue();
            }
        }
    }
    return nullParameterValue;
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
static const common::Measure nullMeasure;
//! @endcond

/** \brief Return the parameter value, as a measure, corresponding to a
 * parameter name or EPSG code
 *
 * @param paramName the parameter name (or empty, in which case epsg_code
 *                  should be non zero)
 * @param epsg_code the parameter EPSG code (possibly zero)
 * @return the measure, or the empty Measure() object if not found.
 */
const common::Measure &
SingleOperation::parameterValueMeasure(const std::string &paramName,
                                       int epsg_code) const noexcept {
    const auto &val = parameterValue(paramName, epsg_code);
    if (val && val->type() == ParameterValue::Type::MEASURE) {
        return val->value();
    }
    return nullMeasure;
}

/** \brief Return the parameter value, as a measure, corresponding to a
 * EPSG code
 *
 * @param epsg_code the parameter EPSG code
 * @return the measure, or the empty Measure() object if not found.
 */
const common::Measure &
SingleOperation::parameterValueMeasure(int epsg_code) const noexcept {
    const auto &val = parameterValue(epsg_code);
    if (val && val->type() == ParameterValue::Type::MEASURE) {
        return val->value();
    }
    return nullMeasure;
}

//! @cond Doxygen_Suppress

double SingleOperation::parameterValueNumericAsSI(int epsg_code) const
    noexcept {
    const auto &val = parameterValue(epsg_code);
    if (val && val->type() == ParameterValue::Type::MEASURE) {
        return val->value().getSIValue();
    }
    return 0.0;
}

double SingleOperation::parameterValueNumeric(
    int epsg_code, const common::UnitOfMeasure &targetUnit) const noexcept {
    const auto &val = parameterValue(epsg_code);
    if (val && val->type() == ParameterValue::Type::MEASURE) {
        return val->value().convertToUnit(targetUnit);
    }
    return 0.0;
}

double SingleOperation::parameterValueNumeric(
    const char *param_name, const common::UnitOfMeasure &targetUnit) const
    noexcept {
    const auto &val = parameterValue(param_name, 0);
    if (val && val->type() == ParameterValue::Type::MEASURE) {
        return val->value().convertToUnit(targetUnit);
    }
    return 0.0;
}

//! @endcond
// ---------------------------------------------------------------------------

/** \brief Instantiate a PROJ-based single operation.
 *
 * \note The operation might internally be a pipeline chaining several
 * operations.
 * The use of the SingleOperation modeling here is mostly to be able to get
 * the PROJ string as a parameter.
 *
 * @param properties Properties
 * @param PROJString the PROJ string.
 * @param sourceCRS source CRS (might be null).
 * @param targetCRS target CRS (might be null).
 * @param accuracies Vector of positional accuracy (might be empty).
 * @return the new instance
 */
SingleOperationNNPtr SingleOperation::createPROJBased(
    const util::PropertyMap &properties, const std::string &PROJString,
    const crs::CRSPtr &sourceCRS, const crs::CRSPtr &targetCRS,
    const std::vector<metadata::PositionalAccuracyNNPtr> &accuracies) {
    return util::nn_static_pointer_cast<SingleOperation>(
        PROJBasedOperation::create(properties, PROJString, sourceCRS, targetCRS,
                                   accuracies));
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
static SingleOperationNNPtr createPROJBased(
    const util::PropertyMap &properties,
    const io::IPROJStringExportableNNPtr &projExportable,
    const crs::CRSNNPtr &sourceCRS, const crs::CRSNNPtr &targetCRS,
    const crs::CRSPtr &interpolationCRS,
    const std::vector<metadata::PositionalAccuracyNNPtr> &accuracies,
    bool hasBallparkTransformation) {
    return util::nn_static_pointer_cast<SingleOperation>(
        PROJBasedOperation::create(properties, projExportable, false, sourceCRS,
                                   targetCRS, interpolationCRS, accuracies,
                                   hasBallparkTransformation));
}
//! @endcond

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
bool SingleOperation::_isEquivalentTo(
    const util::IComparable *other, util::IComparable::Criterion criterion,
    const io::DatabaseContextPtr &dbContext) const {
    return _isEquivalentTo(other, criterion, dbContext, false);
}

bool SingleOperation::_isEquivalentTo(const util::IComparable *other,
                                      util::IComparable::Criterion criterion,
                                      const io::DatabaseContextPtr &dbContext,
                                      bool inOtherDirection) const {

    auto otherSO = dynamic_cast<const SingleOperation *>(other);
    if (otherSO == nullptr ||
        (criterion == util::IComparable::Criterion::STRICT &&
         !ObjectUsage::_isEquivalentTo(other, criterion, dbContext))) {
        return false;
    }

    const int methodEPSGCode = d->method_->getEPSGCode();
    const int otherMethodEPSGCode = otherSO->d->method_->getEPSGCode();

    bool equivalentMethods =
        (criterion == util::IComparable::Criterion::EQUIVALENT &&
         methodEPSGCode != 0 && methodEPSGCode == otherMethodEPSGCode) ||
        d->method_->_isEquivalentTo(otherSO->d->method_.get(), criterion,
                                    dbContext);
    if (!equivalentMethods &&
        criterion == util::IComparable::Criterion::EQUIVALENT) {
        if ((methodEPSGCode == EPSG_CODE_METHOD_LAMBERT_AZIMUTHAL_EQUAL_AREA &&
             otherMethodEPSGCode ==
                 EPSG_CODE_METHOD_LAMBERT_AZIMUTHAL_EQUAL_AREA_SPHERICAL) ||
            (otherMethodEPSGCode ==
                 EPSG_CODE_METHOD_LAMBERT_AZIMUTHAL_EQUAL_AREA &&
             methodEPSGCode ==
                 EPSG_CODE_METHOD_LAMBERT_AZIMUTHAL_EQUAL_AREA_SPHERICAL) ||
            (methodEPSGCode ==
                 EPSG_CODE_METHOD_LAMBERT_CYLINDRICAL_EQUAL_AREA &&
             otherMethodEPSGCode ==
                 EPSG_CODE_METHOD_LAMBERT_CYLINDRICAL_EQUAL_AREA_SPHERICAL) ||
            (otherMethodEPSGCode ==
                 EPSG_CODE_METHOD_LAMBERT_CYLINDRICAL_EQUAL_AREA &&
             methodEPSGCode ==
                 EPSG_CODE_METHOD_LAMBERT_CYLINDRICAL_EQUAL_AREA_SPHERICAL) ||
            (methodEPSGCode == EPSG_CODE_METHOD_EQUIDISTANT_CYLINDRICAL &&
             otherMethodEPSGCode ==
                 EPSG_CODE_METHOD_EQUIDISTANT_CYLINDRICAL_SPHERICAL) ||
            (otherMethodEPSGCode == EPSG_CODE_METHOD_EQUIDISTANT_CYLINDRICAL &&
             methodEPSGCode ==
                 EPSG_CODE_METHOD_EQUIDISTANT_CYLINDRICAL_SPHERICAL)) {
            auto geodCRS =
                dynamic_cast<const crs::GeodeticCRS *>(sourceCRS().get());
            auto otherGeodCRS = dynamic_cast<const crs::GeodeticCRS *>(
                otherSO->sourceCRS().get());
            if (geodCRS && otherGeodCRS && geodCRS->ellipsoid()->isSphere() &&
                otherGeodCRS->ellipsoid()->isSphere()) {
                equivalentMethods = true;
            }
        }
    }

    if (!equivalentMethods) {
        if (criterion == util::IComparable::Criterion::EQUIVALENT) {

            const auto isTOWGS84Transf = [](int code) {
                return code ==
                           EPSG_CODE_METHOD_GEOCENTRIC_TRANSLATION_GEOCENTRIC ||
                       code == EPSG_CODE_METHOD_POSITION_VECTOR_GEOCENTRIC ||
                       code == EPSG_CODE_METHOD_COORDINATE_FRAME_GEOCENTRIC ||
                       code ==
                           EPSG_CODE_METHOD_GEOCENTRIC_TRANSLATION_GEOGRAPHIC_2D ||
                       code == EPSG_CODE_METHOD_POSITION_VECTOR_GEOGRAPHIC_2D ||
                       code ==
                           EPSG_CODE_METHOD_COORDINATE_FRAME_GEOGRAPHIC_2D ||
                       code ==
                           EPSG_CODE_METHOD_GEOCENTRIC_TRANSLATION_GEOGRAPHIC_3D ||
                       code == EPSG_CODE_METHOD_POSITION_VECTOR_GEOGRAPHIC_3D ||
                       code == EPSG_CODE_METHOD_COORDINATE_FRAME_GEOGRAPHIC_3D;
            };

            // Translation vs (PV or CF)
            // or different PV vs CF convention
            if (isTOWGS84Transf(methodEPSGCode) &&
                isTOWGS84Transf(otherMethodEPSGCode)) {
                auto transf = static_cast<const Transformation *>(this);
                auto otherTransf = static_cast<const Transformation *>(otherSO);
                auto params = transf->getTOWGS84Parameters();
                auto otherParams = otherTransf->getTOWGS84Parameters();
                assert(params.size() == 7);
                assert(otherParams.size() == 7);
                for (size_t i = 0; i < 7; i++) {
                    if (std::fabs(params[i] - otherParams[i]) >
                        1e-10 * std::fabs(params[i])) {
                        return false;
                    }
                }
                return true;
            }

            // _1SP methods can sometimes be equivalent to _2SP ones
            // Check it by using convertToOtherMethod()
            if (methodEPSGCode ==
                    EPSG_CODE_METHOD_LAMBERT_CONIC_CONFORMAL_1SP &&
                otherMethodEPSGCode ==
                    EPSG_CODE_METHOD_LAMBERT_CONIC_CONFORMAL_2SP) {
                // Convert from 2SP to 1SP as the other direction has more
                // degree of liberties.
                return otherSO->_isEquivalentTo(this, criterion, dbContext);
            } else if ((methodEPSGCode == EPSG_CODE_METHOD_MERCATOR_VARIANT_A &&
                        otherMethodEPSGCode ==
                            EPSG_CODE_METHOD_MERCATOR_VARIANT_B) ||
                       (methodEPSGCode == EPSG_CODE_METHOD_MERCATOR_VARIANT_B &&
                        otherMethodEPSGCode ==
                            EPSG_CODE_METHOD_MERCATOR_VARIANT_A) ||
                       (methodEPSGCode ==
                            EPSG_CODE_METHOD_LAMBERT_CONIC_CONFORMAL_2SP &&
                        otherMethodEPSGCode ==
                            EPSG_CODE_METHOD_LAMBERT_CONIC_CONFORMAL_1SP)) {
                auto conv = dynamic_cast<const Conversion *>(this);
                if (conv) {
                    auto eqConv =
                        conv->convertToOtherMethod(otherMethodEPSGCode);
                    if (eqConv) {
                        return eqConv->_isEquivalentTo(other, criterion,
                                                       dbContext);
                    }
                }
            }
        }

        return false;
    }

    const auto &values = d->parameterValues_;
    const auto &otherValues = otherSO->d->parameterValues_;
    const auto valuesSize = values.size();
    const auto otherValuesSize = otherValues.size();
    if (criterion == util::IComparable::Criterion::STRICT) {
        if (valuesSize != otherValuesSize) {
            return false;
        }
        for (size_t i = 0; i < valuesSize; i++) {
            if (!values[i]->_isEquivalentTo(otherValues[i].get(), criterion,
                                            dbContext)) {
                return false;
            }
        }
        return true;
    }

    std::vector<bool> candidateIndices(otherValuesSize, true);
    bool equivalent = true;
    bool foundMissingArgs = valuesSize != otherValuesSize;

    for (size_t i = 0; equivalent && i < valuesSize; i++) {
        auto opParamvalue =
            dynamic_cast<const OperationParameterValue *>(values[i].get());
        if (!opParamvalue)
            return false;

        equivalent = false;
        bool sameNameDifferentValue = false;
        for (size_t j = 0; j < otherValuesSize; j++) {
            if (candidateIndices[j] &&
                values[i]->_isEquivalentTo(otherValues[j].get(), criterion,
                                           dbContext)) {
                candidateIndices[j] = false;
                equivalent = true;
                break;
            } else if (candidateIndices[j]) {
                auto otherOpParamvalue =
                    dynamic_cast<const OperationParameterValue *>(
                        otherValues[j].get());
                if (!otherOpParamvalue)
                    return false;
                sameNameDifferentValue =
                    opParamvalue->parameter()->_isEquivalentTo(
                        otherOpParamvalue->parameter().get(), criterion,
                        dbContext);
                if (sameNameDifferentValue) {
                    candidateIndices[j] = false;
                    break;
                }
            }
        }

        if (!equivalent &&
            methodEPSGCode == EPSG_CODE_METHOD_LAMBERT_CONIC_CONFORMAL_2SP) {
            // For LCC_2SP, the standard parallels can be switched and
            // this will result in the same result.
            const int paramEPSGCode = opParamvalue->parameter()->getEPSGCode();
            if (paramEPSGCode ==
                    EPSG_CODE_PARAMETER_LATITUDE_1ST_STD_PARALLEL ||
                paramEPSGCode ==
                    EPSG_CODE_PARAMETER_LATITUDE_2ND_STD_PARALLEL) {
                auto value_1st = parameterValue(
                    EPSG_CODE_PARAMETER_LATITUDE_1ST_STD_PARALLEL);
                auto value_2nd = parameterValue(
                    EPSG_CODE_PARAMETER_LATITUDE_2ND_STD_PARALLEL);
                if (value_1st && value_2nd) {
                    equivalent =
                        value_1st->_isEquivalentTo(
                            otherSO
                                ->parameterValue(
                                    EPSG_CODE_PARAMETER_LATITUDE_2ND_STD_PARALLEL)
                                .get(),
                            criterion, dbContext) &&
                        value_2nd->_isEquivalentTo(
                            otherSO
                                ->parameterValue(
                                    EPSG_CODE_PARAMETER_LATITUDE_1ST_STD_PARALLEL)
                                .get(),
                            criterion, dbContext);
                }
            }
        }

        if (equivalent) {
            continue;
        }

        if (sameNameDifferentValue) {
            break;
        }

        // If there are parameters in this method not found in the other one,
        // check that they are set to a default neutral value, that is 1
        // for scale, and 0 otherwise.
        foundMissingArgs = true;
        const auto &value = opParamvalue->parameterValue();
        if (value->type() != ParameterValue::Type::MEASURE) {
            break;
        }
        if (value->value().unit().type() ==
            common::UnitOfMeasure::Type::SCALE) {
            equivalent = value->value().getSIValue() == 1.0;
        } else {
            equivalent = value->value().getSIValue() == 0.0;
        }
    }

    // In the case the arguments don't perfectly match, try the reverse
    // check.
    if (equivalent && foundMissingArgs && !inOtherDirection) {
        return otherSO->_isEquivalentTo(this, criterion, dbContext, true);
    }

    // Equivalent formulations of 2SP can have different parameters
    // Then convert to 1SP and compare.
    if (!equivalent &&
        methodEPSGCode == EPSG_CODE_METHOD_LAMBERT_CONIC_CONFORMAL_2SP) {
        auto conv = dynamic_cast<const Conversion *>(this);
        auto otherConv = dynamic_cast<const Conversion *>(other);
        if (conv && otherConv) {
            auto thisAs1SP = conv->convertToOtherMethod(
                EPSG_CODE_METHOD_LAMBERT_CONIC_CONFORMAL_1SP);
            auto otherAs1SP = otherConv->convertToOtherMethod(
                EPSG_CODE_METHOD_LAMBERT_CONIC_CONFORMAL_1SP);
            if (thisAs1SP && otherAs1SP) {
                equivalent = thisAs1SP->_isEquivalentTo(otherAs1SP.get(),
                                                        criterion, dbContext);
            }
        }
    }
    return equivalent;
}
//! @endcond

// ---------------------------------------------------------------------------

std::set<GridDescription>
SingleOperation::gridsNeeded(const io::DatabaseContextPtr &databaseContext,
                             bool considerKnownGridsAsAvailable) const {
    std::set<GridDescription> res;
    for (const auto &genOpParamvalue : parameterValues()) {
        auto opParamvalue = dynamic_cast<const OperationParameterValue *>(
            genOpParamvalue.get());
        if (opParamvalue) {
            const auto &value = opParamvalue->parameterValue();
            if (value->type() == ParameterValue::Type::FILENAME) {
                const auto gridNames = split(value->valueFile(), ",");
                for (const auto &gridName : gridNames) {
                    GridDescription desc;
                    desc.shortName = gridName;
                    if (databaseContext) {
                        databaseContext->lookForGridInfo(
                            desc.shortName, considerKnownGridsAsAvailable,
                            desc.fullName, desc.packageName, desc.url,
                            desc.directDownload, desc.openLicense,
                            desc.available);
                    }
                    res.insert(desc);
                }
            }
        }
    }
    return res;
}

// ---------------------------------------------------------------------------

/** \brief Validate the parameters used by a coordinate operation.
 *
 * Return whether the method is known or not, or a list of missing or extra
 * parameters for the operations recognized by this implementation.
 */
std::list<std::string> SingleOperation::validateParameters() const {
    std::list<std::string> res;

    const auto &l_method = method();
    const auto &methodName = l_method->nameStr();
    const MethodMapping *methodMapping = nullptr;
    const auto methodEPSGCode = l_method->getEPSGCode();
    for (const auto &mapping : projectionMethodMappings) {
        if (metadata::Identifier::isEquivalentName(mapping.wkt2_name,
                                                   methodName.c_str()) ||
            (methodEPSGCode != 0 && methodEPSGCode == mapping.epsg_code)) {
            methodMapping = &mapping;
        }
    }
    if (methodMapping == nullptr) {
        for (const auto &mapping : otherMethodMappings) {
            if (metadata::Identifier::isEquivalentName(mapping.wkt2_name,
                                                       methodName.c_str()) ||
                (methodEPSGCode != 0 && methodEPSGCode == mapping.epsg_code)) {
                methodMapping = &mapping;
            }
        }
    }
    if (!methodMapping) {
        res.emplace_back("Unknown method " + methodName);
        return res;
    }
    if (methodMapping->wkt2_name != methodName) {
        if (metadata::Identifier::isEquivalentName(methodMapping->wkt2_name,
                                                   methodName.c_str())) {
            std::string msg("Method name ");
            msg += methodName;
            msg += " is equivalent to official ";
            msg += methodMapping->wkt2_name;
            msg += " but not strictly equal";
            res.emplace_back(msg);
        } else {
            std::string msg("Method name ");
            msg += methodName;
            msg += ", matched to ";
            msg += methodMapping->wkt2_name;
            msg += ", through its EPSG code has not an equivalent name";
            res.emplace_back(msg);
        }
    }
    if (methodEPSGCode != 0 && methodEPSGCode != methodMapping->epsg_code) {
        std::string msg("Method of EPSG code ");
        msg += toString(methodEPSGCode);
        msg += " does not match official code (";
        msg += toString(methodMapping->epsg_code);
        msg += ')';
        res.emplace_back(msg);
    }

    // Check if expected parameters are found
    for (int i = 0;
         methodMapping->params && methodMapping->params[i] != nullptr; ++i) {
        const auto *paramMapping = methodMapping->params[i];

        const OperationParameterValue *opv = nullptr;
        for (const auto &genOpParamvalue : parameterValues()) {
            auto opParamvalue = dynamic_cast<const OperationParameterValue *>(
                genOpParamvalue.get());
            if (opParamvalue) {
                const auto &parameter = opParamvalue->parameter();
                if ((paramMapping->epsg_code != 0 &&
                     parameter->getEPSGCode() == paramMapping->epsg_code) ||
                    ci_equal(parameter->nameStr(), paramMapping->wkt2_name)) {
                    opv = opParamvalue;
                    break;
                }
            }
        }

        if (!opv) {
            if ((methodEPSGCode == EPSG_CODE_METHOD_EQUIDISTANT_CYLINDRICAL ||
                 methodEPSGCode ==
                     EPSG_CODE_METHOD_EQUIDISTANT_CYLINDRICAL_SPHERICAL) &&
                paramMapping == &paramLatitudeNatOrigin) {
                // extension of EPSG used by GDAL/PROJ, so we should not
                // warn on its absence.
                continue;
            }
            std::string msg("Cannot find expected parameter ");
            msg += paramMapping->wkt2_name;
            res.emplace_back(msg);
            continue;
        }
        const auto &parameter = opv->parameter();
        if (paramMapping->wkt2_name != parameter->nameStr()) {
            if (ci_equal(parameter->nameStr(), paramMapping->wkt2_name)) {
                std::string msg("Parameter name ");
                msg += parameter->nameStr();
                msg += " is equivalent to official ";
                msg += paramMapping->wkt2_name;
                msg += " but not strictly equal";
                res.emplace_back(msg);
            } else {
                std::string msg("Parameter name ");
                msg += parameter->nameStr();
                msg += ", matched to ";
                msg += paramMapping->wkt2_name;
                msg += ", through its EPSG code has not an equivalent name";
                res.emplace_back(msg);
            }
        }
        const auto paramEPSGCode = parameter->getEPSGCode();
        if (paramEPSGCode != 0 && paramEPSGCode != paramMapping->epsg_code) {
            std::string msg("Parameter of EPSG code ");
            msg += toString(paramEPSGCode);
            msg += " does not match official code (";
            msg += toString(paramMapping->epsg_code);
            msg += ')';
            res.emplace_back(msg);
        }
    }

    // Check if there are extra parameters
    for (const auto &genOpParamvalue : parameterValues()) {
        auto opParamvalue = dynamic_cast<const OperationParameterValue *>(
            genOpParamvalue.get());
        if (opParamvalue) {
            const auto &parameter = opParamvalue->parameter();
            if (!getMapping(methodMapping, parameter)) {
                std::string msg("Parameter ");
                msg += parameter->nameStr();
                msg += " found but not expected for this method";
                res.emplace_back(msg);
            }
        }
    }

    return res;
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
struct ParameterValue::Private {
    ParameterValue::Type type_{ParameterValue::Type::STRING};
    std::unique_ptr<common::Measure> measure_{};
    std::unique_ptr<std::string> stringValue_{};
    int integerValue_{};
    bool booleanValue_{};

    explicit Private(const common::Measure &valueIn)
        : type_(ParameterValue::Type::MEASURE),
          measure_(internal::make_unique<common::Measure>(valueIn)) {}

    Private(const std::string &stringValueIn, ParameterValue::Type typeIn)
        : type_(typeIn),
          stringValue_(internal::make_unique<std::string>(stringValueIn)) {}

    explicit Private(int integerValueIn)
        : type_(ParameterValue::Type::INTEGER), integerValue_(integerValueIn) {}

    explicit Private(bool booleanValueIn)
        : type_(ParameterValue::Type::BOOLEAN), booleanValue_(booleanValueIn) {}
};
//! @endcond

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
ParameterValue::~ParameterValue() = default;
//! @endcond

// ---------------------------------------------------------------------------

ParameterValue::ParameterValue(const common::Measure &measureIn)
    : d(internal::make_unique<Private>(measureIn)) {}

// ---------------------------------------------------------------------------

ParameterValue::ParameterValue(const std::string &stringValueIn,
                               ParameterValue::Type typeIn)
    : d(internal::make_unique<Private>(stringValueIn, typeIn)) {}

// ---------------------------------------------------------------------------

ParameterValue::ParameterValue(int integerValueIn)
    : d(internal::make_unique<Private>(integerValueIn)) {}

// ---------------------------------------------------------------------------

ParameterValue::ParameterValue(bool booleanValueIn)
    : d(internal::make_unique<Private>(booleanValueIn)) {}

// ---------------------------------------------------------------------------

/** \brief Instantiate a ParameterValue from a Measure (i.e. a value associated
 * with a
 * unit)
 *
 * @return a new ParameterValue.
 */
ParameterValueNNPtr ParameterValue::create(const common::Measure &measureIn) {
    return ParameterValue::nn_make_shared<ParameterValue>(measureIn);
}

// ---------------------------------------------------------------------------

/** \brief Instantiate a ParameterValue from a string value.
 *
 * @return a new ParameterValue.
 */
ParameterValueNNPtr ParameterValue::create(const char *stringValueIn) {
    return ParameterValue::nn_make_shared<ParameterValue>(
        std::string(stringValueIn), ParameterValue::Type::STRING);
}

// ---------------------------------------------------------------------------

/** \brief Instantiate a ParameterValue from a string value.
 *
 * @return a new ParameterValue.
 */
ParameterValueNNPtr ParameterValue::create(const std::string &stringValueIn) {
    return ParameterValue::nn_make_shared<ParameterValue>(
        stringValueIn, ParameterValue::Type::STRING);
}

// ---------------------------------------------------------------------------

/** \brief Instantiate a ParameterValue from a filename.
 *
 * @return a new ParameterValue.
 */
ParameterValueNNPtr
ParameterValue::createFilename(const std::string &stringValueIn) {
    return ParameterValue::nn_make_shared<ParameterValue>(
        stringValueIn, ParameterValue::Type::FILENAME);
}

// ---------------------------------------------------------------------------

/** \brief Instantiate a ParameterValue from a integer value.
 *
 * @return a new ParameterValue.
 */
ParameterValueNNPtr ParameterValue::create(int integerValueIn) {
    return ParameterValue::nn_make_shared<ParameterValue>(integerValueIn);
}

// ---------------------------------------------------------------------------

/** \brief Instantiate a ParameterValue from a boolean value.
 *
 * @return a new ParameterValue.
 */
ParameterValueNNPtr ParameterValue::create(bool booleanValueIn) {
    return ParameterValue::nn_make_shared<ParameterValue>(booleanValueIn);
}

// ---------------------------------------------------------------------------

/** \brief Returns the type of a parameter value.
 *
 * @return the type.
 */
const ParameterValue::Type &ParameterValue::type() PROJ_PURE_DEFN {
    return d->type_;
}

// ---------------------------------------------------------------------------

/** \brief Returns the value as a Measure (assumes type() == Type::MEASURE)
 * @return the value as a Measure.
 */
const common::Measure &ParameterValue::value() PROJ_PURE_DEFN {
    return *d->measure_;
}

// ---------------------------------------------------------------------------

/** \brief Returns the value as a string (assumes type() == Type::STRING)
 * @return the value as a string.
 */
const std::string &ParameterValue::stringValue() PROJ_PURE_DEFN {
    return *d->stringValue_;
}

// ---------------------------------------------------------------------------

/** \brief Returns the value as a filename (assumes type() == Type::FILENAME)
 * @return the value as a filename.
 */
const std::string &ParameterValue::valueFile() PROJ_PURE_DEFN {
    return *d->stringValue_;
}

// ---------------------------------------------------------------------------

/** \brief Returns the value as a integer (assumes type() == Type::INTEGER)
 * @return the value as a integer.
 */
int ParameterValue::integerValue() PROJ_PURE_DEFN { return d->integerValue_; }

// ---------------------------------------------------------------------------

/** \brief Returns the value as a boolean (assumes type() == Type::BOOLEAN)
 * @return the value as a boolean.
 */
bool ParameterValue::booleanValue() PROJ_PURE_DEFN { return d->booleanValue_; }

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
void ParameterValue::_exportToWKT(io::WKTFormatter *formatter) const {
    const bool isWKT2 = formatter->version() == io::WKTFormatter::Version::WKT2;

    const auto &l_type = type();
    if (l_type == Type::MEASURE) {
        const auto &l_value = value();
        if (formatter->abridgedTransformation()) {
            const auto &unit = l_value.unit();
            const auto &unitType = unit.type();
            if (unitType == common::UnitOfMeasure::Type::LINEAR) {
                formatter->add(l_value.getSIValue());
            } else if (unitType == common::UnitOfMeasure::Type::ANGULAR) {
                formatter->add(
                    l_value.convertToUnit(common::UnitOfMeasure::ARC_SECOND));
            } else if (unit == common::UnitOfMeasure::PARTS_PER_MILLION) {
                formatter->add(1.0 + l_value.value() * 1e-6);
            } else {
                formatter->add(l_value.value());
            }
        } else {
            const auto &unit = l_value.unit();
            if (isWKT2) {
                formatter->add(l_value.value());
            } else {
                // In WKT1, as we don't output the natural unit, output to the
                // registered linear / angular unit.
                const auto &unitType = unit.type();
                if (unitType == common::UnitOfMeasure::Type::LINEAR) {
                    const auto &targetUnit = *(formatter->axisLinearUnit());
                    if (targetUnit.conversionToSI() == 0.0) {
                        throw io::FormattingException(
                            "cannot convert value to target linear unit");
                    }
                    formatter->add(l_value.convertToUnit(targetUnit));
                } else if (unitType == common::UnitOfMeasure::Type::ANGULAR) {
                    const auto &targetUnit = *(formatter->axisAngularUnit());
                    if (targetUnit.conversionToSI() == 0.0) {
                        throw io::FormattingException(
                            "cannot convert value to target angular unit");
                    }
                    formatter->add(l_value.convertToUnit(targetUnit));
                } else {
                    formatter->add(l_value.getSIValue());
                }
            }
            if (isWKT2 && unit != common::UnitOfMeasure::NONE) {
                if (!formatter
                         ->primeMeridianOrParameterUnitOmittedIfSameAsAxis() ||
                    (unit != common::UnitOfMeasure::SCALE_UNITY &&
                     unit != *(formatter->axisLinearUnit()) &&
                     unit != *(formatter->axisAngularUnit()))) {
                    unit._exportToWKT(formatter);
                }
            }
        }
    } else if (l_type == Type::STRING || l_type == Type::FILENAME) {
        formatter->addQuotedString(stringValue());
    } else if (l_type == Type::INTEGER) {
        formatter->add(integerValue());
    } else {
        throw io::FormattingException("boolean parameter value not handled");
    }
}
//! @endcond

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
bool ParameterValue::_isEquivalentTo(const util::IComparable *other,
                                     util::IComparable::Criterion criterion,
                                     const io::DatabaseContextPtr &) const {
    auto otherPV = dynamic_cast<const ParameterValue *>(other);
    if (otherPV == nullptr) {
        return false;
    }
    if (type() != otherPV->type()) {
        return false;
    }
    switch (type()) {
    case Type::MEASURE: {
        return value()._isEquivalentTo(otherPV->value(), criterion, 2e-10);
    }

    case Type::STRING:
    case Type::FILENAME: {
        return stringValue() == otherPV->stringValue();
    }

    case Type::INTEGER: {
        return integerValue() == otherPV->integerValue();
    }

    case Type::BOOLEAN: {
        return booleanValue() == otherPV->booleanValue();
    }

    default: {
        assert(false);
        break;
    }
    }
    return true;
}
//! @endcond

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
struct Conversion::Private {};
//! @endcond

// ---------------------------------------------------------------------------

Conversion::Conversion(const OperationMethodNNPtr &methodIn,
                       const std::vector<GeneralParameterValueNNPtr> &values)
    : SingleOperation(methodIn), d(nullptr) {
    setParameterValues(values);
}

// ---------------------------------------------------------------------------

Conversion::Conversion(const Conversion &other)
    : CoordinateOperation(other), SingleOperation(other), d(nullptr) {}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
Conversion::~Conversion() = default;
//! @endcond

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
ConversionNNPtr Conversion::shallowClone() const {
    auto conv = Conversion::nn_make_shared<Conversion>(*this);
    conv->assignSelf(conv);
    conv->setCRSs(this, false);
    return conv;
}

CoordinateOperationNNPtr Conversion::_shallowClone() const {
    return util::nn_static_pointer_cast<CoordinateOperation>(shallowClone());
}
//! @endcond

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
ConversionNNPtr
Conversion::alterParametersLinearUnit(const common::UnitOfMeasure &unit,
                                      bool convertToNewUnit) const {

    std::vector<GeneralParameterValueNNPtr> newValues;
    bool changesDone = false;
    for (const auto &genOpParamvalue : parameterValues()) {
        bool updated = false;
        auto opParamvalue = dynamic_cast<const OperationParameterValue *>(
            genOpParamvalue.get());
        if (opParamvalue) {
            const auto &paramValue = opParamvalue->parameterValue();
            if (paramValue->type() == ParameterValue::Type::MEASURE) {
                const auto &measure = paramValue->value();
                if (measure.unit().type() ==
                    common::UnitOfMeasure::Type::LINEAR) {
                    if (!measure.unit()._isEquivalentTo(
                            unit, util::IComparable::Criterion::EQUIVALENT)) {
                        const double newValue =
                            convertToNewUnit ? measure.convertToUnit(unit)
                                             : measure.value();
                        newValues.emplace_back(OperationParameterValue::create(
                            opParamvalue->parameter(),
                            ParameterValue::create(
                                common::Measure(newValue, unit))));
                        updated = true;
                    }
                }
            }
        }
        if (updated) {
            changesDone = true;
        } else {
            newValues.emplace_back(genOpParamvalue);
        }
    }
    if (changesDone) {
        auto conv = create(util::PropertyMap().set(
                               common::IdentifiedObject::NAME_KEY, "unknown"),
                           method(), newValues);
        conv->setCRSs(this, false);
        return conv;
    } else {
        return NN_NO_CHECK(
            util::nn_dynamic_pointer_cast<Conversion>(shared_from_this()));
    }
}
//! @endcond

// ---------------------------------------------------------------------------

/** \brief Instantiate a Conversion from a vector of GeneralParameterValue.
 *
 * @param properties See \ref general_properties. At minimum the name should be
 * defined.
 * @param methodIn the operation method.
 * @param values the values.
 * @return a new Conversion.
 * @throws InvalidOperation
 */
ConversionNNPtr Conversion::create(const util::PropertyMap &properties,
                                   const OperationMethodNNPtr &methodIn,
                                   const std::vector<GeneralParameterValueNNPtr>
                                       &values) // throw InvalidOperation
{
    if (methodIn->parameters().size() != values.size()) {
        throw InvalidOperation(
            "Inconsistent number of parameters and parameter values");
    }
    auto conv = Conversion::nn_make_shared<Conversion>(methodIn, values);
    conv->assignSelf(conv);
    conv->setProperties(properties);
    return conv;
}

// ---------------------------------------------------------------------------

/** \brief Instantiate a Conversion and its OperationMethod
 *
 * @param propertiesConversion See \ref general_properties of the conversion.
 * At minimum the name should be defined.
 * @param propertiesOperationMethod See \ref general_properties of the operation
 * method. At minimum the name should be defined.
 * @param parameters the operation parameters.
 * @param values the operation values. Constraint:
 * values.size() == parameters.size()
 * @return a new Conversion.
 * @throws InvalidOperation
 */
ConversionNNPtr Conversion::create(
    const util::PropertyMap &propertiesConversion,
    const util::PropertyMap &propertiesOperationMethod,
    const std::vector<OperationParameterNNPtr> &parameters,
    const std::vector<ParameterValueNNPtr> &values) // throw InvalidOperation
{
    OperationMethodNNPtr op(
        OperationMethod::create(propertiesOperationMethod, parameters));

    if (parameters.size() != values.size()) {
        throw InvalidOperation(
            "Inconsistent number of parameters and parameter values");
    }
    std::vector<GeneralParameterValueNNPtr> generalParameterValues;
    generalParameterValues.reserve(values.size());
    for (size_t i = 0; i < values.size(); i++) {
        generalParameterValues.push_back(
            OperationParameterValue::create(parameters[i], values[i]));
    }
    return create(propertiesConversion, op, generalParameterValues);
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress

// ---------------------------------------------------------------------------

static util::PropertyMap createMapNameEPSGCode(const std::string &name,
                                               int code) {
    return util::PropertyMap()
        .set(common::IdentifiedObject::NAME_KEY, name)
        .set(metadata::Identifier::CODESPACE_KEY, metadata::Identifier::EPSG)
        .set(metadata::Identifier::CODE_KEY, code);
}

// ---------------------------------------------------------------------------

static util::PropertyMap createMapNameEPSGCode(const char *name, int code) {
    return util::PropertyMap()
        .set(common::IdentifiedObject::NAME_KEY, name)
        .set(metadata::Identifier::CODESPACE_KEY, metadata::Identifier::EPSG)
        .set(metadata::Identifier::CODE_KEY, code);
}

// ---------------------------------------------------------------------------

static util::PropertyMap createMethodMapNameEPSGCode(int code) {
    const char *name = nullptr;
    for (const auto &tuple : methodNameCodes) {
        if (tuple.epsg_code == code) {
            name = tuple.name;
            break;
        }
    }
    assert(name);
    return createMapNameEPSGCode(name, code);
}

// ---------------------------------------------------------------------------

static util::PropertyMap
getUTMConversionProperty(const util::PropertyMap &properties, int zone,
                         bool north) {
    if (!properties.get(common::IdentifiedObject::NAME_KEY)) {
        std::string conversionName("UTM zone ");
        conversionName += toString(zone);
        conversionName += (north ? 'N' : 'S');

        return createMapNameEPSGCode(conversionName,
                                     (north ? 16000 : 17000) + zone);
    } else {
        return properties;
    }
}

// ---------------------------------------------------------------------------

static util::PropertyMap
addDefaultNameIfNeeded(const util::PropertyMap &properties,
                       const std::string &defaultName) {
    if (!properties.get(common::IdentifiedObject::NAME_KEY)) {
        return util::PropertyMap(properties)
            .set(common::IdentifiedObject::NAME_KEY, defaultName);
    } else {
        return properties;
    }
}

// ---------------------------------------------------------------------------

static ConversionNNPtr
createConversion(const util::PropertyMap &properties,
                 const MethodMapping *mapping,
                 const std::vector<ParameterValueNNPtr> &values) {

    std::vector<OperationParameterNNPtr> parameters;
    for (int i = 0; mapping->params[i] != nullptr; i++) {
        const auto *param = mapping->params[i];
        auto paramProperties = util::PropertyMap().set(
            common::IdentifiedObject::NAME_KEY, param->wkt2_name);
        if (param->epsg_code != 0) {
            paramProperties
                .set(metadata::Identifier::CODESPACE_KEY,
                     metadata::Identifier::EPSG)
                .set(metadata::Identifier::CODE_KEY, param->epsg_code);
        }
        auto parameter = OperationParameter::create(paramProperties);
        parameters.push_back(parameter);
    }

    auto methodProperties = util::PropertyMap().set(
        common::IdentifiedObject::NAME_KEY, mapping->wkt2_name);
    if (mapping->epsg_code != 0) {
        methodProperties
            .set(metadata::Identifier::CODESPACE_KEY,
                 metadata::Identifier::EPSG)
            .set(metadata::Identifier::CODE_KEY, mapping->epsg_code);
    }
    return Conversion::create(
        addDefaultNameIfNeeded(properties, mapping->wkt2_name),
        methodProperties, parameters, values);
}
//! @endcond

// ---------------------------------------------------------------------------

ConversionNNPtr
Conversion::create(const util::PropertyMap &properties, int method_epsg_code,
                   const std::vector<ParameterValueNNPtr> &values) {
    const MethodMapping *mapping = getMapping(method_epsg_code);
    assert(mapping);
    return createConversion(properties, mapping, values);
}

// ---------------------------------------------------------------------------

ConversionNNPtr
Conversion::create(const util::PropertyMap &properties,
                   const char *method_wkt2_name,
                   const std::vector<ParameterValueNNPtr> &values) {
    const MethodMapping *mapping = getMapping(method_wkt2_name);
    assert(mapping);
    return createConversion(properties, mapping, values);
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress

struct VectorOfParameters : public std::vector<OperationParameterNNPtr> {
    VectorOfParameters() : std::vector<OperationParameterNNPtr>() {}
    explicit VectorOfParameters(
        std::initializer_list<OperationParameterNNPtr> list)
        : std::vector<OperationParameterNNPtr>(list) {}
    VectorOfParameters(const VectorOfParameters &) = delete;

    ~VectorOfParameters();
};

// This way, we disable inlining of destruction, and save a lot of space
VectorOfParameters::~VectorOfParameters() = default;

struct VectorOfValues : public std::vector<ParameterValueNNPtr> {
    VectorOfValues() : std::vector<ParameterValueNNPtr>() {}
    explicit VectorOfValues(std::initializer_list<ParameterValueNNPtr> list)
        : std::vector<ParameterValueNNPtr>(list) {}

    explicit VectorOfValues(std::initializer_list<common::Measure> list);
    VectorOfValues(const VectorOfValues &) = delete;
    VectorOfValues(VectorOfValues &&) = default;

    ~VectorOfValues();
};

static std::vector<ParameterValueNNPtr> buildParameterValueFromMeasure(
    const std::initializer_list<common::Measure> &list) {
    std::vector<ParameterValueNNPtr> res;
    for (const auto &v : list) {
        res.emplace_back(ParameterValue::create(v));
    }
    return res;
}

VectorOfValues::VectorOfValues(std::initializer_list<common::Measure> list)
    : std::vector<ParameterValueNNPtr>(buildParameterValueFromMeasure(list)) {}

// This way, we disable inlining of destruction, and save a lot of space
VectorOfValues::~VectorOfValues() = default;

PROJ_NO_INLINE static VectorOfValues createParams(const common::Measure &m1,
                                                  const common::Measure &m2,
                                                  const common::Measure &m3) {
    return VectorOfValues{ParameterValue::create(m1),
                          ParameterValue::create(m2),
                          ParameterValue::create(m3)};
}

PROJ_NO_INLINE static VectorOfValues createParams(const common::Measure &m1,
                                                  const common::Measure &m2,
                                                  const common::Measure &m3,
                                                  const common::Measure &m4) {
    return VectorOfValues{
        ParameterValue::create(m1), ParameterValue::create(m2),
        ParameterValue::create(m3), ParameterValue::create(m4)};
}

PROJ_NO_INLINE static VectorOfValues createParams(const common::Measure &m1,
                                                  const common::Measure &m2,
                                                  const common::Measure &m3,
                                                  const common::Measure &m4,
                                                  const common::Measure &m5) {
    return VectorOfValues{
        ParameterValue::create(m1), ParameterValue::create(m2),
        ParameterValue::create(m3), ParameterValue::create(m4),
        ParameterValue::create(m5),
    };
}

PROJ_NO_INLINE static VectorOfValues
createParams(const common::Measure &m1, const common::Measure &m2,
             const common::Measure &m3, const common::Measure &m4,
             const common::Measure &m5, const common::Measure &m6) {
    return VectorOfValues{
        ParameterValue::create(m1), ParameterValue::create(m2),
        ParameterValue::create(m3), ParameterValue::create(m4),
        ParameterValue::create(m5), ParameterValue::create(m6),
    };
}

PROJ_NO_INLINE static VectorOfValues
createParams(const common::Measure &m1, const common::Measure &m2,
             const common::Measure &m3, const common::Measure &m4,
             const common::Measure &m5, const common::Measure &m6,
             const common::Measure &m7) {
    return VectorOfValues{
        ParameterValue::create(m1), ParameterValue::create(m2),
        ParameterValue::create(m3), ParameterValue::create(m4),
        ParameterValue::create(m5), ParameterValue::create(m6),
        ParameterValue::create(m7),
    };
}

//! @endcond

// ---------------------------------------------------------------------------

/** \brief Instantiate a [Universal Transverse Mercator]
 *(https://proj.org/operations/projections/utm.html) conversion.
 *
 * UTM is a family of conversions, of EPSG codes from 16001 to 16060 for the
 * northern hemisphere, and 17001 to 17060 for the southern hemisphere,
 * based on the Transverse Mercator projection method.
 *
 * @param properties See \ref general_properties of the conversion. If the name
 * is not provided, it is automatically set.
 * @param zone UTM zone number between 1 and 60.
 * @param north true for UTM northern hemisphere, false for UTM southern
 * hemisphere.
 * @return a new Conversion.
 */
ConversionNNPtr Conversion::createUTM(const util::PropertyMap &properties,
                                      int zone, bool north) {
    return create(
        getUTMConversionProperty(properties, zone, north),
        EPSG_CODE_METHOD_TRANSVERSE_MERCATOR,
        createParams(common::Angle(UTM_LATITUDE_OF_NATURAL_ORIGIN),
                     common::Angle(zone * 6.0 - 183.0),
                     common::Scale(UTM_SCALE_FACTOR),
                     common::Length(UTM_FALSE_EASTING),
                     common::Length(north ? UTM_NORTH_FALSE_NORTHING
                                          : UTM_SOUTH_FALSE_NORTHING)));
}

// ---------------------------------------------------------------------------

/** \brief Instantiate a conversion based on the [Transverse Mercator]
 *(https://proj.org/operations/projections/tmerc.html) projection method.
 *
 * This method is defined as [EPSG:9807]
 * (https://www.epsg-registry.org/export.htm?gml=urn:ogc:def:method:EPSG::9807)
 *
 * @param properties See \ref general_properties of the conversion. If the name
 * is not provided, it is automatically set.
 * @param centerLat See \ref center_latitude
 * @param centerLong See \ref center_longitude
 * @param scale See \ref scale
 * @param falseEasting See \ref false_easting
 * @param falseNorthing See \ref false_northing
 * @return a new Conversion.
 */
ConversionNNPtr Conversion::createTransverseMercator(
    const util::PropertyMap &properties, const common::Angle &centerLat,
    const common::Angle &centerLong, const common::Scale &scale,
    const common::Length &falseEasting, const common::Length &falseNorthing) {
    return create(properties, EPSG_CODE_METHOD_TRANSVERSE_MERCATOR,
                  createParams(centerLat, centerLong, scale, falseEasting,
                               falseNorthing));
}

// ---------------------------------------------------------------------------

/** \brief Instantiate a conversion based on the [Gauss Schreiber Transverse
 *Mercator]
 *(https://proj.org/operations/projections/gstmerc.html) projection method.
 *
 * This method is also known as Gauss-Laborde Reunion.
 *
 * There is no equivalent in EPSG.
 *
 * @param properties See \ref general_properties of the conversion. If the name
 * is not provided, it is automatically set.
 * @param centerLat See \ref center_latitude
 * @param centerLong See \ref center_longitude
 * @param scale See \ref scale
 * @param falseEasting See \ref false_easting
 * @param falseNorthing See \ref false_northing
 * @return a new Conversion.
 */
ConversionNNPtr Conversion::createGaussSchreiberTransverseMercator(
    const util::PropertyMap &properties, const common::Angle &centerLat,
    const common::Angle &centerLong, const common::Scale &scale,
    const common::Length &falseEasting, const common::Length &falseNorthing) {
    return create(properties,
                  PROJ_WKT2_NAME_METHOD_GAUSS_SCHREIBER_TRANSVERSE_MERCATOR,
                  createParams(centerLat, centerLong, scale, falseEasting,
                               falseNorthing));
}

// ---------------------------------------------------------------------------

/** \brief Instantiate a conversion based on the [Transverse Mercator South
 *Orientated]
 *(https://proj.org/operations/projections/tmerc.html) projection method.
 *
 * This method is defined as [EPSG:9808]
 * (https://www.epsg-registry.org/export.htm?gml=urn:ogc:def:method:EPSG::9808)
 *
 * @param properties See \ref general_properties of the conversion. If the name
 * is not provided, it is automatically set.
 * @param centerLat See \ref center_latitude
 * @param centerLong See \ref center_longitude
 * @param scale See \ref scale
 * @param falseEasting See \ref false_easting
 * @param falseNorthing See \ref false_northing
 * @return a new Conversion.
 */
ConversionNNPtr Conversion::createTransverseMercatorSouthOriented(
    const util::PropertyMap &properties, const common::Angle &centerLat,
    const common::Angle &centerLong, const common::Scale &scale,
    const common::Length &falseEasting, const common::Length &falseNorthing) {
    return create(properties,
                  EPSG_CODE_METHOD_TRANSVERSE_MERCATOR_SOUTH_ORIENTATED,
                  createParams(centerLat, centerLong, scale, falseEasting,
                               falseNorthing));
}

// ---------------------------------------------------------------------------

/** \brief Instantiate a conversion based on the  [Two Point Equidistant]
 *(https://proj.org/operations/projections/tpeqd.html) projection method.
 *
 * There is no equivalent in EPSG.
 *
 * @param properties See \ref general_properties of the conversion. If the name
 * is not provided, it is automatically set.
 * @param latitudeFirstPoint Latitude of first point.
 * @param longitudeFirstPoint Longitude of first point.
 * @param latitudeSecondPoint Latitude of second point.
 * @param longitudeSeconPoint Longitude of second point.
 * @param falseEasting See \ref false_easting
 * @param falseNorthing See \ref false_northing
 * @return a new Conversion.
 */
ConversionNNPtr
Conversion::createTwoPointEquidistant(const util::PropertyMap &properties,
                                      const common::Angle &latitudeFirstPoint,
                                      const common::Angle &longitudeFirstPoint,
                                      const common::Angle &latitudeSecondPoint,
                                      const common::Angle &longitudeSeconPoint,
                                      const common::Length &falseEasting,
                                      const common::Length &falseNorthing) {
    return create(properties, PROJ_WKT2_NAME_METHOD_TWO_POINT_EQUIDISTANT,
                  createParams(latitudeFirstPoint, longitudeFirstPoint,
                               latitudeSecondPoint, longitudeSeconPoint,
                               falseEasting, falseNorthing));
}

// ---------------------------------------------------------------------------

/** \brief Instantiate a conversion based on the Tunisia Mapping Grid projection
 * method.
 *
 * This method is defined as [EPSG:9816]
 * (https://www.epsg-registry.org/export.htm?gml=urn:ogc:def:method:EPSG::9816)
 *
 * \note There is currently no implementation of the method formulas in PROJ.
 *
 * @param properties See \ref general_properties of the conversion. If the name
 * is not provided, it is automatically set.
 * @param centerLat See \ref center_latitude
 * @param centerLong See \ref center_longitude
 * @param falseEasting See \ref false_easting
 * @param falseNorthing See \ref false_northing
 * @return a new Conversion.
 */
ConversionNNPtr Conversion::createTunisiaMappingGrid(
    const util::PropertyMap &properties, const common::Angle &centerLat,
    const common::Angle &centerLong, const common::Length &falseEasting,
    const common::Length &falseNorthing) {
    return create(
        properties, EPSG_CODE_METHOD_TUNISIA_MAPPING_GRID,
        createParams(centerLat, centerLong, falseEasting, falseNorthing));
}

// ---------------------------------------------------------------------------

/** \brief Instantiate a conversion based on the [Albers Conic Equal Area]
 *(https://proj.org/operations/projections/aea.html) projection method.
 *
 * This method is defined as [EPSG:9822]
 * (https://www.epsg-registry.org/export.htm?gml=urn:ogc:def:method:EPSG::9822)
 *
 * @note the order of arguments is conformant with the corresponding EPSG
 * mode and different than OGRSpatialReference::setACEA() of GDAL &lt;= 2.3
 *
 * @param properties See \ref general_properties of the conversion. If the name
 * is not provided, it is automatically set.
 * @param latitudeFalseOrigin See \ref latitude_false_origin
 * @param longitudeFalseOrigin See \ref longitude_false_origin
 * @param latitudeFirstParallel See \ref latitude_first_std_parallel
 * @param latitudeSecondParallel See \ref latitude_second_std_parallel
 * @param eastingFalseOrigin See \ref easting_false_origin
 * @param northingFalseOrigin See \ref northing_false_origin
 * @return a new Conversion.
 */
ConversionNNPtr
Conversion::createAlbersEqualArea(const util::PropertyMap &properties,
                                  const common::Angle &latitudeFalseOrigin,
                                  const common::Angle &longitudeFalseOrigin,
                                  const common::Angle &latitudeFirstParallel,
                                  const common::Angle &latitudeSecondParallel,
                                  const common::Length &eastingFalseOrigin,
                                  const common::Length &northingFalseOrigin) {
    return create(properties, EPSG_CODE_METHOD_ALBERS_EQUAL_AREA,
                  createParams(latitudeFalseOrigin, longitudeFalseOrigin,
                               latitudeFirstParallel, latitudeSecondParallel,
                               eastingFalseOrigin, northingFalseOrigin));
}

// ---------------------------------------------------------------------------

/** \brief Instantiate a conversion based on the [Lambert Conic Conformal 1SP]
 *(https://proj.org/operations/projections/lcc.html) projection method.
 *
 * This method is defined as [EPSG:9801]
 * (https://www.epsg-registry.org/export.htm?gml=urn:ogc:def:method:EPSG::9801)
 *
 * @param properties See \ref general_properties of the conversion. If the name
 * is not provided, it is automatically set.
 * @param centerLat See \ref center_latitude
 * @param centerLong See \ref center_longitude
 * @param scale See \ref scale
 * @param falseEasting See \ref false_easting
 * @param falseNorthing See \ref false_northing
 * @return a new Conversion.
 */
ConversionNNPtr Conversion::createLambertConicConformal_1SP(
    const util::PropertyMap &properties, const common::Angle &centerLat,
    const common::Angle &centerLong, const common::Scale &scale,
    const common::Length &falseEasting, const common::Length &falseNorthing) {
    return create(properties, EPSG_CODE_METHOD_LAMBERT_CONIC_CONFORMAL_1SP,
                  createParams(centerLat, centerLong, scale, falseEasting,
                               falseNorthing));
}

// ---------------------------------------------------------------------------

/** \brief Instantiate a conversion based on the [Lambert Conic Conformal (2SP)]
 *(https://proj.org/operations/projections/lcc.html) projection method.
 *
 * This method is defined as [EPSG:9802]
 * (https://www.epsg-registry.org/export.htm?gml=urn:ogc:def:method:EPSG::9802)
 *
 * @note the order of arguments is conformant with the corresponding EPSG
 * mode and different than OGRSpatialReference::setLCC() of GDAL &lt;= 2.3
 *
 * @param properties See \ref general_properties of the conversion. If the name
 * is not provided, it is automatically set.
 * @param latitudeFalseOrigin See \ref latitude_false_origin
 * @param longitudeFalseOrigin See \ref longitude_false_origin
 * @param latitudeFirstParallel See \ref latitude_first_std_parallel
 * @param latitudeSecondParallel See \ref latitude_second_std_parallel
 * @param eastingFalseOrigin See \ref easting_false_origin
 * @param northingFalseOrigin See \ref northing_false_origin
 * @return a new Conversion.
 */
ConversionNNPtr Conversion::createLambertConicConformal_2SP(
    const util::PropertyMap &properties,
    const common::Angle &latitudeFalseOrigin,
    const common::Angle &longitudeFalseOrigin,
    const common::Angle &latitudeFirstParallel,
    const common::Angle &latitudeSecondParallel,
    const common::Length &eastingFalseOrigin,
    const common::Length &northingFalseOrigin) {
    return create(properties, EPSG_CODE_METHOD_LAMBERT_CONIC_CONFORMAL_2SP,
                  createParams(latitudeFalseOrigin, longitudeFalseOrigin,
                               latitudeFirstParallel, latitudeSecondParallel,
                               eastingFalseOrigin, northingFalseOrigin));
}

// ---------------------------------------------------------------------------

/** \brief Instantiate a conversion based on the [Lambert Conic Conformal (2SP
 *Michigan)]
 *(https://proj.org/operations/projections/lcc.html) projection method.
 *
 * This method is defined as [EPSG:1051]
 * (https://www.epsg-registry.org/export.htm?gml=urn:ogc:def:method:EPSG::1051)
 *
 * @param properties See \ref general_properties of the conversion. If the name
 * is not provided, it is automatically set.
 * @param latitudeFalseOrigin See \ref latitude_false_origin
 * @param longitudeFalseOrigin See \ref longitude_false_origin
 * @param latitudeFirstParallel See \ref latitude_first_std_parallel
 * @param latitudeSecondParallel See \ref latitude_second_std_parallel
 * @param eastingFalseOrigin See \ref easting_false_origin
 * @param northingFalseOrigin See \ref northing_false_origin
 * @param ellipsoidScalingFactor Ellipsoid scaling factor.
 * @return a new Conversion.
 */
ConversionNNPtr Conversion::createLambertConicConformal_2SP_Michigan(
    const util::PropertyMap &properties,
    const common::Angle &latitudeFalseOrigin,
    const common::Angle &longitudeFalseOrigin,
    const common::Angle &latitudeFirstParallel,
    const common::Angle &latitudeSecondParallel,
    const common::Length &eastingFalseOrigin,
    const common::Length &northingFalseOrigin,
    const common::Scale &ellipsoidScalingFactor) {
    return create(properties,
                  EPSG_CODE_METHOD_LAMBERT_CONIC_CONFORMAL_2SP_MICHIGAN,
                  createParams(latitudeFalseOrigin, longitudeFalseOrigin,
                               latitudeFirstParallel, latitudeSecondParallel,
                               eastingFalseOrigin, northingFalseOrigin,
                               ellipsoidScalingFactor));
}

// ---------------------------------------------------------------------------

/** \brief Instantiate a conversion based on the [Lambert Conic Conformal (2SP
 *Belgium)]
 *(https://proj.org/operations/projections/lcc.html) projection method.
 *
 * This method is defined as [EPSG:9803]
 * (https://www.epsg-registry.org/export.htm?gml=urn:ogc:def:method:EPSG::9803)
 *
 * \warning The formulas used currently in PROJ are, incorrectly, the ones of
 * the regular LCC_2SP method.
 *
 * @note the order of arguments is conformant with the corresponding EPSG
 * mode and different than OGRSpatialReference::setLCCB() of GDAL &lt;= 2.3
 *
 * @param properties See \ref general_properties of the conversion. If the name
 * is not provided, it is automatically set.
 * @param latitudeFalseOrigin See \ref latitude_false_origin
 * @param longitudeFalseOrigin See \ref longitude_false_origin
 * @param latitudeFirstParallel See \ref latitude_first_std_parallel
 * @param latitudeSecondParallel See \ref latitude_second_std_parallel
 * @param eastingFalseOrigin See \ref easting_false_origin
 * @param northingFalseOrigin See \ref northing_false_origin
 * @return a new Conversion.
 */
ConversionNNPtr Conversion::createLambertConicConformal_2SP_Belgium(
    const util::PropertyMap &properties,
    const common::Angle &latitudeFalseOrigin,
    const common::Angle &longitudeFalseOrigin,
    const common::Angle &latitudeFirstParallel,
    const common::Angle &latitudeSecondParallel,
    const common::Length &eastingFalseOrigin,
    const common::Length &northingFalseOrigin) {

    return create(properties,
                  EPSG_CODE_METHOD_LAMBERT_CONIC_CONFORMAL_2SP_BELGIUM,
                  createParams(latitudeFalseOrigin, longitudeFalseOrigin,
                               latitudeFirstParallel, latitudeSecondParallel,
                               eastingFalseOrigin, northingFalseOrigin));
}

// ---------------------------------------------------------------------------

/** \brief Instantiate a conversion based on the [Modified Azimuthal
 *Equidistant]
 *(https://proj.org/operations/projections/aeqd.html) projection method.
 *
 * This method is defined as [EPSG:9832]
 * (https://www.epsg-registry.org/export.htm?gml=urn:ogc:def:method:EPSG::9832)
 *
 * @param properties See \ref general_properties of the conversion. If the name
 * is not provided, it is automatically set.
 * @param latitudeNatOrigin See \ref center_latitude
 * @param longitudeNatOrigin See \ref center_longitude
 * @param falseEasting See \ref false_easting
 * @param falseNorthing See \ref false_northing
 * @return a new Conversion.
 */
ConversionNNPtr Conversion::createAzimuthalEquidistant(
    const util::PropertyMap &properties, const common::Angle &latitudeNatOrigin,
    const common::Angle &longitudeNatOrigin, const common::Length &falseEasting,
    const common::Length &falseNorthing) {
    return create(properties, EPSG_CODE_METHOD_MODIFIED_AZIMUTHAL_EQUIDISTANT,
                  createParams(latitudeNatOrigin, longitudeNatOrigin,
                               falseEasting, falseNorthing));
}

// ---------------------------------------------------------------------------

/** \brief Instantiate a conversion based on the [Guam Projection]
 *(https://proj.org/operations/projections/aeqd.html) projection method.
 *
 * This method is defined as [EPSG:9831]
 * (https://www.epsg-registry.org/export.htm?gml=urn:ogc:def:method:EPSG::9831)
 *
 * @param properties See \ref general_properties of the conversion. If the name
 *is
 * not provided, it is automatically set.
 * @param latitudeNatOrigin See \ref center_latitude
 * @param longitudeNatOrigin See \ref center_longitude
 * @param falseEasting See \ref false_easting
 * @param falseNorthing See \ref false_northing
 * @return a new Conversion.
 */
ConversionNNPtr Conversion::createGuamProjection(
    const util::PropertyMap &properties, const common::Angle &latitudeNatOrigin,
    const common::Angle &longitudeNatOrigin, const common::Length &falseEasting,
    const common::Length &falseNorthing) {
    return create(properties, EPSG_CODE_METHOD_GUAM_PROJECTION,
                  createParams(latitudeNatOrigin, longitudeNatOrigin,
                               falseEasting, falseNorthing));
}

// ---------------------------------------------------------------------------

/** \brief Instantiate a conversion based on the [Bonne]
 *(https://proj.org/operations/projections/bonne.html) projection method.
 *
 * This method is defined as [EPSG:9827]
 * (https://www.epsg-registry.org/export.htm?gml=urn:ogc:def:method:EPSG::9827)
 *
 * @param properties See \ref general_properties of the conversion. If the name
 * is not provided, it is automatically set.
 * @param latitudeNatOrigin See \ref center_latitude . PROJ calls its the
 * standard parallel 1.
 * @param longitudeNatOrigin See \ref center_longitude
 * @param falseEasting See \ref false_easting
 * @param falseNorthing See \ref false_northing
 * @return a new Conversion.
 */
ConversionNNPtr Conversion::createBonne(const util::PropertyMap &properties,
                                        const common::Angle &latitudeNatOrigin,
                                        const common::Angle &longitudeNatOrigin,
                                        const common::Length &falseEasting,
                                        const common::Length &falseNorthing) {
    return create(properties, EPSG_CODE_METHOD_BONNE,
                  createParams(latitudeNatOrigin, longitudeNatOrigin,
                               falseEasting, falseNorthing));
}

// ---------------------------------------------------------------------------

/** \brief Instantiate a conversion based on the [Lambert Cylindrical Equal Area
 *(Spherical)]
 *(https://proj.org/operations/projections/cea.html) projection method.
 *
 * This method is defined as [EPSG:9834]
 * (https://www.epsg-registry.org/export.htm?gml=urn:ogc:def:method:EPSG::9834)
 *
 * \warning The PROJ cea computation code would select the ellipsoidal form if
 * a non-spherical ellipsoid is used for the base GeographicCRS.
 *
 * @param properties See \ref general_properties of the conversion. If the name
 * is not provided, it is automatically set.
 * @param latitudeFirstParallel See \ref latitude_first_std_parallel.
 * @param longitudeNatOrigin See \ref center_longitude
 * @param falseEasting See \ref false_easting
 * @param falseNorthing See \ref false_northing
 * @return a new Conversion.
 */
ConversionNNPtr Conversion::createLambertCylindricalEqualAreaSpherical(
    const util::PropertyMap &properties,
    const common::Angle &latitudeFirstParallel,
    const common::Angle &longitudeNatOrigin, const common::Length &falseEasting,
    const common::Length &falseNorthing) {
    return create(properties,
                  EPSG_CODE_METHOD_LAMBERT_CYLINDRICAL_EQUAL_AREA_SPHERICAL,
                  createParams(latitudeFirstParallel, longitudeNatOrigin,
                               falseEasting, falseNorthing));
}

// ---------------------------------------------------------------------------

/** \brief Instantiate a conversion based on the [Lambert Cylindrical Equal Area
 *(ellipsoidal form)]
 *(https://proj.org/operations/projections/cea.html) projection method.
 *
 * This method is defined as [EPSG:9835]
 * (https://www.epsg-registry.org/export.htm?gml=urn:ogc:def:method:EPSG::9835)
 *
 * @param properties See \ref general_properties of the conversion. If the name
 * is not provided, it is automatically set.
 * @param latitudeFirstParallel See \ref latitude_first_std_parallel.
 * @param longitudeNatOrigin See \ref center_longitude
 * @param falseEasting See \ref false_easting
 * @param falseNorthing See \ref false_northing
 * @return a new Conversion.
 */
ConversionNNPtr Conversion::createLambertCylindricalEqualArea(
    const util::PropertyMap &properties,
    const common::Angle &latitudeFirstParallel,
    const common::Angle &longitudeNatOrigin, const common::Length &falseEasting,
    const common::Length &falseNorthing) {
    return create(properties, EPSG_CODE_METHOD_LAMBERT_CYLINDRICAL_EQUAL_AREA,
                  createParams(latitudeFirstParallel, longitudeNatOrigin,
                               falseEasting, falseNorthing));
}

// ---------------------------------------------------------------------------

/** \brief Instantiate a conversion based on the [Cassini-Soldner]
 * (https://proj.org/operations/projections/cass.html) projection method.
 *
 * This method is defined as [EPSG:9806]
 * (https://www.epsg-registry.org/export.htm?gml=urn:ogc:def:method:EPSG::9806)
 *
 * @param properties See \ref general_properties of the conversion. If the name
 * is not provided, it is automatically set.
 * @param centerLat See \ref center_latitude
 * @param centerLong See \ref center_longitude
 * @param falseEasting See \ref false_easting
 * @param falseNorthing See \ref false_northing
 * @return a new Conversion.
 */
ConversionNNPtr Conversion::createCassiniSoldner(
    const util::PropertyMap &properties, const common::Angle &centerLat,
    const common::Angle &centerLong, const common::Length &falseEasting,
    const common::Length &falseNorthing) {
    return create(
        properties, EPSG_CODE_METHOD_CASSINI_SOLDNER,
        createParams(centerLat, centerLong, falseEasting, falseNorthing));
}

// ---------------------------------------------------------------------------

/** \brief Instantiate a conversion based on the [Equidistant Conic]
 *(https://proj.org/operations/projections/eqdc.html) projection method.
 *
 * There is no equivalent in EPSG.
 *
 * @note Although not found in EPSG, the order of arguments is conformant with
 * the "spirit" of EPSG and different than OGRSpatialReference::setEC() of GDAL
 *&lt;= 2.3 * @param properties See \ref general_properties of the conversion.
 *If the name
 * is not provided, it is automatically set.
 *
 * @param centerLat See \ref center_latitude
 * @param centerLong See \ref center_longitude
 * @param latitudeFirstParallel See \ref latitude_first_std_parallel
 * @param latitudeSecondParallel See \ref latitude_second_std_parallel
 * @param falseEasting See \ref false_easting
 * @param falseNorthing See \ref false_northing
 * @return a new Conversion.
 */
ConversionNNPtr Conversion::createEquidistantConic(
    const util::PropertyMap &properties, const common::Angle &centerLat,
    const common::Angle &centerLong, const common::Angle &latitudeFirstParallel,
    const common::Angle &latitudeSecondParallel,
    const common::Length &falseEasting, const common::Length &falseNorthing) {
    return create(properties, PROJ_WKT2_NAME_METHOD_EQUIDISTANT_CONIC,
                  createParams(centerLat, centerLong, latitudeFirstParallel,
                               latitudeSecondParallel, falseEasting,
                               falseNorthing));
}

// ---------------------------------------------------------------------------

/** \brief Instantiate a conversion based on the [Eckert I]
 * (https://proj.org/operations/projections/eck1.html) projection method.
 *
 * There is no equivalent in EPSG.
 *
 * @param properties See \ref general_properties of the conversion. If the name
 * is not provided, it is automatically set.
 * @param centerLong See \ref center_longitude
 * @param falseEasting See \ref false_easting
 * @param falseNorthing See \ref false_northing
 * @return a new Conversion.
 */
ConversionNNPtr Conversion::createEckertI(const util::PropertyMap &properties,
                                          const common::Angle &centerLong,
                                          const common::Length &falseEasting,
                                          const common::Length &falseNorthing) {
    return create(properties, PROJ_WKT2_NAME_METHOD_ECKERT_I,
                  createParams(centerLong, falseEasting, falseNorthing));
}

// ---------------------------------------------------------------------------

/** \brief Instantiate a conversion based on the [Eckert II]
 * (https://proj.org/operations/projections/eck2.html) projection method.
 *
 * There is no equivalent in EPSG.
 *
 * @param properties See \ref general_properties of the conversion. If the name
 * is not provided, it is automatically set.
 * @param centerLong See \ref center_longitude
 * @param falseEasting See \ref false_easting
 * @param falseNorthing See \ref false_northing
 * @return a new Conversion.
 */
ConversionNNPtr Conversion::createEckertII(
    const util::PropertyMap &properties, const common::Angle &centerLong,
    const common::Length &falseEasting, const common::Length &falseNorthing) {
    return create(properties, PROJ_WKT2_NAME_METHOD_ECKERT_II,
                  createParams(centerLong, falseEasting, falseNorthing));
}

// ---------------------------------------------------------------------------

/** \brief Instantiate a conversion based on the [Eckert III]
 * (https://proj.org/operations/projections/eck3.html) projection method.
 *
 * There is no equivalent in EPSG.
 *
 * @param properties See \ref general_properties of the conversion. If the name
 * is not provided, it is automatically set.
 * @param centerLong See \ref center_longitude
 * @param falseEasting See \ref false_easting
 * @param falseNorthing See \ref false_northing
 * @return a new Conversion.
 */
ConversionNNPtr Conversion::createEckertIII(
    const util::PropertyMap &properties, const common::Angle &centerLong,
    const common::Length &falseEasting, const common::Length &falseNorthing) {
    return create(properties, PROJ_WKT2_NAME_METHOD_ECKERT_III,
                  createParams(centerLong, falseEasting, falseNorthing));
}

// ---------------------------------------------------------------------------

/** \brief Instantiate a conversion based on the [Eckert IV]
 * (https://proj.org/operations/projections/eck4.html) projection method.
 *
 * There is no equivalent in EPSG.
 *
 * @param properties See \ref general_properties of the conversion. If the name
 * is not provided, it is automatically set.
 * @param centerLong See \ref center_longitude
 * @param falseEasting See \ref false_easting
 * @param falseNorthing See \ref false_northing
 * @return a new Conversion.
 */
ConversionNNPtr Conversion::createEckertIV(
    const util::PropertyMap &properties, const common::Angle &centerLong,
    const common::Length &falseEasting, const common::Length &falseNorthing) {
    return create(properties, PROJ_WKT2_NAME_METHOD_ECKERT_IV,
                  createParams(centerLong, falseEasting, falseNorthing));
}

// ---------------------------------------------------------------------------

/** \brief Instantiate a conversion based on the [Eckert V]
 * (https://proj.org/operations/projections/eck5.html) projection method.
 *
 * There is no equivalent in EPSG.
 *
 * @param properties See \ref general_properties of the conversion. If the name
 * is not provided, it is automatically set.
 * @param centerLong See \ref center_longitude
 * @param falseEasting See \ref false_easting
 * @param falseNorthing See \ref false_northing
 * @return a new Conversion.
 */
ConversionNNPtr Conversion::createEckertV(const util::PropertyMap &properties,
                                          const common::Angle &centerLong,
                                          const common::Length &falseEasting,
                                          const common::Length &falseNorthing) {
    return create(properties, PROJ_WKT2_NAME_METHOD_ECKERT_V,
                  createParams(centerLong, falseEasting, falseNorthing));
}

// ---------------------------------------------------------------------------

/** \brief Instantiate a conversion based on the [Eckert VI]
 * (https://proj.org/operations/projections/eck6.html) projection method.
 *
 * There is no equivalent in EPSG.
 *
 * @param properties See \ref general_properties of the conversion. If the name
 * is not provided, it is automatically set.
 * @param centerLong See \ref center_longitude
 * @param falseEasting See \ref false_easting
 * @param falseNorthing See \ref false_northing
 * @return a new Conversion.
 */
ConversionNNPtr Conversion::createEckertVI(
    const util::PropertyMap &properties, const common::Angle &centerLong,
    const common::Length &falseEasting, const common::Length &falseNorthing) {
    return create(properties, PROJ_WKT2_NAME_METHOD_ECKERT_VI,
                  createParams(centerLong, falseEasting, falseNorthing));
}

// ---------------------------------------------------------------------------

/** \brief Instantiate a conversion based on the [Equidistant Cylindrical]
 *(https://proj.org/operations/projections/eqc.html) projection method.
 *
 * This is also known as the Equirectangular method, and in the particular case
 * where the latitude of first parallel is 0.
 *
 * This method is defined as [EPSG:1028]
 * (https://www.epsg-registry.org/export.htm?gml=urn:ogc:def:method:EPSG::1028)
 *
 * @note This is the equivalent OGRSpatialReference::SetEquirectangular2(
 * 0.0, latitudeFirstParallel, falseEasting, falseNorthing ) of GDAL &lt;= 2.3,
 * where the lat_0 / center_latitude parameter is forced to 0.
 *
 * @param properties See \ref general_properties of the conversion. If the name
 * is not provided, it is automatically set.
 * @param latitudeFirstParallel See \ref latitude_first_std_parallel.
 * @param longitudeNatOrigin See \ref center_longitude
 * @param falseEasting See \ref false_easting
 * @param falseNorthing See \ref false_northing
 * @return a new Conversion.
 */
ConversionNNPtr Conversion::createEquidistantCylindrical(
    const util::PropertyMap &properties,
    const common::Angle &latitudeFirstParallel,
    const common::Angle &longitudeNatOrigin, const common::Length &falseEasting,
    const common::Length &falseNorthing) {
    return create(properties, EPSG_CODE_METHOD_EQUIDISTANT_CYLINDRICAL,
                  createParams(latitudeFirstParallel, 0.0, longitudeNatOrigin,
                               falseEasting, falseNorthing));
}

// ---------------------------------------------------------------------------

/** \brief Instantiate a conversion based on the [Equidistant Cylindrical
 *(Spherical)]
 *(https://proj.org/operations/projections/eqc.html) projection method.
 *
 * This is also known as the Equirectangular method, and in the particular case
 * where the latitude of first parallel is 0.
 *
 * This method is defined as [EPSG:1029]
 * (https://www.epsg-registry.org/export.htm?gml=urn:ogc:def:method:EPSG::1029)
 *
 * @note This is the equivalent OGRSpatialReference::SetEquirectangular2(
 * 0.0, latitudeFirstParallel, falseEasting, falseNorthing ) of GDAL &lt;= 2.3,
 * where the lat_0 / center_latitude parameter is forced to 0.
 *
 * @param properties See \ref general_properties of the conversion. If the name
 * is not provided, it is automatically set.
 * @param latitudeFirstParallel See \ref latitude_first_std_parallel.
 * @param longitudeNatOrigin See \ref center_longitude
 * @param falseEasting See \ref false_easting
 * @param falseNorthing See \ref false_northing
 * @return a new Conversion.
 */
ConversionNNPtr Conversion::createEquidistantCylindricalSpherical(
    const util::PropertyMap &properties,
    const common::Angle &latitudeFirstParallel,
    const common::Angle &longitudeNatOrigin, const common::Length &falseEasting,
    const common::Length &falseNorthing) {
    return create(properties,
                  EPSG_CODE_METHOD_EQUIDISTANT_CYLINDRICAL_SPHERICAL,
                  createParams(latitudeFirstParallel, 0.0, longitudeNatOrigin,
                               falseEasting, falseNorthing));
}

// ---------------------------------------------------------------------------

/** \brief Instantiate a conversion based on the [Gall (Stereographic)]
 * (https://proj.org/operations/projections/gall.html) projection method.
 *
 * There is no equivalent in EPSG.
 *
 * @param properties See \ref general_properties of the conversion. If the name
 * is not provided, it is automatically set.
 * @param centerLong See \ref center_longitude
 * @param falseEasting See \ref false_easting
 * @param falseNorthing See \ref false_northing
 * @return a new Conversion.
 */
ConversionNNPtr Conversion::createGall(const util::PropertyMap &properties,
                                       const common::Angle &centerLong,
                                       const common::Length &falseEasting,
                                       const common::Length &falseNorthing) {
    return create(properties, PROJ_WKT2_NAME_METHOD_GALL_STEREOGRAPHIC,
                  createParams(centerLong, falseEasting, falseNorthing));
}

// ---------------------------------------------------------------------------

/** \brief Instantiate a conversion based on the [Goode Homolosine]
 * (https://proj.org/operations/projections/goode.html) projection method.
 *
 * There is no equivalent in EPSG.
 *
 * @param properties See \ref general_properties of the conversion. If the name
 * is not provided, it is automatically set.
 * @param centerLong See \ref center_longitude
 * @param falseEasting See \ref false_easting
 * @param falseNorthing See \ref false_northing
 * @return a new Conversion.
 */
ConversionNNPtr Conversion::createGoodeHomolosine(
    const util::PropertyMap &properties, const common::Angle &centerLong,
    const common::Length &falseEasting, const common::Length &falseNorthing) {
    return create(properties, PROJ_WKT2_NAME_METHOD_GOODE_HOMOLOSINE,
                  createParams(centerLong, falseEasting, falseNorthing));
}

// ---------------------------------------------------------------------------

/** \brief Instantiate a conversion based on the [Interrupted Goode Homolosine]
 * (https://proj.org/operations/projections/igh.html) projection method.
 *
 * There is no equivalent in EPSG.
 *
 * @note OGRSpatialReference::SetIGH() of GDAL &lt;= 2.3 assumes the 3
 * projection
 * parameters to be zero and this is the nominal case.
 *
 * @param properties See \ref general_properties of the conversion. If the name
 * is not provided, it is automatically set.
 * @param centerLong See \ref center_longitude
 * @param falseEasting See \ref false_easting
 * @param falseNorthing See \ref false_northing
 * @return a new Conversion.
 */
ConversionNNPtr Conversion::createInterruptedGoodeHomolosine(
    const util::PropertyMap &properties, const common::Angle &centerLong,
    const common::Length &falseEasting, const common::Length &falseNorthing) {
    return create(properties,
                  PROJ_WKT2_NAME_METHOD_INTERRUPTED_GOODE_HOMOLOSINE,
                  createParams(centerLong, falseEasting, falseNorthing));
}

// ---------------------------------------------------------------------------

/** \brief Instantiate a conversion based on the [Geostationary Satellite View]
 * (https://proj.org/operations/projections/geos.html) projection method,
 * with the sweep angle axis of the viewing instrument being x
 *
 * There is no equivalent in EPSG.
 *
 * @param properties See \ref general_properties of the conversion. If the name
 * is not provided, it is automatically set.
 * @param centerLong See \ref center_longitude
 * @param height Height of the view point above the Earth.
 * @param falseEasting See \ref false_easting
 * @param falseNorthing See \ref false_northing
 * @return a new Conversion.
 */
ConversionNNPtr Conversion::createGeostationarySatelliteSweepX(
    const util::PropertyMap &properties, const common::Angle &centerLong,
    const common::Length &height, const common::Length &falseEasting,
    const common::Length &falseNorthing) {
    return create(
        properties, PROJ_WKT2_NAME_METHOD_GEOSTATIONARY_SATELLITE_SWEEP_X,
        createParams(centerLong, height, falseEasting, falseNorthing));
}

// ---------------------------------------------------------------------------

/** \brief Instantiate a conversion based on the [Geostationary Satellite View]
 * (https://proj.org/operations/projections/geos.html) projection method,
 * with the sweep angle axis of the viewing instrument being y.
 *
 * There is no equivalent in EPSG.
 *
 * @param properties See \ref general_properties of the conversion. If the name
 * is not provided, it is automatically set.
 * @param centerLong See \ref center_longitude
 * @param height Height of the view point above the Earth.
 * @param falseEasting See \ref false_easting
 * @param falseNorthing See \ref false_northing
 * @return a new Conversion.
 */
ConversionNNPtr Conversion::createGeostationarySatelliteSweepY(
    const util::PropertyMap &properties, const common::Angle &centerLong,
    const common::Length &height, const common::Length &falseEasting,
    const common::Length &falseNorthing) {
    return create(
        properties, PROJ_WKT2_NAME_METHOD_GEOSTATIONARY_SATELLITE_SWEEP_Y,
        createParams(centerLong, height, falseEasting, falseNorthing));
}

// ---------------------------------------------------------------------------

/** \brief Instantiate a conversion based on the [Gnomonic]
 *(https://proj.org/operations/projections/gnom.html) projection method.
 *
 * There is no equivalent in EPSG.
 *
 * @param properties See \ref general_properties of the conversion. If the name
 * is not provided, it is automatically set.
 * @param centerLat See \ref center_latitude
 * @param centerLong See \ref center_longitude
 * @param falseEasting See \ref false_easting
 * @param falseNorthing See \ref false_northing
 * @return a new Conversion.
 */
ConversionNNPtr Conversion::createGnomonic(
    const util::PropertyMap &properties, const common::Angle &centerLat,
    const common::Angle &centerLong, const common::Length &falseEasting,
    const common::Length &falseNorthing) {
    return create(
        properties, PROJ_WKT2_NAME_METHOD_GNOMONIC,
        createParams(centerLat, centerLong, falseEasting, falseNorthing));
}

// ---------------------------------------------------------------------------

/** \brief Instantiate a conversion based on the [Hotine Oblique Mercator
 *(Variant A)]
 *(https://proj.org/operations/projections/omerc.html) projection method
 *
 * This is the variant with the no_uoff parameter, which corresponds to
 * GDAL &gt;=2.3 Hotine_Oblique_Mercator projection.
 * In this variant, the false grid coordinates are
 * defined at the intersection of the initial line and the aposphere (the
 * equator on one of the intermediate surfaces inherent in the method), that is
 * at the natural origin of the coordinate system).
 *
 * This method is defined as [EPSG:9812]
 * (https://www.epsg-registry.org/export.htm?gml=urn:ogc:def:method:EPSG::9812)
 *
 * \note In the case where azimuthInitialLine = angleFromRectifiedToSkrewGrid =
 *90deg,
 * this maps to the  [Swiss Oblique Mercator]
 *(https://proj.org/operations/projections/somerc.html) formulas.
 *
 * @param properties See \ref general_properties of the conversion. If the name
 * is not provided, it is automatically set.
 * @param latitudeProjectionCentre See \ref latitude_projection_centre
 * @param longitudeProjectionCentre See \ref longitude_projection_centre
 * @param azimuthInitialLine See \ref azimuth_initial_line
 * @param angleFromRectifiedToSkrewGrid See
 * \ref angle_from_recitfied_to_skrew_grid
 * @param scale See \ref scale_factor_initial_line
 * @param falseEasting See \ref false_easting
 * @param falseNorthing See \ref false_northing
 * @return a new Conversion.
 */
ConversionNNPtr Conversion::createHotineObliqueMercatorVariantA(
    const util::PropertyMap &properties,
    const common::Angle &latitudeProjectionCentre,
    const common::Angle &longitudeProjectionCentre,
    const common::Angle &azimuthInitialLine,
    const common::Angle &angleFromRectifiedToSkrewGrid,
    const common::Scale &scale, const common::Length &falseEasting,
    const common::Length &falseNorthing) {
    return create(
        properties, EPSG_CODE_METHOD_HOTINE_OBLIQUE_MERCATOR_VARIANT_A,
        createParams(latitudeProjectionCentre, longitudeProjectionCentre,
                     azimuthInitialLine, angleFromRectifiedToSkrewGrid, scale,
                     falseEasting, falseNorthing));
}

// ---------------------------------------------------------------------------

/** \brief Instantiate a conversion based on the [Hotine Oblique Mercator
 *(Variant B)]
 *(https://proj.org/operations/projections/omerc.html) projection method
 *
 * This is the variant without the no_uoff parameter, which corresponds to
 * GDAL &gt;=2.3 Hotine_Oblique_Mercator_Azimuth_Center projection.
 * In this variant, the false grid coordinates are defined at the projection
 *centre.
 *
 * This method is defined as [EPSG:9815]
 * (https://www.epsg-registry.org/export.htm?gml=urn:ogc:def:method:EPSG::9815)
 *
 * \note In the case where azimuthInitialLine = angleFromRectifiedToSkrewGrid =
 *90deg,
 * this maps to the  [Swiss Oblique Mercator]
 *(https://proj.org/operations/projections/somerc.html) formulas.
 *
 * @param properties See \ref general_properties of the conversion. If the name
 * is not provided, it is automatically set.
 * @param latitudeProjectionCentre See \ref latitude_projection_centre
 * @param longitudeProjectionCentre See \ref longitude_projection_centre
 * @param azimuthInitialLine See \ref azimuth_initial_line
 * @param angleFromRectifiedToSkrewGrid See
 * \ref angle_from_recitfied_to_skrew_grid
 * @param scale See \ref scale_factor_initial_line
 * @param eastingProjectionCentre See \ref easting_projection_centre
 * @param northingProjectionCentre See \ref northing_projection_centre
 * @return a new Conversion.
 */
ConversionNNPtr Conversion::createHotineObliqueMercatorVariantB(
    const util::PropertyMap &properties,
    const common::Angle &latitudeProjectionCentre,
    const common::Angle &longitudeProjectionCentre,
    const common::Angle &azimuthInitialLine,
    const common::Angle &angleFromRectifiedToSkrewGrid,
    const common::Scale &scale, const common::Length &eastingProjectionCentre,
    const common::Length &northingProjectionCentre) {
    return create(
        properties, EPSG_CODE_METHOD_HOTINE_OBLIQUE_MERCATOR_VARIANT_B,
        createParams(latitudeProjectionCentre, longitudeProjectionCentre,
                     azimuthInitialLine, angleFromRectifiedToSkrewGrid, scale,
                     eastingProjectionCentre, northingProjectionCentre));
}

// ---------------------------------------------------------------------------

/** \brief Instantiate a conversion based on the [Hotine Oblique Mercator Two
 *Point Natural Origin]
 *(https://proj.org/operations/projections/omerc.html) projection method.
 *
 * There is no equivalent in EPSG.
 *
 * @param properties See \ref general_properties of the conversion. If the name
 * is not provided, it is automatically set.
 * @param latitudeProjectionCentre See \ref latitude_projection_centre
 * @param latitudePoint1 Latitude of point 1.
 * @param longitudePoint1 Latitude of point 1.
 * @param latitudePoint2 Latitude of point 2.
 * @param longitudePoint2 Longitude of point 2.
 * @param scale See \ref scale_factor_initial_line
 * @param eastingProjectionCentre See \ref easting_projection_centre
 * @param northingProjectionCentre See \ref northing_projection_centre
 * @return a new Conversion.
 */
ConversionNNPtr Conversion::createHotineObliqueMercatorTwoPointNaturalOrigin(
    const util::PropertyMap &properties,
    const common::Angle &latitudeProjectionCentre,
    const common::Angle &latitudePoint1, const common::Angle &longitudePoint1,
    const common::Angle &latitudePoint2, const common::Angle &longitudePoint2,
    const common::Scale &scale, const common::Length &eastingProjectionCentre,
    const common::Length &northingProjectionCentre) {
    return create(
        properties,
        PROJ_WKT2_NAME_METHOD_HOTINE_OBLIQUE_MERCATOR_TWO_POINT_NATURAL_ORIGIN,
        {
            ParameterValue::create(latitudeProjectionCentre),
            ParameterValue::create(latitudePoint1),
            ParameterValue::create(longitudePoint1),
            ParameterValue::create(latitudePoint2),
            ParameterValue::create(longitudePoint2),
            ParameterValue::create(scale),
            ParameterValue::create(eastingProjectionCentre),
            ParameterValue::create(northingProjectionCentre),
        });
}

// ---------------------------------------------------------------------------

/** \brief Instantiate a conversion based on the [Laborde Oblique Mercator]
 *(https://proj.org/operations/projections/labrd.html) projection method.
 *
 * This method is defined as [EPSG:9813]
 * (https://www.epsg-registry.org/export.htm?gml=urn:ogc:def:method:EPSG::9813)
 *
 * @param properties See \ref general_properties of the conversion. If the name
 * is not provided, it is automatically set.
 * @param latitudeProjectionCentre See \ref latitude_projection_centre
 * @param longitudeProjectionCentre See \ref longitude_projection_centre
 * @param azimuthInitialLine See \ref azimuth_initial_line
 * @param scale See \ref scale_factor_initial_line
 * @param falseEasting See \ref false_easting
 * @param falseNorthing See \ref false_northing
 * @return a new Conversion.
 */
ConversionNNPtr Conversion::createLabordeObliqueMercator(
    const util::PropertyMap &properties,
    const common::Angle &latitudeProjectionCentre,
    const common::Angle &longitudeProjectionCentre,
    const common::Angle &azimuthInitialLine, const common::Scale &scale,
    const common::Length &falseEasting, const common::Length &falseNorthing) {
    return create(properties, EPSG_CODE_METHOD_LABORDE_OBLIQUE_MERCATOR,
                  createParams(latitudeProjectionCentre,
                               longitudeProjectionCentre, azimuthInitialLine,
                               scale, falseEasting, falseNorthing));
}

// ---------------------------------------------------------------------------

/** \brief Instantiate a conversion based on the [International Map of the World
 *Polyconic]
 *(https://proj.org/operations/projections/imw_p.html) projection method.
 *
 * There is no equivalent in EPSG.
 *
 * @note the order of arguments is conformant with the corresponding EPSG
 * mode and different than OGRSpatialReference::SetIWMPolyconic() of GDAL &lt;=
 *2.3
 *
 * @param properties See \ref general_properties of the conversion. If the name
 * is not provided, it is automatically set.
 * @param centerLong See \ref center_longitude
 * @param latitudeFirstParallel See \ref latitude_first_std_parallel
 * @param latitudeSecondParallel See \ref latitude_second_std_parallel
 * @param falseEasting See \ref false_easting
 * @param falseNorthing See \ref false_northing
 * @return a new Conversion.
 */
ConversionNNPtr Conversion::createInternationalMapWorldPolyconic(
    const util::PropertyMap &properties, const common::Angle &centerLong,
    const common::Angle &latitudeFirstParallel,
    const common::Angle &latitudeSecondParallel,
    const common::Length &falseEasting, const common::Length &falseNorthing) {
    return create(properties, PROJ_WKT2_NAME_INTERNATIONAL_MAP_WORLD_POLYCONIC,
                  createParams(centerLong, latitudeFirstParallel,
                               latitudeSecondParallel, falseEasting,
                               falseNorthing));
}

// ---------------------------------------------------------------------------

/** \brief Instantiate a conversion based on the [Krovak (north oriented)]
 *(https://proj.org/operations/projections/krovak.html) projection method.
 *
 * This method is defined as [EPSG:1041]
 * (https://www.epsg-registry.org/export.htm?gml=urn:ogc:def:method:EPSG::1041)
 *
 * The coordinates are returned in the "GIS friendly" order: easting, northing.
 * This method is similar to createKrovak(), except that the later returns
 * projected values as southing, westing, where
 * southing(Krovak) = -northing(Krovak_North) and
 * westing(Krovak) = -easting(Krovak_North).
 *
 * @note The current PROJ implementation of Krovak hard-codes
 * colatitudeConeAxis = 30deg17'17.30311"
 * and latitudePseudoStandardParallel = 78deg30'N, which are the values used for
 * the ProjectedCRS S-JTSK (Ferro) / Krovak East North (EPSG:5221).
 * It also hard-codes the parameters of the Bessel ellipsoid typically used for
 * Krovak.
 *
 * @param properties See \ref general_properties of the conversion. If the name
 * is not provided, it is automatically set.
 * @param latitudeProjectionCentre See \ref latitude_projection_centre
 * @param longitudeOfOrigin See \ref longitude_of_origin
 * @param colatitudeConeAxis See \ref colatitude_cone_axis
 * @param latitudePseudoStandardParallel See \ref
 *latitude_pseudo_standard_parallel
 * @param scaleFactorPseudoStandardParallel See \ref
 *scale_factor_pseudo_standard_parallel
 * @param falseEasting See \ref false_easting
 * @param falseNorthing See \ref false_northing
 * @return a new Conversion.
 */
ConversionNNPtr Conversion::createKrovakNorthOriented(
    const util::PropertyMap &properties,
    const common::Angle &latitudeProjectionCentre,
    const common::Angle &longitudeOfOrigin,
    const common::Angle &colatitudeConeAxis,
    const common::Angle &latitudePseudoStandardParallel,
    const common::Scale &scaleFactorPseudoStandardParallel,
    const common::Length &falseEasting, const common::Length &falseNorthing) {
    return create(properties, EPSG_CODE_METHOD_KROVAK_NORTH_ORIENTED,
                  createParams(latitudeProjectionCentre, longitudeOfOrigin,
                               colatitudeConeAxis,
                               latitudePseudoStandardParallel,
                               scaleFactorPseudoStandardParallel, falseEasting,
                               falseNorthing));
}

// ---------------------------------------------------------------------------

/** \brief Instantiate a conversion based on the [Krovak]
 *(https://proj.org/operations/projections/krovak.html) projection method.
 *
 * This method is defined as [EPSG:9819]
 * (https://www.epsg-registry.org/export.htm?gml=urn:ogc:def:method:EPSG::9819)
 *
 * The coordinates are returned in the historical order: southing, westing
 * This method is similar to createKrovakNorthOriented(), except that the later
 *returns
 * projected values as easting, northing, where
 * easting(Krovak_North) = -westing(Krovak) and
 * northing(Krovak_North) = -southing(Krovak).
 *
 * @note The current PROJ implementation of Krovak hard-codes
 * colatitudeConeAxis = 30deg17'17.30311"
 * and latitudePseudoStandardParallel = 78deg30'N, which are the values used for
 * the ProjectedCRS S-JTSK (Ferro) / Krovak East North (EPSG:5221).
 * It also hard-codes the parameters of the Bessel ellipsoid typically used for
 * Krovak.
 *
 * @param properties See \ref general_properties of the conversion. If the name
 * is not provided, it is automatically set.
 * @param latitudeProjectionCentre See \ref latitude_projection_centre
 * @param longitudeOfOrigin See \ref longitude_of_origin
 * @param colatitudeConeAxis See \ref colatitude_cone_axis
 * @param latitudePseudoStandardParallel See \ref
 *latitude_pseudo_standard_parallel
 * @param scaleFactorPseudoStandardParallel See \ref
 *scale_factor_pseudo_standard_parallel
 * @param falseEasting See \ref false_easting
 * @param falseNorthing See \ref false_northing
 * @return a new Conversion.
 */
ConversionNNPtr
Conversion::createKrovak(const util::PropertyMap &properties,
                         const common::Angle &latitudeProjectionCentre,
                         const common::Angle &longitudeOfOrigin,
                         const common::Angle &colatitudeConeAxis,
                         const common::Angle &latitudePseudoStandardParallel,
                         const common::Scale &scaleFactorPseudoStandardParallel,
                         const common::Length &falseEasting,
                         const common::Length &falseNorthing) {
    return create(properties, EPSG_CODE_METHOD_KROVAK,
                  createParams(latitudeProjectionCentre, longitudeOfOrigin,
                               colatitudeConeAxis,
                               latitudePseudoStandardParallel,
                               scaleFactorPseudoStandardParallel, falseEasting,
                               falseNorthing));
}

// ---------------------------------------------------------------------------

/** \brief Instantiate a conversion based on the [Lambert Azimuthal Equal Area]
 *(https://proj.org/operations/projections/laea.html) projection method.
 *
 * This method is defined as [EPSG:9820]
 * (https://www.epsg-registry.org/export.htm?gml=urn:ogc:def:method:EPSG::9820)
 *
 * @param properties See \ref general_properties of the conversion. If the name
 * is not provided, it is automatically set.
 * @param latitudeNatOrigin See \ref center_latitude
 * @param longitudeNatOrigin See \ref center_longitude
 * @param falseEasting See \ref false_easting
 * @param falseNorthing See \ref false_northing
 * @return a new Conversion.
 */
ConversionNNPtr Conversion::createLambertAzimuthalEqualArea(
    const util::PropertyMap &properties, const common::Angle &latitudeNatOrigin,
    const common::Angle &longitudeNatOrigin, const common::Length &falseEasting,
    const common::Length &falseNorthing) {
    return create(properties, EPSG_CODE_METHOD_LAMBERT_AZIMUTHAL_EQUAL_AREA,
                  createParams(latitudeNatOrigin, longitudeNatOrigin,
                               falseEasting, falseNorthing));
}

// ---------------------------------------------------------------------------

/** \brief Instantiate a conversion based on the [Miller Cylindrical]
 *(https://proj.org/operations/projections/mill.html) projection method.
 *
 * There is no equivalent in EPSG.
 *
 * @param properties See \ref general_properties of the conversion. If the name
 * is not provided, it is automatically set.
 * @param centerLong See \ref center_longitude
 * @param falseEasting See \ref false_easting
 * @param falseNorthing See \ref false_northing
 * @return a new Conversion.
 */
ConversionNNPtr Conversion::createMillerCylindrical(
    const util::PropertyMap &properties, const common::Angle &centerLong,
    const common::Length &falseEasting, const common::Length &falseNorthing) {
    return create(properties, PROJ_WKT2_NAME_METHOD_MILLER_CYLINDRICAL,
                  createParams(centerLong, falseEasting, falseNorthing));
}

// ---------------------------------------------------------------------------

/** \brief Instantiate a conversion based on the [Mercator]
 *(https://proj.org/operations/projections/merc.html) projection method.
 *
 * This is the variant, also known as Mercator (1SP), defined with the scale
 * factor. Note that latitude of natural origin (centerLat) is a parameter,
 * but unused in the transformation formulas.
 *
 * This method is defined as [EPSG:9804]
 * (https://www.epsg-registry.org/export.htm?gml=urn:ogc:def:method:EPSG::9804)
 *
 * @param properties See \ref general_properties of the conversion. If the name
 * is not provided, it is automatically set.
 * @param centerLat See \ref center_latitude . Should be 0.
 * @param centerLong See \ref center_longitude
 * @param scale See \ref scale
 * @param falseEasting See \ref false_easting
 * @param falseNorthing See \ref false_northing
 * @return a new Conversion.
 */
ConversionNNPtr Conversion::createMercatorVariantA(
    const util::PropertyMap &properties, const common::Angle &centerLat,
    const common::Angle &centerLong, const common::Scale &scale,
    const common::Length &falseEasting, const common::Length &falseNorthing) {
    return create(properties, EPSG_CODE_METHOD_MERCATOR_VARIANT_A,
                  createParams(centerLat, centerLong, scale, falseEasting,
                               falseNorthing));
}

// ---------------------------------------------------------------------------

/** \brief Instantiate a conversion based on the [Mercator]
 *(https://proj.org/operations/projections/merc.html) projection method.
 *
 * This is the variant, also known as Mercator (2SP), defined with the latitude
 * of the first standard parallel (the second standard parallel is implicitly
 * the opposite value). The latitude of natural origin is fixed to zero.
 *
 * This method is defined as [EPSG:9805]
 * (https://www.epsg-registry.org/export.htm?gml=urn:ogc:def:method:EPSG::9805)
 *
 * @param properties See \ref general_properties of the conversion. If the name
 * is not provided, it is automatically set.
 * @param latitudeFirstParallel See \ref latitude_first_std_parallel
 * @param centerLong See \ref center_longitude
 * @param falseEasting See \ref false_easting
 * @param falseNorthing See \ref false_northing
 * @return a new Conversion.
 */
ConversionNNPtr Conversion::createMercatorVariantB(
    const util::PropertyMap &properties,
    const common::Angle &latitudeFirstParallel, const common::Angle &centerLong,
    const common::Length &falseEasting, const common::Length &falseNorthing) {
    return create(properties, EPSG_CODE_METHOD_MERCATOR_VARIANT_B,
                  createParams(latitudeFirstParallel, centerLong, falseEasting,
                               falseNorthing));
}

// ---------------------------------------------------------------------------

/** \brief Instantiate a conversion based on the [Popular Visualisation Pseudo
 *Mercator]
 *(https://proj.org/operations/projections/webmerc.html) projection method.
 *
 * Also known as WebMercator. Mostly/only used for Projected CRS EPSG:3857
 * (WGS 84 / Pseudo-Mercator)
 *
 * This method is defined as [EPSG:1024]
 * (https://www.epsg-registry.org/export.htm?gml=urn:ogc:def:method:EPSG::1024)
 *
 * @param properties See \ref general_properties of the conversion. If the name
 * is not provided, it is automatically set.
 * @param centerLat See \ref center_latitude . Usually 0
 * @param centerLong See \ref center_longitude . Usually 0
 * @param falseEasting See \ref false_easting . Usually 0
 * @param falseNorthing See \ref false_northing . Usually 0
 * @return a new Conversion.
 */
ConversionNNPtr Conversion::createPopularVisualisationPseudoMercator(
    const util::PropertyMap &properties, const common::Angle &centerLat,
    const common::Angle &centerLong, const common::Length &falseEasting,
    const common::Length &falseNorthing) {
    return create(
        properties, EPSG_CODE_METHOD_POPULAR_VISUALISATION_PSEUDO_MERCATOR,
        createParams(centerLat, centerLong, falseEasting, falseNorthing));
}

// ---------------------------------------------------------------------------

/** \brief Instantiate a conversion based on the [Mollweide]
 * (https://proj.org/operations/projections/moll.html) projection method.
 *
 * There is no equivalent in EPSG.
 *
 * @param properties See \ref general_properties of the conversion. If the name
 * is not provided, it is automatically set.
 * @param centerLong See \ref center_longitude
 * @param falseEasting See \ref false_easting
 * @param falseNorthing See \ref false_northing
 * @return a new Conversion.
 */
ConversionNNPtr Conversion::createMollweide(
    const util::PropertyMap &properties, const common::Angle &centerLong,
    const common::Length &falseEasting, const common::Length &falseNorthing) {
    return create(properties, PROJ_WKT2_NAME_METHOD_MOLLWEIDE,
                  createParams(centerLong, falseEasting, falseNorthing));
}

// ---------------------------------------------------------------------------

/** \brief Instantiate a conversion based on the [New Zealand Map Grid]
 * (https://proj.org/operations/projections/nzmg.html) projection method.
 *
 * This method is defined as [EPSG:9811]
 * (https://www.epsg-registry.org/export.htm?gml=urn:ogc:def:method:EPSG::9811)
 *
 * @param properties See \ref general_properties of the conversion. If the name
 * is not provided, it is automatically set.
 * @param centerLat See \ref center_latitude
 * @param centerLong See \ref center_longitude
 * @param falseEasting See \ref false_easting
 * @param falseNorthing See \ref false_northing
 * @return a new Conversion.
 */
ConversionNNPtr Conversion::createNewZealandMappingGrid(
    const util::PropertyMap &properties, const common::Angle &centerLat,
    const common::Angle &centerLong, const common::Length &falseEasting,
    const common::Length &falseNorthing) {
    return create(
        properties, EPSG_CODE_METHOD_NZMG,
        createParams(centerLat, centerLong, falseEasting, falseNorthing));
}

// ---------------------------------------------------------------------------

/** \brief Instantiate a conversion based on the [Oblique Stereographic
 *(Alternative)]
 *(https://proj.org/operations/projections/sterea.html) projection method.
 *
 * This method is defined as [EPSG:9809]
 * (https://www.epsg-registry.org/export.htm?gml=urn:ogc:def:method:EPSG::9809)
 *
 * @param properties See \ref general_properties of the conversion. If the name
 * is not provided, it is automatically set.
 * @param centerLat See \ref center_latitude
 * @param centerLong See \ref center_longitude
 * @param scale See \ref scale
 * @param falseEasting See \ref false_easting
 * @param falseNorthing See \ref false_northing
 * @return a new Conversion.
 */
ConversionNNPtr Conversion::createObliqueStereographic(
    const util::PropertyMap &properties, const common::Angle &centerLat,
    const common::Angle &centerLong, const common::Scale &scale,
    const common::Length &falseEasting, const common::Length &falseNorthing) {
    return create(properties, EPSG_CODE_METHOD_OBLIQUE_STEREOGRAPHIC,
                  createParams(centerLat, centerLong, scale, falseEasting,
                               falseNorthing));
}

// ---------------------------------------------------------------------------

/** \brief Instantiate a conversion based on the [Orthographic]
 *(https://proj.org/operations/projections/ortho.html) projection method.
 *
 * This method is defined as [EPSG:9840]
 * (https://www.epsg-registry.org/export.htm?gml=urn:ogc:def:method:EPSG::9840)
 *
 * \note Before PROJ 7.2, only the spherical formulation was implemented.
 *
 * @param properties See \ref general_properties of the conversion. If the name
 * is not provided, it is automatically set.
 * @param centerLat See \ref center_latitude
 * @param centerLong See \ref center_longitude
 * @param falseEasting See \ref false_easting
 * @param falseNorthing See \ref false_northing
 * @return a new Conversion.
 */
ConversionNNPtr Conversion::createOrthographic(
    const util::PropertyMap &properties, const common::Angle &centerLat,
    const common::Angle &centerLong, const common::Length &falseEasting,
    const common::Length &falseNorthing) {
    return create(
        properties, EPSG_CODE_METHOD_ORTHOGRAPHIC,
        createParams(centerLat, centerLong, falseEasting, falseNorthing));
}

// ---------------------------------------------------------------------------

/** \brief Instantiate a conversion based on the [American Polyconic]
 *(https://proj.org/operations/projections/poly.html) projection method.
 *
 * This method is defined as [EPSG:9818]
 * (https://www.epsg-registry.org/export.htm?gml=urn:ogc:def:method:EPSG::9818)
 *
 * @param properties See \ref general_properties of the conversion. If the name
 * is not provided, it is automatically set.
 * @param centerLat See \ref center_latitude
 * @param centerLong See \ref center_longitude
 * @param falseEasting See \ref false_easting
 * @param falseNorthing See \ref false_northing
 * @return a new Conversion.
 */
ConversionNNPtr Conversion::createAmericanPolyconic(
    const util::PropertyMap &properties, const common::Angle &centerLat,
    const common::Angle &centerLong, const common::Length &falseEasting,
    const common::Length &falseNorthing) {
    return create(
        properties, EPSG_CODE_METHOD_AMERICAN_POLYCONIC,
        createParams(centerLat, centerLong, falseEasting, falseNorthing));
}

// ---------------------------------------------------------------------------

/** \brief Instantiate a conversion based on the [Polar Stereographic (Variant
 *A)]
 *(https://proj.org/operations/projections/stere.html) projection method.
 *
 * This method is defined as [EPSG:9810]
 * (https://www.epsg-registry.org/export.htm?gml=urn:ogc:def:method:EPSG::9810)
 *
 * This is the variant of polar stereographic defined with a scale factor.
 *
 * @param properties See \ref general_properties of the conversion. If the name
 * is not provided, it is automatically set.
 * @param centerLat See \ref center_latitude . Should be 90 deg ou -90 deg.
 * @param centerLong See \ref center_longitude
 * @param scale See \ref scale
 * @param falseEasting See \ref false_easting
 * @param falseNorthing See \ref false_northing
 * @return a new Conversion.
 */
ConversionNNPtr Conversion::createPolarStereographicVariantA(
    const util::PropertyMap &properties, const common::Angle &centerLat,
    const common::Angle &centerLong, const common::Scale &scale,
    const common::Length &falseEasting, const common::Length &falseNorthing) {
    return create(properties, EPSG_CODE_METHOD_POLAR_STEREOGRAPHIC_VARIANT_A,
                  createParams(centerLat, centerLong, scale, falseEasting,
                               falseNorthing));
}

// ---------------------------------------------------------------------------

/** \brief Instantiate a conversion based on the [Polar Stereographic (Variant
 *B)]
 *(https://proj.org/operations/projections/stere.html) projection method.
 *
 * This method is defined as [EPSG:9829]
 * (https://www.epsg-registry.org/export.htm?gml=urn:ogc:def:method:EPSG::9829)
 *
 * This is the variant of polar stereographic defined with a latitude of
 * standard parallel.
 *
 * @param properties See \ref general_properties of the conversion. If the name
 * is not provided, it is automatically set.
 * @param latitudeStandardParallel See \ref latitude_std_parallel
 * @param longitudeOfOrigin See \ref longitude_of_origin
 * @param falseEasting See \ref false_easting
 * @param falseNorthing See \ref false_northing
 * @return a new Conversion.
 */
ConversionNNPtr Conversion::createPolarStereographicVariantB(
    const util::PropertyMap &properties,
    const common::Angle &latitudeStandardParallel,
    const common::Angle &longitudeOfOrigin, const common::Length &falseEasting,
    const common::Length &falseNorthing) {
    return create(properties, EPSG_CODE_METHOD_POLAR_STEREOGRAPHIC_VARIANT_B,
                  createParams(latitudeStandardParallel, longitudeOfOrigin,
                               falseEasting, falseNorthing));
}

// ---------------------------------------------------------------------------

/** \brief Instantiate a conversion based on the [Robinson]
 * (https://proj.org/operations/projections/robin.html) projection method.
 *
 * There is no equivalent in EPSG.
 *
 * @param properties See \ref general_properties of the conversion. If the name
 * is not provided, it is automatically set.
 * @param centerLong See \ref center_longitude
 * @param falseEasting See \ref false_easting
 * @param falseNorthing See \ref false_northing
 * @return a new Conversion.
 */
ConversionNNPtr Conversion::createRobinson(
    const util::PropertyMap &properties, const common::Angle &centerLong,
    const common::Length &falseEasting, const common::Length &falseNorthing) {
    return create(properties, PROJ_WKT2_NAME_METHOD_ROBINSON,
                  createParams(centerLong, falseEasting, falseNorthing));
}

// ---------------------------------------------------------------------------

/** \brief Instantiate a conversion based on the [Sinusoidal]
 * (https://proj.org/operations/projections/sinu.html) projection method.
 *
 * There is no equivalent in EPSG.
 *
 * @param properties See \ref general_properties of the conversion. If the name
 * is not provided, it is automatically set.
 * @param centerLong See \ref center_longitude
 * @param falseEasting See \ref false_easting
 * @param falseNorthing See \ref false_northing
 * @return a new Conversion.
 */
ConversionNNPtr Conversion::createSinusoidal(
    const util::PropertyMap &properties, const common::Angle &centerLong,
    const common::Length &falseEasting, const common::Length &falseNorthing) {
    return create(properties, PROJ_WKT2_NAME_METHOD_SINUSOIDAL,
                  createParams(centerLong, falseEasting, falseNorthing));
}

// ---------------------------------------------------------------------------

/** \brief Instantiate a conversion based on the [Stereographic]
 *(https://proj.org/operations/projections/stere.html) projection method.
 *
 * There is no equivalent in EPSG. This method implements the original "Oblique
 * Stereographic" method described in "Snyder's Map Projections - A Working
 *manual",
 * which is different from the "Oblique Stereographic (alternative") method
 * implemented in createObliqueStereographic().
 *
 * @param properties See \ref general_properties of the conversion. If the name
 * is not provided, it is automatically set.
 * @param centerLat See \ref center_latitude
 * @param centerLong See \ref center_longitude
 * @param scale See \ref scale
 * @param falseEasting See \ref false_easting
 * @param falseNorthing See \ref false_northing
 * @return a new Conversion.
 */
ConversionNNPtr Conversion::createStereographic(
    const util::PropertyMap &properties, const common::Angle &centerLat,
    const common::Angle &centerLong, const common::Scale &scale,
    const common::Length &falseEasting, const common::Length &falseNorthing) {
    return create(properties, PROJ_WKT2_NAME_METHOD_STEREOGRAPHIC,
                  createParams(centerLat, centerLong, scale, falseEasting,
                               falseNorthing));
}

// ---------------------------------------------------------------------------

/** \brief Instantiate a conversion based on the [Van der Grinten]
 * (https://proj.org/operations/projections/vandg.html) projection method.
 *
 * There is no equivalent in EPSG.
 *
 * @param properties See \ref general_properties of the conversion. If the name
 * is not provided, it is automatically set.
 * @param centerLong See \ref center_longitude
 * @param falseEasting See \ref false_easting
 * @param falseNorthing See \ref false_northing
 * @return a new Conversion.
 */
ConversionNNPtr Conversion::createVanDerGrinten(
    const util::PropertyMap &properties, const common::Angle &centerLong,
    const common::Length &falseEasting, const common::Length &falseNorthing) {
    return create(properties, PROJ_WKT2_NAME_METHOD_VAN_DER_GRINTEN,
                  createParams(centerLong, falseEasting, falseNorthing));
}

// ---------------------------------------------------------------------------

/** \brief Instantiate a conversion based on the [Wagner I]
 * (https://proj.org/operations/projections/wag1.html) projection method.
 *
 * There is no equivalent in EPSG.
 *
 * @param properties See \ref general_properties of the conversion. If the name
 * is not provided, it is automatically set.
 * @param centerLong See \ref center_longitude
 * @param falseEasting See \ref false_easting
 * @param falseNorthing See \ref false_northing
 * @return a new Conversion.
 */
ConversionNNPtr Conversion::createWagnerI(const util::PropertyMap &properties,
                                          const common::Angle &centerLong,
                                          const common::Length &falseEasting,
                                          const common::Length &falseNorthing) {
    return create(properties, PROJ_WKT2_NAME_METHOD_WAGNER_I,
                  createParams(centerLong, falseEasting, falseNorthing));
}

// ---------------------------------------------------------------------------

/** \brief Instantiate a conversion based on the [Wagner II]
 * (https://proj.org/operations/projections/wag2.html) projection method.
 *
 * There is no equivalent in EPSG.
 *
 * @param properties See \ref general_properties of the conversion. If the name
 * is not provided, it is automatically set.
 * @param centerLong See \ref center_longitude
 * @param falseEasting See \ref false_easting
 * @param falseNorthing See \ref false_northing
 * @return a new Conversion.
 */
ConversionNNPtr Conversion::createWagnerII(
    const util::PropertyMap &properties, const common::Angle &centerLong,
    const common::Length &falseEasting, const common::Length &falseNorthing) {
    return create(properties, PROJ_WKT2_NAME_METHOD_WAGNER_II,
                  createParams(centerLong, falseEasting, falseNorthing));
}

// ---------------------------------------------------------------------------

/** \brief Instantiate a conversion based on the [Wagner III]
 * (https://proj.org/operations/projections/wag3.html) projection method.
 *
 * There is no equivalent in EPSG.
 *
 * @param properties See \ref general_properties of the conversion. If the name
 * is not provided, it is automatically set.
 * @param latitudeTrueScale Latitude of true scale.
 * @param centerLong See \ref center_longitude
 * @param falseEasting See \ref false_easting
 * @param falseNorthing See \ref false_northing
 * @return a new Conversion.
 */
ConversionNNPtr Conversion::createWagnerIII(
    const util::PropertyMap &properties, const common::Angle &latitudeTrueScale,
    const common::Angle &centerLong, const common::Length &falseEasting,
    const common::Length &falseNorthing) {
    return create(properties, PROJ_WKT2_NAME_METHOD_WAGNER_III,
                  createParams(latitudeTrueScale, centerLong, falseEasting,
                               falseNorthing));
}

// ---------------------------------------------------------------------------

/** \brief Instantiate a conversion based on the [Wagner IV]
 * (https://proj.org/operations/projections/wag4.html) projection method.
 *
 * There is no equivalent in EPSG.
 *
 * @param properties See \ref general_properties of the conversion. If the name
 * is not provided, it is automatically set.
 * @param centerLong See \ref center_longitude
 * @param falseEasting See \ref false_easting
 * @param falseNorthing See \ref false_northing
 * @return a new Conversion.
 */
ConversionNNPtr Conversion::createWagnerIV(
    const util::PropertyMap &properties, const common::Angle &centerLong,
    const common::Length &falseEasting, const common::Length &falseNorthing) {
    return create(properties, PROJ_WKT2_NAME_METHOD_WAGNER_IV,
                  createParams(centerLong, falseEasting, falseNorthing));
}

// ---------------------------------------------------------------------------

/** \brief Instantiate a conversion based on the [Wagner V]
 * (https://proj.org/operations/projections/wag5.html) projection method.
 *
 * There is no equivalent in EPSG.
 *
 * @param properties See \ref general_properties of the conversion. If the name
 * is not provided, it is automatically set.
 * @param centerLong See \ref center_longitude
 * @param falseEasting See \ref false_easting
 * @param falseNorthing See \ref false_northing
 * @return a new Conversion.
 */
ConversionNNPtr Conversion::createWagnerV(const util::PropertyMap &properties,
                                          const common::Angle &centerLong,
                                          const common::Length &falseEasting,
                                          const common::Length &falseNorthing) {
    return create(properties, PROJ_WKT2_NAME_METHOD_WAGNER_V,
                  createParams(centerLong, falseEasting, falseNorthing));
}

// ---------------------------------------------------------------------------

/** \brief Instantiate a conversion based on the [Wagner VI]
 * (https://proj.org/operations/projections/wag6.html) projection method.
 *
 * There is no equivalent in EPSG.
 *
 * @param properties See \ref general_properties of the conversion. If the name
 * is not provided, it is automatically set.
 * @param centerLong See \ref center_longitude
 * @param falseEasting See \ref false_easting
 * @param falseNorthing See \ref false_northing
 * @return a new Conversion.
 */
ConversionNNPtr Conversion::createWagnerVI(
    const util::PropertyMap &properties, const common::Angle &centerLong,
    const common::Length &falseEasting, const common::Length &falseNorthing) {
    return create(properties, PROJ_WKT2_NAME_METHOD_WAGNER_VI,
                  createParams(centerLong, falseEasting, falseNorthing));
}

// ---------------------------------------------------------------------------

/** \brief Instantiate a conversion based on the [Wagner VII]
 * (https://proj.org/operations/projections/wag7.html) projection method.
 *
 * There is no equivalent in EPSG.
 *
 * @param properties See \ref general_properties of the conversion. If the name
 * is not provided, it is automatically set.
 * @param centerLong See \ref center_longitude
 * @param falseEasting See \ref false_easting
 * @param falseNorthing See \ref false_northing
 * @return a new Conversion.
 */
ConversionNNPtr Conversion::createWagnerVII(
    const util::PropertyMap &properties, const common::Angle &centerLong,
    const common::Length &falseEasting, const common::Length &falseNorthing) {
    return create(properties, PROJ_WKT2_NAME_METHOD_WAGNER_VII,
                  createParams(centerLong, falseEasting, falseNorthing));
}

// ---------------------------------------------------------------------------

/** \brief Instantiate a conversion based on the [Quadrilateralized Spherical
 *Cube]
 *(https://proj.org/operations/projections/qsc.html) projection method.
 *
 * There is no equivalent in EPSG.
 *
 * @param properties See \ref general_properties of the conversion. If the name
 * is not provided, it is automatically set.
 * @param centerLat See \ref center_latitude
 * @param centerLong See \ref center_longitude
 * @param falseEasting See \ref false_easting
 * @param falseNorthing See \ref false_northing
 * @return a new Conversion.
 */
ConversionNNPtr Conversion::createQuadrilateralizedSphericalCube(
    const util::PropertyMap &properties, const common::Angle &centerLat,
    const common::Angle &centerLong, const common::Length &falseEasting,
    const common::Length &falseNorthing) {
    return create(
        properties, PROJ_WKT2_NAME_METHOD_QUADRILATERALIZED_SPHERICAL_CUBE,
        createParams(centerLat, centerLong, falseEasting, falseNorthing));
}

// ---------------------------------------------------------------------------

/** \brief Instantiate a conversion based on the [Spherical Cross-Track Height]
 *(https://proj.org/operations/projections/sch.html) projection method.
 *
 * There is no equivalent in EPSG.
 *
 * @param properties See \ref general_properties of the conversion. If the name
 * is not provided, it is automatically set.
 * @param pegPointLat Peg point latitude.
 * @param pegPointLong Peg point longitude.
 * @param pegPointHeading Peg point heading.
 * @param pegPointHeight Peg point height.
 * @return a new Conversion.
 */
ConversionNNPtr Conversion::createSphericalCrossTrackHeight(
    const util::PropertyMap &properties, const common::Angle &pegPointLat,
    const common::Angle &pegPointLong, const common::Angle &pegPointHeading,
    const common::Length &pegPointHeight) {
    return create(properties,
                  PROJ_WKT2_NAME_METHOD_SPHERICAL_CROSS_TRACK_HEIGHT,
                  createParams(pegPointLat, pegPointLong, pegPointHeading,
                               pegPointHeight));
}

// ---------------------------------------------------------------------------

/** \brief Instantiate a conversion based on the [Equal Earth]
 * (https://proj.org/operations/projections/eqearth.html) projection method.
 *
 * This method is defined as [EPSG:1078]
 * (https://www.epsg-registry.org/export.htm?gml=urn:ogc:def:method:EPSG::1078)
 *
 * @param properties See \ref general_properties of the conversion. If the name
 * is not provided, it is automatically set.
 * @param centerLong See \ref center_longitude
 * @param falseEasting See \ref false_easting
 * @param falseNorthing See \ref false_northing
 * @return a new Conversion.
 */
ConversionNNPtr Conversion::createEqualEarth(
    const util::PropertyMap &properties, const common::Angle &centerLong,
    const common::Length &falseEasting, const common::Length &falseNorthing) {
    return create(properties, EPSG_CODE_METHOD_EQUAL_EARTH,
                  createParams(centerLong, falseEasting, falseNorthing));
}

// ---------------------------------------------------------------------------

/** \brief Instantiate a conversion based on the [Vertical Perspective]
 * (https://proj.org/operations/projections/nsper.html) projection method.
 *
 * This method is defined as [EPSG:9838]
 * (https://www.epsg-registry.org/export.htm?gml=urn:ogc:def:method:EPSG::9838)
 *
 * The PROJ implementation of the EPSG Vertical Perspective has the current
 * limitations with respect to the method described in EPSG:
 * <ul>
 * <li> it is a 2D-only method, ignoring the ellipsoidal height of the point to
 *      project.</li>
 * <li> it has only a spherical development.</li>
 * <li> the height of the topocentric origin is ignored, and thus assumed to be
 * 0.</li>
 * </ul>
 *
 * For completeness, PROJ adds the falseEasting and falseNorthing parameter,
 * which are not described in EPSG. They should usually be set to 0.
 *
 * @param properties See \ref general_properties of the conversion. If the name
 * is not provided, it is automatically set.
 * @param topoOriginLat Latitude of topocentric origin
 * @param topoOriginLong Longitude of topocentric origin
 * @param topoOriginHeight Ellipsoidal height of topocentric origin. Ignored by
 * PROJ (that is assumed to be 0)
 * @param viewPointHeight Viewpoint height with respect to the
 * topocentric/mapping plane. In the case where topoOriginHeight = 0, this is
 * the height above the ellipsoid surface at topoOriginLat, topoOriginLong.
 * @param falseEasting See \ref false_easting . (not in EPSG)
 * @param falseNorthing See \ref false_northing . (not in EPSG)
 * @return a new Conversion.
 *
 * @since 6.3
 */
ConversionNNPtr Conversion::createVerticalPerspective(
    const util::PropertyMap &properties, const common::Angle &topoOriginLat,
    const common::Angle &topoOriginLong, const common::Length &topoOriginHeight,
    const common::Length &viewPointHeight, const common::Length &falseEasting,
    const common::Length &falseNorthing) {
    return create(properties, EPSG_CODE_METHOD_VERTICAL_PERSPECTIVE,
                  createParams(topoOriginLat, topoOriginLong, topoOriginHeight,
                               viewPointHeight, falseEasting, falseNorthing));
}

// ---------------------------------------------------------------------------

/** \brief Instantiate a conversion based on the Pole Rotation method, using
 * the conventions of the GRIB 1 and GRIB 2 data formats.
 *
 * Those are mentioned in the Note 2 of
 * https://www.nco.ncep.noaa.gov/pmb/docs/grib2/grib2_doc/grib2_temp3-1.shtml
 *
 * Several conventions for the pole rotation method exists.
 * The parameters provided in this method are remapped to the PROJ ob_tran
 * operation with:
 * <pre>
 * +proj=ob_tran +o_proj=longlat +o_lon_p=-rotationAngle
 *                               +o_lat_p=-southPoleLatInUnrotatedCRS
 *                               +lon_0=southPoleLongInUnrotatedCRS
 * </pre>
 *
 * Another implementation of that convention is also in the netcdf-java library:
 * https://github.com/Unidata/netcdf-java/blob/3ce72c0cd167609ed8c69152bb4a004d1daa9273/cdm/core/src/main/java/ucar/unidata/geoloc/projection/RotatedLatLon.java
 *
 * The PROJ implementation of this method assumes a spherical ellipsoid.
 *
 * @param properties See \ref general_properties of the conversion. If the name
 * is not provided, it is automatically set.
 * @param southPoleLatInUnrotatedCRS Latitude of the point from the unrotated
 * CRS, expressed in the unrotated CRS, that will become the south pole of the
 * rotated CRS.
 * @param southPoleLongInUnrotatedCRS Longitude of the point from the unrotated
 * CRS, expressed in the unrotated CRS, that will become the south pole of the
 * rotated CRS.
 * @param axisRotation The angle of rotation about the new polar
 * axis (measured clockwise when looking from the southern to the northern pole)
 * of the coordinate system, assuming the new axis to have been obtained by
 * first rotating the sphere through southPoleLongInUnrotatedCRS degrees about
 * the geographic polar axis and then rotating through
 * (90 + southPoleLatInUnrotatedCRS) degrees so that the southern pole moved
 * along the (previously rotated) Greenwich meridian.
 * @return a new Conversion.
 *
 * @since 7.0
 */
ConversionNNPtr Conversion::createPoleRotationGRIBConvention(
    const util::PropertyMap &properties,
    const common::Angle &southPoleLatInUnrotatedCRS,
    const common::Angle &southPoleLongInUnrotatedCRS,
    const common::Angle &axisRotation) {
    return create(properties,
                  PROJ_WKT2_NAME_METHOD_POLE_ROTATION_GRIB_CONVENTION,
                  createParams(southPoleLatInUnrotatedCRS,
                               southPoleLongInUnrotatedCRS, axisRotation));
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress

static OperationParameterNNPtr createOpParamNameEPSGCode(int code) {
    const char *name = OperationParameter::getNameForEPSGCode(code);
    assert(name);
    return OperationParameter::create(createMapNameEPSGCode(name, code));
}
//! @endcond

// ---------------------------------------------------------------------------

/** \brief Instantiate a conversion based on the Change of Vertical Unit
 * method.
 *
 * This method is defined as [EPSG:1069]
 * (https://www.epsg-registry.org/export.htm?gml=urn:ogc:def:method:EPSG::1069)
 *
 * @param properties See \ref general_properties of the conversion. If the name
 * is not provided, it is automatically set.
 * @param factor Conversion factor
 * @return a new Conversion.
 */
ConversionNNPtr
Conversion::createChangeVerticalUnit(const util::PropertyMap &properties,
                                     const common::Scale &factor) {
    return create(properties, createMethodMapNameEPSGCode(
                                  EPSG_CODE_METHOD_CHANGE_VERTICAL_UNIT),
                  VectorOfParameters{
                      createOpParamNameEPSGCode(
                          EPSG_CODE_PARAMETER_UNIT_CONVERSION_SCALAR),
                  },
                  VectorOfValues{
                      factor,
                  });
}

// ---------------------------------------------------------------------------

/** \brief Instantiate a conversion based on the Height Depth Reversal
 * method.
 *
 * This method is defined as [EPSG:1068]
 * (https://www.epsg-registry.org/export.htm?gml=urn:ogc:def:method:EPSG::1068)
 *
 * @param properties See \ref general_properties of the conversion. If the name
 * is not provided, it is automatically set.
 * @return a new Conversion.
 * @since 6.3
 */
ConversionNNPtr
Conversion::createHeightDepthReversal(const util::PropertyMap &properties) {
    return create(properties, createMethodMapNameEPSGCode(
                                  EPSG_CODE_METHOD_HEIGHT_DEPTH_REVERSAL),
                  {}, {});
}

// ---------------------------------------------------------------------------

/** \brief Instantiate a conversion based on the Axis order reversal method
 *
 * This swaps the longitude, latitude axis.
 *
 * This method is defined as [EPSG:9843]
 * (https://www.epsg-registry.org/export.htm?gml=urn:ogc:def:method:EPSG::9843),
 * or for 3D as [EPSG:9844]
 * (https://www.epsg-registry.org/export.htm?gml=urn:ogc:def:method:EPSG::9844)
 *
 * @param is3D Whether this should apply on 3D geographicCRS
 * @return a new Conversion.
 */
ConversionNNPtr Conversion::createAxisOrderReversal(bool is3D) {
    if (is3D) {
        return create(createMapNameEPSGCode(AXIS_ORDER_CHANGE_3D_NAME, 15499),
                      createMethodMapNameEPSGCode(
                          EPSG_CODE_METHOD_AXIS_ORDER_REVERSAL_3D),
                      {}, {});
    } else {
        return create(createMapNameEPSGCode(AXIS_ORDER_CHANGE_2D_NAME, 15498),
                      createMethodMapNameEPSGCode(
                          EPSG_CODE_METHOD_AXIS_ORDER_REVERSAL_2D),
                      {}, {});
    }
}

// ---------------------------------------------------------------------------

/** \brief Instantiate a conversion based on the Geographic/Geocentric method.
 *
 * This method is defined as [EPSG:9602]
 * (https://www.epsg-registry.org/export.htm?gml=urn:ogc:def:method:EPSG::9602),
 *
 * @param properties See \ref general_properties of the conversion. If the name
 * is not provided, it is automatically set.
 * @return a new Conversion.
 */
ConversionNNPtr
Conversion::createGeographicGeocentric(const util::PropertyMap &properties) {
    return create(properties, createMethodMapNameEPSGCode(
                                  EPSG_CODE_METHOD_GEOGRAPHIC_GEOCENTRIC),
                  {}, {});
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress

static const char *getCRSQualifierStr(const crs::CRSPtr &crs) {
    auto geod = dynamic_cast<crs::GeodeticCRS *>(crs.get());
    if (geod) {
        if (geod->isGeocentric()) {
            return " (geocentric)";
        }
        auto geog = dynamic_cast<crs::GeographicCRS *>(geod);
        if (geog) {
            if (geog->coordinateSystem()->axisList().size() == 2) {
                return " (geog2D)";
            } else {
                return " (geog3D)";
            }
        }
    }
    return "";
}

// ---------------------------------------------------------------------------

static std::string buildOpName(const char *opType, const crs::CRSPtr &source,
                               const crs::CRSPtr &target) {
    std::string res(opType);
    const auto &srcName = source->nameStr();
    const auto &targetName = target->nameStr();
    const char *srcQualifier = "";
    const char *targetQualifier = "";
    if (srcName == targetName) {
        srcQualifier = getCRSQualifierStr(source);
        targetQualifier = getCRSQualifierStr(target);
        if (strcmp(srcQualifier, targetQualifier) == 0) {
            srcQualifier = "";
            targetQualifier = "";
        }
    }
    res += " from ";
    res += srcName;
    res += srcQualifier;
    res += " to ";
    res += targetName;
    res += targetQualifier;
    return res;
}

// ---------------------------------------------------------------------------

ConversionNNPtr
Conversion::createGeographicGeocentric(const crs::CRSNNPtr &sourceCRS,
                                       const crs::CRSNNPtr &targetCRS) {
    auto properties = util::PropertyMap().set(
        common::IdentifiedObject::NAME_KEY,
        buildOpName("Conversion", sourceCRS, targetCRS));
    auto conv = createGeographicGeocentric(properties);
    conv->setCRSs(sourceCRS, targetCRS, nullptr);
    return conv;
}
// ---------------------------------------------------------------------------

static util::PropertyMap &addDomains(util::PropertyMap &map,
                                     const common::ObjectUsage *obj) {

    auto ar = util::ArrayOfBaseObject::create();
    for (const auto &domain : obj->domains()) {
        ar->add(domain);
    }
    if (!ar->empty()) {
        map.set(common::ObjectUsage::OBJECT_DOMAIN_KEY, ar);
    }
    return map;
}

// ---------------------------------------------------------------------------

static void addModifiedIdentifier(util::PropertyMap &map,
                                  const common::IdentifiedObject *obj,
                                  bool inverse, bool derivedFrom) {
    // If original operation is AUTH:CODE, then assign INVERSE(AUTH):CODE
    // as identifier.

    auto ar = util::ArrayOfBaseObject::create();
    for (const auto &idSrc : obj->identifiers()) {
        auto authName = *(idSrc->codeSpace());
        const auto &srcCode = idSrc->code();
        if (derivedFrom) {
            authName = concat("DERIVED_FROM(", authName, ")");
        }
        if (inverse) {
            if (starts_with(authName, "INVERSE(") && authName.back() == ')') {
                authName = authName.substr(strlen("INVERSE("));
                authName.resize(authName.size() - 1);
            } else {
                authName = concat("INVERSE(", authName, ")");
            }
        }
        auto idsProp = util::PropertyMap().set(
            metadata::Identifier::CODESPACE_KEY, authName);
        ar->add(metadata::Identifier::create(srcCode, idsProp));
    }
    if (!ar->empty()) {
        map.set(common::IdentifiedObject::IDENTIFIERS_KEY, ar);
    }
}

// ---------------------------------------------------------------------------

static util::PropertyMap
createPropertiesForInverse(const OperationMethodNNPtr &method) {
    util::PropertyMap map;

    const std::string &forwardName = method->nameStr();
    if (!forwardName.empty()) {
        if (starts_with(forwardName, INVERSE_OF)) {
            map.set(common::IdentifiedObject::NAME_KEY,
                    forwardName.substr(INVERSE_OF.size()));
        } else {
            map.set(common::IdentifiedObject::NAME_KEY,
                    INVERSE_OF + forwardName);
        }
    }

    addModifiedIdentifier(map, method.get(), true, false);

    return map;
}

// ---------------------------------------------------------------------------

InverseConversion::InverseConversion(const ConversionNNPtr &forward)
    : Conversion(
          OperationMethod::create(createPropertiesForInverse(forward->method()),
                                  forward->method()->parameters()),
          forward->parameterValues()),
      InverseCoordinateOperation(forward, true) {
    setPropertiesFromForward();
}

// ---------------------------------------------------------------------------

InverseConversion::~InverseConversion() = default;

// ---------------------------------------------------------------------------

ConversionNNPtr InverseConversion::inverseAsConversion() const {
    return NN_NO_CHECK(
        util::nn_dynamic_pointer_cast<Conversion>(forwardOperation_));
}

// ---------------------------------------------------------------------------

CoordinateOperationNNPtr
InverseConversion::create(const ConversionNNPtr &forward) {
    auto conv = util::nn_make_shared<InverseConversion>(forward);
    conv->assignSelf(conv);
    return conv;
}

// ---------------------------------------------------------------------------

CoordinateOperationNNPtr InverseConversion::_shallowClone() const {
    auto op = InverseConversion::nn_make_shared<InverseConversion>(
        inverseAsConversion()->shallowClone());
    op->assignSelf(op);
    op->setCRSs(this, false);
    return util::nn_static_pointer_cast<CoordinateOperation>(op);
}

//! @endcond

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress

static bool isAxisOrderReversal2D(int methodEPSGCode) {
    return methodEPSGCode == EPSG_CODE_METHOD_AXIS_ORDER_REVERSAL_2D;
}

static bool isAxisOrderReversal3D(int methodEPSGCode) {
    return methodEPSGCode == EPSG_CODE_METHOD_AXIS_ORDER_REVERSAL_3D;
}

bool isAxisOrderReversal(int methodEPSGCode) {
    return isAxisOrderReversal2D(methodEPSGCode) ||
           isAxisOrderReversal3D(methodEPSGCode);
}
//! @endcond

// ---------------------------------------------------------------------------

CoordinateOperationNNPtr Conversion::inverse() const {
    const int methodEPSGCode = method()->getEPSGCode();

    if (methodEPSGCode == EPSG_CODE_METHOD_CHANGE_VERTICAL_UNIT) {
        const double convFactor = parameterValueNumericAsSI(
            EPSG_CODE_PARAMETER_UNIT_CONVERSION_SCALAR);
        auto conv = createChangeVerticalUnit(
            createPropertiesForInverse(this, false, false),
            common::Scale(1.0 / convFactor));
        conv->setCRSs(this, true);
        return conv;
    }

    const bool l_isAxisOrderReversal2D = isAxisOrderReversal2D(methodEPSGCode);
    const bool l_isAxisOrderReversal3D = isAxisOrderReversal3D(methodEPSGCode);
    if (l_isAxisOrderReversal2D || l_isAxisOrderReversal3D) {
        auto conv = createAxisOrderReversal(l_isAxisOrderReversal3D);
        conv->setCRSs(this, true);
        return conv;
    }

    if (methodEPSGCode == EPSG_CODE_METHOD_GEOGRAPHIC_GEOCENTRIC) {

        auto conv = createGeographicGeocentric(
            createPropertiesForInverse(this, false, false));
        conv->setCRSs(this, true);
        return conv;
    }

    if (methodEPSGCode == EPSG_CODE_METHOD_HEIGHT_DEPTH_REVERSAL) {

        auto conv = createHeightDepthReversal(
            createPropertiesForInverse(this, false, false));
        conv->setCRSs(this, true);
        return conv;
    }

    return InverseConversion::create(NN_NO_CHECK(
        util::nn_dynamic_pointer_cast<Conversion>(shared_from_this())));
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress

static double msfn(double phi, double e2) {
    const double sinphi = std::sin(phi);
    const double cosphi = std::cos(phi);
    return pj_msfn(sinphi, cosphi, e2);
}

// ---------------------------------------------------------------------------

static double tsfn(double phi, double ec) {
    const double sinphi = std::sin(phi);
    return pj_tsfn(phi, sinphi, ec);
}

// ---------------------------------------------------------------------------

// Function whose zeroes are the sin of the standard parallels of LCC_2SP
static double lcc_1sp_to_2sp_f(double sinphi, double K, double ec, double n) {
    const double x = sinphi;
    const double ecx = ec * x;
    return (1 - x * x) / (1 - ecx * ecx) -
           K * K * std::pow((1.0 - x) / (1.0 + x) *
                                std::pow((1.0 + ecx) / (1.0 - ecx), ec),
                            n);
}

// ---------------------------------------------------------------------------

// Find the sin of the standard parallels of LCC_2SP
static double find_zero_lcc_1sp_to_2sp_f(double sinphi0, bool bNorth, double K,
                                         double ec) {
    double a, b;
    double f_a;
    if (bNorth) {
        // Look for zero above phi0
        a = sinphi0;
        b = 1.0;   // sin(North pole)
        f_a = 1.0; // some positive value, but we only care about the sign
    } else {
        // Look for zero below phi0
        a = -1.0; // sin(South pole)
        b = sinphi0;
        f_a = -1.0; // minus infinity in fact, but we only care about the sign
    }
    // We use dichotomy search. lcc_1sp_to_2sp_f() is positive at sinphi_init,
    // has a zero in ]-1,sinphi0[ and ]sinphi0,1[ ranges
    for (int N = 0; N < 100; N++) {
        double c = (a + b) / 2;
        double f_c = lcc_1sp_to_2sp_f(c, K, ec, sinphi0);
        if (f_c == 0.0 || (b - a) < 1e-18) {
            return c;
        }
        if ((f_c > 0 && f_a > 0) || (f_c < 0 && f_a < 0)) {
            a = c;
            f_a = f_c;
        } else {
            b = c;
        }
    }
    return (a + b) / 2;
}

static inline double DegToRad(double x) { return x / 180.0 * M_PI; }
static inline double RadToDeg(double x) { return x / M_PI * 180.0; }

//! @endcond

// ---------------------------------------------------------------------------

/**
 * \brief Return an equivalent projection.
 *
 * Currently implemented:
 * <ul>
 * <li>EPSG_CODE_METHOD_MERCATOR_VARIANT_A (1SP) to
 * EPSG_CODE_METHOD_MERCATOR_VARIANT_B (2SP)</li>
 * <li>EPSG_CODE_METHOD_MERCATOR_VARIANT_B (2SP) to
 * EPSG_CODE_METHOD_MERCATOR_VARIANT_A (1SP)</li>
 * <li>EPSG_CODE_METHOD_LAMBERT_CONIC_CONFORMAL_1SP to
 * EPSG_CODE_METHOD_LAMBERT_CONIC_CONFORMAL_2SP</li>
 * <li>EPSG_CODE_METHOD_LAMBERT_CONIC_CONFORMAL_2SP to
 * EPSG_CODE_METHOD_LAMBERT_CONIC_CONFORMAL_1SP</li>
 * </ul>
 *
 * @param targetEPSGCode EPSG code of the target method.
 * @return new conversion, or nullptr
 */
ConversionPtr Conversion::convertToOtherMethod(int targetEPSGCode) const {
    const int current_epsg_code = method()->getEPSGCode();
    if (current_epsg_code == targetEPSGCode) {
        return util::nn_dynamic_pointer_cast<Conversion>(shared_from_this());
    }

    auto geogCRS = dynamic_cast<crs::GeodeticCRS *>(sourceCRS().get());
    if (!geogCRS) {
        return nullptr;
    }

    const double e2 = geogCRS->ellipsoid()->squaredEccentricity();
    if (e2 < 0) {
        return nullptr;
    }

    if (current_epsg_code == EPSG_CODE_METHOD_MERCATOR_VARIANT_A &&
        targetEPSGCode == EPSG_CODE_METHOD_MERCATOR_VARIANT_B &&
        parameterValueNumericAsSI(
            EPSG_CODE_PARAMETER_LATITUDE_OF_NATURAL_ORIGIN) == 0.0) {
        const double k0 = parameterValueNumericAsSI(
            EPSG_CODE_PARAMETER_SCALE_FACTOR_AT_NATURAL_ORIGIN);
        if (!(k0 > 0 && k0 <= 1.0 + 1e-10))
            return nullptr;
        const double dfStdP1Lat =
            (k0 >= 1.0)
                ? 0.0
                : std::acos(std::sqrt((1.0 - e2) / ((1.0 / (k0 * k0)) - e2)));
        auto latitudeFirstParallel = common::Angle(
            common::Angle(dfStdP1Lat, common::UnitOfMeasure::RADIAN)
                .convertToUnit(common::UnitOfMeasure::DEGREE),
            common::UnitOfMeasure::DEGREE);
        auto conv = createMercatorVariantB(
            util::PropertyMap(), latitudeFirstParallel,
            common::Angle(parameterValueMeasure(
                EPSG_CODE_PARAMETER_LONGITUDE_OF_NATURAL_ORIGIN)),
            common::Length(
                parameterValueMeasure(EPSG_CODE_PARAMETER_FALSE_EASTING)),
            common::Length(
                parameterValueMeasure(EPSG_CODE_PARAMETER_FALSE_NORTHING)));
        conv->setCRSs(this, false);
        return conv.as_nullable();
    }

    if (current_epsg_code == EPSG_CODE_METHOD_MERCATOR_VARIANT_B &&
        targetEPSGCode == EPSG_CODE_METHOD_MERCATOR_VARIANT_A) {
        const double phi1 = parameterValueNumericAsSI(
            EPSG_CODE_PARAMETER_LATITUDE_1ST_STD_PARALLEL);
        if (!(std::fabs(phi1) < M_PI / 2))
            return nullptr;
        const double k0 = msfn(phi1, e2);
        auto conv = createMercatorVariantA(
            util::PropertyMap(),
            common::Angle(0.0, common::UnitOfMeasure::DEGREE),
            common::Angle(parameterValueMeasure(
                EPSG_CODE_PARAMETER_LONGITUDE_OF_NATURAL_ORIGIN)),
            common::Scale(k0, common::UnitOfMeasure::SCALE_UNITY),
            common::Length(
                parameterValueMeasure(EPSG_CODE_PARAMETER_FALSE_EASTING)),
            common::Length(
                parameterValueMeasure(EPSG_CODE_PARAMETER_FALSE_NORTHING)));
        conv->setCRSs(this, false);
        return conv.as_nullable();
    }

    if (current_epsg_code == EPSG_CODE_METHOD_LAMBERT_CONIC_CONFORMAL_1SP &&
        targetEPSGCode == EPSG_CODE_METHOD_LAMBERT_CONIC_CONFORMAL_2SP) {
        // Notations m0, t0, n, m1, t1, F are those of the EPSG guidance
        // "1.3.1.1 Lambert Conic Conformal (2SP)" and
        // "1.3.1.2 Lambert Conic Conformal (1SP)" and
        // or Snyder pages 106-109
        auto latitudeOfOrigin = common::Angle(parameterValueMeasure(
            EPSG_CODE_PARAMETER_LATITUDE_OF_NATURAL_ORIGIN));
        const double phi0 = latitudeOfOrigin.getSIValue();
        const double k0 = parameterValueNumericAsSI(
            EPSG_CODE_PARAMETER_SCALE_FACTOR_AT_NATURAL_ORIGIN);
        if (!(std::fabs(phi0) < M_PI / 2))
            return nullptr;
        if (!(k0 > 0 && k0 <= 1.0 + 1e-10))
            return nullptr;
        const double ec = std::sqrt(e2);
        const double m0 = msfn(phi0, e2);
        const double t0 = tsfn(phi0, ec);
        const double n = sin(phi0);
        if (std::fabs(n) < 1e-10)
            return nullptr;
        if (fabs(k0 - 1.0) <= 1e-10) {
            auto conv = createLambertConicConformal_2SP(
                util::PropertyMap(), latitudeOfOrigin,
                common::Angle(parameterValueMeasure(
                    EPSG_CODE_PARAMETER_LONGITUDE_OF_NATURAL_ORIGIN)),
                latitudeOfOrigin, latitudeOfOrigin,
                common::Length(
                    parameterValueMeasure(EPSG_CODE_PARAMETER_FALSE_EASTING)),
                common::Length(
                    parameterValueMeasure(EPSG_CODE_PARAMETER_FALSE_NORTHING)));
            conv->setCRSs(this, false);
            return conv.as_nullable();
        } else {
            const double K = k0 * m0 / std::pow(t0, n);
            const double phi1 =
                std::asin(find_zero_lcc_1sp_to_2sp_f(n, true, K, ec));
            const double phi2 =
                std::asin(find_zero_lcc_1sp_to_2sp_f(n, false, K, ec));
            double phi1Deg = RadToDeg(phi1);
            double phi2Deg = RadToDeg(phi2);

            // Try to round to hundreth of degree if very close to it
            if (std::fabs(phi1Deg * 1000 - std::floor(phi1Deg * 1000 + 0.5)) <
                1e-8)
                phi1Deg = floor(phi1Deg * 1000 + 0.5) / 1000;
            if (std::fabs(phi2Deg * 1000 - std::floor(phi2Deg * 1000 + 0.5)) <
                1e-8)
                phi2Deg = std::floor(phi2Deg * 1000 + 0.5) / 1000;

            // The following improvement is too turn the LCC1SP equivalent of
            // EPSG:2154 to the real LCC2SP
            // If the computed latitude of origin is close to .0 or .5 degrees
            // then check if rounding it to it will get a false northing
            // close to an integer
            const double FN =
                parameterValueNumericAsSI(EPSG_CODE_PARAMETER_FALSE_NORTHING);
            const double latitudeOfOriginDeg =
                latitudeOfOrigin.convertToUnit(common::UnitOfMeasure::DEGREE);
            if (std::fabs(latitudeOfOriginDeg * 2 -
                          std::floor(latitudeOfOriginDeg * 2 + 0.5)) < 0.2) {
                const double dfRoundedLatOfOrig =
                    std::floor(latitudeOfOriginDeg * 2 + 0.5) / 2;
                const double m1 = msfn(phi1, e2);
                const double t1 = tsfn(phi1, ec);
                const double F = m1 / (n * std::pow(t1, n));
                const double a =
                    geogCRS->ellipsoid()->semiMajorAxis().getSIValue();
                const double tRoundedLatOfOrig =
                    tsfn(DegToRad(dfRoundedLatOfOrig), ec);
                const double FN_correction =
                    a * F * (std::pow(tRoundedLatOfOrig, n) - std::pow(t0, n));
                const double FN_corrected = FN - FN_correction;
                const double FN_corrected_rounded =
                    std::floor(FN_corrected + 0.5);
                if (std::fabs(FN_corrected - FN_corrected_rounded) < 1e-8) {
                    auto conv = createLambertConicConformal_2SP(
                        util::PropertyMap(),
                        common::Angle(dfRoundedLatOfOrig,
                                      common::UnitOfMeasure::DEGREE),
                        common::Angle(parameterValueMeasure(
                            EPSG_CODE_PARAMETER_LONGITUDE_OF_NATURAL_ORIGIN)),
                        common::Angle(phi1Deg, common::UnitOfMeasure::DEGREE),
                        common::Angle(phi2Deg, common::UnitOfMeasure::DEGREE),
                        common::Length(parameterValueMeasure(
                            EPSG_CODE_PARAMETER_FALSE_EASTING)),
                        common::Length(FN_corrected_rounded));
                    conv->setCRSs(this, false);
                    return conv.as_nullable();
                }
            }

            auto conv = createLambertConicConformal_2SP(
                util::PropertyMap(), latitudeOfOrigin,
                common::Angle(parameterValueMeasure(
                    EPSG_CODE_PARAMETER_LONGITUDE_OF_NATURAL_ORIGIN)),
                common::Angle(phi1Deg, common::UnitOfMeasure::DEGREE),
                common::Angle(phi2Deg, common::UnitOfMeasure::DEGREE),
                common::Length(
                    parameterValueMeasure(EPSG_CODE_PARAMETER_FALSE_EASTING)),
                common::Length(FN));
            conv->setCRSs(this, false);
            return conv.as_nullable();
        }
    }

    if (current_epsg_code == EPSG_CODE_METHOD_LAMBERT_CONIC_CONFORMAL_2SP &&
        targetEPSGCode == EPSG_CODE_METHOD_LAMBERT_CONIC_CONFORMAL_1SP) {
        // Notations m0, t0, m1, t1, m2, t2 n, F are those of the EPSG guidance
        // "1.3.1.1 Lambert Conic Conformal (2SP)" and
        // "1.3.1.2 Lambert Conic Conformal (1SP)" and
        // or Snyder pages 106-109
        const double phiF =
            parameterValueMeasure(EPSG_CODE_PARAMETER_LATITUDE_FALSE_ORIGIN)
                .getSIValue();
        const double phi1 =
            parameterValueMeasure(EPSG_CODE_PARAMETER_LATITUDE_1ST_STD_PARALLEL)
                .getSIValue();
        const double phi2 =
            parameterValueMeasure(EPSG_CODE_PARAMETER_LATITUDE_2ND_STD_PARALLEL)
                .getSIValue();
        if (!(std::fabs(phiF) < M_PI / 2))
            return nullptr;
        if (!(std::fabs(phi1) < M_PI / 2))
            return nullptr;
        if (!(std::fabs(phi2) < M_PI / 2))
            return nullptr;
        const double ec = std::sqrt(e2);
        const double m1 = msfn(phi1, e2);
        const double m2 = msfn(phi2, e2);
        const double t1 = tsfn(phi1, ec);
        const double t2 = tsfn(phi2, ec);
        const double n_denom = std::log(t1) - std::log(t2);
        const double n = (std::fabs(n_denom) < 1e-10)
                             ? std::sin(phi1)
                             : (std::log(m1) - std::log(m2)) / n_denom;
        if (std::fabs(n) < 1e-10)
            return nullptr;
        const double F = m1 / (n * std::pow(t1, n));
        const double phi0 = std::asin(n);
        const double m0 = msfn(phi0, e2);
        const double t0 = tsfn(phi0, ec);
        const double F0 = m0 / (n * std::pow(t0, n));
        const double k0 = F / F0;
        const double a = geogCRS->ellipsoid()->semiMajorAxis().getSIValue();
        const double tF = tsfn(phiF, ec);
        const double FN_correction =
            a * F * (std::pow(tF, n) - std::pow(t0, n));

        double phi0Deg = RadToDeg(phi0);
        // Try to round to thousandth of degree if very close to it
        if (std::fabs(phi0Deg * 1000 - std::floor(phi0Deg * 1000 + 0.5)) < 1e-8)
            phi0Deg = std::floor(phi0Deg * 1000 + 0.5) / 1000;

        auto conv = createLambertConicConformal_1SP(
            util::PropertyMap(),
            common::Angle(phi0Deg, common::UnitOfMeasure::DEGREE),
            common::Angle(parameterValueMeasure(
                EPSG_CODE_PARAMETER_LONGITUDE_FALSE_ORIGIN)),
            common::Scale(k0), common::Length(parameterValueMeasure(
                                   EPSG_CODE_PARAMETER_EASTING_FALSE_ORIGIN)),
            common::Length(
                parameterValueNumericAsSI(
                    EPSG_CODE_PARAMETER_NORTHING_FALSE_ORIGIN) +
                (std::fabs(FN_correction) > 1e-8 ? FN_correction : 0)));
        conv->setCRSs(this, false);
        return conv.as_nullable();
    }

    return nullptr;
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress

static void getESRIMethodNameAndParams(const Conversion *conv,
                                       const std::string &methodName,
                                       int methodEPSGCode,
                                       const char *&esriMethodName,
                                       const ESRIParamMapping *&esriParams) {
    esriParams = nullptr;
    esriMethodName = nullptr;
    const auto *esriMapping = getESRIMapping(methodName, methodEPSGCode);
    const auto l_targetCRS = conv->targetCRS();
    if (esriMapping) {
        esriParams = esriMapping->params;
        esriMethodName = esriMapping->esri_name;
        if (esriMapping->epsg_code ==
                EPSG_CODE_METHOD_EQUIDISTANT_CYLINDRICAL ||
            esriMapping->epsg_code ==
                EPSG_CODE_METHOD_EQUIDISTANT_CYLINDRICAL_SPHERICAL) {
            if (l_targetCRS &&
                ci_find(l_targetCRS->nameStr(), "Plate Carree") !=
                    std::string::npos &&
                conv->parameterValueNumericAsSI(
                    EPSG_CODE_PARAMETER_LATITUDE_OF_NATURAL_ORIGIN) == 0.0) {
                esriParams = paramsESRI_Plate_Carree;
                esriMethodName = "Plate_Carree";
            } else {
                esriParams = paramsESRI_Equidistant_Cylindrical;
                esriMethodName = "Equidistant_Cylindrical";
            }
        } else if (esriMapping->epsg_code ==
                   EPSG_CODE_METHOD_TRANSVERSE_MERCATOR) {
            if (ci_find(conv->nameStr(), "Gauss Kruger") != std::string::npos ||
                (l_targetCRS && (ci_find(l_targetCRS->nameStr(), "Gauss") !=
                                     std::string::npos ||
                                 ci_find(l_targetCRS->nameStr(), "GK_") !=
                                     std::string::npos))) {
                esriParams = paramsESRI_Gauss_Kruger;
                esriMethodName = "Gauss_Kruger";
            } else {
                esriParams = paramsESRI_Transverse_Mercator;
                esriMethodName = "Transverse_Mercator";
            }
        } else if (esriMapping->epsg_code ==
                   EPSG_CODE_METHOD_HOTINE_OBLIQUE_MERCATOR_VARIANT_A) {
            if (std::abs(
                    conv->parameterValueNumericAsSI(
                        EPSG_CODE_PARAMETER_AZIMUTH_INITIAL_LINE) -
                    conv->parameterValueNumericAsSI(
                        EPSG_CODE_PARAMETER_ANGLE_RECTIFIED_TO_SKEW_GRID)) <
                1e-15) {
                esriParams =
                    paramsESRI_Hotine_Oblique_Mercator_Azimuth_Natural_Origin;
                esriMethodName =
                    "Hotine_Oblique_Mercator_Azimuth_Natural_Origin";
            } else {
                esriParams =
                    paramsESRI_Rectified_Skew_Orthomorphic_Natural_Origin;
                esriMethodName = "Rectified_Skew_Orthomorphic_Natural_Origin";
            }
        } else if (esriMapping->epsg_code ==
                   EPSG_CODE_METHOD_HOTINE_OBLIQUE_MERCATOR_VARIANT_B) {
            if (std::abs(
                    conv->parameterValueNumericAsSI(
                        EPSG_CODE_PARAMETER_AZIMUTH_INITIAL_LINE) -
                    conv->parameterValueNumericAsSI(
                        EPSG_CODE_PARAMETER_ANGLE_RECTIFIED_TO_SKEW_GRID)) <
                1e-15) {
                esriParams = paramsESRI_Hotine_Oblique_Mercator_Azimuth_Center;
                esriMethodName = "Hotine_Oblique_Mercator_Azimuth_Center";
            } else {
                esriParams = paramsESRI_Rectified_Skew_Orthomorphic_Center;
                esriMethodName = "Rectified_Skew_Orthomorphic_Center";
            }
        } else if (esriMapping->epsg_code ==
                   EPSG_CODE_METHOD_POLAR_STEREOGRAPHIC_VARIANT_B) {
            if (conv->parameterValueNumericAsSI(
                    EPSG_CODE_PARAMETER_LATITUDE_STD_PARALLEL) > 0) {
                esriMethodName = "Stereographic_North_Pole";
            } else {
                esriMethodName = "Stereographic_South_Pole";
            }
        }
    }
}

// ---------------------------------------------------------------------------

const char *Conversion::getESRIMethodName() const {
    const auto &l_method = method();
    const auto &methodName = l_method->nameStr();
    const auto methodEPSGCode = l_method->getEPSGCode();
    const ESRIParamMapping *esriParams = nullptr;
    const char *esriMethodName = nullptr;
    getESRIMethodNameAndParams(this, methodName, methodEPSGCode, esriMethodName,
                               esriParams);
    return esriMethodName;
}

//! @endcond

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
const char *Conversion::getWKT1GDALMethodName() const {
    const auto &l_method = method();
    const auto methodEPSGCode = l_method->getEPSGCode();
    if (methodEPSGCode ==
        EPSG_CODE_METHOD_POPULAR_VISUALISATION_PSEUDO_MERCATOR) {
        return "Mercator_1SP";
    }
    const MethodMapping *mapping = getMapping(l_method.get());
    return mapping ? mapping->wkt1_name : nullptr;
}

//! @endcond

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress

void Conversion::_exportToWKT(io::WKTFormatter *formatter) const {
    const auto &l_method = method();
    const auto &methodName = l_method->nameStr();
    const auto methodEPSGCode = l_method->getEPSGCode();
    const bool isWKT2 = formatter->version() == io::WKTFormatter::Version::WKT2;

    if (!isWKT2 && formatter->useESRIDialect()) {
        if (methodEPSGCode == EPSG_CODE_METHOD_MERCATOR_VARIANT_A) {
            auto eqConv =
                convertToOtherMethod(EPSG_CODE_METHOD_MERCATOR_VARIANT_B);
            if (eqConv) {
                eqConv->_exportToWKT(formatter);
                return;
            }
        }
    }

    if (isWKT2) {
        formatter->startNode(formatter->useDerivingConversion()
                                 ? io::WKTConstants::DERIVINGCONVERSION
                                 : io::WKTConstants::CONVERSION,
                             !identifiers().empty());
        formatter->addQuotedString(nameStr());
    } else {
        formatter->enter();
        formatter->pushOutputUnit(false);
        formatter->pushOutputId(false);
    }

#ifdef DEBUG_CONVERSION_ID
    if (sourceCRS() && targetCRS()) {
        formatter->startNode("SOURCECRS_ID", false);
        sourceCRS()->formatID(formatter);
        formatter->endNode();
        formatter->startNode("TARGETCRS_ID", false);
        targetCRS()->formatID(formatter);
        formatter->endNode();
    }
#endif

    bool bAlreadyWritten = false;
    if (!isWKT2 && formatter->useESRIDialect()) {
        const ESRIParamMapping *esriParams = nullptr;
        const char *esriMethodName = nullptr;
        getESRIMethodNameAndParams(this, methodName, methodEPSGCode,
                                   esriMethodName, esriParams);
        if (esriMethodName && esriParams) {
            formatter->startNode(io::WKTConstants::PROJECTION, false);
            formatter->addQuotedString(esriMethodName);
            formatter->endNode();

            for (int i = 0; esriParams[i].esri_name != nullptr; i++) {
                const auto &esriParam = esriParams[i];
                formatter->startNode(io::WKTConstants::PARAMETER, false);
                formatter->addQuotedString(esriParam.esri_name);
                if (esriParam.wkt2_name) {
                    const auto &pv = parameterValue(esriParam.wkt2_name,
                                                    esriParam.epsg_code);
                    if (pv && pv->type() == ParameterValue::Type::MEASURE) {
                        const auto &v = pv->value();
                        // as we don't output the natural unit, output
                        // to the registered linear / angular unit.
                        const auto &unitType = v.unit().type();
                        if (unitType == common::UnitOfMeasure::Type::LINEAR) {
                            formatter->add(v.convertToUnit(
                                *(formatter->axisLinearUnit())));
                        } else if (unitType ==
                                   common::UnitOfMeasure::Type::ANGULAR) {
                            const auto &angUnit =
                                *(formatter->axisAngularUnit());
                            double val = v.convertToUnit(angUnit);
                            if (angUnit == common::UnitOfMeasure::DEGREE) {
                                if (val > 180.0) {
                                    val -= 360.0;
                                } else if (val < -180.0) {
                                    val += 360.0;
                                }
                            }
                            formatter->add(val);
                        } else {
                            formatter->add(v.getSIValue());
                        }
                    } else if (ci_find(esriParam.esri_name, "scale") !=
                               std::string::npos) {
                        formatter->add(1.0);
                    } else {
                        formatter->add(0.0);
                    }
                } else {
                    formatter->add(esriParam.fixed_value);
                }
                formatter->endNode();
            }
            bAlreadyWritten = true;
        }
    } else if (!isWKT2) {
        if (methodEPSGCode ==
            EPSG_CODE_METHOD_POPULAR_VISUALISATION_PSEUDO_MERCATOR) {
            const double latitudeOrigin = parameterValueNumeric(
                EPSG_CODE_PARAMETER_LATITUDE_OF_NATURAL_ORIGIN,
                common::UnitOfMeasure::DEGREE);
            if (latitudeOrigin != 0) {
                throw io::FormattingException(
                    std::string("Unsupported value for ") +
                    EPSG_NAME_PARAMETER_LATITUDE_OF_NATURAL_ORIGIN);
            }

            bAlreadyWritten = true;
            formatter->startNode(io::WKTConstants::PROJECTION, false);
            formatter->addQuotedString("Mercator_1SP");
            formatter->endNode();

            formatter->startNode(io::WKTConstants::PARAMETER, false);
            formatter->addQuotedString("central_meridian");
            const double centralMeridian = parameterValueNumeric(
                EPSG_CODE_PARAMETER_LONGITUDE_OF_NATURAL_ORIGIN,
                common::UnitOfMeasure::DEGREE);
            formatter->add(centralMeridian);
            formatter->endNode();

            formatter->startNode(io::WKTConstants::PARAMETER, false);
            formatter->addQuotedString("scale_factor");
            formatter->add(1.0);
            formatter->endNode();

            formatter->startNode(io::WKTConstants::PARAMETER, false);
            formatter->addQuotedString("false_easting");
            const double falseEasting =
                parameterValueNumericAsSI(EPSG_CODE_PARAMETER_FALSE_EASTING);
            formatter->add(falseEasting);
            formatter->endNode();

            formatter->startNode(io::WKTConstants::PARAMETER, false);
            formatter->addQuotedString("false_northing");
            const double falseNorthing =
                parameterValueNumericAsSI(EPSG_CODE_PARAMETER_FALSE_NORTHING);
            formatter->add(falseNorthing);
            formatter->endNode();
        } else if (starts_with(methodName, "PROJ ")) {
            bAlreadyWritten = true;
            formatter->startNode(io::WKTConstants::PROJECTION, false);
            formatter->addQuotedString("custom_proj4");
            formatter->endNode();
        }
    }

    if (!bAlreadyWritten) {
        l_method->_exportToWKT(formatter);

        const MethodMapping *mapping =
            !isWKT2 ? getMapping(l_method.get()) : nullptr;
        for (const auto &genOpParamvalue : parameterValues()) {

            // EPSG has normally no Latitude of natural origin for Equidistant
            // Cylindrical but PROJ can handle it, so output the parameter if
            // not zero
            if ((methodEPSGCode == EPSG_CODE_METHOD_EQUIDISTANT_CYLINDRICAL ||
                 methodEPSGCode ==
                     EPSG_CODE_METHOD_EQUIDISTANT_CYLINDRICAL_SPHERICAL)) {
                auto opParamvalue =
                    dynamic_cast<const OperationParameterValue *>(
                        genOpParamvalue.get());
                if (opParamvalue &&
                    opParamvalue->parameter()->getEPSGCode() ==
                        EPSG_CODE_PARAMETER_LATITUDE_OF_NATURAL_ORIGIN) {
                    const auto &paramValue = opParamvalue->parameterValue();
                    if (paramValue->type() == ParameterValue::Type::MEASURE) {
                        const auto &measure = paramValue->value();
                        if (measure.getSIValue() == 0) {
                            continue;
                        }
                    }
                }
            }
            // Same for false easting / false northing for Vertical Perspective
            else if (methodEPSGCode == EPSG_CODE_METHOD_VERTICAL_PERSPECTIVE) {
                auto opParamvalue =
                    dynamic_cast<const OperationParameterValue *>(
                        genOpParamvalue.get());
                if (opParamvalue) {
                    const auto paramEPSGCode =
                        opParamvalue->parameter()->getEPSGCode();
                    if (paramEPSGCode == EPSG_CODE_PARAMETER_FALSE_EASTING ||
                        paramEPSGCode == EPSG_CODE_PARAMETER_FALSE_NORTHING) {
                        const auto &paramValue = opParamvalue->parameterValue();
                        if (paramValue->type() ==
                            ParameterValue::Type::MEASURE) {
                            const auto &measure = paramValue->value();
                            if (measure.getSIValue() == 0) {
                                continue;
                            }
                        }
                    }
                }
            }
            genOpParamvalue->_exportToWKT(formatter, mapping);
        }
    }

    if (isWKT2) {
        if (formatter->outputId()) {
            formatID(formatter);
        }
        formatter->endNode();
    } else {
        formatter->popOutputUnit();
        formatter->popOutputId();
        formatter->leave();
    }
}
//! @endcond

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
void Conversion::_exportToJSON(
    io::JSONFormatter *formatter) const // throw(FormattingException)
{
    auto writer = formatter->writer();
    auto objectContext(
        formatter->MakeObjectContext("Conversion", !identifiers().empty()));

    writer->AddObjKey("name");
    auto l_name = nameStr();
    if (l_name.empty()) {
        writer->Add("unnamed");
    } else {
        writer->Add(l_name);
    }

    writer->AddObjKey("method");
    formatter->setOmitTypeInImmediateChild();
    formatter->setAllowIDInImmediateChild();
    method()->_exportToJSON(formatter);

    const auto &l_parameterValues = parameterValues();
    if (!l_parameterValues.empty()) {
        writer->AddObjKey("parameters");
        {
            auto parametersContext(writer->MakeArrayContext(false));
            for (const auto &genOpParamvalue : l_parameterValues) {
                formatter->setAllowIDInImmediateChild();
                formatter->setOmitTypeInImmediateChild();
                genOpParamvalue->_exportToJSON(formatter);
            }
        }
    }

    if (formatter->outputId()) {
        formatID(formatter);
    }
}

//! @endcond

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
static bool createPROJ4WebMercator(const Conversion *conv,
                                   io::PROJStringFormatter *formatter) {
    const double centralMeridian = conv->parameterValueNumeric(
        EPSG_CODE_PARAMETER_LONGITUDE_OF_NATURAL_ORIGIN,
        common::UnitOfMeasure::DEGREE);

    const double falseEasting =
        conv->parameterValueNumericAsSI(EPSG_CODE_PARAMETER_FALSE_EASTING);

    const double falseNorthing =
        conv->parameterValueNumericAsSI(EPSG_CODE_PARAMETER_FALSE_NORTHING);

    auto sourceCRS = conv->sourceCRS();
    auto geogCRS = dynamic_cast<const crs::GeographicCRS *>(sourceCRS.get());
    if (!geogCRS) {
        return false;
    }

    std::string units("m");
    auto targetCRS = conv->targetCRS();
    auto targetProjCRS =
        dynamic_cast<const crs::ProjectedCRS *>(targetCRS.get());
    if (targetProjCRS) {
        const auto &axisList = targetProjCRS->coordinateSystem()->axisList();
        const auto &unit = axisList[0]->unit();
        if (!unit._isEquivalentTo(common::UnitOfMeasure::METRE,
                                  util::IComparable::Criterion::EQUIVALENT)) {
            auto projUnit = unit.exportToPROJString();
            if (!projUnit.empty()) {
                units = projUnit;
            } else {
                return false;
            }
        }
    }

    formatter->addStep("merc");
    const double a = geogCRS->ellipsoid()->semiMajorAxis().getSIValue();
    formatter->addParam("a", a);
    formatter->addParam("b", a);
    formatter->addParam("lat_ts", 0.0);
    formatter->addParam("lon_0", centralMeridian);
    formatter->addParam("x_0", falseEasting);
    formatter->addParam("y_0", falseNorthing);
    formatter->addParam("k", 1.0);
    formatter->addParam("units", units);
    formatter->addParam("nadgrids", "@null");
    formatter->addParam("wktext");
    formatter->addParam("no_defs");
    return true;
}

// ---------------------------------------------------------------------------

static bool
createPROJExtensionFromCustomProj(const Conversion *conv,
                                  io::PROJStringFormatter *formatter,
                                  bool forExtensionNode) {
    const auto &methodName = conv->method()->nameStr();
    assert(starts_with(methodName, "PROJ "));
    auto tokens = split(methodName, ' ');

    formatter->addStep(tokens[1]);

    if (forExtensionNode) {
        auto sourceCRS = conv->sourceCRS();
        auto geogCRS =
            dynamic_cast<const crs::GeographicCRS *>(sourceCRS.get());
        if (!geogCRS) {
            return false;
        }
        geogCRS->addDatumInfoToPROJString(formatter);
    }

    for (size_t i = 2; i < tokens.size(); i++) {
        auto kv = split(tokens[i], '=');
        if (kv.size() == 2) {
            formatter->addParam(kv[0], kv[1]);
        } else {
            formatter->addParam(tokens[i]);
        }
    }

    for (const auto &genOpParamvalue : conv->parameterValues()) {
        auto opParamvalue = dynamic_cast<const OperationParameterValue *>(
            genOpParamvalue.get());
        if (opParamvalue) {
            const auto &paramName = opParamvalue->parameter()->nameStr();
            const auto &paramValue = opParamvalue->parameterValue();
            if (paramValue->type() == ParameterValue::Type::MEASURE) {
                const auto &measure = paramValue->value();
                const auto unitType = measure.unit().type();
                if (unitType == common::UnitOfMeasure::Type::LINEAR) {
                    formatter->addParam(paramName, measure.getSIValue());
                } else if (unitType == common::UnitOfMeasure::Type::ANGULAR) {
                    formatter->addParam(
                        paramName,
                        measure.convertToUnit(common::UnitOfMeasure::DEGREE));
                } else {
                    formatter->addParam(paramName, measure.value());
                }
            }
        }
    }

    if (forExtensionNode) {
        formatter->addParam("wktext");
        formatter->addParam("no_defs");
    }
    return true;
}
//! @endcond

// ---------------------------------------------------------------------------

bool Conversion::addWKTExtensionNode(io::WKTFormatter *formatter) const {
    const bool isWKT2 = formatter->version() == io::WKTFormatter::Version::WKT2;
    if (!isWKT2) {
        const auto &l_method = method();
        const auto &methodName = l_method->nameStr();
        const int methodEPSGCode = l_method->getEPSGCode();
        if (l_method->getPrivate()->projMethodOverride_ == "tmerc approx" ||
            l_method->getPrivate()->projMethodOverride_ == "utm approx") {
            auto projFormatter = io::PROJStringFormatter::create();
            projFormatter->setCRSExport(true);
            projFormatter->setUseApproxTMerc(true);
            formatter->startNode(io::WKTConstants::EXTENSION, false);
            formatter->addQuotedString("PROJ4");
            _exportToPROJString(projFormatter.get());
            projFormatter->addParam("no_defs");
            formatter->addQuotedString(projFormatter->toString());
            formatter->endNode();
            return true;
        } else if (methodEPSGCode ==
                       EPSG_CODE_METHOD_POPULAR_VISUALISATION_PSEUDO_MERCATOR ||
                   nameStr() == "Popular Visualisation Mercator") {

            auto projFormatter = io::PROJStringFormatter::create();
            projFormatter->setCRSExport(true);
            if (createPROJ4WebMercator(this, projFormatter.get())) {
                formatter->startNode(io::WKTConstants::EXTENSION, false);
                formatter->addQuotedString("PROJ4");
                formatter->addQuotedString(projFormatter->toString());
                formatter->endNode();
                return true;
            }
        } else if (starts_with(methodName, "PROJ ")) {
            auto projFormatter = io::PROJStringFormatter::create();
            projFormatter->setCRSExport(true);
            if (createPROJExtensionFromCustomProj(this, projFormatter.get(),
                                                  true)) {
                formatter->startNode(io::WKTConstants::EXTENSION, false);
                formatter->addQuotedString("PROJ4");
                formatter->addQuotedString(projFormatter->toString());
                formatter->endNode();
                return true;
            }
        } else if (methodName ==
                   PROJ_WKT2_NAME_METHOD_GEOSTATIONARY_SATELLITE_SWEEP_X) {
            auto projFormatter = io::PROJStringFormatter::create();
            projFormatter->setCRSExport(true);
            formatter->startNode(io::WKTConstants::EXTENSION, false);
            formatter->addQuotedString("PROJ4");
            _exportToPROJString(projFormatter.get());
            projFormatter->addParam("no_defs");
            formatter->addQuotedString(projFormatter->toString());
            formatter->endNode();
            return true;
        }
    }
    return false;
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
void Conversion::_exportToPROJString(
    io::PROJStringFormatter *formatter) const // throw(FormattingException)
{
    const auto &l_method = method();
    const auto &methodName = l_method->nameStr();
    const int methodEPSGCode = l_method->getEPSGCode();
    const bool isZUnitConversion =
        methodEPSGCode == EPSG_CODE_METHOD_CHANGE_VERTICAL_UNIT;
    const bool isAffineParametric =
        methodEPSGCode == EPSG_CODE_METHOD_AFFINE_PARAMETRIC_TRANSFORMATION;
    const bool isGeographicGeocentric =
        methodEPSGCode == EPSG_CODE_METHOD_GEOGRAPHIC_GEOCENTRIC;
    const bool isHeightDepthReversal =
        methodEPSGCode == EPSG_CODE_METHOD_HEIGHT_DEPTH_REVERSAL;
    const bool applySourceCRSModifiers =
        !isZUnitConversion && !isAffineParametric &&
        !isAxisOrderReversal(methodEPSGCode) && !isGeographicGeocentric &&
        !isHeightDepthReversal;
    bool applyTargetCRSModifiers = applySourceCRSModifiers;

    auto l_sourceCRS = sourceCRS();
    if (!formatter->getCRSExport() && l_sourceCRS && applySourceCRSModifiers) {

        crs::CRS *horiz = l_sourceCRS.get();
        const auto compound = dynamic_cast<const crs::CompoundCRS *>(horiz);
        if (compound) {
            const auto &components = compound->componentReferenceSystems();
            if (!components.empty()) {
                horiz = components.front().get();
            }
        }

        auto geogCRS = dynamic_cast<const crs::GeographicCRS *>(horiz);
        if (geogCRS) {
            formatter->setOmitProjLongLatIfPossible(true);
            formatter->startInversion();
            geogCRS->_exportToPROJString(formatter);
            formatter->stopInversion();
            formatter->setOmitProjLongLatIfPossible(false);
        }

        auto projCRS = dynamic_cast<const crs::ProjectedCRS *>(horiz);
        if (projCRS) {
            formatter->startInversion();
            formatter->pushOmitZUnitConversion();
            projCRS->addUnitConvertAndAxisSwap(formatter, false);
            formatter->popOmitZUnitConversion();
            formatter->stopInversion();
        }
    }

    const auto &convName = nameStr();
    bool bConversionDone = false;
    bool bEllipsoidParametersDone = false;
    bool useApprox = false;
    if (methodEPSGCode == EPSG_CODE_METHOD_TRANSVERSE_MERCATOR) {
        // Check for UTM
        int zone = 0;
        bool north = true;
        useApprox =
            formatter->getUseApproxTMerc() ||
            l_method->getPrivate()->projMethodOverride_ == "tmerc approx" ||
            l_method->getPrivate()->projMethodOverride_ == "utm approx";
        if (isUTM(zone, north)) {
            bConversionDone = true;
            formatter->addStep("utm");
            if (useApprox) {
                formatter->addParam("approx");
            }
            formatter->addParam("zone", zone);
            if (!north) {
                formatter->addParam("south");
            }
        }
    } else if (methodEPSGCode ==
               EPSG_CODE_METHOD_HOTINE_OBLIQUE_MERCATOR_VARIANT_A) {
        const double azimuth =
            parameterValueNumeric(EPSG_CODE_PARAMETER_AZIMUTH_INITIAL_LINE,
                                  common::UnitOfMeasure::DEGREE);
        const double angleRectifiedToSkewGrid = parameterValueNumeric(
            EPSG_CODE_PARAMETER_ANGLE_RECTIFIED_TO_SKEW_GRID,
            common::UnitOfMeasure::DEGREE);
        // Map to Swiss Oblique Mercator / somerc
        if (std::fabs(azimuth - 90) < 1e-4 &&
            std::fabs(angleRectifiedToSkewGrid - 90) < 1e-4) {
            bConversionDone = true;
            formatter->addStep("somerc");
            formatter->addParam(
                "lat_0", parameterValueNumeric(
                             EPSG_CODE_PARAMETER_LATITUDE_PROJECTION_CENTRE,
                             common::UnitOfMeasure::DEGREE));
            formatter->addParam(
                "lon_0", parameterValueNumeric(
                             EPSG_CODE_PARAMETER_LONGITUDE_PROJECTION_CENTRE,
                             common::UnitOfMeasure::DEGREE));
            formatter->addParam(
                "k_0", parameterValueNumericAsSI(
                           EPSG_CODE_PARAMETER_SCALE_FACTOR_INITIAL_LINE));
            formatter->addParam("x_0", parameterValueNumericAsSI(
                                           EPSG_CODE_PARAMETER_FALSE_EASTING));
            formatter->addParam("y_0", parameterValueNumericAsSI(
                                           EPSG_CODE_PARAMETER_FALSE_NORTHING));
        }
    } else if (methodEPSGCode ==
               EPSG_CODE_METHOD_HOTINE_OBLIQUE_MERCATOR_VARIANT_B) {
        const double azimuth =
            parameterValueNumeric(EPSG_CODE_PARAMETER_AZIMUTH_INITIAL_LINE,
                                  common::UnitOfMeasure::DEGREE);
        const double angleRectifiedToSkewGrid = parameterValueNumeric(
            EPSG_CODE_PARAMETER_ANGLE_RECTIFIED_TO_SKEW_GRID,
            common::UnitOfMeasure::DEGREE);
        // Map to Swiss Oblique Mercator / somerc
        if (std::fabs(azimuth - 90) < 1e-4 &&
            std::fabs(angleRectifiedToSkewGrid - 90) < 1e-4) {
            bConversionDone = true;
            formatter->addStep("somerc");
            formatter->addParam(
                "lat_0", parameterValueNumeric(
                             EPSG_CODE_PARAMETER_LATITUDE_PROJECTION_CENTRE,
                             common::UnitOfMeasure::DEGREE));
            formatter->addParam(
                "lon_0", parameterValueNumeric(
                             EPSG_CODE_PARAMETER_LONGITUDE_PROJECTION_CENTRE,
                             common::UnitOfMeasure::DEGREE));
            formatter->addParam(
                "k_0", parameterValueNumericAsSI(
                           EPSG_CODE_PARAMETER_SCALE_FACTOR_INITIAL_LINE));
            formatter->addParam(
                "x_0", parameterValueNumericAsSI(
                           EPSG_CODE_PARAMETER_EASTING_PROJECTION_CENTRE));
            formatter->addParam(
                "y_0", parameterValueNumericAsSI(
                           EPSG_CODE_PARAMETER_NORTHING_PROJECTION_CENTRE));
        }
    } else if (methodEPSGCode == EPSG_CODE_METHOD_KROVAK_NORTH_ORIENTED) {
        double colatitude =
            parameterValueNumeric(EPSG_CODE_PARAMETER_COLATITUDE_CONE_AXIS,
                                  common::UnitOfMeasure::DEGREE);
        double latitudePseudoStandardParallel = parameterValueNumeric(
            EPSG_CODE_PARAMETER_LATITUDE_PSEUDO_STANDARD_PARALLEL,
            common::UnitOfMeasure::DEGREE);
        // 30deg 17' 17.30311'' = 30.28813975277777776
        // 30deg 17' 17.303''   = 30.288139722222223 as used in GDAL WKT1
        if (std::fabs(colatitude - 30.2881397) > 1e-7) {
            throw io::FormattingException(
                std::string("Unsupported value for ") +
                EPSG_NAME_PARAMETER_COLATITUDE_CONE_AXIS);
        }
        if (std::fabs(latitudePseudoStandardParallel - 78.5) > 1e-8) {
            throw io::FormattingException(
                std::string("Unsupported value for ") +
                EPSG_NAME_PARAMETER_LATITUDE_PSEUDO_STANDARD_PARALLEL);
        }
    } else if (methodEPSGCode == EPSG_CODE_METHOD_MERCATOR_VARIANT_A) {
        double latitudeOrigin = parameterValueNumeric(
            EPSG_CODE_PARAMETER_LATITUDE_OF_NATURAL_ORIGIN,
            common::UnitOfMeasure::DEGREE);
        if (latitudeOrigin != 0) {
            throw io::FormattingException(
                std::string("Unsupported value for ") +
                EPSG_NAME_PARAMETER_LATITUDE_OF_NATURAL_ORIGIN);
        }
    } else if (methodEPSGCode == EPSG_CODE_METHOD_MERCATOR_VARIANT_B) {
        const auto &scaleFactor = parameterValueMeasure(WKT1_SCALE_FACTOR, 0);
        if (scaleFactor.unit().type() != common::UnitOfMeasure::Type::UNKNOWN &&
            std::fabs(scaleFactor.getSIValue() - 1.0) > 1e-10) {
            throw io::FormattingException(
                "Unexpected presence of scale factor in Mercator (variant B)");
        }
        double latitudeOrigin = parameterValueNumeric(
            EPSG_CODE_PARAMETER_LATITUDE_OF_NATURAL_ORIGIN,
            common::UnitOfMeasure::DEGREE);
        if (latitudeOrigin != 0) {
            throw io::FormattingException(
                std::string("Unsupported value for ") +
                EPSG_NAME_PARAMETER_LATITUDE_OF_NATURAL_ORIGIN);
        }
    } else if (methodEPSGCode ==
               EPSG_CODE_METHOD_TRANSVERSE_MERCATOR_SOUTH_ORIENTATED) {
        // We map TMSO to tmerc with axis=wsu. This only works if false easting
        // and northings are zero, which is the case in practice for South
        // African and Namibian EPSG CRS
        const auto falseEasting = parameterValueNumeric(
            EPSG_CODE_PARAMETER_FALSE_EASTING, common::UnitOfMeasure::METRE);
        if (falseEasting != 0) {
            throw io::FormattingException(
                std::string("Unsupported value for ") +
                EPSG_NAME_PARAMETER_FALSE_EASTING);
        }
        const auto falseNorthing = parameterValueNumeric(
            EPSG_CODE_PARAMETER_FALSE_NORTHING, common::UnitOfMeasure::METRE);
        if (falseNorthing != 0) {
            throw io::FormattingException(
                std::string("Unsupported value for ") +
                EPSG_NAME_PARAMETER_FALSE_NORTHING);
        }
        // PROJ.4 specific hack for webmercator
    } else if (formatter->getCRSExport() &&
               methodEPSGCode ==
                   EPSG_CODE_METHOD_POPULAR_VISUALISATION_PSEUDO_MERCATOR) {
        if (!createPROJ4WebMercator(this, formatter)) {
            throw io::FormattingException(
                std::string("Cannot export ") +
                EPSG_NAME_METHOD_POPULAR_VISUALISATION_PSEUDO_MERCATOR +
                " as PROJ.4 string outside of a ProjectedCRS context");
        }
        bConversionDone = true;
        bEllipsoidParametersDone = true;
        applyTargetCRSModifiers = false;
    } else if (ci_equal(convName, "Popular Visualisation Mercator")) {
        if (formatter->getCRSExport()) {
            if (!createPROJ4WebMercator(this, formatter)) {
                throw io::FormattingException(concat(
                    "Cannot export ", convName,
                    " as PROJ.4 string outside of a ProjectedCRS context"));
            }
            applyTargetCRSModifiers = false;
        } else {
            formatter->addStep("webmerc");
            if (l_sourceCRS) {
                datum::Ellipsoid::WGS84->_exportToPROJString(formatter);
            }
        }
        bConversionDone = true;
        bEllipsoidParametersDone = true;
    } else if (starts_with(methodName, "PROJ ")) {
        bConversionDone = true;
        createPROJExtensionFromCustomProj(this, formatter, false);
    } else if (ci_equal(methodName,
                        PROJ_WKT2_NAME_METHOD_POLE_ROTATION_GRIB_CONVENTION)) {
        double southPoleLat = parameterValueNumeric(
            PROJ_WKT2_NAME_PARAMETER_SOUTH_POLE_LATITUDE_GRIB_CONVENTION,
            common::UnitOfMeasure::DEGREE);
        double southPoleLon = parameterValueNumeric(
            PROJ_WKT2_NAME_PARAMETER_SOUTH_POLE_LONGITUDE_GRIB_CONVENTION,
            common::UnitOfMeasure::DEGREE);
        double rotation = parameterValueNumeric(
            PROJ_WKT2_NAME_PARAMETER_AXIS_ROTATION_GRIB_CONVENTION,
            common::UnitOfMeasure::DEGREE);
        formatter->addStep("ob_tran");
        formatter->addParam("o_proj", "longlat");
        formatter->addParam("o_lon_p", -rotation);
        formatter->addParam("o_lat_p", -southPoleLat);
        formatter->addParam("lon_0", southPoleLon);
        bConversionDone = true;
    } else if (ci_equal(methodName, "Adams_Square_II")) {
        // Look for ESRI method and parameter names (to be opposed
        // to the OGC WKT2 names we use elsewhere, because there's no mapping
        // of those parameters to OGC WKT2)
        // We also reject non-default values for a number of parameters,
        // because they are not implemented on PROJ side. The subset we
        // support can handle ESRI:54098 WGS_1984_Adams_Square_II, but not
        // ESRI:54099 WGS_1984_Spilhaus_Ocean_Map_in_Square
        const double falseEasting = parameterValueNumeric(
            "False_Easting", common::UnitOfMeasure::METRE);
        const double falseNorthing = parameterValueNumeric(
            "False_Northing", common::UnitOfMeasure::METRE);
        const double scaleFactor =
            parameterValue("Scale_Factor", 0)
                ? parameterValueNumeric("Scale_Factor",
                                        common::UnitOfMeasure::SCALE_UNITY)
                : 1.0;
        const double azimuth =
            parameterValueNumeric("Azimuth", common::UnitOfMeasure::DEGREE);
        const double longitudeOfCenter = parameterValueNumeric(
            "Longitude_Of_Center", common::UnitOfMeasure::DEGREE);
        const double latitudeOfCenter = parameterValueNumeric(
            "Latitude_Of_Center", common::UnitOfMeasure::DEGREE);
        const double XYPlaneRotation = parameterValueNumeric(
            "XY_Plane_Rotation", common::UnitOfMeasure::DEGREE);
        if (scaleFactor != 1.0 || azimuth != 0.0 || latitudeOfCenter != 0.0 ||
            XYPlaneRotation != 0.0) {
            throw io::FormattingException("Unsupported value for one or "
                                          "several parameters of "
                                          "Adams_Square_II");
        }
        formatter->addStep("adams_ws2");
        formatter->addParam("lon_0", longitudeOfCenter);
        formatter->addParam("x_0", falseEasting);
        formatter->addParam("y_0", falseNorthing);
        bConversionDone = true;
    } else if (formatter->convention() ==
                   io::PROJStringFormatter::Convention::PROJ_5 &&
               isZUnitConversion) {
        double convFactor = parameterValueNumericAsSI(
            EPSG_CODE_PARAMETER_UNIT_CONVERSION_SCALAR);
        auto uom = common::UnitOfMeasure(std::string(), convFactor,
                                         common::UnitOfMeasure::Type::LINEAR)
                       .exportToPROJString();
        auto reverse_uom =
            convFactor == 0.0
                ? std::string()
                : common::UnitOfMeasure(std::string(), 1.0 / convFactor,
                                        common::UnitOfMeasure::Type::LINEAR)
                      .exportToPROJString();
        if (uom == "m") {
            // do nothing
        } else if (!uom.empty()) {
            formatter->addStep("unitconvert");
            formatter->addParam("z_in", uom);
            formatter->addParam("z_out", "m");
        } else if (!reverse_uom.empty()) {
            formatter->addStep("unitconvert");
            formatter->addParam("z_in", "m");
            formatter->addParam("z_out", reverse_uom);
        } else {
            formatter->addStep("affine");
            formatter->addParam("s33", convFactor);
        }
        bConversionDone = true;
        bEllipsoidParametersDone = true;
    }

    auto l_targetCRS = targetCRS();

    bool bAxisSpecFound = false;
    if (!bConversionDone) {
        const MethodMapping *mapping = getMapping(l_method.get());
        if (mapping && mapping->proj_name_main) {
            formatter->addStep(mapping->proj_name_main);
            if (useApprox) {
                formatter->addParam("approx");
            }
            if (mapping->proj_name_aux) {
                bool addAux = true;
                if (internal::starts_with(mapping->proj_name_aux, "axis=")) {
                    if (mapping->epsg_code == EPSG_CODE_METHOD_KROVAK) {
                        auto projCRS = dynamic_cast<const crs::ProjectedCRS *>(
                            l_targetCRS.get());
                        if (projCRS) {
                            const auto &axisList =
                                projCRS->coordinateSystem()->axisList();
                            if (axisList[0]->direction() ==
                                    cs::AxisDirection::WEST &&
                                axisList[1]->direction() ==
                                    cs::AxisDirection::SOUTH) {
                                formatter->addParam("czech");
                                addAux = false;
                            }
                        }
                    }
                    bAxisSpecFound = true;
                }

                // No need to add explicit f=0 if the ellipsoid is a sphere
                if (strcmp(mapping->proj_name_aux, "f=0") == 0) {
                    crs::CRS *horiz = l_sourceCRS.get();
                    const auto compound =
                        dynamic_cast<const crs::CompoundCRS *>(horiz);
                    if (compound) {
                        const auto &components =
                            compound->componentReferenceSystems();
                        if (!components.empty()) {
                            horiz = components.front().get();
                        }
                    }

                    auto geogCRS =
                        dynamic_cast<const crs::GeographicCRS *>(horiz);
                    if (geogCRS && geogCRS->ellipsoid()->isSphere()) {
                        addAux = false;
                    }
                }

                if (addAux) {
                    auto kv = split(mapping->proj_name_aux, '=');
                    if (kv.size() == 2) {
                        formatter->addParam(kv[0], kv[1]);
                    } else {
                        formatter->addParam(mapping->proj_name_aux);
                    }
                }
            }

            if (mapping->epsg_code ==
                EPSG_CODE_METHOD_POLAR_STEREOGRAPHIC_VARIANT_B) {
                double latitudeStdParallel = parameterValueNumeric(
                    EPSG_CODE_PARAMETER_LATITUDE_STD_PARALLEL,
                    common::UnitOfMeasure::DEGREE);
                formatter->addParam("lat_0",
                                    (latitudeStdParallel >= 0) ? 90.0 : -90.0);
            }

            for (int i = 0; mapping->params[i] != nullptr; i++) {
                const auto *param = mapping->params[i];
                if (!param->proj_name) {
                    continue;
                }
                const auto &value =
                    parameterValueMeasure(param->wkt2_name, param->epsg_code);
                double valueConverted = 0;
                if (value == nullMeasure) {
                    // Deal with missing values. In an ideal world, this would
                    // not happen
                    if (param->epsg_code ==
                        EPSG_CODE_PARAMETER_SCALE_FACTOR_AT_NATURAL_ORIGIN) {
                        valueConverted = 1.0;
                    }
                } else if (param->unit_type ==
                           common::UnitOfMeasure::Type::ANGULAR) {
                    valueConverted =
                        value.convertToUnit(common::UnitOfMeasure::DEGREE);
                } else {
                    valueConverted = value.getSIValue();
                }

                if (mapping->epsg_code ==
                        EPSG_CODE_METHOD_LAMBERT_CONIC_CONFORMAL_1SP &&
                    strcmp(param->proj_name, "lat_1") == 0) {
                    formatter->addParam(param->proj_name, valueConverted);
                    formatter->addParam("lat_0", valueConverted);
                } else {
                    formatter->addParam(param->proj_name, valueConverted);
                }
            }

        } else {
            if (!exportToPROJStringGeneric(formatter)) {
                throw io::FormattingException(
                    concat("Unsupported conversion method: ", methodName));
            }
        }
    }

    if (l_targetCRS && applyTargetCRSModifiers) {
        crs::CRS *horiz = l_targetCRS.get();
        const auto compound = dynamic_cast<const crs::CompoundCRS *>(horiz);
        if (compound) {
            const auto &components = compound->componentReferenceSystems();
            if (!components.empty()) {
                horiz = components.front().get();
            }
        }

        if (!bEllipsoidParametersDone) {
            auto targetGeogCRS = horiz->extractGeographicCRS();
            if (targetGeogCRS) {
                if (formatter->getCRSExport()) {
                    targetGeogCRS->addDatumInfoToPROJString(formatter);
                } else {
                    targetGeogCRS->ellipsoid()->_exportToPROJString(formatter);
                    targetGeogCRS->primeMeridian()->_exportToPROJString(
                        formatter);
                }
            }
        }

        auto projCRS = dynamic_cast<const crs::ProjectedCRS *>(horiz);
        if (projCRS) {
            formatter->pushOmitZUnitConversion();
            projCRS->addUnitConvertAndAxisSwap(formatter, bAxisSpecFound);
            formatter->popOmitZUnitConversion();
        }

        auto derivedGeographicCRS =
            dynamic_cast<const crs::DerivedGeographicCRS *>(horiz);
        if (!formatter->getCRSExport() && derivedGeographicCRS) {
            formatter->setOmitProjLongLatIfPossible(true);
            derivedGeographicCRS->addAngularUnitConvertAndAxisSwap(formatter);
            formatter->setOmitProjLongLatIfPossible(false);
        }
    }
}
//! @endcond

// ---------------------------------------------------------------------------

/** \brief Return whether a conversion is a [Universal Transverse Mercator]
 * (https://proj.org/operations/projections/utm.html) conversion.
 *
 * @param[out] zone UTM zone number between 1 and 60.
 * @param[out] north true for UTM northern hemisphere, false for UTM southern
 * hemisphere.
 * @return true if it is a UTM conversion.
 */
bool Conversion::isUTM(int &zone, bool &north) const {
    zone = 0;
    north = true;

    if (method()->getEPSGCode() == EPSG_CODE_METHOD_TRANSVERSE_MERCATOR) {
        // Check for UTM

        bool bLatitudeNatOriginUTM = false;
        bool bScaleFactorUTM = false;
        bool bFalseEastingUTM = false;
        bool bFalseNorthingUTM = false;
        for (const auto &genOpParamvalue : parameterValues()) {
            auto opParamvalue = dynamic_cast<const OperationParameterValue *>(
                genOpParamvalue.get());
            if (opParamvalue) {
                const auto epsg_code = opParamvalue->parameter()->getEPSGCode();
                const auto &l_parameterValue = opParamvalue->parameterValue();
                if (l_parameterValue->type() == ParameterValue::Type::MEASURE) {
                    const auto &measure = l_parameterValue->value();
                    if (epsg_code ==
                            EPSG_CODE_PARAMETER_LATITUDE_OF_NATURAL_ORIGIN &&
                        std::fabs(measure.value() -
                                  UTM_LATITUDE_OF_NATURAL_ORIGIN) < 1e-10) {
                        bLatitudeNatOriginUTM = true;
                    } else if (
                        (epsg_code ==
                             EPSG_CODE_PARAMETER_LONGITUDE_OF_NATURAL_ORIGIN ||
                         epsg_code ==
                             EPSG_CODE_PARAMETER_LONGITUDE_OF_ORIGIN) &&
                        measure.unit()._isEquivalentTo(
                            common::UnitOfMeasure::DEGREE,
                            util::IComparable::Criterion::EQUIVALENT)) {
                        double dfZone = (measure.value() + 183.0) / 6.0;
                        if (dfZone > 0.9 && dfZone < 60.1 &&
                            std::abs(dfZone - std::round(dfZone)) < 1e-10) {
                            zone = static_cast<int>(std::lround(dfZone));
                        }
                    } else if (
                        epsg_code ==
                            EPSG_CODE_PARAMETER_SCALE_FACTOR_AT_NATURAL_ORIGIN &&
                        measure.unit()._isEquivalentTo(
                            common::UnitOfMeasure::SCALE_UNITY,
                            util::IComparable::Criterion::EQUIVALENT) &&
                        std::fabs(measure.value() - UTM_SCALE_FACTOR) < 1e-10) {
                        bScaleFactorUTM = true;
                    } else if (epsg_code == EPSG_CODE_PARAMETER_FALSE_EASTING &&
                               measure.value() == UTM_FALSE_EASTING &&
                               measure.unit()._isEquivalentTo(
                                   common::UnitOfMeasure::METRE,
                                   util::IComparable::Criterion::EQUIVALENT)) {
                        bFalseEastingUTM = true;
                    } else if (epsg_code ==
                                   EPSG_CODE_PARAMETER_FALSE_NORTHING &&
                               measure.unit()._isEquivalentTo(
                                   common::UnitOfMeasure::METRE,
                                   util::IComparable::Criterion::EQUIVALENT)) {
                        if (std::fabs(measure.value() -
                                      UTM_NORTH_FALSE_NORTHING) < 1e-10) {
                            bFalseNorthingUTM = true;
                            north = true;
                        } else if (std::fabs(measure.value() -
                                             UTM_SOUTH_FALSE_NORTHING) <
                                   1e-10) {
                            bFalseNorthingUTM = true;
                            north = false;
                        }
                    }
                }
            }
        }
        if (bLatitudeNatOriginUTM && zone > 0 && bScaleFactorUTM &&
            bFalseEastingUTM && bFalseNorthingUTM) {
            return true;
        }
    }
    return false;
}

// ---------------------------------------------------------------------------

/** \brief Return a Conversion object where some parameters are better
 * identified.
 *
 * @return a new Conversion.
 */
ConversionNNPtr Conversion::identify() const {
    auto newConversion = Conversion::nn_make_shared<Conversion>(*this);
    newConversion->assignSelf(newConversion);

    if (method()->getEPSGCode() == EPSG_CODE_METHOD_TRANSVERSE_MERCATOR) {
        // Check for UTM
        int zone = 0;
        bool north = true;
        if (isUTM(zone, north)) {
            newConversion->setProperties(
                getUTMConversionProperty(util::PropertyMap(), zone, north));
        }
    }

    return newConversion;
}

//! @cond Doxygen_Suppress
// ---------------------------------------------------------------------------

InvalidOperation::InvalidOperation(const char *message) : Exception(message) {}

// ---------------------------------------------------------------------------

InvalidOperation::InvalidOperation(const std::string &message)
    : Exception(message) {}

// ---------------------------------------------------------------------------

InvalidOperation::InvalidOperation(const InvalidOperation &) = default;

// ---------------------------------------------------------------------------

InvalidOperation::~InvalidOperation() = default;
//! @endcond

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
struct Transformation::Private {

    TransformationPtr forwardOperation_{};

    static TransformationNNPtr registerInv(const Transformation *thisIn,
                                           TransformationNNPtr invTransform);
};
//! @endcond

// ---------------------------------------------------------------------------

Transformation::Transformation(
    const crs::CRSNNPtr &sourceCRSIn, const crs::CRSNNPtr &targetCRSIn,
    const crs::CRSPtr &interpolationCRSIn, const OperationMethodNNPtr &methodIn,
    const std::vector<GeneralParameterValueNNPtr> &values,
    const std::vector<metadata::PositionalAccuracyNNPtr> &accuracies)
    : SingleOperation(methodIn), d(internal::make_unique<Private>()) {
    setParameterValues(values);
    setCRSs(sourceCRSIn, targetCRSIn, interpolationCRSIn);
    setAccuracies(accuracies);
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
Transformation::~Transformation() = default;
//! @endcond

// ---------------------------------------------------------------------------

Transformation::Transformation(const Transformation &other)
    : CoordinateOperation(other), SingleOperation(other),
      d(internal::make_unique<Private>(*other.d)) {}

// ---------------------------------------------------------------------------

/** \brief Return the source crs::CRS of the transformation.
 *
 * @return the source CRS.
 */
const crs::CRSNNPtr &Transformation::sourceCRS() PROJ_PURE_DEFN {
    return CoordinateOperation::getPrivate()->strongRef_->sourceCRS_;
}

// ---------------------------------------------------------------------------

/** \brief Return the target crs::CRS of the transformation.
 *
 * @return the target CRS.
 */
const crs::CRSNNPtr &Transformation::targetCRS() PROJ_PURE_DEFN {
    return CoordinateOperation::getPrivate()->strongRef_->targetCRS_;
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
TransformationNNPtr Transformation::shallowClone() const {
    auto transf = Transformation::nn_make_shared<Transformation>(*this);
    transf->assignSelf(transf);
    transf->setCRSs(this, false);
    if (transf->d->forwardOperation_) {
        transf->d->forwardOperation_ =
            transf->d->forwardOperation_->shallowClone().as_nullable();
    }
    return transf;
}

CoordinateOperationNNPtr Transformation::_shallowClone() const {
    return util::nn_static_pointer_cast<CoordinateOperation>(shallowClone());
}

// ---------------------------------------------------------------------------

TransformationNNPtr
Transformation::promoteTo3D(const std::string &,
                            const io::DatabaseContextPtr &dbContext) const {
    auto transf = shallowClone();
    transf->setCRSs(sourceCRS()->promoteTo3D(std::string(), dbContext),
                    targetCRS()->promoteTo3D(std::string(), dbContext),
                    interpolationCRS());
    return transf;
}

// ---------------------------------------------------------------------------

TransformationNNPtr
Transformation::demoteTo2D(const std::string &,
                           const io::DatabaseContextPtr &dbContext) const {
    auto transf = shallowClone();
    transf->setCRSs(sourceCRS()->demoteTo2D(std::string(), dbContext),
                    targetCRS()->demoteTo2D(std::string(), dbContext),
                    interpolationCRS());
    return transf;
}

//! @endcond

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
/** \brief Return the TOWGS84 parameters of the transformation.
 *
 * If this transformation uses Coordinate Frame Rotation, Position Vector
 * transformation or Geocentric translations, a vector of 7 double values
 * using the Position Vector convention (EPSG:9606) is returned. Those values
 * can be used as the value of the WKT1 TOWGS84 parameter or
 * PROJ +towgs84 parameter.
 *
 * @return a vector of 7 values if valid, otherwise a io::FormattingException
 * is thrown.
 * @throws io::FormattingException
 */
std::vector<double>
Transformation::getTOWGS84Parameters() const // throw(io::FormattingException)
{
    // GDAL WKT1 assumes EPSG:9606 / Position Vector convention

    bool sevenParamsTransform = false;
    bool threeParamsTransform = false;
    bool invertRotSigns = false;
    const auto &l_method = method();
    const auto &methodName = l_method->nameStr();
    const int methodEPSGCode = l_method->getEPSGCode();
    const auto paramCount = parameterValues().size();
    if ((paramCount == 7 &&
         ci_find(methodName, "Coordinate Frame") != std::string::npos) ||
        methodEPSGCode == EPSG_CODE_METHOD_COORDINATE_FRAME_GEOCENTRIC ||
        methodEPSGCode == EPSG_CODE_METHOD_COORDINATE_FRAME_GEOGRAPHIC_2D ||
        methodEPSGCode == EPSG_CODE_METHOD_COORDINATE_FRAME_GEOGRAPHIC_3D) {
        sevenParamsTransform = true;
        invertRotSigns = true;
    } else if ((paramCount == 7 &&
                ci_find(methodName, "Position Vector") != std::string::npos) ||
               methodEPSGCode == EPSG_CODE_METHOD_POSITION_VECTOR_GEOCENTRIC ||
               methodEPSGCode ==
                   EPSG_CODE_METHOD_POSITION_VECTOR_GEOGRAPHIC_2D ||
               methodEPSGCode ==
                   EPSG_CODE_METHOD_POSITION_VECTOR_GEOGRAPHIC_3D) {
        sevenParamsTransform = true;
        invertRotSigns = false;
    } else if ((paramCount == 3 &&
                ci_find(methodName, "Geocentric translations") !=
                    std::string::npos) ||
               methodEPSGCode ==
                   EPSG_CODE_METHOD_GEOCENTRIC_TRANSLATION_GEOCENTRIC ||
               methodEPSGCode ==
                   EPSG_CODE_METHOD_GEOCENTRIC_TRANSLATION_GEOGRAPHIC_2D ||
               methodEPSGCode ==
                   EPSG_CODE_METHOD_GEOCENTRIC_TRANSLATION_GEOGRAPHIC_3D) {
        threeParamsTransform = true;
    }

    if (threeParamsTransform || sevenParamsTransform) {
        std::vector<double> params(7, 0.0);
        bool foundX = false;
        bool foundY = false;
        bool foundZ = false;
        bool foundRotX = false;
        bool foundRotY = false;
        bool foundRotZ = false;
        bool foundScale = false;
        const double rotSign = invertRotSigns ? -1.0 : 1.0;

        const auto fixNegativeZero = [](double x) {
            if (x == 0.0)
                return 0.0;
            return x;
        };

        for (const auto &genOpParamvalue : parameterValues()) {
            auto opParamvalue = dynamic_cast<const OperationParameterValue *>(
                genOpParamvalue.get());
            if (opParamvalue) {
                const auto &parameter = opParamvalue->parameter();
                const auto epsg_code = parameter->getEPSGCode();
                const auto &l_parameterValue = opParamvalue->parameterValue();
                if (l_parameterValue->type() == ParameterValue::Type::MEASURE) {
                    const auto &measure = l_parameterValue->value();
                    if (epsg_code == EPSG_CODE_PARAMETER_X_AXIS_TRANSLATION) {
                        params[0] = measure.getSIValue();
                        foundX = true;
                    } else if (epsg_code ==
                               EPSG_CODE_PARAMETER_Y_AXIS_TRANSLATION) {
                        params[1] = measure.getSIValue();
                        foundY = true;
                    } else if (epsg_code ==
                               EPSG_CODE_PARAMETER_Z_AXIS_TRANSLATION) {
                        params[2] = measure.getSIValue();
                        foundZ = true;
                    } else if (epsg_code ==
                               EPSG_CODE_PARAMETER_X_AXIS_ROTATION) {
                        params[3] = fixNegativeZero(
                            rotSign *
                            measure.convertToUnit(
                                common::UnitOfMeasure::ARC_SECOND));
                        foundRotX = true;
                    } else if (epsg_code ==
                               EPSG_CODE_PARAMETER_Y_AXIS_ROTATION) {
                        params[4] = fixNegativeZero(
                            rotSign *
                            measure.convertToUnit(
                                common::UnitOfMeasure::ARC_SECOND));
                        foundRotY = true;
                    } else if (epsg_code ==
                               EPSG_CODE_PARAMETER_Z_AXIS_ROTATION) {
                        params[5] = fixNegativeZero(
                            rotSign *
                            measure.convertToUnit(
                                common::UnitOfMeasure::ARC_SECOND));
                        foundRotZ = true;
                    } else if (epsg_code ==
                               EPSG_CODE_PARAMETER_SCALE_DIFFERENCE) {
                        params[6] = measure.convertToUnit(
                            common::UnitOfMeasure::PARTS_PER_MILLION);
                        foundScale = true;
                    }
                }
            }
        }
        if (foundX && foundY && foundZ &&
            (threeParamsTransform ||
             (foundRotX && foundRotY && foundRotZ && foundScale))) {
            return params;
        } else {
            throw io::FormattingException(
                "Missing required parameter values in transformation");
        }
    }

#if 0
    if (methodEPSGCode == EPSG_CODE_METHOD_GEOGRAPHIC2D_OFFSETS ||
        methodEPSGCode == EPSG_CODE_METHOD_GEOGRAPHIC3D_OFFSETS) {
        auto offsetLat =
            parameterValueMeasure(EPSG_CODE_PARAMETER_LATITUDE_OFFSET);
        auto offsetLong =
            parameterValueMeasure(EPSG_CODE_PARAMETER_LONGITUDE_OFFSET);

        auto offsetHeight =
            parameterValueMeasure(EPSG_CODE_PARAMETER_VERTICAL_OFFSET);

        if (offsetLat.getSIValue() == 0.0 && offsetLong.getSIValue() == 0.0 &&
            offsetHeight.getSIValue() == 0.0) {
            std::vector<double> params(7, 0.0);
            return params;
        }
    }
#endif

    throw io::FormattingException(
        "Transformation cannot be formatted as WKT1 TOWGS84 parameters");
}
//! @endcond

// ---------------------------------------------------------------------------

/** \brief Instantiate a transformation from a vector of GeneralParameterValue.
 *
 * @param properties See \ref general_properties. At minimum the name should be
 * defined.
 * @param sourceCRSIn Source CRS.
 * @param targetCRSIn Target CRS.
 * @param interpolationCRSIn Interpolation CRS (might be null)
 * @param methodIn Operation method.
 * @param values Vector of GeneralOperationParameterNNPtr.
 * @param accuracies Vector of positional accuracy (might be empty).
 * @return new Transformation.
 * @throws InvalidOperation
 */
TransformationNNPtr Transformation::create(
    const util::PropertyMap &properties, const crs::CRSNNPtr &sourceCRSIn,
    const crs::CRSNNPtr &targetCRSIn, const crs::CRSPtr &interpolationCRSIn,
    const OperationMethodNNPtr &methodIn,
    const std::vector<GeneralParameterValueNNPtr> &values,
    const std::vector<metadata::PositionalAccuracyNNPtr> &accuracies) {
    if (methodIn->parameters().size() != values.size()) {
        throw InvalidOperation(
            "Inconsistent number of parameters and parameter values");
    }
    auto transf = Transformation::nn_make_shared<Transformation>(
        sourceCRSIn, targetCRSIn, interpolationCRSIn, methodIn, values,
        accuracies);
    transf->assignSelf(transf);
    transf->setProperties(properties);
    std::string name;
    if (properties.getStringValue(common::IdentifiedObject::NAME_KEY, name) &&
        ci_find(name, "ballpark") != std::string::npos) {
        transf->setHasBallparkTransformation(true);
    }
    return transf;
}

// ---------------------------------------------------------------------------

/** \brief Instantiate a transformation ands its OperationMethod.
 *
 * @param propertiesTransformation The \ref general_properties of the
 * Transformation.
 * At minimum the name should be defined.
 * @param sourceCRSIn Source CRS.
 * @param targetCRSIn Target CRS.
 * @param interpolationCRSIn Interpolation CRS (might be null)
 * @param propertiesOperationMethod The \ref general_properties of the
 * OperationMethod.
 * At minimum the name should be defined.
 * @param parameters Vector of parameters of the operation method.
 * @param values Vector of ParameterValueNNPtr. Constraint:
 * values.size() == parameters.size()
 * @param accuracies Vector of positional accuracy (might be empty).
 * @return new Transformation.
 * @throws InvalidOperation
 */
TransformationNNPtr
Transformation::create(const util::PropertyMap &propertiesTransformation,
                       const crs::CRSNNPtr &sourceCRSIn,
                       const crs::CRSNNPtr &targetCRSIn,
                       const crs::CRSPtr &interpolationCRSIn,
                       const util::PropertyMap &propertiesOperationMethod,
                       const std::vector<OperationParameterNNPtr> &parameters,
                       const std::vector<ParameterValueNNPtr> &values,
                       const std::vector<metadata::PositionalAccuracyNNPtr>
                           &accuracies) // throw InvalidOperation
{
    OperationMethodNNPtr op(
        OperationMethod::create(propertiesOperationMethod, parameters));

    if (parameters.size() != values.size()) {
        throw InvalidOperation(
            "Inconsistent number of parameters and parameter values");
    }
    std::vector<GeneralParameterValueNNPtr> generalParameterValues;
    generalParameterValues.reserve(values.size());
    for (size_t i = 0; i < values.size(); i++) {
        generalParameterValues.push_back(
            OperationParameterValue::create(parameters[i], values[i]));
    }
    return create(propertiesTransformation, sourceCRSIn, targetCRSIn,
                  interpolationCRSIn, op, generalParameterValues, accuracies);
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress

// ---------------------------------------------------------------------------

static TransformationNNPtr createSevenParamsTransform(
    const util::PropertyMap &properties,
    const util::PropertyMap &methodProperties, const crs::CRSNNPtr &sourceCRSIn,
    const crs::CRSNNPtr &targetCRSIn, double translationXMetre,
    double translationYMetre, double translationZMetre,
    double rotationXArcSecond, double rotationYArcSecond,
    double rotationZArcSecond, double scaleDifferencePPM,
    const std::vector<metadata::PositionalAccuracyNNPtr> &accuracies) {
    return Transformation::create(
        properties, sourceCRSIn, targetCRSIn, nullptr, methodProperties,
        VectorOfParameters{
            createOpParamNameEPSGCode(EPSG_CODE_PARAMETER_X_AXIS_TRANSLATION),
            createOpParamNameEPSGCode(EPSG_CODE_PARAMETER_Y_AXIS_TRANSLATION),
            createOpParamNameEPSGCode(EPSG_CODE_PARAMETER_Z_AXIS_TRANSLATION),
            createOpParamNameEPSGCode(EPSG_CODE_PARAMETER_X_AXIS_ROTATION),
            createOpParamNameEPSGCode(EPSG_CODE_PARAMETER_Y_AXIS_ROTATION),
            createOpParamNameEPSGCode(EPSG_CODE_PARAMETER_Z_AXIS_ROTATION),
            createOpParamNameEPSGCode(EPSG_CODE_PARAMETER_SCALE_DIFFERENCE),
        },
        createParams(common::Length(translationXMetre),
                     common::Length(translationYMetre),
                     common::Length(translationZMetre),
                     common::Angle(rotationXArcSecond,
                                   common::UnitOfMeasure::ARC_SECOND),
                     common::Angle(rotationYArcSecond,
                                   common::UnitOfMeasure::ARC_SECOND),
                     common::Angle(rotationZArcSecond,
                                   common::UnitOfMeasure::ARC_SECOND),
                     common::Scale(scaleDifferencePPM,
                                   common::UnitOfMeasure::PARTS_PER_MILLION)),
        accuracies);
}

// ---------------------------------------------------------------------------

static void getTransformationType(const crs::CRSNNPtr &sourceCRSIn,
                                  const crs::CRSNNPtr &targetCRSIn,
                                  bool &isGeocentric, bool &isGeog2D,
                                  bool &isGeog3D) {
    auto sourceCRSGeod =
        dynamic_cast<const crs::GeodeticCRS *>(sourceCRSIn.get());
    auto targetCRSGeod =
        dynamic_cast<const crs::GeodeticCRS *>(targetCRSIn.get());
    isGeocentric = sourceCRSGeod && sourceCRSGeod->isGeocentric() &&
                   targetCRSGeod && targetCRSGeod->isGeocentric();
    if (isGeocentric) {
        isGeog2D = false;
        isGeog3D = false;
        return;
    }
    isGeocentric = false;

    auto sourceCRSGeog =
        dynamic_cast<const crs::GeographicCRS *>(sourceCRSIn.get());
    auto targetCRSGeog =
        dynamic_cast<const crs::GeographicCRS *>(targetCRSIn.get());
    if (!sourceCRSGeog || !targetCRSGeog) {
        throw InvalidOperation("Inconsistent CRS type");
    }
    const auto nSrcAxisCount =
        sourceCRSGeog->coordinateSystem()->axisList().size();
    const auto nTargetAxisCount =
        targetCRSGeog->coordinateSystem()->axisList().size();
    isGeog2D = nSrcAxisCount == 2 && nTargetAxisCount == 2;
    isGeog3D = !isGeog2D && nSrcAxisCount >= 2 && nTargetAxisCount >= 2;
}

// ---------------------------------------------------------------------------

static int
useOperationMethodEPSGCodeIfPresent(const util::PropertyMap &properties,
                                    int nDefaultOperationMethodEPSGCode) {
    const auto *operationMethodEPSGCode =
        properties.get("OPERATION_METHOD_EPSG_CODE");
    if (operationMethodEPSGCode) {
        const auto boxedValue = dynamic_cast<const util::BoxedValue *>(
            (*operationMethodEPSGCode).get());
        if (boxedValue &&
            boxedValue->type() == util::BoxedValue::Type::INTEGER) {
            return boxedValue->integerValue();
        }
    }
    return nDefaultOperationMethodEPSGCode;
}
//! @endcond

// ---------------------------------------------------------------------------

/** \brief Instantiate a transformation with Geocentric Translations method.
 *
 * @param properties See \ref general_properties of the Transformation.
 * At minimum the name should be defined.
 * @param sourceCRSIn Source CRS.
 * @param targetCRSIn Target CRS.
 * @param translationXMetre Value of the Translation_X parameter (in metre).
 * @param translationYMetre Value of the Translation_Y parameter (in metre).
 * @param translationZMetre Value of the Translation_Z parameter (in metre).
 * @param accuracies Vector of positional accuracy (might be empty).
 * @return new Transformation.
 */
TransformationNNPtr Transformation::createGeocentricTranslations(
    const util::PropertyMap &properties, const crs::CRSNNPtr &sourceCRSIn,
    const crs::CRSNNPtr &targetCRSIn, double translationXMetre,
    double translationYMetre, double translationZMetre,
    const std::vector<metadata::PositionalAccuracyNNPtr> &accuracies) {
    bool isGeocentric;
    bool isGeog2D;
    bool isGeog3D;
    getTransformationType(sourceCRSIn, targetCRSIn, isGeocentric, isGeog2D,
                          isGeog3D);
    return create(
        properties, sourceCRSIn, targetCRSIn, nullptr,
        createMethodMapNameEPSGCode(useOperationMethodEPSGCodeIfPresent(
            properties,
            isGeocentric
                ? EPSG_CODE_METHOD_GEOCENTRIC_TRANSLATION_GEOCENTRIC
                : isGeog2D
                      ? EPSG_CODE_METHOD_GEOCENTRIC_TRANSLATION_GEOGRAPHIC_2D
                      : EPSG_CODE_METHOD_GEOCENTRIC_TRANSLATION_GEOGRAPHIC_3D)),
        VectorOfParameters{
            createOpParamNameEPSGCode(EPSG_CODE_PARAMETER_X_AXIS_TRANSLATION),
            createOpParamNameEPSGCode(EPSG_CODE_PARAMETER_Y_AXIS_TRANSLATION),
            createOpParamNameEPSGCode(EPSG_CODE_PARAMETER_Z_AXIS_TRANSLATION),
        },
        createParams(common::Length(translationXMetre),
                     common::Length(translationYMetre),
                     common::Length(translationZMetre)),
        accuracies);
}

// ---------------------------------------------------------------------------

/** \brief Instantiate a transformation with Position vector transformation
 * method.
 *
 * This is similar to createCoordinateFrameRotation(), except that the sign of
 * the rotation terms is inverted.
 *
 * @param properties See \ref general_properties of the Transformation.
 * At minimum the name should be defined.
 * @param sourceCRSIn Source CRS.
 * @param targetCRSIn Target CRS.
 * @param translationXMetre Value of the Translation_X parameter (in metre).
 * @param translationYMetre Value of the Translation_Y parameter (in metre).
 * @param translationZMetre Value of the Translation_Z parameter (in metre).
 * @param rotationXArcSecond Value of the Rotation_X parameter (in
 * arc-second).
 * @param rotationYArcSecond Value of the Rotation_Y parameter (in
 * arc-second).
 * @param rotationZArcSecond Value of the Rotation_Z parameter (in
 * arc-second).
 * @param scaleDifferencePPM Value of the Scale_Difference parameter (in
 * parts-per-million).
 * @param accuracies Vector of positional accuracy (might be empty).
 * @return new Transformation.
 */
TransformationNNPtr Transformation::createPositionVector(
    const util::PropertyMap &properties, const crs::CRSNNPtr &sourceCRSIn,
    const crs::CRSNNPtr &targetCRSIn, double translationXMetre,
    double translationYMetre, double translationZMetre,
    double rotationXArcSecond, double rotationYArcSecond,
    double rotationZArcSecond, double scaleDifferencePPM,
    const std::vector<metadata::PositionalAccuracyNNPtr> &accuracies) {

    bool isGeocentric;
    bool isGeog2D;
    bool isGeog3D;
    getTransformationType(sourceCRSIn, targetCRSIn, isGeocentric, isGeog2D,
                          isGeog3D);
    return createSevenParamsTransform(
        properties,
        createMethodMapNameEPSGCode(useOperationMethodEPSGCodeIfPresent(
            properties,
            isGeocentric
                ? EPSG_CODE_METHOD_POSITION_VECTOR_GEOCENTRIC
                : isGeog2D ? EPSG_CODE_METHOD_POSITION_VECTOR_GEOGRAPHIC_2D
                           : EPSG_CODE_METHOD_POSITION_VECTOR_GEOGRAPHIC_3D)),
        sourceCRSIn, targetCRSIn, translationXMetre, translationYMetre,
        translationZMetre, rotationXArcSecond, rotationYArcSecond,
        rotationZArcSecond, scaleDifferencePPM, accuracies);
}

// ---------------------------------------------------------------------------

/** \brief Instantiate a transformation with Coordinate Frame Rotation method.
 *
 * This is similar to createPositionVector(), except that the sign of
 * the rotation terms is inverted.
 *
 * @param properties See \ref general_properties of the Transformation.
 * At minimum the name should be defined.
 * @param sourceCRSIn Source CRS.
 * @param targetCRSIn Target CRS.
 * @param translationXMetre Value of the Translation_X parameter (in metre).
 * @param translationYMetre Value of the Translation_Y parameter (in metre).
 * @param translationZMetre Value of the Translation_Z parameter (in metre).
 * @param rotationXArcSecond Value of the Rotation_X parameter (in
 * arc-second).
 * @param rotationYArcSecond Value of the Rotation_Y parameter (in
 * arc-second).
 * @param rotationZArcSecond Value of the Rotation_Z parameter (in
 * arc-second).
 * @param scaleDifferencePPM Value of the Scale_Difference parameter (in
 * parts-per-million).
 * @param accuracies Vector of positional accuracy (might be empty).
 * @return new Transformation.
 */
TransformationNNPtr Transformation::createCoordinateFrameRotation(
    const util::PropertyMap &properties, const crs::CRSNNPtr &sourceCRSIn,
    const crs::CRSNNPtr &targetCRSIn, double translationXMetre,
    double translationYMetre, double translationZMetre,
    double rotationXArcSecond, double rotationYArcSecond,
    double rotationZArcSecond, double scaleDifferencePPM,
    const std::vector<metadata::PositionalAccuracyNNPtr> &accuracies) {
    bool isGeocentric;
    bool isGeog2D;
    bool isGeog3D;
    getTransformationType(sourceCRSIn, targetCRSIn, isGeocentric, isGeog2D,
                          isGeog3D);
    return createSevenParamsTransform(
        properties,
        createMethodMapNameEPSGCode(useOperationMethodEPSGCodeIfPresent(
            properties,
            isGeocentric
                ? EPSG_CODE_METHOD_COORDINATE_FRAME_GEOCENTRIC
                : isGeog2D ? EPSG_CODE_METHOD_COORDINATE_FRAME_GEOGRAPHIC_2D
                           : EPSG_CODE_METHOD_COORDINATE_FRAME_GEOGRAPHIC_3D)),
        sourceCRSIn, targetCRSIn, translationXMetre, translationYMetre,
        translationZMetre, rotationXArcSecond, rotationYArcSecond,
        rotationZArcSecond, scaleDifferencePPM, accuracies);
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
static TransformationNNPtr createFifteenParamsTransform(
    const util::PropertyMap &properties,
    const util::PropertyMap &methodProperties, const crs::CRSNNPtr &sourceCRSIn,
    const crs::CRSNNPtr &targetCRSIn, double translationXMetre,
    double translationYMetre, double translationZMetre,
    double rotationXArcSecond, double rotationYArcSecond,
    double rotationZArcSecond, double scaleDifferencePPM,
    double rateTranslationX, double rateTranslationY, double rateTranslationZ,
    double rateRotationX, double rateRotationY, double rateRotationZ,
    double rateScaleDifference, double referenceEpochYear,
    const std::vector<metadata::PositionalAccuracyNNPtr> &accuracies) {
    return Transformation::create(
        properties, sourceCRSIn, targetCRSIn, nullptr, methodProperties,
        VectorOfParameters{
            createOpParamNameEPSGCode(EPSG_CODE_PARAMETER_X_AXIS_TRANSLATION),
            createOpParamNameEPSGCode(EPSG_CODE_PARAMETER_Y_AXIS_TRANSLATION),
            createOpParamNameEPSGCode(EPSG_CODE_PARAMETER_Z_AXIS_TRANSLATION),
            createOpParamNameEPSGCode(EPSG_CODE_PARAMETER_X_AXIS_ROTATION),
            createOpParamNameEPSGCode(EPSG_CODE_PARAMETER_Y_AXIS_ROTATION),
            createOpParamNameEPSGCode(EPSG_CODE_PARAMETER_Z_AXIS_ROTATION),
            createOpParamNameEPSGCode(EPSG_CODE_PARAMETER_SCALE_DIFFERENCE),

            createOpParamNameEPSGCode(
                EPSG_CODE_PARAMETER_RATE_X_AXIS_TRANSLATION),
            createOpParamNameEPSGCode(
                EPSG_CODE_PARAMETER_RATE_Y_AXIS_TRANSLATION),
            createOpParamNameEPSGCode(
                EPSG_CODE_PARAMETER_RATE_Z_AXIS_TRANSLATION),
            createOpParamNameEPSGCode(EPSG_CODE_PARAMETER_RATE_X_AXIS_ROTATION),
            createOpParamNameEPSGCode(EPSG_CODE_PARAMETER_RATE_Y_AXIS_ROTATION),
            createOpParamNameEPSGCode(EPSG_CODE_PARAMETER_RATE_Z_AXIS_ROTATION),
            createOpParamNameEPSGCode(
                EPSG_CODE_PARAMETER_RATE_SCALE_DIFFERENCE),

            createOpParamNameEPSGCode(EPSG_CODE_PARAMETER_REFERENCE_EPOCH),
        },
        VectorOfValues{
            common::Length(translationXMetre),
            common::Length(translationYMetre),
            common::Length(translationZMetre),
            common::Angle(rotationXArcSecond,
                          common::UnitOfMeasure::ARC_SECOND),
            common::Angle(rotationYArcSecond,
                          common::UnitOfMeasure::ARC_SECOND),
            common::Angle(rotationZArcSecond,
                          common::UnitOfMeasure::ARC_SECOND),
            common::Scale(scaleDifferencePPM,
                          common::UnitOfMeasure::PARTS_PER_MILLION),
            common::Measure(rateTranslationX,
                            common::UnitOfMeasure::METRE_PER_YEAR),
            common::Measure(rateTranslationY,
                            common::UnitOfMeasure::METRE_PER_YEAR),
            common::Measure(rateTranslationZ,
                            common::UnitOfMeasure::METRE_PER_YEAR),
            common::Measure(rateRotationX,
                            common::UnitOfMeasure::ARC_SECOND_PER_YEAR),
            common::Measure(rateRotationY,
                            common::UnitOfMeasure::ARC_SECOND_PER_YEAR),
            common::Measure(rateRotationZ,
                            common::UnitOfMeasure::ARC_SECOND_PER_YEAR),
            common::Measure(rateScaleDifference,
                            common::UnitOfMeasure::PPM_PER_YEAR),
            common::Measure(referenceEpochYear, common::UnitOfMeasure::YEAR),
        },
        accuracies);
}
//! @endcond

// ---------------------------------------------------------------------------

/** \brief Instantiate a transformation with Time Dependent position vector
 * transformation method.
 *
 * This is similar to createTimeDependentCoordinateFrameRotation(), except that
 * the sign of
 * the rotation terms is inverted.
 *
 * This method is defined as [EPSG:1053]
 * (https://www.epsg-registry.org/export.htm?gml=urn:ogc:def:method:EPSG::1053)
 *
 * @param properties See \ref general_properties of the Transformation.
 * At minimum the name should be defined.
 * @param sourceCRSIn Source CRS.
 * @param targetCRSIn Target CRS.
 * @param translationXMetre Value of the Translation_X parameter (in metre).
 * @param translationYMetre Value of the Translation_Y parameter (in metre).
 * @param translationZMetre Value of the Translation_Z parameter (in metre).
 * @param rotationXArcSecond Value of the Rotation_X parameter (in
 * arc-second).
 * @param rotationYArcSecond Value of the Rotation_Y parameter (in
 * arc-second).
 * @param rotationZArcSecond Value of the Rotation_Z parameter (in
 * arc-second).
 * @param scaleDifferencePPM Value of the Scale_Difference parameter (in
 * parts-per-million).
 * @param rateTranslationX Value of the rate of change of X-axis translation (in
 * metre/year)
 * @param rateTranslationY Value of the rate of change of Y-axis translation (in
 * metre/year)
 * @param rateTranslationZ Value of the rate of change of Z-axis translation (in
 * metre/year)
 * @param rateRotationX Value of the rate of change of X-axis rotation (in
 * arc-second/year)
 * @param rateRotationY Value of the rate of change of Y-axis rotation (in
 * arc-second/year)
 * @param rateRotationZ Value of the rate of change of Z-axis rotation (in
 * arc-second/year)
 * @param rateScaleDifference Value of the rate of change of scale difference
 * (in PPM/year)
 * @param referenceEpochYear Parameter reference epoch (in decimal year)
 * @param accuracies Vector of positional accuracy (might be empty).
 * @return new Transformation.
 */
TransformationNNPtr Transformation::createTimeDependentPositionVector(
    const util::PropertyMap &properties, const crs::CRSNNPtr &sourceCRSIn,
    const crs::CRSNNPtr &targetCRSIn, double translationXMetre,
    double translationYMetre, double translationZMetre,
    double rotationXArcSecond, double rotationYArcSecond,
    double rotationZArcSecond, double scaleDifferencePPM,
    double rateTranslationX, double rateTranslationY, double rateTranslationZ,
    double rateRotationX, double rateRotationY, double rateRotationZ,
    double rateScaleDifference, double referenceEpochYear,
    const std::vector<metadata::PositionalAccuracyNNPtr> &accuracies) {
    bool isGeocentric;
    bool isGeog2D;
    bool isGeog3D;
    getTransformationType(sourceCRSIn, targetCRSIn, isGeocentric, isGeog2D,
                          isGeog3D);
    return createFifteenParamsTransform(
        properties,
        createMethodMapNameEPSGCode(useOperationMethodEPSGCodeIfPresent(
            properties,
            isGeocentric
                ? EPSG_CODE_METHOD_TIME_DEPENDENT_POSITION_VECTOR_GEOCENTRIC
                : isGeog2D
                      ? EPSG_CODE_METHOD_TIME_DEPENDENT_POSITION_VECTOR_GEOGRAPHIC_2D
                      : EPSG_CODE_METHOD_TIME_DEPENDENT_POSITION_VECTOR_GEOGRAPHIC_3D)),
        sourceCRSIn, targetCRSIn, translationXMetre, translationYMetre,
        translationZMetre, rotationXArcSecond, rotationYArcSecond,
        rotationZArcSecond, scaleDifferencePPM, rateTranslationX,
        rateTranslationY, rateTranslationZ, rateRotationX, rateRotationY,
        rateRotationZ, rateScaleDifference, referenceEpochYear, accuracies);
}

// ---------------------------------------------------------------------------

/** \brief Instantiate a transformation with Time Dependent Position coordinate
 * frame rotation transformation method.
 *
 * This is similar to createTimeDependentPositionVector(), except that the sign
 * of
 * the rotation terms is inverted.
 *
 * This method is defined as [EPSG:1056]
 * (https://www.epsg-registry.org/export.htm?gml=urn:ogc:def:method:EPSG::1056)
 *
 * @param properties See \ref general_properties of the Transformation.
 * At minimum the name should be defined.
 * @param sourceCRSIn Source CRS.
 * @param targetCRSIn Target CRS.
 * @param translationXMetre Value of the Translation_X parameter (in metre).
 * @param translationYMetre Value of the Translation_Y parameter (in metre).
 * @param translationZMetre Value of the Translation_Z parameter (in metre).
 * @param rotationXArcSecond Value of the Rotation_X parameter (in
 * arc-second).
 * @param rotationYArcSecond Value of the Rotation_Y parameter (in
 * arc-second).
 * @param rotationZArcSecond Value of the Rotation_Z parameter (in
 * arc-second).
 * @param scaleDifferencePPM Value of the Scale_Difference parameter (in
 * parts-per-million).
 * @param rateTranslationX Value of the rate of change of X-axis translation (in
 * metre/year)
 * @param rateTranslationY Value of the rate of change of Y-axis translation (in
 * metre/year)
 * @param rateTranslationZ Value of the rate of change of Z-axis translation (in
 * metre/year)
 * @param rateRotationX Value of the rate of change of X-axis rotation (in
 * arc-second/year)
 * @param rateRotationY Value of the rate of change of Y-axis rotation (in
 * arc-second/year)
 * @param rateRotationZ Value of the rate of change of Z-axis rotation (in
 * arc-second/year)
 * @param rateScaleDifference Value of the rate of change of scale difference
 * (in PPM/year)
 * @param referenceEpochYear Parameter reference epoch (in decimal year)
 * @param accuracies Vector of positional accuracy (might be empty).
 * @return new Transformation.
 * @throws InvalidOperation
 */
TransformationNNPtr Transformation::createTimeDependentCoordinateFrameRotation(
    const util::PropertyMap &properties, const crs::CRSNNPtr &sourceCRSIn,
    const crs::CRSNNPtr &targetCRSIn, double translationXMetre,
    double translationYMetre, double translationZMetre,
    double rotationXArcSecond, double rotationYArcSecond,
    double rotationZArcSecond, double scaleDifferencePPM,
    double rateTranslationX, double rateTranslationY, double rateTranslationZ,
    double rateRotationX, double rateRotationY, double rateRotationZ,
    double rateScaleDifference, double referenceEpochYear,
    const std::vector<metadata::PositionalAccuracyNNPtr> &accuracies) {

    bool isGeocentric;
    bool isGeog2D;
    bool isGeog3D;
    getTransformationType(sourceCRSIn, targetCRSIn, isGeocentric, isGeog2D,
                          isGeog3D);
    return createFifteenParamsTransform(
        properties,
        createMethodMapNameEPSGCode(useOperationMethodEPSGCodeIfPresent(
            properties,
            isGeocentric
                ? EPSG_CODE_METHOD_TIME_DEPENDENT_COORDINATE_FRAME_GEOCENTRIC
                : isGeog2D
                      ? EPSG_CODE_METHOD_TIME_DEPENDENT_COORDINATE_FRAME_GEOGRAPHIC_2D
                      : EPSG_CODE_METHOD_TIME_DEPENDENT_COORDINATE_FRAME_GEOGRAPHIC_3D)),
        sourceCRSIn, targetCRSIn, translationXMetre, translationYMetre,
        translationZMetre, rotationXArcSecond, rotationYArcSecond,
        rotationZArcSecond, scaleDifferencePPM, rateTranslationX,
        rateTranslationY, rateTranslationZ, rateRotationX, rateRotationY,
        rateRotationZ, rateScaleDifference, referenceEpochYear, accuracies);
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
static TransformationNNPtr _createMolodensky(
    const util::PropertyMap &properties, const crs::CRSNNPtr &sourceCRSIn,
    const crs::CRSNNPtr &targetCRSIn, int methodEPSGCode,
    double translationXMetre, double translationYMetre,
    double translationZMetre, double semiMajorAxisDifferenceMetre,
    double flattingDifference,
    const std::vector<metadata::PositionalAccuracyNNPtr> &accuracies) {
    return Transformation::create(
        properties, sourceCRSIn, targetCRSIn, nullptr,
        createMethodMapNameEPSGCode(methodEPSGCode),
        VectorOfParameters{
            createOpParamNameEPSGCode(EPSG_CODE_PARAMETER_X_AXIS_TRANSLATION),
            createOpParamNameEPSGCode(EPSG_CODE_PARAMETER_Y_AXIS_TRANSLATION),
            createOpParamNameEPSGCode(EPSG_CODE_PARAMETER_Z_AXIS_TRANSLATION),
            createOpParamNameEPSGCode(
                EPSG_CODE_PARAMETER_SEMI_MAJOR_AXIS_DIFFERENCE),
            createOpParamNameEPSGCode(
                EPSG_CODE_PARAMETER_FLATTENING_DIFFERENCE),
        },
        createParams(
            common::Length(translationXMetre),
            common::Length(translationYMetre),
            common::Length(translationZMetre),
            common::Length(semiMajorAxisDifferenceMetre),
            common::Measure(flattingDifference, common::UnitOfMeasure::NONE)),
        accuracies);
}
//! @endcond

// ---------------------------------------------------------------------------

/** \brief Instantiate a transformation with Molodensky method.
 *
 * @see createAbridgedMolodensky() for a related method.
 *
 * This method is defined as [EPSG:9604]
 * (https://www.epsg-registry.org/export.htm?gml=urn:ogc:def:method:EPSG::9604)
 *
 * @param properties See \ref general_properties of the Transformation.
 * At minimum the name should be defined.
 * @param sourceCRSIn Source CRS.
 * @param targetCRSIn Target CRS.
 * @param translationXMetre Value of the Translation_X parameter (in metre).
 * @param translationYMetre Value of the Translation_Y parameter (in metre).
 * @param translationZMetre Value of the Translation_Z parameter (in metre).
 * @param semiMajorAxisDifferenceMetre The difference between the semi-major
 * axis values of the ellipsoids used in the target and source CRS (in metre).
 * @param flattingDifference The difference  between the flattening values of
 * the ellipsoids used in the target and source CRS.
 * @param accuracies Vector of positional accuracy (might be empty).
 * @return new Transformation.
 * @throws InvalidOperation
 */
TransformationNNPtr Transformation::createMolodensky(
    const util::PropertyMap &properties, const crs::CRSNNPtr &sourceCRSIn,
    const crs::CRSNNPtr &targetCRSIn, double translationXMetre,
    double translationYMetre, double translationZMetre,
    double semiMajorAxisDifferenceMetre, double flattingDifference,
    const std::vector<metadata::PositionalAccuracyNNPtr> &accuracies) {
    return _createMolodensky(
        properties, sourceCRSIn, targetCRSIn, EPSG_CODE_METHOD_MOLODENSKY,
        translationXMetre, translationYMetre, translationZMetre,
        semiMajorAxisDifferenceMetre, flattingDifference, accuracies);
}

// ---------------------------------------------------------------------------

/** \brief Instantiate a transformation with Abridged Molodensky method.
 *
 * @see createdMolodensky() for a related method.
 *
 * This method is defined as [EPSG:9605]
 * (https://www.epsg-registry.org/export.htm?gml=urn:ogc:def:method:EPSG::9605)
 *
 * @param properties See \ref general_properties of the Transformation.
 * At minimum the name should be defined.
 * @param sourceCRSIn Source CRS.
 * @param targetCRSIn Target CRS.
 * @param translationXMetre Value of the Translation_X parameter (in metre).
 * @param translationYMetre Value of the Translation_Y parameter (in metre).
 * @param translationZMetre Value of the Translation_Z parameter (in metre).
 * @param semiMajorAxisDifferenceMetre The difference between the semi-major
 * axis values of the ellipsoids used in the target and source CRS (in metre).
 * @param flattingDifference The difference  between the flattening values of
 * the ellipsoids used in the target and source CRS.
 * @param accuracies Vector of positional accuracy (might be empty).
 * @return new Transformation.
 * @throws InvalidOperation
 */
TransformationNNPtr Transformation::createAbridgedMolodensky(
    const util::PropertyMap &properties, const crs::CRSNNPtr &sourceCRSIn,
    const crs::CRSNNPtr &targetCRSIn, double translationXMetre,
    double translationYMetre, double translationZMetre,
    double semiMajorAxisDifferenceMetre, double flattingDifference,
    const std::vector<metadata::PositionalAccuracyNNPtr> &accuracies) {
    return _createMolodensky(properties, sourceCRSIn, targetCRSIn,
                             EPSG_CODE_METHOD_ABRIDGED_MOLODENSKY,
                             translationXMetre, translationYMetre,
                             translationZMetre, semiMajorAxisDifferenceMetre,
                             flattingDifference, accuracies);
}

// ---------------------------------------------------------------------------

/** \brief Instantiate a transformation from TOWGS84 parameters.
 *
 * This is a helper of createPositionVector() with the source CRS being the
 * GeographicCRS of sourceCRSIn, and the target CRS being EPSG:4326
 *
 * @param sourceCRSIn Source CRS.
 * @param TOWGS84Parameters The vector of 3 double values (Translation_X,_Y,_Z)
 * or 7 double values (Translation_X,_Y,_Z, Rotation_X,_Y,_Z, Scale_Difference)
 * passed to createPositionVector()
 * @return new Transformation.
 * @throws InvalidOperation
 */
TransformationNNPtr Transformation::createTOWGS84(
    const crs::CRSNNPtr &sourceCRSIn,
    const std::vector<double> &TOWGS84Parameters) // throw InvalidOperation
{
    if (TOWGS84Parameters.size() != 3 && TOWGS84Parameters.size() != 7) {
        throw InvalidOperation(
            "Invalid number of elements in TOWGS84Parameters");
    }

    crs::CRSPtr transformSourceCRS = sourceCRSIn->extractGeodeticCRS();
    if (!transformSourceCRS) {
        throw InvalidOperation(
            "Cannot find GeodeticCRS in sourceCRS of TOWGS84 transformation");
    }

    util::PropertyMap properties;
    properties.set(common::IdentifiedObject::NAME_KEY,
                   concat("Transformation from ", transformSourceCRS->nameStr(),
                          " to WGS84"));

    auto targetCRS =
        dynamic_cast<const crs::GeographicCRS *>(transformSourceCRS.get())
            ? util::nn_static_pointer_cast<crs::CRS>(
                  crs::GeographicCRS::EPSG_4326)
            : util::nn_static_pointer_cast<crs::CRS>(
                  crs::GeodeticCRS::EPSG_4978);

    if (TOWGS84Parameters.size() == 3) {
        return createGeocentricTranslations(
            properties, NN_NO_CHECK(transformSourceCRS), targetCRS,
            TOWGS84Parameters[0], TOWGS84Parameters[1], TOWGS84Parameters[2],
            {});
    }

    return createPositionVector(properties, NN_NO_CHECK(transformSourceCRS),
                                targetCRS, TOWGS84Parameters[0],
                                TOWGS84Parameters[1], TOWGS84Parameters[2],
                                TOWGS84Parameters[3], TOWGS84Parameters[4],
                                TOWGS84Parameters[5], TOWGS84Parameters[6], {});
}

// ---------------------------------------------------------------------------
/** \brief Instantiate a transformation with NTv2 method.
 *
 * @param properties See \ref general_properties of the Transformation.
 * At minimum the name should be defined.
 * @param sourceCRSIn Source CRS.
 * @param targetCRSIn Target CRS.
 * @param filename NTv2 filename.
 * @param accuracies Vector of positional accuracy (might be empty).
 * @return new Transformation.
 */
TransformationNNPtr Transformation::createNTv2(
    const util::PropertyMap &properties, const crs::CRSNNPtr &sourceCRSIn,
    const crs::CRSNNPtr &targetCRSIn, const std::string &filename,
    const std::vector<metadata::PositionalAccuracyNNPtr> &accuracies) {

    return create(properties, sourceCRSIn, targetCRSIn, nullptr,
                  createMethodMapNameEPSGCode(EPSG_CODE_METHOD_NTV2),
                  VectorOfParameters{createOpParamNameEPSGCode(
                      EPSG_CODE_PARAMETER_LATITUDE_LONGITUDE_DIFFERENCE_FILE)},
                  VectorOfValues{ParameterValue::createFilename(filename)},
                  accuracies);
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
static TransformationNNPtr _createGravityRelatedHeightToGeographic3D(
    const util::PropertyMap &properties, bool inverse,
    const crs::CRSNNPtr &sourceCRSIn, const crs::CRSNNPtr &targetCRSIn,
    const crs::CRSPtr &interpolationCRSIn, const std::string &filename,
    const std::vector<metadata::PositionalAccuracyNNPtr> &accuracies) {

    return Transformation::create(
        properties, sourceCRSIn, targetCRSIn, interpolationCRSIn,
        util::PropertyMap().set(
            common::IdentifiedObject::NAME_KEY,
            inverse ? INVERSE_OF + PROJ_WKT2_NAME_METHOD_HEIGHT_TO_GEOG3D
                    : PROJ_WKT2_NAME_METHOD_HEIGHT_TO_GEOG3D),
        VectorOfParameters{createOpParamNameEPSGCode(
            EPSG_CODE_PARAMETER_GEOID_CORRECTION_FILENAME)},
        VectorOfValues{ParameterValue::createFilename(filename)}, accuracies);
}
//! @endcond

// ---------------------------------------------------------------------------
/** \brief Instantiate a transformation from GravityRelatedHeight to
 * Geographic3D
 *
 * @param properties See \ref general_properties of the Transformation.
 * At minimum the name should be defined.
 * @param sourceCRSIn Source CRS.
 * @param targetCRSIn Target CRS.
 * @param interpolationCRSIn Interpolation CRS. (might be null)
 * @param filename GRID filename.
 * @param accuracies Vector of positional accuracy (might be empty).
 * @return new Transformation.
 */
TransformationNNPtr Transformation::createGravityRelatedHeightToGeographic3D(
    const util::PropertyMap &properties, const crs::CRSNNPtr &sourceCRSIn,
    const crs::CRSNNPtr &targetCRSIn, const crs::CRSPtr &interpolationCRSIn,
    const std::string &filename,
    const std::vector<metadata::PositionalAccuracyNNPtr> &accuracies) {

    return _createGravityRelatedHeightToGeographic3D(
        properties, false, sourceCRSIn, targetCRSIn, interpolationCRSIn,
        filename, accuracies);
}

// ---------------------------------------------------------------------------

/** \brief Instantiate a transformation with method VERTCON
 *
 * @param properties See \ref general_properties of the Transformation.
 * At minimum the name should be defined.
 * @param sourceCRSIn Source CRS.
 * @param targetCRSIn Target CRS.
 * @param filename GRID filename.
 * @param accuracies Vector of positional accuracy (might be empty).
 * @return new Transformation.
 */
TransformationNNPtr Transformation::createVERTCON(
    const util::PropertyMap &properties, const crs::CRSNNPtr &sourceCRSIn,
    const crs::CRSNNPtr &targetCRSIn, const std::string &filename,
    const std::vector<metadata::PositionalAccuracyNNPtr> &accuracies) {

    return create(properties, sourceCRSIn, targetCRSIn, nullptr,
                  createMethodMapNameEPSGCode(EPSG_CODE_METHOD_VERTCON),
                  VectorOfParameters{createOpParamNameEPSGCode(
                      EPSG_CODE_PARAMETER_VERTICAL_OFFSET_FILE)},
                  VectorOfValues{ParameterValue::createFilename(filename)},
                  accuracies);
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
static inline std::vector<metadata::PositionalAccuracyNNPtr>
buildAccuracyZero() {
    return std::vector<metadata::PositionalAccuracyNNPtr>{
        metadata::PositionalAccuracy::create("0")};
}

// ---------------------------------------------------------------------------

//! @endcond

/** \brief Instantiate a transformation with method Longitude rotation
 *
 * This method is defined as [EPSG:9601]
 * (https://www.epsg-registry.org/export.htm?gml=urn:ogc:def:method:EPSG::9601)
 * *
 * @param properties See \ref general_properties of the Transformation.
 * At minimum the name should be defined.
 * @param sourceCRSIn Source CRS.
 * @param targetCRSIn Target CRS.
 * @param offset Longitude offset to add.
 * @return new Transformation.
 */
TransformationNNPtr Transformation::createLongitudeRotation(
    const util::PropertyMap &properties, const crs::CRSNNPtr &sourceCRSIn,
    const crs::CRSNNPtr &targetCRSIn, const common::Angle &offset) {

    return create(
        properties, sourceCRSIn, targetCRSIn, nullptr,
        createMethodMapNameEPSGCode(EPSG_CODE_METHOD_LONGITUDE_ROTATION),
        VectorOfParameters{
            createOpParamNameEPSGCode(EPSG_CODE_PARAMETER_LONGITUDE_OFFSET)},
        VectorOfValues{ParameterValue::create(offset)}, buildAccuracyZero());
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
bool Transformation::isLongitudeRotation() const {
    return method()->getEPSGCode() == EPSG_CODE_METHOD_LONGITUDE_ROTATION;
}

//! @endcond

// ---------------------------------------------------------------------------

/** \brief Instantiate a transformation with method Geographic 2D offsets
 *
 * This method is defined as [EPSG:9619]
 * (https://www.epsg-registry.org/export.htm?gml=urn:ogc:def:method:EPSG::9619)
 * *
 * @param properties See \ref general_properties of the Transformation.
 * At minimum the name should be defined.
 * @param sourceCRSIn Source CRS.
 * @param targetCRSIn Target CRS.
 * @param offsetLat Latitude offset to add.
 * @param offsetLon Longitude offset to add.
 * @param accuracies Vector of positional accuracy (might be empty).
 * @return new Transformation.
 */
TransformationNNPtr Transformation::createGeographic2DOffsets(
    const util::PropertyMap &properties, const crs::CRSNNPtr &sourceCRSIn,
    const crs::CRSNNPtr &targetCRSIn, const common::Angle &offsetLat,
    const common::Angle &offsetLon,
    const std::vector<metadata::PositionalAccuracyNNPtr> &accuracies) {
    return create(
        properties, sourceCRSIn, targetCRSIn, nullptr,
        createMethodMapNameEPSGCode(EPSG_CODE_METHOD_GEOGRAPHIC2D_OFFSETS),
        VectorOfParameters{
            createOpParamNameEPSGCode(EPSG_CODE_PARAMETER_LATITUDE_OFFSET),
            createOpParamNameEPSGCode(EPSG_CODE_PARAMETER_LONGITUDE_OFFSET)},
        VectorOfValues{offsetLat, offsetLon}, accuracies);
}

// ---------------------------------------------------------------------------

/** \brief Instantiate a transformation with method Geographic 3D offsets
 *
 * This method is defined as [EPSG:9660]
 * (https://www.epsg-registry.org/export.htm?gml=urn:ogc:def:method:EPSG::9660)
 * *
 * @param properties See \ref general_properties of the Transformation.
 * At minimum the name should be defined.
 * @param sourceCRSIn Source CRS.
 * @param targetCRSIn Target CRS.
 * @param offsetLat Latitude offset to add.
 * @param offsetLon Longitude offset to add.
 * @param offsetHeight Height offset to add.
 * @param accuracies Vector of positional accuracy (might be empty).
 * @return new Transformation.
 */
TransformationNNPtr Transformation::createGeographic3DOffsets(
    const util::PropertyMap &properties, const crs::CRSNNPtr &sourceCRSIn,
    const crs::CRSNNPtr &targetCRSIn, const common::Angle &offsetLat,
    const common::Angle &offsetLon, const common::Length &offsetHeight,
    const std::vector<metadata::PositionalAccuracyNNPtr> &accuracies) {
    return create(
        properties, sourceCRSIn, targetCRSIn, nullptr,
        createMethodMapNameEPSGCode(EPSG_CODE_METHOD_GEOGRAPHIC3D_OFFSETS),
        VectorOfParameters{
            createOpParamNameEPSGCode(EPSG_CODE_PARAMETER_LATITUDE_OFFSET),
            createOpParamNameEPSGCode(EPSG_CODE_PARAMETER_LONGITUDE_OFFSET),
            createOpParamNameEPSGCode(EPSG_CODE_PARAMETER_VERTICAL_OFFSET)},
        VectorOfValues{offsetLat, offsetLon, offsetHeight}, accuracies);
}

// ---------------------------------------------------------------------------

/** \brief Instantiate a transformation with method Geographic 2D with
 * height
 * offsets
 *
 * This method is defined as [EPSG:9618]
 * (https://www.epsg-registry.org/export.htm?gml=urn:ogc:def:method:EPSG::9618)
 * *
 * @param properties See \ref general_properties of the Transformation.
 * At minimum the name should be defined.
 * @param sourceCRSIn Source CRS.
 * @param targetCRSIn Target CRS.
 * @param offsetLat Latitude offset to add.
 * @param offsetLon Longitude offset to add.
 * @param offsetHeight Geoid undulation to add.
 * @param accuracies Vector of positional accuracy (might be empty).
 * @return new Transformation.
 */
TransformationNNPtr Transformation::createGeographic2DWithHeightOffsets(
    const util::PropertyMap &properties, const crs::CRSNNPtr &sourceCRSIn,
    const crs::CRSNNPtr &targetCRSIn, const common::Angle &offsetLat,
    const common::Angle &offsetLon, const common::Length &offsetHeight,
    const std::vector<metadata::PositionalAccuracyNNPtr> &accuracies) {
    return create(
        properties, sourceCRSIn, targetCRSIn, nullptr,
        createMethodMapNameEPSGCode(
            EPSG_CODE_METHOD_GEOGRAPHIC2D_WITH_HEIGHT_OFFSETS),
        VectorOfParameters{
            createOpParamNameEPSGCode(EPSG_CODE_PARAMETER_LATITUDE_OFFSET),
            createOpParamNameEPSGCode(EPSG_CODE_PARAMETER_LONGITUDE_OFFSET),
            createOpParamNameEPSGCode(EPSG_CODE_PARAMETER_GEOID_UNDULATION)},
        VectorOfValues{offsetLat, offsetLon, offsetHeight}, accuracies);
}

// ---------------------------------------------------------------------------

/** \brief Instantiate a transformation with method Vertical Offset.
 *
 * This method is defined as [EPSG:9616]
 * (https://www.epsg-registry.org/export.htm?gml=urn:ogc:def:method:EPSG::9616)
 * *
 * @param properties See \ref general_properties of the Transformation.
 * At minimum the name should be defined.
 * @param sourceCRSIn Source CRS.
 * @param targetCRSIn Target CRS.
 * @param offsetHeight Geoid undulation to add.
 * @param accuracies Vector of positional accuracy (might be empty).
 * @return new Transformation.
 */
TransformationNNPtr Transformation::createVerticalOffset(
    const util::PropertyMap &properties, const crs::CRSNNPtr &sourceCRSIn,
    const crs::CRSNNPtr &targetCRSIn, const common::Length &offsetHeight,
    const std::vector<metadata::PositionalAccuracyNNPtr> &accuracies) {
    return create(properties, sourceCRSIn, targetCRSIn, nullptr,
                  createMethodMapNameEPSGCode(EPSG_CODE_METHOD_VERTICAL_OFFSET),
                  VectorOfParameters{createOpParamNameEPSGCode(
                      EPSG_CODE_PARAMETER_VERTICAL_OFFSET)},
                  VectorOfValues{offsetHeight}, accuracies);
}

// ---------------------------------------------------------------------------

/** \brief Instantiate a transformation based on the Change of Vertical Unit
 * method.
 *
 * This method is defined as [EPSG:1069]
 * (https://www.epsg-registry.org/export.htm?gml=urn:ogc:def:method:EPSG::1069)
 *
 * @param properties See \ref general_properties of the conversion. If the name
 * is not provided, it is automatically set.
 * @param sourceCRSIn Source CRS.
 * @param targetCRSIn Target CRS.
 * @param factor Conversion factor
 * @param accuracies Vector of positional accuracy (might be empty).
 * @return a new Transformation.
 */
TransformationNNPtr Transformation::createChangeVerticalUnit(
    const util::PropertyMap &properties, const crs::CRSNNPtr &sourceCRSIn,
    const crs::CRSNNPtr &targetCRSIn, const common::Scale &factor,
    const std::vector<metadata::PositionalAccuracyNNPtr> &accuracies) {
    return create(
        properties, sourceCRSIn, targetCRSIn, nullptr,
        createMethodMapNameEPSGCode(EPSG_CODE_METHOD_CHANGE_VERTICAL_UNIT),
        VectorOfParameters{
            createOpParamNameEPSGCode(
                EPSG_CODE_PARAMETER_UNIT_CONVERSION_SCALAR),
        },
        VectorOfValues{
            factor,
        },
        accuracies);
}

// ---------------------------------------------------------------------------

static util::PropertyMap
createPropertiesForInverse(const CoordinateOperation *op, bool derivedFrom,
                           bool approximateInversion) {
    assert(op);
    util::PropertyMap map;

    // The domain(s) are unchanged by the inverse operation
    addDomains(map, op);

    const std::string &forwardName = op->nameStr();

    // Forge a name for the inverse, either from the forward name, or
    // from the source and target CRS names
    const char *opType;
    if (starts_with(forwardName, BALLPARK_GEOCENTRIC_TRANSLATION)) {
        opType = BALLPARK_GEOCENTRIC_TRANSLATION;
    } else if (starts_with(forwardName, BALLPARK_GEOGRAPHIC_OFFSET)) {
        opType = BALLPARK_GEOGRAPHIC_OFFSET;
    } else if (starts_with(forwardName, NULL_GEOGRAPHIC_OFFSET)) {
        opType = NULL_GEOGRAPHIC_OFFSET;
    } else if (starts_with(forwardName, NULL_GEOCENTRIC_TRANSLATION)) {
        opType = NULL_GEOCENTRIC_TRANSLATION;
    } else if (dynamic_cast<const Transformation *>(op) ||
               starts_with(forwardName, "Transformation from ")) {
        opType = "Transformation";
    } else if (dynamic_cast<const Conversion *>(op)) {
        opType = "Conversion";
    } else {
        opType = "Operation";
    }

    auto sourceCRS = op->sourceCRS();
    auto targetCRS = op->targetCRS();
    std::string name;
    if (!forwardName.empty()) {
        if (dynamic_cast<const Transformation *>(op) == nullptr &&
            dynamic_cast<const ConcatenatedOperation *>(op) == nullptr &&
            (starts_with(forwardName, INVERSE_OF) ||
             forwardName.find(" + ") != std::string::npos)) {
            std::vector<std::string> tokens;
            std::string curToken;
            bool inString = false;
            for (size_t i = 0; i < forwardName.size(); ++i) {
                if (inString) {
                    curToken += forwardName[i];
                    if (forwardName[i] == '\'') {
                        inString = false;
                    }
                } else if (i + 3 < forwardName.size() &&
                           memcmp(&forwardName[i], " + ", 3) == 0) {
                    tokens.push_back(curToken);
                    curToken.clear();
                    i += 2;
                } else if (forwardName[i] == '\'') {
                    inString = true;
                    curToken += forwardName[i];
                } else {
                    curToken += forwardName[i];
                }
            }
            if (!curToken.empty()) {
                tokens.push_back(curToken);
            }
            for (size_t i = tokens.size(); i > 0;) {
                i--;
                if (!name.empty()) {
                    name += " + ";
                }
                if (starts_with(tokens[i], INVERSE_OF)) {
                    name += tokens[i].substr(INVERSE_OF.size());
                } else if (tokens[i] == AXIS_ORDER_CHANGE_2D_NAME ||
                           tokens[i] == AXIS_ORDER_CHANGE_3D_NAME) {
                    name += tokens[i];
                } else {
                    name += INVERSE_OF + tokens[i];
                }
            }
        } else if (!sourceCRS || !targetCRS ||
                   forwardName != buildOpName(opType, sourceCRS, targetCRS)) {
            if (forwardName.find(" + ") != std::string::npos) {
                name = INVERSE_OF + '\'' + forwardName + '\'';
            } else {
                name = INVERSE_OF + forwardName;
            }
        }
    }
    if (name.empty() && sourceCRS && targetCRS) {
        name = buildOpName(opType, targetCRS, sourceCRS);
    }
    if (approximateInversion) {
        name += " (approx. inversion)";
    }

    if (!name.empty()) {
        map.set(common::IdentifiedObject::NAME_KEY, name);
    }

    const std::string &remarks = op->remarks();
    if (!remarks.empty()) {
        map.set(common::IdentifiedObject::REMARKS_KEY, remarks);
    }

    addModifiedIdentifier(map, op, true, derivedFrom);

    const auto so = dynamic_cast<const SingleOperation *>(op);
    if (so) {
        const int soMethodEPSGCode = so->method()->getEPSGCode();
        if (soMethodEPSGCode > 0) {
            map.set("OPERATION_METHOD_EPSG_CODE", soMethodEPSGCode);
        }
    }

    return map;
}

// ---------------------------------------------------------------------------

static bool isTimeDependent(const std::string &methodName) {
    return ci_find(methodName, "Time dependent") != std::string::npos ||
           ci_find(methodName, "Time-dependent") != std::string::npos;
}

// ---------------------------------------------------------------------------

// to avoid -0...
static double negate(double val) {
    if (val != 0) {
        return -val;
    }
    return 0.0;
}

// ---------------------------------------------------------------------------

static CoordinateOperationPtr
createApproximateInverseIfPossible(const Transformation *op) {
    bool sevenParamsTransform = false;
    bool fifteenParamsTransform = false;
    const auto &method = op->method();
    const auto &methodName = method->nameStr();
    const int methodEPSGCode = method->getEPSGCode();
    const auto paramCount = op->parameterValues().size();
    const bool isPositionVector =
        ci_find(methodName, "Position Vector") != std::string::npos;
    const bool isCoordinateFrame =
        ci_find(methodName, "Coordinate Frame") != std::string::npos;

    // See end of "2.4.3.3 Helmert 7-parameter transformations"
    // in EPSG 7-2 guidance
    // For practical purposes, the inverse of 7- or 15-parameters Helmert
    // can be obtained by using the forward method with all parameters
    // negated
    // (except reference epoch!)
    // So for WKT export use that. But for PROJ string, we use the +inv flag
    // so as to get "perfect" round-tripability.
    if ((paramCount == 7 && isCoordinateFrame &&
         !isTimeDependent(methodName)) ||
        methodEPSGCode == EPSG_CODE_METHOD_COORDINATE_FRAME_GEOCENTRIC ||
        methodEPSGCode == EPSG_CODE_METHOD_COORDINATE_FRAME_GEOGRAPHIC_2D ||
        methodEPSGCode == EPSG_CODE_METHOD_COORDINATE_FRAME_GEOGRAPHIC_3D) {
        sevenParamsTransform = true;
    } else if (
        (paramCount == 15 && isCoordinateFrame &&
         isTimeDependent(methodName)) ||
        methodEPSGCode ==
            EPSG_CODE_METHOD_TIME_DEPENDENT_COORDINATE_FRAME_GEOCENTRIC ||
        methodEPSGCode ==
            EPSG_CODE_METHOD_TIME_DEPENDENT_COORDINATE_FRAME_GEOGRAPHIC_2D ||
        methodEPSGCode ==
            EPSG_CODE_METHOD_TIME_DEPENDENT_COORDINATE_FRAME_GEOGRAPHIC_3D) {
        fifteenParamsTransform = true;
    } else if ((paramCount == 7 && isPositionVector &&
                !isTimeDependent(methodName)) ||
               methodEPSGCode == EPSG_CODE_METHOD_POSITION_VECTOR_GEOCENTRIC ||
               methodEPSGCode ==
                   EPSG_CODE_METHOD_POSITION_VECTOR_GEOGRAPHIC_2D ||
               methodEPSGCode ==
                   EPSG_CODE_METHOD_POSITION_VECTOR_GEOGRAPHIC_3D) {
        sevenParamsTransform = true;
    } else if (
        (paramCount == 15 && isPositionVector && isTimeDependent(methodName)) ||
        methodEPSGCode ==
            EPSG_CODE_METHOD_TIME_DEPENDENT_POSITION_VECTOR_GEOCENTRIC ||
        methodEPSGCode ==
            EPSG_CODE_METHOD_TIME_DEPENDENT_POSITION_VECTOR_GEOGRAPHIC_2D ||
        methodEPSGCode ==
            EPSG_CODE_METHOD_TIME_DEPENDENT_POSITION_VECTOR_GEOGRAPHIC_3D) {
        fifteenParamsTransform = true;
    }
    if (sevenParamsTransform || fifteenParamsTransform) {
        double neg_x = negate(op->parameterValueNumericAsSI(
            EPSG_CODE_PARAMETER_X_AXIS_TRANSLATION));
        double neg_y = negate(op->parameterValueNumericAsSI(
            EPSG_CODE_PARAMETER_Y_AXIS_TRANSLATION));
        double neg_z = negate(op->parameterValueNumericAsSI(
            EPSG_CODE_PARAMETER_Z_AXIS_TRANSLATION));
        double neg_rx = negate(
            op->parameterValueNumeric(EPSG_CODE_PARAMETER_X_AXIS_ROTATION,
                                      common::UnitOfMeasure::ARC_SECOND));
        double neg_ry = negate(
            op->parameterValueNumeric(EPSG_CODE_PARAMETER_Y_AXIS_ROTATION,
                                      common::UnitOfMeasure::ARC_SECOND));
        double neg_rz = negate(
            op->parameterValueNumeric(EPSG_CODE_PARAMETER_Z_AXIS_ROTATION,
                                      common::UnitOfMeasure::ARC_SECOND));
        double neg_scaleDiff = negate(op->parameterValueNumeric(
            EPSG_CODE_PARAMETER_SCALE_DIFFERENCE,
            common::UnitOfMeasure::PARTS_PER_MILLION));
        auto methodProperties = util::PropertyMap().set(
            common::IdentifiedObject::NAME_KEY, methodName);
        int method_epsg_code = method->getEPSGCode();
        if (method_epsg_code) {
            methodProperties
                .set(metadata::Identifier::CODESPACE_KEY,
                     metadata::Identifier::EPSG)
                .set(metadata::Identifier::CODE_KEY, method_epsg_code);
        }
        if (fifteenParamsTransform) {
            double neg_rate_x = negate(op->parameterValueNumeric(
                EPSG_CODE_PARAMETER_RATE_X_AXIS_TRANSLATION,
                common::UnitOfMeasure::METRE_PER_YEAR));
            double neg_rate_y = negate(op->parameterValueNumeric(
                EPSG_CODE_PARAMETER_RATE_Y_AXIS_TRANSLATION,
                common::UnitOfMeasure::METRE_PER_YEAR));
            double neg_rate_z = negate(op->parameterValueNumeric(
                EPSG_CODE_PARAMETER_RATE_Z_AXIS_TRANSLATION,
                common::UnitOfMeasure::METRE_PER_YEAR));
            double neg_rate_rx = negate(op->parameterValueNumeric(
                EPSG_CODE_PARAMETER_RATE_X_AXIS_ROTATION,
                common::UnitOfMeasure::ARC_SECOND_PER_YEAR));
            double neg_rate_ry = negate(op->parameterValueNumeric(
                EPSG_CODE_PARAMETER_RATE_Y_AXIS_ROTATION,
                common::UnitOfMeasure::ARC_SECOND_PER_YEAR));
            double neg_rate_rz = negate(op->parameterValueNumeric(
                EPSG_CODE_PARAMETER_RATE_Z_AXIS_ROTATION,
                common::UnitOfMeasure::ARC_SECOND_PER_YEAR));
            double neg_rate_scaleDiff = negate(op->parameterValueNumeric(
                EPSG_CODE_PARAMETER_RATE_SCALE_DIFFERENCE,
                common::UnitOfMeasure::PPM_PER_YEAR));
            double referenceEpochYear =
                op->parameterValueNumeric(EPSG_CODE_PARAMETER_REFERENCE_EPOCH,
                                          common::UnitOfMeasure::YEAR);
            return util::nn_static_pointer_cast<CoordinateOperation>(
                       createFifteenParamsTransform(
                           createPropertiesForInverse(op, false, true),
                           methodProperties, op->targetCRS(), op->sourceCRS(),
                           neg_x, neg_y, neg_z, neg_rx, neg_ry, neg_rz,
                           neg_scaleDiff, neg_rate_x, neg_rate_y, neg_rate_z,
                           neg_rate_rx, neg_rate_ry, neg_rate_rz,
                           neg_rate_scaleDiff, referenceEpochYear,
                           op->coordinateOperationAccuracies()))
                .as_nullable();
        } else {
            return util::nn_static_pointer_cast<CoordinateOperation>(
                       createSevenParamsTransform(
                           createPropertiesForInverse(op, false, true),
                           methodProperties, op->targetCRS(), op->sourceCRS(),
                           neg_x, neg_y, neg_z, neg_rx, neg_ry, neg_rz,
                           neg_scaleDiff, op->coordinateOperationAccuracies()))
                .as_nullable();
        }
    }

    return nullptr;
}
//! @endcond

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
TransformationNNPtr
Transformation::Private::registerInv(const Transformation *thisIn,
                                     TransformationNNPtr invTransform) {
    invTransform->d->forwardOperation_ = thisIn->shallowClone().as_nullable();
    invTransform->setHasBallparkTransformation(
        thisIn->hasBallparkTransformation());
    return invTransform;
}
//! @endcond

// ---------------------------------------------------------------------------

CoordinateOperationNNPtr Transformation::inverse() const {
    return inverseAsTransformation();
}

// ---------------------------------------------------------------------------

TransformationNNPtr Transformation::inverseAsTransformation() const {

    if (d->forwardOperation_) {
        return NN_NO_CHECK(d->forwardOperation_);
    }
    const auto &l_method = method();
    const auto &methodName = l_method->nameStr();
    const int methodEPSGCode = l_method->getEPSGCode();
    const auto &l_sourceCRS = sourceCRS();
    const auto &l_targetCRS = targetCRS();

    // For geocentric translation, the inverse is exactly the negation of
    // the parameters.
    if (ci_find(methodName, "Geocentric translations") != std::string::npos ||
        methodEPSGCode == EPSG_CODE_METHOD_GEOCENTRIC_TRANSLATION_GEOCENTRIC ||
        methodEPSGCode ==
            EPSG_CODE_METHOD_GEOCENTRIC_TRANSLATION_GEOGRAPHIC_2D ||
        methodEPSGCode ==
            EPSG_CODE_METHOD_GEOCENTRIC_TRANSLATION_GEOGRAPHIC_3D) {
        double x =
            parameterValueNumericAsSI(EPSG_CODE_PARAMETER_X_AXIS_TRANSLATION);
        double y =
            parameterValueNumericAsSI(EPSG_CODE_PARAMETER_Y_AXIS_TRANSLATION);
        double z =
            parameterValueNumericAsSI(EPSG_CODE_PARAMETER_Z_AXIS_TRANSLATION);
        auto properties = createPropertiesForInverse(this, false, false);
        return Private::registerInv(
            this, create(properties, l_targetCRS, l_sourceCRS, nullptr,
                         createMethodMapNameEPSGCode(
                             useOperationMethodEPSGCodeIfPresent(
                                 properties, methodEPSGCode)),
                         VectorOfParameters{
                             createOpParamNameEPSGCode(
                                 EPSG_CODE_PARAMETER_X_AXIS_TRANSLATION),
                             createOpParamNameEPSGCode(
                                 EPSG_CODE_PARAMETER_Y_AXIS_TRANSLATION),
                             createOpParamNameEPSGCode(
                                 EPSG_CODE_PARAMETER_Z_AXIS_TRANSLATION),
                         },
                         createParams(common::Length(negate(x)),
                                      common::Length(negate(y)),
                                      common::Length(negate(z))),
                         coordinateOperationAccuracies()));
    }

    if (methodEPSGCode == EPSG_CODE_METHOD_MOLODENSKY ||
        methodEPSGCode == EPSG_CODE_METHOD_ABRIDGED_MOLODENSKY) {
        double x =
            parameterValueNumericAsSI(EPSG_CODE_PARAMETER_X_AXIS_TRANSLATION);
        double y =
            parameterValueNumericAsSI(EPSG_CODE_PARAMETER_Y_AXIS_TRANSLATION);
        double z =
            parameterValueNumericAsSI(EPSG_CODE_PARAMETER_Z_AXIS_TRANSLATION);
        double da = parameterValueNumericAsSI(
            EPSG_CODE_PARAMETER_SEMI_MAJOR_AXIS_DIFFERENCE);
        double df = parameterValueNumericAsSI(
            EPSG_CODE_PARAMETER_FLATTENING_DIFFERENCE);

        if (methodEPSGCode == EPSG_CODE_METHOD_ABRIDGED_MOLODENSKY) {
            return Private::registerInv(
                this,
                createAbridgedMolodensky(
                    createPropertiesForInverse(this, false, false), l_targetCRS,
                    l_sourceCRS, negate(x), negate(y), negate(z), negate(da),
                    negate(df), coordinateOperationAccuracies()));
        } else {
            return Private::registerInv(
                this,
                createMolodensky(createPropertiesForInverse(this, false, false),
                                 l_targetCRS, l_sourceCRS, negate(x), negate(y),
                                 negate(z), negate(da), negate(df),
                                 coordinateOperationAccuracies()));
        }
    }

    if (isLongitudeRotation()) {
        auto offset =
            parameterValueMeasure(EPSG_CODE_PARAMETER_LONGITUDE_OFFSET);
        const common::Angle newOffset(negate(offset.value()), offset.unit());
        return Private::registerInv(
            this, createLongitudeRotation(
                      createPropertiesForInverse(this, false, false),
                      l_targetCRS, l_sourceCRS, newOffset));
    }

    if (methodEPSGCode == EPSG_CODE_METHOD_GEOGRAPHIC2D_OFFSETS) {
        auto offsetLat =
            parameterValueMeasure(EPSG_CODE_PARAMETER_LATITUDE_OFFSET);
        const common::Angle newOffsetLat(negate(offsetLat.value()),
                                         offsetLat.unit());

        auto offsetLong =
            parameterValueMeasure(EPSG_CODE_PARAMETER_LONGITUDE_OFFSET);
        const common::Angle newOffsetLong(negate(offsetLong.value()),
                                          offsetLong.unit());

        return Private::registerInv(
            this, createGeographic2DOffsets(
                      createPropertiesForInverse(this, false, false),
                      l_targetCRS, l_sourceCRS, newOffsetLat, newOffsetLong,
                      coordinateOperationAccuracies()));
    }

    if (methodEPSGCode == EPSG_CODE_METHOD_GEOGRAPHIC3D_OFFSETS) {
        auto offsetLat =
            parameterValueMeasure(EPSG_CODE_PARAMETER_LATITUDE_OFFSET);
        const common::Angle newOffsetLat(negate(offsetLat.value()),
                                         offsetLat.unit());

        auto offsetLong =
            parameterValueMeasure(EPSG_CODE_PARAMETER_LONGITUDE_OFFSET);
        const common::Angle newOffsetLong(negate(offsetLong.value()),
                                          offsetLong.unit());

        auto offsetHeight =
            parameterValueMeasure(EPSG_CODE_PARAMETER_VERTICAL_OFFSET);
        const common::Length newOffsetHeight(negate(offsetHeight.value()),
                                             offsetHeight.unit());

        return Private::registerInv(
            this, createGeographic3DOffsets(
                      createPropertiesForInverse(this, false, false),
                      l_targetCRS, l_sourceCRS, newOffsetLat, newOffsetLong,
                      newOffsetHeight, coordinateOperationAccuracies()));
    }

    if (methodEPSGCode == EPSG_CODE_METHOD_GEOGRAPHIC2D_WITH_HEIGHT_OFFSETS) {
        auto offsetLat =
            parameterValueMeasure(EPSG_CODE_PARAMETER_LATITUDE_OFFSET);
        const common::Angle newOffsetLat(negate(offsetLat.value()),
                                         offsetLat.unit());

        auto offsetLong =
            parameterValueMeasure(EPSG_CODE_PARAMETER_LONGITUDE_OFFSET);
        const common::Angle newOffsetLong(negate(offsetLong.value()),
                                          offsetLong.unit());

        auto offsetHeight =
            parameterValueMeasure(EPSG_CODE_PARAMETER_GEOID_UNDULATION);
        const common::Length newOffsetHeight(negate(offsetHeight.value()),
                                             offsetHeight.unit());

        return Private::registerInv(
            this, createGeographic2DWithHeightOffsets(
                      createPropertiesForInverse(this, false, false),
                      l_targetCRS, l_sourceCRS, newOffsetLat, newOffsetLong,
                      newOffsetHeight, coordinateOperationAccuracies()));
    }

    if (methodEPSGCode == EPSG_CODE_METHOD_VERTICAL_OFFSET) {

        auto offsetHeight =
            parameterValueMeasure(EPSG_CODE_PARAMETER_VERTICAL_OFFSET);
        const common::Length newOffsetHeight(negate(offsetHeight.value()),
                                             offsetHeight.unit());

        return Private::registerInv(
            this,
            createVerticalOffset(createPropertiesForInverse(this, false, false),
                                 l_targetCRS, l_sourceCRS, newOffsetHeight,
                                 coordinateOperationAccuracies()));
    }

    if (methodEPSGCode == EPSG_CODE_METHOD_CHANGE_VERTICAL_UNIT) {
        const double convFactor = parameterValueNumericAsSI(
            EPSG_CODE_PARAMETER_UNIT_CONVERSION_SCALAR);
        return Private::registerInv(
            this, createChangeVerticalUnit(
                      createPropertiesForInverse(this, false, false),
                      l_targetCRS, l_sourceCRS, common::Scale(1.0 / convFactor),
                      coordinateOperationAccuracies()));
    }

#ifdef notdef
    // We don't need that currently, but we might...
    if (methodEPSGCode == EPSG_CODE_METHOD_HEIGHT_DEPTH_REVERSAL) {
        return Private::registerInv(
            this,
            createHeightDepthReversal(
                createPropertiesForInverse(this, false, false), l_targetCRS,
                l_sourceCRS, coordinateOperationAccuracies()));
    }
#endif

    return InverseTransformation::create(NN_NO_CHECK(
        util::nn_dynamic_pointer_cast<Transformation>(shared_from_this())));
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress

// ---------------------------------------------------------------------------

InverseTransformation::InverseTransformation(const TransformationNNPtr &forward)
    : Transformation(
          forward->targetCRS(), forward->sourceCRS(),
          forward->interpolationCRS(),
          OperationMethod::create(createPropertiesForInverse(forward->method()),
                                  forward->method()->parameters()),
          forward->parameterValues(), forward->coordinateOperationAccuracies()),
      InverseCoordinateOperation(forward, true) {
    setPropertiesFromForward();
}

// ---------------------------------------------------------------------------

InverseTransformation::~InverseTransformation() = default;

// ---------------------------------------------------------------------------

TransformationNNPtr
InverseTransformation::create(const TransformationNNPtr &forward) {
    auto conv = util::nn_make_shared<InverseTransformation>(forward);
    conv->assignSelf(conv);
    return conv;
}

// ---------------------------------------------------------------------------

TransformationNNPtr InverseTransformation::inverseAsTransformation() const {
    return NN_NO_CHECK(
        util::nn_dynamic_pointer_cast<Transformation>(forwardOperation_));
}

// ---------------------------------------------------------------------------

void InverseTransformation::_exportToWKT(io::WKTFormatter *formatter) const {

    auto approxInverse = createApproximateInverseIfPossible(
        util::nn_dynamic_pointer_cast<Transformation>(forwardOperation_).get());
    if (approxInverse) {
        approxInverse->_exportToWKT(formatter);
    } else {
        Transformation::_exportToWKT(formatter);
    }
}

// ---------------------------------------------------------------------------

CoordinateOperationNNPtr InverseTransformation::_shallowClone() const {
    auto op = InverseTransformation::nn_make_shared<InverseTransformation>(
        inverseAsTransformation()->shallowClone());
    op->assignSelf(op);
    op->setCRSs(this, false);
    return util::nn_static_pointer_cast<CoordinateOperation>(op);
}

//! @endcond

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
void Transformation::_exportToWKT(io::WKTFormatter *formatter) const {
    exportTransformationToWKT(formatter);
}
//! @endcond

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
void Transformation::_exportToJSON(
    io::JSONFormatter *formatter) const // throw(FormattingException)
{
    auto writer = formatter->writer();
    auto objectContext(formatter->MakeObjectContext(
        formatter->abridgedTransformation() ? "AbridgedTransformation"
                                            : "Transformation",
        !identifiers().empty()));

    writer->AddObjKey("name");
    auto l_name = nameStr();
    if (l_name.empty()) {
        writer->Add("unnamed");
    } else {
        writer->Add(l_name);
    }

    if (!formatter->abridgedTransformation()) {
        writer->AddObjKey("source_crs");
        formatter->setAllowIDInImmediateChild();
        sourceCRS()->_exportToJSON(formatter);

        writer->AddObjKey("target_crs");
        formatter->setAllowIDInImmediateChild();
        targetCRS()->_exportToJSON(formatter);

        const auto &l_interpolationCRS = interpolationCRS();
        if (l_interpolationCRS) {
            writer->AddObjKey("interpolation_crs");
            formatter->setAllowIDInImmediateChild();
            l_interpolationCRS->_exportToJSON(formatter);
        }
    }

    writer->AddObjKey("method");
    formatter->setOmitTypeInImmediateChild();
    formatter->setAllowIDInImmediateChild();
    method()->_exportToJSON(formatter);

    writer->AddObjKey("parameters");
    {
        auto parametersContext(writer->MakeArrayContext(false));
        for (const auto &genOpParamvalue : parameterValues()) {
            formatter->setAllowIDInImmediateChild();
            formatter->setOmitTypeInImmediateChild();
            genOpParamvalue->_exportToJSON(formatter);
        }
    }

    if (!formatter->abridgedTransformation()) {
        if (!coordinateOperationAccuracies().empty()) {
            writer->AddObjKey("accuracy");
            writer->Add(coordinateOperationAccuracies()[0]->value());
        }
    }

    if (formatter->abridgedTransformation()) {
        if (formatter->outputId()) {
            formatID(formatter);
        }
    } else {
        ObjectUsage::baseExportToJSON(formatter);
    }
}

//! @endcond

// ---------------------------------------------------------------------------

static void exportSourceCRSAndTargetCRSToWKT(const CoordinateOperation *co,
                                             io::WKTFormatter *formatter) {
    auto l_sourceCRS = co->sourceCRS();
    assert(l_sourceCRS);
    auto l_targetCRS = co->targetCRS();
    assert(l_targetCRS);
    const bool isWKT2 = formatter->version() == io::WKTFormatter::Version::WKT2;
    const bool canExportCRSId =
        (isWKT2 && formatter->use2019Keywords() &&
         !(formatter->idOnTopLevelOnly() && formatter->topLevelHasId()));

    const bool hasDomains = !co->domains().empty();
    if (hasDomains) {
        formatter->pushDisableUsage();
    }

    formatter->startNode(io::WKTConstants::SOURCECRS, false);
    if (canExportCRSId && !l_sourceCRS->identifiers().empty()) {
        // fake that top node has no id, so that the sourceCRS id is
        // considered
        formatter->pushHasId(false);
        l_sourceCRS->_exportToWKT(formatter);
        formatter->popHasId();
    } else {
        l_sourceCRS->_exportToWKT(formatter);
    }
    formatter->endNode();

    formatter->startNode(io::WKTConstants::TARGETCRS, false);
    if (canExportCRSId && !l_targetCRS->identifiers().empty()) {
        // fake that top node has no id, so that the targetCRS id is
        // considered
        formatter->pushHasId(false);
        l_targetCRS->_exportToWKT(formatter);
        formatter->popHasId();
    } else {
        l_targetCRS->_exportToWKT(formatter);
    }
    formatter->endNode();

    if (hasDomains) {
        formatter->popDisableUsage();
    }
}

// ---------------------------------------------------------------------------

void SingleOperation::exportTransformationToWKT(
    io::WKTFormatter *formatter) const {
    const bool isWKT2 = formatter->version() == io::WKTFormatter::Version::WKT2;
    if (!isWKT2) {
        throw io::FormattingException(
            "Transformation can only be exported to WKT2");
    }

    if (formatter->abridgedTransformation()) {
        formatter->startNode(io::WKTConstants::ABRIDGEDTRANSFORMATION,
                             !identifiers().empty());
    } else {
        formatter->startNode(io::WKTConstants::COORDINATEOPERATION,
                             !identifiers().empty());
    }

    formatter->addQuotedString(nameStr());

    if (formatter->use2019Keywords()) {
        const auto &version = operationVersion();
        if (version.has_value()) {
            formatter->startNode(io::WKTConstants::VERSION, false);
            formatter->addQuotedString(*version);
            formatter->endNode();
        }
    }

    if (!formatter->abridgedTransformation()) {
        exportSourceCRSAndTargetCRSToWKT(this, formatter);
    }

    method()->_exportToWKT(formatter);

    for (const auto &paramValue : parameterValues()) {
        paramValue->_exportToWKT(formatter, nullptr);
    }

    if (!formatter->abridgedTransformation()) {
        if (interpolationCRS()) {
            formatter->startNode(io::WKTConstants::INTERPOLATIONCRS, false);
            interpolationCRS()->_exportToWKT(formatter);
            formatter->endNode();
        }

        if (!coordinateOperationAccuracies().empty()) {
            formatter->startNode(io::WKTConstants::OPERATIONACCURACY, false);
            formatter->add(coordinateOperationAccuracies()[0]->value());
            formatter->endNode();
        }
    }

    ObjectUsage::baseExportToWKT(formatter);
    formatter->endNode();
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
static const std::string nullString;

static const std::string &_getNTv2Filename(const Transformation *op,
                                           bool allowInverse) {

    const auto &l_method = op->method();
    if (l_method->getEPSGCode() == EPSG_CODE_METHOD_NTV2 ||
        (allowInverse &&
         ci_equal(l_method->nameStr(), INVERSE_OF + EPSG_NAME_METHOD_NTV2))) {
        const auto &fileParameter = op->parameterValue(
            EPSG_NAME_PARAMETER_LATITUDE_LONGITUDE_DIFFERENCE_FILE,
            EPSG_CODE_PARAMETER_LATITUDE_LONGITUDE_DIFFERENCE_FILE);
        if (fileParameter &&
            fileParameter->type() == ParameterValue::Type::FILENAME) {
            return fileParameter->valueFile();
        }
    }
    return nullString;
}
//! @endcond

// ---------------------------------------------------------------------------
//! @cond Doxygen_Suppress
const std::string &Transformation::getNTv2Filename() const {

    return _getNTv2Filename(this, false);
}
//! @endcond

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
static const std::string &_getNTv1Filename(const Transformation *op,
                                           bool allowInverse) {

    const auto &l_method = op->method();
    const auto &methodName = l_method->nameStr();
    if (l_method->getEPSGCode() == EPSG_CODE_METHOD_NTV1 ||
        (allowInverse &&
         ci_equal(methodName, INVERSE_OF + EPSG_NAME_METHOD_NTV1))) {
        const auto &fileParameter = op->parameterValue(
            EPSG_NAME_PARAMETER_LATITUDE_LONGITUDE_DIFFERENCE_FILE,
            EPSG_CODE_PARAMETER_LATITUDE_LONGITUDE_DIFFERENCE_FILE);
        if (fileParameter &&
            fileParameter->type() == ParameterValue::Type::FILENAME) {
            return fileParameter->valueFile();
        }
    }
    return nullString;
}
//! @endcond

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
static const std::string &_getCTABLE2Filename(const Transformation *op,
                                              bool allowInverse) {
    const auto &l_method = op->method();
    const auto &methodName = l_method->nameStr();
    if (ci_equal(methodName, PROJ_WKT2_NAME_METHOD_CTABLE2) ||
        (allowInverse &&
         ci_equal(methodName, INVERSE_OF + PROJ_WKT2_NAME_METHOD_CTABLE2))) {
        const auto &fileParameter = op->parameterValue(
            EPSG_NAME_PARAMETER_LATITUDE_LONGITUDE_DIFFERENCE_FILE,
            EPSG_CODE_PARAMETER_LATITUDE_LONGITUDE_DIFFERENCE_FILE);
        if (fileParameter &&
            fileParameter->type() == ParameterValue::Type::FILENAME) {
            return fileParameter->valueFile();
        }
    }
    return nullString;
}
//! @endcond

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
static const std::string &
_getHorizontalShiftGTIFFFilename(const Transformation *op, bool allowInverse) {
    const auto &l_method = op->method();
    const auto &methodName = l_method->nameStr();
    if (ci_equal(methodName, PROJ_WKT2_NAME_METHOD_HORIZONTAL_SHIFT_GTIFF) ||
        (allowInverse &&
         ci_equal(methodName,
                  INVERSE_OF + PROJ_WKT2_NAME_METHOD_HORIZONTAL_SHIFT_GTIFF))) {
        const auto &fileParameter = op->parameterValue(
            EPSG_NAME_PARAMETER_LATITUDE_LONGITUDE_DIFFERENCE_FILE,
            EPSG_CODE_PARAMETER_LATITUDE_LONGITUDE_DIFFERENCE_FILE);
        if (fileParameter &&
            fileParameter->type() == ParameterValue::Type::FILENAME) {
            return fileParameter->valueFile();
        }
    }
    return nullString;
}
//! @endcond

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
static const std::string &
_getGeocentricTranslationFilename(const Transformation *op, bool allowInverse) {

    const auto &l_method = op->method();
    const auto &methodName = l_method->nameStr();
    if (l_method->getEPSGCode() ==
            EPSG_CODE_METHOD_GEOCENTRIC_TRANSLATION_BY_GRID_INTERPOLATION_IGN ||
        (allowInverse &&
         ci_equal(
             methodName,
             INVERSE_OF +
                 EPSG_NAME_METHOD_GEOCENTRIC_TRANSLATION_BY_GRID_INTERPOLATION_IGN))) {
        const auto &fileParameter =
            op->parameterValue(EPSG_NAME_PARAMETER_GEOCENTRIC_TRANSLATION_FILE,
                               EPSG_CODE_PARAMETER_GEOCENTRIC_TRANSLATION_FILE);
        if (fileParameter &&
            fileParameter->type() == ParameterValue::Type::FILENAME) {
            return fileParameter->valueFile();
        }
    }
    return nullString;
}
//! @endcond

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
static const std::string &
_getHeightToGeographic3DFilename(const Transformation *op, bool allowInverse) {

    const auto &methodName = op->method()->nameStr();

    if (ci_equal(methodName, PROJ_WKT2_NAME_METHOD_HEIGHT_TO_GEOG3D) ||
        (allowInverse &&
         ci_equal(methodName,
                  INVERSE_OF + PROJ_WKT2_NAME_METHOD_HEIGHT_TO_GEOG3D))) {
        const auto &fileParameter =
            op->parameterValue(EPSG_NAME_PARAMETER_GEOID_CORRECTION_FILENAME,
                               EPSG_CODE_PARAMETER_GEOID_CORRECTION_FILENAME);
        if (fileParameter &&
            fileParameter->type() == ParameterValue::Type::FILENAME) {
            return fileParameter->valueFile();
        }
    }
    return nullString;
}
//! @endcond

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
static bool
isGeographic3DToGravityRelatedHeight(const OperationMethodNNPtr &method,
                                     bool allowInverse) {
    const auto &methodName = method->nameStr();
    static const char *const methodCodes[] = {
        "1025", // Geographic3D to GravityRelatedHeight (EGM2008)
        "1030", // Geographic3D to GravityRelatedHeight (NZgeoid)
        "1045", // Geographic3D to GravityRelatedHeight (OSGM02-Ire)
        "1047", // Geographic3D to GravityRelatedHeight (Gravsoft)
        "1048", // Geographic3D to GravityRelatedHeight (Ausgeoid v2)
        "1050", // Geographic3D to GravityRelatedHeight (CI)
        "1059", // Geographic3D to GravityRelatedHeight (PNG)
        "1060", // Geographic3D to GravityRelatedHeight (CGG2013)
        "1072", // Geographic3D to GravityRelatedHeight (OSGM15-Ire)
        "1073", // Geographic3D to GravityRelatedHeight (IGN2009)
        "1081", // Geographic3D to GravityRelatedHeight (BEV AT)
        "1083", // Geog3D to Geog2D+Vertical (AUSGeoid v2)
        "9661", // Geographic3D to GravityRelatedHeight (EGM)
        "9662", // Geographic3D to GravityRelatedHeight (Ausgeoid98)
        "9663", // Geographic3D to GravityRelatedHeight (OSGM-GB)
        "9664", // Geographic3D to GravityRelatedHeight (IGN1997)
        "9665", // Geographic3D to GravityRelatedHeight (US .gtx)
        "9635", // Geog3D to Geog2D+GravityRelatedHeight (US .gtx)
    };

    if (ci_find(methodName, "Geographic3D to GravityRelatedHeight") == 0) {
        return true;
    }
    if (allowInverse &&
        ci_find(methodName,
                INVERSE_OF + "Geographic3D to GravityRelatedHeight") == 0) {
        return true;
    }

    for (const auto &code : methodCodes) {
        for (const auto &idSrc : method->identifiers()) {
            const auto &srcAuthName = *(idSrc->codeSpace());
            const auto &srcCode = idSrc->code();
            if (ci_equal(srcAuthName, "EPSG") && srcCode == code) {
                return true;
            }
            if (allowInverse && ci_equal(srcAuthName, "INVERSE(EPSG)") &&
                srcCode == code) {
                return true;
            }
        }
    }
    return false;
}
//! @endcond

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
const std::string &Transformation::getHeightToGeographic3DFilename() const {

    const std::string &ret = _getHeightToGeographic3DFilename(this, false);
    if (!ret.empty())
        return ret;
    if (isGeographic3DToGravityRelatedHeight(method(), false)) {
        const auto &fileParameter =
            parameterValue(EPSG_NAME_PARAMETER_GEOID_CORRECTION_FILENAME,
                           EPSG_CODE_PARAMETER_GEOID_CORRECTION_FILENAME);
        if (fileParameter &&
            fileParameter->type() == ParameterValue::Type::FILENAME) {
            return fileParameter->valueFile();
        }
    }
    return nullString;
}
//! @endcond

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
static util::PropertyMap
createSimilarPropertiesMethod(common::IdentifiedObjectNNPtr obj) {
    util::PropertyMap map;

    const std::string &forwardName = obj->nameStr();
    if (!forwardName.empty()) {
        map.set(common::IdentifiedObject::NAME_KEY, forwardName);
    }

    {
        auto ar = util::ArrayOfBaseObject::create();
        for (const auto &idSrc : obj->identifiers()) {
            const auto &srcAuthName = *(idSrc->codeSpace());
            const auto &srcCode = idSrc->code();
            auto idsProp = util::PropertyMap().set(
                metadata::Identifier::CODESPACE_KEY, srcAuthName);
            ar->add(metadata::Identifier::create(srcCode, idsProp));
        }
        if (!ar->empty()) {
            map.set(common::IdentifiedObject::IDENTIFIERS_KEY, ar);
        }
    }

    return map;
}
//! @endcond

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
static util::PropertyMap
createSimilarPropertiesTransformation(TransformationNNPtr obj) {
    util::PropertyMap map;

    // The domain(s) are unchanged
    addDomains(map, obj.get());

    const std::string &forwardName = obj->nameStr();
    if (!forwardName.empty()) {
        map.set(common::IdentifiedObject::NAME_KEY, forwardName);
    }

    const std::string &remarks = obj->remarks();
    if (!remarks.empty()) {
        map.set(common::IdentifiedObject::REMARKS_KEY, remarks);
    }

    addModifiedIdentifier(map, obj.get(), false, true);

    return map;
}
//! @endcond

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
static TransformationNNPtr
createNTv1(const util::PropertyMap &properties,
           const crs::CRSNNPtr &sourceCRSIn, const crs::CRSNNPtr &targetCRSIn,
           const std::string &filename,
           const std::vector<metadata::PositionalAccuracyNNPtr> &accuracies) {
    return Transformation::create(
        properties, sourceCRSIn, targetCRSIn, nullptr,
        createMethodMapNameEPSGCode(EPSG_CODE_METHOD_NTV1),
        {OperationParameter::create(
            util::PropertyMap()
                .set(common::IdentifiedObject::NAME_KEY,
                     EPSG_NAME_PARAMETER_LATITUDE_LONGITUDE_DIFFERENCE_FILE)
                .set(metadata::Identifier::CODESPACE_KEY,
                     metadata::Identifier::EPSG)
                .set(metadata::Identifier::CODE_KEY,
                     EPSG_CODE_PARAMETER_LATITUDE_LONGITUDE_DIFFERENCE_FILE))},
        {ParameterValue::createFilename(filename)}, accuracies);
}
//! @endcond

// ---------------------------------------------------------------------------

/** \brief Return an equivalent transformation to the current one, but using
 * PROJ alternative grid names.
 */
TransformationNNPtr Transformation::substitutePROJAlternativeGridNames(
    io::DatabaseContextNNPtr databaseContext) const {
    auto self = NN_NO_CHECK(std::dynamic_pointer_cast<Transformation>(
        shared_from_this().as_nullable()));

    const auto &l_method = method();
    const int methodEPSGCode = l_method->getEPSGCode();

    std::string projFilename;
    std::string projGridFormat;
    bool inverseDirection = false;

    const auto &NTv1Filename = _getNTv1Filename(this, false);
    const auto &NTv2Filename = _getNTv2Filename(this, false);
    std::string lasFilename;
    if (methodEPSGCode == EPSG_CODE_METHOD_NADCON) {
        const auto &latitudeFileParameter =
            parameterValue(EPSG_NAME_PARAMETER_LATITUDE_DIFFERENCE_FILE,
                           EPSG_CODE_PARAMETER_LATITUDE_DIFFERENCE_FILE);
        const auto &longitudeFileParameter =
            parameterValue(EPSG_NAME_PARAMETER_LONGITUDE_DIFFERENCE_FILE,
                           EPSG_CODE_PARAMETER_LONGITUDE_DIFFERENCE_FILE);
        if (latitudeFileParameter &&
            latitudeFileParameter->type() == ParameterValue::Type::FILENAME &&
            longitudeFileParameter &&
            longitudeFileParameter->type() == ParameterValue::Type::FILENAME) {
            lasFilename = latitudeFileParameter->valueFile();
        }
    }
    const auto &horizontalGridName =
        !NTv1Filename.empty() ? NTv1Filename : !NTv2Filename.empty()
                                                   ? NTv2Filename
                                                   : lasFilename;

    if (!horizontalGridName.empty() &&
        databaseContext->lookForGridAlternative(horizontalGridName,
                                                projFilename, projGridFormat,
                                                inverseDirection)) {

        if (horizontalGridName == projFilename) {
            if (inverseDirection) {
                throw util::UnsupportedOperationException(
                    "Inverse direction for " + projFilename + " not supported");
            }
            return self;
        }

        const auto &l_sourceCRS = sourceCRS();
        const auto &l_targetCRS = targetCRS();
        const auto &l_accuracies = coordinateOperationAccuracies();
        if (projGridFormat == "GTiff") {
            auto parameters =
                std::vector<OperationParameterNNPtr>{createOpParamNameEPSGCode(
                    EPSG_CODE_PARAMETER_LATITUDE_LONGITUDE_DIFFERENCE_FILE)};
            auto methodProperties = util::PropertyMap().set(
                common::IdentifiedObject::NAME_KEY,
                PROJ_WKT2_NAME_METHOD_HORIZONTAL_SHIFT_GTIFF);
            auto values = std::vector<ParameterValueNNPtr>{
                ParameterValue::createFilename(projFilename)};
            if (inverseDirection) {
                return create(createPropertiesForInverse(
                                  self.as_nullable().get(), true, false),
                              l_targetCRS, l_sourceCRS, nullptr,
                              methodProperties, parameters, values,
                              l_accuracies)
                    ->inverseAsTransformation();

            } else {
                return create(createSimilarPropertiesTransformation(self),
                              l_sourceCRS, l_targetCRS, nullptr,
                              methodProperties, parameters, values,
                              l_accuracies);
            }
        } else if (projGridFormat == "NTv1") {
            if (inverseDirection) {
                return createNTv1(createPropertiesForInverse(
                                      self.as_nullable().get(), true, false),
                                  l_targetCRS, l_sourceCRS, projFilename,
                                  l_accuracies)
                    ->inverseAsTransformation();
            } else {
                return createNTv1(createSimilarPropertiesTransformation(self),
                                  l_sourceCRS, l_targetCRS, projFilename,
                                  l_accuracies);
            }
        } else if (projGridFormat == "NTv2") {
            if (inverseDirection) {
                return createNTv2(createPropertiesForInverse(
                                      self.as_nullable().get(), true, false),
                                  l_targetCRS, l_sourceCRS, projFilename,
                                  l_accuracies)
                    ->inverseAsTransformation();
            } else {
                return createNTv2(createSimilarPropertiesTransformation(self),
                                  l_sourceCRS, l_targetCRS, projFilename,
                                  l_accuracies);
            }
        } else if (projGridFormat == "CTable2") {
            auto parameters =
                std::vector<OperationParameterNNPtr>{createOpParamNameEPSGCode(
                    EPSG_CODE_PARAMETER_LATITUDE_LONGITUDE_DIFFERENCE_FILE)};
            auto methodProperties =
                util::PropertyMap().set(common::IdentifiedObject::NAME_KEY,
                                        PROJ_WKT2_NAME_METHOD_CTABLE2);
            auto values = std::vector<ParameterValueNNPtr>{
                ParameterValue::createFilename(projFilename)};
            if (inverseDirection) {
                return create(createPropertiesForInverse(
                                  self.as_nullable().get(), true, false),
                              l_targetCRS, l_sourceCRS, nullptr,
                              methodProperties, parameters, values,
                              l_accuracies)
                    ->inverseAsTransformation();

            } else {
                return create(createSimilarPropertiesTransformation(self),
                              l_sourceCRS, l_targetCRS, nullptr,
                              methodProperties, parameters, values,
                              l_accuracies);
            }
        }
    }

    if (isGeographic3DToGravityRelatedHeight(method(), false)) {
        const auto &fileParameter =
            parameterValue(EPSG_NAME_PARAMETER_GEOID_CORRECTION_FILENAME,
                           EPSG_CODE_PARAMETER_GEOID_CORRECTION_FILENAME);
        if (fileParameter &&
            fileParameter->type() == ParameterValue::Type::FILENAME) {
            auto filename = fileParameter->valueFile();
            if (databaseContext->lookForGridAlternative(
                    filename, projFilename, projGridFormat, inverseDirection)) {

                if (inverseDirection) {
                    throw util::UnsupportedOperationException(
                        "Inverse direction for "
                        "Geographic3DToGravityRelatedHeight not supported");
                }

                if (filename == projFilename) {
                    return self;
                }

                auto parameters = std::vector<OperationParameterNNPtr>{
                    createOpParamNameEPSGCode(
                        EPSG_CODE_PARAMETER_GEOID_CORRECTION_FILENAME)};
#ifdef disabled_for_now
                if (inverseDirection) {
                    return create(createPropertiesForInverse(
                                      self.as_nullable().get(), true, false),
                                  targetCRS(), sourceCRS(), nullptr,
                                  createSimilarPropertiesMethod(method()),
                                  parameters, {ParameterValue::createFilename(
                                                  projFilename)},
                                  coordinateOperationAccuracies())
                        ->inverseAsTransformation();
                } else
#endif
                {
                    return create(
                        createSimilarPropertiesTransformation(self),
                        sourceCRS(), targetCRS(), nullptr,
                        createSimilarPropertiesMethod(method()), parameters,
                        {ParameterValue::createFilename(projFilename)},
                        coordinateOperationAccuracies());
                }
            }
        }
    }

    const auto &geocentricTranslationFilename =
        _getGeocentricTranslationFilename(this, false);
    if (!geocentricTranslationFilename.empty()) {
        if (databaseContext->lookForGridAlternative(
                geocentricTranslationFilename, projFilename, projGridFormat,
                inverseDirection)) {

            if (inverseDirection) {
                throw util::UnsupportedOperationException(
                    "Inverse direction for "
                    "GeocentricTranslation not supported");
            }

            if (geocentricTranslationFilename == projFilename) {
                return self;
            }

            auto parameters =
                std::vector<OperationParameterNNPtr>{createOpParamNameEPSGCode(
                    EPSG_CODE_PARAMETER_GEOCENTRIC_TRANSLATION_FILE)};
            return create(createSimilarPropertiesTransformation(self),
                          sourceCRS(), targetCRS(), interpolationCRS(),
                          createSimilarPropertiesMethod(method()), parameters,
                          {ParameterValue::createFilename(projFilename)},
                          coordinateOperationAccuracies());
        }
    }

    if (methodEPSGCode == EPSG_CODE_METHOD_VERTCON ||
        methodEPSGCode == EPSG_CODE_METHOD_VERTICALGRID_NZLVD ||
        methodEPSGCode == EPSG_CODE_METHOD_VERTICALGRID_BEV_AT ||
        methodEPSGCode == EPSG_CODE_METHOD_VERTICALGRID_GTX) {
        auto fileParameter =
            parameterValue(EPSG_NAME_PARAMETER_VERTICAL_OFFSET_FILE,
                           EPSG_CODE_PARAMETER_VERTICAL_OFFSET_FILE);
        if (fileParameter &&
            fileParameter->type() == ParameterValue::Type::FILENAME) {

            auto filename = fileParameter->valueFile();
            if (databaseContext->lookForGridAlternative(
                    filename, projFilename, projGridFormat, inverseDirection)) {

                if (filename == projFilename) {
                    if (inverseDirection) {
                        throw util::UnsupportedOperationException(
                            "Inverse direction for " + projFilename +
                            " not supported");
                    }
                    return self;
                }

                auto parameters = std::vector<OperationParameterNNPtr>{
                    createOpParamNameEPSGCode(
                        EPSG_CODE_PARAMETER_VERTICAL_OFFSET_FILE)};
                if (inverseDirection) {
                    return create(createPropertiesForInverse(
                                      self.as_nullable().get(), true, false),
                                  targetCRS(), sourceCRS(), nullptr,
                                  createSimilarPropertiesMethod(method()),
                                  parameters, {ParameterValue::createFilename(
                                                  projFilename)},
                                  coordinateOperationAccuracies())
                        ->inverseAsTransformation();
                } else {
                    return create(
                        createSimilarPropertiesTransformation(self),
                        sourceCRS(), targetCRS(), nullptr,
                        createSimilarPropertiesMethod(method()), parameters,
                        {ParameterValue::createFilename(projFilename)},
                        coordinateOperationAccuracies());
                }
            }
        }
    }

    return self;
}
// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress

static void ThrowExceptionNotGeodeticGeographic(const char *trfrm_name) {
    throw io::FormattingException(concat("Can apply ", std::string(trfrm_name),
                                         " only to GeodeticCRS / "
                                         "GeographicCRS"));
}

// ---------------------------------------------------------------------------

// If crs is a geographic CRS, or a compound CRS of a geographic CRS,
// or a compoundCRS of a bound CRS of a geographic CRS, return that
// geographic CRS
static crs::GeographicCRSPtr
extractGeographicCRSIfGeographicCRSOrEquivalent(const crs::CRSNNPtr &crs) {
    auto geogCRS = util::nn_dynamic_pointer_cast<crs::GeographicCRS>(crs);
    if (!geogCRS) {
        auto compoundCRS = util::nn_dynamic_pointer_cast<crs::CompoundCRS>(crs);
        if (compoundCRS) {
            const auto &components = compoundCRS->componentReferenceSystems();
            if (!components.empty()) {
                geogCRS = util::nn_dynamic_pointer_cast<crs::GeographicCRS>(
                    components[0]);
                if (!geogCRS) {
                    auto boundCRS =
                        util::nn_dynamic_pointer_cast<crs::BoundCRS>(
                            components[0]);
                    if (boundCRS) {
                        geogCRS =
                            util::nn_dynamic_pointer_cast<crs::GeographicCRS>(
                                boundCRS->baseCRS());
                    }
                }
            }
        } else {
            auto boundCRS = util::nn_dynamic_pointer_cast<crs::BoundCRS>(crs);
            if (boundCRS) {
                geogCRS = util::nn_dynamic_pointer_cast<crs::GeographicCRS>(
                    boundCRS->baseCRS());
            }
        }
    }
    return geogCRS;
}

// ---------------------------------------------------------------------------

static void setupPROJGeodeticSourceCRS(io::PROJStringFormatter *formatter,
                                       const crs::CRSNNPtr &crs, bool addPushV3,
                                       const char *trfrm_name) {
    auto sourceCRSGeog = extractGeographicCRSIfGeographicCRSOrEquivalent(crs);
    if (sourceCRSGeog) {
        formatter->startInversion();
        sourceCRSGeog->_exportToPROJString(formatter);
        formatter->stopInversion();
        if (util::isOfExactType<crs::DerivedGeographicCRS>(
                *(sourceCRSGeog.get()))) {
            // The export of a DerivedGeographicCRS in non-CRS mode adds
            // unit conversion and axis swapping. We must compensate for that
            formatter->startInversion();
            sourceCRSGeog->addAngularUnitConvertAndAxisSwap(formatter);
            formatter->stopInversion();
        }

        if (addPushV3) {
            formatter->addStep("push");
            formatter->addParam("v_3");
        }

        formatter->addStep("cart");
        sourceCRSGeog->ellipsoid()->_exportToPROJString(formatter);
    } else {
        auto sourceCRSGeod = dynamic_cast<const crs::GeodeticCRS *>(crs.get());
        if (!sourceCRSGeod) {
            ThrowExceptionNotGeodeticGeographic(trfrm_name);
        }
        formatter->startInversion();
        sourceCRSGeod->addGeocentricUnitConversionIntoPROJString(formatter);
        formatter->stopInversion();
    }
}
// ---------------------------------------------------------------------------

static void setupPROJGeodeticTargetCRS(io::PROJStringFormatter *formatter,
                                       const crs::CRSNNPtr &crs, bool addPopV3,
                                       const char *trfrm_name) {
    auto targetCRSGeog = extractGeographicCRSIfGeographicCRSOrEquivalent(crs);
    if (targetCRSGeog) {
        formatter->addStep("cart");
        formatter->setCurrentStepInverted(true);
        targetCRSGeog->ellipsoid()->_exportToPROJString(formatter);

        if (addPopV3) {
            formatter->addStep("pop");
            formatter->addParam("v_3");
        }
        if (util::isOfExactType<crs::DerivedGeographicCRS>(
                *(targetCRSGeog.get()))) {
            // The export of a DerivedGeographicCRS in non-CRS mode adds
            // unit conversion and axis swapping. We must compensate for that
            targetCRSGeog->addAngularUnitConvertAndAxisSwap(formatter);
        }
        targetCRSGeog->_exportToPROJString(formatter);
    } else {
        auto targetCRSGeod = dynamic_cast<const crs::GeodeticCRS *>(crs.get());
        if (!targetCRSGeod) {
            ThrowExceptionNotGeodeticGeographic(trfrm_name);
        }
        targetCRSGeod->addGeocentricUnitConversionIntoPROJString(formatter);
    }
}

//! @endcond
// ---------------------------------------------------------------------------

void Transformation::_exportToPROJString(
    io::PROJStringFormatter *formatter) const // throw(FormattingException)
{
    if (formatter->convention() ==
        io::PROJStringFormatter::Convention::PROJ_4) {
        throw io::FormattingException(
            "Transformation cannot be exported as a PROJ.4 string");
    }

    formatter->setCoordinateOperationOptimizations(true);

    bool positionVectorConvention = true;
    bool sevenParamsTransform = false;
    bool threeParamsTransform = false;
    bool fifteenParamsTransform = false;
    const auto &l_method = method();
    const int methodEPSGCode = l_method->getEPSGCode();
    const auto &methodName = l_method->nameStr();
    const auto paramCount = parameterValues().size();
    const bool l_isTimeDependent = isTimeDependent(methodName);
    const bool isPositionVector =
        ci_find(methodName, "Position Vector") != std::string::npos ||
        ci_find(methodName, "PV") != std::string::npos;
    const bool isCoordinateFrame =
        ci_find(methodName, "Coordinate Frame") != std::string::npos ||
        ci_find(methodName, "CF") != std::string::npos;
    if ((paramCount == 7 && isCoordinateFrame && !l_isTimeDependent) ||
        methodEPSGCode == EPSG_CODE_METHOD_COORDINATE_FRAME_GEOCENTRIC ||
        methodEPSGCode == EPSG_CODE_METHOD_COORDINATE_FRAME_GEOGRAPHIC_2D ||
        methodEPSGCode == EPSG_CODE_METHOD_COORDINATE_FRAME_GEOGRAPHIC_3D) {
        positionVectorConvention = false;
        sevenParamsTransform = true;
    } else if (
        (paramCount == 15 && isCoordinateFrame && l_isTimeDependent) ||
        methodEPSGCode ==
            EPSG_CODE_METHOD_TIME_DEPENDENT_COORDINATE_FRAME_GEOCENTRIC ||
        methodEPSGCode ==
            EPSG_CODE_METHOD_TIME_DEPENDENT_COORDINATE_FRAME_GEOGRAPHIC_2D ||
        methodEPSGCode ==
            EPSG_CODE_METHOD_TIME_DEPENDENT_COORDINATE_FRAME_GEOGRAPHIC_3D) {
        positionVectorConvention = false;
        fifteenParamsTransform = true;
    } else if ((paramCount == 7 && isPositionVector && !l_isTimeDependent) ||
               methodEPSGCode == EPSG_CODE_METHOD_POSITION_VECTOR_GEOCENTRIC ||
               methodEPSGCode ==
                   EPSG_CODE_METHOD_POSITION_VECTOR_GEOGRAPHIC_2D ||
               methodEPSGCode ==
                   EPSG_CODE_METHOD_POSITION_VECTOR_GEOGRAPHIC_3D) {
        sevenParamsTransform = true;
    } else if (
        (paramCount == 15 && isPositionVector && l_isTimeDependent) ||
        methodEPSGCode ==
            EPSG_CODE_METHOD_TIME_DEPENDENT_POSITION_VECTOR_GEOCENTRIC ||
        methodEPSGCode ==
            EPSG_CODE_METHOD_TIME_DEPENDENT_POSITION_VECTOR_GEOGRAPHIC_2D ||
        methodEPSGCode ==
            EPSG_CODE_METHOD_TIME_DEPENDENT_POSITION_VECTOR_GEOGRAPHIC_3D) {
        fifteenParamsTransform = true;
    } else if ((paramCount == 3 &&
                ci_find(methodName, "Geocentric translations") !=
                    std::string::npos) ||
               methodEPSGCode ==
                   EPSG_CODE_METHOD_GEOCENTRIC_TRANSLATION_GEOCENTRIC ||
               methodEPSGCode ==
                   EPSG_CODE_METHOD_GEOCENTRIC_TRANSLATION_GEOGRAPHIC_2D ||
               methodEPSGCode ==
                   EPSG_CODE_METHOD_GEOCENTRIC_TRANSLATION_GEOGRAPHIC_3D) {
        threeParamsTransform = true;
    }
    if (threeParamsTransform || sevenParamsTransform ||
        fifteenParamsTransform) {
        double x =
            parameterValueNumericAsSI(EPSG_CODE_PARAMETER_X_AXIS_TRANSLATION);
        double y =
            parameterValueNumericAsSI(EPSG_CODE_PARAMETER_Y_AXIS_TRANSLATION);
        double z =
            parameterValueNumericAsSI(EPSG_CODE_PARAMETER_Z_AXIS_TRANSLATION);

        auto sourceCRSGeog =
            dynamic_cast<const crs::GeographicCRS *>(sourceCRS().get());
        auto targetCRSGeog =
            dynamic_cast<const crs::GeographicCRS *>(targetCRS().get());
        const bool addPushPopV3 =
            !CoordinateOperation::getPrivate()->use3DHelmert_ &&
            ((sourceCRSGeog &&
              sourceCRSGeog->coordinateSystem()->axisList().size() == 2) ||
             (targetCRSGeog &&
              targetCRSGeog->coordinateSystem()->axisList().size() == 2));

        setupPROJGeodeticSourceCRS(formatter, sourceCRS(), addPushPopV3,
                                   "Helmert");

        formatter->addStep("helmert");
        formatter->addParam("x", x);
        formatter->addParam("y", y);
        formatter->addParam("z", z);
        if (sevenParamsTransform || fifteenParamsTransform) {
            double rx =
                parameterValueNumeric(EPSG_CODE_PARAMETER_X_AXIS_ROTATION,
                                      common::UnitOfMeasure::ARC_SECOND);
            double ry =
                parameterValueNumeric(EPSG_CODE_PARAMETER_Y_AXIS_ROTATION,
                                      common::UnitOfMeasure::ARC_SECOND);
            double rz =
                parameterValueNumeric(EPSG_CODE_PARAMETER_Z_AXIS_ROTATION,
                                      common::UnitOfMeasure::ARC_SECOND);
            double scaleDiff =
                parameterValueNumeric(EPSG_CODE_PARAMETER_SCALE_DIFFERENCE,
                                      common::UnitOfMeasure::PARTS_PER_MILLION);
            formatter->addParam("rx", rx);
            formatter->addParam("ry", ry);
            formatter->addParam("rz", rz);
            formatter->addParam("s", scaleDiff);
            if (fifteenParamsTransform) {
                double rate_x = parameterValueNumeric(
                    EPSG_CODE_PARAMETER_RATE_X_AXIS_TRANSLATION,
                    common::UnitOfMeasure::METRE_PER_YEAR);
                double rate_y = parameterValueNumeric(
                    EPSG_CODE_PARAMETER_RATE_Y_AXIS_TRANSLATION,
                    common::UnitOfMeasure::METRE_PER_YEAR);
                double rate_z = parameterValueNumeric(
                    EPSG_CODE_PARAMETER_RATE_Z_AXIS_TRANSLATION,
                    common::UnitOfMeasure::METRE_PER_YEAR);
                double rate_rx = parameterValueNumeric(
                    EPSG_CODE_PARAMETER_RATE_X_AXIS_ROTATION,
                    common::UnitOfMeasure::ARC_SECOND_PER_YEAR);
                double rate_ry = parameterValueNumeric(
                    EPSG_CODE_PARAMETER_RATE_Y_AXIS_ROTATION,
                    common::UnitOfMeasure::ARC_SECOND_PER_YEAR);
                double rate_rz = parameterValueNumeric(
                    EPSG_CODE_PARAMETER_RATE_Z_AXIS_ROTATION,
                    common::UnitOfMeasure::ARC_SECOND_PER_YEAR);
                double rate_scaleDiff = parameterValueNumeric(
                    EPSG_CODE_PARAMETER_RATE_SCALE_DIFFERENCE,
                    common::UnitOfMeasure::PPM_PER_YEAR);
                double referenceEpochYear =
                    parameterValueNumeric(EPSG_CODE_PARAMETER_REFERENCE_EPOCH,
                                          common::UnitOfMeasure::YEAR);
                formatter->addParam("dx", rate_x);
                formatter->addParam("dy", rate_y);
                formatter->addParam("dz", rate_z);
                formatter->addParam("drx", rate_rx);
                formatter->addParam("dry", rate_ry);
                formatter->addParam("drz", rate_rz);
                formatter->addParam("ds", rate_scaleDiff);
                formatter->addParam("t_epoch", referenceEpochYear);
            }
            if (positionVectorConvention) {
                formatter->addParam("convention", "position_vector");
            } else {
                formatter->addParam("convention", "coordinate_frame");
            }
        }

        setupPROJGeodeticTargetCRS(formatter, targetCRS(), addPushPopV3,
                                   "Helmert");

        return;
    }

    if (methodEPSGCode == EPSG_CODE_METHOD_MOLODENSKY_BADEKAS_CF_GEOCENTRIC ||
        methodEPSGCode == EPSG_CODE_METHOD_MOLODENSKY_BADEKAS_PV_GEOCENTRIC ||
        methodEPSGCode ==
            EPSG_CODE_METHOD_MOLODENSKY_BADEKAS_CF_GEOGRAPHIC_3D ||
        methodEPSGCode ==
            EPSG_CODE_METHOD_MOLODENSKY_BADEKAS_PV_GEOGRAPHIC_3D ||
        methodEPSGCode ==
            EPSG_CODE_METHOD_MOLODENSKY_BADEKAS_CF_GEOGRAPHIC_2D ||
        methodEPSGCode ==
            EPSG_CODE_METHOD_MOLODENSKY_BADEKAS_PV_GEOGRAPHIC_2D) {

        positionVectorConvention =
            isPositionVector ||
            methodEPSGCode ==
                EPSG_CODE_METHOD_MOLODENSKY_BADEKAS_PV_GEOCENTRIC ||
            methodEPSGCode ==
                EPSG_CODE_METHOD_MOLODENSKY_BADEKAS_PV_GEOGRAPHIC_3D ||
            methodEPSGCode ==
                EPSG_CODE_METHOD_MOLODENSKY_BADEKAS_PV_GEOGRAPHIC_2D;

        double x =
            parameterValueNumericAsSI(EPSG_CODE_PARAMETER_X_AXIS_TRANSLATION);
        double y =
            parameterValueNumericAsSI(EPSG_CODE_PARAMETER_Y_AXIS_TRANSLATION);
        double z =
            parameterValueNumericAsSI(EPSG_CODE_PARAMETER_Z_AXIS_TRANSLATION);
        double rx = parameterValueNumeric(EPSG_CODE_PARAMETER_X_AXIS_ROTATION,
                                          common::UnitOfMeasure::ARC_SECOND);
        double ry = parameterValueNumeric(EPSG_CODE_PARAMETER_Y_AXIS_ROTATION,
                                          common::UnitOfMeasure::ARC_SECOND);
        double rz = parameterValueNumeric(EPSG_CODE_PARAMETER_Z_AXIS_ROTATION,
                                          common::UnitOfMeasure::ARC_SECOND);
        double scaleDiff =
            parameterValueNumeric(EPSG_CODE_PARAMETER_SCALE_DIFFERENCE,
                                  common::UnitOfMeasure::PARTS_PER_MILLION);

        double px = parameterValueNumericAsSI(
            EPSG_CODE_PARAMETER_ORDINATE_1_EVAL_POINT);
        double py = parameterValueNumericAsSI(
            EPSG_CODE_PARAMETER_ORDINATE_2_EVAL_POINT);
        double pz = parameterValueNumericAsSI(
            EPSG_CODE_PARAMETER_ORDINATE_3_EVAL_POINT);

        bool addPushPopV3 =
            (methodEPSGCode ==
                 EPSG_CODE_METHOD_MOLODENSKY_BADEKAS_PV_GEOGRAPHIC_2D ||
             methodEPSGCode ==
                 EPSG_CODE_METHOD_MOLODENSKY_BADEKAS_CF_GEOGRAPHIC_2D);

        setupPROJGeodeticSourceCRS(formatter, sourceCRS(), addPushPopV3,
                                   "Molodensky-Badekas");

        formatter->addStep("molobadekas");
        formatter->addParam("x", x);
        formatter->addParam("y", y);
        formatter->addParam("z", z);
        formatter->addParam("rx", rx);
        formatter->addParam("ry", ry);
        formatter->addParam("rz", rz);
        formatter->addParam("s", scaleDiff);
        formatter->addParam("px", px);
        formatter->addParam("py", py);
        formatter->addParam("pz", pz);
        if (positionVectorConvention) {
            formatter->addParam("convention", "position_vector");
        } else {
            formatter->addParam("convention", "coordinate_frame");
        }

        setupPROJGeodeticTargetCRS(formatter, targetCRS(), addPushPopV3,
                                   "Molodensky-Badekas");

        return;
    }

    if (methodEPSGCode == EPSG_CODE_METHOD_MOLODENSKY ||
        methodEPSGCode == EPSG_CODE_METHOD_ABRIDGED_MOLODENSKY) {
        double x =
            parameterValueNumericAsSI(EPSG_CODE_PARAMETER_X_AXIS_TRANSLATION);
        double y =
            parameterValueNumericAsSI(EPSG_CODE_PARAMETER_Y_AXIS_TRANSLATION);
        double z =
            parameterValueNumericAsSI(EPSG_CODE_PARAMETER_Z_AXIS_TRANSLATION);
        double da = parameterValueNumericAsSI(
            EPSG_CODE_PARAMETER_SEMI_MAJOR_AXIS_DIFFERENCE);
        double df = parameterValueNumericAsSI(
            EPSG_CODE_PARAMETER_FLATTENING_DIFFERENCE);

        auto sourceCRSGeog =
            dynamic_cast<const crs::GeographicCRS *>(sourceCRS().get());
        if (!sourceCRSGeog) {
            throw io::FormattingException(
                "Can apply Molodensky only to GeographicCRS");
        }

        auto targetCRSGeog =
            dynamic_cast<const crs::GeographicCRS *>(targetCRS().get());
        if (!targetCRSGeog) {
            throw io::FormattingException(
                "Can apply Molodensky only to GeographicCRS");
        }

        formatter->startInversion();
        sourceCRSGeog->_exportToPROJString(formatter);
        formatter->stopInversion();

        formatter->addStep("molodensky");
        sourceCRSGeog->ellipsoid()->_exportToPROJString(formatter);
        formatter->addParam("dx", x);
        formatter->addParam("dy", y);
        formatter->addParam("dz", z);
        formatter->addParam("da", da);
        formatter->addParam("df", df);

        if (ci_find(methodName, "Abridged") != std::string::npos ||
            methodEPSGCode == EPSG_CODE_METHOD_ABRIDGED_MOLODENSKY) {
            formatter->addParam("abridged");
        }

        targetCRSGeog->_exportToPROJString(formatter);

        return;
    }

    if (methodEPSGCode == EPSG_CODE_METHOD_GEOGRAPHIC2D_OFFSETS) {
        double offsetLat =
            parameterValueNumeric(EPSG_CODE_PARAMETER_LATITUDE_OFFSET,
                                  common::UnitOfMeasure::ARC_SECOND);
        double offsetLong =
            parameterValueNumeric(EPSG_CODE_PARAMETER_LONGITUDE_OFFSET,
                                  common::UnitOfMeasure::ARC_SECOND);

        auto sourceCRSGeog =
            dynamic_cast<const crs::GeographicCRS *>(sourceCRS().get());
        if (!sourceCRSGeog) {
            throw io::FormattingException(
                "Can apply Geographic 2D offsets only to GeographicCRS");
        }

        auto targetCRSGeog =
            dynamic_cast<const crs::GeographicCRS *>(targetCRS().get());
        if (!targetCRSGeog) {
            throw io::FormattingException(
                "Can apply Geographic 2D offsets only to GeographicCRS");
        }

        formatter->startInversion();
        sourceCRSGeog->addAngularUnitConvertAndAxisSwap(formatter);
        formatter->stopInversion();

        if (offsetLat != 0.0 || offsetLong != 0.0) {
            formatter->addStep("geogoffset");
            formatter->addParam("dlat", offsetLat);
            formatter->addParam("dlon", offsetLong);
        }

        targetCRSGeog->addAngularUnitConvertAndAxisSwap(formatter);

        return;
    }

    if (methodEPSGCode == EPSG_CODE_METHOD_GEOGRAPHIC3D_OFFSETS) {
        double offsetLat =
            parameterValueNumeric(EPSG_CODE_PARAMETER_LATITUDE_OFFSET,
                                  common::UnitOfMeasure::ARC_SECOND);
        double offsetLong =
            parameterValueNumeric(EPSG_CODE_PARAMETER_LONGITUDE_OFFSET,
                                  common::UnitOfMeasure::ARC_SECOND);
        double offsetHeight =
            parameterValueNumericAsSI(EPSG_CODE_PARAMETER_VERTICAL_OFFSET);

        auto sourceCRSGeog =
            dynamic_cast<const crs::GeographicCRS *>(sourceCRS().get());
        if (!sourceCRSGeog) {
            throw io::FormattingException(
                "Can apply Geographic 3D offsets only to GeographicCRS");
        }

        auto targetCRSGeog =
            dynamic_cast<const crs::GeographicCRS *>(targetCRS().get());
        if (!targetCRSGeog) {
            throw io::FormattingException(
                "Can apply Geographic 3D offsets only to GeographicCRS");
        }

        formatter->startInversion();
        sourceCRSGeog->addAngularUnitConvertAndAxisSwap(formatter);
        formatter->stopInversion();

        if (offsetLat != 0.0 || offsetLong != 0.0 || offsetHeight != 0.0) {
            formatter->addStep("geogoffset");
            formatter->addParam("dlat", offsetLat);
            formatter->addParam("dlon", offsetLong);
            formatter->addParam("dh", offsetHeight);
        }

        targetCRSGeog->addAngularUnitConvertAndAxisSwap(formatter);

        return;
    }

    if (methodEPSGCode == EPSG_CODE_METHOD_GEOGRAPHIC2D_WITH_HEIGHT_OFFSETS) {
        double offsetLat =
            parameterValueNumeric(EPSG_CODE_PARAMETER_LATITUDE_OFFSET,
                                  common::UnitOfMeasure::ARC_SECOND);
        double offsetLong =
            parameterValueNumeric(EPSG_CODE_PARAMETER_LONGITUDE_OFFSET,
                                  common::UnitOfMeasure::ARC_SECOND);
        double offsetHeight =
            parameterValueNumericAsSI(EPSG_CODE_PARAMETER_GEOID_UNDULATION);

        auto sourceCRSGeog =
            dynamic_cast<const crs::GeographicCRS *>(sourceCRS().get());
        if (!sourceCRSGeog) {
            auto sourceCRSCompound =
                dynamic_cast<const crs::CompoundCRS *>(sourceCRS().get());
            if (sourceCRSCompound) {
                sourceCRSGeog = sourceCRSCompound->extractGeographicCRS().get();
            }
            if (!sourceCRSGeog) {
                throw io::FormattingException("Can apply Geographic 2D with "
                                              "height offsets only to "
                                              "GeographicCRS / CompoundCRS");
            }
        }

        auto targetCRSGeog =
            dynamic_cast<const crs::GeographicCRS *>(targetCRS().get());
        if (!targetCRSGeog) {
            auto targetCRSCompound =
                dynamic_cast<const crs::CompoundCRS *>(targetCRS().get());
            if (targetCRSCompound) {
                targetCRSGeog = targetCRSCompound->extractGeographicCRS().get();
            }
            if (!targetCRSGeog) {
                throw io::FormattingException("Can apply Geographic 2D with "
                                              "height offsets only to "
                                              "GeographicCRS / CompoundCRS");
            }
        }

        formatter->startInversion();
        sourceCRSGeog->addAngularUnitConvertAndAxisSwap(formatter);
        formatter->stopInversion();

        if (offsetLat != 0.0 || offsetLong != 0.0 || offsetHeight != 0.0) {
            formatter->addStep("geogoffset");
            formatter->addParam("dlat", offsetLat);
            formatter->addParam("dlon", offsetLong);
            formatter->addParam("dh", offsetHeight);
        }

        targetCRSGeog->addAngularUnitConvertAndAxisSwap(formatter);

        return;
    }

    if (methodEPSGCode == EPSG_CODE_METHOD_VERTICAL_OFFSET) {

        auto sourceCRSVert =
            dynamic_cast<const crs::VerticalCRS *>(sourceCRS().get());
        if (!sourceCRSVert) {
            throw io::FormattingException(
                "Can apply Vertical offset only to VerticalCRS");
        }

        auto targetCRSVert =
            dynamic_cast<const crs::VerticalCRS *>(targetCRS().get());
        if (!targetCRSVert) {
            throw io::FormattingException(
                "Can apply Vertical offset only to VerticalCRS");
        }

        auto offsetHeight =
            parameterValueNumericAsSI(EPSG_CODE_PARAMETER_VERTICAL_OFFSET);

        formatter->startInversion();
        sourceCRSVert->addLinearUnitConvert(formatter);
        formatter->stopInversion();

        formatter->addStep("geogoffset");
        formatter->addParam("dh", offsetHeight);

        targetCRSVert->addLinearUnitConvert(formatter);

        return;
    }

    // Substitute grid names with PROJ friendly names.
    if (formatter->databaseContext()) {
        auto alternate = substitutePROJAlternativeGridNames(
            NN_NO_CHECK(formatter->databaseContext()));
        auto self = NN_NO_CHECK(std::dynamic_pointer_cast<Transformation>(
            shared_from_this().as_nullable()));

        if (alternate != self) {
            alternate->_exportToPROJString(formatter);
            return;
        }
    }

    const bool isMethodInverseOf = starts_with(methodName, INVERSE_OF);

    const auto &NTv1Filename = _getNTv1Filename(this, true);
    const auto &NTv2Filename = _getNTv2Filename(this, true);
    const auto &CTABLE2Filename = _getCTABLE2Filename(this, true);
    const auto &HorizontalShiftGTIFFFilename =
        _getHorizontalShiftGTIFFFilename(this, true);
    const auto &hGridShiftFilename =
        !HorizontalShiftGTIFFFilename.empty()
            ? HorizontalShiftGTIFFFilename
            : !NTv1Filename.empty() ? NTv1Filename : !NTv2Filename.empty()
                                                         ? NTv2Filename
                                                         : CTABLE2Filename;
    if (!hGridShiftFilename.empty()) {
        auto sourceCRSGeog =
            extractGeographicCRSIfGeographicCRSOrEquivalent(sourceCRS());
        if (!sourceCRSGeog) {
            throw io::FormattingException(
                concat("Can apply ", methodName, " only to GeographicCRS"));
        }

        auto targetCRSGeog =
            extractGeographicCRSIfGeographicCRSOrEquivalent(targetCRS());
        if (!targetCRSGeog) {
            throw io::FormattingException(
                concat("Can apply ", methodName, " only to GeographicCRS"));
        }

        formatter->startInversion();
        sourceCRSGeog->addAngularUnitConvertAndAxisSwap(formatter);
        formatter->stopInversion();

        if (isMethodInverseOf) {
            formatter->startInversion();
        }
        formatter->addStep("hgridshift");
        formatter->addParam("grids", hGridShiftFilename);
        if (isMethodInverseOf) {
            formatter->stopInversion();
        }

        targetCRSGeog->addAngularUnitConvertAndAxisSwap(formatter);

        return;
    }

    const auto &geocentricTranslationFilename =
        _getGeocentricTranslationFilename(this, true);
    if (!geocentricTranslationFilename.empty()) {
        auto sourceCRSGeog =
            dynamic_cast<const crs::GeographicCRS *>(sourceCRS().get());
        if (!sourceCRSGeog) {
            throw io::FormattingException(
                concat("Can apply ", methodName, " only to GeographicCRS"));
        }

        auto targetCRSGeog =
            dynamic_cast<const crs::GeographicCRS *>(targetCRS().get());
        if (!targetCRSGeog) {
            throw io::FormattingException(
                concat("Can apply ", methodName, " only to GeographicCRS"));
        }

        const auto &interpCRS = interpolationCRS();
        if (!interpCRS) {
            throw io::FormattingException(
                "InterpolationCRS required "
                "for"
                " " EPSG_NAME_METHOD_GEOCENTRIC_TRANSLATION_BY_GRID_INTERPOLATION_IGN);
        }
        const bool interpIsSrc = interpCRS->_isEquivalentTo(
            sourceCRS().get(), util::IComparable::Criterion::EQUIVALENT);
        const bool interpIsTarget = interpCRS->_isEquivalentTo(
            targetCRS().get(), util::IComparable::Criterion::EQUIVALENT);
        if (!interpIsSrc && !interpIsTarget) {
            throw io::FormattingException(
                "For"
                " " EPSG_NAME_METHOD_GEOCENTRIC_TRANSLATION_BY_GRID_INTERPOLATION_IGN
                ", interpolation CRS should be the source or target CRS");
        }

        formatter->startInversion();
        sourceCRSGeog->addAngularUnitConvertAndAxisSwap(formatter);
        formatter->stopInversion();

        if (isMethodInverseOf) {
            formatter->startInversion();
        }

        formatter->addStep("push");
        formatter->addParam("v_3");

        formatter->addStep("cart");
        sourceCRSGeog->ellipsoid()->_exportToPROJString(formatter);

        formatter->addStep("xyzgridshift");
        formatter->addParam("grids", geocentricTranslationFilename);
        formatter->addParam("grid_ref",
                            interpIsTarget ? "output_crs" : "input_crs");
        (interpIsTarget ? targetCRSGeog : sourceCRSGeog)
            ->ellipsoid()
            ->_exportToPROJString(formatter);

        formatter->startInversion();
        formatter->addStep("cart");
        targetCRSGeog->ellipsoid()->_exportToPROJString(formatter);
        formatter->stopInversion();

        formatter->addStep("pop");
        formatter->addParam("v_3");

        if (isMethodInverseOf) {
            formatter->stopInversion();
        }

        targetCRSGeog->addAngularUnitConvertAndAxisSwap(formatter);

        return;
    }

    const auto &heightFilename = _getHeightToGeographic3DFilename(this, true);
    if (!heightFilename.empty()) {
        auto targetCRSGeog =
            extractGeographicCRSIfGeographicCRSOrEquivalent(targetCRS());
        if (!targetCRSGeog) {
            throw io::FormattingException(
                concat("Can apply ", methodName, " only to GeographicCRS"));
        }

        if (!formatter->omitHorizontalConversionInVertTransformation()) {
            formatter->startInversion();
            formatter->pushOmitZUnitConversion();
            targetCRSGeog->addAngularUnitConvertAndAxisSwap(formatter);
            formatter->popOmitZUnitConversion();
            formatter->stopInversion();
        }

        if (isMethodInverseOf) {
            formatter->startInversion();
        }
        formatter->addStep("vgridshift");
        formatter->addParam("grids", heightFilename);
        formatter->addParam("multiplier", 1.0);
        if (isMethodInverseOf) {
            formatter->stopInversion();
        }

        if (!formatter->omitHorizontalConversionInVertTransformation()) {
            formatter->pushOmitZUnitConversion();
            targetCRSGeog->addAngularUnitConvertAndAxisSwap(formatter);
            formatter->popOmitZUnitConversion();
        }

        return;
    }

    if (isGeographic3DToGravityRelatedHeight(method(), true)) {
        auto fileParameter =
            parameterValue(EPSG_NAME_PARAMETER_GEOID_CORRECTION_FILENAME,
                           EPSG_CODE_PARAMETER_GEOID_CORRECTION_FILENAME);
        if (fileParameter &&
            fileParameter->type() == ParameterValue::Type::FILENAME) {
            auto filename = fileParameter->valueFile();

            auto sourceCRSGeog =
                extractGeographicCRSIfGeographicCRSOrEquivalent(sourceCRS());
            if (!sourceCRSGeog) {
                throw io::FormattingException(
                    concat("Can apply ", methodName, " only to GeographicCRS"));
            }

            if (!formatter->omitHorizontalConversionInVertTransformation()) {
                formatter->startInversion();
                formatter->pushOmitZUnitConversion();
                sourceCRSGeog->addAngularUnitConvertAndAxisSwap(formatter);
                formatter->popOmitZUnitConversion();
                formatter->stopInversion();
            }

            bool doInversion = isMethodInverseOf;
            // The EPSG Geog3DToHeight is the reverse convention of PROJ !
            doInversion = !doInversion;
            if (doInversion) {
                formatter->startInversion();
            }
            formatter->addStep("vgridshift");
            formatter->addParam("grids", filename);
            formatter->addParam("multiplier", 1.0);
            if (doInversion) {
                formatter->stopInversion();
            }

            if (!formatter->omitHorizontalConversionInVertTransformation()) {
                formatter->pushOmitZUnitConversion();
                sourceCRSGeog->addAngularUnitConvertAndAxisSwap(formatter);
                formatter->popOmitZUnitConversion();
            }

            return;
        }
    }

    if (methodEPSGCode == EPSG_CODE_METHOD_VERTCON) {
        auto fileParameter =
            parameterValue(EPSG_NAME_PARAMETER_VERTICAL_OFFSET_FILE,
                           EPSG_CODE_PARAMETER_VERTICAL_OFFSET_FILE);
        if (fileParameter &&
            fileParameter->type() == ParameterValue::Type::FILENAME) {
            formatter->addStep("vgridshift");
            formatter->addParam("grids", fileParameter->valueFile());
            if (fileParameter->valueFile().find(".tif") != std::string::npos) {
                formatter->addParam("multiplier", 1.0);
            } else {
                // The vertcon grids go from NGVD 29 to NAVD 88, with units
                // in millimeter (see
                // https://github.com/OSGeo/proj.4/issues/1071), for gtx files
                formatter->addParam("multiplier", 0.001);
            }
            return;
        }
    }

    if (methodEPSGCode == EPSG_CODE_METHOD_VERTICALGRID_NZLVD ||
        methodEPSGCode == EPSG_CODE_METHOD_VERTICALGRID_BEV_AT ||
        methodEPSGCode == EPSG_CODE_METHOD_VERTICALGRID_GTX) {
        auto fileParameter =
            parameterValue(EPSG_NAME_PARAMETER_VERTICAL_OFFSET_FILE,
                           EPSG_CODE_PARAMETER_VERTICAL_OFFSET_FILE);
        if (fileParameter &&
            fileParameter->type() == ParameterValue::Type::FILENAME) {
            formatter->addStep("vgridshift");
            formatter->addParam("grids", fileParameter->valueFile());
            formatter->addParam("multiplier", 1.0);
            return;
        }
    }

    if (isLongitudeRotation()) {
        double offsetDeg =
            parameterValueNumeric(EPSG_CODE_PARAMETER_LONGITUDE_OFFSET,
                                  common::UnitOfMeasure::DEGREE);

        auto sourceCRSGeog =
            dynamic_cast<const crs::GeographicCRS *>(sourceCRS().get());
        if (!sourceCRSGeog) {
            throw io::FormattingException(
                concat("Can apply ", methodName, " only to GeographicCRS"));
        }

        auto targetCRSGeog =
            dynamic_cast<const crs::GeographicCRS *>(targetCRS().get());
        if (!targetCRSGeog) {
            throw io::FormattingException(
                concat("Can apply ", methodName + " only to GeographicCRS"));
        }

        if (!sourceCRSGeog->ellipsoid()->_isEquivalentTo(
                targetCRSGeog->ellipsoid().get(),
                util::IComparable::Criterion::EQUIVALENT)) {
            // This is arguable if we should check this...
            throw io::FormattingException("Can apply Longitude rotation "
                                          "only to SRS with same "
                                          "ellipsoid");
        }

        formatter->startInversion();
        sourceCRSGeog->addAngularUnitConvertAndAxisSwap(formatter);
        formatter->stopInversion();

        bool done = false;
        if (offsetDeg != 0.0) {
            // Optimization: as we are doing nominally a +step=inv,
            // if the negation of the offset value is a well-known name,
            // then use forward case with this name.
            auto projPMName = datum::PrimeMeridian::getPROJStringWellKnownName(
                common::Angle(-offsetDeg));
            if (!projPMName.empty()) {
                done = true;
                formatter->addStep("longlat");
                sourceCRSGeog->ellipsoid()->_exportToPROJString(formatter);
                formatter->addParam("pm", projPMName);
            }
        }
        if (!done) {
            // To actually add the offset, we must use the reverse longlat
            // operation.
            formatter->startInversion();
            formatter->addStep("longlat");
            sourceCRSGeog->ellipsoid()->_exportToPROJString(formatter);
            datum::PrimeMeridian::create(util::PropertyMap(),
                                         common::Angle(offsetDeg))
                ->_exportToPROJString(formatter);
            formatter->stopInversion();
        }

        targetCRSGeog->addAngularUnitConvertAndAxisSwap(formatter);

        return;
    }

    if (exportToPROJStringGeneric(formatter)) {
        return;
    }

    throw io::FormattingException("Unimplemented");
}

// ---------------------------------------------------------------------------

bool SingleOperation::exportToPROJStringGeneric(
    io::PROJStringFormatter *formatter) const {
    const int methodEPSGCode = method()->getEPSGCode();

    if (methodEPSGCode == EPSG_CODE_METHOD_AFFINE_PARAMETRIC_TRANSFORMATION) {
        const double A0 = parameterValueMeasure(EPSG_CODE_PARAMETER_A0).value();
        const double A1 = parameterValueMeasure(EPSG_CODE_PARAMETER_A1).value();
        const double A2 = parameterValueMeasure(EPSG_CODE_PARAMETER_A2).value();
        const double B0 = parameterValueMeasure(EPSG_CODE_PARAMETER_B0).value();
        const double B1 = parameterValueMeasure(EPSG_CODE_PARAMETER_B1).value();
        const double B2 = parameterValueMeasure(EPSG_CODE_PARAMETER_B2).value();

        // Do not mess with axis unit and order for that transformation

        formatter->addStep("affine");
        formatter->addParam("xoff", A0);
        formatter->addParam("s11", A1);
        formatter->addParam("s12", A2);
        formatter->addParam("yoff", B0);
        formatter->addParam("s21", B1);
        formatter->addParam("s22", B2);

        return true;
    }

    if (isAxisOrderReversal(methodEPSGCode)) {
        formatter->addStep("axisswap");
        formatter->addParam("order", "2,1");
        auto sourceCRSGeog =
            dynamic_cast<const crs::GeographicCRS *>(sourceCRS().get());
        auto targetCRSGeog =
            dynamic_cast<const crs::GeographicCRS *>(targetCRS().get());
        if (sourceCRSGeog && targetCRSGeog) {
            const auto &unitSrc =
                sourceCRSGeog->coordinateSystem()->axisList()[0]->unit();
            const auto &unitDst =
                targetCRSGeog->coordinateSystem()->axisList()[0]->unit();
            if (!unitSrc._isEquivalentTo(
                    unitDst, util::IComparable::Criterion::EQUIVALENT)) {
                formatter->addStep("unitconvert");
                auto projUnit = unitSrc.exportToPROJString();
                if (projUnit.empty()) {
                    formatter->addParam("xy_in", unitSrc.conversionToSI());
                } else {
                    formatter->addParam("xy_in", projUnit);
                }
                projUnit = unitDst.exportToPROJString();
                if (projUnit.empty()) {
                    formatter->addParam("xy_out", unitDst.conversionToSI());
                } else {
                    formatter->addParam("xy_out", projUnit);
                }
            }
        }
        return true;
    }

    if (methodEPSGCode == EPSG_CODE_METHOD_GEOGRAPHIC_GEOCENTRIC) {

        auto sourceCRSGeod =
            dynamic_cast<const crs::GeodeticCRS *>(sourceCRS().get());
        auto targetCRSGeod =
            dynamic_cast<const crs::GeodeticCRS *>(targetCRS().get());
        if (sourceCRSGeod && targetCRSGeod) {
            auto sourceCRSGeog =
                dynamic_cast<const crs::GeographicCRS *>(sourceCRSGeod);
            auto targetCRSGeog =
                dynamic_cast<const crs::GeographicCRS *>(targetCRSGeod);
            bool isSrcGeocentric = sourceCRSGeod->isGeocentric();
            bool isSrcGeographic = sourceCRSGeog != nullptr;
            bool isTargetGeocentric = targetCRSGeod->isGeocentric();
            bool isTargetGeographic = targetCRSGeog != nullptr;
            if ((isSrcGeocentric && isTargetGeographic) ||
                (isSrcGeographic && isTargetGeocentric)) {

                formatter->startInversion();
                sourceCRSGeod->_exportToPROJString(formatter);
                formatter->stopInversion();

                targetCRSGeod->_exportToPROJString(formatter);

                return true;
            }
        }

        throw io::FormattingException("Invalid nature of source and/or "
                                      "targetCRS for Geographic/Geocentric "
                                      "conversion");
    }

    if (methodEPSGCode == EPSG_CODE_METHOD_CHANGE_VERTICAL_UNIT) {
        double convFactor = parameterValueNumericAsSI(
            EPSG_CODE_PARAMETER_UNIT_CONVERSION_SCALAR);
        auto uom = common::UnitOfMeasure(std::string(), convFactor,
                                         common::UnitOfMeasure::Type::LINEAR)
                       .exportToPROJString();
        auto reverse_uom =
            common::UnitOfMeasure(std::string(), 1.0 / convFactor,
                                  common::UnitOfMeasure::Type::LINEAR)
                .exportToPROJString();
        if (uom == "m") {
            // do nothing
        } else if (!uom.empty()) {
            formatter->addStep("unitconvert");
            formatter->addParam("z_in", uom);
            formatter->addParam("z_out", "m");
        } else if (!reverse_uom.empty()) {
            formatter->addStep("unitconvert");
            formatter->addParam("z_in", "m");
            formatter->addParam("z_out", reverse_uom);
        } else {
            formatter->addStep("affine");
            formatter->addParam("s33", convFactor);
        }
        return true;
    }

    if (methodEPSGCode == EPSG_CODE_METHOD_HEIGHT_DEPTH_REVERSAL) {
        formatter->addStep("axisswap");
        formatter->addParam("order", "1,2,-3");
        return true;
    }

    return false;
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
PointMotionOperation::~PointMotionOperation() = default;
//! @endcond

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
struct ConcatenatedOperation::Private {
    std::vector<CoordinateOperationNNPtr> operations_{};
    bool computedName_ = false;

    explicit Private(const std::vector<CoordinateOperationNNPtr> &operationsIn)
        : operations_(operationsIn) {}
    Private(const Private &) = default;
};
//! @endcond

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
ConcatenatedOperation::~ConcatenatedOperation() = default;
//! @endcond

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
ConcatenatedOperation::ConcatenatedOperation(const ConcatenatedOperation &other)
    : CoordinateOperation(other),
      d(internal::make_unique<Private>(*(other.d))) {}
//! @endcond

// ---------------------------------------------------------------------------

ConcatenatedOperation::ConcatenatedOperation(
    const std::vector<CoordinateOperationNNPtr> &operationsIn)
    : CoordinateOperation(), d(internal::make_unique<Private>(operationsIn)) {}

// ---------------------------------------------------------------------------

/** \brief Return the operation steps of the concatenated operation.
 *
 * @return the operation steps.
 */
const std::vector<CoordinateOperationNNPtr> &
ConcatenatedOperation::operations() const {
    return d->operations_;
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
static bool compareStepCRS(const crs::CRS *a, const crs::CRS *b) {
    const auto &aIds = a->identifiers();
    const auto &bIds = b->identifiers();
    if (aIds.size() == 1 && bIds.size() == 1 &&
        aIds[0]->code() == bIds[0]->code() &&
        *aIds[0]->codeSpace() == *bIds[0]->codeSpace()) {
        return true;
    }
    return a->_isEquivalentTo(b, util::IComparable::Criterion::EQUIVALENT);
}
//! @endcond

// ---------------------------------------------------------------------------

/** \brief Instantiate a ConcatenatedOperation
 *
 * @param properties See \ref general_properties. At minimum the name should
 * be
 * defined.
 * @param operationsIn Vector of the CoordinateOperation steps.
 * @param accuracies Vector of positional accuracy (might be empty).
 * @return new Transformation.
 * @throws InvalidOperation
 */
ConcatenatedOperationNNPtr ConcatenatedOperation::create(
    const util::PropertyMap &properties,
    const std::vector<CoordinateOperationNNPtr> &operationsIn,
    const std::vector<metadata::PositionalAccuracyNNPtr>
        &accuracies) // throw InvalidOperation
{
    if (operationsIn.size() < 2) {
        throw InvalidOperation(
            "ConcatenatedOperation must have at least 2 operations");
    }
    crs::CRSPtr lastTargetCRS;

    crs::CRSPtr interpolationCRS;
    bool interpolationCRSValid = true;
    for (size_t i = 0; i < operationsIn.size(); i++) {
        auto l_sourceCRS = operationsIn[i]->sourceCRS();
        auto l_targetCRS = operationsIn[i]->targetCRS();

        if (interpolationCRSValid) {
            auto subOpInterpCRS = operationsIn[i]->interpolationCRS();
            if (interpolationCRS == nullptr)
                interpolationCRS = subOpInterpCRS;
            else if (subOpInterpCRS == nullptr ||
                     !(subOpInterpCRS->isEquivalentTo(
                         interpolationCRS.get(),
                         util::IComparable::Criterion::EQUIVALENT))) {
                interpolationCRS = nullptr;
                interpolationCRSValid = false;
            }
        }

        if (l_sourceCRS == nullptr || l_targetCRS == nullptr) {
            throw InvalidOperation("At least one of the operation lacks a "
                                   "source and/or target CRS");
        }
        if (i >= 1) {
            if (!compareStepCRS(l_sourceCRS.get(), lastTargetCRS.get())) {
#ifdef DEBUG_CONCATENATED_OPERATION
                std::cerr << "Step " << i - 1 << ": "
                          << operationsIn[i - 1]->nameStr() << std::endl;
                std::cerr << "Step " << i << ": " << operationsIn[i]->nameStr()
                          << std::endl;
                {
                    auto f(io::WKTFormatter::create(
                        io::WKTFormatter::Convention::WKT2_2019));
                    std::cerr << "Source CRS of step " << i << ":" << std::endl;
                    std::cerr << l_sourceCRS->exportToWKT(f.get()) << std::endl;
                }
                {
                    auto f(io::WKTFormatter::create(
                        io::WKTFormatter::Convention::WKT2_2019));
                    std::cerr << "Target CRS of step " << i - 1 << ":"
                              << std::endl;
                    std::cerr << lastTargetCRS->exportToWKT(f.get())
                              << std::endl;
                }
#endif
                throw InvalidOperation(
                    "Inconsistent chaining of CRS in operations");
            }
        }
        lastTargetCRS = l_targetCRS;
    }
    auto op = ConcatenatedOperation::nn_make_shared<ConcatenatedOperation>(
        operationsIn);
    op->assignSelf(op);
    op->setProperties(properties);
    op->setCRSs(NN_NO_CHECK(operationsIn[0]->sourceCRS()),
                NN_NO_CHECK(operationsIn.back()->targetCRS()),
                interpolationCRS);
    op->setAccuracies(accuracies);
#ifdef DEBUG_CONCATENATED_OPERATION
    {
        auto f(
            io::WKTFormatter::create(io::WKTFormatter::Convention::WKT2_2019));
        std::cerr << "ConcatenatedOperation::create()" << std::endl;
        std::cerr << op->exportToWKT(f.get()) << std::endl;
    }
#endif
    return op;
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress

// ---------------------------------------------------------------------------

void ConcatenatedOperation::fixStepsDirection(
    const crs::CRSNNPtr &concatOpSourceCRS,
    const crs::CRSNNPtr &concatOpTargetCRS,
    std::vector<CoordinateOperationNNPtr> &operationsInOut) {

    // Set of heuristics to assign CRS to steps, and possibly reverse them.

    const auto isGeographic = [](const crs::CRS *crs) -> bool {
        return dynamic_cast<const crs::GeographicCRS *>(crs) != nullptr;
    };

    const auto isGeocentric = [](const crs::CRS *crs) -> bool {
        auto geodCRS = dynamic_cast<const crs::GeodeticCRS *>(crs);
        if (geodCRS && geodCRS->coordinateSystem()->axisList().size() == 3)
            return true;
        return false;
    };

    for (size_t i = 0; i < operationsInOut.size(); ++i) {
        auto &op = operationsInOut[i];
        auto l_sourceCRS = op->sourceCRS();
        auto l_targetCRS = op->targetCRS();
        auto conv = dynamic_cast<const Conversion *>(op.get());
        if (conv && i == 0 && !l_sourceCRS && !l_targetCRS) {
            auto derivedCRS =
                dynamic_cast<const crs::DerivedCRS *>(concatOpSourceCRS.get());
            if (derivedCRS) {
                if (i + 1 < operationsInOut.size()) {
                    // use the sourceCRS of the next operation as our target CRS
                    l_targetCRS = operationsInOut[i + 1]->sourceCRS();
                    // except if it looks like the next operation should
                    // actually be reversed !!!
                    if (l_targetCRS &&
                        !compareStepCRS(l_targetCRS.get(),
                                        derivedCRS->baseCRS().get()) &&
                        operationsInOut[i + 1]->targetCRS() &&
                        compareStepCRS(
                            operationsInOut[i + 1]->targetCRS().get(),
                            derivedCRS->baseCRS().get())) {
                        l_targetCRS = operationsInOut[i + 1]->targetCRS();
                    }
                }
                if (!l_targetCRS) {
                    l_targetCRS = derivedCRS->baseCRS().as_nullable();
                }
                auto invConv =
                    util::nn_dynamic_pointer_cast<InverseConversion>(op);
                auto nn_targetCRS = NN_NO_CHECK(l_targetCRS);
                if (invConv) {
                    invConv->inverse()->setCRSs(nn_targetCRS, concatOpSourceCRS,
                                                nullptr);
                    op->setCRSs(concatOpSourceCRS, nn_targetCRS, nullptr);
                } else {
                    op->setCRSs(nn_targetCRS, concatOpSourceCRS, nullptr);
                    op = op->inverse();
                }
            } else if (i + 1 < operationsInOut.size()) {
                /* coverity[copy_paste_error] */
                l_targetCRS = operationsInOut[i + 1]->sourceCRS();
                if (l_targetCRS) {
                    op->setCRSs(concatOpSourceCRS, NN_NO_CHECK(l_targetCRS),
                                nullptr);
                }
            }
        } else if (conv && i + 1 == operationsInOut.size() && !l_sourceCRS &&
                   !l_targetCRS) {
            auto derivedCRS =
                dynamic_cast<const crs::DerivedCRS *>(concatOpTargetCRS.get());
            if (derivedCRS) {
                if (i >= 1) {
                    // use the sourceCRS of the previous operation as our source
                    // CRS
                    l_sourceCRS = operationsInOut[i - 1]->targetCRS();
                    // except if it looks like the previous operation should
                    // actually be reversed !!!
                    if (l_sourceCRS &&
                        !compareStepCRS(l_sourceCRS.get(),
                                        derivedCRS->baseCRS().get()) &&
                        operationsInOut[i - 1]->sourceCRS() &&
                        compareStepCRS(
                            operationsInOut[i - 1]->sourceCRS().get(),
                            derivedCRS->baseCRS().get())) {
                        l_targetCRS = operationsInOut[i - 1]->sourceCRS();
                    }
                }
                if (!l_sourceCRS) {
                    l_sourceCRS = derivedCRS->baseCRS().as_nullable();
                }
                op->setCRSs(NN_NO_CHECK(l_sourceCRS), concatOpTargetCRS,
                            nullptr);
            } else if (i >= 1) {
                l_sourceCRS = operationsInOut[i - 1]->targetCRS();
                if (l_sourceCRS) {
                    derivedCRS = dynamic_cast<const crs::DerivedCRS *>(
                        l_sourceCRS.get());
                    if (conv->isEquivalentTo(
                            derivedCRS->derivingConversion().get(),
                            util::IComparable::Criterion::EQUIVALENT)) {
                        op->setCRSs(concatOpTargetCRS, NN_NO_CHECK(l_sourceCRS),
                                    nullptr);
                        op = op->inverse();
                    }
                    op->setCRSs(NN_NO_CHECK(l_sourceCRS), concatOpTargetCRS,
                                nullptr);
                }
            }
        } else if (conv && i > 0 && i < operationsInOut.size() - 1) {
            // For an intermediate conversion, use the target CRS of the
            // previous step and the source CRS of the next step
            l_sourceCRS = operationsInOut[i - 1]->targetCRS();
            l_targetCRS = operationsInOut[i + 1]->sourceCRS();
            if (l_sourceCRS && l_targetCRS) {
                op->setCRSs(NN_NO_CHECK(l_sourceCRS), NN_NO_CHECK(l_targetCRS),
                            nullptr);
            }
        } else if (!conv && l_sourceCRS && l_targetCRS) {

            // Transformations might be mentioned in their forward directions,
            // whereas we should instead use the reverse path.
            auto prevOpTarget = (i == 0) ? concatOpSourceCRS.as_nullable()
                                         : operationsInOut[i - 1]->targetCRS();
            if (compareStepCRS(l_sourceCRS.get(), prevOpTarget.get())) {
                // do nothing
            } else if (compareStepCRS(l_targetCRS.get(), prevOpTarget.get())) {
                op = op->inverse();
            }
            // Below is needed for EPSG:9103 which chains NAD83(2011) geographic
            // 2D with NAD83(2011) geocentric
            else if (l_sourceCRS->nameStr() == prevOpTarget->nameStr() &&
                     ((isGeographic(l_sourceCRS.get()) &&
                       isGeocentric(prevOpTarget.get())) ||
                      (isGeocentric(l_sourceCRS.get()) &&
                       isGeographic(prevOpTarget.get())))) {
                auto newOp(Conversion::createGeographicGeocentric(
                    NN_NO_CHECK(prevOpTarget), NN_NO_CHECK(l_sourceCRS)));
                operationsInOut.insert(operationsInOut.begin() + i, newOp);
            } else if (l_targetCRS->nameStr() == prevOpTarget->nameStr() &&
                       ((isGeographic(l_targetCRS.get()) &&
                         isGeocentric(prevOpTarget.get())) ||
                        (isGeocentric(l_targetCRS.get()) &&
                         isGeographic(prevOpTarget.get())))) {
                auto newOp(Conversion::createGeographicGeocentric(
                    NN_NO_CHECK(prevOpTarget), NN_NO_CHECK(l_targetCRS)));
                operationsInOut.insert(operationsInOut.begin() + i, newOp);
            }
        }
    }

    if (!operationsInOut.empty()) {
        auto l_sourceCRS = operationsInOut.front()->sourceCRS();
        if (l_sourceCRS &&
            !compareStepCRS(l_sourceCRS.get(), concatOpSourceCRS.get())) {
            throw InvalidOperation("The source CRS of the first step of "
                                   "concatenated operation is not the same "
                                   "as the source CRS of the concatenated "
                                   "operation itself");
        }

        auto l_targetCRS = operationsInOut.back()->targetCRS();
        if (l_targetCRS &&
            !compareStepCRS(l_targetCRS.get(), concatOpTargetCRS.get())) {
            if (l_targetCRS->nameStr() == concatOpTargetCRS->nameStr() &&
                ((isGeographic(l_targetCRS.get()) &&
                  isGeocentric(concatOpTargetCRS.get())) ||
                 (isGeocentric(l_targetCRS.get()) &&
                  isGeographic(concatOpTargetCRS.get())))) {
                auto newOp(Conversion::createGeographicGeocentric(
                    NN_NO_CHECK(l_targetCRS), concatOpTargetCRS));
                operationsInOut.push_back(newOp);
            } else {
                throw InvalidOperation("The target CRS of the last step of "
                                       "concatenated operation is not the same "
                                       "as the target CRS of the concatenated "
                                       "operation itself");
            }
        }
    }
}
//! @endcond

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
static std::string computeConcatenatedName(
    const std::vector<CoordinateOperationNNPtr> &flattenOps) {
    std::string name;
    for (const auto &subOp : flattenOps) {
        if (!name.empty()) {
            name += " + ";
        }
        const auto &l_name = subOp->nameStr();
        if (l_name.empty()) {
            name += "unnamed";
        } else {
            name += l_name;
        }
    }
    return name;
}
//! @endcond

// ---------------------------------------------------------------------------

/** \brief Instantiate a ConcatenatedOperation, or return a single
 * coordinate
 * operation.
 *
 * This computes its accuracy from the sum of its member operations, its
 * extent
 *
 * @param operationsIn Vector of the CoordinateOperation steps.
 * @param checkExtent Whether we should check the non-emptyness of the
 * intersection
 * of the extents of the operations
 * @throws InvalidOperation
 */
CoordinateOperationNNPtr ConcatenatedOperation::createComputeMetadata(
    const std::vector<CoordinateOperationNNPtr> &operationsIn,
    bool checkExtent) // throw InvalidOperation
{
    util::PropertyMap properties;

    if (operationsIn.size() == 1) {
        return operationsIn[0];
    }

    std::vector<CoordinateOperationNNPtr> flattenOps;
    bool hasBallparkTransformation = false;
    for (const auto &subOp : operationsIn) {
        hasBallparkTransformation |= subOp->hasBallparkTransformation();
        auto subOpConcat =
            dynamic_cast<const ConcatenatedOperation *>(subOp.get());
        if (subOpConcat) {
            auto subOps = subOpConcat->operations();
            for (const auto &subSubOp : subOps) {
                flattenOps.emplace_back(subSubOp);
            }
        } else {
            flattenOps.emplace_back(subOp);
        }
    }
    if (flattenOps.size() == 1) {
        return flattenOps[0];
    }

    properties.set(common::IdentifiedObject::NAME_KEY,
                   computeConcatenatedName(flattenOps));

    bool emptyIntersection = false;
    auto extent = getExtent(flattenOps, false, emptyIntersection);
    if (checkExtent && emptyIntersection) {
        std::string msg(
            "empty intersection of area of validity of concatenated "
            "operations");
        throw InvalidOperationEmptyIntersection(msg);
    }
    if (extent) {
        properties.set(common::ObjectUsage::DOMAIN_OF_VALIDITY_KEY,
                       NN_NO_CHECK(extent));
    }

    std::vector<metadata::PositionalAccuracyNNPtr> accuracies;
    const double accuracy = getAccuracy(flattenOps);
    if (accuracy >= 0.0) {
        accuracies.emplace_back(
            metadata::PositionalAccuracy::create(toString(accuracy)));
    }

    auto op = create(properties, flattenOps, accuracies);
    op->setHasBallparkTransformation(hasBallparkTransformation);
    op->d->computedName_ = true;
    return op;
}

// ---------------------------------------------------------------------------

CoordinateOperationNNPtr ConcatenatedOperation::inverse() const {
    std::vector<CoordinateOperationNNPtr> inversedOperations;
    auto l_operations = operations();
    inversedOperations.reserve(l_operations.size());
    for (const auto &operation : l_operations) {
        inversedOperations.emplace_back(operation->inverse());
    }
    std::reverse(inversedOperations.begin(), inversedOperations.end());

    auto properties = createPropertiesForInverse(this, false, false);
    if (d->computedName_) {
        properties.set(common::IdentifiedObject::NAME_KEY,
                       computeConcatenatedName(inversedOperations));
    }

    auto op =
        create(properties, inversedOperations, coordinateOperationAccuracies());
    op->d->computedName_ = d->computedName_;
    op->setHasBallparkTransformation(hasBallparkTransformation());
    return op;
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
void ConcatenatedOperation::_exportToWKT(io::WKTFormatter *formatter) const {
    const bool isWKT2 = formatter->version() == io::WKTFormatter::Version::WKT2;
    if (!isWKT2 || !formatter->use2019Keywords()) {
        throw io::FormattingException(
            "Transformation can only be exported to WKT2:2019");
    }

    formatter->startNode(io::WKTConstants::CONCATENATEDOPERATION,
                         !identifiers().empty());
    formatter->addQuotedString(nameStr());

    if (formatter->use2019Keywords()) {
        const auto &version = operationVersion();
        if (version.has_value()) {
            formatter->startNode(io::WKTConstants::VERSION, false);
            formatter->addQuotedString(*version);
            formatter->endNode();
        }
    }

    exportSourceCRSAndTargetCRSToWKT(this, formatter);

    const bool canExportOperationId =
        !(formatter->idOnTopLevelOnly() && formatter->topLevelHasId());

    const bool hasDomains = !domains().empty();
    if (hasDomains) {
        formatter->pushDisableUsage();
    }

    for (const auto &operation : operations()) {
        formatter->startNode(io::WKTConstants::STEP, false);
        if (canExportOperationId && !operation->identifiers().empty()) {
            // fake that top node has no id, so that the operation id is
            // considered
            formatter->pushHasId(false);
            operation->_exportToWKT(formatter);
            formatter->popHasId();
        } else {
            operation->_exportToWKT(formatter);
        }
        formatter->endNode();
    }

    if (hasDomains) {
        formatter->popDisableUsage();
    }

    ObjectUsage::baseExportToWKT(formatter);
    formatter->endNode();
}
//! @endcond

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
void ConcatenatedOperation::_exportToJSON(
    io::JSONFormatter *formatter) const // throw(FormattingException)
{
    auto writer = formatter->writer();
    auto objectContext(formatter->MakeObjectContext("ConcatenatedOperation",
                                                    !identifiers().empty()));

    writer->AddObjKey("name");
    auto l_name = nameStr();
    if (l_name.empty()) {
        writer->Add("unnamed");
    } else {
        writer->Add(l_name);
    }

    writer->AddObjKey("source_crs");
    formatter->setAllowIDInImmediateChild();
    sourceCRS()->_exportToJSON(formatter);

    writer->AddObjKey("target_crs");
    formatter->setAllowIDInImmediateChild();
    targetCRS()->_exportToJSON(formatter);

    writer->AddObjKey("steps");
    {
        auto parametersContext(writer->MakeArrayContext(false));
        for (const auto &operation : operations()) {
            formatter->setAllowIDInImmediateChild();
            operation->_exportToJSON(formatter);
        }
    }

    ObjectUsage::baseExportToJSON(formatter);
}

//! @endcond

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
CoordinateOperationNNPtr ConcatenatedOperation::_shallowClone() const {
    auto op =
        ConcatenatedOperation::nn_make_shared<ConcatenatedOperation>(*this);
    std::vector<CoordinateOperationNNPtr> ops;
    for (const auto &subOp : d->operations_) {
        ops.emplace_back(subOp->shallowClone());
    }
    op->d->operations_ = ops;
    op->assignSelf(op);
    op->setCRSs(this, false);
    return util::nn_static_pointer_cast<CoordinateOperation>(op);
}
//! @endcond

// ---------------------------------------------------------------------------

void ConcatenatedOperation::_exportToPROJString(
    io::PROJStringFormatter *formatter) const // throw(FormattingException)
{
    for (const auto &operation : operations()) {
        operation->_exportToPROJString(formatter);
    }
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
bool ConcatenatedOperation::_isEquivalentTo(
    const util::IComparable *other, util::IComparable::Criterion criterion,
    const io::DatabaseContextPtr &dbContext) const {
    auto otherCO = dynamic_cast<const ConcatenatedOperation *>(other);
    if (otherCO == nullptr ||
        (criterion == util::IComparable::Criterion::STRICT &&
         !ObjectUsage::_isEquivalentTo(other, criterion, dbContext))) {
        return false;
    }
    const auto &steps = operations();
    const auto &otherSteps = otherCO->operations();
    if (steps.size() != otherSteps.size()) {
        return false;
    }
    for (size_t i = 0; i < steps.size(); i++) {
        if (!steps[i]->_isEquivalentTo(otherSteps[i].get(), criterion,
                                       dbContext)) {
            return false;
        }
    }
    return true;
}
//! @endcond

// ---------------------------------------------------------------------------

std::set<GridDescription> ConcatenatedOperation::gridsNeeded(
    const io::DatabaseContextPtr &databaseContext,
    bool considerKnownGridsAsAvailable) const {
    std::set<GridDescription> res;
    for (const auto &operation : operations()) {
        const auto l_gridsNeeded = operation->gridsNeeded(
            databaseContext, considerKnownGridsAsAvailable);
        for (const auto &gridDesc : l_gridsNeeded) {
            res.insert(gridDesc);
        }
    }
    return res;
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
struct CoordinateOperationContext::Private {
    io::AuthorityFactoryPtr authorityFactory_{};
    metadata::ExtentPtr extent_{};
    double accuracy_ = 0.0;
    SourceTargetCRSExtentUse sourceAndTargetCRSExtentUse_ =
        CoordinateOperationContext::SourceTargetCRSExtentUse::SMALLEST;
    SpatialCriterion spatialCriterion_ =
        CoordinateOperationContext::SpatialCriterion::STRICT_CONTAINMENT;
    bool usePROJNames_ = true;
    GridAvailabilityUse gridAvailabilityUse_ =
        GridAvailabilityUse::USE_FOR_SORTING;
    IntermediateCRSUse allowUseIntermediateCRS_ = CoordinateOperationContext::
        IntermediateCRSUse::IF_NO_DIRECT_TRANSFORMATION;
    std::vector<std::pair<std::string, std::string>>
        intermediateCRSAuthCodes_{};
    bool discardSuperseded_ = true;
    bool allowBallpark_ = true;
};
//! @endcond

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
CoordinateOperationContext::~CoordinateOperationContext() = default;
//! @endcond

// ---------------------------------------------------------------------------

CoordinateOperationContext::CoordinateOperationContext()
    : d(internal::make_unique<Private>()) {}

// ---------------------------------------------------------------------------

/** \brief Return the authority factory, or null */
const io::AuthorityFactoryPtr &
CoordinateOperationContext::getAuthorityFactory() const {
    return d->authorityFactory_;
}

// ---------------------------------------------------------------------------

/** \brief Return the desired area of interest, or null */
const metadata::ExtentPtr &
CoordinateOperationContext::getAreaOfInterest() const {
    return d->extent_;
}

// ---------------------------------------------------------------------------

/** \brief Set the desired area of interest, or null */
void CoordinateOperationContext::setAreaOfInterest(
    const metadata::ExtentPtr &extent) {
    d->extent_ = extent;
}

// ---------------------------------------------------------------------------

/** \brief Return the desired accuracy (in metre), or 0 */
double CoordinateOperationContext::getDesiredAccuracy() const {
    return d->accuracy_;
}

// ---------------------------------------------------------------------------

/** \brief Set the desired accuracy (in metre), or 0 */
void CoordinateOperationContext::setDesiredAccuracy(double accuracy) {
    d->accuracy_ = accuracy;
}

// ---------------------------------------------------------------------------

/** \brief Return whether ballpark transformations are allowed */
bool CoordinateOperationContext::getAllowBallparkTransformations() const {
    return d->allowBallpark_;
}

// ---------------------------------------------------------------------------

/** \brief Set whether ballpark transformations are allowed */
void CoordinateOperationContext::setAllowBallparkTransformations(bool allow) {
    d->allowBallpark_ = allow;
}

// ---------------------------------------------------------------------------

/** \brief Set how source and target CRS extent should be used
 * when considering if a transformation can be used (only takes effect if
 * no area of interest is explicitly defined).
 *
 * The default is
 * CoordinateOperationContext::SourceTargetCRSExtentUse::SMALLEST.
 */
void CoordinateOperationContext::setSourceAndTargetCRSExtentUse(
    SourceTargetCRSExtentUse use) {
    d->sourceAndTargetCRSExtentUse_ = use;
}

// ---------------------------------------------------------------------------

/** \brief Return how source and target CRS extent should be used
 * when considering if a transformation can be used (only takes effect if
 * no area of interest is explicitly defined).
 *
 * The default is
 * CoordinateOperationContext::SourceTargetCRSExtentUse::SMALLEST.
 */
CoordinateOperationContext::SourceTargetCRSExtentUse
CoordinateOperationContext::getSourceAndTargetCRSExtentUse() const {
    return d->sourceAndTargetCRSExtentUse_;
}

// ---------------------------------------------------------------------------

/** \brief Set the spatial criterion to use when comparing the area of
 * validity
 * of coordinate operations with the area of interest / area of validity of
 * source and target CRS.
 *
 * The default is STRICT_CONTAINMENT.
 */
void CoordinateOperationContext::setSpatialCriterion(
    SpatialCriterion criterion) {
    d->spatialCriterion_ = criterion;
}

// ---------------------------------------------------------------------------

/** \brief Return the spatial criterion to use when comparing the area of
 * validity
 * of coordinate operations with the area of interest / area of validity of
 * source and target CRS.
 *
 * The default is STRICT_CONTAINMENT.
 */
CoordinateOperationContext::SpatialCriterion
CoordinateOperationContext::getSpatialCriterion() const {
    return d->spatialCriterion_;
}

// ---------------------------------------------------------------------------

/** \brief Set whether PROJ alternative grid names should be substituted to
 * the official authority names.
 *
 * This only has effect is an authority factory with a non-null database context
 * has been attached to this context.
 *
 * If set to false, it is still possible to
 * obtain later the substitution by using io::PROJStringFormatter::create()
 * with a non-null database context.
 *
 * The default is true.
 */
void CoordinateOperationContext::setUsePROJAlternativeGridNames(
    bool usePROJNames) {
    d->usePROJNames_ = usePROJNames;
}

// ---------------------------------------------------------------------------

/** \brief Return whether PROJ alternative grid names should be substituted to
 * the official authority names.
 *
 * The default is true.
 */
bool CoordinateOperationContext::getUsePROJAlternativeGridNames() const {
    return d->usePROJNames_;
}

// ---------------------------------------------------------------------------

/** \brief Return whether transformations that are superseded (but not
 * deprecated)
 * should be discarded.
 *
 * The default is true.
 */
bool CoordinateOperationContext::getDiscardSuperseded() const {
    return d->discardSuperseded_;
}

// ---------------------------------------------------------------------------

/** \brief Set whether transformations that are superseded (but not deprecated)
 * should be discarded.
 *
 * The default is true.
 */
void CoordinateOperationContext::setDiscardSuperseded(bool discard) {
    d->discardSuperseded_ = discard;
}

// ---------------------------------------------------------------------------

/** \brief Set how grid availability is used.
 *
 * The default is USE_FOR_SORTING.
 */
void CoordinateOperationContext::setGridAvailabilityUse(
    GridAvailabilityUse use) {
    d->gridAvailabilityUse_ = use;
}

// ---------------------------------------------------------------------------

/** \brief Return how grid availability is used.
 *
 * The default is USE_FOR_SORTING.
 */
CoordinateOperationContext::GridAvailabilityUse
CoordinateOperationContext::getGridAvailabilityUse() const {
    return d->gridAvailabilityUse_;
}

// ---------------------------------------------------------------------------

/** \brief Set whether an intermediate pivot CRS can be used for researching
 * coordinate operations between a source and target CRS.
 *
 * Concretely if in the database there is an operation from A to C
 * (or C to A), and another one from C to B (or B to C), but no direct
 * operation between A and B, setting this parameter to
 * ALWAYS/IF_NO_DIRECT_TRANSFORMATION, allow chaining both operations.
 *
 * The current implementation is limited to researching one intermediate
 * step.
 *
 * By default, with the IF_NO_DIRECT_TRANSFORMATION stratgey, all potential
 * C candidates will be used if there is no direct transformation.
 */
void CoordinateOperationContext::setAllowUseIntermediateCRS(
    IntermediateCRSUse use) {
    d->allowUseIntermediateCRS_ = use;
}

// ---------------------------------------------------------------------------

/** \brief Return whether an intermediate pivot CRS can be used for researching
 * coordinate operations between a source and target CRS.
 *
 * Concretely if in the database there is an operation from A to C
 * (or C to A), and another one from C to B (or B to C), but no direct
 * operation between A and B, setting this parameter to
 * ALWAYS/IF_NO_DIRECT_TRANSFORMATION, allow chaining both operations.
 *
 * The default is IF_NO_DIRECT_TRANSFORMATION.
 */
CoordinateOperationContext::IntermediateCRSUse
CoordinateOperationContext::getAllowUseIntermediateCRS() const {
    return d->allowUseIntermediateCRS_;
}

// ---------------------------------------------------------------------------

/** \brief Restrict the potential pivot CRSs that can be used when trying to
 * build a coordinate operation between two CRS that have no direct operation.
 *
 * @param intermediateCRSAuthCodes a vector of (auth_name, code) that can be
 * used as potential pivot RS
 */
void CoordinateOperationContext::setIntermediateCRS(
    const std::vector<std::pair<std::string, std::string>>
        &intermediateCRSAuthCodes) {
    d->intermediateCRSAuthCodes_ = intermediateCRSAuthCodes;
}

// ---------------------------------------------------------------------------

/** \brief Return the potential pivot CRSs that can be used when trying to
 * build a coordinate operation between two CRS that have no direct operation.
 *
 */
const std::vector<std::pair<std::string, std::string>> &
CoordinateOperationContext::getIntermediateCRS() const {
    return d->intermediateCRSAuthCodes_;
}

// ---------------------------------------------------------------------------

/** \brief Creates a context for a coordinate operation.
 *
 * If a non null authorityFactory is provided, the resulting context should
 * not be used simultaneously by more than one thread.
 *
 * If authorityFactory->getAuthority() is the empty string, then coordinate
 * operations from any authority will be searched, with the restrictions set
 * in the authority_to_authority_preference database table.
 * If authorityFactory->getAuthority() is set to "any", then coordinate
 * operations from any authority will be searched
 * If authorityFactory->getAuthority() is a non-empty string different of "any",
 * then coordinate operatiosn will be searched only in that authority namespace.
 *
 * @param authorityFactory Authority factory, or null if no database lookup
 * is allowed.
 * Use io::authorityFactory::create(context, std::string()) to allow all
 * authorities to be used.
 * @param extent Area of interest, or null if none is known.
 * @param accuracy Maximum allowed accuracy in metre, as specified in or
 * 0 to get best accuracy.
 * @return a new context.
 */
CoordinateOperationContextNNPtr CoordinateOperationContext::create(
    const io::AuthorityFactoryPtr &authorityFactory,
    const metadata::ExtentPtr &extent, double accuracy) {
    auto ctxt = NN_NO_CHECK(
        CoordinateOperationContext::make_unique<CoordinateOperationContext>());
    ctxt->d->authorityFactory_ = authorityFactory;
    ctxt->d->extent_ = extent;
    ctxt->d->accuracy_ = accuracy;
    return ctxt;
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
struct CoordinateOperationFactory::Private {

    struct Context {
        // This is the extent of the source CRS and target CRS of the initial
        // CoordinateOperationFactory::createOperations() public call, not
        // necessarily the ones of intermediate
        // CoordinateOperationFactory::Private::createOperations() calls.
        // This is used to compare transformations area of use against the
        // area of use of the source & target CRS.
        const metadata::ExtentPtr &extent1;
        const metadata::ExtentPtr &extent2;
        const CoordinateOperationContextNNPtr &context;
        bool inCreateOperationsWithDatumPivotAntiRecursion = false;
        bool inCreateOperationsGeogToVertWithAlternativeGeog = false;
        bool inCreateOperationsGeogToVertWithIntermediateVert = false;
        bool skipHorizontalTransformation = false;
        std::map<std::pair<io::AuthorityFactory::ObjectType, std::string>,
                 std::list<std::pair<std::string, std::string>>>
            cacheNameToCRS{};

        Context(const metadata::ExtentPtr &extent1In,
                const metadata::ExtentPtr &extent2In,
                const CoordinateOperationContextNNPtr &contextIn)
            : extent1(extent1In), extent2(extent2In), context(contextIn) {}
    };

    static std::vector<CoordinateOperationNNPtr>
    createOperations(const crs::CRSNNPtr &sourceCRS,
                     const crs::CRSNNPtr &targetCRS, Context &context);

  private:
    static constexpr bool disallowEmptyIntersection = true;

    static void
    buildCRSIds(const crs::CRSNNPtr &crs, Private::Context &context,
                std::list<std::pair<std::string, std::string>> &ids);

    static std::vector<CoordinateOperationNNPtr> findOpsInRegistryDirect(
        const crs::CRSNNPtr &sourceCRS, const crs::CRSNNPtr &targetCRS,
        Private::Context &context, bool &resNonEmptyBeforeFiltering);

    static std::vector<CoordinateOperationNNPtr>
    findOpsInRegistryDirectTo(const crs::CRSNNPtr &targetCRS,
                              Private::Context &context);

    static std::vector<CoordinateOperationNNPtr>
    findsOpsInRegistryWithIntermediate(
        const crs::CRSNNPtr &sourceCRS, const crs::CRSNNPtr &targetCRS,
        Private::Context &context,
        bool useCreateBetweenGeodeticCRSWithDatumBasedIntermediates);

    static void createOperationsFromProj4Ext(
        const crs::CRSNNPtr &sourceCRS, const crs::CRSNNPtr &targetCRS,
        const crs::BoundCRS *boundSrc, const crs::BoundCRS *boundDst,
        std::vector<CoordinateOperationNNPtr> &res);

    static bool createOperationsFromDatabase(
        const crs::CRSNNPtr &sourceCRS, const crs::CRSNNPtr &targetCRS,
        Private::Context &context, const crs::GeodeticCRS *geodSrc,
        const crs::GeodeticCRS *geodDst, const crs::GeographicCRS *geogSrc,
        const crs::GeographicCRS *geogDst, const crs::VerticalCRS *vertSrc,
        const crs::VerticalCRS *vertDst,
        std::vector<CoordinateOperationNNPtr> &res);

    static std::vector<CoordinateOperationNNPtr>
    createOperationsGeogToVertFromGeoid(const crs::CRSNNPtr &sourceCRS,
                                        const crs::CRSNNPtr &targetCRS,
                                        const crs::VerticalCRS *vertDst,
                                        Context &context);

    static std::vector<CoordinateOperationNNPtr>
    createOperationsGeogToVertWithIntermediateVert(
        const crs::CRSNNPtr &sourceCRS, const crs::CRSNNPtr &targetCRS,
        const crs::VerticalCRS *vertDst, Context &context);

    static std::vector<CoordinateOperationNNPtr>
    createOperationsGeogToVertWithAlternativeGeog(
        const crs::CRSNNPtr &sourceCRS, const crs::CRSNNPtr &targetCRS,
        Context &context);

    static void createOperationsFromDatabaseWithVertCRS(
        const crs::CRSNNPtr &sourceCRS, const crs::CRSNNPtr &targetCRS,
        Private::Context &context, const crs::GeographicCRS *geogSrc,
        const crs::GeographicCRS *geogDst, const crs::VerticalCRS *vertSrc,
        const crs::VerticalCRS *vertDst,
        std::vector<CoordinateOperationNNPtr> &res);

    static void createOperationsGeodToGeod(
        const crs::CRSNNPtr &sourceCRS, const crs::CRSNNPtr &targetCRS,
        Private::Context &context, const crs::GeodeticCRS *geodSrc,
        const crs::GeodeticCRS *geodDst,
        std::vector<CoordinateOperationNNPtr> &res);

    static void createOperationsDerivedTo(
        const crs::CRSNNPtr &sourceCRS, const crs::CRSNNPtr &targetCRS,
        Private::Context &context, const crs::DerivedCRS *derivedSrc,
        std::vector<CoordinateOperationNNPtr> &res);

    static void createOperationsBoundToGeog(
        const crs::CRSNNPtr &sourceCRS, const crs::CRSNNPtr &targetCRS,
        Private::Context &context, const crs::BoundCRS *boundSrc,
        const crs::GeographicCRS *geogDst,
        std::vector<CoordinateOperationNNPtr> &res);

    static void createOperationsBoundToVert(
        const crs::CRSNNPtr &sourceCRS, const crs::CRSNNPtr &targetCRS,
        Private::Context &context, const crs::BoundCRS *boundSrc,
        const crs::VerticalCRS *vertDst,
        std::vector<CoordinateOperationNNPtr> &res);

    static void createOperationsVertToVert(
        const crs::CRSNNPtr &sourceCRS, const crs::CRSNNPtr &targetCRS,
        Private::Context &context, const crs::VerticalCRS *vertSrc,
        const crs::VerticalCRS *vertDst,
        std::vector<CoordinateOperationNNPtr> &res);

    static void createOperationsVertToGeog(
        const crs::CRSNNPtr &sourceCRS, const crs::CRSNNPtr &targetCRS,
        Private::Context &context, const crs::VerticalCRS *vertSrc,
        const crs::GeographicCRS *geogDst,
        std::vector<CoordinateOperationNNPtr> &res);

    static void createOperationsVertToGeogBallpark(
        const crs::CRSNNPtr &sourceCRS, const crs::CRSNNPtr &targetCRS,
        Private::Context &context, const crs::VerticalCRS *vertSrc,
        const crs::GeographicCRS *geogDst,
        std::vector<CoordinateOperationNNPtr> &res);

    static void createOperationsBoundToBound(
        const crs::CRSNNPtr &sourceCRS, const crs::CRSNNPtr &targetCRS,
        Private::Context &context, const crs::BoundCRS *boundSrc,
        const crs::BoundCRS *boundDst,
        std::vector<CoordinateOperationNNPtr> &res);

    static void createOperationsCompoundToGeog(
        const crs::CRSNNPtr &sourceCRS, const crs::CRSNNPtr &targetCRS,
        Private::Context &context, const crs::CompoundCRS *compoundSrc,
        const crs::GeographicCRS *geogDst,
        std::vector<CoordinateOperationNNPtr> &res);

    static void createOperationsToGeod(
        const crs::CRSNNPtr &sourceCRS, const crs::CRSNNPtr &targetCRS,
        Private::Context &context, const crs::GeodeticCRS *geodDst,
        std::vector<CoordinateOperationNNPtr> &res);

    static void createOperationsCompoundToCompound(
        const crs::CRSNNPtr &sourceCRS, const crs::CRSNNPtr &targetCRS,
        Private::Context &context, const crs::CompoundCRS *compoundSrc,
        const crs::CompoundCRS *compoundDst,
        std::vector<CoordinateOperationNNPtr> &res);

    static void createOperationsBoundToCompound(
        const crs::CRSNNPtr &sourceCRS, const crs::CRSNNPtr &targetCRS,
        Private::Context &context, const crs::BoundCRS *boundSrc,
        const crs::CompoundCRS *compoundDst,
        std::vector<CoordinateOperationNNPtr> &res);

    static std::vector<CoordinateOperationNNPtr> createOperationsGeogToGeog(
        std::vector<CoordinateOperationNNPtr> &res,
        const crs::CRSNNPtr &sourceCRS, const crs::CRSNNPtr &targetCRS,
        Private::Context &context, const crs::GeographicCRS *geogSrc,
        const crs::GeographicCRS *geogDst);

    static void createOperationsWithDatumPivot(
        std::vector<CoordinateOperationNNPtr> &res,
        const crs::CRSNNPtr &sourceCRS, const crs::CRSNNPtr &targetCRS,
        const crs::GeodeticCRS *geodSrc, const crs::GeodeticCRS *geodDst,
        Context &context);

    static bool
    hasPerfectAccuracyResult(const std::vector<CoordinateOperationNNPtr> &res,
                             const Context &context);

    static void setCRSs(CoordinateOperation *co, const crs::CRSNNPtr &sourceCRS,
                        const crs::CRSNNPtr &targetCRS);
};
//! @endcond

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
CoordinateOperationFactory::~CoordinateOperationFactory() = default;
//! @endcond

// ---------------------------------------------------------------------------

CoordinateOperationFactory::CoordinateOperationFactory() : d(nullptr) {}

// ---------------------------------------------------------------------------

/** \brief Find a CoordinateOperation from sourceCRS to targetCRS.
 *
 * This is a helper of createOperations(), using a coordinate operation
 * context
 * with no authority factory (so no catalog searching is done), no desired
 * accuracy and no area of interest.
 * This returns the first operation of the result set of createOperations(),
 * or null if none found.
 *
 * @param sourceCRS source CRS.
 * @param targetCRS source CRS.
 * @return a CoordinateOperation or nullptr.
 */
CoordinateOperationPtr CoordinateOperationFactory::createOperation(
    const crs::CRSNNPtr &sourceCRS, const crs::CRSNNPtr &targetCRS) const {
    auto res = createOperations(
        sourceCRS, targetCRS,
        CoordinateOperationContext::create(nullptr, nullptr, 0.0));
    if (!res.empty()) {
        return res[0];
    }
    return nullptr;
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress

// ---------------------------------------------------------------------------

struct PrecomputedOpCharacteristics {
    double area_{};
    double accuracy_{};
    bool isPROJExportable_ = false;
    bool hasGrids_ = false;
    bool gridsAvailable_ = false;
    bool gridsKnown_ = false;
    size_t stepCount_ = 0;
    bool isApprox_ = false;
    bool hasBallparkVertical_ = false;
    bool isNullTransformation_ = false;

    PrecomputedOpCharacteristics() = default;
    PrecomputedOpCharacteristics(double area, double accuracy,
                                 bool isPROJExportable, bool hasGrids,
                                 bool gridsAvailable, bool gridsKnown,
                                 size_t stepCount, bool isApprox,
                                 bool hasBallparkVertical,
                                 bool isNullTransformation)
        : area_(area), accuracy_(accuracy), isPROJExportable_(isPROJExportable),
          hasGrids_(hasGrids), gridsAvailable_(gridsAvailable),
          gridsKnown_(gridsKnown), stepCount_(stepCount), isApprox_(isApprox),
          hasBallparkVertical_(hasBallparkVertical),
          isNullTransformation_(isNullTransformation) {}
};

// ---------------------------------------------------------------------------

// We could have used a lambda instead of this old-school way, but
// filterAndSort() is already huge.
struct SortFunction {

    const std::map<CoordinateOperation *, PrecomputedOpCharacteristics> &map;

    explicit SortFunction(const std::map<CoordinateOperation *,
                                         PrecomputedOpCharacteristics> &mapIn)
        : map(mapIn) {}

    // Sorting function
    // Return true if a < b
    bool compare(const CoordinateOperationNNPtr &a,
                 const CoordinateOperationNNPtr &b) const {
        auto iterA = map.find(a.get());
        assert(iterA != map.end());
        auto iterB = map.find(b.get());
        assert(iterB != map.end());

        // CAUTION: the order of the comparisons is extremely important
        // to get the intended result.

        if (iterA->second.isPROJExportable_ &&
            !iterB->second.isPROJExportable_) {
            return true;
        }
        if (!iterA->second.isPROJExportable_ &&
            iterB->second.isPROJExportable_) {
            return false;
        }

        if (!iterA->second.isApprox_ && iterB->second.isApprox_) {
            return true;
        }
        if (iterA->second.isApprox_ && !iterB->second.isApprox_) {
            return false;
        }

        if (!iterA->second.hasBallparkVertical_ &&
            iterB->second.hasBallparkVertical_) {
            return true;
        }
        if (iterA->second.hasBallparkVertical_ &&
            !iterB->second.hasBallparkVertical_) {
            return false;
        }

        if (!iterA->second.isNullTransformation_ &&
            iterB->second.isNullTransformation_) {
            return true;
        }
        if (iterA->second.isNullTransformation_ &&
            !iterB->second.isNullTransformation_) {
            return false;
        }

        // Operations where grids are all available go before other
        if (iterA->second.gridsAvailable_ && !iterB->second.gridsAvailable_) {
            return true;
        }
        if (iterB->second.gridsAvailable_ && !iterA->second.gridsAvailable_) {
            return false;
        }

        // Operations where grids are all known in our DB go before other
        if (iterA->second.gridsKnown_ && !iterB->second.gridsKnown_) {
            return true;
        }
        if (iterB->second.gridsKnown_ && !iterA->second.gridsKnown_) {
            return false;
        }

        // Operations with known accuracy go before those with unknown accuracy
        const double accuracyA = iterA->second.accuracy_;
        const double accuracyB = iterB->second.accuracy_;
        if (accuracyA >= 0 && accuracyB < 0) {
            return true;
        }
        if (accuracyB >= 0 && accuracyA < 0) {
            return false;
        }

        if (accuracyA < 0 && accuracyB < 0) {
            // unknown accuracy ? then prefer operations with grids, which
            // are likely to have best practical accuracy
            if (iterA->second.hasGrids_ && !iterB->second.hasGrids_) {
                return true;
            }
            if (!iterA->second.hasGrids_ && iterB->second.hasGrids_) {
                return false;
            }
        }

        // Operations with larger non-zero area of use go before those with
        // lower one
        const double areaA = iterA->second.area_;
        const double areaB = iterB->second.area_;
        if (areaA > 0) {
            if (areaA > areaB) {
                return true;
            }
            if (areaA < areaB) {
                return false;
            }
        } else if (areaB > 0) {
            return false;
        }

        // Operations with better accuracy go before those with worse one
        if (accuracyA >= 0 && accuracyA < accuracyB) {
            return true;
        }
        if (accuracyB >= 0 && accuracyB < accuracyA) {
            return false;
        }

        if (accuracyA >= 0 && accuracyA == accuracyB) {
            // same accuracy ? then prefer operations without grids
            if (!iterA->second.hasGrids_ && iterB->second.hasGrids_) {
                return true;
            }
            if (iterA->second.hasGrids_ && !iterB->second.hasGrids_) {
                return false;
            }
        }

        // The less intermediate steps, the better
        if (iterA->second.stepCount_ < iterB->second.stepCount_) {
            return true;
        }
        if (iterB->second.stepCount_ < iterA->second.stepCount_) {
            return false;
        }

        const auto &a_name = a->nameStr();
        const auto &b_name = b->nameStr();
        // The shorter name, the better ?
        if (a_name.size() < b_name.size()) {
            return true;
        }
        if (b_name.size() < a_name.size()) {
            return false;
        }

        // Arbitrary final criterion. We actually return the greater element
        // first, so that "Amersfoort to WGS 84 (4)" is presented before
        // "Amersfoort to WGS 84 (3)", which is probably a better guess.

        // Except for French NTF (Paris) to NTF, where the (1) conversion
        // should be preferred because in the remarks of (2), it is mentioned
        // OGP prefers value from IGN Paris (code 1467)...
        if (a_name.find("NTF (Paris) to NTF (1)") != std::string::npos &&
            b_name.find("NTF (Paris) to NTF (2)") != std::string::npos) {
            return true;
        }
        if (a_name.find("NTF (Paris) to NTF (2)") != std::string::npos &&
            b_name.find("NTF (Paris) to NTF (1)") != std::string::npos) {
            return false;
        }
        if (a_name.find("NTF (Paris) to RGF93 (1)") != std::string::npos &&
            b_name.find("NTF (Paris) to RGF93 (2)") != std::string::npos) {
            return true;
        }
        if (a_name.find("NTF (Paris) to RGF93 (2)") != std::string::npos &&
            b_name.find("NTF (Paris) to RGF93 (1)") != std::string::npos) {
            return false;
        }

        return a_name > b_name;
    }

    bool operator()(const CoordinateOperationNNPtr &a,
                    const CoordinateOperationNNPtr &b) const {
        const bool ret = compare(a, b);
#if 0
        std::cerr << a->nameStr() << " < " << b->nameStr() << " : " << ret << std::endl;
#endif
        return ret;
    }
};

// ---------------------------------------------------------------------------

static size_t getStepCount(const CoordinateOperationNNPtr &op) {
    auto concat = dynamic_cast<const ConcatenatedOperation *>(op.get());
    size_t stepCount = 1;
    if (concat) {
        stepCount = concat->operations().size();
    }
    return stepCount;
}

// ---------------------------------------------------------------------------

// Return number of steps that are transformations (and not conversions)
static size_t getTransformationStepCount(const CoordinateOperationNNPtr &op) {
    auto concat = dynamic_cast<const ConcatenatedOperation *>(op.get());
    size_t stepCount = 1;
    if (concat) {
        stepCount = 0;
        for (const auto &subOp : concat->operations()) {
            if (dynamic_cast<const Conversion *>(subOp.get()) == nullptr) {
                stepCount++;
            }
        }
    }
    return stepCount;
}

// ---------------------------------------------------------------------------

static bool isNullTransformation(const std::string &name) {
    if (name.find(" + ") != std::string::npos)
        return false;
    return starts_with(name, BALLPARK_GEOCENTRIC_TRANSLATION) ||
           starts_with(name, BALLPARK_GEOGRAPHIC_OFFSET) ||
           starts_with(name, NULL_GEOGRAPHIC_OFFSET) ||
           starts_with(name, NULL_GEOCENTRIC_TRANSLATION);
}

// ---------------------------------------------------------------------------

struct FilterResults {

    FilterResults(const std::vector<CoordinateOperationNNPtr> &sourceListIn,
                  const CoordinateOperationContextNNPtr &contextIn,
                  const metadata::ExtentPtr &extent1In,
                  const metadata::ExtentPtr &extent2In,
                  bool forceStrictContainmentTest)
        : sourceList(sourceListIn), context(contextIn), extent1(extent1In),
          extent2(extent2In), areaOfInterest(context->getAreaOfInterest()),
          desiredAccuracy(context->getDesiredAccuracy()),
          sourceAndTargetCRSExtentUse(
              context->getSourceAndTargetCRSExtentUse()) {

        computeAreaOfInterest();
        filterOut(forceStrictContainmentTest);
    }

    FilterResults &andSort() {
        sort();

        // And now that we have a sorted list, we can remove uninteresting
        // results
        // ...
        removeSyntheticNullTransforms();
        removeUninterestingOps();
        removeDuplicateOps();
        removeSyntheticNullTransforms();
        return *this;
    }

    // ----------------------------------------------------------------------

    // cppcheck-suppress functionStatic
    const std::vector<CoordinateOperationNNPtr> &getRes() { return res; }

    // ----------------------------------------------------------------------
  private:
    const std::vector<CoordinateOperationNNPtr> &sourceList;
    const CoordinateOperationContextNNPtr &context;
    const metadata::ExtentPtr &extent1;
    const metadata::ExtentPtr &extent2;
    metadata::ExtentPtr areaOfInterest;
    const double desiredAccuracy = context->getDesiredAccuracy();
    const CoordinateOperationContext::SourceTargetCRSExtentUse
        sourceAndTargetCRSExtentUse;

    bool hasOpThatContainsAreaOfInterestAndNoGrid = false;
    std::vector<CoordinateOperationNNPtr> res{};

    // ----------------------------------------------------------------------
    void computeAreaOfInterest() {

        // Compute an area of interest from the CRS extent if the user did
        // not specify one
        if (!areaOfInterest) {
            if (sourceAndTargetCRSExtentUse ==
                CoordinateOperationContext::SourceTargetCRSExtentUse::
                    INTERSECTION) {
                if (extent1 && extent2) {
                    areaOfInterest =
                        extent1->intersection(NN_NO_CHECK(extent2));
                }
            } else if (sourceAndTargetCRSExtentUse ==
                       CoordinateOperationContext::SourceTargetCRSExtentUse::
                           SMALLEST) {
                if (extent1 && extent2) {
                    if (getPseudoArea(extent1) < getPseudoArea(extent2)) {
                        areaOfInterest = extent1;
                    } else {
                        areaOfInterest = extent2;
                    }
                } else if (extent1) {
                    areaOfInterest = extent1;
                } else {
                    areaOfInterest = extent2;
                }
            }
        }
    }

    // ---------------------------------------------------------------------------

    void filterOut(bool forceStrictContainmentTest) {

        // Filter out operations that do not match the expected accuracy
        // and area of use.
        const auto spatialCriterion =
            forceStrictContainmentTest
                ? CoordinateOperationContext::SpatialCriterion::
                      STRICT_CONTAINMENT
                : context->getSpatialCriterion();
        bool hasOnlyBallpark = true;
        bool hasNonBallparkWithoutExtent = false;
        bool hasNonBallparkOpWithExtent = false;
        const bool allowBallpark = context->getAllowBallparkTransformations();
        for (const auto &op : sourceList) {
            if (desiredAccuracy != 0) {
                const double accuracy = getAccuracy(op);
                if (accuracy < 0 || accuracy > desiredAccuracy) {
                    continue;
                }
            }
            if (!allowBallpark && op->hasBallparkTransformation()) {
                continue;
            }
            if (areaOfInterest) {
                bool emptyIntersection = false;
                auto extent = getExtent(op, true, emptyIntersection);
                if (!extent) {
                    if (!op->hasBallparkTransformation()) {
                        hasNonBallparkWithoutExtent = true;
                    }
                    continue;
                }
                if (!op->hasBallparkTransformation()) {
                    hasNonBallparkOpWithExtent = true;
                }
                bool extentContains =
                    extent->contains(NN_NO_CHECK(areaOfInterest));
                if (!hasOpThatContainsAreaOfInterestAndNoGrid &&
                    extentContains) {
                    if (!op->hasBallparkTransformation() &&
                        op->gridsNeeded(nullptr, true).empty()) {
                        hasOpThatContainsAreaOfInterestAndNoGrid = true;
                    }
                }
                if (spatialCriterion ==
                        CoordinateOperationContext::SpatialCriterion::
                            STRICT_CONTAINMENT &&
                    !extentContains) {
                    continue;
                }
                if (spatialCriterion ==
                        CoordinateOperationContext::SpatialCriterion::
                            PARTIAL_INTERSECTION &&
                    !extent->intersects(NN_NO_CHECK(areaOfInterest))) {
                    continue;
                }
            } else if (sourceAndTargetCRSExtentUse ==
                       CoordinateOperationContext::SourceTargetCRSExtentUse::
                           BOTH) {
                bool emptyIntersection = false;
                auto extent = getExtent(op, true, emptyIntersection);
                if (!extent) {
                    if (!op->hasBallparkTransformation()) {
                        hasNonBallparkWithoutExtent = true;
                    }
                    continue;
                }
                if (!op->hasBallparkTransformation()) {
                    hasNonBallparkOpWithExtent = true;
                }
                bool extentContainsExtent1 =
                    !extent1 || extent->contains(NN_NO_CHECK(extent1));
                bool extentContainsExtent2 =
                    !extent2 || extent->contains(NN_NO_CHECK(extent2));
                if (!hasOpThatContainsAreaOfInterestAndNoGrid &&
                    extentContainsExtent1 && extentContainsExtent2) {
                    if (!op->hasBallparkTransformation() &&
                        op->gridsNeeded(nullptr, true).empty()) {
                        hasOpThatContainsAreaOfInterestAndNoGrid = true;
                    }
                }
                if (spatialCriterion ==
                    CoordinateOperationContext::SpatialCriterion::
                        STRICT_CONTAINMENT) {
                    if (!extentContainsExtent1 || !extentContainsExtent2) {
                        continue;
                    }
                } else if (spatialCriterion ==
                           CoordinateOperationContext::SpatialCriterion::
                               PARTIAL_INTERSECTION) {
                    bool extentIntersectsExtent1 =
                        !extent1 || extent->intersects(NN_NO_CHECK(extent1));
                    bool extentIntersectsExtent2 =
                        extent2 && extent->intersects(NN_NO_CHECK(extent2));
                    if (!extentIntersectsExtent1 || !extentIntersectsExtent2) {
                        continue;
                    }
                }
            }
            if (!op->hasBallparkTransformation()) {
                hasOnlyBallpark = false;
            }
            res.emplace_back(op);
        }

        // In case no operation has an extent and no result is found,
        // retain all initial operations that match accuracy criterion.
        if ((res.empty() && !hasNonBallparkOpWithExtent) ||
            (hasOnlyBallpark && hasNonBallparkWithoutExtent)) {
            for (const auto &op : sourceList) {
                if (desiredAccuracy != 0) {
                    const double accuracy = getAccuracy(op);
                    if (accuracy < 0 || accuracy > desiredAccuracy) {
                        continue;
                    }
                }
                if (!allowBallpark && op->hasBallparkTransformation()) {
                    continue;
                }
                res.emplace_back(op);
            }
        }
    }

    // ----------------------------------------------------------------------

    void sort() {

        // Precompute a number of parameters for each operation that will be
        // useful for the sorting.
        std::map<CoordinateOperation *, PrecomputedOpCharacteristics> map;
        const auto gridAvailabilityUse = context->getGridAvailabilityUse();
        for (const auto &op : res) {
            bool dummy = false;
            auto extentOp = getExtent(op, true, dummy);
            double area = 0.0;
            if (extentOp) {
                if (areaOfInterest) {
                    area = getPseudoArea(
                        extentOp->intersection(NN_NO_CHECK(areaOfInterest)));
                } else if (extent1 && extent2) {
                    auto x = extentOp->intersection(NN_NO_CHECK(extent1));
                    auto y = extentOp->intersection(NN_NO_CHECK(extent2));
                    area = getPseudoArea(x) + getPseudoArea(y) -
                           ((x && y)
                                ? getPseudoArea(x->intersection(NN_NO_CHECK(y)))
                                : 0.0);
                } else if (extent1) {
                    area = getPseudoArea(
                        extentOp->intersection(NN_NO_CHECK(extent1)));
                } else if (extent2) {
                    area = getPseudoArea(
                        extentOp->intersection(NN_NO_CHECK(extent2)));
                } else {
                    area = getPseudoArea(extentOp);
                }
            }

            bool hasGrids = false;
            bool gridsAvailable = true;
            bool gridsKnown = true;
            if (context->getAuthorityFactory()) {
                const auto gridsNeeded = op->gridsNeeded(
                    context->getAuthorityFactory()->databaseContext(),
                    gridAvailabilityUse ==
                        CoordinateOperationContext::GridAvailabilityUse::
                            KNOWN_AVAILABLE);
                for (const auto &gridDesc : gridsNeeded) {
                    hasGrids = true;
                    if (gridAvailabilityUse ==
                            CoordinateOperationContext::GridAvailabilityUse::
                                USE_FOR_SORTING &&
                        !gridDesc.available) {
                        gridsAvailable = false;
                    }
                    if (gridDesc.packageName.empty() &&
                        !(!gridDesc.url.empty() && gridDesc.openLicense) &&
                        !gridDesc.available) {
                        gridsKnown = false;
                    }
                }
            }

            const auto stepCount = getStepCount(op);

            bool isPROJExportable = false;
            auto formatter = io::PROJStringFormatter::create();
            try {
                op->exportToPROJString(formatter.get());
                // Grids might be missing, but at least this is something
                // PROJ could potentially process
                isPROJExportable = true;
            } catch (const std::exception &) {
            }

#if 0
            std::cerr << op->nameStr() << " ";
            std::cerr << area << " ";
            std::cerr << getAccuracy(op) << " ";
            std::cerr << isPROJExportable << " ";
            std::cerr << hasGrids << " ";
            std::cerr << gridsAvailable << " ";
            std::cerr << gridsKnown << " ";
            std::cerr << stepCount << " ";
            std::cerr << op->hasBallparkTransformation() << " ";
            std::cerr << isNullTransformation(op->nameStr()) << " ";
            std::cerr << std::endl;
#endif
            map[op.get()] = PrecomputedOpCharacteristics(
                area, getAccuracy(op), isPROJExportable, hasGrids,
                gridsAvailable, gridsKnown, stepCount,
                op->hasBallparkTransformation(),
                op->nameStr().find("ballpark vertical transformation") !=
                    std::string::npos,
                isNullTransformation(op->nameStr()));
        }

        // Sort !
        SortFunction sortFunc(map);
        std::sort(res.begin(), res.end(), sortFunc);

// Debug code to check consistency of the sort function
#ifdef DEBUG_SORT
        constexpr bool debugSort = true;
#elif !defined(NDEBUG)
        const bool debugSort = getenv("PROJ_DEBUG_SORT_FUNCT") != nullptr;
#endif
#if defined(DEBUG_SORT) || !defined(NDEBUG)
        if (debugSort) {
            const bool assertIfIssue =
                !(getenv("PROJ_DEBUG_SORT_FUNCT_ASSERT") != nullptr);
            for (size_t i = 0; i < res.size(); ++i) {
                for (size_t j = i + 1; j < res.size(); ++j) {
                    if (sortFunc(res[j], res[i])) {
#ifdef DEBUG_SORT
                        std::cerr << "Sorting issue with entry " << i << "("
                                  << res[i]->nameStr() << ") and " << j << "("
                                  << res[j]->nameStr() << ")" << std::endl;
#endif
                        if (assertIfIssue) {
                            assert(false);
                        }
                    }
                }
            }
        }
#endif
    }

    // ----------------------------------------------------------------------

    void removeSyntheticNullTransforms() {

        // If we have more than one result, and than the last result is the
        // default "Ballpark geographic offset" or "Ballpark geocentric
        // translation" operations we have synthetized, and that at least one
        // operation has the desired area of interest and does not require the
        // use of grids, remove it as all previous results are necessarily
        // better
        if (hasOpThatContainsAreaOfInterestAndNoGrid && res.size() > 1) {
            const auto &opLast = res.back();
            if (opLast->hasBallparkTransformation() ||
                isNullTransformation(opLast->nameStr())) {
                std::vector<CoordinateOperationNNPtr> resTemp;
                for (size_t i = 0; i < res.size() - 1; i++) {
                    resTemp.emplace_back(res[i]);
                }
                res = std::move(resTemp);
            }
        }
    }

    // ----------------------------------------------------------------------

    void removeUninterestingOps() {

        // Eliminate operations that bring nothing, ie for a given area of use,
        // do not keep operations that have similar or worse accuracy, but
        // involve more (non conversion) steps
        std::vector<CoordinateOperationNNPtr> resTemp;
        metadata::ExtentPtr lastExtent;
        double lastAccuracy = -1;
        size_t lastStepCount = 0;
        CoordinateOperationPtr lastOp;

        bool first = true;
        for (const auto &op : res) {
            const auto curAccuracy = getAccuracy(op);
            bool dummy = false;
            const auto curExtent = getExtent(op, true, dummy);
            const auto curStepCount = getTransformationStepCount(op);

            if (first) {
                resTemp.emplace_back(op);
                first = false;
            } else {
                if (lastOp->_isEquivalentTo(op.get())) {
                    continue;
                }
                const bool sameExtent =
                    ((!curExtent && !lastExtent) ||
                     (curExtent && lastExtent &&
                      curExtent->contains(NN_NO_CHECK(lastExtent)) &&
                      lastExtent->contains(NN_NO_CHECK(curExtent))));
                if (((curAccuracy >= lastAccuracy && lastAccuracy >= 0) ||
                     (curAccuracy < 0 && lastAccuracy >= 0)) &&
                    sameExtent && curStepCount > lastStepCount) {
                    continue;
                }

                resTemp.emplace_back(op);
            }

            lastOp = op.as_nullable();
            lastStepCount = curStepCount;
            lastExtent = curExtent;
            lastAccuracy = curAccuracy;
        }
        res = std::move(resTemp);
    }

    // ----------------------------------------------------------------------

    // cppcheck-suppress functionStatic
    void removeDuplicateOps() {

        if (res.size() <= 1) {
            return;
        }

        // When going from EPSG:4807 (NTF Paris) to EPSG:4171 (RGC93), we get
        // EPSG:7811, NTF (Paris) to RGF93 (2), 1 m
        // and unknown id, NTF (Paris) to NTF (1) + Inverse of RGF93 to NTF (2),
        // 1 m
        // both have same PROJ string and extent
        // Do not keep the later (that has more steps) as it adds no value.

        std::set<std::string> setPROJPlusExtent;
        std::vector<CoordinateOperationNNPtr> resTemp;
        for (const auto &op : res) {
            auto formatter = io::PROJStringFormatter::create();
            try {
                std::string key(op->exportToPROJString(formatter.get()));
                bool dummy = false;
                auto extentOp = getExtent(op, true, dummy);
                if (extentOp) {
                    const auto &geogElts = extentOp->geographicElements();
                    if (geogElts.size() == 1) {
                        auto bbox = dynamic_cast<
                            const metadata::GeographicBoundingBox *>(
                            geogElts[0].get());
                        if (bbox) {
                            double w = bbox->westBoundLongitude();
                            double s = bbox->southBoundLatitude();
                            double e = bbox->eastBoundLongitude();
                            double n = bbox->northBoundLatitude();
                            key += "-";
                            key += toString(w);
                            key += "-";
                            key += toString(s);
                            key += "-";
                            key += toString(e);
                            key += "-";
                            key += toString(n);
                        }
                    }
                }

                if (setPROJPlusExtent.find(key) == setPROJPlusExtent.end()) {
                    resTemp.emplace_back(op);
                    setPROJPlusExtent.insert(key);
                }
            } catch (const std::exception &) {
                resTemp.emplace_back(op);
            }
        }
        res = std::move(resTemp);
    }
};

// ---------------------------------------------------------------------------

/** \brief Filter operations and sort them given context.
 *
 * If a desired accuracy is specified, only keep operations whose accuracy
 * is at least the desired one.
 * If an area of interest is specified, only keep operations whose area of
 * use include the area of interest.
 * Then sort remaining operations by descending area of use, and increasing
 * accuracy.
 */
static std::vector<CoordinateOperationNNPtr>
filterAndSort(const std::vector<CoordinateOperationNNPtr> &sourceList,
              const CoordinateOperationContextNNPtr &context,
              const metadata::ExtentPtr &extent1,
              const metadata::ExtentPtr &extent2) {
#ifdef TRACE_CREATE_OPERATIONS
    ENTER_FUNCTION();
    logTrace("number of results before filter and sort: " +
             toString(static_cast<int>(sourceList.size())));
#endif
    auto resFiltered =
        FilterResults(sourceList, context, extent1, extent2, false)
            .andSort()
            .getRes();
#ifdef TRACE_CREATE_OPERATIONS
    logTrace("number of results after filter and sort: " +
             toString(static_cast<int>(resFiltered.size())));
#endif
    return resFiltered;
}
//! @endcond

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
// Apply the inverse() method on all elements of the input list
static std::vector<CoordinateOperationNNPtr>
applyInverse(const std::vector<CoordinateOperationNNPtr> &list) {
    auto res = list;
    for (auto &op : res) {
#ifdef DEBUG
        auto opNew = op->inverse();
        assert(opNew->targetCRS()->isEquivalentTo(op->sourceCRS().get()));
        assert(opNew->sourceCRS()->isEquivalentTo(op->targetCRS().get()));
        op = opNew;
#else
        op = op->inverse();
#endif
    }
    return res;
}
//! @endcond

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress

void CoordinateOperationFactory::Private::buildCRSIds(
    const crs::CRSNNPtr &crs, Private::Context &context,
    std::list<std::pair<std::string, std::string>> &ids) {
    const auto &authFactory = context.context->getAuthorityFactory();
    assert(authFactory);
    for (const auto &id : crs->identifiers()) {
        const auto &authName = *(id->codeSpace());
        const auto &code = id->code();
        if (!authName.empty()) {
            const auto tmpAuthFactory = io::AuthorityFactory::create(
                authFactory->databaseContext(), authName);
            try {
                // Consistency check for the ID attached to the object.
                // See https://github.com/OSGeo/PROJ/issues/1982 where EPSG:4656
                // is attached to a GeographicCRS whereas it is a ProjectedCRS
                if (tmpAuthFactory->createCoordinateReferenceSystem(code)
                        ->_isEquivalentTo(
                            crs.get(),
                            util::IComparable::Criterion::
                                EQUIVALENT_EXCEPT_AXIS_ORDER_GEOGCRS)) {
                    ids.emplace_back(authName, code);
                } else {
                    // TODO? log this inconsistency
                }
            } catch (const std::exception &) {
                // TODO? log this inconsistency
            }
        }
    }
    if (ids.empty()) {
        std::vector<io::AuthorityFactory::ObjectType> allowedObjects;
        auto geogCRS = dynamic_cast<const crs::GeographicCRS *>(crs.get());
        if (geogCRS) {
            allowedObjects.push_back(
                geogCRS->coordinateSystem()->axisList().size() == 2
                    ? io::AuthorityFactory::ObjectType::GEOGRAPHIC_2D_CRS
                    : io::AuthorityFactory::ObjectType::GEOGRAPHIC_3D_CRS);
        } else if (dynamic_cast<crs::ProjectedCRS *>(crs.get())) {
            allowedObjects.push_back(
                io::AuthorityFactory::ObjectType::PROJECTED_CRS);
        } else if (dynamic_cast<crs::VerticalCRS *>(crs.get())) {
            allowedObjects.push_back(
                io::AuthorityFactory::ObjectType::VERTICAL_CRS);
        }
        if (!allowedObjects.empty()) {

            const std::pair<io::AuthorityFactory::ObjectType, std::string> key(
                allowedObjects[0], crs->nameStr());
            auto iter = context.cacheNameToCRS.find(key);
            if (iter != context.cacheNameToCRS.end()) {
                ids = iter->second;
                return;
            }

            const auto &authFactoryName = authFactory->getAuthority();
            try {
                const auto tmpAuthFactory = io::AuthorityFactory::create(
                    authFactory->databaseContext(),
                    (authFactoryName.empty() || authFactoryName == "any")
                        ? std::string()
                        : authFactoryName);

                auto matches = tmpAuthFactory->createObjectsFromName(
                    crs->nameStr(), allowedObjects, false, 2);
                if (matches.size() == 1 &&
                    crs->_isEquivalentTo(
                        matches.front().get(),
                        util::IComparable::Criterion::EQUIVALENT) &&
                    !matches.front()->identifiers().empty()) {
                    const auto &tmpIds = matches.front()->identifiers();
                    ids.emplace_back(*(tmpIds[0]->codeSpace()),
                                     tmpIds[0]->code());
                }
            } catch (const std::exception &) {
            }
            context.cacheNameToCRS[key] = ids;
        }
    }
}

// ---------------------------------------------------------------------------

static std::vector<std::string>
getCandidateAuthorities(const io::AuthorityFactoryPtr &authFactory,
                        const std::string &srcAuthName,
                        const std::string &targetAuthName) {
    const auto &authFactoryName = authFactory->getAuthority();
    std::vector<std::string> authorities;
    if (authFactoryName == "any") {
        authorities.emplace_back();
    }
    if (authFactoryName.empty()) {
        authorities = authFactory->databaseContext()->getAllowedAuthorities(
            srcAuthName, targetAuthName);
        if (authorities.empty()) {
            authorities.emplace_back();
        }
    } else {
        authorities.emplace_back(authFactoryName);
    }
    return authorities;
}

// ---------------------------------------------------------------------------

// Look in the authority registry for operations from sourceCRS to targetCRS
std::vector<CoordinateOperationNNPtr>
CoordinateOperationFactory::Private::findOpsInRegistryDirect(
    const crs::CRSNNPtr &sourceCRS, const crs::CRSNNPtr &targetCRS,
    Private::Context &context, bool &resNonEmptyBeforeFiltering) {
    const auto &authFactory = context.context->getAuthorityFactory();
    assert(authFactory);

#ifdef TRACE_CREATE_OPERATIONS
    ENTER_BLOCK("findOpsInRegistryDirect(" + objectAsStr(sourceCRS.get()) +
                " --> " + objectAsStr(targetCRS.get()) + ")");
#endif

    resNonEmptyBeforeFiltering = false;
    std::list<std::pair<std::string, std::string>> sourceIds;
    std::list<std::pair<std::string, std::string>> targetIds;
    buildCRSIds(sourceCRS, context, sourceIds);
    buildCRSIds(targetCRS, context, targetIds);

    const auto gridAvailabilityUse = context.context->getGridAvailabilityUse();
    for (const auto &idSrc : sourceIds) {
        const auto &srcAuthName = idSrc.first;
        const auto &srcCode = idSrc.second;
        for (const auto &idTarget : targetIds) {
            const auto &targetAuthName = idTarget.first;
            const auto &targetCode = idTarget.second;

            const auto authorities(getCandidateAuthorities(
                authFactory, srcAuthName, targetAuthName));
            std::vector<CoordinateOperationNNPtr> res;
            for (const auto &authority : authorities) {
                const auto authName =
                    authority == "any" ? std::string() : authority;
                const auto tmpAuthFactory = io::AuthorityFactory::create(
                    authFactory->databaseContext(), authName);
                auto resTmp =
                    tmpAuthFactory->createFromCoordinateReferenceSystemCodes(
                        srcAuthName, srcCode, targetAuthName, targetCode,
                        context.context->getUsePROJAlternativeGridNames(),
                        gridAvailabilityUse ==
                                CoordinateOperationContext::
                                    GridAvailabilityUse::
                                        DISCARD_OPERATION_IF_MISSING_GRID ||
                            gridAvailabilityUse ==
                                CoordinateOperationContext::
                                    GridAvailabilityUse::KNOWN_AVAILABLE,
                        gridAvailabilityUse ==
                            CoordinateOperationContext::GridAvailabilityUse::
                                KNOWN_AVAILABLE,
                        context.context->getDiscardSuperseded(), true, false,
                        context.extent1, context.extent2);
                res.insert(res.end(), resTmp.begin(), resTmp.end());
                if (authName == "PROJ") {
                    continue;
                }
                if (!res.empty()) {
                    resNonEmptyBeforeFiltering = true;
                    auto resFiltered =
                        FilterResults(res, context.context, context.extent1,
                                      context.extent2, false)
                            .getRes();
#ifdef TRACE_CREATE_OPERATIONS
                    logTrace("filtering reduced from " +
                             toString(static_cast<int>(res.size())) + " to " +
                             toString(static_cast<int>(resFiltered.size())));
#endif
                    return resFiltered;
                }
            }
        }
    }
    return std::vector<CoordinateOperationNNPtr>();
}

// ---------------------------------------------------------------------------

// Look in the authority registry for operations to targetCRS
std::vector<CoordinateOperationNNPtr>
CoordinateOperationFactory::Private::findOpsInRegistryDirectTo(
    const crs::CRSNNPtr &targetCRS, Private::Context &context) {
#ifdef TRACE_CREATE_OPERATIONS
    ENTER_BLOCK("findOpsInRegistryDirectTo({any} -->" +
                objectAsStr(targetCRS.get()) + ")");
#endif

    const auto &authFactory = context.context->getAuthorityFactory();
    assert(authFactory);

    std::list<std::pair<std::string, std::string>> ids;
    buildCRSIds(targetCRS, context, ids);

    const auto gridAvailabilityUse = context.context->getGridAvailabilityUse();
    for (const auto &id : ids) {
        const auto &targetAuthName = id.first;
        const auto &targetCode = id.second;

        const auto authorities(getCandidateAuthorities(
            authFactory, targetAuthName, targetAuthName));
        for (const auto &authority : authorities) {
            const auto tmpAuthFactory = io::AuthorityFactory::create(
                authFactory->databaseContext(),
                authority == "any" ? std::string() : authority);
            auto res = tmpAuthFactory->createFromCoordinateReferenceSystemCodes(
                std::string(), std::string(), targetAuthName, targetCode,
                context.context->getUsePROJAlternativeGridNames(),

                gridAvailabilityUse ==
                        CoordinateOperationContext::GridAvailabilityUse::
                            DISCARD_OPERATION_IF_MISSING_GRID ||
                    gridAvailabilityUse ==
                        CoordinateOperationContext::GridAvailabilityUse::
                            KNOWN_AVAILABLE,
                gridAvailabilityUse == CoordinateOperationContext::
                                           GridAvailabilityUse::KNOWN_AVAILABLE,
                context.context->getDiscardSuperseded(), true, true,
                context.extent1, context.extent2);
            if (!res.empty()) {
                auto resFiltered =
                    FilterResults(res, context.context, context.extent1,
                                  context.extent2, false)
                        .getRes();
#ifdef TRACE_CREATE_OPERATIONS
                logTrace("filtering reduced from " +
                         toString(static_cast<int>(res.size())) + " to " +
                         toString(static_cast<int>(resFiltered.size())));
#endif
                return resFiltered;
            }
        }
    }
    return std::vector<CoordinateOperationNNPtr>();
}

//! @endcond

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress

// Look in the authority registry for operations from sourceCRS to targetCRS
// using an intermediate pivot
std::vector<CoordinateOperationNNPtr>
CoordinateOperationFactory::Private::findsOpsInRegistryWithIntermediate(
    const crs::CRSNNPtr &sourceCRS, const crs::CRSNNPtr &targetCRS,
    Private::Context &context,
    bool useCreateBetweenGeodeticCRSWithDatumBasedIntermediates) {

#ifdef TRACE_CREATE_OPERATIONS
    ENTER_BLOCK("findsOpsInRegistryWithIntermediate(" +
                objectAsStr(sourceCRS.get()) + " --> " +
                objectAsStr(targetCRS.get()) + ")");
#endif

    const auto &authFactory = context.context->getAuthorityFactory();
    assert(authFactory);

    std::list<std::pair<std::string, std::string>> sourceIds;
    std::list<std::pair<std::string, std::string>> targetIds;
    buildCRSIds(sourceCRS, context, sourceIds);
    buildCRSIds(targetCRS, context, targetIds);

    const auto gridAvailabilityUse = context.context->getGridAvailabilityUse();
    for (const auto &idSrc : sourceIds) {
        const auto &srcAuthName = idSrc.first;
        const auto &srcCode = idSrc.second;
        for (const auto &idTarget : targetIds) {
            const auto &targetAuthName = idTarget.first;
            const auto &targetCode = idTarget.second;

            const auto authorities(getCandidateAuthorities(
                authFactory, srcAuthName, targetAuthName));
            assert(!authorities.empty());

            const auto tmpAuthFactory = io::AuthorityFactory::create(
                authFactory->databaseContext(),
                (authFactory->getAuthority() == "any" || authorities.size() > 1)
                    ? std::string()
                    : authorities.front());

            std::vector<CoordinateOperationNNPtr> res;
            if (useCreateBetweenGeodeticCRSWithDatumBasedIntermediates) {
                res =
                    tmpAuthFactory
                        ->createBetweenGeodeticCRSWithDatumBasedIntermediates(
                            sourceCRS, srcAuthName, srcCode, targetCRS,
                            targetAuthName, targetCode,
                            context.context->getUsePROJAlternativeGridNames(),
                            gridAvailabilityUse ==
                                    CoordinateOperationContext::
                                        GridAvailabilityUse::
                                            DISCARD_OPERATION_IF_MISSING_GRID ||
                                gridAvailabilityUse ==
                                    CoordinateOperationContext::
                                        GridAvailabilityUse::KNOWN_AVAILABLE,
                            gridAvailabilityUse ==
                                CoordinateOperationContext::
                                    GridAvailabilityUse::KNOWN_AVAILABLE,
                            context.context->getDiscardSuperseded(),
                            authFactory->getAuthority() != "any" &&
                                    authorities.size() > 1
                                ? authorities
                                : std::vector<std::string>(),
                            context.extent1, context.extent2);
            } else {
                io::AuthorityFactory::ObjectType intermediateObjectType =
                    io::AuthorityFactory::ObjectType::CRS;

                // If doing GeogCRS --> GeogCRS, only use GeogCRS as
                // intermediate CRS
                // Avoid weird behavior when doing NAD83 -> NAD83(2011)
                // that would go through NAVD88 otherwise.
                if (context.context->getIntermediateCRS().empty() &&
                    dynamic_cast<const crs::GeographicCRS *>(sourceCRS.get()) &&
                    dynamic_cast<const crs::GeographicCRS *>(targetCRS.get())) {
                    intermediateObjectType =
                        io::AuthorityFactory::ObjectType::GEOGRAPHIC_CRS;
                }
                res = tmpAuthFactory->createFromCRSCodesWithIntermediates(
                    srcAuthName, srcCode, targetAuthName, targetCode,
                    context.context->getUsePROJAlternativeGridNames(),
                    gridAvailabilityUse ==
                            CoordinateOperationContext::GridAvailabilityUse::
                                DISCARD_OPERATION_IF_MISSING_GRID ||
                        gridAvailabilityUse ==
                            CoordinateOperationContext::GridAvailabilityUse::
                                KNOWN_AVAILABLE,
                    gridAvailabilityUse ==
                        CoordinateOperationContext::GridAvailabilityUse::
                            KNOWN_AVAILABLE,
                    context.context->getDiscardSuperseded(),
                    context.context->getIntermediateCRS(),
                    intermediateObjectType,
                    authFactory->getAuthority() != "any" &&
                            authorities.size() > 1
                        ? authorities
                        : std::vector<std::string>(),
                    context.extent1, context.extent2);
            }
            if (!res.empty()) {

                auto resFiltered =
                    FilterResults(res, context.context, context.extent1,
                                  context.extent2, false)
                        .getRes();
#ifdef TRACE_CREATE_OPERATIONS
                logTrace("filtering reduced from " +
                         toString(static_cast<int>(res.size())) + " to " +
                         toString(static_cast<int>(resFiltered.size())));
#endif
                return resFiltered;
            }
        }
    }
    return std::vector<CoordinateOperationNNPtr>();
}
//! @endcond

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
static TransformationNNPtr
createBallparkGeographicOffset(const crs::CRSNNPtr &sourceCRS,
                               const crs::CRSNNPtr &targetCRS,
                               const io::DatabaseContextPtr &dbContext) {

    const crs::GeographicCRS *geogSrc =
        dynamic_cast<const crs::GeographicCRS *>(sourceCRS.get());
    const crs::GeographicCRS *geogDst =
        dynamic_cast<const crs::GeographicCRS *>(targetCRS.get());
    const bool isSameDatum = geogSrc && geogDst &&
                             geogSrc->datumNonNull(dbContext)->_isEquivalentTo(
                                 geogDst->datumNonNull(dbContext).get(),
                                 util::IComparable::Criterion::EQUIVALENT);

    auto name = buildOpName(isSameDatum ? NULL_GEOGRAPHIC_OFFSET
                                        : BALLPARK_GEOGRAPHIC_OFFSET,
                            sourceCRS, targetCRS);

    const auto &sourceCRSExtent = getExtent(sourceCRS);
    const auto &targetCRSExtent = getExtent(targetCRS);
    const bool sameExtent =
        sourceCRSExtent && targetCRSExtent &&
        sourceCRSExtent->_isEquivalentTo(
            targetCRSExtent.get(), util::IComparable::Criterion::EQUIVALENT);

    util::PropertyMap map;
    map.set(common::IdentifiedObject::NAME_KEY, name)
        .set(common::ObjectUsage::DOMAIN_OF_VALIDITY_KEY,
             sameExtent ? NN_NO_CHECK(sourceCRSExtent)
                        : metadata::Extent::WORLD);
    const common::Angle angle0(0);

    std::vector<metadata::PositionalAccuracyNNPtr> accuracies;
    if (isSameDatum) {
        accuracies.emplace_back(metadata::PositionalAccuracy::create("0"));
    }

    if (dynamic_cast<const crs::SingleCRS *>(sourceCRS.get())
                ->coordinateSystem()
                ->axisList()
                .size() == 3 ||
        dynamic_cast<const crs::SingleCRS *>(targetCRS.get())
                ->coordinateSystem()
                ->axisList()
                .size() == 3) {
        return Transformation::createGeographic3DOffsets(
            map, sourceCRS, targetCRS, angle0, angle0, common::Length(0),
            accuracies);
    } else {
        return Transformation::createGeographic2DOffsets(
            map, sourceCRS, targetCRS, angle0, angle0, accuracies);
    }
}
//! @endcond

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress

// ---------------------------------------------------------------------------

struct MyPROJStringExportableGeodToGeod final
    : public io::IPROJStringExportable {
    crs::GeodeticCRSPtr geodSrc{};
    crs::GeodeticCRSPtr geodDst{};

    MyPROJStringExportableGeodToGeod(const crs::GeodeticCRSPtr &geodSrcIn,
                                     const crs::GeodeticCRSPtr &geodDstIn)
        : geodSrc(geodSrcIn), geodDst(geodDstIn) {}

    ~MyPROJStringExportableGeodToGeod() override;

    void
        // cppcheck-suppress functionStatic
        _exportToPROJString(io::PROJStringFormatter *formatter) const override {

        formatter->startInversion();
        geodSrc->_exportToPROJString(formatter);
        formatter->stopInversion();
        geodDst->_exportToPROJString(formatter);
    }
};

MyPROJStringExportableGeodToGeod::~MyPROJStringExportableGeodToGeod() = default;

// ---------------------------------------------------------------------------

struct MyPROJStringExportableHorizVertical final
    : public io::IPROJStringExportable {
    CoordinateOperationPtr horizTransform{};
    CoordinateOperationPtr verticalTransform{};
    crs::GeographicCRSPtr geogDst{};

    MyPROJStringExportableHorizVertical(
        const CoordinateOperationPtr &horizTransformIn,
        const CoordinateOperationPtr &verticalTransformIn,
        const crs::GeographicCRSPtr &geogDstIn)
        : horizTransform(horizTransformIn),
          verticalTransform(verticalTransformIn), geogDst(geogDstIn) {}

    ~MyPROJStringExportableHorizVertical() override;

    void
        // cppcheck-suppress functionStatic
        _exportToPROJString(io::PROJStringFormatter *formatter) const override {

        formatter->pushOmitZUnitConversion();

        horizTransform->_exportToPROJString(formatter);

        formatter->startInversion();
        geogDst->addAngularUnitConvertAndAxisSwap(formatter);
        formatter->stopInversion();

        formatter->popOmitZUnitConversion();

        formatter->pushOmitHorizontalConversionInVertTransformation();
        verticalTransform->_exportToPROJString(formatter);
        formatter->popOmitHorizontalConversionInVertTransformation();

        formatter->pushOmitZUnitConversion();
        geogDst->addAngularUnitConvertAndAxisSwap(formatter);
        formatter->popOmitZUnitConversion();
    }
};

MyPROJStringExportableHorizVertical::~MyPROJStringExportableHorizVertical() =
    default;

// ---------------------------------------------------------------------------

struct MyPROJStringExportableHorizVerticalHorizPROJBased final
    : public io::IPROJStringExportable {
    CoordinateOperationPtr opSrcCRSToGeogCRS{};
    CoordinateOperationPtr verticalTransform{};
    CoordinateOperationPtr opGeogCRStoDstCRS{};
    crs::GeographicCRSPtr interpolationGeogCRS{};

    MyPROJStringExportableHorizVerticalHorizPROJBased(
        const CoordinateOperationPtr &opSrcCRSToGeogCRSIn,
        const CoordinateOperationPtr &verticalTransformIn,
        const CoordinateOperationPtr &opGeogCRStoDstCRSIn,
        const crs::GeographicCRSPtr &interpolationGeogCRSIn)
        : opSrcCRSToGeogCRS(opSrcCRSToGeogCRSIn),
          verticalTransform(verticalTransformIn),
          opGeogCRStoDstCRS(opGeogCRStoDstCRSIn),
          interpolationGeogCRS(interpolationGeogCRSIn) {}

    ~MyPROJStringExportableHorizVerticalHorizPROJBased() override;

    void
        // cppcheck-suppress functionStatic
        _exportToPROJString(io::PROJStringFormatter *formatter) const override {

        formatter->pushOmitZUnitConversion();

        opSrcCRSToGeogCRS->_exportToPROJString(formatter);

        formatter->startInversion();
        interpolationGeogCRS->addAngularUnitConvertAndAxisSwap(formatter);
        formatter->stopInversion();

        formatter->popOmitZUnitConversion();

        formatter->pushOmitHorizontalConversionInVertTransformation();
        verticalTransform->_exportToPROJString(formatter);
        formatter->popOmitHorizontalConversionInVertTransformation();

        formatter->pushOmitZUnitConversion();

        interpolationGeogCRS->addAngularUnitConvertAndAxisSwap(formatter);

        opGeogCRStoDstCRS->_exportToPROJString(formatter);

        formatter->popOmitZUnitConversion();
    }
};

MyPROJStringExportableHorizVerticalHorizPROJBased::
    ~MyPROJStringExportableHorizVerticalHorizPROJBased() = default;

//! @endcond

} // namespace operation
NS_PROJ_END

#if 0
namespace dropbox{ namespace oxygen {
template<> nn<std::shared_ptr<NS_PROJ::operation::MyPROJStringExportableGeodToGeod>>::~nn() = default;
template<> nn<std::shared_ptr<NS_PROJ::operation::MyPROJStringExportableHorizVertical>>::~nn() = default;
template<> nn<std::shared_ptr<NS_PROJ::operation::MyPROJStringExportableHorizVerticalHorizPROJBased>>::~nn() = default;
}}
#endif

NS_PROJ_START
namespace operation {

//! @cond Doxygen_Suppress

// ---------------------------------------------------------------------------

static std::string buildTransfName(const std::string &srcName,
                                   const std::string &targetName) {
    std::string name("Transformation from ");
    name += srcName;
    name += " to ";
    name += targetName;
    return name;
}

// ---------------------------------------------------------------------------

static CoordinateOperationNNPtr
createGeodToGeodPROJBased(const crs::CRSNNPtr &geodSrc,
                          const crs::CRSNNPtr &geodDst) {

    auto exportable = util::nn_make_shared<MyPROJStringExportableGeodToGeod>(
        util::nn_dynamic_pointer_cast<crs::GeodeticCRS>(geodSrc),
        util::nn_dynamic_pointer_cast<crs::GeodeticCRS>(geodDst));

    auto properties = util::PropertyMap().set(
        common::IdentifiedObject::NAME_KEY,
        buildTransfName(geodSrc->nameStr(), geodDst->nameStr()));
    return createPROJBased(properties, exportable, geodSrc, geodDst, nullptr,
                           {}, false);
}

// ---------------------------------------------------------------------------

static std::string
getRemarks(const std::vector<operation::CoordinateOperationNNPtr> &ops) {
    std::string remarks;
    for (const auto &op : ops) {
        const auto &opRemarks = op->remarks();
        if (!opRemarks.empty()) {
            if (!remarks.empty()) {
                remarks += '\n';
            }

            std::string opName(op->nameStr());
            if (starts_with(opName, INVERSE_OF)) {
                opName = opName.substr(INVERSE_OF.size());
            }

            remarks += "For ";
            remarks += opName;

            const auto &ids = op->identifiers();
            if (!ids.empty()) {
                std::string authority(*ids.front()->codeSpace());
                if (starts_with(authority, "INVERSE(") &&
                    authority.back() == ')') {
                    authority = authority.substr(strlen("INVERSE("),
                                                 authority.size() - 1 -
                                                     strlen("INVERSE("));
                }
                if (starts_with(authority, "DERIVED_FROM(") &&
                    authority.back() == ')') {
                    authority = authority.substr(strlen("DERIVED_FROM("),
                                                 authority.size() - 1 -
                                                     strlen("DERIVED_FROM("));
                }

                remarks += " (";
                remarks += authority;
                remarks += ':';
                remarks += ids.front()->code();
                remarks += ')';
            }
            remarks += ": ";
            remarks += opRemarks;
        }
    }
    return remarks;
}

// ---------------------------------------------------------------------------

static CoordinateOperationNNPtr createHorizVerticalPROJBased(
    const crs::CRSNNPtr &sourceCRS, const crs::CRSNNPtr &targetCRS,
    const operation::CoordinateOperationNNPtr &horizTransform,
    const operation::CoordinateOperationNNPtr &verticalTransform,
    bool checkExtent) {

    auto geogDst = util::nn_dynamic_pointer_cast<crs::GeographicCRS>(targetCRS);
    assert(geogDst);

    auto exportable = util::nn_make_shared<MyPROJStringExportableHorizVertical>(
        horizTransform, verticalTransform, geogDst);

    const bool horizTransformIsNoOp =
        starts_with(horizTransform->nameStr(), NULL_GEOGRAPHIC_OFFSET) &&
        horizTransform->nameStr().find(" + ") == std::string::npos;
    if (horizTransformIsNoOp) {
        auto properties = util::PropertyMap();
        properties.set(common::IdentifiedObject::NAME_KEY,
                       verticalTransform->nameStr());
        bool dummy = false;
        auto extent = getExtent(verticalTransform, true, dummy);
        if (extent) {
            properties.set(common::ObjectUsage::DOMAIN_OF_VALIDITY_KEY,
                           NN_NO_CHECK(extent));
        }
        const auto &remarks = verticalTransform->remarks();
        if (!remarks.empty()) {
            properties.set(common::IdentifiedObject::REMARKS_KEY, remarks);
        }
        return createPROJBased(
            properties, exportable, sourceCRS, targetCRS, nullptr,
            verticalTransform->coordinateOperationAccuracies(),
            verticalTransform->hasBallparkTransformation());
    } else {
        bool emptyIntersection = false;
        auto ops = std::vector<CoordinateOperationNNPtr>{horizTransform,
                                                         verticalTransform};
        auto extent = getExtent(ops, true, emptyIntersection);
        if (checkExtent && emptyIntersection) {
            std::string msg(
                "empty intersection of area of validity of concatenated "
                "operations");
            throw InvalidOperationEmptyIntersection(msg);
        }
        auto properties = util::PropertyMap();
        properties.set(common::IdentifiedObject::NAME_KEY,
                       computeConcatenatedName(ops));

        if (extent) {
            properties.set(common::ObjectUsage::DOMAIN_OF_VALIDITY_KEY,
                           NN_NO_CHECK(extent));
        }

        const auto remarks = getRemarks(ops);
        if (!remarks.empty()) {
            properties.set(common::IdentifiedObject::REMARKS_KEY, remarks);
        }

        std::vector<metadata::PositionalAccuracyNNPtr> accuracies;
        const double accuracy = getAccuracy(ops);
        if (accuracy >= 0.0) {
            accuracies.emplace_back(
                metadata::PositionalAccuracy::create(toString(accuracy)));
        }

        return createPROJBased(
            properties, exportable, sourceCRS, targetCRS, nullptr, accuracies,
            horizTransform->hasBallparkTransformation() ||
                verticalTransform->hasBallparkTransformation());
    }
}

// ---------------------------------------------------------------------------

static CoordinateOperationNNPtr createHorizVerticalHorizPROJBased(
    const crs::CRSNNPtr &sourceCRS, const crs::CRSNNPtr &targetCRS,
    const operation::CoordinateOperationNNPtr &opSrcCRSToGeogCRS,
    const operation::CoordinateOperationNNPtr &verticalTransform,
    const operation::CoordinateOperationNNPtr &opGeogCRStoDstCRS,
    const crs::GeographicCRSPtr &interpolationGeogCRS, bool checkExtent) {

    auto exportable =
        util::nn_make_shared<MyPROJStringExportableHorizVerticalHorizPROJBased>(
            opSrcCRSToGeogCRS, verticalTransform, opGeogCRStoDstCRS,
            interpolationGeogCRS);

    std::vector<CoordinateOperationNNPtr> ops;
    if (!(starts_with(opSrcCRSToGeogCRS->nameStr(), NULL_GEOGRAPHIC_OFFSET) &&
          opSrcCRSToGeogCRS->nameStr().find(" + ") == std::string::npos)) {
        ops.emplace_back(opSrcCRSToGeogCRS);
    }
    ops.emplace_back(verticalTransform);
    if (!(starts_with(opGeogCRStoDstCRS->nameStr(), NULL_GEOGRAPHIC_OFFSET) &&
          opGeogCRStoDstCRS->nameStr().find(" + ") == std::string::npos)) {
        ops.emplace_back(opGeogCRStoDstCRS);
    }

    bool hasBallparkTransformation = false;
    for (const auto &op : ops) {
        hasBallparkTransformation |= op->hasBallparkTransformation();
    }
    bool emptyIntersection = false;
    auto extent = getExtent(ops, false, emptyIntersection);
    if (checkExtent && emptyIntersection) {
        std::string msg(
            "empty intersection of area of validity of concatenated "
            "operations");
        throw InvalidOperationEmptyIntersection(msg);
    }
    auto properties = util::PropertyMap();
    properties.set(common::IdentifiedObject::NAME_KEY,
                   computeConcatenatedName(ops));

    if (extent) {
        properties.set(common::ObjectUsage::DOMAIN_OF_VALIDITY_KEY,
                       NN_NO_CHECK(extent));
    }

    const auto remarks = getRemarks(ops);
    if (!remarks.empty()) {
        properties.set(common::IdentifiedObject::REMARKS_KEY, remarks);
    }

    std::vector<metadata::PositionalAccuracyNNPtr> accuracies;
    const double accuracy = getAccuracy(ops);
    if (accuracy >= 0.0) {
        accuracies.emplace_back(
            metadata::PositionalAccuracy::create(toString(accuracy)));
    }

    return createPROJBased(properties, exportable, sourceCRS, targetCRS,
                           nullptr, accuracies, hasBallparkTransformation);
}

//! @endcond

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress

std::vector<CoordinateOperationNNPtr>
CoordinateOperationFactory::Private::createOperationsGeogToGeog(
    std::vector<CoordinateOperationNNPtr> &res, const crs::CRSNNPtr &sourceCRS,
    const crs::CRSNNPtr &targetCRS, Private::Context &context,
    const crs::GeographicCRS *geogSrc, const crs::GeographicCRS *geogDst) {

    assert(sourceCRS.get() == geogSrc);
    assert(targetCRS.get() == geogDst);

    const auto &src_pm = geogSrc->primeMeridian()->longitude();
    const auto &dst_pm = geogDst->primeMeridian()->longitude();
    auto offset_pm =
        (src_pm.unit() == dst_pm.unit())
            ? common::Angle(src_pm.value() - dst_pm.value(), src_pm.unit())
            : common::Angle(
                  src_pm.convertToUnit(common::UnitOfMeasure::DEGREE) -
                      dst_pm.convertToUnit(common::UnitOfMeasure::DEGREE),
                  common::UnitOfMeasure::DEGREE);

    double vconvSrc = 1.0;
    const auto &srcCS = geogSrc->coordinateSystem();
    const auto &srcAxisList = srcCS->axisList();
    if (srcAxisList.size() == 3) {
        vconvSrc = srcAxisList[2]->unit().conversionToSI();
    }
    double vconvDst = 1.0;
    const auto &dstCS = geogDst->coordinateSystem();
    const auto &dstAxisList = dstCS->axisList();
    if (dstAxisList.size() == 3) {
        vconvDst = dstAxisList[2]->unit().conversionToSI();
    }

    std::string name(buildTransfName(geogSrc->nameStr(), geogDst->nameStr()));

    const auto &authFactory = context.context->getAuthorityFactory();
    const auto dbContext =
        authFactory ? authFactory->databaseContext().as_nullable() : nullptr;

    const bool sameDatum = geogSrc->datumNonNull(dbContext)->_isEquivalentTo(
        geogDst->datumNonNull(dbContext).get(),
        util::IComparable::Criterion::EQUIVALENT);

    // Do the CRS differ by their axis order ?
    bool axisReversal2D = false;
    bool axisReversal3D = false;
    if (!srcCS->_isEquivalentTo(dstCS.get(),
                                util::IComparable::Criterion::EQUIVALENT)) {
        auto srcOrder = srcCS->axisOrder();
        auto dstOrder = dstCS->axisOrder();
        if (((srcOrder == cs::EllipsoidalCS::AxisOrder::LAT_NORTH_LONG_EAST ||
              srcOrder == cs::EllipsoidalCS::AxisOrder::
                              LAT_NORTH_LONG_EAST_HEIGHT_UP) &&
             (dstOrder == cs::EllipsoidalCS::AxisOrder::LONG_EAST_LAT_NORTH ||
              dstOrder == cs::EllipsoidalCS::AxisOrder::
                              LONG_EAST_LAT_NORTH_HEIGHT_UP)) ||
            ((srcOrder == cs::EllipsoidalCS::AxisOrder::LONG_EAST_LAT_NORTH ||
              srcOrder == cs::EllipsoidalCS::AxisOrder::
                              LONG_EAST_LAT_NORTH_HEIGHT_UP) &&
             (dstOrder == cs::EllipsoidalCS::AxisOrder::LAT_NORTH_LONG_EAST ||
              dstOrder == cs::EllipsoidalCS::AxisOrder::
                              LAT_NORTH_LONG_EAST_HEIGHT_UP))) {
            if (srcAxisList.size() == 3 || dstAxisList.size() == 3)
                axisReversal3D = true;
            else
                axisReversal2D = true;
        }
    }

    // Do they differ by vertical units ?
    if (vconvSrc != vconvDst &&
        geogSrc->ellipsoid()->_isEquivalentTo(
            geogDst->ellipsoid().get(),
            util::IComparable::Criterion::EQUIVALENT)) {
        if (offset_pm.value() == 0 && !axisReversal2D && !axisReversal3D) {
            // If only by vertical units, use a Change of Vertical
            // Unit
            // transformation
            const double factor = vconvSrc / vconvDst;
            auto conv = Conversion::createChangeVerticalUnit(
                util::PropertyMap().set(common::IdentifiedObject::NAME_KEY,
                                        name),
                common::Scale(factor));
            conv->setCRSs(sourceCRS, targetCRS, nullptr);
            conv->setHasBallparkTransformation(!sameDatum);
            res.push_back(conv);
            return res;
        } else {
            auto op = createGeodToGeodPROJBased(sourceCRS, targetCRS);
            op->setHasBallparkTransformation(!sameDatum);
            res.emplace_back(op);
            return res;
        }
    }

    // Do the CRS differ only by their axis order ?
    if (sameDatum && (axisReversal2D || axisReversal3D)) {
        auto conv = Conversion::createAxisOrderReversal(axisReversal3D);
        conv->setCRSs(sourceCRS, targetCRS, nullptr);
        res.emplace_back(conv);
        return res;
    }

    std::vector<CoordinateOperationNNPtr> steps;
    // If both are geographic and only differ by their prime
    // meridian,
    // apply a longitude rotation transformation.
    if (geogSrc->ellipsoid()->_isEquivalentTo(
            geogDst->ellipsoid().get(),
            util::IComparable::Criterion::EQUIVALENT) &&
        src_pm.getSIValue() != dst_pm.getSIValue()) {

        steps.emplace_back(Transformation::createLongitudeRotation(
            util::PropertyMap()
                .set(common::IdentifiedObject::NAME_KEY, name)
                .set(common::ObjectUsage::DOMAIN_OF_VALIDITY_KEY,
                     metadata::Extent::WORLD),
            sourceCRS, targetCRS, offset_pm));
        // If only the target has a non-zero prime meridian, chain a
        // null geographic offset and then the longitude rotation
    } else if (src_pm.getSIValue() == 0 && dst_pm.getSIValue() != 0) {
        auto datum = datum::GeodeticReferenceFrame::create(
            util::PropertyMap(), geogDst->ellipsoid(),
            util::optional<std::string>(), geogSrc->primeMeridian());
        std::string interm_crs_name(geogDst->nameStr());
        interm_crs_name += " altered to use prime meridian of ";
        interm_crs_name += geogSrc->nameStr();
        auto interm_crs =
            util::nn_static_pointer_cast<crs::CRS>(crs::GeographicCRS::create(
                util::PropertyMap()
                    .set(common::IdentifiedObject::NAME_KEY, interm_crs_name)
                    .set(common::ObjectUsage::DOMAIN_OF_VALIDITY_KEY,
                         metadata::Extent::WORLD),
                datum, dstCS));

        steps.emplace_back(
            createBallparkGeographicOffset(sourceCRS, interm_crs, dbContext));

        steps.emplace_back(Transformation::createLongitudeRotation(
            util::PropertyMap()
                .set(common::IdentifiedObject::NAME_KEY,
                     buildTransfName(geogSrc->nameStr(), interm_crs->nameStr()))
                .set(common::ObjectUsage::DOMAIN_OF_VALIDITY_KEY,
                     metadata::Extent::WORLD),
            interm_crs, targetCRS, offset_pm));

    } else {
        // If the prime meridians are different, chain a longitude
        // rotation and the null geographic offset.
        if (src_pm.getSIValue() != dst_pm.getSIValue()) {
            auto datum = datum::GeodeticReferenceFrame::create(
                util::PropertyMap(), geogSrc->ellipsoid(),
                util::optional<std::string>(), geogDst->primeMeridian());
            std::string interm_crs_name(geogSrc->nameStr());
            interm_crs_name += " altered to use prime meridian of ";
            interm_crs_name += geogDst->nameStr();
            auto interm_crs = util::nn_static_pointer_cast<crs::CRS>(
                crs::GeographicCRS::create(
                    util::PropertyMap().set(common::IdentifiedObject::NAME_KEY,
                                            interm_crs_name),
                    datum, srcCS));

            steps.emplace_back(Transformation::createLongitudeRotation(
                util::PropertyMap()
                    .set(common::IdentifiedObject::NAME_KEY,
                         buildTransfName(geogSrc->nameStr(),
                                         interm_crs->nameStr()))
                    .set(common::ObjectUsage::DOMAIN_OF_VALIDITY_KEY,
                         metadata::Extent::WORLD),
                sourceCRS, interm_crs, offset_pm));
            steps.emplace_back(createBallparkGeographicOffset(
                interm_crs, targetCRS, dbContext));
        } else {
            steps.emplace_back(createBallparkGeographicOffset(
                sourceCRS, targetCRS, dbContext));
        }
    }

    auto op = ConcatenatedOperation::createComputeMetadata(
        steps, disallowEmptyIntersection);
    op->setHasBallparkTransformation(!sameDatum);
    res.emplace_back(op);
    return res;
}

// ---------------------------------------------------------------------------

static bool hasIdentifiers(const CoordinateOperationNNPtr &op) {
    if (!op->identifiers().empty()) {
        return true;
    }
    auto concatenated = dynamic_cast<const ConcatenatedOperation *>(op.get());
    if (concatenated) {
        for (const auto &subOp : concatenated->operations()) {
            if (hasIdentifiers(subOp)) {
                return true;
            }
        }
    }
    return false;
}

// ---------------------------------------------------------------------------

static std::vector<crs::CRSNNPtr>
findCandidateGeodCRSForDatum(const io::AuthorityFactoryPtr &authFactory,
                             const crs::GeodeticCRS *crs,
                             const datum::GeodeticReferenceFrame *datum) {
    std::vector<crs::CRSNNPtr> candidates;
    assert(datum);
    const auto &ids = datum->identifiers();
    const auto &datumName = datum->nameStr();
    if (!ids.empty()) {
        for (const auto &id : ids) {
            const auto &authName = *(id->codeSpace());
            const auto &code = id->code();
            if (!authName.empty()) {
                const auto crsIds = crs->identifiers();
                const auto tmpFactory =
                    (crsIds.size() == 1 &&
                     *(crsIds.front()->codeSpace()) == authName)
                        ? io::AuthorityFactory::create(
                              authFactory->databaseContext(), authName)
                              .as_nullable()
                        : authFactory;
                auto l_candidates = tmpFactory->createGeodeticCRSFromDatum(
                    authName, code, std::string());
                for (const auto &candidate : l_candidates) {
                    candidates.emplace_back(candidate);
                }
            }
        }
    } else if (datumName != "unknown" && datumName != "unnamed") {
        auto matches = authFactory->createObjectsFromName(
            datumName,
            {io::AuthorityFactory::ObjectType::GEODETIC_REFERENCE_FRAME}, false,
            2);
        if (matches.size() == 1) {
            const auto &match = matches.front();
            if (datum->_isEquivalentTo(
                    match.get(), util::IComparable::Criterion::EQUIVALENT) &&
                !match->identifiers().empty()) {
                return findCandidateGeodCRSForDatum(
                    authFactory, crs,
                    dynamic_cast<const datum::GeodeticReferenceFrame *>(
                        match.get()));
            }
        }
    }
    return candidates;
}

// ---------------------------------------------------------------------------

void CoordinateOperationFactory::Private::setCRSs(
    CoordinateOperation *co, const crs::CRSNNPtr &sourceCRS,
    const crs::CRSNNPtr &targetCRS) {
    co->setCRSs(sourceCRS, targetCRS, nullptr);

    auto invCO = dynamic_cast<InverseCoordinateOperation *>(co);
    if (invCO) {
        invCO->forwardOperation()->setCRSs(targetCRS, sourceCRS, nullptr);
    }

    auto transf = dynamic_cast<Transformation *>(co);
    if (transf) {
        transf->inverseAsTransformation()->setCRSs(targetCRS, sourceCRS,
                                                   nullptr);
    }

    auto concat = dynamic_cast<ConcatenatedOperation *>(co);
    if (concat) {
        auto first = concat->operations().front().get();
        auto &firstTarget(first->targetCRS());
        if (firstTarget) {
            setCRSs(first, sourceCRS, NN_NO_CHECK(firstTarget));
        }
        auto last = concat->operations().back().get();
        auto &lastSource(last->sourceCRS());
        if (lastSource) {
            setCRSs(last, NN_NO_CHECK(lastSource), targetCRS);
        }
    }
}

// ---------------------------------------------------------------------------

static bool hasResultSetOnlyResultsWithPROJStep(
    const std::vector<CoordinateOperationNNPtr> &res) {
    for (const auto &op : res) {
        auto concat = dynamic_cast<const ConcatenatedOperation *>(op.get());
        if (concat) {
            bool hasPROJStep = false;
            const auto &steps = concat->operations();
            for (const auto &step : steps) {
                const auto &ids = step->identifiers();
                if (!ids.empty()) {
                    const auto &opAuthority = *(ids.front()->codeSpace());
                    if (opAuthority == "PROJ" ||
                        opAuthority == "INVERSE(PROJ)" ||
                        opAuthority == "DERIVED_FROM(PROJ)") {
                        hasPROJStep = true;
                        break;
                    }
                }
            }
            if (!hasPROJStep) {
                return false;
            }
        } else {
            return false;
        }
    }
    return true;
}

// ---------------------------------------------------------------------------

void CoordinateOperationFactory::Private::createOperationsWithDatumPivot(
    std::vector<CoordinateOperationNNPtr> &res, const crs::CRSNNPtr &sourceCRS,
    const crs::CRSNNPtr &targetCRS, const crs::GeodeticCRS *geodSrc,
    const crs::GeodeticCRS *geodDst, Private::Context &context) {

#ifdef TRACE_CREATE_OPERATIONS
    ENTER_BLOCK("createOperationsWithDatumPivot(" +
                objectAsStr(sourceCRS.get()) + "," +
                objectAsStr(targetCRS.get()) + ")");
#endif

    struct CreateOperationsWithDatumPivotAntiRecursion {
        Context &context;

        explicit CreateOperationsWithDatumPivotAntiRecursion(Context &contextIn)
            : context(contextIn) {
            assert(!context.inCreateOperationsWithDatumPivotAntiRecursion);
            context.inCreateOperationsWithDatumPivotAntiRecursion = true;
        }

        ~CreateOperationsWithDatumPivotAntiRecursion() {
            context.inCreateOperationsWithDatumPivotAntiRecursion = false;
        }
    };
    CreateOperationsWithDatumPivotAntiRecursion guard(context);

    const auto &authFactory = context.context->getAuthorityFactory();
    const auto &dbContext = authFactory->databaseContext();

    const auto candidatesSrcGeod(findCandidateGeodCRSForDatum(
        authFactory, geodSrc,
        geodSrc->datumNonNull(dbContext.as_nullable()).get()));
    const auto candidatesDstGeod(findCandidateGeodCRSForDatum(
        authFactory, geodDst,
        geodDst->datumNonNull(dbContext.as_nullable()).get()));

    const bool sourceAndTargetAre3D =
        geodSrc->coordinateSystem()->axisList().size() == 3 &&
        geodDst->coordinateSystem()->axisList().size() == 3;

    auto createTransformations = [&](const crs::CRSNNPtr &candidateSrcGeod,
                                     const crs::CRSNNPtr &candidateDstGeod,
                                     const CoordinateOperationNNPtr &opFirst,
                                     bool isNullFirst) {
        const auto opsSecond =
            createOperations(candidateSrcGeod, candidateDstGeod, context);
        const auto opsThird =
            createOperations(candidateDstGeod, targetCRS, context);
        assert(!opsThird.empty());

        for (auto &opSecond : opsSecond) {
            // Check that it is not a transformation synthetized by
            // ourselves
            if (!hasIdentifiers(opSecond)) {
                continue;
            }
            // And even if it is a referenced transformation, check that
            // it is not a trivial one
            auto so = dynamic_cast<const SingleOperation *>(opSecond.get());
            if (so && isAxisOrderReversal(so->method()->getEPSGCode())) {
                continue;
            }

            std::vector<CoordinateOperationNNPtr> subOps;
            const bool isNullThird =
                isNullTransformation(opsThird[0]->nameStr());
            CoordinateOperationNNPtr opSecondCloned(
                (isNullFirst || isNullThird || sourceAndTargetAre3D)
                    ? opSecond->shallowClone()
                    : opSecond);
            if (isNullFirst || isNullThird) {
                if (opSecondCloned->identifiers().size() == 1 &&
                    (*opSecondCloned->identifiers()[0]->codeSpace())
                            .find("DERIVED_FROM") == std::string::npos) {
                    {
                        util::PropertyMap map;
                        addModifiedIdentifier(map, opSecondCloned.get(), false,
                                              true);
                        opSecondCloned->setProperties(map);
                    }
                    auto invCO = dynamic_cast<InverseCoordinateOperation *>(
                        opSecondCloned.get());
                    if (invCO) {
                        auto invCOForward = invCO->forwardOperation().get();
                        if (invCOForward->identifiers().size() == 1 &&
                            (*invCOForward->identifiers()[0]->codeSpace())
                                    .find("DERIVED_FROM") ==
                                std::string::npos) {
                            util::PropertyMap map;
                            addModifiedIdentifier(map, invCOForward, false,
                                                  true);
                            invCOForward->setProperties(map);
                        }
                    }
                }
            }
            if (sourceAndTargetAre3D) {
                opSecondCloned->getPrivate()->use3DHelmert_ = true;
                auto invCO = dynamic_cast<InverseCoordinateOperation *>(
                    opSecondCloned.get());
                if (invCO) {
                    auto invCOForward = invCO->forwardOperation().get();
                    invCOForward->getPrivate()->use3DHelmert_ = true;
                }
            }
            if (isNullFirst) {
                auto oldTarget(NN_CHECK_ASSERT(opSecondCloned->targetCRS()));
                setCRSs(opSecondCloned.get(), sourceCRS, oldTarget);
            } else {
                subOps.emplace_back(opFirst);
            }
            if (isNullThird) {
                auto oldSource(NN_CHECK_ASSERT(opSecondCloned->sourceCRS()));
                setCRSs(opSecondCloned.get(), oldSource, targetCRS);
                subOps.emplace_back(opSecondCloned);
            } else {
                subOps.emplace_back(opSecondCloned);
                subOps.emplace_back(opsThird[0]);
            }
#ifdef TRACE_CREATE_OPERATIONS
            std::string debugStr;
            for (const auto &op : subOps) {
                if (!debugStr.empty()) {
                    debugStr += " + ";
                }
                debugStr += objectAsStr(op.get());
                debugStr += " (";
                debugStr += objectAsStr(op->sourceCRS().get());
                debugStr += "->";
                debugStr += objectAsStr(op->targetCRS().get());
                debugStr += ")";
            }
            logTrace("transformation " + debugStr);
#endif
            try {
                res.emplace_back(ConcatenatedOperation::createComputeMetadata(
                    subOps, disallowEmptyIntersection));
            } catch (const InvalidOperationEmptyIntersection &) {
            }
        }
    };

    // Start in priority with candidates that have exactly the same name as
    // the sourcCRS and targetCRS. Typically for the case of init=IGNF:XXXX

    // Transformation from IGNF:NTFP to IGNF:RGF93G,
    // using
    // NTF geographiques Paris (gr) vers NTF GEOGRAPHIQUES GREENWICH (DMS) +
    // NOUVELLE TRIANGULATION DE LA FRANCE (NTF) vers RGF93 (ETRS89)
    // that is using ntf_r93.gsb, is horribly dependent
    // of IGNF:RGF93G being returned before IGNF:RGF93GEO in candidatesDstGeod.
    // If RGF93GEO is returned before then we go through WGS84 and use
    // instead a Helmert transformation.
    // The below logic is thus quite fragile, and attempts at changing it
    // result in degraded results for other use cases...

    for (const auto &candidateSrcGeod : candidatesSrcGeod) {
        if (candidateSrcGeod->nameStr() == sourceCRS->nameStr()) {
            for (const auto &candidateDstGeod : candidatesDstGeod) {
                if (candidateDstGeod->nameStr() == targetCRS->nameStr()) {
#ifdef TRACE_CREATE_OPERATIONS
                    ENTER_BLOCK("try " + objectAsStr(sourceCRS.get()) + "->" +
                                objectAsStr(candidateSrcGeod.get()) + "->" +
                                objectAsStr(candidateDstGeod.get()) + "->" +
                                objectAsStr(targetCRS.get()) + ")");
#endif
                    const auto opsFirst =
                        createOperations(sourceCRS, candidateSrcGeod, context);
                    assert(!opsFirst.empty());
                    const bool isNullFirst =
                        isNullTransformation(opsFirst[0]->nameStr());
                    createTransformations(candidateSrcGeod, candidateDstGeod,
                                          opsFirst[0], isNullFirst);
                    if (!res.empty()) {
                        if (hasResultSetOnlyResultsWithPROJStep(res)) {
                            continue;
                        }
                        return;
                    }
                }
            }
        }
    }

    for (const auto &candidateSrcGeod : candidatesSrcGeod) {
        const bool bSameSrcName =
            candidateSrcGeod->nameStr() == sourceCRS->nameStr();
#ifdef TRACE_CREATE_OPERATIONS
        ENTER_BLOCK("");
#endif
        const auto opsFirst =
            createOperations(sourceCRS, candidateSrcGeod, context);
        assert(!opsFirst.empty());
        const bool isNullFirst = isNullTransformation(opsFirst[0]->nameStr());

        for (const auto &candidateDstGeod : candidatesDstGeod) {
            if (bSameSrcName &&
                candidateDstGeod->nameStr() == targetCRS->nameStr()) {
                continue;
            }

#ifdef TRACE_CREATE_OPERATIONS
            ENTER_BLOCK("try " + objectAsStr(sourceCRS.get()) + "->" +
                        objectAsStr(candidateSrcGeod.get()) + "->" +
                        objectAsStr(candidateDstGeod.get()) + "->" +
                        objectAsStr(targetCRS.get()) + ")");
#endif
            createTransformations(candidateSrcGeod, candidateDstGeod,
                                  opsFirst[0], isNullFirst);
            if (!res.empty() && !hasResultSetOnlyResultsWithPROJStep(res)) {
                return;
            }
        }
    }
}

// ---------------------------------------------------------------------------

static CoordinateOperationNNPtr
createBallparkGeocentricTranslation(const crs::CRSNNPtr &sourceCRS,
                                    const crs::CRSNNPtr &targetCRS) {
    std::string name(BALLPARK_GEOCENTRIC_TRANSLATION);
    name += " from ";
    name += sourceCRS->nameStr();
    name += " to ";
    name += targetCRS->nameStr();

    return util::nn_static_pointer_cast<CoordinateOperation>(
        Transformation::createGeocentricTranslations(
            util::PropertyMap()
                .set(common::IdentifiedObject::NAME_KEY, name)
                .set(common::ObjectUsage::DOMAIN_OF_VALIDITY_KEY,
                     metadata::Extent::WORLD),
            sourceCRS, targetCRS, 0.0, 0.0, 0.0, {}));
}

// ---------------------------------------------------------------------------

bool CoordinateOperationFactory::Private::hasPerfectAccuracyResult(
    const std::vector<CoordinateOperationNNPtr> &res, const Context &context) {
    auto resTmp = FilterResults(res, context.context, context.extent1,
                                context.extent2, true)
                      .getRes();
    for (const auto &op : resTmp) {
        const double acc = getAccuracy(op);
        if (acc == 0.0) {
            return true;
        }
    }
    return false;
}

// ---------------------------------------------------------------------------

std::vector<CoordinateOperationNNPtr>
CoordinateOperationFactory::Private::createOperations(
    const crs::CRSNNPtr &sourceCRS, const crs::CRSNNPtr &targetCRS,
    Private::Context &context) {

#ifdef TRACE_CREATE_OPERATIONS
    ENTER_BLOCK("createOperations(" + objectAsStr(sourceCRS.get()) + " --> " +
                objectAsStr(targetCRS.get()) + ")");
#endif

    std::vector<CoordinateOperationNNPtr> res;

    auto boundSrc = dynamic_cast<const crs::BoundCRS *>(sourceCRS.get());
    auto boundDst = dynamic_cast<const crs::BoundCRS *>(targetCRS.get());

    const auto &sourceProj4Ext = boundSrc
                                     ? boundSrc->baseCRS()->getExtensionProj4()
                                     : sourceCRS->getExtensionProj4();
    const auto &targetProj4Ext = boundDst
                                     ? boundDst->baseCRS()->getExtensionProj4()
                                     : targetCRS->getExtensionProj4();
    if (!sourceProj4Ext.empty() || !targetProj4Ext.empty()) {
        createOperationsFromProj4Ext(sourceCRS, targetCRS, boundSrc, boundDst,
                                     res);
        return res;
    }

    auto geodSrc = dynamic_cast<const crs::GeodeticCRS *>(sourceCRS.get());
    auto geodDst = dynamic_cast<const crs::GeodeticCRS *>(targetCRS.get());
    auto geogSrc = dynamic_cast<const crs::GeographicCRS *>(sourceCRS.get());
    auto geogDst = dynamic_cast<const crs::GeographicCRS *>(targetCRS.get());
    auto vertSrc = dynamic_cast<const crs::VerticalCRS *>(sourceCRS.get());
    auto vertDst = dynamic_cast<const crs::VerticalCRS *>(targetCRS.get());

    // First look-up if the registry provide us with operations.
    auto derivedSrc = dynamic_cast<const crs::DerivedCRS *>(sourceCRS.get());
    auto derivedDst = dynamic_cast<const crs::DerivedCRS *>(targetCRS.get());
    const auto &authFactory = context.context->getAuthorityFactory();
    if (authFactory &&
        (derivedSrc == nullptr ||
         !derivedSrc->baseCRS()->_isEquivalentTo(
             targetCRS.get(), util::IComparable::Criterion::EQUIVALENT)) &&
        (derivedDst == nullptr ||
         !derivedDst->baseCRS()->_isEquivalentTo(
             sourceCRS.get(), util::IComparable::Criterion::EQUIVALENT))) {

        if (createOperationsFromDatabase(sourceCRS, targetCRS, context, geodSrc,
                                         geodDst, geogSrc, geogDst, vertSrc,
                                         vertDst, res)) {
            return res;
        }
    }

    // Special case if both CRS are geodetic
    if (geodSrc && geodDst && !derivedSrc && !derivedDst) {
        createOperationsGeodToGeod(sourceCRS, targetCRS, context, geodSrc,
                                   geodDst, res);
        return res;
    }

    // If the source is a derived CRS, then chain the inverse of its
    // deriving conversion, with transforms from its baseCRS to the
    // targetCRS
    if (derivedSrc) {
        createOperationsDerivedTo(sourceCRS, targetCRS, context, derivedSrc,
                                  res);
        return res;
    }

    // reverse of previous case
    if (derivedDst) {
        return applyInverse(createOperations(targetCRS, sourceCRS, context));
    }

    // Order of comparison between the geogDst vs geodDst is impotant
    if (boundSrc && geogDst) {
        createOperationsBoundToGeog(sourceCRS, targetCRS, context, boundSrc,
                                    geogDst, res);
        return res;
    } else if (boundSrc && geodDst) {
        createOperationsToGeod(sourceCRS, targetCRS, context, geodDst, res);
        return res;
    }

    // reverse of previous case
    if (geodSrc && boundDst) {
        return applyInverse(createOperations(targetCRS, sourceCRS, context));
    }

    // vertCRS (as boundCRS with transformation to target vertCRS) to
    // vertCRS
    if (boundSrc && vertDst) {
        createOperationsBoundToVert(sourceCRS, targetCRS, context, boundSrc,
                                    vertDst, res);
        return res;
    }

    // reverse of previous case
    if (boundDst && vertSrc) {
        return applyInverse(createOperations(targetCRS, sourceCRS, context));
    }

    if (vertSrc && vertDst) {
        createOperationsVertToVert(sourceCRS, targetCRS, context, vertSrc,
                                   vertDst, res);
        return res;
    }

    // A bit odd case as we are comparing apples to oranges, but in case
    // the vertical unit differ, do something useful.
    if (vertSrc && geogDst) {
        createOperationsVertToGeog(sourceCRS, targetCRS, context, vertSrc,
                                   geogDst, res);
        return res;
    }

    // reverse of previous case
    if (vertDst && geogSrc) {
        return applyInverse(createOperations(targetCRS, sourceCRS, context));
    }

    // boundCRS to boundCRS
    if (boundSrc && boundDst) {
        createOperationsBoundToBound(sourceCRS, targetCRS, context, boundSrc,
                                     boundDst, res);
        return res;
    }

    auto compoundSrc = dynamic_cast<crs::CompoundCRS *>(sourceCRS.get());
    // Order of comparison between the geogDst vs geodDst is impotant
    if (compoundSrc && geogDst) {
        createOperationsCompoundToGeog(sourceCRS, targetCRS, context,
                                       compoundSrc, geogDst, res);
        return res;
    } else if (compoundSrc && geodDst) {
        createOperationsToGeod(sourceCRS, targetCRS, context, geodDst, res);
        return res;
    }

    // reverse of previous cases
    auto compoundDst = dynamic_cast<const crs::CompoundCRS *>(targetCRS.get());
    if (geodSrc && compoundDst) {
        return applyInverse(createOperations(targetCRS, sourceCRS, context));
    }

    if (compoundSrc && compoundDst) {
        createOperationsCompoundToCompound(sourceCRS, targetCRS, context,
                                           compoundSrc, compoundDst, res);
        return res;
    }

    // '+proj=longlat +ellps=GRS67 +nadgrids=@foo.gsb +type=crs' to
    // '+proj=longlat +ellps=GRS80 +nadgrids=@bar.gsb +geoidgrids=@bar.gtx
    // +type=crs'
    if (boundSrc && compoundDst) {
        createOperationsBoundToCompound(sourceCRS, targetCRS, context, boundSrc,
                                        compoundDst, res);
        return res;
    }

    // reverse of previous case
    if (boundDst && compoundSrc) {
        return applyInverse(createOperations(targetCRS, sourceCRS, context));
    }

    return res;
}

// ---------------------------------------------------------------------------

void CoordinateOperationFactory::Private::createOperationsFromProj4Ext(
    const crs::CRSNNPtr &sourceCRS, const crs::CRSNNPtr &targetCRS,
    const crs::BoundCRS *boundSrc, const crs::BoundCRS *boundDst,
    std::vector<CoordinateOperationNNPtr> &res) {

    ENTER_FUNCTION();

    auto sourceProjExportable = dynamic_cast<const io::IPROJStringExportable *>(
        boundSrc ? boundSrc : sourceCRS.get());
    auto targetProjExportable = dynamic_cast<const io::IPROJStringExportable *>(
        boundDst ? boundDst : targetCRS.get());
    if (!sourceProjExportable) {
        throw InvalidOperation("Source CRS is not PROJ exportable");
    }
    if (!targetProjExportable) {
        throw InvalidOperation("Target CRS is not PROJ exportable");
    }
    auto projFormatter = io::PROJStringFormatter::create();
    projFormatter->setCRSExport(true);
    projFormatter->setLegacyCRSToCRSContext(true);
    projFormatter->startInversion();
    sourceProjExportable->_exportToPROJString(projFormatter.get());
    auto geogSrc = dynamic_cast<const crs::GeographicCRS *>(sourceCRS.get());
    if (geogSrc) {
        auto tmpFormatter = io::PROJStringFormatter::create();
        geogSrc->addAngularUnitConvertAndAxisSwap(tmpFormatter.get());
        projFormatter->ingestPROJString(tmpFormatter->toString());
    }

    projFormatter->stopInversion();

    targetProjExportable->_exportToPROJString(projFormatter.get());
    auto geogDst = dynamic_cast<const crs::GeographicCRS *>(targetCRS.get());
    if (geogDst) {
        auto tmpFormatter = io::PROJStringFormatter::create();
        geogDst->addAngularUnitConvertAndAxisSwap(tmpFormatter.get());
        projFormatter->ingestPROJString(tmpFormatter->toString());
    }

    const auto PROJString = projFormatter->toString();
    auto properties = util::PropertyMap().set(
        common::IdentifiedObject::NAME_KEY,
        buildTransfName(sourceCRS->nameStr(), targetCRS->nameStr()));
    res.emplace_back(SingleOperation::createPROJBased(
        properties, PROJString, sourceCRS, targetCRS, {}));
}

// ---------------------------------------------------------------------------

bool CoordinateOperationFactory::Private::createOperationsFromDatabase(
    const crs::CRSNNPtr &sourceCRS, const crs::CRSNNPtr &targetCRS,
    Private::Context &context, const crs::GeodeticCRS *geodSrc,
    const crs::GeodeticCRS *geodDst, const crs::GeographicCRS *geogSrc,
    const crs::GeographicCRS *geogDst, const crs::VerticalCRS *vertSrc,
    const crs::VerticalCRS *vertDst,
    std::vector<CoordinateOperationNNPtr> &res) {

    ENTER_FUNCTION();

    if (geogSrc && vertDst) {
        createOperationsFromDatabase(targetCRS, sourceCRS, context, geodDst,
                                     geodSrc, geogDst, geogSrc, vertDst,
                                     vertSrc, res);
        res = applyInverse(res);
    } else if (geogDst && vertSrc) {
        res = applyInverse(createOperationsGeogToVertFromGeoid(
            targetCRS, sourceCRS, vertSrc, context));
        if (!res.empty()) {
            createOperationsVertToGeogBallpark(sourceCRS, targetCRS, context,
                                               vertSrc, geogDst, res);
        }
    }

    if (!res.empty()) {
        return true;
    }

    bool resFindDirectNonEmptyBeforeFiltering = false;
    res = findOpsInRegistryDirect(sourceCRS, targetCRS, context,
                                  resFindDirectNonEmptyBeforeFiltering);

    // If we get at least a result with perfect accuracy, do not
    // bother generating synthetic transforms.
    if (hasPerfectAccuracyResult(res, context)) {
        return true;
    }

    bool doFilterAndCheckPerfectOp = false;

    bool sameGeodeticDatum = false;

    if (vertSrc || vertDst) {
        if (res.empty()) {
            if (geogSrc &&
                geogSrc->coordinateSystem()->axisList().size() == 2 &&
                vertDst) {
                auto dbContext =
                    context.context->getAuthorityFactory()->databaseContext();
                auto resTmp = findOpsInRegistryDirect(
                    sourceCRS->promoteTo3D(std::string(), dbContext), targetCRS,
                    context, resFindDirectNonEmptyBeforeFiltering);
                for (auto &op : resTmp) {
                    auto newOp = op->shallowClone();
                    setCRSs(newOp.get(), sourceCRS, targetCRS);
                    res.emplace_back(newOp);
                }
            } else if (geogDst &&
                       geogDst->coordinateSystem()->axisList().size() == 2 &&
                       vertSrc) {
                auto dbContext =
                    context.context->getAuthorityFactory()->databaseContext();
                auto resTmp = findOpsInRegistryDirect(
                    sourceCRS, targetCRS->promoteTo3D(std::string(), dbContext),
                    context, resFindDirectNonEmptyBeforeFiltering);
                for (auto &op : resTmp) {
                    auto newOp = op->shallowClone();
                    setCRSs(newOp.get(), sourceCRS, targetCRS);
                    res.emplace_back(newOp);
                }
            }
        }
        if (res.empty()) {
            createOperationsFromDatabaseWithVertCRS(sourceCRS, targetCRS,
                                                    context, geogSrc, geogDst,
                                                    vertSrc, vertDst, res);
        }
    } else if (geodSrc && geodDst) {

        const auto &authFactory = context.context->getAuthorityFactory();
        const auto dbContext = authFactory->databaseContext().as_nullable();

        const auto srcDatum = geodSrc->datumNonNull(dbContext);
        const auto dstDatum = geodDst->datumNonNull(dbContext);
        sameGeodeticDatum = srcDatum->_isEquivalentTo(
            dstDatum.get(), util::IComparable::Criterion::EQUIVALENT);

        if (res.empty() && !sameGeodeticDatum &&
            !context.inCreateOperationsWithDatumPivotAntiRecursion) {
            // If we still didn't find a transformation, and that the source
            // and target are GeodeticCRS, then go through their underlying
            // datum to find potential transformations between other
            // GeodeticCRSs
            // that are made of those datum
            // The typical example is if transforming between two
            // GeographicCRS,
            // but transformations are only available between their
            // corresponding geocentric CRS.
            createOperationsWithDatumPivot(res, sourceCRS, targetCRS, geodSrc,
                                           geodDst, context);
            doFilterAndCheckPerfectOp = !res.empty();
        }
    }

    bool foundInstantiableOp = false;
    // FIXME: the limitation to .size() == 1 is just for the
    // -s EPSG:4959+5759 -t "EPSG:4959+7839" case
    // finding EPSG:7860 'NZVD2016 height to Auckland 1946
    // height (1)', which uses the EPSG:1071 'Vertical Offset by Grid
    // Interpolation (NZLVD)' method which is not currently implemented by PROJ
    // (cannot deal with .csv files)
    // Initially the test was written to iterate over for all operations of a
    // non-empty res, but this causes failures in the test suite when no grids
    // are installed at all. Ideally we should tweak the test suite to be
    // robust to that, or skip some tests.
    if (res.size() == 1) {
        try {
            res.front()->exportToPROJString(
                io::PROJStringFormatter::create().get());
            foundInstantiableOp = true;
        } catch (const std::exception &) {
        }
        if (!foundInstantiableOp) {
            resFindDirectNonEmptyBeforeFiltering = false;
        }
    } else if (res.size() > 1) {
        foundInstantiableOp = true;
    }

    // NAD27 to NAD83 has tens of results already. No need to look
    // for a pivot
    if (!sameGeodeticDatum &&
        (((res.empty() || !foundInstantiableOp) &&
          !resFindDirectNonEmptyBeforeFiltering &&
          context.context->getAllowUseIntermediateCRS() ==
              CoordinateOperationContext::IntermediateCRSUse::
                  IF_NO_DIRECT_TRANSFORMATION) ||
         context.context->getAllowUseIntermediateCRS() ==
             CoordinateOperationContext::IntermediateCRSUse::ALWAYS ||
         getenv("PROJ_FORCE_SEARCH_PIVOT"))) {
        auto resWithIntermediate = findsOpsInRegistryWithIntermediate(
            sourceCRS, targetCRS, context, false);
        res.insert(res.end(), resWithIntermediate.begin(),
                   resWithIntermediate.end());
        doFilterAndCheckPerfectOp = !res.empty();

    } else if (!context.inCreateOperationsWithDatumPivotAntiRecursion &&
               !resFindDirectNonEmptyBeforeFiltering && geodSrc && geodDst &&
               !sameGeodeticDatum &&
               context.context->getIntermediateCRS().empty() &&
               context.context->getAllowUseIntermediateCRS() !=
                   CoordinateOperationContext::IntermediateCRSUse::NEVER) {

        bool tryWithGeodeticDatumIntermediate = res.empty();
        if (!tryWithGeodeticDatumIntermediate) {
            // This is in particular for the GDA94 to WGS 84 (G1762) case
            // As we have a WGS 84 -> WGS 84 (G1762) null-transformation in the
            // PROJ authority, previous steps might have use that WGS 84
            // intermediate directly. They might also have generated a path
            // through ITRF2008, as there is a path
            // GDA94 (geoc.) -> ITRF2008 (geoc.) -> WGS84 (G1762) (geoc.)
            // But there's a better path using
            // GDA94 (geog.) --> GDA2020 (geog.) and
            // GDA2020 (geoc.) -> WGS84 (G1762) (geoc.) that requires to
            // explore intermediates through their datum, and not directly
            // trough the CRS code.
            // Do that only if the number of results we got through other
            // algorithms is small, or if all results we have go through an
            // operation in the PROJ authority.
            constexpr size_t ARBITRARY_SMALL_NUMBER = 5U;
            tryWithGeodeticDatumIntermediate =
                res.size() < ARBITRARY_SMALL_NUMBER ||
                hasResultSetOnlyResultsWithPROJStep(res);
        }
        if (tryWithGeodeticDatumIntermediate) {
            auto resWithIntermediate = findsOpsInRegistryWithIntermediate(
                sourceCRS, targetCRS, context, true);
            res.insert(res.end(), resWithIntermediate.begin(),
                       resWithIntermediate.end());
            doFilterAndCheckPerfectOp = !res.empty();
        }
    }

    if (doFilterAndCheckPerfectOp) {
        // If we get at least a result with perfect accuracy, do not bother
        // generating synthetic transforms.
        if (hasPerfectAccuracyResult(res, context)) {
            return true;
        }
    }
    return false;
}

// ---------------------------------------------------------------------------

static std::vector<crs::CRSNNPtr>
findCandidateVertCRSForDatum(const io::AuthorityFactoryPtr &authFactory,
                             const datum::VerticalReferenceFrame *datum) {
    std::vector<crs::CRSNNPtr> candidates;
    assert(datum);
    const auto &ids = datum->identifiers();
    const auto &datumName = datum->nameStr();
    if (!ids.empty()) {
        for (const auto &id : ids) {
            const auto &authName = *(id->codeSpace());
            const auto &code = id->code();
            if (!authName.empty()) {
                auto l_candidates =
                    authFactory->createVerticalCRSFromDatum(authName, code);
                for (const auto &candidate : l_candidates) {
                    candidates.emplace_back(candidate);
                }
            }
        }
    } else if (datumName != "unknown" && datumName != "unnamed") {
        auto matches = authFactory->createObjectsFromName(
            datumName,
            {io::AuthorityFactory::ObjectType::VERTICAL_REFERENCE_FRAME}, false,
            2);
        if (matches.size() == 1) {
            const auto &match = matches.front();
            if (datum->_isEquivalentTo(
                    match.get(), util::IComparable::Criterion::EQUIVALENT) &&
                !match->identifiers().empty()) {
                return findCandidateVertCRSForDatum(
                    authFactory,
                    dynamic_cast<const datum::VerticalReferenceFrame *>(
                        match.get()));
            }
        }
    }
    return candidates;
}

// ---------------------------------------------------------------------------

std::vector<CoordinateOperationNNPtr>
CoordinateOperationFactory::Private::createOperationsGeogToVertFromGeoid(
    const crs::CRSNNPtr &sourceCRS, const crs::CRSNNPtr &targetCRS,
    const crs::VerticalCRS *vertDst, Private::Context &context) {

    ENTER_FUNCTION();

    const auto useTransf = [&targetCRS, &context,
                            vertDst](const CoordinateOperationNNPtr &op) {
        const auto targetOp =
            dynamic_cast<const crs::VerticalCRS *>(op->targetCRS().get());
        assert(targetOp);
        if (targetOp->_isEquivalentTo(
                vertDst, util::IComparable::Criterion::EQUIVALENT)) {
            return op;
        }
        std::vector<CoordinateOperationNNPtr> tmp;
        createOperationsVertToVert(NN_NO_CHECK(op->targetCRS()), targetCRS,
                                   context, targetOp, vertDst, tmp);
        assert(!tmp.empty());
        auto ret = ConcatenatedOperation::createComputeMetadata(
            {op, tmp.front()}, disallowEmptyIntersection);
        return ret;
    };

    const auto getProjGeoidTransformation = [&sourceCRS, &targetCRS, &vertDst,
                                             &context](
        const CoordinateOperationNNPtr &model,
        const std::string &projFilename) {

        const auto getNameVertCRSMetre = [](const std::string &name) {
            if (name.empty())
                return std::string("unnamed");
            auto ret(name);
            bool haveOriginalUnit = false;
            if (name.back() == ')') {
                const auto pos = ret.rfind(" (");
                if (pos != std::string::npos) {
                    haveOriginalUnit = true;
                    ret = ret.substr(0, pos);
                }
            }
            const auto pos = ret.rfind(" depth");
            if (pos != std::string::npos) {
                ret = ret.substr(0, pos) + " height";
            }
            if (!haveOriginalUnit) {
                ret += " (metre)";
            }
            return ret;
        };

        const auto &axis = vertDst->coordinateSystem()->axisList()[0];
        const auto geogSrcCRS =
            dynamic_cast<crs::GeographicCRS *>(model->interpolationCRS().get())
                ? NN_NO_CHECK(model->interpolationCRS())
                : sourceCRS;
        const auto vertCRSMetre =
            axis->unit() == common::UnitOfMeasure::METRE &&
                    axis->direction() == cs::AxisDirection::UP
                ? targetCRS
                : util::nn_static_pointer_cast<crs::CRS>(
                      crs::VerticalCRS::create(
                          util::PropertyMap().set(
                              common::IdentifiedObject::NAME_KEY,
                              getNameVertCRSMetre(targetCRS->nameStr())),
                          vertDst->datum(), vertDst->datumEnsemble(),
                          cs::VerticalCS::createGravityRelatedHeight(
                              common::UnitOfMeasure::METRE)));
        const auto properties = util::PropertyMap().set(
            common::IdentifiedObject::NAME_KEY,
            buildOpName("Transformation", vertCRSMetre, geogSrcCRS));

        // Try to find a representative value for the accuracy of this grid
        // from the registered transformations.
        std::vector<metadata::PositionalAccuracyNNPtr> accuracies;
        const auto &modelAccuracies = model->coordinateOperationAccuracies();
        if (modelAccuracies.empty()) {
            const auto &authFactory = context.context->getAuthorityFactory();
            if (authFactory) {
                const auto transformationsForGrid =
                    io::DatabaseContext::getTransformationsForGridName(
                        authFactory->databaseContext(), projFilename);
                double accuracy = -1;
                for (const auto &transf : transformationsForGrid) {
                    accuracy = std::max(accuracy, getAccuracy(transf));
                }
                if (accuracy >= 0) {
                    accuracies.emplace_back(
                        metadata::PositionalAccuracy::create(
                            toString(accuracy)));
                }
            }
        }

        return Transformation::createGravityRelatedHeightToGeographic3D(
            properties, vertCRSMetre, geogSrcCRS, nullptr, projFilename,
            !modelAccuracies.empty() ? modelAccuracies : accuracies);
    };

    std::vector<CoordinateOperationNNPtr> res;
    const auto &authFactory = context.context->getAuthorityFactory();
    if (authFactory) {
        const auto &models = vertDst->geoidModel();
        for (const auto &model : models) {
            const auto &modelName = model->nameStr();
            const auto transformations =
                starts_with(modelName, "PROJ ")
                    ? std::vector<
                          CoordinateOperationNNPtr>{getProjGeoidTransformation(
                          model, modelName.substr(strlen("PROJ ")))}
                    : authFactory->getTransformationsForGeoid(
                          modelName,
                          context.context->getUsePROJAlternativeGridNames());
            for (const auto &transf : transformations) {
                if (dynamic_cast<crs::GeographicCRS *>(
                        transf->sourceCRS().get()) &&
                    dynamic_cast<crs::VerticalCRS *>(
                        transf->targetCRS().get())) {
                    res.push_back(useTransf(transf));
                } else if (dynamic_cast<crs::GeographicCRS *>(
                               transf->targetCRS().get()) &&
                           dynamic_cast<crs::VerticalCRS *>(
                               transf->sourceCRS().get())) {
                    res.push_back(useTransf(transf->inverse()));
                }
            }
        }
    }

    return res;
}

// ---------------------------------------------------------------------------

std::vector<CoordinateOperationNNPtr> CoordinateOperationFactory::Private::
    createOperationsGeogToVertWithIntermediateVert(
        const crs::CRSNNPtr &sourceCRS, const crs::CRSNNPtr &targetCRS,
        const crs::VerticalCRS *vertDst, Private::Context &context) {

    ENTER_FUNCTION();

    std::vector<CoordinateOperationNNPtr> res;

    struct AntiRecursionGuard {
        Context &context;

        explicit AntiRecursionGuard(Context &contextIn) : context(contextIn) {
            assert(!context.inCreateOperationsGeogToVertWithIntermediateVert);
            context.inCreateOperationsGeogToVertWithIntermediateVert = true;
        }

        ~AntiRecursionGuard() {
            context.inCreateOperationsGeogToVertWithIntermediateVert = false;
        }
    };
    AntiRecursionGuard guard(context);
    const auto &authFactory = context.context->getAuthorityFactory();
    const auto dbContext = authFactory->databaseContext().as_nullable();

    auto candidatesVert = findCandidateVertCRSForDatum(
        authFactory, vertDst->datumNonNull(dbContext).get());
    for (const auto &candidateVert : candidatesVert) {
        auto resTmp = createOperations(sourceCRS, candidateVert, context);
        if (!resTmp.empty()) {
            const auto opsSecond =
                createOperations(candidateVert, targetCRS, context);
            if (!opsSecond.empty()) {
                // The transformation from candidateVert to targetCRS should
                // be just a unit change typically, so take only the first one,
                // which is likely/hopefully the only one.
                for (const auto &opFirst : resTmp) {
                    if (hasIdentifiers(opFirst)) {
                        if (candidateVert->_isEquivalentTo(
                                targetCRS.get(),
                                util::IComparable::Criterion::EQUIVALENT)) {
                            res.emplace_back(opFirst);
                        } else {
                            res.emplace_back(
                                ConcatenatedOperation::createComputeMetadata(
                                    {opFirst, opsSecond.front()},
                                    disallowEmptyIntersection));
                        }
                    }
                }
                if (!res.empty())
                    break;
            }
        }
    }

    return res;
}

// ---------------------------------------------------------------------------

std::vector<CoordinateOperationNNPtr> CoordinateOperationFactory::Private::
    createOperationsGeogToVertWithAlternativeGeog(
        const crs::CRSNNPtr & /*sourceCRS*/, // geographic CRS
        const crs::CRSNNPtr &targetCRS,      // vertical CRS
        Private::Context &context) {

    ENTER_FUNCTION();

    std::vector<CoordinateOperationNNPtr> res;

    struct AntiRecursionGuard {
        Context &context;

        explicit AntiRecursionGuard(Context &contextIn) : context(contextIn) {
            assert(!context.inCreateOperationsGeogToVertWithAlternativeGeog);
            context.inCreateOperationsGeogToVertWithAlternativeGeog = true;
        }

        ~AntiRecursionGuard() {
            context.inCreateOperationsGeogToVertWithAlternativeGeog = false;
        }
    };
    AntiRecursionGuard guard(context);

    // Generally EPSG has operations from GeogCrs to VertCRS
    auto ops = findOpsInRegistryDirectTo(targetCRS, context);

    for (const auto &op : ops) {
        const auto tmpCRS = op->sourceCRS();
        if (tmpCRS && dynamic_cast<const crs::GeographicCRS *>(tmpCRS.get())) {
            res.emplace_back(op);
        }
    }

    return res;
}

// ---------------------------------------------------------------------------

void CoordinateOperationFactory::Private::
    createOperationsFromDatabaseWithVertCRS(
        const crs::CRSNNPtr &sourceCRS, const crs::CRSNNPtr &targetCRS,
        Private::Context &context, const crs::GeographicCRS *geogSrc,
        const crs::GeographicCRS *geogDst, const crs::VerticalCRS *vertSrc,
        const crs::VerticalCRS *vertDst,
        std::vector<CoordinateOperationNNPtr> &res) {

    // Typically to transform from "NAVD88 height (ftUS)" to a geog CRS
    // by using transformations of "NAVD88 height" (metre) to that geog CRS
    if (res.empty() &&
        !context.inCreateOperationsGeogToVertWithIntermediateVert && geogSrc &&
        vertDst) {
        res = createOperationsGeogToVertWithIntermediateVert(
            sourceCRS, targetCRS, vertDst, context);
    } else if (res.empty() &&
               !context.inCreateOperationsGeogToVertWithIntermediateVert &&
               geogDst && vertSrc) {
        res = applyInverse(createOperationsGeogToVertWithIntermediateVert(
            targetCRS, sourceCRS, vertSrc, context));
    }

    // NAD83 only exists in 2D version in EPSG, so if it has been
    // promoted to 3D, when researching a vertical to geog
    // transformation, try to down cast to 2D.
    const auto geog3DToVertTryThroughGeog2D = [&res, &context](
        const crs::GeographicCRS *geogSrcIn, const crs::VerticalCRS *vertDstIn,
        const crs::CRSNNPtr &targetCRSIn) {
        if (res.empty() && geogSrcIn && vertDstIn &&
            geogSrcIn->coordinateSystem()->axisList().size() == 3) {
            const auto &authFactory = context.context->getAuthorityFactory();
            const auto dbContext =
                authFactory ? authFactory->databaseContext().as_nullable()
                            : nullptr;
            const auto candidatesSrcGeod(findCandidateGeodCRSForDatum(
                authFactory, geogSrcIn,
                geogSrcIn->datumNonNull(dbContext).get()));
            for (const auto &candidate : candidatesSrcGeod) {
                auto geogCandidate =
                    util::nn_dynamic_pointer_cast<crs::GeographicCRS>(
                        candidate);
                if (geogCandidate &&
                    geogCandidate->coordinateSystem()->axisList().size() == 2) {
                    bool ignored;
                    res =
                        findOpsInRegistryDirect(NN_NO_CHECK(geogCandidate),
                                                targetCRSIn, context, ignored);
                    break;
                }
            }
            return true;
        }
        return false;
    };

    if (geog3DToVertTryThroughGeog2D(geogSrc, vertDst, targetCRS)) {
        // do nothing
    } else if (geog3DToVertTryThroughGeog2D(geogDst, vertSrc, sourceCRS)) {
        res = applyInverse(res);
    }

    // There's no direct transformation from NAVD88 height to WGS84,
    // so try to research all transformations from NAVD88 to another
    // intermediate GeographicCRS.
    if (res.empty() &&
        !context.inCreateOperationsGeogToVertWithAlternativeGeog && geogSrc &&
        vertDst) {
        res = createOperationsGeogToVertWithAlternativeGeog(sourceCRS,
                                                            targetCRS, context);
    } else if (res.empty() &&
               !context.inCreateOperationsGeogToVertWithAlternativeGeog &&
               geogDst && vertSrc) {
        res = applyInverse(createOperationsGeogToVertWithAlternativeGeog(
            targetCRS, sourceCRS, context));
    }
}

// ---------------------------------------------------------------------------

void CoordinateOperationFactory::Private::createOperationsGeodToGeod(
    const crs::CRSNNPtr &sourceCRS, const crs::CRSNNPtr &targetCRS,
    Private::Context &context, const crs::GeodeticCRS *geodSrc,
    const crs::GeodeticCRS *geodDst,
    std::vector<CoordinateOperationNNPtr> &res) {

    ENTER_FUNCTION();

    if (geodSrc->ellipsoid()->celestialBody() !=
        geodDst->ellipsoid()->celestialBody()) {
        throw util::UnsupportedOperationException(
            "Source and target ellipsoid do not belong to the same "
            "celestial body");
    }

    auto geogSrc = dynamic_cast<const crs::GeographicCRS *>(geodSrc);
    auto geogDst = dynamic_cast<const crs::GeographicCRS *>(geodDst);

    if (geogSrc && geogDst) {
        createOperationsGeogToGeog(res, sourceCRS, targetCRS, context, geogSrc,
                                   geogDst);
        return;
    }

    const bool isSrcGeocentric = geodSrc->isGeocentric();
    const bool isSrcGeographic = geogSrc != nullptr;
    const bool isTargetGeocentric = geodDst->isGeocentric();
    const bool isTargetGeographic = geogDst != nullptr;

    const auto IsSameDatum = [&context, &geodSrc, &geodDst]() {
        const auto &authFactory = context.context->getAuthorityFactory();
        const auto dbContext =
            authFactory ? authFactory->databaseContext().as_nullable()
                        : nullptr;

        return geodSrc->datumNonNull(dbContext)->_isEquivalentTo(
            geodDst->datumNonNull(dbContext).get(),
            util::IComparable::Criterion::EQUIVALENT);
    };

    if (((isSrcGeocentric && isTargetGeographic) ||
         (isSrcGeographic && isTargetGeocentric))) {

        // Same datum ?
        if (IsSameDatum()) {
            res.emplace_back(
                Conversion::createGeographicGeocentric(sourceCRS, targetCRS));
        } else if (isSrcGeocentric && geogDst) {
            std::string interm_crs_name(geogDst->nameStr());
            interm_crs_name += " (geocentric)";
            auto interm_crs =
                util::nn_static_pointer_cast<crs::CRS>(crs::GeodeticCRS::create(
                    addDomains(util::PropertyMap().set(
                                   common::IdentifiedObject::NAME_KEY,
                                   interm_crs_name),
                               geogDst),
                    geogDst->datum(), geogDst->datumEnsemble(),
                    NN_CHECK_ASSERT(
                        util::nn_dynamic_pointer_cast<cs::CartesianCS>(
                            geodSrc->coordinateSystem()))));
            auto opFirst =
                createBallparkGeocentricTranslation(sourceCRS, interm_crs);
            auto opSecond =
                Conversion::createGeographicGeocentric(interm_crs, targetCRS);
            res.emplace_back(ConcatenatedOperation::createComputeMetadata(
                {opFirst, opSecond}, disallowEmptyIntersection));
        } else {
            // Apply previous case in reverse way
            std::vector<CoordinateOperationNNPtr> resTmp;
            createOperationsGeodToGeod(targetCRS, sourceCRS, context, geodDst,
                                       geodSrc, resTmp);
            assert(resTmp.size() == 1);
            res.emplace_back(resTmp.front()->inverse());
        }

        return;
    }

    if (isSrcGeocentric && isTargetGeocentric) {
        if (sourceCRS->_isEquivalentTo(
                targetCRS.get(), util::IComparable::Criterion::EQUIVALENT) ||
            IsSameDatum()) {
            std::string name(NULL_GEOCENTRIC_TRANSLATION);
            name += " from ";
            name += sourceCRS->nameStr();
            name += " to ";
            name += targetCRS->nameStr();
            res.emplace_back(Transformation::createGeocentricTranslations(
                util::PropertyMap()
                    .set(common::IdentifiedObject::NAME_KEY, name)
                    .set(common::ObjectUsage::DOMAIN_OF_VALIDITY_KEY,
                         metadata::Extent::WORLD),
                sourceCRS, targetCRS, 0.0, 0.0, 0.0,
                {metadata::PositionalAccuracy::create("0")}));
        } else {
            res.emplace_back(
                createBallparkGeocentricTranslation(sourceCRS, targetCRS));
        }
        return;
    }

    // Transformation between two geodetic systems of unknown type
    // This should normally not be triggered with "standard" CRS
    res.emplace_back(createGeodToGeodPROJBased(sourceCRS, targetCRS));
}

// ---------------------------------------------------------------------------

void CoordinateOperationFactory::Private::createOperationsDerivedTo(
    const crs::CRSNNPtr & /*sourceCRS*/, const crs::CRSNNPtr &targetCRS,
    Private::Context &context, const crs::DerivedCRS *derivedSrc,
    std::vector<CoordinateOperationNNPtr> &res) {

    ENTER_FUNCTION();

    auto opFirst = derivedSrc->derivingConversion()->inverse();
    // Small optimization if the targetCRS is the baseCRS of the source
    // derivedCRS.
    if (derivedSrc->baseCRS()->_isEquivalentTo(
            targetCRS.get(), util::IComparable::Criterion::EQUIVALENT)) {
        res.emplace_back(opFirst);
        return;
    }
    auto opsSecond =
        createOperations(derivedSrc->baseCRS(), targetCRS, context);
    for (const auto &opSecond : opsSecond) {
        try {
            res.emplace_back(ConcatenatedOperation::createComputeMetadata(
                {opFirst, opSecond}, disallowEmptyIntersection));
        } catch (const InvalidOperationEmptyIntersection &) {
        }
    }
}

// ---------------------------------------------------------------------------

void CoordinateOperationFactory::Private::createOperationsBoundToGeog(
    const crs::CRSNNPtr &sourceCRS, const crs::CRSNNPtr &targetCRS,
    Private::Context &context, const crs::BoundCRS *boundSrc,
    const crs::GeographicCRS *geogDst,
    std::vector<CoordinateOperationNNPtr> &res) {

    ENTER_FUNCTION();

    const auto &hubSrc = boundSrc->hubCRS();
    auto hubSrcGeog = dynamic_cast<const crs::GeographicCRS *>(hubSrc.get());
    auto geogCRSOfBaseOfBoundSrc = boundSrc->baseCRS()->extractGeographicCRS();
    {
        // If geogCRSOfBaseOfBoundSrc is a DerivedGeographicCRS, use its base
        // instead (if it is a GeographicCRS)
        auto derivedGeogCRS =
            std::dynamic_pointer_cast<crs::DerivedGeographicCRS>(
                geogCRSOfBaseOfBoundSrc);
        if (derivedGeogCRS) {
            auto baseCRS = std::dynamic_pointer_cast<crs::GeographicCRS>(
                derivedGeogCRS->baseCRS().as_nullable());
            if (baseCRS) {
                geogCRSOfBaseOfBoundSrc = baseCRS;
            }
        }
    }

    const auto &authFactory = context.context->getAuthorityFactory();
    const auto dbContext =
        authFactory ? authFactory->databaseContext().as_nullable() : nullptr;

    const auto geogDstDatum = geogDst->datumNonNull(dbContext);

    // If the underlying datum of the source is the same as the target, do
    // not consider the boundCRS at all, but just its base
    if (geogCRSOfBaseOfBoundSrc) {
        auto geogCRSOfBaseOfBoundSrcDatum =
            geogCRSOfBaseOfBoundSrc->datumNonNull(dbContext);
        if (geogCRSOfBaseOfBoundSrcDatum->_isEquivalentTo(
                geogDstDatum.get(), util::IComparable::Criterion::EQUIVALENT)) {
            res = createOperations(boundSrc->baseCRS(), targetCRS, context);
            return;
        }
    }

    bool triedBoundCrsToGeogCRSSameAsHubCRS = false;
    // Is it: boundCRS to a geogCRS that is the same as the hubCRS ?
    if (hubSrcGeog && geogCRSOfBaseOfBoundSrc &&
        (hubSrcGeog->_isEquivalentTo(
             geogDst, util::IComparable::Criterion::EQUIVALENT) ||
         hubSrcGeog->is2DPartOf3D(NN_NO_CHECK(geogDst), dbContext))) {
        triedBoundCrsToGeogCRSSameAsHubCRS = true;

        CoordinateOperationPtr opIntermediate;
        if (!geogCRSOfBaseOfBoundSrc->_isEquivalentTo(
                boundSrc->transformation()->sourceCRS().get(),
                util::IComparable::Criterion::EQUIVALENT)) {
            auto opsIntermediate = createOperations(
                NN_NO_CHECK(geogCRSOfBaseOfBoundSrc),
                boundSrc->transformation()->sourceCRS(), context);
            assert(!opsIntermediate.empty());
            opIntermediate = opsIntermediate.front();
        }

        if (boundSrc->baseCRS() == geogCRSOfBaseOfBoundSrc) {
            if (opIntermediate) {
                try {
                    res.emplace_back(
                        ConcatenatedOperation::createComputeMetadata(
                            {NN_NO_CHECK(opIntermediate),
                             boundSrc->transformation()},
                            disallowEmptyIntersection));
                } catch (const InvalidOperationEmptyIntersection &) {
                }
            } else {
                // Optimization to avoid creating a useless concatenated
                // operation
                res.emplace_back(boundSrc->transformation());
            }
            return;
        }
        auto opsFirst = createOperations(
            boundSrc->baseCRS(), NN_NO_CHECK(geogCRSOfBaseOfBoundSrc), context);
        if (!opsFirst.empty()) {
            for (const auto &opFirst : opsFirst) {
                try {
                    std::vector<CoordinateOperationNNPtr> subops;
                    subops.emplace_back(opFirst);
                    if (opIntermediate) {
                        subops.emplace_back(NN_NO_CHECK(opIntermediate));
                    }
                    subops.emplace_back(boundSrc->transformation());
                    res.emplace_back(
                        ConcatenatedOperation::createComputeMetadata(
                            subops, disallowEmptyIntersection));
                } catch (const InvalidOperationEmptyIntersection &) {
                }
            }
            if (!res.empty()) {
                return;
            }
        }
        // If the datum are equivalent, this is also fine
    } else if (geogCRSOfBaseOfBoundSrc && hubSrcGeog &&
               hubSrcGeog->datumNonNull(dbContext)->_isEquivalentTo(
                   geogDstDatum.get(),
                   util::IComparable::Criterion::EQUIVALENT)) {
        auto opsFirst = createOperations(
            boundSrc->baseCRS(), NN_NO_CHECK(geogCRSOfBaseOfBoundSrc), context);
        auto opsLast = createOperations(hubSrc, targetCRS, context);
        if (!opsFirst.empty() && !opsLast.empty()) {
            CoordinateOperationPtr opIntermediate;
            if (!geogCRSOfBaseOfBoundSrc->_isEquivalentTo(
                    boundSrc->transformation()->sourceCRS().get(),
                    util::IComparable::Criterion::EQUIVALENT)) {
                auto opsIntermediate = createOperations(
                    NN_NO_CHECK(geogCRSOfBaseOfBoundSrc),
                    boundSrc->transformation()->sourceCRS(), context);
                assert(!opsIntermediate.empty());
                opIntermediate = opsIntermediate.front();
            }
            for (const auto &opFirst : opsFirst) {
                for (const auto &opLast : opsLast) {
                    try {
                        std::vector<CoordinateOperationNNPtr> subops;
                        subops.emplace_back(opFirst);
                        if (opIntermediate) {
                            subops.emplace_back(NN_NO_CHECK(opIntermediate));
                        }
                        subops.emplace_back(boundSrc->transformation());
                        subops.emplace_back(opLast);
                        res.emplace_back(
                            ConcatenatedOperation::createComputeMetadata(
                                subops, disallowEmptyIntersection));
                    } catch (const InvalidOperationEmptyIntersection &) {
                    }
                }
            }
            if (!res.empty()) {
                return;
            }
        }
        // Consider WGS 84 and NAD83 as equivalent in that context if the
        // geogCRSOfBaseOfBoundSrc ellipsoid is Clarke66 (for NAD27)
        // Case of "+proj=latlong +ellps=clrk66
        // +nadgrids=ntv1_can.dat,conus"
        // to "+proj=latlong +datum=NAD83"
    } else if (geogCRSOfBaseOfBoundSrc && hubSrcGeog &&
               geogCRSOfBaseOfBoundSrc->ellipsoid()->_isEquivalentTo(
                   datum::Ellipsoid::CLARKE_1866.get(),
                   util::IComparable::Criterion::EQUIVALENT) &&
               hubSrcGeog->datumNonNull(dbContext)->_isEquivalentTo(
                   datum::GeodeticReferenceFrame::EPSG_6326.get(),
                   util::IComparable::Criterion::EQUIVALENT) &&
               geogDstDatum->_isEquivalentTo(
                   datum::GeodeticReferenceFrame::EPSG_6269.get(),
                   util::IComparable::Criterion::EQUIVALENT)) {
        auto nnGeogCRSOfBaseOfBoundSrc = NN_NO_CHECK(geogCRSOfBaseOfBoundSrc);
        if (boundSrc->baseCRS()->_isEquivalentTo(
                nnGeogCRSOfBaseOfBoundSrc.get(),
                util::IComparable::Criterion::EQUIVALENT)) {
            auto transf = boundSrc->transformation()->shallowClone();
            transf->setProperties(util::PropertyMap().set(
                common::IdentifiedObject::NAME_KEY,
                buildTransfName(boundSrc->baseCRS()->nameStr(),
                                targetCRS->nameStr())));
            transf->setCRSs(boundSrc->baseCRS(), targetCRS, nullptr);
            res.emplace_back(transf);
            return;
        } else {
            auto opsFirst = createOperations(
                boundSrc->baseCRS(), nnGeogCRSOfBaseOfBoundSrc, context);
            auto transf = boundSrc->transformation()->shallowClone();
            transf->setProperties(util::PropertyMap().set(
                common::IdentifiedObject::NAME_KEY,
                buildTransfName(nnGeogCRSOfBaseOfBoundSrc->nameStr(),
                                targetCRS->nameStr())));
            transf->setCRSs(nnGeogCRSOfBaseOfBoundSrc, targetCRS, nullptr);
            if (!opsFirst.empty()) {
                for (const auto &opFirst : opsFirst) {
                    try {
                        res.emplace_back(
                            ConcatenatedOperation::createComputeMetadata(
                                {opFirst, transf}, disallowEmptyIntersection));
                    } catch (const InvalidOperationEmptyIntersection &) {
                    }
                }
                if (!res.empty()) {
                    return;
                }
            }
        }
    }

    if (hubSrcGeog &&
        hubSrcGeog->_isEquivalentTo(geogDst,
                                    util::IComparable::Criterion::EQUIVALENT) &&
        dynamic_cast<const crs::VerticalCRS *>(boundSrc->baseCRS().get())) {
        auto transfSrc = boundSrc->transformation()->sourceCRS();
        if (dynamic_cast<const crs::VerticalCRS *>(transfSrc.get()) &&
            !boundSrc->baseCRS()->_isEquivalentTo(
                transfSrc.get(), util::IComparable::Criterion::EQUIVALENT)) {
            auto opsFirst =
                createOperations(boundSrc->baseCRS(), transfSrc, context);
            for (const auto &opFirst : opsFirst) {
                try {
                    res.emplace_back(
                        ConcatenatedOperation::createComputeMetadata(
                            {opFirst, boundSrc->transformation()},
                            disallowEmptyIntersection));
                } catch (const InvalidOperationEmptyIntersection &) {
                }
            }
            return;
        }

        res.emplace_back(boundSrc->transformation());
        return;
    }

    if (!triedBoundCrsToGeogCRSSameAsHubCRS && hubSrcGeog &&
        geogCRSOfBaseOfBoundSrc) {
        // This one should go to the above 'Is it: boundCRS to a geogCRS
        // that is the same as the hubCRS ?' case
        auto opsFirst = createOperations(sourceCRS, hubSrc, context);
        auto opsLast = createOperations(hubSrc, targetCRS, context);
        if (!opsFirst.empty() && !opsLast.empty()) {
            for (const auto &opFirst : opsFirst) {
                for (const auto &opLast : opsLast) {
                    // Exclude artificial transformations from the hub
                    // to the target CRS, if it is the only one.
                    if (opsLast.size() > 1 ||
                        !opLast->hasBallparkTransformation()) {
                        try {
                            res.emplace_back(
                                ConcatenatedOperation::createComputeMetadata(
                                    {opFirst, opLast},
                                    disallowEmptyIntersection));
                        } catch (const InvalidOperationEmptyIntersection &) {
                        }
                    } else {
                        // std::cerr << "excluded " << opLast->nameStr() <<
                        // std::endl;
                    }
                }
            }
            if (!res.empty()) {
                return;
            }
        }
    }

    auto vertCRSOfBaseOfBoundSrc =
        dynamic_cast<const crs::VerticalCRS *>(boundSrc->baseCRS().get());
    if (vertCRSOfBaseOfBoundSrc && hubSrcGeog) {
        auto opsFirst = createOperations(sourceCRS, hubSrc, context);
        if (context.skipHorizontalTransformation) {
            if (!opsFirst.empty()) {
                const auto &hubAxisList =
                    hubSrcGeog->coordinateSystem()->axisList();
                const auto &targetAxisList =
                    geogDst->coordinateSystem()->axisList();
                if (hubAxisList.size() == 3 && targetAxisList.size() == 3 &&
                    !hubAxisList[2]->_isEquivalentTo(
                        targetAxisList[2].get(),
                        util::IComparable::Criterion::EQUIVALENT)) {

                    const auto &srcAxis = hubAxisList[2];
                    const double convSrc = srcAxis->unit().conversionToSI();
                    const auto &dstAxis = targetAxisList[2];
                    const double convDst = dstAxis->unit().conversionToSI();
                    const bool srcIsUp =
                        srcAxis->direction() == cs::AxisDirection::UP;
                    const bool srcIsDown =
                        srcAxis->direction() == cs::AxisDirection::DOWN;
                    const bool dstIsUp =
                        dstAxis->direction() == cs::AxisDirection::UP;
                    const bool dstIsDown =
                        dstAxis->direction() == cs::AxisDirection::DOWN;
                    const bool heightDepthReversal =
                        ((srcIsUp && dstIsDown) || (srcIsDown && dstIsUp));

                    const double factor = convSrc / convDst;
                    auto conv = Conversion::createChangeVerticalUnit(
                        util::PropertyMap().set(
                            common::IdentifiedObject::NAME_KEY,
                            "Change of vertical unit"),
                        common::Scale(heightDepthReversal ? -factor : factor));

                    conv->setCRSs(
                        hubSrc,
                        hubSrc->demoteTo2D(std::string(), dbContext)
                            ->promoteTo3D(std::string(), dbContext, dstAxis),
                        nullptr);

                    for (const auto &op : opsFirst) {
                        try {
                            res.emplace_back(
                                ConcatenatedOperation::createComputeMetadata(
                                    {op, conv}, disallowEmptyIntersection));
                        } catch (const InvalidOperationEmptyIntersection &) {
                        }
                    }
                } else {
                    res = opsFirst;
                }
            }
            return;
        } else {
            auto opsSecond = createOperations(hubSrc, targetCRS, context);
            if (!opsFirst.empty() && !opsSecond.empty()) {
                for (const auto &opFirst : opsFirst) {
                    for (const auto &opLast : opsSecond) {
                        // Exclude artificial transformations from the hub
                        // to the target CRS
                        if (!opLast->hasBallparkTransformation()) {
                            try {
                                res.emplace_back(
                                    ConcatenatedOperation::
                                        createComputeMetadata(
                                            {opFirst, opLast},
                                            disallowEmptyIntersection));
                            } catch (
                                const InvalidOperationEmptyIntersection &) {
                            }
                        } else {
                            // std::cerr << "excluded " << opLast->nameStr() <<
                            // std::endl;
                        }
                    }
                }
                if (!res.empty()) {
                    return;
                }
            }
        }
    }

    res = createOperations(boundSrc->baseCRS(), targetCRS, context);
}

// ---------------------------------------------------------------------------

void CoordinateOperationFactory::Private::createOperationsBoundToVert(
    const crs::CRSNNPtr & /*sourceCRS*/, const crs::CRSNNPtr &targetCRS,
    Private::Context &context, const crs::BoundCRS *boundSrc,
    const crs::VerticalCRS *vertDst,
    std::vector<CoordinateOperationNNPtr> &res) {

    ENTER_FUNCTION();

    auto baseSrcVert =
        dynamic_cast<const crs::VerticalCRS *>(boundSrc->baseCRS().get());
    const auto &hubSrc = boundSrc->hubCRS();
    auto hubSrcVert = dynamic_cast<const crs::VerticalCRS *>(hubSrc.get());
    if (baseSrcVert && hubSrcVert &&
        vertDst->_isEquivalentTo(hubSrcVert,
                                 util::IComparable::Criterion::EQUIVALENT)) {
        res.emplace_back(boundSrc->transformation());
        return;
    }

    res = createOperations(boundSrc->baseCRS(), targetCRS, context);
}

// ---------------------------------------------------------------------------

void CoordinateOperationFactory::Private::createOperationsVertToVert(
    const crs::CRSNNPtr &sourceCRS, const crs::CRSNNPtr &targetCRS,
    Private::Context &context, const crs::VerticalCRS *vertSrc,
    const crs::VerticalCRS *vertDst,
    std::vector<CoordinateOperationNNPtr> &res) {

    ENTER_FUNCTION();

    const auto &authFactory = context.context->getAuthorityFactory();
    const auto dbContext =
        authFactory ? authFactory->databaseContext().as_nullable() : nullptr;

    const auto srcDatum = vertSrc->datumNonNull(dbContext);
    const auto dstDatum = vertDst->datumNonNull(dbContext);
    const bool equivalentVDatum = srcDatum->_isEquivalentTo(
        dstDatum.get(), util::IComparable::Criterion::EQUIVALENT);

    const auto &srcAxis = vertSrc->coordinateSystem()->axisList()[0];
    const double convSrc = srcAxis->unit().conversionToSI();
    const auto &dstAxis = vertDst->coordinateSystem()->axisList()[0];
    const double convDst = dstAxis->unit().conversionToSI();
    const bool srcIsUp = srcAxis->direction() == cs::AxisDirection::UP;
    const bool srcIsDown = srcAxis->direction() == cs::AxisDirection::DOWN;
    const bool dstIsUp = dstAxis->direction() == cs::AxisDirection::UP;
    const bool dstIsDown = dstAxis->direction() == cs::AxisDirection::DOWN;
    const bool heightDepthReversal =
        ((srcIsUp && dstIsDown) || (srcIsDown && dstIsUp));

    const double factor = convSrc / convDst;
    auto name = buildTransfName(sourceCRS->nameStr(), targetCRS->nameStr());
    if (!equivalentVDatum) {
        name += BALLPARK_VERTICAL_TRANSFORMATION;
        auto conv = Transformation::createChangeVerticalUnit(
            util::PropertyMap().set(common::IdentifiedObject::NAME_KEY, name),
            sourceCRS, targetCRS,
            // In case of a height depth reversal, we should probably have
            // 2 steps instead of putting a negative factor...
            common::Scale(heightDepthReversal ? -factor : factor), {});
        conv->setHasBallparkTransformation(true);
        res.push_back(conv);
    } else if (convSrc != convDst || !heightDepthReversal) {
        auto conv = Conversion::createChangeVerticalUnit(
            util::PropertyMap().set(common::IdentifiedObject::NAME_KEY, name),
            // In case of a height depth reversal, we should probably have
            // 2 steps instead of putting a negative factor...
            common::Scale(heightDepthReversal ? -factor : factor));
        conv->setCRSs(sourceCRS, targetCRS, nullptr);
        res.push_back(conv);
    } else {
        auto conv = Conversion::createHeightDepthReversal(
            util::PropertyMap().set(common::IdentifiedObject::NAME_KEY, name));
        conv->setCRSs(sourceCRS, targetCRS, nullptr);
        res.push_back(conv);
    }
}

// ---------------------------------------------------------------------------

void CoordinateOperationFactory::Private::createOperationsVertToGeog(
    const crs::CRSNNPtr &sourceCRS, const crs::CRSNNPtr &targetCRS,
    Private::Context &context, const crs::VerticalCRS *vertSrc,
    const crs::GeographicCRS *geogDst,
    std::vector<CoordinateOperationNNPtr> &res) {

    ENTER_FUNCTION();

    if (vertSrc->identifiers().empty()) {
        const auto &vertSrcName = vertSrc->nameStr();
        const auto &authFactory = context.context->getAuthorityFactory();
        if (authFactory != nullptr && vertSrcName != "unnamed" &&
            vertSrcName != "unknown") {
            auto matches = authFactory->createObjectsFromName(
                vertSrcName, {io::AuthorityFactory::ObjectType::VERTICAL_CRS},
                false, 2);
            if (matches.size() == 1) {
                const auto &match = matches.front();
                if (vertSrc->_isEquivalentTo(
                        match.get(),
                        util::IComparable::Criterion::EQUIVALENT) &&
                    !match->identifiers().empty()) {
                    auto resTmp = createOperations(
                        NN_NO_CHECK(
                            util::nn_dynamic_pointer_cast<crs::VerticalCRS>(
                                match)),
                        targetCRS, context);
                    res.insert(res.end(), resTmp.begin(), resTmp.end());
                    return;
                }
            }
        }
    }

    createOperationsVertToGeogBallpark(sourceCRS, targetCRS, context, vertSrc,
                                       geogDst, res);
}

// ---------------------------------------------------------------------------

void CoordinateOperationFactory::Private::createOperationsVertToGeogBallpark(
    const crs::CRSNNPtr &sourceCRS, const crs::CRSNNPtr &targetCRS,
    Private::Context &, const crs::VerticalCRS *vertSrc,
    const crs::GeographicCRS *geogDst,
    std::vector<CoordinateOperationNNPtr> &res) {

    ENTER_FUNCTION();

    const auto &srcAxis = vertSrc->coordinateSystem()->axisList()[0];
    const double convSrc = srcAxis->unit().conversionToSI();
    double convDst = 1.0;
    const auto &geogAxis = geogDst->coordinateSystem()->axisList();
    bool dstIsUp = true;
    bool dstIsDown = true;
    if (geogAxis.size() == 3) {
        const auto &dstAxis = geogAxis[2];
        convDst = dstAxis->unit().conversionToSI();
        dstIsUp = dstAxis->direction() == cs::AxisDirection::UP;
        dstIsDown = dstAxis->direction() == cs::AxisDirection::DOWN;
    }
    const bool srcIsUp = srcAxis->direction() == cs::AxisDirection::UP;
    const bool srcIsDown = srcAxis->direction() == cs::AxisDirection::DOWN;
    const bool heightDepthReversal =
        ((srcIsUp && dstIsDown) || (srcIsDown && dstIsUp));

    const double factor = convSrc / convDst;

    const auto &sourceCRSExtent = getExtent(sourceCRS);
    const auto &targetCRSExtent = getExtent(targetCRS);
    const bool sameExtent =
        sourceCRSExtent && targetCRSExtent &&
        sourceCRSExtent->_isEquivalentTo(
            targetCRSExtent.get(), util::IComparable::Criterion::EQUIVALENT);

    util::PropertyMap map;
    map.set(common::IdentifiedObject::NAME_KEY,
            buildTransfName(sourceCRS->nameStr(), targetCRS->nameStr()) +
                BALLPARK_VERTICAL_TRANSFORMATION_NO_ELLIPSOID_VERT_HEIGHT)
        .set(common::ObjectUsage::DOMAIN_OF_VALIDITY_KEY,
             sameExtent ? NN_NO_CHECK(sourceCRSExtent)
                        : metadata::Extent::WORLD);

    auto conv = Transformation::createChangeVerticalUnit(
        map, sourceCRS, targetCRS,
        common::Scale(heightDepthReversal ? -factor : factor), {});
    conv->setHasBallparkTransformation(true);
    res.push_back(conv);
}

// ---------------------------------------------------------------------------

void CoordinateOperationFactory::Private::createOperationsBoundToBound(
    const crs::CRSNNPtr &sourceCRS, const crs::CRSNNPtr &targetCRS,
    Private::Context &context, const crs::BoundCRS *boundSrc,
    const crs::BoundCRS *boundDst, std::vector<CoordinateOperationNNPtr> &res) {

    ENTER_FUNCTION();

    // BoundCRS to BoundCRS of horizontal CRS using the same (geographic) hub
    const auto &hubSrc = boundSrc->hubCRS();
    auto hubSrcGeog = dynamic_cast<const crs::GeographicCRS *>(hubSrc.get());
    const auto &hubDst = boundDst->hubCRS();
    auto hubDstGeog = dynamic_cast<const crs::GeographicCRS *>(hubDst.get());
    auto geogCRSOfBaseOfBoundSrc = boundSrc->baseCRS()->extractGeographicCRS();
    auto geogCRSOfBaseOfBoundDst = boundDst->baseCRS()->extractGeographicCRS();
    if (hubSrcGeog && hubDstGeog &&
        hubSrcGeog->_isEquivalentTo(hubDstGeog,
                                    util::IComparable::Criterion::EQUIVALENT) &&
        geogCRSOfBaseOfBoundSrc && geogCRSOfBaseOfBoundDst) {
        const bool firstIsNoOp = geogCRSOfBaseOfBoundSrc->_isEquivalentTo(
            boundSrc->baseCRS().get(),
            util::IComparable::Criterion::EQUIVALENT);
        const bool lastIsNoOp = geogCRSOfBaseOfBoundDst->_isEquivalentTo(
            boundDst->baseCRS().get(),
            util::IComparable::Criterion::EQUIVALENT);
        auto opsFirst = createOperations(
            boundSrc->baseCRS(), NN_NO_CHECK(geogCRSOfBaseOfBoundSrc), context);
        auto opsLast = createOperations(NN_NO_CHECK(geogCRSOfBaseOfBoundDst),
                                        boundDst->baseCRS(), context);
        if (!opsFirst.empty() && !opsLast.empty()) {
            const auto &opSecond = boundSrc->transformation();
            auto opThird = boundDst->transformation()->inverse();
            for (const auto &opFirst : opsFirst) {
                for (const auto &opLast : opsLast) {
                    try {
                        std::vector<CoordinateOperationNNPtr> ops;
                        if (!firstIsNoOp) {
                            ops.push_back(opFirst);
                        }
                        ops.push_back(opSecond);
                        ops.push_back(opThird);
                        if (!lastIsNoOp) {
                            ops.push_back(opLast);
                        }
                        res.emplace_back(
                            ConcatenatedOperation::createComputeMetadata(
                                ops, disallowEmptyIntersection));
                    } catch (const InvalidOperationEmptyIntersection &) {
                    }
                }
            }
            if (!res.empty()) {
                return;
            }
        }
    }

    // BoundCRS to BoundCRS of vertical CRS using the same vertical datum
    // ==> ignore the bound transformation
    auto baseOfBoundSrcAsVertCRS =
        dynamic_cast<crs::VerticalCRS *>(boundSrc->baseCRS().get());
    auto baseOfBoundDstAsVertCRS =
        dynamic_cast<crs::VerticalCRS *>(boundDst->baseCRS().get());
    if (baseOfBoundSrcAsVertCRS && baseOfBoundDstAsVertCRS) {

        const auto &authFactory = context.context->getAuthorityFactory();
        const auto dbContext =
            authFactory ? authFactory->databaseContext().as_nullable()
                        : nullptr;

        const auto datumSrc = baseOfBoundSrcAsVertCRS->datumNonNull(dbContext);
        const auto datumDst = baseOfBoundDstAsVertCRS->datumNonNull(dbContext);
        if (datumSrc->nameStr() == datumDst->nameStr() &&
            (datumSrc->nameStr() != "unknown" ||
             boundSrc->transformation()->_isEquivalentTo(
                 boundDst->transformation().get(),
                 util::IComparable::Criterion::EQUIVALENT))) {
            res = createOperations(boundSrc->baseCRS(), boundDst->baseCRS(),
                                   context);
            return;
        }
    }

    // BoundCRS to BoundCRS of vertical CRS
    auto vertCRSOfBaseOfBoundSrc = boundSrc->baseCRS()->extractVerticalCRS();
    auto vertCRSOfBaseOfBoundDst = boundDst->baseCRS()->extractVerticalCRS();
    if (hubSrcGeog && hubDstGeog &&
        hubSrcGeog->_isEquivalentTo(hubDstGeog,
                                    util::IComparable::Criterion::EQUIVALENT) &&
        vertCRSOfBaseOfBoundSrc && vertCRSOfBaseOfBoundDst) {
        auto opsFirst = createOperations(sourceCRS, hubSrc, context);
        auto opsLast = createOperations(hubSrc, targetCRS, context);
        if (!opsFirst.empty() && !opsLast.empty()) {
            for (const auto &opFirst : opsFirst) {
                for (const auto &opLast : opsLast) {
                    try {
                        res.emplace_back(
                            ConcatenatedOperation::createComputeMetadata(
                                {opFirst, opLast}, disallowEmptyIntersection));
                    } catch (const InvalidOperationEmptyIntersection &) {
                    }
                }
            }
            if (!res.empty()) {
                return;
            }
        }
    }

    res = createOperations(boundSrc->baseCRS(), boundDst->baseCRS(), context);
}

// ---------------------------------------------------------------------------

static std::vector<CoordinateOperationNNPtr>
getOps(const CoordinateOperationNNPtr &op) {
    auto concatenated = dynamic_cast<const ConcatenatedOperation *>(op.get());
    if (concatenated)
        return concatenated->operations();
    return {op};
}

// ---------------------------------------------------------------------------

static bool useDifferentTransformationsForSameSourceTarget(
    const CoordinateOperationNNPtr &opA, const CoordinateOperationNNPtr &opB) {
    auto subOpsA = getOps(opA);
    auto subOpsB = getOps(opB);
    for (const auto &subOpA : subOpsA) {
        if (!dynamic_cast<const Transformation *>(subOpA.get()))
            continue;
        if (subOpA->sourceCRS()->nameStr() == "unknown" ||
            subOpA->targetCRS()->nameStr() == "unknown")
            continue;
        for (const auto &subOpB : subOpsB) {
            if (!dynamic_cast<const Transformation *>(subOpB.get()))
                continue;
            if (subOpB->sourceCRS()->nameStr() == "unknown" ||
                subOpB->targetCRS()->nameStr() == "unknown")
                continue;

            if (subOpA->sourceCRS()->nameStr() ==
                    subOpB->sourceCRS()->nameStr() &&
                subOpA->targetCRS()->nameStr() ==
                    subOpB->targetCRS()->nameStr()) {
                if (starts_with(subOpA->nameStr(), NULL_GEOGRAPHIC_OFFSET) &&
                    starts_with(subOpB->nameStr(), NULL_GEOGRAPHIC_OFFSET)) {
                    continue;
                }

                if (!subOpA->isEquivalentTo(subOpB.get())) {
                    return true;
                }
            } else if (subOpA->sourceCRS()->nameStr() ==
                           subOpB->targetCRS()->nameStr() &&
                       subOpA->targetCRS()->nameStr() ==
                           subOpB->sourceCRS()->nameStr()) {
                if (starts_with(subOpA->nameStr(), NULL_GEOGRAPHIC_OFFSET) &&
                    starts_with(subOpB->nameStr(), NULL_GEOGRAPHIC_OFFSET)) {
                    continue;
                }

                if (!subOpA->isEquivalentTo(subOpB->inverse().get())) {
                    return true;
                }
            }
        }
    }
    return false;
}

// ---------------------------------------------------------------------------

static crs::GeographicCRSPtr
getInterpolationGeogCRS(const CoordinateOperationNNPtr &verticalTransform,
                        const io::DatabaseContextPtr &dbContext) {
    crs::GeographicCRSPtr interpolationGeogCRS;
    auto transformationVerticalTransform =
        dynamic_cast<const Transformation *>(verticalTransform.get());
    if (transformationVerticalTransform == nullptr) {
        const auto concat = dynamic_cast<const ConcatenatedOperation *>(
            verticalTransform.get());
        if (concat) {
            const auto &steps = concat->operations();
            // Is this change of unit and/or height depth reversal +
            // transformation ?
            for (const auto &step : steps) {
                const auto transf =
                    dynamic_cast<const Transformation *>(step.get());
                if (transf) {
                    // Only support a single Transformation in the steps
                    if (transformationVerticalTransform != nullptr) {
                        transformationVerticalTransform = nullptr;
                        break;
                    }
                    transformationVerticalTransform = transf;
                }
            }
        }
    }
    if (transformationVerticalTransform &&
        !transformationVerticalTransform->hasBallparkTransformation()) {
        auto interpTransformCRS =
            transformationVerticalTransform->interpolationCRS();
        if (interpTransformCRS) {
            interpolationGeogCRS =
                std::dynamic_pointer_cast<crs::GeographicCRS>(
                    interpTransformCRS);
        } else {
            // If no explicit interpolation CRS, then
            // this will be the geographic CRS of the
            // vertical to geog transformation
            interpolationGeogCRS =
                std::dynamic_pointer_cast<crs::GeographicCRS>(
                    transformationVerticalTransform->targetCRS().as_nullable());
        }
    }

    if (interpolationGeogCRS) {
        if (interpolationGeogCRS->coordinateSystem()->axisList().size() == 3) {
            // We need to force the interpolation CRS, which
            // will
            // frequently be 3D, to 2D to avoid transformations
            // between source CRS and interpolation CRS to have
            // 3D terms.
            interpolationGeogCRS =
                interpolationGeogCRS->demoteTo2D(std::string(), dbContext)
                    .as_nullable();
        }
    }

    return interpolationGeogCRS;
}

// ---------------------------------------------------------------------------

void CoordinateOperationFactory::Private::createOperationsCompoundToGeog(
    const crs::CRSNNPtr &sourceCRS, const crs::CRSNNPtr &targetCRS,
    Private::Context &context, const crs::CompoundCRS *compoundSrc,
    const crs::GeographicCRS *geogDst,
    std::vector<CoordinateOperationNNPtr> &res) {

    ENTER_FUNCTION();

    const auto &authFactory = context.context->getAuthorityFactory();
    const auto &componentsSrc = compoundSrc->componentReferenceSystems();
    if (!componentsSrc.empty()) {

        if (componentsSrc.size() == 2) {
            auto derivedHSrc =
                dynamic_cast<const crs::DerivedCRS *>(componentsSrc[0].get());
            if (derivedHSrc) {
                std::vector<crs::CRSNNPtr> intermComponents{
                    derivedHSrc->baseCRS(), componentsSrc[1]};
                auto properties = util::PropertyMap().set(
                    common::IdentifiedObject::NAME_KEY,
                    intermComponents[0]->nameStr() + " + " +
                        intermComponents[1]->nameStr());
                auto intermCompound =
                    crs::CompoundCRS::create(properties, intermComponents);
                auto opsFirst =
                    createOperations(sourceCRS, intermCompound, context);
                assert(!opsFirst.empty());
                auto opsLast =
                    createOperations(intermCompound, targetCRS, context);
                for (const auto &opLast : opsLast) {
                    try {
                        res.emplace_back(
                            ConcatenatedOperation::createComputeMetadata(
                                {opsFirst.front(), opLast},
                                disallowEmptyIntersection));
                    } catch (const std::exception &) {
                    }
                }
                return;
            }
        }

        std::vector<CoordinateOperationNNPtr> horizTransforms;
        auto srcGeogCRS = componentsSrc[0]->extractGeographicCRS();
        if (srcGeogCRS) {
            horizTransforms =
                createOperations(componentsSrc[0], targetCRS, context);
        }
        std::vector<CoordinateOperationNNPtr> verticalTransforms;

        const auto dbContext =
            authFactory ? authFactory->databaseContext().as_nullable()
                        : nullptr;
        if (componentsSrc.size() >= 2 &&
            componentsSrc[1]->extractVerticalCRS()) {

            struct SetSkipHorizontalTransform {
                Context &context;

                explicit SetSkipHorizontalTransform(Context &contextIn)
                    : context(contextIn) {
                    assert(!context.skipHorizontalTransformation);
                    context.skipHorizontalTransformation = true;
                }

                ~SetSkipHorizontalTransform() {
                    context.skipHorizontalTransformation = false;
                }
            };
            SetSkipHorizontalTransform setSkipHorizontalTransform(context);

            verticalTransforms = createOperations(
                componentsSrc[1],
                targetCRS->promoteTo3D(std::string(), dbContext), context);
            bool foundRegisteredTransformWithAllGridsAvailable = false;
            const auto gridAvailabilityUse =
                context.context->getGridAvailabilityUse();
            const bool ignoreMissingGrids =
                gridAvailabilityUse ==
                CoordinateOperationContext::GridAvailabilityUse::
                    IGNORE_GRID_AVAILABILITY;
            for (const auto &op : verticalTransforms) {
                if (hasIdentifiers(op) && dbContext) {
                    bool missingGrid = false;
                    if (!ignoreMissingGrids) {
                        const auto gridsNeeded = op->gridsNeeded(
                            dbContext,
                            gridAvailabilityUse ==
                                CoordinateOperationContext::
                                    GridAvailabilityUse::KNOWN_AVAILABLE);
                        for (const auto &gridDesc : gridsNeeded) {
                            if (!gridDesc.available) {
                                missingGrid = true;
                                break;
                            }
                        }
                    }
                    if (!missingGrid) {
                        foundRegisteredTransformWithAllGridsAvailable = true;
                        break;
                    }
                }
            }
            if (!foundRegisteredTransformWithAllGridsAvailable && srcGeogCRS &&
                !srcGeogCRS->_isEquivalentTo(
                    geogDst, util::IComparable::Criterion::EQUIVALENT) &&
                !srcGeogCRS->is2DPartOf3D(NN_NO_CHECK(geogDst), dbContext)) {
                auto verticalTransformsTmp = createOperations(
                    componentsSrc[1],
                    NN_NO_CHECK(srcGeogCRS)
                        ->promoteTo3D(std::string(), dbContext),
                    context);
                bool foundRegisteredTransform = false;
                foundRegisteredTransformWithAllGridsAvailable = false;
                for (const auto &op : verticalTransformsTmp) {
                    if (hasIdentifiers(op) && dbContext) {
                        bool missingGrid = false;
                        if (!ignoreMissingGrids) {
                            const auto gridsNeeded = op->gridsNeeded(
                                dbContext,
                                gridAvailabilityUse ==
                                    CoordinateOperationContext::
                                        GridAvailabilityUse::KNOWN_AVAILABLE);
                            for (const auto &gridDesc : gridsNeeded) {
                                if (!gridDesc.available) {
                                    missingGrid = true;
                                    break;
                                }
                            }
                        }
                        foundRegisteredTransform = true;
                        if (!missingGrid) {
                            foundRegisteredTransformWithAllGridsAvailable =
                                true;
                            break;
                        }
                    }
                }
                if (foundRegisteredTransformWithAllGridsAvailable) {
                    verticalTransforms = verticalTransformsTmp;
                } else if (foundRegisteredTransform) {
                    verticalTransforms.insert(verticalTransforms.end(),
                                              verticalTransformsTmp.begin(),
                                              verticalTransformsTmp.end());
                }
            }
        }

        if (horizTransforms.empty() || verticalTransforms.empty()) {
            res = horizTransforms;
            return;
        }

        typedef std::pair<std::vector<CoordinateOperationNNPtr>,
                          std::vector<CoordinateOperationNNPtr>>
            PairOfTransforms;
        std::map<std::string, PairOfTransforms>
            cacheHorizToInterpAndInterpToTarget;

        for (const auto &verticalTransform : verticalTransforms) {
#ifdef TRACE_CREATE_OPERATIONS
            ENTER_BLOCK("Considering vertical transform " +
                        objectAsStr(verticalTransform.get()));
#endif
            crs::GeographicCRSPtr interpolationGeogCRS =
                getInterpolationGeogCRS(verticalTransform, dbContext);
            if (interpolationGeogCRS) {
#ifdef TRACE_CREATE_OPERATIONS
                logTrace("Using " + objectAsStr(interpolationGeogCRS.get()) +
                         " as interpolation CRS");
#endif
                std::vector<CoordinateOperationNNPtr> srcToInterpOps;
                std::vector<CoordinateOperationNNPtr> interpToTargetOps;

                std::string key;
                const auto &ids = interpolationGeogCRS->identifiers();
                if (!ids.empty()) {
                    key =
                        (*ids.front()->codeSpace()) + ':' + ids.front()->code();
                }

                const auto computeOpsToInterp =
                    [&srcToInterpOps, &interpToTargetOps, &componentsSrc,
                     &interpolationGeogCRS, &targetCRS, &dbContext,
                     &context]() {
                        srcToInterpOps = createOperations(
                            componentsSrc[0], NN_NO_CHECK(interpolationGeogCRS),
                            context);
                        auto target2D =
                            targetCRS->demoteTo2D(std::string(), dbContext);
                        if (!componentsSrc[0]->isEquivalentTo(
                                target2D.get(),
                                util::IComparable::Criterion::EQUIVALENT)) {
                            // We do the transformation from the
                            // interpolationCRS
                            // to the target one in 3D (see #2225)
                            // But we don't do that between sourceCRS and
                            // interpolationCRS, as this would mess with an
                            // orthometric elevation.
                            auto interp3D = interpolationGeogCRS->promoteTo3D(
                                std::string(), dbContext);
                            interpToTargetOps =
                                createOperations(interp3D, targetCRS, context);
                        }
                    };

                if (!key.empty()) {
                    auto iter = cacheHorizToInterpAndInterpToTarget.find(key);
                    if (iter == cacheHorizToInterpAndInterpToTarget.end()) {
#ifdef TRACE_CREATE_OPERATIONS
                        ENTER_BLOCK("looking for horizontal transformation "
                                    "from source to interpCRS and interpCRS to "
                                    "target");
#endif
                        computeOpsToInterp();
                        cacheHorizToInterpAndInterpToTarget[key] =
                            PairOfTransforms(srcToInterpOps, interpToTargetOps);
                    } else {
                        srcToInterpOps = iter->second.first;
                        interpToTargetOps = iter->second.second;
                    }
                } else {
#ifdef TRACE_CREATE_OPERATIONS
                    ENTER_BLOCK("looking for horizontal transformation "
                                "from source to interpCRS and interpCRS to "
                                "target");
#endif
                    computeOpsToInterp();
                }

#ifdef TRACE_CREATE_OPERATIONS
                ENTER_BLOCK("creating HorizVerticalHorizPROJBased operations");
#endif
                for (const auto &srcToInterp : srcToInterpOps) {
                    if (interpToTargetOps.empty()) {
                        try {
                            auto op = createHorizVerticalHorizPROJBased(
                                sourceCRS, targetCRS, srcToInterp,
                                verticalTransform, srcToInterp->inverse(),
                                interpolationGeogCRS, true);
                            res.emplace_back(op);
                        } catch (const std::exception &) {
                        }
                    } else {
                        for (const auto &interpToTarget : interpToTargetOps) {

                            if (useDifferentTransformationsForSameSourceTarget(
                                    srcToInterp, interpToTarget)) {
                                continue;
                            }

                            try {
                                auto op = createHorizVerticalHorizPROJBased(
                                    sourceCRS, targetCRS, srcToInterp,
                                    verticalTransform, interpToTarget,
                                    interpolationGeogCRS, true);
                                res.emplace_back(op);
                            } catch (const std::exception &) {
                            }
                        }
                    }
                }
            } else {
                // This case is probably only correct if
                // verticalTransform and horizTransform are independent
                // and in particular that verticalTransform does not
                // involve a grid, because of the rather arbitrary order
                // horizontal then vertical applied
                for (const auto &horizTransform : horizTransforms) {
                    try {
                        auto op = createHorizVerticalPROJBased(
                            sourceCRS, targetCRS, horizTransform,
                            verticalTransform, disallowEmptyIntersection);
                        res.emplace_back(op);
                    } catch (const std::exception &) {
                    }
                }
            }
        }
    }
}

// ---------------------------------------------------------------------------

void CoordinateOperationFactory::Private::createOperationsToGeod(
    const crs::CRSNNPtr &sourceCRS, const crs::CRSNNPtr &targetCRS,
    Private::Context &context, const crs::GeodeticCRS *geodDst,
    std::vector<CoordinateOperationNNPtr> &res) {

    auto cs = cs::EllipsoidalCS::createLatitudeLongitudeEllipsoidalHeight(
        common::UnitOfMeasure::DEGREE, common::UnitOfMeasure::METRE);
    auto intermGeog3DCRS =
        util::nn_static_pointer_cast<crs::CRS>(crs::GeographicCRS::create(
            util::PropertyMap()
                .set(common::IdentifiedObject::NAME_KEY, geodDst->nameStr())
                .set(common::ObjectUsage::DOMAIN_OF_VALIDITY_KEY,
                     metadata::Extent::WORLD),
            geodDst->datum(), geodDst->datumEnsemble(), cs));
    auto sourceToGeog3DOps =
        createOperations(sourceCRS, intermGeog3DCRS, context);
    auto geog3DToTargetOps =
        createOperations(intermGeog3DCRS, targetCRS, context);
    if (!geog3DToTargetOps.empty()) {
        for (const auto &op : sourceToGeog3DOps) {
            auto newOp = op->shallowClone();
            setCRSs(newOp.get(), sourceCRS, intermGeog3DCRS);
            try {
                res.emplace_back(ConcatenatedOperation::createComputeMetadata(
                    {newOp, geog3DToTargetOps.front()},
                    disallowEmptyIntersection));
            } catch (const InvalidOperationEmptyIntersection &) {
            }
        }
    }
}

// ---------------------------------------------------------------------------

void CoordinateOperationFactory::Private::createOperationsCompoundToCompound(
    const crs::CRSNNPtr &sourceCRS, const crs::CRSNNPtr &targetCRS,
    Private::Context &context, const crs::CompoundCRS *compoundSrc,
    const crs::CompoundCRS *compoundDst,
    std::vector<CoordinateOperationNNPtr> &res) {

    const auto &componentsSrc = compoundSrc->componentReferenceSystems();
    const auto &componentsDst = compoundDst->componentReferenceSystems();
    if (componentsSrc.empty() || componentsSrc.size() != componentsDst.size()) {
        return;
    }
    const auto srcGeog = componentsSrc[0]->extractGeographicCRS();
    const auto dstGeog = componentsDst[0]->extractGeographicCRS();
    if (srcGeog == nullptr || dstGeog == nullptr) {
        return;
    }

    std::vector<CoordinateOperationNNPtr> verticalTransforms;
    if (componentsSrc.size() >= 2 && componentsSrc[1]->extractVerticalCRS() &&
        componentsDst[1]->extractVerticalCRS()) {
        if (!componentsSrc[1]->_isEquivalentTo(componentsDst[1].get())) {
            verticalTransforms =
                createOperations(componentsSrc[1], componentsDst[1], context);
        }
    }

    // If we didn't find a non-ballpark transformation between
    // the 2 vertical CRS, then try through intermediate geographic CRS
    // For example
    // WGS 84 + EGM96 --> ETRS89 + Belfast height where
    // there is a geoid model for EGM96 referenced to WGS 84
    // and a geoid model for Belfast height referenced to ETRS89
    if (verticalTransforms.size() == 1 &&
        verticalTransforms.front()->hasBallparkTransformation()) {
        auto dbContext =
            context.context->getAuthorityFactory()->databaseContext();
        const auto intermGeogSrc =
            srcGeog->promoteTo3D(std::string(), dbContext);
        const bool intermGeogSrcIsSameAsIntermGeogDst =
            srcGeog->_isEquivalentTo(dstGeog.get());
        const auto intermGeogDst =
            intermGeogSrcIsSameAsIntermGeogDst
                ? intermGeogSrc
                : dstGeog->promoteTo3D(std::string(), dbContext);
        const auto opsSrcToGeog =
            createOperations(sourceCRS, intermGeogSrc, context);
        const auto opsGeogToTarget =
            createOperations(intermGeogDst, targetCRS, context);
        const bool hasNonTrivalSrcTransf =
            !opsSrcToGeog.empty() &&
            !opsSrcToGeog.front()->hasBallparkTransformation();
        const bool hasNonTrivialTargetTransf =
            !opsGeogToTarget.empty() &&
            !opsGeogToTarget.front()->hasBallparkTransformation();
        if (hasNonTrivalSrcTransf && hasNonTrivialTargetTransf) {
            const auto opsGeogSrcToGeogDst =
                createOperations(intermGeogSrc, intermGeogDst, context);
            for (const auto &op1 : opsSrcToGeog) {
                if (op1->hasBallparkTransformation()) {
                    // std::cerr << "excluded " << op1->nameStr() << std::endl;
                    continue;
                }
                for (const auto &op2 : opsGeogSrcToGeogDst) {
                    for (const auto &op3 : opsGeogToTarget) {
                        if (op3->hasBallparkTransformation()) {
                            // std::cerr << "excluded " << op3->nameStr() <<
                            // std::endl;
                            continue;
                        }
                        try {
                            res.emplace_back(
                                ConcatenatedOperation::createComputeMetadata(
                                    intermGeogSrcIsSameAsIntermGeogDst
                                        ? std::vector<
                                              CoordinateOperationNNPtr>{op1,
                                                                        op3}
                                        : std::vector<
                                              CoordinateOperationNNPtr>{op1,
                                                                        op2,
                                                                        op3},
                                    disallowEmptyIntersection));
                        } catch (const std::exception &) {
                        }
                    }
                }
            }
        }
        if (!res.empty()) {
            return;
        }
    }

    for (const auto &verticalTransform : verticalTransforms) {
        auto interpolationGeogCRS = NN_NO_CHECK(srcGeog);
        auto interpTransformCRS = verticalTransform->interpolationCRS();
        if (interpTransformCRS) {
            auto nn_interpTransformCRS = NN_NO_CHECK(interpTransformCRS);
            if (dynamic_cast<const crs::GeographicCRS *>(
                    nn_interpTransformCRS.get())) {
                interpolationGeogCRS = NN_NO_CHECK(
                    util::nn_dynamic_pointer_cast<crs::GeographicCRS>(
                        nn_interpTransformCRS));
            }
        } else {
            auto compSrc0BoundCrs =
                dynamic_cast<crs::BoundCRS *>(componentsSrc[0].get());
            auto compDst0BoundCrs =
                dynamic_cast<crs::BoundCRS *>(componentsDst[0].get());
            if (compSrc0BoundCrs && compDst0BoundCrs &&
                dynamic_cast<crs::GeographicCRS *>(
                    compSrc0BoundCrs->hubCRS().get()) &&
                compSrc0BoundCrs->hubCRS()->_isEquivalentTo(
                    compDst0BoundCrs->hubCRS().get())) {
                interpolationGeogCRS = NN_NO_CHECK(
                    util::nn_dynamic_pointer_cast<crs::GeographicCRS>(
                        compSrc0BoundCrs->hubCRS()));
            }
        }
        auto opSrcCRSToGeogCRS =
            createOperations(componentsSrc[0], interpolationGeogCRS, context);
        auto opGeogCRStoDstCRS =
            createOperations(interpolationGeogCRS, componentsDst[0], context);
        for (const auto &opSrc : opSrcCRSToGeogCRS) {
            for (const auto &opDst : opGeogCRStoDstCRS) {

                try {
                    auto op = createHorizVerticalHorizPROJBased(
                        sourceCRS, targetCRS, opSrc, verticalTransform, opDst,
                        interpolationGeogCRS, true);
                    res.emplace_back(op);
                } catch (const InvalidOperationEmptyIntersection &) {
                } catch (const io::FormattingException &) {
                }
            }
        }
    }

    if (verticalTransforms.empty()) {
        auto resTmp =
            createOperations(componentsSrc[0], componentsDst[0], context);
        for (const auto &op : resTmp) {
            auto opClone = op->shallowClone();
            setCRSs(opClone.get(), sourceCRS, targetCRS);
            res.emplace_back(opClone);
        }
    }
}

// ---------------------------------------------------------------------------

void CoordinateOperationFactory::Private::createOperationsBoundToCompound(
    const crs::CRSNNPtr &sourceCRS, const crs::CRSNNPtr &targetCRS,
    Private::Context &context, const crs::BoundCRS *boundSrc,
    const crs::CompoundCRS *compoundDst,
    std::vector<CoordinateOperationNNPtr> &res) {

    const auto &authFactory = context.context->getAuthorityFactory();
    const auto dbContext =
        authFactory ? authFactory->databaseContext().as_nullable() : nullptr;

    const auto &componentsDst = compoundDst->componentReferenceSystems();
    if (!componentsDst.empty()) {
        auto compDst0BoundCrs =
            dynamic_cast<crs::BoundCRS *>(componentsDst[0].get());
        if (compDst0BoundCrs) {
            auto boundSrcHubAsGeogCRS =
                dynamic_cast<crs::GeographicCRS *>(boundSrc->hubCRS().get());
            auto compDst0BoundCrsHubAsGeogCRS =
                dynamic_cast<crs::GeographicCRS *>(
                    compDst0BoundCrs->hubCRS().get());
            if (boundSrcHubAsGeogCRS && compDst0BoundCrsHubAsGeogCRS) {
                const auto boundSrcHubAsGeogCRSDatum =
                    boundSrcHubAsGeogCRS->datumNonNull(dbContext);
                const auto compDst0BoundCrsHubAsGeogCRSDatum =
                    compDst0BoundCrsHubAsGeogCRS->datumNonNull(dbContext);
                if (boundSrcHubAsGeogCRSDatum->_isEquivalentTo(
                        compDst0BoundCrsHubAsGeogCRSDatum.get())) {
                    auto cs = cs::EllipsoidalCS::
                        createLatitudeLongitudeEllipsoidalHeight(
                            common::UnitOfMeasure::DEGREE,
                            common::UnitOfMeasure::METRE);
                    auto intermGeog3DCRS = util::nn_static_pointer_cast<
                        crs::CRS>(crs::GeographicCRS::create(
                        util::PropertyMap()
                            .set(common::IdentifiedObject::NAME_KEY,
                                 boundSrcHubAsGeogCRS->nameStr())
                            .set(common::ObjectUsage::DOMAIN_OF_VALIDITY_KEY,
                                 metadata::Extent::WORLD),
                        boundSrcHubAsGeogCRS->datum(),
                        boundSrcHubAsGeogCRS->datumEnsemble(), cs));
                    auto sourceToGeog3DOps =
                        createOperations(sourceCRS, intermGeog3DCRS, context);
                    auto geog3DToTargetOps =
                        createOperations(intermGeog3DCRS, targetCRS, context);
                    for (const auto &opSrc : sourceToGeog3DOps) {
                        for (const auto &opDst : geog3DToTargetOps) {
                            if (opSrc->targetCRS() && opDst->sourceCRS() &&
                                !opSrc->targetCRS()->_isEquivalentTo(
                                    opDst->sourceCRS().get())) {
                                // Shouldn't happen normally, but typically
                                // one of them can be 2D and the other 3D
                                // due to above createOperations() not
                                // exactly setting the expected source and
                                // target CRS.
                                // So create an adapter operation...
                                auto intermOps = createOperations(
                                    NN_NO_CHECK(opSrc->targetCRS()),
                                    NN_NO_CHECK(opDst->sourceCRS()), context);
                                if (!intermOps.empty()) {
                                    res.emplace_back(
                                        ConcatenatedOperation::
                                            createComputeMetadata(
                                                {opSrc, intermOps.front(),
                                                 opDst},
                                                disallowEmptyIntersection));
                                }
                            } else {
                                res.emplace_back(
                                    ConcatenatedOperation::
                                        createComputeMetadata(
                                            {opSrc, opDst},
                                            disallowEmptyIntersection));
                            }
                        }
                    }
                    return;
                }
            }
        }
    }

    // There might be better things to do, but for now just ignore the
    // transformation of the bound CRS
    res = createOperations(boundSrc->baseCRS(), targetCRS, context);
}
//! @endcond

// ---------------------------------------------------------------------------

/** \brief Find a list of CoordinateOperation from sourceCRS to targetCRS.
 *
 * The operations are sorted with the most relevant ones first: by
 * descending
 * area (intersection of the transformation area with the area of interest,
 * or intersection of the transformation with the area of use of the CRS),
 * and
 * by increasing accuracy. Operations with unknown accuracy are sorted last,
 * whatever their area.
 *
 * When one of the source or target CRS has a vertical component but not the
 * other one, the one that has no vertical component is automatically promoted
 * to a 3D version, where its vertical axis is the ellipsoidal height in metres,
 * using the ellipsoid of the base geodetic CRS.
 *
 * @param sourceCRS source CRS.
 * @param targetCRS target CRS.
 * @param context Search context.
 * @return a list
 */
std::vector<CoordinateOperationNNPtr>
CoordinateOperationFactory::createOperations(
    const crs::CRSNNPtr &sourceCRS, const crs::CRSNNPtr &targetCRS,
    const CoordinateOperationContextNNPtr &context) const {

#ifdef TRACE_CREATE_OPERATIONS
    ENTER_FUNCTION();
#endif
    // Look if we are called on CRS that have a link to a 'canonical'
    // BoundCRS
    // If so, use that one as input
    const auto &srcBoundCRS = sourceCRS->canonicalBoundCRS();
    const auto &targetBoundCRS = targetCRS->canonicalBoundCRS();
    auto l_sourceCRS = srcBoundCRS ? NN_NO_CHECK(srcBoundCRS) : sourceCRS;
    auto l_targetCRS = targetBoundCRS ? NN_NO_CHECK(targetBoundCRS) : targetCRS;
    const auto &authFactory = context->getAuthorityFactory();

    metadata::ExtentPtr sourceCRSExtent;
    auto l_resolvedSourceCRS =
        crs::CRS::getResolvedCRS(l_sourceCRS, authFactory, sourceCRSExtent);
    metadata::ExtentPtr targetCRSExtent;
    auto l_resolvedTargetCRS =
        crs::CRS::getResolvedCRS(l_targetCRS, authFactory, targetCRSExtent);
    Private::Context contextPrivate(sourceCRSExtent, targetCRSExtent, context);

    if (context->getSourceAndTargetCRSExtentUse() ==
        CoordinateOperationContext::SourceTargetCRSExtentUse::INTERSECTION) {
        if (sourceCRSExtent && targetCRSExtent &&
            !sourceCRSExtent->intersects(NN_NO_CHECK(targetCRSExtent))) {
            return std::vector<CoordinateOperationNNPtr>();
        }
    }

    return filterAndSort(Private::createOperations(l_resolvedSourceCRS,
                                                   l_resolvedTargetCRS,
                                                   contextPrivate),
                         context, sourceCRSExtent, targetCRSExtent);
}

// ---------------------------------------------------------------------------

/** \brief Instantiate a CoordinateOperationFactory.
 */
CoordinateOperationFactoryNNPtr CoordinateOperationFactory::create() {
    return NN_NO_CHECK(
        CoordinateOperationFactory::make_unique<CoordinateOperationFactory>());
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress

InverseCoordinateOperation::~InverseCoordinateOperation() = default;

// ---------------------------------------------------------------------------

InverseCoordinateOperation::InverseCoordinateOperation(
    const CoordinateOperationNNPtr &forwardOperationIn,
    bool wktSupportsInversion)
    : forwardOperation_(forwardOperationIn),
      wktSupportsInversion_(wktSupportsInversion) {}

// ---------------------------------------------------------------------------

void InverseCoordinateOperation::setPropertiesFromForward() {
    setProperties(
        createPropertiesForInverse(forwardOperation_.get(), false, false));
    setAccuracies(forwardOperation_->coordinateOperationAccuracies());
    if (forwardOperation_->sourceCRS() && forwardOperation_->targetCRS()) {
        setCRSs(forwardOperation_.get(), true);
    }
    setHasBallparkTransformation(
        forwardOperation_->hasBallparkTransformation());
}

// ---------------------------------------------------------------------------

CoordinateOperationNNPtr InverseCoordinateOperation::inverse() const {
    return forwardOperation_;
}

// ---------------------------------------------------------------------------

void InverseCoordinateOperation::_exportToPROJString(
    io::PROJStringFormatter *formatter) const {
    formatter->startInversion();
    forwardOperation_->_exportToPROJString(formatter);
    formatter->stopInversion();
}

// ---------------------------------------------------------------------------

bool InverseCoordinateOperation::_isEquivalentTo(
    const util::IComparable *other, util::IComparable::Criterion criterion,
    const io::DatabaseContextPtr &dbContext) const {
    auto otherICO = dynamic_cast<const InverseCoordinateOperation *>(other);
    if (otherICO == nullptr ||
        !ObjectUsage::_isEquivalentTo(other, criterion, dbContext)) {
        return false;
    }
    return inverse()->_isEquivalentTo(otherICO->inverse().get(), criterion,
                                      dbContext);
}

// ---------------------------------------------------------------------------

PROJBasedOperation::~PROJBasedOperation() = default;

// ---------------------------------------------------------------------------

PROJBasedOperation::PROJBasedOperation(const OperationMethodNNPtr &methodIn)
    : SingleOperation(methodIn) {}

// ---------------------------------------------------------------------------

PROJBasedOperationNNPtr PROJBasedOperation::create(
    const util::PropertyMap &properties, const std::string &PROJString,
    const crs::CRSPtr &sourceCRS, const crs::CRSPtr &targetCRS,
    const std::vector<metadata::PositionalAccuracyNNPtr> &accuracies) {
    auto method = OperationMethod::create(
        util::PropertyMap().set(common::IdentifiedObject::NAME_KEY,
                                "PROJ-based operation method: " + PROJString),
        std::vector<GeneralOperationParameterNNPtr>{});
    auto op = PROJBasedOperation::nn_make_shared<PROJBasedOperation>(method);
    op->assignSelf(op);
    op->projString_ = PROJString;
    if (sourceCRS && targetCRS) {
        op->setCRSs(NN_NO_CHECK(sourceCRS), NN_NO_CHECK(targetCRS), nullptr);
    }
    op->setProperties(
        addDefaultNameIfNeeded(properties, "PROJ-based coordinate operation"));
    op->setAccuracies(accuracies);
    return op;
}

// ---------------------------------------------------------------------------

PROJBasedOperationNNPtr PROJBasedOperation::create(
    const util::PropertyMap &properties,
    const io::IPROJStringExportableNNPtr &projExportable, bool inverse,
    const crs::CRSNNPtr &sourceCRS, const crs::CRSNNPtr &targetCRS,
    const crs::CRSPtr &interpolationCRS,
    const std::vector<metadata::PositionalAccuracyNNPtr> &accuracies,
    bool hasBallparkTransformation) {

    auto formatter = io::PROJStringFormatter::create();
    if (inverse) {
        formatter->startInversion();
    }
    projExportable->_exportToPROJString(formatter.get());
    if (inverse) {
        formatter->stopInversion();
    }
    auto projString = formatter->toString();

    auto method = OperationMethod::create(
        util::PropertyMap().set(common::IdentifiedObject::NAME_KEY,
                                "PROJ-based operation method (approximate): " +
                                    projString),
        std::vector<GeneralOperationParameterNNPtr>{});
    auto op = PROJBasedOperation::nn_make_shared<PROJBasedOperation>(method);
    op->assignSelf(op);
    op->projString_ = projString;
    op->setCRSs(sourceCRS, targetCRS, interpolationCRS);
    op->setProperties(
        addDefaultNameIfNeeded(properties, "PROJ-based coordinate operation"));
    op->setAccuracies(accuracies);
    op->projStringExportable_ = projExportable.as_nullable();
    op->inverse_ = inverse;
    op->setHasBallparkTransformation(hasBallparkTransformation);
    return op;
}

// ---------------------------------------------------------------------------

CoordinateOperationNNPtr PROJBasedOperation::inverse() const {

    if (projStringExportable_ && sourceCRS() && targetCRS()) {
        return util::nn_static_pointer_cast<CoordinateOperation>(
            PROJBasedOperation::create(
                createPropertiesForInverse(this, false, false),
                NN_NO_CHECK(projStringExportable_), !inverse_,
                NN_NO_CHECK(targetCRS()), NN_NO_CHECK(sourceCRS()),
                interpolationCRS(), coordinateOperationAccuracies(),
                hasBallparkTransformation()));
    }

    auto formatter = io::PROJStringFormatter::create();
    formatter->startInversion();
    try {
        formatter->ingestPROJString(projString_);
    } catch (const io::ParsingException &e) {
        throw util::UnsupportedOperationException(
            std::string("PROJBasedOperation::inverse() failed: ") + e.what());
    }
    formatter->stopInversion();

    auto op = PROJBasedOperation::create(
        createPropertiesForInverse(this, false, false), formatter->toString(),
        targetCRS(), sourceCRS(), coordinateOperationAccuracies());
    if (sourceCRS() && targetCRS()) {
        op->setCRSs(NN_NO_CHECK(targetCRS()), NN_NO_CHECK(sourceCRS()),
                    interpolationCRS());
    }
    op->setHasBallparkTransformation(hasBallparkTransformation());
    return util::nn_static_pointer_cast<CoordinateOperation>(op);
}

// ---------------------------------------------------------------------------

void PROJBasedOperation::_exportToWKT(io::WKTFormatter *formatter) const {

    if (sourceCRS() && targetCRS()) {
        exportTransformationToWKT(formatter);
        return;
    }

    const bool isWKT2 = formatter->version() == io::WKTFormatter::Version::WKT2;
    if (!isWKT2) {
        throw io::FormattingException(
            "PROJBasedOperation can only be exported to WKT2");
    }

    formatter->startNode(io::WKTConstants::CONVERSION, false);
    formatter->addQuotedString(nameStr());
    method()->_exportToWKT(formatter);

    for (const auto &paramValue : parameterValues()) {
        paramValue->_exportToWKT(formatter);
    }
    formatter->endNode();
}

// ---------------------------------------------------------------------------

void PROJBasedOperation::_exportToJSON(
    io::JSONFormatter *formatter) const // throw(FormattingException)
{
    auto writer = formatter->writer();
    auto objectContext(formatter->MakeObjectContext(
        (sourceCRS() && targetCRS()) ? "Transformation" : "Conversion",
        !identifiers().empty()));

    writer->AddObjKey("name");
    auto l_name = nameStr();
    if (l_name.empty()) {
        writer->Add("unnamed");
    } else {
        writer->Add(l_name);
    }

    if (sourceCRS() && targetCRS()) {
        writer->AddObjKey("source_crs");
        formatter->setAllowIDInImmediateChild();
        sourceCRS()->_exportToJSON(formatter);

        writer->AddObjKey("target_crs");
        formatter->setAllowIDInImmediateChild();
        targetCRS()->_exportToJSON(formatter);
    }

    writer->AddObjKey("method");
    formatter->setOmitTypeInImmediateChild();
    formatter->setAllowIDInImmediateChild();
    method()->_exportToJSON(formatter);

    const auto &l_parameterValues = parameterValues();
    if (!l_parameterValues.empty()) {
        writer->AddObjKey("parameters");
        {
            auto parametersContext(writer->MakeArrayContext(false));
            for (const auto &genOpParamvalue : l_parameterValues) {
                formatter->setAllowIDInImmediateChild();
                formatter->setOmitTypeInImmediateChild();
                genOpParamvalue->_exportToJSON(formatter);
            }
        }
    }
}

// ---------------------------------------------------------------------------

void PROJBasedOperation::_exportToPROJString(
    io::PROJStringFormatter *formatter) const {
    if (projStringExportable_) {
        if (inverse_) {
            formatter->startInversion();
        }
        projStringExportable_->_exportToPROJString(formatter);
        if (inverse_) {
            formatter->stopInversion();
        }
        return;
    }

    try {
        formatter->ingestPROJString(projString_);
    } catch (const io::ParsingException &e) {
        throw io::FormattingException(
            std::string("PROJBasedOperation::exportToPROJString() failed: ") +
            e.what());
    }
}

// ---------------------------------------------------------------------------

CoordinateOperationNNPtr PROJBasedOperation::_shallowClone() const {
    auto op = PROJBasedOperation::nn_make_shared<PROJBasedOperation>(*this);
    op->assignSelf(op);
    op->setCRSs(this, false);
    return util::nn_static_pointer_cast<CoordinateOperation>(op);
}

// ---------------------------------------------------------------------------

std::set<GridDescription>
PROJBasedOperation::gridsNeeded(const io::DatabaseContextPtr &databaseContext,
                                bool considerKnownGridsAsAvailable) const {
    std::set<GridDescription> res;

    try {
        auto formatterOut = io::PROJStringFormatter::create();
        auto formatter = io::PROJStringFormatter::create();
        formatter->ingestPROJString(exportToPROJString(formatterOut.get()));
        const auto usedGridNames = formatter->getUsedGridNames();
        for (const auto &shortName : usedGridNames) {
            GridDescription desc;
            desc.shortName = shortName;
            if (databaseContext) {
                databaseContext->lookForGridInfo(
                    desc.shortName, considerKnownGridsAsAvailable,
                    desc.fullName, desc.packageName, desc.url,
                    desc.directDownload, desc.openLicense, desc.available);
            }
            res.insert(desc);
        }
    } catch (const io::ParsingException &) {
    }

    return res;
}

//! @endcond

// ---------------------------------------------------------------------------

} // namespace operation

namespace crs {
// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress

crs::CRSNNPtr CRS::getResolvedCRS(const crs::CRSNNPtr &crs,
                                  const io::AuthorityFactoryPtr &authFactory,
                                  metadata::ExtentPtr &extentOut) {
    const auto &ids = crs->identifiers();
    const auto &name = crs->nameStr();

    bool approxExtent;
    extentOut = getExtentPossiblySynthetized(crs, approxExtent);

    // We try to "identify" the provided CRS with the ones of the database,
    // but in a more restricted way that what identify() does.
    // If we get a match from id in priority, and from name as a fallback, and
    // that they are equivalent to the input CRS, then use the identified CRS.
    // Even if they aren't equivalent, we update extentOut with the one of the
    // identified CRS if our input one is absent/not reliable.

    const auto tryToIdentifyByName = [&crs, &name, &authFactory, approxExtent,
                                      &extentOut](
        io::AuthorityFactory::ObjectType objectType) {
        if (name != "unknown" && name != "unnamed") {
            auto matches = authFactory->createObjectsFromName(
                name, {objectType}, false, 2);
            if (matches.size() == 1) {
                const auto match =
                    util::nn_static_pointer_cast<crs::CRS>(matches.front());
                if (approxExtent || !extentOut) {
                    extentOut = getExtent(match);
                }
                if (match->isEquivalentTo(
                        crs.get(), util::IComparable::Criterion::EQUIVALENT)) {
                    return match;
                }
            }
        }
        return crs;
    };

    auto geogCRS = dynamic_cast<crs::GeographicCRS *>(crs.get());
    if (geogCRS && authFactory) {
        if (!ids.empty()) {
            const auto tmpAuthFactory = io::AuthorityFactory::create(
                authFactory->databaseContext(), *ids.front()->codeSpace());
            try {
                auto resolvedCrs(
                    tmpAuthFactory->createGeographicCRS(ids.front()->code()));
                if (approxExtent || !extentOut) {
                    extentOut = getExtent(resolvedCrs);
                }
                if (resolvedCrs->isEquivalentTo(
                        crs.get(), util::IComparable::Criterion::EQUIVALENT)) {
                    return util::nn_static_pointer_cast<crs::CRS>(resolvedCrs);
                }
            } catch (const std::exception &) {
            }
        } else {
            return tryToIdentifyByName(
                geogCRS->coordinateSystem()->axisList().size() == 2
                    ? io::AuthorityFactory::ObjectType::GEOGRAPHIC_2D_CRS
                    : io::AuthorityFactory::ObjectType::GEOGRAPHIC_3D_CRS);
        }
    }

    auto projectedCrs = dynamic_cast<crs::ProjectedCRS *>(crs.get());
    if (projectedCrs && authFactory) {
        if (!ids.empty()) {
            const auto tmpAuthFactory = io::AuthorityFactory::create(
                authFactory->databaseContext(), *ids.front()->codeSpace());
            try {
                auto resolvedCrs(
                    tmpAuthFactory->createProjectedCRS(ids.front()->code()));
                if (approxExtent || !extentOut) {
                    extentOut = getExtent(resolvedCrs);
                }
                if (resolvedCrs->isEquivalentTo(
                        crs.get(), util::IComparable::Criterion::EQUIVALENT)) {
                    return util::nn_static_pointer_cast<crs::CRS>(resolvedCrs);
                }
            } catch (const std::exception &) {
            }
        } else {
            return tryToIdentifyByName(
                io::AuthorityFactory::ObjectType::PROJECTED_CRS);
        }
    }

    auto compoundCrs = dynamic_cast<crs::CompoundCRS *>(crs.get());
    if (compoundCrs && authFactory) {
        if (!ids.empty()) {
            const auto tmpAuthFactory = io::AuthorityFactory::create(
                authFactory->databaseContext(), *ids.front()->codeSpace());
            try {
                auto resolvedCrs(
                    tmpAuthFactory->createCompoundCRS(ids.front()->code()));
                if (approxExtent || !extentOut) {
                    extentOut = getExtent(resolvedCrs);
                }
                if (resolvedCrs->isEquivalentTo(
                        crs.get(), util::IComparable::Criterion::EQUIVALENT)) {
                    return util::nn_static_pointer_cast<crs::CRS>(resolvedCrs);
                }
            } catch (const std::exception &) {
            }
        } else {
            auto outCrs = tryToIdentifyByName(
                io::AuthorityFactory::ObjectType::COMPOUND_CRS);
            const auto &components = compoundCrs->componentReferenceSystems();
            if (outCrs.get() != crs.get()) {
                bool hasGeoid = false;
                if (components.size() == 2) {
                    auto vertCRS =
                        dynamic_cast<crs::VerticalCRS *>(components[1].get());
                    if (vertCRS && !vertCRS->geoidModel().empty()) {
                        hasGeoid = true;
                    }
                }
                if (!hasGeoid) {
                    return outCrs;
                }
            }
            if (approxExtent || !extentOut) {
                // If we still did not get a reliable extent, then try to
                // resolve the components of the compoundCRS, and take the
                // intersection of their extent.
                extentOut = metadata::ExtentPtr();
                for (const auto &component : components) {
                    metadata::ExtentPtr componentExtent;
                    getResolvedCRS(component, authFactory, componentExtent);
                    if (extentOut && componentExtent)
                        extentOut = extentOut->intersection(
                            NN_NO_CHECK(componentExtent));
                    else if (componentExtent)
                        extentOut = componentExtent;
                }
            }
        }
    }
    return crs;
}

//! @endcond

} // namespace crs
NS_PROJ_END
