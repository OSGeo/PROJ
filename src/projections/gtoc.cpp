/******************************************************************************
 * This file implements the Ground Track Oblique Cassinin (GTOC) projection
 *
 * This is in somewhat similar to the Space Oblique Mercator (SOM) Projection
 * developed for Landsat. However, Landsat has a very narrow swath, and the
 * GTOC projection better preserves scale across a wider swath, making it more
 * suited for sensors such as VIIRS. Also, unlike the SOM, it keeps the
 * ground-track centered, which is a more efficient use of screen area when it
 * is viewed.
 *
 *****************************************************************************/

#define PJ_LIB__

#include <errno.h>
#include <math.h>

#include "proj.h"
#include "proj_internal.h"

PROJ_HEAD(gtoc, "Ground-Track Oblique Cassini")
"\n\tCyl, Sph&Ell\n\tpath=";

#define TOL 1e-7

namespace { // anonymous namespace

// A Simple 3D vector structure
struct vec {
    double x, y, z;
};

// A simple 3x3 rotation matrix structure
struct mat {
    double m11, m12, m13;
    double m21, m22, m23;
    double m31, m32, m33;
};

struct pj_opaque {
    // Projection parameters:
    double lon0;   // Longitude of the centre of the projection in radians
    double lat0;   // Latitude of the centre of the projection in radians
    double az0;    // Azimuth Rotation Angle in radians
    double theta0; // Theta rotation parameter in radians
    double scale;  // Scaling Factor, will be used as earth radius

    double epr2; // Ratio of Squared earth equatorial radius by Squared earth
                 // polar radius.

    mat e2m; // Earth to Map transformation matrix
    mat m2e; // Map to Earth transformation matrix
};

} // anonymous namespace

/**
 * Helper function used to create a rotation matrix around the X axis.
 */
static mat rotX(double ang) {
    return {1.0, 0.0, 0.0, 0.0, cos(ang), -sin(ang), 0.0, sin(ang), cos(ang)};
}

/**
 * Helper function used to create a rotation matrix around the Y axis.
 */
static mat rotY(double ang) {
    return {cos(ang), 0.0, sin(ang), 0.0, 1.0, 0.0, -sin(ang), 0.0, cos(ang)};
}

/**
 * Helper function used to create a rotation matrix around the Z axis.
 */
static mat rotZ(double ang) {
    return {
        cos(ang), -sin(ang), 0.0, sin(ang), cos(ang), 0.0, 0.0, 0.0, 1.0,
    };
}

/**
 * Helper function used to multiply 2 3x3 matrices.
 */
static mat matmult(const mat &A, const mat &B) {
    return {A.m11 * B.m11 + A.m12 * B.m21 + A.m13 * B.m31,
            A.m11 * B.m12 + A.m12 * B.m22 + A.m13 * B.m32,
            A.m11 * B.m13 + A.m12 * B.m23 + A.m13 * B.m33,
            A.m21 * B.m11 + A.m22 * B.m21 + A.m23 * B.m31,
            A.m21 * B.m12 + A.m22 * B.m22 + A.m23 * B.m32,
            A.m21 * B.m13 + A.m22 * B.m23 + A.m23 * B.m33,
            A.m31 * B.m11 + A.m32 * B.m21 + A.m33 * B.m31,
            A.m31 * B.m12 + A.m32 * B.m22 + A.m33 * B.m32,
            A.m31 * B.m13 + A.m32 * B.m23 + A.m33 * B.m33};
}

/**
 * Helper function used multiply a matrix and a vector.
 */
static vec mult(const mat &A, const vec &B) {
    return {A.m11 * B.x + A.m12 * B.y + A.m13 * B.z,
            A.m21 * B.x + A.m22 * B.y + A.m23 * B.z,
            A.m31 * B.x + A.m32 * B.y + A.m33 * B.z};
}

/**
 * Helper function used to transpose a 3x3 matrices.
 */
static mat transpose(const mat &A) {
    return {A.m11, A.m21, A.m31, A.m12, A.m22, A.m32, A.m13, A.m23, A.m33};
}

/*
 * Helper function to perform the conversion from elliptical to spherical angle
 *
 * parameter epr2: squared equatorial to polar radius ratio
 * parameter ang: input angle to convert in radians
 * returns the converted angle in radians.
 */
static double ell2sph(double epr2, double ang) { return atan(tan(ang) / epr2); }

/*
 * Helper function to perform the conversion from spherical to elliptical angle
 *
 * parameter epr2: squared equatorial to polar radius ratio
 * parameter ang: input angle to convert in radians
 * returns the converted angle in radians.
 */
static double sph2ell(double epr2, double ang) { return atan(tan(ang) * epr2); }

/*
 * Helper function to convert spherical to cartesian coordinates
 *
 * parameter rho: Rho input distance
 * parameter az: Azimuth angle in radians
 * parameter el: Elevation angle in radians
 * returns the x, y, z cartesian coordinates as a 3d vector
 */
