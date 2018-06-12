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

#ifndef DATUM_HH_INCLUDED
#define DATUM_HH_INCLUDED

#include <memory>
#include <string>
#include <vector>

#include "common.hpp"
#include "io.hpp"
#include "util.hpp"

NS_PROJ_START

/** osgeo.proj.datum namespace

    \brief Datum (the relationship of a coordinate system to the body).
 */
namespace datum {

// ---------------------------------------------------------------------------

class Datum : public common::ObjectUsage {
  public:
    PROJ_DLL ~Datum();

    PROJ_DLL const util::optional<std::string> &anchorDefinition() const;
    PROJ_DLL const util::optional<common::Date> &publicationDate() const;
    PROJ_DLL const util::optional<common::IdentifiedObject> &
    conventionalRS() const;

  protected:
    Datum();

#ifdef DOXYGEN_ENABLED
    std::string *anchorDefinition_;
    Date *publicationDate_;
    common::IdentifiedObject *conventionalRS_;
#endif

  private:
    friend class GeodeticReferenceFrame;
    friend class VerticalReferenceFrame;
    PROJ_OPAQUE_PRIVATE_DATA
    Datum &operator=(const Datum &other) = delete;
    Datum(const Datum &other) = delete;
};

using DatumPtr = std::shared_ptr<Datum>;
using DatumNNPtr = util::nn<DatumPtr>;

// ---------------------------------------------------------------------------

class DatumEnsemble : public common::IdentifiedObject {
  public:
    PROJ_DLL ~DatumEnsemble() override;

    PROJ_DLL const std::vector<DatumPtr> &datums() const;
    PROJ_DLL const metadata::PositionalAccuracyNNPtr &
    positionalAccuracy() const;

  protected:
#ifdef DOXYGEN_ENABLED
    Datum datums_[];
    PositionalAccuracy positionalAccuracy_
#endif

        private : PROJ_OPAQUE_PRIVATE_DATA
                  DatumEnsemble(const DatumEnsemble &other) = delete;
    DatumEnsemble &operator=(const DatumEnsemble &other) = delete;
};

using DatumEnsemblePtr = std::shared_ptr<DatumEnsemble>;

// ---------------------------------------------------------------------------

class PrimeMeridian;
using PrimeMeridianPtr = std::shared_ptr<PrimeMeridian>;
using PrimeMeridianNNPtr = util::nn<PrimeMeridianPtr>;

class PrimeMeridian : public common::IdentifiedObject,
                      public io::IWKTExportable {
  public:
    PROJ_DLL ~PrimeMeridian() override;

    PROJ_DLL const common::Angle &greenwichLongitude() const;

    // non-standard
    PROJ_DLL static PrimeMeridianNNPtr
    create(const util::PropertyMap &properties, const common::Angle &longitude);

    PROJ_DLL static const PrimeMeridianNNPtr GREENWICH;

    PROJ_DLL std::string exportToWKT(io::WKTFormatterNNPtr formatter)
        const override; // throw(io::FormattingException)

  protected:
#ifdef DOXYGEN_ENABLED
    Angle greenwichLongitude_;
#endif

    explicit PrimeMeridian(const common::Angle &angle = common::Angle());
    INLINED_MAKE_SHARED

  private:
    PROJ_OPAQUE_PRIVATE_DATA
    PrimeMeridian(const PrimeMeridian &other) = delete;
    PrimeMeridian &operator=(const PrimeMeridian &other) = delete;

    static const PrimeMeridianNNPtr createGREENWICH();
};

// ---------------------------------------------------------------------------

class Ellipsoid;
using EllipsoidPtr = std::shared_ptr<Ellipsoid>;
using EllipsoidNNPtr = util::nn<EllipsoidPtr>;

class Ellipsoid : public common::IdentifiedObject, public io::IWKTExportable {
  public:
    PROJ_DLL ~Ellipsoid() override;

    PROJ_DLL const common::Length &semiMajorAxis() const;

    // Inlined from SecondDefiningParameter union
    PROJ_DLL const util::optional<common::Scale> &inverseFlattening() const;
    PROJ_DLL const util::optional<common::Length> &semiMinorAxis() const;
    PROJ_DLL bool isSphere() const;

    PROJ_DLL const util::optional<common::Length> &semiMedianAxis() const;

    // non-standard
    PROJ_DLL static EllipsoidNNPtr
    createFlattenedSphere(const util::PropertyMap &properties,
                          const common::Length &semiMajorAxisIn,
                          const common::Scale &invFlattening);

    PROJ_DLL static const EllipsoidNNPtr EPSG_7030; // WGS 84

    PROJ_DLL std::string exportToWKT(io::WKTFormatterNNPtr formatter)
        const override; // throw(io::FormattingException)

  protected:
#ifdef DOXYGEN_ENABLED
    common::Length semiMajorAxis_;
    common::Scale *inverseFlattening_;
    common::Length *semiMinorAxis_;
    bool isSphere_;
    common::Length *semiMedianAxis_;
#endif

