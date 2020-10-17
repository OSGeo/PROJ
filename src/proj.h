/******************************************************************************
 * Project:  PROJ.4
 * Purpose:  Revised, experimental API for PROJ.4, intended as the foundation
 *           for added geodetic functionality.
 *
 *           The original proj API (defined previously in projects.h) has grown
 *           organically over the years, but it has also grown somewhat messy.
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
 *           introduces the PJ_COORD union data type, for generic 4D coordinate
 *           handling.
 *
 *           PJ_COORD makes it possible to make explicit the previously used
 *           "implicit type punning", where a XY is turned into a LP by
 *           re#defining both as UV, behind the back of the user.
 *
 *           The PJ_COORD union is used for storing 1D, 2D, 3D and 4D coordinates.
 *
 *           The bare essentials API presented here follows the PROJ.4
 *           convention of sailing the coordinate to be reprojected, up on
 *           the stack ("call by value"), and symmetrically returning the
 *           result on the stack. Although the PJ_COORD object is twice as large
 *           as the traditional XY and LP objects, timing results have shown the
 *           overhead to be very reasonable.
 *
 *           Contexts and thread safety
 *           --------------------------
 *
 *           After a year of experiments (and previous experience from the
 *           trlib transformation library) it has become clear that the
 *           context subsystem is unavoidable in a multi-threaded world.
 *           Hence, instead of hiding it away, we move it into the limelight,
 *           highly recommending (but not formally requiring) the bracketing
 *           of any code block calling PROJ.4 functions with calls to
 *           proj_context_create(...)/proj_context_destroy()
 *
 *           Legacy single threaded code need not do anything, but *may*
 *           implement a bit of future compatibility by using the backward
 *           compatible call proj_context_create(0), which will not create
 *           a new context, but simply provide a pointer to the default one.
 *
 *           See proj_4D_api_test.c for examples of how to use the API.
 *
 * Author:   Thomas Knudsen, <thokn@sdfe.dk>
 *           Benefitting from a large number of comments and suggestions
 *           by (primarily) Kristian Evers and Even Rouault.
 *
 ******************************************************************************
 * Copyright (c) 2016, 2017, Thomas Knudsen / SDFE
 * Copyright (c) 2018, Even Rouault
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

#ifndef PROJ_H
#define PROJ_H

#include <stddef.h>  /* For size_t */

#ifdef PROJ_API_H
#error proj.h must be included before proj_api.h
#endif

#ifdef PROJ_RENAME_SYMBOLS
#include "proj_symbol_rename.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \file proj.h
 *
 * C API new generation
 */

/*! @cond Doxygen_Suppress */

#ifndef PROJ_DLL
#ifdef PROJ_MSVC_DLL_EXPORT
#define PROJ_DLL __declspec(dllexport)
#elif defined(PROJ_MSVC_DLL_IMPORT)
#define PROJ_DLL __declspec(dllimport)
#elif defined(__GNUC__)
#define PROJ_DLL __attribute__ ((visibility("default")))
#else
#define PROJ_DLL
#endif
#endif

#ifdef PROJ_SUPPRESS_DEPRECATION_MESSAGE
  #define PROJ_DEPRECATED(decl, msg)            decl
#elif defined(__has_extension)
  #if __has_extension(attribute_deprecated_with_message)
    #define PROJ_DEPRECATED(decl, msg)          decl __attribute__ ((deprecated(msg)))
  #elif defined(__GNUC__)
    #define PROJ_DEPRECATED(decl, msg)          decl __attribute__ ((deprecated))
  #else
    #define PROJ_DEPRECATED(decl, msg)          decl
  #endif
#elif defined(__GNUC__)
  #define PROJ_DEPRECATED(decl, msg)            decl __attribute__ ((deprecated))
#elif defined(_MSVC_VER)
  #define PROJ_DEPRECATED(decl, msg)            __declspec(deprecated(msg)) decl
#else
  #define PROJ_DEPRECATED(decl, msg)            decl
#endif

/* The version numbers should be updated with every release! **/
#define PROJ_VERSION_MAJOR 7
#define PROJ_VERSION_MINOR 2
#define PROJ_VERSION_PATCH 0

extern char const PROJ_DLL pj_release[]; /* global release id string */

/* first forward declare everything needed */

/* Data type for generic geodetic 3D data plus epoch information */
union PJ_COORD;
typedef union PJ_COORD PJ_COORD;

struct PJ_AREA;
typedef struct PJ_AREA PJ_AREA;

struct P5_FACTORS {                  /* Common designation */
    double meridional_scale;               /* h */
    double parallel_scale;                 /* k */
    double areal_scale;                    /* s */

    double angular_distortion;             /* omega */
    double meridian_parallel_angle;        /* theta-prime */
    double meridian_convergence;           /* alpha */

    double tissot_semimajor;               /* a */
    double tissot_semiminor;               /* b */

    double dx_dlam, dx_dphi;
    double dy_dlam, dy_dphi;
};
typedef struct P5_FACTORS PJ_FACTORS;

/* Data type for projection/transformation information */
struct PJconsts;
typedef struct PJconsts PJ;         /* the PJ object herself */

/* Data type for library level information */
struct PJ_INFO;
typedef struct PJ_INFO PJ_INFO;

struct PJ_PROJ_INFO;
typedef struct PJ_PROJ_INFO PJ_PROJ_INFO;

struct PJ_GRID_INFO;
typedef struct PJ_GRID_INFO PJ_GRID_INFO;

struct PJ_INIT_INFO;
typedef struct PJ_INIT_INFO PJ_INIT_INFO;

/* Data types for list of operations, ellipsoids, datums and units used in PROJ.4 */
struct PJ_LIST {
    const char  *id;                /* projection keyword */
    PJ          *(*proj)(PJ *);     /* projection entry point */
    const char  * const *descr;     /* description text */
};

typedef struct PJ_LIST PJ_OPERATIONS;

struct PJ_ELLPS {
    const char  *id;    /* ellipse keyword name */
    const char  *major; /* a= value */
    const char  *ell;   /* elliptical parameter */
    const char  *name;  /* comments */
};
typedef struct PJ_ELLPS PJ_ELLPS;

struct PJ_UNITS {
    const char  *id;        /* units keyword */
    const char  *to_meter;  /* multiply by value to get meters */
    const char  *name;      /* comments */
    double      factor;     /* to_meter factor in actual numbers */
};
typedef struct PJ_UNITS PJ_UNITS;

struct PJ_PRIME_MERIDIANS {
    const char  *id;        /* prime meridian keyword */
    const char  *defn;      /* offset from greenwich in DMS format. */
};
typedef struct PJ_PRIME_MERIDIANS PJ_PRIME_MERIDIANS;


