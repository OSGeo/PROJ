/* set ellipsoid parameters a and es */

#include <math.h>
#include <stddef.h>
#include <string.h>

#include "proj.h"
#include "proj_internal.h"
#include "projects.h"


/* Prototypes of the pj_ellipsoid helper functions */
static int ellps_ellps (PJ *P);
static int ellps_size (PJ *P);
static int ellps_shape (PJ *P);
static int ellps_spherification (PJ *P);

static paralist *pj_get_param (paralist *list, char *key);
static char     *pj_param_value (paralist *list);
static const PJ_ELLPS *pj_find_ellps (char *name);


/***************************************************************************************/
int pj_ellipsoid (PJ *P) {
/****************************************************************************************
    This is a replacement for the classic PROJ pj_ell_set function. The main difference
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

    If size and shape are given as ellps=xxx, later shape and size parameters
    are are taken into account as modifiers for the built in ellipsoid definition.

    While this may seem strange, it is in accordance with historical PROJ
    behaviour. It can e.g. be used to define coordinates on the ellipsoid
    scaled to unit semimajor axis by specifying "+ellps=xxx +a=1"

****************************************************************************************/
    int err = proj_errno_reset (P);
    char *empty = {""};

    P->def_size = P->def_shape = P->def_spherification = P->def_ellps = 0;

    /* Specifying R overrules everything */
    if (pj_get_param (P->params, "R")) {
        ellps_size (P);
        pj_calc_ellipsoid_params (P, P->a, 0);
        if (proj_errno (P))
            return 1;
        return proj_errno_restore (P, err);
    }


    /* If an ellps argument is specified, start by using that */
    if (0 != ellps_ellps (P))
        return 1;

    /* We may overwrite the size */
    if (0 != ellps_size (P))
        return 2;

    /* We may also overwrite the shape */
    if (0 != ellps_shape (P))
        return 3;

    /* When we're done with it, we compute all related ellipsoid parameters */
    pj_calc_ellipsoid_params (P, P->a, P->es);

    /* And finally, we may turn it into a sphere */
    if (0 != ellps_spherification (P))
        return 4;

    proj_log_debug (P, "pj_ellipsoid - final: a=%.3f f=1/%7.3f, errno=%d",
                        P->a,  P->f!=0? 1/P->f: 0,  proj_errno (P));
    proj_log_debug (P, "pj_ellipsoid - final: %s %s %s %s",
                        P->def_size?           P->def_size: empty,
                        P->def_shape?          P->def_shape: empty,
                        P->def_spherification? P->def_spherification: empty,
                        P->def_ellps?          P->def_ellps: empty            );

    if (proj_errno (P))
        return 5;

    /* success */
    return proj_errno_restore (P, err);
}


/***************************************************************************************/
static int ellps_ellps (PJ *P) {
/***************************************************************************************/
    PJ B;
    const PJ_ELLPS *ellps;
    paralist *par = 0;
    char *name;
    int err;

    /* Sail home if ellps=xxx is not specified */
    par = pj_get_param (P->params, "ellps");
    if (0==par)
        return 0;

    /* Otherwise produce a fake PJ to make ellps_size/ellps_shape do the hard work for us */

    /* First move B into P's context to get error messages onto the right channel */
    B.ctx = P->ctx;

    /* Then look up the right size and shape parameters from the builtin list */
    if (strlen (par->param) < 7)
        return proj_errno_set (P, PJD_ERR_INVALID_ARG);
    name = par->param + 6;
    ellps = pj_find_ellps (name);
    if (0==ellps)
        return proj_errno_set (P, PJD_ERR_UNKNOWN_ELLP_PARAM);

    /* Now, get things ready for ellps_size/ellps_shape, make them do their thing, and clean up */
    err = proj_errno_reset (P);
    B = *P;
    pj_erase_ellipsoid_def (&B);
    B.params = pj_mkparam (ellps->major);
    B.params->next = pj_mkparam (ellps->ell);

    ellps_size (&B);
    ellps_shape (&B);

    pj_dealloc (B.params->next);
    pj_dealloc (B.params);
    if (proj_errno (&B))
        return proj_errno (&B);

    /* Finally update P and sail home */
    pj_inherit_ellipsoid_def (&B, P);
    P->def_ellps = par->param;
    par->used = 1;

    return proj_errno_restore (P, err);
}


/***************************************************************************************/
static int ellps_size (PJ *P) {
/***************************************************************************************/
    paralist *par = 0;
    int a_was_set = 0;

    /* A size parameter *must* be given, but may have been given as ellps prior */
    if (P->a != 0)
        a_was_set = 1;

    /* Check which size key is specified */
    par = pj_get_param (P->params, "R");
    if (0==par)
        par = pj_get_param (P->params, "a");
    if (0==par)
        return a_was_set? 0: proj_errno_set (P, PJD_ERR_MAJOR_AXIS_NOT_GIVEN);

    P->def_size = par->param;
    par->used = 1;
    P->a = pj_atof (pj_param_value (par));
    if (P->a <= 0)
        return proj_errno_set (P, PJD_ERR_MAJOR_AXIS_NOT_GIVEN);
    if (HUGE_VAL==P->a)
        return proj_errno_set (P, PJD_ERR_MAJOR_AXIS_NOT_GIVEN);

    if ('R'==par->param[0]) {
        P->es = P->f = P->e = P->rf = 0;
        P->b = P->a;
    }
    return 0;
}


