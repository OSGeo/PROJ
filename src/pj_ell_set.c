/* set ellipsoid parameters a and es */
#include <string.h>
#include "proj_internal.h"
#include "projects.h"
#define SIXTH .1666666666666666667  /* 1/6 */
#define RA4   .04722222222222222222 /* 17/360 */
#define RA6   .02215608465608465608 /* 67/3024 */
#define RV4   .06944444444444444444 /* 5/72 */
#define RV6   .04243827160493827160 /* 55/1296 */

/* copy ellipsoidal parameters from src to dst */
void pj_inherit_ellipsoid_defs(const PJ *src, PJ *dst) {

    /* The linear parameters */
    dst->a  = src->a;
    dst->b  = src->b;
    dst->ra = src->ra;
    dst->rb = src->rb;

    /* The eccentricities */
    dst->alpha   = src->alpha;
    dst->e       = src->e;
    dst->es      = src->es;
    dst->e2      = src->e2;
    dst->e2s     = src->e2s;
    dst->e3      = src->e3;
    dst->e3s     = src->e3s;
    dst->one_es  = src->one_es;
    dst->rone_es = src->rone_es;

    /* The flattenings */
    dst->f   = src->f;
    dst->f2  = src->f2;
    dst->n   = src->n;
    dst->rf  = src->rf;
    dst->rf2 = src->rf2;
    dst->rn  = src->rn;

    /* This one's for GRS80 */
    dst->J = src->J;

    /* es and a before any +proj related adjustment */
    dst->es_orig = src->es_orig;
    dst->a_orig  = src->a_orig;
}

int pj_calc_ellps_params(PJ *P, double a, double es) {

    P->a = a;
    P->es = es;

    /* Compute some ancillary ellipsoidal parameters */
    P->e = sqrt(P->es);      /* eccentricity */
    P->alpha = asin (P->e);  /* angular eccentricity */

    /* second eccentricity */
    P->e2  = tan (P->alpha);
    P->e2s = P->e2 * P->e2;

    /* third eccentricity */
    P->e3    = (0!=P->alpha)? sin (P->alpha) / sqrt(2 - sin (P->alpha)*sin (P->alpha)): 0;
    P->e3s = P->e3 * P->e3;

    /* flattening */
    P->f  = 1 - cos (P->alpha);   /* = 1 - sqrt (1 - PIN->es); */
    P->rf = P->f != 0.0 ? 1.0/P->f: HUGE_VAL;

    /* second flattening */
    P->f2  = (cos(P->alpha)!=0)? 1/cos (P->alpha) - 1: 0;
    P->rf2 = P->f2 != 0.0 ? 1/P->f2: HUGE_VAL;

    /* third flattening */
    P->n  = pow (tan (P->alpha/2), 2);
    P->rn = P->n != 0.0 ? 1/P->n: HUGE_VAL;

    /* ...and a few more */
    P->b  = (1 - P->f)*P->a;
    P->rb = 1. / P->b;
    P->ra = 1. / P->a;

    P->one_es = 1. - P->es;
    if (P->one_es == 0.) {
        pj_ctx_set_errno( P->ctx, PJD_ERR_ECCENTRICITY_IS_ONE);
        return PJD_ERR_ECCENTRICITY_IS_ONE;
    }

    P->rone_es = 1./P->one_es;

    return 0;
}

