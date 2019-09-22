/**
 * \file geodtest.c
 * \brief Test suite for the geodesic routines in C
 *
 * Run these tests by configuring with cmake and running "make test".
 *
 * Copyright (c) Charles Karney (2015-2019) <charles@karney.com> and licensed
 * under the MIT/X11 License.  For more information, see
 * https://geographiclib.sourceforge.io/
 **********************************************************************/

/** @cond SKIP */

#include "geodesic.h"
#include <stdio.h>
#include <math.h>

#if defined(_MSC_VER)
/* Squelch warnings about assignment within conditional expression */
#  pragma warning (disable: 4706)
#endif

#if !defined(__cplusplus)
#define nullptr 0
#endif

static const double wgs84_a = 6378137, wgs84_f = 1/298.257223563; /* WGS84 */

static int checkEquals(double x, double y, double d) {
  if (fabs(x - y) <= d)
    return 0;
  printf("checkEquals fails: %.7g != %.7g +/- %.7g\n", x, y, d);
  return 1;
}

static int checkNaN(double x) {
  /* cppcheck-suppress duplicateExpression */
  if (x != x)
    return 0;
  printf("checkNaN fails: %.7g\n", x);
  return 1;
}

static const int ncases = 20;
static const double testcases[20][12] = {
  {35.60777, -139.44815, 111.098748429560326,
   -11.17491, -69.95921, 129.289270889708762,
   8935244.5604818305, 80.50729714281974, 6273170.2055303837,
   0.16606318447386067, 0.16479116945612937, 12841384694976.432},
  {55.52454, 106.05087, 22.020059880982801,
   77.03196, 197.18234, 109.112041110671519,
   4105086.1713924406, 36.892740690445894, 3828869.3344387607,
   0.80076349608092607, 0.80101006984201008, 61674961290615.615},
  {-21.97856, 142.59065, -32.44456876433189,
   41.84138, 98.56635, -41.84359951440466,
   8394328.894657671, 75.62930491011522, 6161154.5773110616,
   0.24816339233950381, 0.24930251203627892, -6637997720646.717},
  {-66.99028, 112.2363, 173.73491240878403,
   -12.70631, 285.90344, 2.512956620913668,
   11150344.2312080241, 100.278634181155759, 6289939.5670446687,
   -0.17199490274700385, -0.17722569526345708, -121287239862139.744},
  {-17.42761, 173.34268, -159.033557661192928,
   -15.84784, 5.93557, -20.787484651536988,
   16076603.1631180673, 144.640108810286253, 3732902.1583877189,
   -0.81273638700070476, -0.81299800519154474, 97825992354058.708},
  {32.84994, 48.28919, 150.492927788121982,
   -56.28556, 202.29132, 48.113449399816759,
   16727068.9438164461, 150.565799985466607, 3147838.1910180939,
   -0.87334918086923126, -0.86505036767110637, -72445258525585.010},
  {6.96833, 52.74123, 92.581585386317712,
   -7.39675, 206.17291, 90.721692165923907,
   17102477.2496958388, 154.147366239113561, 2772035.6169917581,
   -0.89991282520302447, -0.89986892177110739, -1311796973197.995},
  {-50.56724, -16.30485, -105.439679907590164,
   -33.56571, -94.97412, -47.348547835650331,
   6455670.5118668696, 58.083719495371259, 5409150.7979815838,
   0.53053508035997263, 0.52988722644436602, 41071447902810.047},
  {-58.93002, -8.90775, 140.965397902500679,
   -8.91104, 133.13503, 19.255429433416599,
   11756066.0219864627, 105.755691241406877, 6151101.2270708536,
   -0.26548622269867183, -0.27068483874510741, -86143460552774.735},
  {-68.82867, -74.28391, 93.774347763114881,
   -50.63005, -8.36685, 34.65564085411343,
   3956936.926063544, 35.572254987389284, 3708890.9544062657,
   0.81443963736383502, 0.81420859815358342, -41845309450093.787},
  {-10.62672, -32.0898, -86.426713286747751,
   5.883, -134.31681, -80.473780971034875,
   11470869.3864563009, 103.387395634504061, 6184411.6622659713,
   -0.23138683500430237, -0.23155097622286792, 4198803992123.548},
  {-21.76221, 166.90563, 29.319421206936428,
   48.72884, 213.97627, 43.508671946410168,
   9098627.3986554915, 81.963476716121964, 6299240.9166992283,
   0.13965943368590333, 0.14152969707656796, 10024709850277.476},
  {-19.79938, -174.47484, 71.167275780171533,
   -11.99349, -154.35109, 65.589099775199228,
   2319004.8601169389, 20.896611684802389, 2267960.8703918325,
   0.93427001867125849, 0.93424887135032789, -3935477535005.785},
  {-11.95887, -116.94513, 92.712619830452549,
   4.57352, 7.16501, 78.64960934409585,
   13834722.5801401374, 124.688684161089762, 5228093.177931598,
   -0.56879356755666463, -0.56918731952397221, -9919582785894.853},
  {-87.85331, 85.66836, -65.120313040242748,
   66.48646, 16.09921, -4.888658719272296,
   17286615.3147144645, 155.58592449699137, 2635887.4729110181,
   -0.90697975771398578, -0.91095608883042767, 42667211366919.534},
  {1.74708, 128.32011, -101.584843631173858,
   -11.16617, 11.87109, -86.325793296437476,
   12942901.1241347408, 116.650512484301857, 5682744.8413270572,
   -0.44857868222697644, -0.44824490340007729, 10763055294345.653},
  {-25.72959, -144.90758, -153.647468693117198,
   -57.70581, -269.17879, -48.343983158876487,
   9413446.7452453107, 84.664533838404295, 6356176.6898881281,
   0.09492245755254703, 0.09737058264766572, 74515122850712.444},
  {-41.22777, 122.32875, 14.285113402275739,
   -7.57291, 130.37946, 10.805303085187369,
   3812686.035106021, 34.34330804743883, 3588703.8812128856,
   0.82605222593217889, 0.82572158200920196, -2456961531057.857},
  {11.01307, 138.25278, 79.43682622782374,
   6.62726, 247.05981, 103.708090215522657,
   11911190.819018408, 107.341669954114577, 6070904.722786735,
   -0.29767608923657404, -0.29785143390252321, 17121631423099.696},
  {-29.47124, 95.14681, -163.779130441688382,
   -27.46601, -69.15955, -15.909335945554969,
   13487015.8381145492, 121.294026715742277, 5481428.9945736388,
   -0.51527225545373252, -0.51556587964721788, 104679964020340.318}};

