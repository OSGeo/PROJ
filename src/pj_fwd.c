/******************************************************************************
 * Project:  PROJ.4
 * Purpose:  Forward operation invocation
 * Author:   Thomas Knudsen,  thokn@sdfe.dk,  2018-01-02
 *           Based on material from Gerald Evenden (original pj_fwd)
 *           and Piyush Agram (original pj_fwd3d)
 *
 ******************************************************************************
 * Copyright (c) 2000, Frank Warmerdam
 * Copyright (c) 2018, Thomas Knudsen / SDFE
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

#include "proj_internal.h"
#include "projects.h"

#define INPUT_UNITS  P->left
#define OUTPUT_UNITS P->right


static PJ_COORD pj_fwd_prepare (PJ *P, PJ_COORD coo) {
    if (HUGE_VAL==coo.v[0])
        return proj_coord_error ();

    /* The helmert datum shift will choke unless it gets a sensible 4D coordinate */
    if (HUGE_VAL==coo.v[2] && P->helmert) coo.v[2] = 0.0;
    if (HUGE_VAL==coo.v[3] && P->helmert) coo.v[3] = 0.0;

    /* Check validity of angular input coordinates */
    if (INPUT_UNITS==PJ_IO_UNITS_ANGULAR) {
        double t;

        /* check for latitude or longitude over-range */
        t = (coo.lp.phi < 0  ?  -coo.lp.phi  :  coo.lp.phi) - M_HALFPI;
        if (t > PJ_EPS_LAT  ||  coo.lp.lam > 10  ||  coo.lp.lam < -10) {
            proj_errno_set (P, PJD_ERR_LAT_OR_LON_EXCEED_LIMIT);
            return proj_coord_error ();
        }

        /* Clamp latitude to -90..90 degree range */
        if (coo.lp.phi > M_HALFPI)
            coo.lp.phi = M_HALFPI;
        if (coo.lp.phi < -M_HALFPI)
            coo.lp.phi = -M_HALFPI;

        /* If input latitude is geocentrical, convert to geographical */
        if (P->geoc)
            coo = proj_geocentric_latitude (P, PJ_INV, coo);

        /* Ensure longitude is in the -pi:pi range */
        if (0==P->over)
            coo.lp.lam = adjlon(coo.lp.lam);

        if (P->hgridshift)
            coo = proj_trans (P->hgridshift, PJ_INV, coo);
        else if (P->helmert) {
            coo = proj_trans (P->cart_wgs84, PJ_FWD, coo); /* Go cartesian in WGS84 frame */
            coo = proj_trans (P->helmert,    PJ_FWD, coo); /* Step into local frame */
            coo = proj_trans (P->cart,       PJ_INV, coo); /* Go back to angular using local ellps */
        }
        if (P->vgridshift)
            coo = proj_trans (P->vgridshift, PJ_FWD, coo);

        /* Distance from central meridian, taking system zero meridian into account */
        coo.lp.lam = (coo.lp.lam - P->from_greenwich) - P->lam0;

        /* Ensure longitude is in the -pi:pi range */
        if (0==P->over)
            coo.lp.lam = adjlon(coo.lp.lam);

        return coo;
    }


    /* We do not support gridshifts on cartesian input */
    if (INPUT_UNITS==PJ_IO_UNITS_CARTESIAN && P->helmert)
            return proj_trans (P->helmert, PJ_FWD, coo);
    return coo;
}



