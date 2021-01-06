/******************************************************************************
 * Project:  PROJ.4
 * Purpose:  Implementation of pj_log() function.
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

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "proj.h"
#include "proj_internal.h"

/************************************************************************/
/*                          pj_stderr_logger()                          */
/************************************************************************/

void pj_stderr_logger( void *app_data, int level, const char *msg )

{
    (void) app_data;
    (void) level;
    fprintf( stderr, "%s\n", msg );
}

/************************************************************************/
/*                               pj_vlog()                              */
/************************************************************************/
void pj_vlog( PJ_CONTEXT *ctx, int level, const char *fmt, va_list args );
/* Workhorse for the log functions - relates to pj_log as vsprintf relates to sprintf */
void pj_vlog( PJ_CONTEXT *ctx, int level, const char *fmt, va_list args )

{
    char *msg_buf;
    int debug_level = ctx->debug_level;
    int shutup_unless_errno_set = debug_level < 0;

    /* For negative debug levels, we first start logging when errno is set */
    if (ctx->last_errno==0 && shutup_unless_errno_set)
        return;

    if (debug_level < 0)
        debug_level = -debug_level;

    if( level > debug_level )
        return;

    msg_buf = (char *) malloc(100000);
    if( msg_buf == nullptr )
        return;

    /* we should use vsnprintf where available once we add configure detect.*/
    vsprintf( msg_buf, fmt, args );

    ctx->logger( ctx->logger_app_data, level, msg_buf );

    free( msg_buf );
}


/************************************************************************/
/*                               pj_log()                               */
/************************************************************************/

void pj_log( PJ_CONTEXT *ctx, int level, const char *fmt, ... )

{
    va_list args;

    if( level > ctx->debug_level )
        return;

    va_start( args, fmt );
    pj_vlog( ctx, level, fmt, args );
    va_end( args );
}



/***************************************************************************************/
PJ_LOG_LEVEL proj_log_level (PJ_CONTEXT *ctx, PJ_LOG_LEVEL log_level) {
/****************************************************************************************
   Set logging level 0-3. Higher number means more debug info. 0 turns it off
****************************************************************************************/
    PJ_LOG_LEVEL previous;
    if (nullptr==ctx)
        ctx = pj_get_default_ctx();
    if (nullptr==ctx)
        return PJ_LOG_TELL;
    previous = static_cast<PJ_LOG_LEVEL>(abs (ctx->debug_level));
    if (PJ_LOG_TELL==log_level)
        return previous;
    ctx->debug_level = log_level;
    return previous;
}

/*****************************************************************************/
static std::string add_short_name_prefix(const PJ* P, const char* fmt)
/*****************************************************************************/
{
    if( P->short_name == nullptr )
        return fmt;
    std::string ret(P->short_name);
    ret += ": ";
    ret += fmt;
    return ret;
}

/*****************************************************************************/
void proj_log_error (const PJ *P, const char *fmt, ...) {
/******************************************************************************
   For reporting the most severe events.
******************************************************************************/
    va_list args;
    va_start( args, fmt );
    pj_vlog (pj_get_ctx ((PJ*)P), PJ_LOG_ERROR , add_short_name_prefix(P, fmt).c_str(), args);
    va_end( args );
}


/*****************************************************************************/
void proj_log_debug (PJ *P, const char *fmt, ...) {
/******************************************************************************
   For reporting debugging information.
******************************************************************************/
    va_list args;
    va_start( args, fmt );
    pj_vlog (pj_get_ctx (P), PJ_LOG_DEBUG , add_short_name_prefix(P, fmt).c_str(), args);
    va_end( args );
}

/*****************************************************************************/
void proj_context_log_debug (PJ_CONTEXT *ctx, const char *fmt, ...) {
/******************************************************************************
   For reporting debugging information.
******************************************************************************/
    va_list args;
    va_start( args, fmt );
    pj_vlog (ctx, PJ_LOG_DEBUG , fmt, args);
    va_end( args );
}

/*****************************************************************************/
void proj_log_trace (PJ *P, const char *fmt, ...) {
/******************************************************************************
   For reporting embarrassingly detailed debugging information.
******************************************************************************/
    va_list args;
    va_start( args, fmt );
    pj_vlog (pj_get_ctx (P), PJ_LOG_TRACE , add_short_name_prefix(P, fmt).c_str(), args);
    va_end( args );
}


/*****************************************************************************/
void proj_log_func (PJ_CONTEXT *ctx, void *app_data, PJ_LOG_FUNCTION logf) {
/******************************************************************************
    Put a new logging function into P's context. The opaque object app_data is
    passed as first arg at each call to the logger
******************************************************************************/
    if (nullptr==ctx)
        ctx = pj_get_default_ctx ();
    ctx->logger_app_data = app_data;
    if (nullptr!=logf)
        ctx->logger = logf;
}
