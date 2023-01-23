/******************************************************************************
 *
 * Project:  PROJ
 * Purpose:  Test behavior of database access across fork()
 * Author:   Even Rouault <even dot rouault at spatialys dot com>
 *
 ******************************************************************************
 * Copyright (c) 2021, Even Rouault <even dot rouault at spatialys dot com>
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
 ****************************************************************************/

#if defined(_WIN32) && defined(PROJ_HAS_PTHREADS)
#undef PROJ_HAS_PTHREADS
#endif

#ifdef PROJ_HAS_PTHREADS

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "proj.h"

int main() {
    PJ_CONTEXT *ctxt = proj_context_create();
    PJ_CONTEXT *ctxt2 = proj_context_create();

    /* Cause database initialization */
    {
        PJ *P = proj_create(ctxt, "EPSG:4326");
        if (P == NULL) {
            proj_context_destroy(ctxt);
            return 1;
        }
        proj_destroy(P);
    }
    {
        PJ *P = proj_create(ctxt2, "EPSG:4326");
        if (P == NULL) {
            proj_context_destroy(ctxt);
            proj_context_destroy(ctxt2);
            return 1;
        }
        proj_destroy(P);
    }

    for (int iters = 0; iters < 100; ++iters) {
        pid_t children[4];
        for (int i = 0; i < 4; i++) {
            children[i] = fork();
            if (children[i] < 0) {
                fprintf(stderr, "fork() failed\n");
                return 1;
            }
            if (children[i] == 0) {
                {
                    PJ *P = proj_create(ctxt, "EPSG:3067");
                    if (P == NULL)
                        _exit(1);
                    proj_destroy(P);
                }
                {
                    PJ *P = proj_create(ctxt2, "EPSG:32631");
                    if (P == NULL)
                        _exit(1);
                    proj_destroy(P);
                }
                _exit(0);
            }
        }
        for (int i = 0; i < 4; i++) {
            int status = 0;
            waitpid(children[i], &status, 0);
            if (status != 0) {
                fprintf(stderr, "Error in child\n");
                return 1;
            }
        }
    }

    proj_context_destroy(ctxt);
    proj_context_destroy(ctxt2);

    return 0;
}

#else

int main() { return 0; }

#endif
