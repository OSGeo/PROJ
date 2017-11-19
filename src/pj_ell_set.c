/* set ellipsoid parameters a and es */
#include <string.h>
#include <errno.h>
#include <proj.h>
#include "proj_internal.h"
#include "projects.h"

/* series coefficients for calculating ellipsoid-equivalent spheres */
#define SIXTH .1666666666666666667  /* 1/6 */
#define RA4   .04722222222222222222 /* 17/360 */
#define RA6   .02215608465608465608 /* 67/3024 */
#define RV4   .06944444444444444444 /* 5/72 */
#define RV6   .04243827160493827160 /* 55/1296 */

/* Helper function prototypes for pj_ellipsoid */
static paralist *pj_get_param (paralist *list, char *key);
static PJ_ELLPS *pj_find_ellps (char *name);
static int pj_spherification (PJ *P);
static int pj_size (PJ *P);
static int pj_shape (PJ *P);
static char *pj_param_value (paralist *list);


/***************************************************************************************/
int pj_ellipsoid (PJ *P) {
/****************************************************************************************
    This is a replacement for the clasic PROJ pj_ell_set function. The main difference
    is that pj_ellipsoid augments the PJ object with a copy of the exact tags used to
    define its related ellipsoid.

    This makes it possible to let a new PJ object inherit the geometrical properties
    of an existing one.

    A complete ellipsoid definition comprises a size (primary) and a shape (secondary)
    parameter.

    Size parameters supported are:
        R, defining the radius of a spherical planet
        a, defining the semimajor axis of an ellipsoidal planet

    Shape parameters supported are:
        rf, the reverse flattening of the ellipsoid
        f,  the flattening of the ellipsoid
        es, the eccentricity squared
        e,  the eccentricity
        b,  the semiminor axis

    The ellps=xxx parameter provides both size and shape for a number of built in
    ellipsoid definitions.

    The ellipsoid definition may be augmented with a spherification flag, turning
    the ellipsoid into a sphere with features defined by the ellipsoid.

    Spherification parameters supported are:
        R_A, which gives a sphere with the same surface area as the ellipsoid
        R_A, which gives a sphere with the same volume as the ellipsoid

        R_a, which gives a sphere with R = (a + b)/2   (arithmetic mean)
        R_g, which gives a sphere with R = sqrt(a*b)   (geometric mean)
        R_h, which gives a sphere with R = 2*a*b/(a+b) (harmonic mean)

        R_lat_a=phi, which gives a sphere with R being the arithmetic mean of
             of the corresponding ellipsoid at latitude phi.
        R_lat_g=phi, which gives a sphere with R being the geometric mean of
             of the corresponding ellipsoid at latitude phi.

    If R is given as size parameter, any shape and spherification parameters
    given are ignored.

****************************************************************************************/
    PJ B;
    int   last_errno;
    char *name;
    PJ_ELLPS *ellps;

    last_errno = proj_errno_reset (P);

    P->def_size = P->def_shape = P->def_spherification = 0;

    /* This amounts to zeroing all ellipsoidal parameters */
    memset (&B, 0, sizeof (B));
    pj_inherit_ellipsoid_defs(&B, P);

    if (0 != pj_size (P))
        return proj_errno (P);

    /* Now, if *R* given ... */
    if ('R'==P->def_size[0]) {
        P->a = pj_atof (P->def_size+2);
        if (HUGE_VAL==P->a)
            return proj_errno_set (P, PJD_ERR_INVALID_ARG);
        if (0>=P->a)
            return proj_errno_set (P, PJD_ERR_INVALID_ARG);
        P->b = P->a;
        P->f = P->es = P->e = 0;
        P->rf = HUGE_VAL;
        pj_calc_ellps_params (P, P->a, 0);
        if (proj_errno (P))
            return proj_errno (P);
        return proj_errno_restore (P, last_errno);
    }

    /* if *a* given ... */
    if ('a'==P->def_size[0]) {
        if (strlen (P->def_size) < 3)
            return proj_errno_set (P, PJD_ERR_INVALID_ARG);
        P->a = pj_atof (P->def_size+2);
        if (HUGE_VAL==P->a)
            return proj_errno_set (P, PJD_ERR_INVALID_ARG);
        if (0==P->a)
            return proj_errno_set (P, PJD_ERR_INVALID_ARG);
        pj_shape (P);
        pj_spherification (P);
        if (proj_errno (P))
            return proj_errno (P);
        return proj_errno_restore (P, last_errno);
    }


    /* If we got so far, it's because *ellps* was given. */
    /* Now find the correct ellipsoid definition */
    if (strlen (P->def_size) < 7)
        return proj_errno_set (P, PJD_ERR_INVALID_ARG);
    name = P->def_size+6;
    ellps = pj_find_ellps (name);
    if (0==ellps)
        return proj_errno_set (P, PJD_ERR_UNKNOWN_ELLP_PARAM);

    /* Now, we have the right size and shape parameters and can produce */
    /* a fake PJ to make pj_shape do the hard work for us */
    B = *P;
    B.params = pj_mkparam (ellps->major);
    B.params->next = pj_mkparam (ellps->ell);
    /* Add the original stuff from P (needed for spherification) */
    B.params->next->next = P->params;

    pj_size (&B);
    pj_shape (&B);
    pj_spherification (&B);
    P->a = B.a;
    P->b = B.b;
    P->f = B.f;
    P->e = B.e;
    P->es = B.es;
    pj_calc_ellps_params (P, P->a, P->es);
    pj_dealloc (B.params->next);
    pj_dealloc (B.params);
    if (proj_errno (P))
        return proj_errno (P);

    /* success - restore previous error status */
    return proj_errno_restore (P, last_errno);
}


