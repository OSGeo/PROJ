/***********************************************************************

                 Transformation pipeline manager

                 Thomas Knudsen, 2016-05-20

************************************************************************

**The Problem**

**The Solution**

**The hack...:**

************************************************************************

Thomas Knudsen, thokn@sdfe.dk, 2016-05-20

***********************************************************************/

#define PJ_LIB__
#include <projects.h>
#include <assert.h>
PROJ_HEAD(pipeline, "Transformation pipeline manager");
PROJ_HEAD(pipe_end, "Transformation pipeline sentinel");

PROJ_HEAD(pipe_test1, "1st transformation pipeline proof-of-concept test function");
PROJ_HEAD(pipe_test2, "2nd transformation pipeline proof-of-concept test function");


/* Projection specific elements for the PJ object */
struct pj_opaque {
    double a;
    int b;
    int reversible;
    int depth;
    XYZ stack[100];
};

#if 0
#define FORWARD3D(name) static XYZ name(LPZ lpz, PJ *P) {XYZ xyz = {0.0, 0.0, 0.0}
#define INVERSE3D(name) static LPZ name(XYZ xyz, PJ *P) {LPZ lpz = {0.0, 0.0, 0.0}
XYZ (*fwd3d)(LPZ, struct PJconsts *);
LPZ (*inv3d)(XYZ, struct PJconsts *);
#endif

/* View one type as the other. Definitely a hack - perhaps even a kludge */
#define ASXYZ(lpz) (*(XYZ *)(&lpz))
#define ASLPZ(xyz) (*(LPZ *)(&xyz))


static XYZ e_pipeline_3d (LPZ lpz, int direction, PJ *P) {   /* Ellipsoidal, forward */
    XYZ xyz = {0.0, 0.0, 0.0};
    xyz.y = lpz.lam + P->es;
    xyz.x = lpz.phi + 42;

    /* Should never happen */
    if (direction && (0==P->opaque->reversible)) {
        pj_ctx_set_errno (P->ctx, -50); /* Pipeline not reversible */
        return xyz;
    }

    if (direction)
        ASLPZ(xyz).lam = 4;
    else
        ASXYZ(ASLPZ(xyz)).y = 42;
    return xyz;
}

static XYZ e_pipeline_forward_3d (LPZ lpz, PJ *P) {
    return e_pipeline_3d (lpz, 0, P);
}

static LPZ e_pipeline_reverse_3d (XYZ xyz, PJ *P) {
    xyz = e_pipeline_3d (ASLPZ(xyz), 1, P);
    return ASLPZ(xyz);
}



static void freeup(PJ *P) {                                    /* Destructor */
    if (P==0)
        return;
    /* Projection specific deallocation goes here */
    pj_dealloc (P->opaque);
    pj_dealloc (P);
    return;
}


static void *pipeline_freeup_new (PJ *P) {              /* Destructor */
    if (0==P)
        return 0;
    if (0==P->opaque)
        return pj_dealloc (P);

    pj_dealloc (P->opaque);
    return pj_dealloc(P);
}

static void pipeline_freeup (PJ *P) {
    pipeline_freeup_new (P);
    return;
}




