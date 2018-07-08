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

  This covers Conversion, Transformation,
  PointMotionOperation or ConcatenatedOperation.
*/
namespace operation {

// ---------------------------------------------------------------------------

class CoordinateOperation;
/** Shared pointer of CoordinateOperation */
using CoordinateOperationPtr = std::shared_ptr<CoordinateOperation>;
/** Non-null shared pointer of CoordinateOperation */
using CoordinateOperationNNPtr = util::nn<CoordinateOperationPtr>;

/** \brief Abstract class for a mathematical operation on coordinates.
 *
 * A mathematical operation:
 * <ul>
 * <li>on coordinates that transforms or converts them from one coordinate
 * reference system to another coordinate reference system</li>
 * <li>or that describes the change of coordinate values within one coordinate
 * reference system due to the motion of the point between one coordinate epoch
 * and another coordinate epoch.</li>
 * </ul>
 * Many but not all coordinate operations (from CRS A to CRS B) also uniquely
 * define the inverse coordinate operation (from CRS B to CRS A). In some cases,
 * the coordinate operation method algorithm for the inverse coordinate
 * operation is the same as for the forward algorithm, but the signs of some
 * coordinate operation parameter values have to be reversed. In other cases,
 * different algorithms are required for the forward and inverse coordinate
 * operations, but the same coordinate operation parameter values are used. If
 * (some) entirely different parameter values are needed, a different coordinate
 * operation shall be defined.
 *
 * \remark Implements CoordinateOperation from \ref ISO_19111_2018
 */
class CoordinateOperation : public common::ObjectUsage,
                            public io::IWKTExportable,
                            public io::IPROJStringExportable,
                            public util::IComparable {
  public:
    //! @cond Doxygen_Suppress
    PROJ_DLL ~CoordinateOperation() override;
    //! @endcond

    PROJ_DLL const util::optional<std::string> &operationVersion() const;
    PROJ_DLL const std::vector<metadata::PositionalAccuracyNNPtr> &
    coordinateOperationAccuracies() const;

    PROJ_DLL const crs::CRSPtr sourceCRS() const;
    PROJ_DLL const crs::CRSPtr targetCRS() const;
    PROJ_DLL const crs::CRSPtr &interpolationCRS() const;
    PROJ_DLL const util::optional<common::DataEpoch> &
    sourceCoordinateEpoch() const;
    PROJ_DLL const util::optional<common::DataEpoch> &
    targetCoordinateEpoch() const;

    // virtual void transform(...) = 0;  TODO

    /** \brief Return the inverse of the coordinate operation.
     * @throw util::UnsupportedOperationException
     */
    PROJ_DLL virtual CoordinateOperationNNPtr inverse() const = 0;

    std::string exportToWKT(io::WKTFormatterNNPtr formatter) const override = 0;
    std::string exportToPROJString(
        io::PROJStringFormatterNNPtr formatter) const override = 0;

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

/** \brief Abstract class modelling a parameter value (OperationParameter)
 * or group of parameters.
 *
 * \remark Implements GeneralOperationParameter from \ref ISO_19111_2018
 */
class GeneralOperationParameter : public common::IdentifiedObject,
                                  public util::IComparable {
  public:
    //! @cond Doxygen_Suppress
    PROJ_DLL ~GeneralOperationParameter() override;
    //! @endcond

    PROJ_DLL bool
    isEquivalentTo(const util::BaseObjectNNPtr &other,
                   util::IComparable::Criterion criterion =
                       util::IComparable::Criterion::STRICT) const override = 0;

  protected:
    GeneralOperationParameter();
    GeneralOperationParameter(const GeneralOperationParameter &other);

  private:
    PROJ_OPAQUE_PRIVATE_DATA
    GeneralOperationParameter &
    operator=(const GeneralOperationParameter &other) = delete;
};

/** Shared pointer of GeneralOperationParameter */
using GeneralOperationParameterPtr = std::shared_ptr<GeneralOperationParameter>;
/** Non-null shared pointer of GeneralOperationParameter */
using GeneralOperationParameterNNPtr = util::nn<GeneralOperationParameterPtr>;

// ---------------------------------------------------------------------------

class OperationParameter;
/** Shared pointer of OperationParameter */
using OperationParameterPtr = std::shared_ptr<OperationParameter>;
/** Non-null shared pointer of OperationParameter */
using OperationParameterNNPtr = util::nn<OperationParameterPtr>;

/** \brief The definition of a parameter used by a coordinate operation method.
 *
 * Most parameter values are numeric, but other types of parameter values are
 * possible.
 *
 * \remark Implements OperationParameter from \ref ISO_19111_2018
 */
class OperationParameter : public GeneralOperationParameter {
  public:
    //! @cond Doxygen_Suppress
    PROJ_DLL ~OperationParameter() override;
    //! @endcond

    PROJ_DLL bool
    isEquivalentTo(const util::BaseObjectNNPtr &other,
                   util::IComparable::Criterion criterion =
                       util::IComparable::Criterion::STRICT) const override;

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

//! @cond Doxygen_Suppress
struct MethodMapping;
//! @endcond

/** \brief Abstract class modelling a parameter value (OperationParameterValue)
 * or group of parameter values.
 *
 * \remark Implements GeneralParameterValue from \ref ISO_19111_2018
 */
class GeneralParameterValue : public util::BaseObject,
                              public io::IWKTExportable,
                              public util::IComparable {
  public:
    //! @cond Doxygen_Suppress
    PROJ_DLL ~GeneralParameterValue() override;
    //! @endcond

    PROJ_DLL std::string exportToWKT(io::WKTFormatterNNPtr formatter)
        const override = 0; // throw(io::FormattingException)

    PROJ_DLL bool
    isEquivalentTo(const util::BaseObjectNNPtr &other,
                   util::IComparable::Criterion criterion =
                       util::IComparable::Criterion::STRICT) const override = 0;

  protected:
    //! @cond Doxygen_Suppress
    GeneralParameterValue();
    GeneralParameterValue(const GeneralParameterValue &other);

    friend class Conversion;
    friend class Transformation;
    virtual std::string _exportToWKT(io::WKTFormatterNNPtr formatter,
                                     const MethodMapping *mapping)
        const = 0; // throw(io::FormattingException)
                   //! @endcond

  private:
    PROJ_OPAQUE_PRIVATE_DATA
    GeneralParameterValue &
    operator=(const GeneralParameterValue &other) = delete;
};

/** Shared pointer of GeneralParameterValue */
using GeneralParameterValuePtr = std::shared_ptr<GeneralParameterValue>;
/** Non-null shared pointer of GeneralParameterValue */
using GeneralParameterValueNNPtr = util::nn<GeneralParameterValuePtr>;

// ---------------------------------------------------------------------------

class ParameterValue;
/** Shared pointer of ParameterValue */
using ParameterValuePtr = std::shared_ptr<ParameterValue>;
/** Non-null shared pointer of ParameterValue */
using ParameterValueNNPtr = util::nn<ParameterValuePtr>;

/** \brief The value of the coordinate operation parameter.
 *
 * Most parameter values are numeric, but other types of parameter values are
 * possible.
 *
 * \remark Implements ParameterValue from \ref ISO_19111_2018
 */
class ParameterValue : public util::BaseObject,
                       public io::IWKTExportable,
                       public util::IComparable {
  public:
    /** Type of the value. */
    enum class Type {
        /** Measure (i.e. value with a unit) */
        MEASURE,
        /** String */
        STRING,
        /** Integer */
        INTEGER,
        /** Boolean */
        BOOLEAN,
        /** Filename */
        FILENAME
    };
    //! @cond Doxygen_Suppress
    PROJ_DLL ~ParameterValue() override;
    //! @endcond
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

    PROJ_DLL bool
    isEquivalentTo(const util::BaseObjectNNPtr &other,
                   util::IComparable::Criterion criterion =
                       util::IComparable::Criterion::STRICT) const override;

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
/** Shared pointer of OperationParameterValue */
using OperationParameterValuePtr = std::shared_ptr<OperationParameterValue>;
/** Non-null shared pointer of OperationParameterValue */
using OperationParameterValueNNPtr = util::nn<OperationParameterValuePtr>;

/** \brief A parameter value, ordered sequence of values, or reference to a
 * file of parameter values.
 *
 * This combines a OperationParameter with the corresponding ParameterValue.
 *
 * \remark Implements OperationParameterValue from \ref ISO_19111_2018
 */
class OperationParameterValue : public GeneralParameterValue {
  public:
    //! @cond Doxygen_Suppress
    PROJ_DLL ~OperationParameterValue() override;
    //! @endcond

    PROJ_DLL const OperationParameterNNPtr &parameter() const;
    PROJ_DLL const ParameterValueNNPtr &parameterValue() const;

    PROJ_DLL std::string exportToWKT(io::WKTFormatterNNPtr formatter)
        const override; // throw(io::FormattingException)

    PROJ_DLL bool
    isEquivalentTo(const util::BaseObjectNNPtr &other,
                   util::IComparable::Criterion criterion =
                       util::IComparable::Criterion::STRICT) const override;

    PROJ_DLL static OperationParameterValueNNPtr
    create(OperationParameterNNPtr parameterIn, ParameterValueNNPtr valueIn);

    //! @cond Doxygen_Suppress
    static bool convertFromAbridged(const std::string &paramName, double &val,
                                    common::UnitOfMeasure &unit,
                                    int &paramEPSGCode);
    //! @endcond

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
/** Shared pointer of OperationMethod */
using OperationMethodPtr = std::shared_ptr<OperationMethod>;
/** Non-null shared pointer of OperationMethod */
using OperationMethodNNPtr = util::nn<OperationMethodPtr>;

/** \brief The method (algorithm or procedure) used to perform the
 * coordinate operation.
 *
 * For a projection method, this contains the name of the projection method
 * and the name of the projection parameters.
 *
 * \remark Implements OperationMethod from \ref ISO_19111_2018
 */
class OperationMethod : public common::IdentifiedObject,
                        public io::IWKTExportable,
                        public util::IComparable {
  public:
    //! @cond Doxygen_Suppress
    PROJ_DLL ~OperationMethod() override;
    //! @endcond

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

    PROJ_DLL bool
    isEquivalentTo(const util::BaseObjectNNPtr &other,
                   util::IComparable::Criterion criterion =
                       util::IComparable::Criterion::STRICT) const override;

  protected:
    OperationMethod();
    OperationMethod(const OperationMethod &other);
    INLINED_MAKE_SHARED

  private:
    PROJ_OPAQUE_PRIVATE_DATA
    OperationMethod &operator=(const OperationMethod &other) = delete;
};

// ---------------------------------------------------------------------------

/** \brief Exception that can be thrown when an invalid operation is attempted
 * to be constructed.
 */
class InvalidOperation : public util::Exception {
  public:
    //! @cond Doxygen_Suppress
    PROJ_DLL explicit InvalidOperation(const char *message);
    PROJ_DLL explicit InvalidOperation(const std::string &message);
    PROJ_DLL InvalidOperation(const InvalidOperation &other);
    PROJ_DLL ~InvalidOperation() override;
    //! @endcond
};

// ---------------------------------------------------------------------------

class SingleOperation;
/** Shared pointer of SingleOperation */
using SingleOperationPtr = std::shared_ptr<SingleOperation>;
/** Non-null shared pointer of SingleOperation */
using SingleOperationNNPtr = util::nn<SingleOperationPtr>;

/** \brief A single (not concatenated) coordinate operation
 * (CoordinateOperation)
 *
 * \remark Implements SingleOperation from \ref ISO_19111_2018
 */
class SingleOperation : public CoordinateOperation {
  public:
    //! @cond Doxygen_Suppress
    PROJ_DLL ~SingleOperation() override;
    //! @endcond

    PROJ_DLL const std::vector<GeneralParameterValueNNPtr> &
    parameterValues() const;
    PROJ_DLL const OperationMethodNNPtr &method() const;

    PROJ_DLL ParameterValuePtr parameterValue(const std::string &paramName,
                                              int epsg_code = 0) const;
    PROJ_DLL common::Measure parameterValueMeasure(const std::string &paramName,
                                                   int epsg_code = 0) const;

    PROJ_DLL static SingleOperationNNPtr
    createPROJBased(const std::string &PROJString, const crs::CRSPtr sourceCRS,
                    const crs::CRSPtr targetCRS);

    PROJ_DLL bool
    isEquivalentTo(const util::BaseObjectNNPtr &other,
                   util::IComparable::Criterion criterion =
                       util::IComparable::Criterion::STRICT) const override;

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
/** Shared pointer of Conversion */
using ConversionPtr = std::shared_ptr<Conversion>;
/** Non-null shared pointer of Conversion */
using ConversionNNPtr = util::nn<ConversionPtr>;

/** \brief A mathematical operation on coordinates in which the parameter
 * values are defined rather than empirically derived.
 *
 * Application of the coordinate conversion introduces no error into output
 * coordinates. The best-known example of a coordinate conversion is a map
 * projection. For coordinate conversions the output coordinates are referenced
 * to the same datum as are the input coordinates.
 *
 * Coordinate conversions forming a component of a derived CRS have a source
 * crs::CRS and a target crs::CRS that are NOT specified through the source and
 * target
 * associations, but through associations from crs::DerivedCRS to
 * crs::SingleCRS.
 *
 * \remark Implements Conversion from \ref ISO_19111_2018
 */

/*!

\section projection_parameters Projection parameters

\subsection center_latitude Center latitude

\subsection center_longitude Center longitude

\subsection scale Scale

\subsection false_easting False easting

\subsection false_northing False northing
*/

class Conversion : public SingleOperation {
  public:
    //! @cond Doxygen_Suppress
    PROJ_DLL ~Conversion() override;
    //! @endcond

    PROJ_DLL CoordinateOperationNNPtr inverse() const override;

    PROJ_DLL std::string exportToWKT(io::WKTFormatterNNPtr formatter)
        const override; // throw(io::FormattingException)

    PROJ_DLL std::string
    exportToPROJString(io::PROJStringFormatterNNPtr formatter)
        const override; // throw(FormattingException)

    PROJ_DLL bool isUTM(int &zone, bool &north) const;

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

    PROJ_DLL static ConversionNNPtr
    createUTM(const util::PropertyMap &properties, int zone, bool north);

    PROJ_DLL static ConversionNNPtr
    createTM(const util::PropertyMap &properties,
             const common::Angle &centerLat, const common::Angle &centerLong,
             const common::Scale &scale, const common::Length &falseEasting,
             const common::Length &falseNorthing);

    PROJ_DLL static ConversionNNPtr
    createLCC_1SP(const util::PropertyMap &properties,
                  const common::Angle &centerLat,
                  const common::Angle &centerLong, const common::Scale &scale,
                  const common::Length &falseEasting,
                  const common::Length &falseNorthing);

    PROJ_DLL static ConversionNNPtr
    createNZMG(const util::PropertyMap &properties,
               const common::Angle &centerLat, const common::Angle &centerLong,
               const common::Length &falseEasting,
               const common::Length &falseNorthing);

  protected:
    Conversion(const OperationMethodNNPtr &methodIn,
               const std::vector<GeneralParameterValueNNPtr> &values);
    Conversion(const Conversion &other);
    INLINED_MAKE_SHARED

  private:
    PROJ_OPAQUE_PRIVATE_DATA
    Conversion &operator=(const Conversion &other) = delete;

    static ConversionNNPtr
    create(const util::PropertyMap &properties, int method_epsg_code,
           const std::vector<ParameterValueNNPtr> &values);
};

// ---------------------------------------------------------------------------

class Transformation;
/** Shared pointer of Transformation */
using TransformationPtr = std::shared_ptr<Transformation>;
/** Non-null shared pointer of Transformation */
using TransformationNNPtr = util::nn<TransformationPtr>;

/** \brief A mathematical operation on coordinates in which parameters are
 * empirically derived from data containing the coordinates of a series of
 * points in both coordinate reference systems.
 *
 * This computational process is usually "over-determined", allowing derivation
 * of error (or accuracy) estimates for the coordinate transformation. Also,
 * the stochastic nature of the parameters may result in multiple (different)
 * versions of the same coordinate transformations between the same source and
 * target CRSs. Any single coordinate operation in which the input and output
 * coordinates are referenced to different datums (reference frames) will be a
 * coordinate transformation.
 *
 * \remark Implements Transformation from \ref ISO_19111_2018
 */
class Transformation : public SingleOperation {
  public:
    //! @cond Doxygen_Suppress
    PROJ_DLL ~Transformation() override;
    //! @endcond

    PROJ_DLL const crs::CRSNNPtr sourceCRS() const;
    PROJ_DLL const crs::CRSNNPtr targetCRS() const;

    PROJ_DLL CoordinateOperationNNPtr inverse() const override;

    PROJ_DLL std::string exportToWKT(io::WKTFormatterNNPtr formatter)
        const override; // throw(io::FormattingException)

    PROJ_DLL std::string
    exportToPROJString(io::PROJStringFormatterNNPtr formatter) const override;

    PROJ_DLL std::vector<double>
    getTOWGS84Parameters() const; // throw(io::FormattingException)

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

    PROJ_DLL static TransformationNNPtr createGeocentricTranslations(
        const util::PropertyMap &properties, const crs::CRSNNPtr &sourceCRSIn,
        const crs::CRSNNPtr &targetCRSIn, double translationXMetre,
        double translationYMetre, double translationZMetre,
        const std::vector<metadata::PositionalAccuracyNNPtr> &accuracies);

    PROJ_DLL static TransformationNNPtr createPositionVector(
        const util::PropertyMap &properties, const crs::CRSNNPtr &sourceCRSIn,
        const crs::CRSNNPtr &targetCRSIn, double translationXMetre,
        double translationYMetre, double translationZMetre,
        double rotationXMicroRadian, double rotationYMicroRadian,
        double rotationZMicroRadian, double scaleDifferencePPM,
        const std::vector<metadata::PositionalAccuracyNNPtr> &accuracies);

    PROJ_DLL static TransformationNNPtr createCoordinateFrameRotation(
        const util::PropertyMap &properties, const crs::CRSNNPtr &sourceCRSIn,
        const crs::CRSNNPtr &targetCRSIn, double translationXMetre,
        double translationYMetre, double translationZMetre,
        double rotationXMicroRadian, double rotationYMicroRadian,
        double rotationZMicroRadian, double scaleDifferencePPM,
        const std::vector<metadata::PositionalAccuracyNNPtr> &accuracies);

    PROJ_DLL static TransformationNNPtr createTOWGS84(
        const crs::CRSNNPtr &sourceCRSIn,
        const std::vector<double> &TOWGS84Parameters); // throw InvalidOperation

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
/** Shared pointer of PointMotionOperation */
using PointMotionOperationPtr = std::shared_ptr<PointMotionOperation>;
/** Non-null shared pointer of PointMotionOperation */
using PointMotionOperationNNPtr = util::nn<PointMotionOperationPtr>;

/** \brief A mathematical operation that describes the change of coordinate
 * values within one coordinate reference system due to the motion of the
 * point between one coordinate epoch and another coordinate epoch.
 *
 * The motion is due to tectonic plate movement or deformation.
 *
 * \remark Implements PointMotionOperation from \ref ISO_19111_2018
 */
class PointMotionOperation : public SingleOperation {
  public:
    // TODO
    //! @cond Doxygen_Suppress
    PROJ_DLL ~PointMotionOperation() override;
    //! @endcond

  private:
    PointMotionOperation(const PointMotionOperation &) = delete;
};

// ---------------------------------------------------------------------------

class ConcatenatedOperation;
/** Shared pointer of ConcatenatedOperation */
using ConcatenatedOperationPtr = std::shared_ptr<ConcatenatedOperation>;
/** Non-null shared pointer of ConcatenatedOperation */
using ConcatenatedOperationNNPtr = util::nn<ConcatenatedOperationPtr>;

/** \brief An ordered sequence of two or more single coordinate operations
 * (SingleOperation).
 *
 * The sequence of coordinate operations is constrained by the requirement that
 * the source coordinate reference system of step n+1 shall be the same as
 * the target coordinate reference system of step n.
 *
 * \remark Implements ConcatenatedOperation from \ref ISO_19111_2018
 */
class ConcatenatedOperation : public CoordinateOperation {
  public:
    //! @cond Doxygen_Suppress
    PROJ_DLL ~ConcatenatedOperation() override;
    //! @endcond

    PROJ_DLL const std::vector<CoordinateOperationNNPtr> &operations() const;

    PROJ_DLL CoordinateOperationNNPtr inverse() const override;

    PROJ_DLL std::string exportToWKT(io::WKTFormatterNNPtr formatter)
        const override; // throw(io::FormattingException)

    PROJ_DLL std::string
    exportToPROJString(io::PROJStringFormatterNNPtr formatter) const override;

    PROJ_DLL bool
    isEquivalentTo(const util::BaseObjectNNPtr &other,
                   util::IComparable::Criterion criterion =
                       util::IComparable::Criterion::STRICT) const override;

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

// ---------------------------------------------------------------------------

class CoordinateOperationFactory;
/** Shared pointer of CoordinateOperationFactory */
using CoordinateOperationFactoryPtr =
    std::shared_ptr<CoordinateOperationFactory>;
/** Non-null shared pointer of CoordinateOperationFactory */
using CoordinateOperationFactoryNNPtr = util::nn<CoordinateOperationFactoryPtr>;

/** \brief Creates coordinate operations. This factory is capable to find
 * coordinate transformations or conversions between two coordinate reference
 * systems.
 *
 * \remark Implements (partially) CoordinateOperationFactory from \ref GeoAPI
 */
class CoordinateOperationFactory {
  public:
    //! @cond Doxygen_Suppress
    PROJ_DLL virtual ~CoordinateOperationFactory();
    //! @endcond

    PROJ_DLL CoordinateOperationPtr
    createOperation(crs::CRSNNPtr sourceCRS, crs::CRSNNPtr targetCRS) const;

    PROJ_DLL static CoordinateOperationFactoryNNPtr create();

  protected:
    CoordinateOperationFactory();
    INLINED_MAKE_SHARED

  private:
    PROJ_OPAQUE_PRIVATE_DATA
};

} // namespace operation

NS_PROJ_END

#endif //  COORDINATEOPERATION_HH_INCLUDED