/* locate parameter in list */
static paralist *pj_get_param (paralist *list, char *key) {
    size_t l = strlen(key);
	while (list && !(0==strncmp(list->param, key, l) && (0==list->param[l] || list->param[l] == '=') ) )
		list = list->next;
	return list;
}


static char *pj_param_value (paralist *list) {
    char *key, *value;
    if (0==list)
        return 0;

    key = list->param;
    value = strchr (key, '=');

    /* a flag (i.e. a key without value) has its own name (key) as value */
    return value? value + 1: key;
}


static PJ_ELLPS *pj_find_ellps (char *name) {
    int i;
    char *s;
    if (0==name)
        return 0;

    /* Search through internal ellipsoid list for name */
    for (i = 0; (s = pj_ellps[i].id) && strcmp(name, s) ; ++i);
    if (0==s)
        return 0;
    return pj_ellps + i;
}


static int pj_spherification (PJ *P) {
    char *keys[] =  {"R_A", "R_V", "R_a", "R_g", "R_h", "R_lat_a", "R_lat_g"};
    size_t len, i;
    paralist *par = 0;
    char *def = 0;

    double t;
    char *v, *endp;

    len = sizeof (keys) /  sizeof (char *);
    P->def_spherification = 0;

    /* Check which spherification key is specified */
    for (i = 0;  i < len;  i++) {
        par = pj_get_param (P->params, keys[i]);
        if (par)
            break;
    }

    /* No spherification specified? Then we're done */
    if (i==len)
        return 0;

    /* Store definition */
    P->def_spherification = def = par->param;
    par->used = 1;

    switch (i) {

    /* R_A - a sphere with same area as ellipsoid */
    case 0:
        P->a *= 1. - P->es * (SIXTH + P->es * (RA4 + P->es * RA6));
        P->es = P->e = P->rf = P->f = P->b = 0;
        break;

    /* R_V - a sphere with same volume as ellipsoid */
    case 1:
        P->a *= 1. - P->es * (SIXTH + P->es * (RV4 + P->es * RV6));
        P->es = P->e = P->rf = P->f = P->b = 0;
        break;

    /* R_a - a sphere with R = the arithmetic mean of the ellipsoid */
    case 2:
        P->a = (P->a + P->b) / 2;
        P->es = P->e = P->rf = P->f = P->b = 0;
        break;

    /* R_g - a sphere with R = the geometric mean of the ellipsoid */
    case 3:
        P->a = sqrt (P->a * P->b);
        P->es = P->e = P->rf = P->f = P->b = 0;
        break;

    /* R_h - a sphere with R = the harmonic mean of the ellipsoid */
    case 4:
        if (P->a + P->b == 0)
            return proj_errno_set (P, PJD_ERR_TOLERANCE_CONDITION);
        P->a = (2*P->a * P->b) / (P->a + P->b);
        P->es = P->e = P->rf = P->f = P->b = 0;
        break;

    /* R_lat_a - a sphere with R = the arithmetic mean of the ellipsoid at given latitude */
    case 5:
    /* R_lat_g - a sphere with R = the geometric  mean of the ellipsoid at given latitude */
    case 6:
        v = pj_param_value (par);
        t = proj_dmstor (v, &endp);
        if (fabs (t) > M_HALFPI)
            return proj_errno_set (P, PJD_ERR_REF_RAD_LARGER_THAN_90);
        t = sin (t);
        t = 1 - P->es * t * t;
        if (i==5)   /* arithmetic */
            P->a *= (1. - P->es + t) / (2 * t * sqrt(t));
        else        /* geometric */
            P->a *= sqrt (1 - P->es) / t;
        P->es = P->e = P->rf = P->f = P->b = 0;
        break;
    }

    pj_calc_ellps_params (P, P->a, P->es);

    return 0;
}

