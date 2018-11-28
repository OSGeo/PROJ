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

#include <stddef.h>  /* For size_t */


#ifdef PROJECTS_H
#error proj.h must be included before projects.h
#endif
#ifdef PROJ_API_H
#error proj.h must be included before proj_api.h
#endif

#ifdef PROJ_RENAME_SYMBOLS
#include "proj_symbol_rename.h"
#endif

#ifndef PROJ_H
#define PROJ_H
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

/* The version numbers should be updated with every release! **/
#define PROJ_VERSION_MAJOR 6
#define PROJ_VERSION_MINOR 0
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
                                    /* semi-colons.                         */
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


/* Functionality for handling thread contexts */
#define PJ_DEFAULT_CTX 0
PJ_CONTEXT PROJ_DLL *proj_context_create (void);
PJ_CONTEXT PROJ_DLL *proj_context_destroy (PJ_CONTEXT *ctx);

void PROJ_DLL proj_context_use_proj4_init_rules(PJ_CONTEXT *ctx, int enable);
int PROJ_DLL proj_context_get_use_proj4_init_rules(PJ_CONTEXT *ctx, int from_legacy_code_path);

/* Manage the transformation definition object PJ */
PJ PROJ_DLL *proj_create (PJ_CONTEXT *ctx, const char *definition);
PJ PROJ_DLL *proj_create_argv (PJ_CONTEXT *ctx, int argc, char **argv);
PJ PROJ_DLL *proj_create_crs_to_crs(PJ_CONTEXT *ctx, const char *srid_from, const char *srid_to, PJ_AREA *area);
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
const PJ_UNITS            PROJ_DLL *proj_list_units(void);
const PJ_UNITS            PROJ_DLL *proj_list_angular_units(void);
const PJ_PRIME_MERIDIANS  PROJ_DLL *proj_list_prime_meridians(void);

/* These are trivial, and while occasionally useful in real code, primarily here to      */
/* simplify demo code, and in acknowledgement of the proj-internal discrepancy between   */
/* angular units expected by classical proj, and by Charles Karney's geodesics subsystem */
double PROJ_DLL proj_torad (double angle_in_degrees);
double PROJ_DLL proj_todeg (double angle_in_radians);

double PROJ_DLL proj_dmstor(const char *is, char **rs);
char PROJ_DLL * proj_rtodms(char *s, double r, int pos, int neg);

/*! @endcond */

/* ------------------------------------------------------------------------- */
/* Binding in C of C++ API */
/* ------------------------------------------------------------------------- */

/**
 * \defgroup basic_cpp_binding Binding in C of basic methods from the C++ API
 * @{
 */

/*! @cond Doxygen_Suppress */
typedef struct PJ_OBJ PJ_OBJ;
/*! @endcond */

/*! @cond Doxygen_Suppress */
typedef struct PJ_OBJ_LIST PJ_OBJ_LIST;
/*! @endcond */

int PROJ_DLL proj_context_set_database_path(PJ_CONTEXT *ctx,
                                            const char *dbPath,
                                            const char *const *auxDbPaths,
                                            const char* const *options);

const char PROJ_DLL *proj_context_get_database_path(PJ_CONTEXT *ctx);

const char PROJ_DLL *proj_context_get_database_metadata(PJ_CONTEXT* ctx,
                                                        const char* key);


/** \brief Guessed WKT "dialect". */
typedef enum
{
    /** \ref WKT2_2018 */
    PJ_GUESSED_WKT2_2018,

    /** \ref WKT2_2015 */
    PJ_GUESSED_WKT2_2015,

    /** \ref WKT1 */
    PJ_GUESSED_WKT1_GDAL,

    /** ESRI variant of WKT1 */
    PJ_GUESSED_WKT1_ESRI,

    /** Not WKT / unrecognized */
    PJ_GUESSED_NOT_WKT
} PJ_GUESSED_WKT_DIALECT;

PJ_GUESSED_WKT_DIALECT PROJ_DLL proj_context_guess_wkt_dialect(PJ_CONTEXT *ctx,
                                                               const char *wkt);

PJ_OBJ PROJ_DLL *proj_obj_create_from_user_input(PJ_CONTEXT *ctx,
                                                 const char *text,
                                                 const char* const *options);

PJ_OBJ PROJ_DLL *proj_obj_create_from_wkt(PJ_CONTEXT *ctx, const char *wkt,
                                          const char* const *options);

PJ_OBJ PROJ_DLL *proj_obj_create_from_proj_string(PJ_CONTEXT *ctx,
                                                  const char *proj_string,
                                                  const char* const *options);

/** \brief Object category. */
typedef enum
{
    PJ_OBJ_CATEGORY_ELLIPSOID,
    PJ_OBJ_CATEGORY_DATUM,
    PJ_OBJ_CATEGORY_CRS,
    PJ_OBJ_CATEGORY_COORDINATE_OPERATION
} PJ_OBJ_CATEGORY;

PJ_OBJ PROJ_DLL *proj_obj_create_from_database(PJ_CONTEXT *ctx,
                                               const char *auth_name,
                                               const char *code,
                                               PJ_OBJ_CATEGORY category,
                                               int usePROJAlternativeGridNames,
                                               const char* const *options);

void PROJ_DLL proj_obj_unref(PJ_OBJ *obj);

PJ_OBJ PROJ_DLL *proj_obj_clone(const PJ_OBJ *obj);

