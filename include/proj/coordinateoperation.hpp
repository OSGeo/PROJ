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
using CRSNNPtr = util::nn<CRSPtr>;

class DerivedCRS;
class ProjectedCRS;
}

/** osgeo.proj.operation namespace

  \brief Coordinate operations (relationship between any two coordinate
  reference systems).
*/
namespace operation {

// ---------------------------------------------------------------------------

class CoordinateOperation;
using CoordinateOperationPtr = std::shared_ptr<CoordinateOperation>;
using CoordinateOperationNNPtr = util::nn<CoordinateOperationPtr>;

class CoordinateOperation : public common::ObjectUsage,
                            public io::IWKTExportable {
  public:
    PROJ_DLL ~CoordinateOperation() override;

    PROJ_DLL const util::optional<std::string> &operationVersion() const;
    PROJ_DLL const std::vector<metadata::PositionalAccuracyNNPtr> &
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
    std::string exportToWKT(io::WKTFormatterNNPtr formatter) const override = 0;

  protected:
    CoordinateOperation();
    CoordinateOperation(const CoordinateOperation &other);

    friend class crs::DerivedCRS;
    friend class crs::ProjectedCRS;
    void setWeakSourceTargetCRS(std::weak_ptr<crs::CRS> sourceCRSIn,
                                std::weak_ptr<crs::CRS> targetCRSIn);
    void setCRSs(const crs::CRSNNPtr &sourceCRSIn,
                 const crs::CRSNNPtr &targetCRSIn,
                 const crs::CRSPtr &interpolationCRSIn);
    void setAccuracies(
        const std::vector<metadata::PositionalAccuracyNNPtr> &accuracies);

  private:
    PROJ_OPAQUE_PRIVATE_DATA
    CoordinateOperation &operator=(const CoordinateOperation &other) = delete;
};

// ---------------------------------------------------------------------------

class GeneralOperationParameter : public common::IdentifiedObject {
  public:
    PROJ_DLL virtual ~GeneralOperationParameter();

  protected:
    GeneralOperationParameter();
    GeneralOperationParameter(const GeneralOperationParameter &other);

  private:
    PROJ_OPAQUE_PRIVATE_DATA
    GeneralOperationParameter &
    operator=(const GeneralOperationParameter &other) = delete;
};

using GeneralOperationParameterPtr = std::shared_ptr<GeneralOperationParameter>;
using GeneralOperationParameterNNPtr = util::nn<GeneralOperationParameterPtr>;

// ---------------------------------------------------------------------------

class OperationParameter;
using OperationParameterPtr = std::shared_ptr<OperationParameter>;
using OperationParameterNNPtr = util::nn<OperationParameterPtr>;

class OperationParameter : public GeneralOperationParameter {
  public:
    PROJ_DLL virtual ~OperationParameter();

    // non-standard
    PROJ_DLL static OperationParameterNNPtr
    create(const util::PropertyMap &properties);

  protected:
    OperationParameter();
    OperationParameter(const OperationParameter &other);
    INLINED_MAKE_SHARED

  private:
    PROJ_OPAQUE_PRIVATE_DATA
    OperationParameter &operator=(const OperationParameter &other) = delete;
};

// ---------------------------------------------------------------------------

struct MethodMapping;

class GeneralParameterValue : public io::IWKTExportable {
  public:
    PROJ_DLL ~GeneralParameterValue() override;

    PROJ_DLL std::string exportToWKT(io::WKTFormatterNNPtr formatter)
        const override = 0; // throw(io::FormattingException)

  protected:
    GeneralParameterValue();
    GeneralParameterValue(const GeneralParameterValue &other);

    friend class Conversion;
    friend class Transformation;
    virtual std::string _exportToWKT(io::WKTFormatterNNPtr formatter,
                                     const MethodMapping *mapping)
        const = 0; // throw(io::FormattingException)

  private:
    PROJ_OPAQUE_PRIVATE_DATA
    GeneralParameterValue &
    operator=(const GeneralParameterValue &other) = delete;
};

using GeneralParameterValuePtr = std::shared_ptr<GeneralParameterValue>;
using GeneralParameterValueNNPtr = util::nn<GeneralParameterValuePtr>;

// ---------------------------------------------------------------------------

class ParameterValue;
using ParameterValuePtr = std::shared_ptr<ParameterValue>;
using ParameterValueNNPtr = util::nn<ParameterValuePtr>;

class ParameterValue : public io::IWKTExportable {
  public:
    enum class Type { MEASURE, STRING, INTEGER, BOOLEAN, FILENAME };

    PROJ_DLL ~ParameterValue() override;

    PROJ_DLL std::string exportToWKT(io::WKTFormatterNNPtr formatter)
        const override; // throw(io::FormattingException)

    PROJ_DLL static ParameterValueNNPtr
    create(const common::Measure &measureIn);
    PROJ_DLL static ParameterValueNNPtr create(const char *stringValueIn);
    PROJ_DLL static ParameterValueNNPtr
    create(const std::string &stringValueIn);
    PROJ_DLL static ParameterValueNNPtr create(int integerValueIn);
    PROJ_DLL static ParameterValueNNPtr create(bool booleanValueIn);
    PROJ_DLL static ParameterValueNNPtr
    createFilename(const std::string &stringValueIn);

    PROJ_DLL const Type &type() const;
    PROJ_DLL const common::Measure &value() const;
    PROJ_DLL const std::string &stringValue() const;
    PROJ_DLL const std::string &valueFile() const;
    PROJ_DLL int integerValue() const;
    PROJ_DLL bool booleanValue() const;

