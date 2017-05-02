/***********************************************************************

             Helmert transformations, 3- and 7-parameter

                    Thomas Knudsen, 2016-05-24

************************************************************************

  Implements 3-, 7- and 14-parameter Helmert transformations for 3D
  data.

  Primarily useful for implementation of datum shifts in transformation
  pipelines.

************************************************************************

Thomas Knudsen, thokn@sdfe.dk, 2016-05-24/06-05
Kristian Evers, kreve@sdfe.dk, 2017-05-01
Last update: 2017-05-01

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
PROJ_HEAD(helmert, "3-, 7- and 14-parameter Helmert shift");

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
    XYZ xyz_0;
    XYZ dxyz;
    PJ_OPK opk;
    PJ_OPK opk_0;
    PJ_OPK dopk;
    double scale;
    double scale_0;
    double dscale;
    double R[3][3];
    double epoch, t_obs;
    int no_rotation, approximate, transpose;
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

static void update_parameters(PJ *P) {
    /**************************************************************************

    Update transformation parameters.
    ---------------------------------

    The 14-parameter Helmert transformation is at it's core the same as the
    7-parameter transformation, since the transformation parameters are
    projected forward or backwards in time via the rate of changes of the
    parameters. The transformation parameters are calculated for a specific
    epoch before the actual Helmert transformation is carried out.

    The transformation parameters are updated with the following equation [0]:

                      .
    P(t) = P(EPOCH) + P * (t - EPOCH)

                                                              .
    where EPOCH is the epoch indicated in the above table and P is the rate
    of that parameter.

    [0] http://itrf.ign.fr/doc_ITRF/Transfo-ITRF2008_ITRFs.txt
    **************************************************************************/

    struct pj_opaque_helmert *Q = (struct pj_opaque_helmert *) P->opaque;
    double dt = Q->t_obs - Q->epoch;

    Q->xyz.x = Q->xyz_0.x + Q->dxyz.x * dt;
    Q->xyz.y = Q->xyz_0.y + Q->dxyz.y * dt;
    Q->xyz.z = Q->xyz_0.z + Q->dxyz.z * dt;

    Q->opk.o = Q->opk_0.o + Q->dopk.o * dt;
    Q->opk.p = Q->opk_0.p + Q->dopk.p * dt;
    Q->opk.k = Q->opk_0.k + Q->dopk.k * dt;

    Q->scale = Q->scale_0 + Q->dscale * dt;

    /* debugging output */
    pj_log_trace(P, "Transformation parameters for observation epoch %g:", Q->t_obs);
    pj_log_trace(P, "x: %g", Q->xyz.x);
    pj_log_trace(P, "y: %g", Q->xyz.y);
    pj_log_trace(P, "z: %g", Q->xyz.z);
    pj_log_trace(P, "s: %g", Q->scale*1e-6);
    pj_log_trace(P, "rx: %g", Q->opk.o);
    pj_log_trace(P, "ry: %g", Q->opk.p);
    pj_log_trace(P, "rz: %g", Q->opk.k);
    return;
}

static void build_rot_matrix(PJ *P) {
    /**************************************************************************

    Build rotation matrix.
    ----------------------

    Here we rename rotation indices from omega, phi, kappa (opk), to
    fi (i.e. phi), theta, psi (ftp), in order to reduce the mental agility
    needed to implement the expression for the rotation matrix derived over
    at https://en.wikipedia.org/wiki/Rotation_formalisms_in_three_dimensions
    The relevant section is Euler angles ( z-’-x" intrinsic) -> Rotation matrix

    If the option "approximate" is set, small angle approximations are used:
    The matrix elements are approximated by expanding the trigonometric
    functions to linear order (i.e. cos(x) = 1, sin(x) = x), and discarding
    products of second order.

    This was a useful hack when calculating by hand was the only option,
    but in general, today, should be avoided because:

    1. It does not save much computation time, as the rotation matrix
       is built only once and probably used many times (except when
       transforming spatio-temporal coordinates).

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
    struct pj_opaque_helmert *Q = (struct pj_opaque_helmert *) P->opaque;

    double  f,  t,  p;    /* phi/fi , theta, psi  */
    double cf, ct, cp;    /* cos (fi, theta, psi) */
    double sf, st, sp;    /* sin (fi, theta, psi) */

    /* rename   (omega, phi, kappa)   to   (fi, theta, psi)   */
    f = Q->opk.o;
    t = Q->opk.p;
    p = Q->opk.k;

    if (Q->approximate) {
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


    if (Q->transpose) {
        double r;
        r = R01;    R01 = R10;    R10 = r;
        r = R02;    R02 = R20;    R20 = r;
        r = R12;    R12 = R21;    R21 = r;
    }

    /* some debugging output */
    pj_log_trace(P, "Rotation Matrix:");
    pj_log_trace(P, "  | % 6.6g  % 6.6g  % 6.6g |", R00, R01, R02);
    pj_log_trace(P, "  | % 6.6g  % 6.6g  % 6.6g |", R10, R11, R12);
    pj_log_trace(P, "  | % 6.6g  % 6.6g  % 6.6g |", R20, R21, R22);

    return;
}




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
    double X, Y, Z, scale, dt;

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
    double X, Y, Z, scale, dt;

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
    struct pj_opaque_helmert *Q = (struct pj_opaque_helmert *) P->opaque;

    /* We only need to rebuild the rotation matrix if the
     * observation time is different from the last call */
    if (point.coo.xyzt.t != Q->t_obs) {
        Q->t_obs = point.coo.xyzt.t;
        update_parameters(P);
        build_rot_matrix(P);
    }

    point.coo.xyz = helmert_forward_3d (point.coo.lpz, P);

    return point;
}


