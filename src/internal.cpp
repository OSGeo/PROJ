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
#define FROM_PROJ_CPP
#endif

#include "proj/internal/internal.hpp"

#include <cstring>
#ifdef _MSC_VER
#include <string.h>
#else
#include <strings.h>
#endif
#include <exception>
#include <iomanip> // std::setprecision
#include <locale>
#include <sstream> // std::istringstream and std::ostringstream
#include <string>

NS_PROJ_START

namespace internal {

// ---------------------------------------------------------------------------

/**
 * Replace all occurrences of before with after.
 */
std::string replaceAll(const std::string &str, const std::string &before,
                       const std::string &after) {
    std::string ret(str);
    const size_t nBeforeSize = before.size();
    const size_t nAfterSize = after.size();
    if (nBeforeSize) {
        size_t nStartPos = 0;
        while ((nStartPos = ret.find(before, nStartPos)) != std::string::npos) {
            ret.replace(nStartPos, nBeforeSize, after);
            nStartPos += nAfterSize;
        }
    }
    return ret;
}

// ---------------------------------------------------------------------------

inline static bool EQUALN(const char *a, const char *b, size_t size) {
#ifdef _MSC_VER
    return _strnicmp(a, b, size) == 0;
#else
    return strncasecmp(a, b, size) == 0;
#endif
}

/**
 * Case-insensitive equality test
 */
bool ci_equal(const std::string &a, const std::string &b) noexcept {
    const auto size = a.size();
    if (size != b.size()) {
        return false;
    }
    return EQUALN(a.c_str(), b.c_str(), size);
}

bool ci_equal(const std::string &a, const char *b) noexcept {
    const auto size = a.size();
    if (size != strlen(b)) {
        return false;
    }
    return EQUALN(a.c_str(), b, size);
}

bool ci_equal(const char *a, const char *b) noexcept {
    const auto size = strlen(a);
    if (size != strlen(b)) {
        return false;
    }
    return EQUALN(a, b, size);
}

// ---------------------------------------------------------------------------

bool ci_less(const std::string &a, const std::string &b) noexcept {
#ifdef _MSC_VER
    return _stricmp(a.c_str(), b.c_str()) < 0;
#else
    return strcasecmp(a.c_str(), b.c_str()) < 0;
#endif
}

// ---------------------------------------------------------------------------

/**
 * Convert to lower case.
 */

std::string tolower(const std::string &str)

{
    std::string ret(str);
    for (size_t i = 0; i < ret.size(); i++)
        ret[i] = static_cast<char>(::tolower(ret[i]));
    return ret;
}

// ---------------------------------------------------------------------------

/**
 * Convert to upper case.
 */

std::string toupper(const std::string &str)

{
    std::string ret(str);
    for (size_t i = 0; i < ret.size(); i++)
        ret[i] = static_cast<char>(::toupper(ret[i]));
    return ret;
}

// ---------------------------------------------------------------------------

/** Strip leading and trailing double quote characters */
std::string stripQuotes(const std::string &str) {
    if (str.size() >= 2 && str[0] == '"' && str.back() == '"') {
        return str.substr(1, str.size() - 2);
    }
    return str;
}

// ---------------------------------------------------------------------------

size_t ci_find(const std::string &str, const char *needle) noexcept {
    const size_t needleSize = strlen(needle);
    for (size_t i = 0; i + needleSize <= str.size(); i++) {
        if (EQUALN(str.c_str() + i, needle, needleSize)) {
            return i;
        }
    }
    return std::string::npos;
}

// ---------------------------------------------------------------------------

size_t ci_find(const std::string &str, const std::string &needle,
               size_t startPos) noexcept {
    const size_t needleSize = needle.size();
    for (size_t i = startPos; i + needleSize <= str.size(); i++) {
        if (EQUALN(str.c_str() + i, needle.c_str(), needleSize)) {
            return i;
        }
    }
    return std::string::npos;
}

// ---------------------------------------------------------------------------

/*
bool starts_with(const std::string &str, const std::string &prefix) noexcept {
    if (str.size() < prefix.size()) {
        return false;
    }
    return std::memcmp(str.c_str(), prefix.c_str(), prefix.size()) == 0;
}
*/

// ---------------------------------------------------------------------------

/*
bool starts_with(const std::string &str, const char *prefix) noexcept {
    const size_t prefixSize = std::strlen(prefix);
    if (str.size() < prefixSize) {
        return false;
    }
    return std::memcmp(str.c_str(), prefix, prefixSize) == 0;
}
*/

// ---------------------------------------------------------------------------

bool ci_starts_with(const char *str, const char *prefix) noexcept {
    const auto str_size = strlen(str);
    const auto prefix_size = strlen(prefix);
    if (str_size < prefix_size) {
        return false;
    }
    return EQUALN(str, prefix, prefix_size);
}

// ---------------------------------------------------------------------------

bool ci_starts_with(const std::string &str,
                    const std::string &prefix) noexcept {
    if (str.size() < prefix.size()) {
        return false;
    }
    return EQUALN(str.c_str(), prefix.c_str(), prefix.size());
}

// ---------------------------------------------------------------------------

bool ends_with(const std::string &str, const std::string &suffix) noexcept {
    if (str.size() < suffix.size()) {
        return false;
    }
    return std::memcmp(str.c_str() + str.size() - suffix.size(), suffix.c_str(),
                       suffix.size()) == 0;
}

// ---------------------------------------------------------------------------

double c_locale_stod(const std::string &s) {
    std::istringstream iss(s);
    iss.imbue(std::locale::classic());
    double d;
    iss >> d;
    if (!iss.eof() || iss.fail()) {
        throw std::invalid_argument("non double value");
    }
    return d;
}

// ---------------------------------------------------------------------------

std::vector<std::string> split(const std::string &str, char separator) {
    std::vector<std::string> res;
    size_t lastPos = 0;
    size_t newPos = 0;
    while ((newPos = str.find(separator, lastPos)) != std::string::npos) {
        res.push_back(str.substr(lastPos, newPos - lastPos));
        lastPos = newPos + 1;
    }
    res.push_back(str.substr(lastPos));
    return res;
}

// ---------------------------------------------------------------------------

std::string toString(int val) {
    std::ostringstream buffer;
    buffer.imbue(std::locale::classic());
    buffer << val;
    return buffer.str();
}

std::string toString(double val, int precision) {
    std::ostringstream buffer;
    buffer.imbue(std::locale::classic());
    buffer << std::setprecision(precision);
    buffer << val;
    auto str = buffer.str();
    if (precision == 15 && str.find("9999999999") != std::string::npos) {
        buffer.str("");
        buffer.clear();
        buffer << std::setprecision(14);
        buffer << val;
        return buffer.str();
    }
    return str;
}

// ---------------------------------------------------------------------------

std::string concat(const char *a, const std::string &b) {
    std::string res(a);
    res += b;
    return res;
}

std::string concat(const char *a, const std::string &b, const char *c) {
    std::string res(a);
    res += b;
    res += c;
    return res;
}

// ---------------------------------------------------------------------------

} // namespace internal

NS_PROJ_END