static PJ_COORD pj_fwd_finalize (PJ *P, PJ_COORD coo) {

    switch (OUTPUT_UNITS) {

    /* Handle false eastings/northings and non-metric linear units */
    case PJ_IO_UNITS_CARTESIAN:

        if (P->is_geocent) {
            coo = proj_trans (P->cart, PJ_FWD, coo);
        }

        coo.xyz.x = P->fr_meter * (coo.xyz.x + P->x0);
        coo.xyz.y = P->fr_meter * (coo.xyz.y + P->y0);
        coo.xyz.z = P->fr_meter * (coo.xyz.z + P->z0);
        break;

    /* Classic proj.4 functions return plane coordinates in units of the semimajor axis */
    case PJ_IO_UNITS_CLASSIC:
        coo.xy.x *= P->a;
        coo.xy.y *= P->a;

    /* Falls through */ /* (<-- GCC warning silencer) */
    /* to continue processing in common with PJ_IO_UNITS_PROJECTED */
    case PJ_IO_UNITS_PROJECTED:
        coo.xyz.x = P->fr_meter  * (coo.xyz.x + P->x0);
        coo.xyz.y = P->fr_meter  * (coo.xyz.y + P->y0);
        coo.xyz.z = P->vfr_meter * (coo.xyz.z + P->z0);
        break;

    case PJ_IO_UNITS_WHATEVER:
        break;

    case PJ_IO_UNITS_ANGULAR:
        if (INPUT_UNITS==PJ_IO_UNITS_ANGULAR)
            break;

        /* adjust longitude to central meridian */
        if (0==P->over)
            coo.lpz.lam = adjlon(coo.lpz.lam);

        if (P->vgridshift)
            coo = proj_trans (P->vgridshift, PJ_INV, coo);
        if (P->hgridshift)
            coo = proj_trans (P->hgridshift, PJ_FWD, coo);
        else if (P->helmert) {
            coo = proj_trans (P->cart_wgs84, PJ_FWD, coo); /* Go cartesian in WGS84 frame */
            coo = proj_trans (P->helmert,    PJ_FWD, coo); /* Step into local frame */
            coo = proj_trans (P->cart,       PJ_INV, coo); /* Go back to angular using local ellps */
        }

        /* If input latitude was geocentrical, convert back to geocentrical */
        if (P->geoc)
            coo = proj_geocentric_latitude (P, PJ_FWD, coo);


        /* Distance from central meridian, taking system zero meridian into account */
        coo.lp.lam = coo.lp.lam + P->from_greenwich + P->lam0;

        /* adjust longitude to central meridian */
        if (0==P->over)
            coo.lpz.lam = adjlon(coo.lpz.lam);
    }

    if (P->axisswap)
        coo = proj_trans (P->axisswap, PJ_FWD, coo);

    return coo;
}



XY pj_fwd(LP lp, PJ *P) {
    PJ_COORD coo = {{0,0,0,0}};
    coo.lp = lp;

    if (!P->skip_fwd_prepare)
        coo = pj_fwd_prepare (P, coo);
    if (HUGE_VAL==coo.v[0])
        return proj_coord_error ().xy;

    /* Do the transformation, using the lowest dimensional transformer available */
    if (P->fwd)
        coo.xy = P->fwd(coo.lp, P);
    else if (P->fwd3d)
        coo.xyz = P->fwd3d (coo.lpz, P);
    else if (P->fwd4d)
        coo = P->fwd4d (coo, P);
    else {
        proj_errno_set (P, EINVAL);
        return proj_coord_error ().xy;
    }
    if (HUGE_VAL==coo.v[0])
        return proj_coord_error ().xy;

    if (!P->skip_fwd_finalize)
        coo = pj_fwd_finalize (P, coo);
    return coo.xy;
}



XYZ pj_fwd3d(LPZ lpz, PJ *P) {
    PJ_COORD coo = {{0,0,0,0}};
    coo.lpz = lpz;

    if (!P->skip_fwd_prepare)
        coo = pj_fwd_prepare (P, coo);
    if (HUGE_VAL==coo.v[0])
        return proj_coord_error ().xyz;

    /* Do the transformation, using the lowest dimensional transformer feasible */
    if (P->fwd3d)
        coo.xyz = P->fwd3d(coo.lpz, P);
    else if (P->fwd4d)
        coo = P->fwd4d (coo, P);
    else if (P->fwd)
        coo.xy = P->fwd (coo.lp, P);
    else {
        proj_errno_set (P, EINVAL);
        return proj_coord_error ().xyz;
    }
    if (HUGE_VAL==coo.v[0])
        return proj_coord_error ().xyz;

    if (!P->skip_fwd_finalize)
        coo = pj_fwd_finalize (P, coo);
    return coo.xyz;
}



PJ_COORD pj_fwd4d (PJ_COORD coo, PJ *P) {
    if (!P->skip_fwd_prepare)
        coo = pj_fwd_prepare (P, coo);
    if (HUGE_VAL==coo.v[0])
        return proj_coord_error ();

    /* Call the highest dimensional converter available */
    if (P->fwd4d)
        coo = P->fwd4d (coo, P);
    else if (P->fwd3d)
        coo.xyz  =  P->fwd3d (coo.lpz, P);
    else if (P->fwd)
        coo.xy  =  P->fwd (coo.lp, P);
    else {
        proj_errno_set (P, EINVAL);
        return proj_coord_error ();
    }
    if (HUGE_VAL==coo.v[0])
        return proj_coord_error ();

    if (!P->skip_fwd_finalize)
        coo = pj_fwd_finalize (P, coo);
    return coo;
}
