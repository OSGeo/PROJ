#define PJ_LIB__
#include <errno.h>
#include <math.h>
#include <stddef.h>
#include <string.h>

#include "proj.h"
#include "projects.h"

struct pj_opaque {
    struct PJconsts *link;
    double  lamp;
    double  cphip, sphip;
};

PROJ_HEAD(ob_tran, "General Oblique Transformation") "\n\tMisc Sph"
"\n\to_proj= plus parameters for projection"
"\n\to_lat_p= o_lon_p= (new pole) or"
"\n\to_alpha= o_lon_c= o_lat_c= or"
"\n\to_lon_1= o_lat_1= o_lon_2= o_lat_2=";

#define TOL 1e-10


static XY o_forward(LP lp, PJ *P) {             /* spheroid */
    struct pj_opaque *Q = P->opaque;
    double coslam, sinphi, cosphi;

    coslam = cos(lp.lam);
    sinphi = sin(lp.phi);
    cosphi = cos(lp.phi);
    lp.lam = adjlon(aatan2(cosphi * sin(lp.lam), Q->sphip * cosphi * coslam +
        Q->cphip * sinphi) + Q->lamp);
    lp.phi = aasin(P->ctx,Q->sphip * sinphi - Q->cphip * cosphi * coslam);

    return Q->link->fwd(lp, Q->link);
}


static XY t_forward(LP lp, PJ *P) {             /* spheroid */
    struct pj_opaque *Q = P->opaque;
    double cosphi, coslam;

    cosphi = cos(lp.phi);
    coslam = cos(lp.lam);
    lp.lam = adjlon(aatan2(cosphi * sin(lp.lam), sin(lp.phi)) + Q->lamp);
    lp.phi = aasin(P->ctx, - cosphi * coslam);

    return Q->link->fwd(lp, Q->link);
}


static LP o_inverse(XY xy, PJ *P) {             /* spheroid */

    struct pj_opaque *Q = P->opaque;
    double coslam, sinphi, cosphi;

    LP lp = Q->link->inv(xy, Q->link);
    if (lp.lam != HUGE_VAL) {
        coslam = cos(lp.lam -= Q->lamp);
        sinphi = sin(lp.phi);
        cosphi = cos(lp.phi);
        lp.phi = aasin(P->ctx,Q->sphip * sinphi + Q->cphip * cosphi * coslam);
        lp.lam = aatan2(cosphi * sin(lp.lam), Q->sphip * cosphi * coslam -
            Q->cphip * sinphi);
    }
    return lp;
}


static LP t_inverse(XY xy, PJ *P) {             /* spheroid */

    struct pj_opaque *Q = P->opaque;
    double cosphi, t;

    LP lp = Q->link->inv(xy, Q->link);
    if (lp.lam != HUGE_VAL) {
        cosphi = cos(lp.phi);
        t = lp.lam - Q->lamp;
        lp.lam = aatan2(cosphi * sin(t), - sin(lp.phi));
        lp.phi = aasin(P->ctx,cosphi * cos(t));
    }
    return lp;
}


static void *destructor(PJ *P, int errlev) {
    if (0==P)
        return 0;
    if (0==P->opaque)
        return pj_default_destructor (P, errlev);

    if (P->opaque->link)
        P->opaque->link->destructor (P->opaque->link, errlev);

    return pj_default_destructor(P, errlev);
}




/***********************************************************************

These functions are modified versions of the functions "argc_params"
and "argv_params" from PJ_pipeline.c

Basically, they do the somewhat backwards stunt of turning the paralist
representation of the +args back into the original +argv, +argc
representation accepted by pj_init_ctx().

This, however, also begs the question of whether we really need the
paralist linked list representation, or if we could do with a simpler
null-terminated argv style array? This would simplfy some code, and
keep memory allocations more localized.

***********************************************************************/

typedef struct {int argc; char **argv;} ARGS;

/* count the number of args in the linked list <params> */
static size_t paralist_params_argc (paralist *params) {
    size_t argc = 0;
    for (; params != 0; params = params->next)
        argc++;
    return argc;
}


