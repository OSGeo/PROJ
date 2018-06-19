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

    \brief CRS (coordinate systems with a datum).
*/
namespace crs {

// ---------------------------------------------------------------------------

class GeographicCRS;
using GeographicCRSPtr = std::shared_ptr<GeographicCRS>;
using GeographicCRSNNPtr = util::nn<GeographicCRSPtr>;

// ---------------------------------------------------------------------------

class CRS : public common::ObjectUsage, public io::IWKTExportable {
  public:
    PROJ_DLL virtual ~CRS();

    PROJ_DLL static GeographicCRSPtr extractGeographicCRS(CRSNNPtr crs);

  protected:
    CRS();
};

using CRSPtr = std::shared_ptr<CRS>;
using CRSNNPtr = util::nn<CRSPtr>;

// ---------------------------------------------------------------------------

class SingleCRS : public CRS {
  public:
    PROJ_DLL ~SingleCRS() override;

    PROJ_DLL const datum::DatumPtr &datum() const;
    PROJ_DLL const datum::DatumEnsemblePtr &datumEnsemble() const;
    PROJ_DLL const cs::CoordinateSystemNNPtr &coordinateSystem() const;

  protected:
    SingleCRS(const datum::DatumNNPtr &datumIn,
              const cs::CoordinateSystemNNPtr &csIn);
    SingleCRS(const datum::DatumPtr &datumIn,
              const cs::CoordinateSystemNNPtr &csIn);

  private:
    PROJ_OPAQUE_PRIVATE_DATA
    SingleCRS &operator=(const SingleCRS &other) = delete;
    SingleCRS(const SingleCRS &other) = delete;
};

using SingleCRSPtr = std::shared_ptr<SingleCRS>;
using SingleCRSNNPtr = util::nn<SingleCRSPtr>;

// ---------------------------------------------------------------------------

class GeodeticCRS;
using GeodeticCRSPtr = std::shared_ptr<GeodeticCRS>;
using GeodeticCRSNNPtr = util::nn<GeodeticCRSPtr>;

class GeodeticCRS : virtual public SingleCRS {
  public:
    PROJ_DLL ~GeodeticCRS() override;

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

class GeographicCRS : public GeodeticCRS {
  public:
    PROJ_DLL ~GeographicCRS();

    PROJ_DLL const cs::EllipsoidalCSNNPtr coordinateSystem() const;

    // Non-standard
    PROJ_DLL static GeographicCRSNNPtr
    create(const util::PropertyMap &properties,
           const datum::GeodeticReferenceFrameNNPtr &datum,
           const cs::EllipsoidalCSNNPtr &cs);

    PROJ_DLL static const GeographicCRSNNPtr EPSG_4326; // WGS 84 2D
    PROJ_DLL static const GeographicCRSNNPtr EPSG_4807; // NTF Paris
    PROJ_DLL static const GeographicCRSNNPtr EPSG_4979; // WGS 84 3D

  protected:
    GeographicCRS(const datum::GeodeticReferenceFrameNNPtr &datumIn,
                  const cs::EllipsoidalCSNNPtr &csIn);
    INLINED_MAKE_SHARED

  private:
    PROJ_OPAQUE_PRIVATE_DATA
    static GeographicCRSNNPtr createEPSG_4326();
    static GeographicCRSNNPtr createEPSG_4807();
    static GeographicCRSNNPtr createEPSG_4979();
    GeographicCRS(const GeographicCRS &other) = delete;
    GeographicCRS &operator=(const GeographicCRS &other) = delete;
};

// ---------------------------------------------------------------------------

class VerticalCRS;
using VerticalCRSPtr = std::shared_ptr<VerticalCRS>;
using VerticalCRSNNPtr = util::nn<VerticalCRSPtr>;

class VerticalCRS : public SingleCRS {
  public:
    PROJ_DLL ~VerticalCRS() override;

    PROJ_DLL const datum::VerticalReferenceFramePtr datum() const;
    PROJ_DLL const cs::VerticalCSNNPtr coordinateSystem() const;
    PROJ_DLL const std::vector<operation::TransformationNNPtr> &
    geoidModel() const;
    PROJ_DLL const std::vector<operation::PointMotionOperationNNPtr> &
    velocityModel() const;

    PROJ_DLL std::string exportToWKT(io::WKTFormatterNNPtr formatter)
        const override; // throw(io::FormattingException)

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

class DerivedCRS : virtual public SingleCRS {
  public:
    PROJ_DLL ~DerivedCRS() override;

    PROJ_DLL const SingleCRSNNPtr &baseCRS() const;
    PROJ_DLL const operation::ConversionNNPtr &derivingConversion() const;

  protected:
    DerivedCRS(const SingleCRSNNPtr &baseCRSIn,
               const operation::ConversionNNPtr &derivingConversionIn,
               const cs::CoordinateSystemNNPtr &cs);

