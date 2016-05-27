/***********************************************************************

               Transformation pipeline manager

                 Thomas Knudsen, 2016-05-20

************************************************************************

**The Problem**

**The Solution**

**The hack...:**

************************************************************************

Thomas Knudsen, thokn@sdfe.dk, 2016-05-20

************************************************************************
* Copyright (c) 2016, Thomas Knudsen / SDFE
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
***********************************************************************/

#define PJ_LIB__
#include <projects.h>
#include <PJ_pipeline.h>

#include <assert.h>
#include <stddef.h>
#include <errno.h>
PROJ_HEAD(pipeline,         "Transformation pipeline manager");
PROJ_HEAD(pipeline_angular, "Transformation pipeline manager, with angular output");


/* Projection specific elements for the PJ object */
#define PIPELINE_STACK_SIZE 100
struct pj_opaque {
    int reversible;
    int steps;
    int depth;
    int verbose;
    int *reverse_step;
    COORDINATE stack[PIPELINE_STACK_SIZE];
};


static COORDINATE pipeline_3d (COORDINATE point, int direction, PJ *P);
static XYZ pipeline_forward_3d (LPZ lpz, PJ *P);
static LPZ pipeline_reverse_3d (XYZ xyz, PJ *P);
static XY pipeline_forward (LP lpz, PJ *P);
static LP pipeline_reverse (XY xyz, PJ *P);


void pj_log_coordinate (projCtx ctx, int level, const char *banner, COORDINATE point, int angular) {
    double s = 1;
    char empty[] = "";
    char angular_fmt[] = {"%20.20s  %12.9f   %12.9f    %12.5f"};
    char linear_fmt[]  = {"%20.20s  %15.5f   %15.5f    %15.5f"};
    char *fmt = linear_fmt;

    if (angular) {
        fmt =  angular_fmt;
        s   =  RAD_TO_DEG;
    }

    if (!banner)
        banner = empty;

    pj_log (ctx, level, fmt, banner, s*point.xyz.x, s*point.xyz.y, point.xyz.z);
}


int pj_show_coordinate (char *banner, COORDINATE point, int angular) {
    int i = 0;
    if (banner)
        printf ("%s", banner);

    if (angular) {
        i += printf("%12.9f ", RAD_TO_DEG * point.xyz.x);
        i += printf("%12.9f ", RAD_TO_DEG * point.xyz.y);
        i += printf("%12.4f",  point.xyz.z);
        if (banner)
            printf("\n");
        return i;
    }

    i += printf("%15.4f ", point.xyz.x);
    i += printf("%15.4f ", point.xyz.y);
    i += printf("%15.4f",  point.xyz.z);
    if (banner)
        printf("\n");
    return i;
}



/* Apply the most appropriate projection function. No-op if none appropriate */
COORDINATE pj_apply_projection (COORDINATE point, int direction, PJ *P) {

    /* Forward */
    if (direction == 0) {
        /* run the pipeline directly: pj_fwd3d rejects on most input values */
        if ((P->fwd3d==pipeline_forward_3d) || (P->fwd==pipeline_forward))
            return pipeline_3d(point, 0, P);

        /* Here come the real cases. We run these through the pj_fwd/inv wrappers, */
        /* in order to get scaling correctly applied */
        if (P->fwd3d)
                point.xyz = pj_fwd3d (point.lpz, P);
        else if (P->fwd)
            point.xy = pj_fwd (point.lp, P);
        return point;
    }

    /* Inverse */
    if ((P->inv3d==pipeline_reverse_3d) || (P->inv==pipeline_reverse))
        return pipeline_3d(point, 1, P);

    if (P->inv3d)
        point.lpz = pj_inv3d (point.xyz, P);
    else if (P->inv)
        point.lp = pj_inv (point.xy, P);
    return point;
}