/* Geodetic, mostly spatiotemporal coordinate types */
typedef struct { double   x,   y,  z, t; }  PJ_XYZT;
typedef struct { double   u,   v,  w, t; }  PJ_UVWT;
typedef struct { double lam, phi,  z, t; }  PJ_LPZT;
typedef struct { double o, p, k; }          PJ_OPK;  /* Rotations: omega, phi, kappa */
typedef struct { double e, n, u; }          PJ_ENU;  /* East, North, Up */
typedef struct { double s, a1, a2; }        PJ_GEOD; /* Geodesic length, fwd azi, rev azi */

/* Classic proj.4 pair/triplet types - moved into the PJ_ name space */
typedef struct { double   u,   v; }  PJ_UV;
typedef struct { double   x,   y; }  PJ_XY;
typedef struct { double lam, phi; }  PJ_LP;

typedef struct { double   x,   y,  z; }  PJ_XYZ;
typedef struct { double   u,   v,  w; }  PJ_UVW;
typedef struct { double lam, phi,  z; }  PJ_LPZ;


/* Avoid preprocessor renaming and implicit type-punning: Use a union to make it explicit */
union PJ_COORD {
     double v[4];   /* First and foremost, it really is "just 4 numbers in a vector" */
    PJ_XYZT xyzt;
    PJ_UVWT uvwt;
    PJ_LPZT lpzt;
    PJ_GEOD geod;
     PJ_OPK opk;
     PJ_ENU enu;
     PJ_XYZ xyz;
     PJ_UVW uvw;
     PJ_LPZ lpz;
      PJ_XY xy;
      PJ_UV uv;
      PJ_LP lp;
};


struct PJ_INFO {
    int         major;              /* Major release number                 */
    int         minor;              /* Minor release number                 */
    int         patch;              /* Patch level                          */
    const char  *release;           /* Release info. Version + date         */
    const char  *version;           /* Full version number                  */
    const char  *searchpath;        /* Paths where init and grid files are  */
                                    /* looked for. Paths are separated by   */
                                    /* semi-colons on Windows, and colons   */
                                    /* on non-Windows platforms.            */
    const char * const *paths;
    size_t path_count;
};

struct PJ_PROJ_INFO {
    const char  *id;                /* Name of the projection in question                       */
    const char  *description;       /* Description of the projection                            */
    const char  *definition;        /* Projection definition                                    */
    int         has_inverse;        /* 1 if an inverse mapping exists, 0 otherwise              */
    double      accuracy;           /* Expected accuracy of the transformation. -1 if unknown.  */
};

struct PJ_GRID_INFO {
    char        gridname[32];       /* name of grid                         */
    char        filename[260];      /* full path to grid                    */
    char        format[8];          /* file format of grid                  */
    PJ_LP       lowerleft;          /* Coordinates of lower left corner     */
    PJ_LP       upperright;         /* Coordinates of upper right corner    */
    int         n_lon, n_lat;       /* Grid size                            */
    double      cs_lon, cs_lat;     /* Cell size of grid                    */
};

struct PJ_INIT_INFO {
    char        name[32];           /* name of init file                        */
    char        filename[260];      /* full path to the init file.              */
    char        version[32];        /* version of the init file                 */
    char        origin[32];         /* origin of the file, e.g. EPSG            */
    char        lastupdate[16];     /* Date of last update in YYYY-MM-DD format */
};

typedef enum PJ_LOG_LEVEL {
    PJ_LOG_NONE  = 0,
    PJ_LOG_ERROR = 1,
    PJ_LOG_DEBUG = 2,
    PJ_LOG_TRACE = 3,
    PJ_LOG_TELL  = 4,
    PJ_LOG_DEBUG_MAJOR = 2, /* for proj_api.h compatibility */
    PJ_LOG_DEBUG_MINOR = 3  /* for proj_api.h compatibility */
} PJ_LOG_LEVEL;

typedef void (*PJ_LOG_FUNCTION)(void *, int, const char *);


/* The context type - properly namespaced synonym for projCtx */
struct projCtx_t;
typedef struct projCtx_t PJ_CONTEXT;

/* A P I */

/**
 * The objects returned by the functions defined in this section have minimal
 * interaction with the functions of the
 * \ref iso19111_functions section, and vice versa. See its introduction
 * paragraph for more details.
 */

/* Functionality for handling thread contexts */
#ifdef __cplusplus
#define PJ_DEFAULT_CTX nullptr
#else
#define PJ_DEFAULT_CTX 0
#endif
PJ_CONTEXT PROJ_DLL *proj_context_create (void);
PJ_CONTEXT PROJ_DLL *proj_context_destroy (PJ_CONTEXT *ctx);
PJ_CONTEXT PROJ_DLL *proj_context_clone (PJ_CONTEXT *ctx);

/** Callback to resolve a filename to a full path */
typedef const char* (*proj_file_finder) (PJ_CONTEXT *ctx, const char*, void* user_data);

void PROJ_DLL proj_context_set_file_finder(PJ_CONTEXT *ctx, proj_file_finder finder, void* user_data);
void PROJ_DLL proj_context_set_search_paths(PJ_CONTEXT *ctx, int count_paths, const char* const* paths);
void PROJ_DLL proj_context_set_ca_bundle_path(PJ_CONTEXT *ctx, const char *path);
void PROJ_DLL proj_context_use_proj4_init_rules(PJ_CONTEXT *ctx, int enable);
int PROJ_DLL proj_context_get_use_proj4_init_rules(PJ_CONTEXT *ctx, int from_legacy_code_path);

/*! @endcond */

/** Opaque structure for PROJ for a file handle. Implementations might cast it to their
 * structure/class of choice. */
typedef struct PROJ_FILE_HANDLE PROJ_FILE_HANDLE;

/** Open access / mode */
typedef enum PROJ_OPEN_ACCESS
{
    /** Read-only access. Equivalent to "rb" */
    PROJ_OPEN_ACCESS_READ_ONLY,

    /** Read-update access. File should be created if not existing. Equivalent to "r+b" */
    PROJ_OPEN_ACCESS_READ_UPDATE,

    /** Create access. File should be truncated to 0-byte if already existing. Equivalent to "w+b" */
    PROJ_OPEN_ACCESS_CREATE
} PROJ_OPEN_ACCESS;

