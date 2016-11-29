/***********************************************************************

    Interfacing to a classic piece of geodetic software

************************************************************************

    gen_pol is a highly efficient, classic implementation of a generic
    2D Horner's Scheme polynomial evaluation routine by Knud Poder and
    Karsten Engsager, originating in the vivid geodetic environment at
    what was then (1960-ish) the Danish Geodetic Institute.

    The original Poder/Engsager gen_pol implementation (where
    the polynomial degree and two sets of polynomial coefficients
    are packed together in one compound array, handled via a simple
    double pointer) is compelling and "true to the code history":

    It has a beautiful classical 1960s ring to it, not unlike the
    original fft implementations, which revolutionized spectral
    analysis in twenty lines of code.

    The Poder coding sound, as classic 1960s as Phil Spector's Wall
    of Sound, is beautiful and inimitable.

	On the other hand: For the uninitiated, the gen_pol code is hard
    to follow, despite being compact.

    Also, since adding metadata and improving maintainability
    of the code are among the implied goals of a current SDFE/DTU Space
	project, the material in this file introduces a version with a more
	more modern (or at least 1990s) look, introducing a "double 2D
	polynomial" data type, HORNER.

    Despite introducing a new data type for handling the polynomial
    coefficients, great care has been taken to keep the coefficient
    array organization identical to that of gen_pol.

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
	around 1960, Karsten Engsager, starting around 1970. It was
    originally written in Algol 60, later (1980s) reimplemented in C.

    The HORNER data type interface, and the organization as a header
    library was implemented by Thomas Knudsen, starting around 2015.

 ***********************************************************************
 *
 * The gen_pol routine comes with this legal statement (ISC/OpenBSD):
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


 #ifndef MORNER_H
 #define HORNER_H
 #ifdef __cplusplus
 extern "C" {
 #endif



#if defined(PROJ_H) || defined(PROJECTS_H)
#define horner_dealloc(x) pj_dealloc(x)
#define horner_calloc(n,x) pj_calloc(n,x)
#else
#define horner_dealloc(x) free(x)
#define horner_calloc(n,x) calloc(n,x)
typedef struct {double u,v;} UV;
#endif

static const UV uv_error = {DBL_MAX, DBL_MAX};

struct horner;
typedef struct horner HORNER;
static UV      horner (const HORNER *transformation, int direction, UV position);
static HORNER *horner_alloc (size_t order);
static void    horner_free (HORNER *h);

struct horner {
    int    order;    /* maximum degree of polynomium */
    int    coefs;    /* number of coefficients for each polynomium  */
    double range;    /* radius of the region of validity */

    double *fwd_u;   /* coefficients for the forward transformations */
    double *fwd_v;   /* i.e. latitude/longitude to northing/easting  */

    double *inv_u;   /* coefficients for the inverse transformations */
    double *inv_v;   /* i.e. northing/easting to latitude/longitude  */

    UV *fwd_origin;  /* False longitude/latitude */
    UV *inv_origin;  /* False easting/northing   */
};

/* e.g. degree = 2: a + bx + cy + dxx + eyy + fxy, i.e. 6 coefficients */
#define horner_number_of_coefficients(order) \
            (((order + 1)*(order + 2)/2))

static int     horner_degree_u (int order, int index);
static int     horner_degree_v (int order, int index);
static int     horner_index (int order, int degree_u, int degree_v);

#ifndef HORNER_HEADER_ONLY

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




static void horner_free (HORNER *h) {
    horner_dealloc (h->inv_v);
    horner_dealloc (h->inv_u);
    horner_dealloc (h->fwd_v);
    horner_dealloc (h->fwd_u);
    horner_dealloc (h);
}


static HORNER *horner_alloc (size_t order) {
    /* size_t is unsigned, so we need not check for order > 0 */
    int n = horner_number_of_coefficients(order);

    HORNER *h = horner_calloc (1, sizeof (HORNER));

    if (0==h)
        return 0;

    h->order = order;
    h->coefs = n;

    h->fwd_u = horner_calloc (n, sizeof(double));
    h->fwd_v = horner_calloc (n, sizeof(double));
    h->inv_u = horner_calloc (n, sizeof(double));
    h->inv_v = horner_calloc (n, sizeof(double));

    h->fwd_origin = horner_calloc (1, sizeof(UV));
    h->inv_origin = horner_calloc (1, sizeof(UV));

    if (h->fwd_u && h->fwd_v && h->inv_u && h->inv_v && h->fwd_origin && h->inv_origin)
        return h;

    /* safe, since all pointers are null-initialized (by calloc) */
    horner_free (h);
    return 0;
}




