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
#include "geocent.h"
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
 *                              GLOBAL DECLARATIONS
 */
/* Ellipsoid parameters, default to WGS 84 */
double Geocent_a = 6378137.0;     /* Semi-major axis of ellipsoid in meters */
double Geocent_b = 6356752.3142;  /* Semi-minor axis of ellipsoid           */

double Geocent_a2 = 40680631590769.0;        /* Square of semi-major axis */
double Geocent_b2 = 40408299984087.05;       /* Square of semi-minor axis */
double Geocent_e2 = 0.0066943799901413800;   /* Eccentricity squared  */
double Geocent_ep2 = 0.00673949675658690300; /* 2nd eccentricity squared */
/*
 * These state variables are for optimization purposes.  The only function
 * that should modify them is Set_Geocentric_Parameters.
 */


/***************************************************************************/
/*
 *                              FUNCTIONS     
 */


long Set_Geocentric_Parameters (double a, 
                                double b) 
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
    Geocent_a = a;
    Geocent_b = b;
    Geocent_a2 = a * a;
    Geocent_b2 = b * b;
    Geocent_e2 = (Geocent_a2 - Geocent_b2) / Geocent_a2;
    Geocent_ep2 = (Geocent_a2 - Geocent_b2) / Geocent_b2;
  }
  return (Error_Code);
} /* END OF Set_Geocentric_Parameters */


void Get_Geocentric_Parameters (double *a, 
                                double *b)
{ /* BEGIN Get_Geocentric_Parameters */
/*
 * The function Get_Geocentric_Parameters returns the ellipsoid parameters
 * to be used in geocentric coordinate conversions.
 *
 *    a  : Semi-major axis, in meters.          (output)
 *    b  : Semi-minor axis, in meters.          (output)
 */

  *a = Geocent_a;
  *b = Geocent_b;
} /* END OF Get_Geocentric_Parameters */


long Convert_Geodetic_To_Geocentric (double Latitude,
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

  if ((Latitude < -PI_OVER_2) || (Latitude > PI_OVER_2))
  { /* Latitude out of range */
    Error_Code |= GEOCENT_LAT_ERROR;
  }
  if ((Longitude < -PI) || (Longitude > (2*PI)))
  { /* Longitude out of range */
    Error_Code |= GEOCENT_LON_ERROR;
  }
  if (!Error_Code)
  { /* no errors */
    if (Longitude > PI)
      Longitude -= (2*PI);
    Sin_Lat = sin(Latitude);
    Cos_Lat = cos(Latitude);
    Sin2_Lat = Sin_Lat * Sin_Lat;
    Rn = Geocent_a / (sqrt(1.0e0 - Geocent_e2 * Sin2_Lat));
    *X = (Rn + Height) * Cos_Lat * cos(Longitude);
    *Y = (Rn + Height) * Cos_Lat * sin(Longitude);
    *Z = ((Rn * (1 - Geocent_e2)) + Height) * Sin_Lat;

  }
  return (Error_Code);
} /* END OF Convert_Geodetic_To_Geocentric */

void Convert_Geocentric_To_Geodetic (double X,
                                     double Y, 
                                     double Z,
                                     double *Latitude,
                                     double *Longitude,
                                     double *Height)
{ /* BEGIN Convert_Geocentric_To_Geodetic */
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
 *
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
  T1 = Z + Geocent_b * Geocent_ep2 * Sin3_B0;
  Sum = W - Geocent_a * Geocent_e2 * Cos_B0 * Cos_B0 * Cos_B0;
  S1 = sqrt(T1*T1 + Sum * Sum);
  Sin_p1 = T1 / S1;
  Cos_p1 = Sum / S1;
  Rn = Geocent_a / sqrt(1.0 - Geocent_e2 * Sin_p1 * Sin_p1);
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
    *Height = Z / Sin_p1 + Rn * (Geocent_e2 - 1.0);
  }
  if (At_Pole == FALSE)
  {
    *Latitude = atan(Sin_p1 / Cos_p1);
  }
} /* END OF Convert_Geocentric_To_Geodetic */