/** File API callbacks */
typedef struct PROJ_FILE_API
{
    /** Version of this structure. Should be set to 1 currently. */
    int version;

    /** Open file. Return NULL if error */
    PROJ_FILE_HANDLE* (*open_cbk)(PJ_CONTEXT *ctx, const char *filename, PROJ_OPEN_ACCESS access, void* user_data);

    /** Read sizeBytes into buffer from current position and return number of bytes read */
    size_t            (*read_cbk)(PJ_CONTEXT *ctx, PROJ_FILE_HANDLE*, void* buffer, size_t sizeBytes, void* user_data);

    /** Write sizeBytes into buffer from current position and return number of bytes written */
    size_t            (*write_cbk)(PJ_CONTEXT *ctx, PROJ_FILE_HANDLE*, const void* buffer, size_t sizeBytes, void* user_data);

    /** Seek to offset using whence=SEEK_SET/SEEK_CUR/SEEK_END. Return TRUE in case of success */
    int               (*seek_cbk)(PJ_CONTEXT *ctx, PROJ_FILE_HANDLE*, long long offset, int whence, void* user_data);

    /** Return current file position */
    unsigned long long (*tell_cbk)(PJ_CONTEXT *ctx, PROJ_FILE_HANDLE*, void* user_data);

    /** Close file */
    void              (*close_cbk)(PJ_CONTEXT *ctx, PROJ_FILE_HANDLE*, void* user_data);

    /** Return TRUE if a file exists */
    int (*exists_cbk)(PJ_CONTEXT *ctx, const char *filename, void* user_data);

    /** Return TRUE if directory exists or could be created  */
    int (*mkdir_cbk)(PJ_CONTEXT *ctx, const char *filename, void* user_data);

    /** Return TRUE if file could be removed  */
    int (*unlink_cbk)(PJ_CONTEXT *ctx, const char *filename, void* user_data);

    /** Return TRUE if file could be renamed  */
    int (*rename_cbk)(PJ_CONTEXT *ctx, const char *oldPath, const char *newPath, void* user_data);
} PROJ_FILE_API;

int PROJ_DLL proj_context_set_fileapi(
    PJ_CONTEXT* ctx, const PROJ_FILE_API* fileapi, void* user_data);

void PROJ_DLL proj_context_set_sqlite3_vfs_name(PJ_CONTEXT* ctx, const char* name);

/** Opaque structure for PROJ for a network handle. Implementations might cast it to their
 * structure/class of choice. */
typedef struct PROJ_NETWORK_HANDLE PROJ_NETWORK_HANDLE;

/** Network access: open callback
 * 
 * Should try to read the size_to_read first bytes at the specified offset of
 * the file given by URL url,
 * and write them to buffer. *out_size_read should be updated with the actual
 * amount of bytes read (== size_to_read if the file is larger than size_to_read).
 * During this read, the implementation should make sure to store the HTTP
 * headers from the server response to be able to respond to
 * proj_network_get_header_value_cbk_type callback.
 *
 * error_string_max_size should be the maximum size that can be written into
 * the out_error_string buffer (including terminating nul character).
 *
 * @return a non-NULL opaque handle in case of success.
 */
typedef PROJ_NETWORK_HANDLE* (*proj_network_open_cbk_type)(
                                                      PJ_CONTEXT* ctx,
                                                      const char* url,
                                                      unsigned long long offset,
                                                      size_t size_to_read,
                                                      void* buffer,
                                                      size_t* out_size_read,
                                                      size_t error_string_max_size,
                                                      char* out_error_string,
                                                      void* user_data);

/** Network access: close callback */
typedef void (*proj_network_close_cbk_type)(PJ_CONTEXT* ctx,
                                            PROJ_NETWORK_HANDLE* handle,
                                            void* user_data);

/** Network access: get HTTP headers */
typedef const char* (*proj_network_get_header_value_cbk_type)(
                                            PJ_CONTEXT* ctx,
                                            PROJ_NETWORK_HANDLE* handle,
                                            const char* header_name,
                                            void* user_data);

/** Network access: read range
 *
 * Read size_to_read bytes from handle, starting at offset, into
 * buffer.
 * During this read, the implementation should make sure to store the HTTP
 * headers from the server response to be able to respond to
 * proj_network_get_header_value_cbk_type callback.
 *
 * error_string_max_size should be the maximum size that can be written into
 * the out_error_string buffer (including terminating nul character).
 *
 * @return the number of bytes actually read (0 in case of error)
 */
typedef size_t (*proj_network_read_range_type)(
                                            PJ_CONTEXT* ctx,
                                            PROJ_NETWORK_HANDLE* handle,
                                            unsigned long long offset,
                                            size_t size_to_read,
                                            void* buffer,
                                            size_t error_string_max_size,
                                            char* out_error_string,
                                            void* user_data);

int PROJ_DLL proj_context_set_network_callbacks(
    PJ_CONTEXT* ctx,
    proj_network_open_cbk_type open_cbk,
    proj_network_close_cbk_type close_cbk,
    proj_network_get_header_value_cbk_type get_header_value_cbk,
    proj_network_read_range_type read_range_cbk,
    void* user_data);

int PROJ_DLL proj_context_set_enable_network(PJ_CONTEXT* ctx,
                                             int enabled);

int PROJ_DLL proj_context_is_network_enabled(PJ_CONTEXT* ctx);

void PROJ_DLL proj_context_set_url_endpoint(PJ_CONTEXT* ctx, const char* url);

const char PROJ_DLL *proj_context_get_url_endpoint(PJ_CONTEXT* ctx);

const char PROJ_DLL *proj_context_get_user_writable_directory(PJ_CONTEXT *ctx, int create);

void PROJ_DLL proj_grid_cache_set_enable(PJ_CONTEXT* ctx, int enabled);

void PROJ_DLL proj_grid_cache_set_filename(PJ_CONTEXT* ctx, const char* fullname);

void PROJ_DLL proj_grid_cache_set_max_size(PJ_CONTEXT* ctx, int max_size_MB);

void PROJ_DLL proj_grid_cache_set_ttl(PJ_CONTEXT* ctx, int ttl_seconds);

void PROJ_DLL proj_grid_cache_clear(PJ_CONTEXT* ctx);

int PROJ_DLL proj_is_download_needed(PJ_CONTEXT* ctx,
                                     const char* url_or_filename,
                                     int ignore_ttl_setting);
int PROJ_DLL proj_download_file(PJ_CONTEXT *ctx, const char *url_or_filename,
                                int ignore_ttl_setting,
                                int (*progress_cbk)(PJ_CONTEXT *, double pct,
                                                    void *user_data),
                                void *user_data);

/*! @cond Doxygen_Suppress */

/* Manage the transformation definition object PJ */
PJ PROJ_DLL *proj_create (PJ_CONTEXT *ctx, const char *definition);
PJ PROJ_DLL *proj_create_argv (PJ_CONTEXT *ctx, int argc, char **argv);
PJ PROJ_DLL *proj_create_crs_to_crs(PJ_CONTEXT *ctx, const char *source_crs, const char *target_crs, PJ_AREA *area);
PJ PROJ_DLL *proj_create_crs_to_crs_from_pj(PJ_CONTEXT *ctx,
                                            const PJ *source_crs,
                                            const PJ *target_crs,
                                            PJ_AREA *area,
                                            const char* const *options);
/*! @endcond Doxygen_Suppress */
PJ PROJ_DLL *proj_normalize_for_visualization(PJ_CONTEXT *ctx, const PJ* obj);
/*! @cond Doxygen_Suppress */
void PROJ_DLL proj_assign_context(PJ* pj, PJ_CONTEXT* ctx);
PJ PROJ_DLL *proj_destroy (PJ *P);


