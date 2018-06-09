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

#include "proj/util.hpp"
#include "proj/internal.hpp"
#include "proj/io.hpp"

#include <map>
#include <memory>
#include <string>

using namespace NS_PROJ::util;
using namespace NS_PROJ::internal;

// ---------------------------------------------------------------------------

BaseObject::~BaseObject() = default;

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
struct BoxedValue::Private {
    BoxedValue::Type type_{BoxedValue::Type::OTHER_OBJECT};
    BaseObjectPtr object_{};
    std::string stringValue_{};
    int integerValue_{};
    bool booleanValue_{};

    explicit Private(const BaseObjectNNPtr &valueIn)
        : type_(BoxedValue::Type::OTHER_OBJECT), object_(valueIn) {}

    explicit Private(const std::string &stringValueIn)
        : type_(BoxedValue::Type::STRING), stringValue_(stringValueIn) {}

    explicit Private(int integerValueIn)
        : type_(BoxedValue::Type::INTEGER), integerValue_(integerValueIn) {}

    explicit Private(bool booleanValueIn)
        : type_(BoxedValue::Type::BOOLEAN), booleanValue_(booleanValueIn) {}
};
//! @endcond

// ---------------------------------------------------------------------------

BoxedValue::BoxedValue() : d(internal::make_unique<Private>(std::string())) {}

// ---------------------------------------------------------------------------

BoxedValue::BoxedValue(const BaseObjectNNPtr &valueIn)
    : d(internal::make_unique<Private>(valueIn)) {}

// ---------------------------------------------------------------------------

BoxedValue::BoxedValue(const char *stringValueIn)
    : d(internal::make_unique<Private>(
          std::string(stringValueIn ? stringValueIn : ""))) {}

// ---------------------------------------------------------------------------

BoxedValue::BoxedValue(const std::string &stringValueIn)
    : d(internal::make_unique<Private>(stringValueIn)) {}

// ---------------------------------------------------------------------------

BoxedValue::BoxedValue(int integerValueIn)
    : d(internal::make_unique<Private>(integerValueIn)) {}

// ---------------------------------------------------------------------------

BoxedValue::BoxedValue(bool booleanValueIn)
    : d(internal::make_unique<Private>(booleanValueIn)) {}

// ---------------------------------------------------------------------------

BoxedValue::BoxedValue(const BoxedValue &other)
    : d(internal::make_unique<Private>(*other.d)) {}

// ---------------------------------------------------------------------------

BoxedValue::~BoxedValue() = default;

// ---------------------------------------------------------------------------

BoxedValue &BoxedValue::operator=(const BoxedValue &other) {
    if (this != &other) {
        *d = *other.d;
    }
    return *this;
}

// ---------------------------------------------------------------------------

const BoxedValue::Type &BoxedValue::type() const { return d->type_; }

// ---------------------------------------------------------------------------

BaseObjectNNPtr BoxedValue::object() const {
    return NN_CHECK_ASSERT(d->object_);
}

// ---------------------------------------------------------------------------

const std::string &BoxedValue::stringValue() const { return d->stringValue_; }

// ---------------------------------------------------------------------------

int BoxedValue::integerValue() const { return d->integerValue_; }

// ---------------------------------------------------------------------------

bool BoxedValue::booleanValue() const { return d->booleanValue_; }

// ---------------------------------------------------------------------------

ArrayOfBaseObject::ArrayOfBaseObject(const ArrayOfBaseObject &other) {
    *this = other;
}

// ---------------------------------------------------------------------------

ArrayOfBaseObject::~ArrayOfBaseObject() = default;

// ---------------------------------------------------------------------------

ArrayOfBaseObject &ArrayOfBaseObject::
operator=(const ArrayOfBaseObject &other) {
    if (this != &other) {
        values = other.values;
    }
    return *this;
}

// ---------------------------------------------------------------------------