static PJ_OBS helmert_reverse_obs (PJ_OBS point, PJ *P) {
    struct pj_opaque_helmert *Q = (struct pj_opaque_helmert *) P->opaque;

    /* We only need to rebuild the rotation matrix if the
     * observation time is different from the last call */
    if (point.coo.xyzt.t != Q->t_obs) {
        Q->t_obs = point.coo.xyzt.t;
        update_parameters(P);
        build_rot_matrix(P);
    }

    point.coo.lpz = helmert_reverse_3d (point.coo.xyz, P);

    return point;
}


/* Arcsecond to radians */
#define ARCSEC_TO_RAD (DEG_TO_RAD / 3600.0)

/***********************************************************************/
PJ *PROJECTION(helmert) {
/***********************************************************************/
    struct pj_opaque_helmert *Q = pj_calloc (1, sizeof (struct pj_opaque_helmert));
    if (0==Q)
        return freeup_msg (P, ENOMEM);
    P->opaque = (void *) Q;

    P->fwdobs = helmert_forward_obs;
    P->invobs = helmert_reverse_obs;
    P->fwd3d  = helmert_forward_3d;
    P->inv3d  = helmert_reverse_3d;
    P->fwd    = helmert_forward;
    P->inv    = helmert_reverse;

    P->left  = PJ_IO_UNITS_METERS;
    P->right = PJ_IO_UNITS_METERS;

    /* Translations */
    if (pj_param (P->ctx, P->params, "tx").i)
        Q->xyz_0.x = pj_param (P->ctx, P->params, "dx").f;

    if (pj_param (P->ctx, P->params, "ty").i)
        Q->xyz_0.y = pj_param (P->ctx, P->params, "dy").f;

    if (pj_param (P->ctx, P->params, "tz").i)
        Q->xyz_0.z = pj_param (P->ctx, P->params, "dz").f;

    /* Rotations */
    if (pj_param (P->ctx, P->params, "trx").i)
        Q->opk_0.o = pj_param (P->ctx, P->params, "drx").f * ARCSEC_TO_RAD;

    if (pj_param (P->ctx, P->params, "try").i)
        Q->opk_0.p = pj_param (P->ctx, P->params, "dry").f * ARCSEC_TO_RAD;

    if (pj_param (P->ctx, P->params, "trz").i)
        Q->opk_0.k = pj_param (P->ctx, P->params, "drz").f * ARCSEC_TO_RAD;


    /* Scale */
    if (pj_param (P->ctx, P->params, "ts").i)
        Q->scale_0 = pj_param (P->ctx, P->params, "ds").f;


    /* Translation rates */
    if (pj_param(P->ctx, P->params, "tdx").i)
        Q->dxyz.x = pj_param (P->ctx, P->params, "ddx").f;

    if (pj_param(P->ctx, P->params, "tdy").i)
        Q->dxyz.y = pj_param (P->ctx, P->params, "ddy").f;

    if (pj_param(P->ctx, P->params, "tdz").i)
        Q->dxyz.z = pj_param (P->ctx, P->params, "ddz").f;

    /* Rotations rates */
    if (pj_param (P->ctx, P->params, "tdrx").i)
        Q->dopk.o = pj_param (P->ctx, P->params, "ddrx").f * ARCSEC_TO_RAD;

    if (pj_param (P->ctx, P->params, "tdry").i)
        Q->dopk.p = pj_param (P->ctx, P->params, "ddry").f * ARCSEC_TO_RAD;

    if (pj_param (P->ctx, P->params, "tdrz").i)
        Q->dopk.k = pj_param (P->ctx, P->params, "ddrz").f * ARCSEC_TO_RAD;


    /* Scale rate */
    if (pj_param (P->ctx, P->params, "tds").i)
        Q->dscale = pj_param (P->ctx, P->params, "dds").f;


    /* Epoch */
    if (pj_param(P->ctx, P->params, "tepoch").i)
        Q->epoch = pj_param (P->ctx, P->params, "depoch").f;

    if (pj_param(P->ctx, P->params, "ttobs").i)
        Q->t_obs = pj_param (P->ctx, P->params, "dtobs").f;

    /* Use small angle approximations? */
    if (pj_param (P->ctx, P->params, "bapprox").i)
        Q->approximate = 1;

    /* Use "other" rotation sign convention? */
    if (pj_param (P->ctx, P->params, "ttranspose").i)
        Q->transpose = 1;

    Q->xyz    =  Q->xyz_0;
    Q->opk    =  Q->opk_0;
    Q->scale  =  Q->scale_0;

    /* Let's help with debugging */
    pj_log_debug(P, "Helmert parameters:");
    pj_log_debug(P, "x=  % 3.5f  y=  % 3.5f  z=  % 3.5f", Q->xyz.x, Q->xyz.y, Q->xyz.z);
    pj_log_debug(P, "rx= % 3.5f  ry= % 3.5f  rz= % 3.5f",
            Q->opk.o / ARCSEC_TO_RAD, Q->opk.p / ARCSEC_TO_RAD, Q->opk.k / ARCSEC_TO_RAD);
    pj_log_debug(P, "s=% 3.5f  approximate=% d  transpose=% d",
            Q->scale, Q->approximate, Q->transpose);
    pj_log_debug(P, "dx= % 3.5f  dy= % 3.5f  dz= % 3.5f", Q->dxyz.x, Q->dxyz.y, Q->dxyz.z);
    pj_log_debug(P, "drx=% 3.5f  dry=% 3.5f  drz=% 3.5f", Q->dopk.o, Q->dopk.p, Q->dopk.k);
    pj_log_debug(P, "ds=% 3.5f  epoch=% 5.5f  tobs=% 5.5f", Q->dscale, Q->epoch, Q->t_obs);

    if ((Q->opk.o==0) && (Q->opk.p==0) && (Q->opk.k==0) && (Q->scale==0)) {
        Q->no_rotation = 1;
        return P;
    }

    update_parameters(P);
    build_rot_matrix(P);

    return P;
}



