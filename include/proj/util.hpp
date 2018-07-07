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
 * \brief A set of base types from ISO 19103, \ref GeoAPI and other PROJ
 * specific classes.
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

/** \brief Loose transposition of [std::optional]
 * (https://en.cppreference.com/w/cpp/utility/optional) available from C++17. */
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

    PROJ_DLL T *operator->() { return &val_; }
    PROJ_DLL T &operator*() { return val_; }

    //! @endcond

    /** Returns a pointer to the contained value. */
    PROJ_DLL const T *operator->() const { return &val_; }

    /** Returns a reference to the contained value. */
    PROJ_DLL const T &operator*() const { return val_; }

    /** Return whether the optional has a value */
    PROJ_DLL explicit operator bool() const noexcept { return hasVal_; }

    /** Return whether the optional has a value */
    PROJ_DLL bool has_value() const noexcept { return hasVal_; }

  private:
    bool hasVal_;
    T val_{};
};

// ---------------------------------------------------------------------------

class BaseObject;
/** Shared pointer of BaseObject. */
using BaseObjectPtr = std::shared_ptr<BaseObject>;
/** Non-null shared pointer of BaseObject. */
using BaseObjectNNPtr = util::nn<BaseObjectPtr>;

/** \brief Class that can be derived from, to emulate Java's Object behaviour.
 */
class BaseObject {
  public:
    //! @cond Doxygen_Suppress
    virtual PROJ_DLL ~BaseObject();
    //! @endcond

  protected:
    PROJ_DLL BaseObject();
    PROJ_DLL void assignSelf(BaseObjectNNPtr self);
    PROJ_DLL BaseObjectNNPtr shared_from_this() const;

  private:
    PROJ_OPAQUE_PRIVATE_DATA
};

// ---------------------------------------------------------------------------

/** \brief Interface for an object that can be compared to another.
 */
class IComparable {
  public:
    //! @cond Doxygen_Suppress
    virtual PROJ_DLL ~IComparable();
    //! @endcond

    /** \brief Comparison criterion. */
    enum class PROJ_DLL Criterion {
        /** All attributes are identical. */
        STRICT,
        /** The objects are equivalent for coordinate operations. */
        EQUIVALENT
    };

    /** \brief Returns whether an object is equivalent to another one.
     * @param other other object to compare to
     * @param criterion comparaison criterion.
     * @return true if objects are equivalent.
     */
    PROJ_DLL virtual bool
    isEquivalentTo(const BaseObjectNNPtr &other,
                   Criterion criterion = Criterion::STRICT) const = 0;
};

// ---------------------------------------------------------------------------

/** \brief Encapsulate standard datatypes in an object.
 */
class BoxedValue : public BaseObject {
  public:
    //! @cond Doxygen_Suppress
    /** Type of data stored in the BoxedValue. */
    enum class Type {
        /** a BaseObjectNNPtr */
        OTHER_OBJECT,
        /** a std::string */
        STRING,
        /** an integer */
        INTEGER,
        /** a boolean */
        BOOLEAN
    };
    //! @endcond

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

    //! @cond Doxygen_Suppress
    PROJ_DLL BoxedValue(const BoxedValue &other);
    PROJ_DLL BoxedValue &operator=(const BoxedValue &other);
    PROJ_DLL ~BoxedValue() override;

    PROJ_DLL const Type &type() const;
    PROJ_DLL BaseObjectNNPtr object() const;
    PROJ_DLL const std::string &stringValue() const;
    PROJ_DLL int integerValue() const;
    PROJ_DLL bool booleanValue() const;
    //! @endcond

  private:
    PROJ_OPAQUE_PRIVATE_DATA

    BoxedValue();
};

/** Shared pointer of BoxedValue. */
using BoxedValuePtr = std::shared_ptr<BoxedValue>;
/** Non-null shared pointer of BoxedValue. */
using BoxedValueNNPtr = util::nn<BoxedValuePtr>;

// ---------------------------------------------------------------------------

class ArrayOfBaseObject;
/** Shared pointer of ArrayOfBaseObject. */
using ArrayOfBaseObjectPtr = std::shared_ptr<ArrayOfBaseObject>;
/** Non-null shared pointer of ArrayOfBaseObject. */
using ArrayOfBaseObjectNNPtr = util::nn<ArrayOfBaseObjectPtr>;

/** \brief Array of BaseObject.
 */
