/******************************************************************************
 * Project:  PROJ.4
 * Purpose:  Internal plumbing for the PROJ.4 library.
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

#ifndef PROJ_INTERNAL_H
#define PROJ_INTERNAL_H

#ifndef __cplusplus
#error "proj_internal.h can only be included from a C++ file"
#endif

#ifdef _MSC_VER
#  ifndef _CRT_SECURE_NO_DEPRECATE
#    define _CRT_SECURE_NO_DEPRECATE
#  endif
#  ifndef _CRT_NONSTDC_NO_DEPRECATE
#    define _CRT_NONSTDC_NO_DEPRECATE
#  endif
/* enable predefined math constants M_* for MS Visual Studio workaround */
#  ifndef _USE_MATH_DEFINES
#     define _USE_MATH_DEFINES
#  endif
#endif

/* standard inclusions */
#include <limits.h>
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "proj/common.hpp"
#include "proj/coordinateoperation.hpp"

#include <string>
#include <vector>

#include "proj.h"

#ifdef PROJ_RENAME_SYMBOLS
#include "proj_symbol_rename.h"
#endif

#define STATIC_ASSERT(COND) ((void)sizeof(char[(COND) ? 1 : -1]))

#ifndef PJ_TODEG
#define PJ_TODEG(rad)  ((rad)*180.0/M_PI)
#endif
#ifndef PJ_TORAD
#define PJ_TORAD(deg)  ((deg)*M_PI/180.0)
#endif

/* Maximum latitudinal overshoot accepted */
#define PJ_EPS_LAT 1e-12

#define C_NAMESPACE extern "C"
#define C_NAMESPACE_VAR extern "C"

#ifndef NULL
#  define NULL 0
#endif

#ifndef FALSE
#  define FALSE 0
#endif

#ifndef TRUE
#  define TRUE  1
#endif

#ifndef MAX
#  define MIN(a,b)      ((a<b) ? a : b)
#  define MAX(a,b)      ((a>b) ? a : b)
#endif

#ifndef ABS
#  define ABS(x)        ((x<0) ? (-1*(x)) : x)
#endif

#if INT_MAX == 2147483647
typedef int pj_int32;
#elif LONG_MAX == 2147483647
typedef long pj_int32;
#else
#warning It seems no 32-bit integer type is available
#endif

/* maximum path/filename */
#ifndef MAX_PATH_FILENAME
#define MAX_PATH_FILENAME 1024
#endif

/* If we still haven't got M_PI*, we rely on our own defines.
 * For example, this is necessary when compiling with gcc and
 * the -ansi flag.
 */
#ifndef M_PI
#define M_PI            3.14159265358979323846
#define M_PI_2          1.57079632679489661923
#define M_PI_4          0.78539816339744830962
#define M_2_PI          0.63661977236758134308
#endif

/* M_SQRT2 might be missing */
#ifndef M_SQRT2
#define M_SQRT2         1.41421356237309504880
#endif

/* some more useful math constants and aliases */
#define M_FORTPI        M_PI_4                   /* pi/4 */
#define M_HALFPI        M_PI_2                   /* pi/2 */
#define M_PI_HALFPI     4.71238898038468985769   /* 1.5*pi */
#define M_TWOPI         6.28318530717958647693   /* 2*pi */
#define M_TWO_D_PI      M_2_PI                   /* 2/pi */
#define M_TWOPI_HALFPI  7.85398163397448309616   /* 2.5*pi */


/* maximum tag id length for +init and default files */
#ifndef ID_TAG_MAX
#define ID_TAG_MAX 50
#endif

/* Use WIN32 as a standard windows 32 bit declaration */
#if defined(_WIN32) && !defined(WIN32)
#  define WIN32
#endif

#if defined(_WINDOWS) && !defined(WIN32)
#  define WIN32
#endif

/* directory delimiter for DOS support */
#ifdef WIN32
#define DIR_CHAR '\\'
#else
#define DIR_CHAR '/'
#endif

