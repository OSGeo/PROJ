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

#include "proj/crs.hpp"
#include "proj/common.hpp"
#include "proj/coordinateoperation.hpp"
#include "proj/coordinatesystem.hpp"
#include "proj/coordinatesystem_internal.hpp"
#include "proj/internal.hpp"
#include "proj/io.hpp"
#include "proj/io_internal.hpp"
#include "proj/util.hpp"

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

GeographicCRSPtr CRS::extractGeographicCRS(CRSNNPtr crs) {
    CRSPtr transformSourceCRS;
    auto geogCRS = util::nn_dynamic_pointer_cast<GeographicCRS>(crs);
    if (geogCRS) {
        return geogCRS;
    }
    auto projCRS = util::nn_dynamic_pointer_cast<ProjectedCRS>(crs);
    if (projCRS) {
        return util::nn_dynamic_pointer_cast<GeographicCRS>(projCRS->baseCRS());
    }
    auto compoundCRS = util::nn_dynamic_pointer_cast<CompoundCRS>(crs);
    if (compoundCRS) {
        for (const auto &subCrs : compoundCRS->componentReferenceSystems()) {
            geogCRS = util::nn_dynamic_pointer_cast<GeographicCRS>(subCrs);
            if (geogCRS) {
                return geogCRS;
            }
        }
    }
    return nullptr;
}

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

#ifdef notdef
SingleCRS::SingleCRS(const SingleCRS &other)
    : // CRS(other),
      d(internal::make_unique<Private>(*other.d)) {}
#endif

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
    std::vector<PointMotionOperationNNPtr> velocityModel{};
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

#ifdef notdef
GeodeticCRS::GeodeticCRS(const GeodeticCRS &other)
    : SingleCRS(other), d(internal::make_unique<Private>(*other.d)) {}
#endif

// ---------------------------------------------------------------------------

GeodeticCRS::~GeodeticCRS() = default;

// ---------------------------------------------------------------------------

const GeodeticReferenceFrameNNPtr GeodeticCRS::datum() const {
    return NN_CHECK_THROW(std::dynamic_pointer_cast<GeodeticReferenceFrame>(
        SingleCRS::getPrivate()->datum));
}

// ---------------------------------------------------------------------------

const std::vector<PointMotionOperationNNPtr> &
GeodeticCRS::velocityModel() const {
    return d->velocityModel;
}

// ---------------------------------------------------------------------------

bool GeodeticCRS::isGeocentric() const {
    return coordinateSystem()->getWKT2Type(WKTFormatter::create()) ==
               "Cartesian" &&
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
                                                 : WKTConstants::GEOGCS,
                         !identifiers().empty());
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
    ObjectUsage::_exportToWKT(formatter);
    formatter->endNode();
    return formatter->toString();
}

// ---------------------------------------------------------------------------

std::string GeodeticCRS::exportToPROJString(
    PROJStringFormatterNNPtr formatter) const // throw(FormattingException)
{
    if (!isGeocentric()) {
        throw FormattingException("GeodeticCRS::exportToPROJString() only "
                                  "supports geocentric coordinate systems");
    }

    formatter->addStep("cart");
    datum()->ellipsoid()->exportToPROJString(formatter);

    auto &axisList = coordinateSystem()->axisList();
    if (!axisList.empty() &&
        axisList[0]->axisUnitID() != UnitOfMeasure::METRE) {
        auto projUnit = axisList[0]->axisUnitID().exportToPROJString();
        if (projUnit.empty()) {
            formatter->addParam("to_meter",
                                axisList[0]->axisUnitID().conversionToSI());
        } else {
            formatter->addParam("units", projUnit);
        }
    }

    return formatter->toString();
}

// ---------------------------------------------------------------------------

void GeodeticCRS::addDatumInfoToPROJString(
    PROJStringFormatterNNPtr formatter) const // throw(FormattingException)
{
    datum()->ellipsoid()->exportToPROJString(formatter);
    datum()->primeMeridian()->exportToPROJString(formatter);
    const auto &TOWGS84Params = formatter->getTOWGS84Parameters();
    if (TOWGS84Params.size() == 7) {
        formatter->addParam("towgs84", TOWGS84Params);
    }
    auto nadgrids = formatter->getHDatumExtension();
    if (!nadgrids.empty()) {
        formatter->addParam("nadgrids", nadgrids);
    }
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

#ifdef notdef
GeographicCRS::GeographicCRS(const GeographicCRS &other)
    : SingleCRS(other), GeodeticCRS(other),
      d(internal::make_unique<Private>(*other.d)) {}
#endif

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
    propertiesCRS.set(Identifier::CODESPACE_KEY, "EPSG")
        .set(Identifier::CODE_KEY, 4326)
        .set(IdentifiedObject::NAME_KEY, "WGS 84");
    return create(
        propertiesCRS, GeodeticReferenceFrame::EPSG_6326,
        EllipsoidalCS::createLatitudeLongitude(UnitOfMeasure::DEGREE));
}

