/*
*                   Transverse Mercator implementations
*
* In this file two transverse mercator implementations are found. One of Gerald
* Evenden/John Snyder origin and one of Knud Poder/Karsten Engsager origin. The
* former is regarded as "approximate" in the following and the latter is "exact".
* This word choice has been made to distinguish between the two algorithms, where
* the Evenden/Snyder implementation is the faster, less accurate implementation
* and the Poder/Engsager algorithm is a slightly slower, but more accurate
* implementation.
*/

#define PJ_LIB__

#include <errno.h>
#include <math.h>

#include "proj.h"
#include "proj_internal.h"
#include <math.h>


PROJ_HEAD(tmerc, "Transverse Mercator") "\n\tCyl, Sph&Ell\n\tapprox";
PROJ_HEAD(etmerc, "Extended Transverse Mercator") "\n\tCyl, Sph";
PROJ_HEAD(utm, "Universal Transverse Mercator (UTM)") "\n\tCyl, Ell\n\tzone= south approx";

namespace { // anonymous namespace
struct pj_opaque_approx {
    double  esp;
    double  ml0;
    double  *en;
};

struct pj_opaque_exact {
    double    Qn;     /* Merid. quad., scaled to the projection */
    double    Zb;     /* Radius vector in polar coord. systems  */
    double    cgb[6]; /* Constants for Gauss -> Geo lat */
    double    cbg[6]; /* Constants for Geo lat -> Gauss */
    double    utg[6]; /* Constants for transv. merc. -> geo */
    double    gtu[6]; /* Constants for geo -> transv. merc. */
};

} // anonymous namespace

/* Constants for "approximate" transverse mercator */
#define EPS10   1.e-10
#define FC1 1.
#define FC2 .5
#define FC3 .16666666666666666666
#define FC4 .08333333333333333333
#define FC5 .05
#define FC6 .03333333333333333333
#define FC7 .02380952380952380952
#define FC8 .01785714285714285714

/* Constant for "exact" transverse mercator */
#define PROJ_ETMERC_ORDER 6

// Determine if we should try to provide optimized versions for the Fused Multiply Addition
// Intel instruction set. We use GCC 6 __attribute__((target_clones("fma","default")))
// mechanism for that, where the compiler builds a default version, and one that
// uses FMA. And at runtimes it figures out automatically which version can be used
// by the current CPU. This allows to create general purpose binaries.
#if defined(__GNUC__) && __GNUC__ >= 6 && defined(__x86_64__) && !defined(__FMA__)
#define BUILD_FMA_OPTIMIZED_VERSION
#endif

/*****************************************************************************/
//
//                  Approximate Transverse Mercator functions
//
/*****************************************************************************/


inline static double inline_pj_mlfn(double phi, double sphi, double cphi, double *en) {
    cphi *= sphi;
    sphi *= sphi;
    return(en[0] * phi - cphi * (en[1] + sphi*(en[2]
            + sphi*(en[3] + sphi*en[4]))));
}

#ifdef BUILD_FMA_OPTIMIZED_VERSION
__attribute__((target_clones("fma","default")))
#endif
inline static PJ_XY approx_e_fwd_internal (PJ_LP lp, PJ *P)
{
    PJ_XY xy = {0.0, 0.0};
    struct pj_opaque_approx *Q = static_cast<struct pj_opaque_approx*>(P->opaque);
    double al, als, n, cosphi, sinphi, t;

    /*
     * Fail if our longitude is more than 90 degrees from the
     * central meridian since the results are essentially garbage.
     * Is error -20 really an appropriate return value?
     *
     *  http://trac.osgeo.org/proj/ticket/5
     */
    if( lp.lam < -M_HALFPI || lp.lam > M_HALFPI ) {
        xy.x = HUGE_VAL;
        xy.y = HUGE_VAL;
        pj_ctx_set_errno( P->ctx, PJD_ERR_LAT_OR_LON_EXCEED_LIMIT );
        return xy;
    }

    sinphi = sin (lp.phi);
    cosphi = cos (lp.phi);
    t = fabs (cosphi) > 1e-10 ? sinphi/cosphi : 0.;
    t *= t;
    al = cosphi * lp.lam;
    als = al * al;
    al /= sqrt (1. - P->es * sinphi * sinphi);
    n = Q->esp * cosphi * cosphi;
    xy.x = P->k0 * al * (FC1 +
        FC3 * als * (1. - t + n +
        FC5 * als * (5. + t * (t - 18.) + n * (14. - 58. * t)
        + FC7 * als * (61. + t * ( t * (179. - t) - 479. ) )
        )));
    xy.y = P->k0 * (inline_pj_mlfn(lp.phi, sinphi, cosphi, Q->en) - Q->ml0 +
        sinphi * al * lp.lam * FC2 * ( 1. +
        FC4 * als * (5. - t + n * (9. + 4. * n) +
        FC6 * als * (61. + t * (t - 58.) + n * (270. - 330 * t)
        + FC8 * als * (1385. + t * ( t * (543. - t) - 3111.) )
        ))));
    return (xy);
}