class ArrayOfBaseObject : public BaseObject {
  public:
    //! @cond Doxygen_Suppress
    PROJ_DLL ~ArrayOfBaseObject() override;
    //! @endcond

    PROJ_DLL void add(BaseObjectNNPtr obj);

    //! @cond Doxygen_Suppress
    std::vector<BaseObjectNNPtr>::const_iterator begin() const;
    std::vector<BaseObjectNNPtr>::const_iterator end() const;
    bool empty() const;
    //! @endcond

    PROJ_DLL static ArrayOfBaseObjectNNPtr create();

  protected:
    ArrayOfBaseObject();
    INLINED_MAKE_SHARED

  private:
    ArrayOfBaseObject(const ArrayOfBaseObject &other) = delete;
    ArrayOfBaseObject &operator=(const ArrayOfBaseObject &other) = delete;
    PROJ_OPAQUE_PRIVATE_DATA
};

// ---------------------------------------------------------------------------

/** \brief Wrapper of a std::map<std::string, BaseObjectNNPtr> */
class PropertyMap {
  public:
    PROJ_DLL PropertyMap();
    //! @cond Doxygen_Suppress
    PROJ_DLL PropertyMap(const PropertyMap &other);
    PROJ_DLL ~PropertyMap();
    //! @endcond

    PROJ_DLL PropertyMap &set(const std::string &key,
                              const BaseObjectNNPtr &val);

    PROJ_DLL PropertyMap &set(const std::string &key, const BoxedValue &val);

    PROJ_DLL PropertyMap &set(const std::string &key,
                              const std::vector<std::string> &array);

    //! @cond Doxygen_Suppress
    std::map<std::string, BaseObjectNNPtr>::iterator
    find(const std::string &key) const;
    std::map<std::string, BaseObjectNNPtr>::iterator end() const;
    //! @endcond

    //! @cond Doxygen_Suppress
    // throw(InvalidValueTypeException)
    bool getStringValue(const std::string &key, std::string &outVal) const;
    //! @endcond

  private:
    PropertyMap &operator=(const PropertyMap &) = delete;

    PROJ_OPAQUE_PRIVATE_DATA
};

// ---------------------------------------------------------------------------

class LocalName;
/** Shared pointer of LocalName. */
using LocalNamePtr = std::shared_ptr<LocalName>;
/** Non-null shared pointer of LocalName. */
using LocalNameNNPtr = util::nn<LocalNamePtr>;

class NameSpace;
/** Shared pointer of NameSpace. */
using NameSpacePtr = std::shared_ptr<NameSpace>;
/** Non-null shared pointer of NameSpace. */
using NameSpaceNNPtr = util::nn<NameSpacePtr>;

class GenericName;
/** Shared pointer of GenericName. */
using GenericNamePtr = std::shared_ptr<GenericName>;
/** Non-null shared pointer of GenericName. */
using GenericNameNNPtr = util::nn<GenericNamePtr>;

// ---------------------------------------------------------------------------

/** \brief A sequence of identifiers rooted within the context of a namespace.
 *
 * \remark Simplified version of [GenericName]
 * (http://www.geoapi.org/3.0/javadoc/org/opengis/util/GenericName.html) from
 * \ref GeoAPI
 */
class GenericName : public BaseObject {
  public:
    //! @cond Doxygen_Suppress
    PROJ_DLL virtual ~GenericName() override;
    //! @endcond

    /** \brief Return the scope of the object, possibly a global one. */
    PROJ_DLL virtual const NameSpacePtr scope() const = 0;

    /** \brief Return the LocalName as a string. */
    PROJ_DLL virtual std::string toString() const = 0;

    /** \brief Return a fully qualified name corresponding to the local name.
     *
     * The namespace of the resulting name is a global one.
     */
    PROJ_DLL virtual GenericNameNNPtr toFullyQualifiedName() const = 0;

  protected:
    GenericName();
    GenericName(const GenericName &other);

  private:
    PROJ_OPAQUE_PRIVATE_DATA
    GenericName &operator=(const GenericName &other) = delete;
};

// ---------------------------------------------------------------------------

/** \brief A domain in which names given by strings are defined.
 *
 * \remark Simplified version of [NameSpace]
 * (http://www.geoapi.org/3.0/javadoc/org/opengis/util/NameSpace.html) from \ref
 * GeoAPI
 */
class NameSpace {
  public:
    //! @cond Doxygen_Suppress
    PROJ_DLL ~NameSpace();
    //! @endcond

    PROJ_DLL bool isGlobal() const;
    PROJ_DLL const GenericNamePtr &name() const;