/** \brief Object type. */
typedef enum
{
    PJ_OBJ_TYPE_UNKNOWN,

    PJ_OBJ_TYPE_ELLIPSOID,

    PJ_OBJ_TYPE_GEODETIC_REFERENCE_FRAME,
    PJ_OBJ_TYPE_DYNAMIC_GEODETIC_REFERENCE_FRAME,
    PJ_OBJ_TYPE_VERTICAL_REFERENCE_FRAME,
    PJ_OBJ_TYPE_DYNAMIC_VERTICAL_REFERENCE_FRAME,
    PJ_OBJ_TYPE_DATUM_ENSEMBLE,

    /** Abstract type, not returned by proj_obj_get_type() */
    PJ_OBJ_TYPE_CRS,

    PJ_OBJ_TYPE_GEODETIC_CRS,
    PJ_OBJ_TYPE_GEOCENTRIC_CRS,

    /** proj_obj_get_type() will never return that type, but
     * PJ_OBJ_TYPE_GEOGRAPHIC_2D_CRS or PJ_OBJ_TYPE_GEOGRAPHIC_3D_CRS. */
    PJ_OBJ_TYPE_GEOGRAPHIC_CRS,

    PJ_OBJ_TYPE_GEOGRAPHIC_2D_CRS,
    PJ_OBJ_TYPE_GEOGRAPHIC_3D_CRS,
    PJ_OBJ_TYPE_VERTICAL_CRS,
    PJ_OBJ_TYPE_PROJECTED_CRS,
    PJ_OBJ_TYPE_COMPOUND_CRS,
    PJ_OBJ_TYPE_TEMPORAL_CRS,
    PJ_OBJ_TYPE_ENGINEERING_CRS,
    PJ_OBJ_TYPE_BOUND_CRS,
    PJ_OBJ_TYPE_OTHER_CRS,

    PJ_OBJ_TYPE_CONVERSION,
    PJ_OBJ_TYPE_TRANSFORMATION,
    PJ_OBJ_TYPE_CONCATENATED_OPERATION,
    PJ_OBJ_TYPE_OTHER_COORDINATE_OPERATION,
} PJ_OBJ_TYPE;

PJ_OBJ_LIST PROJ_DLL *proj_obj_create_from_name(PJ_CONTEXT *ctx,
                                                const char *auth_name,
                                                const char *searchedName,
                                                const PJ_OBJ_TYPE* types,
                                                size_t typesCount,
                                                int approximateMatch,
                                                size_t limitResultCount,
                                                const char* const *options);

PJ_OBJ_TYPE PROJ_DLL proj_obj_get_type(const PJ_OBJ *obj);

int PROJ_DLL proj_obj_is_deprecated(const PJ_OBJ *obj);

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

int PROJ_DLL proj_obj_is_equivalent_to(const PJ_OBJ *obj, const PJ_OBJ* other,
                                       PJ_COMPARISON_CRITERION criterion);

int PROJ_DLL proj_obj_is_crs(const PJ_OBJ *obj);

const char PROJ_DLL* proj_obj_get_name(const PJ_OBJ *obj);

const char PROJ_DLL* proj_obj_get_id_auth_name(const PJ_OBJ *obj, int index);

const char PROJ_DLL* proj_obj_get_id_code(const PJ_OBJ *obj, int index);

int PROJ_DLL proj_obj_get_area_of_use(const PJ_OBJ *obj,
                                      double* p_west_lon_degree,
                                      double* p_south_lat_degree,
                                      double* p_east_lon_degree,
                                      double* p_north_lat_degree,
                                      const char **p_area_name);

/** \brief WKT version. */
typedef enum
{
    /** cf osgeo::proj::io::WKTFormatter::Convention::WKT2 */
    PJ_WKT2_2015,
    /** cf osgeo::proj::io::WKTFormatter::Convention::WKT2_SIMPLIFIED */
    PJ_WKT2_2015_SIMPLIFIED,
    /** cf osgeo::proj::io::WKTFormatter::Convention::WKT2_2018 */
    PJ_WKT2_2018,
    /** cf osgeo::proj::io::WKTFormatter::Convention::WKT2_2018_SIMPLIFIED */
    PJ_WKT2_2018_SIMPLIFIED,
    /** cf osgeo::proj::io::WKTFormatter::Convention::WKT1_GDAL */
    PJ_WKT1_GDAL,
    /** cf osgeo::proj::io::WKTFormatter::Convention::WKT1_ESRI */
    PJ_WKT1_ESRI
} PJ_WKT_TYPE;

const char PROJ_DLL* proj_obj_as_wkt(const PJ_OBJ *obj, PJ_WKT_TYPE type,
                                     const char* const *options);

/** \brief PROJ string version. */
typedef enum
{
    /** cf osgeo::proj::io::PROJStringFormatter::Convention::PROJ_5 */
    PJ_PROJ_5,
    /** cf osgeo::proj::io::PROJStringFormatter::Convention::PROJ_4 */
    PJ_PROJ_4
} PJ_PROJ_STRING_TYPE;

const char PROJ_DLL* proj_obj_as_proj_string(const PJ_OBJ *obj,
                                             PJ_PROJ_STRING_TYPE type,
                                             const char* const *options);

PJ_OBJ PROJ_DLL *proj_obj_get_source_crs(const PJ_OBJ *obj);

PJ_OBJ PROJ_DLL *proj_obj_get_target_crs(const PJ_OBJ *obj);

PJ_OBJ_LIST PROJ_DLL *proj_obj_identify(const PJ_OBJ* obj,
                                        const char *auth_name,
                                        const char* const *options,
                                        int **confidence);

void PROJ_DLL proj_free_int_list(int* list);

/* ------------------------------------------------------------------------- */

/** \brief Type representing a NULL terminated list of NUL-terminate strings. */
typedef char **PROJ_STRING_LIST;

PROJ_STRING_LIST PROJ_DLL proj_get_authorities_from_database(PJ_CONTEXT *ctx);

PROJ_STRING_LIST PROJ_DLL proj_get_codes_from_database(PJ_CONTEXT *ctx,
                                             const char *auth_name,
                                             PJ_OBJ_TYPE type,
                                             int allow_deprecated);

void PROJ_DLL proj_free_string_list(PROJ_STRING_LIST list);

/* ------------------------------------------------------------------------- */


/*! @cond Doxygen_Suppress */
typedef struct PJ_OPERATION_FACTORY_CONTEXT PJ_OPERATION_FACTORY_CONTEXT;
/*! @endcond */

PJ_OPERATION_FACTORY_CONTEXT PROJ_DLL *proj_create_operation_factory_context(
                                            PJ_CONTEXT *ctx,
                                            const char *authority);

void PROJ_DLL proj_operation_factory_context_unref(
                                            PJ_OPERATION_FACTORY_CONTEXT *ctxt);

