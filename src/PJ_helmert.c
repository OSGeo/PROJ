/***********************************************************************

             Helmert transformations, 3- and 7-parameter

                    Thomas Knudsen, 2016-05-24

************************************************************************

  Implements 3- and 7-parameter Helmert transformations for 3D data.

  Primarily useful for implementation of datum shifts in transformation
  pipelines.

************************************************************************

Thomas Knudsen, thokn@sdfe.dk, 2016-05-24/06-05
Last update: 2016-11-24

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
#include <proj.h>
#include <projects.h>
#include <geocent.h>
#include <assert.h>
#include <stddef.h>
#include <errno.h>
PROJ_HEAD(helmert, "3- and 7-parameter Helmert shift");

static XYZ helmert_forward_3d (LPZ lpz, PJ *P);
static LPZ helmert_reverse_3d (XYZ xyz, PJ *P);



static void *freeup_msg (PJ *P, int errlev) {         /* Destructor */
    if (0==P)
        return 0;

    if (0!=P->ctx)
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


/***********************************************************************/
struct pj_opaque_helmert {
/************************************************************************
    Projection specific elements for the "helmert" PJ object
************************************************************************/
    XYZ xyz;
    PJ_OPK opk;
    double scale;
    double R[3][3];
    int no_rotation;
};


/* Make the maths of the rotation operations somewhat more readable and textbook like */
#define R00 (Q->R[0][0])
#define R01 (Q->R[0][1])
#define R02 (Q->R[0][2])

#define R10 (Q->R[1][0])
#define R11 (Q->R[1][1])
#define R12 (Q->R[1][2])

#define R20 (Q->R[2][0])
#define R21 (Q->R[2][1])
#define R22 (Q->R[2][2])






/***********************************************************************/
static XY helmert_forward (LP lp, PJ *P) {
/************************************************************************

    The 2D implementation is a slightly silly stop-gap, returning a
    2D sub-space of the 3D transformation.

    In order to follow the "principle of least astonishment", the 2D
    interface should provide an implementation of the 4 parameter
    2D Helmert shift.

************************************************************************/
    PJ_TRIPLET point;
    point.lp = lp;
    point.lpz.z = 0;
    point.xyz = helmert_forward_3d (point.lpz, P);
    return point.xy;
}


/***********************************************************************/
static LP helmert_reverse (XY xy, PJ *P) {
/***********************************************************************/
    PJ_TRIPLET point;
    point.xy = xy;
    point.xyz.z = 0;
    point.lpz = helmert_reverse_3d (point.xyz, P);
    return point.lp;
}


/***********************************************************************/
static XYZ helmert_forward_3d (LPZ lpz, PJ *P) {
/***********************************************************************/
    struct pj_opaque_helmert *Q = (struct pj_opaque_helmert *) P->opaque;
    PJ_TRIPLET point;
    double X, Y, Z, scale;
    if (Q->no_rotation) {
        point.xyz.x = lpz.lam + Q->xyz.x;
        point.xyz.y = lpz.phi + Q->xyz.y;
        point.xyz.z = lpz.z   + Q->xyz.z;
        return point.xyz;
    }

    scale = 1 + Q->scale * 1e-6;

    X = lpz.lam;
    Y = lpz.phi;
    Z = lpz.z;

    point.lpz = lpz;

    point.xyz.x = scale * ( R00 * X  +   R01 * Y   +   R02 * Z);
    point.xyz.y = scale * ( R10 * X  +   R11 * Y   +   R12 * Z);
    point.xyz.z = scale * ( R20 * X  +   R21 * Y   +   R22 * Z);

    point.xyz.x += Q->xyz.x;
    point.xyz.y += Q->xyz.y;
    point.xyz.z += Q->xyz.z;

    return point.xyz;
}


