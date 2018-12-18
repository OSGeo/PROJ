/******************************************************************************
 * Project:  PROJ
 * Purpose:  Make C99 math functions available on C89 systems
 * Author:   Kristian Evers
 *
 ******************************************************************************
 * Copyright (c) 2018, Kristian Evers
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
 *****************************************************************************/

#include "proj_math.h"

/* pj_isnan is used in gie.c which means that is has to */
/* be exported in the Windows DLL and therefore needs   */
/* to be declared even though we have isnan() on the    */
/* system.                                              */

#ifdef HAVE_C99_MATH
int pj_isnan (double x);
#endif

/* Returns 0 if not a NaN and non-zero if val is a NaN */
int pj_isnan (double x) {
    /* cppcheck-suppress duplicateExpression */
    return x != x;
}

#if !(defined(HAVE_C99_MATH) && HAVE_C99_MATH)

/* Compute hypotenuse */
double pj_hypot(double x, double y) {
    x = fabs(x);
    y = fabs(y);
    if ( x < y ) {
        x /= y;
        return ( y * sqrt( 1. + x * x ) );
    } else {
        y /= (x != 0.0 ? x : 1.0);
        return ( x * sqrt( 1. + y * y ) );
    }
}

/* Compute log(1+x) accurately */
double pj_log1p(double x) {
    volatile double
      y = 1 + x,
      z = y - 1;
    /* Here's the explanation for this magic: y = 1 + z, exactly, and z
     * approx x, thus log(y)/z (which is nearly constant near z = 0) returns
     * a good approximation to the true log(1 + x)/x.  The multiplication x *
     * (log(y)/z) introduces little additional error. */
    return z == 0 ? x : x * log(y) / z;
}

/* Compute asinh(x) accurately */
double pj_asinh(double x) {
    double y = fabs(x); /* Enforce odd parity */
    y = log1p(y * (1 + y/(hypot(1.0, y) + 1)));
    return x > 0 ? y : (x < 0 ? -y : x);
}

double pj_round(double x) {
  /* The handling of corner cases is copied from boost; see
   *   https://github.com/boostorg/math/pull/8
   * with improvements to return -0.0 when appropriate */
  double t;
  if (x == 0)
    return x;               /* Retain sign of 0 */
  else if (0 < x && x <  0.5)
    return +0.0;
  else if (0 > x && x > -0.5)
    return -0.0;
  else if (x > 0) {
    t = ceil(x);
    return 0.5 < t - x ? t - 1 : t;
  } else {                  /* Includes NaN */
    t = floor(x);
    return 0.5 < x - t ? t + 1 : t;
  }
}

long pj_lround(double x) {
  /* Default value for overflow + NaN + (x == LONG_MIN) */
  long r = LONG_MIN;
  x = round(x);
  if (fabs(x) < -(double)LONG_MIN) /* Assume (double)LONG_MIN is exact */
    r = (int)x;
  return r;
}

#endif /* !(defined(HAVE_C99_MATH) && HAVE_C99_MATH) */
