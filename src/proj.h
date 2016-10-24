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


/******************************************************************************
    proj.h is included by projects.h in order to determine the size of the
    PJ_OBSERVATION object.

    In order to stomp as little as possible on the traditional proj.4 name
    space, proj.h is littered with inclusion guards, leaving only the minimum
    possible implementation when included from projects.h

    The PJ_OBSERVATION object is fully defined if proj.h is included alone or
    in connection with *but before* projects.h (the latter may be needed in
    some cases, where it is necessary to access this "bare essentials" API,
    while still having direct access to PJ object internals)
******************************************************************************/

/* Data type for generic geodetic observations */
struct PJ_OBSERVATION;
typedef struct PJ_OBSERVATION PJ_OBSERVATION;

/* Data type for generic geodetic 3D data */
union PJ_TRIPLET;
typedef union PJ_TRIPLET PJ_TRIPLET;

/* Data type for generic geodetic 3D data plus epoch information */
union PJ_SPATIOTEMPORAL;
typedef union PJ_SPATIOTEMPORAL PJ_SPATIOTEMPORAL;

#ifndef PROJECTS_H
/* Data type for projection/transformation information */
struct PJconsts;
typedef struct PJconsts PJ;         /* the PJ object herself */

/* Omega, Phi, Kappa: Rotations */
typedef struct {double o, p, k;}  PJ_OPK;

/* Easting, Northing, and some kind of height (orthometric or ellipsoidal) */
typedef struct {double e, n, h;}  PJ_ENH;

/* Geodetic spatiotemporal coordinate types */
typedef struct { double   x,   y,  z, t; }  PJ_XYZT;
typedef struct { double   e,   n,  h, t; }  PJ_ENHT;
typedef struct { double   u,   v,  w, t; }  PJ_UVWT;
typedef struct { double lam, phi,  z, t; }  PJ_LPZT;

/* Classic proj.4 pair/triplet types */
typedef struct { double   u,   v; }  UV;
typedef struct { double   x,   y; }  XY;
typedef struct { double lam, phi; }  LP;

typedef struct { double   x,   y,  z; }  XYZ;
typedef struct { double   u,   v,  w; }  UVW;
typedef struct { double lam, phi,  z; }  LPZ;

/* Ancillary pairs and triplets for geodetic computations */

/* Degrees, minutes, and seconds */
typedef struct { double d, m,  s; }  PJ_DMS;

/* Geoid undulation (N) and deflections of the vertical (z, e) */
typedef struct { double   N,   z,  e; }  PJ_NZE;

/* Ellipsoidal parameters */
typedef struct { double   a,   f; }  PJ_AF;
#endif  /* ndef PROJECTS_H */

union PJ_SPATIOTEMPORAL {
#ifndef PROJECTS_H
    PJ_XYZT xyzt;
    PJ_UVWT uvwt;
    PJ_ENHT enht;
    PJ_LPZT lpzt;
    PJ_ENH  enh;
#endif
    double v[4]; /* Who cares - it's just a vector! */
    XYZ  xyz;
    UVW  uvw;
    LPZ  lpz;
    XY   xy;
    UV   uv;
    LP   lp;
};



/* Avoid preprocessor renaming and implicit type-punning: Use a union to make it explicit */
union PJ_TRIPLET {
#ifndef PROJECTS_H
    PJ_OPK  opk;
    PJ_ENH  enh;
    PJ_NZE  nze;
    PJ_AF   af;
#endif
    double v[3]; /* Who cares - it's just a vector! */
    XYZ    xyz;
    LPZ    lpz;
    UVW    uvw;
    XY     xy;
    LP     lp;
    UV     uv;
};

struct PJ_OBSERVATION {
    PJ_SPATIOTEMPORAL coo;  /* coordinate data */
    PJ_TRIPLET anc;         /* ancillary data */
    int id;                 /* integer ancillary data - e.g. observation number, EPSG code... */
    unsigned int flags;     /* additional data, intended for flags */
};



#ifndef PROJECTS_H

/* Direction: "+" forward, "-" reverse, 0: do nothing */
enum pj_direction {
    PJ_FWD   = -1,
    PJ_IDENT =  0,
    PJ_INV   =  1
};

/* Apply transformation to observation - in forward or inverse direction */
PJ_OBSERVATION pj_apply (PJ *P, enum pj_direction direction, PJ_OBSERVATION obs);

/* Measure internal consistency - in forward or inverse direction */
double pj_roundtrip(PJ *P, enum pj_direction direction, int n, PJ_OBSERVATION obs);


int pj_show_triplet (FILE *stream, const char *banner, PJ_TRIPLET point);

/* Constructor for the OBSERVATION object */
PJ_OBSERVATION pj_observation (
    double x, double y, double z, double t,
    double o, double p, double k,
    int id, unsigned int flags
);

#ifndef TODEG
#define TODEG(rad)  ((rad)*180.0/M_PI)
#endif
#ifndef TORAD
#define TORAD(deg)  ((deg)*M_PI/180.0)
#endif

#endif /* ndef PROJECTS_H */


#ifdef __cplusplus
}
#endif
#endif /* ndef PROJ_H */
