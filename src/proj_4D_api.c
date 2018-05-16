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

#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "proj.h"
#include "proj_internal.h"
#include "proj_math.h"
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
        return pj_left (P)==PJ_IO_UNITS_ANGULAR;
    return pj_right (P)==PJ_IO_UNITS_ANGULAR;
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


/* Geodesic distance (in meter) + fwd and rev azimuth between two points on the ellipsoid */
PJ_COORD proj_geod (const PJ *P, PJ_COORD a, PJ_COORD b) {
    PJ_COORD c;
    /* Note: the geodesic code takes arguments in degrees */
    geod_inverse (P->geod,
        PJ_TODEG(a.lpz.phi), PJ_TODEG(a.lpz.lam),
        PJ_TODEG(b.lpz.phi), PJ_TODEG(b.lpz.lam),
        c.v, c.v+1, c.v+2
    );

    return c;
}


/* Geodesic distance (in meter) between two points with angular 2D coordinates */
double proj_lp_dist (const PJ *P, PJ_COORD a, PJ_COORD b) {
    double s12, azi1, azi2;
    /* Note: the geodesic code takes arguments in degrees */
    geod_inverse (P->geod,
        PJ_TODEG(a.lpz.phi), PJ_TODEG(a.lpz.lam),
        PJ_TODEG(b.lpz.phi), PJ_TODEG(b.lpz.lam),
        &s12, &azi1, &azi2
    );
    return s12;
}

/* The geodesic distance AND the vertical offset */
double proj_lpz_dist (const PJ *P, PJ_COORD a, PJ_COORD b) {
    if (HUGE_VAL==a.lpz.lam || HUGE_VAL==b.lpz.lam)
        return HUGE_VAL;
    return hypot (proj_lp_dist (P, a, b), a.lpz.z - b.lpz.z);
}

/* Euclidean distance between two points with linear 2D coordinates */
double proj_xy_dist (PJ_COORD a, PJ_COORD b) {
    return hypot (a.xy.x - b.xy.x, a.xy.y - b.xy.y);
}

/* Euclidean distance between two points with linear 3D coordinates */
double proj_xyz_dist (PJ_COORD a, PJ_COORD b) {
    return hypot (proj_xy_dist (a, b), a.xyz.z - b.xyz.z);
}



/* Measure numerical deviation after n roundtrips fwd-inv (or inv-fwd) */
double proj_roundtrip (PJ *P, PJ_DIRECTION direction, int n, PJ_COORD *coord) {
    int i;
    PJ_COORD t, org;

    if (0==P)
        return HUGE_VAL;

    if (n < 1) {
        proj_errno_set (P, EINVAL);
        return HUGE_VAL;
    }

    /* in the first half-step, we generate the output value */
    org  = *coord;
    *coord = proj_trans (P, direction, org);
    t = *coord;

    /* now we take n-1 full steps in inverse direction: We are */
    /* out of phase due to the half step already taken */
    for (i = 0;  i < n - 1;  i++)
        t = proj_trans (P,  direction,  proj_trans (P, -direction, t) );

    /* finally, we take the last half-step */
    t = proj_trans (P, -direction, t);

    /* checking for angular *input* since we do a roundtrip, and end where we begin */
    if (proj_angular_input (P, direction))
        return proj_lpz_dist (P, org, t);

    return proj_xyz_dist (org, t);
}



