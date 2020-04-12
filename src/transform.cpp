/******************************************************************************
 * Project:  PROJ.4
 * Purpose:  Perform overall coordinate system to coordinate system
 *           transformations (pj_transform() function) including reprojection
 *           and datum shifting.
 * Author:   Frank Warmerdam, warmerdam@pobox.com
 *
 ******************************************************************************
 * Copyright (c) 2000, Frank Warmerdam
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *****************************************************************************/

#include <errno.h>
#include <math.h>
#include <string.h>

#include "proj.h"
#include "proj_internal.h"
#include "grids.hpp"

using namespace NS_PROJ;


///////////////////////////////////////////////////////////////////////////////
/// From older geocent.h
///////////////////////////////////////////////////////////////////////////////


/***************************************************************************/
/* RSC IDENTIFIER:  GEOCENTRIC
 *
 * ABSTRACT
 *
 *    This component provides conversions between Geodetic coordinates (latitude,
 *    longitude in radians and height in meters) and Geocentric coordinates
 *    (X, Y, Z) in meters.
 *
 * ERROR HANDLING
 *
 *    This component checks parameters for valid values.  If an invalid value
 *    is found, the error code is combined with the current error code using 
 *    the bitwise or.  This combining allows multiple error codes to be
 *    returned. The possible error codes are:
 *
 *      GEOCENT_NO_ERROR        : No errors occurred in function
 *      GEOCENT_LAT_ERROR       : Latitude out of valid range
 *                                 (-90 to 90 degrees)
 *      GEOCENT_LON_ERROR       : Longitude out of valid range
 *                                 (-180 to 360 degrees)
 *      GEOCENT_A_ERROR         : Semi-major axis less than or equal to zero
 *      GEOCENT_B_ERROR         : Semi-minor axis less than or equal to zero
 *      GEOCENT_A_LESS_B_ERROR  : Semi-major axis less than semi-minor axis
 *
 *
 * REUSE NOTES
 *
 *    GEOCENTRIC is intended for reuse by any application that performs
 *    coordinate conversions between geodetic coordinates and geocentric
 *    coordinates.
 *    
 *
 * REFERENCES
 *    
 *    An Improved Algorithm for Geocentric to Geodetic Coordinate Conversion,
 *    Ralph Toms, February 1996  UCRL-JC-123138.
 *    
 *    Further information on GEOCENTRIC can be found in the Reuse Manual.
 *
 *    GEOCENTRIC originated from : U.S. Army Topographic Engineering Center
 *                                 Geospatial Information Division
 *                                 7701 Telegraph Road
 *                                 Alexandria, VA  22310-3864
 *
 * LICENSES
 *
 *    None apply to this component.
 *
 * RESTRICTIONS
 *
 *    GEOCENTRIC has no restrictions.
 *
 * ENVIRONMENT
 *
 *    GEOCENTRIC was tested and certified in the following environments:
 *
 *    1. Solaris 2.5 with GCC version 2.8.1
 *    2. Windows 95 with MS Visual C++ version 6
 *
 * MODIFICATIONS
 *
 *    Date              Description
 *    ----              -----------
 *
 *
 */


/***************************************************************************/
/*
 *                              DEFINES
 */
#define GEOCENT_NO_ERROR        0x0000
#define GEOCENT_LAT_ERROR       0x0001
#define GEOCENT_LON_ERROR       0x0002
#define GEOCENT_A_ERROR         0x0004
#define GEOCENT_B_ERROR         0x0008
#define GEOCENT_A_LESS_B_ERROR  0x0010


/***************************************************************************/
/*
 *                              FUNCTION PROTOTYPES
 */

/* ensure proper linkage to c++ programs */
#ifdef __cplusplus
extern "C" {
#endif

typedef struct 
{
    double Geocent_a;        /* Semi-major axis of ellipsoid in meters */
    double Geocent_b;        /* Semi-minor axis of ellipsoid           */
    double Geocent_a2;       /* Square of semi-major axis */
    double Geocent_b2;       /* Square of semi-minor axis */
    double Geocent_e2;       /* Eccentricity squared  */
    double Geocent_ep2;      /* 2nd eccentricity squared */
} GeocentricInfo;

void pj_Init_Geocentric( GeocentricInfo *gi );
long pj_Set_Geocentric_Parameters( GeocentricInfo *gi, 
                                   double a, 
                                   double b);

/*
 * The function Set_Geocentric_Parameters receives the ellipsoid parameters
 * as inputs and sets the corresponding state variables.
 *
 *    a  : Semi-major axis, in meters.          (input)
 *    b  : Semi-minor axis, in meters.          (input)
 */


void pj_Get_Geocentric_Parameters ( GeocentricInfo *gi,
                                    double *a, 
                                    double *b);

/*
 * The function Get_Geocentric_Parameters returns the ellipsoid parameters
 * to be used in geocentric coordinate conversions.
 *
 *    a  : Semi-major axis, in meters.          (output)
 *    b  : Semi-minor axis, in meters.          (output)
 */


long pj_Convert_Geodetic_To_Geocentric ( GeocentricInfo *gi,
                                         double Latitude,
                                         double Longitude,
                                         double Height,
                                         double *X,
                                         double *Y,
                                         double *Z);
/*
 * The function Convert_Geodetic_To_Geocentric converts geodetic coordinates
 * (latitude, longitude, and height) to geocentric coordinates (X, Y, Z),
 * according to the current ellipsoid parameters.
 *
 *    Latitude  : Geodetic latitude in radians                     (input)
 *    Longitude : Geodetic longitude in radians                    (input)
 *    Height    : Geodetic height, in meters                       (input)
 *    X         : Calculated Geocentric X coordinate, in meters.   (output)
 *    Y         : Calculated Geocentric Y coordinate, in meters.   (output)
 *    Z         : Calculated Geocentric Z coordinate, in meters.   (output)
 *
 */


void pj_Convert_Geocentric_To_Geodetic (GeocentricInfo *gi,
                                        double X,
                                        double Y, 
                                        double Z,
                                        double *Latitude,
                                        double *Longitude,
                                        double *Height);
/*
 * The function Convert_Geocentric_To_Geodetic converts geocentric
 * coordinates (X, Y, Z) to geodetic coordinates (latitude, longitude, 
 * and height), according to the current ellipsoid parameters.
 *
 *    X         : Geocentric X coordinate, in meters.         (input)
 *    Y         : Geocentric Y coordinate, in meters.         (input)
 *    Z         : Geocentric Z coordinate, in meters.         (input)
 *    Latitude  : Calculated latitude value in radians.       (output)
 *    Longitude : Calculated longitude value in radians.      (output)
 *    Height    : Calculated height value, in meters.         (output)
 */


#ifdef __cplusplus
}
#endif


///////////////////////////////////////////////////////////////////////////////
/// From older geocent.cpp
///////////////////////////////////////////////////////////////////////////////


