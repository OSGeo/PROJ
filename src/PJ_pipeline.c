/*******************************************************************************

                       Transformation pipeline manager

                    Thomas Knudsen, 2016-05-20/2016-11-20

********************************************************************************

    Geodetic transformations are typically organized in a number of
    steps. For example, a datum shift could be carried out through
    these steps:

    1. Convert (latitude, longitude, ellipsoidal height) to
       3D geocentric cartesian coordinates (X, Y, Z)
    2. Transform the (X, Y, Z) coordinates to the new datum, using a
       7 parameter Helmert transformation.
    3. Convert (X, Y, Z) back to (latitude, longitude, ellipsoidal height)

    If the height system used is orthometric, rather than ellipsoidal,
    another step is needed at each end of the process:

    1. Add the local geoid undulation (N) to the orthometric height
       to obtain the ellipsoidal (i.e. geometric) height.
    2. Convert (latitude, longitude, ellipsoidal height) to
       3D geocentric cartesian coordinates (X, Y, Z)
    3. Transform the (X, Y, Z) coordinates to the new datum, using a
       7 parameter Helmert transformation.
    4. Convert (X, Y, Z) back to (latitude, longitude, ellipsoidal height)
    5. Subtract the local geoid undulation (N) from the ellipsoidal height
       to obtain the orthometric height.

    Additional steps can be added for e.g. change of vertical datum, so the
    list can grow fairly long. None of the steps are, however, particularly
    complex, and data flow is strictly from top to bottom.

    Hence, in principle, the first example above could be implemented using
    Unix pipelines:

    cat my_coordinates | geographic_to_xyz | helmert | xyz_to_geographic > my_transformed_coordinates

    in the grand tradition of Software Tools [1].

    The proj pipeline driver implements a similar concept: Stringing together
    a number of steps, feeding the output of one step to the input of the next.

    It is a very powerful concept, that increases the range of relevance of the
    proj.4 system substantially. It is, however, not a particularly intrusive
    addition to the code base: The implementation is by and large completed by
    adding an extra projection called "pipeline" (i.e. this file), which
    handles all business.

    Syntactically, the pipeline system introduces the "+step" keyword (which
    indicates the start of each transformation step), the "+omit_fwd" and
    "+omit_inv" keywords (which indicate that a given transformation step
    should be omitted when the pipeline is executed in forward, resp. inverse
    direction), and reintroduces the +inv keyword (indicating that a given
    transformation step should run in reverse, i.e. forward, when the pipeline
    is executed in inverse direction, and vice versa).

    Hence, the first transformation example above, can be implemented as:

    +proj=pipeline +step proj=cart +step proj=helmert <ARGS> +step proj=cart +inv

    Where <ARGS> indicate the Helmert arguments: 3 translations (+x=..., +y=...,
    +z=...), 3 rotations (+rx=..., +ry=..., +rz=...) and a scale factor (+s=...).
    Following geodetic conventions, the rotations are given in Milliarcseconds,
    and the scale factor is given as parts-per-million.

    [1] B. W. Kernighan & P. J. Plauger: Software tools. Addison-Wesley, 1976, 338 pp.

********************************************************************************

Thomas Knudsen, thokn@sdfe.dk, 2016-05-20

********************************************************************************
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
********************************************************************************/

#define PJ_LIB__
#include <proj.h>
#include <projects.h>

#include <assert.h>
#include <stddef.h>
#include <errno.h>
PROJ_HEAD(pipeline,         "Transformation pipeline manager");

/* Projection specific elements for the PJ object */
#define PIPELINE_STACK_SIZE 100
struct pj_opaque {
    int reversible;
    int steps;
    int depth;
    int verbose;
    int *reverse_step;
    int *omit_forward;
    int *omit_inverse;
    PJ_OBS stack[PIPELINE_STACK_SIZE];
    PJ **pipeline;
};


static PJ_OBS pipeline_forward_obs (PJ_OBS, PJ *P);
static PJ_OBS pipeline_reverse_obs (PJ_OBS, PJ *P);
static XYZ    pipeline_forward_3d (LPZ lpz, PJ *P);
static LPZ    pipeline_reverse_3d (XYZ xyz, PJ *P);
static XY     pipeline_forward (LP lpz, PJ *P);
static LP     pipeline_reverse (XY xyz, PJ *P);



