/******************************************************************************
 * Project:  PROJ.4
 * Purpose:  Implement a (currently minimalistic) proj API based primarily
 *           on the PJ_COORD 4D geodetic spatiotemporal data type.
 *
 * Author:   Thomas Knudsen,  thokn@sdfe.dk,  2016-06-09/2016-11-06
 *
 ******************************************************************************
 * Copyright (c) 2016, 2017 Thomas Knudsen/SDFE
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
#include <stddef.h>
#include <errno.h>
#include <proj.h>
#include "proj_internal.h"
#include "projects.h"
#include "geodesic.h"


/* Initialize PJ_COORD struct */
PJ_COORD proj_coord (double x, double y, double z, double t) {
    PJ_COORD res;
    res.v[0] = x;
    res.v[1] = y;
    res.v[2] = z;
    res.v[3] = t;
    return res;
}

/*****************************************************************************/
int proj_angular_input (PJ *P, enum PJ_DIRECTION dir) {
/******************************************************************************
    Returns 1 if the operator P expects angular input coordinates when
    operating in direction dir, 0 otherwise.
    dir: {PJ_FWD, PJ_INV}
******************************************************************************/
    if (PJ_FWD==dir)
        return pj_left (P)==PJ_IO_UNITS_RADIANS;
    return pj_right (P)==PJ_IO_UNITS_RADIANS;
}

/*****************************************************************************/
int proj_angular_output (PJ *P, enum PJ_DIRECTION dir) {
/******************************************************************************
    Returns 1 if the operator P provides angular output coordinates when
    operating in direction dir, 0 otherwise.
    dir: {PJ_FWD, PJ_INV}
******************************************************************************/
    return proj_angular_input (P, -dir);
}


/* Geodesic distance (in meter) between two points with angular 2D coordinates */
double proj_lp_dist (const PJ *P, LP a, LP b) {
    double s12, azi1, azi2;
    /* Note: the geodesic code takes arguments in degrees */
    geod_inverse (P->geod, PJ_TODEG(a.phi), PJ_TODEG(a.lam), PJ_TODEG(b.phi), PJ_TODEG(b.lam), &s12, &azi1, &azi2);
    return s12;
}

/* The geodesic distance AND the vertical offset */
double proj_lpz_dist (const PJ *P, LPZ a, LPZ b) {
    PJ_COORD aa, bb;
    aa.lpz = a;
    bb.lpz = b;
    if (HUGE_VAL==a.lam || HUGE_VAL==b.lam)
        return HUGE_VAL;
    return hypot (proj_lp_dist (P, aa.lp, bb.lp), a.z - b.z);
}

/* Euclidean distance between two points with linear 2D coordinates */
double proj_xy_dist (XY a, XY b) {
    return hypot (a.x - b.x, a.y - b.y);
}

/* Euclidean distance between two points with linear 3D coordinates */
double proj_xyz_dist (XYZ a, XYZ b) {
    return hypot (hypot (a.x - b.x, a.y - b.y), a.z - b.z);
}



/* Measure numerical deviation after n roundtrips fwd-inv (or inv-fwd) */
double proj_roundtrip (PJ *P, PJ_DIRECTION direction, int n, PJ_COORD *coo) {
    int i;
    PJ_COORD o, u, org;

    if (0==P)
        return HUGE_VAL;

    if (n < 1) {
        proj_errno_set (P, EINVAL);
        return HUGE_VAL;
    }

    /* in the first half-step, we generate the output value */
    u = org = *coo;
    o = *coo = proj_trans (P, direction, u);

    /* now we take n-1 full steps */
    for (i = 0;  i < n - 1;  i++) {
        u = proj_trans (P, -direction, o);
        o = proj_trans (P,  direction, u);
    }

    /* finally, we take the last half-step */
    u = proj_trans (P, -direction, o);

    /* checking for angular *input* since we do a roundtrip, and end where we begin */
    if (proj_angular_input (P, direction))
        return proj_lpz_dist (P, org.lpz, u.lpz);

    return proj_xyz_dist (org.xyz, u.xyz);
}



/**************************************************************************************/
PJ_COORD proj_trans (PJ *P, PJ_DIRECTION direction, PJ_COORD coo) {
/***************************************************************************************
Apply the transformation P to the coordinate coo, preferring the 4D interfaces if
available.

See also pj_approx_2D_trans and pj_approx_3D_trans in pj_internal.c, which work
similarly, but prefers the 2D resp. 3D interfaces if available.
***************************************************************************************/
    if (0==P)
        return coo;
    if (P->inverted)
        direction = -direction;

    switch (direction) {
        case PJ_FWD:
            return pj_fwd4d (coo, P);
        case PJ_INV:
            return  pj_inv4d (coo, P);
        case PJ_IDENT:
            return coo;
        default:
            break;
    }

    proj_errno_set (P, EINVAL);
    return proj_coord_error ();
}



