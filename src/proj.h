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
 *           Hence, all symbols exposed are being moved to the proj_ namespace,
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
 *           renamed struct pj_ctx some day, but for backwards compatibility
 *           it remains as-is for now).
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
 *           Contexts and thread safety
 *           --------------------------
 *
 *           After a year of experiments (and previous experience from the
 *           trlib transformation library) it has become clear that the
 *           context subsystem is unavoidable in a multi-threaded world.
 *           Hence, instead of hiding it away, we move it into the limelight,
 *           highly recommending (but not formally requiring) the bracketing
 *           with calls to proj_context_create(...)/proj_context_destroy() of
 *           any code block calling PROJ.4 functions.
 *
 *           Legacy single threaded code need not do anything, but *may*
 *           implement a bit of future compatibility by using the backward
 *           compatible call proj_context_create(0), which will not create
 *           a new context, but simply provide a pointer to the default one.
 *
 *           See pj_obs_api_test.c for an example of how to use the API.
 *
 * Author:   Thomas Knudsen, <thokn@sdfe.dk>
 *
 ******************************************************************************
 * Copyright (c) 2016, 2017, Thomas Knudsen / SDFE
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
#include <stdarg.h>
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

/* easting and northing */
typedef struct { double   e, n; }  PJ_EN;

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
    PJ_EN   en;
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
    PJ_EN  en;
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



/* A P I */


/* Functionality for handling thread contexts */
PJ_CONTEXT *proj_context_create (int multithreaded);
void proj_context_set (PJ *P, PJ_CONTEXT *ctx);
void proj_context_inherit (PJ *parent, PJ *child);
void proj_context_destroy    (PJ_CONTEXT *ctx);


/* Manage the transformation definition object PJ */
PJ  *proj_create (PJ_CONTEXT *ctx, const char *definition);
PJ  *proj_create_argv (PJ_CONTEXT *ctx, int argc, char **argv);
PJ  *proj_create_crs_to_crs (PJ_CONTEXT *ctx, const char *def_from, const char *def_to);
PJ  *proj_destroy (PJ *P);


/* Apply transformation to observation - in forward or inverse direction */
enum proj_direction {
    PJ_FWD   =  1,   /* Forward    */
    PJ_IDENT =  0,   /* Do nothing */
    PJ_INV   = -1    /* Inverse    */
};

enum proj_trans_batch_const_arrays {
    PJ_X_CONST = 1,
    PJ_Y_CONST = 2,
    PJ_Z_CONST = 4,
    PJ_T_CONST = 8
};

PJ_OBS   proj_trans_obs   (PJ *P, enum proj_direction direction, PJ_OBS obs);
PJ_COORD proj_trans_coord (PJ *P, enum proj_direction direction, PJ_COORD coord);

int proj_trans_batch (
    PJ *P, enum proj_direction direction, 
    size_t n, size_t stride,
    unsigned int flags,
    double *x, double *y, double *z, double *t
);

int proj_trans_obs_batch   (PJ *P, enum proj_direction direction, size_t n, PJ_OBS *obs);
int proj_trans_coord_batch (PJ *P, enum proj_direction direction, size_t n, PJ_COORD *coord);

/* these are not constructors, but initializers */
PJ_COORD proj_coord (double x, double y, double z, double t);
PJ_OBS   proj_obs   (double x, double y, double z, double t, double o, double p, double k, int id, unsigned int flags);
PJ_COORD proj_coord_error (void);
PJ_OBS   proj_obs_error (void);


/* Measure internal consistency - in forward or inverse direction */
double proj_roundtrip (PJ *P, enum proj_direction direction, int n, PJ_OBS obs);

/* Geodesic distance between two points with angular 2D coordinates */
double proj_lp_dist (PJ *P, LP a, LP b);

/* Euclidean distance between two points with linear 2D coordinates */
double proj_xy_dist (XY a, XY b);

/* Euclidean distance between two points with linear 3D coordinates */
double proj_xyz_dist (XYZ a, XYZ b);


#ifndef PJ_OBS_API_C
extern const PJ_COORD proj_coord_null;
extern const PJ_OBS   proj_obs_null;
extern const PJ      *proj_shutdown;
#endif
/* Part of MSVC workaround: Make proj_*_null look function-like for symmetry with proj_*_error */
#define proj_coord_null(x) proj_coord_null
#define proj_obs_null(x) proj_obs_null


#ifndef PJ_TODEG
#define PJ_TODEG(rad)  ((rad)*180.0/M_PI)
#endif
#ifndef PJ_TORAD
#define PJ_TORAD(deg)  ((deg)*M_PI/180.0)
#endif


/* Set or read error level */
int  proj_errno (PJ *P);
void proj_errno_set (PJ *P, int err);
void proj_context_errno_set (PJ_CONTEXT *ctx, int err);






/* stuff below is *not* considered API, and will be moved to an "internal plumbing toolset" */

/* High level functionality for handling thread contexts */
enum proj_log_level {
    PJ_LOG_NONE  = 0,
    PJ_LOG_ERROR = 1,
    PJ_LOG_DEBUG = 2,
    PJ_LOG_TRACE = 3,
    PJ_LOG_TELL  = 4,
    PJ_LOG_DEBUG_MAJOR = 2, /* for proj_api.h compatibility */
    PJ_LOG_DEBUG_MINOR = 3  /* for proj_api.h compatibility */
};

/* Set logging level 0-3. Higher number means more debug info. 0 turns it off */
enum proj_log_level proj_log_level (PJ_CONTEXT *ctx, enum proj_log_level log_level);
typedef void (*PJ_LOG_FUNCTION)(void *, int, const char *);

void proj_log_error (PJ *P, const char *fmt, ...);
void proj_log_debug (PJ *P, const char *fmt, ...);
void proj_log_trace (PJ *P, const char *fmt, ...);
/*void proj_log_func (PJ *P, void *app_data, void (*log)(void *, int, const char *));*/
void proj_log_func (PJ_CONTEXT *ctx, void *app_data, PJ_LOG_FUNCTION log);


/* Lowest level: Minimum support for fileapi */
void proj_fileapi_set (PJ *P, void *fileapi);


#ifdef __cplusplus
}
#endif

#endif /* ndef PROJ_H */
