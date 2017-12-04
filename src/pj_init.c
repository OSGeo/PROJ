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
#include <proj.h>
#include "proj_internal.h"
#include "projects.h"

#if 0
static paralist *string_to_list (PJ_CONTEXT ctx, paralist **start, char *definition) {
    char sword[ID_TAG_MAX + 2];
    int len;
    int in_target = 0;
    const char *next_char = NULL;


    if(found_def)
        *found_def = 0;

    len = (int)strlen(name);
    *sword = 't';

    if (0==next_char)
        return 0;

        /* Her kører vi med output fra shrink - alt ovenfor slettes */
            const char *start_of_word = next_char;
            int word_len = 0;

            /* capture parameter */
            while ( *next_char && !isspace(*next_char) )
            {
                next_char++;
                word_len++;
            }

            strncpy(sword+1, start_of_word, word_len);
            sword[word_len+1] = '\0';

            /* do not override existing parameter value of same name */
            if (!pj_param(ctx, *start, sword).i) {

                /* don't default ellipse if datum, ellps or any earth model information is set */
                if (0==strncmp(sword,"tellps=", 7)) {
                    int n = 0;

                    n += pj_param(ctx, *start, "tdatum").i;
                    n += pj_param(ctx, *start, "tellps").i;
                    n += pj_param(ctx, *start, "ta").i;
                    n += pj_param(ctx, *start, "tb").i;
                    n += pj_param(ctx, *start, "trf").i;
                    n += pj_param(ctx, *start, "tf").i;
                    n += pj_param(ctx, *start, "te").i;
                    n += pj_param(ctx, *start, "tes").i;

                    if (0==n)
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
#endif


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


/**************************************************************************************/
static paralist *string_to_list (PJ_CONTEXT ctx, paralist **start, char *definition) {
/***************************************************************************************
    Convert a string (presumably originating from get_init_string) to a paralist.
***************************************************************************************/
    char *c = definition;
    paralist *first = 0, *next;

    while (*c) {
        /* Find start of next substring */
        while (isspace (*c))
            c++;

        /* Keep a handle to the start of the list, so we have something to return */
        if (0==first)
            first = next = next->next = pj_mkparam_ws (c);
        else
            next = next->next = pj_mkparam_ws (c);

        /* And skip to the end of the substring */
        while ((!isspace(*c)) && 0!=*c)
            c++;
    }

    /* Terminate list and return */
    next->next = next;
    return first;
}


/**************************************************************************************/
static char *get_init_string (PJ_CONTEXT ctx, char *name) {
/***************************************************************************************
    Read a section of an init file. Return its contents as a plain character string.
***************************************************************************************/
    const size_t max_line_length = 1000;
    size_t current_buffer_size = 5 * (max_line_length + 1);
    char fname[MAX_PATH_FILENAME+ID_TAG_MAX+3], *section, *file;
    char *buffer;
    char line[max_line_length + 1];
    PAFile fid;

    /* support "init=file:section", "+init=file:section", and "file:section" format */
    file = strstr (name, "init=");
    if (0==file)
        file = name;
    else
        file += 5;
    if (MAX_PATH_FILENAME + ID_TAG_MAX + 2 < strlen (file))
        return 0;
    strncpy(fname, file, sizeof(fname)-2);
    fname[sizeof(fname)-2] = '\0';

    /* Locate the name of the section we search for */
    section = strrchr(fname, ':');
    if (0==section) {
        proj_errno (ctx, PJD_ERR_NO_COLON_IN_INIT_STRING);
        return 0;
    }

    fid = pj_open_lib (ctx, fname, "rt");
    if (0==fid) {
        proj_errno (ctx, PJD_ERR_NO_OPTION_IN_INIT_FILE);
        return 0;
    }

    /* search for section in init file */
    for (;;) {

        /* End of file? */
        if (0==pj_ctx_fgets (ctx, line, max_line_length, fid)) {
            pj_dealloc (buffer);
            proj_errno (ctx, PJD_ERR_NO_OPTION_IN_INIT_FILE);
            return 0;
        }

        /* At start of right section? */
        pj_chomp (line);
        if ('<'!=line[0])
            continue;
        if (0==strncmp (line + 1, section, strlen (section)))
            break;
    }

    /* We're at the first line of the right section - copy line to buffer */
    buffer = pj_calloc (current_buffer_size);
    if (0==buffer) {
        pj_ctx_fclose (fid);
        return 0;
    }

    /* Skip the "<section>" indicator, and copy the rest of the line over */
    strcpy (buffer, line + strlen (section) + 2);

    /* Copy the remaining lines of the section to buffer */
    for (;;) {
        char *end_i_cator = 0;
        size_t next_length, buffer_length;

        /* Did the section end somewhere in the most recently read line? */
        end_i_cator = strchr (buffer, '<');
        if (end_i_cator) {
            *end_i_cator = 0;
            break;
        }

        /* End of file? - done! */
        if (0==pj_ctx_fgets (ctx, line, max_line_length, fid))
            break;

        /* Otherwise, handle the line. It MAY be the start of the next section, */
        /* but that will be handled at the start of next trip through the loop  */
        buffer_length = strlen (buffer)
        next_length = strlen (line) + buffer_length + 1;
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

    pj_ctx_fclose (fid);
    return pj_shrink (buffer);
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
    int err;
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
        return pj_default_destructor (PIN, proj_errno(PIN));

    if (PIN->need_ellps) {
        int ret = pj_ellipsoid (PIN);
        if (0 != ret) {
            pj_log (ctx, PJ_LOG_DEBUG_MINOR, "pj_init_ctx: Must specify ellipsoid or sphere");
            return pj_default_destructor (PIN, proj_errno(PIN));
        }
        PIN->a_orig = PIN->a;
        PIN->es_orig = PIN->es;
        if (pj_calc_ellipsoid_params (PIN, PIN->a, PIN->es))
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
        double factor;
        int ratio = 0;

        /* ratio number? */
        if (*s == '/') {
            ratio = 1;
            s++;
        }

        factor = pj_strtod(s, &s);
        if ((factor <= 0.0) || (1/factor==0))
            return pj_default_destructor (PIN, PJD_ERR_UNIT_FACTOR_LESS_THAN_0);

        PIN->to_meter = ratio?  1 / factor: factor;
        PIN->fr_meter = 1 / PIN->to_meter;

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
    err = proj_errno_reset (PIN);
    PIN = proj(PIN);
    if (proj_errno (PIN)) {
        pj_free(PIN);
        return 0;
    }
    proj_errno_restore (PIN, err);
    return PIN;
}

/************************************************************************/
/*                              pj_free()                               */
/*                                                                      */
/*      This is the application callable entry point for destroying     */
/*      a projection definition.  It does work generic to all           */
/*      projection types, and then calls the projection specific        */
/*      free function, P->destructor(), to do local work.               */
/*      In most cases P->destructor()==pj_default_destructor.           */
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