static int pj_size (PJ *P) {
    char *keys[]  =  {"R", "a", "ellps"};
    paralist *par = 0;
    char *def = 0;
    size_t i, idx, len;

    len = sizeof (keys) / sizeof (char *);
    /* Check which size key is specified */
    for (i = 0;  i < len;  i++) {
        par = pj_get_param (P->params, keys[i]);
        if (par)
            break;
    }
    idx = i;

    /* A size parameter *must* be given */
    if (0==par)
        return proj_errno_set (P, PJD_ERR_MAJOR_AXIS_NOT_GIVEN);

    P->def_size = def = par->param;
    par->used = 1;

    /* in case of ellps, we need another roundtrip through the system */
    /* to expand the ellipsoidal parameters from the built in list */
    if (2==idx)
        return 0;

    P->a = pj_atof (pj_param_value (par));
    if (P->a <= 0)
        return proj_errno_set (P, PJD_ERR_MAJOR_AXIS_NOT_GIVEN);
    if (HUGE_VAL==P->a)
        return proj_errno_set (P, PJD_ERR_MAJOR_AXIS_NOT_GIVEN);
    return 0;
}

static int pj_shape (PJ *P) {
    char *keys[]  = {"rf", "f", "es", "e", "b"};
    paralist *par = 0;
    char *def = 0;
    size_t i, len;

    par = 0;
    len = sizeof (keys) / sizeof (char *);

    /* Check which shape key is specified */
    for (i = 0;  i < len;  i++) {
        par = pj_get_param (P->params, keys[i]);
        if (par)
            break;
    }

    /* Not giving a shape parameter means selecting a sphere */
    if (0==par) {
        P->es = P->f = 0;
        P->b = P->a;
        return 0;
    }

    P->def_shape = def = par->param;
    par->used = 1;

    switch (i) {

    /* reverse flattening, rf */
    case 0:
         P->rf = pj_atof (pj_param_value (par));
         if (HUGE_VAL==P->rf)
             return proj_errno_set (P, PJD_ERR_INVALID_ARG);
         P->f = 1 / P->rf;
         P->es = 2*P->f - P->f*P->f;
         break;

    /* flattening, f */
    case 1:
        P->f = pj_atof (pj_param_value (par));
        if (HUGE_VAL==P->f)
            return proj_errno_set (P, PJD_ERR_INVALID_ARG);
        if (0==P->f)
            return proj_errno_set (P, PJD_ERR_INVALID_ARG);
        P->rf = 1 / P->f;
        P->es = 2*P->f - P->f*P->f;
        break;

    /* eccentricity squared, es */
    case 2:
        P->es = pj_atof (pj_param_value (par));
        if (HUGE_VAL==P->es)
            return proj_errno_set (P, PJD_ERR_INVALID_ARG);
        if (0==P->es)
            return proj_errno_set (P, PJD_ERR_INVALID_ARG);
        if (1==P->es)
            return proj_errno_set (P, PJD_ERR_ECCENTRICITY_IS_ONE);
        break;

    /* eccentricity, e */
    case 3:
        P->e = pj_atof (pj_param_value (par));
        if (HUGE_VAL==P->e)
            return proj_errno_set (P, PJD_ERR_INVALID_ARG);
        if (0==P->e)
            return proj_errno_set (P, PJD_ERR_INVALID_ARG);
        if (1==P->e)
             return proj_errno_set (P, PJD_ERR_ECCENTRICITY_IS_ONE);
        P->es = P->e * P->e;
        break;

    /* semiminor axis, b */
    case 4:
        P->b = pj_atof (pj_param_value (par));
        if (HUGE_VAL==P->b)
            return proj_errno_set (P, PJD_ERR_INVALID_ARG);
        if (0==P->b)
            return proj_errno_set (P, PJD_ERR_INVALID_ARG);
        P->f = (P->a - P->b) / P->a;
        P->es = 2*P->f - P->f*P->f;
        break;
    default:
        return PJD_ERR_INVALID_ARG;

    }
    if (P->es < 0)
        return proj_errno_set (P, PJD_ERR_ES_LESS_THAN_ZERO);

    pj_calc_ellps_params (P, P->a, P->es);
    return 0;
}





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


