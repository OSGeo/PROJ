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

#include "proj/datum.hh"
#include "proj/common.hh"
#include "proj/internal.hh"
#include "proj/io.hh"
#include "proj/io_internal.hh"
#include "proj/metadata.hh"
#include "proj/util.hh"

#include <memory>
#include <string>

using namespace NS_PROJ::common;
using namespace NS_PROJ::datum;
using namespace NS_PROJ::internal;
using namespace NS_PROJ::io;
using namespace NS_PROJ::metadata;
using namespace NS_PROJ::util;

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
struct Datum::Private {
    optional<std::string> anchorDefinition{};
    optional<Date> publicationDate{};
    optional<IdentifiedObject> conventionalRS{};
};
//! @endcond

// ---------------------------------------------------------------------------

Datum::Datum() : d(internal::make_unique<Private>()) {}

// ---------------------------------------------------------------------------

Datum::Datum(const Datum &other)
    : ObjectUsage(other), d(internal::make_unique<Private>(*other.d)) {}

// ---------------------------------------------------------------------------

Datum::~Datum() = default;

// ---------------------------------------------------------------------------

const optional<std::string> &Datum::anchorDefinition() const {
    return d->anchorDefinition;
}

// ---------------------------------------------------------------------------

const optional<Date> &Datum::publicationDate() const {
    return d->publicationDate;
}

// ---------------------------------------------------------------------------

