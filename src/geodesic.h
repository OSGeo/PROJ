/*
 * This is a C implementation of the geodesic algorithms described in
 *
 *   C. F. F. Karney,
 *   Algorithms for geodesics,
 *   J. Geodesy (2012);
 *   http://dx.doi.org/10.1007/s00190-012-0578-z
 *   Addenda: http://geographiclib.sf.net/geod-addenda.html
 *
 * The principal advantages of these algorithms over previous ones (e.g.,
 * Vincenty, 1975) are
 *   * accurate to round off for abs(f) < 1/50;
 *   * the solution of the inverse problem is always found;
 *   * differential and integral properties of geodesics are computed.
 *
 * The shortest path between two points on the ellipsoid at (lat1, lon1) and
 * (lat2, lon2) is called the geodesic.  Its length is s12 and the geodesic
 * from point 1 to point 2 has forward azimuths azi1 and azi2 at the two end
 * points.
 *
 * Traditionally two geodesic problems are considered:
 *   * the direct problem -- given lat1, lon1, s12, and azi1, determine
 *     lat2, lon2, and azi2.
 *   * the inverse problem -- given lat1, lon1, lat2, lon2, determine
 *     s12, azi1, and azi2.
 *
 * The ellipsoid is specified by its equatorial radius a (typically in meters)
 * and flattening f.  The routines are accurate to round off with double
 * precision arithmetic provided that abs(f) < 1/50; for the WGS84 ellipsoid,
 * the errors are less than 15 nanometers.  (Reasonably accurate results are
 * obtained for abs(f) < 1/5.)  Latitudes, longitudes, and azimuths are in
 * degrees.  Latitudes must lie in [-90,90] and longitudes and azimuths must
 * lie in [-540,540).  The returned values for longitude and azimuths are in
 * [-180,180).  The distance s12 is measured in meters (more precisely the same
 * units as a).
 *
 * The routines also calculate several other quantities of interest
 *   * SS12 is the area between the geodesic from point 1 to point 2 and the
 *     equator; i.e., it is the area, measured counter-clockwise, of the
 *     quadrilateral with corners (lat1,lon1), (0,lon1), (0,lon2), and
 *     (lat2,lon2).  It is given in meters^2.
 *   * m12, the reduced length of the geodesic is defined such that if the
 *     initial azimuth is perturbed by dazi1 (radians) then the second point is
 *     displaced by m12 dazi1 in the direction perpendicular to the geodesic.
 *     m12 is given in meters.  On a curved surface the reduced length obeys a
 *     symmetry relation, m12 + m21 = 0.  On a flat surface, we have m12 = s12.
 *   * MM12 and MM21 are geodesic scales.  If two geodesics are parallel at
 *     point 1 and separated by a small distance dt, then they are separated by
 *     a distance MM12 dt at point 2.  MM21 is defined similarly (with the
 *     geodesics being parallel to one another at point 2).  MM12 and MM21 are
 *     dimensionless quantities.  On a flat surface, we have MM12 = MM21 = 1.
 *   * a12 is the arc length on the auxiliary sphere.  This is a construct for
 *     converting the problem to one in spherical trigonometry.  a12 is
 *     measured in degrees.  The spherical arc length from one equator crossing
 *     to the next is always 180 degrees.
 *
 * Simple interface
 *
 *    #include "geodesic.h"
 *
 *    double a, f, lat1, lon1, azi1, lat2, lon2, azi2, s12;
 *    struct Geodesic g;
 *
 *    GeodesicInit(&g, a, f);
 *    Direct(&g, lat1, lon1, azi1, s12, &lat2, &lon2, &azi2);
 *    Inverse(&g, lat1, lon1, lat2, lon2, &s12, &azi1, &azi2);
 *
 * GeodesicInit initalizes g for the ellipsoid.  Subsequent calls to Direct and
 * Inverse solve the direct and inverse geodesic problems.
 *
 * Returning auxiliary quantities (continued from the previous example):
 *
 *    double a12, s12_a12, m12, M12, M21, S12;
 *    a12 = GenDirect(&g, lat1, lon1, azi1, 0, s12,
 *                    &lat1, &lat2, &azi2, 0, &m12, &M12, &M21, &S21);
 *    GenDirect(&g, lat1, lon1, azi1, 1, a12,
 *              &lat1, &lat2, &azi2, &s12, &m12, &M12, &M21, &S21);
 *    a12 = GenInverse(&g, lat1, lon1, lat2, lon2, &s12, &azi1, &azi2,
 *                     &m12, &M12, &M21, &S12);
 *
 * GenDirect is a generalized version of Direct allowing the return of the
 * auxiliary quantities.  With the first variant (arcmode = 0), the length of
 * the geodesic is specified by the true length s12 and the arc length a12 is
 * returned as the function value.  In the second variant (arcmode = 1), the
 * length is specified by the arc length a12 (in degrees), and the true length
 * is returned via &s12.
 *
 *    a12 = GenInverse(&g, lat1, lon1, lat2, lon2, &s12, &azi1, &azi2,
 *                     &m12, &M12, &M21, &S12);
 *
 * GenInverse is a generalized version of Inverse allowing the return of the
 * auxiliary quantities.
 *
 * Any of the "return" arguments &s12, etc. in these routines may be replaced
 * by 0, if you do not need some quantities computed.
 *
 * Computing multiple points on a geodesic.  This may be accomplished by
 * repeated invocations of Direct varying s12.  However, it is more efficient
 * to create a GeodesicLine object, as follows.
 *
 *    struct GeodesicLine l;
 *    int caps = 0;
 *
 *    GeodesicLineInit(&l, &g, a, f, lat1, lon1, azi1, caps);
 *    Position(l, s12, &lat2, &lon2, &azi2)
 *
 * caps is a bit mask specifying the capabilities of the GeodesicLine object.
 * It should be an or'ed combination of
 *
 *    LATITUDE        compute lat2 (in effect this is always set)
 *    LONGITUDE       compute lon2
 *    AZIMUTH         compute azi2 (in effect this is always set)
 *    DISTANCE        compute s12
 *    DISTANCE_IN     allow the length to be specified in terms of distance
 *    REDUCEDLENGTH   compute m12
 *    GEODESICSCALE   compute M12 and M21
 *    AREA            compute S12
 *    ALL             all of the above
 *
 * caps = 0 is treated as LATITUDE | LONGITUDE | AZIMUTH | DISTANCE_IN (to
 * support the solution "standard" direct problem).
 *
 * There's also a generalized version of Position
 *
 *    a12 = GenPosition(&l, arcmode, s12_a12,
 *                      &lat2, &lon2, &azi2, &s12, &m12, &M12, &M21, &S12);
 *
 * See the documentation on GenDirect for the meaning of arcmode.
 *
 * Copyright (c) Charles Karney (2012) <charles@karney.com> and licensed
 * under the MIT/X11 License.  For more information, see
 * http://geographiclib.sourceforge.net/
 *
 * This file was distributed with GeographicLib 1.27.
 */