static PJ_XY approx_e_fwd (PJ_LP lp, PJ *P)
{
    return approx_e_fwd_internal(lp, P);
}

static PJ_XY approx_s_fwd (PJ_LP lp, PJ *P) {
    PJ_XY xy = {0.0,0.0};
    double b, cosphi;

    /*
     * Fail if our longitude is more than 90 degrees from the
     * central meridian since the results are essentially garbage.
     * Is error -20 really an appropriate return value?
     *
     *  http://trac.osgeo.org/proj/ticket/5
     */
    if( lp.lam < -M_HALFPI || lp.lam > M_HALFPI ) {
        xy.x = HUGE_VAL;
        xy.y = HUGE_VAL;
        pj_ctx_set_errno( P->ctx, PJD_ERR_LAT_OR_LON_EXCEED_LIMIT );
        return xy;
    }

    cosphi = cos(lp.phi);
    b = cosphi * sin (lp.lam);
    if (fabs (fabs (b) - 1.) <= EPS10) {
        proj_errno_set(P, PJD_ERR_TOLERANCE_CONDITION);
        return xy;
    }

    xy.x = static_cast<struct pj_opaque_approx*>(P->opaque)->ml0 * log ((1. + b) / (1. - b));
    xy.y = cosphi * cos (lp.lam) / sqrt (1. - b * b);

    b = fabs ( xy.y );
    if (b >= 1.) {
        if ((b - 1.) > EPS10) {
            proj_errno_set(P, PJD_ERR_TOLERANCE_CONDITION);
            return xy;
        }
        else xy.y = 0.;
    } else
        xy.y = acos (xy.y);

    if (lp.phi < 0.)
        xy.y = -xy.y;
    xy.y = static_cast<struct pj_opaque_approx*>(P->opaque)->esp * (xy.y - P->phi0);
    return xy;
}

inline static double
inline_pj_inv_mlfn(projCtx ctx, double arg, double es, double *en,
                   double* sinphi, double* cosphi) {
    double phi, k = 1./(1.-es);
    int i;
#define EPS 1e-11
#define MAX_ITER 10
    phi = arg;
    double s = sin(phi);
    double c = cos(phi);
    for (i = MAX_ITER; i ; --i) { /* rarely goes over 2 iterations */
        double t = 1. - es * s * s;
        t = (inline_pj_mlfn(phi, s, c, en) - arg) * (t * sqrt(t)) * k;
        phi -= t;
        if (fabs(t) < EPS)
        {
            // Instead of recomputing sin(phi), cos(phi) from scratch,
            // use sin(phi-t) and cos(phi-t) approximate formulas with
            // 1-term approximation of sin(t) and cos(t)
            *sinphi = s - c * t;
            *cosphi = c + s * t;
            return phi;
        }
        if (fabs(t) < 1e-3)
        {
            // 2-term approximation of sin(t) and cos(t)
            // Max relative error is 4e-14 on cos(t), and 8e-15 on sin(t)
            const double t2 = t * t;
            const double cos_t = 1 - 0.5 * t2;
            const double sin_t = t * (1 - (1. / 6) * t2);
            const double s_new = s * cos_t - c * sin_t;
            c = c * cos_t + s * sin_t;
            s = s_new;
        }
        else if (fabs(t) < 1e-2)
        {
            // 3-term approximation of sin(t) and cos(t)
            // Max relative error is 2e-15 on cos(t), and 2e-16 on sin(t)
            const double t2 = t * t;
            const double cos_t = 1 - 0.5 * t2 * (1 - (1. / 12) * t2);
            const double sin_t = t * (1 - (1. / 6) * t2 * (1 - (1. / 20) * t2));
            const double s_new = s * cos_t - c * sin_t;
            c = c * cos_t + s * sin_t;
            s = s_new;
        }
        else
        {
            s = sin(phi);
            c = cos(phi);
        }
    }
    *sinphi = s;
    *cosphi = c;
    pj_ctx_set_errno( ctx, PJD_ERR_NON_CONV_INV_MERI_DIST );
    return phi;
}