  protected:
    friend class NameFactory;
    friend class LocalName;
    explicit NameSpace(const GenericNamePtr &name);
    NameSpace(const NameSpace &other);
    NameSpaceNNPtr getGlobalFromThis() const;
    const std::string &separator() const;
    static const NameSpaceNNPtr GLOBAL;
    INLINED_MAKE_SHARED

  private:
    PROJ_OPAQUE_PRIVATE_DATA
    NameSpace &operator=(const NameSpace &other) = delete;

    static NameSpaceNNPtr createGLOBAL();
};

// ---------------------------------------------------------------------------

/** \brief Identifier within a NameSpace for a local object.
 *
 * Local names are names which are directly accessible to and maintained by a
 * NameSpace within which they are local, indicated by the scope.
 *
 * \remark Simplified version of [LocalName]
 * (http://www.geoapi.org/3.0/javadoc/org/opengis/util/LocalName.html) from \ref
 * GeoAPI
 */
class LocalName : public GenericName {
  public:
    //! @cond Doxygen_Suppress
    PROJ_DLL ~LocalName() override;
    //! @endcond

    PROJ_DLL const NameSpacePtr scope() const override;
    PROJ_DLL std::string toString() const override;
    PROJ_DLL GenericNameNNPtr toFullyQualifiedName() const override;

  protected:
    friend class NameFactory;
    friend class NameSpace;
    explicit LocalName(const std::string &nameIn);
    LocalName(const LocalName &other);
    LocalName(const NameSpacePtr &ns, const std::string &name);
    INLINED_MAKE_SHARED

  private:
    PROJ_OPAQUE_PRIVATE_DATA
    LocalName &operator=(const LocalName &other) = delete;
};

// ---------------------------------------------------------------------------

/** \brief Factory for generic names.
 *
 * \remark Simplified version of [NameFactory]
 * (http://www.geoapi.org/3.0/javadoc/org/opengis/util/NameFactory.html) from
 * \ref GeoAPI
 */
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

/** \brief Abstract class to define an enumeration of values.
 */
class CodeList {
  public:
    //! @cond Doxygen_Suppress
    PROJ_DLL ~CodeList();
    //! @endcond

    PROJ_DLL const std::string &toString() const;
    /** Return the CodeList item as a string. */
    inline operator std::string() const { return toString(); }

    //! @cond Doxygen_Suppress
    PROJ_DLL bool operator==(const CodeList &other) const;
    PROJ_DLL bool operator!=(const CodeList &other) const;
    //! @endcond
  protected:
    explicit CodeList(const std::string &nameIn);
    CodeList(const CodeList &other);
    CodeList &operator=(const CodeList &other);

  private:
    PROJ_OPAQUE_PRIVATE_DATA
};

// ---------------------------------------------------------------------------

/** \brief Root exception class.
 */
class Exception : public std::exception {
    std::string msg_;

  public:
    //! @cond Doxygen_Suppress
    PROJ_DLL explicit Exception(const char *message);
    PROJ_DLL explicit Exception(const std::string &message);
    PROJ_DLL Exception(const Exception &other);
    PROJ_DLL ~Exception() override;
    //! @endcond
    PROJ_DLL virtual const char *what() const noexcept override;
};

// ---------------------------------------------------------------------------

/** \brief Exception thrown when an invalid value type is set as the value of
 * a key of a PropertyMap.
 */
class InvalidValueTypeException : public Exception {
  public:
    //! @cond Doxygen_Suppress
    PROJ_DLL explicit InvalidValueTypeException(const char *message);
    PROJ_DLL explicit InvalidValueTypeException(const std::string &message);
    PROJ_DLL InvalidValueTypeException(const InvalidValueTypeException &other);
    PROJ_DLL ~InvalidValueTypeException() override;
    //! @endcond
};

// ---------------------------------------------------------------------------

/** \brief Exception Thrown to indicate that the requested operation is not
 * supported.
 */
class UnsupportedOperationException : public Exception {
  public:
    //! @cond Doxygen_Suppress
    PROJ_DLL explicit UnsupportedOperationException(const char *message);
    PROJ_DLL explicit UnsupportedOperationException(const std::string &message);
    PROJ_DLL
    UnsupportedOperationException(const UnsupportedOperationException &other);
    PROJ_DLL ~UnsupportedOperationException() override;
    //! @endcond
};

} // namespace util

NS_PROJ_END

#endif // UTIL_HH_INCLUDED
