/******************************************************************************
 * Project:  PROJ.4
 * Purpose:  Implement a (currently minimalistic) proj API based primarily
 *           on the PJ_OBS generic geodetic data type.
 *
 *           proj thread contexts have not seen widespread use, so one of the
 *           intentions with this new API is to make them less visible on the
 *           API surface: Contexts do not have a life by themselves, they are
 *           visible only through their associated PJs, and the number of
 *           functions supporting them is limited.
 *
 * Author:   Thomas Knudsen,  thokn@sdfe.dk,  2016-06-09/2016-11-06
 *
 ******************************************************************************
 * Copyright (c) 2016, Thomas Knudsen/SDFE
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
#define PJ_OBS_API_C
#include <proj.h>
#include "proj_internal.h"
#include "projects.h"
#include <geodesic.h>
#include <stddef.h>
#include <errno.h>


/* Initialize PJ_COORD struct */
PJ_COORD proj_coord (double x, double y, double z, double t) {
    PJ_COORD res;
    res.v[0] = x;
    res.v[1] = y;
    res.v[2] = z;
    res.v[3] = t;
    return res;
}

/* Initialize PJ_OBS struct */
PJ_OBS proj_obs (double x, double y, double z, double t, double o, double p, double k, int id, unsigned int flags) {
    PJ_OBS res;
    res.coo.v[0] = x;
    res.coo.v[1] = y;
    res.coo.v[2] = z;
    res.coo.v[3] = t;
    res.anc.v[0] = o;
    res.anc.v[1] = p;
    res.anc.v[2] = k;
    res.id       = id;
    res.flags    = flags;

    return res;
}