// ---------------------------------------------------------------------------

GeographicCRSNNPtr GeographicCRS::createEPSG_4979() {
    PropertyMap propertiesCRS;
    propertiesCRS.set(Identifier::CODESPACE_KEY, "EPSG")
        .set(Identifier::CODE_KEY, 4979)
        .set(IdentifiedObject::NAME_KEY, "WGS 84");
    return create(propertiesCRS, GeodeticReferenceFrame::EPSG_6326,
                  EllipsoidalCS::createLatitudeLongitudeEllipsoidalHeight(
                      UnitOfMeasure::DEGREE, UnitOfMeasure::METRE));
}

// ---------------------------------------------------------------------------

GeographicCRSNNPtr GeographicCRS::createEPSG_4807() {
    PropertyMap propertiesEllps;
    propertiesEllps.set(Identifier::CODESPACE_KEY, "EPSG")
        .set(Identifier::CODE_KEY, 6807)
        .set(IdentifiedObject::NAME_KEY, "Clarke 1880 (IGN)");
    auto ellps(Ellipsoid::createFlattenedSphere(
        propertiesEllps, Length(6378249.2), Scale(293.4660212936269)));

    auto axisLat(CoordinateSystemAxis::create(
        PropertyMap().set(IdentifiedObject::NAME_KEY, AxisName::Latitude),
        AxisAbbreviation::lat, AxisDirection::NORTH, UnitOfMeasure::GRAD));

    auto axisLong(CoordinateSystemAxis::create(
        PropertyMap().set(IdentifiedObject::NAME_KEY, AxisName::Longitude),
        AxisAbbreviation::lon, AxisDirection::EAST, UnitOfMeasure::GRAD));

    auto cs(EllipsoidalCS::create(PropertyMap(), axisLat, axisLong));

    PropertyMap propertiesDatum;
    propertiesDatum.set(Identifier::CODESPACE_KEY, "EPSG")
        .set(Identifier::CODE_KEY, 6807)
        .set(IdentifiedObject::NAME_KEY,
             "Nouvelle_Triangulation_Francaise_Paris");
    auto datum(GeodeticReferenceFrame::create(
        propertiesDatum, ellps, optional<std::string>(), PrimeMeridian::PARIS));

    PropertyMap propertiesCRS;
    propertiesCRS.set(Identifier::CODESPACE_KEY, "EPSG")
        .set(Identifier::CODE_KEY, 4807)
        .set(IdentifiedObject::NAME_KEY, "NTF (Paris)");
    auto gcrs(create(propertiesCRS, datum, cs));
    return gcrs;
}

// ---------------------------------------------------------------------------

void GeographicCRS::addAngularUnitConvertAndAxisSwap(
    PROJStringFormatterNNPtr formatter) const {
    auto &axisList = coordinateSystem()->axisList();
    if (!axisList.empty() &&
        axisList[0]->axisUnitID() != UnitOfMeasure::DEGREE) {
        formatter->addStep("unitconvert");
        auto projUnit = axisList[0]->axisUnitID().exportToPROJString();
        if (projUnit.empty()) {
            formatter->addParam("xy_in",
                                axisList[0]->axisUnitID().conversionToSI());
        } else {
            formatter->addParam("xy_in", projUnit);
        }
        formatter->addParam("xy_out", "rad");
    }

    if (axisList.size() >= 2 &&
        ci_equal(*(axisList[0]->name()->description()), "Latitude") &&
        ci_equal(*(axisList[1]->name()->description()), "Longitude")) {
        formatter->addStep("axisswap");
        formatter->addParam("order", "2,1");
    }
}

// ---------------------------------------------------------------------------