PJ *PROJECTION(pipeline) {
    paralist *L = P->params;

    paralist *L_last = 0;
    paralist *L_first = 0;
    paralist *L_pipeline = 0;

    paralist *L_sentinel = 0;
    paralist *L_restore = 0;

    int i, number_of_steps = 0;

    PJ       *next_step = 0;
    PJ     *all_steps  = 0;


    P->fwd3d = e_pipeline_forward_3d;
    P->inv3d = e_pipeline_reverse_3d;



    /* This is a loop over all elements in the linked list of params */
    for (L = L_last = P->params; L != 0; L = L->next) {

        L_last = L;
        printf ("%s - %s\n", L->param, L_last->param);

        if (0==strcmp ("proj=pipeline", L->param)) {
            if (0 != L_first)
                return pipeline_freeup_new (P); /* ERROR: +first before +proj=pipeline */
            if (0 != L_pipeline)
                return pipeline_freeup_new (P); /* ERROR: nested pipelines */
            L_pipeline = L;
        }

        if (0==strcmp ("first", L->param)) {
            if (0 != L_first)
                return pipeline_freeup_new (P); /* ERROR: more than one +first */
            L_first = L;
            number_of_steps++;
        }

        if (0==strcmp ("then", L->param)) {
            if (0 == L_first)
                return pipeline_freeup_new (P); /* ERROR: +then precedes +first */
            number_of_steps++;
        }

        if (0==strcmp ("to", L->param))
            break;

    }

    /* We add a sentinel at the end of the list to simplify indexing */
    L_sentinel = pj_mkparam ("then"); /* defined in pj_param.c */
    if (0==L_sentinel)
        return pipeline_freeup_new (P);
    L_restore = L_last->next;
    L_last->next = L_sentinel;

    printf ("first: %s, sentinel: %s\n", L_first->param, L_sentinel->param);

    /*  Room for the pipeline: An array of PJ (not of PJ *) */
    all_steps = pj_calloc (number_of_steps + 2, sizeof(PJ));

    /* This is a shallow copy, so we just release P, without calling P->pfree */
    all_steps[0] = *P;
    pj_dealloc (P);
    P = 0;

	/* TODO: allocate a pipe_end and fill the entire array with copies of it */

    /* Now loop over all steps, building a new set of arguments for each init */
    for (i = 0; i < number_of_steps; i++) {
		paralist *L_start, *L_curr, *L_end;
		int inverse;
		#define MAX_ARG 200
	    char  *argv[MAX_ARG];
		int  argc = 0;

		if (0==i)
		    L_curr = (L_end = L_first->next);
		else
		    L_curr = L_curr->next;

		if (0==L_curr)
		    break;
		printf ("curr: %p, start: %p\n", L_curr, L_start);

        inverse = 0;
		printf ("[ID = %d of %d]: %s %s\n", i, number_of_steps, L_curr->param, L_first->next->param);

		/* Build a set of initialization args for the current step */
		while (1) {
            if (0 == strcmp ("then", L_curr->param)) {
				L_end = L_curr;
			    L_curr = L_pipeline->next;
				if (0 == strcmp ("then", L_curr->param))
				    break;
			}

			/* Through all default args? */
			if (0 == strcmp ("first", L_curr->param))
				break;
            argv[argc++] = 	L_curr->param;
			puts (L_curr->param);
			if (0 == strcmp ("inv", L_curr->param))
			    inverse = 1;
			L_curr = L_curr->next;
		}

        puts ("honsekod");
		next_step = pj_init_ctx(all_steps->ctx, argc, argv);
        if (0 == next_step)
		    return pipeline_freeup_new (all_steps);
		puts ("ulleurt");

		if (inverse) {
			/* TODO swap forward/inverse forward3d/inverse3d */
		}
		all_steps[i + 1] = *next_step;
	    pj_dealloc (next_step);
	    next_step = 0;

    }


    /* Clean up */
    L_last->next = L_restore;
    pj_dealloc (L_sentinel);



    return all_steps;
}



int pj_pipeline_selftest (void) {return 0;}



#if 0
    if ( (is_phits = pj_param(P->ctx, P->params, "tlat_ts").i) ) {
        phits = fabs(pj_param(P->ctx, P->params, "rlat_ts").f);
        if (phits >= HALFPI) E_ERROR(-24);
    }
#endif

#if 0
PJ *pj_projection_specific_setup_minimal (PJ *P) {
    pj_prepare (P, des_minimal, freeup, sizeof (struct pj_opaque));
    if (0==P->opaque) {
        freeup (P);
        return 0;
    }

    P->opaque->a = 42.42;
    P->opaque->b = 42;

    /* Spheroidal? */
    if (0==P->es) {
        P->fwd = s_forward;
        P->inv = s_inverse;
        return P;
    }

    /* Otherwise it's ellipsoidal */
    P->fwd = e_forward;
    P->inv = e_inverse;

    return P;
}

PROJ_HEAD(merc, "Mercator") "\n\tCyl, Sph&Ell\n\tlat_ts=";
#undef PROJECTION
#define PROJECTION(P, name)                                  \
pj_projection_specific_setup_##name (PJ *P);                 \
C_NAMESPACE_VAR const char * const pj_s_##name = des_##name; \
C_NAMESPACE PJ *pj_##name (PJ *P) {                          \
    if (P)                                                   \
        return pj_projection_specific_setup_##name (P);      \
    P = (PJ*) pj_calloc (1, sizeof(PJ));                     \
    if (0==P)                                                \
        return 0;                                            \
    P->pfree = freeup;                                       \
    P->descr = des_##name;                                   \
    return P;                                                \
}                                                            \
PJ *pj_projection_specific_setup_##name (PJ *P)




PJ *PROJECTION(B, merc) {
    double phits=0.0;
    int is_phits;

    if( (is_phits = pj_param(B->ctx, B->params, "tlat_ts").i) ) {
        phits = fabs(pj_param(B->ctx, B->params, "rlat_ts").f);
        if (phits >= HALFPI) E_ERROR(-24);
    }

    if (B->es) { /* ellipsoid */
        if (is_phits)
            P->k0 = pj_msfn(sin(phits), cos(phits), P->es);
        B->inv = e_inverse;
        B->fwd = e_forward;
    }

    else { /* sphere */
        if (is_phits)
            B->k0 = cos(phits);
        B->inv = s_inverse;
        B->fwd = s_forward;
    }

    return B;
}
#endif
