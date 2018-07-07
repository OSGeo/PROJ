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

using namespace NS_PROJ::internal;

NS_PROJ_START
namespace util {

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
struct BaseObject::Private {
    // This is a manual implementation of std::enable_shared_from_this<> that
    // avoids publicly deriving from it.
    std::weak_ptr<BaseObject> self_{};
};
//! @endcond

// ---------------------------------------------------------------------------

BaseObject::BaseObject() : d(internal::make_unique<Private>()) {}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
BaseObject::~BaseObject() = default;
//! @endcond

// ---------------------------------------------------------------------------

/** Keep a reference to ourselves as an internal weak pointer. So that
 * extractGeographicBaseObject() can later return a shared pointer on itself.
 */
void BaseObject::assignSelf(BaseObjectNNPtr self) {
    assert(self.get() == this);
    d->self_ = self.as_nullable();
}

// ---------------------------------------------------------------------------

BaseObjectNNPtr BaseObject::shared_from_this() const {
    // This assertion checks that in all code paths where we create a
    // shared pointer, we took care of assigning it to self_, by calling
    // assignSelf();
    return NN_CHECK_ASSERT(d->self_.lock());
}

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

/** \brief Constructs a BoxedValue from a BaseObjectNNPtr.
 */
BoxedValue::BoxedValue(const BaseObjectNNPtr &valueIn)
    : d(internal::make_unique<Private>(valueIn)) {}

// ---------------------------------------------------------------------------

/** \brief Constructs a BoxedValue from a string.
 */
BoxedValue::BoxedValue(const char *stringValueIn)
    : d(internal::make_unique<Private>(
          std::string(stringValueIn ? stringValueIn : ""))) {}

// ---------------------------------------------------------------------------

/** \brief Constructs a BoxedValue from a string.
 */
BoxedValue::BoxedValue(const std::string &stringValueIn)
    : d(internal::make_unique<Private>(stringValueIn)) {}

// ---------------------------------------------------------------------------

/** \brief Constructs a BoxedValue from an integer.
 */
BoxedValue::BoxedValue(int integerValueIn)
    : d(internal::make_unique<Private>(integerValueIn)) {}

// ---------------------------------------------------------------------------

/** \brief Constructs a BoxedValue from a boolean.
 */
BoxedValue::BoxedValue(bool booleanValueIn)
    : d(internal::make_unique<Private>(booleanValueIn)) {}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
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
//! @endcond

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
struct ArrayOfBaseObject::Private {
    std::vector<BaseObjectNNPtr> values_{};
};
//! @endcond
// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
ArrayOfBaseObject::ArrayOfBaseObject() : d(internal::make_unique<Private>()) {}

// ---------------------------------------------------------------------------

ArrayOfBaseObject::~ArrayOfBaseObject() = default;
//! @endcond

// ---------------------------------------------------------------------------

/** \brief Adds an object to the array.
 *
 * @param obj the object to add.
 */
void ArrayOfBaseObject::add(BaseObjectNNPtr obj) { d->values_.push_back(obj); }

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
std::vector<BaseObjectNNPtr>::const_iterator ArrayOfBaseObject::begin() const {
    return d->values_.begin();
}

// ---------------------------------------------------------------------------

std::vector<BaseObjectNNPtr>::const_iterator ArrayOfBaseObject::end() const {
    return d->values_.end();
}

// ---------------------------------------------------------------------------

bool ArrayOfBaseObject::empty() const { return d->values_.empty(); }
//! @endcond

// ---------------------------------------------------------------------------

/** \brief Instanciate a ArrayOfBaseObject.
 *
 * @return a new ArrayOfBaseObject.
 */
ArrayOfBaseObjectNNPtr ArrayOfBaseObject::create() {
    return ArrayOfBaseObject::nn_make_shared<ArrayOfBaseObject>();
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

//! @cond Doxygen_Suppress
PropertyMap::PropertyMap(const PropertyMap &other)
    : d(internal::make_unique<Private>(*(other.d))) {}
//! @endcond

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
PropertyMap::~PropertyMap() = default;
//! @endcond

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
std::map<std::string, BaseObjectNNPtr>::iterator
PropertyMap::find(const std::string &key) const {
    return d->map_.find(key);
}

// ---------------------------------------------------------------------------

std::map<std::string, BaseObjectNNPtr>::iterator PropertyMap::end() const {
    return d->map_.end();
}
//! @endcond

// ---------------------------------------------------------------------------

/** \brief Set a BaseObjectNNPtr as the value of a key. */
PropertyMap &PropertyMap::set(const std::string &key,
                              const BaseObjectNNPtr &val) {
    auto iter = d->map_.find(key);
    if (iter != d->map_.end()) {
        iter->second = val;
    } else {
        d->map_.insert(std::pair<std::string, BaseObjectNNPtr>(key, val));
    }
    return *this;
}

// ---------------------------------------------------------------------------

/** \brief Set a BoxedValue as the value of a key. */
PropertyMap &PropertyMap::set(const std::string &key, const BoxedValue &val) {
    auto iter = d->map_.find(key);
    if (iter != d->map_.end()) {
        iter->second = util::nn_make_shared<BoxedValue>(val);
    } else {
        d->map_.insert(std::pair<std::string, BaseObjectNNPtr>(
            key, util::nn_make_shared<BoxedValue>(val)));
    }
    return *this;
}

// ---------------------------------------------------------------------------

/** \brief Set a vector of strings as the value of a key. */
PropertyMap &PropertyMap::set(const std::string &key,
                              const std::vector<std::string> &arrayIn) {
    ArrayOfBaseObjectNNPtr array = ArrayOfBaseObject::create();
    for (const auto &str : arrayIn) {
        array->add(util::nn_make_shared<BoxedValue>(str));
    }
    auto iter = d->map_.find(key);
    if (iter != d->map_.end()) {
        iter->second = array;
    } else {
        d->map_.insert(std::pair<std::string, BaseObjectNNPtr>(key, array));
    }
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

//! @cond Doxygen_Suppress
GenericName::~GenericName() = default;
//! @endcond

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

//! @cond Doxygen_Suppress
NameSpace::~NameSpace() = default;
//! @endcond

// ---------------------------------------------------------------------------

/** \brief Returns whether this is a global namespace. */
bool NameSpace::isGlobal() const { return d->isGlobal; }

// ---------------------------------------------------------------------------

NameSpaceNNPtr NameSpace::getGlobalFromThis() const {
    NameSpaceNNPtr ns(NameSpace::nn_make_shared<NameSpace>(*this));
    ns->d->isGlobal = true;
    ns->d->name = LocalName::make_shared<LocalName>("global");
    return ns;
}

// ---------------------------------------------------------------------------

/** \brief Returns the name of this namespace. */
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

//! @cond Doxygen_Suppress
LocalName::~LocalName() = default;
//! @endcond

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

/** \brief Instanciate a NameSpace.
 *
 * @param name name of the namespace.
 * @param properties Properties. Allowed keys are "separator" and
 * "separator.head".
 * @return a new NameFactory.
 */
NameSpaceNNPtr NameFactory::createNameSpace(const GenericNameNNPtr &name,
                                            const PropertyMap &properties) {
    NameSpaceNNPtr ns(NameSpace::nn_make_shared<NameSpace>(name));
    properties.getStringValue("separator", ns->d->separator);
    properties.getStringValue("separator.head", ns->d->separatorHead);

    return ns;
}

// ---------------------------------------------------------------------------

/** \brief Instanciate a LocalName.
 *
 * @param scope scope.
 * @param name string of the local name.
 * @return a new LocalName.
 */
LocalNameNNPtr NameFactory::createLocalName(const NameSpacePtr &scope,
                                            const std::string &name) {
    return LocalName::nn_make_shared<LocalName>(scope, name);
}

// ---------------------------------------------------------------------------

/** \brief Instanciate a GenericName.
 *
 * @param scope scope.
 * @param parsedNames the components of the name.
 * @return a new GenericName.
 */
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

CodeList::CodeList(const CodeList &other)
    : d(internal::make_unique<Private>(*(other.d))) {}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
CodeList::~CodeList() = default;
//! @endcond

// ---------------------------------------------------------------------------

CodeList &CodeList::operator=(const CodeList &other) {
    d->name = other.d->name;
    return *this;
}

// ---------------------------------------------------------------------------

/** Return the CodeList item as a string. */
const std::string &CodeList::toString() const { return d->name; }

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
bool CodeList::operator==(const CodeList &other) const {
    return toString() == other.toString();
}

// ---------------------------------------------------------------------------

bool CodeList::operator!=(const CodeList &other) const {
    return toString() != other.toString();
}
//! @endcond

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
Exception::Exception(const char *message) : msg_(message) {}

// ---------------------------------------------------------------------------

Exception::Exception(const std::string &message) : msg_(message) {}

// ---------------------------------------------------------------------------

Exception::Exception(const Exception &) = default;

// ---------------------------------------------------------------------------

Exception::~Exception() = default;
//! @endcond

// ---------------------------------------------------------------------------

/** Return the exception text. */
const char *Exception::what() const noexcept { return msg_.c_str(); }

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
InvalidValueTypeException::InvalidValueTypeException(const char *message)
    : Exception(message) {}

// ---------------------------------------------------------------------------

InvalidValueTypeException::InvalidValueTypeException(const std::string &message)
    : Exception(message) {}

// ---------------------------------------------------------------------------

InvalidValueTypeException::~InvalidValueTypeException() = default;

// ---------------------------------------------------------------------------

InvalidValueTypeException::InvalidValueTypeException(
    const InvalidValueTypeException &) = default;
//! @endcond

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
UnsupportedOperationException::UnsupportedOperationException(
    const char *message)
    : Exception(message) {}

// ---------------------------------------------------------------------------

UnsupportedOperationException::UnsupportedOperationException(
    const std::string &message)
    : Exception(message) {}

// ---------------------------------------------------------------------------

UnsupportedOperationException::~UnsupportedOperationException() = default;

// ---------------------------------------------------------------------------

UnsupportedOperationException::UnsupportedOperationException(
    const UnsupportedOperationException &) = default;
//! @endcond

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
IComparable::~IComparable() = default;
//! @endcond

// ---------------------------------------------------------------------------

} // namespace util
NS_PROJ_END
