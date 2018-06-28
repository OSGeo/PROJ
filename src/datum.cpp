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
    util::optional<common::DateTime> publicationDate{};
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

//! @cond Doxygen_Suppress
Datum::~Datum() = default;
//! @endcond

// ---------------------------------------------------------------------------

/** \brief Return the anchor definition.
 *
 * A description - possibly including coordinates of an identified point or
 * points - of the relationship used to anchor a coordinate system to the
 * Earth or alternate object.
 * <ul>
 * <li>For modern geodetic reference frames the anchor may be a set of station
 * coordinates; if the reference frame is dynamic it will also include
 * coordinate velocities. For a traditional geodetic datum, this anchor may be
 * a point known as the fundamental point, which is traditionally the point
 * where the relationship between geoid and ellipsoid is defined, together
 * with a direction from that point.</li>
 * <li>For a vertical reference frame the anchor may be the zero level at one
 * or more defined locations or a conventionally defined surface.</li>
 * <li>For an engineering datum, the anchor may be an identified physical point
 * with the orientation defined relative to the object.</li>
 * </ul>
 *
 * @return the anchor definition, or empty.
 */
const util::optional<std::string> &Datum::anchorDefinition() const {
    return d->anchorDefinition;
}

// ---------------------------------------------------------------------------

/** \brief Return the date on which the datum definition was published.
 *
 * \note Departure from \ref ISO_19111_2018: we return a DateTime instead of
 * a Citation::Date.
 *
 * @return the publication date, or empty.
 */
const util::optional<common::DateTime> &Datum::publicationDate() const {
    return d->publicationDate;
}

// ---------------------------------------------------------------------------

/** \brief Return the conventional reference system.
 *
 * This is the name, identifier, alias and remarks for the terrestrial
 * reference system or vertical reference system realized by this reference
 * frame, for example "ITRS" for ITRF88 through ITRF2008 and ITRF2014, or
 * "EVRS" for EVRF2000 and EVRF2007.
 *
 * @return the conventional reference system, or empty.
 */
