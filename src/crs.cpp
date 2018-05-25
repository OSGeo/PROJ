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

#include "proj/crs.hh"
#include "proj/common.hh"
#include "proj/coordinateoperation.hh"
#include "proj/coordinatesystem.hh"
#include "proj/internal.hh"
#include "proj/io.hh"
#include "proj/io_internal.hh"
#include "proj/util.hh"

#include <memory>
#include <string>
#include <vector>

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

CRS::CRS() = default;

// ---------------------------------------------------------------------------

CRS::~CRS() = default;

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
struct SingleCRS::Private {
    DatumPtr datum{};
    DatumEnsemblePtr datumEnsemble{};
    CoordinateSystemNNPtr coordinateSystem;

    Private(const DatumNNPtr &datumIn, const CoordinateSystemNNPtr &csIn)
        : datum(datumIn), coordinateSystem(csIn) {}
    Private(const DatumPtr &datumIn, const CoordinateSystemNNPtr &csIn)
        : datum(datumIn), coordinateSystem(csIn) {}
};
//! @endcond

// ---------------------------------------------------------------------------

SingleCRS::SingleCRS(const DatumNNPtr &datumIn,
                     const CoordinateSystemNNPtr &csIn)
    : d(internal::make_unique<Private>(datumIn, csIn)) {}

// ---------------------------------------------------------------------------

SingleCRS::SingleCRS(const DatumPtr &datumIn, const CoordinateSystemNNPtr &csIn)
    : d(internal::make_unique<Private>(datumIn, csIn)) {}

// ---------------------------------------------------------------------------

SingleCRS::SingleCRS(const SingleCRS &other)
    : // CRS(other),
      d(internal::make_unique<Private>(*other.d)) {}

// ---------------------------------------------------------------------------

SingleCRS::~SingleCRS() = default;

// ---------------------------------------------------------------------------

const DatumPtr &SingleCRS::datum() const { return d->datum; }

// ---------------------------------------------------------------------------

const DatumEnsemblePtr &SingleCRS::datumEnsemble() const {
    return d->datumEnsemble;
}

// ---------------------------------------------------------------------------

