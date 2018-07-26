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

/** \brief Formatter to WKT strings.
 *
 * This class is thread-safe, but one instance can only be used by a single
 * thread at a time.
 */
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

        /** WKT1 as traditionally output by GDAL */
        WKT1_GDAL
    };

    PROJ_DLL static WKTFormatterNNPtr
    create(Convention convention = Convention::WKT2);
    PROJ_DLL static WKTFormatterNNPtr create(WKTFormatterNNPtr other);
    //! @cond Doxygen_Suppress
    PROJ_DLL ~WKTFormatter();
    //! @endcond

    PROJ_DLL WKTFormatter &setMultiLine(bool multiLine);
    PROJ_DLL WKTFormatter &setIndendationWidth(int width);
    PROJ_DLL WKTFormatter &setStrict(bool strict);
    PROJ_DLL bool isStrict() const;

    PROJ_DLL const std::string &toString() const;

    //! @cond Doxygen_Suppress
    PROJ_DLL WKTFormatter &setOutputId(bool outputIdIn);

    void enter();
    void leave();

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

    void setAbridgedTransformation(bool abriged);
    bool abridgedTransformation() const;

    void setUseDerivingConversion(bool useDerivingConversionIn);
    bool useDerivingConversion() const;

    void setTOWGS84Parameters(const std::vector<double> &params);
    const std::vector<double> &getTOWGS84Parameters() const;

    void setVDatumExtension(const std::string &filename);
    const std::string &getVDatumExtension() const;

    void setHDatumExtension(const std::string &filename);
    const std::string &getHDatumExtension() const;

    void startInversion();
    void stopInversion();
    bool isInverted() const;

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

/** \brief Formatter to PROJ strings.
 *
 * This class is thread-safe, but one instance can only be used by a single
 * thread at a time.
 */
class PROJStringFormatter {
  public:
    /** PROJ variant. */
    enum class PROJ_DLL Convention {
        /** PROJ v5 string. */
        PROJ_5,

        /** PROJ v4 string as output by GDAL exportToProj4() */
        PROJ_4
    };

    PROJ_DLL static PROJStringFormatterNNPtr
    create(Convention conventionIn = Convention::PROJ_5);
    //! @cond Doxygen_Suppress
    PROJ_DLL ~PROJStringFormatter();
    //! @endcond

    PROJ_DLL const std::string &toString() const;

    //! @cond Doxygen_Suppress

    class Scope {
      public:
        explicit Scope(PROJStringFormatterNNPtr formatter);
        ~Scope();
        std::string toString();

      private:
        PROJStringFormatterPtr formatter_;
    };

    PROJ_DLL void startInversion();
    PROJ_DLL void stopInversion();
    bool isInverted() const;

    PROJ_DLL void
    ingestPROJString(const std::string &str); // throw ParsingException

    PROJ_DLL void addStep(const std::string &step);
    PROJ_DLL void setCurrentStepInverted(bool inverted);
    PROJ_DLL void addParam(const std::string &paramName);
    PROJ_DLL void addParam(const std::string &paramName, double val);
    PROJ_DLL void addParam(const std::string &paramName, int val);
    PROJ_DLL void addParam(const std::string &paramName, const char *val);
    PROJ_DLL void addParam(const std::string &paramName,
                           const std::string &val);
    PROJ_DLL void addParam(const std::string &paramName,
                           const std::vector<double> &vals);

    void setTOWGS84Parameters(const std::vector<double> &params);
    const std::vector<double> &getTOWGS84Parameters() const;

    void setVDatumExtension(const std::string &filename);
    const std::string &getVDatumExtension() const;

    void setHDatumExtension(const std::string &filename);
    const std::string &getHDatumExtension() const;

    void setOmitProjLongLatIfPossible(bool omit);
    bool omitProjLongLatIfPossible() const;

    Convention convention() const;

    //! @endcond

  protected:
    //! @cond Doxygen_Suppress
    explicit PROJStringFormatter(Convention conventionIn);
    PROJStringFormatter(const PROJStringFormatter &other) = delete;

    INLINED_MAKE_SHARED
    //! @endcond

  private:
    PROJ_OPAQUE_PRIVATE_DATA

    void enter();
    void leave();
};

// ---------------------------------------------------------------------------

/** \brief Exception possibly thrown by IWKTExportable::exportToWKT() or
 * IPROJStringExportable::exportToPROJString(). */
class FormattingException : public util::Exception {
  public:
    //! @cond Doxygen_Suppress
    PROJ_DLL explicit FormattingException(const char *message);
    PROJ_DLL explicit FormattingException(const std::string &message);
    PROJ_DLL FormattingException(const FormattingException &other);
    PROJ_DLL virtual ~FormattingException() override;
    //! @endcond
};

// ---------------------------------------------------------------------------

/** \brief Exception possibly thrown by WKTNode::createFrom() or
 * WKTParser::createFromWKT(). */
