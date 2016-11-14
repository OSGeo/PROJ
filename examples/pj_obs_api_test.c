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
    pj_apply (PJ *P, int direction, PJ_OBS obs):
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


int main (void) {
    PJ *p;
    PJ_OBS a, b;
    char *args[3] = {"proj=utm", "zone=32", "ellps=GRS80"};
    int err;
    double dist;

    /* Log everything libproj offers to log for you */
    pj_debug_set (0, PJ_LOG_DEBUG_MINOR);

    /* An utm projection on the GRS80 ellipsoid */
    p = pj_create ("+proj=utm +zone=32 +ellps=GRS80");
    if (0==p)
        return puts ("Oops"), 0;

    /* Clean up */
    pj_free (p);

    /* Same projection, now using argc/argv style initialization */
    p = pj_create_argv (3, args);
    if (0==p)
        return puts ("Oops"), 0;

    /* zero initialize everything, then set (longitude, latitude) to (12, 55) */
    a = pj_obs_null;
    /* a.coo.lp: The coordinate part of a, interpreted as a classic LP pair */
    a.coo.lp.lam = TORAD(12);
    a.coo.lp.phi = TORAD(55);

    /* Forward projection */
    b = pj_apply (p, PJ_FWD, a);
    printf ("FWD:   %15.9f %15.9f\n", b.coo.enh.e, b.coo.enh.n);

    /* Inverse projection */
    a = pj_apply (p, PJ_INV, b);
    printf ("INV:   %15.9f %15.9f\n", TODEG(a.coo.lpz.lam), TODEG(a.coo.lpz.phi));

    /* Null projection */
    a = pj_apply (p, PJ_IDENT, a);
    printf ("IDENT: %15.9f %15.9f\n", TODEG(a.coo.lpz.lam), TODEG(a.coo.lpz.phi));

    /* Forward again, to get two linear items for comparison */
    a = pj_apply (p, PJ_FWD, a);
    printf ("FWD:   %15.9f %15.9f\n", b.coo.enh.e, b.coo.enh.n);

    dist = pj_xy_dist (a.coo.xy, b.coo.xy);
    printf ("Roundtrip deviation, (nm): %15.9f\n", dist*1e9);

    /* should be identical - checking whether null-init is done */
    dist = pj_xyz_dist (a.coo.xyz, b.coo.xyz);
    printf ("Roundtrip deviation, (nm): %15.9f\n", dist*1e9);

    /* Invalid projection */
    a = pj_apply (p, 42, a);
    if (a.coo.lpz.lam!=DBL_MAX)
        printf ("%15.9f %15.9f\n", a.coo.lpz.lam, a.coo.lpz.phi);
    err = pj_error (p);
    printf ("pj_error: %d\n", err);

    /* Clean up */
    pj_free (p);

    /* Now do some 3D transformations */
    p = pj_create ("+proj=cart +ellps=GRS80");
    if (0==p)
        return puts ("Oops"), 0;

    /* zero initialize everything, then set (longitude, latitude, height) to (12, 55, 100) */
    a = b = pj_obs_null;
    a.coo.lpz.lam = TORAD(12);
    a.coo.lpz.phi = TORAD(55);
    a.coo.lpz.z   = 100;

    /* Forward projection: 3D-Cartesian-to-Ellipsoidal */
    b = pj_apply (p, PJ_FWD, a);
    printf ("FWD:   %15.9f %15.9f %15.9f\n", b.coo.xyz.x, b.coo.xyz.y, b.coo.xyz.z);

    /* Check roundtrip precision for 10000 iterations each way */
    dist = pj_roundtrip (p, PJ_FWD, 10000, a);
    printf ("Roundtrip deviation, fwd (nm): %15.9f\n", dist*1e9*1e5);
    dist = pj_roundtrip (p, PJ_INV, 10000, b);
    printf ("Roundtrip deviation, inv (nm): %15.9f\n", dist*1e9);

    /* Inverse projection: Ellipsoidal-to-3D-Cartesian */
    b = pj_apply (p, PJ_INV, b);
    printf ("INV:   %15.9f %15.9f %15.9f\n", TODEG(b.coo.lpz.lam), TODEG(b.coo.lpz.phi), b.coo.lpz.z);

    /* Move p to another context */
    pj_context_renew (p);
    b = pj_apply (p, PJ_FWD, b);
    printf ("CTX1:  %15.9f %15.9f %15.9f\n", b.coo.xyz.x, b.coo.xyz.y, b.coo.xyz.z);

    /* Move it back to the default context */
    pj_context_free (p);
    b = pj_apply (p, PJ_INV, b);
    printf ("CTX0:  %15.9f %15.9f %15.9f\n", TODEG(b.coo.lpz.lam), TODEG(b.coo.lpz.phi), b.coo.lpz.z);

    pj_free (p);
    return 0;
}
