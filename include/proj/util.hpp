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

#ifndef UTIL_HH_INCLUDED
#define UTIL_HH_INCLUDED

#if !(__cplusplus >= 201103L || (defined(_MSC_VER) && _MSC_VER >= 1900))
#error Must have C++11 or newer.
#endif

#include <exception>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "nn.hpp"

#ifndef NS_PROJ
/** osgeo namespace */
namespace osgeo {
/** osgeo.proj namespace */
namespace proj {}
}
#endif

//! @cond Doxygen_Suppress

#ifndef PROJ_DLL
#ifdef PROJ_MSVC_DLL_EXPORT
#define PROJ_DLL __declspec(dllexport)
#elif defined(PROJ_MSVC_DLL_IMPORT)
#define PROJ_DLL __declspec(dllimport)
#else
#define PROJ_DLL
#endif
#endif

/* To allow customizing the base namespace of PROJ */
#ifndef NS_PROJ
#define NS_PROJ osgeo::proj
#define NS_PROJ_START                                                          \
    namespace osgeo {                                                          \
    namespace proj {
#define NS_PROJ_END                                                            \
    }                                                                          \
    }
#endif

// Private-implementation (Pimpl) pattern
#define PROJ_OPAQUE_PRIVATE_DATA                                               \
  private:                                                                     \
    struct Private;                                                            \
    std::unique_ptr<Private> d;                                                \
                                                                               \
  protected:                                                                   \
    Private *getPrivate() { return d.get(); }                                  \
    const Private *getPrivate() const { return d.get(); }                      \
//! @endcond

// To include in the protected/private section of a class definition,
// to be able to call make_shared on a protected/private constructor
#define INLINED_MAKE_SHARED                                                    \
    template <typename T, typename... Args>                                    \
    static std::shared_ptr<T> make_shared(Args &&... args) {                   \
        return std::shared_ptr<T>(new T(std::forward<Args>(args)...));         \
    }                                                                          \
    template <typename T, typename... Args>                                    \
    static util::nn_shared_ptr<T> nn_make_shared(Args &&... args) {            \
        return util::nn_shared_ptr<T>(                                         \
            util::i_promise_i_checked_for_null,                                \
            std::shared_ptr<T>(new T(std::forward<Args>(args)...)));           \
    }

//! @endcond

NS_PROJ_START

/** osgeo.proj.util namespace.
 *
 * \brief A set of base types from ISO 19103 + other PROJ specific classes.
 */
namespace util {

//! @cond Doxygen_Suppress
// Import a few classes from nn.hpp to expose them under our ::util namespace
// for conveniency.
using ::dropbox::oxygen::nn;
using ::dropbox::oxygen::nn_make_shared;
using ::dropbox::oxygen::nn_static_pointer_cast;
using ::dropbox::oxygen::nn_dynamic_pointer_cast;
using ::dropbox::oxygen::i_promise_i_checked_for_null;
template <typename T> using nn_shared_ptr = nn<std::shared_ptr<T>>;
//! @endcond

// To avoid formatting differences between clang-format 3.8 and 7
#define PROJ_NOEXCEPT noexcept

/** \brief Loose transposition of std::optional available from C++17. */
template <class T> class optional {
  public:
    //! @cond Doxygen_Suppress
    optional() : hasVal_(false) {}
    explicit optional(const T &val) : hasVal_(true), val_(val) {}
    explicit optional(T &&val) : hasVal_(true), val_(std::forward<T>(val)) {}

    optional(const optional &other) = default;
    optional(optional &&other) PROJ_NOEXCEPT
        : hasVal_(other.hasVal_),
          // cppcheck-suppress functionStatic
          val_(std::forward<T>(other.val_)) {
        other.hasVal_ = false;
    }

    optional &operator=(const T &val) {
        hasVal_ = true;
        val_ = val;
        return *this;
    }
    optional &operator=(T &&val) noexcept {
        hasVal_ = true;
        val_ = std::forward<T>(val);
        return *this;
    }
    optional &operator=(const optional &other) = default;
    optional &operator=(optional &&other) noexcept {
        hasVal_ = other.hasVal_;
        val_ = std::forward<T>(other.val_);
        other.hasVal_ = false;
        return *this;
    }
    //! @endcond

    PROJ_DLL const T *operator->() const { return &val_; }
    PROJ_DLL T *operator->() { return &val_; }

    PROJ_DLL const T &operator*() const { return val_; }
    PROJ_DLL T &operator*() { return val_; }

    PROJ_DLL explicit operator bool() const noexcept { return hasVal_; }
    PROJ_DLL bool has_value() const noexcept { return hasVal_; }

  private:
    bool hasVal_;
    T val_{};
};

// ---------------------------------------------------------------------------

/** \brief Class that can be derived from, to emulate Java's Object behaviour.
 */
class BaseObject {
  public:
    virtual PROJ_DLL ~BaseObject();
};

using BaseObjectPtr = std::shared_ptr<BaseObject>;
using BaseObjectNNPtr = util::nn<BaseObjectPtr>;

// ---------------------------------------------------------------------------

/** \brief Encapsulate standard datatypes in an object.
 */
class BoxedValue : public BaseObject {
  public:
    enum class Type { OTHER_OBJECT, STRING, INTEGER, BOOLEAN };

