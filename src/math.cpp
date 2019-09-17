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

/* Define C99 compatible versions of
 *     hypot
 *     log1p
 *     asinh
 *     atanh
 *     copysign
 *     cbrt
 *     remainder
 *     remquo
 *     round
 *     lround
 */

/* Compute hypotenuse */
double pj_hypot(double x, double y) {
  x = fabs(x);
  y = fabs(y);
  if (x < y) {
    x /= y;                     /* y is nonzero */
    return y * sqrt(1 + x * x);
  } else {
    y /= (x != 0 ? x : 1);
    return x * sqrt(1 + y * y);
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
  double y = fabs(x);           /* Enforce odd parity */
  y = pj_log1p(y * (1 + y/(pj_hypot(1.0, y) + 1)));
  return x > 0 ? y : (x < 0 ? -y : x); /* asinh(-0.0) = -0.0 */
}

/* Compute atanh(x) accurately */
double pj_atanh(double x) {
  double y = fabs(x);             /* Enforce odd parity */
  y = pj_log1p(2 * y/(1 - y))/2;
  return x > 0 ? y : (x < 0 ? -y : x); /* atanh(-0.0) = -0.0 */
}

/* Implement copysign(x, y)  */
double pj_copysign(double x, double y) {
  /* 1/y trick to get the sign of -0.0 */
  return fabs(x) * (y < 0 || (y == 0 && 1/y < 0) ? -1 : 1);
}

/* Implement cbrt(x)  */
double pj_cbrt(double x) {
  double y = pow(fabs(x), 1/3.0);      /* Return the real cube root */
  return x > 0 ? y : (x < 0 ? -y : x); /* cbrt(-0.0) = -0.0 */
}

/* Implement remainder(x, y) with ties to even */
double pj_remainder(double x, double y) {
  double z;
  y = fabs(y);                 /* The result doesn't depend on the sign of y */
  z = fmod(x, y);
  if (z == 0)
    /* This shouldn't be necessary.  However, before version 14 (2015),
     * Visual Studio had problems dealing with -0.0.  Specifically
     *   VC 10,11,12 and 32-bit compile: fmod(-0.0, 360.0) -> +0.0
     * python 2.7 on Windows 32-bit machines has the same problem. */
    z = pj_copysign(z, x);
  else if (2 * fabs(z) == y)
    z -= fmod(x, 2 * y) - z;    /* Implement ties to even */
  else if (2 * fabs(z) > y)
    z += (z < 0 ? y : -y);      /* Fold remaining cases to (-y/2, y/2) */
  return z;
}

/* Implement remquo(x, y, n) with n giving low 3 bits + sign of x/y */
double pj_remquo(double x, double y, int* n) {
  double z = pj_remainder(x, y);
  if (n) {
    double
      a = pj_remainder(x, 2 * y),
      b = pj_remainder(x, 4 * y),
      c = pj_remainder(x, 8 * y);
    *n  = (a > z ? 1 : (a < z ? -1 : 0));
    *n += (b > a ? 2 : (b < a ? -2 : 0));
    *n += (c > b ? 4 : (c < b ? -4 : 0));
    if (y < 0) *n *= -1;
    if (y != 0) {
      if (x/y > 0 && *n <= 0)
        *n += 8;
      else if (x/y < 0 && *n >= 0)
        *n -= 8;
    }
  }
  return z;
}

/* Implement round(x) */
double pj_round(double x) {
  /* The handling of corner cases is copied from boost; see
   *   https://github.com/boostorg/math/pull/8
   * with improvements to return -0.0 when appropriate */
  double t;
  if      (0 < x && x <  0.5)
    return +0.0;
  else if (0 > x && x > -0.5)
    return -0.0;
  else if   (x > 0) {
    t = ceil(x);
    return 0.5 < t - x ? t - 1 : t;
  } else if (x < 0) {
    t = floor(x);
    return 0.5 < x - t ? t + 1 : t;
  } else                        /* +/-0 and NaN */
    return x;                   /* retain sign of 0 */
}

/* Implement lround(x) */
long pj_lround(double x) {
  /* Default value for overflow + NaN + (x == LONG_MIN) */
  long r = LONG_MIN;
  x = pj_round(x);
  if (fabs(x) < -(double)r)     /* Assume (double)LONG_MIN is exact */
    r = (long)x;
  return r;
}

#endif /* !(defined(HAVE_C99_MATH) && HAVE_C99_MATH) */