#ifndef PJ_SELFTEST

int pj_helmert_selftest (void) {return 0;}

#else


static int test (char *args, PJ_TRIPLET in, PJ_TRIPLET expect) {
    PJ_TRIPLET out;
    PJ *P = pj_init_plus (args);

    if (0==P)
        return 5;

    out.xyz = pj_fwd3d (in.lpz, P);
    if (pj_xyz_dist (out.xyz, expect.xyz) > 1e-4) {
        pj_log_error(P, "Tolerance of forward calculation not met");
        pj_log_error(P, "  Expect: %10.10f, %10.10f, %10.10f", expect.xyz.x, expect.xyz.y, expect.xyz.z);
        pj_log_error(P, "  Out:    %10.10f, %10.10f, %10.10f", out.xyz.x, out.xyz.y, out.xyz.z);
        pj_log_level(NULL, 0);
        return 1;
    }

    out.lpz = pj_inv3d (out.xyz, P);
    if (pj_xyz_dist (out.xyz, in.xyz) > 1e-4) {
        pj_log_error(P, "Tolerance of inverse calculation not met");
        pj_log_error(P, "  In:  %10.10f, %10.10f, %10.10f", in.xyz.x, in.xyz.y, in.xyz.z);
        pj_log_error(P, "  Out: %10.10f, %10.10f, %10.10f", out.xyz.x, out.xyz.y, out.xyz.z);
        pj_log_level(NULL, 0);
        return 2;
    }

    return 0;
}



