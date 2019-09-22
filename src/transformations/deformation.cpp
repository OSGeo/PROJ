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
to cartesian PJ_XYZ-space and applied to the input coordinates (also in
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
#include <math.h>

PROJ_HEAD(deformation, "Kinematic grid shift");

#define TOL 1e-8
#define MAX_ITERATIONS 10

namespace { // anonymous namespace
struct pj_opaque {
    double dt;
    double t_epoch;
    PJ *cart;
};
} // anonymous namespace

/********************************************************************************/
static PJ_XYZ get_grid_shift(PJ* P, PJ_XYZ cartesian) {
/********************************************************************************
    Read correction values from grid. The cartesian input coordinates are
    converted to geodetic coordinates in order look up the correction values
    in the grid. Once the grid corrections are read we need to convert them
    from ENU-space to cartesian PJ_XYZ-space. ENU -> PJ_XYZ formula described in:

    Nørbech, T., et al, 2003(?), "Transformation from a Common Nordic Reference
    Frame to ETRS89 in Denmark, Finland, Norway, and Sweden – status report"

********************************************************************************/
    PJ_COORD geodetic, shift, temp;
    double sp, cp, sl, cl;
    int previous_errno = proj_errno_reset(P);

    /* cartesian to geodetic */
    geodetic.lpz = pj_inv3d(cartesian, static_cast<struct pj_opaque*>(P->opaque)->cart);

    /* look up correction values in grids */
    shift.lp    = proj_hgrid_value(P, geodetic.lp);
    shift.enu.u = proj_vgrid_value(P, geodetic.lp, 1.0);

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

    /* ENU -> PJ_XYZ */
    temp.xyz.x = -sp*cl*shift.enu.n - sl*shift.enu.e + cp*cl*shift.enu.u;
    temp.xyz.y = -sp*sl*shift.enu.n + cl*shift.enu.e + cp*sl*shift.enu.u;
    temp.xyz.z =     cp*shift.enu.n +                     sp*shift.enu.u;

    shift.xyz = temp.xyz;

    proj_errno_restore(P, previous_errno);

    return shift.xyz;
}

/********************************************************************************/
static PJ_XYZ reverse_shift(PJ *P, PJ_XYZ input, double dt) {
/********************************************************************************
    Iteratively determine the reverse grid shift correction values.
*********************************************************************************/
    PJ_XYZ out, delta, dif;
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

static PJ_XYZ forward_3d(PJ_LPZ lpz, PJ *P) {
    struct pj_opaque *Q = (struct pj_opaque *) P->opaque;
    PJ_COORD out, in;
    PJ_XYZ shift;
    in.lpz = lpz;
    out = in;

    if (Q->dt == HUGE_VAL) {
        out = proj_coord_error(); /* in the 3D case +t_obs must be specified */
        proj_log_debug(P, "deformation: +dt must be specified");
        return out.xyz;
    }

    shift = get_grid_shift(P, in.xyz);

    out.xyz.x += Q->dt * shift.x;
    out.xyz.y += Q->dt * shift.y;
    out.xyz.z += Q->dt * shift.z;

    return out.xyz;
}


static PJ_COORD forward_4d(PJ_COORD in, PJ *P) {
    struct pj_opaque *Q = (struct pj_opaque *) P->opaque;
    double dt;
    PJ_XYZ shift;
    PJ_COORD out = in;

    if (Q->dt != HUGE_VAL) {
        dt = Q->dt;
    } else {
        dt = in.xyzt.t - Q->t_epoch ;
    }

    shift = get_grid_shift(P, in.xyz);

    out.xyzt.x += dt*shift.x;
    out.xyzt.y += dt*shift.y;
    out.xyzt.z += dt*shift.z;


    return out;
}


static PJ_LPZ reverse_3d(PJ_XYZ in, PJ *P) {
    struct pj_opaque *Q = (struct pj_opaque *) P->opaque;
    PJ_COORD out;
    out.xyz = in;

    if (Q->dt == HUGE_VAL) {
        out = proj_coord_error(); /* in the 3D case +t_obs must be specified */
        proj_log_debug(P, "deformation: +dt must be specified");
        return out.lpz;
    }

    out.xyz = reverse_shift(P, in, Q->dt);

    return out.lpz;
}

static PJ_COORD reverse_4d(PJ_COORD in, PJ *P) {
    struct pj_opaque *Q = (struct pj_opaque *) P->opaque;
    PJ_COORD out = in;
    double dt;


    if (Q->dt != HUGE_VAL) {
            dt = Q->dt;
        } else {
            dt = in.xyzt.t - Q->t_epoch;
    }

    out.xyz = reverse_shift(P, in.xyz, dt);
    return out;
}

static PJ *destructor(PJ *P, int errlev) {
    if (nullptr==P)
        return nullptr;

    if (nullptr==P->opaque)
        return pj_default_destructor (P, errlev);

    if (static_cast<struct pj_opaque*>(P->opaque)->cart)
        static_cast<struct pj_opaque*>(P->opaque)->cart->destructor (static_cast<struct pj_opaque*>(P->opaque)->cart, errlev);

    return pj_default_destructor(P, errlev);
}


PJ *TRANSFORMATION(deformation,1) {
    int has_xy_grids = 0;
    int has_z_grids  = 0;
    struct pj_opaque *Q = static_cast<struct pj_opaque*>(pj_calloc (1, sizeof (struct pj_opaque)));
    if (nullptr==Q)
        return destructor(P, ENOMEM);
    P->opaque = (void *) Q;

    // Pass a dummy ellipsoid definition that will be overridden just afterwards
    Q->cart = proj_create(P->ctx, "+proj=cart +a=1");
    if (Q->cart == nullptr)
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

    Q->dt = HUGE_VAL;
    if (pj_param(P->ctx, P->params, "tdt").i) {
       Q->dt = pj_param(P->ctx, P->params, "ddt").f;
    }

    if (pj_param_exists(P->params, "t_obs")) {
        proj_log_error(P, "deformation: +t_obs parameter is deprecated. Use +dt instead.");
        return destructor(P, PJD_ERR_MISSING_ARGS);
    }

    Q->t_epoch = HUGE_VAL;
    if (pj_param(P->ctx, P->params, "tt_epoch").i) {
        Q->t_epoch = pj_param(P->ctx, P->params, "dt_epoch").f;
    }

    if (Q->dt == HUGE_VAL && Q->t_epoch == HUGE_VAL) {
        proj_log_error(P, "deformation: either +dt or +t_epoch needs to be set.");
        return destructor(P, PJD_ERR_MISSING_ARGS);
    }

    if (Q->dt != HUGE_VALL && Q->t_epoch != HUGE_VALL) {
        proj_log_error(P, "deformation: +dt or +t_epoch are mutually exclusive.");
        return destructor(P, PJD_ERR_MUTUALLY_EXCLUSIVE_ARGS);
    }

    P->fwd4d = forward_4d;
    P->inv4d = reverse_4d;
    P->fwd3d  = forward_3d;
    P->inv3d  = reverse_3d;
    P->fwd    = nullptr;
    P->inv    = nullptr;

    P->left  = PJ_IO_UNITS_CARTESIAN;
    P->right = PJ_IO_UNITS_CARTESIAN;
    P->destructor = destructor;

    return P;
}

