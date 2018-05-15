/***********************************************************************

                  (Abridged) Molodensky Transform

                    Kristian Evers, 2017-07-07

************************************************************************

    Implements the (abridged) Molodensky transformations for 2D and 3D
    data.

    Primarily useful for implementation of datum shifts in transformation
    pipelines.

    The code in this file is mostly based on

        The Standard and Abridged Molodensky Coordinate Transformation
        Formulae, 2004, R.E. Deaking,
        http://www.mygeodesy.id.au/documents/Molodensky%20V2.pdf



************************************************************************
* Copyright (c) 2017, Kristian Evers / SDFE
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
*
***********************************************************************/
#define PJ_LIB__

#include <errno.h>
#include <math.h>

#include "proj.h"
#include "proj_internal.h"
#include "projects.h"

PROJ_HEAD(molodensky, "Molodensky transform");

static XYZ forward_3d(LPZ lpz, PJ *P);
static LPZ reverse_3d(XYZ xyz, PJ *P);

struct pj_opaque_molodensky {
    double dx;
    double dy;
    double dz;
    double da;
    double df;
    int    abridged;
};


static double RN (double a, double es, double phi) {
/**********************************************************
    N(phi) - prime vertical radius of curvature
    -------------------------------------------

    This is basically the same function as in PJ_cart.c
    should probably be refactored into it's own file at some
    point.

**********************************************************/
    double s = sin(phi);
    if (es==0)
        return a;

    return a / sqrt (1 - es*s*s);
}


static double RM (double a, double es, double phi) {
/**********************************************************
    M(phi) - Meridian radius of curvature
    -------------------------------------

    Source:

        E.J Krakiwsky & D.B. Thomson, 1974,
        GEODETIC POSITION COMPUTATIONS,

        Fredericton NB, Canada:
        University of New Brunswick,
        Department of Geodesy and Geomatics Engineering,
        Lecture Notes No. 39,
        99 pp.

        http://www2.unb.ca/gge/Pubs/LN39.pdf

**********************************************************/
    double s = sin(phi);
    if (es==0)
        return a;

    /* eq. 13a */
    if (phi == 0)
        return a * (1-es);

    /* eq. 13b */
    if (fabs(phi) == M_PI_2)
        return a / sqrt(1-es);

    /* eq. 13 */
    return (a * (1 - es) ) / pow(1 - es*s*s, 1.5);

}


static LPZ calc_standard_params(LPZ lpz, PJ *P) {
    struct pj_opaque_molodensky *Q = (struct pj_opaque_molodensky *) P->opaque;
    double dphi, dlam, dh;

    /* sines and cosines */
    double slam = sin(lpz.lam);
    double clam = cos(lpz.lam);
    double sphi = sin(lpz.phi);
    double cphi = cos(lpz.phi);

    /* ellipsoid parameters and differences */
    double f = P->f, a = P->a;
    double dx = Q->dx, dy = Q->dy, dz = Q->dz;
    double da = Q->da, df = Q->df;

    /* ellipsoid radii of curvature */
    double rho = RM(a, P->es, lpz.phi);
    double nu  = RN(a, P->e2s, lpz.phi);

    /* delta phi */
    dphi  = (-dx*sphi*clam) - (dy*sphi*slam) + (dz*cphi)
            + ((nu * P->es * sphi * cphi * da) / a)
            + (sphi*cphi * ( rho/(1-f) + nu*(1-f))*df);
    dphi /= (rho + lpz.z);

    /* delta lambda */
    dlam = (-dx*slam + dy*clam) / ((nu+lpz.z)*cphi);

    /* delta h */
    dh = dx*cphi*clam + dy*cphi*slam + dz*sphi - (a/nu)*da + nu*(1-f)*sphi*sphi*df;

    lpz.phi = dphi;
    lpz.lam = dlam;
    lpz.z   = dh;

    return lpz;
}


static LPZ calc_abridged_params(LPZ lpz, PJ *P) {
    struct pj_opaque_molodensky *Q = (struct pj_opaque_molodensky *) P->opaque;
    double dphi, dlam, dh;

    /* sines and cosines */
    double slam = sin(lpz.lam);
    double clam = cos(lpz.lam);
    double sphi = sin(lpz.phi);
    double cphi = cos(lpz.phi);

    /* ellipsoid parameters and differences */
    double dx = Q->dx, dy = Q->dy, dz = Q->dz;
    double da = Q->da, df = Q->df;
    double adffda = (P->a*df + P->f*da);

    /* delta phi */
    dphi = -dx*sphi*clam - dy*sphi*slam + dz*cphi + adffda*sin(2*lpz.phi);
    dphi /= RM(P->a, P->es, lpz.phi);

    /* delta lambda */
    dlam = -dx*slam + dy*clam;
    dlam /= RN(P->a, P->e2s, lpz.phi)*cphi;

    /* delta h */
    dh = dx*cphi*clam + dy*cphi*slam + dz*sphi - da + adffda*sphi*sphi;

    /* offset coordinate */
    lpz.phi = dphi;
    lpz.lam = dlam;
    lpz.z   = dh;

    return lpz;
}


