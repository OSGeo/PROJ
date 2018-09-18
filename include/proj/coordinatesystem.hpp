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

#ifndef CS_HH_INCLUDED
#define CS_HH_INCLUDED

#include <memory>
#include <set>
#include <string>
#include <vector>

#include "common.hpp"
#include "io.hpp"
#include "util.hpp"

NS_PROJ_START

/** osgeo.proj.cs namespace

    \brief Coordinate systems and their axis.
*/
namespace cs {

// ---------------------------------------------------------------------------

/** \brief The direction of positive increase in the coordinate value for a
 * coordinate system axis.
 *
 * \remark Implements AxisDirection from \ref ISO_19111_2018
 */
class AxisDirection : public util::CodeList {
  public:
    //! @cond Doxygen_Suppress
    PROJ_DLL static const AxisDirection *valueOf(const std::string &nameIn);
    PROJ_DLL static const std::set<std::string> &getKeys();
    //! @endcond

    PROJ_DLL static const AxisDirection NORTH;
    PROJ_DLL static const AxisDirection NORTH_NORTH_EAST;
    PROJ_DLL static const AxisDirection NORTH_EAST;
    PROJ_DLL static const AxisDirection EAST_NORTH_EAST;
    PROJ_DLL static const AxisDirection EAST;
    PROJ_DLL static const AxisDirection EAST_SOUTH_EAST;
    PROJ_DLL static const AxisDirection SOUTH_EAST;
    PROJ_DLL static const AxisDirection SOUTH_SOUTH_EAST;
    PROJ_DLL static const AxisDirection SOUTH;
    PROJ_DLL static const AxisDirection SOUTH_SOUTH_WEST;
    PROJ_DLL static const AxisDirection SOUTH_WEST;
    PROJ_DLL static const AxisDirection
        WEST_SOUTH_WEST; // note: was forgotten in WKT2-2015
    PROJ_DLL static const AxisDirection WEST;
    PROJ_DLL static const AxisDirection WEST_NORTH_WEST;
    PROJ_DLL static const AxisDirection NORTH_WEST;
    PROJ_DLL static const AxisDirection NORTH_NORTH_WEST;
    PROJ_DLL static const AxisDirection UP;
    PROJ_DLL static const AxisDirection DOWN;
    PROJ_DLL static const AxisDirection GEOCENTRIC_X;
    PROJ_DLL static const AxisDirection GEOCENTRIC_Y;
    PROJ_DLL static const AxisDirection GEOCENTRIC_Z;
    PROJ_DLL static const AxisDirection COLUMN_POSITIVE;
    PROJ_DLL static const AxisDirection COLUMN_NEGATIVE;
    PROJ_DLL static const AxisDirection ROW_POSITIVE;
    PROJ_DLL static const AxisDirection ROW_NEGATIVE;
    PROJ_DLL static const AxisDirection DISPLAY_RIGHT;
    PROJ_DLL static const AxisDirection DISPLAY_LEFT;
    PROJ_DLL static const AxisDirection DISPLAY_UP;
    PROJ_DLL static const AxisDirection DISPLAY_DOWN;
    PROJ_DLL static const AxisDirection FORWARD;
    PROJ_DLL static const AxisDirection AFT;
    PROJ_DLL static const AxisDirection PORT;
    PROJ_DLL static const AxisDirection STARBOARD;
    PROJ_DLL static const AxisDirection CLOCKWISE;
    PROJ_DLL static const AxisDirection COUNTER_CLOCKWISE;
    PROJ_DLL static const AxisDirection TOWARDS;
    PROJ_DLL static const AxisDirection AWAY_FROM;
    PROJ_DLL static const AxisDirection FUTURE;
    PROJ_DLL static const AxisDirection PAST;
    PROJ_DLL static const AxisDirection UNSPECIFIED;

  private:
    explicit AxisDirection(const std::string &nameIn);