/**********************************************************************/
static UV horner (const HORNER *transformation, int direction, UV position) {
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

    if (0==transformation)
        return uv_error;

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



/**********************************************************************/
static int horner_silence (int i) {
/***********************************************************************
  useless function that silences coompiler warnings about unused stuff
***********************************************************************/
    HORNER *Q;
    if (i==0)
        return i;
    horner(0, 1, uv_error);
    Q = horner_alloc (2);
    if (Q)
        horner_free (Q);
    if (horner_degree_u (2,1))
        return horner_degree_v (2,1);
    return i;
}





#ifdef HORNER_TEST_ORIGINAL_GEN_POL_CODE

double fwd_u[] = {1,2,3};
double fwd_v[] = {4,5,6};
double inv_u[] = {4,6,5};
double inv_v[] = {1,3,2};
UV uv_origin_fwd = {0, 0};
UV uv_origin_inv = {0, 0};
HORNER uv = {1, 3, 500000.0, fwd_u, fwd_v, inv_u, inv_v, &uv_origin_fwd, &uv_origin_inv};

void show_test (void);
void tuut_b_test (void);


int main (int argc, char **argv) {
    int i, j, order = 10;
    UV res = {1,1};

    if (argc==2)
        order = atoi (argv[1]);

    printf ("Testing %d combinations\n", horner_number_of_coefficients(order));

    for (i = 0; i <= order; i++)
        for (j = 0; j <= order - i; j++) {
            int hh = horner_index (order, i, j);
            int ii = horner_degree_u (order, hh);
            int jj = horner_degree_v (order, hh);
            assert (i==ii);
            assert (j==jj);
            printf ("%2.2d  %1d%1du  %1d%1dv\n", hh, i, ii, j, jj);
        }

    tuut_b_test ();

    puts ("Forward...");

    printf ("inp = {%.4f, %.4f}\n", res.u, res.v);
    res = horner (&uv, 1, res);
    printf ("res = {%.4f, %.4f}\n", res.u, res.v);
    assert ( 6==res.u);  /* fwd_u:  a + bu + cv  =  1 + 2*1 + 3*1  =   6 */
    assert (15==res.v);  /* fwd_v:  a + bu + cv  =  4 + 5*1 + 6*1  =  15 */

    res.u = 2;
    printf ("inp = {%.4f, %.4f}\n", res.u, res.v);
    res = horner (&uv, 1, res);
    printf ("res = {%.4f, %.4f}\n", res.u, res.v);      /* u = 2,  v = 15 */
    assert (50==res.u);  /* fwd_u:  a + bu + cv  =  1 + 2*(u=2)  + 3*(v=15)  =  50 */
    assert (91==res.v);  /* fwd_v:  a + bu + cv  =  4 + 5*(v=15) + 6*(u=2)   =  91 */

    puts ("Inverse...");

    res.u = 1;
    res.v = 1;
    printf ("inp = {%.4f, %.4f}\n", res.u, res.v);
    res = horner (&uv, -1, res);
    printf ("res = {%.4f, %.4f}\n", res.u, res.v);      /* u = 1,  v = 1 */
    assert (15==res.u);  /* inv_u:  a + bu + cv  =  4 + 5*1 + 6*1  =  15 */
    assert ( 6==res.v);  /* inv_v:  a + bu + cv  =  1 + 2*1 + 3*1  =   6 */

    res.u = 2;
    printf ("inp = {%.4f, %.4f}\n", res.u, res.v);
    res = horner (&uv, -1, res);
    printf ("res = {%.4f, %.4f}\n", res.u, res.v);      /* u = 2,  v = 6 */
    assert (46==res.u);  /* inv_u:  a + bu + cv  =  4 + 6*(u=2) + 5*(v=6)  =  46 */
    assert (23==res.v);  /* inv_v:  a + bu + cv  =  1 + 2*(u=2) + 3*(v=6)  =  23 */

/*  show_test ();*/
    return 0;
}






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


/* Prototype and forward declarations of the material needed for cross-check with original implementation */
int gen_pol(double *C_f, double *C_r, double N_in, double E_in, double *Nout, double *Eout);
static  double      C_ttu_b[];
static  double      C_utt_b[];


void gen_pol_roundtrip (double *C_ttu_b, double *C_utt_b, UV fwd) {
    UV res, hrn;
    int ret;

    ret = gen_pol(C_ttu_b, C_utt_b, fwd.v, fwd.u, &res.v, &res.u);
    printf ("\n------\n\n");
	if (0!=ret) printf ("ret: %d\n", ret);
    printf ("inp: %11.3f  %11.3f\n", fwd.u, fwd.v);
    printf ("res: %11.3f  %11.3f\n", res.u, res.v);
    hrn = horner (&tuut_b, 1, fwd);
    printf ("hrn: %11.3f  %11.3f\n", hrn.u, hrn.v);
    assert (hrn.u==res.u);
    assert (hrn.v==res.v);

    ret = gen_pol(C_utt_b, C_ttu_b, res.v, res.u, &res.v, &res.u);
    hrn = horner (&tuut_b, -1, hrn);
    printf ("hrn: %11.3f  %11.3f\n", hrn.u, hrn.v);
	if (0!=ret) printf ("ret: %d\n", ret);
    printf ("res: %11.3f  %11.3f\n",   res.u, res.v);
    assert (hrn.u==res.u);
    assert (hrn.v==res.v);
    printf ("inp: %11.3f  %11.3f  (%.3g mm)\n", fwd.u, fwd.v, 1e3*hypot(fwd.u-res.u, fwd.v-res.v));
}

void tuut_b_test (void) {
    UV fwd = tuut_b_origin_fwd;
    UV res;

    puts ("Bornholm");
    printf ("fwd: %11.3f  %11.3f\n", fwd.u, fwd.v);
    res = horner (&tuut_b, 1, fwd);
    printf ("res: %11.3f  %11.3f\n", res.u, res.v);
    res = horner (&tuut_b, -1, res);
    printf ("res: %11.3f  %11.3f\n", res.u, res.v);
    printf ("fwd: %11.3f  %11.3f\n", fwd.u, fwd.v);

    gen_pol_roundtrip (C_ttu_b, C_utt_b, fwd);
    fwd.u = 877000;
    fwd.v = 6125000;
    gen_pol_roundtrip (C_ttu_b, C_utt_b, fwd);
    fwd.u = 800000;
    fwd.v = 6100000;
    gen_pol_roundtrip (C_ttu_b, C_utt_b, fwd);
    fwd.u = 850000;
    fwd.v = 6200000;
    gen_pol_roundtrip (C_ttu_b, C_utt_b, fwd);

}





#define            FLOATLIMIT      5.0e5
#define TRF_AREA_ EDOM

/********************************************************************************************/
int gen_pol(double *C_f, double *C_r, double N_in, double E_in, double *Nout, double *Eout) {
/*********************************************************************************************

    This is the original Poder/Engsager implementation of gen_pol.
	It is included here for test-comparison with the horner() routine.

*********************************************************************************************/
  double             N, E, n, e;
  double            *Cp, *tcy, *tcx;
  double             tol = 1.0e-4;
  int                i;
  int                g;
  int                sz;
  int                r, c;
  int                res = 0;

  /* Preserve input for reverse check */
  N  = N_in;
  E  = E_in;
  Cp = C_f;

  /* Transformation loop */
  for (i = -1; i <= 1 && res == 0; ++i)
    if (i) {

      /* Prepare for double Horner */
      g   = (int) *Cp;
      sz  = (g + 1)*(g + 2)/2 + 1;
      tcy = Cp + sz;
      tcx = tcy  + sz;
      /* Double Horner's scheme */
      /* N = n*Cy*e -> yout, E = e*Cx*n -> xout */
      n   = N - *tcy;
      e   = E - *tcx;
      if ((fabs(n) < FLOATLIMIT) && (fabs(e) < FLOATLIMIT)) {

        for (  r = g, N = *--tcy, E = *--tcx; r >  0; r--) {
          double u, v;
          for (c = g, u = *--tcy, v = *--tcx; c >= r; c--) {
            u = n*u + *--tcy;
            v = e*v + *--tcx;
          }
          N = e*N + u;
          E = n*E + v;
        }
      } else res = TRF_AREA_;
    }
    else { /* collect output coord,switch to reverse checking */
      *Nout = N;
      *Eout = E;
      Cp    = C_r;
    }

  /* tol-check of results*/
  if (res == 0 && (fabs(N - N_in) < tol && fabs(E - E_in) < tol))
      return (0);
  else if (res == 0) res = TRF_AREA_;
  return(res);

#undef   FLOATLIMIT

}


/* s45b polynomia */
  static  double      C_ttu_b[] = {
    /* tc32_ed50 -> utm32_ed50 : Bornholm */
    /* m_lim_gen:  0.086  red  = 0      OBS   =    852   */
    /* m = 1.38 cm my_loss =    +2  y_enp = +10.5 */
    /* m = 1.44 cm mx_loss =    +2  x_enp = +10.4 */

    /*deg*/  4.0,
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

    /*tcy*/    6125810.306769,

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

    /*tcx*/     877605.269066
  };

  static  double      C_utt_b[] = {
    /* utm32_ed50 -> tc32_ed50 : Bornholm */
    /* m_lim_gen:  0.086  red  = 0      OBS   =    852   */
    /* m = 1.38 cm my_loss =    +2  y_enp = +10.8 */
    /* m = 1.44 cm mx_loss =    +2  x_enp = +10.7 */

    /*deg*/  4.0,
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

    /*tcy*/    6125811.281773,

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

    /*tcx*/     877605.760036
  };

#endif /* HORNER_TEST_ORIGINAL_GEN_POL_CODE */


#endif /* ndef HORNER_HEADER_ONLY */

#ifdef __cplusplus
}
#endif

#endif /* ndef HORNER_H */
