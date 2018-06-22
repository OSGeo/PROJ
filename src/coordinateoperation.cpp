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

#include "proj/coordinateoperation.hpp"
#include "proj/common.hpp"
#include "proj/coordinateoperation_internal.hpp"
#include "proj/crs.hpp"
#include "proj/internal.hpp"
#include "proj/io.hpp"
#include "proj/io_internal.hpp"
#include "proj/metadata.hpp"
#include "proj/util.hpp"

#include <cassert>
#include <cmath>
#include <memory>
#include <sstream> // std::ostringstream
#include <string>
#include <vector>

using namespace NS_PROJ::common;
using namespace NS_PROJ::crs;
using namespace NS_PROJ::internal;
using namespace NS_PROJ::io;
using namespace NS_PROJ::metadata;
using namespace NS_PROJ::operation;
using namespace NS_PROJ::util;

// ---------------------------------------------------------------------------

NS_PROJ_START

namespace operation {

static const std::string EPSG_METHOD_TRANSVERSE_MERCATOR("Transverse Mercator");
constexpr int EPSG_METHOD_TRANSVERSE_MERCATOR_CODE = 9807;

static const std::string
    EPSG_METHOD_LAMBERT_CONIC_CONFORMAL_1SP("Lambert Conic Conformal (1SP)");
constexpr int EPSG_METHOD_LAMBERT_CONIC_CONFORMAL_1SP_CODE = 9801;

static const std::string
    EPSG_LATITUDE_OF_NATURAL_ORIGIN("Latitude of natural origin");
static const std::string
    EPSG_LONGITUDE_OF_NATURAL_ORIGIN("Longitude of natural origin");
static const std::string
    EPSG_SCALE_FACTOR_AT_NATURAL_ORIGIN("Scale factor at natural origin");
static const std::string EPSG_FALSE_EASTING("False easting");
static const std::string EPSG_FALSE_NORTHING("False northing");

static const std::string WKT1_LATITUDE_OF_ORIGIN("latitude_of_origin");
static const std::string WKT1_CENTRAL_MERIDIAN("central_meridian");
static const std::string WKT1_SCALE_FACTOR("scale_factor");
static const std::string WKT1_FALSE_EASTING("false_easting");
static const std::string WKT1_FALSE_NORTHING("false_northing");

constexpr double UTM_LATITUDE_OF_NATURAL_ORIGIN = 0.0;
constexpr double UTM_SCALE_FACTOR = 0.9996;
constexpr double UTM_FALSE_EASTING = 500000.0;
constexpr double UTM_NORTH_FALSE_NORTHING = 0.0;
constexpr double UTM_SOUTH_FALSE_NORTHING = 10000000.0;

constexpr int EPSG_PARAMETER_X_AXIS_TRANSLATION = 8605;
constexpr int EPSG_PARAMETER_Y_AXIS_TRANSLATION = 8606;
constexpr int EPSG_PARAMETER_Z_AXIS_TRANSLATION = 8607;
constexpr int EPSG_PARAMETER_X_AXIS_ROTATION = 8608;
constexpr int EPSG_PARAMETER_Y_AXIS_ROTATION = 8609;
constexpr int EPSG_PARAMETER_Z_AXIS_ROTATION = 8610;
constexpr int EPSG_PARAMETER_SCALE_DIFFERENCE = 8611;

static const std::string
    EPSG_PARAMETER_X_AXIS_TRANSLATION_NAME("X-axis translation");
static const std::string
    EPSG_PARAMETER_Y_AXIS_TRANSLATION_NAME("Y-axis translation");
static const std::string
    EPSG_PARAMETER_Z_AXIS_TRANSLATION_NAME("Z-axis translation");
static const std::string EPSG_PARAMETER_X_AXIS_ROTATION_NAME("X-axis rotation");
static const std::string EPSG_PARAMETER_Y_AXIS_ROTATION_NAME("Y-axis rotation");
static const std::string EPSG_PARAMETER_Z_AXIS_ROTATION_NAME("Z-axis rotation");
static const std::string
    EPSG_PARAMETER_SCALE_DIFFERENCE_NAME("Scale difference");

// ---------------------------------------------------------------------------

static const MethodMapping methodMappings[] = {
    {EPSG_METHOD_TRANSVERSE_MERCATOR,
     EPSG_METHOD_TRANSVERSE_MERCATOR_CODE,
     "Transverse_Mercator",
     {
         {EPSG_LATITUDE_OF_NATURAL_ORIGIN, 8801, WKT1_LATITUDE_OF_ORIGIN},
         {EPSG_LONGITUDE_OF_NATURAL_ORIGIN, 8802, WKT1_CENTRAL_MERIDIAN},
         {EPSG_SCALE_FACTOR_AT_NATURAL_ORIGIN, 8805, WKT1_SCALE_FACTOR},
         {EPSG_FALSE_EASTING, 8806, WKT1_FALSE_EASTING},
         {EPSG_FALSE_NORTHING, 8807, WKT1_FALSE_NORTHING},
     }},

    {EPSG_METHOD_LAMBERT_CONIC_CONFORMAL_1SP,
     EPSG_METHOD_LAMBERT_CONIC_CONFORMAL_1SP_CODE,
     "Lambert_Conformal_Conic_1SP",
     {
         {EPSG_LATITUDE_OF_NATURAL_ORIGIN, 8801, WKT1_LATITUDE_OF_ORIGIN},
         {EPSG_LONGITUDE_OF_NATURAL_ORIGIN, 8802, WKT1_CENTRAL_MERIDIAN},
         {EPSG_SCALE_FACTOR_AT_NATURAL_ORIGIN, 8805, WKT1_SCALE_FACTOR},
         {EPSG_FALSE_EASTING, 8806, WKT1_FALSE_EASTING},
         {EPSG_FALSE_NORTHING, 8807, WKT1_FALSE_NORTHING},
     }},
    // TODO: add at least all GDAL supported methods !!!
};

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
        if (mapping.wkt2_name == name || mapping.epsg_code == epsg_code) {
            return &mapping;
        }
    }
    return nullptr;
}

// ---------------------------------------------------------------------------

const MethodMapping *getMappingFromWKT1(const std::string &wkt1_name) {
    // Unusual for a WKT1 projection name, but mentionned in OGC 12-063r5 C.4.2
    if (tolower(wkt1_name).find(tolower("UTM zone")) == 0) {
        return getMapping(EPSG_METHOD_TRANSVERSE_MERCATOR_CODE);
    }

    for (const auto &mapping : methodMappings) {
        if (Identifier::isEquivalentName(mapping.wkt1_name, wkt1_name)) {
            return &mapping;
        }
    }
    return nullptr;
}

// ---------------------------------------------------------------------------

const MethodMapping::ParamMapping *
getMapping(const MethodMapping *mapping, const OperationParameterValue *param) {
    const auto &param_name = param->parameter()->name();
    const std::string name = *(param_name->description());
    const std::string &code = param_name->code();
    const int epsg_code = !code.empty() ? ::atoi(code.c_str()) : 0;
    for (const auto &paramMapping : mapping->params) {
        if (paramMapping.wkt2_name == name ||
            paramMapping.epsg_code == epsg_code) {
            return &paramMapping;
        }
    }
    return nullptr;
}

// ---------------------------------------------------------------------------