/*****************************************************************************/
int proj_trans_array (PJ *P, PJ_DIRECTION direction, size_t n, PJ_COORD *coord) {
/******************************************************************************
    Batch transform an array of PJ_COORD.

    Returns 0 if all coordinates are transformed without error, otherwise
    returns error number.
******************************************************************************/
    size_t i;

    for (i = 0;  i < n;  i++) {
        coord[i] = proj_trans (P, direction, coord[i]);
        if (proj_errno(P))
            return proj_errno (P);
    }

   return 0;
}



/*************************************************************************************/
size_t proj_trans_generic (
    PJ *P,
    PJ_DIRECTION direction,
    double *x, size_t sx, size_t nx,
    double *y, size_t sy, size_t ny,
    double *z, size_t sz, size_t nz,
    double *t, size_t st, size_t nt
) {
/**************************************************************************************

    Transform a series of coordinates, where the individual coordinate dimension
    may be represented by an array that is either

        1. fully populated
        2. a null pointer and/or a length of zero, which will be treated as a
           fully populated array of zeroes
        3. of length one, i.e. a constant, which will be treated as a fully
           populated array of that constant value

    The strides, sx, sy, sz, st, represent the step length, in bytes, between
    consecutive elements of the corresponding array. This makes it possible for
    proj_transform to handle transformation of a large class of application
    specific data structures, without necessarily understanding the data structure
    format, as in:

        typedef struct {double x, y; int quality_level; char surveyor_name[134];} XYQS;
        XYQS survey[345];
        double height = 23.45;
        PJ *P = {...};
        size_t stride = sizeof (XYQS);
        ...
        proj_transform (
            P, PJ_INV, sizeof(XYQS),
            &(survey[0].x), stride, 345,  (*  We have 345 eastings  *)
            &(survey[0].y), stride, 345,  (*  ...and 345 northings. *)
            &height, 1,                   (*  The height is the constant  23.45 m *)
            0, 0                          (*  and the time is the constant 0.00 s *)
        );

    This is similar to the inner workings of the pj_transform function, but the
    stride functionality has been generalized to work for any size of basic unit,
    not just a fixed number of doubles.

    In most cases, the stride will be identical for x, y,z, and t, since they will
    typically be either individual arrays (stride = sizeof(double)), or strided
    views into an array of application specific data structures (stride = sizeof (...)).

    But in order to support cases where x, y, z, and t come from heterogeneous
    sources, individual strides, sx, sy, sz, st, are used.

    Caveat: Since proj_transform does its work *in place*, this means that even the
    supposedly constants (i.e. length 1 arrays) will return from the call in altered
    state. Hence, remember to reinitialize between repeated calls.

    Return value: Number of transformations completed.

**************************************************************************************/
    PJ_COORD coord = {{0,0,0,0}};
    size_t i, nmin;
    double null_broadcast = 0;

    if (0==P)
        return 0;

    if (P->inverted)
        direction = -direction;

    /* ignore lengths of null arrays */
    if (0==x) nx = 0;
    if (0==y) ny = 0;
    if (0==z) nz = 0;
    if (0==t) nt = 0;

    /* and make the nullities point to some real world memory for broadcasting nulls */
    if (0==nx) x = &null_broadcast;
    if (0==ny) y = &null_broadcast;
    if (0==nz) z = &null_broadcast;
    if (0==nt) t = &null_broadcast;

    /* nothing to do? */
    if (0==nx+ny+nz+nt)
        return 0;

    /* arrays of length 1 are constants, which we broadcast along the longer arrays */
    /* so we need to find the length of the shortest non-unity array to figure out  */
    /* how many coordinate pairs we must transform */
    nmin = (nx > 1)? nx: (ny > 1)? ny: (nz > 1)? nz: (nt > 1)? nt: 1;
    if ((nx > 1) && (nx < nmin))  nmin = nx;
    if ((ny > 1) && (ny < nmin))  nmin = ny;
    if ((nz > 1) && (nz < nmin))  nmin = nz;
    if ((nt > 1) && (nt < nmin))  nmin = nt;

    /* Check validity of direction flag */
    switch (direction) {
        case PJ_FWD:
        case PJ_INV:
            break;
        case PJ_IDENT:
            return nmin;
        default:
            proj_errno_set (P, EINVAL);
            return 0;
    }

    /* Arrays of length==0 are broadcast as the constant 0               */
    /* Arrays of length==1 are broadcast as their single value           */
    /* Arrays of length >1 are iterated over (for the first nmin values) */
    /* The slightly convolved incremental indexing is used due           */
    /* to the stride, which may be any size supported by the platform    */
    for (i = 0;  i < nmin;  i++) {
        coord.xyzt.x = *x;
        coord.xyzt.y = *y;
        coord.xyzt.z = *z;
        coord.xyzt.t = *t;

        if (PJ_FWD==direction)
            coord = pj_fwd4d (coord, P);
        else
            coord = pj_inv4d (coord, P);

        /* in all full length cases, we overwrite the input with the output,  */
        /* and step on to the next element.                                   */
        /* The casts are somewhat funky, but they compile down to no-ops and  */
        /* they tell compilers and static analyzers that we know what we do   */
        if (nx > 1)  {
           *x = coord.xyzt.x;
            x = (double *) ((void *) ( ((char *) x) + sx));
        }
        if (ny > 1)  {
           *y = coord.xyzt.y;
            y = (double *) ((void *) ( ((char *) y) + sy));
        }
        if (nz > 1)  {
           *z = coord.xyzt.z;
            z = (double *) ((void *) ( ((char *) z) + sz));
        }
        if (nt > 1)  {
           *t = coord.xyzt.t;
            t = (double *) ((void *) ( ((char *) t) + st));
        }
    }

    /* Last time around, we update the length 1 cases with their transformed alter egos */
    if (nx==1)
        *x = coord.xyzt.x;
    if (ny==1)
        *y = coord.xyzt.y;
    if (nz==1)
        *z = coord.xyzt.z;
    if (nt==1)
        *t = coord.xyzt.t;

    return i;
}


