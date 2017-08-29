#define PJ_LIB__
#include "proj_internal.h"
#include <projects.h>
#include <assert.h>
#include <stddef.h>
#include <math.h>
#include <errno.h>

PROJ_HEAD(horner,    "Horner polynomial evaluation");

/* make horner.h interface with proj's memory management */
#define horner_dealloc(x) pj_dealloc(x)
#define horner_calloc(n,x) pj_calloc(n,x)

/* The next few hundred lines is mostly cut-and-paste from the horner.h header library */

/***********************************************************************

    Interfacing to a classic piece of geodetic software

************************************************************************

    gen_pol is a highly efficient, classic implementation of a generic
    2D Horner's Scheme polynomial evaluation routine by Knud Poder and
    Karsten Engsager, originating in the vivid geodetic environment at
    what was then (1960-ish) the Danish Geodetic Institute.

    The original Poder/Engsager gen_pol implementation (where
    the polynomial degree and two sets of polynomial coefficients
    are packed together in one compound array, handled via a plain
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
	project, the material in this file introduces a version with a
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

 **********************************************************************

	The material included here was written by Knud Poder, starting
	around 1960, and Karsten Engsager, starting around 1970. It was
    originally written in Algol 60, later (1980s) reimplemented in C.

    The HORNER data type interface, and the organization as a header
    library was implemented by Thomas Knudsen, starting around 2015.

 ***********************************************************************
 *
 * Copyright (c) 2016, SDFE http://www.sdfe.dk / Thomas Knudsen / Karsten Engsager
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


struct horner;
typedef struct horner HORNER;
static UV      horner (const HORNER *transformation, enum proj_direction, UV position);
static HORNER *horner_alloc (size_t order, int complex_polynomia);
static void    horner_free (HORNER *h);

struct horner {
    int    order;    /* maximum degree of polynomium */
    int    coefs;    /* number of coefficients for each polynomium  */
    double range;    /* radius of the region of validity */

    double *fwd_u;   /* coefficients for the forward transformations */
    double *fwd_v;   /* i.e. latitude/longitude to northing/easting  */

    double *inv_u;   /* coefficients for the inverse transformations */
    double *inv_v;   /* i.e. northing/easting to latitude/longitude  */

    double *fwd_c;   /* coefficients for the complex forward transformations */
    double *inv_c;   /* coefficients for the complex inverse transformations */

    UV *fwd_origin;  /* False longitude/latitude */
    UV *inv_origin;  /* False easting/northing   */
};

/* e.g. degree = 2: a + bx + cy + dxx + eyy + fxy, i.e. 6 coefficients */
#define horner_number_of_coefficients(order) \
            (((order + 1)*(order + 2)/2))


static void horner_free (HORNER *h) {
    horner_dealloc (h->inv_v);
    horner_dealloc (h->inv_u);
    horner_dealloc (h->fwd_v);
    horner_dealloc (h->fwd_u);
    horner_dealloc (h->fwd_c);
    horner_dealloc (h->inv_c);
    horner_dealloc (h->fwd_origin);
    horner_dealloc (h->inv_origin);
    horner_dealloc (h);
}


static HORNER *horner_alloc (size_t order, int complex_polynomia) {
    /* size_t is unsigned, so we need not check for order > 0 */
    int n = (int)horner_number_of_coefficients(order);
    int polynomia_ok = 0;
    HORNER *h = horner_calloc (1, sizeof (HORNER));

    if (0==h)
        return 0;

    if (complex_polynomia)
        n = 2*(int)order + 2;
    h->order = (int)order;
    h->coefs = n;

    if (complex_polynomia) {
        h->fwd_c = horner_calloc (n, sizeof(double));
        h->inv_c = horner_calloc (n, sizeof(double));
        if (h->fwd_c && h->inv_c)
            polynomia_ok = 1;
    }
    else {
        h->fwd_u = horner_calloc (n, sizeof(double));
        h->fwd_v = horner_calloc (n, sizeof(double));
        h->inv_u = horner_calloc (n, sizeof(double));
        h->inv_v = horner_calloc (n, sizeof(double));
        if (h->fwd_u && h->fwd_v && h->inv_u && h->inv_v)
            polynomia_ok = 1;
    }

    h->fwd_origin = horner_calloc (1, sizeof(UV));
    h->inv_origin = horner_calloc (1, sizeof(UV));

    if (polynomia_ok && h->fwd_origin && h->inv_origin)
        return h;

    /* safe, since all pointers are null-initialized (by calloc) */
    horner_free (h);
    return 0;
}




