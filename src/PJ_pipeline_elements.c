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

#if 0
/* not used - left here as demo for how to do projection specific freeups */
static void *freeup_msg_add (PJ *P, int errlev) {         /* Destructor */
    if (0==P)
        return 0;
    pj_ctx_set_errno (P->ctx, errlev);

    if (0==P->opaque)
        return pj_dealloc (P);

    /* projection specific deallocation goes here */

    pj_dealloc (P->opaque);
    return pj_dealloc(P);
}


/* Adapts pipeline_freeup to the format defined for the PJ object */
static void freeup_msg_add (PJ *P) {
    freeup_new_add (P, 0);
    return;
}
#endif


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

    if ( (pj_param(P->ctx, P->params, "tx").i) ) {
        xyz.x = pj_param(P->ctx, P->params, "rx").f;
    }

    if ( (pj_param(P->ctx, P->params, "ty").i) ) {
        xyz.y = pj_param(P->ctx, P->params, "ry").f;
    }

    if ( (pj_param(P->ctx, P->params, "tz").i) ) {
        xyz.z = pj_param(P->ctx, P->params, "rz").f;
    }

    Q->xyz = xyz;
    return P;
}


/* selftest stub */
int pj_add_selftest (void) {return 0;}
