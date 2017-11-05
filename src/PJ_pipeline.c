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
    addition to the PROJ.4 code base: The implementation is by and large completed
    by adding an extra projection called "pipeline" (i.e. this file), which
    handles all business, and a small amount of added functionality in the
    pj_init code, implementing support for multilevel, embedded pipelines.

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
    Following geodetic conventions, the rotations are given in arcseconds,
    and the scale factor is given as parts-per-million.

    [1] B. W. Kernighan & P. J. Plauger: Software tools.
        Reading, Massachusetts, Addison-Wesley, 1976, 338 pp.

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
#include "proj_internal.h"
#include "projects.h"

#include <assert.h>
#include <stddef.h>
#include <errno.h>
PROJ_HEAD(pipeline,         "Transformation pipeline manager");

/* Projection specific elements for the PJ object */
struct pj_opaque {
    int reversible;
    int steps;
    int verbose;
    char **argv;
    char **current_argv;
    PJ **pipeline;
};



static PJ_COORD pipeline_forward_4d (PJ_COORD, PJ *P);
static PJ_COORD pipeline_reverse_4d (PJ_COORD, PJ *P);
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

    For classic proj style projections, P->left has the value
    PJ_IO_UNITS_RADIANS, while P->right has the value
    PJ_IO_UNITS_CLASSIC, indicating that the forward driver expects
    angular input coordinates, and provides linear output coordinates,
    scaled by the P->a semimajor axis length.

    Newer projections may set P->left and P->right to either
    PJ_IO_UNITS_METERS, PJ_IO_UNITS_RADIANS or PJ_IO_UNITS_ANY,
    to indicate their I/O style.

    For the forward driver, left indicates input coordinate
    type, while right indicates output coordinate type.

    For the inverse driver, left indicates output coordinate
    type, while right indicates input coordinate type.

*********************************************************************/




static PJ_COORD pipeline_forward_4d (PJ_COORD point, PJ *P) {
    int i, first_step, last_step;

    first_step = 1;
    last_step  = P->opaque->steps + 1;

    for (i = first_step;  i != last_step;  i++)
        point = proj_trans (P->opaque->pipeline[i], 1, point);

    return point;
}



static PJ_COORD pipeline_reverse_4d (PJ_COORD point, PJ *P) {
    int i, first_step, last_step;

    first_step = P->opaque->steps;
    last_step  =  0;

    for (i = first_step;  i != last_step;  i--)
        point = proj_trans (P->opaque->pipeline[i], -1, point);

    return point;
}


/* Delegate the work to pipeline_forward_4d() */
static XYZ pipeline_forward_3d (LPZ lpz, PJ *P) {
    PJ_COORD point = {{0,0,0,0}};
    point.lpz = lpz;
    point = pipeline_forward_4d (point, P);
    return point.xyz;
}

/* Delegate the work to pipeline_reverse_4d() */
static LPZ pipeline_reverse_3d (XYZ xyz, PJ *P) {
    PJ_COORD point = {{0,0,0,0}};
    point.xyz = xyz;
    point = pipeline_reverse_4d (point, P);
    return point.lpz;
}

static XY pipeline_forward (LP lp, PJ *P) {
    PJ_COORD point = {{0,0,0,0}};
    point.lp = lp;
    point = pipeline_forward_4d (point, P);
    return point.xy;
}

static LP pipeline_reverse (XY xy, PJ *P) {
    PJ_COORD point = {{0,0,0,0}};
    point.xy = xy;
    point = pipeline_reverse_4d (point, P);
    return point.lp;
}


static void *destructor (PJ *P, int errlev) {
    int i;
    if (0==P)
        return 0;

    if (0==P->opaque)
        return pj_default_destructor (P, errlev);

    /* Deallocate each pipeine step, then pipeline array */
    if (0!=P->opaque->pipeline)
        for (i = 0;  i < P->opaque->steps; i++)
            if (0!=P->opaque->pipeline[i+1])
                P->opaque->pipeline[i+1]->destructor (P->opaque->pipeline[i+1], errlev);
    pj_dealloc (P->opaque->pipeline);

    pj_dealloc (P->opaque->argv);
    pj_dealloc (P->opaque->current_argv);

    return pj_default_destructor(P, errlev);
}


static PJ *pj_create_pipeline (PJ *P, size_t steps) {

    /* Room for the pipeline: An array of PJ * with room for sentinels at both ends */
    P->opaque->pipeline = pj_calloc (steps + 2, sizeof(PJ *));
    if (0==P->opaque->pipeline)
        return 0;

    P->opaque->steps = (int)steps;

    return P;
}


/* count the number of args in pipeline definition */
static size_t argc_params (paralist *params) {
    size_t argc = 0;
    for (; params != 0; params = params->next)
        argc++;
    return ++argc;  /* one extra for the sentinel */
}

