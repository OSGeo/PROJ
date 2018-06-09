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

#ifndef COORDINATEOPERATION_HH_INCLUDED
#define COORDINATEOPERATION_HH_INCLUDED

#include <memory>
#include <string>
#include <vector>

#include "common.hpp"
#include "io.hpp"
#include "metadata.hpp"

NS_PROJ_START

namespace crs {
class CRS;
using CRSPtr = std::shared_ptr<CRS>;
class DerivedCRS;
class ProjectedCRS;
}

/** osgeo.proj.operation namespace

  \brief Coordinate operations (relationship between any two coordinate
  reference systems).
*/
namespace operation {

// ---------------------------------------------------------------------------

class CoordinateOperation : public common::ObjectUsage {
  public:
    PROJ_DLL virtual ~CoordinateOperation();
    PROJ_DLL CoordinateOperation &
    operator=(const CoordinateOperation &other) = delete;

    PROJ_DLL const util::optional<std::string> &operationVersion() const;
    PROJ_DLL const std::vector<metadata::PositionalAccuracy> &
    coordinateOperationAccuracies() const;

    // In the case of a derivingConversion of a DerivedCRS, sourceCRS() and
    // targetCRS()
    // will return null pointers if the owning DerivedCRS has been destroyed.
    PROJ_DLL const crs::CRSPtr sourceCRS() const;
    PROJ_DLL const crs::CRSPtr targetCRS() const;
    PROJ_DLL const crs::CRSPtr &interpolationCRS() const;
    PROJ_DLL const util::optional<common::DataEpoch> &
    sourceCoordinateEpoch() const;
    PROJ_DLL const util::optional<common::DataEpoch> &
    targetCoordinateEpoch() const;

    // virtual void transform(...) = 0;  TODO

  protected:
    CoordinateOperation();
    CoordinateOperation(const CoordinateOperation &other);

    friend class crs::DerivedCRS;
    friend class crs::ProjectedCRS;
    void setWeakSourceTargetCRS(std::weak_ptr<crs::CRS> sourceCRSIn,
                                std::weak_ptr<crs::CRS> targetCRSIn);

  private:
    PROJ_OPAQUE_PRIVATE_DATA
};

// ---------------------------------------------------------------------------

class GeneralOperationParameter : public common::IdentifiedObject {
  public:
    PROJ_DLL virtual ~GeneralOperationParameter();
    PROJ_DLL GeneralOperationParameter &
    operator=(const GeneralOperationParameter &other) = delete;

  protected:
    GeneralOperationParameter();
    GeneralOperationParameter(const GeneralOperationParameter &other);

  private:
    PROJ_OPAQUE_PRIVATE_DATA
};

using GeneralOperationParameterPtr = std::shared_ptr<GeneralOperationParameter>;
using GeneralOperationParameterNNPtr = util::nn<GeneralOperationParameterPtr>;

// ---------------------------------------------------------------------------

class OperationParameter;
using OperationParameterPtr = std::shared_ptr<OperationParameter>;
using OperationParameterNNPtr = util::nn<OperationParameterPtr>;

class OperationParameter : public GeneralOperationParameter {
  public:
    PROJ_DLL OperationParameter(const OperationParameter &other);
    PROJ_DLL OperationParameter &
    operator=(const OperationParameter &other) = delete;
    PROJ_DLL virtual ~OperationParameter();

    // non-standard
    PROJ_DLL static OperationParameterNNPtr
    create(const util::PropertyMap &properties);

  protected:
    OperationParameter();
    INLINED_MAKE_SHARED

  private:
    PROJ_OPAQUE_PRIVATE_DATA
};

// ---------------------------------------------------------------------------

struct MethodMapping;

class GeneralParameterValue : public io::IWKTExportable {
  public:
    PROJ_DLL GeneralParameterValue &
    operator=(const GeneralParameterValue &other) = delete;
    PROJ_DLL virtual ~GeneralParameterValue();

    PROJ_DLL std::string exportToWKT(io::WKTFormatterNNPtr formatter)
        const override = 0; // throw(io::FormattingException)

  protected:
    GeneralParameterValue();
    GeneralParameterValue(const GeneralParameterValue &other);

