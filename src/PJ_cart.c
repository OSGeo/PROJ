/******************************************************************************
 * Project:  PROJ.4
 * Purpose:  Convert between ellipsoidal, geodetic coordinates and
 *           cartesian, geocentric coordinates.
 *
 *           Formally, this functionality is also found in the PJ_geocent.c
 *           code.
 *
 *           Actually, however, the PJ_geocent transformations are carried
 *           out in concert between 2D stubs in PJ_geocent.c and 3D code
 *           placed in pj_transform.c.
 *
 *           For pipeline-style datum shifts, we do need direct access
 *           to the full 3D interface for this functionality.
 *
 *           Hence this code, which may look like "just another PJ_geocent"
 *           but really is something substantially different.
 *
 * Author:   Thomas Knudsen, thokn@sdfe.dk
 *
 ******************************************************************************
 * Copyright (c) 2016, Thomas Knudsen / SDFE
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
#include <PJ_pipeline.h>
#include <assert.h>
#include <stddef.h>
#include <math.h>
#include <errno.h>
PROJ_HEAD(cart,    "Ellipsoidal-to-cartesian");


/* Projection specific elements for the PJ object */
struct pj_opaque {
    GeocentricInfo g;
};




static COORDINATE cart_3d (COORDINATE point, int direction, PJ *P) {
    long ret;
    double X, Y, Z, LAM, PHI;
    COORDINATE result = {{HUGE_VAL, HUGE_VAL, HUGE_VAL}};

    if (direction == 0) {
        ret = pj_Convert_Geodetic_To_Geocentric (
            &(P->opaque->g),
            point.lpz.phi,
            point.lpz.lam,
            point.lpz.z,
            &X,
            &Y,
            &Z
        );


        if (GEOCENT_NO_ERROR != ret)
            return result;

        result.xyz.x = X / P->a;
        result.xyz.y = Y / P->a;
        result.xyz.z = Z;

        return result;
    }

    pj_show_coordinate ("cart1: ", point, 1);
    pj_Convert_Geocentric_To_Geodetic (
        &(P->opaque->g),
        point.xyz.x,
        point.xyz.y,
        point.xyz.z,
        &PHI,
        &LAM,
        &Z
    );

    result.lpz.lam = LAM * P->a;
    result.lpz.phi = PHI * P->a;
    result.lpz.z   =   Z;
pj_show_coordinate ("cart2: ", result, 1);
    return result;
}




static XYZ cart_forward_3d (LPZ lpz, PJ *P) {
    COORDINATE point;
    point.lpz = lpz;
    pj_log( P->ctx, 5, "CART_FORWARD_3D");
    point = cart_3d (point, 0, P);
    return point.xyz;
}

static LPZ cart_reverse_3d (XYZ xyz, PJ *P) {
    COORDINATE point;
    point.xyz = xyz;
    pj_log( P->ctx, 5, "CART_REVERSE_3D");
    point = cart_3d (point, 1, P);
    return point.lpz;
}

static XY cart_forward (LP lp, PJ *P) {
    COORDINATE point;
    point.lp = lp;
    point.lpz.z = 0;

    point = cart_3d (point, 0, P);
    return point.xy;
}

static LP cart_reverse (XY xy, PJ *P) {
    COORDINATE point;
    point.xy = xy;
    point.xyz.z = 0;

    point = cart_3d (point, 1, P);
    return point.lp;
}




static void *cart_freeup (PJ *P, int errlev) {         /* Destructor */
    if (0==P)
        return 0;

    pj_ctx_set_errno (P->ctx, errlev);

    if (0==P->opaque)
        return pj_dealloc (P);

    pj_dealloc (P->opaque);
    return pj_dealloc(P);
}


/* Adapts pipeline_freeup to the format defined for the PJ object */
static void freeup (PJ *P) {
    cart_freeup (P, 0);
    return;
}