static int testinverse() {
  double lat1, lon1, azi1, lat2, lon2, azi2, s12, a12, m12, M12, M21, S12;
  double azi1a, azi2a, s12a, a12a, m12a, M12a, M21a, S12a;
  struct geod_geodesic g;
  int i, result = 0;
  geod_init(&g, wgs84_a, wgs84_f);
  for (i = 0; i < ncases; ++i) {
    lat1 = testcases[i][0]; lon1 = testcases[i][1]; azi1 = testcases[i][2];
    lat2 = testcases[i][3]; lon2 = testcases[i][4]; azi2 = testcases[i][5];
    s12 = testcases[i][6]; a12 = testcases[i][7]; m12 = testcases[i][8];
    M12 = testcases[i][9]; M21 = testcases[i][10]; S12 = testcases[i][11];
    a12a = geod_geninverse(&g, lat1, lon1, lat2, lon2, &s12a, &azi1a, &azi2a,
               &m12a, &M12a, &M21a, &S12a);
    result += checkEquals(azi1, azi1a, 1e-13);
    result += checkEquals(azi2, azi2a, 1e-13);
    result += checkEquals(s12, s12a, 1e-8);
    result += checkEquals(a12, a12a, 1e-13);
    result += checkEquals(m12, m12a, 1e-8);
    result += checkEquals(M12, M12a, 1e-15);
    result += checkEquals(M21, M21a, 1e-15);
    result += checkEquals(S12, S12a, 0.1);
  }
  return result;
}

static int testdirect() {
  double lat1, lon1, azi1, lat2, lon2, azi2, s12, a12, m12, M12, M21, S12;
  double lat2a, lon2a, azi2a, a12a, m12a, M12a, M21a, S12a;
  struct geod_geodesic g;
  int i, result = 0;
  unsigned flags = GEOD_LONG_UNROLL;
  geod_init(&g, wgs84_a, wgs84_f);
  for (i = 0; i < ncases; ++i) {
    lat1 = testcases[i][0]; lon1 = testcases[i][1]; azi1 = testcases[i][2];
    lat2 = testcases[i][3]; lon2 = testcases[i][4]; azi2 = testcases[i][5];
    s12 = testcases[i][6]; a12 = testcases[i][7]; m12 = testcases[i][8];
    M12 = testcases[i][9]; M21 = testcases[i][10]; S12 = testcases[i][11];
    a12a = geod_gendirect(&g, lat1, lon1, azi1, flags, s12,
              &lat2a, &lon2a, &azi2a, nullptr, &m12a, &M12a, &M21a, &S12a);
    result += checkEquals(lat2, lat2a, 1e-13);
    result += checkEquals(lon2, lon2a, 1e-13);
    result += checkEquals(azi2, azi2a, 1e-13);
    result += checkEquals(a12, a12a, 1e-13);
    result += checkEquals(m12, m12a, 1e-8);
    result += checkEquals(M12, M12a, 1e-15);
    result += checkEquals(M21, M21a, 1e-15);
    result += checkEquals(S12, S12a, 0.1);
  }
  return result;
}

static int testarcdirect() {
  double lat1, lon1, azi1, lat2, lon2, azi2, s12, a12, m12, M12, M21, S12;
  double lat2a, lon2a, azi2a, s12a, m12a, M12a, M21a, S12a;
  struct geod_geodesic g;
  int i, result = 0;
  unsigned flags = GEOD_ARCMODE | GEOD_LONG_UNROLL;
  geod_init(&g, wgs84_a, wgs84_f);
  for (i = 0; i < ncases; ++i) {
    lat1 = testcases[i][0]; lon1 = testcases[i][1]; azi1 = testcases[i][2];
    lat2 = testcases[i][3]; lon2 = testcases[i][4]; azi2 = testcases[i][5];
    s12 = testcases[i][6]; a12 = testcases[i][7]; m12 = testcases[i][8];
    M12 = testcases[i][9]; M21 = testcases[i][10]; S12 = testcases[i][11];
    geod_gendirect(&g, lat1, lon1, azi1, flags, a12,
                   &lat2a, &lon2a, &azi2a, &s12a, &m12a, &M12a, &M21a, &S12a);
    result += checkEquals(lat2, lat2a, 1e-13);
    result += checkEquals(lon2, lon2a, 1e-13);
    result += checkEquals(azi2, azi2a, 1e-13);
    result += checkEquals(s12, s12a, 1e-8);
    result += checkEquals(m12, m12a, 1e-8);
    result += checkEquals(M12, M12a, 1e-15);
    result += checkEquals(M21, M21a, 1e-15);
    result += checkEquals(S12, S12a, 0.1);
  }
  return result;
}

static int GeodSolve0() {
  double azi1, azi2, s12;
  struct geod_geodesic g;
  int result = 0;
  geod_init(&g, wgs84_a, wgs84_f);
  geod_inverse(&g, 40.6, -73.8, 49.01666667, 2.55, &s12, &azi1, &azi2);
  result += checkEquals(azi1, 53.47022, 0.5e-5);
  result += checkEquals(azi2, 111.59367, 0.5e-5);
  result += checkEquals(s12, 5853226, 0.5);
  return result;
}

static int GeodSolve1() {
  double lat2, lon2, azi2;
  struct geod_geodesic g;
  int result = 0;
  geod_init(&g, wgs84_a, wgs84_f);
  geod_direct(&g, 40.63972222, -73.77888889, 53.5, 5850e3,
              &lat2, &lon2, &azi2);
  result += checkEquals(lat2, 49.01467, 0.5e-5);
  result += checkEquals(lon2, 2.56106, 0.5e-5);
  result += checkEquals(azi2, 111.62947, 0.5e-5);
  return result;
}

static int GeodSolve2() {
  /* Check fix for antipodal prolate bug found 2010-09-04 */
  double azi1, azi2, s12;
  struct geod_geodesic g;
  int result = 0;
  geod_init(&g, 6.4e6, -1/150.0);
  geod_inverse(&g, 0.07476, 0, -0.07476, 180, &s12, &azi1, &azi2);
  result += checkEquals(azi1, 90.00078, 0.5e-5);
  result += checkEquals(azi2, 90.00078, 0.5e-5);
  result += checkEquals(s12, 20106193, 0.5);
  geod_inverse(&g, 0.1, 0, -0.1, 180, &s12, &azi1, &azi2);
  result += checkEquals(azi1, 90.00105, 0.5e-5);
  result += checkEquals(azi2, 90.00105, 0.5e-5);
  result += checkEquals(s12, 20106193, 0.5);
  return result;
}

static int GeodSolve4() {
  /* Check fix for short line bug found 2010-05-21 */
  double s12;
  struct geod_geodesic g;
  int result = 0;
  geod_init(&g, wgs84_a, wgs84_f);
  geod_inverse(&g, 36.493349428792, 0, 36.49334942879201, .0000008,
               &s12, nullptr, nullptr);
  result += checkEquals(s12, 0.072, 0.5e-3);
  return result;
}

static int GeodSolve5() {
  /* Check fix for point2=pole bug found 2010-05-03 */
  double lat2, lon2, azi2;
  struct geod_geodesic g;
  int result = 0;
  geod_init(&g, wgs84_a, wgs84_f);
  geod_direct(&g, 0.01777745589997, 30, 0, 10e6, &lat2, &lon2, &azi2);
  result += checkEquals(lat2, 90, 0.5e-5);
  if (lon2 < 0) {
    result += checkEquals(lon2, -150, 0.5e-5);
    result += checkEquals(fabs(azi2), 180, 0.5e-5);
  } else {
    result += checkEquals(lon2, 30, 0.5e-5);
    result += checkEquals(azi2, 0, 0.5e-5);
  }
  return result;
}