ArrayOfBaseObjectNNPtr ArrayOfBaseObject::create() {
    return nn_make_shared<ArrayOfBaseObject>();
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
struct PropertyMap::Private {
    std::map<std::string, BaseObjectNNPtr> map_{};
};
//! @endcond

// ---------------------------------------------------------------------------

PropertyMap::PropertyMap() : d(internal::make_unique<Private>()) {}

// ---------------------------------------------------------------------------

PropertyMap::PropertyMap(const PropertyMap &other)
    : d(internal::make_unique<Private>(*(other.d))) {}

// ---------------------------------------------------------------------------

PropertyMap::~PropertyMap() = default;

// ---------------------------------------------------------------------------

std::map<std::string, BaseObjectNNPtr>::iterator
PropertyMap::find(const std::string &key) const {
    return d->map_.find(key);
}

// ---------------------------------------------------------------------------

std::map<std::string, BaseObjectNNPtr>::iterator PropertyMap::end() const {
    return d->map_.end();
}

// ---------------------------------------------------------------------------

PropertyMap &PropertyMap::set(const std::string &key,
                              const BaseObjectNNPtr &val) {
    d->map_.insert(std::pair<std::string, BaseObjectNNPtr>(key, val));
    return *this;
}

// ---------------------------------------------------------------------------

PropertyMap &PropertyMap::set(const std::string &key, const BoxedValue &val) {
    d->map_.insert(std::pair<std::string, BaseObjectNNPtr>(
        key, util::nn_make_shared<BoxedValue>(val)));
    return *this;
}

// ---------------------------------------------------------------------------

PropertyMap &PropertyMap::set(const std::string &key,
                              const std::vector<std::string> &arrayIn) {
    ArrayOfBaseObjectNNPtr array = util::nn_make_shared<ArrayOfBaseObject>();
    for (const auto &str : arrayIn) {
        array->values.push_back(util::nn_make_shared<BoxedValue>(str));
    }
    d->map_.insert(std::pair<std::string, BaseObjectNNPtr>(key, array));
    return *this;
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
bool PropertyMap::getStringValue(
    const std::string &key,
    std::string &outVal) const // throw(InvalidValueTypeException)
{
    auto oIter = d->map_.find(key);
    if (oIter != d->map_.end()) {
        auto genVal = util::nn_dynamic_pointer_cast<BoxedValue>(oIter->second);
        if (genVal) {
            if (genVal->type() == BoxedValue::Type::STRING) {
                outVal = genVal->stringValue();
                return true;
            } else {
                throw InvalidValueTypeException("Invalid value type for " +
                                                key);
            }
        } else {
            throw InvalidValueTypeException("Invalid value type for " + key);
        }
    }
    return false;
}
//! @endcond

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
struct GenericName::Private {};
//! @endcond

// ---------------------------------------------------------------------------

GenericName::GenericName() : d(internal::make_unique<Private>()) {}

// ---------------------------------------------------------------------------

GenericName::GenericName(const GenericName &other)
    : d(internal::make_unique<Private>()) {
    *d = *other.d;
}

// ---------------------------------------------------------------------------

GenericName::~GenericName() = default;

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
struct NameSpace::Private {
    GenericNamePtr name{};
    bool isGlobal{};
    std::string separator = std::string(":");
    std::string separatorHead = std::string(":");
};
//! @endcond

// ---------------------------------------------------------------------------

NameSpace::NameSpace(const GenericNamePtr &nameIn)
    : d(internal::make_unique<Private>()) {
    d->name = nameIn;
}

// ---------------------------------------------------------------------------

NameSpace::NameSpace(const NameSpace &other)
    : d(internal::make_unique<Private>()) {
    *d = *other.d;
}

// ---------------------------------------------------------------------------

NameSpace::~NameSpace() = default;

// ---------------------------------------------------------------------------

bool NameSpace::isGlobal() const { return d->isGlobal; }

// ---------------------------------------------------------------------------

NameSpaceNNPtr NameSpace::getGlobalFromThis() const {
    NameSpaceNNPtr ns(util::nn_make_shared<NameSpace>(*this));
    ns->d->isGlobal = true;
    ns->d->name = LocalName::make_shared<LocalName>("global");
    return ns;
}

// ---------------------------------------------------------------------------

const GenericNamePtr &NameSpace::name() const { return d->name; }

// ---------------------------------------------------------------------------

const std::string &NameSpace::separator() const { return d->separator; }

// ---------------------------------------------------------------------------

NameSpaceNNPtr NameSpace::createGLOBAL() {
    NameSpaceNNPtr ns(NameSpace::nn_make_shared<NameSpace>(
        LocalName::make_shared<LocalName>("global")));
    ns->d->isGlobal = true;
    return ns;
}

// ---------------------------------------------------------------------------

const NameSpaceNNPtr NameSpace::GLOBAL(NameSpace::createGLOBAL());

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
struct LocalName::Private {
    NameSpacePtr scope{};
    std::string name{};
};
//! @endcond

// ---------------------------------------------------------------------------

LocalName::LocalName(const std::string &name)
    : d(internal::make_unique<Private>()) {
    d->name = name;
}

// ---------------------------------------------------------------------------

LocalName::LocalName(const NameSpacePtr &ns, const std::string &name)
    : d(internal::make_unique<Private>()) {
    d->scope = ns ? ns : static_cast<NameSpacePtr>(NameSpace::GLOBAL);
    d->name = name;
}

// ---------------------------------------------------------------------------

LocalName::LocalName(const LocalName &other)
    : GenericName(other), d(internal::make_unique<Private>()) {
    *d = *other.d;
}

// ---------------------------------------------------------------------------

LocalName::~LocalName() = default;

// ---------------------------------------------------------------------------

const NameSpacePtr LocalName::scope() const {
    if (d->scope)
        return d->scope;
    return NameSpace::GLOBAL;
}

// ---------------------------------------------------------------------------

GenericNameNNPtr LocalName::toFullyQualifiedName() const {
    if (scope()->isGlobal())
        return LocalName::nn_make_shared<LocalName>(*this);

    return LocalName::nn_make_shared<LocalName>(
        d->scope->getGlobalFromThis(),
        d->scope->name()->toFullyQualifiedName()->toString() +
            d->scope->separator() + d->name);
}

// ---------------------------------------------------------------------------

std::string LocalName::toString() const { return d->name; }

// ---------------------------------------------------------------------------

NameSpaceNNPtr NameFactory::createNameSpace(const GenericNameNNPtr &name,
                                            const PropertyMap &properties) {
    NameSpaceNNPtr ns(NameSpace::nn_make_shared<NameSpace>(name));
    properties.getStringValue("separator", ns->d->separator);
    properties.getStringValue("separator.head", ns->d->separatorHead);

    return ns;
}

// ---------------------------------------------------------------------------

LocalNameNNPtr NameFactory::createLocalName(const NameSpacePtr &scope,
                                            const std::string &name) {
    return LocalName::nn_make_shared<LocalName>(scope, name);
}

// ---------------------------------------------------------------------------

GenericNameNNPtr
NameFactory::createGenericName(const NameSpacePtr &scope,
                               const std::vector<std::string> &parsedNames) {
    std::string name;
    const std::string separator(scope ? scope->separator()
                                      : NameSpace::GLOBAL->separator());
    bool first = true;
    for (const auto &str : parsedNames) {
        if (!first)
            name += separator;
        first = false;
        name += str;
    }
    return LocalName::nn_make_shared<LocalName>(scope, name);
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
struct CodeList::Private {
    std::string name{};
};
//! @endcond

// ---------------------------------------------------------------------------

CodeList::CodeList(const std::string &nameIn)
    : d(internal::make_unique<Private>()) {
    d->name = nameIn;
}

// ---------------------------------------------------------------------------

CodeList::~CodeList() = default;

// ---------------------------------------------------------------------------

const std::string &CodeList::toString() const { return d->name; }

// ---------------------------------------------------------------------------

bool CodeList::operator==(const CodeList &other) const {
    return toString() == other.toString();
}

// ---------------------------------------------------------------------------

bool CodeList::operator!=(const CodeList &other) const {
    return toString() != other.toString();
}
// ---------------------------------------------------------------------------

Exception::Exception(const char *message) : msg_(message) {}

// ---------------------------------------------------------------------------

Exception::Exception(const std::string &message) : msg_(message) {}

// ---------------------------------------------------------------------------

Exception::~Exception() = default;

// ---------------------------------------------------------------------------

const char *Exception::what() const noexcept { return msg_.c_str(); }

// ---------------------------------------------------------------------------

InvalidValueTypeException::InvalidValueTypeException(const char *message)
    : Exception(message) {}

// ---------------------------------------------------------------------------

InvalidValueTypeException::InvalidValueTypeException(const std::string &message)
    : Exception(message) {}

// ---------------------------------------------------------------------------

InvalidValueTypeException::~InvalidValueTypeException() = default;