/***************************************************************************/
/* RSC IDENTIFIER:  GEOCENTRIC
 *
 * ABSTRACT
 *
 *    This component provides conversions between Geodetic coordinates (latitude,
 *    longitude in radians and height in meters) and Geocentric coordinates
 *    (X, Y, Z) in meters.
 *
 * ERROR HANDLING
 *
 *    This component checks parameters for valid values.  If an invalid value
 *    is found, the error code is combined with the current error code using 
 *    the bitwise or.  This combining allows multiple error codes to be
 *    returned. The possible error codes are:
 *
 *      GEOCENT_NO_ERROR        : No errors occurred in function
 *      GEOCENT_LAT_ERROR       : Latitude out of valid range
 *                                 (-90 to 90 degrees)
 *      GEOCENT_LON_ERROR       : Longitude out of valid range
 *                                 (-180 to 360 degrees)
 *      GEOCENT_A_ERROR         : Semi-major axis lessthan or equal to zero
 *      GEOCENT_B_ERROR         : Semi-minor axis lessthan or equal to zero
 *      GEOCENT_A_LESS_B_ERROR  : Semi-major axis less than semi-minor axis
 *
 *
 * REUSE NOTES
 *
 *    GEOCENTRIC is intended for reuse by any application that performs
 *    coordinate conversions between geodetic coordinates and geocentric
 *    coordinates.
 *    
 *
 * REFERENCES
 *    
 *    An Improved Algorithm for Geocentric to Geodetic Coordinate Conversion,
 *    Ralph Toms, February 1996  UCRL-JC-123138.
 *    
 *    Further information on GEOCENTRIC can be found in the Reuse Manual.
 *
 *    GEOCENTRIC originated from : U.S. Army Topographic Engineering Center
 *                                 Geospatial Information Division
 *                                 7701 Telegraph Road
 *                                 Alexandria, VA  22310-3864
 *
 * LICENSES
 *
 *    None apply to this component.
 *
 * RESTRICTIONS
 *
 *    GEOCENTRIC has no restrictions.
 *
 * ENVIRONMENT
 *
 *    GEOCENTRIC was tested and certified in the following environments:
 *
 *    1. Solaris 2.5 with GCC version 2.8.1
 *    2. Windows 95 with MS Visual C++ version 6
 *
 * MODIFICATIONS
 *
 *    Date              Description
 *    ----              -----------
 *    25-02-97          Original Code
 *
 */


/***************************************************************************/
/*
 *                               INCLUDES
 */
#include <math.h>
//#include "geocent.h"
/*
 *    math.h     - is needed for calls to sin, cos, tan and sqrt.
 *    geocent.h  - is needed for Error codes and prototype error checking.
 */


/***************************************************************************/
/*
 *                               DEFINES
 */
#define PI         3.14159265358979323e0
#define PI_OVER_2  (PI / 2.0e0)
#define FALSE      0
#define TRUE       1
#define COS_67P5   0.38268343236508977  /* cosine of 67.5 degrees */
#define AD_C       1.0026000            /* Toms region 1 constant */


/***************************************************************************/
/*
 *                              FUNCTIONS     
 */


long pj_Set_Geocentric_Parameters (GeocentricInfo *gi, double a, double b) 

{ /* BEGIN Set_Geocentric_Parameters */
/*
 * The function Set_Geocentric_Parameters receives the ellipsoid parameters
 * as inputs and sets the corresponding state variables.
 *
 *    a  : Semi-major axis, in meters.          (input)
 *    b  : Semi-minor axis, in meters.          (input)
 */
    long Error_Code = GEOCENT_NO_ERROR;

    if (a <= 0.0)
        Error_Code |= GEOCENT_A_ERROR;
    if (b <= 0.0)
        Error_Code |= GEOCENT_B_ERROR;
    if (a < b)
        Error_Code |= GEOCENT_A_LESS_B_ERROR;
    if (!Error_Code)
    {
        gi->Geocent_a = a;
        gi->Geocent_b = b;
        gi->Geocent_a2 = a * a;
        gi->Geocent_b2 = b * b;
        gi->Geocent_e2 = (gi->Geocent_a2 - gi->Geocent_b2) / gi->Geocent_a2;
        gi->Geocent_ep2 = (gi->Geocent_a2 - gi->Geocent_b2) / gi->Geocent_b2;
    }
    return (Error_Code);
} /* END OF Set_Geocentric_Parameters */


void pj_Get_Geocentric_Parameters (GeocentricInfo *gi,
                                   double *a, 
                                   double *b)
{ /* BEGIN Get_Geocentric_Parameters */
/*
 * The function Get_Geocentric_Parameters returns the ellipsoid parameters
 * to be used in geocentric coordinate conversions.
 *
 *    a  : Semi-major axis, in meters.          (output)
 *    b  : Semi-minor axis, in meters.          (output)
 */

    *a = gi->Geocent_a;
    *b = gi->Geocent_b;
} /* END OF Get_Geocentric_Parameters */


long pj_Convert_Geodetic_To_Geocentric (GeocentricInfo *gi,
                                        double Latitude,
                                        double Longitude,
                                        double Height,
                                        double *X,
                                        double *Y,
                                        double *Z) 
{ /* BEGIN Convert_Geodetic_To_Geocentric */
/*
 * The function Convert_Geodetic_To_Geocentric converts geodetic coordinates
 * (latitude, longitude, and height) to geocentric coordinates (X, Y, Z),
 * according to the current ellipsoid parameters.
 *
 *    Latitude  : Geodetic latitude in radians                     (input)
 *    Longitude : Geodetic longitude in radians                    (input)
 *    Height    : Geodetic height, in meters                       (input)
 *    X         : Calculated Geocentric X coordinate, in meters    (output)
 *    Y         : Calculated Geocentric Y coordinate, in meters    (output)
 *    Z         : Calculated Geocentric Z coordinate, in meters    (output)
 *
 */
  long Error_Code = GEOCENT_NO_ERROR;
  double Rn;            /*  Earth radius at location  */
  double Sin_Lat;       /*  sin(Latitude)  */
  double Sin2_Lat;      /*  Square of sin(Latitude)  */
  double Cos_Lat;       /*  cos(Latitude)  */

  /*
  ** Don't blow up if Latitude is just a little out of the value
  ** range as it may just be a rounding issue.  Also removed longitude
  ** test, it should be wrapped by cos() and sin().  NFW for PROJ.4, Sep/2001.
  */
  if( Latitude < -PI_OVER_2 && Latitude > -1.001 * PI_OVER_2 )
      Latitude = -PI_OVER_2;
  else if( Latitude > PI_OVER_2 && Latitude < 1.001 * PI_OVER_2 )
      Latitude = PI_OVER_2;
  else if ((Latitude < -PI_OVER_2) || (Latitude > PI_OVER_2))
  { /* Latitude out of range */
    Error_Code |= GEOCENT_LAT_ERROR;
  }

  if (!Error_Code)
  { /* no errors */
    if (Longitude > PI)
      Longitude -= (2*PI);
    Sin_Lat = sin(Latitude);
    Cos_Lat = cos(Latitude);
    Sin2_Lat = Sin_Lat * Sin_Lat;
    Rn = gi->Geocent_a / (sqrt(1.0e0 - gi->Geocent_e2 * Sin2_Lat));
    *X = (Rn + Height) * Cos_Lat * cos(Longitude);
    *Y = (Rn + Height) * Cos_Lat * sin(Longitude);
    *Z = ((Rn * (1 - gi->Geocent_e2)) + Height) * Sin_Lat;

  }
  return (Error_Code);
} /* END OF Convert_Geodetic_To_Geocentric */

/*
 * The function Convert_Geocentric_To_Geodetic converts geocentric
 * coordinates (X, Y, Z) to geodetic coordinates (latitude, longitude, 
 * and height), according to the current ellipsoid parameters.
 *
 *    X         : Geocentric X coordinate, in meters.         (input)
 *    Y         : Geocentric Y coordinate, in meters.         (input)
 *    Z         : Geocentric Z coordinate, in meters.         (input)
 *    Latitude  : Calculated latitude value in radians.       (output)
 *    Longitude : Calculated longitude value in radians.      (output)
 *    Height    : Calculated height value, in meters.         (output)
 */

