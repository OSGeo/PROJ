#include <proj.h>

int main (void) {
    PJ_CONTEXT *C;
    PJ *P;
    PJ_COORD a, b;
    int err;
    errno = 0;
    
    C = proj_context_create(0);
    P = proj_create (C, "+proj=utm +zone=32 +ellps=GRS80");
    if (0==P)
        return puts ("Oops"), 0;

    /* a.coo: The coordinate part of an observation */
    a = proj_coord (PJ_TORAD(12), PJ_TORAD(55), 0, 0);
    b = proj_trans_coord (P, PJ_FWD, a);
    printf ("easting: %g, northing: %g\n", b.en.e, b.en.n);
    b = proj_trans_coord (P, PJ_INV, b);
    printf ("longitude: %g, latitude: %g\n", b.lp.lam, b.lp.phi);

    /* Clean up */
    proj_destroy (P);
    proj_context_destroy (C);
    return 0;
}
