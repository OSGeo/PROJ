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
 *           introduces the PJ_OBSERVATION data type, for generic coordinate and
 *           handling of ancillary data.
 *
 *           It includes the PJ_SPATIOTEMPORAL and PJ_TRIPLET unions
 *           making it possible to make explicit the previously used
 *           "implicit type punning", where a XY is turned into a LP by
 *           re#defining both as UV, behind the back of the user.
 *
 *           The bare essentials API presented here follows the PROJ.4
 *           convention of sailing the coordinate to be reprojected, up on
 *           the stack ("call by value"), and symmetrically returning the
 *           result on the stack. Although the PJ_OBSERVATION object is 4 times
 *           as large as the traditional XY and LP objects, timing results
 *           have shown the overhead to be very reasonable.
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

/******************************************************************************
    proj.h is included by projects.h in order to determine the size of the
    PJ_OBSERVATION object. When included by projects.h, inclusion guards
    ensure that only a minimal implementation being the same size as the fully
    defined one, but stomping as little as possible on the traditional proj.4
    name space by excluding the details.

    The PJ_OBSERVATION object is fully defined if proj.h is included alone or
    in connection with *but before* projects.h (the latter may be needed in
    some cases, where it is necessary to access this "bare essentials" API,
    while still having direct access to PJ object internals)
******************************************************************************/
#ifdef PROJECTS_H_ATEND
#error "proj.h must be included before projects.h"
#endif
#ifdef PROJ_API_H_ATEND
#error "proj.h must be included before proj_api.h"
#endif



#ifdef PROJECTS_H
#ifndef PROJ_H
#ifndef PROJ_H_MINIMAL
#define PROJ_H_MINIMAL
#ifdef __cplusplus
extern "C" {
#endif
/* Minimum version to let projects.h know the sizeof(PJ_OBSERVATION) */
union PJ_SPATIOTEMPORAL { double u[4]; double v[4]; };
union PJ_TRIPLET        { double u[3]; double v[3]; };

typedef union PJ_SPATIOTEMPORAL PJ_SPATIOTEMPORAL;
typedef union PJ_TRIPLET PJ_TRIPLET;

struct PJ_OBSERVATION {
    PJ_SPATIOTEMPORAL coo;  /* coordinate data */
    PJ_TRIPLET anc;         /* ancillary data */
    int id;                 /* integer ancillary data - e.g. observation number, EPSG code... */
    unsigned int flags;     /* additional data, intended for flags */
};

typedef struct PJ_OBSERVATION PJ_OBSERVATION;
#ifdef __cplusplus
}
#endif
#endif /* ndef PROJ_H_MINIMAL */
#endif /* ndef PROJ_H */
#endif /* def PROJECTS_H */






