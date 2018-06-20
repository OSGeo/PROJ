/******************************************************************************
 *
 * Project:  PROJ
 * Purpose:  Test pj_ function.
 * Author:   Kurt Schwehr <schwehr@google.com>
 *
 ******************************************************************************
 * Copyright (c) 2018, Google Inc.
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

#include "projects.h"
#include "proj_strtod.h"

#include <cerrno>
#include <cmath>

#include "gtest_include.h"

namespace {

class ProjStrtodTest : public ::testing::Test {
  void SetUp() override { errno = 0; }
};

TEST_F(ProjStrtodTest, NullNull) {
  EXPECT_DOUBLE_EQ(HUGE_VAL, proj_strtod(nullptr, nullptr));
  EXPECT_EQ(EFAULT, errno);
}

TEST_F(ProjStrtodTest, Null) {
  char *endptr = reinterpret_cast<char *>(0xdeadbeef);
  EXPECT_DOUBLE_EQ(HUGE_VAL, proj_strtod(nullptr, &endptr));
  EXPECT_EQ(endptr, nullptr);
  EXPECT_EQ(EFAULT, errno);
}

TEST_F(ProjStrtodTest, EmptyString) {
  char *endptr = nullptr;
  const char text[] = "";
  EXPECT_DOUBLE_EQ(0.0, proj_strtod(text, &endptr));
  EXPECT_EQ(endptr, &text[sizeof(text) - 1]);
  EXPECT_EQ(0, errno);
}

TEST_F(ProjStrtodTest, Spaces) {
  char *endptr = nullptr;
  const char text[] = "   ";
  EXPECT_DOUBLE_EQ(0.0, proj_strtod(text, &endptr));
  EXPECT_EQ(endptr, &text[0]);
  EXPECT_EQ(0, errno);
}

TEST_F(ProjStrtodTest, UnderscoresAndSpace) {
  char *endptr = nullptr;
  const char text[] = "_ _ _";
  EXPECT_DOUBLE_EQ(0.0, proj_strtod(text, &endptr));
  EXPECT_EQ(endptr, &text[1]);
  EXPECT_EQ(0, errno);
}

TEST_F(ProjStrtodTest, Text) {
  char *endptr = nullptr;
  const char text[] = "a";
  EXPECT_DOUBLE_EQ(0.0, proj_strtod(text, &endptr));
  EXPECT_EQ(endptr, &text[0]);
  EXPECT_EQ(0, errno);
}

TEST_F(ProjStrtodTest, SpaceAndText) {
  char *endptr = nullptr;
  const char text[] = " b";
  EXPECT_DOUBLE_EQ(0.0, proj_strtod(text, &endptr));
  EXPECT_EQ(endptr, &text[0]);
  EXPECT_EQ(0, errno);
}

TEST_F(ProjStrtodTest, Plus) {
  char *endptr = nullptr;
  const char text[] = "+";
  EXPECT_DOUBLE_EQ(0.0, proj_strtod(text, &endptr));
  EXPECT_EQ(endptr, &text[0]);
  EXPECT_EQ(0, errno);
}

TEST_F(ProjStrtodTest, Minus) {
  char *endptr = nullptr;
  const char text[] = "-";
  EXPECT_DOUBLE_EQ(0.0, proj_strtod(text, &endptr));
  EXPECT_EQ(endptr, &text[0]);
  EXPECT_EQ(0, errno);
}

TEST_F(ProjStrtodTest, E1) {
  char *endptr = nullptr;
  const char text[] = "e1";
  EXPECT_DOUBLE_EQ(0.0, proj_strtod(text, &endptr));
  EXPECT_EQ(endptr, &text[0]);
  EXPECT_EQ(0, errno);
}

TEST_F(ProjStrtodTest, Zero) {
  char *endptr = nullptr;
  const char text[] = "0";
  EXPECT_DOUBLE_EQ(0.0, proj_strtod(text, &endptr));
  EXPECT_EQ(endptr, &text[sizeof(text) - 1]);
  EXPECT_EQ(0, errno);
}

TEST_F(ProjStrtodTest, NegZero) {
  char *endptr = nullptr;
  const char text[] = "-0";
  EXPECT_DOUBLE_EQ(0.0, proj_strtod(text, &endptr));
  EXPECT_EQ(endptr, &text[sizeof(text) - 1]);
  EXPECT_EQ(0, errno);
}

TEST_F(ProjStrtodTest, ZeroPointZero) {
  char *endptr = nullptr;
  const char text[] = "0.0";
  EXPECT_DOUBLE_EQ(0.0, proj_strtod(text, &endptr));
  EXPECT_EQ(endptr, &text[sizeof(text) - 1]);
  EXPECT_EQ(0, errno);
}

TEST_F(ProjStrtodTest, PlusNumber) {
  char *endptr = nullptr;
  const char text[] = "+8";
  EXPECT_DOUBLE_EQ(8.0, proj_strtod(text, &endptr));
  EXPECT_EQ(endptr, &text[sizeof(text) - 1]);
  EXPECT_EQ(0, errno);
}

TEST_F(ProjStrtodTest, MinusNumber) {
  char *endptr = nullptr;
  const char text[] = "-9";
  EXPECT_DOUBLE_EQ(-9.0, proj_strtod(text, &endptr));
  EXPECT_EQ(endptr, &text[sizeof(text) - 1]);
  EXPECT_EQ(0, errno);
}

TEST_F(ProjStrtodTest, TwoInts) {
  char *endptr = nullptr;
  const char text[] = "1 2";
  EXPECT_DOUBLE_EQ(1.0, proj_strtod(text, &endptr));
  EXPECT_EQ(endptr, &text[1]);
  EXPECT_EQ(0, errno);
}

TEST_F(ProjStrtodTest, TrailingDecimalPoint) {
  char *endptr = nullptr;
  const char text[] = "3.";
  EXPECT_DOUBLE_EQ(3.0, proj_strtod(text, &endptr));
  EXPECT_EQ(endptr, &text[sizeof(text) - 1]);
  EXPECT_EQ(0, errno);
}

TEST_F(ProjStrtodTest, TrailingUnderscore) {
  char *endptr = nullptr;
  const char text[] = "4_";
  EXPECT_DOUBLE_EQ(4.0, proj_strtod(text, &endptr));
  EXPECT_EQ(endptr, &text[sizeof(text) - 1]);
  EXPECT_EQ(0, errno);
}

TEST_F(ProjStrtodTest, MiddleUnderscore) {
  // Underscores are ignored.
  char *endptr = nullptr;
  const char text[] = "5_6";
  EXPECT_DOUBLE_EQ(56.0, proj_strtod(text, &endptr));
  EXPECT_EQ(endptr, &text[sizeof(text) - 1]);
  EXPECT_EQ(0, errno);
}

TEST_F(ProjStrtodTest, MiddleUnderscoreAndDecimalPoint) {
  char *endptr = nullptr;
  const char text[] = "6_.7";
  EXPECT_DOUBLE_EQ(6.7, proj_strtod(text, &endptr));
  EXPECT_EQ(endptr, &text[sizeof(text) - 1]);
  EXPECT_EQ(0, errno);
}

TEST_F(ProjStrtodTest, UnderscoreEverywhere) {
  char *endptr = nullptr;
  const char text[] = "_7_._8_";
  EXPECT_DOUBLE_EQ(7.8, proj_strtod(text, &endptr));
  EXPECT_EQ(endptr, &text[sizeof(text) - 1]);
  EXPECT_EQ(0, errno);
}

TEST_F(ProjStrtodTest, OneE) {
  // 'e' is ignored.
  char *endptr = nullptr;
  const char text[] = "1e";
  EXPECT_DOUBLE_EQ(1.0, proj_strtod(text, &endptr));
  EXPECT_EQ(endptr, &text[sizeof(text) - 2]);
  EXPECT_EQ(0, errno);
}

TEST_F(ProjStrtodTest, EZero) {
  char *endptr = nullptr;
  const char text[] = "2e0";
  EXPECT_DOUBLE_EQ(2.0, proj_strtod(text, &endptr));
  EXPECT_EQ(endptr, &text[sizeof(text) - 1]);
  EXPECT_EQ(0, errno);
}

TEST_F(ProjStrtodTest, ENegativeNegative) {
  char *endptr = nullptr;
  const char text[] = "-3.4e-2";
  EXPECT_DOUBLE_EQ(-0.034, proj_strtod(text, &endptr));
  EXPECT_EQ(endptr, &text[sizeof(text) - 1]);
  EXPECT_EQ(0, errno);
}

TEST_F(ProjStrtodTest, PrefixZeros) {
  char *endptr = nullptr;
  const char text[] = "00000009";
  EXPECT_DOUBLE_EQ(9.0, proj_strtod(text, &endptr));
  EXPECT_EQ(endptr, &text[sizeof(text) - 1]);
  EXPECT_EQ(0, errno);
}

TEST_F(ProjStrtodTest, LargeExponent) {
  char *endptr = nullptr;
  const char text[] = "1e+99";
  EXPECT_DOUBLE_EQ(1.0e+99, proj_strtod(text, &endptr));
  EXPECT_EQ(endptr, &text[sizeof(text) - 1]);
  EXPECT_EQ(0, errno);
}

TEST_F(ProjStrtodTest, LargeNegativeExponent) {
  char *endptr = nullptr;
  const char text[] = "1e-99";
  EXPECT_DOUBLE_EQ(1.0e-99, proj_strtod(text, &endptr));
  EXPECT_EQ(endptr, &text[sizeof(text) - 1]);
  EXPECT_EQ(0, errno);
}

TEST_F(ProjStrtodTest, ExponentTooLarge) {
  char *endptr = nullptr;
  const char text[] = "1e+99999";
  EXPECT_DOUBLE_EQ(HUGE_VAL, proj_strtod(text, &endptr));
  EXPECT_EQ(endptr, &text[sizeof(text) - 1]);
  EXPECT_EQ(ERANGE, errno);
}

TEST_F(ProjStrtodTest, ExponentTooNegative) {
  char *endptr = nullptr;
  const char text[] = "1e-99999";
  EXPECT_DOUBLE_EQ(HUGE_VAL, proj_strtod(text, &endptr));
  EXPECT_EQ(endptr, &text[sizeof(text) - 1]);
  EXPECT_EQ(ERANGE, errno);
}

TEST_F(ProjStrtodTest, Inf) {
  // Inf is not supported contrary to glibc behavior.
  char *endptr = nullptr;
  const char text[] = "INF";
  EXPECT_DOUBLE_EQ(0.0, proj_strtod(text, &endptr));
  EXPECT_EQ(endptr, &text[0]);
  EXPECT_EQ(0, errno);
}

TEST_F(ProjStrtodTest, Nan) {
  // Nan is not supported contrary to glibc behavior.
  char *endptr = nullptr;
  const char text[] = "NAN";
  EXPECT_DOUBLE_EQ(0.0, proj_strtod(text, &endptr));
  EXPECT_EQ(endptr, &text[0]);
  EXPECT_EQ(0, errno);
}

TEST_F(ProjStrtodTest, Hex) {
  // Hex is not supported contrary to glibc behavior.
  char *endptr = nullptr;
  const char text[] = "0x1";
  EXPECT_DOUBLE_EQ(0.0, proj_strtod(text, &endptr));
  EXPECT_EQ(endptr, &text[1]);
  EXPECT_EQ(0, errno);
}

}  // namespace
