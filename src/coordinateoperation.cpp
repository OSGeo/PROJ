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
#include "coordinateoperation_constants.hpp"
#include "proj/common.hpp"
#include "proj/coordinateoperation_internal.hpp"
#include "proj/crs.hpp"
#include "proj/internal.hpp"
#include "proj/io.hpp"
#include "proj/io_internal.hpp"
#include "proj/metadata.hpp"
#include "proj/util.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <memory>
#include <set>
#include <sstream> // std::ostringstream
#include <string>
#include <vector>

using namespace NS_PROJ::internal;

// ---------------------------------------------------------------------------

NS_PROJ_START
namespace operation {

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress

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

static const MethodMapping *getMapping(const std::string &wkt2_name) {
    for (const auto &mapping : methodMappings) {
        if (metadata::Identifier::isEquivalentName(mapping.wkt2_name,
                                                   wkt2_name)) {
            return &mapping;
        }
    }
    return nullptr;
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
    method->assignSelf(util::nn_static_pointer_cast<util::BaseObject>(method));
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
                         !identifiers().empty() && !formatter->isInverted());
    std::string l_name(*(name()->description()));
    if (formatter->isInverted()) {
        if (isWKT2) {
            l_name = "Inverse of " + l_name;
        } else {
            throw io::FormattingException(
                "Inverse of projection method not supported in WKT1");
        }
    }
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
    if (formatter->outputId() && !formatter->isInverted()) {
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

    Private(OperationParameterNNPtr parameterIn, ParameterValueNNPtr valueIn)
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
    OperationParameterNNPtr parameterIn, ParameterValueNNPtr valueIn)
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
OperationParameterValue::create(OperationParameterNNPtr parameterIn,
                                ParameterValueNNPtr valueIn) {
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

std::string
OperationParameterValue::exportToWKT(io::WKTFormatterNNPtr formatter) const {
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
        unit = common::UnitOfMeasure::MICRORADIAN;
        val = common::Angle(val, common::UnitOfMeasure::ARC_SECOND)
                  .convertToUnit(unit)
                  .value();
        paramEPSGCode = EPSG_CODE_PARAMETER_X_AXIS_ROTATION;
        return true;
    } else if (metadata::Identifier::isEquivalentName(
                   paramName, EPSG_NAME_PARAMETER_Y_AXIS_ROTATION) ||
               paramEPSGCode == EPSG_CODE_PARAMETER_Y_AXIS_ROTATION) {
        unit = common::UnitOfMeasure::MICRORADIAN;
        val = common::Angle(val, common::UnitOfMeasure::ARC_SECOND)
                  .convertToUnit(unit)
                  .value();
        paramEPSGCode = EPSG_CODE_PARAMETER_Y_AXIS_ROTATION;
        return true;

    } else if (metadata::Identifier::isEquivalentName(
                   paramName, EPSG_NAME_PARAMETER_Z_AXIS_ROTATION) ||
               paramEPSGCode == EPSG_CODE_PARAMETER_Z_AXIS_ROTATION) {
        unit = common::UnitOfMeasure::MICRORADIAN;
        val = common::Angle(val, common::UnitOfMeasure::ARC_SECOND)
                  .convertToUnit(unit)
                  .value();
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
               util::nn_static_pointer_cast<util::BaseObject>(
                   otherOPV->parameterValue()),
               criterion);
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
    op->assignSelf(util::nn_static_pointer_cast<util::BaseObject>(op));
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
    : CoordinateOperation(other), d(internal::make_unique<Private>(*other.d)) {}

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
 * @param PROJString the PROJ string.
 * @param sourceCRS source CRS (might be null).
 * @param targetCRS target CRS (might be null).
 * @return the new instance
 */
SingleOperationNNPtr
SingleOperation::createPROJBased(const std::string &PROJString,
                                 const crs::CRSPtr sourceCRS,
                                 const crs::CRSPtr targetCRS) {
    return util::nn_static_pointer_cast<SingleOperation>(
        PROJBasedOperation::create(PROJString, sourceCRS, targetCRS));
}

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
        if (!values[i]->isEquivalentTo(
                util::nn_static_pointer_cast<util::BaseObject>(otherValues[i]),
                criterion)) {
            return false;
        }
    }
    return true;
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
    : SingleOperation(other), d(internal::make_unique<Private>(*other.d)) {}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
Conversion::~Conversion() = default;
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
    conv->assignSelf(util::nn_static_pointer_cast<util::BaseObject>(conv));
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

/** \brief Clone a Conversion.
 *
 * @param other Conversion to clone.
 * @return a cloned Conversion.
 */
ConversionNNPtr Conversion::create(const ConversionNNPtr &other) {
    auto conv = Conversion::nn_make_shared<Conversion>(*other);
    conv->assignSelf(util::nn_static_pointer_cast<util::BaseObject>(conv));
    return conv;
}

// ---------------------------------------------------------------------------

static util::PropertyMap
getUTMConversionProperty(const util::PropertyMap &properties, int zone,
                         bool north) {
    if (properties.find(common::IdentifiedObject::NAME_KEY) ==
        properties.end()) {
        std::ostringstream conversionName;
        conversionName << "UTM zone ";
        conversionName << zone;
        conversionName << (north ? 'N' : 'S');

        return util::PropertyMap()
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
        return util::PropertyMap().set(common::IdentifiedObject::NAME_KEY,
                                       defaultName);
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

    std::vector<ParameterValueNNPtr> values{
        ParameterValue::create(common::Angle(UTM_LATITUDE_OF_NATURAL_ORIGIN)),
        ParameterValue::create(common::Angle(zone * 6.0 - 183.0)),
        ParameterValue::create(common::Scale(UTM_SCALE_FACTOR)),
        ParameterValue::create(common::Length(UTM_FALSE_EASTING)),
        ParameterValue::create(common::Length(
            north ? UTM_NORTH_FALSE_NORTHING : UTM_SOUTH_FALSE_NORTHING)),
    };

    return create(getUTMConversionProperty(properties, zone, north),
                  EPSG_CODE_METHOD_TRANSVERSE_MERCATOR, values);
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
    std::vector<ParameterValueNNPtr> values{
        ParameterValue::create(centerLat),
        ParameterValue::create(centerLong),
        ParameterValue::create(scale),
        ParameterValue::create(falseEasting),
        ParameterValue::create(falseNorthing),
    };

    return create(properties, EPSG_CODE_METHOD_TRANSVERSE_MERCATOR, values);
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
    std::vector<ParameterValueNNPtr> values{
        ParameterValue::create(centerLat),
        ParameterValue::create(centerLong),
        ParameterValue::create(scale),
        ParameterValue::create(falseEasting),
        ParameterValue::create(falseNorthing),
    };

    return create(properties,
                  PROJ_WKT2_NAME_METHOD_GAUSS_SCHREIBER_TRANSVERSE_MERCATOR,
                  values);
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
    std::vector<ParameterValueNNPtr> values{
        ParameterValue::create(centerLat),
        ParameterValue::create(centerLong),
        ParameterValue::create(scale),
        ParameterValue::create(falseEasting),
        ParameterValue::create(falseNorthing),
    };

    return create(properties,
                  EPSG_CODE_METHOD_TRANSVERSE_MERCATOR_SOUTH_ORIENTATED,
                  values);
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
    std::vector<ParameterValueNNPtr> values{
        ParameterValue::create(latitudeFirstPoint),
        ParameterValue::create(longitudeFirstPoint),
        ParameterValue::create(latitudeSecondPoint),
        ParameterValue::create(longitudeSeconPoint),
        ParameterValue::create(falseEasting),
        ParameterValue::create(falseNorthing),
    };

    return create(properties, PROJ_WKT2_NAME_METHOD_TWO_POINT_EQUIDISTANT,
                  values);
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
    std::vector<ParameterValueNNPtr> values{
        ParameterValue::create(centerLat), ParameterValue::create(centerLong),
        ParameterValue::create(falseEasting),
        ParameterValue::create(falseNorthing),
    };

    return create(properties, EPSG_CODE_METHOD_TUNISIA_MAPPING_GRID, values);
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
    std::vector<ParameterValueNNPtr> values{
        ParameterValue::create(latitudeFalseOrigin),
        ParameterValue::create(longitudeFalseOrigin),
        ParameterValue::create(latitudeFirstParallel),
        ParameterValue::create(latitudeSecondParallel),
        ParameterValue::create(eastingFalseOrigin),
        ParameterValue::create(northingFalseOrigin),
    };

    return create(properties, EPSG_CODE_METHOD_ALBERS_EQUAL_AREA, values);
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
    std::vector<ParameterValueNNPtr> values{
        ParameterValue::create(centerLat),
        ParameterValue::create(centerLong),
        ParameterValue::create(scale),
        ParameterValue::create(falseEasting),
        ParameterValue::create(falseNorthing),
    };

    return create(properties, EPSG_CODE_METHOD_LAMBERT_CONIC_CONFORMAL_1SP,
                  values);
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
    std::vector<ParameterValueNNPtr> values{
        ParameterValue::create(latitudeFalseOrigin),
        ParameterValue::create(longitudeFalseOrigin),
        ParameterValue::create(latitudeFirstParallel),
        ParameterValue::create(latitudeSecondParallel),
        ParameterValue::create(eastingFalseOrigin),
        ParameterValue::create(northingFalseOrigin),
    };

    return create(properties, EPSG_CODE_METHOD_LAMBERT_CONIC_CONFORMAL_2SP,
                  values);
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
    std::vector<ParameterValueNNPtr> values{
        ParameterValue::create(latitudeFalseOrigin),
        ParameterValue::create(longitudeFalseOrigin),
        ParameterValue::create(latitudeFirstParallel),
        ParameterValue::create(latitudeSecondParallel),
        ParameterValue::create(eastingFalseOrigin),
        ParameterValue::create(northingFalseOrigin),
    };

    return create(properties,
                  EPSG_CODE_METHOD_LAMBERT_CONIC_CONFORMAL_2SP_BELGIUM, values);
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
    std::vector<ParameterValueNNPtr> values{
        ParameterValue::create(latitudeNatOrigin),
        ParameterValue::create(longitudeNatOrigin),
        ParameterValue::create(falseEasting),
        ParameterValue::create(falseNorthing),
    };

    return create(properties, EPSG_CODE_METHOD_MODIFIED_AZIMUTHAL_EQUIDISTANT,
                  values);
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
    std::vector<ParameterValueNNPtr> values{
        ParameterValue::create(latitudeNatOrigin),
        ParameterValue::create(longitudeNatOrigin),
        ParameterValue::create(falseEasting),
        ParameterValue::create(falseNorthing),
    };

    return create(properties, EPSG_CODE_METHOD_GUAM_PROJECTION, values);
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
    std::vector<ParameterValueNNPtr> values{
        ParameterValue::create(latitudeNatOrigin),
        ParameterValue::create(longitudeNatOrigin),
        ParameterValue::create(falseEasting),
        ParameterValue::create(falseNorthing),
    };

    return create(properties, EPSG_CODE_METHOD_BONNE, values);
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
    std::vector<ParameterValueNNPtr> values{
        ParameterValue::create(latitudeFirstParallel),
        ParameterValue::create(longitudeNatOrigin),
        ParameterValue::create(falseEasting),
        ParameterValue::create(falseNorthing),
    };

    return create(properties,
                  EPSG_CODE_METHOD_LAMBERT_CYLINDRICAL_EQUAL_AREA_SPHERICAL,
                  values);
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
    std::vector<ParameterValueNNPtr> values{
        ParameterValue::create(latitudeFirstParallel),
        ParameterValue::create(longitudeNatOrigin),
        ParameterValue::create(falseEasting),
        ParameterValue::create(falseNorthing),
    };

    return create(properties, EPSG_CODE_METHOD_LAMBERT_CYLINDRICAL_EQUAL_AREA,
                  values);
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
    std::vector<ParameterValueNNPtr> values{
        ParameterValue::create(centerLat), ParameterValue::create(centerLong),
        ParameterValue::create(falseEasting),
        ParameterValue::create(falseNorthing),
    };

    return create(properties, EPSG_CODE_METHOD_CASSINI_SOLDNER, values);
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
    std::vector<ParameterValueNNPtr> values{
        ParameterValue::create(centerLat),
        ParameterValue::create(centerLong),
        ParameterValue::create(latitudeFirstParallel),
        ParameterValue::create(latitudeSecondParallel),
        ParameterValue::create(falseEasting),
        ParameterValue::create(falseNorthing),
    };

    return create(properties, PROJ_WKT2_NAME_METHOD_EQUIDISTANT_CONIC, values);
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
    std::vector<ParameterValueNNPtr> values{
        ParameterValue::create(centerLong),
        ParameterValue::create(falseEasting),
        ParameterValue::create(falseNorthing),
    };

    return create(properties, PROJ_WKT2_NAME_METHOD_ECKERT_I, values);
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
    std::vector<ParameterValueNNPtr> values{
        ParameterValue::create(centerLong),
        ParameterValue::create(falseEasting),
        ParameterValue::create(falseNorthing),
    };

    return create(properties, PROJ_WKT2_NAME_METHOD_ECKERT_II, values);
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
    std::vector<ParameterValueNNPtr> values{
        ParameterValue::create(centerLong),
        ParameterValue::create(falseEasting),
        ParameterValue::create(falseNorthing),
    };

    return create(properties, PROJ_WKT2_NAME_METHOD_ECKERT_III, values);
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
    std::vector<ParameterValueNNPtr> values{
        ParameterValue::create(centerLong),
        ParameterValue::create(falseEasting),
        ParameterValue::create(falseNorthing),
    };

    return create(properties, PROJ_WKT2_NAME_METHOD_ECKERT_IV, values);
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
    std::vector<ParameterValueNNPtr> values{
        ParameterValue::create(centerLong),
        ParameterValue::create(falseEasting),
        ParameterValue::create(falseNorthing),
    };

    return create(properties, PROJ_WKT2_NAME_METHOD_ECKERT_V, values);
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
    std::vector<ParameterValueNNPtr> values{
        ParameterValue::create(centerLong),
        ParameterValue::create(falseEasting),
        ParameterValue::create(falseNorthing),
    };

    return create(properties, PROJ_WKT2_NAME_METHOD_ECKERT_VI, values);
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
    std::vector<ParameterValueNNPtr> values{
        ParameterValue::create(latitudeFirstParallel),
        ParameterValue::create(longitudeNatOrigin),
        ParameterValue::create(falseEasting),
        ParameterValue::create(falseNorthing),
    };

    return create(properties,
                  EPSG_CODE_METHOD_EQUIDISTANT_CYLINDRICAL_SPHERICAL, values);
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
    std::vector<ParameterValueNNPtr> values{
        ParameterValue::create(centerLong),
        ParameterValue::create(falseEasting),
        ParameterValue::create(falseNorthing),
    };

    return create(properties, PROJ_WKT2_NAME_METHOD_GALL, values);
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
    std::vector<ParameterValueNNPtr> values{
        ParameterValue::create(centerLong),
        ParameterValue::create(falseEasting),
        ParameterValue::create(falseNorthing),
    };

    return create(properties, PROJ_WKT2_NAME_METHOD_GOODE_HOMOLOSINE, values);
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
    std::vector<ParameterValueNNPtr> values{
        ParameterValue::create(centerLong),
        ParameterValue::create(falseEasting),
        ParameterValue::create(falseNorthing),
    };

    return create(properties,
                  PROJ_WKT2_NAME_METHOD_INTERRUPTED_GOODE_HOMOLOSINE, values);
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
    std::vector<ParameterValueNNPtr> values{
        ParameterValue::create(centerLong), ParameterValue::create(height),
        ParameterValue::create(falseEasting),
        ParameterValue::create(falseNorthing),
    };

    return create(properties,
                  PROJ_WKT2_NAME_METHOD_GEOSTATIONARY_SATELLITE_SWEEP_X,
                  values);
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
    std::vector<ParameterValueNNPtr> values{
        ParameterValue::create(centerLong), ParameterValue::create(height),
        ParameterValue::create(falseEasting),
        ParameterValue::create(falseNorthing),
    };

    return create(properties,
                  PROJ_WKT2_NAME_METHOD_GEOSTATIONARY_SATELLITE_SWEEP_Y,
                  values);
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
    std::vector<ParameterValueNNPtr> values{
        ParameterValue::create(centerLat), ParameterValue::create(centerLong),
        ParameterValue::create(falseEasting),
        ParameterValue::create(falseNorthing),
    };

    return create(properties, PROJ_WKT2_NAME_METHOD_GNOMONIC, values);
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
    std::vector<ParameterValueNNPtr> values{
        ParameterValue::create(latitudeProjectionCentre),
        ParameterValue::create(longitudeProjectionCentre),
        ParameterValue::create(azimuthInitialLine),
        ParameterValue::create(angleFromRectifiedToSkrewGrid),
        ParameterValue::create(scale),
        ParameterValue::create(falseEasting),
        ParameterValue::create(falseNorthing),
    };

    return create(properties,
                  EPSG_CODE_METHOD_HOTINE_OBLIQUE_MERCATOR_VARIANT_A, values);
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
    std::vector<ParameterValueNNPtr> values{
        ParameterValue::create(latitudeProjectionCentre),
        ParameterValue::create(longitudeProjectionCentre),
        ParameterValue::create(azimuthInitialLine),
        ParameterValue::create(angleFromRectifiedToSkrewGrid),
        ParameterValue::create(scale),
        ParameterValue::create(eastingProjectionCentre),
        ParameterValue::create(northingProjectionCentre),
    };

    return create(properties,
                  EPSG_CODE_METHOD_HOTINE_OBLIQUE_MERCATOR_VARIANT_B, values);
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
    std::vector<ParameterValueNNPtr> values{
        ParameterValue::create(latitudeProjectionCentre),
        ParameterValue::create(latitudePoint1),
        ParameterValue::create(longitudePoint1),
        ParameterValue::create(latitudePoint2),
        ParameterValue::create(longitudePoint2),
        ParameterValue::create(scale),
        ParameterValue::create(eastingProjectionCentre),
        ParameterValue::create(northingProjectionCentre),
    };

    return create(
        properties,
        PROJ_WKT2_NAME_METHOD_HOTINE_OBLIQUE_MERCATOR_TWO_POINT_NATURAL_ORIGIN,
        values);
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
    std::vector<ParameterValueNNPtr> values{
        ParameterValue::create(centerLong),
        ParameterValue::create(latitudeFirstParallel),
        ParameterValue::create(latitudeSecondParallel),
        ParameterValue::create(falseEasting),
        ParameterValue::create(falseNorthing),
    };

    return create(properties, PROJ_WKT2_NAME_INTERNATIONAL_MAP_WORLD_POLYCONIC,
                  values);
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
 * colatitudeConeAxis = 30°17'17.30311"
 * and latitudePseudoStandardParallel = 78°30'N, which are the values used for
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
    std::vector<ParameterValueNNPtr> values{
        ParameterValue::create(latitudeProjectionCentre),
        ParameterValue::create(longitudeOfOrigin),
        ParameterValue::create(colatitudeConeAxis),
        ParameterValue::create(latitudePseudoStandardParallel),
        ParameterValue::create(scaleFactorPseudoStandardParallel),
        ParameterValue::create(falseEasting),
        ParameterValue::create(falseNorthing),
    };

    return create(properties, EPSG_CODE_METHOD_KROVAK_NORTH_ORIENTED, values);
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
 * colatitudeConeAxis = 30°17'17.30311"
 * and latitudePseudoStandardParallel = 78°30'N, which are the values used for
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
    std::vector<ParameterValueNNPtr> values{
        ParameterValue::create(latitudeProjectionCentre),
        ParameterValue::create(longitudeOfOrigin),
        ParameterValue::create(colatitudeConeAxis),
        ParameterValue::create(latitudePseudoStandardParallel),
        ParameterValue::create(scaleFactorPseudoStandardParallel),
        ParameterValue::create(falseEasting),
        ParameterValue::create(falseNorthing),
    };

    return create(properties, EPSG_CODE_METHOD_KROVAK, values);
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
    std::vector<ParameterValueNNPtr> values{
        ParameterValue::create(latitudeNatOrigin),
        ParameterValue::create(longitudeNatOrigin),
        ParameterValue::create(falseEasting),
        ParameterValue::create(falseNorthing),
    };

    return create(properties, EPSG_CODE_METHOD_LAMBERT_AZIMUTHAL_EQUAL_AREA,
                  values);
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
    std::vector<ParameterValueNNPtr> values{
        ParameterValue::create(centerLat), ParameterValue::create(centerLong),
        ParameterValue::create(falseEasting),
        ParameterValue::create(falseNorthing),
    };

    return create(properties, PROJ_WKT2_NAME_METHOD_MILLER_CYLINDRICAL, values);
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
    std::vector<ParameterValueNNPtr> values{
        ParameterValue::create(centerLat),
        ParameterValue::create(centerLong),
        ParameterValue::create(scale),
        ParameterValue::create(falseEasting),
        ParameterValue::create(falseNorthing),
    };

    return create(properties, EPSG_CODE_METHOD_MERCATOR_VARIANT_A, values);
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
    std::vector<ParameterValueNNPtr> values{
        ParameterValue::create(latitudeFirstParallel),
        ParameterValue::create(centerLong),
        ParameterValue::create(falseEasting),
        ParameterValue::create(falseNorthing),
    };

    return create(properties, EPSG_CODE_METHOD_MERCATOR_VARIANT_B, values);
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
    std::vector<ParameterValueNNPtr> values{
        ParameterValue::create(centerLat), ParameterValue::create(centerLong),
        ParameterValue::create(falseEasting),
        ParameterValue::create(falseNorthing),
    };

    return create(properties,
                  EPSG_CODE_METHOD_POPULAR_VISUALISATION_PSEUDO_MERCATOR,
                  values);
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
    std::vector<ParameterValueNNPtr> values{
        ParameterValue::create(centerLong),
        ParameterValue::create(falseEasting),
        ParameterValue::create(falseNorthing),
    };

    return create(properties,
                  PROJ_WKT2_NAME_METHOD_MOLLWEIDE, values);
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
    std::vector<ParameterValueNNPtr> values{
        ParameterValue::create(centerLat), ParameterValue::create(centerLong),
        ParameterValue::create(falseEasting),
        ParameterValue::create(falseNorthing),
    };

    return create(properties, EPSG_CODE_METHOD_NZMG, values);
}

// ---------------------------------------------------------------------------

/** \brief Instanciate a conversion based on the [Oblique Stereographic (Alternative)]
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
    std::vector<ParameterValueNNPtr> values{
        ParameterValue::create(centerLat),
        ParameterValue::create(centerLong),
        ParameterValue::create(scale),
        ParameterValue::create(falseEasting),
        ParameterValue::create(falseNorthing),
    };

    return create(properties, EPSG_CODE_METHOD_OBLIQUE_STEREOGRAPHIC, values);
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
    const common::Angle &centerLong,
    const common::Length &falseEasting, const common::Length &falseNorthing) {
    std::vector<ParameterValueNNPtr> values{
        ParameterValue::create(centerLat),
        ParameterValue::create(centerLong),
        ParameterValue::create(falseEasting),
        ParameterValue::create(falseNorthing),
    };

    return create(properties, EPSG_CODE_METHOD_ORTHOGRAPHIC, values);
}

// ---------------------------------------------------------------------------

CoordinateOperationNNPtr Conversion::inverse() const {
    return util::nn_make_shared<InverseCoordinateOperation>(
        util::nn_static_pointer_cast<CoordinateOperation>(shared_from_this()),
        true);
}

// ---------------------------------------------------------------------------

std::string Conversion::exportToWKT(io::WKTFormatterNNPtr formatter) const {
    const bool isWKT2 = formatter->version() == io::WKTFormatter::Version::WKT2;
    if (isWKT2) {
        formatter->startNode(formatter->useDerivingConversion()
                                 ? io::WKTConstants::DERIVINGCONVERSION
                                 : io::WKTConstants::CONVERSION,
                             !identifiers().empty() &&
                                 !formatter->isInverted());
        if (formatter->isInverted()) {
            formatter->addQuotedString("Inverse of " +
                                       *(name()->description()));
        } else {
            formatter->addQuotedString(*(name()->description()));
        }
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
        if (formatter->outputId() && !formatter->isInverted()) {
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
    const double a =
        geogCRS->datum()->ellipsoid()->semiMajorAxis().getSIValue();
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

void Conversion::addWKTExtensionNode(io::WKTFormatterNNPtr formatter) const {
    const bool isWKT2 = formatter->version() == io::WKTFormatter::Version::WKT2;
    if (!isWKT2) {
        auto projectionMethodName = *(method()->name()->description());
        const int methodEPSGCode = method()->getEPSGCode();
        if (ci_equal(projectionMethodName,
                     EPSG_NAME_METHOD_POPULAR_VISUALISATION_PSEUDO_MERCATOR) ||
            methodEPSGCode ==
                EPSG_CODE_METHOD_POPULAR_VISUALISATION_PSEUDO_MERCATOR) {

            auto projFormatter = io::PROJStringFormatter::create();
            if (createPROJ4WebMercator(this, projFormatter)) {
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

    if (sourceCRS() &&
        formatter->convention() ==
            io::PROJStringFormatter::Convention::PROJ_5) {
        auto geogCRS =
            std::dynamic_pointer_cast<crs::GeographicCRS>(sourceCRS());
        if (geogCRS) {
            formatter->setOmitProjLongLatIfPossible(true);
            formatter->startInversion();
            geogCRS->exportToPROJString(formatter);
            formatter->stopInversion();
            formatter->setOmitProjLongLatIfPossible(false);
        }
    }

    auto projectionMethodName = *(method()->name()->description());
    const int methodEPSGCode = method()->getEPSGCode();
    bool bConversionDone = false;
    bool bEllipsoidParametersDone = false;
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
    }

    bool bAxisSpecFound = false;
    if (!bConversionDone) {
        const MethodMapping *mapping = getMapping(projectionMethodName);
        if (!mapping && methodEPSGCode) {
            mapping = getMapping(methodEPSGCode);
        }
        if (mapping && !mapping->proj_name[0].empty()) {
            formatter->addStep(mapping->proj_name[0]);
            for (size_t i = 1; i < mapping->proj_name.size(); ++i) {
                if (internal::starts_with(mapping->proj_name[i], "axis=")) {
                    bAxisSpecFound = true;
                }
                formatter->addParam(mapping->proj_name[i]);
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

    if (targetCRS()) {
        auto projCRS =
            std::dynamic_pointer_cast<crs::ProjectedCRS>(targetCRS());
        if (projCRS) {
            if (!bEllipsoidParametersDone) {
                if (formatter->convention() ==
                    io::PROJStringFormatter::Convention::PROJ_4) {
                    projCRS->baseCRS()->addDatumInfoToPROJString(formatter);
                } else {
                    projCRS->baseCRS()
                        ->datum()
                        ->ellipsoid()
                        ->exportToPROJString(formatter);
                }
            }

            auto &axisList = projCRS->coordinateSystem()->axisList();
            if (!axisList.empty() &&
                axisList[0]->unit() != common::UnitOfMeasure::METRE) {
                auto projUnit = axisList[0]->unit().exportToPROJString();
                if (formatter->convention() ==
                    io::PROJStringFormatter::Convention::PROJ_5) {
                    formatter->addStep("unitconvert");
                    formatter->addParam("xy_in", "m");
                    formatter->addParam("z_in", "m");
                    if (projUnit.empty()) {
                        formatter->addParam(
                            "xy_out", axisList[0]->unit().conversionToSI());
                        formatter->addParam(
                            "z_out", axisList[0]->unit().conversionToSI());
                    } else {
                        formatter->addParam("xy_out", projUnit);
                        formatter->addParam("z_out", projUnit);
                    }
                } else {
                    if (projUnit.empty()) {
                        formatter->addParam(
                            "to_meter", axisList[0]->unit().conversionToSI());
                    } else {
                        formatter->addParam("units", projUnit);
                    }
                }
            }

            if (formatter->convention() ==
                    io::PROJStringFormatter::Convention::PROJ_5 &&
                !bAxisSpecFound) {
                if (axisList.size() >= 2 &&
                    !(axisList[0]->direction() == cs::AxisDirection::EAST &&
                      axisList[1]->direction() == cs::AxisDirection::NORTH) &&
                    // For polar projections, that have south+south direction,
                    // we don't want to mess with axes.
                    axisList[0]->direction() != axisList[1]->direction()) {

                    std::string order[2];
                    for (int i = 0; i < 2; i++) {
                        if (axisList[i]->direction() == cs::AxisDirection::WEST)
                            order[i] = "-1";
                        else if (axisList[i]->direction() ==
                                 cs::AxisDirection::EAST)
                            order[i] = "1";
                        else if (axisList[i]->direction() ==
                                 cs::AxisDirection::SOUTH)
                            order[i] = "-2";
                        else if (axisList[i]->direction() ==
                                 cs::AxisDirection::NORTH)
                            order[i] = "2";
                    }

                    if (!order[0].empty() && !order[1].empty()) {
                        formatter->addStep("axisswap");
                        formatter->addParam("order", order[0] + "," + order[1]);
                    }
                }
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
    newConversion->assignSelf(
        util::nn_static_pointer_cast<util::BaseObject>(newConversion));

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
struct Transformation::Private {};
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
        method()->isEPSG(EPSG_CODE_METHOD_COORDINATE_FRAME_GEOGRAPHIC)) {
        sevenParamsTransform = true;
        invertRotSigns = true;
    } else if (ci_find(method_name, "Position Vector") != std::string::npos ||
               method()->isEPSG(EPSG_CODE_METHOD_POSITION_VECTOR_GEOCENTRIC) ||
               method()->isEPSG(EPSG_CODE_METHOD_POSITION_VECTOR_GEOGRAPHIC)) {
        sevenParamsTransform = true;
        invertRotSigns = false;
    } else if (ci_find(method_name, "Geocentric translations") !=
                   std::string::npos ||
               method()->isEPSG(
                   EPSG_CODE_METHOD_GEOCENTRIC_TRANSLATION_GEOCENTRIC) ||
               method()->isEPSG(
                   EPSG_CODE_METHOD_GEOCENTRIC_TRANSLATION_GEOGRAPHIC)) {
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
                                            common::UnitOfMeasure::MICRORADIAN)
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
                                            common::UnitOfMeasure::MICRORADIAN)
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
                                            common::UnitOfMeasure::MICRORADIAN)
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

    throw io::FormattingException(
        "Transformation cannot be formatted as WKT1 TOWGS84 parameters");
}

// ---------------------------------------------------------------------------

/** \brief Instanciate a Transformation from a vector of GeneralParameterValue.
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
    conv->assignSelf(util::nn_static_pointer_cast<util::BaseObject>(conv));
    conv->setProperties(properties);
    return conv;
}

// ---------------------------------------------------------------------------

/** \brief Instanciate a Transformation ands its OperationMethod.
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

static TransformationNNPtr createSevenParamsTransform(
    const util::PropertyMap &properties,
    const util::PropertyMap &methodProperties, const crs::CRSNNPtr &sourceCRSIn,
    const crs::CRSNNPtr &targetCRSIn, double translationXMetre,
    double translationYMetre, double translationZMetre,
    double rotationXMicroRadian, double rotationYMicroRadian,
    double rotationZMicroRadian, double scaleDifferencePPM,
    const std::vector<metadata::PositionalAccuracyNNPtr> &accuracies) {
    std::vector<OperationParameterNNPtr> parameters{
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
    };
    std::vector<ParameterValueNNPtr> values{
        ParameterValue::create(common::Length(translationXMetre)),
        ParameterValue::create(common::Length(translationYMetre)),
        ParameterValue::create(common::Length(translationZMetre)),
        ParameterValue::create(common::Angle(
            rotationXMicroRadian, common::UnitOfMeasure::MICRORADIAN)),
        ParameterValue::create(common::Angle(
            rotationYMicroRadian, common::UnitOfMeasure::MICRORADIAN)),
        ParameterValue::create(common::Angle(
            rotationZMicroRadian, common::UnitOfMeasure::MICRORADIAN)),
        ParameterValue::create(common::Scale(
            scaleDifferencePPM, common::UnitOfMeasure::PARTS_PER_MILLION)),
    };

    return Transformation::create(properties, sourceCRSIn, targetCRSIn, nullptr,
                                  methodProperties, parameters, values,
                                  accuracies);
}

// ---------------------------------------------------------------------------

/** \brief Instanciate a Transformation with Geocentric Translations method.
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
    std::vector<OperationParameterNNPtr> parameters{
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
    };
    std::vector<ParameterValueNNPtr> values{
        ParameterValue::create(common::Length(translationXMetre)),
        ParameterValue::create(common::Length(translationYMetre)),
        ParameterValue::create(common::Length(translationZMetre)),
    };

    return create(properties, sourceCRSIn, targetCRSIn, nullptr,
                  util::PropertyMap()
                      .set(common::IdentifiedObject::NAME_KEY,
                           EPSG_NAME_METHOD_GEOCENTRIC_TRANSLATION_GEOCENTRIC)
                      .set(metadata::Identifier::CODESPACE_KEY,
                           metadata::Identifier::EPSG)
                      .set(metadata::Identifier::CODE_KEY,
                           EPSG_CODE_METHOD_GEOCENTRIC_TRANSLATION_GEOCENTRIC),
                  parameters, values, accuracies);
}

// ---------------------------------------------------------------------------

/** \brief Instanciate a Transformation with Position vector transformation
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
 * @param rotationXMicroRadian Value of the Rotation_X parameter (in
 * microradian).
 * @param rotationYMicroRadian Value of the Rotation_Y parameter (in
 * microradian).
 * @param rotationZMicroRadian Value of the Rotation_Z parameter (in
 * microradian).
 * @param scaleDifferencePPM Value of the Scale_Difference parameter (in
 * parts-per-million).
 * @param accuracies Vector of positional accuracy (might be empty).
 * @return new Transformation.
 */
TransformationNNPtr Transformation::createPositionVector(
    const util::PropertyMap &properties, const crs::CRSNNPtr &sourceCRSIn,
    const crs::CRSNNPtr &targetCRSIn, double translationXMetre,
    double translationYMetre, double translationZMetre,
    double rotationXMicroRadian, double rotationYMicroRadian,
    double rotationZMicroRadian, double scaleDifferencePPM,
    const std::vector<metadata::PositionalAccuracyNNPtr> &accuracies) {
    return createSevenParamsTransform(
        properties, util::PropertyMap()
                        .set(common::IdentifiedObject::NAME_KEY,
                             EPSG_NAME_METHOD_POSITION_VECTOR_GEOCENTRIC)
                        .set(metadata::Identifier::CODESPACE_KEY,
                             metadata::Identifier::EPSG)
                        .set(metadata::Identifier::CODE_KEY,
                             EPSG_CODE_METHOD_POSITION_VECTOR_GEOCENTRIC),
        sourceCRSIn, targetCRSIn, translationXMetre, translationYMetre,
        translationZMetre, rotationXMicroRadian, rotationYMicroRadian,
        rotationZMicroRadian, scaleDifferencePPM, accuracies);
}

// ---------------------------------------------------------------------------

/** \brief Instanciate a Transformation with Coordinate Frame Rotation method.
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
 * @param rotationXMicroRadian Value of the Rotation_X parameter (in
 * microradian).
 * @param rotationYMicroRadian Value of the Rotation_Y parameter (in
 * microradian).
 * @param rotationZMicroRadian Value of the Rotation_Z parameter (in
 * microradian).
 * @param scaleDifferencePPM Value of the Scale_Difference parameter (in
 * parts-per-million).
 * @param accuracies Vector of positional accuracy (might be empty).
 * @return new Transformation.
 */
TransformationNNPtr Transformation::createCoordinateFrameRotation(
    const util::PropertyMap &properties, const crs::CRSNNPtr &sourceCRSIn,
    const crs::CRSNNPtr &targetCRSIn, double translationXMetre,
    double translationYMetre, double translationZMetre,
    double rotationXMicroRadian, double rotationYMicroRadian,
    double rotationZMicroRadian, double scaleDifferencePPM,
    const std::vector<metadata::PositionalAccuracyNNPtr> &accuracies) {
    return createSevenParamsTransform(
        properties, util::PropertyMap()
                        .set(common::IdentifiedObject::NAME_KEY,
                             EPSG_NAME_METHOD_COORDINATE_FRAME_GEOCENTRIC)
                        .set(metadata::Identifier::CODESPACE_KEY,
                             metadata::Identifier::EPSG)
                        .set(metadata::Identifier::CODE_KEY,
                             EPSG_CODE_METHOD_COORDINATE_FRAME_GEOCENTRIC),
        sourceCRSIn, targetCRSIn, translationXMetre, translationYMetre,
        translationZMetre, rotationXMicroRadian, rotationYMicroRadian,
        rotationZMicroRadian, scaleDifferencePPM, accuracies);
}

// ---------------------------------------------------------------------------

/** \brief Instanciate a Transformation from TOWGS84 parameters.
 *
 * This is a helper of createPositionVector() with the source CRS being the
 * GeographicCRS of sourceCRSIn, and the target CRS being EPSG:4326
 *
 * @param sourceCRSIn Source CRS.
 * @param TOWGS84Parameters The vector of 7 double values (Translation_X,_Y,_Z,
 * Rotation_X,_Y,_Z, Scale_Difference) passed to createPositionVector()
 * @return new Transformation.
 */
TransformationNNPtr Transformation::createTOWGS84(
    const crs::CRSNNPtr &sourceCRSIn,
    const std::vector<double> &TOWGS84Parameters) // throw InvalidOperation
{
    if (TOWGS84Parameters.size() != 7) {
        throw InvalidOperation(
            "Invalid number of elements in TOWGS84Parameters");
    }

    crs::CRSPtr transformSourceCRS = sourceCRSIn->extractGeographicCRS();
    if (!transformSourceCRS) {
        throw InvalidOperation(
            "Cannot find GeographicCRS in sourceCRS of TOWGS84 transformation");
    }

    return createPositionVector(
        util::PropertyMap().set(
            common::IdentifiedObject::NAME_KEY,
            "Transformation from " +
                *(transformSourceCRS->name()->description()) + " to WGS84"),
        NN_CHECK_ASSERT(transformSourceCRS), crs::GeographicCRS::EPSG_4326,
        TOWGS84Parameters[0], TOWGS84Parameters[1], TOWGS84Parameters[2],
        TOWGS84Parameters[3], TOWGS84Parameters[4], TOWGS84Parameters[5],
        TOWGS84Parameters[6], std::vector<metadata::PositionalAccuracyNNPtr>());
}

// ---------------------------------------------------------------------------

/** \brief Instanciate a Transformation with NTv2 method.
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
        std::vector<OperationParameterNNPtr>{OperationParameter::create(
            util::PropertyMap()
                .set(common::IdentifiedObject::NAME_KEY,
                     EPSG_NAME_PARAMETER_NTV2_FILENAME)
                .set(metadata::Identifier::CODESPACE_KEY,
                     metadata::Identifier::EPSG)
                .set(metadata::Identifier::CODE_KEY,
                     EPSG_CODE_PARAMETER_NTV2_FILENAME))},
        std::vector<ParameterValueNNPtr>{
            ParameterValue::createFilename(filename)},
        accuracies);
}

// ---------------------------------------------------------------------------

/** \brief Instanciate a Transformation from GravityRelatedHeight to
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

    return create(
        properties, sourceCRSIn, targetCRSIn, nullptr,
        util::PropertyMap().set(common::IdentifiedObject::NAME_KEY,
                                PROJ_WKT2_NAME_METHOD_HEIGHT_TO_GEOG3D),
        std::vector<OperationParameterNNPtr>{OperationParameter::create(
            util::PropertyMap()
                .set(common::IdentifiedObject::NAME_KEY,
                     EPSG_NAME_PARAMETER_GEOID_CORRECTION_FILENAME)
                .set(metadata::Identifier::CODESPACE_KEY,
                     metadata::Identifier::EPSG)
                .set(metadata::Identifier::CODE_KEY,
                     EPSG_CODE_PARAMETER_GEOID_CORRECTION_FILENAME))},
        std::vector<ParameterValueNNPtr>{
            ParameterValue::createFilename(filename)},
        accuracies);
}

// ---------------------------------------------------------------------------

/** \brief Instanciate a Transformation with method VERTCON
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
        std::vector<OperationParameterNNPtr>{OperationParameter::create(
            util::PropertyMap()
                .set(common::IdentifiedObject::NAME_KEY,
                     EPSG_NAME_PARAMETER_VERTICAL_OFFSET_FILE)
                .set(metadata::Identifier::CODESPACE_KEY,
                     metadata::Identifier::EPSG)
                .set(metadata::Identifier::CODE_KEY,
                     EPSG_CODE_PARAMETER_VERTICAL_OFFSET_FILE))},
        std::vector<ParameterValueNNPtr>{
            ParameterValue::createFilename(filename)},
        accuracies);
}

// ---------------------------------------------------------------------------

CoordinateOperationNNPtr Transformation::inverse() const {
    // some Transformations like Helmert ones can be inverted as Transformation
    bool sevenParamsTransform = false;
    bool threeParamsTransform = false;
    auto method_name = *(method()->name()->description());
    if (ci_find(method_name, "Coordinate Frame") != std::string::npos ||
        method()->isEPSG(EPSG_CODE_METHOD_COORDINATE_FRAME_GEOCENTRIC) ||
        method()->isEPSG(EPSG_CODE_METHOD_COORDINATE_FRAME_GEOGRAPHIC)) {
        sevenParamsTransform = true;
    } else if (ci_find(method_name, "Position Vector") != std::string::npos ||
               method()->isEPSG(EPSG_CODE_METHOD_POSITION_VECTOR_GEOCENTRIC) ||
               method()->isEPSG(EPSG_CODE_METHOD_POSITION_VECTOR_GEOGRAPHIC)) {
        sevenParamsTransform = true;
    } else if (ci_find(method_name, "Geocentric translations") !=
                   std::string::npos ||
               method()->isEPSG(
                   EPSG_CODE_METHOD_GEOCENTRIC_TRANSLATION_GEOCENTRIC) ||
               method()->isEPSG(
                   EPSG_CODE_METHOD_GEOCENTRIC_TRANSLATION_GEOGRAPHIC)) {
        threeParamsTransform = true;
    }
    if (threeParamsTransform || sevenParamsTransform) {
        double x =
            -parameterValueMeasure(EPSG_NAME_PARAMETER_X_AXIS_TRANSLATION,
                                   EPSG_CODE_PARAMETER_X_AXIS_TRANSLATION)
                 .getSIValue();
        double y =
            -parameterValueMeasure(EPSG_NAME_PARAMETER_Y_AXIS_TRANSLATION,
                                   EPSG_CODE_PARAMETER_Y_AXIS_TRANSLATION)
                 .getSIValue();
        double z =
            -parameterValueMeasure(EPSG_NAME_PARAMETER_Z_AXIS_TRANSLATION,
                                   EPSG_CODE_PARAMETER_Z_AXIS_TRANSLATION)
                 .getSIValue();
        if (sevenParamsTransform) {
            double rx =
                -parameterValueMeasure(EPSG_NAME_PARAMETER_X_AXIS_ROTATION,
                                       EPSG_CODE_PARAMETER_X_AXIS_ROTATION)
                     .convertToUnit(common::UnitOfMeasure::MICRORADIAN)
                     .value();
            double ry =
                -parameterValueMeasure(EPSG_NAME_PARAMETER_Y_AXIS_ROTATION,
                                       EPSG_CODE_PARAMETER_Y_AXIS_ROTATION)
                     .convertToUnit(common::UnitOfMeasure::MICRORADIAN)
                     .value();
            double rz =
                -parameterValueMeasure(EPSG_NAME_PARAMETER_Z_AXIS_ROTATION,
                                       EPSG_CODE_PARAMETER_Z_AXIS_ROTATION)
                     .convertToUnit(common::UnitOfMeasure::MICRORADIAN)
                     .value();
            double scaleDiff =
                parameterValueMeasure(EPSG_NAME_PARAMETER_SCALE_DIFFERENCE,
                                      EPSG_CODE_PARAMETER_SCALE_DIFFERENCE)
                    .convertToUnit(common::UnitOfMeasure::PARTS_PER_MILLION)
                    .value();
            auto methodProperties =
                util::PropertyMap().set(common::IdentifiedObject::NAME_KEY,
                                        *(method()->name()->description()));
            int method_epsg_code = method()->getEPSGCode();
            if (method_epsg_code) {
                methodProperties
                    .set(metadata::Identifier::CODESPACE_KEY,
                         metadata::Identifier::EPSG)
                    .set(metadata::Identifier::CODE_KEY, method_epsg_code);
            }
            return createSevenParamsTransform(
                util::PropertyMap().set(
                    common::IdentifiedObject::NAME_KEY,
                    "Transformation from " +
                        *(targetCRS()->name()->description()) + " to " +
                        *(sourceCRS()->name()->description())),
                methodProperties, targetCRS(), sourceCRS(), x, y, z, rx, ry, rz,
                scaleDiff, coordinateOperationAccuracies());
        } else {
            return createGeocentricTranslations(
                util::PropertyMap().set(
                    common::IdentifiedObject::NAME_KEY,
                    "Transformation from " +
                        *(targetCRS()->name()->description()) + " to " +
                        *(sourceCRS()->name()->description())),
                targetCRS(), sourceCRS(), x, y, z,
                coordinateOperationAccuracies());
        }
    }

    return util::nn_make_shared<InverseCoordinateOperation>(
        util::nn_static_pointer_cast<CoordinateOperation>(shared_from_this()),
        true);
}

// ---------------------------------------------------------------------------

std::string Transformation::exportToWKT(io::WKTFormatterNNPtr formatter) const {
    const bool isWKT2 = formatter->version() == io::WKTFormatter::Version::WKT2;
    if (!isWKT2) {
        throw io::FormattingException(
            "Transformation can only be exported to WKT2");
    }

    if (formatter->abridgedTransformation()) {
        formatter->startNode(io::WKTConstants::ABRIDGEDTRANSFORMATION,
                             !identifiers().empty() &&
                                 !formatter->isInverted());
    } else {
        formatter->startNode(io::WKTConstants::COORDINATEOPERATION,
                             !identifiers().empty() &&
                                 !formatter->isInverted());
    }
    if (formatter->isInverted()) {
        formatter->addQuotedString("Inverse of " + *(name()->description()));
    } else {
        formatter->addQuotedString(*(name()->description()));
    }

    if (!formatter->abridgedTransformation()) {
        formatter->startNode(io::WKTConstants::SOURCECRS, false);
        if (formatter->isInverted()) {
            targetCRS()->exportToWKT(formatter);
        } else {
            sourceCRS()->exportToWKT(formatter);
        }
        formatter->endNode();

        formatter->startNode(io::WKTConstants::TARGETCRS, false);
        if (formatter->isInverted()) {
            sourceCRS()->exportToWKT(formatter);
        } else {
            targetCRS()->exportToWKT(formatter);
        }
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

    if (!formatter->isInverted()) {
        ObjectUsage::_exportToWKT(formatter);
    }
    formatter->endNode();

    return formatter->toString();
}

// ---------------------------------------------------------------------------

/** \brief Return the filename of a NTv2 operation
 *
 * @return filename, or empty string
 */
std::string Transformation::getNTv2Filename() const {

    auto method_name = *(method()->name()->description());
    if (ci_find(method_name, EPSG_NAME_METHOD_NTV2) != std::string::npos ||
        method()->isEPSG(EPSG_CODE_METHOD_NTV2)) {
        auto fileParameter = parameterValue(EPSG_NAME_PARAMETER_NTV2_FILENAME,
                                            EPSG_CODE_PARAMETER_NTV2_FILENAME);
        if (fileParameter &&
            fileParameter->type() == ParameterValue::Type::FILENAME) {
            return fileParameter->valueFile();
        }
    }
    return std::string();
}

// ---------------------------------------------------------------------------

/** \brief Return the filename of a height to Geographic3D operation
 *
 * @return filename, or empty string
 */
std::string Transformation::getHeightToGeographic3DFilename() const {

    auto method_name = *(method()->name()->description());

    if (ci_find(method_name, PROJ_WKT2_NAME_METHOD_HEIGHT_TO_GEOG3D) !=
        std::string::npos) {
        auto fileParameter =
            parameterValue(EPSG_NAME_PARAMETER_GEOID_CORRECTION_FILENAME,
                           EPSG_CODE_PARAMETER_GEOID_CORRECTION_FILENAME);
        if (fileParameter &&
            fileParameter->type() == ParameterValue::Type::FILENAME) {
            return fileParameter->valueFile();
        }
    }
    return std::string();
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
    auto method_name = *(method()->name()->description());
    if (ci_find(method_name, "Coordinate Frame") != std::string::npos ||
        method()->isEPSG(EPSG_CODE_METHOD_COORDINATE_FRAME_GEOCENTRIC) ||
        method()->isEPSG(EPSG_CODE_METHOD_COORDINATE_FRAME_GEOGRAPHIC)) {
        sevenParamsTransform = true;
        positionVectorConvention = false;
    } else if (ci_find(method_name, "Position Vector") != std::string::npos ||
               method()->isEPSG(EPSG_CODE_METHOD_POSITION_VECTOR_GEOCENTRIC) ||
               method()->isEPSG(EPSG_CODE_METHOD_POSITION_VECTOR_GEOGRAPHIC)) {
        sevenParamsTransform = true;
    } else if (ci_find(method_name, "Geocentric translations") !=
                   std::string::npos ||
               method()->isEPSG(
                   EPSG_CODE_METHOD_GEOCENTRIC_TRANSLATION_GEOCENTRIC) ||
               method()->isEPSG(
                   EPSG_CODE_METHOD_GEOCENTRIC_TRANSLATION_GEOGRAPHIC)) {
        threeParamsTransform = true;
    }
    if (threeParamsTransform || sevenParamsTransform) {
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
            sourceCRSGeog->datum()->ellipsoid()->exportToPROJString(formatter);
        } else {
            auto sourceCRSGeod =
                util::nn_dynamic_pointer_cast<crs::GeodeticCRS>(sourceCRS());
            if (!sourceCRSGeod) {
                throw io::FormattingException(
                    "Can apply Helmert only to GeodeticCRS / GeographicCRS");
            }
            formatter->startInversion();
            sourceCRSGeod->addGeocentricUnitConversionIntoPROJString(formatter);
            formatter->stopInversion();
        }

        formatter->addStep("helmert");
        formatter->addParam("x", x);
        formatter->addParam("y", y);
        formatter->addParam("z", z);
        if (sevenParamsTransform) {
            double rx =
                parameterValueMeasure(EPSG_NAME_PARAMETER_X_AXIS_ROTATION,
                                      EPSG_CODE_PARAMETER_X_AXIS_ROTATION)
                    .convertToUnit(common::UnitOfMeasure::MICRORADIAN)
                    .value();
            double ry =
                parameterValueMeasure(EPSG_NAME_PARAMETER_Y_AXIS_ROTATION,
                                      EPSG_CODE_PARAMETER_Y_AXIS_ROTATION)
                    .convertToUnit(common::UnitOfMeasure::MICRORADIAN)
                    .value();
            double rz =
                parameterValueMeasure(EPSG_NAME_PARAMETER_Z_AXIS_ROTATION,
                                      EPSG_CODE_PARAMETER_Z_AXIS_ROTATION)
                    .convertToUnit(common::UnitOfMeasure::MICRORADIAN)
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
            if (!positionVectorConvention) {
                formatter->addParam("transpose");
            }
        }

        auto targetCRSGeog =
            util::nn_dynamic_pointer_cast<crs::GeographicCRS>(targetCRS());
        if (targetCRSGeog) {
            formatter->addStep("cart");
            formatter->setCurrentStepInverted(true);
            targetCRSGeog->datum()->ellipsoid()->exportToPROJString(formatter);

            targetCRSGeog->exportToPROJString(formatter);
        } else {
            auto targetCRSGeod =
                util::nn_dynamic_pointer_cast<crs::GeodeticCRS>(targetCRS());
            if (!targetCRSGeod) {
                throw io::FormattingException(
                    "Can apply Helmert only to GeodeticCRS / GeographicCRS");
            }
            targetCRSGeod->addGeocentricUnitConversionIntoPROJString(formatter);
        }

        return scope.toString();
    }

    auto NTv2Filename = getNTv2Filename();
    if (!NTv2Filename.empty()) {
        io::PROJStringFormatter::Scope scope(formatter);
        formatter->addStep("hgridshift");
        formatter->addParam("grids", NTv2Filename);
        return scope.toString();
    }

    auto heightFilename = getHeightToGeographic3DFilename();
    if (!heightFilename.empty()) {
        io::PROJStringFormatter::Scope scope(formatter);
        formatter->addStep("vgridshift");
        formatter->addParam("grids", heightFilename);
        return scope.toString();
    }

    if (ci_find(method_name, EPSG_NAME_METHOD_VERTCON) != std::string::npos ||
        method()->isEPSG(EPSG_CODE_METHOD_VERTCON)) {
        auto fileParameter =
            parameterValue(EPSG_NAME_PARAMETER_VERTICAL_OFFSET_FILE,
                           EPSG_CODE_PARAMETER_VERTICAL_OFFSET_FILE);
        if (fileParameter &&
            fileParameter->type() == ParameterValue::Type::FILENAME) {
            io::PROJStringFormatter::Scope scope(formatter);
            formatter->addStep("vgridshift");
            // The vertcon grids go from NGVD 29 to NAVD 88, with units
            // in millimeter (see https://github.com/OSGeo/proj.4/issues/1071)
            formatter->addParam("grids", fileParameter->valueFile());
            formatter->addParam("multiplier", 0.001);
            return scope.toString();
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
 * @param properties See \ref general_properties. At minimum the name should be
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
    for (size_t i = 0; i < operationsIn.size(); i++) {
        if (operationsIn[i]->sourceCRS() == nullptr ||
            operationsIn[i]->targetCRS() == nullptr) {
            throw InvalidOperation("At least one of the operation lacks a "
                                   "source and/or target CRS");
        }
        if (i >= 1 &&
            !operationsIn[i]->sourceCRS()->isEquivalentTo(
                NN_CHECK_ASSERT(operationsIn[i - 1]->targetCRS()))) {
            throw InvalidOperation(
                "Inconsistent chaining of CRS in operations");
        }
    }
    auto op = ConcatenatedOperation::nn_make_shared<ConcatenatedOperation>(
        operationsIn);
    op->assignSelf(util::nn_static_pointer_cast<util::BaseObject>(op));
    op->setProperties(properties);
    op->setCRSs(NN_CHECK_ASSERT(operationsIn[0]->sourceCRS()),
                NN_CHECK_ASSERT(operationsIn.back()->targetCRS()), nullptr);
    op->setAccuracies(accuracies);
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
    return create(
        util::PropertyMap().set(common::IdentifiedObject::NAME_KEY,
                                "Inverse of " + *(name()->description())),
        inversedOperations, coordinateOperationAccuracies());
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

//! @cond Doxygen_Suppress
struct CoordinateOperationFactory::Private {};
//! @endcond

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
CoordinateOperationFactory::~CoordinateOperationFactory() = default;
//! @endcond

// ---------------------------------------------------------------------------

CoordinateOperationFactory::CoordinateOperationFactory()
    : d(internal::make_unique<Private>()) {}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
#define NN_CO_CAST util::nn_static_pointer_cast<CoordinateOperation>
//! @endcond

/** \brief Find a CoordinateOperation from sourceCRS to targetCRS.
 *
 * @return a CoordinateOperation or nullptr.
 */
CoordinateOperationPtr
CoordinateOperationFactory::createOperation(crs::CRSNNPtr sourceCRS,
                                            crs::CRSNNPtr targetCRS) const {

    auto geodSrc = util::nn_dynamic_pointer_cast<crs::GeodeticCRS>(sourceCRS);
    auto geodDst = util::nn_dynamic_pointer_cast<crs::GeodeticCRS>(targetCRS);
    if (geodSrc && geodDst) {
        auto formatter = io::PROJStringFormatter::create();
        io::PROJStringFormatter::Scope scope(formatter);
        formatter->startInversion();
        geodSrc->exportToPROJString(formatter);
        formatter->stopInversion();
        geodDst->exportToPROJString(formatter);
        return NN_CO_CAST(SingleOperation::createPROJBased(
            scope.toString(), sourceCRS, targetCRS));
    }

    auto derivedSrc = util::nn_dynamic_pointer_cast<crs::DerivedCRS>(sourceCRS);
    if (derivedSrc) {
        auto opSecond = createOperation(derivedSrc->baseCRS(), targetCRS);
        if (!opSecond) {
            return nullptr;
        }
        return ConcatenatedOperation::create(
                   util::PropertyMap(),
                   std::vector<CoordinateOperationNNPtr>{
                       NN_CO_CAST(derivedSrc->derivingConversion()->inverse()),
                       NN_CO_CAST(NN_CHECK_ASSERT(opSecond))},
                   std::vector<metadata::PositionalAccuracyNNPtr>{})
            .as_nullable();
    }

    auto derivedDst = util::nn_dynamic_pointer_cast<crs::DerivedCRS>(targetCRS);
    if (derivedDst) {
        auto opFirst = createOperation(sourceCRS, derivedDst->baseCRS());
        if (!opFirst) {
            return nullptr;
        }
        return ConcatenatedOperation::create(
                   util::PropertyMap(),
                   std::vector<CoordinateOperationNNPtr>{
                       NN_CO_CAST(NN_CHECK_ASSERT(opFirst)),
                       NN_CO_CAST(derivedDst->derivingConversion())},
                   std::vector<metadata::PositionalAccuracyNNPtr>{})
            .as_nullable();
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
            (hubSrcGeog->isEquivalentTo(NN_CHECK_ASSERT(geogDst)) ||
             hubSrcGeog->is2DPartOf3D(NN_CHECK_ASSERT(geogDst))) &&
            geogCRSOfBaseOfBoundSrc) {
            auto opFirst = createOperation(
                boundSrc->baseCRS(), NN_CHECK_ASSERT(geogCRSOfBaseOfBoundSrc));
            if (opFirst) {
                return ConcatenatedOperation::create(
                           util::PropertyMap(),
                           std::vector<CoordinateOperationNNPtr>{
                               NN_CO_CAST(NN_CHECK_ASSERT(opFirst)),
                               NN_CO_CAST(boundSrc->transformation())},
                           std::vector<metadata::PositionalAccuracyNNPtr>{})
                    .as_nullable();
            }
        }

        if (hubSrcGeog &&
            hubSrcGeog->isEquivalentTo(NN_CHECK_ASSERT(geogDst)) &&
            util::nn_dynamic_pointer_cast<crs::VerticalCRS>(
                boundSrc->baseCRS())) {
            return boundSrc->transformation().as_nullable();
        }

        return createOperation(boundSrc->baseCRS(), targetCRS);
    }

    // reverse of previous case
    auto boundDst = util::nn_dynamic_pointer_cast<crs::BoundCRS>(targetCRS);
    auto geogSrc = util::nn_dynamic_pointer_cast<crs::GeographicCRS>(sourceCRS);
    if (geogSrc && boundDst) {
        auto op = createOperation(targetCRS, sourceCRS);
        if (!op) {
            return nullptr;
        }
        return op->inverse();
    }

    // vertCRS (as boundCRS with transformation to target vertCRS) to vertCRS
    auto vertDst = util::nn_dynamic_pointer_cast<crs::VerticalCRS>(targetCRS);
    if (boundSrc && vertDst) {
        auto baseSrcVert = util::nn_dynamic_pointer_cast<crs::VerticalCRS>(
            boundSrc->baseCRS());
        auto hubSrc = boundSrc->hubCRS();
        auto hubSrcVert =
            util::nn_dynamic_pointer_cast<crs::VerticalCRS>(hubSrc);
        if (baseSrcVert && hubSrcVert &&
            vertDst->isEquivalentTo(NN_CHECK_ASSERT(hubSrcVert))) {
            return boundSrc->transformation().as_nullable();
        }

        return createOperation(boundSrc->baseCRS(), targetCRS);
    }

    // reverse of previous case
    auto vertSrc = util::nn_dynamic_pointer_cast<crs::VerticalCRS>(sourceCRS);
    if (boundDst && vertSrc) {
        auto op = createOperation(targetCRS, sourceCRS);
        if (!op) {
            return nullptr;
        }
        return op->inverse();
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
            hubSrcGeog->isEquivalentTo(NN_CHECK_ASSERT(hubDstGeog)) &&
            geogCRSOfBaseOfBoundSrc && geogCRSOfBaseOfBoundDst) {
            auto opFirst = createOperation(
                boundSrc->baseCRS(), NN_CHECK_ASSERT(geogCRSOfBaseOfBoundSrc));
            auto opLast = createOperation(
                NN_CHECK_ASSERT(geogCRSOfBaseOfBoundDst), boundDst->baseCRS());
            if (opFirst && opLast) {
                return ConcatenatedOperation::create(
                           util::PropertyMap(),
                           std::vector<CoordinateOperationNNPtr>{
                               NN_CO_CAST(NN_CHECK_ASSERT(opFirst)),
                               NN_CO_CAST(boundSrc->transformation()),
                               NN_CO_CAST(
                                   boundDst->transformation()->inverse()),
                               NN_CO_CAST(NN_CHECK_ASSERT(opLast)),
                           },
                           std::vector<metadata::PositionalAccuracyNNPtr>{})
                    .as_nullable();
            }
        }

        return createOperation(boundSrc->baseCRS(), boundDst->baseCRS());
    }

    auto compoundSrc =
        util::nn_dynamic_pointer_cast<crs::CompoundCRS>(sourceCRS);
    if (compoundSrc && geogDst) {
        auto componentsSrc = compoundSrc->componentReferenceSystems();
        if (!componentsSrc.empty()) {
            CoordinateOperationPtr horizTransform = nullptr;
            if (componentsSrc[0]->extractGeographicCRS()) {
                horizTransform = createOperation(componentsSrc[0], targetCRS);
            }
            CoordinateOperationPtr verticalTransform = nullptr;
            if (componentsSrc.size() >= 2 &&
                componentsSrc[1]->extractVerticalCRS()) {
                verticalTransform =
                    createOperation(componentsSrc[1], targetCRS);
            }
            if (horizTransform && verticalTransform) {
                auto formatter = io::PROJStringFormatter::create();
                io::PROJStringFormatter::Scope scope(formatter);
                horizTransform->exportToPROJString(formatter);
                formatter->startInversion();
                geogDst->addAngularUnitConvertAndAxisSwap(formatter);
                formatter->stopInversion();
                verticalTransform->exportToPROJString(formatter);
                geogDst->addAngularUnitConvertAndAxisSwap(formatter);
                return NN_CO_CAST(SingleOperation::createPROJBased(
                    scope.toString(), sourceCRS, targetCRS));
            } else {
                return horizTransform;
            }
        }
    }

    // reverse of previous case
    auto compoundDst =
        util::nn_dynamic_pointer_cast<crs::CompoundCRS>(targetCRS);
    if (geogSrc && compoundDst) {
        auto op = createOperation(targetCRS, sourceCRS);
        if (!op) {
            return nullptr;
        }
        return op->inverse();
    }

    if (compoundSrc && compoundDst) {
        auto componentsSrc = compoundSrc->componentReferenceSystems();
        auto componentsDst = compoundDst->componentReferenceSystems();
        if (!componentsSrc.empty() &&
            componentsSrc.size() == componentsDst.size()) {
            if (componentsSrc[0]->extractGeographicCRS() &&
                componentsDst[0]->extractGeographicCRS()) {

                CoordinateOperationPtr verticalTransform = nullptr;
                if (componentsSrc.size() >= 2 &&
                    componentsSrc[1]->extractVerticalCRS() &&
                    componentsDst[1]->extractVerticalCRS()) {
                    verticalTransform =
                        createOperation(componentsSrc[1], componentsDst[1]);
                }

                if (verticalTransform) {
                    auto interpolationGeogCRS = NN_CHECK_ASSERT(
                        componentsSrc[0]->extractGeographicCRS());
                    auto transformationVerticalTransform =
                        util::nn_dynamic_pointer_cast<Transformation>(
                            NN_CHECK_ASSERT(verticalTransform));
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
                    auto opSrcCRSToGeogCRS =
                        createOperation(componentsSrc[0], interpolationGeogCRS);
                    auto opGeogCRStoDstCRS =
                        createOperation(interpolationGeogCRS, componentsDst[0]);
                    if (opSrcCRSToGeogCRS && opGeogCRStoDstCRS) {
                        auto formatter = io::PROJStringFormatter::create();
                        io::PROJStringFormatter::Scope scope(formatter);
                        opSrcCRSToGeogCRS->exportToPROJString(formatter);
                        formatter->startInversion();
                        interpolationGeogCRS->addAngularUnitConvertAndAxisSwap(
                            formatter);
                        formatter->stopInversion();
                        verticalTransform->exportToPROJString(formatter);
                        interpolationGeogCRS->addAngularUnitConvertAndAxisSwap(
                            formatter);
                        opGeogCRStoDstCRS->exportToPROJString(formatter);
                        return NN_CO_CAST(SingleOperation::createPROJBased(
                            scope.toString(), sourceCRS, targetCRS));
                    }
                } else {
                    auto op =
                        createOperation(componentsSrc[0], componentsDst[0]);
                    if (op) {
                        return op;
                    }
                }
            }
        }
    }

    return nullptr;
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
    CoordinateOperationNNPtr forwardOperation, bool wktSupportsInversion)
    : forwardOperation_(forwardOperation),
      wktSupportsInversion_(wktSupportsInversion) {
    auto fwd_description = forwardOperation->name()->description();
    if (fwd_description.has_value()) {
        setProperties(
            util::PropertyMap().set(common::IdentifiedObject::NAME_KEY,
                                    "Inverse of " + *fwd_description));
    }
    setAccuracies(coordinateOperationAccuracies());
    if (forwardOperation->sourceCRS() && forwardOperation->targetCRS()) {
        setCRSs(NN_CHECK_ASSERT(forwardOperation->targetCRS()),
                NN_CHECK_ASSERT(forwardOperation->sourceCRS()),
                forwardOperation->interpolationCRS());
    }
}

// ---------------------------------------------------------------------------

CoordinateOperationNNPtr InverseCoordinateOperation::inverse() const {
    return forwardOperation_;
}

// ---------------------------------------------------------------------------

std::string
InverseCoordinateOperation::exportToWKT(io::WKTFormatterNNPtr formatter) const {
    if (wktSupportsInversion_) {
        formatter->startInversion();
        forwardOperation_->exportToWKT(formatter);
        formatter->stopInversion();
        return formatter->toString();
    }
    throw io::FormattingException(
        "InverseCoordinateOperation::exportToWKT(): unsupported");
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

PROJBasedOperationNNPtr
PROJBasedOperation::create(const std::string &PROJString,
                           const crs::CRSPtr sourceCRS,
                           const crs::CRSPtr targetCRS) {
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
    op->assignSelf(util::nn_static_pointer_cast<util::BaseObject>(op));
    if (sourceCRS && targetCRS) {
        op->setCRSs(NN_CHECK_ASSERT(sourceCRS), NN_CHECK_ASSERT(targetCRS),
                    nullptr);
    }
    op->setProperties(util::PropertyMap().set(
        common::IdentifiedObject::NAME_KEY, "PROJ-based coordinate operation"));
    return op;
}

// ---------------------------------------------------------------------------

CoordinateOperationNNPtr PROJBasedOperation::inverse() const {
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
        create(formatter->toString(), targetCRS(), sourceCRS()));
}

// ---------------------------------------------------------------------------

std::string
PROJBasedOperation::exportToWKT(io::WKTFormatterNNPtr formatter) const {
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

//! @endcond

// ---------------------------------------------------------------------------

} // namespace operation
NS_PROJ_END
