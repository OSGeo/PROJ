/******************************************************************************
 * Project:  PROJ.4
 * Purpose:  Implementation of pj_open_lib(), and pj_set_finder().  These
 *           provide a standard interface for opening projections support
 *           data files.
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

#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "proj_internal.h"
#include "projects.h"

static const char *(*pj_finder)(const char *) = NULL;
static int path_count = 0;
static char **search_path = NULL;
static const char * proj_lib_name =
#ifdef PROJ_LIB
PROJ_LIB;
#else
0;
#endif

/************************************************************************/
/*                           pj_set_finder()                            */
/************************************************************************/

void pj_set_finder( const char *(*new_finder)(const char *) )

{
    pj_finder = new_finder;
}

/************************************************************************/
/*                         pj_set_searchpath()                          */
/*                                                                      */
/*      Path control for callers that can't practically provide         */
/*      pj_set_finder() style callbacks.  Call with (0,NULL) as args    */
/*      to clear the searchpath set.                                    */
/************************************************************************/

void pj_set_searchpath ( int count, const char **path )
{
    int i;

    if (path_count > 0 && search_path != NULL)
    {
        for (i = 0; i < path_count; i++)
        {
            pj_dalloc(search_path[i]);
        }
        pj_dalloc(search_path);
        path_count = 0;
        search_path = NULL;
    }

    if( count > 0 )
    {
        search_path = pj_malloc(sizeof *search_path * count);
        for (i = 0; i < count; i++)
        {
            search_path[i] = pj_malloc(strlen(path[i]) + 1);
            strcpy(search_path[i], path[i]);
        }
    }

    path_count = count;
}

/* just a couple of helper functions that lets other functions
   access the otherwise private search path */
const char * const *proj_get_searchpath(void) {
    return (const char * const *)search_path;
}

int proj_get_path_count(void) {
    return path_count;
}
/************************************************************************/
/*                          pj_open_lib_ex()                            */
/************************************************************************/

static PAFile
pj_open_lib_ex(projCtx ctx, const char *name, const char *mode,
               char* out_full_filename, size_t out_full_filename_size) {
    char fname[MAX_PATH_FILENAME+1];
    const char *sysname;
    PAFile fid;
    int n = 0;
    int i;
#ifdef WIN32
    static const char dir_chars[] = "/\\";
#else
    static const char dir_chars[] = "/";
#endif

    if( out_full_filename != NULL && out_full_filename_size > 0 )
        out_full_filename[0] = '\0';

    /* check if ~/name */
    if (*name == '~' && strchr(dir_chars,name[1]) )
        if ((sysname = getenv("HOME")) != NULL) {
            if( strlen(sysname) + 1 + strlen(name) + 1 > sizeof(fname) )
            {
                return NULL;
            }
            (void)strcpy(fname, sysname);
            fname[n = (int)strlen(fname)] = DIR_CHAR;
            fname[++n] = '\0';
            (void)strcpy(fname+n, name + 1);
            sysname = fname;
        } else
            return NULL;

    /* or fixed path: /name, ./name or ../name */
    else if (strchr(dir_chars,*name)
             || (*name == '.' && strchr(dir_chars,name[1])) 
             || (!strncmp(name, "..", 2) && strchr(dir_chars,name[2]))
             || (name[1] == ':' && strchr(dir_chars,name[2])) )
        sysname = name;

    /* or try to use application provided file finder */
    else if( pj_finder != NULL && pj_finder( name ) != NULL )
        sysname = pj_finder( name );

    /* or is environment PROJ_LIB defined */
    else if ((sysname = getenv("PROJ_LIB")) || (sysname = proj_lib_name)) {
        if( strlen(sysname) + 1 + strlen(name) + 1 > sizeof(fname) )
        {
            return NULL;
        }
        (void)strcpy(fname, sysname);
        fname[n = (int)strlen(fname)] = DIR_CHAR;
        fname[++n] = '\0';
        (void)strcpy(fname+n, name);
        sysname = fname;
    } else /* just try it bare bones */
        sysname = name;

    if ((fid = pj_ctx_fopen(ctx, sysname, mode)) != NULL)
    {
        if( out_full_filename != NULL && out_full_filename_size > 0 )
        {
            strncpy(out_full_filename, sysname, out_full_filename_size);
            out_full_filename[out_full_filename_size-1] = '\0';
        }
        errno = 0;
    }

    /* If none of those work and we have a search path, try it */
    if (!fid && path_count > 0)
    {
        for (i = 0; fid == NULL && i < path_count; i++)
        {
            if( strlen(search_path[i]) + 1 + strlen(name) + 1 <= sizeof(fname) )
            {
                sprintf(fname, "%s%c%s", search_path[i], DIR_CHAR, name);
                sysname = fname;
                fid = pj_ctx_fopen(ctx, sysname, mode);
            }
        }
        if (fid)
        {
            if( out_full_filename != NULL && out_full_filename_size > 0 )
            {
                strncpy(out_full_filename, sysname, out_full_filename_size);
                out_full_filename[out_full_filename_size-1] = '\0';
            }
            errno = 0;
        }
    }

    if( ctx->last_errno == 0 && errno != 0 )
        pj_ctx_set_errno( ctx, errno );

    pj_log( ctx, PJ_LOG_DEBUG_MAJOR, 
            "pj_open_lib(%s): call fopen(%s) - %s",
            name, sysname,
            fid == NULL ? "failed" : "succeeded" );

    return(fid);
}

/************************************************************************/
/*                            pj_open_lib()                             */
/************************************************************************/

PAFile
pj_open_lib(projCtx ctx, const char *name, const char *mode) {
    return pj_open_lib_ex(ctx, name, mode, NULL, 0);
}

/************************************************************************/
/*                           pj_find_file()                             */
/************************************************************************/

/** Returns the full filename corresponding to a proj resource file specified
 *  as a short filename.
 * 
 * @param ctx context.
 * @param short_filename short filename (e.g. egm96_15.gtx)
 * @param out_full_filename output buffer, of size out_full_filename_size, that
 *                          will receive the full filename on success.
 *                          Will be zero-terminated.
 * @param out_full_filename_size size of out_full_filename.
 * @return 1 if the file was found, 0 otherwise.
 */
int pj_find_file(projCtx ctx, const char *short_filename,
                 char* out_full_filename, size_t out_full_filename_size)
{
    PAFile f = pj_open_lib_ex(ctx, short_filename, "rb", out_full_filename,
                              out_full_filename_size);
    if( f != NULL )
    {
        pj_ctx_fclose(ctx, f);
        return 1;
    }
    return 0;
}