static int GeodSolve6() {
  /* Check fix for volatile sbet12a bug found 2011-06-25 (gcc 4.4.4
   * x86 -O3).  Found again on 2012-03-27 with tdm-mingw32 (g++ 4.6.1). */
  double s12;
  struct geod_geodesic g;
  int result = 0;
  geod_init(&g, wgs84_a, wgs84_f);
  geod_inverse(&g, 88.202499451857, 0,
               -88.202499451857, 179.981022032992859592,
               &s12, nullptr, nullptr);
  result += checkEquals(s12, 20003898.214, 0.5e-3);
  geod_inverse(&g, 89.262080389218, 0,
               -89.262080389218, 179.992207982775375662,
               &s12, nullptr, nullptr);
  result += checkEquals(s12, 20003925.854, 0.5e-3);
  geod_inverse(&g, 89.333123580033, 0,
               -89.333123580032997687, 179.99295812360148422,
               &s12, nullptr, nullptr);
  result += checkEquals(s12, 20003926.881, 0.5e-3);
  return result;
}

static int GeodSolve9() {
  /* Check fix for volatile x bug found 2011-06-25 (gcc 4.4.4 x86 -O3) */
  double s12;
  struct geod_geodesic g;
  int result = 0;
  geod_init(&g, wgs84_a, wgs84_f);
  geod_inverse(&g, 56.320923501171, 0,
               -56.320923501171, 179.664747671772880215,
               &s12, nullptr, nullptr);
  result += checkEquals(s12, 19993558.287, 0.5e-3);
  return result;
}

static int GeodSolve10() {
  /* Check fix for adjust tol1_ bug found 2011-06-25 (Visual Studio
   * 10 rel + debug) */
  double s12;
  struct geod_geodesic g;
  int result = 0;
  geod_init(&g, wgs84_a, wgs84_f);
  geod_inverse(&g, 52.784459512564, 0,
               -52.784459512563990912, 179.634407464943777557,
               &s12, nullptr, nullptr);
  result += checkEquals(s12, 19991596.095, 0.5e-3);
  return result;
}

static int GeodSolve11() {
  /* Check fix for bet2 = -bet1 bug found 2011-06-25 (Visual Studio
   * 10 rel + debug) */
  double s12;
  struct geod_geodesic g;
  int result = 0;
  geod_init(&g, wgs84_a, wgs84_f);
  geod_inverse(&g, 48.522876735459, 0,
               -48.52287673545898293, 179.599720456223079643,
               &s12, nullptr, nullptr);
  result += checkEquals(s12, 19989144.774, 0.5e-3);
  return result;
}

static int GeodSolve12() {
  /* Check fix for inverse geodesics on extreme prolate/oblate
   * ellipsoids Reported 2012-08-29 Stefan Guenther
   * <stefan.gunther@embl.de>; fixed 2012-10-07 */
  double azi1, azi2, s12;
  struct geod_geodesic g;
  int result = 0;
  geod_init(&g, 89.8, -1.83);
  geod_inverse(&g, 0, 0, -10, 160, &s12, &azi1, &azi2);
  result += checkEquals(azi1, 120.27, 1e-2);
  result += checkEquals(azi2, 105.15, 1e-2);
  result += checkEquals(s12, 266.7, 1e-1);
  return result;
}

static int GeodSolve14() {
  /* Check fix for inverse ignoring lon12 = nan */
  double azi1, azi2, s12, nan;
  struct geod_geodesic g;
  int result = 0;
  {
    double minus1 = -1;
    /* cppcheck-suppress wrongmathcall */
    nan = sqrt(minus1);
  }
  geod_init(&g, wgs84_a, wgs84_f);
  geod_inverse(&g, 0, 0, 1, nan, &s12, &azi1, &azi2);
  result += checkNaN(azi1);
  result += checkNaN(azi2);
  result += checkNaN(s12);
  return result;
}

static int GeodSolve15() {
  /* Initial implementation of Math::eatanhe was wrong for e^2 < 0.  This
   * checks that this is fixed. */
  double S12;
  struct geod_geodesic g;
  int result = 0;
  geod_init(&g, 6.4e6, -1/150.0);
  geod_gendirect(&g, 1, 2, 3, 0, 4, nullptr, nullptr, nullptr,
                 nullptr, nullptr, nullptr, nullptr, &S12);
  result += checkEquals(S12, 23700, 0.5);
  return result;
}

static int GeodSolve17() {
  /* Check fix for LONG_UNROLL bug found on 2015-05-07 */
  double lat2, lon2, azi2;
  struct geod_geodesic g;
  struct geod_geodesicline l;
  int result = 0;
  unsigned flags = GEOD_LONG_UNROLL;
  geod_init(&g, wgs84_a, wgs84_f);
  geod_gendirect(&g, 40, -75, -10, flags, 2e7, &lat2, &lon2, &azi2,
                 nullptr, nullptr, nullptr, nullptr, nullptr);
  result += checkEquals(lat2, -39, 1);
  result += checkEquals(lon2, -254, 1);
  result += checkEquals(azi2, -170, 1);
  geod_lineinit(&l, &g, 40, -75, -10, 0);
  geod_genposition(&l, flags, 2e7, &lat2, &lon2, &azi2,
                   nullptr, nullptr, nullptr, nullptr, nullptr);
  result += checkEquals(lat2, -39, 1);
  result += checkEquals(lon2, -254, 1);
  result += checkEquals(azi2, -170, 1);
  geod_direct(&g, 40, -75, -10, 2e7, &lat2, &lon2, &azi2);
  result += checkEquals(lat2, -39, 1);
  result += checkEquals(lon2, 105, 1);
  result += checkEquals(azi2, -170, 1);
  geod_position(&l, 2e7, &lat2, &lon2, &azi2);
  result += checkEquals(lat2, -39, 1);
  result += checkEquals(lon2, 105, 1);
  result += checkEquals(azi2, -170, 1);
  return result;
}

static int GeodSolve26() {
  /* Check 0/0 problem with area calculation on sphere 2015-09-08 */
  double S12;
  struct geod_geodesic g;
  int result = 0;
  geod_init(&g, 6.4e6, 0);
  geod_geninverse(&g, 1, 2, 3, 4, nullptr, nullptr, nullptr,
                  nullptr, nullptr, nullptr, &S12);
  result += checkEquals(S12, 49911046115.0, 0.5);
  return result;
}

static int GeodSolve28() {
  /* Check for bad placement of assignment of r.a12 with |f| > 0.01 (bug in
   * Java implementation fixed on 2015-05-19). */
  double a12;
  struct geod_geodesic g;
  int result = 0;
  geod_init(&g, 6.4e6, 0.1);
  a12 = geod_gendirect(&g, 1, 2, 10, 0, 5e6, nullptr, nullptr, nullptr,
                       nullptr, nullptr, nullptr, nullptr, nullptr);
  result += checkEquals(a12, 48.55570690, 0.5e-8);
  return result;
}

