/*
 * Project:  PROJ
 *
 * Helper functions to compute latitudes
 *
 * Some from Map Projections - A Working Manual. 1987. John P. Snyder
 * https://neacsu.net/docs/geodesy/snyder/2-general/sect_3/
 *
 * Copyright (c) 2025 Javier Jimenez Shaw
 */

#include <math.h>
#include <exception>

#include "proj_internal.h"

/*****************************************************************************/
double pj_conformal_lat(double phi, const PJ *P) {
    /***********************************
     * The conformal latitude chi, in terms of the geodetic latitude, phi.
     *
     * phi: geodetic latitude, in radians
     * returns: conformal latitude, chi, in radians
     * Copied from merc.cpp
     ***********************************/
    if (P->e == 0.0)
        return phi;

    // Instead of calling tan and sin, call sin and cos which the compiler
    // optimizes to a single call to sincos.
    double sphi = sin(phi), cphi = cos(phi);
    return atan(sinh(asinh(sphi / cphi) - P->e * atanh(P->e * sphi)));
}

/*****************************************************************************/
double pj_conformal_lat_inverse(double chi, const PJ *P) {
    /***********************************
     * The geodetic latitude, phi, in terms of the conformal latitude, chi.
     *
     * chi: conformal latitude, in radians
     * returns: geodetic latitude, phi, in radians
     * Copied from merc.cpp
     ***********************************/
    if (P->e == 0.0)
        return chi;

    return atan(pj_sinhpsi2tanphi(P->ctx, tan(chi), P->e));
}

/*****************************************************************************/

// Computes coefficient q such that authalic_latitude = beta = asin(q / qp)
// where qp is q at phi=90deg, i.e. qp = pj_authalic_lat_q(1, e, one_es)
// Cf  Snyder (3-11) and (3-12)
double pj_authalic_lat_q(double sinphi, const PJ *P) {
    constexpr double EPSILON = 1e-7;
    if (P->e >= EPSILON) {
        const double e_sinphi = P->e * sinphi;
        const double one_minus_e_sinphi_sq = 1.0 - e_sinphi * e_sinphi;

        /* avoid zero division, fail gracefully */
        if (one_minus_e_sinphi_sq == 0.0)
            return HUGE_VAL;

        // Snyder uses 0.5 * ln((1-e*sinphi)/(1+e*sinphi) which is
        // -atanh(e*sinphi)
        return P->one_es *
          (sinphi / one_minus_e_sinphi_sq + atanh(e_sinphi) / P->e);
    } else
        return 2 * sinphi;
}

/*****************************************************************************/

// Computes coefficients needed for conversions between geographic and authalic
// latitude.  These are preferred over the analytical expressions for |n| <
// 0.01.  However the inverse series is used to start the inverse method for
// large |n|.

// Ensure we use the cutoff in n consistently
#define PROJ_AUTHALIC_SERIES_VALID(n) (fabs(n) < 0.01)
double *pj_authalic_lat_compute_coeffs(double n) {
    double *APA;
    constexpr int Lmax = int(AuxLat::AUXORDER);
    const int APA_SIZE = Lmax * (PROJ_AUTHALIC_SERIES_VALID(n) ? 2 : 1);
    if ((APA = (double *)malloc(APA_SIZE * sizeof(double)))
        != nullptr) {
        pj_auxlat_coeffs(n, AuxLat::XI, AuxLat::PHI, APA);
        if (PROJ_AUTHALIC_SERIES_VALID(n))
            pj_auxlat_coeffs(n, AuxLat::PHI, AuxLat::XI, APA + Lmax);
    }
    return APA;
}

/*****************************************************************************/

// Computes authalic latitude from the geographic latitude.
// qp is q at phi=90deg, i.e. qp = pj_authalic_lat_q(1, e, one_es)
double pj_authalic_lat(double phi, double sinphi, double cosphi,
                       const double *APA, const PJ *P, double qp) {
    if (PROJ_AUTHALIC_SERIES_VALID(P->n)) {
        constexpr int Lmax = int(AuxLat::AUXORDER);
        return pj_auxlat_convert(phi, sinphi, cosphi, APA + Lmax, Lmax);
    } else {
        // This result is ill-conditioned near the poles.  So don't use this if
        // the series are accurate.
        const double q = pj_authalic_lat_q(sinphi, P);
        double ratio = q / qp;

        if (fabs(ratio) > 1) {
            /* Rounding error. */
            ratio = ratio > 0 ? 1 : -1;
        }
        return asin(ratio);
    }
}