const optional<IdentifiedObject> &Datum::conventionalRS() const {
    return d->conventionalRS;
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
struct PrimeMeridian::Private {
    Angle greenwichLongitude{};
};
//! @endcond

// ---------------------------------------------------------------------------

PrimeMeridian::PrimeMeridian() : d(internal::make_unique<Private>()) {}

// ---------------------------------------------------------------------------

PrimeMeridian::PrimeMeridian(const PrimeMeridian &other)
    : IdentifiedObject(other), d(internal::make_unique<Private>(*other.d)) {}

// ---------------------------------------------------------------------------

PrimeMeridian::~PrimeMeridian() = default;

// ---------------------------------------------------------------------------

const Angle &PrimeMeridian::greenwichLongitude() const {
    return d->greenwichLongitude;
}

// ---------------------------------------------------------------------------

PrimeMeridianNNPtr PrimeMeridian::create(const PropertyMap &properties,
                                         const Angle &longitude) {
    auto pm(PrimeMeridian::nn_make_shared<PrimeMeridian>());
    pm->setProperties(properties);
    pm->d->greenwichLongitude = longitude;
    return pm;
}

// ---------------------------------------------------------------------------

const PrimeMeridianNNPtr PrimeMeridian::createGREENWICH() {
    return create(PropertyMap()
                      .set(Identifier::AUTHORITY_KEY, "EPSG")
                      .set(Identifier::CODE_KEY, 8901)
                      .set(Identifier::DESCRIPTION_KEY, "Greenwich"),
                  Angle(0));
}

// ---------------------------------------------------------------------------

std::string PrimeMeridian::exportToWKT(
    WKTFormatterNNPtr formatter) const // throw(FormattingException)
{
    const bool isWKT2 = formatter->version() == WKTFormatter::Version::WKT2;
    std::string l_name = name()->description().has_value()
                             ? *(name()->description())
                             : "Greenwich";
    if (!(isWKT2 && formatter->primeMeridianOmittedIfGreenwich() &&
          l_name == "Greenwich")) {
        formatter->startNode("PRIMEM");
        formatter->addQuotedString(l_name);
        if (formatter->primeMeridianInDegree()) {
            formatter->add(greenwichLongitude()
                               .convertToUnit(UnitOfMeasure::DEGREE)
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
                    formatter, WKTConstants::ANGLEUNIT);
            }
        } else if (!formatter->primeMeridianInDegree()) {
            greenwichLongitude().unit().exportToWKT(formatter);
        }
        if (formatter->outputId()) {
            formatID(formatter);
        }
        if (isWKT2) {
            formatRemarks(formatter);
        }
        formatter->endNode();
    }
    return formatter->toString();
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
struct Ellipsoid::Private {
    Length semiMajorAxis{};
    optional<Scale> inverseFlattening{};
    optional<Length> semiMinorAxis{};
    bool isSphere{false};
    optional<Length> semiMedianAxis{};
};
//! @endcond

// ---------------------------------------------------------------------------

Ellipsoid::Ellipsoid() : d(internal::make_unique<Private>()) {}

// ---------------------------------------------------------------------------

Ellipsoid::Ellipsoid(const Ellipsoid &other)
    : IdentifiedObject(other), d(internal::make_unique<Private>(*other.d)) {}

// ---------------------------------------------------------------------------

Ellipsoid::~Ellipsoid() = default;

// ---------------------------------------------------------------------------

const Length &Ellipsoid::semiMajorAxis() const { return d->semiMajorAxis; }

// ---------------------------------------------------------------------------

const optional<Scale> &Ellipsoid::inverseFlattening() const {
    return d->inverseFlattening;
}

// ---------------------------------------------------------------------------

const optional<Length> &Ellipsoid::semiMinorAxis() const {
    return d->semiMinorAxis;
}

// ---------------------------------------------------------------------------

bool Ellipsoid::isSphere() const { return d->isSphere; }

// ---------------------------------------------------------------------------

const optional<Length> &Ellipsoid::semiMedianAxis() const {
    return d->semiMedianAxis;
}

// ---------------------------------------------------------------------------

/* static */
EllipsoidNNPtr Ellipsoid::createFlattenedSphere(const PropertyMap &properties,
                                                const Length &semiMajorAxis,
                                                const Scale &invFlattening) {
    auto ellipsoid(Ellipsoid::nn_make_shared<Ellipsoid>());
    ellipsoid->setProperties(properties);
    ellipsoid->d->semiMajorAxis = semiMajorAxis;
    ellipsoid->d->inverseFlattening = Scale(invFlattening);
    return ellipsoid;
}

// ---------------------------------------------------------------------------

const EllipsoidNNPtr Ellipsoid::createEPSG_7030() {
    PropertyMap propertiesEllps;
    propertiesEllps.set(Identifier::AUTHORITY_KEY, "EPSG")
        .set(Identifier::CODE_KEY, 7030)
        .set(Identifier::DESCRIPTION_KEY, "WGS 84");
    return createFlattenedSphere(propertiesEllps, Length(6378137),
                                 Scale(298.257223563));
}

// ---------------------------------------------------------------------------

std::string Ellipsoid::exportToWKT(
    WKTFormatterNNPtr formatter) const // throw(FormattingException)
{
    const bool isWKT2 = formatter->version() == WKTFormatter::Version::WKT2;
    formatter->startNode(isWKT2 ? WKTConstants::ELLIPSOID
                                : WKTConstants::SPHEROID);
    {
        formatter->addQuotedString(name()->description().has_value()
                                       ? *(name()->description())
                                       : "unnamed");
        if (isWKT2) {
            formatter->add(semiMajorAxis().value());
        } else {
            formatter->add(
                semiMajorAxis().convertToUnit(UnitOfMeasure::METRE).value());
        }
        if (inverseFlattening()) {
            formatter->add(inverseFlattening()->value());
        } else if (semiMinorAxis()) {
            double a = semiMajorAxis().value();
            double b = semiMinorAxis()->value();
            if (a != b) {
                formatter->add(a / (a - b));
            } else {
                formatter->add(0.0);
            }
        } else {
            formatter->add(0.0);
        }
        if (isWKT2 &&
            !(formatter->ellipsoidUnitOmittedIfMetre() &&
              semiMajorAxis().unit() == UnitOfMeasure::METRE)) {
            semiMajorAxis().unit().exportToWKT(formatter,
                                               WKTConstants::LENGTHUNIT);
        }
        if (formatter->outputId()) {
            formatID(formatter);
        }
        if (isWKT2) {
            formatRemarks(formatter);
        }
    }
    formatter->endNode();
    return formatter->toString();
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
struct GeodeticReferenceFrame::Private {
    PrimeMeridianPtr primeMeridian{};
    EllipsoidPtr ellipsoid{};
};
//! @endcond

// ---------------------------------------------------------------------------

GeodeticReferenceFrame::GeodeticReferenceFrame()
    : d(internal::make_unique<Private>()) {}

// ---------------------------------------------------------------------------

GeodeticReferenceFrame::GeodeticReferenceFrame(
    const GeodeticReferenceFrame &other)
    : Datum(other), d(internal::make_unique<Private>(*other.d)) {}

// ---------------------------------------------------------------------------

GeodeticReferenceFrame::~GeodeticReferenceFrame() = default;

// ---------------------------------------------------------------------------

const PrimeMeridianPtr &GeodeticReferenceFrame::primeMeridian() const {
    return d->primeMeridian;
}

// ---------------------------------------------------------------------------

const EllipsoidPtr &GeodeticReferenceFrame::ellipsoid() const {
    return d->ellipsoid;
}
// ---------------------------------------------------------------------------

GeodeticReferenceFrameNNPtr
GeodeticReferenceFrame::create(const PropertyMap &properties,
                               const EllipsoidNNPtr &ellipsoid,
                               const optional<std::string> &anchor,
                               const PrimeMeridianNNPtr &primeMeridian) {
    GeodeticReferenceFrameNNPtr grf(
        GeodeticReferenceFrame::nn_make_shared<GeodeticReferenceFrame>());
    util::nn_static_pointer_cast<Datum>(grf)->d->anchorDefinition = anchor;
    grf->setProperties(properties);
    grf->d->primeMeridian = primeMeridian;
    grf->d->ellipsoid = ellipsoid;
    return grf;
}

// ---------------------------------------------------------------------------

const GeodeticReferenceFrameNNPtr GeodeticReferenceFrame::createEPSG_6326() {
    PropertyMap propertiesDatum;
    propertiesDatum.set(Identifier::AUTHORITY_KEY, "EPSG")
        .set(Identifier::CODE_KEY, 6326)
        .set(Identifier::DESCRIPTION_KEY, "WGS_1984");

    return create(propertiesDatum, Ellipsoid::EPSG_7030,
                  optional<std::string>(), PrimeMeridian::GREENWICH);
}

// ---------------------------------------------------------------------------

std::string GeodeticReferenceFrame::exportToWKT(
    WKTFormatterNNPtr formatter) const // throw(FormattingException)
{
    const bool isWKT2 = formatter->version() == WKTFormatter::Version::WKT2;
    formatter->startNode(WKTConstants::DATUM);
    formatter->addQuotedString(name()->description().has_value()
                                   ? *(name()->description())
                                   : "unnamed");
    ellipsoid()->exportToWKT(formatter);
    if (isWKT2) {
        if (anchorDefinition()) {
            formatter->startNode(WKTConstants::ANCHOR);
            formatter->addQuotedString(*anchorDefinition());
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
    Measure frameReferenceEpoch{};
};
//! @endcond

// ---------------------------------------------------------------------------

DynamicGeodeticReferenceFrame::DynamicGeodeticReferenceFrame()
    : d(internal::make_unique<Private>()) {}

// ---------------------------------------------------------------------------

DynamicGeodeticReferenceFrame::DynamicGeodeticReferenceFrame(
    const DynamicGeodeticReferenceFrame &other)
    : GeodeticReferenceFrame(other),
      d(internal::make_unique<Private>(*other.d)) {}

// ---------------------------------------------------------------------------

DynamicGeodeticReferenceFrame::~DynamicGeodeticReferenceFrame() = default;

// ---------------------------------------------------------------------------

const Measure &DynamicGeodeticReferenceFrame::frameReferenceEpoch() const {
    return d->frameReferenceEpoch;
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
// cppcheck-suppress copyCtorAndEqOperator
struct DatumEnsemble::Private {
    std::vector<DatumPtr> datums{};
    PositionalAccuracy positionalAccuracy{};
};
//! @endcond

// ---------------------------------------------------------------------------

DatumEnsemble::DatumEnsemble() : d(internal::make_unique<Private>()) {}

// ---------------------------------------------------------------------------

DatumEnsemble::DatumEnsemble(const DatumEnsemble &other)
    : IdentifiedObject(other), d(internal::make_unique<Private>(*other.d)) {}

// ---------------------------------------------------------------------------

DatumEnsemble::~DatumEnsemble() = default;

// ---------------------------------------------------------------------------

const std::vector<DatumPtr> &DatumEnsemble::datums() const { return d->datums; }

// ---------------------------------------------------------------------------

const PositionalAccuracy &DatumEnsemble::positionalAccuracy() const {
    return d->positionalAccuracy;
}