class ParsingException : public util::Exception {
  public:
    //! @cond Doxygen_Suppress
    PROJ_DLL explicit ParsingException(const char *message);
    PROJ_DLL explicit ParsingException(const std::string &message);
    PROJ_DLL ParsingException(const ParsingException &other);
    PROJ_DLL virtual ~ParsingException() override;
    //! @endcond
};

// ---------------------------------------------------------------------------

/** \brief Interface for an object that can be exported to WKT. */
class PROJ_DLL IWKTExportable {
  public:
    //! @cond Doxygen_Suppress
    virtual ~IWKTExportable();
    //! @endcond

    /** Builds a WKT representation. May throw a FormattingException */
    virtual std::string exportToWKT(
        WKTFormatterNNPtr formatter) const = 0; // throw(FormattingException)
};

// ---------------------------------------------------------------------------

/** \brief Interface for an object that can be exported to a PROJ string. */
class PROJ_DLL IPROJStringExportable {
  public:
    //! @cond Doxygen_Suppress
    virtual ~IPROJStringExportable();
    //! @endcond

    /** \brief Builds a PROJ string representation.
     *
     * <ul>
     * <li>For PROJStringFormatter::Convention::PROJ_5 (the default), return
     * strings that generally express PROJ.5 pipelines.
     * <ul>
     * <li>For a crs::GeographicCRS, returns a string expressing the
     * transformation from geographic coordinates expressed in radian with
     * longitude, latitude order, and with respect to the international
     * reference meridian, into geographic coordinates expressed in the units
     * and axis order of the CRS, taking into account its prime meridian.</li>
     * <li>For a geocentric crs::GeodeticCRS, returns a string expressing the
     * transformation from geographic coordinates expressed in radian with
     * longitude, latitude order, and with respect to the international
     * reference meridian, into geocentric coordinates.</li>
     * <li>For a crs::ProjectedCRS, returns a string expressing the
     * transformation from the base CRS to the crs::ProjectedCRS.</li>
     * <li>For a crs::BoundCRS, throws a FormattingException.</li>
     * <li>For operation::CoordinateTransformations, returns a PROJ
     * pipeline.</li>
     * </ul>
     *
     * <li>For PROJStringFormatter::Convention::PROJ_4, format a string
     * compatible with the OGRSpatialReference::exportToProj4() of GDAL
     * &lt;=2.3.
     * <ul>
     * <li>For a crs::GeographicCRS, returns a proj=longlat string, with
     * ellipsoid / datum / prime meridian information, ignoring axis order
     * and unit information.</li>
     * <li>For a geocentric crs::GeodeticCRS, returns the transformation from
     * geographic coordinates into geocentric coordinates.</li>
     * <li>For a crs::ProjectedCRS, returns the projection method, ignoring
     * axis order.</li>
     * <li>For a crs::BoundCRS, returns the PROJ string of its source/base CRS,
     * amended with towgs84 / nadgrids parameter when the deriving conversion
     * can be expressed in that way.</li>
     * </ul>
     * </li>
     *
     * </ul>
     *
     * @param formatter PROJ string formatter.
     * @return a PROJ string.
     * @throw FormattingException */
    virtual std::string exportToPROJString(PROJStringFormatterNNPtr formatter)
        const = 0; // throw(FormattingException)
};

// ---------------------------------------------------------------------------

class WKTNode;
/** Shared pointer of WKTNode. */
using WKTNodePtr = std::shared_ptr<WKTNode>;
/** Non-null shared pointer of WKTNode. */
using WKTNodeNNPtr = util::nn<WKTNodePtr>;

/** \brief Node in the tree-splitted WKT representation.
 */
class WKTNode {
  public:
    PROJ_DLL explicit WKTNode(const std::string &valueIn);
    //! @cond Doxygen_Suppress
    PROJ_DLL ~WKTNode();
    //! @endcond

    PROJ_DLL const std::string &value() const;
    PROJ_DLL const std::vector<WKTNodeNNPtr> &children() const;

    PROJ_DLL void addChild(WKTNodeNNPtr child);
    PROJ_DLL WKTNodePtr lookForChild(const std::string &childName,
                                     int occurrence = 0) const;
    PROJ_DLL int countChildrenOfName(const std::string &childName) const;

    PROJ_DLL std::string toString() const;

    PROJ_DLL static WKTNodeNNPtr createFrom(const std::string &wkt,
                                            size_t indexStart = 0);

  protected:
    PROJ_DLL static WKTNodeNNPtr
    createFrom(const std::string &wkt, size_t indexStart, int recLevel,
               size_t &indexEnd); // throw(ParsingException)

  private:
    PROJ_OPAQUE_PRIVATE_DATA
};

// ---------------------------------------------------------------------------

/** \brief Parse a WKT string into the appropriate suclass of util::BaseObject.
 */
class WKTParser {
  public:
    PROJ_DLL WKTParser();
    //! @cond Doxygen_Suppress
    PROJ_DLL ~WKTParser();
    //! @endcond

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
