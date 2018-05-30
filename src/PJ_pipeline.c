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
    indicates the start of each transformation step), and reintroduces the +inv
    keyword (indicating that a given transformation step should run in reverse, i.e.
    forward, when the pipeline is executed in inverse direction, and vice versa).

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
* Copyright (c) 2016, 2017, 2018 Thomas Knudsen / SDFE
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

#include <errno.h>
#include <math.h>
#include <stddef.h>
#include <string.h>

#include "geodesic.h"
#include "proj.h"
#include "proj_internal.h"
#include "projects.h"

PROJ_HEAD(pipeline,         "Transformation pipeline manager");

/* Projection specific elements for the PJ object */
struct pj_opaque {
    int steps;
    char **argv;
    char **current_argv;
    PJ **pipeline;
};



static PJ_COORD pipeline_forward_4d (PJ_COORD point, PJ *P);
static PJ_COORD pipeline_reverse_4d (PJ_COORD point, PJ *P);
static XYZ    pipeline_forward_3d (LPZ lpz, PJ *P);
static LPZ    pipeline_reverse_3d (XYZ xyz, PJ *P);
static XY     pipeline_forward (LP lp, PJ *P);
static LP     pipeline_reverse (XY xy, PJ *P);




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




static XYZ pipeline_forward_3d (LPZ lpz, PJ *P) {
    PJ_COORD point = {{0,0,0,0}};
    int i;
    point.lpz = lpz;

    for (i = 1;  i <= P->opaque->steps;  i++)
        point = pj_approx_3D_trans (P->opaque->pipeline[i], 1, point);

    return point.xyz;
}


static LPZ pipeline_reverse_3d (XYZ xyz, PJ *P) {
    PJ_COORD point = {{0,0,0,0}};
    int i;
    point.xyz = xyz;

    for (i = P->opaque->steps;  i > 0 ;  i--)
        point = pj_approx_3D_trans (P->opaque->pipeline[i], -1, point);

    return point.lpz;
}




static XY pipeline_forward (LP lp, PJ *P) {
    PJ_COORD point = {{0,0,0,0}};
    int i;
    point.lp = lp;

    for (i = 1;  i <= P->opaque->steps;  i++)
        point = pj_approx_2D_trans (P->opaque->pipeline[i], 1, point);

    return point.xy;
}


