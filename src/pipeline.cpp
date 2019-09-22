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
#include <stack>

#include "geodesic.h"
#include "proj.h"
#include "proj_internal.h"

PROJ_HEAD(pipeline,         "Transformation pipeline manager");
PROJ_HEAD(pop, "Retrieve coordinate value from pipeline stack");
PROJ_HEAD(push, "Save coordinate value on pipeline stack");

/* Projection specific elements for the PJ object */
namespace { // anonymous namespace
struct pj_opaque {
    int steps;
    char **argv;
    char **current_argv;
    PJ **pipeline;
    std::stack<double> *stack[4];
};

struct pj_opaque_pushpop {
    bool v1;
    bool v2;
    bool v3;
    bool v4;
};
} // anonymous namespace


static PJ_COORD pipeline_forward_4d (PJ_COORD point, PJ *P);
static PJ_COORD pipeline_reverse_4d (PJ_COORD point, PJ *P);
static PJ_XYZ    pipeline_forward_3d (PJ_LPZ lpz, PJ *P);
static PJ_LPZ    pipeline_reverse_3d (PJ_XYZ xyz, PJ *P);
static PJ_XY     pipeline_forward (PJ_LP lp, PJ *P);
static PJ_LP     pipeline_reverse (PJ_XY xy, PJ *P);




static PJ_COORD pipeline_forward_4d (PJ_COORD point, PJ *P) {
    int i, first_step, last_step;

    first_step = 1;
    last_step  = static_cast<struct pj_opaque*>(P->opaque)->steps + 1;

    for (i = first_step;  i != last_step;  i++)
        point = proj_trans (static_cast<struct pj_opaque*>(P->opaque)->pipeline[i], PJ_FWD, point);

    return point;
}


static PJ_COORD pipeline_reverse_4d (PJ_COORD point, PJ *P) {
    int i, first_step, last_step;

    first_step = static_cast<struct pj_opaque*>(P->opaque)->steps;
    last_step  =  0;

    for (i = first_step;  i != last_step;  i--)
        point = proj_trans (static_cast<struct pj_opaque*>(P->opaque)->pipeline[i], PJ_INV, point);

    return point;
}




static PJ_XYZ pipeline_forward_3d (PJ_LPZ lpz, PJ *P) {
    PJ_COORD point = {{0,0,0,0}};
    int i;
    point.lpz = lpz;

    for (i = 1;  i <= static_cast<struct pj_opaque*>(P->opaque)->steps;  i++)
        point = pj_approx_3D_trans (static_cast<struct pj_opaque*>(P->opaque)->pipeline[i], PJ_FWD, point);

    return point.xyz;
}


static PJ_LPZ pipeline_reverse_3d (PJ_XYZ xyz, PJ *P) {
    PJ_COORD point = {{0,0,0,0}};
    int i;
    point.xyz = xyz;

    for (i = static_cast<struct pj_opaque*>(P->opaque)->steps;  i > 0 ;  i--)
        point = pj_approx_3D_trans (static_cast<struct pj_opaque*>(P->opaque)->pipeline[i], PJ_INV, point);

    return point.lpz;
}




static PJ_XY pipeline_forward (PJ_LP lp, PJ *P) {
    PJ_COORD point = {{0,0,0,0}};
    int i;
    point.lp = lp;

    for (i = 1;  i <= static_cast<struct pj_opaque*>(P->opaque)->steps;  i++)
        point = pj_approx_2D_trans (static_cast<struct pj_opaque*>(P->opaque)->pipeline[i], PJ_FWD, point);

    return point.xy;
}


static PJ_LP pipeline_reverse (PJ_XY xy, PJ *P) {
    PJ_COORD point = {{0,0,0,0}};
    int i;
    point.xy = xy;
    for (i = static_cast<struct pj_opaque*>(P->opaque)->steps;  i > 0 ;  i--)
        point = pj_approx_2D_trans (static_cast<struct pj_opaque*>(P->opaque)->pipeline[i], PJ_INV, point);

    return point.lp;
}