/*****************************************************************************/

// Compute the geographic latitude from beta = authalic_latitude
// where APA = pj_compute_coefficients_for_inverse_authalic_lat() and
// qp = pj_authalic_lat_q(1, P->e, P->one_es)
double pj_authalic_lat_inverse(double beta, const double *APA,
                               const PJ *P, double qp) {
    double phi = pj_auxlat_convert(beta, APA, int(AuxLat::AUXORDER));
    if (PROJ_AUTHALIC_SERIES_VALID(P->n))
      return phi;
    // If the flattening is large, solve
    //   f(phi) = qp*sin(beta)/(1-e^2) - q(phi)/(1-e^2) = 0
    // for phi, using Newton's method, where
    //   q(phi)/(1-e^2) = sin(phi)/(1 - e^2*sin(phi)^2) + atanh(e*sin(phi))/e
    // and
    //   df(phi)/dphi = - dq(phi)/dphi / (1-e^2)
    //                = - 2 * (1-e^2) * cos(phi) / (1 - e^2*sinphi^2)^2
    // This is subject to large roundoff errors near the poles, so only use
    // this if the series isn't accurate.
    const double q = sin(beta) * qp;
    const double q_div_one_minus_es = q / P->one_es;
    for (int i = 0; i < 10; ++i) {
        const double sinphi = sin(phi);
        const double cosphi = cos(phi);
        const double one_minus_es_sin2phi = 1 - P->es * (sinphi * sinphi);
        // Snyder uses 0.5 * ln((1-e*sinphi)/(1+e*sinphi) which is
        // -atanh(e*sinphi)
        const double dphi =
            (one_minus_es_sin2phi * one_minus_es_sin2phi) / (2 * cosphi) *
            (q_div_one_minus_es - sinphi / one_minus_es_sin2phi -
             atanh(P->e * sinphi) / P->e);
        if (!(fabs(dphi) >= 1e-15))
            break;
        phi += dphi;
    }
    return phi;
}
#undef PROJ_AUTHALIC_SERIES_VALID