enum pj_io_units {
    PJ_IO_UNITS_WHATEVER  = 0,  /* Doesn't matter (or depends on pipeline neighbours) */
    PJ_IO_UNITS_CLASSIC   = 1,  /* Scaled meters (right), projected system */
    PJ_IO_UNITS_PROJECTED = 2,  /* Meters, projected system */
    PJ_IO_UNITS_CARTESIAN = 3,  /* Meters, 3D cartesian system */
    PJ_IO_UNITS_RADIANS   = 4,  /* Radians */
    PJ_IO_UNITS_DEGREES   = 5,  /* Degrees */

};
enum pj_io_units pj_left (PJ *P);
enum pj_io_units pj_right (PJ *P);

PJ_COORD PROJ_DLL proj_coord_error (void);

void proj_context_errno_set (PJ_CONTEXT *ctx, int err);
void PROJ_DLL proj_context_set (PJ *P, PJ_CONTEXT *ctx);
void proj_context_inherit (PJ *parent, PJ *child);

struct projCppContext;
/* not sure why we need to export it, but mingw needs it */
void PROJ_DLL proj_context_delete_cpp_context(struct projCppContext* cppContext);

PJ_COORD pj_fwd4d (PJ_COORD coo, PJ *P);
PJ_COORD pj_inv4d (PJ_COORD coo, PJ *P);

PJ_COORD PROJ_DLL pj_approx_2D_trans (PJ *P, PJ_DIRECTION direction, PJ_COORD coo);
PJ_COORD PROJ_DLL pj_approx_3D_trans (PJ *P, PJ_DIRECTION direction, PJ_COORD coo);

/* Provision for gettext translatable strings */
#define _(str) (str)

void PROJ_DLL proj_log_error (const PJ *P, const char *fmt, ...);
void proj_log_debug (PJ *P, const char *fmt, ...);
void proj_log_trace (PJ *P, const char *fmt, ...);

void proj_context_log_debug (PJ_CONTEXT *ctx, const char *fmt, ...);

int pj_ellipsoid (PJ *);
void pj_inherit_ellipsoid_def (const PJ *src, PJ *dst);
int pj_calc_ellipsoid_params (PJ *P, double a, double es);

/* Geographical to geocentric latitude - another of the "simple, but useful" */
PJ_COORD pj_geocentric_latitude (const PJ *P, PJ_DIRECTION direction, PJ_COORD coord);

char  PROJ_DLL *pj_chomp (char *c);
char  PROJ_DLL *pj_shrink (char *c);
size_t pj_trim_argc (char *args);
char **pj_trim_argv (size_t argc, char *args);
char  *pj_make_args (size_t argc, char **argv);

typedef struct { double r, i; } COMPLEX;

/* Forward declarations and typedefs for stuff needed inside the PJ object */
struct PJconsts;

union  PJ_COORD;
struct geod_geodesic;
struct ARG_list;
struct PJ_REGION_S;
typedef struct PJ_REGION_S  PJ_Region;
typedef struct ARG_list paralist;   /* parameter list */

#ifndef PROJ_H
typedef struct PJconsts PJ;         /* the PJ object herself */
typedef union  PJ_COORD PJ_COORD;
#endif

struct PJ_REGION_S {
    double ll_long;        /* lower left corner coordinates (radians) */
    double ll_lat;
    double ur_long;        /* upper right corner coordinates (radians) */
    double ur_lat;
};

struct PJ_AREA {
    int     bbox_set;
    double  west_lon_degree;
    double  south_lat_degree;
    double  east_lon_degree;
    double  north_lat_degree;
};


/*****************************************************************************

    Some function types that are especially useful when working with PJs

******************************************************************************

PJ_CONSTRUCTOR:

    A function taking a pointer-to-PJ as arg, and returning a pointer-to-PJ.
    Historically called twice: First with a 0 argument, to allocate memory,
    second with the first return value as argument, for actual setup.

PJ_DESTRUCTOR:

    A function taking a pointer-to-PJ and an integer as args, then first
    handling the deallocation of the PJ, afterwards handing the integer over
    to the error reporting subsystem, and finally returning a null pointer in
    support of the "return free (P)" (aka "get the hell out of here") idiom.

PJ_OPERATOR:

    A function taking a PJ_COORD and a pointer-to-PJ as args, applying the
    PJ to the PJ_COORD, and returning the resulting PJ_COORD.

*****************************************************************************/
typedef    PJ       *(* PJ_CONSTRUCTOR) (PJ *);
typedef    PJ       *(* PJ_DESTRUCTOR)  (PJ *, int);
typedef    PJ_COORD  (* PJ_OPERATOR)    (PJ_COORD, PJ *);
/****************************************************************************/