    static std::map<std::string, const AxisDirection *> registry;
    static std::set<std::string> keys;
};

// ---------------------------------------------------------------------------

class Meridian;
/** Shared pointer of Meridian. */
using MeridianPtr = std::shared_ptr<Meridian>;
/** Non-null shared pointer of Meridian. */
using MeridianNNPtr = util::nn<MeridianPtr>;

/** \brief The meridian that the axis follows from the pole, for a coordinate
 * reference system centered on a pole.
 *
 * \note There is no modelling for this concept in \ref ISO_19111_2018
 *
 * \remark Implements MERIDIAN from \ref WKT2
 */
class Meridian : public common::IdentifiedObject, public io::IWKTExportable {
  public:
    //! @cond Doxygen_Suppress
    PROJ_DLL ~Meridian() override;
    //! @endcond

    PROJ_DLL const common::Angle &longitude() const;

    // non-standard
    PROJ_DLL static MeridianNNPtr create(const common::Angle &longitudeIn);

    PROJ_DLL std::string exportToWKT(io::WKTFormatterNNPtr formatter)
        const override; // throw(io::FormattingException)

  protected:
#ifdef DOXYGEN_ENABLED
    Angle angle_;
#endif

    explicit Meridian(const common::Angle &longitudeIn);
    INLINED_MAKE_SHARED

