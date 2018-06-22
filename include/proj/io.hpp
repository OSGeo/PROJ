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

#ifndef IO_HH_INCLUDED
#define IO_HH_INCLUDED

#include <memory>
#include <string>

#include "util.hpp"

NS_PROJ_START

namespace common {
class UnitOfMeasure;
using UnitOfMeasurePtr = std::shared_ptr<UnitOfMeasure>;
using UnitOfMeasureNNPtr = util::nn<UnitOfMeasurePtr>;
}

/** osgeo.proj.io namespace.
 *
 * \brief I/O classes
 */
namespace io {

// ---------------------------------------------------------------------------

class WKTFormatter;
/** WKTFormatter shared pointer. */
using WKTFormatterPtr = std::shared_ptr<WKTFormatter>;
/** WKTFormatter non-null shared pointer. */
using WKTFormatterNNPtr = util::nn<WKTFormatterPtr>;

/** WKTFormatter class. */
class WKTFormatter {
  public:
    //! @cond Doxygen_Suppress
    /** WKT version. */
    enum class PROJ_DLL Version {
        /** WKT1 */
        WKT1,
        /** WKT2 / ISO 19162 */
        WKT2
    };
    //! @endcond

    /** WKT variant. */
    enum class PROJ_DLL Convention {
        /** Full WKT2 string, conforming to ISO 19162:2015(E) / OGC 12-063r5,
           with all possible nodes and new keyword names. */
        WKT2,
        WKT2_2015 = WKT2,

        /** Same as WKT2 with the following exceptions:
         * <ul>
         *      <li>UNIT keyword used.</li>
         *      <li>ID node only on top element.</li>
         *      <li>No ORDER element in AXIS element.</li>
         *      <li>PRIMEM node omitted if it is Greenwich.</li>
         *      <li>ELLIPSOID.UNIT node omitted if it is
         * UnitOfMeasure::METRE.</li>
         *      <li>PARAMETER.UNIT / PRIMEM.UNIT omitted if same as AXIS.</li>
         *      <li>AXIS.UNIT omitted and replaced by a common GEODCRS.UNIT if
         * they are all the same on all axis.</li>
         * </ul>
         */
        WKT2_SIMPLIFIED,

        /** Full WKT2 string, conforming to ISO 19162:2018 / OGC 18-010, with
         * all possible nodes and new keyword names.
         * Non-normative list of differences:
         * <ul>
         *      <li>WKT2_2018 uses GEOGCRS / BASEGEOGCRS keywords for
         * GeographicCRS.</li>
         * </ul>
         */
        WKT2_2018,

        /** WKT2_2018 with the simplification rule of WKT2_SIMPLIFIED */
        WKT2_2018_SIMPLIFIED,

        /** WKT1 as traditionaly output by GDAL */
        WKT1_GDAL
    };

    PROJ_DLL static WKTFormatterNNPtr
    create(Convention convention = Convention::WKT2);
    PROJ_DLL static WKTFormatterNNPtr create(WKTFormatterNNPtr other);

    PROJ_DLL ~WKTFormatter();

    PROJ_DLL WKTFormatter &setMultiLine(bool multiLine);
    PROJ_DLL WKTFormatter &setIndendationWidth(int width);
    PROJ_DLL WKTFormatter &setStrict(bool strict);
    PROJ_DLL bool isStrict() const;

    /** Returns the WKT string. */
    PROJ_DLL const std::string &toString() const;

    //! @cond Doxygen_Suppress
    PROJ_DLL WKTFormatter &setOutputId(bool outputIdIn);

    void startNode(const std::string &keyword, bool hasId);
    void endNode();
    void addQuotedString(const std::string &str);
    void add(const std::string &str);
    void add(int number);
    void add(size_t number);
    void add(double number, int precision = 15);

    void pushOutputUnit(bool outputUnitIn);
    void popOutputUnit();
    bool outputUnit() const;

    void pushOutputId(bool outputIdIn);
    void popOutputId();
    bool outputId() const;

    void pushAxisLinearUnit(const common::UnitOfMeasureNNPtr &unit);
    void popAxisLinearUnit();
    const common::UnitOfMeasureNNPtr &axisLinearUnit() const;

    void pushAxisAngularUnit(const common::UnitOfMeasureNNPtr &unit);
    void popAxisAngularUnit();
    const common::UnitOfMeasureNNPtr &axisAngularUnit() const;

    void setOutputConversionNode(bool outputIn);
    bool outputConversionNode() const;

    void setAbridgedTransformation(bool abriged);
    bool abridgedTransformation() const;

    void setUseDerivingConversion(bool useDerivingConversionIn);
    bool useDerivingConversion() const;

    void setTOWGS84Parameters(const std::vector<double> &params);
    const std::vector<double> &getTOWGS84Parameters() const;

    bool outputAxisOrder() const;
    bool primeMeridianOmittedIfGreenwich() const;
    bool ellipsoidUnitOmittedIfMetre() const;
    bool forceUNITKeyword() const;
    bool primeMeridianOrParameterUnitOmittedIfSameAsAxis() const;
    bool primeMeridianInDegree() const;
    bool outputCSUnitOnlyOnceIfSame() const;
    Version version() const;
    bool use2018Keywords() const;

    //! @endcond