/**************************************************************
                CARTESIAN / GEODETIC CONVERSIONS
***************************************************************
    This material follows:

    Bernhard Hofmann-Wellenhof & Helmut Moritz:
    Physical Geodesy, 2nd edition.
    Springer, 2005.

    chapter 5.6: Coordinate transformations
    (HM, below)

    and

    Wikipedia: Geographic Coordinate Conversion,
    https://en.m.wikipedia.org/wiki/Geographic_coordinate_conversion

    (WP, below)

    The code is probably not as robust at that in geocent.c,
    since no checks for e.g. latitude = +/-90 is done (which,
    making cos(phi) = 0, will trip up the computation of the
    geodetic height-coordinate).

**************************************************************/

static double semiminor_axis (double a, double es) {
    return a * sqrt (1 - es);
}

static double normal_radius_of_curvature (double a, double es, double phi) {
    double s = sin(phi);
    /* This is from WP.  HM formula 2-149 gives an a,b version */
    return a / sqrt (1 - es*s*s);
}


/*********************************************************************/
static double second_eccentricity_squared (double a, double es) {
/**********************************************************************
    Follows the definition, but uses the identity (a2-b2) = (a-b)(a+b)
    for improved numerical precision.
***********************************************************************/
    double b = semiminor_axis (a, es);
    return (a - b) * (a + b)  /  (b*b);
}

static XYZ cartesian (LPZ geodetic,  PJ *P) {
    double N, h, lam, phi, cosphi = cos(geodetic.phi);
    XYZ xyz;

    /* pj_log_coordinate (P->ctx, 5, "Cartesian - input:  ", geodetic, 1); */

    N   =  normal_radius_of_curvature(P->a, P->es, geodetic.phi);

    phi =  geodetic.phi;
    lam =  geodetic.lam;
    h   =  geodetic.z;

    /* HM formula 5-27 (z formula follows WP) */
    xyz.x = (N + h) * cosphi * cos(lam);
    xyz.y = (N + h) * cosphi * sin(lam);
    xyz.z = (N * (1 - P->es) + h) * sin(phi);
    /* pj_log_coordinate (P->ctx, 5, "Cartesian - output: ", xyz, 0); */

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


static LPZ geodetic (XYZ cartesian,  PJ *P) {
    double N, b, p, theta, c, s, e2s;
    LPZ lpz;

    cartesian.x *= P->a;
    cartesian.y *= P->a;

    /* Perpendicular distance from point to Z-axis (HM eq. 5-28) */
    p = hypot (cartesian.x, cartesian.y);

    /* Ancillary ellipsoidal parameters */
    b   =  semiminor_axis (P->a, P->es);
    e2s =  second_eccentricity_squared (P->a,  P->es);

    /* HM eq. (5-37) */
    theta  =  atan2 (cartesian.z * P->a,  p * b);

    /* HM eq. (5-36) */
    c  =  cos(theta);
    s  =  sin(theta);
    lpz.phi  =  atan2 (cartesian.z + e2s*b*s*s*s,  p - P->es*P->a*c*c*c);
    lpz.lam  =  atan2 (cartesian.y, cartesian.x);
    N        =  normal_radius_of_curvature (P->a, P->es, lpz.phi);
    /* should switch to something more robust close to the poles */
    lpz.z    =  p / cos (lpz.phi)  -  N;

    /* pj_log_coordinate (P->ctx, 5, "Geodetic - output: ", lpz, 0); */
    return lpz;
}


PJ *PROJECTION(cart) {
    int err;
    double semiminor;

    P->fwd3d  =  cart_forward_3d;   /* or use cartesian; */
    P->inv3d  =  cart_reverse_3d;   /* or use geodetic;  */
    P->fwd    =  cart_forward;
    P->inv    =  cart_reverse;
    P->opaque =  pj_calloc (1, sizeof(struct pj_opaque));
    if (0==P->opaque)
        return 0;

    semiminor = P->a * sqrt (1 - P->es);
    err = pj_Set_Geocentric_Parameters (&(P->opaque->g), P->a, semiminor);
    if (GEOCENT_NO_ERROR != err)
        return cart_freeup (P, -6); /* the "e effectively==1" message, abused as generic geocentric error */

    return P;
}

/* selftest stub */
int pj_cart_selftest (void) {return 0;}