#define USE_ITERATIVE_METHOD

void pj_Convert_Geocentric_To_Geodetic (GeocentricInfo *gi,
                                        double X,
                                        double Y, 
                                        double Z,
                                        double *Latitude,
                                        double *Longitude,
                                        double *Height)
{ /* BEGIN Convert_Geocentric_To_Geodetic */
#if !defined(USE_ITERATIVE_METHOD)
/*
 * The method used here is derived from 'An Improved Algorithm for
 * Geocentric to Geodetic Coordinate Conversion', by Ralph Toms, Feb 1996
 */

/* Note: Variable names follow the notation used in Toms, Feb 1996 */

    double W;        /* distance from Z axis */
    double W2;       /* square of distance from Z axis */
    double T0;       /* initial estimate of vertical component */
    double T1;       /* corrected estimate of vertical component */
    double S0;       /* initial estimate of horizontal component */
    double S1;       /* corrected estimate of horizontal component */
    double Sin_B0;   /* sin(B0), B0 is estimate of Bowring aux variable */
    double Sin3_B0;  /* cube of sin(B0) */
    double Cos_B0;   /* cos(B0) */
    double Sin_p1;   /* sin(phi1), phi1 is estimated latitude */
    double Cos_p1;   /* cos(phi1) */
    double Rn;       /* Earth radius at location */
    double Sum;      /* numerator of cos(phi1) */
    int At_Pole;     /* indicates location is in polar region */

    At_Pole = FALSE;
    if (X != 0.0)
    {
        *Longitude = atan2(Y,X);
    }
    else
    {
        if (Y > 0)
        {
            *Longitude = PI_OVER_2;
        }
        else if (Y < 0)
        {
            *Longitude = -PI_OVER_2;
        }
        else
        {
            At_Pole = TRUE;
            *Longitude = 0.0;
            if (Z > 0.0)
            {  /* north pole */
                *Latitude = PI_OVER_2;
            }
            else if (Z < 0.0)
            {  /* south pole */
                *Latitude = -PI_OVER_2;
            }
            else
            {  /* center of earth */
                *Latitude = PI_OVER_2;
                *Height = -Geocent_b;
                return;
            } 
        }
    }
    W2 = X*X + Y*Y;
    W = sqrt(W2);
    T0 = Z * AD_C;
    S0 = sqrt(T0 * T0 + W2);
    Sin_B0 = T0 / S0;
    Cos_B0 = W / S0;
    Sin3_B0 = Sin_B0 * Sin_B0 * Sin_B0;
    T1 = Z + gi->Geocent_b * gi->Geocent_ep2 * Sin3_B0;
    Sum = W - gi->Geocent_a * gi->Geocent_e2 * Cos_B0 * Cos_B0 * Cos_B0;
    S1 = sqrt(T1*T1 + Sum * Sum);
    Sin_p1 = T1 / S1;
    Cos_p1 = Sum / S1;
    Rn = gi->Geocent_a / sqrt(1.0 - gi->Geocent_e2 * Sin_p1 * Sin_p1);
    if (Cos_p1 >= COS_67P5)
    {
        *Height = W / Cos_p1 - Rn;
    }
    else if (Cos_p1 <= -COS_67P5)
    {
        *Height = W / -Cos_p1 - Rn;
    }
    else
    {
        *Height = Z / Sin_p1 + Rn * (gi->Geocent_e2 - 1.0);
    }
    if (At_Pole == FALSE)
    {
        *Latitude = atan(Sin_p1 / Cos_p1);
    }
#else /* defined(USE_ITERATIVE_METHOD) */
/*
* Reference...
* ============
* Wenzel, H.-G.(1985): Hochauflösende Kugelfunktionsmodelle für
* das Gravitationspotential der Erde. Wiss. Arb. Univ. Hannover
* Nr. 137, p. 130-131.

* Programmed by GGA- Leibniz-Institute of Applied Geophysics
*               Stilleweg 2
*               D-30655 Hannover
*               Federal Republic of Germany
*               Internet: www.gga-hannover.de
*
*               Hannover, March 1999, April 2004.
*               see also: comments in statements
* remarks:
* Mathematically exact and because of symmetry of rotation-ellipsoid,
* each point (X,Y,Z) has at least two solutions (Latitude1,Longitude1,Height1) and
* (Latitude2,Longitude2,Height2). Is point=(0.,0.,Z) (P=0.), so you get even
* four solutions,	every two symmetrical to the semi-minor axis.
* Here Height1 and Height2 have at least a difference in order of
* radius of curvature (e.g. (0,0,b)=> (90.,0.,0.) or (-90.,0.,-2b);
* (a+100.)*(sqrt(2.)/2.,sqrt(2.)/2.,0.) => (0.,45.,100.) or
* (0.,225.,-(2a+100.))).
* The algorithm always computes (Latitude,Longitude) with smallest |Height|.
* For normal computations, that means |Height|<10000.m, algorithm normally
* converges after to 2-3 steps!!!
* But if |Height| has the amount of length of ellipsoid's axis
* (e.g. -6300000.m),	algorithm needs about 15 steps.
*/

/* local definitions and variables */
/* end-criterium of loop, accuracy of sin(Latitude) */
#define genau   1.E-12
#define genau2  (genau*genau)
#define maxiter 30

    double P;        /* distance between semi-minor axis and location */
    double RR;       /* distance between center and location */
    double CT;       /* sin of geocentric latitude */
    double ST;       /* cos of geocentric latitude */
    double RX;
    double RK;
    double RN;       /* Earth radius at location */
    double CPHI0;    /* cos of start or old geodetic latitude in iterations */
    double SPHI0;    /* sin of start or old geodetic latitude in iterations */
    double CPHI;     /* cos of searched geodetic latitude */
    double SPHI;     /* sin of searched geodetic latitude */
    double SDPHI;    /* end-criterium: addition-theorem of sin(Latitude(iter)-Latitude(iter-1)) */
    int iter;        /* # of continuous iteration, max. 30 is always enough (s.a.) */

    P = sqrt(X*X+Y*Y);
    RR = sqrt(X*X+Y*Y+Z*Z);

/*	special cases for latitude and longitude */
    if (P/gi->Geocent_a < genau) {

/*  special case, if P=0. (X=0., Y=0.) */
	*Longitude = 0.;

/*  if (X,Y,Z)=(0.,0.,0.) then Height becomes semi-minor axis
 *  of ellipsoid (=center of mass), Latitude becomes PI/2 */
        if (RR/gi->Geocent_a < genau) {
            *Latitude = PI_OVER_2;
            *Height   = -gi->Geocent_b;
            return ;

        }
    }
    else {
/*  ellipsoidal (geodetic) longitude
 *  interval: -PI < Longitude <= +PI */
        *Longitude=atan2(Y,X);
    }

/* --------------------------------------------------------------
 * Following iterative algorithm was developed by
 * "Institut für Erdmessung", University of Hannover, July 1988.
 * Internet: www.ife.uni-hannover.de
 * Iterative computation of CPHI,SPHI and Height.
 * Iteration of CPHI and SPHI to 10**-12 radian resp.
 * 2*10**-7 arcsec.
 * --------------------------------------------------------------
 */
    CT = Z/RR;
    ST = P/RR;
    {
        const double denominator = 1.0-gi->Geocent_e2*(2.0-gi->Geocent_e2)*ST*ST;
        if( denominator == 0 )
        {
            *Latitude = HUGE_VAL;
            *Longitude = HUGE_VAL;
            *Height = HUGE_VAL;
            return;
        }
        RX = 1.0/sqrt(denominator);
    }
    CPHI0 = ST*(1.0-gi->Geocent_e2)*RX;
    SPHI0 = CT*RX;
    iter = 0;

/* loop to find sin(Latitude) resp. Latitude
 * until |sin(Latitude(iter)-Latitude(iter-1))| < genau */
    do
    {
        iter++;
        RN = gi->Geocent_a/sqrt(1.0-gi->Geocent_e2*SPHI0*SPHI0);

/*  ellipsoidal (geodetic) height */
        *Height = P*CPHI0+Z*SPHI0-RN*(1.0-gi->Geocent_e2*SPHI0*SPHI0);

        /* avoid zero division */
        if (RN+*Height==0.0) {
            *Latitude = 0.0;
            return;
        }
        RK = gi->Geocent_e2*RN/(RN+*Height);
        {
            const double denominator = 1.0-RK*(2.0-RK)*ST*ST;
            if( denominator == 0 )
            {
                *Latitude = HUGE_VAL;
                *Longitude = HUGE_VAL;
                *Height = HUGE_VAL;
                return;
            }
            RX = 1.0/sqrt(denominator);
        }
        CPHI = ST*(1.0-RK)*RX;
        SPHI = CT*RX;
        SDPHI = SPHI*CPHI0-CPHI*SPHI0;
        CPHI0 = CPHI;
        SPHI0 = SPHI;
    }
    while (SDPHI*SDPHI > genau2 && iter < maxiter);

/*	ellipsoidal (geodetic) latitude */
    *Latitude=atan2(SPHI, fabs(CPHI));

#endif /* defined(USE_ITERATIVE_METHOD) */
} /* END OF Convert_Geocentric_To_Geodetic */