const CoordinateSystemNNPtr &SingleCRS::coordinateSystem() const {
    return d->coordinateSystem;
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
struct GeodeticCRS::Private {
    std::vector<PointMotionOperation> velocityModel{};
};
//! @endcond

// ---------------------------------------------------------------------------

GeodeticCRS::GeodeticCRS(const GeodeticReferenceFrameNNPtr &datumIn,
                         const EllipsoidalCSNNPtr &csIn)
    : SingleCRS(datumIn, csIn), d(internal::make_unique<Private>()) {}

// ---------------------------------------------------------------------------

GeodeticCRS::GeodeticCRS(const GeodeticReferenceFrameNNPtr &datumIn,
                         const SphericalCSNNPtr &csIn)
    : SingleCRS(datumIn, csIn), d(internal::make_unique<Private>()) {}

// ---------------------------------------------------------------------------

GeodeticCRS::GeodeticCRS(const GeodeticReferenceFrameNNPtr &datumIn,
                         const CartesianCSNNPtr &csIn)
    : SingleCRS(datumIn, csIn), d(internal::make_unique<Private>()) {}

// ---------------------------------------------------------------------------

GeodeticCRS::GeodeticCRS(const GeodeticCRS &other)
    : SingleCRS(other), d(internal::make_unique<Private>(*other.d)) {}

// ---------------------------------------------------------------------------

GeodeticCRS::~GeodeticCRS() = default;

// ---------------------------------------------------------------------------

const GeodeticReferenceFrameNNPtr GeodeticCRS::datum() const {
    return NN_CHECK_THROW(std::dynamic_pointer_cast<GeodeticReferenceFrame>(
        SingleCRS::getPrivate()->datum));
}

// ---------------------------------------------------------------------------

const std::vector<PointMotionOperation> &GeodeticCRS::velocityModel() const {
    return d->velocityModel;
}

// ---------------------------------------------------------------------------

bool GeodeticCRS::isGeocentric() const {
    return coordinateSystem()->getWKT2Type() == "Cartesian" &&
           coordinateSystem()->axisList().size() == 3 &&
           coordinateSystem()->axisList()[0]->axisDirection() ==
               AxisDirection::GEOCENTRIC_X &&
           coordinateSystem()->axisList()[1]->axisDirection() ==
               AxisDirection::GEOCENTRIC_Y &&
           coordinateSystem()->axisList()[2]->axisDirection() ==
               AxisDirection::GEOCENTRIC_Z;
}

// ---------------------------------------------------------------------------

GeodeticCRSNNPtr GeodeticCRS::create(const PropertyMap &properties,
                                     const GeodeticReferenceFrameNNPtr &datum,
                                     const EllipsoidalCSNNPtr &cs) {
    auto crs(GeodeticCRS::nn_make_shared<GeodeticCRS>(datum, cs));
    crs->setProperties(properties);
    return crs;
}

// ---------------------------------------------------------------------------

GeodeticCRSNNPtr GeodeticCRS::create(const PropertyMap &properties,
                                     const GeodeticReferenceFrameNNPtr &datum,
                                     const SphericalCSNNPtr &cs) {
    auto crs(GeodeticCRS::nn_make_shared<GeodeticCRS>(datum, cs));
    crs->setProperties(properties);
    return crs;
}

// ---------------------------------------------------------------------------

GeodeticCRSNNPtr GeodeticCRS::create(const PropertyMap &properties,
                                     const GeodeticReferenceFrameNNPtr &datum,
                                     const CartesianCSNNPtr &cs) {
    auto crs(GeodeticCRS::nn_make_shared<GeodeticCRS>(datum, cs));
    crs->setProperties(properties);
    return crs;
}

// ---------------------------------------------------------------------------

std::string GeodeticCRS::exportToWKT(WKTFormatterNNPtr formatter) const {
    const bool isWKT2 = formatter->version() == WKTFormatter::Version::WKT2;
    formatter->startNode(isWKT2 ? ((formatter->use2018Keywords() &&
                                    dynamic_cast<const GeographicCRS *>(this))
                                       ? WKTConstants::GEOGCRS
                                       : WKTConstants::GEODCRS)
                                : isGeocentric() ? WKTConstants::GEOCCS
                                                 : WKTConstants::GEOGCS);
    formatter->addQuotedString(*(name()->description()));
    auto &axisList = coordinateSystem()->axisList();
    if (!axisList.empty()) {
        formatter->pushAxisAngularUnit(
            util::nn_make_shared<UnitOfMeasure>(axisList[0]->axisUnitID()));
    }
    datum()->exportToWKT(formatter);
    datum()->primeMeridian()->exportToWKT(formatter);
    if (!axisList.empty()) {
        formatter->popAxisAngularUnit();
    }
    if (!isWKT2) {
        if (!axisList.empty()) {
            axisList[0]->axisUnitID().exportToWKT(formatter);
        }
    }
    coordinateSystem()->exportToWKT(formatter);
    if (formatter->outputId()) {
        formatID(formatter);
    }
    if (isWKT2) {
        formatRemarks(formatter);
    }
    formatter->endNode();
    return formatter->toString();
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
struct GeographicCRS::Private {};
//! @endcond

// ---------------------------------------------------------------------------

GeographicCRS::GeographicCRS(const GeodeticReferenceFrameNNPtr &datumIn,
                             const EllipsoidalCSNNPtr &csIn)
    : SingleCRS(datumIn, csIn), GeodeticCRS(datumIn, csIn),
      d(internal::make_unique<Private>()) {}

// ---------------------------------------------------------------------------

GeographicCRS::GeographicCRS(const GeographicCRS &other)
    : SingleCRS(other), GeodeticCRS(other),
      d(internal::make_unique<Private>(*other.d)) {}

// ---------------------------------------------------------------------------

GeographicCRS::~GeographicCRS() = default;

// ---------------------------------------------------------------------------

const EllipsoidalCSNNPtr GeographicCRS::coordinateSystem() const {
    return NN_CHECK_THROW(util::nn_dynamic_pointer_cast<EllipsoidalCS>(
        SingleCRS::getPrivate()->coordinateSystem));
}

// ---------------------------------------------------------------------------

GeographicCRSNNPtr
GeographicCRS::create(const PropertyMap &properties,
                      const GeodeticReferenceFrameNNPtr &datum,
                      const EllipsoidalCSNNPtr &cs) {
    GeographicCRSNNPtr gcrs(
        GeographicCRS::nn_make_shared<GeographicCRS>(datum, cs));
    gcrs->setProperties(properties);
    return gcrs;
}

// ---------------------------------------------------------------------------

GeographicCRSNNPtr GeographicCRS::createEPSG_4326() {
    PropertyMap propertiesCRS;
    propertiesCRS.set(Identifier::AUTHORITY_KEY, "EPSG")
        .set(Identifier::CODE_KEY, 4326)
        .set(Identifier::DESCRIPTION_KEY, "WGS 84");
    return create(propertiesCRS, GeodeticReferenceFrame::EPSG_6326,
                  EllipsoidalCS::createLatitudeLongitudeDegree());
}

// ---------------------------------------------------------------------------

GeographicCRSNNPtr GeographicCRS::createEPSG_4979() {
    PropertyMap propertiesCRS;
    propertiesCRS.set(Identifier::AUTHORITY_KEY, "EPSG")
        .set(Identifier::CODE_KEY, 4979)
        .set(Identifier::DESCRIPTION_KEY, "WGS 84");
    return create(
        propertiesCRS, GeodeticReferenceFrame::EPSG_6326,
        EllipsoidalCS::createLatitudeLongitudeDegreeEllipsoidalHeightMetre());
}

// ---------------------------------------------------------------------------

GeographicCRSNNPtr GeographicCRS::createEPSG_4807() {
    PropertyMap propertiesEllps;
    propertiesEllps.set(Identifier::AUTHORITY_KEY, "EPSG")
        .set(Identifier::CODE_KEY, 6807)
        .set(Identifier::DESCRIPTION_KEY, "Clarke 1880 (IGN)");
    auto ellps(Ellipsoid::createFlattenedSphere(
        propertiesEllps, Length(6378249.2), Scale(293.4660212936269)));

    auto pm(
        PrimeMeridian::create(PropertyMap()
                                  .set(Identifier::AUTHORITY_KEY, "EPSG")
                                  .set(Identifier::CODE_KEY, 8903)
                                  .set(Identifier::DESCRIPTION_KEY, "Paris"),
                              Angle(2.5969213, UnitOfMeasure::GRAD)));

    auto axisLat(CoordinateSystemAxis::create(
        PropertyMap(), "latitude", AxisDirection::NORTH, UnitOfMeasure::GRAD));

    auto axisLong(CoordinateSystemAxis::create(
        PropertyMap(), "longitude", AxisDirection::EAST, UnitOfMeasure::GRAD));

    auto cs(EllipsoidalCS::create(PropertyMap(), axisLat, axisLong));

    PropertyMap propertiesDatum;
    propertiesDatum.set(Identifier::AUTHORITY_KEY, "EPSG")
        .set(Identifier::CODE_KEY, 6807)
        .set(Identifier::DESCRIPTION_KEY,
             "Nouvelle_Triangulation_Francaise_Paris");
    auto datum(GeodeticReferenceFrame::create(propertiesDatum, ellps,
                                              optional<std::string>(), pm));

    PropertyMap propertiesCRS;
    propertiesCRS.set(Identifier::AUTHORITY_KEY, "EPSG")
        .set(Identifier::CODE_KEY, 4807)
        .set(Identifier::DESCRIPTION_KEY, "NTF (Paris)");
    auto gcrs(create(propertiesCRS, datum, cs));
    return gcrs;
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
struct VerticalCRS::Private {
    std::vector<Transformation> geoidModel{};
    std::vector<PointMotionOperation> velocityModel{};
};
//! @endcond

// ---------------------------------------------------------------------------

VerticalCRS::VerticalCRS(const VerticalCRS &other)
    : SingleCRS(other), d(internal::make_unique<Private>(*other.d)) {}

// ---------------------------------------------------------------------------

VerticalCRS::~VerticalCRS() = default;

// ---------------------------------------------------------------------------

const VerticalReferenceFramePtr VerticalCRS::datum() const {
    return std::dynamic_pointer_cast<VerticalReferenceFrame>(
        SingleCRS::getPrivate()->datum);
}

// ---------------------------------------------------------------------------

const std::vector<Transformation> &VerticalCRS::geoidModel() const {
    return d->geoidModel;
}

// ---------------------------------------------------------------------------

const std::vector<PointMotionOperation> &VerticalCRS::velocityModel() const {
    return d->velocityModel;
}

// ---------------------------------------------------------------------------

const VerticalCSNNPtr VerticalCRS::coordinateSystem() const {
    return NN_CHECK_THROW(util::nn_dynamic_pointer_cast<VerticalCS>(
        SingleCRS::getPrivate()->coordinateSystem));
}

// ---------------------------------------------------------------------------

std::string VerticalCRS::exportToWKT(WKTFormatterNNPtr /*formatter*/) const {
    throw FormattingException("Not implemented");
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
struct DerivedCRS::Private {
    SingleCRSNNPtr baseCRS_;
    ConversionNNPtr derivingConversion_;

    Private(const SingleCRSNNPtr &baseCRSIn,
            const operation::ConversionNNPtr &derivingConversionIn)
        : baseCRS_(baseCRSIn), derivingConversion_(derivingConversionIn) {}
};

//! @endcond

// ---------------------------------------------------------------------------

DerivedCRS::DerivedCRS(const SingleCRSNNPtr &baseCRSIn,
                       const ConversionNNPtr &derivingConversionIn,
                       const CoordinateSystemNNPtr &cs)
    : SingleCRS(baseCRSIn->datum(), cs),
      d(internal::make_unique<Private>(
          baseCRSIn, Conversion::create(derivingConversionIn))) {}

// ---------------------------------------------------------------------------

DerivedCRS::DerivedCRS(const DerivedCRS &other)
    : SingleCRS(other), d(internal::make_unique<Private>(*other.d)) {}

// ---------------------------------------------------------------------------

DerivedCRS::~DerivedCRS() = default;

// ---------------------------------------------------------------------------

const SingleCRSNNPtr &DerivedCRS::baseCRS() const { return d->baseCRS_; }

// ---------------------------------------------------------------------------

const ConversionNNPtr &DerivedCRS::derivingConversion() const {
    return d->derivingConversion_;
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
struct ProjectedCRS::Private {};
//! @endcond

// ---------------------------------------------------------------------------

ProjectedCRS::ProjectedCRS(const GeodeticCRSNNPtr &baseCRSIn,
                           const ConversionNNPtr &derivingConversionIn,
                           const CartesianCSNNPtr &csIn)
    : SingleCRS(baseCRSIn->datum(), csIn),
      DerivedCRS(baseCRSIn, derivingConversionIn, csIn),
      d(internal::make_unique<Private>()) {}

// ---------------------------------------------------------------------------

ProjectedCRS::ProjectedCRS(const ProjectedCRS &other)
    : SingleCRS(other), DerivedCRS(other),
      d(internal::make_unique<Private>(*other.d)) {}

// ---------------------------------------------------------------------------

ProjectedCRS::~ProjectedCRS() = default;

// ---------------------------------------------------------------------------

const GeodeticCRSNNPtr ProjectedCRS::baseCRS() const {
    return NN_CHECK_ASSERT(util::nn_dynamic_pointer_cast<GeodeticCRS>(
        DerivedCRS::getPrivate()->baseCRS_));
}

// ---------------------------------------------------------------------------

const CartesianCSNNPtr ProjectedCRS::coordinateSystem() const {
    return NN_CHECK_THROW(util::nn_dynamic_pointer_cast<CartesianCS>(
        SingleCRS::getPrivate()->coordinateSystem));
}

// ---------------------------------------------------------------------------

std::string ProjectedCRS::exportToWKT(WKTFormatterNNPtr formatter) const {
    const bool isWKT2 = formatter->version() == WKTFormatter::Version::WKT2;
    formatter->startNode(isWKT2 ? WKTConstants::PROJCRS : WKTConstants::PROJCS);
    formatter->addQuotedString(*(name()->description()));

    auto &geodeticCRSAxisList = baseCRS()->coordinateSystem()->axisList();

    if (isWKT2) {
        formatter->startNode(
            (formatter->use2018Keywords() &&
             dynamic_cast<const GeographicCRS *>(baseCRS().get()))
                ? WKTConstants::BASEGEOGCRS
                : WKTConstants::BASEGEODCRS);
        formatter->addQuotedString(*(baseCRS()->name()->description()));
        baseCRS()->datum()->exportToWKT(formatter);
        // insert ellipsoidal cs unit when the units of the map
        // projection angular parameters are not explicitly given within those
        // parameters. See
        // http://docs.opengeospatial.org/is/12-063r5/12-063r5.html#61
        if (formatter->primeMeridianOrParameterUnitOmittedIfSameAsAxis() &&
            !geodeticCRSAxisList.empty()) {
            geodeticCRSAxisList[0]->axisUnitID().exportToWKT(formatter);
        }
        baseCRS()->datum()->primeMeridian()->exportToWKT(formatter);
        formatter->endNode();
    } else {
        baseCRS()->exportToWKT(formatter);
    }

    if (!isWKT2) {
        formatter->setOutputConversionNode(false);
    }

    auto &axisList = coordinateSystem()->axisList();
    if (!axisList.empty()) {
        formatter->pushAxisLinearUnit(
            util::nn_make_shared<UnitOfMeasure>(axisList[0]->axisUnitID()));
    }
    if (!geodeticCRSAxisList.empty()) {
        formatter->pushAxisAngularUnit(util::nn_make_shared<UnitOfMeasure>(
            geodeticCRSAxisList[0]->axisUnitID()));
    }

    derivingConversion()->exportToWKT(formatter);

    if (!geodeticCRSAxisList.empty()) {
        formatter->popAxisAngularUnit();
    }
    if (!axisList.empty()) {
        formatter->popAxisLinearUnit();
    }

    if (!isWKT2) {
        formatter->setOutputConversionNode(true);
        if (!axisList.empty()) {
            axisList[0]->axisUnitID().exportToWKT(formatter);
        }
    }

    coordinateSystem()->exportToWKT(formatter);
    if (formatter->outputId()) {
        formatID(formatter);
    }
    if (isWKT2) {
        formatRemarks(formatter);
    }
    formatter->endNode();
    return formatter->toString();
}

// ---------------------------------------------------------------------------

ProjectedCRSNNPtr ProjectedCRS::create(
    const PropertyMap &properties, const GeodeticCRSNNPtr &baseCRSIn,
    const ConversionNNPtr &derivingConversionIn, const CartesianCSNNPtr &csIn) {
    auto crs = ProjectedCRS::nn_make_shared<ProjectedCRS>(
        baseCRSIn, derivingConversionIn, csIn);
    crs->setProperties(properties);
    crs->derivingConversion()->setWeakSourceTargetCRS(
        static_cast<const GeodeticCRSPtr>(baseCRSIn),
        static_cast<const ProjectedCRSPtr>(crs));
    return crs;
}