PJ_AREA PROJ_DLL *proj_area_create(void);
void PROJ_DLL proj_area_set_bbox(PJ_AREA *area,
                                 double west_lon_degree,
                                 double south_lat_degree,
                                 double east_lon_degree,
                                 double north_lat_degree);
void PROJ_DLL proj_area_destroy(PJ_AREA* area);

/* Apply transformation to observation - in forward or inverse direction */
enum PJ_DIRECTION {
    PJ_FWD   =  1,   /* Forward    */
    PJ_IDENT =  0,   /* Do nothing */
    PJ_INV   = -1    /* Inverse    */
};
typedef enum PJ_DIRECTION PJ_DIRECTION;


int PROJ_DLL proj_angular_input (PJ *P, enum PJ_DIRECTION dir);
int PROJ_DLL proj_angular_output (PJ *P, enum PJ_DIRECTION dir);

int PROJ_DLL proj_degree_input (PJ *P, enum PJ_DIRECTION dir);
int PROJ_DLL proj_degree_output (PJ *P, enum PJ_DIRECTION dir);

PJ_COORD PROJ_DLL proj_trans (PJ *P, PJ_DIRECTION direction, PJ_COORD coord);
int PROJ_DLL proj_trans_array (PJ *P, PJ_DIRECTION direction, size_t n, PJ_COORD *coord);
size_t PROJ_DLL proj_trans_generic (
    PJ *P,
    PJ_DIRECTION direction,
    double *x, size_t sx, size_t nx,
    double *y, size_t sy, size_t ny,
    double *z, size_t sz, size_t nz,
    double *t, size_t st, size_t nt
);


/* Initializers */
PJ_COORD PROJ_DLL proj_coord (double x, double y, double z, double t);

/* Measure internal consistency - in forward or inverse direction */
double PROJ_DLL proj_roundtrip (PJ *P, PJ_DIRECTION direction, int n, PJ_COORD *coord);

/* Geodesic distance between two points with angular 2D coordinates */
double PROJ_DLL proj_lp_dist (const PJ *P, PJ_COORD a, PJ_COORD b);

/* The geodesic distance AND the vertical offset */
double PROJ_DLL proj_lpz_dist (const PJ *P, PJ_COORD a, PJ_COORD b);

/* Euclidean distance between two points with linear 2D coordinates */
double PROJ_DLL proj_xy_dist (PJ_COORD a, PJ_COORD b);

/* Euclidean distance between two points with linear 3D coordinates */
double PROJ_DLL proj_xyz_dist (PJ_COORD a, PJ_COORD b);

/* Geodesic distance (in meter) + fwd and rev azimuth between two points on the ellipsoid */
PJ_COORD PROJ_DLL proj_geod (const PJ *P, PJ_COORD a, PJ_COORD b);


/* Set or read error level */
int  PROJ_DLL proj_context_errno (PJ_CONTEXT *ctx);
int  PROJ_DLL proj_errno (const PJ *P);
int  PROJ_DLL proj_errno_set (const PJ *P, int err);
int  PROJ_DLL proj_errno_reset (const PJ *P);
int  PROJ_DLL proj_errno_restore (const PJ *P, int err);
const char PROJ_DLL * proj_errno_string (int err);

PJ_LOG_LEVEL PROJ_DLL proj_log_level (PJ_CONTEXT *ctx, PJ_LOG_LEVEL log_level);
void PROJ_DLL proj_log_func (PJ_CONTEXT *ctx, void *app_data, PJ_LOG_FUNCTION logf);

/* Scaling and angular distortion factors */
PJ_FACTORS PROJ_DLL proj_factors(PJ *P, PJ_COORD lp);

/* Info functions - get information about various PROJ.4 entities */
PJ_INFO PROJ_DLL proj_info(void);
PJ_PROJ_INFO PROJ_DLL proj_pj_info(PJ *P);
PJ_GRID_INFO PROJ_DLL proj_grid_info(const char *gridname);
PJ_INIT_INFO PROJ_DLL proj_init_info(const char *initname);

/* List functions: */
/* Get lists of operations, ellipsoids, units and prime meridians. */
const PJ_OPERATIONS       PROJ_DLL *proj_list_operations(void);
const PJ_ELLPS            PROJ_DLL *proj_list_ellps(void);
PROJ_DEPRECATED(const PJ_UNITS            PROJ_DLL *proj_list_units(void), "Deprecated by proj_get_units_from_database");
PROJ_DEPRECATED(const PJ_UNITS            PROJ_DLL *proj_list_angular_units(void), "Deprecated by proj_get_units_from_database");
const PJ_PRIME_MERIDIANS  PROJ_DLL *proj_list_prime_meridians(void);

/* These are trivial, and while occasionally useful in real code, primarily here to      */
/* simplify demo code, and in acknowledgement of the proj-internal discrepancy between   */
/* angular units expected by classical proj, and by Charles Karney's geodesics subsystem */
double PROJ_DLL proj_torad (double angle_in_degrees);
double PROJ_DLL proj_todeg (double angle_in_radians);

double PROJ_DLL proj_dmstor(const char *is, char **rs);
char PROJ_DLL * proj_rtodms(char *s, double r, int pos, int neg);

void PROJ_DLL proj_cleanup(void);

/*! @endcond */

/* ------------------------------------------------------------------------- */
/* Binding in C of C++ API */
/* ------------------------------------------------------------------------- */


/** @defgroup iso19111_types Data types for ISO19111 C API
 *  Data types for ISO19111 C API
 *  @{
 */

/** \brief Type representing a NULL terminated list of NULL-terminate strings. */
typedef char **PROJ_STRING_LIST;

/** \brief Guessed WKT "dialect". */
typedef enum
{
    /** \ref WKT2_2019 */
    PJ_GUESSED_WKT2_2019,

    /** Deprecated alias for PJ_GUESSED_WKT2_2019 */
    PJ_GUESSED_WKT2_2018 = PJ_GUESSED_WKT2_2019,

    /** \ref WKT2_2015 */
    PJ_GUESSED_WKT2_2015,

    /** \ref WKT1 */
    PJ_GUESSED_WKT1_GDAL,

    /** ESRI variant of WKT1 */
    PJ_GUESSED_WKT1_ESRI,

    /** Not WKT / unrecognized */
    PJ_GUESSED_NOT_WKT
} PJ_GUESSED_WKT_DIALECT;

/** \brief Object category. */
typedef enum
{
    PJ_CATEGORY_ELLIPSOID,
    PJ_CATEGORY_PRIME_MERIDIAN,
    PJ_CATEGORY_DATUM,
    PJ_CATEGORY_CRS,
    PJ_CATEGORY_COORDINATE_OPERATION
} PJ_CATEGORY;