/* datum_type values */
#define PJD_UNKNOWN   0
#define PJD_3PARAM    1
#define PJD_7PARAM    2
#define PJD_GRIDSHIFT 3
#define PJD_WGS84     4   /* WGS84 (or anything considered equivalent) */

struct CoordOperation
{
    int idxInOriginalList;
    double minxSrc = 0.0;
    double minySrc = 0.0;
    double maxxSrc = 0.0;
    double maxySrc = 0.0;
    double minxDst = 0.0;
    double minyDst = 0.0;
    double maxxDst = 0.0;
    double maxyDst = 0.0;
    PJ* pj = nullptr;
    std::string name{};
    double accuracy = -1.0;
    bool isOffshore = false;

    CoordOperation(int idxInOriginalListIn,
                   double minxSrcIn, double minySrcIn, double maxxSrcIn, double maxySrcIn,
                   double minxDstIn, double minyDstIn, double maxxDstIn, double maxyDstIn,
                   PJ* pjIn, const std::string& nameIn, double accuracyIn, bool isOffshoreIn):
        idxInOriginalList(idxInOriginalListIn),
        minxSrc(minxSrcIn), minySrc(minySrcIn), maxxSrc(maxxSrcIn), maxySrc(maxySrcIn),
        minxDst(minxDstIn), minyDst(minyDstIn), maxxDst(maxxDstIn), maxyDst(maxyDstIn),
        pj(pjIn), name(nameIn),
        accuracy(accuracyIn),
        isOffshore(isOffshoreIn)
    {
    }

    CoordOperation(const CoordOperation&) = delete;

    CoordOperation(CoordOperation&& other):
        idxInOriginalList(other.idxInOriginalList),
        minxSrc(other.minxSrc), minySrc(other.minySrc), maxxSrc(other.maxxSrc), maxySrc(other.maxySrc),
        minxDst(other.minxDst), minyDst(other.minyDst), maxxDst(other.maxxDst), maxyDst(other.maxyDst),
        name(std::move(other.name)),
        accuracy(other.accuracy),
        isOffshore(other.isOffshore) {
        pj = other.pj;
        other.pj = nullptr;
    }

    CoordOperation& operator=(const CoordOperation&) = delete;

    ~CoordOperation()
    {
        proj_destroy(pj);
    }
};

enum class TMercAlgo
{
    AUTO, // Poder/Engsager if far from central meridian, otherwise Evenden/Snyder
    EVENDEN_SNYDER,
    PODER_ENGSAGER,
};

/* base projection data structure */
struct PJconsts {

    /*************************************************************************************

                         G E N E R A L   P A R A M E T E R   S T R U C T

    **************************************************************************************

        TODO: Need some description here - especially about the thread context...
        This is the struct behind the PJ typedef

    **************************************************************************************/

    PJ_CONTEXT *ctx = nullptr;
    const char *short_name = nullptr; /* From pj_list.h */
    const char *descr = nullptr;   /* From pj_list.h or individual PJ_*.c file */
    paralist *params = nullptr;    /* Parameter list */
    char *def_full = nullptr;      /* Full textual definition (usually 0 - set by proj_pj_info) */
    PJconsts *parent = nullptr;    /* Parent PJ of pipeline steps - nullptr if not a pipeline step */

    /* For debugging / logging purposes */
    char *def_size = nullptr;      /* Shape and size parameters extracted from params */
    char *def_shape = nullptr;
    char *def_spherification = nullptr;
    char *def_ellps = nullptr;

    struct geod_geodesic *geod = nullptr;    /* For geodesic computations */
    void *opaque = nullptr;                  /* Projection specific parameters, Defined in PJ_*.c */
    int inverted = 0;                        /* Tell high level API functions to swap inv/fwd */


    /*************************************************************************************

                          F U N C T I O N    P O I N T E R S

    **************************************************************************************

        For projection xxx, these are pointers to functions in the corresponding
        PJ_xxx.c file.

        pj_init() delegates the setup of these to pj_projection_specific_setup_xxx(),
        a name which is currently hidden behind the magic curtain of the PROJECTION
        macro.

    **************************************************************************************/