/********************************************************************

                  ISOMORPHIC TRANSFORMATIONS

*********************************************************************

    In this context, an isomorphic transformation is a proj PJ
    object returning the same kind of coordinates that it
    receives, i.e. a transformation from angular to angular or
    linear to linear coordinates.

    The degrees-to-radians operation is an example of the former,
    while the latter is typical for most of the datum shift
    operations used in geodesy, e.g. the Helmert 7-parameter shift.

    Isomorphic transformations trips up the pj_inv/pj_fwd
    functions, which try to check input sanity and scale output to
    internal proj units, under the assumption that if input is of
    angular type, then output must be of linear (or vice versa).

    Hence, to avoid having pj_inv/pj_fwd stomping on output (or
    choking on input), we need a way to tell them that they should
    accept whatever is being handed to them.

    The P->left and P->right elements indicate isomorphism.

    For classic proj style projections, they both have the
    value PJ_IO_UNITS_CLASSIC (default initialization), indicating
    that the forward driver expects angular input coordinates, and
    provides linear output coordinates.

    Newer projections may set P->left and P->right to either
    PJ_IO_UNITS_METERS, PJ_IO_UNITS_RADIANS or PJ_IO_UNITS_ANY,
    to indicate their I/O style.

    For the forward driver, left indicates input coordinate
    type, while right indicates output coordinate type.

    For the inverse driver, left indicates output coordinate
    type, while right indicates input coordinate type.

*********************************************************************/




static PJ_OBS pipeline_forward_obs (PJ_OBS point, PJ *P) {
    int i, first_step, last_step, incr;

    first_step = 1;
    last_step  = P->opaque->steps + 1;
    incr  = 1;

    for (i = first_step; i != last_step; i += incr) {
        if (P->opaque->omit_forward[i])
            continue;
        if (P->opaque->reverse_step[i])
            point = pj_trans (P->opaque->pipeline[i], PJ_INV, point);
        else
            point = pj_trans (P->opaque->pipeline[i], PJ_FWD, point);
        if (P->opaque->depth < PIPELINE_STACK_SIZE)
            P->opaque->stack[P->opaque->depth++] = point;
    }

    P->opaque->depth = 0;    /* Clear the stack */
    return point;
}

static PJ_OBS pipeline_reverse_obs (PJ_OBS point, PJ *P) {
    int i, first_step, last_step, incr;

    first_step = P->opaque->steps;
    last_step  =  0;
    incr  = -1;

    for (i = first_step; i != last_step; i += incr) {
        if (P->opaque->omit_inverse[i])
            continue;
        if (P->opaque->reverse_step[i])
            point = pj_trans (P->opaque->pipeline[i], PJ_FWD, point);
        else
            point = pj_trans (P->opaque->pipeline[i], PJ_INV, point);
        if (P->opaque->depth < PIPELINE_STACK_SIZE)
            P->opaque->stack[P->opaque->depth++] = point;
    }

    P->opaque->depth = 0;    /* Clear the stack */
    return point;
}


/* Delegate the work to pipeline_forward_obs() */
static XYZ pipeline_forward_3d (LPZ lpz, PJ *P) {
    PJ_OBS point = pj_obs_null;
    point.coo.lpz = lpz;
    point = pipeline_forward_obs (point, P);
    return point.coo.xyz;
}

/* Delegate the work to pipeline_reverse_obs() */
static LPZ pipeline_reverse_3d (XYZ xyz, PJ *P) {
    PJ_OBS point = pj_obs_null;
    point.coo.xyz = xyz;
    point = pipeline_reverse_obs (point, P);
    return point.coo.lpz;
}

static XY pipeline_forward (LP lp, PJ *P) {
    PJ_OBS point = pj_obs_null;
    point.coo.lp = lp;
    point = pipeline_forward_obs (point, P);
    return point.coo.xy;
}