/* Geodesic distance between two points with angular 2D coordinates */
double proj_lp_dist (const PJ *P, LP a, LP b) {
    double s12, azi1, azi2;
    /* Note: the geodesic code takes arguments in degrees */
    geod_inverse (P->geod, PJ_TODEG(a.phi), PJ_TODEG(a.lam), PJ_TODEG(b.phi), PJ_TODEG(b.lam), &s12, &azi1, &azi2);
    return s12;
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
double proj_roundtrip (PJ *P, PJ_DIRECTION direction, int n, PJ_OBS obs) {
    int i;
    PJ_OBS o, u;

    if (0==P)
        return HUGE_VAL;

    if (n < 1) {
        proj_errno_set (P, EINVAL);
        return HUGE_VAL;
    }

    o.coo = obs.coo;

    for (i = 0;  i < n;  i++) {
        switch (direction) {
            case PJ_FWD:
                u  =  pj_fwdobs (o, P);
                o  =  pj_invobs (u, P);
                break;
            case PJ_INV:
                u  =  pj_invobs (o, P);
                o  =  pj_fwdobs (u, P);
                break;
            default:
                proj_errno_set (P, EINVAL);
                return HUGE_VAL;
        }
    }

    return proj_xyz_dist (o.coo.xyz, obs.coo.xyz);
}


/* Apply the transformation P to the coordinate coo */
PJ_OBS proj_trans_obs (PJ *P, PJ_DIRECTION direction, PJ_OBS obs) {
    if (0==P)
        return obs;

    switch (direction) {
        case PJ_FWD:
            return pj_fwdobs (obs, P);
        case PJ_INV:
            return  pj_invobs (obs, P);
        case PJ_IDENT:
            return obs;
        default:
            break;
    }

    proj_errno_set (P, EINVAL);
    return proj_obs_error ();
}



/* Apply the transformation P to the coordinate coo */
PJ_COORD proj_trans_coord (PJ *P, PJ_DIRECTION direction, PJ_COORD coo) {
    if (0==P)
        return coo;

    switch (direction) {
        case PJ_FWD:
            return pj_fwdcoord (coo, P);
        case PJ_INV:
            return  pj_invcoord (coo, P);
        case PJ_IDENT:
            return coo;
        default:
            break;
    }

    proj_errno_set (P, EINVAL);
    return proj_coord_error ();
}



/*************************************************************************************/
size_t proj_transform (
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
    PJ_COORD coord = proj_coord_null;
    size_t i, nmin;
    double null_broadcast = 0;
    if (0==P)
        return 0;

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
            coord = pj_fwdcoord (coord, P);
        else
            coord = pj_invcoord (coord, P);

        /* in all full length cases, we overwrite the input with the output */
        if (nx > 1)  {
            *x = coord.xyzt.x;
            x =  (double *) ( ((char *) x) + sx);
        }
        if (ny > 1)  {
            *y = coord.xyzt.y;
            y =  (double *) ( ((char *) y) + sy);
        }
        if (nz > 1)  {
            *z = coord.xyzt.z;
            z =  (double *) ( ((char *) z) + sz);
        }
        if (nt > 1)  {
            *t = coord.xyzt.t;
            t =  (double *) ( ((char *) t) + st);
        }
    }
    /* Last time around, we update the length 1 cases with their transformed alter egos */
    /* ... or would we rather not? Then what about the nmin==1 case?                    */
    /* perhaps signalling the non-array case by setting all strides to 0?               */
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

/*****************************************************************************/
int proj_transform_obs (PJ *P, PJ_DIRECTION direction, size_t n, PJ_OBS *obs) {
/******************************************************************************
    Batch transform an array of PJ_OBS.

    Returns 0 if all observations are transformed without error, otherwise
    returns error number.
******************************************************************************/
    size_t i;
    for (i=0; i<n; i++) {
        obs[i] = proj_trans_obs(P, direction, obs[i]);
        if (proj_errno(P))
            return proj_errno(P);
    }

    return 0;
}

/*****************************************************************************/
int proj_transform_coord (PJ *P, PJ_DIRECTION direction, size_t n, PJ_COORD *coord) {
/******************************************************************************
    Batch transform an array of PJ_COORD.

    Returns 0 if all coordinates are transformed without error, otherwise
    returns error number.
******************************************************************************/
    size_t i;
    for (i=0; i<n; i++) {
        coord[i] = proj_trans_coord(P, direction, coord[i]);
        if (proj_errno(P))
            return proj_errno(P);
    }

    return 0;
}



PJ *proj_create (PJ_CONTEXT *ctx, const char *definition) {
    if (0==ctx)
        ctx = pj_get_default_ctx ();
    return pj_init_plus_ctx (ctx, definition);
}

PJ *proj_create_argv (PJ_CONTEXT *ctx, int argc, char **argv) {
    if (0==ctx)
        ctx = pj_get_default_ctx ();
    return pj_init_ctx (ctx, argc, argv);
}

/*****************************************************************************/
PJ  *proj_create_crs_to_crs (PJ_CONTEXT *ctx, const char *srid_from, const char *srid_to) {
/******************************************************************************
    Create a transformation pipeline between two known coordinate reference
    systems.

    srid_from and srid_to should be the value part of a +init=... parameter
    set, i.e. "epsg:25833" or "IGNF:AMST63". Any projection definition that is
    can be found in a init-file in PROJ_LIB is a valid input to this function.

    For now the function mimics the cs2cs app: An input and an output CRS is
    given and coordinates are transformed via a hub datum (WGS84). This
    transformation strategy is referred to as "early-binding" by the EPSG. The
    function can be extended to support "late-binding" transformations in the
    future without affecting users of the function.

    Example call:

        PJ *P = proj_create_crs_to_crs(0, "epsg:25832", "epsg:25833");

******************************************************************************/
    PJ *P;
    char buffer[512];

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

/* For now, if PJ itself is clean, we return the thread local error level. */
/* This may change as OBS_API error reporting matures */
int proj_errno (PJ *P) {
    if (0==P)
        return pj_ctx_get_errno (pj_get_default_ctx ());
    if (0 != P->last_errno)
        return P->last_errno;
    return pj_ctx_get_errno (pj_get_ctx (P));
}

/*****************************************************************************/
void proj_errno_set (PJ *P, int err) {
/******************************************************************************
    Sets errno in the PJ, and bubbles it up to the context and pj_errno levels
    through the low level pj_ctx interface.
******************************************************************************/
    if (0==P) {
        errno = EINVAL;
        return;
    }

    /* Use proj_errno_reset to explicitly clear the error status */
    if (0==err)
        return;

    /* set local error level */
    P->last_errno = err;
    /* and let it bubble up */
    proj_context_errno_set (pj_get_ctx (P), err);
    errno = err;
    return;
}

/*****************************************************************************/
void proj_errno_restore (PJ *P, int err) {
/******************************************************************************
    Reduce some mental impedance in the canonical reset/restore use case:
    Basically, proj_errno_restore() is a synonym for proj_errno_set(),
    but the use cases are very different (_set: indicate an error to higher
    level user code, _restore: pass previously set error indicators in case
    of no errors at this level).

    Hence, although the inner working is identical, we provide both options,
    to avoid some rather confusing real world code.

    See usage example under proj_errno_reset ()
******************************************************************************/
    proj_errno_set (P, err);
}

/*****************************************************************************/
int proj_errno_reset (PJ *P) {
/******************************************************************************
    Clears errno in the PJ, and bubbles it up to the context and
    pj_errno levels through the low level pj_ctx interface.

    Returns the previous value of the errno, for convenient reset/restore
    operations:

    void foo (PJ *P) {
        int last_errno = proj_errno_reset (P);

        do_something_with_P (P);

        (* failure - keep latest error status *)
        if (proj_errno(P))
            return;
        (* success - restore previous error status *)
        proj_errno_restore (P, last_errno);
        return;
    }
******************************************************************************/
    int last_errno;
    if (0==P) {
        errno = EINVAL;
        return EINVAL;
    }
    last_errno = proj_errno (P);

    /* set local error level */
    P->last_errno = 0;
    /* and let it bubble up */
    pj_ctx_set_errno (pj_get_ctx (P), 0);
    errno = 0;
    return last_errno;
}


/* Create a new context - or provide a pointer to the default context */
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
    const char **paths;
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
PJ_PROJ_INFO proj_pj_info(const PJ *P) {
/******************************************************************************
    Basic info about a particular instance of a projection object.

    Returns PJ_PROJ_INFO struct.

******************************************************************************/
    PJ_PROJ_INFO info;
    char *def;

    memset(&info, 0, sizeof(PJ_PROJ_INFO));

    if (!P) {
        return info;
    }

    /* projection id */
    if (pj_param(P->ctx, P->params, "tproj").i)
        pj_strlcpy(info.id, pj_param(P->ctx, P->params, "sproj").s, sizeof(info.id));

    /* projection description */
    pj_strlcpy(info.description, P->descr, sizeof(info.description));

    /* projection definition */
    def = pj_get_def((PJ *)P, 0); /* pj_get_def takes a non-const PJ pointer */
    pj_strlcpy(info.definition, &def[1], sizeof(info.definition)); /* def includes a leading space */
    pj_dealloc(def);

    /* this does not take into account that a pipeline potentially does not */
    /* have an inverse.                                                     */
    info.has_inverse = (P->inv != 0 || P->inv3d != 0 || P->invobs != 0);

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

    If the init file is not found all members of
    the return struct are set to 0. If the init file is found, but it the
    metadata is missing, the value is set to "Unknown".

******************************************************************************/
    int file_found, def_found=0;
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
    next = pj_get_init(ctx, &start, start, key, &def_found);

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
PJ_DERIVS proj_derivatives(const PJ *P, const LP lp) {
/******************************************************************************
    Derivatives of coordinates.

    returns PJ_DERIVS. If unsuccessfull error number is set and the returned
    struct contains NULL data.

******************************************************************************/
    PJ_DERIVS derivs;

    if (pj_deriv(lp, 1e-5, (PJ *)P, &derivs)) {
        /* errno set in pj_derivs */
        memset(&derivs, 0, sizeof(PJ_DERIVS));
    }

    return derivs;
}


/*****************************************************************************/
PJ_FACTORS proj_factors(const PJ *P, const LP lp) {
/******************************************************************************
    Cartographic characteristics at point lp.

    Characteristics include meridian, parallel and areal scales, angular
    distortion, meridian/parallel, meridian convergence and scale error.

    returns PJ_FACTORS. If unsuccessfull error number is set and the returned
    struct contains NULL data.

******************************************************************************/
    PJ_FACTORS factors;

    /* pj_factors rely code being zero */
    factors.code = 0;

    if (pj_factors(lp, (PJ *)P, 0.0, &factors)) {
        /* errno set in pj_factors */
        memset(&factors, 0, sizeof(PJ_FACTORS));
    }

    return factors;
}


double proj_torad (double angle_in_degrees) { return PJ_TORAD (angle_in_degrees);}
double proj_todeg (double angle_in_radians) { return PJ_TODEG (angle_in_radians);}


double proj_dmstor(const char *is, char **rs) {
    return dmstor(is, rs);
}

char*  proj_rtodms(char *s, double r, int pos, int neg) {
    return rtodms(s, r, pos, neg);
}


