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

#ifndef CRS_HH_INCLUDED
#define CRS_HH_INCLUDED

#include <memory>
#include <string>
#include <vector>

#include "common.hpp"
#include "coordinateoperation.hpp"
#include "coordinatesystem.hpp"
#include "datum.hpp"
#include "io.hpp"
#include "util.hpp"

NS_PROJ_START

/** osgeo.proj.crs namespace

    \brief CRS (coordinate reference system = coordinate system with a datum).
*/
namespace crs {

// ---------------------------------------------------------------------------

class GeographicCRS;
/** Shared pointer of GeographicCRS */
using GeographicCRSPtr = std::shared_ptr<GeographicCRS>;
/** Non-null shared pointer of GeographicCRS */
using GeographicCRSNNPtr = util::nn<GeographicCRSPtr>;

class VerticalCRS;
/** Shared pointer of VerticalCRS */
using VerticalCRSPtr = std::shared_ptr<VerticalCRS>;
/** Non-null shared pointer of VerticalCRS */
using VerticalCRSNNPtr = util::nn<VerticalCRSPtr>;

class BoundCRS;
/** Shared pointer of BoundCRS */
using BoundCRSPtr = std::shared_ptr<BoundCRS>;
/** Non-null shared pointer of BoundCRS */
using BoundCRSNNPtr = util::nn<BoundCRSPtr>;

// ---------------------------------------------------------------------------

class CRS;
/** Shared pointer of CRS */
using CRSPtr = std::shared_ptr<CRS>;
/** Non-null shared pointer of CRS */
using CRSNNPtr = util::nn<CRSPtr>;

/** \brief Abstract class modelling a coordinate reference system which is
 * usually single but may be compound.
 *
 * \remark Implements CRS from \ref ISO_19111_2018
 */
class CRS : public common::ObjectUsage, public io::IWKTExportable {
  public:
    //! @cond Doxygen_Suppress
    PROJ_DLL ~CRS() override;
    //! @endcond

    // Non-standard

    PROJ_DLL GeodeticCRSPtr extractGeodeticCRS() const;
    PROJ_DLL GeographicCRSPtr extractGeographicCRS() const;
    PROJ_DLL VerticalCRSPtr extractVerticalCRS() const;
    PROJ_DLL CRSNNPtr
    createBoundCRSToWGS84IfPossible(io::DatabaseContextPtr dbContext) const;
    PROJ_DLL CRSNNPtr stripVerticalComponent() const;

    /** \brief Return a shallow clone of this object. */
    PROJ_DLL virtual CRSNNPtr shallowClone() const = 0;

    PROJ_DLL const BoundCRSPtr &canonicalBoundCRS() const;

  protected:
    CRS();
    CRS(const CRS &other);
    friend class BoundCRS;
    void setCanonicalBoundCRS(const BoundCRSNNPtr &boundCRS);

  private:
    PROJ_OPAQUE_PRIVATE_DATA
};

// ---------------------------------------------------------------------------

/** \brief Abstract class modelling a coordinate reference system consisting of
 * one Coordinate System and either one datum::Datum or one
 * datum::DatumEnsemble.
 *
 * \remark Implements SingleCRS from \ref ISO_19111_2018
 */
class SingleCRS : public CRS {
  public:
    //! @cond Doxygen_Suppress
    PROJ_DLL ~SingleCRS() override;
    //! @endcond

    PROJ_DLL const datum::DatumPtr &datum() const;
    PROJ_DLL const datum::DatumEnsemblePtr &datumEnsemble() const;
    PROJ_DLL const cs::CoordinateSystemNNPtr &coordinateSystem() const;

    PROJ_PRIVATE :
        //! @cond Doxygen_Suppress
        void
        exportDatumOrDatumEnsembleToWkt(io::WKTFormatterNNPtr formatter)
            const; // throw(io::FormattingException)
                   //! @endcond

  protected:
    SingleCRS(const datum::DatumPtr &datumIn,
              const datum::DatumEnsemblePtr &datumEnsembleIn,
              const cs::CoordinateSystemNNPtr &csIn);
    SingleCRS(const SingleCRS &other);

    bool _isEquivalentTo(const util::IComparable *other,
                         util::IComparable::Criterion criterion =
                             util::IComparable::Criterion::STRICT) const;

  private:
    PROJ_OPAQUE_PRIVATE_DATA
    SingleCRS &operator=(const SingleCRS &other) = delete;
};

/** Shared pointer of SingleCRS */
using SingleCRSPtr = std::shared_ptr<SingleCRS>;
/** Non-null shared pointer of SingleCRS */
using SingleCRSNNPtr = util::nn<SingleCRSPtr>;

// ---------------------------------------------------------------------------

class GeodeticCRS;
/** Shared pointer of GeodeticCRS */
using GeodeticCRSPtr = std::shared_ptr<GeodeticCRS>;
/** Non-null shared pointer of GeodeticCRS */
using GeodeticCRSNNPtr = util::nn<GeodeticCRSPtr>;

/** \brief A coordinate reference system associated with a geodetic reference
 * frame and a three-dimensional Cartesian or spherical coordinate system.
 *
 * If the geodetic reference frame is dynamic or if the geodetic CRS has an
 * association to a velocity model then the geodetic CRS is dynamic, else it
 * is static.
 *
 * \remark Implements GeodeticCRS from \ref ISO_19111_2018
 */
class GeodeticCRS : virtual public SingleCRS, public io::IPROJStringExportable {
  public:
    //! @cond Doxygen_Suppress
    PROJ_DLL ~GeodeticCRS() override;
    //! @endcond

    PROJ_DLL const datum::GeodeticReferenceFramePtr &datum() const;

    PROJ_DLL const datum::PrimeMeridianNNPtr &primeMeridian() const;
    PROJ_DLL const datum::EllipsoidNNPtr &ellipsoid() const;

    // coordinateSystem() returns either a EllipsoidalCS, SphericalCS or
    // CartesianCS

    PROJ_DLL const std::vector<operation::PointMotionOperationNNPtr> &
    velocityModel() const;

    // Non-standard

    PROJ_DLL bool isGeocentric() const;