  private:
    PROJ_OPAQUE_PRIVATE_DATA
    Meridian(const Meridian &other) = delete;
    Meridian &operator=(const Meridian &other) = delete;
};

// ---------------------------------------------------------------------------

class CoordinateSystemAxis;
/** Shared pointer of CoordinateSystemAxis. */
using CoordinateSystemAxisPtr = std::shared_ptr<CoordinateSystemAxis>;
/** Non-null shared pointer of CoordinateSystemAxis. */
using CoordinateSystemAxisNNPtr = util::nn<CoordinateSystemAxisPtr>;

/** \brief The definition of a coordinate system axis.
 *
 * \remark Implements CoordinateSystemAxis from \ref ISO_19111_2018
 */
class CoordinateSystemAxis : public common::IdentifiedObject,
                             public io::IWKTExportable,
                             public util::IComparable {
  public:
    //! @cond Doxygen_Suppress
    PROJ_DLL ~CoordinateSystemAxis() override;
    //! @endcond

    PROJ_DLL const std::string &abbreviation() const;
    PROJ_DLL const AxisDirection &direction() const;
    PROJ_DLL const common::UnitOfMeasure &unit() const;
    PROJ_DLL const util::optional<double> &minimumValue() const;
    PROJ_DLL const util::optional<double> &maximumValue() const;
    PROJ_DLL const MeridianPtr &meridian() const;

    // Non-standard
    PROJ_DLL static CoordinateSystemAxisNNPtr
    create(const util::PropertyMap &properties,
           const std::string &abbreviationIn, const AxisDirection &directionIn,
           const common::UnitOfMeasure &unitIn,
           const MeridianPtr &meridianIn = nullptr);

    PROJ_DLL std::string exportToWKT(io::WKTFormatterNNPtr formatter)
        const override; // throw(io::FormattingException)

    PROJ_DLL bool
    isEquivalentTo(const util::BaseObjectNNPtr &other,
                   util::IComparable::Criterion criterion =
                       util::IComparable::Criterion::STRICT) const override;

    //! @cond Doxygen_Suppress
    std::string exportToWKT(io::WKTFormatterNNPtr formatter, int order,
                            bool disableAbbrev) const;

    static std::string normalizeAxisName(const std::string &str);
    //! @endcond

  private:
    PROJ_OPAQUE_PRIVATE_DATA
    CoordinateSystemAxis(const CoordinateSystemAxis &other) = delete;
    CoordinateSystemAxis &operator=(const CoordinateSystemAxis &other) = delete;

    CoordinateSystemAxis();
    INLINED_MAKE_SHARED
};

// ---------------------------------------------------------------------------

/** \brief Abstract class modelling a coordinate system (CS)
 *
 * A CS is the non-repeating sequence of coordinate system axes that spans a
 * given coordinate space. A CS is derived from a set of mathematical rules for
 * specifying how coordinates in a given space are to be assigned to points.
 * The coordinate values in a coordinate tuple shall be recorded in the order
 * in which the coordinate system axes associations are recorded.
 *
 * \remark Implements CoordinateSystem from \ref ISO_19111_2018
 */
class CoordinateSystem : public common::IdentifiedObject,
                         public io::IWKTExportable,
                         public util::IComparable {
  public:
    //! @cond Doxygen_Suppress
    PROJ_DLL ~CoordinateSystem() override;
    //! @endcond

    PROJ_DLL virtual const std::vector<CoordinateSystemAxisNNPtr> &
    axisList() const;

    PROJ_DLL std::string exportToWKT(io::WKTFormatterNNPtr formatter)
        const override; // throw(io::FormattingException)

    //! @cond Doxygen_Suppress
    PROJ_DLL virtual std::string getWKT2Type(io::WKTFormatterNNPtr) const = 0;
    //! @endcond

    PROJ_DLL bool
    isEquivalentTo(const util::BaseObjectNNPtr &other,
                   util::IComparable::Criterion criterion =
                       util::IComparable::Criterion::STRICT) const override;

  protected:
    CoordinateSystem();
    explicit CoordinateSystem(
        const std::vector<CoordinateSystemAxisNNPtr> &axisIn);

  private:
    PROJ_OPAQUE_PRIVATE_DATA
    CoordinateSystem(const CoordinateSystem &other) = delete;
    CoordinateSystem &operator=(const CoordinateSystem &other) = delete;
};

/** Shared pointer of CoordinateSystem. */
using CoordinateSystemPtr = std::shared_ptr<CoordinateSystem>;
/** Non-null shared pointer of CoordinateSystem. */
using CoordinateSystemNNPtr = util::nn<CoordinateSystemPtr>;

// ---------------------------------------------------------------------------

class SphericalCS;
/** Shared pointer of SphericalCS. */
using SphericalCSPtr = std::shared_ptr<SphericalCS>;
/** Non-null shared pointer of SphericalCS. */
using SphericalCSNNPtr = util::nn<SphericalCSPtr>;

/** \brief A three-dimensional coordinate system in Euclidean space with one
 * distance measured from the origin and two angular coordinates.
 *
 * Not to be confused with an ellipsoidal coordinate system based on an
 * ellipsoid "degenerated" into a sphere. A SphericalCS shall have three
 * axis associations.
 *
 * \remark Implements SphericalCS from \ref ISO_19111_2018
 */
class SphericalCS : public CoordinateSystem {
  public:
    //! @cond Doxygen_Suppress
    PROJ_DLL ~SphericalCS() override;
    //! @endcond

    // non-standard

    PROJ_DLL static SphericalCSNNPtr
    create(const util::PropertyMap &properties,
           const CoordinateSystemAxisNNPtr &axis1,
           const CoordinateSystemAxisNNPtr &axis2,
           const CoordinateSystemAxisNNPtr &axis3);

  protected:
    SphericalCS();
    explicit SphericalCS(const std::vector<CoordinateSystemAxisNNPtr> &axisIn);
    INLINED_MAKE_SHARED

    std::string getWKT2Type(io::WKTFormatterNNPtr) const override {
        return "spherical";
    }

