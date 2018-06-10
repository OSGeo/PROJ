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

#ifndef METADATA_HH_INCLUDED
#define METADATA_HH_INCLUDED

#include <memory>
#include <string>
#include <vector>

#include "io.hpp"
#include "util.hpp"

NS_PROJ_START

namespace common {
class IdentifiedObject;
}

/** osgeo.proj.metadata namespace
 *
 * \brief Common classes from metadata ISO standard
 */
namespace metadata {

// ---------------------------------------------------------------------------

class Citation : public util::BaseObject {
  public:
    PROJ_DLL Citation();
    PROJ_DLL explicit Citation(const std::string &titleIn);
    PROJ_DLL Citation(const Citation &other);
    PROJ_DLL ~Citation();

    PROJ_DLL const util::optional<std::string> &title() const;

  protected:
    friend class util::optional<Citation>;
    Citation &operator=(const Citation &other);

  private:
    PROJ_OPAQUE_PRIVATE_DATA
};

// ---------------------------------------------------------------------------

class GeographicExtent;
using GeographicExtentPtr = std::shared_ptr<GeographicExtent>;
using GeographicExtentNNPtr = util::nn<GeographicExtentPtr>;

// ISO_19115 EX_GeographicExtent */
class GeographicExtent {
  public:
    PROJ_DLL virtual ~GeographicExtent();

    // GeoAPI has a getInclusion() method. We assume that it is included for our
    // use

  protected:
    GeographicExtent();

  private:
    PROJ_OPAQUE_PRIVATE_DATA
};

// ---------------------------------------------------------------------------

class GeographicBoundingBox;
using GeographicBoundingBoxPtr = std::shared_ptr<GeographicBoundingBox>;
using GeographicBoundingBoxNNPtr = util::nn<GeographicBoundingBoxPtr>;

// ISO_19115 EX_GeographicBoundingBox */
class GeographicBoundingBox : public GeographicExtent {
  public:
    PROJ_DLL ~GeographicBoundingBox() override;
    PROJ_DLL double westBoundLongitude() const;
    PROJ_DLL double southBoundLongitude() const;
    PROJ_DLL double eastBoundLongitude() const;
    PROJ_DLL double northBoundLongitude() const;

    PROJ_DLL static GeographicBoundingBoxNNPtr
    create(double west, double south, double east, double north);

  protected:
    GeographicBoundingBox(double west, double south, double east, double north);
    INLINED_MAKE_SHARED

  private:
    PROJ_OPAQUE_PRIVATE_DATA
};

// ---------------------------------------------------------------------------

class Extent;
using ExtentPtr = std::shared_ptr<Extent>;
using ExtentNNPtr = util::nn<ExtentPtr>;

// ISO_19115 EX_Extent */
class Extent : public util::BaseObject {
  public:
    PROJ_DLL Extent(const Extent &other);
    PROJ_DLL Extent &operator=(const Extent &other) = delete;
    PROJ_DLL ~Extent();

    PROJ_DLL const util::optional<std::string> &description() const;
    PROJ_DLL const std::vector<GeographicExtentNNPtr> &
    geographicElements() const;
    // TODO: temporal and vertical part !

    PROJ_DLL static ExtentNNPtr
    create(const util::optional<std::string> &descriptionIn,
           const std::vector<GeographicExtentNNPtr> &geographicElementsIn);

  protected:
    Extent();
    INLINED_MAKE_SHARED

  private:
    PROJ_OPAQUE_PRIVATE_DATA
};

// ---------------------------------------------------------------------------

class Identifier;
using IdentifierPtr = std::shared_ptr<Identifier>;
using IdentifierNNPtr = util::nn<IdentifierPtr>;

class Identifier : public util::BaseObject, public io::IWKTExportable {
  public:
    PROJ_DLL Identifier(const Identifier &other);
    PROJ_DLL ~Identifier() override;

    PROJ_DLL static IdentifierNNPtr
    create(const std::string &codeIn = std::string(),
           const util::PropertyMap &properties =
               util::PropertyMap()); // throw(InvalidValueTypeException)

    PROJ_DLL static const std::string AUTHORITY_KEY;
    PROJ_DLL static const std::string CODE_KEY;
    PROJ_DLL static const std::string CODESPACE_KEY;
    PROJ_DLL static const std::string VERSION_KEY;
    PROJ_DLL static const std::string DESCRIPTION_KEY;
    PROJ_DLL static const std::string URI_KEY;

    PROJ_DLL const util::optional<Citation> &authority() const;
    PROJ_DLL const std::string &code() const;
    PROJ_DLL const util::optional<std::string> &codeSpace() const;
    PROJ_DLL const util::optional<std::string> &version() const;
    PROJ_DLL const util::optional<std::string> &description() const;
    PROJ_DLL const util::optional<std::string> &uri() const;

    std::string exportToWKT(io::WKTFormatterNNPtr formatter)
        const override; // throw(io::FormattingException)

  protected:
    explicit Identifier(const std::string &codeIn = std::string());

    friend class util::optional<Identifier>;
    INLINED_MAKE_SHARED
    Identifier &operator=(const Identifier &other);

    friend class common::IdentifiedObject;

    // Non-standard
    void setProperties(const util::PropertyMap
                           &properties); // throw(InvalidValueTypeException)

  private:
    PROJ_OPAQUE_PRIVATE_DATA
};

// ---------------------------------------------------------------------------

// Content does not match ISO_19115
class PositionalAccuracy {
  public:
    std::string value{};
};

} // namespace metadata

NS_PROJ_END

#endif //  METADATA_HH_INCLUDED
