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

#include <algorithm>
#include <cassert>
#include <cctype>
#include <cmath>
#include <cstring>
#include <iomanip>
#include <locale>
#include <sstream> // std::ostringstream
#include <string>
#include <utility>
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

// PROJ include order is sensitive
// clang-format off
#include "projects.h"
#include "proj_api.h"
// clang-format on

using namespace NS_PROJ::common;
using namespace NS_PROJ::crs;
using namespace NS_PROJ::cs;
using namespace NS_PROJ::datum;
using namespace NS_PROJ::internal;
using namespace NS_PROJ::metadata;
using namespace NS_PROJ::operation;
using namespace NS_PROJ::util;

NS_PROJ_START
namespace io {

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
IWKTExportable::~IWKTExportable() = default;
//! @endcond

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
        bool use2018Keywords_ = false;
    };
    Params params_{};

    int indentLevel_ = 0;
    int level_ = 0;
    std::vector<bool> stackHasChild_{};
    std::vector<bool> stackHasId_{false};
    std::vector<bool> stackEmptyKeyword_{};
    std::vector<bool> outputUnitStack_{true};
    std::vector<bool> outputIdStack_{true};
    std::vector<UnitOfMeasureNNPtr> axisLinearUnitStack_{
        util::nn_make_shared<UnitOfMeasure>(UnitOfMeasure::NONE)};
    std::vector<UnitOfMeasureNNPtr> axisAngularUnitStack_{
        util::nn_make_shared<UnitOfMeasure>(UnitOfMeasure::NONE)};
    bool abridgedTransformation_ = false;
    bool useDerivingConversion_ = false;
    std::vector<double> toWGS84Parameters_{};
    std::string hDatumExtension_{};
    std::string vDatumExtension_{};
    std::vector<bool> inversionStack_{false};
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
//! @cond Doxygen_Suppress
WKTFormatter::~WKTFormatter() = default;
//! @endcond

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
    if (d->indentLevel_ > 0 || d->level_ > 0) {
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

WKTFormatter &WKTFormatter::setOutputId(bool outputIdIn) {
    if (d->indentLevel_ != 0) {
        throw Exception(
            "setOutputId() shall only be called when the stack state is empty");
    }
    d->outputIdStack_[0] = outputIdIn;
    return *this;
}

// ---------------------------------------------------------------------------

void WKTFormatter::Private::addNewLine() { result_ += '\n'; }

// ---------------------------------------------------------------------------

void WKTFormatter::Private::addIndentation() {
    result_ += std::string(indentLevel_ * params_.indentWidth_, ' ');
}

// ---------------------------------------------------------------------------

void WKTFormatter::enter() {
    if (d->indentLevel_ == 0 && d->level_ == 0) {
        d->stackHasChild_.push_back(false);
    }
    ++d->level_;
}

// ---------------------------------------------------------------------------

void WKTFormatter::leave() {
    assert(d->level_ > 0);
    --d->level_;
    if (d->indentLevel_ == 0 && d->level_ == 0) {
        d->stackHasChild_.pop_back();
    }
}

// ---------------------------------------------------------------------------

void WKTFormatter::startNode(const std::string &keyword, bool hasId) {
    if (!d->stackHasChild_.empty()) {
        d->startNewChild();
    }

    if (d->params_.multiLine_) {
        if ((d->indentLevel_ || d->level_) && !keyword.empty()) {
            if (!d->result_.empty()) {
                d->addNewLine();
            }
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

    // Starting from a node that has a ID, we should emit ID nodes for :
    // - this node
    // - and for METHOD&PARAMETER nodes in WKT2, unless idOnTopLevelOnly_ is
    // set.
    // For WKT2, all other intermediate nodes shouldn't have ID ("not
    // recommended")
    if (!d->params_.idOnTopLevelOnly_ && d->indentLevel_ >= 2 &&
        d->params_.version_ == WKTFormatter::Version::WKT2 &&
        (keyword == WKTConstants::METHOD ||
         keyword == WKTConstants::PARAMETER)) {
        pushOutputId(d->outputIdStack_[0]);
    } else if (d->indentLevel_ >= 2 &&
               d->params_.version_ == WKTFormatter::Version::WKT2) {
        pushOutputId(d->outputIdStack_[0] && !d->stackHasId_.back());
    } else {
        pushOutputId(outputId());
    }

    d->stackHasId_.push_back(hasId || d->stackHasId_.back());
}

// ---------------------------------------------------------------------------

void WKTFormatter::endNode() {
    assert(d->indentLevel_ > 0);
    d->stackHasId_.pop_back();
    popOutputId();
    d->indentLevel_--;
    bool emptyKeyword = d->stackEmptyKeyword_.back();
    d->stackEmptyKeyword_.pop_back();
    d->stackHasChild_.pop_back();
    if (!emptyKeyword)
        d->result_ += "]";
}

// ---------------------------------------------------------------------------

WKTFormatter &WKTFormatter::simulCurNodeHasId() {
    d->stackHasId_.back() = true;
    return *this;
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
    d->result_ += "\"" + replaceAll(str, "\"", "\"\"") + "\"";
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
    buffer.imbue(std::locale::classic());
    buffer << number;
    d->result_ += buffer.str();
}

// ---------------------------------------------------------------------------

void WKTFormatter::add(size_t number) {
    d->startNewChild();
    std::ostringstream buffer;
    buffer.imbue(std::locale::classic());
    buffer << number;
    d->result_ += buffer.str();
}

// ---------------------------------------------------------------------------

void WKTFormatter::add(double number, int precision) {
    d->startNewChild();
    std::ostringstream buffer;
    buffer.imbue(std::locale::classic());
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

void WKTFormatter::setAbridgedTransformation(bool outputIn) {
    d->abridgedTransformation_ = outputIn;
}

// ---------------------------------------------------------------------------

bool WKTFormatter::abridgedTransformation() const {
    return d->abridgedTransformation_;
}

// ---------------------------------------------------------------------------

void WKTFormatter::setUseDerivingConversion(bool useDerivingConversionIn) {
    d->useDerivingConversion_ = useDerivingConversionIn;
}

// ---------------------------------------------------------------------------

bool WKTFormatter::useDerivingConversion() const {
    return d->useDerivingConversion_;
}

// ---------------------------------------------------------------------------

void WKTFormatter::setTOWGS84Parameters(const std::vector<double> &params) {
    d->toWGS84Parameters_ = params;
}

// ---------------------------------------------------------------------------

const std::vector<double> &WKTFormatter::getTOWGS84Parameters() const {
    return d->toWGS84Parameters_;
}

// ---------------------------------------------------------------------------

void WKTFormatter::setVDatumExtension(const std::string &filename) {
    d->vDatumExtension_ = filename;
}

// ---------------------------------------------------------------------------

const std::string &WKTFormatter::getVDatumExtension() const {
    return d->vDatumExtension_;
}

// ---------------------------------------------------------------------------

void WKTFormatter::setHDatumExtension(const std::string &filename) {
    d->hDatumExtension_ = filename;
}

// ---------------------------------------------------------------------------

const std::string &WKTFormatter::getHDatumExtension() const {
    return d->hDatumExtension_;
}

// ---------------------------------------------------------------------------

void WKTFormatter::startInversion() {
    d->inversionStack_.push_back(!d->inversionStack_.back());
}

// ---------------------------------------------------------------------------

void WKTFormatter::stopInversion() {
    assert(!d->inversionStack_.empty());
    d->inversionStack_.pop_back();
}

// ---------------------------------------------------------------------------

bool WKTFormatter::isInverted() const { return d->inversionStack_.back(); }

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
struct WKTNode::Private {
    std::string value_{};
    std::vector<WKTNodeNNPtr> children_{};
};
//! @endcond

// ---------------------------------------------------------------------------

/** \brief Instanciate a WKTNode.
 *
 * @param valueIn the name of the node.
 */
WKTNode::WKTNode(const std::string &valueIn)
    : d(internal::make_unique<Private>()) {
    d->value_ = valueIn;
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
WKTNode::~WKTNode() = default;
//! @endcond

// ---------------------------------------------------------------------------

/** \brief Adds a child to the current node.
 *
 * @param child child to add. This should not be a parent of this node.
 */
void WKTNode::addChild(WKTNodeNNPtr child) { d->children_.push_back(child); }

// ---------------------------------------------------------------------------

/** \brief Return the (occurrence-1)th sub-node of name childName.
 *
 * @param childName name of the child.
 * @param occurrence occurrence index (starting at 0)
 * @return the child, or nullptr.
 */
WKTNodePtr WKTNode::lookForChild(const std::string &childName,
                                 int occurrence) const {
    int occCount = 0;
    for (const auto &child : d->children_) {
        if (ci_equal(child->value(), childName)) {
            if (occurrence == occCount) {
                return child;
            }
            occCount++;
        }
    }
    return nullptr;
}

// ---------------------------------------------------------------------------

/** \brief Return the count of children of given name.
 *
 * @param childName name of the children to look for.
 * @return count
 */
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

/** \brief Return the value of a node.
 */
const std::string &WKTNode::value() const { return d->value_; }

// ---------------------------------------------------------------------------

/** \brief Return the children of a node.
 */
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
    std::string closingStringMarker;
    bool inString = false;

    // As used in examples of OGC 12-063r5
    static const std::string startPrintedQuote("\xE2\x80\x9C");
    static const std::string endPrintedQuote("\xE2\x80\x9D");
    for (; i < wkt.size() &&
           (inString || (wkt[i] != '[' && wkt[i] != '(' && wkt[i] != ',' &&
                         wkt[i] != ']' && wkt[i] != ')' && !::isspace(wkt[i])));
         ++i) {
        if (wkt[i] == '"') {
            if (!inString) {
                inString = true;
                closingStringMarker = "\"";
            } else if (closingStringMarker == "\"") {
                if (i + 1 < wkt.size() && wkt[i + 1] == '"') {
                    i++;
                } else {
                    inString = false;
                    closingStringMarker.clear();
                }
            }
        } else if (i + 3 <= wkt.size() &&
                   wkt.substr(i, 3) == startPrintedQuote) {
            if (!inString) {
                inString = true;
                closingStringMarker = endPrintedQuote;
                value += "\"";
                i += 2;
                continue;
            }
        } else if (i + 3 <= wkt.size() &&
                   closingStringMarker == endPrintedQuote &&
                   wkt.substr(i, 3) == endPrintedQuote) {
            inString = false;
            closingStringMarker.clear();
            value += "\"";
            i += 2;
            continue;
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
        if (wkt[i] == ']' || wkt[i] == ')') {
            indexEnd = i;
            return node;
        }
    }
    if (wkt[i] != '[' && wkt[i] != '(') {
        throw ParsingException("missing [");
    }
    ++i; // skip [
    i = skipSpace(wkt, i);
    while (i < wkt.size() && wkt[i] != ']' && wkt[i] != ')') {
        size_t indexEndChild;
        node->addChild(createFrom(wkt, i, recLevel + 1, indexEndChild));
        assert(indexEndChild > i);
        i = indexEndChild;
        i = skipSpace(wkt, i);
    }
    if (i == wkt.size() || (wkt[i] != ']' && wkt[i] != ')')) {
        throw ParsingException("missing ]");
    }
    indexEnd = i + 1;
    return node;
}
// ---------------------------------------------------------------------------

/** \brief Instanciate a WKTNode hierarchy from a WKT string.
 *
 * @param wkt the WKT string to parse.
 * @param indexStart the start index in the wkt string.
 * @throw ParsingException
 */
WKTNodeNNPtr WKTNode::createFrom(const std::string &wkt, size_t indexStart) {
    size_t indexEnd;
    return createFrom(wkt, indexStart, 0, indexEnd);
}

// ---------------------------------------------------------------------------

static std::string escapeIfQuotedString(const std::string &str) {
    if (str.size() > 2 && str[0] == '"' && str.back() == '"') {
        return "\"" + replaceAll(str.substr(1, str.size() - 2), "\"", "\"\"") +
               "\"";
    } else {
        return str;
    }
}

// ---------------------------------------------------------------------------

/** \brief Return a WKT representation of the tree structure.
 */
std::string WKTNode::toString() const {
    std::string str(escapeIfQuotedString(d->value_));
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
    std::vector<double> toWGS84Parameters_{};
    std::string datumPROJ4Grids_{};

    void emitRecoverableAssertion(const std::string &errorMsg);

    BaseObjectNNPtr build(WKTNodeNNPtr node);

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
    optional<std::string> getAnchor(WKTNodeNNPtr node);
    GeodeticReferenceFrameNNPtr
    buildGeodeticReferenceFrame(WKTNodeNNPtr node,
                                PrimeMeridianNNPtr primeMeridian);
    MeridianNNPtr buildMeridian(WKTNodeNNPtr node);
    CoordinateSystemAxisNNPtr buildAxis(WKTNodeNNPtr node,
                                        const UnitOfMeasure &unitIn,
                                        bool isGeocentric,
                                        int expectedOrderNum);
    CoordinateSystemNNPtr buildCS(WKTNodePtr node, /* maybe null */
                                  WKTNodeNNPtr parentNode,
                                  const UnitOfMeasure &defaultAngularUnit);
    GeodeticCRSNNPtr buildGeodeticCRS(WKTNodeNNPtr node);
    CRSNNPtr buildDerivedGeodeticCRS(WKTNodeNNPtr node);
    UnitOfMeasure
    guessUnitForParameter(const std::string &paramName,
                          const UnitOfMeasure &defaultLinearUnit,
                          const UnitOfMeasure &defaultAngularUnit);
    void consumeParameters(WKTNodeNNPtr node, bool isAbridged,
                           std::vector<OperationParameterNNPtr> &parameters,
                           std::vector<ParameterValueNNPtr> &values,
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
    VerticalReferenceFrameNNPtr buildVerticalReferenceFrame(WKTNodeNNPtr node);
    TemporalDatumNNPtr buildTemporalDatum(WKTNodeNNPtr node);
    CRSNNPtr buildVerticalCRS(WKTNodeNNPtr node);
    CompoundCRSNNPtr buildCompoundCRS(WKTNodeNNPtr node);
    BoundCRSNNPtr buildBoundCRS(WKTNodeNNPtr node);
    TemporalCRSNNPtr buildTemporalCRS(WKTNodeNNPtr node);
    CRSPtr buildCRS(WKTNodeNNPtr node);
    CoordinateOperationNNPtr buildCoordinateOperation(WKTNodeNNPtr node);
    ConcatenatedOperationNNPtr buildConcatenatedOperation(WKTNodeNNPtr node);
};

// ---------------------------------------------------------------------------

WKTParser::WKTParser() : d(internal::make_unique<Private>()) {}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
WKTParser::~WKTParser() = default;
//! @endcond

// ---------------------------------------------------------------------------

/** \brief Set whether parsing should be done in strict mode.
 */
WKTParser &WKTParser::setStrict(bool strict) {
    d->strict_ = strict;
    return *this;
}

// ---------------------------------------------------------------------------

/** \brief Return the list of warnings found during parsing.
 *
 * \note The list might be non-empty only is setStrict(false) has been called.
 */
std::vector<std::string> WKTParser::warningList() const {
    return d->warningList_;
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
void WKTParser::Private::emitRecoverableAssertion(const std::string &errorMsg) {
    if (strict_) {
        throw ParsingException(errorMsg);
    } else {
        warningList_.push_back(errorMsg);
    }
}

// ---------------------------------------------------------------------------

static double asDouble(const std::string &val) { return c_locale_stod(val); }

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
                identifiers->add(NN_CHECK_ASSERT(id));
            }
        }
    }
    if (!identifiers->empty()) {
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
                        throw ParsingException("not 4 double values in " +
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
    if ((type != UnitOfMeasure::Type::TIME && children.size() < 2) ||
        (type == UnitOfMeasure::Type::TIME && children.size() < 1)) {
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
            emitRecoverableAssertion("not enough children in " +
                                     idNode->value() + " node");
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
            stripQuotes(children[0]->value()),
            children.size() >= 2 ? asDouble(children[1]->value()) : 0.0, type,
            hasValidNode ? stripQuotes(idNode->children()[0]->value())
                         : std::string(),
            hasValidNode ? stripQuotes(idNode->children()[1]->value())
                         : std::string());
    } catch (const std::exception &e) {
        throw ParsingException(std::string("buildUnit: ") + e.what());
    }
}

// ---------------------------------------------------------------------------

// node here is a parent node, not a UNIT/LENGTHUNIT/ANGLEUNIT/TIMEUNIT node
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

    unitNode = node->lookForChild(WKTConstants::TIMEUNIT);
    if (unitNode) {
        return buildUnit(NN_CHECK_ASSERT(unitNode), UnitOfMeasure::Type::TIME);
    }

    unitNode = node->lookForChild(WKTConstants::UNIT);
    if (unitNode) {
        return buildUnit(NN_CHECK_ASSERT(unitNode), type);
    }

    return UnitOfMeasure::NONE;
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
        if (unit == UnitOfMeasure::NONE) {
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
    if (unit == UnitOfMeasure::NONE) {
        unit = defaultAngularUnit;
        if (unit == UnitOfMeasure::NONE) {
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

optional<std::string> WKTParser::Private::getAnchor(WKTNodeNNPtr node) {

    auto anchorNode = node->lookForChild(WKTConstants::ANCHOR);
    optional<std::string> anchor;
    if (anchorNode && anchorNode->children().size() == 1) {
        anchor = stripQuotes(anchorNode->children()[0]->value());
    }
    return anchor;
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

    auto TOWGS84Node = node->lookForChild(WKTConstants::TOWGS84);
    if (TOWGS84Node) {
        if (TOWGS84Node->children().size() == 7) {
            try {
                for (const auto &child : TOWGS84Node->children()) {
                    toWGS84Parameters_.push_back(asDouble(child->value()));
                }
            } catch (const std::exception &) {
                throw ParsingException("Invalid TOWGS84 node");
            }
        } else {
            throw ParsingException("Invalid TOWGS84 node");
        }
    }
    auto extensionNode = node->lookForChild(WKTConstants::EXTENSION);
    if (extensionNode && extensionNode->children().size() == 2 &&
        ci_equal(stripQuotes(extensionNode->children()[0]->value()),
                 "PROJ4_GRIDS")) {
        datumPROJ4Grids_ = stripQuotes(extensionNode->children()[1]->value());
    }

    auto properties = buildProperties(node);
    auto name = stripQuotes(node->children()[0]->value());
    if (name == "WGS_1984") {
        properties.set(
            IdentifiedObject::NAME_KEY,
            *(GeodeticReferenceFrame::EPSG_6326->name()->description()));
    }
    return GeodeticReferenceFrame::create(properties, ellipsoid,
                                          getAnchor(node), primeMeridian);
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
    std::string abbreviation;
    if (sepPos != std::string::npos && axisDesignation.back() == ')') {
        axisName = CoordinateSystemAxis::normalizeAxisName(
            axisDesignation.substr(0, sepPos));
        abbreviation = axisDesignation.substr(sepPos + 2);
        abbreviation.resize(abbreviation.size() - 1);
    } else if (!axisDesignation.empty() && axisDesignation[0] == '(' &&
               axisDesignation.back() == ')') {
        abbreviation = axisDesignation.substr(1, axisDesignation.size() - 2);
        if (abbreviation == AxisAbbreviation::E) {
            axisName = AxisName::Easting;
        } else if (abbreviation == AxisAbbreviation::N) {
            axisName = AxisName::Northing;
        } else if (abbreviation == AxisAbbreviation::lat) {
            axisName = AxisName::Latitude;
        } else if (abbreviation == AxisAbbreviation::lon) {
            axisName = AxisName::Longitude;
        }
    } else {
        axisName = CoordinateSystemAxis::normalizeAxisName(axisDesignation);
        if (axisName == AxisName::Latitude) {
            abbreviation = AxisAbbreviation::lat;
        } else if (axisName == AxisName::Longitude) {
            abbreviation = AxisAbbreviation::lon;
        } else if (axisName == AxisName::Ellipsoidal_height) {
            abbreviation = AxisAbbreviation::h;
        }
    }
    std::string dirString = children[1]->value();
    const AxisDirection *direction = AxisDirection::valueOf(dirString);

    // WKT2, geocentric CS: axis names are omitted
    if (direction == &AxisDirection::GEOCENTRIC_X &&
        abbreviation == AxisAbbreviation::X && axisName.empty()) {
        axisName = AxisName::Geocentric_X;
    } else if (direction == &AxisDirection::GEOCENTRIC_Y &&
               abbreviation == AxisAbbreviation::Y && axisName.empty()) {
        axisName = AxisName::Geocentric_Y;
    } else if (direction == &AxisDirection::GEOCENTRIC_Z &&
               abbreviation == AxisAbbreviation::Z && axisName.empty()) {
        axisName = AxisName::Geocentric_Z;
    }

    // WKT1
    if (!direction && isGeocentric && axisName == AxisName::Geocentric_X) {
        abbreviation = AxisAbbreviation::X;
        direction = &AxisDirection::GEOCENTRIC_X;
    } else if (!direction && isGeocentric &&
               axisName == AxisName::Geocentric_Y) {
        abbreviation = AxisAbbreviation::Y;
        direction = &AxisDirection::GEOCENTRIC_Y;
    } else if (isGeocentric && axisName == AxisName::Geocentric_Z &&
               dirString == AxisDirectionWKT1::NORTH.toString()) {
        abbreviation = AxisAbbreviation::Z;
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
    if (unit == UnitOfMeasure::NONE) {
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
        abbreviation, *direction, unit, meridian);
}

// ---------------------------------------------------------------------------

CoordinateSystemNNPtr
WKTParser::Private::buildCS(WKTNodePtr node, /* maybe null */
                            WKTNodeNNPtr parentNode,
                            const UnitOfMeasure &defaultAngularUnit) {
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
                auto unit =
                    buildUnitInSubNode(parentNode, UnitOfMeasure::Type::LINEAR);
                if (unit == UnitOfMeasure::NONE) {
                    throw ParsingException("buildCS: missing UNIT");
                }
                return CartesianCS::createGeocentric(unit);
            }
        } else if (ci_equal(parentNode->value(), WKTConstants::GEOGCS)) {
            csType = "Ellipsoidal";
            if (axisCount == 0) {
                // Missing axis with GEOGCS ? Presumably Long/Lat order implied
                auto unit = buildUnitInSubNode(parentNode,
                                               UnitOfMeasure::Type::ANGULAR);
                if (unit == UnitOfMeasure::NONE) {
                    throw ParsingException("buildCS: missing UNIT");
                }
                return EllipsoidalCS::create(
                    PropertyMap(),
                    CoordinateSystemAxis::create(
                        PropertyMap().set(IdentifiedObject::NAME_KEY,
                                          AxisName::Longitude),
                        AxisAbbreviation::lon, AxisDirection::EAST, unit),
                    CoordinateSystemAxis::create(
                        PropertyMap().set(IdentifiedObject::NAME_KEY,
                                          AxisName::Latitude),
                        AxisAbbreviation::lat, AxisDirection::NORTH, unit));
            }
        } else if (ci_equal(parentNode->value(), WKTConstants::BASEGEODCRS) ||
                   ci_equal(parentNode->value(), WKTConstants::BASEGEOGCRS)) {
            csType = "Ellipsoidal";
            if (axisCount == 0) {
                auto unit = buildUnitInSubNode(parentNode,
                                               UnitOfMeasure::Type::ANGULAR);
                if (unit == UnitOfMeasure::NONE) {
                    unit = defaultAngularUnit;
                }
                return EllipsoidalCS::createLatitudeLongitude(unit);
            }
        } else if (ci_equal(parentNode->value(), WKTConstants::PROJCS)) {
            csType = "Cartesian";
            if (axisCount == 0) {
                auto unit =
                    buildUnitInSubNode(parentNode, UnitOfMeasure::Type::LINEAR);
                if (unit == UnitOfMeasure::NONE) {
                    throw ParsingException("buildCS: missing UNIT");
                }
                return CartesianCS::createEastingNorthing(unit);
            }
        } else if (ci_equal(parentNode->value(), WKTConstants::VERT_CS)) {
            csType = "vertical";
            if (axisCount == 0) {
                auto unit =
                    buildUnitInSubNode(parentNode, UnitOfMeasure::Type::LINEAR);
                if (unit == UnitOfMeasure::NONE) {
                    throw ParsingException("buildCS: missing UNIT");
                }
                return VerticalCS::createGravityRelatedHeight(unit);
            }
        } else {
            // Shouldn't happen normally
            throw ParsingException(
                std::string("buildCS: unexpected parent node: ") +
                parentNode->value());
        }
    }

    if (axisCount != 1 && axisCount != 2 && axisCount != 3) {
        throw ParsingException(std::string("buildCS: invalid CS axis count"));
    }
    if (numberOfAxis != axisCount) {
        throw ParsingException("buildCS: declared number of axis by CS node "
                               "and number of AXIS are inconsistent");
    }

    UnitOfMeasure unit = buildUnitInSubNode(
        parentNode,
        ci_equal(csType, "ellipsoidal")
            ? UnitOfMeasure::Type::ANGULAR
            : ci_equal(csType, "Cartesian") || ci_equal(csType, "vertical")
                  ? UnitOfMeasure::Type::LINEAR
                  : (ci_equal(csType, "temporal") ||
                     ci_equal(csType, "TemporalDateTime") ||
                     ci_equal(csType, "TemporalCount") ||
                     ci_equal(csType, "TemporalMeasure"))
                        ? UnitOfMeasure::Type::TIME
                        : UnitOfMeasure::Type::UNKNOWN);

    std::vector<CoordinateSystemAxisNNPtr> axisList;
    for (int i = 0; i < axisCount; i++) {
        axisList.emplace_back(buildAxis(
            NN_CHECK_ASSERT(parentNode->lookForChild(WKTConstants::AXIS, i)),
            unit, isGeocentric, i + 1));
    };

    PropertyMap csMap =
        node ? buildProperties(NN_CHECK_ASSERT(node)) : PropertyMap();
    if (ci_equal(csType, "ellipsoidal")) {
        if (axisCount == 2) {
            return EllipsoidalCS::create(csMap, axisList[0], axisList[1]);
        } else if (axisCount == 3) {
            return EllipsoidalCS::create(csMap, axisList[0], axisList[1],
                                         axisList[2]);
        } else {
            throw ParsingException("buildCS: invalid CS axis count for " +
                                   csType);
        }
    } else if (ci_equal(csType, "Cartesian")) {
        if (axisCount == 2) {
            return CartesianCS::create(csMap, axisList[0], axisList[1]);
        } else if (axisCount == 3) {
            return CartesianCS::create(csMap, axisList[0], axisList[1],
                                       axisList[2]);
        } else {
            throw ParsingException("buildCS: invalid CS axis count for " +
                                   csType);
        }
    } else if (ci_equal(csType, "vertical")) {
        if (axisCount == 1) {
            return VerticalCS::create(csMap, axisList[0]);
        } else {
            throw ParsingException("buildCS: invalid CS axis count for " +
                                   csType);
        }
    } else if (ci_equal(csType, "spherical")) {
        if (axisCount == 3) {
            return SphericalCS::create(csMap, axisList[0], axisList[1],
                                       axisList[2]);
        } else {
            throw ParsingException("buildCS: invalid CS axis count for " +
                                   csType);
        }
    } else if (ci_equal(csType, "temporal")) { // WKT2-2015
        if (axisCount == 1) {
            return DateTimeTemporalCS::create(
                csMap, axisList[0]); // FIXME: there are 3 possible subtypes of
                                     // TemporalCS
        } else {
            throw ParsingException("buildCS: invalid CS axis count for " +
                                   csType);
        }
    } else if (ci_equal(csType, "TemporalDateTime")) { // WKT2-2018
        if (axisCount == 1) {
            return DateTimeTemporalCS::create(csMap, axisList[0]);
        } else {
            throw ParsingException("buildCS: invalid CS axis count for " +
                                   csType);
        }
    } else if (ci_equal(csType, "TemporalCount")) { // WKT2-2018
        if (axisCount == 1) {
            return TemporalCountCS::create(csMap, axisList[0]);
        } else {
            throw ParsingException("buildCS: invalid CS axis count for " +
                                   csType);
        }
    } else if (ci_equal(csType, "TemporalMeasure")) { // WKT2-2018
        if (axisCount == 1) {
            return TemporalMeasureCS::create(csMap, axisList[0]);
        } else {
            throw ParsingException("buildCS: invalid CS axis count for " +
                                   csType);
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
        // PRIMEM is required in WKT1
        if (ci_equal(node->value(), WKTConstants::GEOGCS) ||
            ci_equal(node->value(), WKTConstants::GEOCCS)) {
            emitRecoverableAssertion(node->value() +
                                     " should have a PRIMEM node");
        } else {
            primeMeridianNode = node->lookForChild(WKTConstants::PRIMEMERIDIAN);
        }
    }

    auto angularUnit =
        buildUnitInSubNode(node, ci_equal(node->value(), WKTConstants::GEOGCS)
                                     ? UnitOfMeasure::Type::ANGULAR
                                     : UnitOfMeasure::Type::UNKNOWN);
    if (angularUnit.type() != UnitOfMeasure::Type::ANGULAR) {
        angularUnit = UnitOfMeasure::NONE;
    }

    auto primeMeridian =
        primeMeridianNode ? buildPrimeMeridian(
                                NN_CHECK_ASSERT(primeMeridianNode), angularUnit)
                          : PrimeMeridian::GREENWICH;
    if (angularUnit == UnitOfMeasure::NONE) {
        angularUnit = primeMeridian->longitude().unit();
    }

    auto datum =
        buildGeodeticReferenceFrame(NN_CHECK_ASSERT(datumNode), primeMeridian);
    auto cs = buildCS(csNode, node, angularUnit);
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
                               cs->getWKT2Type(WKTFormatter::create()));
    }

    auto cartesianCS = nn_dynamic_pointer_cast<CartesianCS>(cs);
    if (cartesianCS) {
        if (cartesianCS->axisList().size() != 3) {
            throw ParsingException(
                "Cartesian CS for a GeodeticCRS should have 3 axis");
        }
        return GeodeticCRS::create(buildProperties(node), datum,
                                   NN_CHECK_ASSERT(cartesianCS));
    }

    auto sphericalCS = nn_dynamic_pointer_cast<SphericalCS>(cs);
    if (sphericalCS) {
        return GeodeticCRS::create(buildProperties(node), datum,
                                   NN_CHECK_ASSERT(sphericalCS));
    }

    throw ParsingException("unhandled CS type: " +
                           cs->getWKT2Type(WKTFormatter::create()));
}

// ---------------------------------------------------------------------------

CRSNNPtr WKTParser::Private::buildDerivedGeodeticCRS(WKTNodeNNPtr node) {

    auto baseGeodCRSNode = node->lookForChild(WKTConstants::BASEGEODCRS);
    if (!baseGeodCRSNode) {
        baseGeodCRSNode = node->lookForChild(WKTConstants::BASEGEOGCRS);
    }
    assert(baseGeodCRSNode !=
           nullptr); // given the constraints enforced on calling code path

    auto baseGeodCRS = buildGeodeticCRS(NN_CHECK_ASSERT(baseGeodCRSNode));

    auto derivingConversionNode =
        node->lookForChild(WKTConstants::DERIVINGCONVERSION);
    if (!derivingConversionNode) {
        throw ParsingException("Missing DERIVINGCONVERSION node");
    }
    auto derivingConversion =
        buildConversion(NN_CHECK_ASSERT(derivingConversionNode),
                        UnitOfMeasure::NONE, UnitOfMeasure::NONE);

    auto csNode = node->lookForChild(WKTConstants::CS);
    if (!csNode) {
        throw ParsingException("Missing CS node");
    }
    auto cs = buildCS(csNode, node, UnitOfMeasure::NONE);

    auto ellipsoidalCS = nn_dynamic_pointer_cast<EllipsoidalCS>(cs);
    if (ellipsoidalCS) {
        return DerivedGeographicCRS::create(buildProperties(node), baseGeodCRS,
                                            derivingConversion,
                                            NN_CHECK_ASSERT(ellipsoidalCS));
    } else if (ci_equal(node->value(), WKTConstants::GEOGCRS)) {
        // This is a WKT2-2018 GeographicCRS. An ellipsoidal CS is expected
        throw ParsingException("ellipsoidal CS expected, but found " +
                               cs->getWKT2Type(WKTFormatter::create()));
    }

    auto cartesianCS = nn_dynamic_pointer_cast<CartesianCS>(cs);
    if (cartesianCS) {
        if (cartesianCS->axisList().size() != 3) {
            throw ParsingException(
                "Cartesian CS for a GeodeticCRS should have 3 axis");
        }
        return DerivedGeodeticCRS::create(buildProperties(node), baseGeodCRS,
                                          derivingConversion,
                                          NN_CHECK_ASSERT(cartesianCS));
    }

    auto sphericalCS = nn_dynamic_pointer_cast<SphericalCS>(cs);
    if (sphericalCS) {
        return DerivedGeodeticCRS::create(buildProperties(node), baseGeodCRS,
                                          derivingConversion,
                                          NN_CHECK_ASSERT(sphericalCS));
    }

    throw ParsingException("unhandled CS type: " +
                           cs->getWKT2Type(WKTFormatter::create()));
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

void WKTParser::Private::consumeParameters(
    WKTNodeNNPtr node, bool isAbridged,
    std::vector<OperationParameterNNPtr> &parameters,
    std::vector<ParameterValueNNPtr> &values,
    const UnitOfMeasure &defaultLinearUnit,
    const UnitOfMeasure &defaultAngularUnit) {
    for (const auto &childNode : node->children()) {
        if (ci_equal(childNode->value(), WKTConstants::PARAMETER)) {
            if (childNode->children().size() < 2) {
                throw ParsingException("not enough children in " +
                                       childNode->value() + " node");
            }
            parameters.push_back(
                OperationParameter::create(buildProperties(childNode)));
            auto paramValue = childNode->children()[1]->value();
            if (!paramValue.empty() && paramValue[0] == '"') {
                values.push_back(
                    ParameterValue::create(stripQuotes(paramValue)));
            } else {
                try {
                    double val = asDouble(paramValue);
                    auto unit = buildUnitInSubNode(childNode);
                    if (unit == UnitOfMeasure::NONE) {
                        auto paramName = childNode->children()[0]->value();
                        unit = guessUnitForParameter(
                            paramName, defaultLinearUnit, defaultAngularUnit);
                    }

                    if (isAbridged) {
                        auto paramName =
                            *(parameters.back()->name()->description());
                        int paramEPSGCode = 0;
                        if (parameters.back()->identifiers().size() == 1 &&
                            ci_equal(*(parameters.back()
                                           ->identifiers()[0]
                                           ->codeSpace()),
                                     Identifier::EPSG)) {
                            paramEPSGCode = ::atoi(parameters.back()
                                                       ->identifiers()[0]
                                                       ->code()
                                                       .c_str());
                        }
                        if (OperationParameterValue::convertFromAbridged(
                                paramName, val, unit, paramEPSGCode)) {
                            parameters.back() = OperationParameter::create(
                                buildProperties(childNode)
                                    .set(Identifier::CODESPACE_KEY,
                                         Identifier::EPSG)
                                    .set(Identifier::CODE_KEY, paramEPSGCode));
                        }
                    }

                    values.push_back(
                        ParameterValue::create(Measure(val, unit)));
                } catch (const std::exception &) {
                    throw ParsingException("unhandled parameter value type : " +
                                           paramValue);
                }
            }
        } else if (ci_equal(childNode->value(), WKTConstants::PARAMETERFILE)) {
            if (childNode->children().size() < 2) {
                throw ParsingException("not enough children in " +
                                       childNode->value() + " node");
            }
            parameters.push_back(
                OperationParameter::create(buildProperties(childNode)));
            auto paramValue = childNode->children()[1]->value();
            values.push_back(
                ParameterValue::createFilename(stripQuotes(paramValue)));
        }
    }
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
    if (methodNode->children().empty()) {
        throw ParsingException("not enough children in " +
                               WKTConstants::METHOD + " node");
    }

    std::vector<OperationParameterNNPtr> parameters;
    std::vector<ParameterValueNNPtr> values;
    consumeParameters(node, false, parameters, values, defaultLinearUnit,
                      defaultAngularUnit);

    return Conversion::create(buildProperties(node),
                              buildProperties(NN_CHECK_ASSERT(methodNode)),
                              parameters, values);
}

// ---------------------------------------------------------------------------

CoordinateOperationNNPtr
WKTParser::Private::buildCoordinateOperation(WKTNodeNNPtr node) {
    auto methodNode = node->lookForChild(WKTConstants::METHOD);
    if (!methodNode) {
        throw ParsingException("Missing METHOD node");
    }
    if (methodNode->children().empty()) {
        throw ParsingException("not enough children in " +
                               WKTConstants::METHOD + " node");
    }

    auto sourceCRSNode = node->lookForChild(WKTConstants::SOURCECRS);
    if (!sourceCRSNode || sourceCRSNode->children().size() != 1) {
        throw ParsingException("Missing SOURCECRS node");
    }
    auto sourceCRS = buildCRS(sourceCRSNode->children()[0]);
    if (!sourceCRS) {
        throw ParsingException("Invalid content in SOURCECRS node");
    }

    auto targetCRSNode = node->lookForChild(WKTConstants::TARGETCRS);
    if (!targetCRSNode || targetCRSNode->children().size() != 1) {
        throw ParsingException("Missing TARGETCRS node");
    }
    auto targetCRS = buildCRS(targetCRSNode->children()[0]);
    if (!targetCRS) {
        throw ParsingException("Invalid content in TARGETCRS node");
    }

    auto interpolationCRSNode =
        node->lookForChild(WKTConstants::INTERPOLATIONCRS);
    CRSPtr interpolationCRS;
    if (interpolationCRSNode && interpolationCRSNode->children().size() == 1) {
        interpolationCRS = buildCRS(interpolationCRSNode->children()[0]);
    }

    std::vector<OperationParameterNNPtr> parameters;
    std::vector<ParameterValueNNPtr> values;
    auto defaultLinearUnit = UnitOfMeasure::NONE;
    auto defaultAngularUnit = UnitOfMeasure::NONE;
    consumeParameters(node, false, parameters, values, defaultLinearUnit,
                      defaultAngularUnit);

    std::vector<PositionalAccuracyNNPtr> accuracies;
    auto accuracyNode = node->lookForChild(WKTConstants::OPERATIONACCURACY);
    if (accuracyNode && accuracyNode->children().size() == 1) {
        accuracies.push_back(PositionalAccuracy::create(
            stripQuotes(accuracyNode->children()[0]->value())));
    }

    return util::nn_static_pointer_cast<CoordinateOperation>(
        Transformation::create(buildProperties(node),
                               NN_CHECK_ASSERT(sourceCRS),
                               NN_CHECK_ASSERT(targetCRS), interpolationCRS,
                               buildProperties(NN_CHECK_ASSERT(methodNode)),
                               parameters, values, accuracies));
}

// ---------------------------------------------------------------------------

ConcatenatedOperationNNPtr
WKTParser::Private::buildConcatenatedOperation(WKTNodeNNPtr node) {
    std::vector<CoordinateOperationNNPtr> operations;
    for (const auto &childNode : node->children()) {
        if (ci_equal(childNode->value(), WKTConstants::STEP)) {
            if (childNode->children().size() != 1) {
                throw ParsingException("Invalid content in STEP node");
            }
            auto op = nn_dynamic_pointer_cast<CoordinateOperation>(
                build(childNode->children()[0]));
            if (!op) {
                throw ParsingException("Invalid content in STEP node");
            }
            operations.emplace_back(NN_CHECK_ASSERT(op));
        }
    }
    try {
        return ConcatenatedOperation::create(
            buildProperties(node), operations,
            std::vector<PositionalAccuracyNNPtr>());
    } catch (const InvalidOperation &e) {
        throw ParsingException(
            std::string("Cannot build concatenated operation: ") + e.what());
    }
}

// ---------------------------------------------------------------------------

ConversionNNPtr
WKTParser::Private::buildProjection(WKTNodeNNPtr projCRSNode,
                                    WKTNodeNNPtr projectionNode,
                                    const UnitOfMeasure &defaultLinearUnit,
                                    const UnitOfMeasure &defaultAngularUnit) {
    if (projectionNode->children().empty()) {
        throw ParsingException("not enough children in " +
                               WKTConstants::PROJECTION + " node");
    }
    const std::string wkt1ProjectionName =
        stripQuotes(projectionNode->children()[0]->value());

    std::vector<OperationParameterNNPtr> parameters;
    std::vector<ParameterValueNNPtr> values;
    bool tryToIdentifyWKT1Method = true;

    if (metadata::Identifier::isEquivalentName(wkt1ProjectionName,
                                               "Mercator_1SP") &&
        projCRSNode->countChildrenOfName("center_latitude") == 0) {

        // Hack to detect the hacky way of encodign webmerc in GDAL WKT1
        // with a EXTENSION["PROJ", "+proj=merc +a=6378137 +b=6378137
        // +lat_ts=0.0 +lon_0=0.0 +x_0=0.0 +y_0=0 +k=1.0 +units=m
        // +nadgrids=@null +wktext +no_defs"] node
        auto extensionNode = projCRSNode->lookForChild(WKTConstants::EXTENSION);
        if (extensionNode && extensionNode->children().size() == 2 &&
            ci_equal(stripQuotes(extensionNode->children()[0]->value()),
                     "PROJ4")) {
            std::string projString =
                stripQuotes(extensionNode->children()[1]->value());
            if (projString.find("+proj=merc") != std::string::npos &&
                projString.find("+a=6378137") != std::string::npos &&
                projString.find("+b=6378137") != std::string::npos &&
                projString.find("+lon_0=0") != std::string::npos &&
                projString.find("+x_0=0") != std::string::npos &&
                projString.find("+y_0=0") != std::string::npos &&
                projString.find("+nadgrids=@null") != std::string::npos &&
                (projString.find("+lat_ts=") == std::string::npos ||
                 projString.find("+lat_ts=0") != std::string::npos) &&
                (projString.find("+k=") == std::string::npos ||
                 projString.find("+k=1") != std::string::npos) &&
                (projString.find("+units=") == std::string::npos ||
                 projString.find("+units=m") != std::string::npos)) {

                return Conversion::createPopularVisualisationPseudoMercator(
                    PropertyMap().set(IdentifiedObject::NAME_KEY, "unnamed"),
                    Angle(0), Angle(0), Length(0), Length(0));
            }
        } else {
            // The latitude of origin, which should always be zero, is missing
            // in GDAL WKT1, but provisionned in the EPSG Mercator_1SP
            // definition,
            // so add it manually.
            PropertyMap propertiesParameter;
            propertiesParameter.set(IdentifiedObject::NAME_KEY,
                                    "Latitude of natural origin");
            propertiesParameter.set(Identifier::CODE_KEY, 8801);
            propertiesParameter.set(Identifier::CODESPACE_KEY,
                                    Identifier::EPSG);
            parameters.push_back(
                OperationParameter::create(propertiesParameter));
            values.push_back(
                ParameterValue::create(Measure(0, UnitOfMeasure::DEGREE)));
        }
    } else if (metadata::Identifier::isEquivalentName(wkt1ProjectionName,
                                                      "Polar_Stereographic")) {
        std::map<std::string, Measure> mapParameters;
        for (const auto &childNode : projCRSNode->children()) {
            if (ci_equal(childNode->value(), WKTConstants::PARAMETER) &&
                childNode->children().size() == 2) {
                const std::string wkt1ParameterName(
                    stripQuotes(childNode->children()[0]->value()));
                auto paramValue = childNode->children()[1]->value();
                try {
                    double val = asDouble(paramValue);
                    auto unit = guessUnitForParameter(wkt1ParameterName,
                                                      defaultLinearUnit,
                                                      defaultAngularUnit);
                    mapParameters[wkt1ParameterName] = Measure(val, unit);
                } catch (const std::exception &) {
                }
            }
        }

        Measure latitudeOfOrigin = mapParameters["latitude_of_origin"];
        Measure centralMeridian = mapParameters["central_meridian"];
        Measure scaleFactor = mapParameters["scale_factor"];
        if (scaleFactor.unit() == UnitOfMeasure::NONE)
            scaleFactor = Measure(1.0, UnitOfMeasure::SCALE_UNITY);
        Measure falseEasting = mapParameters["false_easting"];
        Measure falseNorthing = mapParameters["false_northing"];
        if (latitudeOfOrigin.unit() != UnitOfMeasure::NONE &&
            std::fabs(
                std::fabs(latitudeOfOrigin.convertToUnit(UnitOfMeasure::DEGREE)
                              .value()) -
                90.0) < 1e-10) {
            return Conversion::createPolarStereographicVariantA(
                PropertyMap().set(IdentifiedObject::NAME_KEY, "unnamed"),
                Angle(latitudeOfOrigin.value(), latitudeOfOrigin.unit()),
                Angle(centralMeridian.value(), centralMeridian.unit()),
                Scale(scaleFactor.value(), scaleFactor.unit()),
                Length(falseEasting.value(), falseEasting.unit()),
                Length(falseNorthing.value(), falseNorthing.unit()));
        } else if (latitudeOfOrigin.unit() != UnitOfMeasure::NONE &&
                   scaleFactor.getSIValue() == 1.0) {
            return Conversion::createPolarStereographicVariantB(
                PropertyMap().set(IdentifiedObject::NAME_KEY, "unnamed"),
                Angle(latitudeOfOrigin.value(), latitudeOfOrigin.unit()),
                Angle(centralMeridian.value(), centralMeridian.unit()),
                Length(falseEasting.value(), falseEasting.unit()),
                Length(falseNorthing.value(), falseNorthing.unit()));
        }
        tryToIdentifyWKT1Method = false;
    }

    std::string projectionName(wkt1ProjectionName);
    const MethodMapping *mapping =
        tryToIdentifyWKT1Method ? getMappingFromWKT1(projectionName) : nullptr;
    PropertyMap propertiesMethod;
    if (mapping) {
        projectionName = mapping->wkt2_name;
        if (mapping->epsg_code != 0) {
            propertiesMethod.set(Identifier::CODE_KEY, mapping->epsg_code);
            propertiesMethod.set(Identifier::CODESPACE_KEY, Identifier::EPSG);
        }
    }
    propertiesMethod.set(IdentifiedObject::NAME_KEY, projectionName);

    for (const auto &childNode : projCRSNode->children()) {
        if (ci_equal(childNode->value(), WKTConstants::PARAMETER)) {
            if (childNode->children().size() < 2) {
                throw ParsingException("not enough children in " +
                                       WKTConstants::PARAMETER + " node");
            }
            PropertyMap propertiesParameter;
            const std::string wkt1ParameterName(
                stripQuotes(childNode->children()[0]->value()));
            std::string parameterName(wkt1ParameterName);
            const auto *paramMapping =
                mapping ? getMappingFromWKT1(mapping, parameterName) : nullptr;
            if (paramMapping) {
                parameterName = paramMapping->wkt2_name;
                if (paramMapping->epsg_code != 0) {
                    propertiesParameter.set(Identifier::CODE_KEY,
                                            paramMapping->epsg_code);
                    propertiesParameter.set(Identifier::CODESPACE_KEY,
                                            Identifier::EPSG);
                }
            }
            propertiesParameter.set(IdentifiedObject::NAME_KEY, parameterName);
            parameters.push_back(
                OperationParameter::create(propertiesParameter));
            auto paramValue = childNode->children()[1]->value();
            try {
                double val = asDouble(paramValue);
                auto unit = guessUnitForParameter(
                    wkt1ParameterName, defaultLinearUnit, defaultAngularUnit);
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
    auto angularUnit = baseGeodCRS->coordinateSystem()->axisList()[0]->unit();

    auto conversion =
        conversionNode ? buildConversion(NN_CHECK_ASSERT(conversionNode),
                                         linearUnit, angularUnit)
                       : buildProjection(node, NN_CHECK_ASSERT(projectionNode),
                                         linearUnit, angularUnit);

    auto csNode = node->lookForChild(WKTConstants::CS);
    if (!csNode && !ci_equal(node->value(), WKTConstants::PROJCS)) {
        throw ParsingException("Missing CS node");
    }
    auto cs = buildCS(csNode, node, UnitOfMeasure::NONE);
    auto cartesianCS = nn_dynamic_pointer_cast<CartesianCS>(cs);
    if (!cartesianCS) {
        throw ParsingException("CS node is not of type Cartesian");
    }

    return ProjectedCRS::create(buildProperties(node), baseGeodCRS, conversion,
                                NN_CHECK_ASSERT(cartesianCS));
}

// ---------------------------------------------------------------------------

VerticalReferenceFrameNNPtr
WKTParser::Private::buildVerticalReferenceFrame(WKTNodeNNPtr node) {
    // WKT1 VERT_DATUM has a datum type after the datum name that we ignore.
    return VerticalReferenceFrame::create(buildProperties(node),
                                          getAnchor(node));
}

// ---------------------------------------------------------------------------

TemporalDatumNNPtr WKTParser::Private::buildTemporalDatum(WKTNodeNNPtr node) {
    auto calendarNode = node->lookForChild(WKTConstants::CALENDAR);
    std::string calendar = TemporalDatum::CALENDAR_PROLEPTIC_GREGORIAN;
    if (calendarNode && calendarNode->children().size() == 1) {
        calendar = stripQuotes(calendarNode->children()[0]->value());
    }

    auto timeOriginNode = node->lookForChild(WKTConstants::TIMEORIGIN);
    std::string originStr;
    if (timeOriginNode && timeOriginNode->children().size() == 1) {
        originStr = stripQuotes(timeOriginNode->children()[0]->value());
    }
    auto origin = DateTime::create(originStr);
    return TemporalDatum::create(buildProperties(node), origin, calendar);
}

// ---------------------------------------------------------------------------

CRSNNPtr WKTParser::Private::buildVerticalCRS(WKTNodeNNPtr node) {
    auto datumNode = node->lookForChild(WKTConstants::VDATUM);
    if (!datumNode) {
        datumNode = node->lookForChild(WKTConstants::VERT_DATUM);
        if (!datumNode) {
            datumNode = node->lookForChild(WKTConstants::VERTICALDATUM);
            if (!datumNode) {
                throw ParsingException("Missing VDATUM node");
            }
        }
    }
    auto datum = buildVerticalReferenceFrame(NN_CHECK_ASSERT(datumNode));

    auto csNode = node->lookForChild(WKTConstants::CS);
    if (!csNode && !ci_equal(node->value(), WKTConstants::VERT_CS)) {
        throw ParsingException("Missing CS node");
    }
    auto cs = buildCS(csNode, node, UnitOfMeasure::NONE);
    auto verticalCS = nn_dynamic_pointer_cast<VerticalCS>(cs);
    if (!verticalCS) {
        throw ParsingException("CS node is not of type vertical");
    }

    auto crs = nn_static_pointer_cast<CRS>(VerticalCRS::create(
        buildProperties(node), datum, NN_CHECK_ASSERT(verticalCS)));

    auto extensionNode = datumNode->lookForChild(WKTConstants::EXTENSION);
    if (extensionNode && extensionNode->children().size() == 2 &&
        ci_equal(stripQuotes(extensionNode->children()[0]->value()),
                 "PROJ4_GRIDS")) {
        std::string transformationName = *(crs->name()->description());
        if (!ends_with(transformationName, " height")) {
            transformationName += " height";
        }
        transformationName += " to WGS84 ellipsoidal height";
        auto transformation =
            Transformation::createGravityRelatedHeightToGeographic3D(
                PropertyMap().set(IdentifiedObject::NAME_KEY,
                                  transformationName),
                crs, GeographicCRS::EPSG_4979,
                stripQuotes(extensionNode->children()[1]->value()),
                std::vector<PositionalAccuracyNNPtr>());
        return nn_static_pointer_cast<CRS>(
            BoundCRS::create(crs, GeographicCRS::EPSG_4979, transformation));
    }

    return crs;
}

// ---------------------------------------------------------------------------

CompoundCRSNNPtr WKTParser::Private::buildCompoundCRS(WKTNodeNNPtr node) {
    std::vector<CRSNNPtr> components;
    for (const auto &child : node->children()) {
        auto crs = buildCRS(child);
        if (crs) {
            components.push_back(NN_CHECK_ASSERT(crs));
        }
    }
    return CompoundCRS::create(buildProperties(node), components);
}

// ---------------------------------------------------------------------------

BoundCRSNNPtr WKTParser::Private::buildBoundCRS(WKTNodeNNPtr node) {
    auto abridgedNode =
        node->lookForChild(WKTConstants::ABRIDGEDTRANSFORMATION);
    if (!abridgedNode) {
        throw ParsingException("Missing ABRIDGEDTRANSFORMATION node");
    }

    auto methodNode = abridgedNode->lookForChild(WKTConstants::METHOD);
    if (!methodNode) {
        throw ParsingException("Missing METHOD node");
    }
    if (methodNode->children().empty()) {
        throw ParsingException("not enough children in " +
                               WKTConstants::METHOD + " node");
    }

    auto sourceCRSNode = node->lookForChild(WKTConstants::SOURCECRS);
    if (!sourceCRSNode || sourceCRSNode->children().size() != 1) {
        throw ParsingException("Missing SOURCECRS node");
    }
    auto sourceCRS = buildCRS(sourceCRSNode->children()[0]);
    if (!sourceCRS) {
        throw ParsingException("Invalid content in SOURCECRS node");
    }

    auto targetCRSNode = node->lookForChild(WKTConstants::TARGETCRS);
    if (!targetCRSNode || targetCRSNode->children().size() != 1) {
        throw ParsingException("Missing TARGETCRS node");
    }
    auto targetCRS = buildCRS(targetCRSNode->children()[0]);
    if (!targetCRS) {
        throw ParsingException("Invalid content in TARGETCRS node");
    }

    std::vector<OperationParameterNNPtr> parameters;
    std::vector<ParameterValueNNPtr> values;
    auto defaultLinearUnit = UnitOfMeasure::NONE;
    auto defaultAngularUnit = UnitOfMeasure::NONE;
    consumeParameters(NN_CHECK_ASSERT(abridgedNode), true, parameters, values,
                      defaultLinearUnit, defaultAngularUnit);

    CRSPtr sourceTransformationCRS;
    if (std::dynamic_pointer_cast<GeographicCRS>(targetCRS)) {
        sourceTransformationCRS = sourceCRS->extractGeographicCRS();
        if (!sourceTransformationCRS) {
            throw ParsingException("Cannot find GeographicCRS in sourceCRS");
        }
    } else {
        sourceTransformationCRS = sourceCRS;
    }

    auto transformation = Transformation::create(
        buildProperties(NN_CHECK_ASSERT(abridgedNode)),
        NN_CHECK_ASSERT(sourceTransformationCRS), NN_CHECK_ASSERT(targetCRS),
        nullptr, buildProperties(NN_CHECK_ASSERT(methodNode)), parameters,
        values, std::vector<PositionalAccuracyNNPtr>());

    return BoundCRS::create(NN_CHECK_ASSERT(sourceCRS),
                            NN_CHECK_ASSERT(targetCRS), transformation);
}

// ---------------------------------------------------------------------------

TemporalCRSNNPtr WKTParser::Private::buildTemporalCRS(WKTNodeNNPtr node) {
    auto datumNode = node->lookForChild(WKTConstants::TDATUM);
    if (!datumNode) {
        datumNode = node->lookForChild(WKTConstants::TIMEDATUM);
        if (!datumNode) {
            throw ParsingException("Missing TDATUM / TIMEDATUM node");
        }
    }
    auto datum = buildTemporalDatum(NN_CHECK_ASSERT(datumNode));

    auto csNode = node->lookForChild(WKTConstants::CS);
    if (!csNode) {
        throw ParsingException("Missing CS node");
    }
    auto cs = buildCS(csNode, node, UnitOfMeasure::NONE);
    auto temporalCS = nn_dynamic_pointer_cast<TemporalCS>(cs);
    if (!temporalCS) {
        throw ParsingException("CS node is not of type temporal");
    }

    return TemporalCRS::create(buildProperties(node), datum,
                               NN_CHECK_ASSERT(temporalCS));
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

CRSPtr WKTParser::Private::buildCRS(WKTNodeNNPtr node) {
    const std::string &name(node->value());

    if (isGeodeticCRS(name)) {
        if (node->lookForChild(WKTConstants::BASEGEOGCRS) ||
            node->lookForChild(WKTConstants::BASEGEODCRS)) {
            return buildDerivedGeodeticCRS(node);
        } else {
            return util::nn_static_pointer_cast<CRS>(buildGeodeticCRS(node));
        }
    }

    if (ci_equal(name, WKTConstants::PROJCS) ||
        ci_equal(name, WKTConstants::PROJCRS) ||
        ci_equal(name, WKTConstants::PROJECTEDCRS)) {
        return util::nn_static_pointer_cast<CRS>(buildProjectedCRS(node));
    }

    if (ci_equal(name, WKTConstants::VERT_CS) ||
        ci_equal(name, WKTConstants::VERTCRS) ||
        ci_equal(name, WKTConstants::VERTICALCRS)) {
        return util::nn_static_pointer_cast<CRS>(buildVerticalCRS(node));
    }

    if (ci_equal(name, WKTConstants::COMPD_CS) ||
        ci_equal(name, WKTConstants::COMPOUNDCRS)) {
        return util::nn_static_pointer_cast<CRS>(buildCompoundCRS(node));
    }

    if (ci_equal(name, WKTConstants::BOUNDCRS)) {
        return util::nn_static_pointer_cast<CRS>(buildBoundCRS(node));
    }

    if (ci_equal(name, WKTConstants::TIMECRS)) {
        return util::nn_static_pointer_cast<CRS>(buildTemporalCRS(node));
    }

    return nullptr;
}

// ---------------------------------------------------------------------------

BaseObjectNNPtr WKTParser::Private::build(WKTNodeNNPtr node) {
    const std::string &name(node->value());

    auto crs = buildCRS(node);
    if (crs) {
        if (!toWGS84Parameters_.empty()) {
            return util::nn_static_pointer_cast<BaseObject>(
                BoundCRS::createFromTOWGS84(NN_CHECK_ASSERT(crs),
                                            toWGS84Parameters_));
        }
        if (!datumPROJ4Grids_.empty()) {
            return util::nn_static_pointer_cast<BaseObject>(
                BoundCRS::createFromNadgrids(NN_CHECK_ASSERT(crs),
                                             datumPROJ4Grids_));
        }
        return util::nn_static_pointer_cast<BaseObject>(NN_CHECK_ASSERT(crs));
    }

    if (ci_equal(name, WKTConstants::DATUM) ||
        ci_equal(name, WKTConstants::GEODETICDATUM) ||
        ci_equal(name, WKTConstants::GEODETICREFERENCEFRAME)) {
        return util::nn_static_pointer_cast<BaseObject>(
            buildGeodeticReferenceFrame(node, PrimeMeridian::GREENWICH));
    }

    if (ci_equal(name, WKTConstants::VDATUM) ||
        ci_equal(name, WKTConstants::VERT_DATUM) ||
        ci_equal(name, WKTConstants::VERTICALDATUM)) {
        return util::nn_static_pointer_cast<BaseObject>(
            buildVerticalReferenceFrame(node));
    }

    if (ci_equal(name, WKTConstants::TDATUM) ||
        ci_equal(name, WKTConstants::TIMEDATUM)) {
        return util::nn_static_pointer_cast<BaseObject>(
            buildTemporalDatum(node));
    }

    if (ci_equal(name, WKTConstants::ELLIPSOID) ||
        ci_equal(name, WKTConstants::SPHEROID)) {
        return util::nn_static_pointer_cast<BaseObject>(buildEllipsoid(node));
    }

    if (ci_equal(name, WKTConstants::COORDINATEOPERATION)) {
        return util::nn_static_pointer_cast<BaseObject>(
            buildCoordinateOperation(node));
    }

    if (ci_equal(name, WKTConstants::CONCATENATEDOPERATION)) {
        return util::nn_static_pointer_cast<BaseObject>(
            buildConcatenatedOperation(node));
    }

    if (ci_equal(name, WKTConstants::ID) ||
        ci_equal(name, WKTConstants::AUTHORITY)) {
        return util::nn_static_pointer_cast<BaseObject>(
            NN_CHECK_ASSERT(buildId(node, false)));
    }

    throw ParsingException("unhandled keyword: " + name);
}
//! @endcond

// ---------------------------------------------------------------------------

/** \brief Instanciate a sub-class of BaseObject from a WKT string.
 * @throw ParsingException
 */
BaseObjectNNPtr WKTParser::createFromWKT(const std::string &wkt) {
    WKTNodeNNPtr root = WKTNode::createFrom(wkt);
    return d->build(root);
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
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

// ---------------------------------------------------------------------------

IPROJStringExportable::~IPROJStringExportable() = default;
//! @endcond

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
struct PROJStringFormatter::Private {
    PROJStringFormatter::Convention convention_ =
        PROJStringFormatter::Convention::PROJ_5;
    std::vector<double> toWGS84Parameters_{};
    std::string vDatumExtension_{};
    std::string hDatumExtension_{};

    struct Step {
        std::string name{};
        bool inverted{false};
        std::vector<std::string> paramValues{};
    };
    std::vector<Step> steps_{};

    struct InversionStackElt {
        size_t startIdx = 0;
        bool currentInversionState = false;
    };
    std::vector<InversionStackElt> inversionStack_{InversionStackElt()};
    bool omitProjLongLatIfPossible_ = false;
    int level_ = 0;

    std::string result_{};

    void appendToResult(const std::string &str);
};
//! @endcond

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
PROJStringFormatter::PROJStringFormatter(Convention conventionIn)
    : d(internal::make_unique<Private>()) {
    d->convention_ = conventionIn;
}
//! @endcond

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
PROJStringFormatter::~PROJStringFormatter() = default;
//! @endcond

// ---------------------------------------------------------------------------

/** \brief Constructs a new formatter.
 *
 * A formatter can be used only once (its internal state is mutated)
 *
 * Its default behaviour can be adjusted with the different setters.
 *
 * @param conventionIn PROJ string flavor. Defaults to Convention::PROJ_5
 * @return new formatter.
 */
PROJStringFormatterNNPtr PROJStringFormatter::create(Convention conventionIn) {
    return PROJStringFormatter::nn_make_shared<PROJStringFormatter>(
        conventionIn);
}

// ---------------------------------------------------------------------------

/** \brief Returns the PROJ string. */
const std::string &PROJStringFormatter::toString() const {
    d->result_.clear();
    if (d->level_) {
        return d->result_;
    }

    bool changeDone;
    do {
        changeDone = false;
        for (size_t i = 1; i < d->steps_.size(); i++) {

            // longlat (or its inverse) with ellipsoid only is a no-op
            // do that only for an internal step
            if (i + 1 < d->steps_.size() && d->steps_[i].name == "longlat" &&
                d->steps_[i].paramValues.size() == 1 &&
                starts_with(d->steps_[i].paramValues[0], "ellps=")) {
                d->steps_.erase(d->steps_.begin() + i);
                changeDone = true;
                break;
            }

            // unitconvert followed by its inverse is a no-op
            if (d->steps_[i].name == "unitconvert" &&
                d->steps_[i - 1].name == "unitconvert" &&
                !d->steps_[i].inverted && !d->steps_[i - 1].inverted &&
                d->steps_[i].paramValues.size() == 2 &&
                d->steps_[i - 1].paramValues.size() == 2 &&
                starts_with(d->steps_[i].paramValues[0], "xy_in=") &&
                starts_with(d->steps_[i - 1].paramValues[0], "xy_in=") &&
                starts_with(d->steps_[i].paramValues[1], "xy_out=") &&
                starts_with(d->steps_[i - 1].paramValues[1], "xy_out=") &&
                d->steps_[i].paramValues[0].substr(strlen("xy_in=")) ==
                    d->steps_[i - 1].paramValues[1].substr(strlen("xy_out=")) &&
                d->steps_[i].paramValues[1].substr(strlen("xy_out=")) ==
                    d->steps_[i - 1].paramValues[0].substr(strlen("xy_in="))) {
                d->steps_.erase(d->steps_.begin() + i - 1,
                                d->steps_.begin() + i + 1);
                changeDone = true;
                break;
            }

            // axisswap order=2,1 followed by itself is a no-op
            if (d->steps_[i].name == "axisswap" &&
                d->steps_[i - 1].name == "axisswap" &&
                d->steps_[i].paramValues.size() == 1 &&
                d->steps_[i - 1].paramValues.size() == 1 &&
                d->steps_[i].paramValues[0] == "order=2,1" &&
                d->steps_[i - 1].paramValues[0] == "order=2,1") {
                d->steps_.erase(d->steps_.begin() + i - 1,
                                d->steps_.begin() + i + 1);
                changeDone = true;
                break;
            }

            // hermert followed by its inverse is a no-op
            try {
                if (d->steps_[i].name == "helmert" &&
                    d->steps_[i - 1].name == "helmert" &&
                    !d->steps_[i].inverted && !d->steps_[i - 1].inverted &&
                    ((d->steps_[i].paramValues.size() == 7 &&
                      d->steps_[i - 1].paramValues.size() == 7) ||
                     (d->steps_[i].paramValues.size() == 8 &&
                      d->steps_[i].paramValues[7] == "transpose" &&
                      d->steps_[i - 1].paramValues.size() == 8 &&
                      d->steps_[i - 1].paramValues[7] == "transpose")) &&
                    starts_with(d->steps_[i].paramValues[0], "x=") &&
                    starts_with(d->steps_[i - 1].paramValues[0], "x=") &&
                    starts_with(d->steps_[i].paramValues[1], "y=") &&
                    starts_with(d->steps_[i - 1].paramValues[1], "y=") &&
                    starts_with(d->steps_[i].paramValues[2], "z=") &&
                    starts_with(d->steps_[i - 1].paramValues[2], "z=") &&
                    starts_with(d->steps_[i].paramValues[3], "rx=") &&
                    starts_with(d->steps_[i - 1].paramValues[3], "rx=") &&
                    starts_with(d->steps_[i].paramValues[4], "ry=") &&
                    starts_with(d->steps_[i - 1].paramValues[4], "ry=") &&
                    starts_with(d->steps_[i].paramValues[5], "rz=") &&
                    starts_with(d->steps_[i - 1].paramValues[5], "rz=") &&
                    starts_with(d->steps_[i].paramValues[6], "s=") &&
                    starts_with(d->steps_[i - 1].paramValues[6], "s=") &&
                    c_locale_stod(
                        d->steps_[i].paramValues[0].substr(strlen("x="))) ==
                        -c_locale_stod(d->steps_[i - 1].paramValues[0].substr(
                            strlen("x="))) &&
                    c_locale_stod(
                        d->steps_[i].paramValues[1].substr(strlen("y="))) ==
                        -c_locale_stod(d->steps_[i - 1].paramValues[1].substr(
                            strlen("y="))) &&
                    c_locale_stod(
                        d->steps_[i].paramValues[2].substr(strlen("z="))) ==
                        -c_locale_stod(d->steps_[i - 1].paramValues[2].substr(
                            strlen("z="))) &&
                    c_locale_stod(
                        d->steps_[i].paramValues[3].substr(strlen("rx="))) ==
                        -c_locale_stod(d->steps_[i - 1].paramValues[3].substr(
                            strlen("rx="))) &&
                    c_locale_stod(
                        d->steps_[i].paramValues[4].substr(strlen("ry="))) ==
                        -c_locale_stod(d->steps_[i - 1].paramValues[4].substr(
                            strlen("ry="))) &&
                    c_locale_stod(
                        d->steps_[i].paramValues[5].substr(strlen("rz="))) ==
                        -c_locale_stod(d->steps_[i - 1].paramValues[5].substr(
                            strlen("rz="))) &&
                    d->steps_[i].paramValues[6] ==
                        d->steps_[i - 1].paramValues[6]) {
                    d->steps_.erase(d->steps_.begin() + i - 1,
                                    d->steps_.begin() + i + 1);
                    changeDone = true;
                    break;
                }
            } catch (const std::invalid_argument &) {
            }

            // detect a step and its inverse
            if (d->steps_[i].inverted != d->steps_[i - 1].inverted &&
                d->steps_[i].name == d->steps_[i - 1].name &&
                d->steps_[i].paramValues.size() ==
                    d->steps_[i - 1].paramValues.size()) {
                bool allSame = true;
                for (size_t j = 0; j < d->steps_[i].paramValues.size(); j++) {
                    if (d->steps_[i].paramValues[j] !=
                        d->steps_[i - 1].paramValues[j]) {
                        allSame = false;
                        break;
                    }
                }
                if (allSame) {
                    d->steps_.erase(d->steps_.begin() + i - 1,
                                    d->steps_.begin() + i + 1);
                    changeDone = true;
                    break;
                }
            }
        }
    } while (changeDone);

    if (d->steps_.size() > 1 ||
        (d->steps_.size() == 1 && d->steps_[0].inverted)) {
        d->appendToResult("+proj=pipeline");
    }
    for (const auto &step : d->steps_) {
        if (!d->result_.empty()) {
            d->appendToResult("+step");
        }
        if (step.inverted) {
            if (step.name == "unitconvert" && step.paramValues.size() == 2 &&
                starts_with(step.paramValues[0], "xy_in=") &&
                starts_with(step.paramValues[1], "xy_out=")) {
                d->appendToResult("+proj=" + step.name);
                d->appendToResult(
                    "+xy_in=" + step.paramValues[1].substr(strlen("xy_out=")));
                d->appendToResult("+xy_out=" +
                                  step.paramValues[0].substr(strlen("xy_in=")));
                continue;
            }
            // axisswap order=2,1 is its own inverse
            if (!(step.name == "axisswap" && step.paramValues.size() == 1 &&
                  step.paramValues[0] == "order=2,1")) {
                d->appendToResult("+inv");
            }
        }
        if (!step.name.empty()) {
            d->appendToResult("+proj=" + step.name);
        }
        for (const auto &paramValue : step.paramValues) {
            d->appendToResult("+" + paramValue);
        }
    }
    return d->result_;
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress

PROJStringFormatter::Convention PROJStringFormatter::convention() const {
    return d->convention_;
}

// ---------------------------------------------------------------------------

void PROJStringFormatter::enter() { ++d->level_; }

// ---------------------------------------------------------------------------

void PROJStringFormatter::leave() {
    assert(d->level_ > 0);
    --d->level_;
}

// ---------------------------------------------------------------------------

PROJStringFormatter::Scope::Scope(PROJStringFormatterNNPtr formatter)
    : formatter_(formatter) {
    formatter_->enter();
}

// ---------------------------------------------------------------------------

PROJStringFormatter::Scope::~Scope() {
    if (formatter_) {
        formatter_->leave();
    }
}
// ---------------------------------------------------------------------------

std::string PROJStringFormatter::Scope::toString() {
    assert(formatter_);
    formatter_->leave();
    auto ret = formatter_->toString();
    formatter_ = nullptr;
    return ret;
}

// ---------------------------------------------------------------------------

void PROJStringFormatter::Private::appendToResult(const std::string &str) {
    if (!result_.empty()) {
        result_ += " ";
    }
    result_ += str;
}

// ---------------------------------------------------------------------------

void PROJStringFormatter::ingestPROJString(
    const std::string &str) // throw ParsingException
{
    std::string word;
    std::istringstream iss(str, std::istringstream::in);
    bool inverted = false;
    bool prevWasStep = false;
    bool inProj = false;
    while (iss >> word) {
        if (word[0] == '+') {
            word = word.substr(1);
        }

        if (word == "proj=pipeline") {
            inverted = false;
            prevWasStep = false;
            inProj = true;
        } else if (word == "step") {
            inverted = false;
            prevWasStep = true;
        } else if (word == "inv") {
            if (prevWasStep) {
                inverted = true;
            } else {
                if (d->steps_.empty()) {
                    throw ParsingException("+inv found at unexpected place");
                }
                d->steps_.back().inverted = true;
            }
            prevWasStep = false;
        } else if (starts_with(word, "proj=")) {
            addStep(word.substr(std::string("proj=").size()));
            setCurrentStepInverted(inverted);
            prevWasStep = false;
            inProj = true;
        } else if (inProj) {
            addParam(word);
            prevWasStep = false;
        } else {
            throw ParsingException("Unexpected token: " + word);
        }
    }
}

// ---------------------------------------------------------------------------

void PROJStringFormatter::startInversion() {
    PROJStringFormatter::Private::InversionStackElt elt;
    elt.startIdx = d->steps_.size();
    elt.currentInversionState =
        !d->inversionStack_.back().currentInversionState;
    d->inversionStack_.push_back(elt);
}

// ---------------------------------------------------------------------------

void PROJStringFormatter::stopInversion() {
    assert(!d->inversionStack_.empty());
    auto startIdx = d->inversionStack_.back().startIdx;
    for (size_t i = startIdx; i < d->steps_.size(); i++) {
        d->steps_[i].inverted = !d->steps_[i].inverted;
    }
    std::reverse(d->steps_.begin() + startIdx, d->steps_.end());
    d->inversionStack_.pop_back();
}

// ---------------------------------------------------------------------------

bool PROJStringFormatter::isInverted() const {
    return d->inversionStack_.back().currentInversionState;
}

// ---------------------------------------------------------------------------

void PROJStringFormatter::addStep(const std::string &stepName) {
    PROJStringFormatter::Private::Step step;
    step.name = stepName;
    d->steps_.push_back(step);
}

// ---------------------------------------------------------------------------

void PROJStringFormatter::setCurrentStepInverted(bool inverted) {
    assert(!d->steps_.empty());
    d->steps_.back().inverted = inverted;
}

// ---------------------------------------------------------------------------

void PROJStringFormatter::addParam(const std::string &paramName) {
    if (d->steps_.empty()) {
        addStep(std::string());
    }
    d->steps_.back().paramValues.push_back(paramName);
}

// ---------------------------------------------------------------------------

void PROJStringFormatter::addParam(const std::string &paramName, int val) {
    std::ostringstream buffer;
    buffer.imbue(std::locale::classic());
    buffer << val;

    if (d->steps_.empty()) {
        addStep(std::string());
    }
    d->steps_.back().paramValues.push_back(paramName + "=" + buffer.str());
}

// ---------------------------------------------------------------------------

static std::string formatToString(double val) {
    std::ostringstream buffer;
    buffer.imbue(std::locale::classic());
    if (std::abs(val * 10 - std::round(val * 10)) < 1e-8) {
        // For the purpose of
        // https://www.epsg-registry.org/export.htm?wkt=urn:ogc:def:crs:EPSG::27561
        // Latitude of natural of origin to be properly rounded from 55 grad to
        // 49.5 deg
        val = std::round(val * 10) / 10;
    }
    buffer << std::setprecision(15) << val;
    return buffer.str();
}

// ---------------------------------------------------------------------------

void PROJStringFormatter::addParam(const std::string &paramName, double val) {

    if (d->steps_.empty()) {
        addStep(std::string());
    }
    d->steps_.back().paramValues.push_back(paramName + "=" +
                                           formatToString(val));
}

// ---------------------------------------------------------------------------

void PROJStringFormatter::addParam(const std::string &paramName,
                                   const std::vector<double> &vals) {
    std::string paramValue = paramName + "=";
    for (size_t i = 0; i < vals.size(); ++i) {
        if (i > 0) {
            paramValue += ",";
        }
        paramValue += formatToString(vals[i]);
    }

    if (d->steps_.empty()) {
        addStep(std::string());
    }
    d->steps_.back().paramValues.push_back(paramValue);
}

// ---------------------------------------------------------------------------

void PROJStringFormatter::addParam(const std::string &paramName,
                                   const char *val) {
    addParam(paramName, std::string(val));
}

// ---------------------------------------------------------------------------

void PROJStringFormatter::addParam(const std::string &paramName,
                                   const std::string &val) {
    if (d->steps_.empty()) {
        addStep(std::string());
    }
    d->steps_.back().paramValues.push_back(paramName + "=" + val);
}

// ---------------------------------------------------------------------------

void PROJStringFormatter::setTOWGS84Parameters(
    const std::vector<double> &params) {
    d->toWGS84Parameters_ = params;
}

// ---------------------------------------------------------------------------

const std::vector<double> &PROJStringFormatter::getTOWGS84Parameters() const {
    return d->toWGS84Parameters_;
}

// ---------------------------------------------------------------------------

void PROJStringFormatter::setVDatumExtension(const std::string &filename) {
    d->vDatumExtension_ = filename;
}

// ---------------------------------------------------------------------------

const std::string &PROJStringFormatter::getVDatumExtension() const {
    return d->vDatumExtension_;
}

// ---------------------------------------------------------------------------

void PROJStringFormatter::setHDatumExtension(const std::string &filename) {
    d->hDatumExtension_ = filename;
}

// ---------------------------------------------------------------------------

const std::string &PROJStringFormatter::getHDatumExtension() const {
    return d->hDatumExtension_;
}

// ---------------------------------------------------------------------------

void PROJStringFormatter::setOmitProjLongLatIfPossible(bool omit) {
    d->omitProjLongLatIfPossible_ = omit;
}

// ---------------------------------------------------------------------------

bool PROJStringFormatter::omitProjLongLatIfPossible() const {
    return d->omitProjLongLatIfPossible_;
}

//! @endcond

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
struct PROJStringParser::Private {
    std::vector<std::string> warningList_{};

    struct Step {
        std::string name{};
        bool inverted{false};
        std::vector<std::pair<std::string, std::string>> paramValues{};
    };
    std::vector<Step> steps_{};
    std::vector<std::pair<std::string, std::string>> globalParamValues_{};

    bool hasParamValue(const Step &step, const std::string &key);
    std::string getParamValue(const Step &step, const std::string &key);
    GeographicCRSNNPtr buildGeographicCRS(const Step &step, int iUnitConvert,
                                          int iAxisSwap);
    CRSNNPtr buildProjectedCRS(const Step &step, GeographicCRSNNPtr geogCRS);
    CRSNNPtr buildBoundOrCompoundCRSIfNeeded(const Step &step, CRSNNPtr crs);
    UnitOfMeasure buildUnit(const Step &step, const std::string &unitsParamName,
                            const std::string &toMeterParamName);
};

// ---------------------------------------------------------------------------

PROJStringParser::PROJStringParser() : d(internal::make_unique<Private>()) {}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
PROJStringParser::~PROJStringParser() = default;
//! @endcond

// ---------------------------------------------------------------------------

/** \brief Return the list of warnings found during parsing.
 */
std::vector<std::string> PROJStringParser::warningList() const {
    return d->warningList_;
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress

bool PROJStringParser::Private::hasParamValue(
    const PROJStringParser::Private::Step &step, const std::string &key) {
    for (const auto &pair : globalParamValues_) {
        if (ci_equal(pair.first, key)) {
            return true;
        }
    }
    for (const auto &pair : step.paramValues) {
        if (ci_equal(pair.first, key)) {
            return true;
        }
    }
    return false;
}

// ---------------------------------------------------------------------------

std::string PROJStringParser::Private::getParamValue(
    const PROJStringParser::Private::Step &step, const std::string &key) {
    for (const auto &pair : globalParamValues_) {
        if (ci_equal(pair.first, key)) {
            return pair.second;
        }
    }
    for (const auto &pair : step.paramValues) {
        if (ci_equal(pair.first, key)) {
            return pair.second;
        }
    }
    return std::string();
}

// ---------------------------------------------------------------------------

static const struct LinearUnitDesc {
    const char *projName;
    const char *convToMeter;
    const char *name;
    int epsgCode;
} linearUnitDescs[] = {
    {"mm", "0.001", "millimetre", 1025},
    {"cm", "0.01", "centimetre", 1033},
    {"m", "1.0", "metre", 9001},
    {"meter", "1.0", "metre", 9001}, // alternative
    {"metre", "1.0", "metre", 9001}, // alternative
    {"ft", "0.3048", "foot", 9002},
    {"us-ft", "0.3048006096012192", "US survey foot", 9003},
    {"fath", "1.8288", "fathom", 9014},
    {"kmi", "1852", "nautical mile", 9030},
    {"us-ch", "20.11684023368047", "US survey chain", 9033},
    {"us-mi", "1609.347218694437", "US survey mile", 9035},
    {"km", "1000.0", "kilometre", 9036},
    {"ind-ft", "0.30479841", "Indian foot (1937)", 9081},
    {"ind-yd", "0.91439523", "Indian yard (1937)", 9085},
    {"mi", "1609.344", "Statute mile", 9093},
    {"yd", "0.9144", "yard", 9096},
    {"ch", "20.1168", "chain", 9097},
    {"link", "0.201168", "link", 9098},
    {"dm", "0.1", "decimetre", 0},                       // no EPSG equivalent
    {"in", "0.0254", "inch", 0},                         // no EPSG equivalent
    {"us-in", "0.025400050800101", "US survey inch", 0}, // no EPSG equivalent
    {"us-yd", "0.914401828803658", "US survey yard", 0}, // no EPSG equivalent
    {"ind-ch", "20.11669506", "Indian chain", 0},        // no EPSG equivalent
};

static const LinearUnitDesc *getLinearUnits(const std::string &projName) {
    for (const auto &desc : linearUnitDescs) {
        if (desc.projName == projName)
            return &desc;
    }
    return nullptr;
}

static const LinearUnitDesc *getLinearUnits(double toMeter) {
    for (const auto &desc : linearUnitDescs) {
        if (std::fabs(c_locale_stod(desc.convToMeter) - toMeter) <
            1e-10 * toMeter) {
            return &desc;
        }
    }
    return nullptr;
}

// ---------------------------------------------------------------------------

UnitOfMeasure PROJStringParser::Private::buildUnit(
    const PROJStringParser::Private::Step &step,
    const std::string &unitsParamName, const std::string &toMeterParamName) {
    UnitOfMeasure unit = UnitOfMeasure::METRE;
    const LinearUnitDesc *unitsMatch = nullptr;
    auto projUnits = getParamValue(step, unitsParamName);
    if (!projUnits.empty()) {
        unitsMatch = getLinearUnits(projUnits);
        if (unitsMatch == nullptr) {
            throw ParsingException("unhandled " + unitsParamName + "=" +
                                   projUnits);
        }
    }

    auto toMeter = getParamValue(step, toMeterParamName);
    if (!toMeter.empty()) {
        double to_meter_value;
        try {
            to_meter_value = c_locale_stod(toMeter);
        } catch (const std::invalid_argument &) {
            throw ParsingException("invalid value for " + toMeterParamName);
        }
        unitsMatch = getLinearUnits(to_meter_value);
        if (unitsMatch == nullptr) {
            // TODO: look-up in EPSG catalog
            unit = UnitOfMeasure("unknown", to_meter_value,
                                 UnitOfMeasure::Type::LINEAR);
        }
    }

    if (unitsMatch) {
        std::string unitsCode;
        if (unitsMatch->epsgCode) {
            std::ostringstream buffer;
            buffer.imbue(std::locale::classic());
            buffer << unitsMatch->epsgCode;
            unitsCode = buffer.str();
        }
        unit = UnitOfMeasure(
            unitsMatch->name, c_locale_stod(unitsMatch->convToMeter),
            UnitOfMeasure::Type::LINEAR,
            unitsMatch->epsgCode ? Identifier::EPSG : std::string(), unitsCode);
    }

    return unit;
}

// ---------------------------------------------------------------------------

static const struct DatumDesc {
    const char *projName;
    const char *gcsName;
    int gcsCode;
    const char *datumName;
    int datumCode;
    const char *ellipsoidName;
    int ellipsoidCode;
    double a;
    double rf;
} datumDescs[] = {
    {"GGRS87", "GGRS87", 4121, "Greek Geodetic Reference System 1987", 6121,
     "GRS 1980", 7019, 6378137, 298.257222101},
    {"postdam", "DHDN", 4314, "Deutsches Hauptdreiecksnetz", 6314,
     "Bessel 1841", 7004, 6377397.155, 299.1528128},
    {"carthage", "Carthage", 4223, "Carthage", 6223, "Clarke 1880 (IGN)", 7011,
     6378249.2, 293.4660213},
    {"hermannskogel", "MGI", 4312, "Militar-Geographische Institut", 6312,
     "Bessel 1841", 7004, 6377397.155, 299.1528128},
    {"ire65", "TM65", 4299, "TM65", 6299, "Airy Modified 1849", 7002,
     6377340.189, 299.3249646},
    {"nzgd49", "NZGD49", 4272, "New Zealand Geodetic Datum 1949", 6272,
     "International 1924", 7022, 6378388, 297},
    {"OSGB36", "OSGB 1936", 4277, "OSGB 1936", 6277, "Airy 1830", 7001,
     6377563.396, 299.3249646},
};

// ---------------------------------------------------------------------------

GeographicCRSNNPtr PROJStringParser::Private::buildGeographicCRS(
    const PROJStringParser::Private::Step &step, int iUnitConvert,
    int iAxisSwap) {
    auto ellpsStr = getParamValue(step, "ellps");
    auto datumStr = getParamValue(step, "datum");
    auto aStr = getParamValue(step, "a");
    auto bStr = getParamValue(step, "b");
    auto rfStr = getParamValue(step, "rf");
    auto RStr = getParamValue(step, "R");
    auto pmStr = getParamValue(step, "pm");
    GeodeticReferenceFramePtr datum = nullptr;

    PrimeMeridianNNPtr pm = PrimeMeridian::GREENWICH;
    if (!pmStr.empty()) {
        try {
            double pmValue = c_locale_stod(pmStr);
            pm = PrimeMeridian::create(
                PropertyMap().set(IdentifiedObject::NAME_KEY, "unknown"),
                Angle(pmValue));
        } catch (const std::invalid_argument &) {
            bool found = false;
            if (pmStr == "paris") {
                found = true;
                pm = PrimeMeridian::PARIS;
            }
            for (int i = 0; !found && pj_prime_meridians[i].id != nullptr;
                 i++) {
                if (pmStr == pj_prime_meridians[i].id) {
                    found = true;
                    std::string name = static_cast<char>(::toupper(pmStr[0])) +
                                       pmStr.substr(1);
                    double pmValue =
                        dmstor(pj_prime_meridians[i].defn, nullptr) *
                        RAD_TO_DEG;
                    pm = PrimeMeridian::create(
                        PropertyMap().set(IdentifiedObject::NAME_KEY, name),
                        Angle(pmValue));
                    break;
                }
            }
            if (!found) {
                throw ParsingException("unknown pm " + pmStr);
            }
        }
    }

    if (!datumStr.empty()) {
        if (datumStr == "WGS84") {
            datum = GeodeticReferenceFrame::EPSG_6326;
        } else if (datumStr == "NAD83") {
            datum = GeodeticReferenceFrame::EPSG_6269;
        } else if (datumStr == "NAD27") {
            datum = GeodeticReferenceFrame::EPSG_6267;
        } else {

            for (const auto &datumDesc : datumDescs) {
                if (datumStr == datumDesc.projName) {
                    (void)datumDesc.gcsName; // to please cppcheck
                    (void)datumDesc.gcsCode; // to please cppcheck
                    auto ellipsoid = Ellipsoid::createFlattenedSphere(
                        PropertyMap()
                            .set(IdentifiedObject::NAME_KEY,
                                 datumDesc.ellipsoidName)
                            .set(Identifier::CODESPACE_KEY, Identifier::EPSG)
                            .set(Identifier::CODE_KEY, datumDesc.ellipsoidCode),
                        Length(datumDesc.a), Scale(datumDesc.rf));
                    datum =
                        GeodeticReferenceFrame::create(
                            PropertyMap()
                                .set(IdentifiedObject::NAME_KEY,
                                     datumDesc.datumName)
                                .set(Identifier::CODESPACE_KEY,
                                     Identifier::EPSG)
                                .set(Identifier::CODE_KEY, datumDesc.datumCode),
                            ellipsoid, util::optional<std::string>(), pm)
                            .as_nullable();
                    break;
                }
            }
        }
        if (!datum) {
            throw ParsingException("unknown datum " + datumStr);
        }
    } else if (!ellpsStr.empty()) {
        if (ellpsStr == "WGS84") {
            datum = GeodeticReferenceFrame::create(
                        PropertyMap().set(IdentifiedObject::NAME_KEY,
                                          "Unknown based on WGS84 ellipsoid"),
                        Ellipsoid::WGS84, util::optional<std::string>(), pm)
                        .as_nullable();
        } else if (ellpsStr == "GRS80") {
            datum = GeodeticReferenceFrame::create(
                        PropertyMap().set(IdentifiedObject::NAME_KEY,
                                          "Unknown based on GRS80 ellipsoid"),
                        Ellipsoid::GRS1980, util::optional<std::string>(), pm)
                        .as_nullable();
        } else {
            for (int i = 0; pj_ellps[i].id != nullptr; i++) {
                if (ellpsStr == pj_ellps[i].id) {
                    assert(strncmp(pj_ellps[i].major, "a=", 2) == 0);
                    const double a_iter = c_locale_stod(pj_ellps[i].major + 2);
                    EllipsoidPtr ellipsoid;
                    if (strncmp(pj_ellps[i].ell, "b=", 2) == 0) {
                        const double b_iter =
                            c_locale_stod(pj_ellps[i].ell + 2);
                        ellipsoid =
                            Ellipsoid::createTwoAxis(
                                PropertyMap().set(IdentifiedObject::NAME_KEY,
                                                  pj_ellps[i].name),
                                Length(a_iter), Length(b_iter))
                                .as_nullable();
                    } else {
                        assert(strncmp(pj_ellps[i].ell, "rf=", 3) == 0);
                        const double rf_iter =
                            c_locale_stod(pj_ellps[i].ell + 3);
                        ellipsoid =
                            Ellipsoid::createFlattenedSphere(
                                PropertyMap().set(IdentifiedObject::NAME_KEY,
                                                  pj_ellps[i].name),
                                Length(a_iter), Scale(rf_iter))
                                .as_nullable();
                    }
                    datum = GeodeticReferenceFrame::create(
                                PropertyMap().set(
                                    IdentifiedObject::NAME_KEY,
                                    std::string("Unknown based on ") +
                                        pj_ellps[i].name + " ellipsoid"),
                                NN_CHECK_ASSERT(ellipsoid),
                                util::optional<std::string>(), pm)
                                .as_nullable();
                    break;
                }
            }
            if (!datum) {
                throw ParsingException("unknown ellipsoid " + ellpsStr);
            }
        }
    }

    else if (!aStr.empty() && !bStr.empty()) {
        double a;
        try {
            a = c_locale_stod(aStr);
        } catch (const std::invalid_argument &) {
            throw ParsingException("Invalid a value");
        }
        double b;
        try {
            b = c_locale_stod(bStr);
        } catch (const std::invalid_argument &) {
            throw ParsingException("Invalid b value");
        }
        auto ellipsoid =
            Ellipsoid::createTwoAxis(
                PropertyMap().set(IdentifiedObject::NAME_KEY, "unknown"),
                Length(a), Length(b))
                ->identify();
        datum = GeodeticReferenceFrame::create(
                    PropertyMap().set(IdentifiedObject::NAME_KEY, "unknown"),
                    ellipsoid, util::optional<std::string>(), pm)
                    .as_nullable();
    }

    else if (!aStr.empty() && !rfStr.empty()) {
        double a;
        try {
            a = c_locale_stod(aStr);
        } catch (const std::invalid_argument &) {
            throw ParsingException("Invalid a value");
        }
        double rf;
        try {
            rf = c_locale_stod(rfStr);
        } catch (const std::invalid_argument &) {
            throw ParsingException("Invalid rf value");
        }
        auto ellipsoid =
            Ellipsoid::createFlattenedSphere(
                PropertyMap().set(IdentifiedObject::NAME_KEY, "unknown"),
                Length(a), Scale(rf))
                ->identify();
        datum = GeodeticReferenceFrame::create(
                    PropertyMap().set(IdentifiedObject::NAME_KEY, "unknown"),
                    ellipsoid, util::optional<std::string>(), pm)
                    .as_nullable();
    }

    else if (!RStr.empty()) {
        double R;
        try {
            R = c_locale_stod(RStr);
        } catch (const std::invalid_argument &) {
            throw ParsingException("Invalid Rvalue");
        }
        auto ellipsoid = Ellipsoid::createSphere(
            PropertyMap().set(IdentifiedObject::NAME_KEY, "unknown"),
            Length(R));
        datum = GeodeticReferenceFrame::create(
                    PropertyMap().set(IdentifiedObject::NAME_KEY, "unknown"),
                    ellipsoid, util::optional<std::string>(), pm)
                    .as_nullable();
    }

    if (!aStr.empty() && bStr.empty() && rfStr.empty()) {
        throw ParsingException("a found, but b or rf missing");
    }

    if (!bStr.empty() && aStr.empty()) {
        throw ParsingException("b found, but a missing");
    }

    if (!rfStr.empty() && aStr.empty()) {
        throw ParsingException("rf found, but a missing");
    }

    UnitOfMeasure angularUnit = UnitOfMeasure::DEGREE;
    if (iUnitConvert >= 0) {
        auto stepUnitConvert = steps_[iUnitConvert];
        auto xy_in = getParamValue(stepUnitConvert, "xy_in");
        auto xy_out = getParamValue(stepUnitConvert, "xy_out");
        if (stepUnitConvert.inverted) {
            std::swap(xy_in, xy_out);
        }
        if (xy_in.empty() || xy_out.empty() || xy_in != "rad" ||
            (xy_out != "rad" && xy_out != "deg" && xy_out != "grad")) {
            throw ParsingException("unhandled values for xy_in and/or xy_out");
        }
        if (xy_out == "rad") {
            angularUnit = UnitOfMeasure::RADIAN;
        } else if (xy_out == "grad") {
            angularUnit = UnitOfMeasure::GRAD;
        }
    }

    CoordinateSystemAxisNNPtr east = CoordinateSystemAxis::create(
        PropertyMap().set(IdentifiedObject::NAME_KEY, AxisName::Longitude),
        AxisAbbreviation::lon, AxisDirection::EAST, angularUnit);
    CoordinateSystemAxisNNPtr north = CoordinateSystemAxis::create(
        PropertyMap().set(IdentifiedObject::NAME_KEY, AxisName::Latitude),
        AxisAbbreviation::lat, AxisDirection::NORTH, angularUnit);
    CoordinateSystemAxisNNPtr west = CoordinateSystemAxis::create(
        PropertyMap().set(IdentifiedObject::NAME_KEY, AxisName::Longitude),
        AxisAbbreviation::lon, AxisDirection::WEST, angularUnit);
    CoordinateSystemAxisNNPtr south = CoordinateSystemAxis::create(
        PropertyMap().set(IdentifiedObject::NAME_KEY, AxisName::Latitude),
        AxisAbbreviation::lat, AxisDirection::SOUTH, angularUnit);

    std::vector<CoordinateSystemAxisNNPtr> axis{east, north};

    auto axisStr = getParamValue(step, "axis");
    if (!axisStr.empty()) {
        if (axisStr.size() == 3) {
            for (int i = 0; i < 2; i++) {
                if (axisStr[i] == 'n') {
                    axis[i] = north;
                } else if (axisStr[i] == 's') {
                    axis[i] = south;
                } else if (axisStr[i] == 'e') {
                    axis[i] = east;
                } else if (axisStr[i] == 'w') {
                    axis[i] = west;
                } else {
                    throw ParsingException("Unhandled axis=" + axisStr);
                }
            }
        }
    } else if (iAxisSwap >= 0) {
        auto stepAxisSwap = steps_[iAxisSwap];
        auto orderStr = getParamValue(stepAxisSwap, "order");
        auto orderTab = split(orderStr, ',');
        if (orderTab.size() < 2) {
            throw ParsingException("Unhandled order=" + orderStr);
        }
        if (stepAxisSwap.inverted) {
            throw ParsingException("Unhandled +inv for +proj=axisswap");
        }

        for (size_t i = 0; i < 2; i++) {
            if (orderTab[i] == "1") {
                axis[i] = east;
            } else if (orderTab[i] == "-1") {
                axis[i] = west;
            } else if (orderTab[i] == "2") {
                axis[i] = north;
            } else if (orderTab[i] == "-2") {
                axis[i] = south;
            } else {
                throw ParsingException("Unhandled order=" + orderStr);
            }
        }
    }

    CoordinateSystemAxisNNPtr up = CoordinateSystemAxis::create(
        util::PropertyMap().set(IdentifiedObject::NAME_KEY,
                                AxisName::Ellipsoidal_height),
        AxisAbbreviation::h, AxisDirection::UP,
        buildUnit(step, "vunits", "vto_meter"));

    auto cs = (getParamValue(step, "geoidgrids").empty() &&
               (!getParamValue(step, "vunits").empty() ||
                !getParamValue(step, "vto_meter").empty()))
                  ? EllipsoidalCS::create(PropertyMap(), axis[0], axis[1], up)
                  : EllipsoidalCS::create(PropertyMap(), axis[0], axis[1]);

    if (!datum) {
        if (pm->isEquivalentTo(PrimeMeridian::GREENWICH)) {
            datum = GeodeticReferenceFrame::EPSG_6326;
        } else {
            datum = GeodeticReferenceFrame::create(
                        PropertyMap().set(IdentifiedObject::NAME_KEY,
                                          "Unknown based on WGS84 ellipsoid"),
                        Ellipsoid::WGS84, util::optional<std::string>(), pm)
                        .as_nullable();
        }
    }

    return GeographicCRS::create(
        PropertyMap().set(IdentifiedObject::NAME_KEY, "unknown"),
        NN_CHECK_ASSERT(datum), cs);
}

// ---------------------------------------------------------------------------

CRSNNPtr PROJStringParser::Private::buildBoundOrCompoundCRSIfNeeded(
    const PROJStringParser::Private::Step &step, CRSNNPtr crs) {
    auto towgs84 = getParamValue(step, "towgs84");
    if (!towgs84.empty()) {
        std::vector<double> towgs84Values;
        for (const auto &str : split(towgs84, ',')) {
            try {
                towgs84Values.push_back(c_locale_stod(str));
            } catch (const std::invalid_argument &) {
                throw ParsingException("Non numerical value in towgs84 clause");
            }
        }
        crs = BoundCRS::createFromTOWGS84(crs, towgs84Values);
    }

    auto nadgrids = getParamValue(step, "nadgrids");
    if (!nadgrids.empty()) {
        crs = BoundCRS::createFromNadgrids(crs, nadgrids);
    }

    auto geoidgrids = getParamValue(step, "geoidgrids");
    if (!geoidgrids.empty()) {
        auto vdatum = VerticalReferenceFrame::create(
            PropertyMap().set(IdentifiedObject::NAME_KEY, "unknown"));

        const UnitOfMeasure unit = buildUnit(step, "vunits", "vto_meter");

        auto vcrs = VerticalCRS::create(
            PropertyMap().set(IdentifiedObject::NAME_KEY, "unknown"), vdatum,
            VerticalCS::createGravityRelatedHeight(unit));

        auto transformation =
            Transformation::createGravityRelatedHeightToGeographic3D(
                PropertyMap().set(IdentifiedObject::NAME_KEY,
                                  "unknown to WGS84 ellipsoidal height"),
                crs, GeographicCRS::EPSG_4979, geoidgrids,
                std::vector<PositionalAccuracyNNPtr>());
        auto boundvcrs =
            BoundCRS::create(vcrs, GeographicCRS::EPSG_4979, transformation);

        crs = CompoundCRS::create(
            PropertyMap().set(IdentifiedObject::NAME_KEY, "unknown"),
            std::vector<CRSNNPtr>{crs, boundvcrs});
    }

    return crs;
}

// ---------------------------------------------------------------------------

CRSNNPtr PROJStringParser::Private::buildProjectedCRS(
    const PROJStringParser::Private::Step &step, GeographicCRSNNPtr geogCRS) {
    auto mappings = getMappingsFromPROJName(step.name);
    // TODO: handle mappings.size() > 1

    auto convMap =
        PropertyMap().set(IdentifiedObject::NAME_KEY, mappings[0]->wkt2_name);
    if (mappings[0]->epsg_code) {
        convMap.set(Identifier::CODESPACE_KEY, Identifier::EPSG)
            .set(Identifier::CODE_KEY, mappings[0]->epsg_code);
    }
    std::vector<OperationParameterNNPtr> parameters;
    std::vector<ParameterValueNNPtr> values;
    for (const auto &param : mappings[0]->params) {
        auto paramValue = !param.proj_names.empty()
                              ? getParamValue(step, param.proj_names[0])
                              : std::string();
        // k and k_0 may be used indifferently
        if (paramValue.empty() && param.proj_names[0] == "k") {
            paramValue = getParamValue(step, "k_0");
        } else if (paramValue.empty() && param.proj_names[0] == "k_0") {
            paramValue = getParamValue(step, "k");
        }
        double value = 0;
        if (!paramValue.empty()) {
            try {
                value = c_locale_stod(paramValue);
            } catch (const std::invalid_argument &) {
                throw ParsingException("invalid value for " +
                                       param.proj_names[0]);
            }
        } else if (param.unit_type == UnitOfMeasure::Type::SCALE) {
            value = 1;
        }

        PropertyMap propertiesParameter;
        propertiesParameter.set(IdentifiedObject::NAME_KEY, param.wkt2_name);
        if (param.epsg_code) {
            propertiesParameter.set(Identifier::CODE_KEY, param.epsg_code);
            propertiesParameter.set(Identifier::CODESPACE_KEY,
                                    Identifier::EPSG);
        }
        parameters.push_back(OperationParameter::create(propertiesParameter));
        // In PROJ convention, angular parameters are always in degree and
        // linear parameters always in metre.
        values.push_back(ParameterValue::create(Measure(
            value, param.unit_type == UnitOfMeasure::Type::ANGULAR
                       ? UnitOfMeasure::DEGREE
                       : param.unit_type == UnitOfMeasure::Type::LINEAR
                             ? UnitOfMeasure::METRE
                             : param.unit_type == UnitOfMeasure::Type::SCALE
                                   ? UnitOfMeasure::SCALE_UNITY
                                   : UnitOfMeasure::NONE)));
    }

    auto conv = Conversion::create(
        PropertyMap().set(IdentifiedObject::NAME_KEY, "unknown"), convMap,
        parameters, values);

    const UnitOfMeasure unit = buildUnit(step, "units", "to_meter");

    std::vector<CoordinateSystemAxisNNPtr> axis{
        CoordinateSystemAxis::create(
            util::PropertyMap().set(IdentifiedObject::NAME_KEY,
                                    AxisName::Easting),
            AxisAbbreviation::E, AxisDirection::EAST, unit),
        CoordinateSystemAxis::create(
            util::PropertyMap().set(IdentifiedObject::NAME_KEY,
                                    AxisName::Northing),
            AxisAbbreviation::N, AxisDirection::NORTH, unit)};

    auto cs = CartesianCS::create(PropertyMap(), axis[0], axis[1]);

    CRSNNPtr crs = ProjectedCRS::create(
        PropertyMap().set(IdentifiedObject::NAME_KEY, "unknown"), geogCRS, conv,
        cs);

    if (getParamValue(step, "geoidgrids").empty() &&
        (!getParamValue(step, "vunits").empty() ||
         !getParamValue(step, "vto_meter").empty())) {
        auto vdatum = VerticalReferenceFrame::create(
            PropertyMap().set(IdentifiedObject::NAME_KEY, "unknown"));

        const UnitOfMeasure vunit = buildUnit(step, "vunits", "vto_meter");

        auto vcrs = VerticalCRS::create(
            PropertyMap().set(IdentifiedObject::NAME_KEY, "unknown"), vdatum,
            VerticalCS::createGravityRelatedHeight(vunit));

        crs = CompoundCRS::create(
            PropertyMap().set(IdentifiedObject::NAME_KEY, "unknown"),
            std::vector<CRSNNPtr>{crs, vcrs});
    }

    return crs;
}

//! @endcond

// ---------------------------------------------------------------------------

/** \brief Instanciate a sub-class of BaseObject from a PROJ string.
 * @throw ParsingException
 */
BaseObjectNNPtr
PROJStringParser::createFromPROJString(const std::string &projString) {
    std::string word;
    std::istringstream iss(projString, std::istringstream::in);
    bool inverted = false;
    bool prevWasStep = false;
    bool inProj = false;
    bool inPipeline = false;
    while (iss >> word) {
        if (word[0] == '+') {
            word = word.substr(1);
        }

        if (word == "proj=pipeline") {
            if (inPipeline) {
                throw ParsingException("nested pipeline not supported");
            }
            inverted = false;
            prevWasStep = false;
            inProj = true;
            inPipeline = true;
        } else if (word == "step") {
            if (!inPipeline) {
                throw ParsingException("+step found outside pipeline");
            }
            inverted = false;
            prevWasStep = true;
        } else if (word == "inv") {
            if (prevWasStep) {
                inverted = true;
            } else {
                if (d->steps_.empty()) {
                    throw ParsingException("+inv found at unexpected place");
                }
                d->steps_.back().inverted = true;
            }
            prevWasStep = false;
        } else if (starts_with(word, "proj=")) {
            auto stepName = word.substr(std::string("proj=").size());
            d->steps_.push_back(Private::Step());
            d->steps_.back().name = stepName;
            d->steps_.back().inverted = inverted;
            prevWasStep = false;
            inProj = true;
        } else if (inProj) {
            std::string key;
            std::string value;
            size_t pos = word.find('=');
            if (pos != std::string::npos) {
                key = word.substr(0, pos);
                value = word.substr(pos + 1);
            } else {
                key = word;
            }
            auto pair = std::make_pair(key, value);
            if (d->steps_.empty()) {
                d->globalParamValues_.push_back(pair);
            } else {
                d->steps_.back().paramValues.push_back(pair);
            }
            prevWasStep = false;
        } else {
            throw ParsingException("Unexpected token: " + word);
        }
    }

    if (d->steps_.empty()) {
        throw ParsingException("Missing proj= argument");
    }

    if (d->steps_.size() <= 3) {
        int iMain = -1;
        int iUnitConvert = -1;
        int iAxisSwap = -1;
        bool isGeographic = false;
        for (int i = 0; i < static_cast<int>(d->steps_.size()); i++) {
            const auto &projName = d->steps_[i].name;
            if (ci_equal(projName, "longlat") ||
                ci_equal(projName, "latlong") || ci_equal(projName, "lonlat") ||
                ci_equal(projName, "latlon")) {
                iMain = i;
                isGeographic = true;
            } else if (ci_equal(projName, "unitconvert")) {
                iUnitConvert = i;
            } else if (ci_equal(projName, "axisswap")) {
                iAxisSwap = i;
            } else {
                auto res = getMappingsFromPROJName(projName);
                if (!res.empty()) {
                    iMain = i;
                }
            }
        }
        if (iMain >= 0 && (1U + ((iUnitConvert >= 0) ? 1U : 0U) +
                               ((iAxisSwap >= 0) ? 1U : 0U) ==
                           d->steps_.size())) {
            auto geogCRS = d->buildGeographicCRS(d->steps_[iMain], iUnitConvert,
                                                 iAxisSwap);
            if (isGeographic) {
                return d->buildBoundOrCompoundCRSIfNeeded(d->steps_[iMain],
                                                          geogCRS);
            }
            if (!d->steps_[iMain].inverted) {
                return d->buildBoundOrCompoundCRSIfNeeded(
                    d->steps_[iMain],
                    d->buildProjectedCRS(d->steps_[iMain], geogCRS));
            }
        }
    }

    throw ParsingException("could not parse PROJ string");
}

} // namespace io
NS_PROJ_END
