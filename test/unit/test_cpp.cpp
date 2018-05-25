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


/* FIXME: this is temporary manual testing for development purposes */
/* ultimately must be removed */

#include "proj/coordinateoperation.hh"
#include "proj/coordinatesystem.hh"
#include "proj/crs.hh"
#include "proj/datum.hh"
#include "proj/metadata.hh"

#include <iostream>

using namespace osgeo::proj::common;
using namespace osgeo::proj::crs;
using namespace osgeo::proj::datum;
using namespace osgeo::proj::metadata;
using namespace osgeo::proj::util;
using namespace osgeo::proj::cs;
using namespace osgeo::proj::io;
using namespace osgeo::proj::operation;

int main() {

    {
        LocalNameNNPtr localname(NameFactory::createLocalName(nullptr, "foo"));
        auto ns = localname->scope();
        std::cout << ns->isGlobal() << std::endl;
        std::cout << ns->name()->toFullyQualifiedName()->toString()
                  << std::endl;
        std::cout << localname->toFullyQualifiedName()->toString() << std::endl;
    }

    {
        PropertyMap map;
        map.set("separator", "/");
        NameSpaceNNPtr nsIn(NameFactory::createNameSpace(
            NameFactory::createLocalName(nullptr, std::string("bar")), map));
        LocalNameNNPtr localname(
            NameFactory::createLocalName(nsIn, std::string("foo")));
        auto ns = localname->scope();
        std::cout << ns->isGlobal() << std::endl;
        auto fullyqualifiedNS = ns->name()->toFullyQualifiedName();
        std::cout << fullyqualifiedNS->toString() << std::endl;
        std::cout << fullyqualifiedNS->scope()->isGlobal() << std::endl;
        std::cout << fullyqualifiedNS->scope()->name()->scope()->isGlobal()
                  << std::endl;
        std::cout << localname->toFullyQualifiedName()->toString() << std::endl;
    }

    return 0;
}
