/*******************************************************************************
    Tiny test of an evolving new API, demonstrating examples of init-file
    handling.

    NOTE!!!! set PROJ_LIB=. before running this

    Compilation example:
    gcc -pedantic -W -Wall -Wextra -I../src -o pj_init_test pj_init_test.c -L../../../build/proj.4/lib -lproj_4_9

    Thomas Knudsen, 2016-11-21
*******************************************************************************/
#include <proj.h>
#include <projects.h>
#include "../src/horner.h"

int pj_helmert_selftest (void);
int pj_cart_selftest (void);


void show_horner (HORNER *Q) {
    int i, n;
    n = horner_number_of_coefficients (Q->order);
    for (i = 0; i < n; i++)
        printf (
            "%2.2d (%2.2d %2.2d) %20.15g %20.15g %20.15g %20.15g\n",
             i, horner_degree_u(Q->order, i), horner_degree_v(Q->order, i),
             Q->fwd_u[i], Q->fwd_v[i], Q->inv_u[i], Q->inv_v[i]
        );
    /* ######################### */
    printf ("fwd_origin: %20.15g  %20.15g\n", Q->fwd_origin->u, Q->fwd_origin->v);
    printf ("inv_origin: %20.15g  %20.15g\n", Q->inv_origin->u, Q->inv_origin->v);
    printf ("range: %20.15g\n", Q->range);

    for (i = 0; i < horner_number_of_coefficients(2); i++)
        printf ("%2.2d %2.2d %2.2d\n",
            i, horner_degree_u(2, i), horner_degree_v(2, i)
        );
    /* ######################### */
}


#define banner(s) puts(""); puts ("##########################"); puts ("#  " s); puts ("##########################");

int die_func (const char *file, int line, const char *msg, int retcode) {
    printf ("%s(%d): %s - ret=%d\n", file, line, msg, retcode);
    return retcode;
}
#define die(msg, retcode) die_func (__FILE__, __LINE__, msg, retcode)

