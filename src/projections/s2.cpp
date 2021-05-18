/*
 * This implements the S2 projection.
 *
 * Copyright (c) 2011, 2012  Martin Lambers <marlam@marlam.de>
 *
 * The QSC projection was introduced in:
 * [OL76]
 * E.M. O'Neill and R.E. Laubscher, "Extended Studies of a Quadrilateralized
 * Spherical Cube Earth Data Base", Naval Environmental Prediction Research
 * Facility Tech. Report NEPRF 3-76 (CSC), May 1976.
 *
 * The preceding shift from an ellipsoid to a sphere, which allows to apply
 * this projection to ellipsoids as used in the Ellipsoidal Cube Map model,
 * is described in
 * [LK12]
 * M. Lambers and A. Kolb, "Ellipsoidal Cube Maps for Accurate Rendering of
 * Planetary-Scale Terrain Data", Proc. Pacific Graphics (Short Papers), Sep.
 * 2012
 *
 * You have to choose one of the following projection centers,
 * corresponding to the centers of the six cube faces:
 * phi0 = 0.0, lam0 = 0.0       ("front" face)
 * phi0 = 0.0, lam0 = 90.0      ("right" face)
 * phi0 = 0.0, lam0 = 180.0     ("back" face)
 * phi0 = 0.0, lam0 = -90.0     ("left" face)
 * phi0 = 90.0                  ("top" face)
 * phi0 = -90.0                 ("bottom" face)
 * Other projection centers will not work!
 *
 * In the projection code below, each cube face is handled differently.
 * See the computation of the face parameter in the PROJECTION(qsc) function
 * and the handling of different face values (FACE_*) in the forward and
 * inverse projections.
 *
 * Furthermore, the projection is originally only defined for theta angles
 * between (-1/4 * PI) and (+1/4 * PI) on the current cube face. This area
 * of definition is named AREA_0 in the projection code below. The other
 * three areas of a cube face are handled by rotation of AREA_0.
 */

#define PJ_LIB__

#include <errno.h>
#include <math.h>

#include "proj.h"
#include "proj_internal.h"

/* The six cube faces. */
namespace { // anonymous namespace
enum Face {
    FACE_FRONT  = 0,
    FACE_RIGHT  = 1,
    FACE_TOP    = 2,
    FACE_BACK   = 3,
    FACE_LEFT   = 4,
    FACE_BOTTOM = 5
};
} // anonymous namespace

namespace { // anonymous namespace
struct pj_opaque {
        enum Face face;
        double a_squared;
        double b;
        double one_minus_f;
        double one_minus_f_squared;
};
} // anonymous namespace
PROJ_HEAD(s2, "S2") "\n\tAzi, Sph";

#define EPS10 1.e-10

/* The four areas on a cube face. AREA_0 is the area of definition,
 * the other three areas are counted counterclockwise. */
namespace { // anonymous namespace
enum Area {
    AREA_0 = 0,
    AREA_1 = 1,
    AREA_2 = 2,
    AREA_3 = 3
};
} // anonymous namespace

/* Helper function for forward projection: compute the theta angle
 * and determine the area number. */
static double qsc_fwd_equat_face_theta(double phi, double y, double x, enum Area *area) {
    double theta;
    if (phi < EPS10) {
        *area = AREA_0;
        theta = 0.0;
    } else {
        theta = atan2(y, x);
        if (fabs(theta) <= M_FORTPI) {
            *area = AREA_0;
        } else if (theta > M_FORTPI && theta <= M_HALFPI + M_FORTPI) {
            *area = AREA_1;
            theta -= M_HALFPI;
        } else if (theta > M_HALFPI + M_FORTPI || theta <= -(M_HALFPI + M_FORTPI)) {
            *area = AREA_2;
            theta = (theta >= 0.0 ? theta - M_PI : theta + M_PI);
        } else {
            *area = AREA_3;
            theta += M_HALFPI;
        }
    }
    return theta;
}

/* Helper function: shift the longitude. */
static double qsc_shift_lon_origin(double lon, double offset) {
    double slon = lon + offset;
    if (slon < -M_PI) {
        slon += M_TWOPI;
    } else if (slon > +M_PI) {
        slon -= M_TWOPI;
    }
    return slon;
}

