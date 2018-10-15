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
#define FROM_COORDINATE_OPERATION_CPP

#include "proj/coordinateoperation.hpp"
#include "proj/common.hpp"
#include "proj/crs.hpp"
#include "proj/io.hpp"
#include "proj/metadata.hpp"
#include "proj/util.hpp"

#include "proj/internal/coordinateoperation_constants.hpp"
#include "proj/internal/coordinateoperation_internal.hpp"
#include "proj/internal/internal.hpp"
#include "proj/internal/io_internal.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstring>
#include <memory>
#include <set>
#include <sstream> // std::ostringstream
#include <string>
#include <vector>

using namespace NS_PROJ::internal;

// ---------------------------------------------------------------------------

NS_PROJ_START
namespace operation {

//! @cond Doxygen_Suppress
static const std::string INVERSE_OF = "Inverse of ";
//! @endcond

//! @cond Doxygen_Suppress
static util::PropertyMap
createPropertiesForInverse(const CoordinateOperation *op, bool derivedFrom,
                           bool approximateInversion);
//! @endcond

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

using PairOfString = std::pair<std::string, std::string>;

static std::set<PairOfString> buildSetEquivalentParameters() {

    std::set<PairOfString> set;
    for (const auto &paramList : listOfEquivalentParameterNames) {
        for (size_t i = 0; i < paramList.size(); i++) {
            auto a = metadata::Identifier::canonicalizeName(paramList[i]);
            for (size_t j = i + 1; j < paramList.size(); j++) {
                auto b = metadata::Identifier::canonicalizeName(paramList[j]);
                if (a < b)
                    set.insert(PairOfString(a, b));
                else
                    set.insert(PairOfString(b, a));
            }
        }
    }
    return set;
}

static bool areEquivalentParameters(const std::string &a,
                                    const std::string &b) {

    static const std::set<PairOfString> setEquivalentParameters =
        buildSetEquivalentParameters();

    auto a_can = metadata::Identifier::canonicalizeName(a);
    auto b_can = metadata::Identifier::canonicalizeName(b);
    return setEquivalentParameters.find((a_can < b_can)
                                            ? PairOfString(a_can, b_can)
                                            : PairOfString(b_can, a_can)) !=
           setEquivalentParameters.end();
}

// ---------------------------------------------------------------------------

const MethodMapping *getMapping(int epsg_code) {
    for (const auto &mapping : methodMappings) {
        if (mapping.epsg_code == epsg_code) {
            return &mapping;
        }
    }
    return nullptr;
}

// ---------------------------------------------------------------------------

const MethodMapping *getMapping(const OperationMethod *method) {
    const std::string name(*(method->name()->description()));
    const std::string &code = method->name()->code();
    const int epsg_code = !code.empty() ? ::atoi(code.c_str()) : 0;
    for (const auto &mapping : methodMappings) {
        if (metadata::Identifier::isEquivalentName(mapping.wkt2_name, name) ||
            (epsg_code != 0 && mapping.epsg_code == epsg_code)) {
            return &mapping;
        }
    }
    return nullptr;
}

// ---------------------------------------------------------------------------

const MethodMapping *getMappingFromWKT1(const std::string &wkt1_name) {
    // Unusual for a WKT1 projection name, but mentionned in OGC 12-063r5 C.4.2
    if (tolower(wkt1_name).find(tolower("UTM zone")) == 0) {
        return getMapping(EPSG_CODE_METHOD_TRANSVERSE_MERCATOR);
    }

    for (const auto &mapping : methodMappings) {
        if (metadata::Identifier::isEquivalentName(mapping.wkt1_name,
                                                   wkt1_name)) {
            return &mapping;
        }
    }
    return nullptr;
}

// ---------------------------------------------------------------------------

const MethodMapping *getMapping(const std::string &wkt2_name) {
    for (const auto &mapping : methodMappings) {
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
    for (const auto &mapping : methodMappings) {
        if (!mapping.proj_names.empty() && mapping.proj_names[0] == projName) {
            res.push_back(&mapping);
        }
    }
    return res;
}

// ---------------------------------------------------------------------------

const ParamMapping *getMapping(const MethodMapping *mapping,
                               const OperationParameterValue *param) {
    const auto &param_name = param->parameter()->name();
    const std::string name = *(param_name->description());
    const std::string &code = param_name->code();
    const int epsg_code = !code.empty() ? ::atoi(code.c_str()) : 0;
    for (const auto &paramMapping : mapping->params) {
        if (metadata::Identifier::isEquivalentName(paramMapping.wkt2_name,
                                                   name) ||
            (epsg_code != 0 && paramMapping.epsg_code == epsg_code) ||
            areEquivalentParameters(paramMapping.wkt2_name, name)) {
            return &paramMapping;
        }
    }
    return nullptr;
}

// ---------------------------------------------------------------------------

const ParamMapping *getMappingFromWKT1(const MethodMapping *mapping,
                                       const std::string &wkt1_name) {
    for (const auto &paramMapping : mapping->params) {
        if (metadata::Identifier::isEquivalentName(paramMapping.wkt1_name,
                                                   wkt1_name) ||
            areEquivalentParameters(paramMapping.wkt1_name, wkt1_name)) {
            return &paramMapping;
        }
    }
    return nullptr;
}

// ---------------------------------------------------------------------------

static double getAccuracy(const std::vector<CoordinateOperationNNPtr> &ops);

// Returns the accuracy of an operation, or -1 if unknown
static double getAccuracy(CoordinateOperationNNPtr op) {

    if (util::nn_dynamic_pointer_cast<Conversion>(op)) {
        // A conversion is perfectly accurate.
        return 0.0;
    }

    double accuracy = -1.0;
    auto accuracies = op->coordinateOperationAccuracies();
    if (!accuracies.empty()) {
        try {
            accuracy = c_locale_stod(accuracies[0]->value());
        } catch (const std::exception &) {
        }
    } else {
        auto concatenated =
            util::nn_dynamic_pointer_cast<ConcatenatedOperation>(op);
        if (concatenated) {
            accuracy = getAccuracy(concatenated->operations());
        }
    }
    return accuracy;
}

// ---------------------------------------------------------------------------

// Returns the accuracy of a set of concantenated operations, or -1 if unknown
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

static metadata::ExtentPtr getExtent(CoordinateOperationNNPtr op,
                                     bool conversionExtentIsWorld,
                                     bool &emptyIntersection) {
    auto conv = util::nn_dynamic_pointer_cast<Conversion>(op);
    if (conv) {
        emptyIntersection = false;
        return metadata::Extent::WORLD;
    }
    auto domains = op->domains();
    if (!domains.empty()) {
        emptyIntersection = false;
        return domains[0]->domainOfValidity();
    }
    auto concatenated =
        util::nn_dynamic_pointer_cast<ConcatenatedOperation>(op);
    if (!concatenated) {
        emptyIntersection = false;
        return nullptr;
    }
    return getExtent(concatenated->operations(), conversionExtentIsWorld,
                     emptyIntersection);
}

// ---------------------------------------------------------------------------

static metadata::ExtentPtr getExtent(crs::CRSNNPtr crs) {
    auto domains = crs->domains();
    if (!domains.empty()) {
        return domains[0]->domainOfValidity();
    }
    return nullptr;
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
            res = res->intersection(NN_CHECK_ASSERT(subExtent));
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

static double getPseudoArea(metadata::ExtentPtr extent) {
    if (!extent)
        return 0.0;
    auto geographicElements = extent->geographicElements();
    if (geographicElements.empty())
        return 0.0;
    auto bbox = util::nn_dynamic_pointer_cast<metadata::GeographicBoundingBox>(
        geographicElements[0]);
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
    crs::CRSPtr sourceCRSLocked_{}; // do not set this for a
                                    // ProjectedCRS.definingConversion
    crs::CRSPtr targetCRSLocked_{}; // do not set this for a
                                    // ProjectedCRS.definingConversion
    crs::CRSPtr interpolationCRS_{};
    util::optional<common::DataEpoch> sourceCoordinateEpoch_{};
    util::optional<common::DataEpoch> targetCoordinateEpoch_{};
};
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
    d->sourceCRSLocked_ = sourceCRSIn;
    d->sourceCRSWeak_ = sourceCRSIn.as_nullable();
    d->targetCRSLocked_ = targetCRSIn;
    d->targetCRSWeak_ = targetCRSIn.as_nullable();
    d->interpolationCRS_ = interpolationCRSIn;
}

// ---------------------------------------------------------------------------

void CoordinateOperation::setAccuracies(
    const std::vector<metadata::PositionalAccuracyNNPtr> &accuracies) {
    d->coordinateOperationAccuracies_ = accuracies;
}

// ---------------------------------------------------------------------------

/** \brief Return whether a coordinate operation can be instanciated as
 * a PROJ pipeline, checking in particular that referenced grids are
 * available.
 */
bool CoordinateOperation::isPROJInstanciable(
    const io::DatabaseContextNNPtr &databaseContext) const {
    try {
        exportToPROJString(io::PROJStringFormatter::create());
    } catch (const std::exception &) {
        return false;
    }
    for (const auto &gridDesc : gridsNeeded(databaseContext)) {
        if (!gridDesc.available) {
            return false;
        }
    }
    return true;
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
struct OperationMethod::Private {
    util::optional<std::string> formula_{};
    util::optional<metadata::Citation> formulaCitation_{};
    std::vector<GeneralOperationParameterNNPtr> parameters_{};
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
const util::optional<std::string> &OperationMethod::formula() const {
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
OperationMethod::formulaCitation() const {
    return d->formulaCitation_;
}

// ---------------------------------------------------------------------------

/** \brief Return the parameters of this operation method.
 *
 * @return the parameters.
 */
const std::vector<GeneralOperationParameterNNPtr> &
OperationMethod::parameters() const {
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

std::string
OperationMethod::exportToWKT(io::WKTFormatterNNPtr formatter) const {
    const bool isWKT2 = formatter->version() == io::WKTFormatter::Version::WKT2;
    formatter->startNode(isWKT2 ? io::WKTConstants::METHOD
                                : io::WKTConstants::PROJECTION,
                         !identifiers().empty());
    std::string l_name(*(name()->description()));
    if (!isWKT2) {
        const MethodMapping *mapping = getMapping(this);
        if (mapping == nullptr) {
            l_name = replaceAll(l_name, " ", "_");
        } else {
            l_name = mapping->wkt1_name;
            if (l_name.empty()) {
                throw io::FormattingException(
                    "Unsupported conversion method: " + mapping->wkt2_name);
            }
        }
    }
    formatter->addQuotedString(l_name);
    if (formatter->outputId()) {
        formatID(formatter);
    }
    formatter->endNode();
    return formatter->toString();
}

// ---------------------------------------------------------------------------

bool OperationMethod::isEquivalentTo(
    const util::BaseObjectNNPtr &other,
    util::IComparable::Criterion criterion) const {
    auto otherOM = util::nn_dynamic_pointer_cast<OperationMethod>(other);
    if (otherOM == nullptr ||
        !IdentifiedObject::_isEquivalentTo(other, criterion)) {
        return false;
    }
    // TODO test formula and formulaCitation
    const auto &params = parameters();
    const auto &otherParams = otherOM->parameters();
    if (params.size() != otherParams.size()) {
        return false;
    }
    for (size_t i = 0; i < params.size(); i++) {
        if (!params[i]->isEquivalentTo(otherParams[i], criterion)) {
            return false;
        }
    }
    return true;
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
struct GeneralParameterValue::Private {};
//! @endcond

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
GeneralParameterValue::GeneralParameterValue()
    : d(internal::make_unique<Private>()) {}

// ---------------------------------------------------------------------------

GeneralParameterValue::GeneralParameterValue(const GeneralParameterValue &other)
    : d(internal::make_unique<Private>(*other.d)) {}
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
    OperationParameterValueNNPtr opv(
        OperationParameterValue::nn_make_shared<OperationParameterValue>(
            parameterIn, valueIn));
    return opv;
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
const OperationParameterNNPtr &OperationParameterValue::parameter() const {
    return d->parameter;
}

// ---------------------------------------------------------------------------

/** \brief Return the parameter value.
 *
 * @return the parameter value.
 */
const ParameterValueNNPtr &OperationParameterValue::parameterValue() const {
    return d->parameterValue;
}

// ---------------------------------------------------------------------------

std::string OperationParameterValue::exportToWKT(
    // cppcheck-suppress passedByValue
    io::WKTFormatterNNPtr formatter) const {
    return _exportToWKT(formatter, nullptr);
}

std::string
OperationParameterValue::_exportToWKT(io::WKTFormatterNNPtr formatter,
                                      const MethodMapping *mapping) const {
    const ParamMapping *paramMapping =
        mapping ? getMapping(mapping, this) : nullptr;
    if (paramMapping && paramMapping->wkt1_name.empty())
        return std::string();
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
        formatter->addQuotedString(*(parameter()->name()->description()));
    }
    parameterValue()->exportToWKT(formatter);
    if (formatter->outputId()) {
        parameter()->formatID(formatter);
    }
    formatter->endNode();
    return formatter->toString();
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress

/** Utility method used on WKT2 import to convert from abridged transformation
 * to "normal" transformation parameters.
 */
bool OperationParameterValue::convertFromAbridged(const std::string &paramName,
                                                  double &val,
                                                  common::UnitOfMeasure &unit,
                                                  int &paramEPSGCode) {
    if (metadata::Identifier::isEquivalentName(
            paramName, EPSG_NAME_PARAMETER_X_AXIS_TRANSLATION) ||
        paramEPSGCode == EPSG_CODE_PARAMETER_X_AXIS_TRANSLATION) {
        unit = common::UnitOfMeasure::METRE;
        paramEPSGCode = EPSG_CODE_PARAMETER_X_AXIS_TRANSLATION;
        return true;
    } else if (metadata::Identifier::isEquivalentName(
                   paramName, EPSG_NAME_PARAMETER_Y_AXIS_TRANSLATION) ||
               paramEPSGCode == EPSG_CODE_PARAMETER_Y_AXIS_TRANSLATION) {
        unit = common::UnitOfMeasure::METRE;
        paramEPSGCode = EPSG_CODE_PARAMETER_Y_AXIS_TRANSLATION;
        return true;
    } else if (metadata::Identifier::isEquivalentName(
                   paramName, EPSG_NAME_PARAMETER_Z_AXIS_TRANSLATION) ||
               paramEPSGCode == EPSG_CODE_PARAMETER_Z_AXIS_TRANSLATION) {
        unit = common::UnitOfMeasure::METRE;
        paramEPSGCode = EPSG_CODE_PARAMETER_Z_AXIS_TRANSLATION;
        return true;
    } else if (metadata::Identifier::isEquivalentName(
                   paramName, EPSG_NAME_PARAMETER_X_AXIS_ROTATION) ||
               paramEPSGCode == EPSG_CODE_PARAMETER_X_AXIS_ROTATION) {
        unit = common::UnitOfMeasure::ARC_SECOND;
        paramEPSGCode = EPSG_CODE_PARAMETER_X_AXIS_ROTATION;
        return true;
    } else if (metadata::Identifier::isEquivalentName(
                   paramName, EPSG_NAME_PARAMETER_Y_AXIS_ROTATION) ||
               paramEPSGCode == EPSG_CODE_PARAMETER_Y_AXIS_ROTATION) {
        unit = common::UnitOfMeasure::ARC_SECOND;
        paramEPSGCode = EPSG_CODE_PARAMETER_Y_AXIS_ROTATION;
        return true;

    } else if (metadata::Identifier::isEquivalentName(
                   paramName, EPSG_NAME_PARAMETER_Z_AXIS_ROTATION) ||
               paramEPSGCode == EPSG_CODE_PARAMETER_Z_AXIS_ROTATION) {
        unit = common::UnitOfMeasure::ARC_SECOND;
        paramEPSGCode = EPSG_CODE_PARAMETER_Z_AXIS_ROTATION;
        return true;

    } else if (metadata::Identifier::isEquivalentName(
                   paramName, EPSG_NAME_PARAMETER_SCALE_DIFFERENCE) ||
               paramEPSGCode == EPSG_CODE_PARAMETER_SCALE_DIFFERENCE) {
        val = (val - 1.0) * 1e6;
        unit = common::UnitOfMeasure::PARTS_PER_MILLION;
        paramEPSGCode = EPSG_CODE_PARAMETER_SCALE_DIFFERENCE;
        return true;
    }
    return false;
}
//! @endcond

// ---------------------------------------------------------------------------

bool OperationParameterValue::isEquivalentTo(
    const util::BaseObjectNNPtr &other,
    util::IComparable::Criterion criterion) const {
    auto otherOPV =
        util::nn_dynamic_pointer_cast<OperationParameterValue>(other);
    if (otherOPV == nullptr) {
        return false;
    }
    return parameter()->isEquivalentTo(otherOPV->parameter(), criterion) &&
           parameterValue()->isEquivalentTo(

               otherOPV->parameterValue(), criterion);
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
struct GeneralOperationParameter::Private {};
//! @endcond

// ---------------------------------------------------------------------------

GeneralOperationParameter::GeneralOperationParameter()
    : d(internal::make_unique<Private>()) {}

// ---------------------------------------------------------------------------

GeneralOperationParameter::GeneralOperationParameter(
    const GeneralOperationParameter &other)
    : IdentifiedObject(other), d(internal::make_unique<Private>(*other.d)) {}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
GeneralOperationParameter::~GeneralOperationParameter() = default;
//! @endcond

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
struct OperationParameter::Private {};
//! @endcond

// ---------------------------------------------------------------------------

OperationParameter::OperationParameter()
    : d(internal::make_unique<Private>()) {}

// ---------------------------------------------------------------------------

OperationParameter::OperationParameter(const OperationParameter &other)
    : GeneralOperationParameter(other),
      d(internal::make_unique<Private>(*other.d)) {}

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

bool OperationParameter::isEquivalentTo(
    const util::BaseObjectNNPtr &other,
    util::IComparable::Criterion criterion) const {
    auto otherOP = util::nn_dynamic_pointer_cast<OperationParameter>(other);
    return otherOP != nullptr &&
           IdentifiedObject::_isEquivalentTo(other, criterion);
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
SingleOperation::parameterValues() const {
    return d->parameterValues_;
}

// ---------------------------------------------------------------------------

/** \brief Return the operation method associated to the operation.
 *
 * @return the operation method.
 */
const OperationMethodNNPtr &SingleOperation::method() const {
    return d->method_;
}

// ---------------------------------------------------------------------------

void SingleOperation::setParameterValues(
    const std::vector<GeneralParameterValueNNPtr> &values) {
    d->parameterValues_ = values;
}

// ---------------------------------------------------------------------------

/** \brief Return the parameter value corresponding to a parameter name or
 * EPSG code
 *
 * @param paramName the parameter name (or empty, in which case epsg_code
 *                  should be non zero)
 * @param epsg_code the parameter EPSG code (possibly zero)
 * @return the value, or nullptr if not found.
 */
ParameterValuePtr SingleOperation::parameterValue(const std::string &paramName,
                                                  int epsg_code) const {
    for (const auto &genOpParamvalue : parameterValues()) {
        const auto &opParamvalue =
            util::nn_dynamic_pointer_cast<OperationParameterValue>(
                genOpParamvalue);
        if (opParamvalue) {
            const auto &paramNameIter =
                *(opParamvalue->parameter()->name()->description());
            if (ci_equal(paramName, paramNameIter) ||
                (epsg_code != 0 &&
                 opParamvalue->parameter()->isEPSG(epsg_code))) {
                return opParamvalue->parameterValue();
            }
        }
    }
    return nullptr;
}

// ---------------------------------------------------------------------------

/** \brief Return the parameter value, as a measure, corresponding to a
 * parameter name or EPSG code
 *
 * @param paramName the parameter name (or empty, in which case epsg_code
 *                  should be non zero)
 * @param epsg_code the parameter EPSG code (possibly zero)
 * @return the measure, or the empty Measure() object if not found.
 */
common::Measure
SingleOperation::parameterValueMeasure(const std::string &paramName,
                                       int epsg_code) const {
    auto val = parameterValue(paramName, epsg_code);
    if (val && val->type() == ParameterValue::Type::MEASURE) {
        return val->value();
    }
    return common::Measure();
}

// ---------------------------------------------------------------------------

/** \brief Instanciate a PROJ-based single operation;
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
    const std::vector<metadata::PositionalAccuracyNNPtr> &accuracies =
        std::vector<metadata::PositionalAccuracyNNPtr>()) {
    return util::nn_static_pointer_cast<SingleOperation>(
        PROJBasedOperation::create(properties, projExportable, false, sourceCRS,
                                   targetCRS, accuracies));
}
//! @endcond

// ---------------------------------------------------------------------------

bool SingleOperation::isEquivalentTo(
    const util::BaseObjectNNPtr &other,
    util::IComparable::Criterion criterion) const {
    auto otherSO = util::nn_dynamic_pointer_cast<SingleOperation>(other);
    if (otherSO == nullptr || !ObjectUsage::_isEquivalentTo(other, criterion)) {
        return false;
    }
    if (!method()->isEquivalentTo(otherSO->method(), criterion)) {
        return false;
    }
    const auto &values = parameterValues();
    const auto &otherValues = otherSO->parameterValues();
    if (values.size() != otherValues.size()) {
        return false;
    }
    for (size_t i = 0; i < values.size(); i++) {
        if (!values[i]->isEquivalentTo(otherValues[i], criterion)) {
            return false;
        }
    }
    return true;
}

// ---------------------------------------------------------------------------

std::set<CoordinateOperation::GridDescription>
SingleOperation::gridsNeeded(io::DatabaseContextNNPtr databaseContext) const {
    std::set<CoordinateOperation::GridDescription> res;
    for (const auto &genOpParamvalue : parameterValues()) {
        const auto &opParamvalue =
            util::nn_dynamic_pointer_cast<OperationParameterValue>(
                genOpParamvalue);
        if (opParamvalue) {
            const auto &value = opParamvalue->parameterValue();
            if (value->type() == ParameterValue::Type::FILENAME) {
                CoordinateOperation::GridDescription desc;
                desc.shortName = value->valueFile();
                databaseContext->lookForGridInfo(
                    desc.shortName, desc.fullName, desc.packageName, desc.url,
                    desc.directDownload, desc.openLicense, desc.available);
                res.insert(desc);
            }
        }
    }
    return res;
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
struct ParameterValue::Private {
    ParameterValue::Type type_{ParameterValue::Type::STRING};
    common::Measure measure_{};
    std::string stringValue_{};
    int integerValue_{};
    bool booleanValue_{};

    explicit Private(const common::Measure &valueIn)
        : type_(ParameterValue::Type::MEASURE), measure_(valueIn) {}

    Private(const std::string &stringValueIn, ParameterValue::Type typeIn)
        : type_(typeIn), stringValue_(stringValueIn) {}

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

/** \brief Instanciate a ParameterValue from a Measure (i.e. a value associated
 * with a
 * unit)
 *
 * @return a new ParameterValue.
 */
ParameterValueNNPtr ParameterValue::create(const common::Measure &measureIn) {
    return ParameterValue::nn_make_shared<ParameterValue>(measureIn);
}

// ---------------------------------------------------------------------------

/** \brief Instanciate a ParameterValue from a string value.
 *
 * @return a new ParameterValue.
 */
ParameterValueNNPtr ParameterValue::create(const char *stringValueIn) {
    return ParameterValue::nn_make_shared<ParameterValue>(
        std::string(stringValueIn), ParameterValue::Type::STRING);
}

// ---------------------------------------------------------------------------

/** \brief Instanciate a ParameterValue from a string value.
 *
 * @return a new ParameterValue.
 */
ParameterValueNNPtr ParameterValue::create(const std::string &stringValueIn) {
    return ParameterValue::nn_make_shared<ParameterValue>(
        stringValueIn, ParameterValue::Type::STRING);
}

// ---------------------------------------------------------------------------

/** \brief Instanciate a ParameterValue from a filename.
 *
 * @return a new ParameterValue.
 */
ParameterValueNNPtr
ParameterValue::createFilename(const std::string &stringValueIn) {
    return ParameterValue::nn_make_shared<ParameterValue>(
        stringValueIn, ParameterValue::Type::FILENAME);
}

// ---------------------------------------------------------------------------

/** \brief Instanciate a ParameterValue from a integer value.
 *
 * @return a new ParameterValue.
 */
ParameterValueNNPtr ParameterValue::create(int integerValueIn) {
    return ParameterValue::nn_make_shared<ParameterValue>(integerValueIn);
}

// ---------------------------------------------------------------------------

/** \brief Instanciate a ParameterValue from a boolean value.
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
const ParameterValue::Type &ParameterValue::type() const { return d->type_; }

// ---------------------------------------------------------------------------

/** \brief Returns the value as a Measure (assumes type() == Type::MEASURE)
 * @return the value as a Measure.
 */
const common::Measure &ParameterValue::value() const { return d->measure_; }

// ---------------------------------------------------------------------------

/** \brief Returns the value as a string (assumes type() == Type::STRING)
 * @return the value as a string.
 */
const std::string &ParameterValue::stringValue() const {
    return d->stringValue_;
}

// ---------------------------------------------------------------------------

/** \brief Returns the value as a filename (assumes type() == Type::FILENAME)
 * @return the value as a filename.
 */
const std::string &ParameterValue::valueFile() const { return d->stringValue_; }

// ---------------------------------------------------------------------------

/** \brief Returns the value as a integer (assumes type() == Type::INTEGER)
 * @return the value as a integer.
 */
int ParameterValue::integerValue() const { return d->integerValue_; }

// ---------------------------------------------------------------------------

/** \brief Returns the value as a boolean (assumes type() == Type::BOOLEAN)
 * @return the value as a boolean.
 */
bool ParameterValue::booleanValue() const { return d->booleanValue_; }

// ---------------------------------------------------------------------------

std::string ParameterValue::exportToWKT(io::WKTFormatterNNPtr formatter) const {
    const bool isWKT2 = formatter->version() == io::WKTFormatter::Version::WKT2;

    if (formatter->abridgedTransformation() && type() == Type::MEASURE) {
        if (value().unit().type() == common::UnitOfMeasure::Type::LINEAR) {
            formatter->add(value().getSIValue());
        } else if (value().unit().type() ==
                   common::UnitOfMeasure::Type::ANGULAR) {
            formatter->add(value()
                               .convertToUnit(common::UnitOfMeasure::ARC_SECOND)
                               .value());
        } else if (value().unit() == common::UnitOfMeasure::PARTS_PER_MILLION) {
            formatter->add(1.0 + value().value() * 1e-6);
        } else {
            formatter->add(value().value());
        }
    } else if (type() == Type::MEASURE) {
        formatter->add(value().value());
        const auto &unit = value().unit();
        if (isWKT2 && unit != common::UnitOfMeasure::NONE) {
            if (!formatter->primeMeridianOrParameterUnitOmittedIfSameAsAxis() ||
                (unit != common::UnitOfMeasure::SCALE_UNITY &&
                 unit != *(formatter->axisLinearUnit()) &&
                 unit != *(formatter->axisAngularUnit()))) {
                unit.exportToWKT(formatter);
            }
        }
    } else if (type() == Type::STRING || type() == Type::FILENAME) {
        formatter->addQuotedString(stringValue());
    } else if (type() == Type::INTEGER) {
        formatter->add(integerValue());
    } else {
        throw io::FormattingException("boolean parameter value not handled");
    }
    return formatter->toString();
}

// ---------------------------------------------------------------------------

bool ParameterValue::isEquivalentTo(
    const util::BaseObjectNNPtr &other,
    util::IComparable::Criterion criterion) const {
    auto otherPV = util::nn_dynamic_pointer_cast<ParameterValue>(other);
    if (otherPV == nullptr) {
        return false;
    }
    if (type() != otherPV->type()) {
        return false;
    }
    switch (type()) {
    case Type::MEASURE: {
        return value().isEquivalentTo(otherPV->value(), criterion);
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

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
struct Conversion::Private {};
//! @endcond

// ---------------------------------------------------------------------------

Conversion::Conversion(const OperationMethodNNPtr &methodIn,
                       const std::vector<GeneralParameterValueNNPtr> &values)
    : SingleOperation(methodIn), d(internal::make_unique<Private>()) {
    setParameterValues(values);
}

// ---------------------------------------------------------------------------

Conversion::Conversion(const Conversion &other)
    : CoordinateOperation(other), SingleOperation(other),
      d(internal::make_unique<Private>(*other.d)) {}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
Conversion::~Conversion() = default;
//! @endcond

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
ConversionNNPtr Conversion::shallowClone() const {
    auto conv = Conversion::nn_make_shared<Conversion>(*this);
    conv->assignSelf(conv);
    auto l_sourceCRS = sourceCRS();
    auto l_targetCRS = targetCRS();
    if (l_sourceCRS && l_targetCRS) {
        conv->setCRSs(NN_CHECK_ASSERT(l_sourceCRS),
                      NN_CHECK_ASSERT(l_targetCRS), interpolationCRS());
    }
    return conv;
}
//! @endcond

// ---------------------------------------------------------------------------

/** \brief Instanciate a Conversion from a vector of GeneralParameterValue.
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

/** \brief Instanciate a Conversion and its OperationMethod
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
static util::PropertyMap
getUTMConversionProperty(const util::PropertyMap &properties, int zone,
                         bool north) {
    if (properties.find(common::IdentifiedObject::NAME_KEY) ==
        properties.end()) {
        std::ostringstream conversionName;
        conversionName << "UTM zone ";
        conversionName << zone;
        conversionName << (north ? 'N' : 'S');

        return util::PropertyMap(properties)
            .set(common::IdentifiedObject::NAME_KEY, conversionName.str())
            .set(metadata::Identifier::CODESPACE_KEY,
                 metadata::Identifier::EPSG)
            .set(metadata::Identifier::CODE_KEY,
                 (north ? 16000 : 17000) + zone);
    } else {
        return properties;
    }
}

// ---------------------------------------------------------------------------

static util::PropertyMap
addDefaultNameIfNeeded(const util::PropertyMap &properties,
                       const std::string &defaultName) {
    if (properties.find(common::IdentifiedObject::NAME_KEY) ==
        properties.end()) {
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
    for (const auto &param : mapping->params) {
        auto paramProperties = util::PropertyMap().set(
            common::IdentifiedObject::NAME_KEY, param.wkt2_name);
        if (param.epsg_code != 0) {
            paramProperties
                .set(metadata::Identifier::CODESPACE_KEY,
                     metadata::Identifier::EPSG)
                .set(metadata::Identifier::CODE_KEY, param.epsg_code);
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
                   const std::string &method_wkt2_name,
                   const std::vector<ParameterValueNNPtr> &values) {
    const MethodMapping *mapping = getMapping(method_wkt2_name);
    assert(mapping);
    return createConversion(properties, mapping, values);
}

// ---------------------------------------------------------------------------

/** \brief Instanciate a [Universal Transverse Mercator]
 *(https://proj4.org/operations/projections/utm.html) conversion.
 *
 * UTM is a family of conversions, of EPSG codes from 16001 to 16060 for the
 * northern hemisphere, and 17001 t 17060 for the southern hemisphere,
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
        {
            ParameterValue::create(
                common::Angle(UTM_LATITUDE_OF_NATURAL_ORIGIN)),
            ParameterValue::create(common::Angle(zone * 6.0 - 183.0)),
            ParameterValue::create(common::Scale(UTM_SCALE_FACTOR)),
            ParameterValue::create(common::Length(UTM_FALSE_EASTING)),
            ParameterValue::create(common::Length(
                north ? UTM_NORTH_FALSE_NORTHING : UTM_SOUTH_FALSE_NORTHING)),
        });
}

// ---------------------------------------------------------------------------

/** \brief Instanciate a conversion based on the [Transverse Mercator]
 *(https://proj4.org/operations/projections/tmerc.html) projection method.
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
                  {
                      ParameterValue::create(centerLat),
                      ParameterValue::create(centerLong),
                      ParameterValue::create(scale),
                      ParameterValue::create(falseEasting),
                      ParameterValue::create(falseNorthing),
                  });
}

// ---------------------------------------------------------------------------

/** \brief Instanciate a conversion based on the [Gauss Schreiber Transverse
 *Mercator]
 *(https://proj4.org/operations/projections/gstmerc.html) projection method.
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
    return create(
        properties, PROJ_WKT2_NAME_METHOD_GAUSS_SCHREIBER_TRANSVERSE_MERCATOR,
        {
            ParameterValue::create(centerLat),
            ParameterValue::create(centerLong), ParameterValue::create(scale),
            ParameterValue::create(falseEasting),
            ParameterValue::create(falseNorthing),
        });
}

// ---------------------------------------------------------------------------

/** \brief Instanciate a conversion based on the [Transverse Mercator South
 *Orientated]
 *(https://proj4.org/operations/projections/tmerc.html) projection method.
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
    return create(
        properties, EPSG_CODE_METHOD_TRANSVERSE_MERCATOR_SOUTH_ORIENTATED,
        {
            ParameterValue::create(centerLat),
            ParameterValue::create(centerLong), ParameterValue::create(scale),
            ParameterValue::create(falseEasting),
            ParameterValue::create(falseNorthing),
        });
}

// ---------------------------------------------------------------------------

/** \brief Instanciate a conversion based on the  [Two Point Equidistant]
 *(https://proj4.org/operations/projections/tpeqd.html) projection method.
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
                  {
                      ParameterValue::create(latitudeFirstPoint),
                      ParameterValue::create(longitudeFirstPoint),
                      ParameterValue::create(latitudeSecondPoint),
                      ParameterValue::create(longitudeSeconPoint),
                      ParameterValue::create(falseEasting),
                      ParameterValue::create(falseNorthing),
                  });
}

// ---------------------------------------------------------------------------

/** \brief Instanciate a conversion based on the Tunisia Mapping Grid projection
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
    return create(properties, EPSG_CODE_METHOD_TUNISIA_MAPPING_GRID,
                  {
                      ParameterValue::create(centerLat),
                      ParameterValue::create(centerLong),
                      ParameterValue::create(falseEasting),
                      ParameterValue::create(falseNorthing),
                  });
}

// ---------------------------------------------------------------------------

/** \brief Instanciate a conversion based on the [Albers Conic Equal Area]
 *(https://proj4.org/operations/projections/aea.html) projection method.
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
                  {
                      ParameterValue::create(latitudeFalseOrigin),
                      ParameterValue::create(longitudeFalseOrigin),
                      ParameterValue::create(latitudeFirstParallel),
                      ParameterValue::create(latitudeSecondParallel),
                      ParameterValue::create(eastingFalseOrigin),
                      ParameterValue::create(northingFalseOrigin),
                  });
}

// ---------------------------------------------------------------------------

/** \brief Instanciate a conversion based on the [Lambert Conic Conformal 1SP]
 *(https://proj4.org/operations/projections/lcc.html) projection method.
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
                  {
                      ParameterValue::create(centerLat),
                      ParameterValue::create(centerLong),
                      ParameterValue::create(scale),
                      ParameterValue::create(falseEasting),
                      ParameterValue::create(falseNorthing),
                  });
}

// ---------------------------------------------------------------------------

/** \brief Instanciate a conversion based on the [Lambert Conic Conformal (2SP)]
 *(https://proj4.org/operations/projections/lcc.html) projection method.
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
                  {
                      ParameterValue::create(latitudeFalseOrigin),
                      ParameterValue::create(longitudeFalseOrigin),
                      ParameterValue::create(latitudeFirstParallel),
                      ParameterValue::create(latitudeSecondParallel),
                      ParameterValue::create(eastingFalseOrigin),
                      ParameterValue::create(northingFalseOrigin),
                  });
}

// ---------------------------------------------------------------------------

/** \brief Instanciate a conversion based on the [Lambert Conic Conformal (2SP
 *Michigan)]
 *(https://proj4.org/operations/projections/lcc.html) projection method.
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
                  {
                      ParameterValue::create(latitudeFalseOrigin),
                      ParameterValue::create(longitudeFalseOrigin),
                      ParameterValue::create(latitudeFirstParallel),
                      ParameterValue::create(latitudeSecondParallel),
                      ParameterValue::create(eastingFalseOrigin),
                      ParameterValue::create(northingFalseOrigin),
                      ParameterValue::create(ellipsoidScalingFactor),
                  });
}

// ---------------------------------------------------------------------------

/** \brief Instanciate a conversion based on the [Lambert Conic Conformal (2SP
 *Belgium)]
 *(https://proj4.org/operations/projections/lcc.html) projection method.
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
                  {
                      ParameterValue::create(latitudeFalseOrigin),
                      ParameterValue::create(longitudeFalseOrigin),
                      ParameterValue::create(latitudeFirstParallel),
                      ParameterValue::create(latitudeSecondParallel),
                      ParameterValue::create(eastingFalseOrigin),
                      ParameterValue::create(northingFalseOrigin),
                  });
}

// ---------------------------------------------------------------------------

/** \brief Instanciate a conversion based on the [Modified Azimuthal
 *Equidistant]
 *(https://proj4.org/operations/projections/aeqd.html) projection method.
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
                  {
                      ParameterValue::create(latitudeNatOrigin),
                      ParameterValue::create(longitudeNatOrigin),
                      ParameterValue::create(falseEasting),
                      ParameterValue::create(falseNorthing),
                  });
}

// ---------------------------------------------------------------------------

/** \brief Instanciate a conversion based on the [Guam Projection]
 *(https://proj4.org/operations/projections/aeqd.html) projection method.
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
                  {
                      ParameterValue::create(latitudeNatOrigin),
                      ParameterValue::create(longitudeNatOrigin),
                      ParameterValue::create(falseEasting),
                      ParameterValue::create(falseNorthing),
                  });
}

// ---------------------------------------------------------------------------

/** \brief Instanciate a conversion based on the [Bonne]
 *(https://proj4.org/operations/projections/bonne.html) projection method.
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
                  {
                      ParameterValue::create(latitudeNatOrigin),
                      ParameterValue::create(longitudeNatOrigin),
                      ParameterValue::create(falseEasting),
                      ParameterValue::create(falseNorthing),
                  });
}

// ---------------------------------------------------------------------------

/** \brief Instanciate a conversion based on the [Lambert Cylindrical Equal Area
 *(Spherical)]
 *(https://proj4.org/operations/projections/cea.html) projection method.
 *
 * This method is defined as [EPSG:9834]
 * (https://www.epsg-registry.org/export.htm?gml=urn:ogc:def:method:EPSG::9834)
 *
 * \warning The PROJ cea computation code would select the ellipsoidal form if
 * a non-spherical ellipsoid is used for the base GeographicalCRS.
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
                  {
                      ParameterValue::create(latitudeFirstParallel),
                      ParameterValue::create(longitudeNatOrigin),
                      ParameterValue::create(falseEasting),
                      ParameterValue::create(falseNorthing),
                  });
}

// ---------------------------------------------------------------------------

/** \brief Instanciate a conversion based on the [Lambert Cylindrical Equal Area
 *(ellipsoidal form)]
 *(https://proj4.org/operations/projections/cea.html) projection method.
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
                  {
                      ParameterValue::create(latitudeFirstParallel),
                      ParameterValue::create(longitudeNatOrigin),
                      ParameterValue::create(falseEasting),
                      ParameterValue::create(falseNorthing),
                  });
}

// ---------------------------------------------------------------------------

/** \brief Instanciate a conversion based on the [Cassini-Soldner]
 * (https://proj4.org/operations/projections/cass.html) projection method.
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
    return create(properties, EPSG_CODE_METHOD_CASSINI_SOLDNER,
                  {
                      ParameterValue::create(centerLat),
                      ParameterValue::create(centerLong),
                      ParameterValue::create(falseEasting),
                      ParameterValue::create(falseNorthing),
                  });
}

// ---------------------------------------------------------------------------

/** \brief Instanciate a conversion based on the [Equidistant Conic]
 *(https://proj4.org/operations/projections/eqdc.html) projection method.
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
                  {
                      ParameterValue::create(centerLat),
                      ParameterValue::create(centerLong),
                      ParameterValue::create(latitudeFirstParallel),
                      ParameterValue::create(latitudeSecondParallel),
                      ParameterValue::create(falseEasting),
                      ParameterValue::create(falseNorthing),
                  });
}

// ---------------------------------------------------------------------------

/** \brief Instanciate a conversion based on the [Eckert I]
 * (https://proj4.org/operations/projections/eck1.html) projection method.
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
                  {
                      ParameterValue::create(centerLong),
                      ParameterValue::create(falseEasting),
                      ParameterValue::create(falseNorthing),
                  });
}

// ---------------------------------------------------------------------------

/** \brief Instanciate a conversion based on the [Eckert II]
 * (https://proj4.org/operations/projections/eck2.html) projection method.
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
                  {
                      ParameterValue::create(centerLong),
                      ParameterValue::create(falseEasting),
                      ParameterValue::create(falseNorthing),
                  });
}

// ---------------------------------------------------------------------------

/** \brief Instanciate a conversion based on the [Eckert III]
 * (https://proj4.org/operations/projections/eck3.html) projection method.
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
                  {
                      ParameterValue::create(centerLong),
                      ParameterValue::create(falseEasting),
                      ParameterValue::create(falseNorthing),
                  });
}

// ---------------------------------------------------------------------------

/** \brief Instanciate a conversion based on the [Eckert IV]
 * (https://proj4.org/operations/projections/eck4.html) projection method.
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
                  {
                      ParameterValue::create(centerLong),
                      ParameterValue::create(falseEasting),
                      ParameterValue::create(falseNorthing),
                  });
}

// ---------------------------------------------------------------------------

/** \brief Instanciate a conversion based on the [Eckert V]
 * (https://proj4.org/operations/projections/eck5.html) projection method.
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
                  {
                      ParameterValue::create(centerLong),
                      ParameterValue::create(falseEasting),
                      ParameterValue::create(falseNorthing),
                  });
}

// ---------------------------------------------------------------------------

/** \brief Instanciate a conversion based on the [Eckert VI]
 * (https://proj4.org/operations/projections/eck6.html) projection method.
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
                  {
                      ParameterValue::create(centerLong),
                      ParameterValue::create(falseEasting),
                      ParameterValue::create(falseNorthing),
                  });
}

// ---------------------------------------------------------------------------

/** \brief Instanciate a conversion based on the [Equidistant Cylindrical]
 *(https://proj4.org/operations/projections/eqc.html) projection method.
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
                  {
                      ParameterValue::create(latitudeFirstParallel),
                      ParameterValue::create(longitudeNatOrigin),
                      ParameterValue::create(falseEasting),
                      ParameterValue::create(falseNorthing),
                  });
}

// ---------------------------------------------------------------------------

/** \brief Instanciate a conversion based on the [Equidistant Cylindrical
 *(Spherical)]
 *(https://proj4.org/operations/projections/eqc.html) projection method.
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
                  {
                      ParameterValue::create(latitudeFirstParallel),
                      ParameterValue::create(longitudeNatOrigin),
                      ParameterValue::create(falseEasting),
                      ParameterValue::create(falseNorthing),
                  });
}

// ---------------------------------------------------------------------------

/** \brief Instanciate a conversion based on the [Gall (Stereographic)]
 * (https://proj4.org/operations/projections/gall.html) projection method.
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
    return create(properties, PROJ_WKT2_NAME_METHOD_GALL,
                  {
                      ParameterValue::create(centerLong),
                      ParameterValue::create(falseEasting),
                      ParameterValue::create(falseNorthing),
                  });
}

// ---------------------------------------------------------------------------

/** \brief Instanciate a conversion based on the [Goode Homolosine]
 * (https://proj4.org/operations/projections/goode.html) projection method.
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
                  {
                      ParameterValue::create(centerLong),
                      ParameterValue::create(falseEasting),
                      ParameterValue::create(falseNorthing),
                  });
}

// ---------------------------------------------------------------------------

/** \brief Instanciate a conversion based on the [Interrupted Goode Homolosine]
 * (https://proj4.org/operations/projections/igh.html) projection method.
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
                  {
                      ParameterValue::create(centerLong),
                      ParameterValue::create(falseEasting),
                      ParameterValue::create(falseNorthing),
                  });
}

// ---------------------------------------------------------------------------

/** \brief Instanciate a conversion based on the [Geostationary Satellite View]
 * (https://proj4.org/operations/projections/geos.html) projection method,
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
        {
            ParameterValue::create(centerLong), ParameterValue::create(height),
            ParameterValue::create(falseEasting),
            ParameterValue::create(falseNorthing),
        });
}

// ---------------------------------------------------------------------------

/** \brief Instanciate a conversion based on the [Geostationary Satellite View]
 * (https://proj4.org/operations/projections/geos.html) projection method,
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
        {
            ParameterValue::create(centerLong), ParameterValue::create(height),
            ParameterValue::create(falseEasting),
            ParameterValue::create(falseNorthing),
        });
}

// ---------------------------------------------------------------------------

/** \brief Instanciate a conversion based on the [Gnomonic]
 *(https://proj4.org/operations/projections/gnom.html) projection method.
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
    return create(properties, PROJ_WKT2_NAME_METHOD_GNOMONIC,
                  {
                      ParameterValue::create(centerLat),
                      ParameterValue::create(centerLong),
                      ParameterValue::create(falseEasting),
                      ParameterValue::create(falseNorthing),
                  });
}

// ---------------------------------------------------------------------------

/** \brief Instanciate a conversion based on the [Hotine Oblique Mercator
 *(Variant A)]
 *(https://proj4.org/operations/projections/omerc.html) projection method
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
 *(https://proj4.org/operations/projections/somerc.html) formulas.
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
        {
            ParameterValue::create(latitudeProjectionCentre),
            ParameterValue::create(longitudeProjectionCentre),
            ParameterValue::create(azimuthInitialLine),
            ParameterValue::create(angleFromRectifiedToSkrewGrid),
            ParameterValue::create(scale), ParameterValue::create(falseEasting),
            ParameterValue::create(falseNorthing),
        });
}

// ---------------------------------------------------------------------------

/** \brief Instanciate a conversion based on the [Hotine Oblique Mercator
 *(Variant B)]
 *(https://proj4.org/operations/projections/omerc.html) projection method
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
 *(https://proj4.org/operations/projections/somerc.html) formulas.
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
    return create(properties,
                  EPSG_CODE_METHOD_HOTINE_OBLIQUE_MERCATOR_VARIANT_B,
                  {
                      ParameterValue::create(latitudeProjectionCentre),
                      ParameterValue::create(longitudeProjectionCentre),
                      ParameterValue::create(azimuthInitialLine),
                      ParameterValue::create(angleFromRectifiedToSkrewGrid),
                      ParameterValue::create(scale),
                      ParameterValue::create(eastingProjectionCentre),
                      ParameterValue::create(northingProjectionCentre),
                  });
}

// ---------------------------------------------------------------------------

/** \brief Instanciate a conversion based on the [Hotine Oblique Mercator Two
 *Point Natural Origin]
 *(https://proj4.org/operations/projections/omerc.html) projection method.
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

/** \brief Instanciate a conversion based on the [International Map of the World
 *Polyconic]
 *(https://proj4.org/operations/projections/imw_p.html) projection method.
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
                  {
                      ParameterValue::create(centerLong),
                      ParameterValue::create(latitudeFirstParallel),
                      ParameterValue::create(latitudeSecondParallel),
                      ParameterValue::create(falseEasting),
                      ParameterValue::create(falseNorthing),
                  });
}

// ---------------------------------------------------------------------------

/** \brief Instanciate a conversion based on the [Krovak (north oriented)]
 *(https://proj4.org/operations/projections/krovak.html) projection method.
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
                  {
                      ParameterValue::create(latitudeProjectionCentre),
                      ParameterValue::create(longitudeOfOrigin),
                      ParameterValue::create(colatitudeConeAxis),
                      ParameterValue::create(latitudePseudoStandardParallel),
                      ParameterValue::create(scaleFactorPseudoStandardParallel),
                      ParameterValue::create(falseEasting),
                      ParameterValue::create(falseNorthing),
                  });
}

// ---------------------------------------------------------------------------

/** \brief Instanciate a conversion based on the [Krovak]
 *(https://proj4.org/operations/projections/krovak.html) projection method.
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
                  {
                      ParameterValue::create(latitudeProjectionCentre),
                      ParameterValue::create(longitudeOfOrigin),
                      ParameterValue::create(colatitudeConeAxis),
                      ParameterValue::create(latitudePseudoStandardParallel),
                      ParameterValue::create(scaleFactorPseudoStandardParallel),
                      ParameterValue::create(falseEasting),
                      ParameterValue::create(falseNorthing),
                  });
}

// ---------------------------------------------------------------------------

/** \brief Instanciate a conversion based on the [Lambert Azimuthal Equal Area]
 *(https://proj4.org/operations/projections/laea.html) projection method.
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
                  {
                      ParameterValue::create(latitudeNatOrigin),
                      ParameterValue::create(longitudeNatOrigin),
                      ParameterValue::create(falseEasting),
                      ParameterValue::create(falseNorthing),
                  });
}

// ---------------------------------------------------------------------------

/** \brief Instanciate a conversion based on the [Miller Cylindrical]
 *(https://proj4.org/operations/projections/mill.html) projection method.
 *
 * There is no equivalent in EPSG. *
 * @param properties See \ref general_properties of the conversion. If the name
 * is not provided, it is automatically set.
 * @param centerLat See \ref center_latitude
 * @param centerLong See \ref center_longitude
 * @param falseEasting See \ref false_easting
 * @param falseNorthing See \ref false_northing
 * @return a new Conversion.
 */
ConversionNNPtr Conversion::createMillerCylindrical(
    const util::PropertyMap &properties, const common::Angle &centerLat,
    const common::Angle &centerLong, const common::Length &falseEasting,
    const common::Length &falseNorthing) {
    return create(properties, PROJ_WKT2_NAME_METHOD_MILLER_CYLINDRICAL,
                  {
                      ParameterValue::create(centerLat),
                      ParameterValue::create(centerLong),
                      ParameterValue::create(falseEasting),
                      ParameterValue::create(falseNorthing),
                  });
}

// ---------------------------------------------------------------------------

/** \brief Instanciate a conversion based on the [Mercator]
 *(https://proj4.org/operations/projections/merc.html) projection method.
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
                  {
                      ParameterValue::create(centerLat),
                      ParameterValue::create(centerLong),
                      ParameterValue::create(scale),
                      ParameterValue::create(falseEasting),
                      ParameterValue::create(falseNorthing),
                  });
}

// ---------------------------------------------------------------------------

/** \brief Instanciate a conversion based on the [Mercator]
 *(https://proj4.org/operations/projections/merc.html) projection method.
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
                  {
                      ParameterValue::create(latitudeFirstParallel),
                      ParameterValue::create(centerLong),
                      ParameterValue::create(falseEasting),
                      ParameterValue::create(falseNorthing),
                  });
}

// ---------------------------------------------------------------------------

/** \brief Instanciate a conversion based on the [Popular Visualisation Pseudo
 *Mercator]
 *(https://proj4.org/operations/projections/webmerc.html) projection method.
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
    return create(properties,
                  EPSG_CODE_METHOD_POPULAR_VISUALISATION_PSEUDO_MERCATOR,
                  {
                      ParameterValue::create(centerLat),
                      ParameterValue::create(centerLong),
                      ParameterValue::create(falseEasting),
                      ParameterValue::create(falseNorthing),
                  });
}

// ---------------------------------------------------------------------------

/** \brief Instanciate a conversion based on the [Mollweide]
 * (https://proj4.org/operations/projections/moll.html) projection method.
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
                  {
                      ParameterValue::create(centerLong),
                      ParameterValue::create(falseEasting),
                      ParameterValue::create(falseNorthing),
                  });
}

// ---------------------------------------------------------------------------

/** \brief Instanciate a conversion based on the [New Zealand Map Grid]
 * (https://proj4.org/operations/projections/nzmg.html) projection method.
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
    return create(properties, EPSG_CODE_METHOD_NZMG,
                  {
                      ParameterValue::create(centerLat),
                      ParameterValue::create(centerLong),
                      ParameterValue::create(falseEasting),
                      ParameterValue::create(falseNorthing),
                  });
}

// ---------------------------------------------------------------------------

/** \brief Instanciate a conversion based on the [Oblique Stereographic
 *(Alternative)]
 *(https://proj4.org/operations/projections/sterea.html) projection method.
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
                  {
                      ParameterValue::create(centerLat),
                      ParameterValue::create(centerLong),
                      ParameterValue::create(scale),
                      ParameterValue::create(falseEasting),
                      ParameterValue::create(falseNorthing),
                  });
}

// ---------------------------------------------------------------------------

/** \brief Instanciate a conversion based on the [Orthographic]
 *(https://proj4.org/operations/projections/ortho.html) projection method.
 *
 * This method is defined as [EPSG:9840]
 * (https://www.epsg-registry.org/export.htm?gml=urn:ogc:def:method:EPSG::9840)
 *
 * \note At the time of writing, PROJ only implements the spherical formulation
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
    return create(properties, EPSG_CODE_METHOD_ORTHOGRAPHIC,
                  {
                      ParameterValue::create(centerLat),
                      ParameterValue::create(centerLong),
                      ParameterValue::create(falseEasting),
                      ParameterValue::create(falseNorthing),
                  });
}

// ---------------------------------------------------------------------------

/** \brief Instanciate a conversion based on the [American Polyconic]
 *(https://proj4.org/operations/projections/poly.html) projection method.
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
    return create(properties, EPSG_CODE_METHOD_AMERICAN_POLYCONIC,
                  {
                      ParameterValue::create(centerLat),
                      ParameterValue::create(centerLong),
                      ParameterValue::create(falseEasting),
                      ParameterValue::create(falseNorthing),
                  });
}

// ---------------------------------------------------------------------------

/** \brief Instanciate a conversion based on the [Polar Stereographic (Variant
 *A)]
 *(https://proj4.org/operations/projections/stere.html) projection method.
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
                  {
                      ParameterValue::create(centerLat),
                      ParameterValue::create(centerLong),
                      ParameterValue::create(scale),
                      ParameterValue::create(falseEasting),
                      ParameterValue::create(falseNorthing),
                  });
}

// ---------------------------------------------------------------------------

/** \brief Instanciate a conversion based on the [Polar Stereographic (Variant
 *B)]
 *(https://proj4.org/operations/projections/stere.html) projection method.
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
                  {
                      ParameterValue::create(latitudeStandardParallel),
                      ParameterValue::create(longitudeOfOrigin),
                      ParameterValue::create(falseEasting),
                      ParameterValue::create(falseNorthing),
                  });
}

// ---------------------------------------------------------------------------

/** \brief Instanciate a conversion based on the [Robinson]
 * (https://proj4.org/operations/projections/robin.html) projection method.
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
                  {
                      ParameterValue::create(centerLong),
                      ParameterValue::create(falseEasting),
                      ParameterValue::create(falseNorthing),
                  });
}

// ---------------------------------------------------------------------------

/** \brief Instanciate a conversion based on the [Sinusoidal]
 * (https://proj4.org/operations/projections/sinu.html) projection method.
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
                  {
                      ParameterValue::create(centerLong),
                      ParameterValue::create(falseEasting),
                      ParameterValue::create(falseNorthing),
                  });
}

// ---------------------------------------------------------------------------

/** \brief Instanciate a conversion based on the [Stereographic]
 *(https://proj4.org/operations/projections/stere.html) projection method.
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
                  {
                      ParameterValue::create(centerLat),
                      ParameterValue::create(centerLong),
                      ParameterValue::create(scale),
                      ParameterValue::create(falseEasting),
                      ParameterValue::create(falseNorthing),
                  });
}

// ---------------------------------------------------------------------------

/** \brief Instanciate a conversion based on the [Van der Grinten]
 * (https://proj4.org/operations/projections/vandg.html) projection method.
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
                  {
                      ParameterValue::create(centerLong),
                      ParameterValue::create(falseEasting),
                      ParameterValue::create(falseNorthing),
                  });
}

// ---------------------------------------------------------------------------

/** \brief Instanciate a conversion based on the [Wagner I]
 * (https://proj4.org/operations/projections/wag1.html) projection method.
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
                  {
                      ParameterValue::create(centerLong),
                      ParameterValue::create(falseEasting),
                      ParameterValue::create(falseNorthing),
                  });
}

// ---------------------------------------------------------------------------

/** \brief Instanciate a conversion based on the [Wagner II]
 * (https://proj4.org/operations/projections/wag2.html) projection method.
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
                  {
                      ParameterValue::create(centerLong),
                      ParameterValue::create(falseEasting),
                      ParameterValue::create(falseNorthing),
                  });
}

// ---------------------------------------------------------------------------

/** \brief Instanciate a conversion based on the [Wagner III]
 * (https://proj4.org/operations/projections/wag3.html) projection method.
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
                  {
                      ParameterValue::create(latitudeTrueScale),
                      ParameterValue::create(centerLong),
                      ParameterValue::create(falseEasting),
                      ParameterValue::create(falseNorthing),
                  });
}

// ---------------------------------------------------------------------------

/** \brief Instanciate a conversion based on the [Wagner IV]
 * (https://proj4.org/operations/projections/wag4.html) projection method.
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
                  {
                      ParameterValue::create(centerLong),
                      ParameterValue::create(falseEasting),
                      ParameterValue::create(falseNorthing),
                  });
}

// ---------------------------------------------------------------------------

/** \brief Instanciate a conversion based on the [Wagner V]
 * (https://proj4.org/operations/projections/wag5.html) projection method.
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
                  {
                      ParameterValue::create(centerLong),
                      ParameterValue::create(falseEasting),
                      ParameterValue::create(falseNorthing),
                  });
}

// ---------------------------------------------------------------------------

/** \brief Instanciate a conversion based on the [Wagner VI]
 * (https://proj4.org/operations/projections/wag6.html) projection method.
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
                  {
                      ParameterValue::create(centerLong),
                      ParameterValue::create(falseEasting),
                      ParameterValue::create(falseNorthing),
                  });
}

// ---------------------------------------------------------------------------

/** \brief Instanciate a conversion based on the [Wagner VII]
 * (https://proj4.org/operations/projections/wag7.html) projection method.
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
                  {
                      ParameterValue::create(centerLong),
                      ParameterValue::create(falseEasting),
                      ParameterValue::create(falseNorthing),
                  });
}

// ---------------------------------------------------------------------------

/** \brief Instanciate a conversion based on the [Quadrilateralized Spherical
 *Cube]
 *(https://proj4.org/operations/projections/qsc.html) projection method.
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
    return create(properties,
                  PROJ_WKT2_NAME_METHOD_QUADRILATERALIZED_SPHERICAL_CUBE,
                  {
                      ParameterValue::create(centerLat),
                      ParameterValue::create(centerLong),
                      ParameterValue::create(falseEasting),
                      ParameterValue::create(falseNorthing),
                  });
}

// ---------------------------------------------------------------------------

/** \brief Instanciate a conversion based on the [Spherical Cross-Track Height]
 *(https://proj4.org/operations/projections/sch.html) projection method.
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
                  {
                      ParameterValue::create(pegPointLat),
                      ParameterValue::create(pegPointLong),
                      ParameterValue::create(pegPointHeading),
                      ParameterValue::create(pegPointHeight),
                  });
}

// ---------------------------------------------------------------------------

/** \brief Instanciate a conversion based on the [Equal Earth]
 * (https://proj4.org/operations/projections/eqearth.html) projection method.
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
                  {
                      ParameterValue::create(centerLong),
                      ParameterValue::create(falseEasting),
                      ParameterValue::create(falseNorthing),
                  });
}

// ---------------------------------------------------------------------------

/** \brief Instanciate a conversion based on the Change of Vertical Unit
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
    return create(properties, util::PropertyMap()
                                  .set(common::IdentifiedObject::NAME_KEY,
                                       EPSG_NAME_METHOD_CHANGE_VERTICAL_UNIT)
                                  .set(metadata::Identifier::CODESPACE_KEY,
                                       metadata::Identifier::EPSG)
                                  .set(metadata::Identifier::CODE_KEY,
                                       EPSG_CODE_METHOD_CHANGE_VERTICAL_UNIT),
                  {
                      OperationParameter::create(
                          util::PropertyMap()
                              .set(common::IdentifiedObject::NAME_KEY,
                                   EPSG_NAME_PARAMETER_UNIT_CONVERSION_SCALAR)
                              .set(metadata::Identifier::CODESPACE_KEY,
                                   metadata::Identifier::EPSG)
                              .set(metadata::Identifier::CODE_KEY,
                                   EPSG_CODE_PARAMETER_UNIT_CONVERSION_SCALAR)),
                  },
                  {
                      ParameterValue::create(factor),
                  });
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress

static util::PropertyMap
createPropertiesForInverse(const OperationMethodNNPtr &method) {
    util::PropertyMap map;

    std::string forwardName = *method->name()->description();
    std::string name;
    if (!forwardName.empty()) {
        if (starts_with(forwardName, INVERSE_OF)) {
            name = forwardName.substr(INVERSE_OF.size());
        } else {
            name = INVERSE_OF + forwardName;
        }
    }

    if (!name.empty()) {
        map.set(common::IdentifiedObject::NAME_KEY, name);
    }

    // If original operation is AUTH:CODE, then assign INVERSE(AUTH):CODE
    // as identifier.
    {
        auto ar = util::ArrayOfBaseObject::create();
        for (const auto &idSrc : method->identifiers()) {
            auto authName = *(idSrc->codeSpace());
            auto srcCode = idSrc->code();
            if (starts_with(authName, "INVERSE(") && authName.back() == ')') {
                authName = authName.substr(strlen("INVERSE("));
                authName.resize(authName.size() - 1);
            } else {
                authName = "INVERSE(" + authName + ")";
            }
            auto idsProp = util::PropertyMap().set(
                metadata::Identifier::CODESPACE_KEY, authName);
            ar->add(metadata::Identifier::create(srcCode, idsProp));
        }
        if (!ar->empty()) {
            map.set(common::IdentifiedObject::IDENTIFIERS_KEY, ar);
        }
    }

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
    return NN_CHECK_ASSERT(
        util::nn_dynamic_pointer_cast<Conversion>(forwardOperation_));
}

// ---------------------------------------------------------------------------

CoordinateOperationNNPtr
InverseConversion::create(const ConversionNNPtr &forward) {
    auto conv = util::nn_make_shared<InverseConversion>(forward);
    conv->assignSelf(conv);
    return conv;
}

//! @endcond

// ---------------------------------------------------------------------------

CoordinateOperationNNPtr Conversion::inverse() const {
    auto projectionMethodName = *(method()->name()->description());
    const int methodEPSGCode = method()->getEPSGCode();

    if (ci_equal(projectionMethodName, EPSG_NAME_METHOD_CHANGE_VERTICAL_UNIT) ||
        methodEPSGCode == EPSG_CODE_METHOD_CHANGE_VERTICAL_UNIT) {
        const double convFactor =
            parameterValueMeasure(EPSG_NAME_PARAMETER_UNIT_CONVERSION_SCALAR,
                                  EPSG_CODE_PARAMETER_UNIT_CONVERSION_SCALAR)
                .getSIValue();

        auto conv = createChangeVerticalUnit(
            createPropertiesForInverse(this, false, false),
            common::Scale(1.0 / convFactor));
        auto l_sourceCRS = sourceCRS();
        auto l_targetCRS = targetCRS();
        if (l_sourceCRS && l_targetCRS) {
            conv->setCRSs(NN_CHECK_ASSERT(l_targetCRS),
                          NN_CHECK_ASSERT(l_sourceCRS), nullptr);
        }
        return conv;
    }

    return InverseConversion::create(NN_CHECK_ASSERT(
        util::nn_dynamic_pointer_cast<Conversion>(shared_from_this())));
}

// ---------------------------------------------------------------------------

std::string Conversion::exportToWKT(io::WKTFormatterNNPtr formatter) const {
    const bool isWKT2 = formatter->version() == io::WKTFormatter::Version::WKT2;
    if (isWKT2) {
        formatter->startNode(formatter->useDerivingConversion()
                                 ? io::WKTConstants::DERIVINGCONVERSION
                                 : io::WKTConstants::CONVERSION,
                             !identifiers().empty());
        formatter->addQuotedString(*(name()->description()));
    } else {
        formatter->enter();
        formatter->pushOutputUnit(false);
        formatter->pushOutputId(false);
    }

    bool bAlreadyWritten = false;
    if (!isWKT2) {
        auto projectionMethodName = *(method()->name()->description());
        const int methodEPSGCode = method()->getEPSGCode();
        if (ci_equal(projectionMethodName,
                     EPSG_NAME_METHOD_POPULAR_VISUALISATION_PSEUDO_MERCATOR) ||
            methodEPSGCode ==
                EPSG_CODE_METHOD_POPULAR_VISUALISATION_PSEUDO_MERCATOR) {
            const double latitudeOrigin =
                parameterValueMeasure(
                    EPSG_NAME_PARAMETER_LATITUDE_OF_NATURAL_ORIGIN,
                    EPSG_CODE_PARAMETER_LATITUDE_OF_NATURAL_ORIGIN)
                    .convertToUnit(common::UnitOfMeasure::DEGREE)
                    .value();
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
            const double centralMeridian =
                parameterValueMeasure(
                    EPSG_NAME_PARAMETER_LONGITUDE_OF_NATURAL_ORIGIN,
                    EPSG_CODE_PARAMETER_LONGITUDE_OF_NATURAL_ORIGIN)
                    .convertToUnit(common::UnitOfMeasure::DEGREE)
                    .value();
            formatter->add(centralMeridian);
            formatter->endNode();

            formatter->startNode(io::WKTConstants::PARAMETER, false);
            formatter->addQuotedString("scale_factor");
            formatter->add(1.0);
            formatter->endNode();

            formatter->startNode(io::WKTConstants::PARAMETER, false);
            formatter->addQuotedString("false_easting");
            const double falseEasting =
                parameterValueMeasure(EPSG_NAME_PARAMETER_FALSE_EASTING,
                                      EPSG_CODE_PARAMETER_FALSE_EASTING)
                    .getSIValue();
            formatter->add(falseEasting);
            formatter->endNode();

            formatter->startNode(io::WKTConstants::PARAMETER, false);
            formatter->addQuotedString("false_northing");
            const double falseNorthing =
                parameterValueMeasure(EPSG_NAME_PARAMETER_FALSE_NORTHING,
                                      EPSG_CODE_PARAMETER_FALSE_NORTHING)
                    .getSIValue();
            formatter->add(falseNorthing);
            formatter->endNode();
        } else if (starts_with(projectionMethodName, "PROJ ")) {
            bAlreadyWritten = true;
            formatter->startNode(io::WKTConstants::PROJECTION, false);
            formatter->addQuotedString("custom_proj4");
            formatter->endNode();
        }
    }

    if (!bAlreadyWritten) {
        method()->exportToWKT(formatter);

        const MethodMapping *mapping =
            !isWKT2 ? getMapping(method().get()) : nullptr;
        for (const auto &paramValue : parameterValues()) {
            paramValue->_exportToWKT(formatter, mapping);
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
    return formatter->toString();
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
static bool createPROJ4WebMercator(const Conversion *conv,
                                   io::PROJStringFormatterNNPtr formatter) {
    const double centralMeridian =
        conv->parameterValueMeasure(
                EPSG_NAME_PARAMETER_LONGITUDE_OF_NATURAL_ORIGIN,
                EPSG_CODE_PARAMETER_LONGITUDE_OF_NATURAL_ORIGIN)
            .convertToUnit(common::UnitOfMeasure::DEGREE)
            .value();

    const double falseEasting =
        conv->parameterValueMeasure(EPSG_NAME_PARAMETER_FALSE_EASTING,
                                    EPSG_CODE_PARAMETER_FALSE_EASTING)
            .getSIValue();

    const double falseNorthing =
        conv->parameterValueMeasure(EPSG_NAME_PARAMETER_FALSE_NORTHING,
                                    EPSG_CODE_PARAMETER_FALSE_NORTHING)
            .getSIValue();

    auto geogCRS =
        std::dynamic_pointer_cast<crs::GeographicCRS>(conv->sourceCRS());
    if (!geogCRS) {
        return false;
    }
    io::PROJStringFormatter::Scope scope(formatter);
    formatter->addStep("merc");
    const double a = geogCRS->ellipsoid()->semiMajorAxis().getSIValue();
    formatter->addParam("a", a);
    formatter->addParam("b", a);
    formatter->addParam("lat_ts", 0.0);
    formatter->addParam("lon_0", centralMeridian);
    formatter->addParam("x_0", falseEasting);
    formatter->addParam("y_0", falseNorthing);
    formatter->addParam("k", 1.0);
    formatter->addParam("units", "m");
    formatter->addParam("nadgrids", "@null");
    formatter->addParam("wktext");
    formatter->addParam("no_defs");
    return true;
}

// ---------------------------------------------------------------------------

static bool
createPROJExtensionFromCustomProj(const Conversion *conv,
                                  io::PROJStringFormatterNNPtr formatter,
                                  bool forExtensionNode) {
    auto projectionMethodName = *(conv->method()->name()->description());
    assert(starts_with(projectionMethodName, "PROJ "));
    auto tokens = split(projectionMethodName, ' ');

    io::PROJStringFormatter::Scope scope(formatter);
    formatter->addStep(tokens[1]);

    if (forExtensionNode) {
        auto geogCRS =
            std::dynamic_pointer_cast<crs::GeographicCRS>(conv->sourceCRS());
        if (!geogCRS) {
            return false;
        }
        geogCRS->addDatumInfoToPROJString(formatter);
    }

    for (size_t i = 2; i < tokens.size(); i++) {
        formatter->addParam(tokens[i]);
    }

    for (const auto &genOpParamvalue : conv->parameterValues()) {
        const auto &opParamvalue =
            util::nn_dynamic_pointer_cast<OperationParameterValue>(
                genOpParamvalue);
        if (opParamvalue) {
            const auto &paramName =
                *(opParamvalue->parameter()->name()->description());
            const auto &paramValue = opParamvalue->parameterValue();
            if (paramValue->type() == ParameterValue::Type::MEASURE) {
                auto &measure = paramValue->value();
                if (measure.unit().type() ==
                    common::UnitOfMeasure::Type::LINEAR) {
                    formatter->addParam(paramName, measure.getSIValue());
                } else if (measure.unit().type() ==
                           common::UnitOfMeasure::Type::ANGULAR) {
                    formatter->addParam(
                        paramName,
                        measure.convertToUnit(common::UnitOfMeasure::DEGREE)
                            .value());
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

void Conversion::addWKTExtensionNode(io::WKTFormatterNNPtr formatter) const {
    const bool isWKT2 = formatter->version() == io::WKTFormatter::Version::WKT2;
    if (!isWKT2) {
        auto projectionMethodName = *(method()->name()->description());
        const int methodEPSGCode = method()->getEPSGCode();
        if (ci_equal(projectionMethodName,
                     EPSG_NAME_METHOD_POPULAR_VISUALISATION_PSEUDO_MERCATOR) ||
            methodEPSGCode ==
                EPSG_CODE_METHOD_POPULAR_VISUALISATION_PSEUDO_MERCATOR ||
            *(name()->description()) == "Popular Visualisation Mercator") {

            auto projFormatter = io::PROJStringFormatter::create(
                io::PROJStringFormatter::Convention::PROJ_4);
            if (createPROJ4WebMercator(this, projFormatter)) {
                formatter->startNode(io::WKTConstants::EXTENSION, false);
                formatter->addQuotedString("PROJ4");
                formatter->addQuotedString(projFormatter->toString());
                formatter->endNode();
            }
        } else if (starts_with(projectionMethodName, "PROJ ")) {
            auto projFormatter = io::PROJStringFormatter::create(
                io::PROJStringFormatter::Convention::PROJ_4);
            if (createPROJExtensionFromCustomProj(this, projFormatter, true)) {
                formatter->startNode(io::WKTConstants::EXTENSION, false);
                formatter->addQuotedString("PROJ4");
                formatter->addQuotedString(projFormatter->toString());
                formatter->endNode();
            }
        }
    }
}

// ---------------------------------------------------------------------------

std::string Conversion::exportToPROJString(
    io::PROJStringFormatterNNPtr formatter) const // throw(FormattingException)
{
    io::PROJStringFormatter::Scope scope(formatter);

    auto projectionMethodName = *(method()->name()->description());
    const int methodEPSGCode = method()->getEPSGCode();
    const bool isZUnitConversion =
        ci_equal(projectionMethodName, EPSG_NAME_METHOD_CHANGE_VERTICAL_UNIT) ||
        methodEPSGCode == EPSG_CODE_METHOD_CHANGE_VERTICAL_UNIT;

    auto l_sourceCRS = sourceCRS();
    if (l_sourceCRS && !isZUnitConversion &&
        formatter->convention() ==
            io::PROJStringFormatter::Convention::PROJ_5) {
        auto geogCRS =
            std::dynamic_pointer_cast<crs::GeographicCRS>(l_sourceCRS);
        if (geogCRS) {
            formatter->setOmitProjLongLatIfPossible(true);
            formatter->startInversion();
            geogCRS->exportToPROJString(formatter);
            formatter->stopInversion();
            formatter->setOmitProjLongLatIfPossible(false);
        }

        auto projCRS =
            std::dynamic_pointer_cast<crs::ProjectedCRS>(l_sourceCRS);
        if (projCRS) {
            formatter->startInversion();
            projCRS->addUnitConvertAndAxisSwap(formatter, false);
            formatter->stopInversion();
        }
    }

    auto convName = *(name()->description());
    bool bConversionDone = false;
    bool bEllipsoidParametersDone = false;
    bool useETMerc = false;
    if (ci_equal(projectionMethodName, EPSG_NAME_METHOD_TRANSVERSE_MERCATOR) ||
        methodEPSGCode == EPSG_CODE_METHOD_TRANSVERSE_MERCATOR) {
        // Check for UTM
        int zone = 0;
        bool north = true;
        if (isUTM(zone, north)) {
            bConversionDone = true;
            formatter->addStep("utm");
            formatter->addParam("zone", zone);
            if (!north) {
                formatter->addParam("south");
            }
        } else {
            useETMerc = formatter->getUseETMercForTMerc();
        }
    } else if (ci_equal(projectionMethodName,
                        EPSG_NAME_METHOD_HOTINE_OBLIQUE_MERCATOR_VARIANT_A) ||
               methodEPSGCode ==
                   EPSG_CODE_METHOD_HOTINE_OBLIQUE_MERCATOR_VARIANT_A) {
        const double azimuth =
            parameterValueMeasure(EPSG_NAME_PARAMETER_AZIMUTH_INITIAL_LINE,
                                  EPSG_CODE_PARAMETER_AZIMUTH_INITIAL_LINE)
                .convertToUnit(common::UnitOfMeasure::DEGREE)
                .value();
        const double angleRectifiedToSkewGrid =
            parameterValueMeasure(
                EPSG_NAME_PARAMETER_ANGLE_RECTIFIED_TO_SKEW_GRID,
                EPSG_CODE_PARAMETER_ANGLE_RECTIFIED_TO_SKEW_GRID)
                .convertToUnit(common::UnitOfMeasure::DEGREE)
                .value();
        // Map to Swiss Oblique Mercator / somerc
        if (std::fabs(azimuth - 90) < 1e-4 &&
            std::fabs(angleRectifiedToSkewGrid - 90) < 1e-4) {
            bConversionDone = true;
            formatter->addStep("somerc");
            formatter->addParam(
                "lat_0", parameterValueMeasure(
                             EPSG_NAME_PARAMETER_LATITUDE_PROJECTION_CENTRE,
                             EPSG_CODE_PARAMETER_LATITUDE_PROJECTION_CENTRE)
                             .convertToUnit(common::UnitOfMeasure::DEGREE)
                             .value());
            formatter->addParam(
                "lon_0", parameterValueMeasure(
                             EPSG_NAME_PARAMETER_LONGITUDE_PROJECTION_CENTRE,
                             EPSG_CODE_PARAMETER_LONGITUDE_PROJECTION_CENTRE)
                             .convertToUnit(common::UnitOfMeasure::DEGREE)
                             .value());
            formatter->addParam(
                "k_0", parameterValueMeasure(
                           EPSG_NAME_PARAMETER_SCALE_FACTOR_INITIAL_LINE,
                           EPSG_CODE_PARAMETER_SCALE_FACTOR_INITIAL_LINE)
                           .getSIValue());
            formatter->addParam(
                "x_0", parameterValueMeasure(EPSG_NAME_PARAMETER_FALSE_EASTING,
                                             EPSG_CODE_PARAMETER_FALSE_EASTING)
                           .getSIValue());
            formatter->addParam(
                "y_0", parameterValueMeasure(EPSG_NAME_PARAMETER_FALSE_NORTHING,
                                             EPSG_CODE_PARAMETER_FALSE_NORTHING)
                           .getSIValue());
        }
    } else if (ci_equal(projectionMethodName,
                        EPSG_NAME_METHOD_HOTINE_OBLIQUE_MERCATOR_VARIANT_B) ||
               methodEPSGCode ==
                   EPSG_CODE_METHOD_HOTINE_OBLIQUE_MERCATOR_VARIANT_B) {
        const double azimuth =
            parameterValueMeasure(EPSG_NAME_PARAMETER_AZIMUTH_INITIAL_LINE,
                                  EPSG_CODE_PARAMETER_AZIMUTH_INITIAL_LINE)
                .convertToUnit(common::UnitOfMeasure::DEGREE)
                .value();
        const double angleRectifiedToSkewGrid =
            parameterValueMeasure(
                EPSG_NAME_PARAMETER_ANGLE_RECTIFIED_TO_SKEW_GRID,
                EPSG_CODE_PARAMETER_ANGLE_RECTIFIED_TO_SKEW_GRID)
                .convertToUnit(common::UnitOfMeasure::DEGREE)
                .value();
        // Map to Swiss Oblique Mercator / somerc
        if (std::fabs(azimuth - 90) < 1e-4 &&
            std::fabs(angleRectifiedToSkewGrid - 90) < 1e-4) {
            bConversionDone = true;
            formatter->addStep("somerc");
            formatter->addParam(
                "lat_0", parameterValueMeasure(
                             EPSG_NAME_PARAMETER_LATITUDE_PROJECTION_CENTRE,
                             EPSG_CODE_PARAMETER_LATITUDE_PROJECTION_CENTRE)
                             .convertToUnit(common::UnitOfMeasure::DEGREE)
                             .value());
            formatter->addParam(
                "lon_0", parameterValueMeasure(
                             EPSG_NAME_PARAMETER_LONGITUDE_PROJECTION_CENTRE,
                             EPSG_CODE_PARAMETER_LONGITUDE_PROJECTION_CENTRE)
                             .convertToUnit(common::UnitOfMeasure::DEGREE)
                             .value());
            formatter->addParam(
                "k_0", parameterValueMeasure(
                           EPSG_NAME_PARAMETER_SCALE_FACTOR_INITIAL_LINE,
                           EPSG_CODE_PARAMETER_SCALE_FACTOR_INITIAL_LINE)
                           .getSIValue());
            formatter->addParam(
                "x_0", parameterValueMeasure(
                           EPSG_NAME_PARAMETER_EASTING_PROJECTION_CENTRE,
                           EPSG_CODE_PARAMETER_EASTING_PROJECTION_CENTRE)
                           .getSIValue());
            formatter->addParam(
                "y_0", parameterValueMeasure(
                           EPSG_NAME_PARAMETER_NORTHING_PROJECTION_CENTRE,
                           EPSG_CODE_PARAMETER_NORTHING_PROJECTION_CENTRE)
                           .getSIValue());
        }
    } else if (ci_equal(projectionMethodName,
                        EPSG_NAME_METHOD_KROVAK_NORTH_ORIENTED) ||
               methodEPSGCode == EPSG_CODE_METHOD_KROVAK_NORTH_ORIENTED) {
        double colatitude =
            parameterValueMeasure(EPSG_NAME_PARAMETER_COLATITUDE_CONE_AXIS,
                                  EPSG_CODE_PARAMETER_COLATITUDE_CONE_AXIS)
                .convertToUnit(common::UnitOfMeasure::DEGREE)
                .value();
        double latitudePseudoStandardParallel =
            parameterValueMeasure(
                EPSG_NAME_PARAMETER_LATITUDE_PSEUDO_STANDARD_PARALLEL,
                EPSG_CODE_PARAMETER_LATITUDE_PSEUDO_STANDARD_PARALLEL)
                .convertToUnit(common::UnitOfMeasure::DEGREE)
                .value();
        if (std::fabs(colatitude - 30.28813972222222) > 1e-8) {
            throw io::FormattingException(
                std::string("Unsupported value for ") +
                EPSG_NAME_PARAMETER_COLATITUDE_CONE_AXIS);
        }
        if (std::fabs(latitudePseudoStandardParallel - 78.5) > 1e-8) {
            throw io::FormattingException(
                std::string("Unsupported value for ") +
                EPSG_NAME_PARAMETER_LATITUDE_PSEUDO_STANDARD_PARALLEL);
        }
    } else if (ci_equal(projectionMethodName,
                        EPSG_NAME_METHOD_MERCATOR_VARIANT_A) ||
               methodEPSGCode == EPSG_CODE_METHOD_MERCATOR_VARIANT_A) {
        double latitudeOrigin =
            parameterValueMeasure(
                EPSG_NAME_PARAMETER_LATITUDE_OF_NATURAL_ORIGIN,
                EPSG_CODE_PARAMETER_LATITUDE_OF_NATURAL_ORIGIN)
                .convertToUnit(common::UnitOfMeasure::DEGREE)
                .value();
        if (latitudeOrigin != 0) {
            throw io::FormattingException(
                std::string("Unsupported value for ") +
                EPSG_NAME_PARAMETER_LATITUDE_OF_NATURAL_ORIGIN);
        }
        // PROJ.4 specific hack for webmercator
    } else if (formatter->convention() ==
                   io::PROJStringFormatter::Convention::PROJ_4 &&
               (ci_equal(
                    projectionMethodName,
                    EPSG_NAME_METHOD_POPULAR_VISUALISATION_PSEUDO_MERCATOR) ||
                methodEPSGCode ==
                    EPSG_CODE_METHOD_POPULAR_VISUALISATION_PSEUDO_MERCATOR)) {
        if (!createPROJ4WebMercator(this, formatter)) {
            throw io::FormattingException(
                "Cannot export " +
                EPSG_NAME_METHOD_POPULAR_VISUALISATION_PSEUDO_MERCATOR +
                " as PROJ.4 string outside of a ProjectedCRS context");
        }
        bConversionDone = true;
        bEllipsoidParametersDone = true;
    } else if (ci_equal(convName, "Popular Visualisation Mercator")) {
        if (formatter->convention() ==
            io::PROJStringFormatter::Convention::PROJ_4) {
            if (!createPROJ4WebMercator(this, formatter)) {
                throw io::FormattingException(
                    "Cannot export " + convName +
                    " as PROJ.4 string outside of a ProjectedCRS context");
            }
        } else {
            formatter->addStep("webmerc");
            if (l_sourceCRS) {
                datum::Ellipsoid::WGS84->exportToPROJString(formatter);
            }
        }
        bConversionDone = true;
        bEllipsoidParametersDone = true;
    } else if (starts_with(projectionMethodName, "PROJ ")) {
        bConversionDone = true;
        createPROJExtensionFromCustomProj(this, formatter, false);
    } else if (formatter->convention() ==
                   io::PROJStringFormatter::Convention::PROJ_5 &&
               isZUnitConversion) {
        double convFactor =
            parameterValueMeasure(EPSG_NAME_PARAMETER_UNIT_CONVERSION_SCALAR,
                                  EPSG_CODE_PARAMETER_UNIT_CONVERSION_SCALAR)
                .getSIValue();
        auto uom = common::UnitOfMeasure(std::string(), convFactor,
                                         common::UnitOfMeasure::Type::LINEAR)
                       .exportToPROJString();
        auto reverse_uom =
            common::UnitOfMeasure(std::string(), 1.0 / convFactor,
                                  common::UnitOfMeasure::Type::LINEAR)
                .exportToPROJString();
        if (!uom.empty()) {
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

    bool bAxisSpecFound = false;
    if (!bConversionDone) {
        const MethodMapping *mapping = getMapping(projectionMethodName);
        if (!mapping && methodEPSGCode) {
            mapping = getMapping(methodEPSGCode);
        }
        if (mapping && !mapping->proj_names[0].empty()) {
            formatter->addStep(useETMerc ? "etmerc" : mapping->proj_names[0]);
            for (size_t i = 1; i < mapping->proj_names.size(); ++i) {
                if (internal::starts_with(mapping->proj_names[i], "axis=")) {
                    bAxisSpecFound = true;
                }
                formatter->addParam(mapping->proj_names[i]);
            }

            if (ci_equal(projectionMethodName,
                         EPSG_NAME_METHOD_POLAR_STEREOGRAPHIC_VARIANT_B) ||
                methodEPSGCode ==
                    EPSG_CODE_METHOD_POLAR_STEREOGRAPHIC_VARIANT_B) {
                double latitudeStdParallel =
                    parameterValueMeasure(
                        EPSG_NAME_PARAMETER_LATITUDE_STD_PARALLEL,
                        EPSG_CODE_PARAMETER_LATITUDE_STD_PARALLEL)
                        .convertToUnit(common::UnitOfMeasure::DEGREE)
                        .value();
                formatter->addParam("lat_0",
                                    (latitudeStdParallel >= 0) ? 90.0 : -90.0);
            }

            for (const auto &param : mapping->params) {
                for (const auto &proj_name : param.proj_names) {
                    if (proj_name.empty()) {
                        // Case of Krovak COLATITUDE_CONE_AXIS and
                        // LATITUDE_PSEUDO_STANDARD_PARALLEL
                        continue;
                    }
                    if (param.unit_type ==
                        common::UnitOfMeasure::Type::ANGULAR) {
                        formatter->addParam(
                            proj_name,
                            parameterValueMeasure(param.wkt2_name,
                                                  param.epsg_code)
                                .convertToUnit(common::UnitOfMeasure::DEGREE)
                                .value());
                    } else {
                        formatter->addParam(
                            proj_name, parameterValueMeasure(param.wkt2_name,
                                                             param.epsg_code)
                                           .getSIValue());
                    }
                }
            }

        } else {
            throw io::FormattingException("Unsupported conversion method: " +
                                          projectionMethodName);
        }
    }

    auto l_targetCRS = targetCRS();
    if (l_targetCRS && !isZUnitConversion) {
        if (!bEllipsoidParametersDone) {
            auto targetGeogCRS = l_targetCRS->extractGeographicCRS();
            if (targetGeogCRS) {
                if (formatter->convention() ==
                    io::PROJStringFormatter::Convention::PROJ_4) {
                    targetGeogCRS->addDatumInfoToPROJString(formatter);
                } else {
                    targetGeogCRS->ellipsoid()->exportToPROJString(formatter);
                    targetGeogCRS->primeMeridian()->exportToPROJString(
                        formatter);
                }
            }
        }

        auto projCRS =
            std::dynamic_pointer_cast<crs::ProjectedCRS>(l_targetCRS);
        if (projCRS) {
            projCRS->addUnitConvertAndAxisSwap(formatter, bAxisSpecFound);
        }

        auto derivedGeographicCRS =
            std::dynamic_pointer_cast<crs::DerivedGeographicCRS>(l_targetCRS);
        if (derivedGeographicCRS) {
            if (formatter->convention() ==
                io::PROJStringFormatter::Convention::PROJ_5) {
                auto geogCRS = derivedGeographicCRS->baseCRS();
                formatter->setOmitProjLongLatIfPossible(true);
                geogCRS->exportToPROJString(formatter);
                formatter->setOmitProjLongLatIfPossible(false);
            }
        }
    }

    return scope.toString();
}

// ---------------------------------------------------------------------------

/** \brief Return whether a conversion is a [Universal Transverse Mercator]
 * (https://proj4.org/operations/projections/utm.html) conversion.
 *
 * @param[out] zone UTM zone number between 1 and 60.
 * @param[out] north true for UTM northern hemisphere, false for UTM southern
 * hemisphere.
 * @return true if it is a UTM conversion.
 */
bool Conversion::isUTM(int &zone, bool &north) const {
    zone = 0;
    north = true;

    auto projectionMethodName = *(method()->name()->description());
    if (ci_equal(projectionMethodName, EPSG_NAME_METHOD_TRANSVERSE_MERCATOR)) {
        // Check for UTM

        bool bLatitudeNatOriginUTM = false;
        bool bScaleFactorUTM = false;
        bool bFalseEastingUTM = false;
        bool bFalseNorthingUTM = false;
        for (const auto &genOpParamvalue : parameterValues()) {
            const auto &opParamvalue =
                util::nn_dynamic_pointer_cast<OperationParameterValue>(
                    genOpParamvalue);
            if (opParamvalue) {
                const auto &paramName =
                    *(opParamvalue->parameter()->name()->description());
                const auto &l_parameterValue = opParamvalue->parameterValue();
                if (l_parameterValue->type() == ParameterValue::Type::MEASURE) {
                    auto measure = l_parameterValue->value();
                    if (paramName ==
                            EPSG_NAME_PARAMETER_LATITUDE_OF_NATURAL_ORIGIN &&
                        measure.value() == UTM_LATITUDE_OF_NATURAL_ORIGIN) {
                        bLatitudeNatOriginUTM = true;
                    } else if (
                        paramName ==
                            EPSG_NAME_PARAMETER_LONGITUDE_OF_NATURAL_ORIGIN &&
                        measure.unit() == common::UnitOfMeasure::DEGREE) {
                        double dfZone = (measure.value() + 183.0) / 6.0;
                        if (dfZone > 0.9 && dfZone < 60.1 &&
                            std::abs(dfZone - std::round(dfZone)) < 1e-10) {
                            zone = static_cast<int>(std::lround(dfZone));
                        }
                    } else if (
                        paramName ==
                            EPSG_NAME_PARAMETER_SCALE_FACTOR_AT_NATURAL_ORIGIN &&
                        measure.value() == UTM_SCALE_FACTOR) {
                        bScaleFactorUTM = true;
                    } else if (paramName == EPSG_NAME_PARAMETER_FALSE_EASTING &&
                               measure.value() == UTM_FALSE_EASTING &&
                               measure.unit() == common::UnitOfMeasure::METRE) {
                        bFalseEastingUTM = true;
                    } else if (paramName ==
                                   EPSG_NAME_PARAMETER_FALSE_NORTHING &&
                               measure.unit() == common::UnitOfMeasure::METRE) {
                        if (measure.value() == UTM_NORTH_FALSE_NORTHING) {
                            bFalseNorthingUTM = true;
                            north = true;
                        } else if (measure.value() ==
                                   UTM_SOUTH_FALSE_NORTHING) {
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
    auto projectionMethodName = *(method()->name()->description());
    auto newConversion = Conversion::nn_make_shared<Conversion>(*this);
    newConversion->assignSelf(newConversion);

    if (ci_equal(projectionMethodName, EPSG_NAME_METHOD_TRANSVERSE_MERCATOR)) {
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

    TransformationNNPtr registerInv(util::BaseObjectNNPtr thisIn,
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

/** \brief Return the source crs::CRS of the transformation.
 *
 * @return the source CRS.
 */
const crs::CRSNNPtr Transformation::sourceCRS() const {
    return NN_CHECK_ASSERT(CoordinateOperation::sourceCRS());
}

// ---------------------------------------------------------------------------

/** \brief Return the target crs::CRS of the transformation.
 *
 * @return the target CRS.
 */
const crs::CRSNNPtr Transformation::targetCRS() const {
    return NN_CHECK_ASSERT(CoordinateOperation::targetCRS());
}

// ---------------------------------------------------------------------------

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
    auto method_name = *(method()->name()->description());
    if (ci_find(method_name, "Coordinate Frame") != std::string::npos ||
        method()->isEPSG(EPSG_CODE_METHOD_COORDINATE_FRAME_GEOCENTRIC) ||
        method()->isEPSG(EPSG_CODE_METHOD_COORDINATE_FRAME_GEOGRAPHIC_2D) ||
        method()->isEPSG(EPSG_CODE_METHOD_COORDINATE_FRAME_GEOGRAPHIC_3D)) {
        sevenParamsTransform = true;
        invertRotSigns = true;
    } else if (ci_find(method_name, "Position Vector") != std::string::npos ||
               method()->isEPSG(EPSG_CODE_METHOD_POSITION_VECTOR_GEOCENTRIC) ||
               method()->isEPSG(
                   EPSG_CODE_METHOD_POSITION_VECTOR_GEOGRAPHIC_2D) ||
               method()->isEPSG(
                   EPSG_CODE_METHOD_POSITION_VECTOR_GEOGRAPHIC_3D)) {
        sevenParamsTransform = true;
        invertRotSigns = false;
    } else if (ci_find(method_name, "Geocentric translations") !=
                   std::string::npos ||
               method()->isEPSG(
                   EPSG_CODE_METHOD_GEOCENTRIC_TRANSLATION_GEOCENTRIC) ||
               method()->isEPSG(
                   EPSG_CODE_METHOD_GEOCENTRIC_TRANSLATION_GEOGRAPHIC_2D) ||
               method()->isEPSG(
                   EPSG_CODE_METHOD_GEOCENTRIC_TRANSLATION_GEOGRAPHIC_3D)) {
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
        for (const auto &genOpParamvalue : parameterValues()) {
            const auto &opParamvalue =
                util::nn_dynamic_pointer_cast<OperationParameterValue>(
                    genOpParamvalue);
            if (opParamvalue) {
                const auto &parameter = opParamvalue->parameter();
                const auto &paramName = *(parameter->name()->description());
                const auto &l_parameterValue = opParamvalue->parameterValue();
                if (l_parameterValue->type() == ParameterValue::Type::MEASURE) {
                    auto measure = l_parameterValue->value();
                    if (metadata::Identifier::isEquivalentName(
                            paramName,
                            EPSG_NAME_PARAMETER_X_AXIS_TRANSLATION) ||
                        parameter->isEPSG(
                            EPSG_CODE_PARAMETER_X_AXIS_TRANSLATION)) {
                        params[0] = measure.getSIValue();
                        foundX = true;
                    } else if (metadata::Identifier::isEquivalentName(
                                   paramName,
                                   EPSG_NAME_PARAMETER_Y_AXIS_TRANSLATION) ||
                               parameter->isEPSG(
                                   EPSG_CODE_PARAMETER_Y_AXIS_TRANSLATION)) {
                        params[1] = measure.getSIValue();
                        foundY = true;
                    } else if (metadata::Identifier::isEquivalentName(
                                   paramName,
                                   EPSG_NAME_PARAMETER_Z_AXIS_TRANSLATION) ||
                               parameter->isEPSG(
                                   EPSG_CODE_PARAMETER_Z_AXIS_TRANSLATION)) {
                        params[2] = measure.getSIValue();
                        foundZ = true;
                    } else if (metadata::Identifier::isEquivalentName(
                                   paramName,
                                   EPSG_NAME_PARAMETER_X_AXIS_ROTATION) ||
                               parameter->isEPSG(
                                   EPSG_CODE_PARAMETER_X_AXIS_ROTATION)) {
                        params[3] = rotSign *
                                    measure
                                        .convertToUnit(
                                            common::UnitOfMeasure::ARC_SECOND)
                                        .value();
                        foundRotX = true;
                    } else if (metadata::Identifier::isEquivalentName(
                                   paramName,
                                   EPSG_NAME_PARAMETER_Y_AXIS_ROTATION) ||
                               parameter->isEPSG(
                                   EPSG_CODE_PARAMETER_Y_AXIS_ROTATION)) {
                        params[4] = rotSign *
                                    measure
                                        .convertToUnit(
                                            common::UnitOfMeasure::ARC_SECOND)
                                        .value();
                        foundRotY = true;
                    } else if (metadata::Identifier::isEquivalentName(
                                   paramName,
                                   EPSG_NAME_PARAMETER_Z_AXIS_ROTATION) ||
                               parameter->isEPSG(
                                   EPSG_CODE_PARAMETER_Z_AXIS_ROTATION)) {
                        params[5] = rotSign *
                                    measure
                                        .convertToUnit(
                                            common::UnitOfMeasure::ARC_SECOND)
                                        .value();
                        foundRotZ = true;
                    } else if (metadata::Identifier::isEquivalentName(
                                   paramName,
                                   EPSG_NAME_PARAMETER_SCALE_DIFFERENCE) ||
                               parameter->isEPSG(
                                   EPSG_CODE_PARAMETER_SCALE_DIFFERENCE)) {
                        params[6] =
                            measure
                                .convertToUnit(
                                    common::UnitOfMeasure::PARTS_PER_MILLION)
                                .value();
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
    if (ci_equal(method_name, EPSG_NAME_METHOD_GEOGRAPHIC2D_OFFSETS) ||
        method()->isEPSG(EPSG_CODE_METHOD_GEOGRAPHIC2D_OFFSETS) ||
        ci_equal(method_name, EPSG_NAME_METHOD_GEOGRAPHIC3D_OFFSETS) ||
        method()->isEPSG(EPSG_CODE_METHOD_GEOGRAPHIC3D_OFFSETS)) {
        auto offsetLat =
            parameterValueMeasure(EPSG_NAME_PARAMETER_LATITUDE_OFFSET,
                                  EPSG_CODE_PARAMETER_LATITUDE_OFFSET);
        auto offsetLong =
            parameterValueMeasure(EPSG_NAME_PARAMETER_LONGITUDE_OFFSET,
                                  EPSG_CODE_PARAMETER_LONGITUDE_OFFSET);

        auto offsetHeight =
            parameterValueMeasure(EPSG_NAME_PARAMETER_VERTICAL_OFFSET,
                                  EPSG_CODE_PARAMETER_VERTICAL_OFFSET);

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

// ---------------------------------------------------------------------------

/** \brief Instanciate a transformation from a vector of GeneralParameterValue.
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
    auto conv = Transformation::nn_make_shared<Transformation>(
        sourceCRSIn, targetCRSIn, interpolationCRSIn, methodIn, values,
        accuracies);
    conv->assignSelf(conv);
    conv->setProperties(properties);
    return conv;
}

// ---------------------------------------------------------------------------

/** \brief Instanciate a transformation ands its OperationMethod.
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
        {
            OperationParameter::create(
                util::PropertyMap()
                    .set(common::IdentifiedObject::NAME_KEY,
                         EPSG_NAME_PARAMETER_X_AXIS_TRANSLATION)
                    .set(metadata::Identifier::CODESPACE_KEY,
                         metadata::Identifier::EPSG)
                    .set(metadata::Identifier::CODE_KEY,
                         EPSG_CODE_PARAMETER_X_AXIS_TRANSLATION)),
            OperationParameter::create(
                util::PropertyMap()
                    .set(common::IdentifiedObject::NAME_KEY,
                         EPSG_NAME_PARAMETER_Y_AXIS_TRANSLATION)
                    .set(metadata::Identifier::CODESPACE_KEY,
                         metadata::Identifier::EPSG)
                    .set(metadata::Identifier::CODE_KEY,
                         EPSG_CODE_PARAMETER_Y_AXIS_TRANSLATION)),
            OperationParameter::create(
                util::PropertyMap()
                    .set(common::IdentifiedObject::NAME_KEY,
                         EPSG_NAME_PARAMETER_Z_AXIS_TRANSLATION)
                    .set(metadata::Identifier::CODESPACE_KEY,
                         metadata::Identifier::EPSG)
                    .set(metadata::Identifier::CODE_KEY,
                         EPSG_CODE_PARAMETER_Z_AXIS_TRANSLATION)),
            OperationParameter::create(
                util::PropertyMap()
                    .set(common::IdentifiedObject::NAME_KEY,
                         EPSG_NAME_PARAMETER_X_AXIS_ROTATION)
                    .set(metadata::Identifier::CODESPACE_KEY,
                         metadata::Identifier::EPSG)
                    .set(metadata::Identifier::CODE_KEY,
                         EPSG_CODE_PARAMETER_X_AXIS_ROTATION)),
            OperationParameter::create(
                util::PropertyMap()
                    .set(common::IdentifiedObject::NAME_KEY,
                         EPSG_NAME_PARAMETER_Y_AXIS_ROTATION)
                    .set(metadata::Identifier::CODESPACE_KEY,
                         metadata::Identifier::EPSG)
                    .set(metadata::Identifier::CODE_KEY,
                         EPSG_CODE_PARAMETER_Y_AXIS_ROTATION)),
            OperationParameter::create(
                util::PropertyMap()
                    .set(common::IdentifiedObject::NAME_KEY,
                         EPSG_NAME_PARAMETER_Z_AXIS_ROTATION)
                    .set(metadata::Identifier::CODESPACE_KEY,
                         metadata::Identifier::EPSG)
                    .set(metadata::Identifier::CODE_KEY,
                         EPSG_CODE_PARAMETER_Z_AXIS_ROTATION)),
            OperationParameter::create(
                util::PropertyMap()
                    .set(common::IdentifiedObject::NAME_KEY,
                         EPSG_NAME_PARAMETER_SCALE_DIFFERENCE)
                    .set(metadata::Identifier::CODESPACE_KEY,
                         metadata::Identifier::EPSG)
                    .set(metadata::Identifier::CODE_KEY,
                         EPSG_CODE_PARAMETER_SCALE_DIFFERENCE)),
        },
        {
            ParameterValue::create(common::Length(translationXMetre)),
            ParameterValue::create(common::Length(translationYMetre)),
            ParameterValue::create(common::Length(translationZMetre)),
            ParameterValue::create(common::Angle(
                rotationXArcSecond, common::UnitOfMeasure::ARC_SECOND)),
            ParameterValue::create(common::Angle(
                rotationYArcSecond, common::UnitOfMeasure::ARC_SECOND)),
            ParameterValue::create(common::Angle(
                rotationZArcSecond, common::UnitOfMeasure::ARC_SECOND)),
            ParameterValue::create(common::Scale(
                scaleDifferencePPM, common::UnitOfMeasure::PARTS_PER_MILLION)),
        },
        accuracies);
}

// ---------------------------------------------------------------------------

static void getTransformationType(const crs::CRSNNPtr &sourceCRSIn,
                                  const crs::CRSNNPtr &targetCRSIn,
                                  bool &isGeocentric, bool &isGeog2D,
                                  bool &isGeog3D) {
    auto sourceCRSGeod =
        util::nn_dynamic_pointer_cast<crs::GeodeticCRS>(sourceCRSIn);
    auto targetCRSGeod =
        util::nn_dynamic_pointer_cast<crs::GeodeticCRS>(targetCRSIn);
    isGeocentric = sourceCRSGeod && sourceCRSGeod->isGeocentric() &&
                   targetCRSGeod && targetCRSGeod->isGeocentric();

    auto sourceCRSGeog =
        util::nn_dynamic_pointer_cast<crs::GeographicCRS>(sourceCRSIn);
    auto targetCRSGeog =
        util::nn_dynamic_pointer_cast<crs::GeographicCRS>(targetCRSIn);
    isGeog2D = sourceCRSGeog &&
               sourceCRSGeog->coordinateSystem()->axisList().size() == 2 &&
               targetCRSGeog &&
               targetCRSGeog->coordinateSystem()->axisList().size() == 2;
    isGeog3D = !isGeog2D && sourceCRSGeog &&
               sourceCRSGeog->coordinateSystem()->axisList().size() >= 2 &&
               targetCRSGeog &&
               targetCRSGeog->coordinateSystem()->axisList().size() >= 2;
    if (!isGeocentric && !isGeog2D && !isGeog3D) {
        throw InvalidOperation("Inconsistent CRS type");
    }
}
//! @endcond

// ---------------------------------------------------------------------------

/** \brief Instanciate a transformation with Geocentric Translations method.
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
        util::PropertyMap()
            .set(
                common::IdentifiedObject::NAME_KEY,
                isGeocentric
                    ? EPSG_NAME_METHOD_GEOCENTRIC_TRANSLATION_GEOCENTRIC
                    : isGeog2D
                          ? EPSG_NAME_METHOD_GEOCENTRIC_TRANSLATION_GEOGRAPHIC_2D
                          : EPSG_NAME_METHOD_GEOCENTRIC_TRANSLATION_GEOGRAPHIC_3D)
            .set(metadata::Identifier::CODESPACE_KEY,
                 metadata::Identifier::EPSG)
            .set(
                metadata::Identifier::CODE_KEY,
                isGeocentric
                    ? EPSG_CODE_METHOD_GEOCENTRIC_TRANSLATION_GEOCENTRIC
                    : isGeog2D
                          ? EPSG_CODE_METHOD_GEOCENTRIC_TRANSLATION_GEOGRAPHIC_2D
                          : EPSG_CODE_METHOD_GEOCENTRIC_TRANSLATION_GEOGRAPHIC_3D),
        {
            OperationParameter::create(
                util::PropertyMap()
                    .set(common::IdentifiedObject::NAME_KEY,
                         EPSG_NAME_PARAMETER_X_AXIS_TRANSLATION)
                    .set(metadata::Identifier::CODESPACE_KEY,
                         metadata::Identifier::EPSG)
                    .set(metadata::Identifier::CODE_KEY,
                         EPSG_CODE_PARAMETER_X_AXIS_TRANSLATION)),
            OperationParameter::create(
                util::PropertyMap()
                    .set(common::IdentifiedObject::NAME_KEY,
                         EPSG_NAME_PARAMETER_Y_AXIS_TRANSLATION)
                    .set(metadata::Identifier::CODESPACE_KEY,
                         metadata::Identifier::EPSG)
                    .set(metadata::Identifier::CODE_KEY,
                         EPSG_CODE_PARAMETER_Y_AXIS_TRANSLATION)),
            OperationParameter::create(
                util::PropertyMap()
                    .set(common::IdentifiedObject::NAME_KEY,
                         EPSG_NAME_PARAMETER_Z_AXIS_TRANSLATION)
                    .set(metadata::Identifier::CODESPACE_KEY,
                         metadata::Identifier::EPSG)
                    .set(metadata::Identifier::CODE_KEY,
                         EPSG_CODE_PARAMETER_Z_AXIS_TRANSLATION)),
        },
        {
            ParameterValue::create(common::Length(translationXMetre)),
            ParameterValue::create(common::Length(translationYMetre)),
            ParameterValue::create(common::Length(translationZMetre)),
        },
        accuracies);
}

// ---------------------------------------------------------------------------

/** \brief Instanciate a transformation with Position vector transformation
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
        util::PropertyMap()
            .set(common::IdentifiedObject::NAME_KEY,
                 isGeocentric
                     ? EPSG_NAME_METHOD_POSITION_VECTOR_GEOCENTRIC
                     : isGeog2D
                           ? EPSG_NAME_METHOD_POSITION_VECTOR_GEOGRAPHIC_2D
                           : EPSG_NAME_METHOD_POSITION_VECTOR_GEOGRAPHIC_3D)
            .set(metadata::Identifier::CODESPACE_KEY,
                 metadata::Identifier::EPSG)
            .set(metadata::Identifier::CODE_KEY,
                 isGeocentric
                     ? EPSG_CODE_METHOD_POSITION_VECTOR_GEOCENTRIC
                     : isGeog2D
                           ? EPSG_CODE_METHOD_POSITION_VECTOR_GEOGRAPHIC_2D
                           : EPSG_CODE_METHOD_POSITION_VECTOR_GEOGRAPHIC_3D),
        sourceCRSIn, targetCRSIn, translationXMetre, translationYMetre,
        translationZMetre, rotationXArcSecond, rotationYArcSecond,
        rotationZArcSecond, scaleDifferencePPM, accuracies);
}

// ---------------------------------------------------------------------------

/** \brief Instanciate a transformation with Coordinate Frame Rotation method.
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
        util::PropertyMap()
            .set(common::IdentifiedObject::NAME_KEY,
                 isGeocentric
                     ? EPSG_NAME_METHOD_COORDINATE_FRAME_GEOCENTRIC
                     : isGeog2D
                           ? EPSG_NAME_METHOD_COORDINATE_FRAME_GEOGRAPHIC_2D
                           : EPSG_NAME_METHOD_COORDINATE_FRAME_GEOGRAPHIC_3D)
            .set(metadata::Identifier::CODESPACE_KEY,
                 metadata::Identifier::EPSG)
            .set(metadata::Identifier::CODE_KEY,
                 isGeocentric
                     ? EPSG_CODE_METHOD_COORDINATE_FRAME_GEOCENTRIC
                     : isGeog2D
                           ? EPSG_CODE_METHOD_COORDINATE_FRAME_GEOGRAPHIC_2D
                           : EPSG_CODE_METHOD_COORDINATE_FRAME_GEOGRAPHIC_3D),
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
        {
            OperationParameter::create(
                util::PropertyMap()
                    .set(common::IdentifiedObject::NAME_KEY,
                         EPSG_NAME_PARAMETER_X_AXIS_TRANSLATION)
                    .set(metadata::Identifier::CODESPACE_KEY,
                         metadata::Identifier::EPSG)
                    .set(metadata::Identifier::CODE_KEY,
                         EPSG_CODE_PARAMETER_X_AXIS_TRANSLATION)),
            OperationParameter::create(
                util::PropertyMap()
                    .set(common::IdentifiedObject::NAME_KEY,
                         EPSG_NAME_PARAMETER_Y_AXIS_TRANSLATION)
                    .set(metadata::Identifier::CODESPACE_KEY,
                         metadata::Identifier::EPSG)
                    .set(metadata::Identifier::CODE_KEY,
                         EPSG_CODE_PARAMETER_Y_AXIS_TRANSLATION)),
            OperationParameter::create(
                util::PropertyMap()
                    .set(common::IdentifiedObject::NAME_KEY,
                         EPSG_NAME_PARAMETER_Z_AXIS_TRANSLATION)
                    .set(metadata::Identifier::CODESPACE_KEY,
                         metadata::Identifier::EPSG)
                    .set(metadata::Identifier::CODE_KEY,
                         EPSG_CODE_PARAMETER_Z_AXIS_TRANSLATION)),
            OperationParameter::create(
                util::PropertyMap()
                    .set(common::IdentifiedObject::NAME_KEY,
                         EPSG_NAME_PARAMETER_X_AXIS_ROTATION)
                    .set(metadata::Identifier::CODESPACE_KEY,
                         metadata::Identifier::EPSG)
                    .set(metadata::Identifier::CODE_KEY,
                         EPSG_CODE_PARAMETER_X_AXIS_ROTATION)),
            OperationParameter::create(
                util::PropertyMap()
                    .set(common::IdentifiedObject::NAME_KEY,
                         EPSG_NAME_PARAMETER_Y_AXIS_ROTATION)
                    .set(metadata::Identifier::CODESPACE_KEY,
                         metadata::Identifier::EPSG)
                    .set(metadata::Identifier::CODE_KEY,
                         EPSG_CODE_PARAMETER_Y_AXIS_ROTATION)),
            OperationParameter::create(
                util::PropertyMap()
                    .set(common::IdentifiedObject::NAME_KEY,
                         EPSG_NAME_PARAMETER_Z_AXIS_ROTATION)
                    .set(metadata::Identifier::CODESPACE_KEY,
                         metadata::Identifier::EPSG)
                    .set(metadata::Identifier::CODE_KEY,
                         EPSG_CODE_PARAMETER_Z_AXIS_ROTATION)),
            OperationParameter::create(
                util::PropertyMap()
                    .set(common::IdentifiedObject::NAME_KEY,
                         EPSG_NAME_PARAMETER_SCALE_DIFFERENCE)
                    .set(metadata::Identifier::CODESPACE_KEY,
                         metadata::Identifier::EPSG)
                    .set(metadata::Identifier::CODE_KEY,
                         EPSG_CODE_PARAMETER_SCALE_DIFFERENCE)),

            OperationParameter::create(
                util::PropertyMap()
                    .set(common::IdentifiedObject::NAME_KEY,
                         EPSG_NAME_PARAMETER_RATE_X_AXIS_TRANSLATION)
                    .set(metadata::Identifier::CODESPACE_KEY,
                         metadata::Identifier::EPSG)
                    .set(metadata::Identifier::CODE_KEY,
                         EPSG_CODE_PARAMETER_RATE_X_AXIS_TRANSLATION)),
            OperationParameter::create(
                util::PropertyMap()
                    .set(common::IdentifiedObject::NAME_KEY,
                         EPSG_NAME_PARAMETER_RATE_Y_AXIS_TRANSLATION)
                    .set(metadata::Identifier::CODESPACE_KEY,
                         metadata::Identifier::EPSG)
                    .set(metadata::Identifier::CODE_KEY,
                         EPSG_CODE_PARAMETER_RATE_Y_AXIS_TRANSLATION)),
            OperationParameter::create(
                util::PropertyMap()
                    .set(common::IdentifiedObject::NAME_KEY,
                         EPSG_NAME_PARAMETER_RATE_Z_AXIS_TRANSLATION)
                    .set(metadata::Identifier::CODESPACE_KEY,
                         metadata::Identifier::EPSG)
                    .set(metadata::Identifier::CODE_KEY,
                         EPSG_CODE_PARAMETER_RATE_Z_AXIS_TRANSLATION)),
            OperationParameter::create(
                util::PropertyMap()
                    .set(common::IdentifiedObject::NAME_KEY,
                         EPSG_NAME_PARAMETER_RATE_X_AXIS_ROTATION)
                    .set(metadata::Identifier::CODESPACE_KEY,
                         metadata::Identifier::EPSG)
                    .set(metadata::Identifier::CODE_KEY,
                         EPSG_CODE_PARAMETER_RATE_X_AXIS_ROTATION)),
            OperationParameter::create(
                util::PropertyMap()
                    .set(common::IdentifiedObject::NAME_KEY,
                         EPSG_NAME_PARAMETER_RATE_Y_AXIS_ROTATION)
                    .set(metadata::Identifier::CODESPACE_KEY,
                         metadata::Identifier::EPSG)
                    .set(metadata::Identifier::CODE_KEY,
                         EPSG_CODE_PARAMETER_RATE_Y_AXIS_ROTATION)),
            OperationParameter::create(
                util::PropertyMap()
                    .set(common::IdentifiedObject::NAME_KEY,
                         EPSG_NAME_PARAMETER_RATE_Z_AXIS_ROTATION)
                    .set(metadata::Identifier::CODESPACE_KEY,
                         metadata::Identifier::EPSG)
                    .set(metadata::Identifier::CODE_KEY,
                         EPSG_CODE_PARAMETER_RATE_Z_AXIS_ROTATION)),
            OperationParameter::create(
                util::PropertyMap()
                    .set(common::IdentifiedObject::NAME_KEY,
                         EPSG_NAME_PARAMETER_RATE_SCALE_DIFFERENCE)
                    .set(metadata::Identifier::CODESPACE_KEY,
                         metadata::Identifier::EPSG)
                    .set(metadata::Identifier::CODE_KEY,
                         EPSG_CODE_PARAMETER_RATE_SCALE_DIFFERENCE)),

            OperationParameter::create(
                util::PropertyMap()
                    .set(common::IdentifiedObject::NAME_KEY,
                         EPSG_NAME_PARAMETER_REFERENCE_EPOCH)
                    .set(metadata::Identifier::CODESPACE_KEY,
                         metadata::Identifier::EPSG)
                    .set(metadata::Identifier::CODE_KEY,
                         EPSG_CODE_PARAMETER_REFERENCE_EPOCH)),
        },
        {
            ParameterValue::create(common::Length(translationXMetre)),
            ParameterValue::create(common::Length(translationYMetre)),
            ParameterValue::create(common::Length(translationZMetre)),
            ParameterValue::create(common::Angle(
                rotationXArcSecond, common::UnitOfMeasure::ARC_SECOND)),
            ParameterValue::create(common::Angle(
                rotationYArcSecond, common::UnitOfMeasure::ARC_SECOND)),
            ParameterValue::create(common::Angle(
                rotationZArcSecond, common::UnitOfMeasure::ARC_SECOND)),
            ParameterValue::create(common::Scale(
                scaleDifferencePPM, common::UnitOfMeasure::PARTS_PER_MILLION)),
            ParameterValue::create(common::Measure(
                rateTranslationX, common::UnitOfMeasure::METRE_PER_YEAR)),
            ParameterValue::create(common::Measure(
                rateTranslationY, common::UnitOfMeasure::METRE_PER_YEAR)),
            ParameterValue::create(common::Measure(
                rateTranslationZ, common::UnitOfMeasure::METRE_PER_YEAR)),
            ParameterValue::create(common::Measure(
                rateRotationX, common::UnitOfMeasure::ARC_SECOND_PER_YEAR)),
            ParameterValue::create(common::Measure(
                rateRotationY, common::UnitOfMeasure::ARC_SECOND_PER_YEAR)),
            ParameterValue::create(common::Measure(
                rateRotationZ, common::UnitOfMeasure::ARC_SECOND_PER_YEAR)),
            ParameterValue::create(common::Measure(
                rateScaleDifference, common::UnitOfMeasure::PPM_PER_YEAR)),
            ParameterValue::create(common::Measure(
                referenceEpochYear, common::UnitOfMeasure::YEAR)),
        },
        accuracies);
}
//! @endcond

// ---------------------------------------------------------------------------

/** \brief Instanciate a transformation with Time Dependent position vector
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
        util::PropertyMap()
            .set(
                common::IdentifiedObject::NAME_KEY,
                isGeocentric
                    ? EPSG_NAME_METHOD_TIME_DEPENDENT_POSITION_VECTOR_GEOCENTRIC
                    : isGeog2D
                          ? EPSG_NAME_METHOD_TIME_DEPENDENT_POSITION_VECTOR_GEOGRAPHIC_2D
                          : EPSG_NAME_METHOD_TIME_DEPENDENT_POSITION_VECTOR_GEOGRAPHIC_3D)
            .set(metadata::Identifier::CODESPACE_KEY,
                 metadata::Identifier::EPSG)
            .set(
                metadata::Identifier::CODE_KEY,
                isGeocentric
                    ? EPSG_CODE_METHOD_TIME_DEPENDENT_POSITION_VECTOR_GEOCENTRIC
                    : isGeog2D
                          ? EPSG_CODE_METHOD_TIME_DEPENDENT_POSITION_VECTOR_GEOGRAPHIC_2D
                          : EPSG_CODE_METHOD_TIME_DEPENDENT_POSITION_VECTOR_GEOGRAPHIC_3D),
        sourceCRSIn, targetCRSIn, translationXMetre, translationYMetre,
        translationZMetre, rotationXArcSecond, rotationYArcSecond,
        rotationZArcSecond, scaleDifferencePPM, rateTranslationX,
        rateTranslationY, rateTranslationZ, rateRotationX, rateRotationY,
        rateRotationZ, rateScaleDifference, referenceEpochYear, accuracies);
}

// ---------------------------------------------------------------------------

/** \brief Instanciate a transformation with Time Dependent Position coordinate
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
        util::PropertyMap()
            .set(
                common::IdentifiedObject::NAME_KEY,
                isGeocentric
                    ? EPSG_NAME_METHOD_TIME_DEPENDENT_COORDINATE_FRAME_GEOCENTRIC
                    : isGeog2D
                          ? EPSG_NAME_METHOD_TIME_DEPENDENT_COORDINATE_FRAME_GEOGRAPHIC_2D
                          : EPSG_NAME_METHOD_TIME_DEPENDENT_COORDINATE_FRAME_GEOGRAPHIC_3D)
            .set(metadata::Identifier::CODESPACE_KEY,
                 metadata::Identifier::EPSG)
            .set(
                metadata::Identifier::CODE_KEY,
                isGeocentric
                    ? EPSG_CODE_METHOD_TIME_DEPENDENT_COORDINATE_FRAME_GEOCENTRIC
                    : isGeog2D
                          ? EPSG_CODE_METHOD_TIME_DEPENDENT_COORDINATE_FRAME_GEOGRAPHIC_2D
                          : EPSG_CODE_METHOD_TIME_DEPENDENT_COORDINATE_FRAME_GEOGRAPHIC_3D),
        sourceCRSIn, targetCRSIn, translationXMetre, translationYMetre,
        translationZMetre, rotationXArcSecond, rotationYArcSecond,
        rotationZArcSecond, scaleDifferencePPM, rateTranslationX,
        rateTranslationY, rateTranslationZ, rateRotationX, rateRotationY,
        rateRotationZ, rateScaleDifference, referenceEpochYear, accuracies);
}

// ---------------------------------------------------------------------------

/** \brief Instanciate a transformation with Molodensky method.
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
    return Transformation::create(
        properties, sourceCRSIn, targetCRSIn, nullptr,
        util::PropertyMap()
            .set(common::IdentifiedObject::NAME_KEY,
                 EPSG_NAME_METHOD_MOLODENSKY)
            .set(metadata::Identifier::CODESPACE_KEY,
                 metadata::Identifier::EPSG)
            .set(metadata::Identifier::CODE_KEY, EPSG_CODE_METHOD_MOLODENSKY),
        {
            OperationParameter::create(
                util::PropertyMap()
                    .set(common::IdentifiedObject::NAME_KEY,
                         EPSG_NAME_PARAMETER_X_AXIS_TRANSLATION)
                    .set(metadata::Identifier::CODESPACE_KEY,
                         metadata::Identifier::EPSG)
                    .set(metadata::Identifier::CODE_KEY,
                         EPSG_CODE_PARAMETER_X_AXIS_TRANSLATION)),
            OperationParameter::create(
                util::PropertyMap()
                    .set(common::IdentifiedObject::NAME_KEY,
                         EPSG_NAME_PARAMETER_Y_AXIS_TRANSLATION)
                    .set(metadata::Identifier::CODESPACE_KEY,
                         metadata::Identifier::EPSG)
                    .set(metadata::Identifier::CODE_KEY,
                         EPSG_CODE_PARAMETER_Y_AXIS_TRANSLATION)),
            OperationParameter::create(
                util::PropertyMap()
                    .set(common::IdentifiedObject::NAME_KEY,
                         EPSG_NAME_PARAMETER_Z_AXIS_TRANSLATION)
                    .set(metadata::Identifier::CODESPACE_KEY,
                         metadata::Identifier::EPSG)
                    .set(metadata::Identifier::CODE_KEY,
                         EPSG_CODE_PARAMETER_Z_AXIS_TRANSLATION)),

            OperationParameter::create(
                util::PropertyMap()
                    .set(common::IdentifiedObject::NAME_KEY,
                         EPSG_NAME_PARAMETER_SEMI_MAJOR_AXIS_DIFFERENCE)
                    .set(metadata::Identifier::CODESPACE_KEY,
                         metadata::Identifier::EPSG)
                    .set(metadata::Identifier::CODE_KEY,
                         EPSG_CODE_PARAMETER_SEMI_MAJOR_AXIS_DIFFERENCE)),
            OperationParameter::create(
                util::PropertyMap()
                    .set(common::IdentifiedObject::NAME_KEY,
                         EPSG_NAME_PARAMETER_FLATTENING_DIFFERENCE)
                    .set(metadata::Identifier::CODESPACE_KEY,
                         metadata::Identifier::EPSG)
                    .set(metadata::Identifier::CODE_KEY,
                         EPSG_CODE_PARAMETER_FLATTENING_DIFFERENCE)),
        },
        {
            ParameterValue::create(common::Length(translationXMetre)),
            ParameterValue::create(common::Length(translationYMetre)),
            ParameterValue::create(common::Length(translationZMetre)),
            ParameterValue::create(
                common::Length(semiMajorAxisDifferenceMetre)),
            ParameterValue::create(common::Measure(
                flattingDifference, common::UnitOfMeasure::NONE)),
        },
        accuracies);
}

// ---------------------------------------------------------------------------

/** \brief Instanciate a transformation with Abridged Molodensky method.
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
    return Transformation::create(
        properties, sourceCRSIn, targetCRSIn, nullptr,
        util::PropertyMap()
            .set(common::IdentifiedObject::NAME_KEY,
                 EPSG_NAME_METHOD_ABRIDGED_MOLODENSKY)
            .set(metadata::Identifier::CODESPACE_KEY,
                 metadata::Identifier::EPSG)
            .set(metadata::Identifier::CODE_KEY,
                 EPSG_CODE_METHOD_ABRIDGED_MOLODENSKY),
        {
            OperationParameter::create(
                util::PropertyMap()
                    .set(common::IdentifiedObject::NAME_KEY,
                         EPSG_NAME_PARAMETER_X_AXIS_TRANSLATION)
                    .set(metadata::Identifier::CODESPACE_KEY,
                         metadata::Identifier::EPSG)
                    .set(metadata::Identifier::CODE_KEY,
                         EPSG_CODE_PARAMETER_X_AXIS_TRANSLATION)),
            OperationParameter::create(
                util::PropertyMap()
                    .set(common::IdentifiedObject::NAME_KEY,
                         EPSG_NAME_PARAMETER_Y_AXIS_TRANSLATION)
                    .set(metadata::Identifier::CODESPACE_KEY,
                         metadata::Identifier::EPSG)
                    .set(metadata::Identifier::CODE_KEY,
                         EPSG_CODE_PARAMETER_Y_AXIS_TRANSLATION)),
            OperationParameter::create(
                util::PropertyMap()
                    .set(common::IdentifiedObject::NAME_KEY,
                         EPSG_NAME_PARAMETER_Z_AXIS_TRANSLATION)
                    .set(metadata::Identifier::CODESPACE_KEY,
                         metadata::Identifier::EPSG)
                    .set(metadata::Identifier::CODE_KEY,
                         EPSG_CODE_PARAMETER_Z_AXIS_TRANSLATION)),

            OperationParameter::create(
                util::PropertyMap()
                    .set(common::IdentifiedObject::NAME_KEY,
                         EPSG_NAME_PARAMETER_SEMI_MAJOR_AXIS_DIFFERENCE)
                    .set(metadata::Identifier::CODESPACE_KEY,
                         metadata::Identifier::EPSG)
                    .set(metadata::Identifier::CODE_KEY,
                         EPSG_CODE_PARAMETER_SEMI_MAJOR_AXIS_DIFFERENCE)),
            OperationParameter::create(
                util::PropertyMap()
                    .set(common::IdentifiedObject::NAME_KEY,
                         EPSG_NAME_PARAMETER_FLATTENING_DIFFERENCE)
                    .set(metadata::Identifier::CODESPACE_KEY,
                         metadata::Identifier::EPSG)
                    .set(metadata::Identifier::CODE_KEY,
                         EPSG_CODE_PARAMETER_FLATTENING_DIFFERENCE)),
        },
        {
            ParameterValue::create(common::Length(translationXMetre)),
            ParameterValue::create(common::Length(translationYMetre)),
            ParameterValue::create(common::Length(translationZMetre)),
            ParameterValue::create(
                common::Length(semiMajorAxisDifferenceMetre)),
            ParameterValue::create(common::Measure(
                flattingDifference, common::UnitOfMeasure::NONE)),
        },
        accuracies);
}

// ---------------------------------------------------------------------------

/** \brief Instanciate a transformation from TOWGS84 parameters.
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

    crs::CRSPtr transformSourceCRS = sourceCRSIn->extractGeographicCRS();
    if (!transformSourceCRS) {
        throw InvalidOperation(
            "Cannot find GeographicCRS in sourceCRS of TOWGS84 transformation");
    }

    if (TOWGS84Parameters.size() == 3) {
        return createGeocentricTranslations(
            util::PropertyMap().set(
                common::IdentifiedObject::NAME_KEY,
                "Transformation from " +
                    *(transformSourceCRS->name()->description()) + " to WGS84"),
            NN_CHECK_ASSERT(transformSourceCRS), crs::GeographicCRS::EPSG_4326,
            TOWGS84Parameters[0], TOWGS84Parameters[1], TOWGS84Parameters[2],
            {});
    }

    return createPositionVector(
        util::PropertyMap().set(
            common::IdentifiedObject::NAME_KEY,
            "Transformation from " +
                *(transformSourceCRS->name()->description()) + " to WGS84"),
        NN_CHECK_ASSERT(transformSourceCRS), crs::GeographicCRS::EPSG_4326,
        TOWGS84Parameters[0], TOWGS84Parameters[1], TOWGS84Parameters[2],
        TOWGS84Parameters[3], TOWGS84Parameters[4], TOWGS84Parameters[5],
        TOWGS84Parameters[6], {});
}

// ---------------------------------------------------------------------------
/** \brief Instanciate a transformation with NTv2 method.
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

    return create(
        properties, sourceCRSIn, targetCRSIn, nullptr,
        util::PropertyMap()
            .set(common::IdentifiedObject::NAME_KEY, EPSG_NAME_METHOD_NTV2)
            .set(metadata::Identifier::CODESPACE_KEY,
                 metadata::Identifier::EPSG)
            .set(metadata::Identifier::CODE_KEY, EPSG_CODE_METHOD_NTV2),
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

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
static TransformationNNPtr _createGravityRelatedHeightToGeographic3D(
    const util::PropertyMap &properties, bool inverse,
    const crs::CRSNNPtr &sourceCRSIn, const crs::CRSNNPtr &targetCRSIn,
    const std::string &filename,
    const std::vector<metadata::PositionalAccuracyNNPtr> &accuracies) {

    return Transformation::create(
        properties, sourceCRSIn, targetCRSIn, nullptr,
        util::PropertyMap().set(
            common::IdentifiedObject::NAME_KEY,
            inverse ? INVERSE_OF + PROJ_WKT2_NAME_METHOD_HEIGHT_TO_GEOG3D
                    : PROJ_WKT2_NAME_METHOD_HEIGHT_TO_GEOG3D),
        {OperationParameter::create(
            util::PropertyMap()
                .set(common::IdentifiedObject::NAME_KEY,
                     EPSG_NAME_PARAMETER_GEOID_CORRECTION_FILENAME)
                .set(metadata::Identifier::CODESPACE_KEY,
                     metadata::Identifier::EPSG)
                .set(metadata::Identifier::CODE_KEY,
                     EPSG_CODE_PARAMETER_GEOID_CORRECTION_FILENAME))},
        {ParameterValue::createFilename(filename)}, accuracies);
}
//! @endcond

// ---------------------------------------------------------------------------
/** \brief Instanciate a transformation from GravityRelatedHeight to
 * Geographic3D
 *
 * @param properties See \ref general_properties of the Transformation.
 * At minimum the name should be defined.
 * @param sourceCRSIn Source CRS.
 * @param targetCRSIn Target CRS.
 * @param filename GRID filename.
 * @param accuracies Vector of positional accuracy (might be empty).
 * @return new Transformation.
 */
TransformationNNPtr Transformation::createGravityRelatedHeightToGeographic3D(
    const util::PropertyMap &properties, const crs::CRSNNPtr &sourceCRSIn,
    const crs::CRSNNPtr &targetCRSIn, const std::string &filename,
    const std::vector<metadata::PositionalAccuracyNNPtr> &accuracies) {

    return _createGravityRelatedHeightToGeographic3D(
        properties, false, sourceCRSIn, targetCRSIn, filename, accuracies);
}

// ---------------------------------------------------------------------------

/** \brief Instanciate a transformation with method VERTCON
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

    return create(
        properties, sourceCRSIn, targetCRSIn, nullptr,
        util::PropertyMap()
            .set(common::IdentifiedObject::NAME_KEY, EPSG_NAME_METHOD_VERTCON)
            .set(metadata::Identifier::CODESPACE_KEY,
                 metadata::Identifier::EPSG)
            .set(metadata::Identifier::CODE_KEY, EPSG_CODE_METHOD_VERTCON),
        {OperationParameter::create(
            util::PropertyMap()
                .set(common::IdentifiedObject::NAME_KEY,
                     EPSG_NAME_PARAMETER_VERTICAL_OFFSET_FILE)
                .set(metadata::Identifier::CODESPACE_KEY,
                     metadata::Identifier::EPSG)
                .set(metadata::Identifier::CODE_KEY,
                     EPSG_CODE_PARAMETER_VERTICAL_OFFSET_FILE))},
        {ParameterValue::createFilename(filename)}, accuracies);
}

// ---------------------------------------------------------------------------

/** \brief Instanciate a transformation with method Longitude rotation
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

    return create(properties, sourceCRSIn, targetCRSIn, nullptr,
                  util::PropertyMap()
                      .set(common::IdentifiedObject::NAME_KEY,
                           EPSG_NAME_METHOD_LONGITUDE_ROTATION)
                      .set(metadata::Identifier::CODESPACE_KEY,
                           metadata::Identifier::EPSG)
                      .set(metadata::Identifier::CODE_KEY,
                           EPSG_CODE_METHOD_LONGITUDE_ROTATION),
                  {OperationParameter::create(
                      util::PropertyMap()
                          .set(common::IdentifiedObject::NAME_KEY,
                               EPSG_NAME_PARAMETER_LONGITUDE_OFFSET)
                          .set(metadata::Identifier::CODESPACE_KEY,
                               metadata::Identifier::EPSG)
                          .set(metadata::Identifier::CODE_KEY,
                               EPSG_CODE_PARAMETER_LONGITUDE_OFFSET))},
                  {ParameterValue::create(offset)},
                  {metadata::PositionalAccuracy::create("0")});
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
bool Transformation::isLongitudeRotation() const {
    auto method_name = *(method()->name()->description());
    return ci_equal(method_name, EPSG_NAME_METHOD_LONGITUDE_ROTATION) ||
           method()->isEPSG(EPSG_CODE_METHOD_LONGITUDE_ROTATION);
}

//! @endcond

// ---------------------------------------------------------------------------

/** \brief Instanciate a transformation with method Geographic 2D offsets
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
        util::PropertyMap()
            .set(common::IdentifiedObject::NAME_KEY,
                 EPSG_NAME_METHOD_GEOGRAPHIC2D_OFFSETS)
            .set(metadata::Identifier::CODESPACE_KEY,
                 metadata::Identifier::EPSG)
            .set(metadata::Identifier::CODE_KEY,
                 EPSG_CODE_METHOD_GEOGRAPHIC2D_OFFSETS),
        {OperationParameter::create(
             util::PropertyMap()
                 .set(common::IdentifiedObject::NAME_KEY,
                      EPSG_NAME_PARAMETER_LATITUDE_OFFSET)
                 .set(metadata::Identifier::CODESPACE_KEY,
                      metadata::Identifier::EPSG)
                 .set(metadata::Identifier::CODE_KEY,
                      EPSG_CODE_PARAMETER_LATITUDE_OFFSET)),
         OperationParameter::create(
             util::PropertyMap()
                 .set(common::IdentifiedObject::NAME_KEY,
                      EPSG_NAME_PARAMETER_LONGITUDE_OFFSET)
                 .set(metadata::Identifier::CODESPACE_KEY,
                      metadata::Identifier::EPSG)
                 .set(metadata::Identifier::CODE_KEY,
                      EPSG_CODE_PARAMETER_LONGITUDE_OFFSET))},
        {ParameterValue::create(offsetLat), ParameterValue::create(offsetLon)},
        accuracies);
}

// ---------------------------------------------------------------------------

/** \brief Instanciate a transformation with method Geographic 3D offsets
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
    return create(properties, sourceCRSIn, targetCRSIn, nullptr,
                  util::PropertyMap()
                      .set(common::IdentifiedObject::NAME_KEY,
                           EPSG_NAME_METHOD_GEOGRAPHIC3D_OFFSETS)
                      .set(metadata::Identifier::CODESPACE_KEY,
                           metadata::Identifier::EPSG)
                      .set(metadata::Identifier::CODE_KEY,
                           EPSG_CODE_METHOD_GEOGRAPHIC3D_OFFSETS),
                  {OperationParameter::create(
                       util::PropertyMap()
                           .set(common::IdentifiedObject::NAME_KEY,
                                EPSG_NAME_PARAMETER_LATITUDE_OFFSET)
                           .set(metadata::Identifier::CODESPACE_KEY,
                                metadata::Identifier::EPSG)
                           .set(metadata::Identifier::CODE_KEY,
                                EPSG_CODE_PARAMETER_LATITUDE_OFFSET)),
                   OperationParameter::create(
                       util::PropertyMap()
                           .set(common::IdentifiedObject::NAME_KEY,
                                EPSG_NAME_PARAMETER_LONGITUDE_OFFSET)
                           .set(metadata::Identifier::CODESPACE_KEY,
                                metadata::Identifier::EPSG)
                           .set(metadata::Identifier::CODE_KEY,
                                EPSG_CODE_PARAMETER_LONGITUDE_OFFSET)),
                   OperationParameter::create(
                       util::PropertyMap()
                           .set(common::IdentifiedObject::NAME_KEY,
                                EPSG_NAME_PARAMETER_VERTICAL_OFFSET)
                           .set(metadata::Identifier::CODESPACE_KEY,
                                metadata::Identifier::EPSG)
                           .set(metadata::Identifier::CODE_KEY,
                                EPSG_CODE_PARAMETER_VERTICAL_OFFSET))},
                  {ParameterValue::create(offsetLat),
                   ParameterValue::create(offsetLon),
                   ParameterValue::create(offsetHeight)},
                  accuracies);
}

// ---------------------------------------------------------------------------

/** \brief Instanciate a transformation with method Geographic 2D with
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
    return create(properties, sourceCRSIn, targetCRSIn, nullptr,
                  util::PropertyMap()
                      .set(common::IdentifiedObject::NAME_KEY,
                           EPSG_NAME_METHOD_GEOGRAPHIC2D_WITH_HEIGHT_OFFSETS)
                      .set(metadata::Identifier::CODESPACE_KEY,
                           metadata::Identifier::EPSG)
                      .set(metadata::Identifier::CODE_KEY,
                           EPSG_CODE_METHOD_GEOGRAPHIC2D_WITH_HEIGHT_OFFSETS),
                  {OperationParameter::create(
                       util::PropertyMap()
                           .set(common::IdentifiedObject::NAME_KEY,
                                EPSG_NAME_PARAMETER_LATITUDE_OFFSET)
                           .set(metadata::Identifier::CODESPACE_KEY,
                                metadata::Identifier::EPSG)
                           .set(metadata::Identifier::CODE_KEY,
                                EPSG_CODE_PARAMETER_LATITUDE_OFFSET)),
                   OperationParameter::create(
                       util::PropertyMap()
                           .set(common::IdentifiedObject::NAME_KEY,
                                EPSG_NAME_PARAMETER_LONGITUDE_OFFSET)
                           .set(metadata::Identifier::CODESPACE_KEY,
                                metadata::Identifier::EPSG)
                           .set(metadata::Identifier::CODE_KEY,
                                EPSG_CODE_PARAMETER_LONGITUDE_OFFSET)),
                   OperationParameter::create(
                       util::PropertyMap()
                           .set(common::IdentifiedObject::NAME_KEY,
                                EPSG_NAME_PARAMETER_GEOID_UNDULATION)
                           .set(metadata::Identifier::CODESPACE_KEY,
                                metadata::Identifier::EPSG)
                           .set(metadata::Identifier::CODE_KEY,
                                EPSG_CODE_PARAMETER_GEOID_UNDULATION))},
                  {ParameterValue::create(offsetLat),
                   ParameterValue::create(offsetLon),
                   ParameterValue::create(offsetHeight)},
                  accuracies);
}

// ---------------------------------------------------------------------------

/** \brief Instanciate a transformation with method Vertical Offset.
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
                  util::PropertyMap()
                      .set(common::IdentifiedObject::NAME_KEY,
                           EPSG_NAME_METHOD_VERTICAL_OFFSET)
                      .set(metadata::Identifier::CODESPACE_KEY,
                           metadata::Identifier::EPSG)
                      .set(metadata::Identifier::CODE_KEY,
                           EPSG_CODE_METHOD_VERTICAL_OFFSET),
                  {OperationParameter::create(
                      util::PropertyMap()
                          .set(common::IdentifiedObject::NAME_KEY,
                               EPSG_NAME_PARAMETER_VERTICAL_OFFSET)
                          .set(metadata::Identifier::CODESPACE_KEY,
                               metadata::Identifier::EPSG)
                          .set(metadata::Identifier::CODE_KEY,
                               EPSG_CODE_PARAMETER_VERTICAL_OFFSET))},
                  {ParameterValue::create(offsetHeight)}, accuracies);
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
static TransformationNNPtr
createGeographicGeocentric(const util::PropertyMap &properties,
                           const crs::CRSNNPtr &sourceCRSIn,
                           const crs::CRSNNPtr &targetCRSIn) {
    return Transformation::create(
        properties, sourceCRSIn, targetCRSIn, nullptr,
        util::PropertyMap()
            .set(common::IdentifiedObject::NAME_KEY,
                 EPSG_NAME_METHOD_GEOGRAPHIC_GEOCENTRIC)
            .set(metadata::Identifier::CODESPACE_KEY,
                 metadata::Identifier::EPSG)
            .set(metadata::Identifier::CODE_KEY,
                 EPSG_CODE_METHOD_GEOGRAPHIC_GEOCENTRIC),
        {}, {}, {metadata::PositionalAccuracy::create("0")});
}

// ---------------------------------------------------------------------------

static util::PropertyMap
createPropertiesForInverse(const CoordinateOperation *op, bool derivedFrom,
                           bool approximateInversion) {
    util::PropertyMap map;

    // The domain(s) are unchanged by the inverse operation
    {
        auto ar = util::ArrayOfBaseObject::create();
        for (const auto &domain : op->domains()) {
            ar->add(domain);
        }
        if (!ar->empty()) {
            map.set(common::ObjectUsage::OBJECT_DOMAIN_KEY, ar);
        }
    }

    // Forge a name for the inverse, either from the forward name, or
    // from the source and target CRS names
    std::string opType;
    if (dynamic_cast<const Transformation *>(op)) {
        opType = "Transformation";
    } else if (dynamic_cast<const Conversion *>(op)) {
        opType = "Conversion";
    } else if (dynamic_cast<const ConcatenatedOperation *>(op)) {
        opType = "Concatenated operation";
    } else {
        opType = "Operation";
    }

    auto sourceCRS = op->sourceCRS();
    auto targetCRS = op->targetCRS();
    std::string forwardName = *op->name()->description();
    std::string name;
    if (!forwardName.empty()) {
        if (starts_with(forwardName, INVERSE_OF)) {
            name = forwardName.substr(INVERSE_OF.size());
        } else if (!sourceCRS || !targetCRS ||
                   forwardName !=
                       opType + " from " + *(sourceCRS->name()->description()) +
                           " to " + *(targetCRS->name()->description())) {
            name = INVERSE_OF + forwardName;
        }
    }
    if (name.empty() && sourceCRS && targetCRS) {
        name = opType + " from " + *(targetCRS->name()->description()) +
               " to " + *(sourceCRS->name()->description());
    }
    if (approximateInversion) {
        name += " (approx. inversion)";
    }

    if (dynamic_cast<const PROJBasedOperation *>(op)) {
        name = forwardName;
    }

    if (!name.empty()) {
        map.set(common::IdentifiedObject::NAME_KEY, name);
    }

    // If original operation is AUTH:CODE, then assign INVERSE(AUTH):CODE
    // as identifier.
    {
        auto ar = util::ArrayOfBaseObject::create();
        for (const auto &idSrc : op->identifiers()) {
            auto authName = *(idSrc->codeSpace());
            auto srcCode = idSrc->code();
            if (derivedFrom) {
                authName = "DERIVED_FROM(" + authName + ")";
            }
            if (starts_with(authName, "INVERSE(") && authName.back() == ')') {
                authName = authName.substr(strlen("INVERSE("));
                authName.resize(authName.size() - 1);
            } else {
                authName = "INVERSE(" + authName + ")";
            }
            auto idsProp = util::PropertyMap().set(
                metadata::Identifier::CODESPACE_KEY, authName);
            ar->add(metadata::Identifier::create(srcCode, idsProp));
        }
        if (!ar->empty()) {
            map.set(common::IdentifiedObject::IDENTIFIERS_KEY, ar);
        }
    }

    return map;
}

// ---------------------------------------------------------------------------

static bool isTimeDependent(const std::string &method_name) {
    return ci_find(method_name, "Time dependent") != std::string::npos ||
           ci_find(method_name, "Time-dependent") != std::string::npos;
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
    auto method = op->method();
    auto method_name = *(method->name()->description());

    // See end of "2.4.3.3 Helmert 7-parameter transformations"
    // in EPSG 7-2 guidance
    // For practical purposes, the inverse of 7- or 15-parameters Helmert
    // can be obtained by using the forward method with all parameters
    // negated
    // (except reference epoch!)
    // So for WKT export use that. But for PROJ string, we use the +inv flag
    // so as to get "perfect" round-tripability.
    if ((ci_find(method_name, "Coordinate Frame") != std::string::npos &&
         !isTimeDependent(method_name)) ||
        method->isEPSG(EPSG_CODE_METHOD_COORDINATE_FRAME_GEOCENTRIC) ||
        method->isEPSG(EPSG_CODE_METHOD_COORDINATE_FRAME_GEOGRAPHIC_2D) ||
        method->isEPSG(EPSG_CODE_METHOD_COORDINATE_FRAME_GEOGRAPHIC_3D)) {
        sevenParamsTransform = true;
    } else if (
        (ci_find(method_name, "Coordinate Frame") != std::string::npos &&
         isTimeDependent(method_name)) ||
        method->isEPSG(
            EPSG_CODE_METHOD_TIME_DEPENDENT_COORDINATE_FRAME_GEOCENTRIC) ||
        method->isEPSG(
            EPSG_CODE_METHOD_TIME_DEPENDENT_COORDINATE_FRAME_GEOGRAPHIC_2D) ||
        method->isEPSG(
            EPSG_CODE_METHOD_TIME_DEPENDENT_COORDINATE_FRAME_GEOGRAPHIC_3D)) {
        fifteenParamsTransform = true;
    } else if ((ci_find(method_name, "Position Vector") != std::string::npos &&
                !isTimeDependent(method_name)) ||
               method->isEPSG(EPSG_CODE_METHOD_POSITION_VECTOR_GEOCENTRIC) ||
               method->isEPSG(EPSG_CODE_METHOD_POSITION_VECTOR_GEOGRAPHIC_2D) ||
               method->isEPSG(EPSG_CODE_METHOD_POSITION_VECTOR_GEOGRAPHIC_3D)) {
        sevenParamsTransform = true;
    } else if (
        (ci_find(method_name, "Position Vector") != std::string::npos &&
         isTimeDependent(method_name)) ||
        method->isEPSG(
            EPSG_CODE_METHOD_TIME_DEPENDENT_POSITION_VECTOR_GEOCENTRIC) ||
        method->isEPSG(
            EPSG_CODE_METHOD_TIME_DEPENDENT_POSITION_VECTOR_GEOGRAPHIC_2D) ||
        method->isEPSG(
            EPSG_CODE_METHOD_TIME_DEPENDENT_POSITION_VECTOR_GEOGRAPHIC_3D)) {
        fifteenParamsTransform = true;
    }
    if (sevenParamsTransform || fifteenParamsTransform) {
        double neg_x = negate(
            op->parameterValueMeasure(EPSG_NAME_PARAMETER_X_AXIS_TRANSLATION,
                                      EPSG_CODE_PARAMETER_X_AXIS_TRANSLATION)
                .getSIValue());
        double neg_y = negate(
            op->parameterValueMeasure(EPSG_NAME_PARAMETER_Y_AXIS_TRANSLATION,
                                      EPSG_CODE_PARAMETER_Y_AXIS_TRANSLATION)
                .getSIValue());
        double neg_z = negate(
            op->parameterValueMeasure(EPSG_NAME_PARAMETER_Z_AXIS_TRANSLATION,
                                      EPSG_CODE_PARAMETER_Z_AXIS_TRANSLATION)
                .getSIValue());
        double neg_rx = negate(
            op->parameterValueMeasure(EPSG_NAME_PARAMETER_X_AXIS_ROTATION,
                                      EPSG_CODE_PARAMETER_X_AXIS_ROTATION)
                .convertToUnit(common::UnitOfMeasure::ARC_SECOND)
                .value());
        double neg_ry = negate(
            op->parameterValueMeasure(EPSG_NAME_PARAMETER_Y_AXIS_ROTATION,
                                      EPSG_CODE_PARAMETER_Y_AXIS_ROTATION)
                .convertToUnit(common::UnitOfMeasure::ARC_SECOND)
                .value());
        double neg_rz = negate(
            op->parameterValueMeasure(EPSG_NAME_PARAMETER_Z_AXIS_ROTATION,
                                      EPSG_CODE_PARAMETER_Z_AXIS_ROTATION)
                .convertToUnit(common::UnitOfMeasure::ARC_SECOND)
                .value());
        double neg_scaleDiff = negate(
            op->parameterValueMeasure(EPSG_NAME_PARAMETER_SCALE_DIFFERENCE,
                                      EPSG_CODE_PARAMETER_SCALE_DIFFERENCE)
                .convertToUnit(common::UnitOfMeasure::PARTS_PER_MILLION)
                .value());
        auto methodProperties = util::PropertyMap().set(
            common::IdentifiedObject::NAME_KEY, method_name);
        int method_epsg_code = method->getEPSGCode();
        if (method_epsg_code) {
            methodProperties
                .set(metadata::Identifier::CODESPACE_KEY,
                     metadata::Identifier::EPSG)
                .set(metadata::Identifier::CODE_KEY, method_epsg_code);
        }
        if (fifteenParamsTransform) {
            double neg_rate_x =
                negate(op->parameterValueMeasure(
                             EPSG_NAME_PARAMETER_RATE_X_AXIS_TRANSLATION,
                             EPSG_CODE_PARAMETER_RATE_X_AXIS_TRANSLATION)
                           .convertToUnit(common::UnitOfMeasure::METRE_PER_YEAR)
                           .value());
            double neg_rate_y =
                negate(op->parameterValueMeasure(
                             EPSG_NAME_PARAMETER_RATE_Y_AXIS_TRANSLATION,
                             EPSG_CODE_PARAMETER_RATE_Y_AXIS_TRANSLATION)
                           .convertToUnit(common::UnitOfMeasure::METRE_PER_YEAR)
                           .value());
            double neg_rate_z =
                negate(op->parameterValueMeasure(
                             EPSG_NAME_PARAMETER_RATE_Z_AXIS_TRANSLATION,
                             EPSG_CODE_PARAMETER_RATE_Z_AXIS_TRANSLATION)
                           .convertToUnit(common::UnitOfMeasure::METRE_PER_YEAR)
                           .value());
            double neg_rate_rx = negate(
                op->parameterValueMeasure(
                      EPSG_NAME_PARAMETER_RATE_X_AXIS_ROTATION,
                      EPSG_CODE_PARAMETER_RATE_X_AXIS_ROTATION)
                    .convertToUnit(common::UnitOfMeasure::ARC_SECOND_PER_YEAR)
                    .value());
            double neg_rate_ry = negate(
                op->parameterValueMeasure(
                      EPSG_NAME_PARAMETER_RATE_Y_AXIS_ROTATION,
                      EPSG_CODE_PARAMETER_RATE_Y_AXIS_ROTATION)
                    .convertToUnit(common::UnitOfMeasure::ARC_SECOND_PER_YEAR)
                    .value());
            double neg_rate_rz = negate(
                op->parameterValueMeasure(
                      EPSG_NAME_PARAMETER_RATE_Z_AXIS_ROTATION,
                      EPSG_CODE_PARAMETER_RATE_Z_AXIS_ROTATION)
                    .convertToUnit(common::UnitOfMeasure::ARC_SECOND_PER_YEAR)
                    .value());
            double neg_rate_scaleDiff =
                negate(op->parameterValueMeasure(
                             EPSG_NAME_PARAMETER_RATE_SCALE_DIFFERENCE,
                             EPSG_CODE_PARAMETER_RATE_SCALE_DIFFERENCE)
                           .convertToUnit(common::UnitOfMeasure::PPM_PER_YEAR)
                           .value());
            double referenceEpochYear =
                op->parameterValueMeasure(EPSG_NAME_PARAMETER_REFERENCE_EPOCH,
                                          EPSG_CODE_PARAMETER_REFERENCE_EPOCH)
                    .convertToUnit(common::UnitOfMeasure::YEAR)
                    .value();
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
Transformation::Private::registerInv(util::BaseObjectNNPtr thisIn,
                                     TransformationNNPtr invTransform) {
    invTransform->d->forwardOperation_ =
        util::nn_dynamic_pointer_cast<Transformation>(thisIn);
    return invTransform;
}
//! @endcond

// ---------------------------------------------------------------------------

CoordinateOperationNNPtr Transformation::inverse() const {
    return inverseAsTransformation();
}

// ---------------------------------------------------------------------------

TransformationNNPtr Transformation::inverseAsTransformation() const {
    auto method_name = *(method()->name()->description());

    if (d->forwardOperation_) {
        return NN_CHECK_ASSERT(d->forwardOperation_);
    }

    // For geocentric translation, the inverse is exactly the negation of
    // the parameters.
    if (ci_find(method_name, "Geocentric translations") != std::string::npos ||
        method()->isEPSG(EPSG_CODE_METHOD_GEOCENTRIC_TRANSLATION_GEOCENTRIC) ||
        method()->isEPSG(
            EPSG_CODE_METHOD_GEOCENTRIC_TRANSLATION_GEOGRAPHIC_2D) ||
        method()->isEPSG(
            EPSG_CODE_METHOD_GEOCENTRIC_TRANSLATION_GEOGRAPHIC_3D)) {
        double x = parameterValueMeasure(EPSG_NAME_PARAMETER_X_AXIS_TRANSLATION,
                                         EPSG_CODE_PARAMETER_X_AXIS_TRANSLATION)
                       .getSIValue();
        double y = parameterValueMeasure(EPSG_NAME_PARAMETER_Y_AXIS_TRANSLATION,
                                         EPSG_CODE_PARAMETER_Y_AXIS_TRANSLATION)
                       .getSIValue();
        double z = parameterValueMeasure(EPSG_NAME_PARAMETER_Z_AXIS_TRANSLATION,
                                         EPSG_CODE_PARAMETER_Z_AXIS_TRANSLATION)
                       .getSIValue();
        return d->registerInv(
            shared_from_this(),
            createGeocentricTranslations(
                createPropertiesForInverse(this, false, false), targetCRS(),
                sourceCRS(), negate(x), negate(y), negate(z),
                coordinateOperationAccuracies()));
    }

    if (ci_find(method_name, "Molodensky") != std::string::npos ||
        method()->isEPSG(EPSG_CODE_METHOD_MOLODENSKY) ||
        method()->isEPSG(EPSG_CODE_METHOD_ABRIDGED_MOLODENSKY)) {
        double x = parameterValueMeasure(EPSG_NAME_PARAMETER_X_AXIS_TRANSLATION,
                                         EPSG_CODE_PARAMETER_X_AXIS_TRANSLATION)
                       .getSIValue();
        double y = parameterValueMeasure(EPSG_NAME_PARAMETER_Y_AXIS_TRANSLATION,
                                         EPSG_CODE_PARAMETER_Y_AXIS_TRANSLATION)
                       .getSIValue();
        double z = parameterValueMeasure(EPSG_NAME_PARAMETER_Z_AXIS_TRANSLATION,
                                         EPSG_CODE_PARAMETER_Z_AXIS_TRANSLATION)
                       .getSIValue();
        double da = parameterValueMeasure(
                        EPSG_NAME_PARAMETER_SEMI_MAJOR_AXIS_DIFFERENCE,
                        EPSG_CODE_PARAMETER_SEMI_MAJOR_AXIS_DIFFERENCE)
                        .getSIValue();
        double df =
            parameterValueMeasure(EPSG_NAME_PARAMETER_FLATTENING_DIFFERENCE,
                                  EPSG_CODE_PARAMETER_FLATTENING_DIFFERENCE)
                .getSIValue();

        if (ci_find(method_name, "Abridged") != std::string::npos ||
            method()->isEPSG(EPSG_CODE_METHOD_ABRIDGED_MOLODENSKY)) {
            return d->registerInv(
                shared_from_this(),
                createAbridgedMolodensky(
                    createPropertiesForInverse(this, false, false), targetCRS(),
                    sourceCRS(), negate(x), negate(y), negate(z), negate(da),
                    negate(df), coordinateOperationAccuracies()));
        } else {
            return d->registerInv(
                shared_from_this(),
                createMolodensky(createPropertiesForInverse(this, false, false),
                                 targetCRS(), sourceCRS(), negate(x), negate(y),
                                 negate(z), negate(da), negate(df),
                                 coordinateOperationAccuracies()));
        }
    }

    if (isLongitudeRotation()) {
        auto offset =
            parameterValueMeasure(EPSG_NAME_PARAMETER_LONGITUDE_OFFSET,
                                  EPSG_CODE_PARAMETER_LONGITUDE_OFFSET);
        const common::Angle newOffset(negate(offset.value()), offset.unit());
        return d->registerInv(
            shared_from_this(),
            createLongitudeRotation(
                createPropertiesForInverse(this, false, false), targetCRS(),
                sourceCRS(), newOffset));
    }

    if (ci_equal(method_name, EPSG_NAME_METHOD_GEOGRAPHIC2D_OFFSETS) ||
        method()->isEPSG(EPSG_CODE_METHOD_GEOGRAPHIC2D_OFFSETS)) {
        auto offsetLat =
            parameterValueMeasure(EPSG_NAME_PARAMETER_LATITUDE_OFFSET,
                                  EPSG_CODE_PARAMETER_LATITUDE_OFFSET);
        const common::Angle newOffsetLat(negate(offsetLat.value()),
                                         offsetLat.unit());

        auto offsetLong =
            parameterValueMeasure(EPSG_NAME_PARAMETER_LONGITUDE_OFFSET,
                                  EPSG_CODE_PARAMETER_LONGITUDE_OFFSET);
        const common::Angle newOffsetLong(negate(offsetLong.value()),
                                          offsetLong.unit());

        return d->registerInv(
            shared_from_this(),
            createGeographic2DOffsets(
                createPropertiesForInverse(this, false, false), targetCRS(),
                sourceCRS(), newOffsetLat, newOffsetLong,
                coordinateOperationAccuracies()));
    }

    if (ci_equal(method_name, EPSG_NAME_METHOD_GEOGRAPHIC3D_OFFSETS) ||
        method()->isEPSG(EPSG_CODE_METHOD_GEOGRAPHIC3D_OFFSETS)) {
        auto offsetLat =
            parameterValueMeasure(EPSG_NAME_PARAMETER_LATITUDE_OFFSET,
                                  EPSG_CODE_PARAMETER_LATITUDE_OFFSET);
        const common::Angle newOffsetLat(negate(offsetLat.value()),
                                         offsetLat.unit());

        auto offsetLong =
            parameterValueMeasure(EPSG_NAME_PARAMETER_LONGITUDE_OFFSET,
                                  EPSG_CODE_PARAMETER_LONGITUDE_OFFSET);
        const common::Angle newOffsetLong(negate(offsetLong.value()),
                                          offsetLong.unit());

        auto offsetHeight =
            parameterValueMeasure(EPSG_NAME_PARAMETER_VERTICAL_OFFSET,
                                  EPSG_CODE_PARAMETER_VERTICAL_OFFSET);
        const common::Length newOffsetHeight(negate(offsetHeight.value()),
                                             offsetHeight.unit());

        return d->registerInv(
            shared_from_this(),
            createGeographic3DOffsets(
                createPropertiesForInverse(this, false, false), targetCRS(),
                sourceCRS(), newOffsetLat, newOffsetLong, newOffsetHeight,
                coordinateOperationAccuracies()));
    }

    if (ci_equal(method_name,
                 EPSG_NAME_METHOD_GEOGRAPHIC2D_WITH_HEIGHT_OFFSETS) ||
        method()->isEPSG(EPSG_CODE_METHOD_GEOGRAPHIC2D_WITH_HEIGHT_OFFSETS)) {
        auto offsetLat =
            parameterValueMeasure(EPSG_NAME_PARAMETER_LATITUDE_OFFSET,
                                  EPSG_CODE_PARAMETER_LATITUDE_OFFSET);
        const common::Angle newOffsetLat(negate(offsetLat.value()),
                                         offsetLat.unit());

        auto offsetLong =
            parameterValueMeasure(EPSG_NAME_PARAMETER_LONGITUDE_OFFSET,
                                  EPSG_CODE_PARAMETER_LONGITUDE_OFFSET);
        const common::Angle newOffsetLong(negate(offsetLong.value()),
                                          offsetLong.unit());

        auto offsetHeight =
            parameterValueMeasure(EPSG_NAME_PARAMETER_GEOID_UNDULATION,
                                  EPSG_CODE_PARAMETER_GEOID_UNDULATION);
        const common::Length newOffsetHeight(negate(offsetHeight.value()),
                                             offsetHeight.unit());

        return d->registerInv(
            shared_from_this(),
            createGeographic2DWithHeightOffsets(
                createPropertiesForInverse(this, false, false), targetCRS(),
                sourceCRS(), newOffsetLat, newOffsetLong, newOffsetHeight,
                coordinateOperationAccuracies()));
    }

    if (ci_equal(method_name, EPSG_NAME_METHOD_VERTICAL_OFFSET) ||
        method()->isEPSG(EPSG_CODE_METHOD_VERTICAL_OFFSET)) {

        auto offsetHeight =
            parameterValueMeasure(EPSG_NAME_PARAMETER_VERTICAL_OFFSET,
                                  EPSG_CODE_PARAMETER_VERTICAL_OFFSET);
        const common::Length newOffsetHeight(negate(offsetHeight.value()),
                                             offsetHeight.unit());

        return d->registerInv(
            shared_from_this(),
            createVerticalOffset(createPropertiesForInverse(this, false, false),
                                 targetCRS(), sourceCRS(), newOffsetHeight,
                                 coordinateOperationAccuracies()));
    }

    if (ci_equal(method_name, EPSG_NAME_METHOD_GEOGRAPHIC_GEOCENTRIC) ||
        method()->isEPSG(EPSG_CODE_METHOD_GEOGRAPHIC_GEOCENTRIC)) {

        return d->registerInv(
            shared_from_this(),
            createGeographicGeocentric(
                createPropertiesForInverse(this, false, false), targetCRS(),
                sourceCRS()));
    }

    return InverseTransformation::create(NN_CHECK_ASSERT(
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

std::string
InverseTransformation::exportToWKT(io::WKTFormatterNNPtr formatter) const {

    auto approxInverse = createApproximateInverseIfPossible(
        util::nn_dynamic_pointer_cast<Transformation>(forwardOperation_).get());
    if (approxInverse) {
        return approxInverse->exportToWKT(formatter);
    }

    return Transformation::exportToWKT(formatter);
}

//! @endcond

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
std::string Transformation::exportToWKT(io::WKTFormatterNNPtr formatter) const {
    return exportTransformationToWKT(formatter);
}
//! @endcond

// ---------------------------------------------------------------------------

std::string SingleOperation::exportTransformationToWKT(
    io::WKTFormatterNNPtr formatter) const {
    const bool isWKT2 = formatter->version() == io::WKTFormatter::Version::WKT2;
    if (!isWKT2) {
        throw io::FormattingException(
            "Transformation can only be exported to WKT2");
    }

    assert(sourceCRS());
    assert(targetCRS());

    if (formatter->abridgedTransformation()) {
        formatter->startNode(io::WKTConstants::ABRIDGEDTRANSFORMATION,
                             !identifiers().empty());
    } else {
        formatter->startNode(io::WKTConstants::COORDINATEOPERATION,
                             !identifiers().empty());
    }

    formatter->addQuotedString(*(name()->description()));

    if (!formatter->abridgedTransformation()) {
        formatter->startNode(io::WKTConstants::SOURCECRS, false);
        sourceCRS()->exportToWKT(formatter);
        formatter->endNode();

        formatter->startNode(io::WKTConstants::TARGETCRS, false);
        targetCRS()->exportToWKT(formatter);
        formatter->endNode();
    }

    method()->exportToWKT(formatter);

    const MethodMapping *mapping =
        !isWKT2 ? getMapping(method().get()) : nullptr;
    for (const auto &paramValue : parameterValues()) {
        paramValue->_exportToWKT(formatter, mapping);
    }

    if (!formatter->abridgedTransformation()) {
        if (interpolationCRS()) {
            formatter->startNode(io::WKTConstants::INTERPOLATIONCRS, false);
            interpolationCRS()->exportToWKT(formatter);
            formatter->endNode();
        }

        if (!coordinateOperationAccuracies().empty()) {
            formatter->startNode(io::WKTConstants::OPERATIONACCURACY, false);
            formatter->add(coordinateOperationAccuracies()[0]->value());
            formatter->endNode();
        }
    }

    ObjectUsage::_exportToWKT(formatter);
    formatter->endNode();

    return formatter->toString();
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
static std::string _getNTv2Filename(const Transformation *op,
                                    bool allowInverse) {

    auto method_name = *(op->method()->name()->description());
    if (ci_equal(method_name, EPSG_NAME_METHOD_NTV2) ||
        op->method()->isEPSG(EPSG_CODE_METHOD_NTV2) ||
        (allowInverse &&
         ci_equal(method_name, INVERSE_OF + EPSG_NAME_METHOD_NTV2))) {
        auto fileParameter = op->parameterValue(
            EPSG_NAME_PARAMETER_LATITUDE_LONGITUDE_DIFFERENCE_FILE,
            EPSG_CODE_PARAMETER_LATITUDE_LONGITUDE_DIFFERENCE_FILE);
        if (fileParameter &&
            fileParameter->type() == ParameterValue::Type::FILENAME) {
            return fileParameter->valueFile();
        }
    }
    return std::string();
}
//! @endcond

// ---------------------------------------------------------------------------
//! @cond Doxygen_Suppress
std::string Transformation::getNTv2Filename() const {

    return _getNTv2Filename(this, false);
}
//! @endcond

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
static std::string _getNTv1Filename(const Transformation *op,
                                    bool allowInverse) {

    auto method_name = *(op->method()->name()->description());
    if (ci_equal(method_name, EPSG_NAME_METHOD_NTV1) ||
        op->method()->isEPSG(EPSG_CODE_METHOD_NTV1) ||
        (allowInverse &&
         ci_equal(method_name, INVERSE_OF + EPSG_NAME_METHOD_NTV1))) {
        auto fileParameter = op->parameterValue(
            EPSG_NAME_PARAMETER_LATITUDE_LONGITUDE_DIFFERENCE_FILE,
            EPSG_CODE_PARAMETER_LATITUDE_LONGITUDE_DIFFERENCE_FILE);
        if (fileParameter &&
            fileParameter->type() == ParameterValue::Type::FILENAME) {
            return fileParameter->valueFile();
        }
    }
    return std::string();
}
//! @endcond

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
static std::string _getCTABLE2Filename(const Transformation *op,
                                       bool allowInverse) {

    auto method_name = *(op->method()->name()->description());
    if (ci_equal(method_name, PROJ_WKT2_NAME_METHOD_CTABLE2) ||
        (allowInverse &&
         ci_equal(method_name, INVERSE_OF + PROJ_WKT2_NAME_METHOD_CTABLE2))) {
        auto fileParameter = op->parameterValue(
            EPSG_NAME_PARAMETER_LATITUDE_LONGITUDE_DIFFERENCE_FILE,
            EPSG_CODE_PARAMETER_LATITUDE_LONGITUDE_DIFFERENCE_FILE);
        if (fileParameter &&
            fileParameter->type() == ParameterValue::Type::FILENAME) {
            return fileParameter->valueFile();
        }
    }
    return std::string();
}
//! @endcond

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
static std::string _getHeightToGeographic3DFilename(const Transformation *op,
                                                    bool allowInverse) {

    auto method_name = *(op->method()->name()->description());

    if (ci_equal(method_name, PROJ_WKT2_NAME_METHOD_HEIGHT_TO_GEOG3D) ||
        (allowInverse &&
         ci_equal(method_name,
                  INVERSE_OF + PROJ_WKT2_NAME_METHOD_HEIGHT_TO_GEOG3D))) {
        auto fileParameter =
            op->parameterValue(EPSG_NAME_PARAMETER_GEOID_CORRECTION_FILENAME,
                               EPSG_CODE_PARAMETER_GEOID_CORRECTION_FILENAME);
        if (fileParameter &&
            fileParameter->type() == ParameterValue::Type::FILENAME) {
            return fileParameter->valueFile();
        }
    }
    return std::string();
}
//! @endcond

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
std::string Transformation::getHeightToGeographic3DFilename() const {

    return _getHeightToGeographic3DFilename(this, false);
}
//! @endcond

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
static bool
isGeographic3DToGravityRelatedHeight(const OperationMethodNNPtr &method,
                                     bool allowInverse) {
    auto method_name = *(method->name()->description());
    static const std::vector<std::string> methodCodes = {
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
        "9661", // Geographic3D to GravityRelatedHeight (EGM)
        "9662", // Geographic3D to GravityRelatedHeight (Ausgeoid98)
        "9663", // Geographic3D to GravityRelatedHeight (OSGM-GB)
        "9664", // Geographic3D to GravityRelatedHeight (IGN1997)
        "9665", // Geographic3D to GravityRelatedHeight (US .gtx)
    };

    if (ci_find(method_name, "Geographic3D to GravityRelatedHeight") == 0) {
        return true;
    }
    if (allowInverse &&
        ci_find(method_name,
                INVERSE_OF + "Geographic3D to GravityRelatedHeight") == 0) {
        return true;
    }

    for (const auto &code : methodCodes) {
        for (const auto &idSrc : method->identifiers()) {
            auto srcAuthName = *(idSrc->codeSpace());
            auto srcCode = idSrc->code();
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
static util::PropertyMap
createSimilarPropertiesMethod(common::IdentifiedObjectNNPtr obj) {
    util::PropertyMap map;

    std::string forwardName = *obj->name()->description();
    if (!forwardName.empty()) {
        map.set(common::IdentifiedObject::NAME_KEY, forwardName);
    }

    {
        auto ar = util::ArrayOfBaseObject::create();
        for (const auto &idSrc : obj->identifiers()) {
            auto srcAuthName = *(idSrc->codeSpace());
            auto srcCode = idSrc->code();
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
    {
        auto ar = util::ArrayOfBaseObject::create();
        for (const auto &domain : obj->domains()) {
            ar->add(domain);
        }
        if (!ar->empty()) {
            map.set(common::ObjectUsage::OBJECT_DOMAIN_KEY, ar);
        }
    }

    std::string forwardName = *obj->name()->description();
    if (!forwardName.empty()) {
        map.set(common::IdentifiedObject::NAME_KEY, forwardName);
    }

    // If original operation is AUTH:CODE, then assign DERIVED_FROM(AUTH):CODE
    // as identifier.
    {
        auto ar = util::ArrayOfBaseObject::create();
        for (const auto &idSrc : obj->identifiers()) {
            auto srcAuthName = *(idSrc->codeSpace());
            auto srcCode = idSrc->code();
            auto idsProp =
                util::PropertyMap().set(metadata::Identifier::CODESPACE_KEY,
                                        "DERIVED_FROM(" + srcAuthName + ")");
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
static TransformationNNPtr
createNTv1(const util::PropertyMap &properties,
           const crs::CRSNNPtr &sourceCRSIn, const crs::CRSNNPtr &targetCRSIn,
           const std::string &filename,
           const std::vector<metadata::PositionalAccuracyNNPtr> &accuracies) {
    return Transformation::create(
        properties, sourceCRSIn, targetCRSIn, nullptr,
        util::PropertyMap()
            .set(common::IdentifiedObject::NAME_KEY, EPSG_NAME_METHOD_NTV1)
            .set(metadata::Identifier::CODESPACE_KEY,
                 metadata::Identifier::EPSG)
            .set(metadata::Identifier::CODE_KEY, EPSG_CODE_METHOD_NTV1),
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
    auto self = NN_CHECK_ASSERT(std::dynamic_pointer_cast<Transformation>(
        shared_from_this().as_nullable()));

    auto method_name = *(method()->name()->description());

    std::string projFilename;
    std::string projGridFormat;
    bool inverseDirection = false;

    auto NTv1Filename = _getNTv1Filename(this, false);
    auto NTv2Filename = _getNTv2Filename(this, false);
    std::string lasFilename;
    if (ci_equal(method_name, EPSG_NAME_METHOD_NADCON) ||
        method()->isEPSG(EPSG_CODE_METHOD_NADCON)) {
        auto latitudeFileParameter =
            parameterValue(EPSG_NAME_PARAMETER_LATITUDE_DIFFERENCE_FILE,
                           EPSG_CODE_PARAMETER_LATITUDE_DIFFERENCE_FILE);
        auto longitudeFileParameter =
            parameterValue(EPSG_NAME_PARAMETER_LONGITUDE_DIFFERENCE_FILE,
                           EPSG_CODE_PARAMETER_LONGITUDE_DIFFERENCE_FILE);
        if (latitudeFileParameter &&
            latitudeFileParameter->type() == ParameterValue::Type::FILENAME &&
            longitudeFileParameter &&
            longitudeFileParameter->type() == ParameterValue::Type::FILENAME) {
            lasFilename = latitudeFileParameter->valueFile();
        }
    }
    auto horizontalGridName =
        !NTv1Filename.empty() ? NTv1Filename : !NTv2Filename.empty()
                                                   ? NTv2Filename
                                                   : lasFilename;

    if (!horizontalGridName.empty() &&
        databaseContext->lookForGridAlternative(horizontalGridName,
                                                projFilename, projGridFormat,
                                                inverseDirection)) {

        if (horizontalGridName == projFilename) {
            assert(!inverseDirection);
            return self;
        }

        if (projGridFormat == "NTv1") {
            if (inverseDirection) {
                return createNTv1(createPropertiesForInverse(
                                      self.as_nullable().get(), true, false),
                                  targetCRS(), sourceCRS(), projFilename,
                                  coordinateOperationAccuracies())
                    ->inverseAsTransformation();
            } else {
                return createNTv1(createSimilarPropertiesTransformation(self),
                                  sourceCRS(), targetCRS(), projFilename,
                                  coordinateOperationAccuracies());
            }
        } else if (projGridFormat == "NTv2") {
            if (inverseDirection) {
                return createNTv2(createPropertiesForInverse(
                                      self.as_nullable().get(), true, false),
                                  targetCRS(), sourceCRS(), projFilename,
                                  coordinateOperationAccuracies())
                    ->inverseAsTransformation();
            } else {
                return createNTv2(createSimilarPropertiesTransformation(self),
                                  sourceCRS(), targetCRS(), projFilename,
                                  coordinateOperationAccuracies());
            }
        } else if (projGridFormat == "CTable2") {
            auto parameters = std::vector<
                OperationParameterNNPtr>{OperationParameter::create(
                util::PropertyMap()
                    .set(common::IdentifiedObject::NAME_KEY,
                         EPSG_NAME_PARAMETER_LATITUDE_LONGITUDE_DIFFERENCE_FILE)
                    .set(metadata::Identifier::CODESPACE_KEY,
                         metadata::Identifier::EPSG)
                    .set(
                        metadata::Identifier::CODE_KEY,
                        EPSG_CODE_PARAMETER_LATITUDE_LONGITUDE_DIFFERENCE_FILE))};
            if (inverseDirection) {
                return create(createPropertiesForInverse(
                                  self.as_nullable().get(), true, false),
                              targetCRS(), sourceCRS(), nullptr,
                              util::PropertyMap().set(
                                  common::IdentifiedObject::NAME_KEY,
                                  PROJ_WKT2_NAME_METHOD_CTABLE2),
                              parameters,
                              {ParameterValue::createFilename(projFilename)},
                              coordinateOperationAccuracies())
                    ->inverseAsTransformation();

            } else {
                return create(
                    createSimilarPropertiesTransformation(self), sourceCRS(),
                    targetCRS(), nullptr,
                    util::PropertyMap().set(common::IdentifiedObject::NAME_KEY,
                                            PROJ_WKT2_NAME_METHOD_CTABLE2),
                    parameters, {ParameterValue::createFilename(projFilename)},
                    coordinateOperationAccuracies());
            }
        }
    }

    auto heightFilename = getHeightToGeographic3DFilename();
    if (!heightFilename.empty() && !projFilename.empty()) {
        if (databaseContext->lookForGridAlternative(
                heightFilename, projFilename, projGridFormat,
                inverseDirection)) {

            if (heightFilename == projFilename) {
                assert(!inverseDirection);
                return self;
            }

            if (inverseDirection) {
                return createGravityRelatedHeightToGeographic3D(
                           createPropertiesForInverse(self.as_nullable().get(),
                                                      true, false),
                           targetCRS(), sourceCRS(), projFilename,
                           coordinateOperationAccuracies())
                    ->inverseAsTransformation();
            } else {
                return createGravityRelatedHeightToGeographic3D(
                    createSimilarPropertiesTransformation(self), sourceCRS(),
                    targetCRS(), projFilename, coordinateOperationAccuracies());
            }
        }
    }

    if (isGeographic3DToGravityRelatedHeight(method(), false)) {
        auto fileParameter =
            parameterValue(EPSG_NAME_PARAMETER_GEOID_CORRECTION_FILENAME,
                           EPSG_CODE_PARAMETER_GEOID_CORRECTION_FILENAME);
        if (fileParameter &&
            fileParameter->type() == ParameterValue::Type::FILENAME) {
            auto filename = fileParameter->valueFile();
            if (databaseContext->lookForGridAlternative(
                    filename, projFilename, projGridFormat, inverseDirection)) {

                if (filename == projFilename) {
                    assert(!inverseDirection);
                    return self;
                }

                auto parameters = std::vector<
                    OperationParameterNNPtr>{OperationParameter::create(
                    util::PropertyMap()
                        .set(common::IdentifiedObject::NAME_KEY,
                             EPSG_NAME_PARAMETER_GEOID_CORRECTION_FILENAME)
                        .set(metadata::Identifier::CODESPACE_KEY,
                             metadata::Identifier::EPSG)
                        .set(metadata::Identifier::CODE_KEY,
                             EPSG_CODE_PARAMETER_GEOID_CORRECTION_FILENAME))};
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

std::string Transformation::exportToPROJString(
    io::PROJStringFormatterNNPtr formatter) const // throw(FormattingException)
{
    if (formatter->convention() ==
        io::PROJStringFormatter::Convention::PROJ_4) {
        throw io::FormattingException(
            "Transformation cannot be exported as a PROJ.4 string");
    }

    bool positionVectorConvention = true;
    bool sevenParamsTransform = false;
    bool threeParamsTransform = false;
    bool fifteenParamsTransform = false;
    auto method_name = *(method()->name()->description());
    if ((ci_find(method_name, "Coordinate Frame") != std::string::npos &&
         !isTimeDependent(method_name)) ||
        method()->isEPSG(EPSG_CODE_METHOD_COORDINATE_FRAME_GEOCENTRIC) ||
        method()->isEPSG(EPSG_CODE_METHOD_COORDINATE_FRAME_GEOGRAPHIC_2D) ||
        method()->isEPSG(EPSG_CODE_METHOD_COORDINATE_FRAME_GEOGRAPHIC_3D)) {
        positionVectorConvention = false;
        sevenParamsTransform = true;
    } else if (
        (ci_find(method_name, "Coordinate Frame") != std::string::npos &&
         isTimeDependent(method_name)) ||
        method()->isEPSG(
            EPSG_CODE_METHOD_TIME_DEPENDENT_COORDINATE_FRAME_GEOCENTRIC) ||
        method()->isEPSG(
            EPSG_CODE_METHOD_TIME_DEPENDENT_COORDINATE_FRAME_GEOGRAPHIC_2D) ||
        method()->isEPSG(
            EPSG_CODE_METHOD_TIME_DEPENDENT_COORDINATE_FRAME_GEOGRAPHIC_3D)) {
        positionVectorConvention = false;
        fifteenParamsTransform = true;
    } else if ((ci_find(method_name, "Position Vector") != std::string::npos &&
                !isTimeDependent(method_name)) ||
               method()->isEPSG(EPSG_CODE_METHOD_POSITION_VECTOR_GEOCENTRIC) ||
               method()->isEPSG(
                   EPSG_CODE_METHOD_POSITION_VECTOR_GEOGRAPHIC_2D) ||
               method()->isEPSG(
                   EPSG_CODE_METHOD_POSITION_VECTOR_GEOGRAPHIC_3D)) {
        sevenParamsTransform = true;
    } else if (
        (ci_find(method_name, "Position Vector") != std::string::npos &&
         isTimeDependent(method_name)) ||
        method()->isEPSG(
            EPSG_CODE_METHOD_TIME_DEPENDENT_POSITION_VECTOR_GEOCENTRIC) ||
        method()->isEPSG(
            EPSG_CODE_METHOD_TIME_DEPENDENT_POSITION_VECTOR_GEOGRAPHIC_2D) ||
        method()->isEPSG(
            EPSG_CODE_METHOD_TIME_DEPENDENT_POSITION_VECTOR_GEOGRAPHIC_3D)) {
        fifteenParamsTransform = true;
    } else if (ci_find(method_name, "Geocentric translations") !=
                   std::string::npos ||
               method()->isEPSG(
                   EPSG_CODE_METHOD_GEOCENTRIC_TRANSLATION_GEOCENTRIC) ||
               method()->isEPSG(
                   EPSG_CODE_METHOD_GEOCENTRIC_TRANSLATION_GEOGRAPHIC_2D) ||
               method()->isEPSG(
                   EPSG_CODE_METHOD_GEOCENTRIC_TRANSLATION_GEOGRAPHIC_3D)) {
        threeParamsTransform = true;
    }
    if (threeParamsTransform || sevenParamsTransform ||
        fifteenParamsTransform) {
        double x = parameterValueMeasure(EPSG_NAME_PARAMETER_X_AXIS_TRANSLATION,
                                         EPSG_CODE_PARAMETER_X_AXIS_TRANSLATION)
                       .getSIValue();
        double y = parameterValueMeasure(EPSG_NAME_PARAMETER_Y_AXIS_TRANSLATION,
                                         EPSG_CODE_PARAMETER_Y_AXIS_TRANSLATION)
                       .getSIValue();
        double z = parameterValueMeasure(EPSG_NAME_PARAMETER_Z_AXIS_TRANSLATION,
                                         EPSG_CODE_PARAMETER_Z_AXIS_TRANSLATION)
                       .getSIValue();

        io::PROJStringFormatter::Scope scope(formatter);

        auto sourceCRSGeog =
            util::nn_dynamic_pointer_cast<crs::GeographicCRS>(sourceCRS());
        if (sourceCRSGeog) {
            formatter->startInversion();
            sourceCRSGeog->exportToPROJString(formatter);
            formatter->stopInversion();

            formatter->addStep("cart");
            sourceCRSGeog->ellipsoid()->exportToPROJString(formatter);
        } else {
            auto sourceCRSGeod =
                util::nn_dynamic_pointer_cast<crs::GeodeticCRS>(sourceCRS());
            if (!sourceCRSGeod) {
                throw io::FormattingException("Can apply Helmert only to "
                                              "GeodeticCRS / "
                                              "GeographicCRS");
            }
            formatter->startInversion();
            sourceCRSGeod->addGeocentricUnitConversionIntoPROJString(formatter);
            formatter->stopInversion();
        }

        formatter->addStep("helmert");
        formatter->addParam("x", x);
        formatter->addParam("y", y);
        formatter->addParam("z", z);
        if (sevenParamsTransform || fifteenParamsTransform) {
            double rx =
                parameterValueMeasure(EPSG_NAME_PARAMETER_X_AXIS_ROTATION,
                                      EPSG_CODE_PARAMETER_X_AXIS_ROTATION)
                    .convertToUnit(common::UnitOfMeasure::ARC_SECOND)
                    .value();
            double ry =
                parameterValueMeasure(EPSG_NAME_PARAMETER_Y_AXIS_ROTATION,
                                      EPSG_CODE_PARAMETER_Y_AXIS_ROTATION)
                    .convertToUnit(common::UnitOfMeasure::ARC_SECOND)
                    .value();
            double rz =
                parameterValueMeasure(EPSG_NAME_PARAMETER_Z_AXIS_ROTATION,
                                      EPSG_CODE_PARAMETER_Z_AXIS_ROTATION)
                    .convertToUnit(common::UnitOfMeasure::ARC_SECOND)
                    .value();
            double scaleDiff =
                parameterValueMeasure(EPSG_NAME_PARAMETER_SCALE_DIFFERENCE,
                                      EPSG_CODE_PARAMETER_SCALE_DIFFERENCE)
                    .convertToUnit(common::UnitOfMeasure::PARTS_PER_MILLION)
                    .value();
            formatter->addParam("rx", rx);
            formatter->addParam("ry", ry);
            formatter->addParam("rz", rz);
            formatter->addParam("s", scaleDiff);
            if (fifteenParamsTransform) {
                double rate_x =
                    parameterValueMeasure(
                        EPSG_NAME_PARAMETER_RATE_X_AXIS_TRANSLATION,
                        EPSG_CODE_PARAMETER_RATE_X_AXIS_TRANSLATION)
                        .convertToUnit(common::UnitOfMeasure::METRE_PER_YEAR)
                        .value();
                double rate_y =
                    parameterValueMeasure(
                        EPSG_NAME_PARAMETER_RATE_Y_AXIS_TRANSLATION,
                        EPSG_CODE_PARAMETER_RATE_Y_AXIS_TRANSLATION)
                        .convertToUnit(common::UnitOfMeasure::METRE_PER_YEAR)
                        .value();
                double rate_z =
                    parameterValueMeasure(
                        EPSG_NAME_PARAMETER_RATE_Z_AXIS_TRANSLATION,
                        EPSG_CODE_PARAMETER_RATE_Z_AXIS_TRANSLATION)
                        .convertToUnit(common::UnitOfMeasure::METRE_PER_YEAR)
                        .value();
                double rate_rx =
                    parameterValueMeasure(
                        EPSG_NAME_PARAMETER_RATE_X_AXIS_ROTATION,
                        EPSG_CODE_PARAMETER_RATE_X_AXIS_ROTATION)
                        .convertToUnit(
                            common::UnitOfMeasure::ARC_SECOND_PER_YEAR)
                        .value();
                double rate_ry =
                    parameterValueMeasure(
                        EPSG_NAME_PARAMETER_RATE_Y_AXIS_ROTATION,
                        EPSG_CODE_PARAMETER_RATE_Y_AXIS_ROTATION)
                        .convertToUnit(
                            common::UnitOfMeasure::ARC_SECOND_PER_YEAR)
                        .value();
                double rate_rz =
                    parameterValueMeasure(
                        EPSG_NAME_PARAMETER_RATE_Z_AXIS_ROTATION,
                        EPSG_CODE_PARAMETER_RATE_Z_AXIS_ROTATION)
                        .convertToUnit(
                            common::UnitOfMeasure::ARC_SECOND_PER_YEAR)
                        .value();
                double rate_scaleDiff =
                    parameterValueMeasure(
                        EPSG_NAME_PARAMETER_RATE_SCALE_DIFFERENCE,
                        EPSG_CODE_PARAMETER_RATE_SCALE_DIFFERENCE)
                        .convertToUnit(common::UnitOfMeasure::PPM_PER_YEAR)
                        .value();
                double referenceEpochYear =
                    parameterValueMeasure(EPSG_NAME_PARAMETER_REFERENCE_EPOCH,
                                          EPSG_CODE_PARAMETER_REFERENCE_EPOCH)
                        .convertToUnit(common::UnitOfMeasure::YEAR)
                        .value();
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
                formatter->addParam("convention=position_vector");
            } else {
                formatter->addParam("convention=coordinate_frame");
            }
        }

        auto targetCRSGeog =
            util::nn_dynamic_pointer_cast<crs::GeographicCRS>(targetCRS());
        if (targetCRSGeog) {
            formatter->addStep("cart");
            formatter->setCurrentStepInverted(true);
            targetCRSGeog->ellipsoid()->exportToPROJString(formatter);

            targetCRSGeog->exportToPROJString(formatter);
        } else {
            auto targetCRSGeod =
                util::nn_dynamic_pointer_cast<crs::GeodeticCRS>(targetCRS());
            if (!targetCRSGeod) {
                throw io::FormattingException("Can apply Helmert only to "
                                              "GeodeticCRS / "
                                              "GeographicCRS");
            }
            targetCRSGeod->addGeocentricUnitConversionIntoPROJString(formatter);
        }

        return scope.toString();
    }

    if (ci_find(method_name, "Molodensky") != std::string::npos ||
        method()->isEPSG(EPSG_CODE_METHOD_MOLODENSKY) ||
        method()->isEPSG(EPSG_CODE_METHOD_ABRIDGED_MOLODENSKY)) {
        double x = parameterValueMeasure(EPSG_NAME_PARAMETER_X_AXIS_TRANSLATION,
                                         EPSG_CODE_PARAMETER_X_AXIS_TRANSLATION)
                       .getSIValue();
        double y = parameterValueMeasure(EPSG_NAME_PARAMETER_Y_AXIS_TRANSLATION,
                                         EPSG_CODE_PARAMETER_Y_AXIS_TRANSLATION)
                       .getSIValue();
        double z = parameterValueMeasure(EPSG_NAME_PARAMETER_Z_AXIS_TRANSLATION,
                                         EPSG_CODE_PARAMETER_Z_AXIS_TRANSLATION)
                       .getSIValue();
        double da = parameterValueMeasure(
                        EPSG_NAME_PARAMETER_SEMI_MAJOR_AXIS_DIFFERENCE,
                        EPSG_CODE_PARAMETER_SEMI_MAJOR_AXIS_DIFFERENCE)
                        .getSIValue();
        double df =
            parameterValueMeasure(EPSG_NAME_PARAMETER_FLATTENING_DIFFERENCE,
                                  EPSG_CODE_PARAMETER_FLATTENING_DIFFERENCE)
                .getSIValue();

        auto sourceCRSGeog =
            util::nn_dynamic_pointer_cast<crs::GeographicCRS>(sourceCRS());
        if (!sourceCRSGeog) {
            throw io::FormattingException(
                "Can apply Molodensky only to GeographicCRS");
        }

        auto targetCRSGeog =
            util::nn_dynamic_pointer_cast<crs::GeographicCRS>(targetCRS());
        if (!targetCRSGeog) {
            throw io::FormattingException(
                "Can apply Molodensky only to GeographicCRS");
        }

        io::PROJStringFormatter::Scope scope(formatter);

        formatter->startInversion();
        sourceCRSGeog->exportToPROJString(formatter);
        formatter->stopInversion();

        formatter->addStep("molodensky");
        sourceCRSGeog->ellipsoid()->exportToPROJString(formatter);
        formatter->addParam("dx", x);
        formatter->addParam("dy", y);
        formatter->addParam("dz", z);
        formatter->addParam("da", da);
        formatter->addParam("df", df);

        if (ci_find(method_name, "Abridged") != std::string::npos ||
            method()->isEPSG(EPSG_CODE_METHOD_ABRIDGED_MOLODENSKY)) {
            formatter->addParam("abridged");
        }

        targetCRSGeog->exportToPROJString(formatter);

        return scope.toString();
    }

    if (ci_equal(method_name, EPSG_NAME_METHOD_GEOGRAPHIC2D_OFFSETS) ||
        method()->isEPSG(EPSG_CODE_METHOD_GEOGRAPHIC2D_OFFSETS)) {
        double offsetLat =
            parameterValueMeasure(EPSG_NAME_PARAMETER_LATITUDE_OFFSET,
                                  EPSG_CODE_PARAMETER_LATITUDE_OFFSET)
                .convertToUnit(common::UnitOfMeasure::ARC_SECOND)
                .value();
        double offsetLong =
            parameterValueMeasure(EPSG_NAME_PARAMETER_LONGITUDE_OFFSET,
                                  EPSG_CODE_PARAMETER_LONGITUDE_OFFSET)
                .convertToUnit(common::UnitOfMeasure::ARC_SECOND)
                .value();

        auto sourceCRSGeog =
            util::nn_dynamic_pointer_cast<crs::GeographicCRS>(sourceCRS());
        if (!sourceCRSGeog) {
            throw io::FormattingException(
                "Can apply Geographic 2D offsets only to GeographicCRS");
        }

        auto targetCRSGeog =
            util::nn_dynamic_pointer_cast<crs::GeographicCRS>(targetCRS());
        if (!targetCRSGeog) {
            throw io::FormattingException(
                "Can apply Geographic 2D offsets only to GeographicCRS");
        }

        io::PROJStringFormatter::Scope scope(formatter);

        formatter->startInversion();
        sourceCRSGeog->addAngularUnitConvertAndAxisSwap(formatter);
        formatter->stopInversion();

        if (offsetLat != 0.0 || offsetLong != 0.0) {
            formatter->addStep("geogoffset");
            formatter->addParam("dlat", offsetLat);
            formatter->addParam("dlon", offsetLong);
        }

        targetCRSGeog->addAngularUnitConvertAndAxisSwap(formatter);

        return scope.toString();
    }

    if (ci_equal(method_name, EPSG_NAME_METHOD_GEOGRAPHIC3D_OFFSETS) ||
        method()->isEPSG(EPSG_CODE_METHOD_GEOGRAPHIC3D_OFFSETS)) {
        double offsetLat =
            parameterValueMeasure(EPSG_NAME_PARAMETER_LATITUDE_OFFSET,
                                  EPSG_CODE_PARAMETER_LATITUDE_OFFSET)
                .convertToUnit(common::UnitOfMeasure::ARC_SECOND)
                .value();
        double offsetLong =
            parameterValueMeasure(EPSG_NAME_PARAMETER_LONGITUDE_OFFSET,
                                  EPSG_CODE_PARAMETER_LONGITUDE_OFFSET)
                .convertToUnit(common::UnitOfMeasure::ARC_SECOND)
                .value();
        double offsetHeight =
            parameterValueMeasure(EPSG_NAME_PARAMETER_VERTICAL_OFFSET,
                                  EPSG_CODE_PARAMETER_VERTICAL_OFFSET)
                .getSIValue();

        auto sourceCRSGeog =
            util::nn_dynamic_pointer_cast<crs::GeographicCRS>(sourceCRS());
        if (!sourceCRSGeog) {
            throw io::FormattingException(
                "Can apply Geographic 3D offsets only to GeographicCRS");
        }

        auto targetCRSGeog =
            util::nn_dynamic_pointer_cast<crs::GeographicCRS>(targetCRS());
        if (!targetCRSGeog) {
            throw io::FormattingException(
                "Can apply Geographic 3D offsets only to GeographicCRS");
        }

        io::PROJStringFormatter::Scope scope(formatter);

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

        return scope.toString();
    }

    if (ci_equal(method_name,
                 EPSG_NAME_METHOD_GEOGRAPHIC2D_WITH_HEIGHT_OFFSETS) ||
        method()->isEPSG(EPSG_CODE_METHOD_GEOGRAPHIC2D_WITH_HEIGHT_OFFSETS)) {
        double offsetLat =
            parameterValueMeasure(EPSG_NAME_PARAMETER_LATITUDE_OFFSET,
                                  EPSG_CODE_PARAMETER_LATITUDE_OFFSET)
                .convertToUnit(common::UnitOfMeasure::ARC_SECOND)
                .value();
        double offsetLong =
            parameterValueMeasure(EPSG_NAME_PARAMETER_LONGITUDE_OFFSET,
                                  EPSG_CODE_PARAMETER_LONGITUDE_OFFSET)
                .convertToUnit(common::UnitOfMeasure::ARC_SECOND)
                .value();
        double offsetHeight =
            parameterValueMeasure(EPSG_NAME_PARAMETER_GEOID_UNDULATION,
                                  EPSG_CODE_PARAMETER_GEOID_UNDULATION)
                .getSIValue();

        auto sourceCRSGeog =
            util::nn_dynamic_pointer_cast<crs::GeographicCRS>(sourceCRS());
        if (!sourceCRSGeog) {
            auto sourceCRSCompound =
                util::nn_dynamic_pointer_cast<crs::CompoundCRS>(sourceCRS());
            if (sourceCRSCompound) {
                sourceCRSGeog = sourceCRSCompound->extractGeographicCRS();
            }
            if (!sourceCRSGeog) {
                throw io::FormattingException("Can apply Geographic 2D with "
                                              "height offsets only to "
                                              "GeographicCRS / CompoundCRS");
            }
        }

        auto targetCRSGeog =
            util::nn_dynamic_pointer_cast<crs::GeographicCRS>(targetCRS());
        if (!targetCRSGeog) {
            auto targetCRSCompound =
                util::nn_dynamic_pointer_cast<crs::CompoundCRS>(targetCRS());
            if (targetCRSCompound) {
                targetCRSGeog = targetCRSCompound->extractGeographicCRS();
            }
            if (!targetCRSGeog) {
                throw io::FormattingException("Can apply Geographic 2D with "
                                              "height offsets only to "
                                              "GeographicCRS / CompoundCRS");
            }
        }

        io::PROJStringFormatter::Scope scope(formatter);

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

        return scope.toString();
    }

    if (ci_equal(method_name, EPSG_NAME_METHOD_VERTICAL_OFFSET) ||
        method()->isEPSG(EPSG_CODE_METHOD_VERTICAL_OFFSET)) {

        auto sourceCRSVert =
            util::nn_dynamic_pointer_cast<crs::VerticalCRS>(sourceCRS());
        if (!sourceCRSVert) {
            throw io::FormattingException(
                "Can apply Vertical offset only to VerticalCRS");
        }

        auto targetCRSVert =
            util::nn_dynamic_pointer_cast<crs::VerticalCRS>(targetCRS());
        if (!targetCRSVert) {
            throw io::FormattingException(
                "Can apply Vertical offset only to VerticalCRS");
        }

        auto offsetHeight =
            parameterValueMeasure(EPSG_NAME_PARAMETER_VERTICAL_OFFSET,
                                  EPSG_CODE_PARAMETER_VERTICAL_OFFSET)
                .getSIValue();

        io::PROJStringFormatter::Scope scope(formatter);

        formatter->startInversion();
        sourceCRSVert->addLinearUnitConvert(formatter);
        formatter->stopInversion();

        formatter->addStep("geogoffset");
        formatter->addParam("dh", offsetHeight);

        targetCRSVert->addLinearUnitConvert(formatter);

        return scope.toString();
    }

    // Substitute grid names with PROJ friendly names.
    if (formatter->databaseContext()) {
        auto alternate = substitutePROJAlternativeGridNames(
            NN_CHECK_ASSERT(formatter->databaseContext()));
        auto self = NN_CHECK_ASSERT(std::dynamic_pointer_cast<Transformation>(
            shared_from_this().as_nullable()));

        if (alternate != self) {
            return alternate->exportToPROJString(formatter);
        }
    }

    auto NTv1Filename = _getNTv1Filename(this, true);
    auto NTv2Filename = _getNTv2Filename(this, true);
    auto CTABLE2Filename = _getCTABLE2Filename(this, true);
    auto hGridShiftFilename =
        !NTv1Filename.empty() ? NTv1Filename : !NTv2Filename.empty()
                                                   ? NTv2Filename
                                                   : CTABLE2Filename;
    if (!hGridShiftFilename.empty()) {
        auto sourceCRSGeog =
            util::nn_dynamic_pointer_cast<crs::GeographicCRS>(sourceCRS());
        if (!sourceCRSGeog) {
            throw io::FormattingException("Can apply " + method_name +
                                          " only to GeographicCRS");
        }

        auto targetCRSGeog =
            util::nn_dynamic_pointer_cast<crs::GeographicCRS>(targetCRS());
        if (!targetCRSGeog) {
            throw io::FormattingException("Can apply " + method_name +
                                          " only to GeographicCRS");
        }

        io::PROJStringFormatter::Scope scope(formatter);

        formatter->startInversion();
        sourceCRSGeog->addAngularUnitConvertAndAxisSwap(formatter);
        formatter->stopInversion();

        if (starts_with(method_name, INVERSE_OF)) {
            formatter->startInversion();
        }
        formatter->addStep("hgridshift");
        formatter->addParam("grids", hGridShiftFilename);
        if (starts_with(method_name, INVERSE_OF)) {
            formatter->stopInversion();
        }

        targetCRSGeog->addAngularUnitConvertAndAxisSwap(formatter);

        return scope.toString();
    }

    auto heightFilename = _getHeightToGeographic3DFilename(this, true);
    if (!heightFilename.empty()) {
        io::PROJStringFormatter::Scope scope(formatter);
        if (starts_with(method_name, INVERSE_OF)) {
            formatter->startInversion();
        }
        formatter->addStep("vgridshift");
        formatter->addParam("grids", heightFilename);
        if (starts_with(method_name, INVERSE_OF)) {
            formatter->stopInversion();
        }
        return scope.toString();
    }

    if (isGeographic3DToGravityRelatedHeight(method(), true)) {
        auto fileParameter =
            parameterValue(EPSG_NAME_PARAMETER_GEOID_CORRECTION_FILENAME,
                           EPSG_CODE_PARAMETER_GEOID_CORRECTION_FILENAME);
        if (fileParameter &&
            fileParameter->type() == ParameterValue::Type::FILENAME) {
            auto filename = fileParameter->valueFile();
            io::PROJStringFormatter::Scope scope(formatter);
            if (starts_with(method_name, INVERSE_OF)) {
                formatter->startInversion();
            }
            formatter->addStep("vgridshift");
            formatter->addParam("grids", filename);
            if (starts_with(method_name, INVERSE_OF)) {
                formatter->stopInversion();
            }
            return scope.toString();
        }
    }

    if (ci_equal(method_name, EPSG_NAME_METHOD_VERTCON) ||
        method()->isEPSG(EPSG_CODE_METHOD_VERTCON)) {
        auto fileParameter =
            parameterValue(EPSG_NAME_PARAMETER_VERTICAL_OFFSET_FILE,
                           EPSG_CODE_PARAMETER_VERTICAL_OFFSET_FILE);
        if (fileParameter &&
            fileParameter->type() == ParameterValue::Type::FILENAME) {
            io::PROJStringFormatter::Scope scope(formatter);
            formatter->addStep("vgridshift");
            // The vertcon grids go from NGVD 29 to NAVD 88, with units
            // in millimeter (see
            // https://github.com/OSGeo/proj.4/issues/1071)
            formatter->addParam("grids", fileParameter->valueFile());
            formatter->addParam("multiplier", 0.001);
            return scope.toString();
        }
    }

    if (isLongitudeRotation()) {
        double offsetDeg =
            parameterValueMeasure(EPSG_NAME_PARAMETER_LONGITUDE_OFFSET,
                                  EPSG_CODE_PARAMETER_LONGITUDE_OFFSET)
                .convertToUnit(common::UnitOfMeasure::DEGREE)
                .value();

        auto sourceCRSGeog =
            util::nn_dynamic_pointer_cast<crs::GeographicCRS>(sourceCRS());
        if (!sourceCRSGeog) {
            throw io::FormattingException("Can apply " + method_name +
                                          " only to GeographicCRS");
        }

        auto targetCRSGeog =
            util::nn_dynamic_pointer_cast<crs::GeographicCRS>(targetCRS());
        if (!targetCRSGeog) {
            throw io::FormattingException("Can apply " + method_name +
                                          " only to GeographicCRS");
        }

        if (!sourceCRSGeog->ellipsoid()->isEquivalentTo(
                targetCRSGeog->ellipsoid())) {
            // This is arguable if we should check this...
            throw io::FormattingException("Can apply Longitude rotation "
                                          "only to SRS with same "
                                          "ellipsoid");
        }

        io::PROJStringFormatter::Scope scope(formatter);

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
                sourceCRSGeog->ellipsoid()->exportToPROJString(formatter);
                formatter->addParam("pm", projPMName);
            }
        }
        if (!done) {
            // To actually add the offset, we must use the reverse longlat
            // operation.
            formatter->startInversion();
            formatter->addStep("longlat");
            sourceCRSGeog->ellipsoid()->exportToPROJString(formatter);
            datum::PrimeMeridian::create(util::PropertyMap(),
                                         common::Angle(offsetDeg))
                ->exportToPROJString(formatter);
            formatter->stopInversion();
        }

        targetCRSGeog->addAngularUnitConvertAndAxisSwap(formatter);

        return scope.toString();
    }

    if (ci_equal(method_name, EPSG_NAME_METHOD_GEOGRAPHIC_GEOCENTRIC) ||
        method()->isEPSG(EPSG_CODE_METHOD_GEOGRAPHIC_GEOCENTRIC)) {

        auto sourceCRSGeod =
            util::nn_dynamic_pointer_cast<crs::GeodeticCRS>(sourceCRS());
        auto sourceCRSGeog =
            util::nn_dynamic_pointer_cast<crs::GeographicCRS>(sourceCRS());
        auto targetCRSGeod =
            util::nn_dynamic_pointer_cast<crs::GeodeticCRS>(targetCRS());
        auto targetCRSGeog =
            util::nn_dynamic_pointer_cast<crs::GeographicCRS>(targetCRS());
        bool isSrcGeocentric =
            sourceCRSGeod != nullptr && sourceCRSGeod->isGeocentric();
        bool isSrcGeographic = sourceCRSGeog != nullptr;
        bool isTargetGeocentric =
            targetCRSGeod != nullptr && targetCRSGeod->isGeocentric();
        bool isTargetGeographic = targetCRSGeog != nullptr;
        if ((isSrcGeocentric && isTargetGeographic) ||
            (isSrcGeographic && isTargetGeocentric)) {
            io::PROJStringFormatter::Scope scope(formatter);

            formatter->startInversion();
            sourceCRSGeod->exportToPROJString(formatter);
            formatter->stopInversion();

            targetCRSGeod->exportToPROJString(formatter);

            return scope.toString();
        } else {
            throw io::FormattingException("Invalid nature of source and/or "
                                          "targetCRS for Geographic/Geocentric "
                                          "conversion");
        }
    }

    throw io::FormattingException("Unimplemented");
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
};
//! @endcond

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
ConcatenatedOperation::~ConcatenatedOperation() = default;
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

/** \brief Instanciate a ConcatenatedOperation
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
    for (size_t i = 0; i < operationsIn.size(); i++) {
        auto l_sourceCRS = operationsIn[i]->sourceCRS();
        auto l_targetCRS = operationsIn[i]->targetCRS();
        if (l_sourceCRS == nullptr || l_targetCRS == nullptr) {
            throw InvalidOperation("At least one of the operation lacks a "
                                   "source and/or target CRS");
        }
        if (i >= 1) {
            const auto &sourceCRSIds = l_sourceCRS->identifiers();
            const auto &targetCRSIds = lastTargetCRS->identifiers();
            if (sourceCRSIds.size() == 1 && targetCRSIds.size() == 1 &&
                sourceCRSIds[0]->code() == targetCRSIds[0]->code() &&
                *sourceCRSIds[0]->codeSpace() ==
                    *targetCRSIds[0]->codeSpace()) {
                // same id --> ok
            } else if (!l_sourceCRS->isEquivalentTo(
                           NN_CHECK_ASSERT(lastTargetCRS))) {
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
    op->setCRSs(NN_CHECK_ASSERT(operationsIn[0]->sourceCRS()),
                NN_CHECK_ASSERT(operationsIn.back()->targetCRS()), nullptr);
    op->setAccuracies(accuracies);
    return op;
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
static std::string computeConcatenatedName(
    const std::vector<CoordinateOperationNNPtr> &flattenOps) {
    std::string name;
    for (const auto &subOp : flattenOps) {
        if (!name.empty()) {
            name += " + ";
        }
        if (subOp->name()->description()->empty()) {
            name += "unnamed";
        } else {
            name += *(subOp->name()->description());
        }
    }
    return name;
}
//! @endcond

// ---------------------------------------------------------------------------

/** \brief Instanciate a ConcatenatedOperation, or return a single
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
    for (const auto &subOp : operationsIn) {
        auto subOpConcat =
            util::nn_dynamic_pointer_cast<ConcatenatedOperation>(subOp);
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
            "empty intersection of area of validity of concantenated "
            "operations");
        throw InvalidOperationEmptyIntersection(msg);
    }
    if (extent) {
        properties.set(common::ObjectUsage::DOMAIN_OF_VALIDITY_KEY,
                       NN_CHECK_ASSERT(extent));
    }

    std::vector<metadata::PositionalAccuracyNNPtr> accuracies;
    const double accuracy = getAccuracy(flattenOps);
    if (accuracy >= 0.0) {
        std::ostringstream buffer;
        buffer.imbue(std::locale::classic());
        buffer << accuracy;
        accuracies.emplace_back(
            metadata::PositionalAccuracy::create(buffer.str()));
    }

    auto op = create(properties, flattenOps, accuracies);
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
    return op;
}

// ---------------------------------------------------------------------------

std::string
ConcatenatedOperation::exportToWKT(io::WKTFormatterNNPtr formatter) const {
    const bool isWKT2 = formatter->version() == io::WKTFormatter::Version::WKT2;
    if (!isWKT2 || !formatter->use2018Keywords()) {
        throw io::FormattingException(
            "Transformation can only be exported to WKT2:2018");
    }

    formatter->startNode(io::WKTConstants::CONCATENATEDOPERATION,
                         !identifiers().empty());
    formatter->addQuotedString(*(name()->description()));

    formatter->startNode(io::WKTConstants::SOURCECRS, false);
    sourceCRS()->exportToWKT(formatter);
    formatter->endNode();

    formatter->startNode(io::WKTConstants::TARGETCRS, false);
    targetCRS()->exportToWKT(formatter);
    formatter->endNode();

    for (const auto &operation : operations()) {
        formatter->startNode(io::WKTConstants::STEP, false);
        operation->exportToWKT(formatter);
        formatter->endNode();
    }

    ObjectUsage::_exportToWKT(formatter);
    formatter->endNode();

    return formatter->toString();
}

// ---------------------------------------------------------------------------

std::string ConcatenatedOperation::exportToPROJString(
    io::PROJStringFormatterNNPtr formatter) const // throw(FormattingException)
{
    io::PROJStringFormatter::Scope scope(formatter);
    for (const auto &operation : operations()) {
        operation->exportToPROJString(formatter);
    }
    return scope.toString();
}

// ---------------------------------------------------------------------------

bool ConcatenatedOperation::isEquivalentTo(
    const util::BaseObjectNNPtr &other,
    util::IComparable::Criterion criterion) const {
    auto otherCO = util::nn_dynamic_pointer_cast<ConcatenatedOperation>(other);
    if (otherCO == nullptr || !ObjectUsage::_isEquivalentTo(other, criterion)) {
        return false;
    }
    const auto &steps = operations();
    const auto &otherSteps = otherCO->operations();
    if (steps.size() != otherSteps.size()) {
        return false;
    }
    for (size_t i = 0; i < steps.size(); i++) {
        if (!steps[i]->isEquivalentTo(otherSteps[i], criterion)) {
            return false;
        }
    }
    return true;
}

// ---------------------------------------------------------------------------

std::set<CoordinateOperation::GridDescription>
ConcatenatedOperation::gridsNeeded(
    io::DatabaseContextNNPtr databaseContext) const {
    std::set<CoordinateOperation::GridDescription> res;
    for (const auto &operation : operations()) {
        for (const auto &gridDesc : operation->gridsNeeded(databaseContext)) {
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
    bool allowUseIntermediateCRS_ = true;
    std::vector<std::pair<std::string, std::string>>
        intermediateCRSAuthCodes_{};
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
io::AuthorityFactoryPtr
CoordinateOperationContext::getAuthorityFactory() const {
    return d->authorityFactory_;
}

// ---------------------------------------------------------------------------

/** \brief Return the desired area of interest, or null */
metadata::ExtentPtr CoordinateOperationContext::getAreaOfInterest() const {
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
 * operation between A and B, setting this parameter to true, allow
 * chaining both operations.
 *
 * The current implementation is limited to researching one intermediate
 * step.
 *
 * By default, all potential C candidates will be used. setIntermediateCRS()
 * can be used to restrict them.
 *
 * The default is true.
 */
void CoordinateOperationContext::setAllowUseIntermediateCRS(bool use) {
    d->allowUseIntermediateCRS_ = use;
}

// ---------------------------------------------------------------------------

/** \brief Return whether an intermediate pivot CRS can be used for researching
 * coordinate operations between a source and target CRS.
 *
 * Concretely if in the database there is an operation from A to C
 * (or C to A), and another one from C to B (or B to C), but no direct
 * operation between A and B, setting this parameter to true, allow
 * chaining both operations.
 *
 * The default is true.
 */
bool CoordinateOperationContext::getAllowUseIntermediateCRS() const {
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
 * @param authorityFactory Authority factory, or null if no database lookup
 * is
 * allowed.
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
    auto ctxt = CoordinateOperationContext::nn_make_shared<
        CoordinateOperationContext>();
    ctxt->d->authorityFactory_ = authorityFactory;
    ctxt->d->extent_ = extent;
    ctxt->d->accuracy_ = accuracy;
    return ctxt;
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
struct CoordinateOperationFactory::Private {
    std::vector<CoordinateOperationNNPtr>
    createOperations(const crs::CRSNNPtr &sourceCRS,
                     const crs::CRSNNPtr &targetCRS,
                     CoordinateOperationContextNNPtr context);
};
//! @endcond

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
CoordinateOperationFactory::~CoordinateOperationFactory() = default;
//! @endcond

// ---------------------------------------------------------------------------

CoordinateOperationFactory::CoordinateOperationFactory()
    : d(internal::make_unique<Private>()) {}

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
    bool hasGrids_ = false;
    bool gridsAvailable_ = false;
    bool gridsKnown_ = false;
    size_t stepCount_ = 0;

    PrecomputedOpCharacteristics() = default;
    PrecomputedOpCharacteristics(double area, double accuracy, bool hasGrids,
                                 bool gridsAvailable, bool gridsKnown,
                                 size_t stepCount)
        : area_(area), accuracy_(accuracy), hasGrids_(hasGrids),
          gridsAvailable_(gridsAvailable), gridsKnown_(gridsKnown),
          stepCount_(stepCount) {}
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
    bool operator()(const CoordinateOperationNNPtr &a,
                    const CoordinateOperationNNPtr &b) const {
        auto iterA = map.find(a.get());
        assert(iterA != map.end());
        auto iterB = map.find(b.get());
        assert(iterB != map.end());

        // CAUTION: the order of the comparisons is extremely important
        // to get the intended result.

        if (iterA->second.hasGrids_ && iterB->second.hasGrids_) {
            // Operations where grids are all available go before other
            if (iterA->second.gridsAvailable_ &&
                !iterB->second.gridsAvailable_) {
                return true;
            }
            if (iterB->second.gridsAvailable_ &&
                !iterA->second.gridsAvailable_) {
                return false;
            }
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
        } else if (accuracyA < 0 && accuracyB < 0) {
            // unknown accuracy ? then prefer operations with grids, which
            // are likely to have best practical accuracy
            if (iterA->second.hasGrids_ && !iterB->second.hasGrids_) {
                return true;
            }
            if (!iterA->second.hasGrids_ && iterB->second.hasGrids_) {
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

        const auto a_name = *(a->name()->description());
        const auto b_name = *(b->name()->description());
        // The shorter name, the better ?
        if (a_name.size() < b_name.size()) {
            return true;
        }
        if (b_name.size() < a_name.size()) {
            return false;
        }

        // Arbitrary final criterion
        return a_name < b_name;
    }
};

// ---------------------------------------------------------------------------

static size_t getStepCount(const CoordinateOperationNNPtr &op) {
    auto concat = util::nn_dynamic_pointer_cast<ConcatenatedOperation>(op);
    size_t stepCount = 1;
    if (concat) {
        stepCount = concat->operations().size();
    }
    return stepCount;
}

// ---------------------------------------------------------------------------

struct FilterAndSort {

    FilterAndSort(const std::vector<CoordinateOperationNNPtr> &sourceListIn,
                  const CoordinateOperationContextNNPtr &contextIn,
                  const crs::CRSNNPtr &sourceCRSIn,
                  const crs::CRSNNPtr &targetCRSIn)
        : sourceList(sourceListIn), context(contextIn), sourceCRS(sourceCRSIn),
          targetCRS(targetCRSIn), sourceCRSExtent(getExtent(sourceCRS)),
          targetCRSExtent(getExtent(targetCRS)),
          areaOfInterest(context->getAreaOfInterest()),
          desiredAccuracy(context->getDesiredAccuracy()),
          sourceAndTargetCRSExtentUse(
              context->getSourceAndTargetCRSExtentUse()) {

        computeAreaOfIntest();
        filterOut();
        sort();

        // And now that we have a sorted list, we can remove uninteresting
        // results
        // ...
        removeSyntheticNullGeogOffset();
        removeUninterestingOps();
        removeDuplicateOps();
        removeSyntheticNullGeogOffset();
    }

    // ----------------------------------------------------------------------

    // cppcheck-suppress functionStatic
    const std::vector<CoordinateOperationNNPtr> &getRes() { return res; }

    // ----------------------------------------------------------------------
  private:
    const std::vector<CoordinateOperationNNPtr> &sourceList;
    CoordinateOperationContextNNPtr context;
    crs::CRSNNPtr sourceCRS;
    crs::CRSNNPtr targetCRS;
    metadata::ExtentPtr sourceCRSExtent;
    metadata::ExtentPtr targetCRSExtent;
    metadata::ExtentPtr areaOfInterest;
    const double desiredAccuracy = context->getDesiredAccuracy();
    const CoordinateOperationContext::SourceTargetCRSExtentUse
        sourceAndTargetCRSExtentUse;

    bool hasOpThatContainsAreaOfInterest = false;
    std::vector<CoordinateOperationNNPtr> res{};

    // ----------------------------------------------------------------------
    void computeAreaOfIntest() {

        // Compute an area of interest from the CRS extent if the user did
        // not specify one
        if (!areaOfInterest) {
            if (sourceAndTargetCRSExtentUse ==
                CoordinateOperationContext::SourceTargetCRSExtentUse::
                    INTERSECTION) {
                if (sourceCRSExtent && targetCRSExtent) {
                    areaOfInterest = sourceCRSExtent->intersection(
                        NN_CHECK_ASSERT(targetCRSExtent));
                }
            } else if (sourceAndTargetCRSExtentUse ==
                       CoordinateOperationContext::SourceTargetCRSExtentUse::
                           SMALLEST) {
                if (sourceCRSExtent && targetCRSExtent) {
                    if (getPseudoArea(sourceCRSExtent) <
                        getPseudoArea(targetCRSExtent)) {
                        areaOfInterest = sourceCRSExtent;
                    } else {
                        areaOfInterest = targetCRSExtent;
                    }
                } else if (sourceCRSExtent) {
                    areaOfInterest = sourceCRSExtent;
                } else {
                    areaOfInterest = targetCRSExtent;
                }
            }
        }
    }

    // ---------------------------------------------------------------------------

    void filterOut() {

        // Filter out operations that do not match the expected accuracy
        // and area of use.
        const auto spatialCriterion = context->getSpatialCriterion();
        for (const auto &op : sourceList) {
            if (desiredAccuracy != 0) {
                const double accuracy = getAccuracy(op);
                if (accuracy < 0 || accuracy > desiredAccuracy) {
                    continue;
                }
            }
            if (areaOfInterest) {
                bool emptyIntersection = false;
                auto extent = getExtent(op, true, emptyIntersection);
                if (!extent)
                    continue;
                bool extentContains =
                    extent->contains(NN_CHECK_ASSERT(areaOfInterest));
                if (extentContains) {
                    hasOpThatContainsAreaOfInterest = true;
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
                    !extent->intersects(NN_CHECK_ASSERT(areaOfInterest))) {
                    continue;
                }
            } else if (sourceAndTargetCRSExtentUse ==
                       CoordinateOperationContext::SourceTargetCRSExtentUse::
                           BOTH) {
                bool emptyIntersection = false;
                auto extent = getExtent(op, true, emptyIntersection);
                if (!extent)
                    continue;
                bool extentContainsSource =
                    !sourceCRSExtent ||
                    extent->contains(NN_CHECK_ASSERT(sourceCRSExtent));
                bool extentContainsTarget =
                    !targetCRSExtent ||
                    extent->contains(NN_CHECK_ASSERT(targetCRSExtent));
                if (extentContainsSource && extentContainsTarget) {
                    hasOpThatContainsAreaOfInterest = true;
                }
                if (spatialCriterion ==
                    CoordinateOperationContext::SpatialCriterion::
                        STRICT_CONTAINMENT) {
                    if (!extentContainsSource || !extentContainsTarget) {
                        continue;
                    }
                } else if (spatialCriterion ==
                           CoordinateOperationContext::SpatialCriterion::
                               PARTIAL_INTERSECTION) {
                    bool extentIntersectsSource =
                        !sourceCRSExtent ||
                        extent->intersects(NN_CHECK_ASSERT(sourceCRSExtent));
                    bool extentIntersectsTarget =
                        targetCRSExtent &&
                        extent->intersects(NN_CHECK_ASSERT(targetCRSExtent));
                    if (!extentIntersectsSource || !extentIntersectsTarget) {
                        continue;
                    }
                }
            }
            res.emplace_back(op);
        }
    }

    // ----------------------------------------------------------------------

    void sort() {

        // Precompute a number of parameters for each operation that will be
        // useful for the sorting.
        std::map<CoordinateOperation *, PrecomputedOpCharacteristics> map;
        for (const auto &op : res) {
            bool dummy = false;
            auto extentOp = getExtent(op, true, dummy);
            double area = 0.0;
            if (extentOp) {
                if (areaOfInterest) {
                    area = getPseudoArea(extentOp->intersection(
                        NN_CHECK_ASSERT(areaOfInterest)));
                } else if (sourceCRSExtent && targetCRSExtent) {
                    auto x = extentOp->intersection(
                        NN_CHECK_ASSERT(sourceCRSExtent));
                    auto y = extentOp->intersection(
                        NN_CHECK_ASSERT(targetCRSExtent));
                    area = getPseudoArea(x) + getPseudoArea(y) -
                           ((x && y) ? getPseudoArea(
                                           x->intersection(NN_CHECK_ASSERT(y)))
                                     : 0.0);
                } else if (sourceCRSExtent) {
                    area = getPseudoArea(extentOp->intersection(
                        NN_CHECK_ASSERT(sourceCRSExtent)));
                } else if (targetCRSExtent) {
                    area = getPseudoArea(extentOp->intersection(
                        NN_CHECK_ASSERT(targetCRSExtent)));
                } else {
                    area = getPseudoArea(extentOp);
                }
            }

            bool hasGrids = false;
            bool gridsAvailable = true;
            bool gridsKnown = true;
            if (context->getAuthorityFactory() &&
                context->getGridAvailabilityUse() ==
                    CoordinateOperationContext::GridAvailabilityUse::
                        USE_FOR_SORTING) {
                for (const auto &gridDesc : op->gridsNeeded(
                         context->getAuthorityFactory()->databaseContext())) {
                    hasGrids = true;
                    if (!gridDesc.available) {
                        gridsAvailable = false;
                    }
                    if (gridDesc.packageName.empty()) {
                        gridsKnown = false;
                    }
                }
            }

            const auto stepCount = getStepCount(op);

            map[op.get()] = PrecomputedOpCharacteristics(
                area, getAccuracy(op), hasGrids, gridsAvailable, gridsKnown,
                stepCount);
        }

        // Sort !
        std::sort(res.begin(), res.end(), SortFunction(map));
    }

    // ----------------------------------------------------------------------

    void removeSyntheticNullGeogOffset() {

        // If we have more than one result, and than the last result is the
        // default
        // "Null geographic offset" operation we have synthetized, remove it as
        // all previous results are necessarily better
        if (hasOpThatContainsAreaOfInterest && res.size() > 1 &&
            (*(res.back()->name()->description()))
                    .find("Null geographic offset") != std::string::npos) {
            std::vector<CoordinateOperationNNPtr> resTemp;
            for (size_t i = 0; i < res.size() - 1; i++) {
                resTemp.emplace_back(res[i]);
            }
            res = std::move(resTemp);
        }
    }

    // ----------------------------------------------------------------------

    void removeUninterestingOps() {

        // Eliminate operations that bring nothing, ie for a given area of use,
        // do not keep operations that have greater accuracy. Actually we must
        // be a bit more subtle than that, and take into account grid
        // availability
        std::vector<CoordinateOperationNNPtr> resTemp;
        metadata::ExtentPtr lastExtent;
        double lastAccuracy = -1;
        bool lastHasGrids = false;
        bool lastGridsAvailable = true;
        std::set<std::set<std::string>> setOfSetOfGrids;
        size_t lastStepCount = 0;
        CoordinateOperationPtr lastOp;

        bool first = true;
        for (const auto &op : res) {
            const auto curAccuracy = getAccuracy(op);
            bool dummy = false;
            const auto curExtent = getExtent(op, true, dummy);
            bool curHasGrids = false;
            bool curGridsAvailable = true;
            std::set<std::string> curSetOfGrids;

            const auto curStepCount = getStepCount(op);

            if (context->getAuthorityFactory()) {
                for (const auto &gridDesc : op->gridsNeeded(
                         context->getAuthorityFactory()->databaseContext())) {
                    curHasGrids = true;
                    curSetOfGrids.insert(gridDesc.shortName);
                    if (!gridDesc.available) {
                        curGridsAvailable = false;
                    }
                }
            }

            if (first) {
                resTemp.emplace_back(op);

                lastHasGrids = curHasGrids;
                lastGridsAvailable = curGridsAvailable;
                first = false;
            } else {
                if (lastOp->isEquivalentTo(op)) {
                    continue;
                }
                const bool sameExtent =
                    ((!curExtent && !lastExtent) ||
                     (curExtent && lastExtent &&
                      curExtent->contains(NN_CHECK_ASSERT(lastExtent)) &&
                      lastExtent->contains(NN_CHECK_ASSERT(curExtent))));
                if (curAccuracy > lastAccuracy && sameExtent) {
                    // If that set of grids has always been used for that
                    // extent,
                    // no need to add them again
                    if (setOfSetOfGrids.find(curSetOfGrids) !=
                        setOfSetOfGrids.end()) {
                        continue;
                    }
                    // If we have already found a operation without grids for
                    // that extent, no need to add any lower accuracy operation
                    if (!lastHasGrids) {
                        continue;
                    }
                    // If we had only operations involving grids, but one
                    // past operation had available grids, no need to add
                    // the new one.
                    if (curHasGrids && curGridsAvailable &&
                        lastGridsAvailable) {
                        continue;
                    }
                } else if (curAccuracy == lastAccuracy && sameExtent) {
                    if (curStepCount > lastStepCount) {
                        continue;
                    }
                }

                resTemp.emplace_back(op);

                if (sameExtent) {
                    if (!curHasGrids) {
                        lastHasGrids = false;
                    }
                    if (curGridsAvailable) {
                        lastGridsAvailable = true;
                    }
                } else {
                    setOfSetOfGrids.clear();

                    lastHasGrids = curHasGrids;
                    lastGridsAvailable = curGridsAvailable;
                }
            }

            lastOp = op.as_nullable();
            lastStepCount = curStepCount;
            lastExtent = curExtent;
            lastAccuracy = curAccuracy;
            if (!curSetOfGrids.empty()) {
                setOfSetOfGrids.insert(curSetOfGrids);
            }
        }
        res = std::move(resTemp);
    }

    // ----------------------------------------------------------------------

    void removeDuplicateOps() {

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
                std::string key(op->exportToPROJString(formatter));
                bool dummy = false;
                std::ostringstream buffer;
                buffer.imbue(std::locale::classic());
                auto extentOp = getExtent(op, true, dummy);
                if (extentOp) {
                    auto geogElts = extentOp->geographicElements();
                    if (geogElts.size() == 1) {
                        auto bbox = util::nn_dynamic_pointer_cast<
                            metadata::GeographicBoundingBox>(geogElts[0]);
                        if (bbox) {
                            double w = bbox->westBoundLongitude();
                            double s = bbox->southBoundLatitude();
                            double e = bbox->eastBoundLongitude();
                            double n = bbox->northBoundLatitude();
                            buffer << "-" << w;
                            buffer << "-" << s;
                            buffer << "-" << e;
                            buffer << "-" << n;
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
              CoordinateOperationContextNNPtr context,
              const crs::CRSNNPtr &sourceCRS, const crs::CRSNNPtr &targetCRS) {
    return FilterAndSort(sourceList, context, sourceCRS, targetCRS).getRes();
}
//! @endcond

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
// Apply the inverse() method on all elements of the input list
static std::vector<CoordinateOperationNNPtr>
applyInverse(const std::vector<CoordinateOperationNNPtr> &list) {
    auto res = list;
    for (auto &op : res) {
        op = op->inverse();
    }
    return res;
}
//! @endcond

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
// Look in the authority registry for operations from sourceCRS to targetCRS
static std::vector<CoordinateOperationNNPtr>
findOpsInRegistryDirect(const crs::CRSNNPtr &sourceCRS,
                        const crs::CRSNNPtr &targetCRS,
                        const CoordinateOperationContextNNPtr &context) {
    auto authFactory = context->getAuthorityFactory();
    assert(authFactory);
    for (const auto &idSrc : sourceCRS->identifiers()) {
        auto srcAuthName = *(idSrc->codeSpace());
        auto srcCode = idSrc->code();
        if (!srcAuthName.empty()) {
            for (const auto &idTarget : targetCRS->identifiers()) {
                auto targetAuthName = *(idTarget->codeSpace());
                auto targetCode = idTarget->code();
                if (!targetAuthName.empty()) {
                    auto res =
                        authFactory->createFromCoordinateReferenceSystemCodes(
                            srcAuthName, srcCode, targetAuthName, targetCode,
                            context->getUsePROJAlternativeGridNames(),
                            context->getGridAvailabilityUse() ==
                                CoordinateOperationContext::
                                    GridAvailabilityUse::
                                        DISCARD_OPERATION_IF_MISSING_GRID);
                    if (!res.empty()) {
                        return res;
                    }
                }
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
static std::vector<CoordinateOperationNNPtr> findsOpsInRegistryWithIntermediate(
    const crs::CRSNNPtr &sourceCRS, const crs::CRSNNPtr &targetCRS,
    const CoordinateOperationContextNNPtr &context) {
    if (!context->getAllowUseIntermediateCRS()) {
        return std::vector<CoordinateOperationNNPtr>();
    }

    auto authFactory = context->getAuthorityFactory();
    assert(authFactory);
    for (const auto &idSrc : sourceCRS->identifiers()) {
        auto srcAuthName = *(idSrc->codeSpace());
        auto srcCode = idSrc->code();
        if (!srcAuthName.empty()) {
            for (const auto &idTarget : targetCRS->identifiers()) {
                auto targetAuthName = *(idTarget->codeSpace());
                auto targetCode = idTarget->code();
                if (!targetAuthName.empty()) {
                    auto res = authFactory->createFromCRSCodesWithIntermediates(
                        srcAuthName, srcCode, targetAuthName, targetCode,
                        context->getUsePROJAlternativeGridNames(),
                        context->getGridAvailabilityUse() ==
                            CoordinateOperationContext::GridAvailabilityUse::
                                DISCARD_OPERATION_IF_MISSING_GRID,
                        context->getIntermediateCRS());
                    if (!res.empty()) {
                        return res;
                    }
                }
            }
        }
    }
    return std::vector<CoordinateOperationNNPtr>();
}
//! @endcond

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
static TransformationNNPtr
createNullGeographicOffset(const crs::CRSNNPtr &sourceCRS,
                           const crs::CRSNNPtr &targetCRS) {
    if (util::nn_dynamic_pointer_cast<crs::SingleCRS>(sourceCRS)
                ->coordinateSystem()
                ->axisList()
                .size() == 3 ||
        util::nn_dynamic_pointer_cast<crs::SingleCRS>(targetCRS)
                ->coordinateSystem()
                ->axisList()
                .size() == 3) {
        return Transformation::createGeographic3DOffsets(
            util::PropertyMap()
                .set(common::IdentifiedObject::NAME_KEY,
                     "Null geographic offset transformation from " +
                         *(sourceCRS->name()->description()) + " to " +
                         *(targetCRS->name()->description()))
                .set(common::ObjectUsage::DOMAIN_OF_VALIDITY_KEY,
                     metadata::Extent::WORLD),
            sourceCRS, targetCRS, common::Angle(0), common::Angle(0),
            common::Length(0), {});
    } else {
        return Transformation::createGeographic2DOffsets(
            util::PropertyMap()
                .set(common::IdentifiedObject::NAME_KEY,
                     "Null geographic offset transformation from " +
                         *(sourceCRS->name()->description()) + " to " +
                         *(targetCRS->name()->description()))
                .set(common::ObjectUsage::DOMAIN_OF_VALIDITY_KEY,
                     metadata::Extent::WORLD),
            sourceCRS, targetCRS, common::Angle(0), common::Angle(0), {});
    }
}
//! @endcond

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress

static CoordinateOperationNNPtr
createGeodToGeodPROJBased(const crs::GeodeticCRSNNPtr &geodSrc,
                          const crs::GeodeticCRSNNPtr &geodDst) {
    struct MyPROJStringExportable : public io::IPROJStringExportable {
        crs::GeodeticCRSPtr geodSrc{};
        crs::GeodeticCRSPtr geodDst{};

        // cppcheck-suppress functionStatic
        std::string exportToPROJString(
            io::PROJStringFormatterNNPtr formatter) const override {

            io::PROJStringFormatter::Scope scope(formatter);

            formatter->startInversion();
            geodSrc->exportToPROJString(formatter);
            formatter->stopInversion();
            geodDst->exportToPROJString(formatter);

            return scope.toString();
        }
    };

    auto exportable = util::nn_make_shared<MyPROJStringExportable>();
    exportable->geodSrc = geodSrc;
    exportable->geodDst = geodDst;

    auto properties = util::PropertyMap().set(
        common::IdentifiedObject::NAME_KEY,
        "Transformation from " + *(geodSrc->name()->description()) + " to " +
            *(geodDst->name()->description()));
    return createPROJBased(properties, exportable, geodSrc, geodDst);
}

// ---------------------------------------------------------------------------

static CoordinateOperationNNPtr createHorizVerticalPROJBased(
    const crs::CRSNNPtr &sourceCRS, const crs::CRSNNPtr &targetCRS,
    const operation::CoordinateOperationNNPtr &horizTransform,
    const operation::CoordinateOperationNNPtr &verticalTransform) {

    auto geogDst = util::nn_dynamic_pointer_cast<crs::GeographicCRS>(targetCRS);
    assert(geogDst);

    struct MyPROJStringExportable : public io::IPROJStringExportable {
        CoordinateOperationPtr horizTransform{};
        CoordinateOperationPtr verticalTransform{};
        crs::GeographicCRSPtr geogDst{};

        // cppcheck-suppress functionStatic
        std::string exportToPROJString(
            io::PROJStringFormatterNNPtr formatter) const override {

            io::PROJStringFormatter::Scope scope(formatter);

            formatter->setOmitZUnitConversion(true);
            horizTransform->exportToPROJString(formatter);

            formatter->startInversion();
            geogDst->addAngularUnitConvertAndAxisSwap(formatter);
            formatter->stopInversion();
            formatter->setOmitZUnitConversion(false);

            verticalTransform->exportToPROJString(formatter);

            formatter->setOmitZUnitConversion(true);
            geogDst->addAngularUnitConvertAndAxisSwap(formatter);
            formatter->setOmitZUnitConversion(false);

            return scope.toString();
        }
    };

    auto exportable = util::nn_make_shared<MyPROJStringExportable>();
    exportable->horizTransform = horizTransform;
    exportable->verticalTransform = verticalTransform;
    exportable->geogDst = geogDst;

    bool dummy = false;
    auto ops = std::vector<CoordinateOperationNNPtr>{horizTransform,
                                                     verticalTransform};
    auto extent = getExtent(ops, true, dummy);
    auto properties = util::PropertyMap();
    properties.set(common::IdentifiedObject::NAME_KEY,
                   computeConcatenatedName(ops));

    if (extent) {
        properties.set(common::ObjectUsage::DOMAIN_OF_VALIDITY_KEY,
                       NN_CHECK_ASSERT(extent));
    }

    std::vector<metadata::PositionalAccuracyNNPtr> accuracies;
    const double accuracy = getAccuracy(ops);
    if (accuracy >= 0.0) {
        std::ostringstream buffer;
        buffer.imbue(std::locale::classic());
        buffer << accuracy;
        accuracies.emplace_back(
            metadata::PositionalAccuracy::create(buffer.str()));
    }

    return createPROJBased(properties, exportable, sourceCRS, targetCRS,
                           accuracies);
}

// ---------------------------------------------------------------------------

static CoordinateOperationNNPtr createHorizVerticalHorizPROJBased(
    const crs::CRSNNPtr &sourceCRS, const crs::CRSNNPtr &targetCRS,
    const operation::CoordinateOperationNNPtr &opSrcCRSToGeogCRS,
    const operation::CoordinateOperationNNPtr &verticalTransform,
    const operation::CoordinateOperationNNPtr &opGeogCRStoDstCRS,
    const crs::GeographicCRSPtr &interpolationGeogCRS) {

    struct MyPROJStringExportable : public io::IPROJStringExportable {
        CoordinateOperationPtr opSrcCRSToGeogCRS{};
        CoordinateOperationPtr verticalTransform{};
        CoordinateOperationPtr opGeogCRStoDstCRS{};
        crs::GeographicCRSPtr interpolationGeogCRS{};

        // cppcheck-suppress functionStatic
        std::string exportToPROJString(
            io::PROJStringFormatterNNPtr formatter) const override {

            io::PROJStringFormatter::Scope scope(formatter);

            formatter->setOmitZUnitConversion(true);

            opSrcCRSToGeogCRS->exportToPROJString(formatter);

            formatter->startInversion();
            interpolationGeogCRS->addAngularUnitConvertAndAxisSwap(formatter);
            formatter->stopInversion();

            formatter->setOmitZUnitConversion(false);

            verticalTransform->exportToPROJString(formatter);

            formatter->setOmitZUnitConversion(true);

            interpolationGeogCRS->addAngularUnitConvertAndAxisSwap(formatter);

            opGeogCRStoDstCRS->exportToPROJString(formatter);

            formatter->setOmitZUnitConversion(false);

            return scope.toString();
        }
    };

    auto exportable = util::nn_make_shared<MyPROJStringExportable>();
    exportable->opSrcCRSToGeogCRS = opSrcCRSToGeogCRS;
    exportable->verticalTransform = verticalTransform;
    exportable->opGeogCRStoDstCRS = opGeogCRStoDstCRS;
    exportable->interpolationGeogCRS = interpolationGeogCRS;

    bool dummy = false;
    auto ops = std::vector<CoordinateOperationNNPtr>{
        opSrcCRSToGeogCRS, verticalTransform, opGeogCRStoDstCRS};
    auto extent = getExtent(ops, true, dummy);
    auto properties = util::PropertyMap();
    properties.set(common::IdentifiedObject::NAME_KEY,
                   computeConcatenatedName(ops));

    if (extent) {
        properties.set(common::ObjectUsage::DOMAIN_OF_VALIDITY_KEY,
                       NN_CHECK_ASSERT(extent));
    }

    std::vector<metadata::PositionalAccuracyNNPtr> accuracies;
    const double accuracy = getAccuracy(ops);
    if (accuracy >= 0.0) {
        std::ostringstream buffer;
        buffer.imbue(std::locale::classic());
        buffer << accuracy;
        accuracies.emplace_back(
            metadata::PositionalAccuracy::create(buffer.str()));
    }

    return createPROJBased(properties, exportable, sourceCRS, targetCRS,
                           accuracies);
}

//! @endcond

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
std::vector<CoordinateOperationNNPtr>
CoordinateOperationFactory::Private::createOperations(
    const crs::CRSNNPtr &sourceCRS, const crs::CRSNNPtr &targetCRS,
    CoordinateOperationContextNNPtr context) {

    std::vector<CoordinateOperationNNPtr> res;
    const bool allowEmptyIntersection = true;

    // First look-up if the registry provide us with operations.
    auto authFactory = context->getAuthorityFactory();
    io::DatabaseContextPtr databaseContext =
        authFactory ? authFactory->databaseContext().as_nullable() : nullptr;
    auto derivedSrc = util::nn_dynamic_pointer_cast<crs::DerivedCRS>(sourceCRS);
    auto derivedDst = util::nn_dynamic_pointer_cast<crs::DerivedCRS>(targetCRS);
    if (authFactory &&
        (derivedSrc == nullptr ||
         !derivedSrc->baseCRS()->isEquivalentTo(
             targetCRS, util::IComparable::Criterion::EQUIVALENT)) &&
        (derivedDst == nullptr ||
         !derivedDst->baseCRS()->isEquivalentTo(
             sourceCRS, util::IComparable::Criterion::EQUIVALENT))) {

        res = findOpsInRegistryDirect(sourceCRS, targetCRS, context);
        if (!sourceCRS->isEquivalentTo(targetCRS)) {
            auto resFromInverse = applyInverse(
                findOpsInRegistryDirect(targetCRS, sourceCRS, context));
            res.insert(res.end(), resFromInverse.begin(), resFromInverse.end());

            // If we get at least a result with perfect accuracy, do not bother
            // generating synthetic transforms.
            for (const auto &op : res) {
                const double acc = getAccuracy(op);
                if (acc == 0.0) {
                    return res;
                }
            }

            // NAD27 to NAD83 has tens of results already. No need to look
            // for a pivot
            if (res.size() < 5 || getenv("PROJ_FORCE_SEARCH_PIVOT")) {
                auto resWithIntermediate = findsOpsInRegistryWithIntermediate(
                    sourceCRS, targetCRS, context);
                res.insert(res.end(), resWithIntermediate.begin(),
                           resWithIntermediate.end());
            }
        }

        // If we get at least a result with perfect accuracy, do not bother
        // generating synthetic transforms.
        for (const auto &op : res) {
            const double acc = getAccuracy(op);
            if (acc == 0.0) {
                return res;
            }
        }
    }

    // Special case if both CRS are geodetic
    auto geodSrc = util::nn_dynamic_pointer_cast<crs::GeodeticCRS>(sourceCRS);
    auto geodDst = util::nn_dynamic_pointer_cast<crs::GeodeticCRS>(targetCRS);
    if (geodSrc && geodDst && !derivedSrc && !derivedDst) {

        if (geodSrc->ellipsoid()->celestialBody() !=
            geodDst->ellipsoid()->celestialBody()) {
            throw util::UnsupportedOperationException(
                "Source and target ellipsoid do not belong to the same "
                "celestial body");
        }

        auto geogSrc =
            util::nn_dynamic_pointer_cast<crs::GeographicCRS>(sourceCRS);
        auto geogDst =
            util::nn_dynamic_pointer_cast<crs::GeographicCRS>(targetCRS);

        if (geogSrc && geogDst) {
            std::vector<CoordinateOperationNNPtr> steps;

            auto src_pm = geogSrc->primeMeridian()->longitude();
            auto dst_pm = geogDst->primeMeridian()->longitude();
            auto offset_pm =
                (src_pm.unit() == dst_pm.unit())
                    ? common::Angle(src_pm.value() - dst_pm.value(),
                                    src_pm.unit())
                    : common::Angle(
                          src_pm.convertToUnit(common::UnitOfMeasure::DEGREE)
                                  .value() -
                              dst_pm
                                  .convertToUnit(common::UnitOfMeasure::DEGREE)
                                  .value(),
                          common::UnitOfMeasure::DEGREE);

            double vconvSrc = 1.0;
            if (geogSrc->coordinateSystem()->axisList().size() == 3) {
                vconvSrc = geogSrc->coordinateSystem()
                               ->axisList()[2]
                               ->unit()
                               .conversionToSI();
            }
            double vconvDst = 1.0;
            if (geogDst->coordinateSystem()->axisList().size() == 3) {
                vconvDst = geogDst->coordinateSystem()
                               ->axisList()[2]
                               ->unit()
                               .conversionToSI();
            }

            // Do they differ by vertical units ?
            if (vconvSrc != vconvDst &&
                geogSrc->ellipsoid()->isEquivalentTo(
                    geogDst->ellipsoid(),
                    util::IComparable::Criterion::EQUIVALENT)) {
                if (offset_pm.value() == 0) {
                    // If only by vertical units, use a Change of Vertical
                    // Unit
                    // transformation
                    const double factor = vconvSrc / vconvDst;
                    auto conv = Conversion::createChangeVerticalUnit(
                        util::PropertyMap().set(
                            common::IdentifiedObject::NAME_KEY,
                            "Transformation from " +
                                *(sourceCRS->name()->description()) + " to " +
                                *(targetCRS->name()->description())),
                        common::Scale(factor));
                    conv->setCRSs(sourceCRS, targetCRS, nullptr);
                    res.push_back(conv);
                    return res;
                } else {
                    res.emplace_back(createGeodToGeodPROJBased(
                        NN_CHECK_ASSERT(geodSrc), NN_CHECK_ASSERT(geodDst)));
                    return res;
                }
            }

            // If both are geographic and only differ by their prime
            // meridian,
            // apply a longitude rotation transformation.
            if (geogSrc->ellipsoid()->isEquivalentTo(
                    geogDst->ellipsoid(),
                    util::IComparable::Criterion::EQUIVALENT) &&
                src_pm.getSIValue() != dst_pm.getSIValue()) {

                steps.emplace_back(Transformation::createLongitudeRotation(
                    util::PropertyMap()
                        .set(common::IdentifiedObject::NAME_KEY,
                             "Transformation from " +
                                 *(sourceCRS->name()->description()) + " to " +
                                 *(targetCRS->name()->description()))
                        .set(common::ObjectUsage::DOMAIN_OF_VALIDITY_KEY,
                             metadata::Extent::WORLD),
                    sourceCRS, targetCRS, offset_pm));
                // If only the target has a non-zero prime meridian, chain a
                // null geographic offset and then the longitude rotation
            } else if (src_pm.getSIValue() == 0 && dst_pm.getSIValue() != 0) {
                auto datum = datum::GeodeticReferenceFrame::create(
                    util::PropertyMap(), geogDst->ellipsoid(),
                    util::optional<std::string>(), geogSrc->primeMeridian());
                auto interm_crs = crs::GeographicCRS::create(
                    util::PropertyMap()
                        .set(common::IdentifiedObject::NAME_KEY,
                             *(targetCRS->name()->description()) +
                                 " altered to use prime meridian of " +
                                 *(sourceCRS->name()->description()))
                        .set(common::ObjectUsage::DOMAIN_OF_VALIDITY_KEY,
                             metadata::Extent::WORLD),
                    datum, geogDst->coordinateSystem());

                steps.emplace_back(
                    createNullGeographicOffset(sourceCRS, interm_crs));

                steps.emplace_back(Transformation::createLongitudeRotation(
                    util::PropertyMap()
                        .set(common::IdentifiedObject::NAME_KEY,
                             "Transformation from " +
                                 *(sourceCRS->name()->description()) + " to " +
                                 *(interm_crs->name()->description()))
                        .set(common::ObjectUsage::DOMAIN_OF_VALIDITY_KEY,
                             metadata::Extent::WORLD),
                    interm_crs, targetCRS, offset_pm));

            } else {
                auto interm_crs = sourceCRS.as_nullable();
                // If the prime meridians are different, chain a longitude
                // rotation and the null geographic offset.
                if (src_pm.getSIValue() != dst_pm.getSIValue()) {
                    auto datum = datum::GeodeticReferenceFrame::create(
                        util::PropertyMap(), geogSrc->ellipsoid(),
                        util::optional<std::string>(),
                        geogDst->primeMeridian());
                    interm_crs =
                        crs::GeographicCRS::create(
                            util::PropertyMap().set(
                                common::IdentifiedObject::NAME_KEY,
                                *(sourceCRS->name()->description()) +
                                    " altered to use prime meridian of " +
                                    *(targetCRS->name()->description())),
                            datum, geogSrc->coordinateSystem())
                            .as_nullable();
                    steps.emplace_back(Transformation::createLongitudeRotation(
                        util::PropertyMap()
                            .set(common::IdentifiedObject::NAME_KEY,
                                 "Transformation from " +
                                     *(sourceCRS->name()->description()) +
                                     " to " +
                                     *(interm_crs->name()->description()))
                            .set(common::ObjectUsage::DOMAIN_OF_VALIDITY_KEY,
                                 metadata::Extent::WORLD),
                        sourceCRS, NN_CHECK_ASSERT(interm_crs), offset_pm));
                }

                steps.emplace_back(createNullGeographicOffset(
                    NN_CHECK_ASSERT(interm_crs), targetCRS));
            }

            res.emplace_back(ConcatenatedOperation::createComputeMetadata(
                steps, !allowEmptyIntersection));
            return res;
        }

        bool isSrcGeocentric = geodSrc->isGeocentric();
        bool isSrcGeographic = geogSrc != nullptr;
        bool isTargetGeocentric = geodDst->isGeocentric();
        bool isTargetGeographic = geogDst != nullptr;
        if ((isSrcGeocentric && isTargetGeographic) ||
            (isSrcGeographic && isTargetGeocentric)) {
            res.emplace_back(createGeographicGeocentric(
                util::PropertyMap().set(
                    common::IdentifiedObject::NAME_KEY,
                    "Transformation from " +
                        *(sourceCRS->name()->description()) + " to " +
                        *(targetCRS->name()->description())),
                sourceCRS, targetCRS));
            return res;
        }

        // Tranformation between two geocentric systems
        res.emplace_back(createGeodToGeodPROJBased(NN_CHECK_ASSERT(geodSrc),
                                                   NN_CHECK_ASSERT(geodDst)));
        return res;
    }

    // If the source is a derived CRS, then chain the inverse of its
    // deriving conversion, with transforms from its baseCRS to the
    // targetCRS
    if (derivedSrc) {
        auto opFirst = derivedSrc->derivingConversion()->inverse();
        // Small optimization if the targetCRS is the baseCRS of the source
        // derivedCRS.
        if (derivedSrc->baseCRS()->isEquivalentTo(
                targetCRS, util::IComparable::Criterion::EQUIVALENT)) {
            res.emplace_back(opFirst);
            return res;
        }
        auto opsSecond =
            createOperations(derivedSrc->baseCRS(), targetCRS, context);
        for (const auto &opSecond : opsSecond) {
            try {
                res.emplace_back(ConcatenatedOperation::createComputeMetadata(
                    {opFirst, opSecond}, !allowEmptyIntersection));
            } catch (const InvalidOperationEmptyIntersection &) {
            }
        }
        return res;
    }

    // reverse of previous case
    if (derivedDst) {
        return applyInverse(createOperations(targetCRS, sourceCRS, context));
    }

    // boundCRS to a geogCRS that is the same as the hubCRS
    auto boundSrc = util::nn_dynamic_pointer_cast<crs::BoundCRS>(sourceCRS);
    auto geogDst = util::nn_dynamic_pointer_cast<crs::GeographicCRS>(targetCRS);
    if (boundSrc && geogDst) {
        auto hubSrc = boundSrc->hubCRS();
        auto hubSrcGeog =
            util::nn_dynamic_pointer_cast<crs::GeographicCRS>(hubSrc);
        auto geogCRSOfBaseOfBoundSrc =
            boundSrc->baseCRS()->extractGeographicCRS();
        if (hubSrcGeog &&
            (hubSrcGeog->isEquivalentTo(
                 NN_CHECK_ASSERT(geogDst),
                 util::IComparable::Criterion::EQUIVALENT) ||
             hubSrcGeog->is2DPartOf3D(NN_CHECK_ASSERT(geogDst))) &&
            geogCRSOfBaseOfBoundSrc) {
            auto opsFirst = createOperations(
                boundSrc->baseCRS(), NN_CHECK_ASSERT(geogCRSOfBaseOfBoundSrc),
                context);
            if (!opsFirst.empty()) {
                for (const auto &opFirst : opsFirst) {
                    try {
                        res.emplace_back(
                            ConcatenatedOperation::createComputeMetadata(
                                {opFirst, boundSrc->transformation()},
                                !allowEmptyIntersection));
                    } catch (const InvalidOperationEmptyIntersection &) {
                    }
                }
                if (!res.empty()) {
                    return res;
                }
            }
        }

        if (hubSrcGeog &&
            hubSrcGeog->isEquivalentTo(
                NN_CHECK_ASSERT(geogDst),
                util::IComparable::Criterion::EQUIVALENT) &&
            util::nn_dynamic_pointer_cast<crs::VerticalCRS>(
                boundSrc->baseCRS())) {
            res.emplace_back(boundSrc->transformation());
            return res;
        }

        return createOperations(boundSrc->baseCRS(), targetCRS, context);
    }

    // reverse of previous case
    auto boundDst = util::nn_dynamic_pointer_cast<crs::BoundCRS>(targetCRS);
    auto geogSrc = util::nn_dynamic_pointer_cast<crs::GeographicCRS>(sourceCRS);
    if (geogSrc && boundDst) {
        return applyInverse(createOperations(targetCRS, sourceCRS, context));
    }

    // vertCRS (as boundCRS with transformation to target vertCRS) to
    // vertCRS
    auto vertDst = util::nn_dynamic_pointer_cast<crs::VerticalCRS>(targetCRS);
    if (boundSrc && vertDst) {
        auto baseSrcVert = util::nn_dynamic_pointer_cast<crs::VerticalCRS>(
            boundSrc->baseCRS());
        auto hubSrc = boundSrc->hubCRS();
        auto hubSrcVert =
            util::nn_dynamic_pointer_cast<crs::VerticalCRS>(hubSrc);
        if (baseSrcVert && hubSrcVert &&
            vertDst->isEquivalentTo(NN_CHECK_ASSERT(hubSrcVert),
                                    util::IComparable::Criterion::EQUIVALENT)) {
            res.emplace_back(boundSrc->transformation());
            return res;
        }

        return createOperations(boundSrc->baseCRS(), targetCRS, context);
    }

    // reverse of previous case
    auto vertSrc = util::nn_dynamic_pointer_cast<crs::VerticalCRS>(sourceCRS);
    if (boundDst && vertSrc) {
        return applyInverse(createOperations(targetCRS, sourceCRS, context));
    }

    if (vertSrc && vertDst) {
        const auto &srcDatum = vertSrc->datum();
        const auto &dstDatum = vertDst->datum();
        if (srcDatum && dstDatum &&
            srcDatum->isEquivalentTo(
                NN_CHECK_ASSERT(dstDatum),
                util::IComparable::Criterion::EQUIVALENT)) {
            const double convSrc = vertSrc->coordinateSystem()
                                       ->axisList()[0]
                                       ->unit()
                                       .conversionToSI();
            const double convDst = vertDst->coordinateSystem()
                                       ->axisList()[0]
                                       ->unit()
                                       .conversionToSI();
            if (convSrc != convDst) {
                const double factor = convSrc / convDst;
                auto conv = Conversion::createChangeVerticalUnit(
                    util::PropertyMap().set(
                        common::IdentifiedObject::NAME_KEY,
                        "Transformation from " +
                            *(sourceCRS->name()->description()) + " to " +
                            *(targetCRS->name()->description())),
                    common::Scale(factor));
                conv->setCRSs(sourceCRS, targetCRS, nullptr);
                res.push_back(conv);
                return res;
            }
        }
    }

    // A bit odd case as we are comparing apples to oranges, but in case
    // the vertical unit differ, do something useful.
    if (vertSrc && geogDst) {
        const double convSrc =
            vertSrc->coordinateSystem()->axisList()[0]->unit().conversionToSI();
        double convDst = 1.0;
        if (geogDst->coordinateSystem()->axisList().size() == 3) {
            convDst = geogDst->coordinateSystem()
                          ->axisList()[2]
                          ->unit()
                          .conversionToSI();
        }
        if (convSrc != convDst) {
            const double factor = convSrc / convDst;
            auto conv = Conversion::createChangeVerticalUnit(
                util::PropertyMap().set(
                    common::IdentifiedObject::NAME_KEY,
                    "Transformation from " +
                        *(sourceCRS->name()->description()) + " to " +
                        *(targetCRS->name()->description())),
                common::Scale(factor));
            conv->setCRSs(sourceCRS, targetCRS, nullptr);
            res.push_back(conv);
            return res;
        }
    }

    // reverse of previous case
    if (vertDst && geogSrc) {
        return applyInverse(createOperations(targetCRS, sourceCRS, context));
    }

    // boundCRS to boundCRS using the same geographic hubCRS
    if (boundSrc && boundDst) {
        auto hubSrc = boundSrc->hubCRS();
        auto hubSrcGeog =
            util::nn_dynamic_pointer_cast<crs::GeographicCRS>(hubSrc);
        auto hubDst = boundDst->hubCRS();
        auto hubDstGeog =
            util::nn_dynamic_pointer_cast<crs::GeographicCRS>(hubDst);
        auto geogCRSOfBaseOfBoundSrc =
            boundSrc->baseCRS()->extractGeographicCRS();
        auto geogCRSOfBaseOfBoundDst =
            boundDst->baseCRS()->extractGeographicCRS();
        if (hubSrcGeog && hubDstGeog &&
            hubSrcGeog->isEquivalentTo(
                NN_CHECK_ASSERT(hubDstGeog),
                util::IComparable::Criterion::EQUIVALENT) &&
            geogCRSOfBaseOfBoundSrc && geogCRSOfBaseOfBoundDst) {
            auto opsFirst = createOperations(
                boundSrc->baseCRS(), NN_CHECK_ASSERT(geogCRSOfBaseOfBoundSrc),
                context);
            auto opsLast =
                createOperations(NN_CHECK_ASSERT(geogCRSOfBaseOfBoundDst),
                                 boundDst->baseCRS(), context);
            if (!opsFirst.empty() && !opsLast.empty()) {
                auto opSecond = boundSrc->transformation();
                auto opThird = boundDst->transformation()->inverse();
                for (const auto &opFirst : opsFirst) {
                    for (const auto &opLast : opsLast) {
                        try {
                            res.emplace_back(
                                ConcatenatedOperation::createComputeMetadata(
                                    {
                                        opFirst, opSecond, opThird, opLast,
                                    },
                                    !allowEmptyIntersection));
                        } catch (const InvalidOperationEmptyIntersection &) {
                        }
                    }
                }
                if (!res.empty()) {
                    return res;
                }
            }
        }

        return createOperations(boundSrc->baseCRS(), boundDst->baseCRS(),
                                context);
    }

    auto compoundSrc =
        util::nn_dynamic_pointer_cast<crs::CompoundCRS>(sourceCRS);
    if (compoundSrc && geogDst) {
        auto componentsSrc = compoundSrc->componentReferenceSystems();
        if (!componentsSrc.empty()) {
            std::vector<CoordinateOperationNNPtr> horizTransforms;
            if (componentsSrc[0]->extractGeographicCRS()) {
                horizTransforms =
                    createOperations(componentsSrc[0], targetCRS, context);
            }
            std::vector<CoordinateOperationNNPtr> verticalTransforms;
            if (componentsSrc.size() >= 2 &&
                componentsSrc[1]->extractVerticalCRS()) {
                verticalTransforms =
                    createOperations(componentsSrc[1], targetCRS, context);
            }
            if (!horizTransforms.empty() && !verticalTransforms.empty()) {
                for (const auto &horizTransform : horizTransforms) {
                    for (const auto &verticalTransform : verticalTransforms) {

                        auto op = createHorizVerticalPROJBased(
                            sourceCRS, targetCRS, horizTransform,
                            verticalTransform);

                        res.emplace_back(op);
                    }
                }
                return res;
            } else {
                return horizTransforms;
            }
        }
    }

    // reverse of previous case
    auto compoundDst =
        util::nn_dynamic_pointer_cast<crs::CompoundCRS>(targetCRS);
    if (geogSrc && compoundDst) {
        return applyInverse(createOperations(targetCRS, sourceCRS, context));
    }

    if (compoundSrc && compoundDst) {
        auto componentsSrc = compoundSrc->componentReferenceSystems();
        auto componentsDst = compoundDst->componentReferenceSystems();
        if (!componentsSrc.empty() &&
            componentsSrc.size() == componentsDst.size()) {
            if (componentsSrc[0]->extractGeographicCRS() &&
                componentsDst[0]->extractGeographicCRS()) {

                std::vector<CoordinateOperationNNPtr> verticalTransforms;
                if (componentsSrc.size() >= 2 &&
                    componentsSrc[1]->extractVerticalCRS() &&
                    componentsDst[1]->extractVerticalCRS()) {
                    verticalTransforms = createOperations(
                        componentsSrc[1], componentsDst[1], context);
                }

                for (const auto &verticalTransform : verticalTransforms) {
                    auto interpolationGeogCRS = NN_CHECK_ASSERT(
                        componentsSrc[0]->extractGeographicCRS());
                    auto transformationVerticalTransform =
                        util::nn_dynamic_pointer_cast<Transformation>(
                            verticalTransform);
                    if (transformationVerticalTransform) {
                        auto interpTransformCRS =
                            transformationVerticalTransform->interpolationCRS();
                        if (interpTransformCRS) {
                            auto nn_interpTransformCRS =
                                NN_CHECK_ASSERT(interpTransformCRS);
                            if (util::nn_dynamic_pointer_cast<
                                    crs::GeographicCRS>(
                                    nn_interpTransformCRS)) {
                                interpolationGeogCRS = NN_CHECK_ASSERT(
                                    util::nn_dynamic_pointer_cast<
                                        crs::GeographicCRS>(
                                        nn_interpTransformCRS));
                            }
                        }
                    }
                    auto opSrcCRSToGeogCRS = createOperations(
                        componentsSrc[0], interpolationGeogCRS, context);
                    auto opGeogCRStoDstCRS = createOperations(
                        interpolationGeogCRS, componentsDst[0], context);
                    for (const auto &opSrc : opSrcCRSToGeogCRS) {
                        for (const auto &opDst : opGeogCRStoDstCRS) {

                            auto op = createHorizVerticalHorizPROJBased(
                                sourceCRS, targetCRS, opSrc, verticalTransform,
                                opDst, interpolationGeogCRS);
                            res.emplace_back(op);
                        }
                    }
                }

                if (verticalTransforms.empty()) {
                    return createOperations(componentsSrc[0], componentsDst[0],
                                            context);
                }
            }
        }
    }

    return res;
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
 * @param sourceCRS source CRS.
 * @param targetCRS source CRS.
 * @param context Search context.
 * @return a list
 */
std::vector<CoordinateOperationNNPtr>
CoordinateOperationFactory::createOperations(
    const crs::CRSNNPtr &sourceCRS, const crs::CRSNNPtr &targetCRS,
    CoordinateOperationContextNNPtr context) const {
    return filterAndSort(d->createOperations(sourceCRS, targetCRS, context),
                         context, sourceCRS, targetCRS);
}

// ---------------------------------------------------------------------------

/** \brief Instanciate a CoordinateOperationFactory.
 */
CoordinateOperationFactoryNNPtr CoordinateOperationFactory::create() {
    return CoordinateOperationFactory::nn_make_shared<
        CoordinateOperationFactory>();
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress

InverseCoordinateOperation::~InverseCoordinateOperation() = default;

// ---------------------------------------------------------------------------

InverseCoordinateOperation::InverseCoordinateOperation(
    const CoordinateOperationNNPtr &forwardOperation, bool wktSupportsInversion)
    : forwardOperation_(forwardOperation),
      wktSupportsInversion_(wktSupportsInversion) {}

// ---------------------------------------------------------------------------

void InverseCoordinateOperation::setPropertiesFromForward() {
    setProperties(
        createPropertiesForInverse(forwardOperation_.get(), false, false));
    setAccuracies(forwardOperation_->coordinateOperationAccuracies());
    if (forwardOperation_->sourceCRS() && forwardOperation_->targetCRS()) {
        setCRSs(NN_CHECK_ASSERT(forwardOperation_->targetCRS()),
                NN_CHECK_ASSERT(forwardOperation_->sourceCRS()),
                forwardOperation_->interpolationCRS());
    }
}

// ---------------------------------------------------------------------------

CoordinateOperationNNPtr InverseCoordinateOperation::inverse() const {
    return forwardOperation_;
}

// ---------------------------------------------------------------------------

std::string InverseCoordinateOperation::exportToPROJString(
    io::PROJStringFormatterNNPtr formatter) const {
    io::PROJStringFormatter::Scope scope(formatter);
    formatter->startInversion();
    forwardOperation_->exportToPROJString(formatter);
    formatter->stopInversion();
    return scope.toString();
}

// ---------------------------------------------------------------------------

bool InverseCoordinateOperation::isEquivalentTo(
    const util::BaseObjectNNPtr &other,
    util::IComparable::Criterion criterion) const {
    auto otherICO =
        util::nn_dynamic_pointer_cast<InverseCoordinateOperation>(other);
    if (otherICO == nullptr ||
        !ObjectUsage::_isEquivalentTo(other, criterion)) {
        return false;
    }
    return inverse()->isEquivalentTo(otherICO->inverse(), criterion);
}

// ---------------------------------------------------------------------------

PROJBasedOperation::~PROJBasedOperation() = default;

// ---------------------------------------------------------------------------

PROJBasedOperation::PROJBasedOperation(
    const OperationMethodNNPtr &methodIn,
    const std::vector<GeneralParameterValueNNPtr> &values)
    : SingleOperation(methodIn) {
    setParameterValues(values);
}

// ---------------------------------------------------------------------------

static const std::string PROJSTRING_PARAMETER_NAME("PROJ string");

PROJBasedOperationNNPtr PROJBasedOperation::create(
    const util::PropertyMap &properties, const std::string &PROJString,
    const crs::CRSPtr &sourceCRS, const crs::CRSPtr &targetCRS,
    const std::vector<metadata::PositionalAccuracyNNPtr> &accuracies) {
    auto parameter = OperationParameter::create(util::PropertyMap().set(
        common::IdentifiedObject::NAME_KEY, PROJSTRING_PARAMETER_NAME));
    auto method = OperationMethod::create(
        util::PropertyMap().set(common::IdentifiedObject::NAME_KEY,
                                "PROJ-based operation method"),
        std::vector<OperationParameterNNPtr>{parameter});
    std::vector<GeneralParameterValueNNPtr> values;
    values.push_back(OperationParameterValue::create(
        parameter, ParameterValue::create(PROJString)));
    auto op =
        PROJBasedOperation::nn_make_shared<PROJBasedOperation>(method, values);
    op->assignSelf(op);
    if (sourceCRS && targetCRS) {
        op->setCRSs(NN_CHECK_ASSERT(sourceCRS), NN_CHECK_ASSERT(targetCRS),
                    nullptr);
    }
    op->setProperties(
        addDefaultNameIfNeeded(properties, "PROJ-based coordinate operation"));
    op->setAccuracies(accuracies);
    return op;
}

// ---------------------------------------------------------------------------

static const std::string
    APPROX_PROJSTRING_PARAMETER_NAME("(Approximte) PROJ string");

PROJBasedOperationNNPtr PROJBasedOperation::create(
    const util::PropertyMap &properties,
    const io::IPROJStringExportableNNPtr &projExportable, bool inverse,
    const crs::CRSNNPtr &sourceCRS, const crs::CRSNNPtr &targetCRS,
    const std::vector<metadata::PositionalAccuracyNNPtr> &accuracies) {
    auto parameter = OperationParameter::create(util::PropertyMap().set(
        common::IdentifiedObject::NAME_KEY, APPROX_PROJSTRING_PARAMETER_NAME));
    auto method = OperationMethod::create(
        util::PropertyMap().set(common::IdentifiedObject::NAME_KEY,
                                "PROJ-based operation method"),
        std::vector<OperationParameterNNPtr>{parameter});
    std::vector<GeneralParameterValueNNPtr> values;

    auto formatter = io::PROJStringFormatter::create();
    if (inverse) {
        formatter->startInversion();
    }
    projExportable->exportToPROJString(formatter);
    if (inverse) {
        formatter->stopInversion();
    }
    auto projString = formatter->toString();

    values.push_back(OperationParameterValue::create(
        parameter, ParameterValue::create(projString)));
    auto op =
        PROJBasedOperation::nn_make_shared<PROJBasedOperation>(method, values);
    op->assignSelf(op);
    op->setCRSs(sourceCRS, targetCRS, nullptr);
    op->setProperties(
        addDefaultNameIfNeeded(properties, "PROJ-based coordinate operation"));
    op->setAccuracies(accuracies);
    op->projStringExportable_ = projExportable.as_nullable();
    op->inverse_ = inverse;
    return op;
}

// ---------------------------------------------------------------------------

CoordinateOperationNNPtr PROJBasedOperation::inverse() const {

    if (projStringExportable_) {
        return util::nn_static_pointer_cast<CoordinateOperation>(
            PROJBasedOperation::create(
                createPropertiesForInverse(this, false, false),
                NN_CHECK_ASSERT(projStringExportable_), !inverse_,
                NN_CHECK_ASSERT(targetCRS()), NN_CHECK_ASSERT(sourceCRS()),
                coordinateOperationAccuracies()));
    }

    auto formatter = io::PROJStringFormatter::create();
    formatter->startInversion();
    try {
        formatter->ingestPROJString(
            parameterValue(PROJSTRING_PARAMETER_NAME)->stringValue());
    } catch (const io::ParsingException &e) {
        throw util::UnsupportedOperationException(
            std::string("PROJBasedOperation::inverse() failed: ") + e.what());
    }
    formatter->stopInversion();

    return util::nn_static_pointer_cast<CoordinateOperation>(
        PROJBasedOperation::create(
            createPropertiesForInverse(this, false, false),
            formatter->toString(), targetCRS(), sourceCRS(),
            coordinateOperationAccuracies()));
}

// ---------------------------------------------------------------------------

std::string
PROJBasedOperation::exportToWKT(io::WKTFormatterNNPtr formatter) const {

    if (sourceCRS() && targetCRS()) {
        return exportTransformationToWKT(formatter);
    }

    const bool isWKT2 = formatter->version() == io::WKTFormatter::Version::WKT2;
    if (!isWKT2) {
        throw io::FormattingException(
            "PROJBasedOperation can only be exported to WKT2");
    }

    formatter->startNode(io::WKTConstants::CONVERSION, false);
    formatter->addQuotedString(*(name()->description()));
    method()->exportToWKT(formatter);

    for (const auto &paramValue : parameterValues()) {
        paramValue->exportToWKT(formatter);
    }
    formatter->endNode();

    return formatter->toString();
}

// ---------------------------------------------------------------------------

std::string PROJBasedOperation::exportToPROJString(
    io::PROJStringFormatterNNPtr formatter) const {
    if (projStringExportable_) {
        if (inverse_) {
            formatter->startInversion();
        }
        projStringExportable_->exportToPROJString(formatter);
        if (inverse_) {
            formatter->stopInversion();
        }
        return formatter->toString();
    }

    try {
        formatter->ingestPROJString(
            parameterValue(PROJSTRING_PARAMETER_NAME)->stringValue());
    } catch (const io::ParsingException &e) {
        throw io::FormattingException(
            std::string("PROJBasedOperation::exportToPROJString() failed: ") +
            e.what());
    }
    return formatter->toString();
}

// ---------------------------------------------------------------------------

std::set<CoordinateOperation::GridDescription> PROJBasedOperation::gridsNeeded(
    io::DatabaseContextNNPtr databaseContext) const {
    std::set<CoordinateOperation::GridDescription> res;

    try {
        auto formatterOut = io::PROJStringFormatter::create();
        auto formatter = io::PROJStringFormatter::create();
        formatter->ingestPROJString(exportToPROJString(formatterOut));
        for (const auto &shortName : formatter->getUsedGridNames()) {
            CoordinateOperation::GridDescription desc;
            desc.shortName = shortName;
            databaseContext->lookForGridInfo(
                desc.shortName, desc.fullName, desc.packageName, desc.url,
                desc.directDownload, desc.openLicense, desc.available);
            res.insert(desc);
        }
    } catch (const io::ParsingException &) {
    }

    return res;
}

//! @endcond

// ---------------------------------------------------------------------------

} // namespace operation
NS_PROJ_END