    PROJ_DLL static GeodeticCRSNNPtr
    create(const util::PropertyMap &properties,
           const datum::GeodeticReferenceFrameNNPtr &datum,
           const cs::SphericalCSNNPtr &cs);

    PROJ_DLL static GeodeticCRSNNPtr
    create(const util::PropertyMap &properties,
           const datum::GeodeticReferenceFrameNNPtr &datum,
           const cs::CartesianCSNNPtr &cs);

    PROJ_DLL static GeodeticCRSNNPtr
    create(const util::PropertyMap &properties,
           const datum::GeodeticReferenceFramePtr &datum,
           const datum::DatumEnsemblePtr &datumEnsemble,
           const cs::SphericalCSNNPtr &cs);

    PROJ_DLL static GeodeticCRSNNPtr
    create(const util::PropertyMap &properties,
           const datum::GeodeticReferenceFramePtr &datum,
           const datum::DatumEnsemblePtr &datumEnsemble,
           const cs::CartesianCSNNPtr &cs);

    PROJ_DLL std::string exportToWKT(io::WKTFormatterNNPtr formatter)
        const override; // throw(io::FormattingException)

    PROJ_DLL std::string
    exportToPROJString(io::PROJStringFormatterNNPtr formatter)
        const override; // throw(FormattingException)

    PROJ_DLL bool
    isEquivalentTo(const util::IComparable *other,
                   util::IComparable::Criterion criterion =
                       util::IComparable::Criterion::STRICT) const override;

    PROJ_DLL CRSNNPtr shallowClone() const override;

    PROJ_DLL static const GeodeticCRSNNPtr EPSG_4978; // WGS 84 Geocentric

    PROJ_PRIVATE :
        //! @cond Doxygen_Suppress
        void
        addDatumInfoToPROJString(io::PROJStringFormatterNNPtr formatter) const;
    void addGeocentricUnitConversionIntoPROJString(
        io::PROJStringFormatterNNPtr formatter) const;
    //! @endcond

  protected:
    GeodeticCRS(const datum::GeodeticReferenceFramePtr &datumIn,
                const datum::DatumEnsemblePtr &datumEnsembleIn,
                const cs::EllipsoidalCSNNPtr &csIn);
    GeodeticCRS(const datum::GeodeticReferenceFramePtr &datumIn,
                const datum::DatumEnsemblePtr &datumEnsembleIn,
                const cs::SphericalCSNNPtr &csIn);
    GeodeticCRS(const datum::GeodeticReferenceFramePtr &datumIn,
                const datum::DatumEnsemblePtr &datumEnsembleIn,
                const cs::CartesianCSNNPtr &csIn);
    GeodeticCRS(const GeodeticCRS &other);
    INLINED_MAKE_SHARED

    datum::GeodeticReferenceFrameNNPtr oneDatum() const;
    static GeodeticCRSNNPtr createEPSG_4978();

  private:
    PROJ_OPAQUE_PRIVATE_DATA

    GeodeticCRS &operator=(const GeodeticCRS &other) = delete;
};

// ---------------------------------------------------------------------------

/** \brief A coordinate reference system associated with a geodetic reference
 * frame and a two- or three-dimensional ellipsoidal coordinate system.
 *
 * If the geodetic reference frame is dynamic or if the geographic CRS has an
 * association to a velocity model then the geodetic CRS is dynamic, else it is
 * static.
 *
 * \remark Implements GeographicCRS from \ref ISO_19111_2018
 */
class GeographicCRS : public GeodeticCRS {
  public:
    //! @cond Doxygen_Suppress
    PROJ_DLL ~GeographicCRS() override;
    //! @endcond

    PROJ_DLL const cs::EllipsoidalCSNNPtr &coordinateSystem() const;

    // Non-standard
    PROJ_DLL static GeographicCRSNNPtr
    create(const util::PropertyMap &properties,
           const datum::GeodeticReferenceFrameNNPtr &datum,
           const cs::EllipsoidalCSNNPtr &cs);
    PROJ_DLL static GeographicCRSNNPtr
    create(const util::PropertyMap &properties,
           const datum::GeodeticReferenceFramePtr &datum,
           const datum::DatumEnsemblePtr &datumEnsemble,
           const cs::EllipsoidalCSNNPtr &cs);

    PROJ_DLL std::string
    exportToPROJString(io::PROJStringFormatterNNPtr formatter)
        const override; // throw(FormattingException)

    PROJ_DLL bool is2DPartOf3D(const GeographicCRSNNPtr &other) const;

    PROJ_DLL static const GeographicCRSNNPtr EPSG_4267; // NAD27
    PROJ_DLL static const GeographicCRSNNPtr EPSG_4269; // NAD83
    PROJ_DLL static const GeographicCRSNNPtr EPSG_4326; // WGS 84 2D
    PROJ_DLL static const GeographicCRSNNPtr OGC_CRS84; // CRS84 (Long, Lat)
    PROJ_DLL static const GeographicCRSNNPtr EPSG_4807; // NTF Paris
    PROJ_DLL static const GeographicCRSNNPtr EPSG_4979; // WGS 84 3D

    PROJ_DLL CRSNNPtr shallowClone() const override;

    PROJ_PRIVATE :
        //! @cond Doxygen_Suppress
        void
        addAngularUnitConvertAndAxisSwap(
            io::PROJStringFormatterNNPtr formatter) const;
    //! @endcond

  protected:
    GeographicCRS(const datum::GeodeticReferenceFramePtr &datumIn,
                  const datum::DatumEnsemblePtr &datumEnsembleIn,
                  const cs::EllipsoidalCSNNPtr &csIn);
    GeographicCRS(const GeographicCRS &other);
    INLINED_MAKE_SHARED

    static GeographicCRSNNPtr createEPSG_4267();
    static GeographicCRSNNPtr createEPSG_4269();
    static GeographicCRSNNPtr createEPSG_4326();
    static GeographicCRSNNPtr createOGC_CRS84();
    static GeographicCRSNNPtr createEPSG_4807();
    static GeographicCRSNNPtr createEPSG_4979();

  private:
    PROJ_OPAQUE_PRIVATE_DATA