    friend class Conversion;
    virtual std::string _exportToWKT(io::WKTFormatterNNPtr formatter,
                                     const MethodMapping *mapping)
        const = 0; // throw(io::FormattingException)

  private:
    PROJ_OPAQUE_PRIVATE_DATA
};

using GeneralParameterValuePtr = std::shared_ptr<GeneralParameterValue>;
using GeneralParameterValueNNPtr = util::nn<GeneralParameterValuePtr>;

// ---------------------------------------------------------------------------

class ParameterValue;
using ParameterValuePtr = std::shared_ptr<ParameterValue>;
using ParameterValueNNPtr = util::nn<ParameterValuePtr>;

class ParameterValue : public util::BoxedValue, public io::IWKTExportable {
  public:
    PROJ_DLL ParameterValue &operator=(const ParameterValue &other) = delete;
    PROJ_DLL virtual ~ParameterValue();

    PROJ_DLL std::string exportToWKT(io::WKTFormatterNNPtr formatter)
        const override; // throw(io::FormattingException)

    PROJ_DLL static ParameterValueNNPtr
    create(const common::Measure &measureIn);
    PROJ_DLL static ParameterValueNNPtr create(const char *stringValueIn);
    PROJ_DLL static ParameterValueNNPtr
    create(const std::string &stringValueIn);
    PROJ_DLL static ParameterValueNNPtr create(int integerValueIn);
    PROJ_DLL static ParameterValueNNPtr create(bool booleanValueIn);

  protected:
    explicit ParameterValue(const common::MeasureNNPtr &measureIn);
    explicit ParameterValue(
        const char *stringValueIn); // needed to avoid the bool constructor
                                    // to be taken !
    explicit ParameterValue(const std::string &stringValueIn);
    explicit ParameterValue(int integerValueIn);
    explicit ParameterValue(bool booleanValueIn);
    INLINED_MAKE_SHARED
};

// ---------------------------------------------------------------------------

class OperationParameterValue;
using OperationParameterValuePtr = std::shared_ptr<OperationParameterValue>;
using OperationParameterValueNNPtr = util::nn<OperationParameterValuePtr>;

class OperationParameterValue : public GeneralParameterValue {
  public:
    PROJ_DLL OperationParameterValue(const OperationParameterValue &other);
    PROJ_DLL OperationParameterValue &
    operator=(const OperationParameterValue &other) = delete;
    PROJ_DLL ~OperationParameterValue() override;

    PROJ_DLL const OperationParameterNNPtr &parameter() const;
    PROJ_DLL const ParameterValueNNPtr &parameterValue() const;

    PROJ_DLL std::string exportToWKT(io::WKTFormatterNNPtr formatter)
        const override; // throw(io::FormattingException)

    PROJ_DLL static OperationParameterValueNNPtr
    create(OperationParameterNNPtr parameterIn, ParameterValueNNPtr valueIn);

  protected:
    OperationParameterValue(OperationParameterNNPtr parameterIn,
                            ParameterValueNNPtr valueIn);
    INLINED_MAKE_SHARED

    std::string _exportToWKT(io::WKTFormatterNNPtr formatter,
                             const MethodMapping *mapping)
        const override; // throw(io::FormattingException)