void PROJ_DLL proj_operation_factory_context_set_desired_accuracy(
                                            PJ_OPERATION_FACTORY_CONTEXT *ctxt,
                                            double accuracy);

void PROJ_DLL proj_operation_factory_context_set_area_of_interest(
                                            PJ_OPERATION_FACTORY_CONTEXT *ctxt,
                                            double west_lon_degree,
                                            double south_lat_degree,
                                            double east_lon_degree,
                                            double north_lat_degree);

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

void PROJ_DLL proj_operation_factory_context_set_crs_extent_use(
                                            PJ_OPERATION_FACTORY_CONTEXT *ctxt,
                                            PROJ_CRS_EXTENT_USE use);

/** Spatial criterion to restrict candiate operations. */
typedef enum {
    /** The area of validity of transforms should strictly contain the
        * are of interest. */
    PROJ_SPATIAL_CRITERION_STRICT_CONTAINMENT,

    /** The area of validity of transforms should at least intersect the
        * area of interest. */
    PROJ_SPATIAL_CRITERION_PARTIAL_INTERSECTION
} PROJ_SPATIAL_CRITERION;

void PROJ_DLL proj_operation_factory_context_set_spatial_criterion(
                                            PJ_OPERATION_FACTORY_CONTEXT *ctxt,
                                            PROJ_SPATIAL_CRITERION criterion);


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
} PROJ_GRID_AVAILABILITY_USE;

void PROJ_DLL proj_operation_factory_context_set_grid_availability_use(
                                            PJ_OPERATION_FACTORY_CONTEXT *ctxt,
                                            PROJ_GRID_AVAILABILITY_USE use);

void PROJ_DLL proj_operation_factory_context_set_use_proj_alternative_grid_names(
    PJ_OPERATION_FACTORY_CONTEXT *ctxt,
    int usePROJNames);

void PROJ_DLL proj_operation_factory_context_set_allow_use_intermediate_crs(
    PJ_OPERATION_FACTORY_CONTEXT *ctxt, int allow);

void PROJ_DLL proj_operation_factory_context_set_allowed_intermediate_crs(
    PJ_OPERATION_FACTORY_CONTEXT *ctxt,
    const char* const *list_of_auth_name_codes);

/* ------------------------------------------------------------------------- */


PJ_OBJ_LIST PROJ_DLL *proj_obj_create_operations(
                            const PJ_OBJ *source_crs,
                            const PJ_OBJ *target_crs,
                            const PJ_OPERATION_FACTORY_CONTEXT *operationContext);

int PROJ_DLL proj_obj_list_get_count(const PJ_OBJ_LIST *result);

PJ_OBJ PROJ_DLL *proj_obj_list_get(const PJ_OBJ_LIST *result,
                                           int index);

void PROJ_DLL proj_obj_list_unref(PJ_OBJ_LIST *result);

/* ------------------------------------------------------------------------- */

PJ_OBJ PROJ_DLL *proj_obj_crs_get_geodetic_crs(const PJ_OBJ *crs);

PJ_OBJ PROJ_DLL *proj_obj_crs_get_horizontal_datum(const PJ_OBJ *crs);

PJ_OBJ PROJ_DLL *proj_obj_crs_get_sub_crs(const PJ_OBJ *crs, int index);

PJ_OBJ PROJ_DLL *proj_obj_crs_get_datum(const PJ_OBJ *crs);

PJ_OBJ PROJ_DLL *proj_obj_crs_get_coordinate_system(const PJ_OBJ *crs);

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

PJ_COORDINATE_SYSTEM_TYPE PROJ_DLL proj_obj_cs_get_type(const PJ_OBJ* cs);

int PROJ_DLL proj_obj_cs_get_axis_count(const PJ_OBJ *cs);

int PROJ_DLL proj_obj_cs_get_axis_info(const PJ_OBJ *cs, int index,
                                       const char **pName,
                                       const char **pAbbrev,
                                       const char **pDirection,
                                       double *pUnitConvFactor,
                                       const char **pUnitName);

PJ_OBJ PROJ_DLL *proj_obj_get_ellipsoid(const PJ_OBJ *obj);

int PROJ_DLL proj_obj_ellipsoid_get_parameters(const PJ_OBJ *ellipsoid,
                                            double *pSemiMajorMetre,
                                            double *pSemiMinorMetre,
                                            int    *pIsSemiMinorComputed,
                                            double *pInverseFlattening);

PJ_OBJ PROJ_DLL *proj_obj_get_prime_meridian(const PJ_OBJ *obj);

int PROJ_DLL proj_obj_prime_meridian_get_parameters(const PJ_OBJ *prime_meridian,
                                               double *pLongitude,
                                               double *pLongitudeUnitConvFactor,
                                               const char **pLongitudeUnitName);

PJ_OBJ PROJ_DLL *proj_obj_crs_get_coordoperation(const PJ_OBJ *crs,
                                             const char **pMethodName,
                                             const char **pMethodAuthorityName,
                                             const char **pMethodCode);

int PROJ_DLL proj_coordoperation_is_instanciable(const PJ_OBJ *coordoperation);

int PROJ_DLL proj_coordoperation_get_param_count(const PJ_OBJ *coordoperation);

int PROJ_DLL proj_coordoperation_get_param_index(const PJ_OBJ *coordoperation,
                                                 const char *name);

int PROJ_DLL proj_coordoperation_get_param(const PJ_OBJ *coordoperation,
                                           int index,
                                           const char **pName,
                                           const char **pNameAuthorityName,
                                           const char **pNameCode,
                                           double *pValue,
                                           const char **pValueString,
                                           double *pValueUnitConvFactor,
                                           const char **pValueUnitName);

int PROJ_DLL proj_coordoperation_get_grid_used_count(const PJ_OBJ *coordoperation);

int PROJ_DLL proj_coordoperation_get_grid_used(const PJ_OBJ *coordoperation,
                                               int index,
                                               const char **pShortName,
                                               const char **pFullName,
                                               const char **pPackageName,
                                               const char **pURL,
                                               int *pDirectDownload,
                                               int *pOpenLicense,
                                               int *pAvailable);

