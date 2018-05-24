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

#include "projects.h"

static projCtx_t default_context;
static volatile int       default_context_initialized = 0;

/************************************************************************/
/*                             pj_get_ctx()                             */
/************************************************************************/

projCtx pj_get_ctx( projPJ pj )

{
    if (0==pj)
        return pj_get_default_ctx ();
    if (0==pj->ctx)
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
    if (pj==0)
        return;
    pj->ctx = ctx;
}

/************************************************************************/
/*                         pj_get_default_ctx()                         */
/************************************************************************/

projCtx pj_get_default_ctx()

{
    /* If already initialized, don't bother locking */
    if( default_context_initialized )
        return &default_context;

    pj_acquire_lock();

    /* Ask again, since it may have been initialized in another thread */
    if( !default_context_initialized )
    {
        default_context.last_errno = 0;
        default_context.debug_level = PJ_LOG_NONE;
        default_context.logger = pj_stderr_logger;
        default_context.app_data = NULL;
        default_context.fileapi = pj_get_default_fileapi();

        if( getenv("PROJ_DEBUG") != NULL )
        {
            if( atoi(getenv("PROJ_DEBUG")) >= -PJ_LOG_DEBUG_MINOR )
                default_context.debug_level = atoi(getenv("PROJ_DEBUG"));
            else
                default_context.debug_level = PJ_LOG_DEBUG_MINOR;
        }
        default_context_initialized = 1;
    }

    pj_release_lock();

    return &default_context;
}

/************************************************************************/
/*                            pj_ctx_alloc()                            */
/************************************************************************/

projCtx pj_ctx_alloc()

{
    projCtx ctx = (projCtx_t *) malloc(sizeof(projCtx_t));
    if (0==ctx)
        return 0;
    memcpy( ctx, pj_get_default_ctx(), sizeof(projCtx_t) );
    ctx->last_errno = 0;

    return ctx;
}

/************************************************************************/
/*                            pj_ctx_free()                             */
/************************************************************************/

void pj_ctx_free( projCtx ctx )

{
    pj_dealloc( ctx );
}

/************************************************************************/
/*                          pj_ctx_get_errno()                          */
/************************************************************************/

int pj_ctx_get_errno( projCtx ctx )

{
    if (0==ctx)
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
    if (0==ctx)
        return;
    ctx->debug_level = new_debug;
}

/************************************************************************/
/*                         pj_ctx_set_logger()                          */
/************************************************************************/

void pj_ctx_set_logger( projCtx ctx, void (*new_logger)(void*,int,const char*) )

{
    if (0==ctx)
        return;
    ctx->logger = new_logger;
}

/************************************************************************/
/*                        pj_ctx_set_app_data()                         */
/************************************************************************/

void pj_ctx_set_app_data( projCtx ctx, void *new_app_data )

{
    if (0==ctx)
        return;
    ctx->app_data = new_app_data;
}

/************************************************************************/
/*                        pj_ctx_get_app_data()                         */
/************************************************************************/

void *pj_ctx_get_app_data( projCtx ctx )

{
    if (0==ctx)
        return 0;
    return ctx->app_data;
}

/************************************************************************/
/*                         pj_ctx_set_fileapi()                         */
/************************************************************************/

void pj_ctx_set_fileapi( projCtx ctx, projFileAPI *fileapi )

{
    if (0==ctx)
        return;
    ctx->fileapi = fileapi;
}

/************************************************************************/
/*                         pj_ctx_get_fileapi()                         */
/************************************************************************/

projFileAPI *pj_ctx_get_fileapi( projCtx ctx )

{
    if (0==ctx)
        return 0;
    return ctx->fileapi;
}
