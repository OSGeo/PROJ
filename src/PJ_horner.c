/***********************************************************************

    Interfacing to a classic piece of geodetic software

************************************************************************

    gen_pol is a highly efficient, classic implementation of a generic
    2D Horner's Scheme polynomial evaluation routine.

    The original Poder/Engsager gen_pol implementation (where
    the polynomial degree and two sets of polynomial coefficients
    are packed together in one compound array, handled via a simple
    double pointer) is compelling and "true to the code history":

    It has a beautiful classical 1960s ring to it, not unlike the
    original fft implementations, which revolutionized spectral
    analysis in twenty lines of code.

    The Poder coding sound, as classic 1960s as Phil Spector's Wall
    of Sound, is beautiful and inimitable.

	On the other hand: For the uninitiated, the code is hard to
	follow, despite being compact.

    Also, since adding metadata and improving maintainability
    of the code are among the implied goals of a current SDFE/DTU-space
	project, the material in this file introduces a version with a more
	more modern (or at least 1990s) look, introducing a "double 2D
	polynomial" data type, HORNER.

    Despite introducing a new data type for handling the polynomial
    coefficients, great care has been taken to keep the order of
    the coefficients.

    Hence, on one hand, the HORNER data type helps improving the
    long term maintainability of the code by making the data
    organization more mentally accessible.

    On the other hand, it allows us to preserve the business end of
    the original gen_pol implementation - although not including the
	famous "Poder dual autocheck" in all its enigmatic elegance.

	The original code has, however, been included in the conditionally
	compiled TEST-section.

	This is partially for validation of the revised version, partially
	to enable more generic enjoyment of an interesting piece of
	ingenious geodetic code - simplistic and enigmatic at the same time.


 **********************************************************************

	The material included here was written by Knud Poder, starting
	around 1960, Karsten Engsager, starting around 1970, and Thomas
	Knudsen, starting around 2005

 ***********************************************************************
 *
 * The gen_pol routine comes with this legal statement
 *
 * Copyright (c) 2011, National Survey and Cadastre, Denmark
 * (Kort- og Matrikelstyrelsen), kms@kms.dk
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 *
 *******************************************************************************
 *
 * The remaining parts are...
 *
 * Copyright (c) 2016, Thomas Knudsen / Karsten Engsager / SDFE http://www.sdfe.dk
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
 *****************************************************************************/


#define PJ_LIB__
#include <proj.h>
#include <projects.h>
#include <assert.h>
#include <stddef.h>
#include <math.h>
#include <errno.h>
#include "horner.h"
PROJ_HEAD(horner,    "Horner polynomial evaluation");






static PJ_OBS horner_forward_obs (PJ_OBS point, PJ *P) {
    point.coo.uv = horner ((HORNER *) P->opaque, 1, point.coo.uv);
    return point;
}

static PJ_OBS horner_reverse_obs (PJ_OBS point, PJ *P) {
    point.coo.uv = horner ((HORNER *) P->opaque, -1, point.coo.uv);
    return point;
}






static void *horner_freeup (PJ *P) {                        /* Destructor */
    if (0==P)
        return 0;
    if (0==P->opaque)
        return pj_dealloc (P);
    horner_free ((HORNER *) P->opaque);
    return pj_dealloc(P);
}

static void freeup (PJ *P) {
    horner_freeup (P);
    return;
}


static int parse_coefs (PJ *P, double *coefs, char *param, int ncoefs) {
    char buf[20], *init, *next;
    int i;
    sprintf (buf, "t%s", param);
    if (0==pj_param (P->ctx, P->params, buf).i)
        return 0;
    sprintf (buf, "s%s", param);
    init = pj_param(P->ctx, P->params, buf).s;

    for (i = 0; i < ncoefs; i++) {
        if (i > 0) {
            if (','!=*next) {
                pj_log_error (P, "Horner: Malformed polynomium set %s. need %d coefs", param, ncoefs);
                return 0;
            }
            init = ++next;
        }
        coefs[i] = pj_strtod (init, &next);
    }
    return 1;
}


