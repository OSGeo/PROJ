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

#include <ctype.h>
#include <errno.h>
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "geodesic.h"
#include "proj.h"
#include "proj_internal.h"
#include "proj_math.h"
#include "projects.h"



/**************************************************************************************/
static paralist *string_to_paralist (PJ_CONTEXT *ctx, char *definition) {
/***************************************************************************************
    Convert a string (presumably originating from get_init_string) to a paralist.
***************************************************************************************/
    char *c = definition;
    paralist *first = 0, *next = 0;

    while (*c) {
        /* Find start of next substring */
        while (isspace (*c))
            c++;

        /* Keep a handle to the start of the list, so we have something to return */
        if (0==first)
            first = next = pj_mkparam_ws (c);
        else
            next = next->next = pj_mkparam_ws (c);
        if (0==next)
            return pj_dealloc_params (ctx, first, ENOMEM);

        /* And skip to the end of the substring */
        while ((!isspace(*c)) && 0!=*c)
            c++;
    }

    if( next == 0 )
        return 0;

    /* Terminate list and return */
    next->next = 0;
    return first;
}



/**************************************************************************************/
static char *get_init_string (PJ_CONTEXT *ctx, char *name) {
/***************************************************************************************
    Read a section of an init file. Return its contents as a plain character string.
    It is the duty of the caller to free the memory allocated for the string.
***************************************************************************************/
#define MAX_LINE_LENGTH 1000
    size_t current_buffer_size = 5 * (MAX_LINE_LENGTH + 1);
    char *fname, *section, *key;
    char *buffer = 0;
    char *line = 0;
    PAFile fid;
    size_t n;


    line = pj_malloc (MAX_LINE_LENGTH + 1);
    if (0==line)
        return 0;

    fname = pj_malloc (MAX_PATH_FILENAME+ID_TAG_MAX+3);
    if (0==fname) {
        pj_dealloc (line);
        return 0;
    }

    /* Support "init=file:section", "+init=file:section", and "file:section" format */
    key = strstr (name, "init=");
    if (0==key)
        key = name;
    else
        key += 5;
    if (MAX_PATH_FILENAME + ID_TAG_MAX + 2 < strlen (key)) {
        pj_dealloc (fname);
        pj_dealloc (line);
        return 0;
    }
    memmove (fname, key, strlen (key) + 1);

    /* Locate the name of the section we search for */
    section = strrchr(fname, ':');
    if (0==section) {
        proj_context_errno_set (ctx, PJD_ERR_NO_COLON_IN_INIT_STRING);
        pj_dealloc (fname);
        pj_dealloc (line);
        return 0;
    }
    *section = 0;
    section++;
    n = strlen (section);
    pj_log (ctx, PJ_LOG_TRACE,
            "get_init_string: searching for section [%s] in init file [%s]",
            section, fname);

    fid = pj_open_lib (ctx, fname, "rt");
    if (0==fid) {
        pj_dealloc (fname);
        pj_dealloc (line);
        proj_context_errno_set (ctx, PJD_ERR_NO_OPTION_IN_INIT_FILE);
        return 0;
    }

    /* Search for section in init file */
    for (;;) {

        /* End of file? */
        if (0==pj_ctx_fgets (ctx, line, MAX_LINE_LENGTH, fid)) {
            pj_dealloc (buffer);
            pj_dealloc (fname);
            pj_dealloc (line);
            pj_ctx_fclose (ctx, fid);
            proj_context_errno_set (ctx, PJD_ERR_NO_OPTION_IN_INIT_FILE);
            return 0;
        }

        /* At start of right section? */
        pj_chomp (line);
        if ('<'!=line[0])
            continue;
        if (strlen (line) < n + 2)
            continue;
        if (line[n + 1] != '>')
            continue;
        if (0==strncmp (line + 1, section, n))
            break;
    }

    /* We're at the first line of the right section - copy line to buffer */
    buffer = pj_malloc (current_buffer_size);
    if (0==buffer) {
        pj_dealloc (fname);
        pj_dealloc (line);
        pj_ctx_fclose (ctx, fid);
        return 0;
    }

    /* Skip the "<section>" indicator, and copy the rest of the line over */
    strcpy (buffer, line + strlen (section) + 2);

    /* Copy the remaining lines of the section to buffer */
    for (;;) {
        char *end_i_cator;
        size_t next_length, buffer_length;

        /* Did the section end somewhere in the most recently read line? */
        end_i_cator = strchr (buffer, '<');
        if (end_i_cator) {
            *end_i_cator = 0;
            break;
        }

        /* End of file? - done! */
        if (0==pj_ctx_fgets (ctx, line, MAX_LINE_LENGTH, fid))
            break;

        /* Otherwise, handle the line. It MAY be the start of the next section, */
        /* but that will be handled at the start of next trip through the loop  */
        buffer_length = strlen (buffer);
        pj_chomp (line);   /* Remove '#' style comments */
        next_length = strlen (line) + buffer_length + 2;
        if (next_length > current_buffer_size) {
            char *b = pj_malloc (2 * current_buffer_size);
            if (0==b) {
                pj_dealloc (buffer);
                buffer = 0;
                break;
            }
            strcpy (b, buffer);
            current_buffer_size *= 2;
            pj_dealloc (buffer);
            buffer = b;
        }
        buffer[buffer_length] = ' ';
        strcpy (buffer + buffer_length + 1, line);
    }

    pj_ctx_fclose (ctx, fid);
    pj_dealloc (fname);
    pj_dealloc (line);
    if (0==buffer)
        return 0;
    pj_shrink (buffer);
    pj_log (ctx, PJ_LOG_TRACE, "key=%s, value: [%s]", key, buffer);
    return buffer;
}