  private:
    PROJ_OPAQUE_PRIVATE_DATA
    DerivedCRS(const DerivedCRS &other) = delete;
    DerivedCRS &operator=(const DerivedCRS &other) = delete;
};

using DerivedCRSPtr = std::shared_ptr<DerivedCRS>;
using DerivedCRSNNPtr = util::nn<DerivedCRSPtr>;

// ---------------------------------------------------------------------------

class ProjectedCRS;
using ProjectedCRSPtr = std::shared_ptr<ProjectedCRS>;
using ProjectedCRSNNPtr = util::nn<ProjectedCRSPtr>;

class ProjectedCRS : public DerivedCRS {
  public:
    PROJ_DLL ~ProjectedCRS() override;

    PROJ_DLL const GeodeticCRSNNPtr baseCRS() const;
    PROJ_DLL const cs::CartesianCSNNPtr coordinateSystem() const;

    PROJ_DLL std::string exportToWKT(io::WKTFormatterNNPtr formatter)
        const override; // throw(io::FormattingException)

    PROJ_DLL static ProjectedCRSNNPtr
    create(const util::PropertyMap &properties,
           const GeodeticCRSNNPtr &baseCRSIn,
           const operation::ConversionNNPtr &derivingConversionIn,
           const cs::CartesianCSNNPtr &csIn);

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
using CompoundCRSPtr = std::shared_ptr<CompoundCRS>;
using CompoundCRSNNPtr = util::nn<CompoundCRSPtr>;

class CompoundCRS : public CRS {
  public:
    CompoundCRS &operator=(const CompoundCRS &other) = delete;
    PROJ_DLL ~CompoundCRS() override;

    PROJ_DLL std::vector<SingleCRSNNPtr> componentReferenceSystems() const;

    PROJ_DLL std::string exportToWKT(io::WKTFormatterNNPtr formatter)
        const override; // throw(io::FormattingException)

    PROJ_DLL static CompoundCRSNNPtr
    create(const util::PropertyMap &properties,
           const std::vector<CRSNNPtr> &components);

  protected:
    // relaxed: standard say SingleCRSNNPtr
    CompoundCRS(const std::vector<CRSNNPtr> &components);
    CompoundCRS(const CompoundCRS &other) = delete;
    INLINED_MAKE_SHARED

  private:
    PROJ_OPAQUE_PRIVATE_DATA
};

// ---------------------------------------------------------------------------

class BoundCRS;
using BoundCRSPtr = std::shared_ptr<BoundCRS>;
using BoundCRSNNPtr = util::nn<BoundCRSPtr>;

/* PROJ specific modelization: BoundCRS is WKT2 only, and not present in
 * ISO-19111 */
class BoundCRS : public CRS {
  public:
    PROJ_DLL ~BoundCRS() override;

    PROJ_DLL const CRSNNPtr &baseCRS() const;
    PROJ_DLL const CRSNNPtr &hubCRS() const;
    PROJ_DLL const operation::TransformationNNPtr &transformation() const;

    PROJ_DLL std::string exportToWKT(io::WKTFormatterNNPtr formatter)
        const override; // throw(io::FormattingException)

    PROJ_DLL static BoundCRSNNPtr
    create(const CRSNNPtr &baseCRSIn, const CRSNNPtr &hubCRSIn,
           const operation::TransformationNNPtr &transformationIn);

    PROJ_DLL static BoundCRSNNPtr
    createFromTOWGS84(const CRSNNPtr &baseCRSIn,
                      const std::vector<double> TOWGS84Parameters);

  protected:
    BoundCRS(const CRSNNPtr &baseCRSIn, const CRSNNPtr &hubCRSIn,
             const operation::TransformationNNPtr &transformationIn);
    INLINED_MAKE_SHARED

  private:
    PROJ_OPAQUE_PRIVATE_DATA
    BoundCRS(const BoundCRS &other) = delete;
    BoundCRS &operator=(const BoundCRS &other) = delete;
};

// ---------------------------------------------------------------------------

class DerivedGeodeticCRS;
using DerivedGeodeticCRSPtr = std::shared_ptr<DerivedGeodeticCRS>;
using DerivedGeodeticCRSNNPtr = util::nn<DerivedGeodeticCRSPtr>;

class DerivedGeodeticCRS : public GeodeticCRS, public DerivedCRS {
  public:
    PROJ_DLL ~DerivedGeodeticCRS() override;

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
using DerivedGeographicCRSPtr = std::shared_ptr<DerivedGeographicCRS>;
using DerivedGeographicCRSNNPtr = util::nn<DerivedGeographicCRSPtr>;

class DerivedGeographicCRS : public GeographicCRS, public DerivedCRS {
  public:
    PROJ_DLL ~DerivedGeographicCRS() override;

    PROJ_DLL const GeodeticCRSNNPtr baseCRS() const;

    PROJ_DLL static DerivedGeographicCRSNNPtr
    create(const util::PropertyMap &properties,
           const GeodeticCRSNNPtr &baseCRSIn,
           const operation::ConversionNNPtr &derivingConversionIn,
           const cs::EllipsoidalCSNNPtr &csIn);

    PROJ_DLL std::string exportToWKT(io::WKTFormatterNNPtr formatter)
        const override; // throw(io::FormattingException)

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

class TemporalCRS : public SingleCRS {
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