/*********************************************************************/
PJ *PROJECTION(horner) {
/*********************************************************************/
    int   degree = 0, n;
    HORNER *Q;
    P->fwdobs  =  horner_forward_obs;
    P->invobs  =  horner_reverse_obs;
    P->fwd3d   =  0;
    P->inv3d   =  0;
    P->fwd     =  0;
    P->inv     =  0;
    P->left    =  P->right  =  PJ_IO_UNITS_METERS;
    /* silence a few compiler warnings */
    horner_silence (0);

    /* Polynomial degree specified? */
    if (pj_param (P->ctx, P->params, "tdeg").i) /* degree specified? */
		degree = pj_param(P->ctx, P->params, "ideg").i;
    else {
        pj_log_debug (P, "Horner: Need to specify polynomial degree, (+deg=n)");
        return horner_freeup (P);
    }

    Q = horner_alloc (degree);
    P->opaque = (void *) Q;

    n = horner_number_of_coefficients (degree);

    if (0==parse_coefs (P, Q->fwd_u, "fwd_u", n))
        return horner_freeup (P);
    if (0==parse_coefs (P, Q->fwd_v, "fwd_v", n))
        return horner_freeup (P);
    if (0==parse_coefs (P, Q->inv_u, "inv_u", n))
        return horner_freeup (P);
    if (0==parse_coefs (P, Q->inv_v, "inv_v", n))
        return horner_freeup (P);

    if (0==parse_coefs (P, (double *)(Q->fwd_origin), "fwd_origin", 2))
        return horner_freeup (P);
    if (0==parse_coefs (P, (double *)(Q->inv_origin), "inv_origin", 2))
        return horner_freeup (P);
    if (0==parse_coefs (P, &Q->range, "range", 1))
        Q->range = 500000;

    return P;
}

#ifndef PJ_SELFTEST
/* selftest stub */
int pj_horner_selftest (void) {return 0;}
#else




/* ttu_n and ttu_e are based on static double C_ttu_b[] */
static  double ttu_n[] = {
    /* tc32_ed50 -> utm32_ed50 : Bornholm */
    /* m_lim_gen:  0.086  red  = 0      OBS   =    852   */
    /* m = 1.38 cm my_loss =    +2  y_enp = +10.5 */
    /* m = 1.44 cm mx_loss =    +2  x_enp = +10.4 */

    /*deg  4.0,*/
    /*Poly NORTH :: e-degree =  0  : n-degree =  4 */
    /*  0*/   6.1258112678e+06,   9.9999971567e-01,   1.5372750011e-10,
    /*  3*/   5.9300860915e-15,   2.2609497633e-19,

    /*Poly NORTH :: e-degree =  1  : n-degree =  3 */
    /*  5*/   4.3188227445e-05,   2.8225130416e-10,   7.8740007114e-16,
    /*  8*/  -1.7453997279e-19,

    /*Poly NORTH :: e-degree =  2  : n-degree =  2 */
    /*  9*/   1.6877465415e-10,  -1.1234649773e-14,  -1.7042333358e-18,
    /*Poly NORTH :: e-degree =  3  : n-degree =  1 */
    /* 12*/  -7.9303467953e-15,  -5.2906832535e-19,
    /*Poly NORTH :: e-degree =  4  : n-degree =  0 */
    /* 14*/   3.9984284847e-19,

    /*tcy    6125810.306769, */
};

static  double ttu_e[] = {
    /*Poly EAST  :: n-degree =  0  : e-degree =  4 */
    /*  0*/   8.7760574982e+05,   9.9999752475e-01,   2.8817299305e-10,
    /*  3*/   5.5641310680e-15,  -1.5544700949e-18,

    /*Poly EAST  :: n-degree =  1  : e-degree =  3 */
    /*  5*/  -4.1357045890e-05,   4.2106213519e-11,   2.8525551629e-14,
    /*  8*/  -1.9107771273e-18,

    /*Poly EAST  :: n-degree =  2  : e-degree =  2 */
    /*  9*/   3.3615590093e-10,   2.4380247154e-14,  -2.0241230315e-18,
    /*Poly EAST  :: n-degree =  3  : e-degree =  1 */
    /* 12*/   1.2429019719e-15,   5.3886155968e-19,
    /*Poly EAST  :: n-degree =  4  : e-degree =  0 */
    /* 14*/  -1.0167505000e-18,

    /* tcx     877605.269066 */
  };


/* utt_n and utt_e are based on static  double C_utt_b[] */
static  double utt_n[] = {
    /* utm32_ed50 -> tc32_ed50 : Bornholm */
    /* m_lim_gen:  0.086  red  = 0      OBS   =    852   */
    /* m = 1.38 cm my_loss =    +2  y_enp = +10.8 */
    /* m = 1.44 cm mx_loss =    +2  x_enp = +10.7 */

    /*deg  4.0,*/
    /*Poly NORTH :: e-degree =  0  : n-degree =  4 */
    /*  0*/   6.1258103208e+06,   1.0000002826e+00,  -1.5372762184e-10,
    /*  3*/  -5.9304261011e-15,  -2.2612705361e-19,

    /*Poly NORTH :: e-degree =  1  : n-degree =  3 */
    /*  5*/  -4.3188331419e-05,  -2.8225549995e-10,  -7.8529116371e-16,
    /*  8*/   1.7476576773e-19,

    /*Poly NORTH :: e-degree =  2  : n-degree =  2 */
    /*  9*/  -1.6875687989e-10,   1.1236475299e-14,   1.7042518057e-18,
    /*Poly NORTH :: e-degree =  3  : n-degree =  1 */
    /* 12*/   7.9300735257e-15,   5.2881862699e-19,
    /*Poly NORTH :: e-degree =  4  : n-degree =  0 */
    /* 14*/  -3.9990736798e-19,

    /*tcy    6125811.281773,*/
};