/**************************************************************************************/
PJ_COORD proj_trans (PJ *P, PJ_DIRECTION direction, PJ_COORD coord) {
/***************************************************************************************
Apply the transformation P to the coordinate coord, preferring the 4D interfaces if
available.

See also pj_approx_2D_trans and pj_approx_3D_trans in pj_internal.c, which work
similarly, but prefers the 2D resp. 3D interfaces if available.
***************************************************************************************/
    if (0==P)
        return coord;
    if (P->inverted)
        direction = -direction;

    switch (direction) {
        case PJ_FWD:
            return pj_fwd4d (coord, P);
        case PJ_INV:
            return  pj_inv4d (coord, P);
        case PJ_IDENT:
            return coord;
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
PJ_COORD proj_geocentric_latitude (const PJ *P, PJ_DIRECTION direction, PJ_COORD coord) {
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
    PJ_COORD res = coord;
    if ((coord.lp.phi > limit) || (coord.lp.phi < -limit) || (P->es==0))
        return res;
    if (direction==PJ_FWD)
        res.lp.phi = atan (P->one_es * tan (coord.lp.phi) );
    else
        res.lp.phi = atan (P->rone_es * tan (coord.lp.phi) );

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
static PJ* skip_prep_fin(PJ *P) {
/**************************************************************************************
Skip prepare and finalize function for the various "helper operations" added to P when
in cs2cs compatibility mode.
**************************************************************************************/
    P->skip_fwd_prepare  = 1;
    P->skip_fwd_finalize = 1;
    P->skip_inv_prepare  = 1;
    P->skip_inv_finalize = 1;
    return P;
}

/*************************************************************************************/
static int cs2cs_emulation_setup (PJ *P) {
/**************************************************************************************
If any cs2cs style modifiers are given (axis=..., towgs84=..., ) create the 4D API
equivalent operations, so the preparation and finalization steps in the pj_inv/pj_fwd
invocators can emulate the behaviour of pj_transform and the cs2cs app.

Returns 1 on success, 0 on failure
**************************************************************************************/
    PJ *Q;
    paralist *p;
    int do_cart = 0;
    if (0==P)
        return 0;

    /* Don't recurse when calling proj_create (which calls us back) */
    if (pj_param_exists (P->params, "break_cs2cs_recursion"))
        return 1;

    /* Swap axes? */
    p = pj_param_exists (P->params, "axis");

    /* Don't axisswap if data are already in "enu" order */
    if (p && (0!=strcmp ("enu", p->param))) {
        char *def = malloc (100+strlen(P->axis));
        if (0==def)
            return 0;
        sprintf (def, "break_cs2cs_recursion     proj=axisswap  axis=%s", P->axis);
        Q = proj_create (P->ctx, def);
        free (def);
        if (0==Q)
            return 0;
        P->axisswap = skip_prep_fin(Q);
    }

    /* Geoid grid(s) given? */
    p = pj_param_exists (P->params, "geoidgrids");
    if (p  &&  strlen (p->param) > strlen ("geoidgrids=")) {
        char *gridnames = p->param + strlen ("geoidgrids=");
        char *def = malloc (100+strlen(gridnames));
        if (0==def)
            return 0;
        sprintf (def, "break_cs2cs_recursion     proj=vgridshift  grids=%s", gridnames);
        Q = proj_create (P->ctx, def);
        free (def);
        if (0==Q)
            return 0;
        P->vgridshift = skip_prep_fin(Q);
    }

    /* Datum shift grid(s) given? */
    p = pj_param_exists (P->params, "nadgrids");
    if (p  &&  strlen (p->param) > strlen ("nadgrids=")) {
        char *gridnames = p->param + strlen ("nadgrids=");
        char *def = malloc (100+strlen(gridnames));
        if (0==def)
            return 0;
        sprintf (def, "break_cs2cs_recursion     proj=hgridshift  grids=%s", gridnames);
        Q = proj_create (P->ctx, def);
        free (def);
        if (0==Q)
            return 0;
        P->hgridshift = skip_prep_fin(Q);
    }

    /* We ignore helmert if we have grid shift */
    p = P->hgridshift ? 0 : pj_param_exists (P->params, "towgs84");
    while (p) {
        char *def;
        char *s = p->param;
        double *d = P->datum_params;
        size_t n = strlen (s);

        /* We ignore null helmert shifts (common in auto-translated resource files, e.g. epsg) */
        if (0==d[0] && 0==d[1] && 0==d[2] && 0==d[3] && 0==d[4] && 0==d[5] && 0==d[6]) {
            /* If the current ellipsoid is not WGS84, then make sure the */
            /* change in ellipsoid is still done. */
            if (!(fabs(P->a_orig - 6378137.0) < 1e-8 && fabs(P->es_orig - 0.0066943799901413) < 1e-15)) {
                do_cart = 1;
            }
            break;
        }

        if (n <= 8) /* 8==strlen ("towgs84=") */
            return 0;

        def = malloc (100+n);
        if (0==def)
            return 0;
        sprintf (def, "break_cs2cs_recursion     proj=helmert exact %s transpose", s);
        Q = proj_create (P->ctx, def);
        pj_inherit_ellipsoid_def (P, Q);
        free (def);
        if (0==Q)
            return 0;
        P->helmert = skip_prep_fin (Q);

        break;
    }

    /* We also need cartesian/geographical transformations if we are working in */
    /* geocentric/cartesian space or we need to do a Helmert transform.         */
    if (P->is_geocent || P->helmert || do_cart) {
        char def[150];
        sprintf (def, "break_cs2cs_recursion     proj=cart   a=%40.20g  es=%40.20g", P->a_orig, P->es_orig);
        Q = proj_create (P->ctx, def);
        if (0==Q)
            return 0;
        P->cart = skip_prep_fin (Q);

        sprintf (def, "break_cs2cs_recursion     proj=cart  ellps=WGS84");
        Q = proj_create (P->ctx, def);
        if (0==Q)
            return 0;
        P->cart_wgs84 = skip_prep_fin (Q);
    }

    return 1;
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
    PJ    *P;
    char  *args, **argv;
    size_t argc, n;
    int    ret;

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

    /* Support cs2cs-style modifiers */
    ret = cs2cs_emulation_setup  (P);
    if (0==ret)
        return proj_destroy (P);

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
    size_t len;

    /* area not in use yet, suppressing warning */
    (void)area;

    strcpy(buffer, "+proj=pipeline +step +init=");
    len = strlen(buffer);
    strncat(buffer + len, srid_from, sizeof(buffer)-1-len);
    len += strlen(buffer + len);
    strncat(buffer + len, " +inv +step +init=", sizeof(buffer)-1-len);
    len += strlen(buffer + len);
    strncat(buffer + len, srid_to, sizeof(buffer)-1-len);

    P = proj_create(ctx, buffer);

    return P;
}

PJ *proj_destroy (PJ *P) {
    pj_free (P);
    return 0;
}

/*****************************************************************************/
int proj_errno (const PJ *P) {
/******************************************************************************
    Read an error level from the context of a PJ.
******************************************************************************/
    return pj_ctx_get_errno (pj_get_ctx ((PJ *) P));
}

/*****************************************************************************/
int proj_context_errno (PJ_CONTEXT *ctx) {
/******************************************************************************
    Read an error directly from a context, without going through a PJ
    belonging to that context.
******************************************************************************/
    if (0==ctx)
        ctx = pj_get_default_ctx();
    return pj_ctx_get_errno (ctx);
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
    pj_errno = 0;
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
static char *path_append (char *buf, const char *app, size_t *buf_size) {
/******************************************************************************
    Helper for proj_info() below. Append app to buf, separated by a
    semicolon. Also handle allocation of longer buffer if needed.

    Returns buffer and adjusts *buf_size through provided pointer arg.
******************************************************************************/
    char *p;
    size_t len, applen = 0, buflen = 0;
#ifdef _WIN32
    char *delim = ";";
#else
    char *delim = ":";
#endif

    /* Nothing to do? */
    if (0 == app)
        return buf;
    applen = strlen (app);
    if (0 == applen)
        return buf;

    /* Start checking whether buf is long enough */
    if (0 != buf)
        buflen = strlen (buf);
    len = buflen+applen+strlen (delim) + 1;

    /* "pj_realloc", so to speak */
    if (*buf_size < len) {
        p = pj_calloc (2 * len, sizeof (char));
        if (0==p) {
            pj_dealloc (buf);
            return 0;
        }
        *buf_size = 2 * len;
        if (buf != 0)
            strcpy (p, buf);
        pj_dealloc (buf);
        buf = p;
    }

    /* Only append a semicolon if something's already there */
    if (0 != buflen)
        strcat (buf, ";");
    strcat (buf, app);
    return buf;
}

static const char *empty = {""};
static char version[64]  = {""};
static PJ_INFO info = {0, 0, 0, 0, 0, 0, 0, 0};
static volatile int info_initialized = 0;

/*****************************************************************************/
PJ_INFO proj_info (void) {
/******************************************************************************
    Basic info about the current instance of the PROJ.4 library.

    Returns PJ_INFO struct.
******************************************************************************/
    const char * const *paths;
    size_t i, n;

    size_t  buf_size = 0;
    char   *buf = 0;

    pj_acquire_lock ();

    if (0!=info_initialized) {
        pj_release_lock ();
        return info;
    }

    info.major = PROJ_VERSION_MAJOR;
    info.minor = PROJ_VERSION_MINOR;
    info.patch = PROJ_VERSION_PATCH;

    /* This is a controlled environment, so no risk of sprintf buffer
    overflow. A normal version string is xx.yy.zz which is 8 characters
    long and there is room for 64 bytes in the version string. */
    sprintf (version, "%d.%d.%d", info.major, info.minor, info.patch);

    info.searchpath = empty;
    info.version    = version;
    info.release    = pj_get_release ();

    /* build search path string */
    buf = path_append (buf, getenv ("HOME"), &buf_size);
    buf = path_append (buf, getenv ("PROJ_LIB"), &buf_size);

    paths = proj_get_searchpath ();
    n = (size_t) proj_get_path_count ();

    for (i = 0;  i < n;  i++)
        buf = path_append (buf, paths[i], &buf_size);
    info.searchpath = buf ? buf : empty;

    info.paths = paths;
    info.path_count = n;

    info_initialized = 1;
    pj_release_lock ();
    return info;
}


/*****************************************************************************/
PJ_PROJ_INFO proj_pj_info(PJ *P) {
/******************************************************************************
    Basic info about a particular instance of a projection object.

    Returns PJ_PROJ_INFO struct.
******************************************************************************/
    PJ_PROJ_INFO pjinfo;
    char *def;

    memset(&pjinfo, 0, sizeof(PJ_PROJ_INFO));

    /* Expected accuracy of the transformation. Hardcoded for now, will be improved */
    /* later. Most likely to be used when a transformation is set up with           */
    /* proj_create_crs_to_crs in a future version that leverages the EPSG database. */
    pjinfo.accuracy = -1.0;

    if (0==P)
        return pjinfo;

    /* projection id */
    if (pj_param(P->ctx, P->params, "tproj").i)
        pjinfo.id = pj_param(P->ctx, P->params, "sproj").s;

    /* projection description */
    pjinfo.description = P->descr;

    /* projection definition */
    if (P->def_full)
        def = P->def_full;
    else
        def = pj_get_def(P, 0); /* pj_get_def takes a non-const PJ pointer */
    if (0==def)
        pjinfo.definition = empty;
    else
        pjinfo.definition = pj_shrink (def);
    /* Make pj_free clean this up eventually */
    P->def_full = def;

    pjinfo.has_inverse = pj_has_inverse(P);
    return pjinfo;
}


/*****************************************************************************/
PJ_GRID_INFO proj_grid_info(const char *gridname) {
/******************************************************************************
    Information about a named datum grid.

    Returns PJ_GRID_INFO struct.
******************************************************************************/
    PJ_GRID_INFO grinfo;

    /*PJ_CONTEXT *ctx = proj_context_create(); */
    PJ_CONTEXT *ctx = pj_get_default_ctx();
    PJ_GRIDINFO *gridinfo = pj_gridinfo_init(ctx, gridname);
    memset(&grinfo, 0, sizeof(PJ_GRID_INFO));

    /* in case the grid wasn't found */
    if (gridinfo->filename == NULL) {
        pj_gridinfo_free(ctx, gridinfo);
        strcpy(grinfo.format, "missing");
        return grinfo;
    }

    /* The string copies below are automatically null-terminated due to */
    /* the memset above, so strncpy is safe */

    /* name of grid */
    strncpy (grinfo.gridname, gridname, sizeof(grinfo.gridname) - 1);

    /* full path of grid */
    pj_find_file(ctx, gridname, grinfo.filename, sizeof(grinfo.filename) - 1);

    /* grid format */
    strncpy (grinfo.format, gridinfo->format, sizeof(grinfo.format) - 1);

    /* grid size */
    grinfo.n_lon = gridinfo->ct->lim.lam;
    grinfo.n_lat = gridinfo->ct->lim.phi;

    /* cell size */
    grinfo.cs_lon = gridinfo->ct->del.lam;
    grinfo.cs_lat = gridinfo->ct->del.phi;

    /* bounds of grid */
    grinfo.lowerleft  = gridinfo->ct->ll;
    grinfo.upperright.lam = grinfo.lowerleft.lam + grinfo.n_lon*grinfo.cs_lon;
    grinfo.upperright.phi = grinfo.lowerleft.phi + grinfo.n_lat*grinfo.cs_lat;

    pj_gridinfo_free(ctx, gridinfo);

    return grinfo;
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
    PJ_INIT_INFO ininfo;
    PJ_CONTEXT *ctx = pj_get_default_ctx();

    memset(&ininfo, 0, sizeof(PJ_INIT_INFO));

    file_found = pj_find_file(ctx, initname, ininfo.filename, sizeof(ininfo.filename));
    if (!file_found || strlen(initname) > 64) {
        return ininfo;
    }

    /* The initial memset (0) makes strncpy safe here */
    strncpy (ininfo.name, initname, sizeof(ininfo.name) - 1);
    strcpy(ininfo.origin, "Unknown");
    strcpy(ininfo.version, "Unknown");
    strcpy(ininfo.lastupdate, "Unknown");

    strncpy (key, initname, 64); /* make room for ":metadata\0" at the end */
    key[64] = 0;
    strncat(key, ":metadata", 9);
    strcpy(param, "+init=");
    /* The +strlen(param) avoids a cppcheck false positive warning */
    strncat(param + strlen(param), key, sizeof(param)-1-strlen(param));

    start = pj_mkparam(param);
    pj_expand_init(ctx, start);

    if (pj_param(ctx, start, "tversion").i)
        strncpy(ininfo.version, pj_param(ctx, start, "sversion").s, sizeof(ininfo.version) - 1);

    if (pj_param(ctx, start, "torigin").i)
        strncpy(ininfo.origin, pj_param(ctx, start, "sorigin").s, sizeof(ininfo.origin) - 1);

    if (pj_param(ctx, start, "tlastupdate").i)
        strncpy(ininfo.lastupdate, pj_param(ctx, start, "slastupdate").s, sizeof(ininfo.lastupdate) - 1);

    for ( ; start; start = next) {
        next = start->next;
        pj_dalloc(start);
    }

   return ininfo;
}



/*****************************************************************************/
PJ_FACTORS proj_factors(PJ *P, PJ_COORD lp) {
/******************************************************************************
    Cartographic characteristics at point lp.

    Characteristics include meridian, parallel and areal scales, angular
    distortion, meridian/parallel, meridian convergence and scale error.

    returns PJ_FACTORS. If unsuccessful, error number is set and the
    struct returned contains NULL data.
******************************************************************************/
    PJ_FACTORS factors = {0,0,0,  0,0,0,  0,0,  0,0,0,0};
    struct FACTORS f;

    if (0==P)
        return factors;

    if (pj_factors(lp.lp, P, 0.0, &f))
        return factors;

    factors.meridional_scale  =  f.h;
    factors.parallel_scale    =  f.k;
    factors.areal_scale       =  f.s;

    factors.angular_distortion        =  f.omega;
    factors.meridian_parallel_angle   =  f.thetap;
    factors.meridian_convergence      =  f.conv;

    factors.tissot_semimajor  =  f.a;
    factors.tissot_semiminor  =  f.b;

    /* Raw derivatives, for completeness's sake */
    factors.dx_dlam = f.der.x_l;
    factors.dx_dphi = f.der.x_p;
    factors.dy_dlam = f.der.y_l;
    factors.dy_dphi = f.der.y_p;

    return factors;
}