double PROJ_DLL proj_coordoperation_get_accuracy(const PJ_OBJ* obj);

/**@}*/

/* ------------------------------------------------------------------------- */
/* Binding in C of advanced methods from the C++ API                         */
/*                                                                           */
/* Manual construction of CRS objects.                                       */
/* ------------------------------------------------------------------------- */

/**
 * \defgroup advanced_cpp_binding Binding in C of advanced methods from the C++ API
 * @{
 */

/** Type of unit of measure. */
typedef enum
{
    /** Angular unit of measure */
    PJ_UT_ANGULAR,
    /** Linear unit of measure */
    PJ_UT_LINEAR,
    /** Scale unit of measure */
    PJ_UT_SCALE,
    /** Time unit of measure */
    PJ_UT_TIME,
    /** Parametric unit of measure */
    PJ_UT_PARAMETRIC
} PJ_UNIT_TYPE;

/** Axis description. */
typedef struct
{
    /** Axis name. */
    char* name;
    /** Axis abbreviation. */
    char* abbreviation;
    /** Axis direction. */
    char* direction;
    /** Axis unit name. */
    char* unit_name;
    /** Conversion factor to SI of the unit. */
    double unit_conv_factor;
    /** Type of unit */
    PJ_UNIT_TYPE unit_type;
} PJ_AXIS_DESCRIPTION;

PJ_OBJ PROJ_DLL *proj_obj_create_cs(PJ_CONTEXT *ctx,
                                    PJ_COORDINATE_SYSTEM_TYPE type,
                                    int axis_count,
                                    const PJ_AXIS_DESCRIPTION* axis);

/** Type of Cartesian 2D coordinate system. */
typedef enum
{
    /* Easting-Norting */
    PJ_CART2D_EASTING_NORTHING,
    /* Northing-Easting */
    PJ_CART2D_NORTHING_EASTING,
} PJ_CARTESIAN_CS_2D_TYPE;

PJ_OBJ PROJ_DLL *proj_obj_create_cartesian_2D_cs(PJ_CONTEXT *ctx,
                                                 PJ_CARTESIAN_CS_2D_TYPE type, 
                                                 const char* unit_name,
                                                 double unit_conv_factor);


/** Type of Cartesian 2D coordinate system. */
typedef enum
{
    /* Longitude-Latitude */
    PJ_ELLPS2D_LONGITUDE_LATITUDE,
    /* Latitude-Longitude */
    PJ_ELLPS2D_LATITUDE_LONGITUDE,
} PJ_ELLIPSOIDAL_CS_2D_TYPE;

PJ_OBJ PROJ_DLL *proj_obj_create_ellipsoidal_2D_cs(PJ_CONTEXT *ctx,
                                                   PJ_ELLIPSOIDAL_CS_2D_TYPE type, 
                                                   const char* unit_name,
                                                   double unit_conv_factor);

PJ_OBJ PROJ_DLL *proj_obj_create_geographic_crs(
                            PJ_CONTEXT *ctx,
                            const char *crsName,
                            const char *datumName,
                            const char *ellipsoidName,
                            double semiMajorMetre, double invFlattening,
                            const char *primeMeridianName,
                            double primeMeridianOffset,
                            const char *pmAngularUnits,
                            double pmUnitsConv,
                            PJ_OBJ* ellipsoidalCS);

PJ_OBJ PROJ_DLL *proj_obj_create_geographic_crs_from_datum(
                            const char *crsName,
                            PJ_OBJ* datum,
                            PJ_OBJ* ellipsoidalCS);

PJ_OBJ PROJ_DLL *proj_obj_create_geocentric_crs(
                            PJ_CONTEXT *ctx,
                            const char *crsName,
                            const char *datumName,
                            const char *ellipsoidName,
                            double semiMajorMetre, double invFlattening,
                            const char *primeMeridianName,
                            double primeMeridianOffset,
                            const char *angularUnits,
                            double angularUnitsConv,
                            const char *linearUnits,
                            double linearUnitsConv);

PJ_OBJ PROJ_DLL *proj_obj_create_geocentric_crs_from_datum(
                            const char *crsName,
                            const PJ_OBJ* datum,
                            const char *linearUnits,
                            double linearUnitsConv);

PJ_OBJ PROJ_DLL *proj_obj_alter_name(const PJ_OBJ* obj, const char* name);

PJ_OBJ PROJ_DLL *proj_obj_crs_alter_geodetic_crs(const PJ_OBJ* obj,
                                                 const PJ_OBJ* newGeodCRS);

PJ_OBJ PROJ_DLL *proj_obj_crs_alter_cs_angular_unit(const PJ_OBJ* obj,
                                                    const char *angularUnits,
                                                    double angularUnitsConv);

PJ_OBJ PROJ_DLL *proj_obj_crs_alter_cs_linear_unit(const PJ_OBJ* obj,
                                                    const char *linearUnits,
                                                    double linearUnitsConv);

PJ_OBJ PROJ_DLL *proj_obj_crs_alter_parameters_linear_unit(const PJ_OBJ* obj,
                                                    const char *linearUnits,
                                                    double linearUnitsConv,
                                                    int convertToNewUnit);

PJ_OBJ PROJ_DLL *proj_obj_create_engineering_crs(PJ_CONTEXT *ctx,
                                                 const char *crsName);

/** Description of a parameter value for a Conversion. */
typedef struct
{
    /** Parameter name. */
    const char* name;
    /** Parameter authority name. */
    const char* auth_name;
    /** Parameter code. */
    const char* code;
    /** Parameter value. */
    double value;
    /** Name of unit in which parameter value is expressed. */
    const char* unit_name;
    /** Conversion factor to SI of the unit. */
    double unit_conv_factor;
    /** Type of unit */
    PJ_UNIT_TYPE unit_type;
} PJ_PARAM_DESCRIPTION;

PJ_OBJ PROJ_DLL *proj_obj_create_conversion(PJ_CONTEXT *ctx,
                                            const char* name,
                                            const char* auth_name,
                                            const char* code,
                                            const char* method_name,
                                            const char* method_auth_name,
                                            const char* method_code,
                                            int param_count,
                                            const PJ_PARAM_DESCRIPTION* params);

