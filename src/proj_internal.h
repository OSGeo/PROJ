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

#ifndef ACCEPT_USE_OF_DEPRECATED_PROJ_API_H
#define ACCEPT_USE_OF_DEPRECATED_PROJ_API_H
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

#ifdef PROJ_API_H
#error proj_internal.h must be included before proj_api.h
#endif

#ifdef PROJ_RENAME_SYMBOLS
#include "proj_symbol_rename.h"
#endif

#define STATIC_ASSERT(COND) ((void)sizeof(char[(COND) ? 1 : -1]))

#if !defined(HAVE_C99_MATH)
#define HAVE_C99_MATH 0
#endif

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
    PJ_IO_UNITS_RADIANS   = 4   /* Radians */
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


/* Grid functionality */
int             proj_vgrid_init(PJ *P, const char *grids);
int             proj_hgrid_init(PJ *P, const char *grids);
double          proj_vgrid_value(PJ *P, PJ_LP lp, double vmultiplier);
PJ_LP           proj_hgrid_value(PJ *P, PJ_LP lp);
PJ_LP           proj_hgrid_apply(PJ *P, PJ_LP lp, PJ_DIRECTION direction);

void PROJ_DLL proj_log_error (PJ *P, const char *fmt, ...);
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

/* Lowest level: Minimum support for fileapi */
void proj_fileapi_set (PJ *P, void *fileapi);

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

struct projCtx_t;
typedef struct projCtx_t projCtx_t;

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


/* base projection data structure */
struct PJconsts {

    /*************************************************************************************

                         G E N E R A L   P A R A M E T E R   S T R U C T

    **************************************************************************************

        TODO: Need some description here - especially about the thread context...
        This is the struct behind the PJ typedef

    **************************************************************************************/

    projCtx_t *ctx = nullptr;
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
    int  is_pipeline = 0;           /* 1 if PJ represents a pipeline */
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
    struct _pj_gi **gridlist = nullptr;     /* TODO: Description needed */
    int     gridlist_count = 0;

    int     has_geoid_vgrids = 0;      /* TODO: Description needed */
    struct _pj_gi **vgridlist_geoid = nullptr;   /* TODO: Description needed */
    int     vgridlist_geoid_count = 0;

    double  from_greenwich = 0.0;       /* prime meridian offset (in radians) */
    double  long_wrap_center = 0.0;     /* 0.0 for -180 to 180, actually in radians*/
    int     is_long_wrap_set = 0;
    char    axis[4] = {0,0,0,0};        /* Axis order, pj_transform/pj_adjust_axis */

    /* New Datum Shift Grid Catalogs */
    char   *catalog_name = nullptr;
    struct _PJ_GridCatalog *catalog = nullptr;

    double  datum_date = 0.0;           /* TODO: Description needed */

    struct _pj_gi *last_before_grid = nullptr;    /* TODO: Description needed */
    PJ_Region     last_before_region = {0,0,0,0}; /* TODO: Description needed */
    double        last_before_date = 0.0;         /* TODO: Description needed */

    struct _pj_gi *last_after_grid = nullptr;     /* TODO: Description needed */
    PJ_Region     last_after_region = {0,0,0,0};  /* TODO: Description needed */
    double        last_after_date = 0.0;          /* TODO: Description needed */

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

    struct CoordOperation
    {
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

        CoordOperation(double minxSrcIn, double minySrcIn, double maxxSrcIn, double maxySrcIn,
                       double minxDstIn, double minyDstIn, double maxxDstIn, double maxyDstIn,
                       PJ* pjIn, const std::string& nameIn):
            minxSrc(minxSrcIn), minySrc(minySrcIn), maxxSrc(maxxSrcIn), maxySrc(maxySrcIn),
            minxDst(minxDstIn), minyDst(minyDstIn), maxxDst(maxxDstIn), maxyDst(maxyDstIn),
            pj(pjIn), name(nameIn) {}

        CoordOperation(const CoordOperation&) = delete;

        CoordOperation(CoordOperation&& other):
            minxSrc(other.minxSrc), minySrc(other.minySrc), maxxSrc(other.maxxSrc), maxySrc(other.maxySrc),
            minxDst(other.minxDst), minyDst(other.minyDst), maxxDst(other.maxxDst), maxyDst(other.maxyDst),
            name(std::move(other.name)) {
                pj = other.pj;
                other.pj = nullptr;
            }

        CoordOperation& operator=(const CoordOperation&) = delete;

