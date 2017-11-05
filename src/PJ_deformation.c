/***********************************************************************

        Kinematic datum shifting utilizing a deformation model

                    Kristian Evers, 2017-10-29

************************************************************************

Perform datum shifts by means of a deformation/velocity model.

    X_out = X_in + (T_ct - T_obs)*DX
    Y_out = Y_in + (T_ct - T_obs)*DY
    Z_out = Z_in + (T_ct - T_obs)*DZ


Corrections are done in cartesian space. Coordinates of the
gridded model are in lat/long space and the corrections in the model are
in cartesian space. Hence the input coordinates needs to be converted
to lat/long space when searching for corrections in the grid. The
corrections are then applied to the input coordinates in cartesian
space.

A full deformation model is described by two grids, one for the horizontal
components and one for the vertical component. The horizontal grid is
stored in CTable/CTable2 and the vertical grid is stored in the GTX
format. Both grids are expected to contain grid-values in units of
m/year.

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
#include <proj.h>
#include "proj_internal.h"
#include "projects.h"

PROJ_HEAD(deformation, "Kinematic grid shift");

#define TOL 1e-8
#define MAX_ITERATIONS 10

struct pj_opaque {
    double t_obs;
    double t_epoch;
    int has_xy_grids, has_z_grids;
    PJ *cart;
};

static XYZ get_xygrid_shift(PJ* P, XYZ cartesian) {
    PJ_COORD geodetic, shift;

    geodetic.lpz = pj_inv3d(cartesian, P->opaque->cart);
    shift.lp = proj_hgrid_value(P, geodetic.lp);

    return shift.xyz;
}

static double get_zgrid_shift(PJ* P, XYZ cartesian) {
    PJ_COORD geodetic;

    geodetic.lpz = pj_inv3d(cartesian, P->opaque->cart);

    return proj_vgrid_value(P, geodetic.lp);
}

static XYZ reverse_hshift(PJ *P, XYZ input, double dt) {
    XYZ out, delta, dif;
    int i = MAX_ITERATIONS;

    delta = get_xygrid_shift(P, input);

    out.x = input.x + dt*delta.x;
    out.y = input.y - dt*delta.y;
    out.z = input.z;

    do {
        delta = get_xygrid_shift(P, out);

        if (delta.x == HUGE_VAL)
            break;

        dif.x  = out.x - dt*delta.x - input.x;
        dif.y  = out.y + dt*delta.y - input.y;
        out.x += dif.x;
        out.y += dif.y;

    } while ( --i && hypot(dif.x, dif.y) > TOL );

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
        dt = Q->t_obs - Q->t_epoch;
    } else {
        out = proj_coord_error(); /* in the 3D case +t_obs must be specified */
        proj_log_debug(P, "deformation: +t_obs must be specified");
        return out.xyz;
    }

    if (Q->has_xy_grids) {
        shift = get_xygrid_shift(P, in.xyz);

        out.xyz.x += dt * shift.x;
        out.xyz.y += dt * shift.y;
    }

    if (Q->has_z_grids)
        out.xyz.z += dt * get_zgrid_shift(P, in.xyz);

    return out.xyz;
}


static PJ_COORD forward_4d(PJ_COORD in, PJ *P) {
    struct pj_opaque *Q = (struct pj_opaque *) P->opaque;
    double dt;
    XYZ shift;
    PJ_COORD out = in;

    if (Q->t_obs != HUGE_VAL) {
            dt = Q->t_obs - Q->t_epoch;
        } else {
            dt = in.xyzt.t - Q->t_epoch;
    }

    if (Q->has_xy_grids) {
        shift = get_xygrid_shift(P, in.xyz);
        out.xyz.x += dt * shift.x;
        out.xyz.y += dt * shift.y;
    }

    if (Q->has_z_grids)
        out.xyz.z += dt * get_zgrid_shift(P, in.xyz);

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

    if (Q->has_xy_grids) {
        out.xyz = reverse_hshift(P, in, dt);
    }

    if (Q->has_z_grids)
        out.xyz.z = in.z + dt * get_zgrid_shift(P, in);

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

    if (Q->has_xy_grids)
        out.xyz = reverse_hshift(P, in.xyz, dt);

    if (Q->has_z_grids)
        out.xyz.z = in.xyz.z + dt * get_zgrid_shift(P, in.xyz);

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


PJ *PROJECTION(deformation) {
    struct pj_opaque *Q = pj_calloc (1, sizeof (struct pj_opaque));
    if (0==Q)
        return pj_default_destructor(P, ENOMEM);
    P->opaque = (void *) Q;

    Q->cart = proj_create(P->ctx, "+proj=cart");
    if (Q->cart == 0)
        return destructor(P, ENOMEM);

    /* inherit ellipsoid definition from P to Q->cart (simpler than guessing */
    /* how the ellipsoid was specified in original definition)               */
    pj_inherit_ellipsoid_defs(P, Q->cart);

    Q->has_xy_grids = pj_param(P->ctx, P->params, "txy_grids").i;
    Q->has_z_grids  = pj_param(P->ctx, P->params, "tz_grids").i;

    /* Build gridlists. P->gridlist and P->vgridlist_geoid can be empty if */
    /* +xy_grids or +z_grids only ask for optional grids                   */
    if (!Q->has_xy_grids && !Q->has_z_grids) {
        proj_log_error(P, "deformation: At least one of either +xy_grids or +z_grids should be specified.");
        return destructor(P, PJD_ERR_NO_ARGS );
    }

    if (Q->has_xy_grids) {
        Q->has_xy_grids = proj_hgrid_init(P, "xy_grids");
        if (proj_errno(P)) {
            proj_log_error(P, "deformation: could not find requested grid(s).");
            return destructor(P, PJD_ERR_FAILED_TO_LOAD_GRID);
        }
    }

    if (Q->has_z_grids) {
        Q->has_z_grids = proj_vgrid_init(P, "z_grids");
        if (proj_errno(P)) {
            proj_log_error(P, "deformation: could not find requested grid(s).");
            return destructor(P, PJD_ERR_FAILED_TO_LOAD_GRID);
        }
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

    P->fwdobs = P->fwd4d = forward_4d;
    P->invobs = P->fwd4d = reverse_4d;
    P->fwd3d  = forward_3d;
    P->inv3d  = reverse_3d;
    P->fwd    = 0;
    P->inv    = 0;

    P->left  = PJ_IO_UNITS_METERS;
    P->right = PJ_IO_UNITS_METERS;
    P->destructor = destructor;

    return P;
}

int pj_deformation_selftest (void) {return 10000;}