/* Sentinel for argument list */
static char argv_sentinel[] = "step";

/* turn paralist into argc/argv style argument list */
static char **argv_params (paralist *params, size_t argc) {
    char **argv;
    size_t i = 0;
    argv = pj_calloc (argc, sizeof (char *));
    if (0==argv)
        return 0;
    for (; params != 0; params = params->next)
        argv[i++] = params->param;
    argv[i++] = argv_sentinel;
    return argv;
}



PJ *PROJECTION(pipeline) {
    int i, nsteps = 0, argc;
    int i_pipeline = -1, i_first_step = -1, i_current_step;
    char **argv, **current_argv;

    P->fwdobs =  P->fwd4d = pipeline_forward_4d;
    P->invobs =  P->inv4d = pipeline_reverse_4d;
    P->fwd3d  =  pipeline_forward_3d;
    P->inv3d  =  pipeline_reverse_3d;
    P->fwd    =  pipeline_forward;
    P->inv    =  pipeline_reverse;
    P->destructor  =  destructor;

    P->opaque = pj_calloc (1, sizeof(struct pj_opaque));
    if (0==P->opaque)
        return pj_default_destructor(P, ENOMEM);

    argc = (int)argc_params (P->params);
    P->opaque->argv = argv = argv_params (P->params, argc);
    if (0==argv)
        return destructor (P, ENOMEM);

    P->opaque->current_argv = current_argv = pj_calloc (argc, sizeof (char *));
    if (0==current_argv)
        return destructor (P, ENOMEM);

    /* Do some syntactical sanity checking */
    for (i = 0;  i < argc;  i++) {
        if (0==strcmp ("step", argv[i])) {
            if (-1==i_pipeline) {
                proj_log_error (P, "Pipeline: +step before +proj=pipeline");
                return destructor (P, PJD_ERR_MALFORMED_PIPELINE);
            }
            if (0==nsteps)
                i_first_step = i;
            nsteps++;
            continue;
        }

        if (0==strcmp ("proj=pipeline", argv[i])) {
            if (-1 != i_pipeline) {
                proj_log_error (P, "Pipeline: Nesting only allowed when child pipelines are wrapped in +init's");
                return destructor (P, PJD_ERR_MALFORMED_PIPELINE); /* ERROR: nested pipelines */
            }
            i_pipeline = i;
        }
    }
    nsteps--; /* Last instance of +step is just a sentinel */
    P->opaque->steps = nsteps;

    if (-1==i_pipeline)
        return destructor (P, PJD_ERR_MALFORMED_PIPELINE); /* ERROR: no pipeline def */

    if (0==nsteps)
        return destructor (P, PJD_ERR_MALFORMED_PIPELINE); /* ERROR: no pipeline def */

    /* Make room for the pipeline and execution indicators */
    if (0==pj_create_pipeline (P, nsteps))
        return destructor (P, ENOMEM);

    /* Now loop over all steps, building a new set of arguments for each init */
    for (i_current_step = i_first_step, i = 0;  i < nsteps;  i++) {
        int j;
        int  current_argc = 0;
        PJ     *next_step = 0;

        /* Build a set of setup args for the current step */
        proj_log_trace (P, "Pipeline: Building arg list for step no. %d", i);

        /* First add the step specific args */
        for (j = i_current_step + 1;  0 != strcmp ("step", argv[j]); j++)
            current_argv[current_argc++] = argv[j];

        i_current_step = j;

        /* Then add the global args */
        for (j = i_pipeline + 1;  0 != strcmp ("step", argv[j]); j++)
            current_argv[current_argc++] = argv[j];

        proj_log_trace (P, "Pipeline: init - %s, %d", current_argv[0], current_argc);
        for (j = 1;  j < current_argc; j++)
            proj_log_trace (P, "    %s", current_argv[j]);

        next_step = pj_init_ctx (P->ctx, current_argc, current_argv);

        proj_log_trace (P, "Pipeline: Step %d at %p", i, next_step);
        if (0==next_step) {
            proj_log_error (P, "Pipeline: Bad step definition: %s", current_argv[0]);
            return destructor (P, PJD_ERR_MALFORMED_PIPELINE); /* ERROR: bad pipeline def */
        }

        /* Is this step inverted? */
        for (j = 0;  j < current_argc; j++)
            if (0==strcmp("inv", current_argv[j]))
                next_step->inverted = 1;

        P->opaque->pipeline[i+1] = next_step;

        proj_log_trace (P, "Pipeline:    step done");
    }

    proj_log_trace (P, "Pipeline: %d steps built. Determining i/o characteristics", nsteps);

    /* Determine forward input (= reverse output) data type */
    P->left = pj_left (P->opaque->pipeline[1]);

    /* Now, correspondingly determine forward output (= reverse input) data type */
    P->right = pj_right (P->opaque->pipeline[nsteps]);

    return P;
}

