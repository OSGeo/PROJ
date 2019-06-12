/******************************************************************************
 *
 * Project:  PROJ
 * Purpose:  ISO19111:2019 implementation
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
#include <list>
#include <locale>
#include <map>
#include <set>
#include <sstream> // std::istringstream
#include <string>
#include <utility>
#include <vector>

#include "proj/common.hpp"
#include "proj/coordinateoperation.hpp"
#include "proj/coordinatesystem.hpp"
#include "proj/crs.hpp"
#include "proj/datum.hpp"
#include "proj/io.hpp"
#include "proj/metadata.hpp"
#include "proj/util.hpp"

#include "proj/internal/coordinateoperation_internal.hpp"
#include "proj/internal/coordinatesystem_internal.hpp"
#include "proj/internal/internal.hpp"
#include "proj/internal/io_internal.hpp"

#include "proj_constants.h"

#include "wkt1_parser.h"
#include "wkt2_parser.h"

// PROJ include order is sensitive
// clang-format off
#include "proj.h"
#include "proj_internal.h"
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

//! @cond Doxygen_Suppress
static const std::string emptyString{};
//! @endcond

#if 0
namespace dropbox{ namespace oxygen {
template<> nn<NS_PROJ::io::DatabaseContextPtr>::~nn() = default;
template<> nn<NS_PROJ::io::AuthorityFactoryPtr>::~nn() = default;
template<> nn<std::shared_ptr<NS_PROJ::io::IPROJStringExportable>>::~nn() = default;
template<> nn<std::unique_ptr<NS_PROJ::io::PROJStringFormatter, std::default_delete<NS_PROJ::io::PROJStringFormatter> > >::~nn() = default;
template<> nn<std::unique_ptr<NS_PROJ::io::WKTFormatter, std::default_delete<NS_PROJ::io::WKTFormatter> > >::~nn() = default;
template<> nn<std::unique_ptr<NS_PROJ::io::WKTNode, std::default_delete<NS_PROJ::io::WKTNode> > >::~nn() = default;
}}
#endif

NS_PROJ_START
namespace io {

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
IWKTExportable::~IWKTExportable() = default;

// ---------------------------------------------------------------------------

std::string IWKTExportable::exportToWKT(WKTFormatter *formatter) const {
    _exportToWKT(formatter);
    return formatter->toString();
}

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
        bool useESRIDialect_ = false;
        OutputAxisRule outputAxis_ = WKTFormatter::OutputAxisRule::YES;
    };
    Params params_{};
    DatabaseContextPtr dbContext_{};

    int indentLevel_ = 0;
    int level_ = 0;
    std::vector<bool> stackHasChild_{};
    std::vector<bool> stackHasId_{false};
    std::vector<bool> stackEmptyKeyword_{};
    std::vector<bool> stackDisableUsage_{};
    std::vector<bool> outputUnitStack_{true};
    std::vector<bool> outputIdStack_{true};
    std::vector<UnitOfMeasureNNPtr> axisLinearUnitStack_{
        util::nn_make_shared<UnitOfMeasure>(UnitOfMeasure::METRE)};
    std::vector<UnitOfMeasureNNPtr> axisAngularUnitStack_{
        util::nn_make_shared<UnitOfMeasure>(UnitOfMeasure::DEGREE)};
    bool abridgedTransformation_ = false;
    bool useDerivingConversion_ = false;
    std::vector<double> toWGS84Parameters_{};
    std::string hDatumExtension_{};
    std::string vDatumExtension_{};
    std::vector<bool> inversionStack_{false};
    std::string result_{};

    // cppcheck-suppress functionStatic
    void addNewLine();
    void addIndentation();
    // cppcheck-suppress functionStatic
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
 * @param dbContext Database context, to allow queries in it if needed.
 * This is used for example for WKT1_ESRI output to do name substitutions.
 *
 * @return new formatter.
 */