    Ellipsoid(const common::Length &semiMajorAxisIn,
              const common::Scale &invFlattening);
    INLINED_MAKE_SHARED

  private:
    PROJ_OPAQUE_PRIVATE_DATA
    Ellipsoid(const Ellipsoid &other) = delete;
    Ellipsoid &operator=(const Ellipsoid &other) = delete;

    static const EllipsoidNNPtr createEPSG_7030(); // WGS 84
};

// ---------------------------------------------------------------------------

class GeodeticReferenceFrame;
using GeodeticReferenceFramePtr = std::shared_ptr<GeodeticReferenceFrame>;
using GeodeticReferenceFrameNNPtr = util::nn<GeodeticReferenceFramePtr>;

class GeodeticReferenceFrame : public Datum, public io::IWKTExportable {
  public:
    PROJ_DLL ~GeodeticReferenceFrame() override;

    PROJ_DLL const PrimeMeridianPtr &primeMeridian() const;
    PROJ_DLL const EllipsoidPtr &ellipsoid() const;

    // non-standard
    PROJ_DLL static GeodeticReferenceFrameNNPtr
    create(const util::PropertyMap &properties, const EllipsoidNNPtr &ellipsoid,
           const util::optional<std::string> &anchor,
           const PrimeMeridianNNPtr &primeMeridian);

    PROJ_DLL static const GeodeticReferenceFrameNNPtr EPSG_6326; // WGS 84

    PROJ_DLL std::string exportToWKT(io::WKTFormatterNNPtr formatter)
        const override; // throw(io::FormattingException)

  protected:
#ifdef DOXYGEN_ENABLED
    PrimeMeridian primeMeridian_;
    Ellipsoid *ellipsoid_;
#endif

    GeodeticReferenceFrame();
    INLINED_MAKE_SHARED

  private:
    PROJ_OPAQUE_PRIVATE_DATA
    static const GeodeticReferenceFrameNNPtr createEPSG_6326();
    GeodeticReferenceFrame(const GeodeticReferenceFrame &other) = delete;
    GeodeticReferenceFrame &
    operator=(const GeodeticReferenceFrame &other) = delete;
};

// ---------------------------------------------------------------------------

class DynamicGeodeticReferenceFrame : public GeodeticReferenceFrame {
  public:
    PROJ_DLL ~DynamicGeodeticReferenceFrame() override;

    PROJ_DLL const common::Measure &frameReferenceEpoch() const;

  protected:
#ifdef DOXYGEN_ENABLED
    Measure frameReferenceEpoch_;
#endif

    DynamicGeodeticReferenceFrame();

  private:
    PROJ_OPAQUE_PRIVATE_DATA
    DynamicGeodeticReferenceFrame(const DynamicGeodeticReferenceFrame &other) =
        delete;
    DynamicGeodeticReferenceFrame &
    operator=(const DynamicGeodeticReferenceFrame &other) = delete;
};

// ---------------------------------------------------------------------------

class RealizationMethod : public util::CodeList {
  public:
    PROJ_DLL static const RealizationMethod LEVELLING;
    PROJ_DLL static const RealizationMethod GEOID;
    PROJ_DLL static const RealizationMethod TIDAL;

  private:
    friend class util::optional<RealizationMethod>;
    PROJ_DLL explicit RealizationMethod(
        const std::string &nameIn = std::string());
    PROJ_DLL RealizationMethod(const RealizationMethod &other);
    PROJ_DLL RealizationMethod &operator=(const RealizationMethod &other);
};

// ---------------------------------------------------------------------------

class VerticalReferenceFrame;
using VerticalReferenceFramePtr = std::shared_ptr<VerticalReferenceFrame>;
using VerticalReferenceFrameNNPtr = util::nn<VerticalReferenceFramePtr>;

class VerticalReferenceFrame : public Datum, public io::IWKTExportable {
  public:
    PROJ_DLL ~VerticalReferenceFrame() override;

    PROJ_DLL const util::optional<RealizationMethod> &realizationMethod() const;

    PROJ_DLL std::string exportToWKT(io::WKTFormatterNNPtr formatter)
        const override; // throw(io::FormattingException)

    // non-standard
    PROJ_DLL static VerticalReferenceFrameNNPtr
    create(const util::PropertyMap &properties,
           const util::optional<std::string> &anchor =
               util::optional<std::string>(),
           const util::optional<RealizationMethod> &realizationMethodIn =
               util::optional<RealizationMethod>());

  protected:
#ifdef DOXYGEN_ENABLED
    RealizationMethod realizationMethod_;
#endif

    VerticalReferenceFrame();
    INLINED_MAKE_SHARED

  private:
    PROJ_OPAQUE_PRIVATE_DATA
};

// ---------------------------------------------------------------------------

// TODO DynamicVerticalReferenceFrame

} // namespace datum

NS_PROJ_END

#endif //  DATUM_HH_INCLUDED
