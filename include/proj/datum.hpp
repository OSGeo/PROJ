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
    PROJ_DLL Datum &operator=(const Datum &other) = delete;
    PROJ_DLL ~Datum();

    PROJ_DLL const util::optional<std::string> &anchorDefinition() const;
    PROJ_DLL const util::optional<common::Date> &publicationDate() const;
    PROJ_DLL const util::optional<common::IdentifiedObject> &
    conventionalRS() const;

  protected:
    Datum();
    Datum(const Datum &other);

#ifdef DOXYGEN_ENABLED
    std::string *anchorDefinition_;
    Date *publicationDate_;
    common::IdentifiedObject *conventionalRS_;
#endif

  private:
    friend class GeodeticReferenceFrame;
    PROJ_OPAQUE_PRIVATE_DATA
};

using DatumPtr = std::shared_ptr<Datum>;
using DatumNNPtr = util::nn<DatumPtr>;

// ---------------------------------------------------------------------------

class DatumEnsemble : public common::IdentifiedObject {
    PROJ_OPAQUE_PRIVATE_DATA

#ifdef DOXYGEN_ENABLED
  protected:
    Datum datums_[];
    PositionalAccuracy positionalAccuracy_
#endif

        public : PROJ_DLL
                 DatumEnsemble();
    PROJ_DLL DatumEnsemble(const DatumEnsemble &other);
    PROJ_DLL DatumEnsemble &operator=(const DatumEnsemble &other) = delete;
    PROJ_DLL ~DatumEnsemble();

    PROJ_DLL const std::vector<DatumPtr> &datums() const;
    PROJ_DLL const metadata::PositionalAccuracy &positionalAccuracy() const;
};

using DatumEnsemblePtr = std::shared_ptr<DatumEnsemble>;

// ---------------------------------------------------------------------------

class PrimeMeridian;
using PrimeMeridianPtr = std::shared_ptr<PrimeMeridian>;
using PrimeMeridianNNPtr = util::nn<PrimeMeridianPtr>;

class PrimeMeridian : public common::IdentifiedObject,
                      public io::IWKTExportable {
  public:
    PROJ_DLL PrimeMeridian(const PrimeMeridian &other);
    PROJ_DLL PrimeMeridian &operator=(const PrimeMeridian &other) = delete;
    PROJ_DLL ~PrimeMeridian();

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

    PrimeMeridian();
    INLINED_MAKE_SHARED

  private:
    PROJ_OPAQUE_PRIVATE_DATA

    static const PrimeMeridianNNPtr createGREENWICH();
};

// ---------------------------------------------------------------------------

class Ellipsoid;
using EllipsoidPtr = std::shared_ptr<Ellipsoid>;
using EllipsoidNNPtr = util::nn<EllipsoidPtr>;

class Ellipsoid : public common::IdentifiedObject, public io::IWKTExportable {
  public:
    PROJ_DLL Ellipsoid(const Ellipsoid &other);
    PROJ_DLL Ellipsoid &operator=(const Ellipsoid &other) = delete;
    PROJ_DLL ~Ellipsoid();

    PROJ_DLL const common::Length &semiMajorAxis() const;

    // Inlined from SecondDefiningParameter union
    PROJ_DLL const util::optional<common::Scale> &inverseFlattening() const;
    PROJ_DLL const util::optional<common::Length> &semiMinorAxis() const;
    PROJ_DLL bool isSphere() const;

    PROJ_DLL const util::optional<common::Length> &semiMedianAxis() const;

    // non-standard
    PROJ_DLL static EllipsoidNNPtr
    createFlattenedSphere(const util::PropertyMap &properties,
                          const common::Length &semiMajorAxis,
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

    Ellipsoid();
    INLINED_MAKE_SHARED

  private:
    PROJ_OPAQUE_PRIVATE_DATA

    static const EllipsoidNNPtr createEPSG_7030(); // WGS 84
};

// ---------------------------------------------------------------------------

class GeodeticReferenceFrame;
using GeodeticReferenceFramePtr = std::shared_ptr<GeodeticReferenceFrame>;
using GeodeticReferenceFrameNNPtr = util::nn<GeodeticReferenceFramePtr>;

class GeodeticReferenceFrame : public Datum, public io::IWKTExportable {
  public:
    PROJ_DLL GeodeticReferenceFrame(const GeodeticReferenceFrame &other);
    PROJ_DLL GeodeticReferenceFrame &
    operator=(const GeodeticReferenceFrame &other) = delete;
    PROJ_DLL ~GeodeticReferenceFrame();

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
};

// ---------------------------------------------------------------------------

class DynamicGeodeticReferenceFrame : public GeodeticReferenceFrame {
  public:
    PROJ_DLL DynamicGeodeticReferenceFrame();
    PROJ_DLL
    DynamicGeodeticReferenceFrame(const DynamicGeodeticReferenceFrame &other);
    PROJ_DLL DynamicGeodeticReferenceFrame &
    operator=(const DynamicGeodeticReferenceFrame &other) = delete;
    PROJ_DLL ~DynamicGeodeticReferenceFrame();

    PROJ_DLL const common::Measure &frameReferenceEpoch() const;

#ifdef DOXYGEN_ENABLED
  protected:
    Measure frameReferenceEpoch_;
#endif

  private:
    PROJ_OPAQUE_PRIVATE_DATA
};

// ---------------------------------------------------------------------------

class VerticalReferenceFrame : public Datum {
    // TODO
};

// ---------------------------------------------------------------------------

using VerticalReferenceFramePtr = std::shared_ptr<VerticalReferenceFrame>;

} // namespace datum

NS_PROJ_END

#endif //  DATUM_HH_INCLUDED