WKTFormatterNNPtr WKTFormatter::create(Convention convention,
                                       // cppcheck-suppress passedByValue
                                       DatabaseContextPtr dbContext) {
    auto ret = NN_NO_CHECK(WKTFormatter::make_unique<WKTFormatter>(convention));
    ret->d->dbContext_ = dbContext;
    return ret;
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
WKTFormatterNNPtr WKTFormatter::create(const WKTFormatterNNPtr &other) {
    auto f = create(other->d->params_.convention_, other->d->dbContext_);
    f->d->params_ = other->d->params_;
    return f;
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
WKTFormatter::~WKTFormatter() = default;
//! @endcond

// ---------------------------------------------------------------------------

/** \brief Whether to use multi line output or not. */
WKTFormatter &WKTFormatter::setMultiLine(bool multiLine) noexcept {
    d->params_.multiLine_ = multiLine;
    return *this;
}

// ---------------------------------------------------------------------------

/** \brief Set number of spaces for each indentation level (defaults to 4).
 */
WKTFormatter &WKTFormatter::setIndentationWidth(int width) noexcept {
    d->params_.indentWidth_ = width;
    return *this;
}

// ---------------------------------------------------------------------------

/** \brief Set whether AXIS nodes should be output.
 */
WKTFormatter &
WKTFormatter::setOutputAxis(OutputAxisRule outputAxisIn) noexcept {
    d->params_.outputAxis_ = outputAxisIn;
    return *this;
}

// ---------------------------------------------------------------------------

/** \brief Set whether the formatter should operate on strict more or not.
 *
 * The default is strict mode, in which case a FormattingException can be
 * thrown.
 */
WKTFormatter &WKTFormatter::setStrict(bool strictIn) noexcept {
    d->params_.strict_ = strictIn;
    return *this;
}

// ---------------------------------------------------------------------------

/** \brief Returns whether the formatter is in strict mode. */
bool WKTFormatter::isStrict() const noexcept { return d->params_.strict_; }

// ---------------------------------------------------------------------------

/** Returns the WKT string from the formatter. */
const std::string &WKTFormatter::toString() const {
    if (d->indentLevel_ > 0 || d->level_ > 0) {
        // For intermediary nodes, the formatter is in a inconsistent
        // state.
        throw FormattingException("toString() called on intermediate nodes");
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
    if (d->stackHasId_.size() != 1)
        throw FormattingException("Unbalanced pushHasId() / popHasId()");
    if (!d->stackDisableUsage_.empty())
        throw FormattingException(
            "Unbalanced pushDisableUsage() / popDisableUsage()");

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
        d->params_.outputAxis_ =
            WKTFormatter::OutputAxisRule::WKT1_GDAL_EPSG_STYLE;
        break;

    case Convention::WKT1_ESRI:
        d->params_.version_ = WKTFormatter::Version::WKT1;
        d->params_.outputAxisOrder_ = false;
        d->params_.forceUNITKeyword_ = true;
        d->params_.primeMeridianInDegree_ = true;
        d->params_.useESRIDialect_ = true;
        d->params_.multiLine_ = false;
        d->params_.outputAxis_ = WKTFormatter::OutputAxisRule::NO;
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
    } else if (!d->result_.empty()) {
        d->result_ += ',';
        if (d->params_.multiLine_ && !keyword.empty()) {
            d->addNewLine();
        }
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
        d->result_ += '[';
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
        d->result_ += ']';
}

// ---------------------------------------------------------------------------

WKTFormatter &WKTFormatter::simulCurNodeHasId() {
    d->stackHasId_.back() = true;
    return *this;
}

// ---------------------------------------------------------------------------

void WKTFormatter::Private::startNewChild() {
    assert(!stackHasChild_.empty());
    if (stackHasChild_.back()) {
        result_ += ',';
    }
    stackHasChild_.back() = true;
}

// ---------------------------------------------------------------------------

void WKTFormatter::addQuotedString(const char *str) {
    addQuotedString(std::string(str));
}

void WKTFormatter::addQuotedString(const std::string &str) {
    d->startNewChild();
    d->result_ += '"';
    d->result_ += replaceAll(str, "\"", "\"\"");
    d->result_ += '"';
}

// ---------------------------------------------------------------------------

void WKTFormatter::add(const std::string &str) {
    d->startNewChild();
    d->result_ += str;
}

// ---------------------------------------------------------------------------

void WKTFormatter::add(int number) {
    d->startNewChild();
    d->result_ += internal::toString(number);
}

// ---------------------------------------------------------------------------

#ifdef __MINGW32__
static std::string normalizeExponent(const std::string &in) {
    // mingw will output 1e-0xy instead of 1e-xy. Fix that
    auto pos = in.find("e-0");
    if (pos == std::string::npos) {
        return in;
    }
    if (pos + 4 < in.size() && isdigit(in[pos + 3]) && isdigit(in[pos + 4])) {
        return in.substr(0, pos + 2) + in.substr(pos + 3);
    }
    return in;
}
#else
static inline std::string normalizeExponent(const std::string &in) {
    return in;
}
#endif

static inline std::string normalizeSerializedString(const std::string &in) {
    auto ret(normalizeExponent(in));
    return ret;
}

// ---------------------------------------------------------------------------

void WKTFormatter::add(double number, int precision) {
    d->startNewChild();
    if (number == 0.0) {
        if (d->params_.useESRIDialect_) {
            d->result_ += "0.0";
        } else {
            d->result_ += '0';
        }
    } else {
        std::string val(
            normalizeSerializedString(internal::toString(number, precision)));
        d->result_ += replaceAll(val, "e", "E");
        if (d->params_.useESRIDialect_ && val.find('.') == std::string::npos) {
            d->result_ += ".0";
        }
    }
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

bool WKTFormatter::outputId() const {
    return !d->params_.useESRIDialect_ && d->outputIdStack_.back();
}

// ---------------------------------------------------------------------------

void WKTFormatter::pushHasId(bool hasId) { d->stackHasId_.push_back(hasId); }

// ---------------------------------------------------------------------------

void WKTFormatter::popHasId() { d->stackHasId_.pop_back(); }

// ---------------------------------------------------------------------------

void WKTFormatter::pushDisableUsage() { d->stackDisableUsage_.push_back(true); }

// ---------------------------------------------------------------------------

void WKTFormatter::popDisableUsage() { d->stackDisableUsage_.pop_back(); }

// ---------------------------------------------------------------------------

bool WKTFormatter::outputUsage() const {
    return outputId() && d->stackDisableUsage_.empty();
}

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

WKTFormatter::OutputAxisRule WKTFormatter::outputAxis() const {
    return d->params_.outputAxis_;
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

bool WKTFormatter::idOnTopLevelOnly() const {
    return d->params_.idOnTopLevelOnly_;
}

// ---------------------------------------------------------------------------

bool WKTFormatter::topLevelHasId() const {
    return d->stackHasId_.size() >= 2 && d->stackHasId_[1];
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

bool WKTFormatter::useESRIDialect() const { return d->params_.useESRIDialect_; }

// ---------------------------------------------------------------------------

const DatabaseContextPtr &WKTFormatter::databaseContext() const {
    return d->dbContext_;
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

std::string WKTFormatter::morphNameToESRI(const std::string &name) {

    for (const auto *suffix : {"(m)", "(ftUS)", "(E-N)", "(N-E)"}) {
        if (ends_with(name, suffix)) {
            return morphNameToESRI(
                       name.substr(0, name.size() - strlen(suffix))) +
                   suffix;
        }
    }

    std::string ret;
    bool insertUnderscore = false;
    // Replace any special character by underscore, except at the beginning
    // and of the name where those characters are removed.
    for (char ch : name) {
        if (ch == '+' || ch == '-' || (ch >= '0' && ch <= '9') ||
            (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z')) {
            if (insertUnderscore && !ret.empty()) {
                ret += '_';
            }
            ret += ch;
            insertUnderscore = false;
        } else {
            insertUnderscore = true;
        }
    }
    return ret;
}

// ---------------------------------------------------------------------------

void WKTFormatter::ingestWKTNode(const WKTNodeNNPtr &node) {
    startNode(node->value(), true);
    for (const auto &child : node->children()) {
        if (!child->children().empty()) {
            ingestWKTNode(child);
        } else {
            add(child->value());
        }
    }
    endNode();
}

#ifdef unused
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
#endif

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress

static WKTNodeNNPtr
    null_node(NN_NO_CHECK(internal::make_unique<WKTNode>(std::string())));

static inline bool isNull(const WKTNodeNNPtr &node) {
    return &node == &null_node;
}

struct WKTNode::Private {
    std::string value_{};
    std::vector<WKTNodeNNPtr> children_{};

    explicit Private(const std::string &valueIn) : value_(valueIn) {}

    // cppcheck-suppress functionStatic
    inline const std::string &value() PROJ_PURE_DEFN { return value_; }

    // cppcheck-suppress functionStatic
    inline const std::vector<WKTNodeNNPtr> &children() PROJ_PURE_DEFN {
        return children_;
    }

    // cppcheck-suppress functionStatic
    inline size_t childrenSize() PROJ_PURE_DEFN { return children_.size(); }

    // cppcheck-suppress functionStatic
    const WKTNodeNNPtr &lookForChild(const std::string &childName,
                                     int occurrence) const noexcept;

    // cppcheck-suppress functionStatic
    const WKTNodeNNPtr &lookForChild(const std::string &name) const noexcept;

    // cppcheck-suppress functionStatic
    const WKTNodeNNPtr &lookForChild(const std::string &name,
                                     const std::string &name2) const noexcept;

    // cppcheck-suppress functionStatic
    const WKTNodeNNPtr &lookForChild(const std::string &name,
                                     const std::string &name2,
                                     const std::string &name3) const noexcept;

    // cppcheck-suppress functionStatic
    const WKTNodeNNPtr &lookForChild(const std::string &name,
                                     const std::string &name2,
                                     const std::string &name3,
                                     const std::string &name4) const noexcept;
};

#define GP() getPrivate()

// ---------------------------------------------------------------------------

const WKTNodeNNPtr &WKTNode::Private::lookForChild(const std::string &childName,
                                                   int occurrence) const
    noexcept {
    int occCount = 0;
    for (const auto &child : children_) {
        if (ci_equal(child->GP()->value(), childName)) {
            if (occurrence == occCount) {
                return child;
            }
            occCount++;
        }
    }
    return null_node;
}

const WKTNodeNNPtr &
WKTNode::Private::lookForChild(const std::string &name) const noexcept {
    for (const auto &child : children_) {
        const auto &v = child->GP()->value();
        if (ci_equal(v, name)) {
            return child;
        }
    }
    return null_node;
}

const WKTNodeNNPtr &
WKTNode::Private::lookForChild(const std::string &name,
                               const std::string &name2) const noexcept {
    for (const auto &child : children_) {
        const auto &v = child->GP()->value();
        if (ci_equal(v, name) || ci_equal(v, name2)) {
            return child;
        }
    }
    return null_node;
}

const WKTNodeNNPtr &
WKTNode::Private::lookForChild(const std::string &name,
                               const std::string &name2,
                               const std::string &name3) const noexcept {
    for (const auto &child : children_) {
        const auto &v = child->GP()->value();
        if (ci_equal(v, name) || ci_equal(v, name2) || ci_equal(v, name3)) {
            return child;
        }
    }
    return null_node;
}

const WKTNodeNNPtr &WKTNode::Private::lookForChild(
    const std::string &name, const std::string &name2, const std::string &name3,
    const std::string &name4) const noexcept {
    for (const auto &child : children_) {
        const auto &v = child->GP()->value();
        if (ci_equal(v, name) || ci_equal(v, name2) || ci_equal(v, name3) ||
            ci_equal(v, name4)) {
            return child;
        }
    }
    return null_node;
}

//! @endcond

// ---------------------------------------------------------------------------

/** \brief Instantiate a WKTNode.
 *
 * @param valueIn the name of the node.
 */
WKTNode::WKTNode(const std::string &valueIn)
    : d(internal::make_unique<Private>(valueIn)) {}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
WKTNode::~WKTNode() = default;
//! @endcond

// ---------------------------------------------------------------------------

/** \brief Adds a child to the current node.
 *
 * @param child child to add. This should not be a parent of this node.
 */
void WKTNode::addChild(WKTNodeNNPtr &&child) {
    d->children_.push_back(std::move(child));
}

// ---------------------------------------------------------------------------

/** \brief Return the (occurrence-1)th sub-node of name childName.
 *
 * @param childName name of the child.
 * @param occurrence occurrence index (starting at 0)
 * @return the child, or nullptr.
 */
const WKTNodePtr &WKTNode::lookForChild(const std::string &childName,
                                        int occurrence) const noexcept {
    int occCount = 0;
    for (const auto &child : d->children_) {
        if (ci_equal(child->GP()->value(), childName)) {
            if (occurrence == occCount) {
                return child;
            }
            occCount++;
        }
    }
    return null_node;
}

// ---------------------------------------------------------------------------

/** \brief Return the count of children of given name.
 *
 * @param childName name of the children to look for.
 * @return count
 */
int WKTNode::countChildrenOfName(const std::string &childName) const noexcept {
    int occCount = 0;
    for (const auto &child : d->children_) {
        if (ci_equal(child->GP()->value(), childName)) {
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

//! @cond Doxygen_Suppress
static size_t skipSpace(const std::string &str, size_t start) {
    size_t i = start;
    while (i < str.size() && ::isspace(static_cast<unsigned char>(str[i]))) {
        ++i;
    }
    return i;
}
//! @endcond

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
// As used in examples of OGC 12-063r5
static const std::string startPrintedQuote("\xE2\x80\x9C");
static const std::string endPrintedQuote("\xE2\x80\x9D");
//! @endcond

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

    for (; i < wkt.size() &&
           (inString ||
            (wkt[i] != '[' && wkt[i] != '(' && wkt[i] != ',' && wkt[i] != ']' &&
             wkt[i] != ')' && !::isspace(static_cast<unsigned char>(wkt[i]))));
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
                value += '"';
                i += 2;
                continue;
            }
        } else if (i + 3 <= wkt.size() &&
                   closingStringMarker == endPrintedQuote &&
                   wkt.substr(i, 3) == endPrintedQuote) {
            inString = false;
            closingStringMarker.clear();
            value += '"';
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

    auto node = NN_NO_CHECK(internal::make_unique<WKTNode>(value));

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
        if (i < wkt.size() && wkt[i] == ',') {
            ++i;
            i = skipSpace(wkt, i);
        }
    }
    if (i == wkt.size() || (wkt[i] != ']' && wkt[i] != ')')) {
        throw ParsingException("missing ]");
    }
    indexEnd = i + 1;
    return node;
}
// ---------------------------------------------------------------------------

/** \brief Instantiate a WKTNode hierarchy from a WKT string.
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

//! @cond Doxygen_Suppress
static std::string escapeIfQuotedString(const std::string &str) {
    if (str.size() > 2 && str[0] == '"' && str.back() == '"') {
        std::string res("\"");
        res += replaceAll(str.substr(1, str.size() - 2), "\"", "\"\"");
        res += '"';
        return res;
    } else {
        return str;
    }
}
//! @endcond

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
                str += ',';
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
    std::list<std::string> warningList_{};
    std::vector<double> toWGS84Parameters_{};
    std::string datumPROJ4Grids_{};
    bool esriStyle_ = false;
    DatabaseContextPtr dbContext_{};

    static constexpr int MAX_PROPERTY_SIZE = 1024;
    PropertyMap **properties_{};
    int propertyCount_ = 0;

    Private() { properties_ = new PropertyMap *[MAX_PROPERTY_SIZE]; }

    ~Private() {
        for (int i = 0; i < propertyCount_; i++) {
            delete properties_[i];
        }
        delete[] properties_;
    }
    Private(const Private &) = delete;
    Private &operator=(const Private &) = delete;

    void emitRecoverableWarning(const std::string &errorMsg);

    BaseObjectNNPtr build(const WKTNodeNNPtr &node);

    IdentifierPtr buildId(const WKTNodeNNPtr &node, bool tolerant,
                          bool removeInverseOf);

    PropertyMap &buildProperties(const WKTNodeNNPtr &node,
                                 bool removeInverseOf = false);

    ObjectDomainPtr buildObjectDomain(const WKTNodeNNPtr &node);

    static std::string stripQuotes(const WKTNodeNNPtr &node);

    static double asDouble(const WKTNodeNNPtr &node);

    UnitOfMeasure
    buildUnit(const WKTNodeNNPtr &node,
              UnitOfMeasure::Type type = UnitOfMeasure::Type::UNKNOWN);

    UnitOfMeasure buildUnitInSubNode(
        const WKTNodeNNPtr &node,
        common::UnitOfMeasure::Type type = UnitOfMeasure::Type::UNKNOWN);

    EllipsoidNNPtr buildEllipsoid(const WKTNodeNNPtr &node);

    PrimeMeridianNNPtr
    buildPrimeMeridian(const WKTNodeNNPtr &node,
                       const UnitOfMeasure &defaultAngularUnit);

    static optional<std::string> getAnchor(const WKTNodeNNPtr &node);

    static void parseDynamic(const WKTNodeNNPtr &dynamicNode,
                             double &frameReferenceEpoch,
                             util::optional<std::string> &modelName);

    GeodeticReferenceFrameNNPtr
    buildGeodeticReferenceFrame(const WKTNodeNNPtr &node,
                                const PrimeMeridianNNPtr &primeMeridian,
                                const WKTNodeNNPtr &dynamicNode);

    DatumEnsembleNNPtr buildDatumEnsemble(const WKTNodeNNPtr &node,
                                          const PrimeMeridianPtr &primeMeridian,
                                          bool expectEllipsoid);

    MeridianNNPtr buildMeridian(const WKTNodeNNPtr &node);
    CoordinateSystemAxisNNPtr buildAxis(const WKTNodeNNPtr &node,
                                        const UnitOfMeasure &unitIn,
                                        const UnitOfMeasure::Type &unitType,
                                        bool isGeocentric,
                                        int expectedOrderNum);

    CoordinateSystemNNPtr buildCS(const WKTNodeNNPtr &node, /* maybe null */
                                  const WKTNodeNNPtr &parentNode,
                                  const UnitOfMeasure &defaultAngularUnit);

    GeodeticCRSNNPtr buildGeodeticCRS(const WKTNodeNNPtr &node);

    CRSNNPtr buildDerivedGeodeticCRS(const WKTNodeNNPtr &node);

    static UnitOfMeasure
    guessUnitForParameter(const std::string &paramName,
                          const UnitOfMeasure &defaultLinearUnit,
                          const UnitOfMeasure &defaultAngularUnit);

    void consumeParameters(const WKTNodeNNPtr &node, bool isAbridged,
                           std::vector<OperationParameterNNPtr> &parameters,
                           std::vector<ParameterValueNNPtr> &values,
                           const UnitOfMeasure &defaultLinearUnit,
                           const UnitOfMeasure &defaultAngularUnit);

    static void addExtensionProj4ToProp(const WKTNode::Private *nodeP,
                                        PropertyMap &props);

    ConversionNNPtr buildConversion(const WKTNodeNNPtr &node,
                                    const UnitOfMeasure &defaultLinearUnit,
                                    const UnitOfMeasure &defaultAngularUnit);

    static bool hasWebMercPROJ4String(const WKTNodeNNPtr &projCRSNode,
                                      const WKTNodeNNPtr &projectionNode);

    static std::string projectionGetParameter(const WKTNodeNNPtr &projCRSNode,
                                              const char *paramName);

    ConversionNNPtr buildProjection(const WKTNodeNNPtr &projCRSNode,
                                    const WKTNodeNNPtr &projectionNode,
                                    const UnitOfMeasure &defaultLinearUnit,
                                    const UnitOfMeasure &defaultAngularUnit);

    ConversionNNPtr
    buildProjectionStandard(const WKTNodeNNPtr &projCRSNode,
                            const WKTNodeNNPtr &projectionNode,
                            const UnitOfMeasure &defaultLinearUnit,
                            const UnitOfMeasure &defaultAngularUnit);

    ConversionNNPtr
    buildProjectionFromESRI(const WKTNodeNNPtr &projCRSNode,
                            const WKTNodeNNPtr &projectionNode,
                            const UnitOfMeasure &defaultLinearUnit,
                            const UnitOfMeasure &defaultAngularUnit);

    ProjectedCRSNNPtr buildProjectedCRS(const WKTNodeNNPtr &node);

    VerticalReferenceFrameNNPtr
    buildVerticalReferenceFrame(const WKTNodeNNPtr &node,
                                const WKTNodeNNPtr &dynamicNode);

    TemporalDatumNNPtr buildTemporalDatum(const WKTNodeNNPtr &node);

    EngineeringDatumNNPtr buildEngineeringDatum(const WKTNodeNNPtr &node);

    ParametricDatumNNPtr buildParametricDatum(const WKTNodeNNPtr &node);

    CRSNNPtr buildVerticalCRS(const WKTNodeNNPtr &node);

    DerivedVerticalCRSNNPtr buildDerivedVerticalCRS(const WKTNodeNNPtr &node);

    CompoundCRSNNPtr buildCompoundCRS(const WKTNodeNNPtr &node);

    BoundCRSNNPtr buildBoundCRS(const WKTNodeNNPtr &node);

    TemporalCSNNPtr buildTemporalCS(const WKTNodeNNPtr &parentNode);

    TemporalCRSNNPtr buildTemporalCRS(const WKTNodeNNPtr &node);

    DerivedTemporalCRSNNPtr buildDerivedTemporalCRS(const WKTNodeNNPtr &node);

    EngineeringCRSNNPtr buildEngineeringCRS(const WKTNodeNNPtr &node);

    EngineeringCRSNNPtr
    buildEngineeringCRSFromLocalCS(const WKTNodeNNPtr &node);

    DerivedEngineeringCRSNNPtr
    buildDerivedEngineeringCRS(const WKTNodeNNPtr &node);

    ParametricCSNNPtr buildParametricCS(const WKTNodeNNPtr &parentNode);

    ParametricCRSNNPtr buildParametricCRS(const WKTNodeNNPtr &node);

    DerivedParametricCRSNNPtr
    buildDerivedParametricCRS(const WKTNodeNNPtr &node);

    DerivedProjectedCRSNNPtr buildDerivedProjectedCRS(const WKTNodeNNPtr &node);

    CRSPtr buildCRS(const WKTNodeNNPtr &node);

    TransformationNNPtr buildCoordinateOperation(const WKTNodeNNPtr &node);

    ConcatenatedOperationNNPtr
    buildConcatenatedOperation(const WKTNodeNNPtr &node);
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
std::list<std::string> WKTParser::warningList() const {
    return d->warningList_;
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
void WKTParser::Private::emitRecoverableWarning(const std::string &errorMsg) {
    if (strict_) {
        throw ParsingException(errorMsg);
    } else {
        warningList_.push_back(errorMsg);
    }
}

// ---------------------------------------------------------------------------

static double asDouble(const std::string &val) { return c_locale_stod(val); }

// ---------------------------------------------------------------------------

PROJ_NO_RETURN static void ThrowNotEnoughChildren(const std::string &nodeName) {
    throw ParsingException(
        concat("not enough children in ", nodeName, " node"));
}

// ---------------------------------------------------------------------------

PROJ_NO_RETURN static void
ThrowNotRequiredNumberOfChildren(const std::string &nodeName) {
    throw ParsingException(
        concat("not required number of children in ", nodeName, " node"));
}

// ---------------------------------------------------------------------------

PROJ_NO_RETURN static void ThrowMissing(const std::string &nodeName) {
    throw ParsingException(concat("missing ", nodeName, " node"));
}

// ---------------------------------------------------------------------------

PROJ_NO_RETURN static void
ThrowNotExpectedCSType(const std::string &expectedCSType) {
    throw ParsingException(concat("CS node is not of type ", expectedCSType));
}

// ---------------------------------------------------------------------------

static ParsingException buildRethrow(const char *funcName,
                                     const std::exception &e) {
    std::string res(funcName);
    res += ": ";
    res += e.what();
    return ParsingException(res);
}

// ---------------------------------------------------------------------------

std::string WKTParser::Private::stripQuotes(const WKTNodeNNPtr &node) {
    return ::stripQuotes(node->GP()->value());
}

// ---------------------------------------------------------------------------

double WKTParser::Private::asDouble(const WKTNodeNNPtr &node) {
    return io::asDouble(node->GP()->value());
}

// ---------------------------------------------------------------------------

IdentifierPtr WKTParser::Private::buildId(const WKTNodeNNPtr &node,
                                          bool tolerant, bool removeInverseOf) {
    const auto *nodeP = node->GP();
    const auto &nodeChidren = nodeP->children();
    if (nodeChidren.size() >= 2) {
        auto codeSpace = stripQuotes(nodeChidren[0]);
        if (removeInverseOf && starts_with(codeSpace, "INVERSE(") &&
            codeSpace.back() == ')') {
            codeSpace = codeSpace.substr(strlen("INVERSE("));
            codeSpace.resize(codeSpace.size() - 1);
        }
        auto code = stripQuotes(nodeChidren[1]);
        auto &citationNode = nodeP->lookForChild(WKTConstants::CITATION);
        auto &uriNode = nodeP->lookForChild(WKTConstants::URI);
        PropertyMap propertiesId;
        propertiesId.set(Identifier::CODESPACE_KEY, codeSpace);
        bool authoritySet = false;
        /*if (!isNull(citationNode))*/ {
            const auto *citationNodeP = citationNode->GP();
            if (citationNodeP->childrenSize() == 1) {
                authoritySet = true;
                propertiesId.set(Identifier::AUTHORITY_KEY,
                                 stripQuotes(citationNodeP->children()[0]));
            }
        }
        if (!authoritySet) {
            propertiesId.set(Identifier::AUTHORITY_KEY, codeSpace);
        }
        /*if (!isNull(uriNode))*/ {
            const auto *uriNodeP = uriNode->GP();
            if (uriNodeP->childrenSize() == 1) {
                propertiesId.set(Identifier::URI_KEY,
                                 stripQuotes(uriNodeP->children()[0]));
            }
        }
        if (nodeChidren.size() >= 3 &&
            nodeChidren[2]->GP()->childrenSize() == 0) {
            auto version = stripQuotes(nodeChidren[2]);
            propertiesId.set(Identifier::VERSION_KEY, version);
        }
        return Identifier::create(code, propertiesId);
    } else if (strict_ || !tolerant) {
        ThrowNotEnoughChildren(nodeP->value());
    } else {
        std::string msg("not enough children in ");
        msg += nodeP->value();
        msg += " node";
        warningList_.emplace_back(std::move(msg));
    }
    return nullptr;
}

// ---------------------------------------------------------------------------

PropertyMap &WKTParser::Private::buildProperties(const WKTNodeNNPtr &node,
                                                 bool removeInverseOf) {

    if (propertyCount_ == MAX_PROPERTY_SIZE) {
        throw ParsingException("MAX_PROPERTY_SIZE reached");
    }
    properties_[propertyCount_] = new PropertyMap();
    auto &&properties = properties_[propertyCount_];
    propertyCount_++;

    std::string authNameFromAlias;
    std::string codeFromAlias;
    const auto *nodeP = node->GP();
    const auto &nodeChildren = nodeP->children();
    if (!nodeChildren.empty()) {
        const auto &nodeName(nodeP->value());
        auto name(stripQuotes(nodeChildren[0]));
        if (removeInverseOf && starts_with(name, "Inverse of ")) {
            name = name.substr(strlen("Inverse of "));
        }

        if (ends_with(name, " (deprecated)")) {
            name.resize(name.size() - strlen(" (deprecated)"));
            properties->set(common::IdentifiedObject::DEPRECATED_KEY, true);
        }

        const char *tableNameForAlias = nullptr;
        if (ci_equal(nodeName, WKTConstants::GEOGCS)) {
            if (starts_with(name, "GCS_")) {
                esriStyle_ = true;
                if (name == "GCS_WGS_1984") {
                    name = "WGS 84";
                } else {
                    tableNameForAlias = "geodetic_crs";
                }
            }
        } else if (esriStyle_ && ci_equal(nodeName, WKTConstants::SPHEROID)) {
            if (name == "WGS_1984") {
                name = "WGS 84";
                authNameFromAlias = Identifier::EPSG;
                codeFromAlias = "7030";
            } else {
                tableNameForAlias = "ellipsoid";
            }
        }

        if (dbContext_ && tableNameForAlias) {
            std::string outTableName;
            auto authFactory = AuthorityFactory::create(NN_NO_CHECK(dbContext_),
                                                        std::string());
            auto officialName = authFactory->getOfficialNameFromAlias(
                name, tableNameForAlias, "ESRI", false, outTableName,
                authNameFromAlias, codeFromAlias);
            if (!officialName.empty()) {
                name = officialName;

                // Clearing authority for geodetic_crs because of
                // potential axis order mismatch.
                if (strcmp(tableNameForAlias, "geodetic_crs") == 0) {
                    authNameFromAlias.clear();
                    codeFromAlias.clear();
                }
            }
        }

        properties->set(IdentifiedObject::NAME_KEY, name);
    }

    auto identifiers = ArrayOfBaseObject::create();
    for (const auto &subNode : nodeChildren) {
        const auto &subNodeName(subNode->GP()->value());
        if (ci_equal(subNodeName, WKTConstants::ID) ||
            ci_equal(subNodeName, WKTConstants::AUTHORITY)) {
            auto id = buildId(subNode, true, removeInverseOf);
            if (id) {
                identifiers->add(NN_NO_CHECK(id));
            }
        }
    }
    if (identifiers->empty() && !authNameFromAlias.empty()) {
        identifiers->add(Identifier::create(
            codeFromAlias,
            PropertyMap()
                .set(Identifier::CODESPACE_KEY, authNameFromAlias)
                .set(Identifier::AUTHORITY_KEY, authNameFromAlias)));
    }
    if (!identifiers->empty()) {
        properties->set(IdentifiedObject::IDENTIFIERS_KEY, identifiers);
    }

    auto &remarkNode = nodeP->lookForChild(WKTConstants::REMARK);
    if (!isNull(remarkNode)) {
        const auto &remarkChildren = remarkNode->GP()->children();
        if (remarkChildren.size() == 1) {
            properties->set(IdentifiedObject::REMARKS_KEY,
                            stripQuotes(remarkChildren[0]));
        } else {
            ThrowNotRequiredNumberOfChildren(remarkNode->GP()->value());
        }
    }

    ArrayOfBaseObjectNNPtr array = ArrayOfBaseObject::create();
    for (const auto &subNode : nodeP->children()) {
        const auto &subNodeName(subNode->GP()->value());
        if (ci_equal(subNodeName, WKTConstants::USAGE)) {
            auto objectDomain = buildObjectDomain(subNode);
            if (!objectDomain) {
                throw ParsingException(
                    concat("missing children in ", subNodeName, " node"));
            }
            array->add(NN_NO_CHECK(objectDomain));
        }
    }
    if (!array->empty()) {
        properties->set(ObjectUsage::OBJECT_DOMAIN_KEY, array);
    } else {
        auto objectDomain = buildObjectDomain(node);
        if (objectDomain) {
            properties->set(ObjectUsage::OBJECT_DOMAIN_KEY,
                            NN_NO_CHECK(objectDomain));
        }
    }

    auto &versionNode = nodeP->lookForChild(WKTConstants::VERSION);
    if (!isNull(versionNode)) {
        const auto &versionChildren = versionNode->GP()->children();
        if (versionChildren.size() == 1) {
            properties->set(CoordinateOperation::OPERATION_VERSION_KEY,
                            stripQuotes(versionChildren[0]));
        } else {
            ThrowNotRequiredNumberOfChildren(versionNode->GP()->value());
        }
    }

    return *properties;
}

// ---------------------------------------------------------------------------

ObjectDomainPtr
WKTParser::Private::buildObjectDomain(const WKTNodeNNPtr &node) {

    const auto *nodeP = node->GP();
    auto &scopeNode = nodeP->lookForChild(WKTConstants::SCOPE);
    auto &areaNode = nodeP->lookForChild(WKTConstants::AREA);
    auto &bboxNode = nodeP->lookForChild(WKTConstants::BBOX);
    auto &verticalExtentNode =
        nodeP->lookForChild(WKTConstants::VERTICALEXTENT);
    auto &temporalExtentNode = nodeP->lookForChild(WKTConstants::TIMEEXTENT);
    if (!isNull(scopeNode) || !isNull(areaNode) || !isNull(bboxNode) ||
        !isNull(verticalExtentNode) || !isNull(temporalExtentNode)) {
        optional<std::string> scope;
        const auto *scopeNodeP = scopeNode->GP();
        const auto &scopeChildren = scopeNodeP->children();
        if (scopeChildren.size() == 1) {
            scope = stripQuotes(scopeChildren[0]);
        }
        ExtentPtr extent;
        if (!isNull(areaNode) || !isNull(bboxNode)) {
            util::optional<std::string> description;
            std::vector<GeographicExtentNNPtr> geogExtent;
            std::vector<VerticalExtentNNPtr> verticalExtent;
            std::vector<TemporalExtentNNPtr> temporalExtent;
            if (!isNull(areaNode)) {
                const auto &areaChildren = areaNode->GP()->children();
                if (areaChildren.size() == 1) {
                    description = stripQuotes(areaChildren[0]);
                } else {
                    ThrowNotRequiredNumberOfChildren(areaNode->GP()->value());
                }
            }
            if (!isNull(bboxNode)) {
                const auto &bboxChildren = bboxNode->GP()->children();
                if (bboxChildren.size() == 4) {
                    try {
                        double south = asDouble(bboxChildren[0]);
                        double west = asDouble(bboxChildren[1]);
                        double north = asDouble(bboxChildren[2]);
                        double east = asDouble(bboxChildren[3]);
                        auto bbox = GeographicBoundingBox::create(west, south,
                                                                  east, north);
                        geogExtent.emplace_back(bbox);
                    } catch (const std::exception &) {
                        throw ParsingException(concat("not 4 double values in ",
                                                      bboxNode->GP()->value(),
                                                      " node"));
                    }
                } else {
                    ThrowNotRequiredNumberOfChildren(bboxNode->GP()->value());
                }
            }

            if (!isNull(verticalExtentNode)) {
                const auto &verticalExtentChildren =
                    verticalExtentNode->GP()->children();
                const auto verticalExtentChildrenSize =
                    verticalExtentChildren.size();
                if (verticalExtentChildrenSize == 2 ||
                    verticalExtentChildrenSize == 3) {
                    double min;
                    double max;
                    try {
                        min = asDouble(verticalExtentChildren[0]);
                        max = asDouble(verticalExtentChildren[1]);
                    } catch (const std::exception &) {
                        throw ParsingException(
                            concat("not 2 double values in ",
                                   verticalExtentNode->GP()->value(), " node"));
                    }
                    UnitOfMeasure unit = UnitOfMeasure::METRE;
                    if (verticalExtentChildrenSize == 3) {
                        unit = buildUnit(verticalExtentChildren[2],
                                         UnitOfMeasure::Type::LINEAR);
                    }
                    verticalExtent.emplace_back(VerticalExtent::create(
                        min, max, util::nn_make_shared<UnitOfMeasure>(unit)));
                } else {
                    ThrowNotRequiredNumberOfChildren(
                        verticalExtentNode->GP()->value());
                }
            }

            if (!isNull(temporalExtentNode)) {
                const auto &temporalExtentChildren =
                    temporalExtentNode->GP()->children();
                if (temporalExtentChildren.size() == 2) {
                    temporalExtent.emplace_back(TemporalExtent::create(
                        stripQuotes(temporalExtentChildren[0]),
                        stripQuotes(temporalExtentChildren[1])));
                } else {
                    ThrowNotRequiredNumberOfChildren(
                        temporalExtentNode->GP()->value());
                }
            }
            extent = Extent::create(description, geogExtent, verticalExtent,
                                    temporalExtent)
                         .as_nullable();
        }
        return ObjectDomain::create(scope, extent).as_nullable();
    }

    return nullptr;
}

// ---------------------------------------------------------------------------

UnitOfMeasure WKTParser::Private::buildUnit(const WKTNodeNNPtr &node,
                                            UnitOfMeasure::Type type) {
    const auto *nodeP = node->GP();
    const auto &children = nodeP->children();
    if ((type != UnitOfMeasure::Type::TIME && children.size() < 2) ||
        (type == UnitOfMeasure::Type::TIME && children.size() < 1)) {
        ThrowNotEnoughChildren(nodeP->value());
    }
    try {
        std::string unitName(stripQuotes(children[0]));
        PropertyMap properties(buildProperties(node));
        auto &idNode =
            nodeP->lookForChild(WKTConstants::ID, WKTConstants::AUTHORITY);
        if (!isNull(idNode) && idNode->GP()->childrenSize() < 2) {
            emitRecoverableWarning("not enough children in " +
                                   idNode->GP()->value() + " node");
        }
        const bool hasValidIdNode =
            !isNull(idNode) && idNode->GP()->childrenSize() >= 2;

        const auto &idNodeChildren(idNode->GP()->children());
        std::string codeSpace(hasValidIdNode ? stripQuotes(idNodeChildren[0])
                                             : std::string());
        std::string code(hasValidIdNode ? stripQuotes(idNodeChildren[1])
                                        : std::string());

        bool queryDb = true;
        if (type == UnitOfMeasure::Type::UNKNOWN) {
            if (ci_equal(unitName, "METER") || ci_equal(unitName, "METRE")) {
                type = UnitOfMeasure::Type::LINEAR;
                unitName = "metre";
                if (codeSpace.empty()) {
                    codeSpace = Identifier::EPSG;
                    code = "9001";
                    queryDb = false;
                }
            } else if (ci_equal(unitName, "DEGREE") ||
                       ci_equal(unitName, "GRAD")) {
                type = UnitOfMeasure::Type::ANGULAR;
            }
        }

        if (esriStyle_ && dbContext_ && queryDb) {
            std::string outTableName;
            std::string authNameFromAlias;
            std::string codeFromAlias;
            auto authFactory = AuthorityFactory::create(NN_NO_CHECK(dbContext_),
                                                        std::string());
            auto officialName = authFactory->getOfficialNameFromAlias(
                unitName, "unit_of_measure", "ESRI", false, outTableName,
                authNameFromAlias, codeFromAlias);
            if (!officialName.empty()) {
                unitName = officialName;
                codeSpace = authNameFromAlias;
                code = codeFromAlias;
            }
        }

        double convFactor = children.size() >= 2 ? asDouble(children[1]) : 0.0;
        constexpr double US_FOOT_CONV_FACTOR = 12.0 / 39.37;
        constexpr double REL_ERROR = 1e-10;
        // Fix common rounding errors
        if (std::fabs(convFactor - UnitOfMeasure::DEGREE.conversionToSI()) <
            REL_ERROR * convFactor) {
            convFactor = UnitOfMeasure::DEGREE.conversionToSI();
        } else if (std::fabs(convFactor - US_FOOT_CONV_FACTOR) <
                   REL_ERROR * convFactor) {
            convFactor = US_FOOT_CONV_FACTOR;
        }

        return UnitOfMeasure(unitName, convFactor, type, codeSpace, code);
    } catch (const std::exception &e) {
        throw buildRethrow(__FUNCTION__, e);
    }
}

// ---------------------------------------------------------------------------

// node here is a parent node, not a UNIT/LENGTHUNIT/ANGLEUNIT/TIMEUNIT/... node
UnitOfMeasure WKTParser::Private::buildUnitInSubNode(const WKTNodeNNPtr &node,
                                                     UnitOfMeasure::Type type) {
    const auto *nodeP = node->GP();
    {
        auto &unitNode = nodeP->lookForChild(WKTConstants::LENGTHUNIT);
        if (!isNull(unitNode)) {
            return buildUnit(unitNode, UnitOfMeasure::Type::LINEAR);
        }
    }

    {
        auto &unitNode = nodeP->lookForChild(WKTConstants::ANGLEUNIT);
        if (!isNull(unitNode)) {
            return buildUnit(unitNode, UnitOfMeasure::Type::ANGULAR);
        }
    }

    {
        auto &unitNode = nodeP->lookForChild(WKTConstants::SCALEUNIT);
        if (!isNull(unitNode)) {
            return buildUnit(unitNode, UnitOfMeasure::Type::SCALE);
        }
    }

    {
        auto &unitNode = nodeP->lookForChild(WKTConstants::TIMEUNIT);
        if (!isNull(unitNode)) {
            return buildUnit(unitNode, UnitOfMeasure::Type::TIME);
        }
    }
    {
        auto &unitNode = nodeP->lookForChild(WKTConstants::TEMPORALQUANTITY);
        if (!isNull(unitNode)) {
            return buildUnit(unitNode, UnitOfMeasure::Type::TIME);
        }
    }

    {
        auto &unitNode = nodeP->lookForChild(WKTConstants::PARAMETRICUNIT);
        if (!isNull(unitNode)) {
            return buildUnit(unitNode, UnitOfMeasure::Type::PARAMETRIC);
        }
    }

    {
        auto &unitNode = nodeP->lookForChild(WKTConstants::UNIT);
        if (!isNull(unitNode)) {
            return buildUnit(unitNode, type);
        }
    }

    return UnitOfMeasure::NONE;
}

// ---------------------------------------------------------------------------

EllipsoidNNPtr WKTParser::Private::buildEllipsoid(const WKTNodeNNPtr &node) {
    const auto *nodeP = node->GP();
    const auto &children = nodeP->children();
    if (children.size() < 3) {
        ThrowNotEnoughChildren(nodeP->value());
    }
    try {
        UnitOfMeasure unit =
            buildUnitInSubNode(node, UnitOfMeasure::Type::LINEAR);
        if (unit == UnitOfMeasure::NONE) {
            unit = UnitOfMeasure::METRE;
        }
        Length semiMajorAxis(asDouble(children[1]), unit);
        Scale invFlattening(asDouble(children[2]));
        const auto celestialBody(
            Ellipsoid::guessBodyName(dbContext_, semiMajorAxis.getSIValue()));
        if (invFlattening.getSIValue() == 0) {
            return Ellipsoid::createSphere(buildProperties(node), semiMajorAxis,
                                           celestialBody);
        } else {
            return Ellipsoid::createFlattenedSphere(
                buildProperties(node), semiMajorAxis, invFlattening,
                celestialBody);
        }
    } catch (const std::exception &e) {
        throw buildRethrow(__FUNCTION__, e);
    }
}

// ---------------------------------------------------------------------------

PrimeMeridianNNPtr WKTParser::Private::buildPrimeMeridian(
    const WKTNodeNNPtr &node, const UnitOfMeasure &defaultAngularUnit) {
    const auto *nodeP = node->GP();
    const auto &children = nodeP->children();
    if (children.size() < 2) {
        ThrowNotEnoughChildren(nodeP->value());
    }
    auto name = stripQuotes(children[0]);
    UnitOfMeasure unit = buildUnitInSubNode(node, UnitOfMeasure::Type::ANGULAR);
    if (unit == UnitOfMeasure::NONE) {
        unit = defaultAngularUnit;
        if (unit == UnitOfMeasure::NONE) {
            unit = UnitOfMeasure::DEGREE;
        }
    }
    try {
        double angleValue = asDouble(children[1]);

        // Correct for GDAL WKT1 departure
        if (name == "Paris" && std::fabs(angleValue - 2.33722917) < 1e-8 &&
            unit == UnitOfMeasure::GRAD) {
            angleValue = 2.5969213;
        }

        Angle angle(angleValue, unit);
        return PrimeMeridian::create(buildProperties(node), angle);
    } catch (const std::exception &e) {
        throw buildRethrow(__FUNCTION__, e);
    }
}

// ---------------------------------------------------------------------------

optional<std::string> WKTParser::Private::getAnchor(const WKTNodeNNPtr &node) {

    auto &anchorNode = node->GP()->lookForChild(WKTConstants::ANCHOR);
    if (anchorNode->GP()->childrenSize() == 1) {
        return optional<std::string>(
            stripQuotes(anchorNode->GP()->children()[0]));
    }
    return optional<std::string>();
}

// ---------------------------------------------------------------------------

static const PrimeMeridianNNPtr &
fixupPrimeMeridan(const EllipsoidNNPtr &ellipsoid,
                  const PrimeMeridianNNPtr &pm) {
    return (ellipsoid->celestialBody() != Ellipsoid::EARTH &&
            pm.get() == PrimeMeridian::GREENWICH.get())
               ? PrimeMeridian::REFERENCE_MERIDIAN
               : pm;
}

// ---------------------------------------------------------------------------

GeodeticReferenceFrameNNPtr WKTParser::Private::buildGeodeticReferenceFrame(
    const WKTNodeNNPtr &node, const PrimeMeridianNNPtr &primeMeridian,
    const WKTNodeNNPtr &dynamicNode) {
    const auto *nodeP = node->GP();
    auto &ellipsoidNode =
        nodeP->lookForChild(WKTConstants::ELLIPSOID, WKTConstants::SPHEROID);
    if (isNull(ellipsoidNode)) {
        ThrowMissing(WKTConstants::ELLIPSOID);
    }
    auto &properties = buildProperties(node);

    // do that before buildEllipsoid() so that esriStyle_ can be set
    auto name = stripQuotes(nodeP->children()[0]);
    if (name == "WGS_1984") {
        properties.set(IdentifiedObject::NAME_KEY,
                       GeodeticReferenceFrame::EPSG_6326->nameStr());
    } else if (starts_with(name, "D_")) {
        esriStyle_ = true;
        const char *tableNameForAlias = nullptr;
        std::string authNameFromAlias;
        std::string codeFromAlias;
        if (name == "D_WGS_1984") {
            name = "World Geodetic System 1984";
            authNameFromAlias = Identifier::EPSG;
            codeFromAlias = "6326";
        } else {
            tableNameForAlias = "geodetic_datum";
        }

        if (dbContext_ && tableNameForAlias) {
            std::string outTableName;
            auto authFactory = AuthorityFactory::create(NN_NO_CHECK(dbContext_),
                                                        std::string());
            auto officialName = authFactory->getOfficialNameFromAlias(
                name, tableNameForAlias, "ESRI", false, outTableName,
                authNameFromAlias, codeFromAlias);
            if (!officialName.empty()) {
                if (primeMeridian->nameStr() !=
                    PrimeMeridian::GREENWICH->nameStr()) {
                    auto nameWithPM =
                        officialName + " (" + primeMeridian->nameStr() + ")";
                    if (dbContext_->isKnownName(nameWithPM, "geodetic_datum")) {
                        officialName = nameWithPM;
                    }
                }
                name = officialName;
            }
        }

        properties.set(IdentifiedObject::NAME_KEY, name);
        if (!authNameFromAlias.empty()) {
            auto identifiers = ArrayOfBaseObject::create();
            identifiers->add(Identifier::create(
                codeFromAlias,
                PropertyMap()
                    .set(Identifier::CODESPACE_KEY, authNameFromAlias)
                    .set(Identifier::AUTHORITY_KEY, authNameFromAlias)));
            properties.set(IdentifiedObject::IDENTIFIERS_KEY, identifiers);
        }
    } else if (name.find('_') != std::string::npos) {
        // Likely coming from WKT1
        if (dbContext_) {
            auto authFactory = AuthorityFactory::create(NN_NO_CHECK(dbContext_),
                                                        std::string());
            auto res = authFactory->createObjectsFromName(
                name, {AuthorityFactory::ObjectType::GEODETIC_REFERENCE_FRAME},
                true, 1);
            bool foundDatumName = false;
            if (!res.empty()) {
                const auto &refDatum = res.front();
                if (metadata::Identifier::isEquivalentName(
                        name.c_str(), refDatum->nameStr().c_str())) {
                    foundDatumName = true;
                    properties.set(IdentifiedObject::NAME_KEY,
                                   refDatum->nameStr());
                    if (!properties.get(Identifier::CODESPACE_KEY) &&
                        refDatum->identifiers().size() == 1) {
                        const auto &id = refDatum->identifiers()[0];
                        auto identifiers = ArrayOfBaseObject::create();
                        identifiers->add(Identifier::create(
                            id->code(), PropertyMap()
                                            .set(Identifier::CODESPACE_KEY,
                                                 *id->codeSpace())
                                            .set(Identifier::AUTHORITY_KEY,
                                                 *id->codeSpace())));
                        properties.set(IdentifiedObject::IDENTIFIERS_KEY,
                                       identifiers);
                    }
                }
            } else {
                // Get official name from database if AUTHORITY is present
                auto &idNode = nodeP->lookForChild(WKTConstants::AUTHORITY);
                if (!isNull(idNode)) {
                    try {
                        auto id = buildId(idNode, true, false);
                        auto authFactory2 = AuthorityFactory::create(
                            NN_NO_CHECK(dbContext_), *id->codeSpace());
                        auto dbDatum =
                            authFactory2->createGeodeticDatum(id->code());
                        foundDatumName = true;
                        properties.set(IdentifiedObject::NAME_KEY,
                                       dbDatum->nameStr());
                    } catch (const std::exception &) {
                    }
                }
            }

            if (!foundDatumName) {
                std::string outTableName;
                std::string authNameFromAlias;
                std::string codeFromAlias;
                auto officialName = authFactory->getOfficialNameFromAlias(
                    name, "geodetic_datum", std::string(), true, outTableName,
                    authNameFromAlias, codeFromAlias);
                if (!officialName.empty()) {
                    properties.set(IdentifiedObject::NAME_KEY, officialName);
                }
            }
        }
    }

    auto ellipsoid = buildEllipsoid(ellipsoidNode);
    const auto &primeMeridianModified =
        fixupPrimeMeridan(ellipsoid, primeMeridian);

    auto &TOWGS84Node = nodeP->lookForChild(WKTConstants::TOWGS84);
    if (!isNull(TOWGS84Node)) {
        const auto &TOWGS84Children = TOWGS84Node->GP()->children();
        const size_t TOWGS84Size = TOWGS84Children.size();
        if (TOWGS84Size == 3 || TOWGS84Size == 7) {
            try {
                for (const auto &child : TOWGS84Children) {
                    toWGS84Parameters_.push_back(asDouble(child));
                }
                for (size_t i = TOWGS84Size; i < 7; ++i) {
                    toWGS84Parameters_.push_back(0.0);
                }
            } catch (const std::exception &) {
                throw ParsingException("Invalid TOWGS84 node");
            }
        } else {
            throw ParsingException("Invalid TOWGS84 node");
        }
    }

    auto &extensionNode = nodeP->lookForChild(WKTConstants::EXTENSION);
    const auto &extensionChildren = extensionNode->GP()->children();
    if (extensionChildren.size() == 2) {
        if (ci_equal(stripQuotes(extensionChildren[0]), "PROJ4_GRIDS")) {
            datumPROJ4Grids_ = stripQuotes(extensionChildren[1]);
        }
    }

    if (!isNull(dynamicNode)) {
        double frameReferenceEpoch = 0.0;
        util::optional<std::string> modelName;
        parseDynamic(dynamicNode, frameReferenceEpoch, modelName);
        return DynamicGeodeticReferenceFrame::create(
            properties, ellipsoid, getAnchor(node), primeMeridianModified,
            common::Measure(frameReferenceEpoch, common::UnitOfMeasure::YEAR),
            modelName);
    }

    return GeodeticReferenceFrame::create(
        properties, ellipsoid, getAnchor(node), primeMeridianModified);
}

// ---------------------------------------------------------------------------

DatumEnsembleNNPtr
WKTParser::Private::buildDatumEnsemble(const WKTNodeNNPtr &node,
                                       const PrimeMeridianPtr &primeMeridian,
                                       bool expectEllipsoid) {
    const auto *nodeP = node->GP();
    auto &ellipsoidNode =
        nodeP->lookForChild(WKTConstants::ELLIPSOID, WKTConstants::SPHEROID);
    if (expectEllipsoid && isNull(ellipsoidNode)) {
        ThrowMissing(WKTConstants::ELLIPSOID);
    }

    std::vector<DatumNNPtr> datums;
    for (const auto &subNode : nodeP->children()) {
        if (ci_equal(subNode->GP()->value(), WKTConstants::MEMBER)) {
            if (subNode->GP()->childrenSize() == 0) {
                throw ParsingException("Invalid MEMBER node");
            }
            if (expectEllipsoid) {
                datums.emplace_back(GeodeticReferenceFrame::create(
                    buildProperties(subNode), buildEllipsoid(ellipsoidNode),
                    optional<std::string>(),
                    primeMeridian ? NN_NO_CHECK(primeMeridian)
                                  : PrimeMeridian::GREENWICH));
            } else {
                datums.emplace_back(
                    VerticalReferenceFrame::create(buildProperties(subNode)));
            }
        }
    }

    auto &accuracyNode = nodeP->lookForChild(WKTConstants::ENSEMBLEACCURACY);
    auto &accuracyNodeChildren = accuracyNode->GP()->children();
    if (accuracyNodeChildren.empty()) {
        ThrowMissing(WKTConstants::ENSEMBLEACCURACY);
    }
    auto accuracy =
        PositionalAccuracy::create(accuracyNodeChildren[0]->GP()->value());

    try {
        return DatumEnsemble::create(buildProperties(node), datums, accuracy);
    } catch (const util::Exception &e) {
        throw buildRethrow(__FUNCTION__, e);
    }
}

// ---------------------------------------------------------------------------

MeridianNNPtr WKTParser::Private::buildMeridian(const WKTNodeNNPtr &node) {
    const auto *nodeP = node->GP();
    const auto &children = nodeP->children();
    if (children.size() < 2) {
        ThrowNotEnoughChildren(nodeP->value());
    }
    UnitOfMeasure unit = buildUnitInSubNode(node, UnitOfMeasure::Type::ANGULAR);
    try {
        double angleValue = asDouble(children[0]);
        Angle angle(angleValue, unit);
        return Meridian::create(angle);
    } catch (const std::exception &e) {
        throw buildRethrow(__FUNCTION__, e);
    }
}

// ---------------------------------------------------------------------------

PROJ_NO_RETURN static void ThrowParsingExceptionMissingUNIT() {
    throw ParsingException("buildCS: missing UNIT");
}

// ---------------------------------------------------------------------------

CoordinateSystemAxisNNPtr
WKTParser::Private::buildAxis(const WKTNodeNNPtr &node,
                              const UnitOfMeasure &unitIn,
                              const UnitOfMeasure::Type &unitType,
                              bool isGeocentric, int expectedOrderNum) {
    const auto *nodeP = node->GP();
    const auto &children = nodeP->children();
    if (children.size() < 2) {
        ThrowNotEnoughChildren(nodeP->value());
    }

    auto &orderNode = nodeP->lookForChild(WKTConstants::ORDER);
    if (!isNull(orderNode)) {
        const auto &orderNodeChildren = orderNode->GP()->children();
        if (orderNodeChildren.size() != 1) {
            ThrowNotEnoughChildren(WKTConstants::ORDER);
        }
        const auto &order = orderNodeChildren[0]->GP()->value();
        int orderNum;
        try {
            orderNum = std::stoi(order);
        } catch (const std::exception &) {
            throw ParsingException(
                concat("buildAxis: invalid ORDER value: ", order));
        }
        if (orderNum != expectedOrderNum) {
            throw ParsingException(
                concat("buildAxis: did not get expected ORDER value: ", order));
        }
    }

    // The axis designation in WK2 can be: "name", "(abbrev)" or "name
    // (abbrev)"
    std::string axisDesignation(stripQuotes(children[0]));
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
    const std::string &dirString = children[1]->GP()->value();
    const AxisDirection *direction = AxisDirection::valueOf(dirString);

    // WKT2, geocentric CS: axis names are omitted
    if (axisName.empty()) {
        if (direction == &AxisDirection::GEOCENTRIC_X &&
            abbreviation == AxisAbbreviation::X) {
            axisName = AxisName::Geocentric_X;
        } else if (direction == &AxisDirection::GEOCENTRIC_Y &&
                   abbreviation == AxisAbbreviation::Y) {
            axisName = AxisName::Geocentric_Y;
        } else if (direction == &AxisDirection::GEOCENTRIC_Z &&
                   abbreviation == AxisAbbreviation::Z) {
            axisName = AxisName::Geocentric_Z;
        }
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
               (dirString == AxisDirectionWKT1::NORTH.toString() ||
                dirString == AxisDirectionWKT1::OTHER.toString())) {
        abbreviation = AxisAbbreviation::Z;
        direction = &AxisDirection::GEOCENTRIC_Z;
    } else if (dirString == AxisDirectionWKT1::OTHER.toString()) {
        direction = &AxisDirection::UNSPECIFIED;
    } else if (!direction &&
               AxisDirectionWKT1::valueOf(toupper(dirString)) != nullptr) {
        direction = AxisDirection::valueOf(tolower(dirString));
    }

    if (!direction) {
        throw ParsingException(
            concat("unhandled axis direction: ", children[1]->GP()->value()));
    }
    UnitOfMeasure unit(buildUnitInSubNode(node));
    if (unit == UnitOfMeasure::NONE) {
        // If no unit in the AXIS node, use the one potentially coming from
        // the CS.
        unit = unitIn;
        if (unit == UnitOfMeasure::NONE &&
            unitType != UnitOfMeasure::Type::NONE &&
            unitType != UnitOfMeasure::Type::TIME) {
            ThrowParsingExceptionMissingUNIT();
        }
    }

    auto &meridianNode = nodeP->lookForChild(WKTConstants::MERIDIAN);

    return CoordinateSystemAxis::create(
        buildProperties(node).set(IdentifiedObject::NAME_KEY, axisName),
        abbreviation, *direction, unit,
        !isNull(meridianNode) ? buildMeridian(meridianNode).as_nullable()
                              : nullptr);
}

// ---------------------------------------------------------------------------

static const PropertyMap emptyPropertyMap{};

PROJ_NO_RETURN static void ThrowParsingException(const std::string &msg) {
    throw ParsingException(msg);
}

static ParsingException
buildParsingExceptionInvalidAxisCount(const std::string &csType) {
    return ParsingException(
        concat("buildCS: invalid CS axis count for ", csType));
}

CoordinateSystemNNPtr
WKTParser::Private::buildCS(const WKTNodeNNPtr &node, /* maybe null */
                            const WKTNodeNNPtr &parentNode,
                            const UnitOfMeasure &defaultAngularUnit) {
    bool isGeocentric = false;
    std::string csType;
    const int numberOfAxis =
        parentNode->countChildrenOfName(WKTConstants::AXIS);
    int axisCount = numberOfAxis;
    if (!isNull(node)) {
        const auto *nodeP = node->GP();
        const auto &children = nodeP->children();
        if (children.size() < 2) {
            ThrowNotEnoughChildren(nodeP->value());
        }
        csType = children[0]->GP()->value();
        try {
            axisCount = std::stoi(children[1]->GP()->value());
        } catch (const std::exception &) {
            ThrowParsingException(concat("buildCS: invalid CS axis count: ",
                                         children[1]->GP()->value()));
        }
    } else {
        const char *csTypeCStr = "";
        const auto &parentNodeName = parentNode->GP()->value();
        if (ci_equal(parentNodeName, WKTConstants::GEOCCS)) {
            csTypeCStr = "Cartesian";
            isGeocentric = true;
            if (axisCount == 0) {
                auto unit =
                    buildUnitInSubNode(parentNode, UnitOfMeasure::Type::LINEAR);
                if (unit == UnitOfMeasure::NONE) {
                    ThrowParsingExceptionMissingUNIT();
                }
                return CartesianCS::createGeocentric(unit);
            }
        } else if (ci_equal(parentNodeName, WKTConstants::GEOGCS)) {
            csTypeCStr = "Ellipsoidal";
            if (axisCount == 0) {
                // Missing axis with GEOGCS ? Presumably Long/Lat order
                // implied
                auto unit = buildUnitInSubNode(parentNode,
                                               UnitOfMeasure::Type::ANGULAR);
                if (unit == UnitOfMeasure::NONE) {
                    ThrowParsingExceptionMissingUNIT();
                }
                // WKT1 --> long/lat
                return EllipsoidalCS::createLongitudeLatitude(unit);
            }
        } else if (ci_equal(parentNodeName, WKTConstants::BASEGEODCRS) ||
                   ci_equal(parentNodeName, WKTConstants::BASEGEOGCRS)) {
            csTypeCStr = "Ellipsoidal";
            if (axisCount == 0) {
                auto unit = buildUnitInSubNode(parentNode,
                                               UnitOfMeasure::Type::ANGULAR);
                if (unit == UnitOfMeasure::NONE) {
                    unit = defaultAngularUnit;
                }
                // WKT2 --> presumably lat/long
                return EllipsoidalCS::createLatitudeLongitude(unit);
            }
        } else if (ci_equal(parentNodeName, WKTConstants::PROJCS) ||
                   ci_equal(parentNodeName, WKTConstants::BASEPROJCRS) ||
                   ci_equal(parentNodeName, WKTConstants::BASEENGCRS)) {
            csTypeCStr = "Cartesian";
            if (axisCount == 0) {
                auto unit =
                    buildUnitInSubNode(parentNode, UnitOfMeasure::Type::LINEAR);
                if (unit == UnitOfMeasure::NONE) {
                    if (ci_equal(parentNodeName, WKTConstants::PROJCS)) {
                        ThrowParsingExceptionMissingUNIT();
                    } else {
                        unit = UnitOfMeasure::METRE;
                    }
                }
                return CartesianCS::createEastingNorthing(unit);
            }
        } else if (ci_equal(parentNodeName, WKTConstants::VERT_CS) ||
                   ci_equal(parentNodeName, WKTConstants::BASEVERTCRS)) {
            csTypeCStr = "vertical";
            if (axisCount == 0) {
                auto unit =
                    buildUnitInSubNode(parentNode, UnitOfMeasure::Type::LINEAR);
                if (unit == UnitOfMeasure::NONE) {
                    if (ci_equal(parentNodeName, WKTConstants::VERT_CS)) {
                        ThrowParsingExceptionMissingUNIT();
                    } else {
                        unit = UnitOfMeasure::METRE;
                    }
                }
                return VerticalCS::createGravityRelatedHeight(unit);
            }
        } else if (ci_equal(parentNodeName, WKTConstants::LOCAL_CS)) {
            if (axisCount == 0) {
                auto unit =
                    buildUnitInSubNode(parentNode, UnitOfMeasure::Type::LINEAR);
                if (unit == UnitOfMeasure::NONE) {
                    unit = UnitOfMeasure::METRE;
                }
                return CartesianCS::createEastingNorthing(unit);
            } else if (axisCount == 1) {
                csTypeCStr = "vertical";
            } else if (axisCount == 2) {
                csTypeCStr = "Cartesian";
            } else {
                throw ParsingException(
                    "buildCS: unexpected AXIS count for LOCAL_CS");
            }
        } else if (ci_equal(parentNodeName, WKTConstants::BASEPARAMCRS)) {
            csTypeCStr = "parametric";
            if (axisCount == 0) {
                auto unit =
                    buildUnitInSubNode(parentNode, UnitOfMeasure::Type::LINEAR);
                if (unit == UnitOfMeasure::NONE) {
                    unit = UnitOfMeasure("unknown", 1,
                                         UnitOfMeasure::Type::PARAMETRIC);
                }
                return ParametricCS::create(
                    emptyPropertyMap,
                    CoordinateSystemAxis::create(
                        PropertyMap().set(IdentifiedObject::NAME_KEY,
                                          "unknown parametric"),
                        std::string(), AxisDirection::UNSPECIFIED, unit));
            }
        } else if (ci_equal(parentNodeName, WKTConstants::BASETIMECRS)) {
            csTypeCStr = "temporal";
            if (axisCount == 0) {
                auto unit =
                    buildUnitInSubNode(parentNode, UnitOfMeasure::Type::TIME);
                if (unit == UnitOfMeasure::NONE) {
                    unit =
                        UnitOfMeasure("unknown", 1, UnitOfMeasure::Type::TIME);
                }
                return DateTimeTemporalCS::create(
                    emptyPropertyMap,
                    CoordinateSystemAxis::create(
                        PropertyMap().set(IdentifiedObject::NAME_KEY,
                                          "unknown temporal"),
                        std::string(), AxisDirection::FUTURE, unit));
            }
        } else {
            // Shouldn't happen normally
            throw ParsingException(
                concat("buildCS: unexpected parent node: ", parentNodeName));
        }
        csType = csTypeCStr;
    }

    if (axisCount != 1 && axisCount != 2 && axisCount != 3) {
        throw buildParsingExceptionInvalidAxisCount(csType);
    }
    if (numberOfAxis != axisCount) {
        throw ParsingException("buildCS: declared number of axis by CS node "
                               "and number of AXIS are inconsistent");
    }

    const auto unitType =
        ci_equal(csType, "ellipsoidal")
            ? UnitOfMeasure::Type::ANGULAR
            : ci_equal(csType, "ordinal")
                  ? UnitOfMeasure::Type::NONE
                  : ci_equal(csType, "parametric")
                        ? UnitOfMeasure::Type::PARAMETRIC
                        : ci_equal(csType, "Cartesian") ||
                                  ci_equal(csType, "vertical")
                              ? UnitOfMeasure::Type::LINEAR
                              : (ci_equal(csType, "temporal") ||
                                 ci_equal(csType, "TemporalDateTime") ||
                                 ci_equal(csType, "TemporalCount") ||
                                 ci_equal(csType, "TemporalMeasure"))
                                    ? UnitOfMeasure::Type::TIME
                                    : UnitOfMeasure::Type::UNKNOWN;
    UnitOfMeasure unit = buildUnitInSubNode(parentNode, unitType);

    std::vector<CoordinateSystemAxisNNPtr> axisList;
    for (int i = 0; i < axisCount; i++) {
        axisList.emplace_back(
            buildAxis(parentNode->GP()->lookForChild(WKTConstants::AXIS, i),
                      unit, unitType, isGeocentric, i + 1));
    };

    const PropertyMap &csMap = emptyPropertyMap;
    if (ci_equal(csType, "ellipsoidal")) {
        if (axisCount == 2) {
            return EllipsoidalCS::create(csMap, axisList[0], axisList[1]);
        } else if (axisCount == 3) {
            return EllipsoidalCS::create(csMap, axisList[0], axisList[1],
                                         axisList[2]);
        }
    } else if (ci_equal(csType, "Cartesian")) {
        if (axisCount == 2) {
            return CartesianCS::create(csMap, axisList[0], axisList[1]);
        } else if (axisCount == 3) {
            return CartesianCS::create(csMap, axisList[0], axisList[1],
                                       axisList[2]);
        }
    } else if (ci_equal(csType, "vertical")) {
        if (axisCount == 1) {
            return VerticalCS::create(csMap, axisList[0]);
        }
    } else if (ci_equal(csType, "spherical")) {
        if (axisCount == 3) {
            return SphericalCS::create(csMap, axisList[0], axisList[1],
                                       axisList[2]);
        }
    } else if (ci_equal(csType, "ordinal")) { // WKT2-2018
        return OrdinalCS::create(csMap, axisList);
    } else if (ci_equal(csType, "parametric")) {
        if (axisCount == 1) {
            return ParametricCS::create(csMap, axisList[0]);
        }
    } else if (ci_equal(csType, "temporal")) { // WKT2-2015
        if (axisCount == 1) {
            return DateTimeTemporalCS::create(
                csMap,
                axisList[0]); // FIXME: there are 3 possible subtypes of
                              // TemporalCS
        }
    } else if (ci_equal(csType, "TemporalDateTime")) { // WKT2-2018
        if (axisCount == 1) {
            return DateTimeTemporalCS::create(csMap, axisList[0]);
        }
    } else if (ci_equal(csType, "TemporalCount")) { // WKT2-2018
        if (axisCount == 1) {
            return TemporalCountCS::create(csMap, axisList[0]);
        }
    } else if (ci_equal(csType, "TemporalMeasure")) { // WKT2-2018
        if (axisCount == 1) {
            return TemporalMeasureCS::create(csMap, axisList[0]);
        }
    } else {
        throw ParsingException(concat("unhandled CS type: ", csType));
    }
    throw buildParsingExceptionInvalidAxisCount(csType);
}

// ---------------------------------------------------------------------------

void WKTParser::Private::addExtensionProj4ToProp(const WKTNode::Private *nodeP,
                                                 PropertyMap &props) {
    auto &extensionNode = nodeP->lookForChild(WKTConstants::EXTENSION);
    const auto &extensionChildren = extensionNode->GP()->children();
    if (extensionChildren.size() == 2) {
        if (ci_equal(stripQuotes(extensionChildren[0]), "PROJ4")) {
            props.set("EXTENSION_PROJ4", stripQuotes(extensionChildren[1]));
        }
    }
}

// ---------------------------------------------------------------------------

GeodeticCRSNNPtr
WKTParser::Private::buildGeodeticCRS(const WKTNodeNNPtr &node) {
    const auto *nodeP = node->GP();
    auto &datumNode = nodeP->lookForChild(
        WKTConstants::DATUM, WKTConstants::GEODETICDATUM, WKTConstants::TRF);
    auto &ensembleNode = nodeP->lookForChild(WKTConstants::ENSEMBLE);
    if (isNull(datumNode) && isNull(ensembleNode)) {
        throw ParsingException("Missing DATUM or ENSEMBLE node");
    }

    auto &dynamicNode = nodeP->lookForChild(WKTConstants::DYNAMIC);

    auto &csNode = nodeP->lookForChild(WKTConstants::CS_);
    const auto &nodeName = nodeP->value();
    if (isNull(csNode) && !ci_equal(nodeName, WKTConstants::GEOGCS) &&
        !ci_equal(nodeName, WKTConstants::GEOCCS) &&
        !ci_equal(nodeName, WKTConstants::BASEGEODCRS) &&
        !ci_equal(nodeName, WKTConstants::BASEGEOGCRS)) {
        ThrowMissing(WKTConstants::CS_);
    }

    auto &primeMeridianNode =
        nodeP->lookForChild(WKTConstants::PRIMEM, WKTConstants::PRIMEMERIDIAN);
    if (isNull(primeMeridianNode)) {
        // PRIMEM is required in WKT1
        if (ci_equal(nodeName, WKTConstants::GEOGCS) ||
            ci_equal(nodeName, WKTConstants::GEOCCS)) {
            emitRecoverableWarning(nodeName + " should have a PRIMEM node");
        }
    }

    auto angularUnit =
        buildUnitInSubNode(node, ci_equal(nodeName, WKTConstants::GEOGCS)
                                     ? UnitOfMeasure::Type::ANGULAR
                                     : UnitOfMeasure::Type::UNKNOWN);
    if (angularUnit.type() != UnitOfMeasure::Type::ANGULAR) {
        angularUnit = UnitOfMeasure::NONE;
    }

    auto primeMeridian =
        !isNull(primeMeridianNode)
            ? buildPrimeMeridian(primeMeridianNode, angularUnit)
            : PrimeMeridian::GREENWICH;
    if (angularUnit == UnitOfMeasure::NONE) {
        angularUnit = primeMeridian->longitude().unit();
    }

    auto props = buildProperties(node);
    addExtensionProj4ToProp(nodeP, props);

    // No explicit AXIS node ? (WKT1)
    if (isNull(nodeP->lookForChild(WKTConstants::AXIS))) {
        props.set("IMPLICIT_CS", true);
    }

    auto datum =
        !isNull(datumNode)
            ? buildGeodeticReferenceFrame(datumNode, primeMeridian, dynamicNode)
                  .as_nullable()
            : nullptr;
    auto datumEnsemble =
        !isNull(ensembleNode)
            ? buildDatumEnsemble(ensembleNode, primeMeridian, true)
                  .as_nullable()
            : nullptr;
    auto cs = buildCS(csNode, node, angularUnit);
    auto ellipsoidalCS = nn_dynamic_pointer_cast<EllipsoidalCS>(cs);
    if (ellipsoidalCS) {
        assert(!ci_equal(nodeName, WKTConstants::GEOCCS));
        try {
            auto crs = GeographicCRS::create(props, datum, datumEnsemble,
                                             NN_NO_CHECK(ellipsoidalCS));
            // In case of missing CS node, or to check it, query the coordinate
            // system from the DB if possible (typically for the baseCRS of a
            // ProjectedCRS)
            if (!crs->identifiers().empty() && dbContext_) {
                GeographicCRSPtr dbCRS;
                try {
                    const auto &id = crs->identifiers()[0];
                    auto authFactory = AuthorityFactory::create(
                        NN_NO_CHECK(dbContext_), *id->codeSpace());
                    dbCRS = authFactory->createGeographicCRS(id->code())
                                .as_nullable();
                } catch (const util::Exception &) {
                }
                if (dbCRS &&
                    (!isNull(csNode) ||
                     node->countChildrenOfName(WKTConstants::AXIS) != 0) &&
                    !ellipsoidalCS->_isEquivalentTo(
                        dbCRS->coordinateSystem().get(),
                        util::IComparable::Criterion::EQUIVALENT)) {
                    emitRecoverableWarning(
                        "Coordinate system of GeographicCRS in the WKT "
                        "definition is different from the one of the "
                        "authority. Unsetting the identifier to avoid "
                        "confusion");
                    props.unset(Identifier::CODESPACE_KEY);
                    props.unset(Identifier::AUTHORITY_KEY);
                    props.unset(IdentifiedObject::IDENTIFIERS_KEY);
                    crs = GeographicCRS::create(props, datum, datumEnsemble,
                                                NN_NO_CHECK(ellipsoidalCS));
                } else if (dbCRS) {
                    crs = GeographicCRS::create(props, datum, datumEnsemble,
                                                dbCRS->coordinateSystem());
                }
            }
            return crs;
        } catch (const util::Exception &e) {
            throw ParsingException(std::string("buildGeodeticCRS: ") +
                                   e.what());
        }
    } else if (ci_equal(nodeName, WKTConstants::GEOGCRS) ||
               ci_equal(nodeName, WKTConstants::GEOGRAPHICCRS) ||
               ci_equal(nodeName, WKTConstants::BASEGEOGCRS)) {
        // This is a WKT2-2018 GeographicCRS. An ellipsoidal CS is expected
        throw ParsingException(concat("ellipsoidal CS expected, but found ",
                                      cs->getWKT2Type(true)));
    }

    auto cartesianCS = nn_dynamic_pointer_cast<CartesianCS>(cs);
    if (cartesianCS) {
        if (cartesianCS->axisList().size() != 3) {
            throw ParsingException(
                "Cartesian CS for a GeodeticCRS should have 3 axis");
        }
        try {
            return GeodeticCRS::create(props, datum, datumEnsemble,
                                       NN_NO_CHECK(cartesianCS));
        } catch (const util::Exception &e) {
            throw ParsingException(std::string("buildGeodeticCRS: ") +
                                   e.what());
        }
    }

    auto sphericalCS = nn_dynamic_pointer_cast<SphericalCS>(cs);
    if (sphericalCS) {
        try {
            return GeodeticCRS::create(props, datum, datumEnsemble,
                                       NN_NO_CHECK(sphericalCS));
        } catch (const util::Exception &e) {
            throw ParsingException(std::string("buildGeodeticCRS: ") +
                                   e.what());
        }
    }

    throw ParsingException(
        concat("unhandled CS type: ", cs->getWKT2Type(true)));
}

// ---------------------------------------------------------------------------

CRSNNPtr WKTParser::Private::buildDerivedGeodeticCRS(const WKTNodeNNPtr &node) {
    const auto *nodeP = node->GP();
    auto &baseGeodCRSNode = nodeP->lookForChild(WKTConstants::BASEGEODCRS,
                                                WKTConstants::BASEGEOGCRS);
    // given the constraints enforced on calling code path
    assert(!isNull(baseGeodCRSNode));

    auto baseGeodCRS = buildGeodeticCRS(baseGeodCRSNode);

    auto &derivingConversionNode =
        nodeP->lookForChild(WKTConstants::DERIVINGCONVERSION);
    if (isNull(derivingConversionNode)) {
        ThrowMissing(WKTConstants::DERIVINGCONVERSION);
    }
    auto derivingConversion = buildConversion(
        derivingConversionNode, UnitOfMeasure::NONE, UnitOfMeasure::NONE);

    auto &csNode = nodeP->lookForChild(WKTConstants::CS_);
    if (isNull(csNode)) {
        ThrowMissing(WKTConstants::CS_);
    }
    auto cs = buildCS(csNode, node, UnitOfMeasure::NONE);

    auto ellipsoidalCS = nn_dynamic_pointer_cast<EllipsoidalCS>(cs);
    if (ellipsoidalCS) {
        return DerivedGeographicCRS::create(buildProperties(node), baseGeodCRS,
                                            derivingConversion,
                                            NN_NO_CHECK(ellipsoidalCS));
    } else if (ci_equal(nodeP->value(), WKTConstants::GEOGCRS)) {
        // This is a WKT2-2018 GeographicCRS. An ellipsoidal CS is expected
        throw ParsingException(concat("ellipsoidal CS expected, but found ",
                                      cs->getWKT2Type(true)));
    }

    auto cartesianCS = nn_dynamic_pointer_cast<CartesianCS>(cs);
    if (cartesianCS) {
        if (cartesianCS->axisList().size() != 3) {
            throw ParsingException(
                "Cartesian CS for a GeodeticCRS should have 3 axis");
        }
        return DerivedGeodeticCRS::create(buildProperties(node), baseGeodCRS,
                                          derivingConversion,
                                          NN_NO_CHECK(cartesianCS));
    }

    auto sphericalCS = nn_dynamic_pointer_cast<SphericalCS>(cs);
    if (sphericalCS) {
        return DerivedGeodeticCRS::create(buildProperties(node), baseGeodCRS,
                                          derivingConversion,
                                          NN_NO_CHECK(sphericalCS));
    }

    throw ParsingException(
        concat("unhandled CS type: ", cs->getWKT2Type(true)));
}

// ---------------------------------------------------------------------------

UnitOfMeasure WKTParser::Private::guessUnitForParameter(
    const std::string &paramName, const UnitOfMeasure &defaultLinearUnit,
    const UnitOfMeasure &defaultAngularUnit) {
    UnitOfMeasure unit;
    // scale must be first because of 'Scale factor on pseudo standard parallel'
    if (ci_find(paramName, "scale") != std::string::npos) {
        unit = UnitOfMeasure::SCALE_UNITY;
    } else if (ci_find(paramName, "latitude") != std::string::npos ||
               ci_find(paramName, "longitude") != std::string::npos ||
               ci_find(paramName, "meridian") != std::string::npos ||
               ci_find(paramName, "parallel") != std::string::npos ||
               ci_find(paramName, "azimuth") != std::string::npos ||
               ci_find(paramName, "angle") != std::string::npos ||
               ci_find(paramName, "heading") != std::string::npos) {
        unit = defaultAngularUnit;
    } else if (ci_find(paramName, "easting") != std::string::npos ||
               ci_find(paramName, "northing") != std::string::npos ||
               ci_find(paramName, "height") != std::string::npos) {
        unit = defaultLinearUnit;
    }
    return unit;
}

// ---------------------------------------------------------------------------

void WKTParser::Private::consumeParameters(
    const WKTNodeNNPtr &node, bool isAbridged,
    std::vector<OperationParameterNNPtr> &parameters,
    std::vector<ParameterValueNNPtr> &values,
    const UnitOfMeasure &defaultLinearUnit,
    const UnitOfMeasure &defaultAngularUnit) {
    for (const auto &childNode : node->GP()->children()) {
        const auto &childNodeChildren = childNode->GP()->children();
        if (ci_equal(childNode->GP()->value(), WKTConstants::PARAMETER)) {
            if (childNodeChildren.size() < 2) {
                ThrowNotEnoughChildren(childNode->GP()->value());
            }
            parameters.push_back(
                OperationParameter::create(buildProperties(childNode)));
            const auto &paramValue = childNodeChildren[1]->GP()->value();
            if (!paramValue.empty() && paramValue[0] == '"') {
                values.push_back(
                    ParameterValue::create(stripQuotes(childNodeChildren[1])));
            } else {
                try {
                    double val = asDouble(childNodeChildren[1]);
                    auto unit = buildUnitInSubNode(childNode);
                    if (unit == UnitOfMeasure::NONE) {
                        const auto &paramName =
                            childNodeChildren[0]->GP()->value();
                        unit = guessUnitForParameter(
                            paramName, defaultLinearUnit, defaultAngularUnit);
                    }

                    if (isAbridged) {
                        const auto &paramName = parameters.back()->nameStr();
                        int paramEPSGCode = 0;
                        const auto &paramIds = parameters.back()->identifiers();
                        if (paramIds.size() == 1 &&
                            ci_equal(*(paramIds[0]->codeSpace()),
                                     Identifier::EPSG)) {
                            paramEPSGCode = ::atoi(paramIds[0]->code().c_str());
                        }
                        const common::UnitOfMeasure *pUnit = nullptr;
                        if (OperationParameterValue::convertFromAbridged(
                                paramName, val, pUnit, paramEPSGCode)) {
                            unit = *pUnit;
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
                    throw ParsingException(concat(
                        "unhandled parameter value type : ", paramValue));
                }
            }
        } else if (ci_equal(childNode->GP()->value(),
                            WKTConstants::PARAMETERFILE)) {
            if (childNodeChildren.size() < 2) {
                ThrowNotEnoughChildren(childNode->GP()->value());
            }
            parameters.push_back(
                OperationParameter::create(buildProperties(childNode)));
            values.push_back(ParameterValue::createFilename(
                stripQuotes(childNodeChildren[1])));
        }
    }
}

// ---------------------------------------------------------------------------

ConversionNNPtr
WKTParser::Private::buildConversion(const WKTNodeNNPtr &node,
                                    const UnitOfMeasure &defaultLinearUnit,
                                    const UnitOfMeasure &defaultAngularUnit) {
    auto &methodNode = node->GP()->lookForChild(WKTConstants::METHOD,
                                                WKTConstants::PROJECTION);
    if (isNull(methodNode)) {
        ThrowMissing(WKTConstants::METHOD);
    }
    if (methodNode->GP()->childrenSize() == 0) {
        ThrowNotEnoughChildren(WKTConstants::METHOD);
    }

    std::vector<OperationParameterNNPtr> parameters;
    std::vector<ParameterValueNNPtr> values;
    consumeParameters(node, false, parameters, values, defaultLinearUnit,
                      defaultAngularUnit);

    auto &convProps = buildProperties(node);
    auto &methodProps = buildProperties(methodNode);
    std::string convName;
    std::string methodName;
    if (convProps.getStringValue(IdentifiedObject::NAME_KEY, convName) &&
        methodProps.getStringValue(IdentifiedObject::NAME_KEY, methodName) &&
        starts_with(convName, "Inverse of ") &&
        starts_with(methodName, "Inverse of ")) {

        auto &invConvProps = buildProperties(node, true);
        auto &invMethodProps = buildProperties(methodNode, true);
        return NN_NO_CHECK(util::nn_dynamic_pointer_cast<Conversion>(
            Conversion::create(invConvProps, invMethodProps, parameters, values)
                ->inverse()));
    }
    return Conversion::create(convProps, methodProps, parameters, values);
}

// ---------------------------------------------------------------------------

TransformationNNPtr
WKTParser::Private::buildCoordinateOperation(const WKTNodeNNPtr &node) {
    const auto *nodeP = node->GP();
    auto &methodNode = nodeP->lookForChild(WKTConstants::METHOD);
    if (isNull(methodNode)) {
        ThrowMissing(WKTConstants::METHOD);
    }
    if (methodNode->GP()->childrenSize() == 0) {
        ThrowNotEnoughChildren(WKTConstants::METHOD);
    }

    auto &sourceCRSNode = nodeP->lookForChild(WKTConstants::SOURCECRS);
    if (/*isNull(sourceCRSNode) ||*/ sourceCRSNode->GP()->childrenSize() != 1) {
        ThrowMissing(WKTConstants::SOURCECRS);
    }
    auto sourceCRS = buildCRS(sourceCRSNode->GP()->children()[0]);
    if (!sourceCRS) {
        throw ParsingException("Invalid content in SOURCECRS node");
    }

    auto &targetCRSNode = nodeP->lookForChild(WKTConstants::TARGETCRS);
    if (/*isNull(targetCRSNode) ||*/ targetCRSNode->GP()->childrenSize() != 1) {
        ThrowMissing(WKTConstants::TARGETCRS);
    }
    auto targetCRS = buildCRS(targetCRSNode->GP()->children()[0]);
    if (!targetCRS) {
        throw ParsingException("Invalid content in TARGETCRS node");
    }

    auto &interpolationCRSNode =
        nodeP->lookForChild(WKTConstants::INTERPOLATIONCRS);
    CRSPtr interpolationCRS;
    if (/*!isNull(interpolationCRSNode) && */ interpolationCRSNode->GP()
            ->childrenSize() == 1) {
        interpolationCRS = buildCRS(interpolationCRSNode->GP()->children()[0]);
    }

    std::vector<OperationParameterNNPtr> parameters;
    std::vector<ParameterValueNNPtr> values;
    auto defaultLinearUnit = UnitOfMeasure::NONE;
    auto defaultAngularUnit = UnitOfMeasure::NONE;
    consumeParameters(node, false, parameters, values, defaultLinearUnit,
                      defaultAngularUnit);

    std::vector<PositionalAccuracyNNPtr> accuracies;
    auto &accuracyNode = nodeP->lookForChild(WKTConstants::OPERATIONACCURACY);
    if (/*!isNull(accuracyNode) && */ accuracyNode->GP()->childrenSize() == 1) {
        accuracies.push_back(PositionalAccuracy::create(
            stripQuotes(accuracyNode->GP()->children()[0])));
    }

    return Transformation::create(buildProperties(node), NN_NO_CHECK(sourceCRS),
                                  NN_NO_CHECK(targetCRS), interpolationCRS,
                                  buildProperties(methodNode), parameters,
                                  values, accuracies);
}

// ---------------------------------------------------------------------------

ConcatenatedOperationNNPtr
WKTParser::Private::buildConcatenatedOperation(const WKTNodeNNPtr &node) {

    const auto *nodeP = node->GP();
    auto &sourceCRSNode = nodeP->lookForChild(WKTConstants::SOURCECRS);
    if (/*isNull(sourceCRSNode) ||*/ sourceCRSNode->GP()->childrenSize() != 1) {
        ThrowMissing(WKTConstants::SOURCECRS);
    }
    auto sourceCRS = buildCRS(sourceCRSNode->GP()->children()[0]);
    if (!sourceCRS) {
        throw ParsingException("Invalid content in SOURCECRS node");
    }

    auto &targetCRSNode = nodeP->lookForChild(WKTConstants::TARGETCRS);
    if (/*isNull(targetCRSNode) ||*/ targetCRSNode->GP()->childrenSize() != 1) {
        ThrowMissing(WKTConstants::TARGETCRS);
    }
    auto targetCRS = buildCRS(targetCRSNode->GP()->children()[0]);
    if (!targetCRS) {
        throw ParsingException("Invalid content in TARGETCRS node");
    }

    std::vector<CoordinateOperationNNPtr> operations;
    for (const auto &childNode : nodeP->children()) {
        if (ci_equal(childNode->GP()->value(), WKTConstants::STEP)) {
            if (childNode->GP()->childrenSize() != 1) {
                throw ParsingException("Invalid content in STEP node");
            }
            auto op = nn_dynamic_pointer_cast<CoordinateOperation>(
                build(childNode->GP()->children()[0]));
            if (!op) {
                throw ParsingException("Invalid content in STEP node");
            }
            operations.emplace_back(NN_NO_CHECK(op));
        }
    }

    ConcatenatedOperation::fixStepsDirection(
        NN_NO_CHECK(sourceCRS), NN_NO_CHECK(targetCRS), operations);

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

bool WKTParser::Private::hasWebMercPROJ4String(
    const WKTNodeNNPtr &projCRSNode, const WKTNodeNNPtr &projectionNode) {
    if (projectionNode->GP()->childrenSize() == 0) {
        ThrowNotEnoughChildren(WKTConstants::PROJECTION);
    }
    const std::string wkt1ProjectionName =
        stripQuotes(projectionNode->GP()->children()[0]);

    auto &extensionNode = projCRSNode->lookForChild(WKTConstants::EXTENSION);

    if (metadata::Identifier::isEquivalentName(wkt1ProjectionName.c_str(),
                                               "Mercator_1SP") &&
        projCRSNode->countChildrenOfName("center_latitude") == 0) {

        // Hack to detect the hacky way of encodign webmerc in GDAL WKT1
        // with a EXTENSION["PROJ4", "+proj=merc +a=6378137 +b=6378137
        // +lat_ts=0.0 +lon_0=0.0 +x_0=0.0 +y_0=0 +k=1.0 +units=m
        // +nadgrids=@null +wktext +no_defs"] node
        if (extensionNode && extensionNode->GP()->childrenSize() == 2 &&
            ci_equal(stripQuotes(extensionNode->GP()->children()[0]),
                     "PROJ4")) {
            std::string projString =
                stripQuotes(extensionNode->GP()->children()[1]);
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
                return true;
            }
        }
    }
    return false;
}

// ---------------------------------------------------------------------------

ConversionNNPtr WKTParser::Private::buildProjectionFromESRI(
    const WKTNodeNNPtr &projCRSNode, const WKTNodeNNPtr &projectionNode,
    const UnitOfMeasure &defaultLinearUnit,
    const UnitOfMeasure &defaultAngularUnit) {
    const std::string esriProjectionName =
        stripQuotes(projectionNode->GP()->children()[0]);

    // Lambert_Conformal_Conic or Krovak may map to different WKT2 methods
    // depending
    // on the parameters / their values
    const auto esriMappings = getMappingsFromESRI(esriProjectionName);
    if (esriMappings.empty()) {
        return buildProjectionStandard(projCRSNode, projectionNode,
                                       defaultLinearUnit, defaultAngularUnit);
    }

    struct ci_less_struct {
        bool operator()(const std::string &lhs, const std::string &rhs) const
            noexcept {
            return ci_less(lhs, rhs);
        }
    };

    // Build a map of present parameters
    std::map<std::string, std::string, ci_less_struct> mapParamNameToValue;
    for (const auto &childNode : projCRSNode->GP()->children()) {
        if (ci_equal(childNode->GP()->value(), WKTConstants::PARAMETER)) {
            const auto &childNodeChildren = childNode->GP()->children();
            if (childNodeChildren.size() < 2) {
                ThrowNotEnoughChildren(WKTConstants::PARAMETER);
            }
            const std::string parameterName(stripQuotes(childNodeChildren[0]));
            const auto &paramValue = childNodeChildren[1]->GP()->value();
            mapParamNameToValue[parameterName] = paramValue;
        }
    }

    // Compare parameters present with the ones expected in the mapping
    const ESRIMethodMapping *esriMapping = esriMappings[0];
    int bestMatchCount = -1;
    for (const auto &mapping : esriMappings) {
        int matchCount = 0;
        for (const auto *param = mapping->params; param->esri_name; ++param) {
            auto iter = mapParamNameToValue.find(param->esri_name);
            if (iter != mapParamNameToValue.end()) {
                if (param->wkt2_name == nullptr) {
                    try {
                        if (param->fixed_value == io::asDouble(iter->second)) {
                            matchCount++;
                        }
                    } catch (const std::exception &) {
                    }
                } else {
                    matchCount++;
                }
            }
        }
        if (matchCount > bestMatchCount) {
            esriMapping = mapping;
            bestMatchCount = matchCount;
        }
    }

    std::map<std::string, const char *> mapWKT2NameToESRIName;
    for (const auto *param = esriMapping->params; param->esri_name; ++param) {
        if (param->wkt2_name) {
            mapWKT2NameToESRIName[param->wkt2_name] = param->esri_name;
        }
    }

    const auto *wkt2_mapping = getMapping(esriMapping->wkt2_name);
    if (ci_equal(esriProjectionName, "Stereographic")) {
        try {
            if (std::fabs(io::asDouble(
                    mapParamNameToValue["Latitude_Of_Origin"])) == 90.0) {
                wkt2_mapping =
                    getMapping(EPSG_CODE_METHOD_POLAR_STEREOGRAPHIC_VARIANT_A);
            }
        } catch (const std::exception &) {
        }
    }
    assert(wkt2_mapping);

    PropertyMap propertiesMethod;
    propertiesMethod.set(IdentifiedObject::NAME_KEY, wkt2_mapping->wkt2_name);
    if (wkt2_mapping->epsg_code != 0) {
        propertiesMethod.set(Identifier::CODE_KEY, wkt2_mapping->epsg_code);
        propertiesMethod.set(Identifier::CODESPACE_KEY, Identifier::EPSG);
    }

    std::vector<OperationParameterNNPtr> parameters;
    std::vector<ParameterValueNNPtr> values;

    if (wkt2_mapping->epsg_code == EPSG_CODE_METHOD_EQUIDISTANT_CYLINDRICAL &&
        ci_equal(esriProjectionName, "Plate_Carree")) {
        // Add a fixed  Latitude of 1st parallel = 0 so as to have all
        // parameters expected by Equidistant Cylindrical.
        mapWKT2NameToESRIName[EPSG_NAME_PARAMETER_LATITUDE_1ST_STD_PARALLEL] =
            "Standard_Parallel_1";
        mapParamNameToValue["Standard_Parallel_1"] = "0";
    } else if ((wkt2_mapping->epsg_code ==
                    EPSG_CODE_METHOD_HOTINE_OBLIQUE_MERCATOR_VARIANT_A ||
                wkt2_mapping->epsg_code ==
                    EPSG_CODE_METHOD_HOTINE_OBLIQUE_MERCATOR_VARIANT_B) &&
               !ci_equal(esriProjectionName,
                         "Rectified_Skew_Orthomorphic_Natural_Origin") &&
               !ci_equal(esriProjectionName,
                         "Rectified_Skew_Orthomorphic_Center")) {
        // ESRI WKT lacks the angle to skew grid
        // Take it from the azimuth value
        mapWKT2NameToESRIName
            [EPSG_NAME_PARAMETER_ANGLE_RECTIFIED_TO_SKEW_GRID] = "Azimuth";
    }

    for (int i = 0; wkt2_mapping->params[i] != nullptr; i++) {
        const auto *paramMapping = wkt2_mapping->params[i];

        auto iter = mapWKT2NameToESRIName.find(paramMapping->wkt2_name);
        if (iter == mapWKT2NameToESRIName.end()) {
            continue;
        }
        const auto &esriParamName = iter->second;
        auto iter2 = mapParamNameToValue.find(esriParamName);
        auto mapParamNameToValueEnd = mapParamNameToValue.end();
        if (iter2 == mapParamNameToValueEnd) {
            // In case we don't find a direct match, try the aliases
            for (iter2 = mapParamNameToValue.begin();
                 iter2 != mapParamNameToValueEnd; ++iter2) {
                if (areEquivalentParameters(iter2->first, esriParamName)) {
                    break;
                }
            }
            if (iter2 == mapParamNameToValueEnd) {
                continue;
            }
        }

        PropertyMap propertiesParameter;
        propertiesParameter.set(IdentifiedObject::NAME_KEY,
                                paramMapping->wkt2_name);
        if (paramMapping->epsg_code != 0) {
            propertiesParameter.set(Identifier::CODE_KEY,
                                    paramMapping->epsg_code);
            propertiesParameter.set(Identifier::CODESPACE_KEY,
                                    Identifier::EPSG);
        }
        parameters.push_back(OperationParameter::create(propertiesParameter));

        try {
            double val = io::asDouble(iter2->second);
            auto unit = guessUnitForParameter(
                paramMapping->wkt2_name, defaultLinearUnit, defaultAngularUnit);
            values.push_back(ParameterValue::create(Measure(val, unit)));
        } catch (const std::exception &) {
            throw ParsingException(
                concat("unhandled parameter value type : ", iter2->second));
        }
    }

    return Conversion::create(
               PropertyMap().set(IdentifiedObject::NAME_KEY,
                                 esriProjectionName == "Gauss_Kruger"
                                     ? "unnnamed (Gauss Kruger)"
                                     : "unnamed"),
               propertiesMethod, parameters, values)
        ->identify();
}

// ---------------------------------------------------------------------------

ConversionNNPtr
WKTParser::Private::buildProjection(const WKTNodeNNPtr &projCRSNode,
                                    const WKTNodeNNPtr &projectionNode,
                                    const UnitOfMeasure &defaultLinearUnit,
                                    const UnitOfMeasure &defaultAngularUnit) {
    if (projectionNode->GP()->childrenSize() == 0) {
        ThrowNotEnoughChildren(WKTConstants::PROJECTION);
    }
    if (esriStyle_) {
        return buildProjectionFromESRI(projCRSNode, projectionNode,
                                       defaultLinearUnit, defaultAngularUnit);
    }
    return buildProjectionStandard(projCRSNode, projectionNode,
                                   defaultLinearUnit, defaultAngularUnit);
}

// ---------------------------------------------------------------------------

std::string
WKTParser::Private::projectionGetParameter(const WKTNodeNNPtr &projCRSNode,
                                           const char *paramName) {
    for (const auto &childNode : projCRSNode->GP()->children()) {
        if (ci_equal(childNode->GP()->value(), WKTConstants::PARAMETER)) {
            const auto &childNodeChildren = childNode->GP()->children();
            if (childNodeChildren.size() == 2 &&
                metadata::Identifier::isEquivalentName(
                    stripQuotes(childNodeChildren[0]).c_str(), paramName)) {
                return childNodeChildren[1]->GP()->value();
            }
        }
    }
    return std::string();
}

// ---------------------------------------------------------------------------

ConversionNNPtr WKTParser::Private::buildProjectionStandard(
    const WKTNodeNNPtr &projCRSNode, const WKTNodeNNPtr &projectionNode,
    const UnitOfMeasure &defaultLinearUnit,
    const UnitOfMeasure &defaultAngularUnit) {
    std::string wkt1ProjectionName =
        stripQuotes(projectionNode->GP()->children()[0]);

    std::vector<OperationParameterNNPtr> parameters;
    std::vector<ParameterValueNNPtr> values;
    bool tryToIdentifyWKT1Method = true;

    auto &extensionNode = projCRSNode->lookForChild(WKTConstants::EXTENSION);
    const auto &extensionChildren = extensionNode->GP()->children();

    bool gdal_3026_hack = false;
    if (metadata::Identifier::isEquivalentName(wkt1ProjectionName.c_str(),
                                               "Mercator_1SP") &&
        projectionGetParameter(projCRSNode, "center_latitude").empty()) {

        // Hack for https://trac.osgeo.org/gdal/ticket/3026
        std::string lat0(
            projectionGetParameter(projCRSNode, "latitude_of_origin"));
        if (!lat0.empty() && lat0 != "0" && lat0 != "0.0") {
            wkt1ProjectionName = "Mercator_2SP";
            gdal_3026_hack = true;
        } else {
            // The latitude of origin, which should always be zero, is
            // missing
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

    } else if (metadata::Identifier::isEquivalentName(
                   wkt1ProjectionName.c_str(), "Polar_Stereographic")) {
        std::map<std::string, Measure> mapParameters;
        for (const auto &childNode : projCRSNode->GP()->children()) {
            const auto &childNodeChildren = childNode->GP()->children();
            if (ci_equal(childNode->GP()->value(), WKTConstants::PARAMETER) &&
                childNodeChildren.size() == 2) {
                const std::string wkt1ParameterName(
                    stripQuotes(childNodeChildren[0]));
                try {
                    double val = asDouble(childNodeChildren[1]);
                    auto unit = guessUnitForParameter(wkt1ParameterName,
                                                      defaultLinearUnit,
                                                      defaultAngularUnit);
                    mapParameters.insert(std::pair<std::string, Measure>(
                        tolower(wkt1ParameterName), Measure(val, unit)));
                } catch (const std::exception &) {
                }
            }
        }

        Measure latitudeOfOrigin = mapParameters["latitude_of_origin"];
        Measure centralMeridian = mapParameters["central_meridian"];
        Measure scaleFactorFromMap = mapParameters["scale_factor"];
        Measure scaleFactor((scaleFactorFromMap.unit() == UnitOfMeasure::NONE)
                                ? Measure(1.0, UnitOfMeasure::SCALE_UNITY)
                                : scaleFactorFromMap);
        Measure falseEasting = mapParameters["false_easting"];
        Measure falseNorthing = mapParameters["false_northing"];
        if (latitudeOfOrigin.unit() != UnitOfMeasure::NONE &&
            scaleFactor.getSIValue() == 1.0) {
            return Conversion::createPolarStereographicVariantB(
                PropertyMap().set(IdentifiedObject::NAME_KEY, "unnamed"),
                Angle(latitudeOfOrigin.value(), latitudeOfOrigin.unit()),
                Angle(centralMeridian.value(), centralMeridian.unit()),
                Length(falseEasting.value(), falseEasting.unit()),
                Length(falseNorthing.value(), falseNorthing.unit()));
        }

        if (latitudeOfOrigin.unit() != UnitOfMeasure::NONE &&
            std::fabs(std::fabs(latitudeOfOrigin.convertToUnit(
                          UnitOfMeasure::DEGREE)) -
                      90.0) < 1e-10) {
            return Conversion::createPolarStereographicVariantA(
                PropertyMap().set(IdentifiedObject::NAME_KEY, "unnamed"),
                Angle(latitudeOfOrigin.value(), latitudeOfOrigin.unit()),
                Angle(centralMeridian.value(), centralMeridian.unit()),
                Scale(scaleFactor.value(), scaleFactor.unit()),
                Length(falseEasting.value(), falseEasting.unit()),
                Length(falseNorthing.value(), falseNorthing.unit()));
        }

        tryToIdentifyWKT1Method = false;
        // Import GDAL PROJ4 extension nodes
    } else if (extensionChildren.size() == 2 &&
               ci_equal(stripQuotes(extensionChildren[0]), "PROJ4")) {
        std::string projString = stripQuotes(extensionChildren[1]);
        if (starts_with(projString, "+proj=")) {
            if (projString.find(" +type=crs") == std::string::npos) {
                projString += " +type=crs";
            }
            try {
                auto projObj =
                    PROJStringParser().createFromPROJString(projString);
                auto projObjCrs =
                    nn_dynamic_pointer_cast<ProjectedCRS>(projObj);
                if (projObjCrs) {
                    return projObjCrs->derivingConversion();
                }
            } catch (const io::ParsingException &) {
            }
        }
    }

    std::string projectionName(wkt1ProjectionName);
    const MethodMapping *mapping =
        tryToIdentifyWKT1Method ? getMappingFromWKT1(projectionName) : nullptr;

    // For Krovak, we need to look at axis to decide between the Krovak and
    // Krovak East-North Oriented methods
    if (ci_equal(projectionName, "Krovak") &&
        projCRSNode->countChildrenOfName(WKTConstants::AXIS) == 2 &&
        &buildAxis(
             projCRSNode->GP()->lookForChild(WKTConstants::AXIS, 0),
             defaultLinearUnit, UnitOfMeasure::Type::LINEAR, false,
             1)->direction() == &AxisDirection::SOUTH &&
        &buildAxis(
             projCRSNode->GP()->lookForChild(WKTConstants::AXIS, 1),
             defaultLinearUnit, UnitOfMeasure::Type::LINEAR, false,
             2)->direction() == &AxisDirection::WEST) {
        mapping = getMapping(EPSG_CODE_METHOD_KROVAK);
    }

    PropertyMap propertiesMethod;
    if (mapping) {
        projectionName = mapping->wkt2_name;
        if (mapping->epsg_code != 0) {
            propertiesMethod.set(Identifier::CODE_KEY, mapping->epsg_code);
            propertiesMethod.set(Identifier::CODESPACE_KEY, Identifier::EPSG);
        }
    }
    propertiesMethod.set(IdentifiedObject::NAME_KEY, projectionName);

    for (const auto &childNode : projCRSNode->GP()->children()) {
        if (ci_equal(childNode->GP()->value(), WKTConstants::PARAMETER)) {
            const auto &childNodeChildren = childNode->GP()->children();
            if (childNodeChildren.size() < 2) {
                ThrowNotEnoughChildren(WKTConstants::PARAMETER);
            }
            const auto &paramValue = childNodeChildren[1]->GP()->value();

            PropertyMap propertiesParameter;
            const std::string wkt1ParameterName(
                stripQuotes(childNodeChildren[0]));
            std::string parameterName(wkt1ParameterName);
            if (gdal_3026_hack) {
                if (ci_equal(parameterName, "latitude_of_origin")) {
                    parameterName = "standard_parallel_1";
                } else if (ci_equal(parameterName, "scale_factor") &&
                           paramValue == "1") {
                    continue;
                }
            }
            const auto *paramMapping =
                mapping ? getMappingFromWKT1(mapping, parameterName) : nullptr;
            if (mapping &&
                mapping->epsg_code == EPSG_CODE_METHOD_MERCATOR_VARIANT_B &&
                ci_equal(parameterName, "latitude_of_origin")) {
                parameterName = EPSG_NAME_PARAMETER_LATITUDE_OF_NATURAL_ORIGIN;
                propertiesParameter.set(
                    Identifier::CODE_KEY,
                    EPSG_CODE_PARAMETER_LATITUDE_OF_NATURAL_ORIGIN);
                propertiesParameter.set(Identifier::CODESPACE_KEY,
                                        Identifier::EPSG);
            } else if (paramMapping) {
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
            try {
                double val = io::asDouble(paramValue);
                auto unit = guessUnitForParameter(
                    wkt1ParameterName, defaultLinearUnit, defaultAngularUnit);
                values.push_back(ParameterValue::create(Measure(val, unit)));
            } catch (const std::exception &) {
                throw ParsingException(
                    concat("unhandled parameter value type : ", paramValue));
            }
        }
    }

    return Conversion::create(
               PropertyMap().set(IdentifiedObject::NAME_KEY, "unnamed"),
               propertiesMethod, parameters, values)
        ->identify();
}

// ---------------------------------------------------------------------------

static ProjectedCRSNNPtr createPseudoMercator(const PropertyMap &props) {
    auto conversion = Conversion::createPopularVisualisationPseudoMercator(
        PropertyMap().set(IdentifiedObject::NAME_KEY, "unnamed"), Angle(0),
        Angle(0), Length(0), Length(0));
    return ProjectedCRS::create(
        props, GeographicCRS::EPSG_4326, conversion,
        CartesianCS::createEastingNorthing(UnitOfMeasure::METRE));
}

// ---------------------------------------------------------------------------

ProjectedCRSNNPtr
WKTParser::Private::buildProjectedCRS(const WKTNodeNNPtr &node) {

    const auto *nodeP = node->GP();
    auto &conversionNode = nodeP->lookForChild(WKTConstants::CONVERSION);
    auto &projectionNode = nodeP->lookForChild(WKTConstants::PROJECTION);
    if (isNull(conversionNode) && isNull(projectionNode)) {
        ThrowMissing(WKTConstants::CONVERSION);
    }

    auto &baseGeodCRSNode =
        nodeP->lookForChild(WKTConstants::BASEGEODCRS,
                            WKTConstants::BASEGEOGCRS, WKTConstants::GEOGCS);
    if (isNull(baseGeodCRSNode)) {
        throw ParsingException(
            "Missing BASEGEODCRS / BASEGEOGCRS / GEOGCS node");
    }
    auto baseGeodCRS = buildGeodeticCRS(baseGeodCRSNode);

    auto props = buildProperties(node);

    const std::string projCRSName = stripQuotes(nodeP->children()[0]);
    if (esriStyle_ && dbContext_) {
        // It is likely that the ESRI definition of EPSG:32661 (UPS North) &
        // EPSG:32761 (UPS South) uses the easting-northing order, instead
        // of the EPSG northing-easting order
        // so don't substitue names to avoid confusion.
        if (projCRSName == "UPS_North") {
            props.set(IdentifiedObject::NAME_KEY, "WGS 84 / UPS North (E,N)");
        } else if (projCRSName == "UPS_South") {
            props.set(IdentifiedObject::NAME_KEY, "WGS 84 / UPS South (E,N)");
        } else {
            std::string outTableName;
            std::string authNameFromAlias;
            std::string codeFromAlias;
            auto authFactory = AuthorityFactory::create(NN_NO_CHECK(dbContext_),
                                                        std::string());
            auto officialName = authFactory->getOfficialNameFromAlias(
                projCRSName, "projected_crs", "ESRI", false, outTableName,
                authNameFromAlias, codeFromAlias);
            if (!officialName.empty()) {
                props.set(IdentifiedObject::NAME_KEY, officialName);
            }
        }
    }

    if (isNull(conversionNode) && hasWebMercPROJ4String(node, projectionNode)) {
        toWGS84Parameters_.clear();
        return createPseudoMercator(props);
    }

    // WGS_84_Pseudo_Mercator: Particular case for corrupted ESRI WKT generated
    // by older GDAL versions
    // https://trac.osgeo.org/gdal/changeset/30732
    // WGS_1984_Web_Mercator: deprecated ESRI:102113
    if (metadata::Identifier::isEquivalentName(projCRSName.c_str(),
                                               "WGS_84_Pseudo_Mercator") ||
        metadata::Identifier::isEquivalentName(projCRSName.c_str(),
                                               "WGS_1984_Web_Mercator")) {
        toWGS84Parameters_.clear();
        return createPseudoMercator(props);
    }

    auto linearUnit = buildUnitInSubNode(node, UnitOfMeasure::Type::LINEAR);
    auto angularUnit = baseGeodCRS->coordinateSystem()->axisList()[0]->unit();

    auto conversion =
        !isNull(conversionNode)
            ? buildConversion(conversionNode, linearUnit, angularUnit)
            : buildProjection(node, projectionNode, linearUnit, angularUnit);

    auto &csNode = nodeP->lookForChild(WKTConstants::CS_);
    const auto &nodeValue = nodeP->value();
    if (isNull(csNode) && !ci_equal(nodeValue, WKTConstants::PROJCS) &&
        !ci_equal(nodeValue, WKTConstants::BASEPROJCRS)) {
        ThrowMissing(WKTConstants::CS_);
    }
    auto cs = buildCS(csNode, node, UnitOfMeasure::NONE);
    auto cartesianCS = nn_dynamic_pointer_cast<CartesianCS>(cs);

    // No explicit AXIS node ? (WKT1)
    if (isNull(nodeP->lookForChild(WKTConstants::AXIS))) {
        props.set("IMPLICIT_CS", true);
    }

    if (isNull(csNode) && node->countChildrenOfName(WKTConstants::AXIS) == 0) {

        const auto methodCode = conversion->method()->getEPSGCode();
        // Krovak south oriented ?
        if (methodCode == EPSG_CODE_METHOD_KROVAK) {
            cartesianCS =
                CartesianCS::create(
                    PropertyMap(),
                    CoordinateSystemAxis::create(
                        util::PropertyMap().set(IdentifiedObject::NAME_KEY,
                                                AxisName::Southing),
                        emptyString, AxisDirection::SOUTH, linearUnit),
                    CoordinateSystemAxis::create(
                        util::PropertyMap().set(IdentifiedObject::NAME_KEY,
                                                AxisName::Westing),
                        emptyString, AxisDirection::WEST, linearUnit))
                    .as_nullable();
        } else if (methodCode ==
                       EPSG_CODE_METHOD_POLAR_STEREOGRAPHIC_VARIANT_A ||
                   methodCode ==
                       EPSG_CODE_METHOD_LAMBERT_AZIMUTHAL_EQUAL_AREA) {
            // It is likely that the ESRI definition of EPSG:32661 (UPS North) &
            // EPSG:32761 (UPS South) uses the easting-northing order, instead
            // of the EPSG northing-easting order.
            // Same for WKT1_GDAL
            const double lat0 = conversion->parameterValueNumeric(
                EPSG_CODE_PARAMETER_LATITUDE_OF_NATURAL_ORIGIN,
                common::UnitOfMeasure::DEGREE);
            if (std::fabs(lat0 - 90) < 1e-10) {
                cartesianCS =
                    CartesianCS::createNorthPoleEastingSouthNorthingSouth(
                        linearUnit)
                        .as_nullable();
            } else if (std::fabs(lat0 - -90) < 1e-10) {
                cartesianCS =
                    CartesianCS::createSouthPoleEastingNorthNorthingNorth(
                        linearUnit)
                        .as_nullable();
            }
        } else if (methodCode ==
                   EPSG_CODE_METHOD_POLAR_STEREOGRAPHIC_VARIANT_B) {
            const double lat_ts = conversion->parameterValueNumeric(
                EPSG_CODE_PARAMETER_LATITUDE_STD_PARALLEL,
                common::UnitOfMeasure::DEGREE);
            if (lat_ts > 0) {
                cartesianCS =
                    CartesianCS::createNorthPoleEastingSouthNorthingSouth(
                        linearUnit)
                        .as_nullable();
            } else if (lat_ts < 0) {
                cartesianCS =
                    CartesianCS::createSouthPoleEastingNorthNorthingNorth(
                        linearUnit)
                        .as_nullable();
            }
        } else if (methodCode ==
                   EPSG_CODE_METHOD_TRANSVERSE_MERCATOR_SOUTH_ORIENTATED) {
            cartesianCS =
                CartesianCS::createWestingSouthing(linearUnit).as_nullable();
        }
    }
    if (!cartesianCS) {
        ThrowNotExpectedCSType("Cartesian");
    }

    addExtensionProj4ToProp(nodeP, props);

    return ProjectedCRS::create(props, baseGeodCRS, conversion,
                                NN_NO_CHECK(cartesianCS));
}

// ---------------------------------------------------------------------------

void WKTParser::Private::parseDynamic(const WKTNodeNNPtr &dynamicNode,
                                      double &frameReferenceEpoch,
                                      util::optional<std::string> &modelName) {
    auto &frameEpochNode = dynamicNode->lookForChild(WKTConstants::FRAMEEPOCH);
    const auto &frameEpochChildren = frameEpochNode->GP()->children();
    if (frameEpochChildren.empty()) {
        ThrowMissing(WKTConstants::FRAMEEPOCH);
    }
    try {
        frameReferenceEpoch = asDouble(frameEpochChildren[0]);
    } catch (const std::exception &) {
        throw ParsingException("Invalid FRAMEEPOCH node");
    }
    auto &modelNode = dynamicNode->GP()->lookForChild(
        WKTConstants::MODEL, WKTConstants::VELOCITYGRID);
    const auto &modelChildren = modelNode->GP()->children();
    if (modelChildren.size() == 1) {
        modelName = stripQuotes(modelChildren[0]);
    }
}

// ---------------------------------------------------------------------------

VerticalReferenceFrameNNPtr WKTParser::Private::buildVerticalReferenceFrame(
    const WKTNodeNNPtr &node, const WKTNodeNNPtr &dynamicNode) {

    if (!isNull(dynamicNode)) {
        double frameReferenceEpoch = 0.0;
        util::optional<std::string> modelName;
        parseDynamic(dynamicNode, frameReferenceEpoch, modelName);
        return DynamicVerticalReferenceFrame::create(
            buildProperties(node), getAnchor(node),
            optional<RealizationMethod>(),
            common::Measure(frameReferenceEpoch, common::UnitOfMeasure::YEAR),
            modelName);
    }

    // WKT1 VERT_DATUM has a datum type after the datum name that we ignore.
    return VerticalReferenceFrame::create(buildProperties(node),
                                          getAnchor(node));
}

// ---------------------------------------------------------------------------

TemporalDatumNNPtr
WKTParser::Private::buildTemporalDatum(const WKTNodeNNPtr &node) {
    const auto *nodeP = node->GP();
    auto &calendarNode = nodeP->lookForChild(WKTConstants::CALENDAR);
    std::string calendar = TemporalDatum::CALENDAR_PROLEPTIC_GREGORIAN;
    const auto &calendarChildren = calendarNode->GP()->children();
    if (calendarChildren.size() == 1) {
        calendar = stripQuotes(calendarChildren[0]);
    }

    auto &timeOriginNode = nodeP->lookForChild(WKTConstants::TIMEORIGIN);
    std::string originStr;
    const auto &timeOriginNodeChildren = timeOriginNode->GP()->children();
    if (timeOriginNodeChildren.size() == 1) {
        originStr = stripQuotes(timeOriginNodeChildren[0]);
    }
    auto origin = DateTime::create(originStr);
    return TemporalDatum::create(buildProperties(node), origin, calendar);
}

// ---------------------------------------------------------------------------

EngineeringDatumNNPtr
WKTParser::Private::buildEngineeringDatum(const WKTNodeNNPtr &node) {
    return EngineeringDatum::create(buildProperties(node), getAnchor(node));
}

// ---------------------------------------------------------------------------

ParametricDatumNNPtr
WKTParser::Private::buildParametricDatum(const WKTNodeNNPtr &node) {
    return ParametricDatum::create(buildProperties(node), getAnchor(node));
}

// ---------------------------------------------------------------------------

CRSNNPtr WKTParser::Private::buildVerticalCRS(const WKTNodeNNPtr &node) {
    const auto *nodeP = node->GP();
    auto &datumNode =
        nodeP->lookForChild(WKTConstants::VDATUM, WKTConstants::VERT_DATUM,
                            WKTConstants::VERTICALDATUM, WKTConstants::VRF);
    auto &ensembleNode = nodeP->lookForChild(WKTConstants::ENSEMBLE);
    if (isNull(datumNode) && isNull(ensembleNode)) {
        throw ParsingException("Missing VDATUM or ENSEMBLE node");
    }

    auto &dynamicNode = nodeP->lookForChild(WKTConstants::DYNAMIC);
    auto datum =
        !isNull(datumNode)
            ? buildVerticalReferenceFrame(datumNode, dynamicNode).as_nullable()
            : nullptr;
    auto datumEnsemble =
        !isNull(ensembleNode)
            ? buildDatumEnsemble(ensembleNode, nullptr, false).as_nullable()
            : nullptr;

    auto &csNode = nodeP->lookForChild(WKTConstants::CS_);
    const auto &nodeValue = nodeP->value();
    if (isNull(csNode) && !ci_equal(nodeValue, WKTConstants::VERT_CS) &&
        !ci_equal(nodeValue, WKTConstants::BASEVERTCRS)) {
        ThrowMissing(WKTConstants::CS_);
    }
    auto cs = buildCS(csNode, node, UnitOfMeasure::NONE);
    auto verticalCS = nn_dynamic_pointer_cast<VerticalCS>(cs);
    if (!verticalCS) {
        ThrowNotExpectedCSType("vertical");
    }

    auto crs = nn_static_pointer_cast<CRS>(VerticalCRS::create(
        buildProperties(node), datum, datumEnsemble, NN_NO_CHECK(verticalCS)));

    if (!isNull(datumNode)) {
        auto &extensionNode = datumNode->lookForChild(WKTConstants::EXTENSION);
        const auto &extensionChildren = extensionNode->GP()->children();
        if (extensionChildren.size() == 2) {
            if (ci_equal(stripQuotes(extensionChildren[0]), "PROJ4_GRIDS")) {
                std::string transformationName(crs->nameStr());
                if (!ends_with(transformationName, " height")) {
                    transformationName += " height";
                }
                transformationName += " to WGS84 ellipsoidal height";
                auto transformation =
                    Transformation::createGravityRelatedHeightToGeographic3D(
                        PropertyMap().set(IdentifiedObject::NAME_KEY,
                                          transformationName),
                        crs, GeographicCRS::EPSG_4979, nullptr,
                        stripQuotes(extensionChildren[1]),
                        std::vector<PositionalAccuracyNNPtr>());
                return nn_static_pointer_cast<CRS>(BoundCRS::create(
                    crs, GeographicCRS::EPSG_4979, transformation));
            }
        }
    }

    return crs;
}

// ---------------------------------------------------------------------------

DerivedVerticalCRSNNPtr
WKTParser::Private::buildDerivedVerticalCRS(const WKTNodeNNPtr &node) {
    const auto *nodeP = node->GP();
    auto &baseVertCRSNode = nodeP->lookForChild(WKTConstants::BASEVERTCRS);
    // given the constraints enforced on calling code path
    assert(!isNull(baseVertCRSNode));

    auto baseVertCRS_tmp = buildVerticalCRS(baseVertCRSNode);
    auto baseVertCRS = NN_NO_CHECK(baseVertCRS_tmp->extractVerticalCRS());

    auto &derivingConversionNode =
        nodeP->lookForChild(WKTConstants::DERIVINGCONVERSION);
    if (isNull(derivingConversionNode)) {
        ThrowMissing(WKTConstants::DERIVINGCONVERSION);
    }
    auto derivingConversion = buildConversion(
        derivingConversionNode, UnitOfMeasure::NONE, UnitOfMeasure::NONE);

    auto &csNode = nodeP->lookForChild(WKTConstants::CS_);
    if (isNull(csNode)) {
        ThrowMissing(WKTConstants::CS_);
    }
    auto cs = buildCS(csNode, node, UnitOfMeasure::NONE);

    auto verticalCS = nn_dynamic_pointer_cast<VerticalCS>(cs);
    if (!verticalCS) {
        throw ParsingException(
            concat("vertical CS expected, but found ", cs->getWKT2Type(true)));
    }

    return DerivedVerticalCRS::create(buildProperties(node), baseVertCRS,
                                      derivingConversion,
                                      NN_NO_CHECK(verticalCS));
}

// ---------------------------------------------------------------------------

CompoundCRSNNPtr
WKTParser::Private::buildCompoundCRS(const WKTNodeNNPtr &node) {
    std::vector<CRSNNPtr> components;
    for (const auto &child : node->GP()->children()) {
        auto crs = buildCRS(child);
        if (crs) {
            components.push_back(NN_NO_CHECK(crs));
        }
    }
    return CompoundCRS::create(buildProperties(node), components);
}

// ---------------------------------------------------------------------------

BoundCRSNNPtr WKTParser::Private::buildBoundCRS(const WKTNodeNNPtr &node) {
    const auto *nodeP = node->GP();
    auto &abridgedNode =
        nodeP->lookForChild(WKTConstants::ABRIDGEDTRANSFORMATION);
    if (isNull(abridgedNode)) {
        ThrowNotEnoughChildren(WKTConstants::ABRIDGEDTRANSFORMATION);
    }

    auto &methodNode = abridgedNode->GP()->lookForChild(WKTConstants::METHOD);
    if (isNull(methodNode)) {
        ThrowMissing(WKTConstants::METHOD);
    }
    if (methodNode->GP()->childrenSize() == 0) {
        ThrowNotEnoughChildren(WKTConstants::METHOD);
    }

    auto &sourceCRSNode = nodeP->lookForChild(WKTConstants::SOURCECRS);
    const auto &sourceCRSNodeChildren = sourceCRSNode->GP()->children();
    if (sourceCRSNodeChildren.size() != 1) {
        ThrowNotEnoughChildren(WKTConstants::SOURCECRS);
    }
    auto sourceCRS = buildCRS(sourceCRSNodeChildren[0]);
    if (!sourceCRS) {
        throw ParsingException("Invalid content in SOURCECRS node");
    }

    auto &targetCRSNode = nodeP->lookForChild(WKTConstants::TARGETCRS);
    const auto &targetCRSNodeChildren = targetCRSNode->GP()->children();
    if (targetCRSNodeChildren.size() != 1) {
        ThrowNotEnoughChildren(WKTConstants::TARGETCRS);
    }
    auto targetCRS = buildCRS(targetCRSNodeChildren[0]);
    if (!targetCRS) {
        throw ParsingException("Invalid content in TARGETCRS node");
    }

    std::vector<OperationParameterNNPtr> parameters;
    std::vector<ParameterValueNNPtr> values;
    auto defaultLinearUnit = UnitOfMeasure::NONE;
    auto defaultAngularUnit = UnitOfMeasure::NONE;
    consumeParameters(abridgedNode, true, parameters, values, defaultLinearUnit,
                      defaultAngularUnit);

    CRSPtr sourceTransformationCRS;
    if (dynamic_cast<GeographicCRS *>(targetCRS.get())) {
        sourceTransformationCRS = sourceCRS->extractGeographicCRS();
        if (!sourceTransformationCRS) {
            sourceTransformationCRS =
                std::dynamic_pointer_cast<VerticalCRS>(sourceCRS);
            if (!sourceTransformationCRS) {
                throw ParsingException(
                    "Cannot find GeographicCRS or VerticalCRS in sourceCRS");
            }
        }
    } else {
        sourceTransformationCRS = sourceCRS;
    }

    auto transformation = Transformation::create(
        buildProperties(abridgedNode), NN_NO_CHECK(sourceTransformationCRS),
        NN_NO_CHECK(targetCRS), nullptr, buildProperties(methodNode),
        parameters, values, std::vector<PositionalAccuracyNNPtr>());

    return BoundCRS::create(NN_NO_CHECK(sourceCRS), NN_NO_CHECK(targetCRS),
                            transformation);
}

// ---------------------------------------------------------------------------

TemporalCSNNPtr
WKTParser::Private::buildTemporalCS(const WKTNodeNNPtr &parentNode) {

    auto &csNode = parentNode->GP()->lookForChild(WKTConstants::CS_);
    if (isNull(csNode) &&
        !ci_equal(parentNode->GP()->value(), WKTConstants::BASETIMECRS)) {
        ThrowMissing(WKTConstants::CS_);
    }
    auto cs = buildCS(csNode, parentNode, UnitOfMeasure::NONE);
    auto temporalCS = nn_dynamic_pointer_cast<TemporalCS>(cs);
    if (!temporalCS) {
        ThrowNotExpectedCSType("temporal");
    }
    return NN_NO_CHECK(temporalCS);
}

// ---------------------------------------------------------------------------

TemporalCRSNNPtr
WKTParser::Private::buildTemporalCRS(const WKTNodeNNPtr &node) {
    auto &datumNode =
        node->GP()->lookForChild(WKTConstants::TDATUM, WKTConstants::TIMEDATUM);
    if (isNull(datumNode)) {
        throw ParsingException("Missing TDATUM / TIMEDATUM node");
    }

    return TemporalCRS::create(buildProperties(node),
                               buildTemporalDatum(datumNode),
                               buildTemporalCS(node));
}

// ---------------------------------------------------------------------------

DerivedTemporalCRSNNPtr
WKTParser::Private::buildDerivedTemporalCRS(const WKTNodeNNPtr &node) {
    const auto *nodeP = node->GP();
    auto &baseCRSNode = nodeP->lookForChild(WKTConstants::BASETIMECRS);
    // given the constraints enforced on calling code path
    assert(!isNull(baseCRSNode));

    auto &derivingConversionNode =
        nodeP->lookForChild(WKTConstants::DERIVINGCONVERSION);
    if (isNull(derivingConversionNode)) {
        ThrowNotEnoughChildren(WKTConstants::DERIVINGCONVERSION);
    }

    return DerivedTemporalCRS::create(
        buildProperties(node), buildTemporalCRS(baseCRSNode),
        buildConversion(derivingConversionNode, UnitOfMeasure::NONE,
                        UnitOfMeasure::NONE),
        buildTemporalCS(node));
}

// ---------------------------------------------------------------------------

EngineeringCRSNNPtr
WKTParser::Private::buildEngineeringCRS(const WKTNodeNNPtr &node) {
    const auto *nodeP = node->GP();
    auto &datumNode = nodeP->lookForChild(WKTConstants::EDATUM,
                                          WKTConstants::ENGINEERINGDATUM);
    if (isNull(datumNode)) {
        throw ParsingException("Missing EDATUM / ENGINEERINGDATUM node");
    }

    auto &csNode = nodeP->lookForChild(WKTConstants::CS_);
    if (isNull(csNode) && !ci_equal(nodeP->value(), WKTConstants::BASEENGCRS)) {
        ThrowMissing(WKTConstants::CS_);
    }

    auto cs = buildCS(csNode, node, UnitOfMeasure::NONE);
    return EngineeringCRS::create(buildProperties(node),
                                  buildEngineeringDatum(datumNode), cs);
}

// ---------------------------------------------------------------------------

EngineeringCRSNNPtr
WKTParser::Private::buildEngineeringCRSFromLocalCS(const WKTNodeNNPtr &node) {
    auto &datumNode = node->GP()->lookForChild(WKTConstants::LOCAL_DATUM);
    auto cs = buildCS(null_node, node, UnitOfMeasure::NONE);
    auto datum = EngineeringDatum::create(
        !isNull(datumNode)
            ? buildProperties(datumNode)
            :
            // In theory OGC 01-009 mandates LOCAL_DATUM, but GDAL has a
            // tradition of emitting just LOCAL_CS["foo"]
            emptyPropertyMap);
    return EngineeringCRS::create(buildProperties(node), datum, cs);
}

// ---------------------------------------------------------------------------

DerivedEngineeringCRSNNPtr
WKTParser::Private::buildDerivedEngineeringCRS(const WKTNodeNNPtr &node) {
    const auto *nodeP = node->GP();
    auto &baseEngCRSNode = nodeP->lookForChild(WKTConstants::BASEENGCRS);
    // given the constraints enforced on calling code path
    assert(!isNull(baseEngCRSNode));

    auto baseEngCRS = buildEngineeringCRS(baseEngCRSNode);

    auto &derivingConversionNode =
        nodeP->lookForChild(WKTConstants::DERIVINGCONVERSION);
    if (isNull(derivingConversionNode)) {
        ThrowNotEnoughChildren(WKTConstants::DERIVINGCONVERSION);
    }
    auto derivingConversion = buildConversion(
        derivingConversionNode, UnitOfMeasure::NONE, UnitOfMeasure::NONE);

    auto &csNode = nodeP->lookForChild(WKTConstants::CS_);
    if (isNull(csNode)) {
        ThrowMissing(WKTConstants::CS_);
    }
    auto cs = buildCS(csNode, node, UnitOfMeasure::NONE);

    return DerivedEngineeringCRS::create(buildProperties(node), baseEngCRS,
                                         derivingConversion, cs);
}

// ---------------------------------------------------------------------------

ParametricCSNNPtr
WKTParser::Private::buildParametricCS(const WKTNodeNNPtr &parentNode) {

    auto &csNode = parentNode->GP()->lookForChild(WKTConstants::CS_);
    if (isNull(csNode) &&
        !ci_equal(parentNode->GP()->value(), WKTConstants::BASEPARAMCRS)) {
        ThrowMissing(WKTConstants::CS_);
    }
    auto cs = buildCS(csNode, parentNode, UnitOfMeasure::NONE);
    auto parametricCS = nn_dynamic_pointer_cast<ParametricCS>(cs);
    if (!parametricCS) {
        ThrowNotExpectedCSType("parametric");
    }
    return NN_NO_CHECK(parametricCS);
}

// ---------------------------------------------------------------------------

ParametricCRSNNPtr
WKTParser::Private::buildParametricCRS(const WKTNodeNNPtr &node) {
    auto &datumNode = node->GP()->lookForChild(WKTConstants::PDATUM,
                                               WKTConstants::PARAMETRICDATUM);
    if (isNull(datumNode)) {
        throw ParsingException("Missing PDATUM / PARAMETRICDATUM node");
    }

    return ParametricCRS::create(buildProperties(node),
                                 buildParametricDatum(datumNode),
                                 buildParametricCS(node));
}

// ---------------------------------------------------------------------------

DerivedParametricCRSNNPtr
WKTParser::Private::buildDerivedParametricCRS(const WKTNodeNNPtr &node) {
    const auto *nodeP = node->GP();
    auto &baseParamCRSNode = nodeP->lookForChild(WKTConstants::BASEPARAMCRS);
    // given the constraints enforced on calling code path
    assert(!isNull(baseParamCRSNode));

    auto &derivingConversionNode =
        nodeP->lookForChild(WKTConstants::DERIVINGCONVERSION);
    if (isNull(derivingConversionNode)) {
        ThrowNotEnoughChildren(WKTConstants::DERIVINGCONVERSION);
    }

    return DerivedParametricCRS::create(
        buildProperties(node), buildParametricCRS(baseParamCRSNode),
        buildConversion(derivingConversionNode, UnitOfMeasure::NONE,
                        UnitOfMeasure::NONE),
        buildParametricCS(node));
}

// ---------------------------------------------------------------------------

DerivedProjectedCRSNNPtr
WKTParser::Private::buildDerivedProjectedCRS(const WKTNodeNNPtr &node) {
    const auto *nodeP = node->GP();
    auto &baseProjCRSNode = nodeP->lookForChild(WKTConstants::BASEPROJCRS);
    if (isNull(baseProjCRSNode)) {
        ThrowNotEnoughChildren(WKTConstants::BASEPROJCRS);
    }
    auto baseProjCRS = buildProjectedCRS(baseProjCRSNode);

    auto &conversionNode =
        nodeP->lookForChild(WKTConstants::DERIVINGCONVERSION);
    if (isNull(conversionNode)) {
        ThrowNotEnoughChildren(WKTConstants::DERIVINGCONVERSION);
    }

    auto linearUnit = buildUnitInSubNode(node);
    auto angularUnit =
        baseProjCRS->baseCRS()->coordinateSystem()->axisList()[0]->unit();

    auto conversion = buildConversion(conversionNode, linearUnit, angularUnit);

    auto &csNode = nodeP->lookForChild(WKTConstants::CS_);
    if (isNull(csNode) && !ci_equal(nodeP->value(), WKTConstants::PROJCS)) {
        ThrowMissing(WKTConstants::CS_);
    }
    auto cs = buildCS(csNode, node, UnitOfMeasure::NONE);
    return DerivedProjectedCRS::create(buildProperties(node), baseProjCRS,
                                       conversion, cs);
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

CRSPtr WKTParser::Private::buildCRS(const WKTNodeNNPtr &node) {
    const auto *nodeP = node->GP();
    const std::string &name(nodeP->value());

    if (isGeodeticCRS(name)) {
        if (!isNull(nodeP->lookForChild(WKTConstants::BASEGEOGCRS,
                                        WKTConstants::BASEGEODCRS))) {
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
        if (!isNull(nodeP->lookForChild(WKTConstants::BASEVERTCRS))) {
            return util::nn_static_pointer_cast<CRS>(
                buildDerivedVerticalCRS(node));
        } else {
            return util::nn_static_pointer_cast<CRS>(buildVerticalCRS(node));
        }
    }

    if (ci_equal(name, WKTConstants::COMPD_CS) ||
        ci_equal(name, WKTConstants::COMPOUNDCRS)) {
        return util::nn_static_pointer_cast<CRS>(buildCompoundCRS(node));
    }

    if (ci_equal(name, WKTConstants::BOUNDCRS)) {
        return util::nn_static_pointer_cast<CRS>(buildBoundCRS(node));
    }

    if (ci_equal(name, WKTConstants::TIMECRS)) {
        if (!isNull(nodeP->lookForChild(WKTConstants::BASETIMECRS))) {
            return util::nn_static_pointer_cast<CRS>(
                buildDerivedTemporalCRS(node));
        } else {
            return util::nn_static_pointer_cast<CRS>(buildTemporalCRS(node));
        }
    }

    if (ci_equal(name, WKTConstants::DERIVEDPROJCRS)) {
        return util::nn_static_pointer_cast<CRS>(
            buildDerivedProjectedCRS(node));
    }

    if (ci_equal(name, WKTConstants::ENGCRS) ||
        ci_equal(name, WKTConstants::ENGINEERINGCRS)) {
        if (!isNull(nodeP->lookForChild(WKTConstants::BASEENGCRS))) {
            return util::nn_static_pointer_cast<CRS>(
                buildDerivedEngineeringCRS(node));
        } else {
            return util::nn_static_pointer_cast<CRS>(buildEngineeringCRS(node));
        }
    }

    if (ci_equal(name, WKTConstants::LOCAL_CS)) {
        return util::nn_static_pointer_cast<CRS>(
            buildEngineeringCRSFromLocalCS(node));
    }

    if (ci_equal(name, WKTConstants::PARAMETRICCRS)) {
        if (!isNull(nodeP->lookForChild(WKTConstants::BASEPARAMCRS))) {
            return util::nn_static_pointer_cast<CRS>(
                buildDerivedParametricCRS(node));
        } else {
            return util::nn_static_pointer_cast<CRS>(buildParametricCRS(node));
        }
    }

    return nullptr;
}

// ---------------------------------------------------------------------------

BaseObjectNNPtr WKTParser::Private::build(const WKTNodeNNPtr &node) {
    const auto *nodeP = node->GP();
    const std::string &name(nodeP->value());

    auto crs = buildCRS(node);
    if (crs) {
        if (!toWGS84Parameters_.empty()) {
            return util::nn_static_pointer_cast<BaseObject>(
                BoundCRS::createFromTOWGS84(NN_NO_CHECK(crs),
                                            toWGS84Parameters_));
        }
        if (!datumPROJ4Grids_.empty()) {
            return util::nn_static_pointer_cast<BaseObject>(
                BoundCRS::createFromNadgrids(NN_NO_CHECK(crs),
                                             datumPROJ4Grids_));
        }
        return util::nn_static_pointer_cast<BaseObject>(NN_NO_CHECK(crs));
    }

    if (ci_equal(name, WKTConstants::DATUM) ||
        ci_equal(name, WKTConstants::GEODETICDATUM) ||
        ci_equal(name, WKTConstants::TRF)) {
        return util::nn_static_pointer_cast<BaseObject>(
            buildGeodeticReferenceFrame(node, PrimeMeridian::GREENWICH,
                                        null_node));
    }

    if (ci_equal(name, WKTConstants::ENSEMBLE)) {
        return util::nn_static_pointer_cast<BaseObject>(buildDatumEnsemble(
            node, PrimeMeridian::GREENWICH,
            !isNull(nodeP->lookForChild(WKTConstants::ELLIPSOID))));
    }

    if (ci_equal(name, WKTConstants::VDATUM) ||
        ci_equal(name, WKTConstants::VERT_DATUM) ||
        ci_equal(name, WKTConstants::VERTICALDATUM) ||
        ci_equal(name, WKTConstants::VRF)) {
        return util::nn_static_pointer_cast<BaseObject>(
            buildVerticalReferenceFrame(node, null_node));
    }

    if (ci_equal(name, WKTConstants::TDATUM) ||
        ci_equal(name, WKTConstants::TIMEDATUM)) {
        return util::nn_static_pointer_cast<BaseObject>(
            buildTemporalDatum(node));
    }

    if (ci_equal(name, WKTConstants::EDATUM) ||
        ci_equal(name, WKTConstants::ENGINEERINGDATUM)) {
        return util::nn_static_pointer_cast<BaseObject>(
            buildEngineeringDatum(node));
    }

    if (ci_equal(name, WKTConstants::PDATUM) ||
        ci_equal(name, WKTConstants::PARAMETRICDATUM)) {
        return util::nn_static_pointer_cast<BaseObject>(
            buildParametricDatum(node));
    }

    if (ci_equal(name, WKTConstants::ELLIPSOID) ||
        ci_equal(name, WKTConstants::SPHEROID)) {
        return util::nn_static_pointer_cast<BaseObject>(buildEllipsoid(node));
    }

    if (ci_equal(name, WKTConstants::COORDINATEOPERATION)) {
        auto transf = buildCoordinateOperation(node);

        const char *prefixes[] = {
            "PROJ-based operation method: ",
            "PROJ-based operation method (approximate): "};
        for (const char *prefix : prefixes) {
            if (starts_with(transf->method()->nameStr(), prefix)) {
                auto projString =
                    transf->method()->nameStr().substr(strlen(prefix));
                return util::nn_static_pointer_cast<BaseObject>(
                    PROJBasedOperation::create(
                        PropertyMap(), projString, transf->sourceCRS(),
                        transf->targetCRS(),
                        transf->coordinateOperationAccuracies()));
            }
        }

        return util::nn_static_pointer_cast<BaseObject>(transf);
    }

    if (ci_equal(name, WKTConstants::CONVERSION)) {
        auto conv =
            buildConversion(node, UnitOfMeasure::METRE, UnitOfMeasure::DEGREE);

        if (starts_with(conv->method()->nameStr(),
                        "PROJ-based operation method: ")) {
            auto projString = conv->method()->nameStr().substr(
                strlen("PROJ-based operation method: "));
            return util::nn_static_pointer_cast<BaseObject>(
                PROJBasedOperation::create(PropertyMap(), projString, nullptr,
                                           nullptr, {}));
        }

        return util::nn_static_pointer_cast<BaseObject>(conv);
    }

    if (ci_equal(name, WKTConstants::CONCATENATEDOPERATION)) {
        return util::nn_static_pointer_cast<BaseObject>(
            buildConcatenatedOperation(node));
    }

    if (ci_equal(name, WKTConstants::ID) ||
        ci_equal(name, WKTConstants::AUTHORITY)) {
        return util::nn_static_pointer_cast<BaseObject>(
            NN_NO_CHECK(buildId(node, false, false)));
    }

    throw ParsingException(concat("unhandled keyword: ", name));
}

// ---------------------------------------------------------------------------

static BaseObjectNNPtr createFromUserInput(const std::string &text,
                                           const DatabaseContextPtr &dbContext,
                                           bool usePROJ4InitRules,
                                           PJ_CONTEXT *ctx) {

    if (!ci_starts_with(text, "step proj=") &&
        !ci_starts_with(text, "step +proj=")) {
        for (const auto &wktConstant : WKTConstants::constants()) {
            if (ci_starts_with(text, wktConstant)) {
                for (auto wkt = text.c_str() + wktConstant.size(); *wkt != '\0';
                     ++wkt) {
                    if (isspace(static_cast<unsigned char>(*wkt)))
                        continue;
                    if (*wkt == '[') {
                        return WKTParser()
                            .attachDatabaseContext(dbContext)
                            .setStrict(false)
                            .createFromWKT(text);
                    }
                    break;
                }
            }
        }
    }

    const char *textWithoutPlusPrefix = text.c_str();
    if (textWithoutPlusPrefix[0] == '+')
        textWithoutPlusPrefix++;

    if (strncmp(textWithoutPlusPrefix, "proj=", strlen("proj=")) == 0 ||
        text.find(" +proj=") != std::string::npos ||
        text.find(" proj=") != std::string::npos ||
        strncmp(textWithoutPlusPrefix, "init=", strlen("init=")) == 0 ||
        text.find(" +init=") != std::string::npos ||
        text.find(" init=") != std::string::npos ||
        strncmp(textWithoutPlusPrefix, "title=", strlen("title=")) == 0) {
        return PROJStringParser()
            .attachDatabaseContext(dbContext)
            .attachContext(ctx)
            .setUsePROJ4InitRules(ctx != nullptr
                                      ? (proj_context_get_use_proj4_init_rules(
                                             ctx, false) == TRUE)
                                      : usePROJ4InitRules)
            .createFromPROJString(text);
    }

    auto tokens = split(text, ':');
    if (tokens.size() == 2) {
        if (!dbContext) {
            throw ParsingException("no database context specified");
        }
        DatabaseContextNNPtr dbContextNNPtr(NN_NO_CHECK(dbContext));
        const auto &authName = tokens[0];
        const auto &code = tokens[1];
        auto factory = AuthorityFactory::create(dbContextNNPtr, authName);
        try {
            return factory->createCoordinateReferenceSystem(code);
        } catch (...) {
            const auto authorities = dbContextNNPtr->getAuthorities();
            for (const auto &authCandidate : authorities) {
                if (ci_equal(authCandidate, authName)) {
                    factory =
                        AuthorityFactory::create(dbContextNNPtr, authCandidate);
                    try {
                        return factory->createCoordinateReferenceSystem(code);
                    } catch (...) {
                        // EPSG:4326+3855
                        auto tokensCode = split(code, '+');
                        if (tokensCode.size() == 2) {
                            auto crs1(factory->createCoordinateReferenceSystem(
                                tokensCode[0], false));
                            auto crs2(factory->createCoordinateReferenceSystem(
                                tokensCode[1], false));
                            return CompoundCRS::create(
                                util::PropertyMap().set(
                                    IdentifiedObject::NAME_KEY,
                                    crs1->nameStr() + " + " + crs2->nameStr()),
                                {crs1, crs2});
                        }
                        throw;
                    }
                }
            }
            throw;
        }
    }

    if (starts_with(text, "urn:ogc:def:crs,")) {
        if (!dbContext) {
            throw ParsingException("no database context specified");
        }
        auto tokensComma = split(text, ',');
        if (tokensComma.size() == 4 && starts_with(tokensComma[1], "crs:") &&
            starts_with(tokensComma[2], "cs:") &&
            starts_with(tokensComma[3], "coordinateOperation:")) {
            // OGC 07-092r2: para 7.5.4
            // URN combined references for projected or derived CRSs
            const auto &crsPart = tokensComma[1];
            const auto tokensCRS = split(crsPart, ':');
            if (tokensCRS.size() != 4) {
                throw ParsingException(
                    concat("invalid crs component: ", crsPart));
            }
            auto factoryCRS =
                AuthorityFactory::create(NN_NO_CHECK(dbContext), tokensCRS[1]);
            auto baseCRS =
                factoryCRS->createCoordinateReferenceSystem(tokensCRS[3], true);

            const auto &csPart = tokensComma[2];
            auto tokensCS = split(csPart, ':');
            if (tokensCS.size() != 4) {
                throw ParsingException(
                    concat("invalid cs component: ", csPart));
            }
            auto factoryCS =
                AuthorityFactory::create(NN_NO_CHECK(dbContext), tokensCS[1]);
            auto cs = factoryCS->createCoordinateSystem(tokensCS[3]);

            const auto &opPart = tokensComma[3];
            auto tokensOp = split(opPart, ':');
            if (tokensOp.size() != 4) {
                throw ParsingException(
                    concat("invalid coordinateOperation component: ", opPart));
            }
            auto factoryOp =
                AuthorityFactory::create(NN_NO_CHECK(dbContext), tokensOp[1]);
            auto op = factoryOp->createCoordinateOperation(tokensOp[3], true);

            if (dynamic_cast<GeographicCRS *>(baseCRS.get()) &&
                dynamic_cast<Conversion *>(op.get()) &&
                dynamic_cast<CartesianCS *>(cs.get())) {
                auto geogCRS = NN_NO_CHECK(
                    util::nn_dynamic_pointer_cast<GeographicCRS>(baseCRS));
                auto name = op->nameStr() + " / " + baseCRS->nameStr();
                if (geogCRS->coordinateSystem()->axisList().size() == 3 &&
                    baseCRS->nameStr().find("3D") == std::string::npos) {
                    name += " (3D)";
                }
                return ProjectedCRS::create(
                    util::PropertyMap().set(IdentifiedObject::NAME_KEY, name),
                    geogCRS,
                    NN_NO_CHECK(util::nn_dynamic_pointer_cast<Conversion>(op)),
                    NN_NO_CHECK(
                        util::nn_dynamic_pointer_cast<CartesianCS>(cs)));
            } else if (dynamic_cast<GeodeticCRS *>(baseCRS.get()) &&
                       !dynamic_cast<GeographicCRS *>(baseCRS.get()) &&
                       dynamic_cast<Conversion *>(op.get()) &&
                       dynamic_cast<CartesianCS *>(cs.get())) {
                return DerivedGeodeticCRS::create(
                    util::PropertyMap().set(IdentifiedObject::NAME_KEY,
                                            op->nameStr() + " / " +
                                                baseCRS->nameStr()),
                    NN_NO_CHECK(
                        util::nn_dynamic_pointer_cast<GeodeticCRS>(baseCRS)),
                    NN_NO_CHECK(util::nn_dynamic_pointer_cast<Conversion>(op)),
                    NN_NO_CHECK(
                        util::nn_dynamic_pointer_cast<CartesianCS>(cs)));
            } else if (dynamic_cast<GeographicCRS *>(baseCRS.get()) &&
                       dynamic_cast<Conversion *>(op.get()) &&
                       dynamic_cast<EllipsoidalCS *>(cs.get())) {
                return DerivedGeographicCRS::create(
                    util::PropertyMap().set(IdentifiedObject::NAME_KEY,
                                            op->nameStr() + " / " +
                                                baseCRS->nameStr()),
                    NN_NO_CHECK(
                        util::nn_dynamic_pointer_cast<GeodeticCRS>(baseCRS)),
                    NN_NO_CHECK(util::nn_dynamic_pointer_cast<Conversion>(op)),
                    NN_NO_CHECK(
                        util::nn_dynamic_pointer_cast<EllipsoidalCS>(cs)));
            } else if (dynamic_cast<ProjectedCRS *>(baseCRS.get()) &&
                       dynamic_cast<Conversion *>(op.get())) {
                return DerivedProjectedCRS::create(
                    util::PropertyMap().set(IdentifiedObject::NAME_KEY,
                                            op->nameStr() + " / " +
                                                baseCRS->nameStr()),
                    NN_NO_CHECK(
                        util::nn_dynamic_pointer_cast<ProjectedCRS>(baseCRS)),
                    NN_NO_CHECK(util::nn_dynamic_pointer_cast<Conversion>(op)),
                    cs);
            } else if (dynamic_cast<VerticalCRS *>(baseCRS.get()) &&
                       dynamic_cast<Conversion *>(op.get()) &&
                       dynamic_cast<VerticalCS *>(cs.get())) {
                return DerivedVerticalCRS::create(
                    util::PropertyMap().set(IdentifiedObject::NAME_KEY,
                                            op->nameStr() + " / " +
                                                baseCRS->nameStr()),
                    NN_NO_CHECK(
                        util::nn_dynamic_pointer_cast<VerticalCRS>(baseCRS)),
                    NN_NO_CHECK(util::nn_dynamic_pointer_cast<Conversion>(op)),
                    NN_NO_CHECK(util::nn_dynamic_pointer_cast<VerticalCS>(cs)));
            } else {
                throw ParsingException("unsupported combination of baseCRS, CS "
                                       "and coordinateOperation for a "
                                       "DerivedCRS");
            }
        }

        // OGC 07-092r2: para 7.5.2
        // URN combined references for compound coordinate reference systems
        std::vector<CRSNNPtr> components;
        std::string name;
        for (size_t i = 1; i < tokensComma.size(); i++) {
            tokens = split(tokensComma[i], ':');
            if (tokens.size() != 4) {
                throw ParsingException(
                    concat("invalid crs component: ", tokensComma[i]));
            }
            const auto &type = tokens[0];
            auto factory =
                AuthorityFactory::create(NN_NO_CHECK(dbContext), tokens[1]);
            const auto &code = tokens[3];
            if (type == "crs") {
                auto crs(factory->createCoordinateReferenceSystem(code, false));
                components.emplace_back(crs);
                if (!name.empty()) {
                    name += " + ";
                }
                name += crs->nameStr();
            } else {
                throw ParsingException(
                    concat("unexpected object type: ", type));
            }
        }
        return CompoundCRS::create(
            util::PropertyMap().set(IdentifiedObject::NAME_KEY, name),
            components);
    }

    // OGC 07-092r2: para 7.5.3
    // 7.5.3 URN combined references for concatenated operations
    if (starts_with(text, "urn:ogc:def:coordinateOperation,")) {
        if (!dbContext) {
            throw ParsingException("no database context specified");
        }
        auto tokensComma = split(text, ',');
        std::vector<CoordinateOperationNNPtr> components;
        for (size_t i = 1; i < tokensComma.size(); i++) {
            tokens = split(tokensComma[i], ':');
            if (tokens.size() != 4) {
                throw ParsingException(concat(
                    "invalid coordinateOperation component: ", tokensComma[i]));
            }
            const auto &type = tokens[0];
            auto factory =
                AuthorityFactory::create(NN_NO_CHECK(dbContext), tokens[1]);
            const auto &code = tokens[3];
            if (type == "coordinateOperation") {
                auto op(factory->createCoordinateOperation(code, false));
                components.emplace_back(op);
            } else {
                throw ParsingException(
                    concat("unexpected object type: ", type));
            }
        }
        return ConcatenatedOperation::createComputeMetadata(components, true);
    }

    // urn:ogc:def:crs:EPSG::4326
    if (tokens.size() == 7) {
        if (!dbContext) {
            throw ParsingException("no database context specified");
        }
        const auto &type = tokens[3];
        auto factory =
            AuthorityFactory::create(NN_NO_CHECK(dbContext), tokens[4]);
        const auto &code = tokens[6];
        if (type == "crs") {
            return factory->createCoordinateReferenceSystem(code);
        }
        if (type == "coordinateOperation") {
            return factory->createCoordinateOperation(code, true);
        }
        if (type == "datum") {
            return factory->createDatum(code);
        }
        if (type == "ellipsoid") {
            return factory->createEllipsoid(code);
        }
        if (type == "meridian") {
            return factory->createPrimeMeridian(code);
        }
        throw ParsingException(concat("unhandled object type: ", type));
    }

    if (dbContext) {
        auto factory =
            AuthorityFactory::create(NN_NO_CHECK(dbContext), std::string());
        // First pass: exact match on CRS objects
        // Second pass: exact match on other objects
        // Third pass: approximate match on CRS objects
        // Fourth pass: approximate match on other objects
        constexpr size_t limitResultCount = 10;
        for (int pass = 0; pass <= 3; ++pass) {
            const bool approximateMatch = (pass >= 2);
            auto res = factory->createObjectsFromName(
                text,
                (pass == 0 || pass == 2)
                    ? std::vector<
                          AuthorityFactory::ObjectType>{AuthorityFactory::
                                                            ObjectType::CRS}
                    : std::vector<
                          AuthorityFactory::
                              ObjectType>{AuthorityFactory::ObjectType::
                                              ELLIPSOID,
                                          AuthorityFactory::ObjectType::DATUM,
                                          AuthorityFactory::ObjectType::
                                              COORDINATE_OPERATION},
                approximateMatch, limitResultCount);
            if (res.size() == 1) {
                return res.front();
            }
            if (res.size() > 1) {
                if (pass == 0 || pass == 2) {
                    for (size_t ndim = 2; ndim <= 3; ndim++) {
                        for (const auto &obj : res) {
                            auto crs =
                                dynamic_cast<crs::GeographicCRS *>(obj.get());
                            if (crs &&
                                crs->coordinateSystem()->axisList().size() ==
                                    ndim) {
                                return obj;
                            }
                        }
                    }
                }

                std::string msg("several objects matching this name: ");
                bool first = true;
                for (const auto &obj : res) {
                    if (msg.size() > 200) {
                        msg += ", ...";
                        break;
                    }
                    if (!first) {
                        msg += ", ";
                    }
                    first = false;
                    msg += obj->nameStr();
                }
                throw ParsingException(msg);
            }
        }
    }

    throw ParsingException("unrecognized format / unknown name");
}
//! @endcond

// ---------------------------------------------------------------------------

/** \brief Instantiate a sub-class of BaseObject from a user specified text.
 *
 * The text can be a:
 * <ul>
 * <li>WKT string</li>
 * <li>PROJ string</li>
 * <li>database code, prefixed by its authoriy. e.g. "EPSG:4326"</li>
 * <li>OGC URN. e.g. "urn:ogc:def:crs:EPSG::4326",
 *     "urn:ogc:def:coordinateOperation:EPSG::1671",
 *     "urn:ogc:def:ellipsoid:EPSG::7001"
 *     or "urn:ogc:def:datum:EPSG::6326"</li>
 * <li> OGC URN combining references for compound coordinate reference systems
 *      e.g. "urn:ogc:def:crs,crs:EPSG::2393,crs:EPSG::5717"
 *      We also accept a custom abbreviated syntax EPSG:2393+5717
 * </li>
 * <li> OGC URN combining references for references for projected or derived
 * CRSs
 *      e.g. for Projected 3D CRS "UTM zone 31N / WGS 84 (3D)"
 *      "urn:ogc:def:crs,crs:EPSG::4979,cs:PROJ::ENh,coordinateOperation:EPSG::16031"
 * </li>
 * <li> OGC URN combining references for concatenated operations
 *      e.g.
 * "urn:ogc:def:coordinateOperation,coordinateOperation:EPSG::3895,coordinateOperation:EPSG::1618"</li>
 * <li>an Object name. e.g "WGS 84", "WGS 84 / UTM zone 31N". In that case as
 *     uniqueness is not guaranteed, the function may apply heuristics to
 *     determine the appropriate best match.</li>
 * </ul>
 *
 * @param text One of the above mentioned text format
 * @param dbContext Database context, or nullptr (in which case database
 * lookups will not work)
 * @param usePROJ4InitRules When set to true,
 * init=epsg:XXXX syntax will be allowed and will be interpreted according to
 * PROJ.4 and PROJ.5 rules, that is geodeticCRS will have longitude, latitude
 * order and will expect/output coordinates in radians. ProjectedCRS will have
 * easting, northing axis order (except the ones with Transverse Mercator South
 * Orientated projection). In that mode, the epsg:XXXX syntax will be also
 * interprated the same way.
 * @throw ParsingException
 */
BaseObjectNNPtr createFromUserInput(const std::string &text,
                                    const DatabaseContextPtr &dbContext,
                                    bool usePROJ4InitRules) {
    return createFromUserInput(text, dbContext, usePROJ4InitRules, nullptr);
}

// ---------------------------------------------------------------------------

/** \brief Instantiate a sub-class of BaseObject from a user specified text.
 *
 * The text can be a:
 * <ul>
 * <li>WKT string</li>
 * <li>PROJ string</li>
 * <li>database code, prefixed by its authoriy. e.g. "EPSG:4326"</li>
 * <li>OGC URN. e.g. "urn:ogc:def:crs:EPSG::4326",
 *     "urn:ogc:def:coordinateOperation:EPSG::1671",
 *     "urn:ogc:def:ellipsoid:EPSG::7001"
 *     or "urn:ogc:def:datum:EPSG::6326"</li>
 * <li> OGC URN combining references for compound coordinate reference systems
 *      e.g. "urn:ogc:def:crs,crs:EPSG::2393,crs:EPSG::5717"
 *      We also accept a custom abbreviated syntax EPSG:2393+5717
 * </li>
 * <li> OGC URN combining references for references for projected or derived
 * CRSs
 *      e.g. for Projected 3D CRS "UTM zone 31N / WGS 84 (3D)"
 *      "urn:ogc:def:crs,crs:EPSG::4979,cs:PROJ::ENh,coordinateOperation:EPSG::16031"
 * </li>
 * <li> OGC URN combining references for concatenated operations
 *      e.g.
 * "urn:ogc:def:coordinateOperation,coordinateOperation:EPSG::3895,coordinateOperation:EPSG::1618"</li>
 * <li>an Object name. e.g "WGS 84", "WGS 84 / UTM zone 31N". In that case as
 *     uniqueness is not guaranteed, the function may apply heuristics to
 *     determine the appropriate best match.</li>
 * </ul>
 *
 * @param text One of the above mentioned text format
 * @param ctx PROJ context
 * @throw ParsingException
 */
BaseObjectNNPtr createFromUserInput(const std::string &text, PJ_CONTEXT *ctx) {
    return createFromUserInput(
        text, ctx != nullptr && ctx->cpp_context
                  ? ctx->cpp_context->databaseContext.as_nullable()
                  : nullptr,
        false, ctx);
}

// ---------------------------------------------------------------------------

/** \brief Instantiate a sub-class of BaseObject from a WKT string.
 *
 * By default, validation is strict (to the extent of the checks that are
 * actually implemented. Currently only WKT1 strict grammar is checked), and
 * any issue detected will cause an exception to be thrown, unless
 * setStrict(false) is called priorly.
 *
 * In non-strict mode, non-fatal issues will be recovered and simply listed
 * in warningList(). This does not prevent more severe errors to cause an
 * exception to be thrown.
 *
 * @throw ParsingException
 */
BaseObjectNNPtr WKTParser::createFromWKT(const std::string &wkt) {
    WKTNodeNNPtr root = WKTNode::createFrom(wkt);
    auto obj = d->build(root);

    const auto dialect = guessDialect(wkt);
    if (dialect == WKTGuessedDialect::WKT1_GDAL ||
        dialect == WKTGuessedDialect::WKT1_ESRI) {
        auto errorMsg = pj_wkt1_parse(wkt);
        if (!errorMsg.empty()) {
            d->emitRecoverableWarning(errorMsg);
        }
    } else if (dialect == WKTGuessedDialect::WKT2_2015 ||
               dialect == WKTGuessedDialect::WKT2_2018) {
        auto errorMsg = pj_wkt2_parse(wkt);
        if (!errorMsg.empty()) {
            d->emitRecoverableWarning(errorMsg);
        }
    }

    return obj;
}

// ---------------------------------------------------------------------------

/** \brief Attach a database context, to allow queries in it if needed.
 */
WKTParser &
WKTParser::attachDatabaseContext(const DatabaseContextPtr &dbContext) {
    d->dbContext_ = dbContext;
    return *this;
}

// ---------------------------------------------------------------------------

/** \brief Guess the "dialect" of the WKT string.
 */
WKTParser::WKTGuessedDialect
WKTParser::guessDialect(const std::string &wkt) noexcept {
    const std::string *const wkt1_keywords[] = {
        &WKTConstants::GEOCCS, &WKTConstants::GEOGCS,  &WKTConstants::COMPD_CS,
        &WKTConstants::PROJCS, &WKTConstants::VERT_CS, &WKTConstants::LOCAL_CS};
    for (const auto &pointerKeyword : wkt1_keywords) {
        if (ci_starts_with(wkt, *pointerKeyword)) {

            if (ci_find(wkt, "GEOGCS[\"GCS_") != std::string::npos) {
                return WKTGuessedDialect::WKT1_ESRI;
            }

            return WKTGuessedDialect::WKT1_GDAL;
        }
    }

    const std::string *const wkt2_2018_only_keywords[] = {
        &WKTConstants::GEOGCRS,
        // contained in previous one
        // &WKTConstants::BASEGEOGCRS,
        &WKTConstants::CONCATENATEDOPERATION, &WKTConstants::USAGE,
        &WKTConstants::DYNAMIC, &WKTConstants::FRAMEEPOCH, &WKTConstants::MODEL,
        &WKTConstants::VELOCITYGRID, &WKTConstants::ENSEMBLE,
        &WKTConstants::DERIVEDPROJCRS, &WKTConstants::BASEPROJCRS,
        &WKTConstants::GEOGRAPHICCRS, &WKTConstants::TRF, &WKTConstants::VRF};

    for (const auto &pointerKeyword : wkt2_2018_only_keywords) {
        auto pos = ci_find(wkt, *pointerKeyword);
        if (pos != std::string::npos &&
            wkt[pos + pointerKeyword->size()] == '[') {
            return WKTGuessedDialect::WKT2_2018;
        }
    }
    static const char *const wkt2_2018_only_substrings[] = {
        "CS[TemporalDateTime,", "CS[TemporalCount,", "CS[TemporalMeasure,",
    };
    for (const auto &substrings : wkt2_2018_only_substrings) {
        if (ci_find(wkt, substrings) != std::string::npos) {
            return WKTGuessedDialect::WKT2_2018;
        }
    }

    for (const auto &wktConstant : WKTConstants::constants()) {
        if (ci_starts_with(wkt, wktConstant)) {
            for (auto wktPtr = wkt.c_str() + wktConstant.size();
                 *wktPtr != '\0'; ++wktPtr) {
                if (isspace(static_cast<unsigned char>(*wktPtr)))
                    continue;
                if (*wktPtr == '[') {
                    return WKTGuessedDialect::WKT2_2015;
                }
                break;
            }
        }
    }

    return WKTGuessedDialect::NOT_WKT;
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

void FormattingException::Throw(const char *msg) {
    throw FormattingException(msg);
}

// ---------------------------------------------------------------------------

void FormattingException::Throw(const std::string &msg) {
    throw FormattingException(msg);
}

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

// ---------------------------------------------------------------------------

std::string IPROJStringExportable::exportToPROJString(
    PROJStringFormatter *formatter) const {
    const bool bIsCRS = dynamic_cast<const crs::CRS *>(this) != nullptr;
    if (bIsCRS) {
        formatter->setCRSExport(true);
    }
    _exportToPROJString(formatter);
    if (formatter->getAddNoDefs() && bIsCRS) {
        if (!formatter->hasParam("no_defs")) {
            formatter->addParam("no_defs");
        }
    }
    if (bIsCRS) {
        if (!formatter->hasParam("type")) {
            formatter->addParam("type", "crs");
        }
        formatter->setCRSExport(false);
    }
    return formatter->toString();
}
//! @endcond

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress

struct Step {
    std::string name{};
    bool isInit = false;
    bool inverted{false};

    struct KeyValue {
        std::string key{};
        std::string value{};
        bool usedByParser = false; // only for PROJStringParser used

        explicit KeyValue(const std::string &keyIn) : key(keyIn) {}

        KeyValue(const char *keyIn, const std::string &valueIn);

        KeyValue(const std::string &keyIn, const std::string &valueIn)
            : key(keyIn), value(valueIn) {}

        // cppcheck-suppress functionStatic
        bool keyEquals(const char *otherKey) const noexcept {
            return key == otherKey;
        }

        // cppcheck-suppress functionStatic
        bool equals(const char *otherKey, const char *otherVal) const noexcept {
            return key == otherKey && value == otherVal;
        }

        bool operator!=(const KeyValue &other) const noexcept {
            return key != other.key || value != other.value;
        }
    };

    std::vector<KeyValue> paramValues{};
};

Step::KeyValue::KeyValue(const char *keyIn, const std::string &valueIn)
    : key(keyIn), value(valueIn) {}

struct PROJStringFormatter::Private {
    PROJStringFormatter::Convention convention_ =
        PROJStringFormatter::Convention::PROJ_5;
    std::vector<double> toWGS84Parameters_{};
    std::string vDatumExtension_{};
    std::string hDatumExtension_{};

    std::list<Step> steps_{};
    std::vector<Step::KeyValue> globalParamValues_{};

    struct InversionStackElt {
        std::list<Step>::iterator startIter{};
        bool iterValid = false;
        bool currentInversionState = false;
    };
    std::vector<InversionStackElt> inversionStack_{InversionStackElt()};
    bool omitProjLongLatIfPossible_ = false;
    std::vector<bool> omitZUnitConversion_{false};
    DatabaseContextPtr dbContext_{};
    bool useApproxTMerc_ = false;
    bool addNoDefs_ = true;
    bool coordOperationOptimizations_ = false;
    bool crsExport_ = false;
    bool dropEarlyBindingsTerms_ = false;

    std::string result_{};

    // cppcheck-suppress functionStatic
    void appendToResult(const char *str);

    // cppcheck-suppress functionStatic
    void addStep();
};

//! @endcond

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
PROJStringFormatter::PROJStringFormatter(Convention conventionIn,
                                         const DatabaseContextPtr &dbContext)
    : d(internal::make_unique<Private>()) {
    d->convention_ = conventionIn;
    d->dbContext_ = dbContext;
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
 * @param dbContext Database context (can help to find alternative grid names).
 * May be nullptr
 * @return new formatter.
 */
PROJStringFormatterNNPtr
PROJStringFormatter::create(Convention conventionIn,
                            DatabaseContextPtr dbContext) {
    return NN_NO_CHECK(PROJStringFormatter::make_unique<PROJStringFormatter>(
        conventionIn, dbContext));
}

// ---------------------------------------------------------------------------

/** \brief Set whether approximate Transverse Mercator or UTM should be used */
void PROJStringFormatter::setUseApproxTMerc(bool flag) {
    d->useApproxTMerc_ = flag;
}

// ---------------------------------------------------------------------------

/** \brief Returns the PROJ string. */
const std::string &PROJStringFormatter::toString() const {

    assert(d->inversionStack_.size() == 1);

    d->result_.clear();

    for (auto iter = d->steps_.begin(); iter != d->steps_.end();) {
        // Remove no-op helmert
        auto &step = *iter;
        const auto paramCount = step.paramValues.size();
        if (step.name == "helmert" && (paramCount == 3 || paramCount == 8) &&
            step.paramValues[0].equals("x", "0") &&
            step.paramValues[1].equals("y", "0") &&
            step.paramValues[2].equals("z", "0") &&
            (paramCount == 3 ||
             (step.paramValues[3].equals("rx", "0") &&
              step.paramValues[4].equals("ry", "0") &&
              step.paramValues[5].equals("rz", "0") &&
              step.paramValues[6].equals("s", "0") &&
              step.paramValues[7].keyEquals("convention")))) {
            iter = d->steps_.erase(iter);
        } else if (d->coordOperationOptimizations_ &&
                   step.name == "unitconvert" && paramCount == 2 &&
                   step.paramValues[0].keyEquals("xy_in") &&
                   step.paramValues[1].keyEquals("xy_out") &&
                   step.paramValues[0].value == step.paramValues[1].value) {
            iter = d->steps_.erase(iter);
        } else if (step.name == "push" && step.inverted) {
            step.name = "pop";
            step.inverted = false;
        } else if (step.name == "pop" && step.inverted) {
            step.name = "push";
            step.inverted = false;
        } else {
            ++iter;
        }
    }

    for (auto &step : d->steps_) {
        if (!step.inverted) {
            continue;
        }

        const auto paramCount = step.paramValues.size();

        // axisswap order=2,1 is its own inverse
        if (step.name == "axisswap" && paramCount == 1 &&
            step.paramValues[0].equals("order", "2,1")) {
            step.inverted = false;
            continue;
        }

        // handle unitconvert inverse
        if (step.name == "unitconvert" && paramCount == 2 &&
            step.paramValues[0].keyEquals("xy_in") &&
            step.paramValues[1].keyEquals("xy_out")) {
            std::swap(step.paramValues[0].value, step.paramValues[1].value);
            step.inverted = false;
            continue;
        }

        if (step.name == "unitconvert" && paramCount == 2 &&
            step.paramValues[0].keyEquals("z_in") &&
            step.paramValues[1].keyEquals("z_out")) {
            std::swap(step.paramValues[0].value, step.paramValues[1].value);
            step.inverted = false;
            continue;
        }

        if (step.name == "unitconvert" && paramCount == 4 &&
            step.paramValues[0].keyEquals("xy_in") &&
            step.paramValues[1].keyEquals("z_in") &&
            step.paramValues[2].keyEquals("xy_out") &&
            step.paramValues[3].keyEquals("z_out")) {
            std::swap(step.paramValues[0].value, step.paramValues[2].value);
            std::swap(step.paramValues[1].value, step.paramValues[3].value);
            step.inverted = false;
            continue;
        }
    }

    bool changeDone;
    do {
        changeDone = false;
        auto iterPrev = d->steps_.begin();
        if (iterPrev == d->steps_.end()) {
            break;
        }
        auto iterCur = iterPrev;
        iterCur++;
        for (size_t i = 1; i < d->steps_.size(); ++i, ++iterCur, ++iterPrev) {

            auto &prevStep = *iterPrev;
            auto &curStep = *iterCur;

            const auto curStepParamCount = curStep.paramValues.size();
            const auto prevStepParamCount = prevStep.paramValues.size();

            // longlat (or its inverse) with ellipsoid only is a no-op
            // do that only for an internal step
            if (i + 1 < d->steps_.size() && curStep.name == "longlat" &&
                curStepParamCount == 1 &&
                curStep.paramValues[0].keyEquals("ellps")) {
                d->steps_.erase(iterCur);
                changeDone = true;
                break;
            }

            // push v_x followed by pop v_x is a no-op.
            if (curStep.name == "pop" && prevStep.name == "push" &&
                !curStep.inverted && !prevStep.inverted &&
                curStepParamCount == 1 && prevStepParamCount == 1 &&
                curStep.paramValues[0].key == prevStep.paramValues[0].key) {
                ++iterCur;
                d->steps_.erase(iterPrev, iterCur);
                changeDone = true;
                break;
            }

            // pop v_x followed by push v_x is, almost, a no-op. For our
            // purposes,
            // we consider it as a no-op for better pipeline optimizations.
            if (curStep.name == "push" && prevStep.name == "pop" &&
                !curStep.inverted && !prevStep.inverted &&
                curStepParamCount == 1 && prevStepParamCount == 1 &&
                curStep.paramValues[0].key == prevStep.paramValues[0].key) {
                ++iterCur;
                d->steps_.erase(iterPrev, iterCur);
                changeDone = true;
                break;
            }

            // unitconvert (xy) followed by its inverse is a no-op
            if (curStep.name == "unitconvert" &&
                prevStep.name == "unitconvert" && !curStep.inverted &&
                !prevStep.inverted && curStepParamCount == 2 &&
                prevStepParamCount == 2 &&
                curStep.paramValues[0].keyEquals("xy_in") &&
                prevStep.paramValues[0].keyEquals("xy_in") &&
                curStep.paramValues[1].keyEquals("xy_out") &&
                prevStep.paramValues[1].keyEquals("xy_out") &&
                curStep.paramValues[0].value == prevStep.paramValues[1].value &&
                curStep.paramValues[1].value == prevStep.paramValues[0].value) {
                ++iterCur;
                d->steps_.erase(iterPrev, iterCur);
                changeDone = true;
                break;
            }

            // unitconvert (z) followed by its inverse is a no-op
            if (curStep.name == "unitconvert" &&
                prevStep.name == "unitconvert" && !curStep.inverted &&
                !prevStep.inverted && curStepParamCount == 2 &&
                prevStepParamCount == 2 &&
                curStep.paramValues[0].keyEquals("z_in") &&
                prevStep.paramValues[0].keyEquals("z_in") &&
                curStep.paramValues[1].keyEquals("z_out") &&
                prevStep.paramValues[1].keyEquals("z_out") &&
                curStep.paramValues[0].value == prevStep.paramValues[1].value &&
                curStep.paramValues[1].value == prevStep.paramValues[0].value) {
                ++iterCur;
                d->steps_.erase(iterPrev, iterCur);
                changeDone = true;
                break;
            }

            // unitconvert (xyz) followed by its inverse is a no-op
            if (curStep.name == "unitconvert" &&
                prevStep.name == "unitconvert" && !curStep.inverted &&
                !prevStep.inverted && curStepParamCount == 4 &&
                prevStepParamCount == 4 &&
                curStep.paramValues[0].keyEquals("xy_in") &&
                prevStep.paramValues[0].keyEquals("xy_in") &&
                curStep.paramValues[1].keyEquals("z_in") &&
                prevStep.paramValues[1].keyEquals("z_in") &&
                curStep.paramValues[2].keyEquals("xy_out") &&
                prevStep.paramValues[2].keyEquals("xy_out") &&
                curStep.paramValues[3].keyEquals("z_out") &&
                prevStep.paramValues[3].keyEquals("z_out") &&
                curStep.paramValues[0].value == prevStep.paramValues[2].value &&
                curStep.paramValues[1].value == prevStep.paramValues[3].value &&
                curStep.paramValues[2].value == prevStep.paramValues[0].value &&
                curStep.paramValues[3].value == prevStep.paramValues[1].value) {
                ++iterCur;
                d->steps_.erase(iterPrev, iterCur);
                changeDone = true;
                break;
            }

            // combine unitconvert (xy) and unitconvert (z)
            for (int k = 0; k < 2; ++k) {
                auto &first = (k == 0) ? curStep : prevStep;
                auto &second = (k == 0) ? prevStep : curStep;
                if (first.name == "unitconvert" &&
                    second.name == "unitconvert" && !first.inverted &&
                    !second.inverted && first.paramValues.size() == 2 &&
                    second.paramValues.size() == 2 &&
                    second.paramValues[0].keyEquals("xy_in") &&
                    second.paramValues[1].keyEquals("xy_out") &&
                    first.paramValues[0].keyEquals("z_in") &&
                    first.paramValues[1].keyEquals("z_out")) {

                    auto xy_in = second.paramValues[0].value;
                    auto xy_out = second.paramValues[1].value;
                    auto z_in = first.paramValues[0].value;
                    auto z_out = first.paramValues[1].value;
                    d->steps_.erase(iterPrev, iterCur);
                    iterCur->paramValues.clear();
                    iterCur->paramValues.emplace_back(
                        Step::KeyValue("xy_in", xy_in));
                    iterCur->paramValues.emplace_back(
                        Step::KeyValue("z_in", z_in));
                    iterCur->paramValues.emplace_back(
                        Step::KeyValue("xy_out", xy_out));
                    iterCur->paramValues.emplace_back(
                        Step::KeyValue("z_out", z_out));
                    changeDone = true;
                    break;
                }
            }
            if (changeDone) {
                break;
            }

            // +step +proj=unitconvert +xy_in=X1 +xy_out=X2
            //  +step +proj=unitconvert +xy_in=X2 +z_in=Z1 +xy_out=X1 +z_out=Z2
            // ==> step +proj=unitconvert +z_in=Z1 +z_out=Z2
            for (int k = 0; k < 2; ++k) {
                auto &first = (k == 0) ? curStep : prevStep;
                auto &second = (k == 0) ? prevStep : curStep;
                if (first.name == "unitconvert" &&
                    second.name == "unitconvert" && !first.inverted &&
                    !second.inverted && first.paramValues.size() == 4 &&
                    second.paramValues.size() == 2 &&
                    first.paramValues[0].keyEquals("xy_in") &&
                    first.paramValues[1].keyEquals("z_in") &&
                    first.paramValues[2].keyEquals("xy_out") &&
                    first.paramValues[3].keyEquals("z_out") &&
                    second.paramValues[0].keyEquals("xy_in") &&
                    second.paramValues[1].keyEquals("xy_out") &&
                    first.paramValues[0].value == second.paramValues[1].value &&
                    first.paramValues[2].value == second.paramValues[0].value) {
                    auto z_in = first.paramValues[1].value;
                    auto z_out = first.paramValues[3].value;
                    if (z_in != z_out) {
                        d->steps_.erase(iterPrev, iterCur);
                        iterCur->paramValues.clear();
                        iterCur->paramValues.emplace_back(
                            Step::KeyValue("z_in", z_in));
                        iterCur->paramValues.emplace_back(
                            Step::KeyValue("z_out", z_out));
                    } else {
                        ++iterCur;
                        d->steps_.erase(iterPrev, iterCur);
                    }
                    changeDone = true;
                    break;
                }
            }
            if (changeDone) {
                break;
            }

            // unitconvert (1), axisswap order=2,1, unitconvert(2)  ==>
            // axisswap order=2,1, unitconvert (1), unitconvert(2) which
            // will get further optimized by previous case
            if (i + 1 < d->steps_.size() && prevStep.name == "unitconvert" &&
                curStep.name == "axisswap" && curStepParamCount == 1 &&
                curStep.paramValues[0].equals("order", "2,1")) {
                auto iterNext = iterCur;
                ++iterNext;
                auto &nextStep = *iterNext;
                if (nextStep.name == "unitconvert") {
                    std::swap(*iterPrev, *iterCur);
                    changeDone = true;
                    break;
                }
            }

            // axisswap order=2,1 followed by itself is a no-op
            if (curStep.name == "axisswap" && prevStep.name == "axisswap" &&
                curStepParamCount == 1 && prevStepParamCount == 1 &&
                curStep.paramValues[0].equals("order", "2,1") &&
                prevStep.paramValues[0].equals("order", "2,1")) {
                ++iterCur;
                d->steps_.erase(iterPrev, iterCur);
                changeDone = true;
                break;
            }

            // axisswap order=2,1, unitconvert, axisswap order=2,1 -> can
            // suppress axisswap
            if (i + 1 < d->steps_.size() && prevStep.name == "axisswap" &&
                curStep.name == "unitconvert" && prevStepParamCount == 1 &&
                prevStep.paramValues[0].equals("order", "2,1")) {
                auto iterNext = iterCur;
                ++iterNext;
                auto &nextStep = *iterNext;
                if (nextStep.name == "axisswap" &&
                    nextStep.paramValues.size() == 1 &&
                    nextStep.paramValues[0].equals("order", "2,1")) {
                    d->steps_.erase(iterPrev);
                    d->steps_.erase(iterNext);
                    changeDone = true;
                    break;
                }
            }

            // for practical purposes WGS84 and GRS80 ellipsoids are
            // equivalents (cartesian transform between both lead to differences
            // of the order of 1e-14 deg..).
            // No need to do a cart roundtrip for that...
            // and actually IGNF uses the GRS80 definition for the WGS84 datum
            if (curStep.name == "cart" && prevStep.name == "cart" &&
                curStep.inverted == !prevStep.inverted &&
                curStepParamCount == 1 && prevStepParamCount == 1 &&
                ((curStep.paramValues[0].equals("ellps", "WGS84") &&
                  prevStep.paramValues[0].equals("ellps", "GRS80")) ||
                 (curStep.paramValues[0].equals("ellps", "GRS80") &&
                  prevStep.paramValues[0].equals("ellps", "WGS84")))) {
                ++iterCur;
                d->steps_.erase(iterPrev, iterCur);
                changeDone = true;
                break;
            }

            if (curStep.name == "helmert" && prevStep.name == "helmert" &&
                !curStep.inverted && !prevStep.inverted &&
                curStepParamCount == 3 &&
                curStepParamCount == prevStepParamCount) {
                std::map<std::string, double> leftParamsMap;
                std::map<std::string, double> rightParamsMap;
                try {
                    for (const auto &kv : prevStep.paramValues) {
                        leftParamsMap[kv.key] = c_locale_stod(kv.value);
                    }
                    for (const auto &kv : curStep.paramValues) {
                        rightParamsMap[kv.key] = c_locale_stod(kv.value);
                    }
                } catch (const std::invalid_argument &) {
                    break;
                }
                const std::string x("x");
                const std::string y("y");
                const std::string z("z");
                if (leftParamsMap.find(x) != leftParamsMap.end() &&
                    leftParamsMap.find(y) != leftParamsMap.end() &&
                    leftParamsMap.find(z) != leftParamsMap.end() &&
                    rightParamsMap.find(x) != rightParamsMap.end() &&
                    rightParamsMap.find(y) != rightParamsMap.end() &&
                    rightParamsMap.find(z) != rightParamsMap.end()) {

                    const double xSum = leftParamsMap[x] + rightParamsMap[x];
                    const double ySum = leftParamsMap[y] + rightParamsMap[y];
                    const double zSum = leftParamsMap[z] + rightParamsMap[z];
                    if (xSum == 0.0 && ySum == 0.0 && zSum == 0.0) {
                        ++iterCur;
                        d->steps_.erase(iterPrev, iterCur);
                    } else {
                        prevStep.paramValues[0] =
                            Step::KeyValue("x", internal::toString(xSum));
                        prevStep.paramValues[1] =
                            Step::KeyValue("y", internal::toString(ySum));
                        prevStep.paramValues[2] =
                            Step::KeyValue("z", internal::toString(zSum));

                        d->steps_.erase(iterCur);
                    }
                    changeDone = true;
                    break;
                }
            }

            // hermert followed by its inverse is a no-op
            if (curStep.name == "helmert" && prevStep.name == "helmert" &&
                !curStep.inverted && !prevStep.inverted &&
                curStepParamCount == prevStepParamCount) {
                std::set<std::string> leftParamsSet;
                std::set<std::string> rightParamsSet;
                std::map<std::string, std::string> leftParamsMap;
                std::map<std::string, std::string> rightParamsMap;
                for (const auto &kv : prevStep.paramValues) {
                    leftParamsSet.insert(kv.key);
                    leftParamsMap[kv.key] = kv.value;
                }
                for (const auto &kv : curStep.paramValues) {
                    rightParamsSet.insert(kv.key);
                    rightParamsMap[kv.key] = kv.value;
                }
                if (leftParamsSet == rightParamsSet) {
                    bool doErase = true;
                    try {
                        for (const auto &param : leftParamsSet) {
                            if (param == "convention" || param == "t_epoch" ||
                                param == "t_obs") {
                                if (leftParamsMap[param] !=
                                    rightParamsMap[param]) {
                                    doErase = false;
                                    break;
                                }
                            } else if (c_locale_stod(leftParamsMap[param]) !=
                                       -c_locale_stod(rightParamsMap[param])) {
                                doErase = false;
                                break;
                            }
                        }
                    } catch (const std::invalid_argument &) {
                        break;
                    }
                    if (doErase) {
                        ++iterCur;
                        d->steps_.erase(iterPrev, iterCur);
                        changeDone = true;
                        break;
                    }
                }
            }

            // detect a step and its inverse
            if (curStep.inverted != prevStep.inverted &&
                curStep.name == prevStep.name &&
                curStepParamCount == prevStepParamCount) {
                bool allSame = true;
                for (size_t j = 0; j < curStepParamCount; j++) {
                    if (curStep.paramValues[j] != prevStep.paramValues[j]) {
                        allSame = false;
                        break;
                    }
                }
                if (allSame) {
                    ++iterCur;
                    d->steps_.erase(iterPrev, iterCur);
                    changeDone = true;
                    break;
                }
            }
        }
    } while (changeDone);

    if (d->steps_.size() > 1 ||
        (d->steps_.size() == 1 &&
         (d->steps_.front().inverted || !d->globalParamValues_.empty()))) {
        d->appendToResult("+proj=pipeline");

        for (const auto &paramValue : d->globalParamValues_) {
            d->appendToResult("+");
            d->result_ += paramValue.key;
            if (!paramValue.value.empty()) {
                d->result_ += '=';
                d->result_ +=
                    pj_double_quote_string_param_if_needed(paramValue.value);
            }
        }
    }

    for (const auto &step : d->steps_) {
        if (!d->result_.empty()) {
            d->appendToResult("+step");
        }
        if (step.inverted) {
            d->appendToResult("+inv");
        }
        if (!step.name.empty()) {
            d->appendToResult(step.isInit ? "+init=" : "+proj=");
            d->result_ += step.name;
        }
        for (const auto &paramValue : step.paramValues) {
            d->appendToResult("+");
            d->result_ += paramValue.key;
            if (!paramValue.value.empty()) {
                d->result_ += '=';
                d->result_ +=
                    pj_double_quote_string_param_if_needed(paramValue.value);
            }
        }
    }

    if (d->result_.empty()) {
        d->appendToResult("+proj=noop");
    }

    return d->result_;
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress

PROJStringFormatter::Convention PROJStringFormatter::convention() const {
    return d->convention_;
}

// ---------------------------------------------------------------------------

bool PROJStringFormatter::getUseApproxTMerc() const {
    return d->useApproxTMerc_;
}

// ---------------------------------------------------------------------------

void PROJStringFormatter::setCoordinateOperationOptimizations(bool enable) {
    d->coordOperationOptimizations_ = enable;
}

// ---------------------------------------------------------------------------

void PROJStringFormatter::Private::appendToResult(const char *str) {
    if (!result_.empty()) {
        result_ += ' ';
    }
    result_ += str;
}

// ---------------------------------------------------------------------------

static void
PROJStringSyntaxParser(const std::string &projString, std::vector<Step> &steps,
                       std::vector<Step::KeyValue> &globalParamValues,
                       std::string &title, bool dropEarlyBindingsTerms) {
    const char *c_str = projString.c_str();
    std::vector<std::string> tokens;

    bool hasProj = false;
    bool hasInit = false;
    bool hasPipeline = false;
    {
        size_t i = 0;
        while (true) {
            for (; isspace(static_cast<unsigned char>(c_str[i])); i++) {
            }
            std::string token;
            bool in_string = false;
            for (; c_str[i]; i++) {
                if (in_string) {
                    if (c_str[i] == '"' && c_str[i + 1] == '"') {
                        i++;
                    } else if (c_str[i] == '"') {
                        in_string = false;
                        continue;
                    }
                } else if (c_str[i] == '=' && c_str[i + 1] == '"') {
                    in_string = true;
                    token += c_str[i];
                    i++;
                    continue;
                } else if (isspace(static_cast<unsigned char>(c_str[i]))) {
                    break;
                }
                token += c_str[i];
            }
            if (in_string) {
                throw ParsingException("Unbalanced double quote");
            }
            if (token.empty()) {
                break;
            }
            if (!hasPipeline &&
                (token == "proj=pipeline" || token == "+proj=pipeline")) {
                hasPipeline = true;
            } else if (!hasProj && (starts_with(token, "proj=") ||
                                    starts_with(token, "+proj="))) {
                hasProj = true;
            } else if (!hasInit && (starts_with(token, "init=") ||
                                    starts_with(token, "+init="))) {
                hasInit = true;
            }
            tokens.emplace_back(token);
        }
    }

    bool prevWasTitle = false;

    if (!hasPipeline) {
        if (hasProj || hasInit) {
            steps.push_back(Step());
        }

        for (auto &word : tokens) {
            if (word[0] == '+') {
                word = word.substr(1);
            } else if (prevWasTitle && word.find('=') == std::string::npos) {
                title += " ";
                title += word;
                continue;
            }

            prevWasTitle = false;
            if (starts_with(word, "proj=") && !hasInit) {
                assert(hasProj);
                auto stepName = word.substr(strlen("proj="));
                steps.back().name = stepName;
            } else if (starts_with(word, "init=")) {
                assert(hasInit);
                auto initName = word.substr(strlen("init="));
                steps.back().name = initName;
                steps.back().isInit = true;
            } else if (word == "inv") {
                if (!steps.empty()) {
                    steps.back().inverted = true;
                }
            } else if (starts_with(word, "title=")) {
                title = word.substr(strlen("title="));
                prevWasTitle = true;
            } else if (word != "step") {
                const auto pos = word.find('=');
                auto key = word.substr(0, pos);
                if (!(dropEarlyBindingsTerms &&
                      (key == "towgs84" || key == "nadgrids" ||
                       key == "geoidgrids"))) {
                    auto pair = (pos != std::string::npos)
                                    ? Step::KeyValue(key, word.substr(pos + 1))
                                    : Step::KeyValue(key);
                    if (dropEarlyBindingsTerms && key == "datum") {
                        const auto datums = pj_get_datums_ref();
                        for (int i = 0; datums[i].id != nullptr; i++) {
                            if (pair.value == datums[i].id) {
                                pair.key = "ellps";
                                pair.value = datums[i].ellipse_id;
                                break;
                            }
                        }
                    }
                    if (steps.empty()) {
                        globalParamValues.push_back(pair);
                    } else {
                        steps.back().paramValues.push_back(pair);
                    }
                }
            }
        }
        return;
    }

    bool inPipeline = false;
    bool invGlobal = false;
    for (auto &word : tokens) {
        if (word[0] == '+') {
            word = word.substr(1);
        } else if (prevWasTitle && word.find('=') == std::string::npos) {
            title += " ";
            title += word;
            continue;
        }

        prevWasTitle = false;
        if (word == "proj=pipeline") {
            if (inPipeline) {
                throw ParsingException("nested pipeline not supported");
            }
            inPipeline = true;
        } else if (word == "step") {
            if (!inPipeline) {
                throw ParsingException("+step found outside pipeline");
            }
            steps.push_back(Step());
        } else if (word == "inv") {
            if (steps.empty()) {
                invGlobal = true;
            } else {
                steps.back().inverted = true;
            }
        } else if (inPipeline && !steps.empty() && starts_with(word, "proj=") &&
                   steps.back().name.empty()) {
            auto stepName = word.substr(strlen("proj="));
            steps.back().name = stepName;
        } else if (inPipeline && !steps.empty() && starts_with(word, "init=") &&
                   steps.back().name.empty()) {
            auto initName = word.substr(strlen("init="));
            steps.back().name = initName;
            steps.back().isInit = true;
        } else if (!inPipeline && starts_with(word, "title=")) {
            title = word.substr(strlen("title="));
            prevWasTitle = true;
        } else {
            const auto pos = word.find('=');
            auto key = word.substr(0, pos);
            auto pair = (pos != std::string::npos)
                            ? Step::KeyValue(key, word.substr(pos + 1))
                            : Step::KeyValue(key);
            if (steps.empty()) {
                globalParamValues.push_back(pair);
            } else {
                steps.back().paramValues.push_back(pair);
            }
        }
    }
    if (invGlobal) {
        for (auto &step : steps) {
            step.inverted = !step.inverted;
        }
        std::reverse(steps.begin(), steps.end());
    }
}

// ---------------------------------------------------------------------------

void PROJStringFormatter::ingestPROJString(
    const std::string &str) // throw ParsingException
{
    std::vector<Step> steps;
    std::string title;
    PROJStringSyntaxParser(str, steps, d->globalParamValues_, title,
                           d->dropEarlyBindingsTerms_);
    d->steps_.insert(d->steps_.end(), steps.begin(), steps.end());
}

// ---------------------------------------------------------------------------

void PROJStringFormatter::setCRSExport(bool b) { d->crsExport_ = b; }

// ---------------------------------------------------------------------------

bool PROJStringFormatter::getCRSExport() const { return d->crsExport_; }

// ---------------------------------------------------------------------------

void PROJStringFormatter::startInversion() {
    PROJStringFormatter::Private::InversionStackElt elt;
    elt.startIter = d->steps_.end();
    if (elt.startIter != d->steps_.begin()) {
        elt.iterValid = true;
        --elt.startIter; // point to the last valid element
    } else {
        elt.iterValid = false;
    }
    elt.currentInversionState =
        !d->inversionStack_.back().currentInversionState;
    d->inversionStack_.push_back(elt);
}

// ---------------------------------------------------------------------------

void PROJStringFormatter::stopInversion() {
    assert(!d->inversionStack_.empty());
    auto startIter = d->inversionStack_.back().startIter;
    if (!d->inversionStack_.back().iterValid) {
        startIter = d->steps_.begin();
    } else {
        ++startIter; // advance after the last valid element we marked above
    }
    // Invert the inversion status of the steps between the start point and
    // the current end of steps
    for (auto iter = startIter; iter != d->steps_.end(); ++iter) {
        iter->inverted = !iter->inverted;
    }
    // And reverse the order of steps in that range as well.
    std::reverse(startIter, d->steps_.end());
    d->inversionStack_.pop_back();
}

// ---------------------------------------------------------------------------

bool PROJStringFormatter::isInverted() const {
    return d->inversionStack_.back().currentInversionState;
}

// ---------------------------------------------------------------------------

void PROJStringFormatter::Private::addStep() { steps_.emplace_back(Step()); }

// ---------------------------------------------------------------------------

void PROJStringFormatter::addStep(const char *stepName) {
    d->addStep();
    d->steps_.back().name.assign(stepName);
}

// ---------------------------------------------------------------------------

void PROJStringFormatter::addStep(const std::string &stepName) {
    d->addStep();
    d->steps_.back().name = stepName;
}

// ---------------------------------------------------------------------------

void PROJStringFormatter::setCurrentStepInverted(bool inverted) {
    assert(!d->steps_.empty());
    d->steps_.back().inverted = inverted;
}

// ---------------------------------------------------------------------------

bool PROJStringFormatter::hasParam(const char *paramName) const {
    if (!d->steps_.empty()) {
        for (const auto &paramValue : d->steps_.back().paramValues) {
            if (paramValue.keyEquals(paramName)) {
                return true;
            }
        }
    }
    return false;
}

// ---------------------------------------------------------------------------

void PROJStringFormatter::addNoDefs(bool b) { d->addNoDefs_ = b; }

// ---------------------------------------------------------------------------

bool PROJStringFormatter::getAddNoDefs() const { return d->addNoDefs_; }

// ---------------------------------------------------------------------------

void PROJStringFormatter::addParam(const std::string &paramName) {
    if (d->steps_.empty()) {
        d->addStep();
    }
    d->steps_.back().paramValues.push_back(Step::KeyValue(paramName));
}

// ---------------------------------------------------------------------------

void PROJStringFormatter::addParam(const char *paramName, int val) {
    addParam(std::string(paramName), val);
}

void PROJStringFormatter::addParam(const std::string &paramName, int val) {
    addParam(paramName, internal::toString(val));
}

// ---------------------------------------------------------------------------

static std::string formatToString(double val) {
    if (std::abs(val * 10 - std::round(val * 10)) < 1e-8) {
        // For the purpose of
        // https://www.epsg-registry.org/export.htm?wkt=urn:ogc:def:crs:EPSG::27561
        // Latitude of natural of origin to be properly rounded from 55 grad
        // to
        // 49.5 deg
        val = std::round(val * 10) / 10;
    }
    return normalizeSerializedString(internal::toString(val));
}

// ---------------------------------------------------------------------------

void PROJStringFormatter::addParam(const char *paramName, double val) {
    addParam(std::string(paramName), val);
}

void PROJStringFormatter::addParam(const std::string &paramName, double val) {
    addParam(paramName, formatToString(val));
}

// ---------------------------------------------------------------------------

void PROJStringFormatter::addParam(const char *paramName,
                                   const std::vector<double> &vals) {
    std::string paramValue;
    for (size_t i = 0; i < vals.size(); ++i) {
        if (i > 0) {
            paramValue += ',';
        }
        paramValue += formatToString(vals[i]);
    }
    addParam(paramName, paramValue);
}

// ---------------------------------------------------------------------------

void PROJStringFormatter::addParam(const char *paramName, const char *val) {
    addParam(std::string(paramName), val);
}

void PROJStringFormatter::addParam(const char *paramName,
                                   const std::string &val) {
    addParam(std::string(paramName), val);
}

void PROJStringFormatter::addParam(const std::string &paramName,
                                   const char *val) {
    addParam(paramName, std::string(val));
}

// ---------------------------------------------------------------------------

void PROJStringFormatter::addParam(const std::string &paramName,
                                   const std::string &val) {
    if (d->steps_.empty()) {
        d->addStep();
    }
    d->steps_.back().paramValues.push_back(Step::KeyValue(paramName, val));
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

std::set<std::string> PROJStringFormatter::getUsedGridNames() const {
    std::set<std::string> res;
    for (const auto &step : d->steps_) {
        for (const auto &param : step.paramValues) {
            if (param.keyEquals("grids")) {
                res.insert(param.value);
            }
        }
    }
    return res;
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
    assert(d->omitProjLongLatIfPossible_ ^ omit);
    d->omitProjLongLatIfPossible_ = omit;
}

// ---------------------------------------------------------------------------

bool PROJStringFormatter::omitProjLongLatIfPossible() const {
    return d->omitProjLongLatIfPossible_;
}

// ---------------------------------------------------------------------------

void PROJStringFormatter::pushOmitZUnitConversion() {
    d->omitZUnitConversion_.push_back(true);
}

// ---------------------------------------------------------------------------

void PROJStringFormatter::popOmitZUnitConversion() {
    assert(d->omitZUnitConversion_.size() > 1);
    d->omitZUnitConversion_.pop_back();
}

// ---------------------------------------------------------------------------

bool PROJStringFormatter::omitZUnitConversion() const {
    return d->omitZUnitConversion_.back();
}

// ---------------------------------------------------------------------------

void PROJStringFormatter::setDropEarlyBindingsTerms(bool drop) {
    d->dropEarlyBindingsTerms_ = drop;
}

// ---------------------------------------------------------------------------

bool PROJStringFormatter::getDropEarlyBindingsTerms() const {
    return d->dropEarlyBindingsTerms_;
}

// ---------------------------------------------------------------------------

const DatabaseContextPtr &PROJStringFormatter::databaseContext() const {
    return d->dbContext_;
}

//! @endcond

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress

struct PROJStringParser::Private {
    DatabaseContextPtr dbContext_{};
    PJ_CONTEXT *ctx_{};
    bool usePROJ4InitRules_ = false;
    std::vector<std::string> warningList_{};

    std::string projString_{};

    std::vector<Step> steps_{};
    std::vector<Step::KeyValue> globalParamValues_{};
    std::string title_{};

    bool ignoreNadgrids_ = false;

    template <class T>
    // cppcheck-suppress functionStatic
    bool hasParamValue(Step &step, const T key) {
        for (auto &pair : globalParamValues_) {
            if (ci_equal(pair.key, key)) {
                pair.usedByParser = true;
                return true;
            }
        }
        for (auto &pair : step.paramValues) {
            if (ci_equal(pair.key, key)) {
                pair.usedByParser = true;
                return true;
            }
        }
        return false;
    }

    template <class T>
    // cppcheck-suppress functionStatic
    const std::string &getGlobalParamValue(T key) {
        for (auto &pair : globalParamValues_) {
            if (ci_equal(pair.key, key)) {
                pair.usedByParser = true;
                return pair.value;
            }
        }
        return emptyString;
    }

    template <class T>
    // cppcheck-suppress functionStatic
    const std::string &getParamValue(Step &step, const T key) {
        for (auto &pair : globalParamValues_) {
            if (ci_equal(pair.key, key)) {
                pair.usedByParser = true;
                return pair.value;
            }
        }
        for (auto &pair : step.paramValues) {
            if (ci_equal(pair.key, key)) {
                pair.usedByParser = true;
                return pair.value;
            }
        }
        return emptyString;
    }

    static const std::string &getParamValueK(Step &step) {
        for (auto &pair : step.paramValues) {
            if (ci_equal(pair.key, "k") || ci_equal(pair.key, "k_0")) {
                pair.usedByParser = true;
                return pair.value;
            }
        }
        return emptyString;
    }

    // cppcheck-suppress functionStatic
    bool hasUnusedParameters(const Step &step) const {
        if (steps_.size() == 1) {
            for (const auto &pair : step.paramValues) {
                if (pair.key != "no_defs" && !pair.usedByParser) {
                    return true;
                }
            }
        }
        return false;
    }

    // cppcheck-suppress functionStatic
    std::string guessBodyName(double a);

    PrimeMeridianNNPtr buildPrimeMeridian(Step &step);
    GeodeticReferenceFrameNNPtr buildDatum(Step &step,
                                           const std::string &title);
    GeographicCRSNNPtr buildGeographicCRS(int iStep, int iUnitConvert,
                                          int iAxisSwap, bool ignoreVUnits,
                                          bool ignorePROJAxis);
    GeodeticCRSNNPtr buildGeocentricCRS(int iStep, int iUnitConvert);
    CRSNNPtr buildProjectedCRS(int iStep, GeographicCRSNNPtr geogCRS,
                               int iUnitConvert, int iAxisSwap);
    CRSNNPtr buildBoundOrCompoundCRSIfNeeded(int iStep, CRSNNPtr crs);
    UnitOfMeasure buildUnit(Step &step, const std::string &unitsParamName,
                            const std::string &toMeterParamName);

    enum class AxisType { REGULAR, NORTH_POLE, SOUTH_POLE };

    std::vector<CoordinateSystemAxisNNPtr>
    processAxisSwap(Step &step, const UnitOfMeasure &unit, int iAxisSwap,
                    AxisType axisType, bool ignorePROJAxis);

    EllipsoidalCSNNPtr buildEllipsoidalCS(int iStep, int iUnitConvert,
                                          int iAxisSwap, bool ignoreVUnits,
                                          bool ignorePROJAxis);
};

// ---------------------------------------------------------------------------

PROJStringParser::PROJStringParser() : d(internal::make_unique<Private>()) {}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
PROJStringParser::~PROJStringParser() = default;
//! @endcond

// ---------------------------------------------------------------------------

/** \brief Attach a database context, to allow queries in it if needed.
 */
PROJStringParser &
PROJStringParser::attachDatabaseContext(const DatabaseContextPtr &dbContext) {
    d->dbContext_ = dbContext;
    return *this;
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
PROJStringParser &PROJStringParser::attachContext(PJ_CONTEXT *ctx) {
    d->ctx_ = ctx;
    return *this;
}
//! @endcond

// ---------------------------------------------------------------------------

/** \brief Set how init=epsg:XXXX syntax should be interpreted.
 *
 * @param enable When set to true,
 * init=epsg:XXXX syntax will be allowed and will be interpreted according to
 * PROJ.4 and PROJ.5 rules, that is geodeticCRS will have longitude, latitude
 * order and will expect/output coordinates in radians. ProjectedCRS will have
 * easting, northing axis order (except the ones with Transverse Mercator South
 * Orientated projection).
 */
PROJStringParser &PROJStringParser::setUsePROJ4InitRules(bool enable) {
    d->usePROJ4InitRules_ = enable;
    return *this;
}

// ---------------------------------------------------------------------------

/** \brief Return the list of warnings found during parsing.
 */
std::vector<std::string> PROJStringParser::warningList() const {
    return d->warningList_;
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress

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

static UnitOfMeasure _buildUnit(const LinearUnitDesc *unitsMatch) {
    std::string unitsCode;
    if (unitsMatch->epsgCode) {
        std::ostringstream buffer;
        buffer.imbue(std::locale::classic());
        buffer << unitsMatch->epsgCode;
        unitsCode = buffer.str();
    }
    return UnitOfMeasure(
        unitsMatch->name, c_locale_stod(unitsMatch->convToMeter),
        UnitOfMeasure::Type::LINEAR,
        unitsMatch->epsgCode ? Identifier::EPSG : std::string(), unitsCode);
}

// ---------------------------------------------------------------------------

static UnitOfMeasure _buildUnit(double to_meter_value) {
    // TODO: look-up in EPSG catalog
    if (to_meter_value == 0) {
        throw ParsingException("invalid unit value");
    }
    return UnitOfMeasure("unknown", to_meter_value,
                         UnitOfMeasure::Type::LINEAR);
}

// ---------------------------------------------------------------------------

UnitOfMeasure
PROJStringParser::Private::buildUnit(Step &step,
                                     const std::string &unitsParamName,
                                     const std::string &toMeterParamName) {
    UnitOfMeasure unit = UnitOfMeasure::METRE;
    const LinearUnitDesc *unitsMatch = nullptr;
    const auto &projUnits = getParamValue(step, unitsParamName);
    if (!projUnits.empty()) {
        unitsMatch = getLinearUnits(projUnits);
        if (unitsMatch == nullptr) {
            throw ParsingException("unhandled " + unitsParamName + "=" +
                                   projUnits);
        }
    }

    const auto &toMeter = getParamValue(step, toMeterParamName);
    if (!toMeter.empty()) {
        double to_meter_value;
        try {
            to_meter_value = c_locale_stod(toMeter);
        } catch (const std::invalid_argument &) {
            throw ParsingException("invalid value for " + toMeterParamName);
        }
        unitsMatch = getLinearUnits(to_meter_value);
        if (unitsMatch == nullptr) {
            unit = _buildUnit(to_meter_value);
        }
    }

    if (unitsMatch) {
        unit = _buildUnit(unitsMatch);
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

static bool isGeographicStep(const std::string &name) {
    return name == "longlat" || name == "lonlat" || name == "latlong" ||
           name == "latlon";
}

// ---------------------------------------------------------------------------

static bool isGeocentricStep(const std::string &name) {
    return name == "geocent" || name == "cart";
}

// ---------------------------------------------------------------------------

static bool isProjectedStep(const std::string &name) {
    if (name == "etmerc" || name == "utm" ||
        !getMappingsFromPROJName(name).empty()) {
        return true;
    }
    // IMPROVE ME: have a better way of distinguishing projections from
    // other
    // transformations.
    if (name == "pipeline" || name == "geoc" || name == "deformation" ||
        name == "helmert" || name == "hgridshift" || name == "molodensky" ||
        name == "vgridshit") {
        return false;
    }
    const auto *operations = proj_list_operations();
    for (int i = 0; operations[i].id != nullptr; ++i) {
        if (name == operations[i].id) {
            return true;
        }
    }
    return false;
}

// ---------------------------------------------------------------------------

static PropertyMap createMapWithUnknownName() {
    return PropertyMap().set(common::IdentifiedObject::NAME_KEY, "unknown");
}

// ---------------------------------------------------------------------------

PrimeMeridianNNPtr PROJStringParser::Private::buildPrimeMeridian(Step &step) {

    PrimeMeridianNNPtr pm = PrimeMeridian::GREENWICH;
    const auto &pmStr = getParamValue(step, "pm");
    if (!pmStr.empty()) {
        char *end;
        double pmValue = dmstor(pmStr.c_str(), &end) * RAD_TO_DEG;
        if (pmValue != HUGE_VAL && *end == '\0') {
            pm = PrimeMeridian::create(createMapWithUnknownName(),
                                       Angle(pmValue));
        } else {
            bool found = false;
            if (pmStr == "paris") {
                found = true;
                pm = PrimeMeridian::PARIS;
            }
            auto proj_prime_meridians = proj_list_prime_meridians();
            for (int i = 0; !found && proj_prime_meridians[i].id != nullptr;
                 i++) {
                if (pmStr == proj_prime_meridians[i].id) {
                    found = true;
                    std::string name = static_cast<char>(::toupper(pmStr[0])) +
                                       pmStr.substr(1);
                    pmValue = dmstor(proj_prime_meridians[i].defn, nullptr) *
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
    return pm;
}

// ---------------------------------------------------------------------------

std::string PROJStringParser::Private::guessBodyName(double a) {
    return Ellipsoid::guessBodyName(dbContext_, a);
}

// ---------------------------------------------------------------------------

GeodeticReferenceFrameNNPtr
PROJStringParser::Private::buildDatum(Step &step, const std::string &title) {

    const auto &ellpsStr = getParamValue(step, "ellps");
    const auto &datumStr = getParamValue(step, "datum");
    const auto &RStr = getParamValue(step, "R");
    const auto &aStr = getParamValue(step, "a");
    const auto &bStr = getParamValue(step, "b");
    const auto &rfStr = getParamValue(step, "rf");
    const auto &fStr = getParamValue(step, "f");
    const auto &esStr = getParamValue(step, "es");
    const auto &eStr = getParamValue(step, "e");
    double a = -1.0;
    double b = -1.0;
    double rf = -1.0;
    const util::optional<std::string> optionalEmptyString{};
    const bool numericParamPresent =
        !RStr.empty() || !aStr.empty() || !bStr.empty() || !rfStr.empty() ||
        !fStr.empty() || !esStr.empty() || !eStr.empty();

    PrimeMeridianNNPtr pm(buildPrimeMeridian(step));
    PropertyMap grfMap;

    // It is arguable that we allow the prime meridian of a datum defined by
    // its name to be overridden, but this is found at least in a regression
    // test
    // of GDAL. So let's keep the ellipsoid part of the datum in that case and
    // use the specified prime meridian.
    const auto overridePmIfNeeded =
        [&pm](const GeodeticReferenceFrameNNPtr &grf) {
            if (pm->_isEquivalentTo(PrimeMeridian::GREENWICH.get())) {
                return grf;
            } else {
                return GeodeticReferenceFrame::create(
                    PropertyMap().set(IdentifiedObject::NAME_KEY,
                                      "Unknown based on " +
                                          grf->ellipsoid()->nameStr() +
                                          " ellipsoid"),
                    grf->ellipsoid(), grf->anchorDefinition(), pm);
            }
        };

    // R take precedence
    if (!RStr.empty()) {
        double R;
        try {
            R = c_locale_stod(RStr);
        } catch (const std::invalid_argument &) {
            throw ParsingException("Invalid R value");
        }
        auto ellipsoid = Ellipsoid::createSphere(createMapWithUnknownName(),
                                                 Length(R), guessBodyName(R));
        return GeodeticReferenceFrame::create(
            grfMap.set(IdentifiedObject::NAME_KEY,
                       title.empty() ? "unknown" : title.c_str()),
            ellipsoid, optionalEmptyString, fixupPrimeMeridan(ellipsoid, pm));
    }

    if (!datumStr.empty()) {
        auto l_datum = [&datumStr, &overridePmIfNeeded, &grfMap,
                        &optionalEmptyString, &pm]() {
            if (datumStr == "WGS84") {
                return overridePmIfNeeded(GeodeticReferenceFrame::EPSG_6326);
            } else if (datumStr == "NAD83") {
                return overridePmIfNeeded(GeodeticReferenceFrame::EPSG_6269);
            } else if (datumStr == "NAD27") {
                return overridePmIfNeeded(GeodeticReferenceFrame::EPSG_6267);
            } else {

                for (const auto &datumDesc : datumDescs) {
                    if (datumStr == datumDesc.projName) {
                        (void)datumDesc.gcsName; // to please cppcheck
                        (void)datumDesc.gcsCode; // to please cppcheck
                        auto ellipsoid = Ellipsoid::createFlattenedSphere(
                            grfMap
                                .set(IdentifiedObject::NAME_KEY,
                                     datumDesc.ellipsoidName)
                                .set(Identifier::CODESPACE_KEY,
                                     Identifier::EPSG)
                                .set(Identifier::CODE_KEY,
                                     datumDesc.ellipsoidCode),
                            Length(datumDesc.a), Scale(datumDesc.rf));
                        return GeodeticReferenceFrame::create(
                            grfMap
                                .set(IdentifiedObject::NAME_KEY,
                                     datumDesc.datumName)
                                .set(Identifier::CODESPACE_KEY,
                                     Identifier::EPSG)
                                .set(Identifier::CODE_KEY, datumDesc.datumCode),
                            ellipsoid, optionalEmptyString, pm);
                    }
                }
            }
            throw ParsingException("unknown datum " + datumStr);
        }();
        if (!numericParamPresent) {
            return l_datum;
        }
        a = l_datum->ellipsoid()->semiMajorAxis().getSIValue();
        rf = l_datum->ellipsoid()->computedInverseFlattening();
    }

    else if (!ellpsStr.empty()) {
        auto l_datum = [&ellpsStr, &title, &grfMap, &optionalEmptyString,
                        &pm]() {
            if (ellpsStr == "WGS84") {
                return GeodeticReferenceFrame::create(
                    grfMap.set(IdentifiedObject::NAME_KEY,
                               title.empty()
                                   ? "Unknown based on WGS84 ellipsoid"
                                   : title.c_str()),
                    Ellipsoid::WGS84, optionalEmptyString, pm);
            } else if (ellpsStr == "GRS80") {
                return GeodeticReferenceFrame::create(
                    grfMap.set(IdentifiedObject::NAME_KEY,
                               title.empty()
                                   ? "Unknown based on GRS80 ellipsoid"
                                   : title.c_str()),
                    Ellipsoid::GRS1980, optionalEmptyString, pm);
            } else {
                auto proj_ellps = proj_list_ellps();
                for (int i = 0; proj_ellps[i].id != nullptr; i++) {
                    if (ellpsStr == proj_ellps[i].id) {
                        assert(strncmp(proj_ellps[i].major, "a=", 2) == 0);
                        const double a_iter =
                            c_locale_stod(proj_ellps[i].major + 2);
                        EllipsoidPtr ellipsoid;
                        PropertyMap ellpsMap;
                        if (strncmp(proj_ellps[i].ell, "b=", 2) == 0) {
                            const double b_iter =
                                c_locale_stod(proj_ellps[i].ell + 2);
                            ellipsoid =
                                Ellipsoid::createTwoAxis(
                                    ellpsMap.set(IdentifiedObject::NAME_KEY,
                                                 proj_ellps[i].name),
                                    Length(a_iter), Length(b_iter))
                                    .as_nullable();
                        } else {
                            assert(strncmp(proj_ellps[i].ell, "rf=", 3) == 0);
                            const double rf_iter =
                                c_locale_stod(proj_ellps[i].ell + 3);
                            ellipsoid =
                                Ellipsoid::createFlattenedSphere(
                                    ellpsMap.set(IdentifiedObject::NAME_KEY,
                                                 proj_ellps[i].name),
                                    Length(a_iter), Scale(rf_iter))
                                    .as_nullable();
                        }
                        return GeodeticReferenceFrame::create(
                            grfMap.set(IdentifiedObject::NAME_KEY,
                                       title.empty()
                                           ? std::string("Unknown based on ") +
                                                 proj_ellps[i].name +
                                                 " ellipsoid"
                                           : title),
                            NN_NO_CHECK(ellipsoid), optionalEmptyString, pm);
                    }
                }
                throw ParsingException("unknown ellipsoid " + ellpsStr);
            }
        }();
        if (!numericParamPresent) {
            return l_datum;
        }
        a = l_datum->ellipsoid()->semiMajorAxis().getSIValue();
        if (l_datum->ellipsoid()->semiMinorAxis().has_value()) {
            b = l_datum->ellipsoid()->semiMinorAxis()->getSIValue();
        } else {
            rf = l_datum->ellipsoid()->computedInverseFlattening();
        }
    }

    if (!aStr.empty()) {
        try {
            a = c_locale_stod(aStr);
        } catch (const std::invalid_argument &) {
            throw ParsingException("Invalid a value");
        }
    }

    if (a > 0 && (b > 0 || !bStr.empty())) {
        if (!bStr.empty()) {
            try {
                b = c_locale_stod(bStr);
            } catch (const std::invalid_argument &) {
                throw ParsingException("Invalid b value");
            }
        }
        auto ellipsoid =
            Ellipsoid::createTwoAxis(createMapWithUnknownName(), Length(a),
                                     Length(b), guessBodyName(a))
                ->identify();
        return GeodeticReferenceFrame::create(
            grfMap.set(IdentifiedObject::NAME_KEY,
                       title.empty() ? "unknown" : title.c_str()),
            ellipsoid, optionalEmptyString, fixupPrimeMeridan(ellipsoid, pm));
    }

    else if (a > 0 && (rf >= 0 || !rfStr.empty())) {
        if (!rfStr.empty()) {
            try {
                rf = c_locale_stod(rfStr);
            } catch (const std::invalid_argument &) {
                throw ParsingException("Invalid rf value");
            }
        }
        auto ellipsoid = Ellipsoid::createFlattenedSphere(
                             createMapWithUnknownName(), Length(a), Scale(rf),
                             guessBodyName(a))
                             ->identify();
        return GeodeticReferenceFrame::create(
            grfMap.set(IdentifiedObject::NAME_KEY,
                       title.empty() ? "unknown" : title.c_str()),
            ellipsoid, optionalEmptyString, fixupPrimeMeridan(ellipsoid, pm));
    }

    else if (a > 0 && !fStr.empty()) {
        double f;
        try {
            f = c_locale_stod(fStr);
        } catch (const std::invalid_argument &) {
            throw ParsingException("Invalid f value");
        }
        auto ellipsoid = Ellipsoid::createFlattenedSphere(
                             createMapWithUnknownName(), Length(a),
                             Scale(f != 0.0 ? 1.0 / f : 0.0), guessBodyName(a))
                             ->identify();
        return GeodeticReferenceFrame::create(
            grfMap.set(IdentifiedObject::NAME_KEY,
                       title.empty() ? "unknown" : title.c_str()),
            ellipsoid, optionalEmptyString, fixupPrimeMeridan(ellipsoid, pm));
    }

    else if (a > 0 && !eStr.empty()) {
        double e;
        try {
            e = c_locale_stod(eStr);
        } catch (const std::invalid_argument &) {
            throw ParsingException("Invalid e value");
        }
        double alpha = asin(e);    /* angular eccentricity */
        double f = 1 - cos(alpha); /* = 1 - sqrt (1 - es); */
        auto ellipsoid = Ellipsoid::createFlattenedSphere(
                             createMapWithUnknownName(), Length(a),
                             Scale(f != 0.0 ? 1.0 / f : 0.0), guessBodyName(a))
                             ->identify();
        return GeodeticReferenceFrame::create(
            grfMap.set(IdentifiedObject::NAME_KEY,
                       title.empty() ? "unknown" : title.c_str()),
            ellipsoid, optionalEmptyString, fixupPrimeMeridan(ellipsoid, pm));
    }

    else if (a > 0 && !esStr.empty()) {
        double es;
        try {
            es = c_locale_stod(esStr);
        } catch (const std::invalid_argument &) {
            throw ParsingException("Invalid es value");
        }
        double f = 1 - sqrt(1 - es);
        auto ellipsoid = Ellipsoid::createFlattenedSphere(
                             createMapWithUnknownName(), Length(a),
                             Scale(f != 0.0 ? 1.0 / f : 0.0), guessBodyName(a))
                             ->identify();
        return GeodeticReferenceFrame::create(
            grfMap.set(IdentifiedObject::NAME_KEY,
                       title.empty() ? "unknown" : title.c_str()),
            ellipsoid, optionalEmptyString, fixupPrimeMeridan(ellipsoid, pm));
    }

    // If only a is specified, create a sphere
    if (a > 0 && bStr.empty() && rfStr.empty() && eStr.empty() &&
        esStr.empty()) {
        auto ellipsoid = Ellipsoid::createSphere(createMapWithUnknownName(),
                                                 Length(a), guessBodyName(a));
        return GeodeticReferenceFrame::create(
            grfMap.set(IdentifiedObject::NAME_KEY,
                       title.empty() ? "unknown" : title.c_str()),
            ellipsoid, optionalEmptyString, fixupPrimeMeridan(ellipsoid, pm));
    }

    if (!bStr.empty() && aStr.empty()) {
        throw ParsingException("b found, but a missing");
    }

    if (!rfStr.empty() && aStr.empty()) {
        throw ParsingException("rf found, but a missing");
    }

    if (!fStr.empty() && aStr.empty()) {
        throw ParsingException("f found, but a missing");
    }

    if (!eStr.empty() && aStr.empty()) {
        throw ParsingException("e found, but a missing");
    }

    if (!esStr.empty() && aStr.empty()) {
        throw ParsingException("es found, but a missing");
    }

    return overridePmIfNeeded(GeodeticReferenceFrame::EPSG_6326);
}

// ---------------------------------------------------------------------------

static const MeridianPtr nullMeridian{};

static CoordinateSystemAxisNNPtr
createAxis(const std::string &name, const std::string &abbreviation,
           const AxisDirection &direction, const common::UnitOfMeasure &unit,
           const MeridianPtr &meridian = nullMeridian) {
    return CoordinateSystemAxis::create(
        PropertyMap().set(IdentifiedObject::NAME_KEY, name), abbreviation,
        direction, unit, meridian);
}

std::vector<CoordinateSystemAxisNNPtr>
PROJStringParser::Private::processAxisSwap(Step &step,
                                           const UnitOfMeasure &unit,
                                           int iAxisSwap, AxisType axisType,
                                           bool ignorePROJAxis) {
    assert(iAxisSwap < 0 || ci_equal(steps_[iAxisSwap].name, "axisswap"));

    const bool isGeographic = unit.type() == UnitOfMeasure::Type::ANGULAR;
    const auto &eastName =
        isGeographic ? AxisName::Longitude : AxisName::Easting;
    const auto &eastAbbev =
        isGeographic ? AxisAbbreviation::lon : AxisAbbreviation::E;
    const auto &eastDir = isGeographic
                              ? AxisDirection::EAST
                              : (axisType == AxisType::NORTH_POLE)
                                    ? AxisDirection::SOUTH
                                    : (axisType == AxisType::SOUTH_POLE)
                                          ? AxisDirection::NORTH
                                          : AxisDirection::EAST;
    CoordinateSystemAxisNNPtr east = createAxis(
        eastName, eastAbbev, eastDir, unit,
        (!isGeographic &&
         (axisType == AxisType::NORTH_POLE || axisType == AxisType::SOUTH_POLE))
            ? Meridian::create(Angle(90, UnitOfMeasure::DEGREE)).as_nullable()
            : nullMeridian);

    const auto &northName =
        isGeographic ? AxisName::Latitude : AxisName::Northing;
    const auto &northAbbev =
        isGeographic ? AxisAbbreviation::lat : AxisAbbreviation::N;
    const auto &northDir = isGeographic
                               ? AxisDirection::NORTH
                               : (axisType == AxisType::NORTH_POLE)
                                     ? AxisDirection::SOUTH
                                     /*: (axisType == AxisType::SOUTH_POLE)
                                           ? AxisDirection::NORTH*/
                                     : AxisDirection::NORTH;
    CoordinateSystemAxisNNPtr north = createAxis(
        northName, northAbbev, northDir, unit,
        (!isGeographic && axisType == AxisType::NORTH_POLE)
            ? Meridian::create(Angle(180, UnitOfMeasure::DEGREE)).as_nullable()
            : (!isGeographic && axisType == AxisType::SOUTH_POLE)
                  ? Meridian::create(Angle(0, UnitOfMeasure::DEGREE))
                        .as_nullable()
                  : nullMeridian);

    CoordinateSystemAxisNNPtr west =
        createAxis(isGeographic ? AxisName::Longitude : AxisName::Westing,
                   isGeographic ? AxisAbbreviation::lon : std::string(),
                   AxisDirection::WEST, unit);

    CoordinateSystemAxisNNPtr south =
        createAxis(isGeographic ? AxisName::Latitude : AxisName::Southing,
                   isGeographic ? AxisAbbreviation::lat : std::string(),
                   AxisDirection::SOUTH, unit);

    std::vector<CoordinateSystemAxisNNPtr> axis{east, north};

    const auto &axisStr = getParamValue(step, "axis");
    if (!ignorePROJAxis && !axisStr.empty()) {
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
        } else {
            throw ParsingException("Unhandled axis=" + axisStr);
        }
    } else if (iAxisSwap >= 0) {
        auto &stepAxisSwap = steps_[iAxisSwap];
        const auto &orderStr = getParamValue(stepAxisSwap, "order");
        auto orderTab = split(orderStr, ',');
        if (orderTab.size() != 2) {
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
    return axis;
}

// ---------------------------------------------------------------------------

EllipsoidalCSNNPtr
PROJStringParser::Private::buildEllipsoidalCS(int iStep, int iUnitConvert,
                                              int iAxisSwap, bool ignoreVUnits,
                                              bool ignorePROJAxis) {
    auto &step = steps_[iStep];
    assert(iUnitConvert < 0 ||
           ci_equal(steps_[iUnitConvert].name, "unitconvert"));

    UnitOfMeasure angularUnit = UnitOfMeasure::DEGREE;
    if (iUnitConvert >= 0) {
        auto &stepUnitConvert = steps_[iUnitConvert];
        const std::string *xy_in = &getParamValue(stepUnitConvert, "xy_in");
        const std::string *xy_out = &getParamValue(stepUnitConvert, "xy_out");
        if (stepUnitConvert.inverted) {
            std::swap(xy_in, xy_out);
        }
        if (iUnitConvert < iStep) {
            std::swap(xy_in, xy_out);
        }
        if (xy_in->empty() || xy_out->empty() || *xy_in != "rad" ||
            (*xy_out != "rad" && *xy_out != "deg" && *xy_out != "grad")) {
            throw ParsingException("unhandled values for xy_in and/or xy_out");
        }
        if (*xy_out == "rad") {
            angularUnit = UnitOfMeasure::RADIAN;
        } else if (*xy_out == "grad") {
            angularUnit = UnitOfMeasure::GRAD;
        }
    }

    std::vector<CoordinateSystemAxisNNPtr> axis = processAxisSwap(
        step, angularUnit, iAxisSwap, AxisType::REGULAR, ignorePROJAxis);
    CoordinateSystemAxisNNPtr up = CoordinateSystemAxis::create(
        util::PropertyMap().set(IdentifiedObject::NAME_KEY,
                                AxisName::Ellipsoidal_height),
        AxisAbbreviation::h, AxisDirection::UP,
        buildUnit(step, "vunits", "vto_meter"));

    return (!ignoreVUnits && !hasParamValue(step, "geoidgrids") &&
            (hasParamValue(step, "vunits") || hasParamValue(step, "vto_meter")))
               ? EllipsoidalCS::create(emptyPropertyMap, axis[0], axis[1], up)
               : EllipsoidalCS::create(emptyPropertyMap, axis[0], axis[1]);
}

// ---------------------------------------------------------------------------

static double getNumericValue(const std::string &paramValue,
                              bool *pHasError = nullptr) {
    try {
        double value = c_locale_stod(paramValue);
        if (pHasError)
            *pHasError = false;
        return value;
    } catch (const std::invalid_argument &) {
        if (pHasError)
            *pHasError = true;
        return 0.0;
    }
}

// ---------------------------------------------------------------------------
namespace {
template <class T> inline void ignoreRetVal(T) {}
}

GeographicCRSNNPtr
PROJStringParser::Private::buildGeographicCRS(int iStep, int iUnitConvert,
                                              int iAxisSwap, bool ignoreVUnits,
                                              bool ignorePROJAxis) {
    auto &step = steps_[iStep];

    const bool l_isGeographicStep = isGeographicStep(step.name);
    const auto &title = l_isGeographicStep ? title_ : emptyString;

    // units=m is often found in the wild.
    // No need to create a extension string for this
    ignoreRetVal(hasParamValue(step, "units"));

    auto datum = buildDatum(step, title);

    auto props = PropertyMap().set(IdentifiedObject::NAME_KEY,
                                   title.empty() ? "unknown" : title);
    auto cs = buildEllipsoidalCS(iStep, iUnitConvert, iAxisSwap, ignoreVUnits,
                                 ignorePROJAxis);

    if (l_isGeographicStep &&
        (hasUnusedParameters(step) ||
         getNumericValue(getParamValue(step, "lon_0")) != 0.0)) {
        props.set("EXTENSION_PROJ4", projString_);
    }
    props.set("IMPLICIT_CS", true);

    return GeographicCRS::create(props, datum, cs);
}

// ---------------------------------------------------------------------------

GeodeticCRSNNPtr
PROJStringParser::Private::buildGeocentricCRS(int iStep, int iUnitConvert) {
    auto &step = steps_[iStep];

    assert(isGeocentricStep(step.name));
    assert(iUnitConvert < 0 ||
           ci_equal(steps_[iUnitConvert].name, "unitconvert"));

    const auto &title = title_;

    auto datum = buildDatum(step, title);

    UnitOfMeasure unit = buildUnit(step, "units", "");
    if (iUnitConvert >= 0) {
        auto &stepUnitConvert = steps_[iUnitConvert];
        const std::string *xy_in = &getParamValue(stepUnitConvert, "xy_in");
        const std::string *xy_out = &getParamValue(stepUnitConvert, "xy_out");
        const std::string *z_in = &getParamValue(stepUnitConvert, "z_in");
        const std::string *z_out = &getParamValue(stepUnitConvert, "z_out");
        if (stepUnitConvert.inverted) {
            std::swap(xy_in, xy_out);
            std::swap(z_in, z_out);
        }
        if (xy_in->empty() || xy_out->empty() || *xy_in != "m" ||
            *z_in != "m" || *xy_out != *z_out) {
            throw ParsingException(
                "unhandled values for xy_in, z_in, xy_out or z_out");
        }

        const LinearUnitDesc *unitsMatch = nullptr;
        try {
            double to_meter_value = c_locale_stod(*xy_out);
            unitsMatch = getLinearUnits(to_meter_value);
            if (unitsMatch == nullptr) {
                unit = _buildUnit(to_meter_value);
            }
        } catch (const std::invalid_argument &) {
            unitsMatch = getLinearUnits(*xy_out);
            if (!unitsMatch) {
                throw ParsingException(
                    "unhandled values for xy_in, z_in, xy_out or z_out");
            }
            unit = _buildUnit(unitsMatch);
        }
    }

    auto props = PropertyMap().set(IdentifiedObject::NAME_KEY,
                                   title.empty() ? "unknown" : title);
    auto cs = CartesianCS::createGeocentric(unit);

    if (hasUnusedParameters(step)) {
        props.set("EXTENSION_PROJ4", projString_);
    }

    return GeodeticCRS::create(props, datum, cs);
}

// ---------------------------------------------------------------------------

CRSNNPtr
PROJStringParser::Private::buildBoundOrCompoundCRSIfNeeded(int iStep,
                                                           CRSNNPtr crs) {
    auto &step = steps_[iStep];
    const auto &nadgrids = getParamValue(step, "nadgrids");
    const auto &towgs84 = getParamValue(step, "towgs84");
    // nadgrids has the priority over towgs84
    if (!ignoreNadgrids_ && !nadgrids.empty()) {
        crs = BoundCRS::createFromNadgrids(crs, nadgrids);
    } else if (!towgs84.empty()) {
        std::vector<double> towgs84Values;
        const auto tokens = split(towgs84, ',');
        for (const auto &str : tokens) {
            try {
                towgs84Values.push_back(c_locale_stod(str));
            } catch (const std::invalid_argument &) {
                throw ParsingException("Non numerical value in towgs84 clause");
            }
        }
        crs = BoundCRS::createFromTOWGS84(crs, towgs84Values);
    }

    const auto &geoidgrids = getParamValue(step, "geoidgrids");
    if (!geoidgrids.empty()) {
        auto vdatum =
            VerticalReferenceFrame::create(createMapWithUnknownName());

        const UnitOfMeasure unit = buildUnit(step, "vunits", "vto_meter");

        auto vcrs =
            VerticalCRS::create(createMapWithUnknownName(), vdatum,
                                VerticalCS::createGravityRelatedHeight(unit));

        auto transformation =
            Transformation::createGravityRelatedHeightToGeographic3D(
                PropertyMap().set(IdentifiedObject::NAME_KEY,
                                  "unknown to WGS84 ellipsoidal height"),
                crs, GeographicCRS::EPSG_4979, nullptr, geoidgrids,
                std::vector<PositionalAccuracyNNPtr>());
        auto boundvcrs =
            BoundCRS::create(vcrs, GeographicCRS::EPSG_4979, transformation);

        crs = CompoundCRS::create(createMapWithUnknownName(),
                                  std::vector<CRSNNPtr>{crs, boundvcrs});
    }

    return crs;
}

// ---------------------------------------------------------------------------

static double getAngularValue(const std::string &paramValue,
                              bool *pHasError = nullptr) {
    char *endptr = nullptr;
    double value = dmstor(paramValue.c_str(), &endptr) * RAD_TO_DEG;
    if (value == HUGE_VAL || endptr != paramValue.c_str() + paramValue.size()) {
        if (pHasError)
            *pHasError = true;
        return 0.0;
    }
    if (pHasError)
        *pHasError = false;
    return value;
}

// ---------------------------------------------------------------------------

CRSNNPtr PROJStringParser::Private::buildProjectedCRS(
    int iStep, GeographicCRSNNPtr geogCRS, int iUnitConvert, int iAxisSwap) {
    auto &step = steps_[iStep];
    auto mappings = getMappingsFromPROJName(step.name);
    const MethodMapping *mapping = mappings.empty() ? nullptr : mappings[0];

    assert(isProjectedStep(step.name));
    assert(iUnitConvert < 0 ||
           ci_equal(steps_[iUnitConvert].name, "unitconvert"));

    const auto &title = title_;

    if (!buildPrimeMeridian(step)->longitude()._isEquivalentTo(
            geogCRS->primeMeridian()->longitude(),
            util::IComparable::Criterion::EQUIVALENT)) {
        throw ParsingException("inconsistent pm values between projectedCRS "
                               "and its base geographicalCRS");
    }

    auto axisType = AxisType::REGULAR;

    if (step.name == "tmerc" &&
        ((getParamValue(step, "axis") == "wsu" && iAxisSwap < 0) ||
         (iAxisSwap > 0 &&
          getParamValue(steps_[iAxisSwap], "order") == "-1,-2"))) {
        mapping =
            getMapping(EPSG_CODE_METHOD_TRANSVERSE_MERCATOR_SOUTH_ORIENTATED);
    } else if (step.name == "etmerc") {
        mapping = getMapping(EPSG_CODE_METHOD_TRANSVERSE_MERCATOR);
    } else if (step.name == "lcc") {
        const auto &lat_0 = getParamValue(step, "lat_0");
        const auto &lat_1 = getParamValue(step, "lat_1");
        const auto &lat_2 = getParamValue(step, "lat_2");
        const auto &k = getParamValueK(step);
        if (lat_2.empty() && !lat_0.empty() && !lat_1.empty() &&
            (lat_0 == lat_1 ||
             // For some reason with gcc 5.3.1-14ubuntu2 32bit, the following
             // comparison returns false even if lat_0 == lat_1. Smells like
             // a compiler bug
             getAngularValue(lat_0) == getAngularValue(lat_1))) {
            mapping = getMapping(EPSG_CODE_METHOD_LAMBERT_CONIC_CONFORMAL_1SP);
        } else if (!k.empty() && getNumericValue(k) != 1.0) {
            mapping = getMapping(
                EPSG_CODE_METHOD_LAMBERT_CONIC_CONFORMAL_2SP_MICHIGAN);
        } else {
            mapping = getMapping(EPSG_CODE_METHOD_LAMBERT_CONIC_CONFORMAL_2SP);
        }
    } else if (step.name == "aeqd" && hasParamValue(step, "guam")) {
        mapping = getMapping(EPSG_CODE_METHOD_GUAM_PROJECTION);
    } else if (step.name == "cea" && !geogCRS->ellipsoid()->isSphere()) {
        mapping = getMapping(EPSG_CODE_METHOD_LAMBERT_CYLINDRICAL_EQUAL_AREA);
    } else if (step.name == "geos" && getParamValue(step, "sweep") == "x") {
        mapping =
            getMapping(PROJ_WKT2_NAME_METHOD_GEOSTATIONARY_SATELLITE_SWEEP_X);
    } else if (step.name == "geos") {
        mapping =
            getMapping(PROJ_WKT2_NAME_METHOD_GEOSTATIONARY_SATELLITE_SWEEP_Y);
    } else if (step.name == "omerc") {
        if (hasParamValue(step, "no_rot")) {
            mapping = nullptr;
        } else if (hasParamValue(step, "no_uoff") ||
                   hasParamValue(step, "no_off")) {
            mapping =
                getMapping(EPSG_CODE_METHOD_HOTINE_OBLIQUE_MERCATOR_VARIANT_A);
        } else if (hasParamValue(step, "lat_1") &&
                   hasParamValue(step, "lon_1") &&
                   hasParamValue(step, "lat_2") &&
                   hasParamValue(step, "lon_2")) {
            mapping = getMapping(
                PROJ_WKT2_NAME_METHOD_HOTINE_OBLIQUE_MERCATOR_TWO_POINT_NATURAL_ORIGIN);
        } else {
            mapping =
                getMapping(EPSG_CODE_METHOD_HOTINE_OBLIQUE_MERCATOR_VARIANT_B);
        }
    } else if (step.name == "somerc") {
        mapping =
            getMapping(EPSG_CODE_METHOD_HOTINE_OBLIQUE_MERCATOR_VARIANT_B);
        step.paramValues.emplace_back(Step::KeyValue("alpha", "90"));
        step.paramValues.emplace_back(Step::KeyValue("gamma", "90"));
        step.paramValues.emplace_back(
            Step::KeyValue("lonc", getParamValue(step, "lon_0")));
    } else if (step.name == "krovak" &&
               ((getParamValue(step, "axis") == "swu" && iAxisSwap < 0) ||
                (iAxisSwap > 0 &&
                 getParamValue(steps_[iAxisSwap], "order") == "-2,-1"))) {
        mapping = getMapping(EPSG_CODE_METHOD_KROVAK);
    } else if (step.name == "merc") {
        if (hasParamValue(step, "a") && hasParamValue(step, "b") &&
            getParamValue(step, "a") == getParamValue(step, "b") &&
            (!hasParamValue(step, "lat_ts") ||
             getAngularValue(getParamValue(step, "lat_ts")) == 0.0) &&
            getNumericValue(getParamValueK(step)) == 1.0 &&
            getParamValue(step, "nadgrids") == "@null") {
            mapping = getMapping(
                EPSG_CODE_METHOD_POPULAR_VISUALISATION_PSEUDO_MERCATOR);
            for (size_t i = 0; i < step.paramValues.size(); ++i) {
                if (ci_equal(step.paramValues[i].key, "nadgrids")) {
                    ignoreNadgrids_ = true;
                    break;
                }
            }
        } else if (hasParamValue(step, "lat_ts")) {
            mapping = getMapping(EPSG_CODE_METHOD_MERCATOR_VARIANT_B);
        } else {
            mapping = getMapping(EPSG_CODE_METHOD_MERCATOR_VARIANT_A);
        }
    } else if (step.name == "stere") {
        if (hasParamValue(step, "lat_0") &&
            std::fabs(std::fabs(getAngularValue(getParamValue(step, "lat_0"))) -
                      90.0) < 1e-10) {
            const double lat_0 = getAngularValue(getParamValue(step, "lat_0"));
            if (lat_0 > 0) {
                axisType = AxisType::NORTH_POLE;
            } else {
                axisType = AxisType::SOUTH_POLE;
            }
            const auto &lat_ts = getParamValue(step, "lat_ts");
            const auto &k = getParamValueK(step);
            if (!lat_ts.empty() &&
                std::fabs(getAngularValue(lat_ts) - lat_0) > 1e-10 &&
                !k.empty() && std::fabs(getNumericValue(k) - 1) > 1e-10) {
                throw ParsingException("lat_ts != lat_0 and k != 1 not "
                                       "supported for Polar Stereographic");
            }
            if (!lat_ts.empty() &&
                (k.empty() || std::fabs(getNumericValue(k) - 1) < 1e-10)) {
                mapping =
                    getMapping(EPSG_CODE_METHOD_POLAR_STEREOGRAPHIC_VARIANT_B);
            } else {
                mapping =
                    getMapping(EPSG_CODE_METHOD_POLAR_STEREOGRAPHIC_VARIANT_A);
            }
        } else {
            mapping = getMapping(PROJ_WKT2_NAME_METHOD_STEREOGRAPHIC);
        }
    } else if (step.name == "laea") {
        if (hasParamValue(step, "lat_0") &&
            std::fabs(std::fabs(getAngularValue(getParamValue(step, "lat_0"))) -
                      90.0) < 1e-10) {
            const double lat_0 = getAngularValue(getParamValue(step, "lat_0"));
            if (lat_0 > 0) {
                axisType = AxisType::NORTH_POLE;
            } else {
                axisType = AxisType::SOUTH_POLE;
            }
        }
        if (geogCRS->ellipsoid()->isSphere()) {
            mapping = getMapping(
                EPSG_CODE_METHOD_LAMBERT_AZIMUTHAL_EQUAL_AREA_SPHERICAL);
        }
    } else if (step.name == "eqc") {
        if (geogCRS->ellipsoid()->isSphere()) {
            mapping =
                getMapping(EPSG_CODE_METHOD_EQUIDISTANT_CYLINDRICAL_SPHERICAL);
        }
    }

    UnitOfMeasure unit = buildUnit(step, "units", "to_meter");
    if (iUnitConvert >= 0) {
        auto &stepUnitConvert = steps_[iUnitConvert];
        const std::string *xy_in = &getParamValue(stepUnitConvert, "xy_in");
        const std::string *xy_out = &getParamValue(stepUnitConvert, "xy_out");
        if (stepUnitConvert.inverted) {
            std::swap(xy_in, xy_out);
        }
        if (xy_in->empty() || xy_out->empty() || *xy_in != "m") {
            if (step.name != "ob_tran") {
                throw ParsingException(
                    "unhandled values for xy_in and/or xy_out");
            }
        }

        const LinearUnitDesc *unitsMatch = nullptr;
        try {
            double to_meter_value = c_locale_stod(*xy_out);
            unitsMatch = getLinearUnits(to_meter_value);
            if (unitsMatch == nullptr) {
                unit = _buildUnit(to_meter_value);
            }
        } catch (const std::invalid_argument &) {
            unitsMatch = getLinearUnits(*xy_out);
            if (!unitsMatch) {
                if (step.name != "ob_tran") {
                    throw ParsingException(
                        "unhandled values for xy_in and/or xy_out");
                }
            } else {
                unit = _buildUnit(unitsMatch);
            }
        }
    }

    ConversionPtr conv;

    auto mapWithUnknownName = createMapWithUnknownName();

    if (step.name == "utm") {
        const int zone = std::atoi(getParamValue(step, "zone").c_str());
        const bool north = !hasParamValue(step, "south");
        conv =
            Conversion::createUTM(emptyPropertyMap, zone, north).as_nullable();
    } else if (mapping) {

        auto methodMap =
            PropertyMap().set(IdentifiedObject::NAME_KEY, mapping->wkt2_name);
        if (mapping->epsg_code) {
            methodMap.set(Identifier::CODESPACE_KEY, Identifier::EPSG)
                .set(Identifier::CODE_KEY, mapping->epsg_code);
        }
        std::vector<OperationParameterNNPtr> parameters;
        std::vector<ParameterValueNNPtr> values;
        for (int i = 0; mapping->params[i] != nullptr; i++) {
            const auto *param = mapping->params[i];
            std::string proj_name(param->proj_name ? param->proj_name : "");
            const std::string *paramValue =
                (proj_name == "k" || proj_name == "k_0")
                    ? &getParamValueK(step)
                    : !proj_name.empty() ? &getParamValue(step, proj_name)
                                         : &emptyString;
            double value = 0;
            if (!paramValue->empty()) {
                bool hasError = false;
                if (param->unit_type == UnitOfMeasure::Type::ANGULAR) {
                    value = getAngularValue(*paramValue, &hasError);
                } else {
                    value = getNumericValue(*paramValue, &hasError);
                }
                if (hasError) {
                    throw ParsingException("invalid value for " + proj_name);
                }

            } else if (param->unit_type == UnitOfMeasure::Type::SCALE) {
                value = 1;
            } else {
                // For omerc, if gamma is missing, the default value is
                // alpha
                if (step.name == "omerc" && proj_name == "gamma") {
                    paramValue = &getParamValue(step, "alpha");
                    if (!paramValue->empty()) {
                        value = getAngularValue(*paramValue);
                    }
                } else if (step.name == "krovak") {
                    if (param->epsg_code ==
                        EPSG_CODE_PARAMETER_COLATITUDE_CONE_AXIS) {
                        value = 30.28813975277777776;
                    } else if (
                        param->epsg_code ==
                        EPSG_CODE_PARAMETER_LATITUDE_PSEUDO_STANDARD_PARALLEL) {
                        value = 78.5;
                    }
                }
            }

            PropertyMap propertiesParameter;
            propertiesParameter.set(IdentifiedObject::NAME_KEY,
                                    param->wkt2_name);
            if (param->epsg_code) {
                propertiesParameter.set(Identifier::CODE_KEY, param->epsg_code);
                propertiesParameter.set(Identifier::CODESPACE_KEY,
                                        Identifier::EPSG);
            }
            parameters.push_back(
                OperationParameter::create(propertiesParameter));
            // In PROJ convention, angular parameters are always in degree
            // and linear parameters always in metre.
            double valRounded =
                param->unit_type == UnitOfMeasure::Type::LINEAR
                    ? Length(value, UnitOfMeasure::METRE).convertToUnit(unit)
                    : value;
            if (std::fabs(valRounded - std::round(valRounded)) < 1e-8) {
                valRounded = std::round(valRounded);
            }
            values.push_back(ParameterValue::create(Measure(
                valRounded,
                param->unit_type == UnitOfMeasure::Type::ANGULAR
                    ? UnitOfMeasure::DEGREE
                    : param->unit_type == UnitOfMeasure::Type::LINEAR
                          ? unit
                          : param->unit_type == UnitOfMeasure::Type::SCALE
                                ? UnitOfMeasure::SCALE_UNITY
                                : UnitOfMeasure::NONE)));
        }

        if (step.name == "tmerc" && hasParamValue(step, "approx")) {
            methodMap.set("proj_method", "tmerc approx");
        } else if (step.name == "utm" && hasParamValue(step, "approx")) {
            methodMap.set("proj_method", "utm approx");
        }

        conv = Conversion::create(mapWithUnknownName, methodMap, parameters,
                                  values)
                   .as_nullable();
    } else {
        std::vector<OperationParameterNNPtr> parameters;
        std::vector<ParameterValueNNPtr> values;
        std::string methodName = "PROJ " + step.name;
        for (const auto &param : step.paramValues) {
            if (param.key == "wktext" || param.key == "no_defs" ||
                param.key == "datum" || param.key == "ellps" ||
                param.key == "a" || param.key == "b" || param.key == "R" ||
                param.key == "towgs84" || param.key == "nadgrids" ||
                param.key == "geoidgrids" || param.key == "units" ||
                param.key == "to_meter" || param.key == "vunits" ||
                param.key == "vto_meter" || param.key == "type") {
                continue;
            }
            if (param.value.empty()) {
                methodName += " " + param.key;
            } else if (param.key == "o_proj") {
                methodName += " " + param.key + "=" + param.value;
            } else {
                parameters.push_back(OperationParameter::create(
                    PropertyMap().set(IdentifiedObject::NAME_KEY, param.key)));
                bool hasError = false;
                if (param.key == "x_0" || param.key == "y_0") {
                    double value = getNumericValue(param.value, &hasError);
                    values.push_back(ParameterValue::create(
                        Measure(value, UnitOfMeasure::METRE)));
                } else if (param.key == "k" || param.key == "k_0") {
                    double value = getNumericValue(param.value, &hasError);
                    values.push_back(ParameterValue::create(
                        Measure(value, UnitOfMeasure::SCALE_UNITY)));
                } else {
                    double value = getAngularValue(param.value, &hasError);
                    values.push_back(ParameterValue::create(
                        Measure(value, UnitOfMeasure::DEGREE)));
                }
                if (hasError) {
                    throw ParsingException("invalid value for " + param.key);
                }
            }
        }
        conv = Conversion::create(
                   mapWithUnknownName,
                   PropertyMap().set(IdentifiedObject::NAME_KEY, methodName),
                   parameters, values)
                   .as_nullable();

        if (methodName == "PROJ ob_tran o_proj=longlat") {
            return DerivedGeographicCRS::create(
                PropertyMap().set(IdentifiedObject::NAME_KEY, "unnamed"),
                geogCRS, NN_NO_CHECK(conv),
                buildEllipsoidalCS(iStep, iUnitConvert, iAxisSwap, false,
                                   false));
        }
    }

    std::vector<CoordinateSystemAxisNNPtr> axis =
        processAxisSwap(step, unit, iAxisSwap, axisType, false);

    auto cs = CartesianCS::create(emptyPropertyMap, axis[0], axis[1]);

    auto props = PropertyMap().set(IdentifiedObject::NAME_KEY,
                                   title.empty() ? "unknown" : title);
    if (hasUnusedParameters(step)) {
        props.set("EXTENSION_PROJ4", projString_);
    }

    props.set("IMPLICIT_CS", true);

    CRSNNPtr crs = ProjectedCRS::create(props, geogCRS, NN_NO_CHECK(conv), cs);

    if (!hasParamValue(step, "geoidgrids") &&
        (hasParamValue(step, "vunits") || hasParamValue(step, "vto_meter"))) {
        auto vdatum = VerticalReferenceFrame::create(mapWithUnknownName);

        const UnitOfMeasure vunit = buildUnit(step, "vunits", "vto_meter");

        auto vcrs =
            VerticalCRS::create(mapWithUnknownName, vdatum,
                                VerticalCS::createGravityRelatedHeight(vunit));

        crs = CompoundCRS::create(mapWithUnknownName,
                                  std::vector<CRSNNPtr>{crs, vcrs});
    }
    return crs;
}

//! @endcond

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
static const metadata::ExtentPtr nullExtent{};

static const metadata::ExtentPtr &getExtent(const crs::CRS *crs) {
    const auto &domains = crs->domains();
    if (!domains.empty()) {
        return domains[0]->domainOfValidity();
    }
    return nullExtent;
}

//! @endcond

// ---------------------------------------------------------------------------

/** \brief Instantiate a sub-class of BaseObject from a PROJ string.
 *
 * The projString must contain +type=crs for the object to be detected as a
 * CRS instead of a CoordinateOperation.
 *
 * @throw ParsingException
 */
BaseObjectNNPtr
PROJStringParser::createFromPROJString(const std::string &projString) {

    // In some abnormal situations involving init=epsg:XXXX syntax, we could
    // have infinite loop
    if (d->ctx_ && d->ctx_->curStringInCreateFromPROJString == projString) {
        throw ParsingException("invalid PROJ string");
    }

    d->steps_.clear();
    d->title_.clear();
    d->globalParamValues_.clear();
    d->projString_ = projString;
    PROJStringSyntaxParser(projString, d->steps_, d->globalParamValues_,
                           d->title_, false);

    if (d->steps_.empty()) {
        const auto &vunits = d->getGlobalParamValue("vunits");
        const auto &vto_meter = d->getGlobalParamValue("vto_meter");
        if (!vunits.empty() || !vto_meter.empty()) {
            Step fakeStep;
            if (!vunits.empty()) {
                fakeStep.paramValues.emplace_back(
                    Step::KeyValue("vunits", vunits));
            }
            if (!vto_meter.empty()) {
                fakeStep.paramValues.emplace_back(
                    Step::KeyValue("vto_meter", vto_meter));
            }
            auto vdatum =
                VerticalReferenceFrame::create(createMapWithUnknownName());
            auto vcrs = VerticalCRS::create(
                createMapWithUnknownName(), vdatum,
                VerticalCS::createGravityRelatedHeight(
                    d->buildUnit(fakeStep, "vunits", "vto_meter")));
            return vcrs;
        }
    }

    const bool isGeocentricCRS =
        ((d->steps_.size() == 1 &&
          d->getParamValue(d->steps_[0], "type") == "crs") ||
         (d->steps_.size() == 2 && d->steps_[1].name == "unitconvert")) &&
        !d->steps_[0].inverted && isGeocentricStep(d->steps_[0].name);

    // +init=xxxx:yyyy syntax
    if (d->steps_.size() == 1 && d->steps_[0].isInit &&
        !d->steps_[0].inverted) {

        // Those used to come from a text init file
        // We only support them in compatibility mode
        const std::string &stepName = d->steps_[0].name;
        if (ci_starts_with(stepName, "epsg:") ||
            ci_starts_with(stepName, "IGNF:")) {
            bool usePROJ4InitRules = d->usePROJ4InitRules_;
            if (!usePROJ4InitRules) {
                PJ_CONTEXT *ctx = proj_context_create();
                if (ctx) {
                    usePROJ4InitRules = proj_context_get_use_proj4_init_rules(
                                            ctx, FALSE) == TRUE;
                    proj_context_destroy(ctx);
                }
            }
            if (!usePROJ4InitRules) {
                throw ParsingException("init=epsg:/init=IGNF: syntax not "
                                       "supported in non-PROJ4 emulation mode");
            }

            PJ_CONTEXT *ctx = proj_context_create();
            char unused[256];
            std::string initname(stepName);
            initname.resize(initname.find(':'));
            int file_found =
                pj_find_file(ctx, initname.c_str(), unused, sizeof(unused));
            proj_context_destroy(ctx);
            if (!file_found) {
                auto obj = createFromUserInput(stepName, d->dbContext_, true);
                auto crs = dynamic_cast<CRS *>(obj.get());
                if (crs &&
                    (d->steps_[0].paramValues.empty() ||
                     (d->steps_[0].paramValues.size() == 1 &&
                      d->getParamValue(d->steps_[0], "type") == "crs"))) {
                    PropertyMap properties;
                    properties.set(IdentifiedObject::NAME_KEY,
                                   d->title_.empty() ? crs->nameStr()
                                                     : d->title_);
                    const auto &extent = getExtent(crs);
                    if (extent) {
                        properties.set(
                            common::ObjectUsage::DOMAIN_OF_VALIDITY_KEY,
                            NN_NO_CHECK(extent));
                    }
                    auto geogCRS = dynamic_cast<GeographicCRS *>(crs);
                    if (geogCRS) {
                        // Override with longitude latitude in degrees
                        return GeographicCRS::create(
                            properties, geogCRS->datum(),
                            geogCRS->datumEnsemble(),
                            EllipsoidalCS::createLongitudeLatitude(
                                UnitOfMeasure::DEGREE));
                    }
                    auto projCRS = dynamic_cast<ProjectedCRS *>(crs);
                    if (projCRS) {
                        // Override with easting northing order
                        const auto &conv = projCRS->derivingConversionRef();
                        if (conv->method()->getEPSGCode() !=
                            EPSG_CODE_METHOD_TRANSVERSE_MERCATOR_SOUTH_ORIENTATED) {
                            return ProjectedCRS::create(
                                properties, projCRS->baseCRS(), conv,
                                CartesianCS::createEastingNorthing(
                                    projCRS->coordinateSystem()
                                        ->axisList()[0]
                                        ->unit()));
                        }
                    }
                    return obj;
                }
                auto projStringExportable =
                    dynamic_cast<IPROJStringExportable *>(crs);
                if (projStringExportable) {
                    std::string expanded;
                    if (!d->title_.empty()) {
                        expanded = "title=";
                        expanded += d->title_;
                    }
                    for (const auto &pair : d->steps_[0].paramValues) {
                        if (!expanded.empty())
                            expanded += ' ';
                        expanded += '+';
                        expanded += pair.key;
                        if (!pair.value.empty()) {
                            expanded += '=';
                            expanded += pair.value;
                        }
                    }
                    expanded += ' ';
                    expanded += projStringExportable->exportToPROJString(
                        PROJStringFormatter::create().get());
                    return createFromPROJString(expanded);
                }
            }
        }

        paralist *init = pj_mkparam(("init=" + d->steps_[0].name).c_str());
        if (!init) {
            throw ParsingException("out of memory");
        }
        PJ_CONTEXT *ctx = d->ctx_ ? d->ctx_ : proj_context_create();
        if (!ctx) {
            pj_dealloc(init);
            throw ParsingException("out of memory");
        }
        paralist *list = pj_expand_init(ctx, init);
        if (ctx != d->ctx_) {
            proj_context_destroy(ctx);
        }
        if (!list) {
            pj_dealloc(init);
            throw ParsingException("cannot expand " + projString);
        }
        std::string expanded;
        if (!d->title_.empty()) {
            expanded = "title=" + d->title_;
        }
        bool first = true;
        bool has_init_term = false;
        for (auto t = list; t;) {
            if (!expanded.empty()) {
                expanded += ' ';
            }
            if (first) {
                // first parameter is the init= itself
                first = false;
            } else if (starts_with(t->param, "init=")) {
                has_init_term = true;
            } else {
                expanded += t->param;
            }

            auto n = t->next;
            pj_dealloc(t);
            t = n;
        }
        for (const auto &pair : d->steps_[0].paramValues) {
            expanded += " +";
            expanded += pair.key;
            if (!pair.value.empty()) {
                expanded += '=';
                expanded += pair.value;
            }
        }

        if (!has_init_term) {
            return createFromPROJString(expanded);
        }
    }

    int iFirstGeogStep = -1;
    int iSecondGeogStep = -1;
    int iProjStep = -1;
    int iFirstUnitConvert = -1;
    int iSecondUnitConvert = -1;
    int iFirstAxisSwap = -1;
    int iSecondAxisSwap = -1;
    bool unexpectedStructure = d->steps_.empty();
    for (int i = 0; i < static_cast<int>(d->steps_.size()); i++) {
        const auto &stepName = d->steps_[i].name;
        if (isGeographicStep(stepName)) {
            if (iFirstGeogStep < 0) {
                iFirstGeogStep = i;
            } else if (iSecondGeogStep < 0) {
                iSecondGeogStep = i;
            } else {
                unexpectedStructure = true;
                break;
            }
        } else if (ci_equal(stepName, "unitconvert")) {
            if (iFirstUnitConvert < 0) {
                iFirstUnitConvert = i;
            } else if (iSecondUnitConvert < 0) {
                iSecondUnitConvert = i;
            } else {
                unexpectedStructure = true;
                break;
            }
        } else if (ci_equal(stepName, "axisswap")) {
            if (iFirstAxisSwap < 0) {
                iFirstAxisSwap = i;
            } else if (iSecondAxisSwap < 0) {
                iSecondAxisSwap = i;
            } else {
                unexpectedStructure = true;
                break;
            }
        } else if (isProjectedStep(stepName)) {
            if (iProjStep >= 0) {
                unexpectedStructure = true;
                break;
            }
            iProjStep = i;
        } else {
            unexpectedStructure = true;
            break;
        }
    }

    if (!d->steps_.empty()) {
        // CRS candidate
        if ((d->steps_.size() == 1 &&
             d->getParamValue(d->steps_[0], "type") != "crs") ||
            (d->steps_.size() > 1 && d->getGlobalParamValue("type") != "crs")) {
            unexpectedStructure = true;
        }
    }

    struct Logger {
        std::string msg{};

        // cppcheck-suppress functionStatic
        void setMessage(const char *msgIn) noexcept {
            try {
                msg = msgIn;
            } catch (const std::exception &) {
            }
        }

        static void log(void *user_data, int level, const char *msg) {
            if (level == PJ_LOG_ERROR) {
                static_cast<Logger *>(user_data)->setMessage(msg);
            }
        }
    };

    // If the structure is not recognized, then try to instantiate the
    // pipeline, and if successful, wrap it in a PROJBasedOperation
    Logger logger;
    bool valid;

    auto pj_context = d->ctx_ ? d->ctx_ : proj_context_create();
    if (!pj_context) {
        throw ParsingException("out of memory");
    }
    if (pj_context != d->ctx_) {
        proj_log_func(pj_context, &logger, Logger::log);
        proj_context_use_proj4_init_rules(pj_context, d->usePROJ4InitRules_);
    }
    if (d->ctx_) {
        d->ctx_->curStringInCreateFromPROJString = projString;
    }
    auto log_level = proj_log_level(pj_context, PJ_LOG_NONE);
    auto pj = pj_create_internal(
        pj_context, (projString.find("type=crs") != std::string::npos
                         ? projString + " +disable_grid_presence_check"
                         : projString)
                        .c_str());
    if (d->ctx_) {
        d->ctx_->curStringInCreateFromPROJString.clear();
    }
    valid = pj != nullptr;
    proj_log_level(pj_context, log_level);

    // Remove parameters not understood by PROJ.
    if (valid && d->steps_.size() == 1) {
        std::vector<Step::KeyValue> newParamValues{};
        std::set<std::string> foundKeys;
        auto &step = d->steps_[0];

        for (auto &kv : step.paramValues) {
            bool recognizedByPROJ = false;
            if (foundKeys.find(kv.key) != foundKeys.end()) {
                continue;
            }
            foundKeys.insert(kv.key);
            if (step.name == "krovak" && kv.key == "alpha") {
                // We recognize it in our CRS parsing code
                recognizedByPROJ = true;
            } else {
                for (auto cur = pj->params; cur; cur = cur->next) {
                    const char *equal = strchr(cur->param, '=');
                    if (equal &&
                        static_cast<size_t>(equal - cur->param) ==
                            kv.key.size()) {
                        if (memcmp(cur->param, kv.key.c_str(), kv.key.size()) ==
                            0) {
                            recognizedByPROJ = (cur->used == 1);
                            break;
                        }
                    } else if (strcmp(cur->param, kv.key.c_str()) == 0) {
                        recognizedByPROJ = (cur->used == 1);
                        break;
                    }
                }
            }
            if (recognizedByPROJ) {
                newParamValues.emplace_back(kv);
            }
        }
        step.paramValues = newParamValues;

        d->projString_.clear();
        if (!step.name.empty()) {
            d->projString_ += step.isInit ? "+init=" : "+proj=";
            d->projString_ += step.name;
        }
        for (const auto &paramValue : step.paramValues) {
            if (!d->projString_.empty()) {
                d->projString_ += ' ';
            }
            d->projString_ += '+';
            d->projString_ += paramValue.key;
            if (!paramValue.value.empty()) {
                d->projString_ += '=';
                d->projString_ +=
                    pj_double_quote_string_param_if_needed(paramValue.value);
            }
        }
    }

    proj_destroy(pj);

    if (!valid) {
        std::string prefix("Error " + toString(proj_context_errno(pj_context)) +
                           ": ");
        if (logger.msg.empty()) {
            logger.msg =
                prefix + proj_errno_string(proj_context_errno(pj_context));
        } else {
            logger.msg = prefix + logger.msg;
        }
    }

    if (pj_context != d->ctx_) {
        proj_context_destroy(pj_context);
    }

    if (!valid) {
        throw ParsingException(logger.msg);
    }

    if (isGeocentricCRS) {
        // First run is dry run to mark all recognized/unrecognized tokens
        for (int iter = 0; iter < 2; iter++) {
            auto obj = d->buildBoundOrCompoundCRSIfNeeded(
                0, d->buildGeocentricCRS(0, (d->steps_.size() == 2 &&
                                             d->steps_[1].name == "unitconvert")
                                                ? 1
                                                : -1));
            if (iter == 1) {
                return nn_static_pointer_cast<BaseObject>(obj);
            }
        }
    }

    if (!unexpectedStructure) {
        if (iFirstGeogStep == 0 && !d->steps_[iFirstGeogStep].inverted &&
            iSecondGeogStep < 0 && iProjStep < 0 &&
            (iFirstUnitConvert < 0 || iSecondUnitConvert < 0) &&
            (iFirstAxisSwap < 0 || iSecondAxisSwap < 0)) {
            const bool ignoreVUnits = false;
            // First run is dry run to mark all recognized/unrecognized tokens
            for (int iter = 0; iter < 2; iter++) {
                auto obj = d->buildBoundOrCompoundCRSIfNeeded(
                    0,
                    d->buildGeographicCRS(iFirstGeogStep, iFirstUnitConvert,
                                          iFirstAxisSwap, ignoreVUnits, false));
                if (iter == 1) {
                    return nn_static_pointer_cast<BaseObject>(obj);
                }
            }
        }
        if (iProjStep >= 0 && !d->steps_[iProjStep].inverted &&
            (iFirstGeogStep < 0 || iFirstGeogStep + 1 == iProjStep) &&
            iSecondGeogStep < 0) {
            if (iFirstGeogStep < 0)
                iFirstGeogStep = iProjStep;
            const bool ignoreVUnits = true;
            // First run is dry run to mark all recognized/unrecognized tokens
            for (int iter = 0; iter < 2; iter++) {
                auto obj = d->buildBoundOrCompoundCRSIfNeeded(
                    iProjStep,
                    d->buildProjectedCRS(
                        iProjStep,
                        d->buildGeographicCRS(
                            iFirstGeogStep, iFirstUnitConvert < iFirstGeogStep
                                                ? iFirstUnitConvert
                                                : -1,
                            iFirstAxisSwap < iFirstGeogStep ? iFirstAxisSwap
                                                            : -1,
                            ignoreVUnits, true),
                        iFirstUnitConvert < iFirstGeogStep ? iSecondUnitConvert
                                                           : iFirstUnitConvert,
                        iFirstAxisSwap < iFirstGeogStep ? iSecondAxisSwap
                                                        : iFirstAxisSwap));
                if (iter == 1) {
                    return nn_static_pointer_cast<BaseObject>(obj);
                }
            }
        }
    }

    auto props = PropertyMap();
    if (!d->title_.empty()) {
        props.set(IdentifiedObject::NAME_KEY, d->title_);
    }
    return operation::SingleOperation::createPROJBased(props, projString,
                                                       nullptr, nullptr, {});
}

} // namespace io
NS_PROJ_END
