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

#include "proj/internal.hpp"

#include <exception>
#include <locale>
#include <sstream> // std::istringstream
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

size_t ci_find(const std::string &str, const std::string &needle,
               size_t startPos) {
    if (startPos >= str.size()) {
        return std::string::npos;
    }
    auto lowerStr = tolower(str.substr(startPos));
    auto lowerNeedle = tolower(needle);
    return lowerStr.find(lowerNeedle);
}

// ---------------------------------------------------------------------------

bool starts_with(const std::string &str, const std::string &prefix) {
    if (str.size() < prefix.size()) {
        return false;
    }
    return str.substr(0, prefix.size()) == prefix;
}

// ---------------------------------------------------------------------------

bool ends_with(const std::string &str, const std::string &suffix) {
    if (str.size() < suffix.size()) {
        return false;
    }
    return str.substr(str.size() - suffix.size()) == suffix;
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

} // namespace internal

NS_PROJ_END
