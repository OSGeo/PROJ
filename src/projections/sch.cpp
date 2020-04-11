/******************************************************************************
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

#define PJ_LIB__

#include <errno.h>
#include <math.h>

#include "proj.h"
#include "proj_internal.h"

namespace { // anonymous namespace
struct pj_opaque {
    double plat; /*Peg Latitude */
    double plon; /*Peg Longitude*/
    double phdg; /*Peg heading  */
    double h0;   /*Average altitude */
    double transMat[9];
    double xyzoff[3];
    double rcurv;
    PJ* cart;
    PJ* cart_sph;
};
} // anonymous namespace

PROJ_HEAD(sch, "Spherical Cross-track Height") "\n\tMisc\n\tplat_0= plon_0= phdg_0= [h_0=]";

static PJ_LPZ sch_inverse3d(PJ_XYZ xyz, PJ *P) {
    struct pj_opaque *Q = static_cast<struct pj_opaque*>(P->opaque);

    PJ_LPZ lpz;
    lpz.lam = xyz.x * (P->a / Q->rcurv);
    lpz.phi = xyz.y * (P->a / Q->rcurv);
    lpz.z = xyz.z;
    xyz  =  Q->cart_sph->fwd3d (lpz, Q->cart_sph);
    double temp[] = {xyz.x, xyz.y, xyz.z};

    /* Apply rotation */
    double pxyz[] = {
        Q->transMat[0] * temp[0] + Q->transMat[1] * temp[1] + Q->transMat[2] * temp[2],
        Q->transMat[3] * temp[0] + Q->transMat[4] * temp[1] + Q->transMat[5] * temp[2],
        Q->transMat[6] * temp[0] + Q->transMat[7] * temp[1] + Q->transMat[8] * temp[2]
    };

    /* Apply offset */
    pxyz[0] += Q->xyzoff[0];
    pxyz[1] += Q->xyzoff[1];
    pxyz[2] += Q->xyzoff[2];

    /* Convert geocentric coordinates to lat lon */
    xyz.x = pxyz[0];
    xyz.y = pxyz[1];
    xyz.z = pxyz[2];
    lpz  =  Q->cart->inv3d (xyz, Q->cart);
    return lpz;
}

static PJ_XYZ sch_forward3d(PJ_LPZ lpz, PJ *P) {
    struct pj_opaque *Q = static_cast<struct pj_opaque*>(P->opaque);

    /* Convert lat lon to geocentric coordinates */
    PJ_XYZ xyz  =  Q->cart->fwd3d (lpz, Q->cart);
    double temp[] = {xyz.x, xyz.y, xyz.z};

    /* Adjust for offset */
    temp[0] -= Q->xyzoff[0];
    temp[1] -= Q->xyzoff[1];
    temp[2] -= Q->xyzoff[2];


    /* Apply rotation */
    double pxyz[] = {
        Q->transMat[0] * temp[0] + Q->transMat[3] * temp[1] + Q->transMat[6] * temp[2],
        Q->transMat[1] * temp[0] + Q->transMat[4] * temp[1] + Q->transMat[7] * temp[2],
        Q->transMat[2] * temp[0] + Q->transMat[5] * temp[1] + Q->transMat[8] * temp[2]
    };

    /* Convert to local lat,lon */
    xyz.x = pxyz[0];
    xyz.y = pxyz[1];
    xyz.z = pxyz[2];
    lpz  =  Q->cart_sph->inv3d (xyz, Q->cart_sph);

    /* Scale by radius */
    xyz.x = lpz.lam * (Q->rcurv / P->a);
    xyz.y = lpz.phi * (Q->rcurv / P->a);
    xyz.z = lpz.z;

    return xyz;
}

static PJ *destructor (PJ *P, int errlev) {
    if (nullptr==P)
        return nullptr;

    auto Q = static_cast<struct pj_opaque*>(P->opaque);
    if( Q )
    {
        if (Q->cart)
            Q->cart->destructor (Q->cart, errlev);
        if (Q->cart_sph)
            Q->cart_sph->destructor (Q->cart_sph, errlev);
    }

    return pj_default_destructor(P, errlev);
}

