/*******************************************************************************
    Tiny test of an evolving new API, demonstrating simple examples of
    2D and 3D transformations.

    Compilation example:
    gcc -pedantic -W -Wall -Wextra -I../src -o pj_proj_test pj_proj_test.c -L../../../build/proj.4/lib -lproj_4_9

    Thomas Knudsen, 2016-11-21
*******************************************************************************/
#include <proj.h>
#include <projects.h>


int main (void) {
    PJ *P;
    paralist *params;
    PJ_OBS a, b;

    /* Log everything libproj offers to log for you */
    pj_log_level (0, PJ_LOG_TRACE);

    /* An utm projection on the GRS80 ellipsoid */
    P = pj_create ("+init=./s45b.pol:s45b");
    if (0==P)
        return puts ("Oops"), 0;

    /* zero initialize everything, then set (longitude, latitude) to (12, 55) */
    a = pj_obs_null;
    /* a.coo.lp: The coordinate part of a, interpreted as a classic LP pair */
    a.coo.lp.lam = TORAD(12);
    a.coo.lp.phi = TORAD(55);

    /* Forward projection */
    b = pj_trans (P, PJ_FWD, a);
    printf ("FWD:   %15.9f %15.9f\n", b.coo.enh.e, b.coo.enh.n);

    params = P->params;
    for (; params != 0; params = params->next)
        printf("###%s###\n", params->param);

    pj_free (P);
    return 0;
}
