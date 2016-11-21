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
PROJ_HEAD(horner,    "Horner polynomial evaluation");





UV uv_error = {DBL_MAX, DBL_MAX};



/* Header file contents */
struct horner;
typedef struct horner HORNER;


static UV      horner (HORNER *transformation, int direction, UV position);
static HORNER *horner_alloc (size_t order);
static void    horner_free (HORNER *h);
/* End of header file contents */


struct horner {
    int    order;    /* maximum power of polynomium */
    int    coefs;    /* number of coefficients for each polynomium  */
    double range;    /* radius of the region of validity */

    double *fwd_u;   /* coefficients for the forward transformations */
    double *fwd_v;   /* i.e. latitude/longitude to northing/easting  */

    double *inv_u;   /* coefficients for the inverse transformations */
    double *inv_v;   /* i.e. northing/easting to latitude/longitude  */

    UV *fwd_origin;  /* False longitude/latitude */
    UV *inv_origin;  /* False easting/northing   */
};

/* e.g. power = 2: a + bx + cy + dxx + eyy + fxy, i.e. 6 coefficients */
#define horner_number_of_coefficients(order) \
            (((order + 1)*(order + 2)/2))



static void horner_free (HORNER *h) {
    pj_dealloc (h->inv_v);
    pj_dealloc (h->inv_u);
    pj_dealloc (h->fwd_v);
    pj_dealloc (h->fwd_u);
    pj_dealloc (h);
}

static HORNER *horner_alloc (size_t order) {
    /* size_t is unsigned, so we need not check for order > 0 */
    int n = horner_number_of_coefficients(order);

    HORNER *h = pj_calloc (1, sizeof (HORNER));

    if (0==h)
        return 0;

    h->order = order;
    h->coefs = n;

    h->fwd_u = pj_calloc (n, sizeof(double));
    h->fwd_v = pj_calloc (n, sizeof(double));
    h->inv_u = pj_calloc (n, sizeof(double));
    h->inv_v = pj_calloc (n, sizeof(double));

    if (h->fwd_u && h->fwd_v && h->inv_u && h->inv_v)
        return h;

    /* safe, since all pointers are null-initialized (by calloc) */
    horner_free (h);
    return 0;
}




/**********************************************************************/
static UV horner (HORNER *transformation, int direction, UV position) {
/***********************************************************************

A reimplementation of the classic Engsager/Poder 2D Horner polynomial
evaluation engine "gen_pol".

This version omits the inimitable Poder "dual autocheck"-machinery,
which here is intended to be implemented at a higher level of the
library: We separate the polynomial evaluation from the quality
control (which, given the limited MTBF for "computing machinery",
typical when Knud Poder invented the dual autocheck method,
was not defensible at that time).

Another difference from the original version is that we return the
result on the stack, rather than accepting pointers to result variables
as input. This results in code that is easy to read:

            projected  = horner (s34j,  1, geographic);
            geographic = horner (s34j, -1, projected );

and experiments have shown that on contemporary architectures, the time
taken for returning even comparatively large objects on the stack (and
the UV is not that large - typically only 16 bytes) is negligibly
different from passing two pointers (i.e. typically also 16 bytes) the
other way.

The polynomium has the form:

P = sum (i = [0 : order])
        sum (j = [0 : order - i])
            pow(par_1, i) * pow(par_2, j) * coef(index(order, i, j))

For numerical stability, the summation is carried out backwards,
summing the tiny high order elements first.

***********************************************************************/

    /* These variable names follow the Engsager/Poder  implementation */
    int     sz;              /* Number of coefficients per polynomial */
    double *tcx, *tcy;                        /* Coefficient pointers */
    double  range; /* Equivalent to the gen_pol's FLOATLIMIT constant */
    double  n, e;

    /* Check for valid value of direction (-1, 0, 1) */
    switch (direction) {
        case 0:    /*  no-op  */
            return position;
        case  1:   /* forward */
        case -1:   /* inverse */
            break;
        default:   /* invalid */
            errno = EINVAL;
            return uv_error;
    }

    /* Prepare for double Horner */
    sz    =  horner_number_of_coefficients(transformation->order);
    range =  transformation->range;


    if (direction==1) {                                   /* forward */
        tcx = transformation->fwd_u + sz;
        tcy = transformation->fwd_v + sz;
        e   = position.u - transformation->fwd_origin->u;
        n   = position.v - transformation->fwd_origin->v;
    } else {                                              /* inverse */
        tcx = transformation->inv_u + sz;
        tcy = transformation->inv_v + sz;
        e   = position.u - transformation->inv_origin->u;
        n   = position.v - transformation->inv_origin->v;
    }

    if ((fabs(n) > range) || (fabs(e) > range)) {
        errno = EDOM;
        return uv_error;
    }

    /* The melody of this block is straight out of the great Engsager/Poder songbook */
    else {
        int g =  transformation->order;
        int r = g, c;
        double u, v, N, E;

        /* Double Horner's scheme: N = n*Cy*e -> yout, E = e*Cx*n -> xout */
        for (N = *--tcy,  E = *--tcx;    r > 0;    r--) {
            for (c = g,  u = *--tcy,  v = *--tcx;    c >= r;    c--) {
                u = n*u + *--tcy;
                v = e*v + *--tcx;
            }
            N = e*N + u;
            E = n*E + v;
        }

        position.u = E;
        position.v = N;
    }

    return position;
}





static PJ_OBS horner_forward_obs (PJ_OBS point, PJ *P) {
    (void) P;
    return point;
}

static PJ_OBS horner_reverse_obs (PJ_OBS point, PJ *P) {
    (void) P;
    return point;
}