  private:
    SphericalCS(const SphericalCS &other) = delete;
};

// ---------------------------------------------------------------------------

class EllipsoidalCS;
/** Shared pointer of EllipsoidalCS. */
using EllipsoidalCSPtr = std::shared_ptr<EllipsoidalCS>;
/** Non-null shared pointer of EllipsoidalCS. */
using EllipsoidalCSNNPtr = util::nn<EllipsoidalCSPtr>;

/** \brief A two- or three-dimensional coordinate system in which position is
 * specified by geodetic latitude, geodetic longitude, and (in the
 * three-dimensional case) ellipsoidal height.
 *
 * An EllipsoidalCS shall have two or three associations.
 *
 * \remark Implements EllipsoidalCS from \ref ISO_19111_2018
 */
class EllipsoidalCS : public CoordinateSystem {
  public:
    //! @cond Doxygen_Suppress
    PROJ_DLL ~EllipsoidalCS() override;
    //! @endcond

    // non-standard
    PROJ_DLL static EllipsoidalCSNNPtr
    create(const util::PropertyMap &properties,
           const CoordinateSystemAxisNNPtr &axis1,
           const CoordinateSystemAxisNNPtr &axis2);
    PROJ_DLL static EllipsoidalCSNNPtr
    create(const util::PropertyMap &properties,
           const CoordinateSystemAxisNNPtr &axis1,
           const CoordinateSystemAxisNNPtr &axis2,
           const CoordinateSystemAxisNNPtr &axis3);
    PROJ_DLL static EllipsoidalCSNNPtr
    createLatitudeLongitude(const common::UnitOfMeasure &unit);
    PROJ_DLL static EllipsoidalCSNNPtr createLatitudeLongitudeEllipsoidalHeight(
        const common::UnitOfMeasure &angularUnit,
        const common::UnitOfMeasure &linearUnit);

  protected:
    EllipsoidalCS();
    explicit EllipsoidalCS(
        const std::vector<CoordinateSystemAxisNNPtr> &axisIn);
    INLINED_MAKE_SHARED

    std::string getWKT2Type(io::WKTFormatterNNPtr) const override {
        return "ellipsoidal";
    }

  protected:
    EllipsoidalCS(const EllipsoidalCS &other) = delete;
};

// ---------------------------------------------------------------------------

class VerticalCS;
/** Shared pointer of VerticalCS. */
using VerticalCSPtr = std::shared_ptr<VerticalCS>;
/** Non-null shared pointer of VerticalCS. */
using VerticalCSNNPtr = util::nn<VerticalCSPtr>;

/** \brief A one-dimensional coordinate system used to record the heights or
 * depths of points.
 *
 * Such a coordinate system is usually dependent on the Earth's gravity field.
 * A VerticalCS shall have one axis association.
 *
 * \remark Implements VerticalCS from \ref ISO_19111_2018
 */
class VerticalCS : public CoordinateSystem {
  public:
    //! @cond Doxygen_Suppress
    PROJ_DLL ~VerticalCS() override;
    //! @endcond

    PROJ_DLL static VerticalCSNNPtr
    create(const util::PropertyMap &properties,
           const CoordinateSystemAxisNNPtr &axis);

    PROJ_DLL static VerticalCSNNPtr
    createGravityRelatedHeight(const common::UnitOfMeasure &unit);

  protected:
    VerticalCS();
    explicit VerticalCS(const CoordinateSystemAxisNNPtr &axisIn);
    INLINED_MAKE_SHARED

    std::string getWKT2Type(io::WKTFormatterNNPtr) const override {
        return "vertical";
    }

  private:
    VerticalCS(const VerticalCS &other) = delete;
};

// ---------------------------------------------------------------------------

class CartesianCS;
/** Shared pointer of CartesianCS. */
using CartesianCSPtr = std::shared_ptr<CartesianCS>;
/** Non-null shared pointer of CartesianCS. */
using CartesianCSNNPtr = util::nn<CartesianCSPtr>;

/** \brief A two- or three-dimensional coordinate system in Euclidean space
 * with orthogonal straight axes.
 *
 * All axes shall have the same length unit. A CartesianCS shall have two or
 * three axis associations; the number of associations shall equal the
 * dimension of the CS.
 *
 * \remark Implements CartesianCS from \ref ISO_19111_2018
 */
class CartesianCS : public CoordinateSystem {
  public:
    //! @cond Doxygen_Suppress
    PROJ_DLL ~CartesianCS() override;
    //! @endcond