    // cppcheck-suppress noExplicitConstructor
    PROJ_DLL BoxedValue(const BaseObjectNNPtr &objectIn);

    // cppcheck-suppress noExplicitConstructor
    PROJ_DLL BoxedValue(
        const char *stringValueIn); // needed to avoid the bool constructor
                                    // to be taken !
    // cppcheck-suppress noExplicitConstructor
    PROJ_DLL BoxedValue(const std::string &stringValueIn);
    // cppcheck-suppress noExplicitConstructor
    PROJ_DLL BoxedValue(int integerValueIn);
    // cppcheck-suppress noExplicitConstructor
    PROJ_DLL BoxedValue(bool booleanValueIn);

    PROJ_DLL BoxedValue(const BoxedValue &other);
    PROJ_DLL BoxedValue &operator=(const BoxedValue &other);
    PROJ_DLL ~BoxedValue();

    PROJ_DLL const Type &type() const;
    PROJ_DLL BaseObjectNNPtr object() const;
    PROJ_DLL const std::string &stringValue() const;
    PROJ_DLL int integerValue() const;
    PROJ_DLL bool booleanValue() const;

  private:
    PROJ_OPAQUE_PRIVATE_DATA

    BoxedValue();
};

using BoxedValuePtr = std::shared_ptr<BoxedValue>;
using BoxedValueNNPtr = util::nn<BoxedValuePtr>;

// ---------------------------------------------------------------------------

class ArrayOfBaseObject;
using ArrayOfBaseObjectPtr = std::shared_ptr<ArrayOfBaseObject>;
using ArrayOfBaseObjectNNPtr = util::nn<ArrayOfBaseObjectPtr>;

/** \brief Array of BaseObject.
 */
class ArrayOfBaseObject : public BaseObject {
  public:
    // FIXME
    std::vector<BaseObjectNNPtr> values{};

    PROJ_DLL ArrayOfBaseObject() = default;
    PROJ_DLL ArrayOfBaseObject(const ArrayOfBaseObject &other);
    PROJ_DLL ArrayOfBaseObject &operator=(const ArrayOfBaseObject &other);
    PROJ_DLL ~ArrayOfBaseObject() override;

    PROJ_DLL static ArrayOfBaseObjectNNPtr create();
};

// ---------------------------------------------------------------------------

class PropertyMap {
  public:
    PROJ_DLL PropertyMap();
    PROJ_DLL PropertyMap(const PropertyMap &other);
    PROJ_DLL ~PropertyMap();

    PROJ_DLL PropertyMap &set(const std::string &key,
                              const BaseObjectNNPtr &val);

    PROJ_DLL PropertyMap &set(const std::string &key, const BoxedValue &val);

    PROJ_DLL PropertyMap &set(const std::string &key,
                              const std::vector<std::string> &array);

    PROJ_DLL std::map<std::string, BaseObjectNNPtr>::iterator
    find(const std::string &key) const;
    PROJ_DLL std::map<std::string, BaseObjectNNPtr>::iterator end() const;

    //! @cond Doxygen_Suppress
    bool getStringValue(
        const std::string &key,
        std::string &outVal) const; // throw(InvalidValueTypeException)
                                    //! @endcond

  private:
    PropertyMap &operator=(const PropertyMap &) = delete;