    GeographicCRS &operator=(const GeographicCRS &other) = delete;
};

// ---------------------------------------------------------------------------

/** \brief A coordinate reference system having a vertical reference frame and
 * a one-dimensional vertical coordinate system used for recording
 * gravity-related heights or depths.
 *
 * Vertical CRSs make use of the direction of gravity to define the concept of
 * height or depth, but the relationship with gravity may not be
 * straightforward. If the vertical reference frame is dynamic or if the
 * vertical CRS has an association to a velocity model then the CRS is dynamic,
 * else it is static.
 *
 * \note Ellipsoidal heights cannot be captured in a vertical coordinate
 * reference system. They exist only as an inseparable part of a 3D coordinate
 * tuple defined in a geographic 3D coordinate reference system.
 *
 * \remark Implements VerticalCRS from \ref ISO_19111_2018
 */
class VerticalCRS : virtual public SingleCRS, public io::IPROJStringExportable {
  public:
    //! @cond Doxygen_Suppress
    PROJ_DLL ~VerticalCRS() override;
    //! @endcond

    PROJ_DLL const datum::VerticalReferenceFramePtr datum() const;
    PROJ_DLL const cs::VerticalCSNNPtr coordinateSystem() const;
    PROJ_DLL const std::vector<operation::TransformationNNPtr> &
    geoidModel() const;
    PROJ_DLL const std::vector<operation::PointMotionOperationNNPtr> &
    velocityModel() const;

    PROJ_DLL std::string exportToWKT(io::WKTFormatterNNPtr formatter)
        const override; // throw(io::FormattingException)

    PROJ_DLL std::string
    exportToPROJString(io::PROJStringFormatterNNPtr formatter)
        const override; // throw(FormattingException)

    PROJ_DLL bool
    isEquivalentTo(const util::IComparable *other,
                   util::IComparable::Criterion criterion =
                       util::IComparable::Criterion::STRICT) const override;

    PROJ_DLL static VerticalCRSNNPtr
    create(const util::PropertyMap &properties,
           const datum::VerticalReferenceFrameNNPtr &datumIn,
           const cs::VerticalCSNNPtr &csIn);

    PROJ_DLL static VerticalCRSNNPtr
    create(const util::PropertyMap &properties,
           const datum::VerticalReferenceFramePtr &datumIn,
           const datum::DatumEnsemblePtr &datumEnsembleIn,
           const cs::VerticalCSNNPtr &csIn);

    PROJ_DLL CRSNNPtr shallowClone() const override;

    PROJ_PRIVATE :
        //! @cond Doxygen_Suppress
        void
        addLinearUnitConvert(io::PROJStringFormatterNNPtr formatter) const;
    //! @endcond

  protected:
    VerticalCRS(const datum::VerticalReferenceFramePtr &datumIn,
                const datum::DatumEnsemblePtr &datumEnsembleIn,
                const cs::VerticalCSNNPtr &csIn);
    VerticalCRS(const VerticalCRS &other);
    INLINED_MAKE_SHARED

  private:
    PROJ_OPAQUE_PRIVATE_DATA
    VerticalCRS &operator=(const VerticalCRS &other) = delete;
};

// ---------------------------------------------------------------------------

/** \brief Abstract class modelling a single coordinate reference system that
 * is defined through the application of a specified coordinate conversion to
 * the definition of a previously established single coordinate reference
 * system referred to as the base CRS.
 *
 * A derived coordinate reference system inherits its datum (or datum ensemble)
 * from its base CRS. The coordinate conversion between the base and derived
 * coordinate reference system is implemented using the parameters and
 * formula(s) specified in the definition of the coordinate conversion.
 *
 * \remark Implements DerivedCRS from \ref ISO_19111_2018
 */
class DerivedCRS : virtual public SingleCRS {
  public:
    //! @cond Doxygen_Suppress
    PROJ_DLL ~DerivedCRS() override;
    //! @endcond

    PROJ_DLL const SingleCRSNNPtr &baseCRS() const;
    PROJ_DLL const operation::ConversionNNPtr derivingConversion() const;

    PROJ_DLL bool
    isEquivalentTo(const util::IComparable *other,
                   util::IComparable::Criterion criterion =
                       util::IComparable::Criterion::STRICT) const override;
    PROJ_PRIVATE :
        //! @cond Doxygen_Suppress
        const operation::ConversionNNPtr &
        derivingConversionRef() const;
    //! @endcond

  protected:
    DerivedCRS(const SingleCRSNNPtr &baseCRSIn,
               const operation::ConversionNNPtr &derivingConversionIn,
               const cs::CoordinateSystemNNPtr &cs);
    DerivedCRS(const DerivedCRS &other);

    void setDerivingConversionCRS();

  private:
    PROJ_OPAQUE_PRIVATE_DATA
    DerivedCRS &operator=(const DerivedCRS &other) = delete;
};

/** Shared pointer of DerivedCRS */
using DerivedCRSPtr = std::shared_ptr<DerivedCRS>;
/** Non-null shared pointer of DerivedCRS */
using DerivedCRSNNPtr = util::nn<DerivedCRSPtr>;

// ---------------------------------------------------------------------------

class ProjectedCRS;
/** Shared pointer of ProjectedCRS */
using ProjectedCRSPtr = std::shared_ptr<ProjectedCRS>;
/** Non-null shared pointer of ProjectedCRS */
using ProjectedCRSNNPtr = util::nn<ProjectedCRSPtr>;

/** \brief A derived coordinate reference system which has a geodetic
 * (usually geographic) coordinate reference system as its base CRS, thereby
 * inheriting a geodetic reference frame, and is converted using a map
 * projection.
 *
 * It has a Cartesian coordinate system, usually two-dimensional but may be
 * three-dimensional; in the 3D case the base geographic CRSs ellipsoidal
 * height is passed through unchanged and forms the vertical axis of the
 * projected CRS's Cartesian coordinate system.
 *
 * \remark Implements ProjectedCRS from \ref ISO_19111_2018
 */
class ProjectedCRS final : public DerivedCRS, public io::IPROJStringExportable {
  public:
    //! @cond Doxygen_Suppress
    PROJ_DLL ~ProjectedCRS() override;
    //! @endcond