PJ_OBJ PROJ_DLL *proj_obj_create_projected_crs(const char* crs_name,
                                               const PJ_OBJ* geodetic_crs,
                                               const PJ_OBJ* conversion,
                                               const PJ_OBJ* coordinate_system);

PJ_OBJ PROJ_DLL *proj_obj_crs_create_bound_crs(const PJ_OBJ *base_crs,
                                               const PJ_OBJ *hub_crs,
                                               const PJ_OBJ *transformation);

PJ_OBJ PROJ_DLL *proj_obj_crs_create_bound_crs_to_WGS84(const PJ_OBJ *crs);

/* BEGIN: Generated by scripts/create_c_api_projections.py*/
PJ_OBJ PROJ_DLL *proj_obj_create_conversion_utm(
    PJ_CONTEXT *ctx,
    int zone,
    int north);

PJ_OBJ PROJ_DLL *proj_obj_create_conversion_transverse_mercator(
    PJ_CONTEXT *ctx,
    double centerLat,
    double centerLong,
    double scale,
    double falseEasting,
    double falseNorthing,
    const char* angUnitName, double angUnitConvFactor,
    const char* linearUnitName, double linearUnitConvFactor);

PJ_OBJ PROJ_DLL *proj_obj_create_conversion_gauss_schreiber_transverse_mercator(
    PJ_CONTEXT *ctx,
    double centerLat,
    double centerLong,
    double scale,
    double falseEasting,
    double falseNorthing,
    const char* angUnitName, double angUnitConvFactor,
    const char* linearUnitName, double linearUnitConvFactor);

PJ_OBJ PROJ_DLL *proj_obj_create_conversion_transverse_mercator_south_oriented(
    PJ_CONTEXT *ctx,
    double centerLat,
    double centerLong,
    double scale,
    double falseEasting,
    double falseNorthing,
    const char* angUnitName, double angUnitConvFactor,
    const char* linearUnitName, double linearUnitConvFactor);

PJ_OBJ PROJ_DLL *proj_obj_create_conversion_two_point_equidistant(
    PJ_CONTEXT *ctx,
    double latitudeFirstPoint,
    double longitudeFirstPoint,
    double latitudeSecondPoint,
    double longitudeSeconPoint,
    double falseEasting,
    double falseNorthing,
    const char* angUnitName, double angUnitConvFactor,
    const char* linearUnitName, double linearUnitConvFactor);

PJ_OBJ PROJ_DLL *proj_obj_create_conversion_tunisia_mapping_grid(
    PJ_CONTEXT *ctx,
    double centerLat,
    double centerLong,
    double falseEasting,
    double falseNorthing,
    const char* angUnitName, double angUnitConvFactor,
    const char* linearUnitName, double linearUnitConvFactor);

PJ_OBJ PROJ_DLL *proj_obj_create_conversion_albers_equal_area(
    PJ_CONTEXT *ctx,
    double latitudeFalseOrigin,
    double longitudeFalseOrigin,
    double latitudeFirstParallel,
    double latitudeSecondParallel,
    double eastingFalseOrigin,
    double northingFalseOrigin,
    const char* angUnitName, double angUnitConvFactor,
    const char* linearUnitName, double linearUnitConvFactor);

PJ_OBJ PROJ_DLL *proj_obj_create_conversion_lambert_conic_conformal_1sp(
    PJ_CONTEXT *ctx,
    double centerLat,
    double centerLong,
    double scale,
    double falseEasting,
    double falseNorthing,
    const char* angUnitName, double angUnitConvFactor,
    const char* linearUnitName, double linearUnitConvFactor);

PJ_OBJ PROJ_DLL *proj_obj_create_conversion_lambert_conic_conformal_2sp(
    PJ_CONTEXT *ctx,
    double latitudeFalseOrigin,
    double longitudeFalseOrigin,
    double latitudeFirstParallel,
    double latitudeSecondParallel,
    double eastingFalseOrigin,
    double northingFalseOrigin,
    const char* angUnitName, double angUnitConvFactor,
    const char* linearUnitName, double linearUnitConvFactor);

PJ_OBJ PROJ_DLL *proj_obj_create_conversion_lambert_conic_conformal_2sp_michigan(
    PJ_CONTEXT *ctx,
    double latitudeFalseOrigin,
    double longitudeFalseOrigin,
    double latitudeFirstParallel,
    double latitudeSecondParallel,
    double eastingFalseOrigin,
    double northingFalseOrigin,
    double ellipsoidScalingFactor,
    const char* angUnitName, double angUnitConvFactor,
    const char* linearUnitName, double linearUnitConvFactor);

PJ_OBJ PROJ_DLL *proj_obj_create_conversion_lambert_conic_conformal_2sp_belgium(
    PJ_CONTEXT *ctx,
    double latitudeFalseOrigin,
    double longitudeFalseOrigin,
    double latitudeFirstParallel,
    double latitudeSecondParallel,
    double eastingFalseOrigin,
    double northingFalseOrigin,
    const char* angUnitName, double angUnitConvFactor,
    const char* linearUnitName, double linearUnitConvFactor);

PJ_OBJ PROJ_DLL *proj_obj_create_conversion_azimuthal_equidistant(
    PJ_CONTEXT *ctx,
    double latitudeNatOrigin,
    double longitudeNatOrigin,
    double falseEasting,
    double falseNorthing,
    const char* angUnitName, double angUnitConvFactor,
    const char* linearUnitName, double linearUnitConvFactor);

PJ_OBJ PROJ_DLL *proj_obj_create_conversion_guam_projection(
    PJ_CONTEXT *ctx,
    double latitudeNatOrigin,
    double longitudeNatOrigin,
    double falseEasting,
    double falseNorthing,
    const char* angUnitName, double angUnitConvFactor,
    const char* linearUnitName, double linearUnitConvFactor);

