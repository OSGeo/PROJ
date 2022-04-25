/**
 * \file geodsigntest.c
 * \brief Test treatment of +/-0 and +/-180
 *
 * Copyright (c) Charles Karney (2022) <charles@karney.com> and licensed
 * under the MIT/X11 License.  For more information, see
 * https://geographiclib.sourceforge.io/
 **********************************************************************/

#include <stdio.h>
#include <math.h>
#include <float.h>

/* Include the source file for the library directly so we can access the
 * internal (static) functions. */
#include "geodesic.c"

/* Define function names with the "geod_" prefix. */
#define geod_Init         Init
#define geod_sum          sumx
#define geod_AngNormalize AngNormalize
#define geod_AngDiff      AngDiff
#define geod_AngRound     AngRound
#define geod_sincosd      sincosdx
#define geod_atan2d       atan2dx

typedef double T;

#if !defined(__cplusplus)
#define nullptr 0
#endif

static const T wgs84_a = 6378137, wgs84_f = 1/298.257223563; /* WGS84 */

static int equiv(T x, T y) {
  return ( (isnan(x) && isnan(y)) || (x == y && signbit(x) == signbit(y)) ) ?
    0 : 1;
}

static int checkEquals(T x, T y, T d) {
  if (fabs(x - y) <= d)
    return 0;
  printf("checkEquals fails: %.7g != %.7g +/- %.7g\n", x, y, d);
  return 1;
}

/* use "do { } while (false)" idiom so it can be punctuated like a
 * statement. */

#define check(expr, r) do {             \
    T s = (T)(r),  t = expr;            \
    if (equiv(s, t)) {                  \
    printf("Line %d : %s != %s (%g)\n", \
           __LINE__, #expr, #r, t);     \
      ++n;                              \
    }                                   \
  } while (0)

#define checksincosd(x, s, c) do {            \
    T sx, cx;                                 \
    geod_sincosd(x, &sx, &cx);                \
    if (equiv(s, sx)) {                       \
      printf("Line %d: sin(%g) != %g (%g)\n", \
             __LINE__, x, s, sx);             \
      ++n;                                    \
    }                                         \
    if (equiv(c, cx)) {                       \
      printf("Line %d: cos(%g) != %g (%g)\n", \
             __LINE__, x, c, cx);             \
      ++n;                                    \
    }                                         \
  } while (0)

