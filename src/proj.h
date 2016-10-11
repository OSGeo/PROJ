/******************************************************************************
 * Project:  PROJ.4
 * Purpose:  Revised, experimental, "bare essentials" API for PROJ.4.
 *           Intended as the foundation for added geodetic functionality.
 *
 *           Introduces the OBSERVATION data type, for generic coordinate
 *           and ancillary data handling.
 *
 *           Also introduces the PJ_SPATIOTEMPORAL and PJ_TRIPLET unions
 *           making it possible to make explicit the previously used
 *           "implicit type punning", where a XY is turned into a LP by
 *           re#defining both as UV, behind the back of the user.
 *
 *           The bare essentials API presented here follows the PROJ.4
 *           convention of sailing the coordinate to be reprojected, up on
 *           the stack ("call by value"), and symmetrically returning the
 *           result on the stack. Although the OBSERVATION object is 4 times
 *           as large as the traditional XY and LP objects, timing results
 *           have shown the overhead to be very reasonable.
 *
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

/* Forward declaration of a data type for generic geodetic 3D data */
union PJ_TRIPLET;
typedef union PJ_TRIPLET PJ_TRIPLET;

union PJ_SPATIOTEMPORAL;
typedef union PJ_SPATIOTEMPORAL PJ_SPATIOTEMPORAL;


/* Forward declaration of a data type for generic geodetic observations */
struct OBSERVATION;
typedef struct OBSERVATION OBSERVATION;


PJ *pj_init_plus(const char *init_string);

/* Direction: "+" forward, "-" reverse, 0: roundtrip precision check */
OBSERVATION pj_apply (PJ *P, int direction, OBSERVATION obs);
/* Apply the most appropriate projection function. No-op if none appropriate */
OBSERVATION pj_apply_projection (OBSERVATION obs, int direction, PJ *P);


int pj_show_triplet (FILE *stream, const char *banner, PJ_TRIPLET point);

OBSERVATION pj_observation (
	double x, double y, double z, double t,
	double o, double p, double k,
	int id, unsigned int flags
);

void pj_free(PJ *P);


/* Omega, Phi, Kappa: Rotations */
typedef struct {double o, p, k;}  OPK;

/* Easting, Northing, and some kind of height (orthometric or ellipsoidal) */
typedef struct {double e, n, h;}  ENH;

#ifndef PROJECTS_H

/* Geodetic spatiotemporal coordinate types */
typedef struct { double   x,   y,  z, t; }  XYZT;
typedef struct { double   e,   n,  h, t; }  ENHT;
typedef struct { double   u,   v,  w, t; }  UVWT;
typedef struct { double lam, phi,  z, t; }  LPZT;

/* Classic proj.4 pair/triplet types */
typedef struct { double   u,   v; }  UV;
typedef struct { double   x,   y; }  XY;
typedef struct { double lam, phi; }  LP;

typedef struct { double   x,   y,  z; }  XYZ;
typedef struct { double   u,   v,  w; }  UVW;
typedef struct { double lam, phi,  z; }  LPZ;


union PJ_SPATIOTEMPORAL {
	double v[4]; /* Who cares - it's just a vector! */
	XYZT xyzt;
	UVWT uvwt;
	ENHT enht;
	LPZT lpzt;
	XYZ  xyz;
	UVW  uvw;
	ENH  enh;
	LPZ  lpz;
	XY   xy;
	UV   uv;
	LP   lp;
};



/* Ancillary pairs and triplets for geodetic computations */

/* Geoid undulation and deflections of the vertical */
typedef struct { double   N,   z,  e; }  PJ_ANC_GEOIDAL;

/* Ellipsoidal parameters */
typedef struct { double   a,   f; }  PJ_ANC_ELLIPSOIDAL;


#endif

/* Avoid preprocessor renaming and implicit type-punning: Use a union to make it explicit */
union PJ_TRIPLET {
	double v[3]; /* Who cares - it's just a vector! */
    XYZ    xyz;
    LPZ    lpz;
	UVW    uvw;
	OPK    opk;
	ENH    enh;
    XY     xy;
    LP     lp;
	UV     uv;

	PJ_ANC_GEOIDAL     nze;
	PJ_ANC_ELLIPSOIDAL ell;
};

struct OBSERVATION {
	PJ_SPATIOTEMPORAL coo;
	PJ_TRIPLET anc;
	int id;
	unsigned int flags;
};

#define TODEG(rad)  ((rad)*180/M_PI)
#define TORAD(deg)  ((deg)*M_PI/180)

extern int pj_errno;                    /* global error return code */

#ifdef __cplusplus
}
#endif
#endif /* ndef PROJ_H */
