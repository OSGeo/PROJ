/******************************************************************************
 * Project:  PROJ.4
 * Purpose:  Implementation of the projCtx thread context object.
 * Author:   Frank Warmerdam, warmerdam@pobox.com
 *
 ******************************************************************************
 * Copyright (c) 2010, Frank Warmerdam
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

#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include <new>

#include "proj_experimental.h"
#include "proj_internal.h"

/************************************************************************/
/*                             pj_get_ctx()                             */
/************************************************************************/

projCtx pj_get_ctx( projPJ pj )

{
    if (nullptr==pj)
        return pj_get_default_ctx ();
    if (nullptr==pj->ctx)
        return pj_get_default_ctx ();
    return pj->ctx;
}

/************************************************************************/
/*                             pj_set_ctx()                             */
/*                                                                      */
/*      Note we do not deallocate the old context!                      */
/************************************************************************/

void pj_set_ctx( projPJ pj, projCtx ctx )

{
    if (pj==nullptr)
        return;
    pj->ctx = ctx;
}

/************************************************************************/
/*                        proj_assign_context()                         */
/************************************************************************/

/** \brief Re-assign a context to a PJ* object.
 *
 * This may be useful if the PJ* has been created with a context that is
 * thread-specific, and is later used in another thread. In that case,
 * the user may want to assign another thread-specific context to the
 * object.
 */
void proj_assign_context( PJ* pj, PJ_CONTEXT* ctx )
{
    pj_set_ctx( pj, ctx );
}

/************************************************************************/
/*                          createDefault()                             */
/************************************************************************/

projCtx_t projCtx_t::createDefault()
{
    projCtx_t ctx;
    ctx.debug_level = PJ_LOG_NONE;
    ctx.logger = pj_stderr_logger;
    ctx.fileapi = pj_get_default_fileapi();

    if( getenv("PROJ_DEBUG") != nullptr )
    {
        if( atoi(getenv("PROJ_DEBUG")) >= -PJ_LOG_DEBUG_MINOR )
            ctx.debug_level = atoi(getenv("PROJ_DEBUG"));
        else
            ctx.debug_level = PJ_LOG_DEBUG_MINOR;
    }
    return ctx;
}

/************************************************************************/
/*                           set_search_paths()                         */
/************************************************************************/

void projCtx_t::set_search_paths(const std::vector<std::string>& search_paths_in )
{
    search_paths = search_paths_in;
    delete[] c_compat_paths;
    c_compat_paths = nullptr;
    if( !search_paths.empty() ) {
        c_compat_paths = new const char*[search_paths.size()];
        for( size_t i = 0; i < search_paths.size(); ++i ) {
            c_compat_paths[i] = search_paths[i].c_str();
        }
    }
}

/************************************************************************/
/*                  projCtx_t(const projCtx_t& other)                   */
/************************************************************************/

projCtx_t::projCtx_t(const projCtx_t& other)
{
    debug_level = other.debug_level;
    logger = other.logger;
    logger_app_data = other.logger_app_data;
    fileapi = other.fileapi;
    epsg_file_exists = other.epsg_file_exists;
    set_search_paths(other.search_paths);
    file_finder = other.file_finder;
    file_finder_legacy = other.file_finder_legacy;
    file_finder_user_data = other.file_finder_user_data;
}

/************************************************************************/
/*                         pj_get_default_ctx()                         */
/************************************************************************/

projCtx pj_get_default_ctx()

{
    // C++11 rules guarantee a thread-safe instantiation.
    static projCtx_t default_context(projCtx_t::createDefault());
    return &default_context;
}

/************************************************************************/
/*                            ~projCtx_t()                              */
/************************************************************************/

projCtx_t::~projCtx_t()
{
    delete[] c_compat_paths;
    proj_context_delete_cpp_context(cpp_context);
}

/************************************************************************/
/*                            pj_ctx_alloc()                            */
/************************************************************************/

projCtx pj_ctx_alloc()

{
    return new (std::nothrow) projCtx_t(*pj_get_default_ctx());
}

/************************************************************************/
/*                            pj_ctx_free()                             */
/************************************************************************/

void pj_ctx_free( projCtx ctx )

{
    delete ctx;
}

/************************************************************************/
/*                          pj_ctx_get_errno()                          */
/************************************************************************/

int pj_ctx_get_errno( projCtx ctx )

{
    if (nullptr==ctx)
        return pj_get_default_ctx ()->last_errno;
    return ctx->last_errno;
}

/************************************************************************/
/*                          pj_ctx_set_errno()                          */
/*                                                                      */
/*                      Also sets the global errno                      */
/************************************************************************/

void pj_ctx_set_errno( projCtx ctx, int new_errno )

{
    ctx->last_errno = new_errno;
    if( new_errno == 0 )
        return;
    errno = new_errno;
    pj_errno = new_errno;
}

/************************************************************************/
/*                          pj_ctx_set_debug()                          */
/************************************************************************/

void pj_ctx_set_debug( projCtx ctx, int new_debug )

{
    if (nullptr==ctx)
        return;
    ctx->debug_level = new_debug;
}

/************************************************************************/
/*                         pj_ctx_set_logger()                          */
/************************************************************************/

void pj_ctx_set_logger( projCtx ctx, void (*new_logger)(void*,int,const char*) )

{
    if (nullptr==ctx)
        return;
    ctx->logger = new_logger;
}

/************************************************************************/
/*                        pj_ctx_set_app_data()                         */
/************************************************************************/

void pj_ctx_set_app_data( projCtx ctx, void *new_app_data )

{
    if (nullptr==ctx)
        return;
    ctx->logger_app_data = new_app_data;
}

/************************************************************************/
/*                        pj_ctx_get_app_data()                         */
/************************************************************************/

void *pj_ctx_get_app_data( projCtx ctx )

{
    if (nullptr==ctx)
        return nullptr;
    return ctx->logger_app_data;
}

/************************************************************************/
/*                         pj_ctx_set_fileapi()                         */
/************************************************************************/

void pj_ctx_set_fileapi( projCtx ctx, projFileAPI *fileapi )

{
    if (nullptr==ctx)
        return;
    ctx->fileapi = fileapi;
}

/************************************************************************/
/*                         pj_ctx_get_fileapi()                         */
/************************************************************************/

projFileAPI *pj_ctx_get_fileapi( projCtx ctx )

{
    if (nullptr==ctx)
        return nullptr;
    return ctx->fileapi;
}

