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

#ifndef COMMON_HH_INCLUDED
#define COMMON_HH_INCLUDED

#include <memory>
#include <string>
#include <vector>

#include "io.hpp"
#include "metadata.hpp"
#include "util.hpp"

NS_PROJ_START

/** osgeo.proj.common namespace
 *
 * \brief Common classes.
 */
namespace common {

// ---------------------------------------------------------------------------

/** \brief Unit of measure. */
class UnitOfMeasure {
  public:
    /** \brief Type of unit of measure. */
    enum class PROJ_DLL Type {
        /** Unknown unit of measure */
        UNKNOWN,
        /** No unit of measure */
        NONE,
        /** Angular unit of measure */
        ANGULAR,
        /** Linear unit of measure */
        LINEAR,
        /** Scale unit of measure */
        SCALE,
        /** Time unit of measure */
        TIME,
    };

    PROJ_DLL UnitOfMeasure(const std::string &nameIn = std::string(),
                           double toSIIn = 1.0, Type typeIn = Type::UNKNOWN,
                           const std::string &codeSpaceIn = std::string(),
                           const std::string &codeIn = std::string());

    //! @cond Doxygen_Suppress
    PROJ_DLL UnitOfMeasure(const UnitOfMeasure &other);
    PROJ_DLL ~UnitOfMeasure();
    PROJ_DLL UnitOfMeasure &operator=(const UnitOfMeasure &other);
    //! @endcond

    PROJ_DLL const std::string &name() const;
    PROJ_DLL double conversionToSI() const;
    PROJ_DLL Type type() const;

    PROJ_DLL const std::string &codeSpace() const;
    PROJ_DLL const std::string &code() const;

    PROJ_DLL bool operator==(const UnitOfMeasure &other) const;
    PROJ_DLL bool operator!=(const UnitOfMeasure &other) const;

    //! @cond Doxygen_Suppress
    PROJ_DLL std::string
    exportToWKT(io::WKTFormatterNNPtr formatter,
                const std::string &unitType =
                    std::string()) const; // throw(io::FormattingException)

    PROJ_DLL std::string exportToPROJString() const;
    //! @endcond

    PROJ_DLL static const UnitOfMeasure NONE;

    PROJ_DLL static const UnitOfMeasure SCALE_UNITY;
    PROJ_DLL static const UnitOfMeasure PARTS_PER_MILLION;

    PROJ_DLL static const UnitOfMeasure METRE;

    PROJ_DLL static const UnitOfMeasure RADIAN;
    PROJ_DLL static const UnitOfMeasure MICRORADIAN;
    PROJ_DLL static const UnitOfMeasure DEGREE;
    PROJ_DLL static const UnitOfMeasure ARC_SECOND;
    PROJ_DLL static const UnitOfMeasure GRAD;

    PROJ_DLL static const UnitOfMeasure SECOND;
    PROJ_DLL static const UnitOfMeasure YEAR;

  private:
    PROJ_OPAQUE_PRIVATE_DATA
};

/** Shared pointer of UnitOfMeasure. */
using UnitOfMeasurePtr = std::shared_ptr<UnitOfMeasure>;
/** Non-null shared pointer of UnitOfMeasure. */
using UnitOfMeasureNNPtr = util::nn<UnitOfMeasurePtr>;

// ---------------------------------------------------------------------------

/** \brief Numeric value associated with a UnitOfMeasure. */
class Measure : public util::BaseObject {
  public:
    PROJ_DLL Measure(double valueIn = 0.0,
                     const UnitOfMeasure &unitIn = UnitOfMeasure());

    //! @cond Doxygen_Suppress
    PROJ_DLL Measure(const Measure &other);
    PROJ_DLL ~Measure();
    PROJ_DLL Measure &operator=(const Measure &other);
    //! @endcond

    PROJ_DLL const UnitOfMeasure &unit() const;
    PROJ_DLL double getSIValue() const;
    PROJ_DLL double value() const;

    PROJ_DLL Measure convertToUnit(const UnitOfMeasure &otherUnit) const;

    PROJ_DLL bool operator==(const Measure &other) const;