  protected:
    explicit ParameterValue(const common::Measure &measureIn);
    explicit ParameterValue(const std::string &stringValueIn, Type typeIn);
    explicit ParameterValue(int integerValueIn);
    explicit ParameterValue(bool booleanValueIn);
    INLINED_MAKE_SHARED
  private:
    PROJ_OPAQUE_PRIVATE_DATA
    ParameterValue &operator=(const ParameterValue &other) = delete;
};

// ---------------------------------------------------------------------------

class OperationParameterValue;
using OperationParameterValuePtr = std::shared_ptr<OperationParameterValue>;
using OperationParameterValueNNPtr = util::nn<OperationParameterValuePtr>;

class OperationParameterValue : public GeneralParameterValue {
  public:
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
    OperationParameterValue(const OperationParameterValue &other);
    INLINED_MAKE_SHARED

    std::string _exportToWKT(io::WKTFormatterNNPtr formatter,
                             const MethodMapping *mapping)
        const override; // throw(io::FormattingException)

  private:
    PROJ_OPAQUE_PRIVATE_DATA
    OperationParameterValue &
    operator=(const OperationParameterValue &other) = delete;
};

// ---------------------------------------------------------------------------

class OperationMethod;
using OperationMethodPtr = std::shared_ptr<OperationMethod>;
using OperationMethodNNPtr = util::nn<OperationMethodPtr>;

class OperationMethod : public common::IdentifiedObject,
                        public io::IWKTExportable {
  public:
    PROJ_DLL ~OperationMethod() override;

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
    OperationMethod(const OperationMethod &other);
    INLINED_MAKE_SHARED

  private:
    PROJ_OPAQUE_PRIVATE_DATA
    OperationMethod &operator=(const OperationMethod &other) = delete;
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
    PROJ_DLL ~SingleOperation() override;

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
    SingleOperation &operator=(const SingleOperation &other) = delete;
};

// ---------------------------------------------------------------------------

class Conversion;
using ConversionPtr = std::shared_ptr<Conversion>;
using ConversionNNPtr = util::nn<ConversionPtr>;

class Conversion : public SingleOperation {
  public:
    PROJ_DLL ~Conversion() override;

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
    Conversion &operator=(const Conversion &other) = delete;
};

// ---------------------------------------------------------------------------

class Transformation;
using TransformationPtr = std::shared_ptr<Transformation>;
using TransformationNNPtr = util::nn<TransformationPtr>;

class Transformation : public SingleOperation {
  public:
    PROJ_DLL ~Transformation() override;

    PROJ_DLL const crs::CRSNNPtr sourceCRS() const;
    PROJ_DLL const crs::CRSNNPtr targetCRS() const;

    PROJ_DLL std::string exportToWKT(io::WKTFormatterNNPtr formatter)
        const override; // throw(io::FormattingException)

    PROJ_DLL static TransformationNNPtr
    create(const util::PropertyMap &properties,
           const crs::CRSNNPtr &sourceCRSIn, const crs::CRSNNPtr &targetCRSIn,
           const crs::CRSPtr &interpolationCRSIn,
           const OperationMethodNNPtr &methodIn,
           const std::vector<GeneralParameterValueNNPtr> &values,
           const std::vector<metadata::PositionalAccuracyNNPtr>
               &accuracies); // throw InvalidOperation
    PROJ_DLL static TransformationNNPtr
    create(const util::PropertyMap &propertiesTransformation,
           const crs::CRSNNPtr &sourceCRSIn, const crs::CRSNNPtr &targetCRSIn,
           const crs::CRSPtr &interpolationCRSIn,
           const util::PropertyMap &propertiesOperationMethod,
           const std::vector<OperationParameterNNPtr> &parameters,
           const std::vector<ParameterValueNNPtr> &values,
           const std::vector<metadata::PositionalAccuracyNNPtr>
               &accuracies); // throw InvalidOperation

  protected:
    Transformation(
        const crs::CRSNNPtr &sourceCRSIn, const crs::CRSNNPtr &targetCRSIn,
        const crs::CRSPtr &interpolationCRSIn,
        const OperationMethodNNPtr &methodIn,
        const std::vector<GeneralParameterValueNNPtr> &values,
        const std::vector<metadata::PositionalAccuracyNNPtr> &accuracies);
    INLINED_MAKE_SHARED

  private:
    PROJ_OPAQUE_PRIVATE_DATA
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
};

// ---------------------------------------------------------------------------

class ConcatenatedOperation;
using ConcatenatedOperationPtr = std::shared_ptr<ConcatenatedOperation>;
using ConcatenatedOperationNNPtr = util::nn<ConcatenatedOperationPtr>;

class ConcatenatedOperation : public CoordinateOperation {
  public:
    PROJ_DLL ~ConcatenatedOperation() override;

    PROJ_DLL const std::vector<CoordinateOperationNNPtr> &operations() const;

    PROJ_DLL std::string exportToWKT(io::WKTFormatterNNPtr formatter)
        const override; // throw(io::FormattingException)

    PROJ_DLL static ConcatenatedOperationNNPtr
    create(const util::PropertyMap &properties,
           const std::vector<CoordinateOperationNNPtr> &operationsIn,
           const std::vector<metadata::PositionalAccuracyNNPtr>
               &accuracies); // throw InvalidOperation

  protected:
    explicit ConcatenatedOperation(
        const std::vector<CoordinateOperationNNPtr> &operationsIn);
    INLINED_MAKE_SHARED

  private:
    PROJ_OPAQUE_PRIVATE_DATA
    ConcatenatedOperation &
    operator=(const ConcatenatedOperation &other) = delete;
};

} // namespace operation

NS_PROJ_END

#endif //  COORDINATEOPERATION_HH_INCLUDED