/***************************************************************************************/
int pj_calc_ellps_params(PJ *P, double a, double es) {
/****************************************************************************************
    Calculate a large number of ancillary ellipsoidal parameters, in addition to
    the two traditional PROJ defining parameters: Semimajor axis, a, and the
    eccentricity squared, es.

    Most of these parameters are fairly cheap to compute in comparison to the overall
    effort involved in initializing a PJ object. They may, however, take a substantial
    part of the time taken in computing an individual point transformation.

    So by providing them up front, we can amortize the (already modest) cost over all
    transformations carried out over the entire lifetime of a PJ object, rather than
    incur that cost for every single transformation.

    Most of the parameter calculations here are based on the "angular eccentricity",
    i.e. the angle, measured from the semiminor axis, of a line going from the north
    pole to one of the foci of the ellipsoid - or in other words: The arc sine of the
    eccentricity.

    The formulae used are mostly taken from:

    Richard H. Rapp: Geometric Geodesy, Part I, (178 pp, 1991).
    Columbus, Ohio:  Dept. of Geodetic Science
    and Surveying, Ohio State University.

****************************************************************************************/

    P->a = a;
    P->es = es;

    /* Compute some ancillary ellipsoidal parameters */
    if (P->e==0)
        P->e = sqrt(P->es);  /* eccentricity */
    P->alpha = asin (P->e);  /* angular eccentricity */

    /* second eccentricity */
    P->e2  = tan (P->alpha);
    P->e2s = P->e2 * P->e2;

    /* third eccentricity */
    P->e3    = (0!=P->alpha)? sin (P->alpha) / sqrt(2 - sin (P->alpha)*sin (P->alpha)): 0;
    P->e3s = P->e3 * P->e3;

    /* flattening */
    if (0==P->f)
        P->f  = 1 - cos (P->alpha);   /* = 1 - sqrt (1 - PIN->es); */
    P->rf = P->f != 0.0 ? 1.0/P->f: HUGE_VAL;

    /* second flattening */
    P->f2  = (cos(P->alpha)!=0)? 1/cos (P->alpha) - 1: 0;
    P->rf2 = P->f2 != 0.0 ? 1/P->f2: HUGE_VAL;

    /* third flattening */
    P->n  = pow (tan (P->alpha/2), 2);
    P->rn = P->n != 0.0 ? 1/P->n: HUGE_VAL;

    /* ...and a few more */
    if (0==P->b)
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



#ifndef KEEP_ORIGINAL_PJ_ELL_SET
int pj_ell_set(projCtx ctx, paralist *pl, double *a, double *es) {
    PJ B;
    int ret;

    memset (&B, 0, sizeof (B));
    B.ctx = ctx;
    B.params = pl;

    ret = pj_ellipsoid (&B);
    if (ret)
        return ret;

    *a = B.a;
    *es = B.es;
    return 0;
}
#else



/**************************************************************************************/
int pj_ell_set (projCtx ctx, paralist *pl, double *a, double *es) {
/***************************************************************************************
    Initialize ellipsoidal parameters: This is the original ellipsoid setup
    function by Gerald Evenden - significantly more compact than pj_ellipsoid and
    its many helper functions, and still quite readable.

    It is, however, also so tight that it is hard to modify and add functionality,
    and equally hard to find the right place to add further commentary for improved
    future maintainability.

    Hence, when the need to store in the PJ object, the parameters actually used to
    define the ellipsoid came up, rather than modifying this little gem of
    "economy of expression", a much more verbose reimplementation, pj_ellipsoid,
    was written.
***************************************************************************************/
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
#endif