static LP pipeline_reverse (XY xy, PJ *P) {
    PJ_OBS point = pj_obs_null;
    point.coo.xy = xy;
    point = pipeline_reverse_obs (point, P);
    return point.coo.lp;
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
    int i;
    if (0==P)
        return 0;

    pj_error_set (P, errlev);

    if (0==P->opaque)
        return pj_dealloc (P);

    for (i = 0;  i < P->opaque->steps; i++)
        pj_free (P->opaque->pipeline[i+1]);

    pj_dealloc (P->opaque->reverse_step);
    pj_dealloc (P->opaque->omit_forward);
    pj_dealloc (P->opaque->omit_inverse);
    pj_dealloc (P->opaque->pipeline);

    pj_dealloc (P->opaque);
    return pj_dealloc(P);
}


/* Adapts pipeline_freeup to the format defined for the PJ object */
static void pipeline_freeup_wrapper (PJ *P) {
    pipeline_freeup (P, 0);
    return;
}


static PJ *pj_create_pipeline (PJ *P, size_t steps) {

    /* Room for the pipeline: An array of PJ * with room for sentinels at both ends */
    P->opaque->pipeline = pj_calloc (steps + 2, sizeof(PJ *));
    if (0==P->opaque->pipeline)
        return 0;

    P->opaque->steps = steps;

    P->opaque->reverse_step =  pj_calloc (steps + 2, sizeof(int));
    if (0==P->opaque->reverse_step)
        return 0;

    P->opaque->omit_forward =  pj_calloc (steps + 2, sizeof(int));
    if (0==P->opaque->omit_forward)
        return 0;

    P->opaque->omit_inverse =  pj_calloc (steps + 2, sizeof(int));
    if (0==P->opaque->omit_inverse)
        return 0;

    return P;
}

/* count the number of args in pipeline definition */
size_t argc_params (paralist *params) {
    size_t argc = 0;
    for (; params != 0; params = params->next)
        argc++;
    return ++argc;  /* one extra for the sentinel */
}

/* Sentinel for argument list */
static char argv_sentinel[5] = "step";

/* turn paralist int argc/argv style argument list */
char **argv_params (paralist *params) {
    char **argv;
    size_t argc = 0;
    argv = pj_calloc (argc_params (params), sizeof (char *));
    if (0==argv)
        return 0;
    for (; params != 0; params = params->next)
        argv[argc++] = params->param;
    argv[argc++] = argv_sentinel;
    return argv;
}