    PROJ_DLL const GeodeticCRSNNPtr &baseCRS() const;
    PROJ_DLL const cs::CartesianCSNNPtr &coordinateSystem() const;

    PROJ_DLL std::string exportToWKT(io::WKTFormatterNNPtr formatter)
        const override; // throw(io::FormattingException)

    PROJ_DLL std::string
    exportToPROJString(io::PROJStringFormatterNNPtr formatter)
        const override; // throw(FormattingException)

    PROJ_DLL static ProjectedCRSNNPtr
    create(const util::PropertyMap &properties,
           const GeodeticCRSNNPtr &baseCRSIn,
           const operation::ConversionNNPtr &derivingConversionIn,
           const cs::CartesianCSNNPtr &csIn);

    PROJ_DLL bool
    isEquivalentTo(const util::IComparable *other,
                   util::IComparable::Criterion criterion =
                       util::IComparable::Criterion::STRICT) const override;

    PROJ_DLL CRSNNPtr shallowClone() const override;

    PROJ_PRIVATE :
        //! @cond Doxygen_Suppress
        void
        addUnitConvertAndAxisSwap(io::PROJStringFormatterNNPtr formatter,
                                  bool axisSpecFound) const;
    //! @endcond

  protected:
    ProjectedCRS(const GeodeticCRSNNPtr &baseCRSIn,
                 const operation::ConversionNNPtr &derivingConversionIn,
                 const cs::CartesianCSNNPtr &csIn);
    ProjectedCRS(const ProjectedCRS &other);
    INLINED_MAKE_SHARED

  private:
    PROJ_OPAQUE_PRIVATE_DATA
    ProjectedCRS &operator=(const ProjectedCRS &other) = delete;
};

// ---------------------------------------------------------------------------

class TemporalCRS;
/** Shared pointer of TemporalCRS */
using TemporalCRSPtr = std::shared_ptr<TemporalCRS>;
/** Non-null shared pointer of TemporalCRS */
using TemporalCRSNNPtr = util::nn<TemporalCRSPtr>;

/** \brief A coordinate reference system associated with a temporal datum and a
 * one-dimensional temporal coordinate system.
 *
 * \remark Implements TemporalCRS from \ref ISO_19111_2018
 */
class TemporalCRS : virtual public SingleCRS {
  public:
    //! @cond Doxygen_Suppress
    PROJ_DLL ~TemporalCRS() override;
    //! @endcond

    PROJ_DLL const datum::TemporalDatumNNPtr datum() const;

    PROJ_DLL const cs::TemporalCSNNPtr coordinateSystem() const;

    PROJ_DLL static TemporalCRSNNPtr
    create(const util::PropertyMap &properties,
           const datum::TemporalDatumNNPtr &datumIn,
           const cs::TemporalCSNNPtr &csIn);

    PROJ_DLL std::string exportToWKT(io::WKTFormatterNNPtr formatter)
        const override; // throw(io::FormattingException)

    PROJ_DLL bool
    isEquivalentTo(const util::IComparable *other,
                   util::IComparable::Criterion criterion =
                       util::IComparable::Criterion::STRICT) const override;

    PROJ_DLL CRSNNPtr shallowClone() const override;

  protected:
    TemporalCRS(const datum::TemporalDatumNNPtr &datumIn,
                const cs::TemporalCSNNPtr &csIn);
    TemporalCRS(const TemporalCRS &other);

    INLINED_MAKE_SHARED

  private:
    PROJ_OPAQUE_PRIVATE_DATA
    TemporalCRS &operator=(const TemporalCRS &other) = delete;
};

// ---------------------------------------------------------------------------

class EngineeringCRS;
/** Shared pointer of EngineeringCRS */
using EngineeringCRSPtr = std::shared_ptr<EngineeringCRS>;
/** Non-null shared pointer of EngineeringCRS */
using EngineeringCRSNNPtr = util::nn<EngineeringCRSPtr>;

/** \brief Contextually local coordinate reference system associated with an
 * engineering datum.
 *
 * It is applied either to activities on or near the surface of the Earth
 * without geodetic corrections, or on moving platforms such as road vehicles,
 * vessels, aircraft or spacecraft, or as the internal CRS of an image.
 *
 * In \ref WKT2, it maps to a ENGINEERINGCRS / ENGCRS keyword. In \ref WKT1,
 * it maps to a LOCAL_CS keyword.
 *
 * \remark Implements EngineeringCRS from \ref ISO_19111_2018
 */
class EngineeringCRS : virtual public SingleCRS {
  public:
    //! @cond Doxygen_Suppress
    PROJ_DLL ~EngineeringCRS() override;
    //! @endcond

    PROJ_DLL const datum::EngineeringDatumNNPtr datum() const;

    PROJ_DLL static EngineeringCRSNNPtr
    create(const util::PropertyMap &properties,
           const datum::EngineeringDatumNNPtr &datumIn,
           const cs::CoordinateSystemNNPtr &csIn);

    PROJ_DLL std::string exportToWKT(io::WKTFormatterNNPtr formatter)
        const override; // throw(io::FormattingException)

    PROJ_DLL bool
    isEquivalentTo(const util::IComparable *other,
                   util::IComparable::Criterion criterion =
                       util::IComparable::Criterion::STRICT) const override;

    PROJ_DLL CRSNNPtr shallowClone() const override;

  protected:
    EngineeringCRS(const datum::EngineeringDatumNNPtr &datumIn,
                   const cs::CoordinateSystemNNPtr &csIn);
    EngineeringCRS(const EngineeringCRS &other);

    INLINED_MAKE_SHARED

  private:
    PROJ_OPAQUE_PRIVATE_DATA
    EngineeringCRS &operator=(const EngineeringCRS &other) = delete;
};

// ---------------------------------------------------------------------------

class ParametricCRS;
/** Shared pointer of ParametricCRS */
using ParametricCRSPtr = std::shared_ptr<ParametricCRS>;
/** Non-null shared pointer of ParametricCRS */
using ParametricCRSNNPtr = util::nn<ParametricCRSPtr>;

/** \brief Contextually local coordinate reference system associated with an
 * engineering datum.
 *
 * This is applied either to activities on or near the surface of the Earth
 * without geodetic corrections, or on moving platforms such as road vehicles
 * vessels, aircraft or spacecraft, or as the internal CRS of an image.
 *
 * \remark Implements ParametricCRS from \ref ISO_19111_2018
 */
class ParametricCRS : virtual public SingleCRS {
  public:
    //! @cond Doxygen_Suppress
    PROJ_DLL ~ParametricCRS() override;
    //! @endcond