/************************************************************************/
static paralist *get_init(PJ_CONTEXT *ctx, char *key) {
/*************************************************************************
Expand key from buffer or (if not in buffer) from init file
*************************************************************************/
    char *xkey, *definition;
    paralist *init_items = 0;

    /* support "init=file:section", "+init=file:section", and "file:section" format */
    xkey = strstr (key, "init=");
    if (0==xkey)
        xkey = key;
    else
        xkey += 5;
    pj_log (ctx, PJ_LOG_TRACE, "get_init: searching cache for key: [%s]", xkey);

    /* Is file/key pair already in cache? */
    init_items = pj_search_initcache (xkey);
    if (init_items)
        return init_items;

    /* If not, we must read it from file */
    pj_log (ctx, PJ_LOG_TRACE,
            "get_init: searching on in init files for [%s]", xkey);
    definition = get_init_string (ctx, xkey);
    if (0==definition)
        return 0;
    init_items = string_to_paralist (ctx, definition);
    if (init_items)
        pj_log (ctx, PJ_LOG_TRACE, "get_init: got [%s], paralist[0,1]: [%s,%s]",
                definition,
                init_items->param,
                init_items->next ? init_items->next->param : "(empty)");
    pj_dealloc (definition);
    if (0==init_items)
        return 0;

    /* We found it in file - now insert into the cache, before returning */
    pj_insert_initcache (xkey, init_items);
    return init_items;
}



static paralist *append_defaults_to_paralist (PJ_CONTEXT *ctx, paralist *start, char *key) {
    paralist *defaults, *last = 0;
    char keystring[ID_TAG_MAX + 20];
    paralist *next, *proj;
    int err;

    if (0==start)
        return 0;

    if (strlen(key) > ID_TAG_MAX)
        return 0;

    /* Set defaults, unless inhibited (either explicitly through a "no_defs" token */
    /* or implicitly, because we are initializing a pipeline) */
    if (pj_param_exists (start, "no_defs"))
        return start;
    proj = pj_param_exists (start, "proj");
    if (0==proj)
        return start;
    if (strlen (proj->param) < 6)
        return start;
    if (0==strcmp ("pipeline", proj->param + 5))
        return start;

    err = pj_ctx_get_errno (ctx);
    pj_ctx_set_errno (ctx, 0);

    /* Locate end of start-list */
    for (last = start;  last->next;  last = last->next);

    strcpy (keystring, "proj_def.dat:");
    strcat (keystring, key);
    defaults = get_init (ctx, keystring);

    /* Defaults are optional - so we don't care if we cannot open the file */
    pj_ctx_set_errno (ctx, err);

    if (!defaults)
        return last;

    /* Loop over all default items */
    for (next = defaults;  next;  next = next->next) {

        /* Don't override existing parameter value of same name */
        if (pj_param_exists (start, next->param))
            continue;

        /* Don't default ellipse if datum, ellps or any ellipsoid information is set */
        if (0==strncmp(next->param,"ellps=", 6)) {
            if  (pj_param_exists (start, "datum"))  continue;
            if  (pj_param_exists (start, "ellps"))  continue;
            if  (pj_param_exists (start, "a"))      continue;
            if  (pj_param_exists (start, "b"))      continue;
            if  (pj_param_exists (start, "rf"))     continue;
            if  (pj_param_exists (start, "f"))      continue;
            if  (pj_param_exists (start, "e"))      continue;
            if  (pj_param_exists (start, "es"))     continue;
        }

        /* If we're here, it's OK to append the current default item */
        last = last->next = pj_mkparam(next->param);
    }
    last->next = 0;

    pj_dealloc_params (ctx, defaults, 0);
    return last;
}

