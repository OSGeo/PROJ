/******************************************************************************
 *
 * Project:  PROJ
 * Purpose:  Test
 * Author:   Even Rouault <even dot rouault at spatialys dot com>
 *
 ******************************************************************************
 * Copyright (c) 2014, Even Rouault <even dot rouault at spatialys dot com>
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

#include "proj_internal.h"
#include <stdio.h> /* for printf declaration */


#ifdef _WIN32

int main(int argc, char* argv[])
{
    printf("Test not yet ported on Win32\n");
    return 0;
}

#else

#include <pthread.h>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>

static volatile int run = 0;
static volatile int started = 0;

static void* thread_main(void* unused)
{
    PJ_CONTEXT *p_proj_ctxt;
    PJ *p_WGS84_proj;
    PJ * p_OSGB36_proj;
    (void)unused;

    __sync_add_and_fetch(&started, 1);
    while(run == 0);

    p_proj_ctxt=pj_ctx_alloc();
    p_WGS84_proj=pj_init_plus_ctx(p_proj_ctxt,"+proj=longlat "
            "+ellps=WGS84 +datum=WGS84");
    p_OSGB36_proj=pj_init_plus_ctx(p_proj_ctxt,
            "+proj=longlat +ellps=airy +datum=OSGB36 +nadgrids=OSTN15_NTv2_OSGBtoETRS.gsb");

    while(run)
    {
        double x, y;
        int proj_ret;

        x = -5.2*DEG_TO_RAD;
        y = 50*DEG_TO_RAD;
        proj_ret = pj_transform(p_WGS84_proj,
                                p_OSGB36_proj, 1, 1, &x, &y, nullptr );
        x *= RAD_TO_DEG;
        y *= RAD_TO_DEG;
        /*printf("%.18f %.18f\n", x, y); */
        assert(proj_ret == 0);
        assert(fabs(x - -5.198965207267856492) < 1e-15);
        assert(fabs(y - 49.999396074140378232) < 1e-15);
    }

    pj_free (p_OSGB36_proj);
    pj_free (p_WGS84_proj);
    return nullptr;
}

int main()
{
    int i;

    pthread_t tid1, tid2;
    pthread_attr_t attr1, attr2;

    pthread_attr_init(&attr1);
    pthread_attr_init(&attr2);

    pthread_create(&tid1, &attr1, thread_main, nullptr);
    pthread_create(&tid2, &attr2, thread_main, nullptr);
    while(started != 2);
    run = 1;
    for(i=0;i<2;i++)
        sleep(1);
    run = 0;
    return 0;
}

#endif /* _WIN32 */
