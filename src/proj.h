/******************************************************************************
 * Project:  PROJ.4
 * Purpose:  Revised, experimental API for PROJ.4, intended as the foundation
 *           for added geodetic functionality.
 *
 *           The original proj API (defined in projects.h) has grown organically
 *           over the years, but it has also grown somewhat messy.
 *
 *           The same has happened with the newer high level API (defined in
 *           proj_api.h): To support various historical objectives, proj_api.h
 *           contains a rather complex combination of conditional defines and
 *           typedefs. Probably for good (historical) reasons, which are not
 *           always evident from today's perspective.
 *
 *           This is an evolving attempt at creating a re-rationalized API
 *           with primary design goals focused on sanitizing the namespaces.
 *           Hence, all symbols exposed are being moved to the pj_ namespace,
 *           while all data types are being moved to the PJ_ namespace.
 *
 *           Please note that this API is *orthogonal* to  the previous APIs:
 *           Apart from some inclusion guards, projects.h and proj_api.h are not
 *           touched - if you do not include proj.h, the projects and proj_api
 *           APIs should work as they always have.
 *
 *           A few implementation details:
 *
 *           Apart from the namespacing efforts, I'm trying to eliminate three
 *           proj_api elements, which I have found especially confusing.
 *
 *           FIRST and foremost, I try to avoid typedef'ing away pointer
 *           semantics. I agree that it can be occasionally useful, but I
 *           prefer having the pointer nature of function arguments being
 *           explicitly visible.
 *
 *           Hence, projCtx has been replaced by PJ_CONTEXT *.
 *           and    projPJ  has been replaced by PJ *
 *
 *           SECOND, I try to eliminate cases of information hiding implemented
 *           by redefining data types to void pointers.
 *
 *           I prefer using a combination of forward declarations and typedefs.
 *           Hence:
 *               typedef void *projCtx;
 *           Has been replaced by:
 *               struct projCtx_t;
 *               typedef struct projCtx_t PJ_CONTEXT;
 *           This makes it possible for the calling program to know that the
 *           PJ_CONTEXT data type exists, and handle pointers to that data type
 *           without having any idea about its internals.
 *
 *           (obviously, in this example, struct projCtx_t should also be
 *           renamed struct pj_ctx some day...)
 *
 *           THIRD, I try to eliminate implicit type punning. Hence this API
 *           introduces the PJ_OBS ("observation") data type, for generic
 *           coordinate and handling of ancillary data.
 *
 *           It includes the PJ_COORD and PJ_TRIPLET unions making it possible
 *           to make explicit the previously used "implicit type punning", where
 *           a XY is turned into a LP by re#defining both as UV, behind the back
 *           of the user.
 *
 *           The PJ_COORD union is used for storing 1D, 2D, 3D and 4D coordinates.
 *           The PJ_TRIPLET union is used for storing any set of up to 3 related
 *           observations. At the application code level, the names of these
 *           unions will usually not be used - they will only be accessed via
 *           their tag names in the PJ_OBS data type.
 *
 *           The bare essentials API presented here follows the PROJ.4
 *           convention of sailing the coordinate to be reprojected, up on
 *           the stack ("call by value"), and symmetrically returning the
 *           result on the stack. Although the PJ_OBS object is 4 times
 *           as large as the traditional XY and LP objects, timing results
 *           have shown the overhead to be very reasonable.
 *
 *           In its current incarnation, the API is focused on compactness:
 *           Currently it consists of only nine functions.
 *
 *           Hence, due to the proj_ctx subsystem being little used, it has
 *           been replaced by a tiny set of 3 functions, making it possible,
 *           but not very convenient, to do almost everything possible with
 *           the original ctx API.
 *
 *           See pj_proj_test.c for an example of how to use the API.
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
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO COORD SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *****************************************************************************/
#ifdef _MSC_VER
#define _USE_MATH_DEFINES
#endif
#include <math.h>

#include <float.h>
#include <limits.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#ifdef PROJECTS_H
#error proj.h must be included before projects.h
#endif
#ifdef PROJ_API_H
#error proj.h must be included before proj_api.h
#endif