#if 0
static XY horner_forward (LP lp, PJ *P) {
    PJ_TRIPLET point;
    point.lp = lp;
    point.lpz.z = 0;
    /*********************************************************************/
    /*                                                                   */
    /* For historical reasons, in proj, plane coordinates are measured   */
    /* in units of the semimajor axis. Since 3D handling is grafted on   */
    /* later, this is not the case for heights. And even though this     */
    /* coordinate really is 3D cartesian, the z-part looks like a height */
    /* to proj. Hence, we have the somewhat unusual situation of having  */
    /* a point coordinate with differing units between dimensions.       */
    /*                                                                   */
    /* The scaling and descaling is handled by the pj_fwd/inv functions. */
    /*                                                                   */
    /*********************************************************************/
    xyz.x /= P->a;
    xyz.y /= P->a;

    point.xyz = cartesian (point.lpz, P);
    return point.xy;
}

/* And the other way round. Still rather pointless, but... */
static LP cart_reverse (XY xy, PJ *P) {
    PJ_TRIPLET point;
    point.xy = xy;
    point.xyz.z = 0;

    point.lpz = geodetic (point.xyz, P);
    return point.lp;
}
#endif




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


static int parse_coefs (PJ *P, double *coefs, char *param, int degree) {
    char buf[20], *init, *next;
    int i, n;
    sprintf (buf, "t%s", param);
    if (0==pj_param (P->ctx, P->params, buf).i)
        return 0;
    sprintf (buf, "s%s", param);
    init = pj_param(P->ctx, P->params, buf).s;

    n = horner_number_of_coefficients (degree);

    for (i = 0; i < n; i++) {
        if (i > 0) {
            if (','!=*next) {
                pj_log_error (P, "Horner: Malformed polynomium set %s. need %d coefs", param, n);
                return 0;
            }
            init = ++next;
        }
        printf ("parse[%2.2d]: %s\n", i, init);
        coefs[i] = strtod (init, &next);
    }
    return 1;
}


/*********************************************************************/
PJ *PROJECTION(horner) {
/*********************************************************************/
    int   degree = 0;
    HORNER *Q;
    P->fwdobs  =  horner_forward_obs;
    P->invobs  =  horner_reverse_obs;
    P->fwd3d   =  0;
    P->inv3d   =  0;
    P->fwd     =  0;
    P->inv     =  0;

    if (pj_param (P->ctx, P->params, "tdeg").i) /* degree specified? */
		degree = pj_param(P->ctx, P->params, "ideg").i;
    else
        return horner_freeup (P);

    Q = horner_alloc (degree);
    P->opaque = (void *) Q;

    /* degree specified? */
    if (pj_param (P->ctx, P->params, "tdeg").i)
		degree = pj_param(P->ctx, P->params, "ideg").i;
    else {
        pj_log_error (P, "Horner: Must specify polynomial degree (+deg=N)");
        return horner_freeup (P);
    }

    if (0==parse_coefs (P, Q->fwd_u, "fwd_u", degree))
        return horner_freeup (P);
    if (0==parse_coefs (P, Q->fwd_v, "fwd_v", degree))
        return horner_freeup (P);
    if (0==parse_coefs (P, Q->inv_u, "inv_u", degree))
        return horner_freeup (P);
    if (0==parse_coefs (P, Q->inv_v, "inv_v", degree))
        return horner_freeup (P);

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










static int     horner_degree_u (int order, int index);
static int     horner_degree_v (int order, int index);
static int     horner_index (int order, int degree_u, int degree_v);


/***************************************************************************/
static int horner_index (int order, int degree_1, int degree_2) {
/****************************************************************************

    Returns the index of the polynomial coefficient, C, for the element

              C * pow (c_1, degree_2) * pow (c_2, degree_2),

    given that degree_1 > -1, degree_2 > -1,  degree_1 + degree_2 <= order.

    Otherwise returns -1 and sets errno to EDOM.

    The range of the index is [0 : (order + 1) * (order + 2) / 2 - 1].

    A very important thing to note  is that the order of the coordinates
    c_1 and c_2 depend on the polynomium:

    For the fwd and inv polynomia for the "u" coordinate,
    u is first (degree_1), v is second (degree_2).
    For the fwd and inv polynomia for the "v" coordinate,
    v is first (degree_1), u is second (degree_2).

****************************************************************************/

    if ( (degree_1 < 0)  ||  (degree_2 < 0)  ||  (degree_1 + degree_2 > order) ) {
        errno = EDOM;
        return -1;
    }

    return (   horner_number_of_coefficients(order) - 1
             - (order - degree_1)*(order - degree_1 + 1)/2
             - (order - degree_1 - degree_2));
}

#define index_u(h, u, v) horner_index (h->order, u, v)
#define index_v(h, u, v) horner_index (h->order, v, u)


static int horner_degree_u (int order, int index) {
    int n = horner_number_of_coefficients(order);
    int i, j;
    if ((order < 0) || (index >= n)) {
        errno = EDOM;
        return -1;
    }
    for (i = 0; i <= order; i++)
        for (j = 0; j <= order - i; j++)
            if (index == horner_index (order, i, j))
                return i;
    return -1;
}


static int horner_degree_v (int order, int index) {
    int n = horner_number_of_coefficients(order);
    int i, j;
    if ((order < 0) || (index >= n)) {
        errno = EDOM;
        return -1;
    }
    for (i = 0; i <= order; i++)
        for (j = 0; j <= order - i; j++)
            if (index == horner_index (order, i, j))
                return j;
    return -1;
}










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
