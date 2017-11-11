/******************************************************************************
 * Project:  PROJ.4
 * Purpose:  Initialize projection object from string definition.  Includes
 *           pj_init(), pj_init_plus() and pj_free() function.
 * Author:   Gerald Evenden, Frank Warmerdam <warmerdam@pobox.com>
 *
 ******************************************************************************
 * Copyright (c) 1995, Gerald Evenden
 * Copyright (c) 2002, Frank Warmerdam <warmerdam@pobox.com>
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
 *****************************************************************************/

#define PJ_LIB__
#include <geodesic.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include "proj_internal.h"
#include "projects.h"

/* Maximum size of files using the "escape carriage return" feature */
#define MAX_CR_ESCAPE 65537
typedef struct {
    projCtx ctx;
    PAFile fid;
    char buffer[MAX_CR_ESCAPE];
    int buffer_filled;
    int at_eof;
} pj_read_state;

/************************************************************************/
/*                            fill_buffer()                             */
/************************************************************************/

static const char *fill_buffer(pj_read_state *state, const char *last_char)
{
    size_t bytes_read;
    size_t char_remaining, char_requested;
    char   *r, *w;

/* -------------------------------------------------------------------- */
/*      Don't bother trying to read more if we are at eof, or if the    */
/*      buffer is still over half full.                                 */
/* -------------------------------------------------------------------- */
    if (last_char == NULL)
        last_char = state->buffer;

    if (state->at_eof)
        return last_char;

    char_remaining = state->buffer_filled - (last_char - state->buffer);
    if (char_remaining >= sizeof(state->buffer) / 2)
        return last_char;

/* -------------------------------------------------------------------- */
/*      Move the existing data to the start of the buffer.              */
/* -------------------------------------------------------------------- */
    memmove(state->buffer, last_char, char_remaining);
    state->buffer_filled = (int)char_remaining;
    last_char = state->buffer;

/* -------------------------------------------------------------------- */
/*      Refill.                                                         */
/* -------------------------------------------------------------------- */
    char_requested = sizeof(state->buffer) - state->buffer_filled - 1;
    bytes_read = pj_ctx_fread( state->ctx, state->buffer + state->buffer_filled,
                               1, char_requested, state->fid );
    if (bytes_read < char_requested)
    {
        state->at_eof = 1;
        state->buffer[state->buffer_filled + bytes_read] = '\0';
    }

/* -------------------------------------------------------------------- */
/*      Line continuations: skip whitespace after escaped newlines      */
/* -------------------------------------------------------------------- */
    r = state->buffer;
    w = state->buffer;
    while (*r) {
        /* Escaped newline? */
        while ((r[0]=='\\')  &&  ((r[1]=='\n') || (r[1]=='\r'))) {
            r += 2;
            while (isspace (*r))
                r++;
            /* we also skip comments immediately after an escaped newline */
            while (*r=='#') {
                while( *r && (*r != '\n') )
                    r++;
                while (isspace (*r))
                    r++;
                /* Reaching end of buffer while skipping continuation comment is currently an error */
                if (0==*r) {
                    pj_ctx_set_errno (state->ctx, -2);
                    pj_log (state->ctx, PJ_LOG_ERROR, "init file too big");
                    return 0;
                }
            }
        }
        *w++ = *r++;
    }
    *w = 0;
    state->buffer_filled += (int)(bytes_read - (r-w));

    return last_char;
}