    PROJ_DLL const datum::ParametricDatumNNPtr datum() const;

    PROJ_DLL const cs::ParametricCSNNPtr coordinateSystem() const;

    PROJ_DLL static ParametricCRSNNPtr
    create(const util::PropertyMap &properties,
           const datum::ParametricDatumNNPtr &datumIn,
           const cs::ParametricCSNNPtr &csIn);

    PROJ_DLL std::string exportToWKT(io::WKTFormatterNNPtr formatter)
        const override; // throw(io::FormattingException)

    PROJ_DLL bool
    isEquivalentTo(const util::IComparable *other,
                   util::IComparable::Criterion criterion =
                       util::IComparable::Criterion::STRICT) const override;

    PROJ_DLL CRSNNPtr shallowClone() const override;

  protected:
    ParametricCRS(const datum::ParametricDatumNNPtr &datumIn,
                  const cs::ParametricCSNNPtr &csIn);
    ParametricCRS(const ParametricCRS &other);

    INLINED_MAKE_SHARED

  private:
    PROJ_OPAQUE_PRIVATE_DATA
    ParametricCRS &operator=(const ParametricCRS &other) = delete;
};

// ---------------------------------------------------------------------------

class CompoundCRS;
/** Shared pointer of CompoundCRS */
using CompoundCRSPtr = std::shared_ptr<CompoundCRS>;
/** Non-null shared pointer of CompoundCRS */
using CompoundCRSNNPtr = util::nn<CompoundCRSPtr>;

/** \brief A coordinate reference system describing the position of points
 * through two or more independent single coordinate reference systems.
 *
 * \note Two coordinate reference systems are independent of each other
 * if coordinate values in one cannot be converted or transformed into
 * coordinate values in the other.
 *
 * \note As a departure to \ref ISO_19111_2018, we allow to build a CompoundCRS
 * from CRS objects, whereas ISO19111:2018 restricts the components to
 * SingleCRS.
 * Thus nesting of CompoundCRS is possible. That said,
 * componentReferenceSystems()
 * will return a flattened view of the components.
 *
 * \remark Implements CompoundCRS from \ref ISO_19111_2018
 */
class CompoundCRS final : public CRS, public io::IPROJStringExportable {
  public:
    //! @cond Doxygen_Suppress
    PROJ_DLL ~CompoundCRS() override;
    //! @endcond

    PROJ_DLL std::vector<CRSNNPtr> componentReferenceSystems() const;

    PROJ_DLL std::string exportToWKT(io::WKTFormatterNNPtr formatter)
        const override; // throw(io::FormattingException)

    PROJ_DLL std::string
    exportToPROJString(io::PROJStringFormatterNNPtr formatter)
        const override; // throw(FormattingException)

    PROJ_DLL bool
    isEquivalentTo(const util::IComparable *other,
                   util::IComparable::Criterion criterion =
                       util::IComparable::Criterion::STRICT) const override;

    PROJ_DLL static CompoundCRSNNPtr
    create(const util::PropertyMap &properties,
           const std::vector<CRSNNPtr> &components);

    PROJ_DLL CRSNNPtr shallowClone() const override;

  protected:
    // relaxed: standard say SingleCRSNNPtr
    explicit CompoundCRS(const std::vector<CRSNNPtr> &components);
    CompoundCRS(const CompoundCRS &other);
    INLINED_MAKE_SHARED

  private:
    PROJ_OPAQUE_PRIVATE_DATA
    CompoundCRS &operator=(const CompoundCRS &other) = delete;
};

// ---------------------------------------------------------------------------

/** \brief A coordinate reference system with an associated transformation to
 * a target/hub CRS.
 *
 * The definition of a CRS is not dependent upon any relationship to an
 * independent CRS. However in an implementation that merges datasets
 * referenced to differing CRSs, it is sometimes useful to associate the
 * definition of the transformation that has been used with the CRS definition.
 * This facilitates the interrelationship of CRS by concatenating
 * transformations via a common or hub CRS. This is sometimes referred to as
 * "early-binding". \ref WKT2 permits the association of an abridged coordinate
 * transformation description with a coordinate reference system description in
 * a single text string. In a BoundCRS, the abridged coordinate transformation
 * is applied to the source CRS with the target CRS being the common or hub
 * system.
 *
 * Coordinates referring to a BoundCRS are expressed into its source/base CRS.
 *
 * This abstraction can for example model the concept of TOWGS84 datum shift
 * present in \ref WKT1.
 *
 * \note Contrary to other CRS classes of this package, there is no
 * \ref ISO_19111_2018 modelling of a BoundCRS.
 *
 * \remark Implements BoundCRS from \ref WKT2
 */
class BoundCRS final : public CRS, public io::IPROJStringExportable {
  public:
    //! @cond Doxygen_Suppress
    PROJ_DLL ~BoundCRS() override;
    //! @endcond

    PROJ_DLL const CRSNNPtr &baseCRS() const;
    PROJ_DLL CRSNNPtr baseCRSWithCanonicalBoundCRS() const;

    PROJ_DLL const CRSNNPtr &hubCRS() const;
    PROJ_DLL const operation::TransformationNNPtr &transformation() const;

    PROJ_DLL std::string exportToWKT(io::WKTFormatterNNPtr formatter)
        const override; // throw(io::FormattingException)

    PROJ_DLL std::string
    exportToPROJString(io::PROJStringFormatterNNPtr formatter)
        const override; // throw(FormattingException)

    PROJ_DLL bool
    isEquivalentTo(const util::IComparable *other,
                   util::IComparable::Criterion criterion =
                       util::IComparable::Criterion::STRICT) const override;

    PROJ_DLL static BoundCRSNNPtr
    create(const CRSNNPtr &baseCRSIn, const CRSNNPtr &hubCRSIn,
           const operation::TransformationNNPtr &transformationIn);