    PJ_XY  (*fwd)(PJ_LP,    PJ *) = nullptr;
    PJ_LP  (*inv)(PJ_XY,    PJ *) = nullptr;
    PJ_XYZ (*fwd3d)(PJ_LPZ, PJ *) = nullptr;
    PJ_LPZ (*inv3d)(PJ_XYZ, PJ *) = nullptr;
    PJ_OPERATOR fwd4d = nullptr;
    PJ_OPERATOR inv4d = nullptr;

    PJ_DESTRUCTOR destructor = nullptr;
    void   (*reassign_context)(PJ*, PJ_CONTEXT*) = nullptr;


    /*************************************************************************************

                          E L L I P S O I D     P A R A M E T E R S

    **************************************************************************************

        Despite YAGNI, we add a large number of ellipsoidal shape parameters, which
        are not yet set up in pj_init. They are, however, inexpensive to compute,
        compared to the overall time taken for setting up the complex PJ object
        (cf. e.g. https://en.wikipedia.org/wiki/Angular_eccentricity).

        But during single point projections it will often be a useful thing to have
        these readily available without having to recompute at every pj_fwd / pj_inv
        call.

        With this wide selection, we should be ready for quite a number of geodetic
        algorithms, without having to incur further ABI breakage.

    **************************************************************************************/

    /* The linear parameters */

    double  a = 0.0;                   /* semimajor axis (radius if eccentricity==0) */
    double  b = 0.0;                   /* semiminor axis */
    double  ra = 0.0;                  /* 1/a */
    double  rb = 0.0;                  /* 1/b */

    /* The eccentricities */

    double  alpha = 0.0;               /* angular eccentricity */
    double  e = 0.0;                   /* first  eccentricity */
    double  es = 0.0;                  /* first  eccentricity squared */
    double  e2 = 0.0;                  /* second eccentricity */
    double  e2s = 0.0;                 /* second eccentricity squared */
    double  e3 = 0.0;                  /* third  eccentricity */
    double  e3s = 0.0;                 /* third  eccentricity squared */
    double  one_es = 0.0;              /* 1 - e^2 */
    double  rone_es = 0.0;             /* 1/one_es */


    /* The flattenings */
    double  f = 0.0;                   /* first  flattening */
    double  f2 = 0.0;                  /* second flattening */
    double  n = 0.0;                   /* third  flattening */
    double  rf = 0.0;                  /* 1/f  */
    double  rf2 = 0.0;                 /* 1/f2 */
    double  rn = 0.0;                   /* 1/n  */

    /* This one's for GRS80 */
    double  J = 0.0;                   /* "Dynamic form factor" */

    double  es_orig = 0.0;    /* es and a before any +proj related adjustment */
    double  a_orig = 0.0;


    /*************************************************************************************

                          C O O R D I N A T E   H A N D L I N G

    **************************************************************************************/

    int  over = 0;                  /* Over-range flag */
    int  geoc = 0;                  /* Geocentric latitude flag */
    int  is_latlong = 0;            /* proj=latlong ... not really a projection at all */
    int  is_geocent = 0;            /* proj=geocent ... not really a projection at all */
    int  need_ellps = 0;            /* 0 for operations that are purely cartesian */
    int  skip_fwd_prepare = 0;
    int  skip_fwd_finalize = 0;
    int  skip_inv_prepare = 0;
    int  skip_inv_finalize = 0;

    enum pj_io_units left =  PJ_IO_UNITS_WHATEVER; /* Flags for input/output coordinate types */
    enum pj_io_units right =  PJ_IO_UNITS_WHATEVER;

    /* These PJs are used for implementing cs2cs style coordinate handling in the 4D API */
    PJ *axisswap = nullptr;
    PJ *cart = nullptr;
    PJ *cart_wgs84 = nullptr;
    PJ *helmert = nullptr;
    PJ *hgridshift = nullptr;
    PJ *vgridshift = nullptr;


    /*************************************************************************************

                       C A R T O G R A P H I C       O F F S E T S

    **************************************************************************************/

    double  lam0 = 0.0;    /* central meridian */
    double  phi0 = 0.0;    /* central parallel */
    double  x0 = 0.0;      /* false easting */
    double  y0 = 0.0;      /* false northing  */
    double  z0 = 0.0;      /* height origin */
    double  t0 = 0.0;      /* time origin */


