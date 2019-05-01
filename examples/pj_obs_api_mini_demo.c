/*******************************************************************************
    Tiny test of an evolving new API, demonstrating simple examples of
    2D and 3D transformations.

    The main transformation setup object is PJ, well known from the two
    former proj APIs (projects.h and proj_api.h)

    The main data objects PJ_COORD and PJ_OBS are new, but encapsulate
    the older LP, XY etc. objects in a framework for storing a 3D
    observation taken at a 4D point in space-time, and including some
    additional metadata (e.g. a serial number or an epsg code).

    PJ_OBS and PJ_COORD use unions to enforce explicit statement of what
    kind of coordinates are expected at a given spot in the code, where
    the old API uses type punning, implicitly assuming that "you know what
    you do". For backward compatibility, the new API is not really type
    safe in the sense that you cannot use a cartesian coordinate where a
    geographic is expected - but it makes it possible to explicitly state
    in the code whet the programmer expected and intended.

    The proj thread contexts have not seen widespread use, so one of the
    intentions with this new API is to make them less visible on the API
    surface.

    A series of experiments have, however, shown that they, for (mostly)
    historical reasons, are very hard to eliminate totally. But we have
    reduced their API surface presence to a constructor and a destructor,
    plus an extra argument to the PJ constructor, pj_create().

    For single threaded programs, the calls to the context constructor
    and destructor may be left out, and the default context selected
    by passing a null-pointer to pj_create.

    Note: This file is in-lined in the documentation. Any changes must be
    reflected in docs/source/development/quickstart.rst

    Thomas Knudsen, 2016-10-30/2017-07-06
*******************************************************************************/
#include <stdio.h>
#include <proj.h>

int main (void) {
    PJ_CONTEXT *C;
    PJ *P;
    PJ* P_for_GIS;
    PJ_COORD a, b;

    /* or you may set C=PJ_DEFAULT_CTX if you are sure you will     */
    /* use PJ objects from only one thread                          */
    C = proj_context_create();

    P = proj_create_crs_to_crs (C,
                                "EPSG:4326",
                                "+proj=utm +zone=32 +datum=WGS84", /* or EPSG:32632 */
                                NULL);

    if (0==P) {
        fprintf(stderr, "Oops\n");
        return 1;
    }

    /* This will ensure that the order of coordinates for the input CRS */
    /* will be longitude, latitude, whereas EPSG:4326 mandates latitude, */
    /* longitude */
    P_for_GIS = proj_normalize_for_visualization(C, P);
    if( 0 == P_for_GIS )  {
        fprintf(stderr, "Oops\n");
        return 1;
    }
    proj_destroy(P);
    P = P_for_GIS;

    /* a coordinate union representing Copenhagen: 55d N, 12d E    */
    /* Given that we have used proj_normalize_for_visualization(), the order of
    /* coordinates is longitude, latitude, and values are expressed in degrees. */
    a = proj_coord (12, 55, 0, 0);

    /* transform to UTM zone 32, then back to geographical */
    b = proj_trans (P, PJ_FWD, a);
    printf ("easting: %.3f, northing: %.3f\n", b.enu.e, b.enu.n);
    b = proj_trans (P, PJ_INV, b);
    printf ("longitude: %g, latitude: %g\n", b.lp.lam, b.lp.phi);

    /* Clean up */
    proj_destroy (P);
    proj_context_destroy (C); /* may be omitted in the single threaded case */
    return 0;
}
