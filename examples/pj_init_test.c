/*******************************************************************************
    Tiny test of an evolving new API, demonstrating examples of init-file
    handling.

    Compilation example:
    gcc -pedantic -W -Wall -Wextra -I../src -o pj_init_test pj_init_test.c -L../../../build/proj.4/lib -lproj_4_9

    Thomas Knudsen, 2016-11-21
*******************************************************************************/
#include <proj.h>
#include <projects.h>

#define banner(s) puts(""); puts ("##########################"); puts ("#  " s); puts ("##########################");
int die_func (const char *file, int line, const char *msg, int retcode) {
    printf ("%s(%d): %s - ret=%d\n", file, line, msg, retcode);
    return retcode;
}
#define die(msg, retcode) die_func (__FILE__, __LINE__, msg, retcode)

int main (void) {
    PJ *P;
    paralist *params;
    PJ_OBS a, b;
    double dist;

    /* Log everything libproj offers to log for you */
    pj_log_level (0, PJ_LOG_TRACE);

    /* The "System 45 Bornholm" */
    banner ("pipeline");
    P = pj_create ("+init=./s45b.pol:s45b");
    if (0==P)
        return die ("Oops", 0);
    params = P->params;
    for (; params != 0; params = params->next)
        printf("%s\n", params->param);

    printf ("error level %d\n", pj_err_level (P, PJ_ERR_TELL));
    a = b = pj_obs_null;
    a.coo.uv.u =  877605.269066;
    a.coo.uv.v = 6125810.306769;
    a.coo.uv.u =   51779.260103;
    a.coo.uv.v =   47022.563745;

    printf ("PRE:   %20.15g %20.15g\n", a.coo.enh.e, a.coo.enh.n);

    /* Forward projection */
    b = pj_trans (P, PJ_FWD, a);
    printf ("FWD:   %20.15g %20.15g\n", b.coo.enh.e, b.coo.enh.n);
    b = pj_trans (P, PJ_INV, b);
    printf ("INV:   %20.15g %20.15g\n", b.coo.enh.e, b.coo.enh.n);

    /* Check roundtrip precision for 1 iteration each way */
    dist = pj_roundtrip (P, PJ_FWD, 1, a);
    printf ("Roundtrip deviation, fwd (mm): %15.9f\n", dist*1000);


    pj_free (P);







    /* The TC32 for "System 45 Bornholm" */
    banner ("TC32 <-> UTM32");
    P = pj_create ("+init=./s45b.pol:tc32_utm32");
    if (0==P)
        return die ("Oops", 0);
    params = P->params;
    for (; params != 0; params = params->next)
        printf("%-40.40s\n", params->param);

    printf ("error level %d\n", pj_err_level (P, PJ_ERR_TELL));
    a = b = pj_obs_null;
    a.coo.uv.u =   51779.260103;
    a.coo.uv.v =   47022.563745;
    a.coo.uv.u =  877605.269066;
    a.coo.uv.v = 6125810.306769;

    printf ("PRE:   %20.15g %20.15g\n", a.coo.enh.e, a.coo.enh.n);

    /* Forward projection */
    b = pj_trans (P, PJ_FWD, a);
    printf ("FWD:   %20.15g %20.15g\n", b.coo.enh.e, b.coo.enh.n);
    b = pj_trans (P, PJ_INV, b);
    printf ("INV:   %20.15g %20.15g\n", b.coo.enh.e, b.coo.enh.n);

    /* Check roundtrip precision for 1 iteration each way */
    dist = pj_roundtrip (P, PJ_FWD, 1, a);
    printf ("Roundtrip deviation, fwd (mm): %15.9f\n", dist*1000);


    pj_free (P);







    banner ("S45B <-> TC32");

    /* The "System 45 Bornholm" */
    P = pj_create ("+init=./s45b.pol:s45b_tc32");
    if (0==P)
        return die ("Oops", 0);
    params = P->params;
    for (; params != 0; params = params->next)
        printf("%-40.40s\n", params->param);

    printf ("error level %d\n", pj_err_level (P, PJ_ERR_TELL));
    a = b = pj_obs_null;
    a.coo.uv.u =  877605.269066;
    a.coo.uv.v = 6125810.306769;
    a.coo.uv.u =   51779.260103;
    a.coo.uv.v =   47022.563745;

    printf ("PRE:   %20.15g %20.15g\n", a.coo.enh.e, a.coo.enh.n);

    /* Forward projection */
    b = pj_trans (P, PJ_FWD, a);
    printf ("FWD:   %20.15g %20.15g\n", b.coo.enh.e, b.coo.enh.n);
    b = pj_trans (P, PJ_INV, b);
    printf ("INV:   %20.15g %20.15g\n", b.coo.enh.e, b.coo.enh.n);

    /* Check roundtrip precision for 1 iteration each way */
    dist = pj_roundtrip (P, PJ_FWD, 1, a);
    printf ("Roundtrip deviation, fwd (mm): %15.9f\n", dist*1000);


    pj_free (P);






    return 0;
}