#ifdef BUILD_FMA_OPTIMIZED_VERSION
__attribute__((target_clones("fma","default")))
#endif
inline static PJ_LP approx_e_inv_internal (PJ_XY xy, PJ *P) {
    PJ_LP lp = {0.0,0.0};
    struct pj_opaque_approx *Q = static_cast<struct pj_opaque_approx*>(P->opaque);

    double sinphi, cosphi;
    lp.phi = inline_pj_inv_mlfn(P->ctx, Q->ml0 + xy.y / P->k0, P->es, Q->en, &sinphi, &cosphi);
    if (fabs(lp.phi) >= M_HALFPI) {
        lp.phi = xy.y < 0. ? -M_HALFPI : M_HALFPI;
        lp.lam = 0.;
    } else {
        double t = fabs (cosphi) > 1e-10 ? sinphi/cosphi : 0.;
        const double n = Q->esp * cosphi * cosphi;
        double con = 1. - P->es * sinphi * sinphi;
        const double d = xy.x * sqrt (con) / P->k0;
        con *= t;
        t *= t;
        const double ds = d * d;
        lp.phi -= (con * ds / (1.-P->es)) * FC2 * (1. -
            ds * FC4 * (5. + t * (3. - 9. *  n) + n * (1. - 4 * n) -
            ds * FC6 * (61. + t * (90. - 252. * n +
                45. * t) + 46. * n
           - ds * FC8 * (1385. + t * (3633. + t * (4095. + 1575. * t)) )
            )));
        lp.lam = d*(FC1 -
            ds*FC3*( 1. + 2.*t + n -
            ds*FC5*(5. + t*(28. + 24.*t + 8.*n) + 6.*n
           - ds * FC7 * (61. + t * (662. + t * (1320. + 720. * t)) )
        ))) / cosphi;
    }
    return lp;
}

static PJ_LP approx_e_inv (PJ_XY xy, PJ *P) {
    return approx_e_inv_internal(xy, P);
}

static PJ_LP approx_s_inv (PJ_XY xy, PJ *P) {
    PJ_LP lp = {0.0, 0.0};
    double h, g;

    h = exp(xy.x / static_cast<struct pj_opaque_approx*>(P->opaque)->esp);
    if( h == 0 ) {
        proj_errno_set(P, PJD_ERR_INVALID_X_OR_Y);
        return proj_coord_error().lp;
    }
    g = .5 * (h - 1. / h);
    h = cos (P->phi0 + xy.y / static_cast<struct pj_opaque_approx*>(P->opaque)->esp);
    lp.phi = asin(sqrt((1. - h * h) / (1. + g * g)));

    /* Make sure that phi is on the correct hemisphere when false northing is used */
    if (xy.y < 0. && -lp.phi+P->phi0 < 0.0) lp.phi = -lp.phi;

    lp.lam = (g != 0.0 || h != 0.0) ? atan2 (g, h) : 0.;
    return lp;
}


static PJ *destructor_approx(PJ *P, int errlev) {
    if (nullptr==P)
        return nullptr;

    if (nullptr==P->opaque)
        return pj_default_destructor(P, errlev);

    pj_dealloc (static_cast<struct pj_opaque_approx*>(P->opaque)->en);
    return pj_default_destructor(P, errlev);
}


