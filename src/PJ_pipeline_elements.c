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
    double R[3][3];
};


/* Make the maths of the rotation operations somewhat more readable and textbook like */
#define R00 Q->R[0][0]
#define R01 Q->R[0][1]
#define R02 Q->R[0][2]

#define R10 Q->R[1][0]
#define R11 Q->R[1][1]
#define R12 Q->R[1][2]

#define R20 Q->R[2][0]
#define R21 Q->R[2][1]
#define R22 Q->R[2][2]


static XYZ helmert_forward_3d (LPZ lpz, PJ *P) {
    struct pj_opaque_helmert *Q = (struct pj_opaque_helmert *) P->opaque;
    COORDINATE point;
    double X, Y, Z, scale;

    scale = 1 + Q->scale * 1e-6;

    X = lpz.lam;
    Y = lpz.phi;
    Z = lpz.z;

    point.xyz = Q->xyz;
    point.xyz.x += scale * ( R00 * X  +   R01 * Y   +   R02 * Z);
    point.xyz.y += scale * ( R10 * X  +   R11 * Y   +   R12 * Z);
    point.xyz.z += scale * ( R20 * X  +   R21 * Y   +   R22 * Z);

    return point.xyz;
}

static LPZ helmert_reverse_3d (XYZ xyz, PJ *P) {
    struct pj_opaque_helmert *Q = (struct pj_opaque_helmert *) P->opaque;
    COORDINATE point;
    double X, Y, Z, scale;

    scale = 1 - Q->scale * 1e-6;

    X = xyz.x;
    Y = xyz.y;
    Z = xyz.z;

    point.xyz.x = -Q->xyz.x;
    point.xyz.y = -Q->xyz.y;
    point.xyz.z = -Q->xyz.z;

    /* Transpose multiplication inverts the rotation */
    point.xyz.x += scale * ( R00 * X  +   R10 * Y   +   R20 * Z);
    point.xyz.y += scale * ( R01 * X  +   R11 * Y   +   R21 * Z);
    point.xyz.z += scale * ( R02 * X  +   R12 * Y   +   R22 * Z);

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
    int approximate = 0, transpose = 0;

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

    /* use small angle approximations? */
    if ( (pj_param(P->ctx, P->params, "tapprox").i) )
        approximate = 1;

    /* use "other" rotation sign convention? */
    if ( (pj_param(P->ctx, P->params, "ttranspose").i) )
        transpose = 1;

    Q->xyz = xyz;
    Q->opk = opk;
    Q->scale = scale;


    /**************************************************************************

        Build rotation matrix.
        ----------------------

        Here we rename rotation indices from omega, phi, kappa (opk), to
        fi (i,e, phi), theta, psi (ftp), in order to reduce the mental agility
        needed to implement the expression for the rotation matrix derived over
        at https://en.wikipedia.org/wiki/Rotation_formalisms_in_three_dimensions
        
        If the option "approximate" is set, small angle approximations are used:
        The matrix elements are approximated by expanding the trigonometric
        functions to linear order (i.e. cos(x) = 1, sin(x) = x), and discarding
        products of second order.
        
        This was useful back when calculating by hand was the only option, but in
        general, today, should be avoided because:
        
        1. It does not save much computation time, as the rotation matrix
           is built only once, and probably used many times.
           
        2. The error induced may be too large for ultra high accuracy
           applications: the Earth is huge and the linear error is
           approximately the angular error multiplied by the Earth radius.
        
        However, in many cases the approximation is necessary, since it has
        been used historically: Rotation angles from older published datum
        shifts may actually be a least squares fit to the linearized rotation
        approximation, hence actually not being valid for deriving the full
        rotation matrix.
        
        So in order to fit historically derived coordinates, the access to
        the approximate rotation matrix is necessary - at least in principle.

        Also, when using any published datum transformation information one
        should always check which convention (exact or approximate rotation
        matrix) is expected, and whether the induced error for selecting
        the opposite convention is acceptable.
        
        
        Sign conventions
        ----------------
        
        Take care: Two different sign conventions exist.
        
        Conceptually they relate to whether we rotate the coordinate system
        or the "position vector" (the vector going from the coordinate system
        origin to the point being transformed, i.e. the point coordinates
        interpreted as vector coordinates).
        
        Switching between the "position vector" and "coordinate system"
        conventions is simply a matter of switching the sign of the rotation
        angles, which algebraically also translates into a transposition of
        the rotation matrix.
        
        Hence, as geodetic constants should preferably be referred to exactly
        as published, the "transpose" option provides the ability to switch
        between the conventions.
        
        
        Inverse transformation
        ----------------------
        
        No matter what sign convention, the direction of the overall Helmert
        transformation (i.e. offset, scale, and rotation), is reversed by
        switching signs for *all* 7 parameters. Hence, the code for the inverse
        transformation in helmert_reverse_3d, when compared to the forward code
        in helmert_forward_3d, does 3 things:

        1. reverses the sign of the scale
        2. reverses the sign of the offset
        3. does transpose multiplication.

    ***************************************************************************/
    do {
        double  f,  t,  p;    /* phi/fi , theta, psi  */
        double cf, ct, cp;    /* cos (fi, theta, psi) */
        double sf, st, sp;    /* sin (fi, theta, psi) */

        /* rename   (omega, phi, kappa)   to   (fi, theta, psi)   */
        f = opk.o;
        t = opk.p;
        p = opk.k;

        if (approximate) {
            R00 =  1;
            R01 =  p;
            R02 = -t;

            R10 = -p;
            R11 =  1;
            R12 =  f;

            R20 =  t;
            R21 = -f;
            R22 =  1;
            break;
        }

        cf = cos(f);
        sf = sin(f);
        ct = cos(t);
        st = sin(t);
        cp = cos(p);
        sp = sin(p);


        R00 = ct*cp;
        R01 = cf*sp + sf*st*cp;
        R02 = sf*sp - cf*st*cp;

        R10 = -ct*sp;
        R11 =  cf*cp - sf*st*sp;
        R12 =  sf*cp + cf*st*sp;

        R20 =  st;
        R21 = -sf*ct;
        R22 =  cf*ct;

    } while (0);
    

    if (transpose) {
        double r;
        r = R01;    R01 = R10;    R10 = r;
        r = R02;    R02 = R20;    R20 = r;
        
        r = R10;    R10 = R01;    R01 = r;
        r = R12;    R12 = R21;    R21 = r;
        
        r = R20;    R20 = R02;    R02 = r;
        r = R21;    R21 = R12;    R12 = r;
    }

    return P;
}


/* selftest stub */
int pj_helmert_selftest (void) {return 0;}