const MethodMapping::ParamMapping *
getMappingFromWKT1(const MethodMapping *mapping, const std::string &wkt1_name) {
    for (const auto &paramMapping : mapping->params) {
        if (paramMapping.wkt1_name == wkt1_name) {
            return &paramMapping;
        }
    }
    return nullptr;
}

} // namespace operation

NS_PROJ_END

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
struct CoordinateOperation::Private {
    optional<std::string> operationVersion_{};
    std::vector<PositionalAccuracyNNPtr> coordinateOperationAccuracies_{};
    std::weak_ptr<CRS> sourceCRSWeak_{};
    std::weak_ptr<CRS> targetCRSWeak_{};
    CRSPtr sourceCRSLocked_{}; // do not set this for a
                               // ProjectedCRS.defininingCoversion
    CRSPtr targetCRSLocked_{}; // do not set this for a
                               // ProjectedCRS.defininingCoversion
    CRSPtr interpolationCRS_{};
    optional<DataEpoch> sourceCoordinateEpoch_{};
    optional<DataEpoch> targetCoordinateEpoch_{};
};
//! @endcond

// ---------------------------------------------------------------------------

CoordinateOperation::CoordinateOperation()
    : d(internal::make_unique<Private>()) {}

// ---------------------------------------------------------------------------

CoordinateOperation::CoordinateOperation(const CoordinateOperation &other)
    : ObjectUsage(other), d(internal::make_unique<Private>(*other.d)) {}

// ---------------------------------------------------------------------------

CoordinateOperation::~CoordinateOperation() = default;

// ---------------------------------------------------------------------------
const optional<std::string> &CoordinateOperation::operationVersion() const {
    return d->operationVersion_;
}

// ---------------------------------------------------------------------------

const std::vector<PositionalAccuracyNNPtr> &
CoordinateOperation::coordinateOperationAccuracies() const {
    return d->coordinateOperationAccuracies_;
}

// ---------------------------------------------------------------------------

const CRSPtr CoordinateOperation::sourceCRS() const {
    return d->sourceCRSWeak_.lock();
}

// ---------------------------------------------------------------------------

const CRSPtr CoordinateOperation::targetCRS() const {
    return d->targetCRSWeak_.lock();
}

// ---------------------------------------------------------------------------

const CRSPtr &CoordinateOperation::interpolationCRS() const {
    return d->interpolationCRS_;
}

// ---------------------------------------------------------------------------

const optional<DataEpoch> &CoordinateOperation::sourceCoordinateEpoch() const {
    return d->sourceCoordinateEpoch_;
}

// ---------------------------------------------------------------------------

const optional<DataEpoch> &CoordinateOperation::targetCoordinateEpoch() const {
    return d->targetCoordinateEpoch_;
}

// ---------------------------------------------------------------------------

void CoordinateOperation::setWeakSourceTargetCRS(
    std::weak_ptr<CRS> sourceCRSIn, std::weak_ptr<CRS> targetCRSIn) {
    d->sourceCRSWeak_ = sourceCRSIn;
    d->targetCRSWeak_ = targetCRSIn;
}

// ---------------------------------------------------------------------------

void CoordinateOperation::setCRSs(const CRSNNPtr &sourceCRSIn,
                                  const CRSNNPtr &targetCRSIn,
                                  const CRSPtr &interpolationCRSIn) {
    d->sourceCRSLocked_ = sourceCRSIn;
    d->sourceCRSWeak_ = sourceCRSIn.as_nullable();
    d->targetCRSLocked_ = targetCRSIn;
    d->targetCRSWeak_ = targetCRSIn.as_nullable();
    d->interpolationCRS_ = interpolationCRSIn;
}

// ---------------------------------------------------------------------------