static PJ *setup_approx(PJ *P) {
    struct pj_opaque_approx *Q = static_cast<struct pj_opaque_approx*>(P->opaque);

    P->destructor = destructor_approx;

    if (P->es != 0.0) {
        if (!(Q->en = pj_enfn(P->es)))
            return pj_default_destructor(P, ENOMEM);

        Q->ml0 = pj_mlfn(P->phi0, sin(P->phi0), cos(P->phi0), Q->en);
        Q->esp = P->es / (1. - P->es);
        P->inv = approx_e_inv;
        P->fwd = approx_e_fwd;
    } else {
        Q->esp = P->k0;
        Q->ml0 = .5 * Q->esp;
        P->inv = approx_s_inv;
        P->fwd = approx_s_fwd;
    }
    return P;
}



/*****************************************************************************/
//
//                  Exact Transverse Mercator functions
//
//
// The code in this file is largly based upon procedures:
//
// Written by: Knud Poder and Karsten Engsager
//
// Based on math from: R.Koenig and K.H. Weise, "Mathematische
// Grundlagen der hoeheren Geodaesie und Kartographie,
// Springer-Verlag, Berlin/Goettingen" Heidelberg, 1951.
//
// Modified and used here by permission of Reference Networks
// Division, Kort og Matrikelstyrelsen (KMS), Copenhagen, Denmark
//
/*****************************************************************************/

/* Helper functios for "exact" transverse mercator */
inline
static double gatg(const double *p1, int len_p1, double B, double cos_2B, double sin_2B) {
    double h = 0, h1, h2 = 0;

    const double two_cos_2B = 2*cos_2B;
    const double* p = p1 + len_p1;
    h1 = *--p;
    while (p - p1) {
        h = -h2 + two_cos_2B*h1 + *--p;
        h2 = h1;
        h1 = h;
    }
    return (B + h*sin_2B);
}

/* Complex Clenshaw summation */
inline
static double clenS(const double *a, int size,
                    double sin_arg_r, double cos_arg_r,
                    double sinh_arg_i, double cosh_arg_i,
                    double *R, double *I) {
    double      r, i, hr, hr1, hr2, hi, hi1, hi2;

    /* arguments */
    const double* p = a + size;
    r          =  2*cos_arg_r*cosh_arg_i;
    i          = -2*sin_arg_r*sinh_arg_i;

    /* summation loop */
    hi1 = hr1 = hi = 0;
    hr = *--p;
    for (; a - p;) {
        hr2 = hr1;
        hi2 = hi1;
        hr1 = hr;
        hi1 = hi;
        hr  = -hr2 + r*hr1 - i*hi1 + *--p;
        hi  = -hi2 + i*hr1 + r*hi1;
    }

    r   = sin_arg_r*cosh_arg_i;
    i   = cos_arg_r*sinh_arg_i;
    *R  = r*hr - i*hi;
    *I  = r*hi + i*hr;
    return *R;
}


/* Real Clenshaw summation */
static double clens(double *a, int size, double arg_r) {
    double      *p, r, hr, hr1, hr2, cos_arg_r;

    p = a + size;
    cos_arg_r  = cos(arg_r);
    r          =  2*cos_arg_r;

    /* summation loop */
    hr1 = 0;
    hr = *--p;
    for (; a - p;) {
        hr2 = hr1;
        hr1 = hr;
        hr  = -hr2 + r*hr1 + *--p;
    }
    return sin (arg_r)*hr;
}