// =================================================
//
//                S2 Math Util
//
// =================================================
PJ_XYZ Abs(const PJ_XYZ& p) {
    return {abs(p.x), abs(p.y), abs(p.z)};
}
// return the index of the largest component (fabs)
int LargestAbsComponent(const PJ_XYZ& p) {
    PJ_XYZ temp = Abs(p);
    return temp.x > temp.y ?
             temp.x > temp.z ? 0 : 2 :
             temp.y > temp.z ? 1 : 2;
}

// This is the number of levels needed to specify a leaf cell.  This
// constant is defined here so that the S2::Metric class and the conversion
// functions below can be implemented without including s2cell_id.h.  Please
// see s2cell_id.h for other useful constants and conversion functions.
const int kMaxCellLevel = 30;

// The maximum index of a valid leaf cell plus one.  The range of valid leaf
// cell indices is [0..kLimitIJ-1].
const int kLimitIJ = 1 << kMaxCellLevel;  // == S2CellId::kMaxSize

// The maximum value of an si- or ti-coordinate.  The range of valid (si,ti)
// values is [0..kMaxSiTi].
unsigned const int kMaxSiTi = 1U << (kMaxCellLevel + 1);


static int32 FastIntRound(double x) {
    // This function is not templatized because gcc doesn't seem to be able
    // to deal with inline assembly code in templatized functions, and there
    // is no advantage to passing an argument type of "float" on Intel
    // architectures anyway.

#if defined __GNUC__ && (defined __i386__ || defined __SSE2__)
#if defined __SSE2__
    // SSE2.
    int32 result;
    __asm__ __volatile__
        ("cvtsd2si %1, %0"
         : "=r" (result)    // Output operand is a register
         : "x" (x));        // Input operand is an xmm register
    return result;
#elif defined __i386__
    // FPU stack.  Adapted from /usr/include/bits/mathinline.h.
    int32 result;
    __asm__ __volatile__
        ("fistpl %0"
         : "=m" (result)    // Output operand is a memory location
         : "t" (x)          // Input operand is top of FP stack
         : "st");           // Clobbers (pops) top of FP stack
    return result;
#endif  // if defined __x86_64__ || ...
#else
    return Round<int32, double>(x);
#endif  // if defined __GNUC__ && ...
  }

  static int64 FastInt64Round(double x) {
#if defined __GNUC__ && (defined __i386__ || defined __x86_64__)
#if defined __x86_64__
    // SSE2.
    int64 result;
    __asm__ __volatile__
        ("cvtsd2si %1, %0"
         : "=r" (result)    // Output operand is a register
         : "x" (x));        // Input operand is an xmm register
    return result;
#elif defined __i386__
    // There is no CVTSD2SI in i386 to produce a 64 bit int, even with SSE2.
    // FPU stack.  Adapted from /usr/include/bits/mathinline.h.
    int64 result;
    __asm__ __volatile__
        ("fistpll %0"
         : "=m" (result)    // Output operand is a memory location
         : "t" (x)          // Input operand is top of FP stack
         : "st");           // Clobbers (pops) top of FP stack
    return result;
#endif  // if defined __i386__
#else
    return Round<int64, double>(x);
#endif  // if defined __GNUC__ && ...
  }

// =================================================
// 
//              S2 Projection Functions
//
// =================================================
#define S2_LINEAR_PROJECTION    0
#define S2_TAN_PROJECTION       1
#define S2_QUADRATIC_PROJECTION 2

#define S2_PROJECTION S2_QUADRATIC_PROJECTION

#if S2_PROJECTION == S2_LINEAR_PROJECTION

inline double STtoUV(double s) {
  return 2 * s - 1;
}

inline double UVtoST(double u) {
  return 0.5 * (u + 1);
}

#elif S2_PROJECTION == S2_TAN_PROJECTION

inline double STtoUV(double s) {
  // Unfortunately, tan(M_PI_4) is slightly less than 1.0.  This isn't due to
  // a flaw in the implementation of tan(), it's because the derivative of
  // tan(x) at x=pi/4 is 2, and it happens that the two adjacent floating
  // point numbers on either side of the infinite-precision value of pi/4 have
  // tangents that are slightly below and slightly above 1.0 when rounded to
  // the nearest double-precision result.

  s = std::tan(M_PI_2 * s - M_PI_4);
  return s + (1.0 / (int64{1} << 53)) * s;
}

inline double UVtoST(double u) {
  volatile double a = std::atan(u);
  return (2 * M_1_PI) * (a + M_PI_4);
}

