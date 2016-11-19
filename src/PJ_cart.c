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
#include <proj.h>
#include <projects.h>
#include <assert.h>
#include <stddef.h>
#include <math.h>
#include <errno.h>
PROJ_HEAD(cart,    "Geodetic/cartesian conversions");


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


static void freeup (PJ *P) {
    pj_freeup_plain (P);
    return;
}


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
    if (fabs(c) < 1e-6) {
        /* poleward of 89.99994 deg, we avoid division by zero */
        /* by computing the height as the cartesian z value    */
        /* minus the semiminor axis length                     */
        /* (potential improvement: approx. by 2nd order poly)  */
        lpz.z = cartesian.z - (cartesian.z > 0? b: -b);
    }
    else
        lpz.z =  p / c  -  N;

    return lpz;
}



/* In effect, 2 cartesian coordinates of a point on the ellipsoid. Rather pointless, but... */
static XY cart_forward (LP lp, PJ *P) {
    PJ_TRIPLET point;
    point.lp = lp;
    point.lpz.z = 0;

    point.xyz = cartesian (point.lpz, P);
    return point.xy;
}

/* And the other way round. Still rather pointless, but... */
static LP cart_reverse (XY xy, PJ *P) {
    PJ_TRIPLET point;
    point.xy = xy;
    point.xyz.z = 0;

    point.lpz = geodetic (point.xyz, P);
    return point.lp;
}



/*********************************************************************/
PJ *PROJECTION(cart) {
/*********************************************************************/
    P->fwd3d  =  cartesian;
    P->inv3d  =  geodetic;
    P->fwd    =  cart_forward;
    P->inv    =  cart_reverse;
    return P;
}

#ifndef PJ_SELFTEST
/* selftest stub */
int pj_cart_selftest (void) {return 0;}
#else
/* Testing quite a bit of the pj_obs_api as a side effect (inspired by pj_obs_api_test.c) */
int pj_cart_selftest (void) {
    PJ *P;
    PJ_OBS a, b;
    int err;
    double dist;
    char *args[3] = {"proj=utm", "zone=32", "ellps=GRS80"};


    /* Log everything libproj offers to log for you */
    pj_log_level (0, PJ_LOG_TRACE);

    /* An utm projection on the GRS80 ellipsoid */
    P = pj_create ("+proj=utm +zone=32 +ellps=GRS80");
    if (0==P)
        return 1;

    /* Clean up */
    pj_free (P);

    /* Same projection, now using argc/argv style initialization */
    P = pj_create_argv (3, args);
    if (0==P)
        return puts ("Oops"), 0;

    /* Turn off logging */
    pj_log_level (0, PJ_LOG_NONE);

    /* zero initialize everything, then set (longitude, latitude) to (12, 55) */
    a = pj_obs_null;
    /* a.coo.lp: The coordinate part of a, interpreted as a classic LP pair */
    a.coo.lp.lam = TORAD(12);
    a.coo.lp.phi = TORAD(55);

    /* Forward projection */
    b = pj_trans (P, PJ_FWD, a);

    /* Inverse projection */
    a = pj_trans (P, PJ_INV, b);

    /* Null projection */
    a = pj_trans (P, PJ_IDENT, a);

    /* Forward again, to get two linear items for comparison */
    a = pj_trans (P, PJ_FWD, a);

    dist = pj_xy_dist (a.coo.xy, b.coo.xy);
    if (dist > 2e-9)
        return 2;

    /* Invalid projection */
    a = pj_trans (P, 42, a);
    if (a.coo.lpz.lam!=DBL_MAX)
        return 3;
    err = pj_error (P);
    if (0==err)
        return 4;

    /* Clear error */
    pj_error_set (P, 0);

    /* Clean up */
    pj_free (P);

    /* Now do some 3D transformations */
    P = pj_create ("+proj=cart +ellps=GRS80");
    if (0==P)
        return 5;

    /* zero initialize everything, then set (longitude, latitude, height) to (12, 55, 100) */
    a = b = pj_obs_null;
    a.coo.lpz.lam = TORAD(12);
    a.coo.lpz.phi = TORAD(55);
    a.coo.lpz.z   = 100;

    /* Forward projection: 3D-Cartesian-to-Ellipsoidal */
    b = pj_trans (P, PJ_FWD, a);

    /* Check roundtrip precision for 10000 iterations each way */
    dist = pj_roundtrip (P, PJ_FWD, 10000, a);
    dist = pj_roundtrip (P, PJ_INV, 10000, b);
    if (dist > 2e-9)
        return 6;

    /* Inverse projection: Ellipsoidal-to-3D-Cartesian */
    b = pj_trans (P, PJ_INV, b);

    /* Move p to another context */
    pj_context_renew (P);
    b = pj_trans (P, PJ_FWD, b);

    /* Move it back to the default context */
    pj_context_free (P);
    b = pj_trans (P, PJ_INV, b);

    pj_free (P);
    return 0;
}
#endif