/*****************************************************************************/
paralist *pj_expand_init(PJ_CONTEXT *ctx, paralist *init) {
/******************************************************************************
Append expansion of <key> to the paralist <init>. The expansion is appended,
rather than inserted at <init>'s place, since <init> may contain
overrides to the expansion. These must take precedence, and hence come first
in the expanded list.

Consider e.g. the key 'foo:bar' which (hypothetically) expands to 'proj=utm
zone=32 ellps=GRS80', i.e. a UTM projection on the GRS80 ellipsoid.

The expression 'init=foo:bar ellps=intl' will then expand to:

           'init=foo:bar ellps=intl proj=utm zone=32 ellps=GRS80',

where 'ellps=intl' precedes 'ellps=GRS80', and hence takes precedence,
turning the expansion into an UTM projection on the Hayford ellipsoid.

Note that 'init=foo:bar' stays in the list. It is ignored after expansion.

******************************************************************************/
    paralist *last;
    paralist *expn;

    /* Nowhere to start? */
    if (0==init)
        return 0;

    expn = get_init(ctx, init->param);

    /* Nothing in expansion? */
    if (0==expn)
        return 0;

    /* Locate  the end of the list */
    for (last = init;  last && last->next;  last = last->next);

    /* Then append and return */
    last->next = expn;
    return init;
}



/************************************************************************/
/*                            pj_init_plus()                            */
/*                                                                      */
/*      Same as pj_init() except it takes one argument string with      */
/*      individual arguments preceded by '+', such as "+proj=utm        */
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


static PJ_CONSTRUCTOR locate_constructor (const char *name) {
    int i;
    char *s;
    for (i = 0; (s = pj_list[i].id) && strcmp(name, s) ; ++i) ;
    if (0==s)
        return 0;
    return (PJ_CONSTRUCTOR) pj_list[i].proj;
}