/** \brief Object type. */
typedef enum
{
    PJ_TYPE_UNKNOWN,

    PJ_TYPE_ELLIPSOID,

    PJ_TYPE_PRIME_MERIDIAN,

    PJ_TYPE_GEODETIC_REFERENCE_FRAME,
    PJ_TYPE_DYNAMIC_GEODETIC_REFERENCE_FRAME,
    PJ_TYPE_VERTICAL_REFERENCE_FRAME,
    PJ_TYPE_DYNAMIC_VERTICAL_REFERENCE_FRAME,
    PJ_TYPE_DATUM_ENSEMBLE,

    /** Abstract type, not returned by proj_get_type() */
    PJ_TYPE_CRS,

    PJ_TYPE_GEODETIC_CRS,
    PJ_TYPE_GEOCENTRIC_CRS,

    /** proj_get_type() will never return that type, but
     * PJ_TYPE_GEOGRAPHIC_2D_CRS or PJ_TYPE_GEOGRAPHIC_3D_CRS. */
    PJ_TYPE_GEOGRAPHIC_CRS,

    PJ_TYPE_GEOGRAPHIC_2D_CRS,
    PJ_TYPE_GEOGRAPHIC_3D_CRS,
    PJ_TYPE_VERTICAL_CRS,
    PJ_TYPE_PROJECTED_CRS,
    PJ_TYPE_COMPOUND_CRS,
    PJ_TYPE_TEMPORAL_CRS,
    PJ_TYPE_ENGINEERING_CRS,
    PJ_TYPE_BOUND_CRS,
    PJ_TYPE_OTHER_CRS,

    PJ_TYPE_CONVERSION,
    PJ_TYPE_TRANSFORMATION,
    PJ_TYPE_CONCATENATED_OPERATION,
    PJ_TYPE_OTHER_COORDINATE_OPERATION,

    PJ_TYPE_TEMPORAL_DATUM,
    PJ_TYPE_ENGINEERING_DATUM,
    PJ_TYPE_PARAMETRIC_DATUM,
} PJ_TYPE;

/** Comparison criterion. */
typedef enum
{
    /** All properties are identical. */
    PJ_COMP_STRICT,

    /** The objects are equivalent for the purpose of coordinate
    * operations. They can differ by the name of their objects,
    * identifiers, other metadata.
    * Parameters may be expressed in different units, provided that the
    * value is (with some tolerance) the same once expressed in a
    * common unit.
    */
    PJ_COMP_EQUIVALENT,

    /** Same as EQUIVALENT, relaxed with an exception that the axis order
    * of the base CRS of a DerivedCRS/ProjectedCRS or the axis order of
    * a GeographicCRS is ignored. Only to be used
    * with DerivedCRS/ProjectedCRS/GeographicCRS */
    PJ_COMP_EQUIVALENT_EXCEPT_AXIS_ORDER_GEOGCRS,
} PJ_COMPARISON_CRITERION;


/** \brief WKT version. */
typedef enum
{
    /** cf osgeo::proj::io::WKTFormatter::Convention::WKT2 */
    PJ_WKT2_2015,
    /** cf osgeo::proj::io::WKTFormatter::Convention::WKT2_SIMPLIFIED */
    PJ_WKT2_2015_SIMPLIFIED,
    /** cf osgeo::proj::io::WKTFormatter::Convention::WKT2_2019 */
    PJ_WKT2_2019,
    /** Deprecated alias for PJ_WKT2_2019 */
    PJ_WKT2_2018 = PJ_WKT2_2019,
    /** cf osgeo::proj::io::WKTFormatter::Convention::WKT2_2019_SIMPLIFIED */
    PJ_WKT2_2019_SIMPLIFIED,
    /** Deprecated alias for PJ_WKT2_2019 */
    PJ_WKT2_2018_SIMPLIFIED = PJ_WKT2_2019_SIMPLIFIED,
    /** cf osgeo::proj::io::WKTFormatter::Convention::WKT1_GDAL */
    PJ_WKT1_GDAL,
    /** cf osgeo::proj::io::WKTFormatter::Convention::WKT1_ESRI */
    PJ_WKT1_ESRI
} PJ_WKT_TYPE;

/** Specify how source and target CRS extent should be used to restrict
  * candidate operations (only taken into account if no explicit area of
  * interest is specified. */
typedef enum
{
    /** Ignore CRS extent */
    PJ_CRS_EXTENT_NONE,

    /** Test coordinate operation extent against both CRS extent. */
    PJ_CRS_EXTENT_BOTH,

    /** Test coordinate operation extent against the intersection of both
        CRS extent. */
    PJ_CRS_EXTENT_INTERSECTION,

    /** Test coordinate operation against the smallest of both CRS extent. */
    PJ_CRS_EXTENT_SMALLEST
} PROJ_CRS_EXTENT_USE;

/** Describe how grid availability is used. */
typedef enum {
    /** Grid availability is only used for sorting results. Operations
        * where some grids are missing will be sorted last. */
    PROJ_GRID_AVAILABILITY_USED_FOR_SORTING,

    /** Completely discard an operation if a required grid is missing. */
    PROJ_GRID_AVAILABILITY_DISCARD_OPERATION_IF_MISSING_GRID,

    /** Ignore grid availability at all. Results will be presented as if
        * all grids were available. */
    PROJ_GRID_AVAILABILITY_IGNORED,

    /** Results will be presented as if grids known to PROJ (that is
    * registered in the grid_alternatives table of its database) were
    * available. Used typically when networking is enabled.
    */
    PROJ_GRID_AVAILABILITY_KNOWN_AVAILABLE,
} PROJ_GRID_AVAILABILITY_USE;

/** \brief PROJ string version. */
typedef enum
{
    /** cf osgeo::proj::io::PROJStringFormatter::Convention::PROJ_5 */
    PJ_PROJ_5,
    /** cf osgeo::proj::io::PROJStringFormatter::Convention::PROJ_4 */
    PJ_PROJ_4
} PJ_PROJ_STRING_TYPE;

/** Spatial criterion to restrict candidate operations. */
typedef enum {
    /** The area of validity of transforms should strictly contain the
        * are of interest. */
    PROJ_SPATIAL_CRITERION_STRICT_CONTAINMENT,

    /** The area of validity of transforms should at least intersect the
        * area of interest. */
    PROJ_SPATIAL_CRITERION_PARTIAL_INTERSECTION
} PROJ_SPATIAL_CRITERION;

    /** Describe if and how intermediate CRS should be used */
typedef enum {
    /** Always search for intermediate CRS. */
    PROJ_INTERMEDIATE_CRS_USE_ALWAYS,

    /** Only attempt looking for intermediate CRS if there is no direct
        * transformation available. */
    PROJ_INTERMEDIATE_CRS_USE_IF_NO_DIRECT_TRANSFORMATION,

    /* Do not attempt looking for intermediate CRS. */
    PROJ_INTERMEDIATE_CRS_USE_NEVER,
} PROJ_INTERMEDIATE_CRS_USE;

