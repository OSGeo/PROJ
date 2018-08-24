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
class CRS : public common::ObjectUsage,
            public io::IWKTExportable,
            public util::IComparable {
  public:
    //! @cond Doxygen_Suppress
    PROJ_DLL ~CRS() override;
    //! @endcond

    // Non-standard

    PROJ_DLL GeographicCRSPtr extractGeographicCRS() const;
    PROJ_DLL VerticalCRSPtr extractVerticalCRS() const;

  protected:
    CRS();

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

  protected:
    SingleCRS(const datum::DatumNNPtr &datumIn,
              const cs::CoordinateSystemNNPtr &csIn);
    SingleCRS(const datum::DatumPtr &datumIn,
              const cs::CoordinateSystemNNPtr &csIn);

    bool _isEquivalentTo(const util::BaseObjectNNPtr &other,
                         util::IComparable::Criterion criterion =
                             util::IComparable::Criterion::STRICT) const;

  private:
    PROJ_OPAQUE_PRIVATE_DATA
    SingleCRS &operator=(const SingleCRS &other) = delete;
    SingleCRS(const SingleCRS &other) = delete;
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

    PROJ_DLL const datum::GeodeticReferenceFrameNNPtr datum() const;

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

    PROJ_DLL std::string exportToWKT(io::WKTFormatterNNPtr formatter)
        const override; // throw(io::FormattingException)

    PROJ_DLL std::string
    exportToPROJString(io::PROJStringFormatterNNPtr formatter)
        const override; // throw(FormattingException)

    PROJ_DLL bool
    isEquivalentTo(const util::BaseObjectNNPtr &other,
                   util::IComparable::Criterion criterion =
                       util::IComparable::Criterion::STRICT) const override;

    //! @cond Doxygen_Suppress
    void addDatumInfoToPROJString(io::PROJStringFormatterNNPtr formatter) const;
    void addGeocentricUnitConversionIntoPROJString(
        io::PROJStringFormatterNNPtr formatter) const;
    //! @endcond

  protected:
    GeodeticCRS(const datum::GeodeticReferenceFrameNNPtr &datumIn,
                const cs::EllipsoidalCSNNPtr &csIn);
    GeodeticCRS(const datum::GeodeticReferenceFrameNNPtr &datumIn,
                const cs::SphericalCSNNPtr &csIn);
    GeodeticCRS(const datum::GeodeticReferenceFrameNNPtr &datumIn,
                const cs::CartesianCSNNPtr &csIn);
    INLINED_MAKE_SHARED

  private:
    PROJ_OPAQUE_PRIVATE_DATA
    GeodeticCRS(const GeodeticCRS &other) = delete;
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

    PROJ_DLL const cs::EllipsoidalCSNNPtr coordinateSystem() const;

    // Non-standard
    PROJ_DLL static GeographicCRSNNPtr
    create(const util::PropertyMap &properties,
           const datum::GeodeticReferenceFrameNNPtr &datum,
           const cs::EllipsoidalCSNNPtr &cs);

    PROJ_DLL std::string
    exportToPROJString(io::PROJStringFormatterNNPtr formatter)
        const override; // throw(FormattingException)

    PROJ_DLL bool is2DPartOf3D(const GeographicCRSNNPtr &other) const;

    PROJ_DLL static const GeographicCRSNNPtr EPSG_4269; // NAD83
    PROJ_DLL static const GeographicCRSNNPtr EPSG_4326; // WGS 84 2D
    PROJ_DLL static const GeographicCRSNNPtr EPSG_4807; // NTF Paris
    PROJ_DLL static const GeographicCRSNNPtr EPSG_4979; // WGS 84 3D

    //! @cond Doxygen_Suppress
    void addAngularUnitConvertAndAxisSwap(
        io::PROJStringFormatterNNPtr formatter) const;
    //! @endcond

  protected:
    GeographicCRS(const datum::GeodeticReferenceFrameNNPtr &datumIn,
                  const cs::EllipsoidalCSNNPtr &csIn);
    INLINED_MAKE_SHARED

  private:
    PROJ_OPAQUE_PRIVATE_DATA
    static GeographicCRSNNPtr createEPSG_4269();
    static GeographicCRSNNPtr createEPSG_4326();
    static GeographicCRSNNPtr createEPSG_4807();
    static GeographicCRSNNPtr createEPSG_4979();
    GeographicCRS(const GeographicCRS &other) = delete;
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
class VerticalCRS : public SingleCRS, public io::IPROJStringExportable {
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
    isEquivalentTo(const util::BaseObjectNNPtr &other,
                   util::IComparable::Criterion criterion =
                       util::IComparable::Criterion::STRICT) const override;

    PROJ_DLL static VerticalCRSNNPtr
    create(const util::PropertyMap &properties,
           const datum::VerticalReferenceFrameNNPtr &datumIn,
           const cs::VerticalCSNNPtr &csIn);

  protected:
    VerticalCRS(const datum::VerticalReferenceFrameNNPtr &datumIn,
                const cs::VerticalCSNNPtr &csIn);
    INLINED_MAKE_SHARED

  private:
    PROJ_OPAQUE_PRIVATE_DATA
    VerticalCRS(const VerticalCRS &other) = delete;
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
    PROJ_DLL const operation::ConversionNNPtr &derivingConversion() const;

    PROJ_DLL bool
    isEquivalentTo(const util::BaseObjectNNPtr &other,
                   util::IComparable::Criterion criterion =
                       util::IComparable::Criterion::STRICT) const override;

  protected:
    DerivedCRS(const SingleCRSNNPtr &baseCRSIn,
               const operation::ConversionNNPtr &derivingConversionIn,
               const cs::CoordinateSystemNNPtr &cs);

  private:
    PROJ_OPAQUE_PRIVATE_DATA
    DerivedCRS(const DerivedCRS &other) = delete;
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
class ProjectedCRS : public DerivedCRS, public io::IPROJStringExportable {
  public:
    //! @cond Doxygen_Suppress
    PROJ_DLL ~ProjectedCRS() override;
    //! @endcond

    PROJ_DLL const GeodeticCRSNNPtr baseCRS() const;
    PROJ_DLL const cs::CartesianCSNNPtr coordinateSystem() const;

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
    isEquivalentTo(const util::BaseObjectNNPtr &other,
                   util::IComparable::Criterion criterion =
                       util::IComparable::Criterion::STRICT) const override;

  protected:
    ProjectedCRS(const GeodeticCRSNNPtr &baseCRSIn,
                 const operation::ConversionNNPtr &derivingConversionIn,
                 const cs::CartesianCSNNPtr &csIn);
    INLINED_MAKE_SHARED

  private:
    PROJ_OPAQUE_PRIVATE_DATA
    ProjectedCRS(const ProjectedCRS &other) = delete;
    ProjectedCRS &operator=(const ProjectedCRS &other) = delete;
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
class CompoundCRS : public CRS, public io::IPROJStringExportable {
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
    isEquivalentTo(const util::BaseObjectNNPtr &other,
                   util::IComparable::Criterion criterion =
                       util::IComparable::Criterion::STRICT) const override;

    PROJ_DLL static CompoundCRSNNPtr
    create(const util::PropertyMap &properties,
           const std::vector<CRSNNPtr> &components);

  protected:
    // relaxed: standard say SingleCRSNNPtr
    CompoundCRS(const std::vector<CRSNNPtr> &components);
    CompoundCRS(const CompoundCRS &other) = delete;
    CompoundCRS &operator=(const CompoundCRS &other) = delete;
    INLINED_MAKE_SHARED

  private:
    PROJ_OPAQUE_PRIVATE_DATA
};

// ---------------------------------------------------------------------------

class BoundCRS;
/** Shared pointer of BoundCRS */
using BoundCRSPtr = std::shared_ptr<BoundCRS>;
/** Non-null shared pointer of BoundCRS */
using BoundCRSNNPtr = util::nn<BoundCRSPtr>;

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
class BoundCRS : public CRS, public io::IPROJStringExportable {
  public:
    //! @cond Doxygen_Suppress
    PROJ_DLL ~BoundCRS() override;
    //! @endcond

    PROJ_DLL const CRSNNPtr &baseCRS() const;
    PROJ_DLL const CRSNNPtr &hubCRS() const;
    PROJ_DLL const operation::TransformationNNPtr &transformation() const;

    PROJ_DLL std::string exportToWKT(io::WKTFormatterNNPtr formatter)
        const override; // throw(io::FormattingException)

    PROJ_DLL std::string
    exportToPROJString(io::PROJStringFormatterNNPtr formatter)
        const override; // throw(FormattingException)

    PROJ_DLL bool
    isEquivalentTo(const util::BaseObjectNNPtr &other,
                   util::IComparable::Criterion criterion =
                       util::IComparable::Criterion::STRICT) const override;

    PROJ_DLL static BoundCRSNNPtr
    create(const CRSNNPtr &baseCRSIn, const CRSNNPtr &hubCRSIn,
           const operation::TransformationNNPtr &transformationIn);

    PROJ_DLL static BoundCRSNNPtr
    createFromTOWGS84(const CRSNNPtr &baseCRSIn,
                      const std::vector<double> TOWGS84Parameters);

    PROJ_DLL static BoundCRSNNPtr
    createFromNadgrids(const CRSNNPtr &baseCRSIn, const std::string &filename);

  protected:
    BoundCRS(const CRSNNPtr &baseCRSIn, const CRSNNPtr &hubCRSIn,
             const operation::TransformationNNPtr &transformationIn);
    INLINED_MAKE_SHARED

    bool isTOWGS84Compatible() const;
    std::string getHDatumPROJ4GRIDS() const;
    std::string getVDatumPROJ4GRIDS() const;

  private:
    PROJ_OPAQUE_PRIVATE_DATA
    BoundCRS(const BoundCRS &other) = delete;
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
class DerivedGeodeticCRS : public GeodeticCRS, public DerivedCRS {
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
    isEquivalentTo(const util::BaseObjectNNPtr &other,
                   util::IComparable::Criterion criterion =
                       util::IComparable::Criterion::STRICT) const override;

  protected:
    DerivedGeodeticCRS(const GeodeticCRSNNPtr &baseCRSIn,
                       const operation::ConversionNNPtr &derivingConversionIn,
                       const cs::CartesianCSNNPtr &csIn);
    DerivedGeodeticCRS(const GeodeticCRSNNPtr &baseCRSIn,
                       const operation::ConversionNNPtr &derivingConversionIn,
                       const cs::SphericalCSNNPtr &csIn);
    INLINED_MAKE_SHARED

  private:
    PROJ_OPAQUE_PRIVATE_DATA
    DerivedGeodeticCRS(const DerivedGeodeticCRS &other) = delete;
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
class DerivedGeographicCRS : public GeographicCRS, public DerivedCRS {
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
    isEquivalentTo(const util::BaseObjectNNPtr &other,
                   util::IComparable::Criterion criterion =
                       util::IComparable::Criterion::STRICT) const override;

  protected:
    DerivedGeographicCRS(const GeodeticCRSNNPtr &baseCRSIn,
                         const operation::ConversionNNPtr &derivingConversionIn,
                         const cs::EllipsoidalCSNNPtr &csIn);

    INLINED_MAKE_SHARED

  private:
    PROJ_OPAQUE_PRIVATE_DATA
    DerivedGeographicCRS(const DerivedGeographicCRS &other) = delete;
    DerivedGeographicCRS &operator=(const DerivedGeographicCRS &other) = delete;
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
class TemporalCRS : public SingleCRS {
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
    isEquivalentTo(const util::BaseObjectNNPtr &other,
                   util::IComparable::Criterion criterion =
                       util::IComparable::Criterion::STRICT) const override;

  protected:
    TemporalCRS(const datum::TemporalDatumNNPtr &datumIn,
                const cs::TemporalCSNNPtr &csIn);

    INLINED_MAKE_SHARED

  private:
    PROJ_OPAQUE_PRIVATE_DATA
    TemporalCRS(const TemporalCRS &other) = delete;
    TemporalCRS &operator=(const TemporalCRS &other) = delete;
};

// ---------------------------------------------------------------------------

#ifdef notdef

// ---------------------------------------------------------------------------

class DerivedProjectedCRS : public DerivedCRS {
    // TODO
};

// ---------------------------------------------------------------------------

class EngineeringCRS : public SingleCRS {
    // TODO
};

// ---------------------------------------------------------------------------

class ParametricCRS : public SingleCRS {
    // TODO
};

// ---------------------------------------------------------------------------

class DerivedVerticalCRS : public VerticalCRS, public DerivedCRS {
    // TODO
};

// ---------------------------------------------------------------------------

class DerivedEngineeringCRS : public EngineeringCRS, public DerivedCRS {
    // TODO
};

// ---------------------------------------------------------------------------

class DerivedParametricCRS : public ParametricCRS, public DerivedCRS {
    // TODO
};

// ---------------------------------------------------------------------------

class DerivedTemporalCRS : public TemporalCRS, public DerivedCRS {
    // TODO
};

// ---------------------------------------------------------------------------

#endif

} // namespace crs

NS_PROJ_END

#endif //  CRS_HH_INCLUDED
