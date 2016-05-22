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

PROJ_HEAD(pipe_test1, "1st transformation pipeline proof-of-concept test function");
PROJ_HEAD(pipe_test2, "2nd transformation pipeline proof-of-concept test function");


/* View one type as the other. Definitely a hack - perhaps even a kludge */
/* Using a union would probably be more appropriate */
#define ASXYZ(lpz) (*(XYZ *)(&lpz))
#define ASLPZ(xyz) (*(LPZ *)(&xyz))
#define ASXY(lp)   (*(XY  *)(&lp ))
#define ASLP(xy)   (*(LP  *)(&xy ))

/* Avoid explicit type-punning: Use a union */
typedef union {
    XYZ xyz;
	LPZ lpz;
	XY  xy;
	LP  lp;
} COORDINATE;


/* Projection specific elements for the PJ object */
#define PIPELINE_STACK_SIZE 100
struct pj_opaque {
    int reversible;
	int steps;
    int depth;
    COORDINATE stack[PIPELINE_STACK_SIZE];
};

int show_coordinate (char *banner, COORDINATE point, int angular) {
	int i = 0;
	printf ("%s", banner);

    if (angular) {
	    i += printf("%12.9f", RAD_TO_DEG * point.xyz.x);
		i += printf("%12.9f", RAD_TO_DEG * point.xyz.y);
		i += printf("%12.4f\n", point.xyz.z);
		return i;
	}

	i += printf("%15.4f", point.xyz.x);
	i += printf("%15.4f", point.xyz.y);
	i += printf("%15.4f\n", point.xyz.z);
	return i;
}

/* Apply the most appropriate projection function. No-op if none appropriate */
static COORDINATE pj_apply_projection (COORDINATE point, int direction, PJ *P) {
	puts ("agurg");
    /* Forward */
	if (direction == 0) {
		puts ("forward");
		if (P->fwd3d)
			point.xyz = P->fwd3d (point.lpz, P);
		else if (P->fwd)
			point.xy = P->fwd (point.lp, P);
        return point;
	}

    /* Inverse */
	puts ("inverse");
	if (P->inv3d)
		point.lpz = P->inv3d (point.xyz, P);
	else if (P->inv)
		point.lp = P->inv (point.xy, P);
	return point;
}

/* The actual pipeline driver */
static COORDINATE pipeline_3d (COORDINATE point, int direction, PJ *P) {
    int i, first, last, incr;
	puts (P->descr);

	if (direction == 0) {        /* Forward */
		puts ("forward");
		first = 1;
		last  = P->opaque->steps + 1;
		incr  = 1;
	} else {                     /* Inverse */
		puts ("inverse");
		first = P->opaque->steps;
        last  =  0;
		incr  = -1;
	}

    printf ("interval: %d %d %d\n", first, last, incr);

	for (i = first; i != last; i += incr) {
		puts (P[0].descr);
		puts (P[i].descr);

		point = pj_apply_projection (point, direction, P + i);
		show_coordinate ("p3d: ", point, 1);
		if (P->opaque->depth < PIPELINE_STACK_SIZE)
            P->opaque->stack[P->opaque->depth++] = point;
	}

	return point;

#if 0
    /* Should never happen */
    if (direction && (0==P->opaque->reversible)) {
        pj_ctx_set_errno (P->ctx, -50); /* Pipeline not reversible */
        return point;
    }

    if (direction)
        ASLPZ(xyz).lam = 4;
    else
        ASXYZ(ASLPZ(xyz)).y = 42;
#endif
}

static XYZ pipeline_forward_3d (LPZ lpz, PJ *P) {
	COORDINATE point;
	point.lpz = lpz;
    point = pipeline_3d (point, 0, P);
	return point.xyz;
}

static LPZ pipeline_reverse_3d (XYZ xyz, PJ *P) {
	COORDINATE point;
	point.xyz = xyz;
    point = pipeline_3d (point, 1, P);
    return point.lpz;
}

