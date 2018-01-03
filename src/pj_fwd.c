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

PJ_COORD pj_fwd_prepare (PJ *P, PJ_COORD coo) {
    if (HUGE_VAL==coo.v[0])
        return proj_coord_error ();

    /* Check validity of angular input coordinates */
    if (P->left==PJ_IO_UNITS_RADIANS) {
        double t;
        LP lp = coo.lp;

        /* check for latitude or longitude over-range */
        t = (lp.phi < 0  ?  -lp.phi  :  lp.phi) - M_HALFPI;
        if (t > PJ_EPS_LAT  ||  lp.lam > 10  ||  lp.lam < -10) {
            proj_errno_set (P, PJD_ERR_LAT_OR_LON_EXCEED_LIMIT);
            return proj_coord_error ();
        }

        /* Clamp latitude to -90..90 degree range */
        if (lp.phi > M_HALFPI)
            lp.phi = M_HALFPI;
        if (lp.phi < -M_HALFPI)
            lp.phi = -M_HALFPI;

        /* If input latitude is geocentrical, convert to geographical */
        if (P->geoc)
            coo = proj_geoc_lat (P, PJ_INV, coo);

        /* Distance from central meridian, taking system zero meridian into account */
        lp.lam = (lp.lam - P->from_greenwich) - P->lam0;

        /* Ensure longitude is in the -pi:pi range */
        if (0==P->over)
            lp.lam = adjlon(lp.lam);

        coo.lp = lp;
    }

    return coo;
}


PJ_COORD pj_fwd_finalize (PJ *P, PJ_COORD coo) {

    /* Classic proj.4 functions return plane coordinates in units of the semimajor axis */
    if (P->right==PJ_IO_UNITS_CLASSIC) {
        coo.xy.x *= P->a;
        coo.xy.y *= P->a;
    }

    /* Handle false eastings/northings and non-metric linear units */
    coo.xyz.x = P->fr_meter  * (coo.xyz.x + P->x0);
    coo.xyz.y = P->fr_meter  * (coo.xyz.y + P->y0);
    coo.xyz.z = P->vfr_meter * (coo.xyz.z + P->z0);

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