///////////////////////////////////////////////////////////////////////////////
/// Main of transform.cpp
///////////////////////////////////////////////////////////////////////////////


static int adjust_axis( projCtx ctx, const char *axis, int denormalize_flag,
                           long point_count, int point_offset,
                           double *x, double *y, double *z );

#ifndef SRS_WGS84_SEMIMAJOR
#define SRS_WGS84_SEMIMAJOR 6378137.0
#endif

#ifndef SRS_WGS84_ESQUARED
#define SRS_WGS84_ESQUARED 0.0066943799901413165
#endif

#define Dx_BF (defn->datum_params[0])
#define Dy_BF (defn->datum_params[1])
#define Dz_BF (defn->datum_params[2])
#define Rx_BF (defn->datum_params[3])
#define Ry_BF (defn->datum_params[4])
#define Rz_BF (defn->datum_params[5])
#define M_BF  (defn->datum_params[6])

/*
** This table is intended to indicate for any given error code
** whether that error will occur for all locations (ie.
** it is a problem with the coordinate system as a whole) in which case the
** value would be 0, or if the problem is with the point being transformed
** in which case the value is 1.
**
** At some point we might want to move this array in with the error message
** list or something, but while experimenting with it this should be fine.
**
**
** NOTE (2017-10-01): Non-transient errors really should have resulted in a
** PJ==0 during initialization, and hence should be handled at the level
** before calling pj_transform. The only obvious example of the contrary
** appears to be the PJD_ERR_GRID_AREA case, which may also be taken to
** mean "no grids available"
**
**
*/