static  double utt_e[] = {
    /*Poly EAST  :: n-degree =  0  : e-degree =  4 */
    /*  0*/   8.7760527928e+05,   1.0000024735e+00,  -2.8817540032e-10,
    /*  3*/  -5.5627059451e-15,   1.5543637570e-18,

    /*Poly EAST  :: n-degree =  1  : e-degree =  3 */
    /*  5*/   4.1357152105e-05,  -4.2114813612e-11,  -2.8523713454e-14,
    /*  8*/   1.9109017837e-18,

    /*Poly EAST  :: n-degree =  2  : e-degree =  2 */
    /*  9*/  -3.3616407783e-10,  -2.4382678126e-14,   2.0245020199e-18,
    /*Poly EAST  :: n-degree =  3  : e-degree =  1 */
    /* 12*/  -1.2441377565e-15,  -5.3885232238e-19,
    /*Poly EAST  :: n-degree =  4  : e-degree =  0 */
    /* 14*/   1.0167203661e-18,

    /*tcx     877605.760036 */
};

UV tuut_b_origin_fwd = {877605.269066, 6125810.306769};
UV tuut_b_origin_inv = {877605.760036, 6125811.281773};
HORNER tuut_b = {4, 15, 500000.0,   ttu_e, ttu_n, utt_e, utt_n,   &tuut_b_origin_fwd, &tuut_b_origin_inv};



















/* Testing quite a bit of the pj_obs_api as a side effect (inspired by pj_obs_api_test.c) */
int pj_horner_selftest (void) {
    PJ *P;
    PJ_OBS a, b;
    int err;
    double dist;
    char *args[3] = {"proj=utm", "zone=32", "ellps=GRS80"};


    /* Log everything libproj offers to log for you */
    pj_log_level (0, PJ_LOG_TRACE);

    /* An utm projection on the GRS80 ellipsoid */
    P = pj_create ("+proj=utm +zone=32 +ellps=GRS80");
    if (0==P)
        return 1;

    /* Clean up */
    pj_free (P);

    /* Same projection, now using argc/argv style initialization */
    P = pj_create_argv (3, args);
    if (0==P)
        return 2;

    /* Turn off logging */
    pj_log_level (0, PJ_LOG_NONE);

    /* zero initialize everything, then set (longitude, latitude) to (12, 55) */
    a = pj_obs_null;
    /* a.coo.lp: The coordinate part of a, interpreted as a classic LP pair */
    a.coo.lp.lam = TORAD(12);
    a.coo.lp.phi = TORAD(55);

    /* Forward projection */
    b = pj_trans (P, PJ_FWD, a);

    /* Inverse projection */
    a = pj_trans (P, PJ_INV, b);

    /* Null projection */
    a = pj_trans (P, PJ_IDENT, a);

    /* Forward again, to get two linear items for comparison */
    a = pj_trans (P, PJ_FWD, a);

    dist = pj_xy_dist (a.coo.xy, b.coo.xy);
    if (dist > 2e-9)
        return 3;

    /* Invalid projection */
    a = pj_trans (P, 42, a);
    if (a.coo.lpz.lam!=DBL_MAX)
        return 4;
    err = pj_err_level (P, PJ_ERR_TELL);
    if (0==err)
        return 5;

    /* Clear error */
    pj_err_level (P, 0);

    /* Clean up */
    pj_free (P);

    /* Now do some 3D transformations */
    P = pj_create ("+proj=cart +ellps=GRS80");
    if (0==P)
        return 6;

    /* zero initialize everything, then set (longitude, latitude, height) to (12, 55, 100) */
    a = b = pj_obs_null;
    a.coo.lpz.lam = TORAD(12);
    a.coo.lpz.phi = TORAD(55);
    a.coo.lpz.z   = 100;

    /* Forward projection: 3D-Cartesian-to-Ellipsoidal */
    b = pj_trans (P, PJ_FWD, a);

    /* Check roundtrip precision for 10000 iterations each way */
    dist = pj_roundtrip (P, PJ_FWD, 10000, a);
    dist = pj_roundtrip (P, PJ_INV, 10000, b);
    if (dist > 2e-9)
        return 7;

    /* Inverse projection: Ellipsoidal-to-3D-Cartesian */
    b = pj_trans (P, PJ_INV, b);

    /* Move p to another context */
    pj_context_renew (P);
    b = pj_trans (P, PJ_FWD, b);

    /* Move it back to the default context */
    pj_context_free (P);
    b = pj_trans (P, PJ_INV, b);

    pj_free (P);
    return 0;
}
#endif
