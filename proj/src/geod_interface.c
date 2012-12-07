#include "projects.h"
#include "geod_interface.h"

void geod_ini(void) {
  GeodesicInit(&GlobalGeodesic, geod_a, geod_f);
}

void geod_pre(void) {
  double
    degree = PI/180,
    lat1 = phi1 / degree, lon1 = lam1 /degree, azi1 = al12 / degree;
  GeodesicLineInit(&GlobalGeodesicLine, &GlobalGeodesic,
		   lat1, lon1, azi1, 0U);
}

void geod_for(void) {
  double degree = PI/180, s12 = geod_S, lat2, lon2, azi2;
  Position(&GlobalGeodesicLine, s12, &lat2, &lon2, &azi2);
  azi2 += azi2 >= 0 ? -180 : 180; /* Compute back azimuth */
  phi2 = lat2 * degree;
  lam2 = lon2 * degree;
  al21 = azi2 * degree;
}

void geod_inv(void) {
  double
    degree = PI / 180,
    lat1 = phi1 / degree, lon1 = lam1 / degree,
    lat2 = phi2 / degree, lon2 = lam2 / degree,
    azi1, azi2, s12;
  Inverse(&GlobalGeodesic, lat1, lon1, lat2, lon2, &s12, &azi1, &azi2);
  azi2 += azi2 >= 0 ? -180 : 180; /* Compute back azimuth */
  al12 = azi1 * degree; al21 = azi2 * degree; geod_S = s12;
}
