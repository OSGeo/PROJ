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

#include "proj/coordinateoperation.hpp"

#include <vector>

//! @cond Doxygen_Suppress

NS_PROJ_START

namespace operation {

struct ParamMapping {
    const char *wkt2_name;
    const int epsg_code;
    const char *wkt1_name;
    const common::UnitOfMeasure::Type unit_type;
    const char *proj_name;
};

struct MethodMapping {
    const char *wkt2_name;
    const int epsg_code;
    const char *wkt1_name;
    const char *proj_name_main;
    const char *proj_name_aux;
    const ParamMapping *const *params;
};

const MethodMapping *getMapping(int epsg_code) noexcept;
const MethodMapping *getMappingFromWKT1(const std::string &wkt1_name) noexcept;
const MethodMapping *getMapping(const char *wkt2_name) noexcept;
const MethodMapping *getMapping(const OperationMethod *method) noexcept;
std::vector<const MethodMapping *>
getMappingsFromPROJName(const std::string &projName);
const ParamMapping *getMapping(const MethodMapping *mapping,
                               const OperationParameterValue *param);
const ParamMapping *getMappingFromWKT1(const MethodMapping *mapping,
                                       const std::string &wkt1_name);
// ---------------------------------------------------------------------------

struct ESRIParamMapping {
    const char *esri_name;
    const char *wkt2_name;
    int epsg_code;
    float fixed_value;
};

struct ESRIMethodMapping {
    const char *esri_name;
    const char *wkt2_name;
    int epsg_code;
    const ESRIParamMapping *const params;
};

std::vector<const ESRIMethodMapping *>
getMappingsFromESRI(const std::string &esri_name);

// ---------------------------------------------------------------------------

bool isAxisOrderReversal(int methodEPSGCode);

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
    InverseCoordinateOperation(const CoordinateOperationNNPtr &forwardOperation,
                               bool wktSupportsInversion);

    ~InverseCoordinateOperation() override;

    void _exportToPROJString(io::PROJStringFormatter *formatter)
        const override; // throw(FormattingException)

    bool
    _isEquivalentTo(const util::IComparable *other,
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
    explicit InverseConversion(const ConversionNNPtr &forward);

    ~InverseConversion() override;

    void _exportToWKT(io::WKTFormatter *formatter) const override {
        Conversion::_exportToWKT(formatter);
    }

    void
    _exportToPROJString(io::PROJStringFormatter *formatter) const override {
        InverseCoordinateOperation::_exportToPROJString(formatter);
    }

    bool
    _isEquivalentTo(const util::IComparable *other,
                    util::IComparable::Criterion criterion =
                        util::IComparable::Criterion::STRICT) const override {
        return InverseCoordinateOperation::_isEquivalentTo(other, criterion);
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
    gridsNeeded(const io::DatabaseContextPtr &databaseContext) const override {
        return SingleOperation::gridsNeeded(databaseContext);
    }
#endif

    static CoordinateOperationNNPtr create(const ConversionNNPtr &forward);
};

// ---------------------------------------------------------------------------

/** \brief Inverse of a transformation. */
class InverseTransformation : public Transformation,
                              public InverseCoordinateOperation {
  public:
    explicit InverseTransformation(const TransformationNNPtr &forward);

    ~InverseTransformation() override;

    void _exportToWKT(io::WKTFormatter *formatter) const override;

    void
    _exportToPROJString(io::PROJStringFormatter *formatter) const override {
        return InverseCoordinateOperation::_exportToPROJString(formatter);
    }

    bool
    _isEquivalentTo(const util::IComparable *other,
                    util::IComparable::Criterion criterion =
                        util::IComparable::Criterion::STRICT) const override {
        return InverseCoordinateOperation::_isEquivalentTo(other, criterion);
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
    gridsNeeded(const io::DatabaseContextPtr &databaseContext) const override {
        return SingleOperation::gridsNeeded(databaseContext);
    }
#endif

    static TransformationNNPtr create(const TransformationNNPtr &forward);
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

    void _exportToWKT(io::WKTFormatter *formatter)
        const override; // throw(io::FormattingException)

    CoordinateOperationNNPtr inverse() const override;

    static PROJBasedOperationNNPtr
    create(const util::PropertyMap &properties, const std::string &PROJString,
           const crs::CRSPtr &sourceCRS, const crs::CRSPtr &targetCRS,
           const std::vector<metadata::PositionalAccuracyNNPtr> &accuracies);

    static PROJBasedOperationNNPtr
    create(const util::PropertyMap &properties,
           const io::IPROJStringExportableNNPtr &projExportable, bool inverse,
           const crs::CRSNNPtr &sourceCRS, const crs::CRSNNPtr &targetCRS,
           const std::vector<metadata::PositionalAccuracyNNPtr> &accuracies);

    std::set<GridDescription>
    gridsNeeded(const io::DatabaseContextPtr &databaseContext) const override;

  protected:
    PROJBasedOperation(const OperationMethodNNPtr &methodIn,
                       const std::vector<GeneralParameterValueNNPtr> &values);

    void _exportToPROJString(io::PROJStringFormatter *formatter)
        const override; // throw(FormattingException)

    INLINED_MAKE_SHARED

  private:
    io::IPROJStringExportablePtr projStringExportable_{};
    bool inverse_ = false;
};

} // namespace operation

NS_PROJ_END

//! @endcond

#endif // COORDINATEOPERATION_INTERNAL_HH_INCLUDED
