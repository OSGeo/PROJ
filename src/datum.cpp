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

#include "proj/datum.hpp"
#include "proj/common.hpp"
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

#include <memory>
#include <string>

using namespace NS_PROJ::internal;

NS_PROJ_START
namespace datum {

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
struct Datum::Private {
    util::optional<std::string> anchorDefinition{};
    util::optional<common::Date> publicationDate{};
    util::optional<common::IdentifiedObject> conventionalRS{};
};
//! @endcond

// ---------------------------------------------------------------------------

Datum::Datum() : d(internal::make_unique<Private>()) {}

// ---------------------------------------------------------------------------

#ifdef notdef
Datum::Datum(const Datum &other)
    : ObjectUsage(other), d(internal::make_unique<Private>(*other.d)) {}
#endif

// ---------------------------------------------------------------------------

Datum::~Datum() = default;

// ---------------------------------------------------------------------------

const util::optional<std::string> &Datum::anchorDefinition() const {
    return d->anchorDefinition;
}

// ---------------------------------------------------------------------------

const util::optional<common::Date> &Datum::publicationDate() const {
    return d->publicationDate;
}

// ---------------------------------------------------------------------------

const util::optional<common::IdentifiedObject> &Datum::conventionalRS() const {
    return d->conventionalRS;
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
struct PrimeMeridian::Private {
    common::Angle greenwichLongitude{};

    explicit Private(const common::Angle &longitude)
        : greenwichLongitude(longitude) {}
};
//! @endcond

// ---------------------------------------------------------------------------

PrimeMeridian::PrimeMeridian(const common::Angle &longitude)
    : d(internal::make_unique<Private>(longitude)) {}

// ---------------------------------------------------------------------------

#ifdef notdef
PrimeMeridian::PrimeMeridian(const PrimeMeridian &other)
    : common::IdentifiedObject(other),
      d(internal::make_unique<Private>(*other.d)) {}
#endif

// ---------------------------------------------------------------------------

PrimeMeridian::~PrimeMeridian() = default;

// ---------------------------------------------------------------------------

const common::Angle &PrimeMeridian::greenwichLongitude() const {
    return d->greenwichLongitude;
}

// ---------------------------------------------------------------------------

PrimeMeridianNNPtr PrimeMeridian::create(const util::PropertyMap &properties,
                                         const common::Angle &longitude) {
    auto pm(PrimeMeridian::nn_make_shared<PrimeMeridian>(longitude));
    pm->setProperties(properties);
    return pm;
}

// ---------------------------------------------------------------------------

const PrimeMeridianNNPtr PrimeMeridian::createGREENWICH() {
    return create(util::PropertyMap()
                      .set(metadata::Identifier::CODESPACE_KEY, "EPSG")
                      .set(metadata::Identifier::CODE_KEY, 8901)
                      .set(common::IdentifiedObject::NAME_KEY, "Greenwich"),
                  common::Angle(0));
}

// ---------------------------------------------------------------------------

const PrimeMeridianNNPtr PrimeMeridian::createPARIS() {
    return create(util::PropertyMap()
                      .set(metadata::Identifier::CODESPACE_KEY, "EPSG")
                      .set(metadata::Identifier::CODE_KEY, 8903)
                      .set(common::IdentifiedObject::NAME_KEY, "Paris"),
                  common::Angle(2.5969213, common::UnitOfMeasure::GRAD));
}

// ---------------------------------------------------------------------------

std::string PrimeMeridian::exportToWKT(
    io::WKTFormatterNNPtr formatter) const // throw(FormattingException)
{
    const bool isWKT2 = formatter->version() == io::WKTFormatter::Version::WKT2;
    std::string l_name = name()->description().has_value()
                             ? *(name()->description())
                             : "Greenwich";
    if (!(isWKT2 && formatter->primeMeridianOmittedIfGreenwich() &&
          l_name == "Greenwich")) {
        formatter->startNode(io::WKTConstants::PRIMEM, !identifiers().empty());
        formatter->addQuotedString(l_name);
        if (formatter->primeMeridianInDegree()) {
            formatter->add(greenwichLongitude()
                               .convertToUnit(common::UnitOfMeasure::DEGREE)
                               .value());
        } else {
            formatter->add(greenwichLongitude().value());
        }
        if (isWKT2) {
            if (!(formatter
                      ->primeMeridianOrParameterUnitOmittedIfSameAsAxis() &&
                  greenwichLongitude().unit() ==
                      *(formatter->axisAngularUnit()))) {
                greenwichLongitude().unit().exportToWKT(
                    formatter, io::WKTConstants::ANGLEUNIT);
            }
        } else if (!formatter->primeMeridianInDegree()) {
            greenwichLongitude().unit().exportToWKT(formatter);
        }
        if (formatter->outputId()) {
            formatID(formatter);
        }
        formatter->endNode();
    }
    return formatter->toString();
}

// ---------------------------------------------------------------------------

std::string PrimeMeridian::exportToPROJString(
    io::PROJStringFormatterNNPtr formatter) const // throw(FormattingException)
{
    if (greenwichLongitude().getSIValue() != 0) {
        const double valDeg = greenwichLongitude()
                                  .convertToUnit(common::UnitOfMeasure::DEGREE)
                                  .value();
        const double valRad = greenwichLongitude().getSIValue();
        std::string projPMName;
        projCtx ctxt = pj_ctx_alloc();
        for (int i = 0; pj_prime_meridians[i].id != nullptr; ++i) {
            double valRefRad =
                dmstor_ctx(ctxt, pj_prime_meridians[i].defn, nullptr);
            if (::fabs(valRad - valRefRad) < 1e-10) {
                projPMName = pj_prime_meridians[i].id;
                break;
            }
        }
        pj_ctx_free(ctxt);
        if (!projPMName.empty()) {
            formatter->addParam("pm", projPMName);
        } else {
            formatter->addParam("pm", valDeg);
        }
    }
    return formatter->toString();
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
struct Ellipsoid::Private {
    common::Length semiMajorAxis_{};
    util::optional<common::Scale> inverseFlattening_{};
    util::optional<common::Length> semiMinorAxis_{};
    util::optional<common::Length> semiMedianAxis_{};

    explicit Private(const common::Length &radius) : semiMajorAxis_(radius) {}

    Private(const common::Length &semiMajorAxisIn,
            const common::Scale &invFlattening)
        : semiMajorAxis_(semiMajorAxisIn), inverseFlattening_(invFlattening) {}

    Private(const common::Length &semiMajorAxisIn,
            const common::Length &semiMinorAxisIn)
        : semiMajorAxis_(semiMajorAxisIn), semiMinorAxis_(semiMinorAxisIn) {}
};
//! @endcond

// ---------------------------------------------------------------------------

Ellipsoid::Ellipsoid(const common::Length &radius)
    : d(internal::make_unique<Private>(radius)) {}

// ---------------------------------------------------------------------------

Ellipsoid::Ellipsoid(const common::Length &semiMajorAxisIn,
                     const common::Scale &invFlattening)
    : d(internal::make_unique<Private>(semiMajorAxisIn, invFlattening)) {}

// ---------------------------------------------------------------------------

Ellipsoid::Ellipsoid(const common::Length &semiMajorAxisIn,
                     const common::Length &semiMinorAxisIn)
    : d(internal::make_unique<Private>(semiMajorAxisIn, semiMinorAxisIn)) {}

// ---------------------------------------------------------------------------

#ifdef notdef
Ellipsoid::Ellipsoid(const Ellipsoid &other)
    : common::IdentifiedObject(other),
      d(internal::make_unique<Private>(*other.d)) {}
#endif

// ---------------------------------------------------------------------------

Ellipsoid::~Ellipsoid() = default;

// ---------------------------------------------------------------------------

const common::Length &Ellipsoid::semiMajorAxis() const {
    return d->semiMajorAxis_;
}

// ---------------------------------------------------------------------------

const util::optional<common::Scale> &Ellipsoid::inverseFlattening() const {
    return d->inverseFlattening_;
}

// ---------------------------------------------------------------------------

const util::optional<common::Length> &Ellipsoid::semiMinorAxis() const {
    return d->semiMinorAxis_;
}

// ---------------------------------------------------------------------------

bool Ellipsoid::isSphere() const {
    if (inverseFlattening().has_value()) {
        return inverseFlattening()->value() == 0;
    }

    if (semiMinorAxis().has_value()) {
        return semiMajorAxis() == *semiMinorAxis();
    }

    return true;
}

// ---------------------------------------------------------------------------

const util::optional<common::Length> &Ellipsoid::semiMedianAxis() const {
    return d->semiMedianAxis_;
}

// ---------------------------------------------------------------------------

common::Scale Ellipsoid::computeInverseFlattening() const {
    if (inverseFlattening().has_value()) {
        return *inverseFlattening();
    }

    if (semiMinorAxis().has_value()) {
        const double a = semiMajorAxis().value();
        const double b = semiMinorAxis()->value();
        return common::Scale((a == b) ? 0.0 : a / (a - b));
    }

    return common::Scale(0.0);
}

// ---------------------------------------------------------------------------

common::Length Ellipsoid::computeSemiMinorAxis() const {
    if (semiMinorAxis().has_value()) {
        return *semiMinorAxis();
    }

    if (inverseFlattening().has_value()) {
        return common::Length((1.0 - 1.0 / inverseFlattening()->getSIValue()) *
                                  semiMajorAxis().value(),
                              semiMajorAxis().unit());
    }

    return semiMajorAxis();
}

// ---------------------------------------------------------------------------

/* static */
EllipsoidNNPtr Ellipsoid::createSphere(const util::PropertyMap &properties,
                                       const common::Length &radius) {
    auto ellipsoid(Ellipsoid::nn_make_shared<Ellipsoid>(radius));
    ellipsoid->setProperties(properties);
    return ellipsoid;
}

// ---------------------------------------------------------------------------

/* static */
EllipsoidNNPtr
Ellipsoid::createFlattenedSphere(const util::PropertyMap &properties,
                                 const common::Length &semiMajorAxisIn,
                                 const common::Scale &invFlattening) {
    auto ellipsoid(
        Ellipsoid::nn_make_shared<Ellipsoid>(semiMajorAxisIn, invFlattening));
    ellipsoid->setProperties(properties);
    return ellipsoid;
}

// ---------------------------------------------------------------------------

/* static */
EllipsoidNNPtr Ellipsoid::createTwoAxis(const util::PropertyMap &properties,
                                        const common::Length &semiMajorAxisIn,
                                        const common::Length &semiMinorAxisIn) {
    auto ellipsoid(
        Ellipsoid::nn_make_shared<Ellipsoid>(semiMajorAxisIn, semiMinorAxisIn));
    ellipsoid->setProperties(properties);
    return ellipsoid;
}

// ---------------------------------------------------------------------------

const EllipsoidNNPtr Ellipsoid::createWGS84() {
    util::PropertyMap propertiesEllps;
    propertiesEllps.set(metadata::Identifier::CODESPACE_KEY, "EPSG")
        .set(metadata::Identifier::CODE_KEY, 7030)
        .set(common::IdentifiedObject::NAME_KEY, "WGS 84");
    return createFlattenedSphere(propertiesEllps, common::Length(6378137),
                                 common::Scale(298.257223563));
}

// ---------------------------------------------------------------------------

const EllipsoidNNPtr Ellipsoid::createGRS1980() {
    util::PropertyMap propertiesEllps;
    propertiesEllps.set(metadata::Identifier::CODESPACE_KEY, "EPSG")
        .set(metadata::Identifier::CODE_KEY, 7019)
        .set(common::IdentifiedObject::NAME_KEY, "GRS 1980");
    return createFlattenedSphere(propertiesEllps, common::Length(6378137),
                                 common::Scale(298.257222101));
}

// ---------------------------------------------------------------------------

std::string Ellipsoid::exportToWKT(
    io::WKTFormatterNNPtr formatter) const // throw(FormattingException)
{
    const bool isWKT2 = formatter->version() == io::WKTFormatter::Version::WKT2;
    formatter->startNode(isWKT2 ? io::WKTConstants::ELLIPSOID
                                : io::WKTConstants::SPHEROID,
                         !identifiers().empty());
    {
        formatter->addQuotedString(name()->description().has_value()
                                       ? *(name()->description())
                                       : "unnamed");
        if (isWKT2) {
            formatter->add(semiMajorAxis().value());
        } else {
            formatter->add(semiMajorAxis()
                               .convertToUnit(common::UnitOfMeasure::METRE)
                               .value());
        }
        formatter->add(computeInverseFlattening().value());
        if (isWKT2 &&
            !(formatter->ellipsoidUnitOmittedIfMetre() &&
              semiMajorAxis().unit() == common::UnitOfMeasure::METRE)) {
            semiMajorAxis().unit().exportToWKT(formatter,
                                               io::WKTConstants::LENGTHUNIT);
        }
        if (formatter->outputId()) {
            formatID(formatter);
        }
    }
    formatter->endNode();
    return formatter->toString();
}

// ---------------------------------------------------------------------------

std::string Ellipsoid::exportToPROJString(
    io::PROJStringFormatterNNPtr formatter) const // throw(FormattingException)
{
    const double a = semiMajorAxis().getSIValue();
    const double b = computeSemiMinorAxis().getSIValue();
    const double rf = computeInverseFlattening().getSIValue();
    for (int i = 0; pj_ellps[i].id != nullptr; i++) {
        assert(strncmp(pj_ellps[i].major, "a=", 2) == 0);
        const double a_iter = c_locale_stod(pj_ellps[i].major + 2);
        if (::fabs(a - a_iter) < 1e-10 * a_iter) {
            if (strncmp(pj_ellps[i].ell, "b=", 2) == 0) {
                const double b_iter = c_locale_stod(pj_ellps[i].ell + 2);
                if (::fabs(b - b_iter) < 1e-10 * b_iter) {
                    formatter->addParam("ellps", pj_ellps[i].id);
                    return formatter->toString();
                }
            } else {
                assert(strncmp(pj_ellps[i].ell, "rf=", 3) == 0);
                const double rf_iter = c_locale_stod(pj_ellps[i].ell + 3);
                if (::fabs(rf - rf_iter) < 1e-10 * rf_iter) {
                    formatter->addParam("ellps", pj_ellps[i].id);
                    return formatter->toString();
                }
            }
        }
    }

    formatter->addParam("a", a);
    if (inverseFlattening().has_value()) {
        formatter->addParam("rf", rf);
    } else {
        formatter->addParam("b", b);
    }
    return formatter->toString();
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
struct GeodeticReferenceFrame::Private {
    PrimeMeridianNNPtr primeMeridian_;
    EllipsoidNNPtr ellipsoid_;

    Private(const EllipsoidNNPtr &ellipsoidIn,
            const PrimeMeridianNNPtr &primeMeridianIn)
        : primeMeridian_(primeMeridianIn), ellipsoid_(ellipsoidIn) {}
};
//! @endcond

// ---------------------------------------------------------------------------

GeodeticReferenceFrame::GeodeticReferenceFrame(
    const EllipsoidNNPtr &ellipsoidIn,
    const PrimeMeridianNNPtr &primeMeridianIn)
    : d(internal::make_unique<Private>(ellipsoidIn, primeMeridianIn)) {}

// ---------------------------------------------------------------------------

#ifdef notdef
GeodeticReferenceFrame::GeodeticReferenceFrame(
    const GeodeticReferenceFrame &other)
    : Datum(other), d(internal::make_unique<Private>(*other.d)) {}
#endif

// ---------------------------------------------------------------------------

GeodeticReferenceFrame::~GeodeticReferenceFrame() = default;

// ---------------------------------------------------------------------------

const PrimeMeridianNNPtr &GeodeticReferenceFrame::primeMeridian() const {
    return d->primeMeridian_;
}

// ---------------------------------------------------------------------------

const EllipsoidNNPtr &GeodeticReferenceFrame::ellipsoid() const {
    return d->ellipsoid_;
}
// ---------------------------------------------------------------------------

GeodeticReferenceFrameNNPtr
GeodeticReferenceFrame::create(const util::PropertyMap &properties,
                               const EllipsoidNNPtr &ellipsoid,
                               const util::optional<std::string> &anchor,
                               const PrimeMeridianNNPtr &primeMeridian) {
    GeodeticReferenceFrameNNPtr grf(
        GeodeticReferenceFrame::nn_make_shared<GeodeticReferenceFrame>(
            ellipsoid, primeMeridian));
    util::nn_static_pointer_cast<Datum>(grf)->d->anchorDefinition = anchor;
    grf->setProperties(properties);
    return grf;
}

// ---------------------------------------------------------------------------

const GeodeticReferenceFrameNNPtr GeodeticReferenceFrame::createEPSG_6326() {
    util::PropertyMap propertiesDatum;
    propertiesDatum.set(metadata::Identifier::CODESPACE_KEY, "EPSG")
        .set(metadata::Identifier::CODE_KEY, 6326)
        .set(common::IdentifiedObject::NAME_KEY, "WGS_1984");

    return create(propertiesDatum, Ellipsoid::WGS84,
                  util::optional<std::string>(), PrimeMeridian::GREENWICH);
}

// ---------------------------------------------------------------------------

std::string GeodeticReferenceFrame::exportToWKT(
    io::WKTFormatterNNPtr formatter) const // throw(FormattingException)
{
    const bool isWKT2 = formatter->version() == io::WKTFormatter::Version::WKT2;
    formatter->startNode(io::WKTConstants::DATUM, !identifiers().empty());
    formatter->addQuotedString(name()->description().has_value()
                                   ? *(name()->description())
                                   : "unnamed");
    ellipsoid()->exportToWKT(formatter);
    if (isWKT2) {
        if (anchorDefinition()) {
            formatter->startNode(io::WKTConstants::ANCHOR, false);
            formatter->addQuotedString(*anchorDefinition());
            formatter->endNode();
        }
    } else {
        const auto &TOWGS84Params = formatter->getTOWGS84Parameters();
        if (TOWGS84Params.size() == 7) {
            formatter->startNode(io::WKTConstants::TOWGS84, false);
            for (const auto &val : TOWGS84Params) {
                formatter->add(val);
            }
            formatter->endNode();
        }
        std::string extension = formatter->getHDatumExtension();
        if (!extension.empty()) {
            formatter->startNode(io::WKTConstants::EXTENSION, false);
            formatter->addQuotedString("PROJ4_GRIDS");
            formatter->addQuotedString(extension);
            formatter->endNode();
        }
    }
    if (formatter->outputId()) {
        formatID(formatter);
    }
    // the PRIMEM is exported as a child of the CRS
    formatter->endNode();
    return formatter->toString();
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
struct DynamicGeodeticReferenceFrame::Private {
    common::Measure frameReferenceEpoch{};
};
//! @endcond

// ---------------------------------------------------------------------------

DynamicGeodeticReferenceFrame::DynamicGeodeticReferenceFrame(
    const EllipsoidNNPtr &ellipsoidIn,
    const PrimeMeridianNNPtr &primeMeridianIn)
    : GeodeticReferenceFrame(ellipsoidIn, primeMeridianIn),
      d(internal::make_unique<Private>()) {}

// ---------------------------------------------------------------------------

#ifdef notdef
DynamicGeodeticReferenceFrame::DynamicGeodeticReferenceFrame(
    const DynamicGeodeticReferenceFrame &other)
    : GeodeticReferenceFrame(other),
      d(internal::make_unique<Private>(*other.d)) {}
#endif

// ---------------------------------------------------------------------------

DynamicGeodeticReferenceFrame::~DynamicGeodeticReferenceFrame() = default;

// ---------------------------------------------------------------------------

const common::Measure &
DynamicGeodeticReferenceFrame::frameReferenceEpoch() const {
    return d->frameReferenceEpoch;
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
struct DatumEnsemble::Private {
    std::vector<DatumPtr> datums{};
    metadata::PositionalAccuracyNNPtr positionalAccuracy;
};
//! @endcond

// ---------------------------------------------------------------------------

#ifdef notdef
DatumEnsemble::DatumEnsemble(const DatumEnsemble &other)
    : common::IdentifiedObject(other),
      d(internal::make_unique<Private>(*other.d)) {}
#endif

// ---------------------------------------------------------------------------

DatumEnsemble::~DatumEnsemble() = default;

// ---------------------------------------------------------------------------

const std::vector<DatumPtr> &DatumEnsemble::datums() const { return d->datums; }

// ---------------------------------------------------------------------------

const metadata::PositionalAccuracyNNPtr &
DatumEnsemble::positionalAccuracy() const {
    return d->positionalAccuracy;
}

// ---------------------------------------------------------------------------

RealizationMethod::RealizationMethod(const std::string &nameIn)
    : CodeList(nameIn) {}

// ---------------------------------------------------------------------------

RealizationMethod::RealizationMethod(const RealizationMethod &) = default;

// ---------------------------------------------------------------------------

RealizationMethod &RealizationMethod::
operator=(const RealizationMethod &other) {
    CodeList::operator=(other);
    return *this;
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
struct VerticalReferenceFrame::Private {
    util::optional<RealizationMethod> realizationMethod_{};
};
//! @endcond

// ---------------------------------------------------------------------------

VerticalReferenceFrame::VerticalReferenceFrame()
    : d(internal::make_unique<Private>()) {}

// ---------------------------------------------------------------------------

VerticalReferenceFrame::~VerticalReferenceFrame() = default;

// ---------------------------------------------------------------------------

const util::optional<RealizationMethod> &
VerticalReferenceFrame::realizationMethod() const {
    return d->realizationMethod_;
}

// ---------------------------------------------------------------------------

VerticalReferenceFrameNNPtr VerticalReferenceFrame::create(
    const util::PropertyMap &properties,
    const util::optional<std::string> &anchor,
    const util::optional<RealizationMethod> &realizationMethodIn) {
    auto rf(VerticalReferenceFrame::nn_make_shared<VerticalReferenceFrame>());
    util::nn_static_pointer_cast<Datum>(rf)->d->anchorDefinition = anchor;
    rf->setProperties(properties);
    if (realizationMethodIn.has_value() &&
        !realizationMethodIn->toString().empty()) {
        rf->d->realizationMethod_ = *realizationMethodIn;
    }
    return rf;
}

// ---------------------------------------------------------------------------

std::string VerticalReferenceFrame::exportToWKT(
    io::WKTFormatterNNPtr formatter) const // throw(FormattingException)
{
    const bool isWKT2 = formatter->version() == io::WKTFormatter::Version::WKT2;
    if (isWKT2) {
        formatter->startNode(io::WKTConstants::VDATUM, !identifiers().empty());
        formatter->addQuotedString(name()->description().has_value()
                                       ? *(name()->description())
                                       : "unnamed");
        if (anchorDefinition()) {
            formatter->startNode(io::WKTConstants::ANCHOR, false);
            formatter->addQuotedString(*anchorDefinition());
            formatter->endNode();
        }
        if (formatter->outputId()) {
            formatID(formatter);
        }
        formatter->endNode();
    } else {
        formatter->startNode(io::WKTConstants::VERT_DATUM,
                             !identifiers().empty());
        formatter->addQuotedString(name()->description().has_value()
                                       ? *(name()->description())
                                       : "unnamed");
        formatter->add(2005); // CS_VD_GeoidModelDerived from OGC 01-009
        std::string extension = formatter->getVDatumExtension();
        if (!extension.empty()) {
            formatter->startNode(io::WKTConstants::EXTENSION, false);
            formatter->addQuotedString("PROJ4_GRIDS");
            formatter->addQuotedString(extension);
            formatter->endNode();
        }
        if (formatter->outputId()) {
            formatID(formatter);
        }
        formatter->endNode();
    }
    return formatter->toString();
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
struct TemporalDatum::Private {
    common::DateTime temporalOrigin_;
    std::string calendar_;

    Private(const common::DateTime &temporalOriginIn,
            const std::string &calendarIn)
        : temporalOrigin_(temporalOriginIn), calendar_(calendarIn) {}
};
//! @endcond

// ---------------------------------------------------------------------------

TemporalDatum::TemporalDatum(const common::DateTime &temporalOriginIn,
                             const std::string &calendarIn)
    : d(internal::make_unique<Private>(temporalOriginIn, calendarIn)) {}

// ---------------------------------------------------------------------------

TemporalDatum::~TemporalDatum() = default;

// ---------------------------------------------------------------------------

const common::DateTime &TemporalDatum::temporalOrigin() const {
    return d->temporalOrigin_;
}

// ---------------------------------------------------------------------------

const std::string &TemporalDatum::calendar() const { return d->calendar_; }

// ---------------------------------------------------------------------------

TemporalDatumNNPtr
TemporalDatum::create(const util::PropertyMap &properties,
                      const common::DateTime &temporalOriginIn,
                      const std::string &calendarIn) {
    auto datum(TemporalDatum::nn_make_shared<TemporalDatum>(temporalOriginIn,
                                                            calendarIn));
    datum->setProperties(properties);
    return datum;
}

// ---------------------------------------------------------------------------

std::string TemporalDatum::exportToWKT(
    io::WKTFormatterNNPtr formatter) const // throw(FormattingException)
{
    const bool isWKT2 = formatter->version() == io::WKTFormatter::Version::WKT2;
    if (!isWKT2) {
        throw io::FormattingException(
            "TemporalDatum can only be exported to WKT2");
    }
    formatter->startNode(io::WKTConstants::TDATUM, !identifiers().empty());
    formatter->addQuotedString(*(name()->description()));
    if (formatter->use2018Keywords()) {
        formatter->startNode(io::WKTConstants::CALENDAR, false);
        formatter->addQuotedString(calendar());
        formatter->endNode();
    }

    const auto &timeOriginStr = temporalOrigin().toString();
    if (!timeOriginStr.empty()) {
        formatter->startNode(io::WKTConstants::TIMEORIGIN, false);
        if (temporalOrigin().isISO_8601()) {
            formatter->add(timeOriginStr);
        } else {
            formatter->addQuotedString(timeOriginStr);
        }
        formatter->endNode();
    }

    formatter->endNode();
    return formatter->toString();
}

} // namespace datum
NS_PROJ_END