/*************************************************************************************/
PJ_COORD proj_geoc_lat (const PJ *P, PJ_DIRECTION direction, PJ_COORD coo) {
/**************************************************************************************
    Convert geographical latitude to geocentric (or the other way round if
    direction = PJ_INV)

    The conversion involves a call to the tangent function, which goes through the
    roof at the poles, so very close (the last centimeter) to the poles no
    conversion takes place and the input latitude is copied directly to the output.

    Fortunately, the geocentric latitude converges to the geographical at the
    poles, so the difference is negligible.

    For the spherical case, the geographical latitude equals the geocentric, and
    consequently, the input is copied directly to the output.
**************************************************************************************/
    const double limit = M_HALFPI - 1e-9;
    PJ_COORD res = coo;
    if ((coo.lp.phi > limit) || (coo.lp.phi < -limit) || (P->es==0))
        return res;
    if (direction==PJ_FWD)
        res.lp.phi = atan (P->one_es * tan (coo.lp.phi) );
    else
        res.lp.phi = atan (P->rone_es * tan (coo.lp.phi) );

    return res;
}

double proj_torad (double angle_in_degrees) { return PJ_TORAD (angle_in_degrees);}
double proj_todeg (double angle_in_radians) { return PJ_TODEG (angle_in_radians);}

double proj_dmstor(const char *is, char **rs) {
    return dmstor(is, rs);
}

char*  proj_rtodms(char *s, double r, int pos, int neg) {
    return rtodms(s, r, pos, neg);
}




/*************************************************************************************/
PJ *proj_create (PJ_CONTEXT *ctx, const char *definition) {
/**************************************************************************************
    Create a new PJ object in the context ctx, using the given definition. If ctx==0,
    the default context is used, if definition==0, or invalid, a null-pointer is
    returned. The definition may use '+' as argument start indicator, as in
    "+proj=utm +zone=32", or leave it out, as in "proj=utm zone=32".

    It may even use free formatting "proj  =  utm;  zone  =32  ellps= GRS80".
    Note that the semicolon separator is allowed, but not required.
**************************************************************************************/
    PJ   *P;
    char *args, **argv;
    size_t argc, n;

    if (0==ctx)
        ctx = pj_get_default_ctx ();

    /* Make a copy that we can manipulate */
    n = strlen (definition);
    args = (char *) malloc (n + 1);
    if (0==args)
        return 0;
    strcpy (args, definition);

    argc = pj_trim_argc (args);
    if (argc==0) {
        pj_dealloc (args);
        return 0;
    }

    argv = pj_trim_argv (argc, args);

    /* ...and let pj_init_ctx do the hard work */
    P = pj_init_ctx (ctx, (int) argc, argv);

    pj_dealloc (argv);
    pj_dealloc (args);
    return P;
}