static XY forward_2d(LP lp, PJ *P) {
    PJ_COORD point = {{0,0,0,0}};

    point.lp = lp;
    point.xyz = forward_3d(point.lpz, P);

    return point.xy;
}


static LP reverse_2d(XY xy, PJ *P) {
    PJ_COORD point = {{0,0,0,0}};

    point.xy = xy;
    point.xyz.z = 0;
    point.lpz = reverse_3d(point.xyz, P);

    return point.lp;
}


static XYZ forward_3d(LPZ lpz, PJ *P) {
    struct pj_opaque_molodensky *Q = (struct pj_opaque_molodensky *) P->opaque;
    PJ_COORD point = {{0,0,0,0}};

    point.lpz = lpz;

    /* calculate parameters depending on the mode we are in */
    if (Q->abridged) {
        lpz = calc_abridged_params(lpz, P);
    } else {
        lpz = calc_standard_params(lpz, P);
    }

    /* offset coordinate */
    point.lpz.phi += lpz.phi;
    point.lpz.lam += lpz.lam;
    point.lpz.z   += lpz.z;

    return point.xyz;
}


static PJ_COORD forward_4d(PJ_COORD obs, PJ *P) {
    obs.xyz = forward_3d(obs.lpz, P);
    return obs;
}


static LPZ reverse_3d(XYZ xyz, PJ *P) {
    struct pj_opaque_molodensky *Q = (struct pj_opaque_molodensky *) P->opaque;
    PJ_COORD point = {{0,0,0,0}};
    LPZ lpz;

    /* calculate parameters depending on the mode we are in */
    point.xyz = xyz;
    if (Q->abridged)
        lpz = calc_abridged_params(point.lpz, P);
    else
        lpz = calc_standard_params(point.lpz, P);

    /* offset coordinate */
    point.lpz.phi -= lpz.phi;
    point.lpz.lam -= lpz.lam;
    point.lpz.z   -= lpz.z;

    return point.lpz;
}


static PJ_COORD reverse_4d(PJ_COORD obs, PJ *P) {
    obs.lpz = reverse_3d(obs.xyz, P);
    return obs;
}


PJ *TRANSFORMATION(molodensky,1) {
    struct pj_opaque_molodensky *Q = pj_calloc(1, sizeof(struct pj_opaque_molodensky));
    if (0==Q)
        return pj_default_destructor(P, ENOMEM);
    P->opaque = (void *) Q;

    P->fwd4d = forward_4d;
    P->inv4d = reverse_4d;
    P->fwd3d  = forward_3d;
    P->inv3d  = reverse_3d;
    P->fwd    = forward_2d;
    P->inv    = reverse_2d;

    P->left   = PJ_IO_UNITS_ANGULAR;
    P->right  = PJ_IO_UNITS_ANGULAR;

    /* read args */
    if (pj_param(P->ctx, P->params, "tdx").i)
        Q->dx = pj_param(P->ctx, P->params, "ddx").f;

    if (pj_param(P->ctx, P->params, "tdy").i)
        Q->dy = pj_param(P->ctx, P->params, "ddy").f;

    if (pj_param(P->ctx, P->params, "tdz").i)
        Q->dz = pj_param(P->ctx, P->params, "ddz").f;

    if (pj_param(P->ctx, P->params, "tda").i)
        Q->da = pj_param(P->ctx, P->params, "dda").f;

    if (pj_param(P->ctx, P->params, "tdf").i)
        Q->df = pj_param(P->ctx, P->params, "ddf").f;

    Q->abridged = pj_param(P->ctx, P->params, "tabridged").i;

    /* We want all parameters (except +abridged) to be set */
    if ((Q->dx == 0) && (Q->dy == 0) && (Q->dz == 0) && (Q->da == 0) && (Q->df == 0))
        return pj_default_destructor(P, PJD_ERR_NO_ARGS);

    if ((Q->dx == 0) || (Q->dy == 0) || (Q->dz == 0) || (Q->da == 0) || (Q->df == 0))
        return pj_default_destructor(P, PJD_ERR_MISSING_ARGS);

    return P;
}