/** Type of coordinate system. */
typedef enum
{
    PJ_CS_TYPE_UNKNOWN,

    PJ_CS_TYPE_CARTESIAN,
    PJ_CS_TYPE_ELLIPSOIDAL,
    PJ_CS_TYPE_VERTICAL,
    PJ_CS_TYPE_SPHERICAL,
    PJ_CS_TYPE_ORDINAL,
    PJ_CS_TYPE_PARAMETRIC,
    PJ_CS_TYPE_DATETIMETEMPORAL,
    PJ_CS_TYPE_TEMPORALCOUNT,
    PJ_CS_TYPE_TEMPORALMEASURE
} PJ_COORDINATE_SYSTEM_TYPE;

/** \brief Structure given overall description of a CRS.
 *
 * This structure may grow over time, and should not be directly allocated by
 * client code.
 */
typedef struct
{
    /** Authority name. */
    char* auth_name;
    /** Object code. */
    char* code;
    /** Object name. */
    char* name;
    /** Object type. */
    PJ_TYPE type;
    /** Whether the object is deprecated */
    int deprecated;
    /** Whereas the west_lon_degree, south_lat_degree, east_lon_degree and
     * north_lat_degree fields are valid. */
    int bbox_valid;
    /** Western-most longitude of the area of use, in degrees. */
    double west_lon_degree;
    /** Southern-most latitude of the area of use, in degrees. */
    double south_lat_degree;
    /** Eastern-most longitude of the area of use, in degrees. */
    double east_lon_degree;
    /** Northern-most latitude of the area of use, in degrees. */
    double north_lat_degree;
    /** Name of the area of use. */
    char* area_name;
    /** Name of the projection method for a projected CRS. Might be NULL even
     *for projected CRS in some cases. */
    char* projection_method_name;
} PROJ_CRS_INFO;

/** \brief Structure describing optional parameters for proj_get_crs_list();
 *
 * This structure may grow over time, and should not be directly allocated by
 * client code.
 */
typedef struct
{
    /** Array of allowed object types. Should be NULL if all types are allowed*/
    const PJ_TYPE* types;
    /** Size of types. Should be 0 if all types are allowed*/
    size_t typesCount;

    /** If TRUE and bbox_valid == TRUE, then only CRS whose area of use
     * entirely contains the specified bounding box will be returned.
     * If FALSE and bbox_valid == TRUE, then only CRS whose area of use
     * intersects the specified bounding box will be returned.
     */
    int crs_area_of_use_contains_bbox;
    /** To set to TRUE so that west_lon_degree, south_lat_degree,
     * east_lon_degree and north_lat_degree fields are taken into account. */
    int bbox_valid;
    /** Western-most longitude of the area of use, in degrees. */
    double west_lon_degree;
    /** Southern-most latitude of the area of use, in degrees. */
    double south_lat_degree;
    /** Eastern-most longitude of the area of use, in degrees. */
    double east_lon_degree;
    /** Northern-most latitude of the area of use, in degrees. */
    double north_lat_degree;

    /** Whether deprecated objects are allowed. Default to FALSE. */
    int allow_deprecated;
} PROJ_CRS_LIST_PARAMETERS;

/** \brief Structure given description of a unit.
 *
 * This structure may grow over time, and should not be directly allocated by
 * client code.
 * @since 7.1
 */
typedef struct
{
    /** Authority name. */
    char* auth_name;

    /** Object code. */
    char* code;

    /** Object name. For example "metre", "US survey foot", etc. */
    char* name;

    /** Category of the unit: one of "linear", "linear_per_time", "angular",
     * "angular_per_time", "scale", "scale_per_time" or "time" */
    char* category;

    /** Conversion factor to apply to transform from that unit to the
     * corresponding SI unit (metre for "linear", radian for "angular", etc.).
     * It might be 0 in some cases to indicate no known conversion factor. */
    double conv_factor;

    /** PROJ short name, like "m", "ft", "us-ft", etc... Might be NULL */
    char* proj_short_name;

    /** Whether the object is deprecated */
    int deprecated;
} PROJ_UNIT_INFO;


/**@}*/

/**
 * \defgroup iso19111_functions Binding in C of basic methods from the C++ API
 *  Functions for ISO19111 C API
 *
 * The PJ* objects returned by proj_create_from_wkt(),
 * proj_create_from_database() and other functions in that section
 * will have generally minimal interaction with the functions declared in the
 * upper section of this header file (calling those functions on those objects
 * will either return an error or default/non-sensical values). The exception is
 * for ISO19111 objects of type CoordinateOperation that can be exported as a
 * valid PROJ pipeline. In this case, the PJ objects will work for example with
 * proj_trans_generic().
 * Conversely, objects returned by proj_create() and proj_create_argv(), which
 * are not of type CRS, will return an error when used with functions of this section.
 * @{
 */

/*! @cond Doxygen_Suppress */
typedef struct PJ_OBJ_LIST PJ_OBJ_LIST;
/*! @endcond */

void PROJ_DLL proj_string_list_destroy(PROJ_STRING_LIST list);

void PROJ_DLL proj_context_set_autoclose_database(PJ_CONTEXT *ctx,
                                                  int autoclose);

int PROJ_DLL proj_context_set_database_path(PJ_CONTEXT *ctx,
                                            const char *dbPath,
                                            const char *const *auxDbPaths,
                                            const char* const *options);

const char PROJ_DLL *proj_context_get_database_path(PJ_CONTEXT *ctx);

const char PROJ_DLL *proj_context_get_database_metadata(PJ_CONTEXT* ctx,
                                                        const char* key);


PJ_GUESSED_WKT_DIALECT PROJ_DLL proj_context_guess_wkt_dialect(PJ_CONTEXT *ctx,
                                                               const char *wkt);

PJ PROJ_DLL *proj_create_from_wkt(PJ_CONTEXT *ctx, const char *wkt,
                                          const char* const *options,
                                          PROJ_STRING_LIST *out_warnings,
                                          PROJ_STRING_LIST *out_grammar_errors);

PJ PROJ_DLL *proj_create_from_database(PJ_CONTEXT *ctx,
                                               const char *auth_name,
                                               const char *code,
                                               PJ_CATEGORY category,
                                               int usePROJAlternativeGridNames,
                                               const char* const *options);

int PROJ_DLL proj_uom_get_info_from_database(PJ_CONTEXT *ctx,
                               const char *auth_name,
                               const char *code,
                               const char **out_name,
                               double *out_conv_factor,
                               const char **out_category);

int PROJ_DLL proj_grid_get_info_from_database(PJ_CONTEXT *ctx,
                               const char *grid_name,
                               const char **out_full_name,
                               const char **out_package_name,
                               const char **out_url,
                               int *out_direct_download,
                               int *out_open_license,
                               int *out_available);

PJ PROJ_DLL *proj_clone(PJ_CONTEXT *ctx, const PJ *obj);

PJ_OBJ_LIST PROJ_DLL *proj_create_from_name(PJ_CONTEXT *ctx,
                                                const char *auth_name,
                                                const char *searchedName,
                                                const PJ_TYPE* types,
                                                size_t typesCount,
                                                int approximateMatch,
                                                size_t limitResultCount,
                                                const char* const *options);

