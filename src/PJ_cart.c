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
#include "proj_internal.h"
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

    Close to the poles, we avoid singularities by switching to an
    approximation requiring knowledge of the geocentric radius
    at the given latitude. For this, we use an adaptation of the
    formula given in:

    Wikipedia: Earth Radius
    https://en.wikipedia.org/wiki/Earth_radius#Radius_at_a_given_geodetic_latitude
    (Derivation and commentary at http://gis.stackexchange.com/questions/20200/how-do-you-compute-the-earths-radius-at-a-given-geodetic-latitude)

    (WP2, below)

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
static double normal_radius_of_curvature (double a, double es, double phi) {
/*********************************************************************/
    double s = sin(phi);
    if (es==0)
        return a;
    /* This is from WP.  HM formula 2-149 gives an a,b version */
    return a / sqrt (1 - es*s*s);
}

/*********************************************************************/
static double geocentric_radius (double a, double b, double phi) {
/*********************************************************************/
    /* This is from WP2, but uses hypot() for potentially better numerical robustness */
    return hypot (a*a*cos (phi), b*b*sin(phi)) / hypot (a*cos(phi), b*sin(phi));
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

    return xyz;
}


/*********************************************************************/
static LPZ geodetic (XYZ cartesian,  PJ *P) {
/*********************************************************************/
    double N, b, p, theta, c, s, e2s;
    LPZ lpz;

    /* Perpendicular distance from point to Z-axis (HM eq. 5-28) */
    p = hypot (cartesian.x, cartesian.y);

    /* Ancillary ellipsoidal parameters */
    b   =  P->b;
    e2s =  P->e2s;

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
        /* poleward of 89.99994 deg, we avoid division by zero   */
        /* by computing the height as the cartesian z value      */
        /* minus the geocentric radius of the Earth at the given */
        /* latitude                                              */
        double r = geocentric_radius (P->a, b, lpz.phi);
        lpz.z = fabs (cartesian.z) - r;
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
    P->left   =  PJ_IO_UNITS_RADIANS;
    P->right  =  PJ_IO_UNITS_METERS;
    return P;
}

#ifndef PJ_SELFTEST
/* selftest stub */
int pj_cart_selftest (void) {return 0;}
#else
/* Testing quite a bit of the pj_obs_api as a side effect (inspired by pj_obs_api_test.c) */
int pj_cart_selftest (void) {
    PJ_CONTEXT *ctx;
    PJ *P;
    PJ_OBS a, b, obs[2];
    PJ_COORD coord[2];

    PJ_INFO info;
    PJ_PROJ_INFO pj_info;
    PJ_GRID_INFO grid_info;
    PJ_INIT_INFO init_info;

    PJ_DERIVS derivs;
    PJ_FACTORS factors;

    int err;
    size_t n, sz;
    double dist, h, t;
    char *args[3] = {"proj=utm", "zone=32", "ellps=GRS80"};
    char *arg = {"+proj=utm +zone=32 +ellps=GRS80"};
    char buf[40];

    /* An utm projection on the GRS80 ellipsoid */
    P = proj_create (0, arg);
    if (0==P)
        return 1;


    /* Clean up */
    proj_destroy (P);

    /* Same projection, now using argc/argv style initialization */
    P = proj_create_argv (0, 3, args);
    if (0==P)
        return 2;

    /* zero initialize everything, then set (longitude, latitude) to (12, 55) */
    a = proj_obs_null;
    /* a.coo.lp: The coordinate part of a, interpreted as a classic LP pair */
    a.coo.lp.lam = PJ_TORAD(12);
    a.coo.lp.phi = PJ_TORAD(55);

    /* Forward projection */
    b = proj_trans_obs (P, PJ_FWD, a);

    /* Inverse projection */
    a = proj_trans_obs (P, PJ_INV, b);

    /* Null projection */
    a = proj_trans_obs (P, PJ_IDENT, a);

    /* Forward again, to get two linear items for comparison */
    a = proj_trans_obs (P, PJ_FWD, a);

    dist = proj_xy_dist (a.coo.xy, b.coo.xy);
    if (dist > 2e-9)
        return 3;

    /* Clear any previous error */
    proj_errno_set (P, 0);

    /* Invalid projection */
    a = proj_trans_obs (P, 42, a);
    if (a.coo.lpz.lam!=HUGE_VAL)
        return 4;
    err = proj_errno (P);
    if (0==err)
        return 5;

    /* Clear error again */
    proj_errno_set (P, 0);

    /* Clean up */
    proj_destroy (P);

    /* Now do some 3D transformations */
    P = proj_create (0, "+proj=cart +ellps=GRS80");
    if (0==P)
        return 6;

    /* zero initialize everything, then set (longitude, latitude, height) to (12, 55, 100) */
    a = b = proj_obs_null;
    a.coo.lpz.lam = PJ_TORAD(12);
    a.coo.lpz.phi = PJ_TORAD(55);
    a.coo.lpz.z   = 100;

    /* Forward projection: 3D-Cartesian-to-Ellipsoidal */
    b = proj_trans_obs (P, PJ_FWD, a);

    /* Check roundtrip precision for 10000 iterations each way */
    dist = proj_roundtrip (P, PJ_FWD, 10000, a);
    dist = proj_roundtrip (P, PJ_INV, 10000, b);
    if (dist > 2e-9)
        return 7;


    /* Test at the North Pole */
    a = b = proj_obs_null;
    a.coo.lpz.lam = PJ_TORAD(0);
    a.coo.lpz.phi = PJ_TORAD(90);
    a.coo.lpz.z   = 100;

    /* Forward projection: Ellipsoidal-to-3D-Cartesian */
    dist = proj_roundtrip (P, PJ_FWD, 1, a);
    if (dist > 1e-12)
        return 8;

    /* Test at the South Pole */
    a = b = proj_obs_null;
    a.coo.lpz.lam = PJ_TORAD(0);
    a.coo.lpz.phi = PJ_TORAD(-90);
    a.coo.lpz.z   = 100;

    /* Forward projection: Ellipsoidal-to-3D-Cartesian */
    dist = proj_roundtrip (P, PJ_FWD, 1, a);
    if (dist > 1e-12)
        return 9;

    /* Inverse projection: 3D-Cartesian-to-Ellipsoidal */
    b = proj_trans_obs (P, PJ_INV, b);

    /* Move p to another context */
    ctx = proj_context_create ();
    if (ctx==pj_get_default_ctx())
        return 10;
    proj_context_set (P, ctx);
    if (ctx != P->ctx)
        return 11;
    b = proj_trans_obs (P, PJ_FWD, b);

    /* Move it back to the default context */
    proj_context_set (P, 0);
    if (pj_get_default_ctx() != P->ctx)
        return 12;
    proj_context_destroy (ctx);

    /* We go on with the work - now back on the default context */
    b = proj_trans_obs (P, PJ_INV, b);
    proj_destroy (P);


    /* Testing the proj_transform nightmare */

    /* An utm projection on the GRS80 ellipsoid */
    P = proj_create (0, "+proj=utm +zone=32 +ellps=GRS80");
    if (0==P)
        return 13;

    obs[0].coo = proj_coord (PJ_TORAD(12), PJ_TORAD(55), 45, 0);
    obs[1].coo = proj_coord (PJ_TORAD(12), PJ_TORAD(56), 50, 0);
    sz = sizeof (PJ_OBS);

    /* Forward projection */
    a = proj_trans_obs (P, PJ_FWD, obs[0]);
    b = proj_trans_obs (P, PJ_FWD, obs[1]);

    n = proj_transform (
        P, PJ_FWD, 
        &(obs[0].coo.lpz.lam), sz, 2,
        &(obs[0].coo.lpz.phi), sz, 2,
        &(obs[0].coo.lpz.z),   sz, 2, 
        0,                     sz, 0
    );
    if (2!=n)
        return 14;
    if (a.coo.lpz.lam != obs[0].coo.lpz.lam)  return 15;
    if (a.coo.lpz.phi != obs[0].coo.lpz.phi)  return 16;
    if (a.coo.lpz.z   != obs[0].coo.lpz.z)    return 17;
    if (b.coo.lpz.lam != obs[1].coo.lpz.lam)  return 18;
    if (b.coo.lpz.phi != obs[1].coo.lpz.phi)  return 19;
    if (b.coo.lpz.z   != obs[1].coo.lpz.z)    return 20;

    /* now test the case of constant z */
    obs[0].coo = proj_coord (PJ_TORAD(12), PJ_TORAD(55), 45, 0);
    obs[1].coo = proj_coord (PJ_TORAD(12), PJ_TORAD(56), 50, 0);
    h = 27;
    t = 33;
    n = proj_transform (
        P, PJ_FWD, 
        &(obs[0].coo.lpz.lam), sz, 2,
        &(obs[0].coo.lpz.phi), sz, 2,
        &h,                     0, 1, 
        &t,                     0, 1
    );
    if (2!=n)
        return 21;
    if (a.coo.lpz.lam != obs[0].coo.lpz.lam)  return 22;
    if (a.coo.lpz.phi != obs[0].coo.lpz.phi)  return 23;
    if (45            != obs[0].coo.lpz.z)    return 24;
    if (b.coo.lpz.lam != obs[1].coo.lpz.lam)  return 25;
    if (b.coo.lpz.phi != obs[1].coo.lpz.phi)  return 26;
    if (50            != obs[1].coo.lpz.z)    return 27; /* NOTE: unchanged */
    if (50==h) return 28;

    /* test proj_transform_obs() */

    obs[0].coo = proj_coord (PJ_TORAD(12), PJ_TORAD(55), 45, 0);
    obs[1].coo = proj_coord (PJ_TORAD(12), PJ_TORAD(56), 50, 0);

    if (proj_transform_obs(P, PJ_FWD, 2, obs))
        return 30;

    if (a.coo.lpz.lam != obs[0].coo.lpz.lam)  return 31;
    if (a.coo.lpz.phi != obs[0].coo.lpz.phi)  return 32;
    if (a.coo.lpz.z   != obs[0].coo.lpz.z)    return 33;
    if (b.coo.lpz.lam != obs[1].coo.lpz.lam)  return 34;
    if (b.coo.lpz.phi != obs[1].coo.lpz.phi)  return 35;
    if (b.coo.lpz.z   != obs[1].coo.lpz.z)    return 36;

    /* test proj_transform_coord() */

    coord[0] = proj_coord (PJ_TORAD(12), PJ_TORAD(55), 45, 0);
    coord[1] = proj_coord (PJ_TORAD(12), PJ_TORAD(56), 50, 0);
    if (proj_transform_coord(P, PJ_FWD, 2, coord))
        return 40;

    if (a.coo.lpz.lam != coord[0].lpz.lam)  return 41;
    if (a.coo.lpz.phi != coord[0].lpz.phi)  return 42;
    if (a.coo.lpz.z   != coord[0].lpz.z)    return 43;
    if (b.coo.lpz.lam != coord[1].lpz.lam)  return 44;
    if (b.coo.lpz.phi != coord[1].lpz.phi)  return 45;
    if (b.coo.lpz.z   != coord[1].lpz.z)    return 46;

    /* Clean up  after transform* tests */
    proj_destroy (P);

    /* test proj_create_crs_to_crs() */
    P = proj_create_crs_to_crs(0, "epsg:25832", "epsg:25833");
    if (P==0)
        return 50;

    a.coo.xy.x =  700000.0;
    a.coo.xy.y = 6000000.0;
    b.coo.xy.x =  307788.8761171057;
    b.coo.xy.y = 5999669.3036037628;

    a = proj_trans_obs(P, PJ_FWD, a);
    if (dist > 1e-7)
        return 51;
    proj_destroy(P);

    /* let's make sure that only entries in init-files results in a usable PJ */
    P = proj_create_crs_to_crs(0, "proj=utm +zone=32 +datum=WGS84", "proj=utm +zone=33 +datum=WGS84");
    if (P != 0) {
        proj_destroy(P);
        return 52;
    }
    proj_destroy(P);

    /* ********************************************************************** */
    /*                          Test info functions                           */
    /* ********************************************************************** */

    /* proj_info()                                                            */
    /* this one is difficult to test, since the output changes with the setup */
    info = proj_info();
    if (info.version) {
        char tmpstr[64];
        sprintf(tmpstr, "%d.%d.%d", info.major, info.minor, info.patch);
        if (strcmp(info.version, tmpstr)) return 55;
    }
    if (!info.release)    return 56;
    if (!info.searchpath) return 57;

    /* proj_pj_info() */
    P = proj_create(0, "+proj=august"); /* august has no inverse */
    if (proj_pj_info(P).has_inverse) { proj_destroy(P); return 60; }
    proj_destroy(P);

    P = proj_create(0, arg);
    pj_info = proj_pj_info(P);
    if ( !pj_info.has_inverse )            {  proj_destroy(P); return 61; }
    if ( strcmp(pj_info.definition, arg) ) {  proj_destroy(P); return 62; }
    if ( strcmp(pj_info.id, "utm") )       {  proj_destroy(P); return 63; }
    proj_destroy(P);

    /* proj_grid_info() */
    grid_info = proj_grid_info("egm96_15.gtx");
    if ( strlen(grid_info.filename) == 0 )            return 64;
    if ( strcmp(grid_info.gridname, "egm96_15.gtx") ) return 65;
    grid_info = proj_grid_info("nonexistinggrid");
    if ( strlen(grid_info.filename) > 0 )             return 66;

    /* proj_init_info() */
    init_info = proj_init_info("unknowninit");
    if ( strlen(init_info.filename) != 0 )  return 67;

    init_info = proj_init_info("epsg");
    if ( strcmp(init_info.origin, "EPSG") )    return 69;
    if ( strcmp(init_info.name, "epsg") )      return 68;



    /* test proj_rtodms() and proj_dmstor() */
    if (strcmp("180dN", proj_rtodms(buf, M_PI, 'N', 'S')))
        return 70;

    if (proj_dmstor(&buf[0], NULL) != M_PI)
        return 71;

    if (strcmp("114d35'29.612\"S", proj_rtodms(buf, -2.0, 'N', 'S')))
        return 72;

    /* we can't expect perfect numerical accuracy so testing with a tolerance */
    if (fabs(-2.0 - proj_dmstor(&buf[0], NULL)) > 1e-7)
        return 73;


    /* test proj_derivatives_retrieve() and proj_factors_retrieve() */
    P = proj_create(0, "+proj=merc");
    a = proj_obs_null;
    a.coo.lp.lam = PJ_TORAD(12);
    a.coo.lp.phi = PJ_TORAD(55);

    derivs = proj_derivatives(P, a.coo.lp);
    if (proj_errno(P))
        return 80; /* derivs not created correctly */

    if ( fabs(derivs.x_l - 1.0)     > 1e-5 )   return 81;
    if ( fabs(derivs.x_p - 0.0)     > 1e-5 )   return 82;
    if ( fabs(derivs.y_l - 0.0)     > 1e-5 )   return 83;
    if ( fabs(derivs.y_p - 1.73959) > 1e-5 )   return 84;


    factors = proj_factors(P, a.coo.lp);
    if (proj_errno(P))
        return 85; /* factors not created correctly */

    /* check a few key characteristics of the Mercator projection */
    if (factors.omega != 0.0)       return 86; /* angular distortion should be 0 */
    if (factors.thetap != M_PI_2)   return 87; /* Meridian/parallel angle should be 90 deg */
    if (factors.conv != 0.0)        return 88; /* meridian convergence should be 0 */


    proj_destroy(P);


    return 0;
}


#endif
