/*******************************************************************************
    Example code that illustrates how to convert between a CRS and
    geodetic coordnates for that CRS.

    Note: This file is in-lined in the documentation. Any changes must be
    reflected in docs/source/development/quickstart.rst

*******************************************************************************/
#include <math.h>
#include <proj.h>
#include <stdio.h>

int main(void) {

    /* Create the context. */
    /* You may set C=PJ_DEFAULT_CTX if you are sure you will     */
    /* use PJ objects from only one thread                       */
    PJ_CONTEXT *C = proj_context_create();

    /* Create a projection. */
    PJ *P = proj_create(C, "+proj=utm +zone=32 +datum=WGS84 +type=crs");

    if (0 == P) {
        fprintf(stderr, "Failed to create transformation object.\n");
        return 1;
    }

    /* Get the geodetic CRS for that projection. */
    PJ *G = proj_crs_get_geodetic_crs(C, P);

    /* Create the transform from geodetic to projected coordinates.*/
    PJ_AREA *A = NULL;
    const char *const *options = NULL;
    PJ *G2P = proj_create_crs_to_crs_from_pj(C, G, P, A, options);

    /* Longitude and latitude of Copenhagen, in degrees. */
    double lon = 12.0, lat = 55.0;

    /* Prepare the input */
    PJ_COORD c_in;
    c_in.lpzt.z = 0.0;
    c_in.lpzt.t = HUGE_VAL; // important only for time-dependent projections
    c_in.lp.lam = lon;
    c_in.lp.phi = lat;
    printf("Input longitude: %g, latitude: %g (degrees)\n", c_in.lp.lam,
           c_in.lp.phi);

    /* Compute easting and northing */
    PJ_COORD c_out = proj_trans(G2P, PJ_FWD, c_in);
    printf("Output easting: %g, northing: %g (meters)\n", c_out.enu.e,
           c_out.enu.n);

    /* Apply the inverse transform */
    PJ_COORD c_inv = proj_trans(G2P, PJ_INV, c_out);
    printf("Inverse applied. Longitude: %g, latitude: %g (degrees)\n",
           c_inv.lp.lam, c_inv.lp.phi);

    /* Clean up */
    proj_destroy(P);
    proj_destroy(G);
    proj_destroy(G2P);
    proj_context_destroy(C); /* may be omitted in the single threaded case */
    return 0;
}