/* Ellipsoidal, forward */
static PJ_XY exact_e_fwd (PJ_LP lp, PJ *P) {
    PJ_XY xy = {0.0,0.0};
    struct pj_opaque_exact *Q = static_cast<struct pj_opaque_exact*>(P->opaque);

    /* ell. LAT, LNG -> Gaussian LAT, LNG */
    double Cn  = gatg (Q->cbg, PROJ_ETMERC_ORDER, lp.phi, cos(2*lp.phi), sin(2*lp.phi));
    /* Gaussian LAT, LNG -> compl. sph. LAT */
    const double sin_Cn = sin (Cn);
    const double cos_Cn = cos (Cn);
    const double sin_Ce = sin (lp.lam);
    const double cos_Ce = cos (lp.lam);

    const double cos_Cn_cos_Ce = cos_Cn*cos_Ce;
    Cn     = atan2 (sin_Cn, cos_Cn_cos_Ce);

    const double inv_denom_tan_Ce = 1. / hypot (sin_Cn, cos_Cn_cos_Ce);
    const double tan_Ce = sin_Ce*cos_Cn * inv_denom_tan_Ce;
#if 0
    // Variant of the above: found not to be measurably faster
    const double sin_Ce_cos_Cn = sin_Ce*cos_Cn;
    const double denom = sqrt(1 - sin_Ce_cos_Cn * sin_Ce_cos_Cn);
    const double tan_Ce = sin_Ce_cos_Cn / denom;
#endif

    /* compl. sph. N, E -> ell. norm. N, E */
    double Ce  = asinh ( tan_Ce );     /* Replaces: Ce  = log(tan(FORTPI + Ce*0.5)); */

/*
 *  Non-optimized version:
 *  const double sin_arg_r  = sin(2*Cn);
 *  const double cos_arg_r  = cos(2*Cn);
 *
 *  Given:
 *      sin(2 * Cn) = 2 sin(Cn) cos(Cn)
 *          sin(atan(y)) = y / sqrt(1 + y^2)
 *          cos(atan(y)) = 1 / sqrt(1 + y^2)
 *      ==> sin(2 * Cn) = 2 tan_Cn / (1 + tan_Cn^2)
 *
 *      cos(2 * Cn) = 2cos^2(Cn) - 1
 *                  = 2 / (1 + tan_Cn^2) - 1
 */
    const double tmp_r = 2 * cos_Cn_cos_Ce * inv_denom_tan_Ce * inv_denom_tan_Ce;
    const double sin_arg_r  = sin_Cn * tmp_r;
    const double cos_arg_r  = cos_Cn_cos_Ce * tmp_r - 1;

/*
 *  Non-optimized version:
 *  const double sinh_arg_i = sinh(2*Ce);
 *  const double cosh_arg_i = cosh(2*Ce);
 *
 *  Given
 *      sinh(2 * Ce) = 2 sinh(Ce) cosh(Ce)
 *          sinh(asinh(y)) = y
 *          cosh(asinh(y)) = sqrt(1 + y^2)
 *      ==> sinh(2 * Ce) = 2 tan_Ce sqrt(1 + tan_Ce^2)
 *
 *      cosh(2 * Ce) = 2cosh^2(Ce) - 1
 *                   = 2 * (1 + tan_Ce^2) - 1
 */
    const double tmp_i = 1 + tan_Ce * tan_Ce;
    const double sinh_arg_i = 2 * tan_Ce * sqrt(tmp_i);
    const double cosh_arg_i = 2 * tmp_i - 1;

    double dCn, dCe;
    Cn += clenS (Q->gtu, PROJ_ETMERC_ORDER,
                 sin_arg_r, cos_arg_r, sinh_arg_i, cosh_arg_i,
                 &dCn, &dCe);
    Ce += dCe;
    if (fabs (Ce) <= 2.623395162778) {
        xy.y  = Q->Qn * Cn + Q->Zb;  /* Northing */
        xy.x  = Q->Qn * Ce;          /* Easting  */
    } else
        xy.x = xy.y = HUGE_VAL;
    return xy;
}