PJ_OBJ PROJ_DLL *proj_obj_create_conversion_bonne(
    PJ_CONTEXT *ctx,
    double latitudeNatOrigin,
    double longitudeNatOrigin,
    double falseEasting,
    double falseNorthing,
    const char* angUnitName, double angUnitConvFactor,
    const char* linearUnitName, double linearUnitConvFactor);

PJ_OBJ PROJ_DLL *proj_obj_create_conversion_lambert_cylindrical_equal_area_spherical(
    PJ_CONTEXT *ctx,
    double latitudeFirstParallel,
    double longitudeNatOrigin,
    double falseEasting,
    double falseNorthing,
    const char* angUnitName, double angUnitConvFactor,
    const char* linearUnitName, double linearUnitConvFactor);

PJ_OBJ PROJ_DLL *proj_obj_create_conversion_lambert_cylindrical_equal_area(
    PJ_CONTEXT *ctx,
    double latitudeFirstParallel,
    double longitudeNatOrigin,
    double falseEasting,
    double falseNorthing,
    const char* angUnitName, double angUnitConvFactor,
    const char* linearUnitName, double linearUnitConvFactor);

PJ_OBJ PROJ_DLL *proj_obj_create_conversion_cassini_soldner(
    PJ_CONTEXT *ctx,
    double centerLat,
    double centerLong,
    double falseEasting,
    double falseNorthing,
    const char* angUnitName, double angUnitConvFactor,
    const char* linearUnitName, double linearUnitConvFactor);

PJ_OBJ PROJ_DLL *proj_obj_create_conversion_equidistant_conic(
    PJ_CONTEXT *ctx,
    double centerLat,
    double centerLong,
    double latitudeFirstParallel,
    double latitudeSecondParallel,
    double falseEasting,
    double falseNorthing,
    const char* angUnitName, double angUnitConvFactor,
    const char* linearUnitName, double linearUnitConvFactor);

PJ_OBJ PROJ_DLL *proj_obj_create_conversion_eckert_i(
    PJ_CONTEXT *ctx,
    double centerLong,
    double falseEasting,
    double falseNorthing,
    const char* angUnitName, double angUnitConvFactor,
    const char* linearUnitName, double linearUnitConvFactor);

PJ_OBJ PROJ_DLL *proj_obj_create_conversion_eckert_ii(
    PJ_CONTEXT *ctx,
    double centerLong,
    double falseEasting,
    double falseNorthing,
    const char* angUnitName, double angUnitConvFactor,
    const char* linearUnitName, double linearUnitConvFactor);

PJ_OBJ PROJ_DLL *proj_obj_create_conversion_eckert_iii(
    PJ_CONTEXT *ctx,
    double centerLong,
    double falseEasting,
    double falseNorthing,
    const char* angUnitName, double angUnitConvFactor,
    const char* linearUnitName, double linearUnitConvFactor);

PJ_OBJ PROJ_DLL *proj_obj_create_conversion_eckert_iv(
    PJ_CONTEXT *ctx,
    double centerLong,
    double falseEasting,
    double falseNorthing,
    const char* angUnitName, double angUnitConvFactor,
    const char* linearUnitName, double linearUnitConvFactor);

PJ_OBJ PROJ_DLL *proj_obj_create_conversion_eckert_v(
    PJ_CONTEXT *ctx,
    double centerLong,
    double falseEasting,
    double falseNorthing,
    const char* angUnitName, double angUnitConvFactor,
    const char* linearUnitName, double linearUnitConvFactor);

PJ_OBJ PROJ_DLL *proj_obj_create_conversion_eckert_vi(
    PJ_CONTEXT *ctx,
    double centerLong,
    double falseEasting,
    double falseNorthing,
    const char* angUnitName, double angUnitConvFactor,
    const char* linearUnitName, double linearUnitConvFactor);

PJ_OBJ PROJ_DLL *proj_obj_create_conversion_equidistant_cylindrical(
    PJ_CONTEXT *ctx,
    double latitudeFirstParallel,
    double longitudeNatOrigin,
    double falseEasting,
    double falseNorthing,
    const char* angUnitName, double angUnitConvFactor,
    const char* linearUnitName, double linearUnitConvFactor);

PJ_OBJ PROJ_DLL *proj_obj_create_conversion_equidistant_cylindrical_spherical(
    PJ_CONTEXT *ctx,
    double latitudeFirstParallel,
    double longitudeNatOrigin,
    double falseEasting,
    double falseNorthing,
    const char* angUnitName, double angUnitConvFactor,
    const char* linearUnitName, double linearUnitConvFactor);

PJ_OBJ PROJ_DLL *proj_obj_create_conversion_gall(
    PJ_CONTEXT *ctx,
    double centerLong,
    double falseEasting,
    double falseNorthing,
    const char* angUnitName, double angUnitConvFactor,
    const char* linearUnitName, double linearUnitConvFactor);

PJ_OBJ PROJ_DLL *proj_obj_create_conversion_goode_homolosine(
    PJ_CONTEXT *ctx,
    double centerLong,
    double falseEasting,
    double falseNorthing,
    const char* angUnitName, double angUnitConvFactor,
    const char* linearUnitName, double linearUnitConvFactor);

PJ_OBJ PROJ_DLL *proj_obj_create_conversion_interrupted_goode_homolosine(
    PJ_CONTEXT *ctx,
    double centerLong,
    double falseEasting,
    double falseNorthing,
    const char* angUnitName, double angUnitConvFactor,
    const char* linearUnitName, double linearUnitConvFactor);

PJ_OBJ PROJ_DLL *proj_obj_create_conversion_geostationary_satellite_sweep_x(
    PJ_CONTEXT *ctx,
    double centerLong,
    double height,
    double falseEasting,
    double falseNorthing,
    const char* angUnitName, double angUnitConvFactor,
    const char* linearUnitName, double linearUnitConvFactor);

PJ_OBJ PROJ_DLL *proj_obj_create_conversion_geostationary_satellite_sweep_y(
    PJ_CONTEXT *ctx,
    double centerLong,
    double height,
    double falseEasting,
    double falseNorthing,
    const char* angUnitName, double angUnitConvFactor,
    const char* linearUnitName, double linearUnitConvFactor);