PJ_TYPE PROJ_DLL proj_get_type(const PJ *obj);

int PROJ_DLL proj_is_deprecated(const PJ *obj);

PJ_OBJ_LIST PROJ_DLL *proj_get_non_deprecated(PJ_CONTEXT *ctx,
                                                  const PJ *obj);

int PROJ_DLL proj_is_equivalent_to(const PJ *obj, const PJ *other,
                                       PJ_COMPARISON_CRITERION criterion);

int PROJ_DLL proj_is_equivalent_to_with_ctx(PJ_CONTEXT *ctx,
                                            const PJ *obj, const PJ *other,
                                            PJ_COMPARISON_CRITERION criterion);

int PROJ_DLL proj_is_crs(const PJ *obj);

const char PROJ_DLL* proj_get_name(const PJ *obj);

const char PROJ_DLL* proj_get_id_auth_name(const PJ *obj, int index);

const char PROJ_DLL* proj_get_id_code(const PJ *obj, int index);

const char PROJ_DLL* proj_get_remarks(const PJ *obj);

const char PROJ_DLL* proj_get_scope(const PJ *obj);

int PROJ_DLL proj_get_area_of_use(PJ_CONTEXT *ctx,
                                      const PJ *obj,
                                      double* out_west_lon_degree,
                                      double* out_south_lat_degree,
                                      double* out_east_lon_degree,
                                      double* out_north_lat_degree,
                                      const char **out_area_name);

const char PROJ_DLL* proj_as_wkt(PJ_CONTEXT *ctx,
                                     const PJ *obj, PJ_WKT_TYPE type,
                                     const char* const *options);

const char PROJ_DLL* proj_as_proj_string(PJ_CONTEXT *ctx,
                                             const PJ *obj,
                                             PJ_PROJ_STRING_TYPE type,
                                             const char* const *options);

const char PROJ_DLL* proj_as_projjson(PJ_CONTEXT *ctx,
                                      const PJ *obj,
                                      const char* const *options);

PJ PROJ_DLL *proj_get_source_crs(PJ_CONTEXT *ctx,
                                         const PJ *obj);

PJ PROJ_DLL *proj_get_target_crs(PJ_CONTEXT *ctx,
                                         const PJ *obj);

PJ_OBJ_LIST PROJ_DLL *proj_identify(PJ_CONTEXT *ctx,
                                        const PJ *obj,
                                        const char *auth_name,
                                        const char* const *options,
                                        int **out_confidence);

void PROJ_DLL proj_int_list_destroy(int* list);

/* ------------------------------------------------------------------------- */

PROJ_STRING_LIST PROJ_DLL proj_get_authorities_from_database(PJ_CONTEXT *ctx);

PROJ_STRING_LIST PROJ_DLL proj_get_codes_from_database(PJ_CONTEXT *ctx,
                                             const char *auth_name,
                                             PJ_TYPE type,
                                             int allow_deprecated);

PROJ_CRS_LIST_PARAMETERS PROJ_DLL *proj_get_crs_list_parameters_create(void);

void PROJ_DLL proj_get_crs_list_parameters_destroy(
                                        PROJ_CRS_LIST_PARAMETERS* params);

PROJ_CRS_INFO PROJ_DLL **proj_get_crs_info_list_from_database(
                                      PJ_CONTEXT *ctx,
                                      const char *auth_name,
                                      const PROJ_CRS_LIST_PARAMETERS* params,
                                      int *out_result_count);

void PROJ_DLL proj_crs_info_list_destroy(PROJ_CRS_INFO** list);

PROJ_UNIT_INFO PROJ_DLL **proj_get_units_from_database(
                                            PJ_CONTEXT *ctx,
                                            const char *auth_name,
                                            const char *category,
                                            int allow_deprecated,
                                            int *out_result_count);

void PROJ_DLL proj_unit_list_destroy(PROJ_UNIT_INFO** list);

/* ------------------------------------------------------------------------- */


/*! @cond Doxygen_Suppress */
typedef struct PJ_OPERATION_FACTORY_CONTEXT PJ_OPERATION_FACTORY_CONTEXT;
/*! @endcond */

PJ_OPERATION_FACTORY_CONTEXT PROJ_DLL *proj_create_operation_factory_context(
                                            PJ_CONTEXT *ctx,
                                            const char *authority);

void PROJ_DLL proj_operation_factory_context_destroy(
                                            PJ_OPERATION_FACTORY_CONTEXT *ctx);

void PROJ_DLL proj_operation_factory_context_set_desired_accuracy(
                                            PJ_CONTEXT *ctx,
                                            PJ_OPERATION_FACTORY_CONTEXT *factory_ctx,
                                            double accuracy);

void PROJ_DLL proj_operation_factory_context_set_area_of_interest(
                                            PJ_CONTEXT *ctx,
                                            PJ_OPERATION_FACTORY_CONTEXT *factory_ctx,
                                            double west_lon_degree,
                                            double south_lat_degree,
                                            double east_lon_degree,
                                            double north_lat_degree);

void PROJ_DLL proj_operation_factory_context_set_crs_extent_use(
                                            PJ_CONTEXT *ctx,
                                            PJ_OPERATION_FACTORY_CONTEXT *factory_ctx,
                                            PROJ_CRS_EXTENT_USE use);

void PROJ_DLL proj_operation_factory_context_set_spatial_criterion(
                                            PJ_CONTEXT *ctx,
                                            PJ_OPERATION_FACTORY_CONTEXT *factory_ctx,
                                            PROJ_SPATIAL_CRITERION criterion);


void PROJ_DLL proj_operation_factory_context_set_grid_availability_use(
                                            PJ_CONTEXT *ctx,
                                            PJ_OPERATION_FACTORY_CONTEXT *factory_ctx,
                                            PROJ_GRID_AVAILABILITY_USE use);

void PROJ_DLL proj_operation_factory_context_set_use_proj_alternative_grid_names(
    PJ_CONTEXT *ctx,
    PJ_OPERATION_FACTORY_CONTEXT *factory_ctx,
    int usePROJNames);

void PROJ_DLL proj_operation_factory_context_set_allow_use_intermediate_crs(
    PJ_CONTEXT *ctx,
    PJ_OPERATION_FACTORY_CONTEXT *factory_ctx,
    PROJ_INTERMEDIATE_CRS_USE use);

void PROJ_DLL proj_operation_factory_context_set_allowed_intermediate_crs(
    PJ_CONTEXT *ctx,
    PJ_OPERATION_FACTORY_CONTEXT *factory_ctx,
    const char* const *list_of_auth_name_codes);

void PROJ_DLL proj_operation_factory_context_set_discard_superseded(
    PJ_CONTEXT *ctx,
    PJ_OPERATION_FACTORY_CONTEXT *factory_ctx,
    int discard);