/***************************************************************************************/
static int ellps_shape (PJ *P) {
/***************************************************************************************/
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

    /* Not giving a shape parameter means selecting a sphere, unless shape */
    /* has been selected previously via ellps=xxx */
    if (0==par && P->es != 0)
        return 0;
    if (0==par && P->es==0) {
        P->es = P->f = 0;
        P->b = P->a;
        return 0;
    }

    P->def_shape = def = par->param;
    par->used = 1;
    P->es = P->f = P->b = P->e = P->rf = 0;

    switch (i) {

    /* reverse flattening, rf */
    case 0:
         P->rf = pj_atof (pj_param_value (par));
         if (HUGE_VAL==P->rf)
             return proj_errno_set (P, PJD_ERR_INVALID_ARG);
         if (0==P->rf)
             return proj_errno_set (P, PJD_ERR_REV_FLATTENING_IS_ZERO);
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
            return proj_errno_set (P, PJD_ERR_ECCENTRICITY_IS_ONE);
        if (P->b==P->a)
            break;
        P->f = (P->a - P->b) / P->a;
        P->es = 2*P->f - P->f*P->f;
        break;
    default:
        return PJD_ERR_INVALID_ARG;

    }

    if (P->es < 0)
        return proj_errno_set (P, PJD_ERR_ES_LESS_THAN_ZERO);
    return 0;
}


/* series coefficients for calculating ellipsoid-equivalent spheres */
static const double SIXTH = 1/6.;
static const double RA4   = 17/360.;
static const double RA6   = 67/3024.;
static const double RV4   = 5/72.;
static const double RV6   = 55/1296.;

/***************************************************************************************/
static int ellps_spherification (PJ *P) {
/***************************************************************************************/
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
        break;

    /* R_V - a sphere with same volume as ellipsoid */
    case 1:
        P->a *= 1. - P->es * (SIXTH + P->es * (RV4 + P->es * RV6));
        break;

    /* R_a - a sphere with R = the arithmetic mean of the ellipsoid */
    case 2:
        P->a = (P->a + P->b) / 2;
        break;

    /* R_g - a sphere with R = the geometric mean of the ellipsoid */
    case 3:
        P->a = sqrt (P->a * P->b);
        break;

    /* R_h - a sphere with R = the harmonic mean of the ellipsoid */
    case 4:
        if (P->a + P->b == 0)
            return proj_errno_set (P, PJD_ERR_TOLERANCE_CONDITION);
        P->a = (2*P->a * P->b) / (P->a + P->b);
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
        break;
    }

    /* Clean up the ellipsoidal parameters to reflect the sphere */
    P->es = P->e = P->f = 0;
    P->rf = HUGE_VAL;
    P->b = P->a;
    pj_calc_ellipsoid_params (P, P->a, 0);

    return 0;
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


static const PJ_ELLPS *pj_find_ellps (char *name) {
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


/**************************************************************************************/
void pj_erase_ellipsoid_def (PJ *P) {
/***************************************************************************************
    Erase all ellipsoidal parameters in P
***************************************************************************************/
    PJ B;

    /* Make a blank PJ to copy from */
    memset (&B, 0, sizeof (B));

    /* And use it to overwrite all existing ellipsoid defs */
    pj_inherit_ellipsoid_def (&B, P);
}


/**************************************************************************************/
void pj_inherit_ellipsoid_def (const PJ *src, PJ *dst) {
/***************************************************************************************
    Brute force copy the ellipsoidal parameters from src to dst.  This code was
    written before the actual ellipsoid setup parameters were kept available in
    the PJ->def_xxx elements.
***************************************************************************************/

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
int pj_calc_ellipsoid_params (PJ *P, double a, double es) {
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
/**************************************************************************************/
int pj_ell_set (PJ_CONTEXT *ctx, paralist *pl, double *a, double *es) {
/***************************************************************************************
    Initialize ellipsoidal parameters by emulating the original ellipsoid setup
    function by Gerald Evenden, through a call to pj_ellipsoid
***************************************************************************************/
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
            if (!s) {
                pj_ctx_set_errno( ctx, PJD_ERR_UNKNOWN_ELLP_PARAM);
                return 1;
            }
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
                pj_ctx_set_errno(ctx, PJD_ERR_REV_FLATTENING_IS_ZERO);
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
                pj_ctx_set_errno(ctx, PJD_ERR_TOLERANCE_CONDITION);
                goto bomb;
            }
            *a = 2. * *a * b / (*a + b);
            *es = 0.;
        } else if ((i = pj_param(ctx,pl, "tR_lat_a").i) || /* sphere--arith. */
            pj_param(ctx,pl, "tR_lat_g").i) { /* or geom. mean at latitude */
            double tmp;

            tmp = sin(pj_param(ctx,pl, i ? "rR_lat_a" : "rR_lat_g").f);
            if (fabs(tmp) > M_HALFPI) {
                pj_ctx_set_errno(ctx, PJD_ERR_REF_RAD_LARGER_THAN_90);
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
    if (*es < 0.) {
        pj_ctx_set_errno(ctx, PJD_ERR_ES_LESS_THAN_ZERO);
        return 1;
    }
    if (*a <= 0.) {
        pj_ctx_set_errno(ctx, PJD_ERR_MAJOR_AXIS_NOT_GIVEN);
        return 1;
    }
    return 0;
}
#endif