void CoordinateOperation::setAccuracies(
    const std::vector<PositionalAccuracyNNPtr> &accuracies) {
    d->coordinateOperationAccuracies_ = accuracies;
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
struct OperationMethod::Private {
    optional<std::string> formula_{};
    optional<metadata::Citation> formulaCitation_{};
    std::vector<GeneralOperationParameterNNPtr> parameters_{};
};
//! @endcond

// ---------------------------------------------------------------------------

OperationMethod::OperationMethod() : d(internal::make_unique<Private>()) {}

// ---------------------------------------------------------------------------

OperationMethod::OperationMethod(const OperationMethod &other)
    : IdentifiedObject(other), d(internal::make_unique<Private>(*other.d)) {}

// ---------------------------------------------------------------------------

OperationMethod::~OperationMethod() = default;

// ---------------------------------------------------------------------------

const optional<std::string> &OperationMethod::formula() const {
    return d->formula_;
}

// ---------------------------------------------------------------------------

const optional<Citation> &OperationMethod::formulaCitation() const {
    return d->formulaCitation_;
}

// ---------------------------------------------------------------------------

const std::vector<GeneralOperationParameterNNPtr> &
OperationMethod::parameters() const {
    return d->parameters_;
}

// ---------------------------------------------------------------------------

OperationMethodNNPtr OperationMethod::create(
    const PropertyMap &properties,
    const std::vector<GeneralOperationParameterNNPtr> &parameters) {
    OperationMethodNNPtr method(
        OperationMethod::nn_make_shared<OperationMethod>());
    method->setProperties(properties);
    method->d->parameters_ = parameters;
    return method;
}

// ---------------------------------------------------------------------------

OperationMethodNNPtr OperationMethod::create(
    const PropertyMap &properties,
    const std::vector<OperationParameterNNPtr> &parameters) {
    std::vector<GeneralOperationParameterNNPtr> parametersGeneral;
    parametersGeneral.reserve(parameters.size());
    for (const auto &p : parameters) {
        parametersGeneral.push_back(p);
    }
    return create(properties, parametersGeneral);
}

// ---------------------------------------------------------------------------

std::string OperationMethod::exportToWKT(WKTFormatterNNPtr formatter) const {
    const bool isWKT2 = formatter->version() == WKTFormatter::Version::WKT2;
    formatter->startNode(isWKT2 ? WKTConstants::METHOD
                                : WKTConstants::PROJECTION,
                         !identifiers().empty());
    std::string l_name(*(name()->description()));
    if (!isWKT2) {
        const MethodMapping *mapping = getMapping(this);
        if (mapping == nullptr) {
            l_name = replaceAll(l_name, " ", "_");
        } else {
            l_name = mapping->wkt1_name;
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

//! @cond Doxygen_Suppress
struct GeneralParameterValue::Private {};
//! @endcond

// ---------------------------------------------------------------------------

GeneralParameterValue::GeneralParameterValue()
    : d(internal::make_unique<Private>()) {}

// ---------------------------------------------------------------------------

GeneralParameterValue::GeneralParameterValue(const GeneralParameterValue &other)
    : d(internal::make_unique<Private>(*other.d)) {}

// ---------------------------------------------------------------------------

GeneralParameterValue::~GeneralParameterValue() = default;

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

OperationParameterValueNNPtr
OperationParameterValue::create(OperationParameterNNPtr parameterIn,
                                ParameterValueNNPtr valueIn) {
    OperationParameterValueNNPtr opv(
        OperationParameterValue::nn_make_shared<OperationParameterValue>(
            parameterIn, valueIn));
    return opv;
}

// ---------------------------------------------------------------------------

OperationParameterValue::~OperationParameterValue() = default;

// ---------------------------------------------------------------------------

const OperationParameterNNPtr &OperationParameterValue::parameter() const {
    return d->parameter;
}

// ---------------------------------------------------------------------------

const ParameterValueNNPtr &OperationParameterValue::parameterValue() const {
    return d->parameterValue;
}

// ---------------------------------------------------------------------------

std::string
OperationParameterValue::exportToWKT(WKTFormatterNNPtr formatter) const {
    return _exportToWKT(formatter, nullptr);
}

std::string
OperationParameterValue::_exportToWKT(WKTFormatterNNPtr formatter,
                                      const MethodMapping *mapping) const {
    const MethodMapping::ParamMapping *paramMapping =
        mapping ? getMapping(mapping, this) : nullptr;
    const bool isWKT2 = formatter->version() == WKTFormatter::Version::WKT2;
    if (isWKT2 && parameterValue()->type() == ParameterValue::Type::FILENAME) {
        formatter->startNode(WKTConstants::PARAMETERFILE,
                             !parameter()->identifiers().empty());
    } else {
        formatter->startNode(WKTConstants::PARAMETER,
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

bool OperationParameterValue::convertFromAbridged(const std::string &paramName,
                                                  double &val,
                                                  UnitOfMeasure &unit,
                                                  int &paramEPSGCode) {
    if (Identifier::isEquivalentName(paramName,
                                     EPSG_PARAMETER_X_AXIS_TRANSLATION_NAME) ||
        paramEPSGCode == EPSG_PARAMETER_X_AXIS_TRANSLATION) {
        unit = UnitOfMeasure::METRE;
        paramEPSGCode = EPSG_PARAMETER_X_AXIS_TRANSLATION;
        return true;
    } else if (Identifier::isEquivalentName(
                   paramName, EPSG_PARAMETER_Y_AXIS_TRANSLATION_NAME) ||
               paramEPSGCode == EPSG_PARAMETER_Y_AXIS_TRANSLATION) {
        unit = UnitOfMeasure::METRE;
        paramEPSGCode = EPSG_PARAMETER_Y_AXIS_TRANSLATION;
        return true;
    } else if (Identifier::isEquivalentName(
                   paramName, EPSG_PARAMETER_Z_AXIS_TRANSLATION_NAME) ||
               paramEPSGCode == EPSG_PARAMETER_Z_AXIS_TRANSLATION) {
        unit = UnitOfMeasure::METRE;
        paramEPSGCode = EPSG_PARAMETER_Z_AXIS_TRANSLATION;
        return true;
    } else if (Identifier::isEquivalentName(
                   paramName, EPSG_PARAMETER_X_AXIS_ROTATION_NAME) ||
               paramEPSGCode == EPSG_PARAMETER_X_AXIS_ROTATION) {
        unit = UnitOfMeasure::MICRORADIAN;
        val = Angle(val, UnitOfMeasure::ARC_SECOND).convertToUnit(unit).value();
        paramEPSGCode = EPSG_PARAMETER_X_AXIS_ROTATION;
        return true;
    } else if (Identifier::isEquivalentName(
                   paramName, EPSG_PARAMETER_Y_AXIS_ROTATION_NAME) ||
               paramEPSGCode == EPSG_PARAMETER_Y_AXIS_ROTATION) {
        unit = UnitOfMeasure::MICRORADIAN;
        val = Angle(val, UnitOfMeasure::ARC_SECOND).convertToUnit(unit).value();
        paramEPSGCode = EPSG_PARAMETER_Y_AXIS_ROTATION;
        return true;

    } else if (Identifier::isEquivalentName(
                   paramName, EPSG_PARAMETER_Z_AXIS_ROTATION_NAME) ||
               paramEPSGCode == EPSG_PARAMETER_Z_AXIS_ROTATION) {
        unit = UnitOfMeasure::MICRORADIAN;
        val = Angle(val, UnitOfMeasure::ARC_SECOND).convertToUnit(unit).value();
        paramEPSGCode = EPSG_PARAMETER_Z_AXIS_ROTATION;
        return true;

    } else if (Identifier::isEquivalentName(
                   paramName, EPSG_PARAMETER_SCALE_DIFFERENCE_NAME) ||
               paramEPSGCode == EPSG_PARAMETER_SCALE_DIFFERENCE) {
        val = (val - 1.0) * 1e6;
        unit = UnitOfMeasure::PARTS_PER_MILLION;
        paramEPSGCode = EPSG_PARAMETER_SCALE_DIFFERENCE;
        return true;
    }
    return false;
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

GeneralOperationParameter::~GeneralOperationParameter() = default;

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

OperationParameter::~OperationParameter() = default;

// ---------------------------------------------------------------------------

OperationParameterNNPtr
OperationParameter::create(const PropertyMap &properties) {
    OperationParameterNNPtr op(
        OperationParameter::nn_make_shared<OperationParameter>());
    op->setProperties(properties);
    return op;
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

SingleOperation::~SingleOperation() = default;

// ---------------------------------------------------------------------------

const std::vector<GeneralParameterValueNNPtr> &
SingleOperation::parameterValues() const {
    return d->parameterValues_;
}

// ---------------------------------------------------------------------------

const OperationMethodNNPtr &SingleOperation::method() const {
    return d->method_;
}

// ---------------------------------------------------------------------------

void SingleOperation::setParameterValues(
    const std::vector<GeneralParameterValueNNPtr> &values) {
    d->parameterValues_ = values;
}

// ---------------------------------------------------------------------------

ParameterValuePtr
SingleOperation::parameterValue(const std::string &paramName) const {
    for (const auto &genOpParamvalue : parameterValues()) {
        const auto &opParamvalue =
            util::nn_dynamic_pointer_cast<OperationParameterValue>(
                genOpParamvalue);
        if (opParamvalue) {
            const auto &paramNameIter =
                *(opParamvalue->parameter()->name()->description());
            if (ci_equal(paramName, paramNameIter)) {
                return opParamvalue->parameterValue();
            }
        }
    }
    return nullptr;
}

// ---------------------------------------------------------------------------

Measure
SingleOperation::parameterValueMeasure(const std::string &paramName) const {
    auto val = parameterValue(paramName);
    if (val && val->type() == ParameterValue::Type::MEASURE) {
        return val->value();
    }
    return Measure();
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
struct ParameterValue::Private {
    ParameterValue::Type type_{ParameterValue::Type::STRING};
    Measure measure_{};
    std::string stringValue_{};
    int integerValue_{};
    bool booleanValue_{};

    explicit Private(const Measure &valueIn)
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

ParameterValue::~ParameterValue() = default;

// ---------------------------------------------------------------------------

ParameterValue::ParameterValue(const Measure &measureIn)
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

ParameterValueNNPtr ParameterValue::create(const Measure &measureIn) {
    return ParameterValue::nn_make_shared<ParameterValue>(measureIn);
}

// ---------------------------------------------------------------------------

ParameterValueNNPtr ParameterValue::create(const char *stringValueIn) {
    return ParameterValue::nn_make_shared<ParameterValue>(
        std::string(stringValueIn), ParameterValue::Type::STRING);
}

// ---------------------------------------------------------------------------

ParameterValueNNPtr ParameterValue::create(const std::string &stringValueIn) {
    return ParameterValue::nn_make_shared<ParameterValue>(
        stringValueIn, ParameterValue::Type::STRING);
}

// ---------------------------------------------------------------------------

ParameterValueNNPtr
ParameterValue::createFilename(const std::string &stringValueIn) {
    return ParameterValue::nn_make_shared<ParameterValue>(
        stringValueIn, ParameterValue::Type::FILENAME);
}

// ---------------------------------------------------------------------------

ParameterValueNNPtr ParameterValue::create(int integerValueIn) {
    return ParameterValue::nn_make_shared<ParameterValue>(integerValueIn);
}

// ---------------------------------------------------------------------------

ParameterValueNNPtr ParameterValue::create(bool booleanValueIn) {
    return ParameterValue::nn_make_shared<ParameterValue>(booleanValueIn);
}

// ---------------------------------------------------------------------------

const ParameterValue::Type &ParameterValue::type() const { return d->type_; }

// ---------------------------------------------------------------------------

const Measure &ParameterValue::value() const { return d->measure_; }

// ---------------------------------------------------------------------------

const std::string &ParameterValue::stringValue() const {
    return d->stringValue_;
}

// ---------------------------------------------------------------------------

const std::string &ParameterValue::valueFile() const { return d->stringValue_; }

// ---------------------------------------------------------------------------

int ParameterValue::integerValue() const { return d->integerValue_; }

// ---------------------------------------------------------------------------

bool ParameterValue::booleanValue() const { return d->booleanValue_; }

// ---------------------------------------------------------------------------

std::string ParameterValue::exportToWKT(WKTFormatterNNPtr formatter) const {
    const bool isWKT2 = formatter->version() == WKTFormatter::Version::WKT2;

    if (formatter->abridgedTransformation() && type() == Type::MEASURE) {
        if (value().unit().type() == UnitOfMeasure::Type::LINEAR) {
            formatter->add(value().getSIValue());
        } else if (value().unit().type() == UnitOfMeasure::Type::ANGULAR) {
            formatter->add(
                value().convertToUnit(UnitOfMeasure::ARC_SECOND).value());
        } else if (value().unit() == UnitOfMeasure::PARTS_PER_MILLION) {
            formatter->add(1.0 + value().value() * 1e-6);
        } else {
            formatter->add(value().value());
        }
    } else if (type() == Type::MEASURE) {
        formatter->add(value().value());
        const auto &unit = value().unit();
        if (isWKT2 && unit != UnitOfMeasure::NONE) {
            if (!formatter->primeMeridianOrParameterUnitOmittedIfSameAsAxis() ||
                (unit != UnitOfMeasure::SCALE_UNITY &&
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
        throw FormattingException("boolean parameter value not handled");
    }
    return formatter->toString();
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

Conversion::~Conversion() = default;

// ---------------------------------------------------------------------------

ConversionNNPtr Conversion::create(const PropertyMap &properties,
                                   const OperationMethodNNPtr &methodIn,
                                   const std::vector<GeneralParameterValueNNPtr>
                                       &values) // throw InvalidOperation
{
    if (methodIn->parameters().size() != values.size()) {
        throw InvalidOperation(
            "Inconsistent number of parameters and parameter values");
    }
    auto conv = Conversion::nn_make_shared<Conversion>(methodIn, values);
    conv->setProperties(properties);
    return conv;
}

// ---------------------------------------------------------------------------

ConversionNNPtr Conversion::create(
    const PropertyMap &propertiesConversion,
    const PropertyMap &propertiesOperationMethod,
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

ConversionNNPtr Conversion::create(const ConversionNNPtr &other) {
    return Conversion::nn_make_shared<Conversion>(*other);
}

// ---------------------------------------------------------------------------

static PropertyMap getUTMConversionProperty(const PropertyMap &properties,
                                            int zone, bool north) {
    if (properties.find(IdentifiedObject::NAME_KEY) == properties.end()) {
        std::ostringstream conversionName;
        conversionName << "UTM zone ";
        conversionName << zone;
        conversionName << (north ? 'N' : 'S');

        return PropertyMap()
            .set(IdentifiedObject::NAME_KEY, conversionName.str())
            .set(Identifier::CODESPACE_KEY, "EPSG")
            .set(Identifier::CODE_KEY, (north ? 16000 : 17000) + zone);
    } else {
        return properties;
    }
}

// ---------------------------------------------------------------------------

ConversionNNPtr Conversion::createUTM(const PropertyMap &properties, int zone,
                                      bool north) {
    const MethodMapping *mapping =
        getMapping(EPSG_METHOD_TRANSVERSE_MERCATOR_CODE);
    assert(mapping);
    std::vector<OperationParameterNNPtr> parameters;
    for (const auto &param : mapping->params) {
        parameters.push_back(OperationParameter::create(
            PropertyMap()
                .set(IdentifiedObject::NAME_KEY, param.wkt2_name)
                .set(Identifier::CODESPACE_KEY, "EPSG")
                .set(Identifier::CODE_KEY, param.epsg_code)));
    }

    std::vector<ParameterValueNNPtr> values{
        ParameterValue::create(Angle(UTM_LATITUDE_OF_NATURAL_ORIGIN)),
        ParameterValue::create(Angle(zone * 6.0 - 183.0)),
        ParameterValue::create(Scale(UTM_SCALE_FACTOR)),
        ParameterValue::create(Length(UTM_FALSE_EASTING)),
        ParameterValue::create(Length(north ? UTM_NORTH_FALSE_NORTHING
                                            : UTM_SOUTH_FALSE_NORTHING)),
    };

    return create(
        getUTMConversionProperty(properties, zone, north),
        PropertyMap()
            .set(IdentifiedObject::NAME_KEY, EPSG_METHOD_TRANSVERSE_MERCATOR)
            .set(Identifier::CODESPACE_KEY, "EPSG")
            .set(Identifier::CODE_KEY, EPSG_METHOD_TRANSVERSE_MERCATOR_CODE),
        parameters, values);
}

// ---------------------------------------------------------------------------

static PropertyMap addDefaultNameIfNeeded(const PropertyMap &properties,
                                          const std::string &defaultName) {
    if (properties.find(IdentifiedObject::NAME_KEY) == properties.end()) {
        return PropertyMap().set(IdentifiedObject::NAME_KEY, defaultName);
    } else {
        return properties;
    }
}

// ---------------------------------------------------------------------------

ConversionNNPtr
Conversion::createTM(const PropertyMap &properties, const Angle &centerLat,
                     const Angle &centerLong, const Scale &scale,
                     const Length &falseEasting, const Length &falseNorthing) {
    const MethodMapping *mapping =
        getMapping(EPSG_METHOD_TRANSVERSE_MERCATOR_CODE);
    assert(mapping);
    std::vector<OperationParameterNNPtr> parameters;
    for (const auto &param : mapping->params) {
        parameters.push_back(OperationParameter::create(
            PropertyMap()
                .set(IdentifiedObject::NAME_KEY, param.wkt2_name)
                .set(Identifier::CODESPACE_KEY, "EPSG")
                .set(Identifier::CODE_KEY, param.epsg_code)));
    }

    std::vector<ParameterValueNNPtr> values{
        ParameterValue::create(centerLat),
        ParameterValue::create(centerLong),
        ParameterValue::create(scale),
        ParameterValue::create(falseEasting),
        ParameterValue::create(falseNorthing),
    };

    return create(
        addDefaultNameIfNeeded(properties, EPSG_METHOD_TRANSVERSE_MERCATOR),
        PropertyMap()
            .set(IdentifiedObject::NAME_KEY, EPSG_METHOD_TRANSVERSE_MERCATOR)
            .set(Identifier::CODESPACE_KEY, "EPSG")
            .set(Identifier::CODE_KEY, EPSG_METHOD_TRANSVERSE_MERCATOR_CODE),
        parameters, values);
}

// ---------------------------------------------------------------------------

ConversionNNPtr Conversion::createLCC_1SP(const PropertyMap &properties,
                                          const Angle &centerLat,
                                          const Angle &centerLong,
                                          const Scale &scale,
                                          const Length &falseEasting,
                                          const Length &falseNorthing) {
    const MethodMapping *mapping =
        getMapping(EPSG_METHOD_LAMBERT_CONIC_CONFORMAL_1SP_CODE);
    assert(mapping);
    std::vector<OperationParameterNNPtr> parameters;
    for (const auto &param : mapping->params) {
        parameters.push_back(OperationParameter::create(
            PropertyMap()
                .set(IdentifiedObject::NAME_KEY, param.wkt2_name)
                .set(Identifier::CODESPACE_KEY, "EPSG")
                .set(Identifier::CODE_KEY, param.epsg_code)));
    }

    std::vector<ParameterValueNNPtr> values{
        ParameterValue::create(centerLat),
        ParameterValue::create(centerLong),
        ParameterValue::create(scale),
        ParameterValue::create(falseEasting),
        ParameterValue::create(falseNorthing),
    };

    return create(addDefaultNameIfNeeded(
                      properties, EPSG_METHOD_LAMBERT_CONIC_CONFORMAL_1SP),
                  PropertyMap()
                      .set(IdentifiedObject::NAME_KEY,
                           EPSG_METHOD_LAMBERT_CONIC_CONFORMAL_1SP)
                      .set(Identifier::CODESPACE_KEY, "EPSG")
                      .set(Identifier::CODE_KEY,
                           EPSG_METHOD_LAMBERT_CONIC_CONFORMAL_1SP_CODE),
                  parameters, values);
}

// ---------------------------------------------------------------------------

std::string Conversion::exportToWKT(WKTFormatterNNPtr formatter) const {
    if (formatter->outputConversionNode()) {
        formatter->startNode(formatter->useDerivingConversion()
                                 ? WKTConstants::DERIVINGCONVERSION
                                 : WKTConstants::CONVERSION,
                             !identifiers().empty());
        formatter->addQuotedString(*(name()->description()));
    }
    const bool isWKT2 = formatter->version() == WKTFormatter::Version::WKT2;
    if (!isWKT2) {
        formatter->pushOutputUnit(false);
        formatter->pushOutputId(false);
    }
    method()->exportToWKT(formatter);

    const MethodMapping *mapping =
        !isWKT2 ? getMapping(method().get()) : nullptr;
    for (const auto &paramValue : parameterValues()) {
        paramValue->_exportToWKT(formatter, mapping);
    }

    if (!isWKT2) {
        formatter->popOutputUnit();
        formatter->popOutputId();
    }
    if (formatter->outputConversionNode()) {
        if (formatter->outputId()) {
            formatID(formatter);
        }
        formatter->endNode();
    }
    return formatter->toString();
}

// ---------------------------------------------------------------------------

std::string Conversion::exportToPROJString(
    PROJStringFormatterNNPtr formatter) const // throw(FormattingException)
{
    auto projectionMethodName = *(method()->name()->description());
    if (ci_equal(projectionMethodName, EPSG_METHOD_TRANSVERSE_MERCATOR)) {
        // Check for UTM
        int zone = 0;
        bool north = true;
        if (isUTM(zone, north)) {
            formatter->addStep("utm");
            formatter->addParam("zone", zone);
            if (!north) {
                formatter->addParam("south");
            }
        } else {
            formatter->addStep("tmerc");
            formatter->addParam(
                "lat_0", parameterValueMeasure(EPSG_LATITUDE_OF_NATURAL_ORIGIN)
                             .convertToUnit(UnitOfMeasure::DEGREE)
                             .value());
            formatter->addParam(
                "lon_0", parameterValueMeasure(EPSG_LONGITUDE_OF_NATURAL_ORIGIN)
                             .convertToUnit(UnitOfMeasure::DEGREE)
                             .value());
            formatter->addParam("k_0", parameterValueMeasure(
                                           EPSG_SCALE_FACTOR_AT_NATURAL_ORIGIN)
                                           .getSIValue());
            formatter->addParam(
                "x_0", parameterValueMeasure(EPSG_FALSE_EASTING).getSIValue());
            formatter->addParam(
                "y_0", parameterValueMeasure(EPSG_FALSE_NORTHING).getSIValue());
        }
    } else if (ci_equal(projectionMethodName,
                        EPSG_METHOD_LAMBERT_CONIC_CONFORMAL_1SP)) {

        formatter->addStep("lcc");
        formatter->addParam(
            "lat_1", parameterValueMeasure(EPSG_LATITUDE_OF_NATURAL_ORIGIN)
                         .convertToUnit(UnitOfMeasure::DEGREE)
                         .value());
        formatter->addParam(
            "lat_0", parameterValueMeasure(EPSG_LATITUDE_OF_NATURAL_ORIGIN)
                         .convertToUnit(UnitOfMeasure::DEGREE)
                         .value());
        formatter->addParam(
            "lon_0", parameterValueMeasure(EPSG_LONGITUDE_OF_NATURAL_ORIGIN)
                         .convertToUnit(UnitOfMeasure::DEGREE)
                         .value());
        formatter->addParam(
            "k_0", parameterValueMeasure(EPSG_SCALE_FACTOR_AT_NATURAL_ORIGIN)
                       .getSIValue());
        formatter->addParam(
            "x_0", parameterValueMeasure(EPSG_FALSE_EASTING).getSIValue());
        formatter->addParam(
            "y_0", parameterValueMeasure(EPSG_FALSE_NORTHING).getSIValue());

    } else {
        throw FormattingException("Unsupported conversion method: " +
                                  projectionMethodName);
    }

    return formatter->toString();
}

// ---------------------------------------------------------------------------

bool Conversion::isUTM(int &zone, bool &north) const {
    zone = 0;
    north = true;

    auto projectionMethodName = *(method()->name()->description());
    if (ci_equal(projectionMethodName, EPSG_METHOD_TRANSVERSE_MERCATOR)) {
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
                    if (paramName == EPSG_LATITUDE_OF_NATURAL_ORIGIN &&
                        measure.value() == UTM_LATITUDE_OF_NATURAL_ORIGIN) {
                        bLatitudeNatOriginUTM = true;
                    } else if (paramName == EPSG_LONGITUDE_OF_NATURAL_ORIGIN &&
                               measure.unit() == UnitOfMeasure::DEGREE) {
                        double dfZone = (measure.value() + 183.0) / 6.0;
                        if (dfZone > 0.9 && dfZone < 60.1 &&
                            std::abs(dfZone - std::round(dfZone)) < 1e-10) {
                            zone = static_cast<int>(std::lround(dfZone));
                        }
                    } else if (paramName ==
                                   EPSG_SCALE_FACTOR_AT_NATURAL_ORIGIN &&
                               measure.value() == UTM_SCALE_FACTOR) {
                        bScaleFactorUTM = true;
                    } else if (paramName == EPSG_FALSE_EASTING &&
                               measure.value() == UTM_FALSE_EASTING &&
                               measure.unit() == UnitOfMeasure::METRE) {
                        bFalseEastingUTM = true;
                    } else if (paramName == EPSG_FALSE_NORTHING &&
                               measure.unit() == UnitOfMeasure::METRE) {
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

ConversionNNPtr Conversion::identify() const {
    auto projectionMethodName = *(method()->name()->description());
    auto newConversion = Conversion::nn_make_shared<Conversion>(*this);

    if (ci_equal(projectionMethodName, EPSG_METHOD_TRANSVERSE_MERCATOR)) {
        // Check for UTM
        int zone = 0;
        bool north = true;
        if (isUTM(zone, north)) {
            newConversion->setProperties(
                getUTMConversionProperty(PropertyMap(), zone, north));
        }
    }

    return newConversion;
}

// ---------------------------------------------------------------------------

InvalidOperation::InvalidOperation(const char *message) : Exception(message) {}

// ---------------------------------------------------------------------------

InvalidOperation::InvalidOperation(const std::string &message)
    : Exception(message) {}

// ---------------------------------------------------------------------------

InvalidOperation::InvalidOperation(const InvalidOperation &) = default;

// ---------------------------------------------------------------------------

InvalidOperation::~InvalidOperation() = default;

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
struct Transformation::Private {};
//! @endcond

// ---------------------------------------------------------------------------

Transformation::Transformation(
    const CRSNNPtr &sourceCRSIn, const CRSNNPtr &targetCRSIn,
    const CRSPtr &interpolationCRSIn, const OperationMethodNNPtr &methodIn,
    const std::vector<GeneralParameterValueNNPtr> &values,
    const std::vector<PositionalAccuracyNNPtr> &accuracies)
    : SingleOperation(methodIn), d(internal::make_unique<Private>()) {
    setParameterValues(values);
    setCRSs(sourceCRSIn, targetCRSIn, interpolationCRSIn);
    setAccuracies(accuracies);
}

// ---------------------------------------------------------------------------

Transformation::~Transformation() = default;

// ---------------------------------------------------------------------------

const CRSNNPtr Transformation::sourceCRS() const {
    return NN_CHECK_ASSERT(CoordinateOperation::sourceCRS());
}

// ---------------------------------------------------------------------------

const CRSNNPtr Transformation::targetCRS() const {
    return NN_CHECK_ASSERT(CoordinateOperation::targetCRS());
}

// ---------------------------------------------------------------------------

std::vector<double>
Transformation::getTOWGS84Parameters() const // throw(io::FormattingException)
{
    // GDAL WKT1 assumes EPSG:9606 / Position Vector convention

    bool sevenParamsTransform = false;
    bool threeParamsTransform = false;
    bool invertRotSigns = false;
    auto method_name = *(method()->name()->description());
    if (ci_find(method_name, "Coordinate Frame") != std::string::npos ||
        method()->isEPSG(1032) || method()->isEPSG(9607)) {
        sevenParamsTransform = true;
        invertRotSigns = true;
    } else if (ci_find(method_name, "Position Vector") != std::string::npos ||
               method()->isEPSG(1033) || method()->isEPSG(9606)) {
        sevenParamsTransform = true;
        invertRotSigns = false;
    } else if (ci_find(method_name, "Geocentric translations") !=
                   std::string::npos ||
               method()->isEPSG(1031) || method()->isEPSG(9603)) {
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
                    if (Identifier::isEquivalentName(
                            paramName,
                            EPSG_PARAMETER_X_AXIS_TRANSLATION_NAME) ||
                        parameter->isEPSG(EPSG_PARAMETER_X_AXIS_TRANSLATION)) {
                        params[0] = measure.getSIValue();
                        foundX = true;
                    } else if (Identifier::isEquivalentName(
                                   paramName,
                                   EPSG_PARAMETER_Y_AXIS_TRANSLATION_NAME) ||
                               parameter->isEPSG(
                                   EPSG_PARAMETER_Y_AXIS_TRANSLATION)) {
                        params[1] = measure.getSIValue();
                        foundY = true;
                    } else if (Identifier::isEquivalentName(
                                   paramName,
                                   EPSG_PARAMETER_Z_AXIS_TRANSLATION_NAME) ||
                               parameter->isEPSG(
                                   EPSG_PARAMETER_Z_AXIS_TRANSLATION)) {
                        params[2] = measure.getSIValue();
                        foundZ = true;
                    } else if (Identifier::isEquivalentName(
                                   paramName,
                                   EPSG_PARAMETER_X_AXIS_ROTATION_NAME) ||
                               parameter->isEPSG(
                                   EPSG_PARAMETER_X_AXIS_ROTATION)) {
                        params[3] =
                            rotSign *
                            measure.convertToUnit(UnitOfMeasure::MICRORADIAN)
                                .value();
                        foundRotX = true;
                    } else if (Identifier::isEquivalentName(
                                   paramName,
                                   EPSG_PARAMETER_Y_AXIS_ROTATION_NAME) ||
                               parameter->isEPSG(
                                   EPSG_PARAMETER_Y_AXIS_ROTATION)) {
                        params[4] =
                            rotSign *
                            measure.convertToUnit(UnitOfMeasure::MICRORADIAN)
                                .value();
                        foundRotY = true;
                    } else if (Identifier::isEquivalentName(
                                   paramName,
                                   EPSG_PARAMETER_Z_AXIS_ROTATION_NAME) ||
                               parameter->isEPSG(
                                   EPSG_PARAMETER_Z_AXIS_ROTATION)) {
                        params[5] =
                            rotSign *
                            measure.convertToUnit(UnitOfMeasure::MICRORADIAN)
                                .value();
                        foundRotZ = true;
                    } else if (Identifier::isEquivalentName(
                                   paramName,
                                   EPSG_PARAMETER_SCALE_DIFFERENCE_NAME) ||
                               parameter->isEPSG(
                                   EPSG_PARAMETER_SCALE_DIFFERENCE)) {
                        params[6] =
                            measure
                                .convertToUnit(UnitOfMeasure::PARTS_PER_MILLION)
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
            throw FormattingException(
                "Missing required parameter values in transformation");
        }
    }

    throw FormattingException(
        "Transformation cannot be formatted as WKT1 TOWGS84 parameters");
}

// ---------------------------------------------------------------------------

TransformationNNPtr
Transformation::create(const PropertyMap &properties,
                       const CRSNNPtr &sourceCRSIn, const CRSNNPtr &targetCRSIn,
                       const CRSPtr &interpolationCRSIn,
                       const OperationMethodNNPtr &methodIn,
                       const std::vector<GeneralParameterValueNNPtr> &values,
                       const std::vector<PositionalAccuracyNNPtr>
                           &accuracies) // throw InvalidOperation
{
    if (methodIn->parameters().size() != values.size()) {
        throw InvalidOperation(
            "Inconsistent number of parameters and parameter values");
    }
    auto conv = Transformation::nn_make_shared<Transformation>(
        sourceCRSIn, targetCRSIn, interpolationCRSIn, methodIn, values,
        accuracies);
    conv->setProperties(properties);
    return conv;
}

// ---------------------------------------------------------------------------

TransformationNNPtr
Transformation::create(const PropertyMap &propertiesTransformation,
                       const CRSNNPtr &sourceCRSIn, const CRSNNPtr &targetCRSIn,
                       const CRSPtr &interpolationCRSIn,
                       const PropertyMap &propertiesOperationMethod,
                       const std::vector<OperationParameterNNPtr> &parameters,
                       const std::vector<ParameterValueNNPtr> &values,
                       const std::vector<PositionalAccuracyNNPtr>
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
    const PropertyMap &properties, const PropertyMap &methodProperties,
    const CRSNNPtr &sourceCRSIn, const CRSNNPtr &targetCRSIn,
    double translationXMetre, double translationYMetre,
    double translationZMetre, double rotationXMicroRadian,
    double rotationYMicroRadian, double rotationZMicroRadian,
    double scaleDifferencePPM,
    const std::vector<PositionalAccuracyNNPtr> &accuracies) {
    std::vector<OperationParameterNNPtr> parameters{
        OperationParameter::create(
            PropertyMap()
                .set(IdentifiedObject::NAME_KEY,
                     EPSG_PARAMETER_X_AXIS_TRANSLATION_NAME)
                .set(Identifier::CODESPACE_KEY, "EPSG")
                .set(Identifier::CODE_KEY, EPSG_PARAMETER_X_AXIS_TRANSLATION)),
        OperationParameter::create(
            PropertyMap()
                .set(IdentifiedObject::NAME_KEY,
                     EPSG_PARAMETER_Y_AXIS_TRANSLATION_NAME)
                .set(Identifier::CODESPACE_KEY, "EPSG")
                .set(Identifier::CODE_KEY, EPSG_PARAMETER_Y_AXIS_TRANSLATION)),
        OperationParameter::create(
            PropertyMap()
                .set(IdentifiedObject::NAME_KEY,
                     EPSG_PARAMETER_Z_AXIS_TRANSLATION_NAME)
                .set(Identifier::CODESPACE_KEY, "EPSG")
                .set(Identifier::CODE_KEY, EPSG_PARAMETER_Z_AXIS_TRANSLATION)),
        OperationParameter::create(
            PropertyMap()
                .set(IdentifiedObject::NAME_KEY,
                     EPSG_PARAMETER_X_AXIS_ROTATION_NAME)
                .set(Identifier::CODESPACE_KEY, "EPSG")
                .set(Identifier::CODE_KEY, EPSG_PARAMETER_X_AXIS_ROTATION)),
        OperationParameter::create(
            PropertyMap()
                .set(IdentifiedObject::NAME_KEY,
                     EPSG_PARAMETER_Y_AXIS_ROTATION_NAME)
                .set(Identifier::CODESPACE_KEY, "EPSG")
                .set(Identifier::CODE_KEY, EPSG_PARAMETER_Y_AXIS_ROTATION)),
        OperationParameter::create(
            PropertyMap()
                .set(IdentifiedObject::NAME_KEY,
                     EPSG_PARAMETER_Z_AXIS_ROTATION_NAME)
                .set(Identifier::CODESPACE_KEY, "EPSG")
                .set(Identifier::CODE_KEY, EPSG_PARAMETER_Z_AXIS_ROTATION)),
        OperationParameter::create(
            PropertyMap()
                .set(IdentifiedObject::NAME_KEY,
                     EPSG_PARAMETER_SCALE_DIFFERENCE_NAME)
                .set(Identifier::CODESPACE_KEY, "EPSG")
                .set(Identifier::CODE_KEY, EPSG_PARAMETER_SCALE_DIFFERENCE)),
    };
    std::vector<ParameterValueNNPtr> values{
        ParameterValue::create(Length(translationXMetre)),
        ParameterValue::create(Length(translationYMetre)),
        ParameterValue::create(Length(translationZMetre)),
        ParameterValue::create(
            Angle(rotationXMicroRadian, UnitOfMeasure::MICRORADIAN)),
        ParameterValue::create(
            Angle(rotationYMicroRadian, UnitOfMeasure::MICRORADIAN)),
        ParameterValue::create(
            Angle(rotationZMicroRadian, UnitOfMeasure::MICRORADIAN)),
        ParameterValue::create(
            Scale(scaleDifferencePPM, UnitOfMeasure::PARTS_PER_MILLION)),
    };

    return Transformation::create(properties, sourceCRSIn, targetCRSIn, nullptr,
                                  methodProperties, parameters, values,
                                  accuracies);
}

// ---------------------------------------------------------------------------

TransformationNNPtr Transformation::createGeocentricTranslations(
    const PropertyMap &properties, const CRSNNPtr &sourceCRSIn,
    const CRSNNPtr &targetCRSIn, double translationXMetre,
    double translationYMetre, double translationZMetre,
    const std::vector<PositionalAccuracyNNPtr> &accuracies) {
    std::vector<OperationParameterNNPtr> parameters{
        OperationParameter::create(
            PropertyMap()
                .set(IdentifiedObject::NAME_KEY,
                     EPSG_PARAMETER_X_AXIS_TRANSLATION_NAME)
                .set(Identifier::CODESPACE_KEY, "EPSG")
                .set(Identifier::CODE_KEY, EPSG_PARAMETER_X_AXIS_TRANSLATION)),
        OperationParameter::create(
            PropertyMap()
                .set(IdentifiedObject::NAME_KEY,
                     EPSG_PARAMETER_Y_AXIS_TRANSLATION_NAME)
                .set(Identifier::CODESPACE_KEY, "EPSG")
                .set(Identifier::CODE_KEY, EPSG_PARAMETER_Y_AXIS_TRANSLATION)),
        OperationParameter::create(
            PropertyMap()
                .set(IdentifiedObject::NAME_KEY,
                     EPSG_PARAMETER_Z_AXIS_TRANSLATION_NAME)
                .set(Identifier::CODESPACE_KEY, "EPSG")
                .set(Identifier::CODE_KEY, EPSG_PARAMETER_Z_AXIS_TRANSLATION)),
    };
    std::vector<ParameterValueNNPtr> values{
        ParameterValue::create(Length(translationXMetre)),
        ParameterValue::create(Length(translationYMetre)),
        ParameterValue::create(Length(translationZMetre)),
    };

    return create(properties, sourceCRSIn, targetCRSIn, nullptr,
                  PropertyMap()
                      .set(IdentifiedObject::NAME_KEY,
                           "Geocentric translations (geocentric domain)")
                      .set(Identifier::CODESPACE_KEY, "EPSG")
                      .set(Identifier::CODE_KEY, 1031),
                  parameters, values, accuracies);
}

// ---------------------------------------------------------------------------

TransformationNNPtr Transformation::createPositionVector(
    const PropertyMap &properties, const CRSNNPtr &sourceCRSIn,
    const CRSNNPtr &targetCRSIn, double translationXMetre,
    double translationYMetre, double translationZMetre,
    double rotationXMicroRadian, double rotationYMicroRadian,
    double rotationZMicroRadian, double scaleDifferencePPM,
    const std::vector<PositionalAccuracyNNPtr> &accuracies) {
    return createSevenParamsTransform(
        properties,
        PropertyMap()
            .set(IdentifiedObject::NAME_KEY,
                 "Position Vector transformation (geocentric domain)")
            .set(Identifier::CODESPACE_KEY, "EPSG")
            .set(Identifier::CODE_KEY, 1033),
        sourceCRSIn, targetCRSIn, translationXMetre, translationYMetre,
        translationZMetre, rotationXMicroRadian, rotationYMicroRadian,
        rotationZMicroRadian, scaleDifferencePPM, accuracies);
}

// ---------------------------------------------------------------------------

TransformationNNPtr Transformation::createCoordinateFrameRotation(
    const PropertyMap &properties, const CRSNNPtr &sourceCRSIn,
    const CRSNNPtr &targetCRSIn, double translationXMetre,
    double translationYMetre, double translationZMetre,
    double rotationXMicroRadian, double rotationYMicroRadian,
    double rotationZMicroRadian, double scaleDifferencePPM,
    const std::vector<PositionalAccuracyNNPtr> &accuracies) {
    return createSevenParamsTransform(
        properties, PropertyMap()
                        .set(IdentifiedObject::NAME_KEY,
                             "Coordinate Frame rotation (geocentric domain)")
                        .set(Identifier::CODESPACE_KEY, "EPSG")
                        .set(Identifier::CODE_KEY, 1032),
        sourceCRSIn, targetCRSIn, translationXMetre, translationYMetre,
        translationZMetre, rotationXMicroRadian, rotationYMicroRadian,
        rotationZMicroRadian, scaleDifferencePPM, accuracies);
}

// ---------------------------------------------------------------------------

TransformationNNPtr Transformation::createTOWGS84(
    const CRSNNPtr &sourceCRSIn,
    const std::vector<double> &TOWGS84Parameters) // throw InvalidOperation
{
    if (TOWGS84Parameters.size() != 7) {
        throw InvalidOperation(
            "Invalid number of elements in TOWGS84Parameters");
    }

    CRSPtr transformSourceCRS = CRS::extractGeographicCRS(sourceCRSIn);
    if (!transformSourceCRS) {
        throw InvalidOperation(
            "Cannot find GeographicCRS in sourceCRS of TOWGS84 transformation");
    }

    return createPositionVector(
        PropertyMap().set(IdentifiedObject::NAME_KEY,
                          "Transformation to WGS84"),
        NN_CHECK_ASSERT(transformSourceCRS), GeographicCRS::EPSG_4326,
        TOWGS84Parameters[0], TOWGS84Parameters[1], TOWGS84Parameters[2],
        TOWGS84Parameters[3], TOWGS84Parameters[4], TOWGS84Parameters[5],
        TOWGS84Parameters[6], std::vector<PositionalAccuracyNNPtr>());
}

// ---------------------------------------------------------------------------

std::string Transformation::exportToWKT(WKTFormatterNNPtr formatter) const {
    const bool isWKT2 = formatter->version() == WKTFormatter::Version::WKT2;
    if (!isWKT2) {
        throw FormattingException(
            "Transformation can only be exported to WKT2");
    }

    if (formatter->abridgedTransformation()) {
        formatter->startNode(WKTConstants::ABRIDGEDTRANSFORMATION,
                             !identifiers().empty());
    } else {
        formatter->startNode(WKTConstants::COORDINATEOPERATION,
                             !identifiers().empty());
    }
    formatter->addQuotedString(*(name()->description()));

    if (!formatter->abridgedTransformation()) {
        formatter->startNode(WKTConstants::SOURCECRS, false);
        sourceCRS()->exportToWKT(formatter);
        formatter->endNode();

        formatter->startNode(WKTConstants::TARGETCRS, false);
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
            formatter->startNode(WKTConstants::INTERPOLATIONCRS, false);
            interpolationCRS()->exportToWKT(formatter);
            formatter->endNode();
        }

        if (!coordinateOperationAccuracies().empty()) {
            formatter->startNode(WKTConstants::OPERATIONACCURACY, false);
            formatter->add(coordinateOperationAccuracies()[0]->value());
            formatter->endNode();
        }
    }

    ObjectUsage::_exportToWKT(formatter);
    formatter->endNode();

    return formatter->toString();
}

// ---------------------------------------------------------------------------

PointMotionOperation::~PointMotionOperation() = default;

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
struct ConcatenatedOperation::Private {
    std::vector<CoordinateOperationNNPtr> operations_{};

    explicit Private(const std::vector<CoordinateOperationNNPtr> &operationsIn)
        : operations_(operationsIn) {}
};
//! @endcond

// ---------------------------------------------------------------------------

ConcatenatedOperation::~ConcatenatedOperation() = default;

// ---------------------------------------------------------------------------

ConcatenatedOperation::ConcatenatedOperation(
    const std::vector<CoordinateOperationNNPtr> &operationsIn)
    : CoordinateOperation(), d(internal::make_unique<Private>(operationsIn)) {}

// ---------------------------------------------------------------------------

const std::vector<CoordinateOperationNNPtr> &
ConcatenatedOperation::operations() const {
    return d->operations_;
}

// ---------------------------------------------------------------------------

ConcatenatedOperationNNPtr ConcatenatedOperation::create(
    const PropertyMap &properties,
    const std::vector<CoordinateOperationNNPtr> &operationsIn,
    const std::vector<PositionalAccuracyNNPtr>
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
            *(operationsIn[i]->sourceCRS()->name()->description()) !=
                *(operationsIn[i - 1]->targetCRS()->name()->description())) {
            throw InvalidOperation(
                "Inconsistent chaining of CRS in operations");
        }
    }
    auto op = ConcatenatedOperation::nn_make_shared<ConcatenatedOperation>(
        operationsIn);
    op->setProperties(properties);
    op->setCRSs(NN_CHECK_ASSERT(operationsIn[0]->sourceCRS()),
                NN_CHECK_ASSERT(operationsIn.back()->targetCRS()), nullptr);
    op->setAccuracies(accuracies);
    return op;
}

// ---------------------------------------------------------------------------

std::string
ConcatenatedOperation::exportToWKT(WKTFormatterNNPtr formatter) const {
    const bool isWKT2 = formatter->version() == WKTFormatter::Version::WKT2;
    if (!isWKT2 || !formatter->use2018Keywords()) {
        throw FormattingException(
            "Transformation can only be exported to WKT2:2018");
    }

    formatter->startNode(WKTConstants::CONCATENATEDOPERATION,
                         !identifiers().empty());
    formatter->addQuotedString(*(name()->description()));

    formatter->startNode(WKTConstants::SOURCECRS, false);
    sourceCRS()->exportToWKT(formatter);
    formatter->endNode();

    formatter->startNode(WKTConstants::TARGETCRS, false);
    targetCRS()->exportToWKT(formatter);
    formatter->endNode();

    for (const auto &operation : operations()) {
        formatter->startNode(WKTConstants::STEP, false);
        operation->exportToWKT(formatter);
        formatter->endNode();
    }

    ObjectUsage::_exportToWKT(formatter);
    formatter->endNode();

    return formatter->toString();
}