    /*************************************************************************************

                                    S C A L I N G

    **************************************************************************************/

    double  k0 = 0.0;                  /* General scaling factor - e.g. the 0.9996 of UTM */
    double  to_meter = 0.0, fr_meter = 0.0;        /* Plane coordinate scaling. Internal unit [m] */
    double  vto_meter = 0.0, vfr_meter = 0.0;      /* Vertical scaling. Internal unit [m] */


    /*************************************************************************************

                  D A T U M S   A N D   H E I G H T   S Y S T E M S

    **************************************************************************************

        It may be possible, and meaningful, to move the list parts of this up to the
        PJ_CONTEXT level.

    **************************************************************************************/

    int     datum_type = PJD_UNKNOWN;  /* PJD_UNKNOWN/3PARAM/7PARAM/GRIDSHIFT/WGS84 */
    double  datum_params[7] = {0,0,0,0,0,0,0}; /* Parameters for 3PARAM and 7PARAM */

    int     has_geoid_vgrids = 0;      /* used by legacy transform.cpp */
    void*   hgrids_legacy = nullptr;   /* used by legacy transform.cpp. Is a pointer to a ListOfHGrids* */ 
    void*   vgrids_legacy = nullptr;   /* used by legacy transform.cpp. Is a pointer to a ListOfVGrids* */ 

    double  from_greenwich = 0.0;       /* prime meridian offset (in radians) */
    double  long_wrap_center = 0.0;     /* 0.0 for -180 to 180, actually in radians*/
    int     is_long_wrap_set = 0;
    char    axis[4] = {0,0,0,0};        /* Axis order, pj_transform/pj_adjust_axis */

    /*************************************************************************************
     ISO-19111 interface
    **************************************************************************************/

    NS_PROJ::common::IdentifiedObjectPtr iso_obj{};

    // cached results
    mutable std::string lastWKT{};
    mutable std::string lastPROJString{};
    mutable std::string lastJSONString{};
    mutable bool gridsNeededAsked = false;
    mutable std::vector<NS_PROJ::operation::GridDescription> gridsNeeded{};

    /*************************************************************************************
     proj_create_crs_to_crs() alternative coordinate operations
    **************************************************************************************/
    std::vector<CoordOperation> alternativeCoordinateOperations{};
    int iCurCoordOp = -1;

    /*************************************************************************************

                 E N D   O F    G E N E R A L   P A R A M E T E R   S T R U C T

    **************************************************************************************/

    PJconsts();
    PJconsts(const PJconsts &) = delete;
    PJconsts &operator=(const PJconsts &) = delete;
};




/* Parameter list (a copy of the +proj=... etc. parameters) */
struct ARG_list {
    paralist *next;
    char used;
#if (defined(__GNUC__) && __GNUC__ >= 8) || (defined(__clang__) && __clang_major__ >= 9)
    char param[]; /* variable-length member */
    /* Safer to use [] for gcc 8. See https://github.com/OSGeo/proj.4/pull/1087 */
    /* and https://gcc.gnu.org/bugzilla/show_bug.cgi?id=86914 */
#else
    char param[1]; /* variable-length member */
#endif
};


typedef union { double  f; int  i; char *s; } PROJVALUE;


struct PJ_DATUMS {
    const char    *id;           /* datum keyword */
    const char    *defn;         /* ie. "to_wgs84=..." */
    const char    *ellipse_id;   /* ie from ellipse table */
    const char    *comments;     /* EPSG code, etc */
};


struct DERIVS {
    double x_l, x_p;       /* derivatives of x for lambda-phi */
    double y_l, y_p;       /* derivatives of y for lambda-phi */
};

struct FACTORS {
    struct DERIVS der;
    double h, k;           /* meridional, parallel scales */
    double omega, thetap;  /* angular distortion, theta prime */
    double conv;           /* convergence */
    double s;              /* areal scale factor */
    double a, b;           /* max-min scale error */
    int    code;           /* always 0 */
};

// Legacy
struct projFileAPI_t;

struct projCppContext;