    PROJ_OPAQUE_PRIVATE_DATA
};

// ---------------------------------------------------------------------------

class LocalName;
using LocalNamePtr = std::shared_ptr<LocalName>;
using LocalNameNNPtr = util::nn<LocalNamePtr>;

class NameSpace;
using NameSpacePtr = std::shared_ptr<NameSpace>;
using NameSpaceNNPtr = util::nn<NameSpacePtr>;

class GenericName;
using GenericNamePtr = std::shared_ptr<GenericName>;
using GenericNameNNPtr = util::nn<GenericNamePtr>;

// ---------------------------------------------------------------------------

class GenericName : public BaseObject {
  public:
    PROJ_DLL GenericName(const GenericName &other);
    PROJ_DLL GenericName &operator=(const GenericName &other) = delete;
    PROJ_DLL virtual ~GenericName();

    PROJ_DLL virtual const NameSpacePtr scope() const = 0;
    PROJ_DLL virtual std::string toString() const = 0;
    PROJ_DLL virtual GenericNameNNPtr toFullyQualifiedName() const = 0;

  protected:
    GenericName();

  private:
    PROJ_OPAQUE_PRIVATE_DATA
};

// ---------------------------------------------------------------------------

class NameSpace {
  public:
    PROJ_DLL NameSpace(const NameSpace &other);
    PROJ_DLL NameSpace &operator=(const NameSpace &other) = delete;
    PROJ_DLL ~NameSpace();

    PROJ_DLL bool isGlobal() const;
    PROJ_DLL const GenericNamePtr &name() const;

  protected:
    friend class NameFactory;
    friend class LocalName;
    explicit NameSpace(const GenericNamePtr &name);
    NameSpaceNNPtr getGlobalFromThis() const;
    const std::string &separator() const;
    static const NameSpaceNNPtr GLOBAL;
    INLINED_MAKE_SHARED

  private:
    PROJ_OPAQUE_PRIVATE_DATA

    static NameSpaceNNPtr createGLOBAL();
};

// ---------------------------------------------------------------------------

class LocalName : public GenericName {
  public:
    PROJ_DLL LocalName(const LocalName &other);
    PROJ_DLL LocalName &operator=(const LocalName &other) = delete;
    ~LocalName();

    PROJ_DLL const NameSpacePtr scope() const override;
    PROJ_DLL std::string toString() const override;
    PROJ_DLL GenericNameNNPtr toFullyQualifiedName() const override;

  protected:
    friend class NameFactory;
    friend class NameSpace;
    explicit LocalName(const std::string &nameIn);
    LocalName(const NameSpacePtr &ns, const std::string &name);
    INLINED_MAKE_SHARED

  private:
    PROJ_OPAQUE_PRIVATE_DATA
};

// ---------------------------------------------------------------------------

class NameFactory {
  public:
    PROJ_DLL static NameSpaceNNPtr
    createNameSpace(const GenericNameNNPtr &name,
                    const PropertyMap &properties);
    PROJ_DLL static LocalNameNNPtr createLocalName(const NameSpacePtr &scope,
                                                   const std::string &name);
    PROJ_DLL static GenericNameNNPtr
    createGenericName(const NameSpacePtr &scope,
                      const std::vector<std::string> &parsedNames);
};

// ---------------------------------------------------------------------------

class CodeList {
  public:
    PROJ_DLL ~CodeList();

    PROJ_DLL const std::string &toString() const;
    inline operator std::string() const { return toString(); }

    PROJ_DLL bool operator==(const CodeList &other) const;
    PROJ_DLL bool operator!=(const CodeList &other) const;

  protected:
    explicit CodeList(const std::string &nameIn);

  private:
    PROJ_OPAQUE_PRIVATE_DATA
};

// ---------------------------------------------------------------------------

class Exception : public std::exception {
    std::string msg_;

  public:
    PROJ_DLL explicit Exception(const char *message);
    PROJ_DLL explicit Exception(const std::string &message);
    PROJ_DLL ~Exception() override;
    PROJ_DLL virtual const char *what() const noexcept override;
};

// ---------------------------------------------------------------------------

class InvalidValueTypeException : public Exception {
  public:
    PROJ_DLL explicit InvalidValueTypeException(const char *message);
    PROJ_DLL explicit InvalidValueTypeException(const std::string &message);
    PROJ_DLL ~InvalidValueTypeException() override;
};

} // namespace util

NS_PROJ_END

#endif // UTIL_HH_INCLUDED
