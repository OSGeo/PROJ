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