#ifndef PJ_SELFTEST
/* selftest stub */
int pj_pipeline_selftest (void) {return 0;}
#else

int pj_pipeline_selftest (void) {
    PJ *P;
    PJ_COORD a, b;
    XY cph_utm32 = {691875.63214,  6098907.82501};
    double dist;

    /* forward-reverse geo->utm->geo */
    P = proj_create (PJ_DEFAULT_CTX, "+proj=pipeline +zone=32 +step +proj=utm   +ellps=GRS80 +step +proj=utm   +ellps=GRS80 +inv");
    if (0==P)
        return 1000;
    /* zero initialize everything, then set (longitude, latitude, height) to (12, 55, 0) */
    a = b = proj_coord (0,0,0,0);
    a.lpz.lam = PJ_TORAD(12);
    a.lpz.phi = PJ_TORAD(55);
    a.lpz.z   = 10;

    /* Forward projection */
    b = proj_trans (P, PJ_FWD, a);
    if (proj_lp_dist (P, a.lp, b.lp) > 1e-4)
        return 1001;

    /* Inverse projection (still same result: pipeline is symmetrical) */
    a = proj_trans (P, PJ_INV, b);
    if (proj_lp_dist (P, a.lp, b.lp) > 1e-4)
        return 1002;

    proj_destroy (P);

    /* And now the back-to-back situation utm->geo->utm */
    P = proj_create (PJ_DEFAULT_CTX, "+proj=pipeline +zone=32 +step +proj=utm +ellps=GRS80 +inv +step +proj=utm +ellps=GRS80");
    if (0==P)
        return 2000;

    /* zero initialize everything, then set (easting, northing) to utm(12, 55) */
    a = b = proj_coord (0,0,0,0);
    a.xy = cph_utm32;

    /* Forward projection */
    b = proj_trans (P, PJ_FWD, a);
    if (proj_xy_dist (a.xy, b.xy) > 1e-4)
        return 2001;

    /* Inverse projection */
    a = proj_trans (P, PJ_INV, b);
    if (proj_xy_dist (a.xy, b.xy) > 1e-4)
        return 2001;
    if (proj_xyz_dist (a.xyz, b.xyz) > 1e-4)
        return 2002;

    proj_destroy (P);


    /* Finally testing a corner case: A rather pointless one-step pipeline geo->utm */
    P = proj_create (PJ_DEFAULT_CTX, "+proj=pipeline +zone=32 +step +proj=utm +ellps=GRS80 ");
    if (0==P)
        return 3000;


    a = b = proj_coord (0,0,0,0);
    a.lpz.lam = PJ_TORAD(12);
    a.lpz.phi = PJ_TORAD(55);

    /* Forward projection */
    b = proj_trans (P, PJ_FWD, a);
    if (proj_xy_dist (cph_utm32, b.xy) > 1e-4)
        return 3001;

    /* Inverse projection */
    b = proj_trans (P, PJ_INV, b);
    if (proj_lp_dist (P, a.lp, b.lp) > 1e-4)
        return 3002;


    /* Since we use pj_lp_dist to determine success above, we should also test that it works */

    /* Geodesic distance between two points with angular 2D coordinates */
    a.lp.lam = PJ_TORAD(12);
    a.lp.phi = PJ_TORAD(60);
    b.lp.lam = PJ_TORAD(12);
    b.lp.phi = PJ_TORAD(61);
    dist = proj_lp_dist (P, a.lp, b.lp);
    if (fabs (111420.727870234 - dist) > 1e-4)
        return 4001;

    a.lp.lam = PJ_TORAD(12);
    a.lp.phi = PJ_TORAD(0.);
    b.lp.lam = PJ_TORAD(12);
    b.lp.phi = PJ_TORAD(1.);
    dist = proj_lp_dist (P, a.lp, b.lp);
    if (fabs (110574.388554153 - dist) > 1e-4)
        return 4002;

    proj_destroy (P);

    /* test a pipeline with several +init steps */
    P = proj_create(
        0,
        "+proj=pipeline "
        "+step +init=epsg:25832 +inv "
        "+step +init=epsg:25833  "
        "+step +init=epsg:25833 +inv "
        "+step +init=epsg:25832 "
    );
    if (0==P)
        return 5000;

    a.xy.x =  700000.0;
    a.xy.y = 6000000.0;

    b = proj_trans(P, PJ_FWD, a);
    dist = proj_xy_dist(a.xy, b.xy);
    if (dist > 1e-7)
        return 5001;


    proj_destroy (P);
    return 0;
}
#endif
