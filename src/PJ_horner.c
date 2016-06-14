/******************************************************************************
 * Project:  PROJ.4
 * Purpose:  2D Horner's scheme for (mostly) datum shifts.
 *
 * Author:   Thomas Knudsen, thokn@sdfe.dk
 *
 ******************************************************************************
 * Copyright (c) 2016, SDFE http://www.sdfe.dk
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

#define PJ_LIB__
#include <projects.h>
#include <geocent.h>
#include <assert.h>
#include <stddef.h>
#include <math.h>
#include <errno.h>
PROJ_HEAD(horner,    "2D Horner's scheme");



static void *horner_freeup (PJ *P, int errlev) {         /* Destructor */
    if (0==P)
        return 0;

    if (0!=errlev)
        pj_ctx_set_errno (P->ctx, errlev);

    if (0==P->opaque)
        return pj_dealloc (P);

    pj_dealloc (P->opaque);
    return pj_dealloc(P);
}


/* Adapts cart_freeup to the format defined for the PJ object */
static void freeup (PJ *P) {
    cart_freeup (P, 0);
    return;
}



/*********************************************************************/
static XYZ horner_3D (LPZ lpz,  PJ *P) {
/*********************************************************************/
    XYZ xyz;
    /*********************************************************************/
    /*                                                                   */
    /* For historical reasons, in proj, plane coordinates are measured   */
    /* in units of the semimajor axis. Since 3D handling is grafted on   */
    /* later, this is not the case for heights. And even though this     */
    /* coordinate really is 3D cartesian, the z-part looks like a height */
    /* to proj. Hence, we have the somewhat unusual situation of having  */
    /* a point coordinate with differing units between dimensions.       */
    /*                                                                   */
    /* The scaling and descaling is handled by the pj_fwd/inv functions. */
    /*                                                                   */
    /*********************************************************************/
    xyz.x /= P->a;
    xyz.y /= P->a;

    return xyz;
}


/*********************************************************************/
static LPZ horner_inverse_3D (XYZ xyz,  PJ *P) {
/*********************************************************************/
    LPZ lpz;

    return lpz;
}



/*********************************************************************/
static XY horner_forward_2D (LP lp, PJ *P) {
/*********************************************************************/
    TRIPLET point;
    point.lp = lp;
    point.lpz.z = 0;

    point.xyz = cartesian (point.lpz, P);
    return point.xy;
}

/*********************************************************************/
static LP horner_inverse_2D (XY xy, PJ *P) {
/*********************************************************************/
    TRIPLET point;
    point.xy = xy;
    point.xyz.z = 0;

    point.lpz = geodetic (point.xyz, P);
    return point.lp;
}



/*********************************************************************/
PJ *PROJECTION(horner) {
/*********************************************************************/
    P->fwd3d  =  cartesian;  /* or use cart_forward_3d, but add scaling */
    P->inv3d  =  geodetic;   /* or use cart_reverse_3d, but add scaling */
    P->fwd    =  cart_forward;
    P->inv    =  cart_reverse;
    return P;
}

/* selftest stub */
int pj_cart_selftest (void) {return 0;}