struct projNetworkCallbacksAndData
{
    bool enabled = false;
    bool enabled_env_variable_checked = false; // whereas we have checked PROJ_NETWORK env variable
    proj_network_open_cbk_type open = nullptr;
    proj_network_close_cbk_type close = nullptr;
    proj_network_get_header_value_cbk_type get_header_value = nullptr;
    proj_network_read_range_type read_range = nullptr;
    void* user_data = nullptr;
};

struct projGridChunkCache
{
    bool enabled = true;
    std::string filename{};
    long long max_size = 300 * 1024 * 1024;
    int ttl = 86400; // 1 day
};

struct projFileApiCallbackAndData
{
    PROJ_FILE_HANDLE* (*open_cbk)(PJ_CONTEXT *ctx, const char *filename, PROJ_OPEN_ACCESS access, void* user_data) = nullptr;
    size_t           (*read_cbk)(PJ_CONTEXT *ctx, PROJ_FILE_HANDLE*, void* buffer, size_t size, void* user_data) = nullptr;
    size_t           (*write_cbk)(PJ_CONTEXT *ctx, PROJ_FILE_HANDLE*, const void* buffer, size_t size, void* user_data) = nullptr;
    int              (*seek_cbk)(PJ_CONTEXT *ctx, PROJ_FILE_HANDLE*, long long offset, int whence, void* user_data) = nullptr;
    unsigned long long (*tell_cbk)(PJ_CONTEXT *ctx, PROJ_FILE_HANDLE*, void* user_data) = nullptr;
    void             (*close_cbk)(PJ_CONTEXT *ctx, PROJ_FILE_HANDLE*, void* user_data) = nullptr;

    int (*exists_cbk)(PJ_CONTEXT *ctx, const char *filename, void* user_data) = nullptr;
    int (*mkdir_cbk)(PJ_CONTEXT *ctx, const char *filename, void* user_data) = nullptr;
    int (*unlink_cbk)(PJ_CONTEXT *ctx, const char *filename, void* user_data) = nullptr;
    int (*rename_cbk)(PJ_CONTEXT *ctx, const char *oldPath, const char *newPath, void* user_data) = nullptr;

    void*            user_data = nullptr;
};

/* proj thread context */
struct pj_ctx{
    std::string lastFullErrorMessage{}; // used by proj_context_errno_string
    int     last_errno = 0;
    int     debug_level = 0;
    void    (*logger)(void *, int, const char *) = nullptr;
    void    *logger_app_data = nullptr;
    struct projCppContext* cpp_context = nullptr; /* internal context for C++ code */
    int     use_proj4_init_rules = -1; /* -1 = unknown, 0 = no, 1 = yes */
    int     epsg_file_exists = -1; /* -1 = unknown, 0 = no, 1 = yes */
    std::string ca_bundle_path{};

    std::string env_var_proj_lib{}; // content of PROJ_LIB environment variable. Use Filemanager::getProjLibEnvVar() to access
    std::vector<std::string> search_paths{};
    const char **c_compat_paths = nullptr; // same, but for projinfo usage

    const char* (*file_finder_legacy) (const char*) = nullptr; // Only for proj_api compat. To remove once it is removed
    const char* (*file_finder) (PJ_CONTEXT *, const char*, void* user_data) = nullptr;
    void* file_finder_user_data = nullptr;

    bool defer_grid_opening = false; // set transiently by pj_obj_create()

    projFileApiCallbackAndData fileApi{};
    std::string custom_sqlite3_vfs_name{};
    std::string user_writable_directory{};

    // BEGIN ini file settings
    bool iniFileLoaded = false;
    std::string endpoint{};
    projNetworkCallbacksAndData networking{};
    projGridChunkCache gridChunkCache{};
    TMercAlgo defaultTmercAlgo = TMercAlgo::PODER_ENGSAGER; // can be overridden by content of proj.ini
    // END ini file settings

    int projStringParserCreateFromPROJStringRecursionCounter = 0; // to avoid potential infinite recursion in PROJStringParser::createFromPROJString()
    int pipelineInitRecursiongCounter = 0; // to avoid potential infinite recursion in pipeline.cpp


    pj_ctx() = default;
    pj_ctx(const pj_ctx&);
    ~pj_ctx();

    pj_ctx& operator= (const pj_ctx&) = delete;

    projCppContext* get_cpp_context();
    void safeAutoCloseDbIfNeeded();
    void set_search_paths(const std::vector<std::string>& search_paths_in);
    void set_ca_bundle_path(const std::string& ca_bundle_path_in);