// Compute the coefficients of the trigonometric series for conversions between
// auxiliary latitudes.  The coefficients are produced by the Maxima code
// auxlat.mac bundled with GeographicLib.  Load this file into Maxima and run
// writecppproj()$ -- this produces the 'static const' declarations below.
// Only a subset of conversions is provided.  To add others, include them in
// the list "required" in writecppproj().
//
// auxin and auxout are one of
//
//   AuxLat::GEOGRAPHIC or AuxLat::PHI for geographic latitude
//   AuxLat::PARAMETRIC or AuxLat::BETA for parametric latitude
//   AuxLat::GEOCENTRIC or AuxLat::THETA for geocentric latitude
//   AuxLat::RECTIFYING or AuxLat::MU for rectifying latitude
//   AuxLat::CONFORMAL or AuxLat::CHI for conformal latitude
//   AuxLat::AUTHALIC or AuxLat::MU for authlatic latitude
//
// The conversions currently supported are
//   PHI <-> MU for meridian distance
//   PHI <-> CHI for tmerc
//   PHI <-> XI for authalic latitude conversions
//   CHI <-> MU for tmerc
//
// See C. F. F. Karney, On auxiliary latitudes,
// Survey Review 56, 165-180 (2024)
// https://doi.org/10.1080/00396265.2023.2217604
// Preprint: https://arxiv.org/abs/2212.05818
void pj_auxlat_coeffs(double n, AuxLat auxin, AuxLat auxout, double c[]) {
    // Generated by Maxima on 2025-03-23 19:13:00-04:00
    constexpr int Lmax = 6;
    static_assert(Lmax == int(AuxLat::AUXORDER),
                  "Mismatch between maxima and AuxLat::AUXORDER");
    static const double coeffs[] = {
        // C[phi,phi] skipped
        // C[phi,beta] skipped
        // C[phi,theta] skipped
        // C[phi,mu]; even coeffs only
        3.0/2.0, -27.0/32.0, 269.0/512.0,
        21.0/16.0, -55.0/32.0, 6759.0/4096.0,
        151.0/96.0, -417.0/128.0,
        1097.0/512.0, -15543.0/2560.0,
        8011.0/2560.0,
        293393.0/61440.0,
        // C[phi,chi]
        2.0, -2.0/3.0, -2.0, 116.0/45.0, 26.0/45.0, -2854.0/675.0,
        7.0/3.0, -8.0/5.0, -227.0/45.0, 2704.0/315.0, 2323.0/945.0,
        56.0/15.0, -136.0/35.0, -1262.0/105.0, 73814.0/2835.0,
        4279.0/630.0, -332.0/35.0, -399572.0/14175.0,
        4174.0/315.0, -144838.0/6237.0,
        601676.0/22275.0,
        // C[phi,xi]
        4.0/3.0, 4.0/45.0, -16.0/35.0, -2582.0/14175.0, 60136.0/467775.0,
        28112932.0/212837625.0,
        46.0/45.0, 152.0/945.0, -11966.0/14175.0, -21016.0/51975.0,
        251310128.0/638512875.0,
        3044.0/2835.0, 3802.0/14175.0, -94388.0/66825.0, -8797648.0/10945935.0,
        6059.0/4725.0, 41072.0/93555.0, -1472637812.0/638512875.0,
        768272.0/467775.0, 455935736.0/638512875.0,
        4210684958.0/1915538625.0,
        // C[beta,phi] skipped
        // C[beta,beta] skipped
        // C[beta,theta] skipped
        // C[beta,mu] skipped
        // C[beta,chi] skipped
        // C[beta,xi] skipped
        // C[theta,phi] skipped
        // C[theta,beta] skipped
        // C[theta,theta] skipped
        // C[theta,mu] skipped
        // C[theta,chi] skipped
        // C[theta,xi] skipped
        // C[mu,phi]; even coeffs only
        -3.0/2.0, 9.0/16.0, -3.0/32.0,
        15.0/16.0, -15.0/32.0, 135.0/2048.0,
        -35.0/48.0, 105.0/256.0,
        315.0/512.0, -189.0/512.0,
        -693.0/1280.0,
        1001.0/2048.0,
        // C[mu,beta] skipped
        // C[mu,theta] skipped
        // C[mu,mu] skipped
        // C[mu,chi]
        1.0/2.0, -2.0/3.0, 5.0/16.0, 41.0/180.0, -127.0/288.0, 7891.0/37800.0,
        13.0/48.0, -3.0/5.0, 557.0/1440.0, 281.0/630.0, -1983433.0/1935360.0,
        61.0/240.0, -103.0/140.0, 15061.0/26880.0, 167603.0/181440.0,
        49561.0/161280.0, -179.0/168.0, 6601661.0/7257600.0,
        34729.0/80640.0, -3418889.0/1995840.0,
        212378941.0/319334400.0,
        // C[mu,xi] skipped
        // C[chi,phi]
        -2.0, 2.0/3.0, 4.0/3.0, -82.0/45.0, 32.0/45.0, 4642.0/4725.0,
        5.0/3.0, -16.0/15.0, -13.0/9.0, 904.0/315.0, -1522.0/945.0,
        -26.0/15.0, 34.0/21.0, 8.0/5.0, -12686.0/2835.0,
        1237.0/630.0, -12.0/5.0, -24832.0/14175.0,
        -734.0/315.0, 109598.0/31185.0,
        444337.0/155925.0,
        // C[chi,beta] skipped
        // C[chi,theta] skipped
        // C[chi,mu]
        -1.0/2.0, 2.0/3.0, -37.0/96.0, 1.0/360.0, 81.0/512.0,
        -96199.0/604800.0,
        -1.0/48.0, -1.0/15.0, 437.0/1440.0, -46.0/105.0, 1118711.0/3870720.0,
        -17.0/480.0, 37.0/840.0, 209.0/4480.0, -5569.0/90720.0,
        -4397.0/161280.0, 11.0/504.0, 830251.0/7257600.0,
        -4583.0/161280.0, 108847.0/3991680.0,
        -20648693.0/638668800.0,
        // C[chi,chi] skipped
        // C[chi,xi] skipped
        // C[xi,phi]
        -4.0/3.0, -4.0/45.0, 88.0/315.0, 538.0/4725.0, 20824.0/467775.0,
        -44732.0/2837835.0,
        34.0/45.0, 8.0/105.0, -2482.0/14175.0, -37192.0/467775.0,
        -12467764.0/212837625.0,
        -1532.0/2835.0, -898.0/14175.0, 54968.0/467775.0,
        100320856.0/1915538625.0,
        6007.0/14175.0, 24496.0/467775.0, -5884124.0/70945875.0,
        -23356.0/66825.0, -839792.0/19348875.0,
        570284222.0/1915538625.0,
        // C[xi,beta] skipped
        // C[xi,theta] skipped
        // C[xi,mu] skipped
        // C[xi,chi] skipped
        // C[xi,xi] skipped
    };
    static const int ptrs[] = {
        0, 0, 0, 0, 12, 33, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54,
        66, 66, 66, 66, 87, 87, 108, 108, 108, 129, 129, 129, 150, 150, 150,
        150, 150, 150,
    };
    if (!( auxin >= AuxLat::START && auxin < AuxLat::AUXNUMBER &&
           auxout >= AuxLat::START && auxout < AuxLat::AUXNUMBER ))
        throw std::out_of_range("Bad specification for auxiliary latitude");
    int k = int(AuxLat::AUXNUMBER) * int(auxout) + int(auxin), o = ptrs[k];
    if (o == ptrs[k+1])
        throw std::out_of_range
            ("Unsupported conversion between auxiliary latitudes");
    double d = n, n2 = n*n;
    if (auxin <= AuxLat::RECTIFYING && auxout <= AuxLat::RECTIFYING) {
        for (int l = 0; l < Lmax; ++l) {
            int m = (Lmax - l - 1) / 2; // order of polynomial in n^2
            c[l] = d * pj_polyval(n2, coeffs + o, m);
            o += m + 1;
            d *= n;
        }
    } else {
        for (int l = 0; l < Lmax; ++l) {
            int m = (Lmax - l - 1); // order of polynomial in n
            c[l] = d * pj_polyval(n, coeffs + o, m);
            o += m + 1;
            d *= n;
        }
    }
    // assert (o == ptrs[k+1]);
}

