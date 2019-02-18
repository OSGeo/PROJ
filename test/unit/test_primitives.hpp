/******************************************************************************
 *
 * Project:  PROJ
 * Purpose:  Test ISO19111:2018 implementation
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

#include "gtest_include.h"

#ifndef FROM_PROJ_CPP
#define FROM_PROJ_CPP
#endif

#include "proj/internal/internal.hpp"

#include <cmath>
#include <cstdlib>

using namespace osgeo::proj::internal;

static ::testing::AssertionResult ComparePROJString(const char* m_expr,
                                                    const char* n_expr,
                                                    const std::string& m,
                                                    const std::string& n) {
    //if (m == n) return ::testing::AssertionSuccess();
    auto mTokens = split(m, ' ');
    auto nTokens = split(n, ' ');
    if( mTokens.size() == nTokens.size() )
    {
        bool success = true;
        for( size_t i = 0; i < mTokens.size(); i++ )
        {
            auto mSubTokens = split(mTokens[i], '=');
            auto nSubTokens = split(nTokens[i], '=');
            if( mSubTokens.size() != nSubTokens.size() ) {
                success = false;
                break;
            }
            if( mSubTokens.size() == 2 && nSubTokens.size() == 2 ) {
                if( mSubTokens[0] != nSubTokens[0] ) {
                    success = false;
                    break;
                }
                double mValue = 0.0;
                bool mIsDouble = false;
                try {
                    mValue = c_locale_stod(mSubTokens[1]);
                    mIsDouble = true;
                } catch( const std::exception &) {}
                double nValue = 0.0;
                bool nIsDouble = false;
                try {
                    nValue = c_locale_stod(nSubTokens[1]);
                    nIsDouble = true;
                } catch( const std::exception &) {}
                if( mIsDouble != nIsDouble ) {
                    success = false;
                    break;
                }
                if( mIsDouble ) {
                    success = std::abs(mValue - nValue) <= 1e-14 * std::abs(mValue);
                } else {
                    success = mSubTokens[1] == nSubTokens[1];
                }
                if( !success ) {
                    break;
                }
            }
        }

        if( success ) {
            return ::testing::AssertionSuccess();
        }
    }

    return ::testing::AssertionFailure() << m_expr << " and " << n_expr
        << " (" << m << " and " << n << ") are different";
}