  private:
    PROJ_OPAQUE_PRIVATE_DATA
};

// ---------------------------------------------------------------------------

class OperationMethod;
using OperationMethodPtr = std::shared_ptr<OperationMethod>;
using OperationMethodNNPtr = util::nn<OperationMethodPtr>;

class OperationMethod : public common::IdentifiedObject,
                        public io::IWKTExportable {
  public:
    PROJ_DLL OperationMethod(const OperationMethod &other);
    PROJ_DLL virtual ~OperationMethod();
    PROJ_DLL OperationMethod &operator=(const OperationMethod &other) = delete;

    PROJ_DLL const util::optional<std::string> &formula() const;
    PROJ_DLL const util::optional<metadata::Citation> &formulaCitation() const;
    PROJ_DLL const std::vector<GeneralOperationParameterNNPtr> &
    parameters() const;

    PROJ_DLL std::string exportToWKT(io::WKTFormatterNNPtr formatter)
        const override; // throw(io::FormattingException)

    PROJ_DLL static OperationMethodNNPtr
    create(const util::PropertyMap &properties,
           const std::vector<GeneralOperationParameterNNPtr> &parameters);

    PROJ_DLL static OperationMethodNNPtr
    create(const util::PropertyMap &properties,
           const std::vector<OperationParameterNNPtr> &parameters);

  protected:
    OperationMethod();
    INLINED_MAKE_SHARED

  private:
    PROJ_OPAQUE_PRIVATE_DATA
};

// ---------------------------------------------------------------------------

class InvalidOperation : public util::Exception {
  public:
    PROJ_DLL explicit InvalidOperation(const char *message);
    PROJ_DLL explicit InvalidOperation(const std::string &message);
    PROJ_DLL InvalidOperation(const InvalidOperation &other);
    PROJ_DLL ~InvalidOperation() override;
};

// ---------------------------------------------------------------------------

class SingleOperation : public CoordinateOperation {
  public:
    PROJ_DLL virtual ~SingleOperation();
    PROJ_DLL SingleOperation &operator=(const SingleOperation &other) = delete;

    PROJ_DLL const std::vector<GeneralParameterValueNNPtr> &
    parameterValues() const;
    PROJ_DLL const OperationMethodNNPtr &method() const;

  protected:
    explicit SingleOperation(const OperationMethodNNPtr &methodIn);
    SingleOperation(const SingleOperation &other);

    void
    setParameterValues(const std::vector<GeneralParameterValueNNPtr> &values);

  private:
    PROJ_OPAQUE_PRIVATE_DATA
};

// ---------------------------------------------------------------------------

class Conversion;
using ConversionPtr = std::shared_ptr<Conversion>;
using ConversionNNPtr = util::nn<ConversionPtr>;

class Conversion : public SingleOperation, public io::IWKTExportable {
  public:
    PROJ_DLL virtual ~Conversion();
    PROJ_DLL Conversion &operator=(const Conversion &other) = delete;

    PROJ_DLL std::string exportToWKT(io::WKTFormatterNNPtr formatter)
        const override; // throw(io::FormattingException)

    PROJ_DLL ConversionNNPtr identify() const;

    PROJ_DLL static ConversionNNPtr
    create(const util::PropertyMap &properties,
           const OperationMethodNNPtr &methodIn,
           const std::vector<GeneralParameterValueNNPtr>
               &values); // throw InvalidOperation
    PROJ_DLL static ConversionNNPtr
    create(const util::PropertyMap &propertiesConversion,
           const util::PropertyMap &propertiesOperationMethod,
           const std::vector<OperationParameterNNPtr> &parameters,
           const std::vector<ParameterValueNNPtr>
               &values); // throw InvalidOperation
    PROJ_DLL static ConversionNNPtr create(const ConversionNNPtr &other);
    PROJ_DLL static ConversionNNPtr createUTM(int zone, bool north);

  protected:
    Conversion(const OperationMethodNNPtr &methodIn,
               const std::vector<GeneralParameterValueNNPtr> &values);
    Conversion(const Conversion &other);
    INLINED_MAKE_SHARED

  private:
    PROJ_OPAQUE_PRIVATE_DATA
};

// ---------------------------------------------------------------------------

class Transformation;
using TransformationPtr = std::shared_ptr<Transformation>;
using TransformationNNPtr = util::nn<TransformationPtr>;

class Transformation : public SingleOperation {
  public:
    // TODO
    PROJ_DLL ~Transformation() override;

  private:
    Transformation(const Transformation &) = delete;
};

// ---------------------------------------------------------------------------

class PointMotionOperation;
using PointMotionOperationPtr = std::shared_ptr<PointMotionOperation>;
using PointMotionOperationNNPtr = util::nn<PointMotionOperationPtr>;

class PointMotionOperation : public SingleOperation {
  public:
    // TODO
    PROJ_DLL ~PointMotionOperation() override;

  private:
    PointMotionOperation(const PointMotionOperation &) = delete;
    // ---------------------------------------------------------------------------
};

} // namespace operation

NS_PROJ_END

#endif //  COORDINATEOPERATION_HH_INCLUDED