/***********************************************************************/
static LPZ helmert_reverse_3d (XYZ xyz, PJ *P) {
/***********************************************************************/
    struct pj_opaque_helmert *Q = (struct pj_opaque_helmert *) P->opaque;
    PJ_TRIPLET point;
    double X, Y, Z, scale;

    point.xyz = xyz;

    if (Q->no_rotation) {
        point.xyz.x  =  xyz.x - Q->xyz.x;
        point.xyz.y  =  xyz.y - Q->xyz.y;
        point.xyz.z  =  xyz.z - Q->xyz.z;
        return point.lpz;
    }

    scale = 1 + Q->scale * 1e-6;

    /* Unscale and deoffset */
    X = (xyz.x - Q->xyz.x) / scale;
    Y = (xyz.y - Q->xyz.y) / scale;
    Z = (xyz.z - Q->xyz.z) / scale;

    /* Inverse rotation through transpose multiplication */
    point.xyz.x  =  ( R00 * X   +   R10 * Y   +   R20 * Z);
    point.xyz.y  =  ( R01 * X   +   R11 * Y   +   R21 * Z);
    point.xyz.z  =  ( R02 * X   +   R12 * Y   +   R22 * Z);

    return point.lpz;
}


static PJ_OBS helmert_forward_obs (PJ_OBS point, PJ *P) {
    point.coo.xyz = helmert_forward_3d (point.coo.lpz, P);
    return point;
}


static PJ_OBS helmert_reverse_obs (PJ_OBS point, PJ *P) {
    point.coo.lpz = helmert_reverse_3d (point.coo.xyz, P);
    return point;
}


/* Milliarcsecond to radians */
#define MAS_TO_RAD (DEG_TO_RAD / 3600000.0)


/***********************************************************************/
PJ *PROJECTION(helmert) {
/***********************************************************************/
    XYZ    xyz = {0, 0, 0};
    PJ_OPK opk = {0, 0, 0};
    double scale = 0;
    int approximate = 0, transpose = 0;

    struct pj_opaque_helmert *Q = pj_calloc (1, sizeof (struct pj_opaque_helmert));
    if (0==Q)
        return freeup_msg (P, ENOMEM);
    P->opaque = (void *) Q;

    P->fwdobs = helmert_forward_obs;
    P->invobs = helmert_reverse_obs;
    P->fwdobs = 0;
    P->invobs = 0;
    P->fwd3d  = helmert_forward_3d;
    P->inv3d  = helmert_reverse_3d;
    P->fwd    = helmert_forward;
    P->inv    = helmert_reverse;

    P->left  = PJ_IO_UNITS_METERS;
    P->right = PJ_IO_UNITS_METERS;

    /* Translations */
    if (pj_param (P->ctx, P->params, "tx").i)
        xyz.x = pj_param (P->ctx, P->params, "dx").f;

    if (pj_param (P->ctx, P->params, "ty").i)
        xyz.y = pj_param (P->ctx, P->params, "dy").f;

    if (pj_param (P->ctx, P->params, "tz").i)
        xyz.z = pj_param (P->ctx, P->params, "dz").f;

    /* Rotations */
    if (pj_param (P->ctx, P->params, "trx").i)
        opk.o = pj_param (P->ctx, P->params, "drx").f * MAS_TO_RAD;

    if (pj_param (P->ctx, P->params, "try").i)
        opk.p = pj_param (P->ctx, P->params, "dry").f * MAS_TO_RAD;

    if (pj_param (P->ctx, P->params, "trz").i)
        opk.k = pj_param (P->ctx, P->params, "drz").f * MAS_TO_RAD;


    /* Scale */
    if (pj_param (P->ctx, P->params, "ts").i)
        scale = pj_param (P->ctx, P->params, "ds").f;

    /* Use small angle approximations? */
    if (pj_param (P->ctx, P->params, "bapprox").i)
        approximate = 1;

    /* Use "other" rotation sign convention? */
    if (pj_param (P->ctx, P->params, "ttranspose").i)
        transpose = 1;

    Q->xyz   =  xyz;
    Q->opk   =  opk;
    Q->scale =  scale;

    if ((opk.o==0) && (opk.p==0) && (opk.k==0) && (scale==0)) {
        Q->no_rotation = 1;
        return P;
    }


    /**************************************************************************

        Build rotation matrix.
        ----------------------

        Here we rename rotation indices from omega, phi, kappa (opk), to
        fi (i.e. phi), theta, psi (ftp), in order to reduce the mental agility
        needed to implement the expression for the rotation matrix derived over
        at https://en.wikipedia.org/wiki/Rotation_formalisms_in_three_dimensions

        If the option "approximate" is set, small angle approximations are used:
        The matrix elements are approximated by expanding the trigonometric
        functions to linear order (i.e. cos(x) = 1, sin(x) = x), and discarding
        products of second order.

        This was a useful hack when calculating by hand was the only option,
        but in general, today, should be avoided because:

        1. It does not save much computation time, as the rotation matrix
           is built only once, and probably used many times.

        2. The error induced may be too large for ultra high accuracy
           applications: the Earth is huge and the linear error is
           approximately the angular error multiplied by the Earth radius.

        However, in many cases the approximation is necessary, since it has
        been used historically: Rotation angles from older published datum
        shifts may actually be a least squares fit to the linearized rotation
        approximation, hence not being strictly valid for deriving the full
        rotation matrix.

        So in order to fit historically derived coordinates, the access to
        the approximate rotation matrix is necessary - at least in principle.

        Also, when using any published datum transformation information, one
        should always check which convention (exact or approximate rotation
        matrix) is expected, and whether the induced error for selecting
        the opposite convention is acceptable (which it often is).


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

    ***************************************************************************/
    {
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
        }
        else {
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
        }
    }

    /*
        For comparison: Description from Engsager/Poder implementation
        in set_dtm_1.c (trlib)

        DATUM SHIFT:
        TO = scale * ROTZ * ROTY * ROTX * FROM + TRANSLA

             ( cz sz 0)         (cy 0 -sy)         (1   0  0)
        ROTZ=(-sz cz 0),   ROTY=(0  1   0),   ROTX=(0  cx sx)
             (  0  0 1)         (sy 0  cy)         (0 -sx cx)

        trp->r11  =  cos_ry*cos_rz;
        trp->r12  =  cos_rx*sin_rz + sin_rx*sin_ry*cos_rz;
        trp->r13  =  sin_rx*sin_rz - cos_rx*sin_ry*cos_rz;

        trp->r21  = -cos_ry*sin_rz;
        trp->r22  =  cos_rx*cos_rz - sin_rx*sin_ry*sin_rz;
        trp->r23  =  sin_rx*cos_rz + cos_rx*sin_ry*sin_rz;

        trp->r31  =  sin_ry;
        trp->r32  = -sin_rx*cos_ry;
        trp->r33  =  cos_rx*cos_ry;

        trp->scale = 1.0 + scale;
    */


    if (transpose) {
        double r;
        r = R01;    R01 = R10;    R10 = r;
        r = R02;    R02 = R20;    R20 = r;
        r = R12;    R12 = R21;    R21 = r;
    }

    return P;
}