PJ *
pj_init_ctx(projCtx ctx, int argc, char **argv) {
    char *s, *name;
    PJ_CONSTRUCTOR proj;
    paralist *curr, *init, *start;
    int i;
    int err;
    PJ *PIN = 0;
    int n_pipelines = 0;
    int n_inits = 0;

    if (0==ctx)
        ctx = pj_get_default_ctx ();

    ctx->last_errno = 0;

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

    /* can't have nested pipelines directly */
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


    /* Only expand '+init's in non-pipeline operations. '+init's in pipelines are */
    /* expanded in the individual pipeline steps during pipeline initialization.  */
    /* Potentially this leads to many nested pipelines, which shouldn't be a      */
    /* problem when '+init's are expanded as late as possible.                    */
    init = pj_param_exists (start, "init");
    if (init && n_pipelines == 0) {
        init = pj_expand_init (ctx, init);
        if (!init)
            return pj_dealloc_params (ctx, start, PJD_ERR_NO_ARGS);
    }
    if (ctx->last_errno)
        return pj_dealloc_params (ctx, start, ctx->last_errno);

    /* Find projection selection */
    curr = pj_param_exists (start, "proj");
    if (0==curr)
        return pj_dealloc_params (ctx, start, PJD_ERR_PROJ_NOT_NAMED);
    name =  curr->param;
    if (strlen (name) < 6)
        return pj_dealloc_params (ctx, start, PJD_ERR_PROJ_NOT_NAMED);
    name += 5;

    proj = locate_constructor (name);
    if (0==proj)
        return pj_dealloc_params (ctx, start, PJD_ERR_UNKNOWN_PROJECTION_ID);


    /* Append general and projection specific defaults to the definition list */
    append_defaults_to_paralist (ctx, start, "general");
    append_defaults_to_paralist (ctx, start, name);


    /* Allocate projection structure */
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

    /* Set datum parameters. Similarly to +init parameters we want to expand    */
    /* +datum parameters as late as possible when dealing with pipelines.       */
    /* otherwise only the first occurrence of +datum will be expanded and that */
    if (n_pipelines == 0) {
        if (pj_datum_set(ctx, start, PIN))
            return pj_default_destructor (PIN, proj_errno(PIN));
    }

    err = pj_ellipsoid (PIN);

    if (err) {
        /* Didn't get an ellps, but doesn't need one: Get a free WGS84 */
        if (PIN->need_ellps) {
            pj_log (ctx, PJ_LOG_DEBUG_MINOR, "pj_init_ctx: Must specify ellipsoid or sphere");
            return pj_default_destructor (PIN, proj_errno(PIN));
        }
        else {
            if (PJD_ERR_MAJOR_AXIS_NOT_GIVEN==proj_errno (PIN))
                proj_errno_reset (PIN);
            PIN->f = 1.0/298.257223563;
            PIN->a_orig  = PIN->a  = 6378137.0;
            PIN->es_orig = PIN->es = PIN->f*(2-PIN->f);
        }
    }
    PIN->a_orig = PIN->a;
    PIN->es_orig = PIN->es;
    if (pj_calc_ellipsoid_params (PIN, PIN->a, PIN->es))
        return pj_default_destructor (PIN, PJD_ERR_ECCENTRICITY_IS_ONE);

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

    /* Set PIN->geoc coordinate system */
    PIN->geoc = (PIN->es != 0.0 && pj_param(ctx, start, "bgeoc").i);

    /* Over-ranging flag */
    PIN->over = pj_param(ctx, start, "bover").i;

    /* Vertical datum geoid grids */
    PIN->has_geoid_vgrids = pj_param(ctx, start, "tgeoidgrids").i;
    if( PIN->has_geoid_vgrids ) /* we need to mark it as used. */
        pj_param(ctx, start, "sgeoidgrids");

    /* Longitude center for wrapping */
    PIN->is_long_wrap_set = pj_param(ctx, start, "tlon_wrap").i;
    if (PIN->is_long_wrap_set) {
        PIN->long_wrap_center = pj_param(ctx, start, "rlon_wrap").f;
        /* Don't accept excessive values otherwise we might perform badly */
        /* when correcting longitudes around it */
        /* The test is written this way to error on long_wrap_center "=" NaN */
        if( !(fabs(PIN->long_wrap_center) < 10 * M_TWOPI) )
            return pj_default_destructor (PIN, PJD_ERR_LAT_OR_LON_EXCEED_LIMIT);
    }

    /* Axis orientation */
    if( (pj_param(ctx, start,"saxis").s) != NULL )
    {
        const char *axis_legal = "ewnsud";
        const char *axis_arg = pj_param(ctx, start,"saxis").s;
        if( strlen(axis_arg) != 3 )
            return pj_default_destructor (PIN, PJD_ERR_AXIS);

        if( strchr( axis_legal, axis_arg[0] ) == NULL
            || strchr( axis_legal, axis_arg[1] ) == NULL
            || strchr( axis_legal, axis_arg[2] ) == NULL)
            return pj_default_destructor (PIN, PJD_ERR_AXIS);

        /* TODO: it would be nice to validate we don't have on axis repeated */
        strcpy( PIN->axis, axis_arg );
    }

    /* Central meridian */
    PIN->lam0=pj_param(ctx, start, "rlon_0").f;

    /* Central latitude */
    PIN->phi0 = pj_param(ctx, start, "rlat_0").f;

    /* False easting and northing */
    PIN->x0 = pj_param(ctx, start, "dx_0").f;
    PIN->y0 = pj_param(ctx, start, "dy_0").f;
    PIN->z0 = pj_param(ctx, start, "dz_0").f;
    PIN->t0 = pj_param(ctx, start, "dt_0").f;

    /* General scaling factor */
    if (pj_param(ctx, start, "tk_0").i)
        PIN->k0 = pj_param(ctx, start, "dk_0").f;
    else if (pj_param(ctx, start, "tk").i)
        PIN->k0 = pj_param(ctx, start, "dk").f;
    else
        PIN->k0 = 1.;
    if (PIN->k0 <= 0.)
        return pj_default_destructor (PIN, PJD_ERR_K_LESS_THAN_ZERO);

    /* Set units */
    s = 0;
    if ((name = pj_param(ctx, start, "sunits").s) != NULL) {
        for (i = 0; (s = pj_units[i].id) && strcmp(name, s) ; ++i) ;
        if (!s)
            return pj_default_destructor (PIN, PJD_ERR_UNKNOWN_UNIT_ID);
        s = pj_units[i].to_meter;
    }
    if (s || (s = pj_param(ctx, start, "sto_meter").s)) {
        double factor;
        int ratio = 0;

        /* ratio number? */
        if (strlen (s) > 1 && s[0] == '1' && s[1]=='/') {
            ratio = 1;
            s += 2;
        }

        factor = pj_strtod(s, &s);
        if ((factor <= 0.0) || (1/factor==0))
            return pj_default_destructor (PIN, PJD_ERR_UNIT_FACTOR_LESS_THAN_0);

        PIN->to_meter = ratio?  1 / factor: factor;
        PIN->fr_meter = 1 / PIN->to_meter;

    } else
        PIN->to_meter = PIN->fr_meter = 1.;

    /* Set vertical units */
    s = 0;
    if ((name = pj_param(ctx, start, "svunits").s) != NULL) {
        for (i = 0; (s = pj_units[i].id) && strcmp(name, s) ; ++i) ;
        if (!s)
            return pj_default_destructor (PIN, PJD_ERR_UNKNOWN_UNIT_ID);
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

    /* Prime meridian */
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

    /* Projection specific initialization */
    err = proj_errno_reset (PIN);
    PIN = proj(PIN);
    if (proj_errno (PIN)) {
        pj_free(PIN);
        return 0;
    }
    proj_errno_restore (PIN, err);
    return PIN;
}