    static pj_ctx createDefault();
};

/* Generate pj_list external or make list from include file */
#ifndef PJ_DATUMS__
C_NAMESPACE_VAR struct PJ_DATUMS pj_datums[];
#endif





#ifdef PJ_LIB__
#define PROJ_HEAD(name, desc) static const char des_##name [] = desc

#define OPERATION(name, NEED_ELLPS)                          \
                                                             \
pj_projection_specific_setup_##name (PJ *P);                 \
C_NAMESPACE PJ *pj_##name (PJ *P);                           \
                                                             \
C_NAMESPACE_VAR const char * const pj_s_##name = des_##name; \
                                                             \
C_NAMESPACE PJ *pj_##name (PJ *P) {                          \
    if (P)                                                   \
        return pj_projection_specific_setup_##name (P);      \
    P = pj_new();                                            \
    if (nullptr==P)                                          \
        return nullptr;                                      \
    P->short_name = #name;                                   \
    P->descr = des_##name;                                   \
    P->need_ellps = NEED_ELLPS;                              \
    P->left  = PJ_IO_UNITS_RADIANS;                          \
    P->right = PJ_IO_UNITS_CLASSIC;                          \
    return P;                                                \
}                                                            \
                                                             \
PJ *pj_projection_specific_setup_##name (PJ *P)

/* In ISO19000 lingo, an operation is either a conversion or a transformation */
#define CONVERSION(name, need_ellps)      OPERATION (name, need_ellps)
#define TRANSFORMATION(name, need_ellps)  OPERATION (name, need_ellps)

/* In PROJ.4 a projection is a conversion taking angular input and giving scaled linear output */
#define PROJECTION(name) CONVERSION (name, 1)

#endif /* def PJ_LIB__ */

/* procedure prototypes */
double PROJ_DLL dmstor(const char *, char **);
double dmstor_ctx(PJ_CONTEXT *ctx, const char *, char **);
void   PROJ_DLL set_rtodms(int, int);
char  PROJ_DLL *rtodms(char *, double, int, int);
double PROJ_DLL adjlon(double);
double aacos(PJ_CONTEXT *,double);
double aasin(PJ_CONTEXT *,double);
double asqrt(double);
double aatan2(double, double);

PROJVALUE PROJ_DLL pj_param(PJ_CONTEXT *ctx, paralist *, const char *);
paralist PROJ_DLL *pj_param_exists (paralist *list, const char *parameter);
paralist PROJ_DLL *pj_mkparam(const char *);
paralist *pj_mkparam_ws (const char *str, const char **next_str);


int PROJ_DLL pj_ell_set(PJ_CONTEXT *ctx, paralist *, double *, double *);
int pj_datum_set(PJ_CONTEXT *,paralist *, PJ *);
int pj_angular_units_set(paralist *, PJ *);

paralist *pj_clone_paralist( const paralist* );
paralist *pj_search_initcache( const char *filekey );
void      pj_insert_initcache( const char *filekey, const paralist *list);
paralist *pj_expand_init(PJ_CONTEXT *ctx, paralist *init);

void     *free_params (PJ_CONTEXT *ctx, paralist *start, int errlev);


double *pj_enfn(double);
double  pj_mlfn(double, double, double, const double *);
double  pj_inv_mlfn(PJ_CONTEXT *, double, double, const double *);
double  pj_qsfn(double, double, double);
double  pj_tsfn(double, double, double);
double  pj_msfn(double, double, double);
double  PROJ_DLL pj_phi2(PJ_CONTEXT *, const double, const double);
double  pj_sinhpsi2tanphi(PJ_CONTEXT *, const double, const double);
double  pj_qsfn_(double, PJ *);
double *pj_authset(double);
double  pj_authlat(double, double *);

COMPLEX pj_zpoly1(COMPLEX, const COMPLEX *, int);
COMPLEX pj_zpolyd1(COMPLEX, const COMPLEX *, int, COMPLEX *);

int pj_deriv(PJ_LP, double, const PJ *, struct DERIVS *);
int pj_factors(PJ_LP, const PJ *, double, struct FACTORS *);