/*************************************************************************************/
PJ *proj_create_argv (PJ_CONTEXT *ctx, int argc, char **argv) {
/**************************************************************************************
Create a new PJ object in the context ctx, using the given definition argument
array argv. If ctx==0, the default context is used, if definition==0, or invalid,
a null-pointer is returned. The definition arguments may use '+' as argument start
indicator, as in {"+proj=utm", "+zone=32"}, or leave it out, as in {"proj=utm",
"zone=32"}.
**************************************************************************************/
    PJ *P;
    const char *c;

    if (0==argv)
        return 0;
    if (0==ctx)
        ctx = pj_get_default_ctx ();

    /* We assume that free format is used, and build a full proj_create compatible string */
    c = pj_make_args (argc, argv);
    if (0==c)
        return 0;

    P = proj_create (ctx, c);
    pj_dealloc ((char *) c);
    return P;
}



/*****************************************************************************/
PJ  *proj_create_crs_to_crs (PJ_CONTEXT *ctx, const char *srid_from, const char *srid_to, PJ_AREA *area) {
/******************************************************************************
    Create a transformation pipeline between two known coordinate reference
    systems.

    srid_from and srid_to should be the value part of a +init=... parameter
    set, i.e. "epsg:25833" or "IGNF:AMST63". Any projection definition that
    can be found in a init-file in PROJ_LIB is a valid input to this function.

    For now the function mimics the cs2cs app: An input and an output CRS is
    given and coordinates are transformed via a hub datum (WGS84). This
    transformation strategy is referred to as "early-binding" by the EPSG. The
    function can be extended to support "late-binding" transformations in the
    future without affecting users of the function.

    An "area of use" can be specified in area. In the current version of this
    function is has no function, but is added in anticipation of a
    "late-binding" implementation in the future. The idea being, that if a user
    supplies an area of use, the more accurate transformation between two given
    systems can be chosen.

    Example call:

        PJ *P = proj_create_crs_to_crs(0, "epsg:25832", "epsg:25833", NULL);

******************************************************************************/
    PJ *P;
    char buffer[512];

    /* area not in use yet, suppressing warning */
    (void)area;

    strcpy(buffer, "+proj=pipeline +step +init=");
    strncat(buffer, srid_from, 512-strlen(buffer));
    strncat(buffer, " +inv +step +init=", 512-strlen(buffer));
    strncat(buffer, srid_to, 512-strlen(buffer));

    P = proj_create(ctx, buffer);

    return P;
}

PJ *proj_destroy (PJ *P) {
    pj_free (P);
    return 0;
}

int proj_errno (const PJ *P) {
    return pj_ctx_get_errno (pj_get_ctx ((PJ *) P));
}

/*****************************************************************************/
int proj_errno_set (const PJ *P, int err) {
/******************************************************************************
    Set context-errno, bubble it up to the thread local errno, return err
******************************************************************************/
    /* Use proj_errno_reset to explicitly clear the error status */
    if (0==err)
        return 0;

    /* For P==0 err goes to the default context */
    proj_context_errno_set (pj_get_ctx ((PJ *) P), err);
    errno = err;
    return err;
}

/*****************************************************************************/
int proj_errno_restore (const PJ *P, int err) {
/******************************************************************************
    Use proj_errno_restore when the current function succeeds, but the
    error flag was set on entry, and stored/reset using proj_errno_reset
    in order to monitor for new errors.

    See usage example under proj_errno_reset ()
******************************************************************************/
    if (0==err)
        return 0;
    proj_errno_set (P, err);
    return 0;
}

/*****************************************************************************/
int proj_errno_reset (const PJ *P) {
/******************************************************************************
    Clears errno in the context and thread local levels
    through the low level pj_ctx interface.

    Returns the previous value of the errno, for convenient reset/restore
    operations:

    int foo (PJ *P) {
        // errno may be set on entry, but we need to reset it to be able to
        // check for errors from "do_something_with_P(P)"
        int last_errno = proj_errno_reset (P);

        // local failure
        if (0==P)
            return proj_errno_set (P, 42);

        // call to function that may fail
        do_something_with_P (P);

        // failure in do_something_with_P? - keep latest error status
        if (proj_errno(P))
            return proj_errno (P);

        // success - restore previous error status, return 0
        return proj_errno_restore (P, last_errno);
    }
******************************************************************************/
    int last_errno;
    last_errno = proj_errno (P);

    pj_ctx_set_errno (pj_get_ctx ((PJ *) P), 0);
    errno = 0;
    return last_errno;
}


