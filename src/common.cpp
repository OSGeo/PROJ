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

#include <cmath> // M_PI
#include <cstdlib>
#include <memory>
#include <string>
#include <vector>

using namespace NS_PROJ::common;
using namespace NS_PROJ::internal;
using namespace NS_PROJ::io;
using namespace NS_PROJ::metadata;
using namespace NS_PROJ::util;

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
struct UnitOfMeasure::Private {
    std::string name_{};
    double toSI_ = 1.0;
    UnitOfMeasure::Type type_{UnitOfMeasure::Type::UNKNOWN};
    std::string authority_{};
    std::string code_{};

    Private(const std::string &nameIn, double toSIIn,
            UnitOfMeasure::Type typeIn, const std::string &authorityIn,
            const std::string &codeIn)
        : name_(nameIn), toSI_(toSIIn), type_(typeIn), authority_(authorityIn),
          code_(codeIn) {}
};
//! @endcond

// ---------------------------------------------------------------------------

UnitOfMeasure::UnitOfMeasure(const std::string &nameIn, double toSIIn,
                             UnitOfMeasure::Type typeIn,
                             const std::string &authorityIn,
                             const std::string &codeIn)
    : d(internal::make_unique<Private>(nameIn, toSIIn, typeIn, authorityIn,
                                       codeIn)) {}

// ---------------------------------------------------------------------------

UnitOfMeasure::UnitOfMeasure(const UnitOfMeasure &other)
    : d(internal::make_unique<Private>(*(other.d))) {}

// ---------------------------------------------------------------------------

UnitOfMeasure::~UnitOfMeasure() = default;

// ---------------------------------------------------------------------------

UnitOfMeasure &UnitOfMeasure::operator=(const UnitOfMeasure &other) {
    if (this != &other) {
        *d = *(other.d);
    }
    return *this;
}

// ---------------------------------------------------------------------------

const std::string &UnitOfMeasure::name() const { return d->name_; }

// ---------------------------------------------------------------------------

double UnitOfMeasure::conversionToSI() const { return d->toSI_; }

// ---------------------------------------------------------------------------

UnitOfMeasure::Type UnitOfMeasure::type() const { return d->type_; }

// ---------------------------------------------------------------------------

const std::string &UnitOfMeasure::authority() const { return d->authority_; }

// ---------------------------------------------------------------------------

const std::string &UnitOfMeasure::code() const { return d->code_; }

// ---------------------------------------------------------------------------

std::string UnitOfMeasure::exportToWKT(
    WKTFormatterNNPtr formatter,
    const std::string &unitType) const // throw(FormattingException)
{
    const bool isWKT2 = formatter->version() == WKTFormatter::Version::WKT2;

    if (formatter->forceUNITKeyword()) {
        formatter->startNode(WKTConstants::UNIT, !authority().empty());
    } else if (!unitType.empty()) {
        formatter->startNode(unitType, !authority().empty());
    } else {
        if (isWKT2 && type() == Type::LINEAR) {
            formatter->startNode(WKTConstants::LENGTHUNIT,
                                 !authority().empty());
        } else if (isWKT2 && type() == Type::ANGULAR) {
            formatter->startNode(WKTConstants::ANGLEUNIT, !authority().empty());
        } else if (isWKT2 && type() == Type::SCALE) {
            formatter->startNode(WKTConstants::SCALEUNIT, !authority().empty());
        } else if (isWKT2 && type() == Type::TIME) {
            formatter->startNode(WKTConstants::TIMEUNIT, !authority().empty());
        } else {
            formatter->startNode(WKTConstants::UNIT, !authority().empty());
        }
    }

    {
        formatter->addQuotedString(name());
        const auto &factor = conversionToSI();
        if (factor != 0.0) {
            // Some TIMEUNIT do not have a conversion factor
            formatter->add(factor);
        }
        if (!authority().empty() && formatter->outputId()) {
            formatter->startNode(
                isWKT2 ? WKTConstants::ID : WKTConstants::AUTHORITY, false);
            formatter->addQuotedString(authority());
            formatter->add(code());
            formatter->endNode();
        }
    }
    formatter->endNode();
    return formatter->toString();
}

// ---------------------------------------------------------------------------

bool UnitOfMeasure::operator==(const UnitOfMeasure &other) const {
    return name() == other.name();
}

// ---------------------------------------------------------------------------

bool UnitOfMeasure::operator!=(const UnitOfMeasure &other) const {
    return name() != other.name();
}

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

Measure::Measure(double valueIn, const UnitOfMeasure &unitIn)
    : d(internal::make_unique<Private>(valueIn, unitIn)) {}

// ---------------------------------------------------------------------------

Measure::Measure(const Measure &other)
    : d(internal::make_unique<Private>(*(other.d))) {}

// ---------------------------------------------------------------------------

Measure::~Measure() = default;

// ---------------------------------------------------------------------------

Measure &Measure::operator=(const Measure &other) {
    if (this != &other) {
        *d = *(other.d);
    }
    return *this;
}

// ---------------------------------------------------------------------------

const UnitOfMeasure &Measure::unit() const { return d->unit_; }

// ---------------------------------------------------------------------------

double Measure::getSIValue() const {
    return d->value_ * d->unit_.conversionToSI();
}

// ---------------------------------------------------------------------------

double Measure::value() const { return d->value_; }

// ---------------------------------------------------------------------------

