/***********************************************************************

        Kinematic datum shifting utilizing a deformation model

                    Kristian Evers, 2017-10-29

************************************************************************

Perform datum shifts by means of a deformation/velocity model.

    X_out = X_in + (T_ct - T_obs)*DX
    Y_out = Y_in + (T_ct - T_obs)*DY
    Z_out = Z_in + (T_ct - T_obs)*DZ


The deformation operation takes cartesian coordinates as input and
returns cartesian coordinates as well.

Corrections in the gridded model are in east, north, up (ENU) space.
Hence the input coordinates needs to be converted to ENU-space when
searching for corrections in the grid. The corrections are then converted
to cartesian XYZ-space and applied to the input coordinates (also in
cartesian space).

A full deformation model is described by two grids, one for the horizontal
components and one for the vertical component. The horizontal grid is
stored in CTable/CTable2 and the vertical grid is stored in the GTX
format. The NTv2 format should not be used for this purpose since grid-
values are scaled upon reading. Both grids are expected to contain
grid-values in units of mm/year in ENU-space.

************************************************************************
* Copyright (c) 2017, Kristian Evers
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
*
***********************************************************************/
#define PJ_LIB__
#include <errno.h>
#include "proj.h"
#include "proj_internal.h"
#include "proj_math.h"
#include "projects.h"

PROJ_HEAD(deformation, "Kinematic grid shift");

#define TOL 1e-8
#define MAX_ITERATIONS 10

struct pj_opaque {
    double t_obs;
    double t_epoch;
    PJ *cart;
};

/********************************************************************************/
static XYZ get_grid_shift(PJ* P, XYZ cartesian) {
/********************************************************************************
    Read correction values from grid. The cartesian input coordinates are
    converted to geodetic coordinates in order look up the correction values
    in the grid. Once the grid corrections are read we need to convert them
    from ENU-space to cartesian XYZ-space. ENU -> XYZ formula described in:

    Nørbech, T., et al, 2003(?), "Transformation from a Common Nordic Reference
    Frame to ETRS89 in Denmark, Finland, Norway, and Sweden – status report"

********************************************************************************/
    PJ_COORD geodetic, shift, temp;
    double sp, cp, sl, cl;
    int previous_errno = proj_errno_reset(P);

    /* cartesian to geodetic */
    geodetic.lpz = pj_inv3d(cartesian, P->opaque->cart);

    /* look up correction values in grids */
    shift.lp    = proj_hgrid_value(P, geodetic.lp);
    shift.enu.u = proj_vgrid_value(P, geodetic.lp);

    if (proj_errno(P) == PJD_ERR_GRID_AREA)
        proj_log_debug(P, "deformation: coordinate (%.3f, %.3f) outside deformation model",
                       proj_todeg(geodetic.lp.lam), proj_todeg(geodetic.lp.phi));

    /* grid values are stored as mm/yr, we need m/yr */
    shift.xyz.x /= 1000;
    shift.xyz.y /= 1000;
    shift.xyz.z /= 1000;

    /* pre-calc cosines and sines */
    sp = sin(geodetic.lp.phi);
    cp = cos(geodetic.lp.phi);
    sl = sin(geodetic.lp.lam);
    cl = cos(geodetic.lp.lam);

    /* ENU -> XYZ */
    temp.xyz.x = -sp*cl*shift.enu.n - sl*shift.enu.e + cp*cl*shift.enu.u;
    temp.xyz.y = -sp*sl*shift.enu.n + cl*shift.enu.e + cp*sl*shift.enu.u;
    temp.xyz.z =     cp*shift.enu.n +                     sp*shift.enu.u;

    shift.xyz = temp.xyz;

    proj_errno_restore(P, previous_errno);

    return shift.xyz;
}

/********************************************************************************/
static XYZ reverse_shift(PJ *P, XYZ input, double dt) {
/********************************************************************************
    Iteratively determine the reverse grid shift correction values.
*********************************************************************************/
    XYZ out, delta, dif;
    double z0;
    int i = MAX_ITERATIONS;

    delta = get_grid_shift(P, input);

    /* Store the origial z shift for later application */
    z0 = delta.z;

    /* When iterating to find the best horizontal coordinate we also carry   */
    /* along the z-component, since we need it for the cartesian -> geodetic */
    /* conversion. The z-component adjustment is overwritten with z0 after   */
    /* the loop has finished.                                                */
    out.x = input.x - dt*delta.x;
    out.y = input.y - dt*delta.y;
    out.z = input.z + dt*delta.z;

    do {
        delta = get_grid_shift(P, out);

        if (delta.x == HUGE_VAL)
            break;

        dif.x  = out.x + dt*delta.x - input.x;
        dif.y  = out.y + dt*delta.y - input.y;
        dif.z  = out.z - dt*delta.z - input.z;
        out.x += dif.x;
        out.y += dif.y;
        out.z += dif.z;

    } while ( --i && hypot(dif.x, dif.y) > TOL );

    out.z = input.z - dt*z0;

    return out;
}

