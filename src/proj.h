/******************************************************************************
 * Project:  PROJ.4
 * Purpose:  Revised, experimental, "bare essentials" API for PROJ.4.
 * Author:   Thomas Knudsen, <thokn@sdfe.dk>
 *
 ******************************************************************************
 * Copyright (c) 2016, Thomas Knudsen / SDFE
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
#ifdef _MSC_VER
#define _USE_MATH_DEFINES
#endif
#include <math.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#ifndef PROJ_H
#define PROJ_H
#ifdef __cplusplus
extern "C" {
#endif



/* Data type for projection/transformation information */
#ifndef PROJECTS_H
struct PJconsts;
typedef struct PJconsts PJ;         /* the PJ object herself */
#endif


/* Data type for generic geodetic 3D data */
union TRIPLET;
typedef union TRIPLET TRIPLET;





PJ *pj_init_plus(const char *init_string);


/* Direction: "+" forward, "-" reverse, 0: roundtrip precision check */
TRIPLET pj_apply (PJ *P, int direction, TRIPLET point);
/* Apply the most appropriate projection function. No-op if none appropriate */
TRIPLET pj_apply_projection (TRIPLET point, int direction, PJ *P);


int pj_show_triplet (FILE *stream, const char *banner, TRIPLET point);

TRIPLET pj_triplet (double x, double y, double z);

void pj_free(PJ *P);



/* Omega, Phi, Kappa: Rotations */
typedef struct {double o, p, k;}  OPK;

/* Northing, Easting, and geodetic height */
typedef struct {double n, e, h;}  NEh;
typedef struct {double n, e, h;}  ENh;

/* Northing, Easting, and orthometric height */
typedef struct {double n, e, H;}  NEH;
typedef struct {double n, e, H;}  ENH;

/* Northing, Easting, and some kind of height */
typedef struct {double n, e, z;}  NEZ;
typedef struct {double n, e, z;}  ENZ;

/* Red, green and blue (e.g. for LiDAR colouring) */
typedef struct {double r, g, b;}  TRIPLET_RGB;

/* Generic indices */
typedef struct {double i, j, k;}  TRIPLET_IJK;

/* An overdetermined ellipsoid */
typedef struct {double a, f, es;}  TRIPLET_ELLIPSOID;

/* Classic proj.4 pair/triplet types */
#ifndef PROJECTS_H
typedef struct { double   u,   v; }  UV;
typedef struct { double   x,   y; }  XY;
typedef struct { double lam, phi; }  LP;

typedef struct { double   x,   y,  z; }  XYZ;
typedef struct { double   u,   v,  w; }  UVW;
typedef struct { double lam, phi,  z; }  LPZ;
#endif


/* Avoid preprocessor renaming and  imlicit type-punning: Use a union to make it explicit */
union TRIPLET {
	double v[3]; /* Who cares - it's just a vector! */
    XYZ    xyz;
    LPZ    lpz;
	UVW    uvw;
    OPK    opk;
    NEh    neh;
    NEH    neH;
    NEZ    nez;

    TRIPLET_RGB rgb;
    TRIPLET_IJK ijk;
    TRIPLET_ELLIPSOID ellipsoid;
    XY  xy;
    LP  lp;
	UV  uv;
};





#define TODEG(rad)  ((rad)*180/M_PI)
#define TORAD(deg)  ((deg)*M_PI/180)


extern int pj_errno;                    /* global error return code */



#ifdef __cplusplus
}
#endif
#endif /* ndef PROJ_H */