    PROJ_DLL static CartesianCSNNPtr
    create(const util::PropertyMap &properties,
           const CoordinateSystemAxisNNPtr &axis1,
           const CoordinateSystemAxisNNPtr &axis2);
    PROJ_DLL static CartesianCSNNPtr
    create(const util::PropertyMap &properties,
           const CoordinateSystemAxisNNPtr &axis1,
           const CoordinateSystemAxisNNPtr &axis2,
           const CoordinateSystemAxisNNPtr &axis3);
    PROJ_DLL static CartesianCSNNPtr
    createEastingNorthing(const common::UnitOfMeasure &unit);
    PROJ_DLL static CartesianCSNNPtr
    createGeocentric(const common::UnitOfMeasure &unit);

  protected:
    CartesianCS();
    explicit CartesianCS(const std::vector<CoordinateSystemAxisNNPtr> &axisIn);
    INLINED_MAKE_SHARED

    std::string getWKT2Type(io::WKTFormatterNNPtr) const override {
        return "Cartesian"; // uppercase is intended
    }

  private:
    CartesianCS(const CartesianCS &other) = delete;
};

// ---------------------------------------------------------------------------

class OrdinalCS;
/** Shared pointer of OrdinalCS. */
using OrdinalCSPtr = std::shared_ptr<OrdinalCS>;
/** Non-null shared pointer of OrdinalCS. */
using OrdinalCSNNPtr = util::nn<OrdinalCSPtr>;

/** \brief n-dimensional coordinate system in which every axis uses integers.
 *
 * The number of associations shall equal the
 * dimension of the CS.
 *
 * \remark Implements OrdinalCS from \ref ISO_19111_2018
 */
class OrdinalCS : public CoordinateSystem {
  public:
    //! @cond Doxygen_Suppress
    PROJ_DLL ~OrdinalCS() override;
    //! @endcond

    PROJ_DLL static OrdinalCSNNPtr
    create(const util::PropertyMap &properties,
           const std::vector<CoordinateSystemAxisNNPtr> &axisIn);

  protected:
    OrdinalCS();
    explicit OrdinalCS(const std::vector<CoordinateSystemAxisNNPtr> &axisIn);
    INLINED_MAKE_SHARED

    std::string getWKT2Type(io::WKTFormatterNNPtr) const override {
        return "ordinal";
    }

  private:
    OrdinalCS(const OrdinalCS &other) = delete;
};

// ---------------------------------------------------------------------------

class TemporalCS;
/** Shared pointer of TemporalCS. */
using TemporalCSPtr = std::shared_ptr<TemporalCS>;
/** Non-null shared pointer of TemporalCS. */
using TemporalCSNNPtr = util::nn<TemporalCSPtr>;

/** \brief (Abstract class) A one-dimensional coordinate system used to record
 * time.
 *
 * A TemporalCS shall have one axis association.
 *
 * \remark Implements TemporalCS from \ref ISO_19111_2018
 */
class TemporalCS : public CoordinateSystem {
  public:
    //! @cond Doxygen_Suppress
    PROJ_DLL ~TemporalCS() override;
    //! @endcond

  protected:
    explicit TemporalCS(const CoordinateSystemAxisNNPtr &axis);
    INLINED_MAKE_SHARED

    std::string getWKT2Type(io::WKTFormatterNNPtr formatter) const override = 0;