        ~CoordOperation()
        {
            proj_destroy(pj);
        }
    };
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
#if defined(__GNUC__) && __GNUC__ >= 8
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

/* library errors */
#define PJD_ERR_NO_ARGS                  -1
#define PJD_ERR_NO_OPTION_IN_INIT_FILE   -2
#define PJD_ERR_NO_COLON_IN_INIT_STRING  -3
#define PJD_ERR_PROJ_NOT_NAMED           -4
#define PJD_ERR_UNKNOWN_PROJECTION_ID    -5
#define PJD_ERR_INVALID_ECCENTRICITY     -6
#define PJD_ERR_UNKNOWN_UNIT_ID          -7
#define PJD_ERR_INVALID_BOOLEAN_PARAM    -8
#define PJD_ERR_UNKNOWN_ELLP_PARAM       -9
#define PJD_ERR_REV_FLATTENING_IS_ZERO  -10
#define PJD_ERR_REF_RAD_LARGER_THAN_90  -11
#define PJD_ERR_ES_LESS_THAN_ZERO       -12
#define PJD_ERR_MAJOR_AXIS_NOT_GIVEN    -13
#define PJD_ERR_LAT_OR_LON_EXCEED_LIMIT -14
#define PJD_ERR_INVALID_X_OR_Y          -15
#define PJD_ERR_WRONG_FORMAT_DMS_VALUE  -16
#define PJD_ERR_NON_CONV_INV_MERI_DIST  -17
#define PJD_ERR_NON_CON_INV_PHI2        -18
#define PJD_ERR_ACOS_ASIN_ARG_TOO_LARGE -19
#define PJD_ERR_TOLERANCE_CONDITION     -20
#define PJD_ERR_CONIC_LAT_EQUAL         -21
#define PJD_ERR_LAT_LARGER_THAN_90      -22
#define PJD_ERR_LAT1_IS_ZERO            -23
#define PJD_ERR_LAT_TS_LARGER_THAN_90   -24
#define PJD_ERR_CONTROL_POINT_NO_DIST   -25
#define PJD_ERR_NO_ROTATION_PROJ        -26
#define PJD_ERR_W_OR_M_ZERO_OR_LESS     -27
#define PJD_ERR_LSAT_NOT_IN_RANGE       -28
#define PJD_ERR_PATH_NOT_IN_RANGE       -29
#define PJD_ERR_INVALID_H               -30
#define PJD_ERR_K_LESS_THAN_ZERO        -31
#define PJD_ERR_LAT_1_OR_2_ZERO_OR_90   -32
#define PJD_ERR_LAT_0_OR_ALPHA_EQ_90    -33
#define PJD_ERR_ELLIPSOID_USE_REQUIRED  -34
#define PJD_ERR_INVALID_UTM_ZONE        -35
/* -36 no longer used */
#define PJD_ERR_FAILED_TO_FIND_PROJ     -37
#define PJD_ERR_FAILED_TO_LOAD_GRID     -38
#define PJD_ERR_INVALID_M_OR_N          -39
#define PJD_ERR_N_OUT_OF_RANGE          -40
#define PJD_ERR_LAT_1_2_UNSPECIFIED     -41
#define PJD_ERR_ABS_LAT1_EQ_ABS_LAT2    -42
#define PJD_ERR_LAT_0_HALF_PI_FROM_MEAN -43
#define PJD_ERR_UNPARSEABLE_CS_DEF      -44
#define PJD_ERR_GEOCENTRIC              -45
#define PJD_ERR_UNKNOWN_PRIME_MERIDIAN  -46
#define PJD_ERR_AXIS                    -47
#define PJD_ERR_GRID_AREA               -48
#define PJD_ERR_INVALID_SWEEP_AXIS      -49
#define PJD_ERR_MALFORMED_PIPELINE      -50
#define PJD_ERR_UNIT_FACTOR_LESS_THAN_0 -51
#define PJD_ERR_INVALID_SCALE           -52
#define PJD_ERR_NON_CONVERGENT          -53
#define PJD_ERR_MISSING_ARGS            -54
#define PJD_ERR_LAT_0_IS_ZERO           -55
#define PJD_ERR_ELLIPSOIDAL_UNSUPPORTED -56
#define PJD_ERR_TOO_MANY_INITS          -57
#define PJD_ERR_INVALID_ARG             -58
#define PJD_ERR_INCONSISTENT_UNIT       -59
#define PJD_ERR_MUTUALLY_EXCLUSIVE_ARGS -60
#define PJD_ERR_GENERIC_ERROR           -61
/* NOTE: Remember to update src/strerrno.cpp, src/apps/gie.cpp and transient_error in */
/* src/transform.cpp when adding new value */

struct projFileAPI_t;

struct projCppContext;

/* proj thread context */
struct projCtx_t {
    int     last_errno = 0;
    int     debug_level = 0;
    void    (*logger)(void *, int, const char *) = nullptr;
    void    *logger_app_data = nullptr;
    struct projFileAPI_t *fileapi = nullptr;
    struct projCppContext* cpp_context = nullptr; /* internal context for C++ code */
    int     use_proj4_init_rules = -1; /* -1 = unknown, 0 = no, 1 = yes */
    int     epsg_file_exists = -1; /* -1 = unknown, 0 = no, 1 = yes */