static XYZ forward_3d(LPZ lpz, PJ *P) {
    struct pj_opaque *Q = (struct pj_opaque *) P->opaque;
    PJ_COORD out, in;
    XYZ shift;
    double dt = 0.0;
    in.lpz = lpz;
    out = in;

    if (Q->t_obs != HUGE_VAL) {
        dt = Q->t_epoch - Q->t_obs;
    } else {
        out = proj_coord_error(); /* in the 3D case +t_obs must be specified */
        proj_log_debug(P, "deformation: +t_obs must be specified");
        return out.xyz;
    }

    shift = get_grid_shift(P, in.xyz);

    out.xyz.x += dt * shift.x;
    out.xyz.y += dt * shift.y;
    out.xyz.z += dt * shift.z;

    return out.xyz;
}


static PJ_COORD forward_4d(PJ_COORD in, PJ *P) {
    struct pj_opaque *Q = (struct pj_opaque *) P->opaque;
    double dt;
    XYZ shift;
    PJ_COORD out = in;

    if (Q->t_obs != HUGE_VAL) {
            dt = Q->t_epoch - Q->t_obs;
        } else {
            dt = Q->t_epoch - in.xyzt.t;
    }

    shift = get_grid_shift(P, in.xyz);

    out.xyzt.x += dt*shift.x;
    out.xyzt.y += dt*shift.y;
    out.xyzt.z += dt*shift.z;


    return out;
}


static LPZ reverse_3d(XYZ in, PJ *P) {
    struct pj_opaque *Q = (struct pj_opaque *) P->opaque;
    PJ_COORD out;
    double dt = 0.0;
    out.xyz = in;

    if (Q->t_obs != HUGE_VAL) {
        dt = Q->t_epoch - Q->t_obs;
    } else {
        out = proj_coord_error(); /* in the 3D case +t_obs must be specified */
        proj_log_debug(P, "deformation: +t_obs must be specified");
        return out.lpz;
    }

    out.xyz = reverse_shift(P, in, dt);

    return out.lpz;
}

static PJ_COORD reverse_4d(PJ_COORD in, PJ *P) {
    struct pj_opaque *Q = (struct pj_opaque *) P->opaque;
    PJ_COORD out = in;
    double dt;


    if (Q->t_obs != HUGE_VAL) {
            dt = Q->t_epoch - Q->t_obs;
        } else {
            dt = Q->t_epoch - in.xyzt.t;
    }

    out.xyz = reverse_shift(P, in.xyz, dt);
    return out;
}

static void *destructor(PJ *P, int errlev) {
    if (0==P)
        return 0;

    if (0==P->opaque)
        return pj_default_destructor (P, errlev);

    if (P->opaque->cart)
        P->opaque->cart->destructor (P->opaque->cart, errlev);

    return pj_default_destructor(P, errlev);
}


PJ *TRANSFORMATION(deformation,1) {
    int has_xy_grids = 0;
    int has_z_grids  = 0;
    struct pj_opaque *Q = pj_calloc (1, sizeof (struct pj_opaque));
    if (0==Q)
        return destructor(P, ENOMEM);
    P->opaque = (void *) Q;

    Q->cart = proj_create(P->ctx, "+proj=cart");
    if (Q->cart == 0)
        return destructor(P, ENOMEM);

    /* inherit ellipsoid definition from P to Q->cart */
    pj_inherit_ellipsoid_def (P, Q->cart);

    has_xy_grids = pj_param(P->ctx, P->params, "txy_grids").i;
    has_z_grids  = pj_param(P->ctx, P->params, "tz_grids").i;

    /* Build gridlists. Both horizontal and vertical grids are mandatory. */
    if (!has_xy_grids || !has_z_grids) {
        proj_log_error(P, "deformation: Both +xy_grids and +z_grids should be specified.");
        return destructor(P, PJD_ERR_NO_ARGS );
    }

    proj_hgrid_init(P, "xy_grids");
    if (proj_errno(P)) {
        proj_log_error(P, "deformation: could not find requested xy_grid(s).");
        return destructor(P, PJD_ERR_FAILED_TO_LOAD_GRID);
    }

    proj_vgrid_init(P, "z_grids");
    if (proj_errno(P)) {
        proj_log_error(P, "deformation: could not find requested z_grid(s).");
        return destructor(P, PJD_ERR_FAILED_TO_LOAD_GRID);
    }

    Q->t_obs = HUGE_VAL;
    if (pj_param(P->ctx, P->params, "tt_obs").i) {
       Q->t_obs = pj_param(P->ctx, P->params, "dt_obs").f;
    }

    if (pj_param(P->ctx, P->params, "tt_epoch").i) {
        Q->t_epoch = pj_param(P->ctx, P->params, "dt_epoch").f;
    } else {
       proj_log_error(P, "deformation: +t_epoch parameter missing.");
       return destructor(P, PJD_ERR_MISSING_ARGS);
    }

    P->fwd4d = forward_4d;
    P->inv4d = reverse_4d;
    P->fwd3d  = forward_3d;
    P->inv3d  = reverse_3d;
    P->fwd    = 0;
    P->inv    = 0;

    P->left  = PJ_IO_UNITS_CARTESIAN;
    P->right = PJ_IO_UNITS_CARTESIAN;
    P->destructor = destructor;

    return P;
}