/* Create a new context */
PJ_CONTEXT *proj_context_create (void) {
    return pj_ctx_alloc ();
}


PJ_CONTEXT *proj_context_destroy (PJ_CONTEXT *ctx) {
    if (0==ctx)
        return 0;

    /* Trying to free the default context is a no-op (since it is statically allocated) */
    if (pj_get_default_ctx ()==ctx)
        return 0;

    pj_ctx_free (ctx);
    return 0;
}


/*****************************************************************************/
PJ_INFO proj_info(void) {
/******************************************************************************
    Basic info about the current instance of the PROJ.4 library.

    Returns PJ_INFO struct. Searchpath member of the struct is truncated to 512
    characters.

******************************************************************************/
    PJ_INFO info;
    const char * const *paths;
    char *tmpstr;
    int i, n;
    size_t len = 0;

    memset(&info, 0, sizeof(PJ_INFO));

    info.major = PROJ_VERSION_MAJOR;
    info.minor = PROJ_VERSION_MINOR;
    info.patch = PROJ_VERSION_PATCH;

    /* This is a controlled environment, so no risk of sprintf buffer
       overflow. A normal version string is xx.yy.zz which is 8 characters
       long and there is room for 64 bytes in the version string. */
    sprintf(info.version, "%d.%d.%d", info.major, info.minor, info.patch);

    pj_strlcpy(info.release, pj_get_release(), sizeof(info.release));


    /* build search path string */
    tmpstr = getenv("HOME");
    if (tmpstr != NULL) {
        pj_strlcpy(info.searchpath, tmpstr, sizeof(info.searchpath));
    }

    tmpstr = getenv("PROJ_LIB");
    if (tmpstr != NULL) {
        if (strlen(info.searchpath) != 0) {
            /* $HOME already in path */
            strcat(info.searchpath, ";");
            len = strlen(tmpstr);
            strncat(info.searchpath, tmpstr, sizeof(info.searchpath)-len-1);
        } else {
            /* path is empty */
            pj_strlcpy(info.searchpath, tmpstr, sizeof(info.searchpath));
        }
    }

    paths = proj_get_searchpath();
    n = proj_get_path_count();

    for (i=0; i<n; i++) {
        if (strlen(info.searchpath)+strlen(paths[i]) >= 511)
            continue;

        if (strlen(info.searchpath) != 0) {
            strcat(info.searchpath, ";");
            len = strlen(paths[i]);
            strncat(info.searchpath, paths[i], sizeof(info.searchpath)-len-1);
        } else {
            pj_strlcpy(info.searchpath, paths[i], sizeof(info.searchpath));
        }
    }

    return info;
}


/*****************************************************************************/
PJ_PROJ_INFO proj_pj_info(PJ *P) {
/******************************************************************************
    Basic info about a particular instance of a projection object.

    Returns PJ_PROJ_INFO struct.

******************************************************************************/
    PJ_PROJ_INFO info;
    char *def;

    memset(&info, 0, sizeof(PJ_PROJ_INFO));

    /* Expected accuracy of the transformation. Hardcoded for now, will be improved */
    /* later. Most likely to be used when a transformation is set up with           */
    /* proj_create_crs_to_crs in a future version that leverages the EPSG database. */
    info.accuracy = -1.0;

    if (!P) {
        return info;
    }

    /* projection id */
    if (pj_param(P->ctx, P->params, "tproj").i)
        pj_strlcpy(info.id, pj_param(P->ctx, P->params, "sproj").s, sizeof(info.id));

    /* projection description */
    pj_strlcpy(info.description, P->descr, sizeof(info.description));

    /* projection definition */
    def = pj_get_def(P, 0); /* pj_get_def takes a non-const PJ pointer */
    pj_strlcpy(info.definition, &def[1], sizeof(info.definition)); /* def includes a leading space */
    pj_dealloc(def);

    /* this does not take into account that a pipeline potentially does not */
    /* have an inverse.                                                     */
    info.has_inverse = (P->inv != 0 || P->inv3d != 0 || P->inv4d != 0);

    return info;
}


