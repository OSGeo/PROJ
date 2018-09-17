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
    common::IdentifiedObjectPtr conventionalRS{};
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
 * \note Departure from \ref ISO_19111_2018 : we return a DateTime instead of
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
 * @return the conventional reference system, or nullptr.
 */
const common::IdentifiedObjectPtr &Datum::conventionalRS() const {
    return d->conventionalRS;
}

// ---------------------------------------------------------------------------

bool Datum::_isEquivalentTo(const util::BaseObjectNNPtr &other,
                            util::IComparable::Criterion criterion) const {
    auto otherDatum = util::nn_dynamic_pointer_cast<Datum>(other);
    if (otherDatum == nullptr ||
        !ObjectUsage::_isEquivalentTo(other, criterion)) {
        return false;
    }
    if (criterion == util::IComparable::Criterion::STRICT) {
        if ((anchorDefinition().has_value() ^
             otherDatum->anchorDefinition().has_value())) {
            return false;
        }
        if (anchorDefinition().has_value() &&
            otherDatum->anchorDefinition().has_value() &&
            *anchorDefinition() != *otherDatum->anchorDefinition()) {
            return false;
        }

        if ((publicationDate().has_value() ^
             otherDatum->publicationDate().has_value())) {
            return false;
        }
        if (publicationDate().has_value() &&
            otherDatum->publicationDate().has_value() &&
            publicationDate()->toString() !=
                otherDatum->publicationDate()->toString()) {
            return false;
        }

        if (((conventionalRS() != nullptr) ^
             (otherDatum->conventionalRS() != nullptr))) {
            return false;
        }
        if (conventionalRS() && otherDatum->conventionalRS() &&
            conventionalRS()->_isEquivalentTo(
                NN_CHECK_ASSERT(otherDatum->conventionalRS()), criterion)) {
            return false;
        }
    }
    return true;
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
                      .set(metadata::Identifier::CODESPACE_KEY,
                           metadata::Identifier::EPSG)
                      .set(metadata::Identifier::CODE_KEY, 8901)
                      .set(common::IdentifiedObject::NAME_KEY, "Greenwich"),
                  common::Angle(0));
}

// ---------------------------------------------------------------------------

const PrimeMeridianNNPtr PrimeMeridian::createPARIS() {
    return create(util::PropertyMap()
                      .set(metadata::Identifier::CODESPACE_KEY,
                           metadata::Identifier::EPSG)
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

bool PrimeMeridian::isEquivalentTo(
    const util::BaseObjectNNPtr &other,
    util::IComparable::Criterion criterion) const {
    auto otherPM = util::nn_dynamic_pointer_cast<PrimeMeridian>(other);
    if (otherPM == nullptr ||
        !IdentifiedObject::_isEquivalentTo(other, criterion)) {
        return false;
    }
    return longitude() == otherPM->longitude();
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

Ellipsoid::Ellipsoid(const Ellipsoid &other)
    : IdentifiedObject(other), d(internal::make_unique<Private>(*(other.d))) {}

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

const EllipsoidNNPtr Ellipsoid::createCLARKE_1866() {
    util::PropertyMap propertiesEllps;
    propertiesEllps
        .set(metadata::Identifier::CODESPACE_KEY, metadata::Identifier::EPSG)
        .set(metadata::Identifier::CODE_KEY, 7008)
        .set(common::IdentifiedObject::NAME_KEY, "Clarke 1866");
    return createTwoAxis(propertiesEllps, common::Length(6378206.4),
                         common::Length(6356583.8));
}

// ---------------------------------------------------------------------------

const EllipsoidNNPtr Ellipsoid::createWGS84() {
    util::PropertyMap propertiesEllps;
    propertiesEllps
        .set(metadata::Identifier::CODESPACE_KEY, metadata::Identifier::EPSG)
        .set(metadata::Identifier::CODE_KEY, 7030)
        .set(common::IdentifiedObject::NAME_KEY, "WGS 84");
    return createFlattenedSphere(propertiesEllps, common::Length(6378137),
                                 common::Scale(298.257223563));
}

// ---------------------------------------------------------------------------

const EllipsoidNNPtr Ellipsoid::createGRS1980() {
    util::PropertyMap propertiesEllps;
    propertiesEllps
        .set(metadata::Identifier::CODESPACE_KEY, metadata::Identifier::EPSG)
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

bool Ellipsoid::lookForProjWellKnownEllps(std::string &projEllpsName,
                                          std::string &ellpsName) const {
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
                    projEllpsName = pj_ellps[i].id;
                    ellpsName = pj_ellps[i].name;
                    return true;
                }
            } else {
                assert(strncmp(pj_ellps[i].ell, "rf=", 3) == 0);
                const double rf_iter = c_locale_stod(pj_ellps[i].ell + 3);
                if (::fabs(rf - rf_iter) < 1e-10 * rf_iter) {
                    projEllpsName = pj_ellps[i].id;
                    ellpsName = pj_ellps[i].name;
                    return true;
                }
            }
        }
    }
    return false;
}

