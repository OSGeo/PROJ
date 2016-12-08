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
int pj_horner_selftest (void) {
    PJ *P;
    PJ_OBS a, b;
    double dist;

    /* The TC32 for "System 45 Bornholm" */
    /*    TC32 -> UTM32"    */
    P = pj_create ("+init=../examples/s45b.pol:tc32_utm32");
    if (0==P)
        return 10;

    a = b = pj_obs_null;
    a.coo.uv.v = 6125305.4245;
    a.coo.uv.u =  878354.8539;

    /* Forward projection */
    b = pj_trans (P, PJ_FWD, a);
    b = pj_trans (P, PJ_INV, b);

    /* Check roundtrip precision for 1 iteration each way */
    dist = pj_roundtrip (P, PJ_FWD, 1, a);
    if (dist > 0.01)
        return 1;

    /* The "System 45 Bornholm" */
    P = pj_create ("+init=../examples/s45b.pol:s45b_tc32");
    if (0==P)
        return 20;

    a = b = pj_obs_null;
    a.coo.uv.u =   47022.563745;
    a.coo.uv.v =   51779.260103;

    /* Forward projection */
    b = pj_trans (P, PJ_FWD, a);
    b = pj_trans (P, PJ_INV, b);

    /* Check roundtrip precision for 1 iteration each way */
    dist = pj_roundtrip (P, PJ_FWD, 1, a);
    pj_free (P);

    if (dist > 0.01)
        return 1;

    return 0;
}
#endif