void  *proj_mdist_ini(double);
double proj_mdist(double, double, double, const void *);
double proj_inv_mdist(PJ_CONTEXT *ctx, double, const void *);
void  *pj_gauss_ini(double, double, double *,double *);
PJ_LP     pj_gauss(PJ_CONTEXT *, PJ_LP, const void *);
PJ_LP     pj_inv_gauss(PJ_CONTEXT *, PJ_LP, const void *);

struct PJ_DATUMS           PROJ_DLL *pj_get_datums_ref( void );

PJ *pj_new(void);
PJ *pj_default_destructor (PJ *P, int errlev);

double PROJ_DLL pj_atof( const char* nptr );
double pj_strtod( const char *nptr, char **endptr );
void   pj_freeup_plain (PJ *P);

PJ* pj_init_ctx_with_allow_init_epsg( PJ_CONTEXT *ctx, int argc, char **argv, int allow_init_epsg );

std::string PROJ_DLL pj_add_type_crs_if_needed(const std::string& str);
std::string pj_double_quote_string_param_if_needed(const std::string& str);

PJ *pj_create_internal (PJ_CONTEXT *ctx, const char *definition);
PJ *pj_create_argv_internal (PJ_CONTEXT *ctx, int argc, char **argv);

// For use by projinfo
void pj_load_ini(PJ_CONTEXT* ctx);

// Exported for testing purposes only
std::string PROJ_DLL pj_context_get_grid_cache_filename(PJ_CONTEXT *ctx);

// For use by projsync
void PROJ_DLL pj_context_set_user_writable_directory(PJ_CONTEXT* ctx, const std::string& path);
std::string PROJ_DLL pj_get_relative_share_proj(PJ_CONTEXT *ctx);

std::vector<CoordOperation> pj_create_prepared_operations(PJ_CONTEXT *ctx,
                                                     const PJ *source_crs,
                                                     const PJ *target_crs,
                                                     PJ_OBJ_LIST* op_list);

int pj_get_suggested_operation(PJ_CONTEXT *ctx,
                               const std::vector<CoordOperation>& opList,
                               const int iExcluded[2],
                               PJ_DIRECTION direction,
                               PJ_COORD coord);

const PJ_UNITS *pj_list_linear_units();
const PJ_UNITS *pj_list_angular_units();

void pj_clear_hgridshift_knowngrids_cache();
void pj_clear_vgridshift_knowngrids_cache();

PJ_LP pj_generic_inverse_2d(PJ_XY xy, PJ *P, PJ_LP lpInitial);




/*****************************************************************************/
/*                                                                           */
/*                              proj_api.h                                   */
/*                                                                           */
/*    The rest of this header file includes what used to be "proj_api.h"     */
/*                                                                           */
/*****************************************************************************/

/* pj_init() and similar functions can be used with a non-C locale */
/* Can be detected too at runtime if the symbol pj_atof exists */
#define PJ_LOCALE_SAFE 1

#define RAD_TO_DEG    57.295779513082321
#define DEG_TO_RAD   .017453292519943296




extern char const PROJ_DLL pj_release[]; /* global release id string */

/* procedure prototypes */

PJ_CONTEXT PROJ_DLL *pj_get_default_ctx(void);
PJ_CONTEXT *pj_get_ctx( PJ *);

PJ_XY PROJ_DLL pj_fwd(PJ_LP, PJ *);
PJ_LP PROJ_DLL pj_inv(PJ_XY, PJ *);

PJ_XYZ pj_fwd3d(PJ_LPZ, PJ *);
PJ_LPZ pj_inv3d(PJ_XYZ, PJ *);


void pj_clear_initcache(void);
void PROJ_DLL pj_pr_list(PJ *); /* used by proj.cpp */
char *pj_get_def(PJ *, int);
int pj_has_inverse(PJ *);


char *pj_strdup(const char *str);
const char PROJ_DLL *pj_get_release(void);
void pj_acquire_lock(void);
void pj_release_lock(void);
void pj_cleanup_lock(void);

void pj_log( PJ_CONTEXT * ctx, int level, const char *fmt, ... );
void pj_stderr_logger( void *, int, const char * );

int pj_find_file(PJ_CONTEXT * ctx, const char *short_filename,
                 char* out_full_filename, size_t out_full_filename_size);



#endif /* ndef PROJ_INTERNAL_H */