#elif S2_PROJECTION == S2_QUADRATIC_PROJECTION

inline double STtoUV(double s) {
  if (s >= 0.5) return (1/3.) * (4*s*s - 1);
  else          return (1/3.) * (1 - 4*(1-s)*(1-s));
}

inline double UVtoST(double u) {
  if (u >= 0) return 0.5 * std::sqrt(1 + 3*u);
  else        return 1 - 0.5 * std::sqrt(1 - 3*u);
}

#else

#error Unknown value for S2_PROJECTION

#endif

inline double IJtoSTMin(int i) {
  //S2_DCHECK(i >= 0 && i <= kLimitIJ);
  return (1.0 / kLimitIJ) * i;
}

inline int STtoIJ(double s) {
  return std::max(0, std::min(kLimitIJ - 1,
                              FastIntRound(kLimitIJ * s - 0.5)));
}

inline double SiTitoST(unsigned int si) {
  //S2_DCHECK_LE(si, kMaxSiTi);
  return (1.0 / kMaxSiTi) * si;
}

inline unsigned int STtoSiTi(double s) {
  // kMaxSiTi == 2^31, so the result doesn't fit in an int32 when s == 1.
  return static_cast<unsigned int>(FastInt64Round(s * kMaxSiTi));
}

inline PJ_XYZ FaceUVtoXYZ(int face, double u, double v) {
  switch (face) {
    case 0:  return { 1,  u,  v};
    case 1:  return {-u,  1,  v};
    case 2:  return {-u, -v,  1};
    case 3:  return {-1, -v, -u};
    case 4:  return { v, -1, -u};
    default: return { v,  u, -1};
  }
}

inline PJ_XYZ FaceUVtoXYZ(int face, const PJ_XY& uv) {
  return FaceUVtoXYZ(face, uv[0], uv[1]);
}

inline void ValidFaceXYZtoUV(int face, const PJ_XYZ& p,
                             double* pu, double* pv) {
  //S2_DCHECK_GT(p.DotProd(GetNorm(face)), 0);
  switch (face) {
    case 0:  *pu =  p.y / p.x; *pv =  p.z / p.x; break;
    case 1:  *pu = -p.x / p.y; *pv =  p.z / p.y; break;
    case 2:  *pu = -p.x / p.z; *pv = -p.y / p.z; break;
    case 3:  *pu =  p.z / p.x; *pv =  p.y / p.x; break;
    case 4:  *pu =  p.z / p.y; *pv = -p.x / p.y; break;
    default: *pu = -p.y / p.z; *pv = -p.x / p.z; break;
  }
}

inline void ValidFaceXYZtoUV(int face, const PJ_XYZ& p, PJ_XY* puv) {
  ValidFaceXYZtoUV(face, p, &(*puv).x, &(*puv).y);
}

