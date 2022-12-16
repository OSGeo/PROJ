/*******************************************************************************
    Simple example code demonstrating use of the proj.h API for 2D coordinate
    transformations.

    It illustrates the proj_create() function. It shows how, given a
    projection, one can convert from geodetic to projected coordinates
    and vice-versa.

    Note: This file is in-lined in the documentation. Any changes must be
    reflected in docs/source/development/quickstart.rst

*******************************************************************************/
#include <proj.h>
#include <stdio.h>
#include <math.h>

int main (void) {
  PJ_CONTEXT *C;
  PJ *P;

  /* Create the context. */
  /* You may set C=PJ_DEFAULT_CTX if you are sure you will     */
  /* use PJ objects from only one thread                       */
  C = proj_context_create();

  /* Create the projection. */
  P = proj_create(C, "+proj=utm +zone=32 +datum=WGS84");

  if (0 == P) {
    fprintf(stderr, "Failed to create transformation object.\n");
    return 1;
  }

  /* Longitude and latitude of Copenhagen, in degrees. */
  double lon = 12.0, lat = 55.0;

  /* Prepare the input */
  PJ_COORD c_in;
  c_in.lpzt.z = 0.0;
  c_in.lpzt.t = HUGE_VAL; // important only for time-dependent projections
  c_in.lp.lam = lon;
  c_in.lp.phi = lat;
  printf ("Input longitude: %g, latitude: %g (degrees)\n", c_in.lp.lam, c_in.lp.phi);

  /* Convert from degrees to radians */
  c_in.lp.lam = proj_torad(c_in.lp.lam);
  c_in.lp.phi = proj_torad(c_in.lp.phi);

  /* Compute easting and northing */
  PJ_COORD c_out = proj_trans(P, PJ_FWD, c_in);
  printf ("Output easting: %g, northing: %g (meters)\n", c_out.enu.e, c_out.enu.n);

  /* Apply the inverse transform */
  PJ_COORD c_inv = proj_trans(P, PJ_INV, c_out);

  /* Convert from radians to degrees. */
  c_inv.lp.lam = proj_todeg(c_inv.lp.lam);
  c_inv.lp.phi = proj_todeg(c_inv.lp.phi);

  printf ("Inverse applied. Longitude: %g, latitude: %g (degrees)\n", c_inv.lp.lam, c_inv.lp.phi);

  /* Clean up */
  proj_destroy(P);
  proj_context_destroy(C); /* may be omitted in the single threaded case */
  return 0;
}