/* Ellipsoidal, inverse */
static PJ_LP exact_e_inv (PJ_XY xy, PJ *P) {
    PJ_LP lp = {0.0,0.0};
    struct pj_opaque_exact *Q = static_cast<struct pj_opaque_exact*>(P->opaque);

    /* normalize N, E */
    double Cn = (xy.y - Q->Zb)/Q->Qn;
    double Ce = xy.x/Q->Qn;

    if (fabs(Ce) <= 2.623395162778) { /* 150 degrees */
        /* norm. N, E -> compl. sph. LAT, LNG */
        const double sin_arg_r  = sin(2*Cn);
        const double cos_arg_r  = cos(2*Cn);

        //const double sinh_arg_i = sinh(2*Ce);
        //const double cosh_arg_i = cosh(2*Ce);
        const double exp_2_Ce = exp(2*Ce);
        const double half_inv_exp_2_Ce = 0.5 / exp_2_Ce;
        const double sinh_arg_i = 0.5 * exp_2_Ce - half_inv_exp_2_Ce;
        const double cosh_arg_i = 0.5 * exp_2_Ce + half_inv_exp_2_Ce;

        double dCn_ignored, dCe;
        Cn += clenS(Q->utg, PROJ_ETMERC_ORDER,
                    sin_arg_r, cos_arg_r, sinh_arg_i, cosh_arg_i,
                    &dCn_ignored, &dCe);
        Ce += dCe;

        /* compl. sph. LAT -> Gaussian LAT, LNG */
        const double sin_Cn = sin (Cn);
        const double cos_Cn = cos (Cn);

#if 0
        // Non-optimized version:
        double sin_Ce, cos_Ce;
        Ce = atan (sinh (Ce));  // Replaces: Ce = 2*(atan(exp(Ce)) - FORTPI);
        sin_Ce = sin (Ce);
        cos_Ce = cos (Ce);
        Ce     = atan2 (sin_Ce, cos_Ce*cos_Cn);
        Cn     = atan2 (sin_Cn*cos_Ce,  hypot (sin_Ce, cos_Ce*cos_Cn));
#else
/*
 *      One can divide both member of Ce = atan2(...) by cos_Ce, which gives:
 *      Ce     = atan2 (tan_Ce, cos_Cn) = atan2(sinh(Ce), cos_Cn)
 *
 *      and the same for Cn = atan2(...)
 *      Cn     = atan2 (sin_Cn, hypot (sin_Ce, cos_Ce*cos_Cn)/cos_Ce)
 *             = atan2 (sin_Cn, hypot (sin_Ce/cos_Ce, cos_Cn))
 *             = atan2 (sin_Cn, hypot (tan_Ce, cos_Cn))
 *             = atan2 (sin_Cn, hypot (sinhCe, cos_Cn))
 */
        const double sinhCe = sinh (Ce);
        Ce     = atan2 (sinhCe, cos_Cn);
        const double modulus_Ce = hypot (sinhCe, cos_Cn);
        Cn     = atan2 (sin_Cn, modulus_Ce);
#endif

        /* Gaussian LAT, LNG -> ell. LAT, LNG */

        // Optimization of the computation of cos(2*Cn) and sin(2*Cn)
        const double tmp = 2 * modulus_Ce / (sinhCe * sinhCe + 1);
        const double sin_2_Cn  = sin_Cn * tmp;
        const double cos_2_Cn  = tmp * modulus_Ce - 1.;
        //const double cos_2_Cn = cos(2 * Cn);
        //const double sin_2_Cn = sin(2 * Cn);

        lp.phi = gatg (Q->cgb,  PROJ_ETMERC_ORDER, Cn, cos_2_Cn, sin_2_Cn);
        lp.lam = Ce;
    }
    else
        lp.phi = lp.lam = HUGE_VAL;
    return lp;
}