static const int transient_error[70] = {
    /*             0  1  2  3  4  5  6  7  8  9   */
    /* 0 to 9 */   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    /* 10 to 19 */ 0, 0, 0, 0, 1, 1, 0, 1, 1, 1,
    /* 20 to 29 */ 1, 0, 0, 0, 0, 0, 0, 1, 0, 0,
    /* 30 to 39 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    /* 40 to 49 */ 0, 0, 0, 0, 0, 0, 0, 0, 1, 0,
    /* 50 to 59 */ 1, 0, 1, 0, 1, 1, 1, 1, 0, 0,
    /* 60 to 69 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,};


/* -------------------------------------------------------------------- */
/*      Read transient_error[] in a safe way.                           */
/* -------------------------------------------------------------------- */
static int get_transient_error_value(int pos_index)
{
    const int array_size =
        (int)(sizeof(transient_error) / sizeof(transient_error[0]));
    if( pos_index < 0 || pos_index >= array_size ) {
        return 0;
    }
    return transient_error[pos_index];
}


/* -------------------------------------------------------------------- */
/*      Transform unusual input coordinate axis orientation to          */
/*      standard form if needed.                                        */
/* -------------------------------------------------------------------- */
static int adjust_axes (PJ *P, PJ_DIRECTION dir, long n, int dist, double *x, double *y, double *z) {
    /* Nothing to do? */
    if (0==strcmp(P->axis,"enu"))
        return 0;

    return adjust_axis( P->ctx, P->axis,
                dir==PJ_FWD ? 1: 0, n, dist, x, y, z );
}



/* ----------------------------------------------------------------------- */
/*    Transform geographic (lat/long) source coordinates to                */
/*    cartesian ("geocentric"), if needed                                  */
/* ----------------------------------------------------------------------- */
static int geographic_to_cartesian (PJ *P, PJ_DIRECTION dir, long n, int dist, double *x, double *y, double *z) {
    int res;
    long i;
    double fac = P->to_meter;

    /* Nothing to do? */
    if (!P->is_geocent)
        return 0;

    if ( z == nullptr ) {
        pj_ctx_set_errno( pj_get_ctx(P), PJD_ERR_GEOCENTRIC);
        return PJD_ERR_GEOCENTRIC;
    }

    if (PJ_FWD==dir) {
        fac = P->fr_meter;
        res = pj_geodetic_to_geocentric( P->a_orig, P->es_orig, n, dist, x, y, z );
        if (res)
            return res;
    }

    if (fac != 1.0) {
        for( i = 0; i < n; i++ ) {
            if( x[dist*i] != HUGE_VAL ) {
                x[dist*i] *= fac;
                y[dist*i] *= fac;
                z[dist*i] *= fac;
            }
        }
    }

    if (PJ_FWD==dir)
        return 0;
    return pj_geocentric_to_geodetic(
        P->a_orig, P->es_orig,
        n, dist,
        x, y, z
    );
}










/* -------------------------------------------------------------------- */
/*      Transform destination points to projection coordinates, if      */
/*      desired.                                                        */
/*                                                                      */
/*      Ought to fold this into projected_to_geographic                 */
/* -------------------------------------------------------------------- */
static int geographic_to_projected (PJ *P, long n, int dist, double *x, double *y, double *z) {
    long i;

    /* Nothing to do? */
    if (P->is_latlong && !P->geoc && P->vto_meter == 1.0)
        return 0;
    if (P->is_geocent)
        return 0;

    if(P->fwd3d != nullptr && !(z == nullptr && P->is_latlong))
    {
        /* Three dimensions must be defined */
        if ( z == nullptr)
        {
            pj_ctx_set_errno( pj_get_ctx(P), PJD_ERR_GEOCENTRIC);
            return PJD_ERR_GEOCENTRIC;
        }

        for( i = 0; i < n; i++ )
        {
            PJ_XYZ projected_loc;
            PJ_LPZ geodetic_loc;

            geodetic_loc.lam = x[dist*i];
            geodetic_loc.phi = y[dist*i];
            geodetic_loc.z = z[dist*i];

            if (geodetic_loc.lam == HUGE_VAL)
                continue;

            proj_errno_reset( P );
            projected_loc = pj_fwd3d( geodetic_loc, P);
            if( P->ctx->last_errno != 0 )
            {
                if( (P->ctx->last_errno != EDOM
                        && P->ctx->last_errno != ERANGE)
                    && (P->ctx->last_errno > 0
                        || P->ctx->last_errno < -44 || n == 1
                        || get_transient_error_value(-P->ctx->last_errno) == 0 ) )
                {
                    return P->ctx->last_errno;
                }
                else
                {
                    projected_loc.x = HUGE_VAL;
                    projected_loc.y = HUGE_VAL;
                    projected_loc.z = HUGE_VAL;
                }
            }

            x[dist*i] = projected_loc.x;
            y[dist*i] = projected_loc.y;
            z[dist*i] = projected_loc.z;
        }
        return 0;
    }

    // Ugly hack. See https://github.com/OSGeo/PROJ/issues/1782
    if( P->right == PJ_IO_UNITS_WHATEVER && P->descr &&
        strncmp(P->descr, "General Oblique Transformation",
                strlen("General Oblique Transformation")) == 0 )
    {
        P->right = PJ_IO_UNITS_PROJECTED;
    }

    for( i = 0; i <n; i++ )
    {
        PJ_XY         projected_loc;
        PJ_LP	       geodetic_loc;

        geodetic_loc.lam = x[dist*i];
        geodetic_loc.phi = y[dist*i];

        if( geodetic_loc.lam == HUGE_VAL )
            continue;

        proj_errno_reset( P );
        projected_loc = pj_fwd( geodetic_loc, P );
        if( P->ctx->last_errno != 0 )
        {
            if( (P->ctx->last_errno != EDOM
                    && P->ctx->last_errno != ERANGE)
                && (P->ctx->last_errno > 0
                    || P->ctx->last_errno < -44 || n == 1
                    || get_transient_error_value(-P->ctx->last_errno) == 0 ) )
            {
                return P->ctx->last_errno;
            }
            else
            {
                projected_loc.x = HUGE_VAL;
                projected_loc.y = HUGE_VAL;
            }
        }

        x[dist*i] = projected_loc.x;
        y[dist*i] = projected_loc.y;
    }
    return 0;
}





/* ----------------------------------------------------------------------- */
/*    Transform projected source coordinates to lat/long, if needed        */
/* ----------------------------------------------------------------------- */
static int projected_to_geographic (PJ *P, long n, int dist, double *x, double *y, double *z) {
    long i;

    /* Nothing to do? */
    if (P->is_latlong && !P->geoc && P->vto_meter == 1.0)
        return 0;
    if (P->is_geocent)
        return 0;

    /* Check first if projection is invertible. */
    if( (P->inv3d == nullptr) && (P->inv == nullptr))
    {
        pj_ctx_set_errno(pj_get_ctx(P), PJD_ERR_NON_CONV_INV_MERI_DIST);
        pj_log( pj_get_ctx(P), PJ_LOG_ERROR,
                "pj_transform(): source projection not invertable" );
        return PJD_ERR_NON_CONV_INV_MERI_DIST;
    }

    /* If invertible - First try inv3d if defined */
    if (P->inv3d != nullptr && !(z == nullptr && P->is_latlong))
    {
        /* Three dimensions must be defined */
        if ( z == nullptr)
        {
            pj_ctx_set_errno( pj_get_ctx(P), PJD_ERR_GEOCENTRIC);
            return PJD_ERR_GEOCENTRIC;
        }

        for (i=0; i < n; i++)
        {
            PJ_XYZ projected_loc;
            PJ_LPZ geodetic_loc;

            projected_loc.x = x[dist*i];
            projected_loc.y = y[dist*i];
            projected_loc.z = z[dist*i];

            if (projected_loc.x == HUGE_VAL)
                continue;

            proj_errno_reset( P );
            geodetic_loc = pj_inv3d(projected_loc, P);
            if( P->ctx->last_errno != 0 )
            {
                if( (P->ctx->last_errno != EDOM
                        && P->ctx->last_errno != ERANGE)
                    && (P->ctx->last_errno > 0
                        || P->ctx->last_errno < -44 || n == 1
                        || get_transient_error_value(-P->ctx->last_errno) == 0 ) )
                {
                    return P->ctx->last_errno;
                }
                else
                {
                    geodetic_loc.lam = HUGE_VAL;
                    geodetic_loc.phi = HUGE_VAL;
                    geodetic_loc.z = HUGE_VAL;
                }
            }

            x[dist*i] = geodetic_loc.lam;
            y[dist*i] = geodetic_loc.phi;
            z[dist*i] = geodetic_loc.z;

        }
        return 0;
    }

    // Ugly hack. See https://github.com/OSGeo/PROJ/issues/1782
    if( P->right == PJ_IO_UNITS_WHATEVER && P->descr &&
        strncmp(P->descr, "General Oblique Transformation",
                strlen("General Oblique Transformation")) == 0 )
    {
        P->right = PJ_IO_UNITS_PROJECTED;
    }

    /* Fallback to the original PROJ.4 API 2d inversion - inv */
    for( i = 0; i < n; i++ ) {
        PJ_XY         projected_loc;
        PJ_LP	       geodetic_loc;

        projected_loc.x = x[dist*i];
        projected_loc.y = y[dist*i];

        if( projected_loc.x == HUGE_VAL )
            continue;

        proj_errno_reset( P );
        geodetic_loc = pj_inv( projected_loc, P );
        if( P->ctx->last_errno != 0 )
        {
            if( (P->ctx->last_errno != EDOM
                    && P->ctx->last_errno != ERANGE)
                && (P->ctx->last_errno > 0
                    || P->ctx->last_errno < -44 || n == 1
                    || get_transient_error_value(-P->ctx->last_errno) == 0 ) )
            {
                return P->ctx->last_errno;
            }
            else
            {
                geodetic_loc.lam = HUGE_VAL;
                geodetic_loc.phi = HUGE_VAL;
            }
        }

        x[dist*i] = geodetic_loc.lam;
        y[dist*i] = geodetic_loc.phi;
    }
    return 0;
}



/* -------------------------------------------------------------------- */
/*            Adjust for the prime meridian if needed.                  */
/* -------------------------------------------------------------------- */
static int prime_meridian (PJ *P, PJ_DIRECTION dir, long n, int dist, double *x) {
    int i;
    double pm = P->from_greenwich;

    /* Nothing to do? */
    if (pm==0.0)
        return 0;
    if (!(P->is_geocent || P->is_latlong))
        return 0;

    if (dir==PJ_FWD)
        pm = -pm;

    for (i = 0;  i < n;  i++)
        if (x[dist*i] != HUGE_VAL)
            x[dist*i] += pm;

    return 0;
}



/* -------------------------------------------------------------------- */
/*            Adjust for vertical scale factor if needed                */
/* -------------------------------------------------------------------- */
static int height_unit (PJ *P, PJ_DIRECTION dir, long n, int dist, double *z) {
    int i;
    double fac = P->vto_meter;

    if (PJ_FWD==dir)
        fac = P->vfr_meter;

    /* Nothing to do? */
    if (fac==1.0)
        return 0;
    if (nullptr==z)
        return 0;
    if (P->is_latlong)
        return 0; /* done in pj_inv3d() / pj_fwd3d() */

    for (i = 0;  i < n;  i++)
        if (z[dist*i] != HUGE_VAL )
            z[dist*i] *= fac;

    return 0;
}


/************************************************************************/
/*                        pj_apply_vgridshift()                         */
/*                                                                      */
/*      This implementation takes uses the gridlist from a coordinate   */
/*      system definition.  If the gridlist has not yet been            */
/*      populated in the coordinate system definition we set it up      */
/*      now.                                                            */
/************************************************************************/
static int pj_apply_vgridshift( PJ *defn,
                         int inverse,
                         long point_count, int point_offset,
                         double *x, double *y, double *z )

{
    if( defn->vgrids_legacy == nullptr )
    {
        defn->vgrids_legacy = new ListOfVGrids;
        auto vgrids = pj_vgrid_init(defn, "geoidgrids");
        if( vgrids.empty() )
            return 0;
        *static_cast<ListOfVGrids*>(defn->vgrids_legacy) = std::move(vgrids);
    }
    if( static_cast<ListOfVGrids*>(defn->vgrids_legacy)->empty() )
    {
        return 0;
    }

    for( int i = 0; i < point_count; i++ )
    {
        double value;
        long io = i * point_offset;
        PJ_LP   input;

        input.phi = y[io];
        input.lam = x[io];

        value = pj_vgrid_value(defn, *static_cast<ListOfVGrids*>(defn->vgrids_legacy), input, 1.0);

        if( inverse )
            z[io] -= value;
        else
            z[io] += value;

        if( value == HUGE_VAL )
        {
            std::string gridlist;

            proj_log_debug(defn,
                "pj_apply_vgridshift(): failed to find a grid shift table for\n"
                "                       location (%.7fdW,%.7fdN)",
                x[io] * RAD_TO_DEG,
                y[io] * RAD_TO_DEG );

            for( const auto& gridset: *static_cast<ListOfVGrids*>(defn->vgrids_legacy) )
            {
                if( gridlist.empty() )
                    gridlist += "   tried: ";
                else
                    gridlist += ',';
                gridlist += gridset->name();
            }

            proj_log_debug(defn, "%s", gridlist.c_str());
            pj_ctx_set_errno( defn->ctx, PJD_ERR_GRID_AREA );

            return PJD_ERR_GRID_AREA;
        }
    }

    return 0;
}


/* -------------------------------------------------------------------- */
/*           Transform to ellipsoidal heights if needed                 */
/* -------------------------------------------------------------------- */
static int geometric_to_orthometric (PJ *P, PJ_DIRECTION dir, long n, int dist, double *x, double *y, double *z) {
    int err;
    if (0==P->has_geoid_vgrids)
        return 0;
    if (z==nullptr)
        return PJD_ERR_GEOCENTRIC;
    err = pj_apply_vgridshift (P, dir==PJ_FWD ? 1 : 0, n, dist, x, y, z );
    if (err)
        return pj_ctx_get_errno(P->ctx);
    return 0;
}



/* -------------------------------------------------------------------- */
/*      Convert datums if needed, and possible.                         */
/* -------------------------------------------------------------------- */
static int datum_transform (PJ *P, PJ *Q, long n, int dist, double *x, double *y, double *z) {
    if (0==pj_datum_transform (P, Q, n, dist, x, y, z))
        return 0;
    if (P->ctx->last_errno)
        return P->ctx->last_errno;
    return Q->ctx->last_errno;
}





/* -------------------------------------------------------------------- */
/*      If a wrapping center other than 0 is provided, rewrap around    */
/*      the suggested center (for latlong coordinate systems only).     */
/* -------------------------------------------------------------------- */
static int long_wrap (PJ *P, long n, int dist, double *x) {
    long i;

    /* Nothing to do? */
    if (P->is_geocent)
        return 0;
    if (!P->is_long_wrap_set)
        return 0;
    if (!P->is_latlong)
        return 0;

    for (i = 0;  i < n;  i++ ) {
        double val = x[dist*i];
        if (val == HUGE_VAL)
            continue;

        /* Get fast in ] -2 PI, 2 PI [ range */
        val = fmod(val, M_TWOPI);
        while( val < P->long_wrap_center - M_PI )
            val += M_TWOPI;
        while( val > P->long_wrap_center + M_PI )
            val -= M_TWOPI;
        x[dist*i] = val;
    }
    return 0;
}



/************************************************************************/
/*                            pj_transform()                            */
/*                                                                      */
/*      Currently this function doesn't recognise if two projections    */
/*      are identical (to short circuit reprojection) because it is     */
/*      difficult to compare PJ structures (since there are some        */
/*      projection specific components).                                */
/************************************************************************/

int pj_transform(
    PJ *src, PJ *dst,
    long point_count, int point_offset,
    double *x, double *y, double *z
){
    int       err;

    src->ctx->last_errno = 0;
    dst->ctx->last_errno = 0;

    if( point_offset == 0 )
        point_offset = 1;

    /* Bring input to "normal form": longitude, latitude, ellipsoidal height */

    err = adjust_axes (src, PJ_INV, point_count, point_offset, x, y, z);
    if (err)
        return err;
    err = geographic_to_cartesian (src, PJ_INV, point_count, point_offset, x, y, z);
    if (err)
        return err;
    err = projected_to_geographic (src, point_count, point_offset, x, y, z);
    if (err)
        return err;
    err = prime_meridian (src, PJ_INV, point_count, point_offset, x);
    if (err)
        return err;
    err = height_unit (src, PJ_INV, point_count, point_offset, z);
    if (err)
        return err;
    err = geometric_to_orthometric (src, PJ_INV, point_count, point_offset, x, y, z);
    if (err)
        return err;

    /* At the center of the process we do the datum shift (if needed) */

    err = datum_transform(src, dst, point_count, point_offset, x, y, z );
    if (err)
        return err;

    /* Now get out on the other side: Bring "normal form" to output form */

    err = geometric_to_orthometric (dst, PJ_FWD, point_count, point_offset, x, y, z);
    if (err)
        return err;
    err = height_unit (dst, PJ_FWD, point_count, point_offset, z);
    if (err)
        return err;
    err = prime_meridian (dst, PJ_FWD, point_count, point_offset, x);
    if (err)
        return err;
    err = geographic_to_cartesian (dst, PJ_FWD, point_count, point_offset, x, y, z);
    if (err)
        return err;
    err = geographic_to_projected (dst, point_count, point_offset, x, y, z);
    if (err)
        return err;
    err = long_wrap (dst, point_count, point_offset, x);
    if (err)
        return err;
    err = adjust_axes (dst, PJ_FWD, point_count, point_offset, x, y, z);
    if (err)
        return err;

    return 0;
}



/************************************************************************/
/*                     pj_geodetic_to_geocentric()                      */
/************************************************************************/

int pj_geodetic_to_geocentric( double a, double es,
                               long point_count, int point_offset,
                               double *x, double *y, double *z )

{
    double b;
    int    i;
    GeocentricInfo gi;
    int ret_errno = 0;

    if( es == 0.0 )
        b = a;
    else
        b = a * sqrt(1-es);

    if( pj_Set_Geocentric_Parameters( &gi, a, b ) != 0 )
    {
        return PJD_ERR_GEOCENTRIC;
    }

    for( i = 0; i < point_count; i++ )
    {
        long io = i * point_offset;

        if( x[io] == HUGE_VAL  )
            continue;

        if( pj_Convert_Geodetic_To_Geocentric( &gi, y[io], x[io], z[io],
                                               x+io, y+io, z+io ) != 0 )
        {
            ret_errno = PJD_ERR_LAT_OR_LON_EXCEED_LIMIT;
            x[io] = y[io] = HUGE_VAL;
            /* but keep processing points! */
        }
    }

    return ret_errno;
}

/************************************************************************/
/*                     pj_geocentric_to_geodetic()                      */
/************************************************************************/

int pj_geocentric_to_geodetic( double a, double es,
                               long point_count, int point_offset,
                               double *x, double *y, double *z )

{
    double b;
    int    i;
    GeocentricInfo gi;

    if( es == 0.0 )
        b = a;
    else
        b = a * sqrt(1-es);

    if( pj_Set_Geocentric_Parameters( &gi, a, b ) != 0 )
    {
        return PJD_ERR_GEOCENTRIC;
    }

    for( i = 0; i < point_count; i++ )
    {
        long io = i * point_offset;

        if( x[io] == HUGE_VAL )
            continue;

        pj_Convert_Geocentric_To_Geodetic( &gi, x[io], y[io], z[io],
                                           y+io, x+io, z+io );
    }

    return 0;
}

/************************************************************************/
/*                         pj_compare_datums()                          */
/*                                                                      */
/*      Returns TRUE if the two datums are identical, otherwise         */
/*      FALSE.                                                          */
/************************************************************************/

int pj_compare_datums( PJ *srcdefn, PJ *dstdefn )

{
    if( srcdefn->datum_type != dstdefn->datum_type )
    {
        return 0;
    }
    else if( srcdefn->a_orig != dstdefn->a_orig
             || ABS(srcdefn->es_orig - dstdefn->es_orig) > 0.000000000050 )
    {
        /* the tolerance for es is to ensure that GRS80 and WGS84 are
           considered identical */
        return 0;
    }
    else if( srcdefn->datum_type == PJD_3PARAM )
    {
        return (srcdefn->datum_params[0] == dstdefn->datum_params[0]
                && srcdefn->datum_params[1] == dstdefn->datum_params[1]
                && srcdefn->datum_params[2] == dstdefn->datum_params[2]);
    }
    else if( srcdefn->datum_type == PJD_7PARAM )
    {
        return (srcdefn->datum_params[0] == dstdefn->datum_params[0]
                && srcdefn->datum_params[1] == dstdefn->datum_params[1]
                && srcdefn->datum_params[2] == dstdefn->datum_params[2]
                && srcdefn->datum_params[3] == dstdefn->datum_params[3]
                && srcdefn->datum_params[4] == dstdefn->datum_params[4]
                && srcdefn->datum_params[5] == dstdefn->datum_params[5]
                && srcdefn->datum_params[6] == dstdefn->datum_params[6]);
    }
    else if( srcdefn->datum_type == PJD_GRIDSHIFT )
    {
        const char* srcnadgrids =
            pj_param(srcdefn->ctx, srcdefn->params,"snadgrids").s;
        const char* dstnadgrids =
            pj_param(dstdefn->ctx, dstdefn->params,"snadgrids").s;
        return srcnadgrids != nullptr && dstnadgrids != nullptr &&
               strcmp( srcnadgrids, dstnadgrids ) == 0;
    }
    else
        return 1;
}

/************************************************************************/
/*                       pj_geocentic_to_wgs84()                        */
/************************************************************************/

static
int pj_geocentric_to_wgs84( PJ *defn,
                            long point_count, int point_offset,
                            double *x, double *y, double *z )

{
    int       i;

    if( defn->datum_type == PJD_3PARAM )
    {
        for( i = 0; i < point_count; i++ )
        {
            long io = i * point_offset;

            if( x[io] == HUGE_VAL )
                continue;

            x[io] = x[io] + Dx_BF;
            y[io] = y[io] + Dy_BF;
            z[io] = z[io] + Dz_BF;
        }
    }
    else if( defn->datum_type == PJD_7PARAM )
    {
        for( i = 0; i < point_count; i++ )
        {
            long io = i * point_offset;
            double x_out, y_out, z_out;

            if( x[io] == HUGE_VAL )
                continue;

            x_out = M_BF*(       x[io] - Rz_BF*y[io] + Ry_BF*z[io]) + Dx_BF;
            y_out = M_BF*( Rz_BF*x[io] +       y[io] - Rx_BF*z[io]) + Dy_BF;
            z_out = M_BF*(-Ry_BF*x[io] + Rx_BF*y[io] +       z[io]) + Dz_BF;

            x[io] = x_out;
            y[io] = y_out;
            z[io] = z_out;
        }
    }

    return 0;
}

/************************************************************************/
/*                      pj_geocentic_from_wgs84()                       */
/************************************************************************/

static
int pj_geocentric_from_wgs84( PJ *defn,
                              long point_count, int point_offset,
                              double *x, double *y, double *z )

{
    int       i;

    if( defn->datum_type == PJD_3PARAM )
    {
        for( i = 0; i < point_count; i++ )
        {
            long io = i * point_offset;

            if( x[io] == HUGE_VAL )
                continue;

            x[io] = x[io] - Dx_BF;
            y[io] = y[io] - Dy_BF;
            z[io] = z[io] - Dz_BF;
        }
    }
    else if( defn->datum_type == PJD_7PARAM )
    {
        for( i = 0; i < point_count; i++ )
        {
            long io = i * point_offset;
            double x_tmp, y_tmp, z_tmp;

            if( x[io] == HUGE_VAL )
                continue;

            x_tmp = (x[io] - Dx_BF) / M_BF;
            y_tmp = (y[io] - Dy_BF) / M_BF;
            z_tmp = (z[io] - Dz_BF) / M_BF;

            x[io] =        x_tmp + Rz_BF*y_tmp - Ry_BF*z_tmp;
            y[io] = -Rz_BF*x_tmp +       y_tmp + Rx_BF*z_tmp;
            z[io] =  Ry_BF*x_tmp - Rx_BF*y_tmp +       z_tmp;
        }
    }

    return 0;
}


/************************************************************************/
/*                        pj_apply_gridshift_2()                        */
/*                                                                      */
/*      This implementation uses the gridlist from a coordinate         */
/*      system definition.  If the gridlist has not yet been            */
/*      populated in the coordinate system definition we set it up      */
/*      now.                                                            */
/************************************************************************/
static
int pj_apply_gridshift_2( PJ *defn, int inverse,
                          long point_count, int point_offset,
                          double *x, double *y, double * /*z*/ )

{
    if( defn->hgrids_legacy == nullptr )
    {
        defn->hgrids_legacy = new ListOfHGrids;
        auto hgrids = pj_hgrid_init(defn, "nadgrids");
        if( hgrids.empty() )
            return 0;
        *static_cast<ListOfHGrids*>(defn->hgrids_legacy) = std::move(hgrids);
    }
    if( static_cast<ListOfHGrids*>(defn->hgrids_legacy)->empty() )
    {
        return 0;
    }

    for( long i = 0; i < point_count; i++ )
    {
        PJ_LP   input;

        long io = i * point_offset;
        input.phi = y[io];
        input.lam = x[io];

        auto output = pj_hgrid_apply(defn->ctx, *static_cast<ListOfHGrids*>(defn->hgrids_legacy), input, inverse ? PJ_INV : PJ_FWD);

        if ( output.lam != HUGE_VAL )
        {
            y[io] = output.phi;
            x[io] = output.lam;
        }
        else
        {
            if( defn->ctx->debug_level >= PJ_LOG_DEBUG_MAJOR )
            {
                pj_log( defn->ctx, PJ_LOG_DEBUG_MAJOR,
                    "pj_apply_gridshift(): failed to find a grid shift table for\n"
                    "                      location (%.7fdW,%.7fdN)",
                    x[io] * RAD_TO_DEG,
                    y[io] * RAD_TO_DEG );
            }
        }
    }

    return 0;
}


/************************************************************************/
/*                         pj_datum_transform()                         */
/*                                                                      */
/*      The input should be long/lat/z coordinates in radians in the    */
/*      source datum, and the output should be long/lat/z               */
/*      coordinates in radians in the destination datum.                */
/************************************************************************/

int pj_datum_transform( PJ *src, PJ *dst,
                        long point_count, int point_offset,
                        double *x, double *y, double *z )

{
    double      src_a, src_es, dst_a, dst_es;
    int         z_is_temp = FALSE;

/* -------------------------------------------------------------------- */
/*      We cannot do any meaningful datum transformation if either      */
/*      the source or destination are of an unknown datum type          */
/*      (ie. only a +ellps declaration, no +datum).  This is new        */
/*      behavior for PROJ 4.6.0.                                        */
/* -------------------------------------------------------------------- */
    if( src->datum_type == PJD_UNKNOWN
        || dst->datum_type == PJD_UNKNOWN )
        return 0;

/* -------------------------------------------------------------------- */
/*      Short cut if the datums are identical.                          */
/* -------------------------------------------------------------------- */
    if( pj_compare_datums( src, dst ) )
        return 0;

    src_a = src->a_orig;
    src_es = src->es_orig;

    dst_a = dst->a_orig;
    dst_es = dst->es_orig;

/* -------------------------------------------------------------------- */
/*      Create a temporary Z array if one is not provided.              */
/* -------------------------------------------------------------------- */
    if( z == nullptr )
    {
        size_t	bytes = sizeof(double) * point_count * point_offset;
        z = (double *) pj_malloc(bytes);
        memset( z, 0, bytes );
        z_is_temp = TRUE;
    }

#define CHECK_RETURN(defn) {if( defn->ctx->last_errno != 0 && (defn->ctx->last_errno > 0 || get_transient_error_value(-defn->ctx->last_errno) == 0) ) { if( z_is_temp ) pj_dalloc(z); return defn->ctx->last_errno; }}

/* -------------------------------------------------------------------- */
/*	If this datum requires grid shifts, then apply it to geodetic   */
/*      coordinates.                                                    */
/* -------------------------------------------------------------------- */
    if( src->datum_type == PJD_GRIDSHIFT )
    {
        pj_apply_gridshift_2( src, 0, point_count, point_offset, x, y, z );
        CHECK_RETURN(src);

        src_a = SRS_WGS84_SEMIMAJOR;
        src_es = SRS_WGS84_ESQUARED;
    }

    if( dst->datum_type == PJD_GRIDSHIFT )
    {
        dst_a = SRS_WGS84_SEMIMAJOR;
        dst_es = SRS_WGS84_ESQUARED;
    }

/* ==================================================================== */
/*      Do we need to go through geocentric coordinates?                */
/* ==================================================================== */
    if( src_es != dst_es || src_a != dst_a
        || src->datum_type == PJD_3PARAM
        || src->datum_type == PJD_7PARAM
        || dst->datum_type == PJD_3PARAM
        || dst->datum_type == PJD_7PARAM)
    {
/* -------------------------------------------------------------------- */
/*      Convert to geocentric coordinates.                              */
/* -------------------------------------------------------------------- */
        src->ctx->last_errno =
            pj_geodetic_to_geocentric( src_a, src_es,
                                       point_count, point_offset, x, y, z );
        CHECK_RETURN(src);

/* -------------------------------------------------------------------- */
/*      Convert between datums.                                         */
/* -------------------------------------------------------------------- */
        if( src->datum_type == PJD_3PARAM
            || src->datum_type == PJD_7PARAM )
        {
            pj_geocentric_to_wgs84( src, point_count, point_offset,x,y,z);
            CHECK_RETURN(src);
        }

        if( dst->datum_type == PJD_3PARAM
            || dst->datum_type == PJD_7PARAM )
        {
            pj_geocentric_from_wgs84( dst, point_count,point_offset,x,y,z);
            CHECK_RETURN(dst);
        }

/* -------------------------------------------------------------------- */
/*      Convert back to geodetic coordinates.                           */
/* -------------------------------------------------------------------- */
        dst->ctx->last_errno =
            pj_geocentric_to_geodetic( dst_a, dst_es,
                                       point_count, point_offset, x, y, z );
        CHECK_RETURN(dst);
    }

/* -------------------------------------------------------------------- */
/*      Apply grid shift to destination if required.                    */
/* -------------------------------------------------------------------- */
    if( dst->datum_type == PJD_GRIDSHIFT )
    {
        pj_apply_gridshift_2( dst, 1, point_count, point_offset, x, y, z );
        CHECK_RETURN(dst);
    }

    if( z_is_temp )
        pj_dalloc( z );

    return 0;
}

/************************************************************************/
/*                           adjust_axis()                           */
/*                                                                      */
/*      Normalize or de-normalized the x/y/z axes.  The normal form     */
/*      is "enu" (easting, northing, up).                               */
/************************************************************************/
static int adjust_axis( projCtx ctx,
                           const char *axis, int denormalize_flag,
                           long point_count, int point_offset,
                           double *x, double *y, double *z )

{
    double x_in, y_in, z_in = 0.0;
    int i, i_axis;

    if( !denormalize_flag )
    {
        for( i = 0; i < point_count; i++ )
        {
            x_in = x[point_offset*i];
            y_in = y[point_offset*i];
            if( z )
                z_in = z[point_offset*i];

            for( i_axis = 0; i_axis < 3; i_axis++ )
            {
                double value;

                if( i_axis == 0 )
                    value = x_in;
                else if( i_axis == 1 )
                    value = y_in;
                else
                    value = z_in;

                switch( axis[i_axis] )
                {
                  case 'e':
                    x[point_offset*i] = value;
                    break;
                  case 'w':
                    x[point_offset*i] = -value;
                    break;
                  case 'n':
                    y[point_offset*i] = value;
                    break;
                  case 's':
                    y[point_offset*i] = -value;
                    break;
                  case 'u':
                    if( z )
                        z[point_offset*i] = value;
                    break;
                  case 'd':
                    if( z )
                        z[point_offset*i] = -value;
                    break;
                  default:
                    pj_ctx_set_errno( ctx, PJD_ERR_AXIS );
                    return PJD_ERR_AXIS;
                }
            } /* i_axis */
        } /* i (point) */
    }

    else /* denormalize */
    {
        for( i = 0; i < point_count; i++ )
        {
            x_in = x[point_offset*i];
            y_in = y[point_offset*i];
            if( z )
                z_in = z[point_offset*i];

            for( i_axis = 0; i_axis < 3; i_axis++ )
            {
                double *target;

                if( i_axis == 2 && z == nullptr )
                    continue;

                if( i_axis == 0 )
                    target = x;
                else if( i_axis == 1 )
                    target = y;
                else
                    target = z;

                switch( axis[i_axis] )
                {
                  case 'e':
                    target[point_offset*i] = x_in; break;
                  case 'w':
                    target[point_offset*i] = -x_in; break;
                  case 'n':
                    target[point_offset*i] = y_in; break;
                  case 's':
                    target[point_offset*i] = -y_in; break;
                  case 'u':
                    target[point_offset*i] = z_in; break;
                  case 'd':
                    target[point_offset*i] = -z_in; break;
                  default:
                    pj_ctx_set_errno( ctx, PJD_ERR_AXIS );
                    return PJD_ERR_AXIS;
                }
            } /* i_axis */
        } /* i (point) */
    }

    return 0;
}
// ---------------------------------------------------------------------------

void pj_deallocate_grids()
{
}
