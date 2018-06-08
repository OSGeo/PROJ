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
#error This file should only be included from a PROJ cpp file
#endif

#ifndef COORDINATEOPERATION_INTERNAL_HH_INCLUDED
#define COORDINATEOPERATION_INTERNAL_HH_INCLUDED

#include "coordinateoperation.hpp"

#include <vector>

//! @cond Doxygen_Suppress

NS_PROJ_START

namespace operation {

struct MethodMapping {
    typedef struct {
        std::string wkt2_name;
        int epsg_code;
        std::string wkt1_name;
    } ParamMapping;

    std::string wkt2_name;
    int epsg_code;
    std::string wkt1_name;
    std::vector<ParamMapping> params;
};

const MethodMapping *getMapping(int epsg_code);
const MethodMapping *getMappingFromWKT1(const std::string &wkt1_name);
const MethodMapping *getMapping(const OperationMethod *method);
const MethodMapping::ParamMapping *
getMapping(const MethodMapping *mapping, const OperationParameterValue *param);
const MethodMapping::ParamMapping *
getMappingFromWKT1(const MethodMapping *mapping, const std::string &wkt1_name);

} // namespace operation

NS_PROJ_END

//! @endcond

#endif // COORDINATEOPERATION_INTERNAL_HH_INCLUDED