/* The actual pipeline driver */
static COORDINATE pipeline_3d (COORDINATE point, int direction, PJ *P) {
    int i, j, first_step, last_step, incr;

    /* semi-open interval: first_step is included, last_step excluded */
    if (direction == 0) {        /* Forward */
        first_step = 1;
        last_step  = P->opaque->steps + 1;
        incr  = 1;
    } else {                     /* Inverse - run pipeline backwards */
        first_step = P->opaque->steps;
        last_step  =  0;
        incr  = -1;
    }


    for (i = first_step; i != last_step; i += incr) {
        int d = direction;
        if (P->opaque->reverse_step[i])
            d = !d;
        point = pj_apply_projection (point, d, P + i);
        pj_log_coordinate (P->ctx, 5, (P + i)->descr, point, 0);
        if (P->opaque->depth < PIPELINE_STACK_SIZE)
            P->opaque->stack[P->opaque->depth++] = point;
        if (P->opaque->verbose) for (j = 0;  j < P->opaque->depth; j++)
            pj_log_coordinate (P->ctx, 7, "Stack: ", P->opaque->stack[j], 0);
    }

    P->opaque->depth = 0;    /* Clear the stack */
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

int pj_is_pipeline (PJ *P) {
    if (0==P)
        return 0;
    if (P->fwd3d==pipeline_forward_3d)
        return 1;
    if (P->inv3d==pipeline_reverse_3d)
        return 1;
    if (P->fwd==pipeline_forward)
        return 1;
    if (P->inv==pipeline_reverse)
        return 1;
    return 0;
}


int pj_pipeline_verbose (PJ *P) {
    if (!pj_is_pipeline (P))
        return 0;
    P->opaque->verbose = !P->opaque->verbose;
    return P->opaque->verbose;
}

void pj_log_pipeline_steps (PJ *P, int level) {
    int step;
    if (0==P)
        return;
    if (0==pj_is_pipeline(P))
        return;
    pj_log (P->ctx, level, "PIPELINE STEPS");
    for (step = 0;  step < pj_pipeline_steps(P)+1;  step++) {
        char forward[] = {"(forward)"};
        char inverse[] = {"(inverse)"};
        char *direction = forward;
        if (P->opaque->reverse_step[step]==1)
            direction = inverse;
        pj_log (P->ctx, level, "    step %d: %.15s %s", step, P[step].descr, direction);
    }
}

int pj_pipeline_angular_output (PJ *P, int direction) {
    if (!pj_is_pipeline (P))
        return 0;
    if (direction==0)  /* forward */
        return P->opaque->reverse_step[P->opaque->steps];
    return !P->opaque->reverse_step[1];
}

int pj_pipeline_angular_input (PJ *P, int direction) {
    if (!pj_is_pipeline (P))
        return 1;
    if (direction==0)  /* forward */
        return !P->opaque->reverse_step[1];
    return P->opaque->reverse_step[P->opaque->steps];
}

int pj_pipeline_steps (PJ *P) {
    if (!pj_is_pipeline (P))
        return 0;
        return P->opaque->steps;
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

    pj_dealloc (P->opaque->reverse_step);
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
    P->opaque->reverse_step =  pj_calloc (steps + 2, sizeof(int));
    if (0==P->opaque->reverse_step)
        return pipeline_freeup (P, ENOMEM);

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

static void pj_push_to_pipeline (PJ *pipeline, PJ *P) {
    while (!is_end_of_pipe(pipeline))
        pipeline++;
    pipeline[0] = *P;
    pj_dealloc (P);
}


#if 0
/* When asked for the inverse projection, we USED TO just swap the functions (now using the reverse_step table) */
static void swap_fwd_and_inv (PJ *P) {
    XY  (*fwd)(LP, PJ *);
    LP  (*inv)(XY, PJ *);
    XYZ (*fwd3d)(LPZ, PJ *);
    LPZ (*inv3d)(XYZ, PJ *);

    /* Leads to some funky function casts, but they are stadards compliant */
    fwd = P->fwd;
    inv = P->inv;
    P->fwd = (XY  (*)(LP, PJ *)) inv;
    P->inv = (LP  (*)(XY, PJ *)) fwd;

    fwd3d = P->fwd3d;
    inv3d = P->inv3d;
    P->fwd3d = (XYZ  (*)(LPZ, PJ *)) inv3d;
    P->inv3d = (LPZ  (*)(XYZ, PJ *)) fwd3d;
}
#endif



PJ *PROJECTION(pipeline) {
    paralist *L = P->params;

    paralist *L_last = 0;       /* Pointer to the last element of the paralist */
    paralist *L_first_step = 0; /* Pointer to the first "step" element */
    paralist *L_last_arg = 0;   /* First arg not used in previous step definition */
    paralist *L_first_default_arg = 0;
    paralist *L_pipeline = 0;   /* Pointer to the "+proj=pipeline" element */

    paralist *L_sentinel = 0;
    paralist *L_restore  = 0;

    int i,  number_of_steps = 0, inverse = 0;

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
            L->used = 1;
            if (0 != L_first_step)
                return pipeline_freeup (P, 51); /* ERROR: +first before +proj=pipeline */
            if (0 != L_pipeline)
                return pipeline_freeup (P, 52); /* ERROR: nested pipelines */
            L_pipeline = L;
        }

        if (0==strcmp ("step", L->param)) {
            L->used = 1;
            number_of_steps++;
            if (0 == L_first_step)
                L_first_step = L;
        }

        if (0==strcmp ("to", L->param))
            break;
    }

    if (L_first_step == 0)
        return pipeline_freeup (pipeline, 56);

    /* We add a sentinel at the end of the list to simplify indexing */
    L_sentinel = pj_mkparam ("step"); /* defined in pj_param.c */
    if (0==L_sentinel)
        return pipeline_freeup (P, ENOMEM);
    L_restore = L_last->next;
    L_last->next = L_sentinel;

    /*  Room for the pipeline: An array of PJ (note: NOT of PJ *) */
    pipeline = pj_create_pipeline (P, number_of_steps);

    L_first_default_arg = L_pipeline->next;
    L_last_arg = L_first_step;

    /* Now loop over all steps, building a new set of arguments for each init */
    for (i = 0;  i < number_of_steps;  i++) {
        int j;
        #define MAX_ARG 200
        char  *argv[MAX_ARG];
        int  argc = 0;

        /* Build a set of initialization args for the current step */

        /* First add the step specific args */
        for (L = L_last_arg->next; (argc < MAX_ARG) && (0 != strcmp ("step", L->param)); L = L->next) {
            argv[argc++] =     L->param;
            L->used = 1;
        }

        /* Tell the next step where to start fetching parameters */
        L_last_arg = L;

        /* Then add the default args */
        for (L = L_first_default_arg; (argc < MAX_ARG) && (0 != strcmp ("step", L->param)); L = L->next) {
            argv[argc++] = L->param;
            L->used = 1;
        }

        next_step = pj_init_ctx(pipeline->ctx, argc, argv);
        if (0 == next_step)
            return pipeline_freeup (pipeline,  pipeline->ctx->last_errno);

        /* Were we asked to provide the inverse case? */
        for (j = inverse = 0; j < argc; j++)
            if (0 == strcmp ("inv", argv[j]))
                break;
        if (j < argc)
            inverse = 1;

        /* Should probably check for existence of the inverse cases here? */
        pipeline->opaque->reverse_step[i+1] = inverse;

        pj_push_to_pipeline (pipeline, next_step);

        do {
            char fwd[] = {"(forward)"};
            char inv[] = {"(inverse)"};
            char *direction = fwd;
            if (inverse)
                direction = inv;
            pj_log (pipeline->ctx, 5, "pipeline step %d (%d args) %s:", i + 1, argc, direction);
        } while (0);

        for (j = 0; j < argc; j++)
            pj_log (pipeline->ctx, 5, "    argv[%d]: %s", j, argv[j]);
    }

    /* If last step is an inverse projection, we assume (for now) that output is angular */
    if (inverse)
        pipeline[0].descr = des_pipeline_angular;

    /* Clean up */
    L_last->next = L_restore;
    pj_dealloc (L_sentinel);
    return pipeline;
}

/* selftest stub */
int pj_pipeline_selftest (void) {return 0;}
