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

#include <cassert>
#include <memory>
#include <string>
#include <vector>

using namespace NS_PROJ::internal;

NS_PROJ_START
namespace crs {

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
struct CRS::Private {};
//! @endcond

// ---------------------------------------------------------------------------

CRS::CRS() : d(internal::make_unique<Private>()) {}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
CRS::~CRS() = default;
//! @endcond

// ---------------------------------------------------------------------------

/** \brief Return the GeographicCRS of the CRS.
 *
 * Returns the GeographicCRS contained in a CRS. This works currently with
 * input parameters of type GeographicCRS or derived, ProjectedCRS,
 * CompoundCRS or BoundCRS.
 *
 * @return a GeographicCRSPtr, that might be null.
 */
GeographicCRSPtr CRS::extractGeographicCRS() const {
    CRSPtr transformSourceCRS;
    auto geogCRS = dynamic_cast<const GeographicCRS *>(this);
    if (geogCRS) {
        return std::dynamic_pointer_cast<GeographicCRS>(
            shared_from_this().as_nullable());
    }
    auto projCRS = dynamic_cast<const ProjectedCRS *>(this);
    if (projCRS) {
        return projCRS->baseCRS()->extractGeographicCRS();
    }
    auto compoundCRS = dynamic_cast<const CompoundCRS *>(this);
    if (compoundCRS) {
        for (const auto &subCrs : compoundCRS->componentReferenceSystems()) {
            auto retGeogCRS = subCrs->extractGeographicCRS();
            if (retGeogCRS) {
                return retGeogCRS;
            }
        }
    }
    auto boundCRS = dynamic_cast<const BoundCRS *>(this);
    if (boundCRS) {
        return boundCRS->baseCRS()->extractGeographicCRS();
    }
    return nullptr;
}

// ---------------------------------------------------------------------------

/** \brief Return the VerticalCRS of the CRS.
 *
 * Returns the VerticalCRS contained in a CRS. This works currently with
 * input parameters of type VerticalCRS or derived, CompoundCRS or BoundCRS.
 *
 * @return a VerticalCRSPtr, that might be null.
 */
VerticalCRSPtr CRS::extractVerticalCRS() const {
    CRSPtr transformSourceCRS;
    auto vertCRS = dynamic_cast<const VerticalCRS *>(this);
    if (vertCRS) {
        return std::dynamic_pointer_cast<VerticalCRS>(
            shared_from_this().as_nullable());
    }
    auto compoundCRS = dynamic_cast<const CompoundCRS *>(this);
    if (compoundCRS) {
        for (const auto &subCrs : compoundCRS->componentReferenceSystems()) {
            auto retVertCRS = subCrs->extractVerticalCRS();
            if (retVertCRS) {
                return retVertCRS;
            }
        }
    }
    auto boundCRS = dynamic_cast<const BoundCRS *>(this);
    if (boundCRS) {
        return boundCRS->baseCRS()->extractVerticalCRS();
    }
    return nullptr;
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
struct SingleCRS::Private {
    datum::DatumPtr datum{};
    datum::DatumEnsemblePtr datumEnsemble{};
    cs::CoordinateSystemNNPtr coordinateSystem;

    Private(const datum::DatumNNPtr &datumIn,
            const cs::CoordinateSystemNNPtr &csIn)
        : datum(datumIn), coordinateSystem(csIn) {}
    Private(const datum::DatumPtr &datumIn,
            const cs::CoordinateSystemNNPtr &csIn)
        : datum(datumIn), coordinateSystem(csIn) {}
};
//! @endcond

// ---------------------------------------------------------------------------

SingleCRS::SingleCRS(const datum::DatumNNPtr &datumIn,
                     const cs::CoordinateSystemNNPtr &csIn)
    : d(internal::make_unique<Private>(datumIn, csIn)) {}

// ---------------------------------------------------------------------------

SingleCRS::SingleCRS(const datum::DatumPtr &datumIn,
                     const cs::CoordinateSystemNNPtr &csIn)
    : d(internal::make_unique<Private>(datumIn, csIn)) {}

// ---------------------------------------------------------------------------

#ifdef notdef
SingleCRS::SingleCRS(const SingleCRS &other)
    : // CRS(other),
      d(internal::make_unique<Private>(*other.d)) {}
#endif

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
SingleCRS::~SingleCRS() = default;
//! @endcond

// ---------------------------------------------------------------------------

/** \brief Return the datum::Datum associated with the CRS.
 *
 * This might be null, in which case datumEnsemble() return will not be null.
 *
 * @return a Datum that might be null.
 */
const datum::DatumPtr &SingleCRS::datum() const { return d->datum; }

// ---------------------------------------------------------------------------

/** \brief Return the datum::DatumEnsemble associated with the CRS.
 *
 * This might be null, in which case datum() return will not be null.
 *
 * @return a DatumEnsemble that might be null.
 */
const datum::DatumEnsemblePtr &SingleCRS::datumEnsemble() const {
    return d->datumEnsemble;
}

// ---------------------------------------------------------------------------

/** \brief Return the cs::CoordinateSystem associated with the CRS.
 *
 * This might be null, in which case datumEnsemble() return will not be null.
 *
 * @return a CoordinateSystem that might be null.
 */
const cs::CoordinateSystemNNPtr &SingleCRS::coordinateSystem() const {
    return d->coordinateSystem;
}

// ---------------------------------------------------------------------------

bool SingleCRS::_isEquivalentTo(const util::BaseObjectNNPtr &other,
                                util::IComparable::Criterion criterion) const {
    auto otherSingleCRS = util::nn_dynamic_pointer_cast<SingleCRS>(other);
    if (otherSingleCRS == nullptr ||
        !ObjectUsage::_isEquivalentTo(other, criterion)) {
        return false;
    }
    if ((datum() != nullptr) ^ (otherSingleCRS->datum() != nullptr)) {
        return false;
    }
    if (datum() &&
        !datum()->isEquivalentTo(NN_CHECK_ASSERT(otherSingleCRS->datum()),
                                 criterion))
        return false;

    // TODO test DatumEnsemble
    return coordinateSystem()->isEquivalentTo(
        otherSingleCRS->coordinateSystem(), criterion);
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
struct GeodeticCRS::Private {
    std::vector<operation::PointMotionOperationNNPtr> velocityModel{};
};
//! @endcond

// ---------------------------------------------------------------------------

GeodeticCRS::GeodeticCRS(const datum::GeodeticReferenceFrameNNPtr &datumIn,
                         const cs::EllipsoidalCSNNPtr &csIn)
    : SingleCRS(datumIn, csIn), d(internal::make_unique<Private>()) {}

// ---------------------------------------------------------------------------

GeodeticCRS::GeodeticCRS(const datum::GeodeticReferenceFrameNNPtr &datumIn,
                         const cs::SphericalCSNNPtr &csIn)
    : SingleCRS(datumIn, csIn), d(internal::make_unique<Private>()) {}

// ---------------------------------------------------------------------------

GeodeticCRS::GeodeticCRS(const datum::GeodeticReferenceFrameNNPtr &datumIn,
                         const cs::CartesianCSNNPtr &csIn)
    : SingleCRS(datumIn, csIn), d(internal::make_unique<Private>()) {}

// ---------------------------------------------------------------------------

#ifdef notdef
GeodeticCRS::GeodeticCRS(const GeodeticCRS &other)
    : SingleCRS(other), d(internal::make_unique<Private>(*other.d)) {}
#endif

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
GeodeticCRS::~GeodeticCRS() = default;
//! @endcond

// ---------------------------------------------------------------------------

/** \brief Return the datum::GeodeticReferenceFrame associated with the CRS.
 *
 * @return a GeodeticReferenceFrame
 */
const datum::GeodeticReferenceFrameNNPtr GeodeticCRS::datum() const {
    return NN_CHECK_THROW(
        std::dynamic_pointer_cast<datum::GeodeticReferenceFrame>(
            SingleCRS::getPrivate()->datum));
}

// ---------------------------------------------------------------------------

/** \brief Return the velocity model associated with the CRS.
 *
 * @return a velocity model. might be null.
 */
const std::vector<operation::PointMotionOperationNNPtr> &
GeodeticCRS::velocityModel() const {
    return d->velocityModel;
}

// ---------------------------------------------------------------------------

/** \brief Return whether the CRS is a geocentric one.
 *
 * A geocentric CRS is a geodetic CRS that has a Cartesian coordinate system
 * with three axis, whose direction is respectively
 * cs::AxisDirection::GEOCENTRIC_X,
 * cs::AxisDirection::GEOCENTRIC_Y and cs::AxisDirection::GEOCENTRIC_Z.
 *
 * @return true if the CRS is a geocentric CRS.
 */
bool GeodeticCRS::isGeocentric() const {
    return coordinateSystem()->getWKT2Type(io::WKTFormatter::create()) ==
               "Cartesian" &&
           coordinateSystem()->axisList().size() == 3 &&
           coordinateSystem()->axisList()[0]->direction() ==
               cs::AxisDirection::GEOCENTRIC_X &&
           coordinateSystem()->axisList()[1]->direction() ==
               cs::AxisDirection::GEOCENTRIC_Y &&
           coordinateSystem()->axisList()[2]->direction() ==
               cs::AxisDirection::GEOCENTRIC_Z;
}

// ---------------------------------------------------------------------------

/** \brief Instanciate a GeodeticCRS from a datum::GeodeticReferenceFrame and a
 * cs::SphericalCS.
 *
 * @param properties See \ref general_properties.
 * At minimum the name should be defined.
 * @param datum The datum of the CRS.
 * @param cs a SphericalCS.
 * @return new GeodeticCRS.
 */
GeodeticCRSNNPtr
GeodeticCRS::create(const util::PropertyMap &properties,
                    const datum::GeodeticReferenceFrameNNPtr &datum,
                    const cs::SphericalCSNNPtr &cs) {
    auto crs(GeodeticCRS::nn_make_shared<GeodeticCRS>(datum, cs));
    crs->assignSelf(util::nn_static_pointer_cast<util::BaseObject>(crs));
    crs->setProperties(properties);
    return crs;
}

// ---------------------------------------------------------------------------

/** \brief Instanciate a GeodeticCRS from a datum::GeodeticReferenceFrame and a
 * cs::CartesianCS.
 *
 * @param properties See \ref general_properties.
 * At minimum the name should be defined.
 * @param datum The datum of the CRS.
 * @param cs a CartesianCS.
 * @return new GeodeticCRS.
 */
GeodeticCRSNNPtr
GeodeticCRS::create(const util::PropertyMap &properties,
                    const datum::GeodeticReferenceFrameNNPtr &datum,
                    const cs::CartesianCSNNPtr &cs) {
    auto crs(GeodeticCRS::nn_make_shared<GeodeticCRS>(datum, cs));
    crs->assignSelf(util::nn_static_pointer_cast<util::BaseObject>(crs));
    crs->setProperties(properties);
    return crs;
}

// ---------------------------------------------------------------------------

std::string GeodeticCRS::exportToWKT(io::WKTFormatterNNPtr formatter) const {
    const bool isWKT2 = formatter->version() == io::WKTFormatter::Version::WKT2;
    formatter->startNode(isWKT2 ? ((formatter->use2018Keywords() &&
                                    dynamic_cast<const GeographicCRS *>(this))
                                       ? io::WKTConstants::GEOGCRS
                                       : io::WKTConstants::GEODCRS)
                                : isGeocentric() ? io::WKTConstants::GEOCCS
                                                 : io::WKTConstants::GEOGCS,
                         !identifiers().empty());
    formatter->addQuotedString(*(name()->description()));
    auto &axisList = coordinateSystem()->axisList();
    if (!axisList.empty()) {
        formatter->pushAxisAngularUnit(
            util::nn_make_shared<common::UnitOfMeasure>(axisList[0]->unit()));
    }
    datum()->exportToWKT(formatter);
    datum()->primeMeridian()->exportToWKT(formatter);
    if (!axisList.empty()) {
        formatter->popAxisAngularUnit();
    }
    if (!isWKT2) {
        if (!axisList.empty()) {
            axisList[0]->unit().exportToWKT(formatter);
        }
    }
    coordinateSystem()->exportToWKT(formatter);
    ObjectUsage::_exportToWKT(formatter);
    formatter->endNode();
    return formatter->toString();
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
void GeodeticCRS::addGeocentricUnitConversionIntoPROJString(
    io::PROJStringFormatterNNPtr formatter) const {

    auto &axisList = coordinateSystem()->axisList();
    if (!axisList.empty() &&
        axisList[0]->unit() != common::UnitOfMeasure::METRE) {
        if (formatter->convention() ==
            io::PROJStringFormatter::Convention::PROJ_4) {
            throw io::FormattingException("GeodeticCRS::exportToPROJString(): "
                                          "non-meter unit not supported for "
                                          "PROJ.4");
        }
        auto projUnit = axisList[0]->unit().exportToPROJString();

        formatter->addStep("unitconvert");
        formatter->addParam("xy_in", "m");
        formatter->addParam("z_in", "m");
        if (projUnit.empty()) {
            formatter->addParam("xy_out", axisList[0]->unit().conversionToSI());
            formatter->addParam("z_out", axisList[0]->unit().conversionToSI());
        } else {
            formatter->addParam("xy_out", projUnit);
            formatter->addParam("z_out", projUnit);
        }
    }
}
//! @endcond

// ---------------------------------------------------------------------------

std::string
GeodeticCRS::exportToPROJString(io::PROJStringFormatterNNPtr formatter)
    const // throw(io::FormattingException)
{
    if (!isGeocentric()) {
        throw io::FormattingException("GeodeticCRS::exportToPROJString() only "
                                      "supports geocentric coordinate systems");
    }

    io::PROJStringFormatter::Scope scope(formatter);

    if (formatter->convention() ==
        io::PROJStringFormatter::Convention::PROJ_4) {
        formatter->addStep("geocent");
    } else {
        formatter->addStep("cart");
    }
    datum()->ellipsoid()->exportToPROJString(formatter);
    addGeocentricUnitConversionIntoPROJString(formatter);

    return scope.toString();
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
void GeodeticCRS::addDatumInfoToPROJString(
    io::PROJStringFormatterNNPtr formatter)
    const // throw(io::FormattingException)
{
    const auto &TOWGS84Params = formatter->getTOWGS84Parameters();
    auto nadgrids = formatter->getHDatumExtension();
    if (formatter->convention() ==
            io::PROJStringFormatter::Convention::PROJ_4 &&
        datum()->isEquivalentTo(datum::GeodeticReferenceFrame::EPSG_6326) &&
        TOWGS84Params.empty() && nadgrids.empty()) {
        formatter->addParam("datum", "WGS84");
    } else {
        datum()->ellipsoid()->exportToPROJString(formatter);
        datum()->primeMeridian()->exportToPROJString(formatter);
    }
    if (TOWGS84Params.size() == 7) {
        formatter->addParam("towgs84", TOWGS84Params);
    }
    if (!nadgrids.empty()) {
        formatter->addParam("nadgrids", nadgrids);
    }
}
//! @endcond

// ---------------------------------------------------------------------------

bool GeodeticCRS::isEquivalentTo(const util::BaseObjectNNPtr &other,
                                 util::IComparable::Criterion criterion) const {
    auto otherGeodCRS = util::nn_dynamic_pointer_cast<GeodeticCRS>(other);
    // TODO test velocityModel
    return otherGeodCRS != nullptr &&
           SingleCRS::_isEquivalentTo(other, criterion);
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
struct GeographicCRS::Private {};
//! @endcond

// ---------------------------------------------------------------------------

GeographicCRS::GeographicCRS(const datum::GeodeticReferenceFrameNNPtr &datumIn,
                             const cs::EllipsoidalCSNNPtr &csIn)
    : SingleCRS(datumIn, csIn), GeodeticCRS(datumIn, csIn),
      d(internal::make_unique<Private>()) {}

// ---------------------------------------------------------------------------

#ifdef notdef
GeographicCRS::GeographicCRS(const GeographicCRS &other)
    : SingleCRS(other), GeodeticCRS(other),
      d(internal::make_unique<Private>(*other.d)) {}
#endif

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
GeographicCRS::~GeographicCRS() = default;
//! @endcond

// ---------------------------------------------------------------------------

/** \brief Return the cs::EllipsoidalCS associated with the CRS.
 *
 * @return a EllipsoidalCS.
 */
const cs::EllipsoidalCSNNPtr GeographicCRS::coordinateSystem() const {
    return NN_CHECK_THROW(util::nn_dynamic_pointer_cast<cs::EllipsoidalCS>(
        SingleCRS::getPrivate()->coordinateSystem));
}

// ---------------------------------------------------------------------------

/** \brief Instanciate a GeographicCRS from a datum::Datum and a
 * cs::EllipsoidalCS.
 *
 * @param properties See \ref general_properties.
 * At minimum the name should be defined.
 * @param datum The datum of the CRS.
 * @param cs a EllipsoidalCS.
 * @return new GeographicCRS.
 */
GeographicCRSNNPtr
GeographicCRS::create(const util::PropertyMap &properties,
                      const datum::GeodeticReferenceFrameNNPtr &datum,
                      const cs::EllipsoidalCSNNPtr &cs) {
    GeographicCRSNNPtr crs(
        GeographicCRS::nn_make_shared<GeographicCRS>(datum, cs));
    crs->assignSelf(util::nn_static_pointer_cast<util::BaseObject>(crs));
    crs->setProperties(properties);
    return crs;
}

// ---------------------------------------------------------------------------

/** \brief Return whether the current GeographicCRS is the 2D part of the
 * other 3D GeographicCRS.
 */
bool GeographicCRS::is2DPartOf3D(const GeographicCRSNNPtr &other) const {
    auto axis = coordinateSystem()->axisList();
    auto otherAxis = other->coordinateSystem()->axisList();
    return axis.size() == 2 && otherAxis.size() == 3 &&
           datum()->isEquivalentTo(other->datum()) &&
           axis[0]->isEquivalentTo(otherAxis[0]) &&
           axis[1]->isEquivalentTo(otherAxis[1]);
}

// ---------------------------------------------------------------------------

GeographicCRSNNPtr GeographicCRS::createEPSG_4269() {
    util::PropertyMap propertiesCRS;
    propertiesCRS
        .set(metadata::Identifier::CODESPACE_KEY, metadata::Identifier::EPSG)
        .set(metadata::Identifier::CODE_KEY, 4269)
        .set(common::IdentifiedObject::NAME_KEY, "NAD83");
    return create(propertiesCRS, datum::GeodeticReferenceFrame::EPSG_6269,
                  cs::EllipsoidalCS::createLatitudeLongitude(
                      common::UnitOfMeasure::DEGREE));
}

// ---------------------------------------------------------------------------

GeographicCRSNNPtr GeographicCRS::createEPSG_4326() {
    util::PropertyMap propertiesCRS;
    propertiesCRS
        .set(metadata::Identifier::CODESPACE_KEY, metadata::Identifier::EPSG)
        .set(metadata::Identifier::CODE_KEY, 4326)
        .set(common::IdentifiedObject::NAME_KEY, "WGS 84");
    return create(propertiesCRS, datum::GeodeticReferenceFrame::EPSG_6326,
                  cs::EllipsoidalCS::createLatitudeLongitude(
                      common::UnitOfMeasure::DEGREE));
}

// ---------------------------------------------------------------------------

GeographicCRSNNPtr GeographicCRS::createEPSG_4979() {
    util::PropertyMap propertiesCRS;
    propertiesCRS
        .set(metadata::Identifier::CODESPACE_KEY, metadata::Identifier::EPSG)
        .set(metadata::Identifier::CODE_KEY, 4979)
        .set(common::IdentifiedObject::NAME_KEY, "WGS 84");
    return create(
        propertiesCRS, datum::GeodeticReferenceFrame::EPSG_6326,
        cs::EllipsoidalCS::createLatitudeLongitudeEllipsoidalHeight(
            common::UnitOfMeasure::DEGREE, common::UnitOfMeasure::METRE));
}

// ---------------------------------------------------------------------------

GeographicCRSNNPtr GeographicCRS::createEPSG_4807() {
    util::PropertyMap propertiesEllps;
    propertiesEllps
        .set(metadata::Identifier::CODESPACE_KEY, metadata::Identifier::EPSG)
        .set(metadata::Identifier::CODE_KEY, 6807)
        .set(common::IdentifiedObject::NAME_KEY, "Clarke 1880 (IGN)");
    auto ellps(datum::Ellipsoid::createFlattenedSphere(
        propertiesEllps, common::Length(6378249.2),
        common::Scale(293.4660212936269)));

    auto axisLat(cs::CoordinateSystemAxis::create(
        util::PropertyMap().set(common::IdentifiedObject::NAME_KEY,
                                cs::AxisName::Latitude),
        cs::AxisAbbreviation::lat, cs::AxisDirection::NORTH,
        common::UnitOfMeasure::GRAD));

    auto axisLong(cs::CoordinateSystemAxis::create(
        util::PropertyMap().set(common::IdentifiedObject::NAME_KEY,
                                cs::AxisName::Longitude),
        cs::AxisAbbreviation::lon, cs::AxisDirection::EAST,
        common::UnitOfMeasure::GRAD));

    auto cs(cs::EllipsoidalCS::create(util::PropertyMap(), axisLat, axisLong));

    util::PropertyMap propertiesDatum;
    propertiesDatum
        .set(metadata::Identifier::CODESPACE_KEY, metadata::Identifier::EPSG)
        .set(metadata::Identifier::CODE_KEY, 6807)
        .set(common::IdentifiedObject::NAME_KEY,
             "Nouvelle_Triangulation_Francaise_Paris");
    auto datum(datum::GeodeticReferenceFrame::create(
        propertiesDatum, ellps, util::optional<std::string>(),
        datum::PrimeMeridian::PARIS));

    util::PropertyMap propertiesCRS;
    propertiesCRS
        .set(metadata::Identifier::CODESPACE_KEY, metadata::Identifier::EPSG)
        .set(metadata::Identifier::CODE_KEY, 4807)
        .set(common::IdentifiedObject::NAME_KEY, "NTF (Paris)");
    auto gcrs(create(propertiesCRS, datum, cs));
    return gcrs;
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
void GeographicCRS::addAngularUnitConvertAndAxisSwap(
    io::PROJStringFormatterNNPtr formatter) const {
    auto &axisList = coordinateSystem()->axisList();

    if (!axisList.empty()) {
        auto projUnit = axisList[0]->unit().exportToPROJString();
        formatter->addStep("unitconvert");
        formatter->addParam("xy_in", "rad");
        if (projUnit.empty()) {
            formatter->addParam("xy_out", axisList[0]->unit().conversionToSI());
        } else {
            formatter->addParam("xy_out", projUnit);
        }
    }

    if (axisList.size() >= 2 &&
        ci_equal(*(axisList[0]->name()->description()), "Latitude") &&
        ci_equal(*(axisList[1]->name()->description()), "Longitude")) {
        formatter->addStep("axisswap");
        formatter->addParam("order", "2,1");
    }
}
//! @endcond

// ---------------------------------------------------------------------------

std::string
GeographicCRS::exportToPROJString(io::PROJStringFormatterNNPtr formatter)
    const // throw(io::FormattingException)
{
    io::PROJStringFormatter::Scope scope(formatter);
    if (!formatter->omitProjLongLatIfPossible() ||
        datum()->primeMeridian()->longitude().getSIValue() != 0.0 ||
        !formatter->getTOWGS84Parameters().empty() ||
        !formatter->getHDatumExtension().empty()) {
        formatter->addStep("longlat");
        if (formatter->convention() ==
                io::PROJStringFormatter::Convention::PROJ_5 &&
            (datum()->primeMeridian()->longitude().getSIValue() != 0.0 ||
             !formatter->getTOWGS84Parameters().empty() ||
             !formatter->getHDatumExtension().empty())) {
            formatter->setCurrentStepInverted(true);
        }
        addDatumInfoToPROJString(formatter);
    }

    if (formatter->convention() ==
        io::PROJStringFormatter::Convention::PROJ_5) {
        addAngularUnitConvertAndAxisSwap(formatter);
    }
    return scope.toString();
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
struct VerticalCRS::Private {
    std::vector<operation::TransformationNNPtr> geoidModel{};
    std::vector<operation::PointMotionOperationNNPtr> velocityModel{};
};
//! @endcond

// ---------------------------------------------------------------------------

VerticalCRS::VerticalCRS(const datum::VerticalReferenceFrameNNPtr &datumIn,
                         const cs::VerticalCSNNPtr &csIn)
    : SingleCRS(datumIn, csIn), d(internal::make_unique<Private>()) {}

// ---------------------------------------------------------------------------

#ifdef notdef
VerticalCRS::VerticalCRS(const VerticalCRS &other)
    : SingleCRS(other), d(internal::make_unique<Private>(*other.d)) {}
#endif

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
VerticalCRS::~VerticalCRS() = default;
//! @endcond

// ---------------------------------------------------------------------------

/** \brief Return the datum::VerticalReferenceFrame associated with the CRS.
 *
 * @return a VerticalReferenceFrame.
 */
const datum::VerticalReferenceFramePtr VerticalCRS::datum() const {
    return std::dynamic_pointer_cast<datum::VerticalReferenceFrame>(
        SingleCRS::getPrivate()->datum);
}

// ---------------------------------------------------------------------------

/** \brief Return the geoid model associated with the CRS.
 *
 * Geoid height model or height correction model linked to a geoid-based
 * vertical CRS.
 *
 * @return a geoid model. might be null
 */
const std::vector<operation::TransformationNNPtr> &
VerticalCRS::geoidModel() const {
    return d->geoidModel;
}

// ---------------------------------------------------------------------------

/** \brief Return the velocity model associated with the CRS.
 *
 * @return a velocity model. might be null.
 */
const std::vector<operation::PointMotionOperationNNPtr> &
VerticalCRS::velocityModel() const {
    return d->velocityModel;
}

// ---------------------------------------------------------------------------

/** \brief Return the cs::VerticalCS associated with the CRS.
 *
 * @return a VerticalCS.
 */
const cs::VerticalCSNNPtr VerticalCRS::coordinateSystem() const {
    return NN_CHECK_THROW(util::nn_dynamic_pointer_cast<cs::VerticalCS>(
        SingleCRS::getPrivate()->coordinateSystem));
}

// ---------------------------------------------------------------------------

std::string VerticalCRS::exportToWKT(io::WKTFormatterNNPtr formatter) const {
    const bool isWKT2 = formatter->version() == io::WKTFormatter::Version::WKT2;
    formatter->startNode(isWKT2 ? io::WKTConstants::VERTCRS
                                : io::WKTConstants::VERT_CS,
                         !identifiers().empty());
    formatter->addQuotedString(*(name()->description()));
    if (datum()) {
        datum()->exportToWKT(formatter);
    } else {
        throw io::FormattingException("Missing VDATUM");
    }
    auto &axisList = coordinateSystem()->axisList();
    if (!isWKT2 && !axisList.empty()) {
        axisList[0]->unit().exportToWKT(formatter);
    }
    coordinateSystem()->exportToWKT(formatter);
    ObjectUsage::_exportToWKT(formatter);
    formatter->endNode();
    return formatter->toString();
}

// ---------------------------------------------------------------------------

std::string
VerticalCRS::exportToPROJString(io::PROJStringFormatterNNPtr formatter)
    const // throw(io::FormattingException)
{
    auto nadgrids = formatter->getVDatumExtension();
    if (!nadgrids.empty()) {
        formatter->addParam("nadgrids", nadgrids);
    }

    auto &axisList = coordinateSystem()->axisList();
    if (!axisList.empty() &&
        axisList[0]->unit() != common::UnitOfMeasure::METRE) {
        auto projUnit = axisList[0]->unit().exportToPROJString();
        if (projUnit.empty()) {
            formatter->addParam("vto_meter",
                                axisList[0]->unit().conversionToSI());
        } else {
            formatter->addParam("vunits", projUnit);
        }
    }

    return formatter->toString();
}

// ---------------------------------------------------------------------------

/** \brief Instanciate a VerticalCRS from a datum::VerticalReferenceFrame and a
 * cs::VerticalCS.
 *
 * @param properties See \ref general_properties.
 * At minimum the name should be defined.
 * @param datumIn The datum of the CRS.
 * @param csIn a VerticalCS.
 * @return new VerticalCRS.
 */
VerticalCRSNNPtr
VerticalCRS::create(const util::PropertyMap &properties,
                    const datum::VerticalReferenceFrameNNPtr &datumIn,
                    const cs::VerticalCSNNPtr &csIn) {
    auto crs(VerticalCRS::nn_make_shared<VerticalCRS>(datumIn, csIn));
    crs->assignSelf(util::nn_static_pointer_cast<util::BaseObject>(crs));
    crs->setProperties(properties);
    return crs;
}

// ---------------------------------------------------------------------------

bool VerticalCRS::isEquivalentTo(const util::BaseObjectNNPtr &other,
                                 util::IComparable::Criterion criterion) const {
    auto otherVertCRS = util::nn_dynamic_pointer_cast<VerticalCRS>(other);
    // TODO test geoidModel and velocityModel
    return otherVertCRS != nullptr &&
           SingleCRS::_isEquivalentTo(other, criterion);
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
struct DerivedCRS::Private {
    SingleCRSNNPtr baseCRS_;
    operation::ConversionNNPtr derivingConversion_;

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
                       const operation::ConversionNNPtr &derivingConversionIn,
                       const cs::CoordinateSystemNNPtr &
#if !defined(COMPILER_WARNS_ABOUT_ABSTRACT_VBASE_INIT)
                           cs
#endif
                       )
    :
#if !defined(COMPILER_WARNS_ABOUT_ABSTRACT_VBASE_INIT)
      SingleCRS(baseCRSIn->datum(), cs),
#endif
      d(internal::make_unique<Private>(
          baseCRSIn, operation::Conversion::create(derivingConversionIn))) {
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

//! @cond Doxygen_Suppress
DerivedCRS::~DerivedCRS() = default;
//! @endcond

// ---------------------------------------------------------------------------

/** \brief Return the base CRS of a DerivedCRS.
 *
 * @return the base CRS.
 */
const SingleCRSNNPtr &DerivedCRS::baseCRS() const { return d->baseCRS_; }

// ---------------------------------------------------------------------------

/** \brief Return the deriving conversion from the base CRS to this CRS.
 *
 * @return the deriving conversion.
 */
const operation::ConversionNNPtr &DerivedCRS::derivingConversion() const {
    return d->derivingConversion_;
}

// ---------------------------------------------------------------------------

bool DerivedCRS::isEquivalentTo(const util::BaseObjectNNPtr &other,
                                util::IComparable::Criterion criterion) const {
    auto otherDerivedCRS = util::nn_dynamic_pointer_cast<DerivedCRS>(other);
    if (otherDerivedCRS == nullptr ||
        !SingleCRS::_isEquivalentTo(other, criterion)) {
        return false;
    }
    return baseCRS()->isEquivalentTo(otherDerivedCRS->baseCRS(), criterion) &&
           derivingConversion()->isEquivalentTo(
               otherDerivedCRS->derivingConversion(), criterion);
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
struct ProjectedCRS::Private {};
//! @endcond

// ---------------------------------------------------------------------------

ProjectedCRS::ProjectedCRS(
    const GeodeticCRSNNPtr &baseCRSIn,
    const operation::ConversionNNPtr &derivingConversionIn,
    const cs::CartesianCSNNPtr &csIn)
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

//! @cond Doxygen_Suppress
ProjectedCRS::~ProjectedCRS() = default;
//! @endcond

// ---------------------------------------------------------------------------

/** \brief Return the base CRS (a GeographicCRS) of the ProjectedCRS.
 *
 * @return the base CRS.
 */
const GeodeticCRSNNPtr ProjectedCRS::baseCRS() const {
    return NN_CHECK_ASSERT(util::nn_dynamic_pointer_cast<GeodeticCRS>(
        DerivedCRS::getPrivate()->baseCRS_));
}

// ---------------------------------------------------------------------------

/** \brief Return the cs::CartesianCS associated with the CRS.
 *
 * @return a CartesianCS
 */
const cs::CartesianCSNNPtr ProjectedCRS::coordinateSystem() const {
    return NN_CHECK_THROW(util::nn_dynamic_pointer_cast<cs::CartesianCS>(
        SingleCRS::getPrivate()->coordinateSystem));
}

// ---------------------------------------------------------------------------

std::string ProjectedCRS::exportToWKT(io::WKTFormatterNNPtr formatter) const {
    const bool isWKT2 = formatter->version() == io::WKTFormatter::Version::WKT2;
    formatter->startNode(isWKT2 ? io::WKTConstants::PROJCRS
                                : io::WKTConstants::PROJCS,
                         !identifiers().empty());
    formatter->addQuotedString(*(name()->description()));

    auto &geodeticCRSAxisList = baseCRS()->coordinateSystem()->axisList();

    if (isWKT2) {
        formatter->startNode(
            (formatter->use2018Keywords() &&
             dynamic_cast<const GeographicCRS *>(baseCRS().get()))
                ? io::WKTConstants::BASEGEOGCRS
                : io::WKTConstants::BASEGEODCRS,
            !baseCRS()->identifiers().empty());
        formatter->addQuotedString(*(baseCRS()->name()->description()));
        baseCRS()->datum()->exportToWKT(formatter);
        // insert ellipsoidal cs unit when the units of the map
        // projection angular parameters are not explicitly given within those
        // parameters. See
        // http://docs.opengeospatial.org/is/12-063r5/12-063r5.html#61
        if (formatter->primeMeridianOrParameterUnitOmittedIfSameAsAxis() &&
            !geodeticCRSAxisList.empty()) {
            geodeticCRSAxisList[0]->unit().exportToWKT(formatter);
        }
        baseCRS()->datum()->primeMeridian()->exportToWKT(formatter);
        formatter->endNode();
    } else {
        baseCRS()->exportToWKT(formatter);
    }

    auto &axisList = coordinateSystem()->axisList();
    if (!axisList.empty()) {
        formatter->pushAxisLinearUnit(
            util::nn_make_shared<common::UnitOfMeasure>(axisList[0]->unit()));
    }
    if (!geodeticCRSAxisList.empty()) {
        formatter->pushAxisAngularUnit(
            util::nn_make_shared<common::UnitOfMeasure>(
                geodeticCRSAxisList[0]->unit()));
    }

    derivingConversion()->exportToWKT(formatter);

    if (!geodeticCRSAxisList.empty()) {
        formatter->popAxisAngularUnit();
    }
    if (!axisList.empty()) {
        formatter->popAxisLinearUnit();
    }

    if (!isWKT2) {
        if (!axisList.empty()) {
            axisList[0]->unit().exportToWKT(formatter);
        }
    }

    coordinateSystem()->exportToWKT(formatter);
    ObjectUsage::_exportToWKT(formatter);
    formatter->endNode();
    return formatter->toString();
}

// ---------------------------------------------------------------------------

std::string
ProjectedCRS::exportToPROJString(io::PROJStringFormatterNNPtr formatter)
    const // throw(io::FormattingException)
{
    derivingConversion()->exportToPROJString(formatter);

    return formatter->toString();
}

// ---------------------------------------------------------------------------

/** \brief Instanciate a ProjectedCRS from a base CRS, a deriving
 * operation::Conversion
 * and a coordinate system.
 *
 * @param properties See \ref general_properties.
 * At minimum the name should be defined.
 * @param baseCRSIn The base CRS.
 * @param derivingConversionIn The deriving operation::Conversion (typically
 * using a map
 * projection method)
 * @param csIn The coordniate system.
 * @return new ProjectedCRS.
 */
ProjectedCRSNNPtr
ProjectedCRS::create(const util::PropertyMap &properties,
                     const GeodeticCRSNNPtr &baseCRSIn,
                     const operation::ConversionNNPtr &derivingConversionIn,
                     const cs::CartesianCSNNPtr &csIn) {
    auto crs = ProjectedCRS::nn_make_shared<ProjectedCRS>(
        baseCRSIn, derivingConversionIn, csIn);
    crs->assignSelf(util::nn_static_pointer_cast<util::BaseObject>(crs));
    crs->setProperties(properties);
    crs->derivingConversion()->setWeakSourceTargetCRS(
        static_cast<const GeodeticCRSPtr>(baseCRSIn),
        static_cast<const ProjectedCRSPtr>(crs));
    return crs;
}

// ---------------------------------------------------------------------------

bool ProjectedCRS::isEquivalentTo(
    const util::BaseObjectNNPtr &other,
    util::IComparable::Criterion criterion) const {
    auto otherProjCRS = util::nn_dynamic_pointer_cast<ProjectedCRS>(other);
    return otherProjCRS != nullptr &&
           DerivedCRS::isEquivalentTo(other, criterion);
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

//! @cond Doxygen_Suppress
CompoundCRS::~CompoundCRS() = default;
//! @endcond

// ---------------------------------------------------------------------------

/** \brief Return the components of a CompoundCRS.
 *
 * If the CompoundCRS is itself made of CompoundCRS components, this method
 * will return a flattened view of the components (as SingleCRS or BoundCRS)
 *
 * @return the components.
 */
std::vector<CRSNNPtr> CompoundCRS::componentReferenceSystems() const {
    // Flatten the potential hierarchy to return only SingleCRS
    std::vector<CRSNNPtr> res;
    for (const auto &crs : d->components_) {
        auto childCompound = util::nn_dynamic_pointer_cast<CompoundCRS>(crs);
        if (childCompound) {
            auto childFlattened = childCompound->componentReferenceSystems();
            res.insert(res.end(), childFlattened.begin(), childFlattened.end());
        } else {
            res.push_back(crs);
        }
    }
    return res;
}

// ---------------------------------------------------------------------------

/** \brief Instanciate a CompoundCRS from a vector of CRS.
 *
 * @param properties See \ref general_properties.
 * At minimum the name should be defined.
 * @param components the component CRS of the CompoundCRS.
 * @return new CompoundCRS.
 */
CompoundCRSNNPtr CompoundCRS::create(const util::PropertyMap &properties,
                                     const std::vector<CRSNNPtr> &components) {
    auto compoundCRS(CompoundCRS::nn_make_shared<CompoundCRS>(components));
    compoundCRS->assignSelf(
        util::nn_static_pointer_cast<util::BaseObject>(compoundCRS));
    compoundCRS->setProperties(properties);
    if (properties.find(common::IdentifiedObject::NAME_KEY) ==
        properties.end()) {
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
        util::PropertyMap propertyName;
        propertyName.set(common::IdentifiedObject::NAME_KEY, name);
        compoundCRS->setProperties(propertyName);
    }

    return compoundCRS;
}

// ---------------------------------------------------------------------------

std::string CompoundCRS::exportToWKT(io::WKTFormatterNNPtr formatter) const {
    const bool isWKT2 = formatter->version() == io::WKTFormatter::Version::WKT2;
    formatter->startNode(isWKT2 ? io::WKTConstants::COMPOUNDCRS
                                : io::WKTConstants::COMPD_CS,
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

std::string
CompoundCRS::exportToPROJString(io::PROJStringFormatterNNPtr formatter)
    const // throw(io::FormattingException)
{
    for (const auto &crs : componentReferenceSystems()) {
        auto crs_exportable =
            util::nn_dynamic_pointer_cast<IPROJStringExportable>(crs);
        if (crs_exportable) {
            crs_exportable->exportToPROJString(formatter);
        }
    }

    return formatter->toString();
}

// ---------------------------------------------------------------------------

bool CompoundCRS::isEquivalentTo(const util::BaseObjectNNPtr &other,
                                 util::IComparable::Criterion criterion) const {
    auto otherCompoundCRS = util::nn_dynamic_pointer_cast<CompoundCRS>(other);
    if (otherCompoundCRS == nullptr ||
        !ObjectUsage::_isEquivalentTo(other, criterion)) {
        return false;
    }
    auto components = componentReferenceSystems();
    auto otherComponents = otherCompoundCRS->componentReferenceSystems();
    if (components.size() != otherComponents.size()) {
        return false;
    }
    for (size_t i = 0; i < components.size(); i++) {
        if (!components[i]->isEquivalentTo(otherComponents[i], criterion)) {
            return false;
        }
    }
    return true;
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
struct BoundCRS::Private {
    CRSNNPtr baseCRS_;
    CRSNNPtr hubCRS_;
    operation::TransformationNNPtr transformation_;

    Private(const CRSNNPtr &baseCRSIn, const CRSNNPtr &hubCRSIn,
            const operation::TransformationNNPtr &transformationIn)
        : baseCRS_(baseCRSIn), hubCRS_(hubCRSIn),
          transformation_(transformationIn) {}
};
//! @endcond

// ---------------------------------------------------------------------------

BoundCRS::BoundCRS(const CRSNNPtr &baseCRSIn, const CRSNNPtr &hubCRSIn,
                   const operation::TransformationNNPtr &transformationIn)
    : d(internal::make_unique<Private>(baseCRSIn, hubCRSIn, transformationIn)) {
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
BoundCRS::~BoundCRS() = default;
//! @endcond

// ---------------------------------------------------------------------------

/** \brief Return the base CRS.
 *
 * This is the CRS into which coordinates of the BoundCRS are expressed.
 *
 * @return the base CRS.
 */
const CRSNNPtr &BoundCRS::baseCRS() const { return d->baseCRS_; }

// ---------------------------------------------------------------------------

/** \brief Return the target / hub CRS.
 *
 * @return the hub CRS.
 */
const CRSNNPtr &BoundCRS::hubCRS() const { return d->hubCRS_; }

// ---------------------------------------------------------------------------

/** \brief Return the transformation to the hub RS.
 *
 * @return transformation.
 */
const operation::TransformationNNPtr &BoundCRS::transformation() const {
    return d->transformation_;
}

// ---------------------------------------------------------------------------

/** \brief Instanciate a BoundCRS from a base CRS, a hub CRS and a
 * transformation.
 *
 * @param baseCRSIn base CRS.
 * @param hubCRSIn hub CRS.
 * @param transformationIn transformation from base CRS to hub CRS.
 * @return new BoundCRS.
 */
BoundCRSNNPtr
BoundCRS::create(const CRSNNPtr &baseCRSIn, const CRSNNPtr &hubCRSIn,
                 const operation::TransformationNNPtr &transformationIn) {
    auto crs = BoundCRS::nn_make_shared<BoundCRS>(baseCRSIn, hubCRSIn,
                                                  transformationIn);
    crs->assignSelf(util::nn_static_pointer_cast<util::BaseObject>(crs));
    if (baseCRSIn->name()->description().has_value()) {
        crs->setProperties(
            util::PropertyMap().set(common::IdentifiedObject::NAME_KEY,
                                    *(baseCRSIn->name()->description())));
    }
    return crs;
}

// ---------------------------------------------------------------------------

/** \brief Instanciate a BoundCRS from a base CRS and TOWGS84 parameters
 *
 * @param baseCRSIn base CRS.
 * @param TOWGS84Parameters a vector of 7 double values representing WKT1
 * TOWGS84 parameter.
 * @return new BoundCRS.
 */
BoundCRSNNPtr
BoundCRS::createFromTOWGS84(const CRSNNPtr &baseCRSIn,
                            const std::vector<double> TOWGS84Parameters) {
    auto crs = BoundCRS::nn_make_shared<BoundCRS>(
        baseCRSIn, GeographicCRS::EPSG_4326,
        operation::Transformation::createTOWGS84(baseCRSIn, TOWGS84Parameters));
    crs->assignSelf(util::nn_static_pointer_cast<util::BaseObject>(crs));
    if (baseCRSIn->name()->description().has_value()) {
        crs->setProperties(
            util::PropertyMap().set(common::IdentifiedObject::NAME_KEY,
                                    *(baseCRSIn->name()->description())));
    }
    return crs;
}

// ---------------------------------------------------------------------------

bool BoundCRS::isTOWGS84Compatible() const {
    return util::nn_dynamic_pointer_cast<GeographicCRS>(hubCRS()) != nullptr &&
           ci_equal(*(hubCRS()->name()->description()), "WGS 84");
}

// ---------------------------------------------------------------------------

std::string BoundCRS::getHDatumPROJ4GRIDS() const {
    if (ci_equal(*(hubCRS()->name()->description()), "WGS 84")) {
        return transformation()->getNTv2Filename();
    }
    return std::string();
}

// ---------------------------------------------------------------------------

std::string BoundCRS::getVDatumPROJ4GRIDS() const {
    if (util::nn_dynamic_pointer_cast<VerticalCRS>(baseCRS()) &&
        ci_equal(*(hubCRS()->name()->description()), "WGS 84")) {
        return transformation()->getHeightToGeographic3DFilename();
    }
    return std::string();
}

// ---------------------------------------------------------------------------

std::string BoundCRS::exportToWKT(io::WKTFormatterNNPtr formatter) const {
    const bool isWKT2 = formatter->version() == io::WKTFormatter::Version::WKT2;
    if (isWKT2) {
        formatter->startNode(io::WKTConstants::BOUNDCRS, false);
        formatter->startNode(io::WKTConstants::SOURCECRS, false);
        baseCRS()->exportToWKT(formatter);
        formatter->endNode();
        formatter->startNode(io::WKTConstants::TARGETCRS, false);
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
            throw io::FormattingException(
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

std::string BoundCRS::exportToPROJString(io::PROJStringFormatterNNPtr formatter)
    const // throw(io::FormattingException)
{
    if (formatter->convention() ==
        io::PROJStringFormatter::Convention::PROJ_5) {
        throw io::FormattingException(
            "BoundCRS cannot be exported as a PROJ.5 string, but its baseCRS "
            "might");
    }

    auto crs_exportable =
        util::nn_dynamic_pointer_cast<io::IPROJStringExportable>(baseCRS());
    if (!crs_exportable) {
        throw io::FormattingException(
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

bool BoundCRS::isEquivalentTo(const util::BaseObjectNNPtr &other,
                              util::IComparable::Criterion criterion) const {
    auto otherBoundCRS = util::nn_dynamic_pointer_cast<BoundCRS>(other);
    if (otherBoundCRS == nullptr ||
        !ObjectUsage::_isEquivalentTo(other, criterion)) {
        return false;
    }
    return baseCRS()->isEquivalentTo(otherBoundCRS->baseCRS(), criterion) &&
           hubCRS()->isEquivalentTo(otherBoundCRS->hubCRS(), criterion) &&
           transformation()->isEquivalentTo(otherBoundCRS->transformation(),
                                            criterion);
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
struct DerivedGeodeticCRS::Private {};
//! @endcond

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
DerivedGeodeticCRS::~DerivedGeodeticCRS() = default;
//! @endcond

// ---------------------------------------------------------------------------

DerivedGeodeticCRS::DerivedGeodeticCRS(
    const GeodeticCRSNNPtr &baseCRSIn,
    const operation::ConversionNNPtr &derivingConversionIn,
    const cs::CartesianCSNNPtr &csIn)
    : SingleCRS(baseCRSIn->datum(), csIn),
      GeodeticCRS(baseCRSIn->datum(), csIn),
      DerivedCRS(baseCRSIn, derivingConversionIn, csIn),
      d(internal::make_unique<Private>()) {}

// ---------------------------------------------------------------------------

DerivedGeodeticCRS::DerivedGeodeticCRS(
    const GeodeticCRSNNPtr &baseCRSIn,
    const operation::ConversionNNPtr &derivingConversionIn,
    const cs::SphericalCSNNPtr &csIn)
    : SingleCRS(baseCRSIn->datum(), csIn),
      GeodeticCRS(baseCRSIn->datum(), csIn),
      DerivedCRS(baseCRSIn, derivingConversionIn, csIn),
      d(internal::make_unique<Private>()) {}

// ---------------------------------------------------------------------------

/** \brief Return the base CRS (a GeodeticCRS) of a DerivedGeodeticCRS.
 *
 * @return the base CRS.
 */
const GeodeticCRSNNPtr DerivedGeodeticCRS::baseCRS() const {
    return NN_CHECK_ASSERT(util::nn_dynamic_pointer_cast<GeodeticCRS>(
        DerivedCRS::getPrivate()->baseCRS_));
}

// ---------------------------------------------------------------------------

/** \brief Instanciate a DerivedGeodeticCRS from a base CRS, a deriving
 * conversion and a cs::CartesianCS.
 *
 * @param properties See \ref general_properties.
 * At minimum the name should be defined.
 * @param baseCRSIn base CRS.
 * @param derivingConversionIn the deriving conversion from the base CRS to this
 * CRS.
 * @param csIn the coordinate system.
 * @return new DerivedGeodeticCRS.
 */
DerivedGeodeticCRSNNPtr DerivedGeodeticCRS::create(
    const util::PropertyMap &properties, const GeodeticCRSNNPtr &baseCRSIn,
    const operation::ConversionNNPtr &derivingConversionIn,
    const cs::CartesianCSNNPtr &csIn) {
    auto crs(DerivedGeodeticCRS::nn_make_shared<DerivedGeodeticCRS>(
        baseCRSIn, derivingConversionIn, csIn));
    crs->assignSelf(util::nn_static_pointer_cast<util::BaseObject>(crs));
    crs->setProperties(properties);
    return crs;
}

// ---------------------------------------------------------------------------

/** \brief Instanciate a DerivedGeodeticCRS from a base CRS, a deriving
 * conversion and a cs::SphericalCS.
 *
 * @param properties See \ref general_properties.
 * At minimum the name should be defined.
 * @param baseCRSIn base CRS.
 * @param derivingConversionIn the deriving conversion from the base CRS to this
 * CRS.
 * @param csIn the coordinate system.
 * @return new DerivedGeodeticCRS.
 */
DerivedGeodeticCRSNNPtr DerivedGeodeticCRS::create(
    const util::PropertyMap &properties, const GeodeticCRSNNPtr &baseCRSIn,
    const operation::ConversionNNPtr &derivingConversionIn,
    const cs::SphericalCSNNPtr &csIn) {
    auto crs(DerivedGeodeticCRS::nn_make_shared<DerivedGeodeticCRS>(
        baseCRSIn, derivingConversionIn, csIn));
    crs->assignSelf(util::nn_static_pointer_cast<util::BaseObject>(crs));
    crs->setProperties(properties);
    return crs;
}

// ---------------------------------------------------------------------------

std::string
DerivedGeodeticCRS::exportToWKT(io::WKTFormatterNNPtr formatter) const {
    const bool isWKT2 = formatter->version() == io::WKTFormatter::Version::WKT2;
    if (!isWKT2) {
        throw io::FormattingException(
            "DerivedGeodeticCRS can only be exported to WKT2");
    }
    formatter->startNode(io::WKTConstants::GEODCRS, !identifiers().empty());
    formatter->addQuotedString(*(name()->description()));

    formatter->startNode((formatter->use2018Keywords() &&
                          dynamic_cast<const GeographicCRS *>(baseCRS().get()))
                             ? io::WKTConstants::BASEGEOGCRS
                             : io::WKTConstants::BASEGEODCRS,
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
    io::PROJStringFormatterNNPtr) const // throw(io::FormattingException)
{
    throw io::FormattingException(
        "DerivedGeodeticCRS::exportToPROJString() unimplemented");
}

// ---------------------------------------------------------------------------

bool DerivedGeodeticCRS::isEquivalentTo(
    const util::BaseObjectNNPtr &other,
    util::IComparable::Criterion criterion) const {
    auto otherDerivedGeodCRS =
        util::nn_dynamic_pointer_cast<DerivedGeodeticCRS>(other);
    return otherDerivedGeodCRS != nullptr &&
           DerivedCRS::isEquivalentTo(other, criterion);
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
struct DerivedGeographicCRS::Private {};
//! @endcond

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
DerivedGeographicCRS::~DerivedGeographicCRS() = default;
//! @endcond

// ---------------------------------------------------------------------------

DerivedGeographicCRS::DerivedGeographicCRS(
    const GeodeticCRSNNPtr &baseCRSIn,
    const operation::ConversionNNPtr &derivingConversionIn,
    const cs::EllipsoidalCSNNPtr &csIn)
    : SingleCRS(baseCRSIn->datum(), csIn),
      GeographicCRS(baseCRSIn->datum(), csIn),
      DerivedCRS(baseCRSIn, derivingConversionIn, csIn),
      d(internal::make_unique<Private>()) {}

// ---------------------------------------------------------------------------

/** \brief Return the base CRS (a GeodeticCRS) of a DerivedGeographicCRS.
 *
 * @return the base CRS.
 */
const GeodeticCRSNNPtr DerivedGeographicCRS::baseCRS() const {
    return NN_CHECK_ASSERT(util::nn_dynamic_pointer_cast<GeodeticCRS>(
        DerivedCRS::getPrivate()->baseCRS_));
}

// ---------------------------------------------------------------------------

/** \brief Instanciate a DerivedGeographicCRS from a base CRS, a deriving
 * conversion and a cs::EllipsoidalCS.
 *
 * @param properties See \ref general_properties.
 * At minimum the name should be defined.
 * @param baseCRSIn base CRS.
 * @param derivingConversionIn the deriving conversion from the base CRS to this
 * CRS.
 * @param csIn the coordinate system.
 * @return new DerivedGeographicCRS.
 */
DerivedGeographicCRSNNPtr DerivedGeographicCRS::create(
    const util::PropertyMap &properties, const GeodeticCRSNNPtr &baseCRSIn,
    const operation::ConversionNNPtr &derivingConversionIn,
    const cs::EllipsoidalCSNNPtr &csIn) {
    auto crs(DerivedGeographicCRS::nn_make_shared<DerivedGeographicCRS>(
        baseCRSIn, derivingConversionIn, csIn));
    crs->assignSelf(util::nn_static_pointer_cast<util::BaseObject>(crs));
    crs->setProperties(properties);
    return crs;
}

// ---------------------------------------------------------------------------

std::string
DerivedGeographicCRS::exportToWKT(io::WKTFormatterNNPtr formatter) const {
    const bool isWKT2 = formatter->version() == io::WKTFormatter::Version::WKT2;
    if (!isWKT2) {
        throw io::FormattingException(
            "DerivedGeographicCRS can only be exported to WKT2");
    }
    formatter->startNode(formatter->use2018Keywords()
                             ? io::WKTConstants::GEOGCRS
                             : io::WKTConstants::GEODCRS,
                         !identifiers().empty());
    formatter->addQuotedString(*(name()->description()));

    formatter->startNode((formatter->use2018Keywords() &&
                          dynamic_cast<const GeographicCRS *>(baseCRS().get()))
                             ? io::WKTConstants::BASEGEOGCRS
                             : io::WKTConstants::BASEGEODCRS,
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
    io::PROJStringFormatterNNPtr) const // throw(io::FormattingException)
{
    throw io::FormattingException(
        "DerivedGeographicCRS::exportToPROJString() unimplemented");
}

// ---------------------------------------------------------------------------

bool DerivedGeographicCRS::isEquivalentTo(
    const util::BaseObjectNNPtr &other,
    util::IComparable::Criterion criterion) const {
    auto otherDerivedGeogCRS =
        util::nn_dynamic_pointer_cast<DerivedGeographicCRS>(other);
    return otherDerivedGeogCRS != nullptr &&
           DerivedCRS::isEquivalentTo(other, criterion);
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
struct TemporalCRS::Private {};
//! @endcond

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
TemporalCRS::~TemporalCRS() = default;
//! @endcond

// ---------------------------------------------------------------------------

TemporalCRS::TemporalCRS(const datum::TemporalDatumNNPtr &datumIn,
                         const cs::TemporalCSNNPtr &csIn)
    : SingleCRS(datumIn, csIn), d(internal::make_unique<Private>()) {}

// ---------------------------------------------------------------------------

/** \brief Return the datum::TemporalDatum associated with the CRS.
 *
 * @return a TemporalDatum
 */
const datum::TemporalDatumNNPtr TemporalCRS::datum() const {
    return NN_CHECK_ASSERT(std::dynamic_pointer_cast<datum::TemporalDatum>(
        SingleCRS::getPrivate()->datum));
}

// ---------------------------------------------------------------------------

/** \brief Return the cs::TemporalCS associated with the CRS.
 *
 * @return a TemporalCS
 */
const cs::TemporalCSNNPtr TemporalCRS::coordinateSystem() const {
    return NN_CHECK_ASSERT(util::nn_dynamic_pointer_cast<cs::TemporalCS>(
        SingleCRS::getPrivate()->coordinateSystem));
}

// ---------------------------------------------------------------------------

/** \brief Instanciate a TemporalCRS from a datum and a coordinate system.
 *
 * @param properties See \ref general_properties.
 * At minimum the name should be defined.
 * @param datumIn the datum.
 * @param csIn the coordinate system.
 * @return new TemporalCRS.
 */
TemporalCRSNNPtr TemporalCRS::create(const util::PropertyMap &properties,
                                     const datum::TemporalDatumNNPtr &datumIn,
                                     const cs::TemporalCSNNPtr &csIn) {
    auto crs(TemporalCRS::nn_make_shared<TemporalCRS>(datumIn, csIn));
    crs->assignSelf(util::nn_static_pointer_cast<util::BaseObject>(crs));
    crs->setProperties(properties);
    return crs;
}

// ---------------------------------------------------------------------------

std::string TemporalCRS::exportToWKT(io::WKTFormatterNNPtr formatter) const {
    const bool isWKT2 = formatter->version() == io::WKTFormatter::Version::WKT2;
    if (!isWKT2) {
        throw io::FormattingException(
            "TemporalCRS can only be exported to WKT2");
    }
    formatter->startNode(io::WKTConstants::TIMECRS, !identifiers().empty());
    formatter->addQuotedString(*(name()->description()));
    datum()->exportToWKT(formatter);
    coordinateSystem()->exportToWKT(formatter);
    ObjectUsage::_exportToWKT(formatter);
    formatter->endNode();
    return formatter->toString();
}

// ---------------------------------------------------------------------------

bool TemporalCRS::isEquivalentTo(const util::BaseObjectNNPtr &other,
                                 util::IComparable::Criterion criterion) const {
    auto otherTemporalCRS = util::nn_dynamic_pointer_cast<TemporalCRS>(other);
    return otherTemporalCRS != nullptr &&
           SingleCRS::_isEquivalentTo(other, criterion);
}

} // namespace crs
NS_PROJ_END
