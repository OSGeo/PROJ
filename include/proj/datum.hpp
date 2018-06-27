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

/** \brief Abstract class of the relationship of a coordinate system to an
 * object, thus creating a coordinate reference system.
 *
 * For geodetic and vertical coordinate reference systems, it relates a
 * coordinate system to the Earth (or the celestial body considered). With
 * other types of coordinate reference systems, the datum may relate the
 * coordinate system to another physical or
 * virtual object. A datum uses a parameter or set of parameters that determine
 * the location of the origin of the coordinate reference system. Each datum
 * subtype can be associated with only specific types of coordinate reference
 * systems.
 *
 * \remark Implements Datum from \ref ISO_19111_2018
 */
class Datum : public common::ObjectUsage {
  public:
    //! @cond Doxygen_Suppress
    PROJ_DLL ~Datum() override;
    //! @endcond

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

/** Shared pointer of Datum */
using DatumPtr = std::shared_ptr<Datum>;
/** Non-null shared pointer of Datum */
using DatumNNPtr = util::nn<DatumPtr>;

// ---------------------------------------------------------------------------

/** \brief A collection of two or more geodetic or vertical reference frames
 * (or if not geodetic or vertical reference frame, a collection of two or more
 * datums) which for all but the highest accuracy requirements may be
 * considered to be insignificantly different from each other.
 *
 * Every frame within the datum ensemble must be a realizations of the same
 * Terrestrial Reference System or Vertical Reference System.
 *
 * \remark Implements DatumEnsemble from \ref ISO_19111_2018
 */
class DatumEnsemble : public common::IdentifiedObject {
  public:
    //! @cond Doxygen_Suppress
    PROJ_DLL ~DatumEnsemble() override;
    //! @endcond

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

/** Shared pointer of DatumEnsemble */
using DatumEnsemblePtr = std::shared_ptr<DatumEnsemble>;
/** Non-null shared pointer of DatumEnsemble */
using DatumEnsembleNNPtr = util::nn<DatumEnsemblePtr>;

// ---------------------------------------------------------------------------

class PrimeMeridian;
/** Shared pointer of PrimeMeridian */
using PrimeMeridianPtr = std::shared_ptr<PrimeMeridian>;
/** Non-null shared pointer of PrimeMeridian */
using PrimeMeridianNNPtr = util::nn<PrimeMeridianPtr>;

/** \brief The origin meridian from which longitude values are determined.
 *
 * \note The default value for prime meridian name is "Greenwich". When the
 * default applies, the value for the longitude shall be 0 (degrees).
 *
 * \remark Implements PrimeMeridian from \ref ISO_19111_2018
 */
class PrimeMeridian : public common::IdentifiedObject,
                      public io::IWKTExportable,
                      public io::IPROJStringExportable {
  public:
    //! @cond Doxygen_Suppress
    PROJ_DLL ~PrimeMeridian() override;
    //! @endcond

    PROJ_DLL const common::Angle &longitude() const;

    // non-standard
    PROJ_DLL static PrimeMeridianNNPtr
    create(const util::PropertyMap &properties,
           const common::Angle &longitudeIn);

    PROJ_DLL static const PrimeMeridianNNPtr GREENWICH;
    PROJ_DLL static const PrimeMeridianNNPtr PARIS;

    PROJ_DLL std::string exportToWKT(io::WKTFormatterNNPtr formatter)
        const override; // throw(io::FormattingException)

    PROJ_DLL std::string
    exportToPROJString(io::PROJStringFormatterNNPtr formatter)
        const override; // throw(FormattingException)

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
    static const PrimeMeridianNNPtr createPARIS();
};

// ---------------------------------------------------------------------------

class Ellipsoid;
/** Shared pointer of Ellipsoid */
using EllipsoidPtr = std::shared_ptr<Ellipsoid>;
/** Non-null shared pointer of Ellipsoid */
using EllipsoidNNPtr = util::nn<EllipsoidPtr>;

/** \brief A geometric figure that can be used to describe the approximate
 * shape of an object.
 *
 * For the Earth an oblate biaxial ellipsoid is used: in mathematical terms,
 * it is a surface formed by the rotation of an ellipse about its minor axis.
 *
 * \remark Implements Ellipsoid from \ref ISO_19111_2018
 */
class Ellipsoid : public common::IdentifiedObject,
                  public io::IWKTExportable,
                  public io::IPROJStringExportable {
  public:
    //! @cond Doxygen_Suppress
    PROJ_DLL ~Ellipsoid() override;
    //! @endcond

    PROJ_DLL const common::Length &semiMajorAxis() const;

    // Inlined from SecondDefiningParameter union
    PROJ_DLL const util::optional<common::Scale> &inverseFlattening() const;
    PROJ_DLL const util::optional<common::Length> &semiMinorAxis() const;
    PROJ_DLL bool isSphere() const;

    PROJ_DLL const util::optional<common::Length> &semiMedianAxis() const;

    // non-standard

    PROJ_DLL common::Scale computeInverseFlattening() const;
    PROJ_DLL common::Length computeSemiMinorAxis() const;

    PROJ_DLL static EllipsoidNNPtr
    createSphere(const util::PropertyMap &properties,
                 const common::Length &radius);

    PROJ_DLL static EllipsoidNNPtr
    createFlattenedSphere(const util::PropertyMap &properties,
                          const common::Length &semiMajorAxisIn,
                          const common::Scale &invFlattening);

    PROJ_DLL static EllipsoidNNPtr
    createTwoAxis(const util::PropertyMap &properties,
                  const common::Length &semiMajorAxisIn,
                  const common::Length &semiMinorAxisIn);

    PROJ_DLL static const EllipsoidNNPtr WGS84;
    PROJ_DLL static const EllipsoidNNPtr GRS1980;

    PROJ_DLL std::string exportToWKT(io::WKTFormatterNNPtr formatter)
        const override; // throw(io::FormattingException)

    PROJ_DLL std::string
    exportToPROJString(io::PROJStringFormatterNNPtr formatter)
        const override; // throw(FormattingException)

  protected:
#ifdef DOXYGEN_ENABLED
    common::Length semiMajorAxis_;
    common::Scale *inverseFlattening_;
    common::Length *semiMinorAxis_;
    bool isSphere_;
    common::Length *semiMedianAxis_;
#endif

    Ellipsoid(const common::Length &radius);

    Ellipsoid(const common::Length &semiMajorAxisIn,
              const common::Scale &invFlattening);

    Ellipsoid(const common::Length &semiMajorAxisIn,
              const common::Length &semiMinorAxisIn);

    INLINED_MAKE_SHARED

  private:
    PROJ_OPAQUE_PRIVATE_DATA
    Ellipsoid(const Ellipsoid &other) = delete;
    Ellipsoid &operator=(const Ellipsoid &other) = delete;

    static const EllipsoidNNPtr createWGS84();
    static const EllipsoidNNPtr createGRS1980();
};

// ---------------------------------------------------------------------------

class GeodeticReferenceFrame;
/** Shared pointer of GeodeticReferenceFrame */
using GeodeticReferenceFramePtr = std::shared_ptr<GeodeticReferenceFrame>;
/** Non-null shared pointer of GeodeticReferenceFrame */
using GeodeticReferenceFrameNNPtr = util::nn<GeodeticReferenceFramePtr>;

/** \brief The definition of the position, scale and orientation of a geocentric
 * Cartesian 3D coordinate system relative to the Earth.
 *
 * It may also identify a defined ellipsoid (or sphere) that approximates
 * the shape of the Earth and which is centred on and aligned to this
 * geocentric coordinate system. Older geodetic datums define the location and
 * orientation of a defined ellipsoid (or sphere) that approximates the shape
 * of the earth.
 *
 * \note The terminology "Datum" is often used to mean a GeodeticReferenceFrame.
 *
 * \note In \ref ISO_19111_2007, this class was called GeodeticDatum.
 *
 * \remark Implements GeodeticReferenceFrame from \ref ISO_19111_2018
 */
class GeodeticReferenceFrame : public Datum, public io::IWKTExportable {
  public:
    //! @cond Doxygen_Suppress
    PROJ_DLL ~GeodeticReferenceFrame() override;
    //! @endcond

