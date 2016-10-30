/* Tiny test of an evolving new API. Thomas Knudsen, 2016-10-30 */

#include <proj.h>

int main (void) {
    PJ *p;
    PJ_OBSERVATION a, b;
    int err;

    /* pj_pj plays the role of the pj_init family */
    p = pj_pj (0, "+ellps=GRS80 +proj=utm +zone=32");
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
    err = pj_ctx_get_errno (pj_ctx (p));
    printf ("pj_ctx_errno: %d\n", err);
    return 0;
}
