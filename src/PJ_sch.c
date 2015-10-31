/******************************************************************************
 * $Id$
 *
 * Project:  SCH Coordinate system
 * Purpose:  Implementation of SCH Coordinate system
 * References : 
 *      1. Hensley. Scott. SCH Coordinates and various transformations. June 15, 2000.  
 *      2. Buckley, Sean Monroe. Radar interferometry measurement of land subsidence. 2000..
 *         PhD Thesis. UT Austin. (Appendix)
 *      3. Hensley, Scott, Elaine Chapin, and T. Michel. "Improved processing of AIRSAR
 *         data based on the GeoSAR processor." Airsar earth science and applications
 *         workshop, March. 2002. (http://airsar.jpl.nasa.gov/documents/workshop2002/papers/T3.pdf)
 *
 * Author:   Piyush Agram (piyush.agram@jpl.nasa.gov)
 * Copyright (c) 2015 California Institute of Technology.
 * Government sponsorship acknowledged.
 *
 * NOTE:  The SCH coordinate system is a sensor aligned coordinate system 
 * developed at JPL for radar mapping missions. Details pertaining to the 
 * coordinate system have been release in the public domain (see references above).
 * This code is an independent implementation of the SCH coordinate system
 * that conforms to the PROJ.4 conventions and uses the details presented in these
 * publicly released documents. All credit for the development of the coordinate
 * system and its use should be directed towards the original developers at JPL.
 ******************************************************************************
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 ****************************************************************************/

#include "geocent.h"

#define PROJ_PARMS__ \
	double plat; /*Peg Latitude */    \
        double plon; /*Peg Longitude*/    \
        double phdg; /*Peg heading  */    \
        double h0;   /*Average altitude */\
	double transMat[9]; \
        double xyzoff[3]; \
        double rcurv; \
        GeocentricInfo sph; \
        GeocentricInfo elp_0; 
        
#define PJ_LIB__
#include	<projects.h>

PROJ_HEAD(sch, "Spherical Cross-track Height") "\n\tMisc\n\tplat_0 = ,plon_0 = , phdg_0 = ,[h_0 = ]";

INVERSE3D(inverse3d);
        double temp[3];
        double pxyz[3];

        //Local lat,lon using radius
        pxyz[0] = xyz.y * P->a / P->rcurv;
        pxyz[1] = xyz.x * P->a / P->rcurv;
        pxyz[2] = xyz.z;


        if( pj_Convert_Geodetic_To_Geocentric( &(P->sph), pxyz[0], pxyz[1], pxyz[2],
                    temp, temp+1, temp+2) != 0)
                I3_ERROR;

        //Apply rotation
        pxyz[0] = P->transMat[0] * temp[0] + P->transMat[1] * temp[1] + P->transMat[2] * temp[2];
        pxyz[1] = P->transMat[3] * temp[0] + P->transMat[4] * temp[1] + P->transMat[5] * temp[2];
        pxyz[2] = P->transMat[6] * temp[0] + P->transMat[7] * temp[1] + P->transMat[8] * temp[2];

        //Apply offset
        pxyz[0] += P->xyzoff[0];
        pxyz[1] += P->xyzoff[1];
        pxyz[2] += P->xyzoff[2];

        //Convert geocentric coordinates to lat lon
        pj_Convert_Geocentric_To_Geodetic( &(P->elp_0), pxyz[0], pxyz[1], pxyz[2],
                temp, temp+1, temp+2);


        lpz.lam = temp[1] ;
        lpz.phi = temp[0] ;
        lpz.z = temp[2];

//        printf("INVERSE: \n");
//        printf("XYZ: %f %f %f \n", xyz.x, xyz.y, xyz.z);
//        printf("LPZ: %f %f %f \n", lpz.lam, lpz.phi, lpz.z);
        return (lpz);
}

FORWARD3D(forward3d);
        double temp[3];
        double pxyz[3];


        //Convert lat lon to geocentric coordinates
        if( pj_Convert_Geodetic_To_Geocentric( &(P->elp_0), lpz.phi, lpz.lam, lpz.z,
                    temp, temp+1, temp+2 ) != 0 )
            F3_ERROR;


        //Adjust for offset
        temp[0] -= P->xyzoff[0];
        temp[1] -= P->xyzoff[1];
        temp[2] -= P->xyzoff[2];

       
        //Apply rotation 
        pxyz[0] = P->transMat[0] * temp[0] + P->transMat[3] * temp[1] + P->transMat[6] * temp[2];
        pxyz[1] = P->transMat[1] * temp[0] + P->transMat[4] * temp[1] + P->transMat[7] * temp[2];
        pxyz[2] = P->transMat[2] * temp[0] + P->transMat[5] * temp[1] + P->transMat[8] * temp[2];

        //Convert to local lat,lon
        pj_Convert_Geocentric_To_Geodetic( &(P->sph), pxyz[0], pxyz[1], pxyz[2],
                temp, temp+1, temp+2);


        //Scale by radius
        xyz.x = temp[1] * P->rcurv / P->a;
        xyz.y = temp[0] * P->rcurv / P->a;
        xyz.z = temp[2];