  protected:
    //! @cond Doxygen_Suppress
    explicit WKTFormatter(Convention convention);
    WKTFormatter(const WKTFormatter &other) = delete;

    INLINED_MAKE_SHARED
    //! @endcond

  private:
    PROJ_OPAQUE_PRIVATE_DATA
};

// ---------------------------------------------------------------------------

class PROJStringFormatter;
/** PROJStringFormatter shared pointer. */
using PROJStringFormatterPtr = std::shared_ptr<PROJStringFormatter>;
/** PROJStringFormatter non-null shared pointer. */
using PROJStringFormatterNNPtr = util::nn<PROJStringFormatterPtr>;

/** PROJStringFormatter class. */
class PROJStringFormatter {
  public:
    //! @cond Doxygen_Suppress
    PROJ_DLL static PROJStringFormatterNNPtr create();

    PROJ_DLL ~PROJStringFormatter();

    /** Returns the PROJ string. */
    PROJ_DLL const std::string &toString() const;

    //! @cond Doxygen_Suppress
    PROJ_DLL void addStep(const std::string &step, bool inversed = false);
    PROJ_DLL void addParam(const std::string &paramName);
    PROJ_DLL void addParam(const std::string &paramName, double val);
    PROJ_DLL void addParam(const std::string &paramName, int val);
    PROJ_DLL void addParam(const std::string &paramName, const char *val);
    PROJ_DLL void addParam(const std::string &paramName,
                           const std::string &val);
    //! @endcond

  protected:
    //! @cond Doxygen_Suppress
    explicit PROJStringFormatter();
    PROJStringFormatter(const PROJStringFormatter &other) = delete;

    INLINED_MAKE_SHARED
    //! @endcond

  private:
    PROJ_OPAQUE_PRIVATE_DATA
};

// ---------------------------------------------------------------------------

/** Exception possibly thrown by IWKTExportable::exportToWKT() or
 * IPROJStringExportable::exportToPROJString(). */
class FormattingException : public util::Exception {
  public:
    /** Constructor. */
    PROJ_DLL explicit FormattingException(const char *message);
    /** Constructor. */
    PROJ_DLL explicit FormattingException(const std::string &message);
    PROJ_DLL FormattingException(const FormattingException &other);
    PROJ_DLL virtual ~FormattingException() override;
};

// ---------------------------------------------------------------------------

/** Exception possibly thrown by WKTNode::createFrom(). */
class ParsingException : public util::Exception {
  public:
    /** Constructor. */
    PROJ_DLL explicit ParsingException(const char *message);
    /** Constructor. */
    PROJ_DLL explicit ParsingException(const std::string &message);
    PROJ_DLL ParsingException(const ParsingException &other);
    PROJ_DLL virtual ~ParsingException() override;
};

// ---------------------------------------------------------------------------

/** Interface for an object that can be exported to WKT. */
class PROJ_DLL IWKTExportable {
  public:
    /** \brief Destructor */
    virtual ~IWKTExportable();

    /** Builds a WKT representation. May throw a FormattingException */
    virtual std::string exportToWKT(
        WKTFormatterNNPtr formatter) const = 0; // throw(FormattingException)
};

// ---------------------------------------------------------------------------

/** Interface for an object that can be exported to a PROJ string. */
class PROJ_DLL IPROJStringExportable {
  public:
    /** \brief Destructor */
    virtual ~IPROJStringExportable();

    /** Builds a PROJ string representation. May throw a FormattingException */
    virtual std::string exportToPROJString(PROJStringFormatterNNPtr formatter)
        const = 0; // throw(FormattingException)
};

// ---------------------------------------------------------------------------

class WKTNode;
using WKTNodePtr = std::shared_ptr<WKTNode>;
using WKTNodeNNPtr = util::nn<WKTNodePtr>;

class WKTNode {
  public:
    PROJ_DLL explicit WKTNode(const std::string &valueIn);
    PROJ_DLL ~WKTNode();

    PROJ_DLL const std::string &value() const;
    PROJ_DLL const std::vector<WKTNodeNNPtr> &children() const;

    PROJ_DLL void addChild(WKTNodeNNPtr child);
    PROJ_DLL WKTNodePtr lookForChild(const std::string &childName,
                                     int occurence = 0) const;
    PROJ_DLL int countChildrenOfName(const std::string &childName) const;

    PROJ_DLL std::string toString() const;

    PROJ_DLL static WKTNodeNNPtr createFrom(const std::string &wkt,
                                            size_t indexStart = 0);
    PROJ_DLL static WKTNodeNNPtr
    createFrom(const std::string &wkt, size_t indexStart, int recLevel,
               size_t &indexEnd); // throw(ParsingException)

  private:
    PROJ_OPAQUE_PRIVATE_DATA
};

// ---------------------------------------------------------------------------

class WKTParser {
  public:
    PROJ_DLL WKTParser();
    PROJ_DLL ~WKTParser();

    PROJ_DLL WKTParser &setStrict(bool strict);
    PROJ_DLL std::vector<std::string> warningList() const;

    PROJ_DLL util::BaseObjectNNPtr
    createFromWKT(const std::string &wkt); // throw(ParsingException)

  private:
    PROJ_OPAQUE_PRIVATE_DATA
};

} // namespace io

NS_PROJ_END

#endif // IO_HH_INCLUDED
