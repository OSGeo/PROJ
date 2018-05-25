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

#include "exception.hh"
#include "io.hh"
#include "metadata.hh"
#include "util.hh"

NS_PROJ_START

/** osgeo.proj.common namespace
 *
 * \brief Common classes.
 */
namespace common {

// ---------------------------------------------------------------------------

class Date {
    // TODO
};

// ---------------------------------------------------------------------------

class UnitOfMeasure {
  public:
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
        SCALE
    };

    PROJ_DLL UnitOfMeasure(const std::string &nameIn = std::string(),
                           double toSIIn = 1.0, Type typeIn = Type::UNKNOWN,
                           const std::string &authorityIn = std::string(),
                           const std::string &codeIn = std::string());
    PROJ_DLL UnitOfMeasure(const UnitOfMeasure &other);
    PROJ_DLL ~UnitOfMeasure();
    PROJ_DLL UnitOfMeasure &operator=(const UnitOfMeasure &other);

    PROJ_DLL const std::string &name() const;
    PROJ_DLL double conversionToSI() const;
    PROJ_DLL Type type() const;

    PROJ_DLL const std::string &authority() const;
    PROJ_DLL const std::string &code() const;

    PROJ_DLL bool operator==(const UnitOfMeasure &other) const;
    PROJ_DLL bool operator!=(const UnitOfMeasure &other) const;

    PROJ_DLL std::string
    exportToWKT(io::WKTFormatterNNPtr formatter,
                const std::string &unitType =
                    std::string()) const; // throw(io::FormattingException)

    PROJ_DLL static const UnitOfMeasure NONE;
    PROJ_DLL static const UnitOfMeasure SCALE_UNITY;
    PROJ_DLL static const UnitOfMeasure METRE;
    PROJ_DLL static const UnitOfMeasure RADIAN;
    PROJ_DLL static const UnitOfMeasure DEGREE;
    PROJ_DLL static const UnitOfMeasure GRAD;

  private:
    PROJ_OPAQUE_PRIVATE_DATA
};

// ---------------------------------------------------------------------------

class Measure : public util::BaseObject {
  public:
    PROJ_DLL Measure(double valueIn = 0.0,
                     const UnitOfMeasure &unitIn = UnitOfMeasure());
    PROJ_DLL Measure(const Measure &other);
    PROJ_DLL ~Measure();
    PROJ_DLL Measure &operator=(const Measure &other);

    PROJ_DLL const UnitOfMeasure &unit() const;
    PROJ_DLL double getSIValue() const;
    PROJ_DLL double value() const;

    PROJ_DLL Measure convertToUnit(const UnitOfMeasure &otherUnit) const;

  private:
    PROJ_OPAQUE_PRIVATE_DATA
};

using MeasurePtr = std::shared_ptr<Measure>;
using MeasureNNPtr = util::nn<MeasurePtr>;

// ---------------------------------------------------------------------------

class Scale : public Measure {
  public:
    PROJ_DLL explicit Scale(double valueIn = 0.0)
        : Measure(valueIn, UnitOfMeasure::SCALE_UNITY) {}
};

// ---------------------------------------------------------------------------

class Angle : public Measure {
  public:
    PROJ_DLL Angle(double valueIn = 0.0,
                   const UnitOfMeasure &unitIn = UnitOfMeasure::DEGREE)
        : Measure(valueIn, unitIn) {}

    PROJ_DLL Angle convertToUnit(const UnitOfMeasure &otherUnit) const {
        return Angle(Measure::convertToUnit(otherUnit).value(), otherUnit);
    }
};

// ---------------------------------------------------------------------------

class Length : public Measure {
  public:
    PROJ_DLL Length(double valueIn = 0.0,
                    const UnitOfMeasure &unitIn = UnitOfMeasure::METRE)
        : Measure(valueIn, unitIn) {}

    PROJ_DLL Length convertToUnit(const UnitOfMeasure &otherUnit) const {
        return Length(Measure::convertToUnit(otherUnit).value(), otherUnit);
    }
};

// ---------------------------------------------------------------------------

class DataEpoch {
    // FIXME
  public:
    Measure coordinateEpoch{};
};

// ---------------------------------------------------------------------------

class IdentifiedObject;
using IdentifiedObjectPtr = std::shared_ptr<IdentifiedObject>;
using IdentifiedObjectNNPtr = util::nn<IdentifiedObjectPtr>;

class IdentifiedObject : public util::BaseObject {
  public:
    PROJ_DLL IdentifiedObject(const IdentifiedObject &other);
    PROJ_DLL virtual ~IdentifiedObject();

    PROJ_DLL static IdentifiedObjectNNPtr
    create(const util::PropertyMap
               &properties); // throw(InvalidValueTypeException)

    PROJ_DLL static const std::string NAME_KEY;
    PROJ_DLL static const std::string IDENTIFIER_KEY;
    PROJ_DLL static const std::string ALIAS_KEY;
    PROJ_DLL static const std::string REMARKS_KEY;
    PROJ_DLL static const std::string DEPRECATED_KEY;

    PROJ_DLL const metadata::IdentifierNNPtr &name() const;
    PROJ_DLL const std::vector<metadata::IdentifierNNPtr> &identifiers() const;
    PROJ_DLL const std::vector<util::GenericNameNNPtr> &aliases() const;
    PROJ_DLL const std::string &remarks() const;

    // from Apache SIS AbstractIdentifiedObject
    PROJ_DLL bool isDeprecated() const;

    // Non-standard
    PROJ_DLL std::string alias() const;

    PROJ_DLL void formatID(io::WKTFormatterNNPtr formatter) const;
    PROJ_DLL void formatRemarks(io::WKTFormatterNNPtr formatter) const;

  protected:
    friend class util::optional<IdentifiedObject>;
    INLINED_MAKE_SHARED
    IdentifiedObject();
    IdentifiedObject &operator=(const IdentifiedObject &other);

    void setProperties(const util::PropertyMap
                           &properties); // throw(InvalidValueTypeException)

  private:
    PROJ_OPAQUE_PRIVATE_DATA
};

// ---------------------------------------------------------------------------

class ObjectDomain {
  public:
    PROJ_DLL ObjectDomain();
    PROJ_DLL ObjectDomain(const ObjectDomain &other);
    PROJ_DLL ObjectDomain &operator=(const ObjectDomain &other) = delete;
    PROJ_DLL ~ObjectDomain();

    PROJ_DLL const std::string &scope() const;
    PROJ_DLL const metadata::Extent &domainOfValidity() const;

  private:
    PROJ_OPAQUE_PRIVATE_DATA
};

using ObjectDomainPtr = std::shared_ptr<ObjectDomain>;

// ---------------------------------------------------------------------------

class ObjectUsage : public IdentifiedObject {
  public:
    PROJ_DLL ObjectUsage();
    PROJ_DLL ObjectUsage(const ObjectUsage &other);
    PROJ_DLL ObjectUsage &operator=(const ObjectUsage &other) = delete;
    PROJ_DLL ~ObjectUsage();

    PROJ_DLL const ObjectDomainPtr &domain() const;

  private:
    PROJ_OPAQUE_PRIVATE_DATA
};

} // namespace common

NS_PROJ_END

#endif //  COMMON_HH_INCLUDED