//        printf("FORWARD: \n");
//        printf("LPZ: %f %f %f \n", lpz.lam, lpz.phi, lpz.z);
//        printf("XYZ: %f %f %f \n", xyz.x, xyz.y, xyz.z);

        return (xyz);
}

FREEUP; if (P) pj_dalloc(P); }
	static PJ *
setup(PJ *P) { /* general initialization */
        double reast, rnorth;
        double chdg, shdg;
        double clt, slt;
        double clo, slo;
        double temp;
        double pxyz[3];

        temp = P->a * sqrt(1.0 - P->es);

        //Setup original geocentric system
        if ( pj_Set_Geocentric_Parameters(&(P->elp_0), P->a, temp) != 0)
                E_ERROR(-37);


        clt = cos(P->plat);
        slt = sin(P->plat);
        clo = cos(P->plon);
        slo = sin(P->plon);

        //Estimate the radius of curvature for given peg
        temp = sqrt(1.0 - (P->es) * slt * slt);
        reast = (P->a)/temp;
        rnorth = (P->a) * (1.0 - (P->es))/pow(temp,3);

        chdg = cos(P->phdg);
        shdg = sin(P->phdg);
        
        P->rcurv = P->h0 + (reast*rnorth)/(reast * chdg * chdg + rnorth * shdg * shdg);

//        printf("North Radius: %f \n", rnorth);
//        printf("East Radius: %f \n", reast);
//        printf("Effective Radius: %f \n", P->rcurv);

        //Set up local sphere at the given peg point
        if ( pj_Set_Geocentric_Parameters(&(P->sph), P->rcurv, P->rcurv) != 0)
            E_ERROR(-37);

        //Set up the transformation matrices
        P->transMat[0] = clt * clo; 
        P->transMat[1] = -shdg*slo - slt*clo * chdg;
        P->transMat[2] =  slo*chdg - slt*clo*shdg;
        P->transMat[3] =  clt*slo;
        P->transMat[4] =  clo*shdg - slt*slo*chdg;
        P->transMat[5] = -clo*chdg - slt*slo*shdg;
        P->transMat[6] =  slt;
        P->transMat[7] =  clt*chdg;
        P->transMat[8] =  clt*shdg;        

        
        if( pj_Convert_Geodetic_To_Geocentric( &(P->elp_0), P->plat, P->plon, P->h0, 
                                               pxyz, pxyz+1, pxyz+2 ) != 0 )
        {
            E_ERROR(-14)
        }


        P->xyzoff[0] = pxyz[0] - (P->rcurv) * clt * clo;
        P->xyzoff[1] = pxyz[1] - (P->rcurv) * clt * slo;
        P->xyzoff[2] = pxyz[2] - (P->rcurv) * slt;

//        printf("Offset: %f %f %f \n", P->xyzoff[0], P->xyzoff[1], P->xyzoff[2]);


        P->fwd3d = forward3d;
        P->inv3d = inverse3d;
	return P;
}
ENTRY0(sch)
        P->h0 = 0.0;

        //Check if peg latitude was defined
        if (pj_param(P->ctx, P->params, "tplat_0").i)
            P->plat = pj_param(P->ctx, P->params, "rplat_0").f;
        else
            E_ERROR(-37);

        //Check if peg longitude was defined
        if (pj_param(P->ctx, P->params, "tplon_0").i)
            P->plon = pj_param(P->ctx, P->params, "rplon_0").f;
        else
            E_ERROR(-37);
        
        //Check if peg latitude is defined
        if (pj_param(P->ctx, P->params, "tphdg_0").i)
            P->phdg = pj_param(P->ctx, P->params, "rphdg_0").f;
        else
            E_ERROR(-37);
        

        //Check if average height was defined
        //If so read it in
        if (pj_param(P->ctx, P->params, "th_0").i)
            P->h0 = pj_param(P->ctx, P->params, "dh_0").f;

        //Completed reading in the projection parameters
//        printf("PSA: Lat = %f Lon = %f Hdg = %f \n", P->plat, P->plon, P->phdg);

ENDENTRY(setup(P))