std::string GeographicCRS::exportToPROJString(
    PROJStringFormatterNNPtr formatter) const // throw(FormattingException)
{
    addAngularUnitConvertAndAxisSwap(formatter);

    formatter->addStep("longlat");
    addDatumInfoToPROJString(formatter);

    return formatter->toString();
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
struct VerticalCRS::Private {
    std::vector<TransformationNNPtr> geoidModel{};
    std::vector<PointMotionOperationNNPtr> velocityModel{};
};
//! @endcond

// ---------------------------------------------------------------------------

VerticalCRS::VerticalCRS(const VerticalReferenceFrameNNPtr &datumIn,
                         const VerticalCSNNPtr &csIn)
    : SingleCRS(datumIn, csIn), d(internal::make_unique<Private>()) {}

// ---------------------------------------------------------------------------

#ifdef notdef
VerticalCRS::VerticalCRS(const VerticalCRS &other)
    : SingleCRS(other), d(internal::make_unique<Private>(*other.d)) {}
#endif

// ---------------------------------------------------------------------------

VerticalCRS::~VerticalCRS() = default;

// ---------------------------------------------------------------------------

const VerticalReferenceFramePtr VerticalCRS::datum() const {
    return std::dynamic_pointer_cast<VerticalReferenceFrame>(
        SingleCRS::getPrivate()->datum);
}

// ---------------------------------------------------------------------------

const std::vector<TransformationNNPtr> &VerticalCRS::geoidModel() const {
    return d->geoidModel;
}

// ---------------------------------------------------------------------------

const std::vector<PointMotionOperationNNPtr> &
VerticalCRS::velocityModel() const {
    return d->velocityModel;
}

// ---------------------------------------------------------------------------

const VerticalCSNNPtr VerticalCRS::coordinateSystem() const {
    return NN_CHECK_THROW(util::nn_dynamic_pointer_cast<VerticalCS>(
        SingleCRS::getPrivate()->coordinateSystem));
}

// ---------------------------------------------------------------------------

std::string VerticalCRS::exportToWKT(WKTFormatterNNPtr formatter) const {
    const bool isWKT2 = formatter->version() == WKTFormatter::Version::WKT2;
    formatter->startNode(isWKT2 ? WKTConstants::VERTCRS : WKTConstants::VERT_CS,
                         !identifiers().empty());
    formatter->addQuotedString(*(name()->description()));
    if (datum()) {
        datum()->exportToWKT(formatter);
    } else {
        throw FormattingException("Missing VDATUM");
    }
    auto &axisList = coordinateSystem()->axisList();
    if (!isWKT2 && !axisList.empty()) {
        axisList[0]->axisUnitID().exportToWKT(formatter);
    }
    coordinateSystem()->exportToWKT(formatter);
    ObjectUsage::_exportToWKT(formatter);
    formatter->endNode();
    return formatter->toString();
}

// ---------------------------------------------------------------------------

std::string VerticalCRS::exportToPROJString(
    PROJStringFormatterNNPtr formatter) const // throw(FormattingException)
{
    auto nadgrids = formatter->getVDatumExtension();
    if (!nadgrids.empty()) {
        formatter->addParam("nadgrids", nadgrids);
    }

    auto &axisList = coordinateSystem()->axisList();
    if (!axisList.empty() &&
        axisList[0]->axisUnitID() != UnitOfMeasure::METRE) {
        auto projUnit = axisList[0]->axisUnitID().exportToPROJString();
        if (projUnit.empty()) {
            formatter->addParam("vto_meter",
                                axisList[0]->axisUnitID().conversionToSI());
        } else {
            formatter->addParam("vunits", projUnit);
        }
    }

    return formatter->toString();
}

// ---------------------------------------------------------------------------

VerticalCRSNNPtr VerticalCRS::create(const PropertyMap &properties,
                                     const VerticalReferenceFrameNNPtr &datumIn,
                                     const VerticalCSNNPtr &csIn) {
    auto crs(VerticalCRS::nn_make_shared<VerticalCRS>(datumIn, csIn));
    crs->setProperties(properties);
    return crs;
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

// DerivedCRS is an abstract class, that virtually inherits from SingleCRS
// Consequently the base constructor in SingleCRS will never be called by
// that constructor. clang -Wabstract-vbase-init and VC++ underline this, but
// other
// compilers will complain if we don't call the base constructor.

DerivedCRS::DerivedCRS(const SingleCRSNNPtr &baseCRSIn,
                       const ConversionNNPtr &derivingConversionIn,
                       const CoordinateSystemNNPtr &
#if !defined(COMPILER_WARNS_ABOUT_ABSTRACT_VBASE_INIT)
                           cs
#endif
                       )
    :
#if !defined(COMPILER_WARNS_ABOUT_ABSTRACT_VBASE_INIT)
      SingleCRS(baseCRSIn->datum(), cs),
#endif
      d(internal::make_unique<Private>(
          baseCRSIn, Conversion::create(derivingConversionIn))) {
}

// ---------------------------------------------------------------------------

#ifdef notdef
DerivedCRS::DerivedCRS(const DerivedCRS &other)
    :
#if !defined(COMPILER_WARNS_ABOUT_ABSTRACT_VBASE_INIT)
      SingleCRS(other),
#endif
      d(internal::make_unique<Private>(*other.d)) {
}
#endif

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

#ifdef notdef
ProjectedCRS::ProjectedCRS(const ProjectedCRS &other)
    : SingleCRS(other), DerivedCRS(other),
      d(internal::make_unique<Private>(*other.d)) {}
#endif

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
    formatter->startNode(isWKT2 ? WKTConstants::PROJCRS : WKTConstants::PROJCS,
                         !identifiers().empty());
    formatter->addQuotedString(*(name()->description()));

    auto &geodeticCRSAxisList = baseCRS()->coordinateSystem()->axisList();

    if (isWKT2) {
        formatter->startNode(
            (formatter->use2018Keywords() &&
             dynamic_cast<const GeographicCRS *>(baseCRS().get()))
                ? WKTConstants::BASEGEOGCRS
                : WKTConstants::BASEGEODCRS,
            !baseCRS()->identifiers().empty());
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
    ObjectUsage::_exportToWKT(formatter);
    formatter->endNode();
    return formatter->toString();
}

// ---------------------------------------------------------------------------

std::string ProjectedCRS::exportToPROJString(
    PROJStringFormatterNNPtr formatter) const // throw(FormattingException)
{
    auto geogCRS = nn_dynamic_pointer_cast<GeographicCRS>(baseCRS());
    if (geogCRS) {
        geogCRS->addAngularUnitConvertAndAxisSwap(formatter);
    }

    derivingConversion()->exportToPROJString(formatter);

    baseCRS()->addDatumInfoToPROJString(formatter);

    auto &axisList = coordinateSystem()->axisList();
    if (!axisList.empty() &&
        axisList[0]->axisUnitID() != UnitOfMeasure::METRE) {
        auto projUnit = axisList[0]->axisUnitID().exportToPROJString();
        if (projUnit.empty()) {
            formatter->addParam("to_meter",
                                axisList[0]->axisUnitID().conversionToSI());
        } else {
            formatter->addParam("units", projUnit);
        }
    }

    if (axisList.size() >= 2 &&
        axisList[0]->axisDirection() == AxisDirection::NORTH &&
        axisList[1]->axisDirection() == AxisDirection::EAST) {
        formatter->addStep("axisswap");
        formatter->addParam("order", "2,1");
    }

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

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
struct CompoundCRS::Private {
    std::vector<CRSNNPtr> components_{};
};
//! @endcond

// ---------------------------------------------------------------------------

CompoundCRS::CompoundCRS(const std::vector<CRSNNPtr> &components)
    : CRS(), d(internal::make_unique<Private>()) {
    d->components_ = components;
}

// ---------------------------------------------------------------------------

CompoundCRS::~CompoundCRS() = default;

// ---------------------------------------------------------------------------

std::vector<SingleCRSNNPtr> CompoundCRS::componentReferenceSystems() const {
    // Flatten the potential hierarchy to return only SingleCRS
    std::vector<SingleCRSNNPtr> res;
    for (const auto &crs : d->components_) {
        auto childCompound = util::nn_dynamic_pointer_cast<CompoundCRS>(crs);
        if (childCompound) {
            auto childFlattened = childCompound->componentReferenceSystems();
            res.insert(res.end(), childFlattened.begin(), childFlattened.end());
        } else {
            auto singleCRS = util::nn_dynamic_pointer_cast<SingleCRS>(crs);
            if (singleCRS) {
                res.push_back(NN_CHECK_ASSERT(singleCRS));
            }
        }
    }
    return res;
}

// ---------------------------------------------------------------------------

CompoundCRSNNPtr CompoundCRS::create(const PropertyMap &properties,
                                     const std::vector<CRSNNPtr> &components) {
    auto compoundCRS(CompoundCRS::nn_make_shared<CompoundCRS>(components));
    compoundCRS->setProperties(properties);
    if (properties.find(IdentifiedObject::NAME_KEY) == properties.end()) {
        std::string name;
        for (const auto &crs : components) {
            if (!name.empty()) {
                name += " + ";
            }
            if (crs->name()->description()) {
                name += *crs->name()->description();
            } else {
                name += "unnamed";
            }
        }
        PropertyMap propertyName;
        propertyName.set(IdentifiedObject::NAME_KEY, name);
        compoundCRS->setProperties(propertyName);
    }

    return compoundCRS;
}

// ---------------------------------------------------------------------------

std::string CompoundCRS::exportToWKT(WKTFormatterNNPtr formatter) const {
    const bool isWKT2 = formatter->version() == WKTFormatter::Version::WKT2;
    formatter->startNode(isWKT2 ? WKTConstants::COMPOUNDCRS
                                : WKTConstants::COMPD_CS,
                         !identifiers().empty());
    formatter->addQuotedString(*(name()->description()));
    if (isWKT2) {
        for (const auto &crs : componentReferenceSystems()) {
            crs->exportToWKT(formatter);
        }
    } else {
        for (const auto &crs : d->components_) {
            crs->exportToWKT(formatter);
        }
    }
    ObjectUsage::_exportToWKT(formatter);
    formatter->endNode();
    return formatter->toString();
}

// ---------------------------------------------------------------------------

std::string CompoundCRS::exportToPROJString(
    PROJStringFormatterNNPtr formatter) const // throw(FormattingException)
{
    for (const auto &crs : componentReferenceSystems()) {
        auto crs_exportable =
            nn_dynamic_pointer_cast<IPROJStringExportable>(crs);
        if (crs_exportable) {
            crs_exportable->exportToPROJString(formatter);
        }
    }

    return formatter->toString();
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
struct BoundCRS::Private {
    CRSNNPtr baseCRS_;
    CRSNNPtr hubCRS_;
    TransformationNNPtr transformation_;

    Private(const CRSNNPtr &baseCRSIn, const CRSNNPtr &hubCRSIn,
            const TransformationNNPtr &transformationIn)
        : baseCRS_(baseCRSIn), hubCRS_(hubCRSIn),
          transformation_(transformationIn) {}
};
//! @endcond

// ---------------------------------------------------------------------------

BoundCRS::BoundCRS(const CRSNNPtr &baseCRSIn, const CRSNNPtr &hubCRSIn,
                   const TransformationNNPtr &transformationIn)
    : d(internal::make_unique<Private>(baseCRSIn, hubCRSIn, transformationIn)) {
}

// ---------------------------------------------------------------------------

BoundCRS::~BoundCRS() = default;

// ---------------------------------------------------------------------------

const CRSNNPtr &BoundCRS::baseCRS() const { return d->baseCRS_; }

// ---------------------------------------------------------------------------

const CRSNNPtr &BoundCRS::hubCRS() const { return d->hubCRS_; }

// ---------------------------------------------------------------------------

const TransformationNNPtr &BoundCRS::transformation() const {
    return d->transformation_;
}

// ---------------------------------------------------------------------------

BoundCRSNNPtr BoundCRS::create(const CRSNNPtr &baseCRSIn,
                               const CRSNNPtr &hubCRSIn,
                               const TransformationNNPtr &transformationIn) {
    return BoundCRS::nn_make_shared<BoundCRS>(baseCRSIn, hubCRSIn,
                                              transformationIn);
}

// ---------------------------------------------------------------------------

BoundCRSNNPtr
BoundCRS::createFromTOWGS84(const CRSNNPtr &baseCRSIn,
                            const std::vector<double> TOWGS84Parameters) {
    return BoundCRS::nn_make_shared<BoundCRS>(
        baseCRSIn, GeographicCRS::EPSG_4326,
        Transformation::createTOWGS84(baseCRSIn, TOWGS84Parameters));
}

// ---------------------------------------------------------------------------

bool BoundCRS::isTOWGS84Compatible() const {
    return nn_dynamic_pointer_cast<GeographicCRS>(hubCRS()) != nullptr &&
           ci_equal(*(hubCRS()->name()->description()), "WGS 84");
}
// ---------------------------------------------------------------------------

std::string BoundCRS::getHDatumPROJ4GRIDS() const {
    if (ci_equal(*(transformation()->method()->name()->description()),
                 "NTv2") &&
        ci_equal(*(hubCRS()->name()->description()), "WGS 84") &&
        transformation()->parameterValues().size() == 1) {
        const auto &opParamvalue =
            nn_dynamic_pointer_cast<OperationParameterValue>(
                transformation()->parameterValues()[0]);
        if (opParamvalue) {
            const auto &paramName =
                *(opParamvalue->parameter()->name()->description());
            const auto &parameterValue = opParamvalue->parameterValue();
            if ((opParamvalue->parameter()->isEPSG(8656) ||
                 ci_equal(paramName,
                          "Latitude and longitude difference file")) &&
                parameterValue->type() == ParameterValue::Type::FILENAME) {
                return parameterValue->valueFile();
            }
        }
    }
    return std::string();
}

// ---------------------------------------------------------------------------

std::string BoundCRS::getVDatumPROJ4GRIDS() const {
    if (nn_dynamic_pointer_cast<VerticalCRS>(baseCRS()) &&
        ci_equal(*(transformation()->method()->name()->description()),
                 "GravityRelatedHeight to Geographic3D") &&
        ci_equal(*(hubCRS()->name()->description()), "WGS 84") &&
        transformation()->parameterValues().size() == 1) {
        const auto &opParamvalue =
            nn_dynamic_pointer_cast<OperationParameterValue>(
                transformation()->parameterValues()[0]);
        if (opParamvalue) {
            const auto &paramName =
                *(opParamvalue->parameter()->name()->description());
            const auto &parameterValue = opParamvalue->parameterValue();
            if ((opParamvalue->parameter()->isEPSG(8666) ||
                 ci_equal(paramName, "Geoid (height correction) model file")) &&
                parameterValue->type() == ParameterValue::Type::FILENAME) {
                return parameterValue->valueFile();
            }
        }
    }
    return std::string();
}

// ---------------------------------------------------------------------------

std::string BoundCRS::exportToWKT(WKTFormatterNNPtr formatter) const {
    const bool isWKT2 = formatter->version() == WKTFormatter::Version::WKT2;
    if (isWKT2) {
        formatter->startNode(WKTConstants::BOUNDCRS, false);
        formatter->startNode(WKTConstants::SOURCECRS, false);
        baseCRS()->exportToWKT(formatter);
        formatter->endNode();
        formatter->startNode(WKTConstants::TARGETCRS, false);
        hubCRS()->exportToWKT(formatter);
        formatter->endNode();
        formatter->setAbridgedTransformation(true);
        transformation()->exportToWKT(formatter);
        formatter->setAbridgedTransformation(false);
        formatter->endNode();
    } else {

        auto vdatumProj4GridName = getVDatumPROJ4GRIDS();
        if (!vdatumProj4GridName.empty()) {
            formatter->setVDatumExtension(vdatumProj4GridName);
            baseCRS()->exportToWKT(formatter);
            formatter->setVDatumExtension(std::string());
            return formatter->toString();
        }

        auto hdatumProj4GridName = getHDatumPROJ4GRIDS();
        if (!hdatumProj4GridName.empty()) {
            formatter->setHDatumExtension(hdatumProj4GridName);
            baseCRS()->exportToWKT(formatter);
            formatter->setHDatumExtension(std::string());
            return formatter->toString();
        }

        if (!isTOWGS84Compatible()) {
            throw FormattingException(
                "Cannot export BoundCRS with non-WGS 84 hub CRS in WKT1");
        }
        auto params = transformation()->getTOWGS84Parameters();
        formatter->setTOWGS84Parameters(params);
        baseCRS()->exportToWKT(formatter);
        formatter->setTOWGS84Parameters(std::vector<double>());
    }
    return formatter->toString();
}

// ---------------------------------------------------------------------------

std::string BoundCRS::exportToPROJString(
    PROJStringFormatterNNPtr formatter) const // throw(FormattingException)
{
    auto crs_exportable =
        nn_dynamic_pointer_cast<IPROJStringExportable>(baseCRS());
    if (!crs_exportable) {
        throw FormattingException(
            "baseCRS of BoundCRS cannot be exported as a PROJ string");
    }

    auto vdatumProj4GridName = getVDatumPROJ4GRIDS();
    if (!vdatumProj4GridName.empty()) {
        formatter->setVDatumExtension(vdatumProj4GridName);
        crs_exportable->exportToPROJString(formatter);
        formatter->setVDatumExtension(std::string());
    } else {
        auto hdatumProj4GridName = getHDatumPROJ4GRIDS();
        if (!hdatumProj4GridName.empty()) {
            formatter->setHDatumExtension(hdatumProj4GridName);
            crs_exportable->exportToPROJString(formatter);
            formatter->setHDatumExtension(std::string());
        } else {
            if (isTOWGS84Compatible()) {
                auto params = transformation()->getTOWGS84Parameters();
                formatter->setTOWGS84Parameters(params);
            }
            crs_exportable->exportToPROJString(formatter);
            formatter->setTOWGS84Parameters(std::vector<double>());
        }
    }

    return formatter->toString();
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
struct DerivedGeodeticCRS::Private {};
//! @endcond

// ---------------------------------------------------------------------------

DerivedGeodeticCRS::~DerivedGeodeticCRS() = default;

// ---------------------------------------------------------------------------

DerivedGeodeticCRS::DerivedGeodeticCRS(
    const GeodeticCRSNNPtr &baseCRSIn,
    const ConversionNNPtr &derivingConversionIn, const CartesianCSNNPtr &csIn)
    : SingleCRS(baseCRSIn->datum(), csIn),
      GeodeticCRS(baseCRSIn->datum(), csIn),
      DerivedCRS(baseCRSIn, derivingConversionIn, csIn),
      d(internal::make_unique<Private>()) {}

// ---------------------------------------------------------------------------

DerivedGeodeticCRS::DerivedGeodeticCRS(
    const GeodeticCRSNNPtr &baseCRSIn,
    const ConversionNNPtr &derivingConversionIn, const SphericalCSNNPtr &csIn)
    : SingleCRS(baseCRSIn->datum(), csIn),
      GeodeticCRS(baseCRSIn->datum(), csIn),
      DerivedCRS(baseCRSIn, derivingConversionIn, csIn),
      d(internal::make_unique<Private>()) {}

// ---------------------------------------------------------------------------

const GeodeticCRSNNPtr DerivedGeodeticCRS::baseCRS() const {
    return NN_CHECK_ASSERT(util::nn_dynamic_pointer_cast<GeodeticCRS>(
        DerivedCRS::getPrivate()->baseCRS_));
}

// ---------------------------------------------------------------------------

DerivedGeodeticCRSNNPtr DerivedGeodeticCRS::create(
    const PropertyMap &properties, const GeodeticCRSNNPtr &baseCRSIn,
    const ConversionNNPtr &derivingConversionIn, const CartesianCSNNPtr &csIn) {
    auto crs(DerivedGeodeticCRS::nn_make_shared<DerivedGeodeticCRS>(
        baseCRSIn, derivingConversionIn, csIn));
    crs->setProperties(properties);
    return crs;
}

// ---------------------------------------------------------------------------

DerivedGeodeticCRSNNPtr DerivedGeodeticCRS::create(
    const PropertyMap &properties, const GeodeticCRSNNPtr &baseCRSIn,
    const ConversionNNPtr &derivingConversionIn, const SphericalCSNNPtr &csIn) {
    auto crs(DerivedGeodeticCRS::nn_make_shared<DerivedGeodeticCRS>(
        baseCRSIn, derivingConversionIn, csIn));
    crs->setProperties(properties);
    return crs;
}

// ---------------------------------------------------------------------------

std::string DerivedGeodeticCRS::exportToWKT(WKTFormatterNNPtr formatter) const {
    const bool isWKT2 = formatter->version() == WKTFormatter::Version::WKT2;
    if (!isWKT2) {
        throw FormattingException(
            "DerivedGeodeticCRS can only be exported to WKT2");
    }
    formatter->startNode(WKTConstants::GEODCRS, !identifiers().empty());
    formatter->addQuotedString(*(name()->description()));

    formatter->startNode((formatter->use2018Keywords() &&
                          dynamic_cast<const GeographicCRS *>(baseCRS().get()))
                             ? WKTConstants::BASEGEOGCRS
                             : WKTConstants::BASEGEODCRS,
                         !baseCRS()->identifiers().empty());
    formatter->addQuotedString(*(baseCRS()->name()->description()));
    baseCRS()->datum()->exportToWKT(formatter);
    baseCRS()->datum()->primeMeridian()->exportToWKT(formatter);
    formatter->endNode();

    formatter->setUseDerivingConversion(true);
    derivingConversion()->exportToWKT(formatter);
    formatter->setUseDerivingConversion(false);

    coordinateSystem()->exportToWKT(formatter);
    ObjectUsage::_exportToWKT(formatter);
    formatter->endNode();
    return formatter->toString();
}

// ---------------------------------------------------------------------------

std::string DerivedGeodeticCRS::exportToPROJString(
    PROJStringFormatterNNPtr) const // throw(FormattingException)
{
    throw FormattingException(
        "DerivedGeodeticCRS::exportToPROJString() unimplemented");
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
struct DerivedGeographicCRS::Private {};
//! @endcond

// ---------------------------------------------------------------------------

DerivedGeographicCRS::~DerivedGeographicCRS() = default;

// ---------------------------------------------------------------------------

DerivedGeographicCRS::DerivedGeographicCRS(
    const GeodeticCRSNNPtr &baseCRSIn,
    const ConversionNNPtr &derivingConversionIn, const EllipsoidalCSNNPtr &csIn)
    : SingleCRS(baseCRSIn->datum(), csIn),
      GeographicCRS(baseCRSIn->datum(), csIn),
      DerivedCRS(baseCRSIn, derivingConversionIn, csIn),
      d(internal::make_unique<Private>()) {}

// ---------------------------------------------------------------------------

const GeodeticCRSNNPtr DerivedGeographicCRS::baseCRS() const {
    return NN_CHECK_ASSERT(util::nn_dynamic_pointer_cast<GeodeticCRS>(
        DerivedCRS::getPrivate()->baseCRS_));
}

// ---------------------------------------------------------------------------

DerivedGeographicCRSNNPtr
DerivedGeographicCRS::create(const PropertyMap &properties,
                             const GeodeticCRSNNPtr &baseCRSIn,
                             const ConversionNNPtr &derivingConversionIn,
                             const EllipsoidalCSNNPtr &csIn) {
    auto crs(DerivedGeographicCRS::nn_make_shared<DerivedGeographicCRS>(
        baseCRSIn, derivingConversionIn, csIn));
    crs->setProperties(properties);
    return crs;
}

// ---------------------------------------------------------------------------

std::string
DerivedGeographicCRS::exportToWKT(WKTFormatterNNPtr formatter) const {
    const bool isWKT2 = formatter->version() == WKTFormatter::Version::WKT2;
    if (!isWKT2) {
        throw FormattingException(
            "DerivedGeographicCRS can only be exported to WKT2");
    }
    formatter->startNode(formatter->use2018Keywords() ? WKTConstants::GEOGCRS
                                                      : WKTConstants::GEODCRS,
                         !identifiers().empty());
    formatter->addQuotedString(*(name()->description()));

    formatter->startNode((formatter->use2018Keywords() &&
                          dynamic_cast<const GeographicCRS *>(baseCRS().get()))
                             ? WKTConstants::BASEGEOGCRS
                             : WKTConstants::BASEGEODCRS,
                         !baseCRS()->identifiers().empty());
    formatter->addQuotedString(*(baseCRS()->name()->description()));
    baseCRS()->datum()->exportToWKT(formatter);
    baseCRS()->datum()->primeMeridian()->exportToWKT(formatter);
    formatter->endNode();

    formatter->setUseDerivingConversion(true);
    derivingConversion()->exportToWKT(formatter);
    formatter->setUseDerivingConversion(false);

    coordinateSystem()->exportToWKT(formatter);
    ObjectUsage::_exportToWKT(formatter);
    formatter->endNode();
    return formatter->toString();
}

// ---------------------------------------------------------------------------

std::string DerivedGeographicCRS::exportToPROJString(
    PROJStringFormatterNNPtr) const // throw(FormattingException)
{
    throw FormattingException(
        "DerivedGeographicCRS::exportToPROJString() unimplemented");
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
struct TemporalCRS::Private {};
//! @endcond

// ---------------------------------------------------------------------------

TemporalCRS::~TemporalCRS() = default;

// ---------------------------------------------------------------------------

TemporalCRS::TemporalCRS(const TemporalDatumNNPtr &datumIn,
                         const TemporalCSNNPtr &csIn)
    : SingleCRS(datumIn, csIn), d(internal::make_unique<Private>()) {}

// ---------------------------------------------------------------------------

const TemporalDatumNNPtr TemporalCRS::datum() const {
    return NN_CHECK_ASSERT(std::dynamic_pointer_cast<TemporalDatum>(
        SingleCRS::getPrivate()->datum));
}

// ---------------------------------------------------------------------------

const TemporalCSNNPtr TemporalCRS::coordinateSystem() const {
    return NN_CHECK_ASSERT(util::nn_dynamic_pointer_cast<TemporalCS>(
        SingleCRS::getPrivate()->coordinateSystem));
}

// ---------------------------------------------------------------------------

TemporalCRSNNPtr TemporalCRS::create(const PropertyMap &properties,
                                     const TemporalDatumNNPtr &datumIn,
                                     const TemporalCSNNPtr &csIn) {
    auto crs(TemporalCRS::nn_make_shared<TemporalCRS>(datumIn, csIn));
    crs->setProperties(properties);
    return crs;
}

// ---------------------------------------------------------------------------

std::string TemporalCRS::exportToWKT(WKTFormatterNNPtr formatter) const {
    const bool isWKT2 = formatter->version() == WKTFormatter::Version::WKT2;
    if (!isWKT2) {
        throw FormattingException("TemporalCRS can only be exported to WKT2");
    }
    formatter->startNode(WKTConstants::TIMECRS, !identifiers().empty());
    formatter->addQuotedString(*(name()->description()));
    datum()->exportToWKT(formatter);
    coordinateSystem()->exportToWKT(formatter);
    ObjectUsage::_exportToWKT(formatter);
    formatter->endNode();
    return formatter->toString();
}