static PJ *setup_exact(PJ *P) {
    double f, n, np, Z;
    struct pj_opaque_exact *Q = static_cast<struct pj_opaque_exact*>(P->opaque);

    if (P->es <= 0) {
        return pj_default_destructor(P, PJD_ERR_ELLIPSOID_USE_REQUIRED);
    }

    /* flattening */
    f = P->es / (1 + sqrt (1 -  P->es)); /* Replaces: f = 1 - sqrt(1-P->es); */

    /* third flattening */
    np = n = f/(2 - f);

    /* COEF. OF TRIG SERIES GEO <-> GAUSS */
    /* cgb := Gaussian -> Geodetic, KW p190 - 191 (61) - (62) */
    /* cbg := Geodetic -> Gaussian, KW p186 - 187 (51) - (52) */
    /* PROJ_ETMERC_ORDER = 6th degree : Engsager and Poder: ICC2007 */

    Q->cgb[0] = n*( 2 + n*(-2/3.0  + n*(-2      + n*(116/45.0 + n*(26/45.0 +
                n*(-2854/675.0 ))))));
    Q->cbg[0] = n*(-2 + n*( 2/3.0  + n*( 4/3.0  + n*(-82/45.0 + n*(32/45.0 +
                n*( 4642/4725.0))))));
    np     *= n;
    Q->cgb[1] = np*(7/3.0 + n*( -8/5.0  + n*(-227/45.0 + n*(2704/315.0 +
                n*( 2323/945.0)))));
    Q->cbg[1] = np*(5/3.0 + n*(-16/15.0 + n*( -13/9.0  + n*( 904/315.0 +
                n*(-1522/945.0)))));
    np     *= n;
    /* n^5 coeff corrected from 1262/105 -> -1262/105 */
    Q->cgb[2] = np*( 56/15.0  + n*(-136/35.0 + n*(-1262/105.0 +
                n*( 73814/2835.0))));
    Q->cbg[2] = np*(-26/15.0  + n*(  34/21.0 + n*(    8/5.0   +
                n*(-12686/2835.0))));
    np     *= n;
    /* n^5 coeff corrected from 322/35 -> 332/35 */
    Q->cgb[3] = np*(4279/630.0 + n*(-332/35.0 + n*(-399572/14175.0)));
    Q->cbg[3] = np*(1237/630.0 + n*( -12/5.0  + n*( -24832/14175.0)));
    np     *= n;
    Q->cgb[4] = np*(4174/315.0 + n*(-144838/6237.0 ));
    Q->cbg[4] = np*(-734/315.0 + n*( 109598/31185.0));
    np     *= n;
    Q->cgb[5] = np*(601676/22275.0 );
    Q->cbg[5] = np*(444337/155925.0);

    /* Constants of the projections */
    /* Transverse Mercator (UTM, ITM, etc) */
    np = n*n;
    /* Norm. mer. quad, K&W p.50 (96), p.19 (38b), p.5 (2) */
    Q->Qn = P->k0/(1 + n) * (1 + np*(1/4.0 + np*(1/64.0 + np/256.0)));
    /* coef of trig series */
    /* utg := ell. N, E -> sph. N, E,  KW p194 (65) */
    /* gtu := sph. N, E -> ell. N, E,  KW p196 (69) */
    Q->utg[0] = n*(-0.5  + n*( 2/3.0 + n*(-37/96.0 + n*( 1/360.0 +
                n*(  81/512.0 + n*(-96199/604800.0))))));
    Q->gtu[0] = n*( 0.5  + n*(-2/3.0 + n*(  5/16.0 + n*(41/180.0 +
                n*(-127/288.0 + n*(  7891/37800.0 ))))));
    Q->utg[1] = np*(-1/48.0 + n*(-1/15.0 + n*(437/1440.0 + n*(-46/105.0 +
                n*( 1118711/3870720.0)))));
    Q->gtu[1] = np*(13/48.0 + n*(-3/5.0  + n*(557/1440.0 + n*(281/630.0 +
                n*(-1983433/1935360.0)))));
    np      *= n;
    Q->utg[2] = np*(-17/480.0 + n*(  37/840.0 + n*(  209/4480.0  +
                n*( -5569/90720.0 ))));
    Q->gtu[2] = np*( 61/240.0 + n*(-103/140.0 + n*(15061/26880.0 +
                n*(167603/181440.0))));
    np      *= n;
    Q->utg[3] = np*(-4397/161280.0 + n*(  11/504.0 + n*( 830251/7257600.0)));
    Q->gtu[3] = np*(49561/161280.0 + n*(-179/168.0 + n*(6601661/7257600.0)));
    np     *= n;
    Q->utg[4] = np*(-4583/161280.0 + n*(  108847/3991680.0));
    Q->gtu[4] = np*(34729/80640.0  + n*(-3418889/1995840.0));
    np     *= n;
    Q->utg[5] = np*(-20648693/638668800.0);
    Q->gtu[5] = np*(212378941/319334400.0);

    /* Gaussian latitude value of the origin latitude */
    Z = gatg (Q->cbg, PROJ_ETMERC_ORDER, P->phi0, cos(2*P->phi0), sin(2*P->phi0));

    /* Origin northing minus true northing at the origin latitude */
    /* i.e. true northing = N - P->Zb                         */
    Q->Zb  = - Q->Qn*(Z + clens(Q->gtu, PROJ_ETMERC_ORDER, 2*Z));
    P->inv = exact_e_inv;
    P->fwd = exact_e_fwd;
    return P;
}




