/******************************************************************************
 * Project:  PROJ.4
 * Purpose:  Inverse operation invocation
 * Author:   Thomas Knudsen,  thokn@sdfe.dk,  2018-01-02
 *           Based on material from Gerald Evenden (original pj_inv)
 *           and Piyush Agram (original pj_inv3d)
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



PJ_COORD pj_inv_prepare (PJ *P, PJ_COORD coo) {
    if (coo.xyz.x == HUGE_VAL) {
        proj_errno_set (P, PJD_ERR_INVALID_X_OR_Y);
        return proj_coord_error ();
    }

    /* de-scale and de-offset */
    coo.xyz.x = (coo.xyz.x * P->to_meter  - P->x0);
    coo.xyz.y = (coo.xyz.y * P->to_meter  - P->y0);
    coo.xyz.z = (coo.xyz.z * P->vto_meter - P->z0);

    /* Classic proj.4 functions expect plane coordinates in units of the semimajor axis  */
    /* Multiplying by ra, rather than dividing by a because the CALCOFI projection       */
    /* stomps on a and hence (apparently) depends on this to roundtrip correctly         */
    /* (CALCOFI avoids further scaling by stomping - a better solution must be possible) */
    if (P->right==PJ_IO_UNITS_CLASSIC) {
        coo.xyz.x *= P->ra;
        coo.xyz.y *= P->ra;
    }

    return coo;
}



PJ_COORD pj_inv_finalize (PJ *P, PJ_COORD coo) {
    if (coo.xyz.x == HUGE_VAL) {
        proj_errno_set (P, PJD_ERR_INVALID_X_OR_Y);
        return proj_coord_error ();
    }

    if (P->left==PJ_IO_UNITS_RADIANS) {
        /* Distance from central meridian, taking system zero meridian into account */
        coo.lp.lam = coo.lp.lam + P->from_greenwich + P->lam0;

         /* adjust longitude to central meridian */
        if (0==P->over)
            coo.lpz.lam = adjlon(coo.lpz.lam);

        /* If input latitude was geocentrical, convert back to geocentrical */
        if (P->geoc)
            coo = proj_geoc_lat (P, PJ_FWD, coo);
    }

    return coo;
}



LP pj_inv(XY xy, PJ *P) {
    PJ_COORD coo = {{0,0,0,0}};
    coo.xy = xy;

    if (!P->skip_inv_prepare)
        coo = pj_inv_prepare (P, coo);
    if (HUGE_VAL==coo.v[0])
        return proj_coord_error ().lp;

    /* Do the transformation, using the lowest dimensional transformer available */
    if (P->inv)
        coo.lp = P->inv(coo.xy, P);
    else if (P->inv3d)
        coo.lpz = P->inv3d (coo.xyz, P);
    else if (P->inv4d)
        coo = P->inv4d (coo, P);
    else {
        proj_errno_set (P, EINVAL);
        return proj_coord_error ().lp;
    }
    if (HUGE_VAL==coo.v[0])
        return proj_coord_error ().lp;

    if (!P->skip_inv_finalize)
        coo = pj_inv_finalize (P, coo);
    return coo.lp;
}



LPZ pj_inv3d (XYZ xyz, PJ *P) {
    PJ_COORD coo = {{0,0,0,0}};
    coo.xyz = xyz;

    if (!P->skip_inv_prepare)
        coo = pj_inv_prepare (P, coo);
    if (HUGE_VAL==coo.v[0])
        return proj_coord_error ().lpz;

    /* Do the transformation, using the lowest dimensional transformer feasible */
    if (P->inv3d)
        coo.lpz = P->inv3d (coo.xyz, P);
    else if (P->inv4d)
        coo = P->inv4d (coo, P);
    else if (P->inv)
        coo.lp = P->inv (coo.xy, P);
    else {
        proj_errno_set (P, EINVAL);
        return proj_coord_error ().lpz;
    }
    if (HUGE_VAL==coo.v[0])
        return proj_coord_error ().lpz;

    if (!P->skip_inv_finalize)
        coo = pj_inv_finalize (P, coo);
    return coo.lpz;
}



PJ_COORD pj_inv4d (PJ_COORD coo, PJ *P) {
    if (!P->skip_inv_prepare)
        coo = pj_inv_prepare (P, coo);
    if (HUGE_VAL==coo.v[0])
        return proj_coord_error ();

    /* Call the highest dimensional converter available */
    if (P->inv4d)
        coo = P->inv4d (coo, P);
    else if (P->inv3d)
        coo.lpz = P->inv3d (coo.xyz, P);
    else if (P->inv)
        coo.lp = P->inv (coo.xy, P);
    else {
        proj_errno_set (P, EINVAL);
        return proj_coord_error ();
    }
    if (HUGE_VAL==coo.v[0])
        return proj_coord_error ();

    if (!P->skip_inv_finalize)
        coo = pj_inv_finalize (P, coo);
    return coo;
}