// ---------------------------------------------------------------------------

std::string Ellipsoid::exportToPROJString(
    io::PROJStringFormatterNNPtr formatter) const // throw(FormattingException)
{
    const double a = semiMajorAxis().getSIValue();
    const double b = computeSemiMinorAxis().getSIValue();
    const double rf = computeInverseFlattening().getSIValue();

    std::string projEllpsName;
    std::string ellpsName;
    if (lookForProjWellKnownEllps(projEllpsName, ellpsName)) {
        formatter->addParam("ellps", projEllpsName);
        return formatter->toString();
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

/** \brief Return a Ellipsoid object where some parameters are better
 * identified.
 *
 * @return a new Ellipsoid.
 */
EllipsoidNNPtr Ellipsoid::identify() const {
    auto newEllipsoid = Ellipsoid::nn_make_shared<Ellipsoid>(*this);
    newEllipsoid->assignSelf(
        util::nn_static_pointer_cast<util::BaseObject>(newEllipsoid));

    if (name()->description()->empty() ||
        *(name()->description()) == "unknown") {
        std::string projEllpsName;
        std::string ellpsName;
        if (lookForProjWellKnownEllps(projEllpsName, ellpsName)) {
            newEllipsoid->setProperties(
                util::PropertyMap().set(IdentifiedObject::NAME_KEY, ellpsName));
        }
    }

    return newEllipsoid;
}

// ---------------------------------------------------------------------------

bool Ellipsoid::isEquivalentTo(const util::BaseObjectNNPtr &other,
                               util::IComparable::Criterion criterion) const {
    auto otherEllipsoid = util::nn_dynamic_pointer_cast<Ellipsoid>(other);
    if (otherEllipsoid == nullptr ||
        !IdentifiedObject::_isEquivalentTo(other, criterion)) {
        return false;
    }
    if (!semiMajorAxis().isEquivalentTo(otherEllipsoid->semiMajorAxis(),
                                        criterion)) {
        return false;
    }
    if (criterion == util::IComparable::Criterion::STRICT) {
        if ((semiMinorAxis().has_value() ^
             otherEllipsoid->semiMinorAxis().has_value())) {
            return false;
        }
        if (semiMinorAxis().has_value() &&
            otherEllipsoid->semiMinorAxis().has_value()) {
            if (!semiMinorAxis()->isEquivalentTo(
                    *(otherEllipsoid->semiMinorAxis()), criterion)) {
                return false;
            }
        }

        if ((inverseFlattening().has_value() ^
             otherEllipsoid->inverseFlattening().has_value())) {
            return false;
        }
        if (inverseFlattening().has_value() &&
            otherEllipsoid->inverseFlattening().has_value()) {
            if (!inverseFlattening()->isEquivalentTo(
                    *(otherEllipsoid->inverseFlattening()), criterion)) {
                return false;
            }
        }
    } else {
        if (!otherEllipsoid->computeSemiMinorAxis().isEquivalentTo(
                otherEllipsoid->computeSemiMinorAxis(), criterion)) {
            return false;
        }
    }

    if ((semiMedianAxis().has_value() ^
         otherEllipsoid->semiMedianAxis().has_value())) {
        return false;
    }
    if (semiMedianAxis().has_value() &&
        otherEllipsoid->semiMedianAxis().has_value()) {
        if (!semiMedianAxis()->isEquivalentTo(
                *(otherEllipsoid->semiMedianAxis()), criterion)) {
            return false;
        }
    }
    return true;
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

const GeodeticReferenceFrameNNPtr GeodeticReferenceFrame::createEPSG_6267() {
    util::PropertyMap propertiesDatum;
    propertiesDatum
        .set(metadata::Identifier::CODESPACE_KEY, metadata::Identifier::EPSG)
        .set(metadata::Identifier::CODE_KEY, 6267)
        .set(common::IdentifiedObject::NAME_KEY, "North American Datum 1927");

    return create(propertiesDatum, Ellipsoid::CLARKE_1866,
                  util::optional<std::string>(), PrimeMeridian::GREENWICH);
}

// ---------------------------------------------------------------------------

const GeodeticReferenceFrameNNPtr GeodeticReferenceFrame::createEPSG_6269() {
    util::PropertyMap propertiesDatum;
    propertiesDatum
        .set(metadata::Identifier::CODESPACE_KEY, metadata::Identifier::EPSG)
        .set(metadata::Identifier::CODE_KEY, 6269)
        .set(common::IdentifiedObject::NAME_KEY, "North American Datum 1983");

    return create(propertiesDatum, Ellipsoid::GRS1980,
                  util::optional<std::string>(), PrimeMeridian::GREENWICH);
}

// ---------------------------------------------------------------------------

const GeodeticReferenceFrameNNPtr GeodeticReferenceFrame::createEPSG_6326() {
    util::PropertyMap propertiesDatum;
    propertiesDatum
        .set(metadata::Identifier::CODESPACE_KEY, metadata::Identifier::EPSG)
        .set(metadata::Identifier::CODE_KEY, 6326)
        .set(common::IdentifiedObject::NAME_KEY, "World Geodetic System 1984");

    return create(propertiesDatum, Ellipsoid::WGS84,
                  util::optional<std::string>(), PrimeMeridian::GREENWICH);
}

// ---------------------------------------------------------------------------

std::string GeodeticReferenceFrame::exportToWKT(
    io::WKTFormatterNNPtr formatter) const // throw(FormattingException)
{
    const bool isWKT2 = formatter->version() == io::WKTFormatter::Version::WKT2;
    formatter->startNode(io::WKTConstants::DATUM, !identifiers().empty());
    auto l_name = name()->description().has_value() ? *(name()->description())
                                                    : "unnamed";
    if (!isWKT2) {
        l_name = replaceAll(l_name, " ", "_");
        if (l_name == "World_Geodetic_System_1984") {
            l_name = "WGS_1984";
        }
    }
    formatter->addQuotedString(l_name);

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

bool GeodeticReferenceFrame::isEquivalentTo(
    const util::BaseObjectNNPtr &other,
    util::IComparable::Criterion criterion) const {
    auto otherGRF =
        util::nn_dynamic_pointer_cast<GeodeticReferenceFrame>(other);
    if (otherGRF == nullptr || !Datum::_isEquivalentTo(other, criterion)) {
        return false;
    }
    return primeMeridian()->isEquivalentTo(otherGRF->primeMeridian(),
                                           criterion) &&
           ellipsoid()->isEquivalentTo(otherGRF->ellipsoid(), criterion);
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
struct DynamicGeodeticReferenceFrame::Private {
    common::Measure frameReferenceEpoch{};
    util::optional<std::string> deformationModelName{};
};
//! @endcond

// ---------------------------------------------------------------------------

DynamicGeodeticReferenceFrame::DynamicGeodeticReferenceFrame(
    const EllipsoidNNPtr &ellipsoidIn,
    const PrimeMeridianNNPtr &primeMeridianIn,
    const common::Measure &frameReferenceEpochIn,
    const util::optional<std::string> &deformationModelNameIn)
    : GeodeticReferenceFrame(ellipsoidIn, primeMeridianIn),
      d(internal::make_unique<Private>()) {
    d->frameReferenceEpoch = frameReferenceEpochIn;
    d->deformationModelName = deformationModelNameIn;
}

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

/** \brief Return the name of the deformation model.
 *
 * @note This is an extension to the \ref ISO_19111_2018 modeling, to
 * hold the content of the DYNAMIC.MODEL WKT2 node.
 *
 * @return the name of the deformation model.
 */
const util::optional<std::string> &
DynamicGeodeticReferenceFrame::deformationModelName() const {
    return d->deformationModelName;
}

// ---------------------------------------------------------------------------

bool DynamicGeodeticReferenceFrame::isEquivalentTo(
    const util::BaseObjectNNPtr &other,
    util::IComparable::Criterion criterion) const {
    auto otherDGRF =
        util::nn_dynamic_pointer_cast<DynamicGeodeticReferenceFrame>(other);
    if (otherDGRF == nullptr ||
        !GeodeticReferenceFrame::isEquivalentTo(other, criterion)) {
        return false;
    }
    return frameReferenceEpoch().isEquivalentTo(
               otherDGRF->frameReferenceEpoch(), criterion) &&
           metadata::Identifier::isEquivalentName(
               *deformationModelName(), *otherDGRF->deformationModelName());
}

// ---------------------------------------------------------------------------

std::string DynamicGeodeticReferenceFrame::exportToWKT(
    io::WKTFormatterNNPtr formatter) const // throw(FormattingException)
{
    const bool isWKT2 = formatter->version() == io::WKTFormatter::Version::WKT2;
    if (isWKT2 && formatter->use2018Keywords()) {
        formatter->startNode(io::WKTConstants::DYNAMIC, false);
        formatter->startNode(io::WKTConstants::FRAMEEPOCH, false);
        formatter->add(frameReferenceEpoch()
                           .convertToUnit(common::UnitOfMeasure::YEAR)
                           .value());
        formatter->endNode();
        if (deformationModelName().has_value() &&
            !deformationModelName()->empty()) {
            formatter->startNode(io::WKTConstants::MODEL, false);
            formatter->addQuotedString(*deformationModelName());
            formatter->endNode();
        }
        formatter->endNode();
    }
    return GeodeticReferenceFrame::exportToWKT(formatter);
}

// ---------------------------------------------------------------------------

/** \brief Instanciate a DyanmicGeodeticReferenceFrame
 *
 * @param properties See \ref general_properties.
 * At minimum the name should be defined.
 * @param ellipsoid the Ellipsoid.
 * @param anchor the anchor definition, or empty.
 * @param primeMeridian the PrimeMeridian.
 * @param frameReferenceEpochIn the frame reference epoch.
 * @param deformationModelNameIn deformation model name, or empty
 * @return new DyanmicGeodeticReferenceFrame.
 */
DynamicGeodeticReferenceFrameNNPtr DynamicGeodeticReferenceFrame::create(
    const util::PropertyMap &properties, const EllipsoidNNPtr &ellipsoid,
    const util::optional<std::string> &anchor,
    const PrimeMeridianNNPtr &primeMeridian,
    const common::Measure &frameReferenceEpochIn,
    const util::optional<std::string> &deformationModelNameIn) {
    DynamicGeodeticReferenceFrameNNPtr grf(
        DynamicGeodeticReferenceFrame::nn_make_shared<
            DynamicGeodeticReferenceFrame>(ellipsoid, primeMeridian,
                                           frameReferenceEpochIn,
                                           deformationModelNameIn));
    util::nn_static_pointer_cast<Datum>(grf)->d->anchorDefinition = anchor;
    grf->setProperties(properties);
    return grf;
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
struct DatumEnsemble::Private {
    std::vector<DatumNNPtr> datums{};
    metadata::PositionalAccuracyNNPtr positionalAccuracy;

    Private(const std::vector<DatumNNPtr> &datumsIn,
            const metadata::PositionalAccuracyNNPtr &accuracy)
        : datums(datumsIn), positionalAccuracy(accuracy) {}
};
//! @endcond

// ---------------------------------------------------------------------------

DatumEnsemble::DatumEnsemble(const std::vector<DatumNNPtr> &datumsIn,
                             const metadata::PositionalAccuracyNNPtr &accuracy)
    : d(internal::make_unique<Private>(datumsIn, accuracy)) {}

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
const std::vector<DatumNNPtr> &DatumEnsemble::datums() const {
    return d->datums;
}

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

std::string DatumEnsemble::exportToWKT(
    io::WKTFormatterNNPtr formatter) const // throw(FormattingException)
{
    const bool isWKT2 = formatter->version() == io::WKTFormatter::Version::WKT2;
    if (!isWKT2 || !formatter->use2018Keywords()) {
        throw io::FormattingException(
            "DatumEnsemble can only be exported to WKT2:2018");
    }

    auto l_datums = datums();
    assert(!l_datums.empty());

    formatter->startNode(io::WKTConstants::ENSEMBLE, false);
    formatter->addQuotedString(name()->description().has_value()
                                   ? *(name()->description())
                                   : "unnamed");
    for (const auto &datum : l_datums) {
        formatter->startNode(io::WKTConstants::MEMBER,
                             !datum->identifiers().empty());
        formatter->addQuotedString(datum->name()->description().has_value()
                                       ? *(datum->name()->description())
                                       : "unnamed");
        if (formatter->outputId()) {
            datum->formatID(formatter);
        }
        formatter->endNode();
    }

    auto grfFirst = std::dynamic_pointer_cast<GeodeticReferenceFrame>(
        l_datums[0].as_nullable());
    if (grfFirst) {
        grfFirst->ellipsoid()->exportToWKT(formatter);
    }

    formatter->startNode(io::WKTConstants::ENSEMBLEACCURACY, false);
    formatter->add(positionalAccuracy()->value());
    formatter->endNode();
    formatter->endNode();
    return formatter->toString();
}

// ---------------------------------------------------------------------------

/** \brief Instanciate a DatumEnsemble.
 *
 * @param properties See \ref general_properties.
 * At minimum the name should be defined.
 * @param datumsIn Array of at least 2 datums.
 * @param accuracy Accuracy of the datum ensemble
 * @return new DatumEnsemble.
 * @throw util::Exception
 */
DatumEnsembleNNPtr DatumEnsemble::create(
    const util::PropertyMap &properties,
    const std::vector<DatumNNPtr> &datumsIn,
    const metadata::PositionalAccuracyNNPtr &accuracy) // throw(Exception)
{
    if (datumsIn.size() < 2) {
        throw util::Exception("ensemble should have at least 2 datums");
    }
    auto grfFirst = std::dynamic_pointer_cast<GeodeticReferenceFrame>(
        datumsIn[0].as_nullable());
    if (grfFirst) {
        for (size_t i = 1; i < datumsIn.size(); i++) {
            auto grf = std::dynamic_pointer_cast<GeodeticReferenceFrame>(
                datumsIn[i].as_nullable());
            if (!grf) {
                throw util::Exception(
                    "ensemble should have consistent datum types");
            }
            if (!grfFirst->ellipsoid()->isEquivalentTo(grf->ellipsoid())) {
                throw util::Exception(
                    "ensemble should have datums with identical ellipsoid");
            }
            if (!grfFirst->primeMeridian()->isEquivalentTo(
                    grf->primeMeridian())) {
                throw util::Exception(
                    "ensemble should have datums with identical "
                    "prime meridian");
            }
        }
    } else {
        auto vrfFirst = std::dynamic_pointer_cast<VerticalReferenceFrame>(
            datumsIn[0].as_nullable());
        if (vrfFirst) {
            for (size_t i = 1; i < datumsIn.size(); i++) {
                auto vrf = std::dynamic_pointer_cast<VerticalReferenceFrame>(
                    datumsIn[i].as_nullable());
                if (!vrf) {
                    throw util::Exception(
                        "ensemble should have consistent datum types");
                }
            }
        }
    }
    auto ensemble(
        DatumEnsemble::nn_make_shared<DatumEnsemble>(datumsIn, accuracy));
    ensemble->setProperties(properties);
    return ensemble;
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

VerticalReferenceFrame::VerticalReferenceFrame(
    const util::optional<RealizationMethod> &realizationMethodIn)
    : d(internal::make_unique<Private>()) {
    if (realizationMethodIn.has_value() &&
        !realizationMethodIn->toString().empty()) {
        d->realizationMethod_ = *realizationMethodIn;
    }
}

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
    auto rf(VerticalReferenceFrame::nn_make_shared<VerticalReferenceFrame>(
        realizationMethodIn));
    util::nn_static_pointer_cast<Datum>(rf)->d->anchorDefinition = anchor;
    rf->setProperties(properties);
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

bool VerticalReferenceFrame::isEquivalentTo(
    const util::BaseObjectNNPtr &other,
    util::IComparable::Criterion criterion) const {
    auto otherVRF =
        util::nn_dynamic_pointer_cast<VerticalReferenceFrame>(other);
    if (otherVRF == nullptr || !Datum::_isEquivalentTo(other, criterion)) {
        return false;
    }
    if ((realizationMethod().has_value() ^
         otherVRF->realizationMethod().has_value())) {
        return false;
    }
    if (realizationMethod().has_value() &&
        otherVRF->realizationMethod().has_value()) {
        if (*(realizationMethod()) != *(otherVRF->realizationMethod())) {
            return false;
        }
    }
    return true;
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
struct DynamicVerticalReferenceFrame::Private {
    common::Measure frameReferenceEpoch{};
    util::optional<std::string> deformationModelName{};
};
//! @endcond

// ---------------------------------------------------------------------------

DynamicVerticalReferenceFrame::DynamicVerticalReferenceFrame(
    const util::optional<RealizationMethod> &realizationMethodIn,
    const common::Measure &frameReferenceEpochIn,
    const util::optional<std::string> &deformationModelNameIn)
    : VerticalReferenceFrame(realizationMethodIn),
      d(internal::make_unique<Private>()) {
    d->frameReferenceEpoch = frameReferenceEpochIn;
    d->deformationModelName = deformationModelNameIn;
}

// ---------------------------------------------------------------------------

#ifdef notdef
DynamicVerticalReferenceFrame::DynamicVerticalReferenceFrame(
    const DynamicVerticalReferenceFrame &other)
    : VerticalReferenceFrame(other),
      d(internal::make_unique<Private>(*other.d)) {}
#endif

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
DynamicVerticalReferenceFrame::~DynamicVerticalReferenceFrame() = default;
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
DynamicVerticalReferenceFrame::frameReferenceEpoch() const {
    return d->frameReferenceEpoch;
}

// ---------------------------------------------------------------------------

/** \brief Return the name of the deformation model.
 *
 * @note This is an extension to the \ref ISO_19111_2018 modeling, to
 * hold the content of the DYNAMIC.MODEL WKT2 node.
 *
 * @return the name of the deformation model.
 */
const util::optional<std::string> &
DynamicVerticalReferenceFrame::deformationModelName() const {
    return d->deformationModelName;
}

// ---------------------------------------------------------------------------

bool DynamicVerticalReferenceFrame::isEquivalentTo(
    const util::BaseObjectNNPtr &other,
    util::IComparable::Criterion criterion) const {
    auto otherDGRF =
        util::nn_dynamic_pointer_cast<DynamicVerticalReferenceFrame>(other);
    if (otherDGRF == nullptr ||
        !VerticalReferenceFrame::isEquivalentTo(other, criterion)) {
        return false;
    }
    return frameReferenceEpoch().isEquivalentTo(
               otherDGRF->frameReferenceEpoch(), criterion) &&
           metadata::Identifier::isEquivalentName(
               *deformationModelName(), *otherDGRF->deformationModelName());
}

// ---------------------------------------------------------------------------

std::string DynamicVerticalReferenceFrame::exportToWKT(
    io::WKTFormatterNNPtr formatter) const // throw(FormattingException)
{
    const bool isWKT2 = formatter->version() == io::WKTFormatter::Version::WKT2;
    if (isWKT2 && formatter->use2018Keywords()) {
        formatter->startNode(io::WKTConstants::DYNAMIC, false);
        formatter->startNode(io::WKTConstants::FRAMEEPOCH, false);
        formatter->add(frameReferenceEpoch()
                           .convertToUnit(common::UnitOfMeasure::YEAR)
                           .value());
        formatter->endNode();
        if (deformationModelName().has_value() &&
            !deformationModelName()->empty()) {
            formatter->startNode(io::WKTConstants::MODEL, false);
            formatter->addQuotedString(*deformationModelName());
            formatter->endNode();
        }
        formatter->endNode();
    }
    return VerticalReferenceFrame::exportToWKT(formatter);
}

// ---------------------------------------------------------------------------

/** \brief Instanciate a DyanmicVerticalReferenceFrame
 *
 * @param properties See \ref general_properties.
 * At minimum the name should be defined.
 * @param anchor the anchor definition, or empty.
 * @param realizationMethodIn the realization method, or empty.
 * @param frameReferenceEpochIn the frame reference epoch.
 * @param deformationModelNameIn deformation model name, or empty
 * @return new DyanmicVerticalReferenceFrame.
 */
DynamicVerticalReferenceFrameNNPtr DynamicVerticalReferenceFrame::create(
    const util::PropertyMap &properties,
    const util::optional<std::string> &anchor,
    const util::optional<RealizationMethod> &realizationMethodIn,
    const common::Measure &frameReferenceEpochIn,
    const util::optional<std::string> &deformationModelNameIn) {
    DynamicVerticalReferenceFrameNNPtr grf(
        DynamicVerticalReferenceFrame::nn_make_shared<
            DynamicVerticalReferenceFrame>(realizationMethodIn,
                                           frameReferenceEpochIn,
                                           deformationModelNameIn));
    util::nn_static_pointer_cast<Datum>(grf)->d->anchorDefinition = anchor;
    grf->setProperties(properties);
    return grf;
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

// ---------------------------------------------------------------------------

bool TemporalDatum::isEquivalentTo(
    const util::BaseObjectNNPtr &other,
    util::IComparable::Criterion criterion) const {
    auto otherTD = util::nn_dynamic_pointer_cast<TemporalDatum>(other);
    if (otherTD == nullptr || !Datum::_isEquivalentTo(other, criterion)) {
        return false;
    }
    return temporalOrigin().toString() ==
               otherTD->temporalOrigin().toString() &&
           calendar() == otherTD->calendar();
}

} // namespace datum
NS_PROJ_END
