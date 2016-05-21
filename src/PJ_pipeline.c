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
#include <stddef.h>
#include <errno.h>
PROJ_HEAD(pipeline, "Transformation pipeline manager");
/*PROJ_HEAD(pipe_end, "Transformation pipeline sentinel");*/

PROJ_HEAD(pipe_test1, "1st transformation pipeline proof-of-concept test function");
PROJ_HEAD(pipe_test2, "2nd transformation pipeline proof-of-concept test function");


/* Projection specific elements for the PJ object */
#define PIPELINE_STACK_SIZE 100
struct pj_opaque {
    double a;
    int b;
    int reversible;
	int steps;
    int depth;
    XYZ stack[PIPELINE_STACK_SIZE];
};

/* View one type as the other. Definitely a hack - perhaps even a kludge */
#define ASXYZ(lpz) (*(XYZ *)(&lpz))
#define ASLPZ(xyz) (*(LPZ *)(&xyz))
#define ASXY(lp)   (*(XY  *)(&lp ))
#define ASLP(xy)   (*(LP  *)(&xy ))


static XYZ pipeline_3d (LPZ lpz, int direction, PJ *P) {   /* Ellipsoidal, forward */
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

static XYZ pipeline_forward_3d (LPZ lpz, PJ *P) {
    return pipeline_3d (lpz, 0, P);
}

static LPZ pipeline_reverse_3d (XYZ xyz, PJ *P) {
    xyz = pipeline_3d (ASLPZ(xyz), 1, P);
    return ASLPZ(xyz);
}

static XY pipeline_forward (LP lp, PJ *P) {
	LPZ lpz;
	XYZ xyz;
	XY xy;

	lpz.lam = lp.lam;
	lpz.phi = lp.phi;
	lpz.z = 0;
    xyz = pipeline_3d (lpz, 0, P);

	xy.x = xyz.x;
	xy.y = xyz.y;
	return xy;
}

static LP pipeline_reverse (XY xy, PJ *P) {
	LP lp;
	XYZ xyz;

	xyz.x = xy.x;
	xyz.y = xy.y;
	xyz.z = 0;
    xyz = pipeline_3d (ASLPZ(xyz), 1, P);

	lp.lam = xyz.x;
	lp.phi = xyz.y;
	return lp;
}


/* This one is just used as an indicator for end-of-pipe */
static XY e_pipe_end_forward (LP lp, PJ *P) {
	XY xy = {0,0};
	if (P)
        return ASXY(lp);
	return xy;
}

static int is_end_of_pipe (PJ *P) {
	if (P->fwd == e_pipe_end_forward)
	    return 1;
	return 0;
}

static void freeup(PJ *P) {                                    /* Destructor */
    if (P==0)
        return;
    /* Projection specific deallocation goes here */
    pj_dealloc (P->opaque);
    pj_dealloc (P);
    return;
}


static void *pipeline_freeup_new (PJ *P, int errlev) {         /* Destructor */
    if (0==P)
        return 0;

	pj_ctx_set_errno (P->ctx, errlev);

    if (0==P->opaque)
        return pj_dealloc (P);

    pj_dealloc (P->opaque);
    return pj_dealloc(P);
}

static void pipeline_freeup (PJ *P) {
    pipeline_freeup_new (P, 0);
    return;
}




static PJ *pj_create_pipeline (PJ *P, size_t steps) {
	PJ *pipeline;
	size_t i;

    /*  Room for the pipeline: An array of PJ (not of PJ *) */
    pipeline = pj_calloc (steps + 2, sizeof(PJ));

	/* First element is the pipeline manager herself */
	pipeline[0] = *P;

	/* Fill the rest of the pipeline with pipe_ends */
	P->fwd = e_pipe_end_forward;
	for (i = 1;  i < steps + 2;  i++)
    	pipeline[i] = *P;

    /* This is a shallow copy, so we just release P, without calling P->pfree */
    pj_dealloc (P);

	return pipeline;
}

static void pj_add_to_pipeline (PJ *pipeline, PJ *P) {
    while (!is_end_of_pipe(pipeline))
	    pipeline++;
	pipeline[0] = *P;
	pj_dealloc (P);
}


/* When asked for the inverse projection, we just swap functions */
void swap_fwd_and_inv (PJ *P) {
	XY  (*fwd)(LP, struct PJconsts *);
	LP  (*inv)(XY, struct PJconsts *);
    XYZ (*fwd3d)(LPZ, struct PJconsts *);
    LPZ (*inv3d)(XYZ, struct PJconsts *);

	fwd = P->fwd;
	inv = P->inv;
	P->fwd = (XY  (*)(LP, struct PJconsts *)) inv;
	P->inv = (LP  (*)(XY, struct PJconsts *)) fwd;

	fwd3d = P->fwd3d;
	inv3d = P->inv3d;
	P->fwd3d = (XYZ  (*)(LPZ, struct PJconsts *)) inv3d;
	P->inv3d = (LPZ  (*)(XYZ, struct PJconsts *)) fwd3d;
}




PJ *PROJECTION(pipeline) {
    paralist *L = P->params;

    paralist *L_last = 0;      /* Pointer to the last element of the paralist */
    paralist *L_first = 0;     /* Pointer to the "+first" element */
	paralist *L_last_arg = 0;  /* First arg not used in previous definition */
	paralist *L_first_default_arg = 0;
    paralist *L_pipeline = 0;  /* Pointer to the "+proj=pipeline" element */

    paralist *L_sentinel = 0;
    paralist *L_restore  = 0;

    int i,  number_of_steps = 0;

    PJ     *next_step = 0;
    PJ     *pipeline  = 0;

	P->fwd3d = pipeline_forward_3d;
    P->inv3d = pipeline_reverse_3d;
	P->fwd   = pipeline_forward;
    P->inv   = pipeline_reverse;
	P->pfree = pipeline_freeup;

    /* Check syntax while looping over all elements in the linked list of params */
    for (L = L_last = P->params; L != 0; L = L->next) {
        L_last = L;

        if (0==strcmp ("proj=pipeline", L->param)) {
            if (0 != L_first)
                return pipeline_freeup_new (P, 51); /* ERROR: +first before +proj=pipeline */
            if (0 != L_pipeline)
                return pipeline_freeup_new (P, 52); /* ERROR: nested pipelines */
            L_pipeline = L;
        }

        if (0==strcmp ("first", L->param)) {
            if (0 != L_first)
                return pipeline_freeup_new (P, 53); /* ERROR: more than one +first */
            L_first = L;
            number_of_steps++;
        }

        if (0==strcmp ("then", L->param)) {
            if (0 == L_first)
                return pipeline_freeup_new (P, 54); /* ERROR: +then precedes +first */
            number_of_steps++;
        }

        if (0==strcmp ("to", L->param))
            break;
    }

    if (L_first == 0)
    	return pipeline_freeup_new (pipeline, 56);

    /* We add a sentinel at the end of the list to simplify indexing */
    L_sentinel = pj_mkparam ("then"); /* defined in pj_param.c */
    if (0==L_sentinel)
        return pipeline_freeup_new (P, ENOMEM);
    L_restore = L_last->next;
    L_last->next = L_sentinel;

    /*  Room for the pipeline: An array of PJ (note: NOT of PJ *) */
    pipeline = pj_create_pipeline (P, number_of_steps);

    L_first_default_arg = L_pipeline->next;
	puts (L_first_default_arg->param);
    L_last_arg = L_first;
	puts (L_last_arg->param);
	puts (L_last_arg->next->param);

	/* Now loop over all steps, building a new set of arguments for each init */
    for (i = 0;  i < number_of_steps;  i++) {
		int inverse = 0, j;
		#define MAX_ARG 200
	    char  *argv[MAX_ARG];
		int  argc = 0;

		/* Build a set of initialization args for the current step */

        /* First add the step specific args */
		for (L = L_last_arg->next; (argc < MAX_ARG) && (0 != strcmp ("then", L->param)); L = L->next)
			argv[argc++] = 	L->param;

        /* Tell the next step where to start fetching parameters */
		L_last_arg = L;

		/* Then add the default args */
		for (L = L_first_default_arg; (argc < MAX_ARG) && (0 != strcmp ("first", L->param)); L = L->next)
			argv[argc++] = 	L->param;
		puts ("Absolut og default");
		for (j = 0; j < argc; j++)
		    puts (argv[j]);

		next_step = pj_init_ctx(pipeline->ctx, argc, argv);
        if (0 == next_step)
		    return pipeline_freeup_new (pipeline,  pipeline->ctx->last_errno);

		/* Were we asked to provide the inverse case? */
		for (j = 0; j < argc; j++)
    		if (0 == strcmp ("inv", argv[j]))
			    inverse = 1;

        /* Should probably check for existence of the inverse cases here? */
		if (inverse)
		    swap_fwd_and_inv (next_step);
		pj_add_to_pipeline (pipeline, next_step);
    }

	/* Clean up */
    L_last->next = L_restore;
    pj_dealloc (L_sentinel);

    return pipeline;
}


int pj_pipeline_selftest (void) {return 0;}



#if 0
    if ( (is_phits = pj_param(P->ctx, P->params, "tlat_ts").i) ) {
        phits = fabs(pj_param(P->ctx, P->params, "rlat_ts").f);
        if (phits >= HALFPI) E_ERROR(-24);
    }
#endif
