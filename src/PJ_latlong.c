/******************************************************************************
 * Project:  PROJ.4
 * Purpose:  Stub projection implementation for lat/long coordinates. We
 *           don't actually change the coordinates, but we want proj=latlong
 *           to act sort of like a projection.
 * Author:   Frank Warmerdam, warmerdam@pobox.com
 *
 ******************************************************************************
 * Copyright (c) 2000, Frank Warmerdam
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

/* very loosely based upon DMA code by Bradford W. Drew */
#define PJ_LIB__
#include "proj_internal.h"
#include <proj.h>
#include "projects.h"

PROJ_HEAD(lonlat, "Lat/long (Geodetic)")  "\n\t";
PROJ_HEAD(latlon, "Lat/long (Geodetic alias)")  "\n\t";
PROJ_HEAD(latlong, "Lat/long (Geodetic alias)")  "\n\t";
PROJ_HEAD(longlat, "Lat/long (Geodetic alias)")  "\n\t";


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

static PJ_COORD forward_4d(PJ_COORD obs, PJ *P) {
    (void) P;
    return obs;
}

static PJ_COORD inverse_4d(PJ_COORD obs, PJ *P) {
    (void) P;
    return obs;
}

PJ *PROJECTION(latlong) {
    P->is_latlong = 1;
    P->x0 = 0.0;
    P->y0 = 0.0;
    P->inv = inverse;
    P->fwd = forward;
    P->inv4d = inverse_4d;
    P->fwd4d = forward_4d;
    P->left = PJ_IO_UNITS_RADIANS;
    P->right = PJ_IO_UNITS_RADIANS;

    return P;
}


PJ *PROJECTION(longlat) {
    P->is_latlong = 1;
    P->x0 = 0.0;
    P->y0 = 0.0;
    P->inv = inverse;
    P->fwd = forward;
    P->inv4d = inverse_4d;
    P->fwd4d = forward_4d;
    P->left = PJ_IO_UNITS_RADIANS;
    P->right = PJ_IO_UNITS_RADIANS;

    return P;
}


PJ *PROJECTION(latlon) {
    P->is_latlong = 1;
    P->x0 = 0.0;
    P->y0 = 0.0;
    P->inv = inverse;
    P->fwd = forward;
    P->inv4d = inverse_4d;
    P->fwd4d = forward_4d;
    P->left = PJ_IO_UNITS_RADIANS;
    P->right = PJ_IO_UNITS_RADIANS;

    return P;
}


PJ *PROJECTION(lonlat) {
    P->is_latlong = 1;
    P->x0 = 0.0;
    P->y0 = 0.0;
    P->inv = inverse;
    P->fwd = forward;
    P->inv4d = inverse_4d;
    P->fwd4d = forward_4d;
    P->left = PJ_IO_UNITS_RADIANS;
    P->right = PJ_IO_UNITS_RADIANS;

    return P;
}