/* turn paralist into argc/argv style argument list */
static ARGS ob_tran_target_params (paralist *params) {
    int i = 0;
    ARGS args = {0, 0};
    size_t argc = paralist_params_argc (params);
    if (argc < 2)
        return args;

    /* all args except the proj_ob_tran */
    args.argv = pj_calloc (argc - 1, sizeof (char *));
    if (0==args.argv)
        return args;

    /* Copy all args *except* the proj=ob_tran arg to the argv array */
    for (i = 0;  params != 0;  params = params->next) {
        if (0==strcmp (params->param, "proj=ob_tran"))
            continue;
        args.argv[i++] = params->param;
    }
    args.argc = i;

    /* Then convert the o_proj=xxx element to proj=xxx */
    for (i = 0;   i < args.argc;   i++) {
        if (0!=strncmp (args.argv[i], "o_proj=", 7))
            continue;
        args.argv[i] += 2;
        break;
    }

    return args;
}



PJ *PROJECTION(ob_tran) {
    double phip;
    char *name;
    ARGS args;
    PJ *R; /* projection to rotate */

    struct pj_opaque *Q = pj_calloc (1, sizeof (struct pj_opaque));
    if (0==Q)
        return destructor(P, ENOMEM);

    P->opaque = Q;
    P->destructor = destructor;

    /* get name of projection to be translated */
    if (!(name = pj_param(P->ctx, P->params, "so_proj").s))
        return destructor(P, PJD_ERR_NO_ROTATION_PROJ);

    /* avoid endless recursion */
    if( strcmp(name, "ob_tran") == 0 )
        return destructor(P, PJD_ERR_FAILED_TO_FIND_PROJ);

    /* Create the target projection object to rotate */
    args = ob_tran_target_params (P->params);
    R = pj_init_ctx (pj_get_ctx(P), args.argc, args.argv);
    pj_dealloc (args.argv);

    if (0==R)
        return destructor (P, PJD_ERR_UNKNOWN_PROJECTION_ID);
    Q->link = R;

    if (pj_param(P->ctx, P->params, "to_alpha").i) {
        double lamc, phic, alpha;

        lamc    = pj_param(P->ctx, P->params, "ro_lon_c").f;
        phic    = pj_param(P->ctx, P->params, "ro_lat_c").f;
        alpha   = pj_param(P->ctx, P->params, "ro_alpha").f;

        if (fabs(fabs(phic) - M_HALFPI) <= TOL)
            return destructor(P, PJD_ERR_LAT_0_OR_ALPHA_EQ_90);

        Q->lamp = lamc + aatan2(-cos(alpha), -sin(alpha) * sin(phic));
        phip = aasin(P->ctx,cos(phic) * sin(alpha));
    } else if (pj_param(P->ctx, P->params, "to_lat_p").i) { /* specified new pole */
        Q->lamp = pj_param(P->ctx, P->params, "ro_lon_p").f;
        phip = pj_param(P->ctx, P->params, "ro_lat_p").f;
    } else { /* specified new "equator" points */
        double lam1, lam2, phi1, phi2, con;

        lam1 = pj_param(P->ctx, P->params, "ro_lon_1").f;
        phi1 = pj_param(P->ctx, P->params, "ro_lat_1").f;
        lam2 = pj_param(P->ctx, P->params, "ro_lon_2").f;
        phi2 = pj_param(P->ctx, P->params, "ro_lat_2").f;
        if (fabs(phi1 - phi2) <= TOL || (con = fabs(phi1)) <= TOL ||
            fabs(con - M_HALFPI) <= TOL || fabs(fabs(phi2) - M_HALFPI) <= TOL)
                return destructor(P, PJD_ERR_LAT_1_OR_2_ZERO_OR_90);

        Q->lamp = atan2(cos(phi1) * sin(phi2) * cos(lam1) -
            sin(phi1) * cos(phi2) * cos(lam2),
            sin(phi1) * cos(phi2) * sin(lam2) -
            cos(phi1) * sin(phi2) * sin(lam1));
        phip = atan(-cos(Q->lamp - lam1) / tan(phi1));
    }

    if (fabs(phip) > TOL) { /* oblique */
        Q->cphip = cos(phip);
        Q->sphip = sin(phip);
        P->fwd = Q->link->fwd ? o_forward : 0;
        P->inv = Q->link->inv ? o_inverse : 0;
    } else { /* transverse */
        P->fwd = Q->link->fwd ? t_forward : 0;
        P->inv = Q->link->inv ? t_inverse : 0;
    }

    /* Support some rather speculative test cases, where the rotated projection */
    /* is actually latlong. We do not want scaling in that case... */
    if (Q->link->right==PJ_IO_UNITS_ANGULAR)
        P->right = PJ_IO_UNITS_PROJECTED;


    return P;
}