static int GeodSolve33() {
  /* Check max(-0.0,+0.0) issues 2015-08-22 (triggered by bugs in Octave --
   * sind(-0.0) = +0.0 -- and in some version of Visual Studio --
   * fmod(-0.0, 360.0) = +0.0. */
  double azi1, azi2, s12;
  struct geod_geodesic g;
  int result = 0;
  geod_init(&g, wgs84_a, wgs84_f);
  geod_inverse(&g, 0, 0, 0, 179, &s12, &azi1, &azi2);
  result += checkEquals(azi1, 90.00000, 0.5e-5);
  result += checkEquals(azi2, 90.00000, 0.5e-5);
  result += checkEquals(s12, 19926189, 0.5);
  geod_inverse(&g, 0, 0, 0, 179.5, &s12, &azi1, &azi2);
  result += checkEquals(azi1, 55.96650, 0.5e-5);
  result += checkEquals(azi2, 124.03350, 0.5e-5);
  result += checkEquals(s12, 19980862, 0.5);
  geod_inverse(&g, 0, 0, 0, 180, &s12, &azi1, &azi2);
  result += checkEquals(azi1, 0.00000, 0.5e-5);
  result += checkEquals(fabs(azi2), 180.00000, 0.5e-5);
  result += checkEquals(s12, 20003931, 0.5);
  geod_inverse(&g, 0, 0, 1, 180, &s12, &azi1, &azi2);
  result += checkEquals(azi1, 0.00000, 0.5e-5);
  result += checkEquals(fabs(azi2), 180.00000, 0.5e-5);
  result += checkEquals(s12, 19893357, 0.5);
  geod_init(&g, 6.4e6, 0);
  geod_inverse(&g, 0, 0, 0, 179, &s12, &azi1, &azi2);
  result += checkEquals(azi1, 90.00000, 0.5e-5);
  result += checkEquals(azi2, 90.00000, 0.5e-5);
  result += checkEquals(s12, 19994492, 0.5);
  geod_inverse(&g, 0, 0, 0, 180, &s12, &azi1, &azi2);
  result += checkEquals(azi1, 0.00000, 0.5e-5);
  result += checkEquals(fabs(azi2), 180.00000, 0.5e-5);
  result += checkEquals(s12, 20106193, 0.5);
  geod_inverse(&g, 0, 0, 1, 180, &s12, &azi1, &azi2);
  result += checkEquals(azi1, 0.00000, 0.5e-5);
  result += checkEquals(fabs(azi2), 180.00000, 0.5e-5);
  result += checkEquals(s12, 19994492, 0.5);
  geod_init(&g, 6.4e6, -1/300.0);
  geod_inverse(&g, 0, 0, 0, 179, &s12, &azi1, &azi2);
  result += checkEquals(azi1, 90.00000, 0.5e-5);
  result += checkEquals(azi2, 90.00000, 0.5e-5);
  result += checkEquals(s12, 19994492, 0.5);
  geod_inverse(&g, 0, 0, 0, 180, &s12, &azi1, &azi2);
  result += checkEquals(azi1, 90.00000, 0.5e-5);
  result += checkEquals(azi2, 90.00000, 0.5e-5);
  result += checkEquals(s12, 20106193, 0.5);
  geod_inverse(&g, 0, 0, 0.5, 180, &s12, &azi1, &azi2);
  result += checkEquals(azi1, 33.02493, 0.5e-5);
  result += checkEquals(azi2, 146.97364, 0.5e-5);
  result += checkEquals(s12, 20082617, 0.5);
  geod_inverse(&g, 0, 0, 1, 180, &s12, &azi1, &azi2);
  result += checkEquals(azi1, 0.00000, 0.5e-5);
  result += checkEquals(fabs(azi2), 180.00000, 0.5e-5);
  result += checkEquals(s12, 20027270, 0.5);

  return result;
}

static int GeodSolve55() {
  /* Check fix for nan + point on equator or pole not returning all nans in
   * Geodesic::Inverse, found 2015-09-23. */
  double azi1, azi2, s12, nan;
  struct geod_geodesic g;
  int result = 0;
  {
    double minus1 = -1;
    /* cppcheck-suppress wrongmathcall */
    nan = sqrt(minus1);
  }
  geod_init(&g, wgs84_a, wgs84_f);
  geod_inverse(&g, nan, 0, 0, 90, &s12, &azi1, &azi2);
  result += checkNaN(azi1);
  result += checkNaN(azi2);
  result += checkNaN(s12);
  geod_inverse(&g, nan, 0, 90, 9, &s12, &azi1, &azi2);
  result += checkNaN(azi1);
  result += checkNaN(azi2);
  result += checkNaN(s12);
  return result;
}

static int GeodSolve59() {
  /* Check for points close with longitudes close to 180 deg apart. */
  double azi1, azi2, s12;
  struct geod_geodesic g;
  int result = 0;
  geod_init(&g, wgs84_a, wgs84_f);
  geod_inverse(&g, 5, 0.00000000000001, 10, 180, &s12, &azi1, &azi2);
  result += checkEquals(azi1, 0.000000000000035, 1.5e-14);
  result += checkEquals(azi2, 179.99999999999996, 1.5e-14);
  result += checkEquals(s12, 18345191.174332713, 5e-9);
  return result;
}

static int GeodSolve61() {
  /* Make sure small negative azimuths are west-going */
  double lat2, lon2, azi2;
  struct geod_geodesic g;
  struct geod_geodesicline l;
  int result = 0;
  unsigned flags = GEOD_LONG_UNROLL;
  geod_init(&g, wgs84_a, wgs84_f);
  geod_gendirect(&g, 45, 0, -0.000000000000000003, flags, 1e7,
                 &lat2, &lon2, &azi2,
                 nullptr, nullptr, nullptr, nullptr, nullptr);
  result += checkEquals(lat2, 45.30632, 0.5e-5);
  result += checkEquals(lon2, -180, 0.5e-5);
  result += checkEquals(fabs(azi2), 180, 0.5e-5);
  geod_inverseline(&l, &g, 45, 0, 80, -0.000000000000000003, 0);
  geod_genposition(&l, flags, 1e7, &lat2, &lon2, &azi2,
                   nullptr, nullptr, nullptr, nullptr, nullptr);
  result += checkEquals(lat2, 45.30632, 0.5e-5);
  result += checkEquals(lon2, -180, 0.5e-5);
  result += checkEquals(fabs(azi2), 180, 0.5e-5);
  return result;
}

static int GeodSolve65() {
  /* Check for bug in east-going check in GeodesicLine (needed to check for
   * sign of 0) and sign error in area calculation due to a bogus override of
   * the code for alp12.  Found/fixed on 2015-12-19. */
  double lat2, lon2, azi2, s12, a12, m12, M12, M21, S12;
  struct geod_geodesic g;
  struct geod_geodesicline l;
  int result = 0;
  unsigned flags = GEOD_LONG_UNROLL, caps = GEOD_ALL;
  geod_init(&g, wgs84_a, wgs84_f);
  geod_inverseline(&l, &g, 30, -0.000000000000000001, -31, 180, caps);
  a12 = geod_genposition(&l, flags, 1e7,
                         &lat2, &lon2, &azi2, &s12, &m12, &M12, &M21, &S12);
  result += checkEquals(lat2, -60.23169, 0.5e-5);
  result += checkEquals(lon2, -0.00000, 0.5e-5);
  result += checkEquals(fabs(azi2), 180.00000, 0.5e-5);
  result += checkEquals(s12, 10000000, 0.5);
  result += checkEquals(a12, 90.06544, 0.5e-5);
  result += checkEquals(m12, 6363636, 0.5);
  result += checkEquals(M12, -0.0012834, 0.5e-7);
  result += checkEquals(M21, 0.0013749, 0.5e-7);
  result += checkEquals(S12, 0, 0.5);
  a12 = geod_genposition(&l, flags, 2e7,
                         &lat2, &lon2, &azi2, &s12, &m12, &M12, &M21, &S12);
  result += checkEquals(lat2, -30.03547, 0.5e-5);
  result += checkEquals(lon2, -180.00000, 0.5e-5);
  result += checkEquals(azi2, -0.00000, 0.5e-5);
  result += checkEquals(s12, 20000000, 0.5);
  result += checkEquals(a12, 179.96459, 0.5e-5);
  result += checkEquals(m12, 54342, 0.5);
  result += checkEquals(M12, -1.0045592, 0.5e-7);
  result += checkEquals(M21, -0.9954339, 0.5e-7);
  result += checkEquals(S12, 127516405431022.0, 0.5);
  return result;
}