PJ_OBJ PROJ_DLL *proj_obj_create_conversion_gnomonic(
    PJ_CONTEXT *ctx,
    double centerLat,
    double centerLong,
    double falseEasting,
    double falseNorthing,
    const char* angUnitName, double angUnitConvFactor,
    const char* linearUnitName, double linearUnitConvFactor);

PJ_OBJ PROJ_DLL *proj_obj_create_conversion_hotine_oblique_mercator_variant_a(
    PJ_CONTEXT *ctx,
    double latitudeProjectionCentre,
    double longitudeProjectionCentre,
    double azimuthInitialLine,
    double angleFromRectifiedToSkrewGrid,
    double scale,
    double falseEasting,
    double falseNorthing,
    const char* angUnitName, double angUnitConvFactor,
    const char* linearUnitName, double linearUnitConvFactor);

PJ_OBJ PROJ_DLL *proj_obj_create_conversion_hotine_oblique_mercator_variant_b(
    PJ_CONTEXT *ctx,
    double latitudeProjectionCentre,
    double longitudeProjectionCentre,
    double azimuthInitialLine,
    double angleFromRectifiedToSkrewGrid,
    double scale,
    double eastingProjectionCentre,
    double northingProjectionCentre,
    const char* angUnitName, double angUnitConvFactor,
    const char* linearUnitName, double linearUnitConvFactor);

PJ_OBJ PROJ_DLL *proj_obj_create_conversion_hotine_oblique_mercator_two_point_natural_origin(
    PJ_CONTEXT *ctx,
    double latitudeProjectionCentre,
    double latitudePoint1,
    double longitudePoint1,
    double latitudePoint2,
    double longitudePoint2,
    double scale,
    double eastingProjectionCentre,
    double northingProjectionCentre,
    const char* angUnitName, double angUnitConvFactor,
    const char* linearUnitName, double linearUnitConvFactor);

PJ_OBJ PROJ_DLL *proj_obj_create_conversion_international_map_world_polyconic(
    PJ_CONTEXT *ctx,
    double centerLong,
    double latitudeFirstParallel,
    double latitudeSecondParallel,
    double falseEasting,
    double falseNorthing,
    const char* angUnitName, double angUnitConvFactor,
    const char* linearUnitName, double linearUnitConvFactor);

PJ_OBJ PROJ_DLL *proj_obj_create_conversion_krovak_north_oriented(
    PJ_CONTEXT *ctx,
    double latitudeProjectionCentre,
    double longitudeOfOrigin,
    double colatitudeConeAxis,
    double latitudePseudoStandardParallel,
    double scaleFactorPseudoStandardParallel,
    double falseEasting,
    double falseNorthing,
    const char* angUnitName, double angUnitConvFactor,
    const char* linearUnitName, double linearUnitConvFactor);

PJ_OBJ PROJ_DLL *proj_obj_create_conversion_krovak(
    PJ_CONTEXT *ctx,
    double latitudeProjectionCentre,
    double longitudeOfOrigin,
    double colatitudeConeAxis,
    double latitudePseudoStandardParallel,
    double scaleFactorPseudoStandardParallel,
    double falseEasting,
    double falseNorthing,
    const char* angUnitName, double angUnitConvFactor,
    const char* linearUnitName, double linearUnitConvFactor);

PJ_OBJ PROJ_DLL *proj_obj_create_conversion_lambert_azimuthal_equal_area(
    PJ_CONTEXT *ctx,
    double latitudeNatOrigin,
    double longitudeNatOrigin,
    double falseEasting,
    double falseNorthing,
    const char* angUnitName, double angUnitConvFactor,
    const char* linearUnitName, double linearUnitConvFactor);

PJ_OBJ PROJ_DLL *proj_obj_create_conversion_miller_cylindrical(
    PJ_CONTEXT *ctx,
    double centerLong,
    double falseEasting,
    double falseNorthing,
    const char* angUnitName, double angUnitConvFactor,
    const char* linearUnitName, double linearUnitConvFactor);

PJ_OBJ PROJ_DLL *proj_obj_create_conversion_mercator_variant_a(
    PJ_CONTEXT *ctx,
    double centerLat,
    double centerLong,
    double scale,
    double falseEasting,
    double falseNorthing,
    const char* angUnitName, double angUnitConvFactor,
    const char* linearUnitName, double linearUnitConvFactor);

PJ_OBJ PROJ_DLL *proj_obj_create_conversion_mercator_variant_b(
    PJ_CONTEXT *ctx,
    double latitudeFirstParallel,
    double centerLong,
    double falseEasting,
    double falseNorthing,
    const char* angUnitName, double angUnitConvFactor,
    const char* linearUnitName, double linearUnitConvFactor);

PJ_OBJ PROJ_DLL *proj_obj_create_conversion_popular_visualisation_pseudo_mercator(
    PJ_CONTEXT *ctx,
    double centerLat,
    double centerLong,
    double falseEasting,
    double falseNorthing,
    const char* angUnitName, double angUnitConvFactor,
    const char* linearUnitName, double linearUnitConvFactor);

PJ_OBJ PROJ_DLL *proj_obj_create_conversion_mollweide(
    PJ_CONTEXT *ctx,
    double centerLong,
    double falseEasting,
    double falseNorthing,
    const char* angUnitName, double angUnitConvFactor,
    const char* linearUnitName, double linearUnitConvFactor);

PJ_OBJ PROJ_DLL *proj_obj_create_conversion_new_zealand_mapping_grid(
    PJ_CONTEXT *ctx,
    double centerLat,
    double centerLong,
    double falseEasting,
    double falseNorthing,
    const char* angUnitName, double angUnitConvFactor,
    const char* linearUnitName, double linearUnitConvFactor);

PJ_OBJ PROJ_DLL *proj_obj_create_conversion_oblique_stereographic(
    PJ_CONTEXT *ctx,
    double centerLat,
    double centerLong,
    double scale,
    double falseEasting,
    double falseNorthing,
    const char* angUnitName, double angUnitConvFactor,
    const char* linearUnitName, double linearUnitConvFactor);

