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

#ifndef FROM_PROJ_CPP
#define FROM_PROJ_CPP
#endif

#include <assert.h>
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "proj/internal/internal.hpp"

#include "proj_internal.h"

static const char * proj_lib_name =
#ifdef PROJ_LIB
PROJ_LIB;
#else
nullptr;
#endif

/************************************************************************/
/*                           pj_set_finder()                            */
/************************************************************************/

void pj_set_finder( const char *(*new_finder)(const char *) )

{
    auto ctx = pj_get_default_ctx();
    if( ctx ) {
        ctx->file_finder_legacy = new_finder;
    }
}

/************************************************************************/
/*                   proj_context_set_file_finder()                     */
/************************************************************************/

/** \brief Assign a file finder callback to a context.
 * 
 * This callback will be used whenever PROJ must open one of its resource files
 * (proj.db database, grids, etc...)
 * 
 * The callback will be called with the context currently in use at the moment
 * where it is used (not necessarily the one provided during this call), and
 * with the provided user_data (which may be NULL).
 * The user_data must remain valid during the whole lifetime of the context.
 * 
 * A finder set on the default context will be inherited by contexts created
 * later.
 * 
 * @param ctx PROJ context, or NULL for the default context.
 * @param finder Finder callback. May be NULL
 * @param user_data User data provided to the finder callback. May be NULL.
 * 
 * @since PROJ 6.0
 */
void proj_context_set_file_finder(PJ_CONTEXT *ctx, proj_file_finder finder,
                                  void* user_data)
{
    if( !ctx )
        ctx = pj_get_default_ctx();
    if( !ctx )
        return;
    ctx->file_finder = finder;
    ctx->file_finder_user_data = user_data;
}

/************************************************************************/
/*                  proj_context_set_search_paths()                     */
/************************************************************************/


/** \brief Sets search paths.
 * 
 * Those search paths will be used whenever PROJ must open one of its resource files
 * (proj.db database, grids, etc...)
 *
 * If set on the default context, they will be inherited by contexts created
 * later.
 * 
 * @param ctx PROJ context, or NULL for the default context.
 * @param count_paths Number of paths. 0 if paths == NULL.
 * @param paths Paths. May be NULL.
 * 
 * @since PROJ 6.0
 */
void proj_context_set_search_paths(PJ_CONTEXT *ctx,
                                   int count_paths,
                                   const char* const* paths)
{
    if( !ctx )
        ctx = pj_get_default_ctx();
    if( !ctx )
        return;
    try {
        std::vector<std::string> vector_of_paths;
        for (int i = 0; i < count_paths; i++)
        {
            vector_of_paths.emplace_back(paths[i]);
        }
        ctx->set_search_paths(vector_of_paths);
    } catch( const std::exception& )
    {
    }
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
    proj_context_set_search_paths( nullptr, count, const_cast<const char* const*>(path) );
}

/************************************************************************/
/*                          pj_open_lib_ex()                            */
/************************************************************************/

static PAFile
pj_open_lib_ex(projCtx ctx, const char *name, const char *mode,
               char* out_full_filename, size_t out_full_filename_size) {
    try {
        std::string fname;
        const char *sysname = nullptr;
        PAFile fid = nullptr;
#ifdef WIN32
        static const char dir_chars[] = "/\\";
        const char dirSeparator = ';';
#else
        static const char dir_chars[] = "/";
        const char dirSeparator = ':';
#endif

        if( ctx == nullptr ) {
            ctx = pj_get_default_ctx();
        }

        if( out_full_filename != nullptr && out_full_filename_size > 0 )
            out_full_filename[0] = '\0';

        /* check if ~/name */
        if (*name == '~' && strchr(dir_chars,name[1]) )
            if ((sysname = getenv("HOME")) != nullptr) {
                fname = sysname;
                fname += DIR_CHAR;
                fname += name;
                sysname = fname.c_str();
            } else
                return nullptr;

        /* or fixed path: /name, ./name or ../name */
        else if (strchr(dir_chars,*name)
                || (*name == '.' && strchr(dir_chars,name[1])) 
                || (!strncmp(name, "..", 2) && strchr(dir_chars,name[2]))
                || (name[0] != '\0' && name[1] == ':' && strchr(dir_chars,name[2])) )
            sysname = name;

        /* or try to use application provided file finder */
        else if( ctx->file_finder != nullptr && (sysname = ctx->file_finder( ctx, name, ctx->file_finder_user_data )) != nullptr )
            ;

        else if( ctx->file_finder_legacy != nullptr && (sysname = ctx->file_finder_legacy( name )) != nullptr )
            ;

        /* The user has search paths set */
        else if( !ctx->search_paths.empty() ) {
            for( const auto& path: ctx->search_paths ) {
                try {
                    fname = path;
                    fname += DIR_CHAR;
                    fname += name;
                    sysname = fname.c_str();
                    fid = pj_ctx_fopen(ctx, sysname, mode);
                } catch( const std::exception& )
                {
                }
                if( fid )
                    break;
            }
        }
        /* if is environment PROJ_LIB defined */
        else if ((sysname = getenv("PROJ_LIB")) != nullptr) {
            auto paths = NS_PROJ::internal::split(std::string(sysname), dirSeparator);
            for( const auto& path: paths ) {
                fname = path;
                fname += DIR_CHAR;
                fname += name;
                sysname = fname.c_str();
                fid = pj_ctx_fopen(ctx, sysname, mode);
                if( fid )
                    break;
            }
        /* or hardcoded path */
        } else if ((sysname = proj_lib_name) != nullptr) {
            fname = sysname;
            fname += DIR_CHAR;
            fname += name;
            sysname = fname.c_str();
        /* just try it bare bones */
        } else {
            sysname = name;
        }

        assert(sysname); // to make Coverity Scan happy
        if ( fid != nullptr || (fid = pj_ctx_fopen(ctx, sysname, mode)) != nullptr)
        {
            if( out_full_filename != nullptr && out_full_filename_size > 0 )
            {
                strncpy(out_full_filename, sysname, out_full_filename_size);
                out_full_filename[out_full_filename_size-1] = '\0';
            }
            errno = 0;
        }

        if( ctx->last_errno == 0 && errno != 0 )
            pj_ctx_set_errno( ctx, errno );

        pj_log( ctx, PJ_LOG_DEBUG_MAJOR, 
                "pj_open_lib(%s): call fopen(%s) - %s",
                name, sysname,
                fid == nullptr ? "failed" : "succeeded" );

        return(fid);
    }
    catch( const std::exception& ) {

        pj_log( ctx, PJ_LOG_DEBUG_MAJOR, 
                "pj_open_lib(%s): out of memory",
                name );

        return nullptr;
    }
}

/************************************************************************/
/*                            pj_open_lib()                             */
/************************************************************************/

PAFile
pj_open_lib(projCtx ctx, const char *name, const char *mode) {
    return pj_open_lib_ex(ctx, name, mode, nullptr, 0);
}

/************************************************************************/
/*                           pj_find_file()                             */
/************************************************************************/

/** Returns the full filename corresponding to a proj resource file specified
 *  as a short filename.
 * 
 * @param ctx context.
 * @param short_filename short filename (e.g. egm96_15.gtx). Must not be NULL.
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
    if( f != nullptr )
    {
        pj_ctx_fclose(ctx, f);
        return 1;
    }
    return 0;
}