static vec sph2cart(double rho, double az, double el) {
    double cost = cos(el);
    return {rho * cost * cos(az), rho * cost * sin(az), rho * sin(el)};
}

/*
 * Helper function to convert cartesian to spherical coordinates
 *
 * parameter x: X input coordinate
 * parameter y: X input coordinate
 * parameter z: X input coordinate
 * returns the rho, az, el spherical coordinates as a 3d vector.
 */
static vec cart2sph(double x, double y, double z) {
    double rho = sqrt(x * x + y * y + z * z);
    return {rho, atan2(y, x), asin(z / rho)};
}

static PJ_XY gtoc_e_forward(PJ_LP lp, PJ *P) { /* Ellipsoidal, forward */
    PJ_XY xy = {0.0, 0.0};
    struct pj_opaque *Q = static_cast<struct pj_opaque *>(P->opaque);
    // Conversion to Spherical System:
    double rho = Q->scale;
    double az = lp.lam;
    double el = ell2sph(Q->epr2, lp.phi);

    // Convert Spherical to Cartesian:
    vec v_in = sph2cart(rho, az, el);

    // Perform Rotation:
    vec v_out = mult(Q->e2m, v_in);

    // Convert Cartesian to Spherical
    vec rhoAzEl = cart2sph(v_out.x, v_out.y, v_out.z);

    // Apply Scale and Offset factors
    az = rhoAzEl.y;
    el = rhoAzEl.z;
    xy.x = -Q->scale * (el + Q->theta0);
    xy.y = Q->scale * cos(Q->theta0) * az;

    return xy;
}

static PJ_LP gtoc_e_inverse(PJ_XY xy, PJ *P) { /* Ellipsoidal, inverse */
    PJ_LP lp = {0.0, 0.0};
    struct pj_opaque *Q = static_cast<struct pj_opaque *>(P->opaque);

    // Conversion to spherical coordinates:
    double rho = Q->scale;
    double az = xy.y / (cos(Q->theta0) * Q->scale);
    double el = -xy.x / Q->scale - Q->theta0;

    // Conversion to Cartesian:
    vec v_in = sph2cart(rho, az, el);

    // Perform Rotation
    vec v_out = mult(Q->m2e, v_in);

    // Conversion to Spherical Coordinates
    vec rhoAzEl = cart2sph(v_out.x, v_out.y, v_out.z);

    lp.lam = rhoAzEl.y;
    lp.phi = sph2ell(Q->epr2, rhoAzEl.z);

    return lp;
}

PJ *PROJECTION(gtoc) {
    struct pj_opaque *Q =
        static_cast<struct pj_opaque *>(calloc(1, sizeof(struct pj_opaque)));
    if (nullptr == Q)
        return pj_default_destructor(P, ENOMEM);
    P->opaque = Q;

    // Earth ellipsoid parameters:
    double equR = 6378137.0;
    double polR = 6356752.3;
    double equR2 = pow(equR, 2);
    double polR2 = pow(polR, 2);

    // Initializing projection parameters
    // Note: calloc call above will init all values to 0.0 by default.
    Q->lon0 = P->lam0;
    Q->lat0 = P->phi0;

    // Reset the projection parameters:
    P->lam0 = 0.0;
    P->phi0 = 0.0;

    if (pj_param(P->ctx, P->params, "taz_0").i) {
        Q->az0 = pj_param(P->ctx, P->params, "raz_0").f;
    }

    if (pj_param(P->ctx, P->params, "ttheta_0").i) {
        Q->theta0 = pj_param(P->ctx, P->params, "rtheta_0").f;
    }

    if (pj_param(P->ctx, P->params, "tscale").i) {
        Q->scale = pj_param(P->ctx, P->params, "dscale").f;
    } else {
        // No scale value explicitly provided so we compute the default scale
        // here:
        double CLat = cos(Q->lat0);
        double Clat2 = pow(CLat, 2);
        double SLat2 = 1. - Clat2;
        Q->scale = sqrt((pow(equR2, 2) * Clat2 + pow(polR2, 2) * SLat2) /
                        (equR2 * Clat2 + polR2 * SLat2));
    }

    Q->epr2 = equR2 / polR2;

    // Initializing the trnansformation matrices:
    // Converting Geotedic to Spheric
    mat tmp1 = matmult(rotY(Q->theta0), rotX(-Q->az0));
    mat tmp2 = matmult(rotY(ell2sph(Q->epr2, Q->lat0)), rotZ(-Q->lon0));
    Q->e2m = matmult(tmp1, tmp2);

    Q->m2e = transpose(Q->e2m);

    // Prevent the use of CLASSIC units by default for outputs:
    P->right = PJ_IO_UNITS_WHATEVER;

    P->inv = gtoc_e_inverse;
    P->fwd = gtoc_e_forward;

    return P;
}