static LP pipeline_reverse (XY xy, PJ *P) {
    PJ_COORD point = {{0,0,0,0}};
    int i;
    point.xy = xy;
    for (i = P->opaque->steps;  i > 0 ;  i--)
        point = pj_approx_2D_trans (P->opaque->pipeline[i], -1, point);

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
            proj_destroy (P->opaque->pipeline[i+1]);
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




/* count the number of args in pipeline definition, and mark all args as used */
static size_t argc_params (paralist *params) {
    size_t argc = 0;
    for (; params != 0; params = params->next) {
        argc++;
        params->used = 1;
    }
    return ++argc;  /* one extra for the sentinel */
}

/* Sentinel for argument list */
static char *argv_sentinel = "step";

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




/* Being the special operator that the pipeline is, we have to handle the    */
/* ellipsoid differently than usual. In general, the pipeline operation does */
/* not need an ellipsoid, but in some cases it is beneficial nonetheless.    */
/* Unfortunately we can't use the normal ellipsoid setter in pj_init, since  */
/* it adds a +ellps parameter to the global args if nothing else is specified*/
/* This is problematic since that ellipsoid spec is then passed on to the    */
/* pipeline children. This is rarely what we want, so here we implement our  */
/* own logic instead. If an ellipsoid is set in the global args, it is used  */
/* as the pipeline ellipsoid. Otherwise we use WGS84 parameters as default.  */
/* At last we calculate the rest of the ellipsoid parameters and             */
/* re-initialize P->geod.                                                    */
static void set_ellipsoid(PJ *P) {
    paralist *cur, *attachment;
    int err = proj_errno_reset (P);

    /* Break the linked list after the global args */
    attachment = 0;
    for (cur = P->params; cur != 0; cur = cur->next)
        /* cur->next will always be non 0 given argv_sentinel presence, */
        /* but this is far from being obvious for a static analyzer */
        if (cur->next != 0 && strcmp(argv_sentinel, cur->next->param) == 0) {
            attachment = cur->next;
            cur->next = 0;
            break;
        }

    /* Check if there's any ellipsoid specification in the global params. */
    /* If not, use WGS84 as default                                       */
    if (0 != pj_ellipsoid (P)) {
        P->a  = 6378137.0;
        P->es = .00669438002290341575;

        /* reset an "unerror": In this special use case, the errno is    */
        /* not an error signal, but just a reply from pj_ellipsoid,      */
        /* telling us that "No - there was no ellipsoid definition in    */
        /* the PJ you provided".                                         */
        proj_errno_reset (P);
    }
    P->a_orig = P->a;
    P->es_orig = P->es;

    pj_calc_ellipsoid_params (P, P->a, P->es);

    geod_init(P->geod, P->a,  (1 - sqrt (1 - P->es)));

    /* Re-attach the dangling list */
    /* Note: cur will always be non 0 given argv_sentinel presence, */
    /* but this is far from being obvious for a static analyzer */
    if( cur != 0 )
        cur->next = attachment;
    proj_errno_restore (P, err);
}




PJ *OPERATION(pipeline,0) {
    int i, nsteps = 0, argc;
    int i_pipeline = -1, i_first_step = -1, i_current_step;
    char **argv, **current_argv;

    P->fwd4d  =  pipeline_forward_4d;
    P->inv4d  =  pipeline_reverse_4d;
    P->fwd3d  =  pipeline_forward_3d;
    P->inv3d  =  pipeline_reverse_3d;
    P->fwd    =  pipeline_forward;
    P->inv    =  pipeline_reverse;
    P->destructor  =  destructor;
    P->is_pipeline =  1;

    /* Currently, the pipeline driver is a raw bit mover, enabling other operations */
    /* to collaborate efficiently. All prep/fin stuff is done at the step levels. */
    P->skip_fwd_prepare  = 1;
    P->skip_fwd_finalize = 1;
    P->skip_inv_prepare  = 1;
    P->skip_inv_finalize = 1;


    P->opaque = pj_calloc (1, sizeof(struct pj_opaque));
    if (0==P->opaque)
        return destructor(P, ENOMEM);

    argc = (int)argc_params (P->params);
    P->opaque->argv = argv = argv_params (P->params, argc);
    if (0==argv)
        return destructor (P, ENOMEM);

    P->opaque->current_argv = current_argv = pj_calloc (argc, sizeof (char *));
    if (0==current_argv)
        return destructor (P, ENOMEM);

    /* Do some syntactical sanity checking */
    for (i = 0;  i < argc;  i++) {
        if (0==strcmp (argv_sentinel, argv[i])) {
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
                proj_log_error (P, "Pipeline: Nesting only allowed when child pipelines are wrapped in '+init's");
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

    set_ellipsoid(P);

    /* Now loop over all steps, building a new set of arguments for each init */
    i_current_step = i_first_step;
    for (i = 0;  i < nsteps;  i++) {
        int j;
        int  current_argc = 0;
        int  err;
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

        err = proj_errno_reset (P);

        next_step = proj_create_argv (P->ctx, current_argc, current_argv);
        proj_log_trace (P, "Pipeline: Step %d (%s) at %p", i, current_argv[0], next_step);

        if (0==next_step) {
            /* The step init failed, but possibly without setting errno. If so, we say "malformed" */
            int err_to_report = proj_errno(P);
            if (0==err_to_report)
                err_to_report = PJD_ERR_MALFORMED_PIPELINE;
            proj_log_error (P, "Pipeline: Bad step definition: %s (%s)", current_argv[0], pj_strerrno (err_to_report));
            return destructor (P, err_to_report); /* ERROR: bad pipeline def */
        }

        proj_errno_restore (P, err);

        /* Is this step inverted? */
        for (j = 0;  j < current_argc; j++)
            if (0==strcmp("inv", current_argv[j])) {
                /* if +inv exists in both global and local args the forward operation should be used */
                next_step->inverted = next_step->inverted == 0 ? 1 : 0;
            }

        P->opaque->pipeline[i+1] = next_step;

        proj_log_trace (P, "Pipeline at [%p]:    step at [%p] (%s) done", P, next_step, current_argv[0]);
    }

    /* Require a forward path through the pipeline */
    for (i = 1; i <= nsteps; i++) {
        PJ *Q = P->opaque->pipeline[i];
        if ( ( Q->inverted && (Q->inv || Q->inv3d || Q->fwd4d) ) ||
             (!Q->inverted && (Q->fwd || Q->fwd3d || Q->fwd4d) ) ) {
            continue;
        } else {
            proj_log_error (P, "Pipeline: A forward operation couldn't be constructed");
            return destructor (P, PJD_ERR_MALFORMED_PIPELINE);
        }
    }

    /* determine if an inverse operation is possible */
    for (i = 1; i <= nsteps; i++) {
        PJ *Q = P->opaque->pipeline[i];
        if ( pj_has_inverse(Q) ) {
            continue;
        } else {
            P->inv   = 0;
            P->inv3d = 0;
            P->inv4d = 0;
            break;
        }
    }

    /* Check that output units from step i are compatible with expected units in step i+1 */
    for (i = 1; i < nsteps; i++) {
        enum pj_io_units unit_returned = pj_right (P->opaque->pipeline[i]);
        enum pj_io_units unit_expected = pj_left  (P->opaque->pipeline[i+1]);

        if ( unit_returned == PJ_IO_UNITS_WHATEVER || unit_expected == PJ_IO_UNITS_WHATEVER )
            continue;
        if ( unit_returned != unit_expected ) {
            proj_log_error (P, "Pipeline: Mismatched units between step %d and %d", i, i+1);
            return destructor (P, PJD_ERR_MALFORMED_PIPELINE);
        }
    }

    proj_log_trace (P, "Pipeline: %d steps built. Determining i/o characteristics", nsteps);

    /* Determine forward input (= reverse output) data type */
    P->left = pj_left (P->opaque->pipeline[1]);

    /* Now, correspondingly determine forward output (= reverse input) data type */
    P->right = pj_right (P->opaque->pipeline[nsteps]);
    return P;
}