#ifndef PJ_SELFTEST

int pj_helmert_selftest (void) {return 0;}             /* self test stub */

#else


static int test (char *args, PJ_TRIPLET in, PJ_TRIPLET expect) {
    PJ_TRIPLET out;
    PJ *P = pj_init_plus (args);

    if (0==P)
        return 5;

    out.xyz = pj_fwd3d (in.lpz, P);
    if (pj_xyz_dist (out.xyz, expect.xyz) > 1e-4)
        return 1;

    out.lpz = pj_inv3d (out.xyz, P);
    if (pj_xyz_dist (out.xyz, in.xyz) > 1e-4)
        return 2;

    return 0;
}



int pj_helmert_selftest (void) {

    /* This example is from
       Lotti Jivall:
           Simplified transformations from ITRF2008/IGS08 to ETRS89 for maritime applications */
    PJ_TRIPLET in1     = {{3565285.0000,     855949.0000,     5201383.0000}};
    PJ_TRIPLET expect1 = {{3565285.41342351, 855948.67986759, 5201382.72939791}};
    char args1[] = {
        " +proj=helmert +ellps=GRS80"
        "     +x=0.67678  +y=0.65495 +z=-0.52827"
        "    +rx=-22.742 +ry=12.667 +rz=22.704   +s=-0.01070"
    };


    /* This example is a random point, transformed from ED50 to ETRS89 using KMStrans2 */
    PJ_TRIPLET in2 =     {{3494994.3012, 1056601.9725, 5212382.1666}};
    PJ_TRIPLET expect2 = {{3494909.84026368, 1056506.78938633, 5212265.66699761}};
    char args2[] = {
        " +proj=helmert +ellps=GRS80"
        "  +x=-81.0703 +y=-89.3603 +z=-115.7526"
        " +rx=-484.88 +ry=-24.36  +rz=-413.21   +s=-0.540645"
    };
    int ret;

    ret = test (args1, in1, expect1);   if (ret)  return ret;
    ret = test (args2, in2, expect2);   if (ret)  return ret + 10;
    return 0;
}

#endif
