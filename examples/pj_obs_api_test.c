/*******************************************************************************
    Tiny test of an evolving new API, demonstrating simple examples of
    2D and 3D transformations.

    The main transformation setup object is PJ, well known from the two
    former proj APIs (projects.h and proj_api.h)

    The main data object PJ_OBS is new, but encapsulates the older
    LP, XY etc. objects in a framework for storing a 3D observation taken at
    a 4D point in space-time, and including some additional metadata (e.g.
    a serial number or an epsg code).

    PJ_OBS uses unions to enforce explicit statement of what kind of
    coordinates are expected at a given spot in the code.

    The primary elements of the API are:

    pj_create (char *desc):
        Create a new PJ object from a text description.
    pj_trans (PJ *P, int direction, PJ_OBS obs):
        Forward or inverse transformation of obs.
    pj_error (PJ *P):
        Check error status of P.
    pj_free (PJ *P):
        Clean up when done with the transformation object.

    These 4 functions should cover most of the common use cases. Additional
    functionality for handling thread contexts are provided by the functions
    pj_debug_set,  pj_error_set,  pj_log_set,  pj_context_renew,
    pj_context_inherit,    pj_context_free,    pj_fileapi_set.

    The proj thread contexts have not seen widespread use, so one of the
    intentions with this new API is to make them less visible on the API
    surface: Contexts do not have a life by themselves, they are only
    visible through their associated PJs, and the number of functions
    supporting them is limited.

    Compilation example:
    gcc -pedantic -W -Wall -Wextra -I../src -o pj_proj_test pj_proj_test.c -L../../../build/proj.4/lib -lproj_4_9

    Thomas Knudsen, 2016-10-30/11-03
*******************************************************************************/
#include <proj.h>

int pj_pipeline_selftest (void);