const util::optional<common::IdentifiedObject> &Datum::conventionalRS() const {
    return d->conventionalRS;
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
struct PrimeMeridian::Private {
    common::Angle longitude_{};

    explicit Private(const common::Angle &longitude) : longitude_(longitude) {}
};
//! @endcond

// ---------------------------------------------------------------------------

PrimeMeridian::PrimeMeridian(const common::Angle &longitudeIn)
    : d(internal::make_unique<Private>(longitudeIn)) {}

// ---------------------------------------------------------------------------

#ifdef notdef
PrimeMeridian::PrimeMeridian(const PrimeMeridian &other)
    : common::IdentifiedObject(other),
      d(internal::make_unique<Private>(*other.d)) {}
#endif

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
PrimeMeridian::~PrimeMeridian() = default;
//! @endcond

// ---------------------------------------------------------------------------

/** \brief Return the longitude of the prime meridian.
 *
 * It is measured from the internationally-recognised reference meridian
 * ('Greenwich meridian'), positive eastward.
 * The default value is 0 degrees.
 *
 * @return the longitude of the prime meridian.
 */
const common::Angle &PrimeMeridian::longitude() const { return d->longitude_; }

// ---------------------------------------------------------------------------

/** \brief Instanciate a PrimeMeridian.
 *
 * @param properties See \ref general_properties.
 * At minimum the name should be defined.
 * @param longitudeIn the longitude of the prime meridian.
 * @return new PrimeMeridian.
 */
PrimeMeridianNNPtr PrimeMeridian::create(const util::PropertyMap &properties,
                                         const common::Angle &longitudeIn) {
    auto pm(PrimeMeridian::nn_make_shared<PrimeMeridian>(longitudeIn));
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
            formatter->add(longitude()
                               .convertToUnit(common::UnitOfMeasure::DEGREE)
                               .value());
        } else {
            formatter->add(longitude().value());
        }
        if (isWKT2) {
            if (!(formatter
                      ->primeMeridianOrParameterUnitOmittedIfSameAsAxis() &&
                  longitude().unit() == *(formatter->axisAngularUnit()))) {
                longitude().unit().exportToWKT(formatter,
                                               io::WKTConstants::ANGLEUNIT);
            }
        } else if (!formatter->primeMeridianInDegree()) {
            longitude().unit().exportToWKT(formatter);
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
    if (longitude().getSIValue() != 0) {
        const double valDeg =
            longitude().convertToUnit(common::UnitOfMeasure::DEGREE).value();
        const double valRad = longitude().getSIValue();
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

//! @cond Doxygen_Suppress
Ellipsoid::~Ellipsoid() = default;
//! @endcond

// ---------------------------------------------------------------------------

/** \brief Return the length of the semi-major axis of the ellipsoid.
 *
 * @return the semi-major axis.
 */
const common::Length &Ellipsoid::semiMajorAxis() const {
    return d->semiMajorAxis_;
}

// ---------------------------------------------------------------------------

/** \brief Return the inverse flattening value of the ellipsoid, if the
 * ellipsoid
 * has been defined with this value.
 *
 * @see computeInverseFlattening() that will always return a valid value of the
 * inverse flattening, whether the ellipsoid has been defined through inverse
 * flattening or semi-minor axis.
 *
 * @return the inverse flattening value of the ellipsoid, or empty.
 */
const util::optional<common::Scale> &Ellipsoid::inverseFlattening() const {
    return d->inverseFlattening_;
}

// ---------------------------------------------------------------------------

/** \brief Return the length of the semi-minor axis of the ellipsoid, if the
 * ellipsoid
 * has been defined with this value.
 *
 * @see computeSemiMinorAxis() that will always return a valid value of the
 * inverse flattening, whether the ellipsoid has been defined through inverse
 * flattening or semi-minor axis.
 *
 * @return the semi-minor axis of the ellipsoid, or empty.
 */
const util::optional<common::Length> &Ellipsoid::semiMinorAxis() const {
    return d->semiMinorAxis_;
}

// ---------------------------------------------------------------------------

/** \brief Return whether the ellipsoid is spherical.
 *
 * That is to say is semiMajorAxis() == computeSemiMinorAxis().
 *
 * A sphere is completely defined by the semi-major axis, which is the radius
 * of the sphere.
 *
 * @return true if the ellipsoid is spherical.
 */
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

/** \brief Return the length of the semi-median axis of a triaxial ellipsoid
 *
 * This parameter is not required for a biaxial ellipsoid.
 *
 * @return the semi-median axis of the ellipsoid, or empty.
 */
const util::optional<common::Length> &Ellipsoid::semiMedianAxis() const {
    return d->semiMedianAxis_;
}

// ---------------------------------------------------------------------------

/** \brief Return or compute the inverse flattening value of the ellipsoid.
 *
 * If computed, the inverse flattening is the result of a / (a - b),
 * where a is the semi-major axis and b the semi-minor axis.
 *
 * @return the inverse flattening value of the ellipsoid, or 0 for a sphere.
 */
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

/** \brief Return or compute the length of the semi-minor axis of the ellipsoid.
 *
 * If computed, the semi-minor axis is the result of a * (1 - 1 / rf)
 * where a is the semi-major axis and rf the reverse/inverse flattening.

 * @return the semi-minor axis of the ellipsoid.
 */
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

/** \brief Instanciate a Ellipsoid as a sphere.
 *
 * @param properties See \ref general_properties.
 * At minimum the name should be defined.
 * @param radius the sphere radius (semi-major axis).
 * @return new Ellipsoid.
 */
EllipsoidNNPtr Ellipsoid::createSphere(const util::PropertyMap &properties,
                                       const common::Length &radius) {
    auto ellipsoid(Ellipsoid::nn_make_shared<Ellipsoid>(radius));
    ellipsoid->setProperties(properties);
    return ellipsoid;
}

// ---------------------------------------------------------------------------

/** \brief Instanciate a Ellipsoid from its inverse/reverse flattening.
 *
 * @param properties See \ref general_properties.
 * At minimum the name should be defined.
 * @param semiMajorAxisIn the semi-major axis.
 * @param invFlattening the inverse/reverse flattening.
 * @return new Ellipsoid.
 */
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

/** \brief Instanciate a Ellipsoid from the value of its two semi axis.
 *
 * @param properties See \ref general_properties.
 * At minimum the name should be defined.
 * @param semiMajorAxisIn the semi-major axis.
 * @param semiMinorAxisIn the semi-minor axis.
 * @return new Ellipsoid.
 */
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

//! @cond Doxygen_Suppress
GeodeticReferenceFrame::~GeodeticReferenceFrame() = default;
//! @endcond

// ---------------------------------------------------------------------------

/** \brief Return the PrimeMeridian associated with a GeodeticReferenceFrame.
 *
 * @return the PrimeMeridian.
 */
const PrimeMeridianNNPtr &GeodeticReferenceFrame::primeMeridian() const {
    return d->primeMeridian_;
}

// ---------------------------------------------------------------------------

/** \brief Return the Ellipsoid associated with a GeodeticReferenceFrame.
 *
 * \note The \ref ISO_19111_2018 modelling allows (but discourages) a
 * GeodeticReferenceFrame
 * to not be associated with a Ellipsoid in the case where it is used by a
 * geocentric crs::GeodeticCRS. We have made the choice of making the ellipsoid
 * specification compulsory.
 *
 * @return the Ellipsoid.
 */
const EllipsoidNNPtr &GeodeticReferenceFrame::ellipsoid() const {
    return d->ellipsoid_;
}
// ---------------------------------------------------------------------------

/** \brief Instanciate a GeodeticReferenceFrame
 *
 * @param properties See \ref general_properties.
 * At minimum the name should be defined.
 * @param ellipsoid the Ellipsoid.
 * @param anchor the anchor definition, or empty.
 * @param primeMeridian the PrimeMeridian.
 * @return new GeodeticReferenceFrame.
 */
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

//! @cond Doxygen_Suppress
DynamicGeodeticReferenceFrame::~DynamicGeodeticReferenceFrame() = default;
//! @endcond

// ---------------------------------------------------------------------------

/** \brief Return the epoch to which the coordinates of stations defining the
 * dynamic geodetic reference frame are referenced.
 *
 * Usually given as a decimal year e.g. 2016.47.
 *
 * @return the frame reference epoch.
 */
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

//! @cond Doxygen_Suppress
DatumEnsemble::~DatumEnsemble() = default;
//! @endcond

// ---------------------------------------------------------------------------

/** \brief Return the set of datums which may be considered to be
 * insignificantly different from each other.
 *
 * @return the set of datums of the DatumEnsemble.
 */
const std::vector<DatumPtr> &DatumEnsemble::datums() const { return d->datums; }

// ---------------------------------------------------------------------------

/** \brief Return the inaccuracy introduced through use of this collection of
 * datums.
 *
 * It is an indication of the differences in coordinate values at all points
 * between the various realizations that have been grouped into this datum
 * ensemble.
 *
 * @return the accuracy.
 */
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

//! @cond Doxygen_Suppress
VerticalReferenceFrame::~VerticalReferenceFrame() = default;
//! @endcond

// ---------------------------------------------------------------------------

/** \brief Return the method through which this vertical reference frame is
 * realized.
 *
 * @return the realization method.
 */
const util::optional<RealizationMethod> &
VerticalReferenceFrame::realizationMethod() const {
    return d->realizationMethod_;
}

// ---------------------------------------------------------------------------

/** \brief Instanciate a VerticalReferenceFrame
 *
 * @param properties See \ref general_properties.
 * At minimum the name should be defined.
 * @param anchor the anchor definition, or empty.
 * @param realizationMethodIn the realization method, or empty.
 * @return new VerticalReferenceFrame.
 */
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

//! @cond Doxygen_Suppress
TemporalDatum::~TemporalDatum() = default;
//! @endcond

// ---------------------------------------------------------------------------

/** \brief Return the date and time to which temporal coordinates are
 * referenced, expressed in conformance with ISO 8601.
 *
 * @return the temporal origin.
 */
const common::DateTime &TemporalDatum::temporalOrigin() const {
    return d->temporalOrigin_;
}

// ---------------------------------------------------------------------------

/** \brief Return the calendar to which the temporal origin is referenced
 *
 * Default value: TemporalDatum::CALENDAR_PROLEPTIC_GREGORIAN.
 *
 * @return the calendar.
 */
const std::string &TemporalDatum::calendar() const { return d->calendar_; }

// ---------------------------------------------------------------------------

/** \brief Instanciate a TemporalDatum
 *
 * @param properties See \ref general_properties.
 * At minimum the name should be defined.
 * @param temporalOriginIn the temporal origin into which temporal coordinates
 * are referenced.
 * @param calendarIn the calendar (generally
 * TemporalDatum::CALENDAR_PROLEPTIC_GREGORIAN)
 * @return new VerticalReferenceFrame.
 */
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