/************************************************************************/
/*                              get_opt()                               */
/************************************************************************/
static paralist *
get_opt(projCtx ctx, paralist **start, PAFile fid, char *name, paralist *next,
        int *found_def) {
    pj_read_state *state = (pj_read_state*) calloc(1,sizeof(pj_read_state));
    char sword[MAX_CR_ESCAPE];
    int len;
    int in_target = 0;
    const char *next_char = NULL;
    state->fid = fid;
    state->ctx = ctx;
    next_char = fill_buffer(state, NULL);
    if(found_def)
        *found_def = 0;

    len = (int)strlen(name);
    *sword = 't';

    if (0==next_char)
        return 0;

    /* loop till we find our target keyword */
    while (*next_char)
    {
        next_char = fill_buffer(state, next_char);

        /* Skip white space. */
        while( isspace(*next_char) )
            next_char++;

        next_char = fill_buffer(state, next_char);
        if (0==next_char)
            return 0;

        /* for comments, skip past end of line. */
        if( *next_char == '#' )
        {
            while( *next_char && *next_char != '\n' )
                next_char++;

            next_char = fill_buffer(state, next_char);
            if (0==next_char)
                return 0;
            if (*next_char == '\n')
                next_char++;
            if (*next_char == '\r')
                next_char++;

        }

        /* Is this our target? */
        else if( *next_char == '<' )
        {
            /* terminate processing target on the next block definition */
            if (in_target)
                break;

            next_char++;
            if (strncmp(name, next_char, len) == 0
                && next_char[len] == '>')
            {
                /* skip past target word */
                next_char += len + 1;
                in_target = 1;
                if(found_def)
                    *found_def = 1;
            }
            else
            {
                /* skip past end of line */
                while( *next_char && *next_char != '\n' )
                    next_char++;
            }
        }
        else if (in_target)
        {
            const char *start_of_word = next_char;
            int word_len = 0;

            if (*start_of_word == '+')
            {
                start_of_word++;
                next_char++;
            }

            /* capture parameter */
            while ( *next_char && !isspace(*next_char) )
            {
                next_char++;
                word_len++;
            }

            strncpy(sword+1, start_of_word, word_len);
            sword[word_len+1] = '\0';

            /* do not override existing parameter value of same name - unless in pipeline definition */
            if (!pj_param(ctx, *start, sword).i) {
                /* don't default ellipse if datum, ellps or any earth model
                   information is set. */
                if( strncmp(sword+1,"ellps=",6) != 0
                    || (!pj_param(ctx, *start, "tdatum").i
                        && !pj_param(ctx, *start, "tellps").i
                        && !pj_param(ctx, *start, "ta").i
                        && !pj_param(ctx, *start, "tb").i
                        && !pj_param(ctx, *start, "trf").i
                        && !pj_param(ctx, *start, "tf").i) )
                {
                    next = next->next = pj_mkparam(sword+1);
                }
                else
                    next = next->next = pj_mkparam(sword+1);
            }
        }
        else
        {
            /* skip past word */
            while( *next_char && !isspace(*next_char) ) {
                next_char++;
            }

        }
    }

    if (errno == 25)
        errno = 0;

    free(state);
    return next;
}

/************************************************************************/
/*                            get_defaults()                            */
/************************************************************************/
static paralist *get_defaults(projCtx ctx, paralist **start, paralist *next, char *name) {
    PAFile fid;

    if ( (fid = pj_open_lib(ctx,"proj_def.dat", "rt")) != NULL) {
        next = get_opt(ctx, start, fid, "general", next, NULL);
        pj_ctx_fseek(ctx, fid, 0, SEEK_SET);
        next = get_opt(ctx, start, fid, name, next, NULL);
        pj_ctx_fclose(ctx, fid);
    }
    if (errno)
        errno = 0; /* don't care if can't open file */
    ctx->last_errno = 0;

    return next;
}

