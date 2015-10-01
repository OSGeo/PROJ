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
        lpz.lam = xyz.x ;
        lpz.phi = xyz.y ;
        lpz.z = xyz.z;

//        printf("INVERSE: \n");
//        printf("XYZ: %f %f %f \n", xyz.x, xyz.y, xyz.z);
//        printf("LPZ: %f %f %f \n", lpz.lam, lpz.phi, lpz.z);
        return (lpz);
}

FORWARD3D(forward3d);

        //Scale by radius
        xyz.x = lpz.lam;
        xyz.y = lpz.phi;
        xyz.z = lpz.z;

//        printf("FORWARD: \n");
//        printf("LPZ: %f %f %f \n", lpz.lam, lpz.phi, lpz.z);
//        printf("XYZ: %f %f %f \n", xyz.x, xyz.y, xyz.z);

        return (xyz);
}

FREEUP; if (P) pj_dalloc(P); }
	static PJ *
setup(PJ *P) { /* general initialization */

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
