/******************************************************************************
 * Project:  PROJ.4
 * Purpose:  Internal plumbing for the PROJ.4 library.
 *
 * Author:   Thomas Knudsen, <thokn@sdfe.dk>
 *
 ******************************************************************************
 * Copyright (c) 2016, 2017, Thomas Knudsen / SDFE
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
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO COORD SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *****************************************************************************/

#ifdef _MSC_VER
#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif
#endif
#include <math.h>   /* For M_PI */

#include <proj.h>

#ifdef PROJECTS_H
#error proj_internal.h must be included before projects.h
#endif
#ifdef PROJ_API_H
#error proj_internal.h must be included before proj_api.h
#endif

#ifndef PROJ_INTERNAL_H
#define PROJ_INTERNAL_H
#ifdef __cplusplus
extern "C" {
#endif



#ifndef PJ_TODEG
#define PJ_TODEG(rad)  ((rad)*180.0/M_PI)
#endif
#ifndef PJ_TORAD
#define PJ_TORAD(deg)  ((deg)*M_PI/180.0)
#endif



PJ_COORD proj_coord_error (void);
PJ_OBS   proj_obs_error (void);
#ifndef PJ_INTERNAL_C
extern const PJ_COORD proj_coord_null;
extern const PJ_OBS   proj_obs_null;
#endif
/* Part of MSVC workaround: Make proj_*_null look function-like for symmetry with proj_*_error */
#define proj_coord_null(x) proj_coord_null
#define proj_obs_null(x) proj_obs_null


void proj_context_errno_set (PJ_CONTEXT *ctx, int err);
void proj_context_set (PJ *P, PJ_CONTEXT *ctx);
void proj_context_inherit (PJ *parent, PJ *child);


PJ_OBS pj_fwdobs (PJ_OBS obs, PJ *P);
PJ_OBS pj_invobs (PJ_OBS obs, PJ *P);
PJ_COORD pj_fwdcoord (PJ_COORD coo, PJ *P);
PJ_COORD pj_invcoord (PJ_COORD coo, PJ *P);



/* High level functionality for handling thread contexts */
enum proj_log_level {
    PJ_LOG_NONE  = 0,
    PJ_LOG_ERROR = 1,
    PJ_LOG_DEBUG = 2,
    PJ_LOG_TRACE = 3,
    PJ_LOG_TELL  = 4,
    PJ_LOG_DEBUG_MAJOR = 2, /* for proj_api.h compatibility */
    PJ_LOG_DEBUG_MINOR = 3  /* for proj_api.h compatibility */
};

/* Set logging level 0-3. Higher number means more debug info. 0 turns it off */
enum proj_log_level proj_log_level (PJ_CONTEXT *ctx, enum proj_log_level log_level);
typedef void (*PJ_LOG_FUNCTION)(void *, int, const char *);

void proj_log_error (PJ *P, const char *fmt, ...);
void proj_log_debug (PJ *P, const char *fmt, ...);
void proj_log_trace (PJ *P, const char *fmt, ...);
/*void proj_log_func (PJ *P, void *app_data, void (*log)(void *, int, const char *));*/
void proj_log_func (PJ_CONTEXT *ctx, void *app_data, PJ_LOG_FUNCTION log);


/* Lowest level: Minimum support for fileapi */
void proj_fileapi_set (PJ *P, void *fileapi);

const char **proj_get_searchpath(void);
int    proj_get_path_count(void);

size_t pj_strlcpy(char *dst, const char *src, size_t siz);

#ifdef __cplusplus
}
#endif

#endif /* ndef PROJ_INTERNAL_H */
