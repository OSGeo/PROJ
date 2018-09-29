/************************************************************************
* Copyright (c) 2018, Even Rouault <even.rouault at spatialys.com>
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
#include "projects.h"

PROJ_HEAD(geogoffset, "Geographic Offset");

struct pj_opaque_geogoffset {
    double dlon;
    double dlat;
    double dh;
};


static PJ_COORD forward_4d(PJ_COORD obs, PJ *P) {
    struct pj_opaque_geogoffset *Q = (struct pj_opaque_geogoffset *) P->opaque;
    /* offset coordinate */
    obs.lpz.lam += Q->dlon;
    obs.lpz.phi += Q->dlat;
    obs.lpz.z   += Q->dh;
    return obs;
}

static XYZ forward_3d(LPZ lpz, PJ *P) {
    PJ_COORD point = {{0,0,0,0}};
    point.lpz = lpz;
    return forward_4d(point, P).xyz;
}


static XY forward_2d(LP lp, PJ *P) {
    PJ_COORD point = {{0,0,0,0}};
    point.lp = lp;
    return forward_4d(point, P).xy;
}


static PJ_COORD reverse_4d(PJ_COORD obs, PJ *P) {
    struct pj_opaque_geogoffset *Q = (struct pj_opaque_geogoffset *) P->opaque;
    /* offset coordinate */
    obs.lpz.lam -= Q->dlon;
    obs.lpz.phi -= Q->dlat;
    obs.lpz.z   -= Q->dh;
    return obs;
}

static LPZ reverse_3d(XYZ xyz, PJ *P) {
    PJ_COORD point = {{0,0,0,0}};
    point.xyz = xyz;
    return reverse_4d(point, P).lpz;
}

static LP reverse_2d(XY xy, PJ *P) {
    PJ_COORD point = {{0,0,0,0}};
    point.xy = xy;
    return reverse_4d(point, P).lp;
}


/* Arcsecond to radians */
#define ARCSEC_TO_RAD (DEG_TO_RAD / 3600.0)


PJ *TRANSFORMATION(geogoffset,0 /* no need for ellipsoid */) {
    struct pj_opaque_geogoffset *Q = pj_calloc(1, sizeof(struct pj_opaque_geogoffset));
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
    Q->dlon = pj_param(P->ctx, P->params, "ddlon").f * ARCSEC_TO_RAD;
    Q->dlat = pj_param(P->ctx, P->params, "ddlat").f * ARCSEC_TO_RAD;
    Q->dh = pj_param(P->ctx, P->params, "ddh").f;

    return P;
}
