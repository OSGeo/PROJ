
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

#ifndef EXCEPTION_HH_INCLUDED
#define EXCEPTION_HH_INCLUDED

#include <exception>
#include <string>

#include "util.hh"

NS_PROJ_START

namespace util {

// ---------------------------------------------------------------------------

class Exception : public std::exception {
    std::string msg_;

  public:
    explicit Exception(const char *message) : msg_(message) {}
    explicit Exception(const std::string &message) : msg_(message) {}
    PROJ_DLL virtual const char *what() const noexcept { return msg_.c_str(); }
};

// ---------------------------------------------------------------------------

class InvalidValueTypeException : public Exception {
  public:
    explicit InvalidValueTypeException(const char *message)
        : Exception(message) {}
    explicit InvalidValueTypeException(const std::string &message)
        : Exception(message) {}
};

} // namespace util

NS_PROJ_END

#endif //  EXCEPTION_HH_INCLUDED
