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

#include <cassert>
#include <cctype>
#include <cmath>
#include <iomanip>
#include <sstream> // std::ostringstream
#include <string>
#include <vector>

#include "proj/common.hpp"
#include "proj/coordinateoperation.hpp"
#include "proj/coordinateoperation_internal.hpp"
#include "proj/coordinatesystem.hpp"
#include "proj/coordinatesystem_internal.hpp"
#include "proj/crs.hpp"
#include "proj/datum.hpp"
#include "proj/internal.hpp"
#include "proj/io.hpp"
#include "proj/io_internal.hpp"
#include "proj/metadata.hpp"
#include "proj/util.hpp"

using namespace NS_PROJ::common;
using namespace NS_PROJ::crs;
using namespace NS_PROJ::cs;
using namespace NS_PROJ::datum;
using namespace NS_PROJ::internal;
using namespace NS_PROJ::io;
using namespace NS_PROJ::metadata;
using namespace NS_PROJ::operation;
using namespace NS_PROJ::util;

// ---------------------------------------------------------------------------

IWKTExportable::~IWKTExportable() = default;

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
struct WKTFormatter::Private {
    struct Params {
        WKTFormatter::Convention convention_ = WKTFormatter::Convention::WKT2;
        WKTFormatter::Version version_ = WKTFormatter::Version::WKT2;
        bool multiLine_ = true;
        bool strict_ = true;
        int indentWidth_ = 4;
        bool idOnTopLevelOnly_ = false;
        bool outputAxisOrder_ = false;
        bool primeMeridianOmittedIfGreenwich_ = false;
        bool ellipsoidUnitOmittedIfMetre_ = false;
        bool primeMeridianOrParameterUnitOmittedIfSameAsAxis_ = false;
        bool forceUNITKeyword_ = false;
        bool outputCSUnitOnlyOnceIfSame_ = false;
        bool primeMeridianInDegree_ = false;
        bool outputConversionNode_ = true;
        bool use2018Keywords_ = false;
    };
    Params params_{};

    int indentLevel_ = 0;
    std::vector<bool> stackHasChild_{};
    std::vector<bool> stackEmptyKeyword_{};
    std::vector<bool> outputUnitStack_{true};
    std::vector<bool> outputIdStack_{true};
    std::vector<UnitOfMeasureNNPtr> axisLinearUnitStack_{
        util::nn_make_shared<UnitOfMeasure>(UnitOfMeasure::NONE)};
    std::vector<UnitOfMeasureNNPtr> axisAngularUnitStack_{
        util::nn_make_shared<UnitOfMeasure>(UnitOfMeasure::NONE)};
    std::string result_{};

    void addNewLine();
    void addIndentation();
    void startNewChild();
};
//! @endcond

// ---------------------------------------------------------------------------

/** \brief Constructs a new formatter.
 *
 * A formatter can be used only once (its internal state is mutated)
 *
 * Its default behaviour can be adjusted with the different setters.
 *
 * @param convention WKT flavor. Defaults to Convention::WKT2
 * @return new formatter.
 */
WKTFormatterNNPtr WKTFormatter::create(Convention convention) {
    return WKTFormatter::nn_make_shared<WKTFormatter>(convention);
}

// ---------------------------------------------------------------------------

/** \brief Constructs a new formatter from another one.
 *
 * A formatter can be used only once (its internal state is mutated)
 *
 * Its default behaviour can be adjusted with the different setters.
 *
 * @param other source formatter.
 * @return new formatter.
 */
WKTFormatterNNPtr WKTFormatter::create(WKTFormatterNNPtr other) {
    auto f = WKTFormatter::nn_make_shared<WKTFormatter>(
        other->d->params_.convention_);
    f->d->params_ = other->d->params_;
    return f;
}

// ---------------------------------------------------------------------------

/** \brief Destructor. */
WKTFormatter::~WKTFormatter() = default;

// ---------------------------------------------------------------------------

/** \brief Whether to use multi line output or not. */
WKTFormatter &WKTFormatter::setMultiLine(bool multiLine) {
    d->params_.multiLine_ = multiLine;
    return *this;
}

// ---------------------------------------------------------------------------

/** \brief Set number of spaces for each indentation level (defaults to 4).
 */
WKTFormatter &WKTFormatter::setIndendationWidth(int width) {
    d->params_.indentWidth_ = width;
    return *this;
}

// ---------------------------------------------------------------------------

/** \brief Set whether the formatter should operate on strict more or not.
 *
 * The default is strit mode, in which case a FormattingException can be thrown.
 */
WKTFormatter &WKTFormatter::setStrict(bool strictIn) {
    d->params_.strict_ = strictIn;
    return *this;
}

// ---------------------------------------------------------------------------

/** \brief Returns whether the formatter is in strict mode. */
bool WKTFormatter::isStrict() const { return d->params_.strict_; }

// ---------------------------------------------------------------------------

/** Returns the WKT string from the formatter. */
const std::string &WKTFormatter::toString() const {
    if (d->indentLevel_ > 0) {
        // For intermediary nodes, the formatter is in a inconsistent
        // state, so return a dummy result.
        static const std::string dummyReturn;
        return dummyReturn;
    }
    if (d->axisLinearUnitStack_.size() != 1)
        throw FormattingException(
            "Unbalanced pushAxisLinearUnit() / popAxisLinearUnit()");
    if (d->axisAngularUnitStack_.size() != 1)
        throw FormattingException(
            "Unbalanced pushAxisAngularUnit() / popAxisAngularUnit()");
    if (d->outputIdStack_.size() != 1)
        throw FormattingException("Unbalanced pushOutputId() / popOutputId()");
    if (d->outputUnitStack_.size() != 1)
        throw FormattingException(
            "Unbalanced pushOutputUnit() / popOutputUnit()");
    if (d->indentLevel_)
        throw FormattingException("Unbalanced startNode() / endNode()");
    return d->result_;
}

//! @cond Doxygen_Suppress

// ---------------------------------------------------------------------------

WKTFormatter::WKTFormatter(Convention convention)
    : d(internal::make_unique<Private>()) {
    d->params_.convention_ = convention;
    switch (convention) {
    case Convention::WKT2_2018:
        d->params_.use2018Keywords_ = true;
        PROJ_FALLTHROUGH
    case Convention::WKT2:
        d->params_.version_ = WKTFormatter::Version::WKT2;
        d->params_.outputAxisOrder_ = true;
        break;

    case Convention::WKT2_2018_SIMPLIFIED:
        d->params_.use2018Keywords_ = true;
        PROJ_FALLTHROUGH
    case Convention::WKT2_SIMPLIFIED:
        d->params_.version_ = WKTFormatter::Version::WKT2;
        d->params_.idOnTopLevelOnly_ = true;
        d->params_.outputAxisOrder_ = false;
        d->params_.primeMeridianOmittedIfGreenwich_ = true;
        d->params_.ellipsoidUnitOmittedIfMetre_ = true;
        d->params_.primeMeridianOrParameterUnitOmittedIfSameAsAxis_ = true;
        d->params_.forceUNITKeyword_ = true;
        d->params_.outputCSUnitOnlyOnceIfSame_ = true;
        break;

    case Convention::WKT1_GDAL:
        d->params_.version_ = WKTFormatter::Version::WKT1;
        d->params_.outputAxisOrder_ = false;
        d->params_.forceUNITKeyword_ = true;
        d->params_.primeMeridianInDegree_ = true;
        break;

    default:
        assert(false);
        break;
    }
}

