/***********************************************************************

               Elemental datum shift operations

                 Thomas Knudsen, 2016-05-24

************************************************************************

**Elements**

STUB CODE

************************************************************************

Thomas Knudsen, thokn@sdfe.dk, 2016-05-24

************************************************************************
* Copyright (c) 2016, Thomas Knudsen / SDFE
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
#include <projects.h>
#include <geocent.h>
#include <PJ_pipeline.h>
#include <assert.h>
#include <stddef.h>
#include <errno.h>
PROJ_HEAD(add,     "Add a constant to the given coordinate (3 parameter shift)");
PROJ_HEAD(helmert, "7 parameter Helmert shift");


static void *freeup_msg (PJ *P, int errlev) {         /* Destructor */
    if (0==P)
        return 0;

    pj_ctx_set_errno (P->ctx, errlev);

    if (0==P->opaque)
        return pj_dealloc (P);

    pj_dealloc (P->opaque);
    return pj_dealloc(P);
}


/* Adapts pipeline_freeup to the format defined for the PJ object */
static void freeup (PJ *P) {
    freeup_msg (P, 0);
    return;
}











/* Projection specific elements for the "add" PJ object */
struct pj_opaque_add { XYZ xyz; };

static XYZ add_forward_3d (LPZ lpz, PJ *P) {
    struct pj_opaque_add *Q = (struct pj_opaque_add *) P->opaque;
    COORDINATE point;
    point.lpz = lpz;
    point.xyz.x += Q->xyz.x;
    point.xyz.y += Q->xyz.y;
    point.xyz.z += Q->xyz.z;
    return point.xyz;
}

static LPZ add_reverse_3d (XYZ xyz, PJ *P) {
    struct pj_opaque_add *Q = (struct pj_opaque_add *) P->opaque;
    COORDINATE point;
    point.xyz = xyz;
    point.xyz.x -= Q->xyz.x;
    point.xyz.y -= Q->xyz.y;
    point.xyz.z -= Q->xyz.z;
    return point.lpz;
}

static XY add_forward (LP lp, PJ *P) {
    struct pj_opaque_add *Q = (struct pj_opaque_add *) P->opaque;
    COORDINATE point;
    point.lp = lp;
    point.xy.x += Q->xyz.x;
    point.xy.y += Q->xyz.y;
    return point.xy;
}

static LP add_reverse (XY xy, PJ *P) {
    struct pj_opaque_add *Q = (struct pj_opaque_add *) P->opaque;
    COORDINATE point;
    point.xy = xy;
    point.xy.x -= Q->xyz.x;
    point.xy.y -= Q->xyz.y;
    return point.lp;
}


PJ *PROJECTION(add) {
    XYZ xyz = {0, 0, 0};

    struct pj_opaque_add *Q = pj_calloc (1, sizeof (struct pj_opaque_add));
    if (0==Q)
        return freeup_msg (P, ENOMEM);
    P->opaque = (void *) Q;

    P->fwd3d = add_forward_3d;
    P->inv3d = add_reverse_3d;
    P->fwd   = add_forward;
    P->inv   = add_reverse;
    pj_set_isomorphic (P);

    if ( (pj_param(P->ctx, P->params, "tx").i) ) {
        xyz.x = pj_param(P->ctx, P->params, "dx").f;
    }

    if ( (pj_param(P->ctx, P->params, "ty").i) ) {
        xyz.y = pj_param(P->ctx, P->params, "dy").f;
    }

    if ( (pj_param(P->ctx, P->params, "tz").i) ) {
        xyz.z = pj_param(P->ctx, P->params, "dz").f;
    }

    Q->xyz = xyz;
    return P;
}


/* selftest stub */
int pj_add_selftest (void) {return 0;}









/* Projection specific elements for the "helmert" PJ object */
struct pj_opaque_helmert {
    XYZ xyz;
    OPK opk;
    double scale;
};

