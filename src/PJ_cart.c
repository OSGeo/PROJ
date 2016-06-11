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
#include <assert.h>
#include <stddef.h>
#include <math.h>
#include <errno.h>
PROJ_HEAD(cart,    "Geodetic/cartesian conversions");


static void *cart_freeup (PJ *P, int errlev) {         /* Destructor */
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


/**************************************************************
                CARTESIAN / GEODETIC CONVERSIONS
***************************************************************
    This material follows:

    Bernhard Hofmann-Wellenhof & Helmut Moritz:
    Physical Geodesy, 2nd edition.
    Springer, 2005.

    chapter 5.6: Coordinate transformations
    (HM, below),

    and

    Wikipedia: Geographic Coordinate Conversion,
    https://en.m.wikipedia.org/wiki/Geographic_coordinate_conversion

    (WP, below).

    The cartesian-to-geodetic conversion is based on Bowring's
    celebrated method:

    B. R. Bowring:
    Transformation from spatial to geographical coordinates
    Survey Review 23(181), pp. 323-327, 1976

    (BB, below),

    but could probably use some TLC from a newer and faster
    algorithm:

    Toshio Fukushima:
    Transformation from Cartesian to Geodetic Coordinates
    Accelerated by Halley’s Method
    Journal of Geodesy, February 2006

    (TF, below).


    These routines are probably not as robust at those in
    geocent.c, at least thay haven't been through as heavy
    use as their geocent sisters. Some care has been taken
    to avoid singularities, but extreme cases (e.g. setting
    es, the squared eccentricity, to 1), will cause havoc.

**************************************************************/


/*********************************************************************/
static double semiminor_axis (double a, double es) {
/*********************************************************************/
    return a * sqrt (1 - es);
}

/*********************************************************************/
static double normal_radius_of_curvature (double a, double es, double phi) {
/*********************************************************************/
    double s = sin(phi);
    if (es==0)
        return a;
    /* This is from WP.  HM formula 2-149 gives an a,b version */
    return a / sqrt (1 - es*s*s);
}


/*********************************************************************/
static double second_eccentricity_squared (double a, double es) {
/**********************************************************************
    Follows the definition, e'2 = (a2-b2)/b2, but uses the identity
    (a2-b2) = (a-b)(a+b), for improved numerical precision.
***********************************************************************/
    double b = semiminor_axis (a, es);
    return (a - b) * (a + b)  /  (b*b);
}


/*********************************************************************/
static XYZ cartesian (LPZ geodetic,  PJ *P) {
/*********************************************************************/
    double N, h, lam, phi, cosphi = cos(geodetic.phi);
    XYZ xyz;

    N   =  normal_radius_of_curvature(P->a, P->es, geodetic.phi);
    phi =  geodetic.phi;
    lam =  geodetic.lam;
    h   =  geodetic.z;

    /* HM formula 5-27 (z formula follows WP) */
    xyz.x = (N + h) * cosphi * cos(lam);
    xyz.y = (N + h) * cosphi * sin(lam);
    xyz.z = (N * (1 - P->es) + h) * sin(phi);

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
static LPZ geodetic (XYZ cartesian,  PJ *P) {
/*********************************************************************/
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

    /* HM eq. (5-36) (from BB, 1976) */
    c  =  cos(theta);
    s  =  sin(theta);
    lpz.phi  =  atan2 (cartesian.z + e2s*b*s*s*s,  p - P->es*P->a*c*c*c);
    lpz.lam  =  atan2 (cartesian.y, cartesian.x);
    N        =  normal_radius_of_curvature (P->a, P->es, lpz.phi);


    c  =  cos(lpz.phi);
    if (fabs(c) < 1e-3) {
        /* poleward of 89.9427 deg, we avoid division by zero */
        /* by computing the height as the cartesian z value   */
        /* minus the semiminor axis length                    */
        lpz.z =  cartesian.z - (cartesian.z > 0? b: -b);
    }
    else
        lpz.z =  p / c  -  N;

    return lpz;
}



/* Rather pointless, but... */
static XY cart_forward (LP lp, PJ *P) {
    TRIPLET point;
    point.lp = lp;
    point.lpz.z = 0;

    point.xyz = cartesian (point.lpz, P);
    return point.xy;
}

/* Rather pointless, but... */
static LP cart_reverse (XY xy, PJ *P) {
    TRIPLET point;
    point.xy = xy;
    point.xyz.z = 0;

    point.lpz = geodetic (point.xyz, P);
    return point.lp;
}



/*********************************************************************/
PJ *PROJECTION(cart) {
/*********************************************************************/
    P->fwd3d  =  cartesian;  /* or use cart_forward_3d, but add scaling */
    P->inv3d  =  geodetic;   /* or use cart_reverse_3d, but add scaling */
    P->fwd    =  cart_forward;
    P->inv    =  cart_reverse;
    return P;
}

/* selftest stub */
int pj_cart_selftest (void) {return 0;}