static int GeodSolve67() {
  /* Check for InverseLine if line is slightly west of S and that s13 is
   * correctly set. */
  double lat2, lon2, azi2;
  struct geod_geodesic g;
  struct geod_geodesicline l;
  int result = 0;
  unsigned flags = GEOD_LONG_UNROLL;
  geod_init(&g, wgs84_a, wgs84_f);
  geod_inverseline(&l, &g, -5, -0.000000000000002, -10, 180, 0);
  geod_genposition(&l, flags, 2e7, &lat2, &lon2, &azi2,
                   nullptr, nullptr, nullptr, nullptr, nullptr);
  result += checkEquals(lat2, 4.96445, 0.5e-5);
  result += checkEquals(lon2, -180.00000, 0.5e-5);
  result += checkEquals(azi2, -0.00000, 0.5e-5);
  geod_genposition(&l, flags, 0.5 * l.s13, &lat2, &lon2, &azi2,
                   nullptr, nullptr, nullptr, nullptr, nullptr);
  result += checkEquals(lat2, -87.52461, 0.5e-5);
  result += checkEquals(lon2, -0.00000, 0.5e-5);
  result += checkEquals(azi2, -180.00000, 0.5e-5);
  return result;
}

static int GeodSolve71() {
  /* Check that DirectLine sets s13. */
  double lat2, lon2, azi2;
  struct geod_geodesic g;
  struct geod_geodesicline l;
  int result = 0;
  geod_init(&g, wgs84_a, wgs84_f);
  geod_directline(&l, &g, 1, 2, 45, 1e7, 0);
  geod_position(&l, 0.5 * l.s13, &lat2, &lon2, &azi2);
  result += checkEquals(lat2, 30.92625, 0.5e-5);
  result += checkEquals(lon2, 37.54640, 0.5e-5);
  result += checkEquals(azi2, 55.43104, 0.5e-5);
  return result;
}

static int GeodSolve73() {
  /* Check for backwards from the pole bug reported by Anon on 2016-02-13.
   * This only affected the Java implementation.  It was introduced in Java
   * version 1.44 and fixed in 1.46-SNAPSHOT on 2016-01-17.
   * Also the + sign on azi2 is a check on the normalizing of azimuths
   * (converting -0.0 to +0.0). */
  double lat2, lon2, azi2;
  struct geod_geodesic g;
  int result = 0;
  geod_init(&g, wgs84_a, wgs84_f);
  geod_direct(&g, 90, 10, 180, -1e6,
              &lat2, &lon2, &azi2);
  result += checkEquals(lat2, 81.04623, 0.5e-5);
  result += checkEquals(lon2, -170, 0.5e-5);
  result += azi2 == 0 ? 0 : 1;
  result += 1/azi2 > 0 ? 0 : 1; /* Check that azi2 = +0.0 not -0.0 */
  return result;
}

static void planimeter(const struct geod_geodesic* g,
                       double points[][2], int N,
                       double* perimeter, double* area) {
  struct geod_polygon p;
  int i;
  geod_polygon_init(&p, 0);
  for (i = 0; i < N; ++i)
    geod_polygon_addpoint(g, &p, points[i][0], points[i][1]);
  geod_polygon_compute(g, &p, 0, 1, area, perimeter);
}

static void polylength(const struct geod_geodesic* g,
                       double points[][2], int N,
                       double* perimeter) {
  struct geod_polygon p;
  int i;
  geod_polygon_init(&p, 1);
  for (i = 0; i < N; ++i)
    geod_polygon_addpoint(g, &p, points[i][0], points[i][1]);
  geod_polygon_compute(g, &p, 0, 1, nullptr, perimeter);
}

static int GeodSolve74() {
  /* Check fix for inaccurate areas, bug introduced in v1.46, fixed
   * 2015-10-16. */
  double a12, s12, azi1, azi2, m12, M12, M21, S12;
  struct geod_geodesic g;
  int result = 0;
  geod_init(&g, wgs84_a, wgs84_f);
  a12 = geod_geninverse(&g, 54.1589, 15.3872, 54.1591, 15.3877,
                        &s12, &azi1, &azi2, &m12, &M12, &M21, &S12);
  result += checkEquals(azi1, 55.723110355, 5e-9);
  result += checkEquals(azi2, 55.723515675, 5e-9);
  result += checkEquals(s12,  39.527686385, 5e-9);
  result += checkEquals(a12,   0.000355495, 5e-9);
  result += checkEquals(m12,  39.527686385, 5e-9);
  result += checkEquals(M12,   0.999999995, 5e-9);
  result += checkEquals(M21,   0.999999995, 5e-9);
  result += checkEquals(S12, 286698586.30197, 5e-4);
  return result;
}

static int GeodSolve76() {
  /* The distance from Wellington and Salamanca (a classic failure of
   * Vincenty) */
  double azi1, azi2, s12;
  struct geod_geodesic g;
  int result = 0;
  geod_init(&g, wgs84_a, wgs84_f);
  geod_inverse(&g, -(41+19/60.0), 174+49/60.0, 40+58/60.0, -(5+30/60.0),
               &s12, &azi1, &azi2);
  result += checkEquals(azi1, 160.39137649664, 0.5e-11);
  result += checkEquals(azi2,  19.50042925176, 0.5e-11);
  result += checkEquals(s12,  19960543.857179, 0.5e-6);
  return result;
}

static int GeodSolve78() {
  /* An example where the NGS calculator fails to converge */
  double azi1, azi2, s12;
  struct geod_geodesic g;
  int result = 0;
  geod_init(&g, wgs84_a, wgs84_f);
  geod_inverse(&g, 27.2, 0.0, -27.1, 179.5, &s12, &azi1, &azi2);
  result += checkEquals(azi1,  45.82468716758, 0.5e-11);
  result += checkEquals(azi2, 134.22776532670, 0.5e-11);
  result += checkEquals(s12,  19974354.765767, 0.5e-6);
  return result;
}