// ---------------------------------------------------------------------------

void WKTFormatter::Private::addNewLine() { result_ += '\n'; }

// ---------------------------------------------------------------------------

void WKTFormatter::Private::addIndentation() {
    result_ += std::string(indentLevel_ * params_.indentWidth_, ' ');
}

// ---------------------------------------------------------------------------

void WKTFormatter::startNode(const std::string &keyword) {
    if (!d->stackHasChild_.empty()) {
        d->startNewChild();
    }

    if (d->params_.multiLine_) {
        if (d->indentLevel_ && !keyword.empty()) {
            d->addNewLine();
            d->addIndentation();
        }
    }

    if (!keyword.empty()) {
        d->result_ += keyword;
        d->result_ += "[";
    }
    d->indentLevel_++;
    d->stackHasChild_.push_back(false);
    d->stackEmptyKeyword_.push_back(keyword.empty());

    // We should emit ID nodes for :
    // -root element
    // - and for METHOD&PARAMETER nodes in WKT2, unless idOnTopLevelOnly_ is
    // set.
    // For WKT2, all other intermediate nodes shouldn't have ID ("not
    // recommended")
    if (!d->params_.idOnTopLevelOnly_ && d->indentLevel_ >= 2 &&
        d->params_.version_ == WKTFormatter::Version::WKT2 &&
        (keyword == WKTConstants::METHOD ||
         keyword == WKTConstants::PARAMETER)) {
        pushOutputId(true);
    } else if (d->indentLevel_ >= 2 &&
               d->params_.version_ == WKTFormatter::Version::WKT2) {
        pushOutputId(false);
    } else {
        pushOutputId(outputId());
    }
}

// ---------------------------------------------------------------------------

void WKTFormatter::endNode() {
    assert(d->indentLevel_ > 0);
    popOutputId();
    d->indentLevel_--;
    bool emptyKeyword = d->stackEmptyKeyword_.back();
    d->stackEmptyKeyword_.pop_back();
    d->stackHasChild_.pop_back();
    if (!emptyKeyword)
        d->result_ += "]";
}

// ---------------------------------------------------------------------------

void WKTFormatter::Private::startNewChild() {
    if (!stackHasChild_.empty() && stackHasChild_.back()) {
        result_ += ",";
    }
    stackHasChild_.back() = true;
}

// ---------------------------------------------------------------------------

void WKTFormatter::addQuotedString(const std::string &str) {
    d->startNewChild();
    d->result_ += "\"" + str + "\"";
}

// ---------------------------------------------------------------------------

void WKTFormatter::add(const std::string &str) {
    d->startNewChild();
    d->result_ += str;
}

// ---------------------------------------------------------------------------

void WKTFormatter::add(int number) {
    d->startNewChild();
    std::ostringstream buffer;
    buffer << number;
    d->result_ += buffer.str();
}

// ---------------------------------------------------------------------------

void WKTFormatter::add(size_t number) {
    d->startNewChild();
    std::ostringstream buffer;
    buffer << number;
    d->result_ += buffer.str();
}

// ---------------------------------------------------------------------------

void WKTFormatter::add(double number, int precision) {
    d->startNewChild();
    std::ostringstream buffer;
    buffer << std::setprecision(precision) << number;
    std::string numberStr = buffer.str();
    d->result_ += numberStr;
    /*if( numberStr.find('.') == std::string::npos )
    {
        d->result_ += ".0";
    }*/
}

// ---------------------------------------------------------------------------

void WKTFormatter::pushOutputUnit(bool outputUnitIn) {
    d->outputUnitStack_.push_back(outputUnitIn);
}

// ---------------------------------------------------------------------------

void WKTFormatter::popOutputUnit() { d->outputUnitStack_.pop_back(); }

// ---------------------------------------------------------------------------

bool WKTFormatter::outputUnit() const { return d->outputUnitStack_.back(); }

// ---------------------------------------------------------------------------

void WKTFormatter::pushOutputId(bool outputIdIn) {
    d->outputIdStack_.push_back(outputIdIn);
}

// ---------------------------------------------------------------------------

void WKTFormatter::popOutputId() { d->outputIdStack_.pop_back(); }

// ---------------------------------------------------------------------------

bool WKTFormatter::outputId() const { return d->outputIdStack_.back(); }

// ---------------------------------------------------------------------------

void WKTFormatter::pushAxisLinearUnit(const UnitOfMeasureNNPtr &unit) {
    d->axisLinearUnitStack_.push_back(unit);
}
// ---------------------------------------------------------------------------

void WKTFormatter::popAxisLinearUnit() { d->axisLinearUnitStack_.pop_back(); }

// ---------------------------------------------------------------------------

const UnitOfMeasureNNPtr &WKTFormatter::axisLinearUnit() const {
    return d->axisLinearUnitStack_.back();
}

// ---------------------------------------------------------------------------

void WKTFormatter::pushAxisAngularUnit(const UnitOfMeasureNNPtr &unit) {
    d->axisAngularUnitStack_.push_back(unit);
}
// ---------------------------------------------------------------------------

void WKTFormatter::popAxisAngularUnit() { d->axisAngularUnitStack_.pop_back(); }

// ---------------------------------------------------------------------------

const UnitOfMeasureNNPtr &WKTFormatter::axisAngularUnit() const {
    return d->axisAngularUnitStack_.back();
}

// ---------------------------------------------------------------------------

bool WKTFormatter::outputAxisOrder() const {
    return d->params_.outputAxisOrder_;
}

// ---------------------------------------------------------------------------

bool WKTFormatter::primeMeridianOmittedIfGreenwich() const {
    return d->params_.primeMeridianOmittedIfGreenwich_;
}

// ---------------------------------------------------------------------------

bool WKTFormatter::ellipsoidUnitOmittedIfMetre() const {
    return d->params_.ellipsoidUnitOmittedIfMetre_;
}

// ---------------------------------------------------------------------------

bool WKTFormatter::primeMeridianOrParameterUnitOmittedIfSameAsAxis() const {
    return d->params_.primeMeridianOrParameterUnitOmittedIfSameAsAxis_;
}

// ---------------------------------------------------------------------------

bool WKTFormatter::outputCSUnitOnlyOnceIfSame() const {
    return d->params_.outputCSUnitOnlyOnceIfSame_;
}

// ---------------------------------------------------------------------------

bool WKTFormatter::forceUNITKeyword() const {
    return d->params_.forceUNITKeyword_;
}

// ---------------------------------------------------------------------------

bool WKTFormatter::primeMeridianInDegree() const {
    return d->params_.primeMeridianInDegree_;
}

// ---------------------------------------------------------------------------

WKTFormatter::Version WKTFormatter::version() const {
    return d->params_.version_;
}

// ---------------------------------------------------------------------------

bool WKTFormatter::use2018Keywords() const {
    return d->params_.use2018Keywords_;
}

// ---------------------------------------------------------------------------

void WKTFormatter::setOutputConversionNode(bool outputIn) {
    d->params_.outputConversionNode_ = outputIn;
}

// ---------------------------------------------------------------------------