    PROJ_DLL static BoundCRSNNPtr
    createFromTOWGS84(const CRSNNPtr &baseCRSIn,
                      const std::vector<double> &TOWGS84Parameters);

    PROJ_DLL static BoundCRSNNPtr
    createFromNadgrids(const CRSNNPtr &baseCRSIn, const std::string &filename);

    PROJ_DLL CRSNNPtr shallowClone() const override;

  protected:
    BoundCRS(const CRSNNPtr &baseCRSIn, const CRSNNPtr &hubCRSIn,
             const operation::TransformationNNPtr &transformationIn);
    BoundCRS(const BoundCRS &other);
    INLINED_MAKE_SHARED

    BoundCRSNNPtr shallowCloneAsBoundCRS() const;
    bool isTOWGS84Compatible() const;
    std::string getHDatumPROJ4GRIDS() const;
    std::string getVDatumPROJ4GRIDS() const;

  private:
    PROJ_OPAQUE_PRIVATE_DATA
    BoundCRS &operator=(const BoundCRS &other) = delete;
};

// ---------------------------------------------------------------------------

class DerivedGeodeticCRS;
/** Shared pointer of DerivedGeodeticCRS */
using DerivedGeodeticCRSPtr = std::shared_ptr<DerivedGeodeticCRS>;
/** Non-null shared pointer of DerivedGeodeticCRS */
using DerivedGeodeticCRSNNPtr = util::nn<DerivedGeodeticCRSPtr>;

/** \brief A derived coordinate reference system which has either a geodetic
 * or a geographic coordinate reference system as its base CRS, thereby
 * inheriting a geodetic reference frame, and associated with a 3D Cartesian
 * or spherical coordinate system.
 *
 * \remark Implements DerivedGeodeticCRS from \ref ISO_19111_2018
 */
class DerivedGeodeticCRS final : public GeodeticCRS, public DerivedCRS {
  public:
    //! @cond Doxygen_Suppress
    PROJ_DLL ~DerivedGeodeticCRS() override;
    //! @endcond

    PROJ_DLL const GeodeticCRSNNPtr baseCRS() const;

    PROJ_DLL static DerivedGeodeticCRSNNPtr
    create(const util::PropertyMap &properties,
           const GeodeticCRSNNPtr &baseCRSIn,
           const operation::ConversionNNPtr &derivingConversionIn,
           const cs::CartesianCSNNPtr &csIn);

    PROJ_DLL static DerivedGeodeticCRSNNPtr
    create(const util::PropertyMap &properties,
           const GeodeticCRSNNPtr &baseCRSIn,
           const operation::ConversionNNPtr &derivingConversionIn,
           const cs::SphericalCSNNPtr &csIn);

    PROJ_DLL std::string exportToWKT(io::WKTFormatterNNPtr formatter)
        const override; // throw(io::FormattingException)

    PROJ_DLL std::string
    exportToPROJString(io::PROJStringFormatterNNPtr formatter)
        const override; // throw(FormattingException)

    PROJ_DLL bool
    isEquivalentTo(const util::IComparable *other,
                   util::IComparable::Criterion criterion =
                       util::IComparable::Criterion::STRICT) const override;

    PROJ_DLL CRSNNPtr shallowClone() const override;

  protected:
    DerivedGeodeticCRS(const GeodeticCRSNNPtr &baseCRSIn,
                       const operation::ConversionNNPtr &derivingConversionIn,
                       const cs::CartesianCSNNPtr &csIn);
    DerivedGeodeticCRS(const GeodeticCRSNNPtr &baseCRSIn,
                       const operation::ConversionNNPtr &derivingConversionIn,
                       const cs::SphericalCSNNPtr &csIn);
    DerivedGeodeticCRS(const DerivedGeodeticCRS &other);
    INLINED_MAKE_SHARED

  private:
    PROJ_OPAQUE_PRIVATE_DATA
    DerivedGeodeticCRS &operator=(const DerivedGeodeticCRS &other) = delete;
};

// ---------------------------------------------------------------------------

class DerivedGeographicCRS;
/** Shared pointer of DerivedGeographicCRS */
using DerivedGeographicCRSPtr = std::shared_ptr<DerivedGeographicCRS>;
/** Non-null shared pointer of DerivedGeographicCRS */
using DerivedGeographicCRSNNPtr = util::nn<DerivedGeographicCRSPtr>;

/** \brief A derived coordinate reference system which has either a geodetic or
 * a geographic coordinate reference system as its base CRS, thereby inheriting
 * a geodetic reference frame, and an ellipsoidal coordinate system.
 *
 * A derived geographic CRS can be based on a geodetic CRS only if that
 * geodetic CRS definition includes an ellipsoid.
 *
 * \remark Implements DerivedGeographicCRS from \ref ISO_19111_2018
 */
class DerivedGeographicCRS final : public GeographicCRS, public DerivedCRS {
  public:
    //! @cond Doxygen_Suppress
    PROJ_DLL ~DerivedGeographicCRS() override;
    //! @endcond

    PROJ_DLL const GeodeticCRSNNPtr baseCRS() const;

    PROJ_DLL static DerivedGeographicCRSNNPtr
    create(const util::PropertyMap &properties,
           const GeodeticCRSNNPtr &baseCRSIn,
           const operation::ConversionNNPtr &derivingConversionIn,
           const cs::EllipsoidalCSNNPtr &csIn);

    PROJ_DLL std::string exportToWKT(io::WKTFormatterNNPtr formatter)
        const override; // throw(io::FormattingException)

    PROJ_DLL std::string
    exportToPROJString(io::PROJStringFormatterNNPtr formatter)
        const override; // throw(FormattingException)

    PROJ_DLL bool
    isEquivalentTo(const util::IComparable *other,
                   util::IComparable::Criterion criterion =
                       util::IComparable::Criterion::STRICT) const override;

    PROJ_DLL CRSNNPtr shallowClone() const override;

  protected:
    DerivedGeographicCRS(const GeodeticCRSNNPtr &baseCRSIn,
                         const operation::ConversionNNPtr &derivingConversionIn,
                         const cs::EllipsoidalCSNNPtr &csIn);
    DerivedGeographicCRS(const DerivedGeographicCRS &other);