int main (void) {
    PJ *P;
    PJ_OBS a, b;
    char *args[3] = {"proj=utm", "zone=32", "ellps=GRS80"};
    int err;
    double dist;
    XY cph_utm32;

    /* Log everything libproj offers to log for you */
    pj_log_level (0, PJ_LOG_TRACE);

    /* An utm projection on the GRS80 ellipsoid */
    P = pj_create ("+proj=utm +zone=32 +ellps=GRS80");
    if (0==P)
        return puts ("Oops"), 0;

    /* Clean up */
    pj_free (P);

    /* Same projection, now using argc/argv style initialization */
    P = pj_create_argv (3, args);
    if (0==P)
        return puts ("Oops"), 0;

    /* zero initialize everything, then set (longitude, latitude) to (12, 55) */
    a = pj_obs_null ();
    /* a.coo.lp: The coordinate part of a, interpreted as a classic LP pair */
    a.coo.lp.lam = TORAD(12);
    a.coo.lp.phi = TORAD(55);

    /* Forward projection */
    b = pj_trans (P, PJ_FWD, a);
    printf ("FWD:   %15.9f %15.9f\n", b.coo.enh.e, b.coo.enh.n);
    cph_utm32 = b.coo.xy;

    /* Inverse projection */
    a = pj_trans (P, PJ_INV, b);
    printf ("INV:   %15.9f %15.9f\n", TODEG(a.coo.lpz.lam), TODEG(a.coo.lpz.phi));

    /* Null projection */
    a = pj_trans (P, PJ_IDENT, a);
    printf ("IDENT: %15.9f %15.9f\n", TODEG(a.coo.lpz.lam), TODEG(a.coo.lpz.phi));

    /* Forward again, to get two linear items for comparison */
    a = pj_trans (P, PJ_FWD, a);
    printf ("FWD:   %15.9f %15.9f\n", b.coo.enh.e, b.coo.enh.n);

    dist = pj_xy_dist (a.coo.xy, b.coo.xy);
    printf ("Roundtrip deviation, (nm): %15.9f\n", dist*1e9);

    /* should be identical - checking whether null-init is done */
    dist = pj_xyz_dist (a.coo.xyz, b.coo.xyz);
    printf ("Roundtrip deviation, (nm): %15.9f\n", dist*1e9);

    /* Invalid projection */
    a = pj_trans (P, 42, a);
    if (a.coo.lpz.lam!=DBL_MAX)
        printf ("%15.9f %15.9f\n", a.coo.lpz.lam, a.coo.lpz.phi);
    err = pj_err_level (P, PJ_ERR_TELL);
    printf ("pj_err_level: %d\n", err);

    /* Clean up */
    pj_free (P);

    /* Now do some 3D transformations */
    P = pj_create ("+proj=cart +ellps=GRS80");
    if (0==P)
        return puts ("Oops"), 0;

    /* zero initialize everything, then set (longitude, latitude, height) to (12, 55, 100) */
    a = b = pj_obs_null ();
    a.coo.lpz.lam = TORAD(12);
    a.coo.lpz.phi = TORAD(55);
    a.coo.lpz.z   = 100;

    /* Forward projection: 3D-Cartesian-to-Ellipsoidal */
    b = pj_trans (P, PJ_FWD, a);
    printf ("FWD:   %15.9f %15.9f %15.9f\n", b.coo.xyz.x, b.coo.xyz.y, b.coo.xyz.z);

    /* Check roundtrip precision for 10000 iterations each way */
    dist = pj_roundtrip (P, PJ_FWD, 10000, a);
    printf ("Roundtrip deviation, fwd (nm): %15.9f\n", dist*1e9*1e5);
    dist = pj_roundtrip (P, PJ_INV, 10000, b);
    printf ("Roundtrip deviation, inv (nm): %15.9f\n", dist*1e9);

    /* Inverse projection: Ellipsoidal-to-3D-Cartesian */
    b = pj_trans (P, PJ_INV, b);
    printf ("INV:   %15.9f %15.9f %15.9f\n", TODEG(b.coo.lpz.lam), TODEG(b.coo.lpz.phi), b.coo.lpz.z);

    /* Move p to another context */
    pj_context_renew (P);
    b = pj_trans (P, PJ_FWD, b);
    printf ("CTX1:  %15.9f %15.9f %15.9f\n", b.coo.xyz.x, b.coo.xyz.y, b.coo.xyz.z);

    /* Move it back to the default context */
    pj_context_free (P);
    b = pj_trans (P, PJ_INV, b);
    printf ("CTX0:  %15.9f %15.9f %15.9f\n", TODEG(b.coo.lpz.lam), TODEG(b.coo.lpz.phi), b.coo.lpz.z);

    pj_free (P);

    /***************************************************************************

                  P I P E L I N E   T E S T S

    ***************************************************************************/

    /* forward-reverse geo->utm->geo */
    P = pj_create (
            "+proj=pipeline +ellps=GRS80 +zone=32 +step "
            "+proj=utm +step "
            "+proj=utm +inv"
        );
    if (0==P)
        return puts ("Oops"), 0;
    /* zero initialize everything, then set (longitude, latitude, height) to (12, 55, 100) */
    a = b = pj_obs_null ();
    a.coo.lpz.lam = TORAD(12);
    a.coo.lpz.phi = TORAD(55);
    a.coo.lpz.z   = 100;
    printf ("PRE:   %15.9f %15.9f\n", a.coo.lpz.lam, a.coo.lpz.phi);

    /* Forward projection */
    b = pj_trans (P, PJ_FWD, a);
    printf ("FWD:   %15.9f %15.9f\n", TODEG(b.coo.lpz.lam), TODEG(b.coo.lpz.phi));

    /* Inverse projection */
    a = pj_trans (P, PJ_INV, b);
    printf ("INV:   %15.9f %15.9f\n", TODEG(a.coo.lpz.lam), TODEG(a.coo.lpz.phi));

    pj_free (P);


    /* And now the back-to-back situation utm->geo->utm */
    P = pj_create (
        "+proj=pipeline +ellps=GRS80 +zone=32 +step "
        "+proj=utm +inv +step "
        "+proj=utm");
    if (0==P)
        return puts ("Oops"), 0;

    /* zero initialize everything, then set (easting, northing) to utm(12, 55) */
    a = b = pj_obs_null ();
    a.coo.xy = cph_utm32;
    printf ("PRE:   %15.9f %15.9f\n", a.coo.xy.x, a.coo.xy.y);

    /* Forward projection */
    b = pj_trans (P, PJ_FWD, a);
    printf ("FWD:   %15.9f %15.9f\n", b.coo.xy.x, b.coo.xy.y);

    /* Inverse projection */
    a = pj_trans (P, PJ_INV, b);
    printf ("INV:   %15.9f %15.9f\n", a.coo.xy.x, a.coo.xy.y);

    pj_free (P);



    /* Finally testing a corner case: A rather pointless one-step pipeline geo->utm */
    P = pj_create ("+proj=pipeline +ellps=GRS80 +zone=32 +step +proj=utm");
    if (0==P)
        return puts ("Oops"), 0;

    /* zero initialize everything, then set (easting, northing) to utm(12, 55) */
    a = b = pj_obs_null ();
    a.coo.lpz.lam = TORAD(12);
    a.coo.lpz.phi = TORAD(55);
    printf ("PRE:   %15.9f %15.9f\n", TODEG(a.coo.lp.lam), TODEG(a.coo.lp.phi));
    printf ("EXP:   %15.9f %15.9f\n", cph_utm32.x, cph_utm32.y);

    /* Forward projection */
    b = pj_trans (P, PJ_FWD, a);
    printf ("FWD:   %15.9f %15.9f\n", b.coo.xy.x, b.coo.xy.y);

    /* Inverse projection */
    a = pj_trans (P, PJ_INV, b);
    printf ("INV:   %15.9f %15.9f\n", TODEG(a.coo.lp.lam), TODEG(a.coo.lp.phi));

    /* Geodesic distance between two points with angular 2D coordinates */
    a.coo.lp.lam = TORAD(12);
    a.coo.lp.phi = TORAD(60);
    b.coo.lp.lam = TORAD(12);
    b.coo.lp.phi = TORAD(61);
    dist = pj_lp_dist (P, a.coo.lp, b.coo.lp);
    printf ("1 deg at 60N:   %15.9f\n", dist);

    a.coo.lp.lam = TORAD(12);
    a.coo.lp.phi = TORAD(0.);
    b.coo.lp.lam = TORAD(12);
    b.coo.lp.phi = TORAD(1.);
    dist = pj_lp_dist (P, a.coo.lp, b.coo.lp);
    printf ("1 deg at Equator:   %15.9f\n", dist);

    pj_free (P);
    pj_pipeline_selftest ();
    return 0;
}