    std::vector<std::string> search_paths{};
    const char **c_compat_paths = nullptr; // same, but for projinfo usage

    const char* (*file_finder_legacy) (const char*) = nullptr; // Only for proj_api compat. To remove once it is removed
    const char* (*file_finder) (PJ_CONTEXT *, const char*, void* user_data) = nullptr;
    void* file_finder_user_data = nullptr;

    int projStringParserCreateFromPROJStringRecursionCounter = 0; // to avoid potential infinite recursion in PROJStringParser::createFromPROJString()

    projCtx_t() = default;
    projCtx_t(const projCtx_t&);
    ~projCtx_t();

    projCtx_t& operator= (const projCtx_t&) = delete;

    void set_search_paths(const std::vector<std::string>& search_paths_in);

    static projCtx_t createDefault();
};

/* Generate pj_list external or make list from include file */
#ifndef PJ_DATUMS__
C_NAMESPACE_VAR struct PJ_DATUMS pj_datums[];
#endif





#ifdef PJ_LIB__
#define PROJ_HEAD(name, desc) static const char des_##name [] = desc

#define OPERATION(name, NEED_ELPJ_LPS)                          \
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
    P->descr = des_##name;                                   \
    P->need_ellps = NEED_ELPJ_LPS;                              \
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


#define MAX_TAB_ID 80
typedef struct { float lam, phi; } FLP;
typedef struct { pj_int32 lam, phi; } ILP;

struct CTABLE {
    char id[MAX_TAB_ID];    /* ascii info */
    PJ_LP ll;               /* lower left corner coordinates */
    PJ_LP del;              /* size of cells */
    ILP lim;                /* limits of conversion matrix */
    FLP *cvs;               /* conversion matrix */
};

typedef struct _pj_gi {
    char *gridname;     /* identifying name of grid, eg "conus" or ntv2_0.gsb */
    char *filename;     /* full path to filename */

    const char *format; /* format of this grid, ie "ctable", "ntv1",
                           "ntv2" or "missing". */

    long   grid_offset;  /* offset in file, for delayed loading */
    int   must_swap;    /* only for NTv2 */

    struct CTABLE *ct;

    struct _pj_gi *next;
    struct _pj_gi *child;
} PJ_GRIDINFO;

typedef struct {
    PJ_Region region;
    int  priority;      /* higher used before lower */
    double date;        /* year.fraction */
    char *definition;   /* usually the gridname */

    PJ_GRIDINFO  *gridinfo;
    int available;      /* 0=unknown, 1=true, -1=false */
} PJ_GridCatalogEntry;

typedef struct _PJ_GridCatalog {
    char *catalog_name;

    PJ_Region region;   /* maximum extent of catalog data */

    int entry_count;
    PJ_GridCatalogEntry *entries;

    struct _PJ_GridCatalog *next;
} PJ_GridCatalog;

/* procedure prototypes */
double PROJ_DLL dmstor(const char *, char **);
double dmstor_ctx(projCtx_t *ctx, const char *, char **);
void   PROJ_DLL set_rtodms(int, int);
char  PROJ_DLL *rtodms(char *, double, int, int);
double PROJ_DLL adjlon(double);
double aacos(projCtx_t *,double);
double aasin(projCtx_t *,double);
double asqrt(double);
double aatan2(double, double);

PROJVALUE PROJ_DLL pj_param(projCtx_t *ctx, paralist *, const char *);
paralist PROJ_DLL *pj_param_exists (paralist *list, const char *parameter);
paralist PROJ_DLL *pj_mkparam(const char *);
paralist *pj_mkparam_ws (const char *str, const char **next_str);


int PROJ_DLL pj_ell_set(projCtx_t *ctx, paralist *, double *, double *);
int pj_datum_set(projCtx_t *,paralist *, PJ *);
int pj_angular_units_set(paralist *, PJ *);

paralist *pj_clone_paralist( const paralist* );
paralist *pj_search_initcache( const char *filekey );
void      pj_insert_initcache( const char *filekey, const paralist *list);
paralist *pj_expand_init(projCtx_t *ctx, paralist *init);

void     *pj_dealloc_params (projCtx_t *ctx, paralist *start, int errlev);


double *pj_enfn(double);
double  pj_mlfn(double, double, double, double *);
double  pj_inv_mlfn(projCtx_t *, double, double, double *);
double  pj_qsfn(double, double, double);
double  pj_tsfn(double, double, double);
double  pj_msfn(double, double, double);
double  PROJ_DLL pj_phi2(projCtx_t *, double, double);
double  pj_qsfn_(double, PJ *);
double *pj_authset(double);
double  pj_authlat(double, double *);

COMPLEX pj_zpoly1(COMPLEX, const COMPLEX *, int);
COMPLEX pj_zpolyd1(COMPLEX, const COMPLEX *, int, COMPLEX *);

int pj_deriv(PJ_LP, double, const PJ *, struct DERIVS *);
int pj_factors(PJ_LP, const PJ *, double, struct FACTORS *);

/* nadcon related protos */
PJ_LP             nad_intr(PJ_LP, struct CTABLE *);
PJ_LP             nad_cvt(PJ_LP, int, struct CTABLE *);
struct CTABLE *nad_init(projCtx_t *ctx, char *);
struct CTABLE *nad_ctable_init( projCtx_t *ctx, struct projFileAPI_t* fid );
int            nad_ctable_load( projCtx_t *ctx, struct CTABLE *, struct projFileAPI_t* fid );
struct CTABLE *nad_ctable2_init( projCtx_t *ctx, struct projFileAPI_t* fid );
int            nad_ctable2_load( projCtx_t *ctx, struct CTABLE *, struct projFileAPI_t* fid );
void           nad_free(struct CTABLE *);

/* higher level handling of datum grid shift files */

int pj_apply_vgridshift( PJ *defn, const char *listname,
                         PJ_GRIDINFO ***gridlist_p,
                         int *gridlist_count_p,
                         int inverse,
                         long point_count, int point_offset,
                         double *x, double *y, double *z );
int pj_apply_gridshift_2( PJ *defn, int inverse,
                          long point_count, int point_offset,
                          double *x, double *y, double *z );
int pj_apply_gridshift_3( projCtx_t *ctx,
                          PJ_GRIDINFO **gridlist, int gridlist_count,
                          int inverse, long point_count, int point_offset,
                          double *x, double *y, double *z );

PJ_GRIDINFO **pj_gridlist_from_nadgrids( projCtx_t *, const char *, int * );

PJ_GRIDINFO *pj_gridinfo_init( projCtx_t *, const char * );
int          pj_gridinfo_load( projCtx_t *, PJ_GRIDINFO * );
void         pj_gridinfo_free( projCtx_t *, PJ_GRIDINFO * );

PJ_GridCatalog *pj_gc_findcatalog( projCtx_t *, const char * );
PJ_GridCatalog *pj_gc_readcatalog( projCtx_t *, const char * );
void pj_gc_unloadall( projCtx_t *);
int pj_gc_apply_gridshift( PJ *defn, int inverse,
                           long point_count, int point_offset,
                           double *x, double *y, double *z );
int pj_gc_apply_gridshift( PJ *defn, int inverse,
                           long point_count, int point_offset,
                           double *x, double *y, double *z );

double pj_gc_parsedate( projCtx_t *, const char * );

void  *proj_mdist_ini(double);
double proj_mdist(double, double, double, const void *);
double proj_inv_mdist(projCtx_t *ctx, double, const void *);
void  *pj_gauss_ini(double, double, double *,double *);
PJ_LP     pj_gauss(projCtx_t *, PJ_LP, const void *);
PJ_LP     pj_inv_gauss(projCtx_t *, PJ_LP, const void *);

struct PJ_DATUMS           PROJ_DLL *pj_get_datums_ref( void );

PJ *pj_new(void);
PJ *pj_default_destructor (PJ *P, int errlev);

double PROJ_DLL pj_atof( const char* nptr );
double pj_strtod( const char *nptr, char **endptr );
void   pj_freeup_plain (PJ *P);

PJ* pj_init_ctx_with_allow_init_epsg( projCtx_t *ctx, int argc, char **argv, int allow_init_epsg );

std::string PROJ_DLL pj_add_type_crs_if_needed(const std::string& str);
std::string pj_double_quote_string_param_if_needed(const std::string& str);

PJ *pj_create_internal (PJ_CONTEXT *ctx, const char *definition);
PJ *pj_create_argv_internal (PJ_CONTEXT *ctx, int argc, char **argv);

/* classic public API */
#include "proj_api.h"

#endif /* ndef PROJ_INTERNAL_H */