/**********************************************************************/
static UV horner (const HORNER *transformation, enum proj_direction direction, UV position) {
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
    UV uv_error;
    uv_error.u = uv_error.v = HUGE_VAL;

    if (0==transformation)
        return uv_error;

    /* Check for valid value of direction (-1, 0, 1) */
    switch (direction) {
        case PJ_IDENT:    /*  no-op  */
            return position;
        case PJ_FWD:   /* forward */
        case PJ_INV:   /* inverse */
            break;
        default:   /* invalid */
            errno = EINVAL;
            return uv_error;
    }

    /* Prepare for double Horner */
    sz    =  horner_number_of_coefficients(transformation->order);
    range =  transformation->range;


    if (direction==PJ_FWD) {                              /* forward */
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
    point.coo.uv = horner ((HORNER *) P->opaque, 1, point.coo.uv);
    return point;
}

static PJ_OBS horner_reverse_obs (PJ_OBS point, PJ *P) {
    point.coo.uv = horner ((HORNER *) P->opaque, -1, point.coo.uv);
    return point;
}




/**********************************************************************/
static UV complex_horner (const HORNER *transformation, enum proj_direction direction, UV position) {
/***********************************************************************

A reimplementation of a classic Engsager/Poder Horner complex
polynomial evaluation engine.

***********************************************************************/

    /* These variable names follow the Engsager/Poder  implementation */
    int     sz;                             /* Number of coefficients */
    double *c, *cb;                           /* Coefficient pointers */
    double  range; /* Equivalent to the gen_pol's FLOATLIMIT constant */
    double  n, e, w, N, E;
    UV uv_error;
    uv_error.u = uv_error.v = HUGE_VAL;

    if (0==transformation)
        return uv_error;

    /* Check for valid value of direction (-1, 0, 1) */
    switch (direction) {
        case PJ_IDENT:    /*  no-op  */
            return position;
        case PJ_FWD:   /* forward */
        case PJ_INV:   /* inverse */
            break;
        default:   /* invalid */
            errno = EINVAL;
            return uv_error;
    }

    /* Prepare for double Horner */
    sz    =  2*transformation->order + 2;
    range =  transformation->range;

    if (direction==PJ_FWD) {                              /* forward */
        cb =  transformation->fwd_c;
        c  =  cb + sz;
        e  =  position.u - transformation->fwd_origin->u;
        n  =  position.v - transformation->fwd_origin->v;
    } else {                                              /* inverse */
        cb =  transformation->inv_c;
        c  =  cb + sz;
        e  =  position.u - transformation->inv_origin->u;
        n  =  position.v - transformation->inv_origin->v;
    }

    if ((fabs(n) > range) || (fabs(e) > range)) {
        errno = EDOM;
        return uv_error;
    }

    /* Everything's set up properly - now do the actual polynomium evaluation */
    E = *--c;
    N = *--c;
    while (c > cb) {
        w = n*E + e*N + *--c;
        N = n*N - e*E + *--c;
        E = w;
    }

    position.u = E;
    position.v = N;
    return position;
}



static PJ_OBS complex_horner_forward_obs (PJ_OBS point, PJ *P) {
    point.coo.uv = complex_horner ((HORNER *) P->opaque, PJ_FWD, point.coo.uv);
    return point;
}

static PJ_OBS complex_horner_reverse_obs (PJ_OBS point, PJ *P) {
    point.coo.uv = complex_horner ((HORNER *) P->opaque, PJ_INV, point.coo.uv);
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
    char *buf, *init, *next = 0;
    int i;

    buf = pj_calloc (strlen (param) + 2, sizeof(char));
    if (0==buf) {
        proj_log_error (P, "Horner: Out of core");
        return 0;
    }

    sprintf (buf, "t%s", param);
    if (0==pj_param (P->ctx, P->params, buf).i) {
        pj_dealloc (buf);
        return 0;
    }
    sprintf (buf, "s%s", param);
    init = pj_param(P->ctx, P->params, buf).s;
    pj_dealloc (buf);

    for (i = 0; i < ncoefs; i++) {
        if (i > 0) {
            if ( next == 0 || ','!=*next) {
                proj_log_error (P, "Horner: Malformed polynomium set %s. need %d coefs", param, ncoefs);
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
    int   degree = 0, n, complex_horner = 0;
    HORNER *Q;
    P->fwdobs  =  horner_forward_obs;
    P->invobs  =  horner_reverse_obs;
    P->fwd3d   =  0;
    P->inv3d   =  0;
    P->fwd     =  0;
    P->inv     =  0;
    P->left    =  P->right  =  PJ_IO_UNITS_METERS;

    /* Polynomial degree specified? */
    if (pj_param (P->ctx, P->params, "tdeg").i) /* degree specified? */
		degree = pj_param(P->ctx, P->params, "ideg").i;
    else {
        proj_log_debug (P, "Horner: Must specify polynomial degree, (+deg=n)");
        return horner_freeup (P);
    }

    if (pj_param (P->ctx, P->params, "tfwd_c").i || pj_param (P->ctx, P->params, "tinv_c").i) /* complex polynomium? */
		complex_horner = 1;

    Q = horner_alloc (degree, complex_horner);
    if (Q == 0)
    {
        return horner_freeup (P);
    }
    P->opaque = (void *) Q;

    if (complex_horner) {
        n = 2*degree + 2;
        if (0==parse_coefs (P, Q->fwd_c, "fwd_c", n))
            return horner_freeup (P);
        if (0==parse_coefs (P, Q->inv_c, "inv_c", n))
            return horner_freeup (P);
        P->fwdobs  =  complex_horner_forward_obs;
        P->invobs  =  complex_horner_reverse_obs;

    }
    else {
        n = horner_number_of_coefficients (degree);
        if (0==parse_coefs (P, Q->fwd_u, "fwd_u", n))
            return horner_freeup (P);
        if (0==parse_coefs (P, Q->fwd_v, "fwd_v", n))
            return horner_freeup (P);
        if (0==parse_coefs (P, Q->inv_u, "inv_u", n))
            return horner_freeup (P);
        if (0==parse_coefs (P, Q->inv_v, "inv_v", n))
            return horner_freeup (P);
    }

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
char tc32_utm32[] = {
    " +proj=horner"
    " +ellps=intl"
    " +range=500000"
    " +fwd_origin=877605.269066,6125810.306769"
    " +inv_origin=877605.760036,6125811.281773"
    " +deg=4"
    " +fwd_v=6.1258112678e+06,9.9999971567e-01,1.5372750011e-10,5.9300860915e-15,2.2609497633e-19,4.3188227445e-05,2.8225130416e-10,7.8740007114e-16,-1.7453997279e-19,1.6877465415e-10,-1.1234649773e-14,-1.7042333358e-18,-7.9303467953e-15,-5.2906832535e-19,3.9984284847e-19"
    " +fwd_u=8.7760574982e+05,9.9999752475e-01,2.8817299305e-10,5.5641310680e-15,-1.5544700949e-18,-4.1357045890e-05,4.2106213519e-11,2.8525551629e-14,-1.9107771273e-18,3.3615590093e-10,2.4380247154e-14,-2.0241230315e-18,1.2429019719e-15,5.3886155968e-19,-1.0167505000e-18"
    " +inv_v=6.1258103208e+06,1.0000002826e+00,-1.5372762184e-10,-5.9304261011e-15,-2.2612705361e-19,-4.3188331419e-05,-2.8225549995e-10,-7.8529116371e-16,1.7476576773e-19,-1.6875687989e-10,1.1236475299e-14,1.7042518057e-18,7.9300735257e-15,5.2881862699e-19,-3.9990736798e-19"
    " +inv_u=8.7760527928e+05,1.0000024735e+00,-2.8817540032e-10,-5.5627059451e-15,1.5543637570e-18,4.1357152105e-05,-4.2114813612e-11,-2.8523713454e-14,1.9109017837e-18,-3.3616407783e-10,-2.4382678126e-14,2.0245020199e-18,-1.2441377565e-15,-5.3885232238e-19,1.0167203661e-18"
};


char sb_utm32[] = {
    " +proj=horner"
    " +ellps=intl"
    " +range=500000"
    " +tolerance=0.0005"
    " +fwd_origin=4.94690026817276e+05,6.13342113183056e+06"
    " +inv_origin=6.19480258923588e+05,6.13258568148837e+06"
    " +deg=3"
    " +fwd_c=6.13258562111350e+06,6.19480105709997e+05,9.99378966275206e-01,-2.82153291753490e-02,-2.27089979140026e-10,-1.77019590701470e-09,1.08522286274070e-14,2.11430298751604e-15"
    " +inv_c=6.13342118787027e+06,4.94690181709311e+05,9.99824464710368e-01,2.82279070814774e-02,7.66123542220864e-11,1.78425334628927e-09,-1.05584823306400e-14,-3.32554258683744e-15"
};

int pj_horner_selftest (void) {
    PJ *P;
    PJ_OBS a, b, c;
    double dist;

    /* Real polynonia relating the technical coordinate system TC32 to "System 45 Bornholm" */
    P = proj_create (0, tc32_utm32);
    if (0==P)
        return 10;

    a = b = proj_obs_null;
    a.coo.uv.v = 6125305.4245;
    a.coo.uv.u =  878354.8539;

    /* Check roundtrip precision for 1 iteration each way, starting in forward direction */
    dist = proj_roundtrip (P, PJ_FWD, 1, a);
    if (dist > 0.01)
        return 1;

    /* The complex polynomial transformation between the "System Storebaelt" and utm32/ed50 */
    P = proj_create (0, sb_utm32);
    if (0==P)
        return 11;

    /* Test value: utm32_ed50(620000, 6130000) = sb_ed50(495136.8544, 6130821.2945) */
    a = b = c = proj_obs_null;
    a.coo.uv.v = 6130821.2945;
    a.coo.uv.u =  495136.8544;
    c.coo.uv.v = 6130000.0000;
    c.coo.uv.u =  620000.0000;

    /* Forward projection */
    b = proj_trans_obs (P, PJ_FWD, a);
    dist = proj_xy_dist (b.coo.xy, c.coo.xy);
    if (dist > 0.001)
        return 2;

    /* Inverse projection */
    b = proj_trans_obs (P, PJ_INV, c);
    dist = proj_xy_dist (b.coo.xy, a.coo.xy);
    if (dist > 0.001)
        return 3;

    /* Check roundtrip precision for 1 iteration each way */
    dist = proj_roundtrip (P, PJ_FWD, 1, a);
    if (dist > 0.01)
        return 4;

    proj_destroy(P);
    return 0;
}
#endif
