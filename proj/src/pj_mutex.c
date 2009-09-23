/******************************************************************************
 * $Id: pj_transform.c 1504 2009-01-06 02:11:57Z warmerdam $
 *
 * Project:  PROJ.4
 * Purpose:  Mutex (thread lock) functions.
 * Author:   Frank Warmerdam, warmerdam@pobox.com
 *
 ******************************************************************************
 * Copyright (c) 2009, Frank Warmerdam
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


/* projects.h and windows.h conflict - avoid this! */

#ifndef _WIN32
#include <projects.h>
PJ_CVSID("$Id: pj_transform.c 1504 2009-01-06 02:11:57Z warmerdam $");
#else
#include <proj_api.h>
#endif

#ifdef _WIN32
#  define MUTEX_win32
#endif

#if !defined(MUTEX_stub) && !defined(MUTEX_pthread) && !defined(MUTEX_win32)
#  define MUTEX_stub
#endif

static void pj_init_lock();

/************************************************************************/
/* ==================================================================== */
/*                      stub mutex implementation                       */
/* ==================================================================== */
/************************************************************************/

#ifdef MUTEX_stub

/************************************************************************/
/*                            pj_acquire_lock()                         */
/*                                                                      */
/*      Acquire the PROJ.4 lock.                                        */
/************************************************************************/

void pj_acquire_lock()
{
}

/************************************************************************/
/*                            pj_release_lock()                         */
/*                                                                      */
/*      Release the PROJ.4 lock.                                        */
/************************************************************************/

void pj_release_lock()
{
}

/************************************************************************/
/*                          pj_cleanup_lock()                           */
/************************************************************************/
void pj_cleanup_lock()
{
}

/************************************************************************/
/*                            pj_init_lock()                            */
/************************************************************************/

static void pj_init_lock()

{
}

#endif // def MUTEX_stub

/************************************************************************/
/* ==================================================================== */
/*                    pthread mutex implementation                      */
/* ==================================================================== */
/************************************************************************/

#ifdef MUTEX_pthread

#include "pthread.h"

static pthread_mutex_t core_lock = PTHREAD_MUTEX_INITIALIZER;

/************************************************************************/
/*                          pj_acquire_lock()                           */
/*                                                                      */
/*      Acquire the PROJ.4 lock.                                        */
/************************************************************************/

void pj_acquire_lock()
{
    pthread_mutex_lock( &core_lock);
}

/************************************************************************/
/*                          pj_release_lock()                           */
/*                                                                      */
/*      Release the PROJ.4 lock.                                        */
/************************************************************************/

void pj_release_lock()
{
    pthread_mutex_unlock( &core_lock );
}

/************************************************************************/
/*                          pj_cleanup_lock()                           */
/************************************************************************/
void pj_cleanup_lock()
{
}

/************************************************************************/
/*                            pj_init_lock()                            */
/************************************************************************/

static void pj_init_lock()

{
}

#endif // def MUTEX_pthread

/************************************************************************/
/* ==================================================================== */
/*                      win32 mutex implementation                      */
/* ==================================================================== */
/************************************************************************/

#ifdef MUTEX_win32

#include <windows.h>

static HANDLE mutex_lock = NULL;

/************************************************************************/
/*                          pj_acquire_lock()                           */
/*                                                                      */
/*      Acquire the PROJ.4 lock.                                        */
/************************************************************************/

void pj_acquire_lock()
{
    if( mutex_lock == NULL )
        pj_init_lock();

    WaitForSingleObject( mutex_lock, INFINITE );
}

/************************************************************************/
/*                          pj_release_lock()                           */
/*                                                                      */
/*      Release the PROJ.4 lock.                                        */
/************************************************************************/

void pj_release_lock()
{
    if( mutex_lock == NULL )
        pj_init_lock();

    ReleaseMutex( mutex_lock );
}

/************************************************************************/
/*                          pj_cleanup_lock()                           */
/************************************************************************/
void pj_cleanup_lock()
{
    if( mutex_lock != NULL )
    {
        CloseHandle( mutex_lock );
        mutex_lock = NULL;
    }
}

/************************************************************************/
/*                            pj_init_lock()                            */
/************************************************************************/

static void pj_init_lock()

{
    if( mutex_lock == NULL )
        mutex_lock = CreateMutex( NULL, TRUE, NULL );
}

#endif // def MUTEX_win32