    PROJ_DLL const PrimeMeridianNNPtr &primeMeridian() const;

    // We constraint more than the standard into which the ellipsoid might
    // be omitted for a CRS with a non-ellipsoidal CS
    PROJ_DLL const EllipsoidNNPtr &ellipsoid() const;

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

    GeodeticReferenceFrame(const EllipsoidNNPtr &ellipsoidIn,
                           const PrimeMeridianNNPtr &primeMeridianIn);
    INLINED_MAKE_SHARED

  private:
    PROJ_OPAQUE_PRIVATE_DATA
    static const GeodeticReferenceFrameNNPtr createEPSG_6326();
    GeodeticReferenceFrame(const GeodeticReferenceFrame &other) = delete;
    GeodeticReferenceFrame &
    operator=(const GeodeticReferenceFrame &other) = delete;
};

// ---------------------------------------------------------------------------

class DynamicGeodeticReferenceFrame;
/** Shared pointer of DynamicGeodeticReferenceFrame */
using DynamicGeodeticReferenceFramePtr =
    std::shared_ptr<DynamicGeodeticReferenceFrame>;
/** Non-null shared pointer of DynamicGeodeticReferenceFrame */
using DynamicGeodeticReferenceFrameNNPtr =
    util::nn<DynamicGeodeticReferenceFramePtr>;

/** \brief A geodetic reference frame in which some of the parameters describe
 * time evolution of defining station coordinates.
 *
 * For example defining station coordinates having linear velocities to account
 * for crustal motion.
 *
 * \remark Implements DynamicGeodeticReferenceFrame from \ref ISO_19111_2018
 */
class DynamicGeodeticReferenceFrame : public GeodeticReferenceFrame {
  public:
    //! @cond Doxygen_Suppress
    PROJ_DLL ~DynamicGeodeticReferenceFrame() override;
    //! @endcond