static PJ *destructor (PJ *P, int errlev) {
    int i;
    if (nullptr==P)
        return nullptr;

    if (nullptr==P->opaque)
        return pj_default_destructor (P, errlev);

    /* Deallocate each pipeline step, then pipeline array */
    if (nullptr!=static_cast<struct pj_opaque*>(P->opaque)->pipeline)
        for (i = 0;  i < static_cast<struct pj_opaque*>(P->opaque)->steps; i++)
            proj_destroy (static_cast<struct pj_opaque*>(P->opaque)->pipeline[i+1]);
    pj_dealloc (static_cast<struct pj_opaque*>(P->opaque)->pipeline);

    pj_dealloc (static_cast<struct pj_opaque*>(P->opaque)->argv);
    pj_dealloc (static_cast<struct pj_opaque*>(P->opaque)->current_argv);

    for (i=0; i<4; i++)
        delete static_cast<struct pj_opaque*>(P->opaque)->stack[i];

    return pj_default_destructor(P, errlev);
}


static PJ *pj_create_pipeline (PJ *P, size_t steps) {

    /* Room for the pipeline: An array of PJ * with room for sentinels at both ends */
    static_cast<struct pj_opaque*>(P->opaque)->pipeline = static_cast<PJ**>(pj_calloc (steps + 2, sizeof(PJ *)));
    if (nullptr==static_cast<struct pj_opaque*>(P->opaque)->pipeline)
        return nullptr;

    static_cast<struct pj_opaque*>(P->opaque)->steps = (int)steps;

    return P;
}




/* count the number of args in pipeline definition, and mark all args as used */
static size_t argc_params (paralist *params) {
    size_t argc = 0;
    for (; params != nullptr; params = params->next) {
        argc++;
        params->used = 1;
    }
    return ++argc;  /* one extra for the sentinel */
}

/* Sentinel for argument list */
static const char *argv_sentinel = "step";