    INLINED_MAKE_SHARED

  private:
    PROJ_OPAQUE_PRIVATE_DATA
    DerivedGeographicCRS &operator=(const DerivedGeographicCRS &other) = delete;
};

// ---------------------------------------------------------------------------

class DerivedProjectedCRS;
/** Shared pointer of DerivedProjectedCRS */
using DerivedProjectedCRSPtr = std::shared_ptr<DerivedProjectedCRS>;
/** Non-null shared pointer of DerivedProjectedCRS */
using DerivedProjectedCRSNNPtr = util::nn<DerivedProjectedCRSPtr>;

/** \brief A derived coordinate reference system which has a projected
 * coordinate reference system as its base CRS, thereby inheriting a geodetic
 * reference frame, but also inheriting the distortion characteristics of the
 * base projected CRS.
 *
 * A DerivedProjectedCRS is not a ProjectedCRS.
 *
 * \remark Implements DerivedProjectedCRS from \ref ISO_19111_2018
 */
class DerivedProjectedCRS final : public DerivedCRS,
                                  public io::IPROJStringExportable {
  public:
    //! @cond Doxygen_Suppress
    PROJ_DLL ~DerivedProjectedCRS() override;
    //! @endcond

    PROJ_DLL const ProjectedCRSNNPtr baseCRS() const;

    PROJ_DLL static DerivedProjectedCRSNNPtr
    create(const util::PropertyMap &properties,
           const ProjectedCRSNNPtr &baseCRSIn,
           const operation::ConversionNNPtr &derivingConversionIn,
           const cs::CoordinateSystemNNPtr &csIn);

    PROJ_DLL std::string exportToWKT(io::WKTFormatterNNPtr formatter)
        const override; // throw(io::FormattingException)

    PROJ_DLL std::string
    exportToPROJString(io::PROJStringFormatterNNPtr formatter)
        const override; // throw(FormattingException)

    PROJ_DLL bool
    isEquivalentTo(const util::IComparable *other,
                   util::IComparable::Criterion criterion =
                       util::IComparable::Criterion::STRICT) const override;

    PROJ_DLL CRSNNPtr shallowClone() const override;

  protected:
    DerivedProjectedCRS(const ProjectedCRSNNPtr &baseCRSIn,
                        const operation::ConversionNNPtr &derivingConversionIn,
                        const cs::CoordinateSystemNNPtr &csIn);
    DerivedProjectedCRS(const DerivedProjectedCRS &other);

    INLINED_MAKE_SHARED

  private:
    PROJ_OPAQUE_PRIVATE_DATA
    DerivedProjectedCRS &operator=(const DerivedProjectedCRS &other) = delete;
};

// ---------------------------------------------------------------------------

class DerivedVerticalCRS;
/** Shared pointer of DerivedVerticalCRS */
using DerivedVerticalCRSPtr = std::shared_ptr<DerivedVerticalCRS>;
/** Non-null shared pointer of DerivedVerticalCRS */
using DerivedVerticalCRSNNPtr = util::nn<DerivedVerticalCRSPtr>;

/** \brief A derived coordinate reference system which has a vertical
 * coordinate reference system as its base CRS, thereby inheriting a vertical
 * reference frame, and a vertical coordinate system.
 *
 * \remark Implements DerivedVerticalCRS from \ref ISO_19111_2018
 */
class DerivedVerticalCRS final : public VerticalCRS, public DerivedCRS {
  public:
    //! @cond Doxygen_Suppress
    PROJ_DLL ~DerivedVerticalCRS() override;
    //! @endcond

    PROJ_DLL const VerticalCRSNNPtr baseCRS() const;

    PROJ_DLL static DerivedVerticalCRSNNPtr
    create(const util::PropertyMap &properties,
           const VerticalCRSNNPtr &baseCRSIn,
           const operation::ConversionNNPtr &derivingConversionIn,
           const cs::VerticalCSNNPtr &csIn);

    PROJ_DLL std::string exportToWKT(io::WKTFormatterNNPtr formatter)
        const override; // throw(io::FormattingException)

    PROJ_DLL std::string
    exportToPROJString(io::PROJStringFormatterNNPtr formatter)
        const override; // throw(FormattingException)

    PROJ_DLL bool
    isEquivalentTo(const util::IComparable *other,
                   util::IComparable::Criterion criterion =
                       util::IComparable::Criterion::STRICT) const override;

    PROJ_DLL CRSNNPtr shallowClone() const override;

  protected:
    DerivedVerticalCRS(const VerticalCRSNNPtr &baseCRSIn,
                       const operation::ConversionNNPtr &derivingConversionIn,
                       const cs::VerticalCSNNPtr &csIn);
    DerivedVerticalCRS(const DerivedVerticalCRS &other);

    INLINED_MAKE_SHARED