  private:
    PROJ_OPAQUE_PRIVATE_DATA
};

/** Shared pointer of Measure. */
using MeasurePtr = std::shared_ptr<Measure>;
/** Non-null shared pointer of Measure. */
using MeasureNNPtr = util::nn<MeasurePtr>;

// ---------------------------------------------------------------------------

/** \brief Numeric value, without a physical unit of measure. */
class Scale : public Measure {
  public:
    PROJ_DLL explicit Scale(
        double valueIn = 0.0,
        const UnitOfMeasure &unitIn = UnitOfMeasure::SCALE_UNITY);

    //! @cond Doxygen_Suppress
    PROJ_DLL Scale(const Scale &other);
    PROJ_DLL ~Scale() override;
    //! @endcond

  protected:
    friend class util::optional<Scale>;
    Scale &operator=(const Scale &);
};

// ---------------------------------------------------------------------------

/** \brief Numeric value, with a angular unit of measure. */
class Angle : public Measure {
  public:
    PROJ_DLL Angle(double valueIn = 0.0,
                   const UnitOfMeasure &unitIn = UnitOfMeasure::DEGREE);

    //! @cond Doxygen_Suppress
    PROJ_DLL Angle(const Angle &other);
    PROJ_DLL ~Angle() override;
    //! @endcond

    PROJ_DLL Angle convertToUnit(const UnitOfMeasure &otherUnit) const;

  protected:
    friend class util::optional<Angle>;
    Angle &operator=(const Angle &);
};

// ---------------------------------------------------------------------------

/** \brief Numeric value, with a linear unit of measure. */
class Length : public Measure {
  public:
    PROJ_DLL Length(double valueIn = 0.0,
                    const UnitOfMeasure &unitIn = UnitOfMeasure::METRE);

    //! @cond Doxygen_Suppress
    PROJ_DLL Length(const Length &other);
    PROJ_DLL ~Length() override;
    //! @endcond

    PROJ_DLL Length convertToUnit(const UnitOfMeasure &otherUnit) const;

  protected:
    friend class util::optional<Length>;
    Length &operator=(const Length &);
};

// ---------------------------------------------------------------------------

/** \brief Date-time value, as a ISO:8601 encoded string, or other string
 * encoding */
class DateTime {
  public:
    //! @cond Doxygen_Suppress
    PROJ_DLL DateTime(const DateTime &other);
    PROJ_DLL ~DateTime();
    //! @endcond

    PROJ_DLL bool isISO_8601() const;
    PROJ_DLL std::string toString() const;

    PROJ_DLL static DateTime
    create(const std::string &str); // may throw Exception

  protected:
    DateTime();
    friend class util::optional<DateTime>;

  private:
    explicit DateTime(const std::string &str);
    DateTime &operator=(const DateTime &other) = delete;

    PROJ_OPAQUE_PRIVATE_DATA
};

// ---------------------------------------------------------------------------

/** \brief Data epoch */
class DataEpoch {
    // FIXME
  public:
    /** FIXME */
    Measure coordinateEpoch{};
};

// ---------------------------------------------------------------------------

class IdentifiedObject;
/** Shared pointer of IdentifiedObject. */
using IdentifiedObjectPtr = std::shared_ptr<IdentifiedObject>;
/** Non-null shared pointer of IdentifiedObject. */
using IdentifiedObjectNNPtr = util::nn<IdentifiedObjectPtr>;

/** \brief Identifications of a CRS-related object.
 *
 * \remark Implements IdentifiedObject from \ref ISO_19111_2018
 */
class IdentifiedObject : public util::BaseObject {
  public:
    //! @cond Doxygen_Suppress
    PROJ_DLL ~IdentifiedObject() override;
    //! @endcond

    PROJ_DLL static IdentifiedObjectNNPtr
    create(const util::PropertyMap
               &properties); // throw(InvalidValueTypeException)

    PROJ_DLL static const std::string NAME_KEY;
    PROJ_DLL static const std::string IDENTIFIERS_KEY;
    PROJ_DLL static const std::string ALIAS_KEY;
    PROJ_DLL static const std::string REMARKS_KEY;
    PROJ_DLL static const std::string DEPRECATED_KEY;

