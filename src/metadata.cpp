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
#include "proj/io.hpp"
#include "proj/io_internal.hpp"
#include "proj/util.hpp"

#include <memory>
#include <sstream> // std::ostringstream
#include <string>
#include <vector>

using namespace NS_PROJ::internal;
using namespace NS_PROJ::io;
using namespace NS_PROJ::util;

NS_PROJ_START
namespace metadata {

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
struct Citation::Private {
    optional<std::string> title{};
};
//! @endcond

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
Citation::Citation() : d(internal::make_unique<Private>()) {}
//! @endcond

// ---------------------------------------------------------------------------

/** \brief Constructs a citation by its title. */
Citation::Citation(const std::string &titleIn)
    : d(internal::make_unique<Private>()) {
    d->title = titleIn;
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
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
//! @endcond

// ---------------------------------------------------------------------------

/** \brief Returns the name by which the cited resource is known. */
const optional<std::string> &Citation::title() const { return d->title; }

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
struct GeographicExtent::Private {};
//! @endcond

// ---------------------------------------------------------------------------

GeographicExtent::GeographicExtent() : d(internal::make_unique<Private>()) {}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
GeographicExtent::~GeographicExtent() = default;
//! @endcond

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
struct GeographicBoundingBox::Private {
    double west_{};
    double south_{};
    double east_{};
    double north_{};

    Private(double west, double south, double east, double north)
        : west_(west), south_(south), east_(east), north_(north) {}
};
//! @endcond

// ---------------------------------------------------------------------------

GeographicBoundingBox::GeographicBoundingBox(double west, double south,
                                             double east, double north)
    : GeographicExtent(),
      d(internal::make_unique<Private>(west, south, east, north)) {}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
GeographicBoundingBox::~GeographicBoundingBox() = default;
//! @endcond

// ---------------------------------------------------------------------------

/** \brief Returns the western-most coordinate of the limit of the dataset
 * extent.
 *
 * The unit is degrees.
 */
double GeographicBoundingBox::westBoundLongitude() const { return d->west_; }

// ---------------------------------------------------------------------------

/** \brief Returns the southern-most coordinate of the limit of the dataset
 * extent.
 *
 * The unit is degrees.
 */
double GeographicBoundingBox::southBoundLongitude() const { return d->south_; }

// ---------------------------------------------------------------------------

/** \brief Returns the eastern-most coordinate of the limit of the dataset
 * extent.
 *
 * The unit is degrees.
 */
double GeographicBoundingBox::eastBoundLongitude() const { return d->east_; }

// ---------------------------------------------------------------------------

/** \brief Returns the northern-most coordinate of the limit of the dataset
 * extent.
 *
 * The unit is degrees.
 */
double GeographicBoundingBox::northBoundLongitude() const { return d->north_; }

// ---------------------------------------------------------------------------

/** \brief Instanciate a GeographicBoundingBox.
 *
 * @param west Western-most coordinate of the limit of the dataset extent (in
 * degrees).
 * @param south Southern-most coordinate of the limit of the dataset extent (in
 * degrees).
 * @param east Eastern-most coordinate of the limit of the dataset extent (in
 * degrees).
 * @param north Northern-most coordinate of the limit of the dataset extent (in
 * degrees).
 * @return a new GeographicBoundingBox.
 */
GeographicBoundingBoxNNPtr GeographicBoundingBox::create(double west,
                                                         double south,
                                                         double east,
                                                         double north) {
    return GeographicBoundingBox::nn_make_shared<GeographicBoundingBox>(
        west, south, east, north);
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
struct Extent::Private {
    optional<std::string> description_{};
    std::vector<GeographicExtentNNPtr> geographicElements_{};
};
//! @endcond

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
Extent::Extent() : d(internal::make_unique<Private>()) {}

// ---------------------------------------------------------------------------

Extent::Extent(const Extent &other) : d(internal::make_unique<Private>()) {
    *d = *other.d;
}

// ---------------------------------------------------------------------------

Extent::~Extent() = default;
//! @endcond

// ---------------------------------------------------------------------------

/** Return a textual description of the extent.
 *
 * @return the description, or empty.
 */
const optional<std::string> &Extent::description() const {
    return d->description_;
}

// ---------------------------------------------------------------------------

/** Return the geographic element(s) of the extent
 *
 * @return the geographic element(s), or empty.
 */
const std::vector<GeographicExtentNNPtr> &Extent::geographicElements() const {
    return d->geographicElements_;
}

// ---------------------------------------------------------------------------

/** \brief Instanciate a Extent.
 *
 * @param descriptionIn Textual description, or empty.
 * @param geographicElementsIn Geographic element(s), or empty.
 * @return a new Extent.
 */
ExtentNNPtr
Extent::create(const optional<std::string> &descriptionIn,
               const std::vector<GeographicExtentNNPtr> &geographicElementsIn) {
    auto extent = Extent::nn_make_shared<Extent>();
    extent->d->description_ = descriptionIn;
    extent->d->geographicElements_ = geographicElementsIn;
    return extent;
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
struct Identifier::Private {
    optional<Citation> authority_{};
    std::string code_{};
    optional<std::string> codeSpace_{};
    optional<std::string> version_{};
    optional<std::string> description_{};
    optional<std::string> uri_{};
};
//! @endcond

// ---------------------------------------------------------------------------

Identifier::Identifier(const std::string &codeIn)
    : d(internal::make_unique<Private>()) {
    d->code_ = codeIn;
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
Identifier::Identifier(const Identifier &other)
    : d(internal::make_unique<Private>(*(other.d))) {}

// ---------------------------------------------------------------------------

Identifier::~Identifier() = default;
//! @endcond

// ---------------------------------------------------------------------------

/** \brief Instanciate a Identifier.
 *
 * @param codeIn Alphanumeric value identifying an instance in the codespace
 * @param properties See \ref general_properties.
 * Generally, the Identifier::CODESPACE_KEY should be set.
 * @return a new Identifier.
 */
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

/** \brief Return a citation for the organization responsible for definition and
 * maintenance of the code.
 *
 * @return the citation for the authority, or empty.
 */
const optional<Citation> &Identifier::authority() const {
    return d->authority_;
}

// ---------------------------------------------------------------------------

/** \brief Return the alphanumeric value identifying an instance in the
 * codespace.
 *
 * e.g. "4326" (for EPSG:4326 WGS 84 GeographicCRS)
 *
 * @return the code.
 */
const std::string &Identifier::code() const { return d->code_; }

// ---------------------------------------------------------------------------

/** \brief Return the organization responsible for definition and maintenance of
 * the code.
 *
 * e.g "EPSG"
 *
 * @return the authority codespace, or empty.
 */
const optional<std::string> &Identifier::codeSpace() const {
    return d->codeSpace_;
}

// ---------------------------------------------------------------------------

/** \brief Return the version identifier for the namespace.
 *
 * When appropriate, the edition is identified by the effective date, coded
 * using ISO 8601 date format.
 *
 * @return the version or empty.
 */
const optional<std::string> &Identifier::version() const { return d->version_; }

// ---------------------------------------------------------------------------

/** \brief Return the natural language description of the meaning of the code
 * value.
  *
 * @return the description or empty.
 */
const optional<std::string> &Identifier::description() const {
    return d->description_;
}

// ---------------------------------------------------------------------------

/** \brief Return the URI of the identifier.
  *
 * @return the URI or empty.
 */
const optional<std::string> &Identifier::uri() const { return d->uri_; }

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
                    d->authority_ = Citation(genVal->stringValue());
                } else {
                    throw InvalidValueTypeException("Invalid value type for " +
                                                    AUTHORITY_KEY);
                }
            } else {
                if (auto citation = util::nn_dynamic_pointer_cast<Citation>(
                        oIter->second)) {
                    d->authority_ = Citation(*citation);
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
                    d->code_ = buffer.str();
                } else if (genVal->type() == BoxedValue::Type::STRING) {
                    d->code_ = genVal->stringValue();
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
            d->codeSpace_ = temp;
        }
    }

    {
        std::string temp;
        if (properties.getStringValue(VERSION_KEY, temp)) {
            d->version_ = temp;
        }
    }

    {
        std::string temp;
        if (properties.getStringValue(DESCRIPTION_KEY, temp)) {
            d->description_ = temp;
        }
    }

    {
        std::string temp;
        if (properties.getStringValue(URI_KEY, temp)) {
            d->uri_ = temp;
        }
    }
}

// ---------------------------------------------------------------------------

std::string Identifier::exportToWKT(WKTFormatterNNPtr formatter) const {
    const bool isWKT2 = formatter->version() == WKTFormatter::Version::WKT2;
    const std::string &l_code = code();
    if (codeSpace() && !codeSpace()->empty() && !l_code.empty()) {
        if (isWKT2) {
            formatter->startNode(WKTConstants::ID, false);
            formatter->addQuotedString(*(codeSpace()));
            try {
                (void)std::stoi(l_code);
                formatter->add(l_code);
            } catch (const std::exception &) {
                formatter->addQuotedString(l_code);
            }
            if (version().has_value()) {
                auto l_version = *(version());
                try {
                    (void)c_locale_stod(l_version);
                    formatter->add(l_version);
                } catch (const std::exception &) {
                    formatter->addQuotedString(l_version);
                }
            }
            if (authority().has_value() && authority()->title().has_value() &&
                *(authority()->title()) != *(codeSpace())) {
                formatter->startNode(WKTConstants::CITATION, false);
                formatter->addQuotedString(*(authority()->title()));
                formatter->endNode();
            }
            if (uri().has_value()) {
                formatter->startNode(WKTConstants::URI, false);
                formatter->addQuotedString(*(uri()));
                formatter->endNode();
            }
            formatter->endNode();
        } else {
            formatter->startNode(WKTConstants::AUTHORITY, false);
            formatter->addQuotedString(*(codeSpace()));
            formatter->addQuotedString(l_code);
            formatter->endNode();
        }
    }
    return formatter->toString();
}

// ---------------------------------------------------------------------------

static std::string canonicalizeName(const std::string &str) {
    std::string res(tolower(str));
    for (auto c : std::vector<std::string>{" ", "_", "-", "/", "(", ")"}) {
        res = replaceAll(res, c, "");
    }
    return res;
}

// ---------------------------------------------------------------------------

/** \brief Returns whether two names are considered equivalent.
 *
 * Two names are equivalent by removing any space, underscore, dash, slash,
 * { or } character from them, and comparing ina case insensitive way.
 */
bool Identifier::isEquivalentName(const std::string &a, const std::string &b) {
    return canonicalizeName(a) == canonicalizeName(b);
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
struct PositionalAccuracy::Private {
    std::string value_{};
};
//! @endcond

// ---------------------------------------------------------------------------

PositionalAccuracy::PositionalAccuracy(const std::string &valueIn)
    : d(internal::make_unique<Private>()) {
    d->value_ = valueIn;
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
PositionalAccuracy::~PositionalAccuracy() = default;
//! @endcond

// ---------------------------------------------------------------------------

/** \brief Return the value of the positional accuracy.
 */
const std::string &PositionalAccuracy::value() const { return d->value_; }

// ---------------------------------------------------------------------------

/** \brief Instanciate a PositionalAccuracy.
 *
 * @param valueIn positional accuracy value.
 * @return a new PositionalAccuracy.
 */
PositionalAccuracyNNPtr PositionalAccuracy::create(const std::string &valueIn) {
    return PositionalAccuracy::nn_make_shared<PositionalAccuracy>(valueIn);
}

} // namespace metadata
NS_PROJ_END
