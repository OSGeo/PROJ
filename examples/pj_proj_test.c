/* Tiny test of an evolving new API. Thomas Knudsen, 2016-10-30 */
/* gcc -pedantic -W -Wall -Wextra -I../src -o pj_proj_test pj_proj_test.c -L../../../build/proj.4/lib -lproj_4_9 */
#include <proj.h>

int main (void) {
    PJ *p;
    PJ_OBSERVATION a, b;
    int err;
    double dist;

    p = pj_create ("+ellps=GRS80 +proj=utm +zone=32");
    if (0==p)
        return puts ("Oops"), 0;

    /* zero initialize everything, then set (longitude, latitude) to (12, 55) */
    a = b = pj_observation_null;
    /* a.coo.lp: The coordinate part of a, interpreted as a classic LP pair */
    a.coo.lp.lam = TORAD(12);
    a.coo.lp.phi = TORAD(55);

    /* Forward projection */
    b = pj_apply (p, PJ_FWD, a);
    printf ("%15.9f %15.9f\n", b.coo.enh.e, b.coo.enh.n);

    /* Inverse projection */
    a = pj_apply (p, PJ_INV, b);
    printf ("%15.9f %15.9f\n", TODEG(a.coo.lpz.lam), TODEG(a.coo.lpz.phi));

    /* Null projection */
    a = pj_apply (p, PJ_IDENT, a);
    printf ("%15.9f %15.9f\n", TODEG(a.coo.lpz.lam), TODEG(a.coo.lpz.phi));

    /* Invalid projection */
    a = pj_apply (p, 42, a);
    printf ("%15.9f %15.9f\n", a.coo.lpz.lam, a.coo.lpz.phi);
    err = pj_error (p);
    printf ("pj_ctx_errno: %d\n", err);

    pj_free (p);



    p = pj_create ("+ellps=GRS80 +proj=cart");
    if (0==p)
        return puts ("Oops"), 0;

    /* zero initialize everything, then set (longitude, latitude, height) to (12, 55, 100) */
    a = b = pj_observation_null;
    a.coo.lpz.lam = TORAD(12);
    a.coo.lpz.phi = TORAD(55);
    a.coo.lpz.z   = 100;

    /* Forward projection */
    b = pj_apply (p, PJ_FWD, a);
    printf ("%15.9f %15.9f %15.9f\n", b.coo.xyz.x, b.coo.xyz.y, b.coo.xyz.z);

    /* Roundtrip */
    dist = pj_roundtrip (p, PJ_INV, 1, a);
    printf ("Roundtrip deviation (nm): %15.9f\n", 1e9*dist);


    /* Inverse projection */
    b = pj_apply (p, PJ_INV, b);
    printf ("%15.9f %15.9f %15.9f\n", TODEG(b.coo.lpz.lam), TODEG(b.coo.lpz.phi), b.coo.lpz.z);

    pj_free (p);
    return 0;
}