static int GeodSolve80() {
  /* Some tests to add code coverage: computing scale in special cases + zero
   * length geodesic (includes GeodSolve80 - GeodSolve83) + using an incapable
   * line. */
  double a12, s12, azi1, azi2, m12, M12, M21, S12;
  struct geod_geodesic g;
  struct geod_geodesicline l;
  int result = 0;
  geod_init(&g, wgs84_a, wgs84_f);

  geod_geninverse(&g, 0, 0, 0, 90, nullptr, nullptr, nullptr,
                  nullptr, &M12, &M21, nullptr);
  result += checkEquals(M12, -0.00528427534, 0.5e-10);
  result += checkEquals(M21, -0.00528427534, 0.5e-10);

  geod_geninverse(&g, 0, 0, 1e-6, 1e-6, nullptr, nullptr, nullptr,
                  nullptr, &M12, &M21, nullptr);
  result += checkEquals(M12, 1, 0.5e-10);
  result += checkEquals(M21, 1, 0.5e-10);

  a12 = geod_geninverse(&g, 20.001, 0, 20.001, 0,
                        &s12, &azi1, &azi2, &m12, &M12, &M21, &S12);
  result += checkEquals(a12, 0, 1e-13);
  result += checkEquals(s12, 0, 1e-8);
  result += checkEquals(azi1, 180, 1e-13);
  result += checkEquals(azi2, 180, 1e-13);
  result += checkEquals(m12, 0,  1e-8);
  result += checkEquals(M12, 1, 1e-15);
  result += checkEquals(M21, 1, 1e-15);
  result += checkEquals(S12, 0, 1e-10);

  a12 = geod_geninverse(&g, 90, 0, 90, 180,
                        &s12, &azi1, &azi2, &m12, &M12, &M21, &S12);
  result += checkEquals(a12, 0, 1e-13);
  result += checkEquals(s12, 0, 1e-8);
  result += checkEquals(azi1, 0, 1e-13);
  result += checkEquals(azi2, 180, 1e-13);
  result += checkEquals(m12, 0, 1e-8);
  result += checkEquals(M12, 1, 1e-15);
  result += checkEquals(M21, 1, 1e-15);
  result += checkEquals(S12, 127516405431022.0, 0.5);

  /* An incapable line which can't take distance as input */
  geod_lineinit(&l, &g, 1, 2, 90, GEOD_LATITUDE);
  a12 = geod_genposition(&l, 0, 1000, nullptr, nullptr, nullptr,
                         nullptr, nullptr, nullptr, nullptr, nullptr);
  result += checkNaN(a12);
  return result;
}

static int GeodSolve84() {
  /* Tests for python implementation to check fix for range errors with
   * {fmod,sin,cos}(inf) (includes GeodSolve84 - GeodSolve86). */

  double lat2, lon2, azi2, inf, nan;
  struct geod_geodesic g;
  int result = 0;
  geod_init(&g, wgs84_a, wgs84_f);
  {
    /* a round about way to set inf = 0 */
    geod_direct(&g, 0, 0, 90, 0, &inf, nullptr, nullptr);
    /* so that this doesn't give a compiler time error on Windows */
    inf = 1.0/inf;
  }
  {
    double minus1 = -1;
    /* cppcheck-suppress wrongmathcall */
    nan = sqrt(minus1);
  }
  geod_direct(&g, 0, 0, 90, inf, &lat2, &lon2, &azi2);
  result += checkNaN(lat2);
  result += checkNaN(lon2);
  result += checkNaN(azi2);
  geod_direct(&g, 0, 0, 90, nan, &lat2, &lon2, &azi2);
  result += checkNaN(lat2);
  result += checkNaN(lon2);
  result += checkNaN(azi2);
  geod_direct(&g, 0, 0, inf, 1000, &lat2, &lon2, &azi2);
  result += checkNaN(lat2);
  result += checkNaN(lon2);
  result += checkNaN(azi2);
  geod_direct(&g, 0, 0, nan, 1000, &lat2, &lon2, &azi2);
  result += checkNaN(lat2);
  result += checkNaN(lon2);
  result += checkNaN(azi2);
  geod_direct(&g, 0, inf, 90, 1000, &lat2, &lon2, &azi2);
  result += lat2 == 0 ? 0 : 1;
  result += checkNaN(lon2);
  result += azi2 == 90 ? 0 : 1;
  geod_direct(&g, 0, nan, 90, 1000, &lat2, &lon2, &azi2);
  result += lat2 == 0 ? 0 : 1;
  result += checkNaN(lon2);
  result += azi2 == 90 ? 0 : 1;
  geod_direct(&g, inf, 0, 90, 1000, &lat2, &lon2, &azi2);
  result += checkNaN(lat2);
  result += checkNaN(lon2);
  result += checkNaN(azi2);
  geod_direct(&g, nan, 0, 90, 1000, &lat2, &lon2, &azi2);
  result += checkNaN(lat2);
  result += checkNaN(lon2);
  result += checkNaN(azi2);
  return result;
}

static int Planimeter0() {
  /* Check fix for pole-encircling bug found 2011-03-16 */
  double pa[4][2] = {{89, 0}, {89, 90}, {89, 180}, {89, 270}};
  double pb[4][2] = {{-89, 0}, {-89, 90}, {-89, 180}, {-89, 270}};
  double pc[4][2] = {{0, -1}, {-1, 0}, {0, 1}, {1, 0}};
  double pd[3][2] = {{90, 0}, {0, 0}, {0, 90}};
  struct geod_geodesic g;
  double perimeter, area;
  int result = 0;
  geod_init(&g, wgs84_a, wgs84_f);

  planimeter(&g, pa, 4, &perimeter, &area);
  result += checkEquals(perimeter, 631819.8745, 1e-4);
  result += checkEquals(area, 24952305678.0, 1);

  planimeter(&g, pb, 4, &perimeter, &area);
  result += checkEquals(perimeter, 631819.8745, 1e-4);
  result += checkEquals(area, -24952305678.0, 1);

  planimeter(&g, pc, 4, &perimeter, &area);
  result += checkEquals(perimeter, 627598.2731, 1e-4);
  result += checkEquals(area, 24619419146.0, 1);

  planimeter(&g, pd, 3, &perimeter, &area);
  result += checkEquals(perimeter, 30022685, 1);
  result += checkEquals(area, 63758202715511.0, 1);

  polylength(&g, pd, 3, &perimeter);
  result += checkEquals(perimeter, 20020719, 1);

  return result;
}

static int Planimeter5() {
  /* Check fix for Planimeter pole crossing bug found 2011-06-24 */
  double points[3][2] = {{89, 0.1}, {89, 90.1}, {89, -179.9}};
  struct geod_geodesic g;
  double perimeter, area;
  int result = 0;
  geod_init(&g, wgs84_a, wgs84_f);
  planimeter(&g, points, 3, &perimeter, &area);
  result += checkEquals(perimeter, 539297, 1);
  result += checkEquals(area, 12476152838.5, 1);
  return result;
}

static int Planimeter6() {
  /* Check fix for Planimeter lon12 rounding bug found 2012-12-03 */
  double pa[3][2] = {{9, -0.00000000000001}, {9, 180}, {9, 0}};
  double pb[3][2] = {{9, 0.00000000000001}, {9, 0}, {9, 180}};
  double pc[3][2] = {{9, 0.00000000000001}, {9, 180}, {9, 0}};
  double pd[3][2] = {{9, -0.00000000000001}, {9, 0}, {9, 180}};
  struct geod_geodesic g;
  double perimeter, area;
  int result = 0;
  geod_init(&g, wgs84_a, wgs84_f);

  planimeter(&g, pa, 3, &perimeter, &area);
  result += checkEquals(perimeter, 36026861, 1);
  result += checkEquals(area, 0, 1);
  planimeter(&g, pb, 3, &perimeter, &area);
  result += checkEquals(perimeter, 36026861, 1);
  result += checkEquals(area, 0, 1);
  planimeter(&g, pc, 3, &perimeter, &area);
  result += checkEquals(perimeter, 36026861, 1);
  result += checkEquals(area, 0, 1);
  planimeter(&g, pd, 3, &perimeter, &area);
  result += checkEquals(perimeter, 36026861, 1);
  result += checkEquals(area, 0, 1);
  return result;
}