/* turn paralist into argc/argv style argument list */
static char **argv_params (paralist *params, size_t argc) {
    char **argv;
    size_t i = 0;
    argv = static_cast<char**>(pj_calloc (argc, sizeof (char *)));
    if (nullptr==argv)
        return nullptr;
    for (; params != nullptr; params = params->next)
        argv[i++] = params->param;
    argv[i++] = const_cast<char*>(argv_sentinel);
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
/* as the pipeline ellipsoid. Otherwise we use GRS80 parameters as default.  */
/* At last we calculate the rest of the ellipsoid parameters and             */
/* re-initialize P->geod.                                                    */
static void set_ellipsoid(PJ *P) {
    paralist *cur, *attachment;
    int err = proj_errno_reset (P);

    /* Break the linked list after the global args */
    attachment = nullptr;
    for (cur = P->params; cur != nullptr; cur = cur->next)
        /* cur->next will always be non 0 given argv_sentinel presence, */
        /* but this is far from being obvious for a static analyzer */
        if (cur->next != nullptr && strcmp(argv_sentinel, cur->next->param) == 0) {
            attachment = cur->next;
            cur->next = nullptr;
            break;
        }

    /* Check if there's any ellipsoid specification in the global params. */
    /* If not, use GRS80 as default                                       */
    if (0 != pj_ellipsoid (P)) {
        P->a  = 6378137.0;
        P->f = 1.0 / 298.257222101;
        P->es = 2*P->f - P->f*P->f;

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
    if( cur != nullptr )
        cur->next = attachment;
    proj_errno_restore (P, err);
}


static enum pj_io_units get_next_non_whatever_unit(void *pipeline_data, int step, PJ_DIRECTION dir) {
    PJ **pipeline = static_cast<struct pj_opaque*>(pipeline_data)->pipeline;
    int nsteps = static_cast<struct pj_opaque*>(pipeline_data)->steps;
    int i;

    if (dir == PJ_FWD) {
        for (i = step+1; i<=nsteps; i++) {
            if (pj_left(pipeline[i]) != pj_right(pipeline[i]))
                return pj_left(pipeline[i]);
            if (pj_left(pipeline[i]) != PJ_IO_UNITS_WHATEVER)
                return pj_left(pipeline[i]);
            if (pj_right(pipeline[i]) != PJ_IO_UNITS_WHATEVER)
                return pj_right(pipeline[i]);
        }
    } else {
        for (i=step; i>1; i--) {
            if (pj_right(pipeline[i]) != pj_left(pipeline[i]))
                return pj_right(pipeline[i]);
            if (pj_right(pipeline[i]) != PJ_IO_UNITS_WHATEVER)
                return pj_right(pipeline[i]);
            if (pj_left(pipeline[i]) != PJ_IO_UNITS_WHATEVER)
                return pj_left(pipeline[i]);
        }
    }
    return PJ_IO_UNITS_WHATEVER;
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


    P->opaque = static_cast<struct pj_opaque*>(pj_calloc (1, sizeof(struct pj_opaque)));
    if (nullptr==P->opaque)
        return destructor(P, ENOMEM);

    /* initialize stack */
    for (i=0; i<4; i++)
        static_cast<struct pj_opaque*>(P->opaque)->stack[i] = new std::stack<double>;

    argc = (int)argc_params (P->params);
    static_cast<struct pj_opaque*>(P->opaque)->argv = argv = argv_params (P->params, argc);
    if (nullptr==argv)
        return destructor (P, ENOMEM);

    static_cast<struct pj_opaque*>(P->opaque)->current_argv = current_argv = static_cast<char**>(pj_calloc (argc, sizeof (char *)));
    if (nullptr==current_argv)
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
    static_cast<struct pj_opaque*>(P->opaque)->steps = nsteps;

    if (-1==i_pipeline)
        return destructor (P, PJD_ERR_MALFORMED_PIPELINE); /* ERROR: no pipeline def */

    if (0==nsteps)
        return destructor (P, PJD_ERR_MALFORMED_PIPELINE); /* ERROR: no pipeline def */

    /* Make room for the pipeline and execution indicators */
    if (nullptr==pj_create_pipeline (P, nsteps))
        return destructor (P, ENOMEM);

    set_ellipsoid(P);

    /* Now loop over all steps, building a new set of arguments for each init */
    i_current_step = i_first_step;
    for (i = 0;  i < nsteps;  i++) {
        int j;
        int  current_argc = 0;
        int  err;
        PJ     *next_step = nullptr;

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

        next_step = pj_create_argv_internal (P->ctx, current_argc, current_argv);
        proj_log_trace (P, "Pipeline: Step %d (%s) at %p", i, current_argv[0], next_step);

        if (nullptr==next_step) {
            /* The step init failed, but possibly without setting errno. If so, we say "malformed" */
            int err_to_report = proj_errno(P);
            if (0==err_to_report)
                err_to_report = PJD_ERR_MALFORMED_PIPELINE;
            proj_log_error (P, "Pipeline: Bad step definition: %s (%s)", current_argv[0], pj_strerrno (err_to_report));
            return destructor (P, err_to_report); /* ERROR: bad pipeline def */
        }
        next_step->parent = P;

        proj_errno_restore (P, err);

        /* Is this step inverted? */
        for (j = 0;  j < current_argc; j++)
            if (0==strcmp("inv", current_argv[j])) {
                /* if +inv exists in both global and local args the forward operation should be used */
                next_step->inverted = next_step->inverted == 0 ? 1 : 0;
            }

        static_cast<struct pj_opaque*>(P->opaque)->pipeline[i+1] = next_step;

        proj_log_trace (P, "Pipeline at [%p]:    step at [%p] (%s) done", P, next_step, current_argv[0]);
    }

    /* Require a forward path through the pipeline */
    for (i = 1; i <= nsteps; i++) {
        PJ *Q = static_cast<struct pj_opaque*>(P->opaque)->pipeline[i];
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
        PJ *Q = static_cast<struct pj_opaque*>(P->opaque)->pipeline[i];
        if ( pj_has_inverse(Q) ) {
            continue;
        } else {
            P->inv   = nullptr;
            P->inv3d = nullptr;
            P->inv4d = nullptr;
            break;
        }
    }


    /* Replace PJ_IO_UNITS_WHATEVER with input/output units of neighbouring steps where */
    /* it make sense. It does in most cases but not always, for instance                */
    /*      proj=pipeline step proj=unitconvert xy_in=deg xy_out=rad step ...           */
    /* where the left-hand side units of the first step shouldn't be changed to RADIANS */
    /* as it will result in deg->rad conversions in cs2cs and other applications.       */
    PJ **pipeline = static_cast<struct pj_opaque*>(P->opaque)->pipeline;
    for (i=1; i<=nsteps; i++) {
        if (pj_left(pipeline[i]) == PJ_IO_UNITS_WHATEVER && pj_right(pipeline[i]) == PJ_IO_UNITS_WHATEVER) {
            pipeline[i]->left = get_next_non_whatever_unit(P->opaque, i, PJ_FWD);
            pipeline[i]->right = get_next_non_whatever_unit(P->opaque, i, PJ_FWD);
        }
    }

    for (i=nsteps; i>0; i--) {
        if (pj_left(pipeline[i]) == PJ_IO_UNITS_WHATEVER && pj_right(pipeline[i]) == PJ_IO_UNITS_WHATEVER) {
            pipeline[i]->right = get_next_non_whatever_unit(P->opaque, i, PJ_INV);
            pipeline[i]->left = get_next_non_whatever_unit(P->opaque, i, PJ_INV);
        }
    }

    /* Check that units between each steps match each other, fail if they don't */
    for (i = 1; i < nsteps; i++) {
        enum pj_io_units curr_step_output = pj_right (pipeline[i]);
        enum pj_io_units next_step_input  = pj_left  (pipeline[i+1]);

        if ( curr_step_output == PJ_IO_UNITS_WHATEVER || next_step_input == PJ_IO_UNITS_WHATEVER )
            continue;

        if ( curr_step_output != next_step_input ) {
            proj_log_error (P, "Pipeline: Mismatched units between step %d and %d", i, i+1);
            return destructor (P, PJD_ERR_MALFORMED_PIPELINE);
        }
    }

    proj_log_trace (P, "Pipeline: %d steps built. Determining i/o characteristics", nsteps);

    /* Determine forward input (= reverse output) data type */
    P->left = pj_left (static_cast<struct pj_opaque*>(P->opaque)->pipeline[1]);

    /* Now, correspondingly determine forward output (= reverse input) data type */
    P->right = pj_right (static_cast<struct pj_opaque*>(P->opaque)->pipeline[nsteps]);
    return P;
}

static PJ_COORD push(PJ_COORD point, PJ *P) {
    if (P->parent == nullptr)
        return point;

    struct pj_opaque *pipeline = static_cast<struct pj_opaque*>(P->parent->opaque);
    struct pj_opaque_pushpop *opaque = static_cast<struct pj_opaque_pushpop*>(P->opaque);

    if (opaque->v1)
        pipeline->stack[0]->push(point.v[0]);
    if (opaque->v2)
        pipeline->stack[1]->push(point.v[1]);
    if (opaque->v3)
        pipeline->stack[2]->push(point.v[2]);
    if (opaque->v4)
        pipeline->stack[3]->push(point.v[3]);

    return point;
}

static PJ_COORD pop(PJ_COORD point, PJ *P) {
    if (P->parent == nullptr)
        return point;

    struct pj_opaque *pipeline = static_cast<struct pj_opaque*>(P->parent->opaque);
    struct pj_opaque_pushpop *opaque = static_cast<struct pj_opaque_pushpop*>(P->opaque);

    if (opaque->v1 && !pipeline->stack[0]->empty()) {
            point.v[0] = pipeline->stack[0]->top();
            pipeline->stack[0]->pop();
    }

    if (opaque->v2 && !pipeline->stack[1]->empty()) {
            point.v[1] = pipeline->stack[1]->top();
            pipeline->stack[1]->pop();
    }

    if (opaque->v3 && !pipeline->stack[2]->empty()) {
            point.v[2] = pipeline->stack[2]->top();
            pipeline->stack[2]->pop();
    }

    if (opaque->v4 && !pipeline->stack[3]->empty()) {
            point.v[3] = pipeline->stack[3]->top();
            pipeline->stack[3]->pop();
    }

    return point;
}



static PJ *setup_pushpop(PJ *P) {
    P->opaque = static_cast<struct pj_opaque_pushpop*>(pj_calloc (1, sizeof(struct pj_opaque_pushpop)));
    if (nullptr==P->opaque)
        return destructor(P, ENOMEM);

    if (pj_param_exists(P->params, "v_1"))
        static_cast<struct pj_opaque_pushpop*>(P->opaque)->v1 = true;

    if (pj_param_exists(P->params, "v_2"))
        static_cast<struct pj_opaque_pushpop*>(P->opaque)->v2 = true;

    if (pj_param_exists(P->params, "v_3"))
        static_cast<struct pj_opaque_pushpop*>(P->opaque)->v3 = true;

    if (pj_param_exists(P->params, "v_4"))
        static_cast<struct pj_opaque_pushpop*>(P->opaque)->v4 = true;

    P->left  = PJ_IO_UNITS_WHATEVER;
    P->right = PJ_IO_UNITS_WHATEVER;

    return P;
}


PJ *OPERATION(push, 0) {
    P->fwd4d = push;
    P->inv4d = pop;

    return setup_pushpop(P);
}

PJ *OPERATION(pop, 0) {
    P->inv4d = push;
    P->fwd4d = pop;

    return setup_pushpop(P);
}
