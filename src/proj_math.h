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

#ifndef PROJ_MATH_H
#define PROJ_MATH_H

#include <math.h>
#include <limits.h>

#ifdef __cplusplus
extern "C" {
#endif

#if !(defined(HAVE_C99_MATH) && HAVE_C99_MATH)

#ifndef NAN
#ifdef _WIN32
#define NAN sqrt(-1.0)
#else
#define NAN 0.0/0.0
#endif
#endif

double  pj_hypot(double x, double y);
double  pj_log1p(double x);
double  pj_asinh(double x);
double  pj_round(double x);
long    pj_lround(double x);
int     pj_isnan(double x);

#define hypot   pj_hypot
#define log1p   pj_log1p
#define asinh   pj_asinh
#define round   pj_round
#define lround  pj_lround


#ifdef isnan
#undef isnan
#endif

#define isnan   pj_isnan

#endif /* !(defined(HAVE_C99_MATH) && HAVE_C99_MATH) */

#ifdef __cplusplus
}
#endif

#endif /* PROJ_MATH_H */
