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

class CRS : public common::ObjectUsage, public io::IWKTExportable {
  public:
    PROJ_DLL virtual ~CRS();

  protected:
    CRS();
};

using CRSPtr = std::shared_ptr<CRS>;
using CRSNNPtr = util::nn<CRSPtr>;

// ---------------------------------------------------------------------------

class SingleCRS : public CRS {
  public:
    PROJ_DLL SingleCRS &operator=(const SingleCRS &other) = delete;

    PROJ_DLL ~SingleCRS();

    PROJ_DLL const datum::DatumPtr &datum() const;
    PROJ_DLL const datum::DatumEnsemblePtr &datumEnsemble() const;
    PROJ_DLL const cs::CoordinateSystemNNPtr &coordinateSystem() const;

  protected:
    SingleCRS(const datum::DatumNNPtr &datumIn,
              const cs::CoordinateSystemNNPtr &csIn);
    SingleCRS(const datum::DatumPtr &datumIn,
              const cs::CoordinateSystemNNPtr &csIn);
    SingleCRS(const SingleCRS &other);

  private:
    PROJ_OPAQUE_PRIVATE_DATA
};

using SingleCRSPtr = std::shared_ptr<SingleCRS>;
using SingleCRSNNPtr = util::nn<SingleCRSPtr>;

// ---------------------------------------------------------------------------

class GeodeticCRS;
using GeodeticCRSPtr = std::shared_ptr<GeodeticCRS>;
using GeodeticCRSNNPtr = util::nn<GeodeticCRSPtr>;

class GeodeticCRS : virtual public SingleCRS {
  public:
    PROJ_DLL GeodeticCRS(const GeodeticCRS &other);
    PROJ_DLL GeodeticCRS &operator=(const GeodeticCRS &other) = delete;
    PROJ_DLL ~GeodeticCRS();

    PROJ_DLL const datum::GeodeticReferenceFrameNNPtr datum() const;

    // coordinateSystem() returns either a EllipsoidalCS, SphericalCS or
    // CartesianCS

    PROJ_DLL const std::vector<operation::PointMotionOperation> &
    velocityModel() const;

    // Non-standard

    PROJ_DLL bool isGeocentric() const;

    PROJ_DLL static GeodeticCRSNNPtr
    create(const util::PropertyMap &properties,
           const datum::GeodeticReferenceFrameNNPtr &datum,
           const cs::EllipsoidalCSNNPtr &cs);
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
};

// ---------------------------------------------------------------------------

class GeographicCRS;
using GeographicCRSPtr = std::shared_ptr<GeographicCRS>;
using GeographicCRSNNPtr = util::nn<GeographicCRSPtr>;

class GeographicCRS : public GeodeticCRS {
  public:
    PROJ_DLL GeographicCRS(const GeographicCRS &other);
    PROJ_DLL GeographicCRS &operator=(const GeographicCRS &other) = delete;
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
};

// ---------------------------------------------------------------------------

class VerticalCRS : public SingleCRS {
  public:
    PROJ_DLL VerticalCRS(const VerticalCRS &other);
    PROJ_DLL VerticalCRS &operator=(const VerticalCRS &other) = delete;
    PROJ_DLL ~VerticalCRS();

    PROJ_DLL const datum::VerticalReferenceFramePtr datum() const;
    PROJ_DLL const cs::VerticalCSNNPtr coordinateSystem() const;
    PROJ_DLL const std::vector<operation::Transformation> &geoidModel() const;
    PROJ_DLL const std::vector<operation::PointMotionOperation> &
    velocityModel() const;

    PROJ_DLL std::string exportToWKT(io::WKTFormatterNNPtr formatter)
        const override; // throw(io::FormattingException)

  private:
    PROJ_OPAQUE_PRIVATE_DATA
};

using VerticalCRSPtr = std::shared_ptr<VerticalCRS>;
using VerticalCRSNNPtr = util::nn<VerticalCRSPtr>;

// ---------------------------------------------------------------------------

class DerivedCRS : virtual public SingleCRS {
  public:
    PROJ_DLL ~DerivedCRS();

    PROJ_DLL const SingleCRSNNPtr &baseCRS() const;
    PROJ_DLL const operation::ConversionNNPtr &derivingConversion() const;

  protected:
    DerivedCRS(const SingleCRSNNPtr &baseCRSIn,
               const operation::ConversionNNPtr &derivingConversionIn,
               const cs::CoordinateSystemNNPtr &cs);
    DerivedCRS(const DerivedCRS &other);
    DerivedCRS &operator=(const DerivedCRS &other) = delete;

  private:
    PROJ_OPAQUE_PRIVATE_DATA
};

using DerivedCRSPtr = std::shared_ptr<DerivedCRS>;
using DerivedCRSNNPtr = util::nn<DerivedCRSPtr>;

// ---------------------------------------------------------------------------

class ProjectedCRS;
using ProjectedCRSPtr = std::shared_ptr<ProjectedCRS>;
using ProjectedCRSNNPtr = util::nn<ProjectedCRSPtr>;

class ProjectedCRS : public DerivedCRS {
  public:
    PROJ_DLL ProjectedCRS &operator=(const ProjectedCRS &other) = delete;
    PROJ_DLL ~ProjectedCRS();

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
    ProjectedCRS(const ProjectedCRS &other);
    INLINED_MAKE_SHARED

  private:
    PROJ_OPAQUE_PRIVATE_DATA
};

// ---------------------------------------------------------------------------

class DerivedGeodeticCRS : public GeodeticCRS, public DerivedCRS {
    // TODO

    PROJ_DLL std::string exportToWKT(io::WKTFormatterNNPtr formatter)
        const override = 0; // throw(io::FormattingException)
};

// ---------------------------------------------------------------------------

class CompoundCRS : public CRS {
    // TODO
    // const std::vector<std::unique_ptr<SingleCRS>>&
    // componentReferenceSystems()
    // const;
};

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