int pj_helmert_selftest (void) {
    int ret;

    /* This example is from
       Lotti Jivall:
           Simplified transformations from ITRF2008/IGS08 to ETRS89 for maritime applications */
    PJ_TRIPLET in1     = {{3565285.0000,     855949.0000,     5201383.0000}};
    PJ_TRIPLET expect1 = {{3565285.41342351, 855948.67986759, 5201382.72939791}};
    char args1[] = {
        " +proj=helmert +ellps=GRS80"
        "     +x=0.67678  +y=0.65495 +z=-0.52827"
        "    +rx=-0.022742 +ry=0.012667 +rz=0.022704   +s=-0.01070"
        /*"    +rx=-22.742 +ry=12.667 +rz=22.704   +s=-0.01070" */
    };


    /* This example is a random point, transformed from ED50 to ETRS89 using KMStrans2 */
    PJ_TRIPLET in2 =     {{3494994.3012, 1056601.9725, 5212382.1666}};
    PJ_TRIPLET expect2 = {{3494909.84026368, 1056506.78938633, 5212265.66699761}};
    char args2[] = {
        " +proj=helmert +ellps=GRS80"
        "  +x=-81.0703 +y=-89.3603 +z=-115.7526"
        " +rx=-0.48488 +ry=-0.02436  +rz=-0.41321   +s=-0.540645"
    };


    /* This example is a coordinate from the geodetic observatory in Onsala,
       Sweden transformed from ITRF2000 @ 2017.0 to ITRF93 @ 2017.0.
       The test coordinate was transformed using GNSStrans.
       Transformation parameters published by ITRF:
            ftp://itrf.ensg.ign.fr/pub/itrf/ITRF.TP  */
    PJ_TRIPLET in3 = {{3370658.378, 711877.314, 5349787.086}}; /* ITRF2000@2017.0 */
    PJ_TRIPLET expect3 = {{3370658.18890, 711877.42370, 5349787.12430}}; /* ITRF93@2017.0 */
    char args3[] = {
        " +proj=helmert"
        " +x=0.0127 +y=0.0065 +z=-0.0209 +s=0.00195"
        " +rx=-0.00039 +ry=0.00080 +rz=-0.00114"
        " +dx=-0.0029 +dy=-0.0002 +dz=-0.0006 +ds=0.00001"
        " +drx=-0.00011 +dry=-0.00019 +drz=0.00007"
        " +epoch=1988.0 +tobs=2017.0 +transpose"
    };

    /* Test the 4D-capabilities of the proj.h API, especially that the rotation
       matrix is updated when necessary. Test coordinates from GNSStrans. */
    XYZ expect4a = {3370658.18890, 711877.42370, 5349787.12430};
    XYZ expect4b = {3370658.18087, 711877.42750, 5349787.12648};
    PJ_OBS in4 = {{3370658.378, 711877.314, 5349787.086, 2017.0}}, out;

    PJ *helmert = pj_create(
        " +proj=helmert"
        " +x=0.0127 +y=0.0065 +z=-0.0209 +s=0.00195"
        " +rx=-0.00039 +ry=0.00080 +rz=-0.00114"
        " +dx=-0.0029 +dy=-0.0002 +dz=-0.0006 +ds=0.00001"
        " +drx=-0.00011 +dry=-0.00019 +drz=0.00007"
        " +epoch=1988.0 +transpose"
    );


    /* Run tests 1-3 */
    ret = test (args1, in1, expect1);   if (ret)  return ret;
    ret = test (args2, in2, expect2);   if (ret)  return ret + 10;
    ret = test (args3, in3, expect3);   if (ret)  return ret + 20;

    /* Run test 4 */
    out = pj_trans(helmert, PJ_FWD, in4);
    if (pj_xyz_dist (out.coo.xyz, expect4a) > 1e-4) {
        pj_log_error(helmert, "Tolerance of test 4a not met!");
        pj_log_error(helmert, "  In:  %10.10f, %10.10f, %10.10f", in4.coo.xyz.x, in4.coo.xyz.y, in4.coo.xyz.z);
        pj_log_error(helmert, "  Out: %10.10f, %10.10f, %10.10f", out.coo.xyz.x, out.coo.xyz.y, out.coo.xyz.z);
        return 31;
    }

    in4.coo.xyzt.t = 2018.0;
    out = pj_trans(helmert, PJ_FWD, in4);
    if (pj_xyz_dist (out.coo.xyz, expect4b) > 1e-4) {
        pj_log_error(helmert, "Tolerance of test 4b not met!");
        pj_log_error(helmert, "  In:  %10.10f, %10.10f, %10.10f", in4.coo.xyz.x, in4.coo.xyz.y, in4.coo.xyz.z);
        pj_log_error(helmert, "  Out: %10.10f, %10.10f, %10.10f", out.coo.xyz.x, out.coo.xyz.y, out.coo.xyz.z);
        return 32;
    }

    pj_free(helmert);

    return 0;
}

#endif
