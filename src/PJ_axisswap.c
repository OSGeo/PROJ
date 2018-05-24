/***********************************************************************

        Axis order operation for use with transformation pipelines.

                Kristian Evers, kreve@sdfe.dk, 2017-10-31

************************************************************************

Change the order and sign of 2,3 or 4 axes. Each of the possible four
axes are numbered with 1-4, such that the first input axis is 1, the
second is 2 and so on. The output ordering is controlled by a list of the
input axes re-ordered to the new mapping. Examples:

Reversing the order of the axes:

    +proj=axisswap +order=4,3,2,1

Swapping the first two axes (x and y):

    +proj=axisswap +order=2,1,3,4

The direction, or sign, of an axis can be changed by adding a minus in
front of the axis-number:

    +proj=axisswap +order=1,-2,3,4

It is only necessary to specify the axes that are affected by the swap
operation:

    +proj=axisswap +order=2,1

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
#include <stdlib.h>
#include <string.h>

#include "proj_internal.h"
#include "projects.h"

PROJ_HEAD(axisswap, "Axis ordering");

struct pj_opaque {
    unsigned int axis[4];
    int sign[4];
};

static int sign(int x) {
    return (x > 0) - (x < 0);
}

static XY forward_2d(LP lp, PJ *P) {
    struct pj_opaque *Q = (struct pj_opaque *) P->opaque;
    unsigned int i;
    PJ_COORD out, in;

    in.lp = lp;
    out = proj_coord_error();

    for (i=0; i<2; i++)
        out.v[i] = in.v[Q->axis[i]] * Q->sign[i];

    return out.xy;
}


static LP reverse_2d(XY xy, PJ *P) {
    struct pj_opaque *Q = (struct pj_opaque *) P->opaque;
    unsigned int i;
    PJ_COORD out, in;

    in.xy = xy;
    out = proj_coord_error();

    for (i=0; i<2; i++)
        out.v[Q->axis[i]] = in.v[i] * Q->sign[i];

    return out.lp;
}


static XYZ forward_3d(LPZ lpz, PJ *P) {
    struct pj_opaque *Q = (struct pj_opaque *) P->opaque;
    unsigned int i;
    PJ_COORD out, in;

    in.lpz = lpz;
    out = proj_coord_error();

    for (i=0; i<3; i++)
        out.v[i] = in.v[Q->axis[i]] * Q->sign[i];

    return out.xyz;
}

static LPZ reverse_3d(XYZ xyz, PJ *P) {
    struct pj_opaque *Q = (struct pj_opaque *) P->opaque;
    unsigned int i;
    PJ_COORD in, out;

    out = proj_coord_error();
    in.xyz = xyz;

    for (i=0; i<3; i++)
        out.v[Q->axis[i]] = in.v[i] * Q->sign[i];

    return out.lpz;
}


static PJ_COORD forward_4d(PJ_COORD coo, PJ *P) {
    struct pj_opaque *Q = (struct pj_opaque *) P->opaque;
    unsigned int i;
    PJ_COORD out;

    out = proj_coord_error();

    for (i=0; i<4; i++)
        out.v[i] = coo.v[Q->axis[i]] * Q->sign[i];

    return out;
}


static PJ_COORD reverse_4d(PJ_COORD coo, PJ *P) {
    struct pj_opaque *Q = (struct pj_opaque *) P->opaque;
    unsigned int i;
    PJ_COORD out;

    out = proj_coord_error();

    for (i=0; i<4; i++)
        out.v[Q->axis[i]] = coo.v[i] * Q->sign[i];

    return out;
}


/***********************************************************************/
PJ *CONVERSION(axisswap,0) {
/***********************************************************************/
    struct pj_opaque *Q = pj_calloc (1, sizeof (struct pj_opaque));
    char *s;
    unsigned int i, j, n = 0;

    if (0==Q)
        return pj_default_destructor (P, ENOMEM);
    P->opaque = (void *) Q;


    /* +order and +axis are mutually exclusive */
    if ( !pj_param_exists(P->params, "order") == !pj_param_exists(P->params, "axis") )
        return pj_default_destructor(P, PJD_ERR_AXIS);

    /* fill axis list with indices from 4-7 to simplify duplicate search further down */
    for (i=0; i<4; i++) {
        Q->axis[i] = i+4;
        Q->sign[i] = 1;
    }

    /* if the "order" parameter is used */
    if ( pj_param_exists(P->params, "order") ) {
        /* read axis order */
        char *order = pj_param(P->ctx, P->params, "sorder").s;

        /* check that all characters are valid */
        for (i=0; i<strlen(order); i++)
            if (strchr("1234-,", order[i]) == 0) {
                proj_log_error(P, "axisswap: unknown axis '%c'", order[i]);
                return pj_default_destructor(P, PJD_ERR_AXIS);
            }

        /* read axes numbers and signs */
        for ( s = order, n = 0; *s != '\0' && n < 4; ) {
            Q->axis[n] = abs(atoi(s))-1;
            if (Q->axis[n] > 3) {
                proj_log_error(P, "axisswap: invalid axis '%d'", Q->axis[n]);
                return pj_default_destructor(P, PJD_ERR_AXIS);
            }
            Q->sign[n++] = sign(atoi(s));
            while ( *s != '\0' && *s != ',' )
                s++;
            if ( *s == ',' )
                s++;
        }
    }

    /* if the "axis" parameter is used */
    if ( pj_param_exists(P->params, "axis") ) {
        /* parse the classic PROJ.4 enu axis specification */
        for (i=0; i < 3; i++) {
            switch(P->axis[i]) {
                case 'w':
                    Q->sign[i] = -1;
                    Q->axis[i] = 0;
                    break;
                case 'e':
                    Q->sign[i] = 1;
                    Q->axis[i] = 0;
                    break;
                case 's':
                    Q->sign[i] = -1;
                    Q->axis[i] = 1;
                    break;
                case 'n':
                    Q->sign[i] = 1;
                    Q->axis[i] = 1;
                    break;
                case 'd':
                    Q->sign[i] = -1;
                    Q->axis[i] = 2;
                    break;
                case 'u':
                    Q->sign[i] = 1;
                    Q->axis[i] = 2;
                    break;
                default:
                    proj_log_error(P, "axisswap: unknown axis '%c'", P->axis[i]);
                    return pj_default_destructor(P, PJD_ERR_AXIS);
            }
        }
        n = 3;
    }

    /* check for duplicate axes */
    for (i=0; i<4; i++)
        for (j=0; j<4; j++) {
            if (i==j)
                continue;
            if (Q->axis[i] == Q->axis[j]) {
                proj_log_error(P, "swapaxis: duplicate axes specified");
                return pj_default_destructor(P, PJD_ERR_AXIS);
            }
        }


    /* only map fwd/inv functions that are possible with the given axis setup */
    if (n == 4) {
        P->fwd4d = forward_4d;
        P->inv4d = reverse_4d;
    }
    if (n == 3 && Q->axis[0] < 3 && Q->axis[1] < 3 && Q->axis[2] < 3) {
        P->fwd3d  = forward_3d;
        P->inv3d  = reverse_3d;
    }
    if (n == 2 && Q->axis[0] < 2 && Q->axis[1] < 2) {
        P->fwd    = forward_2d;
        P->inv    = reverse_2d;
    }


    if (P->fwd4d == NULL && P->fwd3d == NULL && P->fwd == NULL) {
        proj_log_error(P, "swapaxis: bad axis order");
        return pj_default_destructor(P, PJD_ERR_AXIS);
    }

    if (pj_param(P->ctx, P->params, "tangularunits").i) {
        P->left  = PJ_IO_UNITS_ANGULAR;
        P->right = PJ_IO_UNITS_ANGULAR;
    } else {
        P->left  = PJ_IO_UNITS_WHATEVER;
        P->right = PJ_IO_UNITS_WHATEVER;
    }


    /* Preparation and finalization steps are skipped, since the raison   */
    /* d'etre of axisswap is to bring input coordinates in line with the  */
    /* the internally expected order (ENU), such that handling of offsets */
    /* etc. can be done correctly in a later step of a pipeline */
    P->skip_fwd_prepare  = 1;
    P->skip_fwd_finalize = 1;
    P->skip_inv_prepare  = 1;
    P->skip_inv_finalize = 1;

    return P;
}