/************************************************************************/
/*                              get_init()                              */
/************************************************************************/
static paralist *get_init(projCtx ctx, paralist **start, paralist *next, char *name, int *found_def) {
    char fname[MAX_PATH_FILENAME+ID_TAG_MAX+3], *opt;
    PAFile fid;
    paralist *init_items = NULL;
    const paralist *orig_next = next;

    (void)strncpy(fname, name, sizeof(fname)-2);
    fname[sizeof(fname)-2] = '\0';

    /*
    ** Search for file/key pair in cache
    */

    init_items = pj_search_initcache( name );
    if( init_items != NULL )
    {
        next->next = init_items;
        while( next->next != NULL )
            next = next->next;
        *found_def = 1;
        return next;
    }

    /*
    ** Otherwise we try to open the file and search for it.
    */
    if ((opt = strrchr(fname, ':')) != NULL)
        *opt++ = '\0';
    else { pj_ctx_set_errno(ctx,-3); return NULL; }

    if ( (fid = pj_open_lib(ctx,fname, "rt")) != NULL)
        next = get_opt(ctx, start, fid, opt, next, found_def);
    else
        return NULL;

    pj_ctx_fclose(ctx, fid);
    if (errno == 25)
        errno = 0; /* unknown problem with some sys errno<-25 */

    /*
    ** If we seem to have gotten a result, insert it into the
    ** init file cache.
    */
    if( next != NULL && next != orig_next )
        pj_insert_initcache( name, orig_next->next );

    return next;
}

paralist * pj_get_init(projCtx ctx, paralist **start, paralist *next, char *name, int *found_def) {
    return get_init(ctx, start, next, name, found_def);
}

/************************************************************************/
/*                            pj_init_plus()                            */
/*                                                                      */
/*      Same as pj_init() except it takes one argument string with      */
/*      individual arguments preceded by '+', such as "+proj=utm       */
/*      +zone=11 +ellps=WGS84".                                         */
/************************************************************************/

PJ *
pj_init_plus( const char *definition )

{
    return pj_init_plus_ctx( pj_get_default_ctx(), definition );
}

PJ *
pj_init_plus_ctx( projCtx ctx, const char *definition )
{
#define MAX_ARG 200
    char	*argv[MAX_ARG];
    char	*defn_copy;
    int		argc = 0, i, blank_count = 0;
    PJ	    *result = NULL;

    /* make a copy that we can manipulate */
    defn_copy = (char *) pj_malloc( strlen(definition)+1 );
    if (!defn_copy)
        return NULL;
    strcpy( defn_copy, definition );

    /* split into arguments based on '+' and trim white space */

    for( i = 0; defn_copy[i] != '\0'; i++ )
    {
        switch( defn_copy[i] )
        {
          case '+':
            if( i == 0 || defn_copy[i-1] == '\0' || blank_count > 0 )
            {
                /* trim trailing spaces from the previous param */
                if( blank_count > 0 )
                {
                    defn_copy[i - blank_count] = '\0';
                    blank_count = 0;
                }

                if( argc+1 == MAX_ARG )
                {
                    pj_dalloc( defn_copy );
                    pj_ctx_set_errno( ctx, PJD_ERR_UNPARSEABLE_CS_DEF );
                    return 0;
                }

                argv[argc++] = defn_copy + i + 1;
            }
            break;

          case ' ':
          case '\t':
          case '\n':
            /* trim leading spaces from the current param */
            if( i == 0 || defn_copy[i-1] == '\0' || argc == 0 || argv[argc-1] == defn_copy + i )
                defn_copy[i] = '\0';
            else
                blank_count++;
            break;

          default:
            /* reset blank_count */
            blank_count = 0;
        }
    }
    /* trim trailing spaces from the last param */
    defn_copy[i - blank_count] = '\0';

    /* perform actual initialization */
    result = pj_init_ctx( ctx, argc, argv );

    pj_dalloc( defn_copy );
    return result;
}



/************************************************************************/
/*                              pj_init()                               */
/*                                                                      */
/*      Main entry point for initialing a PJ projections                */
/*      definition.  Note that the projection specific function is      */
/*      called to do the initial allocation so it can be created        */
/*      large enough to hold projection specific parameters.            */
/************************************************************************/

PJ *
pj_init(int argc, char **argv) {
    return pj_init_ctx( pj_get_default_ctx(), argc, argv );
}

