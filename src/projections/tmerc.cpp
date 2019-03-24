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
#include "proj_math.h"


PROJ_HEAD(tmerc, "Transverse Mercator") "\n\tCyl, Sph&Ell\n\tapprox";
PROJ_HEAD(etmerc, "Extended Transverse Mercator") "\n\tCyl, Sph";
PROJ_HEAD(utm, "Universal Transverse Mercator (UTM)") "\n\tCyl, Sph\n\tzone= south approx";

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

/*****************************************************************************/
//
//                  Approximate Transverse Mercator functions
//
/*****************************************************************************/

static PJ_XY approx_e_fwd (PJ_LP lp, PJ *P) {
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
    xy.y = P->k0 * (pj_mlfn(lp.phi, sinphi, cosphi, Q->en) - Q->ml0 +
        sinphi * al * lp.lam * FC2 * ( 1. +
        FC4 * als * (5. - t + n * (9. + 4. * n) +
        FC6 * als * (61. + t * (t - 58.) + n * (270. - 330 * t)
        + FC8 * als * (1385. + t * ( t * (543. - t) - 3111.) )
        ))));
    return (xy);
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


static PJ_LP approx_e_inv (PJ_XY xy, PJ *P) {
    PJ_LP lp = {0.0,0.0};
    struct pj_opaque_approx *Q = static_cast<struct pj_opaque_approx*>(P->opaque);
    double n, con, cosphi, d, ds, sinphi, t;

    lp.phi = pj_inv_mlfn(P->ctx, Q->ml0 + xy.y / P->k0, P->es, Q->en);
    if (fabs(lp.phi) >= M_HALFPI) {
        lp.phi = xy.y < 0. ? -M_HALFPI : M_HALFPI;
        lp.lam = 0.;
    } else {
        sinphi = sin(lp.phi);
        cosphi = cos(lp.phi);
        t = fabs (cosphi) > 1e-10 ? sinphi/cosphi : 0.;
        n = Q->esp * cosphi * cosphi;
        d = xy.x * sqrt (con = 1. - P->es * sinphi * sinphi) / P->k0;
        con *= t;
        t *= t;
        ds = d * d;
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
#ifdef _GNU_SOURCE
    inline
#endif
static double gatg(double *p1, int len_p1, double B) {
    double *p;
    double h = 0, h1, h2 = 0, cos_2B;

    cos_2B = 2*cos(2*B);
    p = p1 + len_p1;
    h1 = *--p;
    while (p - p1) {
        h = -h2 + cos_2B*h1 + *--p;
        h2 = h1;
        h1 = h;
    }
    return (B + h*sin(2*B));
}

/* Complex Clenshaw summation */
#ifdef _GNU_SOURCE
    inline
#endif
static double clenS(double *a, int size, double arg_r, double arg_i, double *R, double *I) {
    double      *p, r, i, hr, hr1, hr2, hi, hi1, hi2;
    double      sin_arg_r, cos_arg_r, sinh_arg_i, cosh_arg_i;

    /* arguments */
    p = a + size;
#ifdef _GNU_SOURCE
    sincos(arg_r, &sin_arg_r, &cos_arg_r);
#else
    sin_arg_r  = sin(arg_r);
    cos_arg_r  = cos(arg_r);
#endif
    sinh_arg_i = sinh(arg_i);
    cosh_arg_i = cosh(arg_i);
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
    double sin_Cn, cos_Cn, cos_Ce, sin_Ce, dCn, dCe;
    double Cn = lp.phi, Ce = lp.lam;

    /* ell. LAT, LNG -> Gaussian LAT, LNG */
    Cn  = gatg (Q->cbg, PROJ_ETMERC_ORDER, Cn);
    /* Gaussian LAT, LNG -> compl. sph. LAT */
#ifdef _GNU_SOURCE
    sincos (Cn, &sin_Cn, &cos_Cn);
    sincos (Ce, &sin_Ce, &cos_Ce);
#else
    sin_Cn = sin (Cn);
    cos_Cn = cos (Cn);
    sin_Ce = sin (Ce);
    cos_Ce = cos (Ce);
#endif

    Cn     = atan2 (sin_Cn, cos_Ce*cos_Cn);
    Ce     = atan2 (sin_Ce*cos_Cn,  hypot (sin_Cn, cos_Cn*cos_Ce));

    /* compl. sph. N, E -> ell. norm. N, E */
    Ce  = asinh ( tan (Ce) );     /* Replaces: Ce  = log(tan(FORTPI + Ce*0.5)); */
    Cn += clenS (Q->gtu, PROJ_ETMERC_ORDER, 2*Cn, 2*Ce, &dCn, &dCe);
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
    double sin_Cn, cos_Cn, cos_Ce, sin_Ce, dCn, dCe;
    double Cn = xy.y, Ce = xy.x;

    /* normalize N, E */
    Cn = (Cn - Q->Zb)/Q->Qn;
    Ce = Ce/Q->Qn;

    if (fabs(Ce) <= 2.623395162778) { /* 150 degrees */
        /* norm. N, E -> compl. sph. LAT, LNG */
        Cn += clenS(Q->utg, PROJ_ETMERC_ORDER, 2*Cn, 2*Ce, &dCn, &dCe);
        Ce += dCe;
        Ce = atan (sinh (Ce)); /* Replaces: Ce = 2*(atan(exp(Ce)) - FORTPI); */
        /* compl. sph. LAT -> Gaussian LAT, LNG */
#ifdef _GNU_SOURCE
        sincos (Cn, &sin_Cn, &cos_Cn);
        sincos (Ce, &sin_Ce, &cos_Ce);
#else
        sin_Cn = sin (Cn);
        cos_Cn = cos (Cn);
        sin_Ce = sin (Ce);
        cos_Ce = cos (Ce);
#endif
        Ce     = atan2 (sin_Ce, cos_Ce*cos_Cn);
        Cn     = atan2 (sin_Cn*cos_Ce,  hypot (sin_Ce, cos_Ce*cos_Cn));
        /* Gaussian LAT, LNG -> ell. LAT, LNG */
        lp.phi = gatg (Q->cgb,  PROJ_ETMERC_ORDER, Cn);
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
    Z = gatg (Q->cbg, PROJ_ETMERC_ORDER, P->phi0);

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
        proj_errno_set(P, PJD_ERR_ELLIPSOID_USE_REQUIRED);
        return pj_default_destructor(P, ENOMEM);
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