/* initialize geographic shape parameters */
int pj_ell_set(projCtx ctx, paralist *pl, double *a, double *es) {
    int i;
    double b=0.0, e;
    char *name;
    paralist *start = 0;

    /* clear any previous error */
    pj_ctx_set_errno(ctx,0);

    /* check for varying forms of ellipsoid input */
    *a = *es = 0.;

    /* R takes precedence */
    if (pj_param(ctx, pl, "tR").i)
        *a = pj_param(ctx,pl, "dR").f;

    /* probable elliptical figure */
    else {
        /* check if ellps present and temporarily append its values to pl */
        if ((name = pj_param(ctx,pl, "sellps").s) != NULL) {
            char *s;

            for (start = pl; start && start->next ; start = start->next) ;
            for (i = 0; (s = pj_ellps[i].id) && strcmp(name, s) ; ++i) ;
            if (!s) { pj_ctx_set_errno( ctx, -9); return 1; }
            start->next = pj_mkparam(pj_ellps[i].major);
            start->next->next = pj_mkparam(pj_ellps[i].ell);
        }

        *a = pj_param(ctx,pl, "da").f;
        if (pj_param(ctx,pl, "tes").i) /* eccentricity squared */
            *es = pj_param(ctx,pl, "des").f;
        else if (pj_param(ctx,pl, "te").i) { /* eccentricity */
            e = pj_param(ctx,pl, "de").f;
            *es = e * e;
        } else if (pj_param(ctx,pl, "trf").i) { /* recip flattening */
            *es = pj_param(ctx,pl, "drf").f;
            if (*es == 0.0) {
                pj_ctx_set_errno( ctx, -10);
                goto bomb;
            }
            *es = 1./ *es;
            *es = *es * (2. - *es);
        } else if (pj_param(ctx,pl, "tf").i) { /* flattening */
            *es = pj_param(ctx,pl, "df").f;
            *es = *es * (2. - *es);
        } else if (pj_param(ctx,pl, "tb").i) { /* minor axis */
            b = pj_param(ctx,pl, "db").f;
            *es = 1. - (b * b) / (*a * *a);
        }     /* else *es == 0. and sphere of radius *a */
        if (b == 0.0)
            b = *a * sqrt(1. - *es);
        /* following options turn ellipsoid into equivalent sphere */
        if (pj_param(ctx,pl, "bR_A").i) { /* sphere--area of ellipsoid */
            *a *= 1. - *es * (SIXTH + *es * (RA4 + *es * RA6));
            *es = 0.;
        } else if (pj_param(ctx,pl, "bR_V").i) { /* sphere--vol. of ellipsoid */
            *a *= 1. - *es * (SIXTH + *es * (RV4 + *es * RV6));
            *es = 0.;
        } else if (pj_param(ctx,pl, "bR_a").i) { /* sphere--arithmetic mean */
            *a = .5 * (*a + b);
            *es = 0.;
        } else if (pj_param(ctx,pl, "bR_g").i) { /* sphere--geometric mean */
            *a = sqrt(*a * b);
            *es = 0.;
        } else if (pj_param(ctx,pl, "bR_h").i) { /* sphere--harmonic mean */
            if ( (*a + b) == 0.0) {
                pj_ctx_set_errno(ctx, -20);
                goto bomb;
            }
            *a = 2. * *a * b / (*a + b);
            *es = 0.;
        } else if ((i = pj_param(ctx,pl, "tR_lat_a").i) || /* sphere--arith. */
            pj_param(ctx,pl, "tR_lat_g").i) { /* or geom. mean at latitude */
            double tmp;

            tmp = sin(pj_param(ctx,pl, i ? "rR_lat_a" : "rR_lat_g").f);
            if (fabs(tmp) > M_HALFPI) {
                                pj_ctx_set_errno(ctx,-11);
                goto bomb;
            }
            tmp = 1. - *es * tmp * tmp;
            *a *= i ? .5 * (1. - *es + tmp) / ( tmp * sqrt(tmp)) :
                sqrt(1. - *es) / tmp;
            *es = 0.;
        }
bomb:
        if (start) { /* clean up temporary extension of list */
            pj_dalloc(start->next->next);
            pj_dalloc(start->next);
            start->next = 0;
        }
        if (ctx->last_errno)
            return 1;
    }
    /* some remaining checks */
    if (*es < 0.)
            { pj_ctx_set_errno( ctx, -12); return 1; }
    if (*a <= 0.)
            { pj_ctx_set_errno( ctx, -13); return 1; }
    return 0;
}