#if !defined(GEODESIC_H)
#define GEODESIC_H 1

#if defined(__cplusplus)
extern "C" {
#endif

  struct Geodesic {
    double a, f, f1, e2, ep2, n, b, c2, etol2;
    double A3x[6], C3x[15], C4x[21];
  };

  struct GeodesicLine {
    double lat1, lon1, azi1;
    double a, f, b, c2, f1, salp0, calp0, k2,
      salp1, calp1, ssig1, csig1, dn1, stau1, ctau1, somg1, comg1,
      A1m1, A2m1, A3c, B11, B21, B31, A4, B41;
    double C1a[6+1], C1pa[6+1], C2a[6+1], C3a[6], C4a[6];
    unsigned caps;
  };

  void GeodesicInit(struct Geodesic* g, double a, double f);
  void GeodesicLineInit(struct GeodesicLine* l,
                        const struct Geodesic* g,
                        double lat1, double lon1, double azi1, unsigned caps);

  void Direct(const struct Geodesic* g,
              double lat1, double lon1, double azi1, double s12,
              double* plat2, double* plon2, double* pazi2);
  void Inverse(const struct Geodesic* g,
               double lat1, double lon1, double lat2, double lon2,
               double* ps12, double* pazi1, double* pazi2);
  void Position(const struct GeodesicLine* l, double s12,
                double* plat2, double* plon2, double* pazi2);

  double GenDirect(const struct Geodesic* g,
                   double lat1, double lon1, double azi1,
                   int arcmode, double s12_a12,
                   double* plat2, double* plon2, double* pazi2,
                   double* ps12, double* pm12, double* pM12, double* pM21,
                   double* pS12);
  double GenInverse(const struct Geodesic* g,
                    double lat1, double lon1, double lat2, double lon2,
                    double* ps12, double* pazi1, double* pazi2,
                    double* pm12, double* pM12, double* pM21, double* pS12);
  double GenPosition(const struct GeodesicLine* l,
                     int arcmode, double s12_a12,
                     double* plat2, double* plon2, double* pazi2,
                     double* ps12, double* pm12,
                     double* pM12, double* pM21,
                     double* pS12);

  enum mask {
    NONE          = 0U,
    LATITUDE      = 1U<<7  | 0U,
    LONGITUDE     = 1U<<8  | 1U<<3,
    AZIMUTH       = 1U<<9  | 0U,
    DISTANCE      = 1U<<10 | 1U<<0,
    DISTANCE_IN   = 1U<<11 | 1U<<0 | 1U<<1,
    REDUCEDLENGTH = 1U<<12 | 1U<<0 | 1U<<2,
    GEODESICSCALE = 1U<<13 | 1U<<0 | 1U<<2,
    AREA          = 1U<<14 | 1U<<4,
    ALL           = 0x7F80U| 0x1FU
  };

#if defined(__cplusplus)
}
#endif

#endif