  private:
    TemporalCS(const TemporalCS &other) = delete;
};

// ---------------------------------------------------------------------------

class DateTimeTemporalCS;
/** Shared pointer of DateTimeTemporalCS. */
using DateTimeTemporalCSPtr = std::shared_ptr<DateTimeTemporalCS>;
/** Non-null shared pointer of DateTimeTemporalCS. */
using DateTimeTemporalCSNNPtr = util::nn<DateTimeTemporalCSPtr>;

/** \brief A one-dimensional coordinate system used to record time in dateTime
 * representation as defined in ISO 8601.
 *
 * A DateTimeTemporalCS shall have one axis association. It does not use
 * axisUnitID; the temporal quantities are defined through the ISO 8601
 * representation.
 *
 * \remark Implements DateTimeTemporalCS from \ref ISO_19111_2018
 */
class DateTimeTemporalCS : public TemporalCS {
  public:
    //! @cond Doxygen_Suppress
    PROJ_DLL ~DateTimeTemporalCS() override;
    //! @endcond

    PROJ_DLL static DateTimeTemporalCSNNPtr
    create(const util::PropertyMap &properties,
           const CoordinateSystemAxisNNPtr &axis);

  protected:
    explicit DateTimeTemporalCS(const CoordinateSystemAxisNNPtr &axis);
    INLINED_MAKE_SHARED

    std::string getWKT2Type(io::WKTFormatterNNPtr formatter) const override;

  private:
    DateTimeTemporalCS(const DateTimeTemporalCS &other) = delete;
};

// ---------------------------------------------------------------------------

class TemporalCountCS;
/** Shared pointer of TemporalCountCS. */
using TemporalCountCSPtr = std::shared_ptr<TemporalCountCS>;
/** Non-null shared pointer of TemporalCountCS. */
using TemporalCountCSNNPtr = util::nn<TemporalCountCSPtr>;

/** \brief A one-dimensional coordinate system used to record time as an
 * integer count.
 *
 * A TemporalCountCS shall have one axis association.
 *
 * \remark Implements TemporalCountCS from \ref ISO_19111_2018
 */
class TemporalCountCS : public TemporalCS {
  public:
    //! @cond Doxygen_Suppress
    PROJ_DLL ~TemporalCountCS() override;
    //! @endcond

    PROJ_DLL static TemporalCountCSNNPtr
    create(const util::PropertyMap &properties,
           const CoordinateSystemAxisNNPtr &axis);

  protected:
    explicit TemporalCountCS(const CoordinateSystemAxisNNPtr &axis);
    INLINED_MAKE_SHARED

    std::string getWKT2Type(io::WKTFormatterNNPtr formatter) const override;

  private:
    TemporalCountCS(const TemporalCountCS &other) = delete;
};

// ---------------------------------------------------------------------------

class TemporalMeasureCS;
/** Shared pointer of TemporalMeasureCS. */
using TemporalMeasureCSPtr = std::shared_ptr<TemporalMeasureCS>;
/** Non-null shared pointer of TemporalMeasureCS. */
using TemporalMeasureCSNNPtr = util::nn<TemporalMeasureCSPtr>;

/** \brief A one-dimensional coordinate system used to record a time as a
 * real number.
 *
 * A TemporalMeasureCS shall have one axis association.
 *
 * \remark Implements TemporalMeasureCS from \ref ISO_19111_2018
 */
class TemporalMeasureCS : public TemporalCS {
  public:
    //! @cond Doxygen_Suppress
    PROJ_DLL ~TemporalMeasureCS() override;
    //! @endcond

    PROJ_DLL static TemporalMeasureCSNNPtr
    create(const util::PropertyMap &properties,
           const CoordinateSystemAxisNNPtr &axis);

  protected:
    explicit TemporalMeasureCS(const CoordinateSystemAxisNNPtr &axis);
    INLINED_MAKE_SHARED

    std::string getWKT2Type(io::WKTFormatterNNPtr formatter) const override;

  private:
    TemporalMeasureCS(const TemporalMeasureCS &other) = delete;
};

} // namespace cs

NS_PROJ_END

#endif //  CS_HH_INCLUDED
