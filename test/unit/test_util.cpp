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

#include "proj/util.hpp"

#include <iostream>

using namespace osgeo::proj::util;

// ---------------------------------------------------------------------------

TEST(util, NameFactory) {
    LocalNameNNPtr localname(NameFactory::createLocalName(nullptr, "foo"));
    auto ns = localname->scope();
    EXPECT_EQ(ns->isGlobal(), true);
    EXPECT_EQ(ns->name()->toFullyQualifiedName()->toString(), "global");
    EXPECT_EQ(localname->toFullyQualifiedName()->toString(), "foo");
}

// ---------------------------------------------------------------------------

TEST(util, NameFactory2) {
    PropertyMap map;
    map.set("separator", "/");
    NameSpaceNNPtr nsIn(NameFactory::createNameSpace(
        NameFactory::createLocalName(nullptr, std::string("bar")), map));
    LocalNameNNPtr localname(
        NameFactory::createLocalName(nsIn, std::string("foo")));
    auto ns = localname->scope();
    EXPECT_EQ(ns->isGlobal(), false);
    auto fullyqualifiedNS = ns->name()->toFullyQualifiedName();
    EXPECT_EQ(fullyqualifiedNS->toString(), "bar");
    EXPECT_EQ(fullyqualifiedNS->scope()->isGlobal(), true);
    EXPECT_EQ(fullyqualifiedNS->scope()->name()->scope()->isGlobal(), true);
    EXPECT_EQ(localname->toFullyQualifiedName()->toString(), "bar/foo");
}