static XYZ helmert_forward_3d (LPZ lpz, PJ *P) {
    struct pj_opaque_helmert *Q = (struct pj_opaque_helmert *) P->opaque;
    COORDINATE point;
    double X, Y, Z, rX, rY, rZ, scale;

    scale = 1 + Q->scale * 1e-6;

    X = lpz.lam;
    Y = lpz.phi;
    Z = lpz.z;

    rX = Q->opk.o;
    rY = Q->opk.p;
    rZ = Q->opk.k;

    point.xyz = Q->xyz;
    point.xyz.x += scale * (      X   -   rZ * Y   +   rY * Z);
    point.xyz.y += scale * ( rZ * X   +        Y   -   rX * Z);
    point.xyz.z += scale * (-rY * X   +   rX * Y   +        Z);
    return point.xyz;
}

static LPZ helmert_reverse_3d (XYZ xyz, PJ *P) {
    struct pj_opaque_helmert *Q = (struct pj_opaque_helmert *) P->opaque;
    COORDINATE point;
    double X, Y, Z, rX, rY, rZ, scale;

    scale = 1 - Q->scale * 1e-6;

    X = xyz.x;
    Y = xyz.y;
    Z = xyz.z;

    rX = -Q->opk.o;
    rY = -Q->opk.p;
    rZ = -Q->opk.k;

    point.xyz.x = -Q->xyz.x;
    point.xyz.y = -Q->xyz.y;
    point.xyz.z = -Q->xyz.z;

    point.xyz.x += scale * (      X   -   rZ * Y   +   rY * Z);
    point.xyz.y += scale * ( rZ * X   +        Y   -   rX * Z);
    point.xyz.z += scale * (-rY * X   +   rX * Y   +        Z);

    return point.lpz;
}

static XY helmert_forward (LP lp, PJ *P) {
    COORDINATE point;
    point.lp = lp;
    point.lpz.z = 0;
    point.xyz = helmert_forward_3d (point.lpz, P);
    return point.xy;
}

static LP helmert_reverse (XY xy, PJ *P) {
    COORDINATE point;
    point.xy = xy;
    point.xyz.z = 0;
    point.lpz = helmert_reverse_3d (point.xyz, P);
    return point.lp;
}


/* Milliarcsecond to radians */
#define MAS_TO_RAD (DEG_TO_RAD / 3600000)


PJ *PROJECTION(helmert) {
    XYZ xyz = {0, 0, 0};
    OPK opk = {0, 0, 0};
    double scale = 0;

    struct pj_opaque_helmert *Q = pj_calloc (1, sizeof (struct pj_opaque_helmert));
    if (0==Q)
        return freeup_msg (P, ENOMEM);
    P->opaque = (void *) Q;

    P->fwd3d = helmert_forward_3d;
    P->inv3d = helmert_reverse_3d;
    P->fwd   = helmert_forward;
    P->inv   = helmert_reverse;
    pj_set_isomorphic (P);

    if ( (pj_param(P->ctx, P->params, "tx").i) ) {
        xyz.x = pj_param(P->ctx, P->params, "dx").f;
    }

    if ( (pj_param(P->ctx, P->params, "ty").i) ) {
        xyz.y = pj_param(P->ctx, P->params, "dy").f;
    }

    if ( (pj_param(P->ctx, P->params, "tz").i) ) {
        xyz.z = pj_param(P->ctx, P->params, "dz").f;
    }

    if ( (pj_param(P->ctx, P->params, "trx").i) ) {
        opk.o = pj_param(P->ctx, P->params, "drx").f * MAS_TO_RAD;
    }

    if ( (pj_param(P->ctx, P->params, "try").i) ) {
        opk.p = pj_param(P->ctx, P->params, "dry").f * MAS_TO_RAD;
    }

    if ( (pj_param(P->ctx, P->params, "trz").i) ) {
        opk.k = pj_param(P->ctx, P->params, "drz").f * MAS_TO_RAD;
    }

    if ( (pj_param(P->ctx, P->params, "ts").i) ) {
        scale = pj_param(P->ctx, P->params, "ds").f;
    }

    Q->xyz = xyz;
    Q->opk = opk;
    Q->scale = scale;
    return P;
}


/* selftest stub */
int pj_helmert_selftest (void) {return 0;}
