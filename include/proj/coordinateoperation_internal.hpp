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

#ifndef FROM_PROJ_CPP
#error This file should only be included from a PROJ cpp file
#endif

#ifndef COORDINATEOPERATION_INTERNAL_HH_INCLUDED
#define COORDINATEOPERATION_INTERNAL_HH_INCLUDED

#include "coordinateoperation.hpp"

#include <vector>

//! @cond Doxygen_Suppress

NS_PROJ_START

namespace operation {

/* Just std::vector<std::string> but with a friendly initializer in case w
 * have just a single string (which is most of cases) */
struct VectorOfString : public std::vector<std::string> {
    VectorOfString(const char *str) : std::vector<std::string>{str} {}
    VectorOfString(std::initializer_list<std::string> l)
        : std::vector<std::string>{l} {}
};

struct ParamMapping {
    const std::string wkt2_name;
    const int epsg_code;
    const std::string wkt1_name;
    const common::UnitOfMeasure::Type unit_type;
    const VectorOfString proj_names;
};

struct MethodMapping {
    const std::string wkt2_name;
    const int epsg_code;
    const std::string wkt1_name;
    const VectorOfString proj_names;
    const std::vector<ParamMapping> params;
};

const MethodMapping *getMapping(int epsg_code);
const MethodMapping *getMappingFromWKT1(const std::string &wkt1_name);
const MethodMapping *getMapping(const std::string &wkt2_name);
const MethodMapping *getMapping(const OperationMethod *method);
std::vector<const MethodMapping *>
getMappingsFromPROJName(const std::string &projName);
const ParamMapping *getMapping(const MethodMapping *mapping,
                               const OperationParameterValue *param);
const ParamMapping *getMappingFromWKT1(const MethodMapping *mapping,
                                       const std::string &wkt1_name);

// ---------------------------------------------------------------------------

class InverseCoordinateOperation;
/** Shared pointer of InverseCoordinateOperation */
using InverseCoordinateOperationPtr =
    std::shared_ptr<InverseCoordinateOperation>;
/** Non-null shared pointer of InverseCoordinateOperation */
using InverseCoordinateOperationNNPtr = util::nn<InverseCoordinateOperationPtr>;

/** \brief Inverse operation of a CoordinateOperation.
 *
 * This is used when there is no straightforward way of building another
 * subclass of CoordinateOperation that models the inverse operation.
 */
class InverseCoordinateOperation : virtual public CoordinateOperation {
  public:
    InverseCoordinateOperation(CoordinateOperationNNPtr forwardOperation,
                               bool wktSupportsInversion);

    ~InverseCoordinateOperation() override;

    std::string
    exportToPROJString(io::PROJStringFormatterNNPtr formatter) const override;

    bool
    isEquivalentTo(const util::BaseObjectNNPtr &other,
                   util::IComparable::Criterion criterion =
                       util::IComparable::Criterion::STRICT) const override;

    CoordinateOperationNNPtr inverse() const override;

  protected:
    CoordinateOperationNNPtr forwardOperation_;
    bool wktSupportsInversion_;

    void setPropertiesFromForward();
};

// ---------------------------------------------------------------------------

/** \brief Inverse of a conversion. */
class InverseConversion : public Conversion, public InverseCoordinateOperation {
  public:
    InverseConversion(ConversionNNPtr forward);

    ~InverseConversion() override;

    std::string exportToWKT(io::WKTFormatterNNPtr formatter) const override {
        return Conversion::exportToWKT(formatter);
    }

    std::string
    exportToPROJString(io::PROJStringFormatterNNPtr formatter) const override {
        return InverseCoordinateOperation::exportToPROJString(formatter);
    }

    bool
    isEquivalentTo(const util::BaseObjectNNPtr &other,
                   util::IComparable::Criterion criterion =
                       util::IComparable::Criterion::STRICT) const override {
        return InverseCoordinateOperation::isEquivalentTo(other, criterion);
    }

    CoordinateOperationNNPtr inverse() const override {
        return InverseCoordinateOperation::inverse();
    }

    ConversionNNPtr inverseAsConversion() const;

#ifdef _MSC_VER
    // To avoid a warning C4250: 'osgeo::proj::operation::InverseConversion':
    // inherits
    // 'osgeo::proj::operation::SingleOperation::osgeo::proj::operation::SingleOperation::gridsNeeded'
    // via dominance
    std::set<GridDescription>
    gridsNeeded(io::DatabaseContextNNPtr databaseContext) const override {
        return SingleOperation::gridsNeeded(databaseContext);
    }
#endif

    static CoordinateOperationNNPtr create(ConversionNNPtr forward);
};

// ---------------------------------------------------------------------------

/** \brief Inverse of a transformation. */
class InverseTransformation : public Transformation,
                              public InverseCoordinateOperation {
  public:
    InverseTransformation(TransformationNNPtr forward);

    ~InverseTransformation() override;

    std::string exportToWKT(io::WKTFormatterNNPtr formatter) const override;

    std::string
    exportToPROJString(io::PROJStringFormatterNNPtr formatter) const override {
        return InverseCoordinateOperation::exportToPROJString(formatter);
    }

    bool
    isEquivalentTo(const util::BaseObjectNNPtr &other,
                   util::IComparable::Criterion criterion =
                       util::IComparable::Criterion::STRICT) const override {
        return InverseCoordinateOperation::isEquivalentTo(other, criterion);
    }

    CoordinateOperationNNPtr inverse() const override {
        return InverseCoordinateOperation::inverse();
    }

#ifdef _MSC_VER
    // To avoid a warning C4250:
    // 'osgeo::proj::operation::InverseTransformation': inherits
    // 'osgeo::proj::operation::SingleOperation::osgeo::proj::operation::SingleOperation::gridsNeeded'
    // via dominance
    std::set<GridDescription>
    gridsNeeded(io::DatabaseContextNNPtr databaseContext) const override {
        return SingleOperation::gridsNeeded(databaseContext);
    }
#endif

    static TransformationNNPtr create(TransformationNNPtr forward);
};

// ---------------------------------------------------------------------------

class PROJBasedOperation;
/** Shared pointer of PROJBasedOperation */
using PROJBasedOperationPtr = std::shared_ptr<PROJBasedOperation>;
/** Non-null shared pointer of PROJBasedOperation */
using PROJBasedOperationNNPtr = util::nn<PROJBasedOperationPtr>;

/** \brief A PROJ-string based coordinate operation.
 */
class PROJBasedOperation : public SingleOperation {
  public:
    ~PROJBasedOperation() override;

    std::string exportToWKT(io::WKTFormatterNNPtr formatter)
        const override; // throw(io::FormattingException)

    std::string
    exportToPROJString(io::PROJStringFormatterNNPtr formatter) const override;

    CoordinateOperationNNPtr inverse() const override;

    static PROJBasedOperationNNPtr
    create(const util::PropertyMap &properties, const std::string &PROJString,
           const crs::CRSPtr sourceCRS, const crs::CRSPtr targetCRS,
           const std::vector<metadata::PositionalAccuracyNNPtr> &accuracies);

    std::set<GridDescription>
    gridsNeeded(io::DatabaseContextNNPtr databaseContext) const override;

  protected:
    PROJBasedOperation(const OperationMethodNNPtr &methodIn,
                       const std::vector<GeneralParameterValueNNPtr> &values);
    INLINED_MAKE_SHARED
};

} // namespace operation

NS_PROJ_END

//! @endcond

#endif // COORDINATEOPERATION_INTERNAL_HH_INCLUDED
