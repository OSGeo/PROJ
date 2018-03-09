/******************************************************************************
 * Project:  PROJ.4
 * Purpose:  Stub projection for geocentric.  The transformation isn't
 *           really done here since this code is 2D.  The real transformation
 *           is handled by pj_transform.c.
 * Author:   Frank Warmerdam, warmerdam@pobox.com
 *
 ******************************************************************************
 * Copyright (c) 2002, Frank Warmerdam
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

#define PJ_LIB__
#include "projects.h"

PROJ_HEAD(geocent, "Geocentric")  "\n\t";

static XY forward(LP lp, PJ *P) {
    XY xy = {0.0,0.0};
    (void) P;
    xy.x = lp.lam;
    xy.y = lp.phi;
    return xy;
}

static LP inverse(XY xy, PJ *P) {
    LP lp = {0.0,0.0};
    (void) P;
    lp.phi = xy.y;
    lp.lam = xy.x;
    return lp;
}

PJ *CONVERSION (geocent, 0) {
    P->is_geocent = 1;
    P->x0 = 0.0;
    P->y0 = 0.0;
    P->inv = inverse;
    P->fwd = forward;
    P->left = PJ_IO_UNITS_ANGULAR;
    P->right = PJ_IO_UNITS_CARTESIAN;

    return P;
}

