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

#include "proj_internal.h"

/*****************************************************************************/
double pj_conformal_lat(double phi, double e) {
    /***********************************
     * The conformal latitude chi
     * giving a sphere which is truly conformal in accordance with the ellipsoid
     * Snyder, A working manual, formula 3-1
     *
     * phi: geodetic latitude, in radians
     * e: ellipsoid eccentricity
     * returns: conformal latitude, in radians
     ***********************************/
    if (e == 0.0)
        return phi;

    const double esinphi = e * sin(phi);
    const double chi =
        2 * atan(tan(M_FORTPI + phi / 2) *
                 std::pow(((1 - esinphi) / (1 + esinphi)), (e / 2))) -
        M_HALFPI;
    return chi;
}

/*****************************************************************************/
double pj_conformal_lat_inverse(double chi, double e, double threshold) {
    /***********************************
     * The inverse formula of the conformal latitude
     * for phi (geodetic latitude) in terms of chi (conformal latitude),
     * Snyder, A working manual, formula 3-4
     *
     * chi: conformal latitude, in radians
     * e: ellipsoid eccentricity
     * threshold: between two consecutive iteratinons to break the loop, radians
     * returns: geodetic latitude, in radians
     ***********************************/
    if (e == 0.0)
        return chi;

    const double taninit = tan(M_PI / 4 + chi / 2);
    double phi = chi;
    for (int i = 0; i < 10; i++) {
        const double esinphi = e * sin(phi);
        const double new_phi =
            2 * atan(taninit *
                     std::pow(((1 + esinphi) / (1 - esinphi)), (e / 2))) -
            0.5 * M_PI;
        if (abs(new_phi - phi) < threshold) {
            phi = new_phi;
            break;
        }
        phi = new_phi;
    }
    return phi;
}

/*****************************************************************************/

// Computes coefficient q such that authalic_latitude = beta = asin(q / qp)
// where qp is q at phi=90deg, i.e. qp = pj_authalic_lat_q_coeff(1, e, one_es)
// Cf  Snyder (3-11) and (3-12)
double pj_authalic_lat_q_coeff(double sinphi, double e, double one_es) {
    constexpr double EPSILON = 1e-7;
    if (e >= EPSILON) {
        const double e_sinphi = e * sinphi;
        const double one_minus_e_sinphi_sq = 1.0 - e_sinphi * e_sinphi;

        /* avoid zero division, fail gracefully */
        if (one_minus_e_sinphi_sq == 0.0)
            return HUGE_VAL;

        // Snyder uses 0.5 * ln((1-e*sinphi)/(1+e*sinphi) which is
        // -atanh(e*sinphi)
        return (one_es *
                (sinphi / one_minus_e_sinphi_sq + atanh(e_sinphi) / e));
    } else
        return (sinphi + sinphi);
}

/*****************************************************************************/

// Computes authalic latitude from sinus of geographic latitude
// qp is q at phi=90deg, i.e. qp = pj_authalic_lat_q_coeff(1, e, one_es)
double pj_authalic_lat(double sinphi, double e, double one_es, double qp) {
    const double q = pj_authalic_lat_q_coeff(sinphi, e, one_es);
    double ratio = q / qp;

    if (fabs(ratio) > 1) {
        /* Rounding error. */
        ratio = ratio > 0 ? 1 : -1;
    }
    return asin(ratio);
}

/*****************************************************************************/

// Computes coefficients needed for pj_authalic_lat_inverse_approx()
// and pj_authalic_lat_inverse_exact()
double *pj_authalic_lat_compute_coeff_for_inverse(double es) {
    double t, *APA;

    constexpr double P00 = 1.0 / 3;
    constexpr double P01 = 31.0 / 180;
    constexpr double P02 = 517.0 / 5040;
    constexpr double P10 = 23.0 / 360;
    constexpr double P11 = 251.0 / 3780;
    constexpr double P20 = 761.0 / 45360;
    constexpr int APA_SIZE = 3;

    if ((APA = (double *)malloc(APA_SIZE * sizeof(double))) != nullptr) {
        APA[0] = es * P00;
        t = es * es;
        APA[0] += t * P01;
        APA[1] = t * P10;
        t *= es;
        APA[0] += t * P02;
        APA[1] += t * P11;
        APA[2] = t * P20;
    }
    return APA;
}

// Compute the (approximate) geographic latitude from beta = authalic_latitude
// where APA = pj_authalic_lat_compute_coeff_for_inverse(),
// using a 6 degree series expansion.
// Cf Snyder (3-18)
double pj_authalic_lat_inverse_approx(double beta, const double *APA) {
    double t = beta + beta;
    return (beta + APA[0] * sin(t) + APA[1] * sin(t + t) +
            APA[2] * sin(t + t + t));
}

/*****************************************************************************/

#if 0
// Naive version of uncommented below function.
double pj_authalic_lat_inverse_exact(double beta, const double *APA,
                                     const PJ *P, double qp) {
    const double phi_init = pj_authalic_latitude_inverse_approx(beta, APA);
    double phi = phi_init;
    for (int i = 0; i < 10; ++i) {
        const double beta_guess = asin(pj_qsfn(sin(phi), P->e, P->one_es) / qp);
        // double beta_guess_plus_eps = asin(pj_qsfn(sin(phi+1e-10), P->e,
        // P->one_es)/qp); double deriv =
        // (beta_guess_plus_eps-beta_guess)/1e-10;
        //  In practice deriv is very close to 1, so:
        const double deriv = 1;
        const double dphi = -(beta_guess - beta) / deriv;
        if (!(fabs(dphi) >= 1e-15))
            return phi;
        phi += dphi;
    }
    return phi_init;
}
#endif

/*****************************************************************************/

// Compute the geographic latitude from beta = authalic_latitude
// where APA = pj_compute_coefficients_for_inverse_authalic_lat() and
// qp = pj_authalic_lat_q_coeff(1, P->e, P->one_es)
// This is an improvement over pj_authalic_latitude_inverse_approx() using
// a Newton iteration to achieve better precision.
// Cf Snyder (3-16)
// Given beta, solve
//   f(phi) = qp*sin(beta)/(1-e^2) - q(phi)/(1-e^2)
// for phi, where
//   q(phi)/(1-e^2) = sin(phi)/(1 - e^2*sin(phi)^2) + atanh(e*sin(phi))/e
// and
//   df(phi)/dphi = - dq(phi)/dphi / (1-e^2)
//                = - 2 * (1-e^2) * cos(phi) / (1 - e^2*sinphi^2)^2
double pj_authalic_lat_inverse_exact(double beta, const double *APA,
                                     const PJ *P, double qp) {
    const double phi_init = pj_authalic_lat_inverse_approx(beta, APA);
    double phi = phi_init;
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