int main (int argc, char **argv) {
    PJ *P;
    paralist *params;
    PJ_OBS a, b, c, e;
    double dist;


    /* (55,12,100) -> (x,y,z), ed50, fra KMStrans2 */
    const PJ_OBS cph_ed50 = {
        {{3586701.0962,  762376.8528,  5201571.5773, 0}},
        {{0, 0, 0}},  0, 0
    };

    /* (55,12,100) -> (x,y,z)ETRS89, fra KMStrans2 */
    const PJ_OBS cph_etrs89 = {
        {{3586525.7611,  762339.5841,  5201465.4383, 0}},
        {{0, 0, 0}},  0, 0
    };

    /* (55,12,100) -> (x,y,z)ed50 -> (x,y,z)ETRS89, fra KMStrans2 */
    const PJ_OBS cph_etrs89_from_ed50 = {
        {{3586617.3023,  762282.2356, 5201454.2643, 0}},
        {{0, 0, 0}},  0, 0
    };

    /* Log everything libproj offers to log for you */
    pj_log_level (0, PJ_LOG_NONE);

    /* shut up some irrelevant compiler warnings */
    horner_silence (0);

    if ((argc > 1) && (0==strcmp("no-op", argv[1])))
        exit (0);
    P = pj_create ("+proj=cart +ellps=GRS80");

    a = b = c = e = pj_obs_null;
    a.coo.lpz.lam =  TORAD(12);
    a.coo.lpz.phi =  TORAD(55);
    a.coo.lpz.z   =  100;

    e  =  cph_etrs89;

    banner ("CART");
    b = pj_trans (P, PJ_FWD, a);
    printf ("INI:   %20.15g %20.15g %20.15g\n", TODEG(a.coo.lpz.lam), TODEG(a.coo.lpz.phi), a.coo.lpz.z);
    printf ("FWD:   %20.15g %20.15g %20.15g\n", b.coo.xyz.x, b.coo.xyz.y, b.coo.xyz.z);
    printf ("EXP:   %20.15g %20.15g %20.15g\n", e.coo.xyz.x, e.coo.xyz.y, e.coo.xyz.z);
    printf ("dist [mm] = %f\n", 1000 * pj_xyz_dist (e.coo.xyz,b.coo.xyz));
    b = pj_trans (P, PJ_INV, b);
    printf ("INV:   %20.15g %20.15g %20.15g\n", TODEG(b.coo.lpz.lam), TODEG(b.coo.lpz.phi), b.coo.lpz.z);
    printf ("dist [mm] = %f\n", 1000 * pj_xy_dist (a.coo.xy,b.coo.xy));

    /* Now transform to etrs89 */
    b = cph_ed50;

    /* (55,12,100) -> (X,Y,Z) -> ed50, fra KMStrans2 */
    e = cph_etrs89_from_ed50;

    banner ("HELMERT");
    P = pj_create ("+proj=helmert +approx"
            "  +x=-81.0703 +y=-89.3603 +z=-115.7526"
            " +rx=-484.88 +ry=-24.36  +rz=-413.21   +s=-0.540645"
        );

    if (0==P)
        exit ((puts ("Oops"), 1));

    printf ("dist, abs [mm] = %f\n", 1000 * pj_xyz_dist (e.coo.xyz,b.coo.xyz));

    /* b = pj_trans (P, PJ_FWD, b); */
    b.coo.xyz = P->fwd3d(b.coo.lpz, P);

    printf ("FWD:   %20.15g %20.15g %20.15g\n", b.coo.xyz.x, b.coo.xyz.y, b.coo.xyz.z);
    printf ("EXP:   %20.15g %20.15g %20.15g\n", e.coo.xyz.x, e.coo.xyz.y, e.coo.xyz.z);
    printf ("dist [mm] = %f\n", 1000 * pj_xyz_dist (e.coo.xyz,b.coo.xyz));
    pj_free (P);

    banner ("UTM");
    a.coo.lpz.lam =  TORAD(12);
    a.coo.lpz.phi =  TORAD(55);
    a.coo.lpz.z   =  100;
    P = pj_create ("+proj=utm +zone=32 +ellps=GRS80");
    if (0==P)
        die ("bad UTM", 0);
    dist = pj_roundtrip (P, PJ_FWD, 10000, a);
    printf ("Roundtrip deviation, fwd (nm): %15.9f\n", dist*1e9);
    pj_free (P);


    /* The "System 45 Bornholm" */
    banner ("pipeline");
    pj_log_level (0, PJ_LOG_NONE);
    P = pj_create ("+init=./s45b.pol:s45b");
    if (0==P)
        return die ("Oops", 0);

    printf ("error level %d\n", pj_err_level (P, PJ_ERR_TELL));
    a = b = pj_obs_null;
    a.coo.uv.u =   50022.563745; /* x: westing  */
    a.coo.uv.v =   54779.260103; /* y: northing */

    printf ("PRE:   %20.15g %20.15g\n", a.coo.enh.e, a.coo.enh.n);

    /* Forward projection */
    b = pj_trans (P, PJ_FWD, a);
    printf ("FWD:   %20.15g %20.15g\n", b.coo.enh.e, b.coo.enh.n);
    b = pj_trans (P, PJ_INV, b);
    printf ("INV:   %20.15g %20.15g\n", b.coo.enh.e, b.coo.enh.n);
    printf ("dist [mm] = %f\n", 1000 * pj_xy_dist (a.coo.xy,b.coo.xy));
#if 0
    /* Check roundtrip precision for 1 iteration each way */
    dist = pj_roundtrip (P, PJ_FWD, 1, a);
    printf ("Roundtrip deviation, fwd (mm): %15.9f\n", dist*1000);
#endif
    pj_free (P);


exit(0);



    /* The TC32 for "System 45 Bornholm" */
    banner ("TC32 -> UTM32");
    P = pj_create ("+init=./s45b.pol:tc32_utm32");
    if (0==P)
        return die ("Oops", 0);
    params = P->params;
    for (; params != 0; params = params->next)
        printf("%-40.40s\n", params->param);

    printf ("error level %d\n", pj_err_level (P, PJ_ERR_TELL));
    a = b = pj_obs_null;
    a.coo.uv.v = 6125305.4245;
    a.coo.uv.u =  878354.8539;

    printf ("PRE:   %20.15g %20.15g\n", a.coo.enh.e, a.coo.enh.n);

    /* Forward projection */
    b = pj_trans (P, PJ_FWD, a);
    printf ("FWD:   %20.15g %20.15g\n", b.coo.enh.e, b.coo.enh.n);
    b = pj_trans (P, PJ_INV, b);
    printf ("INV:   %20.15g %20.15g\n", b.coo.enh.e, b.coo.enh.n);

    /* Check roundtrip precision for 1 iteration each way */
    dist = pj_roundtrip (P, PJ_FWD, 1, a);
    printf ("Roundtrip deviation, fwd (mm): %15.9f\n", dist*1000);

    if (argc > 1)
        show_horner ((HORNER *) P->opaque);
    pj_free (P);


    banner ("S45B -> TC32");

    /* The "System 45 Bornholm" */
    P = pj_create ("+init=./s45b.pol:s45b_tc32");
    if (0==P)
        return die ("Oops", 0);
    params = P->params;
    for (; params != 0; params = params->next)
        printf("%-40.40s\n", params->param);

    printf ("error level %d\n", pj_err_level (P, PJ_ERR_TELL));
    a = b = pj_obs_null;
    a.coo.uv.u =   47022.563745;
    a.coo.uv.v =   51779.260103;

    printf ("PRE:   %20.15g %20.15g\n", a.coo.enh.e, a.coo.enh.n);

    /* Forward projection */
    b = pj_trans (P, PJ_FWD, a);
    printf ("FWD:   %20.15g %20.15g\n", b.coo.enh.e, b.coo.enh.n);
    b = pj_trans (P, PJ_INV, b);
    printf ("INV:   %20.15g %20.15g\n", b.coo.enh.e, b.coo.enh.n);

    /* Check roundtrip precision for 1 iteration each way */
    dist = pj_roundtrip (P, PJ_FWD, 1, a);
    printf ("Roundtrip deviation, fwd (mm): %15.9f\n", dist*1000);

    if (argc > 1)
        show_horner ((HORNER *) P->opaque);
    pj_free (P);


    printf ("pj_helmert_selftest returns  %d\n", pj_helmert_selftest ());
    printf ("pj_cart_selftest    returns  %d\n", pj_cart_selftest ());

    return 0;
}