Measure Measure::convertToUnit(const UnitOfMeasure &otherUnit) const {
    return Measure(getSIValue() / otherUnit.conversionToSI(), otherUnit);
}

// ---------------------------------------------------------------------------

bool Measure::operator==(const Measure &other) const {
    return d->value_ == other.d->value_ && d->unit_ == other.d->unit_;
}

// ---------------------------------------------------------------------------

Scale::Scale(double valueIn, const UnitOfMeasure &unitIn)
    : Measure(valueIn, unitIn) {}

// ---------------------------------------------------------------------------

Scale::Scale(const Scale &) = default;

// ---------------------------------------------------------------------------

Scale::~Scale() = default;

// ---------------------------------------------------------------------------

Scale &Scale::operator=(const Scale &) = default;

// ---------------------------------------------------------------------------

Angle::Angle(double valueIn, const UnitOfMeasure &unitIn)
    : Measure(valueIn, unitIn) {}

// ---------------------------------------------------------------------------

Angle::Angle(const Angle &) = default;

// ---------------------------------------------------------------------------

Angle Angle::convertToUnit(const UnitOfMeasure &otherUnit) const {
    return Angle(Measure::convertToUnit(otherUnit).value(), otherUnit);
}

// ---------------------------------------------------------------------------

Angle::~Angle() = default;

// ---------------------------------------------------------------------------

Angle &Angle::operator=(const Angle &) = default;

// ---------------------------------------------------------------------------

Length::Length(double valueIn, const UnitOfMeasure &unitIn)
    : Measure(valueIn, unitIn) {}

// ---------------------------------------------------------------------------

Length::Length(const Length &) = default;

// ---------------------------------------------------------------------------

Length Length::convertToUnit(const UnitOfMeasure &otherUnit) const {
    return Length(Measure::convertToUnit(otherUnit).value(), otherUnit);
}

// ---------------------------------------------------------------------------

Length::~Length() = default;

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

DateTime::DateTime(const std::string &str)
    : d(internal::make_unique<Private>(str)) {}

// ---------------------------------------------------------------------------

DateTime::DateTime(const DateTime &other)
    : d(internal::make_unique<Private>(*(other.d))) {}

// ---------------------------------------------------------------------------

DateTime::~DateTime() = default;

// ---------------------------------------------------------------------------

DateTime DateTime::create(const std::string &str) { return DateTime(str); }

// ---------------------------------------------------------------------------

bool DateTime::isISO_8601() const {
    return !d->str_.empty() && d->str_[0] >= '0' && d->str_[0] <= '9' &&
           d->str_.find(' ') == std::string::npos;
}

// ---------------------------------------------------------------------------

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

IdentifiedObject::~IdentifiedObject() = default;

// ---------------------------------------------------------------------------

IdentifiedObjectNNPtr IdentifiedObject::create(
    const PropertyMap &properties) // throw(InvalidValueTypeException)
{
    auto id(IdentifiedObject::nn_make_shared<IdentifiedObject>());
    id->setProperties(properties);
    return id;
}

// ---------------------------------------------------------------------------

const IdentifierNNPtr &IdentifiedObject::name() const { return d->name; }

// ---------------------------------------------------------------------------

const std::vector<IdentifierNNPtr> &IdentifiedObject::identifiers() const {
    return d->identifiers;
}

// ---------------------------------------------------------------------------

const std::vector<GenericNameNNPtr> &IdentifiedObject::aliases() const {
    return d->aliases;
}

// ---------------------------------------------------------------------------

std::string IdentifiedObject::alias() const {
    if (d->aliases.empty())
        return std::string();
    return d->aliases[0]->toFullyQualifiedName()->toString();
}

// ---------------------------------------------------------------------------

bool IdentifiedObject::isEPSG(int code) const {
    for (const auto &id : identifiers()) {
        if (ci_equal(*(id->codeSpace()), "EPSG")) {
            return ::atoi(id->code().c_str()) == code;
        }
    }
    return false;
}

// ---------------------------------------------------------------------------

const std::string &IdentifiedObject::remarks() const { return d->remarks; }

// ---------------------------------------------------------------------------

bool IdentifiedObject::isDeprecated() const { return d->isDeprecated; }

// ---------------------------------------------------------------------------

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

//! @cond Doxygen_Suppress"
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

ObjectDomain::~ObjectDomain() = default;

// ---------------------------------------------------------------------------

const optional<std::string> &ObjectDomain::scope() const { return d->scope_; }

// ---------------------------------------------------------------------------

const ExtentPtr &ObjectDomain::domainOfValidity() const {
    return d->domainOfValidity_;
}

// ---------------------------------------------------------------------------

ObjectDomainNNPtr ObjectDomain::create(const optional<std::string> &scopeIn,
                                       const ExtentPtr &extent) {
    auto objectDomain = ObjectDomain::nn_make_shared<ObjectDomain>();
    objectDomain->d->scope_ = scopeIn;
    objectDomain->d->domainOfValidity_ = extent;
    return objectDomain;
}

// ---------------------------------------------------------------------------

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
    }
    return formatter->toString();
}

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

ObjectUsage::~ObjectUsage() = default;

// ---------------------------------------------------------------------------

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
        if (l_domains.size() == 1) {
            l_domains[0]->exportToWKT(formatter);
        }
        // TODO add support for multiple USAGE nodes in WKT2:2018
    }
    if (formatter->outputId()) {
        formatID(formatter);
    }
    if (isWKT2) {
        formatRemarks(formatter);
    }
}