static int Planimeter12() {
  /* Area of arctic circle (not really -- adjunct to rhumb-area test) */
  double points[2][2] = {{66.562222222, 0}, {66.562222222, 180}};
  struct geod_geodesic g;
  double perimeter, area;
  int result = 0;
  geod_init(&g, wgs84_a, wgs84_f);
  planimeter(&g, points, 2, &perimeter, &area);
  result += checkEquals(perimeter, 10465729, 1);
  result += checkEquals(area, 0, 1);
  return result;
}

static int Planimeter13() {
  /* Check encircling pole twice */
  double points[6][2] = {{89,-360}, {89,-240}, {89,-120},
                         {89,0}, {89,120}, {89,240}};
  struct geod_geodesic g;
  double perimeter, area;
  int result = 0;
  geod_init(&g, wgs84_a, wgs84_f);
  planimeter(&g, points, 6, &perimeter, &area);
  result += checkEquals(perimeter, 1160741, 1);
  result += checkEquals(area, 32415230256.0, 1);
  return result;
}

static int Planimeter15() {
  /* Coverage tests, includes Planimeter15 - Planimeter18 (combinations of
   * reverse and sign) + calls to testpoint, testedge, geod_polygonarea. */
  struct geod_geodesic g;
  struct geod_polygon p;
  double lat[] = {2, 1, 3}, lon[] = {1, 2, 3};
  double area, s12, azi1;
  double r = 18454562325.45119,
    a0 = 510065621724088.5093;  /* ellipsoid area */
  int result = 0;
  geod_init(&g, wgs84_a, wgs84_f);
  geod_polygon_init(&p, 0);
  geod_polygon_addpoint(&g, &p, lat[0], lon[0]);
  geod_polygon_addpoint(&g, &p, lat[1], lon[1]);
  geod_polygon_testpoint(&g, &p, lat[2], lon[2], 0, 1, &area, nullptr);
  result += checkEquals(area, r, 0.5);
  geod_polygon_testpoint(&g, &p, lat[2], lon[2], 0, 0, &area, nullptr);
  result += checkEquals(area, r, 0.5);
  geod_polygon_testpoint(&g, &p, lat[2], lon[2], 1, 1, &area, nullptr);
  result += checkEquals(area, -r, 0.5);
  geod_polygon_testpoint(&g, &p, lat[2], lon[2], 1, 0, &area, nullptr);
  result += checkEquals(area, a0-r, 0.5);
  geod_inverse(&g, lat[1], lon[1], lat[2], lon[2], &s12, &azi1, nullptr);
  geod_polygon_testedge(&g, &p, azi1, s12, 0, 1, &area, nullptr);
  result += checkEquals(area, r, 0.5);
  geod_polygon_testedge(&g, &p, azi1, s12, 0, 0, &area, nullptr);
  result += checkEquals(area, r, 0.5);
  geod_polygon_testedge(&g, &p, azi1, s12, 1, 1, &area, nullptr);
  result += checkEquals(area, -r, 0.5);
  geod_polygon_testedge(&g, &p, azi1, s12, 1, 0, &area, nullptr);
  result += checkEquals(area, a0-r, 0.5);
  geod_polygon_addpoint(&g, &p, lat[2], lon[2]);
  geod_polygon_compute(&g, &p, 0, 1, &area, nullptr);
  result += checkEquals(area, r, 0.5);
  geod_polygon_compute(&g, &p, 0, 0, &area, nullptr);
  result += checkEquals(area, r, 0.5);
  geod_polygon_compute(&g, &p, 1, 1, &area, nullptr);
  result += checkEquals(area, -r, 0.5);
  geod_polygon_compute(&g, &p, 1, 0, &area, nullptr);
  result += checkEquals(area, a0-r, 0.5);
  geod_polygonarea(&g, lat, lon, 3, &area, nullptr);
  result += checkEquals(area, r, 0.5);
  return result;
}

static int Planimeter19() {
  /* Coverage tests, includes Planimeter19 - Planimeter20 (degenerate
   * polygons) + extra cases.  */
  struct geod_geodesic g;
  struct geod_polygon p;
  double area, perim;
  int result = 0;
  geod_init(&g, wgs84_a, wgs84_f);
  geod_polygon_init(&p, 0);
  geod_polygon_compute(&g, &p, 0, 1, &area, &perim);
  result += area == 0 ? 0 : 1;
  result += perim == 0 ? 0 : 1;
  geod_polygon_testpoint(&g, &p, 1, 1, 0, 1, &area, &perim);
  result += area == 0 ? 0 : 1;
  result += perim == 0 ? 0 : 1;
  geod_polygon_testedge(&g, &p, 90, 1000, 0, 1, &area, &perim);
  result += checkNaN(area);
  result += checkNaN(perim);
  geod_polygon_addpoint(&g, &p, 1, 1);
  geod_polygon_compute(&g, &p, 0, 1, &area, &perim);
  result += area == 0 ? 0 : 1;
  result += perim == 0 ? 0 : 1;
  geod_polygon_init(&p, 1);
  geod_polygon_compute(&g, &p, 0, 1, nullptr, &perim);
  result += perim == 0 ? 0 : 1;
  geod_polygon_testpoint(&g, &p, 1, 1, 0, 1, nullptr, &perim);
  result += perim == 0 ? 0 : 1;
  geod_polygon_testedge(&g, &p, 90, 1000, 0, 1, nullptr, &perim);
  result += checkNaN(perim);
  geod_polygon_addpoint(&g, &p, 1, 1);
  geod_polygon_compute(&g, &p, 0, 1, nullptr, &perim);
  result += perim == 0 ? 0 : 1;
  geod_polygon_addpoint(&g, &p, 1, 1);
  geod_polygon_testedge(&g, &p, 90, 1000, 0, 1, nullptr, &perim);
  result += checkEquals(perim, 1000, 1e-10);
  geod_polygon_testpoint(&g, &p, 2, 2, 0, 1, nullptr, &perim);
  result += checkEquals(perim, 156876.149, 0.5e-3);
  return result;
}