/*****************************************************************************/
PJ_GRID_INFO proj_grid_info(const char *gridname) {
/******************************************************************************
    Information about a named datum grid.

    Returns PJ_GRID_INFO struct.

******************************************************************************/
    PJ_GRID_INFO info;

    /*PJ_CONTEXT *ctx = proj_context_create(); */
    PJ_CONTEXT *ctx = pj_get_default_ctx();
    PJ_GRIDINFO *gridinfo = pj_gridinfo_init(ctx, gridname);
    memset(&info, 0, sizeof(PJ_GRID_INFO));

    /* in case the grid wasn't found */
    if (gridinfo->filename == NULL) {
        pj_gridinfo_free(ctx, gridinfo);
        strcpy(info.format, "missing");
        return info;
    }

    /* name of grid */
    pj_strlcpy(info.gridname, gridname, sizeof(info.gridname));

    /* full path of grid */
    pj_find_file(ctx, gridname, info.filename, sizeof(info.filename));

    /* grid format */
    pj_strlcpy(info.format, gridinfo->format, sizeof(info.format));

    /* grid size */
    info.n_lon = gridinfo->ct->lim.lam;
    info.n_lat = gridinfo->ct->lim.phi;

    /* cell size */
    info.cs_lon = gridinfo->ct->del.lam;
    info.cs_lat = gridinfo->ct->del.phi;

    /* bounds of grid */
    info.lowerleft  = gridinfo->ct->ll;
    info.upperright.lam = info.lowerleft.lam + info.n_lon*info.cs_lon;
    info.upperright.phi = info.lowerleft.phi + info.n_lat*info.cs_lat;

    pj_gridinfo_free(ctx, gridinfo);

    return info;
}



/*****************************************************************************/
PJ_INIT_INFO proj_init_info(const char *initname){
/******************************************************************************
    Information about a named init file.

    Maximum length of initname is 64.

    Returns PJ_INIT_INFO struct.

    If the init file is not found all members of the return struct are set
    to the empty string.

    If the init file is found, but the metadata is missing, the value is
    set to "Unknown".

******************************************************************************/
    int file_found;
    char param[80], key[74];
    paralist *start, *next;
    PJ_INIT_INFO info;
    PJ_CONTEXT *ctx = pj_get_default_ctx();

    memset(&info, 0, sizeof(PJ_INIT_INFO));

    file_found = pj_find_file(ctx, initname, info.filename, sizeof(info.filename));
    if (!file_found || strlen(initname) > 64) {
        return info;
    }

    pj_strlcpy(info.name, initname, sizeof(info.name));
    strcpy(info.origin, "Unknown");
    strcpy(info.version, "Unknown");
    strcpy(info.lastupdate, "Unknown");

    pj_strlcpy(key, initname, 64); /* make room for ":metadata\0" at the end */
    strncat(key, ":metadata", 9);
    strcpy(param, "+init=");
    strncat(param, key, 73);

    start = pj_mkparam(param);
    pj_expand_init(ctx, start);

    if (pj_param(ctx, start, "tversion").i) {
        pj_strlcpy(info.version, pj_param(ctx, start, "sversion").s, sizeof(info.version));
    }

    if (pj_param(ctx, start, "torigin").i) {
        pj_strlcpy(info.origin, pj_param(ctx, start, "sorigin").s, sizeof(info.origin));
    }

    if (pj_param(ctx, start, "tlastupdate").i) {
        pj_strlcpy(info.lastupdate, pj_param(ctx, start, "slastupdate").s, sizeof(info.lastupdate));
    }

    for ( ; start; start = next) {
        next = start->next;
        pj_dalloc(start);
    }

   return info;
}



/*****************************************************************************/
PJ_FACTORS proj_factors(PJ *P, LP lp) {
/******************************************************************************
    Cartographic characteristics at point lp.

    Characteristics include meridian, parallel and areal scales, angular
    distortion, meridian/parallel, meridian convergence and scale error.

    returns PJ_FACTORS. If unsuccessfull error number is set and the returned
    struct contains NULL data.

******************************************************************************/
    PJ_FACTORS factors = {0,0,0, 0,0,0, 0,0};
    struct FACTORS f;

    if (0==P)
        return factors;

    if (pj_factors(lp, P, 0.0, &f))
        return factors;

    factors.meridional_scale  =  f.h;
    factors.parallel_scale    =  f.k;
    factors.areal_scale       =  f.s;

    factors.angular_distortion        =  f.omega;
    factors.meridian_parallel_angle   =  f.thetap;
    factors.meridian_convergence      =  f.conv;

    factors.tissot_semimajor  =  f.a;
    factors.tissot_semiminor  =  f.b;

    return factors;
}