PJ *
pj_init_ctx(projCtx ctx, int argc, char **argv) {
    char *s, *name;
    paralist *start = NULL;
    PJ *(*proj)(PJ *);
    paralist *curr;
    int i;
    int found_def = 0;
    PJ *PIN = 0;
    int n_pipelines = 0;
    int n_inits = 0;

    if (0==ctx)
        ctx = pj_get_default_ctx ();

    ctx->last_errno = 0;
    start = NULL;

    if (argc <= 0) {
        pj_ctx_set_errno (ctx, PJD_ERR_NO_ARGS);
        return 0;
    }

    /* count occurrences of pipelines and inits */
    for (i = 0; i < argc; ++i) {
        if (!strcmp (argv[i], "+proj=pipeline") || !strcmp(argv[i], "proj=pipeline") )
                n_pipelines++;
        if (!strncmp (argv[i], "+init=", 6) || !strncmp(argv[i], "init=", 5))
            n_inits++;
    }

    /* can't have nested pipeline directly */
    if (n_pipelines > 1) {
        pj_ctx_set_errno (ctx, PJD_ERR_MALFORMED_PIPELINE);
        return 0;
    }

    /* don't allow more than one +init in non-pipeline operations */
    if (n_pipelines == 0 && n_inits > 1) {
        pj_ctx_set_errno (ctx, PJD_ERR_TOO_MANY_INITS);
        return 0;
    }

    /* put arguments into internal linked list */
    start = curr = pj_mkparam(argv[0]);
    if (!curr)
        return pj_dealloc_params (ctx, start, ENOMEM);

    for (i = 1; i < argc; ++i) {
        curr->next = pj_mkparam(argv[i]);
        if (!curr->next)
            return pj_dealloc_params (ctx, start, ENOMEM);
        curr = curr->next;
    }

    /* Only expand +init's in non-pipeline operations. +init's in pipelines are  */
    /* expanded in the individual pipeline steps during pipeline initialization. */
    /* Potentially this leads to many nested pipelines, which shouldn't be a     */
    /* problem when +inits are expanded as late as possible.                     */
    if (pj_param(ctx, start, "tinit").i && n_pipelines == 0) {
        found_def = 0;
        curr = get_init(ctx, &start, curr, pj_param(ctx, start, "sinit").s, &found_def);
        if (!curr)
            return pj_dealloc_params (ctx, start, PJD_ERR_NO_ARGS);
        if (!found_def)
            return pj_dealloc_params (ctx, start, PJD_ERR_NO_OPTION_IN_INIT_FILE);
    }

    if (ctx->last_errno)
        return pj_dealloc_params (ctx, start, ctx->last_errno);

    /* find projection selection */
    if (!(name = pj_param(ctx, start, "sproj").s))
        return pj_dealloc_params (ctx, start, PJD_ERR_PROJ_NOT_NAMED);
    for (i = 0; (s = pj_list[i].id) && strcmp(name, s) ; ++i) ;

    if (!s)
        return pj_dealloc_params (ctx, start, PJD_ERR_UNKNOWN_PROJECTION_ID);

    /* set defaults, unless inhibited or we are initializing a pipeline */
    if (!(pj_param(ctx, start, "bno_defs").i) && n_pipelines == 0)
            curr = get_defaults(ctx,&start, curr, name);

    proj = (PJ *(*)(PJ *)) pj_list[i].proj;

    /* allocate projection structure */
    PIN = proj(0);
    if (0==PIN)
        return pj_dealloc_params (ctx, start, ENOMEM);

    PIN->ctx = ctx;
    PIN->params = start;
    PIN->is_latlong = 0;
    PIN->is_geocent = 0;
    PIN->is_long_wrap_set = 0;
    PIN->long_wrap_center = 0.0;
    strcpy( PIN->axis, "enu" );

    PIN->gridlist = NULL;
    PIN->gridlist_count = 0;

    PIN->vgridlist_geoid = NULL;
    PIN->vgridlist_geoid_count = 0;

    /* set datum parameters */
    if (pj_datum_set(ctx, start, PIN))
        return pj_default_destructor (PIN, PJD_ERR_MISSING_ARGS);

    if (PIN->need_ellps) {
        if (pj_ell_set(ctx, start, &PIN->a, &PIN->es)) {
            pj_log (ctx, PJ_LOG_DEBUG_MINOR, "pj_init_ctx: Must specify ellipsoid or sphere");
            return pj_default_destructor (PIN, PJD_ERR_MISSING_ARGS);
        }

        PIN->a_orig = PIN->a;
        PIN->es_orig = PIN->es;
        if (pj_calc_ellps_params(PIN, PIN->a, PIN->es))
            return pj_default_destructor (PIN, PJD_ERR_ECCENTRICITY_IS_ONE);
    }

    /* Now that we have ellipse information check for WGS84 datum */
    if( PIN->datum_type == PJD_3PARAM
        && PIN->datum_params[0] == 0.0
        && PIN->datum_params[1] == 0.0
        && PIN->datum_params[2] == 0.0
        && PIN->a == 6378137.0
        && ABS(PIN->es - 0.006694379990) < 0.000000000050 )/*WGS84/GRS80*/
    {
        PIN->datum_type = PJD_WGS84;
    }

    /* set PIN->geoc coordinate system */
    PIN->geoc = (PIN->es != 0.0 && pj_param(ctx, start, "bgeoc").i);

    /* over-ranging flag */
    PIN->over = pj_param(ctx, start, "bover").i;

    /* vertical datum geoid grids */
    PIN->has_geoid_vgrids = pj_param(ctx, start, "tgeoidgrids").i;
    if( PIN->has_geoid_vgrids ) /* we need to mark it as used. */
        pj_param(ctx, start, "sgeoidgrids");

    /* longitude center for wrapping */
    PIN->is_long_wrap_set = pj_param(ctx, start, "tlon_wrap").i;
    if (PIN->is_long_wrap_set) {
        PIN->long_wrap_center = pj_param(ctx, start, "rlon_wrap").f;
        /* Don't accept excessive values otherwise we might perform badly */
        /* when correcting longitudes around it */
        /* The test is written this way to error on long_wrap_center "=" NaN */
        if( !(fabs(PIN->long_wrap_center) < 10 * M_TWOPI) )
            return pj_default_destructor (PIN, PJD_ERR_LAT_OR_LON_EXCEED_LIMIT);
    }

    /* axis orientation */
    if( (pj_param(ctx, start,"saxis").s) != NULL )
    {
        static const char *axis_legal = "ewnsud";
        const char *axis_arg = pj_param(ctx, start,"saxis").s;
        if( strlen(axis_arg) != 3 )
            return pj_default_destructor (PIN, PJD_ERR_AXIS);

        if( strchr( axis_legal, axis_arg[0] ) == NULL
            || strchr( axis_legal, axis_arg[1] ) == NULL
            || strchr( axis_legal, axis_arg[2] ) == NULL)
            return pj_default_destructor (PIN, PJD_ERR_AXIS);

        /* it would be nice to validate we don't have on axis repeated */
        strcpy( PIN->axis, axis_arg );
    }

    /* central meridian */
    PIN->lam0=pj_param(ctx, start, "rlon_0").f;

    /* central latitude */
    PIN->phi0 = pj_param(ctx, start, "rlat_0").f;

    /* false easting and northing */
    PIN->x0 = pj_param(ctx, start, "dx_0").f;
    PIN->y0 = pj_param(ctx, start, "dy_0").f;
    PIN->z0 = pj_param(ctx, start, "dz_0").f;
    PIN->t0 = pj_param(ctx, start, "dt_0").f;

    /* general scaling factor */
    if (pj_param(ctx, start, "tk_0").i)
        PIN->k0 = pj_param(ctx, start, "dk_0").f;
    else if (pj_param(ctx, start, "tk").i)
        PIN->k0 = pj_param(ctx, start, "dk").f;
    else
        PIN->k0 = 1.;
    if (PIN->k0 <= 0.)
        return pj_default_destructor (PIN, PJD_ERR_K_LESS_THAN_ZERO);

    /* set units */
    s = 0;
    if ((name = pj_param(ctx, start, "sunits").s) != NULL) {
        for (i = 0; (s = pj_units[i].id) && strcmp(name, s) ; ++i) ;
        if (!s)
            return pj_default_destructor (PIN, PJD_ERR_UNKNOW_UNIT_ID);
        s = pj_units[i].to_meter;
    }
    if (s || (s = pj_param(ctx, start, "sto_meter").s)) {
        PIN->to_meter = pj_strtod(s, &s);
        if (*s == '/') /* ratio number */
            PIN->to_meter /= pj_strtod(++s, 0);
        if (PIN->to_meter <= 0.0)
            return pj_default_destructor (PIN, PJD_ERR_UNIT_FACTOR_LESS_THAN_0);
        PIN->fr_meter = 1. / PIN->to_meter;
    } else
        PIN->to_meter = PIN->fr_meter = 1.;

    /* set vertical units */
    s = 0;
    if ((name = pj_param(ctx, start, "svunits").s) != NULL) {
        for (i = 0; (s = pj_units[i].id) && strcmp(name, s) ; ++i) ;
        if (!s)
            return pj_default_destructor (PIN, PJD_ERR_UNKNOW_UNIT_ID);
        s = pj_units[i].to_meter;
    }
    if (s || (s = pj_param(ctx, start, "svto_meter").s)) {
        PIN->vto_meter = pj_strtod(s, &s);
        if (*s == '/') /* ratio number */
            PIN->vto_meter /= pj_strtod(++s, 0);
        if (PIN->vto_meter <= 0.0)
            return pj_default_destructor (PIN, PJD_ERR_UNIT_FACTOR_LESS_THAN_0);
        PIN->vfr_meter = 1. / PIN->vto_meter;
    } else {
        PIN->vto_meter = PIN->to_meter;
        PIN->vfr_meter = PIN->fr_meter;
    }

    /* prime meridian */
    s = 0;
    if ((name = pj_param(ctx, start, "spm").s) != NULL) {
        const char *value = NULL;
        char *next_str = NULL;

        for (i = 0; pj_prime_meridians[i].id != NULL; ++i )
        {
            if( strcmp(name,pj_prime_meridians[i].id) == 0 )
            {
                value = pj_prime_meridians[i].defn;
                break;
            }
        }

        if( value == NULL
            && (dmstor_ctx(ctx,name,&next_str) != 0.0  || *name == '0')
            && *next_str == '\0' )
            value = name;

        if (!value)
            return pj_default_destructor (PIN, PJD_ERR_UNKNOWN_PRIME_MERIDIAN);
        PIN->from_greenwich = dmstor_ctx(ctx,value,NULL);
    }
    else
        PIN->from_greenwich = 0.0;

    /* Private object for the geodesic functions */
    PIN->geod = pj_calloc (1, sizeof (struct geod_geodesic));
    if (0==PIN->geod)
        return pj_default_destructor (PIN, ENOMEM);
    geod_init(PIN->geod, PIN->a,  (1 - sqrt (1 - PIN->es)));

    /* projection specific initialization */
    PIN = proj(PIN);
    if ((0==PIN) || ctx->last_errno)
    {
        pj_free(PIN);
        return 0;
    }
    return PIN;
}

/************************************************************************/
/*                              pj_free()                               */
/*                                                                      */
/*      This is the application callable entry point for destroying     */
/*      a projection definition.  It does work generic to all           */
/*      projection types, and then calls the projection specific        */
/*      free function (P->pfree()) to do local work.  In most cases     */
/*      P->pfree()==pj_default_destructor.                              */
/************************************************************************/

void pj_free(PJ *P) {
    if (0==P)
        return;
    /* free projection parameters - all the hard work is done by */
    /* pj_default_destructor (in pj_malloc.c), which is supposed */
    /* to be called as the last step of the local destructor     */
    /* pointed to by P->destructor. In most cases,               */
    /* pj_default_destructor actually *is* what is pointed to    */
    P->destructor (P, 0);
}