PJ_OBJ PROJ_DLL *proj_obj_create_conversion_orthographic(
    PJ_CONTEXT *ctx,
    double centerLat,
    double centerLong,
    double falseEasting,
    double falseNorthing,
    const char* angUnitName, double angUnitConvFactor,
    const char* linearUnitName, double linearUnitConvFactor);

PJ_OBJ PROJ_DLL *proj_obj_create_conversion_american_polyconic(
    PJ_CONTEXT *ctx,
    double centerLat,
    double centerLong,
    double falseEasting,
    double falseNorthing,
    const char* angUnitName, double angUnitConvFactor,
    const char* linearUnitName, double linearUnitConvFactor);

PJ_OBJ PROJ_DLL *proj_obj_create_conversion_polar_stereographic_variant_a(
    PJ_CONTEXT *ctx,
    double centerLat,
    double centerLong,
    double scale,
    double falseEasting,
    double falseNorthing,
    const char* angUnitName, double angUnitConvFactor,
    const char* linearUnitName, double linearUnitConvFactor);

PJ_OBJ PROJ_DLL *proj_obj_create_conversion_polar_stereographic_variant_b(
    PJ_CONTEXT *ctx,
    double latitudeStandardParallel,
    double longitudeOfOrigin,
    double falseEasting,
    double falseNorthing,
    const char* angUnitName, double angUnitConvFactor,
    const char* linearUnitName, double linearUnitConvFactor);

PJ_OBJ PROJ_DLL *proj_obj_create_conversion_robinson(
    PJ_CONTEXT *ctx,
    double centerLong,
    double falseEasting,
    double falseNorthing,
    const char* angUnitName, double angUnitConvFactor,
    const char* linearUnitName, double linearUnitConvFactor);

PJ_OBJ PROJ_DLL *proj_obj_create_conversion_sinusoidal(
    PJ_CONTEXT *ctx,
    double centerLong,
    double falseEasting,
    double falseNorthing,
    const char* angUnitName, double angUnitConvFactor,
    const char* linearUnitName, double linearUnitConvFactor);

PJ_OBJ PROJ_DLL *proj_obj_create_conversion_stereographic(
    PJ_CONTEXT *ctx,
    double centerLat,
    double centerLong,
    double scale,
    double falseEasting,
    double falseNorthing,
    const char* angUnitName, double angUnitConvFactor,
    const char* linearUnitName, double linearUnitConvFactor);

PJ_OBJ PROJ_DLL *proj_obj_create_conversion_van_der_grinten(
    PJ_CONTEXT *ctx,
    double centerLong,
    double falseEasting,
    double falseNorthing,
    const char* angUnitName, double angUnitConvFactor,
    const char* linearUnitName, double linearUnitConvFactor);

PJ_OBJ PROJ_DLL *proj_obj_create_conversion_wagner_i(
    PJ_CONTEXT *ctx,
    double centerLong,
    double falseEasting,
    double falseNorthing,
    const char* angUnitName, double angUnitConvFactor,
    const char* linearUnitName, double linearUnitConvFactor);

PJ_OBJ PROJ_DLL *proj_obj_create_conversion_wagner_ii(
    PJ_CONTEXT *ctx,
    double centerLong,
    double falseEasting,
    double falseNorthing,
    const char* angUnitName, double angUnitConvFactor,
    const char* linearUnitName, double linearUnitConvFactor);

PJ_OBJ PROJ_DLL *proj_obj_create_conversion_wagner_iii(
    PJ_CONTEXT *ctx,
    double latitudeTrueScale,
    double centerLong,
    double falseEasting,
    double falseNorthing,
    const char* angUnitName, double angUnitConvFactor,
    const char* linearUnitName, double linearUnitConvFactor);

PJ_OBJ PROJ_DLL *proj_obj_create_conversion_wagner_iv(
    PJ_CONTEXT *ctx,
    double centerLong,
    double falseEasting,
    double falseNorthing,
    const char* angUnitName, double angUnitConvFactor,
    const char* linearUnitName, double linearUnitConvFactor);

PJ_OBJ PROJ_DLL *proj_obj_create_conversion_wagner_v(
    PJ_CONTEXT *ctx,
    double centerLong,
    double falseEasting,
    double falseNorthing,
    const char* angUnitName, double angUnitConvFactor,
    const char* linearUnitName, double linearUnitConvFactor);

PJ_OBJ PROJ_DLL *proj_obj_create_conversion_wagner_vi(
    PJ_CONTEXT *ctx,
    double centerLong,
    double falseEasting,
    double falseNorthing,
    const char* angUnitName, double angUnitConvFactor,
    const char* linearUnitName, double linearUnitConvFactor);

PJ_OBJ PROJ_DLL *proj_obj_create_conversion_wagner_vii(
    PJ_CONTEXT *ctx,
    double centerLong,
    double falseEasting,
    double falseNorthing,
    const char* angUnitName, double angUnitConvFactor,
    const char* linearUnitName, double linearUnitConvFactor);

PJ_OBJ PROJ_DLL *proj_obj_create_conversion_quadrilateralized_spherical_cube(
    PJ_CONTEXT *ctx,
    double centerLat,
    double centerLong,
    double falseEasting,
    double falseNorthing,
    const char* angUnitName, double angUnitConvFactor,
    const char* linearUnitName, double linearUnitConvFactor);

PJ_OBJ PROJ_DLL *proj_obj_create_conversion_spherical_cross_track_height(
    PJ_CONTEXT *ctx,
    double pegPointLat,
    double pegPointLong,
    double pegPointHeading,
    double pegPointHeight,
    const char* angUnitName, double angUnitConvFactor,
    const char* linearUnitName, double linearUnitConvFactor);

PJ_OBJ PROJ_DLL *proj_obj_create_conversion_equal_earth(
    PJ_CONTEXT *ctx,
    double centerLong,
    double falseEasting,
    double falseNorthing,
    const char* angUnitName, double angUnitConvFactor,
    const char* linearUnitName, double linearUnitConvFactor);

/* END: Generated by scripts/create_c_api_projections.py*/

/**@}*/

#ifdef __cplusplus
}
#endif

#endif /* ndef PROJ_H */