  private:
    PROJ_OPAQUE_PRIVATE_DATA
    DerivedVerticalCRS &operator=(const DerivedVerticalCRS &other) = delete;
};

// ---------------------------------------------------------------------------

/** \brief Template representing a derived coordinate reference system.
 */
template <class DerivedCRSTraits>
class DerivedCRSTemplate final : public DerivedCRSTraits::BaseType,
                                 public DerivedCRS {
  protected:
    /** Base type */
    typedef typename DerivedCRSTraits::BaseType BaseType;
    /** CSType */
    typedef typename DerivedCRSTraits::CSType CSType;

  public:
    //! @cond Doxygen_Suppress
    PROJ_DLL ~DerivedCRSTemplate() override;
    //! @endcond

    /** Non-null shared pointer of DerivedCRSTemplate */
    typedef typename util::nn<std::shared_ptr<DerivedCRSTemplate>> NNPtr;
    /** Non-null shared pointer of BaseType */
    typedef util::nn<std::shared_ptr<BaseType>> BaseNNPtr;
    /** Non-null shared pointer of CSType */
    typedef util::nn<std::shared_ptr<CSType>> CSNNPtr;

    /** \brief Return the base CRS of a DerivedCRSTemplate.
    *
    * @return the base CRS.
    */
    PROJ_DLL const BaseNNPtr baseCRS() const;

    /** \brief Instanciate a DerivedCRSTemplate from a base CRS, a deriving
    * conversion and a cs::CoordinateSystem.
    *
    * @param properties See \ref general_properties.
    * At minimum the name should be defined.
    * @param baseCRSIn base CRS.
    * @param derivingConversionIn the deriving conversion from the base CRS to
    * this
    * CRS.
    * @param csIn the coordinate system.
    * @return new DerivedCRSTemplate.
    */
    PROJ_DLL static NNPtr
    create(const util::PropertyMap &properties, const BaseNNPtr &baseCRSIn,
           const operation::ConversionNNPtr &derivingConversionIn,
           const CSNNPtr &csIn);

    //! @cond Doxygen_Suppress
    PROJ_DLL std::string exportToWKT(io::WKTFormatterNNPtr formatter)
        const override; // throw(io::FormattingException)

    PROJ_DLL bool
    isEquivalentTo(const util::IComparable *other,
                   util::IComparable::Criterion criterion =
                       util::IComparable::Criterion::STRICT) const override;

    PROJ_DLL CRSNNPtr shallowClone() const override;
    //! @endcond

  protected:
    DerivedCRSTemplate(const BaseNNPtr &baseCRSIn,
                       const operation::ConversionNNPtr &derivingConversionIn,
                       const CSNNPtr &csIn);
    DerivedCRSTemplate(const DerivedCRSTemplate &other);

    INLINED_MAKE_SHARED

  private:
    PROJ_OPAQUE_PRIVATE_DATA
    DerivedCRSTemplate &operator=(const DerivedCRSTemplate &other) = delete;
};

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
struct DerivedEngineeringCRSTraits {
    typedef EngineeringCRS BaseType;
    typedef cs::CoordinateSystem CSType;
    // old x86_64-w64-mingw32-g++ has issues with static variables. use method
    // instead
    inline static const std::string CRSName();
    inline static const std::string WKTKeyword();
    inline static const std::string WKTBaseKeyword();
    static const bool wkt2_2018_only = true;
};
//! @endcond

/** \brief A derived coordinate reference system which has an engineering
 * coordinate reference system as its base CRS, thereby inheriting an
 * engineering datum, and is associated with one of the coordinate system
 * types for an EngineeringCRS
 *
 * \remark Implements DerivedEngineeringCRS from \ref ISO_19111_2018
 */
#ifdef DOXYGEN_ENABLED
class DerivedEngineeringCRS
    : public DerivedCRSTemplate<DerivedEngineeringCRSTraits> {};
#else
using DerivedEngineeringCRS = DerivedCRSTemplate<DerivedEngineeringCRSTraits>;
#endif

#ifndef DO_NOT_DEFINE_EXTERN_DERIVED_CRS_TEMPLATE
extern template class DerivedCRSTemplate<DerivedEngineeringCRSTraits>;
#endif

/** Shared pointer of DerivedEngineeringCRS */
using DerivedEngineeringCRSPtr = std::shared_ptr<DerivedEngineeringCRS>;
/** Non-null shared pointer of DerivedEngineeringCRS */
using DerivedEngineeringCRSNNPtr = util::nn<DerivedEngineeringCRSPtr>;

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
struct DerivedParametricCRSTraits {
    typedef ParametricCRS BaseType;
    typedef cs::ParametricCS CSType;
    // old x86_64-w64-mingw32-g++ has issues with static variables. use method
    // instead
    inline static const std::string CRSName();
    inline static const std::string WKTKeyword();
    inline static const std::string WKTBaseKeyword();
    static const bool wkt2_2018_only = false;
};
//! @endcond

/** \brief A derived coordinate reference system which has a parametric
 * coordinate reference system as its base CRS, thereby inheriting a parametric
 * datum, and a parametric coordinate system.
 *
 * \remark Implements DerivedParametricCRS from \ref ISO_19111_2018
 */
#ifdef DOXYGEN_ENABLED
class DerivedParametricCRS
    : public DerivedCRSTemplate<DerivedParametricCRSTraits> {};
#else
using DerivedParametricCRS = DerivedCRSTemplate<DerivedParametricCRSTraits>;
#endif

#ifndef DO_NOT_DEFINE_EXTERN_DERIVED_CRS_TEMPLATE
extern template class DerivedCRSTemplate<DerivedParametricCRSTraits>;
#endif

/** Shared pointer of DerivedParametricCRS */
using DerivedParametricCRSPtr = std::shared_ptr<DerivedParametricCRS>;
/** Non-null shared pointer of DerivedParametricCRS */
using DerivedParametricCRSNNPtr = util::nn<DerivedParametricCRSPtr>;

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
struct DerivedTemporalCRSTraits {
    typedef TemporalCRS BaseType;
    typedef cs::TemporalCS CSType;
    // old x86_64-w64-mingw32-g++ has issues with static variables. use method
    // instead
    inline static const std::string CRSName();
    inline static const std::string WKTKeyword();
    inline static const std::string WKTBaseKeyword();
    static const bool wkt2_2018_only = false;
};
//! @endcond

/** \brief A derived coordinate reference system which has a temporal
 * coordinate reference system as its base CRS, thereby inheriting a temporal
 * datum, and a temporal coordinate system.
 *
 * \remark Implements DerivedTemporalCRS from \ref ISO_19111_2018
 */
#ifdef DOXYGEN_ENABLED
class DerivedTemporalCRS : public DerivedCRSTemplate<DerivedTemporalCRSTraits> {
};
#else
using DerivedTemporalCRS = DerivedCRSTemplate<DerivedTemporalCRSTraits>;
#endif

#ifndef DO_NOT_DEFINE_EXTERN_DERIVED_CRS_TEMPLATE
extern template class DerivedCRSTemplate<DerivedTemporalCRSTraits>;
#endif

/** Shared pointer of DerivedTemporalCRS */
using DerivedTemporalCRSPtr = std::shared_ptr<DerivedTemporalCRS>;
/** Non-null shared pointer of DerivedTemporalCRS */
using DerivedTemporalCRSNNPtr = util::nn<DerivedTemporalCRSPtr>;

// ---------------------------------------------------------------------------

} // namespace crs

NS_PROJ_END

#endif //  CRS_HH_INCLUDED