static PJ *setup(PJ *P) { /* general initialization */
    struct pj_opaque *Q = static_cast<struct pj_opaque*>(P->opaque);

    /* Setup original geocentric system */
    // Pass a dummy ellipsoid definition that will be overridden just afterwards
    Q->cart = proj_create(P->ctx, "+proj=cart +a=1");
    if (Q->cart == nullptr)
        return destructor(P, ENOMEM);

    /* inherit ellipsoid definition from P to Q->cart */
    pj_inherit_ellipsoid_def (P, Q->cart);

    const double clt = cos(Q->plat);
    const double slt = sin(Q->plat);
    const double clo = cos(Q->plon);
    const double slo = sin(Q->plon);

    /* Estimate the radius of curvature for given peg */
    const double temp = sqrt(1.0 - (P->es) * slt * slt);
    const double reast = (P->a)/temp;
    const double rnorth = (P->a) * (1.0 - (P->es))/pow(temp,3);

    const double chdg = cos(Q->phdg);
    const double shdg = sin(Q->phdg);

    Q->rcurv = Q->h0 + (reast*rnorth)/(reast * chdg * chdg + rnorth * shdg * shdg);

    /* Set up local sphere at the given peg point */
    Q->cart_sph = proj_create(P->ctx, "+proj=cart +a=1");
    if (Q->cart_sph == nullptr)
        return destructor(P, ENOMEM);
    pj_calc_ellipsoid_params(Q->cart_sph, Q->rcurv, 0);

    /* Set up the transformation matrices */
    Q->transMat[0] = clt * clo;
    Q->transMat[1] = -shdg*slo - slt*clo * chdg;
    Q->transMat[2] =  slo*chdg - slt*clo*shdg;
    Q->transMat[3] =  clt*slo;
    Q->transMat[4] =  clo*shdg - slt*slo*chdg;
    Q->transMat[5] = -clo*chdg - slt*slo*shdg;
    Q->transMat[6] =  slt;
    Q->transMat[7] =  clt*chdg;
    Q->transMat[8] =  clt*shdg;

    PJ_LPZ lpz;
    lpz.lam = Q->plon;
    lpz.phi = Q->plat;
    lpz.z = Q->h0;
    PJ_XYZ xyz  =  Q->cart->fwd3d (lpz, Q->cart);
    Q->xyzoff[0] = xyz.x - (Q->rcurv) * clt * clo;
    Q->xyzoff[1] = xyz.y- (Q->rcurv) * clt * slo;
    Q->xyzoff[2] = xyz.z - (Q->rcurv) * slt;

    P->fwd3d = sch_forward3d;
    P->inv3d = sch_inverse3d;
    return P;
}


PJ *PROJECTION(sch) {
    struct pj_opaque *Q = static_cast<struct pj_opaque*>(pj_calloc (1, sizeof (struct pj_opaque)));
    if (nullptr==Q)
        return pj_default_destructor(P, ENOMEM);
    P->opaque = Q;
    P->destructor = destructor;

    Q->h0 = 0.0;

    /* Check if peg latitude was defined */
    if (pj_param(P->ctx, P->params, "tplat_0").i)
        Q->plat = pj_param(P->ctx, P->params, "rplat_0").f;
    else {
        return pj_default_destructor(P, PJD_ERR_FAILED_TO_FIND_PROJ);
    }

    /* Check if peg longitude was defined */
    if (pj_param(P->ctx, P->params, "tplon_0").i)
        Q->plon = pj_param(P->ctx, P->params, "rplon_0").f;
    else {
        return pj_default_destructor(P, PJD_ERR_FAILED_TO_FIND_PROJ);
    }

    /* Check if peg heading is defined */
    if (pj_param(P->ctx, P->params, "tphdg_0").i)
        Q->phdg = pj_param(P->ctx, P->params, "rphdg_0").f;
    else {
        return pj_default_destructor(P, PJD_ERR_FAILED_TO_FIND_PROJ);
    }


    /* Check if average height was defined - If so read it in */
    if (pj_param(P->ctx, P->params, "th_0").i)
        Q->h0 = pj_param(P->ctx, P->params, "dh_0").f;


    return setup(P);
}
