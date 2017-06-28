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
    PJ_CONTEXT *C, *D;
    PJ *P;
    PJ_OBS a, b;
    char *args[3] = {"proj=utm", "zone=32", "ellps=GRS80"};
    int err;
    double dist;
    XY cph_utm32;

    /* Get ready for PROJ.4-ing */
    C = proj_context_create(0);

    /* Log everything libproj offers to log for you */
    proj_log_level (0, PJ_LOG_TRACE);

    /* An utm projection on the GRS80 ellipsoid */
    P = proj_create (C, "+proj=utm +zone=32 +ellps=GRS80");
    if (0==P)
        return puts ("Oops"), 0;

    /* Clean up */
    proj_destroy (P);

    /* Same projection, now using argc/argv style initialization */
    P = proj_create_argv (3, args);
    if (0==P)
        return puts ("Oops"), 0;

    /* zero initialize everything, then set (longitude, latitude) to (12, 55) */
    a = proj_obs_null ();
    /* a.coo.lp: The coordinate part of a, interpreted as a classic LP pair */
    a.coo.lp.lam = PJ_TORAD(12);
    a.coo.lp.phi = PJ_TORAD(55);

    /* Forward projection */
    b = proj_trans (P, PJ_FWD, a);
    printf ("FWD:   %15.9f %15.9f\n", b.coo.enh.e, b.coo.enh.n);
    cph_utm32 = b.coo.xy;

    /* Inverse projection */
    a = proj_trans (P, PJ_INV, b);
    printf ("INV:   %15.9f %15.9f\n", TODEG(a.coo.lpz.lam), TODEG(a.coo.lpz.phi));

    /* Null projection */
    a = proj_trans (P, PJ_IDENT, a);
    printf ("IDENT: %15.9f %15.9f\n", TODEG(a.coo.lpz.lam), TODEG(a.coo.lpz.phi));

    /* Forward again, to get two linear items for comparison */
    a = proj_trans (P, PJ_FWD, a);
    printf ("FWD:   %15.9f %15.9f\n", b.coo.enh.e, b.coo.enh.n);

    dist = proj_xy_dist (a.coo.xy, b.coo.xy);
    printf ("Roundtrip deviation, (nm): %15.9f\n", dist*1e9);

    /* should be identical - checking whether null-init is done */
    dist = proj_xyz_dist (a.coo.xyz, b.coo.xyz);
    printf ("Roundtrip deviation, (nm): %15.9f\n", dist*1e9);

    /* Invalid projection */
    a = proj_trans (P, 42, a);
    if (a.coo.lpz.lam!=DBL_MAX)
        printf ("%15.9f %15.9f\n", a.coo.lpz.lam, a.coo.lpz.phi);
    err = pj_err_level (P, PJ_ERR_TELL);
    printf ("pj_err_level: %d\n", err);

    /* Clean up */
    proj_destroy (P);

    /* Now do some 3D transformations */
    P = proj_create (C, "+proj=cart +ellps=GRS80");
    if (0==P)
        return puts ("Oops"), 0;

    /* zero initialize everything, then set (longitude, latitude, height) to (12, 55, 100) */
    a = b = proj_obs_null ();
    a.coo.lpz.lam = PJ_TORAD(12);
    a.coo.lpz.phi = PJ_TORAD(55);
    a.coo.lpz.z   = 100;

    /* Forward projection: 3D-Cartesian-to-Ellipsoidal */
    b = proj_trans (P, PJ_FWD, a);
    printf ("FWD:   %15.9f %15.9f %15.9f\n", b.coo.xyz.x, b.coo.xyz.y, b.coo.xyz.z);

    /* Check roundtrip precision for 10000 iterations each way */
    dist = proj_roundtrip (P, PJ_FWD, 10000, a);
    printf ("Roundtrip deviation, fwd (nm): %15.9f\n", dist*1e9*1e5);
    dist = proj_roundtrip (P, PJ_INV, 10000, b);
    printf ("Roundtrip deviation, inv (nm): %15.9f\n", dist*1e9);

    /* Inverse projection: Ellipsoidal-to-3D-Cartesian */
    b = proj_trans (P, PJ_INV, b);
    printf ("INV:   %15.9f %15.9f %15.9f\n", PJ_TODEG(b.coo.lpz.lam), PJ_TODEG(b.coo.lpz.phi), b.coo.lpz.z);

    /* Move p to another context */
    D = proj_context_create ("");
    if (0==D)
        return puts ("Oops"), 0;
    proj_context_set (P, D);

    /* Go on in the new context*/
    b = proj_trans (P, PJ_FWD, b);
    printf ("CTX1:  %15.9f %15.9f %15.9f\n", b.coo.xyz.x, b.coo.xyz.y, b.coo.xyz.z);

    
    /* move it to the default context */
    proj_context_set (P, 0);
    b = pj_trans (P, PJ_INV, b);
    printf ("CTX0:  %15.9f %15.9f %15.9f\n", PJ_TODEG(b.coo.lpz.lam), PJ_TODEG(b.coo.lpz.phi), b.coo.lpz.z);

    proj_destroy (P);
    proj_context_destroy (D);
    proj_context_destroy (C);

    /***************************************************************************

                  P I P E L I N E   T E S T S

    ***************************************************************************/
    C = proj_context_create (0);

    /* forward-reverse geo->utm->geo */
    P = proj_create (
            C,
            "+proj=pipeline +ellps=GRS80 +zone=32 +step "
            "+proj=utm +step "
            "+proj=utm +inv"
        );
    if (0==P)
        return puts ("Oops"), 0;

    /* zero initialize everything, then set (longitude, latitude, height) to (12, 55, 100) */
    a = b = pj_obs_null ();
    a.coo.lpz.lam = PJ_TORAD(12);
    a.coo.lpz.phi = PJ_TORAD(55);
    a.coo.lpz.z   = 100;
    printf ("PRE:   %15.9f %15.9f\n", a.coo.lpz.lam, a.coo.lpz.phi);

    /* Forward projection */
    b = proj_trans (P, PJ_FWD, a);
    printf ("FWD:   %15.9f %15.9f\n", PJ_TODEG(b.coo.lpz.lam), PJ_TODEG(b.coo.lpz.phi));

    /* Inverse projection */
    a = proj_trans (P, PJ_INV, b);
    printf ("INV:   %15.9f %15.9f\n", PJ_TODEG(a.coo.lpz.lam), PJ_TODEG(a.coo.lpz.phi));

    proj_destroy (P);

    /* And now the back-to-back situation utm->geo->utm */
    P = pj_create (
        C,
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
    b = proj_trans (P, PJ_FWD, a);
    printf ("FWD:   %15.9f %15.9f\n", b.coo.xy.x, b.coo.xy.y);

    /* Inverse projection */
    a = proj_trans (P, PJ_INV, b);
    printf ("INV:   %15.9f %15.9f\n", a.coo.xy.x, a.coo.xy.y);

    proj_destroy (P);


    /* Finally testing a corner case: A rather pointless one-step pipeline geo->utm */
    P = proj_create (C, "+proj=pipeline +ellps=GRS80 +zone=32 +step +proj=utm");
    if (0==P)
        return puts ("Oops"), 0;

    /* zero initialize everything, then set (easting, northing) to utm(12, 55) */
    a = b = proj_obs_null ();
    a.coo.lpz.lam = PJ_TORAD(12);
    a.coo.lpz.phi = PJ_TORAD(55);
    printf ("PRE:   %15.9f %15.9f\n", PJ_TODEG(a.coo.lp.lam), PJ_TODEG(a.coo.lp.phi));
    printf ("EXP:   %15.9f %15.9f\n", cph_utm32.x, cph_utm32.y);

    /* Forward projection */
    b = proj_trans (P, PJ_FWD, a);
    printf ("FWD:   %15.9f %15.9f\n", b.coo.xy.x, b.coo.xy.y);

    /* Inverse projection */
    a = proj_trans (P, PJ_INV, b);
    printf ("INV:   %15.9f %15.9f\n", PJ_TODEG(a.coo.lp.lam), PJ_TODEG(a.coo.lp.phi));

    /* Geodesic distance between two points with angular 2D coordinates */
    a.coo.lp.lam = PJ_TORAD(12);
    a.coo.lp.phi = PJ_TORAD(60);
    b.coo.lp.lam = PJ_TORAD(12);
    b.coo.lp.phi = PJ_TORAD(61);
    dist = proj_lp_dist (P, a.coo.lp, b.coo.lp);
    printf ("1 deg at 60N:   %15.9f\n", dist);

    a.coo.lp.lam = PJ_TORAD(12);
    a.coo.lp.phi = PJ_TORAD(0.);
    b.coo.lp.lam = PJ_TORAD(12);
    b.coo.lp.phi = PJ_TORAD(1.);
    dist = proj_lp_dist (P, a.coo.lp, b.coo.lp);
    printf ("1 deg at Equator:   %15.9f\n", dist);

    proj_destroy (P);
    pj_pipeline_selftest ();
    proj_context_destroy (C);
    return 0;
}
