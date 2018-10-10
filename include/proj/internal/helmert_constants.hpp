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

#ifndef HELMERT_CONSTANTS_HH_INCLUDED
#define HELMERT_CONSTANTS_HH_INCLUDED

constexpr int EPSG_CODE_PARAMETER_X_AXIS_TRANSLATION = 8605;
constexpr int EPSG_CODE_PARAMETER_Y_AXIS_TRANSLATION = 8606;
constexpr int EPSG_CODE_PARAMETER_Z_AXIS_TRANSLATION = 8607;
constexpr int EPSG_CODE_PARAMETER_X_AXIS_ROTATION = 8608;
constexpr int EPSG_CODE_PARAMETER_Y_AXIS_ROTATION = 8609;
constexpr int EPSG_CODE_PARAMETER_Z_AXIS_ROTATION = 8610;
constexpr int EPSG_CODE_PARAMETER_SCALE_DIFFERENCE = 8611;

constexpr int EPSG_CODE_PARAMETER_RATE_X_AXIS_TRANSLATION = 1040;
constexpr int EPSG_CODE_PARAMETER_RATE_Y_AXIS_TRANSLATION = 1041;
constexpr int EPSG_CODE_PARAMETER_RATE_Z_AXIS_TRANSLATION = 1042;
constexpr int EPSG_CODE_PARAMETER_RATE_X_AXIS_ROTATION = 1043;
constexpr int EPSG_CODE_PARAMETER_RATE_Y_AXIS_ROTATION = 1044;
constexpr int EPSG_CODE_PARAMETER_RATE_Z_AXIS_ROTATION = 1045;
constexpr int EPSG_CODE_PARAMETER_RATE_SCALE_DIFFERENCE = 1046;
constexpr int EPSG_CODE_PARAMETER_REFERENCE_EPOCH = 1047;

static const std::string
    EPSG_NAME_PARAMETER_X_AXIS_TRANSLATION("X-axis translation");
static const std::string
    EPSG_NAME_PARAMETER_Y_AXIS_TRANSLATION("Y-axis translation");
static const std::string
    EPSG_NAME_PARAMETER_Z_AXIS_TRANSLATION("Z-axis translation");
static const std::string EPSG_NAME_PARAMETER_X_AXIS_ROTATION("X-axis rotation");
static const std::string EPSG_NAME_PARAMETER_Y_AXIS_ROTATION("Y-axis rotation");
static const std::string EPSG_NAME_PARAMETER_Z_AXIS_ROTATION("Z-axis rotation");
static const std::string
    EPSG_NAME_PARAMETER_SCALE_DIFFERENCE("Scale difference");

static const std::string EPSG_NAME_PARAMETER_RATE_X_AXIS_TRANSLATION(
    "Rate of change of X-axis translation");
static const std::string EPSG_NAME_PARAMETER_RATE_Y_AXIS_TRANSLATION(
    "Rate of change of Y-axis translation");
static const std::string EPSG_NAME_PARAMETER_RATE_Z_AXIS_TRANSLATION(
    "Rate of change of Z-axis translation");
static const std::string EPSG_NAME_PARAMETER_RATE_X_AXIS_ROTATION(
    "Rate of change of X-axis rotation");
static const std::string EPSG_NAME_PARAMETER_RATE_Y_AXIS_ROTATION(
    "Rate of change of Y-axis rotation");
static const std::string EPSG_NAME_PARAMETER_RATE_Z_AXIS_ROTATION(
    "Rate of change of Z-axis rotation");
static const std::string EPSG_NAME_PARAMETER_RATE_SCALE_DIFFERENCE(
    "Rate of change of Scale difference");
static const std::string
    EPSG_NAME_PARAMETER_REFERENCE_EPOCH("Parameter reference epoch");

#endif // HELMERT_CONSTANTS_HH_INCLUDED