    // in practice only name().description() is used
    PROJ_DLL const metadata::IdentifierNNPtr &name() const;
    PROJ_DLL const std::vector<metadata::IdentifierNNPtr> &identifiers() const;
    PROJ_DLL const std::vector<util::GenericNameNNPtr> &aliases() const;
    PROJ_DLL const std::string &remarks() const;

    // from Apache SIS AbstractIdentifiedObject
    PROJ_DLL bool isDeprecated() const;

    // Non-standard
    PROJ_DLL std::string alias() const;
    PROJ_DLL bool isEPSG(int code) const;

    //! @cond Doxygen_Suppress
    void formatID(io::WKTFormatterNNPtr formatter) const;
    void formatRemarks(io::WKTFormatterNNPtr formatter) const;
    //! @endcond

  protected:
    friend class util::optional<IdentifiedObject>;
    INLINED_MAKE_SHARED
    IdentifiedObject();
    IdentifiedObject(const IdentifiedObject &other);
    IdentifiedObject &operator=(const IdentifiedObject &other);

    void setProperties(const util::PropertyMap
                           &properties); // throw(InvalidValueTypeException)

  private:
    PROJ_OPAQUE_PRIVATE_DATA
};

// ---------------------------------------------------------------------------

class ObjectDomain;
/** Shared pointer of ObjectDomain. */
using ObjectDomainPtr = std::shared_ptr<ObjectDomain>;
/** Non-null shared pointer of ObjectDomain. */
using ObjectDomainNNPtr = util::nn<ObjectDomainPtr>;

/** \brief The scope and validity of a CRS-related object.
 *
 * \remark Implements ObjectDomain from \ref ISO_19111_2018
 */
class ObjectDomain : public util::BaseObject {
  public:
    //! @cond Doxygen_Suppress
    PROJ_DLL ~ObjectDomain();
    //! @endcond

    // In ISO_19111:2018, scope and domain are compulsory, but in WKT2:2015,
    // they
    // are not necessarily both specified
    PROJ_DLL const util::optional<std::string> &scope() const;
    PROJ_DLL const metadata::ExtentPtr &domainOfValidity() const;

    PROJ_DLL static ObjectDomainNNPtr
    create(const util::optional<std::string> &scopeIn,
           const metadata::ExtentPtr &extent);

    //! @cond Doxygen_Suppress
    std::string exportToWKT(io::WKTFormatterNNPtr formatter)
        const; // throw(io::FormattingException)
               //! @endcond

  protected:
    ObjectDomain();
    ObjectDomain(const ObjectDomain &other);
    INLINED_MAKE_SHARED

  private:
    PROJ_OPAQUE_PRIVATE_DATA
    ObjectDomain &operator=(const ObjectDomain &other) = delete;
};

// ---------------------------------------------------------------------------

/** \brief Usage of a CRS-related object.
 *
 * \remark Implements ObjectUsage from \ref ISO_19111_2018
 */
class ObjectUsage : public IdentifiedObject {
  public:
    //! @cond Doxygen_Suppress
    PROJ_DLL ~ObjectUsage();
    //! @endcond

    PROJ_DLL const std::vector<ObjectDomainNNPtr> &domains() const;

    PROJ_DLL static const std::string SCOPE_KEY;
    PROJ_DLL static const std::string DOMAIN_OF_VALIDITY_KEY;

    PROJ_DLL static const std::string OBJECT_DOMAIN_KEY;

  protected:
    ObjectUsage();
    ObjectUsage(const ObjectUsage &other);
    void setProperties(const util::PropertyMap
                           &properties); // throw(InvalidValueTypeException)

    void _exportToWKT(io::WKTFormatterNNPtr formatter)
        const; // throw(io::FormattingException)

  private:
    PROJ_OPAQUE_PRIVATE_DATA
    ObjectUsage &operator=(const ObjectUsage &other) = delete;
};

} // namespace common

NS_PROJ_END

#endif //  COMMON_HH_INCLUDED
