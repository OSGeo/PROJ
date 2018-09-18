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
#include "proj/internal.hpp"
#include "proj/io.hpp"
#include "proj/io_internal.hpp"
#include "proj/metadata.hpp"
#include "proj/util.hpp"

#include "projects.h"

#include <cmath> // M_PI
#include <cstdlib>
#include <memory>
#include <string>
#include <vector>

using namespace NS_PROJ::internal;
using namespace NS_PROJ::io;
using namespace NS_PROJ::metadata;
using namespace NS_PROJ::util;

NS_PROJ_START
namespace common {

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
struct UnitOfMeasure::Private {
    std::string name_{};
    double toSI_ = 1.0;
    UnitOfMeasure::Type type_{UnitOfMeasure::Type::UNKNOWN};
    std::string codeSpace_{};
    std::string code_{};

    Private(const std::string &nameIn, double toSIIn,
            UnitOfMeasure::Type typeIn, const std::string &codeSpaceIn,
            const std::string &codeIn)
        : name_(nameIn), toSI_(toSIIn), type_(typeIn), codeSpace_(codeSpaceIn),
          code_(codeIn) {}
};
//! @endcond

// ---------------------------------------------------------------------------

/** \brief Creates a UnitOfMeasure. */
UnitOfMeasure::UnitOfMeasure(const std::string &nameIn, double toSIIn,
                             UnitOfMeasure::Type typeIn,
                             const std::string &codeSpaceIn,
                             const std::string &codeIn)
    : d(internal::make_unique<Private>(nameIn, toSIIn, typeIn, codeSpaceIn,
                                       codeIn)) {}

// ---------------------------------------------------------------------------

UnitOfMeasure::UnitOfMeasure(const UnitOfMeasure &other)
    : d(internal::make_unique<Private>(*(other.d))) {}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
UnitOfMeasure::~UnitOfMeasure() = default;
//! @endcond

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
UnitOfMeasure &UnitOfMeasure::operator=(const UnitOfMeasure &other) {
    if (this != &other) {
        *d = *(other.d);
    }
    return *this;
}
//! @endcond

// ---------------------------------------------------------------------------

/** \brief Return the name of the unit of measure. */
const std::string &UnitOfMeasure::name() const { return d->name_; }

// ---------------------------------------------------------------------------

/** \brief Return the conversion factor to the unit of the
 * International System of Units of the same Type.
 *
 * For example, for foot, this would be 0.3048 (metre)
 *
 * @return the conversion factor, or 0 if no conversion exists.
 */
double UnitOfMeasure::conversionToSI() const { return d->toSI_; }

// ---------------------------------------------------------------------------

/** \brief Return the type of the unit of measure.
 */
UnitOfMeasure::Type UnitOfMeasure::type() const { return d->type_; }

// ---------------------------------------------------------------------------

/** \brief Return the code space of the unit of measure.
 *
 * For example "EPSG"
 *
 * @return the code space, or empty string.
 */
const std::string &UnitOfMeasure::codeSpace() const { return d->codeSpace_; }

// ---------------------------------------------------------------------------

/** \brief Return the code of the unit of measure.
 *
 * @return the code, or empty string.
 */
const std::string &UnitOfMeasure::code() const { return d->code_; }

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
std::string UnitOfMeasure::exportToWKT(
    WKTFormatterNNPtr formatter,
    const std::string &unitType) const // throw(FormattingException)
{
    const bool isWKT2 = formatter->version() == WKTFormatter::Version::WKT2;

    if (formatter->forceUNITKeyword() && type() != Type::PARAMETRIC) {
        formatter->startNode(WKTConstants::UNIT, !codeSpace().empty());
    } else if (!unitType.empty()) {
        formatter->startNode(unitType, !codeSpace().empty());
    } else {
        if (isWKT2 && type() == Type::LINEAR) {
            formatter->startNode(WKTConstants::LENGTHUNIT,
                                 !codeSpace().empty());
        } else if (isWKT2 && type() == Type::ANGULAR) {
            formatter->startNode(WKTConstants::ANGLEUNIT, !codeSpace().empty());
        } else if (isWKT2 && type() == Type::SCALE) {
            formatter->startNode(WKTConstants::SCALEUNIT, !codeSpace().empty());
        } else if (isWKT2 && type() == Type::TIME) {
            formatter->startNode(WKTConstants::TIMEUNIT, !codeSpace().empty());
        } else if (isWKT2 && type() == Type::PARAMETRIC) {
            formatter->startNode(WKTConstants::PARAMETRICUNIT,
                                 !codeSpace().empty());
        } else {
            formatter->startNode(WKTConstants::UNIT, !codeSpace().empty());
        }
    }

    {
        formatter->addQuotedString(name());
        const auto &factor = conversionToSI();
        if (!isWKT2 || factor != 0.0) {
            // Some TIMEUNIT do not have a conversion factor
            formatter->add(factor);
        }
        if (!codeSpace().empty() && formatter->outputId()) {
            formatter->startNode(
                isWKT2 ? WKTConstants::ID : WKTConstants::AUTHORITY, false);
            formatter->addQuotedString(codeSpace());
            const auto &l_code = code();
            if (isWKT2) {
                try {
                    (void)std::stoi(l_code);
                    formatter->add(l_code);
                } catch (const std::exception &) {
                    formatter->addQuotedString(l_code);
                }
            } else {
                formatter->addQuotedString(l_code);
            }
            formatter->endNode();
        }
    }
    formatter->endNode();
    return formatter->toString();
}
//! @endcond

// ---------------------------------------------------------------------------

/** Returns whether two units of measures are equal.
 *
 * The comparison is based on the name.
 */
bool UnitOfMeasure::operator==(const UnitOfMeasure &other) const {
    return name() == other.name();
}

// ---------------------------------------------------------------------------

/** Returns whether two units of measures are different.
 *
 * The comparison is based on the name.
 */
bool UnitOfMeasure::operator!=(const UnitOfMeasure &other) const {
    return name() != other.name();
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
std::string UnitOfMeasure::exportToPROJString() const {
    if (type() == Type::LINEAR) {
        for (int i = 0; pj_units[i].id != nullptr; i++) {
            if (::fabs(pj_units[i].factor - conversionToSI()) <
                1e-10 * conversionToSI()) {
                return pj_units[i].id;
            }
        }
    } else if (type() == Type::ANGULAR) {
        for (int i = 0; pj_angular_units[i].id != nullptr; i++) {
            if (::fabs(pj_angular_units[i].factor - conversionToSI()) <
                1e-10 * conversionToSI()) {
                return pj_angular_units[i].id;
            }
        }
    }
    return std::string();
}
//! @endcond

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
struct Measure::Private {
    double value_ = 0.0;
    UnitOfMeasure unit_{};

    Private(double valueIn, const UnitOfMeasure &unitIn)
        : value_(valueIn), unit_(unitIn) {}
};
//! @endcond

// ---------------------------------------------------------------------------

/** \brief Instanciate a Measure.
 */
Measure::Measure(double valueIn, const UnitOfMeasure &unitIn)
    : d(internal::make_unique<Private>(valueIn, unitIn)) {}

// ---------------------------------------------------------------------------

Measure::Measure(const Measure &other)
    : d(internal::make_unique<Private>(*(other.d))) {}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
Measure::~Measure() = default;
//! @endcond

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
Measure &Measure::operator=(const Measure &other) {
    if (this != &other) {
        *d = *(other.d);
    }
    return *this;
}
//! @endcond

// ---------------------------------------------------------------------------

/** \brief Return the unit of the Measure.
 */
const UnitOfMeasure &Measure::unit() const { return d->unit_; }

// ---------------------------------------------------------------------------

/** \brief Return the value of the Measure, after conversion to the
 * corresponding
 * unit of the International System.
 */
double Measure::getSIValue() const {
    return d->value_ * d->unit_.conversionToSI();
}

// ---------------------------------------------------------------------------

/** \brief Return the value of the measure, expressed in the unit()
 */
double Measure::value() const { return d->value_; }

// ---------------------------------------------------------------------------

/** \brief Return a new Measure equivalent to the this object, but whose value
 * has been converted ot the provided unit.
 */
Measure Measure::convertToUnit(const UnitOfMeasure &otherUnit) const {
    return Measure(getSIValue() / otherUnit.conversionToSI(), otherUnit);
}

// ---------------------------------------------------------------------------

/** \brief Return whether two measures are equal.
 *
 * The comparison is done both on the value and the unit.
 */
bool Measure::operator==(const Measure &other) const {
    return d->value_ == other.d->value_ && d->unit_ == other.d->unit_;
}

// ---------------------------------------------------------------------------

/** \brief Returns whether an object is equivalent to another one.
 * @param other other object to compare to
 * @param criterion comparaison criterion.
 * @return true if objects are equivalent.
 */
bool Measure::isEquivalentTo(const Measure &other,
                             util::IComparable::Criterion criterion) const {
    if (criterion == util::IComparable::Criterion::STRICT) {
        return operator==(other);
    }
    return getSIValue() == other.getSIValue();
}

// ---------------------------------------------------------------------------

/** \brief Instanciate a Scale.
 *
 * @param valueIn value
 * @param unitIn unit. Constraint: unit.type() == UnitOfMeasure::Type::SCALE
 */
Scale::Scale(double valueIn, const UnitOfMeasure &unitIn)
    : Measure(valueIn, unitIn) {}

// ---------------------------------------------------------------------------

Scale::Scale(const Scale &) = default;

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
Scale::~Scale() = default;
//! @endcond

// ---------------------------------------------------------------------------

Scale &Scale::operator=(const Scale &) = default;

// ---------------------------------------------------------------------------

/** \brief Instanciate a Angle.
 *
 * @param valueIn value
 * @param unitIn unit. Constraint: unit.type() == UnitOfMeasure::Type::ANGULAR
 */
Angle::Angle(double valueIn, const UnitOfMeasure &unitIn)
    : Measure(valueIn, unitIn) {}

// ---------------------------------------------------------------------------

Angle::Angle(const Angle &) = default;

// ---------------------------------------------------------------------------

/** \brief Return a new Angle equivalent to the this object, but whose value
 * has been converted ot the provided unit.
 *
 * @param otherUnit target unit. Constraint: otherUnit.type() ==
 * UnitOfMeasure::Type::ANGULAR
 */
Angle Angle::convertToUnit(const UnitOfMeasure &otherUnit) const {
    return Angle(Measure::convertToUnit(otherUnit).value(), otherUnit);
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
Angle::~Angle() = default;
//! @endcond

// ---------------------------------------------------------------------------

Angle &Angle::operator=(const Angle &) = default;

// ---------------------------------------------------------------------------

/** \brief Instanciate a Length.
 *
 * @param valueIn value
 * @param unitIn unit. Constraint: unit.type() == UnitOfMeasure::Type::LINEAR
 */
Length::Length(double valueIn, const UnitOfMeasure &unitIn)
    : Measure(valueIn, unitIn) {}

// ---------------------------------------------------------------------------

Length::Length(const Length &) = default;

// ---------------------------------------------------------------------------

/** \brief Return a new Length equivalent to the this object, but whose value
 * has been converted ot the provided unit.
 *
 * @param otherUnit target unit. Constraint: otherUnit.type() ==
 * UnitOfMeasure::Type::LINEAR
 */
Length Length::convertToUnit(const UnitOfMeasure &otherUnit) const {
    return Length(Measure::convertToUnit(otherUnit).value(), otherUnit);
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
Length::~Length() = default;
//! @endcond

// ---------------------------------------------------------------------------

Length &Length::operator=(const Length &) = default;

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
struct DateTime::Private {
    std::string str_{};

    explicit Private(const std::string &str) : str_(str) {}
};
//! @endcond

// ---------------------------------------------------------------------------

DateTime::DateTime() : d(internal::make_unique<Private>(std::string())) {}

// ---------------------------------------------------------------------------

DateTime::DateTime(const std::string &str)
    : d(internal::make_unique<Private>(str)) {}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
DateTime::DateTime(const DateTime &other)
    : d(internal::make_unique<Private>(*(other.d))) {}
//! @endcond

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
DateTime::~DateTime() = default;
//! @endcond

// ---------------------------------------------------------------------------

/** \brief Instanciate a DateTime. */
DateTime DateTime::create(const std::string &str) { return DateTime(str); }

// ---------------------------------------------------------------------------

/** \brief Return whether the DateTime is ISO:8601 compliant.
 *
 * \remark The current implementation is really simplistic, and aimed at
 * detecting date-times that are not ISO:8601 compliant.
 */
bool DateTime::isISO_8601() const {
    return !d->str_.empty() && d->str_[0] >= '0' && d->str_[0] <= '9' &&
           d->str_.find(' ') == std::string::npos;
}

// ---------------------------------------------------------------------------

/** \brief Return the DateTime as a string.
 */
std::string DateTime::toString() const { return d->str_; }

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
// cppcheck-suppress copyCtorAndEqOperator
struct IdentifiedObject::Private {
    IdentifierNNPtr name{Identifier::create()};
    std::vector<IdentifierNNPtr> identifiers{};
    std::vector<GenericNameNNPtr> aliases{};
    std::string remarks{};
    bool isDeprecated{};

    void setIdentifiers(const PropertyMap &properties);
    void setName(const PropertyMap &properties);
    void setAliases(const PropertyMap &properties);
};
//! @endcond

// ---------------------------------------------------------------------------

IdentifiedObject::IdentifiedObject() : d(internal::make_unique<Private>()) {}

// ---------------------------------------------------------------------------

IdentifiedObject::IdentifiedObject(const IdentifiedObject &other)
    : d(internal::make_unique<Private>(*(other.d))) {}

// ---------------------------------------------------------------------------

IdentifiedObject &IdentifiedObject::operator=(const IdentifiedObject &other) {
    if (this != &other) {
        *d = *(other.d);
    }
    return *this;
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
IdentifiedObject::~IdentifiedObject() = default;
//! @endcond

// ---------------------------------------------------------------------------

/** \brief Instanciate a new IdentifiedObject.
 *
 * @param properties See \ref general_properties.
 */
IdentifiedObjectNNPtr IdentifiedObject::create(
    const PropertyMap &properties) // throw(InvalidValueTypeException)
{
    auto id(IdentifiedObject::nn_make_shared<IdentifiedObject>());
    id->setProperties(properties);
    return id;
}

// ---------------------------------------------------------------------------

/** \brief Return the name of the object.
 *
 * Generally, the only interesting field of the name will be
 * name()->description().
 */
const IdentifierNNPtr &IdentifiedObject::name() const { return d->name; }

// ---------------------------------------------------------------------------

/** \brief Return the identifier(s) of the object
 *
 * Generally, those will have Identifier::code() and Identifier::codeSpace()
 * filled.
 */
const std::vector<IdentifierNNPtr> &IdentifiedObject::identifiers() const {
    return d->identifiers;
}

// ---------------------------------------------------------------------------

/** \brief Return the alias(es) of the object.
 */
const std::vector<GenericNameNNPtr> &IdentifiedObject::aliases() const {
    return d->aliases;
}

// ---------------------------------------------------------------------------

/** \brief Return the (first) alias of the object as a string.
 *
 * Shortcut for aliases()[0]->toFullyQualifiedName()->toString()
 */
std::string IdentifiedObject::alias() const {
    if (d->aliases.empty())
        return std::string();
    return d->aliases[0]->toFullyQualifiedName()->toString();
}

// ---------------------------------------------------------------------------

/** \brief Return the EPSG code.
 * @return code, or 0 if not found
 */
int IdentifiedObject::getEPSGCode() const {
    for (const auto &id : identifiers()) {
        if (ci_equal(*(id->codeSpace()), metadata::Identifier::EPSG)) {
            return ::atoi(id->code().c_str());
        }
    }
    return 0;
}

// ---------------------------------------------------------------------------

/** \brief Return whether the object has a identifiers() in the EPSG code space.
 */
bool IdentifiedObject::isEPSG(int code) const { return getEPSGCode() == code; }

// ---------------------------------------------------------------------------

/** \brief Return the remarks.
 */
const std::string &IdentifiedObject::remarks() const { return d->remarks; }

// ---------------------------------------------------------------------------

/** \brief Return whether the object is deprecated.
 *
 * \remark Extension of \ref ISO_19111_2018
 */
bool IdentifiedObject::isDeprecated() const { return d->isDeprecated; }

// ---------------------------------------------------------------------------
//! @cond Doxygen_Suppress

void IdentifiedObject::Private::setName(
    const PropertyMap &properties) // throw(InvalidValueTypeException)
{
    auto oIter = properties.find(NAME_KEY);
    if (oIter == properties.end()) {
        return;
    }
    if (auto genVal =
            util::nn_dynamic_pointer_cast<BoxedValue>(oIter->second)) {
        if (genVal->type() == BoxedValue::Type::STRING) {
            name = Identifier::create();
            name->setProperties(PropertyMap().set(Identifier::DESCRIPTION_KEY,
                                                  genVal->stringValue()));
        } else {
            throw InvalidValueTypeException("Invalid value type for " +
                                            NAME_KEY);
        }
    } else {
        if (auto identifier =
                util::nn_dynamic_pointer_cast<Identifier>(oIter->second)) {
            name = NN_CHECK_ASSERT(identifier);
        } else {
            throw InvalidValueTypeException("Invalid value type for " +
                                            NAME_KEY);
        }
    }
}

// ---------------------------------------------------------------------------

void IdentifiedObject::Private::setIdentifiers(
    const PropertyMap &properties) // throw(InvalidValueTypeException)
{
    auto oIter = properties.find(IDENTIFIERS_KEY);
    if (oIter == properties.end()) {

        oIter = properties.find(Identifier::CODE_KEY);
        if (oIter != properties.end()) {
            auto identifier = Identifier::create();
            identifier->setProperties(properties);
            identifiers.push_back(identifier);
        }
        return;
    }
    if (auto identifier =
            util::nn_dynamic_pointer_cast<Identifier>(oIter->second)) {
        identifiers.clear();
        identifiers.push_back(NN_CHECK_ASSERT(identifier));
    } else {
        if (auto array = util::nn_dynamic_pointer_cast<ArrayOfBaseObject>(
                oIter->second)) {
            identifiers.clear();
            for (const auto &val : *array) {
                identifier = util::nn_dynamic_pointer_cast<Identifier>(val);
                if (identifier) {
                    identifiers.push_back(NN_CHECK_ASSERT(identifier));
                } else {
                    throw InvalidValueTypeException("Invalid value type for " +
                                                    IDENTIFIERS_KEY);
                }
            }
        } else {
            throw InvalidValueTypeException("Invalid value type for " +
                                            IDENTIFIERS_KEY);
        }
    }
}

// ---------------------------------------------------------------------------

void IdentifiedObject::Private::setAliases(
    const PropertyMap &properties) // throw(InvalidValueTypeException)
{
    auto oIter = properties.find(ALIAS_KEY);
    if (oIter == properties.end()) {
        return;
    }
    if (auto l_name =
            util::nn_dynamic_pointer_cast<GenericName>(oIter->second)) {
        aliases.clear();
        aliases.push_back(NN_CHECK_ASSERT(l_name));
    } else {
        if (auto array = util::nn_dynamic_pointer_cast<ArrayOfBaseObject>(
                oIter->second)) {
            aliases.clear();
            for (const auto &val : *array) {
                l_name = util::nn_dynamic_pointer_cast<GenericName>(val);
                if (l_name) {
                    aliases.push_back(NN_CHECK_ASSERT(l_name));
                } else {
                    if (auto genVal =
                            util::nn_dynamic_pointer_cast<BoxedValue>(val)) {
                        if (genVal->type() == BoxedValue::Type::STRING) {
                            aliases.push_back(NameFactory::createLocalName(
                                nullptr, genVal->stringValue()));
                        } else {
                            throw InvalidValueTypeException(
                                "Invalid value type for " + ALIAS_KEY);
                        }
                    } else {
                        throw InvalidValueTypeException(
                            "Invalid value type for " + ALIAS_KEY);
                    }
                }
            }
        } else {
            std::string temp;
            if (properties.getStringValue(ALIAS_KEY, temp)) {
                aliases.clear();
                aliases.push_back(NameFactory::createLocalName(nullptr, temp));
            } else {
                throw InvalidValueTypeException("Invalid value type for " +
                                                ALIAS_KEY);
            }
        }
    }
}
//! @endcond

// ---------------------------------------------------------------------------

void IdentifiedObject::setProperties(
    const PropertyMap &properties) // throw(InvalidValueTypeException)
{
    d->setName(properties);
    d->setIdentifiers(properties);
    d->setAliases(properties);

    properties.getStringValue(REMARKS_KEY, d->remarks);

    {
        auto oIter = properties.find(DEPRECATED_KEY);
        if (oIter != properties.end()) {
            if (auto genVal =
                    util::nn_dynamic_pointer_cast<BoxedValue>(oIter->second)) {
                if (genVal->type() == BoxedValue::Type::BOOLEAN) {
                    d->isDeprecated = genVal->booleanValue();
                } else {
                    throw InvalidValueTypeException("Invalid value type for " +
                                                    DEPRECATED_KEY);
                }
            } else {
                throw InvalidValueTypeException("Invalid value type for " +
                                                DEPRECATED_KEY);
            }
        }
    }
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
void IdentifiedObject::formatID(WKTFormatterNNPtr formatter) const {
    const bool isWKT2 = formatter->version() == WKTFormatter::Version::WKT2;
    for (const auto &id : identifiers()) {
        id->exportToWKT(formatter);
        if (!isWKT2) {
            break;
        }
    }
}

// ---------------------------------------------------------------------------

void IdentifiedObject::formatRemarks(WKTFormatterNNPtr formatter) const {
    if (!remarks().empty()) {
        formatter->startNode(WKTConstants::REMARK, false);
        formatter->addQuotedString(remarks());
        formatter->endNode();
    }
}

// ---------------------------------------------------------------------------

bool IdentifiedObject::_isEquivalentTo(
    const util::BaseObjectNNPtr &other,
    util::IComparable::Criterion criterion) const {
    auto otherIdObj = util::nn_dynamic_pointer_cast<IdentifiedObject>(other);
    if (!otherIdObj)
        return false;

    if (criterion == util::IComparable::Criterion::STRICT) {
        if (!ci_equal(*(name()->description()),
                      *(otherIdObj->name()->description()))) {
            return false;
        }
        // TODO test id etc
    } else {
        if (!metadata::Identifier::isEquivalentName(
                *(name()->description()),
                *(otherIdObj->name()->description()))) {
            return false;
        }
    }
    return true;
}
//! @endcond

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
struct ObjectDomain::Private {
    optional<std::string> scope_{};
    ExtentPtr domainOfValidity_{};
};
//! @endcond

// ---------------------------------------------------------------------------

ObjectDomain::ObjectDomain() : d(internal::make_unique<Private>()) {}

// ---------------------------------------------------------------------------

ObjectDomain::ObjectDomain(const ObjectDomain &other)
    : d(internal::make_unique<Private>(*(other.d))) {}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
ObjectDomain::~ObjectDomain() = default;
//! @endcond

// ---------------------------------------------------------------------------

/** \brief Return the scope.
 *
 * @return the scope, or empty.
 */
const optional<std::string> &ObjectDomain::scope() const { return d->scope_; }

// ---------------------------------------------------------------------------

/** \brief Return the domain of validity.
 *
 * @return the domain of validity, or nullptr.
 */
const ExtentPtr &ObjectDomain::domainOfValidity() const {
    return d->domainOfValidity_;
}

// ---------------------------------------------------------------------------

/** \brief Instanciate a ObjectDomain.
 */
ObjectDomainNNPtr ObjectDomain::create(const optional<std::string> &scopeIn,
                                       const ExtentPtr &extent) {
    auto objectDomain = ObjectDomain::nn_make_shared<ObjectDomain>();
    objectDomain->d->scope_ = scopeIn;
    objectDomain->d->domainOfValidity_ = extent;
    return objectDomain;
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
std::string ObjectDomain::exportToWKT(WKTFormatterNNPtr formatter) const {
    if (d->scope_.has_value()) {
        formatter->startNode(WKTConstants::SCOPE, false);
        formatter->addQuotedString(*(d->scope_));
        formatter->endNode();
    }
    if (d->domainOfValidity_) {
        if (d->domainOfValidity_->description().has_value()) {
            formatter->startNode(WKTConstants::AREA, false);
            formatter->addQuotedString(*(d->domainOfValidity_->description()));
            formatter->endNode();
        }
        if (d->domainOfValidity_->geographicElements().size() == 1) {
            auto bbox = util::nn_dynamic_pointer_cast<GeographicBoundingBox>(
                d->domainOfValidity_->geographicElements()[0]);
            if (bbox) {
                formatter->startNode(WKTConstants::BBOX, false);
                formatter->add(bbox->southBoundLongitude());
                formatter->add(bbox->westBoundLongitude());
                formatter->add(bbox->northBoundLongitude());
                formatter->add(bbox->eastBoundLongitude());
                formatter->endNode();
            }
        }
        if (d->domainOfValidity_->verticalElements().size() == 1) {
            auto extent = d->domainOfValidity_->verticalElements()[0];
            formatter->startNode(WKTConstants::VERTICALEXTENT, false);
            formatter->add(extent->minimumValue());
            formatter->add(extent->maximumValue());
            extent->unit()->exportToWKT(formatter);
            formatter->endNode();
        }
        if (d->domainOfValidity_->temporalElements().size() == 1) {
            auto extent = d->domainOfValidity_->temporalElements()[0];
            formatter->startNode(WKTConstants::TIMEEXTENT, false);
            if (DateTime::create(extent->start()).isISO_8601()) {
                formatter->add(extent->start());
            } else {
                formatter->addQuotedString(extent->start());
            }
            if (DateTime::create(extent->stop()).isISO_8601()) {
                formatter->add(extent->stop());
            } else {
                formatter->addQuotedString(extent->stop());
            }
            formatter->endNode();
        }
    }
    return formatter->toString();
}
//! @endcond

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
struct ObjectUsage::Private {
    std::vector<ObjectDomainNNPtr> domains_{};
};
//! @endcond

// ---------------------------------------------------------------------------

ObjectUsage::ObjectUsage() : d(internal::make_unique<Private>()) {}

// ---------------------------------------------------------------------------

ObjectUsage::ObjectUsage(const ObjectUsage &other)
    : IdentifiedObject(other), d(internal::make_unique<Private>(*(other.d))) {}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
ObjectUsage::~ObjectUsage() = default;
//! @endcond

// ---------------------------------------------------------------------------

/** \brief Return the domains of the object.
 */
const std::vector<ObjectDomainNNPtr> &ObjectUsage::domains() const {
    return d->domains_;
}

// ---------------------------------------------------------------------------

void ObjectUsage::setProperties(
    const PropertyMap &properties) // throw(InvalidValueTypeException)
{
    IdentifiedObject::setProperties(properties);

    optional<std::string> scope;
    {
        std::string temp;
        if (properties.getStringValue(SCOPE_KEY, temp)) {
            scope = temp;
        }
    }

    ExtentPtr domainOfValidity;
    {
        auto oIter = properties.find(DOMAIN_OF_VALIDITY_KEY);
        if (oIter != properties.end()) {
            domainOfValidity =
                util::nn_dynamic_pointer_cast<Extent>(oIter->second);
            if (!domainOfValidity) {
                throw InvalidValueTypeException("Invalid value type for " +
                                                DOMAIN_OF_VALIDITY_KEY);
            }
        }
    }

    if (scope.has_value() || domainOfValidity) {
        d->domains_.emplace_back(ObjectDomain::create(scope, domainOfValidity));
    }

    {
        auto oIter = properties.find(OBJECT_DOMAIN_KEY);
        if (oIter != properties.end()) {
            if (auto objectDomain = util::nn_dynamic_pointer_cast<ObjectDomain>(
                    oIter->second)) {
                d->domains_.emplace_back(NN_CHECK_ASSERT(objectDomain));
            } else if (auto array =
                           util::nn_dynamic_pointer_cast<ArrayOfBaseObject>(
                               oIter->second)) {
                for (const auto &val : *array) {
                    objectDomain =
                        util::nn_dynamic_pointer_cast<ObjectDomain>(val);
                    if (objectDomain) {
                        d->domains_.emplace_back(NN_CHECK_ASSERT(objectDomain));
                    } else {
                        throw InvalidValueTypeException(
                            "Invalid value type for " + OBJECT_DOMAIN_KEY);
                    }
                }
            } else {
                throw InvalidValueTypeException("Invalid value type for " +
                                                OBJECT_DOMAIN_KEY);
            }
        }
    }
}

// ---------------------------------------------------------------------------

void ObjectUsage::_exportToWKT(WKTFormatterNNPtr formatter) const {
    const bool isWKT2 = formatter->version() == WKTFormatter::Version::WKT2;
    if (isWKT2) {
        auto l_domains = domains();
        if (!l_domains.empty()) {
            if (formatter->use2018Keywords()) {
                for (const auto &domain : l_domains) {
                    formatter->startNode(WKTConstants::USAGE, false);
                    domain->exportToWKT(formatter);
                    formatter->endNode();
                }
            } else {
                l_domains[0]->exportToWKT(formatter);
            }
        }
    }
    if (formatter->outputId()) {
        formatID(formatter);
    }
    if (isWKT2) {
        formatRemarks(formatter);
    }
}

// ---------------------------------------------------------------------------

bool ObjectUsage::_isEquivalentTo(
    const util::BaseObjectNNPtr &other,
    util::IComparable::Criterion criterion) const {
    auto otherObjUsage = util::nn_dynamic_pointer_cast<ObjectUsage>(other);
    if (!otherObjUsage)
        return false;

    // TODO: incomplete
    return IdentifiedObject::_isEquivalentTo(other, criterion);
}

} // namespace common
NS_PROJ_END