int main() {
  T inf = INFINITY,
    nan = NAN,
    eps = DBL_EPSILON,
    e;
  int n = 0;
  geod_Init();

  check( geod_AngRound(-eps/32), -eps/32);
  check( geod_AngRound(-eps/64), -0.0   );
  check( geod_AngRound(-  0.0 ), -0.0   );
  check( geod_AngRound(   0.0 ), +0.0   );
  check( geod_AngRound( eps/64), +0.0   );
  check( geod_AngRound( eps/32), +eps/32);
  check( geod_AngRound((1-2*eps)/64), (1-2*eps)/64);
  check( geod_AngRound((1-eps  )/64),  1.0     /64);
  check( geod_AngRound((1-eps/2)/64),  1.0     /64);
  check( geod_AngRound((1-eps/4)/64),  1.0     /64);
  check( geod_AngRound( 1.0     /64),  1.0     /64);
  check( geod_AngRound((1+eps/2)/64),  1.0     /64);
  check( geod_AngRound((1+eps  )/64),  1.0     /64);
  check( geod_AngRound((1+2*eps)/64), (1+2*eps)/64);
  check( geod_AngRound((1-eps  )/32), (1-eps  )/32);
  check( geod_AngRound((1-eps/2)/32),  1.0     /32);
  check( geod_AngRound((1-eps/4)/32),  1.0     /32);
  check( geod_AngRound( 1.0     /32),  1.0     /32);
  check( geod_AngRound((1+eps/2)/32),  1.0     /32);
  check( geod_AngRound((1+eps  )/32), (1+eps  )/32);
  check( geod_AngRound((1-eps  )/16), (1-eps  )/16);
  check( geod_AngRound((1-eps/2)/16), (1-eps/2)/16);
  check( geod_AngRound((1-eps/4)/16),  1.0     /16);
  check( geod_AngRound( 1.0     /16),  1.0     /16);
  check( geod_AngRound((1+eps/4)/16),  1.0     /16);
  check( geod_AngRound((1+eps/2)/16),  1.0     /16);
  check( geod_AngRound((1+eps  )/16), (1+eps  )/16);
  check( geod_AngRound((1-eps  )/ 8), (1-eps  )/ 8);
  check( geod_AngRound((1-eps/2)/ 8), (1-eps/2)/ 8);
  check( geod_AngRound((1-eps/4)/ 8),  1.0     / 8);
  check( geod_AngRound((1+eps/2)/ 8),  1.0     / 8);
  check( geod_AngRound((1+eps  )/ 8), (1+eps  )/ 8);
  check( geod_AngRound( 1-eps      ),  1-eps      );
  check( geod_AngRound( 1-eps/2    ),  1-eps/2    );
  check( geod_AngRound( 1-eps/4    ),  1          );
  check( geod_AngRound( 1.0        ),  1          );
  check( geod_AngRound( 1+eps/4    ),  1          );
  check( geod_AngRound( 1+eps/2    ),  1          );
  check( geod_AngRound( 1+eps      ),  1+  eps    );
  check( geod_AngRound( 90.0-64*eps),  90-64*eps  );
  check( geod_AngRound( 90.0-32*eps),  90         );
  check( geod_AngRound( 90.0       ),  90         );

  checksincosd(-  inf,  nan,  nan);
  checksincosd(-810.0, -1.0, +0.0);
  checksincosd(-720.0, -0.0, +1.0);
  checksincosd(-630.0, +1.0, +0.0);
  checksincosd(-540.0, -0.0, -1.0);
  checksincosd(-450.0, -1.0, +0.0);
  checksincosd(-360.0, -0.0, +1.0);
  checksincosd(-270.0, +1.0, +0.0);
  checksincosd(-180.0, -0.0, -1.0);
  checksincosd(- 90.0, -1.0, +0.0);
  checksincosd(-  0.0, -0.0, +1.0);
  checksincosd(+  0.0, +0.0, +1.0);
  checksincosd(+ 90.0, +1.0, +0.0);
  checksincosd(+180.0, +0.0, -1.0);
  checksincosd(+270.0, -1.0, +0.0);
  checksincosd(+360.0, +0.0, +1.0);
  checksincosd(+450.0, +1.0, +0.0);
  checksincosd(+540.0, +0.0, -1.0);
  checksincosd(+630.0, -1.0, +0.0);
  checksincosd(+720.0, +0.0, +1.0);
  checksincosd(+810.0, +1.0, +0.0);
  checksincosd(+  inf,  nan,  nan);
  checksincosd(   nan,  nan,  nan);

  {
    T s1, c1, s2, c2, s3, c3;
    geod_sincosd(         9.0, &s1, &c1);
    geod_sincosd(        81.0, &s2, &c2);
    geod_sincosd(-123456789.0, &s3, &c3);
    if ( equiv(s1, c2) + equiv(s1, s3) + equiv(c1, s2) + equiv(c1, -c3) ) {
      printf("Line %d : sincos accuracy fail\n", __LINE__);
      ++n;
    }
  }

  check( geod_atan2d(+0.0 , -0.0 ), +180 );
  check( geod_atan2d(-0.0 , -0.0 ), -180 );
  check( geod_atan2d(+0.0 , +0.0 ), +0.0 );
  check( geod_atan2d(-0.0 , +0.0 ), -0.0 );
  check( geod_atan2d(+0.0 , -1.0 ), +180 );
  check( geod_atan2d(-0.0 , -1.0 ), -180 );
  check( geod_atan2d(+0.0 , +1.0 ), +0.0 );
  check( geod_atan2d(-0.0 , +1.0 ), -0.0 );
  check( geod_atan2d(-1.0 , +0.0 ),  -90 );
  check( geod_atan2d(-1.0 , -0.0 ),  -90 );
  check( geod_atan2d(+1.0 , +0.0 ),  +90 );
  check( geod_atan2d(+1.0 , -0.0 ),  +90 );
  check( geod_atan2d(+1.0 ,  -inf), +180 );
  check( geod_atan2d(-1.0 ,  -inf), -180 );
  check( geod_atan2d(+1.0 ,  +inf), +0.0 );
  check( geod_atan2d(-1.0 ,  +inf), -0.0 );
  check( geod_atan2d( +inf, +1.0 ),  +90 );
  check( geod_atan2d( +inf, -1.0 ),  +90 );
  check( geod_atan2d( -inf, +1.0 ),  -90 );
  check( geod_atan2d( -inf, -1.0 ),  -90 );
  check( geod_atan2d( +inf,  -inf), +135 );
  check( geod_atan2d( -inf,  -inf), -135 );
  check( geod_atan2d( +inf,  +inf),  +45 );
  check( geod_atan2d( -inf,  +inf),  -45 );
  check( geod_atan2d(  nan, +1.0 ),  nan );
  check( geod_atan2d(+1.0 ,   nan),  nan );

  {
    T s = 7e-16;
    if ( equiv( geod_atan2d(s, -1.0), 180 - geod_atan2d(s, 1.0) ) ) {
      printf("Line %d : atan2d accuracy fail\n", __LINE__);
      ++n;
    }
  }

  check( geod_sum(+9.0, -9.0, &e), +0.0 );
  check( geod_sum(-9.0, +9.0, &e), +0.0 );
  check( geod_sum(-0.0, +0.0, &e), +0.0 );
  check( geod_sum(+0.0, -0.0, &e), +0.0 );
  check( geod_sum(-0.0, -0.0, &e), -0.0 );
  check( geod_sum(+0.0, +0.0, &e), +0.0 );

  check( geod_AngNormalize(-900.0), -180 );
  check( geod_AngNormalize(-720.0), -0.0 );
  check( geod_AngNormalize(-540.0), -180 );
  check( geod_AngNormalize(-360.0), -0.0 );
  check( geod_AngNormalize(-180.0), -180 );
  check( geod_AngNormalize(  -0.0), -0.0 );
  check( geod_AngNormalize(  +0.0), +0.0 );
  check( geod_AngNormalize( 180.0), +180 );
  check( geod_AngNormalize( 360.0), +0.0 );
  check( geod_AngNormalize( 540.0), +180 );
  check( geod_AngNormalize( 720.0), +0.0 );
  check( geod_AngNormalize( 900.0), +180 );

  check( geod_AngDiff(+  0.0, +  0.0, &e), +0.0 );
  check( geod_AngDiff(+  0.0, -  0.0, &e), -0.0 );
  check( geod_AngDiff(-  0.0, +  0.0, &e), +0.0 );
  check( geod_AngDiff(-  0.0, -  0.0, &e), +0.0 );
  check( geod_AngDiff(+  5.0, +365.0, &e), +0.0 );
  check( geod_AngDiff(+365.0, +  5.0, &e), -0.0 );
  check( geod_AngDiff(+  5.0, +185.0, &e), +180.0 );
  check( geod_AngDiff(+185.0, +  5.0, &e), -180.0 );
  check( geod_AngDiff( +eps , +180.0, &e), +180.0 );
  check( geod_AngDiff( -eps , +180.0, &e), -180.0 );
  check( geod_AngDiff( +eps , -180.0, &e), +180.0 );
  check( geod_AngDiff( -eps , -180.0, &e), -180.0 );

  {
    T x = 138 + 128 * eps, y = -164;
    if ( equiv( geod_AngDiff(x, y, &e), 58 - 128 * eps ) ) {
      printf("Line %d : AngDiff accuracy fail\n", __LINE__);
      ++n;
    }
  }

  {
    /* azimuth of geodesic line with points on equator determined by signs of
     * latitude
     * lat1 lat2 azi1/2 */
    T C[2][3] = {
      { +0.0, -0.0, 180 },
      { -0.0, +0.0,   0 }
    };
    struct geod_geodesic g;
    geod_init(&g, wgs84_a, wgs84_f);
    T azi1, azi2;
    int i = 0;
    for (int k = 0; k < 2; ++k) {
      geod_inverse(&g, C[k][0], 0.0, C[k][1], 0.0, nullptr, &azi1, &azi2);
      if ( equiv(azi1, C[k][2]) + equiv(azi2, C[k][2]) ) ++i;
    }
    if (i) {
      printf("Line %d: inverse coincident points on equator fail\n", __LINE__);
      ++n;
    }
  }

  {
    /* Does the nearly antipodal equatorial solution go north or south?
     * lat1 lat2 azi1 azi2 */
    T C[2][4] = {
      { +0.0, +0.0,  56, 124},
      { -0.0, -0.0, 124,  56}
    };
    struct geod_geodesic g;
    geod_init(&g, wgs84_a, wgs84_f);
    T azi1, azi2;
    int i = 0;
    for (int k = 0; k < 2; ++k) {
      geod_inverse(&g, C[k][0], 0.0, C[k][1], 179.5, nullptr, &azi1, &azi2);
      i += checkEquals(azi1, C[k][2], 1) + checkEquals(azi2, C[k][3], 1);
    }
    if (i) {
      printf("Line %d: inverse nearly antipodal points on equator fail\n",
             __LINE__);;
      ++n;
    }
  }

  {
    /* How does the exact antipodal equatorial path go N/S + E/W
     * lat1 lat2 lon2 azi1 azi2 */
    T C[4][5] = {
      { +0.0, +0.0, +180,   +0.0, +180},
      { -0.0, -0.0, +180, +180,   +0.0},
      { +0.0, +0.0, -180,   -0.0, -180},
      { -0.0, -0.0, -180, -180,   -0.0}
    };
    struct geod_geodesic g;
    geod_init(&g, wgs84_a, wgs84_f);
    T azi1, azi2;
    int i = 0;
    for (int k = 0; k < 4; ++k) {
      geod_inverse(&g, C[k][0], 0.0, C[k][1], C[k][2], nullptr, &azi1, &azi2);
      if ( equiv(azi1, C[k][3]) + equiv(azi2, C[k][4]) ) ++i;
    }
    if (i) {
      printf("Line %d: inverse antipodal points on equator fail\n",
             __LINE__);
      ++n;
    }
  }

  {
    /* Antipodal points on the equator with prolate ellipsoid
     * lon2 azi1/2 */
    T C[2][2] = {
      { +180, +90 },
      { -180, -90 }
    };
    struct geod_geodesic g;
    geod_init(&g, 6.4e6, -1/300.0);
    T azi1, azi2;
    int i = 0;
    for (int k = 0; k < 2; ++k) {
      geod_inverse(&g, 0.0, 0.0, 0.0, C[k][0], nullptr, &azi1, &azi2);
      if ( equiv(azi1, C[k][1]) + equiv(azi2, C[k][1]) ) ++i;
    }
    if (i) {
      printf("Line %d: inverse antipodal points on equator, prolate, fail\n",
             __LINE__);
      ++n;
    }
  }

  {
    /* azimuths = +/-0 and +/-180 for the direct problem
     * azi1, lon2, azi2 */
    T C[4][3] = {
      { +0.0, +180, +180  },
      { -0.0, -180, -180  },
      { +180 , +180, +0.0 },
      { -180 , -180, -0.0 }
    };
    struct geod_geodesic g;
    geod_init(&g, wgs84_a, wgs84_f);
    T lon2, azi2;
    int i = 0;
    for (int k = 0; k < 4; ++k) {
      geod_gendirect(&g, 0.0, 0.0, C[k][0], GEOD_LONG_UNROLL, 15e6,
                     nullptr, &lon2, &azi2,
                     nullptr, nullptr, nullptr, nullptr, nullptr);
      if ( equiv(lon2, C[k][1]) + equiv(azi2, C[k][2]) ) ++i;
    }
    if (i) {
      printf("Line %d: direct azi1 = +/-0 +/-180, fail\n", __LINE__);
      ++n;
    }
  }

  if (n) {
    printf("%d %s%s\n", n, "failure", (n > 1 ? "s" : ""));
    return 1;
  }
}