    PROJ_DLL const common::Measure &frameReferenceEpoch() const;

  protected:
#ifdef DOXYGEN_ENABLED
    Measure frameReferenceEpoch_;
#endif

    DynamicGeodeticReferenceFrame(const EllipsoidNNPtr &ellipsoidIn,
                                  const PrimeMeridianNNPtr &primeMeridianIn);

  private:
    PROJ_OPAQUE_PRIVATE_DATA
    DynamicGeodeticReferenceFrame(const DynamicGeodeticReferenceFrame &other) =
        delete;
    DynamicGeodeticReferenceFrame &
    operator=(const DynamicGeodeticReferenceFrame &other) = delete;
};

// ---------------------------------------------------------------------------

/** \brief The specification of the method by which the vertical reference frame
 * is realized.
 *
 * \remark Implements RealizationMethod from \ref ISO_19111_2018
 */
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
/** Shared pointer of VerticalReferenceFrame */
using VerticalReferenceFramePtr = std::shared_ptr<VerticalReferenceFrame>;
/** Non-null shared pointer of VerticalReferenceFrame */
using VerticalReferenceFrameNNPtr = util::nn<VerticalReferenceFramePtr>;

/** \brief A textual description and/or a set of parameters identifying a
 * particular reference level surface used as a zero-height or zero-depth
 * surface, including its position with respect to the Earth.
 *
 * \note In \ref ISO_19111_2007, this class was called VerticalDatum.

 * \remark Implements VerticalReferenceFrame from \ref ISO_19111_2018
 */
class VerticalReferenceFrame : public Datum, public io::IWKTExportable {
  public:
    //! @cond Doxygen_Suppress
    PROJ_DLL ~VerticalReferenceFrame() override;
    //! @endcond

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

class TemporalDatum;
/** Shared pointer of TemporalDatum */
using TemporalDatumPtr = std::shared_ptr<TemporalDatum>;
/** Non-null shared pointer of TemporalDatum */
using TemporalDatumNNPtr = util::nn<TemporalDatumPtr>;

/** \brief The definition of the relationship of a temporal coordinate system
 * to an object. The object is normally time on the Earth.
 *
 * \remark Implements TemporalDatum from \ref ISO_19111_2018
 */
class TemporalDatum : public Datum, public io::IWKTExportable {
  public:
    //! @cond Doxygen_Suppress
    PROJ_DLL ~TemporalDatum() override;
    //! @endcond

    PROJ_DLL const common::DateTime &temporalOrigin() const;
    PROJ_DLL const std::string &calendar() const;

    PROJ_DLL std::string exportToWKT(io::WKTFormatterNNPtr formatter)
        const override; // throw(io::FormattingException)

    PROJ_DLL static const std::string CALENDAR_PROLEPTIC_GREGORIAN;

    // non-standard
    PROJ_DLL static TemporalDatumNNPtr
    create(const util::PropertyMap &properties,
           const common::DateTime &temporalOriginIn,
           const std::string &calendarIn);

  protected:
    TemporalDatum(const common::DateTime &temporalOriginIn,
                  const std::string &calendarIn);
    INLINED_MAKE_SHARED

  private:
    PROJ_OPAQUE_PRIVATE_DATA
};

// ---------------------------------------------------------------------------

// TODO DynamicVerticalReferenceFrame

} // namespace datum

NS_PROJ_END

#endif //  DATUM_HH_INCLUDED