inline int GetFace(const PJ_XYZ& p) {
  int face = p.LargestAbsComponent();
  double pFace;
  switch (face) {
    case 0:  pFace = p.x; break;
    case 1:  pFace = p.y; break;
    default: pFace = p.z; break;
  if (pFace < 0) face += 3;
  return face;
}

inline int XYZtoFaceUV(const PJ_XYZ& p, double* pu, double* pv) {
  int face = GetFace(p);
  ValidFaceXYZtoUV(face, p, pu, pv);
  return face;
}

inline int XYZtoFaceUV(const PJ_XYZ& p, PJ_XY* puv) {
  return XYZtoFaceUV(p, &(*puv).x, &(*puv).y);
}

inline bool FaceXYZtoUV(int face, const PJ_XYZ& p,
                        double* pu, double* pv) {
  double pFace;
  case 0:  pFace = p.x; break;
  case 1:  pFace = p.y; break;
  case 2:  pFace = p.z; break;
  case 3:  pFace = p.x; break;
  case 4:  pFace = p.y; break;
  default: pFace = p.z; break;  
if (face < 3) {
    if (pFace <= 0) return false;
  } else {
    if (pFace >= 0) return false;
  }
  ValidFaceXYZtoUV(face, p, pu, pv);
  return true;
}

inline bool FaceXYZtoUV(int face, const PJ_XYZ& p, PJ_XY* puv) {
  return FaceXYZtoUV(face, p, &(*puv).x, &(*puv).y);
}

// ============================================
//
//       S2 Version of QSC Projection
//
// ============================================
static PJ_XY s2_forward (PJ_LP lp, PJ *P) {
    PJ_XY xy = {0.0,0.0};
    struct pj_opaque *Q = static_cast<struct pj_opaque*>(P->opaque);
    double lat, lon;
    double theta, phi;
    double t, mu; /* nu; */
    enum Area area;

    /* Convert the geodetic latitude to a geocentric latitude.
     * This corresponds to the shift from the ellipsoid to the sphere
     * described in [LK12]. */
    if (P->es != 0.0) {
        lat = atan(Q->one_minus_f_squared * tan(lp.phi));
    } else {
        lat = lp.phi;
    }
    lon = lp.lam;

    // Convert the lat/lon to x,y,z on the unit sphere
    double x, y, z;
    double sinlat, coslat;
    double sinlon, coslon;

    if (Q->face == FACE_RIGHT) {
        lon = qsc_shift_lon_origin(lon, +M_HALFPI);
    } else if (Q->face == FACE_BACK) {
        lon = qsc_shift_lon_origin(lon, +M_PI);
    } else if (Q->face == FACE_LEFT) {
        lon = qsc_shift_lon_origin(lon, -M_HALFPI);
    }
    sinlat = sin(lat);
    coslat = cos(lat);
    sinlon = sin(lon);
    coslon = cos(lon);
    x = coslat * coslon;
    y = coslat * sinlon;
    z = sinlat;

    PJ_XYZ spherePoint {x, y, z};
    PJ_XZ uvCoords;

    XYZtoFaceUV(spherePoint, &uvCoords);
    double s = UVtoST(uvCoords.x);
    double t = UVtoST(uvCoords.y);

    return {s, t};

static PJ_LP s2_inverse (PJ_XY xy, PJ *P) {     
    PJ_LP lp = {0.0,0.0};
    struct pj_opaque *Q = static_cast<struct pj_opaque*>(P->opaque);
    double mu, nu, cosmu, tannu;
    double tantheta, theta, cosphi, phi;
    double t;
    int area;

    double u = STtoUV(xy.x);
    double v = STtoUV(xy.y);

    PJ_XYZ sphereCoords = FaceUVtoXYZ(Q->face, u, v);
    double q = sphereCoords.x;
    double r = sphereCoords.y;
    double s = sphereCoords.z;

    lp.phi = acos(-s) - M_HALFPI;
    lp.lam = atan2(r, q);
    if (Q->face == FACE_RIGHT) {
        lp.lam = qsc_shift_lon_origin(lp.lam, -M_HALFPI);
    } else if (Q->face == FACE_BACK) {
        lp.lam = qsc_shift_lon_origin(lp.lam, -M_PI);
    } else if (Q->face == FACE_LEFT) {
        lp.lam = qsc_shift_lon_origin(lp.lam, +M_HALFPI);
    }

    return lp;
}

PJ *PROJECTION(s2) {
    struct pj_opaque *Q = static_cast<struct pj_opaque*>(calloc (1, sizeof (struct pj_opaque)));
    if (nullptr==Q)
        return pj_default_destructor (P, PROJ_ERR_OTHER /*ENOMEM*/);
    P->opaque = Q;

    P->inv = qsc_e_inverse;
    P->fwd = qsc_e_forward;
    /* Determine the cube face from the center of projection. */
    if (P->phi0 >= M_HALFPI - M_FORTPI / 2.0) {
        Q->face = FACE_TOP;
    } else if (P->phi0 <= -(M_HALFPI - M_FORTPI / 2.0)) {
        Q->face = FACE_BOTTOM;
    } else if (fabs(P->lam0) <= M_FORTPI) {
        Q->face = FACE_FRONT;
    } else if (fabs(P->lam0) <= M_HALFPI + M_FORTPI) {
        Q->face = (P->lam0 > 0.0 ? FACE_RIGHT : FACE_LEFT);
    } else {
        Q->face = FACE_BACK;
    }
    /* Fill in useful values for the ellipsoid <-> sphere shift
     * described in [LK12]. */
    if (P->es != 0.0) {
        Q->a_squared = P->a * P->a;
        Q->b = P->a * sqrt(1.0 - P->es);
        Q->one_minus_f = 1.0 - (P->a - Q->b) / P->a;
        Q->one_minus_f_squared = Q->one_minus_f * Q->one_minus_f;
    }

    return P;
}