static XY pipeline_forward (LP lp, PJ *P) {
	COORDINATE point;
	point.lp = lp;
	point.lpz.z = 0;

    point = pipeline_3d (point, 0, P);
	return point.xy;
}

static LP pipeline_reverse (XY xy, PJ *P) {
	COORDINATE point;
	point.xy = xy;
	point.xyz.z = 0;

    point = pipeline_3d (point, 1, P);
	return point.lp;
}


/* Indicator with dummy functionality to silence compiler warnings */
static XY pipe_end_indicator (LP lp, PJ *P) {
	XY xy;

    xy.x = lp.lam;
	xy.y = lp.phi;
	if (P)
        return xy;
	xy.x++;
	return xy;
}

static int is_end_of_pipe (PJ *P) {
	if (P->fwd == pipe_end_indicator)
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


static void *pipeline_freeup (PJ *P, int errlev) {         /* Destructor */
    if (0==P)
        return 0;

	pj_ctx_set_errno (P->ctx, errlev);

    if (0==P->opaque)
        return pj_dealloc (P);

    pj_dealloc (P->opaque);
    return pj_dealloc(P);
}


/* Adapts pipeline_freeup to the format defined for the PJ object */
static void pipeline_freeup_wrapper (PJ *P) {
    pipeline_freeup (P, 0);
    return;
}




static PJ *pj_create_pipeline (PJ *P, size_t steps) {
	PJ *pipeline;
	size_t i;

    /*  Room for the pipeline: An array of PJ (not of PJ *) */
    pipeline = pj_calloc (steps + 2, sizeof(PJ));

	P->opaque->steps = steps;

	/* First element is the pipeline manager herself */
	pipeline[0] = *P;

	/* Fill the rest of the pipeline with pipe_ends */
	P->fwd = pipe_end_indicator;
	for (i = 1;  i < steps + 2;  i++)
    	pipeline[i] = *P;

    /* This is a shallow copy, so we just release P, without calling P->pfree */
	/* The actual deallocation will be done by pipeline_freeup */
    pj_dealloc (P);

	return pipeline;
}

static void pj_add_to_pipeline (PJ *pipeline, PJ *P) {
    while (!is_end_of_pipe(pipeline))
	    pipeline++;
	pipeline[0] = *P;
	pj_dealloc (P);
}


/* When asked for the inverse projection, we just swap the functions */
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
	P->pfree = pipeline_freeup_wrapper;

	P->opaque = pj_calloc (1, sizeof(struct pj_opaque));
    if (0==P->opaque)
	    return 0;

    /* Check syntax while looping over all elements in the linked list of params */
    for (L = L_last = P->params; L != 0; L = L->next) {
        L_last = L;

        if (0==strcmp ("proj=pipeline", L->param)) {
            if (0 != L_first)
                return pipeline_freeup (P, 51); /* ERROR: +first before +proj=pipeline */
            if (0 != L_pipeline)
                return pipeline_freeup (P, 52); /* ERROR: nested pipelines */
            L_pipeline = L;
        }

        if (0==strcmp ("first", L->param)) {
            if (0 != L_first)
                return pipeline_freeup (P, 53); /* ERROR: more than one +first */
            L_first = L;
            number_of_steps++;
        }

        if (0==strcmp ("then", L->param)) {
            if (0 == L_first)
                return pipeline_freeup (P, 54); /* ERROR: +then precedes +first */
            number_of_steps++;
        }

        if (0==strcmp ("to", L->param))
            break;
    }

    if (L_first == 0)
    	return pipeline_freeup (pipeline, 56);

    /* We add a sentinel at the end of the list to simplify indexing */
    L_sentinel = pj_mkparam ("then"); /* defined in pj_param.c */
    if (0==L_sentinel)
        return pipeline_freeup (P, ENOMEM);
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
		    return pipeline_freeup (pipeline,  pipeline->ctx->last_errno);

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

/* selftest stub */
int pj_pipeline_selftest (void) {return 0;}