static int Planimeter21() {
  /* Some test to add code coverage: multiple circlings of pole (includes
   * Planimeter21 - Planimeter28) + invocations via testpoint and testedge. */
  struct geod_geodesic g;
  struct geod_polygon p;
  double area, lat = 45,
    a = 39.2144607176828184218, s = 8420705.40957178156285,
    r = 39433884866571.4277,    /* Area for one circuit */
    a0 = 510065621724088.5093;  /* Ellipsoid area */
  int result = 0, i;
  geod_init(&g, wgs84_a, wgs84_f);
  geod_polygon_init(&p, 0);
  geod_polygon_addpoint(&g, &p, lat,  60);
  geod_polygon_addpoint(&g, &p, lat, 180);
  geod_polygon_addpoint(&g, &p, lat, -60);
  geod_polygon_addpoint(&g, &p, lat,  60);
  geod_polygon_addpoint(&g, &p, lat, 180);
  geod_polygon_addpoint(&g, &p, lat, -60);
  for (i = 3; i <= 4; ++i) {
    geod_polygon_addpoint(&g, &p, lat,  60);
    geod_polygon_addpoint(&g, &p, lat, 180);
    geod_polygon_testpoint(&g, &p, lat, -60, 0, 1, &area, nullptr);
    result += checkEquals(area,  i*r, 0.5);
    geod_polygon_testpoint(&g, &p, lat, -60, 0, 0, &area, nullptr);
    result += checkEquals(area,  i*r, 0.5);
    geod_polygon_testpoint(&g, &p, lat, -60, 1, 1, &area, nullptr);
    result += checkEquals(area, -i*r, 0.5);
    geod_polygon_testpoint(&g, &p, lat, -60, 1, 0, &area, nullptr);
    result += checkEquals(area, -i*r + a0, 0.5);
    geod_polygon_testedge(&g, &p, a, s, 0, 1, &area, nullptr);
    result += checkEquals(area,  i*r, 0.5);
    geod_polygon_testedge(&g, &p, a, s, 0, 0, &area, nullptr);
    result += checkEquals(area,  i*r, 0.5);
    geod_polygon_testedge(&g, &p, a, s, 1, 1, &area, nullptr);
    result += checkEquals(area, -i*r, 0.5);
    geod_polygon_testedge(&g, &p, a, s, 1, 0, &area, nullptr);
    result += checkEquals(area, -i*r + a0, 0.5);
    geod_polygon_addpoint(&g, &p, lat, -60);
    geod_polygon_compute(&g, &p, 0, 1, &area, nullptr);
    result += checkEquals(area,  i*r, 0.5);
    geod_polygon_compute(&g, &p, 0, 0, &area, nullptr);
    result += checkEquals(area,  i*r, 0.5);
    geod_polygon_compute(&g, &p, 1, 1, &area, nullptr);
    result += checkEquals(area, -i*r, 0.5);
    geod_polygon_compute(&g, &p, 1, 0, &area, nullptr);
    result += checkEquals(area, -i*r + a0, 0.5);
  }
  return result;
}

static int Planimeter29() {
  /* Check fix to transitdirect vs transit zero handling inconsistency */
  struct geod_geodesic g;
  struct geod_polygon p;
  double area;
  int result = 0;
  geod_init(&g, wgs84_a, wgs84_f);
  geod_polygon_init(&p, 0);
  geod_polygon_addpoint(&g, &p, 0, 0);
  geod_polygon_addedge(&g, &p,  90, 1000);
  geod_polygon_addedge(&g, &p,   0, 1000);
  geod_polygon_addedge(&g, &p, -90, 1000);
  geod_polygon_compute(&g, &p, 0, 1, &area, nullptr);
  /* The area should be 1e6.  Prior to the fix it was 1e6 - A/2, where
   * A = ellipsoid area. */
  result += checkEquals(area, 1000000.0, 0.01);
  return result;
}

int main() {
  int n = 0, i;
  if ((i = testinverse())) {++n; printf("testinverse fail: %d\n", i);}
  if ((i = testdirect())) {++n; printf("testdirect fail: %d\n", i);}
  if ((i = testarcdirect())) {++n; printf("testarcdirect fail: %d\n", i);}
  if ((i = GeodSolve0())) {++n; printf("GeodSolve0 fail: %d\n", i);}
  if ((i = GeodSolve1())) {++n; printf("GeodSolve1 fail: %d\n", i);}
  if ((i = GeodSolve2())) {++n; printf("GeodSolve2 fail: %d\n", i);}
  if ((i = GeodSolve4())) {++n; printf("GeodSolve4 fail: %d\n", i);}
  if ((i = GeodSolve5())) {++n; printf("GeodSolve5 fail: %d\n", i);}
  if ((i = GeodSolve6())) {++n; printf("GeodSolve6 fail: %d\n", i);}
  if ((i = GeodSolve9())) {++n; printf("GeodSolve9 fail: %d\n", i);}
  if ((i = GeodSolve10())) {++n; printf("GeodSolve10 fail: %d\n", i);}
  if ((i = GeodSolve11())) {++n; printf("GeodSolve11 fail: %d\n", i);}
  if ((i = GeodSolve12())) {++n; printf("GeodSolve12 fail: %d\n", i);}
  if ((i = GeodSolve14())) {++n; printf("GeodSolve14 fail: %d\n", i);}
  if ((i = GeodSolve15())) {++n; printf("GeodSolve15 fail: %d\n", i);}
  if ((i = GeodSolve17())) {++n; printf("GeodSolve17 fail: %d\n", i);}
  if ((i = GeodSolve26())) {++n; printf("GeodSolve26 fail: %d\n", i);}
  if ((i = GeodSolve28())) {++n; printf("GeodSolve28 fail: %d\n", i);}
  if ((i = GeodSolve33())) {++n; printf("GeodSolve33 fail: %d\n", i);}
  if ((i = GeodSolve55())) {++n; printf("GeodSolve55 fail: %d\n", i);}
  if ((i = GeodSolve59())) {++n; printf("GeodSolve59 fail: %d\n", i);}
  if ((i = GeodSolve61())) {++n; printf("GeodSolve61 fail: %d\n", i);}
  if ((i = GeodSolve65())) {++n; printf("GeodSolve65 fail: %d\n", i);}
  if ((i = GeodSolve67())) {++n; printf("GeodSolve67 fail: %d\n", i);}
  if ((i = GeodSolve71())) {++n; printf("GeodSolve71 fail: %d\n", i);}
  if ((i = GeodSolve73())) {++n; printf("GeodSolve73 fail: %d\n", i);}
  if ((i = GeodSolve74())) {++n; printf("GeodSolve74 fail: %d\n", i);}
  if ((i = GeodSolve76())) {++n; printf("GeodSolve76 fail: %d\n", i);}
  if ((i = GeodSolve78())) {++n; printf("GeodSolve78 fail: %d\n", i);}
  if ((i = GeodSolve80())) {++n; printf("GeodSolve80 fail: %d\n", i);}
  if ((i = GeodSolve84())) {++n; printf("GeodSolve84 fail: %d\n", i);}
  if ((i = Planimeter0())) {++n; printf("Planimeter0 fail: %d\n", i);}
  if ((i = Planimeter5())) {++n; printf("Planimeter5 fail: %d\n", i);}
  if ((i = Planimeter6())) {++n; printf("Planimeter6 fail: %d\n", i);}
  if ((i = Planimeter12())) {++n; printf("Planimeter12 fail: %d\n", i);}
  if ((i = Planimeter13())) {++n; printf("Planimeter13 fail: %d\n", i);}
  if ((i = Planimeter15())) {++n; printf("Planimeter15 fail: %d\n", i);}
  if ((i = Planimeter19())) {++n; printf("Planimeter19 fail: %d\n", i);}
  if ((i = Planimeter21())) {++n; printf("Planimeter21 fail: %d\n", i);}
  if ((i = Planimeter29())) {++n; printf("Planimeter29 fail: %d\n", i);}
  return n;
}

/** @endcond */