PJ *PROJECTION(pipeline) {
    int i, nsteps = 0, argc;
    int i_pipeline = -1, i_first_step = -1, i_current_step;
    char **argv, **current_argv;

    P->fwdobs =  pipeline_forward_obs;
    P->invobs =  pipeline_reverse_obs;
    P->fwd3d  =  pipeline_forward_3d;
    P->inv3d  =  pipeline_reverse_3d;
    P->fwd    =  pipeline_forward;
    P->inv    =  pipeline_reverse;
    P->pfree  =  pipeline_freeup_wrapper;

    P->opaque = pj_calloc (1, sizeof(struct pj_opaque));
    if (0==P->opaque)
        return 0;

    argc = argc_params (P->params);
    argv = argv_params (P->params);
    if (0==argv)
        return pipeline_freeup (P, ENOMEM);

    /* The elements of current_argv are not used - we just use argv_params */
    /* as allocator for a "large enough" container needed later            */
    current_argv = argv_params (P->params);
    if (0==current_argv)
        return pipeline_freeup (P, ENOMEM);

    /* Do some syntactic sanity checking */
    for (i = 0;  i < argc;  i++) {
        if (0==strcmp ("step", argv[i])) {
            if (-1==i_pipeline) {
                pj_log_error (P, "Pipeline: +step before +proj=pipeline");
                return pipeline_freeup (P, -50);
            }
            if (0==nsteps)
                i_first_step = i;
            nsteps++;
            continue;
        }

        if (0==strcmp ("proj=pipeline", argv[i])) {
            if (-1 != i_pipeline) {
                pj_log_error (P, "Pipeline: Nesting invalid");
                return pipeline_freeup (P, -50); /* ERROR: nested pipelines */
            }
            i_pipeline = i;
        }
    }
    nsteps--; /* Last instance of +step is just a sentinel */
    P->opaque->steps = nsteps;

    if (-1==i_pipeline)
        return pipeline_freeup (P, -50); /* ERROR: no pipeline def */

    if (0==nsteps)
        return pipeline_freeup (P, -50); /* ERROR: no pipeline def */

    /* Make room for the pipeline and execution indicators */
    if (0==pj_create_pipeline (P, nsteps))
        return pipeline_freeup (P, ENOMEM);

    /* Now loop over all steps, building a new set of arguments for each init */
    for (i_current_step = i_first_step, i = 0;  i < nsteps;  i++) {
        int j;
        int  current_argc = 0;
        PJ     *next_step = 0;

        /* Build a set of setup args for the current step */
        pj_log_trace (P, "Pipeline: Building arg list for step no. %d", i);

        /* First add the step specific args */
        for (j = i_current_step + 1;  0 != strcmp ("step", argv[j]); j++)
            current_argv[current_argc++] = argv[j];

        i_current_step = j;

        /* Then add the global args */
        for (j = i_pipeline + 1;  0 != strcmp ("step", argv[j]); j++)
            current_argv[current_argc++] = argv[j];

        /* Finally handle non-symmetric steps and inverted steps */
        for (j = 0;  j < current_argc; j++) {
            if (0==strcmp("omit_inv", current_argv[j])) {
                P->opaque->omit_inverse[i+1] = 1;
                P->opaque->omit_forward[i+1] = 0;
            }
            if (0==strcmp("omit_fwd", current_argv[j])) {
                P->opaque->omit_inverse[i+1] = 0;
                P->opaque->omit_forward[i+1] = 1;
            }
            if (0==strcmp("inv", current_argv[j]))
                P->opaque->reverse_step[i+1] = 1;
        }

        pj_log_trace (P, "Pipeline: init - %s, %d", current_argv[0], current_argc);
        for (j = 1;  j < current_argc; j++)
            pj_log_trace (P, "    %s", current_argv[j]);

        next_step = pj_init (current_argc, current_argv);
        pj_log_trace (P, "Pipeline: Step %d at %p", i, next_step);
        if (0==next_step) {
            pj_log_error (P, "Pipeline: Bad step definition: %s", current_argv[0]);
            return pipeline_freeup (P, -50); /* ERROR: bad pipeline def */
        }
        P->opaque->pipeline[i+1] = next_step;
        pj_log_trace (P, "Pipeline:    step done");
    }

    pj_log_trace (P, "Pipeline: %d steps built. Determining i/o characteristics", nsteps);

    /* Determine forward input (= reverse output) data type */

    /* First locate the first forward-active pipeline step */
    for (i = 0;  i < nsteps;  i++)
        if (0==P->opaque->omit_forward[i+1])
            break;
    if (i==nsteps) {
        pj_log_error (P, "Pipeline: No forward steps");
        return pipeline_freeup (P, -50);
    }

    if (P->opaque->reverse_step[i + 1])
        P->left = P->opaque->pipeline[i + 1]->right;
    else
        P->left = P->opaque->pipeline[i + 1]->left;

    if (P->left==PJ_IO_UNITS_CLASSIC) {
        if (P->opaque->reverse_step[i + 1])
            P->left = PJ_IO_UNITS_METERS;
        else
            P->left = PJ_IO_UNITS_RADIANS;
    }

    /* Now, correspondingly determine forward output (= reverse input) data type */

    /* First locate the last reverse-active pipeline step */
    for (i = nsteps - 1;  i >= 0;  i--)
        if (0==P->opaque->omit_inverse[i+1])
            break;
    if (i==-1) {
        pj_log (pj_get_ctx (P), PJ_LOG_ERROR, "Pipeline: No reverse steps");
        return pipeline_freeup (P, -50);
    }

    if (P->opaque->reverse_step[i + 1])
        P->right = P->opaque->pipeline[i + 1]->left;
    else
        P->right = P->opaque->pipeline[i + 1]->right;

    if (P->right==PJ_IO_UNITS_CLASSIC) {
        if (P->opaque->reverse_step[i + 1])
            P->right = PJ_IO_UNITS_RADIANS;
        else
            P->right = PJ_IO_UNITS_METERS;
    }
    pj_log_trace (P, "Pipeline: Units - left: [%s], right: [%s]\n",
        P->left ==PJ_IO_UNITS_RADIANS? "angular": "linear",
        P->right==PJ_IO_UNITS_RADIANS? "angular": "linear");

    return P;
}