bool WKTFormatter::outputConversionNode() const {
    return d->params_.outputConversionNode_;
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
struct WKTNode::Private {
    std::string value_{};
    std::vector<WKTNodeNNPtr> children_{};
};
//! @endcond

// ---------------------------------------------------------------------------

WKTNode::WKTNode(const std::string &valueIn)
    : d(internal::make_unique<Private>()) {
    d->value_ = valueIn;
}

// ---------------------------------------------------------------------------

WKTNode::~WKTNode() = default;

// ---------------------------------------------------------------------------

// do not add a parent of this node
void WKTNode::addChild(WKTNodeNNPtr child) { d->children_.push_back(child); }

// ---------------------------------------------------------------------------

WKTNodePtr WKTNode::lookForChild(const std::string &childName,
                                 int occurence) const {
    int occCount = 0;
    for (const auto &child : d->children_) {
        if (ci_equal(child->value(), childName)) {
            if (occurence == occCount) {
                return child;
            }
            occCount++;
        }
    }
    return nullptr;
}

// ---------------------------------------------------------------------------

int WKTNode::countChildrenOfName(const std::string &childName) const {
    int occCount = 0;
    for (const auto &child : d->children_) {
        if (ci_equal(child->value(), childName)) {
            occCount++;
        }
    }
    return occCount;
}

// ---------------------------------------------------------------------------

const std::string &WKTNode::value() const { return d->value_; }

// ---------------------------------------------------------------------------

const std::vector<WKTNodeNNPtr> &WKTNode::children() const {
    return d->children_;
}

// ---------------------------------------------------------------------------

static size_t skipSpace(const std::string &str, size_t start) {
    size_t i = start;
    while (i < str.size() && ::isspace(str[i])) {
        ++i;
    }
    return i;
}

// ---------------------------------------------------------------------------

WKTNodeNNPtr WKTNode::createFrom(const std::string &wkt, size_t indexStart,
                                 int recLevel, size_t &indexEnd) {
    if (recLevel == 16) {
        throw ParsingException("too many nesting levels");
    }
    std::string value;
    size_t i = skipSpace(wkt, indexStart);
    if (i == wkt.size()) {
        throw ParsingException("whitespace only string");
    }
    bool inString = false;
    static const std::string startPrintedQuote("\xE2\x80\x9C");
    static const std::string endPrintedQuote("\xE2\x80\x9D");
    for (;
         i < wkt.size() && (inString || (wkt[i] != '[' && wkt[i] != ',' &&
                                         wkt[i] != ']' && !::isspace(wkt[i])));
         ++i) {
        if (wkt[i] == '"') {
            inString = !inString;
        } else if (i + 3 <= wkt.size() &&
                   (wkt.substr(i, 3) == startPrintedQuote ||
                    wkt.substr(i, 3) == endPrintedQuote)) {
            inString = !inString;
        }
        value += wkt[i];
    }
    i = skipSpace(wkt, i);
    if (i == wkt.size()) {
        if (indexStart == 0) {
            throw ParsingException("missing [");
        } else {
            throw ParsingException("missing , or ]");
        }
    }

    auto node = util::nn_make_shared<WKTNode>(value);

    if (indexStart > 0) {
        if (wkt[i] == ',') {
            indexEnd = i + 1;
            return node;
        }
        if (wkt[i] == ']') {
            indexEnd = i;
            return node;
        }
    }
    if (wkt[i] != '[') {
        throw ParsingException("missing [");
    }
    ++i; // skip [
    i = skipSpace(wkt, i);
    while (i < wkt.size() && wkt[i] != ']') {
        size_t indexEndChild;
        node->addChild(createFrom(wkt, i, recLevel + 1, indexEndChild));
        assert(indexEndChild > i);
        i = indexEndChild;
        i = skipSpace(wkt, i);
    }
    if (i == wkt.size() || wkt[i] != ']') {
        throw ParsingException("missing ]");
    }
    indexEnd = i + 1;
    return node;
}

WKTNodeNNPtr WKTNode::createFrom(const std::string &wkt, size_t indexStart) {
    size_t indexEnd;
    return createFrom(wkt, indexStart, 0, indexEnd);
}

// ---------------------------------------------------------------------------

std::string WKTNode::toString() const {
    std::string str(d->value_);
    if (!d->children_.empty()) {
        str += "[";
        bool first = true;
        for (auto &child : d->children_) {
            if (!first) {
                str += ",";
            }
            first = false;
            str += child->toString();
        }
        str += "]";
    }
    return str;
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
struct WKTParser::Private {
    bool strict_ = true;
    std::vector<std::string> warningList_{};

    IdentifierPtr buildId(WKTNodeNNPtr node, bool tolerant = true);
    PropertyMap buildProperties(WKTNodeNNPtr node);
    UnitOfMeasure
    buildUnit(WKTNodeNNPtr node,
              UnitOfMeasure::Type type = UnitOfMeasure::Type::UNKNOWN);
    UnitOfMeasure buildUnitInSubNode(
        WKTNodeNNPtr node,
        common::UnitOfMeasure::Type type = UnitOfMeasure::Type::UNKNOWN);
    EllipsoidNNPtr buildEllipsoid(WKTNodeNNPtr node);
    PrimeMeridianNNPtr
    buildPrimeMeridian(WKTNodeNNPtr node,
                       const UnitOfMeasure &defaultAngularUnit);
    GeodeticReferenceFrameNNPtr
    buildGeodeticReferenceFrame(WKTNodeNNPtr node,
                                PrimeMeridianNNPtr primeMeridian);
    MeridianNNPtr buildMeridian(WKTNodeNNPtr node);
    CoordinateSystemAxisNNPtr buildAxis(WKTNodeNNPtr node,
                                        const UnitOfMeasure &unitIn,
                                        bool isGeocentric,
                                        int expectedOrderNum);
    CoordinateSystemNNPtr buildCS(WKTNodePtr node, /* maybe null */
                                  WKTNodeNNPtr parentNode);
    GeodeticCRSNNPtr buildGeodeticCRS(WKTNodeNNPtr node);
    UnitOfMeasure
    guessUnitForParameter(const std::string &paramName,
                          const UnitOfMeasure &defaultLinearUnit,
                          const UnitOfMeasure &defaultAngularUnit);
    ConversionNNPtr buildConversion(WKTNodeNNPtr node,
                                    const UnitOfMeasure &defaultLinearUnit,
                                    const UnitOfMeasure &defaultAngularUnit);
    ConversionNNPtr buildProjection(WKTNodeNNPtr projCRSNode,
                                    WKTNodeNNPtr projectionNode,
                                    const UnitOfMeasure &defaultLinearUnit,
                                    const UnitOfMeasure &defaultAngularUnit);
    ProjectedCRSNNPtr buildProjectedCRS(WKTNodeNNPtr node);
};

// ---------------------------------------------------------------------------

WKTParser::WKTParser() : d(internal::make_unique<Private>()) {}

// ---------------------------------------------------------------------------

WKTParser::~WKTParser() = default;

// ---------------------------------------------------------------------------

WKTParser &WKTParser::setStrict(bool strict) {
    d->strict_ = strict;
    return *this;
}

// ---------------------------------------------------------------------------

std::vector<std::string> WKTParser::warningList() const {
    return d->warningList_;
}

// ---------------------------------------------------------------------------

static double asDouble(const std::string &val) { return std::stod(val); }

// ---------------------------------------------------------------------------

IdentifierPtr WKTParser::Private::buildId(WKTNodeNNPtr node, bool tolerant) {
    if (node->children().size() >= 2) {
        auto codeSpace = stripQuotes(node->children()[0]->value());
        auto code = stripQuotes(node->children()[1]->value());
        auto citationNode = node->lookForChild(WKTConstants::CITATION);
        auto uriNode = node->lookForChild(WKTConstants::URI);
        PropertyMap propertiesId;
        propertiesId.set(Identifier::CODESPACE_KEY, codeSpace);
        bool authoritySet = false;
        if (citationNode) {
            if (citationNode->children().size() == 1) {
                authoritySet = true;
                propertiesId.set(
                    Identifier::AUTHORITY_KEY,
                    stripQuotes(citationNode->children()[0]->value()));
            }
        }
        if (!authoritySet) {
            propertiesId.set(Identifier::AUTHORITY_KEY, codeSpace);
        }
        if (uriNode) {
            if (uriNode->children().size() == 1) {
                propertiesId.set(Identifier::URI_KEY,
                                 stripQuotes(uriNode->children()[0]->value()));
            }
        }
        if (node->children().size() >= 3 &&
            node->children()[2]->children().size() == 0) {
            auto version = stripQuotes(node->children()[2]->value());
            propertiesId.set(Identifier::VERSION_KEY, version);
        }
        return Identifier::create(code, propertiesId);
    } else if (strict_ || !tolerant) {
        throw ParsingException("not enough children in " + node->value() +
                               " node");
    } else {
        warningList_.push_back("not enough children in " + node->value() +
                               " node");
        return nullptr;
    }
}

// ---------------------------------------------------------------------------

PropertyMap WKTParser::Private::buildProperties(WKTNodeNNPtr node) {
    PropertyMap properties;
    if (!node->children().empty()) {
        properties.set(IdentifiedObject::NAME_KEY,
                       stripQuotes(node->children()[0]->value()));
    }

    auto identifiers = ArrayOfBaseObject::create();
    for (const auto &subNode : node->children()) {
        if (ci_equal(subNode->value(), WKTConstants::ID) ||
            ci_equal(subNode->value(), WKTConstants::AUTHORITY)) {
            auto id = buildId(subNode);
            if (id) {
                identifiers->values.push_back(NN_CHECK_ASSERT(id));
            }
        }
    }
    if (!identifiers->values.empty()) {
        properties.set(IdentifiedObject::IDENTIFIERS_KEY, identifiers);
    }

    auto remarkNode = node->lookForChild(WKTConstants::REMARK);
    if (remarkNode) {
        if (remarkNode->children().size() == 1) {
            properties.set(IdentifiedObject::REMARKS_KEY,
                           stripQuotes(remarkNode->children()[0]->value()));
        } else {
            throw ParsingException("not required number of children in " +
                                   remarkNode->value() + " node");
        }
    }

    auto scopeNode = node->lookForChild(WKTConstants::SCOPE);
    auto areaNode = node->lookForChild(WKTConstants::AREA);
    auto bboxNode = node->lookForChild(WKTConstants::BBOX);
    if (scopeNode || areaNode || bboxNode) {
        optional<std::string> scope;
        if (scopeNode && scopeNode->children().size() == 1) {
            scope = stripQuotes(scopeNode->children()[0]->value());
        }
        ExtentPtr extent;
        if (areaNode || bboxNode) {
            util::optional<std::string> description;
            std::vector<GeographicExtentNNPtr> geogExtent;
            if (areaNode) {
                if (areaNode->children().size() == 1) {
                    description = stripQuotes(areaNode->children()[0]->value());
                } else {
                    throw ParsingException(
                        "not required number of children in " +
                        areaNode->value() + " node");
                }
            }
            if (bboxNode) {
                if (bboxNode->children().size() == 4) {
                    try {
                        double south =
                            asDouble(bboxNode->children()[0]->value());
                        double west =
                            asDouble(bboxNode->children()[1]->value());
                        double north =
                            asDouble(bboxNode->children()[2]->value());
                        double east =
                            asDouble(bboxNode->children()[3]->value());
                        auto bbox = GeographicBoundingBox::create(west, south,
                                                                  east, north);
                        geogExtent.emplace_back(bbox);
                    } catch (const std::exception &) {
                        throw ParsingException("not 4 double valus in " +
                                               bboxNode->value() + " node");
                    }
                } else {
                    throw ParsingException(
                        "not required number of children in " +
                        bboxNode->value() + " node");
                }
            }
            extent = Extent::create(description, geogExtent).as_nullable();
        }
        properties.set(ObjectUsage::OBJECT_DOMAIN_KEY,
                       ObjectDomain::create(scope, extent));
    }

    return properties;
}

// ---------------------------------------------------------------------------

UnitOfMeasure WKTParser::Private::buildUnit(WKTNodeNNPtr node,
                                            UnitOfMeasure::Type type) {
    const auto &children = node->children();
    if (children.size() < 2) {
        throw ParsingException("not enough children in " + node->value() +
                               " node");
    }
    try {
        std::string unitName(stripQuotes(children[0]->value()));
        PropertyMap properties(buildProperties(node));
        auto idNode = node->lookForChild(WKTConstants::ID);
        if (!idNode) {
            idNode = node->lookForChild(WKTConstants::AUTHORITY);
        }
        if (idNode && idNode->children().size() < 2) {
            if (strict_) {
                throw ParsingException("not enough children in " +
                                       idNode->value() + " node");
            } else {
                warningList_.push_back("not enough children in " +
                                       idNode->value() + " node");
            }
        }
        const bool hasValidNode = idNode && idNode->children().size() >= 2;
        if (type == UnitOfMeasure::Type::UNKNOWN) {
            if (ci_equal(unitName, "METER") || ci_equal(unitName, "METRE")) {
                type = UnitOfMeasure::Type::LINEAR;
            } else if (ci_equal(unitName, "DEGREE") ||
                       ci_equal(unitName, "GRAD")) {
                type = UnitOfMeasure::Type::ANGULAR;
            }
        }
        return UnitOfMeasure(
            stripQuotes(children[0]->value()), asDouble(children[1]->value()),
            type, hasValidNode ? stripQuotes(idNode->children()[0]->value())
                               : std::string(),
            hasValidNode ? stripQuotes(idNode->children()[1]->value())
                         : std::string());
    } catch (const std::exception &e) {
        throw ParsingException(std::string("buildUnit: ") + e.what());
    }
}

// ---------------------------------------------------------------------------

// node here is a parent node, not a UNIT/LENGTHUNIT/ANGLEUNIT node
UnitOfMeasure WKTParser::Private::buildUnitInSubNode(WKTNodeNNPtr node,
                                                     UnitOfMeasure::Type type) {
    auto unitNode = node->lookForChild(WKTConstants::LENGTHUNIT);
    if (unitNode) {
        return buildUnit(NN_CHECK_ASSERT(unitNode),
                         UnitOfMeasure::Type::LINEAR);
    }

    unitNode = node->lookForChild(WKTConstants::ANGLEUNIT);
    if (unitNode) {
        return buildUnit(NN_CHECK_ASSERT(unitNode),
                         UnitOfMeasure::Type::ANGULAR);
    }

    unitNode = node->lookForChild(WKTConstants::SCALEUNIT);
    if (unitNode) {
        return buildUnit(NN_CHECK_ASSERT(unitNode), UnitOfMeasure::Type::SCALE);
    }

    unitNode = node->lookForChild(WKTConstants::UNIT);
    if (unitNode) {
        return buildUnit(NN_CHECK_ASSERT(unitNode), type);
    }

    return UnitOfMeasure();
}

// ---------------------------------------------------------------------------

EllipsoidNNPtr WKTParser::Private::buildEllipsoid(WKTNodeNNPtr node) {
    const auto &children = node->children();
    if (children.size() < 3) {
        throw ParsingException("not enough children in " + node->value() +
                               " node");
    }
    try {
        UnitOfMeasure unit =
            buildUnitInSubNode(node, UnitOfMeasure::Type::LINEAR);
        if (unit == UnitOfMeasure()) {
            unit = UnitOfMeasure::METRE;
        }
        Length semiMajorAxis(asDouble(children[1]->value()), unit);
        Scale invFlattening(asDouble(children[2]->value()));
        return Ellipsoid::createFlattenedSphere(buildProperties(node),
                                                semiMajorAxis, invFlattening);
    } catch (const std::exception &e) {
        throw ParsingException(std::string("buildEllipsoid: ") + e.what());
    }
}

// ---------------------------------------------------------------------------

PrimeMeridianNNPtr WKTParser::Private::buildPrimeMeridian(
    WKTNodeNNPtr node, const UnitOfMeasure &defaultAngularUnit) {
    const auto &children = node->children();
    if (children.size() < 2) {
        throw ParsingException("not enough children in " + node->value() +
                               " node");
    }
    auto name = stripQuotes(children[0]->value());
    UnitOfMeasure unit = buildUnitInSubNode(node, UnitOfMeasure::Type::ANGULAR);
    if (unit == UnitOfMeasure()) {
        unit = defaultAngularUnit;
        if (unit == UnitOfMeasure()) {
            unit = UnitOfMeasure::DEGREE;
        }
    }
    try {
        double angleValue = asDouble(children[1]->value());

        // Correct for GDAL WKT1 departure
        if (name == "Paris" && std::fabs(angleValue - 2.33722917) < 1e-8 &&
            unit == UnitOfMeasure::GRAD) {
            angleValue = 2.5969213;
        }

        Angle angle(angleValue, unit);
        return PrimeMeridian::create(buildProperties(node), angle);
    } catch (const std::exception &e) {
        throw ParsingException(std::string("buildPrimeMeridian: ") + e.what());
    }
}

// ---------------------------------------------------------------------------

GeodeticReferenceFrameNNPtr WKTParser::Private::buildGeodeticReferenceFrame(
    WKTNodeNNPtr node, PrimeMeridianNNPtr primeMeridian) {
    auto ellipsoidNode = node->lookForChild(WKTConstants::ELLIPSOID);
    if (!ellipsoidNode) {
        ellipsoidNode = node->lookForChild(WKTConstants::SPHEROID);
    }
    if (!ellipsoidNode) {
        throw ParsingException("Missing ELLIPSOID node");
    }
    auto ellipsoid = buildEllipsoid(NN_CHECK_ASSERT(ellipsoidNode));

    auto anchorNode = node->lookForChild(WKTConstants::ANCHOR);
    optional<std::string> anchor;
    if (anchorNode && anchorNode->children().size() == 1) {
        anchor = stripQuotes(anchorNode->children()[0]->value());
    }

    return GeodeticReferenceFrame::create(buildProperties(node), ellipsoid,
                                          anchor, primeMeridian);
}

// ---------------------------------------------------------------------------

MeridianNNPtr WKTParser::Private::buildMeridian(WKTNodeNNPtr node) {
    const auto &children = node->children();
    if (children.size() < 2) {
        throw ParsingException("not enough children in " + node->value() +
                               " node");
    }
    UnitOfMeasure unit = buildUnitInSubNode(node, UnitOfMeasure::Type::ANGULAR);
    try {
        double angleValue = asDouble(children[0]->value());
        Angle angle(angleValue, unit);
        return Meridian::create(angle);
    } catch (const std::exception &e) {
        throw ParsingException(std::string("buildMeridian: ") + e.what());
    }
}

// ---------------------------------------------------------------------------

CoordinateSystemAxisNNPtr
WKTParser::Private::buildAxis(WKTNodeNNPtr node, const UnitOfMeasure &unitIn,
                              bool isGeocentric, int expectedOrderNum) {
    const auto &children = node->children();
    if (children.size() < 2) {
        throw ParsingException("not enough children in " + node->value() +
                               " node");
    }

    auto orderNode = node->lookForChild(WKTConstants::ORDER);
    if (orderNode) {
        const auto &orderNodeChildren = orderNode->children();
        if (orderNodeChildren.size() != 1) {
            throw ParsingException("not enough children in " +
                                   orderNode->value() + " node");
        }
        int orderNum;
        try {
            orderNum = std::stoi(orderNodeChildren[0]->value());
        } catch (const std::exception &) {
            throw ParsingException(
                std::string("buildAxis: invalid ORDER value: ") +
                orderNodeChildren[0]->value());
        }
        if (orderNum != expectedOrderNum) {
            throw ParsingException(
                std::string("buildAxis: did not get expected ORDER value: ") +
                orderNodeChildren[0]->value());
        }
    }

    // The axis designation in WK2 can be: "name", "(abbrev)" or "name (abbrev)"
    std::string axisDesignation(stripQuotes(children[0]->value()));
    size_t sepPos = axisDesignation.find(" (");
    std::string axisName;
    std::string axisAbbrev;
    if (sepPos != std::string::npos && axisDesignation.back() == ')') {
        axisName = CoordinateSystemAxis::normalizeAxisName(
            axisDesignation.substr(0, sepPos));
        axisAbbrev = axisDesignation.substr(sepPos + 2);
        axisAbbrev.resize(axisAbbrev.size() - 1);
    } else if (!axisDesignation.empty() && axisDesignation[0] == '(' &&
               axisDesignation.back() == ')') {
        axisAbbrev = axisDesignation.substr(1, axisDesignation.size() - 2);
        if (axisAbbrev == AxisAbbreviation::E) {
            axisName = AxisName::Easting;
        } else if (axisAbbrev == AxisAbbreviation::N) {
            axisName = AxisName::Northing;
        } else if (axisAbbrev == AxisAbbreviation::lat) {
            axisName = AxisName::Latitude;
        } else if (axisAbbrev == AxisAbbreviation::lon) {
            axisName = AxisName::Longitude;
        }
    } else {
        axisName = CoordinateSystemAxis::normalizeAxisName(axisDesignation);
        if (axisName == AxisName::Latitude) {
            axisAbbrev = AxisAbbreviation::lat;
        } else if (axisName == AxisName::Longitude) {
            axisAbbrev = AxisAbbreviation::lon;
        } else if (axisName == AxisName::Ellipsoidal_height) {
            axisAbbrev = AxisAbbreviation::h;
        }
    }
    std::string dirString = children[1]->value();
    const AxisDirection *direction = AxisDirection::valueOf(dirString);

    // WKT2, geocentric CS: axis names are omitted
    if (direction == &AxisDirection::GEOCENTRIC_X &&
        axisAbbrev == AxisAbbreviation::X && axisName.empty()) {
        axisName = AxisName::Geocentric_X;
    } else if (direction == &AxisDirection::GEOCENTRIC_Y &&
               axisAbbrev == AxisAbbreviation::Y && axisName.empty()) {
        axisName = AxisName::Geocentric_Y;
    } else if (direction == &AxisDirection::GEOCENTRIC_Z &&
               axisAbbrev == AxisAbbreviation::Z && axisName.empty()) {
        axisName = AxisName::Geocentric_Z;
    }

    // WKT1
    if (!direction && isGeocentric && axisName == AxisName::Geocentric_X) {
        axisAbbrev = AxisAbbreviation::X;
        direction = &AxisDirection::GEOCENTRIC_X;
    } else if (!direction && isGeocentric &&
               axisName == AxisName::Geocentric_Y) {
        axisAbbrev = AxisAbbreviation::Y;
        direction = &AxisDirection::GEOCENTRIC_Y;
    } else if (isGeocentric && axisName == AxisName::Geocentric_Z &&
               dirString == AxisDirectionWKT1::NORTH.toString()) {
        axisAbbrev = AxisAbbreviation::Z;
        direction = &AxisDirection::GEOCENTRIC_Z;
    } else if (dirString == AxisDirectionWKT1::OTHER.toString()) {
        direction = &AxisDirection::UNSPECIFIED;
    } else if (!direction && AxisDirectionWKT1::valueOf(dirString) != nullptr) {
        direction = AxisDirection::valueOf(tolower(dirString));
    }

    if (!direction) {
        throw ParsingException("unhandled axis direction: " +
                               children[1]->value());
    }
    UnitOfMeasure unit(buildUnitInSubNode(node));
    if (unit == UnitOfMeasure()) {
        // If no unit in the AXIS node, use the one potentially coming from
        // the CS.
        unit = unitIn;
    }

    MeridianPtr meridian;
    auto meridianNode = node->lookForChild(WKTConstants::MERIDIAN);
    if (meridianNode) {
        meridian = buildMeridian(NN_CHECK_ASSERT(meridianNode)).as_nullable();
    }

    return CoordinateSystemAxis::create(
        buildProperties(node).set(IdentifiedObject::NAME_KEY, axisName),
        axisAbbrev, *direction, unit, meridian);
}

// ---------------------------------------------------------------------------

CoordinateSystemNNPtr
WKTParser::Private::buildCS(WKTNodePtr node, /* maybe null */
                            WKTNodeNNPtr parentNode) {
    bool isGeocentric = false;
    std::string csType;
    const int numberOfAxis =
        parentNode->countChildrenOfName(WKTConstants::AXIS);
    int axisCount = numberOfAxis;
    if (node) {
        const auto &children = node->children();
        if (children.size() < 2) {
            throw ParsingException("not enough children in " + node->value() +
                                   " node");
        }
        csType = children[0]->value();
        try {
            axisCount = std::stoi(children[1]->value());
        } catch (const std::exception &) {
            throw ParsingException(
                std::string("buildCS: invalid CS axis count: ") +
                children[1]->value());
        }
    } else {
        if (ci_equal(parentNode->value(), WKTConstants::GEOCCS)) {
            csType = "Cartesian";
            isGeocentric = true;
            if (axisCount == 0) {
                return CartesianCS::createGeocentric();
            }
        } else if (ci_equal(parentNode->value(), WKTConstants::GEOGCS)) {
            csType = "Ellipsoidal";
            if (axisCount == 0) {
                // Missing axis with GEOGCS ? Presumably Long/Lat order implied
                return EllipsoidalCS::create(
                    PropertyMap(),
                    CoordinateSystemAxis::create(
                        PropertyMap().set(IdentifiedObject::NAME_KEY,
                                          AxisName::Longitude),
                        AxisAbbreviation::lon, AxisDirection::EAST,
                        UnitOfMeasure::DEGREE),
                    CoordinateSystemAxis::create(
                        PropertyMap().set(IdentifiedObject::NAME_KEY,
                                          AxisName::Latitude),
                        AxisAbbreviation::lat, AxisDirection::NORTH,
                        UnitOfMeasure::DEGREE));
            }
        } else if (ci_equal(parentNode->value(), WKTConstants::BASEGEODCRS) ||
                   ci_equal(parentNode->value(), WKTConstants::BASEGEOGCRS)) {
            csType = "Ellipsoidal";
            if (axisCount == 0) {
                return EllipsoidalCS::createLatitudeLongitudeDegree();
            }
        } else if (ci_equal(parentNode->value(), WKTConstants::PROJCS)) {
            csType = "Cartesian";
            if (axisCount == 0) {
                return CartesianCS::createEastingNorthingMetre();
            }
        } else {
            // Shouldn't happen normally
            throw ParsingException(
                std::string("buildCS: unexpected parent node: ") +
                parentNode->value());
        }
    }

    if (axisCount != 2 && axisCount != 3) {
        throw ParsingException(std::string("buildCS: invalid CS axis count"));
    }
    if (numberOfAxis != axisCount) {
        throw ParsingException("buildCS: declared number of axis by CS node "
                               "and number of AXIS are inconsistent");
    }

    UnitOfMeasure unit = buildUnitInSubNode(
        parentNode, ci_equal(csType, "ellipsoidal")
                        ? UnitOfMeasure::Type::ANGULAR
                        : ci_equal(csType, "Cartesian")
                              ? UnitOfMeasure::Type::LINEAR
                              : UnitOfMeasure::Type::UNKNOWN);

    auto firstAxis = buildAxis(
        NN_CHECK_ASSERT(parentNode->lookForChild(WKTConstants::AXIS, 0)), unit,
        isGeocentric, 1);
    auto secondAxis = buildAxis(
        NN_CHECK_ASSERT(parentNode->lookForChild(WKTConstants::AXIS, 1)), unit,
        isGeocentric, 2);

    PropertyMap csMap =
        node ? buildProperties(NN_CHECK_ASSERT(node)) : PropertyMap();
    if (ci_equal(csType, "ellipsoidal")) {
        if (axisCount == 2) {
            return EllipsoidalCS::create(csMap, firstAxis, secondAxis);
        } else {
            auto thirdAxis = buildAxis(NN_CHECK_ASSERT(parentNode->lookForChild(
                                           WKTConstants::AXIS, 2)),
                                       unit, false, 3);
            return EllipsoidalCS::create(csMap, firstAxis, secondAxis,
                                         thirdAxis);
        }
    } else if (ci_equal(csType, "Cartesian")) {
        if (axisCount == 2) {
            return CartesianCS::create(csMap, firstAxis, secondAxis);
        } else {
            auto thirdAxis = buildAxis(NN_CHECK_ASSERT(parentNode->lookForChild(
                                           WKTConstants::AXIS, 2)),
                                       unit, isGeocentric, 3);
            return CartesianCS::create(csMap, firstAxis, secondAxis, thirdAxis);
        }
    } else if (ci_equal(csType, "spherical")) {
        if (axisCount == 3) {
            auto thirdAxis = buildAxis(NN_CHECK_ASSERT(parentNode->lookForChild(
                                           WKTConstants::AXIS, 2)),
                                       unit, false, 3);
            return SphericalCS::create(csMap, firstAxis, secondAxis, thirdAxis);
        } else {
            throw ParsingException(
                std::string("buildCS: invalid CS axis count for spherical"));
        }
    } else {
        throw ParsingException(std::string("unhandled CS type: ") + csType);
    }
}

// ---------------------------------------------------------------------------

GeodeticCRSNNPtr WKTParser::Private::buildGeodeticCRS(WKTNodeNNPtr node) {
    auto datumNode = node->lookForChild(WKTConstants::DATUM);
    if (!datumNode) {
        datumNode = node->lookForChild(WKTConstants::GEODETICDATUM);
        if (!datumNode) {
            datumNode =
                node->lookForChild(WKTConstants::GEODETICREFERENCEFRAME);
            if (!datumNode) {
                throw ParsingException("Missing DATUM node");
            }
        }
    }
    auto csNode = node->lookForChild(WKTConstants::CS);
    if (!csNode && !ci_equal(node->value(), WKTConstants::GEOGCS) &&
        !ci_equal(node->value(), WKTConstants::GEOCCS) &&
        !ci_equal(node->value(), WKTConstants::BASEGEODCRS) &&
        !ci_equal(node->value(), WKTConstants::BASEGEOGCRS)) {
        throw ParsingException("Missing CS node");
    }

    auto primeMeridianNode = node->lookForChild(WKTConstants::PRIMEM);
    if (!primeMeridianNode) {
        primeMeridianNode = node->lookForChild(WKTConstants::PRIMEMERIDIAN);
    }

    auto angularUnit =
        buildUnitInSubNode(node, ci_equal(node->value(), WKTConstants::GEOGCS)
                                     ? UnitOfMeasure::Type::ANGULAR
                                     : UnitOfMeasure::Type::UNKNOWN);
    if (angularUnit.type() != UnitOfMeasure::Type::ANGULAR) {
        angularUnit = UnitOfMeasure();
    }

    auto primeMeridian =
        primeMeridianNode ? buildPrimeMeridian(
                                NN_CHECK_ASSERT(primeMeridianNode), angularUnit)
                          : PrimeMeridian::GREENWICH;

    auto datum =
        buildGeodeticReferenceFrame(NN_CHECK_ASSERT(datumNode), primeMeridian);
    auto cs = buildCS(csNode, node);
    auto ellipsoidalCS = nn_dynamic_pointer_cast<EllipsoidalCS>(cs);
    if (ellipsoidalCS) {
        assert(!ci_equal(node->value(), WKTConstants::GEOCCS));
        return GeographicCRS::create(buildProperties(node), datum,
                                     NN_CHECK_ASSERT(ellipsoidalCS));
    } else if (ci_equal(node->value(), WKTConstants::GEOGCRS) ||
               ci_equal(node->value(), WKTConstants::GEOGRAPHICCRS) ||
               ci_equal(node->value(), WKTConstants::BASEGEOGCRS)) {
        // This is a WKT2-2018 GeographicCRS. An ellipsoidal CS is expected
        throw ParsingException("ellipsoidal CS expected, but found " +
                               cs->getWKT2Type());
    }

    auto cartesianCS = nn_dynamic_pointer_cast<CartesianCS>(cs);
    if (cartesianCS) {
        if (cartesianCS->axisList().size() != 3) {
            throw ParsingException(
                "Cartesian CS for a GeodeticCRS should have 3 nodes");
        }
        return GeodeticCRS::create(buildProperties(node), datum,
                                   NN_CHECK_ASSERT(cartesianCS));
    }

    auto sphericalCS = nn_dynamic_pointer_cast<SphericalCS>(cs);
    if (sphericalCS) {
        return GeodeticCRS::create(buildProperties(node), datum,
                                   NN_CHECK_ASSERT(sphericalCS));
    }

    throw ParsingException("unhandled CS type: " + cs->getWKT2Type());
}

// ---------------------------------------------------------------------------

UnitOfMeasure WKTParser::Private::guessUnitForParameter(
    const std::string &paramName, const UnitOfMeasure &defaultLinearUnit,
    const UnitOfMeasure &defaultAngularUnit) {
    UnitOfMeasure unit;
    if (ci_find(paramName, "latitude") != std::string::npos ||
        ci_find(paramName, "longitude") != std::string::npos ||
        ci_find(paramName, "meridian") != std::string::npos ||
        ci_find(paramName, "parallel") != std::string::npos) {
        unit = defaultAngularUnit;
    } else if (ci_find(paramName, "easting") != std::string::npos ||
               ci_find(paramName, "northing") != std::string::npos) {
        unit = defaultLinearUnit;
    } else if (ci_find(paramName, "scale") != std::string::npos) {
        unit = UnitOfMeasure::SCALE_UNITY;
    }
    return unit;
}

// ---------------------------------------------------------------------------

ConversionNNPtr
WKTParser::Private::buildConversion(WKTNodeNNPtr node,
                                    const UnitOfMeasure &defaultLinearUnit,
                                    const UnitOfMeasure &defaultAngularUnit) {
    auto methodNode = node->lookForChild(WKTConstants::METHOD);
    if (!methodNode) {
        methodNode = node->lookForChild(WKTConstants::PROJECTION);
        if (!methodNode) {
            throw ParsingException("Missing METHOD node");
        }
    }
    if (methodNode->children().size() < 1) {
        throw ParsingException("not enough children in " +
                               WKTConstants::METHOD + " node");
    }

    std::vector<OperationParameterNNPtr> parameters;
    std::vector<ParameterValueNNPtr> values;
    for (const auto &childNode : node->children()) {
        if (ci_equal(childNode->value(), WKTConstants::PARAMETER)) {
            if (childNode->children().size() < 2) {
                throw ParsingException("not enough children in " +
                                       WKTConstants::PARAMETER + " node");
            }
            parameters.push_back(
                OperationParameter::create(buildProperties(childNode)));
            auto paramValue = childNode->children()[1]->value();
            try {
                double val = std::stod(paramValue);
                auto unit = buildUnitInSubNode(childNode);
                if (unit == UnitOfMeasure()) {
                    auto paramName = childNode->children()[0]->value();
                    unit = guessUnitForParameter(paramName, defaultLinearUnit,
                                                 defaultAngularUnit);
                }
                values.push_back(ParameterValue::create(Measure(val, unit)));
            } catch (const std::exception &) {
                throw ParsingException("unhandled parameter value type : " +
                                       paramValue);
            }
        }
    }
    return Conversion::create(buildProperties(node),
                              buildProperties(NN_CHECK_ASSERT(methodNode)),
                              parameters, values);
}

// ---------------------------------------------------------------------------

ConversionNNPtr
WKTParser::Private::buildProjection(WKTNodeNNPtr projCRSNode,
                                    WKTNodeNNPtr projectionNode,
                                    const UnitOfMeasure &defaultLinearUnit,
                                    const UnitOfMeasure &defaultAngularUnit) {
    if (projectionNode->children().size() < 1) {
        throw ParsingException("not enough children in " +
                               WKTConstants::PROJECTION + " node");
    }
    std::string projectionName =
        stripQuotes(projectionNode->children()[0]->value());
    const MethodMapping *mapping = getMappingFromWKT1(projectionName);
    PropertyMap propertiesMethod;
    if (mapping) {
        projectionName = mapping->wkt2_name;
        propertiesMethod.set(Identifier::CODE_KEY, mapping->epsg_code);
        propertiesMethod.set(Identifier::CODESPACE_KEY, "EPSG");
    }
    propertiesMethod.set(IdentifiedObject::NAME_KEY, projectionName);

    std::vector<OperationParameterNNPtr> parameters;
    std::vector<ParameterValueNNPtr> values;
    for (const auto &childNode : projCRSNode->children()) {
        if (ci_equal(childNode->value(), WKTConstants::PARAMETER)) {
            if (childNode->children().size() < 2) {
                throw ParsingException("not enough children in " +
                                       WKTConstants::PARAMETER + " node");
            }
            PropertyMap propertiesParameter;
            std::string parameterName(
                stripQuotes(childNode->children()[0]->value()));
            const auto *paramMapping =
                mapping ? getMappingFromWKT1(mapping, parameterName) : nullptr;
            if (paramMapping) {
                parameterName = paramMapping->wkt2_name;
                propertiesParameter.set(Identifier::CODE_KEY,
                                        paramMapping->epsg_code);
                propertiesParameter.set(Identifier::CODESPACE_KEY, "EPSG");
            }
            propertiesParameter.set(IdentifiedObject::NAME_KEY, parameterName);
            parameters.push_back(
                OperationParameter::create(propertiesParameter));
            auto paramValue = childNode->children()[1]->value();
            try {
                double val = std::stod(paramValue);
                auto unit = buildUnitInSubNode(childNode);
                if (unit == UnitOfMeasure()) {
                    auto paramName = childNode->children()[0]->value();
                    unit = guessUnitForParameter(paramName, defaultLinearUnit,
                                                 defaultAngularUnit);
                }
                values.push_back(ParameterValue::create(Measure(val, unit)));
            } catch (const std::exception &) {
                throw ParsingException("unhandled parameter value type : " +
                                       paramValue);
            }
        }
    }

    return Conversion::create(
               PropertyMap().set(IdentifiedObject::NAME_KEY, "unnamed"),
               propertiesMethod, parameters, values)
        ->identify();
}

// ---------------------------------------------------------------------------

ProjectedCRSNNPtr WKTParser::Private::buildProjectedCRS(WKTNodeNNPtr node) {
    auto baseGeodCRSNode = node->lookForChild(WKTConstants::BASEGEODCRS);
    if (!baseGeodCRSNode) {
        baseGeodCRSNode = node->lookForChild(WKTConstants::BASEGEOGCRS);
        if (!baseGeodCRSNode) {
            baseGeodCRSNode = node->lookForChild(WKTConstants::GEOGCS);
            if (!baseGeodCRSNode) {
                throw ParsingException(
                    "Missing BASEGEODCRS / BASEGEOGCRS / GEOGCS node");
            }
        }
    }
    auto baseGeodCRS = buildGeodeticCRS(NN_CHECK_ASSERT(baseGeodCRSNode));

    auto conversionNode = node->lookForChild(WKTConstants::CONVERSION);
    auto projectionNode = node->lookForChild(WKTConstants::PROJECTION);
    if (!conversionNode && !projectionNode) {
        throw ParsingException("Missing CONVERSION node");
    }

    auto linearUnit = buildUnitInSubNode(node);
    auto angularUnit = buildUnitInSubNode(NN_CHECK_ASSERT(baseGeodCRSNode));

    auto conversion =
        conversionNode ? buildConversion(NN_CHECK_ASSERT(conversionNode),
                                         linearUnit, angularUnit)
                       : buildProjection(node, NN_CHECK_ASSERT(projectionNode),
                                         linearUnit, angularUnit);

    auto csNode = node->lookForChild(WKTConstants::CS);
    if (!csNode && !ci_equal(node->value(), WKTConstants::PROJCS)) {
        throw ParsingException("Missing CS node");
    }
    auto cs = buildCS(csNode, node);
    auto cartesianCS = nn_dynamic_pointer_cast<CartesianCS>(cs);
    if (!cartesianCS) {
        throw ParsingException("CS node is not of type Cartesian");
    }

    return ProjectedCRS::create(buildProperties(node), baseGeodCRS, conversion,
                                NN_CHECK_ASSERT(cartesianCS));
}

// ---------------------------------------------------------------------------

static bool isGeodeticCRS(const std::string &name) {
    return ci_equal(name, WKTConstants::GEODCRS) ||       // WKT2
           ci_equal(name, WKTConstants::GEODETICCRS) ||   // WKT2
           ci_equal(name, WKTConstants::GEOGCRS) ||       // WKT2 2018
           ci_equal(name, WKTConstants::GEOGRAPHICCRS) || // WKT2 2018
           ci_equal(name, WKTConstants::GEOGCS) ||        // WKT1
           ci_equal(name, WKTConstants::GEOCCS);          // WKT1
}

// ---------------------------------------------------------------------------

BaseObjectNNPtr WKTParser::createFromWKT(const std::string &wkt) {
    WKTNodeNNPtr root = WKTNode::createFrom(wkt);
    const std::string &name(root->value());
    if (isGeodeticCRS(name)) {
        return util::nn_static_pointer_cast<BaseObject>(
            d->buildGeodeticCRS(root));
    }

    if (ci_equal(name, WKTConstants::DATUM) ||
        ci_equal(name, WKTConstants::GEODETICDATUM) ||
        ci_equal(name, WKTConstants::GEODETICREFERENCEFRAME)) {
        return util::nn_static_pointer_cast<BaseObject>(
            d->buildGeodeticReferenceFrame(root, PrimeMeridian::GREENWICH));
    }

    if (ci_equal(name, WKTConstants::ELLIPSOID) ||
        ci_equal(name, WKTConstants::SPHEROID)) {
        return util::nn_static_pointer_cast<BaseObject>(
            d->buildEllipsoid(root));
    }

    if (ci_equal(name, WKTConstants::PROJCS) ||
        ci_equal(name, WKTConstants::PROJCRS) ||
        ci_equal(name, WKTConstants::PROJECTEDCRS)) {
        return util::nn_static_pointer_cast<BaseObject>(
            d->buildProjectedCRS(root));
    }

    if (ci_equal(name, WKTConstants::ID) ||
        ci_equal(name, WKTConstants::AUTHORITY)) {
        return util::nn_static_pointer_cast<BaseObject>(
            NN_CHECK_ASSERT(d->buildId(root, false)));
    }

    throw ParsingException("unhandled keyword: " + name);
}

//! @endcond

// ---------------------------------------------------------------------------

FormattingException::FormattingException(const char *message)
    : Exception(message) {}

// ---------------------------------------------------------------------------

FormattingException::FormattingException(const std::string &message)
    : Exception(message) {}

// ---------------------------------------------------------------------------

FormattingException::FormattingException(const FormattingException &) = default;

// ---------------------------------------------------------------------------

FormattingException::~FormattingException() = default;

// ---------------------------------------------------------------------------

ParsingException::ParsingException(const char *message) : Exception(message) {}

// ---------------------------------------------------------------------------

ParsingException::ParsingException(const std::string &message)
    : Exception(message) {}

// ---------------------------------------------------------------------------

ParsingException::ParsingException(const ParsingException &) = default;

// ---------------------------------------------------------------------------

ParsingException::~ParsingException() = default;