void PROJ_DLL proj_operation_factory_context_set_allow_ballpark_transformations(
    PJ_CONTEXT *ctx,
    PJ_OPERATION_FACTORY_CONTEXT *factory_ctx,
    int allow);

/* ------------------------------------------------------------------------- */


PJ_OBJ_LIST PROJ_DLL *proj_create_operations(
                            PJ_CONTEXT *ctx,
                            const PJ *source_crs,
                            const PJ *target_crs,
                            const PJ_OPERATION_FACTORY_CONTEXT *operationContext);

int PROJ_DLL proj_list_get_count(const PJ_OBJ_LIST *result);

PJ PROJ_DLL *proj_list_get(PJ_CONTEXT *ctx,
                                   const PJ_OBJ_LIST *result,
                                   int index);

void PROJ_DLL proj_list_destroy(PJ_OBJ_LIST *result);

int PROJ_DLL proj_get_suggested_operation(PJ_CONTEXT *ctx,
                                          PJ_OBJ_LIST *operations,
                                          PJ_DIRECTION direction,
                                          PJ_COORD coord);

/* ------------------------------------------------------------------------- */

PJ PROJ_DLL *proj_crs_get_geodetic_crs(PJ_CONTEXT *ctx, const PJ *crs);

PJ PROJ_DLL *proj_crs_get_horizontal_datum(PJ_CONTEXT *ctx, const PJ *crs);

PJ PROJ_DLL *proj_crs_get_sub_crs(PJ_CONTEXT *ctx, const PJ *crs, int index);

PJ PROJ_DLL *proj_crs_get_datum(PJ_CONTEXT *ctx, const PJ *crs);

PJ PROJ_DLL *proj_crs_get_datum_ensemble(PJ_CONTEXT *ctx, const PJ *crs);

PJ PROJ_DLL *proj_crs_get_datum_forced(PJ_CONTEXT *ctx, const PJ *crs);

int PROJ_DLL proj_datum_ensemble_get_member_count(PJ_CONTEXT *ctx,
                                                  const PJ *datum_ensemble);

double PROJ_DLL proj_datum_ensemble_get_accuracy(PJ_CONTEXT *ctx,
                                                 const PJ *datum_ensemble);

PJ PROJ_DLL *proj_datum_ensemble_get_member(PJ_CONTEXT *ctx,
                                            const PJ *datum_ensemble,
                                            int member_index);

double PROJ_DLL proj_dynamic_datum_get_frame_reference_epoch(PJ_CONTEXT *ctx,
                                                     const PJ *datum);

PJ PROJ_DLL *proj_crs_get_coordinate_system(PJ_CONTEXT *ctx, const PJ *crs);

PJ_COORDINATE_SYSTEM_TYPE PROJ_DLL proj_cs_get_type(PJ_CONTEXT *ctx,
                                                        const PJ *cs);

int PROJ_DLL proj_cs_get_axis_count(PJ_CONTEXT *ctx,
                                        const PJ *cs);

int PROJ_DLL proj_cs_get_axis_info(PJ_CONTEXT *ctx,
                                       const PJ *cs, int index,
                                       const char **out_name,
                                       const char **out_abbrev,
                                       const char **out_direction,
                                       double *out_unit_conv_factor,
                                       const char **out_unit_name,
                                       const char **out_unit_auth_name,
                                       const char **out_unit_code);

PJ PROJ_DLL *proj_get_ellipsoid(PJ_CONTEXT *ctx,
                                        const PJ *obj);

int PROJ_DLL proj_ellipsoid_get_parameters(PJ_CONTEXT *ctx,
                                               const PJ *ellipsoid,
                                            double *out_semi_major_metre,
                                            double *out_semi_minor_metre,
                                            int    *out_is_semi_minor_computed,
                                            double *out_inv_flattening);

PJ PROJ_DLL *proj_get_prime_meridian(PJ_CONTEXT *ctx,
                                             const PJ *obj);

int PROJ_DLL proj_prime_meridian_get_parameters(PJ_CONTEXT *ctx,
                                                    const PJ *prime_meridian,
                                               double *out_longitude,
                                               double *out_unit_conv_factor,
                                               const char **out_unit_name);

PJ PROJ_DLL *proj_crs_get_coordoperation(PJ_CONTEXT *ctx,
                                                 const PJ *crs);

int PROJ_DLL proj_coordoperation_get_method_info(PJ_CONTEXT *ctx,
                                                 const PJ *coordoperation,
                                                 const char **out_method_name,
                                                 const char **out_method_auth_name,
                                                 const char **out_method_code);

int PROJ_DLL proj_coordoperation_is_instantiable(PJ_CONTEXT *ctx,
                                                 const PJ *coordoperation);

int PROJ_DLL proj_coordoperation_has_ballpark_transformation(PJ_CONTEXT *ctx,
                                                 const PJ *coordoperation);

int PROJ_DLL proj_coordoperation_get_param_count(PJ_CONTEXT *ctx,
                                                 const PJ *coordoperation);

int PROJ_DLL proj_coordoperation_get_param_index(PJ_CONTEXT *ctx,
                                                 const PJ *coordoperation,
                                                 const char *name);

int PROJ_DLL proj_coordoperation_get_param(PJ_CONTEXT *ctx,
                                           const PJ *coordoperation,
                                           int index,
                                           const char **out_name,
                                           const char **out_auth_name,
                                           const char **out_code,
                                           double *out_value,
                                           const char **out_value_string,
                                           double *out_unit_conv_factor,
                                           const char **out_unit_name,
                                           const char **out_unit_auth_name,
                                           const char **out_unit_code,
                                           const char **out_unit_category);

int PROJ_DLL proj_coordoperation_get_grid_used_count(PJ_CONTEXT *ctx,
                                                     const PJ *coordoperation);

int PROJ_DLL proj_coordoperation_get_grid_used(PJ_CONTEXT *ctx,
                                               const PJ *coordoperation,
                                               int index,
                                               const char **out_short_name,
                                               const char **out_full_name,
                                               const char **out_package_name,
                                               const char **out_url,
                                               int *out_direct_download,
                                               int *out_open_license,
                                               int *out_available);

double PROJ_DLL proj_coordoperation_get_accuracy(PJ_CONTEXT *ctx,
                                                 const PJ *obj);

int PROJ_DLL proj_coordoperation_get_towgs84_values(PJ_CONTEXT *ctx,
                                                    const PJ *coordoperation,
                                                    double *out_values,
                                                    int value_count,
                                                    int emit_error_if_incompatible);

PJ PROJ_DLL *proj_coordoperation_create_inverse(PJ_CONTEXT *ctx, const PJ *obj);


int PROJ_DLL proj_concatoperation_get_step_count(PJ_CONTEXT *ctx,
                                                 const PJ *concatoperation);

PJ PROJ_DLL *proj_concatoperation_get_step(PJ_CONTEXT *ctx,
                                           const PJ *concatoperation,
                                           int i_step);

/**@}*/

#ifdef __cplusplus
}
#endif

#endif /* ndef PROJ_H */