#ifndef PJ_SELFTEST
/* selftest stub */
int pj_pipeline_selftest (void) {return 0;}
#else

int pj_pipeline_selftest (void) {
    PJ *P;
    PJ_OBS a, b;
    XY cph_utm32 = {691875.63214,  6098907.82501};
    double dist;

    /* forward-reverse geo->utm->geo */
    P = pj_create ("+proj=pipeline +ellps=GRS80 +zone=32 +step +proj=utm +step +proj=utm +inv");
    if (0==P)
        return 1000;
    /* zero initialize everything, then set (longitude, latitude, height) to (12, 55, 0) */
    a = b = pj_obs_null;
    a.coo.lpz.lam = TORAD(12);
    a.coo.lpz.phi = TORAD(55);
    a.coo.lpz.z   = 0;

    /* Forward projection */
    b = pj_trans (P, PJ_FWD, a);
    if (pj_lp_dist (P, a.coo.lp, b.coo.lp) > 1e-4)
        return 1001;

    /* Inverse projection (still same result: pipeline is symmetrical) */
    a = pj_trans (P, PJ_INV, b);
    if (pj_lp_dist (P, a.coo.lp, b.coo.lp) > 1e-4)
        return 1002;

    pj_free (P);

    /* And now the back-to-back situation utm->geo->utm */
    P = pj_create ("+proj=pipeline +ellps=GRS80 +zone=32 +step +proj=utm +inv +step +proj=utm");
    if (0==P)
        return 2000;

    /* zero initialize everything, then set (easting, northing) to utm(12, 55) */
    a = b = pj_obs_null;
    a.coo.xy = cph_utm32;

    /* Forward projection */
    b = pj_trans (P, PJ_FWD, a);
    if (pj_xy_dist (a.coo.xy, b.coo.xy) > 1e-4)
        return 2001;

    /* Inverse projection */
    a = pj_trans (P, PJ_INV, b);
    if (pj_xy_dist (a.coo.xy, b.coo.xy) > 1e-4)
        return 2001;
    if (pj_xyz_dist (a.coo.xyz, b.coo.xyz) > 1e-4)
        return 2002;

    pj_free (P);


    /* Finally testing a corner case: A rather pointless one-step pipeline geo->utm */
    P = pj_create ("+proj=pipeline +ellps=GRS80 +zone=32 +step +proj=utm");
    if (0==P)
        return 3000;

    a = b = pj_obs_null;
    a.coo.lpz.lam = TORAD(12);
    a.coo.lpz.phi = TORAD(55);

    /* Forward projection */
    b = pj_trans (P, PJ_FWD, a);
    if (pj_xy_dist (cph_utm32, b.coo.xy) > 1e-4)
        return 3001;

    /* Inverse projection */
    b = pj_trans (P, PJ_INV, b);
    if (pj_lp_dist (P, a.coo.lp, b.coo.lp) > 1e-4)
        return 3002;


    /* Since we use pj_lp_dist to determine success above, we should also test that it works */

    /* Geodesic distance between two points with angular 2D coordinates */
    a.coo.lp.lam = TORAD(12);
    a.coo.lp.phi = TORAD(60);
    b.coo.lp.lam = TORAD(12);
    b.coo.lp.phi = TORAD(61);
    dist = pj_lp_dist (P, a.coo.lp, b.coo.lp);
    if (fabs (111420.727870234 - dist) > 1e-4)
        return 4001;

    a.coo.lp.lam = TORAD(12);
    a.coo.lp.phi = TORAD(0.);
    b.coo.lp.lam = TORAD(12);
    b.coo.lp.phi = TORAD(1.);
    dist = pj_lp_dist (P, a.coo.lp, b.coo.lp);
    if (fabs (110574.388554153 - dist) > 1e-4)
        return 4002;

    pj_free (P);
    return 0;
}
#endif