// Evaluation sum(p[i] * x^i, i, 0, N) via Horner's method.  N.B. p is of
// length N+1.
double pj_polyval(double x, const double p[], int N) {
    double y = N < 0 ? 0 : p[N];
    for (; N > 0;)
        y = y * x + p[--N];
    return y;
}

// Evaluate y = sum(c[k] * sin((2*k+2) * zeta), k, 0, K-1) by Clenshaw
// summation.
double pj_clenshaw(double szeta, double czeta, const double c[], int K) {
    // Approx operation count = (K + 5) mult and (2 * K + 2) add
    double u0 = 0, u1 = 0,                         // accumulators for sum
        X = 2 * (czeta - szeta) * (czeta + szeta); // 2 * cos(2*zeta)
    for (; K > 0;) {
        double t = X * u0 - u1 + c[--K];
        u1 = u0;
        u0 = t;
    }
    return 2 * szeta * czeta * u0; // sin(2*zeta) * u0
}

// Convert auxiliary latitude zeto to eta, given C[eta, zeta].
double pj_auxlat_convert(double zeta, double szeta, double czeta,
                         const double c[], int K) {
    return zeta + pj_clenshaw(szeta, czeta, c, K);
}

// Convert auxiliary latitude zeto to eta, given C[eta, zeta].
double pj_auxlat_convert(double zeta, const double c[], int K) {
    return pj_auxlat_convert(zeta, sin(zeta), cos(zeta), c, K);
}

// Convert auxiliary latitude zeto to eta, given C[eta, zeta].  In this form
// the sine and cosine of eta are returned.  This provided high relative
// accuracy near the poles.
void pj_auxlat_convert(double szeta, double czeta, const double c[], int K,
                       double& seta, double& ceta) {
    double delta = pj_clenshaw(szeta, czeta, c, K),
        sdelta = sin(delta), cdelta = cos(delta);
    seta = szeta * cdelta + czeta * sdelta;
    ceta = czeta * cdelta - szeta * sdelta;
}

// Compute the rectifying radius = quarter meridian / (pi/2 * a)
double pj_rectifying_radius(double n) {
    // Expansion of (quarter meridian) / ((a+b)/2 * pi/2) as series in n^2;
    // these coefficients are ( (2*k - 3)!! / (2*k)!! )^2 for k = 0..3
    static const double coeff_rad[] = {1, 1.0 / 4, 1.0 / 64, 1.0 / 256};
    return pj_polyval(n * n, coeff_rad, 3) / (1 + n);
}
