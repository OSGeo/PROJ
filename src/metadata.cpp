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

#include "proj/metadata.hpp"
#include "proj/internal.hpp"
#include "proj/util.hpp"

#include <memory>
#include <sstream> // std::ostringstream
#include <string>
#include <vector>

using namespace NS_PROJ::metadata;
using namespace NS_PROJ::internal;
using namespace NS_PROJ::util;

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
struct Citation::Private {
    optional<std::string> title{};
};
//! @endcond

// ---------------------------------------------------------------------------

Citation::Citation() : d(internal::make_unique<Private>()) {}

// ---------------------------------------------------------------------------

Citation::Citation(const std::string &titleIn)
    : d(internal::make_unique<Private>()) {
    d->title = titleIn;
}

// ---------------------------------------------------------------------------

Citation::Citation(const Citation &other)
    : d(internal::make_unique<Private>(*(other.d))) {}

// ---------------------------------------------------------------------------

Citation::~Citation() = default;

// ---------------------------------------------------------------------------

Citation &Citation::operator=(const Citation &other) {
    if (this != &other) {
        *d = *other.d;
    }
    return *this;
}

// ---------------------------------------------------------------------------

const optional<std::string> &Citation::title() const { return d->title; }

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
struct Extent::Private {
    optional<std::string> description{};
};
//! @endcond

// ---------------------------------------------------------------------------

Extent::Extent() : d(internal::make_unique<Private>()) {}

// ---------------------------------------------------------------------------

Extent::Extent(const Extent &other) : d(internal::make_unique<Private>()) {
    *d = *other.d;
}

// ---------------------------------------------------------------------------

Extent::~Extent() = default;

// ---------------------------------------------------------------------------

const optional<std::string> &Extent::description() const {
    return d->description;
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
struct Identifier::Private {
    optional<Citation> authority{};
    std::string code{};
    optional<std::string> codeSpace{};
    optional<std::string> version{};
    optional<std::string> description{};
};
//! @endcond

// ---------------------------------------------------------------------------

Identifier::Identifier(const std::string &codeIn)
    : d(internal::make_unique<Private>()) {
    d->code = codeIn;
}

// ---------------------------------------------------------------------------

Identifier::Identifier(const Identifier &other)
    : d(internal::make_unique<Private>(*(other.d))) {}

// ---------------------------------------------------------------------------

Identifier::~Identifier() = default;

// ---------------------------------------------------------------------------

IdentifierNNPtr Identifier::create(const std::string &codeIn,
                                   const PropertyMap &properties) {
    auto id(Identifier::nn_make_shared<Identifier>(codeIn));
    id->setProperties(properties);
    return id;
}

// ---------------------------------------------------------------------------

Identifier &Identifier::operator=(const Identifier &other) {
    if (this != &other) {
        *d = *other.d;
    }
    return *this;
}

// ---------------------------------------------------------------------------

const optional<Citation> &Identifier::authority() const { return d->authority; }

// ---------------------------------------------------------------------------

const std::string &Identifier::code() const { return d->code; }

// ---------------------------------------------------------------------------

const optional<std::string> &Identifier::codeSpace() const {
    return d->codeSpace;
}

// ---------------------------------------------------------------------------

const optional<std::string> &Identifier::version() const { return d->version; }

// ---------------------------------------------------------------------------

const optional<std::string> &Identifier::description() const {
    return d->description;
}

// ---------------------------------------------------------------------------

void Identifier::setProperties(
    const PropertyMap &properties) // throw(InvalidValueTypeException)
{
    {
        auto oIter = properties.find(AUTHORITY_KEY);
        if (oIter != properties.end()) {
            if (auto genVal =
                    util::nn_dynamic_pointer_cast<BoxedValue>(oIter->second)) {
                if (genVal->type() == BoxedValue::Type::STRING) {
                    d->authority = Citation(genVal->stringValue());
                } else {
                    throw InvalidValueTypeException("Invalid value type for " +
                                                    AUTHORITY_KEY);
                }
            } else {
                if (auto citation = util::nn_dynamic_pointer_cast<Citation>(
                        oIter->second)) {
                    d->authority = Citation(*citation);
                } else {
                    throw InvalidValueTypeException("Invalid value type for " +
                                                    AUTHORITY_KEY);
                }
            }
        }
    }

    {
        auto oIter = properties.find(CODE_KEY);
        if (oIter != properties.end()) {
            if (auto genVal =
                    util::nn_dynamic_pointer_cast<BoxedValue>(oIter->second)) {
                if (genVal->type() == BoxedValue::Type::INTEGER) {
                    std::ostringstream buffer;
                    buffer << genVal->integerValue();
                    d->code = buffer.str();
                } else if (genVal->type() == BoxedValue::Type::STRING) {
                    d->code = genVal->stringValue();
                } else {
                    throw InvalidValueTypeException("Invalid value type for " +
                                                    CODE_KEY);
                }
            } else {
                throw InvalidValueTypeException("Invalid value type for " +
                                                CODE_KEY);
            }
        }
    }

    {
        std::string temp;
        if (properties.getStringValue(CODESPACE_KEY, temp)) {
            d->codeSpace = temp;
        }
    }

    {
        std::string temp;
        if (properties.getStringValue(VERSION_KEY, temp)) {
            d->version = temp;
        }
    }

    {
        std::string temp;
        if (properties.getStringValue(DESCRIPTION_KEY, temp)) {
            d->description = temp;
        }
    }
}

// ---------------------------------------------------------------------------
