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

#include <string>

NS_PROJ_START

namespace internal {

// ---------------------------------------------------------------------------

/**
 * Replace all occurrences of osBefore with osAfter.
 */
std::string replaceAll(const std::string &osStr, const std::string &osBefore,
                       const std::string &osAfter) {
    std::string ret(osStr);
    const size_t nBeforeSize = osBefore.size();
    const size_t nAfterSize = osAfter.size();
    if (nBeforeSize) {
        size_t nStartPos = 0;
        while ((nStartPos = ret.find(osBefore, nStartPos)) !=
               std::string::npos) {
            ret.replace(nStartPos, nBeforeSize, osAfter);
            nStartPos += nAfterSize;
        }
    }
    return ret;
}

// ---------------------------------------------------------------------------

/**
 * Convert to lower case.
 */

std::string tolower(const std::string &osStr)

{
    std::string ret(osStr);
    for (size_t i = 0; i < ret.size(); i++)
        ret[i] = static_cast<char>(::tolower(ret[i]));
    return ret;
}

// ---------------------------------------------------------------------------

/**
 * Convert to upper case.
 */

std::string toupper(const std::string &osStr)

{
    std::string ret(osStr);
    for (size_t i = 0; i < ret.size(); i++)
        ret[i] = static_cast<char>(::toupper(ret[i]));
    return ret;
}

// ---------------------------------------------------------------------------

/** Strip leading and trailing double quote characters */
std::string stripQuotes(const std::string &osStr) {
    // As used in examples of OGC 12-063r5
    static const std::string printedQuote("\xE2\x80\x9C");

    if (osStr.size() >= 2 && osStr[0] == '"' && osStr.back() == '"') {
        return osStr.substr(1, osStr.size() - 2);
    } else if (osStr.size() >= 6 && osStr.substr(0, 3) == printedQuote &&
               osStr.substr(osStr.size() - 3) == printedQuote) {
        return osStr.substr(3, osStr.size() - 6);
    }
    return osStr;
}

// ---------------------------------------------------------------------------

size_t ci_find(const std::string &osStr, const std::string &osNeedle,
               size_t nStartPos) {
    if (nStartPos >= osStr.size()) {
        return std::string::npos;
    }
    auto lowerStr = tolower(osStr.substr(nStartPos));
    auto lowerNeedle = tolower(osNeedle);
    return lowerStr.find(lowerNeedle);
}

// ---------------------------------------------------------------------------

} // namespace internal

NS_PROJ_END