/*****************************************************************************/
//
//                                Operation Setups
//
/*****************************************************************************/

PJ *PROJECTION(tmerc) {
    /* exact transverse mercator only exists in ellipsoidal form, */
    /* use approximate version if +a sphere is requested          */
    if (pj_param (P->ctx, P->params, "bapprox").i || P->es <= 0) {
        struct pj_opaque_approx *Q = static_cast<struct pj_opaque_approx*>(pj_calloc (1, sizeof (struct pj_opaque_approx)));
        if (nullptr==Q)
            return pj_default_destructor (P, ENOMEM);

        P->opaque = Q;

        return setup_approx(P);
    } else {
        struct pj_opaque_exact *Q = static_cast<struct pj_opaque_exact*>(pj_calloc (1, sizeof (struct pj_opaque_exact)));
        if (nullptr==Q)
            return pj_default_destructor (P, ENOMEM);
        P->opaque = Q;
       return setup_exact (P);
    }
}


PJ *PROJECTION(etmerc) {
    struct pj_opaque_exact *Q = static_cast<struct pj_opaque_exact*>(pj_calloc (1, sizeof (struct pj_opaque_exact)));
    if (nullptr==Q)
        return pj_default_destructor (P, ENOMEM);
    P->opaque = Q;
   return setup_exact (P);
}


/* UTM uses the Poder/Engsager implementation for the underlying projection      */
/* UNLESS +approx is set in which case the Evenden/Snyder implemenation is used. */
PJ *PROJECTION(utm) {
    long zone;
    if (P->es == 0.0) {
        return pj_default_destructor(P, PJD_ERR_ELLIPSOID_USE_REQUIRED);
    }
    if (P->lam0 < -1000.0 || P->lam0 > 1000.0) {
        return pj_default_destructor(P, PJD_ERR_INVALID_UTM_ZONE);
    }

    P->y0 = pj_param (P->ctx, P->params, "bsouth").i ? 10000000. : 0.;
    P->x0 = 500000.;
    if (pj_param (P->ctx, P->params, "tzone").i) /* zone input ? */
    {
        zone = pj_param(P->ctx, P->params, "izone").i;
        if (zone > 0 && zone <= 60)
            --zone;
        else {
            return pj_default_destructor(P, PJD_ERR_INVALID_UTM_ZONE);
        }
    }
    else /* nearest central meridian input */
    {
        zone = lround((floor ((adjlon (P->lam0) + M_PI) * 30. / M_PI)));
        if (zone < 0)
            zone = 0;
        else if (zone >= 60)
            zone = 59;
    }
    P->lam0 = (zone + .5) * M_PI / 30. - M_PI;
    P->k0 = 0.9996;
    P->phi0 = 0.;

    if (pj_param(P->ctx, P->params, "bapprox").i) {
        struct pj_opaque_approx *Q = static_cast<struct pj_opaque_approx*>(pj_calloc (1, sizeof (struct pj_opaque_approx)));
        if (nullptr==Q)
            return pj_default_destructor (P, ENOMEM);
        P->opaque = Q;

        return setup_approx(P);
    } else {
        struct pj_opaque_exact *Q = static_cast<struct pj_opaque_exact*>(pj_calloc (1, sizeof (struct pj_opaque_exact)));
        if (nullptr==Q)
            return pj_default_destructor (P, ENOMEM);
        P->opaque = Q;

        return setup_exact(P);
    }
}