#ifndef PROJ_H
#define PROJ_H
#ifdef __cplusplus
extern "C" {
#endif


/* We need to access the PJ_VERSION symbol from proj_api.h */
#define PROJ_API_INCLUDED_FOR_PJ_VERSION_ONLY
#include <proj_api.h>
#undef  PROJ_API_INCLUDED_FOR_PJ_VERSION_ONLY

extern char const pj_release[]; /* global release id string */
extern int        pj_errno;     /* global error return code */

/* first forward declare everything needed */

/* Data type for generic geodetic observations */
struct PJ_OBS;
typedef struct PJ_OBS PJ_OBS;

/* Data type for generic geodetic 3D data */
union PJ_TRIPLET;
typedef union PJ_TRIPLET PJ_TRIPLET;

/* Data type for generic geodetic 3D data plus epoch information */
union PJ_COORD;
typedef union PJ_COORD PJ_COORD;

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

/* Geoid undulation (N) and deflections of the vertical (eta, zeta) */
typedef struct { double   e,  z, N; }  PJ_EZN;

/* Ellipsoidal parameters */
typedef struct { double   a,   f; }  PJ_AF;

/* Avoid preprocessor renaming and implicit type-punning: Use unions to make it explicit */
union PJ_COORD {
    PJ_XYZT xyzt;
    PJ_UVWT uvwt;
    PJ_ENHT enht;
    PJ_LPZT lpzt;
    PJ_ENH  enh;
    double v[4]; /* It's just a vector */
    XYZ  xyz;
    UVW  uvw;
    LPZ  lpz;
    XY   xy;
    UV   uv;
    LP   lp;
};

union PJ_TRIPLET {
    PJ_OPK  opk;
    PJ_ENH  enh;
    PJ_EZN  ezn;
    PJ_DMS  dms;
    double v[3]; /* It's just a vector */
    XYZ    xyz;
    LPZ    lpz;
    UVW    uvw;
    XY     xy;
    LP     lp;
    UV     uv;
    PJ_AF  af;
};

union PJ_PAIR {
    XY     xy;
    LP     lp;
    UV     uv;
    PJ_AF  af;
    double v[2]; /* Yes - It's really just a vector! */
};

struct PJ_OBS {
    PJ_COORD coo;        /* coordinate data */
    PJ_TRIPLET anc;      /* ancillary data */
    int id;              /* integer ancillary data - e.g. observation number, EPSG code... */
    unsigned int flags;  /* additional data, intended for flags */
};

/* The context type - properly namespaced synonym for projCtx */
struct projCtx_t;
typedef struct projCtx_t PJ_CONTEXT;
typedef int *PJ_FILE;


/* Manage the transformation definition object PJ */
PJ  *pj_create (const char *definition);
PJ  *pj_create_argv (int argc, char **argv);
void pj_free (PJ *P);
int  pj_error (PJ *P);


/* Apply transformation to observation - in forward or inverse direction */
enum pj_direction {
    PJ_FWD   =  1,   /* Forward    */
    PJ_IDENT =  0,   /* Do nothing */
    PJ_INV   = -1    /* Inverse    */
};
PJ_OBS pj_trans (PJ *P, enum pj_direction direction, PJ_OBS obs);

/* Measure internal consistency - in forward or inverse direction */
double pj_roundtrip (PJ *P, enum pj_direction direction, int n, PJ_OBS obs);

/* Geodesic distance between two points with angular 2D coordinates */
double pj_lp_dist (PJ *P, LP a, LP b);

/* Euclidean distance between two points with linear 2D coordinates */
double pj_xy_dist (XY a, XY b);

/* Euclidean distance between two points with linear 3D coordinates */
double pj_xyz_dist (XYZ a, XYZ b);


#ifndef PJ_OBS_API_C
/* Part of MSVC workaround: Make pj_*_null look function-like for symmetry with pj_*_error */
#define pj_coo_null(x) pj_coo_null
#define pj_obs_null(x) pj_obs_null
extern const PJ_COORD pj_coo_null;
extern const PJ_OBS   pj_obs_null;
extern const PJ      *pj_shutdown;
#endif

#ifndef TODEG
#define TODEG(rad)  ((rad)*180.0/M_PI)
#endif
#ifndef TORAD
#define TORAD(deg)  ((deg)*M_PI/180.0)
#endif







/* High level functionality for handling thread contexts */
enum pj_log_level {
    PJ_LOG_NONE  = 0,
    PJ_LOG_ERROR = 1,
    PJ_LOG_DEBUG = 2,
    PJ_LOG_TRACE = 3,
    PJ_LOG_TELL  = 4,
    PJ_LOG_DEBUG_MAJOR = 2, /* for proj_api.h compatibility */
    PJ_LOG_DEBUG_MINOR = 3  /* for proj_api.h compatibility */
};

/* Set or read error level */
#define PJ_ERR_TELL -56789
int pj_err_level (PJ *P, int err_level);

/* Set logging level 0-3. Higher number means more debug info. 0 turns it off */
enum pj_log_level pj_log_level (PJ *P, enum pj_log_level log_level);

void pj_log_error (PJ *P, const char *fmt, ...);
void pj_log_debug (PJ *P, const char *fmt, ...);
void pj_log_trace (PJ *P, const char *fmt, ...);
void pj_log_func (PJ *P, void *app_data, void (*log)(void *, int, const char *));


/* Lower level functionality for handling thread contexts */
int  pj_context_renew (PJ *P);
void pj_context_inherit (PJ *mother, PJ *daughter);
void pj_context_free    (const PJ *P);

/* Lowest level: Minimum support for fileapi */
void pj_fileapi_set (PJ *P, void *fileapi);


#ifdef __cplusplus
}
#endif

#endif /* ndef PROJ_H */