#ifndef PROJECTS_H
#ifndef PROJ_H
#define PROJ_H
#ifdef __cplusplus
extern "C" {
#endif


/* Need access to PJ_VERSION */
#define PROJ_API_INCLUDED_FOR_PJ_VERSION_ONLY
#include <proj_api.h>
#undef  PROJ_API_INCLUDED_FOR_PJ_VERSION_ONLY


/* first forward declare everything needed */

/* Data type for generic geodetic observations */
struct PJ_OBSERVATION;
typedef struct PJ_OBSERVATION PJ_OBSERVATION;

/* Data type for generic geodetic 3D data */
union PJ_TRIPLET;
typedef union PJ_TRIPLET PJ_TRIPLET;

/* Data type for generic geodetic 3D data plus epoch information */
union PJ_SPATIOTEMPORAL;
typedef union PJ_SPATIOTEMPORAL PJ_SPATIOTEMPORAL;

/* Data type for projection/transformation information */
struct PJconsts;
typedef struct PJconsts PJ;         /* the PJ object herself */

/* The context type - properly namespaced synonym for projCtx */
struct projCtx_t;
typedef struct projCtx_t PJ_CONTEXT;


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

union PJ_SPATIOTEMPORAL {
    PJ_XYZT xyzt;
    PJ_UVWT uvwt;
    PJ_ENHT enht;
    PJ_LPZT lpzt;
    PJ_ENH  enh;
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
    PJ_OPK  opk;
    PJ_ENH  enh;
    PJ_EZN  ezn;
    PJ_AF   af;
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

/* Direction: "+" forward, "-" reverse, 0: do nothing */
enum pj_direction {
    PJ_FWD   =  1,
    PJ_IDENT =  0,
    PJ_INV   = -1
};

/* Apply transformation to observation - in forward or inverse direction */
PJ_OBSERVATION pj_apply (PJ *P, enum pj_direction direction, PJ_OBSERVATION obs);

/* Measure internal consistency - in forward or inverse direction */
double pj_roundtrip(PJ *P, enum pj_direction direction, int n, PJ_OBSERVATION obs);

int pj_show_triplet (FILE *stream, const char *banner, PJ_TRIPLET point);

PJ *pj_create (PJ_CONTEXT *ctx, const char *definition);
PJ *pj_create_argv (PJ_CONTEXT *ctx, int argc, char **argv);
void pj_free(PJ *P);

#ifndef PJ_OBSERVATION_C
extern const PJ_OBSERVATION pj_observation_error;
extern const PJ_OBSERVATION pj_observation_null;
#endif

#ifndef TODEG
#define TODEG(rad)  ((rad)*180.0/M_PI)
#endif
#ifndef TORAD
#define TORAD(deg)  ((deg)*M_PI/180.0)
#endif




/* This is mostly a direct copy of (parts of) the proj_api header, but with cleaned up name space */
PJ_CONTEXT *pj_ctx (PJ *P);
PJ_CONTEXT *pj_ctx_alloc(void);

void  pj_set_ctx (PJ *, PJ_CONTEXT *);
void  pj_ctx_free (PJ_CONTEXT *);
int   pj_ctx_get_errno (PJ_CONTEXT *);
void  pj_ctx_set_errno (PJ_CONTEXT *, int);
void  pj_ctx_set_debug (PJ_CONTEXT *, int);
void  pj_ctx_set_logger (PJ_CONTEXT *, void (*)(void *, int, const char *));
void  pj_ctx_set_app_data (PJ_CONTEXT *, void * );
void *pj_ctx_get_app_data (PJ_CONTEXT *);

/* file reading api, like stdio */
typedef int *PJ_FILE;
struct pj_fileapi;
typedef struct pj_fileapi PJ_FILEAPI;
struct pj_fileapi {
    PJ_FILE  (*fopen)(PJ_CONTEXT *ctx, const char *filename, const char *access);
    size_t   (*fread)(void *buffer, size_t size, size_t nmemb, PJ_FILE *file);
    int      (*fseek)(PJ_FILE *file, long offset, int whence);
    long     (*ftell)(PJ_FILE *file);
    void     (*fclose)(PJ_FILE *file);
};

void        pj_ctx_set_fileapi (PJ_CONTEXT *, PJ_FILEAPI *);
PJ_FILEAPI *pj_ctx_get_fileapi (PJ_CONTEXT *);
PJ_FILEAPI *pj_get_default_fileapi();

PJ_FILE  pj_ctx_fopen  (PJ_CONTEXT *ctx, const char *filename, const char *access);
size_t   pj_ctx_fread  (PJ_CONTEXT *ctx, void *buffer, size_t size, size_t nmemb, PJ_FILE file);
int      pj_ctx_fseek  (PJ_CONTEXT *ctx, PJ_FILE *file, long offset, int whence);
long     pj_ctx_ftell  (PJ_CONTEXT *ctx, PJ_FILE *file);
void     pj_ctx_fclose (PJ_CONTEXT *ctx, PJ_FILE *file);
char    *pj_ctx_fgets  (PJ_CONTEXT *ctx, char *line, int size, PJ_FILE *file);

PJ_FILE  pj_open_lib(PJ_CONTEXT *, const char *, const char *);


/* Logging API */
void pj_log (PJ_CONTEXT *ctx, int level, const char *fmt, ... );
void pj_stderr_logger (void *, int, const char *);
#define PJ_LOG_NONE        0
#define PJ_LOG_ERROR       1
#define PJ_LOG_DEBUG_MAJOR 2
#define PJ_LOG_DEBUG_MINOR 3


#ifdef __cplusplus
}
#endif

#ifndef PROJ_H_ATEND
#define PROJ_H_ATEND
#endif


#endif /* ndef PROJ_H */
#endif /* ndef PROJECTS_H */
